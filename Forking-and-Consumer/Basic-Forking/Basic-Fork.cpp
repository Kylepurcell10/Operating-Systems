#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
using namespace std;
//This program will repeatedly make children based on the argument passed through in the
function call.
 int main(int argc ,char *argv[] ){
 pid_t pid; // Parent and child PID initialization
 pid_t childpid;
 while(true){ // While Loop that continously forks new processes
 pid = fork(); // Forks child and parent processes
 if(pid < 0){ // Fork cannot occur, exit program
 cout << "No more room for new processes";
 exit(-1);
} 
 else if (pid == 0){ // child process
 childpid = getpid(); // get PID and print it
 cout << childpid << endl;
 sleep(1);
}
 else{ //parent Process
 wait(NULL); // wait until child is dead to end program.
 exit(0);
}
}
 return 0;
}