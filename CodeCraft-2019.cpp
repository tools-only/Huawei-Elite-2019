#include <algorithm>
#include <iterator>
#include <memory.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <cstdio>
#include <cfloat>
#include "RNG.h" 
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
	int value;
};
struct ROAD{
	int   id;
	int   length;
	int   speed;
	int   channel;
	int   from;
	int   to;
	bool  isDuplex;
	int   value;
};
struct CROSS{
	int id;
	int roadid[4];
	int value;
};
struct GRAPH{
	int    roadid=-1;
};
struct PLAN{
	int id;
	int starttime;
	std::vector<int> roadid;
};

std::map<int,int>               CarMap,
                               RoadMap,
				              CrossMap;
int                            CarSize,
                              RoadSize,
				             CrossSize;
std::vector<CAR>                   Car;
std::vector<ROAD>                 Road;
std::vector<CROSS>               Cross;
std::vector<std::vector<GRAPH> > Graph;
std::vector<PLAN>                 Plan;
int                            timenow; 
const double        DjikstraFactor=0.5; 
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
			CarMap.insert(std::pair<int,int>(tmpcar.id,CarSize++));
		}
	}
	Plan.resize(CarSize);
//	for(auto it:Car)
//		std::cout<<it.id<<" "<<it.value<<"\n";
//	for(auto it:CarMap) 
//		std::cout<<it.first<<" "<<it.second<<"\n";
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
				          bool(std::stoi(data[6])),1};
			Road.push_back(tmproad);
		    RoadMap.insert(std::pair<int,int>(tmproad.id,RoadSize++));
		}
	}
//	for(auto it:Road)
//		std::cout<<it.id<<" "<<it.to<<" ";
//	for(auto it:RoadMap) 
//		std::cout<<it.first<<" "<<it.second<<"\n";
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
						   {RoadMap[std::stoi(data[1])],
						    RoadMap[std::stoi(data[2])],
						    RoadMap[std::stoi(data[3])],
						    RoadMap[std::stoi(data[4])]},0};
			Cross.push_back(tmpcross);
		    CrossMap.insert(std::pair<int,int>(tmpcross.id,CrossSize++));
		}
	}
//	for(auto it:CrossMap) 
//		std::cout<<it.first<<" "<<it.second<<"\n";
	IN.close();
	
//	for(auto it:Cross)
//		std::cout<<it.id<<" "<<it.roadid[0]<<"\n";
//	for(auto itCar:Car){//Error cost 4h+!!
//		std::cout<<itCar.id<<" "<<itCar.from<<" ";
//		itCar.from=CrossMap[itCar.from];
//		itCar.to=CrossMap[itCar.to];
//		std::cout<<itCar.from<<"\n";		
//	}
	for(int i=0;i<CarSize;i++){
		Car[i].from=CrossMap[Car[i].from];
		Car[i].to=CrossMap[Car[i].to];
	}

//	for(auto it:Car)
//		std::cout<<it.id<<" "<<it.from<<"\n";
		
	Graph.resize(CrossSize,std::vector<GRAPH>(CrossSize));
	for(int i=0;i<RoadSize;i++){
		Road[i].from=CrossMap[Road[i].from];
		Road[i].to=CrossMap[Road[i].to];
		Graph[Road[i].from][Road[i].to].roadid=i;
		if(Road[i].isDuplex){
			Graph[Road[i].to][Road[i].from].roadid=i;
//			std::cout<<Road[i].from<<" "<<Road[i].to<<"\n";
		}
	}
//	int cntt=0;
//	for(auto it:Graph){
//		std::cout<<cntt++<<":";
//		for(auto it_:it){
//			std::cout<<it_.roadid<<" ";
//		}
//		std::cout<<"\n";
//	}
}

void Output(std::string answerfile){
	std::ofstream OUT(answerfile);
	OUT<<"#(carId,StartTime,RoadId...)\n";
	//for(int )
	for(int i=0;i<CarSize;i++){
		OUT<<"("<<Plan[i].id<<","<<Plan[i].starttime+i/30;
		for(auto it=Plan[i].roadid.begin();it!=Plan[i].roadid.end();it++){
			OUT<<","<<Road[(*it)].id;//vector<int> rns[-1]
			//OUT<<","<<*it_;		
		}
		OUT<<")\n";
	} 
	OUT.close();
}

bool cmpCar(CAR &a,CAR &b){
	return (a.time==b.time)?((a.speed==b.speed)?a.speed>b.speed:a.id<b.id):a.time<b.time;
}

void Floyed(){
	 
}

void PlanWithDjikstra(CAR &car,PLAN &plan){
//	std::vector<int>             tmpRoadList;
//	std::stack<int>            tmpCrossStack; 
	std::vector<int> pathCross(CrossSize,-1);	
	std::vector<int>     dist(CrossSize,INF);
	std::vector<bool>   flag(CrossSize,true);
	int     source=car.from,destation=car.to;
//	int                                  now;
//	tmpRoadList.clear();
	
	for(int i=0;i<RoadSize;i++){
		Road[i].value=Road[i].value+Road[i].length/Road[i].channel/std::min(Road[i].speed,car.speed);
		// TODO:Optimization;
	}
//	std::cout<<source<<" "<<destation<<"\n";
//	int cntt=0;
//	for(auto it:Graph){
//		std::cout<<cntt++<<":";
//		for(auto it_:it){
//			std::cout<<it_.roadid<<" ";
//		}
//		std::cout<<"\n";
//	}
	for(int i=0;i<CrossSize;i++)
		if(Graph[source][i].roadid!=-1){
		  dist[i]=Road[Graph[source][i].roadid].value+Cross[source].value+Cross[i].value;
		  pathCross[i]=source;
//		  std::cout<<i<<" "<<dist[i]<<"\n";
		}
	dist[source]=0;
	flag[source]=false;
	
	for(int k=0;k<CrossSize;k++){
		int minValue=INF,minCrossid=-1;
		bool findOne=false;
//		std::cout<<"-----\nk="<<k<<"\n";
		for(int i=0;i<CrossSize;i++){
 			if((flag[i])&&(minValue>dist[i])){
 				if(findOne){
 					if(randf()>DjikstraFactor){
					  	minValue=dist[i];
						minCrossid=i;	
//						std::cout<<randf()<<'\n'; 
					}
				} 
				else{
	 				minValue=dist[i];
					minCrossid=i;
					findOne=true;					
				}
//				std::cout<<minCrossid<<" "<<minValue<<"\n";
			}
	    }
	    if(minCrossid==-1) break;//not connect;
		flag[minCrossid]=false;
//		plan.roadid.push_back(roadid);
//		std::cout<<"minCross£º"<<minCrossid<<"\n";
		for(int i=0;i<CrossSize;i++){
			if(flag[i]){
				int roadid=Graph[minCrossid][i].roadid;
				if((roadid!=-1)&&(dist[i]>(dist[minCrossid]+Road[roadid].value+Cross[i].value))){
					dist[i]=dist[minCrossid]+Road[roadid].value+Cross[i].value;
					pathCross[i]=minCrossid;
//					std::cout<<"minCross£º"<<minCrossid<<" "<<i<<"\n"; 
				} 
			}	
		}
	}
	
	while(pathCross[destation]!=-1){
		int roadid=Graph[pathCross[destation]][destation].roadid;
		plan.roadid.push_back(roadid);
		Road[roadid].value++;
		Cross[destation].value++;
		Cross[pathCross[destation]].value++;
		destation=pathCross[destation];

	}
	std::reverse(plan.roadid.begin(),plan.roadid.end());
//	for(auto it:plan.roadid){
//		std::cout<<Road[it].id<<' ';
//	}	
//	std::cout<<'\n';
//	for(int i=0;i<tmpCrossList.size()-1;i++){
//		//std::cout<<tmpCrossList[i]<<" "<<tmpCrossList[i+1]<<"\n";
//		//std::cout<<Graph[tmpCrossList[i]][tmpCrossList[i+1]].roadid<<"\n";
//		int roadid=Graph[tmpCrossList[i]][tmpCrossList[i+1]].roadid;
//		tmpRoadList.push_back(roadid);
//		Road[roadid].value--;
//	}
	plan.id=car.id;
	plan.starttime=car.time;
} 
  
void Process(){
//	for(auto it:Car)
//		std::cout<<it.id<<" "<<it.from<<"\n";
//	for(int i=0;i<CarSize;i++) Car[i].time+=i/500;
		
	sort(Car.begin(),Car.end(),cmpCar);
	
	for(int i=0;i<CarSize;i++){
		PlanWithDjikstra(Car[i],Plan[i]);
	}
//	PlanWithDjikstra(Car[0],Plan[0]);
} 

//int main(int argc, char *argv[])
//{
// 	  sgenrand((unsigned)time(NULL));//initialize RNG 
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
//	Process();
//	Output(answerPath);
//  	return 0; 
//} 
int main(){
	sgenrand((unsigned)time(NULL));//initialize RNG
	Input("car.txt","road.txt","cross.txt");
	Process();
	Output("answar.txt");
	return 0;
}
