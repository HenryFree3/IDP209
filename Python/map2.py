from multiprocessing.sharedctypes import Value
from matplotlib.gridspec import GridSpec
from matplotlib.animation import FuncAnimation
import matplotlib.pyplot as plt
from itertools import count
import socket
import numpy as np
import time

"""
Receives, parses and plots the state vector sent by the robot in real time.
"""

def getStateVector():
    """
    Receives the state vector and parses it out into a dictionary.
    """
    try:
        data = s.recv(64).decode("ASCII") # This will need to be lengthened from 64 bytes if the state vector is expanded
        try:
            dataSplit = data.split(" ") # Each part of the state vector is space delimited
            stateVector = {
                # Distinct values within one part of the state vector are comma delimited
                "stateID": dataSplit[1].split(",")[0], # State vector count
                "updateTime": dataSplit[1].split(",")[1], # Time in microseconds since last updateTime was sent
                "displacement": dataSplit[2].split(","), # List of x, y displacement
                "rotation": dataSplit[3], # Rotation in degrees
                "motorSpeeds": dataSplit[4].split(","), # List of left, right motor speed settings, 0-255 
                "motorPIDs": dataSplit[5].split(",") # List of P, I, D values
            }
            return stateVector
        except IndexError:
            print("Bad index into state vector") # Occurs when "data" cannot be interpreted as a valid robot state vector
    except KeyboardInterrupt:
        raise KeyboardInterrupt

# Socket connection setup
HOST = "192.168.4.1" # Robot IP
PORT = 23 # Telnet port
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM) # Websocket
s.settimeout(10) # Timeout for connection, receiving data
s.connect((HOST, PORT))
print("Connected to server.")
s.send(b"Let me in") # The Arduino doesn't seem to recognise the connection until a client sends some bytes

# Plot setup
windowLength = 10

# Style choices for route map
routeColour = "orange"
arrowLength = 150/np.sqrt(2)
arrowWidth = 60
lineWidth = 3

# Subplot setup
rows = 2
columns = 3
fig = plt.figure(figsize=(18, 10))
gs = GridSpec(rows, columns)
ax1 = plt.subplot2grid((rows,columns),(0,columns-1))
ax2 = plt.subplot2grid((rows,columns),(1,columns-1))
ax4 = plt.subplot2grid((rows,columns),(0,0), rowspan=rows, colspan=columns-1)

# Collection of variable lists, added to by new state vector data
# Co-ordinates of robot on arena, with respect to bottom left corner, mm
xVals = [1200]
yVals = [1200]
# State vector IDs
IDs = []
# Motor power settings
leftSpeeds = [0]
rightSpeeds = [0]
# Headings measured clockwise from +y direction
headings = [45]
# Elapsed time between sequential state vectors
times = [0]
# Encoder PID controller terms
pTerms = [0]
iTerms = [0]
dTerms = [0]

# Motor speed line objects
line1_1 = ax1.plot(times, leftSpeeds, color="blue", label="Left Motor")[0]
line1_2 = ax1.plot(times, rightSpeeds, color="red", label="Right Motor")[0]
ax1.set_ylim(0, 255)
#ax1.set_xlabel("Elapsed time")
ax1.set_ylabel("Power setting")
ax1.legend(loc="upper left")

# PID term line objects
line2_1 = ax2.plot(times, pTerms, color="blue", label="Proportional")[0]
line2_2 = ax2.plot(times, iTerms, color="red", label="Integral")[0]
line2_3 = ax2.plot(times, dTerms, color="green", label="Derivative")[0]
ax2.set_ylim(-75, 75)
ax2.set_xlabel("Elapsed time")
ax2.set_ylabel("Controller term")
ax2.legend(loc="upper left")

# Map image and line object
img = plt.imread("Arena.png")
ax4.set_xlim(0, 2400)
ax4.set_ylim(0, 2400)
ax4.imshow(img, extent=(0, 2400, 0, 2400))
ax4.axis("off")
line4 = ax4.plot(xVals, yVals, color=routeColour, lw=lineWidth, linestyle="dashed")[0]

# Final plot configuration
fig.canvas.draw()
plt.grid(False)
plt.tight_layout()
plt.show(block=False)

# Continuously checks socket for data and updates plot
try:
    while True:
        #time_start = time.time() # Used for loop performance analysis
        newState = getStateVector()
        #time_get = time.time() # Used for loop performance analysis
        if type(newState) is dict: # Check we don't have a none type, e.g. if something
                                    # other than a state vector is sent over the socket
            IDs.append(int(newState["stateID"]))
            if len(IDs) > 1: # Check this isn't the first state vector received: No need to check adjacent ID values yet
                if IDs[-1] != (IDs[-2] + 1):
                    # Check received state vectors are in sequence
                    raise ValueError(f"stateIDs of last two state vectors are not consecutive: Old: {IDs[-2]}, New: {IDs[-1]}")
                    break

            # Now we are sure we have received the correct state vector, update:
            xVals.append(xVals[-1]+int(newState["displacement"][0]))
            yVals.append(yVals[-1]+int(newState["displacement"][1]))
            headings.append(headings[-1]+int(newState["rotation"]))
            leftSpeeds.append(int(newState["motorSpeeds"][0]))
            rightSpeeds.append(int(newState["motorSpeeds"][1]))
            pTerms.append(int(newState["motorPIDs"][0]))
            iTerms.append(int(newState["motorPIDs"][1]))
            dTerms.append(int(newState["motorPIDs"][2]))
            times.append(times[-1]+float(newState["updateTime"])/1E6)

            # Update lines to include above data
            line1_1.set_xdata(times)
            line1_2.set_xdata(times)
            line2_1.set_xdata(times)
            line2_2.set_xdata(times)
            line2_3.set_xdata(times)

            line1_1.set_ydata(leftSpeeds)
            line1_2.set_ydata(rightSpeeds)
            line2_1.set_ydata(pTerms)
            line2_2.set_ydata(iTerms)
            line2_3.set_ydata(dTerms)

            line4.set_xdata(xVals)
            line4.set_ydata(yVals)

            # For the right hand side plots, move the window after a set time to keep them readable
            if times[-1] < windowLength:
                ax1.set_xlim(0, times[-1])
                ax2.set_xlim(0, times[-1])
            else:
                ax1.set_xlim(times[-1]-windowLength, times[-1])
                ax2.set_xlim(times[-1]-windowLength, times[-1])

            # Delete the old arrow showing the robot heading, if there is one, and create the new arrow
            if len(IDs) > 1:
                arrow.remove()
            arrow = ax4.arrow(xVals[-1], yVals[-1], arrowLength*np.sin(headings[-1]), arrowLength*np.cos(headings[-1]),
                              width=0, head_width=arrowWidth, length_includes_head=True, head_length=arrowLength,
                              edgecolor = routeColour, facecolor = routeColour)
            fig.canvas.draw()
            fig.canvas.flush_events()
            #print(time.time() - time_start, time.time()-time_get) # Used for loop performance analysis
except KeyboardInterrupt:
    raise KeyboardInterrupt
except ValueError:
    raise ValueError
except TypeError:
    raise TypeError
