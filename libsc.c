/*
 * libsc - Relational memory management
 *
 * Copyright 2011 Nathaniel McCallum <nathaniel@themccallums.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "libsc.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifndef UINT16_MAX
#define UINT16_MAX 65535
#endif

#define DEFAULT_LINK_SIZE 2

#define SC_FLAGS_TAG_ALLOCATED (1 << 0)

#define _GET_CHUNK(mem) \
  (mem >= sizeof(chunk) ? (mem - sizeof(chunk)) : 0)
#define GET_CHUNK(mem) \
  ((chunk*) _GET_CHUNK((uintptr_t) (mem)))
#define GET_ALLOC(chnk) \
  ((void*) (chnk ? chnk + 1 : NULL))
#define OR_MAX(n) \
  (n < UINT16_MAX ? n : UINT16_MAX)

typedef struct chunk chunk;
typedef struct link  link;

struct link {
  chunk  **chunks;
  uint16_t size;
  uint16_t used;
};

struct chunk {
  link    parents;
  link    children;
  chunk  *prev;
  chunk  *next;
  size_t  size;
  char   *name;
  scFree  destructor;
  uint8_t flags;
};

static bool
push(link* lnk, chunk *chnk)
{
  if (!lnk)
    return false;

  if (lnk->used == lnk->size) {
    size_t size = lnk->size > 0
                    ? OR_MAX(((size_t) lnk->size) * 2)
                    : DEFAULT_LINK_SIZE;
    /* Check to make sure we don't roll over our ref */
    if (size == lnk->size)
      return false;
    chunk **tmp = (chunk**) realloc(lnk->chunks, size * sizeof(chunk*));
    if (!tmp)
      return false;
    lnk->chunks = tmp;
    lnk->size = size;
  }

  lnk->chunks[lnk->used++] = chnk;
  return true;
}

static bool
pop(link *lnk, chunk *chnk)
{
  size_t i;
  if (!lnk || !lnk->chunks)
    return false;

  for (i = 0; i < lnk->used; i++) {
    if (lnk->chunks[i] == chnk) {
      lnk->chunks[i] = lnk->chunks[--lnk->used];
      return true;
    }
  }

  return false;
}

#define sib_loop(chnk, tmp, code) \
  for (chunk *step_, *tmp = chnk->prev; tmp; tmp = step_) { \
    step_ = tmp->prev; \
    code; \
  } \
  for (chunk *step_, *tmp = chnk; tmp; tmp = step_) { \
    step_ = tmp->next; \
    code; \
  }

static void
unlink(chunk *prnt, chunk *chld, bool bothsides)
{
  if (!chld)
    return;

  if (pop(&chld->parents, prnt) && prnt && bothsides)
    pop(&prnt->children, chld);

  size_t count = 0;
  sib_loop(chld, tmp, count += tmp->parents.used);
  if (count == 0) {
    /* First loop: call all destructors */
    sib_loop(chld, tmp,
      if (tmp->destructor)
        tmp->destructor(GET_ALLOC(tmp));
    );

    /* Second loop: remove the children, do the free */
    sib_loop(chld, tmp,
      for (size_t i=tmp->children.used; i > 0; i--)
        unlink(tmp, tmp->children.chunks[i-1], false);

      free(tmp->children.chunks);
      free(tmp->parents.chunks);
      free(tmp);
    );
  }
}

void *
_sc_calloc(void *parent, size_t size, size_t count,
           const char *tag, const char *location)
{
  chunk *chnk = NULL;
  void *tmp = NULL;

  chnk = (chunk*) malloc(sizeof(chunk) + (size * count));
  if (!chnk)
    return NULL;
  memset(chnk, 0, sizeof(chunk));

  chnk->size = size * count;
  tmp = sc_incref(parent, chnk ? chnk + 1 : NULL);

  if (!tmp)
    free(chnk);
  return tmp;
}

void *
_sc_calloc0(void *parent, size_t size, size_t count,
            const char *tag, const char *location)
{
  void *tmp = _sc_calloc(parent, size, count, tag, location);
  if (tmp)
    memset(tmp, 0, size * count);
  return tmp;
}

bool
_sc_resizea(void **mem, size_t size, size_t count)
{
  chunk *chnk = GET_CHUNK(mem ? *mem : NULL);
  if (!chnk)
    return false;

  chunk *tmp = (chunk*) realloc(chnk, sizeof(chunk) + (size * count));
  if (!tmp)
    return false;

  tmp->size = size * count;
  *mem = GET_ALLOC(tmp);
  return true;
}

bool
_sc_resizea0(void **mem, size_t size, size_t count)
{
  chunk *chnk = GET_CHUNK(mem ? *mem : NULL);
  if (!chnk)
    return false;
  size_t oldsize = chnk->size;

  if (!_sc_resizea(mem, size, count))
    return false;

  chnk = GET_CHUNK(mem ? *mem : NULL);
  if (!chnk)
    return false;
  if (size * count > oldsize)
    memset(((char *) *mem) + oldsize, 0, size * count - oldsize);
  return true;
}

void *
_sc_incref(void *parent, void *child, const char *location)
{
  chunk *chld, *prnt;

  chld = GET_CHUNK(child);
  prnt = GET_CHUNK(parent);
  if (!chld)
    return NULL;

  if (!push(&(chld->parents), prnt))
    return NULL;

  if (prnt && !push(&(prnt->children), chld)) {
    pop(&(chld->parents), prnt);
    return NULL;
  }

  return child;
}

void
_sc_decref(void *parent, void *child, const char *location)
{
  unlink(GET_CHUNK(parent), GET_CHUNK(child), true);
}

void *
_sc_steal(void *parent, void *child)
{
  chunk *chld = GET_CHUNK(child);
  if (!chld || chld->parents.used != 1)
    return NULL;

  if (chld->parents.chunks[0])
    pop(&chld->parents.chunks[0]->children, chld);
  chld->parents.chunks[0] = GET_CHUNK(parent);
  return child;
}

void
_sc_destructor_set(void *mem, scFree destructor)
{
  chunk *chnk = GET_CHUNK(mem);
  if (chnk)
    chnk->destructor = destructor;
}

void
sc_group(void *cousin, void *mem)
{
  chunk *chnk = GET_CHUNK(mem);
  chunk *csnc = GET_CHUNK(cousin);
  if (!chnk || !csnc)
    return;

  chunk *head = chnk;
  chunk *tail = chnk;
  while (head->prev)
    head = head->prev;
  while (tail->next)
    tail = tail->next;

  if (csnc->next)
    csnc->next->prev = tail;
  tail->next = csnc->next;
  csnc->next = head;
  head->prev = csnc;
}

size_t
sc_size(void *mem)
{
  chunk *chnk = GET_CHUNK(mem);
  if (chnk)
    return chnk->size;
  return 0;
}

bool
sc_tag_set(void *mem, const char *fmt, ...)
{
  va_list ap;
  chunk *chnk;
  char *tmp;

  chnk = GET_CHUNK(mem);
  if (!chnk || !fmt)
    return false;

  va_start(ap, fmt);
  tmp = sc_vasprintf(chnk, fmt, ap);
  va_end(ap);
  if (!tmp)
    return false;

  if (chnk->name && chnk->flags & SC_FLAGS_TAG_ALLOCATED)
    sc_decref(mem, chnk->name);

  chnk->name = tmp;
  chnk->flags |= SC_FLAGS_TAG_ALLOCATED;
  return true;
}

bool
sc_tag_set_const(void *mem, const char *tag)
{
  chunk *chnk = GET_CHUNK(mem);
  if (!chnk)
    return false;

  if (chnk->name && chnk->flags & SC_FLAGS_TAG_ALLOCATED)
    sc_decref(mem, chnk->name);

  chnk->name = (char*) tag;
  chnk->flags &= ~SC_FLAGS_TAG_ALLOCATED;
  return true;
}

const char *
sc_tag_get(void *mem)
{
  chunk *chnk = GET_CHUNK(mem);
  return chnk ? chnk->name : NULL;
}

char *
sc_strdup(void *parent, const char *str)
{
  if (!str)
    return NULL;
  return sc_strndup(parent, str, strlen(str));
}

char *
sc_strndup(void *parent, const char *str, size_t len)
{
  char *tmp;

  if (!str)
    return NULL;

  tmp = sc_newa(parent, char, len + 1);
  if (tmp) {
    strncpy(tmp, str, len);
    tmp[len] = '\0';
  }
  return tmp;
}

char *
sc_asprintf(void *parent, const char *fmt, ...)
{
  va_list ap;
  char *str;

  va_start(ap, fmt);
  str = sc_vasprintf(parent, fmt, ap);
  va_end(ap);
  return str;
}

char *
sc_vasprintf(void *parent, const char *fmt, va_list ap)
{
  va_list apc;
  ssize_t size = 0;
  char *str = NULL;

  va_copy(apc, ap);
  size = vsnprintf(NULL, 0, fmt, apc);
  va_end(apc);

  if (size <= 0 || !(str = sc_newa(parent, char, size)))
    return NULL;

  vsnprintf(str, size + 1, fmt, ap);
  return str;
}
