#include "cachelab.h"
#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/queue.h>

typedef struct {
    short valid;
    long long int tag;
    int counter;
} Block;

typedef struct {
    Block *blocks;
} Set;

typedef struct {
    Set *sets;
} Cache;

typedef struct {
    int hits;
    int miss;
    int evictions;
} Cache_stats;

void handle_op(Cache *cache, Cache_stats *stats, int s_val, int E, long long int tag, int *most_c){
    int empty_index = -1;
    int lru_index = -1;
    long long lru_counter = (long long)((unsigned long long)~0ULL >> 1);
    for(int i=0; i<E; i++){
        if(cache->sets[s_val].blocks[i].valid && cache->sets[s_val].blocks[i].tag == tag){
            printf("Hit for Set:%d Tag:%lld\n", s_val, tag);
            stats->hits++;
            *most_c = *most_c + 1;
            cache->sets[s_val].blocks[i].counter = *most_c;
            return;
        }
        if (!cache->sets[s_val].blocks[i].valid){
            empty_index = i;
        }
        if(cache->sets[s_val].blocks[i].counter<lru_counter){
            lru_counter = cache->sets[s_val].blocks[i].counter;
            lru_index = i;
        }
    }
    int index;
    stats->miss++;
    if(empty_index==-1){
        printf("Miss and eviction for Set:%d Tag:%lld, evicted index=%d\n", s_val, tag, lru_index);
        //miss and eviction
        stats->evictions++;
        index = lru_index;
    }else{
        printf("Miss and add for Set:%d Tag:%lld\n", s_val, tag);
        //miss and add
        index = empty_index;
    }
    *most_c = *most_c + 1;
    cache->sets[s_val].blocks[index].valid = 1;
    cache->sets[s_val].blocks[index].tag = tag;
    cache->sets[s_val].blocks[index].counter = *most_c;
    return;
    
}

int main(int argc, char *argv[])
{
    int opt;
    int s, E, b;
    char *trace_file = NULL;
    int verbose = 0;

    // Loop through all provided arguments
    while ((opt = getopt(argc, argv, "s:E:b:t:v")) != -1) {
        switch (opt) {
            case 's':
                s = atoi(optarg); // optarg holds the string after the -s
                break;
            case 'E':
                E = atoi(optarg);
                break;
            case 'b':
                b = atoi(optarg);
                break;
            case 't':
                trace_file = optarg;
                break;
            case 'v':
                verbose = 1; // Optional flag
                break;
            default:
                fprintf(stderr, "Usage: %s -s <s> -E <E> -b <b> -t <trace>\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    (void)trace_file;
    
    if (verbose) {
        printf("s: %d, E: %d, b: %d, Trace: %s\n", s, E, b, trace_file);
    }
    
    FILE *file = fopen(trace_file, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Could not open trace file %s\n", trace_file);
        exit(1);
    }

    int S = 1<<s;
    int s_mask = S-1;
    int s_val;
    long long int tag;

    //allocating memory for set
    Cache cache;
    cache.sets = malloc(S * sizeof(Set));

    for(int i=0; i<S; i++){
        cache.sets[i].blocks = malloc(E * sizeof(Block));
        for(int j=0; j<E; j++){
            cache.sets[i].blocks[j].valid = 0;
        }
    }

    Cache_stats stats;
    stats.hits = 0;
    stats.miss = 0;
    stats.evictions = 0;

    char op;
    unsigned long long addr;
    int size;
    int most_c = 0;

    // The space before %c is CRITICAL: it skips the leading space in the trace file
    while (fscanf(file, " %c %llx,%d", &op, &addr, &size) == 3) {
        
        if (op == 'I') {
            continue; // The lab instructions say to ignore 'I' (Instruction loads)
        }

        s_val = addr>>b & s_mask;
        tag = addr >> (s+b);

        if (verbose) {
            printf("%c %llx,%d -- set: %d, tag: %lld ----->", op, addr, size, s_val, tag);
        }

        // Now, call your cache logic based on the operation
        if(op=='L' || op=='S'){ handle_op(&cache, &stats, s_val, E, tag, &most_c);}
        else if(op=='M'){ 
            handle_op(&cache, &stats, s_val, E, tag, &most_c);
            handle_op(&cache, &stats, s_val, E, tag, &most_c);
        }
        
        
        if (verbose) printf("\n");
    }

    fclose(file);

    for(int i=0; i<S; i++){
        free(cache.sets[i].blocks);
    }
    free(cache.sets);


    printSummary(stats.hits, stats.miss, stats.evictions);
    return 0;
}
