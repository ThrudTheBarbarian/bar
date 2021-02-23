#include "writerfactory.h"
#include "writer.h"

#include "writertofile.h"

/******************************************************************************\
|* Return the correct writer subclass for a given output filename
|*
|* Author	: SjG
\******************************************************************************/
Writer * WriterFactory::writerForDevice(QString dev)
	{
	return new writerToFile(dev);
	}
