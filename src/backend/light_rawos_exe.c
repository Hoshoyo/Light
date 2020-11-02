#include "light_rawos_exe.h"
#include <string.h>
#include <light_array.h>
#include <light_arena.h>
#include <hoht.h>
#include <stdlib.h>
#include <stddef.h>

typedef struct {
    char* symbol;
    int   length;

    int  section_offset;
    int  rva;
} Import_Symbol;

typedef struct {
    char* name;
    int   length;

    int section_offset;
    int rva;
    Import_Symbol *symbols;
} Import_Libs;

Import_Libs
rawx_new_lib(const char* name, int length)
{
    void* n = calloc(1, length);
    memcpy(n, name, length);

    Import_Libs l = {.name = n, .length = length};
    l.symbols = 0;
    l.symbols = array_new(Import_Symbol);
    return l;
}

Import_Symbol
rawx_new_symbol(const char* name, int length, u16 hint)
{
    void* n = calloc(1, length);
    memcpy(n, name, length);
    Import_Symbol s = {.symbol = n, .length = length};
    return s;
}

// TODO(psv): optimize this, no need to iterate through both hash tables
static Hoht_Table htlibs;
typedef struct {
    string     libstr;
    Hoht_Table symbols;
    int        lib_index;
} ILib;
static Import_Libs*
import_libs(u8* stream_text_base, u8* text_base, X86_Import* imports, int* symbol_count)
{
    Import_Libs* libs = array_new(Import_Libs);
    hoht_new(&htlibs, array_length(imports) * 8, sizeof(ILib), 0.5f, malloc, free);

    // push all libraries and symbols to the hash table
    for(int i = 0; i < array_length(imports); ++i)
    {
        X86_Import* imp = imports + i;
        imp->patch_addr = stream_text_base + ((u8*)imp->patch_addr - text_base); // patch address is now inside the output stream
        *((u32*)imp->patch_addr) = 0xcccccccc;
        
        string libstr = MAKE_STR_LEN(imp->decl->decl_proc.extern_library_name->data + 1, imp->decl->decl_proc.extern_library_name->length - 2);
        string symstr = MAKE_STR_LEN(imp->decl->decl_proc.name->data, imp->decl->decl_proc.name->length);
        u64 hashlib = fnv_1_hash((const u8*)libstr.data, libstr.length);
        u64 hashsymb = fnv_1_hash((const u8*)symstr.data, symstr.length);

        ILib* lentry = (ILib*)hoht_get_value_hashed(&htlibs, hashlib);
        if(!lentry)
        {
            ILib ilib = {.libstr = libstr, .lib_index = array_length(libs)};
            hoht_new(&ilib.symbols, array_length(imports) * 8, sizeof(string), 0.5f, malloc, free);
            hoht_push_hashed(&ilib.symbols, hashsymb, &symstr);
            hoht_push_hashed(&htlibs, hashlib, &ilib);
            
            Import_Libs lib = rawx_new_lib(libstr.data, libstr.length);
            array_push(lib.symbols, rawx_new_symbol(symstr.data, symstr.length, 0xcc));
            array_push(libs, lib);
            imp->lib_index = ilib.lib_index;
            imp->sym_index = 0;
            *symbol_count = *symbol_count + 1;
        }
        else
        {
            string* sym = (string*)hoht_get_value_hashed(&lentry->symbols, hashsymb);
            if(sym)
            {
                // Symbol already exists, just update the X86_Import
                imp->sym_index = sym->value;
                imp->lib_index = lentry->lib_index;
            }
            else
            {
                // Create the symbol
                symstr.value = array_length(libs[lentry->lib_index].symbols);
                imp->lib_index = lentry->lib_index;
                imp->sym_index = symstr.value;
                array_push(libs[lentry->lib_index].symbols, rawx_new_symbol(symstr.data, symstr.length, 0xcc));
                hoht_push_hashed(&lentry->symbols, hashsymb, &symstr);
                *symbol_count = *symbol_count + 1;
            }
        }
    }
    return libs;
}

static int align_delta(int offset, int align_to)
{
	int rest = offset % align_to;
	return((align_to - rest) % align_to);
}

static void
patch_imports(Import_Libs* libs, X86_Import* imports, int load_address)
{
    for(int i = 0; i < array_length(imports); ++i)
    {
        int rva = libs[imports[i].lib_index].symbols[imports[i].sym_index].rva;
        *(u32*)imports[i].patch_addr = rva + load_address;
    }
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

static int
write_data(u8* at, u8* code_ptr, int data_rva, int base_rva, X86_DataSeg_Patch* dseg_patches, IR_Data_Segment_Entry* dseg_entries)
{
    u8* start = at;
    int offset = 0;

    for(int i = 0; i < array_length(dseg_entries); ++i)
    {
        IR_Data_Segment_Entry* entry = dseg_entries + i;

        entry->dseg_offset_bytes = offset;
        offset += entry->data_length_bytes;
    }
    at += offset;   // allocate all the space

    for(int i = 0; i < array_length(dseg_patches); ++i)
    {
        X86_DataSeg_Patch* patch = dseg_patches + i;
        IR_Data_Segment_Entry* dseg_entry = &dseg_entries[patch->dseg_entry_index];
        *(u32*)(code_ptr + patch->patch_offset) = data_rva + dseg_entry->dseg_offset_bytes + base_rva;
    }

    *at++ = 0;
    return at - start;
}

static int
write_imports(u8* at, Import_Libs* libs, int load_address, int rva, RawOS_Import_Table* imp_table)
{
    u8* start = at;

    at += sizeof(RawOS_Import_Table);
    int start_rva = rva + sizeof(RawOS_Import_Table);

    RawOS_Import_Address* imps = (RawOS_Import_Address*)at;
    int import_addresses_size = (sizeof(RawOS_Import_Address) * imp_table->symbol_count);
    at += import_addresses_size;

    int current_rva = rva + load_address + (at - start);
    int symb_index = 0;

    for(int l = 0; l < array_length(libs); ++l)
    {
        Import_Libs* lib = libs + l;
        // write the library name
        lib->rva = current_rva;
        lib->section_offset = (at - start);
        memcpy(at, lib->name, lib->length);
        at += lib->length;
        *at++ = 0;
        current_rva += (lib->length + 1);

        // write symbols
        for(int s = 0; s < array_length(lib->symbols); ++s)
        {
            Import_Symbol* symbol = lib->symbols + s;
            symbol->rva = start_rva + s * sizeof(RawOS_Import_Address) + offsetof(RawOS_Import_Address, call_address);
            symbol->section_offset = (at - start);
            memcpy(at, symbol->symbol, symbol->length);
            at += symbol->length;
            *at++ = 0;
            current_rva += (symbol->length + 1);
            imps[symb_index].section_symbol_offset = symbol->section_offset;
            imps[symb_index].section_lib_offset = lib->section_offset;
            symb_index++;
        }
    }

    return (at - start);
}

void
light_rawos_emit(const char* out_filename, u8* in_stream, int in_stream_size_bytes, int entry_point_offset, 
    X86_Patch* rel_patches, X86_Data* data_seg, X86_Import* imports, 
    X86_DataSeg_Patch* dseg_patches, IR_Data_Segment_Entry* dseg_entries)
{
    u8* stream = (u8*)calloc(1, 1024*1024*16);
    u8* at = stream;

    const u32 load_address = 0x40000000;

    RawOS_Header* header = (RawOS_Header*)at;
    header->magic[0] = 'R';
    header->magic[1] = 'A';
    header->magic[2] = 'W';
    header->magic[3] = 'X';
    header->version = RAWX_VERSION;
    header->flags = RAWX_ARCH_X86;
    header->load_address = load_address;
    header->entry_point_offset = 0x1000 + entry_point_offset;
    header->stack_size = 1024 * 1024;
    header->section_count = 0; // filled after
    at += sizeof(RawOS_Header);

    RawOS_Section* last_section = 0;

    /*
        Define .code section
    */
    RawOS_Section* code_section = (RawOS_Section*)at;
    memcpy(code_section->name, ".code", sizeof(".code") - 1);
    code_section->size_bytes = 0; // filled after
    code_section->virtual_address = 0x1000;
    code_section->file_ptr_to_data = 0; // filled after
    at += sizeof(RawOS_Section);
    header->section_count++;

    fill_relative_patches(load_address, code_section->virtual_address, rel_patches);

    /*
        Define .data section
    */
    RawOS_Section* data_section = (RawOS_Section*)at;
    memcpy(data_section->name, ".data", sizeof(".data") - 1);
    data_section->size_bytes = 0; // filled after
    data_section->virtual_address = 0;  // filled after knowing the size of the code section
    data_section->file_ptr_to_data = 0; // filled after
    at += sizeof(RawOS_Section);
    header->section_count++;

    /*
        Define .import section
    */
    RawOS_Section* imp_section = (RawOS_Section*)at;
    memcpy(imp_section->name, ".import", sizeof(".import") - 1);
    imp_section->size_bytes = 0; // filled after
    imp_section->virtual_address = 0;  // filled after knowing the size of the data section
    imp_section->file_ptr_to_data = 0; // filled after
    at += sizeof(RawOS_Section);
    header->section_count++;

    /*
        Write the .code section
    */
    u8* code_ptr = at;
    code_section->file_ptr_to_data = (at - stream);
    code_section->size_bytes = in_stream_size_bytes;
    memcpy(at, in_stream, in_stream_size_bytes);
    at += in_stream_size_bytes;
    at += align_delta(at - stream, 16);
    last_section = code_section;

    /*
        Write the .data section
    */
    u8* data_ptr = at;
    data_section->virtual_address = last_section->virtual_address + last_section->size_bytes + align_delta(last_section->virtual_address + last_section->size_bytes, 0x1000);
    data_section->file_ptr_to_data = (at-stream);
    at += write_data(at, code_ptr, data_section->virtual_address, load_address, dseg_patches, dseg_entries);
    data_section->size_bytes = (at - data_ptr);
    at += align_delta(at - stream, 16);
    last_section = data_section;

    /*
        Write the .import section
    */
    u8* import_ptr = at;
    imp_section->virtual_address = last_section->virtual_address + last_section->size_bytes + align_delta(last_section->virtual_address + last_section->size_bytes, 0x1000);
    imp_section->file_ptr_to_data = (at - stream);
    RawOS_Import_Table* imp_table = (RawOS_Import_Table*)at;
    Import_Libs* imp_libs = import_libs(code_ptr, in_stream, imports, (int*)&imp_table->symbol_count);
    at += write_imports(at, imp_libs, load_address, imp_section->virtual_address, imp_table);
    patch_imports(imp_libs, imports, load_address);
    imp_section->size_bytes = (at - import_ptr);
    last_section = imp_section;

    /*
        File write
    */
    char filename[256] = { 0 };
    sprintf(filename, "%s.rawx", out_filename);
    FILE* file = fopen(filename, "wb");
    if(!file)
    {
        fprintf(stderr, "Could not open file out1.rawx for writing\n");
        return;
    }
    fwrite(stream, 1, at - stream, file);
    fclose(file);
}