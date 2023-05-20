#include "mm.h"
#include "printf.h"
#include <stddef.h>

/*

    TYPES OF BLOCK:
      - HEADER
      - PAYLOAD
      - FOOTER

----------------------------------------------------------------------------------------------

    .                 |  32 - 1 bit                        |   0 bit    |
    HEADER / FOOTER = | payload size in number of blocks   | allocated? |

    DATA = HEADER|PAYLOAD|FOOTER


    bez sbrk

*/


/* BLOCK TYPE */
typedef u32 word_t;


#define BLOCK_SIZE       sizeof(word_t)
#define METADATA_SIZE    (2 * BLOCK_SIZE)
#define MAX_ALLOC_SIZE   (HEAP_SIZE - METADATA_SIZE)
#define ALLOCATED        true
#define FREE             false


static u8 *heap;
static u8 *max_heap_address;


static word_t *first_block;
static word_t *last_block;

static size_t number_of_blocks(size_t size) {
  return size / BLOCK_SIZE;
}

/* return rounded up to ALIGNMENT size in bytes */
static size_t round_up(size_t size) {
  return (size + ALIGNMENT - 1) & -ALIGNMENT;
}

/* get size of data inside block based on header/footer */
static word_t get_size(word_t *hf) {
  return *hf & -2;
}

/* check if block is allocated based on header/footer */
static bool is_alocated(word_t *hf) {
  return *hf & 1;
}

/* get payload based on header*/
static word_t *get_payload(word_t *header) {
  return header + 1;
}

/* get header based on payload*/
static word_t *get_header(word_t *payload) {
  return payload + 1;
}

static word_t *next_header(word_t *header) {
  // sprawdzenie czy nie wychodzi za heap
  return header + get_size(header);
}

static word_t *prev_header(word_t *header) {
  return header - get_size(header - 1);
}

static void set_header(word_t *header, size_t size, bool is_allocated) {
  *header = size | is_allocated;
}

static void set_is_allocated(word_t *hf, bool is_allocated) {
  *hf = (*hf & -2) | is_allocated;
}

static void set_footer(word_t *header) {
  word_t *footer = header + get_size(header) - 1;
  *footer = *header;
}

static word_t *find_free_block(size_t size) {
  word_t *block = first_block;
  // sprawdz czy gówno
  while((is_alocated(block) || (get_size(block) < size)) && block != last_block){
    block = next_header(block);
  }
  return block;
}
/* add alignment ?*/
void mem_init(void) {
    heap = (u8 *)HEAP_ADDRESS;
    max_heap_address = (u8 *)HEAP_END_ADDRESS;
    first_block = (word_t *)heap;
    last_block = (word_t *)(max_heap_address - 4);
    set_header(first_block, number_of_blocks(HEAP_SIZE), FREE);
    set_footer(first_block);
}

void *mmalloc(size_t size) {
  size = round_up(size + METADATA_SIZE);
  word_t *header = find_free_block(size);
  size_t new_size = get_size(header) - size;
  set_header(header, number_of_blocks(size), ALLOCATED);
  set_footer(header);
  set_header(next_header(header), new_size, FREE);
  set_footer(next_header(header));
  return get_payload(header);
}

static void coalesce(word_t *first_block, word_t *second) {
  set_header(first_block, get_size(first_block) + get_size(second), FREE);
  set_footer(first_block);
}

void ffree(void *ptr) {
  word_t *header = get_header(ptr);
  printf("1\n");
  set_is_allocated(header, FREE);
  printf("2\n");
  set_footer(header);
  printf("3\n");
  if (last_block != header) {
    printf("4\n");
    word_t *next_h = next_header(header);
    printf("5\n");
    if (!is_alocated(next_h)){
        printf("6\n");
        coalesce(header, next_h);
        printf("7\n");
        if(last_block == next_h) {
          printf("8\n");
          last_block = header;
        }
    }
  }
  if (first_block != header) {
    printf("9\n");
    word_t *prev_h = prev_header(header);
    if(!is_alocated(prev_h)) {
      coalesce(prev_h, header);
      if(last_block == header)
        last_block = prev_h;
    }
  }
  printf("10\n");
}
