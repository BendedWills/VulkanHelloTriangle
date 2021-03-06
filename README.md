# VulkanHelloTriangle
A hello triangle using Vulkan in C++.

I only intend for this project to work on linux, but you can have a go on getting it to run on Windows too. I might make a Windows version eventually.

The result of this program should look something like this. Feel free to copy and paste code:

![Vulkan hello triangle](https://github.com/BendedWills/VulkanHelloTriangle/blob/main/triangle.png?raw=true)

# Dependencies
* glslang
* vulkan (of course)
* glfw

# Linux build instructions
If you are on linux and want to build this project, you've come to the right place.

I recommend that you use the shell script that already exists to build the project like so: 

```./Run.sh```

or

```sh Run.sh```

But if you want to build the project without using the shell script, here is how:

First, run the GenerateResources.sh script to convert assets into object files:

	./GenerateResources.sh

Then, CMake to generate the Makefile like so: 

	cmake -S . -B Build

Then, run the make command to compile the executable:

	make -j8 -C Build all

The -C option tells make that the makefile is located in the "Build" directory.

The -j8 option tells make to use 8 threads to build the executable. Replace the 8 with however many cores you have on your computer.

Then run the executable:

	./Build/VulkanHelloTriangle

And there you go! A hello triangle in Vulkan.