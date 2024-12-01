#ifndef RANK_PAIRING_HEAP_H
#define RANK_PAIRING_HEAP_H

#include <iostream>
#include <vector>
#include <limits>
#include <unordered_map>

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
        int id; // Unique ID for handling duplicates

        Node(const KeyType& key, int id)
            : key(key), rank(0), parent(nullptr), leftChild(nullptr), rightSibling(nullptr), id(id) {}
    };

    RankPairingHeap() : root(nullptr), currentID(0) {}

    ~RankPairingHeap() {
        deleteAll(root);
    }

    Node* Insert(const KeyType& key) {
        Node* node = new Node(key, currentID++);
        root = meld(root, node);
        return node;
    }

    void Delete(Node* node) {
        if (!node) {
            std::cout << "Error: Invalid node reference.\n";
            return;
        }
        DecreaseKey(node, std::numeric_limits<KeyType>::min());
        ExtractMin();
    }

    void DecreaseKey(Node* node, const KeyType& newKey) {
        if (!node) {
            std::cout << "Error: Invalid node reference.\n";
            return;
        }
        if (newKey > node->key) {
            std::cout << "Error: New key is greater than current key.\n";
            return;
        }
        node->key = newKey;
        if (node != root) {
            cut(node);
            root = meld(root, node);
        }
    }

    KeyType ExtractMin() {
        if (!root) {
            std::cout << "Error: Heap is empty.\n";
            return std::numeric_limits<KeyType>::max(); // Return a sentinel value
        }
        KeyType minKey = root->key;
        Node* oldRoot = root;
        root = twoPassMeld(root->leftChild);
        delete oldRoot;
        return minKey;
    }

private:
    Node* root;
    int currentID; // To generate unique IDs for nodes

    Node* meld(Node* h1, Node* h2) {
        if (!h1) return h2;
        if (!h2) return h1;

        if (h1->key <= h2->key) {
            h2->rightSibling = h1->leftChild;
            h1->leftChild = h2;
            h2->parent = h1;
            if (h1->rank == h2->rank) {
                h1->rank++;
            }
            return h1;
        } else {
            h1->rightSibling = h2->leftChild;
            h2->leftChild = h1;
            h1->parent = h2;
            if (h1->rank == h2->rank) {
                h2->rank++;
            }
            return h2;
        }
    }

    Node* twoPassMeld(Node* firstSibling) {
        if (!firstSibling) return nullptr;

        std::vector<Node*> trees;
        while (firstSibling) {
            Node* a = firstSibling;
            Node* b = firstSibling->rightSibling;
            firstSibling = (b) ? b->rightSibling : nullptr;

            a->rightSibling = nullptr;
            if (b) {
                b->rightSibling = nullptr;
                trees.push_back(meld(a, b));
            } else {
                trees.push_back(a);
            }
        }

        Node* result = nullptr;
        for (Node* tree : trees) {
            result = meld(result, tree);
        }
        return result;
    }

    void cut(Node* node) {
        Node* parent = node->parent;
        if (parent) {
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

    void deleteAll(Node* node) {
        if (node) {
            deleteAll(node->leftChild);
            deleteAll(node->rightSibling);
            delete node;
        }
    }
};

#endif // RANK_PAIRING_HEAP_H
