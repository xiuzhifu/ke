#include <lua.h>
#include <lauxlib.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include "kiss_queue.h"
#include "atomic.h"
#include <stdbool.h>
#include <assert.h>

#define DEFAULT_KISSEVENT_SIZE 64
#define SLOT_INVALID 0
#define SLOT_RESERVE 1

struct kissevent_context {
	int type;
	void * instance;
	void * cb_ud;
	struct message_queue *queue;
	FILE * logfile;
	char result[32];
	uint32_t handle;
	int session_id;
	int ref;
	bool init;
	bool endless; 
};

struct kiss_event {
	int cap;
	int size;
	struct kissevent_context *slot;
};

struct kiss_event KE;

static struct kissevent_context*
kissevent_get(struct kiss_event *ke, uint32_t id){
	assert(ke);
	return &ke->slot[id];
}

void
kissevent_dispatch(lua_State *L){
	struct message_queue * Q = kiss_globalmq_pop();
	if(Q){
		struct kiss_message *message;
		kiss_mq_pop(Q, message);
		struct kissevent_context *kc = kissevent_get(&KE, Q->handle);
		int func = lua_rawgeti(L, LUA_REGISTRYINDEX, kc->ref);
		if (func == LUA_TFUNCTION){
			lua_pushinteger(L, message->source);
			lua_pushinteger(L, message->session);
			lua_pushlstring(L, message->data, message->size);
			lua_pushinteger(L, message->size);
			int err = lua_pcall(L, 4, 0, 0);
		}
	}
}

static int
reserve_id(struct kiss_event * ke) {
	int i;
	for (i=0; i<ke->size; i++) {
		struct kissevent_context *kc = &ke->slot[i % ke->cap];
		if (kc->type == SLOT_INVALID) {
			kc->type = SLOT_RESERVE;
			kc->handle = i;
			return i;
		}
	}
	return -1;
}

static struct kissevent_context*
kissevent_new(struct kiss_event *ke){
	assert(ke);
	if (ke->size >= ke->cap){
		ke->cap += DEFAULT_KISSEVENT_SIZE;
		struct kissevent_context *slot = kiss_malloc(sizeof(*ke->slot) * ke->cap);
		memset(slot, sizeof(*ke->slot) * ke->cap, SLOT_INVALID);
		memcpy(slot, ke->slot, sizeof(*ke->slot) * ke->size);
		kiss_free(ke->slot);
		ke->slot = slot;
	}
	int handle = reserve_id(ke);
	return &ke->slot[handle];
}

void
kissevent_init(){
	KE.size = 0;
	KE.cap = 0;
}

int
lnew(lua_State *L){
	struct kissevent_context * kc = kissevent_new(&KE);
	kc->queue = kiss_mq_create(kc->handle);
	lua_pushinteger(L, kc->handle);
	return 1;
}

int
ldestroy(lua_State *L){
	uint32_t handle = luaL_checkinteger(L, 1);
	struct kissevent_context* ke = kissevent_get(&KE, handle);
	kiss_mq_release(ke->queue);
	ke->type = SLOT_INVALID;
	luaL_unref(L, LUA_REGISTRYINDEX, ke->handle);
	return 0;
}

int
lsend(lua_State *L){
	struct kissevent_context* kc = lua_touserdata(L, 1);
	struct kiss_message* msg = kiss_malloc(sizeof(*msg));
	msg->source = kc->handle;
	kc->session_id ++;
	msg->session = kc->session_id;
	msg->data = (void*)lua_tolstring(L, 2, &msg->size);
	kiss_mq_push(kc->queue, msg);
	lua_pushinteger(L, kc->session_id);
	return 1;
}

int 
lcallback(lua_State *L){
	int handle = luaL_checkinteger(L, 1);
	struct kissevent_context* ke = kissevent_get(&KE, handle);
	luaL_checktype(L,2,LUA_TFUNCTION);
	lua_settop(L,1);
	ke->ref = luaL_ref(L, LUA_REGISTRYINDEX);
	lua_rawseti(L, LUA_REGISTRYINDEX, ke->ref);
	return 0;
}


int
luaopen_kissevent_c(lua_State *L){
	luaL_Reg l[] = {
	{ "new" , lnew },
	{ "destroy", ldestroy },
	{ "send", lsend},
	{ "callback", lcallback },
	{ NULL, NULL },
	};
	luaL_newlibtable(L, l);
	return 0;
}