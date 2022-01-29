/*
 File: ContFramePool.C
 
 Author:
 Date  : 
 
 */

/*--------------------------------------------------------------------------*/
/* 
 POSSIBLE IMPLEMENTATION
 -----------------------

 The class SimpleFramePool in file "simple_frame_pool.H/C" describes an
 incomplete vanilla implementation of a frame pool that allocates 
 *single* frames at a time. Because it does allocate one frame at a time, 
 it does not guarantee that a sequence of frames is allocated contiguously.
 This can cause problems.
 
 The class ContFramePool has the ability to allocate either single frames,
 or sequences of contiguous frames. This affects how we manage the
 free frames. In SimpleFramePool it is sufficient to maintain the free 
 frames.
 In ContFramePool we need to maintain free *sequences* of frames.
 
 This can be done in many ways, ranging from extensions to bitmaps to 
 free-lists of frames etc.
 
 IMPLEMENTATION:
 
 One simple way to manage sequences of free frames is to add a minor
 extension to the bitmap idea of SimpleFramePool: Instead of maintaining
 whether a frame is FREE or ALLOCATED, which requires one bit per frame, 
 we maintain whether the frame is FREE, or ALLOCATED, or HEAD-OF-SEQUENCE.
 The meaning of FREE is the same as in SimpleFramePool. 
 If a frame is marked as HEAD-OF-SEQUENCE, this means that it is allocated
 and that it is the first such frame in a sequence of frames. Allocated
 frames that are not first in a sequence are marked as ALLOCATED.
 
 NOTE: If we use this scheme to allocate only single frames, then all 
 frames are marked as either FREE or HEAD-OF-SEQUENCE.
 
 NOTE: In SimpleFramePool we needed only one bit to store the state of 
 each frame. Now we need two bits. In a first implementation you can choose
 to use one char per frame. This will allow you to check for a given status
 without having to do bit manipulations. Once you get this to work, 
 revisit the implementation and change it to using two bits. You will get 
 an efficiency penalty if you use one char (i.e., 8 bits) per frame when
 two bits do the trick.
 
 DETAILED IMPLEMENTATION:
 
 How can we use the HEAD-OF-SEQUENCE state to implement a contiguous
 allocator? Let's look a the individual functions:
 
 Constructor: Initialize all frames to FREE, except for any frames that you 
 need for the management of the frame pool, if any.
 
 get_frames(_n_frames): Traverse the "bitmap" of states and look for a 
 sequence of at least _n_frames entries that are FREE. If you find one, 
 mark the first one as HEAD-OF-SEQUENCE and the remaining _n_frames-1 as
 ALLOCATED.

 release_frames(_first_frame_no): Check whether the first frame is marked as
 HEAD-OF-SEQUENCE. If not, something went wrong. If it is, mark it as FREE.
 Traverse the subsequent frames until you reach one that is FREE or 
 HEAD-OF-SEQUENCE. Until then, mark the frames that you traverse as FREE.
 
 mark_inaccessible(_base_frame_no, _n_frames): This is no different than
 get_frames, without having to search for the free sequence. You tell the
 allocator exactly which frame to mark as HEAD-OF-SEQUENCE and how many
 frames after that to mark as ALLOCATED.
 
 needed_info_frames(_n_frames): This depends on how many bits you need 
 to store the state of each frame. If you use a char to represent the state
 of a frame, then you need one info frame for each FRAME_SIZE frames.
 
 A WORD ABOUT RELEASE_FRAMES():
 
 When we releae a frame, we only know its frame number. At the time
 of a frame's release, we don't know necessarily which pool it came
 from. Therefore, the function "release_frame" is static, i.e., 
 not associated with a particular frame pool.
 
 This problem is related to the lack of a so-called "placement delete" in
 C++. For a discussion of this see Stroustrup's FAQ:
 http://www.stroustrup.com/bs_faq2.html#placement-delete
 
 */
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "cont_frame_pool.H"
#include "console.H"
#include "utils.H"
#include "assert.H"

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
/* METHODS FOR CLASS   C o n t F r a m e P o o l */
/*--------------------------------------------------------------------------*/

// Definition of empty constructor
ContFramePool::ContFramePool()
{
     // Blank Constructor
}


ContFramePool::ContFramePool(unsigned long _base_frame_no,
                             unsigned long _n_frames,
                             unsigned long _info_frame_no,
                             unsigned long _n_info_frames)
{




	// TODO: IMPLEMENTATION NEEEDED!

	assert(_n_frames <= FRAME_SIZE * 4); // Check if the total frames fill in the available memory . Here it is 4KB * 4 =16K frames
	//initialising the variables using constructor values
	base_frame_no = _base_frame_no;
	n_frames = _n_frames;
	nFreeFrames = n_frames;
	info_frame_no = _info_frame_no;
	n_info_frames = _n_info_frames;
	
	if(info_frame_no == 0) {
        bitmap = (unsigned char *) (base_frame_no * FRAME_SIZE); //Initializes the bitmap to base_frame_no
    } else {
        bitmap = (unsigned char *) (info_frame_no * FRAME_SIZE);  // Go the frame pointed by the info_frame_no;
    }
	
	// Number of frames must be "fill" the bitmap!
    assert ((n_frames % 4 ) == 0);    // Here 4 is used because we are using two bits to represent one frame in a character bitmap
	
	 // Everything ok. Proceed to mark all bits in the bitmap
    int i;
    for(i=0; i*4 < n_frames; i++) { // 2 bits per frame are marked
        bitmap[i] = 0xFF;
    }
    
    if(info_frame_no == 0) {          
	// After all the frames are marked as free, use the next location to store No. of Free Frames count
	
	// For the sake of convenience let us store the number of free frames in a location after bitmap is filled.
	// We are defining a pointer nFreeFrame to nFreeFrames to modify using its reference
        ptrTonFreeFrames = (unsigned long *) (base_frame_no *FRAME_SIZE + i);
    } else {
         ptrTonFreeFrames = (unsigned long *) (info_frame_no *FRAME_SIZE + i);
    }
   
    
    *ptrTonFreeFrames = nFreeFrames;
    // Mark the first frame as being used if it is being used
    bitmap[0] = 0x3F;    // 
   (*ptrTonFreeFrames)--;
    
    
    Console::puts("Frame Pool initialized\n");

    // TODO: IMPLEMENTATION NEEEDED!
    
    
    
    
    
    
    
    
   // Console::puts("ContframePool::Constructor not implemented!\n");
    //assert(false);
}

unsigned long ContFramePool::get_frames(unsigned int _n_frames)
{
    // TODO: IMPLEMENTATION NEEEDED!
    
    
    
    assert(*ptrTonFreeFrames > 0);   //Check to see if there are requested number of free frames 
	
	unsigned int frame_no = base_frame_no;
	
	unsigned int i = 0, j = 0;
	unsigned int headPosition; 		//helps in gettingi index of head Position at the end to return the starting frame number
    unsigned int offset; 
	unsigned int seqCount = 0;		//current count for sequence if they are free
	unsigned int location = 0;		// location in a particular character
	unsigned char mask = 0xC0;
	unsigned char nextMask = 0xC0;
	unsigned int count = 0;			// Count to end if the desired number is reached
	
	
	//Loop until we get n free spots
	while((count < _n_frames ))
	{
	    // While loop to reach a first free frame checking for all conditions "11" in 4 places of a character
		while(((bitmap[i]&mask) != 0xC0) && ((bitmap[i]&mask) != 0x30) && ((bitmap[i]&mask) != 0x0C) && ((bitmap[i]&mask) != 0x03))
		{
			mask = mask >> 2;
			if(mask == 0x00)
			{
				i++;
				mask = 0xC0;
			}
			location = (location+1)%4;
			
		}

		headPosition = i;
		nextMask = mask;
		offset = location;
        
		// After a free frame is found, loop to check if the next n-1 frames are free or not 
		while((bitmap[i] & mask == mask) && (seqCount < _n_frames ))
		{
			seqCount++;
			mask = mask >> 2;
			location = (location + 1) % 4;
			if(mask == 0x00)
			{
				// shift to next index if the previous mask is at the end of a character
				i++;
				mask = 0xC0;
			}
		}
		
		if(seqCount == _n_frames)
		{
			count = seqCount;
		}
		else
		{
			seqCount = 0;
		}
	}
	count = 1;
	bitmap[headPosition] = bitmap[headPosition] ^ (nextMask & 0xAA);  // Mark as head "01" the first in sequence 
	(*ptrTonFreeFrames)--;
	frame_no = frame_no + headPosition * 4 + offset;
	nextMask = nextMask >> 2;
	
	mask = nextMask;
	j = headPosition;
	
	if(nextMask == 0x00)
	{
		mask = 0xC0;
		j = headPosition + 1;
	}
	
	while(count < _n_frames)   // Marking the rest of the frames as allocated  by sending "00" by XOR operation
	{ 
		bitmap[j] = bitmap[j] ^ mask;
		(*ptrTonFreeFrames)--;
		mask = mask >> 2;
		if(mask == 0x00)
		{
			mask = 0xC0;
			j++;
		}
		count++;
	}
    return (frame_no);  // Returns the first frame after marking is done
    //Console::puts("ContframePool::get_frames not implemented!\n");
   // assert(false);
}

void ContFramePool::mark_inaccessible(unsigned long _base_frame_no,
                                      unsigned long _n_frames)
{
    unsigned int i = 0, location = 0, count = 0;
    unsigned mask = 0xC0;
    i = _base_frame_no / 4;
    location = _base_frame_no % 4;
    mask = mask >> (location*2);

    bitmap[i] = bitmap[i] ^ (mask & 0x55);   // Marking Head of inaccessible area 
    (*ptrTonFreeFrames)--;
    count++;
    
   
    while(count < _n_frames)
    {
	 mask = mask >> 2;
  	 if(mask == 0x00)
    	 {
	     mask = 0xC0;
             i++;
         }
         bitmap[i] = bitmap[i] ^ mask;        // Marking the rest of the frames as allocated in the sequence staring from the head of the seq
         count++;
         (*ptrTonFreeFrames)--;
    }
   
   
    // TODO: IMPLEMENTATION NEEEDED!
    //Console::puts("ContframePool::mark_inaccessible not implemented!\n");
    //assert(false);
}

void ContFramePool::release(unsigned char *bitmap,unsigned long _first_frame_no, unsigned long BASE_ADDRESS)
{
    unsigned long d=0,i=0;
    unsigned char mask = 0xC0;

    i = (_first_frame_no-BASE_ADDRESS)/4;      // Get the index in bitmap
    d = (_first_frame_no-BASE_ADDRESS)%4;		// Get the offset in that particular index

    mask = mask >> (d*2);
    
	//Assert if the first frame being freed is not head and report
    assert(((bitmap[i] & mask) == 0x40) || ((bitmap[i] & mask) == 0x10) || ((bitmap[i] & mask) == 0x04) || ((bitmap[i] & mask) == 0x01));

    bitmap[i] = bitmap[i] | (mask);    // releasing  the head
    (*ptrTonFreeFrames)++;
    mask = mask >> 2;
    if(mask == 0x00)
    {
	mask = 0xC0;
	i++;
    }
    
    while((bitmap[i]&mask)==0x00)      // Releasing the next frames in sequence while looping for next head of sequence
    {
    	bitmap[i] = bitmap[i] | mask;
        (*ptrTonFreeFrames)++;
	mask = mask >> 2;
        if(mask == 0x00)
	{
	    mask = 0xC0;
	    i++;
	}
        
    }  
}  




void ContFramePool::release_frames(unsigned long _first_frame_no)
{
    
    unsigned long n=0,i=0;
    unsigned char mask = 0xC0;   
    ContFramePool dummy;   // Define a reference that can be used to access the private members of a given pool
    if(_first_frame_no > KERNEL_POOL_START_FRAME && _first_frame_no < PROCESS_POOL_START_FRAME)  // then its kernel pool
    {
    	dummy.bitmap = (unsigned char *) (KERNEL_POOL_START_FRAME  * FRAME_SIZE);
        dummy.ptrTonFreeFrames = (unsigned long *)(KERNEL_POOL_START_FRAME * FRAME_SIZE + KERNEL_POOL_SIZE/4 );
        
		//The following condition check helps us in identifying whether we have reached the management frames and breaks if we try to release them
		if(*dummy.ptrTonFreeFrames < (KERNEL_POOL_SIZE-2)){
        return;
        }
	dummy.release(dummy.bitmap,_first_frame_no,KERNEL_POOL_START_FRAME);
        
    }
    else
    {
    
    
   // the process pool
        dummy.bitmap = (unsigned char *) (PROCESS_POOL_START_FRAME  * FRAME_SIZE);
        dummy.ptrTonFreeFrames = (unsigned long *)((KERNEL_POOL_START_FRAME+1) * FRAME_SIZE + PROCESS_POOL_SIZE/4 );
         assert(*dummy.ptrTonFreeFrames < PROCESS_POOL_SIZE);
	dummy.release(dummy.bitmap,_first_frame_no,PROCESS_POOL_START_FRAME);
    }
    
    
    
    
    
    
    
    // TODO: IMPLEMENTATION NEEEDED!
    //Console::puts("ContframePool::release_frames not implemented!\n");
    //assert(false);
}

unsigned long ContFramePool::needed_info_frames(unsigned long _n_frames)
{
    // TODO: IMPLEMENTATION NEEEDED!
    
    return(_n_frames / ((16 KB) + ((_n_frames % (16 KB)) > 0 ? 1 : 0)) + 1); //If we use 1 bit we can map 32K frames and 16K if 2 bits per frame are used.
    //Console::puts("ContframePool::need_info_frames not implemented!\n");
   // assert(false);
}
