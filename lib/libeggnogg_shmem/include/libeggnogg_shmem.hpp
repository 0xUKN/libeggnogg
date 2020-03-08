#pragma once
#define MAX_SWORD_NUMBER 16

namespace LibEggnogg
{

	enum PlayerAction {DEFAULT, DEAD, DUCK, EGGNOGG, JUMP, KICK, NOTHING, PUNCH, RUN, STAB, STANCE, STUN, THROW};
	typedef enum PlayerAction PlayerAction;

	struct Player 
	{
		unsigned char life;
		float pos_x;
		float pos_y;
		bool hasSword;
		float sword_pos_x;
		float sword_pos_y;
		signed char direction;
		unsigned char bounce_ctr;
		unsigned char situation;
		unsigned char keys_pressed;
		PlayerAction action;
	};
	typedef struct Player Player;

	struct Sword
	{
		float pos_x;
		float pos_y;
	};
	typedef struct Sword Sword;

	struct GameState
	{
		Player player1;
		Player player2;
		unsigned char leader;
		unsigned char room_number;
		unsigned char total_room_number;
		unsigned char winner;
		unsigned char nb_swords;
		Sword swords[MAX_SWORD_NUMBER];
	};
	typedef struct GameState GameState;

	void* CreateSharedMemory(std::string name, unsigned int size);
	int RemoveSharedMemory(std::string name);
	GameState* InitGameState();
	void CloseSharedMemory();
}
