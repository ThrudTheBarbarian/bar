#include <QSet>
#include <QMap>

#include "argsparser.h"
#include "console.h"

/******************************************************************************\
|* Parse commandline arguments and generate the usage() function
|*
|* Idea is that you ...
|*  'add' argument definitions
|*  call 'parse' with argc, argv to obtain values locally
|*  call 'value' or 'flag' to pass the parsed values back to the caller
|*
|* Author	: SjG
\******************************************************************************/
ArgsParser::ArgsParser(QObject *parent)
		   :QObject(parent)
	{
	}

/******************************************************************************\
|* Add an argument to the list of those recognised
\******************************************************************************/
void ArgsParser::add(const char *lng,
					 const char *shrt,
					 bool param,
					 const char *dflt,
					 const char *type,
					 const char *msg)
	{
	Arg arg = {lng, shrt, dflt, param,
			   type ? QString(type) : "",
			   msg  ? QString(msg) : "",
			  ""};
	_args.append(arg);
	}


/******************************************************************************\
|* Parse the passed-in commandline
\******************************************************************************/
bool ArgsParser::process(int argc, char **argv)
	{
	_app	= argv[0];
	bool ok = true;
	QString noSwitch;

	if (argc > 1)
		{
		/**********************************************************************\
		|* First convert to a switch/param list form
		\**********************************************************************/
		_positional.clear();
		QVector<QString> switches;

		QString first = _checkSmallSwitches(argv[1]);
		for (int i=1; i<argc; i++)
			{
			bool firstMatch = (i==1) && first.length() > 0;
			bool isSwitch   = argv[i][0] == '-';

			if (firstMatch || isSwitch)
				switches.append(argv[i]);
			else
				_positional.append(argv[i]);
			}

		/**********************************************************************\
		|* Create a set of all the shorter switches to test against
		\**********************************************************************/
		QSet<QString> shrtArgs;
		for (auto i=_args.begin(); i != _args.end(); i++)
			if (i->shrt.length() > 0)
				shrtArgs.insert(i->shrt);

		/**********************************************************************\
		|* Create a set of all the longer switches to test against
		\**********************************************************************/
		QSet<QString> lngArgs;
		for (auto i=_args.begin(); i != _args.end(); i++)
			if (i->lng.length() > 0)
				lngArgs.insert(i->lng);

		/**********************************************************************\
		|* Now expand the first switch if it's a combination of switches
		\**********************************************************************/
		QStringList newSwitches;
		if (first.length() > 0)
			{
			QString first = switches.first();
			switches.removeFirst();
			for (auto i=first.begin(); i != first.end(); i++)
				{
				QString swtch = QString("-").append(*i);
				newSwitches.append(swtch);
				}
			}
		else if (argv[1][0] != '-')
			{
			noSwitch = argv[1];
			ok = false;
			}
		else if (!(lngArgs.contains(argv[1]) || (shrtArgs.contains(argv[1]))))
			{
			noSwitch = argv[1];
			ok = false;
			}

		/**********************************************************************\
		|* Now expand remaining switches so catenated switches are singled out
		\**********************************************************************/
		for (auto i=switches.begin(); i != switches.end(); i++)
			{
			if (lngArgs.contains(*i))
				newSwitches.append(*i);
			else
				{
				QByteArray vec  = i->mid(1).toLocal8Bit();
				QString smlOpts = _checkSmallSwitches(vec.data());
				if (smlOpts.length()>0)
					{
					for (auto i=smlOpts.begin(); i != smlOpts.end(); i++)
						{
						QString swtch = QString("-").append(*i);
						newSwitches.append(swtch);
						}
					}
				else
					{
					noSwitch = *i;
					ok = false;
					}
				}
			}

		/**********************************************************************\
		|* Now run through each of the single-item switches in 'newSwitches' and
		|* populate any argument values they might need from the _positional
		|* StringList
		\**********************************************************************/
		for (auto i=newSwitches.begin(); i != newSwitches.end(); i++)
			{
			Arg which;
			int at = _find(*i, which);

			if (at >= 0)
				{
				if (which.param)
					{
					if (_positional.size() > 0)
						{
						which.value.append(_positional.front());
						_positional.removeFirst();
						}
					else
						{
						ok = false; // FIXME
						break;
						}
					}
				else
					which.value = "1";

				_args.replace(at,which);
				}
			}
		}

	if (!ok)
		fprintf(stderr, "Unrecognised switch '%s'\n",
					noSwitch.toStdString().c_str());

	return ok;
	}


/******************************************************************************\
|* Parse the passed-in commandline
\******************************************************************************/
QStringList ArgsParser::remainingArgs(void)
	{
	return _positional;
	}

/******************************************************************************\
|* Return the value of an arg with a parameter
\******************************************************************************/
QString ArgsParser::value(QString token, bool isShort)
	{
	static QString empty = "";
	Arg *arg = nullptr;

	for (auto i=_args.begin(); (i != _args.end()); i++)
		if (isShort && i->shrt == token)
			{
			arg = i;
			break;
			}
		else if ((!isShort) && i->lng == token)
			{
			arg = i;
			break;
			}
	if (arg)
		return arg->value.length() > 0 ? arg->value : arg->dflt;
	return empty;
	}

/**********************************************************************\
|* Return the presence of an arg with no parameters
\**********************************************************************/
bool ArgsParser::flag(QString token, bool isShort)
	{
	Arg *arg = nullptr;

	for (auto i=_args.begin(); (i != _args.end()); i++)
		if (isShort && i->shrt == token)
			{
			arg = i;
			break;
			}
		else if ((!isShort) && i->lng == token)
			{
			arg = i;
			break;
			}
	if (arg)
		return arg->value.length() > 0 ? true : false;
	return false;
	}


/**********************************************************************\
|* Provide usage
\**********************************************************************/
void ArgsParser::usage(QString extra, bool shouldExit)
	{
	// Group the options by type
	QMap<QString, QVector<Arg>> groups;
	for (auto i=_args.begin(); i!=_args.end(); i++)
		{
		QString grp = (i->type.length() == 0) ? "Misc" : i->type;
		if (!groups.contains(grp))
			groups.insert(grp, QVector<Arg>());
		groups[grp].append(*i);
		}

	QString title = QString("\nUsage: %1 <action> [opts] where opts are:\n\n").arg(_app);
	title		  = Console::colour(title, Console::RED);
	printf("%s", title.toStdString().c_str());

	int llen = 0;
	int hlen = 0;

	// determine the max lengths of args and help text
	foreach (const auto& grp, groups.keys())
		{
		const auto& vec = groups[grp];
		for (auto j=vec.begin(); j != vec.end(); j++)
			{
			int len = j->lng.length();
			llen	= (llen > len) ? llen : len;

			len		= j->msg.length();
			hlen	= (hlen > len) ? hlen : len;
			}
		}


	// Dump the help text out in a formatted fashion
	QString spc = " ";
	foreach (const auto& grp, groups.keys())
		{
		QString title = QString("%1:").arg(grp);
		QString out = Console::colour(title, Console::BLUE);
		printf(" %s\n", out.toStdString().c_str());

		const auto& vec = groups[grp];
		for (auto j=vec.begin(); j != vec.end(); j++)
			{
			QString pad1 = spc.repeated(llen - j->lng.length());
			QString pad2 = spc.repeated(hlen - j->msg.length());

			QString dflt = "";
			if (j->dflt.length() > 0)
				dflt = QString("%1 [%2]").arg(pad2,j->dflt);

			printf("  %s | %s%s : %s%s\n",
				   j->shrt.toStdString().c_str(),
				   j->lng.toStdString().c_str(),
				   pad1.toStdString().c_str(),
				   j->msg.toStdString().c_str(),
				   dflt.toStdString().c_str());
			}
		printf("\n");
		}

	printf("%s", extra.toStdString().c_str());
	if (shouldExit)
		exit(0);
	}

#pragma mark - private methods

/******************************************************************************\
|* Generate the first argument if it solely consist of short args
\******************************************************************************/
QString ArgsParser::_checkSmallSwitches(char *firstWord)
	{
	QSet<char> lookup	= _setOfSingleLetterSwitches();

	int len	= (int) strlen(firstWord);
	bool ok = true;
	for (int i=0; (i<len) && ok; i++)
		if (!lookup.contains(firstWord[i]))
			ok = false;

	return (ok) ? firstWord : "";
	}

/******************************************************************************\
|* Generate a set of all possible args with single-letter switches
\******************************************************************************/
QSet<char> ArgsParser::_setOfSingleLetterSwitches(void)
	{
	QSet<char> lookup;
	for (auto i=_args.begin(); i != _args.end(); i++)
		{
		if ((i->shrt.length() > 0) && (i->shrt.length() < 3))
			{
			QByteArray vec     = i->shrt.toLocal8Bit();
			if ((vec.length() == 2) && (vec.data()[0] == '-'))
				lookup.insert(vec.data()[1]);
			else
				lookup.insert(vec.data()[0]);
			}
		}
	lookup.insert('-');
	return lookup;
	}

/******************************************************************************\
|* Find and return an Arg for a given switch (long or short)
\******************************************************************************/
int ArgsParser::_find(QString name, ArgsParser::Arg &arg)
	{
	int at = -1;

	int count = 0;
	for (auto i=_args.begin(); i != _args.end(); i++)
		{
		if ((i->shrt == name) || (i->lng == name))
			{
			at = count;
			arg = *i;
			break;
			}
		count ++;
		}

	return at;
	}
