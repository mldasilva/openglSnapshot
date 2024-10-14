#include "wfc.h"

// Constructor
WfcTiled::WfcTiled(int row, int col) : maxIndex(row * col), maxRows(row), maxCols(col)
{
    tiles.resize(maxIndex);
    initArray();

    weights = {{hole, 5},{land, 75},{water, 20}};

    // rules
    adjacencyRules[unassigned]      = set<tileTypes>({hole, land, water});
    adjacencyRules[hole]            = set<tileTypes>({hole, land});
    adjacencyRules[land]            = set<tileTypes>({hole, land, water});
    adjacencyRules[water]           = set<tileTypes>({land, water});
}

// Initialize the array in the constructor
void WfcTiled::initArray() {
    for (int i = 0; i < tiles.size(); i++) {
        tiles[i].index = i;
        tiles[i].entropy = MAX_ENTROPY;

        tiles[i].nNorth = getNorth(i);
        tiles[i].nEast  = getEast(i);
        tiles[i].nSouth = getSouth(i);
        tiles[i].nWest  = getWest(i);

        tiles[i].nNWest = getNorthWest(i);
        tiles[i].nNEast = getNorthEast(i);
        tiles[i].nSWest = getSouthWest(i);
        tiles[i].nSEast = getSouthEast(i);
    }
}

// Method to print the array
void WfcTiled::printIndices() const {
    for (size_t i = 0; i < tiles.size(); i++)
    {
        cout << setw(2) << tiles[i].index << " ";
        
        if((i+1) % maxCols == 0)
        {
            cout << endl;
        }
    }
}

void WfcTiled::printValues() const {
    for (size_t i = 0; i < tiles.size(); i++)
    {
        // cout << tiles[i].tileType << " ";
        int t = tiles[i].tileType;
        if(t == 0)
        {
            cout << " " << " ";
        }
        if(t == 1)
        {
            cout << "@" << " ";
        }
        if(t == 2)
        {
            cout << "_" << " ";
        }
        if(t == 3)
        {
            cout << "w" << " ";
        }
        
        if((i+1) % maxCols == 0)
        {
            cout << endl;
        }
    }
}

// return -1 is not found
wfcTile* WfcTiled::getNorth(int index)
{
    if(index > maxIndex || (index - maxCols) < 0 || index < 0)
    {
        return nullptr;
    }
    return &tiles[index - maxCols];
}

// return -1 is not found
wfcTile* WfcTiled::getEast(int index)
{
    if(index > maxIndex || ((index + 1) % maxCols == 0) || index < 0)
    {
        return nullptr;
    }
    return &tiles[index + 1];
}

// return -1 is not found
wfcTile* WfcTiled::getSouth(int index)
{
    if(index > maxIndex || (index + maxCols) >= maxIndex || index < 0)
    {
        return nullptr;
    }
    return &tiles[index + maxCols];
}

wfcTile* WfcTiled::getWest(int index)
{
    if(index > maxIndex || index % maxCols == 0 || index < 0)
    {
        return nullptr;
    }
    return &tiles[index - 1];
}

wfcTile* WfcTiled::getNorthWest(int index)
{
    if(index > maxIndex || (index - maxCols - 1) < 0 || index < 0)
    {
        return nullptr;
    }
    return &tiles[index - maxCols - 1];
}

wfcTile* WfcTiled::getNorthEast(int index)
{
    if(index > maxIndex || (index - maxCols + 1) < 0 || index < 0)
    {
        return nullptr;
    }
    return &tiles[index - maxCols + 1];
}

wfcTile* WfcTiled::getSouthWest(int index)
{
    if(index > maxIndex || (index + maxCols - 1) >= maxIndex || index < 0)
    {
        return nullptr;
    }
    return &tiles[index + maxCols - 1];
}

wfcTile* WfcTiled::getSouthEast(int index)
{
    if(index > maxIndex || (index + maxCols + 1) >= maxIndex || index < 0)
    {
        return nullptr;
    }
    return &tiles[index + maxCols + 1];
}

/// @brief 
/// @param engine 
/// @param range 
/// @param weights in percentages 1-100
/// @return -1 if errored, should never
tileTypes WfcTiled::rndWeighted(mt19937& engine, const vector<pair<tileTypes, int>>& weights) 
{
    // Create a uniform distribution to generate a random number
    uniform_int_distribution<> distr(1, 100);
    int rnd = distr(engine);

    // cout << "Rolled: " << rnd << endl;

    // Calculate the cumulative weights
    int cumulativeWeight = 0;
    for (const auto& weight : weights) 
    {
        cumulativeWeight += weight.second;  // Sum the weights
    }

    // Check if the cumulative weight is greater than 100
    if (cumulativeWeight > 100) 
    {
        cerr << "Error: Total weight exceeds 100." << endl;
        return unassigned; // Return an error code
    }

    // Iterate over the weights to find the corresponding index
    int accumulatedWeight = 0;
    for (const auto& weight : weights) {
        accumulatedWeight += weight.second; // Accumulate weights
        if (rnd <= accumulatedWeight) {
            return weight.first; // Return the corresponding index
        }
    }

    // If no weight matched, return an error code (should not happen)
    cerr << "Error: No matching weight found." << endl;
    return unassigned; // Return an error code
}

/// @brief 
/// @param tile 
/// @param randomNumber 
/// @return returns key
void WfcTiled::generateTile(wfcTile* tile, mt19937& engine)
{   
    // if(tile->tileType != unassigned) return;

    set<tileTypes> types; // set to stop dups

    if(tile->nNorth)types.insert(tile->nNorth->tileType);
    if(tile->nEast) types.insert(tile->nEast->tileType);
    if(tile->nSouth)types.insert(tile->nSouth->tileType);
    if(tile->nWest) types.insert(tile->nWest->tileType);

    // diagonal
    if(tile->nNEast)types.insert(tile->nNEast->tileType);
    if(tile->nSEast)types.insert(tile->nSEast->tileType);
    if(tile->nSWest)types.insert(tile->nSWest->tileType);
    if(tile->nNWest)types.insert(tile->nNWest->tileType);



    set<tileTypes> validOptions = adjacencyRules[unassigned];  // or whatever initial rule is needed

    // figure out what tile type is allowed from surrounding tiles
    for (const tileTypes& neighborType : types) 
    {
        set<tileTypes> possibleWithNeighbor = adjacencyRules[neighborType];

        // Intersect current valid options with what's valid based on this neighbor
        set<tileTypes> newValidOptions;

        set_intersection(
            validOptions.begin(), validOptions.end(),
            possibleWithNeighbor.begin(), possibleWithNeighbor.end(),
            inserter(newValidOptions, newValidOptions.begin())
        );

        // Update validOptions to the intersected set
        validOptions = newValidOptions;

        // If no options are left, return early (no valid tiles)
        if (validOptions.empty()) {
            break;
        }
    }

    // loop thru map
    // tileTypes rulekey = unassigned;
    // for(const auto& pair : adjacencyRules)
    // {
    //     // compare valid options with map second element set<tileTypes>
    //     if(validOptions == pair.second)
    //     {
    //         // return its key eg unassigned
    //         rulekey = pair.first;
    //     }
    // }
    
    
    // old weightless way:
    // vector<tileTypes> vectorRules(validOptions.begin(), validOptions.end());
    // uniform_int_distribution<> distr(1, 100);
    // int indexSelection = distr(engine);
    // tile->tileType = vectorRules[indexSelection % validOptions.size()];


    tileTypes potentialTileType = rndWeighted(engine, weights);
    // while validOptions doesnt contain a randomly seleted weighted tile
    while (validOptions.find(potentialTileType) == validOptions.end()) 
    {
        potentialTileType = rndWeighted(engine, weights);
    } 
    tile->tileType = potentialTileType;
}

void WfcTiled::generate()
{
    // Create a random number generator
    random_device rd;  // Seed for random number engine
    mt19937 gen(rd()); // Mersenne Twister engine
    
    // Define the distribution: numbers from 0 to range_upper_limit-1
    // uniform_int_distribution<> distr(0, maxIndex - 1);

    // Generate the random number for first tile
    // int indexSelection = distr(gen);
    
    // nextTiles.push_back(&tiles[indexSelection]);

    for(int i = 0; i < maxIndex; i++)
    {
        // generate tile // collapse tile
        generateTile(&tiles[i], gen);

        // testing:

        // std::this_thread::sleep_for(std::chrono::milliseconds(250));

        // std::system("clear");
        
        // printValues();
        // cout << endl;
    }

    // printValues();
}