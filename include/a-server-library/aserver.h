/*
Copyright 2019 Andy Curtis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef aserver_H
#define aserver_H

#include <stdbool.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <uv.h>

#include "http-parser-library/hpl.h"

#include "a-memory-library/aml_pool.h"
#include "a-json-library/ajson.h"

#ifdef __cplusplus
extern "C" {
#endif

struct aserver_request_s;
typedef struct aserver_request_s aserver_request_t;

struct aserver_s;
typedef struct aserver_s aserver_t;

typedef void *(*aserver_create_thread_data_cb)(void *gbl);
typedef void (*aserver_destroy_thread_data_cb)(void *gbl, void *tdata);

typedef int (*aserver_cb)(aserver_request_t *r);

aserver_t *aserver_init(int fd, aserver_cb on_url, aserver_cb on_chunk);
void aserver_new_cors(aserver_t *s);
aserver_t *aserver_port_init(int port, aserver_cb on_url, aserver_cb on_chunk);
aserver_t *aserver_unix_domain_init(const char *path, aserver_cb on_url,
                                  aserver_cb on_chunk);

/* specify a list of URIs (no host or port) and the number of times to repeat */
aserver_t *aserver_hammer_init(aserver_cb on_url, aserver_cb on_chunk, char **urls, size_t num_urls, int repeat);

void aserver_thread_data(aserver_t *service,
                        aserver_create_thread_data_cb create,
                        aserver_destroy_thread_data_cb destroy);

void aserver_request_pool_size(aserver_t *w, size_t size);
void aserver_backlog(aserver_t *w, int backlog);
void aserver_threads(aserver_t *w, int num_threads);

void aserver_run(aserver_t *w);

void aserver_destroy(aserver_t *w);

char *aserver_uri(aserver_request_t *r, aml_pool_t *pool);
ajson_t *aserver_parse_body_as_json(aserver_request_t *r, aml_pool_t *pool);

void aserver_http_200(aserver_request_t *r,
                       const char *content_type,
                       void *body, uint64_t body_length);

void aserver_start_chunk_encoding(aserver_request_t *r,
                                   aml_pool_t *pool,
                                   const char *content_type,
                                   aserver_cb cb);

void aserver_chunk(aserver_request_t *r,
                    void *body, uint32_t body_length,
                    aserver_cb cb);

void aserver_chunk2(aserver_request_t *r,
                     void *body, uint32_t body_length,
                     void *body2, uint32_t body2_length,
                     aserver_cb cb);

/* if cb is NULL, default on_request_complete will be called */
void aserver_finish_chunk_encoding(aserver_request_t *r, aserver_cb cb);


struct aserver_request_s {
  hpl_t *http;

  aml_pool_t *pool;
  void *thread_data;
  void *data;

  aserver_cb on_request_complete;

  /* output parameters
    - status_string = NULL defaults to 200 OK
    - content_type = NULL defaults to text/plain
    - output = NULL, len output content
  */
  enum { OK = 0,
         SIZE_EXCEEDED = 1,
         BAD_REQUEST = 2,
         INTERNAL_ERROR = 3 } state;

  int fd; // only to be used for reference

  aserver_t *service;
  aserver_request_t *next;
};

struct aserver_s {
  uv_loop_t loop;
  uv_poll_t server;
  uv_timer_t timer;

  aserver_cb on_url;
  aserver_cb on_chunk;
  aserver_cb on_request_complete;

  aserver_create_thread_data_cb create_thread_data;
  aserver_destroy_thread_data_cb destroy_thread_data;

  int fd;
  int thread_id;
  int chunk_id;
  int num_threads;
  int backlog;
  size_t request_pool_size;
  pthread_t thread;
  aserver_t *services;
  aserver_t *parent;
  bool shutting_down;

  bool old_style_cors;

  /* Date: ... GMT\r\nThread-Id: 000001\r\n - 56 bytes */
  char date[64];

  size_t active;
  size_t num_free;
  aserver_request_t *free_list;

  bool socket_based;
  union {
    int port;
    char *path;
  } base;

  // only used when hammer is true
  bool hammer;
  char **hammer_urls;
  char **hammer_urls_curp;
  char **hammer_urls_ep;
  int num_times_to_repeat_hammering_url;
  double time_spent_hammering;
};

#ifdef __cplusplus
}
#endif

#endif
