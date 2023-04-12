#pragma once
#include "../main.h"
#include "../Network/TcpConnection.h"

enum{
	SEED_DEFAULT,
	SEED_NUMBERS,
	SEED_UPPERCASE,
	SEED_BRAC,

	SEED_NUM
};

// these should be the possible ipv4 address ranges for local networks (is there a better way to determine if local?)
const static int filteredIPRanges[3][2][4] = {
	{{127, 0, 0, 1}, {127, 0, 0, 1}},
	{{172, 16, 0, 0}, {172, 31, 255, 255}},
	{{192, 168, 0, 0}, {192, 168, 255, 255}}
};

class Framework
{
	private:
		int theaterClients, theaterServers;
		int plasmaClients, plasmaServers;
		int maxAllowedServers, maxAllowedClients;
		bool logDatabase, emuLocal;

		std::list<serverConnectionInfo> serverJoinableList;
		emulatorSettings emuSettings;
		connectionSettings ports;

		int totalConnections();
		int serverConnections();
		int clientConnections();
		void updateConnections();

	public:
		Framework(emulatorSettings emuSettings, connectionSettings ports, bool logDatabase);

		std::string getTime();
		std::string randomString(int len, int seed_type);
		void convertHtml(std::string* data);

		bool DatabaseLogging();
		void checkFiles();

		void addJoinableServer(serverConnectionInfo server);
		TcpConnection* getServerSocket(int id);
		bool removeJoinableServer(int id);

		bool addConnection(int type);
		void subtractConnection(int type);

		void resolveEmuIp(boost::asio::io_service* io_service);
		bool isIpLocal(std::string ip);
		bool isEmuLocal();
		emulatorSettings emuCfg();
		connectionSettings portsCfg();
};
