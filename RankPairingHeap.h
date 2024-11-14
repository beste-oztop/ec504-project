#ifndef RANK_PAIRING_HEAP_H
#define RANK_PAIRING_HEAP_H

#include <iostream>
#include <vector>
#include <limits>

// Template class for the Rank Pairing Heap
template <typename KeyType>
class RankPairingHeap {
public:
    // Nested Node class
    class Node {
    public:
        KeyType key;
        int rank;
        Node* parent;
        Node* leftChild;   // Half-order: leftChild pointer
        Node* rightSibling; // Half-order: rightSibling pointer

        // Node constructor initializes key and sets default values for pointers and rank
        Node(const KeyType& key)
            : key(key), rank(0), parent(nullptr), leftChild(nullptr), rightSibling(nullptr) {}
    };

    // Constructor initializes an empty heap
    RankPairingHeap() : root(nullptr) {}

    // Destructor cleans up all nodes in the heap
    ~RankPairingHeap() {
        deleteAll(root);
    }

    // Inserts a key into the heap
    void Insert(const KeyType& key) {
        Node* node = new Node(key);
        root = meld(root, node); // Melds the new node with the existing heap
    }

    // Deletes a node from the heap
    void Delete(const KeyType& key) {
        Node* node = Search(root, key);
        if (!node) {
            std::cout << "Error: Key not found." << std::endl;
            return;
        }
        DecreaseKey(key, std::numeric_limits<KeyType>::min()); //Decreases the node's key to the minimum possible value, effectively moving it to the root.
        ExtractMin(); // Calls ExtractMin to remove it from the heap.
    }

    // Decreases the key of a node with the given key to newKey
    void DecreaseKey(const KeyType& key, const KeyType& newKey) {
        Node* node = Search(root, key);
        if (!node || newKey > node->key) {
            std::cout << "Error: Invalid DecreaseKey operation." << std::endl;
            return;
        }
        node->key = newKey;
        if (node != root) {
            cut(node); // Detaches node from parent and melds it back to the root
            root = meld(root, node);

            // Propagate rank adjustments up the parent chain
            Node* parent = node->parent;
            while (parent && parent->rank > 0) {
                parent->rank = calculateNewRank(parent); // Adjust rank based on RP-Heap rules
                parent = parent->parent;
            }
        }
    }

    // Extracts the minimum key from the heap and removes it
    KeyType ExtractMin() {
        if (!root) {
            std::cout << "Error: Heap is empty." << std::endl;
            return KeyType();
        }
        // Save the current root and its value since it is the minimum!
        KeyType minKey = root->key;
        Node* oldRoot = root;
        // Reconstructs the heap by performing a two-pass meld on the root's children.
        root = twoPassMeld(root->leftChild);
        delete oldRoot;
        return minKey;
    }

    // Checks if a node with the given key exists in the heap
    bool Contains(const KeyType& key) {
        return Search(root, key) != nullptr;
    }

private:
    Node* root;

    // Melds two heaps with half-order structure and rank considerations, returns the new root
    Node* meld(Node* h1, Node* h2) {
        if (!h1) return h2;
        if (!h2) return h1;

        // Determine the new root based on the key values
        if (h1->key <= h2->key) {
            h2->rightSibling = h1->leftChild; // Set h2 as the left child of h1
            h1->leftChild = h2;
            h2->parent = h1;
            // Only increment rank if both have the same rank
            if (h1->rank == h2->rank) {
                h1->rank++;
            }
            return h1;
        } else {
            h1->rightSibling = h2->leftChild; // Set h1 as the left child of h2
            h2->leftChild = h1;
            h1->parent = h2;
            // Only increment rank if both have the same rank
            if (h1->rank == h2->rank) {
                h2->rank++;
            }
            return h2;
        }
    }

    // Two-pass melding to maintain heap structure after ExtractMin
    Node* twoPassMeld(Node* firstSibling) {
        if (!firstSibling || !firstSibling->rightSibling) return firstSibling;

        // Pairing phase: By pairing nodes, we reduce the number of separate trees, which helps maintain a balanced heap structure.
        std::vector<Node*> trees;
        while (firstSibling) {
            Node* a = firstSibling;
            Node* b = firstSibling->rightSibling;
            firstSibling = (b) ? b->rightSibling : nullptr;

            a->rightSibling = nullptr;
            if (b) {
                b->rightSibling = nullptr;
                trees.push_back(meld(a, b)); // Meld pairs of trees
            } else {
                trees.push_back(a);
            }
        }

        // Second pass to meld all resulting trees into a single heap
        Node* result = nullptr;
        for (Node* tree : trees) {
            result = meld(result, tree);
        }

        return result; // Returns the new root after melding
    }

    // Cuts a node from its parent and melds it with the root
    void cut(Node* node) {
        Node* parent = node->parent;
        if (parent) {
            // Remove node from the left child list of its parent
            if (parent->leftChild == node) {
                parent->leftChild = node->rightSibling;
            } else {
                Node* curr = parent->leftChild;
                while (curr->rightSibling != node) {
                    curr = curr->rightSibling;
                }
                curr->rightSibling = node->rightSibling;
            }
            node->parent = nullptr;
            node->rightSibling = nullptr;
        }
    }

    // Adjusts the rank of a node based on its left child and right sibling
    int calculateNewRank(Node* node) {
        int rank = 0;
        Node* child = node->leftChild;
        while (child) {
            rank = std::max(rank, child->rank + 1);
            child = child->rightSibling;
        }
        return rank;
    }

    // Recursively searches for a node with the given key
    Node* Search(Node* node, const KeyType& key) {
        if (!node) return nullptr;
        if (node->key == key) return node;

        Node* result = Search(node->leftChild, key);
        if (result) return result;

        return Search(node->rightSibling, key);
    }

    // Recursively deletes all nodes to prevent memory leaks
    void deleteAll(Node* node) {
        if (node) {
            deleteAll(node->leftChild);
            deleteAll(node->rightSibling);
            delete node;
        }
    }
};

#endif // RANK_PAIRING_HEAP_H
