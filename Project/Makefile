run:
	gcc -g -o shw sharedMemoryWriter.c -lrt -lpthread
	./shw
	gcc -g -o main main.c -lrt -lpthread
	./main


shmw:
	gcc -g -o shw sharedMemoryWriter.c -lrt -lpthread
	./shw

output: shmw
	gcc -g -o main main.c -lrt -D OUTPUT -lpthread
	./main
	
clean:
	rm shw main

threads: shmw
	gcc -g -o main main.c -lrt -D THREADS -lpthread
	./main


seq: shmw
	gcc -g -o main main.c -lrt -D SEQ -lpthread
	./main

threadsOuput: shmw
	gcc -g -o main main.c -lrt -D THREADS -D OUTPUT -lpthread
	./main

seqOutput: shmw
	gcc -g -o main main.c -lrt -D SEQ -D OUTPUT -lpthread
	./main