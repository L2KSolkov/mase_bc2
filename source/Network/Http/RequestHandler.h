#pragma once
#include "../../main.h"
#include "Reply.h"

/// The common handler for all incoming requests.
class RequestHandler : private boost::noncopyable
{
	public:
		/// Construct with a directory containing files to be served.
		explicit RequestHandler(const std::string& doc_root);

		/// Handle a request and produce a reply.
		void handle_request(const request& req, reply& rep, bool updater_respond);

	private:
		/// The directory containing the files to be served.
		std::string doc_root_;

		/// Perform URL-decoding on a std::string. Returns false if the encoding was
		/// invalid.
		static bool url_decode(const std::string& in, std::string& out);
};
