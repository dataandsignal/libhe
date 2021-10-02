# libhe

This is a compact wrapper over [mongoose](https://github.com/cesanta/mongoose) that enables to create simple and robust HTTP(s) endpoints easily.
Basic GET and POST handler examples can be found in /examples folder.

## URL matching

URL of registered handler and request URL get truncated from first and last '/' (if any) before comparison. The rest of the string must match exactly.

"/" will match "//", "/", ""

"api" will match "/api/", "/api", "api/"

## GET handler example


```
#include "he.h"


#define HELLO_GET_URL "/api/v2/hello"

int he_loglevel = 2;

void hello_handler(he_t *he)
{
	fprintif(HE_LOGLEVEL_1, "Handling HTTP GET request for %s", HELLO_GET_URL);
	he_close_http_connection_with_message(he, "200 OK", "Some more details\r\n");
}

int main(void)
{
	he_t *server = he_create(NULL);
	if (!server) {
		return -1;
	}

	he_register_get_handler(server, HELLO_GET_URL, hello_handler);
	he_run(server);

	return 0;
}
```

Test GET handler:

```
$ curl http://localhost/api/v2/hello
Some more details
```

Output:

```
$ ./httpgetendpoint
src/he_http.c:78 "MG_EV_ACCEPT"
src/he_http.c:83 "MG_EV_RECV..."
src/he_http.c:90 "MG_EV_HTTP_REQUEST"
src/he_http.c:97 "Searching handler for: /api/v2/hello"
src/he_http.c:106 "-> Handler found"
http_endpoint.c:12 "Handling HTTP GET request for /api/v2/hello"
```

## POST handler example

```
#include "he.h"
#include <cjson/cJSON.h>


#define BOOM_URL "/api/v2/boom"

/*
 * Define he_loglevel to 1,2 or 3 to see log messages from libhe (0 means no logging)
*/
int he_loglevel = 2;

void boom_handler(he_t *he)
{
	cJSON *json = NULL;
	struct http_message *msg = he->http_msg;

	fprintif(HE_LOGLEVEL_1, "Handling HTTP POST request for %s", BOOM_URL);

	if (!msg) {
		he_close_http_connection_with_message(he, "500 Data missing", "No message?\r\n");
		return;
	}

	if (!msg->body.p || (msg->body.len == 0)) {
		he_close_http_connection_with_message(he, "500 Empty message", NULL);
		return;
	}

	json = cJSON_ParseWithLength(msg->body.p, msg->body.len);
	if (!json) {
		he_close_http_connection_with_message(he, "500 Data invalid", "Cannot parse into JSON\r\n");
		return;
	}

	{
		cJSON *username = NULL, *password = NULL;

		username = cJSON_GetObjectItem(json, "username");
		if (!(username && cJSON_IsString(username) && (username->valuestring != NULL))) {
			he_close_http_connection_with_message(he, "500 Username is missing", NULL);
			goto end;
		}

		fprintif(HE_LOGLEVEL_1, "username: %s", username->valuestring);

		password = cJSON_GetObjectItem(json, "password");
		if (!(password && cJSON_IsString(password) && (password->valuestring != NULL))) {
			he_close_http_connection_with_message(he, "500 Password is missing", NULL);
			goto end;
		}

		fprintif(HE_LOGLEVEL_1, "password: %s", password->valuestring);
	}

	he_close_http_connection_with_message(he, "200 OK", "Your data is valid\r\n");

end:
	if (json) {
		cJSON_Delete(json);
	}
}

int main(void)
{
	he_t *server = he_create(NULL);
	if (!server) {
		return -1;
	}

	he_register_post_handler(server, BOOM_URL, boom_handler);
	he_run(server);

	return 0;
}
```

Test POST handler:

```
$ curl --header "Content-Type: application/json" --request POST --data '{"username":"xyz","password":"xyz"}' http://localhost/api/v2/boom
Your data is valid
```

## Options

HTTPS

For HTTPs use he_set_ssl() with private key and certificate:

```
	he_set_ssl(server, ssl_key_name, ssl_cert_name);
```

Port

By defualt server will attempt to bind to port 80 for HTTP and 443 for HTTPS.
To bind to different port use he_set_port():

```
	he_set_port(server, port);
```

User's data

Pass your data to callbacks:

```
	he_set_user_data(server, ptr);
```

Retrieve your data in callbacks:

```
	my_t *my = (my_t*) he->user_data;
```

## Deps

- [libcd](https://github.com/dataandsignal/libcd)
- libssl-dev (e.g. sudo apt install libssl-dev)
- [cjson](https://github.com/DaveGamble/cJSON) (e.g. sudo apt install libcjson-dev)


## BUILD

This builds on Linux Debian. Install deps before building.

Makefile targets: 'make', 'make debug' and 'make release' produce shared library in build/debug or build/release folders.

'make examples'/'make examples-release'/'make examples-debug' produces examples in examples/build/release or examples/build/debug folder.

Debug targets require 'debug', default targets are relase, so for instance building and running debug version requires:

Build and install debug versions

```
	make debug
	make install-debug
```

Build and install in release mode

```
	make
	make install

or

	make release
	make install-release
```

Build examples in debug versions

```
	make examples-debug
```

Build examples in release mode

```
	make examples

or

	make examples-release 
```

```
make debug			-> for debug library build
make release			-> for release library build
make examples-debug		-> for build and run of debug version of examples
make examples-release		-> for build and run of release version of examples

make				-> same as make release
make examples			-> same as make examples-release

make clean			-> remove library binaries
make examples-clean		-> remove examples binaries
make clean-all			-> remove library and examples binaries

make install-debug		-> install debug version of library to /lib
make install-release		-> install release version of library to /lib
make install			-> same as make install-release

make uninstall			-> remove library from /lib
```
