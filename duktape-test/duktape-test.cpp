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
    if (n > 0)
    {
        res = duk_to_number(ctx, 0);
        for (i = 1; i < n; i++) {
            res -= duk_to_number(ctx, i);
        }
    }
    
	duk_push_number(ctx, res);
	return 1;  /* one return value */
}

int abs(duk_context* ctx)
{
    int n = duk_get_top(ctx);  /* #args */

    duk_push_object(ctx);

    double res = 0.0;
    if (n > 0)
    {
        for (int i = 0; i < n; i++) {
            res = duk_to_number(ctx, i);
            res = abs(res);
            duk_push_number(ctx, res);
            duk_put_prop_index(ctx, -2, i);
        }
    }

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
    duk_remove(ctx, -2); //#args remove Lsh object
}

duk_ret_t jsb_constructor_Vec2(duk_context* ctx)
{
    return 0;
}

void Test(duk_context *ctx)
{
    duk_push_global_object(ctx);
    duk_push_object(ctx);
    duk_idx_t i = duk_push_object(ctx);
    duk_put_prop(ctx, 0);
    duk_pop(ctx);


    js_class_declare(ctx, "Vec2", jsb_constructor_Vec2);
    js_class_push_propertyobject(ctx, "Vec2");

    //duk_eval_string(ctx, "print(Lsh.Vec2)");
}

void Test22(duk_context *ctx)
{
    duk_push_global_object(ctx);
    duk_push_object(ctx);
    duk_put_prop_string(ctx, -2, "Lsh");
    duk_pop(ctx);

    duk_get_global_string(ctx, "Lsh");

    duk_push_string(ctx, "Vesrion");
    duk_push_number(ctx, 1.01);
    duk_put_prop(ctx, -3);

    duk_push_string(ctx, "LastUpdateTime");
    duk_push_string(ctx, "2015/10/7 23:00");
    duk_put_prop(ctx, -3);

    duk_push_c_function(ctx, adder, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "adder");

    duk_push_c_function(ctx, abs, DUK_VARARGS);
    duk_put_prop_string(ctx, -2, "abs");
    duk_pop(ctx);
    
    duk_eval_string(ctx, "print(Lsh.Vesrion)");
    duk_pop(ctx);

    duk_eval_string(ctx, "print(Lsh.LastUpdateTime)");
    duk_pop(ctx);

    duk_eval_string(ctx, "print('2+3+4=' + Lsh.adder(2,3,4));");
    duk_pop(ctx);

    duk_eval_string(ctx, "Lsh.show = (function() { print('call Lsh.show'); });");
    duk_pop(ctx);

    duk_eval_string(ctx, "Lsh.show();");
    duk_pop(ctx);

    duk_eval_string(ctx, "var ary = Lsh.abs(-2,3,-4,-5); for(var a in ary) { print(ary[a]); }");
    duk_pop(ctx);

    printf("stack top is %ld\n", (long) duk_get_top(ctx));
}

int _tmain(int argc, _TCHAR* argv[])
{
	int n = 0;
	duk_context *ctx = duk_create_heap_default();
    printf("stack top is %ld\n", (long) duk_get_top(ctx));
    //Test(ctx);
    Test22(ctx);

	duk_destroy_heap(ctx);

	return 0;
}

