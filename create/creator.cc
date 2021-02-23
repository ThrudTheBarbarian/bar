#include "argsparser.h"
#include "compressor.h"
#include "creator.h"
#include "dirscanner.h"
#include "writer.h"
#include "writerfactory.h"

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
	{
	_file			= ArgsParser::sharedInstance()->value("-f");
	_verbose		= ArgsParser::sharedInstance()->flag("-v");
	_paths			= ArgsParser::sharedInstance()->remainingArgs();
	_compress		= ArgsParser::sharedInstance()->flag("-j");
	_threads		= ArgsParser::sharedInstance()->value("-#").toInt();

	if (_threads == 0)
		_threads = 2;
	}

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
bool Creator::create(void)
	{
	bool ok = false;

	/**************************************************************************\
	|* Start the background thread that enumerates the filesystem going
	\**************************************************************************/
	_scanner = new DirScanner(this);
	_scanner->setPaths(_paths);
	_scanner->start();

	/**************************************************************************\
	|* Start the writer going
	\**************************************************************************/
	_writer = WriterFactory::writerForDevice(_file);
	_writer->setCreator(this);
	_writer->start();

	/**************************************************************************\
	|* Start the compressor (which doesn't necessarily compress files) going
	\**************************************************************************/
	_compressor = new Compressor(this);
	_compressor->setCreator(this);
	_compressor->compress(_compress, _threads);

	/**************************************************************************\
	|* Wait for a clean exit
	\**************************************************************************/
	_scanner->wait();
	_writer->wait();

	/**************************************************************************\
	|* Show any errors
	\**************************************************************************/
	foreach (QString msg, _errors)
		fprintf(stderr, "%s\n", qPrintable(msg));

	return ok;
	}


/******************************************************************************\
|* Return whether we've finished scanning and reading all the files
\******************************************************************************/
bool Creator::creationComplete(void)
	{
	return (_scanComplete && (_active.isEmpty()));
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
|* Determine if the reads have all been done
\******************************************************************************/
bool Creator::itemsToDo(void)
	{
	QMutexLocker guard(&_mutex);
	return (_items.size() == 0);
	}

/******************************************************************************\
|* Remove an item from the list
\******************************************************************************/
FilesystemItem * Creator::nextItem(void)
	{
	QMutexLocker guard(&_mutex);
	return (_items.size() > 0) ? _items.takeFirst() : &_noItem;
	}

/******************************************************************************\
|* Add an error
\******************************************************************************/
void Creator::addError(QString msg)
	{
	_errors.append(msg);
	}


/******************************************************************************\
|* Add the file-reader to the list of active file-readers
\******************************************************************************/
void Creator::readerFinished(FileReader *reader)
	{
	QMutexLocker guard(&_mutex);
	if (!_active.remove(reader))
		{
		fprintf(stderr, "Cannot remove reader %p", reader);
		}
	}


/******************************************************************************\
|* Add the file-reader to the list of active file-readers
\******************************************************************************/
void Creator::addReader(FileReader *reader)
	{
	QMutexLocker guard(&_mutex);
	_active.insert(reader);
	}

#pragma mark - Private Methods


