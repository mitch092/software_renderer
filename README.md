# What is this?

This is my program that implements https://github.com/ssloy/tinyrenderer.
I'm also writing a blogpost using Hugo and will store it in another github repo,
so that I can better explain what I'm doing differently from the tutorial I linked. 

This is a triangle rasterization program implemented completely in software.
I want to optimize the single core performance before moving onto multicore.
This entails:

- [ ] Minimizing cache misses
- [ ] Minimizing memory usage
- [ ] Fully utilizing SIMD/make the data completely vectorized

And when I get to multithreading:
- [ ] Data parallelism (multiple threads working on a single vector)
- [ ] Task parallelism (multiple threads working on different vectors)

I'm only at the end of lesson 3 of the tinyrenderer tutorial, but I want to 
fully optimize everything before moving on in the tutorial.

# Program TODO list

- [x] Rewrite the program to use "data oriented design".

- [x] Replace git submodules with Cmake's fetchcontent_add.

- [x] Refactor/remove the horizontal pixel flipping function in Frame.h (was replaced with a scaling transform matrix in transform.h). 

- [x] Implement a proper framebuffer abstraction (remove SDL_Surface from Frame/Frames).

- [x] Abstract out the SDL window drawing from App.h, out into a new class, using [a faster pixel blitting method](https://stackoverflow.com/questions/33304351/sdl2-fast-pixel-manipulation), in Presenter.h.

- [x] Remove JaggedArrays, std::vector resize(), clear() and pushback() everywhere.

- [x] Re-integrate the old wireframe renderer.

- [ ] Turn Frame.h back into Frames.h. SDL_UpdateTexture knows when to stop reading bytes because of the size of the SDL_Texture, 
so I can make my Frame.h framebuffer class back into a 3D array (std::vector) of Colors, where the x and y coordinates index into
the color of a pixel of a frame, and the z coordinate is the number of the frame itself. I can give SDL_UpdateTexture a const void*
pointing to the beginning of any frame in my Frames object, and it will only read that one frame's worth of data (assuming that
every frame in Frames is the same size, and that the SDL_Texture object in Presenter.h is the exact same size as any of the individual frames in the Frames.h object).

- [ ] Fix the bugs that are preventing the pixels from displaying,
in the RemoveJaggedArray branch.

- [ ] Merge RemoveJaggedArray into the main branch;
make sure to do it in a cloned repository and 
make sure all conflicts/silent issues are resolved.
If everything works out OK, then make the cloned repository the main one (delete the old one).

- [ ] Create a new branch and compare the SDL 
stopwatch functions to C++'s STL equivalents. Pick a winner between the two.

- [ ] Manually Fuse/Unfuse loops in Barycentric.h. Compare the cache misses and performance of both in separate branches.
Pick a winner to merge with the main branch. Use Intel Vtune Amplifier with hardware sampling to pick the winner
and find other cache misses. If there is no difference, then stick with the Fused version,
as the code is simpler.

- [ ] Use Intel Vtune Amplifier to find any more bottlenecks and cache misses anywhere else. 

- [ ] Use Intel Advisor to help me help the compiler autovectorize better.

- [ ] Implement offline rendering for wireframe and model rendering.

- [ ] Make a new branch called "parallel_candidate_OpenMP" in which I will use OpenMP and Intel Advisor in
order to autovectorize and automultithread everything in data.h. GLM will be left alone.

- [ ] Make a new branch called "parallel_candidate_MKL" in which I will replace GLM and implement everything in data.h using Intel's MKL library.

- [ ] Make a new branch called "parallel_candidate_ISPC" in which I will replace GLM and implement everything in data.h using Intel's SPMD Compiler (ISPC).

- [ ] Compare the three branches. Make changes, maybe pick a winner. Perhaps I can integrate multiple things from any of the three candidate branches into main.

- [ ] Perhaps look into allocating every std::vector onto a fixed allocation buffer (memory pool), and then tune it to be just big enough for my program.

- [ ] Implement UV wrapping as per the end of lesson 3 in the tutorial that I was originally following. Move on with the tutorial.


# Blog TODO list

- [ ] Follow the Hugo tutorial: https://www.youtube.com/watch?v=sB0HLHjgQ7E&list=PLLAZ4kZ9dFpOnyRlyS-liKL5ReHDcj4G3&index=4

- [ ] Work on the blog

- [ ] Add all of the URLs that I've used, to the blog. (Tinyrenderer, the opengl tutorial, Mike Acton's talk, Agner Fog's website, Eric Lengyel's book, anything else I'm forgetting.)

- [ ] Add this to the blog:
> Each triangle gets a bounding box. Each bounding box
> needs to store barycentric coordinates. A barycentric coordinate
> is 3 floating point numbers. If the triangle takes up the entire
> screen and the screen is 1920 * 1080 pixels, then the bounding
> box will be 1920 * 1080 * 3 floats * 4 bytes per float.
> If I don't resize the inner vectors in JaggedArrays,
> then my program could potentially take up (for 2000 triangles
> on a 1920 * 1080 size screen):
> 2000 * 1920 * 1080 * 3 floats * 4 bytes per float, which is 59 gigabytes.
> To fix this, I recycle one buffer for every bounding box.
> Now the equation is 1 * 1920 * 1080 * 3 * 4 bytes, which is 
> 25 megabytes. 

- [ ] Also talk about how the remove JaggedArrays change affects cache misses negatively.