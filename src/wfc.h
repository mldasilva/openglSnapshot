#pragma once

#include "glmath.h"

#include <iostream>
#include <iomanip>
#include <random>
#include <unordered_map>
#include <algorithm> // for std::min_element

#include <set>
#include <vector>
// debugging
#include <chrono>
#include <thread>

#define MAX_ENTROPY 4 // 4 tileTypes
using namespace std;


enum tileTypes
{
    unassigned, hole, land, water,
};

// single tile object
struct wfcTile{
    int index = -1;
    int entropy = -1;
    tileTypes tileType = unassigned;

    // neighbours
    wfcTile* nNorth; 
    wfcTile* nEast;
    wfcTile* nSouth;
    wfcTile* nWest;

    wfcTile* nNEast;
    wfcTile* nSEast;
    wfcTile* nNWest;
    wfcTile* nSWest;
};

// Custom comparator for descending order
struct EntropyOrder {
    bool operator()(const wfcTile& lhs, const wfcTile& rhs) const 
    {
        return lhs.entropy > rhs.entropy;
    }
};

class WfcTiled{
    private:
        const int maxIndex; // maxRows * maxCols;
        const int maxRows;  // for error checking setting these values in constructor
        const int maxCols;

        wfcTile* getNorth(int index);
        wfcTile* getEast(int index);
        wfcTile* getSouth(int index);
        wfcTile* getWest(int index);

        wfcTile* getNorthWest(int index);
        wfcTile* getNorthEast(int index);
        wfcTile* getSouthWest(int index);
        wfcTile* getSouthEast(int index);

        vector<pair<tileTypes, int>> weights;
        unordered_map<tileTypes, set<tileTypes>> adjacencyRules;

    public:
        vector<wfcTile> tiles;
        vector<wfcTile*> nextTiles;
      
        WfcTiled(int row, int col);

        void generateTile(wfcTile* tile, mt19937& engine);
        void initArray();

        void printIndices() const;
        void printValues() const;

        void generate();

        tileTypes rndWeighted(mt19937& engine, const vector<pair<tileTypes, int>>& weights);
};