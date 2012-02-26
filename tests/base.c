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

#include "common.h"

static bool dest = false;

static void
destr(void *mem)
{
  dest = true;
}

int
main(int argc, const char **argv)
{
  myStruct *top, *tmp;

  /* Test standard parentage, allocation and destructor */
  assert(top = sc_new(NULL, myStruct));
  assert(sc_size_children(top) == 0);
  assert(tmp = sc_new0(top, myStruct));
  assert(sc_size_children(top) == 1);
  assert(sc_size_parents(tmp) == 1);
  sc_destructor_set(tmp, destr);
  assert(dest == false);
  sc_decref(top, tmp);
  assert(dest == true);
  assert(sc_size_children(top) == 0);

  /* Check aligned allocation */
  tmp = sc_memalign0(top, 4096, sizeof(myStruct), NULL);
  assert(tmp);
  assert(((uintptr_t) tmp) % 4096 == 0);
  assert(sc_size_children(top) == 1);
  assert(sc_size_parents(tmp) == 1);
  assert(_sc_resizea0((void**) &tmp, sizeof(myStruct), 3, 4096));
  assert(((uintptr_t) tmp) % 4096 == 0);
  assert(sc_size_children(top) == 1);
  assert(sc_size_parents(tmp) == 1);
  sc_decref(top, tmp);
  assert(sc_size_children(top) == 0);

  /* Test incref and decref */
  assert(sc_size_parents(top) == 1);
  sc_incref(NULL, top);
  assert(sc_size_parents(top) == 2);
  sc_decref(NULL, top);
  assert(sc_size_parents(top) == 1);

  /* Test steal */
  assert(tmp = sc_new(top, myStruct));
  assert(sc_size_children(top) == 1);
  assert(sc_size_parents(tmp) == 1);
  assert(sc_steal_old(NULL, tmp, top));
  assert(sc_size_children(top) == 0);
  assert(sc_size_parents(tmp) == 1);
  assert(sc_steal(top, tmp));
  assert(sc_size_children(top) == 1);
  assert(sc_size_parents(tmp) == 1);
  sc_decref(top, tmp);

  sc_decref(NULL, top);
  return 0;
}
