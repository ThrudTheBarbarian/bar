#ifndef FILESYSTEMITEM_H
#define FILESYSTEMITEM_H

#include <QFileInfo>

#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

#include "properties.h"

typedef QVector<FilesystemItem *> FsItemList;

class FilesystemItem
	{
	public:
		/**********************************************************************\
		|* Typedefs and enums
		\**********************************************************************/
		enum
			{
			TYPE_DIR,
			TYPE_FILE,
			TYPE_LINK,
			TYPE_ZFILE,
			TYPE_INVALID
			};

	/**************************************************************************\
	|* Properties
	\**************************************************************************/
	GETSET(bool, ok, Ok);				// Did we initialise ok ?

	GETSET(QString, name, Name);		// File's path relative to archive root
	GETSET(size_t, size, Size);			// File's size in bytes
	GETSET(int, type, Type);			// From enumeration above
	GETSET(time_t, modified, Modified);	// Last modification time (since epoch)
	GETSET(uid_t, uid, Uid);			// File's user id
	GETSET(gid_t, gid, Gid);			// File's group id
	GETSET(mode_t, mode, Mode);			// Permissions state
	GETSET(QString, link, Link);		// What the link points to

	GET(size_t, blockSize);				// Size of a compressed block

	public:
		/**********************************************************************\
		|* Constructors/Destructors
		\**********************************************************************/
		explicit FilesystemItem(void);
		explicit FilesystemItem(QFileInfo fi);
		~FilesystemItem(void);

		/**********************************************************************\
		|* Load the file into the structure
		\**********************************************************************/
		int load(DataBuffer *buffer);

		/**********************************************************************\
		|* Load the file into the structure
		\**********************************************************************/
		int compress(DataBuffer *buffer);

	signals:

	};

#endif // FILESYSTEMITEM_H
