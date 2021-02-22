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
	_type			= (fi.isDir())			? TYPE_DIR
					: (fi.isSymLink())		? TYPE_LINK
					: (fi.isFile())			? TYPE_FILE
											: TYPE_INVALID;

	_link			= (_type == TYPE_LINK)	? fi.symLinkTarget()
											: "";

	_verbose		= ArgsParser::sharedInstance()->flag("-v");
	_workFactor		= ArgsParser::sharedInstance()->value("-wf").toInt();

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
bool FilesystemItem::load(DataBuffer *buffer)
	{
	if (_size == 0)
		{
		buffer->state	= IO_DONE;
		return true;
		}

	bool ok = _prepareBuffer(buffer);

	/**************************************************************************\
	|* Now read the appropriate number of bytes into the buffer
	\**************************************************************************/
	if (ok)
		{
		size_t bytesToRead	= _size - buffer->consumed;
		bytesToRead			= (bytesToRead > buffer->bufferSize)
							? buffer->bufferSize
							: bytesToRead;

		if (fread(buffer->out, bytesToRead, 1, buffer->fp) != 1)
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
bool FilesystemItem::compress(DataBuffer *buffer)
	{
	buffer->flags |= FLAG_COMPRESS;
	if (_size == 0)
		{
		buffer->state	= IO_DONE;
		return true;
		}

	bool ok = _prepareBuffer(buffer);

	/**************************************************************************\
	|* Start to handle the compression algorithm
	\**************************************************************************/
	if (ok)
		{
		/**********************************************************************\
		|* Initialise BZ2 if necessary
		\**********************************************************************/
		if (buffer->consumed == 0)
			{
			int status = BZ2_bzCompressInit(&(buffer->bzip),
											9,
											_verbose ? 1 : 0,
											_workFactor);
			if (status == BZ_PARAM_ERROR)
				{
				ok = false;
				_lastError = QString("Parameter error");
				}
			else if (status == BZ_MEM_ERROR)
				{
				ok = false;
				_lastError = QString("Ran out of memory");
				}
			}
		else if (buffer->consumed == _size)
			{
			int status = BZ2_bzCompress(&(buffer->bzip), BZ_FINISH);
			if (status == BZ_STREAM_END)
				buffer->state = IO_DONE;
			}
		else
			{
			/******************************************************************\
			|* Compress another packet
			\******************************************************************/
			size_t max	= buffer->bufferSize;
			size_t left = _size - buffer->consumed;
			size_t size = (left > max) ? max : left;

			if (fread(buffer->in, size, 1, buffer->fp) != 1)
				{
				ok = false;
				_lastError = QString("Couldn't read %1 bytes from %2").
								arg(size).arg(_name);
				}
			else
				{
				buffer->bzip.avail_in = size;
				buffer->bzip.next_in  = buffer->in;
				buffer->bzip.next_out = buffer->out;
				buffer->bzip.avail_out = buffer->bufferSize;
				buffer->consumed += size;

				int status = BZ2_bzCompress(&(buffer->bzip), BZ_RUN);
				if (status != BZ_RUN_OK)
					{
					ok = false;
					_lastError = QString("Error: Got %1 from bzCompress(%2)").
									arg(status).arg(_name);
					}
				}
			}
		}

	return ok;
	}

#pragma mark - Private methods

/******************************************************************************\
|* If we're passed a virgin DataBuffer, then initialise it with our params.
|* Othewise assume we're partway through the process
\******************************************************************************/
bool FilesystemItem::_prepareBuffer(DataBuffer *buffer)
	{
	bool ok = true;

	if (buffer->fp == nullptr)
		{
		buffer->state		= IO_READING;
		buffer->bufferSize	= (_size >= _blockSize) ? _blockSize : _size;

		/**********************************************************************\
		|* Allocate the input buffer if we're compressing data
		\**********************************************************************/
		if ((buffer->flags & FLAG_COMPRESS) == FLAG_COMPRESS)
			{
			buffer->in			= new char[buffer->bufferSize];
			if (buffer->in == nullptr)
				{
				_lastError	= QString("Cannot alloc %1 bytes for '%2'")
								.arg(buffer->bufferSize)
								.arg(_name);
				ok			= false;
				}
			}

		if (ok)
			{
			/******************************************************************\
			|* Allocate the output buffer
			\******************************************************************/
			buffer->out			= new char[buffer->bufferSize];
			if (buffer->out == nullptr)
				{
				_lastError	= QString("Cannot alloc %1 bytes for '%2'")
								.arg(buffer->bufferSize)
								.arg(_name);
				ok			= false;
				}
			else
				{
				/**************************************************************\
				|* Open the file
				\**************************************************************/
				buffer->fp			= fopen(qPrintable(_name), "rb");
				if (buffer->fp == nullptr)
					{
					_lastError = QString("Cannot open '%1' for read").arg(_name);
					ok = false;
					}
				}

			}
		}
	return ok;
	}
