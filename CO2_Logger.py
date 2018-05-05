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
    d = -1
    ser.write(b'=')
    a = ser.readline()
    a = str(a)
    a = re.sub(r"[b\'\\]", '', a)
    a = re.sub(r"\.rn", '', a)

    if a.find('#')>=0:
        a = a.replace('#', '')
        m = a.split(' ')
        d = 0;




    if d == -1:
        print(time.strftime("%d.%m.%Y %H:%M:%S"), '\t', a)
        file = open('Port_Log.txt', 'a')
        file.write(time.strftime("%d.%m.%Y\t%H:%M:%S"))
        file.write('\t')
        file.write(str(a))
        file.write('\n')
        file.close()
    else:
        print(time.strftime("%d.%m.%Y %H:%M:%S"), '\t', m[0], ' ', m[1], ' ', m[2])
        date = time.strftime("%d.%m.%Y CO2_Log.txt")
        file = open(date, 'a+')
        file.write(time.strftime("%d.%m.%Y\t%H:%M:%S"))
        file.write('\t')
        file.write(str(m[0]))
        file.write('\t')
        file.write(str(m[1]))
        file.write('\t')
        file.write(str(m[2]))
        file.write('\n')
        file.close()

    time.sleep(2)