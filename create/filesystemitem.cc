#include "argsparser.h"
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
	{
	_blockSize = ArgsParser::sharedInstance()->value("-bs").toInt();
	}


/******************************************************************************\
|* Destructor
\******************************************************************************/
FilesystemItem::~FilesystemItem(void)
	{
	}

/******************************************************************************\
|* Load the file into RAM
\******************************************************************************/
int FilesystemItem::load(DataBuffer *buffer)
	{
	bool ok = false;

	/**************************************************************************\
	|* If we're passed a virgin DataBuffer, then initialise it with our params.
	|* Othewise assume we're partway through the process
	\**************************************************************************/
	if (buffer->fp == nullptr)
		{
		buffer->bufferSize	= (_size >= _blockSize) ? _blockSize : _size;
		buffer->data		= new uint8_t [buffer->bufferSize];
		}

	FILE *fp = fopen(qPrintable(_name), "rb");
	if (fp)
		{
		_fileData = new uint8_t [_size];
		if (_fileData != nullptr)
			{
			if (fread(_fileData, _size, 1, fp) == 1)
				ok = true;
			else
				_lastError = QString("Cannot read %1 bytes from %2")
								.arg(_size).arg(_name);
			}
		else
			_lastError = QString("Cannot alloc %1 bytes for %2")
						.arg(_size).arg(_name);

		fclose(fp);
		}
	else
		_lastError = QString("Cannot open '%1' for read").arg(_name);

	return ok;
	}

/******************************************************************************\
|* Compress a loaded file
\******************************************************************************/
int FilesystemItem::compress(DataBuffer *buffer)
	{
	bool ok = false;


	return ok;
	}
