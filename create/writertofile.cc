
#include "filesystemitem.h"
#include "writertofile.h"

/******************************************************************************\
|* Implement a write-stream-to-a-single-file
|*
|* Author	: SjG
\******************************************************************************/
writerToFile::writerToFile(QString file)
			 :Writer(file)
			 ,_fp(nullptr)
	{}


/******************************************************************************\
|* Implement a write-stream-to-a-single-file
|*
|* Author	: SjG
\******************************************************************************/
writerToFile::~writerToFile(void)
	{
	if (_fp != nullptr)
		_close();
	}


/******************************************************************************\
|* Open the file and write the header
\******************************************************************************/
bool writerToFile::open(void)
	{
	_fp = fopen(qPrintable(_file), "wb");
	if (_fp == NULL)
		{
		_lastError = QString("Cannot open %1 for output").arg(_file);
		return false;
		}

	/**************************************************************************\
	|* Write the magic header
	\**************************************************************************/
	QString header	= _header();
	int headerLen	= header.length();
	if (fwrite(qPrintable(header), headerLen, 1, _fp) != 1)
		{
		_close();
		_lastError = QString("Cannot write header to %1").arg(_file);
		return false;
		}

	return true;
	}


/******************************************************************************\
|* Called to write an item to the store
\******************************************************************************/
bool writerToFile::persist(FilesystemItem *item)
	{
	bool ok = true;

	// FIXME - need to check for 0-length files
	if (fwrite(item->data()->out, item->data()->dataSize, 1, _fp) != 1)
		{
		_close();
		_lastError	= QString("Cannot append %1 to %2")
						.arg(item->name(), _file);
		ok			= false;
		}
	return ok;
	}

/******************************************************************************\
|* Close the file
\******************************************************************************/
void writerToFile::close(void)
	{
	_close();
	}

#pragma mark - Private methods

/******************************************************************************\
|* Close down the file. This can't be done in a virtual function, even though|
|* it's trivial, so defer close()'s work to _close()
\******************************************************************************/
void writerToFile::_close(void)
	{
	if (_fp != nullptr)
		{
		fclose(_fp);
		_fp = nullptr;
		}
	}

