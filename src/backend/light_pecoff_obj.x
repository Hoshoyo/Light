#include "light_pecoff.h"
#include <stdlib.h>
#include <stdio.h>
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif
#include <light_array.h>
#include <light_arena.h>
#include <hoht.h>

typedef struct {
    int virtual_address;
    int symbol_table_index;
    u16 type;
} COFF_Relocations;

typedef struct {
    COFF_Relocations* relocs;
} PECoff_Generator;

static int align_delta(int offset, int align_to)
{
	int rest = offset % align_to;
	return((align_to - rest) % align_to);
}

static Relocation_Entry
relocation_new(int offset)
{
    Relocation_Entry r = {
        .entry = {.type = IMAGE_REL_BASED_HIGHLOW, .offset = offset},
        .offset32 = offset,
    };
    return r;
}

static void
fill_relative_patches(int base_rva, int rva, X86_Patch* rel_patches, PECoff_Generator* gen)
{
    /*
    gen->relocs = array_new(Relocation_Entry);
    for(int i = 0; i < array_length(rel_patches); ++i)
    {
        if (rel_patches[i].generate_relocation)
        {
            int issuer_offset = rel_patches[i].issuer_offset + rel_patches[i].issuer_imm_offset;
            int offset = rel_patches[i].issuer_offset + (*(int*)(rel_patches[i].addr));
            //*((int*)(rel_patches[i].addr)) = offset + base_rva + rva;
            array_push(gen->relocs, relocation_new(issuer_offset));
        }
    }
    */
}

static int
write_symbol_table(u8* at, X86_Export* exports, int section_number, int section_size, int num_relocs, int* out_symbol_count)
{
    u8* start = at;
    if(out_symbol_count) *out_symbol_count = 0;

    // Define the .text section
    COFF_Symbol_Table_Entry symbol = {
        .section_number = section_number,
        .number_of_aux_symbols = 1,
        .storage_class = IMAGE_SYM_CLASS_STATIC,
        .type = 0,
        .value = 0
    };
    memcpy(symbol.name, ".text$mn", sizeof(".text$mn") - 1);
    *(COFF_Symbol_Table_Entry*)at = symbol;
    at += sizeof(COFF_Symbol_Table_Entry);
    if(out_symbol_count) *out_symbol_count = *out_symbol_count + 2;

    // Define aux section definition for .text
    COFF_Aux_Section_Definition sec_def = {
        .length = section_size,
        .num_relocations = num_relocs,
        .checksum = 0xBDA7CA33
    };
    *(COFF_Aux_Section_Definition*)at = sec_def;
    at += sizeof(COFF_Aux_Section_Definition);

    for(int i = 0; i < array_length(exports); ++i) 
    {
        X86_Export* export = exports + i;

        // TODO(psv): skip the main for now
        //if(export->decl->decl_proc.flags & DECL_PROC_FLAG_MAIN)
        //    continue;

        COFF_Symbol_Table_Entry symbol = {
            .section_number = section_number,
            .number_of_aux_symbols = 0,
            .storage_class = IMAGE_SYM_CLASS_EXTERNAL,
            .type = 0x20, // function
            .value = export->offset
        };
        if(export->symbol_name_length < 8) {
            symbol.name[0] = '_';
            memcpy(symbol.name + 1, export->symbol_name, export->symbol_name_length);
        } else {
            // symbol is greater than 8 bytes
            // TODO(psv): implement string table
        }
        *(COFF_Symbol_Table_Entry*)at = symbol;
        at += sizeof(COFF_Symbol_Table_Entry);
        if(out_symbol_count) *out_symbol_count = *out_symbol_count + 1;
    }

    return (at - start);
}

static int
write_relocations(u8* at, X86_Patch* rel_patches, X86_Export* exports, u16* out_reloc_count)
{
    u8* start = at;
    for(int i = 0; i < array_length(rel_patches); ++i)
    {
        if (rel_patches[i].generate_relocation)
        {
            int issuer_offset = rel_patches[i].issuer_offset + rel_patches[i].issuer_imm_offset;
            int offset = rel_patches[i].issuer_offset + (*(int*)(rel_patches[i].addr));

            // TODO(psv): Speed, this is O(n^2), ugh...
            for(int j = 0; j < array_length(exports); ++j) {
                if(offset == exports[j].offset)
                {
                    COFF_Relocations reloc = {
                        .symbol_table_index = i + 1 + 2,
                        .type = IMAGE_REL_I386_REL32,
                        .virtual_address = issuer_offset
                    };
                    *(COFF_Relocations*)at = reloc;
                    at += sizeof(COFF_Relocations);
                    if(out_reloc_count) *out_reloc_count = *out_reloc_count + 1;
                    break;
                }
            }
        }
    }
    return (at - start);
}

void
light_pecoff_obj_emit(u8* in_stream, int in_stream_size_bytes, int entry_point_offset, 
    X86_Patch* rel_patches, X86_Data* data_seg, X86_Import* imports, 
    X86_DataSeg_Patch* dseg_patches, IR_Data_Segment_Entry* dseg_entries,
    X86_Export* exports)
{
    PECoff_Generator gen = {0};
    gen.relocs = array_new(COFF_Relocations);
    u8* stream = (u8*)calloc(1, 1024*1024*16);
    u8* at = stream;

    Coff_Header* coff_hdr = (Coff_Header*)at;
    at += sizeof(Coff_Header);
    coff_hdr->machine = IMAGE_FILE_MACHINE_I386;
    coff_hdr->num_sections = 0;
    coff_hdr->time_stamp = 0x590bf118;  // TODO(psv): see if necessary
    coff_hdr->ptr_to_symbol_table = 0;  // filled after
    coff_hdr->num_of_symbols = 0;       // filled after
    coff_hdr->size_optional_header = 0; // Zero for obj files
    coff_hdr->characteristics = 0;      // Zero for obj files

    /*
        Section table
        .text (code)
        contains the x86 code for the executable
    */
    Section_Table* text_st = (Section_Table*)at;
    at += sizeof(Section_Table);
    memcpy(text_st->name, ".text$mn", sizeof(".text$mn") - 1);
    text_st->virtual_size = 0;
    text_st->virtual_address = 0;
    text_st->size_of_raw_data = in_stream_size_bytes;
    text_st->ptr_to_raw_data = 0;   // filled after
    text_st->ptr_to_relocations = 0;
    text_st->ptr_to_line_numbers = 0;
    text_st->num_of_relocations = 0;
    text_st->num_of_line_numbers = 0;
    text_st->characteristics = IMAGE_SCN_MEM_EXECUTE|IMAGE_SCN_MEM_READ|IMAGE_SCN_CNT_CODE|IMAGE_SCN_ALIGN_16BYTES;
    coff_hdr->num_sections++;
    int text_section_number = coff_hdr->num_sections;

    // this needs to be written before the text section
    fill_relative_patches(0, 0, rel_patches, &gen);
    at += align_delta(at - stream, 0x100);  // TODO(psv): might be irrelevant

    /*
        .text raw data
    */
    u8* text_ptr = at;
    text_st->ptr_to_raw_data = at - stream;
    // emit text data
    memcpy(at, in_stream, in_stream_size_bytes);
    at += (in_stream_size_bytes);

    text_st->ptr_to_relocations = (at - stream);
    at += write_relocations(at, rel_patches, exports, &text_st->num_of_relocations);

    at += align_delta(at - stream, 0x100); // TODO(psv): might be irrelevant

    // write the coff symbol table
    coff_hdr->ptr_to_symbol_table = (at - stream);
    at += write_symbol_table(at, exports, text_section_number, text_st->size_of_raw_data, text_st->num_of_relocations, &coff_hdr->num_of_symbols);

    at += align_delta(at - stream, 0x100); // TODO(psv): might be irrelevant

    /*
        File write
    */
    FILE* file = fopen("out1.obj", "wb");
    if(!file)
    {
        fprintf(stderr, "Could not open file out1.obj for writing\n");
        return;
    }
    fwrite(stream, 1, at - stream, file);
    fclose(file);
}