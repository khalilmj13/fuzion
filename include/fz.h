/*

This file is part of the Fuzion language implementation.

The Fuzion language implementation is free software: you can redistribute it
and/or modify it under the terms of the GNU General Public License as published
by the Free Software Foundation, version 3 of the License.

The Fuzion language implementation is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
License for more details.

You should have received a copy of the GNU General Public License along with The
Fuzion language implementation.  If not, see <https://www.gnu.org/licenses/>.

*/

/*-----------------------------------------------------------------------
 *
 * Tokiwa Software GmbH, Germany
 *
 * Source of main include of Fuzion C backend.
 *
 *---------------------------------------------------------------------*/


#ifndef _FUZION_H
#define _FUZION_H 1

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

// NYI move to shared
static inline void * fzE_malloc_safe(size_t size) {
#ifdef GC_H
  void *p = GC_MALLOC(size);
#else
  void *p = malloc(size);
#endif
  if (p == NULL) {
    fprintf(stderr, "*** malloc(%zu) failed ***\n", size);
    exit(1);
  }
  return p;
}

// make directory, return zero on success
int fzE_mkdir(const char *pathname);

// set environment variable, return zero on success
int fzE_setenv(const char *name, const char *value, int overwrite);

// unset environment variable, return zero on success
int fzE_unsetenv(const char *name);

void fzE_opendir(const char *pathname, int64_t * result);

char * fzE_readdir(intptr_t * dir);

int fzE_read_dir_has_next(intptr_t * dir);

int fzE_closedir(intptr_t * dir);

// 0 = blocking
// 1 = none_blocking
int fzE_set_blocking(int sockfd, int blocking);

// helper function to retrieve
// the last error that occurred.
int fzE_net_error();

// fuzion family number -> system family number
int get_family(int family);

// fuzion socket type number -> system socket type number
int get_socket_type(int socktype);

// fuzion protocol number -> system protocol number
int get_protocol(int protocol);

// close a socket descriptor
int fzE_close(int sockfd);

// initialize a new socket for given
// family, socket_type, protocol
int fzE_socket(int family, int type, int protocol);

// create a new socket and bind to given host:port
// result[0] contains either an errorcode or a socket descriptor
// -1 error, 0 success
int fzE_bind(int family, int socktype, int protocol, char * host, char * port, int64_t * result);

// set the given socket to listening
// backlog = queuelength of pending connections
int fzE_listen(int sockfd, int backlog);

// accept a new connection
// blocks if socket is blocking
int fzE_accept(int sockfd);

// create connection for given parameters
// result[0] contains either an errorcode or a socket descriptor
// -1 error, 0 success
int fzE_connect(int family, int socktype, int protocol, char * host, char * port, int64_t * result);

// get the peer's ip address
// result is the length of the ip address written to buf
// might return useless information when called on udp socket
int fzE_get_peer_address(int sockfd, void * buf);

// get the peer's port
// result is the port number
// might return useless infomrmation when called on udp socket
unsigned short fzE_get_peer_port(int sockfd);

// read up to count bytes bytes from sockfd
// into buf. may block if socket is  set to blocking.
// return -1 on error or number of bytes read
int fzE_read(int sockfd, void * buf, size_t count);

// write buf to sockfd
// may block if socket is set to blocking.
// return error code or zero on success
int fzE_write(int sockfd, const void * buf, size_t count);

// returns -1 on error, size of file in bytes otherwise
long fzE_get_file_size(FILE* file);

/*
 * create a memory map of a file at an offset.
 * unix:    the offset must be a multiple of the page size, usually 4096 bytes.
 * windows: the offset must be a multiple of the memory allocation granularity, usually 65536 bytes
 *          see also, https://devblogs.microsoft.com/oldnewthing/20031008-00/?p=42223
 *
 * returns:
 *   - error   :  result[0]=-1 and NULL
 *   - success :  result[0]=0  and an address where the file was mapped to
 */
void * fzE_mmap(FILE * file, off_t offset, size_t size, int * result);

// unmap an address that was previously mapped by fzE_mmap
// -1 error, 0 success
int fzE_munmap(void * mapped_address, const int file_size);

/**
 * Perform bitwise comparison of two float values. This is used by
 * concur.atmic.compare_and_swap/set to compare floats. In particular, this
 * results is unequality of +0 and -0 and equality of NaN unless undefined bits
 * differ, etc.
 *
 * @param f1, f2 two float values
 *
 * @return true iff f1 and f2 are represented in memory by the same bit patternsx.
 */
bool fzE_bitwise_compare_float(float f1, float f2);

/**
 * Perform bitwise comparison of two double values. This is used by
 * concur.atmic.compare_and_swap/set to compare floats. In particular, this
 * results is unequality of +0 and -0 and equality of NaN unless undefined bits
 * differ, etc.
 *
 * @param d1, d2 two double values
 *
 * @return true iff d1 and d2 are represented in memory by the same bit patterns.
 */
bool fzE_bitwise_compare_double(double d1, double d2);

/**
 * returns a monotonically increasing timestamp.
 */
uint64_t fzE_nanotime();

/**
 * Sleep for `n` nano seconds.
 */
void fzE_nanosleep(uint64_t n);

/**
 * remove a file or path
 */
int fzE_rm(char * path);

/**
 * Get file status (resolves symbolic links)
 */
int fzE_stat(const char *pathname, int64_t * metadata);

/**
 * Get file status (does not resolve symbolic links)
 */
int fzE_lstat(const char *pathname, int64_t * metadata);

/**
 * Run plattform specific initialisation code
 */
void fzE_init();

/**
 * Start a new thread, returns a pointer to the thread.
 */
int64_t fzE_thread_create(void* code, void* args);

/**
 * Join with a running thread.
 */
// NYI add return value
void fzE_thread_join(int64_t thrd);

/**
 * Global lock
 */
void fzE_lock();
void fzE_unlock();

#endif /* fz.h  */
