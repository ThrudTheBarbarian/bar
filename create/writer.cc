#include <unistd.h>
#include <QDate>

#include "creator.h"
#include "writer.h"

#define QUEUE_MAX			(100)
#define BAR_VERSION			"0.1"

/******************************************************************************\
|* Base class for the various writers. Offers book-keeping facilities
|*
|* Author	: SjG
\******************************************************************************/
Writer::Writer(QString file)
	   :_file(file)
	   ,_lastError("")
	{}

/******************************************************************************\
|* Destructor - nothing to do as yet
\******************************************************************************/
Writer::~Writer(void)
	{}

#pragma mark - public methods

/******************************************************************************\
|* Add an item into the writer queue, public interface which can block until
|* there is sufficient reason to add the item
\******************************************************************************/
void Writer::enqueueItem(FilesystemItem *item)
	{
	/**************************************************************************\
	|* While we have items in the queue sufficient to keep us busy, pause the
	|* submission process
	\**************************************************************************/
	while (_queue.length() >= QUEUE_MAX)
		::sleep(1);

	_unsafeEnqueueItem(item);
	}


/******************************************************************************\
|* Write...
\******************************************************************************/
void Writer::run(void)
	{
	open();

	bool stillRunnning = true;
	while (stillRunnning)
		{
		if (_queue.length() == 0)
			{
			if (_creator->creationComplete())
				stillRunnning = false;
			else
				{
				sleep(1);
				fprintf(stderr, "Sleeping writer\n");
				}
			}
		else
			{
			FilesystemItem *item = nextItem();
			if (!persist(item))
				_creator->addError(item->lastError());
			DELETE(item);
			}
		}

	close();
	}

#pragma mark - private methods

/******************************************************************************\
|* Add an item into the writer queue, private interface which manages the
|* controlled mutex-based access to the queue
\******************************************************************************/
void Writer::_unsafeEnqueueItem(FilesystemItem *item)
	{
	QMutexLocker guard(&_mutex);
	_queue.push_back(item);
	}


#pragma mark - protected methods

/******************************************************************************\
|* Get the next item from the queue, using the lock semantics
\******************************************************************************/
FilesystemItem * Writer::nextItem(void)
	{
	QMutexLocker guard(&_mutex);
	return _queue.takeFirst();
	}

/******************************************************************************\
|* Return the current username
\******************************************************************************/
QString Writer::_username(void)
	{
	QString user = qgetenv("USER");
	if (user.isEmpty())
		user = qgetenv("USERNAME");
	return user;
	}

/******************************************************************************\
|* Return the header for the archive
\******************************************************************************/
QString Writer::_header(void)
	{
	return  QString("BAR\n"
					"Version: " BAR_VERSION "\n"
					"   File: %1\n"
					"   User: %2\n"
					"Created: %3\n")
			.arg(_file,
				 _username(),
				 QDate::currentDate().toString());
	}
