///* 
// * File:   BucketSort.c
// * Author: vijay manoharan
// *. C program for running bucket sort on CUDA.
// * Created on November 8, 2014, 8:19 PM
// */

#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>
#include <stdint.h>

//header file for random value generation.
#include "rnd.h"
#include "rnd.c"

//headed file for cuda
#include <cuda.h>
int size;

/*
 * Using quickselect to partially sort the array
 * algorithm refered from wikipedia "http://en.wikipedia.org/wiki/Quickselect"
 * Implementation is done by me. 
 */

int partition(float *a, int left, int right, int pivot) {
    float pivotValue = a[pivot], temp;
    int index = left;
    int i;

    //move pivot to rightmost of the array
    a[pivot] = a[right];
    a[right] = pivotValue;

    /* We start from left of the array and keep comparing the values.
     *  If we find any value greater than pivot we swap the pivot with that value.
     *  this is done so that the value on the left are always lesser than pivot
     */
    for (i = left; i < right; i++) {
        if (a[i] < pivotValue) {
            temp = a[i];
            a[i] = a[index];
            a[index] = temp;
            index++;
        }
    }
    /*move pivot back to its initial position
     */
    temp = a[index];
    a[index] = a[right];
    a[right] = temp;
    return index;

}

/*the main aspect of the function is 
 * find the pivot value which is pivot given and return the value
 * next thing is all the value in the left index of pivot index is smaller 
 * all the values in the right index is greater.
 */
float quickSelect(float *a, int left, int right, int pivot) {

    //base condition if list contains one element return that
    if (left == right)
        return a[left];

    /* select the pivot index between left and right
     * i am selecting the mid value
     */

    int pivotindex = (right + left) / 2;
    pivotindex = partition(a, left, right, pivotindex);
    //int size_leftarray = pivotposition - low + 1;
    //pivot in its correct position
    if (pivot == pivotindex)
        return a[pivotindex];
    else if (pivot < pivotindex)
        return quickSelect(a, left, pivotindex - 1, pivot);
    else
        return quickSelect(a, pivotindex + 1, right, pivot);
}

//merging the sorted left and right of the merge sort

__device__ void merge(float *a, int low, int mid, int high) {
    //printf("inside merge\n");
    int i = 0;
    int left = low, right = mid + 1;
    int j;
	float *temp =(float*) malloc((high - low + 1 )* sizeof(*temp) );
    
    /* Create a temp sorted list
     * get the min of left part and right part
     * if min is in the left part iterate left or else iterate right
     */

    while ((left <= mid)&&(right <= high)) {
        if (a[left] < a[right])
            temp[i++] = a[left++];
        else
            temp[i++] = a[right++];
    }
    //left part contains larger values
    while (left <= mid)
        temp[i++] = a[left++];
    //right part contains larger values
    while (right <= high)
        temp[i++] = a[right++];


    //copy the sorted values
    for (j = 0; j < i; j++) {
        a[low + j] = temp[j];
    }
}
//merge sort algorithm
void Quick(float *a, int low, int mid, int high) {
    //printf("inside merge\n");
    int i = 0;
    int left = low, right = mid + 1;
    int j;
	float *temp =(float*) malloc((high - low + 1 )* sizeof(*temp) );
    
    /* Create a temp sorted list
     * get the min of left part and right part
     * if min is in the left part iterate left or else iterate right
     */

    while ((left <= mid)&&(right <= high)) {
        if (a[left] < a[right])
            temp[i++] = a[left++];
        else
            temp[i++] = a[right++];
    }
    //left part contains larger values
    while (left <= mid)
        temp[i++] = a[left++];
    //right part contains larger values
    while (right <= high)
        temp[i++] = a[right++];


    //copy the sorted values
    for (j = 0; j < i; j++) {
        a[low + j] = temp[j];
    }
}

__device__ void Mergesort(float *a, int low, int high) {
    int mid;
    if (low < high) {
        mid = (low + high) / 2;
        Mergesort(a, low, mid);
        Mergesort(a, mid + 1, high);
        merge(a, low, mid, high);
    }
}

void Quickselect(float *a, int low, int high) {
    int mid;
    if (low < high) {
        mid = (low + high) / 2;
        Quickselect(a, low, mid);
        Quickselect(a, mid + 1, high);
        Quick(a, low, mid, high);
    }
}

int isnumber(int size) {
    int flag = 0;

    return flag;
}



__global__ void cudaBucketSort(float *array, int *pivots) {
    /* 1.call merge sort for each threads
     * 2.Range is given based on the pivot values.
     * 3.Each thread will sort its own data set.
     */
	if(threadIdx.x==0){ 
    int start = pivots[blockIdx.x],i;
    int end = pivots[blockIdx.x +1];
    printf("pivot index %d\t start %d \t end %d\n", (blockIdx.x ) , start, end);
    Mergesort(array, start, end);
	for(i=start;i<end;i++)
		 printf("%d-->> %f\n", i, array[i]);
	}
	
}

int main(int argc, char** argv) {

    if (strcmp("-t", argv[1]) != 0) {
        printf("invalid command line agrument\n");
        return 0;
    }


    size = atoi(argv[2]);
    
    //variables for size and number of threads, blocks etc.
    int BlockNum=10;
  
    int Range=size/10;
    
    int ThreadSize ;
    
	if(size<1000)
		ThreadSize=size;
	else
		ThreadSize=1000;
	
    //variables for pivots 
    int pivots[11];
    float pivots_value[11];
    struct timeval tv1, tv2;
    float array[size];
    printf("the size is %d", size);
    int i;
    
    //cuda device variables 

    float *dev_array;
	int	*dev_pivots;

  
    cudaMalloc((void **) &dev_array, size * sizeof (float));
    cudaMalloc((void **) &dev_pivots, (11) * sizeof (int));

    random_number_generator_normal(array, size, 10*size);
    //    for (i = 0; i < size; i++) {
    //        printf("%d-->> %f\t", i, array[i]);
    //    }
    //find 25th element

    gettimeofday(&tv1, NULL);

    for (i = 0; i < 10; i++) {
        pivots[i] = Range*i;
    }
    pivots[i] = size;
    pivots_value[0] = 0;
    pivots_value[i] = array[size - 1];
    for (i = 1; i < 10; i++) {
        pivots_value[i] = quickSelect(array, pivots[i - 1], size - 1, (pivots[i]));
		if(size>10000)
			 Quickselect(array, pivots[i-1], pivots[i]);
	   //printf("is the %d largest value %f\n", pivots[i], pivots_value[i]);
        //find 5th largest element

    }

    /* MergeSort here.
     * 
     */
    
    cudaMemcpy(dev_array, array, size * sizeof (float), cudaMemcpyHostToDevice);
    cudaMemcpy(dev_pivots, pivots, (11) * sizeof (int), cudaMemcpyHostToDevice);
    cudaBucketSort <<<BlockNum, ThreadSize>>>(dev_array, dev_pivots);
	if(size<=10000)
    cudaMemcpy(array, dev_array, size * sizeof (float), cudaMemcpyDeviceToHost);

	//cuda function here.
	
    printf("\nmergesort starts \n");




    gettimeofday(&tv2, NULL);


    //sort(array);
    printf("sorted list is \n");
    for (i = 0; i < size; i++) {
       printf("%d-->> %f\n", i, array[i]);
    }
	
    printf("\nTotal time = %f seconds\n",
            (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
            (double) (tv2.tv_sec - tv1.tv_sec));
    return 0;


}