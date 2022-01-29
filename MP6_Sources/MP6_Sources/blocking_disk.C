/*
     File        : blocking_disk.c

     Author      : 
     Modified    : 

     Description : 

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "assert.H"
#include "utils.H"
#include "console.H"
#include "blocking_disk.H"
#include "simple_disk.H"
#include "scheduler.H"
#include "mirroring_disk.H"

///externs

extern Scheduler* SYSTEM_SCHEDULER;
// mirroreddisk reference
MirroredDisk* mirror;

//lock reference
TSLock* lock;

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR */
/*--------------------------------------------------------------------------*/

BlockingDisk::BlockingDisk(DISK_ID _disk_id, unsigned int _size) 
  : SimpleDisk(_disk_id, _size) {
	  // blocked queue initialisation
	  head= NULL;
	  tail= NULL;
	  // construct a new mirror disk
	  mirror= new MirroredDisk(DISK_ID::DEPENDENT, _size);
	 // construct a new lock
	   lock= new TSLock();
}

/*--------------------------------------------------------------------------*/
/* SIMPLE_DISK FUNCTIONS */
/*--------------------------------------------------------------------------*/

void BlockingDisk::wait_until_ready(){
	// add blocked threads to queuee
addToQ(Thread::CurrentThread());

while(!is_ready()){// || mirror->is_ready()){
	// add the threads to the scheduling queue
	SYSTEM_SCHEDULER->resume(Thread::CurrentThread());
	// yield the cpu
	
	SYSTEM_SCHEDULER->yield();
}

// dequeue from the queue
removeFromQ(Thread::CurrentThread());
}


void BlockingDisk::read(unsigned long _block_no, unsigned char * _buf) {
  // -- REPLACE THIS!!!
  //SimpleDisk::read(_block_no, _buf);
  //lock the disk
  lock->seal();
  // issue operation to mirrored disk
  mirror->readFromMirror(_block_no, _buf);
  
  // issue opeeration to blovking disk
  SimpleDisk::issue_operation(DISK_OPERATION::READ, _block_no);
  
  wait_until_ready();
  
  if(is_ready()){
	   /* read data from port */
  int i;
  unsigned short tmpw;
  for (i = 0; i < 256; i++) {
    tmpw = Machine::inportw(0x1F0);
    _buf[i*2]   = (unsigned char)tmpw;
    _buf[i*2+1] = (unsigned char)(tmpw >> 8);
  }
  }else{
	  mirror->read(_block_no, _buf);
}


//release the lock once done
lock->release();
}


void BlockingDisk::write(unsigned long _block_no, unsigned char * _buf) {
  // -- REPLACE THIS!!!
  //SimpleDisk::write(_block_no, _buf);
  //lock the disk
  lock->seal();
  
  //issue operation to mirror disk
  mirror->writeToMirror(_block_no, _buf);
  
  //issue operation to blovking disj
  issue_operation(DISK_OPERATION::WRITE, _block_no);
  
  wait_until_ready();
  if(is_ready()){
	   int i; 
  unsigned short tmpw;
  for (i = 0; i < 256; i++) {
    tmpw = _buf[2*i] | (_buf[2*i+1] << 8);
    Machine::outportw(0x1F0, tmpw);
  }
  }else{
	 mirror->write(_block_no, _buf);
}

//release the lock once done
lock->release();
}

TSLock::TSLock(){
	//initialie key to 0
	this->key=0;
}

int TSLock:: TestAndSet(int* key){
	
	// test and set the key
	int temp = *key;
	*key= 1;
	return temp;
}

void TSLock::seal(){
	//wait until the key can be acquired
	while(TestAndSet(&key));
	Console::puts("Disk is Locked\n");
}

void TSLock::release(){
	//release key by setting to 0;
	key=0;
	
	Console::puts("Disk is unlocked\n");
}



void BlockingDisk::addToQ(Thread* thread){
	
	
	
	//basic queueing operation same as scheduling queue
diskQ* curr= new diskQ;
curr->thread= thread;
curr->next= NULL;

if(!head){
	head= tail=curr;
}
else{
	tail->next= curr;
	tail= curr;
}
}


void BlockingDisk::removeFromQ(Thread* thread){
	
	//same dequeuing as scheduling yielding
	if(head==NULL)
		return;
	diskQ* previous= NULL;
	diskQ* current= head;
	while(current->thread!= thread){
		previous=current;
		current= current->next;
		if(current= NULL)
			return;
	}
	previous->next= current->next;
	
	
	return;
	
	
}



