#include <QCoreApplication>

#include "argsparser.h"
#include "console.h"
#include "create/creator.h"

int main(int argc, char *argv[])
	{
	QCoreApplication a(argc, argv);

	/**************************************************************************\
	|* Handle the commandline arguments
	\**************************************************************************/
	ArgsParser args(&a);

	args.add("--create", "-c", false, NULL, "Actions", "Create an archive");
	args.add("--list", "-t", false, NULL, "Actions", "List an archive");
	args.add("--extract", "-x", false, NULL, "Actions", "Extract an archive");

	args.add("--file", "-f", true, "/dev/st0", "i/o",
			 "Specify the file/device for writing");
	args.add("--compress", "-j", false, "false", "i/o",
			 "Compress the data before write");

	args.add("--help", "-h", false, "false", "Misc", "This wonderful help text");
	args.add("--verbose", "-v", false, "false", "Misc", "Whether to be chatty");

	args.process(argc, argv);

	if (args.flag("-h"))
		args.usage();

	/**************************************************************************\
	|* Decide on which action to take
	\**************************************************************************/
	if (args.flag("-c"))
		{
		Creator creator(&a);
		creator.setFile(args.value("-f"));
		creator.setVerbose(args.flag("-v"));
		creator.setPaths(args.remainingArgs());

		creator.create(args.flag("-j"));
		}

	a.exec();
	}
