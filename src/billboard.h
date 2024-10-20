#pragma once

#include <vector>
#include "glmath.h"
#include "typedef.h"
// #include "model_interface.h"

using namespace std;

class Billboard {
    public:
        vector<vertex> vertices;
        vector<uint> indices;

        Billboard(float halfSize);

        // modelI getInterface();
};