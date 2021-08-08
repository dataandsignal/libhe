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
	void *p = NULL;
	const char *url = pred_obj;

	if (!binding || !binding->url || !url) {
		return NULL;
	}

	p = strstr(url, binding->url);
	if (p && (p == url)) {
		return binding;
	}

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
