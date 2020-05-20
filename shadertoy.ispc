#include "glsl.ispc"
 
uniform float iTime;
uniform vec2 iResolution;
uniform vec4 iMouse;

//#include "program_flower.ispc"
//#include "program_flower2.ispc"
//#include "program_city.ispc"
#include "program_descent3d.ispc"


task void
shadertoy_scanline(uniform float dx, uniform float dy, uniform int width, uniform int height, uniform int span, uniform float output[])
{
    uniform int ystart = taskIndex * span;
    uniform int yend = min((taskIndex + 1) * span, (unsigned int)height);
    foreach (yi = ystart ... yend, xi = 0 ... width) {
        float x = -1.0f + xi * dx;
        float y = -1.0f + yi * dy;

        vec2 fragCoord = vec2(x * 0.5f + 0.5f, 1.0f - (y * 0.5f + 0.5f)) * iResolution;

        vec4 fragColor;
        mainImage(fragColor, fragCoord);

        int index = 3 * (yi * width + xi);
        output[index + 0] = fragColor.x;
        output[index + 1] = fragColor.y;
        output[index + 2] = fragColor.z;
    }
}

export void shadertoy_ispc(uniform int width, uniform int height, uniform float output[], uniform float time, uniform float mouse[4])
{
    uniform float dx = 2.0f / width;
    uniform float dy = 2.0f / height;

    iResolution = vec2(width, height);
    iTime = time;
    iMouse.x = mouse[0]; iMouse.y = mouse[1]; iMouse.z = mouse[2]; iMouse.w = mouse[3];

    uniform int span = 16;
    launch[height/span] shadertoy_scanline(dx, dy, width, height, span, output);
}