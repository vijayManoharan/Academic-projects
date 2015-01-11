//program by Vijay manoharan SUNYB


//header files

#include <stdio.h>
#include <stdlib.h>
#include<math.h>
#include <stdbool.h>

#include<sys/time.h>
//end of header file


//cuda global function

__global__ void cudeprime(int limit,int *threadcounter, int *maxi,int pid,int Num_processor) {
    int  number, prime_div;
	number = 29+(blockIdx.x*2*blockDim.x)+(2*threadIdx.x*Num_processor)+(2*pid);
	//printf("number %d and process %d\n",number,pid);
	//printf("block -%d\t size -%d\tthread- %d\t the number is %d\n",blockIdx.x,blockDim.x,threadIdx.x,number);
	if(number>limit) return;
    if ((number % 3 == 0) || (number % 5 == 0) || (number % 7 == 0) || (number % 13 == 0) || (number % 11 == 0) || (number % 17 == 0) || (number % 19 == 0) || (number % 23 == 0)) {
    //do nothing 
    } else {
	prime_div = 29;
		while (prime_div * prime_div <= number) {

			if ((number % prime_div == 0))
				break;

			prime_div = prime_div + 2;
			
		}
		//if the number is not prime_divisible until the square root of the number then the number is prime.
		if (prime_div * prime_div > number) {
			//write the value to index.
			threadcounter[(blockIdx.x*blockDim.x)+threadIdx.x]=number;
			//printf("thread n number %d %d\n",threadIdx.x,number);
		}
	}
}


//main function

extern "C" int *cuda_main(int p_id, int Num_Processor,int number_of_prime) {
    printf("in cuda n %d pid %d  number of process %d \n",number_of_prime,p_id,Num_Processor);
	static int returnval[2];
    //variables for MPI reduction
    int maxi = 0, count = 0;
    //variable for time stamp
    struct timeval tv1, tv2;
    //variables for checking the a number is prime or not
    
    //end of declaration
    //starting timer here
    gettimeofday(&tv1, NULL);
	//MPI declaration ends here
	//Cuda declaration starts here
    int threadSize=10;
	if(number_of_prime<(2000*Num_Processor))
		threadSize=number_of_prime/(2*Num_Processor);
	else
		threadSize=1000;
	int	blocksize = (number_of_prime / (2*threadSize*Num_Processor));
	int totalthreads=blocksize*threadSize*Num_Processor;
	int arraycount[totalthreads];
	int *dev_count, *dev_max;
			for(int i=0;i<totalthreads;i++)
				arraycount[i]=0;
	cudaMalloc((void **) &dev_count, totalthreads* sizeof(int));
    cudaMalloc((void **) &dev_max, sizeof (int));
	//end of cuda declaration

    if (number_of_prime > 10) {
	    cudaMemcpy(dev_max, &maxi, sizeof (int), cudaMemcpyHostToDevice);
		cudaMemcpy(dev_count, arraycount, totalthreads*sizeof (int), cudaMemcpyHostToDevice);
        printf("number of blocks and threads %d / %d\n", blocksize, threadSize);
        cudeprime <<<blocksize, threadSize >>>(number_of_prime,dev_count, dev_max,p_id,Num_Processor);
        cudaMemcpy(&maxi, dev_max, sizeof (int), cudaMemcpyDeviceToHost);
		cudaMemcpy( arraycount,dev_count, totalthreads*sizeof (int), cudaMemcpyDeviceToHost);
		gettimeofday(&tv2, NULL);
		for(int i=0;i<totalthreads;i++)
		{
			if(arraycount[i]>0)
			{
				count++;
				if(maxi<arraycount[i])maxi=arraycount[i];
				//printf("%d is n and index %d\n",arraycount[i],i);
			}
		}
		
    } else {
        count = 4;
        maxi = 7;
    }
	
    //to broadcast he problem to other nodes from the root node

	
    
	
    printf("Local count = %d \t Local max value = %d\ncuda exit\n", count, maxi);
	
    cudaFree(dev_max);
    cudaFree(dev_count);

    
	returnval[0]=count;
	returnval[1]=maxi;
    return returnval;
}
