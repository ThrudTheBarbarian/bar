#include <QThreadPool>

#include "compressor.h"
#include "creator.h"
#include "filereader.h"

/******************************************************************************\
|* Manage a thread-pool of compression worker-threads
|*
|* Author	: SjG
\******************************************************************************/
Compressor::Compressor(QObject *parent)
		   :QObject(parent)
		   ,_creator(nullptr)
	{
	// Make sure we're cooking with gas
	_pool.setMaxThreadCount(2);//QThread::idealThreadCount());
	}

/******************************************************************************\
|* Delete the instance
\******************************************************************************/
Compressor::~Compressor(void)
	{
	_pool.clear();
	}

/******************************************************************************\
|* Create the thread-pool and start workers going
\******************************************************************************/
bool Compressor::compress(bool useCompression)
	{
	bool ok = false;

	for (int i=0; i<_pool.maxThreadCount(); i++)
		{
		FileReader *reader = new FileReader(_creator);
		reader->setShouldCompress(useCompression);
		_pool.start(reader);
		}
	return ok;
	}
