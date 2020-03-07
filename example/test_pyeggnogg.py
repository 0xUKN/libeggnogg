#!/usr/bin/python3.8

import os, time
os.sys.path.append("../bin/pyeggnogg/lib/python3.8/site-packages/")
import pyeggnogg as EggNogg

lib_path = "../bin/libeggnogg.so"
executable_path = "../../eggnoggplus"

EggNogg.init(lib_path, executable_path)
print("Current speed %d" % EggNogg.getSpeed())
time.sleep(2)
EggNogg.setSpeed(15)
print("Current speed %d" % EggNogg.getSpeed())
for i in range(10):
	time.sleep(10)
	print(EggNogg.getGameState())
