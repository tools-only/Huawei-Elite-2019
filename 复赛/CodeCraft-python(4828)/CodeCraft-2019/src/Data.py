# -*- coding: utf-8 -*-
import pandas as pd
'''
载入数据、预处理、排序
'''

def preproccess_data(file_path):
    data = pd.read_csv(file_path)
    file_name = file_path.strip().split('/')[-1].split('.')[0]
    if file_name == 'cross':
        data.columns = ['id', 'up', 'right', 'down', 'left']
    else:
        data.columns = data.columns.str.strip('#?(?)?')
    data[data.columns[0]] = data[data.columns[0]].str.lstrip('(').apply(pd.to_numeric)
    data[data.columns[-1]] = data[data.columns[-1]].str.rstrip(')').apply(pd.to_numeric)
    
    return data

def proccess(file_path):
    pre_path = dict()
    pre_start_time = dict()
    files = open(file_path)
    lines = files.readlines()
    for line in lines[1:]:
        tem = line.strip('(').split(',') # ['96545', '1', '6254', ' 5687', ' 6607', ' 5821', ' 5813', ' 6468', ' 5652', ' 6257', ' 5030', ' 5354', ' 5753', ' 6888', ' 6993', ' 5954)\n']
        tem_path = list()
        for i in range(2, len(tem)):
            if i == len(tem)-1:
                tem[i] = tem[i].strip(')\n')
            tem_path.append(int(tem[i]))
        pre_path[int(tem[0])] = tem_path 
        pre_start_time[int(tem[0])] = int(tem[1])

    return pre_path, pre_start_time

def update_st(cars, preset_time):
    for car in cars.iterrows():
        car[1][4] = preset_time[car[1][0]]

    return cars 

def load_data(road_path, car_path, cross_path, preset_answer_path):
    roads = preproccess_data(road_path)
    cars = preproccess_data(car_path)
    crosses = preproccess_data(cross_path)
    preset_answer, preset_time = proccess(preset_answer_path)

    return roads, cars, crosses, preset_answer, preset_time

def sort_priority(cars):
    # 车辆按优先级和出发时间进行排序
    # sorted_by = [' priority', 'planTime']
    # sorted_by = ['planTime']
    cars[' priority'] = cars[' priority'].apply(lambda x: -1 if x==1 else x)
    sorted_by = [' priority', 'planTime']
    cars.sort_values(sorted_by, inplace=True)
    # sorted_by = [' priority']
    # cars.sort_values(sorted_by, ascending=False, inplace=True)
    # 对同时出发的车辆，按速度进行降序排序
    # sorted_cars = cars[cars.planTime == 1].sort_values('speed', ascending=False)
    # for i in cars.planTime.unique()[1:]:
    #     item = cars[cars.planTime == i].sort_values('speed', ascending=False)
    #     sorted_cars = pd.concat([sorted_cars, item], axis=0)
    # return sorted_cars 
    return cars

# def initial_state(graph, pre_path, pre_cars):
#     # 预置环境初始化
#     for car in pre_cars:
