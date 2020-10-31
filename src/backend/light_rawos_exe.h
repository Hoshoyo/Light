#pragma once
#include <common.h>
#include "x86/x86.h"

#define RAWX_ARCH_X86 0x1
#define RAWX_VERSION 0

typedef struct {
    u8  magic[4];        // RAWX
    u16 version;
    u32 flags;
    u32 load_address;
    u32 entry_point_offset;
    u32 stack_size;
    s32 section_count;
} RawOS_Header;

typedef struct {
    u8  name[8];            // .code .data .import
    u32 size_bytes;
    u32 virtual_address;    // offset from load_address, must be aligned to 0x1000
    u32 file_ptr_to_data;   // offset within the file to the section data
} RawOS_Section;

// Next in the file is the data for all the sections

// .code is just raw code

// .data is just raw data

// .import
typedef struct {
    u32 section_symbol_offset;  // offset from the beginning of the section where the symbol name is located
    u32 section_lib_offset;     // offset from the beginning of the section where the library name is located
    u32 call_address;           // address to the syscall to be filled by the loader
} RawOS_Import_Address;
typedef struct {
    u32 symbol_count;
    RawOS_Import_Address import_addresses[0];
} RawOS_Import_Table;

// Next the symbol names and library names can be located anywhere
// the only thing that needs to be done is fill the offsets in the import
// addresses properly.
// char* symbol_name;
// char* symbol_library;

void
light_rawos_emit(u8* in_stream, int in_stream_size_bytes, int entry_point_offset, 
    X86_Patch* rel_patches, X86_Data* data_seg, X86_Import* imports, 
    X86_DataSeg_Patch* dseg_patches, IR_Data_Segment_Entry* dseg_entries);