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

	// Retrieving user's data:
	// my_t *my = (my_t*) he->user_data;


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
	he_t *server = he_create();
	if (!server) {
		return -1;
	}

	// For SSL:
	// he_set_ssl(server, ssl_key_name, ssl_cert_name);

	// Passing user's data to callbacks:
	// he_set_user_data(server, ptr);

	he_register_post_handler(server, BOOM_URL, boom_handler);
	he_run(server);

	return 0;
}
