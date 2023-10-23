#ifndef HDC_TYPES_H
#define HDC_TYPES_H

#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>

#define HDC_MAX_DIMS           10
#define HDC_LIST_INIT_CAPACITY 10
#define HDC_LIST_RESIZE_STEP   10
#define HDC_UUID_LENGTH        16

typedef enum {
    HDC_EMPTY  =  0,
    HDC_STRUCT =  1,
    HDC_LIST   =  2,
    HDC_INT8   =  3,
    HDC_INT16  =  4,
    HDC_INT32  =  5,
    HDC_INT64  =  6,
    HDC_UINT8  =  7,
    HDC_UINT16 =  8,
    HDC_UINT32 =  9,
    HDC_UINT64 = 10,
    HDC_FLOAT  = 11,
    HDC_DOUBLE = 12,
    HDC_STRING = 13,
    HDC_BOOL   = 14,
    HDC_ERROR  = 15,
    HDC_UNDEF  = 16,
    HDC_COMPLEX = 17,
    Internal_ForceMyEnumIntSize = ULONG_MAX // enum : unsigned int is not compatible with C, so we will do this and hope that it will be ok.
} hdc_type_t;

#define HDCDefault             0lu
#define HDCFortranOrder        1lu
#define HDCReadOnly            2lu
#define HDCExternal            4lu
#define HDCChildrenInitialized 8lu

typedef size_t hdc_flags_t;

struct hdc_t {
    char        uuid[HDC_UUID_LENGTH];
    size_t      storage_id;
};

typedef struct hdc_t hdc_t;

struct hdc_header_t {
    size_t      buffer_size;
    size_t      data_size;
    hdc_type_t  type;
    hdc_flags_t flags;
    size_t      shape[HDC_MAX_DIMS];
    size_t      rank;
//     hdc_t       parent;
    size_t      pad; // Padding
};
//} __attribute__((packed));

#define HDC_NODE_SIZE_DEFAULT   4096 // we can try less here 3072 works, 2048 does not
#define HDC_MAX_RESIZE_ATTEMPTS    5 // how many times to try increasing buffer

//TODO Specify offsets in header, possibly make header of dynamic length.

struct hdc_data_t {
    hdc_type_t  type;
    hdc_flags_t flags;
    size_t      rank;
    size_t      shape[HDC_MAX_DIMS];
    char*       data;
};

#endif
