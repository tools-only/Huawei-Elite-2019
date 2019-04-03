//Without TimeSlice.Find the Road and Quit.Not Djikstra.
#include <algorithm>
#include <iterator>
#include <memory.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <cstdio>
#include <cfloat>
//#include "RNG.h" 
#include <string>
#include <vector>
#include <cmath>
#include <queue>
#include <ctime>
#include <stack>
#include <set>
#include <map>
const int INF=0x3f3f3f3f; 
struct CAR{
	int id;
	int from;
	int to;
	int speed;
	int time;
	int timeInMap;
};
struct ROAD{
	int   id;
	int   length;
	int   speed;
	int   channel;
	int   from;
	int   to;
	bool  isDuplex;
	int   time;
	int   volume[2];
};
struct CROSS{
	int id;
	int roadid[4];
	int volume;
};
struct GRAPHUNIT{
	int       roadid=-1;
	int    distance=INF;
	int   orientation=0;
	int         value=0;
};
struct PLAN{
	int id;
	int starttime;
	std::vector<int> roadid;
};

int                          SysTime=0;
int                         MapCapcity; 
const double        DjikstraFactor=0.5;
const double         CarTimeFactor=1.1;

std::map<int,int>               CarMap;
std::map<int,int>              RoadMap;
std::map<int,int>             CrossMap;
				              
int                            CarSize;//PlanSize;
int                           RoadSize;//
int                          CrossSize;//GraphSize

std::vector<CAR>                         Car;
std::vector<ROAD>                       Road;
std::vector<CROSS>                     Cross;
std::vector<PLAN>                       Plan;
std::vector<std::vector<GRAPHUNIT> >   Graph;

std::vector<std::vector<std::vector<GRAPHUNIT> > > TimeTable;
//Tool for input string.
void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c){
	std::string::size_type pos1, pos2;
	pos2 = s.find(c);pos1 = 0;
	while(std::string::npos != pos2){
		v.push_back(s.substr(pos1, pos2-pos1));
    	pos1 = pos2 + c.size();pos2 = s.find(c, pos1);
  	}
  	if(pos1 != s.length()) v.push_back(s.substr(pos1));
}

int log2(int _val)
{
	int ret;
	float fdata = (float)_val;
	//unsigned int uData = (fdata>>23)&0xFF;		//直接移动出错
	unsigned int data  = (unsigned int&)fdata;//(unsigned int&)fdata 与*(unsigned int*)&fdata; 一致
	unsigned int udata = (data>>23)&0xFF;
	ret = (int)udata -127;//-无符号到有符号
	
	return ret;
}

void Input(const std::string &Carfile,const std::string &Roadfile,const std::string &Crossfile){
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
						std::stoi(data[4]),0};
			Car.push_back(tmpcar);
			CarMap.insert(std::pair<int,int>(tmpcar.id,CarSize++));
		}
	}
	Plan.resize(CarSize);
	IN.close();
	
	IN.open(Roadfile);
	while(!IN.eof()){
		std::getline(IN,tmps);
		if(tmps.at(0)!='#'){//[Error] ISO C++ forbids comparison between pointer and integer [-fpermissive]
			tmps=tmps.substr(1,tmps.size()-2); 
			std::vector<std::string> data;
			SplitString(tmps,data,",");
			ROAD tmproad={std::stoi(data[0]),
			              std::stoi(data[1]),
						  std::stoi(data[2]),
						  std::stoi(data[3]),
						  std::stoi(data[4]),
						  std::stoi(data[5]),
				          bool(std::stoi(data[6])),0,{0,0}};
			Road.push_back(tmproad);
		    RoadMap.insert(std::pair<int,int>(tmproad.id,RoadSize++));
		}
	}
	IN.close();
	 
	IN.open(Crossfile);
	while(!IN.eof()){
		std::getline(IN,tmps);
		if(tmps.at(0)!='#'){//[Error] ISO C++ forbids comparison between pointer and integer [-fpermissive]
			tmps=tmps.substr(1,tmps.size()-2); 
			std::vector<std::string> data;
			SplitString(tmps,data,",");
			CROSS tmpcross={std::stoi(data[0]),
						   {RoadMap[std::stoi(data[1])],
						    RoadMap[std::stoi(data[2])],
						    RoadMap[std::stoi(data[3])],
						    RoadMap[std::stoi(data[4])]},1};
			Cross.push_back(tmpcross);
		    CrossMap.insert(std::pair<int,int>(tmpcross.id,CrossSize++));
		}
	}
	IN.close();
		
	Graph.resize(CrossSize,std::vector<GRAPHUNIT>(CrossSize));
	for(int i=0;i<RoadSize;i++){
		Road[i].from=CrossMap[Road[i].from];
		Road[i].to=CrossMap[Road[i].to];
		Graph[Road[i].from][Road[i].to].roadid=i;
		Graph[Road[i].from][Road[i].to].distance=Road[i].length;
		if(Road[i].isDuplex){
			Graph[Road[i].to][Road[i].from].roadid=i;
			Graph[Road[i].to][Road[i].from].distance=Road[i].length;
			Graph[Road[i].to][Road[i].from].orientation=1;
		}
	}
	
	for(int i=0;i<CarSize;i++){
		Car[i].from=CrossMap[Car[i].from];
		Car[i].to=CrossMap[Car[i].to];
		Car[i].timeInMap=Graph[Car[i].from][Car[i].to].distance/Car[i].speed;
	}
}

void Output(std::string answerfile){
	std::ofstream OUT(answerfile);
	OUT<<"#(carId,StartTime,RoadId...)\n";
	for(int i=0;i<CarSize;i++){
		OUT<<"("<<Plan[i].id<<","<<Plan[i].starttime;
		for(auto it=Plan[i].roadid.begin();it!=Plan[i].roadid.end();it++){
			OUT<<","<<Road[(*it)].id;//vector<int> rns[-1]
			//OUT<<","<<*it_;		
		}
		OUT<<")\n";
	}
	OUT.close();
}

bool cmpCar(CAR &a,CAR &b){
//	return (a.time==b.time)?((a.timeInMap==b.timeInMap)?a.timeInMap<b.timeInMap:a.id<b.id):a.time<b.time;
	return (a.time==b.time)?a.id<b.id:a.time<b.time;
}

void Floyed(std::vector<std::vector<GRAPHUNIT> > &graph){
	for(int k=0;k<CrossSize;k++){
		for(int i=0;i<CrossSize;i++){
			for(int j=0;j<CrossSize;j++)
				graph[i][j].distance=std::min(graph[i][j].distance,graph[i][k].distance+graph[k][j].distance);
		}
	}
}

void PlanWithDjikstra(CAR &car,PLAN &plan){
	std::vector<int>  pathCross(CrossSize,-1);
	std::vector<int>      dist(CrossSize,INF);
	std::vector<bool>    flag(CrossSize,true);
	int      source=car.from,destation=car.to;
	
	for(int i=0;i<RoadSize;i++){
		Road[i].time=std::ceil(1.0*Road[i].length/std::min(Road[i].speed,car.speed));//Road[i].channel
		// TODO:Optimization;
	}
//	for(int i=0;i<CrossSize;i++){
//		Cross[i].value=log2(Cross[i].value);
//		std::cout<<Cross[destation].value<<" ";
//	} 
//	std::cout<<"\n";
	for(int i=0;i<CrossSize;i++)
		if(Graph[source][i].roadid!=-1){
//		  dist[i]=Road[Graph[source][i].roadid].time+(Cross[i].value+Road[Graph[source][i].roadid].volume[Graph[source][i].orientation])/Road[Graph[source][i].roadid].channel;//
		    dist[i]=Road[Graph[source][i].roadid].time*
		            Cross[i].volume/Road[Graph[source][i].roadid].channel*
				    Road[Graph[source][i].roadid].volume[Graph[source][i].orientation]/Road[Graph[source][i].roadid].channel;
		  pathCross[i]=source;
		}
	dist[source]=0;
	flag[source]=false;
	
	for(int k=0;k<CrossSize;k++){
		int minValue=INF,minCrossid=-1;
		for(int i=0;i<CrossSize;i++){
 			if((flag[i])&&(minValue>dist[i])){
	 			minValue=dist[i];
				minCrossid=i;
			}
	    }
	    if(minCrossid==-1) break;//not connected;
		flag[minCrossid]=false;

		for(int i=0;i<CrossSize;i++){
			if(flag[i]){
				int roadid=Graph[minCrossid][i].roadid;
				int orientation=Graph[minCrossid][i].orientation;
				if((roadid!=-1)&&(dist[i]>(dist[minCrossid]+Road[roadid].time*Road[roadid].volume[orientation]/Road[roadid].channel*Cross[i].volume/Road[roadid].channel))){//+Road[roadid].value+Cross[i].value+Road[Graph[minCrossid][i].roadid].value
					dist[i]=dist[minCrossid]+Road[roadid].time*Road[roadid].volume[orientation]/Road[roadid].channel*Cross[i].volume/Road[roadid].channel;
					pathCross[i]=minCrossid;
				}
			}
//			if(!flag[destation]) break;
		}
	}
	
	while(pathCross[destation]!=-1){
		int roadid=Graph[pathCross[destation]][destation].roadid;
		int  orientation=Graph[pathCross[destation]][destation].orientation;
		plan.roadid.push_back(roadid);
		Road[roadid].volume[orientation]++;
		Cross[destation].volume++;
		Cross[pathCross[destation]].volume++;
		destation=pathCross[destation];
	}
	std::reverse(plan.roadid.begin(),plan.roadid.end());

	plan.id=car.id;
	plan.starttime=car.time;
} 
  
void Process(){
	sort(Car.begin(),Car.end(),cmpCar);
	
	for(int i=0;i<CarSize;i++){
		PlanWithDjikstra(Car[i],Plan[i]);
		Plan[i].starttime+=i/36;
	}
//	std::ofstream OutCross("crossvalue.txt");
//	for(int i=0;i<CrossSize;i++){
//		OutCross<<Cross[i].id<<"\t"<<Cross[i].value<<"\n";
//	}
} 

int main(int argc, char *argv[])
{
 //	  sgenrand((unsigned)time(NULL));//initialize RNG 
    std::cout << "Begin" << std::endl;
	
	if(argc < 5){
		std::cout << "please input args: carPath, roadPath, crossPath, answerPath" << std::endl;
		exit(1);
	}
	
	std::string carPath(argv[1]);
	std::string roadPath(argv[2]);
	std::string crossPath(argv[3]);
	std::string answerPath(argv[4]);
	
	std::cout << "carPath is "    << carPath    << std::endl;
	std::cout << "roadPath is "   << roadPath   << std::endl;
	std::cout << "crossPath is "  << crossPath  << std::endl;
	std::cout << "answerPath is " << answerPath << std::endl;
	
	Input(carPath,roadPath,crossPath);
	Process();
	Output(answerPath);
  	return 0; 
} 
//int main(){
//	//sgenrand((unsigned)time(NULL));//initialize RNG
//	Input("car.txt","road.txt","cross.txt");
//	Floyed(Graph);
//	Process();
//	Output("answar.txt");
//	return 0;
//}
