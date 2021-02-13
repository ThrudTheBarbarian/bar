#ifndef CONSOLE_H
#define CONSOLE_H

#include <QObject>

class Console : public QObject
	{
	Q_OBJECT
	public:
		enum
			{
			BLACK,
			DARKGRAY,
			BLUE,
			LIGHTBLUE,
			GREEN,
			LIGHTGREEN,
			CYAN,
			LIGHTCYAN,
			RED,
			LIGHTRED,
			PURPLE,
			LIGHTPURPLE,
			BROWN,
			YELLOW,
			LIGHTGRAY,
			WHITE,
			MAGENTA,
			COLOUR_MAX
			};

		/**********************************************************************\
		|* Constructors/Destructors
		\**********************************************************************/
		explicit Console(QObject *parent = nullptr);

		static QString colour(QString in, int fg=-1, int bg=-1);

		static bool isDark();

	signals:

	};

#endif // CONSOLE_H
