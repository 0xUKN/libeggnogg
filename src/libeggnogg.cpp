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
#define PLAYER1_ADDRESS 0x75b3e0
#define PLAYER2_ADDRESS 0x75b540

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
		gs->player1.life = *(char *)(PLAYER1_ADDRESS + LIFE_OFFSET);
		gs->player2.life = *(char *)(PLAYER2_ADDRESS + LIFE_OFFSET);
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
