# Huawei-Elite-2019
## - 想吃热干面(是真的想吃   
### 3月26日：  
网络流量分析图(x:当前待放置的车辆编号, y:当前时刻网络中的存活车辆数)  
![当前调度策略下，网络整体流量的估计值](https://github.com/tools-only/Huawei-Elite-2019/blob/master/%E6%B5%81%E9%87%8F%E7%BB%9F%E8%AE%A1%E5%88%86%E6%9E%90/%E7%BD%91%E7%BB%9C%E6%B5%81%E9%87%8F%E5%88%86%E6%9E%90.png?raw=true)
局部信息：  
![当前调度策略下，网络整体流量的估计值（局部图）](https://github.com/tools-only/Huawei-Elite-2019/blob/master/%E6%B5%81%E9%87%8F%E7%BB%9F%E8%AE%A1%E5%88%86%E6%9E%90/%E7%BD%91%E7%BB%9C%E6%B5%81%E9%87%8F%E5%88%86%E6%9E%90(%E5%B1%80%E9%83%A8).png?raw=true)
### 3月25日：
黄：目前我们估计的到达每个节点的时间是用路径长度除以车辆可行驶的最大速度。这个值是理想状态下的值，是真实值的下界。
记录影响车辆到达路径中每个节点时间的因素，基于拥塞严重时与真实到达时间的偏移要大一些的假设：  
1) 时间t越大，进入的车辆越多；  
2) 车道数；  
3) t时刻当前道路上的车的数量（车的速度）。
### 3月24日：  
黄：计算机的动态路由：发送包来实时计算节点之间的cost（网络状态）。在这个场景下，cost是实时度量出来的（真实值），而不是估计的。然后用dijikstra算法计算两个点之间的最短路。如何能类比到赛题的场景下呢？  
### 3月21日： 
#### 调度demo  
![路口调度实例图示(对应DGraph.py)](https://github.com/tools-only/Huawei-Elite-2019/blob/master/%E5%88%A4%E9%A2%98%E5%99%A8/%E8%B7%AF%E5%8F%A3%E8%B0%83%E5%BA%A6%E5%AE%9E%E4%BE%8B%E5%9B%BE.png?raw=true)  
#### 一些想法的记录    
周：利用动态路由算法的思路，在对车辆进行路径规划的时候，考虑网络中道路的实时拥塞情况，用一个二维数组（由于道路可能有两个方向，若只有一个方向，则置反方向为-1）存储道路中的车辆数。此时估计的车辆数是理想情况下的，因此要小于平台调度时真实的数量。至此可以做出在每个时间步中同一批次出发的车辆比较好的路径规划（由于平台对车辆id进行排序，暂时可以默认id排序，不做planTime的规划）。另外，根据t时刻网络中车辆的总数量以及路口中的车辆数（两个值都小于真实调度情况）进行流量控制。局部的流量控制可以对路口加窗口，仿照计算机网络中的流量控制来做，以路口的阈值来延期规划路线中有该路口的车辆出发时间。   
朱：方向选择的偏好。在选择路径的时候，可以**尽量选择直行**，因为直行优先级最高，可以尽快调度出路口，而转弯容易出现等待状态从而导致死锁。此外根据规则的设定，可以尽量选择左转。（优先级排序：直行>左转>右转）  
### 3月19日：  
在.cpp文件实现了简单的车辆调度，车辆排序先按出发时间，再按ID顺序。也考虑对同一时刻出发的车辆所需要行驶的最短路径长度进行排序。  
### 3月18日：  
在py文件的基础上增加了简单的车辆调度、调整输入输出格式，后面可以考虑根据先车辆的速度、出发时间进行排序，对同一时刻出发的车辆所需要行驶的最短路径长度进行排序等。  
### 3月17日：  
定义了C++部分的基础输入类。
### 3月14日：  
定义了基础类，完成数据读取以及单源最短路径的算法部分。 
