#include <iostream>
#include <string>
#include "fcgio.h"

using namespace std;

struct Room {
	string id;
	float oxygen_amount;
	float light;
	bool opened;
	int personal_capacity;
	float water_amount;
	string type;

	string toJson() {
		return "{ id: " + id + ", light:"; 
//			", opened:" + opened + ", oxygen_amount:" + 
//			oxygen_amount + ", personal_capacity:" + 
//			personal_capacity + ", water_amount:" +
//			water_amount + ", type:" + type + "}";  	
	}	
};

int main(void) {
    streambuf * cin_streambuf  = cin.rdbuf();
    streambuf * cout_streambuf = cout.rdbuf();
    streambuf * cerr_streambuf = cerr.rdbuf();

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

	const char* uri = FCGX_GetParam("REQUEST_URI", request.envp);

        cout << "Content-type: text/html\r\n"
             << "\r\n"
             << "<html>\n"
             << "  <head>\n"
             << "    <title>Hello, World!</title>\n"
             << "  </head>\n"
             << "  <body>\n"
             << "    <h1>Hello, " << uri <<" </h1>\n"
             << "  </body>\n"
             << "</html>\n";
    }

    cin.rdbuf(cin_streambuf);
    cout.rdbuf(cout_streambuf);
    cerr.rdbuf(cerr_streambuf);

    return 0;
}
