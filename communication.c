#include <sys/types.h>

#include <communication.h>

Mesh *mesh;

void initialize(int processes_count){
    initLogger();
    initMesh(processes_count + 1);
    createMeshProcesses();
}

void initMesh(int processes_count){
    mesh = (Mesh*)malloc(sizeof(Mesh));
    mesh->processes_count = processes_count;
    mesh->current_id = getpid();
    createMeshPipes();
}

void createMeshProcesses(){
    for (int i = 1; i < mesh->processes_count; i++) {
        pid_t pid = fork();
        if (pid == -1) {
            printf("Cannot create fork process");
            exit(1);
        } else if (pid == 0) {
            mesh->current_id = i;
            startChild();
            exit(0);
        }
    }
    mesh->current_id = 0;
    startParent();
}

void createMeshPipes(){
    int fdPipes[2];
    for (int i = 0; i < mesh->processes_count; i++) {
        for (int j = 0; j < mesh->processes_count; j++) {
            if (i != j) {
                mesh->pipes[i][j] = (Pipe*)malloc(sizeof(Pipe));
                createSinglePipe(fdPipes);
                setPipeFileDescriptors(i, j, fdPipes[0], fdPipes[1], mesh);
                logPipe(mesh->current_id, i, j, mesh->pipes[i][j]->fdRead, mesh->pipes[j][i]->fdWrite);
            }
        }
    }
}

void startChild(){
    closeUnusedPipes();
    handleEvent(EVENT_STARTED, mesh);
    handleEvent(EVENT_DONE, mesh);
    closeLinePipes();
}

void startParent(){
    handleEvent(EVENT_RECEIVED_ALL_STARTED, mesh);
    handleEvent(EVENT_RECEIVED_ALL_DONE, mesh);
    int value = 666;
    while (value > 0) {
        value = wait(NULL);
    }
    closeUnusedPipes();
    closeLogger();
    exit(0);
}

void closeLineCommunication(){
    for (int i = 0; i < mesh->processes_count; i++) {
        if (i != mesh->current_id) {
            closeSinglePipe(mesh->pipes[mesh->current_id][i]);
        }
    }
}

void closeUnusedPipes(){
    for (int i = 0; i < mesh->processes_count; i++) {
        for (int j = 0; j < mesh->processes_count; j++) {
            if (isProcessPairUnused(i, j, mesh->current_id)) {
                closeSinglePipe(mesh->pipes[i][j]);
            }
        }
    }
}

void createSinglePipe(int* fdPipes){
    if (pipe(fdPipes) < 0) {
        perror("pipe creation failed");
        exit(1);
    }
}

void setPipeFileDescriptors(int process1, int process2, int fdRead, int fdWrite, Mesh* mesh){
    mesh->pipes[process1][process2]->fdRead = fdRead;
    mesh->pipes[process2][process1]->fdWrite = fdWrite;
}

void closeSinglePipe(Pipe* pipe){
    logPipe(mesh->current_id, 0, 0, pipe->fdRead, pipe->fdWrite);
    close(pipe->fdRead);
    close(pipe->fdWrite);
}

bool isProcessPairUnused(int process1, int process2, int current_id){
    return process1 != process2 && process1 != current_id 
                                && process2 != current_id;
}