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
    return node->key;
}

int get_num_elements(heap_v heap) {
    if (heap == NULL) {
        return 0;
    }
    return heap->num_elements;
}

// creates a new heap
V_heap * create_heap() {
    V_heap *heap = (V_heap*)malloc(sizeof(*heap)); 
    heap->min = nullptr;
    heap->last = nullptr;
    heap->num_elements = 0; 
    return heap;
}

// freeing the children
void free_children (V_heap_node *node){ 
        V_heap_node *temp_child;
        if(node == nullptr){
                return;
        }
        while(node != nullptr){
                free_children(node->down);
                temp_child=node->right;
                free(node);
                node=temp_child;
        }
}

// frees a heap 
void free_heap(V_heap **heap){
        if (heap == nullptr) {
                return;
        }
        V_heap_node *minimum = (*heap)->min;
        if(minimum == nullptr){
                free(*heap);
                heap = nullptr;
                return;
        }
        V_heap_node *temp = minimum->right;
        V_heap_node *next;

        while(temp != minimum){
                next = temp->right;
                free_children(temp->down);
                free(temp);
                temp = next;
        }
        free_children(minimum->down);
        free(minimum);
        free(*heap);
        heap=nullptr;
}
/* inserts a node into the root list with the given key and value. */        
V_heap_node *insert(V_heap *heap, int value, int key){

        V_heap_node *node = (V_heap_node* ) malloc(sizeof(*node));
        node->left=nullptr;
        node->right=nullptr;
        node->down= nullptr;
        node->data=value;
        node->key=key;
        node->rank=0;  

        if (heap == nullptr) {
                std::cout << "Incorrect initialization of the heap! " << std::endl;
                return node;
        }    

        // if it is the first node
        if (heap->min == nullptr){
                heap->min = node;
                heap->last = node;
                node->right = node;
        }
        // if it's not the first node
        else {
                node->right = heap->min->right;
                heap->min->right = node;

                if (heap->min->right->right == heap->min){
                        heap->last=heap->min->right;
                }
                if(value < heap->min->data){
                        heap->last=heap->min;
                        heap->min=node;
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
int V_heap_min(V_heap *heap){
        if (heap == nullptr){
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
        if (node->down == nullptr){
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

// extracts the minimum node
int extract_min(V_heap *heap){

        if(heap == nullptr){
                return -1;
        }
        int minimum= heap->min->key;
        v_delete_min(heap);

        consolidate(heap);
        
        V_heap_node *temp, *previous;
        V_heap_node *Vmin = nullptr;
        V_heap_node *last = nullptr;
        int current_min;

        temp = heap->min->right;
        previous = heap->min;
        last = heap->last;
        Vmin = heap->min;
        // update min accordingly
        while(temp != heap->min){
                current_min = Vmin->data;
                if (temp->data < current_min){
                        Vmin = temp;
                        last = previous;
                }
                previous = temp;
                temp = temp->right;
        }
        heap->last = last;
        heap->min = Vmin;

        return minimum;
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
void consolidate(V_heap *heap){
        /*compute the number of nodes in the heaplist*/
        int size=ceil(log(heap->num_elements)/LN2)*6+4;
        if(size==0){
                return;
        }
        /* create a list to store the nodes during the reformating process */ 
        // V_heap_node **heaplist= calloc(size, sizeof(*heaplist));
        V_heap_node **heaplist = (V_heap_node**)calloc(size, sizeof(*heaplist)); // Cast to V_heap_node**

        V_heap_node *current=heap->min, *previous=heap->last;
        int rank = current->rank,i;
        /* loop as long as the current node isn't in the heaplist*/
        while((current!=(heaplist[rank*4])) && (current!=(heaplist[rank*4+2]))){
                /* if there is space in the heaplist at currents rank of rank +1
                 * put it there */
                if (heaplist[rank*4]==nullptr){
                        heaplist[rank*4]=current;
                        heaplist[rank*4+1]=previous; 
                        previous=current;
                        current=current->right;
                }
                else if (heaplist[rank*4+2]==nullptr){
                        heaplist[rank*4+2]=current;
                        heaplist[rank*4+3]=previous;
                        previous=current;
                        current=current->right;
                }
                /* if there isn't combine those three then set four spot in the
                 * heaplist with the former rank of current to nullptr. use the
                 * root of the new heap/tree as the new current */
                else {
                        current=update_pointers(heaplist,previous,current);
                        for(i=0;i<4;i++){
                                heaplist[4*rank+i]=nullptr;
                        }
                        previous=current->left;
                        current->left=nullptr;
                }
                /*updatea rank if needed*/
                rank=current->rank;
        }
        /* temporarily make the last node too be refomated the new min */
        heap->min=current;
        heap->last=previous;
        free(heaplist);
}

// removes the minimum from the heap and pushes children to the root list
void v_delete_min(V_heap *heap){
        V_heap_node *minimum = heap->min;
        V_heap_node *last_child = nullptr;
        V_heap_node *previous_root=heap->last;

        // set all of the childrens left pointers to nullptr
        while (minimum->down != nullptr){
                last_child = minimum->down;
                minimum->down->left=nullptr;
                minimum->down = minimum->down->right;
        }
        if (previous_root == minimum){
                // if no children
                if(last_child == nullptr){
                        heap->min = nullptr;
                }
                else {  // push children to the root list
                        heap->min = minimum->down;
                        last_child->right = minimum->down;
                        heap->last = last_child;
                }       
        }
        // other roots
        else{   // if no children
                if (last_child == nullptr) {

                        previous_root->right = minimum->right;
                }
                // if there are children
                else {   
                        previous_root->right = minimum->down;
                        last_child->right = minimum->right;
                }
                heap->min = previous_root->right;
        }
        heap->num_elements--;
        free(minimum);
}       