#include "filesystemitem.h"
#include "macros.h"

/******************************************************************************\
|* Represent anything we find on disk
|*
|* Author	: SjG
\******************************************************************************/
FilesystemItem::FilesystemItem(QFileInfo fi)
			   :_name(fi.filePath())
			   ,_size(fi.size())
			   ,_modified(fi.lastModified().currentSecsSinceEpoch())
			   ,_uid(fi.ownerId())
			   ,_gid(fi.groupId())
			   ,_mode(fi.permissions())
			   ,_fileData(nullptr)
			   ,_zlibData(nullptr)
	{
	_type = (fi.isDir())			? TYPE_DIR
		  : (fi.isSymLink())		? TYPE_LINK
		  : (fi.isFile())			? TYPE_FILE
									: TYPE_INVALID;

	_link = (_type == TYPE_LINK)	? fi.symLinkTarget()
									: "";
	_ok	  = true;
	}

/******************************************************************************\
|* Empty constructor, specifically invalid
\******************************************************************************/
FilesystemItem::FilesystemItem(void)
			   :_ok(false)
			   ,_fileData(nullptr)
			   ,_zlibData(nullptr)
	{}


/******************************************************************************\
|* Destructor
\******************************************************************************/
FilesystemItem::~FilesystemItem(void)
	{
	DELETE(_fileData);
	DELETE(_zlibData);
	}
