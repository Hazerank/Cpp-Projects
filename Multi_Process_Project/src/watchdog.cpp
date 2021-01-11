/**
 * @file watchdog.cpp
 * @author Hazar Cakir
 * @brief The source code for Watchdog.
 * @version 1
 * @date 2021-01-02
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include <iostream>
#include <string>
#include <unistd.h>
#include <csignal>
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <fcntl.h>
#include <map>
#include <sys/wait.h>
#include <bits/stdc++.h>


using namespace std;
/**
 * The number of the processes that will be created.
 * 
 */
int processNum;
/**
 * The value of in-line argument path of the output process file.
 * 
 */
string process_output; 
/**
 * The value of in-line argument path of the output watchdog file.
 * 
 */
string watchdog_output;
/**
 * Global ofstream variable to store ofstream object globally.
 * 
 */
ofstream result;

/**
 * 0.3 time interval
 * 
 */
struct timespec delta = {0 /*secs*/, 300000000 /*nanosecs*/};

/**
 * The array that stores the pid values of the children as indexed sam as in executor.
 * 
 */
pid_t *pidList;

/**
 * The varible that stores pipe variable globally
 * 
 */
int unnamedPipe;


/**
 * @brief It handles the exit situation.
 * 
 * When watchdog received SIGTERM, it is catched from this function and exterminating watchdog. Pipe is also closing here.
 * 
 */
void exitHandle(int code){
    sleep(1);
    result.open(watchdog_output,ios::app);
    result << "Watchdog is terminating gracefully" << endl;
    result.close();
    close(unnamedPipe);
    exit(code);
}

/**
 * @brief Creates all processses.
 * 
 * Creates all processes via forking and for every child, exec is called. Parrent process which is watchdog is keeping pid of the children in a pid_t array namely pidList.
 * 
 * @param unnamedPipe The pipe to send the data about newly created processes.
 * @param pidList An array which stores information about children processes.
 */
void createAll(int unnamedPipe, pid_t pidList[]){
     for (int i=1; i<=processNum; i++) {
        
       
        pid_t childpid = fork();
        nanosleep(&delta, &delta); 
        if( childpid == -1){
            perror("Fork Error!!");
        }
        if(childpid == 0){
            string name = "P" + to_string(i);
            result.open(watchdog_output,ios::app);
            result << name << " is started and it has a pid of " << getpid() << endl;
            result.close();
            execl("./process","./process", name.c_str(), process_output.c_str(), (char *) 0);

        }
        else{
            pidList[i] = childpid;
        }
        
    }

    
    for (int i=1; i<=processNum; i++) {
        string temp = "P" + to_string(i) + " " + to_string(pidList[i]);
        write(unnamedPipe, temp.c_str(), 30);
    }

}

/**
 * @brief Handles the situation of a death child.
 * 
 * This function is awaked when a child is killed by executor.
 * Watchdog can determine which child is death by using waitpid() function.
 * It will be waiting till the get the terminated process' id and stores it in the p value.
 * It traverses pidList array to determine killed child's name and then creates a new process with that name.
 * It pushes the information about recreated process to the pipe.
 * 
 * When P1 is terminated, there are some specific series of operations are done. The signalProcess() functions send SIGTERM to all other processes.
 * Thereafter calls createAll() function to create all processes again.
 * 
 * It writes corresponding messages to the corresponding file. 
 * 
 * @param code This code value will be SIGTERM
 */
void signalProcess( int code ) {
   
    pid_t p;
    int status;

    while ((p = waitpid(-1, &status, WNOHANG)) >= 0){
        int i;
        bool find = false;
        for( i = 1;  i <= processNum ; i++){
            if(pidList[i] == p){
                find = true;
                break;
            }
        }
        if(find){
            
            result.open(watchdog_output,ios::app);
            if(i == 1){
                result << "P1 is killed, all processes must be killed" << endl;
                result << "Restarting all processes" << endl;
                result.close();
                for(int k = 2; k <= processNum; k++){
                    kill(pidList[k],SIGTERM);
                    nanosleep(&delta, &delta);  // Deal with writing delays
                    
                }
                
                createAll(unnamedPipe, pidList);
                break;
            }
            result << "P" << i << " is killed" << endl;
            result << "Restarting P" << i << endl;
            result.close();

            pid_t childpid = fork();
            if( childpid == -1){
                perror("Fork Error!!");
            }
            if(childpid == 0){
                
                result.open(watchdog_output,ios::app);
                string name = "P" + to_string(i);
                result <<  name << " is started and it has a pid of " << getpid() << endl;
                result.close();
                execl("./process","./process", name.c_str(), process_output.c_str(), (char *) 0);
                
            }
            else{
                pidList[i] = childpid;
                string temp = "P" + to_string(i) + " " + to_string(pidList[i]);

                write(unnamedPipe, temp.c_str(), 30);
            }
            break;
        }
    }

}


/**
 * @brief Main of the watchdog.
 * 
 * After connecting signals to the signal handler functions, it takes console arguments and initializes the basic variables.
 * It firstly creates output files, if exists, clears it.
 * Then pipe is created in write mode.
 * 
 * Then watchdog writes its information as P0 to the pipe and to the pidList().
 * Then main calls the createAll() function to create all children processes.
 * 
 */
int main(int argc, char *argv[]) {
    

    signal(SIGCHLD, signalProcess);
    signal(SIGTERM, exitHandle);

    processNum = stoi(argv[1]) ;
    process_output = argv[2];
    watchdog_output = argv[3];
    pidList = new pid_t[processNum+1]; // Keep PID of watchdog at 0, PID of P1 at 1, ...


    result.open(watchdog_output,ios::trunc);
    result.close();
    result.open(process_output,ios::trunc);
    result.close();
    




    /*
    * Create the named file (FIFO) under /tmp/myfifo directory.
    * executor only reads the newly written PIDs from the pipe.
    */
    char * myfifo = (char*) "/tmp/myfifo";
    mkfifo(myfifo, 0644);
    unnamedPipe = open(myfifo, O_WRONLY);


    pidList[0] = getpid();
    string temp = "P0 " + to_string(getpid());
    write(unnamedPipe, temp.c_str(), 30);

    createAll(unnamedPipe, pidList);


    while(1){
        sleep(0.3);
    }

    return 0;
}