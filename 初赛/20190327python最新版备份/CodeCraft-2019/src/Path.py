# -*- coding: utf-8 -*-
import Class
import Analysis
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
    # # test
    # print('%d在当前时间%d下的拥塞%d' % (cur_road.id, t, congestion))
    # print(graph.cong[t][0][cur_road.id][0][0], cur_road.lanes)
    # print(graph.cong[t][0][cur_road.id][0][1], cur_road.lanes)
    # print('时间消耗%d-拥挤程度%d' % (time_cost, congestion))
    
    return time_cost, weight

def update_road(graph, path, time_step, start_time):
    '''
    path：最终选择的路径 dict()
    time_step: 车辆到达各路口的时间点估计 dict() 03/25：是否维护的是正确的时间？
    start_time: 车辆的出发时间
    '''
    cur_t = start_time
    # print(path)
    for road, forward, cross in list(path): 
    # 遍历：最终路径的道路(road)，道路的方向(forward:bool类型)，该道路的起始路口(cross)
        t = int(time_step[cross]) # 车辆抵达当前cross的时间点
        time_cost = int(t - cur_t) # 两个路口之间的时间差，即车辆在对应道路上的运行时间段。
        i = 0
        # 更新道路状况
        while i <= time_cost:
            time = int(cur_t + i)
      
            if forward: # 正向
                graph.cong[time][0][road][0][0] += 1  # 这里有bug +1操作对cong的所有时间片生效  
            else: # 反向
                graph.cong[time][0][road][0][1] += 1
            i += 1
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
                        time[next_node] = time[cur_node] + int(time_cost) # 更新到达next_node的时间（这里没有体现因为拥堵产生的延误）
                        # time[next_node] = time[cur_node] + int(math.ceil(cur_road_weight))
                        heappush(q, (nc, next_node, p_path)) 
          
def plan(graph, cars, answer_path):
    car_statistics = []
    cur_carlist = {}
    nu = [1500, 2000, 2500, 3000, 3500, 4000, 4500, 5000, 5500, 6000, 6500, 7000, 7500, 8000,8500,9000,9500,10000]
    for index, row in enumerate(cars.iterrows()):
        if index > 100:
            # if index >
            row[1]['planTime'] += (index-100)//15 + 1

        # elif index > nu[0] and index < nu[1]:
        #     row[1]['planTime'] += (index-nu[1])//20 + 1
        # elif index > nu[2] and index < nu[3]:
        #     row[1]['planTime'] += (index-nu[0])//20 + 1
        # elif index > nu[4] and index < nu[5]:
        #     row[1]['planTime'] += (index-nu[0])//20 + 1
        # elif index > nu[6] and index < nu[7]:
        #     row[1]['planTime'] += (index-nu[0])//20 + 1
        # elif index > nu[8] and index < nu[9]:
        #     row[1]['planTime'] += (index-nu[0])//20 + 1
        # elif index > nu[10] and index < nu[11]:
        #     row[1]['planTime'] += (index-nu[0])//20 + 1
        # elif index > nu[12] and index < nu[13]:
        #     row[1]['planTime'] += (index-nu[0])//20 + 1
        # elif index > nu[14] and index < nu[15]:
        #     row[1]['planTime'] += (index-nu[0])//20 + 1
        # elif index > nu[16] and index < nu[17]:
        #     row[1]['planTime'] += (index-nu[0])//20 + 1
        # else:
        #     row[1]['planTime'] += (index-nu[0])//15 + 1
        # elif index >= 500:
        #     row[1]['planTime'] += (index-100)//20 + 1
        single_answer = list()
        cur_carlist[index] = Class.Cars(row[1]['id'], row[1]['from'], row[1]['to'], row[1]['speed'],\
         row[1]['planTime'], collections.defaultdict(set))
        # 当前网络状况评估
        # print('%d - %d' % (row[1]['from'], row[1]['to']))
        f_nums, r_nums, car_nums = Analysis.traffic_analysis(graph, row[1]['planTime'])
        # print(f_nums, r_nums, car_nums) # 发现上面plan函数里更新path_value有错
        # if index == 1:
        #     print(s)
        car_statistics.append([index , row[1]['planTime'], car_nums])
        # np.savetxt('analysis.txt', car_statistics)
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

    np.savetxt('analysis.txt', car_statistics)