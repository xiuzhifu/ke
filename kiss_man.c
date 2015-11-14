#include "kiss_event.h"
#include "kiss_queue.h"
#include <stddef.h>
#include <lua.h>
#include <lauxlib.h>
int kiss_start(lua_State *L, const char * path);
int main( int argc, const char* argv[] )
{
	lua_State *L = luaL_newstate();
	luaL_requiref(L, "kissevent.c", luaopen_kissevent_c, 1);
	kissevent_init();
	kiss_mq_init();
	kiss_start(L, argv[0]);
	int i = 0;
	for (;;)
	{
		kissevent_dispatch(L);
		//printf("%s%d\n", "kiss", i);
		i++;
	};
}
