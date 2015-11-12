#include "kiss_event.h"
#include <stddef.h>
#include <lua.h>
#include <lauxlib.h>

int main( int argc, const char* argv[] )
{
	lua_State *L = luaL_newstate();
	kissevent_init();
	for (;;)
	{
		kissevent_dispatch(L);
	};
}