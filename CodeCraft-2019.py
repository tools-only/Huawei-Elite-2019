# -*- coding: utf-8 -*
import logging
import sys
import math
import random
import pandas as pd
import numpy as np
import collections
from queue import PriorityQueue

logging.basicConfig(level=logging.DEBUG,
                    filename='../logs/CodeCraft-2019.log',
                    format='[%(asctime)s] %(levelname)s [%(funcName)s: %(filename)s, %(lineno)d] %(message)s',
                    datefmt='%Y-%m-%d %H:%M:%S',
                    filemode='a')

class Road:
    '''
    lanes：车道数量
    isDuplex：是否双向（bool类型）
    '''
    def __init__(self, road_id, length, speed_limit, lanes, start_point, end_point, isDuplex, congestion):
    # def __init__(self, road_id, length, start_point, end_point):
        self.id = road_id
        self.length = length
        self.speed_limit = speed_limit
        self.lanes = lanes
        self.start_point = start_point
        self.end_point = end_point
        self.isDuplex = isDuplex
        self.cong = congestion 
        # t时间道路拥塞情况，dict类型，存储数据为二维向量[i, j]：i为正向车辆数，j为逆向车辆数

class Cars:
    # def __init__(self, car_id, from_id, to_id, road_id, speed, departure_time, state, dist, next_road, dir, state, plan):
    def __init__(self, car_id, from_id, to_id, speed, departure_time, plan)
        self.id = car_id
        self.start = from_id
        self.end = to_id
        # self.road_id = road_id # 当前所处道路
        self.speed = speed
        self.departure_time = departure_time 
        # self.dist = dist # 与路口的距离
        # self.dir = dir # 到达路口后的方向
        # self.state = state # 当前车辆状态
        self.plan = plan 
        # 当前车辆的规划路径，dict类型，存储第i时刻所处的道路id，同时表示当前车辆的规划路径。 
        # (补充：如果死锁，plan将一直增加)

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
    """docstring for Graph"""
    def __init__(self, roads, crosses):
        '''
        根据roads信息构造graph：
        道路的数据表示为：（id，道路长度，最高限速，车道数目，起点id，终点id，是否双向）
        根据道路的起点和终点以及道路长度构造邻接矩阵
        '''
        self.adj = collections.defaultdict(set)
        self.road_list = collections.defaultdict(set)
        # for index, cross in crosses.iterrows():
        #     self.crs[index+1].append(Cross(cross['id'], cross['up'], cross['right'], \
        #      cross['down'], cross['left'], {}))
        for _, road in roads.iterrows():
            if road['isDuplex'] == 1: # 双向道路
                road_object = Road(road['id'], road['length'], road['speed'],\
                 road['channel'], road['to'], road['from'], road['isDuplex'], collections.defaultdict(set))
                self.adj[road['to']].add(road_object)
                # self.road_list[road['id']].add(road_object)
            # adj[road_i]保存从i号路口出发能够到达的路口 一阶邻居
            road_object = Road(road['id'], road['length'], road['speed'], \
                road['channel'], road['from'], road['to'], road['isDuplex'], collections.defaultdict(set))
            self.adj[road['from']].add(road_object)
            self.road_list[road['id']].add(road_object)

    def update(self, road_list, car_list, t):
        # 遍历一遍所有已调度的车辆, 更新在t时刻网络中道路的拥塞值
        for index, car in enumerate(car_list):
            if t > max(car.plan): # 车辆已跑完全程
                continue
            road_list[car.plan[t]].cong[t][0] += 1 # 判断正反


        road.cong[t] = 

    def plan_path(self, car_list, i):
        '''
        对第i车辆进行路径规划
        '''
        car = car_list[i]
        start = car.start
        end = car.end
        t = car.planTime
        path = collections.OrderedDict() # 规划的路径
        visited = set()
        visited.add(start)
        # 判断网络中的车辆数量，如果超过一定阈值，则planTime+1
        # 路径规划的时候，需要实时更新网络中道路的权重
        for road in self.adj[start]:
            # alpha为拥塞因子，path存储道路当前的权重（扩展出直行的权重体现）
            path[road.id] = [road.length/min(road.speed, car.speed), \
             math.ceil(float(road.length)/min(road.speed, car.speed))] if road.cong[t][0]\
             else [road.length/min(road.speed, car.speed)+alpha*(road.cong[t][0]/road.lanes), \
             math.ceil(float(road.length)/min(road.speed, car.speed))] # 是否有正反之分-ok       
        while path:
            cur_road = min(path, key=path.get) # 取path中第一维值最小的索引(即road_id)
            if road_list[cur_road].start in visited: # 如果该道路的start路口被标记
                continue
            # car.plan[t].add(cur_path) # 将当前最优路径进入到car.plan中
            # 对道路的拥塞值进行更新
            if cur_road == road_list[cur_road].to: 
                road_list[cur_road].cong[t][0] += 1 # 正向道路更新
            else:
                road_list[cur_road].cong[t][1] += 1 # 反向道路更新
            # 车辆需要k个时间步走完当前道路(实际停留k-1个时间步)，则保持此期间车辆的plan路径状态
            k = path[cur_road][1] - 1
            for i in range(k):
                car.plan[t+i].add(cur_road)      
            # 暂时放弃 车辆在最高优先级状态下走出当前道路后，更新网络车辆状态
            if road_list[cur_road].end == end: # 找到终点则返回路径
                path = list(set(path.values()))[::-1]
                return path

            t += k 
            '''
             对于较前批次的车而言，此时的t时间步可能道路的congestion并没有数据，需要写一个简单的判断语句
             如果没有数据则权重为0
            '''
            next_start = road_list[cur_road].end
            for n in self.adj[next_start]:
                if n.start not in visited:
                    path[n.id] = [n.length/min(n.speed, car.speed), \
             math.ceil(float(n.length)/min(n.speed, car.speed))] if n.cong[t][0] \
             else [n.length/min(n.speed, car.speed)+alpha*(n.cong[t][0]/n.lanes), \
             math.ceil(float(n.length)/min(n.speed, car.speed))]

            visited.add(road_list[cur_road].start)
            # self.update(road_list, car_list, t)

    # def plan_path(self, start, end, speed, t):
    #     '''
    #     Dijkstra算法求最短路径
    #     start：起点 end：终点 t：当前调度车辆所处的时间步
    #     '''
    #     sp = dict() # 最短路径
    #     sp[start] = start        
    #     visited = set()
    #     visited.add(start)        
    #     # pq = PriorityQueue(100) # 优先队列
    #     # 在t时间步每条道路的拥塞情况
    #     if car.id == 0 or Graph.num == 0: # 如果是第一辆车或网络中已经没有车了
    #         # 迪杰斯特拉走最短路径 并更新道路拥塞、cross拥塞、网络拥塞
    #     else：
    #         #     

    #     for road in self.adj[start]:  # 将起点的所有一阶邻居(Road对象)加入优先队列
    #         road /= (min(road.speed_limit, speed) * road.lanes) # 按运行时间排序
    #         pq.put(road)
    #     while pq:
    #         road = pq.get()
    #         if road.end_point in visited: # 如果该end_point被标记
    #             continue
    #         # 更新从源点到该点的最短路径长度
    #         sp[road.end_point] = [road.start_point, road.id]
    #         # 如果到达终点
    #         if road.end_point == end:
    #             path = collections.OrderedDict()
    #             # path_road = dict()
    #             path_end = end
    #             while path_end != start: # 反向计算最短路径和路径长度
    #                 path[path_end] = sp[path_end][1]
    #                 path_end = sp[path_end][0]
    #             path = list(path.values())
    #             path.reverse()

    #             return path

    #         for n in self.adj[road.end_point]: # 将该end-point的一阶邻居加入优先队列
    #             if n not in visited:
    #                 n.length /= min(n.speed_limit, speed)
    #                 pq.put(n)
    #         visited.add(road.end_point) 
    #     print('Error')
    #     return dict()

def sort_speed(cars):
    # 车辆按出发时间进行排序
    sorted_by = ['planTime', 'id']
    cars.sort_values(sorted_by, inplace=True)
    # 对同时出发的车辆，按速度进行降序排序
    sorted_cars = cars[cars.planTime == 1].sort_values('speed', ascending=False)
    for i in cars.planTime.unique()[1:]:
        item = cars[cars.planTime == i].sort_values('speed', ascending=False)
        sorted_cars = pd.concat([sorted_cars, item], axis=0)

    return sorted_cars 

def load_data(file_path):
    data = pd.read_csv(file_path)
    file_name = file_path.strip().split('/')[-1].split('.')[0]
    if file_name == 'cross':
        data.columns = ['id', 'up', 'right', 'down', 'left']
    else:
        data.columns = data.columns.str.strip('#?(?)?')

    data[data.columns[0]] = data[data.columns[0]].str.lstrip('(').apply(pd.to_numeric)
    data[data.columns[-1]] = data[data.columns[-1]].str.rstrip(')').apply(pd.to_numeric)
    return data

def main():
    if len(sys.argv) != 5:
        logging.info('please input args: car_path, road_path, cross_path, answerPath')
        exit(1)

    car_path = sys.argv[1]
    road_path = sys.argv[2]
    cross_path = sys.argv[3]
    answer_path = sys.argv[4]

    logging.info("car_path is %s" % (car_path))
    logging.info("road_path is %s" % (road_path))
    logging.info("cross_path is %s" % (cross_path))
    logging.info("answer_path is %s" % (answer_path))

    roads = load_data(road_path)
    cars = load_data(car_path)
    crosses = load_data(cross_path)
    graph = Graph(roads, crosses)
    # print(roads.loc[roads.id==5091])
    # cars = sort_speed(cars)
    with open(answer_path, 'w') as f:
        f.write("#(carId,StartTime,RoadId...)\n")
    # sort_by = ['planTime', 'from']
    # cars.sort_values(by=sort_by, inplace=True)
    path_dict = {}
    cur_carlist = {}
    for index,row in enumerate(cars.iterrows()):
        single_answer = list()
        cur_carlist[index] = Car(row[1]['id'], row[1]['from'], row[1]['to'], row[1]['speed'],\
         row[1]['departure_time'], collections.defaultdict(set))
        path = graph.plan_path(cur_carlist, index) # 对当前车辆进行路径规划
        single_answer.append(row[1]['id'])
        single_answer.append(row[1]['planTime']+int(index/10))
        for i in path:
            single_answer.append(i)
        single_answer = tuple(single_answer)
        answer = str(single_answer)
        ans_str = answer.replace("[","(").replace("],",")\n").replace("((","(").\
        replace("]]",")\n").replace(" ", '')
        with open(answer_path, 'a') as f:
            f.write(ans_str+'\n')

# to read input file
# process
# to write output file
if __name__ == "__main__":
    main()
