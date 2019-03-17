#include <algorithm>
#include <iterator>
#include <memory.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <cstdio>
#include <cfloat>
#include <string>
#include <vector>
#include <cmath>
#include <queue>
#include <ctime>
#include <set>
#include <map>
using namespace std; 

struct Road
{
	int id;
	int length;
	int speed;
	int channel;
	int form;
	int to;
	bool isDuplex;
}

struct Car
{
	int id;
	int from;
	int to;
	int speed;
	int time;
}

struct Cross
{
	int id;
	int nid;
	int eid;
	int sid;
	int wid;
}

int main(int argc, char *argv[])
{
    std::cout << "Begin" << std::endl;
	
	if(argc < 5){
		std::cout << "please input args: carPath, roadPath, crossPath, answerPath" << std::endl;
		exit(1);
	}
	
	std::string carPath(argv[1]);
	std::string roadPath(argv[2]);
	std::string crossPath(argv[3]);
	std::string answerPath(argv[4]);
	
	std::cout << "carPath is " << carPath << std::endl;
	std::cout << "roadPath is " << roadPath << std::endl;
	std::cout << "crossPath is " << crossPath << std::endl;
	std::cout << "answerPath is " << answerPath << std::endl;
	
	// TODO:read input filebuf
	// TODO:process
	// TODO:write output file
	
	return 0;
}
