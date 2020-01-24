/*
Student Name: Hazar CAKIR
Student Number: 2017400093
Project Number: 2
Compile Status: [SUCCESS/FAIL]
Running Status: [SUCCESS/FAIL]
Notes: Anything you want to say about your code that will be helpful in the grading process.
*/
#include "RestaurantOrganizer.h"

using namespace std;

RestaurantOrganizer::RestaurantOrganizer(const vector<int>& tableCapacityInput){
    numberOfTables = tableCapacityInput.size();

    for(int i=0;i<numberOfTables;i++){
        tableCapacity[i] = tableCapacityInput[i];
        heap[i] = i;
        heapUp(i);
    }
}

void swap(int index1,int index2,RestaurantOrganizer & the); //Predefinition of function
int pop(RestaurantOrganizer & the); //Predefinition of function


/**
 * Controls that whether there is suitable table for group.
 * If it is, adds group as decreasing tablecapacity of corresponding table
 * Prints to file and heapdown the table in order to keep heap ordered.
 *
 * @param groupSize Group size of the incoming group
 * @param outFile Outstream belongs to output file
 */
void RestaurantOrganizer::addNewGroup(int groupSize, ofstream& outFile){
    if(groupSize > tableCapacity[0]) outFile << "-1" << endl;
    else{
        tableCapacity[0] -= groupSize;
        outFile << heap[0] << endl;
        heapDown(0);
    }
}
/**
 * Adds an element to the heap.
 * Controls parent of the added element if it is smaller or not.
 * Works recursevely.
 *
 * @param index Index of the table which will heapen up.
 */
void RestaurantOrganizer::heapUp(int index){
    if(index == 0)
        return;
    int up = (index-1)/2; // Index of the parent.
    if(tableCapacity[up] < tableCapacity[index]) {
        swap(index, up, *this);
        heapUp(up);
    }
}
/**
 * Controls index element and its children.
 * If needed, send index element to the down in the hype.
 * Works recursevely.
 * All comparisons are done with specific comparison values which I generate with
 * tableCapacity and HeapIndex.
 * These values eliminate equality situations and makes comparison much easier.
 *
 * @param index Index of the table which will heapen down.
 */
void RestaurantOrganizer::heapDown(int index){
    int leftIn = 2* index + 1; // Index of the left child.
    int rightIn = 2 * index + 2; // Index of the right child.
    double n = 0.00001; // The compare factor in order to calculate compare values of cells.
    double compLeft, compRight,compCurr; // The compare values of cells. All comparisons will done with these values.
    compCurr = tableCapacity[index] - (n * heap[index]); // specific comparison value of current element.

    if(numberOfTables - 1 < (leftIn)) return; // base statement of recursion, with no child.

    compLeft = tableCapacity[leftIn] - (n * heap[leftIn]) ; // specific comparison value of left children.

    if(numberOfTables - 1 == (leftIn)) { // base statement of recursion, with just left child.
        if (compLeft > compCurr)
            swap(leftIn, index, *this);
    }
    else {
        compRight = tableCapacity[rightIn] - (n * heap[rightIn]); // specific comparison value of right children.

        //Comparison and swap part of function.
        if(compCurr < compLeft || compCurr <  compRight){
            if(compLeft > compRight) {
                swap(leftIn,index,*this);
                heapDown(leftIn);
            }
            else {
                swap(rightIn,index,*this);
                heapDown(rightIn);
            }
        }
    }
}
/**
 * Prints elements ordered from bigger capacity to lower.
 * It pops 0'th element of the heap always for n times.
 * Uses pop helping function.
 *
 * @param outFile Outstream belongs to output file
 */
void RestaurantOrganizer::printSorted(ofstream& outFile){
    for(int i = 0; i < numberOfTables ; i++)
        outFile << pop(*this) << " ";
}
/**
 * Swaps given indices' elements in both heap array and tableCapacity array.
 *
 * @param index1 First index that needed to be swapped.
 * @param index2 Second index that needed to be swapped.
 * @param the The RestaurantOrganizer object's itself, in order to reach object's arrays.
 */
void swap(int index1,int index2,RestaurantOrganizer & the){
    int temp = the.heap[index1];
    the.heap[index1] = the.heap[index2];
    the.heap[index2] = temp;

    temp = the.tableCapacity[index1];
    the.tableCapacity[index1] = the.tableCapacity[index2];
    the.tableCapacity[index2] = temp;
}
/**
 * Pops the 0'th element of the heap.
 * Changes element's capacity and heapDown it in order to delete it.
 *
 * @param the The RestaurantOrganizer object's itself, in order to reach object's arrays.
 * @return Regarding tableCapacity value of popped element.
 */
int pop(RestaurantOrganizer & the){
    int retVal = the.tableCapacity[0];
    the.tableCapacity[0] = 0;
    the.heapDown(0);
    return retVal;
}
