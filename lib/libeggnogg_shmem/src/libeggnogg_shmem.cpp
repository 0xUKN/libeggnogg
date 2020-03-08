#include <string>
#include <cstdio>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <cstring>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "../include/libeggnogg_shmem.hpp"
#define SHMEM "eggnogg_shmem"

namespace LibEggnogg
{
	void* CreateSharedMemory(std::string name, unsigned int size)
	{
		int shm_fd;
		void* ptr;
		shm_fd = shm_open(name.c_str(), O_CREAT | O_RDWR, 0666);
		if(shm_fd == -1)
		{
			perror("shm_fd");
			return NULL;
		}
		if(ftruncate(shm_fd, size) == -1)
		{
			perror("ftruncate");
			return NULL;	
		}
		ptr = mmap(0, size, PROT_WRITE, MAP_SHARED, shm_fd, 0);
		if(ptr == NULL)
		{
			perror("mmap");
			return NULL;	
		}
		return ptr;
	}

	int RemoveSharedMemory(std::string name)
	{
		return shm_unlink(name.c_str());
	}

	void CloseSharedMemory()
	{
		RemoveSharedMemory(SHMEM);
	}

	GameState* InitGameState()
	{
		return (GameState*)CreateSharedMemory(SHMEM, sizeof(LibEggnogg::GameState));
	}
}
