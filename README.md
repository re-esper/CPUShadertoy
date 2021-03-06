# CPU Shadertoy
Run shadertoy shaders on CPU with multithreading and SIMD. Just for fun and learning about [ISPC](https://ispc.github.io/), the Intel SPMD program compiler.

With this project, you can just **copy-and-paste** shadertoy shaders, run it on CPU with **no** or **little** modification, and get high performance.

This project is highly inspired from zigguratvertigo's [hlsl-to-ispc](https://github.com/zigguratvertigo/hlsl-to-ispc).

This project is still incomplete and working in progress.

## Showcases

[Descent 3D](https://github.com/re-esper/CPUShadertoy/blob/master/program_descent3d.ispc) (Shadertoy's "Shader of the Week" for now,  [original](https://www.shadertoy.com/view/wdfGW4))  
<img src="https://github.com/re-esper/CPUShadertoy/blob/master/screenshot/descent.png" width="50%" height="50%">

[Warping - texture](https://github.com/re-esper/CPUShadertoy/blob/master/program_warptexture.ispc) ([original](https://www.shadertoy.com/view/Xsl3zn))  
<img src="https://github.com/re-esper/CPUShadertoy/blob/master/screenshot/texture.png" width="50%" height="50%">

[Deform - flower](https://github.com/re-esper/CPUShadertoy/blob/master/program_flower.ispc) ([original](https://www.shadertoy.com/view/4dX3Rn))  
<img src="https://github.com/re-esper/CPUShadertoy/blob/master/screenshot/flower.png" width="50%" height="50%">

[Flower at night](https://github.com/re-esper/CPUShadertoy/blob/master/program_flower2.ispc) ([original](https://www.shadertoy.com/view/4lSXzR))  
<img src="https://github.com/re-esper/CPUShadertoy/blob/master/screenshot/flower2.png" width="50%" height="50%">

[Glow City](https://github.com/re-esper/CPUShadertoy/blob/master/program_city.ispc) ([original](https://www.shadertoy.com/view/XlsyWB))  
<img src="https://github.com/re-esper/CPUShadertoy/blob/master/screenshot/city.png" width="50%" height="50%">

## Performance

I got around **18FPS**, running "Glow City" on my Intel Core i7-7700K. As a comparison, around **31FPS** when using GPU(HD 630) on shadertoy.com(Chrome).
