
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
		_parent->appendItem(fsi);

		if (fsi->type() == FilesystemItem::TYPE_DIR)
			{
			QDirIterator di(path, FILTERS, QDirIterator::Subdirectories);
			while (di.hasNext())
				{
				di.next();
				fsi = new FilesystemItem(di.fileInfo());
				_parent->appendItem(fsi);

				while (_parent->items().size() > 500)
					{
					fprintf(stderr, "Sleeping scanner\n");
					sleep(1);
					}
				}
			}
		}
	_parent->setScanComplete(true);
	}

