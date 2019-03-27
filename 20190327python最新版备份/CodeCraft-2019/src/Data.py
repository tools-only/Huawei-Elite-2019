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

def load_data(road_path, car_path, cross_path):
    roads = preproccess_data(road_path)
    cars = preproccess_data(car_path)
    crosses = preproccess_data(cross_path)
    
    return roads, cars, crosses

def sort_speed(cars):
    # 车辆按出发时间进行排序
    sorted_by = ['planTime', 'id']
    # sorted_by = ['planTime'] # 只按出发时间排序会死锁
    cars.sort_values(sorted_by, inplace=True)
    # 对同时出发的车辆，按速度进行降序排序
    sorted_cars = cars[cars.planTime == 1].sort_values('speed', ascending=False)
    for i in cars.planTime.unique()[1:]:
        item = cars[cars.planTime == i].sort_values('speed', ascending=False)
        sorted_cars = pd.concat([sorted_cars, item], axis=0)
    # print(cars.head(4))
    return sorted_cars 
    # return cars