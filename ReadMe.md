# Offscreen Renderer

A commandline tool for running OpenGL fragment shaders or compute shaders to draw something or process some data using GPU.

## Usage

When compiled, just run like this:

```
./offscreen_renderer [path/to/inifile]
```

It will load the ini file, read the `[common]` section to see how many tasks here, for example:

```
[common]
tasks=sometask1,sometask2
```

Then, it will see if `[sometask1]` and `[sometask2]` is there, and try to get some information about the task.

For example, `[sometask1]` will be like this:

```
[sometask1]
type=Draw
shader=xxxx.fsh
input=a1,a2
output=a3
```

See the `type` here should be `Draw` or `Compute`, which indicates the task type to be a fragment shader draw, or a compute shader task.

If `type` is `Compute`, you need also provide `groups=x,y,z` to indicate the work group size.

Then you need to define what is `a1`, `a2`, `a3`. To do this, you have to create sections for the input and output textures, for example:

```
[a1]
load=a1.png
format=RGBA8
```

Then `a1` will be loaded from `a1.png`. You can use the same way to create `a2`.

The output `a3` should also be defined like this:

```
[a3]
size=512,512
save=a3.png
format=RGBA8
```

Then your shader will run, process something, and draw some pixels to `a3.png`.

Your fragment shader can use `uniform sampler2D a1, a2;` to sample the textures from `a1` and `a2`. 

By default, your fragment shader has `uniform vec2 Resolution;` which is the size of output `a3`.

### See `presets` folder, there's some examples:

`pano`: a shader that turns 6 pictures into a pano.

`test`: a script that uses 2 shaders to do something to generate some picture. The 2 shaders contains a fragment shader and a compute shader.


