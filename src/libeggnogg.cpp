#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <dlfcn.h>
#include <thread>
#include "../include/libeggnogg.hpp"
#include "../include/libeggnogg_rpc.hpp"
//#define DEBUG
#define LIBSDL "/usr/lib/x86_64-linux-gnu/libSDL2-2.0.so.0.10.0"
#define SDL_NumJoysticks_GOT 0x665158
#define LOGIC_RATE_ADDRESS 0x665418

namespace LibEggnogg
{
	__attribute__((constructor))void _init(void)
	{
		puts("[+] Library loaded !");
		std::thread background_task(init_libeggnogg_rpc_serv); 
		background_task.detach();
		atexit(exit_libeggnogg_rpc_serv);

		logic_rate = (unsigned long*)LOGIC_RATE_ADDRESS;
		SDL_NumJoysticks_real_GOT = (void**)SDL_NumJoysticks_GOT;

		void * libSDL_handle;
		if((libSDL_handle = dlopen(LIBSDL, RTLD_LAZY)) == NULL)
		{
			fprintf(stderr, "[-] Hooking failed : could not find libSDL !\n");
		}
		if((SDL_NumJoysticks_real = dlsym(libSDL_handle, "SDL_NumJoysticks")) == NULL)
		{
			fprintf(stderr, "[-] Hooking failed : could not find SDL_NumJoysticks !\n");
		}

		*SDL_NumJoysticks_real_GOT = (void*)&SDL_NumJoysticks_hook;
		#ifdef DEBUG
		printf("Real SDL_NumJoysticks address : %p\n", SDL_NumJoysticks_real);
		printf("Fake SDL_NumJoysticks address : %p\n", SDL_NumJoysticks_hook);
		printf("GOT SDL_NumJoysticks value : %p\n", *SDL_NumJoysticks_real_GOT);
		#endif
		puts("[+] Main loop hooking successful !");
	}

	int SDL_NumJoysticks_hook(void)
	{
		#ifdef DEBUG
		puts("[+] NumJoysticks called !");
		#endif
		
		return ((int (*)(void))SDL_NumJoysticks_real)();
	}

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
//g++ -shared -fPIC libeggnogg.cpp rpc/libeggnogg_rpc_svc.cpp -o libeggnogg.so -ldl
