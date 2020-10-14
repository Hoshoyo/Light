#include "light_pecoff.h"
#include <stdlib.h>
#include <stdio.h>
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <light_array.h>

#define COFF_IDATA
#define COFF_RELOC
#define COFF_RDATA

typedef struct {
    Relocation_Entry* relocs;
} PECoff_Generator;

static int align_delta(int offset, int align_to)
{
	int rest = offset % align_to;
	return((align_to - rest) % align_to);
}

static char dos_hdr[] = {
    0x77, 0x90, 0x144, 0x0, 0x3, 0x0, 0x0, 0x0, 0x4, 0x0, 0x0, 0x0, 0x255, 0x255, 0x0, 0x0, 0x184, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x64, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x176, 0x0, 0x0, 0x0, 0x14, 0x31, 0x186, 0x14, 0x0, 0x180, 0x9, 0x205, 0x33, 0x184, 0x1, 0x76, 0x205, 0x33, 0x84, 0x104, 0x105, 0x115, 0x32, 0x112, 0x114, 0x111, 0x103, 0x114, 0x97, 0x109, 0x32, 0x99, 0x97, 0x110, 0x110, 0x111, 0x116, 0x32, 0x98, 0x101, 0x32, 0x114, 0x117, 0x110, 0x32, 0x105, 0x110, 0x32, 0x68, 0x79, 0x83, 0x32, 0x109, 0x111, 0x100, 0x101, 0x46, 0x13, 0x13, 0x10, 0x36, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x239, 0x10, 0x11, 0x221, 0x171, 0x107, 0x101, 0x142, 0x171, 0x107, 0x101, 0x142, 0x171, 0x107, 0x101, 0x142, 0x60, 0x53, 0x97, 0x143, 0x169, 0x107, 0x101, 0x142, 0x60, 0x53, 0x103, 0x143, 0x170, 0x107, 0x101, 0x142, 0x82, 0x105, 0x99, 0x104, 0x171, 0x107, 0x101, 0x142, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0
};

static Relocation_Entry
relocation_new(int offset)
{
    Relocation_Entry r = {.type = IMAGE_REL_BASED_HIGHLOW, .offset = offset};
    return r;
}

static u8*
write_rdata(u8* at, u8* text_ptr, int rdata_rva, int base_rva, X86_Data* data_seg, PECoff_Generator* gen)
{
    int offset = 0;
    for(int i = 0; i < array_length(data_seg); ++i)
    {
        X86_Data* data = data_seg + i;
        if(data->length_bytes > 8)
        {
            memcpy(at, data->large_data, data->length_bytes);
        }
        else
        {
            memcpy(at, data->reg_size_data, data->length_bytes);
        }
        *(u32*)(text_ptr + data->patch_offset) = rdata_rva + offset + base_rva;
        offset += data->length_bytes;
        at += data->length_bytes;
        array_push(gen->relocs, relocation_new(data->patch_offset));
    }
    *at++ = 0;
    return at;
}

static u8*
write_reloc(u8* at, int reloc_rva, int text_rva, Optional_Header_DataDir* data_dir, PECoff_Generator* gen)
{
    u8* start = at;
    Base_Relocation_Block* brb = (Base_Relocation_Block*)at;
    brb->page_rva = text_rva; // .text address to which calculate offsets from
    at += sizeof(Base_Relocation_Block);

    for(int r = 0; r < array_length(gen->relocs); ++r)
    {
        *(Relocation_Entry*)at = gen->relocs[r];
        at += sizeof(Relocation_Entry);
    }

    brb->block_size = (at - start);
    data_dir->base_relocation_table.size = brb->block_size;
    data_dir->base_relocation_table.virtual_address = reloc_rva;

    return at;
}

typedef struct {
    char* symbol;
    int   length;
    u16   hint;

    // import address table address
    u32* iat_patch_addr;
    int  iat_rva;

    // import name table address to patch
    u32* inamt_patch_addr;
} Import_Symbol;

typedef struct {
    char* name;
    int   length;
    Import_Symbol *symbols;

    // import address table start address
    void* iat_addr;
    int   iat_rva;

    u32* ilt_patch_addr;        // import lookup table address to patch
    u32* ilt_patch_name_rva;    // name rva address to patch
} Import_Libs;

Import_Libs
pecoff_new_lib(const char* name)
{
    int length = strlen(name);
    void* n = calloc(length, 1);
    memcpy(n, name, length);

    Import_Libs l = {.name = n, .length = length};
    l.symbols = array_new(Import_Symbol);
    return l;
}

Import_Symbol
pecoff_new_symbol(const char* name, u16 hint)
{
    int length = strlen(name);
    void* n = calloc(length, 1);
    memcpy(n, name, length);
    Import_Symbol s = {.symbol = n, .length = length, .hint = hint};
    return s;
}

// @Temporary
static Import_Libs*
import_libs_new()
{
    Import_Libs* libs = array_new(Import_Libs);

    // user32
    Import_Libs user32 = pecoff_new_lib("USER32.dll");
    array_push(user32.symbols, pecoff_new_symbol("MessageBoxA", 0x27f));
    array_push(libs, user32);

    // kernel32
    Import_Libs kernel32 = pecoff_new_lib("KERNEL32.dll");
    array_push(kernel32.symbols, pecoff_new_symbol("LoadLibraryA", 0x3c1));
    array_push(kernel32.symbols, pecoff_new_symbol("FreeLibrary", 0x3c1));
    array_push(libs, kernel32);

    return libs;
}

static u8*
write_iat(u8* at, Import_Libs* libs, int base_rva, int rva, Optional_Header_DataDir* data_dir, int ptr_size_bytes)
{
    u8* start = at;

    for(int lib = 0; lib < array_length(libs); ++lib)
    {
        Import_Libs* l = libs + lib;
        l->iat_addr = at;
        l->iat_rva = rva + (at - start);
        for(int s = 0; s < array_length(l->symbols); ++s)
        {
            Import_Symbol* symbol = l->symbols + s;
            symbol->iat_patch_addr = (u32*)at;
            symbol->iat_rva = rva + (at - start);
            at += ptr_size_bytes;
        }
    }

    // at the end, fill out the size
    data_dir->import_address_table.size = (at - start);
    data_dir->import_address_table.virtual_address = rva;

    return at;
}

// @IMPORTANT the at stream must be zero filled for this function to work
static u8*
write_idt(u8* at, Import_Libs* libs, int base_rva, int rva, Optional_Header_DataDir* data_dir, int ptr_size_bytes)
{
    u8* start = at;

    // fil the data directory information
    data_dir->import_table.size = sizeof(Import_Directory_Table) * (array_length(libs) + 1); // one extra for the null entry
    data_dir->import_table.virtual_address = rva;

    for(int lib = 0; lib < array_length(libs); ++lib)
    {
        Import_Libs* l = libs + lib;
        Import_Directory_Table* idt = (Import_Directory_Table*)at;
        idt->import_address_table = l->iat_rva;
        
        // fill addresses to be patched later
        l->ilt_patch_addr = &idt->import_lookup_table;
        l->ilt_patch_name_rva = &idt->name_rva;

        at += sizeof(Import_Directory_Table);
    }
    at += sizeof(Import_Directory_Table); // null entry

    return at;
}

static u8*
write_ilt(u8* at, Import_Libs* libs, int base_rva, int rva, Optional_Header_DataDir* data_dir, int ptr_size_bytes)
{
    u8* start = at;

    for(int lib = 0; lib < array_length(libs); ++lib)
    {
        Import_Libs* l = libs + lib;
        *l->ilt_patch_addr = rva + (at - start);

        for(int s = 0; s < array_length(l->symbols); ++s)
        {
            Import_Symbol* symbol = l->symbols + s;

            symbol->inamt_patch_addr = (u32*)at;
            at += ptr_size_bytes;       // allocate 4 bytes to symbol name address
        }

        *(u32*)at = 0;
        at += ptr_size_bytes;   // null entry
    }

    return at;
}

static u8*
write_iname_table(u8* at, Import_Libs* libs, int base_rva, int rva, Optional_Header_DataDir* data_dir, int ptr_size_bytes)
{
    u8* start = at;

    for(int lib = 0; lib < array_length(libs); ++lib)
    {
        Import_Libs* l = libs + lib;
        for(int s = 0; s < array_length(l->symbols); ++s)
        {
            Import_Symbol* symbol = l->symbols + s;
            *symbol->iat_patch_addr = rva + (at - start);
            *symbol->inamt_patch_addr = rva + (at - start);

            // write hint
            *(u16*)at = symbol->hint;
            at += sizeof(u16);

            // write symbol
            memcpy(at, symbol->symbol, symbol->length);
            at += (symbol->length);
            *at++ = 0;

            // align to even address
            if((rva + (at - start)) % 2 != 0) *at++ = 0;
        }
        // patch the idt name_rva and write the library name
        *l->ilt_patch_name_rva = rva + (at - start);
        memcpy(at, l->name, l->length);
        at += l->length;
        *at++ = 0;

        // align to even address
        if((rva + (at - start)) % 2 != 0) *at++ = 0;
    }

    return at;
}

static u8*
write_idata(u8* at, Import_Libs* libs, int base_rva, Optional_Header_DataDir* data_dir)
{
    u8* start = at;

    // write the import address table IAT
    at = write_iat(at, libs, base_rva, base_rva, data_dir, 4);

    // write the import directory table IDT
    at = write_idt(at, libs, base_rva, base_rva + (at - start), data_dir, 4);

    // write the import lookup table ILT
    at = write_ilt(at, libs, base_rva, base_rva + (at - start), data_dir, 4);

    // write the import name table INT
    at = write_iname_table(at, libs, base_rva, base_rva + (at - start), data_dir, 4);

    at = at + align_delta(at - start, 0x200);

    return at;
}

static void
fill_relative_patches(int base_rva, int rva, X86_Patch* rel_patches, PECoff_Generator* gen)
{
    gen->relocs = array_new(Relocation_Entry);
    for(int i = 0; i < array_length(rel_patches); ++i)
    {
        if (rel_patches[i].generate_relocation)
        {
            int issuer_offset = rel_patches[i].issuer_offset + rel_patches[i].issuer_imm_offset;
            int offset = rel_patches[i].issuer_offset + (*(int*)(rel_patches[i].addr));
            *((int*)(rel_patches[i].addr)) = offset + base_rva + rva;
            array_push(gen->relocs, relocation_new(issuer_offset));
        }
    }
}

void
light_pecoff_emit(u8* in_stream, int in_stream_size_bytes, X86_Patch* rel_patches, X86_Data* data_seg)
{
    PECoff_Generator gen = { 0 };
    u8* stream = (u8*)calloc(1, 1024*1024);
    u8* at = stream;

    u8* ptr_to_pe_header = 0;
    
#if 1
    DOS_Header* dos_header = (DOS_Header*)at;
    at += sizeof(DOS_Header);
    dos_header->signature[0] = 'M';
    dos_header->signature[1] = 'Z';
    dos_header->ptr_to_pe_header = at - stream;
#else
    memcpy(at, dos_hdr, sizeof(dos_hdr));
    at += sizeof(dos_hdr);
#endif

    Coff_Header* coff_hdr = (Coff_Header*)at;
    at += sizeof(Coff_Header);
    coff_hdr->signature.p = 'P';
    coff_hdr->signature.e = 'E';
    coff_hdr->machine = IMAGE_FILE_MACHINE_I386;
    coff_hdr->num_sections = 0;
    coff_hdr->time_stamp = 0x590bf118;    // TODO(psv): see if necessary
    coff_hdr->ptr_to_symbol_table = 0;
    coff_hdr->num_of_symbols = 0;
    coff_hdr->size_optional_header = sizeof(Optional_Header) + sizeof(Optional_Image_Only_PE32) + sizeof(Optional_Header_DataDir);
    coff_hdr->characteristics = IMAGE_FILE_EXECUTABLE_IMAGE|IMAGE_FILE_32BIT_MACHINE|IMAGE_FILE_LARGE_ADDRESS_AWARE;

    Optional_Header* optional_hdr = (Optional_Header*)at;
    at += sizeof(Optional_Header);
    optional_hdr->magic = 0x10b;                // 32bit, 0x20b for 64
    optional_hdr->maj_linker_version = 0x0e;
    optional_hdr->min_linker_version = 0x18;
    optional_hdr->size_of_code = 0;             // filled after
    optional_hdr->size_of_init_data = 0;        // only needed for static data
    optional_hdr->size_of_uninit_data = 0;      // only needed for static data
    optional_hdr->address_of_entry = 0x1000;    // TODO <---------------- Entry point address 
    optional_hdr->base_of_code = 0x1000;
    optional_hdr->base_of_data = 0;

    Optional_Image_Only_PE32* opt_pe32 = (Optional_Image_Only_PE32*)at;
    at += sizeof(Optional_Image_Only_PE32);
    opt_pe32->image_base_pe32 = 0x400000;
    opt_pe32->section_alignment = 0x1000;
    opt_pe32->file_alignment = 0x200;
    opt_pe32->maj_os_version = 6;
    opt_pe32->min_os_version = 0;
    opt_pe32->maj_image_version = 0;
    opt_pe32->min_image_version = 0;
    opt_pe32->maj_subsystem_version = 6;
    opt_pe32->min_subsystem_version = 0;
    opt_pe32->win32_version_value = 0;
    opt_pe32->size_of_image = 0; // filled after
    opt_pe32->size_of_headers = 0; // filled after
    opt_pe32->check_sum = 0;
    opt_pe32->subsystem = 3;
    opt_pe32->dll_characteristics = 0x8540;  // TODO(psv): see what is is
    opt_pe32->size_stack_reserve_pe32 = 0x100000;
    opt_pe32->size_stack_commit_pe32 = 0x1000;
    opt_pe32->size_heap_reserve_pe32 = 0x100000;
    opt_pe32->size_heap_commit_pe32 = 0x1000;
    opt_pe32->loader_flags = 0;
    opt_pe32->num_of_rva_and_sizes = 0x10;

    Optional_Header_DataDir* opt_datadir = (Optional_Header_DataDir*)at;
    at += sizeof(Optional_Header_DataDir);
    opt_datadir->export_table.size = 0;
    opt_datadir->export_table.virtual_address = 0;
    opt_datadir->import_table.size = 0;
    opt_datadir->import_table.virtual_address = 0;
    opt_datadir->resource_table.size = 0;
    opt_datadir->resource_table.virtual_address = 0;
    opt_datadir->exception_table.size = 0;
    opt_datadir->exception_table.virtual_address = 0;
    opt_datadir->certificate_table.size = 0;
    opt_datadir->certificate_table.virtual_address = 0;
    opt_datadir->base_relocation_table.size = 0;
    opt_datadir->base_relocation_table.virtual_address = 0;
    opt_datadir->debug.size = 0;
    opt_datadir->debug.virtual_address = 0;
    opt_datadir->architecture.size = 0;
    opt_datadir->architecture.virtual_address = 0;

    /*
        Section table
        .text (code)
        contains the x86 code for the executable
    */
    Section_Table* text_st = (Section_Table*)at;
    at += sizeof(Section_Table);
    memcpy(text_st->name, ".text", sizeof(".text") - 1);
    text_st->virtual_size = in_stream_size_bytes;
    text_st->virtual_address = 0x1000;
    text_st->size_of_raw_data = in_stream_size_bytes + align_delta(in_stream_size_bytes, opt_pe32->file_alignment);    // must be multiple of FileAlignment
    text_st->ptr_to_raw_data = 0;   // filled after
    text_st->ptr_to_relocations = 0;
    text_st->ptr_to_line_numbers = 0;
    text_st->num_of_relocations = 0;
    text_st->num_of_line_numbers = 0;
    text_st->characteristics = IMAGE_SCN_MEM_EXECUTE|IMAGE_SCN_MEM_READ|IMAGE_SCN_CNT_CODE;
    coff_hdr->num_sections++;

    optional_hdr->size_of_code = text_st->size_of_raw_data;

    // this needs to be written before the text section
    fill_relative_patches(opt_pe32->image_base_pe32, text_st->virtual_address, rel_patches, &gen);

#if defined(COFF_IDATA)
    /*
        Section table
        .idata (read only initialized data)
        contains imported symbols
    */
    Section_Table* idata_st = (Section_Table*)at;
    at += sizeof(Section_Table);
    memcpy(idata_st->name, ".idata", sizeof(".idata") - 1);
    idata_st->virtual_size = 0;         // filled after
    idata_st->virtual_address = text_st->virtual_address + text_st->size_of_raw_data + align_delta(text_st->size_of_raw_data, opt_pe32->section_alignment); // TODO(psv): maybe this should be text_st->size_of_raw_data + text_st->size_of_raw_data
    idata_st->size_of_raw_data = 0;     // filled after, must be a multiple of file alignment
    idata_st->ptr_to_raw_data = 0;      // filled after
    idata_st->ptr_to_relocations = 0;
    idata_st->ptr_to_line_numbers = 0;
    idata_st->num_of_relocations = 0;
    idata_st->num_of_line_numbers = 0;
    idata_st->characteristics = IMAGE_SCN_CNT_INITIALIZED_DATA|IMAGE_SCN_MEM_READ;
    coff_hdr->num_sections++;
#endif
#if defined(COFF_RDATA)
    /*
        Section table
        .rdata (read only initialized data)
        contains 
    */
    Section_Table* rdata_st = (Section_Table*)at;
    at += sizeof(Section_Table);
    memcpy(rdata_st->name, ".rdata", sizeof(".rdata") - 1);
    rdata_st->virtual_size = 0;      // filled after
    rdata_st->virtual_address = 0;   // filled after
    rdata_st->size_of_raw_data = 0;  // filled after, must be multiple of file alignment
    rdata_st->ptr_to_raw_data = 0;   // filled after
    rdata_st->ptr_to_relocations = 0;
    rdata_st->ptr_to_line_numbers = 0;
    rdata_st->num_of_relocations = 0;
    rdata_st->num_of_line_numbers = 0;
    rdata_st->characteristics = IMAGE_SCN_CNT_INITIALIZED_DATA|IMAGE_SCN_MEM_READ;
    coff_hdr->num_sections++;
#endif
#if defined(COFF_RELOC)
    /*
        Section table
        .reloc (relocation)
        contains relocation tables
    */
    Section_Table* reloc_st = (Section_Table*)at;
    at += sizeof(Section_Table);
    memcpy(reloc_st->name, ".reloc", sizeof(".reloc") - 1);
    reloc_st->virtual_size = 0;         // filled after
    reloc_st->virtual_address = 0;      // filled after
    reloc_st->size_of_raw_data = 0;     // filled after, must be a multiple of file alignment
    reloc_st->ptr_to_raw_data = 0;      // filled after
    reloc_st->ptr_to_relocations = 0;
    reloc_st->ptr_to_line_numbers = 0;
    reloc_st->num_of_relocations = 0;
    reloc_st->num_of_line_numbers = 0;
    reloc_st->characteristics = IMAGE_SCN_CNT_INITIALIZED_DATA|IMAGE_SCN_MEM_READ|IMAGE_SCN_MEM_DISCARDABLE;
    coff_hdr->num_sections++;
#endif
    Section_Table* last_before_reloc = 0;

    // align to filealignment
    int offset = at - stream;
    at += align_delta(offset, opt_pe32->file_alignment);

    // End of section headers
    opt_pe32->size_of_headers = at - stream;

    /*
        .text raw data
    */
    u8* text_ptr = at;
    text_st->ptr_to_raw_data = at - stream;
    // emit text data
    memcpy(at, in_stream, in_stream_size_bytes);
    at += in_stream_size_bytes;
    last_before_reloc = text_st;

    /*
        .idata raw data
    */
#if defined(COFF_IDATA)
    // idata
    at += align_delta(at - stream, opt_pe32->section_alignment);    // this is needed before every section raw data
    idata_st->ptr_to_raw_data = at - stream;
    u8* idata_start = at;
    at = write_idata(at, import_libs_new(), idata_st->virtual_address, opt_datadir);
    idata_st->virtual_size = at - idata_start;
    idata_st->size_of_raw_data = idata_st->virtual_size + align_delta(idata_st->virtual_size, opt_pe32->file_alignment);
    last_before_reloc = idata_st;
#endif
#if defined(COFF_RDATA)
    // rdata
    at += align_delta(at - stream, opt_pe32->section_alignment);    // this is needed before every section raw data
    rdata_st->ptr_to_raw_data = at - stream;
    u8* rdata_start = at;
    rdata_st->virtual_address = idata_st->virtual_address + idata_st->size_of_raw_data + align_delta(idata_st->size_of_raw_data, opt_pe32->section_alignment);
    at = write_rdata(at, text_ptr, rdata_st->virtual_address, opt_pe32->image_base_pe32, data_seg, &gen);
    rdata_st->virtual_size = at - rdata_start;
    rdata_st->size_of_raw_data = rdata_st->virtual_size + align_delta(rdata_st->virtual_size, opt_pe32->file_alignment);
    last_before_reloc = rdata_st;
#endif
#if defined(COFF_RELOC)
    // reloc
    at += align_delta(at - stream, opt_pe32->section_alignment);    // this is needed before every section raw data
    reloc_st->ptr_to_raw_data = at - stream;
    u8* reloc_start = at;
    reloc_st->virtual_address = last_before_reloc->virtual_address + last_before_reloc->size_of_raw_data + align_delta(last_before_reloc->size_of_raw_data, opt_pe32->section_alignment);
    at = write_reloc(at, reloc_st->virtual_address, text_st->virtual_address, opt_datadir, &gen);
    reloc_st->virtual_size = at - reloc_start;
    reloc_st->size_of_raw_data = reloc_st->virtual_size + align_delta(reloc_st->virtual_size, opt_pe32->file_alignment);
#endif
    // this should be set to the last virtual address section table
    Section_Table* last_table_va = reloc_st;

    /*
        End of file
    */
    at += align_delta(at - stream, opt_pe32->section_alignment);
    opt_pe32->size_of_image = last_table_va->virtual_address + last_table_va->size_of_raw_data +
        align_delta(last_table_va->size_of_raw_data, opt_pe32->section_alignment);

    /*
        File write
    */
    FILE* file = fopen("out1.exe", "wb");
    if(!file)
    {
        fprintf(stderr, "Could not open file out2.bin for writing\n");
        return;
    }
    fwrite(stream, at - stream, 1, file);
    fclose(file);
}
#endif