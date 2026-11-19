#pragma once

#include "gild_basic.h"
#include "gild_hash.h"

#ifdef __cplusplus
extern "C" {
#endif

#define KOB_STR_MAX_LEN (1u << 20)   // 1 MiB per string

struct KOB_StrTable{
	struct Gild_HashMap *table;
};

struct KOB_StrTable *kob_make_strtable(u32 str_count);
bool kob_add_strtable(struct KOB_StrTable *tbl, const char *str, const char *key);
const char *kob_get_strtable(struct KOB_StrTable *tbl, const char *key);
void kob_free_strtable(struct KOB_StrTable *tbl);

bool kob_save_strtable(struct KOB_StrTable *tbl, const char *path);
struct KOB_StrTable *kob_load_strtable(const char *path);

#ifdef __cplusplus
}
#endif
