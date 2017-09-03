import numpy as np
from sklearn.svm import SVC

import matplotlib as mpl
from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt

import math 

X = np.zeros((0, 3))
Y = np.zeros(0)

part = 8
part_range = np.zeros((part, 2))

def add_data(file_name, id):
	array = []
	with open(file_name) as f:
		for line in f:
			array.append(line)
	
	number = np.zeros((len(array), 3))

	for i in range(0, len(array), 1):
	    x = array[i].split()
	    number[i][0] = x[0]
	    number[i][1] = x[1]
	    number[i][2] = x[2]	

	global X
	if len(X) == 0:
		X = number
	else:
		X = np.vstack((X, number))

	global Y
	part_range[id][0] = len(Y)
	if len(Y) == 0:
		Y = [id] * len(array)
	else:
		Y = Y + [id] * len(array)
	part_range[id][1] = len(Y)

def get_data(file_name, x, y, z):
	array = []
	with open(file_name) as f:
		for line in f:
			array.append(line)
	
	x = np.zeros((len(array), 1))
	y = np.zeros((len(array), 1))
	z = np.zeros((len(array), 1))

	for i in range(0, len(array), 1):
	    temp = array[i].split()
	    x[i] = temp[0]
	    y[i] = temp[1]
	    z[i] = temp[2]	

def convert_index(x, y):
	small = min(x, y)
	big = max(x, y)
	count = 0
	for i in range(0, small, 1):
		count += (part - 1 - i)
	count += (big - small - 1)
	#print(count)
	#print(x, y, small)
	return count

def judge_dir(a, b, c, d, id):
    global X
    global Y
    global part_range
    
    dirs = np.zeros(3)
    for i in range(int(part_range[id][0]), int(part_range[id][1]), 1):
    	now = np.zeros((0, 3))
    	now = X[i]
    	judge = a * now[0] + b * now[1] + c * now[2] - d

    	if judge > 0.001:
    	    dirs[0] += 1 
    	elif judge < -0.001:
    	    dirs[2] += 1
    	else:
    		dirs[1] += 1

    if dirs[0] > dirs[2]:
    	return 1
    else:
    	return -1
"""
add_data("vertex.txt", 0)
add_data("vertex.txt", 1)
"""
add_data("0.txt", 0)
add_data("1.txt", 1)
add_data("2.txt", 2)
add_data("3.txt", 3)
add_data("4.txt", 4)
add_data("5.txt", 5)
add_data("6.txt", 6)
add_data("7.txt", 7)
"""
add_data("8.txt", 8)
add_data("9.txt", 9)
add_data("10.txt", 10)
add_data("11.txt", 11)
add_data("12.txt", 12)
add_data("13.txt", 13)
add_data("14.txt", 14)
add_data("15.txt", 15)
"""


clf = SVC(kernel='linear')
clf.fit(X, Y) 


txt = []
with open("MAOi_neighbor.txt") as f:
	for line in f:
		txt.append(line)

neighbor = np.zeros((part, part))
num_neighbor = np.zeros((part, 1))		

for i in range(0, part, 1):
    x = txt[i].split()
    for j in range(0, part, 1):
    	neighbor[i][j] = x[j]
    	if neighbor[i][j] != 0:
    		num_neighbor[i] += 1
"""
for i in range(0, part, 1):
	print('%d : %d' %(i, num_neighbor[i]))
"""

"""
"""
"""
for i in range(0, 1, 1):
    for j in range(0, part, 1):
    	print(neighbor[i][j])
"""

"""
index = convert_index(4, 5)
p1_z = lambda p1_x,p1_y: (-clf.intercept_[index] - clf.coef_[index][0] * p1_x - clf.coef_[index][1] * p1_y )/ clf.coef_[index][2]

mpl.rcParams['legend.fontsize'] = 10
tmp = np.linspace(-500, 500, 400)
p1_x,p1_y = np.meshgrid(tmp,tmp)

fig = plt.figure()
ax = fig.gca(projection='3d')
ax.plot_surface(p1_x, p1_y, p1_z(p1_x,p1_y))

array1 = []
with open("4.txt") as f:
	for line in f:
		array1.append(line)
	
x1 = np.zeros(len(array1))
y1 = np.zeros(len(array1))
z1 = np.zeros(len(array1))

for i in range(0, len(array1), 1):
    temp = array1[i].split()
    x1[i] = temp[0]
    y1[i] = temp[1]
    z1[i] = temp[2]	

print(x1)
print(y1)
print(z1)

array2 = []
with open("5.txt") as f:
	for line in f:
		array2.append(line)
	
x2 = np.zeros(len(array2))
y2 = np.zeros(len(array2))
z2 = np.zeros(len(array2))

for i in range(0, len(array2), 1):
    temp = array2[i].split()
    x2[i] = temp[0]
    y2[i] = temp[1]
    z2[i] = temp[2]	

ax.plot(x1, y1, z1, color='#9F35FF', marker='o')
ax.plot(x2, y2, z2, color='#6A6AFF', marker='*')
ax.legend()

plt.show()
"""

"""
for i in range(0, len(clf.coef_), 1):
	length = math.sqrt(clf.coef_[i][0] * clf.coef_[i][0] + clf.coef_[i][1] * clf.coef_[i][1] + clf.coef_[i][2] * clf.coef_[i][2])
	print(i, ":")
	print(clf.coef_[i][0] / length, ",", clf.coef_[i][1] / length, ",", clf.coef_[i][2] / length, ",", -clf.intercept_[i] / length)
"""

"""
length = math.sqrt(clf.coef_[0][0] * clf.coef_[0][0] + clf.coef_[0][1] * clf.coef_[0][1] + clf.coef_[0][2] * clf.coef_[0][2])
p_dir = judge_dir(clf.coef_[0][0] / length, clf.coef_[0][1] / length, clf.coef_[0][2] / length, -clf.intercept_[0] / length, 0)
print('plane test_plane%d( %f, %f, %f, %f, %d);' % (0 + 1, clf.coef_[0][0] / length, clf.coef_[0][1] / length, clf.coef_[0][2] / length, -clf.intercept_[0] / length, p_dir))
"""
fo = open("training_plane.txt", "w")

print('%d' %(part))
fo.write('%d\n' %(part))

for i in range(0, part, 1):
    #print(i, ":")
    print('%d' %(num_neighbor[i]))
    fo.write('%d\n' %(num_neighbor[i]))
    plane_number = 0
    for j in range(0, part, 1):
        if neighbor[i][j] > 1:
    		#print(i, j)
            index = convert_index(i, j)
            length = math.sqrt(clf.coef_[index][0] * clf.coef_[index][0] + clf.coef_[index][1] * clf.coef_[index][1] + clf.coef_[index][2] * clf.coef_[index][2])
            p_dir = judge_dir(clf.coef_[index][0] / length, clf.coef_[index][1] / length, clf.coef_[index][2] / length, -clf.intercept_[index] / length, i)
            #print('plane test_plane%d( %f, %f, %f, %f, %d);' % (plane_number + 1, clf.coef_[index][0] / length, clf.coef_[index][1] / length, clf.coef_[index][2] / length, -clf.intercept_[index] / length, p_dir));
            print('%f, %f, %f, %f, %d' % (clf.coef_[index][0] / length, clf.coef_[index][1] / length, clf.coef_[index][2] / length, -clf.intercept_[index] / length, p_dir))
            fo.write('%f %f %f %f %d\n' % (clf.coef_[index][0] / length, clf.coef_[index][1] / length, clf.coef_[index][2] / length, -clf.intercept_[index] / length, p_dir))
            plane_number += 1
    #print()

fo.close()

"""
length = math.sqrt(clf.coef_[0][0] * clf.coef_[0][0] + clf.coef_[0][1] * clf.coef_[0][1] + clf.coef_[0][2] * clf.coef_[0][2])
p_dir = judge_dir(clf.coef_[0][0] / length, clf.coef_[0][1] / length, clf.coef_[0][2] / length, -clf.intercept_[0] / length, 0)
print('plane test_plane%d( %f, %f, %f, %f, %d);' % (0 + 1, clf.coef_[0][0] / length, clf.coef_[0][1] / length, clf.coef_[0][2] / length, -clf.intercept_[0] / length, p_dir))
"""