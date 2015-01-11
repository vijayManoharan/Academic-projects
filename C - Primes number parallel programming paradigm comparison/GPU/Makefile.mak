all:prime_cuda.c
	nvcc -arch compute_20 prime_cuda.cu -o prime_cuda
