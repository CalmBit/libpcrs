/** 
 * This file is part of libpcrs.
 *
 * libpcrs is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libpcrs is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libpcrs.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "logging.h"
#include <stdio.h>

void _PCRS_error(const char* msg) {
	fprintf("libcrss: ", stderr);
	fprintf(msg, stderr);	
	fprintf("\n", stderr);
}
