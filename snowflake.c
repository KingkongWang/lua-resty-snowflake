/**
 * original version(scala):
 * https://github.com/twitter-archive/snowflake/blob/snowflake-2010/src/main/scala/com/twitter/service/snowflake/IdWorker.scala
 * @author KingkongWang
 */

#include <stdbool.h>
#include <sys/time.h>
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>


/**
 * 2019-01-01T00:00:00Z
 */
#define SNOWFLAKE_EPOC 1546272000000L 

#define WORKER_ID_BITS 5
#define DATACENTER_ID_BITS 5
#define MAX_WORKER_ID (-1L ^ (-1L << WORKER_ID_BITS))
#define MAX_DATACENTER_ID (-1L ^ (-1L << DATACENTER_ID_BITS))
#define SEQUENCE_BITS 12


#define WORKER_ID_SHIFT SEQUENCE_BITS
#define DATACENTER_ID_SHIFT (SEQUENCE_BITS + WORKER_ID_BITS)
#define TIMESTAMP_SHIFT (DATACENTER_ID_SHIFT + DATACENTER_ID_BITS)
#define SEQUENCE_MASK (-1L ^ (-1L << SEQUENCE_BITS))

typedef struct snowflake{
    int worker_id;
    int datacenter_id;
    int sequence;
    int64_t last_timestamp;
    bool initialized;
}snowflake_t;

static int64_t time_gen() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (int64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

static int64_t til_next_millis(int64_t last_timestamp) {
    int64_t ts = time_gen();
    while (ts < last_timestamp) {
        ts = time_gen();
    }

    return ts;
}

bool snowflake_init(snowflake_t* context,int worker_id, int datacenter_id) {
    if(context == NULL) {
        return false;
    }

    if (worker_id > MAX_WORKER_ID || worker_id < 0) {
        return false;
    }

    if (datacenter_id > MAX_DATACENTER_ID || datacenter_id < 0) {
        return false;
    }
    
    context->worker_id = worker_id;
    context->datacenter_id = datacenter_id;
    context->sequence = 0;
    context->last_timestamp = -1;
    context->initialized = true;


    return true;
}

bool snowflake_next_id(snowflake_t* context, int64_t* id) {
    int64_t ts;
    if(context == NULL) {
        //printf("context is null\n");
        return false;
    }

    if (!context->initialized) {
        //printf("initialized is error\n");
        return false;
    }

    ts = time_gen();
    if (ts < context->last_timestamp) {
        //printf("ts error\n");
        return false;
    }

    if (context->last_timestamp == ts) {
        context->sequence = (context->sequence + 1) & SEQUENCE_MASK;
        if (context->sequence == 0) {
            ts = til_next_millis(context->last_timestamp);
        }
    } else {
        context->sequence = 0;
    }

    context->last_timestamp = ts;
    ts = ((ts - SNOWFLAKE_EPOC) << TIMESTAMP_SHIFT) |
            (context->datacenter_id << DATACENTER_ID_SHIFT) |
            (context->worker_id << WORKER_ID_SHIFT) | 
            context->sequence;

    //printf("ts:%ld\n", time_gen());
    //printf("epoc:%lld\n", SNOWFLAKE_EPOC);
    //printf("sequence:%d\n", context->sequence);
    //printf("%llx\n", ts);
    //printf("%lld\n", ts);
    *id = ts;

    return true;
}

