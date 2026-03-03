#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#define bool _Bool
#define uint uint32_t

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

typedef struct string {
    size_t length;
    char* data;
} string;

void xx_free_string(uintptr_t address)
{
    string* s = (string*)address;

    if (s->data != NULL)
        free(s->data);

    free(s);
}

void xx_string_set(string* s, size_t index, char value)
{
    if (!s) return;
    if (index >= s->length) return;

    s->data[index] = value;
}

string* xx_init_string(const char* literal)
{
    string* s = malloc(sizeof(string));

    s->length = strlen(literal);
    s->data = malloc(s->length + 1);

    memcpy(s->data, literal, s->length + 1);

    return s;
}

string* xx_string_add(string* a, bool a_temp,
    string* b, bool b_temp)
{
    if (!a || !b) return NULL;

    string* result = malloc(sizeof(string));

    result->length = a->length + b->length;
    result->data = malloc(result->length + 1);

    memcpy(result->data, a->data, a->length);
    memcpy(result->data + a->length, b->data, b->length);

    result->data[result->length] = '\0';

    // free temporaries AFTER usage
    if (a_temp)
        xx_free_string((uintptr_t)a);

    if (b_temp)
        xx_free_string((uintptr_t)b);

    return result;
}

string* xx_string_sub_right(string* s, bool s_temp, size_t n)
{
    if (!s) return NULL;

    string* result;

    if (n >= s->length)
    {
        result = xx_init_string("");
    }
    else
    {
        size_t new_len = s->length - n;

        result = malloc(sizeof(string));
        result->length = new_len;
        result->data = malloc(new_len + 1);

        memcpy(result->data, s->data, new_len);
        result->data[new_len] = '\0';
    }

    if (s_temp)
        xx_free_string((uintptr_t)s);

    return result;
}

string* xx_string_sub_left(size_t n, string* s, bool s_temp)
{
    if (!s) return NULL;

    string* result;

    if (n >= s->length)
    {
        result = xx_init_string("");
    }
    else
    {
        size_t new_len = s->length - n;

        result = malloc(sizeof(string));
        result->length = new_len;
        result->data = malloc(new_len + 1);

        memcpy(result->data, s->data + n, new_len);
        result->data[new_len] = '\0';
    }

    if (s_temp)
        xx_free_string((uintptr_t)s);

    return result;
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

Example* xx_init_Example(int a, double b, char c) 
{
	Example *xx_Example_init_ptr = malloc(sizeof(Example));
	xx_Example_init_ptr->a = a;
	xx_Example_init_ptr->b = b;
	xx_Example_init_ptr->c = c;
	return xx_Example_init_ptr;
}

void xx_update_Example(Example* dest, int a, double b, char c)
{
	if (dest == NULL) { return; }
	if (MVS_RefCount((uintptr_t)dest) > 1) {
		Example* new_dest = malloc(sizeof(Example));
		new_dest->a = a;
		new_dest->b = b;
		new_dest->c = c;
		MVS_DetachPointer((uintptr_t)dest);
		*dest = *new_dest;
		return;
	}
	dest->a = a;
	dest->b = b;
	dest->c = c;
}

typedef struct Example2 {
	int a;
	Example* e;
} Example2;

void xx_free_Example2(uintptr_t address)
{
	if (((Example2*)address)->e != NULL)
	{
		MVS_DetachPointer((uintptr_t)((Example2*)address)->e);
	}
	free(((Example2*)address));
}

Example2* xx_init_Example2(int a, Example* e) 
{
	Example2 *xx_Example2_init_ptr = malloc(sizeof(Example2));
	xx_Example2_init_ptr->a = a;
	xx_Example2_init_ptr->e = e;
	if (e != NULL) {
		MVS_RegisterNew((uintptr_t)e, sizeof(Example), xx_free_Example);
	}
	return xx_Example2_init_ptr;
}

void xx_update_Example2(Example2* dest, int a, Example* e)
{
	if (dest == NULL) { return; }
	if (MVS_RefCount((uintptr_t)dest) > 1) {
		Example2* new_dest = malloc(sizeof(Example2));
		new_dest->a = a;
		new_dest->e = e;
		if (new_dest->e != NULL) {
			MVS_RegisterNew((uintptr_t)new_dest->e, sizeof(Example), xx_free_Example);
		}
		MVS_DetachPointer((uintptr_t)dest);
		*dest = *new_dest;
		return;
	}
	dest->a = a;
	dest->e = e;
}

typedef struct Example3 {
	Example2* e;
} Example3;

void xx_free_Example3(uintptr_t address)
{
	if (((Example3*)address)->e != NULL)
	{
		MVS_DetachPointer((uintptr_t)((Example3*)address)->e);
	}
	free(((Example3*)address));
}

Example3* xx_init_Example3(Example2* e) 
{
	Example3 *xx_Example3_init_ptr = malloc(sizeof(Example3));
	xx_Example3_init_ptr->e = e;
	if (e != NULL) {
		MVS_RegisterNew((uintptr_t)e, sizeof(Example2), xx_free_Example2);
	}
	return xx_Example3_init_ptr;
}

void xx_update_Example3(Example3* dest, Example2* e)
{
	if (dest == NULL) { return; }
	if (MVS_RefCount((uintptr_t)dest) > 1) {
		Example3* new_dest = malloc(sizeof(Example3));
		new_dest->e = e;
		if (new_dest->e != NULL) {
			MVS_RegisterNew((uintptr_t)new_dest->e, sizeof(Example2), xx_free_Example2);
		}
		MVS_DetachPointer((uintptr_t)dest);
		*dest = *new_dest;
		return;
	}
	dest->e = e;
}



int main()
{
	MVS_Init();
	Example3* ex3 = xx_init_Example3(xx_init_Example2(1, xx_init_Example(1, 1, 'x')));
	MVS_RegisterNew((uintptr_t)ex3, sizeof(Example3), xx_free_Example3);
	MVS_DetachPointer((uintptr_t)ex3);
	MVS_Destroy();
}
