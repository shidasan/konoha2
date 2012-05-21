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

#define HEAP_SIZE 4096
#define MINIMUM_ALLOCATE_SIZE 4

typedef struct heap_free_area
{
	size_t size;
	struct heap_free_area *next;
}header;

static char space[HEAP_SIZE];
static header *header = NULL;

static void *heap_alloc(header *header, size_t size)
{
	void *mem = NULL;
	while (mem == NULL) {
		if (header->size > size + sizeof(header)) {
			header *_header = header;
			size_t size = _header->size - (sizeof(header) + size);
			mem = (void*)(((char*)header) + sizeof(header));
			if (size < MINIMUM_ALLOCATE_SIZE) {
				header = header->next;
			} else {
				header = (header*)(((char*)header) + (sizeof(header) + size));
				header->size = _header->size - (sizeof(header) + size);
				header->next = _header->next;
			}
		} else if (header->next == NULL) {
			return NULL;
		} else {
			header = header->next;
		}
	}
	return mem;
}

static header *ptr_to_header(void *ptr)
{
	char *_ptr = (char*)_ptr;
	return _ptr - sizeof(header);
}

static void heap_init()
{
	header = (header*)space;
	header->size = HEAP_SIZE;
	header->next = NULL;
}

static void *malloc(size_t size)
{
	void *mem;
	mem = heap_alloc(header, size);
	return mem;
}

static void heap_free(header *free_header, header *header)
{
	header *prev = header;
	while (header < free_header) {
		prev = header;
		header = header->next;
	}
	char *ptr0 = (char*)prev, ptr1 = (char*)free_header;
	if (ptr0 + sizeof(header) + prev->size == ptr1) {
		prev->size += (sizeof(header) + free_header->size);
	} else {
		header *tmp = prev->next;
		prev->next = free_header;
		free_header->next = tmp;
	}
}

static void free(void* ptr)
{
	heap_free(ptr_to_header(ptr), header);
}
