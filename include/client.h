#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <string>

class Client
{
private:
    
public:
    std::string name;
    bool is_playing;

    Client();
    Client(std::string, bool);
    ~Client();
};

Client::Client()
{}

Client::Client(std::string _name, bool _is_playing = false) : name(_name), is_playing(_is_playing)
{}

Client::~Client()
{}

#endif //__CLIENT_H__