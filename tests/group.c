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
  myStruct *a, *b, *c;

  assert(a = sc_new0(NULL, myStruct));
  assert(sc_size_parents(a) == 1);

  assert(b = sc_new0(NULL, myStruct));
  assert(sc_size_parents(b) == 1);

  assert(c = sc_new0(NULL, myStruct));
  assert(sc_size_parents(c) == 1);

  sc_destructor_set(a, destr);
  assert(dest == false);

  sc_group(a, b);
  sc_group(b, c);

  sc_decref(NULL, a);
  assert(dest == false);

  sc_decref(NULL, b);
  assert(dest == false);

  sc_decref(NULL, c);
  assert(dest == true);

  return 0;
}
