#include <stdint.h>

#define LC_SEGMENT_64 		0x19
#define LC_LINK_LIBRARY_64 	0x0C

#define LC_LINK_LIBRARY_STR_OFF		0x18 /* Always 24 */

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

typedef struct LinkLibrary {
	uint32_t	cmd;
	uint32_t 	cmdsize;
	uint32_t	str_off; /* Always 24 */
	uint32_t	td;
	uint32_t	ver;
	uint32_t	compat_ver;
	uint32_t	file_path;
} LinkLibrary_t;