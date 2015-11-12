#include <lua.h>
#include <lauxlib.h>

#include <unistd.h>

#define BACKLOG 32

#define MAX_SOCKET 2^16

static int
_send_request(){
	return 0;
}

static int
lstart(lua_State *L){
	return 0;
}

static int
lconnect(lua_State *L){
	return 0;
}

static int
lwrite(lua_State *L){
	return 0;
}

static int
lwriteall(lua_State *L){
	return 0;
}

static int
lread(lua_State *L){
	return 0;
}

static int
lreadline(lua_State *L){
	return 0;
}

static int
lreadall(lua_State *L){
	return 0;
}

static int
llisten(lua_State *L){
	return 0;
}

static int
laccept(lua_State *L){
	return 0;
}

static int
lclose(lua_State *L){
	return 0;
}

static int
ldrop(lua_State *L){
	return 0;
}

int
luaopen_kissocket_c(lua_State *L) {
	luaL_Reg l[] = {
	{ "start" , lstart },
	{ "listen", llisten },
	{ "accept", laccept },
	{ "read", lread },
	{ "readline", lreadline },
	{ "readall", lreadall },
	{ "write", lwrite },
	{ "writeall", lwriteall },
	{ "drop", ldrop },
	{ "close", lclose },
	{ "connect", lconnect },
	{ NULL, NULL },
	};
	luaL_newlibtable(L, l);
	return 0;
}

