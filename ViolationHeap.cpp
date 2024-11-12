#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include "ViolationHeap.h"
#include <math.h>
typedef struct v_node {
        struct v_node* left;
        struct v_node* right;
        struct v_node* down;
        int rank;
        int vertex;
        int data;
} v_node;

typedef struct v_heap{
        v_node* min;
        v_node* last;
        int num_elem;
} v_heap;

#define LN2 .69314718

v_node *new_node(int value, int ver);

void free_children(v_node *node);

static inline int first_active(v_node *node);

static inline int second_active(v_node *node);

void cut (v_heap *heap, v_node *node);

void propagate(v_node *node);

int find_rank(v_node *node);

void v_delete_min(v_heap *heap);

v_node *three_way_combine(v_node **heaplist,v_node *previous_a, v_node *node_a);

static inline void change_previous(v_node **heaplist,v_node* node);

void reformat_heap(v_heap *heap);

void reset_min(v_heap *heap);

void print_children (v_node *node);

v_node *find_parent(v_node *node);
/* allocates space for a new violation heap*/
v_heap *v_heap_new() {
    v_heap *heap = (v_heap*)malloc(sizeof(*heap)); // Explicitly cast malloc's return to v_heap*
    heap->min = NULL;
    heap->last = NULL;
    heap->num_elem = 0; 
    return heap;
}
/* frees a violation heap by freeing the root list and calling a helper function
to free the children of those nodes */
void v_heap_free(v_heap **heap){
        if(!(heap && *heap)){
                return;
        }
        v_node *minimum = (*heap)->min;
        if(minimum==NULL){
                free(*heap);
                heap=NULL;
                return;
        }
        v_node *temp = minimum->right;
        v_node *next;
        /* loops through the root list freeing children of the current node then
         * free the node itself*/
        while(temp != minimum){
                next=temp->right;
                free_children(temp->down);
                free(temp);
                temp=next;
        }
        free_children(minimum->down);
        free(minimum);
        free(*heap);
        heap=NULL;
}
/*recursive helper function to help free the children of a node */
void free_children(v_node *node){
        v_node *temp;
        if(node== NULL){
                return;
        }
        /* loops through the root list freeing children of the current node then
         * free the node itself*/
        while(node != NULL){
                free_children(node->down);
                temp=node->right;
                free(node);
                node=temp;
        }
}
/* inserts a node into the root list with the given vertex and value. */        
v_node *v_heap_insert(v_heap *heap, int value, int ver){
        v_node *node;
        /* creates a new node with all values set to there intial states*/
        node=new_node(value,ver);
        if(!(heap)){
                free(node);
                return NULL;
        }
        /*if the heap is empty make it the min and last and set its right to itself*/
        if (heap->min==NULL){
                heap->min=node;
                heap->last=node;
                node->right=node;
        }
        /* put the node right of min otherwise*/
        else {
                node->right = heap->min->right;
                heap->min->right = node;
                /* if tere is only one node in the heap then right of the min is
                 * also the last node set it as such*/
                if (heap->min->right->right==heap->min){
                        heap->last=heap->min->right;
                }
                /*adjust min if necassary*/
                if(value < heap->min->data){
                        heap->last=heap->min;
                        heap->min=node;
                }
        }
        heap->num_elem = heap->num_elem + 1;
        return node;
}
/*allocates space for a new node and initalizes its values */
v_node *new_node(int value, int ver){
        v_node *newNode = (v_node* ) malloc(sizeof(*newNode));
        newNode->left=NULL;
        newNode->right=NULL;
        newNode->down= NULL;
        newNode->data=value;
        newNode->vertex=ver;
        newNode->rank=0;      
        return newNode;
} 
/* merges the two given heaps and retruns the merged heap */
v_heap *merge_v_heaps( v_heap *heap1, v_heap *heap2){
        if ((!(heap1))||(!(heap2))){
                return NULL;
        }
        /* make big the heap with the larger min value and small the smaller min
         * value*/
        v_heap *big, *small;
        big = (heap1->min->data >= heap2->min->data) ? heap1:heap2;
        small = (heap1->min->data < heap2->min->data) ? heap1:heap2;
        /* wire the node of big between small last and small min.*/
        small->last->right = big->min;
        big->last->right=small->min;
        small->last=big->last;
        small->num_elem=small->num_elem+big->num_elem;
        free(big);
        return small;
        
        
}
/* returns the vertex of the minimum element in the heap*/
int v_heap_min(v_heap *heap){
        if ((!(heap)) ||(!(heap->min))){
                return -1;
        }
        return heap->min->vertex;
}
/*checks to see if a cut is necassary and if it is cut the node*/
void v_heap_decrease_key(v_heap *heap, v_node *node, int value){
        v_node *last=heap->min;
        if((!(heap)) ||(!(node))){
                return;
        }
        node->data=value;
        /*if the node is in the root list no cut is necassary*/
        if (node->left==NULL){
                /* change min if necassary*/
                if (value < heap->min->data){
                        /* this could take O(n) time amortized but it is a very
                        rare step */
                        while(last->right!=node){
                                last=last->right;
                        }
                        heap->last=last;
                        heap->min=node;
                }
                node->rank=find_rank(node);
                return;
        }
        /* if the first active child and still smaller than its parent return*/
        if (first_active(node)){
                if (node->left->data <= node->data){
                        return;
                }
        }
        /* if the second active child and still smaller than its parent return*/
        else if(second_active(node)){
                if (node->left->left->data <= node->data){
                        return;
                }
        }
        cut(heap, node);
}
/* checks to see if the given node is the first active node*/
static inline int first_active(v_node *node){
        return ((node->left!=NULL) && (node->left->down == node));
}

/* checks to see if the given node is the second active node*/
static inline int second_active(v_node *node){
        return ((((node->left != NULL) && (node->left->left!=NULL))
                 && (node->left->left->down !=NULL)) 
                 && (node->left->left->down->right==node));
}
/* cuts the node from the tree and calls a helper ot propagate the cut upward if
 * needed */
void cut (v_heap *heap, v_node *node){
        v_node *temp;
        v_node *parent=NULL;
        int first=0, not_last=0, second=0;
        /* define the parent and whether it is first or second child*/
        if(first_active(node)){
                first =1;
                parent=node->left;
        }
        else if (second_active(node)){
                second=1;
                parent=node->left->left;
        }
        /* since this is gauranteed to be a child the rightmost node right
         * pointer will point to NULL */
        not_last= (node->right != NULL);
        /* if the node does not have children they don't need to be rewired  so
         * simply unwire the node from its current postion*/
        if (node->down == NULL){
                if (first){
                        node->left->down=node->right;
                }
                else{
                        node->left->right=node->right;
                }
                if (not_last){
                        node->right->left = node->left;
                }
                
        }
        /* if does have children find the child to put in its place*/
        else{
                /* if it has only one child make that the node that will takes
                 * the nodes place in the nodes siblings list*/
                if (node->down->right==NULL){
                        temp=node->down;
                        node->down=NULL;
                }
                else {
                        /*otherwise make temp the active child with larger rank*/
                        if (node->down->rank > node->down->right->rank){
                                node->down->right->left=node;
                                temp=node->down;
                                node->down=temp->right;
                        }
                        else{
                                temp=node->down->right;
                                node->down->right=temp->right;
                                if(temp->right!=NULL){
                                        temp->right->left=node->down;
                                }
                        }
                }/*wire temp in*/
                temp->left=node->left;
                temp->right=node->right;
                /* if node was the first child change the down pointer of the
                 * parent*/
                if (first){
                        node->left->down=temp;
                }
                else{
                        node->left->right=temp;
                }
                /* if node was not the last node change its rights node left
                 * pointer*/
                if(not_last){
                        node->right->left=temp;
                }
        }
        /* wire node into the root list right of the min node and change min if
         * needed and recalculate its rank*/
        node->rank=find_rank(node);
        node->right=heap->min->right;
        heap->min->right=node;
        node->left=NULL;
        /* same as for insert that when inserting into the root list which only
         * had one node the inserted node has to become last.*/
        if(heap->min->right->right==heap->min){
                heap->last=heap->min->right;
        }
        if(node->data < heap->min->data){
                heap->last=heap->min;
                heap->min=node;
        }
        /* if this node is active propagate*/
        if(first || second){
                propagate(parent);
        }

}
/* recalculate the rank of the given node*/
int find_rank(v_node *node){
        int r1=-1, r2=-1;
        if(node->down!=NULL){
                r1=node->down->rank;
                if(node->down->right!=NULL){
                        r2=node->down->right->rank;
                }
        }
        return ceil(((r1+r2)/2))+1;
        
}
/* propagate the rank change up the heap tree until the one of the end
 * conditions is met*/
void propagate(v_node *node){
        int rank;
        if (node==NULL){
                return;
        }
        rank=find_rank(node);
        /* if the rank went down change the rank then if it is one of the two
         * active children propagate the rank update up*/
        if (rank < node->rank){
                node->rank=rank;
                if (first_active(node)){
                        propagate(node->left);
                }
                else if (second_active(node)){
                        propagate(node->left->left);
                }
        }
        else {
                node->rank=rank; 
        }
}
/* extracts the minimum node from the heap and the reformats the heap back to
 * its proper structure */
int v_extract_min(v_heap *heap){
        /*returns -1 if the heap is empty */
        if(!((heap!=NULL) && (heap->min!=NULL))){
                return -1;
        }
        int minimum= heap->min->vertex;
        /* remove the minimum node*/
        v_delete_min(heap);
        /* if that was not the last node in the heap remofat the heap and find
         * the new minimum */
        if(heap->min!=NULL){
                reformat_heap(heap);
                reset_min(heap);
        } 
        /* return the vertex of the minimum element*/
        return minimum;
}
/* finds the new minimum of the heap*/
void reset_min(v_heap *heap){
        v_node *temp,*previous, *min=NULL, *last=NULL;
        int datam;
        int datat;
        temp=heap->min->right;
        previous=heap->min;
        last=heap->last;
        min=heap->min;
        /* loop through the root list looking for the new minimum. If the current
         * node is less the current minimum make minimum the current node and
         * last the node before the new minimum*/
        while(temp != heap->min){
                datam=min->data;
                datat=temp->data;
                if (datat < datam){
                        min=temp;
                        last=previous;
                }
                previous=temp;
                temp=temp->right;
        }
        heap->last=last;
        heap->min=min;
}
/* restore the heap to the desired shape*/
void reformat_heap(v_heap *heap){
        /*compute the number of nodes in the heaplist*/
        int size=ceil(log(heap->num_elem)/LN2)*6+4;
        if(size==0){
                return;
        }
        /* create a list to store the nodes during the reformating process */ 
        // v_node **heaplist= calloc(size, sizeof(*heaplist));
        v_node **heaplist = (v_node**)calloc(size, sizeof(*heaplist)); // Cast to v_node**

        v_node *current=heap->min, *previous=heap->last;
        int rank = current->rank,i;
        /* loop as long as the current node isn't in the heaplist*/
        while((current!=(heaplist[rank*4])) && (current!=(heaplist[rank*4+2]))){
                /* if there is space in the heaplist at currents rank of rank +1
                 * put it there */
                if (heaplist[rank*4]==NULL){
                        heaplist[rank*4]=current;
                        heaplist[rank*4+1]=previous; 
                        previous=current;
                        current=current->right;
                }
                else if (heaplist[rank*4+2]==NULL){
                        heaplist[rank*4+2]=current;
                        heaplist[rank*4+3]=previous;
                        previous=current;
                        current=current->right;
                }
                /* if there isn't combine those three then set four spot in the
                 * heaplist with the former rank of current to null. use the
                 * root of the new heap/tree as the new current */
                else {
                        current=three_way_combine(heaplist,previous,current);
                        for(i=0;i<4;i++){
                                heaplist[4*rank+i]=NULL;
                        }
                        previous=current->left;
                        current->left=NULL;
                }
                /*updatea rank if needed*/
                rank=current->rank;
        }
        /* temporarily make the last node too be refomated the new min */
        heap->min=current;
        heap->last=previous;
        free(heaplist);
}
/* performs a three-way combine on the given nodes*/
v_node *three_way_combine(v_node **heaplist,v_node* previous_a, v_node *node_a){
        v_node *parent=NULL, *first=NULL, *second=NULL, *previous2=NULL,
               *previous1=NULL, *node_b, *node_c, *previous_b,
               *previous_c, *previous_p = NULL, *temp;
        int rank=node_a->rank;
        /*extract the needed information from the heaplist*/
        node_b=heaplist[rank*4];
        node_c=heaplist[rank*4+2];
        previous_b=heaplist[rank*4+1];
        previous_c=heaplist[rank*4+3];
        /* set parent first and second to the nodes that have the correct relative
         * values */
        if(node_a->data >= node_b->data){
                second=node_a;
                previous2=previous_a;
                parent= (node_c->data >= node_b->data) ? node_b : node_c;
                previous_p= (node_c->data >= node_b->data) ? previous_b : previous_c;
                first = (node_c->data < node_b->data) ? node_b : node_c;
                previous1 = (node_c->data < node_b->data) ? previous_b : previous_c;
        }
        else{
                second=node_b;
                previous2=previous_b;
                parent= (node_c->data >= node_a->data) ? node_a : node_c;
                previous_p= (node_c->data >= node_a->data) ? previous_a : previous_c;
                first = (node_c->data < node_a->data) ? node_a : node_c;
                previous1 = (node_c->data < node_a->data) ? previous_a : previous_c;
        }
        /* if second is the node before first wire previous2 to the node after
         * first. Necassary so the previous2 right doesn't point to first */
        if(second==previous1){
                previous2->right=first->right;
                change_previous(heaplist,previous2);
                //update the parent previous if needed
                if(previous2->right==parent){
                        previous_p=previous2;
                }
        }
        else{
        /* else wire the right of the previous node the nexts of there
         * respective nodes */
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
        /* wire them together*/
        first->right=second;
        second->right=parent->down;
        second->left=first;
        first->left=parent;
        /*if parent has at least one child*/
        if (parent->down!=NULL){
                /*if parent has at least two children */
                if(parent->down->right!=NULL){
                        /* if the second child has a greater rank than the first
                         * child switch them (i.e. first child becomes second
                         * and second becomes first)*/
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
/* as removing a node of the root list might have made it no longer the previous
 * of another node find that node and make the given node the new previous.
 * function is given the previous of the node that was removed from the root
 * list. */
static inline void change_previous(v_node **heaplist,v_node* node){
        int rank= node->right->rank;
        if ( heaplist[rank*4] == node->right){
                heaplist[rank*4+1]=node;
        }
        else if ( heaplist[rank*4+2] == node->right){
                heaplist[rank*4+3]=node;
        }
}
/* removes the minimum from the heap wiring in its children */
void v_delete_min(v_heap *heap){
        v_node *minimum=heap->min;
        v_node *temp = minimum->down;
        v_node *last_child=NULL;
        v_node *previous_root=heap->last;
        /* set all of the childrens left pointers to null as no root list node
         * has left pointers*/
        while(temp!=NULL){
                last_child=temp;
                temp->left=NULL;
                temp=temp->right;
        }
        /* if minimum is the only root in the heap*/
        if(previous_root==minimum){
                /* if minimum has no children*/
                if(last_child==NULL){
                        // theN new heap is empty
                        heap->min=NULL;
                }
                else{
                        // If it has children then that children list becomes
                        // the new root list wire the last node to the first 
                        // node   
                        heap->min=minimum->down;
                        last_child->right=minimum->down;
                        heap->last=last_child;
                }       
        }
        // if there are other root list nodes
        else{  
                //if there are no childern
                if(last_child==NULL){
                        // then just update previous_roots pointer to skip over
                        // the former minimum.
                        previous_root->right=minimum->right;
                }
                // if there are childern
                else{   
                        // wire them into the root list
                        previous_root->right = minimum->down;
                        last_child->right = minimum->right;
                }
                heap->min=previous_root->right;
        }
        heap->num_elem = heap->num_elem -1;
        free(minimum);
}       

/* prints the ro0t list of a heap calling a helper to print the children at each
 * level */
void v_print_heap(v_heap *heap){
        if(!(heap)){
                return;
        }
        v_node *minimum = heap->min;
        if(minimum==NULL){
                fprintf(stdout,"\nn");
                return;
        }
        v_node *temp = minimum->right;
        while(temp != minimum){
                if (temp==heap->last){
                        fprintf(stdout,"last: ");
                }
                fprintf (stdout, "vertex: %d rank: %d value: %d ",
                         temp->vertex,temp->rank,temp->data);
                fprintf(stdout, "->: %d ",temp->right->vertex);
                fprintf(stdout, " root\n");
                if (temp->left != NULL){
                        fprintf(stdout, "THIS IS WRONG: <-:%d", temp->left->vertex);
                }
                else{
                }
                if (temp->down ==NULL){
                }
                else{
                        fprintf(stdout, "representative child: %d \n\n", temp->down->vertex);
                }
                print_children(temp->down);
                temp=temp->right;
        }
        if (temp==heap->last){
                fprintf(stdout,"last: ");
        }
        fprintf(stdout,"minimum: ");
        fprintf (stdout, "vertex: %d with rank: %d and value: %d ",
                 minimum->vertex,minimum->rank,minimum->data);
        fprintf(stdout, "->: %d ", minimum->right->vertex);
        fprintf(stdout, " root \n");
        if (minimum->down ==NULL){
        }
        else{
                fprintf(stdout, "representative child: %d \n\n", minimum->down->vertex);
        }
        print_children(minimum->down);
        fprintf(stdout,"\n");
}
/*prints the children of a node calling this same function to print eah nodes
children */
void print_children (v_node *node){
        v_node *parent;
        while(node != NULL){
                fprintf(stdout, "child: ");
                fprintf (stdout, "vertex: %d rank: %d value: %d ",
                         node->vertex,node->rank,node->data);
                if(node->right==NULL){

                }
                else{
                        fprintf(stdout," -> : %d ", node->right->vertex); 
                }                
                if(first_active(node)){
                }
                else{
                        fprintf(stdout," <-: %d ", node->left->vertex); 
                }
                parent=find_parent(node);
                fprintf(stdout, " parent: %d ", parent->vertex);
                if (node->down ==NULL){
                }
                else{
                        fprintf(stdout, " representative child: %d \n\n", node->down->vertex);
                }
                print_children(node->down);
                node=node->right;
        }
}
/* finds the parent of a node used for printing purposes*/
v_node *find_parent(v_node *node){
        while(node->left->down != node){
                node=node->left;
        }
        return node->left;
}