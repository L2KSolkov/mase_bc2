#include "../main.h"

class Config
{
	private:
		std::string data;
		debugSettings debugCfg;
		connectionSettings portsCfg;
		emulatorSettings emuCfg;
		consoleSettings conCfg;

		std::string getValue(const char* key);
		bool setValue(std::string value, int& storage);
		bool setValue(std::string value, bool& storage);
		bool setValue(std::string value, std::string& storage, bool allow_empty = false);

		bool getValuesFromSection(const char* section);
		bool createDefaultConfig();
		bool storeValues();

	public:
		Config();
		bool loadConfigFile();
		debugSettings getDebugCfg();
		consoleSettings getConCfg();
		emulatorSettings getEmuCfg();
		connectionSettings getPortsCfg();
};
