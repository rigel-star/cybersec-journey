#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "load_command.h"

#define FAT_MAGIC 			0xCAFEBABE
#define FAT_CIGAM      		0xBEBAFECA
#define MH_MAGIC       		0xFEEDFACE
#define MH_CIGAM       		0xCEFAEDFE
#define MH_MAGIC_64    		0xFEEDFACF
#define MH_CIGAM_64    		0xCFFAEDFE

typedef struct FatHeader {
	uint32_t magic;
	uint32_t nfat_arch;
} FatHeader_t;

typedef struct FatArch {
    uint32_t cputype;
    uint32_t cpusubtype;
    uint32_t offset;
    uint32_t size;
    uint32_t align;
} FatArch_t;

typedef struct MachHeader32 {
    uint32_t magic;
    uint32_t cputype;
    uint32_t cpusubtype;
    uint32_t filetype;
    uint32_t ncmds;
    uint32_t sizeofcmds;
    uint32_t flags;
} MachHeader32_t;

typedef struct MachHeader64 {
    uint32_t magic;
    uint32_t cputype;
    uint32_t cpusubtype;
    uint32_t filetype;
    uint32_t ncmds;
    uint32_t sizeofcmds;
    uint32_t flags;
    uint32_t reserved;
} MachHeader64_t;

static uint32_t bswap32(uint32_t x) {
    return ((x & 0xff000000) >> 24) |
           ((x & 0x00ff0000) >> 8)  |
           ((x & 0x0000ff00) << 8)  |
           ((x & 0x000000ff) << 24);
}

static uint32_t maybe_swap(uint32_t v, int swapped) {
    return swapped ? bswap32(v) : v;
}

static void read_load_commands(FILE *f, uint32_t ncmds, int swapped, int is64) {
    for (uint32_t i = 0; i < ncmds; i++) {
        long cmd_start = ftell(f);

        LoadCommand_t lc;
        fread(&lc, sizeof(lc), 1, f);

        uint32_t cmd     = maybe_swap(lc.cmd, swapped);
        uint32_t cmdsize = maybe_swap(lc.cmdsize, swapped);

        printf("  LoadCmd %u: cmd=0x%x size=%u\n", i, cmd, cmdsize);

        if (is64 && cmd == LC_SEGMENT_64) {
            fseek(f, cmd_start, SEEK_SET);

            SegmentCommand64_t seg;
            fread(&seg, sizeof(seg), 1, f);

            printf("    SEGMENT_64: %.16s\n", seg.segname);
            printf("      vmaddr:  0x%llx\n",
                   (unsigned long long)maybe_swap(seg.vmaddr, swapped));
            printf("      vmsize:  0x%llx\n",
                   (unsigned long long)maybe_swap(seg.vmsize, swapped));
            printf("      fileoff: %llu\n",
                   (unsigned long long)maybe_swap(seg.fileoff, swapped));
            printf("      filesize:%llu\n",
                   (unsigned long long)maybe_swap(seg.filesize, swapped));
        }

        fseek(f, cmd_start + cmdsize, SEEK_SET);
    }
}

static void read_macho_header(FILE* f) {
	uint32_t magic;
    fread(&magic, 4, 1, f);
    fseek(f, -4, SEEK_CUR);

    int swapped = 0;
    int is64 = 0;

	switch (magic) {
        case MH_MAGIC:      swapped = 0; is64 = 0; break;
        case MH_MAGIC_64:   swapped = 0; is64 = 1; break;
        case MH_CIGAM:      swapped = 1; is64 = 0; break;
        case MH_CIGAM_64:   swapped = 1; is64 = 1; break;
        default:
            printf("Not a Mach-O slice\n");
            return;
    }

	if (is64) {
        MachHeader64_t h;
        fread(&h, sizeof(h), 1, f);

        printf("Mach-O 64\n");
        printf("cputype:    0x%x\n", maybe_swap(h.cputype, swapped));
        printf("cpusubtype: 0x%x\n", maybe_swap(h.cpusubtype, swapped));
        printf("filetype:   0x%x\n", maybe_swap(h.filetype, swapped));
        printf("ncmds:      %u\n",   maybe_swap(h.ncmds, swapped));
        printf("sizeofcmds: %u\n",   maybe_swap(h.sizeofcmds, swapped));
        printf("flags:      0x%x\n", maybe_swap(h.flags, swapped));

		uint32_t ncmds = maybe_swap(h.ncmds, swapped);
		read_load_commands(f, ncmds, swapped, 1);
    } else {
        MachHeader32_t h;
        fread(&h, sizeof(h), 1, f);

        printf("Mach-O 32\n");
        printf("cputype:    0x%x\n", maybe_swap(h.cputype, swapped));
        printf("cpusubtype: 0x%x\n", maybe_swap(h.cpusubtype, swapped));
        printf("filetype:   0x%x\n", maybe_swap(h.filetype, swapped));
        printf("ncmds:      %u\n",   maybe_swap(h.ncmds, swapped));
        printf("sizeofcmds: %u\n",   maybe_swap(h.sizeofcmds, swapped));
        printf("flags:      0x%x\n", maybe_swap(h.flags, swapped));

		uint32_t ncmds = maybe_swap(h.ncmds, swapped);
		read_load_commands(f, ncmds, swapped, 0);
    }
}

static void handle_fat(FILE *f, uint32_t magic) {
    int swapped = (magic == FAT_CIGAM);

    FatHeader_t fh;
    fread(&fh, sizeof(fh), 1, f);

    uint32_t narch = maybe_swap(fh.nfat_arch, swapped);

    printf("FAT binary (%u architectures)\n", narch);

    FatArch_t *archs = malloc(sizeof(FatArch_t) * narch);
    fread(archs, sizeof(FatArch_t), narch, f);

	for (uint32_t i = 0; i < narch; i++) {
		uint32_t offset    = maybe_swap(archs[i].offset, swapped);
		uint32_t size      = maybe_swap(archs[i].size, swapped);

		printf("\n== Architecture %u ==\n", i);
		printf("offset:     %u\n", offset);
		printf("size:       %u\n", size);

		long saved = ftell(f);
		fseek(f, offset, SEEK_SET);

		read_macho_header(f);

		fseek(f, saved, SEEK_SET);
	}

    free(archs);
}

int main(int argc, char** argv) {
	if (argc < 2) {
		printf("Usage: ./main [mach-o file's path]\n");
		return 0;
	}

	char* bin_path = argv[1];
	printf("=== Reading %s ===\n\n", bin_path);

    FILE *f = fopen(bin_path, "rb");
    if (!f) {
        perror("open");
        return 1;
    }

    uint32_t magic;
    fread(&magic, 4, 1, f);
    fseek(f, 0, SEEK_SET);

    if (magic == FAT_MAGIC || magic == FAT_CIGAM) {
		/*
		 As per Wikipedia(https://en.wikipedia.org/wiki/Mach-O#Multi-architecture_binaries):
		 The magic number in a multi-architecture binary is 0xcafebabe in big-endian byte order, 
		 so the first four bytes of the header will always be 0xca 0xfe 0xba 0xbe, in that order.
		*/
        handle_fat(f, magic);
    } 
	else {
        read_macho_header(f);
    }

    fclose(f);
    return 0;
}
