# -*- coding: utf-8 -*-
import numpy as np
import math
'''
对于交通网络状态的分析：
1. t时刻网络中的"存活车辆"估计;
2. t时刻网络中负载较重的道路或路口估计。
3. 分析当前车辆是否能够进入网络。若不能，应调整出发时间，让后面的车先调度。
'''
def traffic_analysis(graph, t):
    # 统计t时刻整个网络中的"存活车辆"
    fcong = sum(graph.road_value_t[t][0].values())[0] 
    rcong = sum(graph.road_value_t[t][0].values())[1]
    total_car_num = fcong + rcong
    # print('sum(graph.cong[t][0].values(), [])', sum(graph.cong[t][0].values(), []))
    # print(graph.cong[t][0].values())
    return fcong, rcong, total_car_num
