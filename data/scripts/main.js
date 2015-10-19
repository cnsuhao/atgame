
function abs(x)
{
    if(x < 0)
        return -x;
        
    return x;
}

function sqrt(x)
{
    var new_value = 1;
    var last_value;
    do
    {
        last_value = new_value;
        new_value =  (last_value + x / last_value) * 0.5;
    } while (abs(new_value - last_value) > 0.000001)

    return new_value;
}

var vector2 = function(x, y)
{
    this.x = x;
    this.y = y;
}

vector2.prototype = 
{
    Length : function()
    {
        return sqrt(this.x * this.x + this.y * this.y);
    }
};

function show()
{
    //ClearScreen();
    var v = new vector2(10.0,10);
    print("vector2 length = " + v.Length());
}

function start()
{
    show();
    ScriptMng.AddScriptObject("scripts/obj001.js", "obj001");
    ScriptMng.AddScriptObject("scripts/obj002.js", "obj002");
}

function update()
{
    //show();
    //print("update!");
}

function end()
{
    
}