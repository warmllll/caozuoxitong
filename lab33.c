#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <unistd.h>
#include <assert.h>

char buf[10] = {0};

sem_t s1;	
sem_t s2;	

void *worker1(void *arg){
	for (int i = 0; i < 10;) {
	sem_wait(&s1);	
	scanf("%c",&buf[i]);
	i++;
	i %= 10;			
	sem_post(&s2);	
	}
	return NULL;
}

void *worker2(void *arg){
	for (int i = 0; i < 10;) {
	sem_wait(&s2);	
	printf("%c ",buf[i]);
	sleep(1);			
	i++;
	i %= 10;
	sem_post(&s1);	
	}
	return NULL;
}

int main(int argc, char *argv[]){	
	sem_init(&s1, 0, 10);
	sem_init(&s2, 0, 0);

	pthread_t p1, p2;
	pthread_create(&p1, NULL, worker1, NULL);	
	pthread_create(&p2, NULL, worker2, NULL);

	pthread_join(p1, NULL);
	pthread_join(p2, NULL);

	sem_destroy(&s1);
	sem_destroy(&s2);

	return 0;
}
