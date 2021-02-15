//
//  structures.h
//  dbio
//
//  Created by Simon Gornall on 7/18/20.
//  Copyright © 2020 Simon Gornall. All rights reserved.
//

#ifndef structures_h
#define structures_h

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
typedef struct
	{
	uint8_t *		data;
	int				length;
	int				state;
	int				pass;
	int				start;
	} DataBuffer;


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
