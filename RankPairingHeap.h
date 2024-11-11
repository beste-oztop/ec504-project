// RankPairingHeap.h

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
        Node* child;
        Node* sibling;

        Node(const KeyType& key)
            : key(key), rank(0), child(nullptr), sibling(nullptr) {}
    };

    // Constructor
    RankPairingHeap() : root(nullptr) {}

    // Destructor
    ~RankPairingHeap() {
        deleteAll(root);
    }

    // Inserts a key into the heap and returns a pointer to the node
    void Insert(const KeyType& key) {
        Node* node = new Node(key);
        root = meld(root, node); // Melds the new node with the existing heap.
    }

    // Deletes a node from the heap
    void Delete(const KeyType& key) {
        Node* node = Search(root, key);
        if (node == nullptr) {
            std::cout << "Error: Key not found." << std::endl;
            return;
        }
        Decreasenode(node, std::numeric_limits<KeyType>::min()); //Decreases the node's key to the minimum possible value, effectively moving it to the root.
        ExtractMin(); // Calls ExtractMin to remove it from the heap.
    }

    // Decreases the key of a node with the given key to newKey
    void DecreaseKey(const KeyType& key, const KeyType& newKey) {
        Node* node = Search(root, key);
        if (node == nullptr) {
            std::cout << "Error: Key not found." << std::endl;
            return;
        }
        Decreasenode(node, newKey);
    }

    // Extracts the minimum key from the heap and removes it
    KeyType ExtractMin() {
        if (root == nullptr) {
            std::cout << "Error: Heap is empty." << std::endl;
            return KeyType();
        }
        // Save the current root and its value since it is the minimum!
        KeyType minKey = root->key;
        Node* oldRoot = root;
        // Reconstructs the heap by performing a two-pass meld on the root's children.
        root = twoPassMeld(root->child);
        delete oldRoot;
        return minKey;
    }

    // Searches for a node with the given key
    bool Contains(const KeyType& key) {
        return Search(root, key) != nullptr;
    }

private:
    Node* root;

    // Melds two heaps and returns the new root
    Node* meld(Node* h1, Node* h2) {
        if (h1 == nullptr) return h2; // If h1 is empty, return h2
        if (h2 == nullptr) return h1; // If h2 is empty, return h1

        if (h1->key <= h2->key) {
            // h1 has the smaller key; make h1 the root
            h2->sibling = h1->child; 
            h1->child = h2;          
            h1->rank = h2->rank + 1; 
            return h1;               
        } else {
            // h2 has the smaller key; make h2 the root
            h1->sibling = h2->child;
            h2->child = h1;          
            h2->rank = h1->rank + 1; 
            return h2;               
        }
    }

    // Rebuild the heap after Extract-Min
    Node* twoPassMeld(Node* firstSibling) {
        if (firstSibling == nullptr || firstSibling->sibling == nullptr) {
            return firstSibling;
        }

        // Pairwise Melding: By pairing nodes, we reduce the number of separate trees, which helps maintain a balanced heap structure.
        std::vector<Node*> trees;
        while (firstSibling != nullptr) {
            Node* a = firstSibling;
            Node* b = firstSibling->sibling;
            firstSibling = (b != nullptr) ? b->sibling : nullptr;

            a->sibling = nullptr;
            if (b != nullptr) {
                b->sibling = nullptr;
                trees.push_back(meld(a, b));
            } else {
                trees.push_back(a);
            }
        }

        // Meld all trees in forward order to form a single heap
        Node* result = nullptr;
        for (Node* tree : trees) {
            result = meld(result, tree);
        }

        return result; // The new root after melding.
    }

    // Decreases the key of a given node
    void Decreasenode(Node* node, const KeyType& newKey) {
        //Checks if the node is valid and if the new key is not greater than the current key.
        if (node == nullptr) {
            std::cout << "Error: Node is null." << std::endl;
            return;
        }
        if (newKey > node->key) {
            std::cout << "Error: New key is greater than current key." << std::endl;
            return;
        }
        // If the node is not the root, cuts it from its current position and melds it back into the heap to maintain the heap property.
        node->key = newKey;
        if (node != root) {
            cut(node);
            root = meld(root, node);
        }
    }

    // Cuts a node from its parent
    void cut(Node* node) {
        Node* parent = findParent(root, node); // Find the parent with findParent function.
        if (parent != nullptr) {
            // Remove node from the child list of its parent
            if (parent->child == node) {
                parent->child = node->sibling;
            } else {
                Node* curr = parent->child;
                while (curr->sibling != node) {
                    curr = curr->sibling;
                }
                curr->sibling = node->sibling;
            }
            node->sibling = nullptr;
        }
    }

    // Finds the parent of a node (used in DecreaseKey)
    Node* findParent(Node* current, Node* target) {
        if (current == nullptr) return nullptr;
        for (Node* child = current->child; child != nullptr; child = child->sibling) {
            // If a child matches the target, returns the current node as the parent.
            if (child == target) {
                return current;
            }
            // Recursively searches in the child subtrees.
            Node* parent = findParent(child, target);
            if (parent != nullptr) {
                return parent;
            }
        }
        return nullptr;
    }

    // Recursively searches for a node with the given key
    Node* Search(Node* node, const KeyType& key) {
        if (node == nullptr) return nullptr;
        if (node->key == key) return node;

        Node* result = Search(node->child, key);
        if (result != nullptr) return result;

        return Search(node->sibling, key);
    }

    // Recursively deletes all nodes to prevent memory leaks
    void deleteAll(Node* node) {
        if (node != nullptr) {
            deleteAll(node->child);
            deleteAll(node->sibling);
            delete node;
        }
    }
};

#endif // RANK_PAIRING_HEAP_H
