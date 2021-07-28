#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
// This program repeatedly creates children and has them call a programB and then exit
 int main(int argc ,char *argv[], char *envp[] ){
 int count = 0;
 pid_t pid; // declaration of child PID
 pid_t childpid;
 count = atoi(argv[1]); // convert character number to usable integer
 for(int i = count ; i > 0 ; i--){
 pid = fork(); // fork
 if(pid < 0){ // fork failed and program is over.
 printf( "No more room for new Processes");
 exit(-1);
}
 else if(pid == 0){ // child process
 char* const argv[] = {"programB" , NULL}; // holds the name of the program to be
executed
 char* envp[] = { NULL }; // envirenment is empty
 childpid = getpid(); // gets the PID of the child and prints it
 printf( "%d\n", childpid); 
 execve("./programB.out", argv, envp); // each child calls the executable of
programB
 perror("execve"); // end if EXECVE fails
 exit(0);
}
 else{ //parent Process
 wait(NULL); // wait until children are dead
}
}
 return 0;
} 
