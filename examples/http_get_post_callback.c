#include <he.h>


#define GET_URL "/get"
#define POST_URL "/post"
#define CDR_URL "/cdr"

/*
 * Define he_loglevel to 1,2 or 3 to see log messages from libhe (0 means no logging)
*/
int he_loglevel = 2;
he_t *server = NULL;

void handler(he_t *he, int get)
{
	struct http_message *msg = he->http_msg;
	char *m = NULL;

	// Retrieving user's data:
	// my_t *my = (my_t*) he->user_data;


	fprintif(HE_LOGLEVEL_1, "Handling HTTP %s request\n", get ? "GET" : "POST");

	if (!msg) {
		he_close_http_connection_with_message(he, "500 Data missing", "No message?\r\n");
		return;
	}

	if (!msg->body.p || (msg->body.len == 0)) {
		he_close_http_connection_with_message(he, "500 Empty message", NULL);
		return;
	}

	m = malloc(msg->body.len+1);
	if (!m) {
		he_close_http_connection_with_message(he, "500 Internal server error", NULL);
		return;
	}

	memset(m, 0, msg->body.len);
	strncpy(m, msg->body.p, msg->body.len);
	m[msg->body.len] = 0;
	fprintif(HE_LOGLEVEL_1, "Message is: %s\n", m);

	he_close_http_connection_with_message(he, "200 OK", "This callback is great\r\n");

	if (m) {
		free(m);
		m = NULL;
	}
}

void get_handler(he_t *he) {
	return handler(he, 1);
}

void post_handler(he_t *he) {
	return handler(he, 0);
}

void cdr_handler(he_t *he) {
	return handler(he, 0);
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
	he_set_port(server, 9999);

	he_register_get_handler(server, GET_URL, get_handler);
	he_register_post_handler(server, CDR_URL, cdr_handler);
	he_register_post_handler(server, POST_URL, post_handler);
	signal(SIGINT, sigint_handler);
	he_run(server);

	he_destroy(&server);
	return 0;
}
