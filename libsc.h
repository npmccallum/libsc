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

#ifndef LIBSC_H_
#define LIBSC_H_
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define __str__(s) #s
#define __str(s) __str__(s)

#ifndef __location__
#define __loc__ __FILE__ ":" __str(__LINE__)
#endif

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


typedef void (*scFree)(void *);
typedef bool (*scForEach)(void *parent, void *child, void *data);

#define sc_new(p, t)            ((t*) sc_calloc(p, sizeof(t), 1, __str(t)))
#define sc_new0(p, t)           ((t*) sc_calloc0(p, sizeof(t), 1, __str(t)))
#define sc_malloc(p, s, n)      sc_calloc(p, s, 1, n)
#define sc_malloc0(p, s, n)     sc_calloc0(p, s, 1, n)
#define sc_newa(p, t, c)        ((t*) sc_calloc(p, sizeof(t), c, __str(t)))
#define sc_newa0(p, t, c)       ((t*) sc_calloc0(p, sizeof(t), c, __str(t)))
#define sc_calloc(p, s, c, n)   _sc_calloc(p, s, c, n, __loc__)
#define sc_calloc0(p, s, c, n)  _sc_calloc0(p, s, c, n, __loc__)
#define sc_resizea(m, c)        _sc_resizea(m, sizeof(__typeof__(**m)), c)
#define sc_resizea0(m, c)       _sc_resizea0(m, sizeof(__typeof__(**m)), c)
#define sc_incref(p, m)         ((__typeof__(m)) _sc_incref(p, m, __loc__))
#define sc_decref(p, m)         _sc_decref(p, m, __loc__);
#define sc_size_item(m)         sizeof(__typeof__(*m))
#define sc_size_items(m)        (sc_size(m) / sc_size_item(m))
#define sc_steal(p, m)          ((__typeof__(m)) _sc_steal(p, m))
#define sc_destructor_set(m, d) _sc_destructor_set(m, (memFree) d)

void *
_sc_calloc(void *parent, size_t size, size_t count,
           const char *tag, const char *location);

void *
_sc_calloc0(void *parent, size_t size, size_t count,
            const char *tag, const char *location);

bool
_sc_resizea(void **mem, size_t size, size_t count);

bool
_sc_resizea0(void **mem, size_t size, size_t count);

void *
_sc_incref(void *parent, void *child, const char *location);

void
_sc_decref(void *parent, void *child, const char *location);

void *
_sc_steal(void *parent, void *child);

void
_sc_destructor_set(void *mem, scFree destructor);

void
sc_group(void *cousin, void *mem);

size_t
sc_size(void *mem);

bool
sc_tag_set(void *mem, const char *fmt, ...);

bool
sc_tag_set_const(void *mem, const char *tag);

const char *
sc_tag_get(void *mem);

char *
sc_strdup(void *parent, const char *str);

char *
sc_strndup(void *parent, const char *str, size_t len);

char *
sc_asprintf(void *parent, const char *fmt, ...);

char *
sc_vasprintf(void *parent, const char *fmt, va_list ap);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
#endif /* LIBSC_H_ */
