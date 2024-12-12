#include <utility>
#include <string>
#include <list>
#include <vector>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <unordered_map>

template <typename Key, typename Value>
class QuakeHeap {
private:
    

    
    unsigned long _size;
    float _alpha;

public:
    struct TEntry;
    struct TNode;

    struct TEntry {
        TEntry() {}
        TEntry(Key k, Value v);
        Key key() const { return _key; }
        Value value() const { return _value; }

    private:
        friend class QuakeHeap;
        Key _key;
        Value _value;
        TNode* _node;
    };

    struct TNode {
        TNode(TEntry* e = nullptr);
        TNode(TNode* l, TNode* r);
        TNode* _l;
        TNode* _r;
        TNode* _p;
        TEntry* _entry;
        unsigned int _height;
        typename std::list<TNode*>::iterator _container;
    };

    using RootIterator = typename std::list<TNode*>::iterator;

    RootIterator _minimum;
    std::list<TNode*> _forest;

    using Entry = TEntry*;
    using Exception = std::runtime_error;

    QuakeHeap(float alpha = 0.5);
    virtual ~QuakeHeap();
    void insert(TEntry*& entry);
    void decreaseKey(TEntry*& entry, Key newKey);
    TEntry* getMin();
    TEntry* deleteMin();
    bool empty();
    unsigned long size();
    static TEntry* makeEntry(Key key, Value value);

private:
    void updateMin();
    void shake(TNode* root, bool isUpdateMin = false);
};

// Definitions
template <typename Key, typename Value>
QuakeHeap<Key, Value>::TEntry::TEntry(Key k, Value v)
    : _key(k), _value(v), _node(nullptr) {}

// TNode Definitions
template <typename Key, typename Value>
QuakeHeap<Key, Value>::TNode::TNode(TEntry* e)
    : _l(nullptr), _r(nullptr), _p(nullptr), _height(0), _entry(e) {}

template <typename Key, typename Value>
QuakeHeap<Key, Value>::TNode::TNode(TNode* l, TNode* r)
    : _l(l), _r(r), _p(nullptr), _entry(nullptr) {
    if (l && r) {
        _entry = (l->_entry->_key < r->_entry->_key) ? l->_entry : r->_entry;
    } else if (l) {
        _entry = l->_entry;
    } else if (r) {
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

template <typename Key, typename Value>
QuakeHeap<Key, Value>::~QuakeHeap() {
    for (auto node : _forest) {
        delete node;
    }
}

template <typename Key, typename Value>
typename QuakeHeap<Key, Value>::TEntry* QuakeHeap<Key, Value>::makeEntry(Key key, Value value) {
    return new TEntry(key, value);
}

template <typename Key, typename Value>
void QuakeHeap<Key, Value>::updateMin() {
    if (_forest.empty()) {
        _minimum = _forest.end();
        return;
    }

    _minimum = _forest.begin();
    for (auto it = _forest.begin(); it != _forest.end(); ++it) {
        if ((*it)->_entry->_key < (*_minimum)->_entry->_key) {
            _minimum = it;
        }
    }
}

template <typename Key, typename Value>
void QuakeHeap<Key, Value>::shake(TNode* root, bool isUpdateMin) {
    if (!root) return;

    while (root) {
        if (root->_l && root->_l->_entry == root->_entry) {
            if (root->_r) {
                root->_r->_p = nullptr;
                _forest.push_back(root->_r);
                if (isUpdateMin) updateMin();
            }
            root = root->_l;
        } else if (root->_r && root->_r->_entry == root->_entry) {
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

template <typename Key, typename Value>
bool QuakeHeap<Key, Value>::empty() {
    return _size == 0;
}

template <typename Key, typename Value>
unsigned long QuakeHeap<Key, Value>::size() {
    return _size;
}

template <typename Key, typename Value>
void QuakeHeap<Key, Value>::insert(TEntry*& entry) {
    TNode* node = new TNode(entry);
    entry->_node = node;
    _forest.push_back(node);
    node->_container = --_forest.end();

    if (_minimum == _forest.end() || node->_entry->_key < (*_minimum)->_entry->_key) {
        _minimum = node->_container;
    }
    ++_size;
}

template <typename Key, typename Value>
void QuakeHeap<Key, Value>::decreaseKey(TEntry*& entry, Key newKey) {
    entry->_key = newKey;
    TNode* x = entry->_node;

    if (x->_p) {
        if (x->_p->_l == x) {
            x->_p->_l = nullptr;
        } else if (x->_p->_r == x) {
            x->_p->_r = nullptr;
        }
    }

    _forest.erase(x->_container);
    _forest.push_back(x);
    x->_container = --_forest.end();
    updateMin();
}

template <typename Key, typename Value>
typename QuakeHeap<Key, Value>::TEntry* QuakeHeap<Key, Value>::getMin() {
    return (_minimum != _forest.end()) ? (*_minimum)->_entry : nullptr;
}

template <typename Key, typename Value>
typename QuakeHeap<Key, Value>::TEntry* QuakeHeap<Key, Value>::deleteMin() {
    if (_forest.empty()) throw Exception("Heap is empty");

    RootIterator old_minimum = _minimum;
    TEntry* min_entry = (*_minimum)->_entry;
    shake(*_minimum);
    _forest.erase(old_minimum);

    std::vector<std::list<TNode*>> buckets(1 + std::ceil(log2(_size + 1)));
    std::vector<int> heights(buckets.size(), 0);

    while (!_forest.empty()) {
        TNode* node = _forest.front();
        _forest.pop_front();
        buckets[node->_height].push_back(node);
    }

    for (size_t i = 0; i < buckets.size(); ++i) {
        std::list<TNode*>& node_list = buckets[i];
        while (node_list.size() > 1) {
            TNode* a = node_list.back();
            node_list.pop_back();
            TNode* b = node_list.back();
            node_list.pop_back();
            TNode* linked = new TNode(a, b);
            a->_p = linked;
            b->_p = linked;
            buckets[i + 1].push_back(linked);
        }
    }

    int min_i = -1;
    for (size_t i = 1; i < heights.size(); ++i) {
        if (heights[i] > heights[i - 1] * _alpha) {
            min_i = i;
            break;
        }
    }

    _minimum = _forest.end();
    for (size_t i = 0; i < buckets.size(); ++i) {
        if (!buckets[i].empty()) {
            if (min_i != -1 && i > static_cast<size_t>(min_i)) {
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
