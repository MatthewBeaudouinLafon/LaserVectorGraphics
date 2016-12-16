from svgpathtools import svg2paths
import numpy as np
import matplotlib.pyplot as plt
import serial, struct, time

gearRatio = 43/float(24)

def makeCartesian(complexPoints):
	cartesianPoints = []

	for curve in complexPoints:
		cartPts = []
		for point in curve:
			x, y = point.real, point.imag
			cartPts.append((x, y))
		cartesianPoints.append(cartPts)

	return cartesianPoints

# Parse XML
paths, attributes = svg2paths('images/TestBezier.svg')

# Discretize Parametric Curve
discretePaths = [] # List of paths ie. list of list of points
for path in paths: # Each path counts all lines together
	points = []

	for curve in path:
		points += [(curve.point(t).real, curve.point(t).imag) for t in np.arange(0, 1, 10/curve.length())]

	discretePaths.append(points)

# Find min-max for scaling image
minX = np.inf
minY = np.inf

maxX = 0
maxY = 0
for path in discretePaths:
	minX = min(zip(*path)[0])
	maxX = max(zip(*path)[0])
	minY = min(zip(*path)[1])
	maxY = max(zip(*path)[1])

# Scale image
scaledPaths = []
scaleDown = maxX if maxX > maxY else maxY
scaleUp = 200.0

for path in discretePaths:

	newPath = []
	for point in path:
		x, y = point
		zerod = (x - minX, y - minY)
		scaled = (int(zerod[0]*scaleUp/scaleDown), int(zerod[1]*scaleUp/(scaleDown))) # y is divided by a factor of two due to the mirror setup
		newPath.append(scaled)

	scaledPaths.append(newPath)

# #Print x, y values to console individually for each curve
# for path in scaledPaths:
# 		print '\nNew Curve'
# 		print 'x'
# 		for point in zip(*path)[0]:
# 			print point

# 		print 'y'
# 		for point in zip(*path)[1]:
# 			print point

toArduino = []
for path in scaledPaths:
	toArduino.append(len(path)//255)
	toArduino.append(len(path)%255)
	for point in path:
		toArduino.append(point[0])
	for point in path:
		toArduino.append(point[1])

# print "Array length: {}".format(len(toArduino))
# for x in toArduino:
# 	print x

# # Send x-y over Serial
ser = serial.Serial('/dev/ttyACM0', 9600, timeout=10)  # open first serial port
print ser.portstr       # check which port was really used

time.sleep(2)

# for i in [2, 1, 1, 5, 5]:]
# 	# ser.write(struct.pack('>B', i))
# 	ser.write(bin(i))

# vals = bytearray([4, 1, 1, 5, 5, 10, 10, 5, 5])
# print(vals)
ser.write(bytearray(toArduino))
print len(toArduino)
# for path in scaledPaths:
# 	ser.write(bytearray(toArduino))

# 	# ser.write(str(len(path)))
# 	# # print(len(path))
# 	# for point in path:
# 	# 	pass
# 	# 	# print("{}, {}".format(point[0], point[1]))
# 	# 	ser.write(str(point[0]))
# 	# 	ser.write(str(point[1]))

print "Done"

while True:
	print(ser.readline())

ser.close()             # close port
