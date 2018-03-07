#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <algorithm>
#include <sstream>
#include <stdlib.h>
#include <errno.h>

using namespace std;

struct myclass {
	bool operator()(const pair<uint32_t, string>& p1, const pair<uint32_t, string>& p2) const
    {
     	return p1.first < p2.first;
    }
} customsort;

struct mypqclass {
	bool operator()(const pair<int, string>& p1, const pair<int, string>& p2) const
	{
		if (p1.first != p2.first)
		{
			return p1.first < p2.first;
		}
		
		return p1.second > p2.second;
	}
	
};

int ReadFileData(string fileName, vector<pair<uint32_t, string> > &fileData)
{
	int res = -1;
	string data;
	ifstream file(fileName);
	if (file.is_open() == false)
	{
		printf("Unable to open file %s, 'error %s'\n", fileName.c_str(), strerror(errno));
		return res;
	}
	int count = 0;
	while(getline(file, data))
	{
		string str_ts;
		uint32_t ts;
		string url;
		istringstream iss(data);
		getline(iss, str_ts, '|');
		if (str_ts.length() == 0)
		{
			printf("Unable to get timestamp\n");
			continue;
		}
		ts = stoi(str_ts);
		getline(iss, url, '|');
		if (url.length() == 0)
		{
			printf("Unable to get url\n");
			continue;
		}
		fileData.push_back(make_pair(ts, url));
		count++;
	}
	//printf("Read %d entries\n", count);
	res = count;
	return res;
}

string getCurrentDate(uint32_t epoch)
{
	char buffer[50];
	time_t e = (time_t)epoch;
	struct tm* s = gmtime(&e);
	strftime(buffer, 50, "%m/%d/%Y", s);
	return string(buffer);
}

void processDailyData(string date, map<string, int> & m)
{
	priority_queue<pair<int, string>, vector<pair<int, string> >, mypqclass> pq;
	if (m.size() == 0)
		return;
	for (map<string, int>::iterator itr = m.begin(); itr != m.end();itr++)
	{
		pq.push(make_pair(itr->second, itr->first));
	}
	date += " GMT";
	printf("%s\n", date.c_str());
	while(pq.size() > 0)
	{
		pair<int, string> p = pq.top();
		pq.pop();
		printf("%s %d\n", p.second.c_str(), p.first);
	}
}

int main(int argc, char **argv)
{
	vector<pair<uint32_t, string> > fileData;
	//vector of <timestamp, and url>
	map<string, int> m;
	//key = url, value = hit count
	if (argc != 2)
	{
		printf("Format is '%s <input file name>'\n", argv[0]);
		return 0;
	}
	
	string fileName(argv[1]);
	if (fileName.length() == 0)
	{
		printf("Error in opening file\n");
		return 0;
	}
	
	if (ReadFileData(fileName, fileData) < 0)
	{
		printf("Error in Reading data from input file\n");
		return 0;
	}
	if (fileData.size() == 0)
	{
		printf("No entries read\n");
		return 0;
	}
	
	sort(fileData.begin(), fileData.end(), customsort);
	
	string currTime = getCurrentDate(fileData[0].first);
	m[fileData[0].second]++;
	for(int i=1;i<fileData.size();i++)
	{
		string nextTime = getCurrentDate(fileData[i].first);
		if (nextTime != currTime)
		{
			processDailyData(currTime, m);
			m.clear();
			currTime = nextTime;
		}
	
		m[fileData[i].second]++;	
	}
	if (m.size() != 0)
	{
		processDailyData(currTime, m);
	}
	
	return 0;
}


