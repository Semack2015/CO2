import serial
import time
import re
config = open('config.txt')
port = config.read().split('\n')[0]
import serial.tools.list_ports
list = serial.tools.list_ports.comports()
connected = []
for element in list:
    connected.append(element.description)
print("Connected COM ports: " + str(connected))
print(port)
time.sleep(3)
ser = serial.Serial(port, 9600, timeout=1)
#ser.port = config.read()
#ser.baudrate = 9600
#ser.timeout = 1
#ser.open()
while(1):
    ser.write(b'=')
    a = ser.readline()
    a = str(a)
    #print(a)
    # a = a.replace('\'', '')
    # a = a.replace('b', '')
    # a = a.replace('\\', '')
    # a = a.replace('r', '')
    # a = a.replace('n', '')
    a = re.sub('[^0123456789\s]', "", a)
    #print(a)
    if a.split(' ').__len__()==2:
        if a.split(' ')[0] != '':
            a1 = int(a.split(' ')[0])
        else: a1 = 0
        if a.split(' ')[1] != '':
            a2 = int(a.split(' ')[1])
        else: a2 = 0
    else:
        a1 = 0
        a2 = 0
    a3 = a1*256+a2
    print(time.strftime("%d.%m.%Y %H:%M:%S"), '\t',a3 )
    file = open('CO2_Log.txt', 'a')
    file.write(time.strftime("%d.%m.%Y %H:%M:%S"))
    file.write('\t')
    file.write(str(a3))
    file.write('\n')
    file.close()
    time.sleep(1)