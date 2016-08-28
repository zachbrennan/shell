/* 
 * tsh - A tiny shell program with job control
 * 
 * Zachary Brennan ZAB37 
 */

//test11-14?
//last test case
//pipe and redirect?

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>

/* Misc manifest constants */
#define MAXLINE    1024   /* max line size */
#define MAXARGS     128   /* max args on a command line */
#define MAXJOBS      16   /* max jobs at any point in time */
#define MAXJID    1<<16   /* max job ID */
#define PIPE_READ 	 0
#define PIPE_WRITE  	 1

/* Job states */
#define UNDEF 0 /* undefined */
#define FG 1    /* running in foreground */
#define BG 2    /* running in background */
#define ST 3    /* stopped */

/* 
 * Jobs states: FG (foreground), BG (background), ST (stopped)
 * Job state transitions and enabling actions:
 *     FG -> ST  : ctrl-z
 *     ST -> FG  : fg command
 *     ST -> BG  : bg command
 *     BG -> FG  : fg command
 * At most 1 job can be in the FG state.
 */

/* Global variables */
extern char **environ;      /* defined in libc */
char prompt[] = "tsh> ";    /* command line prompt (DO NOT CHANGE) */
int verbose = 0;            /* if true, print additional output */
int nextjid = 1;            /* next job ID to allocate */
char sbuf[MAXLINE];         /* for composing sprintf messages */
pid_t fpid;		    			 /* foreground pid*/
int in = -1;	    				 /* sets to true(1) in parseLine if < is found  */
int out = -1;	   			 /* sets to true(1) in parseLine if > is found  */
int IOfileIndex = 0;			 /* set in ParseLine, to the index of the filename for IO redirection  */
char* IOredirFile;	 /* file name for IO redirection  */
char** pipeCommand;
int pipeCount = 0;
int parsedLength = 0;
int pipeIndex = 0;

struct job_t {              /* The job struct */
    pid_t pid;              /* job PID */
    int jid;                /* job ID [1, 2, ...] */
    int state;              /* UNDEF, BG, FG, or ST */
    char cmdline[MAXLINE];  /* command line */
};
struct job_t jobs[MAXJOBS]; /* The job list */
/* End global variables */


/* Function prototypes */

/* Here are the functions that you will implement */
void eval(char *cmdline);
int builtin_cmd(char **argv);
void do_bgfg(char **argv);
void waitfg(pid_t pid);
void launch(char** argv, int bg);
void launch2(char** argv);

void sigchld_handler(int sig);
void sigtstp_handler(int sig);
void sigint_handler(int sig);
void sigcont_handler(int sig);
void printJob(pid_t pid);
void getIOfile(char** argv);
void parseParsed(char** argv);
void launch3(char** argv);
void launch4(char** argv);
void getParsedLength(char** argv);

/* Here are helper routines that we've provided for you */
int parseline(const char *cmdline, char **argv); 
void sigquit_handler(int sig);

void clearjob(struct job_t *job);
void initjobs(struct job_t *jobs);
int maxjid(struct job_t *jobs); 
int addjob(struct job_t *jobs, pid_t pid, int state, char *cmdline);
int deletejob(struct job_t *jobs, pid_t pid); 
pid_t fgpid(struct job_t *jobs);
struct job_t *getjobpid(struct job_t *jobs, pid_t pid);
struct job_t *getjobjid(struct job_t *jobs, int jid); 
int pid2jid(pid_t pid); 
void listjobs(struct job_t *jobs);

void usage(void);
void unix_error(char *msg);
void app_error(char *msg);
typedef void handler_t(int);
handler_t *Signal(int signum, handler_t *handler);

/*
 * main - The shell's main routine 
 */
int main(int argc, char **argv) 
{
    char c;
    char cmdline[MAXLINE];
    int emit_prompt = 1; /* emit prompt (default) */

    /* Redirect stderr to stdout (so that driver will get all output
     * on the pipe connected to stdout) */
    dup2(1, 2);

    /* Parse the command line */
    while ((c = getopt(argc, argv, "hvp")) != EOF) {
        switch (c) {
        case 'h':             /* print help message */
            usage();
	    break;
        case 'v':             /* emit additional diagnostic info */
            verbose = 1;
	    break;
        case 'p':             /* don't print a prompt */
            emit_prompt = 0;  /* handy for automatic testing */
	    break;
		default:
            usage();
		}
	 }

    /* Install the signal handlers */

    /* These are the ones you will need to implement */
    Signal(SIGINT,  sigint_handler);   /* ctrl-c */
    Signal(SIGTSTP, sigtstp_handler);  /* ctrl-z */
    Signal(SIGCHLD, sigchld_handler);  /* Terminated or stopped child */

    /* This one provides a clean way to kill the shell */
    Signal(SIGQUIT, sigquit_handler); 

    /* Initialize the job list */
    initjobs(jobs);

    /* Execute the shell's read/eval loop */
    while (1) {

	/* Read command line */
	if (emit_prompt) {
	    printf("%s", prompt);
	    fflush(stdout);
	}
	if ((fgets(cmdline, MAXLINE, stdin) == NULL) && ferror(stdin))
	    app_error("fgets error");
	if (feof(stdin)) { /* End of file (ctrl-d) */
	    fflush(stdout);
	    exit(0);
	}
		
	char** parsedArgs = malloc(MAXARGS * sizeof parsedArgs);
	int bg = parseline(cmdline, parsedArgs);
	//Argv is now the input line, parsed to separate arguments from the command.
	//the last entry in the array is NULL, so that we do not need to find or store the size
	
	/* Evaluate the command line */
	int builtin = builtin_cmd(parsedArgs);

	//	getIOfile(parsedArgs); //gets location of file name if < or > is found
	//	getPipeIndex(parsedArgs);

	//parsedArgs[IOfileIndex-1] = NULL; //sets the < or > if found to NULL so they are not included when command is run

	//	printf("parsed %c \n builtin %d\n", *parsedArgs[0], builtin);
	parseParsed(parsedArgs);
	getParsedLength(parsedArgs);
	//if(pipeCount > 0)
//	{
		launch4(parsedArgs);
//	}
	/*
	if(builtin == 0)
	{
		launch3(parsedArgs);
	}
	*/

	//if(builtin == 0)
	//	launch(parsedArgs);
/*
	if(builtin == 0)
	{
		eval(cmdline);
	}	
*/
	fflush(stdout);
	fflush(stdout);
   }
    exit(0); /* control never reaches here */
}
  
/* 
 * eval - Evaluate the command line that the user has just typed in
 * 
 * If the user has requested a built-in command (quit, jobs, bg or fg)
 * then execute it immediately. Otherwise, fork a child process and
 * run the job in the context of the child. If the job is running in
 * the foreground, wait for it to terminate and then return.  Note:
 * each child process must have a unique process group ID so that our
 * background children don't receive SIGINT (SIGTSTP) from the kernel
 * when we type ctrl-c (ctrl-z) at the keyboard.  
*/
void eval(char *cmdline) 
{
	if(strcmp(cmdline, "help\n") == 0)
	{
		printf("Entered help\n");
	}
	else if(strcmp(cmdline, "quit\n") == 0)
	{
		printf("\nQuitting\n\n");
		exit(0);
	}
	else
	{
		printf("Command not recognized.\n");
	}
	return;
}

/* 
 * parseline - Parse the command line and build the argv array.
 * 
 * Characters enclosed in single quotes are treated as a single
 * argument.  Return true if the user has requested a BG job, false if
 * the user has requested a FG job.  
 */
int parseline(const char *cmdline, char **argv) 
{
    static char array[MAXLINE]; /* holds local copy of command line */
    char *buf = array;          /* ptr that traverses command line */
    char *delim;                /* points to first space delimiter */
    int argc;                   /* number of args */
    int bg;                     /* background job? */
	 in = -1;
	 out = -1;

	 strcpy(buf, cmdline);
    buf[strlen(buf)-1] = ' ';  /* replace trailing '\n' with space */
    while (*buf && (*buf == ' ')) /* ignore leading spaces */
		buf++;

    /* Build the argv list */
    argc = 0;
    if (*buf == '\'') 
	 {
	   buf++;
  	 	delim = strchr(buf, '\'');
    }
 	/* else if (*buf == '<')
	 {
		delim = strchr(buf,' ');
		in = 1;
		printf("\nin\n");
	 }
	 else if(*buf == '>')
	 {
		delim = strchr(buf, ' ');
		out = 1;
		printf("\nout\n");
	 }*/
    else 
	 {
	 	delim = strchr(buf, ' ');
    }

    while (delim) {
	argv[argc++] = buf;
	*delim = '\0';
	buf = delim + 1;
	while (*buf && (*buf == ' ')) /* ignore spaces */
	       buf++;

	if (*buf == '\'') {
	    buf++;
	    delim = strchr(buf, '\'');
	}
	else {
	    delim = strchr(buf, ' ');
	}
    }
    argv[argc] = NULL;
    
    if (argc == 0)  /* ignore blank line */
	return 1;

    /* should the job run in the background? */
    if ((bg = (*argv[argc-1] == '&')) != 0) {
	argv[--argc] = NULL;
    }
    return bg;
}

/* 
 * builtin_cmd - If the user has typed a built-in command then execute
 *    it immediately.  
 */
int builtin_cmd(char **argv) 
{	
	 if(argv[0] == NULL)
	 {
		return -1;
 	 }
	 if(strcmp(argv[0], "quit") == 0)
	 {
		 printf("\nQuitting\n\n");
		 exit(0);
	 }	
	 else if(strcmp(argv[0], "jobs") == 0)
	 {
		 listjobs(jobs);
	 	 return 1;	
	 } 
	 else if(strcmp(argv[0], "bg") == 0)
	 {
		 do_bgfg(argv);
		 return 1;
	 }
  	 else if(strcmp(argv[0], "fg") == 0)
	 {
		 do_bgfg(argv);
		 return 1;
	 }
	
    return 0;     /* not a builtin command */
}

/* 
 * do_bgfg - Execute the builtin bg and fg commands
 */
void do_bgfg(char **argv) 
{
	if (argv[1] == NULL)
	{
		printf("%s requires an argument of the form PID or %%JID\n",argv[0]);
		return;
	}
	struct job_t* bgfgJob;
	if(strcmp(argv[0], "bg") == 0)
	{
		if(strncmp(argv[1], "%",1) == 0)
		{
			char* ch = "%";
			char* ret = strtok(argv[1],ch);
			int jid = atoi(ret);	
			bgfgJob = getjobjid(jobs, jid);
			if(bgfgJob == NULL)
			{
				printf("%%%d: no such job\n",jid);
				return;
			}
			kill(-bgfgJob->pid, SIGCONT);	
			bgfgJob->state = BG;	
			printJob(bgfgJob->pid);
		}
		else
		{
			int pid = atoi(argv[1]);	
			bgfgJob = getjobpid(jobs, pid);
			if(bgfgJob == NULL)
			{
				printf("(%d): no such process\n",pid);
				return;
			}
			kill(-bgfgJob->pid, SIGCONT);	
			bgfgJob->state = BG;	
			printJob(bgfgJob->pid);
		}
	}
	else if(strcmp(argv[0], "fg") == 0)
	{
		if(strncmp(argv[1], "%",1) == 0)
		{
			char* ch = "%";
			char* ret = strtok(argv[1],ch);
			int jid = atoi(ret);	
			bgfgJob = getjobjid(jobs, jid);
			if(bgfgJob == NULL)
			{
				printf("%%%d: no such job\n",jid);
				return;
			}
			kill(-bgfgJob->pid, SIGCONT);	
			bgfgJob->state = FG;	
			fpid = bgfgJob->pid;
			waitfg(fpid);
		}
		else
		{
			int pid = atoi(argv[1]);	
			bgfgJob = getjobpid(jobs, pid);
			if(bgfgJob == NULL)
			{
				printf("(%d): no such process\n",pid);
				return;
			}
			kill(-bgfgJob->pid, SIGCONT);	
			bgfgJob->state = FG;
			fpid = bgfgJob->pid;	
			waitfg(fpid);
		}

	}

	return;
}

/* 
 * waitfg - Block until process pid is no longer the foreground process
 */
void waitfg(pid_t fpid)
{

	while (fpid != 0)
	{
		fpid = fgpid(jobs);
		//sleep(1); //This line would make it more efficient, but way slower, it seems. Find an alternative?
	};

    return;
}

/*****************
 * Signal handlers
 *****************/

/* 
 * sigchld_handler - The kernel sends a SIGCHLD to the shell whenever
 *     a child job terminates (becomes a zombie), or stops because it
 *     received a SIGSTOP or SIGTSTP signal. The handler reaps all
 *     available zombie children, but doesn't wait for any other
 *     currently running children to terminate.  
 */
void sigchld_handler(int sig) 
{
	int pid = waitpid(-1, NULL, WNOHANG);
	//fpid = fgpid(jobs);
	deletejob(jobs, pid);
	return;
}

/* 
 * sigint_handler - The kernel sends a SIGINT to the shell whenver the
 *    user types ctrl-c at the keyboard.  Catch it and send it along
 *    to the foreground job.  
 */
void sigint_handler(int sig) 
{
	 fpid = fgpid(jobs);
	 if(fpid == 0)
	 {
		 printf("No foreground process to terminate.\n");
		 return;
	 }
	 else
	 {
		struct job_t* termJob = getjobpid(jobs,fpid);
		printf("\nJob [%d] (%d) terminated with signal %d\n",termJob->jid,termJob->pid,sig);
	 	killpg(fpid, SIGTERM);
		fpid = 0;
	 }
	 return;
}

/*
 * sigtstp_handler - The kernel sends a SIGTSTP to the shell whenever
 *     the user types ctrl-z at the keyboard. Catch it and suspend the
 *     foreground job by sending it a SIGTSTP.  
 */
void sigtstp_handler(int sig) 
{
	 fpid = fgpid(jobs);
	 if(fpid == 0)
	 {
		 printf("\nNo foreground process to stop.\n");
		 return;
	 }
	 else
	 {
		struct job_t* stoppedJob = getjobpid(jobs,fpid);
		printf("\nJob [%d] (%d) stopped with signal %d\n",stoppedJob->jid,stoppedJob->pid,sig);
	 	killpg(fpid, SIGTSTP);
		struct job_t* paused = getjobpid(jobs,fpid);
		paused->state = ST;
		fpid = 0;
	 }
    return;
}

void sigcont_handler(int sig)
{
	
	return;
}

/*********************
 * End signal handlers
 *********************/

/***********************************************
 * Helper routines that manipulate the job list
 **********************************************/

/* clearjob - Clear the entries in a job struct */
void clearjob(struct job_t *job) {
    job->pid = 0;
    job->jid = 0;
    job->state = UNDEF;
    job->cmdline[0] = '\0';
}

/* initjobs - Initialize the job list */
void initjobs(struct job_t *jobs) {
    int i;

    for (i = 0; i < MAXJOBS; i++)
	clearjob(&jobs[i]);
}

/* maxjid - Returns largest allocated job ID */
int maxjid(struct job_t *jobs) 
{
    int i, max=0;

    for (i = 0; i < MAXJOBS; i++)
	if (jobs[i].jid > max)
	    max = jobs[i].jid;
    return max;
}

/* addjob - Add a job to the job list */
int addjob(struct job_t *jobs, pid_t pid, int state, char *cmdline) 
{
    int i;
    
    if (pid < 1)
	return 0;

    for (i = 0; i < MAXJOBS; i++) {
	if (jobs[i].pid == 0) {
	    jobs[i].pid = pid;
	    jobs[i].state = state;
	    jobs[i].jid = nextjid++;
	    if (nextjid > MAXJOBS)
		nextjid = 1;
	    strcpy(jobs[i].cmdline, cmdline);
  	    if(verbose){
	        printf("Added job [%d] %d %s\n", jobs[i].jid, jobs[i].pid, jobs[i].cmdline);
            }
            return 1;
	}
    }
    printf("Tried to create too many jobs\n");
    return 0;
}

/* deletejob - Delete a job whose PID=pid from the job list */
int deletejob(struct job_t *jobs, pid_t pid) 
{
    int i;

    if (pid < 1)
	return 0;

    for (i = 0; i < MAXJOBS; i++) {
	if (jobs[i].pid == pid) {
	    clearjob(&jobs[i]);
	    nextjid = maxjid(jobs)+1;
	    return 1;
	}
    }
    return 0;
}

/* fgpid - Return PID of current foreground job, 0 if no such job */
pid_t fgpid(struct job_t *jobs) {
    int i;

    for (i = 0; i < MAXJOBS; i++)
	if (jobs[i].state == FG)
	    return jobs[i].pid;
    return 0;
}

/* getjobpid  - Find a job (by PID) on the job list */
struct job_t *getjobpid(struct job_t *jobs, pid_t pid) {
    int i;

    if (pid < 1)
	return NULL;
    for (i = 0; i < MAXJOBS; i++)
	if (jobs[i].pid == pid)
	    return &jobs[i];
    return NULL;
}

/* getjobjid  - Find a job (by JID) on the job list */
struct job_t *getjobjid(struct job_t *jobs, int jid) 
{
    int i;

    if (jid < 1)
	return NULL;
    for (i = 0; i < MAXJOBS; i++)
	if (jobs[i].jid == jid)
	    return &jobs[i];
    return NULL;
}

/* pid2jid - Map process ID to job ID */
int pid2jid(pid_t pid) 
{
    int i;

    if (pid < 1)
	return 0;
    for (i = 0; i < MAXJOBS; i++)
	if (jobs[i].pid == pid) {
            return jobs[i].jid;
        }
    return 0;
}

/* listjobs - Print the job list */
void listjobs(struct job_t *jobs) 
{
    int i;
    for (i = 0; i < MAXJOBS; i++) {
	if (jobs[i].pid != 0) {
	    printf("[%d] (%d) ", jobs[i].jid, jobs[i].pid);
	    switch (jobs[i].state) {
		case BG: 
		    printf("Running ");
		    break;
		case FG: 
		    printf("Foreground ");
		    break;
		case ST: 
		    printf("Stopped ");
		    break;
	    default:
		    printf("listjobs: Internal error: job[%d].state=%d ", 
			   i, jobs[i].state);
	    }
	    printf("%s", jobs[i].cmdline);
		 printf("\n");
	}
    }
}

void printJob(pid_t pid)
{
	struct job_t* job = getjobpid(jobs,pid);
	printf("[%d] (%d) ", job->jid, job->pid);
	switch (job->state) {
		case BG: 
			printf("Running ");
			break;
		case FG: 
			printf("Foreground ");
			break;
		case ST: 
			printf("Stopped ");
			break;
	}
	printf("%s", job->cmdline);
	printf("\n");
	return;
}

/******************************
 * end job list helper routines
 ******************************/


/***********************
 * Other helper routines
 ***********************/

/*
 * usage - print a help message
 */
void usage(void) 
{
    printf("Usage: shell [-hvp]\n");
    printf("   -h   print this message\n");
    printf("   -v   print additional diagnostic information\n");
    printf("   -p   do not emit a command prompt\n");
    exit(1);
}

/*
 * unix_error - unix-style error routine
 */
void unix_error(char *msg)
{
    fprintf(stdout, "%s: %s\n", msg, strerror(errno));
    exit(1);
}

/*
 * app_error - application-style error routine
 */
void app_error(char *msg)
{
    fprintf(stdout, "%s\n", msg);
    exit(1);
}

/*
 * Signal - wrapper for the sigaction function
 */
handler_t *Signal(int signum, handler_t *handler) 
{
    struct sigaction action, old_action;

    action.sa_handler = handler;  
    sigemptyset(&action.sa_mask); /* block sigs of type being handled */
    action.sa_flags = SA_RESTART; /* restart syscalls if possible */

    if (sigaction(signum, &action, &old_action) < 0)
	unix_error("Signal error");
    return (old_action.sa_handler);
}

/*
 * sigquit_handler - The driver program can gracefully terminate the
 *    child shell by sending it a SIGQUIT signal.
 */
void sigquit_handler(int sig) 
{
    printf("Terminating after receipt of SIGQUIT signal\n");
    exit(1);
}

void launch(char** argv, int bg)
{
	//bg == 1 means do in background
	pid_t pid;

	pid = fork();

	if(bg == 1)
		addjob(jobs,pid,BG,argv[0]);
	else if(bg == 0)
		addjob(jobs,pid,FG, argv[0]);
	//if(pid != 0)
//		printJob(pid);


//		printf("pid from parent %d \n",getpid());
	signal(SIGCHLD, sigchld_handler);
	if(pid < 0)
	{
		//Fork error
		printf("Error creating fork");
	}
	else if(pid == 0)
	{
		//Child process
		//printJob(getpid());

		if(in == 1)
		{
			int inFD = open(IOredirFile, O_RDONLY);
			dup2(inFD,STDIN_FILENO);
			close(inFD);
		}
		else if(out == 1)
		{
			int outFD = creat(IOredirFile, 0644);
			dup2(outFD, STDOUT_FILENO);
			close(outFD);
		}
		/*
		if(foundPipe == 1)
		{	
			int fd[2];
			pipe(fd);
			getIOfile(pipeCommand);
			if(fork() == 0)
			{
				//child process for pipe command
				close(fd[1]);
				dup2(fd[0],0);
				execvp(pipeCommand[0],pipeCommand);
//				launch(pipeCommand,0);
			}
			else
			{
				//parent
				close(fd[0]);
				dup2(fd[1],1);
//				execvp(argv[0],argv);
			}
		}
		*/
		setpgid(getpid(),getpid());//can be replaced with setpgid(0,0), which does the same thing
		if(execvp(argv[0], argv) == -1)
		{
			printf("(");
			fputs(argv[0], stdout);
			printf("): is not a valid command.\n");
		}
		exit(-1); //exec shouldn't return, if it does, exit.
	}
	else
	{
		//Parent process
//		printJob(pid);
		if(bg == 1)
		{
			printJob(pid);
			return;
		}
		else
		{
			fpid = fgpid(jobs);
			waitfg(fpid);
		}
			
	}

}

void getIOfile(char** argv)
{
	in = -1;
	out = -1;

	int i = 0;
	while(argv[i] != NULL)
	{
		if(strcmp(argv[i],"<") == 0)
		{
			in = 1;
		}
		else if(strcmp(argv[i],">") == 0)
		{
			out = 1;
		}
		if(in == 1 || out == 1)
		{
			if(argv[i+1] != NULL)
			{
				IOfileIndex = i+1;
				IOredirFile = malloc(sizeof argv[i+1]);
				IOredirFile = argv[i+1];
			}
		}
		i++;
	}
	return;
}

void launch2(char** argv)
{
	int i = 0;
	pid_t pid;
	while(argv[i] != NULL)
	{

		if(strcmp(argv[i], "<") == 0)
		{
			if((pid = fork()) == 0)
			{
				int inFD = open(argv[i+1], O_RDONLY);
				dup2(inFD,STDIN_FILENO);
				argv[i] = NULL;
				close(inFD);
//				execvp(argv[0], argv);
				launch2(argv);
			}
		}
		else if(strcmp(argv[i], ">") == 0)
		{
			if((pid = fork()) == 0)
			{
				int outFD = creat(argv[i+1], 0644);
				dup2(outFD, STDOUT_FILENO);
				argv[i] = NULL;
				close(outFD);
//				execvp(argv[0],argv);
				launch2(argv);
			}
		}
		else if(strcmp(argv[i], "|") == 0)
		{
			int fd[2];
			pipe(fd);
			argv[i] = NULL;
			if ((pid = fork()) == 0) {
				dup2(fd[PIPE_WRITE], 1);
				close(fd[PIPE_READ]);
				close(fd[PIPE_WRITE]);
		//		argv[i] = NULL;
				//execvp(argv[0],argv);            
				launch2(argv);

			}
			if ((pid = fork()) == 0) {
				dup2(fd[PIPE_READ], 0);
				close(fd[PIPE_READ]);
				close(fd[PIPE_WRITE]);
				//execvp(argv[i+1], &argv[i+1]);            
				launch2(&argv[i+1]);

			}
			close(fd[0]);
 	      close(fd[1]);
	      wait(NULL);
	      wait(NULL);
		}
		i++;
	}
	if(pid == 0)
	{
		if(fork() == 0)
		{
			execvp(argv[0], argv);
		}
	}
}

void parseParsed(char** argv)
{
	int i = 0;
	while(argv[i] != NULL)
	{
		 if(strcmp(argv[i], "|") == 0)
		 {
		 	pipeCount++;
			pipeIndex = i;
			argv[i] = NULL;
		 }
		 i++;
	}	
	return;
}

void getParsedLength(char** argv)
{
	parsedLength = 0;
	int i = 0;
	while(argv[i] != NULL)
	{
		parsedLength++;
		i++;	
	}
	return;
}

void launch3(char** argv)
{
	int i = 0;
	int piped = 0;
	pid_t pid;
	int fd[2];
	int start = 0;
	fd[0] = -1;
	fd[1] = -1;
	while(i < parsedLength)
	{
			if(pipeCount > 0)
			{
				pipe(fd);
				piped = 1;
				pipeCount--;
			}
			else
			{
				fd[0] = -1;
				fd[1] = -1;
			}

		while(argv[i] != NULL)
		{
			if(strcmp(argv[i],"<") == 0)
			{
				int j = i;
				argv[i] = NULL;
				start = i;
				i += 2;
				if((pid = fork()) == 0)
				{
					if(piped != 1)
					{
						fd[0] = creat(argv[j+1], 0644);
						dup2(fd[0], STDIN_FILENO);
						argv[j] = NULL;
						close(fd[0]);
						execvp(argv[start],argv);
					}
					else
					{
						dup2(fd[0], STDIN_FILENO);
						argv[j] = NULL;
						piped = 0;
						execvp(argv[start], argv);
					}
				}
				start = i + 1;
			}
			else if(strcmp(argv[i], ">") == 0)
			{
				int j = i;
				argv[i] = NULL;
				i += 2;
				if((pid = fork()) == 0)
				{
					if(piped != 1)
					{
						fd[1] = creat(argv[j+1], 0644);
						dup2(fd[1], STDOUT_FILENO);
						argv[j] = NULL;
						close(fd[1]);
						int k = 0;
						while(argv[start+k] != NULL)
						{
							fputs(argv[start+k],stdout);
							printf("\n");
							k++;
						}
						execvp(argv[start],argv);
					}
					else
					{
						dup2(fd[1], STDOUT_FILENO);
						argv[j] = NULL;
						piped = 0;
						execvp(argv[start], argv);
					}
	
				}
				start = i + 1;
			}
			i++;
		}
		if((pid = fork()) == 0)
		{
			printf("end ex\n");
			execvp(argv[start], argv);
		}
	}
	return;
}

void launch4(char** argv)
{	
	int i = 0;
	pid_t pid;
	int fd[2];
	pipe(fd);
	int start = 0;
	start = pipeIndex+1;

	i = start;
	while(argv[i] != NULL)
	{
		if(strcmp(argv[i], ">") == 0)
		{
			printf("found it\n");
			argv[i] = NULL;

			if((pid = fork()) == 0)
			{
				close(fd[0]);
				dup2(fd[1],1);
				
	//			argv[i] = NULL;
				execvp(argv[0], argv);
			}
			if((pid = fork()) == 0)
			{
				int outFD = creat(argv[i+1], 0664);
		   	dup2(fd[0],0);
				close(fd[1]);
				dup2(outFD,1);
				close(outFD);

//	//			argv[i] = NULL;
				execvp(argv[start], &argv[start]);
			
			}
			close(fd[1]);
			close(fd[0]);

		}
		
		i++;
	}
	return;
}
