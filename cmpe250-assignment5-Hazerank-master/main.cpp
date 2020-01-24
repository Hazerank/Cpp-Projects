/*
Student Name: Hazar Çakır
Student Number: 2017400093
Project Number: 5
Compile Status: [SUCCESS/FAIL]
Running Status: [SUCCESS/FAIL]
Notes: Anything you want to say about your code that will be helpful in the grading process.
*/

#include <iostream>
#include <algorithm>
#include <fstream>

using namespace std;


int main(int argc, char* argv[]){

    ifstream inFile(argv[1]);
    ofstream outFile(argv[2]);


    int chocoNum, limit; // These are the given chocolate num and daily bought limit


    // Geting and storing data from file
    inFile >> chocoNum >> limit;
    int chocos[chocoNum];

    long long int memory[chocoNum];

    for(int i = 0; i < chocoNum; i++){
        inFile >> chocos[i];
    }

    // Sorting data
    sort(chocos,chocos + chocoNum);

    // Calculating and solving problem.
    long long int sum = 0;

    for(int i = 0; i < chocoNum ; i++){
        if(i < limit) {
            sum += chocos[i];
            memory[i] = chocos[i];
        }
        else{
            long long int move = chocos[i] + memory[i-limit];
            sum += move;
            memory[i] = move;
        }
        outFile << sum << " " ;
    }

    return 0;
}