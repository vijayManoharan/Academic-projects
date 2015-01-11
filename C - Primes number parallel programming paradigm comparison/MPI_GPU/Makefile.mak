


MPI_CUDA_VecAdd: prime_mpi.o prime_cuda.o
	mpiicc prime_mpi.o prime_cuda.o -L /util/cuda/4.2.9/cuda/lib64/ -I /util/cuda/4.2.9/cuda/include/ -lcudart -o MPI_CUDA_prime
prime_mpi.o: prime_mpi.c
	mpiicc -c prime_mpi.c -o prime_mpi.o
prime_cuda.o: prime_cuda.cu
	nvcc -c -arch compute_20 prime_cuda.cu -o prime_cuda.o

.phony: clean
clean:
	rm -rvf MPI_CUDA_VecAdd prime_mpi.o prime_cuda.o
