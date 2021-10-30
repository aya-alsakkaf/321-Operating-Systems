/*
 * Aya Marouf Al-Sakkaf
 * 2181156174
 * Accesing and reading from the shared memory segment, and executing the algorithms through threads.
 */
#include "phase1.h"
#define FILENAME "newdata_100.txt"
#define OUTFILE "output.txt"
#define SIZEP 100
#define ALGO 6
pthread_mutex_t mutex;
extern int errno;
int jobs = 0; 

void *FCFST(void *args);
void *SJFNPT(void *args);
void *PNPT(void *args);
void *RRT(void *args);
void *SJFPT(void *args);
void *PPT(void *args);

void *FCFS(double *burstT, int count);
void *SJFNP(double *burst, int *proc, int count);
void *PNP(double *burst, int *proc, int count);
void *RR(double *burst, int *proc, int count);
void *SJFP(double *burst, int *proc, int count);
void *PP(double *burst, int *proc, int count);

void swapD(double *x, double *y)
{
	double temp = *x;
	*x = *y;
	*y = temp;
}
void swapI(int *x, int *y)
{
	int temp = *x;
	*x = *y;
	*y = temp;
}
long double getTime( )
{                       
struct timezone *tzp;
struct timeval  *tp;

  tp  = (struct timeval  *)malloc (sizeof(struct timeval));
  tzp = (struct timezone *)malloc (sizeof(struct timezone));
  gettimeofday(tp,tzp);
  return ( (tp->tv_sec) + (tp->tv_usec*1e-6) );
 }

typedef struct data{
	double burst[SIZEP];
	int procNum[SIZEP];
	int count;
} data;
int main(void)
{
	const char *name = "BurstTime";
	int shm_fd, SIZE;
	void *ptr;
	int i = 0, len = 0;
	char *token, *p;
	struct stat st;
	long double start, stop, elapse;

	start = getTime();
	stat(FILENAME, &st);
	SIZE = st.st_size;

	shm_fd = shm_open(name, O_RDONLY, 0666); //Open the shared memory segment
	if (shm_fd == -1)
	{
		printf("shared memory failed\n");
		exit(-1);
	}

	ptr = mmap(0, SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
	if (ptr == MAP_FAILED)
	{
		printf("Map failed\n");
		exit(-1);
	}

	p = (char *)malloc(strlen(ptr));

	if (shm_unlink(name) == -1)
	{ //Remove the shared memory segment
		printf("Error removing %s\n", name);
		exit(-1);
	}

	strcpy(p, ptr);
	token = strtok(p, "\n");
	while (token != NULL)
	{
		len++; //counting how many process there are
		token = strtok(NULL, "\n");
	}
	strcpy(p, ptr);

	double *burst = (double *)malloc(sizeof(double) * len);
	int *proc = (int *)malloc(sizeof(int) * len);

	token = strtok(p, "\n");
	while (token != NULL)
	{ //intializing the process and burst array to their respective data
		burst[i] = atof(token);
		proc[i] = i + 1;
		i++;
		token = strtok(NULL, "\n");
	}

	free(p);

#ifdef OUTPUT
	int fd = open(OUTFILE, O_WRONLY | O_CREAT, 0666);
	if (fd < 0)
	{
		printf("Error creating %s\n", OUTFILE);
		exit(1);
	}
	if (dup2(fd, 1) < 0)
	{ //redirecting the output to the file
		perror("Dup2 Error: ");
		exit(1);
	}
	close(fd);
#endif


#ifdef THREADS

printf("\n\n CPU Scheduling Algorithims Using Threads! \n\n");
//Creating a struct d to pass the arguments to the thread functions
	data *d = malloc(sizeof(data));
	memcpy(d->burst, burst, len*sizeof(double));
	memcpy(d->procNum, proc, len*sizeof(int));
	d->count = len;

//Creating 6 threads for the 6 available scheduling algorithim
	pthread_t tid[ALGO];
	if(pthread_create(&tid[0], NULL, FCFST, (void*)d ) < 0)
		fprintf(stderr, "Error creating the thread: %s\n", strerror(errno));

	if(pthread_create(&tid[1], NULL, SJFNPT, (void*)d ) < 0)
		fprintf(stderr, "Error creating the thread: %s\n", strerror(errno));

	if(pthread_create(&tid[2], NULL, PNPT, (void*)d ) < 0)
		fprintf(stderr, "Error creating the thread: %s\n", strerror(errno));
	
	if(pthread_create(&tid[3], NULL, RRT, (void*)d ) < 0)
		fprintf(stderr, "Error creating the thread: %s\n", strerror(errno));

	if(pthread_create(&tid[4], NULL, SJFPT, (void*)d ) < 0)
		fprintf(stderr, "Error creating the thread: %s\n", strerror(errno));
	
	if(pthread_create(&tid[5], NULL, PPT, (void*)d ) < 0)
		fprintf(stderr, "Error creating the thread: %s\n", strerror(errno));


	for(i = 0; i < ALGO; i++)
		pthread_join(tid[i], NULL);

	stop = getTime();
	printf("\n Number of Jobs Processed: %d\n", jobs);
	printf("\n *** Total time of execution: %Lfs *** \n", stop-start);
	free(proc);
	free(burst);
	free(d);
#endif


#ifdef SEQ
printf("\n\nSequential CPU Scheduling Algorithims! \n\n");

	FCFS(burst, len);
	printf("------------------------------------------------------------------------\n");
	PNP(burst, proc, len);
	printf("------------------------------------------------------------------------\n");
	SJFNP(burst, proc, len);
	printf("------------------------------------------------------------------------\n");
	RR(burst, proc, len);
	printf("------------------------------------------------------------------------\n");
	SJFP(burst, proc, len);
	printf("------------------------------------------------------------------------\n");
	PP(burst, proc, len);

	stop = getTime();

	printf("\n *** Total time of execution: %Lfs *** \n", stop-start);
	free(proc);
	free(burst);
	return 0;

#endif

return 0;
}

/* Implementation of First Come First Serve */
void *FCFST(void *args) {

	double *burstT = (((struct data*)args)->burst); //accessing the arguments from the passed struct
	int count = (((struct data*)args)->count);
	double waitT[count], TurnT[count], avgWait = 0, avgTurn = 0;
	int j = 0, i = 0;
	waitT[0] = 0;
	for (int i = 1; i < count; i++)
	{ //Calculating the wait time for each process
		waitT[i] = 0;
		for (j = 0; j < i; j++)
			waitT[i] += burstT[j];

	}

	for (i = 0; i < count; i++)
	{ //Calculating the turn around time for each process
		TurnT[i] = burstT[i] + waitT[i];
	}

	for (i = 0; i < count; i++)
	{ //Calculating the average for wait and turn around
		avgWait += waitT[i];
		avgTurn += TurnT[i];

		pthread_mutex_lock(&mutex);
		jobs++;
		printf("Job has been incremented to: %d by PID[%d]\n", jobs, getpid());
		pthread_mutex_unlock(&mutex);
	}

	//Printing the results
	pthread_mutex_lock(&mutex);
	printf("\n\nFirst Come First Server Scheduling Algorithm\n\n");
	printf("Process \t Burst Time \t Waiting Time \t Turnaround Time\n\n");
	for (i = 0; i < count; i++)
		printf("Process [%d] \t %.2lf \t\t %.2lf \t\t %.2lf\n", i + 1, burstT[i], waitT[i], TurnT[i]);

	printf("\n\nAverage Waiting Time = %lf\n", avgWait / count);
	printf("Avgerage Turnaroud Time = %lf\n\n\n", avgTurn / count);
	printf("------------------------------------------------------------------------\n");
	pthread_mutex_unlock(&mutex);

	return NULL;
}

/*Implementation Shortest Job First (Non-Preemptive) */
void *SJFNPT(void *args)
{
	double *burst = (((struct data*)args)->burst); //accessing the arguments from the passed struct
	int *proc = (((struct data*)args)->procNum);
	int count = (((struct data*)args)->count);
	int i = 0, j = 0, index;
	double waitT[count], TurnT[count], avgWait = 0, avgTurn = 0;
	//Copying the burst and process arrays into different arrays, so the original values don't get altered
	double burstC[count];
	memcpy(burstC, burst, count*sizeof(double));

	int procC[count];
	memcpy(procC, proc, count*sizeof(int));

	for (i = 0; i < count; i++)
	{ //Sorting the burst times to find the shortest process
		index = i;
		for (j = i + 1; j < count; j++)
		{
			if (burstC[j] < burstC[index]) //If the next burst is shorter than the previous one, switch it.
				index = j;
		}
		swapD(&burstC[i], &burstC[index]);
		swapI(&procC[i], &procC[index]);
		pthread_mutex_lock(&mutex);
		jobs++;
		pthread_mutex_unlock(&mutex);
	}

	waitT[0] = 0;
	for (int i = 1; i < count; i++)
	{ //Calulating the wait time for each process
		waitT[i] = 0;
		for (j = 0; j < i; j++)
			waitT[i] += burstC[j];
	}
	for (i = 0; i < count; i++)
	{ //Calculating the turn aroud time for each process
		TurnT[i] = burstC[i] + waitT[i];
	}

	for (i = 0; i < count; i++)
	{ //Calculating the average for wait and turn around time
		avgWait += waitT[i];
		avgTurn += TurnT[i];
	}

	//Printing the results
	pthread_mutex_lock(&mutex);
	printf("Shortest Job First (Non-Preemptive) Algorithm\n\n");
	printf("Process \t Burst Time \t Waiting Time \t Turnaround Time\n\n");
	for (i = 0; i < count; i++)
		printf("Process [%d] \t %.2lf \t\t %.2lf \t\t %.2lf\n", procC[i], burstC[i], waitT[i], TurnT[i]);

	printf("\n\nAverage Waiting Time = %lf\n", avgWait / count);
	printf("Avgerage Turnaroud Time = %lf\n\n\n", avgTurn / count);
	printf("------------------------------------------------------------------------\n");
	pthread_mutex_unlock(&mutex);

	return NULL;
}

/* Implementation Priority (Non-Preemptive) */
void *PNPT(void *args)
{
	double *burst = (((struct data*)args)->burst); //accessing the arguments from the passed struct
	int *proc = (((struct data*)args)->procNum);
	int count = (((struct data*)args)->count);
	int i = 0, j = 0, index, p, t, a;
	int *priority = (int *)malloc(sizeof(int) * count);
	double wait[count], turn[count], avgWait = 0, avgTurn = 0;
	
	//Copying the burst and process arrays into different arrays, so the original values don't get altered
	double burstC[count];
	memcpy(burstC, burst, count*sizeof(double));
	int procC[count];
	memcpy(procC, proc, count*sizeof(int));

	FILE *f = fopen(FILENAME, "r");

	if (!f)
	{
		printf("Error opening the file %s\n", FILENAME);
		return NULL;
	}
	fseek(f, 24, SEEK_SET); //Ignore the first line in the data file
	while (fscanf(f, "%d%d%d", &t, &a, &p) > 0)
	{ //reading the priorities for each process
		priority[i] = p;
		i++;
	}
	fclose(f);

	for (i = 0; i < count; i++)
	{ //Sorting the processes according to their priority
		index = i;
		for (j = i + 1; j < count; j++)
		{
			if (priority[j] < priority[index]) //If the prorioty of the next process is smaller than the current one
				index = j;
		}

		swapD(&burstC[i], &burstC[index]);
		swapI(&procC[i], &procC[index]);
		swapI(&priority[i], &priority[index]);
		pthread_mutex_lock(&mutex);
		jobs++;
		pthread_mutex_unlock(&mutex);
	}

	wait[0] = 0;
	for (int i = 1; i < count; i++){ 
		//Calulating the wait time for each process
		wait[i] = 0;
		for (j = 0; j < i; j++)
			wait[i] += burstC[j];
	}
	for (i = 0; i < count; i++) //Caluclating the turnaround time
		turn[i] = burstC[i] + wait[i];

	for (i = 0; i < count; i++)
	{ //Calculating the averages for bursts and turn around
		avgWait += wait[i];
		avgTurn += turn[i];
	}

	//Printing the results
	pthread_mutex_lock(&mutex);
	printf("Priority Scheduling (Non-Preemptive) Algorithm\n\n");
	printf("Process \t Burst Time \t Waiting Time \t Turnaround Time\n\n");
	for (i = 0; i < count; i++)
		printf("Process [%d] \t %.2lf \t\t %.2lf \t\t %.2lf\n", procC[i], burstC[i], wait[i], turn[i]);

	printf("\n\nAverage Waiting Time = %lf\n", avgWait / count);
	printf("Avgerage Turnaroud Time = %lf\n\n\n", avgTurn / count);
	printf("------------------------------------------------------------------------\n");
	free(priority);
	pthread_mutex_unlock(&mutex);

	return NULL;
}

/* Implementation of Round Robin */

void *RRT(void *args){
	double *burst = (((struct data*)args)->burst); //accessing the arguments from the passed struct
	int *proc = (((struct data*)args)->procNum);
	int count = (((struct data*)args)->count);
	double waitT[count], TurnT[count], avgWait = 0, avgTurn = 0, b, temp[count];
	int *arrival = (int *)malloc(sizeof(int) * count), a, p,i = 0, q, len, time, remain = count, flag = 0;
	FILE *f = fopen(FILENAME, "r");

	//Copying the burst and process arrays into different arrays, so the original values don't get altered
	double burstC[count];
	memcpy(burstC, burst, count*sizeof(double));
	int procC[count];
	memcpy(procC, proc, count*sizeof(int));

	if (!f){
		printf("Error opening the file %s\n", FILENAME);
		return NULL;
	}

	//Reading the time quantum from file
	fscanf(f, "%d", &q);
	fseek(f, 3, SEEK_SET); 

	while (fscanf(f, "%lf%d%d", &b, &a, &p) > 0){ //Reading the values of the arrival time for each process
		arrival[i] = a;
		i++;
	}
	fclose(f);

	//Making a copy for the burst times
	for(i = 0; i < count; i++)
		temp[i] = burstC[i];
		pthread_mutex_lock(&mutex);
		jobs++;
		pthread_mutex_unlock(&mutex);
	pthread_mutex_lock(&mutex);
	printf("Round Robin Schedluling\n\n");
	printf("Time quantum: %d\n", q);
	printf("\nProcess \t Burst Time \t Waiting Time \t Turnaround Time\n\n");

	 for(time = 0, len = 0; remain != 0;){
    	if(temp[len] <= q && temp[len] > 0){ //if the burst time is less than TQ
     		time += temp[len]; 
      		temp[len] = 0;
      		flag = 1;
		}
    	else if(temp[len] > 0){
      		temp[len] -= q;
      		time += q;
    	}
    	if(temp[len] == 0 && flag == 1){ //Calculating wait and turnaround times and printing process information
      		remain--;
	  		waitT[len] = burstC[len]+(time-arrival[len]-burstC[len]);
	  		TurnT[len] = time-arrival[len]-burstC[len];
      		printf("Process [%d] \t %.2lf \t\t %.2lf \t\t %.2lf\n",len+1,burstC[len], waitT[len], TurnT[len]);
      		avgWait+=waitT[len];
      		avgTurn+=TurnT[len];
      		flag=0;
    	}
    	if(len == count-1)
      		len = 0;
    	else if(arrival[len+1] <= time)
      		len++;
    	else
     		 len = 0;
  	}

  //Printing Results
	printf("\n\nAverage Waiting Time = %lf\n", avgWait / count);
	printf("Avgerage Turnaroud Time = %lf\n\n\n", avgTurn / count);
	printf("------------------------------------------------------------------------\n");
	free(arrival);
	pthread_mutex_unlock(&mutex);
	
	return NULL;
}

/*Implementation of Shortest Job First (Preemptive) */

void *SJFPT(void *args) {
	double *burst = (((struct data*)args)->burst); //accessing the arguemnts from the passed struct
	int *proc = (((struct data*)args)->procNum);
	int count = (((struct data*)args)->count);
	double waitT[count], TurnT[count], avgWait = 0, avgTurn = 0, b, p, temp[count];
	int *arrival = (int *)malloc(sizeof(int) * count), a, i = 0, j = 0, completed = 0, min = INT_MAX, t = 0, shortest = 0,finish;
	bool check = false;

	//Copying the burst and process arrays into different arrays, so the original values don't get altered
	double burstC[count];
	memcpy(burstC, burst, count*sizeof(double));
	int procC[count];
	memcpy(procC, proc, count*sizeof(int));

	FILE *f = fopen(FILENAME, "r");

	if (!f)
	{
		printf("Error opening the file %s\n", FILENAME);
		return NULL;
	}
	fseek(f, 3, SEEK_SET);

	while (fscanf(f, "%lf%lf%d", &b, &a, &p) > 0){ //Reading arrival times from data file
		arrival[i] = a;
		i++;
	}
	fclose(f);
	for(i = 0; i < count; i++)
		temp[i] = burstC[i];
		

	while (completed != count){
    	for (int j = 0; j < count; j++) {
    		if ((arrival[j] <= t) && (temp[j] < min) && temp[j] > 0) {
        		min = temp[j];
            	shortest = j;
            	check = true;
				pthread_mutex_lock(&mutex);
				jobs++;
				pthread_mutex_unlock(&mutex);
         	}
      	}
      if (check == false) {
         t++;
         continue;
      }
      // decrementing the remaining time
      temp[shortest]--;
      min = temp[shortest];
      if (min == 0)
         min = INT_MAX;
         // If a process gets completely executed
         if (temp[shortest] == 0) {
            completed++;
            check = false;
            finish = t + 1;
            // Calculate waiting time
            waitT[shortest] = finish - burstC[shortest] - arrival[shortest];
            if (waitT[shortest] < 0)
               waitT[shortest] = 0;
         }
         // Increment time
         t++;
	}

	//Calculate the turnaround time
	for(i = 0; i < count; i++)
		TurnT[i] = burst[i] + waitT[i];

	//Calculate the average for turnaround and wait time
	for(i = 0; i < count; i++){
		avgTurn += TurnT[i];
		avgWait += waitT[i];
	}

//Printing Results
	pthread_mutex_lock(&mutex);
	printf("Shortest Job First Scheduling (Preemptive) Algorithm\n\n");
	printf("\nProcess \t Burst Time \t Waiting Time \t Turnaround Time\n\n");
	for (i = 0; i < count; i++)
		printf("Process [%d] \t %.2lf \t\t %.2lf \t\t %.2lf\n", i+1, burstC[i], waitT[i], TurnT[i]);

	printf("\nAverage Waiting Time = %lf\n", avgWait / count);
	printf("Avgerage Turnaroud Time = %lf\n\n", avgTurn / count);
	printf("------------------------------------------------------------------------\n");
	free(arrival);
	pthread_mutex_unlock(&mutex);

	return NULL;
}

/* Implementation of priority scheduling (Preepmtive) */

void *PPT(void *args){

	double *burst = (((struct data*)args)->burst); //accessing the arguments passed in the struct
	int *proc = (((struct data*)args)->procNum);
	int count = (((struct data*)args)->count);
	int i = 0, j = 0, index, p, a;
	int *priority = (int *)malloc(sizeof(int) * count);
	int *arrival = (int *)malloc(sizeof(int) * count);
	double waitT[count], TurnT[count], avgWait = 0, avgTurn = 0, b, ft[count], temp[count];
	int completion[count], time, len = 0, smallest;
	double end;
	//Copying the burst and process arrays into different arrays, so the original values don't get altered
	double burstC[count];
	memcpy(burstC, burst, count*sizeof(double));
	int procC[count];
	memcpy(procC, proc, count*sizeof(int));

	FILE *f = fopen(FILENAME, "r");

	if (!f){
		printf("Error opening the file %s\n", FILENAME);
		return NULL;
	}
	fseek(f, 3, SEEK_SET); //Ignore the first line in the data file
	while (fscanf(f, "%lf%d%d", &b, &a, &p) > 0)
	{ //reading the priorities and arrival for each process
		priority[i] = p;
		arrival[i] = a;
		i++;
	}
	fclose(f);
	//Copying the burst times into a temporary array
	for(i = 0; i < count; i++)
		temp[i] = burstC[i];

	priority[count+1]=-1;
    for(time = 0; len != count; time++) {

        smallest=count+1;
        for(i=0; i < count; i++){
            if(arrival[i] <= time && priority[i] > priority[smallest] && temp[i] > 0 )
                smallest=i;
        }
        temp[smallest]--;

        if(temp[smallest]==0)
        {
            len++;
            end=time+1;
            completion[smallest] = end;
            waitT[smallest] = end - arrival[smallest] - burstC[smallest]; //calculate the wait time
            TurnT[smallest] = end - arrival[smallest]; //calculate the turn around time
			pthread_mutex_lock(&mutex);
			jobs++;
			printf("Job has been incremented to: %d by PID[%d]\n", jobs, getpid());
			pthread_mutex_unlock(&mutex);
		}
    }

	//Calculate the average turnaround and waiting time
	for(i = 0; i < count; i++){
		avgTurn += TurnT[i];
		avgWait += waitT[i];
	}

//Printing results
	pthread_mutex_lock(&mutex);
	printf("Priorioty Scheduling (Preemptive) Algorithm\n\n");
	printf("\nProcess \t Burst Time \t Priority \tWaiting Time \t Turnaround Time\n\n");
	for (i = 0; i < count; i++)
		printf("Process [%d] \t %.2lf \t\t %d \t\t %.2lf \t\t %.2lf\n", procC[i], burstC[i],priority[i], waitT[i], TurnT[i]);

	printf("\nAverage Waiting Time = %lf\n", avgWait / count);
	printf("Avgerage Turnaroud Time = %lf\n\n", avgTurn / count);
	printf("------------------------------------------------------------------------\n");
	free(arrival);
	free(priority);
	pthread_mutex_unlock(&mutex);
	
	return NULL;
}

//NONE THREADING SCHEDULIHG ALGO


void *FCFS(double *burstT, int count) {

	double waitT[count], TurnT[count], avgWait = 0, avgTurn = 0;
	int j = 0, i = 0;
	waitT[0] = 0;
	for (int i = 1; i < count; i++)
	{ //Calculating the wait time for each process
		waitT[i] = 0;
		for (j = 0; j < i; j++)
			waitT[i] += burstT[j];
	}

	for (i = 0; i < count; i++)
	{ //Calculating the turn around time for each process
		TurnT[i] = burstT[i] + waitT[i];
	}

	for (i = 0; i < count; i++)
	{ //Calculating the average for wait and turn around
		avgWait += waitT[i];
		avgTurn += TurnT[i];
	}

	//Printing the results
	
	printf("\n\nFirst Come First Server Scheduling Algorithm\n\n");
	printf("Process \t Burst Time \t Waiting Time \t Turnaround Time\n\n");
	for (i = 0; i < count; i++)
		printf("Process [%d] \t %.2lf \t\t %.2lf \t\t %.2lf\n", i + 1, burstT[i], waitT[i], TurnT[i]);

	printf("\n\nAverage Waiting Time = %lf\n", avgWait / count);
	printf("Avgerage Turnaroud Time = %lf\n\n\n", avgTurn / count);
	printf("------------------------------------------------------------------------\n");
	

	return NULL;
}

/*Implementation Shortest Job First (Non-Preemptive) */
void *SJFNP(double *burst, int *proc, int count)
{
	int i = 0, j = 0, index;
	double waitT[count], TurnT[count], avgWait = 0, avgTurn = 0;
	//Copying the burst and process arrays into different arrays, so the original values don't get altered
	double burstC[count];
	memcpy(burstC, burst, count*sizeof(double));

	int procC[count];
	memcpy(procC, proc, count*sizeof(int));

	for (i = 0; i < count; i++)
	{ //Sorting the burst times to find the shortest process
		index = i;
		for (j = i + 1; j < count; j++)
		{
			if (burstC[j] < burstC[index]) //If the next burst is shorter than the previous one, switch it.
				index = j;
		}
		swapD(&burstC[i], &burstC[index]);
		swapI(&procC[i], &procC[index]);
	}

	waitT[0] = 0;
	for (int i = 1; i < count; i++)
	{ //Calulating the wait time for each process
		waitT[i] = 0;
		for (j = 0; j < i; j++)
			waitT[i] += burstC[j];
	}
	for (i = 0; i < count; i++)
	{ //Calculating the turn aroud time for each process
		TurnT[i] = burstC[i] + waitT[i];
	}

	for (i = 0; i < count; i++)
	{ //Calculating the average for wait and turn around time
		avgWait += waitT[i];
		avgTurn += TurnT[i];
	}

	//Printing the results
	printf("Shortest Job First (Non-Preemptive) Algorithm\n\n");
	printf("Process \t Burst Time \t Waiting Time \t Turnaround Time\n\n");
	for (i = 0; i < count; i++)
		printf("Process [%d] \t %.2lf \t\t %.2lf \t\t %.2lf\n", procC[i], burstC[i], waitT[i], TurnT[i]);

	printf("\n\nAverage Waiting Time = %lf\n", avgWait / count);
	printf("Avgerage Turnaroud Time = %lf\n\n\n", avgTurn / count);
	printf("------------------------------------------------------------------------\n");

	return NULL;
}

/* Implementation Priority (Non-Preemptive) */
void *PNP(double *burst, int *proc, int count)
{
	int i = 0, j = 0, index, p, t, a;
	int *priority = (int *)malloc(sizeof(int) * count);
	double wait[count], turn[count], avgWait = 0, avgTurn = 0;
	
	//Copying the burst and process arrays into different arrays, so the original values don't get altered
	double burstC[count];
	memcpy(burstC, burst, count*sizeof(double));
	int procC[count];
	memcpy(procC, proc, count*sizeof(int));

	FILE *f = fopen(FILENAME, "r");

	if (!f)
	{
		printf("Error opening the file %s\n", FILENAME);
		return NULL;
	}
	fseek(f, 3, SEEK_SET); //Ignore the first line in the data file
	while (fscanf(f, "%d%d%d", &t, &a, &p) > 0)
	{ //reading the priorities for each process
		priority[i] = p;
		i++;
	}
	fclose(f);

	for (i = 0; i < count; i++)
	{ //Sorting the processes according to their priority
		index = i;
		for (j = i + 1; j < count; j++)
		{
			if (priority[j] < priority[index]) //If the prorioty of the next process is smaller than the current one
				index = j;
		}

		swapD(&burstC[i], &burstC[index]);
		swapI(&procC[i], &procC[index]);
		swapI(&priority[i], &priority[index]);
	}

	wait[0] = 0;
	for (int i = 1; i < count; i++){ 
		//Calulating the wait time for each process
		wait[i] = 0;
		for (j = 0; j < i; j++)
			wait[i] += burstC[j];
	}
	for (i = 0; i < count; i++) //Caluclating the turnaround time
		turn[i] = burstC[i] + wait[i];

	for (i = 0; i < count; i++)
	{ //Calculating the averages for bursts and turn around
		avgWait += wait[i];
		avgTurn += turn[i];
	}

	//Printing the results
	printf("Priority Scheduling (Non-Preemptive) Algorithm\n\n");
	printf("Process \t Burst Time \t Waiting Time \t Turnaround Time\n\n");
	for (i = 0; i < count; i++)
		printf("Process [%d] \t %.2lf \t\t %.2lf \t\t %.2lf\n", procC[i], burstC[i], wait[i], turn[i]);

	printf("\n\nAverage Waiting Time = %lf\n", avgWait / count);
	printf("Avgerage Turnaroud Time = %lf\n\n\n", avgTurn / count);
	printf("------------------------------------------------------------------------\n");
	free(priority);

	return NULL;
}

/* Implementation of Round Robin */

void *RR(double *burst, int *proc, int count){
	double waitT[count], TurnT[count], avgWait = 0, avgTurn = 0, b, temp[count];
	int *arrival = (int *)malloc(sizeof(int) * count), a, p,i = 0, q, len, time, remain = count, flag = 0;
	FILE *f = fopen(FILENAME, "r");

	//Copying the burst and process arrays into different arrays, so the original values don't get altered
	double burstC[count];
	memcpy(burstC, burst, count*sizeof(double));
	int procC[count];
	memcpy(procC, proc, count*sizeof(int));

	if (!f){
		printf("Error opening the file %s\n", FILENAME);
		return NULL;
	}

	//Reading the time quantum from file
	fscanf(f, "%d", &q);
	fseek(f, 2, SEEK_SET); //Skipping the burst, priority, and arrival line

	while (fscanf(f, "%lf%d%d", &b, &a, &p) > 0){ //Reading the values of the arrival time for each process
		arrival[i] = a;
		i++;
	}
	fclose(f);

	//Making a copy for the burst times
	for(i = 0; i < count; i++)
		temp[i] = burstC[i];
	printf("Round Robin Schedluling\n\n");
	printf("Time quantum: %d\n", q);
	printf("\nProcess \t Burst Time \t Waiting Time \t Turnaround Time\n\n");

	 for(time = 0, len = 0; remain != 0;){
    	if(temp[len] <= q && temp[len] > 0){ //if the burst time is less than TQ
     		time += temp[len]; 
      		temp[len] = 0;
      		flag = 1;
		}
    	else if(temp[len] > 0){
      		temp[len] -= q;
      		time += q;
    	}
    	if(temp[len] == 0 && flag == 1){ //Calculating wait and turnaround times and printing process information
      		remain--;
	  		waitT[len] = burstC[len]+(time-arrival[len]-burstC[len]);
	  		TurnT[len] = time-arrival[len]-burstC[len];
      		printf("Process [%d] \t %.2lf \t\t %.2lf \t\t %.2lf\n",len+1,burstC[len], waitT[len], TurnT[len]);
      		avgWait+=waitT[len];
      		avgTurn+=TurnT[len];
      		flag=0;
    	}
    	if(len == count-1)
      		len = 0;
    	else if(arrival[len+1] <= time)
      		len++;
    	else
     		 len = 0;
  	}

  //Printing Results
	printf("\n\nAverage Waiting Time = %lf\n", avgWait / count);
	printf("Avgerage Turnaroud Time = %lf\n\n\n", avgTurn / count);
	printf("------------------------------------------------------------------------\n");
	free(arrival);
	
	return NULL;
}

/*Implementation of Shortest Job First (Preemptive) */

void *SJFP(double *burst, int *proc, int count) {
	double waitT[count], TurnT[count], avgWait = 0, avgTurn = 0, b, p, temp[count];
	int *arrival = (int *)malloc(sizeof(int) * count), a, i = 0, j = 0, completed = 0, min = INT_MAX, t = 0, shortest = 0,finish;
	bool check = false;

	//Copying the burst and process arrays into different arrays, so the original values don't get altered
	double burstC[count];
	memcpy(burstC, burst, count*sizeof(double));
	int procC[count];
	memcpy(procC, proc, count*sizeof(int));

	FILE *f = fopen(FILENAME, "r");

	if (!f)
	{
		printf("Error opening the file %s\n", FILENAME);
		return NULL;
	}
	fseek(f, 3, SEEK_SET);

	while (fscanf(f, "%lf%lf%d", &b, &a, &p) > 0){ //Reading arrival times from data file
		arrival[i] = a;
		i++;
	}
	fclose(f);
	for(i = 0; i < count; i++)
		temp[i] = burstC[i];
		

	while (completed != count){
    	for (int j = 0; j < count; j++) {
    		if ((arrival[j] <= t) && (temp[j] < min) && temp[j] > 0) {
        		min = temp[j];
            	shortest = j;
            	check = true;
         	}
      	}
      if (check == false) {
         t++;
         continue;
      }
      // decrementing the remaining time
      temp[shortest]--;
      min = temp[shortest];
      if (min == 0)
         min = INT_MAX;
         // If a process gets completely executed
         if (temp[shortest] == 0) {
            completed++;
            check = false;
            finish = t + 1;
            // Calculate waiting time
            waitT[shortest] = finish - burstC[shortest] - arrival[shortest];
            if (waitT[shortest] < 0)
               waitT[shortest] = 0;
         }
         // Increment time
         t++;
	}

	//Calculate the turnaround time
	for(i = 0; i < count; i++)
		TurnT[i] = burst[i] + waitT[i];

	//Calculate the average for turnaround and wait time
	for(i = 0; i < count; i++){
		avgTurn += TurnT[i];
		avgWait += waitT[i];
	}

//Printing Results
	printf("Shortest Job First Scheduling (Preemptive) Algorithm\n\n");
	printf("\nProcess \t Burst Time \t Waiting Time \t Turnaround Time\n\n");
	for (i = 0; i < count; i++)
		printf("Process [%d] \t %.2lf \t\t %.2lf \t\t %.2lf\n", i+1, burstC[i], waitT[i], TurnT[i]);

	printf("\nAverage Waiting Time = %lf\n", avgWait / count);
	printf("Avgerage Turnaroud Time = %lf\n\n", avgTurn / count);
	printf("------------------------------------------------------------------------\n");
	free(arrival);

	return NULL;
}

/* Implementation of priority scheduling (Preepmtive) */

void *PP(double *burst, int *proc, int count){
	int i = 0, j = 0, index, p, a;
	int *priority = (int *)malloc(sizeof(int) * count);
	int *arrival = (int *)malloc(sizeof(int) * count);
	double waitT[count], TurnT[count], avgWait = 0, avgTurn = 0, b, ft[count], temp[count];
	int completion[count], time, len = 0, smallest;
	double end;
	//Copying the burst and process arrays into different arrays, so the original values don't get altered
	double burstC[count];
	memcpy(burstC, burst, count*sizeof(double));
	int procC[count];
	memcpy(procC, proc, count*sizeof(int));

	FILE *f = fopen(FILENAME, "r");

	if (!f){
		printf("Error opening the file %s\n", FILENAME);
		return NULL;
	}
	fseek(f, 3, SEEK_SET); //Ignore the first line in the data file
	while (fscanf(f, "%lf%d%d", &b, &a, &p) > 0)
	{ //reading the priorities and arrival for each process
		priority[i] = p;
		arrival[i] = a;
		i++;
	}
	fclose(f);
	//Copying the burst times into a temporary array
	for(i = 0; i < count; i++)
		temp[i] = burstC[i];

	priority[count+1]=-1;
    for(time = 0; len != count; time++) {

        smallest=count+1;
        for(i=0; i < count; i++){
            if(arrival[i] <= time && priority[i] > priority[smallest] && temp[i] > 0 )
                smallest=i;
        }
        temp[smallest]--;

        if(temp[smallest]==0)
        {
            len++;
            end=time+1;
            completion[smallest] = end;
            waitT[smallest] = end - arrival[smallest] - burstC[smallest]; //calculate the wait time
            TurnT[smallest] = end - arrival[smallest]; //calculate the turn around time
        }
    }

	//Calculate the average turnaround and waiting time
	for(i = 0; i < count; i++){
		avgTurn += TurnT[i];
		avgWait += waitT[i];
	}

//Printing results
	printf("Priorioty Scheduling (Preemptive) Algorithm\n\n");
	printf("\nProcess \t Burst Time \t Priority \tWaiting Time \t Turnaround Time\n\n");
	for (i = 0; i < count; i++)
		printf("Process [%d] \t %.2lf \t\t %d \t\t %.2lf \t\t %.2lf\n", procC[i], burstC[i],priority[i], waitT[i], TurnT[i]);

	printf("\nAverage Waiting Time = %lf\n", avgWait / count);
	printf("Avgerage Turnaroud Time = %lf\n\n", avgTurn / count);
	printf("------------------------------------------------------------------------\n");
	free(arrival);
	free(priority);
	
	return NULL;
}