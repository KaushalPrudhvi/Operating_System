/*
     File        : file.C

     Author      : Riccardo Bettati
     Modified    : 2021/11/28

     Description : Implementation of simple File class, with support for
                   sequential read/write operations.
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
#include "file.H"

//extern FileSystem* this->fs;
/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR/DESTRUCTOR */
/*--------------------------------------------------------------------------*/

File::File(FileSystem *_fs, int _id) {
	//refer to the filesystem pointer
	this->fs= _fs;
	id=_id;
	
    Console::puts("Opening file.\n");
    //assert(false);
}

File::~File() {
    Console::puts("Closing file.\n");
    /* Make sure that you write any cached data to disk. */
    /* Also make sure that the inode in the inode list is updated. */
}


File::File(int _id, int _startBlock, int _total, int _currentPosition){

//creates a new file
	id=_id;
	currentPosition= currentPosition;
	total= _total;
	startBlock= _startBlock;
	currentBlock= startBlock;


}
/*--------------------------------------------------------------------------*/
/* FILE FUNCTIONS */
/*--------------------------------------------------------------------------*/

int File::Read(unsigned int _n, char *_buf) {
    Console::puts("reading from file\n");
	int size=_n;
	int len=0;
	unsigned char readBuf[512];
	int i;
	int readCount;
	
	while(size>0){
		//clear buffer
		memset(readBuf, 0, 512);
		//read current block into buffer
		this->fs->disk->read(currentBlock, readBuf);
		for(i=0; i<size; i++)
		{
		memcpy(_buf++, readBuf+currentPosition++,1);
		readCount++;
		//check for endoffile
		if(EoF()){
		currentPosition=0;
		currentBlock++;
		break;
		
		
		}
		}
		
		size-=readCount;
		
		
		
	}
		Console::puts("Done reading from file\n");
	//return number of bytes read
		return _n;
		
}

int File::Write(unsigned int _n, const char *_buf) {
    Console::puts("writing to file\n");
	int size=_n;
	unsigned char writeBuf[512];
	//temporary variables to keep track of start and current incase 
	int tempStart;
	int tempCurrent;
	int i;
	int writeCount;
	//clear the buffer
	memset(writeBuf, 0, 512);
	//check if blocksize exceeds basing on the current position
	if(size > 512-currentPosition-1){
		writeCount= 512-currentPosition-1;
		memcpy(writeBuf+currentPosition, _buf, writeCount);
		this->fs->disk->write(currentBlock, writeBuf+currentPosition);
	
		tempStart= this->fs->GetFreeBlock(total+1, id);
		tempCurrent= tempStart;
		for(i=startBlock; i<= startBlock+total-1; i++){
		memset(writeBuf, 0, 512);
		//read into buffer from the ith block
		this->fs->disk->read(i, writeBuf);
		
		//write buffer into tempCurrent
		this->fs->disk ->write(tempCurrent++, writeBuf);
		
		
		}
		
		//Free the old blocks
		
		this->fs->freeBlocks(startBlock, total);
		//update startBlock and currentBlock
		total= total+1;
		currentPosition=0;
		currentBlock= tempCurrent;
		currentBlock= tempStart+currentBlock-startBlock;
		
		startBlock= tempStart;
		
		writeCount=size- writeCount;
		// copy into the currentPosition
		memcpy(writeBuf+currentPosition, _buf, writeCount);
		
		//write into disk
		this->fs->disk->write(currentBlock, writeBuf+currentPosition);
		//update current Position
		
		currentPosition+=writeCount;
	}
	else{
	
	//copy to new blocks
	memcpy(writeBuf+currentPosition, _buf, size);
	//write into the currentBlock
	this->fs->disk->write(currentBlock, writeBuf+currentPosition);
	currentPosition+=size;
	
	
	}
	Console::puts("Done writing to file\n");
	
	
	return _n;

}

void File::Reset() {
    Console::puts("resetting file\n");
    //set back currentPosition to 0
	currentPosition=0;
	
	//assert(false);
}

bool File::EoF() {
   
	//check if currentPosition has reached end of block
	if(currentPosition==512-1){
		 Console::puts("checking for EoF\n");
	
		return true;
	}
	else
		return false;
  //  assert(false);
}
