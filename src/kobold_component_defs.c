#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "gild_io.h"
#include "gild_mem.h"
#include "gild_log.h"
#include "kobold_component_defs.h"

#define MAG "LottaLov"
#define VER 1
#define ARR_GROW 15
static bool write_map(FILE *f, const void *data, void *user) {
	(void)user;
	if (!data) {
		return false;
	}

	u32 idx = *(const u32 *)data;
	return gild_write_u32(f, idx);
}
static void *read_map(FILE *f, void *user) {
	(void)user;

	u32 idx = 0;
	if (!gild_read_u32(f, &idx)) {
	return NULL;
	}

	u32 *val = GILD_XCALLOC(1, sizeof(*val)); 
	*val = idx;
	return val;
}
struct KOB_ComponentDefinitionFile *kob_create_comdef_file(u32 count){
	struct KOB_ComponentDefinitionFile *cdef = GILD_XCALLOC(1, sizeof(struct KOB_ComponentDefinitionFile));
	cdef->defs = GILD_XCALLOC(count, sizeof(u32));
	cdef->_temp_capacity = count;
	cdef->def_count = 0;
	return cdef;
};
bool kob_free_cdef(struct KOB_ComponentDefinitionFile *cdef){
	if(!cdef){GILD_LOG(GILD_IS_NULL, "Cdef is NULL"); return false;}
	if(cdef->defs){free(cdef->defs); cdef->defs = NULL;}
	free(cdef);
	return true;
}
bool kob_add_component(struct KOB_ComponentDefinitionFile *cdef, u32 *out){
	if(!cdef){GILD_LOG(GILD_IS_NULL, "Cdef is NULL"); return false;}
	if(cdef->def_count + 1 >= cdef->_temp_capacity){
		cdef->defs = GILD_XREALLOC(cdef->defs, (cdef->_temp_capacity += ARR_GROW) * sizeof(u32));		
	}

	u32 idx = cdef->def_count;
	cdef->def_count++;
	if(out){*out = idx;}
	return true;
}
bool kob_add_fields(struct KOB_ComponentDefinitionFile *cdef, u32 component, u32 fields){
	if(!cdef){GILD_LOG(GILD_IS_NULL, "Cdef is NULL"); return false;}
	if(component >= cdef->def_count){GILD_LOG(GILD_OUT_OF_BOUNDS, "Component index %d is larger than def_count %d", (int)component, (int)cdef->def_count); return false;}
	cdef->defs[component] |= fields;	
	return true;
}
bool kob_remove_fields(struct KOB_ComponentDefinitionFile *cdef, u32 component, u32 fields) {
	if(!cdef){GILD_LOG(GILD_IS_NULL, "Cdef is NULL"); return false;}
	if(component >= cdef->def_count){GILD_LOG(GILD_OUT_OF_BOUNDS, "Component index %d is larger than def_count %d", (int)component, (int)cdef->def_count); return false;}
	cdef->defs[component] &= ~fields;
	return true;
}
bool kob_write_cdef(struct KOB_ComponentDefinitionFile *cdef, const char *path){
	if(!cdef){GILD_LOG(GILD_IS_NULL, "cdef is NULL"); return false;}
	if(!cdef->defs){GILD_LOG(GILD_IS_NULL, "cdef defintions array is NULL"); return false;}
	if(!path){GILD_LOG(GILD_IS_NULL, "path is NULL"); return false;}
	
	FILE *f = gild_fopen(path, "wb");
	if(!f){GILD_LOG(GILD_NO_FILE, "Failed to open %s", path); return NULL;}
	
	if(!gild_write_header(f, MAG, VER)){
		GILD_LOG(GILD_WRITE, "Failed to write header");
		fclose(f);
		return false;
	}
	if(!gild_write_u32(f, cdef->def_count)){
		GILD_LOG(GILD_WRITE, "Failed to write def count");
		fclose(f);
		return false;
	}
	
	return true;
}
struct KOB_ComponentDefinitionFile *kob_read_cdef(const char *path){
	if(!path){GILD_LOG(GILD_IS_NULL, "path is NULL"); return false;}
	FILE *f = gild_fopen(path, "rb");
	if(!f){GILD_LOG(GILD_NO_FILE, "Failed to open %s", path); return NULL;}
	
	if(!gild_read_header(f, MAG, VER)){
		GILD_LOG(GILD_READ, "Failed header check for %s", path);
		fclose(f);  
		return NULL;
	}
	
	
	u32 temp_def_count;
	if(!gild_read_u32(f, &temp_def_count)){
		GILD_LOG(GILD_READ, "Failed to read def count");
		fclose(f);
		return false;
	}
	
	struct KOB_ComponentDefinitionFile *cdef = GILD_XCALLOC(1, sizeof(struct KOB_ComponentDefinitionFile));
	cdef->def_count = temp_def_count;
	cdef->defs = GILD_XCALLOC(cdef->def_count, sizeof(u32));

	for(u32 i = 0; i < cdef->def_count; i++){
		if(!gild_read_u32(f, &cdef->defs[i])){
			GILD_LOG(GILD_READ, "Failed to read index %d", (int)i);
			fclose(f);
			return false;
		}	
	}
	return cdef;
}
