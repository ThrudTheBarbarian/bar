#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include <QObject>
#include <QThreadPool>

#include "properties.h"

class DirScanner;

class Compressor : public QObject
	{
	Q_OBJECT

	/**************************************************************************\
	|* Properties
	\**************************************************************************/
	GET(QThreadPool, pool);					// The compression threadpool
	GETSET(DirScanner *, scanner, Scanner);	// The directory scanner

	public:
		/**********************************************************************\
		|* Constructors/Destructors
		\**********************************************************************/
		explicit Compressor(QObject *parent = nullptr);

		/**********************************************************************\
		|* Start compressing as files become available
		\**********************************************************************/
		bool compress(bool useCompression);

	signals:

	};

#endif // COMPRESSOR_H
