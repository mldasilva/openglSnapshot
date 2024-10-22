#ifndef model_h
#define model_h

#include <string>
#include <fstream>
#include <sstream>
#include <vector>

#include <json.h>
#include "glmath.h"
#include "typedef.h"
#include "model_interface.h"

using namespace std;
using namespace nlohmann;
using namespace glm;

class Model {
private:
    const char* filepath;
    vector<uchar> data;
    
    vector<uchar> get_data();
    string get_file_contents(const char* filename);

    vector<float> get_floats(json accessor);
    vector<uint> get_indices(json accessor);

    vector<vec2> group_floats_into_vec2(vector<float> floatVec);
    vector<vec3> group_floats_into_vec3(vector<float> floatVec);
    vector<vec4> group_floats_into_vec4(vector<float> floatVec);

    vector<vertex> group_vecs_into_vertices(vector<vec3> positions,vector<vec3> normals,vector<vec2> texUVs);

    void loadMesh(uint indexMesh);

    // ModelInterface data;

    json _json;
    vector<vertex> vertices;
	vector<uint> indices;

public:

    Model(const char* file);
    ~Model();   

    modelI getInterface();
    void cout_vertices();
    void cout_indices(); 

    vector<vertex> getVertices();
    vector<uint> getIndices();
};
#endif