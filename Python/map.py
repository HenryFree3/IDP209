from telnetlib import Telnet
import random
from itertools import count
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import telnetlib
import matplotlib.pyplot as plt

#HOST = "192.168.4.1"
HOST = "LOCALHOST"

#tn = telnetlib.Telnet(HOST)

plt.style.use('fivethirtyeight')

x_vals = [1200]
y_vals = [1200]

index = count()

img = plt.imread("Arena.png")
fig, ax = plt.subplots(figsize=(12, 12))
ax.set_xlim(0, 2400)
ax.set_ylim(0, 2400)
ax.imshow(img, extent=(0, 2400, 0, 2400))

plt.grid(False)
plt.axis("off")

line = ax.plot(x_vals, y_vals)[0]

def animate(i):
    x_vals.append(x_vals[-1]+random.randint(-50, 51))
    y_vals.append(y_vals[-1]+random.randint(-50, 51))
    line.set_xdata(x_vals)
    line.set_ydata(y_vals)
    fig.tight_layout()


ani = FuncAnimation(fig, animate, interval=100)

plt.draw()
plt.show()