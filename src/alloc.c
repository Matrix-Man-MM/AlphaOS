#if !defined(DEBUG)
#define NDEBUG
#endif

#define _XOPEN_SOURCE 700

#include "sys.h"

#if SIZE_MAX == UINT32_MAX
#define NUM_BINS 11U
#define SMALLEST_BIN_LOG 2U
#else
#define NUM_BINS 10U
#define SMALLEST_BIN_LOG 3U
#endif

#define BIG_BIN (NUM_BINS - 1)
#define SMALLEST_BIN (1UL << SMALLEST_BIN_LOG)

#define PAGE_SIZE 0x1000
#define PAGE_MASK (PAGE_SIZE - 1)
#define SKIP_P INT32_MAX
#define SKIP_MAX_LEVEL 6

static void* __attribute__ ((malloc)) klmalloc(size_t size);
static void* __attribute__ ((malloc)) klrealloc(void* ptr, size_t size);
static void* __attribute__ ((malloc)) klcalloc(size_t nmemb, size_t size);
static void klfree(void* ptr);

void* __attribute__ ((malloc)) amalloc(size_t size)
{
	return klmalloc(size);
}

void* __attribute__ ((malloc)) arealloc(void* ptr, size_t size)
{
	return klrealloc(ptr, size);
}

void* __attribute__ ((malloc)) acalloc(size_t nmemb, size_t size)
{
	return klcalloc(nmemb, size);
}

void free(void* ptr)
{
	klfree(ptr);
}

static size_t __attribute__ ((always_inline, pure)) klmalloc_adjust_bin(size_t bin)
{
	if (bin <= SMALLEST_BIN_LOG)
		return 0;

	bin -= SMALLEST_BIN_LOG + 1;
	if (bin > BIG_BIN)
		return BIG_BIN;

	return bin;
}

static size_t __attribute__ ((always_inline, pure)) klmalloc_bin_size(size_t size)
{
	size_t bin = sizeof(size) * CHAR_BIT - __builtin_clzl(size);
	bin += !!(size & (size - 1));
	return klmalloc_adjust_bin(bin);
}

typedef struct _klmalloc_bin_header
{
	struct _klmalloc_bin_header* next;
	void* head;
	size_t size;
} klmalloc_bin_header;

typedef struct _klmalloc_big_bin_header
{
	struct _klmalloc_big_bin_header* next;
	void* head;
	size_t size;
	struct _klmalloc_big_bin_header* prev;
	struct _klmalloc_big_bin_header* forward[SKIP_MAX_LEVEL + 1];
} klmalloc_big_bin_header;

typedef struct _klmalloc_bin_header_head
{
	klmalloc_bin_header * first;
} klmalloc_bin_header_head;

static klmalloc_bin_header_head klmalloc_bin_head[NUM_BINS - 1];
static struct _klmalloc_big_bins
{
	klmalloc_big_bin_header head;
	int level;
} klmalloc_big_bins;
static klmalloc_big_bin_header* klmalloc_newest_big = NULL;

static void __attribute__ ((always_inline)) klmalloc_list_decouple(klmalloc_bin_header_head* head, klmalloc_bin_header* node)
{
	klmalloc_bin_header* next = node->next;
	head->first = next;
	node->next = NULL;
}

static void __attribute__ ((always_inline)) klmalloc_list_insert(klmalloc_bin_header_head* head, klmalloc_bin_header* node)
{
	node->next = head->first;
	head->first = node;
}

static klmalloc_bin_header* __attribute__ ((always_inline)) klmalloc_list_head(klmalloc_bin_header_head* head)
{
	return head->first;
}

static uint32_t __attribute__ ((pure)) klmalloc_skip_rand()
{
	static uint32_t x = 123456789;
	static uint32_t y = 362436069;
	static uint32_t z = 521288629;
	static uint32_t w = 88675123;

	uint32_t t;

	t = x ^ (x << 11);
	x = y;
	y = z;
	z = w;

	return w = w ^ (w >> 19) ^ t ^ (t >> 8);
}

static int __attribute__ ((pure, always_inline)) klmalloc_random_level()
{
	int level = 0;

	while (klmalloc_skip_rand() < SKIP_P && level < SKIP_MAX_LEVEL)
		++level;

	return level;
}

static klmalloc_big_bin_header* klmalloc_skip_list_findbest(size_t search_size)
{
	klmalloc_big_bin_header* node = &klmalloc_big_bins.head;

	int i;
	for (i = klmalloc_big_bins.level; i >= 0; --i)
	{
		while (node->forward[i] && (node->forward[i]->size < search_size))
		{
			node = node->forward[i];

			if (node)
				kernel_assert((node->size + sizeof(klmalloc_big_bin_header)) % PAGE_SIZE == 0);
		}
	}

	node = node->forward[0];

	if (node)
	{
		kernel_assert((uintptr_t)node % PAGE_SIZE == 0);
		kernel_assert((node->size + sizeof(klmalloc_big_bin_header)) % PAGE_SIZE == 0);
	}

	return node;
}

static void klmalloc_skip_list_insert(klmalloc_big_bin_header* value)
{
	kernel_assert(value != NULL);
	kernel_assert(value->head != NULL);
	kernel_assert((uintptr_t)value % PAGE_SIZE == 0);
	kernel_assert((value->size + sizeof(klmalloc_big_bin_header)) % PAGE_SIZE == 0);
	kernel_assert(value->size != 0);

	klmalloc_big_bin_header* node = &klmalloc_big_bins.head;
	klmalloc_big_bin_header* update[SKIP_MAX_LEVEL + 1];

	int i;
	for (i = klmalloc_big_bins.level; i >= 0; --i)
	{
		while (node->forward[i] && node->forward[i]->size < value->size)
		{
			node = node->forward[i];
			if (node)
				kernel_assert((node->size + sizeof(klmalloc_big_bin_header)) % PAGE_SIZE == 0);
		}

		update[i] = node;
	}

	node = node->forward[0];

	if (node != value)
	{
		int level = klmalloc_random_level();

		if (level > klmalloc_big_bins.level)
		{
			for (i = klmalloc_big_bins.level + 1; i <= level; ++i)
				update[i] = &klmalloc_big_bins.head;

			klmalloc_big_bins.level = level;
		}

		node = value;

		for (i = 0; i <= level; ++i)
		{
			node->forward[i] = update[i]->forward[i];

			if (node->forward[i])
				kernel_assert((node->forward[i]->size + sizeof(klmalloc_big_bin_header)) % PAGE_SIZE == 0);

			update[i]->forward[i] = node;
		}
	}
}

static void klmalloc_skip_list_delete(klmalloc_big_bin_header* value)
{
	kernel_assert(value != NULL);
	kernel_assert(value->head);

	klmalloc_big_bin_header* node = &klmalloc_big_bins.head;
	klmalloc_big_bin_header* update[SKIP_MAX_LEVEL + 1];

	int i;
	for (i = klmalloc_big_bins.level; i >= 0; --i)
	{
		while (node->forward[i] && node->forward[i]->size < value->size)
		{
			node = node->forward[i];
			if (node)
				kernel_assert((node->size + sizeof(klmalloc_big_bin_header)) % PAGE_SIZE == 0);
		}

		update[i] = node;
	}

	node = node->forward[0];
	while (node != value)
		node = node->forward[0];

	if (node != value)
	{
		node = klmalloc_big_bins.head.forward[0];

		while (node->forward[0] && node->forward[0] != value)
			node = node->forward[0];

		node = node->forward[0];
	}

	if (node == value)
	{
		for (i = 0; i <= klmalloc_big_bins.level; ++i)
		{
			if (update[i]->forward[i] != node)
				break;

			update[i]->forward[i] = node->forward[i];

			if (update[i]->forward[i])
			{
				kernel_assert((uintptr_t)(update[i]->forward[i]) % PAGE_SIZE == 0);
				kernel_assert((update[i]->forward[i]->size + sizeof(klmalloc_big_bin_header)) % PAGE_SIZE == 0);
			}
		}

		while (klmalloc_big_bins.level > 0 && klmalloc_big_bins.head.forward[klmalloc_big_bins.level] == NULL)
			--klmalloc_big_bins.level;
	}
}

static void* klmalloc_stack_pop(klmalloc_bin_header* header)
{
	kernel_assert(header);
	kernel_assert(header->head != NULL);

	void* item = header->head;
	size_t** head = header->head;
	size_t* next = *head;
	header->head = next;

	return item;
}

static void klmalloc_stack_push(klmalloc_bin_header* header, void* ptr)
{
	kernel_assert(ptr != NULL);
	size_t** item = (size_t**)ptr;
	*item = (size_t*)header->head;
	header->head = item;
}

static int __attribute__ ((always_inline)) klmalloc_stack_empty(klmalloc_bin_header* header)
{
	return header->head == NULL;
}

static void* __attribute__ ((malloc)) klmalloc(size_t size)
{
	if (__builtin_expect(size == 0, 0))
		return NULL;

	unsigned int bucket_id = klmalloc_bin_size(size);

	if (bucket_id < BIG_BIN)
	{
		klmalloc_bin_header* bin_header = klmalloc_list_head(&klmalloc_bin_head[bucket_id]);
		if (!bin_header)
		{
			bin_header = (klmalloc_bin_header*)sbrk(PAGE_SIZE);
			kernel_assert((uintptr_t)bin_header % PAGE_SIZE == 0);
			printf("bin_header = 0x%x\n", bin_header);

			bin_header->head = (void*)((uintptr_t)bin_header + sizeof(klmalloc_bin_header));

			klmalloc_list_insert(&klmalloc_bin_head[bucket_id], bin_header);

			size_t adj = SMALLEST_BIN_LOG + bucket_id;
			size_t i, available = ((PAGE_SIZE - sizeof(klmalloc_bin_header)) >> adj) - 1;

			size_t** base = bin_header->head;
			for (i = 0; i < available; ++i)
				base[i << bucket_id] = (size_t*)&base[(i + 1) << bucket_id];

			base[available << bucket_id] = NULL;
			bin_header->size = bucket_id;
		}

		size_t** item = klmalloc_stack_pop(bin_header);
		if (klmalloc_stack_empty(bin_header))
			klmalloc_list_decouple(&(klmalloc_bin_head[bucket_id]), bin_header);

		return item;
	}
	else
	{
		klmalloc_big_bin_header* bin_header = klmalloc_skip_list_findbest(size);
		if (bin_header)
		{
			klmalloc_skip_list_delete(bin_header);
			size_t** item = klmalloc_stack_pop((klmalloc_bin_header*)bin_header);

#if 0
			kernel_assert(bin_header->head == NULL);
			size_t old_size = bin_header->size;

			size = ((size + sizeof(klmalloc_big_bin_header)) / PAGE_SIZE + 1) * PAGE_SIZE - sizeof(klmalloc_big_bin_header);
			assert((size + sizeof(klmalloc_big_bin_header)) % PAGE_SIZE == 0);

			if (bin_header->size > size * 2)
			{
				kernel_assert(old_size != size);

				bin_header->size = size;
				kernel_assert(sbrk(0) >= bin_header->size + (uintptr_t)bin_header);

				klmalloc_big_bin_header* header_new = (klmalloc_big_bin_header*)((uintptr_t)bin_header + sizeof(klmalloc_big_bin_header) + size);
				kernel_assert((uintptr_t)header_new % PAGE_SIZE == 0);
				memset(header_new, 0, sizeof(klmalloc_big_bin_header) + sizeof(void*));
				header_new->prev = bin_header;

				if (bin_header->next)
					bin_header->next->prev = header_new;

				header_new->next = bin_header->next;
				bin_header->next = header_new;

				if (klmalloc_newest_big == bin_header)
					klmalloc_newest_big = header_new;

				header_new->size = old_size - (size + sizeof(klmalloc_big_bin_header));
				kernel_assert(((uintptr_t)header_new->size + sizeof(klmalloc_big_bin_header)) % PAGE_SIZE == 0);
				fprintf(stderr, "Splitting %p [now %zx] at %p [%zx] from [%zx,%zx].\n", (void*)bin_header, bin_header->size, (void*)header_new, header_new->size, old_size, size);

				klfree((void*)((uintptr_t)header_new + sizeof(klmalloc_big_bin_header)));
			}
#endif

			return item;
		}
		else
		{
			size_t pages = (size + sizeof(klmalloc_big_bin_header)) / PAGE_SIZE + 1;
			bin_header = (klmalloc_big_bin_header*)sbrk(PAGE_SIZE * pages);
			kernel_assert((uintptr_t)bin_header % PAGE_SIZE == 0);

			bin_header->size = pages * PAGE_SIZE - sizeof(klmalloc_big_bin_header);
			kernel_assert((bin_header->size + sizeof(klmalloc_big_bin_header)) % PAGE_SIZE == 0);

			bin_header->prev = klmalloc_newest_big;
			if (bin_header->prev)
				bin_header->prev->next = bin_header;

			klmalloc_newest_big = bin_header;
			bin_header->next = NULL;

			bin_header->head = NULL;
			return (void*)((uintptr_t)bin_header + sizeof(klmalloc_big_bin_header));
		}
	}
}

static void klfree(void* ptr)
{
	if (__builtin_expect(ptr == NULL, 0))
		return;

	klmalloc_bin_header* header = (klmalloc_bin_header*)((uintptr_t)ptr & (size_t)~PAGE_MASK);
	kernel_assert((uintptr_t)header % PAGE_SIZE == 0);

	size_t bucket_id = header->size;
	if (bucket_id > NUM_BINS)
	{
		bucket_id = BIG_BIN;
		klmalloc_big_bin_header* bheader = (klmalloc_big_bin_header*)header;

		kernel_assert(bheader);
		kernel_assert(bheader->head == NULL);
		kernel_assert((bheader->size + sizeof(klmalloc_big_bin_header)) % PAGE_SIZE == 0);
#if 0
		if (bheader != klmalloc_newest_big)
		{
			kernel_assert((bheader->size + sizeof(klmalloc_big_bin_header)) % PAGE_SIZE == 0);
			klmalloc_big_bin_header* next = (void*)((uintptr_t)bheader + sizeof(klmalloc_big_bin_header) + bheader->size);
			kernel_assert((uintptr_t)next % PAGE_SIZE == 0);

			if (next == bheader->next && next->head)
			{
				size_t old_size = bheader->size;

				klmalloc_skip_list_delete(next);
				bheader->size = (size_t)bheader->size + (size_t)sizeof(klmalloc_big_bin_header) + next->size;
				kernel_assert((bheader->size + sizeof(klmalloc_big_bin_header)) % PAGE_SIZE == 0);

				if (next == klmalloc_newest_big)
					klmalloc_newest_big = bheader;
				else
				{
					if (next->next)
						next->next->prev = bheader;
				}

				fprintf(stderr, "Coelesced (forwards) %p [%zx] <- %p [%zx] = %zx\n", (void*)bheader, old_size, (void*)next, next->size, bheader->size;
			}
		}
#endif

#if 0
		if (bheader->prev && bheader->prev->head)
		{
			if ((uintptr_t)bheader->prev + (bheader->prev->size + sizeof(klmalloc_big_bin_header)) == (uintptr_t)bheader)
			{
				size_t old_size = bheader->prev->size;

				klmalloc_skip_list_delete(bheader->prev);
				bheader->prev->size = (size_t)bheader->prev->size + (size_t)bheader->size + sizeof(klmalloc_big_bin_header);
				assert((bheader->prev->size + sizeof(klmalloc_big_bin_header)) % PAGE_SIZE == 0);
				klmalloc_skip_list_insert(bheader->prev);

				if (klmalloc_newest_big == bheader)
					klmalloc_newest_big = bheader->prev;
				else
				{
					if (bheader->next)
						bheader->next->prev = bheader->prev;
				}

				fprintf(stderr, "Coelesced (backwards) %p [%zx] <- %p [%zx] = %zx\n", (void*)bheader->prev, old_size, (void*)bheader, bheader->size, bheader->size);

				return;
			}
		}
#endif

		klmalloc_stack_push((klmalloc_bin_header*)bheader, (void*)((uintptr_t)bheader + sizeof(klmalloc_big_bin_header)));
		kernel_assert(bheader->head != NULL);

		klmalloc_skip_list_insert(bheader);


	}
	else
	{
		if (klmalloc_stack_empty(header))
			klmalloc_list_insert(&klmalloc_bin_head[bucket_id], header);

		klmalloc_stack_push(header, ptr);
	}
}

static void* __attribute__ ((malloc)) klrealloc(void* ptr, size_t size)
{
	if (__builtin_expect(ptr == NULL, 0))
		return amalloc(size);

	if (__builtin_expect(size == 0, 0))
	{
		free(ptr);
		return NULL;
	}

	klmalloc_bin_header* header_old = (void*)((uintptr_t)ptr & (size_t)~PAGE_MASK);

	if (header_old->size >= size)
		return ptr;

	void* newptr = klmalloc(size);
	if (__builtin_expect(newptr != NULL, 1))
	{
		size_t old_size = header_old->size;
		if (old_size < BIG_BIN)
			old_size = (1UL << (SMALLEST_BIN_LOG + old_size));

		memcpy(newptr, ptr, old_size);
		klfree(ptr);
		return newptr;
	}

	return NULL;
}

static void* __attribute__ ((malloc)) klcalloc(size_t nmemb, size_t size)
{
	void* ptr = klmalloc(nmemb * size);

	if (__builtin_expect(ptr != NULL, 1))
		memset(ptr, 0x00, nmemb * size);

	return ptr;
}
