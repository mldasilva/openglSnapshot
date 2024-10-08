#pragma once

#include "glmath.h"

using namespace std;

class WfcTiled{
private:
    int maxRows; // for error checking setting these values in constructor
    int maxCols;
    vector<vector<int>> arr;  // 2D vector

public:
    WfcTiled(int row, int col);

    void initArray();
    void initArray(int value);
    void printArray() const;

    int getValue(int row, int col);
    void setValue(int row, int col);
};