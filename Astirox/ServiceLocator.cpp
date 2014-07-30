#include "stdafx.h"
#include "ServiceLocator.h"

IAudioProvider* ServiceLocator::_audioProvider = NULL;
tmx::MapLoader* ServiceLocator::_mapLoader = NULL;