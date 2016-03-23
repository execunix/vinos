/*	$NetBSD: partman.c,v 1.4.4.4 2015/05/14 07:58:49 snj Exp $ */

/*
 * Copyright 2012 Eugene Lozovoy
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of Eugene Lozovoy may not be used to endorse
 *    or promote products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY PIERMONT INFORMATION SYSTEMS INC. ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL PIERMONT INFORMATION SYSTEMS INC. BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/* partman.c - extended partitioning */

#include <fcntl.h>
#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <util.h>

#include "defs.h"
#include "msg_defs.h"
#include "menu_defs.h"

/* flags whether to offer the respective options (depending on helper
   programs available on install media */
int have_vnd, have_dk;

/* XXX: replace all MAX_* defines with vars that depend on kernel settings */
#define MAX_ENTRIES 96

#define MAX_VND 4
typedef struct vnds_t {
	int enabled;
	int blocked;
	int node;
	char filepath[STRSIZE];
	int size;
	int readonly;
	int is_exist;
	int manual_geom;
	int secsize, nsectors, ntracks, ncylinders;
	int pm_part;    /* Used only for */
	pm_devs_t *pm;  /* referring device */
} vnds_t;
vnds_t vnds[MAX_VND];

typedef struct structinfo_t {
	int max;
	uint entry_size;
	uint parent_size;
	void *entry_first;
	void *entry_enabled;
	void *entry_blocked;
	void *entry_node;
} structinfo_t;
structinfo_t vnds_t_info;

typedef struct pm_upddevlist_adv_t {
	const char *create_msg;
	int pe_type;
	structinfo_t *s;
	int sub_num;
	struct pm_upddevlist_adv_t *sub;
} pm_upddevlist_adv_t;

#define MAX_MNTS 48
struct {
    char dev[STRSIZE];
    const char *mnt_opts, *on;
} mnts[MAX_MNTS];

int cursel; /* Number of selected entry in main menu */
int changed; /* flag indicating that we have unsaved changes */

enum { /* VND menu enum */
	PMV_MENU_FILEPATH, PMV_MENU_EXIST, PMV_MENU_SIZE, PMV_MENU_RO, PMV_MENU_MGEOM,
	PMV_MENU_SECSIZE, PMV_MENU_NSECTORS, PMV_MENU_NTRACKS, PMV_MENU_NCYLINDERS,
	PMV_MENU_REMOVE, PMV_MENU_END
};

part_entry_t pm_dev_list(int);
static int pm_mount(pm_devs_t *, int);
static int pm_upddevlist(menudesc *, void *);
static void pm_select(pm_devs_t *);

/* Universal menu for VND entry edit */
static int
pm_edit(int menu_entries_count, void (*menu_fmt)(menudesc *, int, void *),
	int (*action)(menudesc *, void *), int (*check_fun)(void *),
	void (*entry_init)(void *, void *),	void *entry_init_arg,
	void *dev_ptr, int dev_ptr_delta, structinfo_t *s)
{
	int i, ok = 0;

	if (dev_ptr == NULL) {
		/* We should create new device */
		for (i = 0; i < s->max && !ok; i++)
			if (*(int*)((char*)s->entry_enabled + dev_ptr_delta + s->entry_size * i) == 0) {
				dev_ptr = (char*)s->entry_first + dev_ptr_delta + s->entry_size * i;
				entry_init(dev_ptr, entry_init_arg);
				ok = 1;
			}
		if (!ok) {
			/* We do not have free device slots */
			process_menu(MENU_ok, deconst(MSG_limitcount));
			return -1;
		}
	}

	menu_ent menu_entries[menu_entries_count];
	for (i = 0; i < menu_entries_count - 1; i++)
		menu_entries[i] = (menu_ent) {NULL, OPT_NOMENU, 0, action};
	menu_entries[i] = (menu_ent) {MSG_fremove, OPT_NOMENU, OPT_EXIT, action};

	int menu_no = -1;
	menu_no = new_menu(NULL, menu_entries, menu_entries_count,
		-1, -1, 0, 40, MC_NOCLEAR | MC_SCROLL,
		NULL, menu_fmt, NULL, NULL, MSG_DONE);
	
	process_menu(menu_no, dev_ptr);
	free_menu(menu_no);

	return check_fun(dev_ptr);
}

static void
pm_getdevstring(char *buf, int len, pm_devs_t *pm_cur, int num)
{
	int i;

	if (num + 'a' < 'a' || num + 'a' > 'a' + MAXPARTITIONS)
		snprintf(buf, len, "%sd", pm_cur->diskdev);
	else
		snprintf(buf, len, "%s%c", pm_cur->diskdev, num + 'a');

	return;
}

/* Show filtered partitions menu */
part_entry_t
pm_dev_list(int type)
{
	int dev_num = -1, num_devs = 0;
	int i, ok;
	int menu_no;
	menu_ent menu_entries[MAX_DISKS*MAXPARTITIONS];
	part_entry_t disk_entries[MAX_DISKS*MAXPARTITIONS];
	pm_devs_t *pm_i;

	SLIST_FOREACH(pm_i, &pm_head, l)
		for (i = 0; i < MAXPARTITIONS; i++) {
			ok = 0;
			if (ok && pm_partusage(pm_i, i, 0) == 0) {
				disk_entries[num_devs].dev_ptr = pm_i;
				disk_entries[num_devs].dev_num = i;
				pm_getdevstring(disk_entries[num_devs].fullname, SSTRSIZE, pm_i, i);

				menu_entries[num_devs] = (struct menu_ent) {
					.opt_name = disk_entries[num_devs].fullname,
					.opt_action = set_menu_select,
					.opt_menu = OPT_NOMENU,
					.opt_flags = OPT_EXIT,
				};
				num_devs++;
			}
		}

	menu_no = new_menu(MSG_avdisks,
		menu_entries, num_devs, -1, -1, (num_devs+1<3)?3:num_devs+1, 13,
		MC_SCROLL | MC_NOCLEAR, NULL, NULL, NULL, NULL, NULL);
	if (menu_no == -1)
		return (part_entry_t) { .retvalue = -1, };
	process_menu(menu_no, &dev_num);
	free_menu(menu_no);

	if (dev_num < 0 || dev_num >= num_devs)
		return (part_entry_t) { .retvalue = -1, };

	disk_entries[dev_num].retvalue = dev_num;
	return disk_entries[dev_num];
}

/* Get unused vnd* device */
static int
pm_manage_getfreenode(void *node, const char *d, structinfo_t *s)
{
	int i, ii, ok;
	char buf[SSTRSIZE];
	pm_devs_t *pm_i;

	*(int*)node = -1;
	for (i = 0; i < s->max; i++) {
		ok = 1;
		/* Check that node is not already reserved */
		for (ii = 0; ii < s->max; ii++)
			if (*(int*)((char*)s->entry_node + s->entry_size * ii) == i) {
				ok = 0;
				break;
			}
		if (! ok)
			continue;
		/* Check that node is not in the device list */
		snprintf(buf, SSTRSIZE, "%s%d", d, i);
		SLIST_FOREACH(pm_i, &pm_head, l)
			if (! strcmp(pm_i->diskdev, buf)) {
				ok = 0;
				break;
			}
		if (ok) {
			*(int*)node = i;
			return i;
		}
	}
	process_menu(MENU_ok, deconst(MSG_nofreedev));
	return -1;
}

/***
 VND
 ***/

static void
pm_vnd_menufmt(menudesc *m, int opt, void *arg)
{
	vnds_t *dev_ptr = ((part_entry_t *)arg)[opt].dev_ptr;

	if (dev_ptr->enabled == 0)
		return;
	if (strlen(dev_ptr->filepath) < 1)
		wprintw(m->mw, "%s", msg_string(MSG_vnd_err_menufmt));
	else if (dev_ptr->is_exist)
		wprintw(m->mw, msg_string(MSG_vnd_assgn_menufmt),
			dev_ptr->node, dev_ptr->filepath);
	else 
		wprintw(m->mw, msg_string(MSG_vnd_menufmt),
			dev_ptr->node, dev_ptr->filepath, dev_ptr->size);
	return;
}

static void
pm_vnd_edit_menufmt(menudesc *m, int opt, void *arg)
{
	vnds_t *dev_ptr = arg;
	char buf[SSTRSIZE];
	strcpy(buf, "-");

	switch (opt) {
		case PMV_MENU_FILEPATH:
			wprintw(m->mw, msg_string(MSG_vnd_path_fmt), dev_ptr->filepath);
			break;
		case PMV_MENU_EXIST:
			wprintw(m->mw, msg_string(MSG_vnd_assgn_fmt),
				dev_ptr->is_exist? msg_string(MSG_Yes) : msg_string(MSG_No));
			break;
		case PMV_MENU_SIZE:
			if (!dev_ptr->is_exist)
				snprintf(buf, SSTRSIZE, "%d", dev_ptr->size);
			wprintw(m->mw, msg_string(MSG_vnd_size_fmt), buf);
			break;
		case PMV_MENU_RO:
			wprintw(m->mw, msg_string(MSG_vnd_ro_fmt),
				dev_ptr->readonly? msg_string(MSG_Yes) : msg_string(MSG_No));
			break;
		case PMV_MENU_MGEOM:
			if (!dev_ptr->is_exist)
				snprintf(buf, SSTRSIZE, "%s",
					dev_ptr->manual_geom? msg_string(MSG_Yes) : msg_string(MSG_No));
			wprintw(m->mw, msg_string(MSG_vnd_geom_fmt), buf);
			break;
		case PMV_MENU_SECSIZE:
			if (dev_ptr->manual_geom && !dev_ptr->is_exist)
				snprintf(buf, SSTRSIZE, "%d", dev_ptr->secsize);
			wprintw(m->mw, msg_string(MSG_vnd_bps_fmt), buf);
			break;
		case PMV_MENU_NSECTORS:
			if (dev_ptr->manual_geom && !dev_ptr->is_exist)
				snprintf(buf, SSTRSIZE, "%d", dev_ptr->nsectors);
			wprintw(m->mw, msg_string(MSG_vnd_spt_fmt), buf);
			break;
		case PMV_MENU_NTRACKS:
			if (dev_ptr->manual_geom && !dev_ptr->is_exist)
				snprintf(buf, SSTRSIZE, "%d", dev_ptr->ntracks);
			wprintw(m->mw, msg_string(MSG_vnd_tpc_fmt), buf);
			break;
		case PMV_MENU_NCYLINDERS:
			if (dev_ptr->manual_geom && !dev_ptr->is_exist)
				snprintf(buf, SSTRSIZE, "%d", dev_ptr->ncylinders);
			wprintw(m->mw, msg_string(MSG_vnd_cyl_fmt), buf);
			break;
	}
	return;
}

static int
pm_vnd_set_value(menudesc *m, void *arg)
{
	vnds_t *dev_ptr = arg;
	char buf[STRSIZE];
	const char *msg_to_show = NULL;
	int *out_var = NULL;
	
	switch (m->cursel) {
		case PMV_MENU_FILEPATH:
			msg_prompt_win(MSG_vnd_path_ask, -1, 18, 0, 0, dev_ptr->filepath,
				dev_ptr->filepath, STRSIZE);
			if (dev_ptr->filepath[0] != '/') {
				strlcpy(buf, dev_ptr->filepath, MOUNTLEN);
				snprintf(dev_ptr->filepath, MOUNTLEN, "/%s", buf);
			}
			if (dev_ptr->filepath[strlen(dev_ptr->filepath) - 1] == '/')
				dev_ptr->filepath[strlen(dev_ptr->filepath) - 1] = '\0';
			return 0;
		case PMV_MENU_EXIST:
			dev_ptr->is_exist = !dev_ptr->is_exist;
			return 0;
		case PMV_MENU_SIZE:
			if (dev_ptr->is_exist)
				return 0;
			msg_to_show = MSG_vnd_size_ask;
			out_var = &(dev_ptr->size);
			break;
		case PMV_MENU_RO:
			dev_ptr->readonly = !dev_ptr->readonly;
			return 0;
		case PMV_MENU_MGEOM:
			if (dev_ptr->is_exist)
				return 0;
			dev_ptr->manual_geom = !dev_ptr->manual_geom;
			return 0;
		case PMV_MENU_SECSIZE:
			if (!dev_ptr->manual_geom || dev_ptr->is_exist)
				return 0;
			msg_to_show = MSG_vnd_bps_ask;
			out_var = &(dev_ptr->secsize);
			break;
		case PMV_MENU_NSECTORS:
			if (!dev_ptr->manual_geom || dev_ptr->is_exist)
				return 0;
			msg_to_show = MSG_vnd_spt_ask;
			out_var = &(dev_ptr->nsectors);
			break;
		case PMV_MENU_NTRACKS:
			if (!dev_ptr->manual_geom || dev_ptr->is_exist)
				return 0;
			msg_to_show = MSG_vnd_tpc_ask;
			out_var = &(dev_ptr->ntracks);
			break;
		case PMV_MENU_NCYLINDERS:
			if (!dev_ptr->manual_geom || dev_ptr->is_exist)
				return 0;
			msg_to_show = MSG_vnd_cyl_ask;
			out_var = &(dev_ptr->ncylinders);
			break;
		case PMV_MENU_REMOVE:
			dev_ptr->enabled = 0;
			return 0;
	}
	if (out_var == NULL || msg_to_show == NULL)
		return -1;
	snprintf(buf, SSTRSIZE, "%d", *out_var);
	msg_prompt_win(msg_to_show, -1, 18, 0, 0, buf, buf, SSTRSIZE);
	if (atoi(buf) >= 0)
		*out_var = atoi(buf);
	return 0;
}

static void
pm_vnd_init(void *arg, void *none)
{
	vnds_t *dev_ptr = arg;
	memset(dev_ptr, 0, sizeof(*dev_ptr));
	*dev_ptr = (struct vnds_t) {
		.enabled = 1,
		.blocked = 0,
		.filepath[0] = '\0',
		.is_exist = 0,
		.size = 1024,
		.readonly = 0,
		.manual_geom = 0,
		.secsize = 512,
		.nsectors = 18,
		.ntracks = 2,
		.ncylinders = 80
	};
	return;
}

static int
pm_vnd_check(void *arg)
{
	vnds_t *dev_ptr = arg;

	if (dev_ptr->blocked)
		return 0;
	if (strlen(dev_ptr->filepath) < 1 ||
			dev_ptr->size < 1)
		dev_ptr->enabled = 0;
	else {
		pm_manage_getfreenode(&(dev_ptr->node), "vnd", &vnds_t_info);
		if (dev_ptr->node < 0)
			dev_ptr->enabled = 0;
	}
	return dev_ptr->enabled;
}

/* XXX: vnconfig always return 0? */
static int
pm_vnd_commit(void)
{
	int i, ii, error, part_suit = -1;
	char r_o[3], buf[MOUNTLEN+3], resultpath[STRSIZE]; 
	pm_devs_t *pm_i, *pm_suit = NULL;

	for (i = 0; i < MAX_VND; i++) {
		error = 0;
		if (! pm_vnd_check(&vnds[i]))
			continue;

		/* Trying to assign target device */
		SLIST_FOREACH(pm_i, &pm_head, l)
			for (ii = 0; ii < 6; ii++) {
				strcpy(buf, pm_i->bsdlabel[ii].pi_mount);
				if (buf[strlen(buf)-1] != '/')
					sprintf(buf,"%s/", buf);
				printf("%s\n",buf);
				if (strstr(vnds[i].filepath, buf) == vnds[i].filepath)
					if (part_suit < 0 || pm_suit == NULL ||
						strlen(buf) > strlen(pm_suit->bsdlabel[part_suit].pi_mount)) {
						part_suit = ii;
						pm_suit = pm_i;
					}
			}
		if (part_suit < 0 || pm_suit == NULL || 
			pm_suit->bsdlabel[part_suit].mnt_opts == NULL ||
			! (pm_suit->bsdlabel[part_suit].pi_flags & PIF_MOUNT))
			continue;
		
		/* Mounting assigned partition and try to get real file path*/
		if (pm_mount(pm_suit, part_suit) != 0)
			continue;
		snprintf(resultpath, STRSIZE, "%s/%s",
			pm_suit->bsdlabel[part_suit].mounted,
			&(vnds[i].filepath[strlen(pm_suit->bsdlabel[part_suit].pi_mount)]));

		strcpy(r_o, vnds[i].readonly?"-r":"");
		/* If this is a new image */
		if (!vnds[i].is_exist) {
			run_program(RUN_DISPLAY | RUN_PROGRESS, "mkdir -p %s ", dirname(resultpath));
			error += run_program(RUN_DISPLAY | RUN_PROGRESS,
						"dd if=/dev/zero of=%s bs=1m count=%d progress=100 msgfmt=human",
						resultpath, vnds[i].size);
		}
		if (error)
			continue;
		/* If this is a new image with manual geometry */
		if (!vnds[i].is_exist && vnds[i].manual_geom)
			error += run_program(RUN_DISPLAY | RUN_PROGRESS,
						"vnconfig %s vnd%d %s %d %d %d %d", r_o, vnds[i].node,
						resultpath, vnds[i].secsize, vnds[i].nsectors,
						vnds[i].ntracks, vnds[i].ncylinders);
		/* If this is a existing image or image without manual geometry */
		else
			error += run_program(RUN_DISPLAY | RUN_PROGRESS, "vnconfig %s vnd%d %s",
						r_o, vnds[i].node, resultpath);

		if (! error) {
			vnds[i].blocked = 1;
			vnds[i].pm_part = part_suit;
			vnds[i].pm = pm_suit;
			vnds[i].pm->blocked++;
		}
	}
	return 0;
}

/***
 Partman generic functions 
 ***/

int
pm_getrefdev(pm_devs_t *pm_cur)
{
	int i, dev_num;
	char dev[SSTRSIZE]; dev[0] = '\0';

	pm_cur->refdev = NULL;
 	if (! strncmp(pm_cur->diskdev, "vnd", 3)) {
 		dev_num = pm_cur->diskdev[3] - '0';
 		for (i = 0; i < MAX_VND; i++)
			if (vnds[i].blocked && vnds[i].node == dev_num) {
				pm_cur->refdev = &vnds[i];
				pm_getdevstring(dev, SSTRSIZE, vnds[i].pm, vnds[i].pm_part);
				snprintf(pm_cur->diskdev_descr, STRSIZE, "%s (%s, %s)",
					pm_cur->diskdev_descr, dev, vnds[i].filepath);
				break;
			}
	} else
		return -1;
	return 0;
}

/* Detect that partition is in use */
int
pm_partusage(pm_devs_t *pm_cur, int part_num, int do_del)
{
	int i, ii, retvalue = 0;

	if (part_num < 0) {
		/* Check all partitions on device */
		for (i = 0; i < MAXPARTITIONS; i++)
			retvalue += pm_partusage(pm_cur, i, do_del);
		return retvalue;
	}

	return 0;
}

/* Clean up removed devices */
static int
pm_clean(void)
{
	int count = 0;
	pm_devs_t *pm_i;

	SLIST_FOREACH(pm_i, &pm_head, l)
		if (! pm_i->found) {
			count++;
			SLIST_REMOVE(&pm_head, pm_i, pm_devs_t, l);
			free(pm_i);
		}
	return count;
}

void 
pm_make_bsd_partitions(pm_devs_t *pm_cur)
{
	int i;
	partinfo baklabel[MAXPARTITIONS];
	memcpy(baklabel, &pm_cur->bsdlabel, sizeof baklabel);

	pm_cur->unsaved = 1;
	layoutkind = LY_SETNEW;
	pm_cur->bootable = (pm_cur->bsdlabel[pm_cur->rootpart].pi_flags & PIF_MOUNT) ?
		1 : 0;
	for (i = 0; i < MAXPARTITIONS; i++)
		if (pm_cur->bsdlabel[i].pi_fstype != baklabel[i].pi_fstype)
			pm_partusage(pm_cur, i, 1);
}

void
pm_setfstype(pm_devs_t *pm_cur, int part_num, int fstype)
{
	pm_cur->bsdlabel[part_num].pi_mount[0] = '\0';
	pm_cur->bsdlabel[part_num].pi_flags = 0;
	pm_cur->bsdlabel[part_num].pi_fstype = fstype;
	return;	
}

static void
pm_select(pm_devs_t *pm_devs_in)
{
	pm = pm_devs_in;
	if (logfp)
		(void)fprintf(logfp, "Partman device: %s\n", pm->diskdev);
	return;
}

void
pm_rename(pm_devs_t *pm_cur)
{
	pm_select(pm_cur);
	msg_prompt_win(MSG_packname, -1, 18, 0, 0, pm_cur->bsddiskname,
		pm_cur->bsddiskname, sizeof pm_cur->bsddiskname);
	return;
}

int
pm_editpart(int part_num)
{
	partinfo backup = pm->bsdlabel[part_num];

	edit_ptn(&(struct menudesc){.cursel = part_num}, NULL);
	if (checkoverlap(pm->bsdlabel, MAXPARTITIONS, PART_D, PART_C)) {
		msg_display_add(MSG_cantsave);
		process_menu(MENU_ok, NULL);
		pm->bsdlabel[part_num] = backup;
		return -1;
	}
	pm->unsaved = 1;
	return 0;
}

/* Safe erase of disk */
int
pm_shred(pm_devs_t *pm_cur, int part, int shredtype)
{
	int error = -1;
	char dev[SSTRSIZE];

	pm_getdevstring(dev, SSTRSIZE, pm_cur, part);
	switch(shredtype) {
		case SHRED_ZEROS:
			error += run_program(RUN_DISPLAY | RUN_PROGRESS,
				"dd of=/dev/%s if=/dev/zero bs=1m progress=100 msgfmt=human",
				dev);
			break;
		case SHRED_RANDOM:
			error += run_program(RUN_DISPLAY | RUN_PROGRESS,
				"dd of=/dev/%s if=/dev/urandom bs=1m progress=100 msgfmt=human",
				dev);
			break;
		default:
			return -1;
	}
	pm_partusage(pm_cur, -1, 1);
	memset(&pm_cur->oldlabel, 0, sizeof pm_cur->oldlabel);
	memset(&pm_cur->bsdlabel, 0, sizeof pm_cur->bsdlabel);
	return error;
}

static int
pm_mountall_sort(const void *a, const void *b)
{
	return strcmp(mnts[*(const int *)a].on, mnts[*(const int *)b].on);
}

/* Mount all available partitions */
static int
pm_mountall(void)
{
	int num_devs = 0;
	int mnts_order[MAX_MNTS];
	int i, ii, error, ok;
	char dev[SSTRSIZE]; dev[0] = '\0';
	pm_devs_t *pm_i;
	
	localfs_dev[0] = '\0';
	memset(&mnts, 0, sizeof mnts);

	SLIST_FOREACH(pm_i, &pm_head, l) {
		ok = 0;
		for (i = 0; i < MAXPARTITIONS; i++) {
			if (!(pm_i->bsdlabel[i].pi_flags & PIF_MOUNT && 
					pm_i->bsdlabel[i].mnt_opts != NULL))
				continue;
			mnts[num_devs].mnt_opts = pm_i->bsdlabel[i].mnt_opts;
			if (strlen(pm_i->bsdlabel[i].mounted) > 0) {
				/* Device is already mounted. So, doing mount_null */
				strlcpy(mnts[num_devs].dev, pm_i->bsdlabel[i].mounted, MOUNTLEN);
				mnts[num_devs].mnt_opts = "-t null";
			} else {
				pm_getdevstring(dev, SSTRSIZE, pm_i, i);
				snprintf(mnts[num_devs].dev, STRSIZE, "/dev/%s", dev);
			}
			mnts[num_devs].on = pm_i->bsdlabel[i].pi_mount;
			if (strcmp(pm_i->bsdlabel[i].pi_mount, "/") == 0) {
				/* Use disk with / as a default if the user has 
				the sets on a local disk */
				strlcpy(localfs_dev, pm_i->diskdev, SSTRSIZE);
			}
			num_devs++;
			ok = 1;
		}
		if (ok)
			md_pre_mount();
	}
	if (strlen(localfs_dev) < 1) {
		msg_display(MSG_noroot);
		process_menu(MENU_ok, NULL);
		return -1;
	}
	for (i = 0; i < num_devs; i++)
		mnts_order[i] = i;
	qsort(mnts_order, num_devs, sizeof mnts_order[0], pm_mountall_sort);

	for (i = 0; i < num_devs; i++) {
		ii = mnts_order[i];
		make_target_dir(mnts[ii].on);
		error = target_mount_do(mnts[ii].mnt_opts, mnts[ii].dev, mnts[ii].on);
		if (error)
			return error;
	}
	return 0;
}

/* Mount partition bypassing ordinary order */
static int
pm_mount(pm_devs_t *pm_cur, int part_num)
{
	int error = 0;
	char buf[MOUNTLEN];

	if (strlen(pm_cur->bsdlabel[part_num].mounted) > 0)
		return 0;

	snprintf(buf, MOUNTLEN, "/tmp/%s%c", pm_cur->diskdev, part_num + 'a');
	if (! dir_exists_p(buf))
		run_program(RUN_DISPLAY | RUN_PROGRESS, "/bin/mkdir -p %s", buf);
	if (pm_cur->bsdlabel[part_num].pi_flags & PIF_MOUNT &&
		pm_cur->bsdlabel[part_num].mnt_opts != NULL &&
		strlen(pm_cur->bsdlabel[part_num].mounted) < 1)
		error += run_program(RUN_DISPLAY | RUN_PROGRESS, "/sbin/mount %s /dev/%s%c %s",
				pm_cur->bsdlabel[part_num].mnt_opts,
				pm_cur->diskdev, part_num + 'a', buf);

	if (error)
		pm_cur->bsdlabel[part_num].mounted[0] = '\0';
	else {
		strlcpy(pm_cur->bsdlabel[part_num].mounted, buf, MOUNTLEN);
		pm_cur->blocked++;
	}
	return error;
}

void
pm_umount(pm_devs_t *pm_cur, int part_num)
{
	char buf[SSTRSIZE]; buf[0] = '\0';
	pm_getdevstring(buf, SSTRSIZE, pm_cur, part_num);

	if (run_program(RUN_DISPLAY | RUN_PROGRESS,
			"umount -f /dev/%s", buf) == 0) {
		pm_cur->bsdlabel[part_num].mounted[0] = '\0';
		if (pm_cur->blocked > 0)
			pm_cur->blocked--;
	}
	return;
}

int
pm_unconfigure(pm_devs_t *pm_cur)
{
	int error = 0, num = 0;
	if (! strncmp(pm_cur->diskdev, "vnd", 3)) {
		error = run_program(RUN_DISPLAY | RUN_PROGRESS, "vnconfig -u %s", pm_cur->diskdev);
 		if (! error && pm_cur->refdev != NULL) {
			((vnds_t*)pm_cur->refdev)->pm->blocked--;
			((vnds_t*)pm_cur->refdev)->blocked = 0;
 		}
	}
	else
		error = run_program(RUN_DISPLAY | RUN_PROGRESS, "eject -t disk /dev/%sd",
			pm_cur->diskdev);
	if (!error)
		pm_cur->found = 0;
	return error;
}

/* Last checks before leaving partition manager */
static int
pm_lastcheck(void)
{
	FILE *file_tmp = fopen(concat_paths(targetroot_mnt, "/etc/fstab"), "r");
	if (file_tmp == NULL)
		return 1;
	fclose(file_tmp);
	return 0;
}

/* Are there unsaved changes? */
static int
pm_needsave(void)
{
	pm_devs_t *pm_i;
	SLIST_FOREACH(pm_i, &pm_head, l)
		if (pm_i->unsaved) {
			/* Oops, we have unsaved changes */
			changed = 1;
			msg_display(MSG_saveprompt);
			return ask_yesno(NULL);
		}
	return 0;
}

/* Write all changes to disk */
static int
pm_commit(menudesc *m, void *arg)
{
	int retcode;
	pm_devs_t *pm_i;
	if (m != NULL && arg != NULL)
		((part_entry_t *)arg)[0].retvalue = m->cursel + 1;

	SLIST_FOREACH(pm_i, &pm_head, l) {
		if (! pm_i->unsaved)
			continue;
		pm_select(pm_i);

		if (
			set_swap_if_low_ram(pm->diskdev, pm->bsdlabel) != 0 || 
			make_filesystems() != 0     /* Create filesystems with newfs */
		) {
			/* Oops, something failed... */
			if (logfp)
				fprintf(logfp, "Disk %s preparing error\n", pm_i->diskdev);
			continue;
		}
		pm_i->unsaved = 0;
	}

	/* Call all functions that may create new devices */
	if ((retcode = pm_vnd_commit()) != 0) {
		if (logfp)
			fprintf(logfp, "VND configuring error #%d\n", retcode);
		return -1;
	}
	if (m != NULL && arg != NULL)
		pm_upddevlist(m, arg);
	if (logfp)
		fflush (logfp);
    return 0;
}

static int
pm_savebootsector(void)
{
	pm_devs_t *pm_i;
	SLIST_FOREACH(pm_i, &pm_head, l)
		if (pm_i->bootable) {
			pm_select(pm_i);
			if (
			    md_post_newfs() != 0) {
				if (logfp)
					fprintf(logfp, "Error writting bootsector to %s\n",
						pm_i->diskdev);
				continue;
			}
		}
	return 0;
}

/* Function for 'Enter'-menu */
static int
pm_submenu(menudesc *m, void *arg)
{
	int part_num = -1;
	pm_devs_t *pm_cur = NULL;
	((part_entry_t *)arg)[0].retvalue = m->cursel + 1;

	switch (((part_entry_t *)arg)[m->cursel].type) {
		case PM_DISK_T:
		case PM_PART_T:
			if (((part_entry_t *)arg)[m->cursel].dev_ptr != NULL) {
				pm_cur = ((part_entry_t *)arg)[m->cursel].dev_ptr;
				if (pm_cur == NULL) 
					return -1;
				if (pm_cur->blocked) {
					msg_display(MSG_wannaunblock);
					if (!ask_noyes(NULL))
						return -2;
					pm_cur->blocked = 0;
				}
				pm_select(pm_cur);
			}
		default:
			break;
	}

	switch (((part_entry_t *)arg)[m->cursel].type) {
		case PM_DISK_T:
			process_menu(MENU_pmdiskentry, &part_num);
			break;
		case PM_PART_T:
			part_num = ((part_entry_t *)arg)[m->cursel].dev_num;
			process_menu(MENU_pmpartentry, &part_num);
			break;
		case PM_VND_T:
			return pm_edit(PMV_MENU_END, pm_vnd_edit_menufmt,
				pm_vnd_set_value, pm_vnd_check, pm_vnd_init,
				NULL, ((part_entry_t *)arg)[m->cursel].dev_ptr, 0, &vnds_t_info);
	}
	return 0;
}

/* Functions that generate menu entries text */
static void
pm_menufmt(menudesc *m, int opt, void *arg)
{
	const char *dev_status = "";
	char buf[STRSIZE];
	int part_num = ((part_entry_t *)arg)[opt].dev_num;
	pm_devs_t *pm_cur = ((part_entry_t *)arg)[opt].dev_ptr;

	switch (((part_entry_t *)arg)[opt].type) {
		case PM_DISK_T:
			if (pm_cur->blocked)
				dev_status = msg_string(MSG_pmblocked);
			else if (! pm_cur->unsaved)
				dev_status = msg_string(MSG_pmunchanged);
			else if (pm_cur->bootable)
				dev_status = msg_string(MSG_pmsetboot);
			else
				dev_status = msg_string(MSG_pmused);
			wprintw(m->mw, "%-33.32s %-22.21s %12.12s",
				pm_cur->diskdev_descr,
				pm_cur->bsddiskname,
				dev_status);
			break;
		case PM_PART_T:
			snprintf(buf, STRSIZE, "%s%c: %s %s",
				pm_cur->diskdev,
				'a' + part_num,
				pm_cur->bsdlabel[part_num].pi_mount,
				(strlen(pm_cur->bsdlabel[part_num].mounted) > 0) ? msg_string(MSG_pmmounted) : 
					(part_num == PART_B ||
						strlen (pm_cur->bsdlabel[part_num].pi_mount ) < 1 ||
						pm_cur->bsdlabel[part_num].pi_flags & PIF_MOUNT) ?
						"" : msg_string(MSG_pmunused));
			wprintw(m->mw, "   %-30.29s %-22.21s %11uM",
				buf,
				getfslabelname(pm_cur->bsdlabel[part_num].pi_fstype),
				pm_cur->bsdlabel[part_num].pi_size / (MEG / pm_cur->sectorsize));
			break;
		case PM_VND_T:
			pm_vnd_menufmt(m, opt, arg);
			break;
	}
	return;
}

/* Update partman main menu with devices list */
static int
pm_upddevlist(menudesc *m, void *arg)
{
	int i = 0, ii;
	pm_devs_t *pm_i;

	if (arg != NULL)
		((part_entry_t *)arg)[0].retvalue = m->cursel + 1;

	changed = 0;
	/* Mark all devices as not found */
	SLIST_FOREACH(pm_i, &pm_head, l)
		if (pm_i->found > 0)
			pm_i->found = 0;
	/* Detect all present devices */
	(void)find_disks("partman");
	pm_clean();

	if (m == NULL || arg == NULL)
		return -1;

	SLIST_FOREACH(pm_i, &pm_head, l) {
		m->opts[i].opt_name = NULL;
		m->opts[i].opt_action = pm_submenu;
		((part_entry_t *)arg)[i].dev_ptr = pm_i;
		((part_entry_t *)arg)[i].dev_num = -1;
		{
			((part_entry_t *)arg)[i].type = PM_DISK_T;
			for (ii = 0; ii < MAXPARTITIONS; ii++)
				if (pm_i->bsdlabel[ii].pi_fstype != FS_UNUSED) {
					i++;
					m->opts[i].opt_name = NULL;
					m->opts[i].opt_action = pm_submenu;
					((part_entry_t *)arg)[i].dev_ptr = pm_i;
					((part_entry_t *)arg)[i].dev_num = ii;
					((part_entry_t *)arg)[i].type = PM_PART_T;
				}
		}
		i++;
	}

	m->opts[i++] = (struct menu_ent) {
		.opt_name = MSG_updpmlist,
		.opt_action = pm_upddevlist,
	};
	m->opts[i  ] = (struct menu_ent) {
		.opt_name = MSG_savepm,
		.opt_action = pm_commit,
	};
	for (ii = 0; ii <= i; ii++) {
		m->opts[ii].opt_menu = OPT_NOMENU;
		m->opts[ii].opt_flags = OPT_EXIT;
	}

	if (((part_entry_t *)arg)[0].retvalue >= 0)
		m->cursel = ((part_entry_t *)arg)[0].retvalue - 1;
	return i;
}

static void
pm_menuin(menudesc *m, void *arg)
{
	if (cursel > m->numopts)
		m->cursel = m->numopts;
	else if (cursel < 0)
		m->cursel = 0;
	else
		m->cursel = cursel;
}

static void
pm_menuout(menudesc *m, void *arg)
{
	cursel = m->cursel;
}

/* initialize have_* variables */
void
check_available_binaries()
{
	static int did_test = false;

	if (did_test) return;
	did_test = 1;

	have_vnd = binary_available("vnconfig");
	have_dk = binary_available("dkctl");
}

/* Main partman function */
int
partman(void)
{
	int menu_no, menu_num_entries;
	static int firstrun = 1;
	menu_ent menu_entries[MAX_ENTRIES+6];
	part_entry_t args[MAX_ENTRIES];

	if (firstrun) {
		check_available_binaries();

		vnds_t_info = (structinfo_t) {
			.max = MAX_VND,
			.entry_size = sizeof vnds[0],
			.entry_first = &vnds[0],
			.entry_enabled = &(vnds[0].enabled),
			.entry_blocked = &(vnds[0].blocked),
			.entry_node = &(vnds[0].node),
		};

		memset(&vnds, 0, sizeof vnds);
		cursel = 0;
		changed = 0;
		firstrun = 0;
	}

	do {
		menu_num_entries = pm_upddevlist(&(menudesc){.opts = menu_entries}, args);
		menu_no = new_menu(MSG_partman_header,
			menu_entries, menu_num_entries+1, 1, 1, 0, 75, /* Fixed width */
			MC_ALWAYS_SCROLL | MC_NOBOX | MC_NOCLEAR,
			pm_menuin, pm_menufmt, pm_menuout, NULL, MSG_finishpm);
		if (menu_no == -1)
			args[0].retvalue = -1;
		else {
			args[0].retvalue = 0;
			clear();
			refresh();
			process_menu(menu_no, &args);
			free_menu(menu_no);
		}

		if (args[0].retvalue == 0) {
			if (pm_needsave())
				pm_commit(NULL, NULL);
			if (pm_mountall() != 0 ||
				make_fstab() != 0 ||
				pm_lastcheck() != 0 ||
				pm_savebootsector() != 0) {
					msg_display(MSG_wannatry);
					args[0].retvalue = (ask_yesno(NULL)) ? 1:-1;
			}
		}
	} while (args[0].retvalue > 0);
	
	/* retvalue <0 - error, retvalue ==0 - user quits, retvalue >0 - all ok */
	return (args[0].retvalue >= 0)?0:-1;
}
