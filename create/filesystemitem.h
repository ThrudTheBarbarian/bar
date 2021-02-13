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
			TYPE_INVALID
			};

	/**************************************************************************\
	|* Properties
	\**************************************************************************/
	GETSET(bool, ok, Ok);

	GETSET(QString, name, Name);
	GETSET(size_t, size, Size);
	GETSET(int, type, Type);
	GETSET(time_t, modified, Modified);
	GETSET(uid_t, uid, Uid);
	GETSET(gid_t, gid, Gid);
	GETSET(mode_t, mode, Mode);
	GETSET(QString, link, Link);
	GETSET(uint8_t *, fileData, FileData);
	GETSET(uint8_t *, zlibData, ZlibData);

	public:
		/**********************************************************************\
		|* Constructors/Destructors
		\**********************************************************************/
		explicit FilesystemItem(void);
		explicit FilesystemItem(QFileInfo fi);
		~FilesystemItem(void);

	signals:

	};

#endif // FILESYSTEMITEM_H
