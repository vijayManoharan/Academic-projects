all:OpenMP_MPI.c
	mpiicc -std=c99 -openmp OpenMP_MPI.c -o OpenMP_MPI
