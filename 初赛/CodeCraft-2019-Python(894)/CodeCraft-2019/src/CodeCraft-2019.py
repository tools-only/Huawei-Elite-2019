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

    # 导入数据
    roads, cars, crosses = Data.load_data(road_path, car_path, cross_path)
    # 创建模型实例
    graph = Class.Graph(roads)
    cars = Data.sort_speed(cars) # 按速度排序没有起到作用
    # cars = 

    with open(answer_path, 'w') as f:
        f.write("#(carId,StartTime,RoadId...)\n")

    # 车辆调度
    answer = Path.plan(graph, cars, answer_path)
    # 输出


# to read input file
# process
# to write output file
if __name__ == "__main__":
    main()