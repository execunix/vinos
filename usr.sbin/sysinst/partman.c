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

/* XXX: replace all MAX_* defines with vars that depend on kernel settings */
#define MAX_ENTRIES 96

typedef struct pm_upddevlist_adv_t {
	const char *create_msg;
	int pe_type;
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

part_entry_t pm_dev_list(int);
static int pm_upddevlist(menudesc *, void *);
static void pm_select(pm_devs_t *);

static void
pm_getdevstring(char *buf, int len, pm_devs_t *pm_cur, int num)
{
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

/***
 Partman generic functions 
 ***/

/* Detect that partition is in use */
int
pm_partusage(pm_devs_t *pm_cur, int part_num, int do_del)
{
	int i, retvalue = 0;

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
	int error = 0;
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

/* Main partman function */
int
partman(void)
{
	int menu_no, menu_num_entries;
	static int firstrun = 1;
	menu_ent menu_entries[MAX_ENTRIES+6];
	part_entry_t args[MAX_ENTRIES];

	if (firstrun) {
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
