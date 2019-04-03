# -*- coding: utf-8 -*-
import collections
'''
数据结构定义
'''
class Cars:
    # def __init__(self, car_id, from_id, to_id, road_id, speed, departure_time, state, dist, next_road, dir, state, plan):
    def __init__(self, car_id, from_id, to_id, speed, planTime, plan):
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
    def __init__(self, cross_id, up, right, down, left, wait):
        self.cross_id = cross_id
        self.up = up
        self.right = right
        self.down = down
        self.left = left
        self.wait = wait # t时间步路口等待车辆数（只考虑转弯的情况？）dict类型

class Graph:
    def __init__(self, roads):
        '''
        adj：[cross_i] = [cross_j, road_k] 存储路口i到路口j的道路k实例
        道路正/反向表示
        '''
        self.adj = collections.defaultdict(list)
        self.cong_slice = collections.defaultdict(list)
        self.cong = collections.defaultdict(list)

        for _, road in roads.iterrows():
            road_object = Road(road['id'], road['length'], road['speed'],\
                road['channel'], road['from'], road['to'], road['isDuplex'])
            # congestion = Congestion(road['id'], 0, 0)
            if road['isDuplex'] == 1:
                self.adj[road['to']].append((road['from'], road_object))
                # self.dist[road['to']].append((road['from'], road['length']))
            self.adj[road['from']].append((road['to'], road_object))
            # self.dist[road['from']].append((road['to'], road['length']))
            self.cong_slice[road['id']].append([0, 0]) # append进去的向量[i, j]代表当前时刻i道路上正向和反向的车辆数
            # self.cong_slice[road['id']].append([[Cars], [Cars]]) # Cars代表在某个时刻i道路上运行了哪些车辆 
        
        self.cong[0].append(self.cong_slice)
        
        # 对各个时间片进行初始化初始化(待改进)
        for i in range(1, 2000):
            self.cong[i].append(self.cong_slice)
        
