#include "assert.H"
#include "exceptions.H"
#include "console.H"
#include "paging_low.H"
#include "page_table.H"

PageTable * PageTable::current_page_table = NULL;
unsigned int PageTable::paging_enabled = 0;
ContFramePool * PageTable::kernel_mem_pool = NULL;
ContFramePool * PageTable::process_mem_pool = NULL;
unsigned long PageTable::shared_size = 0;


void PageTable::init_paging(ContFramePool * _kernel_mem_pool,
                            ContFramePool * _process_mem_pool,
                            const unsigned long _shared_size)
{
	kernel_mem_pool= _kernel_mem_pool;
	process_mem_pool= _process_mem_pool;
	shared_size= _shared_size;
	
	
    //assert(false);
    Console::puts("Initialized Paging System\n");
}

PageTable::PageTable()
{
	
	
	//allocate one page for directory that to be a page size as in handout from process pool
	page_directory = (unsigned long*) (process_mem_pool->get_frames(1) *PAGE_SIZE);
	
	//Now page directory is initialised.
	
	// Allocate a page table in the process pool itself as instructed
	// Assign one page to a page table to store pages and entry this in directory
	unsigned long *pageTable = (unsigned long *)(process_mem_pool->get_frames(1) * PAGE_SIZE);
	
	
	unsigned long mappedAddress=0;
	
	
	// Map the first 4MB directly to physical memory
	
	int i=0;
	while(i<ENTRIES_PER_PAGE){
		pageTable[i]= mappedAddress|0x00000003; //  set to supervisor , read/write, present(last three bits to 011)
		mappedAddress+= 4096; // each page table represents a page of size 4KB = 4096;
		i+=1;
	}
	
	
	//store this pageTable in the page_directory;
	page_directory[0]= (unsigned long) pageTable;
	
	page_directory[0]= page_directory[0] | 0x00000003;  //set to supervisor , read/write, present(last three bits to 011)
	
	// Mark the remanining entries of the page_directory array 
	//to supervisor, read/write and not present i.e last three bits to be 010;
		
	for(int i = 1; i< ENTRIES_PER_PAGE; i++){
		page_directory[i] = 0x00000002;
	}
	 

	
	page_directory[1023] = (unsigned long ) (page_directory )|0x00000003; // set to supervisor, read, present
	
vmpoolcount=0;// Initialized the pools  count to 0
	// initialise ll the pool entry pointers to NULL and update them as pools are allocated
	for(int p=0; p< pools; p++){
	virtualpools[p] = NULL;
}


   //assert(false);
   Console::puts("Constructed Page Table object\n");
	
	
   
}



void PageTable::load()
{
	
	
	// Referring the pageTable to the current object using "this" pointer;
	
	// This pointer can be used for further functions
	current_page_table= this;
	// references the current object that calls this function
	
	//also the CR3 should contain the page_directory address
	unsigned long directory = (unsigned long) current_page_table->page_directory;
		write_cr3(directory);
   //assert(false);
   Console::puts("Loaded page table\n");
}

void PageTable::enable_paging()
{
	
	// Set the paging enabled variable to 1;
	paging_enabled=1;
	
	

	
	// To enable paging we need to set the CR0 register 
	unsigned long cr0= read_cr0(); // read the current cr0 register value
	cr0= cr0|0x80000000;// set the paging bit 32 to 1 to enable paging
	write_cr0(cr0);//  write the value of cr0 after setting the paging bit;
	
	
   //assert(false);
   Console::puts("Enabled paging\n");
}






void PageTable::handle_fault(REGS * _r)
{
	
	// This is triggered when a page referenced is not in the page table
	// Hence read the address by read_cr2() function 
	

	unsigned long * pageTable = NULL;
	unsigned long indexInDirectory, indexInPageTable;
	unsigned long processFrame;
	
	
	// 0xFFFFF000 is used as a directory mask
	//0x3FF is used as a page table mask 
	indexInDirectory = read_cr2() >> 22; 
	// Leaves us with the 10 most MSB bits
	indexInPageTable = (read_cr2() >> 12) &0x000003FF ;
	// Leaves us with the 10 bits after the page_directory index bits
   
	
	
	unsigned long* 	Directory=current_page_table->PDE_address(read_cr2()); // set to the starting adddress of the directory
	
	if((_r->err_code & 0x01)==0){
		int position=-1;	 
		// this is used to get the desired pool index
		// This loop is used to get the pool of the address that trggered an exception
		for(unsigned int i =0; i< current_page_table->vmpoolcount; i++){
			if( current_page_table->virtualpools[i] != NULL){
				if( current_page_table->virtualpools[i]->is_legitimate(read_cr2())){
					position= i;
					// break after the pool has been found if and only if the address is legitimate
					break;
				}
			}
		}	
		// Do a check to see if the pool is not yet initialised
		// assert if the address does not belong to one of the legitimate addresses and trap
		if(position < 0){
			assert(true);
		}
			if((Directory[indexInDirectory] &0x01)==1){
			
			
			// This means directory has  present  bit set and go further to deal with the page tables
			//That means there is a fault in the table
			// Get the address of the page Table
			
			//Doing the recursive lookup;
		
			pageTable=current_page_table->PTE_address(read_cr2());
				//Mask the LSB bits to supervisor, read, present;
			processFrame= (unsigned long )(process_mem_pool->get_frames(1)*PAGE_SIZE) |0x00000003;
			
		
			
			pageTable[indexInPageTable]= (processFrame);
		
			
		}
		else{ 
			// Create a new pageTable directory entry;
			
			unsigned long tableEntry= (process_mem_pool->get_frames(1) * PAGE_SIZE) ;//| 0x00000003;
			// Now store this in directory
			
			Directory[indexInDirectory ] = (unsigned long)tableEntry;
			
			
		    pageTable=current_page_table->PTE_address(read_cr2());
			
			processFrame= (unsigned long)(process_mem_pool->get_frames(1)*PAGE_SIZE)|0x00000003;
			
			//Mask the LSB bits to supervisor, read, present;
			
			
			pageTable[indexInPageTable]= ( processFrame);
		
			
			
			// Now set the remaining entries in the pageTable now created to invalid
			// Also mark the lower LSB bits to supervisor, read and not present (010)
			for(int i=1; i< ENTRIES_PER_PAGE; i++){
				pageTable[i]= 0|0x00000002;
			}
			
		} 
	
	
	
	}

   // assert(false);
    Console::puts("handled page fault\n");

}

void PageTable::register_pool(VMPool * _vm_pool)
{
	
	
	if((!this->virtualpools[this->vmpoolcount])  && (this->vmpoolcount < pools)){
		this->virtualpools[(this->vmpoolcount)++]= _vm_pool;
		// store it in the array of pools and increment the count whenever a new pool is registered
		
	}
	
	else{
		// vmpool count might have exceeded the preset count(500 here) and hence cannot register a new pool
		Console::puts("Too many pools already existing");
	}
    //assert(false);
    Console::puts("registered VM pool\n");
}

void PageTable::free_page(unsigned long _page_no) {
	unsigned long indexInDirectory= _page_no/ENTRIES_PER_PAGE; // To get the page directory number
	unsigned long indexInPageTable= _page_no%ENTRIES_PER_PAGE; // To get the offset in the page directory which gives the pageTable

	 unsigned long* pageTable=current_page_table->PTE_address(read_cr2());
	unsigned long processFrame= (unsigned long) ((pageTable[indexInPageTable]&0xFFFFF000)/ PAGE_SIZE);
	
	ContFramePool::release_frames(processFrame);
	// Also clear the pageTable entry corresponding to the freed entry
	// Marking the entry invalid
	pageTable[indexInPageTable] &= 0xFFFFFFFE;
	
	
    //assert(false);
    Console::puts("freed page\n");
}

// PDE recursive lookup
unsigned long* PageTable:: PDE_address(unsigned long addr){
	
	return (unsigned long*)0xFFFFF000;
	
	
	
	
}
// PTE recursive lookup
unsigned long* PageTable:: PTE_address(unsigned long addr){
	unsigned long pdeShift = addr>> 22; // Leaves us with the 10 most MSB bits
	unsigned long pteShift= (addr>> 12) &0x000003FF ;// Leaves us with the 10 bits after the page_directory index bits
   
	unsigned long * recursivePTE= (unsigned long*)((0x000003FF<<22)| pdeShift<<12); 
	return recursivePTE;
	
	
	
	
	
}
