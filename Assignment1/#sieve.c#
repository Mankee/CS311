/* main.c: Marks and lists all prime number of a given n
 * Author: Austin Dubina
 * Date Started: 09/26/12
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct array *create_array(int x);
void init_array(struct array *v, int capacity);
void print_array(struct array *v);
void mark_array(struct array *v);
void clean_up(struct array *v);

struct array
	{
		int *data;				/* pointer of my array of marked prime numbers */
		int num_of_primes;			/* tally of marked prime numbers */
		int capacity;				/* total size of my fixed array */
	};

int main(int argc, char* argv[])
{
	int n = atoi(argv[1]);			/* convert argument into integer value */

	if(n <= 0){					/* quick check to see if the input is not something silly like a word or negitive value */
		printf("invalid input!\n");			
	}
	else{
		printf("you have entered a size of %d, performing calculations...\n",n);
		struct array *fixed_array = create_array(n);
		mark_array(fixed_array);
		print_array(fixed_array);
		clean_up(fixed_array);		
	}
	return 0;
}

struct array *create_array(int x)
{
	struct array *a = malloc(sizeof(struct array));
	init_array(a, x);
	return a;
}

void init_array(struct array *v, int capacity)
{
	v->data = malloc(sizeof(int) * capacity);
	v->capacity = capacity;
	v->num_of_primes = 0;
}

void print_array(struct array *v)
{
	int i;
	printf("Prime Numbers = [");
	for(i = 0; i < v->capacity; i++){
		if(v->data[i] == 0){
			printf(" %d",i+1);
			v->num_of_primes++;
		}
	}
	printf(" ]\n");
	printf("Number of primes = %d\n", v->num_of_primes);
}

void clean_up(struct array *v){
	if(v->data != 0){
		free(v->data); 	/* free the space on the heap */
		v->data = NULL;   	/* make it point to null */
	}
	free(v);
}

void mark_array(struct array *v){
	int j = 0;
	int k;	
	v->data[0] = 1;

	if(v->data[j]){
		for(j = 2; j <= sqrt(v->capacity); j++){
			for(k = 2 * j; k <= v->capacity; k += j){
				v->data[k-1] = 1;
			}
		}
	}
}
