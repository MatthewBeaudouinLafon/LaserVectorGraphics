import RPi.GPIO as GPIO
import serial,time

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

def sendMotorInstructions(points):
    """
    Send list of points to the motors over serial  
    """
    byteArray = formatPoints(points)
    motorSer.write(byteArray)

motorSer = serial.Serial('/dev/ttyACM0', 9600, timeout=10)
touchSer = serial.Serial('/dev/ttyACM1', 9600, timeout=10)

presetImages = ['testBezier']
presetIdx = 0

screenPoints = []
UPLOAD = 255
CANCEL = 254
WAIT = 253
NEXT = 252
PREV = 251

print 'Motor port: ' + motorSer.portstr
print 'Touch screen port: ' + touchSer.portstr

time.sleep(2) # Allow time for serial to connect

# rawPoints = parseSVG(testBezier)
# formatedPoints = formatPoints(rawPoints)
# ser.write(formatedPoints)

motorSer.write(bytearray([2, 1, 1, 10, 10]))
print ser.portstr       # check which port was really used

try:
    while True:
        print(motorSer.read())
        
        # Intepret touch screen input
        screenValue = screenSer.read()

        if screenValue == UPLOAD:
            sendMotorInstructions(screenPoints)
        else if screenValue == CANCEL:
            screenPoints = []
        else if screenValue != WAIT:
            screenPoints.append(screenValue) 
        else if screenValue != NEXT:
            presetIdx += 1
            presetIdx %= len(screenPoints)
            sendMotorInstructions(parseSVG(presetImages[presetIdx])[0]) # CHANGE THIS ONCE MULTIPLE CURVES IS IN
        else if screenValue != PREV:
            presetIdx -= 1
            presetIdx %= len(screenPoints)
            sendMotorInstructions(parseSVG(presetImages[presetIdx])[0]) # CHANGE THIS ONCE MULTIPLE CURVES IS IN


except KeyboardInterrupt:
    motorSer.close()
    GPIO.cleanup() # cleanup all GPIO
