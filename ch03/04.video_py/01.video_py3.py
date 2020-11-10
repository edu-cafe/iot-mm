#coding: utf-8

import picamera

with picamera.PiCamera() as camera:
	res = int(input('Resolution(1:320x240, 2:640x480, 3:1024x768)? '))	
	if res == 3:
		camera.resolution = (1024, 768)
	elif res == 2:
		camera.resolution = (640, 480)
	else:
		camera.resolution = (320, 240)

	filename = input('File Name? ')
	camera.start_preview()
	camera.start_recording(output = filename + '.h264')
	camera.wait_recording(5)
	camera.stop_preview()
	camera.stop_recording()

