#include "messages.h"

Message createMessage(uint16_t magic, local_id id, MessageType type, timestamp_t time){
    Message message;
    MessageHeader messageHeader;
    char * buf = "";

    if (type == DONE) {
        buf = logEvent(EVENT_DONE, id);
    } 

    if (type == STARTED){
        buf = logEvent(EVENT_STARTED, id);
    }

    messageHeader = createMessageHeader(magic, strlen(buf), type, time);
    message.s_header = messageHeader;
    strcpy(message.s_payload, buf);
    return message;
}

MessageHeader createMessageHeader(uint16_t magic, uint16_t p_len, int16_t type, timestamp_t l_time){
    MessageHeader header;
    header.s_magic = MESSAGE_MAGIC;
    header.s_payload_len = p_len;
    header.s_type = type;
    header.s_local_time = l_time;
    return header;
}

void handleEvent(EventStatus status, Mesh* mesh){
    switch (status) {
        case EVENT_STARTED:
            sendStartedSignal(mesh);
            waitForAllStarted(mesh);
            break;
        case EVENT_DONE:
            sendDoneSignal(mesh);
            waitForAllDone(mesh);
            break;
        case EVENT_RECEIVED_ALL_STARTED:
            waitForAnyStarted(mesh);
            break;
        case EVENT_RECEIVED_ALL_DONE:
            waitForAnyDone(mesh);
            break;
        default:
            printf("Invalid event status");
            exit(1);
    }
}

void sendStartedSignal(Mesh* mesh) {
    unsigned long current_time = time(NULL);
    Message send = createMessage(MESSAGE_MAGIC, mesh->current_id, STARTED, current_time);
    if(send_multicast(mesh, &send) != 0) {
        printf("Can't send multicast");
        exit(1);
    }
}

void waitForAllStarted(Mesh* mesh) {
    for(int i = 0; i < mesh->processes_count; i++) {
        Message msg;
        receive_any(mesh, &msg);
        logEvent(EVENT_RECEIVED_ALL_STARTED, mesh->current_id);
    }
}

void sendDoneSignal(Mesh* mesh) {
    unsigned long current_time = time(NULL);
    Message send = createMessage(MESSAGE_MAGIC, mesh->current_id, DONE, current_time);
    if(send_multicast(mesh, &send) != 0) {
        printf("Can't send multicast");
        exit(1);
    }
}

void waitForAllDone(Mesh* mesh) {
    for(int i = 0; i < mesh->processes_count; i++) {
        Message msg;
        receive_any(mesh, &msg);
        logEvent(EVENT_RECEIVED_ALL_DONE, mesh->current_id);
    }
}

void waitForAnyStarted(Mesh* mesh) {
    Message msg;
    receive_any(mesh, &msg);
    logEvent(EVENT_RECEIVED_ALL_STARTED, mesh->current_id);
}

void waitForAnyDone(Mesh* mesh) {
    Message msg;
    receive_any(mesh, &msg);
    logEvent(EVENT_RECEIVED_ALL_DONE, mesh->current_id);
}
