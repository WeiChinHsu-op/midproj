import numpy as np
import serial
import time

waitTime = 0.1

# generate the waveform table
signalLength = 96
#t = np.linspace(0, 2*np.pi, signalLength)
#signalTable = (np.sin(t) + 1.0) / 2.0
song0 = [330,294,261,294,330,330,330,330,
  294,294,294,294,330,392,392,392,
  330,294,261,294,330,330,330,330,
  294,294,330,294,261,261,261,261]
song1 = [392,330,330,330,349,294,294,294,
  261,294,330,349,392,392,392,392,
  392,330,330,330,349,294,294,294,
  261,330,392,392,261,261,261,261]
song2 = [261,294,330,261,261,294,330,261,
  330,349,392,392,330,349,392,392,
  440,349,330,261,440,349,330,261,
  261,294,261,261,261,294,261,261]
# output formatter
song = [330,294,261,294,330,330,330,330,
  294,294,294,294,330,392,392,392,
  330,294,261,294,330,330,330,330,
  294,294,330,294,261,261,261,261,
  392,330,330,330,349,294,294,294,
  261,294,330,349,392,392,392,392,
  392,330,330,330,349,294,294,294,
  261,330,392,392,261,261,261,261,
  261,294,330,261,261,294,330,261,
  330,349,392,392,330,349,392,392,
  440,349,330,261,440,349,330,261,
  261,294,261,261,261,294,261,261]
formatter = lambda x: "%d" % x

# send the waveform table to K66F


serdev = '/dev/ttyACM0'
s = serial.Serial(serdev)
print("Sending signal ...")
print("It may take about %d seconds ..." % (int(signalLength * waitTime)))
for data in song:
  print(data)
  s.write(bytes(formatter(data),'UTF-8'))
  time.sleep(waitTime)
s.close()
print("Signal sended")


'''
while (1):
  serdev = '/dev/ttyACM0'
  s = serial.Serial(serdev)
  nowsong = '0'
  nowsong = s.read()
  if int(nowsong)==0:
    print("Sending signal ...")
    print("It may take about %d seconds ..." % (int(signalLength * waitTime)))
    for data in song0:
      s.write(bytes(formatter(data), 'UTF-8'))
      time.sleep(waitTime)
    s.close()
    print("Signal sended")
  if int(nowsong)==1:
    print("Sending signal ...")
    print("It may take about %d seconds ..." % (int(signalLength * waitTime)))
    for data in song1:
      s.write(bytes(formatter(data), 'UTF-8'))
      time.sleep(waitTime)
    s.close()
    print("Signal sended")
  if int(nowsong)==2:
    print("Sending signal ...")
    print("It may take about %d seconds ..." % (int(signalLength * waitTime)))
    for data in song2:
      s.write(bytes(formatter(data), 'UTF-8'))
      time.sleep(waitTime)
    s.close()
    print("Signal sended")
'''




    

