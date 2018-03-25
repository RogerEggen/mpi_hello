#include <stdio.h>
#include <mpi/mpi.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

void printStringArray(char* array, int count);

void replaceNulls(char* str, int len, char replaceWith);

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    const int MESSAGE_LENGTH_MAX = MPI_MAX_PROCESSOR_NAME * 2;

    int rank, worldSize;

    int error = MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (error)
        printf("get rank failed: error %d\n", error);

    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
    if (error)
        printf("get size failed: error %d\n", error);

    char msg[MESSAGE_LENGTH_MAX];
    int recvBufSize = MESSAGE_LENGTH_MAX * sizeof(char) * worldSize;
    char* recvBuf;

    if (rank == 0)
    {
        // Initialize receive buffer

        recvBuf = malloc((size_t)recvBufSize);
        memset(recvBuf, (int)'a', (size_t)recvBufSize);
        recvBuf[recvBufSize-1] = '\0';
    }

    // Do work to create data to be gathered...
    char* pName = malloc(sizeof(char) * MPI_MAX_PROCESSOR_NAME);
    int pNameLength;
    MPI_Get_processor_name(pName, &pNameLength);
    //pName[pNameLength] = '\0';
    char* pHostName = malloc(sizeof(char) * MPI_MAX_PROCESSOR_NAME);
    gethostname(pHostName, MPI_MAX_PROCESSOR_NAME);
    sprintf(msg, "Hello from process %d/%d. I am \"%s\", running on %s.", rank, worldSize,
            pName, pHostName);

    error = MPI_Gather(msg, // what to send
                       MESSAGE_LENGTH_MAX, // length of what to send
                       MPI_CHAR, // send type
                       recvBuf, // where to receive
                       MESSAGE_LENGTH_MAX, // length of what to be received (per sender)
                       MPI_CHAR, // receive type
                       0, // who will receive
                       MPI_COMM_WORLD); // world
    if (error)
        printf("gather failed: error %d\n", error);

    if (rank == 0)
    {
        printf("World size is %d.\n", worldSize);
        for (int i = 0; i < worldSize; ++i)
        {
            printf("%s\n", &recvBuf[i * MESSAGE_LENGTH_MAX]);
        }
        //printStringArray(recvBuf, worldSize);
    }


    MPI_Finalize();

    return 0;
}

void replaceNulls(char* str, int len, char replaceWith)
{
    for (int i=0; i<len; ++i)
    {
        char c = str[i];
        if (c == '\0')
            str[i] = replaceWith;
    }
}

void printStringArray(char* array, int count)
{
    int startIdx = 0;
    for (int i=0; i < count; )
    {
        printf("%s\n", &array[startIdx]);
        int mylen = (int)strlen(&array[startIdx]);
        //printf("just printed %d chars from %d to %d.\n", mylen, startIdx, startIdx + mylen);
        startIdx += mylen + 1;
        ++i;
    }
}