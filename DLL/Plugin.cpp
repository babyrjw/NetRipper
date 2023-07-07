
#include "stdafx.h"
#include "Plugin.h"
#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
// Initialize pointers

Plugin::Plugin()
{
	ReadCallback = NULL;
	WriteCallback = NULL;
}

// Set config data

void Plugin::SetConfig(string p_sConfig)
{ 
	m_sConfig = p_sConfig;
}

// Called by PluginSystem to process read data

PLUGIN_DATA Plugin::ProcessReadData(unsigned char *p_pcData, unsigned int p_nSize)
{
	PLUGIN_DATA ret;

	ret.data = p_pcData;
	ret.size = p_nSize;

	if(this->ReadCallback != NULL) ret = this->ReadCallback(ret.data, ret.size, this->m_sConfig);

	return ret;
}

// Called by PluginSystem to process write data

PLUGIN_DATA Plugin::ProcessWriteData(unsigned char *p_pcData, unsigned int p_nSize)
{
	PLUGIN_DATA ret;

	ret.data = p_pcData;
	ret.size = p_nSize;

	if(this->WriteCallback != NULL) ret = this->WriteCallback(ret.data, ret.size, this->m_sConfig);

	return ret;
}


// Plugin that extracts only plain text

PLUGIN_DATA Plugin_PlainText(unsigned char *p_pcData, unsigned int p_nSize, string p_sConfigData)
{
	PLUGIN_DATA ret;
	unsigned int new_size = 0;

	ret.data = new unsigned char[p_nSize];

	// Get only printable text

	for(size_t i = 0; i < p_nSize; i++)
	{
		if(p_pcData[i] >= 32 && p_pcData[i] <= 126) ret.data[new_size++] = p_pcData[i];
		if(p_pcData[i] == 0xA || p_pcData[i] == 0xD) ret.data[new_size++] = p_pcData[i];
	}

	ret.size = new_size;

	return ret;
}

// Plugin to limit data

PLUGIN_DATA Plugin_DataLimit(unsigned char *p_pcData, unsigned int p_nSize, string p_sConfigData)
{
	PLUGIN_DATA ret;
	unsigned int max_data = Utils::StringToInt(p_sConfigData);

	if(max_data == 0 || max_data > p_nSize) 
		max_data = p_nSize;

	ret.data = new unsigned char[max_data];
	ret.size = max_data;

	// Copy first max_data data

	memcpy(ret.data, p_pcData, max_data);
	
	return ret;
}

// Plugin to find specific strings

PLUGIN_DATA Plugin_StringFinder(unsigned char *p_pcData, unsigned int p_nSize, string p_sConfigData)
{
	PLUGIN_DATA ret;
	
	ret.data = p_pcData;
	ret.size = p_nSize;

	// Search strings, data will be saved in other file

	vector<string> searchStrings = Utils::SplitString(p_sConfigData, ",");
	//for(size_t x = 0; x < searchStrings.size(); x++) searchStrings[x] = searchStrings[x];

	string allStrings;
	allStrings.assign((char *)p_pcData, p_nSize);

	bool bFound = false;
	for (size_t j = 0; j < searchStrings.size(); j++)
	{
		if (allStrings.find(searchStrings[j]) != string::npos)
		{
			Utils::WriteToTempFile("StringFinder.txt", (unsigned char*)"\r\n\r\n", 4);
			Utils::WriteToTempFile("StringFinder.txt", (unsigned char*)allStrings.c_str(), p_nSize);
			Utils::WriteToTempFile("StringFinder.txt", (unsigned char*)"\r\n\r\n", 4);
			bFound = true;

			size_t start = allStrings.find("staffId=");
			if (start != string::npos) {
				size_t end = allStrings.find("&", start);
				if (end != string::npos) {
					string staffId = allStrings.substr(start + 8, end - start - 8);
					std::ifstream in(DynConfig::GetDataPath() + "cbss.txt");
					json data;
					if (in.is_open()) {
						try
						{
							data = json::parse(in);
						}
						catch (json::parse_error& ex)
						{
						}
						in.close();
					}
					if (data == nullptr) {
						data = json::parse("{}");
					}
					data[staffId] = allStrings;
					std::ofstream out(DynConfig::GetDataPath() + "cbss.txt");
					if (out.is_open()) {
						out << data;
						out.close();
					}
				}
			}
			break;
		}
	}
	
	return ret;
}
