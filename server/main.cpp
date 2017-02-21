#include <stdlib.h>
#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <time.h>
#include "manageconnection.h"
#include "websocket.h"

using namespace std;

//struct for serialization

webSocket server;
ConnectionManager cm = ConnectionManager(&server, 12, 9);//server is not initialized..well see.
int count = 0;
time_t  timev;
//time(&timev);

/* called when a client connects */
void openHandler(int clientID)
{
	time(&timev);
	time_t temp = timev;
	ostringstream os;
	//bool isZero = count == 0;
	//os << "init"<<":"<<isZero?"2:2":"4:4";
	
	cm.connNumWithClientID(clientID, count);
	time(&timev);
	os << "init:" << count << ":" <<(timev-temp);
	cm.send(clientID, os.str());
	/*int x,y = 4;
	if(isZero)
	{
		x = 2;
		y = 2;
	}
	cm.addSnake(clientID, x, y, Tuple(0,1));*/
	count = count == 0 ? 1 : 0;
}

/* called when a client disconnects */
void closeHandler(int clientID)
{   
	cm.removeConn(clientID);
	cm.removeSnake(clientID);
	count = cm.getConnNum(clientID) == 1 ? 1 : 0;
}

vector<string> parseMessage(string message)
{
	vector<string> mVect = vector<string>();
	ostringstream in;
	string::iterator it;
	for(it = message.begin(); it != message.end(); ++it)
	{
		if((*it) != ':')
		{
			in << (*it);
		}
		else
		{
			mVect.push_back(in.str());
			in.str("");
		}
	}
	mVect.push_back(in.str());
	return mVect;
}

bool isInitMessage(string str)
{
	return strcmp(str.c_str(), "init") == 0;
}

void initializeConnection(int clientID, vector<string> mVect)
{
	cm.addConn(clientID, atoi(mVect.at(1).c_str()));
	//if(cm.connReady())
	//{
		cm.newGame();
		cm.sendIDs();//on client side, wait until "begin"
	//}
}

string handleBinaryConversion(int i)
{
	ostringstream os;
	int posVal = 128;
	//big endian string repr of bits
	while(posVal >= 1)
	{
		if(i >= posVal)
		{
			os << "1";
			i = i-posVal;
		}
		else
		{
			os << "0";
		}
		posVal /=2;
	}
	return os.str();
}

/* called when a client sends a message to the server */
void messageHandler(int clientID, string message)
{
	time(&timev);
	time_t temp = timev;
	cout << message << endl;
	vector<string> mVect = parseMessage(message);
	if(isInitMessage(mVect.at(0)))
	{
		//parse message and get id
		cout << "clientID: " << clientID << " otherID: " << mVect.at(1) << endl;
		initializeConnection(clientID, mVect);
		return;
	}
	//if(cm.connReady())
	//{
		//update model from message
		cout << "desrialize" << endl;
	       
		cm.updateModel(clientID, cm.deserialize((unsigned char*)message.c_str()));
	//}
	//if(cm.stateReady(clientID))
	//{		
		//serializing new state
		Compressed* c = static_cast<Compressed*>(malloc(sizeof(struct Compressed)));
		
		cm.moveModel(c);
		ostringstream os;
		time(&timev);		
		
		os << handleBinaryConversion(cm.serialize(c)[0]) << ":" <<(timev-temp);
		cout << os.str() << endl;
		cm.sendAll(os.str().c_str());
		os.str("");
		cout << "sendAll\n";
			
		free(c);
	//}
}

/* called orrnce per select() loop */
/*void periodicHandler(){
    static time_t next = time(NULL) + 10;
    time_t current = time(NULL);
    if (current >= next){
        ostringstream os;
        string timestring = ctime(&current);
        timestring = timestring.substr(0, timestring.size() - 1);
        os << timestring;

        vector<int> clientIDs = server.getClientIDs();
        for (int i = 0; i < clientIDs.size(); i++)
            server.wsSend(clientIDs[i], os.str());

        next = time(NULL) + 10;
    }
}*/

int main(int argc, char *argv[]){
    int port  = 21234;

    //cout << "Please set server port: ";
    //cin >> port;

    /* set event handler */
    server.setOpenHandler(openHandler);
    server.setCloseHandler(closeHandler);
    server.setMessageHandler(messageHandler);
    //server.setPeriodicHandler(periodicHandler);

    /* start the chatroom server, listen to ip '127.0.0.1' and port '8000' */
    server.startServer(port);

    return 1;
}
