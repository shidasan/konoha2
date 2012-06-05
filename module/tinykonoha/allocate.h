/****************************************************************************
 * Copyright (c) 2012, the Konoha project authors. All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ***************************************************************************/

#ifdef K_USING_TOPPERS
#define HEAP_SIZE ((size_t)(1024 * 6)) /* 6KB */
#elif defined K_USING_TINYVM
#define HEAP_SIZE (4096 * 3 * 3) /* 36KB */
#else
#define HEAP_SIZE (1024 * 1024 * 32)
#endif
#define MINIMUM_ALLOCATE_SIZE 4

typedef struct heap_free_area
{
	size_t size;
	struct heap_free_area *next;
}heap_header;

static char space[HEAP_SIZE];
static heap_header *header_global = NULL;

static void *heap_alloc(size_t size, heap_header **header)
{
	heap_header *_header = *header;
	heap_header *prev = NULL;
	void *mem = NULL;
	char *ptr;
	while (mem == NULL) {
		if (_header->size > size + sizeof(heap_header)) {
			int newsize = _header->size - (sizeof(heap_header) + size);
			ptr = (char*)_header;
			mem = (void*)(ptr + sizeof(heap_header));
			if (newsize < MINIMUM_ALLOCATE_SIZE) {
				if (prev != NULL) prev->next = _header->next;
				else *header = _header->next;
			} else {
				heap_header *newheader = (heap_header*)(ptr + sizeof(heap_header) + size);
				newheader->size = newsize;
				newheader->next = _header->next;
				if (prev != NULL) prev->next = newheader;
				else {
					*header = newheader;
				}
			}
		} else if (_header->next == NULL) {
			return NULL;
		} else {
			prev = _header;
			_header = _header->next;
		}
	}
	return mem;
}

static heap_header *ptr_to_header(void *ptr)
{
	char *_ptr = (char*)ptr;
	return (heap_header*)(_ptr - sizeof(heap_header));
}

static void heap_init()
{
	header_global = (heap_header*)space;
	header_global->size = HEAP_SIZE;
	header_global->next = NULL;
}

static int total_malloced = 0;
static void *tiny_malloc(size_t size)
{
	size = size + ((4 - size % 4) % 4);
	total_malloced+=size + sizeof(heap_header);
	//if (total_malloced > HEAP_SIZE / 10 * 9) {
	//	TDBG_i("total mallocked", total_malloced);
	//}
	void *mem;
	mem = heap_alloc(size, &header_global);
	//TDBG_s("malloc end");
	if (mem == NULL) {
		TDBG_abort("NULL");
	}
	return mem;
}

static void heap_free(heap_header *free_header, heap_header **header)
{
	heap_header *prev = NULL;
	heap_header *_header = *header;
	while (_header != NULL && _header < free_header) {
		prev = _header;
		_header = _header->next;
	}

	char *ptr0 = (char*)prev, *ptr1 = (char*)free_header;
	if (prev == NULL) {
		free_header->next = *header;
		*header = free_header;
	} else if (ptr0 + sizeof(heap_header) + prev->size == ptr1) {
		prev->size += (sizeof(heap_header) + free_header->size);
	} else {
		heap_header *tmp = prev->next;
		prev->next = free_header;
		free_header->next = tmp;
	}

	ptr0 = (char*)free_header; ptr1 = (char*)free_header->next;
	if (ptr0 + sizeof(heap_header) + free_header->size == ptr1) {
		free_header->next = free_header->next->next;
		free_header->size += sizeof(heap_header) + free_header->next->size;
	}
}

static void tiny_free(void* ptr)
{
	heap_free(ptr_to_header(ptr), &header_global);
}
