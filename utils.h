#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>

#include "ipc.h"

#define MAX_PROCESSES_COUNT 11

typedef struct{
    int fdRead;
    int fdWrite;
} Pipe;

typedef struct{
    int processes_count;
    Pipe *pipes[MAX_PROCESSES_COUNT][MAX_PROCESSES_COUNT];
    local_id current_id;
} Mesh;

typedef struct {
    FILE *events_log_file;
    FILE *pipes_log_file;
} Logger;
