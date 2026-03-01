#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#define bool _Bool

typedef void (*_MVS_Deleter)(uintptr_t);

typedef struct HeapMemory
{
    size_t ref_count;
    size_t size_in_bytes;
    _MVS_Deleter deleter;
} HeapMemory;

typedef struct MVSNode
{
    uintptr_t key;
    HeapMemory value;
    struct MVSNode* next;
} MVSNode;

typedef struct
{
    MVSNode** buckets;
    size_t capacity;
    size_t size;
} MVSHashMap;

static MVSHashMap mu;

static size_t _hash_address(uintptr_t key_address, size_t capacity)
{
    return (key_address * 2654435761u) % capacity;
}

static void _resize()
{
    if ((double)mu.size / mu.capacity < 0.75) { return; }

    size_t new_capacity = mu.capacity * 2;
    MVSNode** new_buckets = calloc(new_capacity, sizeof(MVSNode*));

    for (size_t i = 0; i < mu.capacity; i++)
    {
        MVSNode* current = mu.buckets[i];

        while (current)
        {
            MVSNode* next = current->next;

            size_t new_index = _hash_address(current->key, new_capacity);

            current->next = new_buckets[new_index];
            new_buckets[new_index] = current;

            current = next;
        }
    }

    free(mu.buckets);
    mu.buckets = new_buckets;
    mu.capacity = new_capacity;
}

void MVS_Init()
{
    mu.capacity = 16;
    mu.size = 0;
    mu.buckets = calloc(mu.capacity, sizeof(MVSNode*));
}

void MVS_Destroy()
{
    for (size_t i = 0; i < mu.capacity; i++)
    {
        MVSNode* current = mu.buckets[i];

        while (current)
        {
            MVSNode* next = current->next;
            free(current);
            current = next;
        }
    }

    free(mu.buckets);
}

static MVSNode* _find_node(uintptr_t address)
{
    size_t index = _hash_address(address, mu.capacity);
    MVSNode* current = mu.buckets[index];

    while (current)
    {
        if (current->key == address)
            return current;

        current = current->next;
    }

    return NULL;
}

size_t MVS_RefCount(uintptr_t address)
{
    MVSNode* node = _find_node(address);

    if (!node) { return 0; }

    return node->value.ref_count;
}

void MVS_RegisterNew(uintptr_t address, size_t size, _MVS_Deleter deleter)
{
    MVSNode* node = _find_node(address);

    if (node)
    {
        node->value.ref_count++;
        return;
    }

    _resize();

    size_t index = _hash_address(address, mu.capacity);

    MVSNode* new_node = malloc(sizeof(MVSNode));
    new_node->key = address;
    new_node->value.ref_count = 1;
    new_node->value.size_in_bytes = size;
    new_node->value.deleter = deleter;

    new_node->next = mu.buckets[index];
    mu.buckets[index] = new_node;

    mu.size++;
}

bool MVS_DetachPointer(uintptr_t address)
{
    size_t index = _hash_address(address, mu.capacity);

    MVSNode* current = mu.buckets[index];
    MVSNode* prev = NULL;

    while (current)
    {
        if (current->key == address)
        {
            if (current->value.ref_count > 1)
            {
                current->value.ref_count--;
                return 0;
            }
            if (current->value.ref_count == 0)
            {
                return 0;
            }

            current->value.deleter(address);

            if (prev) { prev->next = current->next; }
            else { mu.buckets[index] = current->next; }

            free(current);
            mu.size--;

            return 1;
        }

        prev = current;
        current = current->next;
    }

    return 0;
}

bool MVS_SameLoc(uintptr_t a, uintptr_t b)
{
    return a == b;
}

typedef struct prop prop;
void xx_free_prop(uintptr_t address);

typedef struct opt1 opt1;

typedef struct opt2 opt2;

typedef struct opt3 opt3;

typedef struct opt4 opt4;

typedef enum prop_Tag {
	opt1_tag, opt2_tag, opt3_tag, opt4_tag, 
} prop_Tag;

typedef union prop_Opts {
	opt1* opt1_opt;
	opt2* opt2_opt;
	opt3* opt3_opt;
	opt4* opt4_opt;
} prop_Opts;

typedef struct prop {
	prop_Tag tag;
	prop_Opts opts;
} prop;

bool xx_prop_option_is_opt1(const prop* prop)
{
	return prop->tag == opt1_tag;
}

bool xx_prop_option_is_opt2(const prop* prop)
{
	return prop->tag == opt2_tag;
}

bool xx_prop_option_is_opt3(const prop* prop)
{
	return prop->tag == opt3_tag;
}

bool xx_prop_option_is_opt4(const prop* prop)
{
	return prop->tag == opt4_tag;
}

void xx_free_prop(uintptr_t address)
{
	if ((((prop*)address)->tag == opt1_tag) && (((prop*)address)->opts.opt1_opt != NULL)) 
	{
		MVS_DetachPointer((uintptr_t)((prop*)address)->opts.opt1_opt);
	}
	if ((((prop*)address)->tag == opt2_tag) && (((prop*)address)->opts.opt2_opt != NULL)) 
	{
		MVS_DetachPointer((uintptr_t)((prop*)address)->opts.opt2_opt);
	}
	if ((((prop*)address)->tag == opt3_tag) && (((prop*)address)->opts.opt3_opt != NULL)) 
	{
		MVS_DetachPointer((uintptr_t)((prop*)address)->opts.opt3_opt);
	}
	if ((((prop*)address)->tag == opt4_tag) && (((prop*)address)->opts.opt4_opt != NULL)) 
	{
		MVS_DetachPointer((uintptr_t)((prop*)address)->opts.opt4_opt);
	}
	free(((prop*)address));
}

void xx_free_opt1(uintptr_t address)
{
}

opt1* xx_init_opt1()
{
	return NULL;
}

void xx_free_opt2(uintptr_t address)
{
}

opt2* xx_init_opt2()
{
	return NULL;
}

void xx_free_opt3(uintptr_t address)
{
}

opt3* xx_init_opt3()
{
	return NULL;
}

void xx_free_opt4(uintptr_t address)
{
}

opt4* xx_init_opt4()
{
	return NULL;
}

prop* xx_init_prop_opt1(opt1* opt1_opt)
{
	prop* xx_init_prop_ptr = malloc(sizeof(prop));
	xx_init_prop_ptr->tag = opt1_tag;
	xx_init_prop_ptr->opts.opt1_opt = opt1_opt;
	return xx_init_prop_ptr;
}

prop* xx_init_prop_opt2(opt2* opt2_opt)
{
	prop* xx_init_prop_ptr = malloc(sizeof(prop));
	xx_init_prop_ptr->tag = opt2_tag;
	xx_init_prop_ptr->opts.opt2_opt = opt2_opt;
	return xx_init_prop_ptr;
}

prop* xx_init_prop_opt3(opt3* opt3_opt)
{
	prop* xx_init_prop_ptr = malloc(sizeof(prop));
	xx_init_prop_ptr->tag = opt3_tag;
	xx_init_prop_ptr->opts.opt3_opt = opt3_opt;
	return xx_init_prop_ptr;
}

prop* xx_init_prop_opt4(opt4* opt4_opt)
{
	prop* xx_init_prop_ptr = malloc(sizeof(prop));
	xx_init_prop_ptr->tag = opt4_tag;
	xx_init_prop_ptr->opts.opt4_opt = opt4_opt;
	return xx_init_prop_ptr;
}



int main()
{
	MVS_Init();

	prop* p = xx_init_prop_opt1(xx_init_opt1());
	MVS_RegisterNew((uintptr_t)p, sizeof(prop), xx_free_prop);
	if (xx_prop_option_is_opt1(p))
	{
	}
	if (xx_prop_option_is_opt2(p))
	{
	}
	if (xx_prop_option_is_opt3(p))
	{
	}
	if (xx_prop_option_is_opt4(p))
	{
	}
/* HERE SHOULD BE STRING INIT */
	MVS_Destroy();
}
