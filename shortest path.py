# -*-conding=utf-8-*-
import pandas as pd
import numpy as np
import collections
from queue import PriorityQueue

def load_data(file_path, file_name):
    data = pd.read_csv(file_path + file_name + '.txt')
    if file_name == 'crossroad':
        data.columns = ['id', 'up', 'right', 'down', 'left']
    else:
        data.columns = data.columns.str.strip('#?(?)?')

    data[data.columns[0]] = data[data.columns[0]].str.strip('(').apply(pd.to_numeric)
    data[data.columns[-1]] = data[data.columns[-1]].str.strip(')').apply(pd.to_numeric)
    return data

class Road:
    '''
    lanes：车道数量
    isDuplex：是否双向（bool类型）
    '''
    # def __init__(self, road_id, length, speed_limit, lanes, start_point, end_point, isDuplex):
    def __init__(self, length, start_point, end_point):
        # self.id = road_id
        self.length = length
        # self.speed_limit = speed_limit
        # self.lanes = lanes
        self.start_point = start_point
        self.end_point = end_point
        # self.isDuplex = isDuplex
    def __lt__(self, x):
        # less than函数
        return self.length <= x.length

class Cars:
    def __init__(self, car_id, from_id, to_id, speed, departure_time):
        self.car_id = car_id
        self.from_id = from_id
        self.to_id = to_id
        self.speed = speed
        self.departure_time = departure_time

class Crossroad:
    '''
    题目里顺时针记录路口的道路，值为-1则表示丁字路口
    '''
    def __init__(self, cross_id, up, right, down, left):
        self.cross_id = cross_id
        self.up = up
        self.right = right
        self.down = down
        self.left = left

class Graph:
    """docstring for Graph"""
    def __init__(self, roads):
        '''
        根据roads信息构造graph：
        道路的数据表示为：（id，道路长度，最高限速，车道数目，起点id，终点id，是否双向）
        根据道路的起点和终点以及道路长度构造邻接矩阵
        '''
        self.adj = collections.defaultdict(set) 
        for _, road in roads.iterrows():
            if road['isDuplex'] == 1: # 双向道路
                self.adj[road['to']].add(Road(road['to'],road['from'],road['length']))
                # self.adj[road['to']].add(Road(road['length'],road['to'],road['from']))
            self.adj[road['from']].add(Road(road['from'],road['to'],road['length']))
   
    def Shortest_path(self, start, end):
        '''
        Dijkstra算法求最短路径
        start：起点 end：终点
        '''
        sp = dict() # 最短路径
        sp[start] = start
        
        visited = set()
        visited.add(start)
        
        pq = PriorityQueue(100) # 优先队列
        for road in self.adj[start]:   # 将起点的所有一阶邻居加入优先队列
            pq.put(road)

        while pq:
            road = pq.get() # 取队列里最短的路径(start-end)
            
            if road.end_point in visited: # 如果该end_point被标记
                continue
            # 更新从源点到该点的最短路径长度
            sp[road.end_point] = road.start_point       
            # 如果到达终点
            if road.end_point == end:
                path = dict()
                path_end = end
                while path_end != start: # 反向计算最短路径和路径长度
                    path[path_end] = sp[path_end]
                    path_end = sp[path_end]
                return path

            for n in self.adj[road.end_point]: # 将该end-point的一阶邻居加入优先队列
                if n not in visited:
                    pq.put(n)
            visited.add(road.end_point) 

        print('Error')
        return dict()


if __name__ == '__main__':
    file_path = 'config/'
    file_name = 'road' # car, cross
    roads = load_data(file_path, file_name)
    graph = Graph(roads)
    shortest_path = graph.Shortest_path(1,2)
    print(shortest_path)
