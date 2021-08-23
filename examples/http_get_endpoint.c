#include "he.h"


#define HELLO_URL "/api/v2/hello"

/*
 * Define he_loglevel to 1,2 or 3 to see log messages from libhe (0 means no logging)
*/
int he_loglevel = 2;

void hello_handler(he_t *he)
{
	fprintif(HE_LOGLEVEL_1, "Handling HTTP GET request for %s", HELLO_URL);
	he_close_http_connection_with_message(he, "200 OK", "Some more details\r\n");
}

int main(void)
{
	he_t *server = he_create();
	if (!server) {
		return -1;
	}

	// For SSL:
	// he_set_ssl(server, ssl_key_name, ssl_cert_name);

	he_register_get_handler(server, HELLO_URL, hello_handler);
	he_run(server);

	return 0;
}
