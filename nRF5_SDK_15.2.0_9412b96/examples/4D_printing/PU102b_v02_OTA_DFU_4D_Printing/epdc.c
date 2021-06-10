#include "epdc.h"
#include "epdpsu.h"

#include <string.h>
//#include "assert.h"

//#define LOG_TAG "epdc"
//#include "utils.h"

const char wf_init[] = WF_INIT;
const char wf_refresh[] = WF_REFRESH;
const char wf_delta[] = WF_DELTA;
const char wf_refresh_mono[] = WF_REFRESH"/"WF_MONO;
const char wf_delta_mono[] = WF_DELTA"/"WF_MONO;

int pl_epdc_get_wfid(struct pl_epdc *p, const char *wf_path)
{
	const struct pl_wfid *wfid;

//	assert(p != NULL);
//	assert(wf_path != NULL);

	/* Optimised string comparison first */
	for (wfid = p->wf_table; wfid->path != NULL; ++wfid)
		if (wfid->path == wf_path)
			return wfid->id;

	/* Full string compare now */
	for (wfid = p->wf_table; wfid->path != NULL; ++wfid)
		if (!strcmp(wfid->path, wf_path))
			return wfid->id;

	return -1;
}

int pl_epdc_single_update(struct pl_epdc *epdc, struct pl_epdpsu *psu,
			  int wfid, const struct pl_area *area)
{
	if (epdc->update_temp(epdc))
		return -1;

	if (psu->on(psu))
		return -1;

	if (epdc->update(epdc, wfid, area))
		return -1;

	if (epdc->wait_update_end(epdc))
		return -1;

	return psu->off(psu);
}