#include "compressor.h"
#include "creator.h"
#include "dirscanner.h"

/******************************************************************************\
|* Handle all the creation logic, basically if a user uses -c, it happens here
|*
|* Author	: SjG
\******************************************************************************/
Creator::Creator(QObject *parent)
		   :QObject(parent)
		   ,_file("")
		   ,_verbose(false)
		   ,_scanComplete(false)
		   ,_scanner(nullptr)
		   ,_compressor(nullptr)
	{}

/******************************************************************************\
|* Destructor
\******************************************************************************/
Creator::~Creator(void)
	{
	DELETE(_scanner);
	DELETE(_compressor);
	}

/******************************************************************************\
|* Handle everything for a 'create' action
\******************************************************************************/
bool Creator::create(bool compress)
	{
	bool ok = false;

	/**************************************************************************\
	|* Start the background thread that enumerates the filesystem going
	\**************************************************************************/
	_scanner = new DirScanner(this);
	_scanner->setPaths(_paths);
	_scanner->start();

	/**************************************************************************\
	|* Start the compressor (which doesn't necessarily compress files) going
	\**************************************************************************/
	_compressor = new Compressor(this);
	_compressor->setCreator(this);
	_compressor->compress(compress);

	/**************************************************************************\
	|* Wait for a clean exit
	\**************************************************************************/
	_scanner->wait();

	return ok;
	}


/******************************************************************************\
|* Add an item into the list
\******************************************************************************/
void Creator::appendItem(FilesystemItem *fsi)
	{
	QMutexLocker guard(&_mutex);
	_items.append(fsi);
	fprintf(stderr, "[%s](%d)\n", qPrintable(fsi->name()), (int)_items.size());
	}

/******************************************************************************\
|* Remove an item from the list
\******************************************************************************/
FilesystemItem * Creator::nextItem(void)
	{
	QMutexLocker guard(&_mutex);
	return (_items.size() > 0) ? _items.takeFirst() : &_noItem;
	}

#pragma mark - Private Methods

