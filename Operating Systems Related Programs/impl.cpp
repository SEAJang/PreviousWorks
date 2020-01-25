// CPSC457 Fall 2017, University of Calgary
// Skeleton C++ program for Q7 of Assignment 5.
//
// The program reads in the input, then calls the (wrongly implemented) checkConsistency()
// function, and finally formats the output.
//
// You only need to reimplement the checkConsistency() function.
//
// Author: Pavol Federl (pfederl@ucalgary.ca or federl@gmail.com)
// Date: November 29, 2017
// Version: 1

#include <stdio.h>
#include <string>
#include <vector>

typedef std::string SS;
typedef std::vector<SS> VS;

struct DEntry {
    SS fname = SS( 4096, 0);
    int size = 0;
    int ind = 0;
    bool tooManyBlocks = false;
    bool tooFewBlocks = false;
    bool hasCycle = false;
    bool sharesBlocks = false;
	std::vector<int> blocks;

};

static SS join( const VS & toks, const SS & sep) {
    SS res;
    bool first = true;
    for( auto & t : toks) { res += (first ? "" : sep) + t; first = false;}
    return res;
}

//takes in blocksize, a DEntry, and the FAT
void solveBlock(int blocksize, DEntry &f, std::vector<int> & fat)
{
	f.blocks = std::vector<int>();		//initializes the vector used to figure out which blocks the file uses
	int current = f.ind;
	bool loop = true;
	if(current == -1)			//if file starts out with a null pointer block (special case)
	{
		loop = false;
	}
	while(loop)
	{
		bool invec = false;		//used to figure out if block is in the vector

		for(auto & i : f.blocks)
		{
			if( i == current)
			{
				invec = true;
				f.hasCycle = true;		//has a cycle
				loop = false;
				break;
			}
		}
		if(!invec)
		{
			f.blocks.push_back(current);
		}
		current = fat[current];
		if(current == -1)
		{
			loop = false;
		}
	}
	int64_t tsize = blocksize * f.blocks.size();		//calculate the total size
	int64_t tempi = tsize - f.size;
	if(tempi < 0)
	{
		f.tooFewBlocks = true;
	}
	else if(tempi >= blocksize)
	{
		f.tooManyBlocks = true;
	}
}
int checkShare( std::vector<DEntry> & files, std::vector<int> & fat)
{
	int counter[fat.size()];
	for(auto & c : counter)			//initialize counter array
	{
		c = 0;
	}
	int notUsed = 0;			//initialize counter to keep track of unused blocks
	for(auto & f : files)			//increment counters for blocks which are used by files
	{
		for(auto const & bv : f.blocks)
		{
			counter[bv]++;
		}
	}
	for(auto & c : counter)			//count how many not used
	{
		if(c <= 0)
		{
			notUsed++;
		}
	}
	for(auto & f : files)			//for each file, find out if it has any blocks that coincide with another
	{
		for(auto const & bv : f.blocks)
		{
			if(counter[bv] > 1)
			{
				f.sharesBlocks = true;
				break;
			}
		}
	}
	return notUsed;
}

// Parameters:
//   blockSize - contains block size as read in from input
//   files - array containing the entries as read in from input
//   fat - array representing the FAT as read in from input
// Return value:
//   the function should return the number of free blocks
//   also, for ever entry in the files[] array, you need to set the appropriate flags:
//      i.e. tooManyBlocks, tooFewBlocks, hasCycle and sharesBlocks
int checkConsistency( int blockSize, std::vector<DEntry> & files, std::vector<int> & fat)
{
	for(auto & f : files)
	{
		solveBlock(blockSize, f, fat);
	}
	int notUsed;
	notUsed = checkShare(files, fat);
    return notUsed;
}
int main()
{
    try {
        // read in blockSize, nFiles, fatSize
        int blockSize, nFiles, fatSize;
        if( 3 != scanf( "%d %d %d", & blockSize, & nFiles, & fatSize))
            throw "cannot read blockSize, nFiles and fatSize";
        if( blockSize < 1 || blockSize > 1024) throw "bad block size";
        if( nFiles < 0 || nFiles > 50) throw "bad number of files";
        if( fatSize < 1 || fatSize > 200000) throw "bad FAT size";
        // read in the entries
        std::vector<DEntry> entries;
        for( int i = 0 ; i < nFiles ; i ++ ) {
            DEntry e;
            if( 3 != scanf( "%s %d %d", (char *) e.fname.c_str(), & e.ind, & e.size))
                throw "bad file entry";
            e.fname = e.fname.c_str();
            if( e.fname.size() < 1 || e.fname.size() > 16)
                throw "bad filename in file entry";
            if( e.ind < -1 || e.ind >= fatSize) throw "bad first block in fille entry";
            if( e.size < 0 || e.size > 1073741824) throw "bad file size in file entry";
            entries.push_back( e);
        }
        // read in the FAT
        std::vector<int> fat( fatSize);
        for( int i = 0 ; i < fatSize ; i ++ ) {
            if( 1 != scanf( "%d", & fat[i])) throw "could not read FAT entry";
            if( fat[i] < -1 || fat[i] >= fatSize) throw "bad FAT entry";
        }

        int nFreeBlocks = checkConsistency( blockSize, entries, fat);
        size_t maxflen = 0;
        for( auto & e : entries ) maxflen = std::max( maxflen, e.fname.size());
        SS fmt = "  %" + std::to_string( maxflen) + "s: %s\n";

        printf( "Issues with files:\n");
        for( auto & e : entries ) {
            VS issues;
            if( e.tooFewBlocks) issues.push_back( "not enough blocks");
            if( e.tooManyBlocks) issues.push_back( "too many blocks");
            if( e.hasCycle) issues.push_back( "contains cycle");
            if( e.sharesBlocks) issues.push_back( "shares blocks");
            printf( fmt.c_str(), e.fname.c_str(), join( issues, ", ").c_str());
        }
        printf( "Number of free blocks: %d\n", nFreeBlocks);

    }
    catch( const char * err) {
        fprintf( stderr, "Error: %s\n", err);
    }
    catch( ... ) {
        fprintf( stderr, "Errro: unknown.\n");
    }
    return 0;
}
