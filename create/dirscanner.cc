
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>

#include "creator.h"
#include "dirscanner.h"

#define FILTERS															\
	QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot | QDir::Readable |	\
	QDir::Writable | QDir::Executable | QDir::Hidden
/******************************************************************************\
|* Scan directories and files, stat them, and let threads have access to them.
|* This is the worker thread controlled by Creator
|*
|* Author	: SjG
\******************************************************************************/


/******************************************************************************\
|* Constructor
\******************************************************************************/
DirScanner::DirScanner(Creator *parent)
		   :_parent(parent)
	{}

/******************************************************************************\
|* Scan...
\******************************************************************************/
void DirScanner::run(void)
	{
	foreach(QString path, _paths)
		{
		QFileInfo fi(path);
		FilesystemItem *fsi = new FilesystemItem(fi);
		_appendItem(fsi);

		if (fsi->type() == FilesystemItem::TYPE_DIR)
			{
			QDirIterator di(path, FILTERS, QDirIterator::Subdirectories);
			while (di.hasNext())
				{
				di.next();
				fsi = new FilesystemItem(di.fileInfo());
				_appendItem(fsi);

				while (_items.size() > 500)
					{
					fprintf(stderr, "Sleeping scanner\n");
					sleep(1);
					}
				}
			}
		}
	_parent->scanComplete();
	}

/******************************************************************************\
|* Add an item into the list
\******************************************************************************/
void DirScanner::_appendItem(FilesystemItem *fsi)
	{
	QMutexLocker guard(&_mutex);
	_items.append(fsi);
	fprintf(stderr, "[%s](%d)\n", qPrintable(fsi->name()), (int)_items.size());
	}

/******************************************************************************\
|* Remove an item from the list
\******************************************************************************/
FilesystemItem * DirScanner::nextItem(void)
	{
	QMutexLocker guard(&_mutex);
	return (_items.size() > 0) ? _items.takeFirst() : &_noItem;
	}
