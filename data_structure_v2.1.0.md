[TOC]
# 类定义
##  基础信息类
### 车的基础信息
```c++
struct INFO_CAR {
    int                  id;
	int                from;
	int                  to;
	int               speed;
	int            plantime;
	bool        hasPriority;
	bool           isPreset;
};
```
### 马路的基础信息
+ time是一个变量；
+ volume[2]保存流量，之前的roadvalue。
```c++
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
```
### 路口的基础信息
+ 同理，volume为之前crossvalue；
```c++
struct INFO_CROSS {
	int                  id;  
	int           roadid[4]; 
	int          crossid[4];
	int              volume;
};
```
### 车的规划的基础信息
```c++
+ 主要是roadid[]这个可变数组，保存车辆的路径信息；
##PresetCar直接存入PLAN类##
struct INFO_PLAN {
	int                  id;
	bool        hasPriority;
	bool           isPreset;
	int           starttime;
	std::vector<int> roadid;
};
```
### 预设车的基础信息
+ 这个类，暂时不需要，莫得用处；
+ INFO_PLAN和INFO_CAR类基本就可以刻画了，下面有个映射表PresetCarMap用来访问保存在Car[]数组中的PresetCar；
```c++
//struct INFO_PRESETCAR{
//	int                  id;
//	std::vector<int> roadid; 
//};
```
### 路口映射基本单元
```c++
struct CROSSMAPUNIT {
	int         roadid = -1;
	int      distance = INF;
	int     orientation = 0;//路口的方向，正反。正为0，反为1。
	int           value = 0;
};
```
### 马路映射基础单元
+ 可以存方向；
```c++
struct  ROADMAPUNIT {
	int        crossid = -1;
	int     orientation = 1;//3:Direct;2:Left;1:Right;
};
```
##  仿真类
+ 待完成。
```c++
struct SIMULATION_CAR{
	int channel;
	int posation;
	int state;
};
```
```c++
struct SIMULATION_ROAD{
	std::vector<std::queue<int> > channelqueue[2];
}; 
```
```c++
struct SIMULATION_CROSS{
	std::queue<int> waitarea[4];
};
```
# 变量申明
### 基本常量
```C++
int                       deltaTime = 400;//刚开始多放一点车；
const int               triggerTime = 500;//每个多少辆车挥发流量；
const int          carFlowSliceSize =  36;//没次放多少量车加出发时间；

const double               phoCross = 0.2;//挥发率；
const double                phoRoad = 0.2;
```
### 映射表 
```c++
std::map<int, int>                 CarMap;
std::map<int, int>           PresetCarMap;//有可能是多余或没法使用的；
std::map<int, int>     HighPriorityCarMap;//同理;
std::map<int, int>                RoadMap;
std::map<int, int>               CrossMap;
```
### 所有的基本信息及其SIZE
+ SIZE有点重复，完全可以用```class.size()```，但是我喜欢用:smiling_imp:（咬我呀）；
```c++
int                               CarSize;//PlanSize; 
int                         PresetCarSize;//
int                   HighPriorityCarSize;//
int                              RoadSize;//
int                             CrossSize;//GraphSize

std::vector<INFO_CAR>                         Car;
std::vector<INFO_ROAD>                       Road;
std::vector<INFO_CROSS>                     Cross;
std::vector<INFO_PLAN>                       Plan;//PresetCar的路径直接写入；
```
```c++
std::vector<std::vector<CROSSMAPUNIT> >   CrossGraphMap;//根据Cross,寻找Road.
std::vector<std::vector<ROADMAPUNIT> >     RoadGraphMap;//
```
### 仿真相关 
+ 待完成。

# 变更记录
+ 版本号2表示复赛，版本1.0；

