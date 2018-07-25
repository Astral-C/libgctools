## libgctools
libgctools is a c library (intended to be a static library) for various gamecube formats such as GCM, rarc, and yay0/yaz0

## what can it do so far
At the moment you can use it for loading and extracting rarc, ex
```c
//Assuming that you've already have a rarc stored as void* called buffer
rarcArchive* arc;
rarcLoadArchive(arc, buffer);
rarcDump(arc, 0);
//Then free whatever memory you have allocated. 
```

### cool but it sucks
no u