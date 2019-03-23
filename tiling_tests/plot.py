# Get command-line file name.
import sys
if len(sys.argv) < 2:
    print('Provide a file name.')
    sys.exit()
filename = sys.argv[1]

if len(sys.argv) > 2:
    zoom = float(sys.argv[2])
else:
    zoom = 10

# Read lines from CSV file.
import csv
with open(filename, 'r') as f:
    reader = csv.reader(f)
    lines = [[float(t) * zoom for t in line] for line in reader]

# Find the scale of the drawing to fit.
size = (1800, 1000)

# Create UI to display the lines.
import aggdraw
import tkinter
root = tkinter.Tk()

# Draw the lines.
draw = aggdraw.Dib("RGB", size)
for line in lines:
    draw.line(line, aggdraw.Pen("black"))

# Display the image.
frame = tkinter.Frame(root, width=size[0], height=size[1], bg="")
frame.bind("<Expose>", lambda e: draw.expose(hwnd=e.widget.winfo_id()))
frame.pack()

frame.mainloop()

