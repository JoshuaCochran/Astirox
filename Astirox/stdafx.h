// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO: reference additional headers your program requires here
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
//#include <SFML/Mouse.hpp>

#include <tmx/MapLoader.h>
#include <tmx/DebugShape.h>
#include <tmx/MapLayer.h>
#include <tmx/MapLoader.h>
#include <tmx/MapObject.h>
#include <tmx/QuadTreeNode.h>

#include "AnimatedSprite.hpp"
#include "Animation.hpp"

#include <map>
#include <iostream>
#include <cassert>
#include <list>
#include <string>
#include <sstream>
#include <stdio.h>

#include <Windows.h>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/discrete_distribution.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/optional/optional.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>