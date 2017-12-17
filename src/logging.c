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

enum _PCRS_loglevel {
	TRACE,
	DEBUG,
	INFO,
	WARNING,
	ERROR,
	FATAL
};

const char* _PCRS_levelStrings[] = {
	"TRACE",
	"DEBUG",
	"INFO",
	"WARNING",
	"ERROR",
	"FATAL"
};

static void _PCRS_log(enum _PCRS_loglevel logLevel, 
					  const char* msg, 
					  FILE* file) {
#ifndef PCRS_STOP_LOGGING
	fprintf(file, "libpcrss: [");
	fprintf(file, _PCRS_levelStrings[logLevel]);
	fprintf(file, "] ");
	fprintf(file, msg) ;
#endif
}

void _PCRS_info(const char* msg) {
	_PCRS_log(INFO, msg, stdout);
}

void _PCRS_error(const char* msg) {
	_PCRS_log(ERROR, msg, stderr);
}	

