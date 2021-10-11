#include "he.h"


#define HELLO_URL "/api/v2/hello"

/*
 * Define he_loglevel to 1,2 or 3 to see log messages from libhe (0 means no logging)
*/
int he_loglevel = 2;
he_t *server = NULL;

void hello_handler(he_t *he)
{
	// Retrieving user's data:
	// my_t *my = (my_t*) he->user_data;

	fprintif(HE_LOGLEVEL_1, "Handling HTTP GET request for %s", HELLO_URL);
	he_close_http_connection_with_message(he, "200 OK", "Some more details\r\n");
}

/**
 * We stop server from signal handler running in same thread, so only stop = 1 is needed,
 * if you want to stop it from different thread then he_stop() should be used.
 */
void sigint_handler(int signo)
{
	if (signo == SIGINT) {
		server->stop = 1;
	}
}

int main(void)
{
	server = he_create();
	if (!server) {
		return -1;
	}

	// For SSL:
	// he_set_ssl(server, ssl_key_name, ssl_cert_name);

	// Passing user's data to callbacks:
	// he_set_user_data(server, ptr);
	//
	// Set different port:
	he_set_port(server, 8088);

	he_register_get_handler(server, HELLO_URL, hello_handler);
	signal(SIGINT, sigint_handler);
	he_run(server);

	he_destroy(&server);
	return 0;
}
