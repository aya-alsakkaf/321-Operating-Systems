/* Creating shared memory for data1.txt for burst time */
#include "phase1.h"
#define FILENAME "newdata_100.txt"

int main()
{
	const char *name = "BurstTime";
	int shm_fd, SIZE = 0;
	void *ptr;
	struct stat st;
	int t, tt, ttt;
	char *buffer = NULL;
    size_t buf_size = 0;

	FILE *f = fopen(FILENAME, "r");
	if(!f){
		printf("File Couldn't be opened\n");
		return 1;
	}
	stat(FILENAME, &st);
	SIZE = st.st_size;
	char msg[SIZE];

	shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666); //Creating the shared memory

	ftruncate(shm_fd,SIZE); //Configure the size of the shared memory segment

	ptr = mmap(0,SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0); //Map the shared memory 
	if (ptr == MAP_FAILED) {
		printf("Map failed\n");
		return -1;
	}

	 //write to the shared memory region.
	fseek(f, 3, SEEK_SET); //ignore the first line in the data file
	int read = getline(&buffer, &buf_size, f);
	while(read >= 0){
		sscanf(buffer, "%d%d%d", &t, &tt, &ttt);
		sprintf(msg, "%d\n", t);
		sprintf(ptr, "%s", msg);
		ptr += strlen(msg);
		memset(msg, 0, sizeof(msg));
		read = getline(&buffer, &buf_size, f);
	}
	fclose(f);
	return 0;
}