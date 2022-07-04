# Shader compilation

Both shaders have been compiled using `qsb -b --glsl "100 es,120,150" --hlsl 50 --msl 12 -o textured.vert.qsb textured.vert` and `.frag` accordingly.
The `-b` flag is required in order for the QSG to actually find and load the shaders, otherwise the program will crash or display an empty window.