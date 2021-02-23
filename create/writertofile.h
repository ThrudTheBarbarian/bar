#ifndef WRITERTOFILE_H
#define WRITERTOFILE_H

#include "writer.h"

class writerToFile : public Writer
	{
	private:
		/**********************************************************************\
		|* Instance variables
		\**********************************************************************/
		FILE *			_fp;			// Where we write

		/**********************************************************************\
		|* Actually close the file. Can't be virtual
		\**********************************************************************/
		void			 _close(void);

	public:
		writerToFile(QString file);
		virtual ~writerToFile(void);

		/**********************************************************************\
		|* Called at the start of the writing process
		\**********************************************************************/
		virtual bool open(void);

		/**********************************************************************\
		|* Called to write an item to the store
		\**********************************************************************/
		virtual bool persist(FilesystemItem *item);

		/**********************************************************************\
		|* Called at the end of the writing process
		\**********************************************************************/
		virtual void close(void);
	};

#endif // WRITERTOFILE_H
