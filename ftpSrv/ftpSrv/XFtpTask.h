#pragma once
#include <event2/bufferevent.h>

#include "XTask.h"

#include <string>
using namespace std;

struct bufferevent;

class XFtpTask : public XTask
{
public:
	string curDir = "/";
	string rootDir = "/";
	string ip = "";
	int port = 0;
	XFtpTask *cmdTask = 0;


	virtual void Parse(std::string, std::string) {}


	void ResCMD(string msg);


	void ConnectoPORT();

	void ClosePORT();


	void Send(const string& data);
	void Send(const char *data, size_t datasize);

	virtual void Event(bufferevent *, short) {}
	virtual void Read(bufferevent *) {}
	virtual void Write(bufferevent *) {}

	//设置回调函数
	void Setcb(struct bufferevent*);
	bool Init() override { return true; }

	~XFtpTask();

protected:
	static void EventCB(bufferevent *, short, void *);
	static void ReadCB(bufferevent *, void *);
	static void WriteCB(bufferevent *, void *);


	bufferevent *bev = 0;
	FILE *fp = 0;
};