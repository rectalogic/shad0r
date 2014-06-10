shad0r
=====
[frei0r](http://www.dyne.org/software/frei0r/) mixer2 plugin that mixes video sources using WebGL GLSL transition shaders compatible with [glsl.io]([http://glsl.io).

Building
--------
Install cmake, glfw3, frei0r, pkgconfig, pthreads. Initialize [ANGLE](https://code.google.com/p/angleproject/) submodule `git submodule update --init`.

    mkdir build
    cd build
    cmake ..

Running
-------
[MLT](http://www.mltframework.org/) supports frei0r mixer2 plugins.

    FREI0R_PATH=$PWD/build/src/mixer2 melt movie1.mov out=149 -track -blank 24 movie2.mov \
        -transition frei0r.shad0r in=25 out=149 a_track=0 b_track=1 real_time=0

Issues
------
This is a work in progress. Currently doesn't work on MacOS because we use a core OpenGL profile and Apple OpenGL requires `#version` in the shader, but ANGLE does not always generate it. Also frei0r mixer2 time is effectively wall-clock time, not normalized to 0..1. This isn't useful.