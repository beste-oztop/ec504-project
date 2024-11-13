#include <utility>
#include <string>
#include <list>
#include <vector>
#include <cmath>
#include <iostream>

template <typename Key, typename Value>
class QuakeHeap {
private:
    // Forward decleration of each structure for future use
    struct TEntry;      // Each value is declared as a key-value entry in the heap
    struct TNode;       // Each node of the tournament tree

    struct TEntry {
        TEntry() {}
        TEntry(Key k, Value v);
        Key key() { return _key; }
        Value value() { return _value; }

    private:
        friend class QuakeHeap; //The class has access to the values of the structure
        Key _key;
        Value _value;
        TNode* _node;
    };

    struct TNode {
        TNode(TEntry* e = nullptr);
        TNode(TNode* l, TNode* r);
        TNode* _l;  // Left child
        TNode* _r;  // Right child
        TNode* _p;  // Parent
        TEntry* _entry;
        unsigned int _height;
        typename std::list<TNode*>::iterator _container; // Stores the position in the root list
    };

    using RootIterator = typename std::list<TNode*>::iterator;

    RootIterator _minimum; // Minimum value
    std::list<TNode*> _forest;
    unsigned long _size;
    float _alpha;

public:
    using Entry = TEntry*;
    using Exception = std::runtime_error;

    QuakeHeap(float alpha = 0.5);
    // virtual ~QuakeHeap();
    void insert(TEntry*& entry);
    void decreaseKey(TEntry*& entry, Key newKey);
    TEntry* getMin();
    TEntry* deleteMin();
    bool empty();
    unsigned long size();
    static TEntry* makeEntry(Key key, Value value);

private:
    void updateMin();   // Keeping the updated minimum value
    void shake(TNode* root, bool isUpdateMin = false);  // The quake or shake operation in quake data structure
};

// Definitions
template <typename Key, typename Value>
QuakeHeap<Key, Value>::TEntry::TEntry(Key k, Value v)
    : _key(k), _value(v), _node(nullptr) {}

// TNode Definitions
template <typename Key, typename Value>
QuakeHeap<Key, Value>::TNode::TNode(TEntry* e)
    : _l(nullptr), _r(nullptr), _p(nullptr), _height(0), _entry(e) {}

// Linking two tournament trees
template <typename Key, typename Value>
QuakeHeap<Key, Value>::TNode::TNode(TNode* l, TNode* r)
    : _l(l), _r(r), _p(nullptr), _entry(nullptr) {
    if (l != nullptr && r != nullptr) {
        _entry = (l->_entry->_key < r->_entry->_key) ? l->_entry : r->_entry;
    } else if (l != nullptr) {
        _entry = l->_entry;
    } else if (r != nullptr) {
        _entry = r->_entry;
    }
    
    _height = std::max(l ? l->_height : 0, r ? r->_height : 0) + 1;
}

// QuakeHeap Definitions
template <typename Key, typename Value>
QuakeHeap<Key, Value>::QuakeHeap(float alpha)
    : _size(0), _alpha(alpha) {
    _minimum = _forest.end();
}

// template <typename Key, typename Value>
// QuakeHeap<Key, Value>::~QuakeHeap() {
//     for (auto node : _forest) {
//         delete node;
//     }
// }

// Turning each value into an entry to insert into the data structure
template <typename Key, typename Value>
typename QuakeHeap<Key, Value>::TEntry* QuakeHeap<Key, Value>::makeEntry(Key key, Value value) {
    return new TEntry(key, value);
}

// Checking each value in the list of the nodes and updating the minimum value pointer
template <typename Key, typename Value>
void QuakeHeap<Key, Value>::updateMin() {
    if (_minimum == _forest.end()) {
        --_minimum;
    } else if (_forest.back()->_entry->_key < (*_minimum)->_entry->_key) {
        _minimum = _forest.end();
        --_minimum;
    }
}

// Pushing back the branches to the main root
template <typename Key, typename Value>
void QuakeHeap<Key, Value>::shake(TNode* root, bool isUpdateMin) {
    TEntry* entry = root->_entry;
    while (root != nullptr) {
        if (root->_l && root->_l->_entry == entry) {
            if (root->_r) {
                root->_r->_p = nullptr;
                _forest.push_back(root->_r);
                if (isUpdateMin) updateMin();
            }
            root = root->_l;
        } else if (root->_r && root->_r->_entry == entry) {
            if (root->_l) {
                root->_l->_p = nullptr;
                _forest.push_back(root->_l);
                if (isUpdateMin) updateMin();
            }
            root = root->_r;
        } else {
            break;
        }
    }
}

// Checking if the heap is empty
template <typename Key, typename Value>
bool QuakeHeap<Key, Value>::empty() {
    return _size == 0;
}

// Checking the size of the heap
template <typename Key, typename Value>
unsigned long QuakeHeap<Key, Value>::size() {
    return _size;
}

// Inserting the values into the heap
template <typename Key, typename Value>
void QuakeHeap<Key, Value>::insert(TEntry*& entry) {
    TNode* node = new TNode(entry);
    entry->_node = node;
    _forest.push_back(node);
    node->_container = --_forest.end(); // Store the iterator to the new node in _container
    updateMin();
    ++_size;
}

// Decreasing the key
template <typename Key, typename Value>
void QuakeHeap<Key, Value>::decreaseKey(TEntry*& entry, Key newKey) {

    entry->_key = newKey;
    TNode* x = entry->_node;
    if (x->_p == nullptr) {
        _forest.erase(x->_container);
    } else if (x->_p->_l == x) {
        x->_p->_l = nullptr;
    } else if (x->_p->_r == x) {
        x->_p->_r = nullptr;
    }
    _forest.push_back(x);
    x->_container = --_forest.end(); // Update _container with the new iterator
    updateMin();
}

// Reporting the minimum value
template <typename Key, typename Value>
typename QuakeHeap<Key, Value>::TEntry* QuakeHeap<Key, Value>::getMin() {
    return (*_minimum)->_entry;
}

// Delete min value and shake the data structure and also performing the quake operation
template <typename Key, typename Value>
typename QuakeHeap<Key, Value>::TEntry* QuakeHeap<Key, Value>::deleteMin() {
    RootIterator old_minimum = _minimum;
    TEntry* min_entry = (*_minimum)->_entry;
    shake(*_minimum);
    _forest.erase(old_minimum);

    std::vector<std::list<TNode*>> buckets(1 + std::ceil(log2(_size)));
    std::vector<int> heights(1 + std::ceil(log2(_size)));

    while (!_forest.empty()) {
        TNode* node = _forest.front();
        _forest.pop_front();
        buckets[node->_height].push_back(node);
    }

    for (int i = 0; i < buckets.size(); ++i) {
        std::list<TNode*>* node_list = &buckets[i];
        if (node_list->size() > 1) {
            heights[i] = 1;
        }
        while (node_list->size() > 1) {
            TNode* a = node_list->back();
            node_list->pop_back();
            TNode* b = node_list->back();
            node_list->pop_back();
            TNode* linked = new TNode(a, b);
            a->_p = linked;
            b->_p = linked;
            buckets[i + 1].push_back(linked);
        }
    }

    int min_i = -1;
    for (int i = 1; i < heights.size(); ++i) {
        if (heights[i] > heights[i - 1] * _alpha) {
            min_i = i;
            break;
        }
    }

    _minimum = _forest.end();
    for (int i = 0; i < buckets.size(); ++i) {
        if (!buckets[i].empty()) {
            if (min_i != -1 && i > min_i) {
                shake(buckets[i].front(), true);
            } else {
                _forest.push_back(buckets[i].front());
                updateMin();
            }
        }
    }

    --_size;
    return min_entry;
}
