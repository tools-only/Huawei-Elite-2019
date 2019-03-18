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


struct CAR{
	int id;
	int from;
	int to;
	int speed;
	int time;
};
struct ROAD{
	int  id;
	int  length;
	int  speed;
	int  channel;
	int  form;
	int  to;
	bool  isDuplex;
};
struct CROSS{
	int id;
	int nid;
	int eid;
	int sid;
	int wid;
};

std::map<int,int>  CarMap,
                  RoadMap,
				 CrossMap;
int               CarSIZE,
                 RoadSIZE,
		        CrossSIZE;
std::vector<CAR>      Car;
std::vector<ROAD>    Road;
std::vector<CROSS>  Cross;

void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c){
	std::string::size_type pos1, pos2;
	pos2 = s.find(c);
	pos1 = 0;
	while(std::string::npos != pos2){
		v.push_back(s.substr(pos1, pos2-pos1));
    	pos1 = pos2 + c.size();
    	pos2 = s.find(c, pos1);
  	}
  	if(pos1 != s.length()) 
	v.push_back(s.substr(pos1));
}

void Input(std::string Carfile,std::string Roadfile,std::string Crossfile){
	std::string tmps;
	std::ifstream IN(Carfile);

	while(!IN.eof()){
		std::getline(IN,tmps);
		if(tmps.at(0)!='#'){//[Error] ISO C++ forbids comparison between pointer and integer [-fpermissive]
			tmps=tmps.substr(1,tmps.size()-2); 
			std::vector<std::string> data;
			SplitString(tmps,data,",");
			
			CAR tmpcar={std::stoi(data[0]),
						std::stoi(data[1]),
						std::stoi(data[2]),
						std::stoi(data[3]),
						std::stoi(data[4])};
			Car.push_back(tmpcar);
		    CarMap.insert(std::pair<int,int>(tmpcar.id,CarSIZE++));
		}
	}
//	for(auto it:Car)
//		std::cout<<it.id<<" "<<it.time<<" ";
	IN.close();
	
	IN.open(Roadfile);
	while(!IN.eof()){
		std::getline(IN,tmps);
		if(tmps.at(0)!='#'){//[Error] ISO C++ forbids comparison between pointer and integer [-fpermissive]
			tmps=tmps.substr(1,tmps.size()-2); 
			std::vector<std::string> data;
			SplitString(tmps,data,",");
//			for(auto it:data)
//				std::cout<<it<<" ";
			ROAD tmproad={std::stoi(data[0]),
			              std::stoi(data[1]),
						  std::stoi(data[2]),
						  std::stoi(data[3]),
						  std::stoi(data[4]),
						  std::stoi(data[5]),
				          bool(std::stoi(data[6]))};
			Road.push_back(tmproad);
		    RoadMap.insert(std::pair<int,int>(tmproad.id,RoadSIZE++));
		}
	}
//	for(auto it:Road)
//		std::cout<<it.id<<" "<<it.to<<" ";
	IN.close();
	
	IN.open(Crossfile);
	while(!IN.eof()){
		std::getline(IN,tmps);
		if(tmps.at(0)!='#'){//[Error] ISO C++ forbids comparison between pointer and integer [-fpermissive]
			tmps=tmps.substr(1,tmps.size()-2); 
			std::vector<std::string> data;
			SplitString(tmps,data,",");
//			for(auto it:data)
//				std::cout<<it<<" ";
			CROSS tmpcross={std::stoi(data[0]),
			                std::stoi(data[1]),
						    std::stoi(data[2]),
						    std::stoi(data[3]),
						    std::stoi(data[4])};
			Cross.push_back(tmpcross);
		    CrossMap.insert(std::pair<int,int>(tmpcross.id,CrossSIZE++));
		}
	}
//	for(auto it:Cross)
//		std::cout<<it.id<<" "<<it.eid<<" ";
	IN.close();
}

void Output(std::string answerfile){
	std::ofstream OUT(answerfile);
	OUT<<"#(carId,StartTime,RoadId...)\n";
	//for(int )
	OUT.close();
}
//int main(int argc, char *argv[])
//{
//    std::cout << "Begin" << std::endl;
//	
//	if(argc < 5){
//		std::cout << "please input args: carPath, roadPath, crossPath, answerPath" << std::endl;
//		exit(1);
//	}
//	
//	std::string carPath(argv[1]);
//	std::string roadPath(argv[2]);
//	std::string crossPath(argv[3]);
//	std::string answerPath(argv[4]);
//	
//	std::cout << "carPath is " << carPath << std::endl;
//	std::cout << "roadPath is " << roadPath << std::endl;
//	std::cout << "crossPath is " << crossPath << std::endl;
//	std::cout << "answerPath is " << answerPath << std::endl;
	
//	//TODO:read input filebuf
//	// TODO:process
//  //TODO:write output file
//	
//	//readfile(carPath,roadPath,crossPath);
//	//process();
//	//output(); 

	
int main(){
	Input("car.txt","road.txt","cross.txt");
	Output("answar.txt");
	return 0;
}
