#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include <pthread.h>
#define DATA_QUANTITY 100000000

//Данные и график во вложенных скринах.
//Зависимость получилась, как и ожидалось - что-то типо корневой

struct Args
{
	int* array;
	int nums_quant;
	float* sum;
	float* deviation;
	float average;
};

void* calc_Deviations(void* arguments)
{
	struct Args th_args = *((struct Args*)arguments);
	int sum = 0;
	int i;
	float deviations = 0;
	for(i = 0; i < th_args.nums_quant; i++)
	{
		deviations = deviations + 
			(th_args.array[i] - th_args.average) * 
			(th_args.array[i] - th_args.average);
	}
	*(th_args.deviation) = deviations;
}


void* calc_Sums(void* arguments)
{
	struct Args th_args = *((struct Args*)arguments);
	float sum = 0;
	int i;
	for(i = 0; i < th_args.nums_quant; i++)
	{
		sum = sum + th_args.array[i];
	}
	*(th_args.sum) = sum;
}

int main()
{
	int* array = malloc(DATA_QUANTITY*sizeof(int));
	int i;
	int th_quant;
	int scanf_result;
	printf("Enter thread's quantity:\n");
	scanf_result = scanf("%d", &th_quant);
	for(i = 0; i < DATA_QUANTITY; i++)
	{
		array[i] = 1 + rand() %100000;
	}
	double average, dispersion;
	float all_sum = 0, all_deviation = 0;
	float* th_deviations = (float*)malloc(th_quant * sizeof(float));
	float* th_sums = (float*)malloc(th_quant * sizeof(float));

	pthread_t* th_ids = (pthread_t*)malloc(th_quant * sizeof(pthread_t));
	int result;
	struct Args* args = (struct Args*)malloc(th_quant * sizeof(struct Args));
	args[0].array = array;
	args[0].nums_quant = DATA_QUANTITY / th_quant;
	args[0].deviation = &th_deviations[0];
	args[0].sum = &th_sums[0];
	for(i = 1; i < th_quant; i++)
	{
		args[i].array = args[i - 1].array + (DATA_QUANTITY / th_quant);
		args[i].nums_quant = DATA_QUANTITY / th_quant;
		args[i].deviation = &th_deviations[i];
		args[i].sum = &th_sums[i];
	}
	clock_t begin = clock();
	for(i = 0; i < th_quant; i++)
	{
		result = pthread_create(&th_ids[i], NULL, calc_Sums, &args[i]);
	}
	for(i = 0; i < th_quant; i++)
	{
		pthread_join(th_ids[i], (void**)NULL);
	}
	for(i = 0; i < th_quant; i++)
	{
		all_sum = all_sum + th_sums[i];
	}
	average = all_sum / (float)DATA_QUANTITY;
	for(i = 0; i < th_quant; i++)
	{
		args[i].average = average;
	}

	for(i = 0; i < th_quant; i++)
	{
		result = pthread_create(&th_ids[i], NULL,
			       	calc_Deviations, &args[i]);
	}
	for(i = 0; i < th_quant; i++)
	{
		pthread_join(th_ids[i], (void**)NULL);
	}
	for(i = 0; i < th_quant; i++)
	{
		all_deviation = all_deviation + th_deviations[i];
	}
	dispersion = all_deviation / (float)(DATA_QUANTITY - 1);

	clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

	printf("Average: %f\nDispersion: %f\nTime_spent:%f\n",
		       	average, dispersion, time_spent);

	return 0;
}
