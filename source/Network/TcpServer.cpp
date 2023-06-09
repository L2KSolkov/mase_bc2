#include "TcpServer.h"
#include "../Framework/Logger.h"

extern Logger* debug;

TcpServer::TcpServer(boost::asio::io_service& io_service, int type, int port, Database* db) :
			acceptor_(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
			context_(boost::asio::ssl::context::sslv3)
{
	this->type = type;
	this->port = port;
	this->db = db;

	if(type == GAME_SERVER_SSL || type == GAME_CLIENT_SSL)
	{
		SSL_CTX_set_cipher_list(context_.native_handle(), "ALL");
		SSL_CTX_set_options(context_.native_handle(), SSL_OP_ALL);

		SSL_CTX_use_certificate_ASN1(context_.native_handle(), sizeof(SSL_CERT_X509), SSL_CERT_X509);
		SSL_CTX_use_PrivateKey_ASN1(EVP_PKEY_RSA, context_.native_handle(), SSL_CERT_RSA, sizeof(SSL_CERT_RSA));
		SSL_CTX_set_verify_depth(context_.native_handle(), 1);
	}
	start_accept();
	debug->notification(1, type, "Created TCP Socket (listening on port %i)", port);
}

void TcpServer::start_accept()
{
	if(type == GAME_SERVER_SSL || type == GAME_CLIENT_SSL)
	{
		new_ssl_connection.reset(new TcpConnectionSSL((boost::asio::io_context&)(acceptor_).get_executor().context(), type, context_, db));
		acceptor_.async_accept(	new_ssl_connection->socket(),
								boost::bind(&TcpServer::handle_acceptSSL, this, boost::asio::placeholders::error));
	}
	else
	{
		new_connection.reset(new TcpConnection((boost::asio::io_context&)(acceptor_).get_executor().context(), type, db));
		acceptor_.async_accept(	new_connection->socket(),
								boost::bind(&TcpServer::handle_accept, this, boost::asio::placeholders::error));
	}
}

void TcpServer::handle_accept(const boost::system::error_code& error)
{
	if(!acceptor_.is_open())
	{
		debug->warning(1, type, "TCP Socket, port %i - acceptor is not open!", port);
		return;
	}
	if(!error)
		new_connection->start();
	else
		debug->warning(1, type, "TCP Socket, port %i - error: %s, error code: %i", error.message().c_str(), error.value());
	start_accept();
}

void TcpServer::handle_acceptSSL(const boost::system::error_code& error)
{
	if(!acceptor_.is_open())
	{
		debug->warning(1, type, "TCP Socket, port %i - acceptor is not open!", port);
		return;
	}

	if(!error)
		new_ssl_connection->start();
	else
		debug->warning(1, type, "TCP Socket, port %i - error: %s, error code: %i", error.message().c_str(), error.value());
	start_accept();
}

void TcpServer::handle_stop()
{
	//acceptor_.stop();
	acceptor_.close();
}
