#pragma once
#include "../main.h"

class Packet
{
	private:
		bool count;
		bool first;
		bool encoded;
		bool delayed;
		int delay_time;
		char*			type;
		unsigned int	type2;
		int				length;
		std::string			var_buffer;

	public:
		Packet(const char* createType, unsigned int createType2, size_t createLength, const char* createData);	//constructor for receiving a packet
		Packet(const char* createType, unsigned int createType2, bool count = true, bool first = true);	//constructor for preparing a packet
		Packet(Packet* packet);		//copy constructor
		~Packet();

		std::string GetVar(std::string varname);
		std::string GetVar(const char* varname);
		void SetVar(std::string varname, std::string varvalue, bool skipCheck = true);
		void SetVar(std::string varname, int varvalue, bool skipCheck = true);
		void SetType2(unsigned int type2);
		int GetNumberOfKeys();
		std::string toString();

		std::string GetData();
		char* GetType();
		unsigned int GetType2();
		int	GetLength();
		bool isEncoded();
		void isEncoded(bool flag);
		bool isDelayed();
		int getDelayTime();
		void isDelayed(bool flag, int seconds);
		bool GetValCount();
		bool GetValFirst();
};
