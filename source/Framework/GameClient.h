#pragma once
#include "../main.h"
#include "../Network/Packet.h"
#include "Stats.h"
#include "Database.h"

class GameClient
{
	private:
		int connectionType;
		Database* database;
		bool logDatabase;
		list_entry persona;
		list_entry user;

		//clientInfoPlasma->
		std::string	user_lkey;
		std::string	persona_lkey;
		Stats*	stats_data;
		//<-clientInfoPlasma

		//clientInfoTheater->
		int		sock_tid;
		int		queue_position;
		int		original_queue_length;
		//<-clientInfoTheater

		bool isLocal;	// is the client in the same network as the emulator?
		std::string emuIp;
		bool alreadyLoggedIn;
		std::list<std::string> playerList;
		void filterGames(Packet* packet, std::list<int>* filtered);

	public:
		GameClient(int connectionType, Database* db, std::string emuIp, bool isLocal);
		~GameClient();

		bool acct(std::list<Packet*>* queue, Packet* packet, std::string txn, const char* ip);
		bool asso(std::list<Packet*>* queue, Packet* packet, std::string txn);
		bool xmsg(std::list<Packet*>* queue, Packet* packet, std::string txn);
		bool pres(std::list<Packet*>* queue, Packet* packet, std::string txn);
		bool recp(std::list<Packet*>* queue, Packet* packet, std::string txn);
		bool pnow(std::list<Packet*>* queue, Packet* packet, std::string txn);
		int rank(std::list<Packet*>* queue, Packet* packet, std::string txn);

		int ProcessTheater(std::list<Packet*>* queue, Packet* packet, char* type, const char* ip);
};
