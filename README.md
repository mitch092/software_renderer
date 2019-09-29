#What is this?

This is my program that implements https://github.com/ssloy/tinyrenderer.
I'm also writing a blogpost using Hugo and will store it in another github repo,
so that I can better explain what I'm doing differently from the tutorial I linked. 

This is a triangle rasterization program implemented completely in software.
I want to optimize the single core performance before moving onto multicore.
This entails:

- [] Minimizing cache misses
- [] Minimizing memory usage
- [] Fully utilizing SIMD/make the data completely vectorized

And when I get to multithreading:
- [] Data parallelism (multiple threads working on a single vector)
- [] Task parallelism (multiple threads working on different vectors)

I'm only at the end of lesson 3 of the tinyrenderer tutorial, but I want to 
structure the program in the most optimum way before moving on in the tutorial.

#Program TODO list

- [] Fix the bugs that are preventing the pixels from displaying,
in the RemoveJaggedArray branch.

- [] Merge RemoveJaggedArray into the main branch;
make sure to do it in a cloned repository and 
make sure all conflicts/silent issues are resolved.
If everything works out OK, then make the cloned repository the main one (delete the old one).

- [] Create a new branch and compare the SDL 
stopwatch functions to C++'s STL equivalents. Pick a winner between the two.

- [] Manually Fuse/Unfuse loops in Barycentric.h. Compare cache misses of both in separate branches.
Pick a winner to merge with the main branch. Use Intel Vtune Amplifier with hardware sampling to pick the winner
and find other cache misses. If there is no difference, then stick with the Fused version,
as the code is simpler.

- [] Use Intel Vtune Amplifier to find any more bottlenecks and cache misses anywhere else. 

- [] Use Intel Advisor to help me help the compiler autovectorize better.

- [] Make a new branch called "parallel_candidate_OpenMP" in which I will use OpenMP and Intel Advisor in
order to autovectorize and automultithread everything in data.h. GLM will be left alone.

- [] Make a new branch called "parallel_candidate_MKL" in which I will replace GLM and implement everything in data.h using Intel's MKL library.

- [] Make a new branch called "parallel_candidate_ISPC" in which I will replace GLM and implement everything in data.h using Intel's SPMD Compiler (ISPC).

- [] Compare the three branches. Make changes, maybe pick a winner. Perhaps I can merge some things from any of the three candidate branches.

- [] Implement offline rendering for wireframe and model rendering.

- [] Perhaps look into allocating every std::vector onto a fixed allocation buffer (memory pool).

- [] Implement UV wrapping as per the end of lesson 3 in the tutorial that I was originally following. Move on with the tutorial.


#Blog TODO list

- [] Follow the Hugo tutorial: https://www.youtube.com/watch?v=sB0HLHjgQ7E&list=PLLAZ4kZ9dFpOnyRlyS-liKL5ReHDcj4G3&index=4

- [] Work on the blog

- [] Add all of the URLs that I've used, to the blog. (Tinyrenderer, the opengl tutorial, Mike Acton's talk, Agner Fog's website, Eric Lengyel's book, anything else I'm forgetting.)

- [] Add this to the blog:
> Each triangle gets a bounding box. Each bounding box
> needs to store barycentric coordinates. A barycentric coordinate
> is 3 floating point numbers. If the triangle takes up the entire
> screen and the screen is 1920*1080 pixels, then the bounding
> box will be 1920*1080*3 floats * 4 bytes per float.
> If I don't resize the inner vectors in JaggedArrays,
> then my program could potentially take up (for 2000 triangles
> on a 1920*1080 size screen)
> 2000*1920*1080*3floats*4bytes per float, which is 59 gigabytes.
> To fix this, I recycle one buffer for every bounding box.
> Now the equation is 1*1920*1080*3*4 bytes, which is 
> 25 megabytes. 