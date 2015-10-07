#ifdef _ANDROID

#include <android_native_app_glue.h>
#include <cstdlib>
    
#include <android/log.h>
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "native-activity", __VA_ARGS__))

extern "C" { 
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}


/*the lua interpreter*/
lua_State* L;
int
luaadd(int x, int y)
{
        int sum;
/*the function name*/
        lua_getglobal(L,"add");
/*the first argument*/
        lua_pushnumber(L, x);
/*the second argument*/
        lua_pushnumber(L, y);
/*call the function with 2 arguments, return 1 result.*/
        lua_call(L, 2, 1);
/*get the result.*/
        sum = (int)lua_tonumber(L, -1);
/*cleanup the return*/
        lua_pop(L,1);
        return sum;
}

/**
 * Main entry point.
 */
void android_main(struct android_app* state)
{
    // Android specific : Dummy function that needs to be called to 
    // ensure that the native activity works properly behind the scenes.
    app_dummy();

    LOGW("-----------------lua test--------->");

    const char* buff = "function add(x,y) return x + y end";
    int error;
    L = luaL_newstate();
    luaL_openlibs(L);

    luaL_loadbuffer(L,buff,strlen(buff),"line");
    lua_pcall(L,0,0,0);
    int sum = luaadd(10, 15);
    LOGW("The sum is %d \n",sum);
    
    lua_close(L);
    // Android specific : the process needs to exit to trigger
    // cleanup of global and static resources (such as the game).
    exit(0);
}

#endif