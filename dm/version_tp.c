/* ex: set expandtab ts=3:                                                 */
/* -*- Mode: C; tab-width:3 -*-                                            */
#include <stdio.h>

static const char rcsid[] = "$Id: version_tp.c,v 1.4 2003/09/24 06:11:02 thaust Exp $";

/*
 * This is the template file for the version string. The makefile
 * uses this file to create a file called "version.c"; which contains
 * identifiers in it to allow one to ascertain where and how a given
 * copy of DM was built.
 *
 * This is essential when one has many developers, and one core dump.
 *
 * N.B.: The print_version() function has a subtle purpose here. Without it,
 * some common optimizations of gcc will simply delete all the variables
 * here, as they don't have an external reference.
 */

/*
 * start of the declarations for the strings which get generated
 * by the makefile:
 */

/* Each of the dm_ strings get stamped via the makefile.
 * Furthermore, each should start with "Version", so that one can
 * run: strings coredump | grep Version.
 */
extern const char dm_version[];        /* The basic version information    */
extern const char dm_uid[];            /* Who built it                     */
extern const char dm_pwd[];            /* What directory it was built in   */
extern const char dm_date[];           /* When it was built                */

void print_version()
{
   fprintf
   (
      stderr,
      "Darkmists version %s\n"
      "Built on %s, by %s, in %s\n",
      dm_version,
      dm_date,
      dm_uid,
      dm_pwd
   );
}

/* After this, all information is generated by the makefile:           */
