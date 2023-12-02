# Overview of A Server Library

A Server Library (ASL) is a comprehensive and efficient server-side library in C, adept at handling network communications and HTTP requests. It integrates smoothly with `libuv` for high-performance asynchronous I/O operations and uses the HTTP Parser Library (HPL) for parsing incoming HTTP requests. With its powerful APIs, ASL is tailored for server applications, providing capabilities for URL routing, request handling, chunked transfers, and JSON processing. Coupled with A Memory Library, it ensures optimal memory management, making it ideal for building scalable and robust server-side applications.

## Dependencies

* [libuv](https://github.com/libuv/libuv) for handling asynchronous I/O operations.
* [HTTP Parser Library](https://github.com/contactandyc/http-parser-library) for HTTP request parsing.
* [A memory library](https://github.com/contactandyc/a-memory-library) for effective memory management.
* [A JSON library](https://github.com/contactandyc/a-json-library) for JSON processing (optional).

## Installation

### Clone the library and change to the directory

```bash
git clone https://github.com/contactandyc/a-server-library.git
cd a-server-library
```

### Build and install the library

```bash
mkdir -p build
cd build
cmake ..
make
make install
```

## An Example

```c
#include <stdio.h>
#include "a-server-library/aserver.h"
#include "http-parser-library/hpl.h"
#include "a-memory-library/aml_pool.h"

// Callback function for handling URL requests
int on_url(aserver_request_t *request) {
    // Retrieve and print the URL
    aml_pool_t *pool = aml_pool_init(1024);
    char *url = aserver_uri(request, pool);
    printf("Received request for URL: %s\n", url);

    // Prepare a response
    const char *response_body = "Hello, World!";
    aserver_http_200(request, "text/plain", (void *)response_body, strlen(response_body));

    // Clean up
    aml_pool_destroy(pool);
    return 0;
}

int main() {
    // Initialize the server on port 8080
    aserver_t *server = aserver_port_init(8080, on_url, NULL);

    // Run the server
    aserver_run(server);

    // Clean up
    aserver_destroy(server);
    return 0;
}
```

In this example:

- The server is initialized to listen on port 8080 using `aserver_port_init`.
- The `on_url` callback function is set to handle incoming URL requests, print the URL, and send a "Hello, World!" response.
- The server is started with `aserver_run` and later cleaned up using `aserver_destroy`.

## Core Functions

- **aserver_init**: Initializes a server with a specified file descriptor.
- **aserver_port_init**: Initializes a server to listen on a specified port.
- **aserver_unix_domain_init**: Initializes a Unix Domain Socket server.
- **aserver_hammer_init**: Initializes the server for load testing with repeated URL requests.
- **aserver_thread_data**: Sets up thread data handlers.
- **aserver_request_pool_size**: Configures the request pool size.
- **aserver_backlog**: Sets the server's connection backlog.
- **aserver_threads**: Configures the number of threads for the server.
- **aserver_run**: Starts the server loop.
- **aserver_destroy**: Destroys the server and frees resources.

## Response Functions

- **aserver_http_200**: Sends a standard HTTP 200 response.
- **aserver_start_chunk_encoding**: Starts chunked transfer encoding for responses.
- **aserver_chunk**: Sends a chunk of data in chunked transfer encoding.
- **aserver_finish_chunk_encoding**: Finishes chunked transfer encoding.

## Utility Functions

- **aserver_uri**: Retrieves the request URI.
- **aserver_parse_body_as_json**: Parses the request body as JSON.

This README provides a detailed guide to using A Server Library, including its dependencies, installation process, a basic usage example, and a comprehensive overview of its core functionalities and server response handling capabilities.