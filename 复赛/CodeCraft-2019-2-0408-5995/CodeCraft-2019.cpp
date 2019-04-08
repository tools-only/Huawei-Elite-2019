#include <algorithm>
#include <iterator>
#include <memory.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <climits> 
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
const int INF = 0x3f3f3f3f;
//交通类数据结构. 
struct INFO_CAR {
	int                  id;
	int                from;
	int                  to;
	int               speed;
	int            plantime;
	bool        hasPriority;
	bool           isPreset;
	//	int timeInMap;
};
struct INFO_PRESETCAR{
	int                  id;
	int           starttime;
	std::vector<int> roadid;
};
struct INFO_ROAD {
	int                  id;
	int              length;
	int               speed;
	int             channel;
	int                from;
	int                  to;
	bool           isDuplex;
	int                time;
	int           volume[2];
};
struct INFO_CROSS {
	int                  id;
	int           roadid[4];
	int          crossid[4];
	int            roadSize;
	int              volume;
};
struct INFO_PLAN {
	int                  id;
	int           starttime;
	std::vector<int> roadid;
};
struct CROSSMAPUNIT {
	int         roadid = -1;
	int      distance = INF;
	int     orientation = 0; 
	int           value = 0;
};
struct  ROADMAPUNIT {
	int        crossid = -1;
	int     orientation = 1;//3:Direct;2:Left;1:Right;
};

//Based on TimeSlice;
struct SIMULATION_CAR{
	int               state;
	int              roadid;
	int             channel;
	int            posation;
};
struct SIMULATION_ROAD{
	std::vector<std::queue<int> > channelqueue[2];
};
struct SIMULATION_CROSS{
	std::queue<int>                   waitarea[4];
};

int                      minPlanTime  = INF;
int             maxPresetCarPlanTime  =  -1;

int                  carFlowSliceSize =  40;//36
const int                 triggerTime = 500;
const int  deltaTime =  32*carFlowSliceSize;
const int              topHideCrossSize = 1;
int                             sysTime = 0; 

const int              priorityFactor =   1;
const int                presetFactor =   2;
const int                normalFactor =   1;

const double                 phoCross = 0.1;
const double                  phoRoad = 0.2;
//const double          phoCrossVolume1 = 0.2;
const double          phoCrossVolume2 = 0.8;

std::map<int, int>                   CarMap;
std::map<int, int>             PresetCarMap;
std::map<int, int>       HighPriorityCarMap;
std::map<int, int>                  RoadMap;
std::map<int, int>                 CrossMap;

int                                 CarSize;//PlanSize; 
int                           PresetCarSize;//
int                     HighPriorityCarSize;//
int                                RoadSize;//
int                               CrossSize;//GraphSize
int                                PlanSize;

std::vector<INFO_CAR>                         Car;
std::vector<INFO_PRESETCAR>             PresetCar;
std::vector<INFO_ROAD>                       Road;
std::vector<INFO_CROSS>                     Cross;
std::vector<INFO_PLAN>                       Plan;

std::vector<std::vector<CROSSMAPUNIT> >   CrossGraphMap;//根据Cross,寻找Road.
std::vector<std::vector<ROADMAPUNIT> >     RoadGraphMap;//

//字符串处理函数.
void splitString(const std::string& s, std::vector<int>& data, const std::string& c) {
	std::string::size_type pos1, pos2;
	pos2 = s.find(c); pos1 = 0; data.clear();
	while (std::string::npos != pos2) {
		data.push_back(std::stoi(s.substr(pos1, pos2 - pos1)));
		pos1 = pos2 + c.size(); pos2 = s.find(c, pos1);
	}
	if (pos1 != s.length()) data.push_back(std::stoi(s.substr(pos1)));
}
//log2函数
int log2(int _val) {
	int ret;
	float fdata = (float)_val;
	//unsigned int uData = (fdata>>23)&0xFF; //直接移动出错
	unsigned int data = (unsigned int&)fdata;//(unsigned int&)fdata 与*(unsigned int*)&fdata; 一致
	unsigned int udata = (data >> 23) & 0xFF;
	ret = (int)udata - 127;//-无符号到有符号

	return ret;
}
bool cmpCar(INFO_CAR &a, INFO_CAR &b) {
	return (a.hasPriority == b.hasPriority) ? ((a.plantime == b.plantime)? a.id < b.id: a.plantime < b.plantime) : a.hasPriority > b.hasPriority;//this is best. 
}

void makeRoadOrientation(int roadid, int crossin, int crossout, bool isD) {
	int i, j;
	for (i = 0; i < 4; i++) if (Cross[crossin].roadid[i] == roadid) break;//find the Road[roadid] index in Cross[crossin] 
	Cross[crossin].crossid[i] = crossout;
	RoadGraphMap[Cross[crossin].roadid[(i+2)%4]][roadid].orientation=3;
	RoadGraphMap[Cross[crossin].roadid[(i+3)%4]][roadid].orientation=2;
	RoadGraphMap[Cross[crossin].roadid[(i+1)%4]][roadid].orientation=1;
	if(isD){
		RoadGraphMap[roadid][Cross[crossin].roadid[(i+2)%4]].orientation=3;
		RoadGraphMap[roadid][Cross[crossin].roadid[(i+1)%4]].orientation=2;
		RoadGraphMap[roadid][Cross[crossin].roadid[(i+3)%4]].orientation=1;
	} 
	for (j = 0; j < 4; j++)
		if (j != i) {
			RoadGraphMap[Cross[crossin].roadid[j]][roadid].crossid=crossin;
			if(isD) RoadGraphMap[roadid][Cross[crossin].roadid[j]].crossid=crossin;
		}
	for (i = 0; i < 4; i++)  if (Cross[crossout].roadid[i] == roadid) break;//find the Road[roadid] index in Cross[crossout]
	if (isD) Cross[crossout].crossid[i] = crossin;
	RoadGraphMap[roadid][Cross[crossout].roadid[(i + 2) % 4]].orientation = 3;
	RoadGraphMap[roadid][Cross[crossout].roadid[(i + 1) % 4]].orientation = 2;
	RoadGraphMap[roadid][Cross[crossout].roadid[(i + 3) % 4]].orientation = 1;
	if (isD) {
		RoadGraphMap[Cross[crossout].roadid[(i + 2) % 4]][roadid].orientation = 3;
		RoadGraphMap[Cross[crossout].roadid[(i + 3) % 4]][roadid].orientation = 2;
		RoadGraphMap[Cross[crossout].roadid[(i + 1) % 4]][roadid].orientation = 1;
	}
	for (j = 0; j < 4; j++)
		if (j != i) {
			RoadGraphMap[roadid][Cross[crossout].roadid[j]].crossid = crossout;
			if (isD) RoadGraphMap[Cross[crossout].roadid[j]][roadid].crossid = crossout;
		}
}

void Input(const std::string &Carfile, const std::string &Roadfile, const std::string &Crossfile,const std::string &PresetCarFile) {
	std::string tmps;
	std::vector<int> data;
	std::ifstream IN(Carfile);
	int cnt=0;
	
	CarSize = PresetCarSize = HighPriorityCarSize = RoadSize = CrossSize = PlanSize = 0;
	Car.clear(); PresetCar.clear(); Road.clear(); Cross.clear(); Plan.clear();
	CarMap.clear(); PresetCarMap.clear(); HighPriorityCarMap.clear(); RoadMap.clear(); CrossMap.clear();
	RoadGraphMap.clear(); CrossGraphMap.clear();
	
	std::getline(IN, tmps);
	while (!IN.eof()) {
		std::getline(IN, tmps);
//		if (tmps.at(0) != '#') {//[Error] ISO C++ forbids comparison between pointer and integer [-fpermissive]
			tmps = tmps.substr(1, tmps.size() - 2);
			splitString(tmps, data, ",");
			INFO_CAR tmpcar = {data[0],data[1],data[2],data[3],data[4],bool(data[5]),bool(data[6])};
			if(minPlanTime>tmpcar.plantime) minPlanTime=tmpcar.plantime;
			Car.push_back(tmpcar);CarSize++;
//		}
	}
	std::sort(Car.begin(),Car.end(),cmpCar);
	cnt=0,for_each(Car.begin(),Car.end(),[&](INFO_CAR &a){CarMap.insert(std::pair<int, int>(a.id,cnt++));});
	Plan.resize(CarSize);
	IN.close();
	
	IN.open(Roadfile);		std::getline(IN, tmps);
	while (!IN.eof()) {
		std::getline(IN, tmps);
//		if (tmps.at(0) != '#') {//[Error] ISO C++ forbids comparison between pointer and integer [-fpermissive]
			tmps = tmps.substr(1, tmps.size() - 2);
			splitString(tmps, data, ",");
			INFO_ROAD tmproad = {data[0],data[1],data[2],data[3],data[4],data[5],bool(data[6]),0,{1,1}};
			Road.push_back(tmproad);RoadSize++;
//		}
	}
	std::sort(Road.begin(),Road.end(),[](INFO_ROAD &a, INFO_ROAD &b){return (a.id < b.id);});
	cnt=0,for_each(Road.begin(),Road.end(),[&](INFO_ROAD &a){RoadMap.insert(std::pair<int, int>(a.id,cnt++));});
	IN.close();

	IN.open(Crossfile);		std::getline(IN, tmps);
	while (!IN.eof()) {
		std::getline(IN, tmps);
//		if (tmps.at(0) != '#') {//[Error] ISO C++ forbids comparison between pointer and integer [-fpermissive]
			tmps = tmps.substr(1, tmps.size() - 2);
			splitString(tmps, data, ",");
			INFO_CROSS tmpcross = {data[0],{RoadMap[data[1]],RoadMap[data[2]],RoadMap[data[3]],RoadMap[data[4]]},{-1,-1,-1,-1},0,1 };
			cnt=0;for(int i=0;i<4;i++) if(tmpcross.roadid[i]!=0)cnt++;tmpcross.roadSize=cnt;
			Cross.push_back(tmpcross);CrossSize++;
//		}
	}
	std::sort(Cross.begin(),Cross.end(),[](INFO_CROSS &a,INFO_CROSS &b){return (a.id < b.id);});
	cnt=0,for_each(Cross.begin(),Cross.end(),[&](INFO_CROSS &a){CrossMap.insert(std::pair<int, int>(a.id,cnt++));});
	IN.close();
	
	CrossGraphMap.resize(CrossSize, std::vector<CROSSMAPUNIT>(CrossSize));
	RoadGraphMap.resize(RoadSize,std::vector<ROADMAPUNIT>(RoadSize));
	
	for (int i = 0; i < RoadSize; i++) {
		Road[i].from = CrossMap[Road[i].from];
		Road[i].to   = CrossMap[Road[i].to];
		makeRoadOrientation(i, Road[i].from, Road[i].to, Road[i].isDuplex);

		CrossGraphMap[Road[i].from][Road[i].to].roadid = i;
		CrossGraphMap[Road[i].from][Road[i].to].distance = Road[i].length;
		if (Road[i].isDuplex) {
			CrossGraphMap[Road[i].to][Road[i].from].roadid = i;
			CrossGraphMap[Road[i].to][Road[i].from].distance = Road[i].length;
			CrossGraphMap[Road[i].to][Road[i].from].orientation = 1;
		}
	}
	//CrossMap must be right. 
	for (int i = 0; i < CarSize; i++) {
		Car[i].from = CrossMap[Car[i].from];
		Car[i].to   = CrossMap[Car[i].to];
	}
	
	IN.open(PresetCarFile);		std::getline(IN, tmps);
	while (!IN.eof()) {
		std::getline(IN, tmps);
//		if (tmps.at(0) != '#') {//[Error] ISO C++ forbids comparison between pointer and integer [-fpermissive]
			tmps = tmps.substr(1, tmps.size() - 2);
			splitString(tmps, data, ",");
			INFO_PRESETCAR tmppcar={data[0],data[1]};
			INFO_PLAN tmpplan={data[0],data[1]}; 
			tmppcar.roadid.assign(data.begin()+2,data.end());
			for(unsigned int i=0;i<tmppcar.roadid.size();i++) tmppcar.roadid[i]=RoadMap[tmppcar.roadid[i]];
			tmpplan.roadid=tmppcar.roadid;
			PresetCar.push_back(tmppcar);
//			Plan[CarMap[tmppcar.id]]=tmpplan;
			PresetCarSize++;
//		}
	}
	sort(PresetCar.begin(),PresetCar.end(),[&](INFO_PRESETCAR &a,INFO_PRESETCAR &b){return (a.starttime==b.starttime)?a.id<b.id:a.starttime<b.starttime;});
	cnt=0,for_each(PresetCar.begin(),PresetCar.end(),[&](INFO_PRESETCAR &a){PresetCarMap.insert(std::pair<int, int>(a.id,cnt++));});
	IN.close();
	
	maxPresetCarPlanTime=PresetCar[PresetCarSize-1].starttime;
}

void Output(std::string answerfile) {
	std::ofstream OUT(answerfile);
	OUT << "#(carId,StartTime,RoadId...)\n";
	for (int i = 0; i < CarSize; i++) {
		if(!Car[i].isPreset){
			OUT << "(" << Plan[i].id << "," << Plan[i].starttime;
			for (auto it = Plan[i].roadid.begin(); it != Plan[i].roadid.end(); it++) OUT << "," << Road[(*it)].id;//vector<int> rns[-1]
			OUT << ")\n";	
		}
//		else{
//			int presetcarid=PresetCarMap[Car[i].id]; 
//			OUT << "(" << PresetCar[presetcarid].id << "," << PresetCar[presetcarid].starttime;
//			for (auto it = PresetCar[presetcarid].roadid.begin(); it != PresetCar[presetcarid].roadid.end(); it++) OUT << "," << Road[(*it)].id;//vector<int> rns[-1]
//			OUT << ")\n";	
//		} 
	}
	OUT.close();
}

void Floyed(std::vector<std::vector<CROSSMAPUNIT> > &crossgraph) {
	for (int k = 0; k < CrossSize; k++) {
		for (int i = 0; i < CrossSize; i++) {
			for (int j = 0; j < CrossSize; j++)
				crossgraph[i][j].distance = std::min(crossgraph[i][j].distance, crossgraph[i][k].distance + crossgraph[k][j].distance);
		}
	}
}

void PlanCar(INFO_CAR &car, INFO_PLAN &plan, bool trigger) {
	std::vector<int>                 pathCross(CrossSize, -1);
	std::vector<long long>     dist(CrossSize, LONG_LONG_MAX);
	std::vector<bool>                   flag(CrossSize, true);
	std::vector<int>        	     CrossVolume(CrossSize,0);
 	int                 source = car.from, destation = car.to;

	for (int i = 0; i < RoadSize; i++) {
		Road[i].time = std::ceil(1.0*Road[i].length / std::min(Road[i].speed, car.speed));//Road[i].channel
	}
	
	if (trigger) {
		for (int i = 0; i < RoadSize; i++) {
			Road[i].volume[0] *= (1 - phoRoad);
			if (Road[i].volume[0] == 0) Road[i].volume[0] = 1;
			Road[i].volume[1] *= (1 - phoRoad);
			if (Road[i].volume[1] == 0) Road[i].volume[1] = 1;
		}
		for (int i = 0; i < CrossSize; i++) {
			Cross[i].volume *= (1 - phoCross);
			if (Cross[i].volume == 0) Cross[i].volume = 1;
		}
	}

//	for(int i = 0; i < CrossSize; i++){
//		int crossNeighborVolume=0;
//		for(int j=0;j < 4;j++)
//		if(Cross[i].crossid[j]!=-1){
//			crossNeighborVolume+=Cross[Cross[i].crossid[j]].volume; 
//		}
//		CrossVolume[i]=Cross[i].volume*Cross[i].roadSize*(1-phoCrossVolume1)+
//			crossNeighborVolume/(Cross[i].roadSize)*phoCrossVolume1;
//	}	
//	for(int i = 0; i < CrossSize; i++){
//		int crossNeighborVolume=0;
//		for(int j=0;j < 4;j++)
//		if(Cross[i].crossid[j]!=-1){
//			crossNeighborVolume+=Cross[Cross[i].crossid[j]].volume; 
//		}
//		CrossVolume[i]=Cross[i].volume*(1-phoCrossVolume1)+
//			crossNeighborVolume/(Cross[i].roadSize)*phoCrossVolume1;
//	}
	
	for(int i = 0; i < CrossSize; i++){
		int crossNeighborVolume=0;
		for(int j=0;j < 4;j++)
		if(Cross[i].crossid[j]!=-1){
			crossNeighborVolume+=CrossVolume[Cross[i].crossid[j]]; 
		}
		CrossVolume[i]=Cross[i].volume*(1-phoCrossVolume2)+
			crossNeighborVolume/(Cross[i].roadSize)*phoCrossVolume2;
	}

//--------------------
	for(int i = 0; i < CrossSize; i++)
		if (CrossGraphMap[source][i].roadid != -1) {
			dist[i] = Road[CrossGraphMap[source][i].roadid].time+ 
				CrossVolume[i] / Road[CrossGraphMap[source][i].roadid].channel *
				Road[CrossGraphMap[source][i].roadid].volume[CrossGraphMap[source][i].orientation] / Road[CrossGraphMap[source][i].roadid].channel;// 
			pathCross[i] = source;
		}
	dist[source] = 0;
	flag[source] = false;

	for (int k = 0; k < CrossSize; k++) {
		long long minValue = LONG_LONG_MAX;
		int minCrossid = -1;
		for (int i = 0; i < CrossSize; i++) {
			if ((flag[i]) && (minValue > dist[i])) {
				minValue = dist[i];
				minCrossid = i;
			}
		}
		if (minCrossid == -1) break;//not connected;
		flag[minCrossid] = false;

		for (int i = 0; i < CrossSize; i++) {
			if (flag[i]) {
				int roadid = CrossGraphMap[minCrossid][i].roadid;
				if (roadid != -1) {
					//int roadidp = CrossGraphMap[pathCross[minCrossid]][minCrossid].roadid;
					int orientation = CrossGraphMap[minCrossid][i].orientation;
					long long deltadist = Road[roadid].time +
						Road[roadid].volume[orientation] / Road[roadid].channel*
						CrossVolume[i] / Road[roadid].channel ;
					if (dist[i] > (dist[minCrossid] + deltadist)) {
						dist[i] = dist[minCrossid] + deltadist;
						pathCross[i] = minCrossid;
					}
				}
			}
			if(!flag[destation]) break;
		}
	}

	while (pathCross[destation] != -1) {
		int roadid = CrossGraphMap[pathCross[destation]][destation].roadid;
		int orientation = CrossGraphMap[pathCross[destation]][destation].orientation;
		plan.roadid.push_back(roadid);
		Road[roadid].volume[orientation] += car.hasPriority*priorityFactor + normalFactor;
		Cross[destation].volume += car.hasPriority*priorityFactor + normalFactor;
		Cross[pathCross[destation]].volume += car.hasPriority*priorityFactor + normalFactor;
		destation = pathCross[destation];
	}
	std::reverse(plan.roadid.begin(), plan.roadid.end());

	plan.id = car.id;
	plan.starttime = car.plantime;
}

void PlanPresetCar(INFO_CAR &car,INFO_PRESETCAR &presetcar){
	int formCross=car.from,nextCross,orientation;
	for(unsigned int i=0;i<presetcar.roadid.size()-1;i++){
		nextCross   = RoadGraphMap[presetcar.roadid[i]][presetcar.roadid[i+1]].crossid;
		orientation = CrossGraphMap[formCross][nextCross].orientation;
		Cross[formCross].volume += presetFactor+car.hasPriority*priorityFactor;
		Cross[nextCross].volume += presetFactor+car.hasPriority*priorityFactor;
		Road[presetcar.roadid[i]].volume[orientation] += presetFactor+car.hasPriority*priorityFactor;
		formCross=nextCross; 
	}
	nextCross = car.to;
	orientation = CrossGraphMap[formCross][nextCross].orientation;
	
	Cross[formCross].volume += presetFactor+car.hasPriority*priorityFactor;
	Cross[car.to].volume += presetFactor+car.hasPriority*priorityFactor;
	Road[presetcar.roadid[presetcar.roadid.size()-1]].volume[orientation]+=presetFactor+car.hasPriority*priorityFactor;
}

void Process() {
	bool trigger = false;
	std::vector<double> t(6000,0);
	for (int i = 0; i < CarSize; i++) {
		if (( i > triggerTime ) && (i % deltaTime == 0)) trigger = true;
		else trigger = false;
		int time=Car[i].plantime;
//		if((maxPresetCarPlanTime+1000)>time) carFlowSliceSize=40;
		if(!Car[i].isPreset){
			PlanCar(Car[i], Plan[i], trigger);
			while((t[time]+1)>carFlowSliceSize){
				time++;
			} 
			Plan[i].starttime =time;
		} 
		else{
			while((t[time]+1)>carFlowSliceSize){
				t[time++]+=0.5;
			}
			PlanPresetCar(Car[i],PresetCar[PresetCarMap[Car[i].id]]);
		}
		t[time]++;
	}
}

//int main(int argc, char *argv[])
//{
//	//	  sgenrand((unsigned)time(NULL));//initialize RNG 
//	std::cout << "Begin" << std::endl;
//
//	if(argc < 6){
//		std::cout << "please input args: carPath, roadPath, crossPath, answerPath" << std::endl;
//		exit(1);
//	}
//
//	std::string carPath(argv[1]);
//	std::string roadPath(argv[2]);
//	std::string crossPath(argv[3]);
//	std::string presetAnswerPath(argv[4]);
//	std::string answerPath(argv[5]);
//
//	std::cout << "carPath is " << carPath << std::endl;
//	std::cout << "roadPath is " << roadPath << std::endl;
//	std::cout << "crossPath is " << crossPath << std::endl;
//	std::cout << "presetAnswerPath is " << presetAnswerPath << std::endl;
//	std::cout << "answerPath is " << answerPath << std::endl;
//
//	Input(carPath, roadPath, crossPath, presetAnswerPath);
//	Process();
//	Output(answerPath);
//	return 0;
//}
int main(){
	Input("car.txt","road.txt","cross.txt","presetAnswer.txt");
	Process();
	Output("answer.txt");
	return 0;
}
