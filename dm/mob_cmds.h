/* ex: set expandtab ts=3:                                                 */
/* -*- Mode: C; tab-width:3 -*-                                            */
/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik Staerfeldt, Tom Madsen, and Katja Nyboe.  *
 *                                                                         *
 *  Merc Diku Mud improvements copyright (C) 1992, 1993 by Michael         *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************
 *     ROM 2.4 is copyright 1993-1996 Russ Taylor                          *
 *     ROM has been brought to you by the ROM consortium                   *
 *         Russ Taylor (rtaylor@pacinfo.com)                               *
 *         Gabrielle Taylor (gtaylor@pacinfo.com)                          *
 *         Brian Moore (rom@rom.efn.org)                                   *
 *     By using this code, you have agreed to follow the terms of the      *
 *     ROM license, in the file Rom24/doc/rom.license                      *
 ***************************************************************************
 *  Based on MERC 2.2 MOBprograms by N'Atas-ha.                            *
 *  Written and adapted to ROM 2.4 by                                      *
 *          Markku Nylander (markku.nylander@uta.fi)                       *
 ***************************************************************************/

/***************************************************************************



 ***************************************************************************/

static const char mod_cmds_h_rcsid[] = "$Id: mob_cmds.h,v 1.9 2004/08/30 16:23:43 xurinos Exp $";



struct    mob_cmd_type
{
    char * const    name;
    DO_FUN *        do_fun;
};

/* the command table itself */
extern    const    struct    mob_cmd_type    mob_cmd_table    [];

/*
 * Command functions.
 * Defined in mob_cmds.c
 */
DECLARE_DO_FUN(do_mpasound);
DECLARE_DO_FUN(do_mpgecho);
DECLARE_DO_FUN(do_mpzecho);
DECLARE_DO_FUN(do_mpkill);
DECLARE_DO_FUN(do_mpassist);
DECLARE_DO_FUN(do_mpjunk);
DECLARE_DO_FUN(do_mpechoaround);
DECLARE_DO_FUN(do_mpecho);
DECLARE_DO_FUN(do_mpechoat);
DECLARE_DO_FUN(do_mpmload);
DECLARE_DO_FUN(do_mpoload);
DECLARE_DO_FUN(do_mppurge);
DECLARE_DO_FUN(do_mpgoto);
DECLARE_DO_FUN(do_mpat);
DECLARE_DO_FUN(do_mptransfer);
DECLARE_DO_FUN(do_mpgtransfer);
DECLARE_DO_FUN(do_mpforce);
DECLARE_DO_FUN(do_mpgforce);
DECLARE_DO_FUN(do_mpvforce);
DECLARE_DO_FUN(do_mpcast);
DECLARE_DO_FUN(do_mpskillcast);
DECLARE_DO_FUN(do_mpdamage);
DECLARE_DO_FUN(do_mpremember);
DECLARE_DO_FUN(do_mpforget);
DECLARE_DO_FUN(do_mpdelay);
DECLARE_DO_FUN(do_mpcancel);
DECLARE_DO_FUN(do_mpcall);
DECLARE_DO_FUN(do_mpflee);
DECLARE_DO_FUN(do_mpotransfer);
DECLARE_DO_FUN(do_mpremove);
DECLARE_DO_FUN(do_mpwait);
DECLARE_DO_FUN(do_mppeace);
DECLARE_DO_FUN(do_mpmod_skill);
DECLARE_DO_FUN(do_peace);
DECLARE_DO_FUN(do_mpsetquest);
DECLARE_DO_FUN(do_mpremovequest);
DECLARE_DO_FUN(do_mptimestamp);
DECLARE_DO_FUN(do_mpreward_gold);
DECLARE_DO_FUN(do_mpreward_xp);
DECLARE_DO_FUN(do_mpstring);
