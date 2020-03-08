#!/usr/bin/python3.8

import os, time
os.sys.path.append("../bin/pyeggnogg/lib/python3.8/site-packages/")
import pyeggnogg as EggNogg

lib_path = "../bin/libeggnogg.so"
executable_path = "../../eggnoggplus"

EggNogg.init(lib_path, executable_path)
print("Current speed %d" % EggNogg.getSpeed())
time.sleep(5)
EggNogg.setSpeed(15)
#print("Current speed %d" % EggNogg.getSpeed())
while True:
	time.sleep(0.5)
	gs = EggNogg.getGameState()
	if gs['player2']['bounce_ctr'] >= 3:
		print(gs)
