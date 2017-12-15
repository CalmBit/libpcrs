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

#include "libpcrs.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "logging.h"

static bool _PCRS_readToBuffer(uint8_t* buff, size_t num, FILE* file);

struct PCRS_file* PCRS_load(char* filename) {
	FILE* tmp = fopen(filename, "rb");
	if(tmp == NULL) {
		int err = errno;
		char buff[MAX_LOG_SIZE];
		snprintf(buff, MAX_LOG_SIZE, "unable to load file %s", filename);
		_PCRS_error(buff);
		return NULL;
	}		

	return PCRS_parse(tmp);
}

struct PCRS_file* PCRS_parse(FILE* file) {
	PCRS_file* fin = NULL;

	if(file == NULL) {
		_PCRS_error("PCRS_parse recieved null file pointer");
		return NULL;
	}

	char mgcnum[LIBPCRS_MGCNM_SIZE];

	fgets(mgcnum, LIBPCRS_MGCNM_SIZE, file);

	if(strcmp(mgcnum, LIBPCRS_MAGIC)) {
		_PCRS_error("PCRS_parse recieved invalid file type");
		return NULL;
	}

	fin = (PCRS_file*)malloc(sizeof(PCRS_file));

	uint8_t bcBuff[2];

	if(!_PCRS_readToBuffer(bcBuff, 2, file)) {
		PCRS_destroy(fin);
		return NULL;
	}	

	// Our parser has to work with the possibility that there's NULL
	// blocks about.
	uint16_t statedBlockCount = (bcBuff[0] + (bcBuff[1] << 8));

	uint8_t dirBuff[5]; // 1 byte for ID, 4 for offset

	uint8_t blockType;
	uint32_t blockOffset;

	for(uint16_t i =0;i < fin->statedBlockCount;i++) {
		if(!_PCRS_readToBuffer(dirBuff, 5, file)) {
			PCRS_destroy(fin);
			return NULL;
		}	

		blockType = dirBuff[0];

		blockOffset = dirBuff[1] + (dirBuff[2] << 8) +
			(dirBuff[3] << 16) + (dirBuff[4] << 24);
	}

	return fin;
}


bool _PCRS_readToBuffer(uint8_t* buff, size_t num, FILE* file) {
	if(fread(buff, 1, 2, file) != 2) {
		if(feof(file)) {
			_PCRS_error("pcrs file ended early");
			return false;
		}
		else if(ferror(file)) {
			_PCRS_error("PCRS_parse encountered an error reading the file");
			return false;
		}
		else {
			_PCRS_error("fatal: edge case, neither eof nor error"
						"but short read");
			exit(-1);
		}
	}

	return true;
}

void PCRS_destroy(struct PCRS_file* file) {
	if(file == NULL)
		return;

	if(file->directory != NULL) {
		for(uint16_t i =0;i < file->directory->blockCount;i++) {
			if(file->directory->blocks[i] == NULL)
				continue;

			free(file->directory->blocks[i]);
		}
		free(file->directory);
	}

	free(file);
}
