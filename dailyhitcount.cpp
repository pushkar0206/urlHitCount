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

struct myclass {
	bool operator()(const pair<uint32_t, uintptr_t >  & p1, 
	const pair<uint32_t, uintptr_t > & p2) const
	{
		return p1.first < p2.first;
	}
} customsort;



uint32_t getCurrentDateInInt(uint32_t epoch)
{
	char buffer[50];
	time_t e = (time_t)epoch;
	struct tm* s = gmtime(&e);
	strftime(buffer, 50, "%m:%d:%Y", s);
	struct tm r = {0};
	strptime(buffer, "%m:%d:%Y", &r);
	time_t t = mktime(&r);
	return (uint32_t)t;
}

string getCurrentDate(uint32_t epoch)
{
	char buffer[50];
	time_t e = (time_t)epoch;
	struct tm* s = gmtime(&e);
	strftime(buffer, 50, "%m/%d/%Y", s);
	return string(buffer);
}


int ReadFileData(string fileName, map<uint32_t, map<string, int> > &fileData)
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
		uint32_t index = getCurrentDateInInt(ts);
		if (fileData.find(index) == fileData.end())
		{
			map<string, int> m;
			m[url]++;
			fileData[index] = m;
		}
		else
		{
			fileData[index][url]++;
		}
		count++;
	}
	//printf("Read %d entries\n", count);
	file.close();
	res = count;
	return res;
}

void ProcessDailyData(map<uint32_t, map<string, int> >& fileData,
					  vector<pair<uint32_t, uintptr_t > >& sortedList)
{

	for(map<uint32_t, map<string, int> >::iterator itr = fileData.begin(); itr != fileData.end(); itr++)
	{
		pair<uint32_t, uintptr_t> p;
		p.first = itr->first;
		priority_queue<pair<int, string>, vector<pair<int, string> >, mypqclass> *pq = new 
			priority_queue<pair<int, string>, vector<pair<int, string> >, mypqclass>;
		for(map<string, int>::iterator itr2 = itr->second.begin(); itr2 != itr->second.end(); itr2++)
		{
			pq->push(make_pair(itr2->second, itr2->first));
		}
		p.second = reinterpret_cast<uintptr_t>(pq);
		sortedList.push_back(p);
	}

	sort(sortedList.begin(), sortedList.end(), customsort);
	
	for(int i=0;i<sortedList.size();i++)
	{
		pair<uint32_t, uintptr_t> p2 = sortedList[i];
		string date =  getCurrentDate(p2.first);
		date += " GMT";
		printf("%s\n", date.c_str());
		priority_queue<pair<int, string>, vector<pair<int, string> >, mypqclass> *pq = 
			reinterpret_cast<priority_queue<pair<int, string>, vector<pair<int, string> >, mypqclass>*>(p2.second);
		while(pq->size() > 0)
		{
			pair<int, string> p3 = pq->top();
			pq->pop();
			printf("%s %d\n", p3.second.c_str(), p3.first);
		}
		delete pq;
	}
}

int main(int argc, char **argv)
{
	map<uint32_t, map<string, int> > fileData;
	vector<pair<uint32_t, uintptr_t > > sortedList;
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

	ProcessDailyData(fileData, sortedList);
	return 0;
}
