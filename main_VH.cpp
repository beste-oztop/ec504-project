#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <limits>
#include <iostream>
#include "ViolationHeap.cpp" // Include the Violation Heap header

using namespace std;

// Function to read the graph from the input file and store it in an adjacency list
unordered_map<int, vector<int>> readGraph(const string& filename) {
    unordered_map<int, vector<int>> graph;
    ifstream file(filename);
    string line;

    while (getline(file, line)) {
        if (line[0] == '#') {
            continue; // Skip header lines
        }

        istringstream iss(line);
        int fromNode, toNode;
        if (iss >> fromNode >> toNode) {
            graph[fromNode].push_back(toNode);
            graph[toNode].push_back(fromNode); // Since the graph is undirected
        }
    }

    return graph;
}

// Function to perform Dijkstra's algorithm using Violation Heap
pair<int, int> dijkstraUsingHeap(const unordered_map<int, vector<int>>& graph, int startNode) {
    unordered_map<int, int> distances;       // Distance from startNode to each node
    unordered_map<int, v_node_T> nodeMap;    // Map to store heap nodes
    v_heap_T heap = v_heap_new();            // Create a new Violation Heap

    // Initialize distances and heap nodes
    for (const auto& [node, _] : graph) {
        distances[node] = numeric_limits<int>::max(); // Infinite distance initially
        nodeMap[node] = nullptr; // No nodes in the heap initially
    }

    // Insert the start node into the heap
    distances[startNode] = 0;
    nodeMap[startNode] = v_heap_insert(heap, 0, startNode); // Insert startNode with distance 0

    int farthestNode = startNode;
    int maxDistance = 0;

    // Dijkstra's algorithm loop
    while (get_num_elements(heap) > 0) {
        // Extract the minimum distance node
        int currentDist = v_extract_min(heap);

        // Find the corresponding node with this distance
        int currentNode = -1;
        for (const auto& [node, heapNode] : nodeMap) {
            if (heapNode && distances[node] == currentDist) {
                currentNode = node;
                nodeMap[node] = nullptr; // Mark node as processed
                break;
            }
        }

        // If no valid node is found, skip
        if (currentNode == -1) {
            continue;
        }

        // Debugging: Print current node and distance
        cout << "Processing node: " << currentNode 
             << " with distance: " << currentDist << "\n";

        // Process all neighbors of the current node
        for (int neighbor : graph.at(currentNode)) {
            int newDist = distances[currentNode] + 1; // Unit weight for unweighted graphs

            // Update distance if the new distance is shorter
            if (newDist < distances[neighbor]) {
                distances[neighbor] = newDist;

                // Update heap for the neighbor
                if (nodeMap[neighbor]) {
                    v_heap_decrease_key(heap, nodeMap[neighbor], newDist);
                } else {
                    nodeMap[neighbor] = v_heap_insert(heap, newDist, neighbor);
                }

                // Track the farthest node
                if (newDist > maxDistance) {
                    maxDistance = newDist;
                    farthestNode = neighbor;
                }
            }
        }
    }

    v_heap_free(&heap); // Free the heap memory
    return {farthestNode, maxDistance};
}

// Function to find the diameter of the graph
int findGraphDiameter(const unordered_map<int, vector<int>>& graph) {
    if (graph.empty()) {
        return 0;
    }

    // Pick an arbitrary node to start Dijkstra's
    int startNode = graph.begin()->first;
    pair<int, int> dijkstraResult = dijkstraUsingHeap(graph, startNode);

    // Perform Dijkstra's from the farthest node found in the first run
    pair<int, int> diameterResult = dijkstraUsingHeap(graph, dijkstraResult.first);

    return diameterResult.second;
}

int main() {
    // string filename = "p2p-Gnutella04.txt"; 
    string filename = "p2p-Gnutella24.txt"; 
    // string filename = "email-EuAll.txt"; 
    unordered_map<int, vector<int>> graph = readGraph(filename);

    cout << "Graph loaded successfully with " << graph.size() << " nodes.\n";

    int graphDiameter = findGraphDiameter(graph);
    cout << "The longest shortest path (Diameter) is: " << graphDiameter << endl;

    return 0;
}
