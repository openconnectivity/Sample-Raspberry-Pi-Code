import time

import explorerhat

def readAnalog(channel):
    if 0 < channel < 5:
        voltage = explorerhat.analog[channel - 1].read()
    else:
        voltage = 0
        print "Specified analog channel is out of range."
    print "voltage value = ", voltage
    return float(voltage)

def readInput(channel):
    if channel == 1:
        state = explorerhat.input.one.read()
    elif channel == 2:
        state = explorerhat.input.two.read()
    elif channel == 3:
        state = explorerhat.input.three.read()
    elif channel == 4:
        state = explorerhat.input.four.read()
    else:
        state = False
        print "Specified input channel is out of range."
    print "Input value = ", state
    return int(state)

def readTouch(channel):
    if channel == 1:
        state = explorerhat.touch.one.read()
    elif channel == 2:
        state = explorerhat.touch.two.read()
    elif channel == 3:
        state = explorerhat.touch.three.read()
    elif channel == 4:
        state = explorerhat.touch.four.read()
    elif channel == 5:
        state = explorerhat.touch.five.read()
    elif channel == 6:
        state = explorerhat.touch.six.read()
    elif channel == 7:
        state = explorerhat.touch.seven.read()
    elif channel == 8:
        state = explorerhat.touch.eight.read()
    else:
        state = False
        print "Specified touch channel is out of range."
    print "touch value = ", state
    return int(state)

def writeOutput(channel, state):
    if channel == 1:
        explorerhat.output.one.write(state)
    elif channel == 2:
        explorerhat.output.two.write(state)
    elif channel == 3:
        explorerhat.output.three.write(state)
    elif channel == 4:
        explorerhat.output.four.write(state)
    else:
        print "Specified output channel is out of range."
    return 0

def writeLight(channel, state):
    print "in WriteLight"
    if 0 < channel < 5:
        print "channel"
        explorerhat.light[channel - 1].write(state)
    else:
        print "Specified light channel is out of range."
    return 0

def writeMotor(channel, motorSpeed):
    if 0 < channel < 3:
        if -100 < motorSpeed < 100:
            explorerhat.motor[channel - 1].speed(motorSpeed)
        else:
            print "Speicified motor speed is out of range (-100 < x < 100)."
    else:
        print "Specified motor channel is out of range."
    return 0
