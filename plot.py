#!/usr/bin/env python2
# Plot a graph of Data which is comming in on the fly from the server
# uses pylab
# Author: Amziab MOhamed
# Date: 2013

import pylab
from pylab import *

xAchse=pylab.arange(0,50,1)
yAchse=pylab.array([5000]*50)

fig = pylab.figure(1)
ax = fig.add_subplot(111)
ax.grid(True)
ax.set_title("Realtime Plot")
ax.set_xlabel("Time(s)")
ax.set_ylabel("Bitrate")
ax.axis([0,100,3000,5300])
line1=ax.plot(xAchse,yAchse,'-',lw=2)
manager = pylab.get_current_fig_manager()

values=[]
values = [5000 for x in range(50)]


f2 = open('data.out', 'r')
lines = f2.readlines()
f2.close()
line=lines[len(lines)-1]
p = line.split()
T0=p[0]
T1=p[1]

def read_data(arg):
  global values,T1,h,T0
  f2 = open('data.out', 'r')
  lines = f2.readlines()
  f2.close()
  line = lines[len(lines)-1]
  p = line.split()
  values.append(p[1])
  T0=T1
  T1=p[1]

def RealtimePloter(arg):
  global values,T1
  CurrentXAxis=pylab.arange(len(values)-50,len(values),1)
  CurrentYAxis=pylab.array(values[-50:])
  new_values=map(int,values[-50:])
  ymin, ymax = ax.get_ylim()
  line1[0].set_data(CurrentXAxis,pylab.array(values[-50:]))
  ax.axis([CurrentXAxis.min(),CurrentXAxis.max()+10,int(min(new_values))-1000,1000+int(max(new_values))])

  #ax.axis([CurrentXAxis.min(),CurrentXAxis.max()+10,3000,5300])
  manager.canvas.draw()


timer = fig.canvas.new_timer(interval=950)
timer.add_callback(RealtimePloter, ())
timer2 = fig.canvas.new_timer(interval=950)
timer2.add_callback(read_data, ())
timer.start()
timer2.start()

pylab.show()
