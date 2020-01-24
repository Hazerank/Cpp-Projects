/*
Student Name: Hazar Çakır
Student Number: 2017400093
Project Number: 4
Compile Status: [SUCCESS/FAIL]
Running Status: [SUCCESS/FAIL]
Notes: Anything you want to say about your code that will be helpful in the grading process.
*/
#include <set>
#include "HelpStudents.h"

using namespace std;

HelpStudents::HelpStudents(int  N, int  M, int K, vector < pair< pair <int,int> , int > > ways) {
    summit = K -1 ;
    vertices = N;
    edges = M;
    //adjacentList = new priority_queue <Edge*> [N];
    //adjacencyListPrior = new priority_queue <pair<int,int>> [N];
    adjacencyList = new set< pair<int,int>,PairComparator > [N];
    fillList(ways);
    //  Matrix Form
    //graph = new vector<vector<int>>(N,vector<int>(N));
    //fillGraph(ways);
}

void HelpStudents::fillList(vector < pair< pair <int,int> , int > > ways){

    for(auto &data : ways ){
        adjacencyList[data.first.first-1].insert(make_pair(data.second,data.first.second-1));
        adjacencyList[data.first.second-1].insert(make_pair(data.second,data.first.first-1));
    }

}

HelpStudents::~HelpStudents() {
}

long long int HelpStudents::firstStudent() {
    vector<long long int> * road = new vector<long long int> (vertices,INF);
    set <pair <long long int , int > > * toGo = new set<pair<long long int,int>> ;
    road->at(0) = 0;

    long long int result = 0;
    toGo->insert(make_pair(0,0));

    while(!toGo->empty()){
        pair<int,int> curr = *toGo->begin();
        toGo->erase(toGo->begin());
        if(curr.second == summit) {
            return road->at(curr.second);
        }

        for(auto & adj : adjacencyList[curr.second]){
            if(road->at(adj.second) > adj.first + road->at(curr.second)){
                if(road->at(adj.second) != INF){
                    toGo->erase(toGo->find(make_pair(road->at(adj.second),adj.second)));
                }
                road->at(adj.second) = adj.first + road->at(curr.second);
                toGo->insert(make_pair(road->at(adj.second),adj.second));
            }
        }

    }

}
long long int HelpStudents::secondStudent() {
    vector<long long int> * road = new vector<long long int> (vertices,INF);
    set <pair <int , int > > * toGo = new set<pair<int,int>> ;
    vector<int> * father = new vector<int> (vertices,0);
    vector<bool> * visited = new vector<bool> (vertices, false);
    road->at(0) = 0;

    long long int result = 0;
    toGo->insert(make_pair(0,0));

    while(!toGo->empty()){
        pair<int,int> curr = *toGo->begin();
        visited->at(curr.second) = 1;
        toGo->erase(toGo->begin());
        if(curr.second == summit) break;

        for(auto & adj : adjacencyList[curr.second]){
            if(visited->at(adj.second) == 1)
                continue;
            if(road->at(adj.second) > adj.first ){
                if(road->at(adj.second) != INF){
                    toGo->erase(toGo->find(make_pair(road->at(adj.second),adj.second)));
                }
                road->at(adj.second) = adj.first;
                toGo->insert(make_pair(adj.first,adj.second));
                father->at(adj.second) = curr.second;
            }
        }

    }
    int i = summit;

    while(i != 0){
        if(result < road->at(i))
            result = road->at(i);
        i = father->at(i);
    }
    return result;
}

long long int HelpStudents::thirdStudent() {
    long long int result = 0;
    queue<pair<int,int>> * process  = new queue<pair<int,int>>;
    vector<bool> * visited = new vector<bool> (vertices, false);
    visited->at(0) = 1;
    process->push(make_pair(0,0));
    while(!process->empty()){
        pair<int,int> node = process->front();
        process->pop();
        if(node.second == summit){
            result += node.first;
            return result;
        }
        for(auto & adj : adjacencyList[node.second]){
            if(visited->at(adj.second) == false) {
                process->push(make_pair(node.first + 1, adj.second));
                visited->at(adj.second) = true;
            }
        }

    }
}
long long int HelpStudents::fourthStudent() {
    vector<vector<int>> * visited = new vector<vector<int>>(vertices, vector<int>(vertices,0));
    pair<int,int> road = make_pair(0,0);
    long long int result = 0;

    while(road.second != summit){
        if((adjacencyList[road.second]).empty()) return -1;
        while((*visited)[((*adjacencyList[road.second].begin()).second)][road.second] == 1){
            adjacencyList[road.second].erase(adjacencyList[road.second].begin());
        }
        if((adjacencyList[road.second]).empty()) return -1;

        (*visited)[(*adjacencyList[road.second].begin()).second][road.second] =
        (*visited)[road.second][(*adjacencyList[road.second].begin()).second] = 1;
        road = *adjacencyList[road.second].begin();
        result += road.first;
    }
    return result;
}
long long int HelpStudents::fifthStudent() {
    vector<long long int> * road = new vector<long long int> (vertices,INF);
    set <pair <long long int , int > > * toGo = new set<pair<long long int,int>> ;
    road->at(0) = 0;

    long long int result = 0;
    toGo->insert(make_pair(0,0));

    while(!toGo->empty()){
        pair<int,int> curr = *toGo->begin();
        toGo->erase(toGo->begin());
        if(curr.second == summit) {
            return road->at(curr.second);
        }
        long long int newMin = (*adjacencyList[curr.second].begin()).first * 2 + road->at(curr.second);

        for(auto & adj : adjacencyList[curr.second]){
            if(road->at(adj.second) > newMin){
                if(road->at(adj.second) != INF){
                    toGo->erase(toGo->find(make_pair(road->at(adj.second),adj.second)));
                }
                road->at(adj.second) = newMin;
                toGo->insert(make_pair(newMin, adj.second ) );
            }
        }

    }
}

// YOU CAN ADD YOUR HELPER FUNCTIONS