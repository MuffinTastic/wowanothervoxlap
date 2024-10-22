# wowanothervoxlap

This repo contains all of the code I wrote while working with Ken Silverman's VOXLAP engine in 2017.

Provided as-is; do not expect tech support or easy compilation.

## angelscript_test

For voxscript and voxscript1024, a small test fusing AngelScript and VOXLAP. No networking capabilities, and in general pretty limited.

[Video](https://www.youtube.com/watch?v=4AwtolJfShU)

## aosclone

My first C++ project ever: A VOXLAP-based client for Ace of Spades. It has some issues which definitely show, but it was pretty cool.

[Video](https://www.youtube.com/watch?v=Blfyd0LSqZU)

## Other subdirectories

### ericson

A download of [Ericon2314's VOXLAP fork](https://github.com/Ericson2314/Voxlap), actually unused.

### voxlap

My modified version of VOXLAP used in all the executables. Uses the original assembly and original WINMAIN.

Not much is different as I recall it, but there are some things:
- Increased raycast resolution
- Increased supported display resolution (2560^2)
- AoS map size (by default, you can just change it back to 1024^2 x 256 in voxlap5.h if you want to compile for voxscript1024)
- drawsquarefill
- A failed attempt at changing floating block detection behavior to match AoS (I only eliminated diagonals on the XY plane)

### voxdata

Test assets from Ken Silverman's VOXLAP downloads.

## License

MIT unless otherwise stated. For more detail refer to the LICENSE file.
