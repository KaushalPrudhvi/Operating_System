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
	
  // assert(false);
   Console::puts("Initialized Paging System\n");
}

PageTable::PageTable()
{
	
	//Give one frame to page to page table directory;
	page_directory = (unsigned long*) (kernel_mem_pool->get_frames(1) * PAGE_SIZE);
	//Now page directory is initialised.
	
	// Allocate a page table in the kernel pool itself as instructed
	// Assign one frame to a page table to store all the entires of  4MB 
	unsigned long *pageTable = (unsigned long *)(kernel_mem_pool->get_frames(1) * PAGE_SIZE);
	
	
	unsigned long kernelAddress;
	
	
	// Map the first 4MB directly to physical memory
	
	int i=0;
	while(i<1024){
		pageTable[i]= kernelAddress|0x00000003; //  set to supervisor , read/write, present(last three bits to 011)
		kernelAddress+= 4096; // each page table represents a page of size 4KB = 4096;
		i+=1;
	}
	
	
	//store this pageTable in the page_directory;
	page_directory[0]= (unsigned long) pageTable;
	
	page_directory[0]= page_directory[0] | 0x00000003;  //set to supervisor , read/write, present(last three bits to 011)
	
	// Mark the remanining entries of the page_directory array 
	//to supervisor, read/write and not present i.e last three bits to be 010;
		
	for(int i = 1; i< 1024; i++){
		page_directory[i] = 0x00000002;
	}
	
	
   //assert(false);
   Console::puts("Constructed Page Table object\n");
}


void PageTable::load()
{
	
	
	// Referring the pageTable to the current object using "this" pointer;
	
	// This pointer can be used for further functions
	current_page_table= this;// references the current object that calls this function
   //assert(false);
   Console::puts("Loaded page table\n");
}

void PageTable::enable_paging()
{
	
	// Set the paging enabled variable to 1;
	paging_enabled=1;
	
	
	//also the CR3 should contain the page_directory address
	unsigned long directory = (unsigned long) current_page_table->page_directory;
	write_cr3(directory);
	
	// To enable paging we need to set the CR0 register 
	unsigned long cr0= read_cr0(); // read the current cr0 register value
	cr0= cr0|0x80000000;// set the paging bit 32 to 1 to enable paging
	write_cr0(cr0);//  write the value of cr0 after setting the paging bit;
	
	
   //assert(false);
   Console::puts("Enabled paging\n");
}

void PageTable::handle_fault(REGS * _r)
{
	// This is triggered when a page referenced is not in the page table;
	// Hence read the address by read_cr2() function 
	

	unsigned long * pageTable;
	unsigned long indexInDirectory, indexInPageTable;
	unsigned long *processFrame;
	
	
	// 0xFFFFF000 is used as a directory mask
	//0x3FF is used as a page table mask 
	indexInDirectory = read_cr2() >> 22; // Leaves us with the 10 most MSB bits
	indexInPageTable = (read_cr2() >> 12) & 0x000003FF;// Leaves us with the 10 bits after the page_directory index bits
		
	// Check if the page is present by checking its present/not present bit
	if(!(_r->err_code & 1)){
		// page is not present hence derive the 
		// 1) page directory index (10 MSB bits)
		// 2) Page Table index bits from next 10 MSB bits
		// Next bits are then processed as the page is fetched 
		
		
		// Now we need to get the pageTable first by using indexInDirectory
		pageTable= (unsigned long*)((current_page_table->page_directory[indexInDirectory]) & (0xFFFFF000));
		// Now that we have got a pageTable check whether it is present or not
		if(!((unsigned long)pageTable & 0x00000001)) {
			// That means the pageTable requested is not present 
			//Hence we need to create a new page Table by allocating a frame
			pageTable = (unsigned long*)(kernel_mem_pool->get_frames(1)*PAGE_SIZE);
			// Now allocate a frame from process pool and then store it in the pageTable
			processFrame=(unsigned long*)(process_mem_pool->get_frames(1) *PAGE_SIZE);
			pageTable[0]= (unsigned long)processFrame;
			//Also mark the necessary supervisor, read/write and present bits i.e. 011;
			pageTable[0]|= 0x00000003;
			//And mark rest all entries as not present in that pageTable (rem.1023)
			for(int i=1;i<1024;i++){
				pageTable[i]= 0;
				// Marking the relevant bits to supervisor, read/write and not present i.e 010;
				pageTable[i]|= 0x00000002;
			}
			// After the pageTable is filled refer this pageTable in the pageDirectory
			current_page_table-> page_directory[indexInDirectory]=(unsigned long) pageTable;
			// and mark the necessary bits to supervisor, read/write and present i.e. 011
		(current_page_table->page_directory[indexInDirectory ])|= 0x00000003;
			
		}
		else if(!(pageTable[indexInPageTable &0x3FF] & 1))
			// PageTable is present but the page in the pageTable is not present 
			// Then we allocate a frame and put it in the relevant index
			pageTable= (unsigned long*) (current_page_table->page_directory[indexInDirectory] &0xFFFFF000);
			processFrame= (unsigned long*)(process_mem_pool->get_frames(1)*PAGE_SIZE);
			pageTable[indexInPageTable& 0x3FF] = (unsigned long)processFrame;
			// Now mark the relevant supervisor, read/write, present bits i.e. 011
			pageTable[indexInPageTable &0x3FF]|= 0x00000003;
			
		}
	
	


  //assert(false);
  Console::puts("handled page fault\n");
}
