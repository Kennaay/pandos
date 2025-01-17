/* ---------------- 2.4 The Active Semaphore List (ASL) ------------------------
 * Written by Kate Plas and Travis Wahl
 * For CSCI-320 Operating Systems
 * ----- Includes two main parts: -------------------
 *   1. The ASL is a null-terminated single, linearly lined list whose nodes each have a s_next field.
 *      The ASL has a head pointer called semd_h. A semaphore is active if there is at least one pcb on the process
 *	queue associated with it.
 *   2. The semdFree list which is also a single linearly linked list. It keeps the semaphores that are free.
*/

#include "../h/types.h"
#include "../h/const.h"
#include "../h/pcb.h"
#include "../h/asl.h"

HIDDEN semd_t *semd_h, *semdFreeList_h;
/* semd_h is the head pointer of the active semaphore list. semdFree_h is the head pointer to the semdFree list that 
 * holds the unused semaphore descriptors. */

/**************************** semdFreeList Methods ***************************/
/* semdFreeList is a singly linked NULL terminated stack that holds the free semds.*/

/* Pushes a node pointed to by s onto the stack that is the semdFreeList
   Note that the next pointer of each node points downwards in the stack */
void freeSemdByPushingItOntoTheSemdFreeList(semd_t *s){
	if (semdFreeList_h == NULL){ /* If the freeList is empty */
				s -> s_next = NULL; /* set the new node's next to NULL since there is no other node in 
						     * the stack */
				semdFreeList_h = s;
	}
  	else if (semdFreeList_h != NULL){ /* If the freeList is not empty: */
        s -> s_next = semdFreeList_h; /* set the new node's next to hold the head address because the head will be below
				       * the new node on the stack. */
        semdFreeList_h = s;  /* the head points to the new node. */
	}
}

/* Pop a semd from the FreeList */
semd_t *popSemdFromFreeList(){
	semd_t *temp = semdFreeList_h;
	if(semdFreeList_h != NULL){ /* if the free list has nodes already */
		semdFreeList_h = semdFreeList_h -> s_next;
		temp -> s_next = NULL;
		temp -> s_semAdd = NULL;
		temp -> s_procQ = mkEmptyProcQ(); /* mkEMptyProcQ() Returns a pointer to the tail of an empty process queue; i.e. NULL. */
		return temp;
	}
	else { /* if the free list is already empty, can't pop anything else, return NULL */
		return NULL;
	}
}


/**************************** Active Semaphore List Methods ***************************/
/* Look through the active semaphore list for the semAdd that is given as a parameter.
Return the address of the parent whose child's semaphore had the semAdd that was the one we wanted.
Remember the ASL is "sorted in ascending order using the s_semAdd field as a sort key." p. 13 pandos*/
semd_t *search(int *semAdd){
	semd_t *temp = semd_h;
	/* Continue the search while the search's semAdd is greater than temp's next's semAdd*/
	while (semAdd > (temp->s_next->s_semAdd)){
			/* Increment to the next node */
			temp = temp -> s_next;
	}
	/* Returns the address of the node whose child has the correct semAdd */
	return temp;
}


/* Insert the pcb pointed to by p at the tail of the process queue associated with the
semaphore whose physical address is semAdd and set the semaphore address of p to semAdd
If the semaphore is currently not active (i.e. there is no descriptor for it in the ASL),
allocate a new descriptor from the semdFree list, insert it in the ASL (at the appropriate position), initialize all of the 
fields (i.e. set s_semAdd to semAdd, and s_procQ to mkEmptyProcQ()), and proceed as above.
If a new semaphore descriptor needs to be allocated and the semdFree list is empty, return TRUE. In all other cases return FALSE. */
int insertBlocked (int *semAdd, pcb_t *p) {
	/* The search() gets the address of the parent whose kid has the correct semAdd.
	Then this addres is set to be held by temp */
	semd_t *temp = search(semAdd);
	/* If temp's kid actually does hold the correct semAdd and the node exists */
	if (temp -> s_next -> s_semAdd == semAdd) {
		/* Remember that p_semAdd was defined in types.h. It is the "pointer to a semaphore on which the process is blocked," (p.8 pandos).
		Get the pcb pointed to by p and get its semAdd. Set the correct semaphore's semAdd to be the address that the pcb's 
		pointer to its semaphore holds. */
			p -> p_semAdd = semAdd;
			/* Insert the pcb pointed to by p onto the semaphore's process queue. */
			insertProcQ(&(temp -> s_next -> s_procQ), p);
	}
	else {
		/* Remove a semd from the free list so that we can make a new semaphore */
			semd_t *newSemaphore = popSemdFromFreeList();
			if (newSemaphore == NULL) {
					return TRUE;
			}
			/* set the new semaphore's semAdd to hold the correct semAdd */
			newSemaphore -> s_semAdd = semAdd;
			/* set the new semaphore's s_procQ to be initialized */
			newSemaphore -> s_procQ = mkEmptyProcQ();
			/* link the semAdd and the pcb */
			p -> p_semAdd = semAdd;
			insertProcQ(&(newSemaphore->s_procQ), p);
			newSemaphore->s_next = temp->s_next;
			temp->s_next = newSemaphore;
	}
	return FALSE;
}


/* Search the ASL for a descriptor of this semaphore. If none is found, return NULL;
otherwise, remove the first (i.e. head) pcb from the process queue of the found semaphore descriptor and
return a pointer to it. If the process queue for this semaphore becomes empty (emptyProcQ(s procq) is TRUE),
remove the semaphore de- scriptor from the ASL and return it to the semdFree list. */
pcb_t *removeBlocked(int *semAdd) {
	semd_t *temp = search(semAdd);	/* Set a temp var using the search method on semADD */
	if (temp -> s_next -> s_semAdd == semAdd) {		/* If pointer to sempahor (s_semAdd) of the next element on
								 *	the ASL from temp == semADD */
		pcb_t *removed = removeProcQ(&temp -> s_next -> s_procQ);	/* Creation of removed var to track removed pcb */
		if (emptyProcQ(temp -> s_next -> s_procQ)){	/* run emptyProcQ to test if empty */
			semd_t *emptySemd = temp -> s_next;	/* Create emptySemd to track what we will use freeSemd on */
			temp -> s_next = emptySemd -> s_next;	/* next element from temp is equal to the next element of emptySemd */
			freeSemdByPushingItOntoTheSemdFreeList(emptySemd);			/* run freeSemd on emptySemd */
			removed -> p_semAdd = NULL;		/* reset p_semADD to NULL */
			return removed;
		}
		else {
			removed -> p_semAdd = NULL;		/* Otherwise set the blocked pointer to NULL */
			return removed;
		}
	}
	else {							/* Otherwise return NULL */
		return NULL;
	}

}


/* Remove the pcb pointed to by p from the process queue associated with p’s semaphore
(p→ p semAdd) on the ASL. If pcb pointed to by p does not appear in the process queue associated
with p’s semaphore, which is an error condition, return NULL; otherwise, re- turn p. */
pcb_t *outBlocked(pcb_t *p) {
	semd_t *temp = search(p -> p_semAdd);	/* Set a temp var using the search method on p _> p_semADD */
	if(temp -> s_next -> s_semAdd == p -> p_semAdd) {	/* If the pointer to the semaphore from temp's s_next equals
								 * p's blocked pointer */
		pcb_t *outted = outProcQ(&temp -> s_next -> s_procQ, p);	/* Create outted to track the outProcQ pcb */
		if(emptyProcQ(temp -> s_next -> s_procQ)) {	/* if emptyProcQ returns True */
			semd_t *emptySemd = temp -> s_next;	/* Create emptySemd to track what we will later use freeSemd on */
			temp -> s_next = emptySemd -> s_next;	/* Set s_next of temp equal to s-next of emptySemd */
			freeSemdByPushingItOntoTheSemdFreeList(emptySemd);			/* run FreeSemd on emptySemd */
			return outted;
		}
		else {
			outted -> p_semAdd = NULL;		/* Otherwise set the blocked pointer to NULL */
			return outted;
		}
	}
	else {							/* Otherwise return NULL */
		return NULL;
	}
}


/* Return a pointer to the pcb that is at the head of the process queue associated with the semaphore
semAdd. Return NULL if semAdd is not found on the ASL or if the process queue associated with semAdd is empty. */
pcb_t *headBlocked(int *semAdd){
	semd_t *temp = search(semAdd);		/* Create a temp var using the search method on semADD */
	if ((temp == NULL) || (emptyProcQ(temp -> s_next -> s_procQ))) { /* If the temp var is NULL OR if emptyProcQ returns false */
		return NULL;
	}
	else {
		return headProcQ(temp -> s_next -> s_procQ);		/* otherwise run headProcq (pcb.c) */
	}
}


/* Initialize the semdFree list to contain all the elements of the array
static semd_t semdTable[MAXPROC]
This method will be only called once during data structure initialization. */
void initASL(){
	static semd_t semdTableArray[MAXPROC + 2]; /* need two more bc of dumb nodes on either end */
	semdFreeList_h = NULL; /* the head of the free list is set to NULL */

	/* Initialize dumb nodes */
	semd_t *dumbFirst = &(semdTableArray[0]); /* Head Dummy */
	semd_t *dumbLast = &(semdTableArray[1]);  /* Tail Dummy */

	/* Initialize  the first dumb node */
	semd_h = dumbFirst;		/* the head pointer needs to be set to the first dummy */
	dumbFirst -> s_semAdd = 0;	/* Set the semAdd of the semaphore that the head points (first dummy) and set it to 0 */
	dumbFirst -> s_next = dumbLast;	/* Set the Last Dummy as next to the Head Dummy */
	dumbFirst -> s_procQ = mkEmptyProcQ();	/* clear s_procQ (sets to NULL) */


	/* Initialize the last dumb node */
	dumbLast -> s_semAdd = MAXINT; 	/* Tail dummy gets set to the MAXINT value, defined in const.h */
	dumbLast -> s_next = NULL;	/* Nothing comes after the Tail dummy, so we set the last dummy's next to NULL */
	dumbLast -> s_procQ = mkEmptyProcQ();	/* clear s_procQ (sets to NULL) */

	int i = 2;	/* Set the loop variable i to 2 (we have 2 dummies in there already */
	while (i < MAXPROC + 2) {
		freeSemdByPushingItOntoTheSemdFreeList(&semdTableArray[i]);	/* take a node off the free list */
		i++;
	}
}
