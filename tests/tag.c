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
#include "string.h"

int
main(int argc, const char **argv)
{
  myStruct *top, *tmp;

  assert(top = sc_new(NULL, myStruct));
  assert(sc_ensure(top, myStruct));
  assert(!strcmp(sc_tag_get(top), "myStruct"));
  assert(sc_tag_set_const(top, "foo"));
  assert(!strcmp(sc_tag_get(top), "foo"));
  assert(sc_tag_set(top, "foo %s", "bar"));
  assert(!strcmp(sc_tag_get(top), "foo bar"));
  sc_decref(NULL, top);

  return 0;
}
