#ifndef ARGSPARSER_H
#define ARGSPARSER_H

#include <QObject>

class ArgsParser : public QObject
	{
	Q_OBJECT

	private:
		typedef struct
			{
			QString lng;					// Long form argument
			QString shrt;					// short form argument
			QString dflt;					// Default value if none given
			bool param;						// This argument has a parameter
			QString type;					// Section to put cmd in, in help
			QString msg;					// Used in help-generation
			QString value;					// The parsed value
			} Arg;

		QVector<Arg>	_args;				// List of argument definitions
		QStringList		_positional;		// The list of parameters
		char *			_app;				// The application name

	public:
		/**********************************************************************\
		|* Constructors/Destructors
		\**********************************************************************/
		explicit ArgsParser(QObject *parent = nullptr);

		/**********************************************************************\
		|* Once constructed in main(), allow global access to the args parser
		\**********************************************************************/
		static ArgsParser * sharedInstance(void);

		/**********************************************************************\
		|* Define a queryable argument.
		|*
		|* Any argument with a short arg of only one letter can also be used
		|* without '-' in the first argument, if that argument solely consists
		|* of short argument flags
		\**********************************************************************/
		void add(const char *lng,			// Long form argument
				 const char *shrt,			// short form argument
				 bool param = false,		// This argument has a parameter
				 const char * dflt = NULL,	// Default value if none given
				 const char *type = NULL,	// Section to put cmd in, in help
				 const char *msg = NULL		// Readable 'dflt', if useful
				);


		/**********************************************************************\
		|* Process the arguments according to the specifications registered
		\**********************************************************************/
		bool process(int argc, char *argv[]);

		/**********************************************************************\
		|* Return the value of an arg with a parameter
		\**********************************************************************/
		QString value(QString token, bool isShort=true);

		/**********************************************************************\
		|* Return the presence of an arg with no parameters
		\**********************************************************************/
		bool flag(QString token, bool isShort=true);

		/**********************************************************************\
		|* Generate the help text
		\**********************************************************************/
		void usage(QString extra="", bool shouldExit=true);

		/**********************************************************************\
		|* Return any left-over args after arg-parsing
		\**********************************************************************/
		QStringList remainingArgs(void);

	private:
		/**********************************************************************\
		|* Generate the first argument if it solely consist of short args
		\**********************************************************************/
		QString _checkSmallSwitches(char *txt);

		/**********************************************************************\
		|* Generate a set of all possible args with single-letter switches
		\**********************************************************************/
		QSet<char> _setOfSingleLetterSwitches(void);

		/**********************************************************************\
		|* Find and return an Arg for a given switch (long or short)
		\**********************************************************************/
		int _find(QString name, Arg& arg);
	};

#endif // ARGSPARSER_H
