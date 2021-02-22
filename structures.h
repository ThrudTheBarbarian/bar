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

#include <bzlib.h>

#include "macros.h"

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
	char *			in;				// Memory buffer, capped in size
	char *			out;			// Memory buffer, capped in size
	size_t			bufferSize;		// Number of bytes in the buffer
	size_t			dataSize;		// Number of used bytes in the buffer
	size_t			consumed;		// Number of bytes read thus far
	int				state;			// Internal state
	int				block;			// Which block we're on
	int				flags;			// Where to start reading from
	bz_stream		bzip;			// bzip2 state

	DataBuffer() : fp(nullptr),
				   in(nullptr),
				   out(nullptr),
				   bufferSize(0),
				   dataSize(0),
				   consumed(0),
				   state(0),
				   block(0),
				   flags(0)
		{}

	~DataBuffer(void)
		{
		DELETE_ARRAY(in);
		DELETE_ARRAY(out);
		if (fp)
			{
			fclose(fp);
			fp = nullptr;
			}
		}
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
