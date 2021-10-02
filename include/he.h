#ifndef HTTP_ENDPOINT_H
#define HTTP_ENDPOINT_H


#include <cd/cd.h>

#define MG_ENABLE_SSL 1
#include "mongoose.h"

#include <cjson/cJSON.h>

// cJSON_ParseWithLength may be unavialable on older cJSON, so let's fix this
#ifndef cJSON_ParseWithLength
	#define cJSON_ParseWithLength(v, b) cJSON_ParseWithOpts((v), 0, 0)
#endif

#define HE_DEFAULT_HTTP_PORT 80
#define HE_DEFAULT_HTTPS_PORT 443
#define HE_BUFLEN 2000

#define HE_HTTP_METHOD_INVALID (0u)
#define HE_HTTP_METHOD_GET	(1u << 1)
#define HE_HTTP_METHOD_PUT	(1u << 2)
#define HE_HTTP_METHOD_POST (1u << 3)
#define HE_HTTP_METHOD_HEAD	(1u << 4)
#define HE_HTTP_METHOD_ANY	(0xff)

#define HE_LOGLEVEL_0 0
#define HE_LOGLEVEL_1 1
#define HE_LOGLEVEL_2 2
#define HE_LOGLEVEL_3 3

typedef enum he_status_e {
	HE_STATUS_OK,
	HE_STATUS_TERM,
} he_status_t;

typedef struct he_options_s {
	uint16_t port;
	uint8_t use_ssl;
	char ssl_key_name[HE_BUFLEN];
	char ssl_cert_name[HE_BUFLEN];
} he_options_t;

typedef struct he_s {
	he_options_t		options;
	struct cd_list_head	api_bindings;
	pthread_mutex_t big_fat_lock;
	struct mg_connection *nc;
	struct http_message *http_msg;
	void *user_data;
} he_t;

typedef void he_api_handler_t(he_t *he);

typedef struct he_api_binding_pred_obj_s {
	const char *url;
	uint32_t http_method;
	uint32_t accept_mask;
	uint32_t reject_mask;
} he_api_binding_pred_obj_t;

typedef struct he_api_binding_s {
	struct cd_list_head link;
	char url[HE_BUFLEN];
	he_api_handler_t *f;
	int is_accepting_params;
	uint32_t http_method_mask_accept;
	uint32_t http_method_mask_reject;
} he_api_binding_t;

uint32_t he_str_2_http_method(const char *m);
he_api_binding_t* he_api_binding_create(char *url, he_api_handler_t handler, uint32_t accept_mask, uint32_t reject_mask);
void he_api_binding_destroy(he_api_binding_t **binding);
void he_api_binding_add(he_t *he, he_api_binding_t *binding);
he_api_binding_t* he_api_binding_find_by_url(he_t *he, const char *url);
he_api_binding_t* he_api_binding_find_by_url_and_http_method_type(he_t *he, const char *url, uint32_t method);
void he_super_event_handler(struct mg_connection *nc, int event, void *hm, void *d);

uint8_t he_zstr(const char *s);
char* he_string_trim_whitespace(char *str);


// Public API

he_status_t he_run(he_t *he);
void he_destroy(he_t **he);
he_t* he_create(void);
void he_set_port(he_t *he, uint16_t port);
void he_set_user_data(he_t *he, void *user_data);
void he_set_ssl(he_t *he, char *ssl_key_name, char *ssl_cert_name);
he_status_t he_register_get_handler(he_t *he, char *url, he_api_handler_t handler);
he_status_t he_register_put_handler(he_t *he, char *url, he_api_handler_t handler);
he_status_t he_register_post_handler(he_t *he, char *url, he_api_handler_t handler);
he_status_t he_register_head_handler(he_t *he, char *url, he_api_handler_t handler);
void he_close_http_connection_with_message(he_t *he, const char *msg, const char *body);

// Extern, so it can be redefined (without warning) in a user file
extern int he_loglevel;
void he_fprintf(char *fmt, ...);
#define HE_LOG_FMT(x) "%s:%d " x "\n"
#define fprintif(level, fmt, ...) if (he_loglevel >= level) { he_fprintf(HE_LOG_FMT(fmt), __FILE__, __LINE__, ##__VA_ARGS__); }
#define he_min(a, b) (a) < (b) ? (a) : (b)

#endif // HTTP_ENDPOINT_H
