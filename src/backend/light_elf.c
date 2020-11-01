#include "light_elf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <light_array.h>
#if defined(__linux__)
#include <elf.h>
#else
#define ET_EXEC 2
#define PT_LOAD 1
#define PF_X (1 << 0)
#define PF_R (1 << 2)
#define SHT_PROGBITS 1
#define SHT_STRTAB 3
#define SHF_ALLOC (1 << 1)
#define SHF_EXECINSTR (1 << 2)
#endif

static int align_delta(int offset, int align_to){
	int rest = offset % align_to;
	return((align_to - rest) % align_to);
}

static u8*
copy_stream(u8* dst, u8* src, int size)
{
    for(int i = 0; i < size; ++i)
    {
        *dst++ = src[i];
    }
    return dst;
}

static void
fill_relative_patches(int base_rva, int rva, X86_Patch* rel_patches)
{
    for(int i = 0; i < array_length(rel_patches); ++i)
    {
        if (rel_patches[i].generate_relocation)
        {
            int issuer_offset = rel_patches[i].issuer_offset + rel_patches[i].issuer_imm_offset;
            int offset = rel_patches[i].issuer_offset + (*(int*)(rel_patches[i].addr));
            *((int*)(rel_patches[i].addr)) = offset + base_rva + rva;
        }
    }
}

void
light_elf_emit(const char* output_filename, u8* in_stream, int in_stream_size_bytes, X86_Patch* rel_patches, X86_Import* imports)
{
    unsigned char* stream = (unsigned char*)calloc(1, 1024*1024);
    unsigned char* at = stream;

    int file_offset = 0;

    ELF32_Header elf_header = {0};
    elf_header.magic[0] = 0x7f;
    elf_header.magic[1] = 'E';
    elf_header.magic[2] = 'L';
    elf_header.magic[3] = 'F';
    elf_header.class = 1;
    elf_header.data = 1;
    elf_header.ei_version = 1;
    elf_header.os_abi = 0x3;
    elf_header.abi_version = 0;
    elf_header.obj_ftype = ET_EXEC;
    elf_header.machine = 0x3;
    elf_header.e_version = 1;
    elf_header.e_entry = 0; // filled after
    elf_header.pheader_off = sizeof(ELF32_Header);  // after elf header
    elf_header.sheader_off = 0; // TODO
    elf_header.flags = 0;
    elf_header.ehsize = sizeof(ELF32_Header);
    elf_header.phsize = sizeof(ELF32_Program_Header);
    elf_header.phnum = 1;
    elf_header.e_shentsize = sizeof(ELF32_Section_Header);
    elf_header.e_shnum = 3;
    elf_header.e_shstrndx = 2;
    memcpy(at, &elf_header, sizeof(elf_header));
    at += sizeof(elf_header);
    file_offset += sizeof(elf_header);

    int tpadding = align_delta(sizeof(ELF32_Header) + sizeof(ELF32_Program_Header), 16);

    // LOAD program header
    ELF32_Program_Header ph = {0};
    ph.type = PT_LOAD;
    ph.offset = 0;
    ph.vaddr = 0x08048000;
    ph.paddr = 0x08048000;
    ph.filesz = elf_header.phsize + sizeof(ELF32_Header) + tpadding;
    ph.memsz = ph.filesz;
    ph.flags = PF_X | PF_R;
    ph.align = 0x1000;
    memcpy(at, &ph, sizeof(ph));
    at += sizeof(ph);
    file_offset += sizeof(ph);

    // align to 16
    file_offset += tpadding;
    at += tpadding;
    elf_header.e_entry = 0x8048000 + file_offset;
    ((ELF32_Header*)stream)->e_entry = elf_header.e_entry;

    // emit code section .text
    int text_offset = file_offset;

    fill_relative_patches(ph.vaddr, file_offset, rel_patches);
    
    unsigned char* mem = at;
    mem = copy_stream(mem, in_stream, in_stream_size_bytes);

    file_offset += (mem-at);
    int text_size = mem-at;
    at = mem;

    // emit section .shstrtab
    int strtab_offset = file_offset;
    unsigned char* section_start = at;
    *mem++ = 0;
    mem += (sprintf((char*)mem, ".text") + 1);
    mem += (sprintf((char*)mem, ".shstrtab") + 1);
    file_offset += (mem - section_start);
    int strtab_size = (mem - section_start);
    at = mem;

    // SECTION TABLES

    int sh_offset = file_offset;
    ((ELF32_Header*)stream)->sheader_off = sh_offset;

    // emit null section header table
    ELF32_Section_Header null_sh = {0};
    memcpy(at, &null_sh, sizeof(null_sh));
    file_offset += sizeof(null_sh);
    at += sizeof(null_sh);

    // emit .text section header table entry
    ELF32_Section_Header text_sh = {0};
    text_sh.type = SHT_PROGBITS;
    text_sh.addr = elf_header.e_entry;
    text_sh.offset = text_offset;
    text_sh.size = text_size;
    text_sh.flags = SHF_ALLOC|SHF_EXECINSTR;
    text_sh.link = 0;
    text_sh.info = 0;
    text_sh.addralign = 16;
    text_sh.endsize = 0;
    memcpy(at, &text_sh, sizeof(text_sh));
    file_offset += sizeof(text_sh);
    at += sizeof(text_sh);

    // emit .shstrtab section header table entry
    ELF32_Section_Header str_sh = {0};
    str_sh.type = SHT_STRTAB;
    str_sh.addr = 0; // no runtime address
    str_sh.offset = strtab_offset;
    str_sh.size = strtab_size;
    str_sh.flags = 0;
    str_sh.link = 0;
    str_sh.info = 0;
    str_sh.addralign = 1;
    str_sh.endsize = 0;
    memcpy(at, &str_sh, sizeof(str_sh));
    file_offset += sizeof(str_sh);
    at += sizeof(str_sh);

    FILE* file = fopen(output_filename, "wb");
    if(!file)
    {
        fprintf(stderr, "Could not open file %s for writing\n", output_filename);
        return;
    }
    fwrite(stream, at - stream, 1, file);
    fclose(file);
}