#include "argsparser.h"
#include "filesystemitem.h"
#include "macros.h"

#include "util/scalednumber.h"

#define BUFFER_SLOP		(128000)

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
			   ,_data(nullptr)
	{
	_type			= (fi.isDir())			? TYPE_DIR
					: (fi.isSymLink())		? TYPE_LINK
					: (fi.isFile())			? TYPE_FILE
											: TYPE_INVALID;

	_link			= (_type == TYPE_LINK)	? fi.symLinkTarget()
											: "";

	_verbose		= ArgsParser::sharedInstance()->flag("-v");
	_workFactor		= ArgsParser::sharedInstance()->value("-wf").toInt();

	QString bs		= ArgsParser::sharedInstance()->value("-bs");
	_blockSize		= ScaledNumber::toNumber(bs);

	_data			= new DataBuffer;
	_ok				= true;
	}

/******************************************************************************\
|* Empty constructor, specifically invalid
\******************************************************************************/
FilesystemItem::FilesystemItem(void)
			   :_ok(false)
			   ,_data(nullptr)

	{
	QString bs		= ArgsParser::sharedInstance()->value("-bs");
	_blockSize		= ScaledNumber::toNumber(bs);
	_data			= new DataBuffer;
	}


/******************************************************************************\
|* Destructor
\******************************************************************************/
FilesystemItem::~FilesystemItem(void)
	{
	DELETE(_data);
	}

/******************************************************************************\
|* Load the file into RAM
\******************************************************************************/
bool FilesystemItem::load(void)
	{
	if (_size == 0)
		{
		_data->state	= IO_DONE;
		return true;
		}

	bool ok = _prepareBuffer();

	/**************************************************************************\
	|* Now read the appropriate number of bytes into the buffer
	\**************************************************************************/
	if (ok)
		{
		size_t bytesToRead	= _size - _data->consumed;
		bytesToRead			= (bytesToRead > _data->bufferSize)
							? _data->bufferSize
							: bytesToRead;

		if (fread(_data->out, bytesToRead, 1, _data->fp) != 1)
			{
			_ok			= false;
			_lastError	= QString("Cannot read %1 bytes from '%2'")
							.arg(bytesToRead)
							.arg(_name);
			}
		else
			{
			_data->consumed += bytesToRead;
			_data->dataSize  = bytesToRead;
			}

		if (_data->consumed == _size)
			{
			fclose(_data->fp);
			_data->fp		= nullptr;
			_data->state	= IO_DONE;
			}
		}
	return ok;
	}

/******************************************************************************\
|* Compress a loaded file
\******************************************************************************/
bool FilesystemItem::compress(void)
	{
	_data->flags |= FLAG_COMPRESS;
	if (_size == 0)
		{
		_data->state	= IO_DONE;
		return true;
		}

	bool ok = _prepareBuffer();

	/**************************************************************************\
	|* Start to handle the compression algorithm
	\**************************************************************************/
	if (ok)
		{
		/**********************************************************************\
		|* Initialise BZ2 if necessary
		\**********************************************************************/
		if (_data->consumed == 0)
			{
			int status = BZ2_bzCompressInit(&(_data->bzip),
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

		if (_data->consumed == _size)
			{
			int status = BZ2_bzCompress(&(_data->bzip), BZ_FINISH);
			_data->dataSize = _data->bzip.avail_out;

			if (status == BZ_STREAM_END)
				_data->state = IO_DONE;
			}
		else
			{
			/******************************************************************\
			|* Compress another packet
			\******************************************************************/
			size_t max	= _data->bufferSize;
			size_t left = _size - _data->consumed;
			size_t size = (left > max) ? max : left;

			if (fread(_data->in, size, 1, _data->fp) != 1)
				{
				ok = false;
				_lastError = QString("Couldn't read %1 bytes from %2").
								arg(size).arg(_name);
				}
			else
				{
				_data->bzip.avail_in = size;
				_data->bzip.next_in  = _data->in;
				_data->bzip.next_out = _data->out;
				_data->bzip.avail_out = _data->bufferSize;
				_data->consumed += size;

				int status = BZ2_bzCompress(&(_data->bzip), BZ_RUN);
				if (status != BZ_RUN_OK)
					{
					ok = false;
					_lastError = QString("Error: Got %1 from bzCompress(%2)").
									arg(status).arg(_name);
					}
				_data->dataSize = _data->bzip.avail_out;
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
bool FilesystemItem::_prepareBuffer(void)
	{
	bool ok = true;

	if (_data->fp == nullptr)
		{
		_data->state		= IO_READING;
		_data->bufferSize	= (_size >= _blockSize) ? _blockSize : _size;
		if (_data->bufferSize < BUFFER_SLOP)
			_data->bufferSize = BUFFER_SLOP;

		/**********************************************************************\
		|* Allocate the input buffer if we're compressing data
		\**********************************************************************/
		if ((_data->flags & FLAG_COMPRESS) == FLAG_COMPRESS)
			{
			_data->in			= new char[_data->bufferSize];
			if (_data->in == nullptr)
				{
				_lastError	= QString("Cannot alloc %1 bytes for '%2'")
								.arg(_data->bufferSize)
								.arg(_name);
				ok			= false;
				}
			}

		if (ok)
			{
			/******************************************************************\
			|* Allocate the output buffer
			\******************************************************************/
			_data->out			= new char[_data->bufferSize];
			if (_data->out == nullptr)
				{
				_lastError	= QString("Cannot alloc %1 bytes for '%2'")
								.arg(_data->bufferSize)
								.arg(_name);
				ok			= false;
				}
			else
				{
				/**************************************************************\
				|* Open the file
				\**************************************************************/
				_data->fp			= fopen(qPrintable(_name), "rb");
				if (_data->fp == nullptr)
					{
					_lastError = QString("Cannot open '%1' for read").arg(_name);
					ok = false;
					}
				}

			}
		}
	return ok;
	}
