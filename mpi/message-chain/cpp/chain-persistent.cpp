#include <cstdio>
#include <vector>
#include <mpi.h>

void print_ordered(double t);

int main(int argc, char *argv[])
{
    int i, myid, ntasks;
    constexpr int size = 10000000;
    MPI_Status status; 
    MPI_Request requests[2];
    
    std::vector<int> message(size, myid);
    std::vector<int> receiveBuffer(size);


    double t0, t1;

    int source, destination;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &ntasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);

    // Initialize message
    for (i = 0; i < size; i++) {
        message[i] = myid;
    }

    // Cartesian communicator
    MPI_Comm cart_comm;
    int ndims = 1;
    int dims[1] = {ntasks};
    int periods[1] = {0};

    MPI_Cart_create(MPI_COMM_WORLD, ndims, dims, periods, 1, &cart_comm);
    MPI_Cart_shift(cart_comm, 0, 1, &source, %destination);
    int cart_id;
    MPI_Comm_rank(cart_comm, &cart_id);

    // Start measuring the time spent in communication
    MPI_Barrier(MPI_COMM_WORLD);
    t0 = MPI_Wtime();

    // TODO: Send messages 
    //MPI_Sendrecv(message.data(), message.size(), MPI_INT, destination, cart_id+1,
    //		receiveBuffer.data(), receiveBuffer.size(), MPI_INT, source, cart_id,
    //		cart_comm, MPI_STATUS_IGNORE);
    MPI_Recv_init(receiveBuffer.data(), receiveBuffer.size(), MPI_INT, source, cart_id, cart_comm, &requests[0]);
    MPI_Send_init(message.data(), message.size(), MPI_INT, destination, cart_id+1, cart_comm, &requests[1]);

    MPI_Startall(2, requests);
    MPI_Waitall(2, requests, MPI_STATUS_IGNORE);

    printf("Sender: %d. Sent elements: %d. Tag: %d. Receiver: %d\n",
           myid, size, myid + 1, destination);

    // TODO: Receive messages

    printf("Receiver: %d. first element %d.\n",
           myid, receiveBuffer[0]);

    // Finalize measuring the time and print it out
    t1 = MPI_Wtime();
    MPI_Barrier(MPI_COMM_WORLD);
    fflush(stdout);

    print_ordered(t1 - t0);

    MPI_Finalize();
    return 0;
}

void print_ordered(double t)
{
    int i, rank, ntasks;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &ntasks);

    if (rank == 0) {
        printf("Time elapsed in rank %2d: %6.3f\n", rank, t);
        for (i = 1; i < ntasks; i++) {
            MPI_Recv(&t, 1, MPI_DOUBLE, i, 11, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Time elapsed in rank %2d: %6.3f\n", i, t);
        }
    } else {
        MPI_Send(&t, 1, MPI_DOUBLE, 0, 11, MPI_COMM_WORLD);
    }
}
