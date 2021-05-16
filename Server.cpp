#include <cpprest/http_listener.h>
#include <cpprest/json.h>

#include <iostream>
#include <string>
#include <utility>
#include <thread>

using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;
using namespace std;
using namespace utility;

string GString;
//curl -X POST -H "Content-Type:application/json" -d \"test_string\" http://localhost/
void POST(http_request request)
{
	cout << "\nRecived POST request" << endl;

	auto extracted = request.extract_json();
	auto jvalue = extracted.get();
	try
	{
		if (!jvalue.is_null())
		{
			auto str = jvalue.as_string();
			GString = conversions::to_utf8string(str);
			request.reply(status_codes::OK, json::value(conversions::to_string_t(" Status posted")));
		}
		else
		{
			request.reply(status_codes::OK, json::value(conversions::to_string_t(" Empty request")));
		}
	}
	catch (const http_exception& e)
	{
		cout << "ERROR: " << e.what() << endl;
	}
}

void GET(http_request request)
{
	cout << "\nRecived GET request" << endl;
	string answerString;
	if (!GString.empty())
	{
		request.reply(status_codes::OK, json::value(conversions::to_string_t(" Current status: " + GString)));
	}
	else
	{
		request.reply(status_codes::OK, json::value(conversions::to_string_t(" Status not initialized")));
	}
}

void PUT(http_request request)
{
	cout << "\nRecived PUT request" << endl;

	auto extracted = request.extract_json();
	auto jvalue = extracted.get();
	auto str = jvalue.as_string();
	if (!GString.empty())
	{
		GString = conversions::to_utf8string(str);
		request.reply(status_codes::OK, json::value(conversions::to_string_t(" Updated"), true));
	}
	else
	{
		GString = conversions::to_utf8string(str);
		request.reply(status_codes::OK, json::value(conversions::to_string_t(" Set"), true));
	}
}

void DELETE(http_request request)
{
	cout << "\nRecived DELETE request" << endl;

	GString.clear();
	request.reply(status_codes::OK, json::value(conversions::to_string_t(" Status deleted"), true));
}

void server(http_listener& listener)
{
	try
	{
		listener.open().then([&listener]() {
			cout << "\nWAITING FOR REQUEST...\n" << endl;
			}).wait();
			while (1);
	}
	catch (const http_exception& e)
	{
		cout << "ERROR: " << e.what() << endl;
	}
}

int main()
{
	http_listener listener(conversions::to_string_t("http://localhost/"));
	listener.support(methods::POST, POST);
	listener.support(methods::GET, GET);
	listener.support(methods::PUT, PUT);
	listener.support(methods::DEL, DELETE);

	std::thread serverThread(server, std::ref(listener));

	serverThread.join();

	return 0;
}
