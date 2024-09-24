#include "model.h"

Model::Model(const char* file)
{
	cout << "creating model object..." << endl;
    string text = get_file_contents(file);
    _json = json::parse(text);

	// Get the binary data
	Model::filepath = file;
	data = get_data();

	loadMesh(0); // currently only loading first mesh in the gltf
}

Model::~Model()
{
	cout << "deleting model object..." << endl;
}

// Reads a text file and outputs a string with everything in the text file
string Model::get_file_contents(const char* filename)
{
	ifstream in(filename, ios::binary);
	if (in)
	{
		string contents;
		in.seekg(0, ios::end);
		contents.resize(in.tellg());
		in.seekg(0, ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	// throw(errno);
	throw runtime_error("Error reading/opening file: " + string(strerror(errno)));
}

//https://godotengine.org/article/we-should-all-use-gltf-20-export-3d-assets-game-engines/
// gets bin file of gltf
vector<uchar> Model::get_data()
{
    /*In glTF 2.0, a URI is a flexible way to reference external or embedded resources, allowing for efficient and organized handling of 3D assets. External URIs keep files modular, while data URIs allow for a self-contained glTF file.*/

	// Create a place to store the raw text, and get the uri of the .bin file
    string bytesText;
    string uri = _json["buffers"][0]["uri"];

    string fileStr = string(filepath);
    string fileDirectory = fileStr.substr(0, fileStr.find_last_of('/') + 1);
	
    bytesText = get_file_contents((fileDirectory + uri).c_str());
	
    vector<uchar> data(bytesText.begin(), bytesText.end());

    return data;
}

//https://github.com/KhronosGroup/glTF/tree/main/specification/2.0
vector<float> Model::get_floats(json accessor)
{
    vector<float> verts;

    // value functions gives the value of the key if it exists otherwise the second input is the default
    // buferview and byteOffset are not always found in the gltf 2.0 
    uint bufferViewIndex = accessor.value("bufferView", 1);
    uint count = accessor["count"];
    uint accByteOffset = accessor.value("byteOffset", 0);
    string type = accessor["type"];

    json bufferView = _json["bufferViews"][bufferViewIndex];
    uint byteOffset = bufferView["byteOffset"];

    // Interpret the type and store it into numPerVert
	uint numPerVert;
	if (type == "SCALAR") numPerVert = 1;
	else if (type == "VEC2") numPerVert = 2;
	else if (type == "VEC3") numPerVert = 3;
	else if (type == "VEC4") numPerVert = 4;
	else throw invalid_argument("Type is invalid (not SCALAR, VEC2, VEC3, or VEC4)");

	// Go over all the bytes in the data at the correct place using the properties from above
	uint beginningOfData = byteOffset + accByteOffset;
	uint lengthOfData = count * 4 * numPerVert;
	for (uint i = beginningOfData; i < beginningOfData + lengthOfData; i)
	{
		uchar bytes[] = { data[i++], data[i++], data[i++], data[i++] };
		float value;
		memcpy(&value, bytes, sizeof(float));
		verts.push_back(value);
	}

	return verts;
}

vector<uint> Model::get_indices(json accessor)
{
	vector<uint> indices;

	// Get properties from the accessor
	uint buffViewIndex = accessor.value("bufferView", 0);
	uint count = accessor["count"];
	uint accByteOffset = accessor.value("byteOffset", 0);
	uint componentType = accessor["componentType"];

	// Get properties from the bufferView
	json bufferView = _json["bufferViews"][buffViewIndex];
	uint byteOffset = bufferView["byteOffset"];

	// Get indices with regards to their type: unsigned int, unsigned short, or short
	uint beginningOfData = byteOffset + accByteOffset;
	if (componentType == 5125) // u int
	{
		for (uint i = beginningOfData; i < byteOffset + accByteOffset + count * 4; i)
		{
			uchar bytes[] = { data[i++], data[i++], data[i++], data[i++] };
			uint value;
			memcpy(&value, bytes, sizeof(uint));
			indices.push_back((uint)value);
		}
	}
	else if (componentType == 5123) // u short
	{
		for (uint i = beginningOfData; i < byteOffset + accByteOffset + count * 2; i)
		{
			uchar bytes[] = { data[i++], data[i++] };
			ushort value;
			memcpy(&value, bytes, sizeof(ushort));
			indices.push_back((uint)value);
		}
	}
	else if (componentType == 5122) // short
	{
		for (uint i = beginningOfData; i < byteOffset + accByteOffset + count * 2; i)
		{
			uchar bytes[] = { data[i++], data[i++] };
			short value;
			memcpy(&value, bytes, sizeof(short));
			indices.push_back((uint)value);
		}
	}

	return indices;
}

void Model::loadMesh(uint indexMesh)
{
	// Get all accessor indices
	uint posAccIndex 		= _json["meshes"][indexMesh]["primitives"][0]["attributes"]["POSITION"];
	uint normalAccIndex 	= _json["meshes"][indexMesh]["primitives"][0]["attributes"]["NORMAL"];
	uint texAccIndex 		= _json["meshes"][indexMesh]["primitives"][0]["attributes"]["TEXCOORD_0"];
	uint indAccIndex 		= _json["meshes"][indexMesh]["primitives"][0]["indices"];

	// Use accessor indices to get all vertices components
	vector<float> posVec 	= get_floats(_json["accessors"][posAccIndex]);
	vector<vec3> positions 	= group_floats_into_vec3(posVec);

	vector<float> normalVec = get_floats(_json["accessors"][normalAccIndex]);
	vector<vec3> normals 	= group_floats_into_vec3(normalVec);

	vector<float> texVec 	= get_floats(_json["accessors"][texAccIndex]);
	vector<vec2> texUVs 	= group_floats_into_vec2(texVec);

	// // Combine all the vertex components and also get the indices and textures
	vertices 	= group_vecs_into_vertices(positions, normals, texUVs);
	indices 	= get_indices(_json["accessors"][indAccIndex]);
	// vector<Texture> textures = getTextures();

	// Combine the vertices, indices, and textures into a mesh
	// meshes.push_back(Mesh(vertices, indices, textures));
}

vector<vertex> Model::group_vecs_into_vertices(vector<vec3> positions, vector<vec3> normals, vector<vec2> texUVs)
{
	vector<vertex> vertices;
	for (int i = 0; i < positions.size(); i++)
	{
		vertices.push_back
		(
			vertex
			{
				positions[i],
				normals[i],
				texUVs[i]
			}
		);
	}
	return vertices;
}

vector<vec2> Model::group_floats_into_vec2(vector<float> floatVec)
{
	vector<vec2> vectors;
	for (int i = 0; i < floatVec.size();)
	{
		vectors.push_back(vec2(floatVec[i], floatVec[i+1]));
		i+=2;
	}
	return vectors;
}

vector<vec3> Model::group_floats_into_vec3(vector<float> floatVec)
{
	vector<vec3> vectors;
	for (int i = 0; i < floatVec.size();)
	{
		// compile increments from right to left, doing it this way would require rotation later
		// vectors.push_back(vec3(floatVec[i++], floatVec[i++], floatVec[i++]));
		vectors.push_back(vec3(floatVec[i], floatVec[i+1], floatVec[i+2]));
		i+=3;
	}
	return vectors;
}

vector<vec4> Model::group_floats_into_vec4(vector<float> floatVec)
{
	vector<vec4> vectors;
	for (int i = 0; i < floatVec.size();)
	{
		vectors.push_back(vec4(floatVec[i], floatVec[i+1], floatVec[i+2], floatVec[i+3]));
		i += 4;
	}
	return vectors;
}

void Model::cout_vertices()
{
	for (size_t i = 0; i < vertices.size(); i++)
    {
        cout << "[" << i << "]"<< vertices[i].position;
        cout << vertices[i].normal;
        cout << vertices[i].texUV << endl;
    }
}

void Model::cout_indices()
{
    for (size_t i = 0; i < indices.size(); i++)
    {
        cout << indices[i] << endl;
    }
}

Billboard::Billboard(float halfSize)
{
	// float vertexData[] = {
    //     // positions          // colors           // texture coords
    //     o,  o, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
    //     o, -o, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
    //     -o, -o, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
    //     -o,  o, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
    // };

    // swapping order of indices draws the plane with the normal facing the camera 
    // when the billboard shader is applied, so i no longer need to disable depth culling
    // unsigned int indices[] = {  // note that we start from 0!
    //     3,1,0,   // first triangle
    //     3,2,1    // second triangle
    // };

	vertex ver;

    indices.push_back(3);
    indices.push_back(1);
    indices.push_back(0);
    indices.push_back(3);
    indices.push_back(2);
    indices.push_back(1);

    ver.position = vec3(halfSize, halfSize, 0);
    ver.normal = vec3(0,0,0);
    ver.texUV = vec2(1.0, 1.0);

    vertices.push_back(ver);

    ver.position = vec3(halfSize, -halfSize, 0);
    ver.normal = vec3(0,0,0);
    ver.texUV = vec2(1.0, 0.0);

    vertices.push_back(ver);

    ver.position = vec3(-halfSize, -halfSize, 0);
    ver.normal = vec3(0,0,0);
    ver.texUV = vec2(0.0, 0.0);

    vertices.push_back(ver);

    ver.position = vec3(-halfSize, halfSize, 0);
    ver.normal = vec3(0,0,0);
    ver.texUV = vec2(0.0, 1.0);

    vertices.push_back(ver);
}
