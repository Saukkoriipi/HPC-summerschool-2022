#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <hdf5.h>
#include <mpi.h>

#define DATASIZE   64
#define WRITER_ID   0

void h5_writer(int, int *, int);


int main(int argc, char *argv[])
{
    int my_id, ntasks, i, localsize;
    int *localvector;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &ntasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_id);

    if (ntasks > 64) {
        fprintf(stderr, "Datasize (64) should be divisible by number "
                "of tasks.\n");
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }
    if (DATASIZE % ntasks != 0) {
        fprintf(stderr, "Datasize (64) should be divisible by number "
                "of tasks.\n");
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    localsize = DATASIZE / ntasks;
    localvector = (int *) malloc(localsize * sizeof(int));
    for (i = 0; i < localsize; i++)
        localvector[i] = i + 1 + localsize * my_id;

    h5_writer(my_id, localvector, localsize);

    free(localvector);
    MPI_Finalize();
    return 0;
}

void h5_writer(int my_id, int *localvector, int localsize)
{
    herr_t status;
    hid_t plist_id, dset_id, filespace, memspace, file_id;
    hsize_t dims, counts, offsets;

    /* Create the handle for parallel file access property list
       and create a new file */
    plist_id = H5Pcreate(H5P_FILE_ACCESS);
    H5Pset_fapl_mpio(plist_id, MPI_COMM_WORLD, MPI_INFO_NULL);
    file_id = H5Fcreate("data.h5", H5F_ACC_TRUNC, H5P_DEFAULT, plist_id);
    H5Pclose(plist_id);

    /* Create the dataset */
    dims = DATASIZE;
    filespace = H5Screate_simple(1, &dims, NULL);
    dset_id = H5Dcreate(file_id, "data", H5T_NATIVE_INT, filespace,
                        H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    H5Sclose(filespace);

    /* Select a hyperslab of the file dataspace */
    counts = localsize;
    offsets = my_id * localsize;
    filespace = H5Dget_space(dset_id);
    H5Sselect_hyperslab(filespace, H5S_SELECT_SET, &offsets, NULL, &counts,
                        NULL);

    /* Now we can write our local data to the correct position in the
       dataset. Here we use collective write, but independent writes are
       also possible. */
    memspace = H5Screate_simple(1, &counts, NULL);
    plist_id = H5Pcreate(H5P_DATASET_XFER);
    H5Pset_dxpl_mpio(plist_id, H5FD_MPIO_COLLECTIVE);

    status = H5Dwrite(dset_id, H5T_NATIVE_INT, memspace, filespace,
                      plist_id, localvector);

    /* Close all opened HDF5 handles */
    H5Dclose(dset_id);
    H5Pclose(plist_id);
    H5Sclose(filespace);
    H5Sclose(memspace);
    H5Fclose(file_id);
}
