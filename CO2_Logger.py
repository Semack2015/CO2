import time
import re
import serial.tools.list_ports

config = open('config.txt')


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
    # d = 0
    a1 = -1
    ser.write(b'=')
    a = ser.readline()
    a = str(a)
    # print(a)
    # a = a.replace('\'', '')
    # a = a.replace('b', '')
    # a = a.replace('\\', '')
    # a = a.replace('r', '')
    # a = a.replace('n', '')
    if a == 'b\'BREAK\\r\\n\'':
        a = "BREAK"
        # d = 1
    else:
        if a == 'b\'CheckSum failed\\r\\n\'' :
            a = "CheckSum failed"
            # d = 1
        else:
            if a == 'b\'Preheat\\r\\n\'':
                a = "Preheat"
            else:
                a = re.sub('[^0123456789]', "", a)
            # if a.split(' ').__len__() == 2:
            #     if a.split(' ')[0] != '':
            #         a1 = int(a.split(' ')[0])
            #     if a.split(' ')[1] != '':
            #         a2 = int(a.split(' ')[1])

    # a3 = a1 * 256 + a2

    if a1 == -1:
        print(time.strftime("%d.%m.%Y\t%H:%M:%S"), '\t', a)
        file = open('Port_Log.txt', 'a')
        file.write(time.strftime("%d.%m.%Y\t%H:%M:%S"))
        file.write('\t')
        file.write(str(a))
        file.write('\n')
        file.close()
    else:
        print(time.strftime("%d.%m.%Y\t%H:%M:%S"), '\t', a1)
        # date = time.strftime("%d.%m.%Y CO2_Log.txt")
        date = 'CO2_Logger.txt'
        file = open(date, 'a')
        file.write(time.strftime("%d.%m.%Y\t%H:%M:%S"))
        file.write('\t')
        file.write(str(a1))
        file.write('\n')
        file.close()

    time.sleep(2)