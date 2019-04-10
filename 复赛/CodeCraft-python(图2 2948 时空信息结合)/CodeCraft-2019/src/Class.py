# -*- coding: utf-8 -*-
import numpy as np
import collections
import copy

'''
数据结构定义
'''
class Cars:
    # def __init__(self, car_id, from_id, to_id, road_id, speed, departure_time, state, dist, next_road, dir, state, plan):
    def __init__(self, car_id, from_id, to_id, speed, planTime, plan, priority, preset):
        self.id = car_id
        self.start = from_id
        self.end = to_id
        # self.road_id = road_id # 当前所处道路
        self.speed = speed
        self.planTime = planTime 
        # self.dist = dist # 与路口的距离
        # self.dir = dir # 到达路口后的方向
        # self.state = state # 当前车辆状态
        self.plan = plan
        self.priority = priority
        self.preset = preset
        # 当前车辆的规划路径，dict类型，存储第i时刻所处的道路id，同时表示当前车辆的规划路径。 
        # (补充：如果死锁，plan将一直增加)

class Road:
    '''
    lanes：车道数量
    isDuplex：是否双向（bool类型）
    '''
    # def __init__(self, road_id, length, speed_limit, lanes, start_point, end_point, isDuplex, forward_congestion, reverse_congestion):
    def __init__(self, road_id, length, speed_limit, lanes, start_point, end_point, isDuplex):
        self.id = road_id
        self.length = length
        self.speed = speed_limit
        self.lanes = lanes
        self.start = start_point
        self.end = end_point
        self.isDuplex = isDuplex
        # self.fcong = forward_congestion
        # self.rcong = reverse_congestion
        # fcong(rcong)：t时间道路拥塞情况，dict类型：rcong[t]=i, i为t时刻道路上的行驶车辆数

class Cross:
    '''
    题目里顺时针记录路口的道路，值为-1则表示丁字路口
    '''
    def __init__(self, cross_id, up, right, down, left, value):
        self.cross_id = cross_id
        self.up = up
        self.right = right
        self.down = down
        self.left = left
        # self.wait = wait # t时间步路口等待车辆数（只考虑转弯的情况？）dict类型
        self.value = value # cross.value 路口的拥挤程度

class Graph:
    def __init__(self, roads, crosses):
        '''
        adj：[cross_i] = [cross_j, road_k] 存储路口i到路口j的道路k实例
        道路正/反向表示
        '''
        self.adj = collections.defaultdict(list)
        self.road_list = dict()
        self.cross_list = dict()
        self.cross_value = dict()
        self.road_value = collections.defaultdict(np.array) # 方便后面利用广播机制
        self.cross_neibor = collections.defaultdict(np.array)
        self.road_value_t = collections.defaultdict(list)
        self.cross_value_t = collections.defaultdict(list)
        self.road_value_copy = collections.defaultdict(np.array)
        self.cross_value_copy = collections.defaultdict(np.array)
        # road拥挤度量 
        for index, road in roads.iterrows():
            self.road_list[road['id']] = Road(road['id'], road['length'], road['speed'],\
                road['channel'], road['from'], road['to'], road['isDuplex'])
            # congestion = Congestion(road['id'], 0, 0)
            if road['isDuplex'] == 1:
                self.adj[road['to']].append((road['from'], self.road_list[road['id']]))
                # self.dist[road['to']].append((road['from'], road['length']))
            self.adj[road['from']].append((road['to'], self.road_list[road['id']]))
            self.road_value[road['id']] = np.array([0., 0.]) # append进去的向量[i, j]代表当前时刻i道路上正向和反向的车辆数 float方便后面广播
            # self.road_value_copy[road['id']] = np.array([0, 0])
        # road_value时间片初始化
        self.road_value_t[1].append(self.road_value)

        # cross拥挤度量
        for index, cross in crosses.iterrows():
            cross_road = []
            cross_nb = []
            # road_cong = 0
            for i in cross[1:]:
                if i == -1:
                    cross_road.append(-1)
                    continue
                else:
                    cross_road.append(self.road_list[i])
                    # 邻居cross
                    if self.road_list[i].start == cross['id']:
                        cross_nb.append(self.road_list[i].end)
                    else:
                        cross_nb.append(self.road_list[i].start)
            self.cross_list[cross['id']] = cross_road # 存放路口处道路的实例
            self.cross_value[cross['id']] = 0. # 初始化cross_value
            self.cross_neibor[cross['id']] = cross_nb
            # self.cross_value_copy[cross['id']] = 0
        # cross_value时间片初始化
        self.cross_value_t[1].append(self.cross_value)