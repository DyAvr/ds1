#include <stdlib.h>

#include "logging.h"

Logger logger;

void initLogger(){
    logger.events_log_file = fopen(events_log, "w");
    if (logger.events_log_file == NULL) {
        perror("Error opening events log file");
        exit(1);
    }
    logger.pipes_log_file = fopen(pipes_log, "w");
    if (logger.pipes_log_file == NULL) {
        perror("Error opening pipes log file");
        exit(1);
    }
}

void closeLogger(){
    if (fclose(logger.events_log_file) != 0) {
        perror("Error closing events log file");
    }
    if (fclose(logger.pipes_log_file) != 0) {
        perror("Error closing pipes log file");
    }
}

char* logEvent(local_id id, EventStatus status){
    char buf[255];

    //printf("%1d: %u\n", id, status);

    switch(status) {
        case EVENT_STARTED:
            sprintf(buf, log_started_fmt, id, getpid(), getppid());
            break;
        case EVENT_RECEIVED_ALL_STARTED:
            sprintf(buf, log_received_all_started_fmt, id);
            break;
        case EVENT_DONE:
            sprintf(buf, log_done_fmt, id);
            break;
        case EVENT_RECEIVED_ALL_DONE:
            sprintf(buf, log_received_all_done_fmt, id);
            break;
    }

    printf("%s", buf);
    fprintf(logger.events_log_file, "%s", buf);
    fflush(logger.events_log_file);

    char *result = (char*)malloc(strlen(buf)+1);
    strcpy(result, buf);
    return result;
}

void logPipe(local_id id, int p1, int p2, int ds_read, int ds_write){
    char buf[100];

    if (p1 == 0 && p2 == 0){
        sprintf(buf, log_pipe_closed_fmt, id, ds_read, ds_write);
    } else {
        sprintf(buf, log_pipe_open_fmt, id, p1, p2, ds_read, ds_write);
    }

    fprintf(logger.pipes_log_file, "%s", buf);
    fflush(logger.pipes_log_file);
}
