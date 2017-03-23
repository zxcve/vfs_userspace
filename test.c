#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/wait.h>

/* Max levels used for process tree */
int max_level = 0;

/* Signal Character Array for Printing the signal */
char *signal_array[] = {"SIGHUP","SIGINT","SIGQUIT","SIGILL","SIGTRAP","SIGIOT",
	"SIGBUS","SIGFPE","SIGKILL","SIGUSR1","SIGSEGV","SIGUSR2","SIGPIPE","SIGALRM",
	"SIGTERM","SIGSTKFLT","SIGCHLD","SIGCONT","SIGSTOP","SIGTSTP","SIGTTIN",
	"SIGTTOU","SIGURG","SIGXCPU","SIGXFSZ","SIGVTALRM","SIGPROF","SIGWINCH",
	"SIGIO","SIGPWR"};

/* Declaration to compile */
static void * process_handler(void *arg);

/**
 * @brief Spawns two threads and wait for joining. The wait only succeeds when
 * the child threads are killed explicitely. The process never exits as it
 * always wait infinitely for the signals.
 *
 * @param level The level for the tree.
 */
static void thread_handler(int level)
{
	/* Only two threads per process */
	pthread_t thread[2];
	int i, ret;
	int nlevel = level;

	/* Dont create new threads once the max level is reached */
	if (nlevel < max_level) {

		for (i = 0; i < 2; i++) {
			/* Create the threads */
			ret = pthread_create(&thread[i], NULL,
					     process_handler, &nlevel);
			if(ret) {
				printf("Error - pthread_create() return code: %d\n",ret);
				exit(EXIT_FAILURE);
			}
		}
		/* Wait for joining of both the threads */
		pthread_join(thread[0], NULL);
		pthread_join(thread[1], NULL);
	}

	/* Infinitely wait for signals */
	while (1) {
		pause();
	}
}

/**
 * @brief Forks two process and wait for completion. This wait will only succeed
 * when the child was killed. The parent process never exits as it always wait
 * infinitely for the signals
 *
 * @param arg Pointer for the level
 *
 * @return Never returns as it waits for signals
 */
static void *process_handler(void *arg)
{
	/* Only two process per parent */
	pid_t pid[2];
	int i;
	int level = (*(int *)arg) + 1;

	for (i = 0; i < 2; i++) {
		pid[i] = fork();
		if (pid[i] == 0) {
			/* Child goes for spawning threads */
			thread_handler(level);
		}
	}

	/* Parent waits for child completion */
	for (i = 0; i < 2; i++)
		wait(NULL);

	/* Infinitely wait for signals */
	while (1) {
		pause();
	}

	/* Should never reach here */
	return NULL;
}

/**
 * @brief Generic Handler for all signals
 *
 * @param s Signal number being handled
 */
static void sig_handler(int s)
{
	/*NOTE: SIGKILL & SIGSTOP cannot be handled */
	printf("\n%s-> TID<%ld> PID<%d>\n",
	       signal_array[s-1], syscall( __NR_gettid), getpid());
}

void main(int argc, const char *argv[])
{
	int i;
	struct sigaction sa;

	/* Argument checking */
	if (argc < 2) {
		printf("./test <levels>\n");
		printf("Max allowed level is 5 as tree is exponential\n");
		exit(EXIT_FAILURE);
	}

	max_level = atoi(argv[1]);

	/* Argument range checking */
	if (max_level > 5 || max_level <= 0) {
		printf("Allowed level allowed is (0-5]\n");
		exit(EXIT_FAILURE);
	}

	/* Installs all the signal handlers */
	for (i = 1; i <= 30; i++) {
		if (i == 9 || i == 19)
			continue;
		sa.sa_handler = sig_handler;
		sigemptyset(&sa.sa_mask);
		if (sigaction(i, &sa, NULL) == -1) {
			printf("Signal handler setting failed for %s signal\n",
			       signal_array[i-1]);
		}
	}

	/* Initiate recurse process creation */
	thread_handler(0);
}
