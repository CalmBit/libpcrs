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

#ifndef LIBPCRS_LOGGING_H
#define LIBPRCS_LOGGING_H

#define PCRS_MAX_LOG_SIZE 4608
// #define PCRS_STOP_LOGGING /* uncomment if you don't want libpcrs to log */

extern void _PCRS_info(const char*);
extern void _PCRS_error(const char*);

#endif
