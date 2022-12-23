#include "bg.h"
#include "time.h"
#include "string.h"
#include "stdlib.h"
#include "nanorc.h"
#include "stdbool.h"

#define __USE_GNU
#include <stdio.h>

#define farbeld_magic  "farbfeld"
#define WIDTH_OFFSET   strlen(farbeld_magic)
#define HEIGHT_OFFSET  WIDTH_OFFSET + sizeof(uint32_t)
#define PICTURE_OFFSET HEIGHT_OFFSET + sizeof(uint32_t)

#include "pictures.h"

/*
 * Read a 4 byte big endian number
 */
static int32_t read_big_endian(const uint8_t* data) {
	int32_t ret = 0;
	for (int i=0; i<4; i++) {
		ret = ret << 8;
		ret |= (0xFF & data[i]);
	}
	return ret;
}

/*
 * Write a 4 byte big endian number
 */
static void write_big_endian(uint8_t* data, int32_t number) {
	for (int i=0; i<4; i++) {
		uint8_t byte = number >> (((3-i) * 8) & 0xFF);
		data[i] = byte;
	}
}

/*
 * Generate an empty farbfeld image of the desire size.
 */
static uint8_t* farbfeld_init(uint32_t width, uint32_t height) {
	uint8_t* ret = malloc(strlen(farbeld_magic) + (2 * sizeof(uint32_t)) + (height * width * sizeof(uint64_t)));
	strcpy((char*) ret, farbeld_magic);
	write_big_endian(ret + WIDTH_OFFSET, width);
	write_big_endian(ret + HEIGHT_OFFSET, height);
	return ret;
}

/*
 * Get the index of the desired background picture.
 * Choose it once randomly and then return always the same.
 */
static int get_bg_index(void) {
	static int ret = -1;
	if (ret == -1) {
		srand(time(NULL));
		ret = rand() % bg_len;
	}
	return ret;
}

/*
 * Scale a farbfeld picture by the desired factor.
 * Allocate the memory and return a new pointer
 * As the input picture will be anime character which have a lot of flat
 * colors, it makes sense to have no anti-aliasing in order to reduce
 * blur. As the pictures are very flat, there will be only a minimal
 * amount of high frequency that might cause aliasing.
 */
static uint8_t* farbfeld_scale(const uint8_t* pic, double scale) {
	uint32_t old_width = read_big_endian(pic + WIDTH_OFFSET);
	uint32_t old_height = read_big_endian(pic + HEIGHT_OFFSET);
	uint32_t new_width = old_width * scale;
	uint32_t new_height = old_height * scale;
	uint8_t* ret = farbfeld_init(new_width, new_height);
	for (uint32_t width=0; width<new_width; width++) {
		for (uint32_t height=0; height<new_height; height++) {
			uint32_t source_width = ((double) width) / scale;
			uint32_t source_height = ((double) height) / scale;
			uint64_t source_offset = source_height * old_width + source_width;
			uint64_t target_offset = height * new_width + width;
			((uint64_t*) (ret + PICTURE_OFFSET))[target_offset] = ((uint64_t*) (pic + PICTURE_OFFSET))[source_offset];
		}
	}
	return ret;
}

/*
 * Add padding to a picture in the desired axis.
 * The color of the padding is selected from the edge of the picture.
 * For horizontal padding, it is added on both sides equally. For vertical
 * padding, it is only added to the top.
 */
static uint8_t* farbfeld_pad(const uint8_t* pic, uint32_t pixels_to_add, bool expand_in_row) {
	uint32_t old_width = read_big_endian(pic + WIDTH_OFFSET);
	uint32_t old_height = read_big_endian(pic + HEIGHT_OFFSET);
	uint32_t new_width = old_width + (expand_in_row ? pixels_to_add : 0);
	uint32_t new_height = old_height + (expand_in_row ? 0 : pixels_to_add);
	uint8_t* ret = farbfeld_init(new_width, new_height);

	for (uint32_t width=0; width<new_width; width++) {
		for (uint32_t height=0; height<new_height; height++) {
			uint64_t target_offset = height * new_width + width;
			uint32_t source_width = (expand_in_row ?
									  (width < pixels_to_add / 2 ? 0
									    : (width > old_width + pixels_to_add / 2 - 1 ? old_width - 1 : width - pixels_to_add / 2)) : width);

			uint32_t source_height = (expand_in_row ? height :
					                   (height < pixels_to_add ? 0 :
										 (height - pixels_to_add)));
			uint64_t source_offset = source_height * old_width + source_width;

			((uint64_t*) (ret + PICTURE_OFFSET))[target_offset] = ((uint64_t*) (pic + PICTURE_OFFSET))[source_offset];
		}
	}

	return ret;
}

/*
 * Resize a farbfeld picture to the required dimentions.
 * Firstly, the picture is scaled so that one of its dimentions match
 * the requirement and that it fully fits inside of the requirements.
 * Then, it is padded to fit.
 */
static uint8_t* farbfeld_resize(const uint8_t* pic, uint32_t width, uint32_t height) {
	uint32_t old_width = read_big_endian(pic + WIDTH_OFFSET);
	uint32_t old_height = read_big_endian(pic + HEIGHT_OFFSET);
	double scaling_factor = (double) width / (double) old_width; // Try to use the width as scaling reference
	if (old_height * scaling_factor > height) {
		scaling_factor = (double) height / (double) old_height; // If not working well, use the height as reference
	}
	uint8_t* scaled = farbfeld_scale(pic, scaling_factor);

	uint32_t intermediate_width = read_big_endian(scaled + WIDTH_OFFSET);
	uint32_t intermediate_height = read_big_endian(scaled + HEIGHT_OFFSET);
	uint32_t size_diff;
	bool expand_in_row;
	if (abs((int32_t) intermediate_width - (int32_t) width) > abs((int32_t)intermediate_height - (int32_t)height)) {
		size_diff = width - intermediate_width;
		expand_in_row = true;
	} else {
		size_diff = height - intermediate_height;
		expand_in_row = false;
	}
	uint8_t* ret = farbfeld_pad(scaled, size_diff, expand_in_row);

	free(scaled);
	return ret;
}

/*
 * Return a background with the correct dimentions.
 */
uint8_t* picture_get_bg(uint32_t width, uint32_t height) {
	const uint8_t* origin = bg[get_bg_index()];

	uint8_t* ret = farbfeld_resize(origin, width, height); // TODO Fix memory leak
	return ret;
}

/*
 * Return the foreground color of the theme
 */
#define stfg_beacon "stfg="
unsigned int picture_get_fg(void) {
	const char* theme = (const char*) nanorc[get_bg_index()];
	for (size_t i=0; i<strlen(theme)-strlen(stfg_beacon); i++) {
		if (!memcmp(stfg_beacon, theme+i, strlen(stfg_beacon))) {
			for (size_t j=i+strlen(stfg_beacon); j<strlen(theme); j++) {
				if (theme[j] < '0' || theme[j] > '9') {
					char buff[j-i];
					memset(buff, 0, j-i);
					memcpy(buff, theme+i+strlen(stfg_beacon), j-i-strlen(stfg_beacon));
					return atoi(buff);	
				}
			}
			break;
		}
	}
	return ~0;
}

/*
 * Return the nano theme of the current background as a FILE*
 */
FILE* picture_get_nano_theme(void) {
	ssize_t theme_read(void* _cookie, char* buf, size_t size) {
		size_t* cookie = (size_t*) _cookie;
		const char* theme = (const char*) nanorc[get_bg_index()];
		ssize_t ret = 0;
		FILE* f = fopen("log", "a");
		fprintf(f, "bgi=%i, len=%li, size=%li\n", get_bg_index(), strlen((const char*)nanorc[get_bg_index()]), size);
		for (size_t i=0; i<size; i++) {
			fprintf(f, "i=%li; ret=%li; cookie=%li\n", i, ret, *cookie);
			buf[i] = theme[i+ *cookie];
			if (theme[i+ *cookie] == 0) {
				break;
			}
			ret++;
		}
		*cookie = *cookie + ret;
		fprintf(f, "\n\n\n");
		fclose(f);
		return ret;
	}

	int theme_seek(void* _cookie, __off64_t* offset, int whence) {
		size_t* cookie = (size_t*) _cookie;
		switch (whence) {
			case SEEK_SET:
				*cookie = *offset;
				*offset = *cookie;
				return 0;
			case SEEK_CUR:
				*offset = *cookie;
				return 0;
			case SEEK_END:
				*cookie = strlen((const char*) nanorc[get_bg_index()]) + *offset;
				*offset = *cookie;
				return 0;
			default:
				return -1;
		}
	}

	int free_cookie(void* cookie) {
		free(cookie);
		return 0;
	}

	cookie_io_functions_t cf = {
		.read = theme_read,
		.write = NULL,
		.seek = theme_seek,
		.close = free_cookie,
	};

	size_t* cookie = malloc(sizeof(size_t));
	*cookie = 0;	
	return fopencookie((void*) cookie, "r", cf);
}

#ifdef TEST_PICTURES_C
#include "stdio.h"


static void farbfeld_write(const uint8_t* pic, const char* filename) {
	FILE* f = fopen(filename, "w");
	if (f == NULL) {
		fprintf(stderr, "Unable to open %s.\n", filename);
		return;
	}

	uint32_t width = read_big_endian(pic + WIDTH_OFFSET);
	uint32_t height = read_big_endian(pic + HEIGHT_OFFSET);
	size_t total_size = PICTURE_OFFSET + (width * height * sizeof(uint64_t));
	for (size_t i=0; i<total_size; i++) {
		fputc(pic[i], f);
	}
	fclose(f);
}

static uint8_t* read_bin_file(const char* filename) {
	FILE* f = fopen(filename, "r");
	if (f == NULL) {
		fprintf(stderr, "Unable to open %s.\n", filename);
		return NULL;
	}

	fseek(f, 0L, SEEK_END);
	size_t size = ftell(f);
	rewind(f);

	uint8_t* ret = malloc(size);
	for (size_t i=0; i<size; i++) {
		ret[i] = fgetc(f);
	}
	fclose(f);
	return ret;
}

int main(void) {
	uint8_t* origin = read_bin_file("./pictures/mire.ff");
	uint8_t* sized = farbfeld_resize(origin, 300, 300);
	farbfeld_write(sized, "test-pictures.ff");
	free(sized);
	free(origin);
	return 0;
}
#endif

