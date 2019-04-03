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
	int timeInMap;//Unused. 
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
	int   volume[2];//Unused. 
};
struct CROSS{
	int id;
	int roadid[4];
	int crossid[4];
	int volume;//unused.
};
struct CROSSGRAPHUNIT{
	int       roadid=-1;
	int    distance=INF;
	int   orientation=0;//isD.
	int         value=0;
};
struct  ROADGRAPHUNIT{
	int      crossid=-1;
	int   orientation=1;//3:Direct;2:Left;1:Right;
};
struct PLAN{
	int id;
	int starttime;
	std::vector<int> roadid;
};
struct  TIMESLICE{
	std::vector<int>                 crossVolume;
	std::vector<std::vector<int>>     roadVolume;
};
struct CROSSTIMESLICE{
	std::vector<int>                 crossVolume;
};
struct ROADTIMESLICE{
	std::vector<int>               roadVolume[2];
};

int                          SysTime=0;//Unused.
int                         MapCapcity;//Unused.
int                   deltaTime =  400;
const int           triggerTime =  500;
const int      carFlowSliceSize =   20;//36
const int         TimeSliceSize = 2000;
const int   CrossValueThreshold =   20;//unused. 
const int        timeCheckSize  =   30;
 
const double           phoCross = 0.1;
const double            phoRoad = 0.1;

std::map<int,int>               CarMap;
std::map<int,int>              RoadMap;
std::map<int,int>             CrossMap;

int                            CarSize;//PlanSize;
int                           RoadSize;//
int                          CrossSize;//GraphSize

std::vector<CAR>                             Car;
std::vector<ROAD>                           Road;
std::vector<CROSS>                         Cross;
std::vector<PLAN>                           Plan;

std::vector<std::vector<CROSSGRAPHUNIT> >   CrossGraph;
std::vector<std::vector<ROADGRAPHUNIT> >     RoadGraph;

std::vector<TIMESLICE>                            Time; 

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

void Floyed(std::vector<std::vector<CROSSGRAPHUNIT> > &crossgraph){
	for(int k=0;k<CrossSize;k++){
		for(int i=0;i<CrossSize;i++){
			for(int j=0;j<CrossSize;j++)
				crossgraph[i][j].distance=std::min(crossgraph[i][j].distance,crossgraph[i][k].distance+crossgraph[k][j].distance);
		}
	}
}

bool cmpCar(CAR &a,CAR &b){
//	return (a.time==b.time)?((a.timeInMap==b.timeInMap)?a.timeInMap<b.timeInMap:a.id<b.id):a.time<b.time;
	return (a.time==b.time)?a.id<b.id:a.time<b.time;
}

void MakeRoadOrientation(int roadid, int crossin, int crossout,bool isD){
	int i,j;
	for(i=0;i<4;i++) if(Cross[crossin].roadid[i]==roadid) break;//find the Road[roadid] index in Cross[crossin] 
	Cross[crossin].crossid[i]=crossout;
	RoadGraph[Cross[crossin].roadid[(i+2)%4]][roadid].orientation=3;
	RoadGraph[Cross[crossin].roadid[(i+3)%4]][roadid].orientation=2;
	RoadGraph[Cross[crossin].roadid[(i+1)%4]][roadid].orientation=1;
	if(isD){
		RoadGraph[roadid][Cross[crossin].roadid[(i+2)%4]].orientation=3;
		RoadGraph[roadid][Cross[crossin].roadid[(i+1)%4]].orientation=2;
		RoadGraph[roadid][Cross[crossin].roadid[(i+3)%4]].orientation=1;
	} 
	for(j=0;j<4;j++)
		if(j!=i){
			RoadGraph[Cross[crossin].roadid[j]][roadid].crossid=crossin;
			if(isD) RoadGraph[roadid][Cross[crossin].roadid[j]].crossid=crossin;
		}
	for(i=0;i<4;i++)  if(Cross[crossout].roadid[i]==roadid) break;//find the Road[roadid] index in Cross[crossout]
	if(isD) Cross[crossout].crossid[i]=crossin;
	RoadGraph[roadid][Cross[crossout].roadid[(i+2)%4]].orientation=3;
	RoadGraph[roadid][Cross[crossout].roadid[(i+1)%4]].orientation=2;
	RoadGraph[roadid][Cross[crossout].roadid[(i+3)%4]].orientation=1;
	if(isD){
		RoadGraph[Cross[crossout].roadid[(i+2)%4]][roadid].orientation=4;
		RoadGraph[Cross[crossout].roadid[(i+3)%4]][roadid].orientation=3;
		RoadGraph[Cross[crossout].roadid[(i+1)%4]][roadid].orientation=2;
	} 
	for(j=0;j<4;j++)
		if(j!=i){
			RoadGraph[roadid][Cross[crossout].roadid[j]].crossid=crossout;
			if(isD) RoadGraph[Cross[crossout].roadid[j]][roadid].crossid=crossout;
		}
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
						std::stoi(data[4]),0};
			Car.push_back(tmpcar);
			CarMap.insert(std::pair<int,int>(tmpcar.id,CarSize++));
		}
	}
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
				          bool(std::stoi(data[6])),0,{1,1}};
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
						    RoadMap[std::stoi(data[4])]},{-1,-1,-1,-1},1};
			Cross.push_back(tmpcross);
		    CrossMap.insert(std::pair<int,int>(tmpcross.id,CrossSize++));
		}
	}
	IN.close();

	RoadGraph.resize(RoadSize,std::vector<ROADGRAPHUNIT>(RoadSize));
	CrossGraph.resize(CrossSize,std::vector<CROSSGRAPHUNIT>(CrossSize));

	for(int i=0;i<RoadSize;i++){
		Road[i].from=CrossMap[Road[i].from];
		Road[i].to=CrossMap[Road[i].to];
		MakeRoadOrientation(i, Road[i].from, Road[i].to, Road[i].isDuplex);
		
		CrossGraph[Road[i].from][Road[i].to].roadid=i;
		CrossGraph[Road[i].from][Road[i].to].distance=Road[i].length;
		if(Road[i].isDuplex){
			CrossGraph[Road[i].to][Road[i].from].roadid=i;
			CrossGraph[Road[i].to][Road[i].from].distance=Road[i].length;
			CrossGraph[Road[i].to][Road[i].from].orientation=1;
		}
	}
	Floyed(CrossGraph);
	for(int i=0;i<CarSize;i++){
		Car[i].from = CrossMap[Car[i].from];
		Car[i].to   = CrossMap[Car[i].to];
		Car[i].timeInMap=CrossGraph[Car[i].from][Car[i].to].distance/Car[i].speed;
	}
}

inline float getCrossVolume(int cur_t,int crossid){
	int starttime=(cur_t-timeCheckSize<0?0:cur_t-timeCheckSize),endtime=cur_t+timeCheckSize;
//	int starttime=cur_t,endtime=cur_t+timeCheckSize;
	double crossvolume=0;
	for(int i=starttime;i<=endtime;i++){
		crossvolume+=Time[i].crossVolume[crossid];
	}
	return crossvolume/(endtime-starttime+1);	
}

inline float getRoadVolume(int cur_t,int roadid,int isD){
	int starttime=(cur_t-timeCheckSize<0?0:cur_t-timeCheckSize),endtime=cur_t+timeCheckSize;
//	int starttime=cur_t,endtime=cur_t+timeCheckSize;
	double roadvolume=0;
	for(int i=starttime;i<=endtime;i++){
		roadvolume+=Time[i].roadVolume[roadid][isD];
	}
	return roadvolume/(endtime-starttime+1);
} 
void PlanCar(CAR &car,PLAN &plan){
	std::vector<float>                dist(CrossSize,FLT_MAX);
	std::vector<int>                  pathCross(CrossSize,-1);//pathCross[t][i]:the pro of Cross[i] in Time[+t]
	std::vector<bool>                    flag(CrossSize,true);
	std::vector<int>                              plancrossid;
	std::vector<std::vector<int> >    roadtime(CrossSize,std::vector<int>(CrossSize,INF)); 
	int      source=car.from,destation=car.to;
	int         currenttime=car.time;
	 
	for(int i=0;i<RoadSize;i++){
		Road[i].time=std::ceil(Road[i].length/std::min(Road[i].speed,car.speed));//ceil is better.
	}
	for(int i=0;i<CrossSize;i++){
		for(int j=0;j<CrossSize;j++){
			roadtime[i][j]=Road[CrossGraph[i][j].roadid].time;
		}
	}
	for(int i=0;i<CrossSize;i++){
		int roadid=CrossGraph[source][i].roadid;
		if(roadid!=-1){
			int t=Road[roadid].time;
			dist[i]=Road[roadid].time* 
					getCrossVolume(currenttime+t,i)/Road[roadid].channel*
					getRoadVolume(currenttime+t,roadid,CrossGraph[source][i].orientation);//Road[roadid].channel;
//			std::cout<<"|RoadTime "<<Road[roadid].time
//					 <<"|CrossVolume"<<getCrossVolume(currenttime+t,i)/Road[roadid].channel
//					 <<"|RoadVolume"<<getRoadVolume(currenttime+t,roadid,CrossGraph[source][i].orientation)/Road[roadid].channel<<"\n";
			pathCross[i]=source;
//			std::cout<<i<<" "<<dist[i]<<"\n";
		}
	}
	flag[source]=false;
	int cartime=0;
	
	for(int k=0;k<CrossSize;k++){
		int minCrossid=-1;
		float minValue=FLT_MAX;
		for(int i=0;i<CrossSize;i++){
 			if((flag[i])&&(minValue>dist[i])){
	 			minValue=dist[i];
				minCrossid=i;
			}
	    }
	    if(minCrossid==-1) break;//not connected;
		flag[minCrossid]=false;

		currenttime+=roadtime[pathCross[minCrossid]][minCrossid];
//		std::cout<<"currenttime:"<<currenttime<<"\n";
		for(int i=0;i<CrossSize;i++){
			if(flag[i]){
				int roadid  = CrossGraph[minCrossid][i].roadid;
				int roadidp = CrossGraph[pathCross[minCrossid]][minCrossid].roadid;//Unused.
				if(roadid!=-1){
					int t=roadtime[minCrossid][i];
					double deltadist=Road[roadid].time* 
									 getCrossVolume(currenttime+t, i)/Road[roadid].channel*
									 getRoadVolume(currenttime+t, roadid,CrossGraph[minCrossid][i].orientation);//Road[roadid].channel; 
					if(dist[i]>(dist[minCrossid]+deltadist))
						dist[i]=dist[minCrossid]+deltadist;
					pathCross[i]=minCrossid;
				}
			}
			if(!flag[destation]) break;
		}
	}

	int to=destation;plan.roadid.clear();plancrossid.clear();
	while(pathCross[to]!=-1){
		int roadid=CrossGraph[pathCross[to]][to].roadid;
		plan.roadid.push_back(roadid);
		plancrossid.push_back(to);
		to=pathCross[to];
	}
	plancrossid.push_back(to);
	
	std::reverse(plan.roadid.begin(),plan.roadid.end());
	std::reverse(plancrossid.begin(),plancrossid.end());
	
	//Update CrossVolume with Road.
	int time=car.time;
	for(int i=0;i<plan.roadid.size();i++){
		time+=Road[plan.roadid[i]].time;
		for(int t=0;t<=time;t++){
			Time[t].crossVolume[Road[plan.roadid[i]].from]++;
			Time[t].crossVolume[Road[plan.roadid[i]].to]++;
		}
		time--;
	}
	time=car.time;
	//RoadVolume use Road.
	for(int i=0;i<plancrossid.size()-1;i++){
		int roadid=CrossGraph[plancrossid[i]][plancrossid[i+1]].roadid;
		int orientation=CrossGraph[plancrossid[i]][plancrossid[i+1]].orientation;
		
		time+=Road[roadid].time;
		for(int t=0;t<=car.time+time;t++){
			Time[t].roadVolume[roadid][orientation]++;
		}
		time--;
//		for(int t=0;t<=Road[roadid].time;t++){
//			Time[car.time+time].roadVolume[roadid][orientation]++;
//			time++;
//		}
//		time--;
	} 

	plan.id=car.id;
	plan.starttime=car.time;
} 

void Process(std::string answerfile){
	std::ofstream OUT(answerfile);
	OUT<<"#(carId,StartTime,RoadId...)\n";
	
	sort(Car.begin(),Car.end(),cmpCar);

	for(int i=0;i<TimeSliceSize;i++)
		Time.push_back({std::vector<int>(CrossSize,1),
		                std::vector<std::vector<int> >(RoadSize,std::vector<int>(2,1))});
	Plan.resize(CarSize);
//	PlanCar(Car[0],Plan[0]);
	for(int i=0;i<CarSize;i++){
		Car[i].time+=i/carFlowSliceSize;
	    PlanCar(Car[i],Plan[i]);// Time.push_back({std::vector<int>(CrossSize,0)});

		OUT<<"("<<Plan[i].id<<","<<Plan[i].starttime;//+i/15
		for(auto it=Plan[i].roadid.begin();it!=Plan[i].roadid.end();it++) 
			OUT<<","<<Road[(*it)].id;//vector<int> rns[-1]
		OUT<<")\n";
	}
	OUT.close();
} 

//int main(int argc, char *argv[])
//{
//// 	  sgenrand((unsigned)time(NULL));//initialize RNG 
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
//	std::cout << "carPath is "    << carPath    << std::endl;
//	std::cout << "roadPath is "   << roadPath   << std::endl;
//	std::cout << "crossPath is "  << crossPath  << std::endl;
//	std::cout << "answerPath is " << answerPath << std::endl;
//
//	Input(carPath,roadPath,crossPath);
//	Process(answerPath);
//  return 0;
//}
int main(){
//	sgenrand((unsigned)time(NULL));//initialize RNG
	Input("car.txt","road.txt","cross.txt");
	Process("answer.txt");
	return 0;
}
