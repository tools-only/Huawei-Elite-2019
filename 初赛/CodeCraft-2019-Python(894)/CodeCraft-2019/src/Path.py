# -*- coding: utf-8 -*-
import Class
import numpy as np
from heapq import *
import collections
import math

def weight_update(graph, car, cur_road, next_node, t, beta=0.2):
    # 判断正向或反向
    flag = True if cur_road.end == next_node else False
    t = int(t)
    # time_cost: 车辆在cur_road上的理想行驶时间
    time_cost = math.ceil(float(cur_road.length) / min(car.speed, cur_road.speed))
    # congestion: 当前时刻t，cur_road上的拥塞情况，车道数（吞吐量）处以当前时刻道路上的车辆数
    if flag:
        congestion = float(graph.cong[t][0][cur_road.id][0][0] / cur_road.lanes) 
    else: 
        congestion = float(graph.cong[t][0][cur_road.id][0][1] / cur_road.lanes) # 正/反向
    weight = time_cost + beta * congestion
    
    return time_cost, weight

def update_road(graph, path, time_step, start_time):
    '''
    path：最终选择的路径 dict()
    time_step: 车辆到达各路口的时间点估计 dict() 03/25：是否维护的是正确的时间？
    start_time: 车辆的出发时间
    '''
    cur_t = start_time
    for road, forward, cross in list(path): 
    # 遍历：最终路径的道路(road)，道路的方向(forward:bool类型)，该道路的起始路口(cross)
        t = time_step[cross] # 车辆抵达当前cross的时间点
        time_cost = int(t - cur_t) # 两个路口之间的时间差，即车辆在对应道路上的运行时间段。

        for i in range(time_cost): # 更新道路状况(改进为统一更新)
            time = int(cur_t + i)
            # print('时间变化%d' % time)
            if forward: # 正向
                graph.cong[time][0][road][0][0] += 1      
            else: # 反向
                graph.cong[time][0][road][0][1] += 1 
        cur_t += time_cost # 当前时间更新为抵达下一个路口的时间

def path_plan(graph, car_list, i, alpha=0.5):
        '''
        对第i车辆进行路径规划
        '''
        adj = graph.adj # 邻接矩阵
        car = car_list[i]
        start = car.start
        end = car.end
        t = car.planTime # t应该为min(car.planTime, currentTime)
        q = [(0, start, ())] # 连接到节点的路径权值、节点id、路径
        visited = set()
        sp = dict() # 存储的最短距离
        sp[start] = 0 
        p_path = collections.OrderedDict() # 维护路径
        time = dict() # 维护时间
        time[start] = t 
        while q:
            (weight, cur_node, path) = heappop(q) # 根据权重weight取堆中的最小元素(权重、当前节点、源到当前节点的最短路径)
            if cur_node not in visited: # 找到终点就返回，减少搜索时间、
                visited.add(cur_node)
                # 在这里更新时间t
                if cur_node == end: 
                    # 更新graph.cong
                    update_road(graph, p_path[cur_node], time, t)
                    return np.array(p_path[cur_node])[:, 0]
                # 从cur_node出发的道路以及能够到达的路口id
                for next_node, cur_road in adj.get(cur_node, ()):
                    if next_node in visited: 
                        continue
                    # 在sp字典中查找是否存在连通next_node的路径，pre为该路径长度，不存在则返回None
                    pre = sp.get(next_node, None)
                    # time[cur_node]到达cur_node的时刻，不仅要返回当前道路的权重，还要计算从当前路径到达next_node的时间
                    time_cost, cur_road_weight = weight_update(graph, car, cur_road, next_node, time[cur_node])
                    # next_node的时间步 = cur_node的时间步加上当前车辆在cur_road上花费的时间
                    # weight表示start到当前节点的路径权重之和
                    # 权重有车辆行驶时间的因素影响
                    nc = weight + cur_road_weight

                    if pre is None or nc < pre: # 之前到next_node未连通或路径可优化
                        sp[next_node] = nc 
                        tem = list(p_path.get(cur_node, [])) # 获取从start到达cur_node的路径
                        flag = True if cur_road.end == next_node else False
                        tem.append([cur_road.id, flag, cur_node])
                        p_path[next_node] = tem # 更新路径：到达next_node的路径以及路径中各条道路的正反方向
                        # 有修改03/25 20:12
                        time[next_node] = time[cur_node] + time_cost # 更新到达next_node的时间（这里没有体现因为拥堵产生的延误）
                        # time[next_node] = time[cur_node] + int(math.ceil(cur_road_weight))
                        heappush(q, (nc, next_node, p_path)) 
          
def plan(graph, cars, answer_path):
    cur_carlist = {}
    for index, row in enumerate(cars.iterrows()):
        if index > 100:
            row[1]['planTime'] += (index-100)//40 + 1
        single_answer = list()
        cur_carlist[index] = Class.Cars(row[1]['id'], row[1]['from'], row[1]['to'], row[1]['speed'],\
         row[1]['planTime'], collections.defaultdict(set))
        
        # 对当前车辆进行路径规划（+planTime的延迟）
        path = path_plan(graph, cur_carlist, index) 
        single_answer.append(row[1]['id'])
        single_answer.append(row[1]['planTime'])
        for i in path:
            single_answer.append(i)
        single_answer = tuple(single_answer)
        answer = str(single_answer)
        ans_str = answer.replace("[","(").replace("],",")\n").replace("((","(").\
        replace("]]",")\n").replace(" ", '')

        with open(answer_path, 'a') as f:
            f.write(ans_str+'\n')

