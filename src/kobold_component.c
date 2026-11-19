#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "gild_log.h"
#include "gild_mem.h"
#include "kobold_component_defs.h"
#include "kobold_component.h"

#define U8 0
#define U16 1
#define U32 2
#define U64 3
struct KOB_ComponentStore *kob_create_cstore(){
	struct KOB_ComponentStore *store = GILD_XCALLOC(1, sizeof(struct KOB_ComponentStore));
	memset(store->next_free, 0xFF, sizeof(store->next_free));	
	return store;
}
bool kob_instantiate(struct KOB_ComponentStore *cs, u32 definition, struct KOB_ComponentHandle *out){
	if(!cs){GILD_LOG(GILD_IS_NULL, "Component store is NULL"); return false;}
	
	struct KOB_ComponentHandle handle = {};

	// Loop t hrough the 32 bits of defintion
	for (int i = 0; i < 32; i++){
		// Check if the bit is flipped
		if (!(definition & (1U << i))){continue;}
		u8 type = (i & 7) & 3;
		
		void *arena = NULL;
		switch(type){
			case U8:arena = cs->byte_arena;break;
			case U16:arena = cs->word_arena;break;
			case U32:arena = cs->dword_arena;break;
			case U64:arena = cs->qword_arena; break;
		}
		
		// Grow slightly if exceeding
		if(cs->arena_count[type] + 1 >= cs->arena_allocated[type]){	
			u32 old_allocated = cs->arena_allocated[type];
			u32 growth = (old_allocated == 0) ? 32 : (old_allocated / 4); 
			u32 new_allocated = old_allocated + growth;
			u32 new_size = new_allocated * KOB_FIELD_SIZE[type];

			arena = GILD_XREALLOC(arena, new_size);

			cs->arena_gens[type] = (u16 *)GILD_XREALLOC(cs->arena_gens[type], new_allocated * sizeof(u16));
			memset(cs->arena_gens[type] + old_allocated, 0, growth * sizeof(u16));

			switch(type){
				case U8: cs->byte_arena  = arena; break;
				case U16: cs->word_arena  = arena; break;
				case U32: cs->dword_arena = arena; break;
				case U64: cs->qword_arena = arena; break;
			}

			cs->arena_allocated[type] = new_allocated;
		}
		
		u16 idx;

		// No free slots so use end slot
		if(cs->next_free[type] == UINT16_MAX){
			idx = cs->arena_count[type];
			cs->arena_count[type]++;	
		} else{
			idx = cs->next_free[type];
			// This actually will hold garbage value btw
			cs->next_free[type] = (u16)((u16 *)arena)[cs->next_free[type]];
		}
		handle.fields[i].idx = idx;
		handle.fields[i].gen = ++cs->gen_count;
		handle.fields[i].type = type;
		cs->arena_gens[type][idx] = handle.fields[i].gen;
	}
	
	if(out){*out = handle;}
	return true;
}
bool kob_free_instance(struct KOB_ComponentStore *cs, struct KOB_ComponentHandle handle){
	if(!cs){GILD_LOG(GILD_IS_NULL, "Component store is NULL"); return false;}
	for(u32 i = 0; i < MAX_FIELD_COUNT; i++){
		if(handle.fields[i].type > 3){continue;}
		if(handle.fields[i].idx >= cs->arena_count[handle.fields[i].type]){continue;}
		if(handle.fields[i].gen != cs->arena_gens[handle.fields[i].type][handle.fields[i].idx]){continue;}
		
		// Handle is valid! yipee
		// invalidate the generation
		cs->arena_gens[handle.fields[i].type][handle.fields[i].idx] = 0;

		u16 *arena_as_free_list = NULL;
		switch(handle.fields[i].type) {
			case U8:  arena_as_free_list = (u16 *)cs->byte_arena; break;
			case U16: arena_as_free_list = (u16 *)cs->word_arena; break;
			case U32: arena_as_free_list = (u16 *)cs->dword_arena; break;
			case U64: arena_as_free_list = (u16 *)cs->qword_arena; break;
		}
			
		// push free list
		arena_as_free_list[handle.fields[i].idx] = cs->next_free[handle.fields[i].type];
		cs->next_free[handle.fields[i].type] = handle.fields[i].idx;
	}
	return true;
}
void *kob_impl_get_field_checked(struct KOB_ComponentStore *cs, struct KOB_FieldHandle handle, u8 expected_type){
	if(!cs){GILD_LOG(GILD_IS_NULL, "Component store is NULL"); return NULL;}
	if(handle.type != expected_type){return NULL;}
	if(handle.type > 3){return NULL;}
	if(handle.idx >= cs->arena_count[handle.type]){return NULL;}
	if(handle.gen != cs->arena_gens[handle.type][handle.idx]){return NULL;}

	switch(handle.type) {
		case U8:  return (void *)(&cs->byte_arena[handle.idx]);
		case U16: return (void *)(&cs->word_arena[handle.idx]);
		case U32: return (void *)(&cs->dword_arena[handle.idx]);
		case U64: return (void *)(&cs->qword_arena[handle.idx]);
		default:  return NULL;
	}
}
