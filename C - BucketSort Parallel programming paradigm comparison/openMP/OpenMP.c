#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>
#include <stdint.h>
#include<omp.h>
//other c and h files
#include "rnd.h"
#include "rnd.c"

int size;

/*
 * Using quickselect to partially sort the array
 * algorithm refered from wikipedia "http://en.wikipedia.org/wiki/Quickselect"
 * Implementation is done by me. 
 */

int partition(float a[size], int left, int right, int pivot) {
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
float quickSelect(float a[size], int left, int right, int pivot) {

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

void merge(float *a, int low, int mid, int high) {
    //printf("inside merge\n");
    int i = 0;
    int left = low, right = mid + 1;
    int j;
    float temp[high - low + 1 ];
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

void Mergesort(float *a, int low, int high) {
    int mid;
    if (low < high) {
        mid = (low + high) / 2;
        Mergesort(a, low, mid);
        Mergesort(a, mid + 1, high);
        merge(a, low, mid, high);
    }
}

void sort(float *a, int n, int m) {
    float temp;
    int i, j;

    for (i = n; i < m - 1; i++) {
        for (j = i; j < m; j++)
            if (a[i] < a[j]) {
                temp = a[i];
                a[i] = a[j];
                a[j] = temp;
            }
    }
}

int isnumber(int size) {
    int flag = 0;

    return flag;
}

int main(int argc, char** argv) {
    if (strcmp("-t", argv[1]) != 0) {
        printf("invalid command line agrument\n");
        return 0;
    }
    int parallel_scaling = 16;
    size = atoi(argv[2]);
    int pivots[parallel_scaling + 1];
    float pivots_value[parallel_scaling + 1];
    int subarray_size;
    float subarray[parallel_scaling][size + 1];
    struct timeval tv1, tv2;
    float array[size];
    printf("the size is %d", size);
    int i, j, k, nth;
    random_number_generator_normal(array, size, size);
    //    for (i = 0; i < size; i++) {
    //        printf("%d-->> %f\t", i, array[i]);
    //    }
    //find 25th element

    gettimeofday(&tv1, NULL);

    //starting parallelism here
#pragma omp parallel private(i), shared(array,pivots) 
    {
        nth = omp_get_num_threads();
        if (omp_get_thread_num() == 0)
            printf("The number of threads are %d \n", nth);
        parallel_scaling = nth;
        subarray_size = size / parallel_scaling;
        for (i = 0; i < parallel_scaling; i++) {
            pivots[i] = subarray_size * (i);
        }

        pivots[i] = size;
        pivots_value[0] = 0;
        pivots_value[i] = array[size - 1];
#pragma omp for 
        for (i = 1; i < parallel_scaling; i++) {
            pivots_value[i] = quickSelect(array, 0, size - 1, (pivots[i]));
            printf("is the %d largest value %f\n", pivots[i], pivots_value[i]);
            //find 5th largest element

        }
        printf("\nmergesort starts \n");

        //i must parallelize here.
#pragma omp for 
        for (i = 0; i < parallel_scaling; i++) {
            printf("till pivot %d to %d\n", pivots[i], pivots[i + 1] - 1);
            Mergesort(array, pivots[i], pivots[i + 1] - 1);
            //sort(array,pivots[i],pivots[i+1]);
        }
    }
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


