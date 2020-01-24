/*
Student Name: Hazar Çakır
Student Number: 2017400093
Project Number: 3
Compile Status: [SUCCESS/FAIL]
Running Status: [SUCCESS/FAIL]
Notes: Anything you want to say about your code that will be helpful in the grading process.
*/
#include "SeatOperations.h"

using namespace std;

ostream &operator<<(ostream &out, const Person &person);
/**
 * The constructor of the project.
 * @param N Size of the first list.
 * @param M Size of the second list.
 */
SeatOperations::SeatOperations(int N, int M){
    this->N = N;
    this->M = M;
    this->lines[0].resize(N);
    this->lines[1].resize(M);
}
/**
 * Adds new person to the corresponding list.
 * @param personType Type of the person.
 * @param ticketInfo Ticket info of person.
 */
void SeatOperations::addNewPerson(int personType, const string& ticketInfo){
    //Creating regarding person struct.
    Person person;
    person.type = personType;
    person.line = ticketInfo.at(0) == 'A' ? 0 : 1;
    person.seatNumber = stoi(ticketInfo.substr(1)) - 1 ; // I keep the position at array.
    person.ticketInfo = ticketInfo;

    // If the seat is empty already, makes s/he sit
    int seatNum = (person.line == 0) ? person.seatNumber % N: person.seatNumber % M;
    if(lines[person.line][ seatNum ].type == 0 )
        lines[person.line][seatNum ] = person;

    // If the seat is occupied, sits even so, but the person who sits gets up and move with "move" method.
    else{
        Person getUp = sit(person , person.line, seatNum);
        move(getUp);
    }
}

/**
 * It moves person to where s/he sits.
 * Works recursevely till reach an empty seat.
 * It makes all move actions.
 *
 * @param person Who is going to move.
 */
void SeatOperations::move(Person &person){
    Person getUp; // The person gets up.
    int newSeatNum; // New seat number of person.
    switch (person.type){
        // Base Case.
        case 0:{
            return;
        }
        // If person is type 1 person.
        case 1: {
            newSeatNum = (person.line == 0) ? person.seatNumber % M : person.seatNumber % N;
            person.line = 1 - person.line; // Changes person's line.
            getUp = sit(person, person.line, newSeatNum); // Make the person sit.
            move(getUp); // Moving the before sitting person
            break;
        }
        // If person is type 2 person.
        case 2: {
            newSeatNum = ++person.seatNumber;
            // The case when the person at the end of the line, changes line.
            if((newSeatNum) % lines[person.line].size() == 0 ){
                person.line = 1 - person.line; // Changes person's line.
                person.seatNumber = 0; // Resetting person's seat number.
                getUp = sit(person, person.line, 0);
                move(getUp);
            }
            else{
                int seatNum = (person.line == 0) ? newSeatNum % N: newSeatNum % M;
                getUp = sit(person, person.line, seatNum);
                move(getUp);
            }
            break;
        }
        // If person is type 3 person.
        case 3: {
           int moveBy = person.type3LastOperation * person.type3LastOperation; // How many move should have done.
           int size = person.line == 0 ? N : M;
           int remaining = size  - (person.seatNumber % size + 1);
           // The case when the person can stay in the current line.
           if(moveBy <= remaining ){
               person.type3LastOperation++;
               getUp = sit(person, person.line, (person.seatNumber + moveBy ) % size);
               move(getUp);

               break;
           }
           moveBy -= remaining;
           while(1){
               // The case when person moves to other line.
               if(moveBy <= lines[1 - person.line].size()){
                   person.type3LastOperation++;
                   getUp = sit(person, 1 - person.line, moveBy - 1);
                   move(getUp);
                   break;
               }
               moveBy -= lines[1 - person.line].size();
               // The case when person makes a ring and return its own line.
               if(moveBy <= lines[person.line].size()){
                   person.type3LastOperation++;
                   getUp = sit(person, person.line, moveBy - 1);
                   move(getUp);
                   break;
               }
               moveBy -= lines[person.line].size();
           }
            break;
        }
    }
}
/**
 * Make person sit in the given position, keeps the man already sits there.
 * @param put Person who will sit.
 * @param line Which line will the person sits.
 * @param pos Which position will the person sits.
 * @return Person who gets up.
 */
Person SeatOperations::sit(Person put, int line, int pos){
    Person result;
    result = lines[line][pos ];
    lines[line][pos ] = put;
    return result;
}
/**
 * Prints all seats.
 * @param outFile Outstream object.
 */
void SeatOperations::printAllSeats(ofstream& outFile){

    int i = 0;
    for(; i< N ;i++){
        outFile << this->lines[0].at(i) << endl;
    }
    for(i = 0; i < M ;i++){
        outFile << this->lines[1].at(i) << endl;
    }
}

/**
 * Overriding the "<" sign to "cout" person.
 * @param out Outstream object
 * @param person Person who will be streamed.
 * @return return result.
 */
ostream &operator<<(ostream &out, const Person &person) {
    out << person.type;
    if(person.type == 0)
        return out;
    else
        out << " " << person.ticketInfo;
    return out;
}