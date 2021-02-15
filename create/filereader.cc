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
	{}


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
	fprintf(stderr, "Removed %s\n", item->name().toStdString().c_str());

	/**************************************************************************\
	|* Read in the file
	\**************************************************************************/
	if (!item->load())
		_creator->addError(item->lastError());
	else
		{
		if (_shouldCompress)
			{

			}
		}
	}
