#ifndef DIRSCANNER_H
#define DIRSCANNER_H

#include <QMutex>
#include <QThread>

#include "properties.h"
#include "filesystemitem.h"

class Creator;

class DirScanner : public QThread
	{
	Q_OBJECT

	/**************************************************************************\
	|* Properties
	\**************************************************************************/
	GET(Creator *, parent);					// Who to talk to
	GETSET(bool, verbose, Verbose);			// Whether to be chatty about it
	GETSET(QStringList, paths, Paths);		// Where to look

	private:

	public:
		/**********************************************************************\
		|* Constructor
		\**********************************************************************/
		DirScanner(Creator *parent);

		/**********************************************************************\
		|* Recursively scan the directories/files in _paths
		\**********************************************************************/
		void run(void);

		/**********************************************************************\
		|* Obtain the next item
		\**********************************************************************/
		FilesystemItem * nextItem(void);
	};

#endif // DIRSCANNER_H
