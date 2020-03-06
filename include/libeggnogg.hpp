namespace LibEggnogg
{
	static unsigned long* logic_rate;
	static void** SDL_NumJoysticks_real_GOT;
	static void* SDL_NumJoysticks_real;
	void _init(void);
	int SDL_NumJoysticks_hook(void);
}
