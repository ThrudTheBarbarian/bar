#ifndef CREATOR_H
#define CREATOR_H

#include <QObject>
#include <QThread>

#include "properties.h"
#include "filesystemitem.h"
#include "macros.h"

class Compressor;
class DirScanner;

class Creator: public QObject
	{
	Q_OBJECT

	/**************************************************************************\
	|* Properties
	\**************************************************************************/
	GETSET(QString, file, File);			// File or device to write to
	GETSET(bool, verbose, Verbose);			// Whether to be chatty about it
	GETSET(QStringList, paths, Paths);		// Which file-trees to add
	GET(FsItemList, items);					// Files we found

	private:
		/**********************************************************************\
		|* Instance variables
		\**********************************************************************/
		DirScanner *_scanner;				// Directory scanner
		Compressor *_compressor;			// Data compressor

	public:
		/**********************************************************************\
		|* Constructors/Destructors
		\**********************************************************************/
		explicit Creator(QObject *parent = nullptr);
		~Creator(void);

		/**********************************************************************\
		|* Create action implementation
		\**********************************************************************/
		bool create(bool compress);

		/**********************************************************************\
		|* We're all done scanning directories
		\**********************************************************************/
		void scanComplete(void);

	signals:
		/**********************************************************************\
		|* We're all done scanning directories
		\**********************************************************************/
		void startScan(QStringList paths);

	};

#endif // CREATOR_H
