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

#ifndef LIBPCRS_H
#define LIBPCRS_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "stddefs.h"

// TODO - Decide where the type specifier will be - beginning of the block
// or in the directory

enum PCRS_blocktype {
	NUL = 0x00, // NULL block
	DAT = 0x01, // DATA block
	CDT = 0x02, // CDAT block
	MTA = 0x03, // META block
	GRP = 0x04, // GRUP BLOCk
};

/**
 * PCRS_DATA
 *
 * Stores the data encapsulated by a DATA block, which includes the vertical
 * and horizontal size of the board specified by the block, along with the
 * fully decompressed grid of values for the board.
 */
struct PCRS_DATA {
	uint8_t sizeX, sizeY;
	bool* grid;
};

/**
 * PCRS_direntry
 *
 * Represents a single entry in the PCRS directory, specifying the type of
 * block pointed to, along with an offset value to the location of the block's
 * first byte.
 */
struct PCRS_direntry {
	enum PCRS_blocktype blockType;
	uint32_t offset;
};

struct PCRS_directory {
	uint16_t blockCount;
	struct PCRS_direntry* blockList;
	void** blocks;
};

struct PCRS_file {
	struct PCRS_directory* directory;
	char* name;
	/* other metadata awaited */
};

struct PCRS_file* PCRS_load(char* filename);
struct PCRS_file* PCRS_parse(FILE* file);
void PCRS_destroy(struct PCRS_file* file);

#endif
