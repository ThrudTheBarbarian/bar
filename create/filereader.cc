#include "compressor.h"
#include "creator.h"
#include "dirscanner.h"
#include "filereader.h"
#include "filesystemitem.h"
#include "writer.h"

/******************************************************************************\
|* Constructor
\******************************************************************************/
FileReader::FileReader(Creator *creator)
		   :_creator(creator)
	{
	// Let the creator know there's a reader to wait for
	_creator->addReader(this);
	}


/******************************************************************************\
|* Run method
\******************************************************************************/
void FileReader::run(void)
	{
	bool allDone	= false;
	bool scanDone	= false;

	while (!allDone)
		{
		FilesystemItem *item = _creator->nextItem();
		if (item->ok())
			_handleItem(item);
		else
			{
			bool readsDone	= _creator->itemsToDo();
			scanDone		= scanDone ? scanDone
							: _creator->scanComplete();

			allDone			= readsDone && scanDone;
			if (!allDone)
				usleep(250000);
			}
		}

	// Let the creator know that we've finished reading
	_creator->readerFinished(this);
	}

/******************************************************************************\
|* Handle everything to do with a single FilesystemItem
\******************************************************************************/
void FileReader::_handleItem(FilesystemItem* item)
	{
	fprintf(stderr, "Doing %s\n", item->name().toStdString().c_str());

	if (item->type() == FilesystemItem::TYPE_FILE)
		{
		/**********************************************************************\
		|* Read in the file
		\**********************************************************************/
		DataBuffer *data = item->data();
		bool readOk= true;

		while (readOk && (data->state != FilesystemItem::IO_DONE))
			{
			data->block ++;
			readOk = (_shouldCompress)
					? item->compress()
					: item->load();
			}

		/**********************************************************************\
		|* Tell the world how far we've come
		\**********************************************************************/
		//if (readOk)
			{
			QString msg = QString("Read %1 bytes from %2 [%3]\n")
							.arg(data->consumed)
							.arg(item->name())
							.arg(readOk);
			fprintf(stderr, "%s\n", qPrintable(msg));
			}

		/**********************************************************************\
		|* Tell the writer it has a new item
		\**********************************************************************/
		_creator->writer()->enqueueItem(item);
		}
	}
