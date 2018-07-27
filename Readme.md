## libgctools
libgctools is a c library (intended to be a static library) for various gamecube formats such as GCM, rarc, and yay0/yaz0

## what can it do so far
At the moment you can use it for loading and extracting rarc, ex
```c
//Assuming that you've already have a rarc stored as void* called buffer
rarcArchive arc;
rarcLoadArchive(&arc, buffer);
rarcDump(&arc, 0);
//Then free whatever memory you have allocated. 
```
You can then get a pointer (as a char*) to a specific file in said arc, ex
```c
//Again assuming you already have the file buffered
rarcArchive arc;
rarcLoadArchive(&arc, buffer);
//The third parameter is the starting dir index, 0 being the root index
char* modelFile = rarcGetFile(&arc, "mario.bmd", 0);
//then you can do whatever operations on the file as if you had read it directly
```
You can even read a rarc thats in a rarc, ex
```c
rarcArchive arc1;
rarcArchive arc2;
rarcLoadArchive(&arc1, buffer);
rarcLoadArchive((void*)rarcGetFile(&arc1, "another_rarc.rarc", 0);
```


## TODO
- finish gcm stuff, its close
- clean up rarc stuff and add utility functions
- bug gamma to help me with compression/do compression stuff
- make gud

### cool but it sucks
no u
