#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ViolationHeap.h"

// mark: whether the node is colored of not
typedef struct f_node {
        struct f_node *up;
        struct f_node *down;
        struct f_node *left;
        struct f_node *right;
        int vertex;
        int data;
        int children;
        char colored;
} f_node;
/* Definition of what a fibonacci heap is a pointer to the min
 * and num_elem the number of elements in the heap.
*/
typedef struct f_heap{
        f_node *min;
        int num_elem;
} f_heap;

#define LN2 .693147
/* ************************ FUNCTION DEFINITIONS *************************/
void initialize_node(f_node *node, int value,int ver);
void set_min (f_heap *heap1);
void delete_min(f_heap *heap1);
void correct_heap(f_heap *heap1);
void free_level(f_node *node);
void print_level(f_node *node);
void combine_as_needed(f_node *current, f_node **heaplist);
f_node* merge_heaps(f_node* node1, f_node* node2);

/************************** FUNCTION CODE*****************/                    


/*purpose: creates a new heap */                                                    
f_heap *f_heap_new(){
        f_heap *heap = (f_heap*) malloc(sizeof(*heap));
        heap->min=NULL;
        heap->num_elem=0;
        return heap;
}
/*purpose: frees the heap and all the nodes in the heap */
void f_heap_free(f_heap **heap){
	/* frees heap->min and all of it siblings*/
        free_level((*heap)->min);
	/*frees the heap*/
        free(*heap);
}
/*frees the given node and all of its sibling and calls this function to free
 * each siblings children*/
void free_level(f_node *node){
	/*if node is NULL return*/
        if (node == NULL){
                return;
        }
/*	frees the current nodes children */
        free_level(node->down);
        f_node *temp, *previous;
        temp=node->left;
/*	loop through all the siblings of the given node free their children and
 * 	then the node */
        while (temp!=node){
                free_level(temp->down);
                previous=temp;
                temp=temp->left;
                free(previous);
        }
        free(node);
}
/* prints the heap */        
void print_heap(f_heap *heap){
        fprintf(stdout,"start of heap that contains %d element \n\n\n",
		 heap->num_elem);
        if (heap->min!=NULL){
                fprintf(stdout, "minimum: ");
                print_level(heap->min);
        }
        fprintf(stdout,"end of heap \n\n");
}

void print_level(f_node *node){
	/* prints all of the relevent information about the node then its children
	 * then siblings*/
        fprintf(stdout,"this is vertex: %d with value: %d and color value: %d ", 
                node->vertex,node->data,node->colored);
        if (node->up!=NULL){
                fprintf(stdout, "child of vertex: %d " ,
                        node->up->vertex);
        }
        else{
                fprintf(stdout, "root list node ");
        }
        if(node->left!=NULL){
                fprintf(stdout, "left sibling: %d ", node->left->vertex);
        }
        else{
                fprintf(stdout, "THIS IS WRONG NO LEFT NODE ");
        }
        if(node->right!=NULL){
                fprintf(stdout, "right sibling: %d ", node->right->vertex);
        }
        else{
                fprintf(stdout, "THIS IS WRONG NO RIGHT NODE ");
        }        
        if(node->down!=NULL){
                fprintf(stdout,"with representative child: %d\n",node->down->vertex);
                print_level(node->down);
        }
        else{
                fprintf(stdout, "has no child\n");
        }
        f_node *temp;
        temp=node->left;
        while (temp!=node){
                fprintf(stdout,"this is vertex: %d with value: %d and color value: %d ",
                        temp->vertex,temp->data,temp->colored);
                if (temp->up!=NULL){
                        fprintf(stdout, "child of vertex: %d " ,
                                temp->up->vertex);
                }
                else{
                        fprintf(stdout, "root list node ");
                }
                if(temp->left!=NULL){
                        fprintf(stdout, "left sibling: %d ", temp->left->vertex);
                }
                else{
                        fprintf(stdout, "THIS IS WRONG NO LEFT NODE ");
                }
                if(temp->right!=NULL){
                        fprintf(stdout, "right sibling: %d ", temp->right->vertex);
                }
                else{
                        fprintf(stdout, "THIS IS WRONG NO RIGHT NODE ");
                }        
                if(temp->down!=NULL){
                        fprintf(stdout,"with representative child: %d\n",
			temp->down->vertex);
                        print_level(temp->down);
                }
                else{
                        fprintf(stdout, "has no child\n");
                }     
                temp=temp->left;
        }
}
/* inserts the new node into the heap with given value and ver and returns a pointer 
 * to that node*/
f_node *f_heap_insert(f_heap *heaps, int value, int ver){
        f_node *new_node = (f_node* ) malloc(sizeof(*new_node));
        f_node *temp;
        initialize_node(new_node, value, ver);
        
        // If the heap is empty, make this node the minimum
        if (heaps->min == NULL){
                new_node->left = new_node;
                new_node->right = new_node;
                heaps->min = new_node;
        }
        else {
		/* If there are elements in the heap, put the new node to the left of
		 * current min. If it is smaller than the min, make it the new min */
                temp = heaps->min->left;
                temp->right = new_node;
                new_node->left = temp;
                new_node->right = heaps->min; 
                heaps->min->left = new_node; 
                if (heaps->min->data > value){
                        heaps->min = new_node;
                }
        }
        
	// Increment the number of elements in the heap 
        heaps->num_elem = heaps->num_elem + 1;
        return new_node;
}
/* set all elements of the node to zero/NULL */
void initialize_node(f_node *node, int value, int ver){
        node->up=NULL;
        node->down=NULL;
        node->left=NULL;
        node->right = NULL;
        node->data=value;
        node->colored=0;
        node->vertex=ver;
        node->children=0;
}
/*merging two heaps with the result stored in heap1*/
void merge_f_heaps( f_heap *heap1, f_heap *heap2){
        // if heap2 is empty free it and return
	if (heap2->min == NULL){
                free(heap2);
                return;
        }
	// if heap1 is empty set the min of heap1 to the min of heap 2 change the 
	// number of elements in heap1 then free heap2
        if (heap1->min == NULL){
                heap1->num_elem=heap2->num_elem;
                heap1->min=heap2->min;
                free(heap2);
                return;
        }
        f_node *temp;
        f_node *temp1;
	//Change pointers as necassary at sert heap min to the right value
        temp = heap1->min->right;
        temp1 = heap2->min->left;
        heap1->min->right=heap2->min;
        heap2->min->left= heap1->min;
        temp->left=temp1;
        temp1->right=temp;
        heap1->min= (heap1->min->data > heap2->min->data) ? 
                    heap1->min : heap2->min;
        heap1->num_elem = heap1->num_elem + heap2->num_elem;                  
        free(heap2);      
}
/* returns the min of the -1 if there is no min*/
int f_heap_min(f_heap *heap1){
        if ( heap1->min == NULL)
        {
                return -1;
        }
        return heap1->min->vertex;
}
/* prints the root list*/
void print_root_list(f_heap *heap){
        f_node *temp;
        temp=heap->min->left;
        while(temp!= heap->min){
                fprintf(stderr,"%d ->",temp->data);
                temp=temp->left;
        }
        fprintf(stderr,"%d", heap->min->data);
        fprintf(stderr, "\n");
}
/* extracts the min from the heap*/
int f_extract_min(f_heap *heap1){
        int minimum;
        if(heap1->min==NULL){
                return -1;
        }
        minimum=heap1->min->vertex;
        delete_min(heap1);
        if(heap1->min !=NULL){
                correct_heap(heap1);
                set_min(heap1);
        }
        return minimum;
}
/*sets the min of heap to the smallest element in the root list which is the
 * smallest in the entire heap */
void set_min (f_heap *heap1){
        f_node *minimum, *temp1, *start;
        start=heap1->min;
        minimum=start;
        temp1=start->left;
        // loops through all the elements in the root list and if it smaller than
        // the current minimum set it to the minimum
        while(temp1 != start){
                minimum= (minimum->data < temp1->data) ? minimum : temp1; 
                          temp1=temp1->left;  
        }
        heap1->min = minimum;
}
/* removes the minimum from the heap by unwiring it from the surrounding nodes
*/
void delete_min(f_heap *heap1){
        f_node *temp1, *temp2, *minimum;
        minimum=heap1->min;
        temp1=minimum->down;
/* if the minimum node has children set all the parent nodes to null then merge
 * it with the current heao*/
        if (temp1 != NULL){
                temp2=temp1->left;
                temp1->up=NULL;
                while (temp2!=temp1){  
                        temp2->up=NULL;
                        temp2=temp2->left;
                }
                temp1->left->right=minimum->right;
                minimum->right->left=temp1->left;
                minimum->right=temp1;
                temp1->left=minimum;
        }
        temp1=minimum->left;
/* if the minimum node has siblings wire the neghibhors togethe then set minimum
 * to th nodes left sibling */
        if (temp1==minimum){
                heap1->min=NULL;
        } 
        else{
                minimum->right->left=minimum->left;
                minimum->left->right=minimum->right;
                heap1->min=minimum->left;
        }
/* decrease the number of elements then free the minimum node */
        heap1->num_elem=heap1->num_elem-1;
        free(minimum);
}
/*rebuilds the heap so that it has the desired shape again. */
void correct_heap(f_heap *heap1){
        f_node *current,*minimum;
        current=heap1->min->left;
        /*creates a list of nodes long enought to hold log(n) nodes*/
        int size=ceil((log(heap1->num_elem))/LN2+1)*2, siz=sizeof(f_node*);
        f_node **heaplist= (f_node**)calloc(size, siz);
        minimum=heap1->min;
        size = minimum->children;
        heaplist[size]= minimum;
/* keep looping until the current node is already in the heaplist. if the isn't
one of the size put the current node in that location of the array. If there is
merge the two nodes and set that location of the nodelist to null. Use the top
of that node tree as the next node */
        while (current!=heaplist[current->children]){ 
                size=current->children;
                if(heaplist[size]==NULL){
                        heaplist[size]=current;
                        current=current->right;
                }
                else{
                        current=merge_heaps(current, heaplist[size]);
                        heaplist[size]=NULL;
                        
                }
        }
        heap1->min=current;
        free(heaplist);
}
/* takes two node and merges with the smaller on top*/
f_node* merge_heaps(f_node* node1, f_node* node2){
        f_node *small, *big, *temp;
        /* set small and big the apprate roots of trees */ 
        if (node1->data >=node2-> data){
                small=node2;
                big= node1; }
        else{
                big=node2;
                small=node1;
        }
        big->left->right=big->right;
        big->right->left=big->left;
        /* if small has no children just make big it child else wire big into
         * the root list */
        if (small->down == NULL){
                big->left=big;
                big->right=big;
        }
        else{
                temp=small->down->left;
                temp->right=big;
                big->left=temp;
                big->right=small->down;
                small->down->left=big;
        } 
        small->down=big;
        big->up=small;
        small->children = small->children + 1;
        return small;
}
/* decreases the key og the given node to the given value */
void f_heap_decrease_key(f_heap *heap1, f_node* node, int value){
        if(node==NULL){
                return;
        }
        /* creates a pointer to the parent*/
        f_node* parent = node->up;
        node->data=value;
        /* if there is no parent no cut is needed*/
        if (parent==NULL){
                return;
        }
        /* if a cut is needed or this is being used as a recursive cut on a
         * colored parent unwire the current node and then rewire it left of the
         * min*/
        if ((parent->data >= value)||(node->data==value)){
                node->up=NULL;
                if ((node->left) !=node){       
                        if ((parent->down == node)){
                                parent->down=node->left;
                        }
                        node->left->right=node->right;
                        node->right->left=node->left;
                }
                else{
                        parent->down=NULL;
                }
                node->left=heap1->min->left;
                heap1->min->left->right=node;
                node->right=heap1->min;
                heap1->min->left=node;
                parent->children = parent->children-1;
                if (parent->colored==1){
                        parent->colored=0;
                        f_heap_decrease_key(heap1,parent,parent->data);
                }
                else{
                        parent->colored=1;
                }
        }
}