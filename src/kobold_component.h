#pragma once
#include "gild_basic.h"

#define MAX_FIELD_COUNT 32
#define SAME_SIZE_FIELD_COUNT 4

struct KOB_FieldHandle{
	u16 idx;
	u16 gen;
	u8 type;
};
struct KOB_ComponentHandle{
	// We can sort here by type which is cool!
	struct KOB_FieldHandle fields[MAX_FIELD_COUNT];
};

// This stores instances of components
struct KOB_ComponentStore{
	u16 gen_count;

	u16 *byte_arena;
	u16 *word_arena;
	u32 *dword_arena;
	u64 *qword_arena;
	
	u16 *arena_gens[SAME_SIZE_FIELD_COUNT];
	u16 arena_count[SAME_SIZE_FIELD_COUNT];
	u16 arena_allocated[SAME_SIZE_FIELD_COUNT];
	u16 next_free[SAME_SIZE_FIELD_COUNT];
};

#define KOB_TYPE_FOR_PTR(ptr) _Generic((ptr), \
	u8  *: U8,  \
	u16 *: U16, \
	u32 *: U32, \
	u64 *: U64, \
	default: (u8)0xFF \
)

struct KOB_ComponentDefinitionFile;

struct KOB_ComponentStore *kob_create_cstore();
bool kob_free_instance(struct KOB_ComponentStore *cs, struct KOB_ComponentHandle handle);
bool kob_file_instantiate(struct KOB_ComponentStore *cs, struct KOB_ComponentDefinitionFile *file, const char *name);

bool kob_instantiate(struct KOB_ComponentStore *cs, u32 definition, struct KOB_ComponentHandle *out);

void *kob_impl_get_field_checked(struct KOB_ComponentStore *cs, struct KOB_FieldHandle handle, u8 expected_type);
#define kob_get_field(cs, handle, out_ptr) ((out_ptr) = (__typeof__(out_ptr))kob_impl_get_field_checked((cs), (handle), KOB_TYPE_FOR_PTR(out_ptr)))
