from envirophat import light, weather, motion, analog, leds

def readADC(channel):
    voltage = analog.read(channel)
    print "voltage value = ", voltage
    return float(voltage)

def readBrightness():
    brightness = light.light()
    print "brightness value = ", brightness
    return long(brightness)

def writeLEDs(state):
    if (state):
        leds.on()
    else:
        leds.off()
    return 0
