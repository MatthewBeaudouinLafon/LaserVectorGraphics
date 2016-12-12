import RPi.GPIO as GPIO
import serial,time
ser = serial.Serial('/dev/ttyACM0', 9600, timeout=10)
print ser.portstr

# rawPoints = parseSVG(testBezier)
# formatedPoints = formatPoints(rawPoints)
# ser.write(formatedPoints)
time.sleep(2)
ser.write(bytearray([2, 1, 10, 1, 10]))
print("serial written")

while(True):
    try:
        print(ser.readline())
    except KeyboardInterrupt:
        ser.close()

def parseSVG(name):
    """
    Take the name of an image as a string, and return a discretized list of points
    """
    # Parse XML
    paths, attributes = svg2paths('images/' + name + '.svg')

    # Discretize Parametric Curve
    discretePaths = [] # List of paths ie. list of list of points
    for path in paths: # Each path counts all lines together
        points = []

        for curve in path:
            points += [(curve.point(t).real, curve.point(t).imag) for t in np.arange(0, 1, 10/curve.length())]

        discretePaths.append(points)

    return discretePaths

def getTouchScreenInput():
    """
    Get input from the touch screen trough a secondary serial
    """
    pass

def formatPoints(points, printToConsole=False):
    """
    Takes a list of points and formats them to be sent to the Arduino (scaled)
    """
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
    scaleUp = 200

    for path in discretePaths:

        newPath = []
        for point in path:
            x, y = point
            zerod = (x - minX, y - minY)
            scaled = (int(zerod[0]*scaleUp/scaleDown), int(zerod[1]*scaleUp/(scaleDown))) # y is divided by a factor of two due to the mirror setup
            newPath.append(scaled)

        scaledPaths.append(newPath)

    if printToConsole:
        #Print x, y values to console individually for each curve
        for path in scaledPaths:
              print '\nNew Curve'
              print 'x'
              for point in zip(*path)[0]:
                  print point

              print 'y'
              for point in zip(*path)[1]:
                  print point

    toArduino = []
    for path in scaledPaths:
        toArduino.append(len(path))
        for point in path:
            toArduino.append(point[0])
            toArduino.append(point[1])

    return toArduino

ser = serial.Serial('/dev/ttyACM0', 9600)
time.sleep(2)


# rawPoints = parseSVG(testBezier)
# formatedPoints = formatPoints(rawPoints)
# ser.write(formatedPoints)

ser.write(bytearray([2, 1, 1, 10, 10]))
print ser.portstr       # check which port was really used

try:
    while True:
        print(ser.read())

        # If the touch screen is touched, get touch screen data

        # If there is nothing drawn, print the selected preset
except KeyboardInterrupt:
    ser.close()
    GPIO.cleanup() # cleanup all GPIO
