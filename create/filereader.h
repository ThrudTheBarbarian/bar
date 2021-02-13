#ifndef FILEREADER_H
#define FILEREADER_H

#include <QObject>
#include <QRunnable>

#include "properties.h"

class Compressor;

class FileReader : public QRunnable
	{
	/**************************************************************************\
	|* Properties
	\**************************************************************************/
	GETSET(bool, shouldCompress, ShouldCompress);

	private:
		Compressor *		_parent;			// Link back to parent

		/**********************************************************************\
		|* Handle everything to do with a single FilesystemItem
		\**********************************************************************/
		void _handleItem(FilesystemItem* item);

	public:
		/**********************************************************************\
		|* Constructor
		\**********************************************************************/
		FileReader(Compressor *parent);

		/**********************************************************************\
		|* Run the task
		\**********************************************************************/
		void run() override;

	signals:

	};

#endif // FILEREADER_H
