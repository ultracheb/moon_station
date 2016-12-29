#include <iostream>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <regex>
#include <stdlib.h>
#include <ctype.h>
#include "fcgio.h"

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

Room getRoomById(Room *rooms, string id) {
    for (int i = 0; i < N; ++i) {
        if (rooms[i] == id) {
            return rooms[i];
        }
    }
    return NULL;
}

void fillRooms() {
    for (int i = 0; i < N; ++i) {
        rooms[i].id = toString(i);
        rooms[i].oxygen_amount = (float) rand() / RAND_MAX;
        rooms[i].light = (float) rand() / RAND_MAX;
        rooms[i].water_amount = (float) rand() / RAND_MAX;
    }
}

int main(void) {
    rooms = new Room[N];
    fillRooms();

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

        regex regexTemplate("\\b(\\rooms\\)([^ ]*)");
        char*match = new char[100];

        if (uri == "/rooms") {
            cout << roomsJson(rooms);
        } else if (regex_search(uri, match, regexTemplate)) {
            Room room = getRoomById(rooms, match);
            if (room != NULL) {
                cout << room.toJson();
            } else {

            }
        }
    }

    cin.rdbuf(cin_streambuf);
    cout.rdbuf(cout_streambuf);
    cerr.rdbuf(cerr_streambuf);

    return 0;
}
