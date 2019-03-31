#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <vector>

typedef struct
{
	int first;
	int last;
	long result;
} tagArgs;

int arr[5000];
long s0 = 0;
long s1 = 0;
long s2 = 0;

void *sumfunc(void *args)
{
	int i;
	tagArgs *myargs = (tagArgs *)args;
	for (i = myargs->first; i < myargs->last; i++)
	{
		s0 += arr[i];
	}
	return NULL;
}

void multiSum()
{
	int i;
	for (i = 0; i < 5000; i++)
	{
		arr[i] = rand() % 50;
	}

	pthread_t th1, th2;
	tagArgs args1 = {0, 2500, 0}, args2 = {2500, 5000, 0};
	pthread_create(&th1, NULL, sumfunc, (void *)&args1);
	pthread_create(&th2, NULL, sumfunc, (void *)&args2);

	pthread_join(th1, NULL);
	pthread_join(th2, NULL);

	// printf("s1 %d \n", args1.result);
	// printf("s2 %d \n", args2.result);
	// printf("s1 + s2 %d \n", args1.result + args2.result);
	printf("s1 + s2 %d \n", s0);
	return;
}

bool test_and_set(bool *lock)
{
	bool rv = *lock;
	*lock = true;
	return rv;
}

long s = 0;
pthread_mutex_t lock;

void *falseSharing(void *args)
{
	pthread_mutex_lock(&lock);
	for (int i = 0; i < 1000000; i++)
	{
		// pthread_mutex_lock(&lock);
		s++;
		// pthread_mutex_unlock(&lock);
	}
	pthread_mutex_unlock(&lock);
	return NULL;
}

void multi_lock()
{
	pthread_t th1, th2;
	pthread_mutex_init(&lock, NULL);

	pthread_create(&th1, NULL, falseSharing, NULL);
	pthread_create(&th2, NULL, falseSharing, NULL);

	pthread_join(th1, NULL);
	pthread_join(th2, NULL);

	printf("%d\n", s);
	return;
}
