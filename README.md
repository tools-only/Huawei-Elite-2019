# Huawei-Elite-2019
## 想吃热干面(是真的想吃  

虽然纽约时间比加州时间快三个小时，但加州时间并没有变慢，感谢队友@din4e、@CherryHuna和这段经历:clap::clap::clap:，继续:running::running::running:

--------------------------------------------------------------------------------------
### 4月13日：  
**复赛成绩**  
![复赛成绩](https://github.com/tools-only/Huawei-Elite-2019/blob/master/%E5%A4%8D%E8%B5%9B/%E5%A4%8D%E8%B5%9B%E6%88%90%E7%BB%A9.png?raw=true)  
总结：准备不够充分，加上时间有些短，训练赛最好版本模型没能调好，最后仓促传了另一版没做数据拟合的模型。
### 4月9日：  
昨天，用了一下判题器发现，之前的流量优化（算法写错了）没做好。线性的放入车辆确实阻碍了成绩的提升（优化空间大）。我们的路径安排算法还是很合理的。

---

### 3月31日：  
**相关博客**  
[交通分配与复杂网络分析](https://blog.csdn.net/LZX19901012)  
**初赛成绩**  
感谢女神和星座！  
![初赛成绩](https://github.com/tools-only/Huawei-Elite-2019/blob/master/%E5%88%9D%E8%B5%9B/%E5%88%9D%E8%B5%9B%E6%88%90%E7%BB%A9.png?raw=true)  

### 3月30日：  
**初赛**  
发现路径time_cost+cross_value \* road_value比三项相乘结果要好，记录几组较好的参数：  
1. delta_time=695，挥发率=0.80，流量进入间隔=75 ==> map1:1075，map2:1067  
2. 挥发率=0.80，流量进入间隔=75 ==> map1:1103，map2:1075  
3. 挥发率=0.80，流量进入间隔=70 ==> map1:1139，map2:1154   

**如果有复赛(有了！**  
1. 分析一下权重公式原因，需要数据可视化组件；  
2. 周的前期后期流量优化考虑一下，及优化数据结构；  
3. 想看一下，先把车放到每个Cross上，然后按Cross[].id把车放入地图是否有效果；  
4. 强化学习+判题器的方法调参不是不可以。  
5. 朱：对时间片算法持否认态度。  
### 3月29日：
删除了一些错误的图  
网络流量图：  
![网络流量图](https://github.com/tools-only/Huawei-Elite-2019/blob/master/%E5%88%9D%E8%B5%9B/%E6%B5%81%E9%87%8F%E7%BB%9F%E8%AE%A1%E5%88%86%E6%9E%90/%E7%BD%91%E7%BB%9C%E6%B5%81%E9%87%8F%E5%9B%BE.png?raw=true)

### 3月28日：  
发现权重累加反而效果更好，让路径尽可能均匀分布，然后调整进入网络的流量或许是个办法。  
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
