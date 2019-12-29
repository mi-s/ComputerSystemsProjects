#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cachelab.h"


typedef struct line{
  int age; // Used for LRU eviction (max age = E - 1)
  char validBit; 
  int tag; 
  int* block; // Holds an array of memory addresses
} line;

typedef struct set{
  line* lines; // line array
} set;

typedef struct cache{
  set* sets; // set array
} cache;

// Returns 2^x (input assumed to be positive)
int pow2(int x){
  if(x == 0){
    return 1;
  } else{
    return 2 * pow2(x - 1);
  }
}

/* Creates a new cache.  First, creates an array of sets.  Then, assigns each
   set an array of lines.  Each line has the validBit set to 0 and block 
   memory is malloc'd.  Then, assigns cache the array of sets 
   and returns cache.  */
cache* newCache(int sets, int lines, int blockSize){
  set* res_sets = (set*) malloc(sets * sizeof(set));
  for(int i = 0; i < sets; i++){
      line* res_lines = (line*) malloc(lines * sizeof(line));
      for(int j = 0; j < lines; j++){
          res_lines[j].validBit = 0;
      }
      res_sets[i].lines = res_lines;
  }
  cache* res = (cache*) malloc(sizeof(cache));
  res->sets = res_sets;
  return res;
}

/* Frees a cache's dynamically allocated memory */
void freeCache(cache* c, int sets, int lines){
    for(int i = 0; i < sets; i++){
        for(int j = 0; j < lines; j++){
            free(c->sets[i].lines[j].block);
        }
        free(c->sets[i].lines);
    }
    free(c->sets);
    free(c);
    return;
}

/* Returns the address of memory address in form of long integer, returns
   the number of bytes in address as an int */
int getAddress(char* traceLine, long* decAddr){
    int addrlen = 0;
    while(traceLine[3 + addrlen] != ','){
        addrlen++;
    }
    char* addr = (char*) malloc(addrlen * sizeof(char));
    int i = 0;
    do{
        addr[i] = traceLine[i + 3];
        i++;
    } while(i < addrlen);
    *(addr + addrlen) = '\0';
    *decAddr = strtol(addr, NULL, 16);
    free(addr);
    return addrlen;
}

/* Increments the age of all lines in a set with valid bits.
   The MRU line will have an age of -1 + 1 = 0 */
void updateAge(set* currSet, int lines){
    for(int i = 0; i < lines; i++){
        if(currSet->lines[i].validBit == 1){
            currSet->lines[i].age += 1;
        }
    }
    return;
}

/* Creates a line using the given tag, block size in bytes, and mem address */
line newLine(int tag, int blockSize, int decAddr){
    line resLine;
    resLine.validBit = 1;
    resLine.age = -1;
    resLine.tag = tag;
    int* resBlock = (int*) malloc(blockSize * sizeof(int));
    int byteNum = decAddr & (blockSize - 1);
    for(int i = 0; i < blockSize; i++){
        resBlock[i] = decAddr - byteNum + i;
    }
    resLine.block = resBlock;
    return resLine;
}       

/* Handles load instructions for the simulator, returning hits, misses,
   and evictions as out-parameters */
void checkHitMiss(set* currSet, int lines, int tag, int blockSize, long decAddr,
                  int* hits, int* misses, int* evictions){
    int firstInvalid = -1; // index of first invalid bit
    int lru = -1;  // index of lru line
    int oldestAge = 0;
    for(int i = 0; i < lines; i++){
        if((currSet->lines[i].validBit == 1) && (currSet->lines[i].tag == tag)){
            *hits += 1;
            currSet->lines[i].age = -1;
            updateAge(currSet, lines);
            return;
        } else if((firstInvalid == -1) && (currSet->lines[i].validBit == 0)){
            firstInvalid = i;
        } else if((currSet->lines[i].validBit == 1) && 
                  (currSet->lines[i].age >= oldestAge)){
            lru = i;
            oldestAge = currSet->lines[i].age;
        }
    }
    *misses += 1;
    if(firstInvalid != -1){
        currSet->lines[firstInvalid] = newLine(tag, blockSize, decAddr);
        updateAge(currSet, lines);
    } else{
        *evictions += 1;
        free(currSet->lines[lru].block);
        currSet->lines[lru] = newLine(tag, blockSize, decAddr);
        updateAge(currSet, lines);
    }
    return;
}

/* Prints "Hit", "Miss", and "Eviction" by checking if
   *hit, *miss, or *eviction have increased in a loop cycle */
void verboseCheck(int* oldHits, int* oldMisses, int* oldEvictions,
                  int* hits, int* misses, int* evictions){
    if(*misses > *oldMisses){
        *oldMisses = *misses;
        printf(" miss");
    }
    if(*evictions > *oldEvictions){
        *oldEvictions = *evictions;
        printf(" eviction");
    }
    if(*hits > *oldHits){
        *oldHits = *hits;
        printf(" hit");
    }
    return;
}
            
            
/* Simulates a cache with the given s, E, b, on a given memory trace.
   Stores the number of hits, misses, and evictions as out-parameters */
void simulate(int s, int E, int b, FILE* trace, int verbose,
              int* hits, int* misses, int* evictions){
    *hits = 0;
    *misses = 0;
    *evictions = 0;
    int sets = pow2(s),
        blockSize = pow2(b);
    cache* simCache = newCache(sets, E, blockSize);
    char* traceLine = NULL; // memory trace line
    size_t len = 0; // allocated size of line
    ssize_t read;
    if(verbose == 1){
        int oldHits = *hits;
        int oldMisses = *misses;
        int oldEvictions = *evictions;
        while((read = getline(&traceLine, &len, trace)) != -1){
            char* printLine = traceLine;
            printLine[strlen(printLine) - 1] = '\0';
            printf("%s", printLine);
            if(traceLine[0] == 'I'){continue;}
            long decAddr;
            int addrlen = getAddress(traceLine, &decAddr);
            int setNum = (decAddr >> b) & (sets - 1);
            set* currSet = &(simCache->sets[setNum]); 
            int maxTag = pow2((4 * addrlen) - s - b) - 1;
            int tag = (decAddr >> (s + b)) & maxTag;
            char acctype = traceLine[1];             
            if(acctype == 'M'){ // modify(read -> write)
                checkHitMiss(currSet, E, tag, blockSize, decAddr, hits, misses, evictions);
                verboseCheck(&oldHits, &oldMisses, &oldEvictions, 
                             hits, misses, evictions);
                checkHitMiss(currSet, E, tag, blockSize, decAddr, hits, misses, evictions);
                verboseCheck(&oldHits, &oldMisses, &oldEvictions, 
                             hits, misses, evictions);
            } else{
                checkHitMiss(currSet, E, tag, blockSize, decAddr, hits, misses, evictions);
                verboseCheck(&oldHits, &oldMisses, &oldEvictions, 
                             hits, misses, evictions);
            }
            printf("\n");
        }
    } else{
        while((read = getline(&traceLine, &len, trace)) != -1){
            if(traceLine[0] == 'I'){continue;}
            long decAddr;
            int addrlen = getAddress(traceLine, &decAddr);
            int setNum = (decAddr >> b) & (sets - 1);
            set* currSet = &(simCache->sets[setNum]); 
            int maxTag = pow2((4 * addrlen) - s - b) - 1;
            int tag = (decAddr >> (s + b)) & maxTag;
            char acctype = traceLine[1];            
            if(acctype == 'M'){ // modify(read -> write)
                checkHitMiss(currSet, E, tag, blockSize, decAddr, hits, misses, evictions);
                checkHitMiss(currSet, E, tag, blockSize, decAddr, hits, misses, evictions);
            } else{
                checkHitMiss(currSet, E, tag, blockSize, decAddr, hits, misses, evictions);
            }
        }
    }
    fclose(trace);
    free(traceLine);
    freeCache(simCache, sets, E);
    return;
} 

int main(int argc, char **argv){
    int verbose = 0,
        s, E, b, i;
    FILE* trace; 
    for(i = 1; i < argc; ++i){
        char* arg = *(argv + i);
        char* nextarg = *(argv + i + 1);
        if(strcmp(arg, "-v") == 0){
            verbose = 1;
        } else if(strcmp(arg, "-s") == 0){
            s = atoi(nextarg);
            i++;
        } else if(strcmp(arg, "-E") == 0){
            E = atoi(nextarg);
            i++;
        } else if(strcmp(arg, "-b") == 0){
            b = atoi(nextarg);
            i++;
        } else if(strcmp(arg, "-t") == 0){
            trace = fopen(nextarg, "r");
            i++;
        }
    }
    int hits, misses, evictions;
    simulate(s, E, b, trace, verbose, &hits, &misses, &evictions);  
    printSummary(hits, misses, evictions);
    return 0;
}
