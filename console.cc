#include <unistd.h>
#include <stdio.h>

#include "console.h"

static const char * _fg[] =
	{
	"0;30",
	"1;30",
	"0;34",
	"1;34",
	"0;32",
	"1;32",
	"0;36",
	"1;36",
	"0;31",
	"1;31",
	"0;35",
	"1;35",
	"0;33",
	"1;33",
	"0;37",
	"1;37"
	};

static const char * _bg[] =
	{
	"40",
	"41",
	"42",
	"43",
	"44",
	"45",
	"46",
	"47"
	};

Console::Console(QObject *parent) : QObject(parent)
	{}

/******************************************************************************\
|* Return a colourised string if we're on a console
\******************************************************************************/
QString Console::colour(QString in, int fg, int bg)
	{
	if (!isatty(STDOUT_FILENO))
		return in;

	QString foreground = "";
	if (fg >= 0 && fg < Console::COLOUR_MAX)
		foreground = QString("\033[%1m").arg(_fg[fg]);

	QString background = "";
	if (bg >= 0 && bg <8)
		background = QString("\033[%1m").arg(_bg[bg]);

	if ((foreground.length() == 0) && (background.length() == 0))
		return in;

	return QString("%1%2%3\033[0m").arg(foreground,background,in);
	}

/******************************************************************************\
|* Determine if the console has a dark background
\******************************************************************************/
bool Console::isDark(void)
	{
	bool isDark = false;
	printf("%c]11;?%c\\", 27, 27);

	char buf[1024];
	fread(buf,1, 24, stdin);
	buf[25] = '\0';

	/*char *rgb = strstr(buf, "rgb:");
	if (rgb != nullptr)
		{
		int r,g,b;
		sscanf(rgb+4,"%x/%x/%x", &r, &g, &b);
		if ((r < 0x4fff) && (g < 0x4fff) && (b < 0x4fff))
			isDark = true;
		}
*/
	return isDark;
	}
