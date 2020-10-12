#include "light_pecoff.h"
#include <stdlib.h>
#include <stdio.h>
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>

#define COFF_IDATA
//#define COFF_RELOC

static int align_delta(int offset, int align_to)
{
	int rest = offset % align_to;
	return((align_to - rest) % align_to);
}

static char dos_hdr[] = {
    0x77, 0x90, 0x144, 0x0, 0x3, 0x0, 0x0, 0x0, 0x4, 0x0, 0x0, 0x0, 0x255, 0x255, 0x0, 0x0, 0x184, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x64, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x176, 0x0, 0x0, 0x0, 0x14, 0x31, 0x186, 0x14, 0x0, 0x180, 0x9, 0x205, 0x33, 0x184, 0x1, 0x76, 0x205, 0x33, 0x84, 0x104, 0x105, 0x115, 0x32, 0x112, 0x114, 0x111, 0x103, 0x114, 0x97, 0x109, 0x32, 0x99, 0x97, 0x110, 0x110, 0x111, 0x116, 0x32, 0x98, 0x101, 0x32, 0x114, 0x117, 0x110, 0x32, 0x105, 0x110, 0x32, 0x68, 0x79, 0x83, 0x32, 0x109, 0x111, 0x100, 0x101, 0x46, 0x13, 0x13, 0x10, 0x36, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x239, 0x10, 0x11, 0x221, 0x171, 0x107, 0x101, 0x142, 0x171, 0x107, 0x101, 0x142, 0x171, 0x107, 0x101, 0x142, 0x60, 0x53, 0x97, 0x143, 0x169, 0x107, 0x101, 0x142, 0x60, 0x53, 0x103, 0x143, 0x170, 0x107, 0x101, 0x142, 0x82, 0x105, 0x99, 0x104, 0x171, 0x107, 0x101, 0x142, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0
};

typedef struct {
    u16 type   : 4;
    u16 offset : 12;
} Relocation_Entry;

static u8*
write_reloc(u8* at, int base_rva, Optional_Header_DataDir* data_dir)
{
    u8* start = at;
    Base_Relocation_Block* brb = (Base_Relocation_Block*)at;
    brb->page_rva = 0x1000;
    brb->block_size = (at - start);
    at += sizeof(Base_Relocation_Block);

   // Relocation_Entry entry = {.type = IMAGE_REL_BASED_HIGHLOW, .offset = };

    return at;
}

static u8*
write_idata(u8* at, int base_rva, Optional_Header_DataDir* data_dir)
{
    int rva_offset = 0;
    u8* start = at;
    // Import address table
    u32* iat_msg_box = (u32*)at;
    at += 4 + 44; // size of pointer (MessageBoxA)
    //at += 4 + 44; // size of pointer (LoadLibraryA)
    rva_offset += (at - start);
    data_dir->import_address_table.size = (at - start);
    data_dir->import_address_table.virtual_address = base_rva;

    // Import directory table (one for each DLL that the image refers)
    int idt_rva = base_rva + (at - start);
    Import_Directory_Table* idt = (Import_Directory_Table*)at;
    rva_offset += sizeof(Import_Directory_Table);
    at += sizeof(Import_Directory_Table);
    idt->import_address_table = base_rva;
    data_dir->import_table.size = sizeof(Import_Directory_Table) * 2; // 2 entries, counting the null one
    data_dir->import_table.virtual_address = idt_rva;
    // Null entry
    at += sizeof(Import_Directory_Table);
    idt->import_lookup_table = base_rva + (at - start);

    // Indirect RVA to name of functions + 44 bytes of padding (it seems)
    u32* msgbox_rva_addr = (u32*)at;
    at += (sizeof(u32) + 44);

    // Import name table
    *msgbox_rva_addr = base_rva + (at - start);
    *iat_msg_box = base_rva + (at - start);
    Hint_Name_Table msgbox = {0};
    msgbox.hint = 0x27f;
    *(Hint_Name_Table*)at = msgbox;
    at += sizeof(Hint_Name_Table);
    memcpy(at, "MessageBoxA", sizeof "MessageBoxA");
    at += sizeof "MessageBoxA";
    if((at - start) % 2 != 0) *at++ = 0;    // address must be even
    idt->name_rva = (at - start) + base_rva;
    memcpy(at, "USER32.dll", sizeof "USER32.dll");
    at += sizeof "USER32.dll";
    *at++ = 0;
    
    at = at + align_delta(at - start, 0x200);
/*
    Hint_Name_Table loadliba = {0};
    loadliba.hint = 0x3c1;
    *(Hint_Name_Table*)at = loadliba;
    at += sizeof(Hint_Name_Table);
    memcpy(at, "LoadLibraryA", sizeof "LoadLibraryA");
    at += sizeof "LoadLibraryA";
    if((at - start) % 2 != 0) *at++ = 0;    // address must be even
    memcpy(at, "KERNEL32.dll", sizeof "KERNEL32.dll");
    at += sizeof "KERNEL32.dll";
    *at++ = 0;
    if((at - start) % 2 != 0) *at++ = 0;    // address must be even
*/
    return at;
}

void
light_pecoff_emit(u8* in_stream, int in_stream_size_bytes)
{
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
    optional_hdr->magic = 0x10b; // 32bit, 0x20b for 64
    optional_hdr->maj_linker_version = 0x0e;
    optional_hdr->min_linker_version = 0x18;
    optional_hdr->size_of_code = 0x200;//(u32)in_stream_size_bytes;
    optional_hdr->size_of_init_data = 0; // only needed for static data
    optional_hdr->size_of_uninit_data = 0; // only needed for static data
    optional_hdr->address_of_entry = 0x1000;  // TODO <---------------- Entry point address 
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
    //opt_datadir->base_relocation_table.size = 0x10;
    //opt_datadir->base_relocation_table.virtual_address = 0x5000;
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
    idata_st->virtual_address = 0x2000; // TODO(psv): maybe this should be text_st->size_of_raw_data + text_st->size_of_raw_data
    idata_st->size_of_raw_data = 0;     // filled after, must be a multiple of file alignment
    idata_st->ptr_to_raw_data = 0;      // filled after
    idata_st->ptr_to_relocations = 0;
    idata_st->ptr_to_line_numbers = 0;
    idata_st->num_of_relocations = 0;
    idata_st->num_of_line_numbers = 0;
    idata_st->characteristics = IMAGE_SCN_CNT_INITIALIZED_DATA|IMAGE_SCN_MEM_READ;
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
    reloc_st->virtual_address = 0x4000; // TODO(psv): 
    reloc_st->size_of_raw_data = 0;     // filled after, must be a multiple of file alignment
    reloc_st->ptr_to_raw_data = 0;      // filled after
    reloc_st->ptr_to_relocations = 0;
    reloc_st->ptr_to_line_numbers = 0;
    reloc_st->num_of_relocations = 0;
    reloc_st->num_of_line_numbers = 0;
    reloc_st->characteristics = IMAGE_SCN_CNT_INITIALIZED_DATA|IMAGE_SCN_MEM_READ|IMAGE_SCN_MEM_DISCARDABLE;
    coff_hdr->num_sections++;
#endif

    /*
    // Section table .data
    Section_Table* data_st = (Section_Table*)at;
    at += sizeof(Section_Table);
    memcpy(text_st->name, ".data", sizeof(".data") - 1);
    data_st->virtual_size = 0;
    data_st->virtual_address = 0x4000;
    data_st->size_of_raw_data = 0;
    data_st->ptr_to_raw_data = 0;   // filled after
    data_st->ptr_to_relocations = 0;
    data_st->ptr_to_line_numbers = 0;
    data_st->num_of_relocations = 0;
    data_st->num_of_line_numbers = 0;
    data_st->characteristics = IMAGE_SCN_CNT_INITIALIZED_DATA|IMAGE_SCN_MEM_READ|IMAGE_SCN_MEM_WRITE;

    offset = at - stream;
    at += align_delta(offset, opt_pe32->file_alignment);
    */

    // align to filealignment
    int offset = at - stream;
    at += align_delta(offset, opt_pe32->file_alignment);

    // End of section headers
    opt_pe32->size_of_headers = at - stream;

    /*
        .text raw data
    */
    text_st->ptr_to_raw_data = at - stream;
    // emit text data
    memcpy(at, in_stream, in_stream_size_bytes);
    at += in_stream_size_bytes;

    /*
        .idata raw data
    */
#if defined(COFF_IDATA)
    // idata
    at += align_delta(at - stream, opt_pe32->section_alignment);    // this is needed before every section raw data
    idata_st->ptr_to_raw_data = at - stream;
    u8* idata_start = at;
    at = write_idata(at, idata_st->virtual_address, opt_datadir);
    idata_st->virtual_size = at - idata_start;
    idata_st->size_of_raw_data = idata_st->virtual_size + align_delta(idata_st->virtual_size, opt_pe32->file_alignment);
#endif
#if defined(COFF_RELOC)
    // reloc
    at += align_delta(at - stream, opt_pe32->section_alignment);    // this is needed before every section raw data
    reloc_st->ptr_to_raw_data = at - stream;
    u8* reloc_start = at;
    at = write_reloc(at, reloc_st->virtual_address, opt_datadir);
    reloc_st->virtual_size = at - reloc_start;
    reloc_st->size_of_raw_data = reloc_st->virtual_size + align_delta(reloc_st->virtual_size, opt_pe32->file_alignment);
#endif

    /*
        End of file
    */
    at += align_delta(at - stream, opt_pe32->section_alignment);
    opt_pe32->size_of_image = at - stream + 0x4000;

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