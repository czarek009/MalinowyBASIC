#include "mm.h"
#include "printf.h"
#include "types.h"
#include "debug.h"

/*

  BLOCK = 4 bytes

  TYPES OF BLOCKS:
    - HEADER
    - FOOTER
    - PAYLOAD

----------------------------------------------------------------------------------------------

  .                 |        32 - 1 bit          |   0 bit    |
  HEADER / FOOTER = |  size     (hd + pld + ft)  | allocated? |

----------------------------------------------------------------------------------------------

  .      |  4   |   size_in_blocks()        |  4   |
  .      | bytes|          bytes            | bytes|
  DATA = |HEADER|PAYLOAD|PAYLOAD|...|PAYLOAD|FOOTER|

*/


/* BLOCK TYPE */
typedef u32 word_t;

#define BLOCK_SIZE       sizeof(word_t)
#define METADATA_SIZE    (2 * BLOCK_SIZE)
#define OFFSET           (ALIGNMENT - BLOCK_SIZE)
#define MAX_ALLOC        (HEAP_SIZE - OFFSET)

#define ALLOCATED        true
#define FREE             false


/* pointer to first header */
static word_t *first_header;
/* pointer to last header */
static word_t *last_header;
/* ponter to end of heap */
static word_t *heap_end;


/* round up size to ALIGNMENT (in bytes) */
static size_t round_up(size_t size) {
  return (size + ALIGNMENT - 1) & -ALIGNMENT;
}

/* get size in number of blocks based on header/footer */
static size_t size_in_blocks(word_t *hf) {
  return (*hf & -2) / BLOCK_SIZE;
}

/* get size */
static size_t get_size(word_t *hf) {
  return *hf & -2;
}

/* function for printing memory map*/
static size_t size_of_alloc(word_t *hf) {
  return get_size(hf) - METADATA_SIZE;
}

/* check if block is allocated based on header/footer */
static bool is_alocated(word_t *hf) {
  return *hf & 1;
}

/* get ponter to first payload block based on header */
static word_t *get_payload(word_t *header) {
  return header + 1;
}

/* get header based on first payload block */
static word_t *get_header(word_t *payload) {
  return payload - 1;
}

/* get next header if possible else return NULL */
static word_t *next_header(word_t *header) {
  word_t *next = header + size_in_blocks(header);
  if (heap_end <= next) {
    return NULL;
  }
  return next;
}

static word_t *prev_header(word_t *header) {
  return header - size_in_blocks(header - 1);
}

static void set_header(word_t *header, size_t size, bool is_allocated) {
  printf("size = %lu\n", size);
  *header = size | is_allocated;
}

static void set_footer(word_t *header) {
  word_t *footer = header + size_in_blocks(header) - 1;
  *footer = *header;
}

void mem_init(void) {
  first_header = (word_t *)((u8 *)HEAP_ADDRESS + OFFSET);
  last_header = first_header;
  set_header(first_header, MAX_ALLOC, FREE);
  set_footer(first_header);
  heap_end = (word_t *)HEAP_END_ADDRESS;
}

static word_t *find_free_block(size_t size) {
  word_t *block = first_header;
  while((is_alocated(block) || (get_size(block) < size)) && block != NULL){
    block = next_header(block);
  }
  return block;
}

static void update_last_header(word_t *old_header, word_t *new_header) {
  if (last_header == old_header){
    last_header = new_header;
  }
}

static void split(word_t *header, size_t new_alloc_size) {
  size_t new_free_size = get_size(header) - new_alloc_size;
  set_header(header, new_alloc_size, ALLOCATED);
  set_footer(header);
  if (0 < new_free_size){
    word_t *free_header = next_header(header);
    set_header(free_header, new_free_size, FREE);
    set_footer(free_header);
    update_last_header(header, free_header);
  }
}

/* allocate number of bytes if unsuccessful return NULL*/
void *mmalloc(size_t size) {
  size = round_up(size + METADATA_SIZE);
  word_t *header = find_free_block(size);
  if (header == NULL){
    return header;
  }
  split(header, size);
  return get_payload(header);
}

static word_t *coalesce(word_t *first, word_t *second) {
  set_header(first, get_size(first) + get_size(second), FREE);
  set_footer(first);
  update_last_header(second, first);
  return first;
}

void ffree(void *ptr) {
  word_t *header = get_header((word_t *)ptr);
  set_header(header, get_size(header), FREE);
  print_memory_map();
  set_footer(header);
  if (first_header != header) {
    word_t *prev = prev_header(header);
    if(!is_alocated(prev)) {
      header = coalesce(prev, header);
    }
  }
  if (last_header != header) {
    word_t *next = next_header(header);
    printf("ptr = %lu, next = %lu\n", (u64)header, (u64)next);
    if(!is_alocated(next)) {
      printf("scalanie\n");
      header = coalesce(header, next);
    }
  }
}

void print_memory_map(void) {
  printf("\nlast_header: %lu\n\n", (u64)last_header);
  u32 data_counter = 0;
  for(word_t *header = first_header; header != NULL; header = next_header(header)) {
    printf("DATA NUMBER: %d\n", data_counter);
    printf("header address: %lu\n", (u64)header);
    printf("memory address: %lu\n", (u64)get_payload(header));
    printf("allocated: %d\n", is_alocated(header));
    printf("sizeof alloc: %lu\n\n", (u64)size_of_alloc(header));
    data_counter++;
  }
  printf("----------------------------------------------------------\n");
}

void print_pointer_contents(void *ptr) {
  word_t *header = get_header((word_t *)ptr);
  word_t *footer = header + size_in_blocks(header) - 1;
  size_t size = size_of_alloc(header);
  u8 *content = (u8 *)ptr;
  printf("header address: %lu\n", (u64)header);
  printf("memory address: %lu\n", (u64)ptr);
  printf("footer address: %lu\n", (u64)footer);
  printf("allocated: %d\n", is_alocated(header));
  printf("sizeof alloc: %lu\n", (u64)size);
  printf("header content: %u\n", *header);
  printf("footer content: %u\n", *footer);
  printf("payload content:\n");
  for (u8 i = 0; i < size; i++) {
    printf("%ld\n", content[i]);
  }
}

bool lickitung_check(void) {
  bool liks = false;

  for(word_t *header = first_header; header != NULL; header = next_header(header)) {
    if (is_alocated(header)) {
      liks = true;
      ERROR("[LICKITUNG] Memory leak detected! Address: %lu  Size: %lu\n",
        (u64)get_payload(header),
        (u64)size_of_alloc(header));
      ERROR("            Content: ");
      char* content = (char*)(u64)get_payload(header);
      for (u64 i = 0; i < (u64)size_of_alloc(header); ++i) {
        printf("%c", content[i]);
      }
      printf("\n");
    }
  }

  if (!liks) {
    ERROR("[LICKITUNG] No memory leaks detected\n");
  }

  return liks;
}

void* memcpy(void *dest, const void *src, size_t len) {
  char *d = dest;
  const char *s = src;
  while (len--)
    *d++ = *s++;
  return dest;
}

void *memset (void *dest, int val, size_t len) {
  unsigned char *ptr = dest;
  while (len-- > 0)
    *ptr++ = val;
  return dest;
}