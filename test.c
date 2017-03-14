#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/wait.h>

int max_level = 0;

char *signal_array[] = {"SIGHUP","SIGINT","SIGQUIT","SIGILL","SIGTRAP","SIGIOT",
	"SIGBUS","SIGFPE","SIGKILL","SIGUSR1","SIGSEGV","SIGUSR2","SIGPIPE","SIGALRM",
	"SIGTERM","SIGSTKFLT","SIGCHLD","SIGCONT","SIGSTOP","SIGTSTP","SIGTTIN",
	"SIGTTOU","SIGURG","SIGXCPU","SIGXFSZ","SIGVTALRM","SIGPROF","SIGWINCH",
	"SIGIO","SIGPWR"};

static void * process_handler(void *arg);

static void thread_handler(int level)
{
	pthread_t thread[2];
	int i, ret;
	int nlevel = level;

	if (nlevel < max_level) {

		for (i = 0; i < 2; i++) {
			ret = pthread_create(&thread[i], NULL,
					     process_handler, &nlevel);
			if(ret) {
				printf("Error - pthread_create() return code: %d\n",ret);
				exit(EXIT_FAILURE);
			}
		}
		pthread_join(thread[0], NULL);
		pthread_join(thread[1], NULL);
	}
	while (1) {
		pause();
	}
}

static void *process_handler(void *arg)
{
	int i;
	int pid[2];
	int level = (*(int *)arg) + 1;

	for (i = 0; i < 2; i++) {
		pid[i] = fork();
		if (pid[i] == 0) {
			thread_handler(level);
		}
	}
	for (i = 0; i < 2; i++)
		wait(NULL);

	while (1) {
		pause();
	}
}

static void sig_handler(int s)
{
	printf("\nSignal %s for tid %ld pid %d\n",
	       signal_array[s-1], syscall( __NR_gettid), getpid());
}

void main(int argc, const char *argv[])
{
	int i;

	if (argc < 2) {
		printf("./test <levels>\n");
		printf("Max allowed level is 5 as tree is exponential\n");
		exit(EXIT_FAILURE);
	}

	max_level = atoi(argv[1]);

	if (max_level > 5) {
		printf("max level allowed is 5\n");
		exit(EXIT_FAILURE);
	}

	printf("%d",getpid());

	for (i = 1; i <= 30; i++)
		signal(i, sig_handler);

	thread_handler(0);
}


