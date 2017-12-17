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

/**
 * PCRS_load
 *
 * Handles loading from a filename, and encapsulates the necessary
 * logic used to open a FILE* for reading, and passes information to
 * PCRS_parse if the FILE* is able to be opened.
 *
 * Returns the result of PCRS_parse() on the file if successful,
 * returning NULL if the file is unable to be loaded.
 */
struct PCRS_file* PCRS_load(char* filename) {
	FILE* tmp = fopen(filename, "rb");
	if(tmp == NULL) {
		int err = errno;
		char buff[PCRS_MAX_LOG_SIZE];
		snprintf(buff, PCRS_MAX_LOG_SIZE, "unable to load file %s", filename);
		_PCRS_error(buff);
		return NULL;
	}		

	return PCRS_parse(tmp);
}

/**
 * PCRS_prase
 *
 * Parses a FILE* and attempts to convert its data
 * into a PCRS_file structure.
 *
 * Returns a pointer to the new struct upon success,
 * and NULL on failure.
 */
struct PCRS_file* PCRS_parse(FILE* file) {
	struct PCRS_file* fin = NULL;

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

	fin = (struct PCRS_file*)malloc(sizeof(struct PCRS_file));

	uint8_t bcBuff[2];

	if(!_PCRS_readToBuffer(bcBuff, 2, file)) {
		PCRS_destroy(fin);
		return NULL;
	}	

	// Our parser has to work with the possibility that there's NULL
	// blocks about. Therefore, we'll use this count as guide as to
	// the _stated_ block count, not the actual count, which is
	// calculated in the loop below.
	uint16_t statedBlockCount = (bcBuff[0] + (bcBuff[1] << 8));

	uint8_t dirBuff[5]; // 1 byte for ID, 4 for offset

	uint8_t blockType;
	uint32_t blockOffset;

	for(uint16_t i =0;i < statedBlockCount;i++) {
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

/**
 * _PCRS_readToBuffer
 *
 * Internal function, reads an arbitrary number of bytes, num, into a buffer,
 * buff, supplied from a FILE*, file.
 * 
 * Returns a boolean indicating whether or not the read was successful
 */
bool _PCRS_readToBuffer(uint8_t* buff, size_t num, FILE* file) {
	if(fread(buff, 1, 2, file) != 2) {

		// If an fread() call fails, there's two possibilities:

		// a) We've reached the end of the file
		if(feof(file)) {
			_PCRS_error("pcrs file ended early");
			return false;
		}

		// b) We've encountered an error
		else if(ferror(file)) {
			_PCRS_error("PCRS_parse encountered an error reading the file");
			return false;
		}

		// If we're here, something weird is going on and we should abort.
		else {
			_PCRS_error("fatal: edge case, neither eof nor error"
						"but short read");
			exit(-1);
		}
	}

	return true;
}

/**
 * PCRS_destroy
 *
 * Destructor, makes sure to clear all reserved memory in a PCRS_file
 * structure. Since a PCRS_file structure is made up of a lot of pieces,
 * each piece is recursively checked and destructed if necessary.
 */
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
