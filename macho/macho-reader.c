#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MACHO_HEADER_SIZE 	32
#define UNIVERSAL_BIN 		0xCAFEBABE

typedef struct MachoHeader {
	uint32_t magic;
	uint32_t cpu_type;
	uint32_t cpu_subtype;
	uint32_t file_type;
	uint32_t ncmds;
	uint32_t sizecmds;
	uint32_t flags;
	uint32_t reserved;
} __attribute__((packed)) MachoHeader_t;

typedef struct MachoUniversalHeader {
	uint32_t magic; // magic number 0xCAFEBABE
	uint32_t nbins; // number of binaries
	uint32_t cpu_type;
	uint32_t cpu_subtype;
	uint32_t file_off;
	uint32_t size;
	uint32_t sec_align;
} MachoUniversalHeader_t;

int main(void) {
	FILE* macho_file = fopen("/bin/ls", "rb");
	if (macho_file == NULL) {
		fprintf(stderr, "Failed to open file!");
		return -1;
	}

	uint8_t* content = malloc(sizeof(MachoHeader_t));
	
	if(fread(content, 1, MACHO_HEADER_SIZE, macho_file) < (size_t)MACHO_HEADER_SIZE) {
		fprintf(stderr, "Failed to read content!");
		free(content);
		fclose(macho_file);
		return -2;
	}

	// Manual reconstruction of magic to check type
	uint32_t magic = (content[0] << 24 | content[1] << 16 | content[2] << 8 | content[3]);

	if (magic != UNIVERSAL_BIN) {
		MachoHeader_t* header = (MachoHeader_t*) content;

		puts("Mach header:");
		printf("magic: %x\n", ntohl(header->magic));
		printf("cputype: %x\n", ntohl(header->cpu_type));
		printf("cpusubtype: %x\n", ntohl(header->cpu_subtype));
		printf("filetype: %x\n", ntohl(header->file_type));
		printf("ncmds: %x\n", ntohl(header->ncmds));
		printf("sizecmds: %x\n", ntohl(header->sizecmds));
		printf("flags: %x\n", ntohl(header->flags));
	}
	else {
		puts("Universal Binary:");
		MachoUniversalHeader_t* header = (MachoUniversalHeader_t*) content;

		printf("magic: %x\n", ntohl(header->magic));
		printf("nbins: %x\n", ntohl(header->nbins));
		printf("cputype: %x\n", ntohl(header->cpu_type));
		printf("cpusubtype: %x\n", ntohl(header->cpu_subtype));
		printf("fileoff: %x\n", ntohl(header->file_off));
		printf("size: %x\n", ntohl(header->size));
		printf("secalign: %x\n", ntohl(header->sec_align));
	}

	free(content);
	fclose(macho_file);
	return 0;
}