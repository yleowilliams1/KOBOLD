#pragma once

#include "gild_basic.h"

#define KOB_BASIC_FIELD_TYPES_LIST\
	X(u8_t, u8)\
	X(u16_t, u16)\
	X(u32_t, u32)\
	X(u64_t, u64)\
	X(s8_t, s8)\
	X(s16_t, s16)\
	X(s32_t, s32)\
	X(s64_t, s64)\

static const u8 KOB_FIELD_SIZE[] = {
        #define X(name, type) sizeof(type),
        KOB_BASIC_FIELD_TYPES_LIST
        #undef X
};
enum KOB_FieldType{
	#define X(name, type) FIELD_##name,
	KOB_BASIC_FIELD_TYPES_LIST
	#undef X
	KOB_FIELD_COUNT
};
struct KOB_ComponentDefinitionFile{
	u32 def_count;
	// We don't write this and just set it to def_count when read 
	u32 _temp_capacity;
/*A definition is a list of fields. There can only be four of a single type, and only 32 fields. This is just a u32, with every four strings of 8 bits telling you which type it uses. bit zero of eight being the first type in the enum, and bit seven being the seventh type in the enum, repeated over four times.*/
	u32 *defs;
};
struct KOB_ComponentDefinitionFile *kob_create_comdef_file(u32 count);
bool kob_free_cdef(struct KOB_ComponentDefinitionFile *cdef);

bool kob_add_component(struct KOB_ComponentDefinitionFile *cdef, u32 *out);
bool kob_add_fields(struct KOB_ComponentDefinitionFile *cdef, u32 component, u32 fields);
bool kob_remove_fields(struct KOB_ComponentDefinitionFile *cdef, u32 component, u32 fields);

bool kob_write_cdef(struct KOB_ComponentDefinitionFile *cdef, const char *path);
struct KOB_ComponentDefinitionFile *kob_read_cdef(const char *path);
