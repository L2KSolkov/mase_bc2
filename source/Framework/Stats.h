#pragma once
#include "../main.h"

class Stats
{
	private:
		std::string email, persona, path, dogtagPath, data, dogtags;

	public:
		Stats();
		Stats(std::string email, std::string persona, bool createNew = false, bool initOnly = false);

		struct dogtag
		{
			std::string id;
			std::string data;
		};

		void saveStats();
		std::string getKey(std::string key);
		bool setKey(std::string key, std::string value, bool ignore_warning = false);
		bool isEmpty();

		int loadDogtags();					// returns number of dogtags for persona
		dogtag getDogtag(int lineNumber);	// returns a dogtag value from the specified line number in the file
		void addDogtag(dogtag newDogtag);
		void saveDogtags();

		void deleteStats();
};
