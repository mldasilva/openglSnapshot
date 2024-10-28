/* keeps track where ai and players are in the world*/
#pragma once
#include "glmath.h"
#include <unordered_map>

class Scene{
    private:
        unordered_map<string, int> map;  // int will be index to vector index
        vector<vec4> positions;
    public:
    
        void add(string name, vec3 position)
        {
            int index = positions.size();
            map.insert({name, index});
            positions.push_back(vec4(position,1));
        }
        
        void update(int index, vec3 position)
        {
            positions[index] = vec4(position,1);
        }

        int find(string name)
        {           
            if (map.find(name) != map.end()) 
            {
                // Key found
                return map[name];
            } 
            else 
            {
                // Key not found
                throw std::runtime_error("Scene Value not found!");
            }

            return 0;
        }

        vec3 fetch(string name)
        {
            const auto um = map.find(name);
            if (um != map.end()) 
            {
                // Key found
                return vec3(positions[um->second]);
            } 
            else 
            {
                // Key not found
                throw std::runtime_error("Scene Value Attribute not found!");
            }
            return vec3(0);
        }

        unsigned int getBufferSize()
        {
            return positions.size() * sizeof(vec4);
        }

        const void *getBufferData()
        {
            return positions.data();
        }


};