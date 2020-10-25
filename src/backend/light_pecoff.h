#pragma once
#include <common.h>
#include "x86/x86.h"

#pragma pack(push)
#pragma pack(1)

typedef struct {
    u8  signature[2];
    u8  dos_program[58];
    u32 ptr_to_pe_header;
} DOS_Header;

typedef struct {
	u8 p;		// P
	u8 e;		// E
	u16 nil;	// must be null
} PE_Signature;

typedef struct {
	PE_Signature signature;		//	Image only
	u16 machine;				//	The number that identifies the type of target machine.
	u16 num_sections;			//	The number of sections. This indicates the size of the section table, which immediately follows the headers.
	u32 time_stamp;				//	The low 32 bits of the number of seconds since 00:00 January 1, 1970 (a C run-time time_t value), 
								//	that indicates when the file was created.
	u32 ptr_to_symbol_table;	//	The file offset of the COFF symbol table, or zero if no COFF symbol table is present. 
								//	This value should be zero for an image because COFF debugging information is deprecated.
	u32 num_of_symbols;			//	The number of entries in the symbol table. This data can be used to locate the string table, 
								//	which immediately follows the symbol table. This value should be zero for an image because COFF debugging information is deprecated.
	u16 size_optional_header;	//	The size of the optional header, which is required for executable files but not for object files. 
								//	This value should be zero for an object file.
	u16 characteristics;		//	The flags that indicate the attributes of the file. IMAGE_FILE_EXECUTABLE_IMAGE
} Coff_Header;

typedef struct {
	u16 magic;					//	The unsigned integer that identifies the state of the image file. 
								//	The most common number is 0x10B, which identifies it as a normal executable file. 
								//	0x107 identifies it as a ROM image, and 0x20B identifies it as a PE32+ executable.
	u8 maj_linker_version;
	u8 min_linker_version;
	u32 size_of_code;			//	The size of the code (text) section, or the sum of all code sections if there are multiple sections.
	u32 size_of_init_data;		//	The size of the initialized data section, or the sum of all such sections if there are multiple data sections.
	u32 size_of_uninit_data;	//	The size of the uninitialized data section (BSS), or the sum of all such sections if there are multiple BSS sections.

	u32 address_of_entry;		//	The address of the entry point relative to the image base when the executable file is loaded into memory. 
								//	For program images, this is the starting address. For device drivers, this is the address of the initialization function. 
								//	An entry point is optional for DLLs. When no entry point is present, this field must be zero.

	u32 base_of_code;			//	The address that is relative to the image base of the beginning-of-code section when it is loaded into memory.
	
	u32 base_of_data;			//	(PE32 only)	The address that is relative to the image base of the beginning-of-data section when it is loaded into memory.
} Optional_Header;

typedef struct {
	u64 image_base_pe32p;		//	The preferred address of the first byte of image when loaded into memory; must be a multiple of 64 K. 
								//	The default for DLLs is 0x10000000. The default for Windows CE EXEs is 0x00010000. 
								//	The default for Windows NT, Windows 2000, Windows XP, Windows 95, Windows 98, and Windows Me is 0x00400000.

	u32 section_alignment;		//	The alignment (in bytes) of sections when they are loaded into memory. 
								//	It must be greater than or equal to FileAlignment. The default is the page size for the architecture.

	u32 file_alignment;			//	The alignment factor (in bytes) that is used to align the raw data of sections in the image file. 
								//	The value should be a power of 2 between 512 and 64 K, inclusive. The default is 512. 
								//	If the SectionAlignment is less than the architecture�s page size, then FileAlignment must match SectionAlignment.
	u16 maj_os_version;
	u16 min_os_version;
	u16 maj_image_version;
	u16 min_image_version;
	u16 maj_subsystem_version;
	u16 min_subsystem_version;

	u32 win32_version_value;	//	Reserved. Must be zero
	u32 size_of_image;			//	The size (in bytes) of the image, including all headers, as the image is loaded in memory. 
								//	It must be a multiple of SectionAlignment.
	u32 size_of_headers;		//	The combined size of an MS DOS stub, PE header, and section headers rounded up to a multiple of FileAlignment.
	u32 check_sum;				//	The image file checksum. The algorithm for computing the checksum is incorporated into IMAGHELP.DLL. 
								//	The following are checked for validation at load time: all drivers, any DLL loaded at boot time, 
								//	and any DLL that is loaded into a critical Windows process.

	u16 subsystem;				//	The subsystem that is required to run this image. For more information, see �Windows Subsystem� later in this specification.
	u16 dll_characteristics;

	// differs here between PE32+ and PE32
	u64 size_stack_reserve_pe32p;		//	The size of the stack to reserve. Only SizeOfStackCommit is committed; 
										//	the rest is made available one page at a time until the reserve size is reached.
	u64 size_stack_commit_pe32p;
	u64 size_heap_reserve_pe32p;		//	The size of the local heap space to reserve. Only SizeOfHeapCommit is committed; 
										//	the rest is made available one page at a time until the reserve size is reached.
	u64 size_heap_commit_pe32p;

	u32 loader_flags;					//	Reserved. Must be zero
	u32 num_of_rva_and_sizes;			//	The number of data-directory entries in the remainder of the optional header. Each describes a location and size.

} Optional_Image_Only_PE32PLUS;

typedef struct {

	u32 image_base_pe32;		//	The preferred address of the first byte of image when loaded into memory; must be a multiple of 64 K. 
								//	The default for DLLs is 0x10000000. The default for Windows CE EXEs is 0x00010000. 
								//	The default for Windows NT, Windows 2000, Windows XP, Windows 95, Windows 98, and Windows Me is 0x00400000.

	u32 section_alignment;		//	The alignment (in bytes) of sections when they are loaded into memory. 
								//	It must be greater than or equal to FileAlignment. The default is the page size for the architecture.

	u32 file_alignment;			//	The alignment factor (in bytes) that is used to align the raw data of sections in the image file. 
								//	The value should be a power of 2 between 512 and 64 K, inclusive. The default is 512. 
								//	If the SectionAlignment is less than the architecture�s page size, then FileAlignment must match SectionAlignment.
	u16 maj_os_version;
	u16 min_os_version;
	u16 maj_image_version;
	u16 min_image_version;
	u16 maj_subsystem_version;
	u16 min_subsystem_version;
	
	u32 win32_version_value;	//	Reserved. Must be zero
	u32 size_of_image;			//	The size (in bytes) of the image, including all headers, as the image is loaded in memory. 
								//	It must be a multiple of SectionAlignment.
	u32 size_of_headers;		//	The combined size of an MS DOS stub, PE header, and section headers rounded up to a multiple of FileAlignment.
	u32 check_sum;				//	The image file checksum. The algorithm for computing the checksum is incorporated into IMAGHELP.DLL. 
								//	The following are checked for validation at load time: all drivers, any DLL loaded at boot time, 
								//	and any DLL that is loaded into a critical Windows process.

	u16 subsystem;				//	The subsystem that is required to run this image. For more information, see �Windows Subsystem� later in this specification.
	u16 dll_characteristics;

	// differs here between PE32+ and PE32
	u32 size_stack_reserve_pe32;
	u32 size_stack_commit_pe32;
	u32 size_heap_reserve_pe32;
	u32 size_heap_commit_pe32;

	u32 loader_flags;					//	Reserved. Must be zero
	u32 num_of_rva_and_sizes;			//	The number of data-directory entries in the remainder of the optional header. Each describes a location and size.
} Optional_Image_Only_PE32;

typedef struct {
	u32 virtual_address;
	u32 size;
} Image_Data_Dir;

typedef struct {
	Image_Data_Dir export_table;				//	The export table address and size. For more information see section 6.3, �The .edata Section (Image Only).�
	Image_Data_Dir import_table;				//	The import table address and size. For more information, see section 6.4, �The .idata Section.�
	Image_Data_Dir resource_table;				//	The resource table address and size. For more information, see section 6.9, �The .rsrc Section.�
	Image_Data_Dir exception_table;				//	The exception table address and size. For more information, see section 6.5, �The .pdata Section.�
	Image_Data_Dir certificate_table;			//	The attribute certificate table address and size. �The Attribute Certificate Table (Image Only).�
	Image_Data_Dir base_relocation_table;		//	The base relocation table address and size. For more information, see section 6.6, "The .reloc Section (Image Only)."
	Image_Data_Dir debug;						//	The debug data starting address and size. For more information, see section 6.1, �The .debug Section.�
	Image_Data_Dir architecture;				//	Reserved, must be zero
	Image_Data_Dir global_ptr;					//	The RVA of the value to be stored in the global pointer register. 
												//	The size member of this structure must be set to zero. 
	Image_Data_Dir tls_table;					//	The thread local storage (TLS) table address and size. For more information, see section 6.7, �The .tls Section.�
	Image_Data_Dir load_config_table;			//	The load configuration table address and size. �The Load Configuration Structure (Image Only).�
	Image_Data_Dir bound_import;				//	The bound import table address and size. 
	Image_Data_Dir import_address_table;		//	The import address table address and size. �Import Address Table.�
	Image_Data_Dir delay_import_descriptor;		//	The delay import descriptor address and size. �Delay-Load Import Tables (Image Only).�
	Image_Data_Dir clr_runtime_header;			//	The CLR runtime header address and size. �The .cormeta Section (Object Only).�
	Image_Data_Dir reserved_zero;
} Optional_Header_DataDir;

typedef struct {
	u8 name[8];									//	An 8-byte, null-padded UTF-8 encoded string. If the string is exactly 8 characters long, 
												//	there is no terminating null. For longer names, this field contains a slash (/) that is followed by an ASCII 
	//	representation of a decimal number that is an offset into the string table. 
	//	Executable images do not use a string table and do not support section names longer than 8 characters. 
	//	Long names in object files are truncated if they are emitted to an executable file.

	u32 virtual_size;							//	The total size of the section when loaded into memory. If this value is greater than SizeOfRawData, 
												//	the section is zero-padded. This field is valid only for executable images and should be set to zero 
												//	for object files.

	u32 virtual_address;						//	For executable images, the address of the first byte of the section relative to the image base 
												//	when the section is loaded into memory. For object files, this field is the address of the first byte before 
												//	relocation is applied; for simplicity, compilers should set this to zero. Otherwise, it is an arbitrary value 
												//	that is subtracted from offsets during relocation.

	u32 size_of_raw_data;						//	The size of the section (for object files) or the size of the initialized data on disk (for image files). 
												//	For executable images, this must be a multiple of FileAlignment from the optional header. 
												//	If this is less than VirtualSize, the remainder of the section is zero-filled. 
												//	Because the SizeOfRawData field is rounded but the VirtualSize field is not, it is possible for SizeOfRawData 
												//	to be greater than VirtualSize as well. When a section contains only uninitialized data, this field should be zero.

	u32 ptr_to_raw_data;						//	The file pointer to the first page of the section within the COFF file. For executable images, this must be a
												//	multiple of FileAlignment from the optional header. For object files, the value should be aligned on a 4 byte 
												//	boundary for best performance. When a section contains only uninitialized data, this field should be zero.

	u32 ptr_to_relocations;						//	The file pointer to the beginning of relocation entries for the section. 
												//	This is set to zero for executable images or if there are no relocations.

	u32 ptr_to_line_numbers;					//	The file pointer to the beginning of line-number entries for the section. This is set to zero if there are no COFF
												//	line numbers. This value should be zero for an image because COFF debugging information is deprecated.

	u16 num_of_relocations;						//	The number of relocation entries for the section. This is set to zero for executable images.

	u16 num_of_line_numbers;					//	The number of line-number entries for the section. 
												//	This value should be zero for an image because COFF debugging information is deprecated.

	u32 characteristics;						//	The flags that describe the characteristics of the section. �Section Flags.�
} Section_Table;

typedef struct {
	u32 page_rva;
	u32 block_size;
	//u16* type_and_offset;						// high 4 bits type -> low 12 bits offset
} Base_Relocation_Block;

typedef struct {
	u16 hint;						// An index into the export name pointer table. A match is attempted 
									// first with this value. If it fails, a binary search is performed 
									// on the DLL’s export name pointer table.
	u8 name[0];						// An ASCII string that contains the name to import. This is the string 
									// that must be matched to the public name in the DLL. This string is 
									// case sensitive and terminated by a null byte.

	// A trailing zero-pad byte that appears after the trailing null byte, if necessary, to align the next entry on an even boundary
} Hint_Name_Table;

typedef struct {
	//u8 entries[24];
	u32 ordinal_number    : 16;
	u32 unused            : 15;
	u32 ordinal_name_flag : 1;
	u32 hint_name_table_rva;
} Import_Lookup_Table_PE32;

typedef struct {
	u32 import_lookup_table;		// The RVA of the import lookup table. This table contains a name or ordinal for each import. 
									// (The name “Characteristics” is used in Winnt.h, but no longer describes this field.)
	u32 time_date_stamp;			// The stamp that is set to zero until the image is bound. After the image is bound, 
									// this field is set to the time/data stamp of the DLL.
	u32 forwarder_chain;
	u32 name_rva;					// The address of an ASCII string that contains the name of the DLL. 
									// This address is relative to the image base.
	u32 import_address_table;		// The RVA of the import address table. The contents of this table are 
									// identical to the contents of the import lookup table until the image is bound
} Import_Directory_Table;

typedef struct {
	char name[8];
	int value;
	short section_number;
	short type;
	u8 storage_class;
	u8 number_of_aux_symbols;
} Symbol_Table_Entry;

typedef struct {
	struct PE_Entry {
		u16 offset : 12;
		u16 type   : 4;
	} entry;
	int offset32;
} Relocation_Entry;

#pragma pack(pop)

void
light_pecoff_emit(u8* in_stream, int in_stream_size_bytes, int entry_point_offset, 
    X86_Patch* rel_patches, X86_Data* data_seg, X86_Import* imports, 
    X86_DataSeg_Patch* dseg_patches, IR_Data_Segment_Entry* dseg_entries);