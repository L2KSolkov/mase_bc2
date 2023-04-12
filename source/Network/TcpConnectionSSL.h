#include "../main.h"
#include <boost/asio/ssl.hpp>
#include "Packet.h"
#include "../Framework/GameClient.h"
#include "../Framework/GameServer.h"

typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> socketSSL;

class TcpConnectionSSL : public boost::enable_shared_from_this<TcpConnectionSSL>, private boost::noncopyable
{
	public:
		typedef boost::shared_ptr<TcpConnectionSSL> pointer;
		socketSSL::lowest_layer_type& socket();
		void start();
		TcpConnectionSSL(boost::asio::io_service& io_service, int type, boost::asio::ssl::context& context, Database* db);

	private:
		const char* send_data;
		socketSSL socket_;

		Database* db;
		int type;
		int state;
		int packetCounter;
		enum { max_length = PACKET_MAX_LENGTH };
		unsigned char received_data[max_length];
		int send_length;
		int received_length;
		std::list<Packet*> incomingQueue;
		std::list<Packet*> outgoingQueue;

		boost::asio::deadline_timer* ping_timer;
		boost::asio::deadline_timer* memcheck_timer;
		const char* special_data;
		int special_data_length;
		std::list<Packet*> specialQueue;

		void handle_handshake(const boost::system::error_code& error);	// first sequence of packets for ssl connection
		void handle_read(const boost::system::error_code& error, size_t bytes_transferred);	// normal read (also deals with multiple packets)
		void handle_write(const boost::system::error_code& error);		// normal send (also deals with multiple packets)
		void handle_stop();		// cleanup after a disconnect

		void handle_one_write(const boost::system::error_code& error);	// processes the one-way packet queue (special queue)
		void handle_ping(const boost::system::error_code& error);		// send a one-way ping in a constant interval (dependent on ping_timer)
		void handle_memcheck(const boost::system::error_code& error);	// send a one-way memcheck in a constant interval (dependent on memcheck_timer)

		bool self_created_packet;
		std::string encoded_data;

		GameServer* server;
		GameClient* client;
		void ProcessPacket(Packet* receivedPacket);

		void StoreIncomingData();
		char* PacketToData(Packet* packet, bool is_special = false);
		unsigned int decode(unsigned char* data, int bytes);
		unsigned int encode(uint8_t *data, uint32_t num, int bytes);

		// need remoteIp and port stored when error occurs to get it because at that time the socket is already destroyed (linux only?)
		std::string emuIp, remoteIp;
		int remotePort;

		std::string getLocalIp();
		int getLocalPort();
		std::string getRemoteIp();
		int getRemotePort();
};
