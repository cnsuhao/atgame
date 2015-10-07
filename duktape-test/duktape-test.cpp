// duktape-test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#pragma warning(disable:4996)
#include "duktape.h"

#include <cassert>

int adder(duk_context *ctx) {
	int i;
	int n = duk_get_top(ctx);  /* #args */
	double res = 0.0;

	for (i = 0; i < n; i++) {
		res += duk_to_number(ctx, i);
	}

	duk_push_number(ctx, res);
	return 1;  /* one return value */
}

int sub(duk_context *ctx) {
	int i;
	int n = duk_get_top(ctx);  /* #args */
	double res = 0.0;

	for (i = 0; i < n; i++) {
		res -= duk_to_number(ctx, i);
	}

	duk_push_number(ctx, res);
	return 1;  /* one return value */
}



void js_class_declare(duk_context *ctx, const char* classname, duk_c_function constructor)
{
    duk_get_global_string(ctx, "Lsh");
    duk_push_c_function(ctx, constructor, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, classname);
    duk_pop(ctx);
}

void js_class_push_propertyobject(duk_context *ctx, const char* classname)
{
    char name[64];
    sprintf(name, "__%s__Properties", classname);

    duk_get_global_string(ctx, "Lsh");
    duk_push_object(ctx);
    duk_dup(ctx, -1);
    duk_put_prop_string(ctx, -3, name);
    duk_remove(ctx, -2); // remove Lsh object
}

duk_ret_t jsb_constructor_Vec2(duk_context* ctx)
{
    return 0;
}

void Test(duk_context *ctx)
{
    duk_push_global_object(ctx);
    duk_push_string(ctx, "Lsh");
    duk_idx_t i = duk_push_object(ctx);
    duk_put_prop(ctx, 0);
    duk_pop(ctx);


    js_class_declare(ctx, "Vec2", jsb_constructor_Vec2);
    js_class_push_propertyobject(ctx, "Vec2");

    //duk_eval_string(ctx, "print(Lsh.Vec2)");
}

int _tmain(int argc, _TCHAR* argv[])
{
	int n = 0;
	duk_context *ctx = duk_create_heap_default();

    Test(ctx);

	duk_destroy_heap(ctx);

	return 0;
}

