/* ---------------- Nucleus Initialization ------------------------
 * Written by Kate Plas and Travis Wahl
 * For CSCI-320 Operating Systems
 *
 * The scheduler ensures that every ready process has an opportunity to execute. It is a preemptive round robin algorithm with a time slice value of 5 miliseconds
 *
 */

#include "../h/types.h"
#include "../h/const.h"
#include "../h/pcb.h"
#include "../h/asl.h"
#include "../h/nucleusInitialization.h"
#include "../h/scheduler.h"


void scheduler() {
  cpu_t elapsedTime;
  /* currentProcess is a pointer to a pcb and is definined in nucleusPointer.h */
  if(currentProcess == NULL){ /* if the currentProcess points to NULL, there is nothing executing so get a pcb from the readyQueue */
    pcb_PTR nextProcess = removeProcQ(&readyQueue);
  } else { /* If currentProcess is not null, there is a process running, so check its time */
    STCK(elapsedTime); /* get how long the process has been executing */
    /* Adjust the CPU time used by the process by adding how much time has elapsed since the clock started added to the CPU time used by the processor so far (p_time) */
    currentProcess -> p_time = currentProcess -> p_time + (elapsedTime - startTOD);
    LDIT(INTERVALTIMER);
    pcb_PTR nextProcess = removeProcQ(&readyQueue);
  }
  if(nextProcess != NULL){
    currentProcess = next;
    STCK(startTimeOfDayClock);
    /* setTIMER is a control register write command. p. 60 in pops. The timer is set to a quantum which is 5ms. */
    setTIMER(QUANTUM);
    /* get the process state of the current process */
    LDST(&(currentProcess -> p_s));
    /* more stuff */
  } else { /* if the Ready Queue is empty */
    /* processCount and softBlockCount are initialized in nucleusInitialization.c */
    /* "If the Process Count > 0 and the Soft-block Count > 0 enter a Wait State." -p. 23 pandos */
    if((processCount > 0) && (softBlockCount > 0)){
      /* " The Scheduler must first set the Status register to enable interrupts and either disable the processor Local Timer (also through the Status register), or load it with a very large value.*/
      /* "Interrupts enabled via the STATUS register [Section 7.1-pops]" */
      setSTATUS(ALLOFF | IECON | IMON);
      WAIT();
    }
    if((processCount > 0) && (softBlockCount == 0)){ /* this is a deadlock */
      PANIC();
    }
    /* "If the Process Count is zero invoke the HALT BIOS service/instruction." - p. 23 pandos  */
    if(processCount == 0){
      HALT();
    }
  }
}
