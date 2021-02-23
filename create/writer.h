#ifndef WRITER_H
#define WRITER_H

#include <QVector>
#include <QString>
#include <QThread>

#include "properties.h"

class Creator;
class FilesystemItem;

class Writer : public QThread
	{
	/**************************************************************************\
	|* Properties
	\**************************************************************************/
	GETSET(Creator *, creator, Creator);		// Parent object
	GETSET(QString, file, File);				// File or device to write to
	GETSET(QString, lastError, LastError);		// Last error string

	private:
		/**********************************************************************\
		|* Instance variables
		\**********************************************************************/
		QVector<FilesystemItem *>	_queue;		// What to write
		QMutex						_mutex;		// Prevent races

		/**********************************************************************\
		|* Actually write an item to the queue
		\**********************************************************************/
		void				_unsafeEnqueueItem(FilesystemItem *item);

	protected:
		/**********************************************************************\
		|* Get the next item to write, from the queue
		\**********************************************************************/
		FilesystemItem * nextItem(void);

		/**********************************************************************\
		|* Helper method - return the user's name
		\**********************************************************************/
		QString				_username(void);

		/**********************************************************************\
		|* Helper method - Return the header
		\**********************************************************************/
		QString				_header(void);

	public:
		Writer(QString file);
		virtual ~Writer(void);

		/**********************************************************************\
		|* Allow other objects to enqueue items to write
		\**********************************************************************/
		void enqueueItem(FilesystemItem *item);

		/**********************************************************************\
		|* Write out what we have left to do, or pause waiting for more until
		|* we're done
		\**********************************************************************/
		void run(void);

		/**********************************************************************\
		|* Concrete classes need to implement these
		\**********************************************************************/
		virtual bool open(void) =0;
		virtual bool persist(FilesystemItem *item) =0;
		virtual void close(void) =0;
	};

#endif // WRITER_H
