#ifndef CREATOR_H
#define CREATOR_H

#include <QObject>
#include <QThread>

#include "properties.h"
#include "filesystemitem.h"
#include "macros.h"

class Compressor;
class DirScanner;
class FileReader;
class Writer;

class Creator: public QObject
	{
	Q_OBJECT

	/**************************************************************************\
	|* Properties
	\**************************************************************************/
	GETSET(QString, file, File);				// File or device to write to
	GETSET(bool, verbose, Verbose);				// Whether to be chatty
	GETSET(QStringList, paths, Paths);			// Which file-trees to add
	GET(FsItemList, items);						// Files we found
	GETSET(bool, scanComplete, ScanComplete);	// The files scan is complete
	GETSET(bool, compress, Compress);			// Do we compress
	GETSET(int, threads, Threads);				// How many threads to use
	GET(Writer *, writer);						// The writer object

	private:
		/**********************************************************************\
		|* Instance variables
		\**********************************************************************/
		DirScanner			*_scanner;			// Directory scanner
		Compressor			*_compressor;		// Data compressor
		QMutex				_mutex;				// Prevent races
		FilesystemItem		_noItem;			// Used as an invalid flag
		QStringList			_errors;			// List of errors found
		QSet<FileReader *>	_active;			// List of active readers

	public:
		/**********************************************************************\
		|* Constructors/Destructors
		\**********************************************************************/
		explicit Creator(QObject *parent = nullptr);
		~Creator(void);

		/**********************************************************************\
		|* Create action implementation
		\**********************************************************************/
		bool create(void);

		/**********************************************************************\
		|* Handle the items list changes
		\**********************************************************************/
		void appendItem(FilesystemItem *item);
		FilesystemItem * nextItem(void);

		/**********************************************************************\
		|* Append an error message
		\**********************************************************************/
		void addError(QString msg);

		/**********************************************************************\
		|* Let the reader tell us it is finished
		\**********************************************************************/
		void readerFinished(FileReader *reader);

		/**********************************************************************\
		|* Add the file-reader to the set of active file-readers
		\**********************************************************************/
		void addReader(FileReader *reader);

		/**********************************************************************\
		|* Return whether all reads are complete
		\**********************************************************************/
		bool itemsToDo(void);

		/**********************************************************************\
		|* Return whether we've finished scanning and reading all the files
		\**********************************************************************/
		bool creationComplete(void);

	signals:
		/**********************************************************************\
		|* We're all done scanning directories
		\**********************************************************************/
		void startScan(QStringList paths);

	};

#endif // CREATOR_H
