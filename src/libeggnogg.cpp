#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <dlfcn.h>
#include <thread>
#include "../include/libeggnogg.hpp"
#include "../lib/libeggnogg_shmem/include/libeggnogg_shmem.hpp"
#include "../include/libeggnogg_rpc.hpp"
//#define DEBUG
#define LIBSDL "/usr/lib/x86_64-linux-gnu/libSDL2-2.0.so.0.10.0"
#define SDL_NumJoysticks_GOT 0x665158
#define LOGIC_RATE_ADDRESS 0x665418
#define LIFE_OFFSET 0x9a
#define POSX_OFFSET 0x24
#define POSY_OFFSET 0x28
#define ISDISARMED_OFFSET 0x11
#define SWORD_POSX_OFFSET 0x54
#define SWORD_POSY_OFFSET 0x58
#define DIRECTION_OFFSET 0x98
#define BOUNCE_CTR_OFFSET 0x90
#define SITUATION_OFFSET 0xad
#define KEYS_PRESSED_OFFSET 0x9f
#define ACTION_OFFSET 0x158
#define PLAYER1_ADDRESS 0x75b3e0
#define PLAYER2_ADDRESS 0x75b540
#define LEADER_ADDRESS 0x75b260
#define ROOM_NUMBER_ADDRESS 0x75b268
#define TOTAL_ROOM_NUMBER_ADDRESS 0x763938
#define NUM_THINGS_ALLOCATED_ADDRESS 0x75c880

namespace LibEggnogg
{
	__attribute__((constructor))void _init(void)
	{
		void * libSDL_handle;

		puts("[+] Library loaded !");

		std::thread background_task(init_libeggnogg_rpc_serv); 
		background_task.detach();
		atexit(exit_libeggnogg_rpc_serv);
		atexit(CloseSharedMemory);

		logic_rate = (unsigned long*)LOGIC_RATE_ADDRESS;
		SDL_NumJoysticks_real_GOT = (void**)SDL_NumJoysticks_GOT;
		*SDL_NumJoysticks_real_GOT = (void*)&SDL_NumJoysticks_hook;

		if((libSDL_handle = dlopen(LIBSDL, RTLD_LAZY)) == NULL)
		{
			fprintf(stderr, "[-] Hooking failed : could not find libSDL !\n");
			throw std::runtime_error("[-] Hooking failed : could not find libSDL !");
		}
		if((SDL_NumJoysticks_real = dlsym(libSDL_handle, "SDL_NumJoysticks")) == NULL)
		{
			fprintf(stderr, "[-] Hooking failed : could not find SDL_NumJoysticks !\n");
			throw std::runtime_error("[-] Hooking failed : could not find SDL_NumJoysticks !");
		}
		if((gs = InitGameState()) == NULL)
		{
			fprintf(stderr, "[-] Game state initialization failed !\n");
			throw std::runtime_error("[-] Game state initialization failed !");
		}
		memset(gs, 0, sizeof(LibEggnogg::GameState));

		#ifdef DEBUG
		*logic_rate = 5;
		printf("Real SDL_NumJoysticks address : %p\n", SDL_NumJoysticks_real);
		printf("Fake SDL_NumJoysticks address : %p\n", SDL_NumJoysticks_hook);
		printf("GOT SDL_NumJoysticks value : %p\n", *SDL_NumJoysticks_real_GOT);
		#endif

		puts("[+] Shared memory initialization successful !");
		puts("[+] Main loop hooking successful !");
	}

	int SDL_NumJoysticks_hook(void)
	{
		#ifdef DEBUG
		puts("[+] Main loop hook called !");
		#endif

		UpdateGameState();
		return ((int (*)(void))SDL_NumJoysticks_real)();
	}

	//Shared Memory Functions
	void UpdateGameState()
	{
		gs->player1.life = *(unsigned char *)(PLAYER1_ADDRESS + LIFE_OFFSET);
		gs->player2.life = *(unsigned char *)(PLAYER2_ADDRESS + LIFE_OFFSET);

		gs->player1.pos_x = *(float *)(PLAYER1_ADDRESS + POSX_OFFSET);
		gs->player2.pos_x = *(float *)(PLAYER2_ADDRESS + POSX_OFFSET);

		gs->player1.pos_y = *(float *)(PLAYER1_ADDRESS + POSY_OFFSET);
		gs->player2.pos_y = *(float *)(PLAYER2_ADDRESS + POSY_OFFSET);

		gs->player1.hasSword = !(*(bool *)(PLAYER1_ADDRESS + ISDISARMED_OFFSET));
		gs->player2.hasSword = !(*(bool *)(PLAYER2_ADDRESS + ISDISARMED_OFFSET));
		
		if(gs->player1.hasSword)
		{
			gs->player1.sword_pos_x = *(float *)(PLAYER1_ADDRESS + SWORD_POSX_OFFSET);
			gs->player1.sword_pos_y = *(float *)(PLAYER1_ADDRESS + SWORD_POSY_OFFSET);
		}
		else
		{
			gs->player1.sword_pos_x = -1;
			gs->player1.sword_pos_y = -1;
		}	
		if(gs->player2.hasSword)
		{
			gs->player2.sword_pos_x = *(float *)(PLAYER2_ADDRESS + SWORD_POSX_OFFSET);
			gs->player2.sword_pos_y = *(float *)(PLAYER2_ADDRESS + SWORD_POSY_OFFSET);
		}
		else
		{
			gs->player2.sword_pos_x = -1;
			gs->player2.sword_pos_y = -1;
		}

		gs->player1.bounce_ctr = (*(signed char *)(PLAYER1_ADDRESS + DIRECTION_OFFSET));
		gs->player2.bounce_ctr = (*(signed char *)(PLAYER2_ADDRESS + DIRECTION_OFFSET));
		//-1 left +1 right

		gs->player1.bounce_ctr = __builtin_popcount(*(unsigned char *)(PLAYER1_ADDRESS + BOUNCE_CTR_OFFSET));
		gs->player2.bounce_ctr = __builtin_popcount(*(unsigned char *)(PLAYER2_ADDRESS + BOUNCE_CTR_OFFSET));
		//bits 1 2 4

		gs->player1.situation = (*(unsigned char *)(PLAYER1_ADDRESS + SITUATION_OFFSET));
		gs->player2.situation = (*(unsigned char *)(PLAYER2_ADDRESS + SITUATION_OFFSET));
		//bits 0 JUMPING/FALLING, 1 AU SOL ( CAN JUMP), 8 AGGRIPPÉ AU MUR

		gs->player1.keys_pressed = (*(unsigned char *)(PLAYER1_ADDRESS + KEYS_PRESSED_OFFSET));
		gs->player2.keys_pressed = (*(unsigned char *)(PLAYER2_ADDRESS + KEYS_PRESSED_OFFSET)); 
		//bits up : 0x20, down : 0x10, left : 0x8, right : 0x4, jump : 0x2, attack : 0x1

		switch((*(unsigned long*)(PLAYER1_ADDRESS + ACTION_OFFSET)))
		{
			case 0x42aec0:
				gs->player1.action = DEAD;
				break;

			case 0x43fbf0:
				gs->player1.action = DUCK; //SLIDE
				break;

			case 0x440e00:
				gs->player1.action = EGGNOGG; //WIN
				break;

			case 0x43e740:
				gs->player1.action = JUMP;
				break;

			case 0x43dd90:
				gs->player1.action = KICK;
				break;

			case 0x441f30:
				gs->player1.action = NOTHING;
				break;

			case 0x43a700:
				gs->player1.action = PUNCH;
				break;

			case 0x43c4b0:
				gs->player1.action = RUN;
				break;

			case 0x43d2a0:
				gs->player1.action = STAB;
				break;

			case 0x43b240:
				gs->player1.action = STANCE;
				break;

			case 0x4407e0:
				gs->player1.action = STUN;
				break;

			case 0x43f6d0:
				gs->player1.action = THROW;
				break;

			default:
				gs->player1.action = DEFAULT;
				break;
		}

		switch((*(unsigned long*)(PLAYER2_ADDRESS + ACTION_OFFSET)))
		{
			case 0x42aec0:
				gs->player2.action = DEAD;
				break;

			case 0x43fbf0:
				gs->player2.action = DUCK; //SLIDE
				break;

			case 0x440e00:
				gs->player2.action = EGGNOGG; //WIN
				break;

			case 0x43e740:
				gs->player2.action = JUMP;
				break;

			case 0x43dd90:
				gs->player2.action = KICK;
				break;

			case 0x441f30:
				gs->player2.action = NOTHING;
				break;

			case 0x43a700:
				gs->player2.action = PUNCH;
				break;

			case 0x43c4b0:
				gs->player2.action = RUN;
				break;

			case 0x43d2a0:
				gs->player2.action = STAB;
				break;

			case 0x43b240:
				gs->player2.action = STANCE;
				break;

			case 0x4407e0:
				gs->player2.action = STUN;
				break;

			case 0x43f6d0:
				gs->player2.action = THROW;
				break;

			default:
				gs->player2.action = DEFAULT;
				break;
		}

		if(*(unsigned long *)(LEADER_ADDRESS) == PLAYER1_ADDRESS)
		{
			gs->leader = 1;
		}
		else if(*(unsigned long *)(LEADER_ADDRESS) == PLAYER2_ADDRESS)
		{
			gs->leader = 2;
		}
		else
		{
			gs->leader = 0;
		}

		gs->room_number = (*(unsigned char *)(ROOM_NUMBER_ADDRESS));

		gs->total_room_number = (*(unsigned char *)(TOTAL_ROOM_NUMBER_ADDRESS)) * 2 - 1;

		gs->nb_swords = (*(unsigned char *)(NUM_THINGS_ALLOCATED_ADDRESS)) - 2; //2 things allocated are the 2 players
	}

	//RPC Functions
	void * set_speed_3_svc(u_long *argp, struct svc_req *rqstp)
	{
		static char * result;
		*logic_rate = *argp;
		return (void *) &result;
	}


	u_long * get_speed_3_svc(void *argp, struct svc_req *rqstp)
	{
		static u_long  result;
		result = *logic_rate;
		return &result;
	}

}
