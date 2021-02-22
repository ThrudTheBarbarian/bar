#include "compressor.h"
#include "creator.h"
#include "dirscanner.h"
#include "filereader.h"
#include "filesystemitem.h"

/******************************************************************************\
|* Constructor
\******************************************************************************/
FileReader::FileReader(Creator *creator)
		   :_creator(creator)
	{
	}


/******************************************************************************\
|* Run method
\******************************************************************************/
void FileReader::run(void)
	{
	bool allDone	= false;

	while (!allDone)
		{
		FilesystemItem *item = _creator->nextItem();
		if (item->ok())
			_handleItem(item);
		else
			{
			allDone = _creator->scanComplete();
			if (!allDone)
				usleep(250000);
			}
		}
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
		DataBuffer data;
		bool readOk= true;

		while (readOk && (data.state != FilesystemItem::IO_DONE))
			{
			data.block ++;
			readOk = (_shouldCompress)
					? item->compress(&data)
					: item->load(&data);
			}

		//if (readOk)
			{
			QString msg = QString("Read %1 bytes from %2 [%3]\n")
							.arg(data.consumed)
							.arg(item->name())
							.arg(readOk);
			fprintf(stderr, "%s\n", qPrintable(msg));
			}
		}
	}
