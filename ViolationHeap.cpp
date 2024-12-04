#include <stdio.h>
#include <stdlib.h>
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

int get_key(v_node_T node) {
    if (!node) {
        fprintf(stderr, "Invalid node in get_key\n");
        return -1;
    }
    return node->data;
}

int get_num_elements(v_heap_T heap) {
    if (!heap) {
        fprintf(stderr, "Invalid heap in get_num_elements\n");
        return -1;
    }
    return heap->num_elem;
}

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

void validate_circular_list(v_heap *heap);

void validate_heap_structure(v_heap *heap) {
    if (!heap) return;

    v_node *current = heap->min;
    int count = 0;
    do {
        if (current->left != NULL) {
            fprintf(stderr, "Error: Root node has a left pointer!\n");
            exit(1);
        }
        if (current->down != NULL) {
            v_node *child = current->down;
            while (child) {
                if (child->left == NULL) {
                    fprintf(stderr, "Error: Child's left pointer is NULL!\n");
                    exit(1);
                }
                child = child->right;
            }
        }
        // printf("Validating Node: %p, Data: %d\n", current, current->data);
        current = current->right;
        count++;
    } while (current && current != heap->min);

    if (count != heap->num_elem) {
        fprintf(stderr, "Error: Heap element count mismatch. Expected %d, Found %d\n", heap->num_elem, count);
        exit(1);
    }
}

void debug_heap_structure(v_heap *heap) {
    if (!heap) {
        printf("Heap is NULL\n");
        return;
    }
    printf("Heap -> num_elem: %d\n", heap->num_elem);

    v_node *current = heap->min;
    if (!current) {
        printf("Heap is empty\n");
        return;
    }

    printf("Root list:\n");
    do {
        printf("Node: %p, Data: %d, Right: %p\n", current, current->data, current->right);
        current = current->right;
    } while (current && current != heap->min);
    printf("End of root list\n");

    printf("Heap -> min: %p, Heap -> min->right: %p\n", heap->min, heap->min->right);

}


/* allocates space for a new violation heap*/
v_heap *v_heap_new(){
        v_heap* heap = (v_heap*)malloc(sizeof(*heap));
        heap->min=NULL;
        heap->last=NULL;
        heap->num_elem=0; 
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
        v_node *new_node = (v_node* ) malloc(sizeof(*new_node));
        new_node->left=NULL;
        new_node->right=NULL;
        new_node->down= NULL;
        new_node->data=value;
        new_node->vertex=ver;
        new_node->rank=0;      
        return new_node;
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
        int minimum;
        if (heap->min->data) {
                minimum = heap->min->data;
        }
        else { minimum = -1;}
        /* remove the minimum node*/
        // debug_heap_structure(heap);
        v_delete_min(heap);
        // debug_heap_structure(heap);
        /* if that was not the last node in the heap remofat the heap and find
         * the new minimum */
        if(heap->min!=NULL){
                // reformat_heap(heap);
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
void reformat_heap(v_heap *heap) {
    printf("1: reformat\n");
    debug_heap_structure(heap);
    printf("\n\n");

    if (heap->num_elem == 0) {
        heap->min = NULL;
        heap->last = NULL;
        return;
    }

    int size = ceil(log(heap->num_elem) / LN2) * 6 + 4;
    if (size == 0) return;

    v_node **heaplist = (v_node **)calloc(size, sizeof(*heaplist));
    if (!heaplist) {
        fprintf(stderr, "Error: Memory allocation failed in reformat_heap.\n");
        exit(1);
    }

    v_node *current = heap->min;
    int processed_nodes = 0;

    // Detach the circular root list
    heap->min = NULL;
    heap->last = NULL;

    printf("2: reformat\n");
    debug_heap_structure(heap);
    printf("\n\n");


    do {
        if (!current) {
            fprintf(stderr, "Error: Null current node during reformat_heap.\n");
            free(heaplist);
            exit(1);
        }

        printf("Processing Node: %p, Rank: %d, Data: %d\n", current, current->rank, current->data);

        int rank = current->rank;
        if (rank * 4 >= size) {
            fprintf(stderr, "Error: Rank out of bounds in reformat_heap. Rank: %d, Size: %d\n", rank, size);
            free(heaplist);
            exit(1);
        }

        printf("loop: reformat\n");
        debug_heap_structure(heap);
        printf("\n\n");

        // Combine nodes with the same rank
        while (heaplist[rank * 4] && heaplist[rank * 4 + 2]) {
            printf("Combining Nodes: Rank: %d\n", rank);
            current = three_way_combine(heaplist, NULL, current);
            rank = current->rank;
        }

        // Add current node to heaplist
        if (!heaplist[rank * 4]) {
            heaplist[rank * 4] = current;
        } else {
            heaplist[rank * 4 + 2] = current;
        }

        // Move to the next node and detach the current node
        v_node *next = current->right;
        current->right = NULL;
        current = next;

        processed_nodes++;
    } while (processed_nodes < heap->num_elem);

    // Rebuild the root list from heaplist
    v_node *new_min = NULL;
    v_node *last_node = NULL;
    int root_count = 0;

    for (int i = 0; i < size; i += 4) {
        if (heaplist[i]) {
            root_count++;
            if (!new_min) {
                new_min = heaplist[i];
                new_min->right = new_min; // Initialize circular list
            } else {
                last_node->right = heaplist[i];
            }
            last_node = heaplist[i];
            if (!heap->min || heaplist[i]->data < heap->min->data) {
                heap->min = heaplist[i];
            }
        }
    }

    if (last_node) {
        last_node->right = new_min; // Close the circular list
    }

    heap->last = last_node;

    free(heaplist);

    // Debugging
    printf("3: reformat\n");
    debug_heap_structure(heap);
    printf("\n\n");

    // Validate the new root list
    if (root_count != heap->num_elem) {
        fprintf(stderr, "Error: Root list count mismatch. Expected: %d, Found: %d\n", heap->num_elem, root_count);
        exit(1);
    }

    validate_circular_list(heap);
}


/* performs a three-way combine on the given nodes*/
v_node *three_way_combine(v_node **heaplist, v_node *previous_a, v_node *node_a) {
    int rank = node_a->rank;

    // Validate heaplist entries
    if (!heaplist[rank * 4] || !heaplist[rank * 4 + 2]) {
        fprintf(stderr, "Error: Invalid heaplist entry for rank %d.\n", rank);
        exit(1);
    }

    v_node *node_b = heaplist[rank * 4];
    v_node *node_c = heaplist[rank * 4 + 2];

    // Determine parent and children
    v_node *parent = NULL, *first = NULL, *second = NULL;
    if (node_a->data >= node_b->data) {
        second = node_a;
        parent = (node_c->data >= node_b->data) ? node_b : node_c;
        first = (node_c->data < node_b->data) ? node_b : node_c;
    } else {
        second = node_b;
        parent = (node_c->data >= node_a->data) ? node_a : node_c;
        first = (node_c->data < node_a->data) ? node_a : node_c;
    }

    // Link nodes
    first->right = second;
    second->right = parent->down;
    if (parent->down) parent->down->left = second;

    first->left = parent;
    second->left = first;
    parent->down = first;
    parent->rank++;

    // Update root list
    if (previous_a) {
        previous_a->right = parent;
    }
    parent->right = node_a->right;

    // Clear heaplist entries
    for (int i = 0; i < 4; i++) {
        heaplist[rank * 4 + i] = NULL;
    }

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
void validate_circular_list(v_heap *heap) {
    if (heap->min == NULL) {
        if (heap->num_elem != 0) {
            fprintf(stderr, "Error: Heap is empty, but num_elem is not zero.\n");
            exit(1);
        }
        return;
    }

    v_node *temp = heap->min;
    int count = 0;

    do {
        if (temp->right == NULL) {
            fprintf(stderr, "Error: Node %p (Data: %d) has NULL right pointer. Expected: %p\n",
                    temp, temp->data, heap->min);
            exit(1);
        }
        temp = temp->right;
        count++;
        if (count > heap->num_elem) {
            fprintf(stderr, "Error: Too many nodes in the root list. Possible infinite loop.\n");
            exit(1);
        }
    } while (temp != heap->min);

//     if (count != heap->num_elem) {
//         fprintf(stderr, "Error: Root list count mismatch. Expected: %d, Found: %d\n", heap->num_elem, count);
//         exit(1);
//     }

    if (heap->last->right != heap->min) {
        fprintf(stderr, "Error: Last node's right pointer does not point to the minimum node.\n");
        printf("Last Node: %p (Data: %d), Right: %p, Expected: %p\n",
               heap->last, heap->last->data, heap->last->right, heap->min);
        exit(1);
    }

    printf("Validation Passed: Circular root list is consistent.\n");
}

/* removes the minimum from the heap wiring in its children */
void v_delete_min(v_heap *heap) {
    v_node *minimum = heap->min;

    if (!minimum) return;

//     printf("Deleting minimum node: %p (Data: %d)\n", minimum, minimum->data);

    v_node *child = minimum->down;
    v_node *last_child = NULL;

    // Detach all children from their parent and link them into the root list
    while (child) {
        child->left = NULL; // Detach child from parent
        last_child = child;
        child = child->right;
    }

    // If the heap has only one root node
    if (minimum == heap->last) {
        if (last_child) {
            heap->min = minimum->down;
            heap->last = last_child;
            heap->last->right = heap->min; // Close the circular structure
        } else {
            heap->min = NULL;
            heap->last = NULL;
        }
    } else {
        if (last_child) {
            // Link children into the root list
            last_child->right = minimum->right;
            heap->last->right = minimum->down;
        } else {
            heap->last->right = minimum->right;
        }
        heap->min = heap->last->right;
    }

    heap->num_elem--;

//     validate_circular_list(heap);

    free(minimum);
}


/* prints the ro0t list of a heap calling a helper to print the children at each
 * level */
void v_print_heap(v_heap *heap){
        fprintf(stdout, "there are %d nodes in this heap \n",heap->num_elem);
        if(!(heap)){
                return;
        }
        v_node *minimum = heap->min;
        if(minimum==NULL){
                fprintf(stdout,"\n\n\n\n");
                return;
        }
        v_node *temp = minimum->right;
        while(temp != minimum){
                if (temp==heap->last){
                        fprintf(stdout,"last: ");
                }
                fprintf (stdout, "this is vertex: %d with rank: %d and value: %d ",
                         temp->vertex,temp->rank,temp->data);
                fprintf(stdout, "right vertice: %d ",temp->right->vertex);
                fprintf(stdout, " root node so no parent ");
                if (temp->left != NULL){
                        fprintf(stdout, "THIS IS WRONG: left node:%d", temp->left->vertex);
                }
                else{
                        fprintf(stdout, "no left sibling "); 
                }
                if (temp->down ==NULL){
                        fprintf(stdout, "no children\n\n");
                }
                else{
                        fprintf(stdout, "representative child is : %d \n\n", temp->down->vertex);
                }
                print_children(temp->down);
                temp=temp->right;
        }
        if (temp==heap->last){
                fprintf(stdout,"last: ");
        }
        fprintf(stdout,"minimum: ");
        fprintf (stdout, "this is vertex: %d with rank: %d and value: %d ",
                 minimum->vertex,minimum->rank,minimum->data);
        fprintf(stdout, "right vertice: %d ", minimum->right->vertex);
        fprintf(stdout, " root node so no left or parent ");
        if (minimum->down ==NULL){
                fprintf(stdout, "no children\n\n");
        }
        else{
                fprintf(stdout, "representative child is : %d \n\n", minimum->down->vertex);
        }
        print_children(minimum->down);
        fprintf(stdout,"\n\n\n\n");
}
/*prints the children of a node calling this same function to print eah nodes
children */
void print_children (v_node *node){
        v_node *parent;
        while(node != NULL){
                fprintf(stdout, "child: ");
                fprintf (stdout, "this is vertex: %d with rank: %d and value: %d ",
                         node->vertex,node->rank,node->data);
                if(node->right==NULL)
                        fprintf(stdout," there is no right node ");
                else{
                        fprintf(stdout," right node: %d ", node->right->vertex); 
                }                
                if(first_active(node)){
                        fprintf(stdout," i am the reprentative child ");
                }
                else{
                        fprintf(stdout," left node: %d ", node->left->vertex); 
                }
                parent=find_parent(node);
                fprintf(stdout, " the parent is: %d ", parent->vertex);
                if (node->down ==NULL){
                        fprintf(stdout, " no children \n\n");
                }
                else{
                        fprintf(stdout, " representative child is : %d \n\n", node->down->vertex);
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