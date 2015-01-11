all:OpenMP.c
	icc -std=c99 -openmp OpenMP.c -o OpenMP
