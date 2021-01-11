/**
 * @file process.cpp
 * @author Hazar Cakir
 * @brief The source code for processes.
 * @version 1.0
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
#include <bits/stdc++.h>


using namespace std;
/**
 * The value of in-line argument name like "P1"
 * 
 */
string name;
/**
 * The value of in-line argument path of the output file.
 * 
 */
string process_output;

/**
 * Global ofstream variable to store ofstream object globally.
 * 
 */
ofstream result;



/**
 * @brief Handles the signal process
 * 
 * It is awaked when a signal is catched by a process instance. All possible signals are connected with this specific function.
 * It determines whether the signal is 15 (SIGTERM) and handles accordingly. It prints to the file necessary messages. In case of SIGTERM, it ends the process with exit() method.  
 * 
 * @param code The value of received signal.
 */
void signalProcess( int code ) {
    result.open(process_output,ios::app);
    if( code == 15){
        result << name << " received signal " << code << ", terminating gracefully" << endl;
        result.close();
        exit(code);
    }

    result << name << " received signal " << code << endl;
    result.close();
    
    return;  
}

/**
 * @brief Takes in-line arguments and process. Connects signals to handler and after writing creation message, sleeps for next signals.
 * 
 * The main function of a process. It processes the arguments which are the name of the process and the path of the output file. Then after connecting signals to the signalProcess() function
 * opens the corresponding file and announce that creation was succesfully done. It starts to sleep thereafter until next signal. 
 *   
 */
int main(int argc, char *argv[]) {

    name = argv[1] ;
    process_output = argv[2];
    
    /**
     * Connecting all required signals to the signalProcess function.
     * 
     */
    signal(SIGINT, signalProcess);  
    signal(SIGHUP, signalProcess);  
    signal(SIGILL, signalProcess);  
    signal(SIGTRAP, signalProcess);  
    signal(SIGBUS, signalProcess);  
    signal(SIGFPE, signalProcess);  
    signal(SIGSEGV, signalProcess);  
    signal(SIGXCPU, signalProcess);  
    signal(SIGTERM, signalProcess);  

    
    /**
     * Announcing the succesful creation of a process
     * 
     */
    result.open(process_output,ios::app);
    result << name << " is waiting for a signal" << endl ;
    result.close();

    /**
     * Process sleeps until a new signal is received.
     * 
     */
    while(1) {
        sleep(0.1);
    }

    return 0;
}