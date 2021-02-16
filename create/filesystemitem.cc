#include "argsparser.h"
#include "filesystemitem.h"
#include "macros.h"

#include "util/scalednumber.h"

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
			   ,_lastError("")
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
	QString bs	= ArgsParser::sharedInstance()->value("-bs");
	_blockSize	= ScaledNumber::toNumber(bs);
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
	bool ok = true;

	/**************************************************************************\
	|* If we're passed a virgin DataBuffer, then initialise it with our params.
	|* Othewise assume we're partway through the process
	\**************************************************************************/
	if (buffer->fp == nullptr)
		{
		buffer->state		= IO_READING;
		buffer->bufferSize	= (_size >= _blockSize) ? _blockSize : _size;
		buffer->data		= new uint8_t [buffer->bufferSize];
		if (buffer->data == nullptr)
			{
			_lastError	= QString("Cannot alloc %1 bytes for '%2'")
							.arg(buffer->bufferSize)
							.arg(_name);
			ok			= false;
			}
		else
			{
			buffer->fp			= fopen(qPrintable(_name), "rb");
			if (buffer->fp == nullptr)
				{
				_lastError = QString("Cannot open '%1' for read").arg(_name);
				ok = false;
				}
			}
		}

	/**************************************************************************\
	|* Now read the appropriate number of bytes into the buffer
	\**************************************************************************/
	if (ok)
		{
		size_t bytesToRead	= _size - buffer->consumed;
		bytesToRead			= (bytesToRead > buffer->bufferSize)
							? buffer->bufferSize
							: bytesToRead;

		if (fread(buffer->data, bytesToRead, 1, buffer->fp) != 1)
			{
			_ok			= false;
			_lastError	= QString("Cannot read %1 bytes from '%2'")
							.arg(bytesToRead)
							.arg(_name);
			}
		else
			buffer->consumed += bytesToRead;

		if (buffer->consumed == _size)
			{
			fclose(buffer->fp);
			buffer->fp		= nullptr;
			buffer->state	= IO_DONE;
			}
		}
	return ok;
	}

/******************************************************************************\
|* Compress a loaded file
\******************************************************************************/
int FilesystemItem::compress(DataBuffer *buffer)
	{
	bool ok = false;

	if (buffer->fp == nullptr)
		{}

	return ok;
	}
