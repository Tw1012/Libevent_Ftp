#include "XFtpLIST.h"
#include "event2/bufferevent.h"
#include "event2/event.h"
#include "testUtil.h"
#include <string>
#include <sys/stat.h>
using namespace std;


void XFtpLIST::Write(bufferevent *bev) {
	testout("XFtpLIST::Write");
	ResCMD("226 Transfer comlete");
	ClosePORT();
}

void XFtpLIST::Event(bufferevent *bev, short events) {
	testout("At XFtpLIST::Event");
	if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR | BEV_EVENT_TIMEOUT)) {
		ClosePORT();
	}
	else if (events & BEV_EVENT_CONNECTED)
		cout << "XFtpLIST BEV_EVENT_CONNECTED" << endl;
}

void XFtpLIST::Parse(std::string type, std::string msg) {
	testout("At XFtpLIST::Parse");
	string resmsg = "";
	if (type == "PWD") {
		// 257 "/" is current directory.
		resmsg = "257 \"";
		resmsg += cmdTask->curDir;
		resmsg += "\" is current dir.";
		ResCMD(resmsg);
	}
	else if (type == "LIST") {
		//  "-rwxrwxrwx 1 root root      418 Mar 21 16:10 XFtpFactory.cpp";
		string path = cmdTask->rootDir + cmdTask->curDir;
		testout("listpath: " << path);
		string listdata = GetListData(path);
		ConnectoPORT();
		ResCMD("150 Here coms the directory listing.");
		Send(listdata);
	}
	else if (type == "CWD") 
	{
		//CWD test\r\n
		int pos = msg.rfind(" ") + 1;
		string path = msg.substr(pos, msg.size() - pos - 2);
		string curDir = cmdTask->curDir;
		if (path[0] == '/') 
		{
			curDir = path;
		}
		else
		{
			if (curDir[curDir.size() - 1] != '/')
				curDir += "/";
			curDir += path + "/";
		}
		if (curDir[curDir.size() - 1] != '/')
			curDir += "/";
		struct stat s_buf;
		stat(curDir.c_str(),&s_buf);
		if(S_ISDIR(s_buf.st_mode))
		{
			cmdTask->curDir = curDir;
			ResCMD("250 Directory succes chanaged.\r\n");
		}
		else
		{
			ResCMD("501 Directory not chanaged.\r\n");
		}
		//cmdTask->curDir = curDir;
		//  /test/
		//ResCMD("250 Directory succes chanaged.\r\n");

		//cmdTask->curDir += 
	}
	else if (type == "CDUP")
	{
		cout << "msg:" << msg << endl;
		cout << "cmdTask->curDir:" << cmdTask->curDir << endl;
		// if (msg[4] == '\r') {
		// 	cmdTask->curDir = "/";
		// }
		// else {
		// 	//  /Debug/test_ser.A3C61E95.tlog /Debug   /Debug/
		// 	string path = cmdTask->curDir;
		// 	
		// 	////  /Debug/test_ser.A3C61E95.tlog /Debug  
			
		// 	if (path[path.size() - 1] == '/')
		// 	{
		// 		path = path.substr(0, path.size() - 1);
		// 	}
		// 	int pos = path.rfind("/");
		// 	path = path.substr(0, pos);
		// 	cmdTask->curDir = path;
		// 	if (cmdTask->curDir[cmdTask->curDir.size() - 1] != '/')
		// 		cmdTask->curDir += "/";
		// }
		//  /Debug/test_ser.A3C61E95.tlog /Debug   /Debug/
			string path = cmdTask->curDir;
			
			////  /Debug/test_ser.A3C61E95.tlog /Debug  
			
		if (path[path.size() - 1] == '/')
		{
			path = path.substr(0, path.size() - 1);
		}
		int pos = path.rfind("/");
		path = path.substr(0, pos);
		cmdTask->curDir = path;
		if (cmdTask->curDir[cmdTask->curDir.size() - 1] != '/')
			cmdTask->curDir += "/";
		cout << "cmdTask->curDir:" << cmdTask->curDir << endl;
		ResCMD("250 Directory succes chanaged.\r\n");
	}
}

string XFtpLIST::GetListData(string path) {
	// -rwxrwxrwx 1 root root 418 Mar 21 16:10 XFtpFactory.cpp

	string data = "";
	string cmd = "ls -l ";
	cmd += path;
	cout << cmd << endl;
	FILE *f = popen(cmd.c_str(), "r");
	if (!f) return data;
	char buf[1024] = {0};
	while (1) {
		int len = fread(buf, 1, sizeof(buf) - 1, f);
		if (len <= 0)break;
		buf[len] = '\0';
		data += buf;
	}
	pclose(f);
 
	return data;
}