#include "wfc.h"

// Constructor
WfcTiled::WfcTiled(int row, int col) 
{
    maxRows = row;
    maxCols = col;
    arr = vector<vector<int>>(row, vector<int>(col));  // Initialize arr in the body
    initArray();
}


// Initialize the array in the constructor
void WfcTiled::initArray() {
    for (int i = 0; i < arr.size(); ++i) {
        for (int j = 0; j < arr[i].size(); ++j) {
            arr[i][j] = i * j;  // For example, fill with some values
        }
    }
}

void WfcTiled::initArray(int value) {
    for (int i = 0; i < arr.size(); ++i) {
        for (int j = 0; j < arr[i].size(); ++j) {
            arr[i][j] = value;  // For example, fill with some values
        }
    }
}

// Method to print the array
void WfcTiled::printArray() const {
    for (const auto& row : arr) {
        for (int elem : row) {
            cout << elem << " ";
        }
        cout << endl;
    }
}

int WfcTiled::getValue(int row, int col)
{
    if(row > maxRows || col > maxCols)
    {
        return -1;
    }

    return arr[row][col];
}

void WfcTiled::setValue(int row, int col)
{

}
