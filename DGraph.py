class Car:
    '''
    定义车辆类：
    目的地、速度、状态、行驶道路id、车道、距离、下一条道路id、在路口处的行驶方向
    '''
    def __init__(self, end, speed, state, roadId, channel, dist, nextRoad, dir):
        self.end = end
        self.speed = speed
        self.state = state
        self.roadId = roadId
        self.channel = channel
        self.dist = dist
        self.nextRoad = nextRoad
        self.dir=dir

class Road:
    '''
    定义道路类：
    id、源、目的、限速、长度、车道数、是否双向
    '''
    def __init__(self, id, start, end, speed, length, channel, isDuplex):
        self.id = id
        self.start = start
        self.end = end
        self.speed = speed
        self.length = length
        self.channel = channel
        self.isDuplex = isDuplex
        self.carList = []

    def getCarList(self):
        for i in range(len(CarT)):
            if CarT[i].channel == self.channel and CarT[i].roadId == self.id:
                self.carList.append(CarT[i])
        sorted(self.carList, key=lambda car: car.dist)
        return self.carList

class  Cross:
    def __init__(self, id, up, right, down, left):
        self.id=id
        self.Road=[]
        self.Road.append(up)
        self.Road.append(right)
        self.Road.append(down)
        self.Road.append(left)

    def getRoadSpeed(self, roadId):
        for road in RoadT:
            if road.id== roadId:
                return road.speed

    def getRoadLength(self, roadId):
        for road in RoadT:
            if road.id==roadId:
                return road.length

    def getRoadChannel(self, roadId):
        for road in RoadT:
            if road.id == roadId:
                return road.channel

    def getRoadCar(self, roadId, channel):
        carList=[]
        for i in range(len(CarT)):
            if CarT[i].channel==channel and CarT[i].roadId==roadId:
                carList.append(CarT[i])
        return carList

    def driveOnRoad(self, roadId, channel): # 对道路roadId上车道channel上的车辆进行一次调度
        speed = self.getRoadSpeed(roadId)
        carList = self.getRoadCar(roadId, channel) # 需要判断车道没有车的情况

        sorted(carList, key=lambda car: car.dist)
        speedDrive = min(speed,carList[0].speed) # bug:carList为空

        if carList[0].dist - speedDrive >= 0: # 如果前方没有阻挡且不能走出路口，则执行一次调度，并将车辆标记为终止状态(2)
            carList[0].dist -= speedDrive 
            carList[0].state = 2
        else: # 如果此次调度能走出路口，则将车辆标记为等待状态(1) 对应的carT是否需要更新？
            carList[0].state = 1

        for i in range(1,len(carList)):
            speedDrive = min(speed, carList[i].speed)
            #print(carList[i].dist,speedDrive,carList[i-1].dist)
            if carList[i].dist - speedDrive >= carList[i-1].dist: # 如果前方有车但不阻挡，则执行一次调度，并将车辆标记为终止状态
                carList[i].dist -= speedDrive
                carList[i].state = 2
            else: # 如果前方有车辆阻挡，判断前车是否处于等待状态
                if carList[i-1].state == 1: # 若前车处于等待状态，则该车也标记为等待状态
                    carList[i].state = 1
                else: # 若前车处于终止状态，则该车也标记为终止状态（此时应该对该车的行驶速度进行更新）
                    carList[i].dist = carList[i-1].dist
                    carList[i].state = 2
        return carList

    def crossMoveCar(self): # 一个路口，根据Road_id从小到大遍历进行调度
        Road = sorted(self.Road)
        CarRoadOn = []
        for roadId in Road:
            aa = []
            channel = self.getRoadChannel(roadId)
            for channel_i in range(1, channel+1):
                a = self.driveOnRoad(roadId, channel_i)
                aa.append(a)
            CarRoadOn.append(aa)
        return CarRoadOn  #获取了四条道路上的不同通道上的汽车状态

    def getDir(self, roadId, nextRoadId):
        #1,2,3 D,L,R
        index1=0
        index2=0
        index1=self.Road.index(roadId)
        index2=self.Road.index(nextRoadId)
        if abs(index1-index2)==2:
            return 1
        if index1-index2==1:
            return 3
        if index2-index1==1:
            return 2
        if index1==0 and index2==3:
            return 3
        if index1==3 and index2==0:
            return 2

    def isconflict(self, roadId, dir, CarRoadOn):
        # 会发生冲突的主要是看，要进入的车道对面是否有直行，如果是左转弯只看直行，右转弯则看直行和左转弯
        index = self.Road.index(roadId)
        if dir == 1:
            return False
        if dir == 2: # 左转让直行
            indexD = (index - 1) % 4 # index好像要调用cross的数据结构？因为road的id可能是乱序的
            for channel in CarRoadOn[indexD]:
                for car in channel:
                    if car.dir==1:
                        return True
            return False
        if dir == 3: # 右转
            indexD = (index + 1) % 4 # 同理
            indexR = (index - 2) % 4
            for channel in CarRoadOn[indexD]:
                for car in channel:
                    if car.dir==1:
                        return True
            for channel in CarRoadOn[indexR]:
                for car in channel:
                    if car.dir == 2:
                        return True
            return False

    def isAllEndRoad(self, road): 
        for channel in road:
            for car in channel:
                if car.state == 1:
                    return False
        return True

    def isALLEndCross(self, CarRoadOn): # 判断道路上是否有处于等待状态的车辆
        for road in CarRoadOn:
            if self.isAllEndRoad(road)==False:
                return False
        return True

    def markDir(self, CarRoadOn):
        for road in CarRoadOn:  # 每一条道路等待出口的方向
            for channel in road:
                for car in channel:
                    if car.state == 1:
                        d = self.getDir(car.roadId, car.nextRoad)
                        car.dir = d

    def selectChannel(self, roadId):
        channel = self.getRoadChannel(roadId)
        length = self.getRoadChannel(roadId)
        for ch in range(channel):
            a = Road(roadId, 0, 0, 0, 0, ch+1, 0)
            carList = a.getCarList() # carT的内容好像没有更新
            sorted(carList, key=lambda car: car.dist)
            if carList[-1].dist < length:
                return channel
        return -1

    def driveCrossRoad(self):
        CarRoadOn = self.crossMoveCar()  # 获取路口四个方向的车辆情况
        self.markDir(CarRoadOn)  # 车辆的方向
        while self.isALLEndCross(CarRoadOn) == False: # 路口有处于等待的车辆
            for road in CarRoadOn:  # 多个channel的二维list
                while self.isAllEndRoad(road) == False: # 按道路id顺序依次调度
                    for channel in road:
                        id = channel[0].roadId
                        if channel[0].state == 1:
                            if self.isconflict(channel[0].roadId, channel[0].dir, CarRoadOn) == False:
                                channel[0].roadId = channel[0].nextRoad
                                r = self.selectChannel(channel[0].roadId)
                                if r > 0:
                                    channel[0].channel = r
                                channel[0].state = 2
                            else:
                                channel[0].state = 2

                    for each in CarT:
                        print(each.state, each.roadId, each.channel, each.dist)
                ch = self.getRoadChannel(id)
                for cc in range(ch):
                    self.driveOnRoad(id, cc+1)

# def main():
RoadT = []
CarT = []
CrossT = []
CrossT.append(Cross(1,-1,5000,-1,-1))
CrossT.append(Cross(2,5001,-1,-1,-1))
CrossT.append(Cross(3,-1,-1,-1,5002))
CrossT.append(Cross(4,-1,-1,5003,-1))
CrossT.append(Cross(5,5003,5002,5001,5000))
# id、源、目的、限速、长度、车道数、是否双向
RoadT.append(Road(5000,1,5,3,9,2,1))
RoadT.append(Road(5001,2,5,3,9,1,1))
RoadT.append(Road(5002,3,5,3,9,1,1))
RoadT.append(Road(5003,4,5,3,9,1,1))

# 目的地、速度、状态、行驶道路id、车道、到路口的距离、下一条道路id、dir
CarT.append(Car(0,3,1,5000,1,0,5002,0)) # end,speed,state,roadId,channel,dist,nextRoad,dir
CarT.append(Car(0,3,2,5000,1,6,5002,0))
CarT.append(Car(0,3,1,5000,2,0,5001,0))
CarT.append(Car(0,3,2,5001,1,3,5002,0))
CarT.append(Car(0,3,2,5002,1,3,5001,0))
CarT.append(Car(0,3,1,5003,1,0,5001,0))

test = Cross(5,5003,5002,5001,5000)
#test.driveOnRoad(5000,1)
a = test.crossMoveCar()
# print(a)
#print(CarT[0].dist,CarT[0].state)
#print(CarT[1].dist,CarT[1].state)
dir = test.getDir(5003,5001)
print(dir)
test.markDir(a)
for i in range(len(CarT)):
    print(CarT[i].state,CarT[i].dir)
#test.driveCrossRoad()
print(test.isconflict(5000,3,a))
print(test.isAllEndRoad(a[0]))
test.driveCrossRoad()

# if __name__ == '__main__':
#     main()