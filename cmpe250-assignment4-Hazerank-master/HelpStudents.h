#ifndef CMPE250_ASSIGNMENT3_HELPSTUDENTS_H
#define CMPE250_ASSIGNMENT3_HELPSTUDENTS_H
#include <vector>
#include <iostream>
#include <fstream>
#include <list>
#include <iterator>
#include <queue>
#include<limits>
#include <set>

using namespace std;
const long long int INF = numeric_limits<long long int >::max();


class HelpStudents{

public:
    HelpStudents(int  N, int  M, int K, vector < pair< pair <int,int> , int > > ways);
    long long int firstStudent();
    long long int secondStudent();
    long long int thirdStudent();
    long long int fourthStudent();
    long long int fifthStudent();
    ~HelpStudents();

    // YOU CAN ADD YOUR HELPER FUNCTIONS AND MEMBER FIELDS
private:
    int summit;
    int vertices;
    int edges;
    //  AdjacentList Form
    priority_queue<pair<int,int>> *adjacencyListPrior; //list in the form of (weight, toWhere)
    void fillList(vector < pair< pair <int,int> , int > > ways);

    struct PairComparator{
        bool operator () ( const pair<int,int> & left , const pair<int,int> & right){
            return(left.first < right.first ||
            (left.first == right.first && left.second <right.second ));
        }
    };
    set<pair<int,int>,PairComparator> *adjacencyList;
};

#endif //CMPE250_ASSIGNMENT3_HELPSTUDENTS_H
