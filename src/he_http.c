#include "he.h"


uint32_t he_str_2_http_method(const char *m)
{
	if (he_zstr(m)) {
		return HE_HTTP_METHOD_INVALID;
	}

	if (strstr(m, "GET") == m) return HE_HTTP_METHOD_GET;
	if (strstr(m, "PUT") == m) return HE_HTTP_METHOD_PUT;
	if (strstr(m, "POST") == m) return HE_HTTP_METHOD_POST;
	if (strstr(m, "HEAD") == m) return HE_HTTP_METHOD_HEAD;

	return HE_HTTP_METHOD_INVALID;
}

static const char* he_http_method_2_str(uint32_t method)
{
	switch (method) {
		case HE_HTTP_METHOD_GET:
			return "GET";
		case HE_HTTP_METHOD_PUT:
			return "PUT";
		case HE_HTTP_METHOD_POST:
			return "POST";
		case HE_HTTP_METHOD_HEAD:
			return "HEAD";
		default:
			return "INVALID";
	}
}

void he_close_http_connection_with_message(he_t *he, const char *msg, const char *body)
{
	if (!he) {
		return;
	}

	if (he->nc) {
		if (!he_zstr(msg)) {
			if (!he_zstr(body))
				mg_printf(he->nc, "HTTP/1.1 %s\r\nContent-Length: %zu\r\nContent-Type: text/plain\r\n\r\n%s", msg, strlen(body), body);
			else
				mg_printf(he->nc, "HTTP/1.1 %s\r\nContent-Length: 0\r\n\r\n", msg);
		}
		he->nc->flags |= MG_F_SEND_AND_CLOSE;
	}
}

static void he_close_http_connection_with_404_not_found_error(struct mg_connection *nc)
{
	if (nc) {
		mg_printf(nc, "HTTP/1.1 404 Not Found\r\n\r\n");
	}
	if (nc) nc->flags |= MG_F_SEND_AND_CLOSE;
}

static void he_close_http_connection(struct mg_connection *nc, struct mbuf *io)
{
	if (nc) mg_send_http_chunk(nc, "", 0);
	if (io) mbuf_remove(io, io->len);
	if (nc) nc->flags |= MG_F_SEND_AND_CLOSE;
}

void he_super_event_handler(struct mg_connection *nc, int event, void *hm, void *d)
{
	struct http_message *m = (struct http_message*) hm;
	he_api_binding_t *binding = NULL;
	he_t *he = (he_t*) d;
	char addr[32] = { 0 };


	if (!he) {
		fprintif(HE_LOGLEVEL_2, "Bad params");
		goto exit;
	}

	he->nc = nc;
	he->http_msg = m;

	fprintif(HE_LOGLEVEL_3, "Event [%d]...", event);

	if (nc) {
		mg_sock_addr_to_str(&nc->sa, addr, sizeof(addr), MG_SOCK_STRINGIFY_IP | MG_SOCK_STRINGIFY_PORT);
		fprintif(HE_LOGLEVEL_3, "%p: %s\n", nc, addr);
	}

	switch (event) {

		case MG_EV_ACCEPT:
			{
				fprintif(HE_LOGLEVEL_2, "MG_EV_ACCEPT");
				break;
			}

		case MG_EV_RECV:
			fprintif(HE_LOGLEVEL_2, "MG_EV_RECV...");
			break;

		case MG_EV_HTTP_REQUEST:
			{
				char this_uri[HE_BUFLEN] = { 0 };
				char this_method[HE_BUFLEN] = { 0 };
				uint32_t http_method = 0;

				fprintif(HE_LOGLEVEL_2, "MG_EV_HTTP_REQUEST");

				if (!m || !m->method.p) {
					fprintif(HE_LOGLEVEL_2, "Bad params, no HTTP context");
					return;
				}

				strncpy(this_method, m->method.p, he_min(HE_BUFLEN, m->method.len));
				this_method[HE_BUFLEN - 1] = '\0';

				http_method = he_str_2_http_method(this_method);
				if (http_method == HE_HTTP_METHOD_INVALID) {
					he_close_http_connection_with_message(he, "404 Method invalid", NULL);
					fprintif(HE_LOGLEVEL_2, "===HTTP method invalid (closed HTTP connection with error 404)");
					return;
				}

				if (m->uri.p) {

					strncpy(this_uri, m->uri.p, he_min(HE_BUFLEN, m->uri.len));
					this_uri[HE_BUFLEN - 1] = '\0';

					fprintif(HE_LOGLEVEL_2, "Searching %s (%u) handler for: %s", he_http_method_2_str(http_method), http_method, this_uri);

					binding = he_api_binding_find_by_url_and_http_method_type(he, m->uri.p, http_method);
					if (!binding) {
						he_close_http_connection_with_message(he, "404 API binding not found", NULL);
						fprintif(HE_LOGLEVEL_2, "Binding for %s not found (closed HTTP connection with error 404)", this_uri);
						break;
					}

					fprintif(HE_LOGLEVEL_2, "-> Handler found");

					if (!binding->f) {
						he_close_http_connection_with_message(he, "500 API handler misconfigured", "API binding found but handler is not set");
						fprintif(HE_LOGLEVEL_2, "Binding for %s found, but handler is not set (closed HTTP connection with error 500)", this_uri);
						break;
					}

					binding->f(he);
				}
				break;
			}

		default:
			break;
	}

exit:

	return;
}

static he_status_t he_register_handler_ex(he_t *he, char *url, he_api_handler_t handler, uint32_t accept_mask, uint32_t reject_mask)
{
	he_api_binding_t *binding = NULL;

	if (!he || he_zstr(url) || !handler) {
		return HE_STATUS_TERM;
	}

	binding = he_api_binding_create(url, handler, accept_mask, reject_mask);
	if (!binding) {
		return HE_STATUS_TERM;
	}

	pthread_mutex_lock(&he->big_fat_lock);
	he_api_binding_add(he, binding);
	pthread_mutex_unlock(&he->big_fat_lock);

	return HE_STATUS_OK;
}

he_status_t he_register_get_handler(he_t *he, char *url, he_api_handler_t handler)
{
	return he_register_handler_ex(he, url, handler, HE_HTTP_METHOD_GET, 0);
}

he_status_t he_register_put_handler(he_t *he, char *url, he_api_handler_t handler)
{
	return he_register_handler_ex(he, url, handler, HE_HTTP_METHOD_PUT, 0);
}

he_status_t he_register_post_handler(he_t *he, char *url, he_api_handler_t handler)
{
	return he_register_handler_ex(he, url, handler, HE_HTTP_METHOD_POST, 0);
}

he_status_t he_register_head_handler(he_t *he, char *url, he_api_handler_t handler)
{
	return he_register_handler_ex(he, url, handler, HE_HTTP_METHOD_HEAD, 0);
}
