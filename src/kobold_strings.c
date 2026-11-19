#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "gild_mem.h"
#include "gild_hash.h"
#include "gild_log.h"
#include "gild_io.h"
#include "gild_string.h"

#include "kobold_strings.h"

// Bump when the string format below changes; old files are then rejected.
#define STR_VER 1
#define STR_MAGIC "KobStrTb"   // exactly 8 characters

static bool write_strtbl(FILE *f, const void *data, void *user);
static void *read_strtbl(FILE *f, void *user);

struct KOB_StrTable *kob_make_strtable(u32 str_count){
	struct Gild_HashMap *map = gild_map_make(str_count);
	if(!map){return NULL;}   // gild_map_make already logged why

	struct KOB_StrTable *tbl = GILD_XCALLOC(1, sizeof(struct KOB_StrTable));
	tbl->table = map;
	return tbl;
}
bool kob_add_strtable(struct KOB_StrTable *tbl, const char *str, const char *key){
	if(!tbl || !str || !key){GILD_LOG(GILD_IS_NULL, "strtbl, str, or key is null"); return false;}

	size_t len = strlen(str);
	if(len > KOB_STR_MAX_LEN){
		GILD_LOG(GILD_INVALID_ARG, "String for key %.15s is too long (%zu bytes, max %u)", key, len, KOB_STR_MAX_LEN);
		return false;
	}

	// The map owns and later free()s its values, so it must get its own copy:
	// `str` may be a string literal or a buffer the caller reuses.
	char *copy = gild_copy_alloc_str(str, len);
	if(!gild_map_add(tbl->table, copy, key)){
		free(copy);   // not added, so it's still ours (gild_map_add logged why)
		return false;
	}
	return true;
}
const char *kob_get_strtable(struct KOB_StrTable *tbl, const char *key){
	if(!tbl || !key){GILD_LOG(GILD_IS_NULL, "strtbl, or key is null"); return NULL;}

	const char *str = NULL;
	if(!gild_map_get(tbl->table, (void *)&str, key)){
		GILD_LOG(GILD_NOT_FOUND, "No string for key %.15s", key);
		return NULL;
	}
	return str;
}
void kob_free_strtable(struct KOB_StrTable *tbl){
	if(!tbl){return;}
	gild_map_free(tbl->table);
	free(tbl);
}

bool kob_save_strtable(struct KOB_StrTable *tbl, const char *path){
	if(!tbl || !path){GILD_LOG(GILD_IS_NULL, "table or path is NULL"); return false;}

	bool success = gild_map_save(tbl->table, path, STR_MAGIC, STR_VER, write_strtbl, NULL);
	if(!success){GILD_LOG(GILD_WRITE, "Failed to write string table to %s", path);}
	return success;
}
struct KOB_StrTable *kob_load_strtable(const char *path){
	if(!path){GILD_LOG(GILD_IS_NULL, "path is NULL"); return NULL;}

	struct Gild_HashMap *map = gild_map_load(path, STR_MAGIC, STR_VER, read_strtbl, NULL);
	if(!map){GILD_LOG(GILD_READ, "Failed to load string table from %s", path); return NULL;}

	struct KOB_StrTable *tbl = GILD_XCALLOC(1, sizeof(struct KOB_StrTable));
	tbl->table = map;
	return tbl;
}

// Each string is stored as: u32 byte length, then the bytes (no '\0').
static bool write_strtbl(FILE *f, const void *data, void *user){
	(void)user;
	const char *str = data;
	size_t len = strlen(str);
	if(len > KOB_STR_MAX_LEN){return false;}
	return gild_write_u32(f, (u32)len) && gild_write_bytes(f, str, len);
}
static void *read_strtbl(FILE *f, void *user){
	(void)user;
	u32 len;
	if(!gild_read_u32(f, &len)){return NULL;}
	// A damaged file could claim a huge length; refuse before allocating.
	if(len > KOB_STR_MAX_LEN){
		GILD_LOG(GILD_CORRUPT, "String length %u in file is over the %u byte limit", len, KOB_STR_MAX_LEN);
		return NULL;
	}

	char *str = GILD_XMALLOC((size_t)len + 1);
	if(!gild_read_bytes(f, str, len)){
		free(str);
		return NULL;
	}
	str[len] = '\0';
	return str;
}
