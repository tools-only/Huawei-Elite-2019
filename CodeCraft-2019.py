# -*- coding: utf-8 -*
import logging
import sys
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
    # def __init__(self, road_id, length, speed_limit, lanes, start_point, end_point, isDuplex):
    def __init__(self, road_id, length, start_point, end_point):
        self.id = road_id
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
                self.adj[road['to']].add(Road(road['id'], road['length'], road['to'], road['from']))
            self.adj[road['from']].add(Road(road['id'], road['length'], road['from'], road['to']))

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
        for road in self.adj[start]:   # 将起点的所有一阶邻居(Road对象)加入优先队列
            pq.put(road)

        while pq:
            road = pq.get() # 取队列里最短的路径(start-end)

            if road.end_point in visited: # 如果该end_point被标记
                continue
            # 更新从源点到该点的最短路径长度
            sp[road.end_point] = [road.start_point, road.id]
            # 如果到达终点
            if road.end_point == end:
                path = dict()
                path_road = dict()
                path_end = end
                while path_end != start: # 反向计算最短路径和路径长度
                    path[path_end] = sp[path_end][1]
                    path_end = sp[path_end][0]
                path = list(path.values())
                path.reverse()
                # print(path)
                return path

            for n in self.adj[road.end_point]: # 将该end-point的一阶邻居加入优先队列
                if n not in visited:
                    pq.put(n)
            visited.add(road.end_point) 

        print('Error')
        return dict()

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
    graph = Graph(roads)  
    with open(answer_path, 'w') as f:
        f.write("#(carId,StartTime,RoadId...)\n")
    # sort_by = ['planTime', 'from']
    # cars.sort_values(by=sort_by, inplace=True)

    
    answer = list()
    # print(answer[0])
    for row in cars.iterrows():
        # if row[1]['id'] == 10000:
        single_answer = list()
        path = graph.Shortest_path(row[1]['from'], row[1]['to'])
        single_answer.append(row[1]['id'])
        single_answer.append(row[1]['planTime'])
        for i in path:
            single_answer.append(i)
        single_answer = tuple(single_answer)
        # answer.append(single_answer)
        # print(answer)
        answer = str(single_answer)
        ans_str = answer.replace("[","(").replace("],",")\n").replace("((","(").replace("]]",")\n").replace(" ", '')
        # answer = np.concatenate(([answer_head],answer))
        with open(answer_path, 'a') as f:
            f.write(ans_str+'\n')
    # np.savetxt(answer_path, [ans_str], fmt = '%s',delimiter=',')

# to read input file
# process
# to write output file


if __name__ == "__main__":
    main()