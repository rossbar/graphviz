/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: Details at https://graphviz.org
 *************************************************************************/

/*
 * Glenn Fowler
 * AT&T Bell Laboratories
 *
 * return path to file a/b with access mode using : separated dirs
 * both a and b may be 0
 * if (mode&PATH_REGULAR) then path must not be a directory
 * if (mode&PATH_ABSOLUTE) then path must be rooted
 * path returned in path buffer
 */

#include <ast/ast.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

char *pathaccess(char *path, const char *dirs,
		 const char *a, const char *b, int mode)
{
    int m = 0;
    int sep = ':';
    char cwd[PATH_MAX];
    struct stat st;

#ifdef EFF_ONLY_OK
    m |= EFF_ONLY_OK;
#endif
#ifdef EX_OK
    if (mode == (PATH_EXECUTE | PATH_REGULAR)) {
	mode &= ~PATH_REGULAR;
	m |= EX_OK;
    } else
#endif
    {
	if (mode & PATH_READ)
	    m |= R_OK;
	if (mode & PATH_WRITE)
	    m |= W_OK;
	if (mode & PATH_EXECUTE)
	    m |= X_OK;
    }
    do {
	dirs = pathcat(path, dirs, sep, a, b);
	pathcanon(path);
	if (!access(path, m)) {
	    if ((mode & PATH_REGULAR)
		&& (stat(path, &st) || S_ISDIR(st.st_mode)))
		continue;
	    if (*path == '/' || !(mode & PATH_ABSOLUTE))
		return (path);
	    dirs = getcwd(cwd, sizeof(cwd));
	    sep = 0;
	}
    } while (dirs);
    return (0);
}
