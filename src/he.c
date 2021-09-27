#include "he.h"


int he_loglevel = HE_LOGLEVEL_0;

he_options_t he_default_options = {
	.port = 0,
	.use_ssl = 0,
	.ssl_key_name = { 0 },
	.ssl_cert_name = { 0 }
};

uint8_t he_zstr(const char *s)
{
	if (!s || (strlen(s) == 0)) {
		return 1;
	}
	return 0;
}

/*
 * Trim leading white space and truncate string on first whitespace after non whitespace part.
 */
char* he_string_trim_whitespace(char *str)
{
  char *p;

  // Trim leading space
  while (isspace((unsigned char) *str)) str++;

  p = str;

  if (*str == 0) {
    return p;
  }

  while (!isspace((unsigned char) *str) && *str != 0) str++;
  if (*str == 0) {
	  return p;
  }

  *str = 0;

  return p;
}

void he_fprintf(char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
}

static void he_remove_api_bindings(he_t *he)
{
	struct cd_list_head *it = NULL, *n = NULL;

	if (!he) {
		return;
	}

	cd_list_for_each_safe(it, n, &he->api_bindings)
	{
		he_api_binding_t *b = cd_container_of(it, he_api_binding_t, link);
		cd_list_del_init(it);
		if (b) {
			he_api_binding_destroy(&b);
		}
	}
}

void he_destroy(he_t **he)
{
	if (!he || !(*he)) {
		return;
	}

	he_remove_api_bindings(*he);
	pthread_mutex_destroy(&(*he)->big_fat_lock);
	free(*he);
	*he = NULL;
}

he_t* he_create(void)
{
	he_t *he = malloc(sizeof(*he));
	if (!he) {
		return NULL;
	}

	memset(he, 0, sizeof(*he));
	memcpy(&he->options, &he_default_options, sizeof(he->options));

	pthread_mutex_init(&he->big_fat_lock, NULL);
	CD_INIT_LIST_HEAD(&he->api_bindings);

	return he;
}

void he_set_user_data(he_t *he, void *user_data)
{
	if (!he) {
		return;
	}

	he->user_data = user_data;
}

void he_set_port(he_t *he, uint16_t port)
{
	if (!he) {
		return;
	}

	he->options.port = port;
}

void he_set_ssl(he_t *he, char *ssl_key_name, char *ssl_cert_name)
{
	if (!he) {
		return;
	}

	he->options.use_ssl = 1;

	if (!he_zstr(ssl_key_name)) {
		strncpy(he->options.ssl_key_name, ssl_key_name, HE_BUFLEN);
		he->options.ssl_key_name[HE_BUFLEN -1] = '\0';
	}

	if (!he_zstr(ssl_cert_name)) {
		strncpy(he->options.ssl_cert_name, ssl_cert_name, HE_BUFLEN);
		he->options.ssl_cert_name[HE_BUFLEN -1] = '\0';
	}
}

he_status_t he_run(he_t *he)
{
	struct mg_mgr mgr = { 0 };
	struct mg_connection *nc = NULL;
	char port[100] = { 0 };
	struct mg_bind_opts bopts = { 0 };


	if (!he)
		return HE_STATUS_TERM;

	bopts.user_data = he;

	if (he->options.use_ssl) {

		if (he_zstr(he->options.ssl_cert_name)) {
			fprintif(HE_LOGLEVEL_1, "HTTPS requested, but no cert specified");
			return HE_STATUS_TERM;
		}
		bopts.ssl_cert = he->options.ssl_cert_name;

		if (he_zstr(he->options.ssl_key_name)) {
			fprintif(HE_LOGLEVEL_1, "HTTPS requested, but no key specified");
			return HE_STATUS_TERM;
		}
		bopts.ssl_key = he->options.ssl_key_name;

		fprintif(HE_LOGLEVEL_1, "Using HTTPS with cert (%s) and key (%s)...", bopts.ssl_cert, bopts.ssl_key);
	}

	mg_mgr_init(&mgr, NULL);

	if (he->options.port == 0) {
		he->options.port = he->options.use_ssl ? HE_DEFAULT_HTTPS_PORT : HE_DEFAULT_HTTP_PORT;
	}

	snprintf(port, 100, ":%u", he->options.port); 
	nc = mg_bind_opt(&mgr, port, he_super_event_handler, he, bopts);
	if (!nc) {
		fprintif(HE_LOGLEVEL_1, "Cannnot bind to port %u", he->options.port);
		return HE_STATUS_TERM;
	}

	mg_set_protocol_http_websocket(nc);

	for (;;) {
		mg_mgr_poll(&mgr, 100);
	}

	he_remove_api_bindings(he);
	mg_mgr_free(&mgr);

	return HE_STATUS_OK;
}
