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

string toString(float a) {
    ostringstream temp;
    temp << a;
    return temp.str();
}

string toString(int a) {
    ostringstream temp;
    temp << a;
    return temp.str();
}

string toString(bool a) {
    ostringstream temp;
    temp << a;
    return temp.str();
}


struct Room {
    string id;
    float oxygen_amount;
    float light;
    bool opened;
    int personal_capacity;
    float water_amount;
    string type;

    string toJson() {
        return "{ id:" + id + ", light:" + toString(light) +
               ", opened:" + toString(opened) + ", oxygen_amount:" +
                toString(oxygen_amount) + ", personal_capacity:" +
                toString(personal_capacity) + ", water_amount:" +
                toString(water_amount) + ", type:" + type + "}";
    }
};


Room *rooms;

const int N = 10;

string roomsJson(Room *rooms) {
    string result = "{ rooms: [";
    for (int i = 0; i < N; ++i) {
        result += rooms[i].toJson();
        if (i != N -1) {
            result += ",";
        }
    }
    result += "]}";
    return result;
}

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
	//cout << uri << "\n";
	//cout << "noup";
        //regex regexTemplate("\\b(\\rooms\\)([^ ]*)");
        //char*match = new char[100];
	if (strcmp(uri, "/rooms") == 0) {
            result = dbWork();
        }// else{cout << "nop";}// else if (regex_search(uri, match, regexTemplate)) {
        //    Room room = getRoomById(rooms, match);
        //    if (room != NULL) {
        //        cout << room.toJson();
        //    } else {
         //       cout << "{ status: 404}";
        //    }
        //}
	cout << "Content-type: text/html\r\n" 
	<< "\r\n" << result << "\n";
        
    }

    cin.rdbuf(cin_streambuf);
    cout.rdbuf(cout_streambuf);
    cerr.rdbuf(cerr_streambuf);

    return 0;
}
