#ifndef model_h
#define model_h

#include <string>
#include <fstream>
#include <sstream>
#include <vector>

#include <json.h>

#include "typedef.h"

using namespace std;
using namespace nlohmann;

// mesh
// class mesh
// {
// public:
// 	vector <float> vertices;
// 	vector <uint> indices;
// };

// model
class model{
private:
    const char* filepath;
    vector<uchar> data;
    

    vector<uchar> get_data();
    string get_file_contents(const char* filename);

public:
    json _json;

    model(const char* file);
    ~model();

    vector<float> get_floats(json accessor);
    vector<uint> get_indices(json accessor);

    void loadMesh(uint indexMesh);
};

#endif