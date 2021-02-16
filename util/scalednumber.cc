#include "scalednumber.h"

int64_t ScaledNumber::toNumber(QString input)
	{
	int64_t value	= 0;
	int64_t mult	= 1;

	if (input.length() > 0)
		{
		QByteArray last		= input.right(1).toLocal8Bit();
		switch (last.data()[0])
			{
			case 'k':
			case 'K':
				mult = 1024;
				break;
			case 'M':
			case 'm':
				mult = 1024*1024;
				break;
			case 'T':
			case 't':
				mult = 1024*1024*1024;
				break;
			}
		value = input.left(input.length()-1).toLongLong();
		}
	else
		value = input.toLongLong();

	return value * mult;
	}
