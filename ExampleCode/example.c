#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

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

_Bool MVS_DetachPointer(uintptr_t address)
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

_Bool MVS_SameLoc(uintptr_t a, uintptr_t b)
{
    return a == b;
}

typedef struct Example {
	int a;
	double b;
	char c;
} Example;

void xx_free_Example(uintptr_t address)
{
	free(((Example*)address));
}

typedef struct Example2 {
	int a;
	Example* e1;
} Example2;

void xx_free_Example2(uintptr_t address)
{
	if (((Example2*)address)->e1 != NULL)
	{
		xx_free_Example((uintptr_t)((Example2*)address)->e1);
	}
	free(((Example2*)address));
}

typedef struct Example3 {
	int a;
	Example2* e2;
	struct Example3* e3;
} Example3;

void xx_free_Example3(uintptr_t address)
{
	if (((Example3*)address)->e2 != NULL)
	{
		xx_free_Example2((uintptr_t)((Example3*)address)->e2);
	}
	if (((Example3*)address)->e3 != NULL)
	{
		xx_free_Example3((uintptr_t)((Example3*)address)->e3);
	}
	free(((Example3*)address));
}



int main()
{
	MVS_Init();

	Example* e = malloc(sizeof(Example));
	e = NULL;
	MVS_RegisterNew((uintptr_t)e, sizeof(Example), xx_free_Example);

	int a = 7;
	MVS_Destroy();
}
