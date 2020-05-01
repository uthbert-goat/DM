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
 ***************************************************************************/


static const char tables_h_rcsid[] = "$Id: tables.h,v 1.16 2004/10/19 00:11:31 maelstrom Exp $";

struct flag_type
{
   char *name;
   int bit;
   bool settable;
};

struct association_type
{
   char *name;
   int  type;
};

struct skill_list_type
{
   char*    name;
   int      level;
   int      percent;
   bool     enabled;
   int      alignment;
   int      min_induct;
};

typedef struct skill_list_type SKILL_LIST_TYPE;

struct house_type
{
   char*    name;
   char*    who_name;
   char*    who_list_name;
   char*    item_name;
   bool     independent; /* true for loners */
   int      vnum_key;
   int      vnum_altar;
   int      vnum_guardian;
   int      vnum_item;
   int      vnum_statue;
   SKILL_LIST_TYPE  skills[99];
   char*    greeting_self;
   char*    greeting_other;
   char*    farewell_self;
   char*    farewell_other;
   char*    raid_ht;
   char*    guardian_greeting;
   char*    greeting_emote;
   char*    raid_say;
   char*    item_exists_enemy_say;
   char*    item_exists_ally_say;
   char*    item_exists_house_say;
   char*    give_enemy_say;
   char*    give_ally_say;
   char*    give_house_say;
   char*    balance_name;
   char*    morale_name;
   bool     powers_on;
   int      min_induct;
   int      max_induct;
   char*    induct_note_to;
   bool     induct_on;
   bool     imm_tag_who;
   bool     leader_tag_who;
   bool     player_tag_who;
   ROSTER*  proster;
   int      restrict_r_bit;
   int      restrict_e2_bit;
};

struct position_type
{
   char*    name;
   char*    short_name;
};

struct sex_type
{
   char*    name;
};

struct search_type
{
   char*    name;
   int      type;
   long     scope;
};

struct flag_info
{
   char*             name;
   const FLAG_TYPE*  flag;
   const FLAG_TYPE*  flag2;
   int               flag_location;
   long              scope;
};


/* game tables */
/* House_table not const so we can do rosters */
extern               struct   house_type        house_table[MAX_HOUSE];
extern                        SKILL_LIST_TYPE   old_house_skills[99];
extern      const    struct   position_type     position_table[];
extern      const    struct   sex_type          sex_table[];

/* flag tables */
extern      const    FLAG_TYPE                  act_flags[];
extern      const    FLAG_TYPE                  plr_flags[];
extern      const    FLAG_TYPE                  plr2_flags[];
extern      const    FLAG_TYPE                  affect_flags[];
extern      const    FLAG_TYPE                  affect2_flags[];
extern      const    FLAG_TYPE                  off_flags[];
extern      const    FLAG_TYPE                  imm_flags[];
extern      const    FLAG_TYPE                  form_flags[];
extern      const    FLAG_TYPE                  part_flags[];
extern      const    FLAG_TYPE                  comm_flags[];
extern      const    FLAG_TYPE                  comm2_flags[];
extern      const    FLAG_TYPE                  mount_flags[];
extern      const    FLAG_TYPE                  extra_flags[];
extern      const    FLAG_TYPE                  extra2_flags[];
extern      const    FLAG_TYPE                  act_flags[];
extern      const    FLAG_TYPE                  act2_flags[];
extern      const    FLAG_TYPE                  act_pc_flags[];
extern      const    FLAG_TYPE                  act2_pc_flags[];
extern      const    FLAG_TYPE                  restrict_flags[];
extern      const    FLAG_TYPE                  wear_flags[];
extern      const    FLAG_TYPE                  weapon_flags[];
extern      const    FLAG_TYPE                  container_flags[];
extern      const    FLAG_TYPE                  portal_flags[];
extern      const    FLAG_TYPE                  room_flags[];
extern      const    FLAG_TYPE                  extra_room_flags[];
extern      const    FLAG_TYPE                  exit_flags[];
extern      const    FLAG_TYPE                  mprog_flags[];
extern      const    char*                      wear_slot_names[];
extern      const    ASSOCIATION_TYPE           wear_slot_table[];
extern      const    struct   search_type       search_type_table[];
extern      const    ASSOCIATION_TYPE           affect_type_table[];
extern      const    struct   flag_info         flag_type_table[];
extern      const    ASSOCIATION_TYPE           damage_class_table[];
