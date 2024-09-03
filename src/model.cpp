#include "model.h"
#include <iostream>

model::model(const char* file)
{
    string text = get_file_contents(file);
    _json = json::parse(text);

	// Get the binary data
	model::filepath = file;
	data = get_data();
}

model::~model()
{

}

// Reads a text file and outputs a string with everything in the text file
string model::get_file_contents(const char* filename)
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
vector<uchar> model::get_data()
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
vector<float> model::get_floats(json accessor)
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

vector<uint> model::get_indices(json accessor)
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

void model::loadMesh(uint indexMesh)
{
	// Get all accessor indices
	uint posAccInd 		= _json["meshes"][indexMesh]["primitives"][0]["attributes"]["POSITION"];
	uint normalAccInd 	= _json["meshes"][indexMesh]["primitives"][0]["attributes"]["NORMAL"];
	uint texAccInd 		= _json["meshes"][indexMesh]["primitives"][0]["attributes"]["TEXCOORD_0"];
	uint indAccInd 		= _json["meshes"][indexMesh]["primitives"][0]["indices"];

	// Use accessor indices to get all vertices components
	vector<float> posVec = get_floats(_json["accessors"][posAccInd]);
	// vector<glm::vec3> positions = groupFloatsVec3(posVec);
	// vector<float> normalVec = get_floats(_json["accessors"][normalAccInd]);
	// vector<glm::vec3> normals = groupFloatsVec3(normalVec);
	// vector<float> texVec = get_floats(_json["accessors"][texAccInd]);
	// vector<glm::vec2> texUVs = groupFloatsVec2(texVec);

	// // Combine all the vertex components and also get the indices and textures
	// vector<Vertex> vertices = assembleVertices(positions, normals, texUVs);
	// vector<GLuint> indices = getIndices(JSON["accessors"][indAccInd]);
	// vector<Texture> textures = getTextures();

	// // Combine the vertices, indices, and textures into a mesh
	// meshes.push_back(Mesh(vertices, indices, textures));
}

// vector<Vertex> model::assembleVertices
// (
// 	vector<glm::vec3> positions,
// 	vector<glm::vec3> normals,
// 	vector<glm::vec2> texUVs
// )
// {
// 	vector<Vertex> vertices;
// 	for (int i = 0; i < positions.size(); i++)
// 	{
// 		vertices.push_back
// 		(
// 			Vertex
// 			{
// 				positions[i],
// 				normals[i],
// 				glm::vec3(1.0f, 1.0f, 1.0f),
// 				texUVs[i]
// 			}
// 		);
// 	}
// 	return vertices;
// }

