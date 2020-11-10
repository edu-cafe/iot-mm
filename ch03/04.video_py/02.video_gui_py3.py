#coding: utf-8

import picamera
from tkinter import *

def rec_vdo():
	if res.get() == 3:
		camera.resolution = (1024, 768)
	elif res.get() == 2:
		camera.resolution = (640, 480)
	else:
		camera.resolution = (320, 240)
		
	if rot.get() == 4:
		camera.rotation = int(rotation[3][0])
	elif rot.get() == 3:
		camera.rotation = int(rotation[2][0])
	elif rot.get() == 2:
		camera.rotation = int(rotation[1][0])
	else:
		camera.rotation = int(rotation[0][0])
		
	filename = et.get()
	camera.start_recording(filename + '.h264')
	camera.wait_recording(5)
	camera.stop_recording()
	
	label4 = Label(root, text='Movie Saved')
	label4.pack()
	
root = Tk()

with picamera.PiCamera() as camera:
	resolution = [('320x240', 1), ('640x480', 2), ('1024x768', 3)]
	rotation = [('0', 1), ('90', 2), ('180', 3), ('270', 4)]
	
	res = IntVar()
	res.set(1);
	
	rot = IntVar()
	rot.set(1)
	
	label1 = Label(root, text='Resolution')
	label1.pack()	
	for text, mode in resolution:
		rb1 = Radiobutton(root, text=text, 
				variable=res, value=mode)
		rb1.pack(anchor='w')
		
	label2 = Label(root, text='Rotation')
	label2.pack()	
	for text, mode in rotation:
		rb2 = Radiobutton(root, text=text, 
				variable=rot, value=mode)
		rb2.pack(anchor='w')

	label3 = Label(root, text='Save Filename')
	label3.pack()
	et = Entry(root)
	et.pack()
	
	btn = Button(root, text='Picture', width=10, command=rec_vdo)
	btn.pack()
	
	root.mainloop()


