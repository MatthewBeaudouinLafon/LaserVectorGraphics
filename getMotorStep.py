from svgpathtools import svg2paths
import numpy as np
import matplotlib.pyplot as plt
import serial

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
paths, attributes = svg2paths('TestBezier.svg')

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
scaleUp = 100

for path in discretePaths:

	newPath = []
	for point in path:
		x, y = point
		point = ((x - minX)*scaleUp/scaleDown, (y - minY)*scaleUp/(2*scaleDown)) # y is divided by a factor of two due to the mirror setup
		newPath.append(point)

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

# Send x-y over Serial
ser = serial.Serial('COM6', 9600, timeout=0)  # open first serial port
print ser.portstr       # check which port was really used

for path in scaledPaths:
	ser.write(str(len(path)))
	for point in path:
		ser.write(str(point[0]))
		ser.write(str(point[1]))

print "Done"
ser.close()             # close port