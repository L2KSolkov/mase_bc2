#include "../main.h"
#include "Http/HttpConnection.h"
#include "Http/RequestHandler.h"

class HttpServer : private boost::noncopyable
{
	public:
		HttpServer(boost::asio::io_service& io_service, bool updater_respond);
		void handle_stop();

	private:
		boost::asio::ip::tcp::acceptor acceptor_;
		bool updater_respond;

		/// The next connection to be accepted.
		http_connection_ptr new_connection_;
		/// The handler for all incoming requests.
		RequestHandler request_handler_;

		void start_accept();
		void handle_accept(const boost::system::error_code& error);
};
