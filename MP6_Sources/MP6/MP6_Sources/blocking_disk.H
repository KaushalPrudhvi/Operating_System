/*
     File        : blocking_disk.H

     Author      : 

     Date        : 
     Description : 

*/

#ifndef _BLOCKING_DISK_H_
#define _BLOCKING_DISK_H_

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "simple_disk.H"
#include "thread.H"
#include "scheduler.H"
/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

extern Scheduler* SYSTEM_SCHEDULER;

// queue for blocked threads
struct diskQ{
Thread* thread;
diskQ* next;
};
/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* B l o c k i n g D i s k  */
/*--------------------------------------------------------------------------*/

class BlockingDisk : public SimpleDisk {
diskQ* head;
diskQ* tail;

 DISK_ID disk_id;
   unsigned int size;

//function to add threads requesting disk to queue
void addToQ(Thread* thread);

//function to deque blocked threads
void removeFromQ(Thread* thread);


public:

virtual void wait_until_ready();
   BlockingDisk(DISK_ID _disk_id, unsigned int _size); 
   /* Creates a BlockingDisk device with the given size connected to the 
      MASTER or SLAVE slot of the primary ATA controller.
      NOTE: We are passing the _size argument out of laziness. 
      In a real system, we would infer this information from the 
      disk controller. */



   /* DISK OPERATIONS */
   
  
   virtual void read(unsigned long _block_no, unsigned char * _buf);
   /* Reads 512 Bytes from the given block of the disk and copies them 
      to the given buffer. No error check! */

   virtual void write(unsigned long _block_no, unsigned char * _buf);
   /* Writes 512 Bytes from the buffer to the given block on the disk. */

};


class TSLock{
private:
int key;
public:
//constructor 
TSLock();

// test the key and then seal the disk 
int TestAndSet(int *key);
//function to implement sealing
void seal();

//function to release the disk
void release();
};

#endif
