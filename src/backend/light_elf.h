#pragma once
#include <common.h>

typedef struct {
    int name;       // An offset to a string in the .shstrtab section that represents the name of this section.
    int type;       // type (Example: SHT_NULL)
    int flags;      // attributes (Example: SHF_WRITE)
    int addr;       // Virtual address of the section in memory, for sections that are loaded.
    int offset;     // Offset of the section in the file image
    int size;       // Size in bytes of the section in the file image. May be 0.
    int link;       // Contains the section index of an associated section. This field is used for several purposes, depending on the type of section.
    int info;       // Contains extra information about the section. This field is used for several purposes, depending on the type of section.
    int addralign;  // Contains the required alignment of the section. This field must be a power of two.
    int endsize;    // Contains the size, in bytes, of each entry, for sections that contain fixed-size entries. Otherwise, this field contains zero.
} ELF32_Section_Header;

typedef struct {
    int type;
    int offset;     // Offset of the segment in the file image.
    int vaddr;      // Virtual address of the segment in memory.
    int paddr;      // On systems where physical address is relevant, reserved for segment's physical address.
    int filesz;     // Size in bytes of the segment in the file image. May be 0.
    int memsz;      // Size in bytes of the segment in memory. May be 0.
    int flags;      // Segment-dependent flags (position for 64-bit structure).
    int align;      // 0 and 1 specify no alignment. Otherwise should be a positive, integral power of 2, with p_vaddr equating p_offset modulus p_align.
} ELF32_Program_Header;

typedef struct {
    char  magic[4];      // must be 0x7f 0x45 0x4c 0x46
    char  class;         // 1 -> 32bit, 2 -> 64bit
    char  data;          // 1 -> little endian, 2 -> big endian
    char  ei_version;    // 1 -> current elf version
    char  os_abi;        // 0x3 for linux
    char  abi_version;
    char  padd0[7];      // unused, should be 0
    short obj_ftype;     // ET_EXEC (2) for executable
    short machine;       // 0x3 for x86, 0x3e for amd64
    int   e_version;     // 1
    int   e_entry;       // memory address of entry point
    int   pheader_off;   // offset to the program header
    int   sheader_off;   // offset to the section header
    int   flags;
    short ehsize;        // size of this header 52
    short phsize;        // size of the program header
    short phnum;         // number of entries in the program header
    short e_shentsize;   // Contains the size of a section h1eader table entry.
    short e_shnum;       // Contains the number of entries in the section header table.
    short e_shstrndx;    // Contains index of the section header table entry that contains the section names.
} ELF32_Header;

void light_elf_emit(u8* in_stream, int in_stream_size_bytes);