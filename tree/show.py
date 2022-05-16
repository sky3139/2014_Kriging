
import cv2  # Not actually necessary if you just want to create an image.
import numpy as np
import matplotlib.pyplot as plt
#将(0,1)点和(2,4)连起来
# plt.plot([0,2],[1,4])
# 

inputpath = r'../tree/test.data'


xp=[]
yp=[]
with open(inputpath, 'r', encoding='utf-8') as infile:
    # 第一种：读取所有行
    #data1 = infile.readlines()
    #print(data1)
    # 输出：['1 2 3 4 5\n', '6 7 8 9 10']

    # 第二种：每行分开读取
    data2 = []
    for line in infile:
        data_line = line.strip("\n").split()  # 去除首尾换行符，并按空格划分
        # print(data_line)
        data2.append([float(i) for i in data_line])

d3=data2[1:]
d3=np.array(d3).reshape(2,-1)
# blank_image = np.zeros((400,400,3), np.uint8)
plt.scatter(d3[0:][0],d3[0:][1])

rec=np.array(data2[0]) #.reshape(-1,2)
print(rec[0],rec[1])
# plt.plot(rec,data2[0][1])
# plt.plot(rec[0],rec[1])


plt.gca().add_patch(
plt.Rectangle((rec[0], rec[1]), rec[2]-rec[0],
                  rec[3]-rec[1], fill=False,
                  edgecolor='r', linewidth=1))
# plt.plot(data2[1][1],data2[1][0])
# plt.plot(data2[1][0],data2[0][0])
# for pt in data2[1:]:
#     plt.scatter(pt.x,pt.y)
#     # blank_image[pt[0],pt[1]]=[255,255,255]
#     print(pt)
# cv2.imshow("blank_image",blank_image)
# cv2.waitKey()
plt.show()
print(data2[0][1])