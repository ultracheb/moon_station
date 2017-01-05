#include <iostream>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <regex>
#include <stdlib.h>
#include <ctype.h>
#include "fcgio.h"
#include "json.hpp"

#include <cstdint>
#include <vector>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/types.hpp>


using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;
using nlohmann::json;
using namespace std;

mongocxx::instance inst{};
mongocxx::client conn{mongocxx::uri{}};

auto collection = conn["moondatabase"]["rooms"];
auto desiredCollection = conn["moondatabase"]["desired_rooms"];
  
string getAllRooms() {
   auto cursor = collection.find({});
   string result = "";
    for (auto&& doc : cursor) {
        result += bsoncxx::to_json(doc);
    } 
    return result;
}

string getRoomById(char * id) {
    string result = "404 Not found";
    mongocxx::stdx::optional<bsoncxx::document::value> maybe_result =
    collection.find_one(document{}<< "id"<<atoi(id)<< finalize);
    result += id;    
    if(maybe_result) {
        result = bsoncxx::to_json(*maybe_result);
    }
    return result;
}

string updateDesired(char* id, string desiredRoom) {
    auto j1 = json::parse(desiredRoom);    
    bsoncxx::builder::stream::document filterBuilder, replaceBuilder;
    filterBuilder << "id" << atoi(id);
    string key;
    string value;
    for (nlohmann::json::iterator it = j1.begin(); it != j1.end(); ++it)  {
	key = it.key();
	value = it.value();    
	replaceBuilder <<"$set"<<open_document<< key << value << close_document;
    }
    desiredCollection.replace_one(filterBuilder.view(), replaceBuilder.view());
    return "200";
}

string updateRoom(char* id, string room) {
    auto j1 = json::parse(room);    
    bsoncxx::builder::stream::document filterBuilder, replaceBuilder;
    filterBuilder << "id" << atoi(id);
    string key;
    string value;
    for (nlohmann::json::iterator it = j1.begin(); it != j1.end(); ++it)  {
	key = it.key();
	value = it.value();    
	replaceBuilder <<"$set"<<open_document<< key << value << close_document;
    }
    collection.update_one(filterBuilder.view(), replaceBuilder.view());
    return "200";
}


string get_request_content(const FCGX_Request & request) {
    char * content_length_str = FCGX_GetParam("CONTENT_LENGTH",
                                               request.envp);
    int MAX_LENGTH = 256;
    unsigned long content_length = MAX_LENGTH;

    if (content_length_str) {
        content_length = strtol(content_length_str,
                               &content_length_str,
                                10);

        if (*content_length_str) {
            cerr << "Can't Parse 'CONTENT_LENGTH='"
                 << FCGX_GetParam("CONTENT_LENGTH", request.envp)
                 << "'. Consuming stdin up to " << MAX_LENGTH << endl;
        }
        if (content_length > MAX_LENGTH) {
            content_length = MAX_LENGTH;
        }
    } else {
        content_length = 0;
    }
    char* content_buffer = new char[content_length];
    cin.read(content_buffer, content_length);
    content_length = cin.gcount();
    do cin.ignore(1024); while (cin.gcount() == 1024); 
    string content(content_buffer, content_length);
    delete [] content_buffer;
    return content;
}

int main(void) {
    streambuf *cin_streambuf = cin.rdbuf();
    streambuf *cout_streambuf = cout.rdbuf();
    streambuf *cerr_streambuf = cerr.rdbuf();

    FCGX_Request request;

    FCGX_Init();
    FCGX_InitRequest(&request, 0, 0);

    while (FCGX_Accept_r(&request) == 0) {
        fcgi_streambuf cin_fcgi_streambuf(request.in);
        fcgi_streambuf cout_fcgi_streambuf(request.out);
        fcgi_streambuf cerr_fcgi_streambuf(request.err);

        cin.rdbuf(&cin_fcgi_streambuf);
        cout.rdbuf(&cout_fcgi_streambuf);
        cerr.rdbuf(&cerr_fcgi_streambuf);

        char *uri = FCGX_GetParam("REQUEST_URI", request.envp);
	string result = "empty";
        regex regexTemplate("\\b(rooms/)([^ ]*)");
        regex desiredRoomTemplate("\\b(rooms/desired/)([^ ]*)");
	smatch match;
	string content = get_request_content(request);
	//result += content;
	if (strcmp(uri, "/rooms") == 0) {
            result = getAllRooms();
        } else if (regex_search((string)(uri + 1), match, regexTemplate)) {
            if (content.length() == 0) {
		result = getRoomById(uri + 7);
	    } else {
		result = updateRoom(uri + 7, content);
	    }
        } else if (regex_search((string)(uri + 1), match,desiredRoomTemplate)) {
            if (content.length() != 0) {
		updateDesired(uri + 15, content);
	    }           
	}
	cout << "Content-type: bson\r\n" << "\r\n" << result << "\n";
    }

    cin.rdbuf(cin_streambuf);
    cout.rdbuf(cout_streambuf);
    cerr.rdbuf(cerr_streambuf);

    return 0;
}
