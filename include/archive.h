#ifndef LIBGC_ARCHIVE_H
#define LIBGC_ARCHIVE_H

// -------------------------------------------------------------------------- //

#include "context.h"
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// -------------------------------------------------------------------------- //

typedef struct GCarchive GCarchive;
typedef struct GCarcdir GCarcdir;
typedef struct GCarcfile GCarcfile;

struct GCarchive {

	/* these are read-only */

	const GCcontext * ctx;

	GCarcdir * dirs; // list of directories in the archive
	GCsize dirnum; // number of directories in the archive

	GCarcfile * files; // list of files in the archive
	GCsize filenum; // number of files in the archive

	void* rawFileData; //raw copy of all the file data
	char* stringTable;
	// ...

};

struct GCarcdir {

	/* these are read-only */

	const GCcontext * ctx;
	const GCarchive * arc;

	GCarcdir * parent; // parent directory
	GCuint8 attr; // attribute bits (see GCarcattr)
	char * name; // name of the archive. pointer is managed by the GCarchive.
	GCsize fileoff; // index of first file in the directory
	GCsize filenum; // number of files in the directory

	// ...

};

struct GCarcfile {

	/* these are read-only */

	const GCcontext * ctx;
	const GCarchive * arc;
	const GCarcdir * dir;

	GCarcdir * parent; // parent directory
	GCuint8 attr; // attribute bits (see GCarcattr)
	char * name; // name of the archive. pointer is managed by the GCarchive.
	GCsize size; // size of the file
	void * data; // data of the file

	// ...

};

typedef enum {

	GC_ARCATTR_CMPRSZP = 0x01, // file is szp compressed
	GC_ARCATTR_CMPRSZS = 0x02, // file is szs compressed
	// add more attributes with the prefix 'GC_ARC_ATTR_'

} GCarcattr;


// -------------------------------------------------------------------------- //

// initializes an archive control block with the given context.
GCerror gcInitArchive(GCarchive * arc, const GCcontext * ctx);

// deinitializes an archive control block and frees
// all resources still associated with it.
GCerror gcFreeArchive(GCarchive * arc);

// loads the archive from the given memory block. the control block
// must be initialized before with gcInitArchive. the archive data
// is expected to be in standard big-endian RARC format.
GCerror gcLoadArchive(GCarchive * arc, const void * ptr, GCsize sz);

// if no pointer is provided, will generate the filesize of the resuling arc
// if a pointer is provided, it will generate an archive file into that memory block
GCsize gcSaveArchive(GCarchive * arc, const GCuint8* ptr);

// replaces the file data for a given file
GCerror gcReplaceFileData(GCarcfile * file, const GCuint8* ptr, GCsize sz);

// -------------------------------------------------------------------------- //

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // LIBGC_ARCHIVE_H
