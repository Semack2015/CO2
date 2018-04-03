import time
import re
import serial.tools.list_ports

config = open('config.txt')
log = open('Port_Log.txt', 'w')


port = config.read().split('\n')[0]
p_list = serial.tools.list_ports.comports()
connected = []
for element in p_list:
    connected.append(element.description)
print("Connected COM ports: " + str(connected))
print(port)
time.sleep(2)


ser = serial.Serial(port, 9600, timeout=1)

while(1):
    #d = 0
    a1 = 0
    a2 = 0
    ser.write(b'=')
    a = ser.readline()
    a = str(a)
    # print(a)
    # a = a.replace('\'', '')
    # a = a.replace('b', '')
    # a = a.replace('\\', '')
    # a = a.replace('r', '')
    # a = a.replace('n', '')
    if a == 'b\'BREAK\\r\\n\'' :
        a = re.sub(r'b\'BREAK\\n\\r\'', 'BREAK', a)
        #d = 1
    else:
        if a == 'b\'CheckSum failed\\r\\n\'' :
            a = re.sub(r'b\'CheckSum failed\\n\\r\'', 'CheckSum failed', a)
            #d = 1
        else:
            a = re.sub('[^0123456789\s]', "", a)
            if a.split(' ').__len__() == 2:
                if a.split(' ')[0] != '':
                    a1 = int(a.split(' ')[0])
                if a.split(' ')[1] != '':
                    a2 = int(a.split(' ')[1])

    a3 = a1 * 256 + a2

    if a3 == 0:
        print(time.strftime("%d.%m.%Y %H:%M:%S"), '\t', a)
        file = open('Port_Log.txt', 'a')
        file.write(time.strftime("%d.%m.%Y %H:%M:%S"))
        file.write('\t')
        file.write(str(a))
        file.write('\n')
        file.close()
    else:
        print(time.strftime("%d.%m.%Y %H:%M:%S"), '\t', a3)
        file = open('CO2_Log.txt', 'a')
        file.write(time.strftime("%d.%m.%Y %H:%M:%S"))
        file.write('\t')
        file.write(str(a3))
        file.write('\n')
        file.close()

    time.sleep(2)