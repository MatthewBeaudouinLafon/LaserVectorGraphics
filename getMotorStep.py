from svgpathtools import svg2paths
import numpy as np
import matplotlib.pyplot as plt

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
		points += [(curve.point(t).real, curve.point(t).imag) for t in np.arange(0, 1, 100/curve.length())]

	discretePaths.append(points)

#Print x, y values individually for each curve
for path in discretePaths:
		print '\nNew Curve'
		print 'x'
		for point in zip(*path)[0]:
			print point

		print 'y'	
		for point in zip(*path)[1]:
			print point
