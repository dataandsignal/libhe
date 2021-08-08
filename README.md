# libhe

This is a compact wrapper over [mongoose](https://github.com/cesanta/mongoose) that enables to create robust HTTP(s) endpoints easily.
Basic GET and POST handler examples can be found in /examples folder.

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
$ ./httpendpoint
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

## Deps

- [libcd](https://github.com/dataandsignal/libcd)
- [cjson](https://github.com/DaveGamble/cJSON) (POST handler example only)


## BUILD

This builds on Linux Debian. make, make debug and make release produce shared library in build/debug or build/release folders.

```
make debug			-> for debug library build
make release			-> for release library build
make test-debug			-> for build and run of debug version of tests
make test-release		-> for build and run of release version of tests
make examples-debug		-> for build and run of debug version of examples
make examples-release		-> for build and run of release version of examples

make				-> same as make release
make test			-> same as make test-release
make examples			-> same as make examples-release

make clean			-> remove library binaries
make test-clean			-> remove test binaries
make examples-clean		-> remove examples binaries
make clean-all			-> remove library and test and examples binaries

make install-debug		-> install debug version of library to /lib
make install-release		-> install release version of library to /lib
make install			-> same as make install-release

make uninstall			-> remove library from /lib
```
