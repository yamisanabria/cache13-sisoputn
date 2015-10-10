#include "shared.h"
#include <semaphore.h>
#include <pthread.h>

t_queue* ioQueue;
t_dictionary* sleepTimes; //Diccionario que tiene como clave el PID y como data el tiempo a dormir
pthread_mutex_t mutex_ioSleep;
sem_t* sem_ioSleep;

pthread_t* ioThread;

void ioQueueRunner(){
	while(1){
		sem_wait(&sem_ioSleep);
		pthread_mutex_lock(&mutex_ioSleep);

		PCBItem* process = queue_pop(ioQueue);

		sprintf(log_buffer, "Hacemos sleep del proceso PID-%d.\n", process->PID);
		log_info(logger, log_buffer);

		int sleep_time = dictionary_remove(sleepTimes, process->PID);
		sleep(sleep_time);

		sprintf(log_buffer, "Despertó el proceso PID-%d.\n", process->PID);
		log_info(logger, log_buffer);

		process->status = P_READY;

		pQueueAddProcess();
		checkReadyProcesses();

		pthread_mutex_unlock(&mutex_ioSleep);
	}
}

void createIOQueue(){

	pthread_create(ioThread, NULL, (void *) ioQueueRunner);

	ioQueue = queue_create();
	sleepTimes = dictionary_create();
	sem_init(sem_ioSleep, 0, 0);
}

void ioQueueAddProcess(PCBItem* process, int sleep_time){

	queue_push(ioQueue, process);
	dictionary_put(sleepTimes, process->PID, sleep_time);
	sem_post(&sem_ioSleep);
}
