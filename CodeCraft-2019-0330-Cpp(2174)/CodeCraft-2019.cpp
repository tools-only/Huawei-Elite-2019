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
	int time;//计划出发时间
};
struct ROAD{
	int   id;
	int   length;
	int   speed;
	int   channel;
	int   from;
	int   to;
	bool  isDuplex;
	int   time;//一个变量
	int   volume[2];//正反向流量
};
struct CROSS{
	int id;
	int roadid[4];//连4条马路
	int crossid[4];//连的4个路口
	int volume;//流量
};
struct CROSSGRAPHUNIT{//有些文件里是CROSSMAPUNIT,Cross寻找Road的映射(含方向).
	int       roadid=-1;
	int    distance=INF;
	int   orientation=0;//0,Road正向;1,Road逆向.正向值
	int         value=0;
};
struct  ROADGRAPHUNIT{//闲置的类,本意是做一个Road寻找Cross的映射.
	int      crossid=-1;
	int   orientation=1;//3:Direct;2:Left;1:Right;
};
struct PLAN{
	int id;
	int starttime;
	std::vector<int> roadid;
};

int                      deltaTime=700;//700辆车之后开始按/carFlowSliceSize(75)安排.
const int              triggerTime=500;//触发时间(指多少辆车之后Cross[].volume和Road[].volume开始挥发)
const int          carFlowSliceSize=75;//直接在时间上加的数字,训练赛36.

const double              phoCross=0.2;//Cross[].volume的挥发率.
const double               phoRoad=0.2;//Road[].volume的挥发率.

std::map<int,int>               CarMap;//映射比如第2两辆输出的车Car.id是13879,CarMap[13879]=2-1;
std::map<int,int>              RoadMap;//类似
std::map<int,int>             CrossMap;//类似
				              
int                            CarSize;//PlanSize;
int                           RoadSize;//
int                          CrossSize;//GraphSize

std::vector<CAR>                         Car;//Car
std::vector<ROAD>                       Road;
std::vector<CROSS>                     Cross;
std::vector<PLAN>                       Plan;//这个是可以优化掉的Plan类.

std::vector<std::vector<CROSSGRAPHUNIT> >   CrossGraph;//牛逼哄哄的映射表.

//字符串处理工具函数.C++里你要自己写的你敢信?不过我是baidu来的.
void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c){
	std::string::size_type pos1, pos2;
	pos2 = s.find(c);pos1 = 0;
	while(std::string::npos != pos2){
		v.push_back(s.substr(pos1, pos2-pos1));
    	pos1 = pos2 + c.size();pos2 = s.find(c, pos1);
  	}
  	if(pos1 != s.length()) v.push_back(s.substr(pos1));
}
//一个之前很长的判断方向的工具函数.
void MakeRoadOrientation(int roadid, int crossin, int crossout,bool isD){
	int i,j;
	for(i=0;i<4;i++) if(Cross[crossin].roadid[i]==roadid) break;//find the Road[roadid] index in Cross[crossin] 
	Cross[crossin].crossid[i]=crossout;
	for(i=0;i<4;i++)  if(Cross[crossout].roadid[i]==roadid) break;//find the Road[roadid] index in Cross[crossout]
	if(isD) Cross[crossout].crossid[i]=crossin;
}
//处理输入的函数.
void Input(const std::string &Carfile,const std::string &Roadfile,const std::string &Crossfile){
	std::string tmps;
	std::ifstream IN(Carfile);
	
	while(!IN.eof()){
		std::getline(IN,tmps);
		if(tmps.at(0)!='#'){//[Error] ISO C++ forbids comparison between pointer and integer [-fpermissive]
			tmps=tmps.substr(1,tmps.size()-2); 
			std::vector<std::string> data;
			SplitString(tmps,data,",");
			
			CAR tmpcar={std::stoi(data[0]),std::stoi(data[1]),std::stoi(data[2]),std::stoi(data[3]),std::stoi(data[4])};
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
			ROAD tmproad={std::stoi(data[0]),std::stoi(data[1]),std::stoi(data[2]),std::stoi(data[3]),std::stoi(data[4]),std::stoi(data[5]),
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
						   {RoadMap[std::stoi(data[1])],RoadMap[std::stoi(data[2])],RoadMap[std::stoi(data[3])],
						    RoadMap[std::stoi(data[4])]},{-1,-1,-1,-1},1};
			Cross.push_back(tmpcross);
		    CrossMap.insert(std::pair<int,int>(tmpcross.id,CrossSize++));
		}
	}
	IN.close();
		
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
	
	for(int i=0;i<CarSize;i++){
		Car[i].from=CrossMap[Car[i].from];
		Car[i].to=CrossMap[Car[i].to];
	}
}
//可以和Plam类一起删除的输出函数.
void Output(std::string answerfile){
	std::ofstream OUT(answerfile);
	OUT<<"#(carId,StartTime,RoadId...)\n";
	for(int i=0;i<CarSize;i++){
		OUT<<"("<<Plan[i].id<<","<<Plan[i].starttime;
		for(auto it=Plan[i].roadid.begin();it!=Plan[i].roadid.end();it++)
			OUT<<","<<Road[(*it)].id;
		OUT<<")\n";
	}
	OUT.close();
}
//对car排序的比较函数,先比出发时间,再比id大小,这个可以跳过.
bool cmpCar(CAR &a,CAR &b){
	return (a.time==b.time)?a.id<b.id:a.time<b.time;
}
//也许是很牛批能把我们送进复赛的函数.
void PlanCar(CAR &car,PLAN &plan,bool trigger){
	std::vector<int>  pathCross(CrossSize,-1);//记录更新某个Cross的前一个Crossid(程序内部id).
	std::vector<int>      dist(CrossSize,INF);//记录权值函数的累加.
	std::vector<bool>    flag(CrossSize,true);//是否被访问的标志.
	int      source=car.from,destation=car.to;//
	//计算车经过，,
	for(int i=0;i<RoadSize;i++){
		Road[i].time=std::ceil(1.0*Road[i].length/std::min(Road[i].speed,car.speed));
	}
	//触发了trigger=true的话挥发,算是挽回一点点累加的消极影响吧.
	if(trigger){
		for(int i=0;i<RoadSize;i++){
			Road[i].volume[0]*=(1-phoRoad);//正向
			if(Road[i].volume[0]==0) Road[i].volume[0]=1;
			Road[i].volume[1]*=(1-phoRoad);//反向
			if(Road[i].volume[1]==0) Road[i].volume[1]=1;
		}
		for(int i=0;i<CrossSize;i++){	
			Cross[i].volume*=(1-phoCross);
			if(Cross[i].volume==0) Cross[i].volume=1;
		}
	}
	//更新source出来的Cross.可优化,直接从Cross[].crossid[]里读.
	for(int i=0;i<CrossSize;i++)
		if(CrossGraph[source][i].roadid!=-1){
			//核心科技,热干面出品.
		    dist[i]=Road[CrossGraph[source][i].roadid].time+
		            Cross[i].volume/Road[CrossGraph[source][i].roadid].channel*
				    Road[CrossGraph[source][i].roadid].volume[CrossGraph[source][i].orientation]/Road[CrossGraph[source][i].roadid].channel;
			pathCross[i]=source;
		}
	dist[source]=0;
	flag[source]=false;
	//注意,要开始了.
	for(int k=0;k<CrossSize;k++){
		int minValue=INF,minCrossid=-1;
		//找个dist[]值最低没访问过的路口.
		for(int i=0;i<CrossSize;i++){
 			if((flag[i])&&(minValue>dist[i])){
	 			minValue=dist[i];
				minCrossid=i;
			}
	    }
	    if(minCrossid==-1) break;//not connected;
		flag[minCrossid]=false;
		//路口id为minCross,以这个路口来更新其他路口.
		for(int i=0;i<CrossSize;i++){
			if(flag[i]){
				int roadid=CrossGraph[minCrossid][i].roadid;
				if(roadid!=-1){
					//判断方向需要两个路roadidp是前一个路口.
					int roadidp=CrossGraph[pathCross[minCrossid]][minCrossid].roadid;
					int orientation=CrossGraph[minCrossid][i].orientation;
					int deltadist=Road[roadid].time+Road[roadid].volume[orientation]/Road[roadid].channel*Cross[i].volume/Road[roadid].channel;
					if(dist[i]>(dist[minCrossid]+deltadist)){
						dist[i]=dist[minCrossid]+deltadist;
						pathCross[i]=minCrossid;
					}
				}
			}
		}
	}
	//根据记录的pathCross,给流量加值.
	while(pathCross[destation]!=-1){
		int roadid=CrossGraph[pathCross[destation]][destation].roadid;
		int orientation=CrossGraph[pathCross[destation]][destation].orientation;
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
	bool trigger=false;
	sort(Car.begin(),Car.end(),cmpCar);
	
	for(int i=0;i<CarSize;i++){
		//如果满足触发时间或者刚开始发了足够多的车(triggerTime改成triggerCarNumber比较好).
		if((i>triggerTime)&&(i%deltaTime==0)) trigger=true;
		else trigger=false;
		Cat[i].time+=i/carFlowSliceSize;
		PlanCar(Car[i],Plan[i],trigger);
	}
}
//这部分是上交时候的入口.
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
//	std::cout << "carPath is "    << carPath    << std::endl;
//	std::cout << "roadPath is "   << roadPath   << std::endl;
//	std::cout << "crossPath is "  << crossPath  << std::endl;
//	std::cout << "answerPath is " << answerPath << std::endl;
//	
//	Input(carPath,roadPath,crossPath);
//	Process();
//	Output(answerPath);
//  return 0; 
//}
int main(){
	Input("car.txt","road.txt","cross.txt");
	Process();
	Output("answer.txt");
	return 0;
}