#ifndef FILEREADER_H
#define FILEREADER_H

#include <QObject>
#include <QRunnable>

#include "properties.h"

class Creator;

class FileReader : public QRunnable
	{
	/**************************************************************************\
	|* Properties
	\**************************************************************************/
	GETSET(bool, shouldCompress, ShouldCompress);

	private:
		Creator *			_creator;			// Link back to creator

		/**********************************************************************\
		|* Handle everything to do with a single FilesystemItem
		\**********************************************************************/
		void _handleItem(FilesystemItem* item);

	public:
		/**********************************************************************\
		|* Constructor
		\**********************************************************************/
		FileReader(Creator *parent);

		/**********************************************************************\
		|* Run the task
		\**********************************************************************/
		void run() override;

	signals:

	};

#endif // FILEREADER_H
