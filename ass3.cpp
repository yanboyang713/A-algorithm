/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   Student ’s Name : Boyang YAN
   Student ’s email address : by932@uowmail.edu.au
   Student user name : by932
   Assignment : Assignment three
   About : single source - single destination shortest path problem
   Last Modification : 29/10/2017 9:08
   - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#include <iostream>
#include <fstream>
#include <cmath>
#define INFINITY 99999.0
using namespace std;
struct Vertex//vertex data structure
{
    int vertexNumber;//the index, or the number of the node
    char label;//vertex label
    double x;//X-coordinate
    double y;//Y-coordinate
};


struct Graph
{
    Vertex * vertexSet;//an array that stores nodes in the graph
    double ** arcs;//adjacent matrix
    int vertexAmount;//number of vertices
    int edgeAmount;//number of edge

};
struct Stack
{
    int * vertex;
    int top;
};
struct shortestPath
{
    char *path;
    int numberOfEdge;
    int numberOfadditionNode;
    int lengthOfTheShortestPath;
};

double getDistanceBetweenTwoNodes(Vertex &, Vertex &);
int findMin(double [], double &, bool []);//get the index of minimum value in a double-type array. min is an output variable, which tells you the smallest value
void caluShortestPath(int [], int, shortestPath &);//iteration here is just for display, is it the first or second round of getting the shortest path
void enStack(Stack &, int);
void readAndInit(ifstream&);
void Astar(bool [], int [], double [], int &);
void printShortestInfo(shortestPath &);
void siftUp(int, shortestPath *);
int parent(int);
void swap(int, int, shortestPath *);

Graph graph;
int sourceVertex = -1;//read the starting node
int destinationVertex = -1;//read the ending node

int main(int argc, char *argv[])
{
    graph.vertexAmount = 0;//initialize the graph vertex Amount
    graph.edgeAmount = 0;//initialize the graph edge Amount

    char fileName [20];
    cout << "Please input file name for open : ";
    cin >> fileName;
    // open file
    ifstream file (fileName);
    if ( file.is_open() )
        readAndInit(file);

    bool * selectedSet = new bool[graph.vertexAmount];
    int * path = new int [graph.vertexAmount];
    double * distanceF = new double [graph.vertexAmount];
    int numberOfSelectedVertex = 0;
    //the shortest Path
    Astar(selectedSet, path, distanceF, numberOfSelectedVertex);

    shortestPath shortestPathInfo;
    caluShortestPath(path, numberOfSelectedVertex, shortestPathInfo);
    //print the first shortest Path
    cout << "The shortest Path: ";
    printShortestInfo(shortestPathInfo);
    cout << endl << endl;

    //secondShortestPath
    shortestPath * secondShortestPath;//for management which one is the smallest path represent by heap, the smallest one on the top
    int edgeWeight;//temp story edge Weight for after insert back
    secondShortestPath = new shortestPath[shortestPathInfo.numberOfEdge];

    for (int i = 0; i < shortestPathInfo.numberOfEdge; ++i) {
        //backup old edgeWeight
        edgeWeight = graph.arcs[static_cast<int>(shortestPathInfo.path[i])-97][static_cast<int>(shortestPathInfo.path[i+1])- 97];
        //delete edge
        graph.arcs[static_cast<int>(shortestPathInfo.path[i+1])-97][static_cast<int>(shortestPathInfo.path[i]) - 97] = INFINITY;
        //find the shortest Path
        Astar(selectedSet, path, distanceF, numberOfSelectedVertex);
        //insert to heap
        caluShortestPath(path, numberOfSelectedVertex, secondShortestPath[i]);
        //siftUp
        siftUp(i, secondShortestPath);
        //put edge Weight back
        graph.arcs[static_cast<int>(shortestPathInfo.path[i])-97][static_cast<int>(shortestPathInfo.path[i+1])- 97] = edgeWeight;

    }
    cout << "The second shortest Path: ";
    printShortestInfo(secondShortestPath[0]);

    //free memory
    for(int i = 0; i < graph.vertexAmount; ++i) {
        delete [] graph.arcs[i];
    }
    delete [] graph.arcs;

    delete [] graph.vertexSet;

    for (int i = 0; i < shortestPathInfo.numberOfEdge; i++)
        delete [] secondShortestPath[i].path;

    delete [] secondShortestPath;

    delete [] shortestPathInfo.path;

    delete [] selectedSet;

    delete [] path;

    delete [] distanceF;

    return 0;
}
void Astar(bool selectedSet[], int path[], double distanceF[], int & numberOfSelectedVertex){
    numberOfSelectedVertex = 0;
    for(int i = 0;i < graph.vertexAmount; ++i){
        selectedSet[i] = false;
        path[i] = sourceVertex;
        distanceF[i] = graph.arcs[sourceVertex][i] + getDistanceBetweenTwoNodes(graph.vertexSet[i], graph.vertexSet[destinationVertex]);//for implimentation of A*
    }
    selectedSet[sourceVertex] = true;
    numberOfSelectedVertex++;

    double min = 0.0;
    for(int i = 0; i < graph.vertexAmount; ++i)
    {
        if(selectedSet[destinationVertex] == true)//shortest path has already been found
            break;
        if(i == sourceVertex)
            continue;
        int index = findMin(distanceF, min, selectedSet);
        selectedSet[index] = true;//The shortest path has been found for the current vertex.
        numberOfSelectedVertex++;
        for(int i = 0;i < graph.vertexAmount; ++i)//find out shortest path, with the consideration of Manhattan distance
        {
            if(i == sourceVertex)
                continue;
            //!selectedSet[i] for solve cycles
            if(!selectedSet[i] && min+ graph.arcs[index][i] + getDistanceBetweenTwoNodes(graph.vertexSet[i], graph.vertexSet[destinationVertex]) < distanceF[i])
            {
                distanceF[i] = min + graph.arcs[index][i] + getDistanceBetweenTwoNodes(graph.vertexSet[i], graph.vertexSet[destinationVertex]);
                path[i] = index;
            }
        }
    }
    return;
}
void readAndInit(ifstream& file){
    file >> graph.vertexAmount;//read number of vertex
    cout << "The total number of vertexes in the graph is: " << graph.vertexAmount << endl;
    file >> graph.edgeAmount;//read number of edge

    if (file.good()){
        graph.vertexSet = new Vertex[graph.vertexAmount];
        graph.arcs = new double* [graph.vertexAmount];
        for (int i = 0; i < graph.vertexAmount; ++i) {
            graph.arcs[i] = new double[graph.vertexAmount];
            for(int j = 0; j < graph.vertexAmount; ++j)
                graph.arcs[i][j] = INFINITY;
        }
        //read every vertex information
        char label;
        double x;
        double y;
        for(int i = 0; i < graph.vertexAmount; ++i){
            file >> label >> x >> y;
            if (file.good()) {
                graph.vertexSet[i].label = label;
                graph.vertexSet[i].x = x;
                graph.vertexSet[i].y = y;
            }
        }

        //read every edge information
        char vertexA;
        char vertexB;
        double weight = 0.0;
        for (int i = 0; i < graph.edgeAmount; ++i) {
            file >> vertexA >> vertexB >> weight;
            if (file.good()) {
                graph.arcs[static_cast<int>(vertexB) - 97][static_cast<int>(vertexA) - 97] = weight;
                graph.arcs[static_cast<int>(vertexA) - 97][static_cast<int>(vertexB) - 97] = weight;
            }
        }
        char tempSourceVertex;//story temp sourceVertex
        char tempDestinationVertex;//story temp destinationVertex
        file >> tempSourceVertex;
        sourceVertex = static_cast<int>(tempSourceVertex) - 97;
        file >> tempDestinationVertex;
        destinationVertex = static_cast<int>(tempDestinationVertex) - 97;
        cout << "Start Vertex: " << tempSourceVertex << endl;
        cout << "Goal Vertex: " << tempDestinationVertex << endl << endl;
    }
    return;
}
double getDistanceBetweenTwoNodes(Vertex &start, Vertex &end)
{
    return abs(start.x - end.x) + abs(start.y - end.y);//Manhattan distance
}
int findMin(double distanceF[], double &min, bool selectedSet[])//get the index of minimum value in a array. min is an output variable, which tells the smallest value
{
    double minVal = INFINITY;
    int index = 0;
    //get the smallest value among the numbers in distance function array
    for(int i = 0; i <graph.vertexAmount; ++i)
    {
        if(minVal > distanceF[i] && selectedSet[i] == false)
        {
            minVal = distanceF[i];
            index = i;
        }
    }
    min = minVal;
    if(min == INFINITY)
    {
        cout<<"No path between the two vertexes you specified."<<endl;
        return INFINITY;
    }
    return index;
}
void caluShortestPath(int path[], int numberOfSelectedVertex, shortestPath & shortestPathInfo)
{
    Stack tmpStack;
    tmpStack.vertex = new int[graph.vertexAmount];
    tmpStack.top = 0;
    bool twoNodeSame = false;//judge two nodes the same
    for(int i = 0; i < graph.vertexAmount; ++i)
        tmpStack.vertex[i] = 0;
    //get ready to display
    enStack(tmpStack, destinationVertex);//let the terminal node get into the stack first, as it will be popped up last
    int tmp = destinationVertex;
    while(path[tmp] != sourceVertex)//if some nodes still need to be stored
    {
        tmp = path[tmp];
        enStack(tmpStack, tmp);//push into the stack

    }
    enStack(tmpStack, sourceVertex);//store the start node

    //start to display
    shortestPathInfo.numberOfEdge = tmpStack.top - 1;
    shortestPathInfo.path = new char[tmpStack.top];
    shortestPathInfo.lengthOfTheShortestPath = 0;
    for(int i = shortestPathInfo.numberOfEdge; i >= 0; --i)
    {
        shortestPathInfo.path[i] = static_cast<char>(tmpStack.vertex[i] + 97);
        if(i != 0 )
        {
            shortestPathInfo.lengthOfTheShortestPath += graph.arcs[tmpStack.vertex[i]][tmpStack.vertex[i - 1]];//length of shortest path
            if(tmpStack.vertex[i] == tmpStack.vertex[i - 1])//test whether the two specified nodes are same
                twoNodeSame = true;
        }
    }
    if(twoNodeSame == false)//two node are not same
        shortestPathInfo.numberOfadditionNode = numberOfSelectedVertex - tmpStack.top;
    else//two nodes are the same
    {
        shortestPathInfo.lengthOfTheShortestPath = 0;
        shortestPathInfo.numberOfadditionNode = 0;
    }

    //free memory
    delete [] tmpStack.vertex;
    return;
}

void printShortestInfo(shortestPath & shortestPathInfo){
    for (int i = shortestPathInfo.numberOfEdge; i >= 0; --i) {
        cout << shortestPathInfo.path[i];
        if (i != 0)
            cout << " -> ";
    }
    cout << endl;

    if(shortestPathInfo.lengthOfTheShortestPath == INFINITY)//No path between two nodes found
        cout << "The length of the shortest path: No path exists between the two nodes you specified!" << endl;
    else{
        cout << "The length of the path: " << shortestPathInfo.lengthOfTheShortestPath << endl;
        cout << "The number of additional nodes in the solution tree: "<< shortestPathInfo.numberOfadditionNode << endl;
    }

    return;
}
void enStack(Stack &s, int vertexNumber)
{
    s.vertex[s.top++] = vertexNumber;
    return;
}
//siftUp heap
void siftUp(int indexIn, shortestPath * heapArray)
{
    int parentIdx = parent(indexIn);
    if (parentIdx == -1)//index is roof
        return;
    if (heapArray[indexIn].lengthOfTheShortestPath < heapArray[parentIdx].lengthOfTheShortestPath) {
        swap(parentIdx, indexIn, heapArray);
        siftUp(parentIdx, heapArray);
    }
    return;
}
int parent(int index)
{
    index++;
    if (index <= 1)//empty or root has no parent
        return -1;

    return (index / 2) - 1;
}
void swap(int indexOne, int indexTwo, shortestPath * heapArray)
{
    shortestPath temp;
    temp.path = new char[heapArray[indexOne].numberOfEdge + 1];

    //temp = indexOne
    for (int i = heapArray[indexOne].numberOfEdge; i >= 0; --i)
        temp.path[i] = heapArray[indexOne].path[i];
    temp.numberOfEdge = heapArray[indexOne].numberOfEdge;
    temp.numberOfadditionNode = heapArray[indexOne].numberOfadditionNode;
    temp.lengthOfTheShortestPath = heapArray[indexOne].lengthOfTheShortestPath;
    //indexOne = indexTwo
    for (int i = heapArray[indexOne].numberOfEdge; i >= 0; --i)
        heapArray[indexOne].path[i] = heapArray[indexTwo].path[i];
    heapArray[indexOne].numberOfEdge = heapArray[indexTwo].numberOfEdge;
    heapArray[indexOne].numberOfadditionNode = heapArray[indexTwo].numberOfadditionNode;
    heapArray[indexOne].lengthOfTheShortestPath = heapArray[indexTwo].lengthOfTheShortestPath;
    //indexTwo = temp
    for (int i = heapArray[indexOne].numberOfEdge; i >= 0; --i)
        heapArray[indexTwo].path[i] = temp.path[i];
    heapArray[indexTwo].numberOfEdge = temp.numberOfEdge;
    heapArray[indexTwo].numberOfadditionNode = temp.numberOfadditionNode;
    heapArray[indexTwo].lengthOfTheShortestPath = temp.lengthOfTheShortestPath;

    delete [] temp.path;
    return;
}
