// -----------------------------------------------------
// Операционные системы - лабораторная №1
// Проблема читателей и писателей
// Мютексы (UNIX)
// Юрченко Владимир, ДА-82
// -----------------------------------------------------

#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <errno.h>

#define readerCOUNT 5
#define writerCOUNT 5
#define maxWaitTime 500

pthread_t readerThread[readerCOUNT];
pthread_t writerThread[writerCOUNT];
bool the_end = false;

int main()
{
	srand(1);
	
	// Создаём читателей и писателей
	for (int i = 0; i < readerCOUNT; i++)
	{
		pthread_create (readerThread[i], NULL, &Reader, NULL);
	}
	for (int i = 0; i < writerCOUNT; i++)
	{
		pthread_create (writerThread[i], NULL, &Writer, NULL);
	}

	for (int i = 0; i < 4; i++)
	{
		sleep(rand() % maxWaitTime);
	}

	the_end = true;
		
	for (int i = 0; i < readerCOUNT; i++)
	{
		pthread_join(readerThread[i]);
	}
	for (int i = 0; i < writerCOUNT; i++)
	{
		pthread_join(writerThread[i]);
	}
	return 0;
}


pthread_mutex_t work_reader = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t work_writer = PTHREAD_MUTEX_INITIALIZER; // (initial value = 1)
pthread_mutex_t write_enabled = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t read_enabled = PTHREAD_MUTEX_INITIALIZER;
int writercount = 0;
int readercount = 0;

// WRITER
void Writer()
{
    //pthread_mutex_unlock(&work_writer);
	while (!the_end)
	{
		printf("The Writer has come\n");
		// Приходит писатель
		if (pthread_mutex_lock(&work_writer))			// Ждёт, пока остальные писатели завершат входить или выходить
		{
			printf("work_writer locked (first)\n");	
			writercount = writercount + 1;				// Добавляем писателя в очередь
			printf("Write++ = %d \n",writercount);			

			if (pthread_mutex_lock(&read_enabled))		// Закрываем вход читателям
			{
				printf("mutexRead locked\n");
			}
			pthread_mutex_unlock(&work_writer);			// Оставляем возможность прийти другим писателям
		}						
			
		if (pthread_mutex_lock(&write_enabled))			// Ждём, пока всё читатели не уберутся  
		{												// восвояси
			// writing is performed						   и запрещаем писать остальным писателям
			printf("-- writing is performed --\n");		// Тут писатель что-то пишет			
			sleep(rand() % maxWaitTime);				// в течение некоторого времени
			pthread_mutex_unlock(&write_enabled);		// Открываем возможность писать остальным писателям
		}

		// Писатель уходит
		if (pthread_mutex_lock(&work_writer))			
		{
			writercount = writercount - 1;
			printf("Write-- = %d \n",writercount);
			if (writercount == 0)
			{ 
				if (pthread_mutex_unlock(&read_enabled)) // Разрешаем приходить читателям
				{
					printf("mutexRead unlocked\n");
				}
			}
			printf("work_writer unlocked\n");
			pthread_mutex_unlock(&work_writer);
		}		
		printf("The Writer has gone away\n");
		sleep(rand() % maxWaitTime);		
	}	
}

// READER
void Reader()
{
	while (!the_end)
	{
		printf("The Reader has come\n");
		
		if (pthread_mutex_lock(&read_enabled))		// Если писателей нет, то разрешение читать есть
		{
			pthread_mutex_unlock(&read_enabled);	// Проверил, что разрешение есть, другие тоже хотят проверить
			if (pthread_mutex_lock(&work_reader))	// Ждёт, пока остальные читатели завершат входить или выходить
			{
				readercount = readercount + 1;			// Тогда добавляем читателя в очередь

				pthread_mutex_lock(&write_enabled);	// Есть читатель - писатели писать не могут	
				pthread_mutex_unlock(&work_reader);
			}
			
			
		}
 
		//if (pthread_mutex_trylock(&read_enabled))	// Пытается что-то прочитать
		if (pthread_mutex_lock(&read_enabled))      // Не уйдёт, пока что-то не прочитает
		{
			// reading is done
			printf("-- reading is done --\n");		// Тут читатель читает
			sleep(rand() % maxWaitTime);			// в течение некоторого времени
			pthread_mutex_unlock(&read_enabled);
		}
 
		// Читатель уходит
		if (pthread_mutex_lock(&work_reader))
		{
			readercount = readercount - 1;
			if (readercount == 0)
			{
				pthread_mutex_unlock(&write_enabled);	// Все читатели свалили, писатель может писать
			}
			pthread_mutex_unlock(&work_reader);
		}
		printf("The Reader has gone away\n");
		sleep(rand() % maxWaitTime);
	}	
}			pthread_mutex_unlock(&read_enabled);
		}
 
		// Читатель уходит
		//if (pthread_mutex_lock(&work_reader))
		pthread_mutex_lock(&work_reader);
		{
			readercount = readercount - 1;
			if (readercount == 0)
			{
				pthread_mutex_unlock(&write_enabled);	// Все читатели свалили, писатель может писать
			}
			pthread_mutex_unlock(&work_reader);
		}
		printf("The Reader has gone away\n");
		sleep(rand() % maxWaitTime);
	}	
}
