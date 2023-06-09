#include "HttpServer.h"
#include "../Framework/Logger.h"

extern Logger* debug;

HttpServer::HttpServer(boost::asio::io_service& io_service, bool updater_respond) : acceptor_(io_service), new_connection_(), request_handler_("./templates/")
{
	boost::asio::ip::tcp::resolver resolver(io_service);
	boost::asio::ip::tcp::resolver::query query("0.0.0.0", "80");
	boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);

	acceptor_.open(endpoint.protocol());
	acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
	acceptor_.bind(endpoint);
	acceptor_.listen();

	this->updater_respond = updater_respond;
	start_accept();
	debug->notification(1, HTTP, "Created HTTP Socket (listening on port 80)");
}

void HttpServer::start_accept()
{
	new_connection_.reset(new HttpConnection((boost::asio::io_context&)(acceptor_).get_executor().context(), request_handler_, updater_respond));
	acceptor_.async_accept(new_connection_->socket(),
							boost::bind(&HttpServer::handle_accept, this, boost::asio::placeholders::error));
}

void HttpServer::handle_accept(const boost::system::error_code& e)
{
	if (!e)
		new_connection_->start();

	start_accept();
}
