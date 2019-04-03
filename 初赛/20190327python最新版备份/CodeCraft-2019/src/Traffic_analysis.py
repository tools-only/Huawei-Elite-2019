import numpy as np 
import matplotlib.pyplot as plt 
plt.rcParams['font.sans-serif']=['SimHei'] # 解决中文乱码

data = np.loadtxt('analysis.txt')
# x1：车辆编号
x1 = data[:, 0]
# y1：车辆实际出发时间
y1 = data[:, 1]
# y2: 当前车辆、时刻网络中的存活车辆数
y2 = data[:, 2]

fig = plt.figure(num=1, figsize=(15, 8), dpi=80) 
plt.plot(x1, y1, label=u'车辆编号-出发时刻')
plt.plot(x1, y2, label=u'车辆编号-网络状况')
plt.legend()
plt.xlabel(u'车辆编号')
plt.ylabel(u'时刻/数量')
plt.show()