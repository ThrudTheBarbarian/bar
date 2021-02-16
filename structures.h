//
//  structures.h
//  dbio
//
//  Created by Simon Gornall on 7/18/20.
//  Copyright © 2020 Simon Gornall. All rights reserved.
//

#ifndef structures_h
#define structures_h

#include <stdio.h>
#include <stdint.h>

#include <map>
#include <queue>
#include <string>
#include <vector>

/*****************************************************************************\
|* Basically a clone of NSRange
\*****************************************************************************/
typedef struct Range
	{
	uint64_t    location;		// first index
	int64_t     length;			// length of range
 
    Range(uint64_t offset, int64_t len)
        {
        location = offset;
        length   = len;
        }
    
    Range()
        {
        location    = 0;
        length      = 0;
        }
        
	} Range;

#define NotFound        ((uint64_t)-1)

/*****************************************************************************\
|* Used for compression in the filesystemItem class and the compressor class
\*****************************************************************************/
struct DataBuffer
	{
	FILE *			fp;				// Pointer to the FILE structor
	uint8_t *		data;			// Memory buffer, capped in size
	size_t			bufferSize;		// Number of bytes in the buffer
	size_t			dataSize;		// Number of used bytes in the buffer
	int				state;			// Internal state
	int				block;			// Which block we're on
	int				start;			// Where to start reading from

	DataBuffer() : fp(nullptr),
				   data(nullptr),
				   bufferSize(0),
				   dataSize(0),
				   state(0),
				   block(0),
				   start(0)
		{}
	};


/*****************************************************************************\
|* Used to iterate over a map using the for: approach, as in:
|*
|*  for (Elements<int, std::string> kv : my_map )
|*		{
|*		std::cout << kv.key << " --> " << kv.value;
|*		}
\*****************************************************************************/
template <class K, class T>
struct Elements {
    K const& key;
    T& value;
 
    Elements(std::pair<K const, T>& pair)
        : key(pair.first)
        , value(pair.second)
    { }
};

/*****************************************************************************\
|* Useful typedefs
\*****************************************************************************/
class FilesystemItem;

typedef std::queue<FilesystemItem> FilesystemItemQueue;
typedef std::vector<std::string> StringVector;

#endif /* structures_h */
