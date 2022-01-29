/*
     File        : mirroring_disk.c

     Author      : KAUSHAL
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
#include "scheduler.H"
#include "simple_disk.H"
#include "mirroring_disk.H"

//externs

extern Scheduler* SYSTEM_SCHEDULER;

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR */
/*--------------------------------------------------------------------------*/

MirroredDisk::MirroredDisk(DISK_ID _disk_id, unsigned int _size) 
  : SimpleDisk(_disk_id, _size) {
	  
	  Console::puts("Constructed MirroredDisk\n");
}

/*--------------------------------------------------------------------------*/
/* SIMPLE_DISK FUNCTIONS */
/*--------------------------------------------------------------------------*/



void MirroredDisk::wait_until_ready(){
	while(!is_ready()){
		//resume 
		SYSTEM_SCHEDULER->resume(Thread::CurrentThread());
		//yield
		SYSTEM_SCHEDULER->yield();
	}
}

void MirroredDisk::readFromMirror(unsigned long _block_no, unsigned char * _buf) {
	
	//issue operation to disk low level
  SimpleDisk::issue_operation(DISK_OPERATION::READ, _block_no);

}
void MirroredDisk::read(unsigned long _block_no, unsigned char * _buf) {
  // -- REPLACE THIS!!!
  //SimpleDisk::read(_block_no, _buf);
  int i;
  unsigned short tmpw;
  for (i = 0; i < 256; i++) {
    tmpw = Machine::inportw(0x170);
    _buf[i*2]   = (unsigned char)tmpw;
    _buf[i*2+1] = (unsigned char)(tmpw >> 8);
  }
  

}

void MirroredDisk::writeToMirror(unsigned long _block_no, unsigned char * _buf) {


//issue write operation to disk ... low level
issue_operation(DISK_OPERATION::WRITE, _block_no);




}
void MirroredDisk::write(unsigned long _block_no, unsigned char * _buf) {
  // -- REPLACE THIS!!!
//  SimpleDisk::write(_block_no, _buf);

/* write data to port */
  int i; 
  unsigned short tmpw;
  for (i = 0; i < 256; i++) {
    tmpw = _buf[2*i] | (_buf[2*i+1] << 8);
    Machine::outportw(0x170, tmpw);
  }


}
