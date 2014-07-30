#ifndef _SERVICELOCATOR_H
#define _SERVICELOCATOR_H

#include "IAudioProvider.h"


class ServiceLocator
{
public:

	static void RegisterServiceLocator(IAudioProvider* provider)
	{
		_audioProvider = provider;
	}
	static void RegisterServiceLocator(tmx::MapLoader* provider)
	{
		_mapLoader = provider;
	}


	static IAudioProvider* GetAudio() { return _audioProvider; } const
	static tmx::MapLoader* GetMapLoader() { return _mapLoader; }


private:
	static IAudioProvider * _audioProvider;
	static tmx::MapLoader* _mapLoader;
};

#endif