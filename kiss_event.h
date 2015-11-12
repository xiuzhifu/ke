// struct kissevent_context;
// struct kissevent_context* kissevent_new();
// void kissevent_destroy(struct kissevent_context * ke);
// int kissevent_send(struct kissevent_context * ke, int source, char * date, uint32_t size);
#include <lua.h>
#ifndef KISS_EVENT_H
#define KISS_EVENT_H
void kissevent_dispatch(lua_State *L);
void kissevent_init();
#endif