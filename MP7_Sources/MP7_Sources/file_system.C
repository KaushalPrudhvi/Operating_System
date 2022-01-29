/*
     File        : file_system.C

     Author      : Riccardo Bettati
     Modified    : 2021/11/28

     Description : Implementation of simple File System class.
                   Has support for numerical file identifiers.
 */

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "assert.H"
#include "console.H"
#include "file_system.H"
#include "file.H"


/*--------------------------------------------------------------------------*/
/* CLASS Inode */
/*--------------------------------------------------------------------------*/

/* You may need to add a few functions, for example to help read and store 
   inodes from and to disk. */
extern FileSystem* FILE_SYSTEM;
/*--------------------------------------------------------------------------*/
/* CLASS FileSystem */
/*--------------------------------------------------------------------------*/
//used to find the total number of blocks
//resued from kernel.C
#define MB * (0x1 << 20)
#define SYSTEM_DISK_SIZE (10 MB)

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR */
/*--------------------------------------------------------------------------*/

FileSystem::FileSystem() {
    Console::puts("In file system constructor.\n");
	//initialise a bitmap
	memset(bitmap, 0, 512);
	
	//initailise inodes 
	inodes= (Inode*) bitmap;
    //assert(false);
}

FileSystem::~FileSystem() {
	/*It will be helpful to have a cached copy of the block that you are reading
       from and writing to. In the base submission, files have only one block, which
       you can cache here. You read the data from disk to cache whenever you open the
       file, and you write the data to disk whenever you close the file. 
    */
	//FILE_SYSTEM->disk->read(0,FILE_SYSTEM->block_cache);
    Console::puts("unmounting file system\n");
    /* Make sure that the inode list and the free list are saved. */
    //assert(false);
}


/*--------------------------------------------------------------------------*/
/* FILE SYSTEM FUNCTIONS */
/*--------------------------------------------------------------------------*/


bool FileSystem::Mount(SimpleDisk * _disk) {
    Console::puts("mounting file system from disk\n");

	disk=_disk;
	
	Console::puts("Disk Mounted\n");

	return true;
    /* Here you read the inode list and the free list into memory */
    
    //assert(false);
}

bool FileSystem::Format(SimpleDisk * _disk, unsigned int _size) { // static!
    Console::puts("formatting disk\n");
	Console::puts("\n Formatting disk takes 15seconds. Please wait for 15seconds\n");
	 /* Here you populate the disk with an initialized (probably empty) inode list
       and a free list. Make sure that blocks used for the inodes and for the free list
       are marked as used, otherwise they may get overwritten. */
    //assert(false);
	int i;
	//clear bitmap during format
	memset(bitmap, 0, 512);
	//clearing all data in disk 
	for(i=0; i<SYSTEM_DISK_SIZE/512;i++) 
		//write bitmap to block i
		_disk->write(i, bitmap);
	
	//read 0th block info that gives inode info
	_disk->read(0,  bitmap);
	for(i=0; i< MAX_INODES; i++){
		
		//used to initialise inodes
		//file id
		//starting block of that file
		//total block size of that particular file pointed by an inode
		FILE_SYSTEM->inodes[i].id=-1;
		FILE_SYSTEM->inodes[i].startBlock=0;
		FILE_SYSTEM->inodes[i].total=0;
		
	}
	// write bitmap to block 0  
	_disk->write(0, bitmap);
	
	//clear bitmap
	memset(bitmap, 0, 512);
	
	//read bitmap from block 1
	_disk->read(1, bitmap);
	
	for(i=0; i<512; i++){
	// declare all blocks as free
	bitmap[i]|=0xFF;
	
	
	}
	
	int j;
	
	//Mark 0th and 1st Block as used
	
	bitmap[0]=0x3F;
	//and write back to block 1
	_disk->write(1, bitmap);
	
	
	// Now the disk is formatted
	
	Console::puts("Disk Formatted\n");
	
	return true;
   
}


Inode* FileSystem::LookupFile(int _file_id) {
    Console::puts("looking up file with id = "); Console::puti(_file_id); Console::puts("\n");
	int i;
	File* f= NULL;
	
	memset(bitmap, 0, 512);
	// clear out bitmap
	disk->read(0, bitmap);
	// read from block 0 into bitmap
	for(i=0; i<MAX_INODES;i++){
		if(inodes[i].id==_file_id){
			// if the file exists 
			//get file
			Console::puts("\n file with id = "); Console::puti(_file_id); Console::puts("Found\n");
			f= (File*)new File(_file_id, inodes[i].startBlock,inodes[i].total, inodes[i].currentPosition);
			break;
		}
	}
	// get the file id for the file obtained above
	for(i=0; i<MAX_INODES;i++){
		if(inodes[i].id==_file_id){
			//Console::puts("\n file with id = "); Console::puti(_file_id); Console::puts("Found\n");
				return inodes;
		}
	}
	
	return NULL;
	
	
	
	/* Here you go through the inode list to find the file. */
    //assert(false);
}

bool FileSystem::CreateFile(int _file_id) {
   int i, j;
   i=0;
   //initialise block number as 0;
   int blockNum=0;
   //  define a mask to identify to identify free blocks
   unsigned char mask=0x80;
   
   // clear the bitmap
   memset(bitmap, 0, 512);
   disk->read(1, bitmap);
   
   
   //check for free blocks
   while(bitmap[i]==0){
	   i++;
   }
   
   blockNum+=i*8;
   //lop until a free block is found
   while((mask& bitmap[i])==0){
	   mask= mask>>1;
	   blockNum++;
   }
   // mask the bit
   bitmap[i]= bitmap[i]^mask;
   
   //write bitmap back to block 0
   disk->write(1, bitmap);
   
   memset(bitmap, 0, 512);
   
   //read bitmap from block 0
   disk->read(0, bitmap);
   
   //look up file for file_id
   if(LookupFile(_file_id))
	   return false;
   
   // loop through inodes to get a free inode
   for(i=0; i< MAX_INODES; i++){
	   /* Here you check if the file exists already. If so, throw an error.
       Then get yourself a free inode and initialize all the data needed for the
       new file. After this function there will be a new file on disk. */
   
	   if(inodes[i].id==-1){
		   
		   inodes[i].id= _file_id;
		   inodes[i].startBlock=blockNum;
		   inodes[i].total= 1;
		   inodes[i].currentPosition=0;
		   break;
	   }
   }
   
   //write bitmap back to block 0;
    disk->write(0, bitmap);
    
	
    Console::puts("creating file with id:"); Console::puti(_file_id); Console::puts("\n");
    
    return true;
}

bool FileSystem::DeleteFile(int _file_id) {
   int i;
   // temp starting block num;
   int startBlockNum;
   //total num of blocks;
   int noOfBlocks;
   //  define a char mask 0x80 to identify an allocated inode
   unsigned char mask=0x80;
   // clear bitmap
   memset(bitmap, 0, 512);
   // read bitmap to block 0
   disk->read(0, bitmap);
   //look up file with _file_id
   if(LookupFile(_file_id))
	   return false;
   //loop through inodes to find the required file
   for(i=0; i< MAX_INODES;i++){
	    /* First, check if the file exists. If not, throw an error. 
       Then free all blocks that belong to the file and delete/invalidate 
       (depending on your implementation of the inode list) the inode. */
		// loop if id is equal to _file_id
	   if(inodes[i].id==_file_id){
		   startBlockNum= inodes[i].startBlock;
		   noOfBlocks= inodes[i].total;
		   inodes[i].id==-1;
		   inodes[i].startBlock=-1;
		   inodes[i].total=-1;
		   inodes[i].currentPosition=0;
		   break;
	   }
   }
   //write back to block 0
   disk->write(0, bitmap);
   //after the block is identified free the blocks from bitmap
   freeBlocks(startBlockNum, noOfBlocks);
   
    Console::puts("deleting file with id:"); Console::puti(_file_id); Console::puts("\n");
   
   return true;
   
}


//Used to Get free blocks
// get free blocks from avaialable free blocks
int FileSystem::GetFreeBlock(int noOfBlocks, int id){
int i,j;
int startBlock=0;
int lastBlock=0;
int blockCount=0;
unsigned char mask =0x80;

memset(bitmap, 0, 512);
disk->read(1, bitmap);

for(i=0; i< SYSTEM_DISK_SIZE/512 &&blockCount <noOfBlocks;i++){
for( j=0; j<0 && blockCount<noOfBlocks;j++){
//loop through to get bitmap 
if((bitmap[i] &mask)!=0){
	// each character represents 8 blocks 
lastBlock=i*8+j;
   // increment the block num 
blockCount++;

}else{
blockCount=0;


}
//shift the mask
mask= mask>>1;


}

// mask set again to 0x80
mask=0x80;


}

// check if allocated number of blocks is equal to 1
if(noOfBlocks!=1)
	startBlock= lastBlock-noOfBlocks;
else
	startBlock= lastBlock;


//Make it used

for(j=startBlock; j<= lastBlock; j++){
	mask= 0x80>>(j%8);
	bitmap[j/8]^=mask;
}

//write bitmap to block 1
disk->write(1, bitmap);


// clear bitmap
memset(bitmap, 0, 512);
//read bitmap to block 0
disk->read(0, bitmap);


//  assign values to inodes[i]
for(i=0; i< MAX_INODES; i++){
	if(inodes[i].id==id){
	inodes[i].startBlock= startBlock;
	inodes[i].total= noOfBlocks;
	inodes[i].currentPosition=0;
	break;
	
	
	}
}


//write bitmap to block 0
disk->write(0, bitmap);

return startBlock;
  
  }
  
  
  
  int FileSystem::freeBlocks(int startBlockNum, int totalBlockNum){
    // Freeing the blocks;
   
   int block;
   
   unsigned char mask=0x80;
   memset(bitmap, 0, 512);
   disk->read(1, bitmap);
   //Make the blocks free
   for(block= startBlockNum; block<= startBlockNum+totalBlockNum-1; block++){
   mask= 0x80>>(block%8);
   bitmap[block/8]|= mask;
   }
   
   disk->write(1, bitmap);
   return true;

  }
  
  
  
  
  




