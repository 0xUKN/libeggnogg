#!/usr/bin/python3.8

import os, time
os.sys.path.append("../bin/pyeggnogg/lib/python3.8/site-packages/")
import pyeggnogg as EggNogg

lib_path = "../bin/libeggnogg.so"
executable_path = "../../eggnoggplus"

def show_map(map_str):
	width = 33
	for line in [map_str[i:i+width] for i in range(0, len(map_str), width)]:
		print(line)

EggNogg.init(lib_path, executable_path)
print("Current speed %d" % EggNogg.getSpeed())
#EggNogg.setSpeed(15)
print("Current speed : %d" % EggNogg.getSpeed())
prev_map = -1
print("Playing on : %s" % EggNogg.getMapName())
while True:
	time.sleep(1)
	gs = EggNogg.getGameState()
	print(gs)
	print("\n")
	if gs['room_number'] != prev_map:
		print("Current room definition :")
		show_map(EggNogg.getRoomDef())
		prev_map = gs['room_number']
