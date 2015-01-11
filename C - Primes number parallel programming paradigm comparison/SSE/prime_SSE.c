


/* 
 * File:   main.c
 * Author: vijay
 *
 * Created on September 29, 2014, 7:15 PM
 */

//header files
#include <stdio.h>
#include <stdlib.h>
#include<math.h>
#include <stdbool.h>
#include <sys/time.h>
#include <xmmintrin.h>  //x86 SSE1
#include <emmintrin.h>  //x86 SSE2
#include <pmmintrin.h>  //x86 SSE3
#include <smmintrin.h>  //x86 SSE4.1
#include <nmmintrin.h>  //x86 SSE4.2

/*sequential program to find prime numbers upto a given value
 * 
 */



int main(int argc, char** argv) {

    struct timeval tv1, tv2;
    //m128 vector array division
    __m128 Vec_rem, Vec_i, Vec_div, Vec_prime_div;
    __m128d Vecd_rem, Vecd_i, Vecd_div, Vecd_prime_div;
    int test_number, i, max, k, flag, prime_div, count;
    float rem[5];
    double remd[5];
    gettimeofday(&tv1, NULL);
    int number_of_prime;

    number_of_prime = atoi(argv[1]);
    printf("prime numbers between 1 and %d\n", number_of_prime);
    if (number_of_prime > 10) {
        count = 9;
    } else {
        count = 4;
        max = 7;
    }
    test_number = 3;
    for (i = 29; i <= number_of_prime; i = i + 2) {
        flag = 1;
        if ((i % 3 == 0) || (i % 5 == 0) || (i % 7 == 0) || (i % 13 == 0) || (i % 11 == 0) || (i % 17 == 0) || (i % 19 == 0) || (i % 23 == 0)) {
            //do nothing 
        } else {
            prime_div = 29;
            if (i > 16777216) //2^24 
            {
                while ((prime_div + 4) *(prime_div + 4) <= i) {
                    //load i and j
                    Vecd_i = _mm_set_pd(i, i);
                    Vecd_prime_div = _mm_set_pd(prime_div, prime_div + 2); //alternate way, pointer arithmetic
                    Vecd_rem = _mm_sub_pd(Vecd_i, (_mm_mul_pd(Vecd_prime_div, _mm_floor_pd(_mm_div_pd(Vecd_i, Vecd_prime_div)))));
                    //if any rem value is zero then set flag=0
                    _mm_store_pd(remd, Vecd_rem);
                    if ((remd[0] == 0) || (remd[1] == 0)) {
                        flag = 0; //not a prime
                        break;
                    }
                    prime_div = prime_div + 4;
                }
                while ((flag == 1) &&(prime_div * prime_div <= i)) {
                    if ((i % prime_div == 0))
                        flag = 0; //not a prime
                    prime_div = prime_div + 2;
                }
                //if the number is not prime_divisible until the square root of the number then the number is prime.

                if ((flag == 1)) {
                    //		printf("%d\t",i); prime number
                    max = i;
                    count++;
                }



            } else {
                while ((prime_div + 8) *(prime_div + 8) <= i) {
                    //load i and j
                    Vec_i = _mm_set_ps(i, i, i, i);
                    Vec_prime_div = _mm_set_ps(prime_div, prime_div + 2, prime_div + 4, prime_div + 6); //alternate way, pointer arithmetic
                    Vec_rem = _mm_sub_ps(Vec_i, (_mm_mul_ps(Vec_prime_div, _mm_floor_ps(_mm_div_ps(Vec_i, Vec_prime_div)))));
                    //if any rem value is zero then set flag=0
                    _mm_store_ps(rem, Vec_rem);
                    if ((rem[0] == 0) || (rem[1] == 0) || (rem[2] == 0) || (rem[3] == 0)) {
                        flag = 0; //not a prime
                        break;
                    }
                    prime_div = prime_div + 8;
                }
                while ((flag == 1) &&(prime_div * prime_div <= i)) {
                    if ((i % prime_div == 0))
                        flag = 0; //not a prime
                    prime_div = prime_div + 2;
                }
                //if the number is not prime_divisible until the square root of the number then the number is prime.

                if ((flag == 1)) {
                    //printf("%d\t",i); prime number
                    max = i;
                    count++;
                }
            }
        }
    }
    printf("\n count is %d\n", count);
    printf("\n max is %d\n", max);
    gettimeofday(&tv2, NULL);

    printf("Total time = %f seconds\n",
            (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
            (double) (tv2.tv_sec - tv1.tv_sec));
    return 0;

}










