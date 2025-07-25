/*
 *  * See COPYRIGHT in top-level directory.
 *  */

/*
 *  * 2D stencil code using a nonblocking send/receive with manual packing/unpacking.
 *  *
 *  * 2D regular grid is divided into px * py blocks of grid points (px * py = # of processes.)
 *  * In every iteration, each process calls nonblocking operations to exchange a halo with
 *  * neighbors. Grid points in a halo are packed and unpacked before and after communications.
 *  */

#include "stencil_par.h"
#include <stdbool.h>

/* row-major order */
#define ind(i,j) ((j)*(bx+2)+(i))

int ind_f(int i, int j, int bx)
{
    return ind(i, j);
}

void setup(int rank, int proc, int argc, char **argv,
           int *n_ptr, int *energy_ptr, int *niters_ptr, int *px_ptr, int *py_ptr, int *final_flag);

void init_sources(int bx, int by, int offx, int offy, int n,
                  const int nsources, int sources[][2], int *locnsources_ptr, int locsources[][2]);

void alloc_bufs(int bx, int by,
                double **aold_ptr, double **anew_ptr,
                double **sbufnorth_ptr, double **sbufsouth_ptr,
                double **sbufeast_ptr, double **sbufwest_ptr,
                double **rbufnorth_ptr, double **rbufsouth_ptr,
                double **rbufeast_ptr, double **rbufwest_ptr);

void pack_data(int bx, int by, double *aold,
               double *sbufnorth, double *sbufsouth, double *sbfueast, double *sbufwest);

void unpack_data(int bx, int by, double *aold,
                 double *rbufnorth, double *rbufsouth, double *rbufeast, double *rbufwest);

void update_grid(int bx, int by, double *aold, double *anew, double *heat_ptr);

void free_bufs(double *aold, double *anew,
               double *sbufnorth, double *sbufsouth,
               double *sbufeast, double *sbufwest,
               double *rbufnorth, double *rbufsouth, double *rbufeast, double *rbufwest);

int main(int argc, char **argv)
{
    int rank, size;
    int n, energy, niters, px, py;

    int rx, ry;
    int north, south, west, east;
    int bx, by, offx, offy;

    /* three heat sources */
    const int nsources = 3;
    int sources[nsources][2];
    int locnsources;              /* number of sources in my area */
    int locsources[nsources][2];  /* sources local to my rank */

    double t1, t2;

    int iter, i, out_freq;

    double *sbufnorth, *sbufsouth, *sbufeast, *sbufwest;
    double *rbufnorth, *rbufsouth, *rbufeast, *rbufwest;
    double *aold, *anew, *tmp;

    double heat, rheat;

    int final_flag;
    MPI_Request reqs[8];
    MPI_Status array_of_statuses[8];

    /* initialize MPI environment */
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /* argument checking and setting */
    setup(rank, size, argc, argv, &n, &energy, &niters, &px, &py, &final_flag);

    /* set output frequency */
    out_freq = 1000;

    if (final_flag == 1) {
        MPI_Finalize();
        exit(0);
    }

    /* ======================================================================================= */
    /*  Step 1. Create MPI_Cart_create
     *    Summary:
     *      Makes a new communicator to which topology information
     *      has been attached
     *
     *    Function Call:
     *      int MPI_Cart_create(MPI_Comm comm_old, int ndims, const int dims[],
     *                          const int periods[], int reorder,
     *                          MPI_Comm *comm_cart);
     *
     *   Input Parameters:
     *       comm_old
     *           input communicator (handle)
     *       ndims
     *           number of dimensions of cartesian grid (integer)
     *       dims
     *           integer array of size ndims specifying the number of processes in each dimension
     *       periods
     *           logical array of size ndims specifying whether the grid is periodic (true) or not (false) in each dimension
     *       reorder
     *           ranking may be reordered (true) or not (false) (logical)
     *
     *   Output Parameters
     *       comm_cart
     *           communicator with new cartesian topology (handle)
     */
    // TODO: get a new cartesian communicator (comm_cart)
    int dim = //TODO;
    int dims[] = //TODO;
    int periods[] = //TODO;
    int reorder = //TODO;
    MPI_Comm comm_cart;
    //TODO: create comm_cart communicator
    /* ======================================================================================= */

    /* ======================================================================================= */
    /* Step 2. create communication neighbors via MPI_Cart_Shift
     *  Summary:
     *       Returns the shifted source and destination ranks, given a shift
     *       direction and amount
     *
     *  Function Call:
     *       int MPI_Cart_shift(MPI_Comm comm, int direction, int disp,
     *                          int *rank_source, int *rank_dest);
     *
     *  Input Parameters:
     *       comm
     *           communicator with cartesian structure (handle)
     *       direction
     *           coordinate dimension of shift (integer)
     *       disp
     *           displacement (> 0: upwards shift, < 0: downwards shift) (integer)
     *
     *  Output Parameters:
     *       rank_source
     *           rank of source process (integer)
     *       rank_dest
     *           rank of destination process (integer)
     */
    // TODO: set west,east,south,north using MPI_Cart_shift

    /* ======================================================================================= */

    /* ======================================================================================= */
    /* Step 3. determine rank coordinates (rx,ry) in new communicator using MPI_Cart_coords
     *   Summary:
     *
     *   Function Call:
     *      int MPI_Cart_coords(MPI_Comm comm, int rank, int maxdims, int coords[]);
     *
     *   Input Parameters:
     *      comm
     *           communicator with cartesian structure (handle)
     *      rank
     *          rank of a process within group of comm (integer)
     *      maxdims
     *          length of vector coords in the calling program (integer)
     *
     *   Output Parameters:
     *      coords
     *          integer array (of size ndims) containing the Cartesian
     *          coordinates of specified process (integer)
     */
    int coords[2];
    // TODO: get 'coords'
    
    // TODO: set rx,ry from 'coords'
    rx = //TODO;
    ry = //TODO;
    /* ======================================================================================= */

    /* decompose the domain */
    bx = n / px;        /* block size in x */
    by = n / py;        /* block size in y */
    offx = rx * bx;     /* offset in x */
    offy = ry * by;     /* offset in y */

    printf("Rank[%2i]\t (%i,%i) - ",rank,ry,rx);
    (west < 0) ? printf("[w]: %i, ", west):printf("[w]: %2i, ",west);
    (east < 0) ? printf("[e]: %i, ", east):printf("[e]: %2i, ",east);
    (north< 0) ? printf("[n]: %i, ",north):printf("[n]: %2i, ",north);
    (south< 0) ? printf("[s]: %i  ",south):printf("[s]: %2i  ",south);
    printf("\n");

    /* initialize three heat sources */
    init_sources(bx, by, offx, offy, n, nsources, sources, &locnsources, locsources);

    /* allocate working arrays & communication buffers */
    alloc_bufs(bx, by, &aold, &anew,
               &sbufnorth, &sbufsouth, &sbufeast, &sbufwest,
               &rbufnorth, &rbufsouth, &rbufeast, &rbufwest);

    t1 = MPI_Wtime();   /* take time */
    for (iter = 0; iter < niters; ++iter) {

        /* refresh heat sources */
        for (i = 0; i < locnsources; ++i) {
            aold[ind(locsources[i][0], locsources[i][1])] = energy;    /* heat source */
        }

        /* pack data */
        pack_data(bx, by, aold, sbufnorth, sbufsouth, sbufeast, sbufwest);

        /* exchange data with neighbors */
        MPI_Irecv(rbufnorth, bx, MPI_DOUBLE, north, 9, MPI_COMM_WORLD, &reqs[0]);
        MPI_Irecv(rbufsouth, bx, MPI_DOUBLE, south, 9, MPI_COMM_WORLD, &reqs[1]);
        MPI_Irecv(rbufeast, by, MPI_DOUBLE, east, 9, MPI_COMM_WORLD, &reqs[2]);
        MPI_Irecv(rbufwest, by, MPI_DOUBLE, west, 9, MPI_COMM_WORLD, &reqs[3]);

        MPI_Isend(sbufnorth, bx, MPI_DOUBLE, north, 9, MPI_COMM_WORLD, &reqs[4]);
        MPI_Isend(sbufsouth, bx, MPI_DOUBLE, south, 9, MPI_COMM_WORLD, &reqs[5]);
        MPI_Isend(sbufeast, by, MPI_DOUBLE, east, 9, MPI_COMM_WORLD, &reqs[6]);
        MPI_Isend(sbufwest, by, MPI_DOUBLE, west, 9, MPI_COMM_WORLD, &reqs[7]);


        MPI_Waitall(8, reqs, array_of_statuses);

        /* unpack data */
        unpack_data(bx, by, aold, rbufnorth, rbufsouth, rbufeast, rbufwest);

        /* update grid points */
        update_grid(bx, by, aold, anew, &heat);

        /* swap working arrays */
        tmp = anew;
        anew = aold;
        aold = tmp;

        /* optional - print image */
        if (iter % out_freq == 0 || iter == niters - 1) {
            if (!rank) printf("Outputting state: %d\n",iter);
            printarr_par(iter, anew, n, px, py, rx, ry, bx, by, offx, offy, ind_f, MPI_COMM_WORLD, size);
        }
    }
    t2 = MPI_Wtime();

    /* free working arrays and communication buffers */
    free_bufs(aold, anew, sbufnorth, sbufsouth, sbufeast, sbufwest,
              rbufnorth, rbufsouth, rbufeast, rbufwest);

    /* get final heat in the system */
    MPI_Allreduce(&heat, &rheat, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    if (!rank)
        printf("[%i] last heat: %f time: %f\n", rank, rheat, t2 - t1);

    MPI_Finalize();
    return 0;
}

void setup(int rank, int proc, int argc, char **argv,
           int *n_ptr, int *energy_ptr, int *niters_ptr, int *px_ptr, int *py_ptr, int *final_flag)
{
    int n, energy, niters, px, py;

    (*final_flag) = 0;

    if (argc < 6) {
        if (!rank)
            printf("usage: stencil_mpi <n> <energy> <niters> <px> <py>\n");
        (*final_flag) = 1;
        return;
    }

    n = atoi(argv[1]);          /* nxn grid */
    energy = atoi(argv[2]);     /* energy to be injected per iteration */
    niters = atoi(argv[3]);     /* number of iterations */
    px = atoi(argv[4]);         /* 1st dim processes */
    py = atoi(argv[5]);         /* 2nd dim processes */

    if (px * py != proc) {
        fprintf(stderr, "px * py must equal to the number of processes.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);   /* abort if px or py are wrong */
    }
    if (n % px != 0) {
        fprintf(stderr, "grid size n must be divisible by px.\n");
        MPI_Abort(MPI_COMM_WORLD, 2);   /* abort px needs to divide n */
    }
    if (n % py != 0) {
        fprintf(stderr, "grid size n must be divisible by py.\n");
        MPI_Abort(MPI_COMM_WORLD, 3);   /* abort py needs to divide n */
    }

    (*n_ptr) = n;
    (*energy_ptr) = energy;
    (*niters_ptr) = niters;
    (*px_ptr) = px;
    (*py_ptr) = py;
}

void init_sources(int bx, int by, int offx, int offy, int n,
                  const int nsources, int sources[][2], int *locnsources_ptr, int locsources[][2])
{
    int i, locnsources = 0;

    sources[0][0] = n / 2;
    sources[0][1] = n / 2;
    sources[1][0] = n / 3;
    sources[1][1] = n / 3;
    sources[2][0] = n * 4 / 5;
    sources[2][1] = n * 8 / 9;

    for (i = 0; i < nsources; ++i) {    /* determine which sources are in my patch */
        int locx = sources[i][0] - offx;
        int locy = sources[i][1] - offy;
        if (locx >= 0 && locx < bx && locy >= 0 && locy < by) {
            locsources[locnsources][0] = locx + 1;      /* offset by halo zone */
            locsources[locnsources][1] = locy + 1;      /* offset by halo zone */
            locnsources++;
        }
    }

    (*locnsources_ptr) = locnsources;
}

void alloc_bufs(int bx, int by, double **aold_ptr, double **anew_ptr,
                double **sbufnorth_ptr, double **sbufsouth_ptr,
                double **sbufeast_ptr, double **sbufwest_ptr,
                double **rbufnorth_ptr, double **rbufsouth_ptr,
                double **rbufeast_ptr, double **rbufwest_ptr)
{
    double *aold, *anew;
    double *sbufnorth, *sbufsouth, *sbufeast, *sbufwest;
    double *rbufnorth, *rbufsouth, *rbufeast, *rbufwest;

    /* allocate two working arrays */
    anew = (double *) malloc((bx + 2) * (by + 2) * sizeof(double));     /* 1-wide halo zones! */
    aold = (double *) malloc((bx + 2) * (by + 2) * sizeof(double));     /* 1-wide halo zones! */

    memset(aold, 0, (bx + 2) * (by + 2) * sizeof(double));
    memset(anew, 0, (bx + 2) * (by + 2) * sizeof(double));

    /* allocate communication buffers */
    sbufnorth = (double *) malloc(bx * sizeof(double)); /* send buffers */
    sbufsouth = (double *) malloc(bx * sizeof(double));
    sbufeast = (double *) malloc(by * sizeof(double));
    sbufwest = (double *) malloc(by * sizeof(double));
    rbufnorth = (double *) malloc(bx * sizeof(double)); /* receive buffers */
    rbufsouth = (double *) malloc(bx * sizeof(double));
    rbufeast = (double *) malloc(by * sizeof(double));
    rbufwest = (double *) malloc(by * sizeof(double));

    memset(sbufnorth, 0, bx * sizeof(double));
    memset(sbufsouth, 0, bx * sizeof(double));
    memset(sbufeast, 0, by * sizeof(double));
    memset(sbufwest, 0, by * sizeof(double));
    memset(rbufnorth, 0, bx * sizeof(double));
    memset(rbufsouth, 0, bx * sizeof(double));
    memset(rbufeast, 0, by * sizeof(double));
    memset(rbufwest, 0, by * sizeof(double));

    (*aold_ptr) = aold;
    (*anew_ptr) = anew;
    (*sbufnorth_ptr) = sbufnorth;
    (*sbufsouth_ptr) = sbufsouth;
    (*sbufeast_ptr) = sbufeast;
    (*sbufwest_ptr) = sbufwest;
    (*rbufnorth_ptr) = rbufnorth;
    (*rbufsouth_ptr) = rbufsouth;
    (*rbufeast_ptr) = rbufeast;
    (*rbufwest_ptr) = rbufwest;
}

void free_bufs(double *aold, double *anew,
               double *sbufnorth, double *sbufsouth,
               double *sbufeast, double *sbufwest,
               double *rbufnorth, double *rbufsouth, double *rbufeast, double *rbufwest)
{
    free(aold);
    free(anew);
    free(sbufnorth);
    free(sbufsouth);
    free(sbufeast);
    free(sbufwest);
    free(rbufnorth);
    free(rbufsouth);
    free(rbufeast);
    free(rbufwest);
}

void pack_data(int bx, int by, double *aold,
               double *sbufnorth, double *sbufsouth, double *sbufeast, double *sbufwest)
{
    int i;
    for (i = 0; i < bx; ++i)
        sbufnorth[i] = aold[ind(i + 1, 1)];     /* #1 row */
    for (i = 0; i < bx; ++i)
        sbufsouth[i] = aold[ind(i + 1, by)];    /* #(by) row */
    for (i = 0; i < by; ++i)
        sbufeast[i] = aold[ind(bx, i + 1)];     /* #(bx) col */
    for (i = 0; i < by; ++i)
        sbufwest[i] = aold[ind(1, i + 1)];      /* #1 col */
}

void unpack_data(int bx, int by, double *aold,
                 double *rbufnorth, double *rbufsouth, double *rbufeast, double *rbufwest)
{
    int i;
    for (i = 0; i < bx; ++i)
        aold[ind(i + 1, 0)] = rbufnorth[i];     /* #0 row */
    for (i = 0; i < bx; ++i)
        aold[ind(i + 1, by + 1)] = rbufsouth[i];/* #(by+1) row */
    for (i = 0; i < by; ++i)
        aold[ind(bx + 1, i + 1)] = rbufeast[i]; /* #(bx+1) col */
    for (i = 0; i < by; ++i)
        aold[ind(0, i + 1)] = rbufwest[i];      /* #0 col */
}

void update_grid(int bx, int by, double *aold, double *anew, double *heat_ptr)
{
    int i, j;
    double heat = 0.0;

    for (i = 1; i < bx + 1; ++i) {
        for (j = 1; j < by + 1; ++j) {
            anew[ind(i, j)] =
                anew[ind(i, j)] / 2.0 + (aold[ind(i - 1, j)] + aold[ind(i + 1, j)] +
                                         aold[ind(i, j - 1)] + aold[ind(i, j + 1)]) / 4.0 / 2.0 ;
            heat += anew[ind(i, j)];
        }
    }

    (*heat_ptr) = heat;
}

