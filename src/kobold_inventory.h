#pragma once

#include "gild_hash.h"
#include "kobold_component.h"

// Item has it's own ecs for it's stuff.
struct KOB_ItemPrototype{
	u32 first_component;
	u16 component_count;
	u16 _pad;
};
// This doesn't change, it's written into memory once and then loaded.
struct KOB_ItemManagerDead{
	
	struct KOB_ItemPrototype *prototypes;
	struct KOB_ComponentStore *data;
	struct ComponentRef *component_ref_arena;
	
	u32 item_count;
};


struct KOB_InventoryItem{
	u16 prototype_index;
	u16 stack_count;
};
struct KOB_Inventory{
	struct KOB_InventoryItem *item_indexes;
	u32 item_count;	
};
