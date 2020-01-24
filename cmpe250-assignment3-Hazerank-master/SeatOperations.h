#ifndef CMPE250_ASSIGNMENT3_SEATOPERATIONS_H
#define CMPE250_ASSIGNMENT3_SEATOPERATIONS_H
#include <iostream>
#include <vector>
#include <fstream>

using namespace std;

struct Person{
    int type=0;
    int line=0, seatNumber=0;
    int type3LastOperation=1;
    string ticketInfo = "";
};

class SeatOperations{

private:
    vector<Person> lines[2];
    int N, M;

public:

    SeatOperations(int N, int M); // Constructor
    void addNewPerson(int personType, const string& ticketInfo); // Adds new person.
    void printAllSeats(ofstream& outFile); // Prints all seats.
    void move(Person &person); // Move a person to where s/he will sit.
    friend ostream &operator<< (ostream &out, const Person &person); // Overriding of "<".
    Person sit(Person put, int line, int pos); // Sits person and returns the sitting one.
};

#endif //CMPE250_ASSIGNMENT3_SEATOPERATIONS_H
