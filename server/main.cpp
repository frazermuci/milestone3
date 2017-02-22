#include <stdlib.h>
#include <iostream>
#include <string>
#include <sstream>
#include <time.h>
#include "websocket.h"
#include <cstring>
#include "manageconnection.h"

using namespace std;

//struct for serialization

webSocket server;

ConnectionManager cm = ConnectionManager(&server, 12, 9);//server is not initialized..well see.
int gcount = 0;


/* called when a client connects */
void openHandler(int clientID)
{
    ostringstream os;
    //bool isZero = gcount == 0;
    //os << "init"<<":"<<isZero?"2:2":"4:4";
    cout << __FUNCTION__ << ": " << "init"<<":"<< gcount << endl;
    os << "init:" << gcount;

    cm.connNumWithClientID(clientID, gcount);
    cm.send(clientID, os.str());
    /*int x,y = 4;
    if(isZero)
    {
      x = 2;
      y = 2;
    }

    cm.addSnake(clientID, x, y, Tuple(0,1));*/
    gcount = gcount == 0 ? 1 : 0;
}

/* called when a client disconnects */
void closeHandler(int clientID)
{   
  cout << __FUNCTION__ << ": " <<"end"<<":"<< endl;
  cm.removeConn(clientID);
  cm.removeSnake(clientID);
  gcount = cm.getConnNum(clientID) == 1 ? 1 : 0;
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
  cout << __FUNCTION__ << ": " << "in" << endl;
  cm.addConn(clientID, atoi(mVect.at(1).c_str()));
  if(cm.connReady())
  {
    cout << __FUNCTION__ << ": " << "connReady" << endl;
    cm.newGame();
    cm.sendIDs();//on client side, wait until "begin"
  }
}

/* called when a client sends a message to the server */
void messageHandler(int clientID, string message)
{
    cout << __FUNCTION__ << ": " <<"message : [" << message << "]" << endl;
    vector<string> mVect = parseMessage(message);
    if(isInitMessage(mVect.at(0)))
    {
      //parse message and get id
      cout << __FUNCTION__ << ": " << "clientID: " << clientID << " otherID: " << mVect.at(1) << endl;
      initializeConnection(clientID, mVect);
      return;
    }
    if(cm.connReady())
    {
      //update model from message
      cout << __FUNCTION__ << ": " << "deserialize (uchar): " << (unsigned char)message[0] << endl;
           //GT todo deserialize return
      cm.updateModel(clientID, cm.deserialize((unsigned char*)message.c_str()));
    }
    if(cm.stateReady(clientID))
    {			
      //serializing new state
      Compressed* c = static_cast<Compressed*>(malloc(sizeof(struct Compressed)));
      
      cm.moveModel(c);
      ostringstream os;
      os << + cm.serialize(c);
      cout << __FUNCTION__ << ": " << "serialize : " << os.str() << endl;
      cm.sendAll(os.str());
      os.str("");
      
      free(c);
    }
}

/* called once per select() loop 
void periodicHandler(){
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
}
*/
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
