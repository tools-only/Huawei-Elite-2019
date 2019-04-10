# -*- coding: utf-8 -*-

import Class
import Data
import Path
import logging
import sys
import collections

logging.basicConfig(level=logging.DEBUG,
                    filename='../logs/CodeCraft-2019.log',
                    format='[%(asctime)s] %(levelname)s [%(funcName)s: %(filename)s, %(lineno)d] %(message)s',
                    datefmt='%Y-%m-%d %H:%M:%S',
                    filemode='a')

def main():
    if len(sys.argv) != 6:
        logging.info('please input args: car_path, road_path, cross_path, answerPath')
        exit(1)

    car_path = sys.argv[1]
    road_path = sys.argv[2]
    cross_path = sys.argv[3]
    preset_answer_path = sys.argv[4]
    answer_path = sys.argv[5]

    logging.info("car_path is %s" % (car_path))
    logging.info("road_path is %s" % (road_path))
    logging.info("cross_path is %s" % (cross_path))
    logging.info("preset_answer_path is %s" % (preset_answer_path))
    logging.info("answer_path is %s" % (answer_path))
    
    # 导入数据
    roads, cars, crosses, pre_path, preset_time = Data.load_data(road_path, car_path, cross_path, preset_answer_path)
    # 创建模型实例
    graph = Class.Graph(roads, crosses)
    pre_cars = cars[cars[' preset']==1] # 预置车辆
    our_cars = cars[cars[' preset']==0] # 非预置车辆
    # 更新预置车辆的实际start_time
    pre_cars = Data.sort_priority(pre_cars) 
    pre_cars = Data.update_st(pre_cars, preset_time)
    our_cars = Data.sort_priority(our_cars) 
    # print('pre', our_cars)
    with open(answer_path, 'w') as f:
        f.write("#(carId,StartTime,RoadId...)\n")
    # 预置车辆交通状态初始化
    # initial_state(graph, pre_path, pre_cars)

    # 车辆调度
    answer = Path.plan(graph, pre_cars, our_cars, pre_path, answer_path)
    # 输出


# to read input file
# process
# to write output file
if __name__ == "__main__":
    main()