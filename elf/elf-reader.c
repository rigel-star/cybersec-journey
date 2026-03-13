#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

struct elf_hdr32 {
    uint32_t ident;
    uint8_t class;
    uint8_t data;
    uint8_t elf_ver;
    uint8_t os_abi;
    uint64_t abi_ver_padd; // 7 bytes padding
    uint16_t type;
    uint16_t mach;
    uint32_t version;
    uint32_t entry;
    uint32_t ph_off;
    uint32_t sh_off;
    uint32_t flags;
    uint16_t eh_size;
    uint16_t phentsize;
    uint16_t phnum;
    uint16_t shentsize;
    uint16_t shnum;
    uint16_t shstrndx;
} __attribute__((packed));

struct elf_hdr64 {
    uint32_t ident;
    uint8_t class;
    uint8_t data;
    uint8_t elf_ver;
    uint8_t os_abi;
    uint64_t abi_ver_padd; // 7 bytes padding
    uint16_t type;
    uint16_t mach;
    uint32_t version;
    uint64_t entry;
    uint64_t ph_off;
    uint64_t sh_off;
    uint32_t flags;
    uint16_t eh_size;
    uint16_t phentsize;
    uint16_t phnum;
    uint16_t shentsize;
    uint16_t shnum;
    uint16_t shstrndx;
} __attribute__((packed));

#define ELF64_HDR_SIZE sizeof(struct elf_hdr64)

int main(void) {
    FILE* fp = fopen("/bin/ls", "rb");
    struct elf_hdr64* buffer = malloc(ELF64_HDR_SIZE);

    int result = fread(buffer, ELF64_HDR_SIZE, 1, fp);
    if (result < 0) {
            fprintf(stderr, "Couldn't read file");
            exit(EXIT_FAILURE);
    }

    uint8_t magic[4];
    memcpy(magic, buffer, 4);

    printf("Magic: 0x%02x%02x%02x%02x (%s)\n", magic[0], magic[1], magic[2], magic[3], magic);
    printf("Class: %u (%d-bit)\n", buffer->class, buffer->class == 1 ? 32 : 64);
    printf("Data: %u (%s endian)\n", buffer->data, buffer->data == 1 ? "little" : "big");
    printf("Version: %u\n", buffer->elf_ver);
    printf("OS ABI: %u\n", buffer->os_abi);
    printf("ABI Version: %u\n", (uint8_t) (buffer->abi_ver_padd >> 56) & 0xFF);
    printf("Padding: 0x00000000000000\n");
    printf("Type: 0x%2x\n", buffer->type);
    printf("ISA: 0x%2x\n", buffer->mach);
    printf("Version: 0x1\n");
    printf("Entry Point: 0x%lx\n", buffer->entry);
    printf("Program Header Table: 0x%lx\n", buffer->ph_off);
    printf("Section Header Table: 0x%lx\n", buffer->sh_off);
    printf("Flags: 0x%x\n", buffer->flags);
    printf("Header Size: 0x%x\n", buffer->eh_size);
    printf("Program Header Table Entry Size: 0x%x\n", buffer->phentsize);
    printf("Program Header Table Items Count: 0x%x\n", buffer->phnum);
    printf("Section Header Table Entry Size: 0x%x\n", buffer->shentsize);
    printf("Section Header Table Items Count: 0x%x\n", buffer->shnum);
    printf("Section Header Table Entry: 0x%x\n", buffer->shstrndx);

    free(buffer);
    return 0;
}
