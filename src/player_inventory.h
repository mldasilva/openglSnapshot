#pragma once

#include <string>
#include <vector>

using namespace std;

struct itemStruct
{
    int index;
    string name;
};

class inventoryManager
{
    private:
        vector<itemStruct> items;
    public:
        void addItem();
};