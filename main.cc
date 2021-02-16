#include <QCoreApplication>

#include "argsparser.h"
#include "console.h"
#include "create/creator.h"

int main(int argc, char *argv[])
	{
	QCoreApplication a(argc, argv);
	char threads[32];
	sprintf(threads, "%d", QThread::idealThreadCount());

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

	args.add("--num-threads", "-#", true, threads, "Compression",
			"Number of file i/o threads");
	args.add("--block-size", "-bs", true, "10M", "Compression",
			"Max size of a compressed/file block");

	args.process(argc, argv);

	if (args.flag("-h"))
		args.usage();

	/**************************************************************************\
	|* Decide on which action to take
	\**************************************************************************/
	if (args.flag("-c"))
		{
		Creator creator(&a);
		creator.create();
		}
	}
