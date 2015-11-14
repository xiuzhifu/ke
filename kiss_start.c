#include <lua.h>
#include <lauxlib.h>
#include <stdio.h>
int 
kiss_start(lua_State *L, const char * path){
	const char * bootstrap = "package.path = package.path..';./?.lua' local path = ... local chunk = assert(loadfile('main.lua')) assert(pcall(chunk))";
	int i = lua_gettop(L);
	int err = luaL_loadstring(L, bootstrap);
	switch (err) {
		case LUA_OK: break;
		case LUA_ERRSYNTAX: fprintf(stderr,"syntax error:\n%s\n",lua_tostring(L,-1));
	}
	lua_pushstring(L, path);
	err = lua_pcall(L, 1, 0, 0);
	if (err) {
		fprintf(stderr,"error: %s\n",lua_tostring(L,-1));
		lua_close(L);
		return 1;
	}
	return 0;
}
