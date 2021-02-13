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
		   ,_scanner(nullptr)
		   ,_compressor(nullptr)
	{}

/******************************************************************************\
|* Destructor
\******************************************************************************/
Creator::~Creator(void)
	{
	DELETE(_scanner);
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
	_compressor->setScanner(_scanner);
	_compressor->compress(compress);

	/**************************************************************************\
	|* Wait for a clean exit
	\**************************************************************************/
	_scanner->wait();

	return ok;
	}


#pragma mark - Public slots

/******************************************************************************\
|* We're all done scanning directories
\******************************************************************************/
void Creator::scanComplete(void)
	{
	fprintf(stderr, "Scan complete\n");
	}

#pragma mark - Private Methods

