#include <iostream>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <regex>
#include <stdlib.h>
#include <ctype.h>
#include "fcgio.h"

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

using namespace std;

mongocxx::instance inst{};
mongocxx::client conn{mongocxx::uri{}};

auto collection = conn["moondb"]["rooms"];
  
string dbWork() {
   auto cursor = collection.find({});
   string result = "";
    for (auto&& doc : cursor) {
        result += bsoncxx::to_json(doc);
    } 
    return result;   
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

        const char *uri = FCGX_GetParam("REQUEST_URI", request.envp);
	string result = "empty";
        //regex regexTemplate("\\b(\\rooms\\)([^ ]*)");
        //char*match = new char[100];
	if (strcmp(uri, "/rooms") == 0) {
            result = dbWork();
        }
	// else if (regex_search(uri, match, regexTemplate)) {
        //    Room room = getRoomById(rooms, match);
        // }
	cout << "Content-type: bson\r\n" 
	<< "\r\n" << result << "\n";
    }

    cin.rdbuf(cin_streambuf);
    cout.rdbuf(cout_streambuf);
    cerr.rdbuf(cerr_streambuf);

    return 0;
}
