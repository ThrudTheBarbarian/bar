#ifndef WRITERFACTORY_H
#define WRITERFACTORY_H

#include <QString>

class Writer;

class WriterFactory
	{

	public:

		static Writer * writerForDevice(QString device);
	};

#endif // WRITERFACTORY_H
