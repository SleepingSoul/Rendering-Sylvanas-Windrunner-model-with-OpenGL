![alt text](https://raw.githubusercontent.com/SleepingSoul/Rendering_Sylvanas_Windrunner_model_via_OpenGL/master/result.gif)
Hello!
--------------------------------------

In this project i will render Sylvanas Windrunner 3D model via C++ and OpenGL.
Also i will create a FPS camera simulation for convenient reviewing of it.
For the better and more beautifull result i will create few light casters.

For this task i will use assimp library for importing the obj model into CPU;
glad, glfw3 for OpenGL.

Important!
--------
If you want to run it in your computer, you should install assimp (http://www.assimp.org/), glwf3 (http://www.glfw.org/)
and glad (https://www.khronos.org/opengl/wiki/OpenGL_Loading_Library, http://glad.dav1d.de/). I do not neither own nor provide
this headers.
Also, i use stb_image.h header from stb library. It is provided in external folder for convenience, but i do not own this header.
Link is provided here: https://github.com/nothings/stb.
glad.c header is generated automatically and i cannot guarantee it will work in your computer. I would recommend to replace it on new
file, generated in http://glad.dav1d.de/.
I cannot guarantee that glfw3.dll and libassimp.dll will work on your computer. You should download precompiled versions of this
libraries from it's official web sources or download source code and compile it on your computer. This way you will get both of this
.dll libraries.
Also, i use 3D model in this program provided by Sanguinax here: https://sketchfab.com/models/6ef3b827f5e742e8bc32ba48ea600ee0#.
I do not own it. This model is protected by CC Attribution.
