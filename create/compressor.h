#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include <QObject>
#include <QThreadPool>

#include "properties.h"

class Creator;

class Compressor : public QObject
	{
	Q_OBJECT

	/**************************************************************************\
	|* Properties
	\**************************************************************************/
	GET(QThreadPool, pool);					// The compression threadpool
	GETSET(Creator *, creator, Creator);	// The 'create' controller

	public:
		/**********************************************************************\
		|* Constructors/Destructors
		\**********************************************************************/
		explicit Compressor(QObject *parent = nullptr);
		~Compressor(void);

		/**********************************************************************\
		|* Start compressing as files become available
		\**********************************************************************/
		bool compress(bool useCompression, int threadCount);

	signals:

	};

#endif // COMPRESSOR_H
