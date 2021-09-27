#include "he.h"


he_api_binding_t* he_api_binding_create(char *url, he_api_handler_t handler, uint32_t accept_mask, uint32_t reject_mask)
{
	he_api_binding_t *binding = NULL;


	if (!handler || he_zstr(url)) {
		return NULL;
	}

	binding = malloc(sizeof(*binding));
	if (!binding) {
		return NULL;
	}

	memset(binding, 0, sizeof(*binding));

	strncpy(binding->url, url, HE_BUFLEN);
	binding->url[HE_BUFLEN - 1] = 0;

	binding->http_method_mask_accept = accept_mask;
	binding->http_method_mask_reject = reject_mask;

	binding->f = handler;
	return binding;
}

void he_api_binding_destroy(he_api_binding_t **binding)
{
	if (!binding || !(*binding)) {
		return;
	}
	free(*binding);
	*binding = NULL;
}

void he_api_binding_add(he_t *he, he_api_binding_t *binding)
{
	if (!he || !binding) {
		return;
	}

	cd_list_add(&binding->link, &he->api_bindings);
}

typedef he_api_binding_t* he_api_binding_predicate_t(he_api_binding_t *e, void *pred_obj);

he_api_binding_t* he_api_binding_walk(struct cd_list_head *bindings, he_api_binding_predicate_t pred, void *pred_obj)
{
	struct cd_list_head *it = NULL;

	if (!bindings || !pred) {
		return NULL;
	}

	cd_list_for_each(it, bindings)
	{
		he_api_binding_t *b = cd_container_of(it, he_api_binding_t, link);
		if (b) {
			if (pred(b, pred_obj)) {
				return b;
			}
		}
	}

	return NULL;
}

static he_api_binding_t* he_api_binding_find_by_url_pred(he_api_binding_t *binding, void *pred_obj)
{
	const char *url = pred_obj;
	char *u = NULL, *b = NULL, *up = NULL, *bp = NULL;
	uint32_t u_len = 0, b_len = 0, up_len = 0, bp_len = 0;

	if (!binding || !binding->url || !url) {
		return NULL;
	}

	// Strip first '/' (if any) and last '/' if any and compare

	u_len = strlen(url);
	b_len = strlen(binding->url);
	up = (char*) url;
	bp = (char*) binding->url;

	if ((u_len == 0) && (b_len == 0)) {
		return binding;
	}

	if (u_len > 0) {
		u = (url[0] == '/' ? strdup(url + 1) : strdup(url));
		up = he_string_trim_whitespace(u);
		up_len = strlen(u);
		if (up_len > 0) {
			if (up[up_len - 1] == '/') {
				up[up_len - 1] = '\0';
			}
		}
	}

	if (b_len > 0) {
		b = (binding->url[0] == '/' ? strdup(binding->url + 1) : strdup(binding->url));
		bp = he_string_trim_whitespace(b);
		bp_len = strlen(bp);
		if (bp_len > 0) {
			if (bp[bp_len - 1] == '/') {
				bp[bp_len - 1] = '\0';
			}
		}
	}

	if ((up_len == 0) && (bp_len == 0)) {
		if (u) free(u);
		if (b) free(b);
		return binding;
	}

	if (!strcmp(up, bp)) {
		if (u) free(u);
		if (b) free(b);
		return binding;
	}

	if (u) free(u);
	if (b) free(b);

	return NULL;
}

static he_api_binding_t* he_api_binding_find_by_url_and_mask_pred(he_api_binding_t *binding, void *pred_obj)
{
	he_api_binding_pred_obj_t *po = pred_obj;

	if (!binding || !binding->url || !po) {
		return NULL;
	}

	if (he_api_binding_find_by_url_pred(binding, (void*) po->url) == binding) {
		if (0xff & (po->http_method & binding->http_method_mask_accept)) {
			return binding;
		}
	}

	return NULL;
}

he_api_binding_t* he_api_binding_find_by_url(he_t *he, const char *url)
{
	return he_api_binding_walk(&he->api_bindings, he_api_binding_find_by_url_pred, (void*) url);
}

he_api_binding_t* he_api_binding_find_by_url_and_http_method_type(he_t *he, const char *url, uint32_t method)
{
	he_api_binding_pred_obj_t pred_obj = { .url = url, .http_method = method };
	return he_api_binding_walk(&he->api_bindings, he_api_binding_find_by_url_and_mask_pred, &pred_obj);
}
