#include <stdint.h>

#define LC_SEGMENT_64 0x19

typedef struct LoadCommand {
    uint32_t 	cmd;     // Type of load command
    uint32_t 	cmdsize; // Total size of this command
} LoadCommand_t;

typedef struct SegmentCommand64 {
    uint32_t 	cmd;
    uint32_t 	cmdsize;
    char     	segname[16];
    uint64_t    vmaddr;
    uint64_t    vmsize;
    uint64_t    fileoff;
    uint64_t    filesize;
    uint32_t    maxprot;
    uint32_t    initprot;
    uint32_t    nsects;
    uint32_t    flags;
} SegmentCommand64_t;