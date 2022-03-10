from multiprocessing.sharedctypes import Value
from matplotlib.gridspec import GridSpec
from matplotlib.widgets import Button
from matplotlib.widgets import TextBox
from matplotlib.animation import FuncAnimation
import matplotlib.pyplot as plt
from itertools import count
import socket
import numpy as np
import time

"""
Receives, parses and plots the state vector sent by the robot in real time.
"""



s = socket.socket(socket.AF_INET, socket.SOCK_STREAM) # Websocket initialisation
blit = True
stableTime = 2 # Time to wait before starting plotting

def startCallback():
    
    # Get user selected case, start co-ordinates from Matplotlib GUI
    # Send through socket
    s.send(b"Command Message")

def getStateVector():
    """
    Requests a state vector and parses it out into a dictionary.
    """
    try:
        #time_prereceive = time.time() # Used for loop performance analysis
        data = s.recv(128).decode("ASCII") # This will need to be lengthened from 64 bytes if the state vector is expanded
        #time_postreceive = time.time()
        #print(f"Received in {time_postreceive-time_prereceive:.4f} s") # Used for loop performance analysis
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

# Plot setup
windowLength = 10

# Style choices for route map
routeColour = "orange"
arrowLength = 100/np.sqrt(2)
arrowWidth = 60
lineWidth = 3

# Subplot setup
rows = 3
columns = 3
fig = plt.figure(figsize=(20, 10), dpi=100)
gs = GridSpec(rows, columns)
ax1 = plt.subplot2grid((rows,columns),(0,columns-1))
ax2 = plt.subplot2grid((rows,columns),(1,columns-1))
ax3 = plt.subplot2grid((rows, columns),(2, columns-1))
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
generationTimes = [0]
# Encoder PID controller terms
pTerms = [0]
iTerms = [0]
dTerms = [0]
# Performance metrics
plotTimes = [0]

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
#ax2.set_xlabel("Elapsed time")
ax2.set_ylabel("Controller term")
ax2.legend(loc="upper left")

# Performance line objects
line3_1 = ax3.plot(times, plotTimes, color="blue", label="Processing (PC)")[0]
line3_2 = ax3.plot(times, generationTimes, color="red", label="Generation (Robot)")[0]
ax3.set_ylim(0, 1)
ax3.set_xlabel("Elapsed time, $s$")
ax3.set_ylabel("Compute time, $s$")
ax3.legend(loc="upper left")

# Map image and line object
img = plt.imread("Arena.png")
ax4.set_xlim(0, 2400)
ax4.set_ylim(0, 2400)
ax4.imshow(img, extent=(0, 2400, 0, 2400))
ax4.axis("off")
line4 = ax4.plot(xVals, yVals, color=routeColour, lw=lineWidth, linestyle="dashed")[0]

# Command button
startAxes = plt.axes([0.03, 0.15, 0.02, 0.015])
buttonStart = Button(startAxes, "Send")
#buttonStart.on_clicked(startCallback())

# Final plot configuration
fig.canvas.draw()
plt.grid(False)
plt.tight_layout()
plt.show(block=False)
#generationTimes.pop()
# Allow a long time to show the initial plot
# This is because on the first draw, the arena map
# must be loaded and the window is fullscreened.
# In subsequent draws, blitting is used.

plt.get_current_fig_manager().full_screen_toggle()
plt.pause(0.01) # Small time to go fullscreen
arenaBackground = fig.canvas.copy_from_bbox(ax4.bbox)

# Need to disable axes here so only dynamic axes, redrawn on each update, are included when blitting
ax1.axis("off")
ax2.axis("off")
ax3.axis("off")
plt.pause(0.01) # Seems to copy with static axes on still unless there is a small delay
ax1Background = fig.canvas.copy_from_bbox(ax1.get_figure().bbox)
ax2Background = fig.canvas.copy_from_bbox(ax2.get_figure().bbox)
ax3Background = fig.canvas.copy_from_bbox(ax2.get_figure().bbox)
ax1.axis("on")
ax2.axis("on")
ax3.axis("on")

# Websocket final setup
HOST = "192.168.4.1" # Robot IP
PORT = 23 # Telnet port
s.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1) # Faster TCP
s.settimeout(5) # Timeout for connection, receiving data
s.connect((HOST, PORT))
s.send(b"Let me in") # The Arduino can't seem to recognise the connection until a client sends some bytes

ack = s.recv(64).decode("ASCII")
while ack != "New client connection": # Wait for connection
    ack = s.recv(64).decode("ASCII")
print("Connected to server.")
time.sleep(0.5)

time_end = time.time()
while (time.time()-time_end < stableTime) :
    getStateVector()
time_end = time.time()

# Continuously request state vector from robot and updates plot
try:
    while True:
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
                #else:
                    #plotTimes.append(time.time()-time_end)
                    #time_end = time.time()

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
            generationTimes.append(float(newState["updateTime"])/1E6)

            plotTimes.append(time.time()-time_end)
            time_end = time.time()

            # For the right hand side plots, move the window after a set time to keep them readable
            if times[-1] < windowLength:
                ax1.set_xlim(0, times[-1])
                ax2.set_xlim(0, times[-1])
                ax3.set_xlim(0, times[-1])
                windowIndex = 1 
                # Offset from 0 is needed because we want to make
                # sure we just plot data past the window later
                # windowIndex keeps the data plotted as small as needed,
                # to just cover the window, and is necessary to
                # find dynamic y axis limits for ax2 and ax3 anyway.
            else:
                ax1.set_xlim(times[-1]-windowLength, times[-1])
                ax2.set_xlim(times[-1]-windowLength, times[-1])
                ax3.set_xlim(times[-1]-windowLength, times[-1])
                new = np.array(times) + windowLength - times[-1]
                windowIndex = int(np.argmin(abs(new)))
                ax3.set_ylim(0, np.max((plotTimes[windowIndex:], generationTimes[windowIndex:]))*1.05)

            # Update lines to include above data
            line1_1.set_data(times[windowIndex-1:], leftSpeeds[windowIndex-1:])
            line1_2.set_data(times[windowIndex-1:], rightSpeeds[windowIndex-1:])
            line2_1.set_data(times[windowIndex-1:], pTerms[windowIndex-1:])
            line2_2.set_data(times[windowIndex-1:], iTerms[windowIndex-1:])
            line2_3.set_data(times[windowIndex-1:], dTerms[windowIndex-1:])
            line3_1.set_data(times[windowIndex-1:], plotTimes[windowIndex-1:])
            line3_2.set_data(times[windowIndex-1:], generationTimes[windowIndex-1:])
            line4.set_data(xVals, yVals)

            # Delete the old arrow showing the robot heading, if there is one, and create the new arrow
            if len(IDs) > 1:
                arrow.remove()
            arrow = ax4.arrow(xVals[-1], yVals[-1], arrowLength*np.sin(headings[-1]), arrowLength*np.cos(headings[-1]),
                              width=0, head_width=arrowWidth, length_includes_head=True, head_length=arrowLength,
                              edgecolor = routeColour, facecolor = routeColour)
            
            if blit: # Avoid updating the static map image, but redraw everything else
                fig.canvas.restore_region(arenaBackground)
                fig.canvas.restore_region(ax1Background)
                fig.canvas.restore_region(ax2Background)
                fig.canvas.restore_region(ax3Background)
                ax1.draw_artist(ax1)
                ax2.draw_artist(ax2)
                ax3.draw_artist(ax3)
                ax4.draw_artist(line4)
                ax4.draw_artist(arrow)
                fig.canvas.blit(ax4.bbox)
                fig.canvas.blit(ax1.clipbox)
                fig.canvas.blit(ax2.clipbox)
                fig.canvas.blit(ax3.clipbox)
            else: # Redraw all
                fig.canvas.draw()
            
            fig.canvas.flush_events()
            time_end = time.time()
            #print(time.time() - time_start, time.time()-time_get) # Used for loop performance analysis
except KeyboardInterrupt:
    raise KeyboardInterrupt
except ValueError:
    raise ValueError
except TypeError:
    raise TypeError