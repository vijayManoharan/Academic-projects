
MPI_CUDA_VecAdd: BucketSort_mpi.o BucketSort_cuda.o
	mpiicc BucketSort_mpi.o BucketSort_cuda.o -L /util/cuda/4.2.9/cuda/lib64/ -I /util/cuda/4.2.9/cuda/include/ -lcudart -o MPI_CUDA_BucketSort
BucketSort_mpi.o: BucketSort_mpi.c
	mpiicc -c BucketSort_mpi.c -o BucketSort_mpi.o
BucketSort_cuda.o: BucketSort_cuda.cu
	nvcc -c -arch compute_20 BucketSort_cuda.cu -o BucketSort_cuda.o

.phony: clean
clean:
	rm -rvf MPI_CUDA_VecAdd BucketSort_mpi.o BucketSort_cuda.o
