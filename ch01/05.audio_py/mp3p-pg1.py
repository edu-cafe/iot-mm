import pygame as pg

pg.init()
pg.mixer.music.load('kangnam.mp3')
pg.mixer.music.play()
clock = pg.time.Clock()
while pg.mixer.music.get_busy():
	clock.tick(30)
