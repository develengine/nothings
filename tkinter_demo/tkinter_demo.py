# Az na reklamy dobry tutorial
# https://www.w3schools.com/python/python_getstarted.asp

# Rychlejsi video tutorial
# https://www.youtube.com/watch?v=N4mEzFDjqtA


import tkinter as tk
import math

win_width = 1240
win_height = 720
frame_rate = 60
frame_delay = 1000 // frame_rate
center = complex(win_width // 2, win_height // 2)

frame_counter = 0
descrete_unit = 0.0345 # Basically the speed by which it moves

radius_a = 50
# radius_b = radius_a * 1
# radius_b = radius_a * math.pi
radius_b = radius_a * 1.6180339887

lines = [ ] # List for storing the points of the curve
center_a = 0


def draw():
    ''' Function for drawing to the screen '''
    def draw_circle(x, y, r, cnvs):
        ''' Helper function for drawing circles '''
        cnvs.create_oval(x - r, y - r, x + r, y + r)

    canvas.delete("all") # Clears everithing on the canvas
    draw_circle(center.real, center.imag, radius_b, canvas) # Static circle
    draw_circle(center_a.real, center_a.imag, radius_a, canvas) # Moving circle
    draw_circle(lines[-2], lines[-1], 2, canvas) # Tracing point
    if len(lines) > 2:
        canvas.create_line(*lines, fill = '#FF0000') # The curve

def compute(x):
    ''' Function for computing new point for 'lines'
    and the center of the moving circle '''
    global center_a # Need to say that the variable is outside this function (global),
                    # because we change it's value
    angle_a = descrete_unit * x
    angle_b = angle_a * (radius_a / radius_b)

    rotation_a = complex(math.cos(-angle_a), math.sin(-angle_a))
    rotation_b = complex(math.cos(angle_b), -math.sin(angle_b))
    vector_a = ((rotation_a * rotation_b) * radius_a)
    vector_b = rotation_b * (radius_b + radius_a)

    center_a = center + vector_b
    new_point = center_a + vector_a

    lines.append(new_point.real)
    lines.append(new_point.imag)




root = tk.Tk() # Creates window and saves it into variable 'root'
root.title("Cardioid")

canvas = tk.Canvas(root, width = win_width, height = win_height)
canvas.pack() # Adds canvas to window

def task():
    ''' This function gets called after every frame delay.
    It makes the whole program tick. '''
    global frame_counter

    frame_counter += 1
    compute(frame_counter)
    draw()

    root.after(frame_delay, task) # Calls itself again

root.after(frame_delay, task) # Initiates the 'task' which creates a loop
root.mainloop() # Calls the loop of the window instead of our program
                # Makes it active
