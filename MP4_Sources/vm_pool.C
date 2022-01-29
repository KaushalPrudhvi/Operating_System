/*
 File: vm_pool.C
 
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

#include "vm_pool.H"
#include "console.H"
#include "utils.H"
#include "assert.H"
#include "simple_keyboard.H"

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
/* METHODS FOR CLASS   V M P o o l */
/*--------------------------------------------------------------------------*/

VMPool::VMPool(unsigned long  _base_address,
               unsigned long  _size,
               ContFramePool *_frame_pool,
               PageTable     *_page_table) {
				   
				   // initialisation
		base_address= _base_address;
		size= _size;
		frame_pool= _frame_pool;
 	    page_table= _page_table;
				   
		page_table->register_pool(this);		 				   
	    currentRegion=0;  // Tells how many regions are currently active ;
		// calculate the total number of regions in the pool that may be addressed by the structure defined
		totalregions =Machine::PAGE_SIZE/sizeof(container);
       container= (struct Container*) (PageTable::PAGE_SIZE *(_frame_pool->get_frames(1))); //allocate a page to store the structure that points to many regions in a pool
		
        
    //assert(false);
    Console::puts("Constructed VMPool object.\n");
}

unsigned long VMPool::allocate(unsigned long _size) {
	unsigned long start_address=0;
	
	// zero size request should be trapped 
	if(_size==0){
		assert(false);
	}
	
	
	// illegal access should be trapped
	
	if(currentRegion > totalregions){
		assert(false);
	}
	
	if(currentRegion==0){
		// this is the first region to be allocated
		start_address= base_address; 
	}
	else{
		// may be another region in that array.
		start_address= container[currentRegion].relocatable_address+ container[currentRegion].offset;
	}
	
	
	container[currentRegion].relocatable_address= start_address;  // store the address of the requested region
	container[currentRegion].offset= _size;    // also the size which may be a fragment but we give a full page here as no internal internal frgamentation case is considered for simplicity
	   
	currentRegion++; // Track the current region number to allow for future requests
	
	
	
	
    //assert(false);
    Console::puts("Allocated region of memory.\n");
	return start_address;

}

void VMPool::release(unsigned long _start_address) {
	
	int position=0; 
	// used to store the position of the region to be freed and loop to get the value
	unsigned int p=0;
	while( p< currentRegion){
		if(container[p]. relocatable_address== _start_address){
			position= p;
			// break if found
			break;
		}
		p++;
	}
	
	
	
	unsigned int limit= (container[position].offset)/ Machine::PAGE_SIZE;
	unsigned int k=0; 
	// loop to release the frames allocated to it for the entire region range and modify the start_address
	while(k< limit){
		page_table->free_page(_start_address);
		_start_address+= PageTable::PAGE_SIZE;
		k++;
	}
	
	
	// Releasing the region created
	// create a temporary structure to keep track of regions in order to release the requested pool frames
	Container* temp= container;
	container= (Container*)(Machine::PAGE_SIZE * (frame_pool->get_frames(1)));
	
	unsigned int pos=0;
	unsigned int j=0;
	while( j< currentRegion){
		if(j!= position)
			container[pos]= temp[j];
		pos++;
		j++;
	}

		// release the frames allocated.
		frame_pool->release_frames((unsigned long) temp/Machine::PAGE_SIZE);
	
	
	currentRegion--; // decrease the current  region for future access 
	
	
	page_table->load(); 	//Flush the TLB;
	
   // assert(false);
    Console::puts("Released region of memory.\n");
}


bool VMPool::is_legitimate(unsigned long _address) {
	
			
	unsigned int p=0;
	while(p< this->currentRegion){
			unsigned long base= this->container[p].relocatable_address;
			unsigned long limit = base +  this->container[p].offset;
			if(_address >= base&& _address <= limit) {
				// check to see the address is within the base and base+ limit
				//Console::puts("Checked whether address is part of an allocated region.\n");
				return 1;  // Means it is legitimate
			}
			p++;
	
	}
	
	
    //assert(false);
    Console::puts("Checked whether address is part of an allocated region.\n");
	return 0;
}

