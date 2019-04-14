# -*- coding: utf-8 -*-
import Class
import Analysis
import numpy as np
from heapq import *
import collections
import math
import copy

def memory_reduce(graph, pre):
    # # 内存释放
    # del_time = now - 10 # 删除十个时间片之前的流量信息
    # for t in range(pre, del_time):
    #     # a = graph.cong.get(t, None)
    #     del graph.cong[t] # 删除t-1时刻的数据
    #     del graph.cross_value[t]
    del graph.cong[pre]
    del graph.cross_value[pre]

# 路权计算
def weight_update(graph, car, cur_road, next_node, t):
    # 判断正向或反向
    flag = True if cur_road.end == next_node else False
    t = int(t)
    # time_cost: 车辆在cur_road上的理想行驶时间
    time_cost = math.ceil(float(cur_road.length) / min(car.speed, cur_road.speed))
    # road_congestion: 当前时刻t，cur_road上的拥塞情况，车道数（吞吐量）处以当前时刻道路上的车辆数
    # cross_congestion：当前时刻t, 路口上的拥塞情况
    # '''
    # neibor aggregate
    # '''
    # if flag:
    #     road_congestion = graph.road_value[cur_road.id][0][0]
    # else:
    #     road_congestion = graph.road_value[cur_road.id][0][1]
    # cross_congestion = graph.cross_value[next_node]
    # for cross in graph.cross_neibor[next_node]:
    #     cross_congestion += graph.cross_value[cross]
    #     # 二阶邻居反而变差了
    #     for cross_2 in graph.cross_neibor[cross]:
    #         if cross_2 != next_node:
    #             cross_congestion += (1/2) * graph.cross_value[cross_2]
    '''
    道路权重：Markov 0409
    '''
    # 判空
    f1 = graph.road_value_t.get(t, None)
    f2 = graph.cross_value_t.get(t, None)
    if not f1:
        # road_value_t = copy.deepcopy(graph.road_value_copy) # copy时可以考虑直接copy上一个时间步的流量值，然后做flow_out
        # road_value_t = copy.deepcopy(graph.road_value_t[t-1]) # 取上一个时间步的流量值，并做一定的衰减，衰减的大小根据上一个时间进入网络的车辆数决定
        # graph.road_value_t[t].append(road_value_t)
        road_weight_decay(graph, t) # 道路权重填充
        # graph.road_value_t[t].append(road_value_t)
    if not f2:
        cross_weight_decay(graph, t) # 路口权重填充
    if flag: # 正向
        # road_congestion = graph.road_value[cur_road.id][0][0]
        road_congestion = graph.road_value_t[t][0][cur_road.id][0]
        if t < 5:
            for time in range(1, t+1):
                # time/t待斟酌
                road_congestion += float(time/t) * graph.road_value_t[t][0][cur_road.id][0] # 该道路前t个时刻的流量情况   
                # road_congestion += graph.road_value_t[t][0][cur_road.id][0] # 该道路前t个时刻的流量情况   
        else:
            for time in range(t-4, t+1): # 马尔可夫 长度为5
                # x1 = graph.road_value_t.get(time, None) # bug:flow_in和flow_out没有建模
                # if not x1: # 缺失值填充 (有问题)
                #     road_value_t = copy.deepcopy(graph.road_value_copy) 
                #     graph.road_value_t[time].append(road_value_t)
                #     # graph.road_value_t[time][0][cur_road.id][0][0] = graph.road_value_t[time-1][0][cur_road.id][0][0] # bug 时间步不是连续的
                road_congestion += float(time/t) * graph.road_value_t[time][0][cur_road.id][0] # 该道路前5个时刻的流量情况
                # road_congestion += graph.road_value_t[time][0][cur_road.id][0]
    else: # 逆向
        # road_congestion = graph.road_value[cur_road.id][0][0]
        road_congestion = graph.road_value_t[t][0][cur_road.id][1]
        if t < 5:
            for time in range(1, t+1):
                # time/t待斟酌
                road_congestion += float(time/t) * graph.road_value_t[time][0][cur_road.id][1] # 该道路前t个时刻的流量情况  
                # road_congestion += graph.road_value_t[time][0][cur_road.id][1] # 该道路前t个时刻的流量情况   
        else:
            for time in range(t-4, t+1): 
                # x1 = graph.road_value_t.get(time, None) # 判空
                # if not x1: # 缺失值填充
                #     road_value_t = copy.deepcopy(graph.road_value_copy) 
                #     graph.road_value_t[time].append(road_value_t)
                    # graph.road_value_t[time][0][cur_road.id][0][1] = graph.road_value_t[time-1][0][cur_road.id][0][1] 
                road_congestion += float(time/t) * graph.road_value_t[time][0][cur_road.id][1] # 该道路前5个时刻的流量情况
                # road_congestion += graph.road_value_t[time][0][cur_road.id][1] # 该道路前5个时刻的流量情况
    
    # cross_congestion = graph.cross_value_t[t][0][next_node]
    cross_congestion = 0 
    if t < 5:
        for time in range(1, t+1): # 1-hop cross t-Markov
            cross_congestion += float(time/t) * graph.cross_value_t[time][0][next_node]
            # cross_congestion += graph.cross_value_t[time][0][next_node]
    else:
        for time in range(t-4, t+1): # 1-hop cross 10-Markov
            # x2 = graph.cross_value_t.get(time, None) 
            # if not x2: # 缺失值填充
            #     cross_value_t = copy.deepcopy(graph.cross_value_copy) 
            #     graph.cross_value_t[time].append(cross_value_t)
            #     # graph.road_value_t[time][0][cur_road.id][0][1] = graph.road_value_t[time-1][0][cur_road.id][0][1] 
            cross_congestion += float(time/t) * graph.cross_value_t[time][0][next_node] # 该路口前5个时刻的流量情况
            # cross_congestion += graph.cross_value_t[time][0][next_node] # 该路口前5个时刻的流量情况
    '''
    # cross：马尔可夫结合1阶邻域权重聚集
    '''
    for cross in graph.cross_neibor[next_node]:
        cross_congestion_i = graph.cross_value_t[t][0][cross] # 1-hop cross_value
        if t < 5:
            for time in range(1, t+1): # 1-hop cross t-Markov
                cross_congestion_i += float(time/t) * graph.cross_value_t[time][0][cross]
                # cross_congestion_i += graph.cross_value_t[time][0][cross]
        else:
            for time in range(t-4, t+1): # 1-hop cross 10-Markov
                # x2 = graph.cross_value_t.get(time, None) 
                # if not x2: # 缺失值填充
                #     cross_value_t = copy.deepcopy(graph.cross_value_copy) 
                #     graph.cross_value_t[time].append(cross_value_t)
                #     # graph.road_value_t[time][0][cur_road.id][0][1] = graph.road_value_t[time-1][0][cur_road.id][0][1] 
                cross_congestion_i += float(time/t) * graph.cross_value_t[time][0][cross] # 该路口前5个时刻的流量情况
                # cross_congestion_i += graph.cross_value_t[time][0][cross] # 该路口前5个时刻的流量情况
        
        cross_congestion += 1/2 * cross_congestion_i # k-neibor-hop与k-time-hop结合
    weight = time_cost + (road_congestion/math.pow(cur_road.lanes, 2)) * (cross_congestion)
    # weight = road_congestion + cross_congestion
    
    return time_cost, weight

def update_road(graph, path, time_step, start_time):
    '''
    path：最终选择的路径 dict()
    time_step: 车辆到达各路口的时间点估计 dict() 03/25：是否维护的是正确的时间？
    start_time: 车辆的出发时间
    这里如何让车辆走得更均匀呢？ 我们一直没有考虑道路本身的特征以及道路上车辆的特征
    '''
    cur_t = start_time
    for road, forward, cur_cross, next_cross in list(path): 
        # 更新road状况 
        if forward: # 正向
            # graph.road_value[road][0][0] += 1
            graph.road_value_t[cur_t][0][road][0] += 1
        else:
            # graph.road_value[road][0][1] += 1
            graph.road_value_t[cur_t][0][road][1] += 1
        # 更新cross状况
        # graph.cross_value[next_cross] += 1
        # graph.cross_value[cur_cross] += 1
        graph.cross_value_t[cur_t][0][next_cross] += 1
        graph.cross_value_t[cur_t][0][cur_cross] += 1

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
                    # 0407更新
                    time_cost, cur_road_weight = weight_update(graph, car, cur_road, next_node, t)
                    # next_node的时间步 = cur_node的时间步加上当前车辆在cur_road上花费的时间
                    # weight表示start到当前节点的路径权重之和
                    # 权重有车辆行驶时间的因素影响
                    nc = weight + cur_road_weight
                    if pre is None or nc < pre: # 之前到next_node未连通或路径可优化
                        sp[next_node] = nc 
                        tem = list(p_path.get(cur_node, [])) # 获取从start到达cur_node的路径
                        flag = True if cur_road.end == next_node else False
                        tem.append([cur_road.id, flag, cur_node, next_node])
                        p_path[next_node] = tem # 更新路径：到达next_node的路径以及路径中各条道路的正反方向
                        # 有修改03/25 20:12
                        time[next_node] = time[cur_node] + int(time_cost) # 更新到达next_node的时间（这里没有体现因为拥堵产生的延误）
                        # time[next_node] = time[cur_node] + int(math.ceil(cur_road_weight))
                        heappush(q, (nc, next_node, p_path)) 

def road_weight_decay(graph, t):
    '''
    当前时刻为t, 首先判断当前时刻的前k个时刻是否有流量，若没有则进行填充
    '''
    # 衰减模型应该是非线性的 根据每条道路的容量和流量以及相邻道路的容量及流量而定 待改进
    decay_rate = 0.1
    if graph.road_value_t.get(t, None):
        _, _, car_nums = Analysis.traffic_analysis(graph, t)
        if car_nums < 3000:
            decay_rate = 0.3
    # _, _, car_nums = Analysis.traffic_analysis(graph, t)
    # if car_nums > 8000:
    #     decay_rate = 0.1
    # elif car_nums < 3000:
    #     decay_rate = 0.5
    k = 1
    while not graph.road_value_t.get(t-k, None):
        k += 1

    for i in range(k-1, -1, -1): # 首先对前k个缺失进行填充
        tem_road_value_t = collections.defaultdict(np.array)
        for j in graph.road_value_t[t-i-1][0]:
            tem_road_value_t[j] = (1-decay_rate) * graph.road_value_t[t-i-1][0][j]
            # graph.road_value_t[t-i][0][j] = 0.9 * graph.road_value_t[t-i-1][0][j] # 道路权重衰减
        graph.road_value_t[t-i].append(tem_road_value_t)

def cross_weight_decay(graph, t): 
    decay_rate = 0.05
    # _, _, car_nums = Analysis.traffic_analysis(graph, t)
    # if car_nums > 8000:
    #     decay_rate = 0.05
    # elif car_nums < 3000:
    #     decay_rate = 0.2
    if graph.road_value_t.get(t, None):
        _, _, car_nums = Analysis.traffic_analysis(graph, t)
        if car_nums < 3000:
            decay_rate = 0.2
    k = 1
    while not graph.cross_value_t.get(t-k, None):
        k += 1

    for i in range(k-1, -1, -1): # 首先对前k个缺失进行填充
        tem_cross_value_t = dict()
        for j in graph.cross_value_t[t-i-1][0]:
            tem_cross_value_t[j] = (1-decay_rate) * graph.cross_value_t[t-i-1][0][j]
        graph.cross_value_t[t-i].append(tem_cross_value_t)

def load_traffic(graph, pre_cars, pre_path, pre, now, pre_last):
    '''
    加载pre至当前时刻的预置车辆交通状态
    '''
    if now + 5 <= pre_last:
        time = now + 5
    else:
        time = pre_last
    load_cars = pre_cars.loc[(pre_cars['planTime'] > pre)&(pre_cars['planTime'] <= time)] 
    for index, car in enumerate(load_cars.iterrows()):
        # st = car[1]['planTime'] # 出发时间
        st = now
        path_i = pre_path[car[1]['id']] # 车辆的预置路径
        flag = dict()
        i = 0
        while i < len(path_i)-1:
            a = path_i[i]
            b = path_i[i+1]
            fromA = graph.road_list[a].start
            toA = graph.road_list[a].end
            fromB = graph.road_list[b].start
            toB = graph.road_list[b].end
            if toA == fromB:
                flag[i] = 1
                flag[i+1] = 1
            if fromA == toB:
                flag[i] = 0
                flag[i+1] = 0
            if toA == toB:
                flag[i] = 1
                flag[i+1] = 0
            if fromA == fromB:
                flag[i] = 0
                flag[i+1] = 1
            i += 1

        for index, path in enumerate(path_i):
            path_i = graph.road_list[path] # path_i的对象
            # 判空
            f1 = graph.road_value_t.get(st, None)
            f2 = graph.cross_value_t.get(st, None)

            if not f1:
                # 初始化时不能直接置为0，而是在前一步的基础上衰减 具体怎么衰减要考虑进入网络的车辆数以及当前网络中车的数量 
                # 数量越大衰减越慢(更进一步对道路i进行建模 待商榷) 道路上的平均车速，平均车速快则衰减快，平均车速慢则衰减满
                road_weight_decay(graph, st) # 道路权重衰减
                # graph.road_value_t[st].append(road_value_t)
            if not f2:
                # cross_value_t = copy.deepcopy(graph.cross_value_t[st-1])
                cross_weight_decay(graph, st) # 路口权重衰减
                # graph.cross_value_t[st].append(cross_value_t)
            if flag[index] == 1: # 正向
                # graph.road_value[path_i.id][0][0] += 1
                # 时间片记录
                # print(st, path_i.id)
                # if st==2:
                #     print(graph.road_value_t[st])
                # print(graph.road_value_t[st][0][path_i.id])
                # print(graph.road_value_t[st][0][5425])
                graph.road_value_t[st][0][path_i.id][0] += 1
            else:
                # graph.road_value[path_i.id][0][1] += 1
                # 时间片记录
                graph.road_value_t[st][0][path_i.id][1] += 1
            # graph.cross_value[path_i.start] += 1
            # graph.cross_value[path_i.end] += 1
            # 时间片记录
            graph.cross_value_t[st][0][path_i.start] += 1
            graph.cross_value_t[st][0][path_i.end] += 1

def plan(graph, pre_cars, our_cars, pre_path, answer_path):
    cur_carlist = {}
    pre = 0
    # flow_size, cur_size = 140, 140 # 每个时间步可进入网络的流量 包括预置车在内 预置车也将被分片
    flow_size, cur_size = 20, 20
    # t = [0] * 6000 # 分配6000个时间片，记录时刻t已分配的车辆数
    add = 0 # 记录每个时间片应该后移的时间片数
    # pre_carnum = pre_cars[pre_cars['planTime']==1].shape(0) # 初始化 需要注意pre_cars可能不是从planTime=1开始
    # if pre_carnum > flow_size:
    #     flow_size = 0
    # else:
    #     flow_size -= pre_carnum
    # car_statistics = dict()
    car_statistics = []
    now = 1 # 记录当前调度所处时间步（非系统时间）

    # 预置车最晚的出发时间
    pre_last = max(pre_cars['planTime'])

    for index, car in enumerate(our_cars.iterrows()):
        '''
        判断普通车流量
        '''
        if cur_size <= 0:
            # add += 1 # 如果这时planTime=4 +1的话 又需要重新载入预置车辆了 怎么办？ 
            now += 1 # 时间片后移一个
            # cur_size重置
            cur_size = flow_size
        else:
            cur_size -= 1 # 流量窗口减一

        '''
        载入预置车 
        '''
        if now > pre_last+5:
            flow_size = 50 # 后面的普通车
            # if graph.road_value_t.get(now, None):
            #     _, _, car_nums = Analysis.traffic_analysis(graph, now-1)
            #     # if car_nums >= 8000:
            #     #     cur_size -= 1
            #     if car_nums >= 8500:
            #         cur_size -= 2

        if pre < now:
            # 统计此时刻预置车辆的数量(注意这里的时间now可能不是连续的)
            pre_carnum = len(pre_cars[pre_cars['planTime']==now]) 
            if pre_carnum >= flow_size: # 预置车过多
                ''' 
                将时间片后移（整体后移）
                例如，planTime=1时有80辆预置车要出发，则时间步后移一个单位到planTime=2，且planTime=1的时刻多出的20辆车将占用20个流量窗口
                （尽管实际调度时这80辆车都是在planTime=1时出发的）
                '''
                add += pre_carnum//flow_size # 此时间步需要后移的步数
                # 载入预置车后，重置流量窗口
                now += add
                cur_size = flow_size - (pre_carnum % flow_size) # 剩余流量窗口
            else:
                cur_size -= pre_carnum # 剩余流量窗口

            # 加载上一次时间步pre到当前时间步now的预置车辆网络状态
            load_traffic(graph, pre_cars, pre_path, pre, now, pre_last) 
            pre = now

        # 当前时刻网络状态评估
        # bug：now在上面可能被更新，但这个时期的流量还未更新
        # if graph.road_value_t.get(now, None):
        #     _, _, car_nums = Analysis.traffic_analysis(graph, now)
        #     # if car_nums >= 8000:
        #     #     cur_size -= 1
        #     if car_nums >= 9600: # 改进，如果在上一时刻发现car_nums也大 则这里应该继续减少
        #         cur_size -= 1
        #     if car_nums >= 10000:
        #         cur_size -= 5
        #     # else:
        #     #     cur_size = 65
        #     # car_statistics[index] = [now, car_nums]
        if car[1]['planTime'] > now:
            now = car[1]['planTime']
        car[1]['planTime'] = now
        
        single_answer = list()
        # if car[1][' preset'] == 0: # 非预置车辆
        cur_carlist[index] = Class.Cars(car[1]['id'], car[1]['from'], car[1]['to'], car[1]['speed'],\
         car[1]['planTime'], collections.defaultdict(set), car[1][' priority'], car[1][' preset'])

        path = path_plan(graph, cur_carlist, index) 
        single_answer.append(car[1]['id'])
        single_answer.append(car[1]['planTime'])
        for i in path:
            single_answer.append(i)
        single_answer = tuple(single_answer)
        answer = str(single_answer)
        ans_str = answer.replace("[","(").replace("],",")\n").replace("((","(").\
        replace("]]",")\n").replace(" ", '')

        with open(answer_path, 'a') as f:
            f.write(ans_str+'\n')
    # np.savetxt('analysis.txt', car_statistics)