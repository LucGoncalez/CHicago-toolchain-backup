# CHicago Toolchain

## Building it

For building it from scratch, you need:

	A Unix-like environment
	GCC and G++
	Make
	Bison
	Flex
	GMP
	MPFR
	MPC
	Texinfo

To build it, go to the root directory and type:

	make

You can append ARCH=\<ARCH\> and SUBARCH=\<SUBARCH\> to change the system architecture, BUILD_CORES=\<CORES_AMOUNT\> to build it with multiple cores, VERBOSE=true to enable verbose build (good for finding compilation errors) and DEBUG=yes to disable optimizations and make a debug build.

After building it, you should have your toolchain inside of the \<ARCH\> folder.

## Building the CHIR tool

For building the CHIR tool, you only need GCC.
Go to the chir folder and type:

	make

Now you should have the executable of the tool inside of the build folder.
