#include "Solver.h"
#include <iostream>
#include <ctime>
#include <fstream>

using std::string;
using std::vector;
using std::cout;
using std::endl;

uint8_t Solver::cornerTable[88179840];
uint8_t Solver::topEdgeTable[510935040];
uint8_t Solver::bottomEdgeTable[510935040];
std::unordered_set<std::string> Solver::lookupTable;

const int Solver::cornerPoses[8] = { 0, 2, 5, 7, 12, 14, 17, 19 };
const int Solver::edgePoses[12] = { 1, 3, 4, 6, 8, 9, 10, 11, 13, 15, 16, 18 };
const int Solver::indexByID[20] = { 0, 0, 1, 1, 2, 2, 3, 3, 4, 5, 6, 7, 4, 8, 5, 9, 10, 6, 11, 7 };
bool Solver::loadedCornerTable = false;
int Solver::statesVisited;
int Solver::solutionDepth;
int usedCornerTable = 0;

vector<int> Solver::solve(Cube c) {
    //std::ifstream inFile("corners.txt");
    //int count = std::count(std::istreambuf_iterator<char>(inFile),
        //std::istreambuf_iterator<char>(), '\n');
    //cout << count << " c " << count / 2 << endl;
    //generateCornerTable();
    /*
    loadCornerTable();
    size_t count = 0;
    for (unsigned i = 0; i < cornerTable.bucket_count(); ++i) {
        size_t bucket_size = cornerTable.bucket_size(i);
        if (bucket_size == 0) {
            count++;
        }
        else {
            count += bucket_size;
        }
    }
    cout << cornerTable.size() << endl;
    cout << "Size of table is now " << count << " bytes" << endl << endl;

    int s = cornerTable["heeheehaw"];
    cout << s << endl;
    cout << "count of non zero states: " << count << endl;
    while (true) {
        c.doMove();
        cout << "amount " << int(cornerTable[Cube::cornersAsStringFirstFixed(c)]) << endl;
    }
    */
    /*
    loadCornerTable();
    int total = 0;
    for (int i = 0; i < 88179840; i++) {
        total += cornerTable[i];
    }
    cout << "avg of corner table is " << float(total) / float() << endl;
    */
    //cout << getIndexOfFirstSixEdges(c) << "AF";
    //cout << getIndexOfLastSixEdges(c) << " EF";




    
    std::clock_t start;
    double duration;
    start = std::clock();
    
    if (!loadedCornerTable) {
        loadCornerTable();
        loadedCornerTable = true;
    }

    statesVisited = 0;
    usedCornerTable = 0;
    Cube cube = c.copy();
    vector<int> totalPath;

    vector<int> path;
    path.push_back(-1);
    while (!path.empty()) {
        path = getPathToTable(cube);
        cout << "Adding to path:";
        for (int i = 0; i < path.size(); i++) {
            cout << " " << Cube::getMoveByNumber(path.at(i));
            totalPath.push_back(path.at(i));
            cube.makeAMoveBasedOnI(path.at(i));
        }
        cout << endl;
    }
    
    duration = (double(std::clock()) - double(start)) / (double)CLOCKS_PER_SEC;
    cout << "Solving took " << duration << " seconds" << endl;
    cout << "Total states visited: " << statesVisited << endl;
    cout << "Used corner table to return " << usedCornerTable << " times" << endl;
    return totalPath;
    /*
    vector<int> path;
    path.push_back(-1);
    return path;
    */
}

vector<int> Solver::getPathToTable(Cube cube) {
    solutionDepth = getLowerBound(cube);
    if (solutionDepth >= 10 && !loadedCornerTable) {
        loadCornerTable();
        loadedCornerTable = true;
    }
    int searchDepth;
    int currentTableDepth = -2;
    int tableDepth;
    vector<int> path;
    path.push_back(-1);

    while (!path.empty() && path.at(0) == -1) {
        //if (searchDepth >= 5 && cornerTable.size() == 0) { loadCornerTable(); }
        tableDepth = solutionDepth / 2;
        if (tableDepth > MAX_TABLE_DEPTH) {
            tableDepth = MAX_TABLE_DEPTH;
        }
        searchDepth = solutionDepth - tableDepth;

        if (currentTableDepth != tableDepth && tableDepth >= 0) {
            cout << "Increasing depth of table" << endl;
            currentTableDepth = tableDepth;
            loadTableByDepth(tableDepth);
        }

        cout << "Searching with search depth of: " << searchDepth << " and table depth of: " << tableDepth << endl;
        Move m = Move(cube, 100, 1000);
        path = depthFirstForTable(m, 0, searchDepth);
        
        solutionDepth++;
    }

    cout << "States visited to find in table: " << statesVisited << endl;


    cout << "Clearing table" << endl;
    lookupTable.clear();
    cout << "Done clearing table" << endl;
    return path;
}

vector<int> Solver::depthFirstForTable(Move current, int depth, int maxDepth) {
    vector<int> path;
    statesVisited++;
    
    if (depth == maxDepth) {
        if (inTable(current.cube.convertToString())) {
            return current.path;
        }
        else {
            path.push_back(-1);
            return path;
        }
    }


    if (getLowerBound(current.cube) > solutionDepth - depth) {
        path.push_back(-1);
        usedCornerTable++;
        return path;
    }

    bool doOppositeMove = current.lastMove / 6 != current.secondLastMove / 6;
	//cout << "last move " << current.lastMove << endl << "second last: " << current.secondLastMove << endl;
    for (int i = 0;  i < 18; i++) {
		//doOppositeMove &= i / 3 > current.lastMove / 3;
		bool isSameTypeAsLastMove = current.lastMove / 3 == i / 3;
		bool isOppositeLayerOfLastMove = current.lastMove / 6 == i / 6;
		if (!isSameTypeAsLastMove && (!isOppositeLayerOfLastMove || doOppositeMove)) {
            Move nextMove = current.copy();
            nextMove.makeMove(i);
            path = Solver::depthFirstForTable(nextMove, depth + 1, maxDepth);
            if (!path.empty() && !(path.at(0) == -1)) {
                return path;
            }
		}
	}
    return path;
}

void Solver::inflateStates(int newDepth) {
    //int x;
    std::cout << endl << "Generating table of depth " << newDepth << std::endl;
    //std::cin >> x;
    lookupTable.clear();
    cout << "cleard" << endl;
    //std::cin >> x;
    Cube solved = Cube();
    Move move = Move(solved, 100, 1000);
    cout << "genenign" << endl;
    genStates(move, 0, newDepth);
    std::cout << "Finished generating table of depth " << newDepth << std::endl;
    size_t count = 0;
    for (unsigned i = 0; i < lookupTable.bucket_count(); ++i) {
        size_t bucket_size = lookupTable.bucket_size(i);
        if (bucket_size == 0) {
            count++;
        }
        else {
            count += bucket_size;
        }
    }
    std::cout << "Size of table is now " << float(count) / 1024 << " kilobytes" << std::endl << endl;
}

void Solver::genStates(Move current, int depth, int maxDepth) {
    if (depth >= maxDepth) {
        /*
        string s = current.cube.convertToString();
        if (inTable(s)) {
            cout << "duplicate" << endl;
            for (int i = 0; i < current.path.size(); i++) {
                cout << Cube::getMoveByNumber(current.path.at(i)) << " ";
            }
            cout << endl;
        }
        lookupTable[s] = true;
        */
        //if (current.path.size() == 4 && current.path.at(0) == 6 && current.path.at(1) == 13 && ((current.path.at(2) == 10 && current.path.at(3) == 6) || (current.path.at(2) == 6 && current.path.at(3) == 10))) {
          //  cout << "in !" << endl;
        //}
        lookupTable.emplace(current.cube.convertToString());
        //std::cout << sizeof(lookupTable) << " m" << std::endl;
        return;
    }

    bool doOppositeMove = current.lastMove / 6 != current.secondLastMove / 6;
    //cout << "last move " << current.lastMove << endl << "second last: " << current.secondLastMove << endl;
    for (int i = 0; i < 18; i++) {
        //doOppositeMove &= i / 6 > current.lastMove / 6;
        bool isSameTypeAsLastMove = current.lastMove / 3 == i / 3;
        bool isOppositeLayerOfLastMove = current.lastMove / 6 == i / 6;
        if (!isSameTypeAsLastMove && (!isOppositeLayerOfLastMove || doOppositeMove)) {
            Move nextMove = current.copy();
            nextMove.makeMove(i);
            genStates(nextMove, depth + 1, maxDepth);
        }
    }
}

bool Solver::inTable(string key) {
    return lookupTable.find(key) != lookupTable.end();
}

void Solver::generateCornerStates(Move current, int depth, int maxDepth) {
    int index = Cube::getIndexOfCorners(current.cube);
    if (cornerTable[index] == 13) {
        cornerTable[index] = depth;
        statesVisited++;
    }
    else if (cornerTable[index] > depth) {
        cornerTable[index] = depth;
    }
    else {
        //is worse or same as previously visited corner state, no point in continuing
        return;
    }

    if (depth == maxDepth) {
        return;
    }
    
    bool doOppositeMove = current.lastMove / 6 != current.secondLastMove / 6;
    //cout << "last move " << current.lastMove << endl << "second last: " << current.secondLastMove << endl;
    for (int i = 0; i < 18; i++) {
        //doOppositeMove &= i / 3 > current.lastMove / 3;
        bool isSameTypeAsLastMove = current.lastMove / 3 == i / 3;
        bool isOppositeLayerOfLastMove = current.lastMove / 6 == i / 6;
        if (!isSameTypeAsLastMove && (!isOppositeLayerOfLastMove || doOppositeMove)) {
            Move nextMove = current.copy();
            nextMove.makeMove(i);
            if (depth <= 2) { cout << "Doing move: " << Cube::getMoveByNumber(i) << " at depth of " << depth << endl; }
            generateCornerStates(nextMove, depth + 1, maxDepth);
            if (depth == 5) {
                cout << 88179840 - statesVisited << " elements left to calculate" << endl;
                //cout << "Done " << numOfThreeDepthStates << " out of " << 81 * 81 << " (" << double(numOfThreeDepthStates) / double(81 * 81) * 100 << "% done)" << endl;
            }
        }
    }
}

void Solver::generateCornerTable() {
    std::clock_t totalStart;
    std::clock_t start;
    double duration;
    
    totalStart = std::clock();
    start = std::clock();
    statesVisited = 0;
    Cube cube = Cube();
    Move move = Move(cube, 100, 1000);
    //uint8_t t[88179840];
    //cornerTable = t;
    for (int i = 0; i < 88179840; i++) {
        cornerTable[i] = 13;
    }

    generateCornerStates(move, 0, 11);
    /*
    for (int i = 0; i <= 11; i++) {
        cout << "Starting with a depth of " << i << endl;
        start = std::clock();
        Cube cube = Cube();
        Move move = Move(cube, 100, 1000);
        generateCornerStates(move, 0, i);
        duration = (double(std::clock()) - double(start)) / (double)CLOCKS_PER_SEC;
        cout << "Took " << duration << " seconds" << endl;
        cout << "Elements in table: " << cornerTable.size() << endl;
        size_t count = 0;
        for (unsigned i = 0; i < cornerTable.bucket_count(); ++i) {
            size_t bucket_size = cornerTable.bucket_size(i);
            if (bucket_size == 0) {
                count++;
            }
            else {
                count += bucket_size;
            }
        }
        std::cout << "Size of table is now " << float(count) / 1024 << " kilobytes" << std::endl << endl;
    }
    */
    
    
    cout << "All states generated" << endl;
    cout << "States visited: " << statesVisited << endl;
    duration = (double(std::clock()) - double(start)) / (double)CLOCKS_PER_SEC;
    cout << "Took " << duration << " seconds" << endl;
    start = std::clock();
    cout << "Writing to File..." << endl;

    std::ofstream cornerFile;
    cornerFile.open("corners.txt");
    for (int i = 0; i < 88179840; i++) {
        cornerFile << int(cornerTable[i]) << ' ';
    }
    cornerFile.close();
    cout << "Done!" << endl;
    duration = (double(std::clock()) - double(start)) / (double)CLOCKS_PER_SEC;
    cout << "Took " << duration << " seconds" << endl;
    
    


    /*
    cout << "Reading File..." <<  endl;
    start = std::clock();
    std::ifstream cornerFile2;
    string key;
    string val;
    cornerFile2.open("corners.txt");
    while (std::getline(cornerFile2, key)) {
        std::getline(cornerFile2, val);
        uint8_t x = char(val[0]);
        cornerTable[key] = x;
    }
    cornerFile2.close();
    cout << "Done reading file!" << endl;
    */

    duration = (double(std::clock()) - double(start)) / (double)CLOCKS_PER_SEC;
    cout << "Took " << duration << " seconds" << endl;
    duration = (double(std::clock()) - double(totalStart)) / (double)CLOCKS_PER_SEC;
    cout << "Total time was " << duration << " seconds" << endl;
}

void Solver::loadCornerTable() {
    cout << "Reading File..." << endl;
    std::clock_t start = std::clock();
    double duration;
    std::ifstream cornerFile;

    cornerFile.open("corners.txt");
    int lineNum = 0;
    int x;
    while (cornerFile >> x) {
        cornerTable[lineNum] = x;
        lineNum++;
        //if (lineNum > 20) break;
    }
    /*for (int x = 0; x < 100; x++) {
        cout << int(cornerTable[x]) << " at " << x << endl;
    }*/
    cornerFile.close();
    cout << "Done reading file!" << endl;
    duration = (double(std::clock()) - double(start)) / (double)CLOCKS_PER_SEC;
    cout << "Took " << duration << " seconds" << endl;
}

int Solver::getLowerBound(Cube cube) {
    if (!loadedCornerTable) {
        return 0;
    }
    return cornerTable[Cube::getIndexOfCorners(cube)];
}

int Solver::getIndexOfCorners(Cube cube) {
    int totalNumOfStates = 88179840;
    int index = 0;
    int cornersLeft = 8;
    vector<int> cornerNums;
    cornerNums.insert(cornerNums.begin(), { 0, 2, 5, 7, 12, 14, 17, 19 });

    for (int i = 0; i < 7; i++) {
        //cout << "i " << i;
        Cuboid corner = cube.cube[Solver::edgePoses[i]];
        //get the element it is in the sorted list of the corners left
        int cornerNum = 0;
        while (cornerNums.at(cornerNum) != corner.identity) {
            cornerNum++;
        }
        cornerNums.erase(cornerNums.begin() + cornerNum);
        //cout << " corner num " << cornerNum << endl;

        totalNumOfStates /= cornersLeft;
        index += cornerNum * totalNumOfStates;
        //cout << "index now " << index << endl;
        totalNumOfStates /= 3;
        index += corner.orientation * totalNumOfStates;
        //cout << "index now " << index << endl;
        cornersLeft--;
    }

    return index;
}

int Solver::getIndexOfFirstSevenEdges(Cube cube) {
    int totalNumOfStates = 510935040;
    int index = 0;
    int posLeft = 12;
    int edgePlacements[7] = { 0,0,0,0,0,0,0, };
    int edgeOrientaions[7] = { 0,0,0,0,0,0,0, };
    for (int i = 0; i < 12; ++i) {
        if (cube.cube[edgePoses[i]].identity < 11) {
            edgePlacements[indexByID[cube.cube[edgePoses[i]].identity]] = i;
            edgeOrientaions[indexByID[cube.cube[edgePoses[i]].identity]] = cube.cube[edgePoses[i]].orientation;
        }
    }

    vector<int> places;
    places.insert(places.begin(), { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
    int edgenum;
    for (int i = 0; i < 7; i++) {
        edgenum = 0;
        while (places.at(edgenum) != edgePlacements[i]) {
            edgenum++;
        }
        places.erase(places.begin() + edgenum);
        
        totalNumOfStates /= posLeft;
        index += totalNumOfStates * edgenum;
        totalNumOfStates /= 2;
        index += totalNumOfStates * edgeOrientaions[i];
        posLeft--;
    }
    
    return index;
}

int Solver::getIndexOfLastSevenEdges(Cube cube) {
    int totalNumOfStates = 510935040;
    int index = 0;
    int posLeft = 12;
    int edgePlacements[7] = { 0,0,0,0,0,0,0, };
    int edgeOrientaions[7] = { 0,0,0,0,0,0,0, };
    for (int i = 0; i < 12; ++i) {
        if (cube.cube[edgePoses[i]].identity > 8) {
            edgePlacements[indexByID[cube.cube[edgePoses[i]].identity] - 5] = i;
            edgeOrientaions[indexByID[cube.cube[edgePoses[i]].identity] - 5] = cube.cube[edgePoses[i]].orientation;
        }
    }

    vector<int> places;
    places.insert(places.begin(), { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
    int edgenum;
    for (int i = 0; i < 7; i++) {
        edgenum = 0;
        while (places.at(edgenum) != edgePlacements[i]) {
            edgenum++;
        }
        places.erase(places.begin() + edgenum);

        totalNumOfStates /= posLeft;
        index += totalNumOfStates * edgenum;
        totalNumOfStates /= 2;
        index += totalNumOfStates * edgeOrientaions[i];
        posLeft--;
    }

    return index;
}

void Solver::generateTopEdgeTable() {
    std::clock_t totalStart;
    std::clock_t start;
    double duration;

    totalStart = std::clock();
    start = std::clock();
    statesVisited = 0;
    Cube cube = Cube();
    Move move = Move(cube, 100, 1000);
    //uint8_t t[88179840];
    //cornerTable = t;

    for (int i = 0; i < 510935040; ++i) {
        topEdgeTable[i] = 13;
    }

    generateTopEdgeStates(move, 0, 10);


    cout << "All states generated" << endl;
    cout << "States visited: " << statesVisited << endl;
    duration = (double(std::clock()) - double(start)) / (double)CLOCKS_PER_SEC;
    cout << "Took " << duration << " seconds" << endl;
    start = std::clock();
    cout << "Writing to File..." << endl;

    std::ofstream cornerFile;
    cornerFile.open("topEdges.txt");
    for (int i = 0; i < 510935040; i++) {
        cornerFile << int(topEdgeTable[i]) << ' ';
    }
    cornerFile.close();
    cout << "Done!" << endl; 

    duration = (double(std::clock()) - double(start)) / (double)CLOCKS_PER_SEC;
    cout << "Took " << duration << " seconds" << endl;
    duration = (double(std::clock()) - double(totalStart)) / (double)CLOCKS_PER_SEC;
    cout << "Total time was " << duration << " seconds" << endl;
}

void Solver::generateTopEdgeStates(Move current, int depth, int maxDepth) {
    int index = Solver::getIndexOfFirstSevenEdges(current.cube);
    if (topEdgeTable[index] == 13) {
        topEdgeTable[index] = depth;
        statesVisited++;
    }
    else if (topEdgeTable[index] > depth) {
        topEdgeTable[index] = depth;
    }
    else {
        //is worse than previously visited corner state, no point in continuing
        return;
    }

    if (depth == maxDepth) {
        return;
    }

    bool doOppositeMove = current.lastMove / 6 != current.secondLastMove / 6;
    //cout << "last move " << current.lastMove << endl << "second last: " << current.secondLastMove << endl;
    for (int i = 0; i < 18; i++) {
        //doOppositeMove &= i / 3 > current.lastMove / 3;
        bool isSameTypeAsLastMove = current.lastMove / 3 == i / 3;
        bool isOppositeLayerOfLastMove = current.lastMove / 6 == i / 6;
        if (!isSameTypeAsLastMove && (!isOppositeLayerOfLastMove || doOppositeMove)) {
            Move nextMove = current.copy();
            nextMove.makeMove(i);
            if (depth <= 2) {
                cout << "Doing path: ";
                for (int j = 0; j < nextMove.path.size(); j++) {
                    cout << Cube::getMoveByNumber(nextMove.path.at(j)) << " ";
                }
                cout << endl;
            }
            generateTopEdgeStates(nextMove, depth + 1, maxDepth);
            if (depth == 3) {
                cout << 510935040 - statesVisited << " elements left to calculate" << endl;
                //cout << "Done " << numOfThreeDepthStates << " out of " << 81 * 81 << " (" << double(numOfThreeDepthStates) / double(81 * 81) * 100 << "% done)" << endl;
            }
        }
    }
}

void Solver::generateBottomEdgeTable() {
    std::clock_t totalStart;
    std::clock_t start;
    double duration;

    totalStart = std::clock();
    start = std::clock();
    statesVisited = 0;
    Cube cube = Cube();
    Move move = Move(cube, 100, 1000);
    //uint8_t t[88179840];
    //cornerTable = t;
    for (int i = 0; i < 510935040; ++i) {
        bottomEdgeTable[i] = 13;
    }
    
    generateBottomEdgeStates(move, 0, 10);


    cout << "All states generated" << endl;
    cout << "States visited: " << statesVisited << endl;
    duration = (double(std::clock()) - double(start)) / (double)CLOCKS_PER_SEC;
    cout << "Took " << duration << " seconds" << endl;

    start = std::clock();
    cout << "Writing to File..." << endl;

    std::ofstream cornerFile;
    cornerFile.open("bottomEdges.txt");
    for (int i = 0; i < 510935040; i++) {
        cornerFile << int(bottomEdgeTable[i]) << '\n';
    }
    cornerFile.close();
    cout << "Done!" << endl;
    duration = (double(std::clock()) - double(start)) / (double)CLOCKS_PER_SEC;


    duration = (double(std::clock()) - double(start)) / (double)CLOCKS_PER_SEC;
    cout << "Took " << duration << " seconds" << endl;
    duration = (double(std::clock()) - double(totalStart)) / (double)CLOCKS_PER_SEC;
    cout << "Total time was " << duration << " seconds" << endl;
}

void Solver::generateBottomEdgeStates(Move current, int depth, int maxDepth) {
    int index = Solver::getIndexOfLastSevenEdges(current.cube);
    //cout << "got index" << endl;
    if (bottomEdgeTable[index] == 13) {
        bottomEdgeTable[index] = depth;
        statesVisited++;
    }
    else if (bottomEdgeTable[index] > depth) {
        bottomEdgeTable[index] = depth;
    }
    else {
        //is worse than previously visited corner state, no point in continuing
        return;
    }

    if (depth == maxDepth) {
        return;
    }

    bool doOppositeMove = current.lastMove / 6 != current.secondLastMove / 6;
    //cout << "last move " << current.lastMove << endl << "second last: " << current.secondLastMove << endl;
    for (int i = 0; i < 18; i++) {
        //doOppositeMove &= i / 3 > current.lastMove / 3;
        bool isSameTypeAsLastMove = current.lastMove / 3 == i / 3;
        bool isOppositeLayerOfLastMove = current.lastMove / 6 == i / 6;
        if (!isSameTypeAsLastMove && (!isOppositeLayerOfLastMove || doOppositeMove)) {
            Move nextMove = current.copy();
            nextMove.makeMove(i);
            if (depth <= 2) { 
                cout << "Doing path: ";
                for (int j = 0; j < nextMove.path.size(); j++) {
                    cout << Cube::getMoveByNumber(nextMove.path.at(j)) << " ";
                }
                cout << endl;
            }
            generateBottomEdgeStates(nextMove, depth + 1, maxDepth);
            if (depth == 3) {
                cout << 510935040 - statesVisited << " elements left to calculate" << endl;
                //cout << "Done " << numOfThreeDepthStates << " out of " << 81 * 81 << " (" << double(numOfThreeDepthStates) / double(81 * 81) * 100 << "% done)" << endl;
            }
        }
    }
}

void Solver::loadTableByDepth(int depth) {
    cout << "Loading table with a depth of " << depth << endl;
    lookupTable.clear();
    std::ifstream tableFile;
    tableFile.open("tableDepth" + std::to_string(depth) + ".txt");

    string key;
    while (std::getline(tableFile, key)) {
        for (int i = 0; i < key.length(); ++i) {
            key[i] -= 150;
        }
        lookupTable.insert(key);
    }
    tableFile.close();
    cout << "Done loading table" << endl;
}