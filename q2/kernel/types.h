typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char uchar;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long uint64;

typedef uint64 pde_t;

struct meminfo {
  uint64 total_pages;
  uint64 free_pages;
  uint64 process_pages;
  uint64 page_table_pages;
};
