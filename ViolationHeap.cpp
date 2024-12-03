#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include "ViolationHeap.h"
#include <math.h>

typedef struct V_heap_node {
        struct V_heap_node* left;
        struct V_heap_node* right;
        struct V_heap_node* down;
        int rank;
        int key;
        int data;
} V_heap_node;

typedef struct V_heap{
        V_heap_node* min;
        V_heap_node* last;
        int num_elements;
} V_heap;

#define LN2 .69314718

void cut (V_heap *heap, V_heap_node *node);

void propagate(V_heap_node *node, int is_active);

int find_rank(V_heap_node *node);

void v_delete_min(V_heap *heap);

void consolidate(V_heap *heap);

void decrease_key(V_heap *heap, V_heap_node *node, int value);

int extract_min(V_heap *heap);

int get_key(node_v node) {
    if (!node) {
        fprintf(stderr, "Invalid node in get_key\n");
        return -1;
    }
    return node->key;
}

int get_num_elements(heap_v heap) {
    if (!heap) {
        fprintf(stderr, "Invalid heap in get_num_elements\n");
        return -1;
    }
    return heap->num_elements;
}

void validate_heap_structure(V_heap* heap) {
    if (!heap || !heap->min) return;

    V_heap_node* current = heap->min;
    do {
        if (current->down && current->down->left == nullptr) {
            fprintf(stderr, "Invalid child list: x->down->left is null for node with key %d\n", current->key);
        }
        current = current->right;
    } while (current != heap->min);
}


void validate_heap(V_heap* heap, const char* context) {
    if (!heap) {
        fprintf(stderr, "Heap is NULL: %s\n", context);
        return;
    }

    printf("Validating heap: %s\n", context);

    if (!heap->min) {
        if (heap->num_elements != 0) {
            fprintf(stderr, "Heap size mismatch: expected 0, but num_elements=%d\n", heap->num_elements);
        }
        return;
    }

    int count = 0;
    V_heap_node* current = heap->min;
    do {
        if (!current) {
            fprintf(stderr, "NULL node in root list at position %d: %s\n", count, context);
            return;
        }

        printf("Node %d: key=%d, data=%d, left=%p, right=%p\n",
               count, current->key, current->data, current->left, current->right);

        // Check for cycles or invalid pointers
        if (current->right == current) {
            fprintf(stderr, "Cycle detected in root list at node %d: %s\n", count, context);
            return;
        }
        current = current->right;
        count++;
    } while (current != heap->min);

    if (count != heap->num_elements) {
        fprintf(stderr, "Heap size mismatch: expected %d, found %d (context: %s)\n",
                heap->num_elements, count, context);
    }
}


void debug_root_list(V_heap* heap) {
    if (!heap || !heap->min) {
        printf("Root list is empty\n");
        return;
    }

    printf("Root list:\n");
    V_heap_node* current = heap->min;
    int count = 0;
    do {
        if (!current) {
            fprintf(stderr, "NULL node in root list at position %d\n", count);
            break;
        }
        printf("Node %d: key=%d, data=%d, left=%p, right=%p\n",
               count, current->key, current->data, current->left, current->right);
        current = current->right;
        count++;
    } while (current != heap->min);

    printf("Root list traversal complete: count = %d\n", count);
}


V_heap* create_heap() {
    V_heap* heap = (V_heap*)malloc(sizeof(*heap));
    if (!heap) {
        perror("Heap allocation failed");
        exit(EXIT_FAILURE);
    }
    heap->min = nullptr;
    heap->last = nullptr;
    heap->num_elements = 0;
    return heap;
}

void free_children(V_heap_node* node) {
    V_heap_node* temp_child;
    while (node) {
        free_children(node->down);
        temp_child = node->right;
        free(node);
        node = temp_child;
    }
}

// frees a heap 
void free_heap(V_heap** heap) {
    if (!heap || !(*heap)) return;
    V_heap_node* minimum = (*heap)->min;
    if (!minimum) {
        free(*heap);
        *heap = nullptr;
        return;
    }
    V_heap_node* temp = minimum->right;
    V_heap_node* next;

    while (temp != minimum) {
        next = temp->right;
        free_children(temp->down);
        free(temp);
        temp = next;
    }
    free_children(minimum->down);
    free(minimum);
    free(*heap);
    *heap = nullptr;
}
/* inserts a node into the root list with the given key and value. */        
V_heap_node* insert(V_heap* heap, int value, int key) {
    V_heap_node* node = (V_heap_node*)malloc(sizeof(*node));
    node->left = nullptr;
    node->right = nullptr;
    node->down = nullptr;
    node->data = value;
    node->key = key;
    node->rank = 0;

    if (!heap) {
        fprintf(stderr, "Heap not initialized\n");
        return node;
    }

    if (!heap->min) {
        heap->min = node;
        heap->last = node;
        node->right = node;
    } else {
        node->right = heap->min->right;
        heap->min->right = node;

        if (heap->min->right->right == heap->min) {
            heap->last = heap->min->right;
        }
        if (value < heap->min->data) {
            heap->last = heap->min;
            heap->min = node;
        }
    }
    heap->num_elements++;
    return node;
}

// merges two heaps
V_heap *union_heaps( V_heap *heap1, V_heap *heap2){

        if (heap1 == nullptr || heap2 == nullptr) {
                return nullptr;
        }
        // union by rank
        V_heap *large_rank, *small_rank;
        if (heap1->min->data >= heap2->min->data) {
                large_rank = heap1;
                small_rank = heap2;
        }
        else {
                large_rank = heap2;
                small_rank = heap1;
        }
        // update last pointers accordingly
        small_rank->last->right = large_rank->min;
        large_rank->last->right = small_rank->min;

        small_rank->last = large_rank->last;
        small_rank->num_elements += large_rank->num_elements;

        free(large_rank);
        return small_rank;
}

// returns the key of the minimum element in the heap
int v_heap_min(heap_v heap) {
    if (!heap || !heap->min) {
        fprintf(stderr, "Heap is empty or not initialized\n");
        return -1;
    }
    return heap->min->key;
}

// decreases key, cuts the child (if necassary)
void decrease_key(V_heap *heap, V_heap_node *node, int value){
        V_heap_node *last = heap->min;

        // invalid value check
        if (value > node->data) {
                return;
        }
        else {
                node->data = value;
        }

        // below are all the conditions to avoid a cut
        if (node->left == nullptr){
                // update min accordingly
                if (value < heap->min->data){
                        while(last->right!=node){
                                last=last->right;
                        }
                        heap->last = last;
                        heap->min = node;
                }
                node->rank = find_rank(node);
                return;
        }
        int is_active = 0;
        if ((node->left != nullptr) && (node->left->down == node)) {
                is_active = 1;
        }
        else if ((((node->left != nullptr) && (node->left->left!=nullptr))
                 && (node->left->left->down !=nullptr)) 
                 && (node->left->left->down->right==node)) {
                      is_active = 1;  
                 }
        if (is_active) {
                return;
        }
        // cut is necessary
        cut(heap, node);
}

// cuts the node (if necessary) 
void cut (V_heap *heap, V_heap_node *node){
        V_heap_node *temp;
        V_heap_node *parent=nullptr;

        int is_active = 0;
        if ((node->left!=nullptr) && (node->left->down == node)) {
                is_active = 1;
                parent=node->left;
        }
        else if ((((node->left != nullptr) && (node->left->left!=nullptr))
                 && (node->left->left->down !=nullptr)) 
                 && (node->left->left->down->right==node)) {
                      is_active = 2; 
                      parent=node->left->left; 
                 }


        // push the children to the root list if any
        if (node->down != nullptr){ // check this line 
                if (is_active == 1) {
                        node->left->down = node->right;
                }
                else{
                        node->left->right = node->right;
                }
                if (node->right != nullptr) {
                        node->right->left = node->left;
                }        
        }
        // Replacing a parent with a child
        else{
                if (node->down->right == nullptr){ // one child
                        temp = node->down;
                        node->down = nullptr;
                }
                else {  // cut the larger rank child
                        if (node->down->rank > node->down->right->rank) {
                                node->down->right->left=node;
                                temp=node->down;
                                node->down=temp->right;
                        }
                        else {
                                temp=node->down->right;
                                node->down->right=temp->right;
                                if(temp->right!=nullptr){
                                        temp->right->left=node->down;
                                }
                        }
                }
                temp->left = node->left;
                temp->right = node->right;
                // update the parent pointers accordingly
                if (is_active == 1){
                        node->left->down=temp;
                }
                else {
                        node->left->right=temp;
                }
                if(node->right != nullptr){
                        node->right->left=temp;
                }
        }
        // push node into the root list
        node->rank = find_rank(node);
        node->right = heap->min->right;
        heap->min->right = node;
        node->left = nullptr;

        if(heap->min->right->right == heap->min){
                heap->last = heap->min->right;
        }
        // update min accordingly
        if(node->data < heap->min->data){
                heap->last = heap->min;
                heap->min = node;
        }
        // if this node is active propagate
        if(is_active){
                propagate(parent, is_active);
        }
}

// finding the rank
int find_rank(V_heap_node *node){
        int r1=-1, r2=-1;
        if(node->down != nullptr){
                r1 = node->down->rank;
                if(node->down->right != nullptr){
                        r2 = node->down->right->rank;
                }
        }
        return round(((r1+r2)/2)+1)+1;
}

// propagate the rank change up the heap
void propagate(V_heap_node *node, int is_active){
        int rank;
        if (node == nullptr){
                return;
        }
        rank = find_rank(node);
        // if active, propagate the rank update
        if (rank < node->rank){
                node->rank = rank;
                if (is_active == 1){
                        propagate(node->left, is_active);
                }
                else {
                        propagate(node->left->left, is_active);
                }
        }
        else {
                node->rank = rank; 
        }
}

void debug_heap_state(const V_heap *heap, const char *message) {
    printf("%s\n", message);
    printf("Heap: num_elements = %d\n", heap->num_elements);
    if (heap->min) {
        printf("Heap min: key = %d, data = %d, rank = %d\n",
               heap->min->key, heap->min->data, heap->min->rank);
    } else {
        printf("Heap min is NULL\n");
    }
    if (heap->last) {
        printf("Heap last: key = %d\n", heap->last->key);
    } else {
        printf("Heap last is NULL\n");
    }
}



// extracts the minimum node
int extract_min(V_heap* heap) {

    std::cout << std::endl << "in extract min " << std::endl;
    if (!heap || heap->num_elements == 0 || heap->min == nullptr) {
        fprintf(stderr, "Cannot extract min: Heap is empty\n");
        // return -1;
    }
    if (get_num_elements(heap)>0 and heap->min==nullptr){
        std::cout << "Invalid heap structure! min is null, but heap has elements. " << std::endl;
        std::cout.flush();
        return -1;
    }

    int minimum_key = heap->min->key;

    std::cout << "helo im in delete min";
    v_delete_min(heap);
    std::cout << ", gbye im out " << std::endl;

    if (heap->num_elements > 0) {
        std::cout << "helo im in consolidate ";
        std::cout.flush();
        validate_heap_structure(heap);
        consolidate(heap);
        validate_heap_structure(heap);
        std::cout << ", gbye im out " << std::endl;
    } else {
        std::cout <<  "Heap is now empty" << std::endl;
        std::cout.flush();
    }
    std::cout << "done" << std::endl ;
    std::cout.flush();

    return minimum_key;
}




// update the nodes that point to deleted node pointers
static inline void change_previous(V_heap_node **heaplist,V_heap_node* node){
        int rank= node->right->rank;
        if ( heaplist[rank*4] == node->right){
                heaplist[rank*4+1]=node;
        }
        else if ( heaplist[rank*4+2] == node->right){
                heaplist[rank*4+3]=node;
        }
}


V_heap_node *update_pointers(V_heap_node **heaplist,V_heap_node* previous_a, V_heap_node *node_a){
        V_heap_node *parent=nullptr, *first=nullptr, *second=nullptr, *previous2=nullptr,
               *previous1=nullptr, *node_b, *node_c, *previous_b,
               *previous_c, *previous_p = nullptr, *temp;
        int rank=node_a->rank;

        node_b=heaplist[rank*4];
        node_c=heaplist[rank*4+2];
        previous_b=heaplist[rank*4+1];
        previous_c=heaplist[rank*4+3];

        if (node_a == NULL || node_b == NULL || node_c == NULL) {
                fprintf(stderr, "Invalid nodes in update_pointers\n");
                return NULL;
        }


        if (node_a->data >= node_b->data) {
                second=node_a;
                previous2=previous_a;
                parent= (node_c->data >= node_b->data) ? node_b : node_c;
                previous_p= (node_c->data >= node_b->data) ? previous_b : previous_c;
                first = (node_c->data < node_b->data) ? node_b : node_c;
                previous1 = (node_c->data < node_b->data) ? previous_b : previous_c;
        }
        else {
                second=node_b;
                previous2=previous_b;
                parent= (node_c->data >= node_a->data) ? node_a : node_c;
                previous_p= (node_c->data >= node_a->data) ? previous_a : previous_c;
                first = (node_c->data < node_a->data) ? node_a : node_c;
                previous1 = (node_c->data < node_a->data) ? previous_a : previous_c;
        }

        if(second==previous1){
                previous2->right=first->right;
                change_previous(heaplist,previous2);

                if(previous2->right==parent){
                        previous_p=previous2;
                }
        }
        else{

                previous2->right = second->right;
                change_previous(heaplist,previous2);
                if(previous2->right==parent){
                        previous_p=previous2;
                }
                previous1->right = first->right;
                change_previous(heaplist,previous1);
                if(previous1->right==parent){
                        previous_p=previous1;
                }
        }
        first->right=second;
        second->right=parent->down;
        second->left=first;
        first->left=parent;
        if (parent->down!=nullptr){
                if(parent->down->right!=nullptr){
                        if (parent->down->right->rank > parent->down->rank){
                                temp=parent->down->right;
                                parent->down->right=temp->right;
                                parent->down->left=temp;
                                temp->right=parent->down;
                                temp->left=parent;
                                parent->down=temp;
                        }
                }
                parent->down->left=second;
        }
        parent->down = first;
        parent->rank=parent->rank+1; 
        parent->left=previous_p;
        return parent;
}

// run consolidate
void consolidate(V_heap* heap) {
    if (!heap || heap->num_elements == 0) return;

    validate_heap_structure(heap);

    int max_rank = (int)(log2(heap->num_elements)) + 1;
    V_heap_node* ranks[max_rank];
    memset(ranks, 0, sizeof(ranks));

    V_heap_node* current = heap->min;
    do {
        V_heap_node* x = current;
        int rank = x->rank;

        while (ranks[rank] != NULL) {
            V_heap_node* y = ranks[rank];         
            if (x->key > y->key) {              
                V_heap_node* temp = x;
                x = y;
                y = temp;
            }
            // Link y under x
            if (y->left){
                if (y->left->right != nullptr) {
                    y->left->right = y->right;
                }
            }
            if (y->right){ 
                if (y->right->left) {
                    y->right->left = y->left;
                }
            }
            if (y->right and y->left) {
                y->left = y->right = NULL;
            }
            if (x->down) {
                if (x->down->left && y->left){ 
                    y->left = x->down->left;
                    x->down->left = y;
                    if (x->down->left->right){
                        x->down->left->right = y;
                    }
                }
            } else {
                x->down = y;
                y->right = y->left = y;
            }
            x->rank++;
            ranks[rank] = NULL;
            rank++;
        }

        ranks[rank] = x;
        current = current->right;
    } while (current != heap->min and current != nullptr);
    heap->min = NULL;
    heap->last = NULL;

    validate_heap_structure(heap);

    for (int i = 0; i < max_rank; i++) {
        if (ranks[i]) {
            if (!heap->min) {
                heap->min = ranks[i];
                heap->last = ranks[i];
                ranks[i]->right = ranks[i];
                ranks[i]->left = ranks[i];
            } else {
                ranks[i]->right = heap->min;
                ranks[i]->left = heap->last;
                heap->last->right = ranks[i];
                heap->last = ranks[i];
            }
            if (ranks[i]->key < heap->min->key) {
                heap->min = ranks[i];
            }
        }
    }

    validate_heap_structure(heap);
    if (get_num_elements(heap)>0 and heap->min==nullptr){
        std::cout << "Invalid heap structure! min is null. " << std::endl;
        std::cout.flush();
    }
}
void v_delete_min(V_heap* heap) {
    if (!heap || !heap->min) {
        fprintf(stderr, "Invalid heap or min node in v_delete_min\n");
        return;
    }

    V_heap_node* minimum = heap->min;

    // printf("Removing min node: key=%d, data=%d\n", minimum->key, minimum->data);
    if (minimum->down) {
        // printf("Minimum has children, adding to root list\n");
        V_heap_node* child = minimum->down;
        do {
            // printf("Adding child to root list: key=%d, data=%d\n", child->key, child->data);
            child->left = heap->last;
            heap->last->right = child;
            heap->last = child;
            child = child->right;
        } while (child != minimum->down and child != nullptr);
    }

    // Update root list pointers
    if (minimum->right == minimum) {  // Single node case
        heap->min = nullptr;
        heap->last = nullptr;
    } else {
        if (minimum->left) minimum->left->right = minimum->right;
        if (minimum->right) minimum->right->left = minimum->left;

        heap->min = minimum->right;
        if (heap->last == minimum) heap->last = minimum->left;
    }

    heap->num_elements--;
    free(minimum);
}
