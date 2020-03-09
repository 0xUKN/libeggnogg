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
print("Current speed %d" % EggNogg.getSpeed())
while True:
	time.sleep(0.5)
	gs = EggNogg.getGameState()
	if True or gs['player2']['life'] < 100:
		print(gs)
		print("\n")
