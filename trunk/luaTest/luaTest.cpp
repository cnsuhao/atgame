// luaTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "string.h"

extern "C" { 
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

/*the lua interpreter*/
lua_State* L;

int	luaadd(int x, int y)
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

static int test_stack(lua_State* l){
    // 获取最后一个入栈参数
    double d1 = lua_tonumber(l, -1); 
    // 从栈里弹出一个参数
    lua_pop(l, 1); 

    double d2 = lua_tonumber(l, -1);
    lua_pop(l, 1);

    double d3 = lua_tonumber(l, -1);
    lua_pop(l, 1);

    lua_pushnumber(l, 3333.3);
    lua_pushnumber(l, 3333.4);
    lua_pushnumber(l, 3333.5);
    return 3; // 这里要返回3个值
}

int _tmain(int argc, _TCHAR* argv[])
{
    //L = luaL_newstate();   
    //luaL_openlibs(L);			// 载入Lua基本库  
    //luaL_dofile(L, "test.lua"); // 运行脚本   
    //lua_close(L);				// 清除Lua 


    const char* buff = "print(1111) function add(x,y) return x + y end";
    L = luaL_newstate();
    luaL_openlibs(L);
    luaL_loadbuffer(L,buff,strlen(buff),"line");
    lua_pcall(L,0,0,0);
    int sum = luaadd(910, 15);
    printf("The sum is %d \n",sum);
    lua_close(L);

    L = luaL_newstate();
    luaL_openlibs(L);
    lua_pushcfunction(L, test_stack);
    lua_setglobal(L, "test_stack");
    const char* buff3 = "function get() return 1,2,3 end return test_stack(get())";
    luaL_loadbuffer(L,buff3,strlen(buff3),"line");
    int error = lua_pcall(L,0/*0个输入参数*/,3/*3个返回值*/,0);
    if (error){
        printf("%s", lua_tostring(L, -1));
    }
    double d1 = lua_tonumber(L, -3);
    double d2 = lua_tonumber(L, -2);
    double d3 = lua_tonumber(L, -1);
    lua_pop(L, 1); 
    lua_close(L);

    L = luaL_newstate();
    luaL_openlibs(L);
    //const char* buff2 = "vector = { x=1, y=2, z=3 } function vector:show(str) print(self.x, str) end";
    const char* buff2 = "vector = { x=1, y=2, z=3, show = (function(self, str) print(self.x, str) end) }";
    luaL_loadbuffer(L,buff2,strlen(buff2),"line");
    lua_pcall(L,0,0,0);
    
    //lua_getglobal(L,"vector");
    //if (!lua_istable(L,-1)) {
    //    printf("'vector' is not a table.\n" );
    //    return -1;
    //}
    //int size= lua_objlen(L,-1);
    //for (int i=1;i<=size;i++)
    //{
    //    lua_pushnumber(L,i);
    //    lua_gettable(L,-2);
    //    printf("%f\n",lua_tonumber(L,-1));
    //    lua_pop(L,1);
    //}

    //lua_getglobal(L,"vector");
    //lua_pushnil(L);
    //while (lua_next(L,-2))
    //{   
    //    if (lua_isnumber(L,-1)){
    //        printf("%f\n",lua_tonumber(L,-1));
    //    }
    //    else if(lua_isfunction(L, -1)){
    //        printf("%s\n",lua_tostring(L,-1));
    //    }
    //    lua_pop(L,1);
    //}

    lua_getglobal(L,"vector");
    if (!lua_istable(L,-1)) {
        printf("'vector' is not a table.\n" );
        return -1;
    }
    int r_obj = luaL_ref(L, LUA_REGISTRYINDEX);
    lua_getfield(L, -1, "show");
    int r_func = luaL_ref(L, LUA_REGISTRYINDEX);

    //lua_rawgeti(L, LUA_REGISTRYINDEX, r_obj);
    //lua_rawgeti(L, LUA_REGISTRYINDEX, r_func);
    //lua_pushstring(L, "1314");
    //if(lua_pcall(L,1,0,0))
    //    printf("%s\n", lua_tostring(L, -1));

    //lua_getglobal(L,"vector");
    //lua_pushnil(L);
    //while (lua_next(L,-2))
    //{   
    //    if (lua_isnumber(L,-1))
    //    {
    //        printf("%f\n",lua_tonumber(L,-1));
    //    }
    //    else if(lua_isfunction(L, -1))
    //    {
    //        lua_getglobal(L,"vector");
    //        lua_pushstring(L, "1314");
    //        if(0 != lua_pcall(L,2,0,0))
    //        {
    //            printf("%s\n", lua_tostring(L, -1));
    //        }
    //        continue;
    //    }
    //    lua_pop(L,1);
    //}

    lua_close(L);
    return 0;
}
