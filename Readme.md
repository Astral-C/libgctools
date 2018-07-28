## libgctools
libgctools is a c library (intended to be a static library) for various gamecube formats such as GCM, rarc, and yay0/yaz0

## The ReWrite Branch
The point of the rewrite branch is to make it so that I can work on rewriting libgctools to be a more clean library than it is now while leaving the old version as is for people to use while this one is in the works.

## TODO
- Finalize current GCcontext
- Readd rarc loading based on new rarc structures
- Add yay/yaz decompression
- Test rarc loading, decoding if compressed, and dumping
- Plan out gcm