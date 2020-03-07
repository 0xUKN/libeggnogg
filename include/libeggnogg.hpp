#include "../lib/libeggnogg_shmem/include/libeggnogg_shmem.hpp"

namespace LibEggnogg
{
	static unsigned long* logic_rate;
	static GameState* gs;
	static void** SDL_NumJoysticks_real_GOT;
	static void* SDL_NumJoysticks_real;

	void _init(void);

	int SDL_NumJoysticks_hook(void);
	void UpdateGameState();
}
