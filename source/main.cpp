#include "atgBase.h"
#include "atgMath.h"
#include "atgGame.h"

int main(int argc,char ** argv)
{
    //Vec3 v1,v2,v3;
    //v1.x = 1.0f;
 //   v1.y = 1.0f;
 //   v1.z = 0.0f;

 //   v2.x = 0.0f;
 //   v2.y = 0.0f;
 //   v2.z = 1.0f;

 //   float r = VecDot(v1.m, v2.m);
 //   printf("VecDot=%f.\n", r);

 //   Quat q1;
 //   QuatFromAxisAngle(q1.m, v2.m, MATH_PI_4);
 //   Matrix mat;
 //   QuatToMat(mat.m, q1.m);
 //   VecRotate(v3.m, v1.m, mat.m);

 //   Vec3 v4;
 //   VecRotate(v4.m, v1.m, q1.m);

    Game game;
    game.Initialize();
    game.Run();
    game.Exit();

    return EXIT_SUCCESS;
}