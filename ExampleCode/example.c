#define _CRT_SECURE_NO_WARNINGS
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

bool MVS_DetachPointer(uintptr_t address) {
    size_t index = _hash_address(address, mu.capacity);
    MVSNode* current = mu.buckets[index];
    MVSNode* prev = NULL;

    while (current) {
        if (current->key == address) {
            if (current->value.ref_count > 1) {
                current->value.ref_count--;
                return 0;
            }

            if (prev) { prev->next = current->next; }
            else { mu.buckets[index] = current->next; }

            current->value.deleter(address);
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

typedef struct array
{
    size_t length;
    size_t elem_size;
    bool is_pointer;
    _MVS_Deleter element_deleter;
    void* data;
} array;

void xx_free_array(uintptr_t address)
{
    array* arr = (array*)address;

    if (arr->data)
    {
        if (arr->is_pointer)
        {
            void** ptrs = (void**)arr->data;

            for (size_t i = 0; i < arr->length; i++)
            {
                if (ptrs[i])
                    MVS_DetachPointer((uintptr_t)ptrs[i]);
            }
        }

        free(arr->data);
    }

    free(arr);
}

array* xx_init_array(const void* data,
    size_t length,
    size_t elem_size,
    bool is_pointer,
    _MVS_Deleter element_deleter)
{
    array* arr = malloc(sizeof(array));

    arr->length = length;
    arr->elem_size = elem_size;
    arr->is_pointer = is_pointer;
    arr->element_deleter = element_deleter;

    arr->data = malloc(length * elem_size);

    if (data)
        memcpy(arr->data, data, length * elem_size);

    if (is_pointer && data)
    {
        void** ptrs = (void**)arr->data;

        for (size_t i = 0; i < length; i++)
        {
            if (ptrs[i])
                MVS_RegisterNew((uintptr_t)ptrs[i], 0, element_deleter);
        }
    }

    return arr;
}

void xx_array_set(array* arr, size_t index, void* value)
{
    if (!arr) return;
    if (index >= arr->length) return;

    char* base = (char*)arr->data + index * arr->elem_size;

    if (arr->is_pointer)
    {
        void* old_ptr = *(void**)base;

        if (old_ptr)
            MVS_DetachPointer((uintptr_t)old_ptr);

        void* new_ptr = *(void**)value;

        if (new_ptr)
            MVS_RegisterNew((uintptr_t)new_ptr, 0, arr->element_deleter);
    }

    memcpy(base, value, arr->elem_size);
}

void* xx_array_get(array* arr, size_t index)
{
    if (!arr) return NULL;
    if (index >= arr->length) return NULL;

    return (char*)arr->data + index * arr->elem_size;
}

array* xx_array_add(array* a, bool a_temp,
    array* b, bool b_temp)
{
    if (!a || !b) return NULL;
    if (a->elem_size != b->elem_size) return NULL;
    if (a->is_pointer != b->is_pointer) return NULL;

    array* result = malloc(sizeof(array));

    result->elem_size = a->elem_size;
    result->is_pointer = a->is_pointer;
    result->length = a->length + b->length;

    size_t bytes = result->length * result->elem_size;

    result->data = malloc(bytes);

    memcpy(result->data,
        a->data,
        a->length * a->elem_size);

    memcpy((char*)result->data + a->length * a->elem_size,
        b->data,
        b->length * b->elem_size);

    if (result->is_pointer)
    {
        void** ptrs = (void**)result->data;

        for (size_t i = 0; i < result->length; i++)
        {
            if (ptrs[i])
                MVS_RegisterNew((uintptr_t)ptrs[i], 0, a->element_deleter);
        }
    }

    if (a_temp)
        xx_free_array((uintptr_t)a);

    if (b_temp)
        xx_free_array((uintptr_t)b);

    return result;
}

array* xx_array_add_elem(array* a, bool a_temp, void* elem)
{
    if (!a) return NULL;

    array* result = malloc(sizeof(array));

    result->length = a->length + 1;
    result->elem_size = a->elem_size;
    result->is_pointer = a->is_pointer;

    result->data = malloc(result->length * result->elem_size);

    memcpy(result->data,
        a->data,
        a->length * a->elem_size);

    memcpy((char*)result->data + a->length * a->elem_size,
        elem,
        a->elem_size);

    if (result->is_pointer)
    {
        void* ptr = *(void**)elem;

        if (ptr)
            MVS_RegisterNew((uintptr_t)ptr, 0, a->element_deleter);
    }

    if (a_temp)
        xx_free_array((uintptr_t)a);

    return result;
}

array* xx_array_sub_right(array* arr, bool arr_temp, size_t n)
{
    if (!arr) return NULL;

    array* result;

    if (n >= arr->length)
    {
        result = xx_init_array(NULL, 0, arr->elem_size, arr->is_pointer, arr->element_deleter);
    }
    else
    {
        size_t new_len = arr->length - n;

        result = malloc(sizeof(array));

        result->length = new_len;
        result->elem_size = arr->elem_size;
        result->is_pointer = arr->is_pointer;

        result->data = malloc(new_len * result->elem_size);

        memcpy(result->data,
            arr->data,
            new_len * result->elem_size);

        if (result->is_pointer)
        {
            void** ptrs = (void**)result->data;

            for (size_t i = 0; i < new_len; i++)
            {
                if (ptrs[i])
                    MVS_RegisterNew((uintptr_t)ptrs[i], 0, arr->element_deleter);
            }
        }
    }

    if (arr_temp)
        xx_free_array((uintptr_t)arr);

    return result;
}

array* xx_array_sub_left(size_t n, array* arr, bool arr_temp)
{
    if (!arr) return NULL;

    array* result;

    if (n >= arr->length)
    {
        result = xx_init_array(NULL, 0, arr->elem_size, arr->is_pointer, arr->element_deleter);
    }
    else
    {
        size_t new_len = arr->length - n;

        result = malloc(sizeof(array));

        result->length = new_len;
        result->elem_size = arr->elem_size;
        result->is_pointer = arr->is_pointer;

        result->data = malloc(new_len * result->elem_size);

        memcpy(result->data,
            (char*)arr->data + n * arr->elem_size,
            new_len * result->elem_size);

        if (result->is_pointer)
        {
            void** ptrs = (void**)result->data;

            for (size_t i = 0; i < new_len; i++)
            {
                if (ptrs[i])
                    MVS_RegisterNew((uintptr_t)ptrs[i], 0, arr->element_deleter);
            }
        }
    }

    if (arr_temp)
        xx_free_array((uintptr_t)arr);

    return result;
}

array* xx_clone_array(array* ptr)
{
    if (!ptr)
        return NULL;

    array* clone = malloc(sizeof(array));

    clone->length = ptr->length;
    clone->elem_size = ptr->elem_size;
    clone->is_pointer = ptr->is_pointer;

    size_t bytes = ptr->length * ptr->elem_size;

    clone->data = malloc(bytes);

    memcpy(clone->data, ptr->data, bytes);

    if (clone->is_pointer)
    {
        void** ptrs = (void**)clone->data;

        for (size_t i = 0; i < clone->length; i++)
        {
            if (ptrs[i])
                MVS_RegisterNew((uintptr_t)ptrs[i], 0, ptr->element_deleter);
        }
    }

    MVS_RegisterNew((uintptr_t)clone, sizeof(array), xx_free_array);

    return clone;
}

typedef struct string {
    int length;
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

    if (a_temp)
        xx_free_string((uintptr_t)a);

    if (b_temp)
        xx_free_string((uintptr_t)b);

    return result;
}

string* xx_string_add_char(string* a, bool a_temp, char c)
{
    if (!a) return NULL;

    string* result = malloc(sizeof(string));

    result->length = a->length + 1;
    result->data = malloc(result->length + 1);

    memcpy(result->data, a->data, a->length);

    result->data[a->length] = c;
    result->data[result->length] = '\0';

    if (a_temp)
        xx_free_string((uintptr_t)a);

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

string* xx_clone_string(string* ptr)
{
    if (ptr == NULL)
        return NULL;

    string* clone = malloc(sizeof(string));

    clone->length = ptr->length;
    clone->data = malloc(ptr->length + 1);

    memcpy(clone->data, ptr->data, ptr->length + 1);

    MVS_RegisterNew((uintptr_t)clone, sizeof(string), xx_free_string);

    return clone;
}



int main()
{
	MVS_Init();
	array* s = xx_init_array((string*[]) {xx_init_string("Hello"), xx_init_string("World"), xx_init_string("!"), }, 3, sizeof(string*), 1, xx_free_string);
	MVS_RegisterNew((uintptr_t)s, sizeof(array), xx_free_array);
	array* ss = s;
	MVS_RegisterNew((uintptr_t)ss, sizeof(array), xx_free_array);
	if (MVS_RefCount((uintptr_t)s) > 1)
	{
		array* xx_clone = xx_clone_array(s);
		MVS_DetachPointer((uintptr_t)s);
		string* xx_new_s = xx_init_string("Jello");
		MVS_RegisterNew((uintptr_t)xx_new_s, sizeof(string), xx_free_string);
		xx_array_set(xx_clone, 0, &xx_new_s);
		s = xx_clone;
	}
	else
	{
		string* xx_new_s = xx_init_string("Jello");
		MVS_RegisterNew((uintptr_t)xx_new_s, sizeof(string), xx_free_string);
		xx_array_set(s, 0, &xx_new_s);
	}
	printf("%s", (*(string**)xx_array_get(ss, 0))->data);
	MVS_DetachPointer((uintptr_t)s);
	MVS_DetachPointer((uintptr_t)ss);
	MVS_Destroy();
}
