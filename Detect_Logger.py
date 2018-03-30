import serial
import time
config = open('config.txt')
port = config.read().split('\n')[0]
import serial.tools.list_ports
list = serial.tools.list_ports.comports()
connected = []
for element in list:
    connected.append(element.description)
print("Connected COM ports: " + str(connected))
print(port)
time.sleep(5)
ser = serial.Serial(port, 9600, timeout=1)
#ser.port = config.read()
#ser.baudrate = 9600
#ser.timeout = 1
#ser.open()
while(1):
    ser.write(b'=')
    a1 = ser.read()
    a2 = ser.read()
    a3 = int(a1)*256+int(a2)
    print(time.strftime("%d.%m.%Y %H:%M:%S"), '\t', a3)
    file = open('CO2_Log.txt', 'a')
    file.write(time.strftime("%d.%m.%Y %H:%M:%S"))
    file.write('\t')
    file.write(str(a3))
    file.write('\n')
    file.close()
    ser.readline()
    time.sleep(1)