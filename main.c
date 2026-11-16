#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MEMSIZE 64

typedef enum {
  FIRST,
  BEST,
  WORST,
  NEXT
} FitAlg;

typedef struct {
  int address;
  int size;
} Hole;

typedef struct {
  int address;
  int size;
  int id;
} Allocation;

char* init(int size){
  size_t memsize = size * sizeof(char);
  char* memory = malloc(memsize + 1);
  memory[memsize] = '\0';

  for(int i = 0; i < memsize; i++){
    memory[i] = '-';
  }

  return memory;
}

void fill(int size, char* memory, int address, int id){
  for(int i = address; i < address + size; i++){
    memory[i] = id + '0';
  }
}

int firstfit(int size, char* memory, int memsize, int id, int* next){
  int hole_address = -1;
  int hole_size = 0;
  int found_space = 0;
  for(int i = 0; i < memsize; i++){
    // skips if the space is being used
    if(memory[i] != '-') { 
      hole_address = -1;
      hole_size = 0; 
      continue; 
    }
    // stores the beginning of the hole
    if(hole_address == -1) hole_address = i; 
    // keeps counting the hole size
    hole_size++;

    if(hole_size >= size) { found_space = 1; break; }
  }

  if(found_space) { 
    fill(size, memory, hole_address, id); 
    *next = hole_address + size;
    return hole_address;
  }
  else printf("Error: No contiguous space for %i bytes", size);

  return -1;
}

int bestfit(int size, char* memory, int memsize, int id, int* next){
  int hole_address = -1;
  int hole_size = 0;
  int best_size = INT_MAX;
  int best_address = -1;
  int found_space = 0;
  for(int i = 0; i < memsize; i++){
    // skips if the space is being used
    if(memory[i] != '-') { 
      if(hole_size >= size && hole_size < best_size) {
        best_size = hole_size;
        best_address = hole_address;
        found_space = 1;
      }
      hole_address = -1;
      hole_size = 0; 
      continue; 
    }
    // stores the beginning of the hole
    if(hole_address == -1) hole_address = i; 
    // keeps counting the hole size
    hole_size++;
  }

  if(hole_size >= size && hole_size < best_size) {
    best_size = hole_size;
    best_address = hole_address;
    found_space = 1;
  }

  if(found_space) { 
    fill(size, memory, best_address, id); 
    *next = best_address + size;
    return best_address;
  }
  else printf("Error: No contiguos space for %i bytes", size);

  return -1;
}

int worstfit(int size, char* memory, int memsize, int id, int* next){
  int hole_address = -1;
  int hole_size = 0;
  int worst_size = INT_MIN;
  int worst_address = -1;
  int found_space = 0;
  for(int i = 0; i < memsize; i++){
    // skips if the space is being used
    if(memory[i] != '-') { 
      if(hole_size >= size && hole_size > worst_size) {
        worst_size = hole_size;
        worst_address = hole_address;
        found_space = 1;
      }
      hole_address = -1;
      hole_size = 0; 
      continue; 
    }
    // stores the beginning of the hole
    if(hole_address == -1) hole_address = i; 
    // keeps counting the hole size
    hole_size++;
  }

  if(hole_size >= size && hole_size > worst_size) {
    worst_size = hole_size;
    worst_address = hole_address;
    found_space = 1;
  }

  if(found_space) { 
    fill(size, memory, worst_address, id); 
    *next = worst_address + size;
    return worst_address;
  }
  else printf("Error: No contiguos space for %i bytes", size);
  
  return -1;
}

int nextfit(int size, char* memory, int memsize, int id, int* next){
  int hole_address = *next == MEMSIZE ? 0 : *next;
  int hole_size = 0;
  uint8_t found_space = 0;
  int i = *next;
  for(int count = 0; count < memsize; count++){
    if(memory[i] != '-') { 
      hole_address = -1; 
      hole_size = 0;
      i++;
      continue; 
    }
    if(hole_address == -1) hole_address = i;
    hole_size++;

    if(i >= memsize) { i = 0; hole_size = 0; hole_address = -1; }

    if(hole_size >= size) { found_space = 1; break; }
  }

  if(found_space) { 
    fill(size, memory, hole_address, id); 
    *next = i; 
    return hole_address;
  }
  else printf("Error: No contiguous space for %i bytes", size);

  return -1;
}

void freeid(int id, Allocation* allocs[], char* memory){
  int i = allocs[id-1]->address;

  while((memory[i] - '0') == allocs[id-1]->id){
    memory[i] = '-';
    i++;
  }
}

void freeaddr(int address, Allocation* allocs[], char* memory){
  int addr = -1;
  int id = -1;
  for(int i = 0; i < MEMSIZE; i++){
    if(allocs[i]->address != -1 && allocs[i]->address == address){
      addr = allocs[i]->address;
      id = i;
    }
  }

  while((memory[addr] - '0') == allocs[id]->id){
    memory[addr] = '-';
    addr++;
  }
}

int alloc(int size, FitAlg alg, char* memory, int memsize, int id, int* next){
  int address = -1;
  switch(alg){
    case 0:
      address = firstfit(size, memory, memsize, id, next);
      return address;
    case 1:
      address = bestfit(size, memory, memsize, id, next);
      return address;
    case 2:
      address = worstfit(size, memory, memsize, id, next);
      return address;
    case 3:
      address = nextfit(size, memory, memsize, id, next);
      return address;
    default:
      printf("Option non existent.");
      break;
  }

  return address;
}

void show(char* memory, int memsize){
  printf("[");
  for(int i = 0; i < memsize; i++){
    printf("%c", memory[i]);
  }
  printf("]");
}

int main(){
  char* memo = init(MEMSIZE);
  // [address, id]
  Allocation* allocs[MEMSIZE];
  for(int i = 0; i < MEMSIZE; i++) {
    allocs[i] = malloc(sizeof(Allocation));
    allocs[i]->address = -1;
    allocs[i]->id = -1;
  }
  int next = 0;
  int* next_pointer = &next;

  allocs[0]->address = alloc(10, FIRST, memo, MEMSIZE, 1, next_pointer);
  allocs[0]->id = 1;
  
  allocs[1]->address = alloc(10, FIRST, memo, MEMSIZE, 2, next_pointer);
  allocs[1]->id = 2;
  
  allocs[2]->address = alloc(10, FIRST, memo, MEMSIZE, 3, next_pointer);
  allocs[2]->id = 3;

  freeid(2, allocs, memo);
  
  allocs[3]->address = alloc(8, BEST, memo, MEMSIZE, 4, next_pointer);
  allocs[3]->id = 4;
  
  show(memo, MEMSIZE);

  return 0;
}