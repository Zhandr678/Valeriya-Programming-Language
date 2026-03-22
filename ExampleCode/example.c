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

typedef struct cmd cmd;
void xx_free_cmd(uintptr_t address);

typedef struct move {
	double dx;
	double dy;
	} move;

typedef struct rotate {
	double angle;
	} rotate;

typedef struct stop stop;

typedef enum cmd_Tag {
	move_tag, rotate_tag, stop_tag, 
} cmd_Tag;

typedef union cmd_Opts {
	move* move_opt;
	rotate* rotate_opt;
	stop* stop_opt;
} cmd_Opts;

typedef struct cmd {
	cmd_Tag tag;
	cmd_Opts opts;
} cmd;

bool xx_cmd_option_is_move(const cmd* prop)
{
	return prop->tag == move_tag;
}

bool xx_cmd_option_is_rotate(const cmd* prop)
{
	return prop->tag == rotate_tag;
}

bool xx_cmd_option_is_stop(const cmd* prop)
{
	return prop->tag == stop_tag;
}

void xx_free_cmd(uintptr_t address)
{
	if ((((cmd*)address)->tag == move_tag) && (((cmd*)address)->opts.move_opt != NULL)) 
	{
		MVS_DetachPointer((uintptr_t)((cmd*)address)->opts.move_opt);
	}
	if ((((cmd*)address)->tag == rotate_tag) && (((cmd*)address)->opts.rotate_opt != NULL)) 
	{
		MVS_DetachPointer((uintptr_t)((cmd*)address)->opts.rotate_opt);
	}
	if ((((cmd*)address)->tag == stop_tag) && (((cmd*)address)->opts.stop_opt != NULL)) 
	{
		MVS_DetachPointer((uintptr_t)((cmd*)address)->opts.stop_opt);
	}
	free(((cmd*)address));
}

void xx_free_move(uintptr_t address)
{
	free(((move*)address));
}

move* xx_init_move(double dx, double dy) 
{
	move *xx_move_init_ptr = malloc(sizeof(move));
	xx_move_init_ptr->dx = dx;
	xx_move_init_ptr->dy = dy;
	return xx_move_init_ptr;
}

move* xx_clone_move(move* ptr) 
{
	if (ptr == NULL) { return NULL; }
	move* clone = xx_init_move(ptr->dx, ptr->dy);
	MVS_RegisterNew((uintptr_t)clone, sizeof(move), xx_free_move);
	return clone;
}

void xx_free_rotate(uintptr_t address)
{
	free(((rotate*)address));
}

rotate* xx_init_rotate(double angle) 
{
	rotate *xx_rotate_init_ptr = malloc(sizeof(rotate));
	xx_rotate_init_ptr->angle = angle;
	return xx_rotate_init_ptr;
}

rotate* xx_clone_rotate(rotate* ptr) 
{
	if (ptr == NULL) { return NULL; }
	rotate* clone = xx_init_rotate(ptr->angle);
	MVS_RegisterNew((uintptr_t)clone, sizeof(rotate), xx_free_rotate);
	return clone;
}

void xx_free_stop(uintptr_t address)
{
}

stop* xx_init_stop()
{
	return NULL;
}

stop* xx_clone_stop(stop* ptr) 
{
	if (ptr == NULL) { return NULL; }
	stop* clone = xx_init_stop();
	return clone;
}

cmd* xx_init_cmd_move(move* move_opt)
{
	cmd* xx_init_cmd_ptr = malloc(sizeof(cmd));
	xx_init_cmd_ptr->tag = move_tag;
	xx_init_cmd_ptr->opts.move_opt = move_opt;
	if (move_opt != NULL)
	{
		MVS_RegisterNew((uintptr_t)move_opt, sizeof(move), xx_free_move);
	}
	return xx_init_cmd_ptr;
}

cmd* xx_init_cmd_rotate(rotate* rotate_opt)
{
	cmd* xx_init_cmd_ptr = malloc(sizeof(cmd));
	xx_init_cmd_ptr->tag = rotate_tag;
	xx_init_cmd_ptr->opts.rotate_opt = rotate_opt;
	if (rotate_opt != NULL)
	{
		MVS_RegisterNew((uintptr_t)rotate_opt, sizeof(rotate), xx_free_rotate);
	}
	return xx_init_cmd_ptr;
}

cmd* xx_init_cmd_stop(stop* stop_opt)
{
	cmd* xx_init_cmd_ptr = malloc(sizeof(cmd));
	xx_init_cmd_ptr->tag = stop_tag;
	xx_init_cmd_ptr->opts.stop_opt = stop_opt;
	return xx_init_cmd_ptr;
}

cmd* xx_clone_cmd(cmd* ptr) 
{
	if (ptr == NULL) { return NULL; }
	cmd* clone = malloc(sizeof(cmd));
	clone->tag = ptr->tag;
	switch (ptr->tag)
	{
	case move_tag:
		clone->opts.move_opt = xx_clone_move(ptr->opts.move_opt);
		break;
	case rotate_tag:
		clone->opts.rotate_opt = xx_clone_rotate(ptr->opts.rotate_opt);
		break;
	case stop_tag:
		clone->opts.stop_opt = xx_clone_stop(ptr->opts.stop_opt);
		break;
	}
	MVS_RegisterNew((uintptr_t)clone, sizeof(cmd), xx_free_cmd);
	return clone;
}



int main()
{
	MVS_Init();
	array* cc = xx_init_array((cmd*[]) {xx_init_cmd_move(xx_init_move(5.000000, 5.000000)), xx_init_cmd_rotate(xx_init_rotate(90.000000)), xx_init_cmd_stop(xx_init_stop()), }, 3, sizeof(stop*), 1, xx_free_stop);
	MVS_RegisterNew((uintptr_t)cc, sizeof(array), xx_free_array);
	{
		int i = 0;
		while(1)
		{
			if (!(i<cc->length)) { break; }
			{
				printf("[Command Number %d] ", i);
				if (xx_cmd_option_is_move((*(cmd**)xx_array_get(cc, i))))
				{
					printf("moved: %f, %f\n", (*(cmd**)xx_array_get(cc, i))->opts.move_opt->dx, (*(cmd**)xx_array_get(cc, i))->opts.move_opt->dy);
				}
				else if (xx_cmd_option_is_rotate((*(cmd**)xx_array_get(cc, i))))
				{
					printf("rotated: %f\n", (*(cmd**)xx_array_get(cc, i))->opts.rotate_opt->angle);
				}
				else if (xx_cmd_option_is_stop((*(cmd**)xx_array_get(cc, i))))
				{
					printf("stopped\n");
				}
			}
			{
				i = i+1;
			}
		}
	}
	MVS_DetachPointer((uintptr_t)cc);
	MVS_Destroy();
}
