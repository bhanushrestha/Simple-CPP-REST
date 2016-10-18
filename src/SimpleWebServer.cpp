#include "../headers/server_http.hpp"
#include "../headers/client_http.hpp"
#include "../headers/Producer.h"
#include "../headers/Consumer.h"
#include "../headers/Configuration.h"
#include "../headers/Utils.h"

//Added for the json-example
#define BOOST_SPIRIT_THREADSAFE
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "librdkafka/rdkafkacpp.h"
#include "../libraries/Random.h"
//Added for the default_resource example
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>
#include <vector>
#include <algorithm>

using namespace std;
using namespace boost::property_tree; //Added for the json-example:

typedef SimpleWeb::Server<SimpleWeb::HTTP> HttpServer;
typedef SimpleWeb::Client<SimpleWeb::HTTP> HttpClient;

void generate_POST_message(shared_ptr<HttpServer::Request> request, string requestid);
void generate_GET_message(shared_ptr<HttpServer::Request> request, string requestid);
void produce_message(string oss);

//Added for the default_resource example
void default_resource_send(const HttpServer &server,
		shared_ptr<HttpServer::Response> response, shared_ptr<ifstream> ifs,
		shared_ptr<vector<char> > buffer);
Configuration configuration;

int main() {
	static string requestid = "";
	const char * env_p = std::getenv("CONFIG"); // get the value of environment variable
	cout << "program init :: " << env_p << endl;
	configuration.readEnv(env_p);


	cout << "KAFKAIP " << configuration.message_broker << endl;
	cout << "jtod " << configuration.jtodtopic << endl;
	cout << "dtoj " << configuration.dtojtopic << endl;


	//HTTP-server at port 8080 using 1 thread
	//Unless you do more heavy non-threaded processing in the resources,
	//1 thread is usually faster than several threads
	HttpServer server(8080, 1);

	//GET request to get all content of table given in api
	server.resource["^/v1/([a-zA-Z0-9_]+)$"]["GET"] =
			[&server](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
				//  string tablename=request->path_match[1];
		try{
			Utils* utils = new Utils();
			requestid="";
			requestid=utils->GenerateRandomNumber(5);
				generate_GET_message(request,requestid);
				thread work_thread([response] {
												//this_thread::sleep_for(chrono::seconds(5));
												Consumer* consumer = new Consumer();
												string message = consumer->consume(configuration.message_broker,configuration.jtodtopic,requestid);
												cout << "message recieved " << message << endl;
												*response << "HTTP/1.1 200 OK\r\nContent-Length: " << message.length() << "\r\n\r\n" << message;
											});
									work_thread.detach();
								}
								catch(exception& e) {
									*response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n" << e.what();
								}
			};

/*
	server.resource["^/v1/([a-zA-Z0-9_]+)/([a-zA-Z0-9_]+)$"]["GET"] =
			[&server](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
				//  string tablename=request->path_match[1];
				string tableName=request->path_match[1];
				string column=request->path_match[2];
				*response << "HTTP/1.1 200 OK\r\nContent-Length: " << column.length() << "\r\n\r\n" << column;
			};
*/


	//POST-example for the path /v1/addvalues
	//Responds with an appropriate error message if the posted json is not valid, or if firstName or lastName is missing
	//Example posted json:
	//{
	//  "id": "1",
	//  "name": "Smith",
	//  "tablename": "employees"
	//}
	server.resource["^/v1/addvalues"]["POST"] =
			[](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
				try {
					//****Producer code ends*****//
					Utils* utils = new Utils();
					requestid="";
					requestid=utils->GenerateRandomNumber(5);
					generate_POST_message(request,requestid);// produce message sent in API body
					/**Consumer code here**/
					thread work_thread([response] {
								//this_thread::sleep_for(chrono::seconds(5));
								Consumer* consumer = new Consumer();
								string message = consumer->consume(configuration.message_broker,configuration.jtodtopic,requestid);
								cout << "message recieved " << message << endl;
								*response << "HTTP/1.1 200 OK\r\nContent-Length: " << message.length() << "\r\n\r\n" << message;
							});
					work_thread.detach();

				}
				catch(exception& e) {
					*response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n" << e.what();
				}
			};

 //Default GET-example. If no other matches, this anonymous function will be called.
 //Will respond with content in the web/-directory, and its subdirectories.
 //Default file: index.html
 //Can for instance be used to retrieve an HTML 5 client that uses REST-resources on this server

server.default_resource["GET"] =
[&server](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
const auto web_root_path=boost::filesystem::canonical("web");
boost::filesystem::path path=web_root_path;
path/=request->path;
if(boost::filesystem::exists(path)) {
	path=boost::filesystem::canonical(path);
	//Check if path is within web_root_path
	if(distance(web_root_path.begin(), web_root_path.end())<=distance(path.begin(), path.end()) &&
			equal(web_root_path.begin(), web_root_path.end(), path.begin())) {
		if(boost::filesystem::is_directory(path))
		path/="index.html";
		if(boost::filesystem::exists(path) && boost::filesystem::is_regular_file(path)) {
			auto ifs=make_shared<ifstream>();
			ifs->open(path.string(), ifstream::in | ios::binary);

			if(*ifs) {
				//read and send 128 KB at a time
				streamsize buffer_size=131072;
				auto buffer=make_shared<vector<char> >(buffer_size);

				ifs->seekg(0, ios::end);
				auto length=ifs->tellg();

				ifs->seekg(0, ios::beg);

				*response << "HTTP/1.1 200 OK\r\nContent-Length: " << length << "\r\n\r\n";
				default_resource_send(server, response, ifs, buffer);
				return;
			}
		}
	}
}
string content="Could not open path "+request->path;
*response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << content.length() << "\r\n\r\n" << content;
};

thread server_thread([&server]() {
//Start server
	server.start();
});

server_thread.join();

return 0;
}

void default_resource_send(const HttpServer &server,
shared_ptr<HttpServer::Response> response, shared_ptr<ifstream> ifs,
shared_ptr<vector<char> > buffer) {
streamsize read_length;
if ((read_length = ifs->read(&(*buffer)[0], buffer->size()).gcount()) > 0) {
response->write(&(*buffer)[0], read_length);
if (read_length == static_cast<streamsize>(buffer->size())) {
	server.send(response,
			[&server, response, ifs, buffer](const boost::system::error_code &ec) {
				if(!ec)
				default_resource_send(server, response, ifs, buffer);
				else
				cerr << "Connection interrupted" << endl;
			});
}
}
}

void produce_message( string oss){
	try {
						//Producer code starts
						RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
						RdKafka::Conf *tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);
						string errstr;
						//string brokers = "172.16.10.14";
						string brokers = configuration.message_broker;
						int32_t partition = 0;
						/*
						 * Set configuration properties
						 */
						conf->set("metadata.broker.list", brokers, errstr);
						conf->set("group.id","1587",errstr);
						tconf->set("auto.commit.enable","true",errstr);
						tconf->set("auto.commit.interval.ms", "1000", errstr);
						tconf->set("auto.offset.reset","largest",errstr);
						tconf->set("offsets.storage","kafka",errstr);
						Producer* producer = new Producer();
						producer->produce(configuration.dtojtopic,conf,tconf,errstr,partition,oss);
					//	producer->produce("dmztojcore",conf,tconf,errstr,partition,oss);
}catch(exception& e) {
	throw e;
}
}

void generate_POST_message(shared_ptr<HttpServer::Request> request, string requestid){
	ptree pt;
	ptree request_pt;
	read_json(request->content, request_pt); // request->content => std::istream
	pt.put("table", request_pt.get<string>("tablename"));
	request_pt.erase("tablename");
	pt.add_child("body", request_pt);
	pt.put("requestid", requestid); //TODO generate random string as requestID
	pt.put("requestmethod", "POST");
	pt.put("sender", "DMZ");
	std::ostringstream oss;
	write_json(oss, pt);
	pt.clear();
	request_pt.clear();
	produce_message(oss.str());
}
void generate_GET_message(shared_ptr<HttpServer::Request> request, string requestid){
	ptree pt;
	ptree body_pt;
	body_pt.put("columns","");//blank for getAll, otherwise columns are to be listed
	pt.add_child("body",body_pt);
	pt.put("table",request->path_match[1]);
	pt.put("requestid",requestid ); //TODO generate random string as requestID
	pt.put("requestmethod", "GET");
	pt.put("sender", "DMZ");
	std::ostringstream oss;
	write_json(oss, pt);
	pt.clear();
	body_pt.clear();
	produce_message(oss.str());
}
