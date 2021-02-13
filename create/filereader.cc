#include "compressor.h"
#include "dirscanner.h"
#include "filereader.h"
#include "filesystemitem.h"

/******************************************************************************\
|* Constructor
\******************************************************************************/
FileReader::FileReader(Compressor *parent)
		   :_parent(parent)
	{}


/******************************************************************************\
|* Run method
\******************************************************************************/
void FileReader::run(void)
	{
	bool allDone	= false;
	DirScanner *ds	= _parent->scanner();

	while (!allDone)
		{
		FilesystemItem *item = ds->nextItem();
		if (item->ok())
			_handleItem(item);
		else
			usleep(250000);
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
	FILE *fp = fopen(qPrintable(item->name()), "rb");
	if (fp)
		{
		// FIXME : read file into allocated RAM
		fclose(fp);
		}
	}
