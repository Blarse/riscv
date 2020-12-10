#include "hashmap.h"


static void hm_destroy_bucket_list(HM_Bucket *bucket);
static size_t hm_hash(HM_Key key);
static HM_Bucket *hm_create_bucket(HM_Key key, HM_Value value);

HM_Map *hm_create_map(size_t slots)
{
	HM_Map *map = (HM_Map *) malloc(sizeof(HM_Map));
	hm_init_map(map, slots);
	return map;
}

void hm_destroy_map(HM_Map *map)
{
	for(int32 i = 0; i < map->slots; ++i)
	{
		if(map->buckets[i] != NULL)
			hm_destroy_bucket_list(map->buckets[i]);
	}
	free(map->buckets);
	free(map);
}


void hm_init_map(HM_Map *map, size_t slots)
{
	map->slots = slots;
	map->buckets = (HM_Bucket **) calloc(slots, sizeof(HM_Bucket));
}

void hm_insert(HM_Map *map, HM_Key key, HM_Value value)
{
	size_t hash = hm_hash(key);
	size_t bucket_index = hash%map->slots;
	if(map->buckets[bucket_index] == NULL)
	{
		map->buckets[bucket_index] = hm_create_bucket(key, value);
	}
	else // chaining
	{
		HM_Bucket *last = map->buckets[bucket_index];
		while(last->next != NULL)
		{
			last = last->next;
		}
		last->next = hm_create_bucket(key, value);
		last->next->prev = last;
	}
}

HM_Value hm_search(HM_Map *map, HM_Key key)
{
	size_t hash = hm_hash(key);
	size_t bucket_index = hash%map->slots;

	HM_Bucket *current = map->buckets[bucket_index];

	if(current == NULL)
		return 0;

	do
	{
		if(!strcmp(key, current->key))
		{
			return current->value;
		}
		current = current->next;
	} while(current != NULL);

	return 0; //TODO: some indication of a search fault
}

HM_Value hm_delete(HM_Map *map, HM_Key key)
{
	size_t hash = hm_hash(key);
	size_t bucket_index = hash%map->slots;

	HM_Bucket *current = map->buckets[bucket_index];

	if(current == NULL)
		return 0;

	do
	{
		if(!strcmp(key, current->key))
		{
			if(current->prev != NULL)
				current->prev->next = current->next;
			else
				map->buckets[bucket_index] = NULL;

			if(current->next != NULL)
				current->next->prev = current->prev;

			HM_Value value = current->value;
			free((char *)current->key);
			free(current);



			return value;
		}
		current = current->next;
	} while(current != NULL);

	return 0; //TODO: some indication of a search fault
}

void hm_print_map(HM_Map *map)
{
	printf("Hash Map:\n");
	for(size_t i = 0; i < map->slots; ++i)
	{
		if(map->buckets[i] != NULL)
		{
			HM_Bucket *current = map->buckets[i];
			printf("[%lu]: ", i);
			do
			{
				printf("\t - '%s':%u; ", current->key, current->value);
				current = current->next;
			} while(current != NULL);

			printf("\n");
		}

	}
}

static void hm_destroy_bucket_list(HM_Bucket *bucket)
{
	HM_Bucket *last = bucket;
	while(last->next != NULL)
	{
		last = last->next;
	}

	while(last->prev != NULL)
	{
		last = last->prev;
		free((char *)last->next->key);
		free(last->next);
	}

	free((char *)last->key);
	free(last);
}

static HM_Bucket *hm_create_bucket(HM_Key key, HM_Value value)
{
	HM_Bucket *bucket = (HM_Bucket *) malloc(sizeof(HM_Bucket));
	bucket->next = NULL;
	bucket->prev = NULL;
	bucket->value = value;
	size_t key_len = strlen(key) + 1;
	bucket->key = (HM_Key) malloc(key_len * sizeof(key[0]));

	memcpy((char *)bucket->key, key, key_len);

	return bucket;
}

//NOTE: Placeholder hash function
//TODO: Change hash function
static uint64 ipow(uint64 b, uint64 e)
{
	uint64 r = b;
	for(uint64 i = 1; i < e; ++i)
		r *= b;
	return r;
}

static size_t hm_hash(HM_Key key)
{
	size_t hash = 0;
	size_t index = 0;
	while(key[index] != '\0')
	{
		hash += key[index] * ipow(128, index);
		index++;
	}
	return hash;
}
