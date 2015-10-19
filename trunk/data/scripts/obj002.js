obj002 = {};

obj002.load = function ()
{
    print("load obj002.");
    ScriptMng.AddScriptObject("scripts/obj003.js", "obj003");
}

obj002.update = function (delteTime)
{
    var t = delteTime;
}

obj002.unload = function ()
{
    print("unload obj002.");
    ScriptMng.RemoveScriptObject("obj003");
}
