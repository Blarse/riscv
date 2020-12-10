#if !defined(HASHMAP_H)
#define HASHMAP_H

typedef const char*  HM_Key;
typedef uint32 HM_Value;

typedef struct HM_Bucket
{
	struct HM_Bucket *next;
	struct HM_Bucket *prev;
	HM_Key key;
	HM_Value value;
} HM_Bucket;

typedef struct HM_Map
{
	size_t slots;
	HM_Bucket **buckets;
} HM_Map;

HM_Map *hm_create_map(size_t slots);
void hm_destroy_map(HM_Map *map);
void hm_init_map(HM_Map *map, size_t slots);

void hm_insert(HM_Map *map, HM_Key key, HM_Value value);
HM_Value hm_search(HM_Map *map, HM_Key key);
HM_Value hm_delete(HM_Map *map, HM_Key key);

void hm_print_map(HM_Map *map);

#endif
