#!/usr/bin/python3.8

import os, time
os.sys.path.append("../bin/pyeggnogg/lib/python3.8/site-packages/")
import pyeggnogg as EggNogg
from math import trunc as truncate

#   x:0-->
# y
# 0
# |
# v

ROOM_WIDTH = 33 #blocks
ROOM_HEIGHT = 12 #blocks
BLOCK_HEIGHT = 16 #pixels
BLOCK_WIDTH = 16 #pixels
ROOM_REAL_HEIGHT = BLOCK_HEIGHT * ROOM_HEIGHT
ROOM_REAL_WIDTH = BLOCK_WIDTH * ROOM_WIDTH
MAP_ROOF = 0 #pixels
MAP_LEFT = 0 #pixels
MAPPINGS = {'v':'down_pikes', 'w':'water', '@':'wall', '*':'sword', 'X':'up_pikes', '+':'mine', '^':'win_surface', 'E':'lava', '~':'back_fall', 'O':'back_sun1', ' ':'back_air', 'F':'back_column1', 'H':'back_column2', 'I':'back_column3', '_':'back_chandelier_support', 'C':'back_chandelier1', 'G':'back_sun2', '#':'back_crack', 'S':'back_eyes', 'P':'back_hiero', 'A':'back_mill', '=':'back_grilling1', '|':'back_column4', 'f':'back_column5', 'L':'back_perso1', 'N':'back_perso2', 'Y':'back_perso3', ':':'back_column6', '-':'back_line', 'Q':'back_skull1', 't':'back_tentacle1', 'T':'back_tentacle2', 's':'back_tentacle3', '(':'back_left_sewers', ')':'back_right_sewers', 'c':'back_chandelier2', 'Z':'back_bluesquare', 'x':'back_grilling2', 'q':'back_skull2', 'u':'back_sewers2', 'K':'moving_pikes', '.':'back_black', 'Z':'back_blue', 'i':'back_people', '`':'back_grilling3'}
BACKGROUND_SYMBOLS = "`Iz.KuqxZc)(sTtQ-:YNLf|=APS#GC_IHFO~"

lib_path = "../bin/libeggnogg.so"
executable_path = "../../eggnoggplus"

def show_map(map_str, simplified = True):
	out = '\n'.join([map_str[i:i+ROOM_WIDTH] for i in range(0, len(map_str), ROOM_WIDTH)])
	if simplified:
		for symbol in BACKGROUND_SYMBOLS:
			out = out.replace(symbol, ' ')
	print(out)

def pixels_to_block_coord(x, y):
	y_ = truncate(y) // BLOCK_HEIGHT
	absolute_x = truncate(x) // BLOCK_WIDTH
	x_ = absolute_x % ROOM_WIDTH
	return x_, y_

def add_players_to_roomdef(gs, mapstring):
	x_1, y_1 = pixels_to_block_coord(gs["player1"]["pos_x"], gs["player1"]["pos_y"])
	x_2, y_2 = pixels_to_block_coord(gs["player2"]["pos_x"], gs["player2"]["pos_y"])
	roomdef = list(mapstring)
	roomdef[x_2 + y_2 * ROOM_WIDTH] = '2'
	roomdef[x_1 + y_1 * ROOM_WIDTH] = '1'
	return ''.join(roomdef)
	

EggNogg.init(lib_path, executable_path)
#EggNogg.setSpeed(15)

prev_map = -1
while True:
	try:
		gs = EggNogg.getGameState()
		print("PLAYING ON : %s - SPEED : %d" % (EggNogg.getMapName(), EggNogg.getSpeed()))
		roomdef = add_players_to_roomdef(gs, EggNogg.getRoomDef())
		show_map(roomdef)
		pos_p1 = {'pos_x':round(gs['player1']['pos_x'],1), 'pos_y':round(gs['player1']['pos_y'],2), 'sword_pos_x':round(gs['player1']['sword_pos_x'],1), 'sword_pos_y':round(gs['player1']['sword_pos_y'],1)}
		pos_p2 = {'pos_x':round(gs['player2']['pos_x'],1), 'pos_y':round(gs['player2']['pos_y'],1), 'sword_pos_x':round(gs['player2']['sword_pos_x'],1), 'sword_pos_y':round(gs['player2']['sword_pos_y'],1)}
		del gs['player1']['pos_x']
		del gs['player1']['pos_y']
		del gs['player2']['pos_x']
		del gs['player2']['pos_y']
		del gs['player1']['last_pos_x']
		del gs['player1']['last_pos_y']
		del gs['player2']['last_pos_x']
		del gs['player2']['last_pos_y']
		del gs['player1']['sword_pos_x']
		del gs['player1']['sword_pos_y']
		del gs['player2']['sword_pos_x']
		del gs['player2']['sword_pos_y']
		print("STATS PLAYER 1\n=>", gs['player1'])
		print("STATS PLAYER 2\n=>", gs['player2'])
		print("POSITION PLAYER 1\n=>", pos_p1)
		print("POSITION PLAYER 2\n=>", pos_p2)
		#if False and gs['winner'] != 0:
		#	EggNogg.resetGame()
		time.sleep(0.1)
		os.system('clear')
	except Exception as e:
		print("An error occured : ", e)
		print("[+] Exiting !")
		break



