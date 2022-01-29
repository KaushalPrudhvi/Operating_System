/*
 File: scheduler.C
 
 Author:
 Date  :
 
 */

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "scheduler.H"
#include "thread.H"
#include "console.H"
#include "utils.H"
#include "assert.H"
#include "simple_keyboard.H"
#include "simple_timer.H"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* METHODS FOR CLASS   S c h e d u l e r  */
/*--------------------------------------------------------------------------*/

Scheduler::Scheduler() {
	// initialise the head and tail to NULL;
	head= NULL;
	tail= NULL;
	
	
	//assert(false);
  Console::puts("Constructed Scheduler.\n");
}

void Scheduler::yield() {
	// check if interrupts are enabled and properly disable them to yield the CPU
	if(Machine::interrupts_enabled())
	Machine::disable_interrupts();
	
	// Get the thread next inn queue and make it head
	if(head!=NULL){
	struct FIFO* current= head;
	Thread* currentThread= head->thread;
	head= head->next;
	if(head==NULL){
		tail==NULL;
	}
	
	// dispatch the thread now pointed by head
	delete(current);
	Thread::dispatch_to(currentThread);
	}
	
	// again properly enable interrupts after the thread has been dispatched
	if(!Machine::interrupts_enabled())
	Machine::enable_interrupts();
		
	
	
  //assert(false);
}

void Scheduler::resume(Thread * _thread) {
	// Properly handle interrupts 
	if(Machine::interrupts_enabled())
	Machine::disable_interrupts();
	// add the thread to end of the queue and continue thread execution as per the queue
	struct FIFO* newEntry= new struct FIFO;
	newEntry->thread= _thread;
	newEntry->next= NULL;
	if(head==NULL){
		head= newEntry;
		tail= newEntry;
		
	}
	else{
		// adding item at the end of the list
		
		tail->next= newEntry;
		tail= newEntry;
	}
	
	if(!Machine::interrupts_enabled())
	Machine::enable_interrupts();
	
	// enable the interrupts again
  //assert(false);
}

void Scheduler::add(Thread * _thread) {
	resume(_thread);
	
  //assert(false);
}

void Scheduler::terminate(Thread * _thread) {
	
	
	// propelry handle interrupts and terminate the thread by yielding the CPU 
	if(Machine::interrupts_enabled())
	Machine::disable_interrupts();
	yield();
	
	
	if(!Machine::interrupts_enabled())
	Machine::enable_interrupts();
  //assert(false);
}

RRScheduler::RRScheduler(unsigned eoq){
	
	// RR scheduler 
	// intialise Head and tail
	head= NULL;
	tail= NULL;
	// define a simple timer as defined in kernel.c
	SimpleTimer* timer = new SimpleTimer(1000/eoq);
	//register the time with the interupt handler
	InterruptHandler::register_handler(0, timer);
	Console::puts("Constructed Round Robin Scheduler\n");

}

void RRScheduler::yield() {
	// Properly handle interrupts
	if(Machine::interrupts_enabled())
	Machine::disable_interrupts();
	// make the next thread pointed by head and then dispatch it
	if(head!=NULL){
	struct FIFO* current= head;
	Thread* currentThread= head->thread;
	head= head->next;
	if(head==NULL){
		tail==NULL;
	}
	
	
	delete(current);
	Thread::dispatch_to(currentThread);
	}
	// enable the interrupts after checking the status 
	if(!Machine::interrupts_enabled())
	Machine::enable_interrupts();
		
	
	
  //assert(false);
}

void RRScheduler::resume(Thread * _thread) {
	// check the interrupts and handle them properly
	if(Machine::interrupts_enabled())
	Machine::disable_interrupts();
	// add the thread to the end of the queue and continue with queue
	struct FIFO* newEntry= new struct FIFO;
	newEntry->thread= _thread;
	newEntry->next= NULL;
	if(head==NULL){
		head= newEntry;
		tail= newEntry;
		
	}
	else{
		// adding item at the end of the list
		
		tail->next= newEntry;
		tail= newEntry;
	}
	
	if(!Machine::interrupts_enabled())
	Machine::enable_interrupts();
	
	// enable the interrupts again
  //assert(false);
}

void RRScheduler::add(Thread * _thread) {
	// adding is similar to resume
	resume(_thread);
	
  //assert(false);
}



void RRScheduler::terminate(Thread * _thread) {
	
	// check for the interrupts and yield the CPU by properly disabling the interrupts before yielding
	
	if(Machine::interrupts_enabled())
	Machine::disable_interrupts();
	yield();
	
	// Properly enable interrupts after yielding the CPU
	if(!Machine::interrupts_enabled())
	Machine::enable_interrupts();
  //assert(false);
}
