//program by Vijay manoharan SUNYB


//header files

#include <stdio.h>
#include <stdlib.h>
#include<math.h>
#include <stdbool.h>
#include "mpi.h"
#include<sys/time.h>
//end of header file


//cuda global function
int *cuda_main(int , int ,int ) ;



//main function

int main(int argc, char *argv[]) {
    int p_id, Num_Processor;
    int len, number_of_prime = atoi(argv[1]);
    char procname[MPI_MAX_PROCESSOR_NAME];
    int number;
	int *countNmax;
	    //variables for MPI reduction
    int maxi = 0, overall_max = 0, count = 0, total_count = 0;
    //variable for time stamp
    struct timeval tv1, tv2;
    //variables for checking the a number is prime or not
    int prime_div, i;
    //end of declaration
    //mpi initialization
    MPI_Init(&argc, &argv);
    //getting the number of processors
    MPI_Comm_size(MPI_COMM_WORLD, &Num_Processor);
    // assigning process id's
    MPI_Comm_rank(MPI_COMM_WORLD, &p_id);
    //getting the processor name
    MPI_Get_processor_name(procname, &len);
    //starting timer here
    gettimeofday(&tv1, NULL);
	//MPI declaration ends here
	//Cuda declaration starts here
    	
    //to broadcast he problem to other nodes from the root node
    //MPI_Bcast(<shared variable>,<count>,<mpi_datatype>,<root's rank>,<communicator>)
    MPI_Bcast(&number_of_prime, 1, MPI_LONG, 0, MPI_COMM_WORLD);

	countNmax=cuda_main(p_id,Num_Processor,number_of_prime);
	    
	printf("\nPrime number from process %d on node  %s\t \n", p_id, procname);
    printf("Local count = %d in %d and max value is %d \n",countNmax[0],p_id,countNmax[1]);
	
    MPI_Reduce(&countNmax[0], &total_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&countNmax[1], &overall_max, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

    MPI_Finalize();
    // printf("Prime number from process %d on node  %s\n", p_id, procname);
	gettimeofday(&tv2, NULL);

    if (p_id == 0) {
        total_count+=9;
		printf("number of processor :%d \n", Num_Processor);
        printf("Global count = %d \t Global max value = %d\n", total_count, overall_max);
        printf("Total time = %f seconds\n", (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 + (double) (tv2.tv_sec - tv1.tv_sec));
    }
    return 0;
}
