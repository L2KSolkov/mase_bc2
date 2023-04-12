#include "TcpConnection.h"
#include "TcpConnectionSSL.h"
#include "../SSL_cert.h"

class TcpServer : private boost::noncopyable
{
	public:
		TcpServer(boost::asio::io_service& io_service, int type, int port, Database* db);
		void handle_stop();

	private:
		boost::asio::ip::tcp::acceptor acceptor_;
		boost::asio::ssl::context context_;
		TcpConnectionSSL::pointer new_ssl_connection;
		TcpConnection::pointer new_connection;

		int type;	//we only differentiate between SSL and non-SSL here
		int port;
		Database* db;

		void start_accept();
		void handle_accept(const boost::system::error_code& error);
		void handle_acceptSSL(const boost::system::error_code& error);
};
