// ---------------- 2.4 The Active Semaphore List (ASL) ------------------------
/* Written by Kate Plas and Travis Wahl
 * For CSCI-320 Operating Systems
 * ----- Includes two main parts: -------------------
 *   1. The ASL is a null-terminated single, linearly lined list whose nodes each have a s_next field.
 *      The ASL has a head pointer called semd_h. A semaphore is active if there is at least one pcb on the process queue associated with it.
 *   2. The semdFree list which is also a single linearly linked list. It keeps the semaphores that are free.
*/

#include "../h/types.h"
#include "../h/const.h"
#include "../h/pcb.h"
#include "../h/asl.h"

HIDDEN semd_t *semd_h, *semdFree_h;
// semd_h is the head pointer of the active semaphore list .
// semdFree_h is the head pointer to the semdFree list that holds the unused semaphore descriptors.

/* Insert the pcb pointed to by p at the tail of the process queue associated with the
semaphore whose physical address is semAdd and set the semaphore address of p to semAdd.
If the semaphore is currently not active (i.e. there is no descriptor for it in the ASL),
allocate a new descriptor from the semdFree list, insert it in the ASL (at the appropriate
position), initialize all of the fields (i.e. set s_semAdd to semAdd, and s_procQ to
mkEmptyProcQ()), and proceed as above. If a new semaphore descriptor needs to be allocated
and the semdFree list is empty, return TRUE. In all other cases return FALSE. */
int insertBlocked (int *semAdd, pcb_t *p) {
	/* Insert the pcb pointed to by p
	at the tail of the process queue associated with the
	semaphore whose physical address is semAdd */

	/* find the process queue associated with the semaphore whose physical address is semAdd. */

}


/* Search the ASL for a descriptor of this semaphore. If none is found, return NULL;
otherwise, remove the first (i.e. head) pcb from the process queue of the found semaphore descriptor and
return a pointer to it. If the process queue for this semaphore becomes empty (emptyProcQ(s procq) is TRUE),
remove the semaphore de- scriptor from the ASL and return it to the semdFree list. */
pcb_t *removeBlocked(int *semAdd) {
	//code
}


/* Remove the pcb pointed to by p from the process queue associated with p’s semaphore
(p→ p semAdd) on the ASL. If pcb pointed to by p does not appear in the process queue associated
with p’s semaphore, which is an error condition, return NULL; otherwise, re- turn p. */
pcb_t *outBlocked(pcb t *p) {
	// code
}


/* Return a pointer to the pcb that is at the head of the process queue associated with the semaphore
semAdd. Return NULL if semAdd is not found on the ASL or if the process queue associated with semAdd is empty. */
pcb_t *headBlocked(int *semAdd){
	//code
}


/* Initialize the semdFree list to contain all the elements of the array
static semd_t semdTable[MAXPROC]
This method will be only called once during data structure initialization. */
void initASL(){
	static semd_t semdTableArray[MAXPROC + 2]; // need two more bc of dumb nodes on either end
	semd_h = NULL; // the head of the active semaphore list is set to NULL
	semdFreeList_h = NULL; // the head of the free list is set to NULL
	int i = 0;
	/* increment through nodes of semdTableArray and insert MAXPROC nodes onto the semdFreeList */
	while(i < MAXPROC){
		freeSemd(&(semdTableArray[i]));
		i++;
	}
	// STILL NEED INITIALIZATIONS
}

/* Pushes a node pointed to by s onto the stack that is the semdFreeList
   Note that the next pointer of each node points downwards in the stack */
void freeSemd(semd_t *s){
	if (semdFreeList_h == NULL){ // if the freeList is empty:
				s -> s_next = NULL; // set the new node's next to NULL since there is no other node in the stack
  if (semdFreeList_h != NULL){ // if the freeList is not empty:
        s -> s_next = semdFreeList_h; // set the new node's next to hold the head address because the head will be below the new node on the stack.
    }
	semdFreeList_h = s;  // the head points to the new node.
}