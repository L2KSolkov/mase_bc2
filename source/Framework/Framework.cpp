#include "Framework.h"
#include "Logger.h"
#include "../Base64.h"
//for randomString
#include <boost/random/random_device.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/filesystem/operations.hpp>
#ifndef _WIN32
	#include <time.h>	// to get date and time
#endif

extern Logger* debug;
const char* months[12] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};

/*struct encoding
{
	const char* real;
	const char* encoded_up;
	const char* encoded_low;
}encoded_chars[38] =
{
	{" ", "%20", "%20"},
	{"!", "%21", "%21"},
	{"\"", "%22", "%22"},	//
	{"#", "%23", "%23"},
	{"$", "%24", "%24"},
	//{"%", "%25", "%25"},
	{"&", "%26", "%26"},
	{"\'", "%27", "%27"},
	{"(", "%28", "%28"},
	{")", "%29", "%29"},
	{"+", "%2B", "%2b"},
	{",", "%2C", "%2c"},
	{"-", "%2D", "%2d"},
	{".", "%2E", "%2e"},
	{";", "%3B", "%3b"},
	{"=", "%3D", "%3d"},	//
	{"@", "%40", "%40"},
	{"[", "%5B", "%5b"},
	{"]", "%5D", "%5d"},
	{"^", "%5E", "%5e"},
	{"_", "%5F", "%5f"},
	{"`", "%60", "%60"},
	{"{", "%7B", "%7b"},
	{"}", "%7D", "%7d"},
	{"~", "%7E", "%7e"},
	{"�", "%82", "%82"},
	{"§", "%A7", "%a7"},	//�
	{"°", "%B0", "%b0"},	//�
	{"´", "%B4", "%b4"},	//�
	{"µ", "%B5", "%b4"},	//�
	{"Ä", "%C4", "%c4"},	//�
	{"�", "%C7", "%c7"},
	{"Ö", "%D6", "%d6"},	//�
	{"Ü", "%DC", "%dc"},	//�
	{"ß", "%DF", "%df"},	//�
	{"ä", "%E4", "%e4"},	//�
	{"�", "%E7", "%e7"},
	{"ö", "%F6", "%f6"},	//�
	{"ü", "%FC", "%fc"}	//�
};*/

Framework::Framework(emulatorSettings emuSettings, connectionSettings ports, bool logDatabase)
{
	plasmaClients = 0;
	plasmaServers = 0;
	theaterClients = 0;
	theaterServers = 0;
	maxAllowedServers = 1024;
	maxAllowedClients = 8192;
	updateConnections();
	this->emuSettings = emuSettings;
	this->ports = ports;
	this->logDatabase = logDatabase;

	this->emuLocal = false;
}

std::string Framework::getTime()
{
	//format: "Jan-00-0000 00%3a00%3a00 UTC"
	char str[32];
#if defined (_WIN32)
	SYSTEMTIME st = {0};
	GetLocalTime(&st);
	sprintf(str, "\"%s-%02d-%04d %02d%%3a%02d%%3a%02d UTC\"", months[st.wMonth-1], st.wDay, st.wYear, st.wHour, st.wMinute, st.wSecond);
#else
	time_t now = time(0);
	struct tm st = *localtime(&now);
	sprintf(str, "\"%s-%02d-%04d %02d%%3a%02d%%3a%02d UTC\"", months[st.tm_mon], st.tm_mday, 1900+st.tm_year, st.tm_hour, st.tm_min, st.tm_sec);
#endif
	std::string time = str;
	return time;
}
/*
void Framework::convertHtml(std::string* data)
{
	int pos = 0;
	while((pos = data->find('%', pos)) != std::string::npos)
	{
		std::string enc_char = data->substr(pos, pos+3);
		for(int i = 0; i < 38; i++)
		{
			if(enc_char.compare(encoded_chars[i].encoded_up) == 0)
			{
				enc_char = encoded_chars[i].real;
				data->replace(pos, enc_char.size(), enc_char.c_str());
				break;
			}
		}
		pos++;
	}
}
*/
bool Framework::DatabaseLogging()
{
	return logDatabase;
}

void Framework::checkFiles()
{
	if(!boost::filesystem::exists("./database"))
	{
		debug->warning(1, DEBUG, "The subdirectory 'database' was not found, creating a default one");
		boost::filesystem::path dir("./database");
		if(!boost::filesystem::create_directory(dir))
			debug->warning(1, DEBUG, "Unable to create subdirectory 'database', check your permissions");

		std::ofstream users("./database/users.lst", std::ios::out | std::ios::app);
		if(users.is_open() && users.good())
			users.close();
		else
			debug->warning(1, DEBUG, "Failed to create users.lst");

		std::ofstream personas("./database/personas.lst", std::ios::out | std::ios::app);
		if(personas.is_open() && personas.good())
			personas.close();
		else
			debug->warning(1, DEBUG, "Failed to create personas.lst");
	}
	else
	{
		if(!boost::filesystem::exists("./database/users.lst"))
		{
			debug->warning(1, DEBUG, "The file 'users.lst' was not found in the subfolder 'database', creating an empty one (which means no users were loaded)");
			std::ofstream users("./database/users.lst", std::ios::out | std::ios::app);
			if(users.is_open() && users.good())
				users.close();
			else
				debug->warning(1, DEBUG, "Failed to create users.lst");
		}

		if(!boost::filesystem::exists("./database/personas.lst"))
		{
			debug->warning(1, DEBUG, "The file 'personas.lst' was not found in the subfolder 'database', creating an empty one (which means no personas were loaded)");
			std::ofstream personas("./database/personas.lst", std::ios::out | std::ios::app);
			if(personas.is_open() && personas.good())
				personas.close();
			else
				debug->warning(1, DEBUG, "Failed to create personas.lst");
		}
	}

	if(!boost::filesystem::exists("./templates"))
	{
		debug->warning(1, DEBUG, "The subdirectory 'templates' was not found, the emulator will not be able to create any personas without it (and might even crash) so make sure you have the original folder with all required files and restart the process!");
		boost::filesystem::path dir("./templates");
		if(!boost::filesystem::create_directory(dir))
			debug->warning(1, DEBUG, "Unable to create subdirectory 'templates', check your permissions");
	}
}

//generates random std::string (e.g. for lkey)
std::string Framework::randomString(int len, int seed_type)
{
	std::string buffer, chars;
	boost::random::random_device rng;
	switch(seed_type)
	{
		case SEED_BRAC:
			chars = ("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ][");
		case SEED_UPPERCASE:	//uppercase only
			chars = ("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
		case SEED_NUMBERS:		//numbers only
			chars = ("1234567890");
		case SEED_DEFAULT:
		default:
			chars = ("abcdefghijklmnopqrstuvwxyz"
					 "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
					 "1234567890-_");
	}
	boost::random::uniform_int_distribution<> index_dist(0, chars.size()-1);
	for(int i = 0; i < len; i++)
		buffer.push_back(chars[index_dist(rng)]);
	return buffer;
}

void Framework::addJoinableServer(serverConnectionInfo server)
{
	serverJoinableList.push_back(server);
}

TcpConnection* Framework::getServerSocket(int id)
{
	for(std::list<serverConnectionInfo>::iterator it = serverJoinableList.begin(); it != serverJoinableList.end(); it++)
	{
		if(it->id == id)
			return it->socket.get();
	}
	return NULL;
}

bool Framework::removeJoinableServer(int id)
{
	for(std::list<serverConnectionInfo>::iterator it = serverJoinableList.begin(); it != serverJoinableList.end(); it++)
	{
		if(it->id == id)
		{
			serverJoinableList.erase(it);
			return true;
		}
	}
	return false;
}

bool Framework::addConnection(int type)
{
	bool added = true;
	switch(type)
	{
		case GAME_SERVER:
			theaterServers++;
			break;
		case GAME_SERVER_SSL:
		{
			if(plasmaServers+1 <= this->maxAllowedServers)
				plasmaServers++;
			else
				added = false;
			break;
		}
		case GAME_CLIENT:
			theaterClients++;
			break;
		case GAME_CLIENT_SSL:
		{
			if(plasmaClients+1 <= this->maxAllowedClients)
				plasmaClients++;
			else
				added = false;
			break;
		}
		default:
			added = false;
	}
	if(added)
		updateConnections();

	return added;
}

void Framework::subtractConnection(int type)
{
	bool added = true;
	switch(type)
	{
		case GAME_SERVER:
			theaterServers--;
			break;
		case GAME_SERVER_SSL:
			plasmaServers--;
			break;
		case GAME_CLIENT:
			theaterClients--;
			break;
		case GAME_CLIENT_SSL:
			plasmaClients--;
			break;
		default:
			added = false;
	}
	if(added)
		updateConnections();
}

int Framework::clientConnections()
{
	return plasmaClients;
}

int Framework::serverConnections()
{
	return plasmaServers;
}

int Framework::totalConnections()
{
	int connections = clientConnections()+serverConnections();
	connections += theaterClients+theaterServers;
	return connections;
}

void Framework::updateConnections()
{
#if defined(_WIN32)
#ifndef _DEMO_RELEASE
	char windowTitle[64];
	sprintf(windowTitle, "Total Connections: %i - Clients: %i - Servers: %i", totalConnections(), clientConnections(), serverConnections());
	debug->updateTitle(windowTitle);
#endif
#else
	// do nothing in linux since there is no real equivalent to what we do in windows (update console window title)
#endif
}

void Framework::resolveEmuIp(boost::asio::io_service* io_service)
{
	std::vector<std::string> ip_list;
	boost::asio::ip::tcp::resolver resolver(*io_service);
	boost::asio::ip::tcp::resolver::query query(ports.emulator_ip, "");
	for(boost::asio::ip::tcp::resolver::iterator it = resolver.resolve(query); it != boost::asio::ip::tcp::resolver::iterator(); ++it)
	{
		boost::asio::ip::tcp::endpoint end = *it;
		if(end.protocol() == boost::asio::ip::tcp::v4())	// only want ip's in v4 format
			ip_list.push_back(end.address().to_string());
	}

	// if the hostname gives multiple IP's should we even check if all of them are (not) local?
	// and what if one is local and the others aren't? in that case just set it to local setting (to be safe)
	for(std::vector<std::string>::iterator h = ip_list.begin(); h != ip_list.end(); h++)
	{
		ports.emulator_ip = *h; // in case of a given hostname we don't want to re-resolve the domain again so we just store the last checked ip from the resolved list instead
		emuLocal = isIpLocal(ports.emulator_ip);
	}
}

bool Framework::isIpLocal(std::string ip)
{
	bool isLocal = false;
	int count = sizeof(filteredIPRanges)/sizeof(*filteredIPRanges);

	for(int i = 0; i < count && !isLocal; i++)
	{
		bool rangeFits = true;
		int pos = -1, end = -1, range = -1;
		for(int j = 0; j < 4 && rangeFits; j++)
		{
			pos = end+1;
			end = ip.find_first_of('.', pos);
			if(end == (int)std::string::npos)
				end = ip.size();
			range = boost::lexical_cast<int>(ip.substr(pos, end-pos));

			if(range < filteredIPRanges[i][0][j] || range > filteredIPRanges[i][1][j])
				rangeFits = false;
		}
		if(rangeFits)
			isLocal = true;
	}
	return isLocal;
}

bool Framework::isEmuLocal()
{
	return emuLocal;
}

emulatorSettings Framework::emuCfg()
{
	return emuSettings;
}

connectionSettings Framework::portsCfg()
{
	return ports;
}
