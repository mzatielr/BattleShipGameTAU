#include "IArgumentParser.h"

void IArgumentParser::ParseArguments(Configuration& config, int argc, char* argv[])
{
	if (argc > 1)
	{
		for (int i = 1; i < argc; i++)
		{
			string s = argv[i];
			if (!s.compare("-quiet")) 
			{
				config.bonusParam.isQuiet = true;
			}
			else if (!s.compare("-delay"))
			{
				int del = atoi(argv[i + 1]);
				config.bonusParam.delayInMiliseconds = del;
				i++;
			}
			else
			{
				config.path = argv[i];
			}
		}
	}
}
