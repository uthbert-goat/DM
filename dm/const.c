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

/***************************************************************************



 ***************************************************************************/

static const char rcsid[] = "$Id: const.c,v 1.259 2004/11/25 06:37:40 fizzfaldt Exp $";

#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include "merc.h"
#include "magic.h"
#include "marauder.h"
#include "interp.h"
#include "subskill.h"

/* animal table for beastial stances & animal call & beastial command */
const struct animal_type         animal_table    []       =
{
   /*
      char *name;
      int  vnum;
      int  apply_where;
      int  apply_location;
      int  apply_modifier;
      int  apply_bitvector;
      int  level;
   */
#if defined(MSDOS)
   {  "", 0, 0, 0, 0, 0, 0, 0, 0 },
#else
   {  "eagle", 2939, TO_CHAR, APPLY_DEX, 4, AFF_FLYING, 25, 0, 0 },
   {  "crocodile", 2940, TO_CHAR, APPLY_DAMROLL, 5, 0, 35, 0, 0 },
   {  "lion", 2909, TO_CHAR, APPLY_STR, 10, 0, 35, 0, 0 },
   {  "spider", 2963, TO_CHAR, APPLY_STR, 10, 0, 35, DARK_ONLY, 0 },
   {  "wolf", 2907, TO_CHAR, APPLY_DEX, 6, 0, 25, 0, 0 },
   {  "bear", 2908, TO_CHAR, APPLY_HIT, 60, 0, 30, 0, 0 },
   {  "monkey", 2941, TO_CHAR, APPLY_DAMROLL, 4, 0, 20, 0, 0 },
   {  "rhino", 2942, TO_CHAR, APPLY_HIT, 100, 0, 45, 0, 0 },
   {  "gorilla", 2943, TO_CHAR, APPLY_DAMROLL, 6, 0, 40, 0, 0 },
   {  "panther", 2945, TO_CHAR, APPLY_DEX, 5, AFF_HASTE, 45, 0, 0 },
   {  "unicorn", 2944, TO_CHAR, APPLY_SAVES, -5, AFF_DETECT_INVIS, 40, 0, 0 },
   {  "python", 2946, TO_CHAR, APPLY_DAMROLL, 5, 0, 25, 0, 0 },
   {  "rat", 2947, TO_CHAR, APPLY_STR, 1, AFF_SNEAK, 10, 0, 0 },
   {  "cobra", 2948, TO_CHAR, APPLY_HITROLL, 10, 0, 30, 0, 0 },
   {  "elephant", 2949, TO_CHAR, APPLY_DAMROLL, 15, AFF_SLOW, 40, 0, 0 },
   {  "tiger", 2950, TO_CHAR, APPLY_DAMROLL, 12, 0, 40, 0, 0 },
   {  "falcon", 2906, TO_CHAR, APPLY_MANA, 50, 0, 10, 0, 0 },
   {  "boar", 2951, TO_CHAR, APPLY_AC, -35, 0, 25, 0, 0 },
   {  "parrot", 2952, TO_CHAR, APPLY_INT, 5, 0, 30, 0, 0 },
   {  "viper", 2953, TO_CHAR, APPLY_DEX, 3, 0, 20, 0, 0 },
   {  "golden", 2954, TO_CHAR, APPLY_AC, -50, AFF_PROTECT_GOOD|AFF_PROTECT_EVIL, 51, 0, 0 },
   {  "hounds", 2955, TO_CHAR, APPLY_CON, 5, 0, 30, 0, 0 },
#endif
   {  NULL, 0, 0, 0, 0, 0, 0, 0, 0 }
};

/*
This is the table used to check if strings
contain vulgar content.
add any vulgar words you want
boolean is substring search
TRUE means substring, FALSE
means whole word only
*/
struct vulgar_type vulgar_table [] =
{
   /* Anywhere */
   {"fuck",       TRUE},
   {"porn",       TRUE},
   {"bastard",    TRUE},
   {"cunt",       TRUE},
   {"bitch",      TRUE},
   {"asshole",    TRUE},
   {"faggot",     TRUE},
   {"shitty",     TRUE},
   {"shittiest",  TRUE},
   {"shithead",   TRUE},
   {"bullshit",   TRUE},
   {"shitting",   TRUE},
   {"shite",      TRUE},
   {"whore",      TRUE},
   /* Whole word only */
   {"cunt",       FALSE},
   {"ass",        FALSE},
   {"anal",       FALSE},
   {"fag",        FALSE},
   {"fags",       FALSE},
   {"gay",        FALSE},
   {"gays",       FALSE},
   {"shit",       FALSE},
   {"cock",       FALSE},
   {"pussy",      FALSE},
   {"pussies",    FALSE},
   {"dick",       FALSE},
   {"dicks",      FALSE},
   {"dickhead",   FALSE},
   /* Terminator */
   {NULL,         FALSE},
};

/*
This is the table used to check if strings
contain possible cybersex, or other content
we want to look for. (i.e. 'cheat')
add any cybersex words you want
boolean is substring search
TRUE means substring, FALSE
means whole word only

Vulgar table and cyberTable should
be mutually exclusive.
*/
struct vulgar_type cyber_table [] =
{
   /* Anywhere */
   {"molest",     TRUE},
   {"cheat",      TRUE},
   {"licks your", TRUE},
   {"feels your", TRUE},
   {"licks",      TRUE},
   {"stroke",     TRUE},
   {"fondle",     TRUE},
   {"squeeze",    TRUE},
   {"suck",       TRUE},
   /* Whole word only */
   {"rub",        FALSE},
   {"rubs",       FALSE},
   {"rubbed",     FALSE},
   {"rubbing",    FALSE},
   /* Terminator */
   {NULL,         FALSE},
};

/* war paint table */
const struct warpaint_type       warpaint_table  []       =
{
   /*
   {  "paint name", apply_where, apply_location, apply_mod, apply_bits, level},
   */
#if defined(MSDOS)
   {  "", 0, 0, 0, 0, 0 },
#else
   {  "earth", TO_WARPAINT, 0, 1, 0, 15 },
   {  "earth", TO_CHAR, APPLY_AC, -50, 0, 15 },
   {  "wind", TO_WARPAINT, 0, 2, 0, 25 },
   {  "wind",  TO_CHAR, APPLY_DEX, 10, 0, 25},
   {  "wind",  TO_CHAR, APPLY_SAVES, -5, 0, 25},
   {  "water", TO_WARPAINT, 0, 3, 0, 35 },
   {  "water", TO_CHAR, APPLY_HIT, 50, 0, 35},
   {  "sun", TO_WARPAINT, 0, 4, 0, 45 },
   {  "sun",   TO_CHAR, APPLY_DAMROLL, 5, 0, 45},
   {  "bear", TO_WARPAINT, 0, 5, 0, 20 },
   {  "bear",  TO_CHAR, APPLY_STR, 3, 0, 20},
   {  "bear",  TO_CHAR, APPLY_DAMROLL, 2, 0, 20},
   {  "eagle", TO_WARPAINT, 0, 6, 0, 25 },
   {  "eagle", TO_CHAR, APPLY_HITROLL, 5, 0, 25},
   {  "crocodile", TO_WARPAINT, 0, 7, 0, 25 },
   {  "crocodile", TO_CHAR, APPLY_DAMROLL, 3, 0, 25},
   {  "lion", TO_WARPAINT, 0, 8, 0, 30 },
   {  "lion",  TO_CHAR, APPLY_DAMROLL, 5, 0, 30},
   {  "lion",  TO_CHAR, APPLY_DEX, 3, 0, 30},
   {  "lion",  TO_CHAR, APPLY_STR, 3, 0, 30},
   {  "falcon", TO_WARPAINT, 0, 9, 0, 10 },
   {  "falcon", TO_CHAR, APPLY_DEX, 1, 0, 10},
   {  "monkey", TO_WARPAINT, 0, 10, 0, 35 },
   {  "monkey", TO_CHAR, APPLY_DEX, 8, 0, 35},
   {  "monkey", TO_CHAR, APPLY_SAVES, -3, 0, 35},
   {  "rhino", TO_WARPAINT, 0, 11, 0, 40 },
   {  "rhino", TO_CHAR, APPLY_DAMROLL, 5, 0, 40},
   {  "rhino", TO_CHAR, APPLY_STR, 5, 0, 40},
   {  "rhino", TO_CHAR, APPLY_HIT, 50, 0, 40},
   {  "gorilla", TO_WARPAINT, 0, 12, 0, 45 },
   {  "gorilla", TO_CHAR, APPLY_STR, 2, 0, 45},
   {  "gorilla", TO_CHAR, APPLY_DAMROLL, 4, 0, 45},
   {  "gorilla", TO_CHAR, APPLY_HITROLL, 4, 0, 45},
   {  "unicorn", TO_WARPAINT, 0, 13, 0, 45 },
   {  "unicorn", TO_CHAR, APPLY_SAVES, -10, 0, 45},
   {  "unicorn", TO_CHAR, APPLY_WIS, 5, 0, 45},
   {  "unicorn", TO_CHAR, APPLY_INT, 5, 0, 45},
   {  "panther", TO_WARPAINT, 0, 14, 0, 50 },
   {  "panther", TO_CHAR, APPLY_DEX, 10, 0, 50},
   {  "panther", TO_CHAR, APPLY_STR, 10, 0, 50},
   {  "rat", TO_WARPAINT, 0, 15, 0, 10 },
   {  "rat", TO_CHAR, APPLY_CON, 1, 0, 10},
   {  "cobra", TO_WARPAINT, 0, 16, 0, 30 },
   {  "elephant", TO_WARPAINT, 0, 17, 0, 30 },
   {  "elephant", TO_CHAR, APPLY_STR, 10, 0, 30},
   {  "tiger", TO_WARPAINT, 0, 18, 0, 30 },
   {  "tiger", TO_CHAR, APPLY_DAMROLL, 5, 0, 30},
   {  "boar", TO_WARPAINT, 0, 19, 0, 15 },
   {  "boar",  TO_CHAR, APPLY_HIT, 20, 0, 15},
   {  "parrot", TO_WARPAINT, 0, 20, 0, 30 },
   {  "parrot", TO_CHAR, APPLY_MANA, 30, 0, 30},
   {  "viper", TO_WARPAINT, 0, 21, 0, 20 },
   {  "viper", TO_CHAR, APPLY_INT, 2, 0, 20},
   {  "golden unicorn", TO_WARPAINT, 0, 22, 0, 50 },
   {  "golden unicorn", TO_CHAR, APPLY_INT, 10, 0, 50},
   {  "golden unicorn", TO_CHAR, APPLY_WIS, 10, 0, 50},
   {  "golden unicorn", TO_CHAR, APPLY_DAMROLL, 2, 0, 50},
   {  "wolf", TO_WARPAINT, 0 , 23, 0, 20},
   {  "wolf", TO_CHAR, APPLY_DEX, 3, 0, 20},
   {  "wolf", TO_CHAR, APPLY_MANA, 30, 0, 20},
   {  "python", TO_WARPAINT, 0, 24, 0, 25},
   {  "python", TO_CHAR, APPLY_DEX, 2, 0, 25},
   {  "python", TO_CHAR, APPLY_DAMROLL, 2, 0, 25},
   {  "python", TO_CHAR, APPLY_HITROLL, 2, 0, 25},
#endif
   {  NULL,     0,       0,             0, 0, 0 }
};


/* forest table */
const struct forest_type         forest_table    []       =
{
#if defined(MSDOS)
   {
      "", 0, 0,
      {0, 0, 0, 0, 0, 0, 0}, /* herbs */
      {0, 0, 0, 0, 0, 0, 0, 0}, /* foods */
      {0, 0, 0, 0, 0, 0, 0, 0}, /* woods */
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*animals */
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
   },
#else
   {
      "default", 0, 0,
      {50, 50, 50, 50, 12, 4, 0},
      {100, 50, 50, 0, 5, 10, 30, 50},
      {0, 75, 3, 75, 50, 30, 20, 0},
      {100, 0, 50, 50, 100, 75, 0, 0, 0, 0, 0, 0,
      100, 0, 0, 0, 100, 50, 0, 20, 0, 100}
   },
   {
      "River Forest", 22100, 22299,
      {70, 10, 5, 40, 10, 7, 0},
      {50, 50, 50, -1, 50, 50, 50, 50},
      {50, 50, 50, 100, 100, 50, 50, 50},
      {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
   },
   {
      "Haon Dor", 6000, 6199,
      {-1, -1, -1, 40, -1, 5, 2},
      {-1, -1, 50, -1, -1, -1, 50, -1},
      {-1, 50, -1, 100, 100, -1, -1, -1},
      {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
   },
   {
      "Emerald Forest", 201, 299,
      {60, 10, 80, 80, 20, 7, 0},
      {100, 100, 100, 0, 30, 10, 100, 100},
      {20, 40, 20, 50, 70, 40, 50, 10},
      {100, 0, 100, 100, 100, 100, 0, 0, 0, 0, 20, 0,
      100, 0, 0, 5, 100, 100, 0, 0, 0, 100}
   },
   {
      "Holy Grove", 8900, 8999,
      {70, -1, -1, -1, -1, -1},
      {-1, -1, -1, -1, -1, -1, -1, -1},
      {-1, -1, -1, -1, -1, -1, -1, 30},
      {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
   },
   {
      "Southern Jungle", 29900, 29999,
      {30, 30, 50, 50, 10, 30, 30},
      {-1, 100, -1, 100, 100, 100, -1, -1},
      {100, 0, 0, 0, 0, 0, 20, 1},
      {-1, 20, 100, 100, 100, 100, 100, 20, 30, 1, 50, 20,
      100, 20, 50, 50, 100, 50, 100, 20, 1, 100}
   },
   {
      "Panther Clan", 19100, 19249,
      {-1, -1, -1, -1, 50, -1, -1},
      {-1, -1, -1, -1, -1, -1, -1, -1},
      {-1, -1, -1, -1, -1, -1, -1, -1},
      {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 100, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
   },
   {
      "Seasonal Forest", 15700, 15899,
      {75, 70, 70, 70, 20, 14, 0},
      {100, 100, 100, 0, 100, 100, 100, 100},
      {0, 100, 50, 30, 30, 30, 30, 30},
      {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
   },
   {
      "Magic Forest", 1800, 1899,
      { 60, 30, 30, 30, 30, 10, 0},
      { 100, 30, 40, 0, 40, 60, 20, 70},
      { 0, 100, 20, 75, 80, 40, 10, 100},
      { 50, 0, 40, 40, 40, 0, 0, 0, 0, 100, 0, 0,
      20, 0, 0, 0, 80, 30, 0, 10, 50, 10}
   },
   {
      "Magic Forest", 1821, 1824,
      { 60, 0, 0, 0, 0, 0, 0},
      { 0, 0, 0, 0, 0, 0, 0, 50},
      { 0, 0, 0, 0, 0, 0, 0, 0},
      { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
   },
   {
      "Magic Forest", 1829, 1832,
      { 60, 0, 0, 0, 0, 0, 0},
      { 0, 0, 0, 0, 0, 0, 0, 50},
      { 0, 0, 0, 0, 0, 0, 0, 0},
      { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
   },
   {
      "The swamps of Sin Nombre", 25300, 25350,
      {30, 30, 30, 30, -1, 1, 40},
      {-1, 0, 100, 0, 100, 10, 0, 100},
      {0, 0, 0, 40, 0, 40, 100, 10},
      {-1, 100, 40, 40, -1, 40, -1, -1, -1, -1, 0, 100,
      100, 30, -1, -1, -1, 100, -1, 50, -1, -1}
   },
   {
      "The Dead Forest", 24300, 24399,
      {50, 20, 50, 50, 10, 12, 0},
      {100, 100, 40, 0, 20, 10, 20, 50},
      {0, 100, 25, 100, 100, 100, 20, 0},
      {-1, -1, -1, -1, -1, 100, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, 100, 100, -1, -1, -1, -1}
   },
   {
      "The Dying Forest", 24300, 24399,
      {80, 100, 100, 100, 30, 15, 0},
      {100, 100, 100, 0, 50, 20, 20, 100},
      {0, 100, 100, 100, 100, 100, 0, 0},
      {100, 0, 100, 100, 100, 100, 0, 0, 0, 0, 20, 20,
      100, 10, 0, 0, 100, 100, 0, 10, 0, 100}
   },
   {
      "Targoth Mountains", 10000, 10200,
      {50, 50, 50, 50, 40, 5, 0},
      {100, 100, 20, 0, 10, 5, 100, 100},
      {30, 100, 50, 50, 100, 100, 0, 5},
      {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
   },
   {
      "Elvenhame Park", 28054, 28099,
      {40, 60, 20, 50, 15, 10, 60},
      {100, 100, 20, 0, 10, 5, 0, 100},
      {4, 10, 10, 100, 100, 50, 100, 10},
      {50, 0, 0, 0, 30, 10, 0, 0, 0, 10, 50, 50,
      70, 40, 0, 0, 70, 15, 0, 50, 70, 100}
   },
   {
      "City of the Pure", 20400, 20422, /*forest area*/
      {50, 50, 50, 50, 0, 0, 0}, /* herbs */
      {100, 80, 100, 0, 100, 50, 30, 30}, /* foods */
      {0, 75, 75, 75, 75, 75, 20, 0}, /* woods */
      {100, 0, 20, 20, 50, 0, 0, 0, 0, 0, 30, 40, /*animals */
      50, 10, 0, 0, 100, 70, 0, 20, 30, 100}
   },

   {
      "City of the Pure", 20436, 20444, /*herb garden area*/
      {100, 100, 100, 100, 0, 0, 0}, /* herbs */
      {0, 0, 0, 0, 20, 0, 0, 0}, /* foods */
      {0, 0, 0, 0, 0, 0, 0, 0}, /* woods */
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*animals */
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
   },
   {
      "City of the Pure", 20423, 20430, /*small pond area*/
      {0, 0, 0, 0, 0, 80, 80}, /* herbs */
      {20, 0, 80, 0, 60, 0, 50, 50}, /* foods */
      {0, 0, 0, 0, 0, 0, 0, 0}, /* woods */
      {0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 50, /*animals */
      0, 20, 0, 0, 30, 0, 0, 30, 0, 0}
   },
#endif
   {
      NULL, 0, 0,
      {0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
   }
};


/*tinker table
notes:
*For weapon flags, put a 100 for weapon_modifier and the flag you want for weapon modifier
*If you make something weapon only or armor only, and you put a modifier on both, it will
put both onto the one item it works with.(weapon flags still have to be in weapon modifier)
*{ "name" , need_ore, use_weapon, use_armor, weapon_location, weapon_modifier, armor_location,
armor_modifier, rbonus_resistance, forgable, forge_location, forge_modifier},
*/
const struct tinker_type      tinker_table  []      =
{

#if defined(MSDOS)
   { "", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#else
   { " ", 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
   { "poison_vial" , 1, 1, 0, 100, WEAPON_POISON, 0, 0, 0, 0, 0, 0},
   { "oil_vial", 1, 1, 0, 100, WEAPON_FLAMING, 0, 0, 0, 0, 0, 0},
   { "mithril_alloy", 1, 1, 1, APPLY_SAVES, -3, APPLY_SAVES, -2, RES_POISON, 1, 0, 0},
   { "brass_alloy", 1, 1, 1, APPLY_DEX, 4, APPLY_MOVE, 30, RES_MENTAL, 1, 0, 0},
   { "bronze_alloy", 1, 1, 1, APPLY_HITROLL, 5, APPLY_DEX, 3, RES_PIERCE, 1, 0, 0},
   { "silver_alloy", 1, 1, 1, APPLY_AC, -5, APPLY_HITROLL, 2, RES_ACID, 1, 0, 0},
   { "gold_alloy", 1, 1, 1, APPLY_STR, 5, APPLY_CON, 3, RES_FIRE, 1, 0, 0},
   { "steel_alloy", 1, 1, 1, APPLY_HIT, 10, APPLY_AC, -3, RES_SLASH, 1, APPLY_DAMROLL, 1},
   { "platinum_alloy", 1, 1, 1, APPLY_DAMROLL, 4, APPLY_HIT, 10, RES_BASH, 1, APPLY_DAMROLL, 2},
#endif
   { NULL,             0, 0, 0, 0,             0, 0,          0,        0, 0,             0, 0}

};



/* abyss demon table */
const struct demon_type         demon_table    []       =
{
   /*  format -has to be in the order of demon_type in merc.h
   but change as you see fit
   {
      "name",  "summoning name name",
      "short description",
      "long description",
      "look description",
      min level to summon,
      base dam roll, base hp, base ac, imm_flags, res_flags, vul_flags,
      "spell name", demon_id #,
      apply_where, apply_location, apply_mod, apply_bits
   },
   */
#if defined(MSDOS)
   {  "demon", "demon", "demon", "demon", "demon", 0, 0, 0, 0, 0, 0, 0,
   "spell", 0, 0, 0, 0, 0},
#else
   /* add demons here */
   {
      "dretch", "dretch", "a Dretch",
      "A squat, wicked Dretch stands here, bound to servitude.\n\r",
      "This is a dretch, one of the lesser demons of the Abyss. It has the\n\r"
      "form of a short, bloated man with spindly arms and it is incredibly\n\r"
      "stupid.  It has been bound to serve a mortal here in the Material\n\r"
      "Plane.\n\r",
      20, 20, 2000, -100, 0, 0, 0,
      "magic missile", DEMON_DRETCH, TO_OBJECT, APPLY_MANA, 100, 0
   },

   {
      "mezzoloth", "mezzoloth", "a Mezzoloth",
      "A vicious, plated Mezzoloth stands here, bound to servitude.\n\r",
      "This is a mezzoloth, one of the weakest yugoloths of the Abyss.\n\r"
      "It has the form of a seven-foot tall humanoid with insect-like\n\r"
      "features, vicious claws and chitinous plates. It is bound to the\n\r"
      "service of a mortal here in the Material Plane.\n\r",
      25, 30, 2500, -200, 0, 0, 0,
      "flamestrike", DEMON_WIMP, TO_OBJECT, APPLY_DAMROLL, 5, 0
   },

   {
      "tengu", "tengu", "a Tengu",
      "A birdlike Tengu stands here, bound to servitude.\n\r",
      "This tengu, like most of its demonic kindred of the same name, is\n\r"
      "largely covered in feathers and wielding winglike arms tipped with\n\r"
      "three-clawed appendages. Standing just over 5 feet tall, its beak\n\r"
      "appears as razor sharp as its claws. It seems to be bound to the\n\r"
      "service of a mortal here in the Material Plane.\n\r",
      30, 40, 3000, -250, 0, 0, 0,
      "curse", 0, TO_OBJECT, APPLY_AC, -40, 0
   },

   {
      "tengwar", "tengwar", "a Tengwar",
      "A large Tengwar looks for its next victim while bound to servitude.\n\r",
      "A larger and more bloodthirsty version of the tengu, ram horns\n\r"
      "menacingly protrude from its forehead. Feathers and matted fur\n\r"
      "protect its conjured hide while clawed hands drag near the\n\r"
      "ground from its 7 foot frame. It is bound to the service of a\n\r"
      "mortal here in the Material Plane.\n\r",
      35, 45, 3500, -300, 0, 0, 0,
      "fire and ice", DEMON_TENGWAR, TO_OBJECT, APPLY_SAVES, -10, 0
   },

   {
      "cthul", "cthul", "a Cthul",
      "A creeping Cthul stands ready to serve its bounded master.\n\r",
      "Mostly akin to a formless mound of decaying flesh, it is not\n\r"
      "undead. Averaging only about 5 feet in height, this is a Cthul.\n\r"
      "A shambling mass of iscous flesh and protruding bones, this\n\r"
      "demonic conjuration is bound to the service of a mortal here in\n\r"
      "the Material Plane.\n\r",
      40, 45, 4000, -300, 0, 0, 0,
      "chromatic shield", DEMON_CTHUL, TO_OBJECT, APPLY_HIT, 100, 0
   },

   {
      "cthulahdd", "cthulahdd", "a Cthulahdd",
      "A putrid Cthulahd stands here, bound to servitude.\n\r",
      "A massive mound of shattered bones and jagged shards of metal.\n\r"
      "Towering near to 9 feet tall, this being from the netheregions\n\r"
      "of the 9 Hells may look more hungry than intimidating. Then, a\n\r"
      "gaping, many-toothed maw opens and closes and you know this being's\n\r"
      "appetite cannot be sated by mere flesh. While ever-hungry, it stands\n\r"
      "bound to the service of a mortal here\n\rin the Material Plane.\n\r",
      45, 55, 4500, -320, 0, 0, 0,
      "syphon soul", 0, TO_OBJECT, APPLY_DAMROLL, 10, 0
   },

   {
      "balrog", "balrog", "a Balrog",
      "A titanic Balrog towers here, bound to demonic servitude.\n\r",
      "With massive bat-like wings and a 12 foot tall frame resemblant of\n\r"
      "an angry gorilla, this Balrog exudes evil. Large hands ending in\n\r"
      "pointed nails look ready to rend anything limb from limb. Bound in\n\r"
      "the service of a mortal here in the Material Plane,  the Balrog\n\r"
      "awaits its orders for his next commanded act of destruction.\n\r",
      50, 60, 5000, -350, 0, 0, 0,
      "windwall", DEMON_AGGRESSIVE, TO_OBJECT, APPLY_MANA, 200, 0
   },

   {
      "klahgaht", "klahgaht", "a Klahgaht",
      "An evil Klahgaht stands here, bound to servitude.\n\r",
      "Surrounded by a horrible stench, rivaling even that of a troll,\n\r"
      "this mass of sinew and muscle stands an impressive 15 feet in\n\r"
      "height. Almost seeming as though it's not quite in this plane at\n\r"
      "times, this Klahgaht is simply massive. pure manifestation of\n\r"
      "greed for blood and violence. A pair of sharpened teeth protrude\n\r"
      "from its lower jaw and roughened flesh-scales cover its\n\r"
      "musclebound hide. Through all its bloodlust, it is bound in the\n\r"
      "service of a mortal here in the Material Plane.\n\r",
      51, 65, 5300, -370, 0, 0, 0,
      "acid blast", 0, TO_AFFECTS, 0, 0, AFF_HASTE
   },

   {
      "griekul", "griekul", "a Griekul",
      "A misshapen Griekul stands here, bound to servitude.\n\r",
      "Wielding four powerfully built arms ending in knife-like growths,\n\r"
      "this high level demon means business...and death. Towering easily\n\r"
      "18 feet tall and covered in the tough hide of a troll, fleshy\n\r"
      "folds of skin drape down its back to resemble a cloak. Peering\n\r"
      "around its abnormal surroundings with piercing eyes that glow\n\r"
      "with a demonic red, this creature exudes an aura of pure hatred.\n\r"
      "Called a soulreaver in legend, this Griekul is bound to a being\n\r"
      "of the Material Plane.\n\r",
      55, 70, 6000, -400, 0, 0, 0,
      "spiritblade", 0, TO_OBJECT, APPLY_AC, -100, 0
   },

   {
      "dragkagh", "dragkagh", "a Dragkagh",
      "A mind-rending Dragkagh stands here, bound to servitude.\n\r",
      "A beastly cross between dragon and demon, this Dragkagh is often\n\r"
      "the master of its domain. Its imposing height of roughly 30 feet\n\r"
      "tall makes its dragon blood obvious to any observer. Gleeming\n\r"
      "fangs and the deadliest of claws astound the casual observer.\n\r"
      /*
      Shortened to fix a warning.  I think the desc looks better without
      this line anyway. -Fizzfaldt
      "Plated largely in the scales of a green dragon it rends the\n\r"
      "ground for lack of anything else to do."
      */
      "Its gaze would turn any but a God to a quivering mass of flesh.\n\r"
      "Bound to a powerful being of the Material Plane, this demon is not"
      "one with which to be trifled.\n\r",
      60, 80, 8000, -500, 0, 0, 0,
      "gas breath", DEMON_DRAGKAGH, TO_OBJECT, APPLY_DAMROLL, 20, 0
   },
#endif
   {  NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, NULL, 0, 0, 0, 0, 0 }
};

/* hometowns */
const struct hometown_type      hometown_table  []      =
{
   /*
   { "name", "abr", good?, neutral?, evil?, "race", temple_vnum, cost,
   creation?},
   */
#if defined(MSDOS)
   {"hometown",          "home", 1, 1, 1, NULL, 10681, 100, 1},
#else
   {"Glyndane",          "glyn", 1, 0, 0, NULL, 10670, 100, 1},
   {"Glyndane",          "glyn", 0, 1, 0, NULL, 10681, 100, 1},
   {"Glyndane",          "glyn", 0, 0, 1, NULL, 10699, 100, 1},
   {"Elvenhame",         "elv",  1, 0, 0, NULL, 8839,  100, 1},
   {"Elvenhame",         "elv",  0, 1, 0, NULL, 8844,  100, 1},
   {"Elvenhame",         "elv",  0, 0, 1, NULL, 8848,  100, 1},
   {"Dhaes Drae Asilon", "dda",  1, 0, 0, NULL, 27815, 250, 1},
/*   {"New Thalos",        "nt",   1, 1, 1, NULL, 9609,  125, 1}, */
   {"Tyr-Zinet",         "tz",   1, 1, 1, NULL, 70090, 125, 1},
   {"Ofcol",             "ofc",  1, 0, 0, NULL, 699,   200, 1},
   {"Arkham",            "ark",  0, 0, 1, NULL, 768,   200, 1},
   {"Sheqeston",         "sheq", 0, 0, 1, NULL, 6837,  250, 1},
   {"Ethshar",           "eth",  1, 1, 1, NULL, 3054,  200, 1},
   /* Daltigoth is only here for call of duty, is not a real hometown - FIZZ */
   {"Daltigoth",         "dal",  0, 0, 0, NULL, 8243,    0, 0},
#endif
   {NULL,                NULL,   0, 0, 0, NULL, 0,       0, 0}
};

/* item type list */
const struct item_type        item_table    []    =
{
   { ITEM_LIGHT,       "light"      },
   { ITEM_SCROLL,      "scroll"     },
   { ITEM_WAND,        "wand"       },
   { ITEM_STAFF,       "staff"      },
   { ITEM_WEAPON,      "weapon"     },
   { ITEM_NULL6,       "null6"      },
   { ITEM_NULL7,       "null7"      },
   { ITEM_TREASURE,    "treasure"   },
   { ITEM_ARMOR,       "armor"      },
   { ITEM_POTION,      "potion"     },
   { ITEM_CLOTHING,    "clothing"   },
   { ITEM_FURNITURE,   "furniture"  },
   { ITEM_TRASH,       "trash"      },
   { ITEM_CONTAINER,   "container"  },
   { ITEM_DRINK_CON,   "drink"      },
   { ITEM_KEY,         "key"        },
   { ITEM_FOOD,        "food"       },
   { ITEM_MONEY,       "money"      },
   { ITEM_BOAT,        "boat"       },
   { ITEM_CORPSE_NPC,  "npc_corpse" },
   { ITEM_CORPSE_PC,   "pc_corpse"  },
   { ITEM_FOUNTAIN,    "fountain"   },
   { ITEM_PILL,        "pill"       },
   { ITEM_PROTECT,     "protect"    },
   { ITEM_MAP,         "map"        },
   { ITEM_WARP_STONE,  "warp_stone" },
   { ITEM_ROOM_KEY,    "room_key"   },
   { ITEM_GEM,         "gem"        },
   { ITEM_JEWELRY,     "jewelry"    },
   { ITEM_JUKEBOX,     "jukebox"    },
   { ITEM_HOUSE_ITEM,  "house_item" },
   { ITEM_SKELETON,    "skeleton"   },
   { ITEM_PORTAL_NEW,  "portal"     },
   { ITEM_SPELLBOOK,   "spellbook"  },
   { ITEM_HERB,        "herb"       },
   { ITEM_BOOK,        "book"       },
   { ITEM_PEN,         "pen"        },
   { ITEM_INKWELL,     "inkwell"    },
   { ITEM_TOOL,        "tool"       },
   { 0,                NULL         }
};

const   struct    bless_weapon_type bless_weapon_type_names           []=
{
   {
      WEAPON_EXOTIC,
      "a %s named '%s'",
      "crystalline orb",
      "holy book",
      "Solitude",
      "Love",
      "Blinding Light",
      "crystal"
   },
   {
      WEAPON_SWORD,
      "a %s named '%s'",
      "longsword",
      "claymore",
      "Peacekeeper",
      "Lightbringer",
      "Shadowbane",
      "steel"
   },
   {
      WEAPON_DAGGER,
      "a %s named '%s'",
      "crystal shard",
      "wavy blade",
      "Mercy",
      "Compassion",
      "Vigilance",
      "crystal"
   },
   {
      WEAPON_SPEAR,
      "a %s named '%s'",
      "harpoon",
      "barbed spear",
      "Dedication",
      "Purity",
      "Divine Intervention",
      "steel"
   },
   {
      WEAPON_MACE,
      "a %s named '%s'",
      "truncheon",
      "mallet",
      "Innocence",
      "Holy Bastion",
      "Judgement",
      "steel"
   },
   {
      WEAPON_AXE,
      "a %s named '%s'",
      "handaxe",
      "wide bladed axe",
      "Split Allegiance",
      "Deliverance",
      "Silence",
      "steel"
   },
   {
      WEAPON_FLAIL,
      "a %s named '%s'",
      "flail",
      "morningstar",
      "Harmony",
      "Sanctity",
      "Piercing Truth",
      "steel"
   },
   {
      WEAPON_WHIP,
      "a %s named '%s'",
      "wrapped cord",
      "deadly whip",
      "Conscience",
      "Repentance",
      "Faith",
      "steel"
   },
   {
      WEAPON_POLEARM,
      "a %s named '%s'",
      "bowed halberd",
      "axe bladed halberd",
      "Tenacity",
      "Chilling Purity",
      "Retribution",
      "steel"
   },
   {
      WEAPON_STAFF,
      "a %s named '%s'",
      "quarterstaff",
      "long staff",
      "Calm Before the Storm",
      "Senseless Kindness",
      "Redemption",
      "steel"
   },
   {
      WEAPON_BOW,
      "a %s named '%s'",
      "shortbow",
      "longbow",
      "Hope",
      "Spiritual Release",
      "Glinting in the Shadows",
      "steel"
   },
   {
      WEAPON_ARROWS,
      "a %s of %s",
      "bundle",
      "bundle",
      "divine arrows",
      "divine arrows",
      "divine arrows",
      "steel"
   },
   {
      -1,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL
   },
};

const   struct   herb_spell_type herb_spells      []      =
{
   {
      &gsn_heal,
      "herbal remedy",
      "!herbal remedy!"
   },
   {
      &gsn_cure_blindness,
      "vision renewal",
      "!vision renewal!"
   },
   {
      &gsn_cure_disease,
      "antiseptic",
      "!antiseptic!"
   },
   {
      &gsn_cure_poison,
      "antitoxin",
      "!antitoxin!"
   },
   {
      &gsn_imbue_regeneration,
      "enhanced health",
      "You return to your normal rate of healing."
   },
   {
      &gsn_giant_strength,
      "adrenaline",
      "You feel yourself calming down; your blood stops rushing."
   },
   {
      &gsn_remove_curse,
      "consecration",
      "!consecration!"
   },
   {
      &gsn_herb,
      "curative herbs",
      "!curative herbs!"
   },
   {
      &gsn_waterbreathing,
      "gill enhancement",
      "You feel as though your skin has stopped absorbing oxygen from the water."
   },
   {
      &gsn_cancellation,
      "neutralize effects",
      "!neutralize effects!"
   },
   {
      &gsn_forage,
      NULL,
      NULL
   }
};

/* weapon selection table */
const    struct    weapon_type    weapon_table    []    =
{
   { "sword",   OBJ_VNUM_SCHOOL_SWORD,   WEAPON_SWORD,   &gsn_sword   },
   { "mace",    OBJ_VNUM_SCHOOL_MACE,    WEAPON_MACE,    &gsn_mace    },
   { "dagger",  OBJ_VNUM_SCHOOL_DAGGER,  WEAPON_DAGGER,  &gsn_dagger  },
   { "axe",     OBJ_VNUM_SCHOOL_AXE,     WEAPON_AXE,     &gsn_axe     },
   { "spear",   OBJ_VNUM_SCHOOL_STAFF,   WEAPON_SPEAR,   &gsn_spear   },
   { "flail",   OBJ_VNUM_SCHOOL_FLAIL,   WEAPON_FLAIL,   &gsn_flail   },
   { "whip",    OBJ_VNUM_SCHOOL_WHIP,    WEAPON_WHIP,    &gsn_whip    },
   { "polearm", OBJ_VNUM_SCHOOL_POLEARM, WEAPON_POLEARM, &gsn_polearm },
   { "staff",   OBJ_VNUM_SCHOOL_POLEARM, WEAPON_STAFF,   &gsn_staff   },
   { "bow",     OBJ_VNUM_SCHOOL_SWORD,   WEAPON_BOW,     &gsn_archery },
   { "arrows",  OBJ_VNUM_SCHOOL_SWORD,   WEAPON_ARROWS,  &gsn_archery },
   { NULL,      0,                       0,              NULL         }
};



/* wiznet table and prototype for future flag setting */
/*
 * Please note, this table is now ordered.  DO NOT change the order of table
 * elements without looking into what impact this could have!  Important places
 * to look would be in wiznet() and do_wiznet().  -Maelstrom
 */
const   struct wiznet_type      wiznet_table    []              =
{
   { "on",        WIZ_ON,        IM },
   { "prefix",    WIZ_PREFIX,    IM },
   { "ticks",     WIZ_TICKS,     IM },
   { "logins",    WIZ_LOGINS,    IM },
   { "sites",     WIZ_SITES,     L4 },
   { "links",     WIZ_LINKS,     L7 },
   { "newbies",   WIZ_NEWBIE,    IM },
   { "spam",      WIZ_SPAM,      L5 },
   { "deaths",    WIZ_DEATHS,    IM },
   { "resets",    WIZ_RESETS,    L4 },
   { "mobdeaths", WIZ_MOBDEATHS, L4 },
   { "flags",     WIZ_FLAGS,     L5 },
   { "penalties", WIZ_PENALTIES, L5 },
   { "saccing",   WIZ_SACCING,   L5 },
   { "levels",    WIZ_LEVELS,    IM },
   { "load",      WIZ_LOAD,      L2 },
   { "restore",   WIZ_RESTORE,   L2 },
   { "snoops",    WIZ_SNOOPS,    L2 },
   { "switches",  WIZ_SWITCHES,  L2 },
   { "secure",    WIZ_SECURE,    L1 },
   { "swearing",  WIZ_SWEAR,     IM },
   { "deletes",   WIZ_DELETES,   IM },
   { "transfers", WIZ_TRANSFERS, L7 },
   { "items",     WIZ_ITEMS,     L7 },
   { "marriages", WIZ_MARRY,     IM },
   { "worship",   WIZ_WORSHIP,   IM },
   { "bugs",      WIZ_BUGS,      IM },
   { "clans",     WIZ_CLANS,     IM },
   { "cyber",     WIZ_CYBER,     IM },
   { "name",      WIZ_NAME,      IM },
   { NULL,        0,             0  }
};

/* attack table  -- not very organized :( */
const     struct attack_type    attack_table    [MAX_DAMAGE_MESSAGE]    =
{
   {"none",      "hit",                -1},  /*  0 */
   {"slice",     "slice",              DAM_SLASH},
   {"stab",      "stab",               DAM_PIERCE},
   {"slash",     "slash",              DAM_SLASH},
   {"whip",      "whip",               DAM_SLASH},
   {"claw",      "claw",               DAM_SLASH},  /*  5 */
   {"blast",     "blast",              DAM_BASH},
   {"pound",     "pound",              DAM_BASH},
   {"crush",     "crush",              DAM_BASH},
   {"grep",      "grep",               DAM_SLASH},
   {"bite",      "bite",               DAM_PIERCE},  /* 10 */
   {"pierce",    "pierce",             DAM_PIERCE},
   {"suction",   "suction",            DAM_BASH},
   {"beating",   "beating",            DAM_BASH},
   {"digestion", "digestion",          DAM_ACID},
   {"charge",    "charge",             DAM_BASH},  /* 15 */
   {"slap",      "slap",               DAM_BASH},
   {"punch",     "punch",              DAM_BASH},
   {"wrath",     "wrath",              DAM_ENERGY},
   {"magic",     "magic",              DAM_ENERGY},
   {"divine",    "divine power",       DAM_HOLY},  /* 20 */
   {"cleave",    "cleave",             DAM_SLASH},
   {"scratch",   "scratch",            DAM_PIERCE},
   {"peck",      "peck",               DAM_PIERCE},
   {"peckb",     "peck",               DAM_BASH},
   {"chop",      "chop",               DAM_SLASH},  /* 25 */
   {"sting",     "sting",              DAM_PIERCE},
   {"smash",     "smash",              DAM_BASH},
   {"shbite",    "shocking bite",      DAM_LIGHTNING},
   {"flbite",    "flaming bite",       DAM_FIRE},
   {"frbite",    "freezing bite",      DAM_COLD},  /* 30 */
   {"acbite",    "acidic bite",        DAM_ACID},
   {"chomp",     "chomp",              DAM_PIERCE},
   {"drain",     "life drain",         DAM_NEGATIVE},
   {"thrust",    "thrust",             DAM_PIERCE},
   {"slime",     "slime",              DAM_ACID}, /* 35 */
   {"shock",     "shock",              DAM_LIGHTNING},
   {"thwack",    "thwack",             DAM_BASH},
   {"flame",     "flame",              DAM_FIRE},
   {"chill",     "chill",              DAM_COLD},
   {"light",     "blast of light",     DAM_LIGHT}, /* 40 */
   {"nature",    "force of nature",    DAM_NATURE},
   {"blare",     "blare",              DAM_SOUND},
   {"screech",   "screech",            DAM_SOUND},
   {"tone",      "tone",               DAM_SOUND},
   {"deluge",    "deluge",             DAM_DROWNING}, /* 45 */
   {"anguish",   "mental anguish",     DAM_MENTAL},
   {"arrow",     "arrow",              DAM_PIERCE},
   {"flarrow",   "flaming arrow",      DAM_FIRE},
   {"trample",   "trampling",          DAM_BASH},
   {"damnation", "cries of damnation", DAM_MENTAL}, /* 50 */
   {"lichpunch", "freezing punch",     DAM_COLD},  /* Lich Freezing Punch */
   {NULL,        NULL,                 0}
};

/* race table */
const     struct    race_type    race_table    []        =
{
   /*
   {
   name,        pc_race?,
   act bits,    aff_by bits,  aff_by2,   off bits,
   imm,        res,        vuln,
   form,        parts
   },
   */
   {
      "unique",       FALSE,
      0,       0,       0,          0,
      0,       0,          0,
      0,       0,
      FALSE,
      TRUE,
      &grn_unique
   },
   {
      "human",        TRUE,
      0,       0,         0,  0,
      0,         0,        0,
      A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K,
      FALSE,
      TRUE,
      &grn_human
   },
   {
      "dwarf",        TRUE,
      0,        AFF_INFRARED,    0, 0,
      0,        RES_POISON|RES_MAGIC, VULN_DROWNING,
      A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K,
      FALSE,
      TRUE,
      &grn_dwarf
   },
   {
      "elf",            TRUE,
      0,              AFF_SNEAK|AFF_INFRARED,   0, 0,
      0,        RES_CHARM|RES_FIRE,    VULN_IRON,
      A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K,
      FALSE,
      TRUE,
      &grn_elf
   },
   {
      "grey-elf",             TRUE,
      0,              AFF_SNEAK|AFF_INFRARED,   0,    0,
      0,              RES_CHARM,      VULN_IRON,
      A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K,
      FALSE,
      TRUE,
      &grn_grey_elf
   },
   {
      "dark-elf",             TRUE,
      0,              AFF_SNEAK|AFF_INFRARED,   0,    0,
      0,              RES_CHARM|RES_COLD,      VULN_IRON,
      A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K,
      FALSE,
      TRUE,
      &grn_dark_elf
   },
   {
      "centaur",                  TRUE,
      0,        0,        0,  0,
      0,              RES_CHARM|RES_BASH,      VULN_MENTAL,
      A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K,
      FALSE,
      TRUE,
      &grn_centaur
   },
   {
      "troll",                TRUE,
      0,        0,        0,  0,
      0,              RES_POISON|RES_DISEASE|RES_WEAPON,             VULN_ACID|VULN_FIRE,
      A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K,
      FALSE,
      TRUE,
      &grn_troll
   },
   {
      "giant",        TRUE,
      0,        0,        0,  0,
      0,        RES_WEAPON|RES_BASH,    VULN_MENTAL,
      A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K,
      FALSE,
      TRUE,
      &grn_giant
   },
   {
      "gnome",                TRUE,
      0,        AFF_INFRARED,        0,   0,
      0,              RES_MENTAL|RES_MAGIC,              VULN_BASH,
      A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K,
      FALSE,
      TRUE,
      &grn_gnome
   },
   {
      "draconian",                TRUE,
      0,              AFF_FLYING,   0, 0,
      0,              RES_SLASH|RES_DISEASE,   0,
      A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K,
      FALSE,
      TRUE,
      &grn_draconian
   },
   {
      "ethereal",                FALSE,
      0,        AFF_PASS_DOOR|AFF_FLYING|AFF_INFRARED,    0,   0,
      IMM_DISEASE|IMM_POISON,        RES_WEAPON|RES_MENTAL|RES_BASH,
      VULN_FIRE|VULN_LIGHTNING,
      A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K,
      FALSE,
      TRUE,
      &grn_ethereal
   },
   {
      "changeling", TRUE,
      0,    0,    0, 0,
      0,    0,    0,
      H|M|V,    A|B|V|D|E|F|G|H|I|J|K,
      FALSE,
      TRUE,
      &grn_changeling
   },
   {
      "illithid", TRUE,
      0,    AFF_FLYING|AFF_INFRARED,     0,  0,
      0,    RES_MENTAL|RES_MAGIC|RES_CHARM,    VULN_FIRE|VULN_LIGHT,
      A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K,
      TRUE,
      TRUE,
      &grn_illithid
   },
   {
      "halfling",        TRUE,
      0,        AFF_SNEAK,    0, 0,
      0,        0,    0,
      A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K,
      FALSE,
      TRUE,
      &grn_halfling
   },
   {
      "minotaur",        TRUE,
      0,    0,    0, 0,
      0,      RES_METAL, VULN_MENTAL|VULN_CHARM|VULN_SOUND,
      A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K,
      FALSE,
      TRUE,
      &grn_minotaur
   },
   {
      "arborian",        TRUE,
      0,    AFF_ACUTE_VISION,    0, 0,
      IMM_LIGHT,      RES_MENTAL|RES_CHARM|RES_SPELLS, VULN_DISEASE|VULN_POISON|VULN_SLASH|VULN_COLD,
      A|E|dd,      A|aa|bb|E,
      TRUE,
      TRUE,
      &grn_arborian
   },
   {
      "book",          FALSE,
      0,        AFF_DETECT_INVIS | AFF_DETECT_HIDDEN | AFF_ACUTE_VISION | AFF_FLYING | AFF_SWIM,    AFF_AWARENESS,   0,
      0,        0,      VULN_WEAPON|VULN_MAGIC|VULN_WOOD|VULN_SILVER|VULN_IRON|VULN_METAL|VULN_SPELLS,
      0,    P,
      TRUE,
      FALSE,
      &grn_book
   },
   {
      "demon",         FALSE,
      0,    AFF_FLYING|AFF_INFRARED,     0,  0,
      0,    RES_NEGATIVE,    VULN_HOLY|VULN_LIGHT,
      A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K|P,
      FALSE,
      TRUE,
      &grn_demon
   },
   {
      "bat",             FALSE,
      0,        AFF_FLYING|AFF_DARK_VISION,  0,    OFF_DODGE|OFF_FAST,
      0,        0,        VULN_LIGHT,
      A|G|V,        A|C|D|E|F|H|J|K|P,
      FALSE,
      TRUE,
      &grn_bat
   },
   {
      "bear",            FALSE,
      0,        0,   0,        OFF_CRUSH|OFF_DISARM|OFF_BERSERK,
      0,        RES_BASH|RES_COLD,    0,
      A|G|V,        A|B|C|D|E|F|H|J|K|U|V,
      FALSE,
      TRUE,
      &grn_bear
   },
   {
      "cat",            FALSE,
      0,        AFF_DARK_VISION, 0,    OFF_FAST|OFF_DODGE,
      0,        0,        0,
      A|G|V,        A|C|D|E|F|H|J|K|Q|U|V,
      FALSE,
      TRUE,
      &grn_cat
   },
   {
      "centipede",        FALSE,
      0,        AFF_DARK_VISION, 0,    0,
      0,        RES_PIERCE|RES_COLD,    VULN_BASH,
      A|B|G|O,        A|C|K|E,
      FALSE,
      TRUE,
      &grn_centipede
   },
   {
      "duergar",        FALSE,
      0,        AFF_INFRARED, 0,    0,
      0,        RES_POISON|RES_DISEASE, VULN_DROWNING|VULN_LIGHT,
      A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K,
      FALSE,
      TRUE,
      &grn_duergar
   },
   {
      "dog",            FALSE,
      0,        0,   0,        OFF_FAST,
      0,        0,        0,
      A|G|V,        A|C|D|E|F|H|J|K|U|V,
      FALSE,
      TRUE,
      &grn_dog
   },
   {
      "doll",            FALSE,
      0,        0,   0,        0,
      IMM_COLD|IMM_POISON|IMM_HOLY|IMM_NEGATIVE|IMM_MENTAL|IMM_DISEASE
      |IMM_DROWNING,    RES_BASH|RES_LIGHT,
      VULN_SLASH|VULN_FIRE|VULN_ACID|VULN_LIGHTNING|VULN_ENERGY,
      E|J|M|cc,    A|B|C|G|H|K,
      FALSE,
      FALSE,
      &grn_doll
   },
   {
      "doppelganger",                FALSE,
      0,              0,   0,              0,
      IMM_CHARM|IMM_SUMMON,              0,              0,
      A|C|H|M|V,        A|B|C|D|E|F|G|H|I|J|K,
      FALSE,
      TRUE,
      &grn_doppelganger
   },
   {
      "dragon",         FALSE,
      0,             AFF_INFRARED|AFF_FLYING,   0,    0,
      0,            RES_FIRE|RES_BASH|RES_CHARM,
      VULN_PIERCE|VULN_COLD,
      A|H|Z,        A|C|D|E|F|G|H|I|J|K|P|Q|U|V|X,
      FALSE,
      TRUE,
      &grn_dragon
   },
   {
      "fido",            FALSE,
      0,        0,   0,        OFF_DODGE|ASSIST_RACE,
      0,        0,            VULN_MAGIC,
      A|B|G|V,    A|C|D|E|F|H|J|K|Q|V,
      FALSE,
      TRUE,
      &grn_fido
   },
   {
      "fish",    FALSE,
      0,    AFF_SWIM,   0,    OFF_DODGE,
      0,    RES_SUMMON|RES_CHARM,    0,
      A|G|V,    A|D|E|F|K|O|Q|X,
      FALSE,
      TRUE,
      &grn_fish
   },
   {
      "fox",            FALSE,
      0,        AFF_DARK_VISION, 0,    OFF_FAST|OFF_DODGE,
      0,        0,        0,
      A|G|V,        A|C|D|E|F|H|J|K|Q|V,
      FALSE,
      TRUE,
      &grn_fox
   },
   {
      "goblin",        FALSE,
      0,        AFF_INFRARED, 0,    0,
      0,        RES_DISEASE,    VULN_MAGIC,
      A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K,
      FALSE,
      TRUE,
      &grn_goblin
   },
   {
      "hobgoblin",        FALSE,
      0,        AFF_INFRARED, 0,    0,
      0,        RES_DISEASE|RES_POISON,    0,
      A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K|Y,
      FALSE,
      TRUE,
      &grn_hobgoblin
   },
   {
      "kobold",        FALSE,
      0,        AFF_INFRARED, 0,    0,
      0,        RES_POISON,    VULN_MAGIC,
      A|B|H|M|V,    A|B|C|D|E|F|G|H|I|J|K|Q,
      FALSE,
      TRUE,
      &grn_kobold
   },
   {
      "lizard",        FALSE,
      0,        0,   0,        0,
      0,        RES_POISON,    VULN_COLD,
      A|G|X|cc,    A|C|D|E|F|H|K|Q|V,
      FALSE,
      TRUE,
      &grn_lizard
   },
   {
      "modron",        FALSE,
      0,        AFF_INFRARED, 0,        ASSIST_RACE|ASSIST_ALIGN,
      IMM_CHARM|IMM_DISEASE|IMM_MENTAL|IMM_HOLY|IMM_NEGATIVE,
      RES_FIRE|RES_COLD|RES_ACID,    0,
      H,        A|B|C|E|G|H|J|K,
      FALSE,
      TRUE,
      &grn_modron
   },
   {
      "pig",            FALSE,
      0,        0,   0,        0,
      0,        0,        0,
      A|G|V,         A|C|D|E|F|H|J|K,
      FALSE,
      TRUE,
      &grn_pig
   },
   {
      "rabbit",        FALSE,
      0,        0,   0,        OFF_DODGE|OFF_FAST,
      0,        0,        0,
      A|G|V,        A|C|D|E|F|H|J|K,
      FALSE,
      TRUE,
      &grn_rabbit
   },
   {
      "school monster",    FALSE,
      ACT_NOALIGN,        0,  0,        0,
      IMM_CHARM|IMM_SUMMON,    0,        VULN_MAGIC,
      A|M|V,        A|B|C|D|E|F|H|J|K|Q|U,
      FALSE,
      TRUE,
      &grn_school_monster
   },
   {
      "snake",        FALSE,
      0,        0,   0,        0,
      0,        RES_POISON,    VULN_COLD,
      A|G|X|Y|cc,    A|D|E|F|K|L|Q|V|X,
      FALSE,
      TRUE,
      &grn_snake
   },
   {
      "song bird",        FALSE,
      0,        AFF_FLYING,   0,        OFF_FAST|OFF_DODGE,
      0,        0,        0,
      A|G|W,        A|C|D|E|F|H|K|P,
      FALSE,
      TRUE,
      &grn_song_bird
   },
   {
      "water fowl",        FALSE,
      0,        AFF_SWIM|AFF_FLYING,    0,   0,
      0,        RES_DROWNING,        0,
      A|G|W,        A|C|D|E|F|H|K|P,
      FALSE,
      TRUE,
      &grn_water_fowl
   },
   {
      "wolf",            FALSE,
      0,        AFF_DARK_VISION, 0,    OFF_FAST|OFF_DODGE,
      0,        0,        0,
      A|G|V,        A|C|D|E|F|J|K|Q|V,
      FALSE,
      TRUE,
      &grn_wolf
   },
   {
      "wyvern",        FALSE,
      0,        AFF_FLYING|AFF_DETECT_INVIS|AFF_DETECT_HIDDEN, 0,
      OFF_BASH|OFF_FAST|OFF_DODGE,
      IMM_POISON,    0,    VULN_LIGHT,
      A|B|G|Z,        A|C|D|E|F|H|J|K|Q|V|X,
      FALSE,
      TRUE,
      &grn_wyvern
   },
   {
      "spider",        FALSE,
      0,        AFF_DARK_VISION, 0,    0,
      0,        RES_PIERCE|RES_COLD,    VULN_BASH,
      A|B|G|O,        A|C|E|K,
      FALSE,
      TRUE,
      &grn_spider
   },
   {
      NULL,             FALSE,
      0,       0,       0, 0,
      0,       0,       0,
      0,       0,
      FALSE,
      TRUE,
      NULL
   }
};

const    struct    pc_race_type    pc_race_table    [MAX_PC_RACE]    =
{
   {
      "null race",    "",             0, 0,
      { "" }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
      { 13, 13, 13, 13, 13 }, { 18, 18, 18, 18, 18 }, 0,
      17,  /* punch */
      0,
      0
   },
   /*
   {
      "race name",    short name,     align, xpadd,
      { bonus skills }, {class types}
      { base stats },        { max stats },        size,
      dam_type
   },
   */
   {
      "human",        "Human",        0, 0,
      { NULL }, { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
      { 20, 18, 18, 20, 20 }, { 20, 20, 20, 20, 20 }, SIZE_MEDIUM,
      17,  /* punch */
      HUMAN_ONLY,
      0
   },
   {
      "dwarf",        "Dwarf",        0, 500,
      { "berserk" }, { 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 1 },
      { 20, 17, 19, 18, 21 }, { 22, 17, 22, 19, 25 }, SIZE_MEDIUM,
      17,  /* punch */
      DWARF_ONLY,
      0
   },
   {
      "elf",          " Elf ",        3, 500,
      { "awareness", "forest blending", "archery", "sneak" }, { 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1 },
      { 18, 21, 18, 20, 17 }, { 18, 25, 20, 23, 17 }, SIZE_MEDIUM,
      17,  /* punch */
      ELF_ONLY,
      0
   },
   {
      "grey-elf",          "G-Elf",        4, 400,
      { "awareness", "forest blending", "archery", "sneak" },
      { 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1 },
      { 17, 20, 21, 20, 16 }, { 17, 24, 24, 22, 16 }, SIZE_MEDIUM,
      17,  /* punch */
      GREY_ONLY,
      0
   },
   {
      "dark-elf",     "D-Elf",        5, 500,
      { "globe of darkness", "faerie fire", "sneak" },
      { 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1 },
      { 18, 20, 18, 21, 17 }, { 18, 24, 20, 24, 17 }, SIZE_MEDIUM,
      17,  /* punch */
      DARK_ONLY,
      0
   },
   {
      "centaur",          " Cen ",        3, 400,
      { "rear kick", "enhanced damage", "kick", "door bash" },
      { 1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1 },
      { 20, 17, 18, 18, 19 }, { 24, 17, 18, 20, 22 }, SIZE_LARGE,
      17,  /* punch */
      CENTAUR_ONLY,
      0
   },
   {
      "troll",        "Troll",        5, 400,
      { "regeneration", "enhanced damage", "bash", "door bash" },
      { 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0},
      { 21, 16, 17, 18, 20 }, { 25, 16, 17, 19, 24 }, SIZE_LARGE,
      17,  /* punch */
      TROLL_ONLY,
      0
   },
   {
      "giant",        "Giant",        4, 400,
      { "door bash", "enhanced damage", "bash" },
      { 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0},
      { 21, 16, 17, 18, 21 }, { 25, 16, 17, 18, 24 }, SIZE_LARGE,
      17,  /* punch */
      GIANT_ONLY,
      0
   },
   {
      "gnome",        "Gnome",        0, 400,
      { "staves", "wands" },
      { 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1 },
      { 17, 19, 21, 18, 18 }, { 18, 22, 25, 20, 19 }, SIZE_SMALL,
      17,  /* punch */
      GNOME_ONLY,
      0
   },
   {
      "draconian",    "Draco",        6, 300,
      { "breath attack" }, { 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1 },
      { 21, 19, 19, 18, 18 }, { 23, 23, 20, 18, 20 }, SIZE_MEDIUM,
      5,  /* claw */
      DRACONIAN_ONLY,
      0
   },
   {
      "ethereal",      " Eth ",        1, 400,
      { "gaseous form" }, { 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1 },
      { 17, 20, 19, 20, 18 }, { 17, 23, 22, 23, 18 }, SIZE_SMALL,
      17,  /* punch */
      0,
      ETHEREAL_ONLY
   },
   {
      "changeling",   "Chnge",        0, 300,
      {"shapeshift"}, {1, 1, 0, 1, 1, 0, 0, 1, 0, 0, 1},
      {17, 20, 20, 21, 15},    {17, 24, 22, 25, 15},    SIZE_MEDIUM,
      17,  /* punch */
      CHANGELING_ONLY,
      0
   },
   {
      "illithid",    "Ilthd",    5, 450,
      { "psionic blast" },    { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1 },
      { 16, 23, 20, 18, 16 },    {17, 25, 21, 22, 17},    SIZE_MEDIUM,
      17,  /* punch */
      ILLITHID_ONLY,
      0
   },
   {
      "halfling",    "Half",        1, 300,
      {"steal", "peek", "pick lock", "sneak"}, {1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0},
      {16, 20, 21, 22, 17 }, {16, 21, 23, 25, 18}, SIZE_SMALL,
      17,  /* punch */
      0,
      HALFLING_ONLY
   },
   {
      "minotaur",    "Mino",    0, 300,
      { "ramming" },    {1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0},
      {21, 17, 20, 18, 20},    {23, 17, 23, 20, 22}, SIZE_LARGE,
      17,  /* punch */
      MINOTAUR_ONLY,
      0
   },
   {
      "arborian",     "Arbor",        3, 500,
      { "awareness", "forest blending", "mental link" },
      {0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1},
      {15, 21, 21, 19, 17},   {15, 24, 25, 20, 17}, SIZE_SMALL,
      37,  /* thwack */
      0,
      ARBOR_ONLY
   },
   {
      "book",     "Book",        0, 0,
      {"hyper", "nap"},       {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
      {12, 25, 12, 25, 12},   {5, 25, 5, 25, 25}, SIZE_TINY,
      37,  /* thwack */
      0,
      BOOK_ONLY
   },
   {
      "demon",    "Demon",    5, 450,
      {NULL},    { 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1 },
      { 16, 22, 18, 18, 16 },    {21, 22, 18, 22, 20},    SIZE_MEDIUM,
      50,  /* cries of damnation */
      0,
      DEMON_ONLY
   },
};

/*
* Class table.
*/
const    struct    class_type    class_table    [MAX_CLASS]    =
{
   {
      "warrior", "War",  0, 0, OBJ_VNUM_SCHOOL_SWORD,
      { 9633, 8818 },  75,  20,  -10,  8, 19, FALSE,
      "class basics", "class default",
      WARRIOR_ONLY,
      0
   },
   {
      "thief", "Thi",  0, 0, OBJ_VNUM_SCHOOL_DAGGER,
      { 9639, 8850 },  75,  20,  -4,  8, 17, FALSE,
      "class basics", "class default",
      THIEF_ONLY,
      0
   },
   {
      "cleric", "Cle",  0, 0, OBJ_VNUM_SCHOOL_MACE,
      { 9619, 8836 },  75,  20, 2,  7, 15, TRUE,
      "class  basics", "class default",
      CLERIC_ONLY,
      0
   },
   {
      "paladin", "Pal",  3, 250, OBJ_VNUM_SCHOOL_SWORD,
      { 9753, 8835, 9754 },  75,  20, -6,  8,  18, TRUE,
      "class basics", "class default",
      PALADIN_ONLY,
      0
   },
   {
      "anti-paladin", "A-P",  5, 250, OBJ_VNUM_SCHOOL_SWORD,
      { 8820, 9780, 9751 },  75,  20, -6,  8,  18, TRUE,
      "class basics", "class default",
      ANTI_PALADIN_ONLY,
      0
   },
   {
      "ranger", "Ran",  0, 200, OBJ_VNUM_SCHOOL_SWORD,
      { 6156, 1874, 1890, 1891 },  75,  20, -8,  8,  18, FALSE,
      "class basics", "class default",
      RANGER_ONLY,
      0
   },
   {
      "monk", "Mon",  1, 200, 0,  /* NO WEAPON */
      { 9597, 8819 },  75,  20, 2,  10,  16, FALSE,
      "class basics", "class default",
      MONK_ONLY,
      0
   },
   {
      "channeler", "Cha",  0, 250, OBJ_VNUM_SCHOOL_DAGGER,
      { 9617, 9618, 9648, 8817 },  75,  20, 2,  7,  15, TRUE,
      "class basics", "class default",
      CHANNELER_ONLY,
      0
   },
   {
      "nightwalker", "N-W",  5, 300, OBJ_VNUM_SCHOOL_SWORD,
      { 8837, 9776, 9777 },  75,  20, -4,  8,  17, FALSE,
      "class basics", "class default",
      NIGHTWALKER_ONLY,
      0
   },
   {
      "necromancer", "Nec",  5, 250, OBJ_VNUM_SCHOOL_DAGGER,
      { 8815, 9727, 9728 },  75,  20, -2,  7,  15, TRUE,
      "class basics", "class default",
      NECROMANCER_ONLY,
      0
   },
   {
      "elementalist", "Ele",  0, 250, OBJ_VNUM_SCHOOL_DAGGER,
      { 8897, 9779 },  75,  20, -4,  8,  16, TRUE,
      "class basics", "class default",
      ELEMENTALIST_ONLY,
      0
   },
};


/* Specialization Types */
char *  const                   special_table   [MAX_CLASS][16] =
{
   {
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""
   },
   {
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""
   },
   {
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""
   },
   {
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""
   },
   {
      "Apocalyptic Knight of War",
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""
   },
   {
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""
   },
   {
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""
   },
   {
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""
   },
   {
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""
   },
   {
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""
   },
   {
      "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""
   }
};

/*
   Titles.
*/
char *    const            title_table    [MAX_CLASS][MAX_LEVEL+1][2] =
{
   {
      { "Man",            "Woman"                },

      { "Swordpupil",            "Swordpupil"            },
      { "Recruit",            "Recruit"            },
      { "Sentry",            "Sentress"            },
      { "Fighter",            "Fighter"            },
      { "Soldier",            "Soldier"            },

      { "Warrior",            "Warrior"            },
      { "Veteran",            "Veteran"            },
      { "Swordsman",            "Swordswoman"            },
      { "Fencer",            "Fenceress"            },
      { "Combatant",            "Combatess"            },

      { "Hero",            "Heroine"            },
      { "Myrmidon",            "Myrmidon"            },
      { "Swashbuckler",        "Swashbuckleress"        },
      { "Mercenary",            "Mercenaress"            },
      { "Swordmaster",        "Swordmistress"            },

      { "Lieutenant",            "Lieutenant"            },
      { "Champion",            "Lady Champion"            },
      { "Dragoon",            "Lady Dragoon"            },
      { "Cavalier",            "Lady Cavalier"            },
      { "Knight",            "Lady Knight"            },

      { "Grand Knight",        "Grand Knight"            },
      { "Master Knight",        "Master Knight"            },
      { "Grand Warrior",              "Grand Warrior"                       },
      { "Greater Combatant",           "Greater Combatant"                 },
      { "Demon Slayer",        "Demon Slayer"            },

      { "Greater Demon Slayer",    "Greater Demon Slayer"        },
      { "Dragon Slayer",        "Dragon Slayer"            },
      { "Greater Dragon Slayer",    "Greater Dragon Slayer"        },
      { "Underlord",            "Underlord"            },
      { "Overlord",            "Overlord"            },

      { "Baron of Thunder",        "Baroness of Thunder"        },
      { "Baron of Storms",        "Baroness of Storms"        },
      { "Baron of Tornadoes",        "Baroness of Tornadoes"        },
      { "Baron of Hurricanes",    "Baroness of Hurricanes"    },
      { "Baron of Meteors",        "Baroness of Meteors"        },

      { "Rock Crusher",       "Rock Crusher"  },
      { "Shield Cleaver",     "Shield Cleaver"        },
      { "Student of War",     "Student of War"        },
      { "Learned of War",     "Learned of War"        },
      { "Master of War",      "Mistress of War"       },

      { "Bone Shatterer",  "Bone Shatterer"     },
      { "Skull Cracker",  "Skull Cracker"     },
      { "Armor Splitter",   "Armor Splitter"    },
      { "Sword Breaker",      "Sword Breaker"         },
      { "Weapon Master",      "Weapon Mistress"       },

      { "Student of Battle",  "Student of Battle"     },
      { "Learned of Battle",  "Learned of Battle"     },
      { "Master of Battle",   "Mistress of Battle"    },
      { "Grand Battle Master",        "Grand Battle Mistress"   },
      { "Supreme Battle Master",      "Supreme Battle Mistress"       },

      { "Knight Hero",        "Knight Heroine"        },
      { "Avatar",              "Avatar"                 },
      { "Angel",               "Angel"                  },
      { "Demigod",             "Demigoddess"            },
      { "Immortal",           "Immortal"              },
      { "God",                 "God"                    },
      { "Deity",               "Deity"                  },
      { "Supreme Master",      "Supreme Mistress"       },
      { "Creator",                    "Creator"                       },
      { "Implementor",        "Implementress"            }
   },
   {
      { "Man",            "Woman"                },

      { "Pilferer",            "Pilferess"            },
      { "Footpad",            "Footpad"            },
      { "Filcher",            "Filcheress"            },
      { "Pick-Pocket",        "Pick-Pocket"            },
      { "Sneak",            "Sneak"                },

      { "Pincher",            "Pincheress"            },
      { "Cut-Purse",            "Cut-Purse"            },
      { "Snatcher",            "Snatcheress"            },
      { "Sharper",            "Sharpress"            },
      { "Rogue",            "Rogue"                },

      { "Robber",            "Robber"            },
      { "Magsman",            "Magswoman"            },
      { "Highwayman",            "Highwaywoman"            },
      { "Burglar",            "Burglaress"            },
      { "Thief",            "Thief"                },

      { "Knifer",            "Knifer"            },
      { "Quick-Blade",        "Quick-Blade"            },
      { "Killer",            "Murderess"            },
      { "Brigand",            "Brigand"            },
      { "Cut-Throat",            "Cut-Throat"            },

      { "Spy",            "Spy"                },
      { "Grand Spy",            "Grand Spy"            },
      { "Master Spy",            "Master Spy"            },
      { "Greater Thief",              "Greater Thief"                      },
      { "Master of Thievery",         "Mistress of Thievery"              },

      { "Master of Vision",        "Mistress of Vision"        },
      { "Master of Hearing",        "Mistress of Hearing"        },
      { "Master of Smell",        "Mistress of Smell"        },
      { "Master of Taste",        "Mistress of Taste"        },
      { "Master of Touch",        "Mistress of Touch"        },

      { "Crime Lord",            "Crime Mistress"        },
      { "Infamous Crime Lord",    "Infamous Crime Mistress"    },
      { "Greater Crime Lord",        "Greater Crime Mistress"    },
      { "Master Crime Lord",        "Master Crime Mistress"        },
      { "Godfather",            "Godmother"            },

      { "Student of Speed",   "Student of Speed"      },
      { "Learned of Speed",   "Learned of Speed"      },
      { "Master of Speed",    "Mistress of Speed"     },
      { "Shadow",     "Shadow"        },
      { "Master of Shadows",  "Mistress of Shadows"   },

      { "Hidden",     "Hidden"        },
      { "Unseen",     "Unseen"        },
      { "Undetectable",       "Undetectable"  },
      { "Untouchable",        "Untouchable"   },
      { "Covert",    "Covert"       },

      { "Master of Disguise",     "Mistress of Disguise"        },
      { "Master Blackjacker",    "Mistress Blackjacker"       },
      { "Master Backstabber",     "Mistress Backstabber"        },
      { "Master of the Blade",   "Mistress of the Blade"      },
      { "Grand Master of Thieves",    "Grand Mistress of Thieves"     },

      { "Thief Hero",              "Thief Heroine"              },
      { "Avatar",            "Avatar",              },
      { "Angel",             "Angel"                },
      { "Demigod",       "Demigoddess"      },
      { "Immortal",           "Immortal"             },
      { "God",           "God",             },
      { "Deity",         "Deity"            },
      { "Supreme Master",        "Supreme Mistress"        },
      { "Creator",                    "Creator"                       },
      { "Implementor",        "Implementress"            }
   },
   {
      { "Man",            "Woman"                },

      { "Apprentice of Magic",    "Apprentice of Magic"        },
      { "Altar Boy",  "Altar Girl"    },
      { "Believer",   "Believer"      },
      { "Attendant", "Attendant"      },
      { "Acolyte", "Acolyte"  },

      { "Novice", "Novice"    },
      { "Hermit", "Hermit"    },
      { "Monk", "Nun"         },
      { "Adept", "Adept"      },
      { "Deacon", "Deaconess" },

      { "Seminary Student", "Seminary Student"        },
      { "Seminary Scholar", "Seminary Scholar"        },
      { "Receiver of the Call", "Receiver of the Call" },
      { "Chosen", "Chosen"    },
      { "Ordained", "Ordained"        },

      { "Priest", "Priestess"         },
      { "Minister", "Lady Minister"   },
      { "Curate", "Curate"    },
      { "Canon", "Canon"      },
      { "Healer", "Healeress"         },

      { "Chaplain", "Chaplain"        },
      { "Expositor", "Expositoress"   },
      { "Missionary", "Missionary"    },
      { "Seminary Teacher", "Seminary Teacher"        },
      { "Bishop", "Bishop"    },

      { "Arch Bishop", "Arch Bishop"  },
      { "Patriarch", "Matriarch"      },
      { "Elder Patriarch", "Elder Matriarch"  },
      { "Grand Patriarch", "Grand Matriarch"  },
      { "Great Patriarch", "Great Matriarch"  },

      { "Exorcist", "Exorcist"        },
      { "Demon Killer", "Demon Killer"        },
      { "Greater Demon Killer", "Greater Demon Killer"        },
      { "Cardinal of the Earth", "Cardinal of the Earth"      },
      { "Cardinal of the Seas", "Cardinal of the Seas"        },

      { "Cardinal of the Winds", "Cardinal of the Winds"      },
      { "Cardinal of the Fire", "Cardinal of the Fire"        },
      { "Cardinal of the Ether", "Cardinal of the Ether"      },
      { "Cardinal of the North", "Cardinal of the North"      },
      { "Cardinal of the East", "Cardinal of the East"        },

      { "Cardinal of the South", "Cardinal of the South"      },
      { "Cardinal of the West", "Cardinal of the West"        },
      { "Avatar of an Immortal", "Avatar of an Immortal"      },
      { "Avatar of a Deity", "Avatar of a Deity"      },
      { "Avatar of a God", "Avatar of a God"  },

      { "Avatar of an Implementor", "Avatar of an Implementor"        },
      { "Master of Divinity", "Mistress of Divinity"  },
      { "Saint", "Saint"      },
      { "Divine", "Divine"    },
      { "Holy Father", "Holy Mother"  },

      { "Holy Hero",                  "Holy Heroine"                  },
      { "Avatar",            "Avatar"               },
      { "Angel",             "Angel"                },
      { "Demigod",           "Demigoddess"          },
      { "Immortal",          "Immortal"             },
      { "God",               "Goddess"              },
      { "Deity",             "Deity"                },
      { "Supremity",         "Supremity"            },
      { "Creator",            "Creator"            },
      { "Implementor",        "Implementress"            }
   },

   {
      { "Man",            "Woman"                },

      { "Paladin Pupil", "Paladin Pupil" },
      { "Scullery Man", "Scullery Maid" },
      { "Squire Candidate", "Squire Candidate" },
      { "Shield Bearer", "Shield Bearer" },
      { "Sword Bearer", "Sword Bearer" },

      { "Bow Bearer", "Bow Bearer" },
      { "Standard Bearer", "Standard Bearer" },
      { "Horseman", "Horsewoman" },
      { "Squire Initiate", "Squire Initiate" },
      { "Squire", "Squire" },

      { "Footman", "Footwoman" },
      { "Pikeman", "Pikewoman" },
      { "Bowman", "Bowwoman" },
      { "Swordsman", "Swordsman" },
      { "Honorable", "Honorable" },

      { "Noble", "Noble" },
      { "Trustworthy", "Trustworthy" },
      { "Truthful", "Truthful" },
      { "Chivalrous", "Chivalrous" },
      { "Paladin", "Paladin" },

      { "Questor", "Questor" },
      { "Cavalier", "Cavalier" },
      { "Champion", "Champion" },
      { "Knight of Renown", "Knight of Renown" },
      { "Paladin Knight", "Paladin Knight" },

      { "Templar Initiate", "Templar Initiate" },
      { "Priest-Knight", "Priestess-Knight" },
      { "Knight of the Cross", "Knight of the Cross" },
      { "Champion of the Cross", "Champion of the Cross" },
      { "Knight Templar", "Lady Templar" },

      { "Warrior of the White Rose", "Warrior of the White Rose" },
      { "Knight of the White Rose", "Lady of the White Rose" },
      { "Master of the White Rose", "Mistress of the White Rose" },
      { "Prince of the White Rose", "Princess of the White Rose" },
      { "King of the White Rose", "Queen of the White Rose" },

      { "Valiant", "Valiant" },
      { "Courageous", "Courageous" },
      { "Healer-Knight", "Healer-Lady" },
      { "Avenger", "Avenger" },
      { "Defender", "Defender" },

      { "Protector of Innocents", "Protector of Innocents" },
      { "Champion of Innocents", "Champion of Innocents" },
      { "Champion of the Templars", "Champion of the Templars" },
      { "Priest of the Templars", "Priestess of the Templars" },
      { "High Priest of the Templars", "High Priestess of the Templars" },

      { "Lord of the Templars", "Lady of the Templars" },
      { "Hammer of Heretics", "Hammer of Heretics" },
      { "Slayer of Infidels", "Slayer of Infidels" },
      { "Pious", "Pious" },
      { "Holy Knight", "Holy Lady" },

      { "Paladin Hero",                  "Paladin Heroine"                  },
      { "Avatar",            "Avatar"               },
      { "Angel",             "Angel"                },
      { "Demigod",           "Demigoddess"          },
      { "Immortal",          "Immortal"             },
      { "God",               "Goddess"              },
      { "Deity",             "Deity"                },
      { "Supremity",         "Supremity"            },
      { "Creator",            "Creator"            },
      { "Implementor",        "Implementress"            }
   },

   {
      { "Man",            "Woman"                },

      { "Scum", "Scum" },
      { "Bully", "Bully" },
      { "Thug", "Moll" },
      { "Brute", "Brute" },
      { "Ruffian", "Ruffian" },

      { "Pillager", "Pillager" },
      { "Destroyer", "Destroyer" },
      { "Arsonist", "Arsonist" },
      { "Hired Killer", "Hired Killer" },
      { "Brigand", "Brigand" },

      { "Mercenary", "Mercenary" },
      { "Black Sword", "Black Sword" },
      { "Crimson Sword", "Crimson Sword" },
      { "Black Hearted", "Black Hearted" },
      { "Cruel", "Cruel" },

      { "Stealer", "Stealer" },
      { "Infamous", "Infamous" },
      { "Despised", "Despised" },
      { "Complete Bastard", "Complete Bitch" },
      { "Anti-Paladin", "Anti-Paladin" },

      { "Evil Fighter", "Evil Fighter" },
      { "Rogue Knight", "Rogue Lady" },
      { "Evil Champion", "Evil Champion" },
      { "Slayer of Innocents", "Slayer of Innocents" },
      { "Black Knight", "Black Lady" },

      { "Crimson Knight", "Crimson Lady" },
      { "Knight of Brimstone", "Lady of Brimstone" },
      { "Knight of the Inverted Cross", "Lady of the Inverted Cross" },
      { "Knight of Pain", "Lady of Pain" },
      { "Knight of Darkness", "Lady of Darkness" },

      { "Footman of Darkness", "Footwoman of Darkness" },
      { "Cavalier of Darkness", "Cavalier of Darkness" },
      { "Captain of Darkness", "Captain of Darkness" },
      { "General of Darkness", "General of Darkness" },
      { "Field Marshall of Darkness", "Field Marshall of Darkness" },

      { "Knight of the Apocalypse", "Lady of the Apocalypse" },
      { "LightSlayer", "LightSlayer" },
      { "Invoker of Suffering", "Invoker of Suffering" },
      { "Arch-Fiend", "Arch-Fiend" },
      { "Evil Lord", "Evil Lady" },

      { "Evil Prince", "Evil Princess" },
      { "Evil King", "Evil Queen" },
      { "Destroyer of Hope", "Destroyer of Hope" },
      { "Spreader of Pestilence", "Spreader of Pestilence" },
      { "Bringer of Famine", "Bringer of Famine" },

      { "Harbinger of Death", "Harbinger of Death" },
      { "Herald of Doom", "Herald of Doom" },
      { "Dark Slayer", "Dark Slayer" },
      { "Evil Incarnate", "Evil Incarnate" },
      { "Hand of the Dark", "Hand of the Dark" },

      { "Anti-Hero",                  "Anti-Heroine"                  },
      { "Avatar",            "Avatar"               },
      { "Angel",             "Angel"                },
      { "Demigod",           "Demigoddess"          },
      { "Immortal",          "Immortal"             },
      { "God",               "Goddess"              },
      { "Deity",             "Deity"                },
      { "Supremity",         "Supremity"            },
      { "Creator",            "Creator"            },
      { "Implementor",        "Implementress"            }
   },

   {
      { "Man",            "Woman"                },

      { "Forest Pupil", "Forest Pupil" },
      { "Forest Recruit", "Forest Recruit" },
      { "Forest Squire", "Forest Squire" },
      { "Apprentice Forester", "Apprentice Forester" },
      { "Forester", "Forester" },

      { "Greater Forester", "Greater Forester" },
      { "Master Forester", "Mistress Forester" },
      { "Trapper", "Trapper" },
      { "Hunter", "Hunter" },
      { "Scout", "Scout" },

      { "Master Scout", "Mistress Scout" },
      { "Tracker", "Tracker" },
      { "Master Tracker", "Master Tracker" },
      { "Forest Walker", "Forest Walker" },
      { "Woodsman", "Woodswoman" },

      { "Master Woodsman", "Mistress Woodswoman" },
      { "Ranger Initiate", "Ranger Initiate" },
      { "Ranger Candidate", "Ranger Candidate" },
      { "Ranger Squire", "Ranger Squire" },
      { "Apprentice Ranger", "Apprentice Ranger" },

      { "Ranger", "Ranger" },
      { "Greater Ranger", "Greater Ranger" },
      { "Ranger Captain", "Ranger Captain" },
      { "Ranger General", "Ranger General" },
      { "Master Ranger", "Mistress Ranger" },

      { "Ranger Lord", "Ranger Lady" },
      { "Ranger Baron", "Ranger Baroness" },
      { "Ranger Prince", "Ranger Princess" },
      { "Ranger King", "Ranger Queen" },
      { "Warder", "Warder" },

      { "Warder Captain", "Warder Captain" },
      { "Warder General", "Warder General" },
      { "Master of Warders", "Mistress of Warders" },
      { "Lord of Warders", "Lady of Warders" },
      { "Warder Baron", "Warder Baroness" },

      { "Warder Prince", "Warder Princess" },
      { "Warder King", "Warder Queen" },
      { "Warrior of the Forest", "Warrior of the Forest" },
      { "Sword of the Forest", "Sword of the Forest" },
      { "Knight of the Forest", "Knight of the Forest" },

      { "Master of the Forest", "Mistress of the Forest" },
      { "Lord of the Forest", "Lady of the Forest" },
      { "Baron of the Forest", "Baroness of the Forest" },
      { "Prince of the Forest", "Princess of the Forest" },
      { "King of the Forest", "Queen of the Forest" },

      { "Watcher of the Glade", "Watcher of the Glade" },
      { "Defender of the Glade", "Defender of the Glade" },
      { "Keeper of the Glade", "Keeper of the Glade" },
      { "Preserver of Nature", "Preserver of Nature" },
      { "Supreme Protector of Nature", "Supreme Protector of Nature" },

      { "Ranger Hero",                  "Ranger Heroine"                  },
      { "Avatar",            "Avatar"               },
      { "Angel",             "Angel"                },
      { "Demigod",           "Demigoddess"          },
      { "Immortal",          "Immortal"             },
      { "God",               "Goddess"              },
      { "Deity",             "Deity"                },
      { "Supremity",         "Supremity"            },
      { "Creator",            "Creator"            },
      { "Implementor",        "Implementress"            }
   },
   {
      { "Man",        "Woman"                         },
      { "Novice",     "Novice",                       },      /* 1 */
      { "Seeker of Belts", "Seeker of Belts"          },
      { "Monastic",   "Monastic"                      },
      { "Thoughtful", "Thoughtful"                    },
      { "Novice of Judo",     "Novice of Judo"        }, /* 5 */
      { "Scholar of Combat",  "Scholar of Combat"     },
      { "Student of Kung-Fu", "Student of Kung-Fu"    },
      { "Learned of Judo",    "Learned of Judo"       },
      { "Scholar of War",     "Scholar of War"        },
      { "Unarmed Fighter",    "Unarmed Fighter"       },      /* 10 */
      { "Novice of Kung-Fu",  "Novice of Kung-Fu"     },
      { "Boxer",              "Boxer"                 },
      { "Kickboxer",          "Kickboxer"             },
      { "Learned of Kung-Fu", "Learned of Kung-Fu"    },
      { "Learned Combatant",  "Learned Combatant"     }, /* 15 */
      { "Expert of Kung-Fu",  "Expert of Kung-Fu"     },
      { "Master of Kung-Fu",  "Master of Kung-Fu"     },
      { "Novice Monk",        "Novice Monk"           },
      { "Initiate Monk",      "Initiate Monk"         },
      { "Monk",               "Monk"                  },     /* 20 */
      { "Shaolin Fighter",    "Shaolin Fighter"       },
      { "Expert of Judo",     "Expert of Judo"        },
      { "Shaolin Master",     "Shaolin Master"        },
      { "Grandmaster of Judo", "Grandmistress of Judo" },
      { "First Dan",          "First Dan"             },      /* 25 */
      { "Second Dan",         "Second Dan"            },
      { "Third Dan",          "Third Dan"             },
      { "Master of Akemi",    "Mistress of Akemi"     },
      { "Master of Patience", "Mistress of Patience"  },
      { "Master of Thought",  "Mistress of Thought"   },  /* 30 */
      { "Master of Mind",     "Mistress of Mind"      },
      { "Master of Combat",   "Mistress of Combat"    },
      { "Master of Unarmed Combat", "Mistress of Unarmed Combat"},
      { "Tranquil",           "Tranquil",             },
      { "Master of Shuto",    "Mistress of Shuto"     },      /* 35 */
      { "Enlightened",        "Enlightened"           },
      { "Lore Master",        "Lore Mistress"         },
      { "Adept",              "Adept"                 },
      { "Adept of Wind",      "Adept of Wind"         },
      { "Adept of Mind",      "Adept of Mind"         },      /* 40 */
      { "Adept of Thought",   "Adept of Thought"      },
      { "Adept of State",     "Adept of State"        },
      { "Kaiden",             "Kaiden"                },
      { "Sensei",             "Sensei"                },
      { "Sensei",             "Sensei"                },      /* 45 */
      { "Shihan",             "Shihan"                },
      { "Archon of Thought",  "Archon of Thought"     },
      { "Archon of State",    "Archon of State"       },
      { "Transcendant",       "Transcendant"          },
      { "Lord Adept",         "Lady Adept"            },  /* 50 */
      { "Monk Hero",          "Monk Heroine"          },
      { "Avatar of Magic",        "Avatar of Magic"        },
      { "Angel of Magic",        "Angel of Magic"        },
      { "Demigod of Magic",        "Demigoddess of Magic"        },
      { "Immortal of Magic",        "Immortal of Magic"        },
      { "God of Magic",        "Goddess of Magic"        },
      { "Deity of Magic",        "Deity of Magic"        },
      { "Supremity of Magic",        "Supremity of Magic"        },
      { "Creator",            "Creator"            },
      { "Implementor",        "Implementress"            }
   },

   {
      { "Man",            "Woman"                },
      {"Apprentice Channeler", "Apprentice Channeler"},
      {"Student of Channeling",  "Student of Channeling"},
      {"Journeyman Channeler", "Journeywoman Channeler"},
      {"Novice Channeler", "Novice Channeler"},
      {"Medium of Channeling", "Medium of Channeling"},
      {"Apprentice Scrivener", "Apprentice Scrivener"},
      {"Journeyman Scrivener", "Journeywoman Scrivener"},
      {"Scrivener", "Scrivener"},
      {"Accomplished Scrivener", "Accomplished Scrivener"},
      {"Master Scribe", "Master Scribe"},
      {"Apprentice Alchemist", "Apprentice Alchemist"},
      {"Student of Alchemy", "Student of Alchemy"},
      {"Journeyman Alchemist", "Journeywoman Alchemist"},
      {"Alchemist", "Alchemist"},
      {"Master of Alchemy", "Master of Alchemy"},
      {"Wizard", "Witch"},
      {"Sorcerer", "Sorceress"},
      {"Grand Sorcerer", "Grand Sorceress"},
      {"Master of Sorcery", "Mistress of Sorcery"},
      {"Channeler", "Channeler"},
      {"Maker of Stones", "Maker of Stones"},
      {"Maker of Wands", "Maker of Wands"},
      {"Maker of Potions", "Maker of Potions"},
      {"Maker of Staves", "Maker of Staves"},
      {"Craftsman", "Craftswoman",},
      {"Master Craftsman", "Mistress Craftswoman"},
      {"Student of Light", "Student of Light"},
      {"Student of Focus", "Student of Focus"},
      {"Student of Absorption", "Student of Absorption"},
      {"Student of Form", "Student of Form"},
      {"Student of Space", "Student of Space"},
      {"Student of Time", "Student of Time"},
      {"Wizard of Light", "Wizard of Light"},
      {"Wizard of Focus", "Wizard of Focus"},
      {"Wizard of Absorption", "Wizard of Absorption"},
      {"Wizard of Form", "Wizard of Form"},
      {"Wizard of Space", "Wizard of Space"},
      {"Temporal Wizard", "Temporal Wizard"},
      {"Temporal Master", "Temporal Mistress"},
      {"Mystic", "Mystic"},
      {"Grand Mystic", "Grand Mystic"},
      {"Master Mystic", "Mistress Mystic"},
      {"Archmage of Light", "Archmage of Light"},
      {"Archmage of Focus", "Archmage of Focus"},
      {"Archmage of Absorption", "Archmage of Absorption"},
      {"Archmage of Form", "Archmage of Form"},
      {"Archmage of Space", "Archmage of Space"},
      {"Temporal Archmage", "Temporal Archmage"},
      {"Archmage of Mysticism", "Archmage of Mysticism"},
      {"Archmage of Channeling", "Archmage of Channeling"},
      {"Hero of Magical Transference", "Heroine of Magical Transference"},

      { "Avatar of Magic",        "Avatar of Magic"        },
      { "Angel of Magic",        "Angel of Magic"        },
      { "Demigod of Magic",        "Demigoddess of Magic"        },
      { "Immortal of Magic",        "Immortal of Magic"        },
      { "God of Magic",        "Goddess of Magic"        },
      { "Deity of Magic",        "Deity of Magic"        },
      { "Supremity of Magic",        "Supremity of Magic"        },
      { "Creator",            "Creator"            },
      { "Implementor",        "Implementress"            }
   },
   {
      { "Man",            "Woman"                },

      { "Uke", "Uke"  },
      { "Tori", "Tori"        },
      { "White Belt", "White Belt"    },
      { "Yellow Belt", "Yellow Belt"  },
      { "Green Belt", "Green Belt"    },

      { "3rd Brown", "3rd Brown"      },
      { "2nd Brown", "2nd Brown"      },
      { "1st Brown", "1st Brown"      },
      { "3rd Black", "3rd Black"      },
      { "2nd Black", "2nd Black"      },

      { "Okuri", "Okuri"      },
      { "Shuto", "Shuto"      },
      { "Uraken", "Uraken"    },
      { "Hidden", "Hidden"    },
      { "Shrouded", "Shrouded"        },

      { "Prowler", "Prowler"  },
      { "Tailer", "Tailer"    },
      { "Stalker", "Stalker"  },
      { "Master Stalker", "Master Stalker"    },
      { "Nightwalker", "Nightwalker"  },

      { "Master of the Shadows", "Mistress of the Shadows"    },
      { "Master of the Darkness", "Mistress of the Darkness"  },
      { "Master of the Night", "Mistress of the Night"  },
      { "King of Black", "Queen of Black"      },
      { "Shadowwalker", "Shadowwalker"        },

      { "Master of Stealth", "Mistress of Stealth"      },
      { "Master of Initiative", "Mistress of Initiative"      },
      { "Master of Surprise", "Mistress of Surprise"  },
      { "Contracted Killer", "Contracted Killer"      },
      { "Assassin", "Assassin"        },

      { "Master of Guile", "Mistress of Guile"  },
      { "Master of Severance", "Mistress of Severance"        },
      { "Master of the Blade", "Mistress of the Blade"        },
      { "Mad Knifer", "Mad Knifer"    },
      { "Slasher", "Slasher"  },

      { "Lord of the Night", "Lady of the Night"      },
      { "Prince of the Night", "Princess of the Night"        },
      { "King of the Night", "Queen of the Night"     },
      { "Overlord of Darkness", "Overlord of Darkness"        },
      { "Malevolent", "Malevolent"    },

      { "Sadist", "Sadist"    },
      { "Malignant", "Malignant"      },
      { "Conspirator", "Conspirator"  },
      { "Despised", "Despised"        },
      { "Forsaken", "Forsaken"        },

      { "Unseen", "Unseen"    },
      { "Unheard", "Unheard"  },
      { "Unknown", "Unknown"  },
      { "Unspeakable Thing", "Unspeakable Thing"      },
      { "Unholy Terror", "Unholy Terror"      },


      { "Hero of the Dark",           "Heroine of the Dark"                  },
      { "Avatar",            "Avatar"               },
      { "Angel",             "Angel"                },
      { "Demigod",           "Demigoddess"          },
      { "Immortal",          "Immortal"             },
      { "God",               "Goddess"              },
      { "Deity",             "Deity"                },
      { "Supremity",         "Supremity"            },
      { "Creator",            "Creator"            },
      { "Implementor",        "Implementress"            }
   },
   {
      { "Man",            "Woman"                },

      { "Apprentice of Black Magic", "Apprentice of Black Magic" },
      { "Student of Dark Spells", "Student of Dark Spells" },
      { "Scholar of Dark Magic", "Scholar of Dark Magic" },
      { "Delver in Dark Spells", "Delver in Dark Spells" },
      { "Channeler of Dark Forces", "Channeler of Dark Forces" },

      { "Scribe of Black Magic", "Scribe of Black Magic" },
      { "Dark Seer", "Dark Seer" },
      { "Dark Sage", "Dark Sage" },
      { "Apprentice of Dark Illusions", "Apprentice of Dark Illusions" },
      { "Student of Dark Illusions", "Student of Dark Illusions" },

      { "Dark Illusionist", "Dark Illusionist" },
      { "Master of Dark Illusions", "Mistress of Dark Illusions" },
      { "Conjurer of Evil", "Conjurer of Evil" },
      { "Evil Magician", "Evil Magician" },
      { "Evil Creator", "Evil Creator" },

      { "Evil Savant", "Evil Savant" },
      { "Warlock", "Witch" },
      { "Sorcerer", "Sorceress" },
      { "Grand Sorcerer", "Grand Sorceress" },
      { "Apprentice Golem Maker", "Apprentice Golem Maker" },

      { "Golem Maker", "Golem Maker" },
      { "Creator of Disease", "Creator of Disease" },
      { "Hurler of Acid", "Hurler of Acid" },
      { "Ghostly Illusionist", "Ghostly Illusionist" },
      { "Apprentice Animator", "Apprentice Animator" },

      { "Animator", "Animator" },
      { "Archmage of Animation", "Archmage of Animation" },
      { "Bringer of Plague", "Bringer of Plague" },
      { "Necromancer", "Necromancer" },
      { "Apprentice Summoner", "Apprentice Summoner" },

      { "Summoner", "Summoner" },
      { "Archmage of Summoning", "Archmage of Summoning" },
      { "Apprentice Summoner of Demons", "Apprentice Summoner of Demons" },
      { "Summoner of Demons", "Summoner of Demons" },
      { "Archmage Summoner of Demons", "Archmage Summoner of Demons" },

      { "Abominable Archmage", "Abominable Archmage" },
      { "Malefic Archmage", "Malefic Archmage" },
      { "Infernal Archmage", "Infernal Archmage" },
      { "Malevolent Archmage", "Malevolent Archmage" },
      { "Apprentice Occultist", "Apprentice Occultist" },

      { "Student of the Occult", "Student of the Occult" },
      { "Occultist", "Occultist" },
      { "Archmage of the Occult", "Archmage of the Occult" },
      { "Student of Black Magic", "Student of Black Magic" },
      { "Archmage of Black Magic", "Archmage of Black Magic" },

      { "Archmage of Necromancy", "Archmage of Necromancy" },
      { "Spectral Archmage", "Spectral Archmage" },
      { "Archmagical Wraith", "Archmagical Wraith" },
      { "Lich", "Lich" },
      { "Lich Lord", "Lich Lady" },

      { "Anti-Hero of Death",              "Anti-Heroine of Death"      },
      { "Avatar",            "Avatar"               },
      { "Angel",             "Angel"                },
      { "Demigod",           "Demigoddess"          },
      { "Immortal",          "Immortal"             },
      { "God",               "Goddess"              },
      { "Deity",             "Deity"                },
      { "Supremity",         "Supremity"            },
      { "Creator",            "Creator"            },
      { "Implementor",        "Implementress"            }
   },
   {
      { "Man",            "Woman"                },

      { "Apprentice of Magic", "Apprentice of Magic" },
      { "Spell Student", "Spell Student" },
      { "Scholar of Magic", "Scholar of Magic" },
      { "Delver in Spells", "Delver in Spells" },
      { "Medium of Magic", "Medium of Magic" },

      { "Scribe of Magic", "Scribe of Magic" },
      { "Seer", "Seer" },
      { "Sage", "Sage" },
      { "Illusionist", "Illusionist" },
      { "Abjurer", "Abjurer" },

      { "Invoker", "Invoker" },
      { "Conjurer", "Conjurer" },
      { "Magician", "Magician" },
      { "Creator", "Creator" },
      { "Savant", "Savant" },

      { "Magus", "Magus" },
      { "Wizard", "Witch" },
      { "Warlock", "War Witch" },
      { "Elemental Apprentice", "Elemental Apprentice" },
      { "Elemental Magician", "Elemental Magician" },

      { "Student of the Elements", "Student of the Elements" },
      { "Student of Fire", "Student of Fire" },
      { "Student of Ice", "Student of Ice" },
      { "Student of Earth", "Student of Earth" },
      { "Student of Water", "Student of Water" },

      { "Student of Air", "Student of Air" },
      { "Student of Ether", "Student of Ether" },
      { "Student of Conjuration", "Student of Conjuration" },
      { "Student of Abjuration", "Student of Abjuration" },
      { "Elder Elemental Magician", "Elder Elemental Magician" },

      { "Learned of the Elements", "Learned of the Elements" },
      { "Master of Fire", "Mistress of Fire" },
      { "Master of Ice", "Mistress of Ice" },
      { "Master of Earth", "Mistress of Earth" },
      { "Master of Water", "Mistress of Water" },

      { "Master of Air", "Mistress of Air" },
      { "Master of Ether", "Mistress of Ether" },
      { "Master of Conjuration", "Mistress of Conjuration" },
      { "Master of Abjuration", "Mistress of Abjuration" },
      { "Master of the Elements", "Mistress of the Elements" },

      { "Greater Elemental Magician", "Greater Elemental Magician" },
      { "Master of Alteration", "Mistress of Alteration" },
      { "Master of Evocation", "Mistress of Evocation" },
      { "Master of Creation", "Mistress of Creation" },
      { "Grand Elemental Magician", "Grand Elemental Magician" },

      { "Commander of Air", "Commander of Air" },
      { "Summoner of Water", "Summoner of Water" },
      { "Caller of Fire", "Caller of Fire" },
      { "Wielder of Earth", "Wielder of Earth" },
      { "Grand Master of the Elements", "Grand Mistress of the Elements" },

      { "Elemental Hero",             "Elemental Heroine"                  },
      { "Avatar",                "Avatar"                   },
      { "Angel",            "Angel"                },
      { "Demigod",            "Demigoddess",            },
      { "Immortal",            "Immortal"            },
      { "God",            "Goddess"            },
      { "Deity",            "Deity"                },
      { "Supreme Master",        "Supreme Mistress"        },
      { "Creator",                    "Creator"                       },
      { "Implementor",        "Implementress"            }
   }
};



/*
* Attribute bonus tables.
*/
const    struct    str_app_type    str_app        [26]        =
{
   { -5, -4,   0,  0 },  /* 0  */
   { -5, -4,   3,  1 },  /* 1  */
   { -3, -2,   3,  2 },
   { -3, -1,  10,  3 },  /* 3  */
   { -2, -1,  25,  4 },
   { -2, -1,  55,  5 },  /* 5  */
   { -1,  0,  80,  6 },
   { -1,  0, 100,  7 },
   {  0,  0, 150,  8 },
   {  0,  0, 180,  9 },
   {  0,  0, 200, 10 }, /* 10  */
   {  0,  0, 215, 11 },
   {  0,  0, 230, 12 },
   {  0,  0, 230, 13 }, /* 13  */
   {  0,  1, 240, 14 },
   {  1,  1, 250, 15 }, /* 15  */
   {  1,  2, 265, 16 },
   {  2,  3, 280, 22 },
   {  2,  3, 300, 25 }, /* 18  */
   {  3,  4, 325, 30 },
   {  3,  5, 350, 35 }, /* 20  */
   {  4,  6, 400, 40 },
   {  4,  6, 450, 45 },
   {  5,  7, 500, 50 },
   {  5,  8, 550, 55 },
   {  6,  9, 600, 60 }  /* 25   */
};



const    struct    int_app_type    int_app        [26]        =
{
   {  3 },    /*  0 */
   {  5 },    /*  1 */
   {  7 },
   {  8 },    /*  3 */
   {  9 },
   { 10 },    /*  5 */
   { 11 },
   { 12 },
   { 13 },
   { 15 },
   { 17 },    /* 10 */
   { 19 },
   { 22 },
   { 25 },
   { 28 },
   { 31 },    /* 15 */
   { 34 },
   { 37 },
   { 40 },    /* 18 */
   { 44 },
   { 49 },    /* 20 */
   { 55 },
   { 60 },
   { 70 },
   { 80 },
   { 85 }    /* 25 */
};



const    struct    wis_app_type    wis_app        [26]        =
{
   { 0 },    /*  0 */
   { 0 },    /*  1 */
   { 0 },
   { 0 },    /*  3 */
   { 0 },
   { 1 },    /*  5 */
   { 1 },
   { 1 },
   { 1 },
   { 1 },
   { 1 },    /* 10 */
   { 1 },
   { 1 },
   { 1 },
   { 1 },
   { 2 },    /* 15 */
   { 2 },
   { 2 },
   { 3 },    /* 18 */
   { 3 },
   { 3 },    /* 20 */
   { 3 },
   { 4 },
   { 4 },
   { 4 },
   { 5 }    /* 25 */
};



const    struct    dex_app_type    dex_app        [26]        =
{
   {   60, 0 },   /* 0 */
   {   50, 0 },   /* 1 */
   {   50, 0 },
   {   40, 0 },
   {   30, 0 },
   {   20, 0 },   /* 5 */
   {   10, 0 },
   {    0, 0 },
   {    0, 0 },
   {    0, 0 },
   {    0, 0 },   /* 10 */
   {    0, 1 },
   {    0, 1 },
   {    0, 1 },
   {    0, 2 },
   { - 10, 2 },   /* 15 */
   { - 15, 2 },
   { - 20, 3 },
   { - 30, 3 },
   { - 40, 4 },
   { - 50, 4 },   /* 20 */
   { - 60, 5 },
   { - 75, 6 },
   { - 90, 7 },
   { -105, 8 },
   { -120, 9 }    /* 25 */
};


const    struct    con_app_type    con_app        [26]        =
{
   { -4, 20 },   /*  0 */
   { -3, 25 },   /*  1 */
   { -2, 30 },
   { -2, 35 },      /*  3 */
   { -1, 40 },
   { -1, 45 },   /*  5 */
   { -1, 50 },
   {  0, 55 },
   {  0, 60 },
   {  0, 65 },
   {  0, 70 },   /* 10 */
   {  0, 75 },
   {  0, 80 },
   {  0, 85 },
   {  0, 88 },
   {  1, 90 },   /* 15 */
   {  2, 95 },
   {  2, 97 },
   {  3, 99 },   /* 18 */
   {  3, 99 },
   {  4, 99 },   /* 20 */
   {  4, 99 },
   {  5, 99 },
   {  6, 99 },
   {  7, 99 },
   {  8, 99 }    /* 25 */
};



/*
* Liquid properties.
* Used in world.obj.
* Not const anymore, cause of the immrp blood command - Werv
*/
const struct  liq_type        liq_table       []      =
{
   /*    name            color                   proof, full, thirst, food, ssize */
   { "water",              "clear",              {   0,   1,    10,   0,  16}, FALSE },

   { "murky water",        "cloudy",             {   0,   1,    10,   1,  16}, FALSE },
   { "beer",               "amber",              {  12,   1,     8,   1,  12}, FALSE },
   { "red wine",           "burgundy",           {  30,   1,     8,   1,   5}, FALSE },
   { "ale",                "brown",              {  15,   1,     8,   1,  12}, FALSE },
   { "dark ale",           "dark",               {  16,   1,     8,   1,  12}, FALSE },

   { "whiskey",            "golden",             { 120,   1,     5,   0,   2}, FALSE },
   { "lemonade",           "pink",               {   0,   1,     9,   2,  12}, FALSE },
   { "firebreather",       "boiling",            { 190,   0,     4,   0,   2}, FALSE },
   { "local specialty",    "clear",              { 151,   1,     3,   0,   2}, FALSE },
   { "slime mold juice",   "green",              {   0,   2,    -8,   1,   2}, FALSE },

   { "milk",               "white",              {   0,   2,     9,   3,  12}, TRUE  },
   { "tea",                "tan",                {   0,   1,     8,   0,   6}, FALSE },
   { "coffee",             "black",              {   0,   1,     8,   0,   6}, FALSE },
   { "blood",              "red",                {   0,   2,    -1,   2,   6}, FALSE },
   { "salt water",         "clear",              {   0,   1,    -2,   0,   1}, FALSE },

   { "grog",               "brown",              { 200,   2,     9,   2,  12}, FALSE },
   { "pomegranate juice",  "golden",             {   0,   2,     9,   2,  12}, FALSE },
   { "elvish wine",        "green",              {  35,   2,     8,   1,   5}, FALSE },
   { "white wine",         "golden",             {  28,   1,     8,   1,   5}, FALSE },
   { "champagne",          "golden",             {  32,   1,     8,   1,   5}, FALSE },

   { "mead",               "honey-colored",      {  34,   2,     8,   2,  12}, FALSE },
   { "rose wine",          "pink",               {  26,   1,     8,   1,   5}, FALSE },
   { "benedictine wine",   "burgundy",           {  40,   1,     8,   1,   5}, FALSE },
   { "vodka",              "clear",              { 130,   1,     5,   0,   2}, FALSE },
   { "cranberry juice",    "red",                {   0,   1,     9,   2,  12}, FALSE },

   { "orange juice",       "orange",             {   0,   2,     9,   3,  12}, FALSE },
   { "absinthe",           "green",              { 200,   1,     4,   0,   2}, FALSE },
   { "brandy",             "golden",             {  80,   1,     5,   0,   4}, FALSE },
   { "aquavit",            "clear",              { 140,   1,     5,   0,   2}, FALSE },
   { "schnapps",           "clear",              {  90,   1,     5,   0,   2}, FALSE },

   { "icewine",            "purple",             {  50,   2,     6,   1,   5}, FALSE },
   { "amontillado",        "burgundy",           {  35,   2,     8,   1,   5}, FALSE },
   { "sherry",             "red",                {  38,   2,     7,   1,   5}, FALSE },
   { "framboise",          "red",                {  50,   1,     7,   1,   5}, FALSE },
   { "rum",                "amber",              { 151,   1,     4,   0,   2}, FALSE },

   { "eggnog",             "white",              {  25,   1,     4,  24,  12}, FALSE },
   { "wassail",            "brown",              { 100,   1,     8,   1,  12}, FALSE },
   { "cordial",            "clear",              { 100,   1,     5,   0,   2}, FALSE },
   { "lifeblood",          "deep-red",           {   0,   2,    -1,   2,   6}, FALSE },
   { "Aquilaen Aquifier",  "clear",              {   6,   1,    10,   1,  16}, FALSE },

   { "Strawberry Sylvara", "milky pink",         {  14,   2,     2,   3,  16}, FALSE },
   { "Sherry Shandril",    "red-tinted clear",   {  84,   2,     4,   2,   6}, FALSE },
   { "Eluria",             "golden cherry",      {  20,   2,     4,   8,  16}, FALSE },
   { "Shortleg Ale",       "golden",             { 190,   3,     6,   2,  16}, FALSE },
   { "Hellshot",           "clear green",        { 198,   1,     6,   1,   2}, FALSE },

   { "Snake Bite",         "light amber",        { 195,   2,     6,   1,   2}, FALSE },
   { "barrel aged ale",    "dark brown",         {  25,   1,     8,   1,  12}, FALSE },
   { "blended rum",        "dark amber",         { 160,   1,     4,   0,   2}, FALSE },
   { "elvish white wine",  "amber-tinted clear", {  20,   2,     8,   1,   5}, FALSE },
   { "elvish red wine",    "deep red",           {  15,   2,     8,   1,   5}, FALSE },

   { "elvish rose wine",   "rose-tinted clear",  {  18,   2,     8,   1,   5}, FALSE },
   { "caudell",            "creamy pink",        {  20,   6,     5,   6,  12}, FALSE },
   { "strawberry juice",   "pink",               {   0,   1,     7,   2,  12}, FALSE },
   { "chocolate milk",     "light brown",        {   0,   1,     7,   2,  12}, TRUE  },
   { "strawberry milk",    "whitish pink",       {   0,   1,     7,   2,  12}, TRUE  },

   { "honey mead",         "honey-colored",      {  34,   2,     8,   9,  12}, FALSE },
   { "honeysuckle nectar", "honey-colored",      {   0,   1,     7,   2,  12}, FALSE },

   { "raki",               "white",              { 120,   2,     7,   7,   2}, FALSE },
   { "turnip juice",       "dark red",           {   0,   1,     7,   5,   5}, FALSE },

   { "green tea",          "green",              {   0,   1,     7,   5,   5}, FALSE },
   { "Drinlinda's sacred blood",  "red",         {   0,   1,     7,   2,  12}, FALSE },
   { "Blood of the Asp",   "red",                {   0,   1,     7,   2,  12}, FALSE },
   { NULL,                 NULL,                 {   0,   0,     0,   0,   0}, FALSE }
};



/*
* The skill and spell table.
*/

/* Commented out skills, so they do not interfere with tablecheck */
/* Awaken - Cirdan - Commented out for now
   {
      "awaken",               { 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53 },
      {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
      spell_awaken,           TAR_IGNORE,             POS_STANDING,
      NULL,                   25,     84,
      "",                     "You feel ready to awaken others again.",
      ""
   },
*/
/* Xeonauz's brand - Jord
   {
      "Sword of the Flame",  {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      spell_null,  TAR_IGNORE,  POS_DEAD,
      &gsn_xeonauz_sword,    0,  0,
      "",  "",  ""
   },
   {
      "Rage of War",  {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      spell_null,  TAR_IGNORE,  POS_DEAD,
      &gsn_war_rage,    0,  0,
      "",  "Your rage slowly fades.",  ""
   },
*/
/*
   {
      "transform",               { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10 },
      {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
      spell_transform,    TAR_CHAR_SELF,        POS_STANDING,
      NULL,                        125,    24,
      "",    "You feel your health return to normal.",    ""
   },
*/
/*
   {
      "noxious gas",  { 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 25},
      { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
      spell_noxious_gas,      TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
      NULL,                        20,     24,
      "deadly poison",        "You feel able to breath properly once again.", ""
   },
   {
      "thunderclap",  { 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 29},
      { 0, 0, 0, 0, 0, 0, 0, 0, 0 , 0, 1},
      spell_thunderclap,      TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
      NULL,                        50,     12,
      "thunderclap",          "The ringing in your ears dies away.", ""
      Add double lag till 15 levels if this is ever used
   },
*/
/*
   {
      "icestorm",  { 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 32},
      { 0, 0, 0, 0, 0, 0, 0, 0, 0 , 0, 1},
      spell_icestorm,      TAR_IGNORE,     POS_FIGHTING,
      NULL,                        40,     12,
      "sheets of ice",          "", ""
      Add double lag till 15 levels if this is ever used
   },
   {
      "raise hell",  { 53, 53, 53, 53, 40, 53, 53, 53, 53, 53, 53},
      { 0, 0, 0, 0, 1, 0, 0, 0, 0 , 0, 0},
      spell_raise_hell,      TAR_CHAR_OFFENSIVE,     POS_STANDING,
      NULL,                        80,     24,
      "unholy hellspawn",          "", ""
   },
*/
/*
   {
      "moonshine",  { 53, 53, 53, 53, 53, 53, 53, 53, 8, 53, 53},
      { 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
      spell_moonshine,      TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
      NULL,                        25,     12,
      "dark rays of light",          "", ""
   },
*/
/*
   {
      "slip",  { 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 12},
      { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
      spell_slip,      TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
      NULL,                        15,     12,
      "",          "", ""
   },
*/
/*
   {
      "morph dragon",        { 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 0, 0, 0, 0, 0, 0, 1, 0 , 0 , 0, 0},
      spell_null,        TAR_IGNORE,        POS_STANDING,
      &gsn_morph_dragon,            100,    56,
      "",            "You return to your natural form.", ""
   },
*/

const    struct   skill_type  skill_table [] =
{
   /*
   * Magic spells.
   * Skills and spells, numbers beside name designate classes and the rank
   * those classes get those skills
   * Format: { "NAME", {rank according to class table where the skill
   * is given}, {rate of improvement, again, according to class table},
   * castable or non castable (spell_null), target (TAR_CHAR_SELF,
   * TAR_OFFENSIVE, TAR_IGNORE), Position it can be used (POS_STANDING, POS
   * RESTING, POS_SLEEPING), GSN for fast lookup (or NULL), then mana cost,
   * then lag in pulses for using it
   * damage noun, wear off message for the affect, wear off message
   * for objects/room affects (the last one is the same for room/object)}
   */
   {
      "reserved",
      {99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99},
      {99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_reserved,
      0,
      0,
      "",
      "",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   {
      "absorb",
      {53, 53, 53, 53, 53, 53, 53, 33, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_absorb,
      TAR_CHAR_SELF,       POS_STANDING,
      &gsn_absorb,
      100,
      PULSE_VIOLENCE,
      "",
      "The shimmering fields around you fade.",
      "$p's shimmering field fades.",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "acid blast",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 28, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_acid_blast,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_acid_blast,
      20,
      PULSE_VIOLENCE,
      "acid blast",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "embalm",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 15, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_embalm,
      20,
      PULSE_VIOLENCE,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "trace",
      {25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_trace,
      40,
      PULSE_VIOLENCE,
      "",
      "You may send out your Justicar trainees out in search again.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "judgement",
      {15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_judgement,
      TAR_IGNORE,          POS_STANDING,
      NULL,
      20,
      PULSE_VIOLENCE,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "flame scorch",
      {38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_flame_scorch,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_flame_scorch,
      50,
      PULSE_VIOLENCE,
      "searing blue-green flame",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "flame scorch_blue",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_flame_scorch,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_flame_scorch_blue,
      0,
      0,
      "freezing blue flame",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "flame scorch_green",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_flame_scorch,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_flame_scorch_green,
      0,
      0,
      "scorching green flame",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "soulbind",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 40, 61},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_soulbind,
      TAR_CHAR_DEFENSIVE,  POS_STANDING,
      NULL,
      100,
      PULSE_VIOLENCE,
      "",
      "",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "shattered bone",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_DEAD,
      &gsn_shattered_bone,
      0,
      0,
      "fragments of blasted bone",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "blood tide",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_DEAD,
      &gsn_blood_tide,
      0,
      0,
      "bloody tide",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   /* const.c skill table stuff for flesh-cutter */
   {
      "mutilated left hand",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_DEAD,
      &gsn_mutilated_left_hand,
      0,
      0,
      "",
      "The feeling in your mangled left hand returns.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "mutilated right hand",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_DEAD,
      &gsn_mutilated_right_hand,
      0,
      0,
      "",
      "The feeling in your mangled right hand returns.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "mutilated left arm",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_DEAD,
      &gsn_mutilated_left_arm,
      0,
      0,
      "",
      "The feeling in your mangled left arm returns.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "mutilated right arm",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_DEAD,
      &gsn_mutilated_right_arm,
      0,
      0,
      "",
      "The feeling in your mangled right arm returns.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "mutilated left leg",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_DEAD,
      &gsn_mutilated_left_leg,
      0,
      0,
      "",
      "The feeling in your mangled left leg returns.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "mutilated right leg",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_DEAD,
      &gsn_mutilated_right_leg,
      0,
      0,
      "",
      "The feeling in your mangled right leg returns.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "talk to dead",
      {52, 52, 18, 24, 52, 52, 52, 52, 52, 21, 52},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_talk_to_dead,
      TAR_IGNORE,          POS_STANDING,
      NULL,
      20,
      PULSE_VIOLENCE,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "deathspell",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 35, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_deathspell,
      TAR_IGNORE,          POS_STANDING,
      &gsn_deathspell,
      150,
      PULSE_VIOLENCE,
      "explosion of negative energy",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "trinal wield",
      {25, 30, 53, 30, 32, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_tertiary_wield,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "forest blending",
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_forest_blending,
      0,
      0,
      "",
      "You lose your focus and stop attempting to blend with your"
      " surroundings.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "awareness",
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_awareness,
      0,
      0,
      "",
      "Your awareness of the surrounding trees lessens.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "door bash",
      {18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_door_bash,
      0,
      0,
      "failed door bashing",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "gaseous form",
      {25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_gaseous_form,
      0,
      0,
      "",
      "You come out of gaseous form.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "breath attack",
      {20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_breath_fire,
      0,
      PULSE_VIOLENCE,
      "blast of flame",
      "You are ready to use your breath weapon again.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "regeneration",
      { 5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_regeneration,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "War Brand",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_DEAD,
      &gsn_wolverine_brand,
      0,
      0,
      "",
      "The fire coursing through your blood cools down.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   /* Hector's brand - Wicket */
   {
      "Spark of Innovation",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_DEAD,
      &gsn_hector_brand_spark,
      0,
      0,
      "mercuric shock",
      "The energy infusing you subsides.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   {
      "Essence Identity",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_DEAD,
      &gsn_hector_brand_essence,
      0,
      0,
      "",
      "Your ability to discern essence returns to you.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   /* Lestregus's brand - Maelstrom */
   {
      "Burnt Sigil",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,
      POS_DEAD,
      &gsn_lestregus_brand_eternal,
      0,
      0,
      "flame of the damned",
      "The torment of your mind subsides as your eyes return to normal.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   {
      "Imbue Affliction",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,
      POS_DEAD,
      &gsn_lestregus_brand_taint,
      0,
      0,
      "flame of the damned",
      "The power to blacken the hearts of the innocent returns to you.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   {
      "Thirst of Affliction",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,
      POS_DEAD,
      &gsn_lestregus_brand_thirst,
      0,
      0,
      "brand of a Burnt Sigil",
      "",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   /* sham's brand effects */
   {
      "Delusions of the Dove",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_DEAD,
      &gsn_delusions,
      0,
      0,
      "",
      "The illusions fade away in a violet mist.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   {
      "Mosaic Dove",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_DEAD,
      &gsn_sham_brand2,
      0,
      0,
      "",
      "You can once again call upon the Dove for Delusions.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   {
      "Dove Brand",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_DEAD,
      &gsn_sham_brand1,
      0,
      0,
      "",
      "The violet aura about your body fades.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   /* Utara's brand - Wicket */
   {
      "Blur of the Gremlin",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_DEAD,
      &gsn_utara_brand_blur,
      0,
      0,
      "",
      "Your body begins to focus once more.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   {
      "Gremlin Brand",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_DEAD,
      &gsn_utara_brand2,
      0,
      0,
      "",
      "You can once more call upon the gremlin.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   {
      "silence",
      {25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_DEAD,
      &gsn_mute,
      0,
      0,
      "",
      "The pain in your throat subsides and you are slowly able to speak"
      " normally again.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "brand",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_DEAD,
      &gsn_parrot,
      0,
      0,
      "brand of the silent parrot",
      "You feel once again ready to call upon your god.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   {
      "hunger",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_DEAD,
      &gsn_starvation,
      0,
      0,
      "starvation",
      "You no longer feel the pangs of famine.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "dehydrated",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_DEAD,
      &gsn_dehydrated,
      0,
      0,
      "dehydration",
      "You no longer feel so thirsty.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "rear kick",
      {18, 53, 27, 23, 53, 20, 32, 32, 32, 32, 32},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_rear_kick,
      0,
      0,
      "rear kick",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "psionic blast",
      {22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_psionic_blast,
      0,
      PULSE_VIOLENCE * 2,
      "psionic blast",
      "You can once again project a psionic blast.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "spike",
      {52, 52, 52, 29, 52, 52, 52, 52, 52, 52, 52},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_spike,
      0,
      0,
      "spike",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "shapeshift",
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_shapeshift,
      0,
      0,
      "",
      "You resume your natural form.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "feign death",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_feign_death,
      0,
      0,
      "",
      "You are no longer feigning death.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "globe of darkness",
      {20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_globe_of_darkness,
      TAR_IGNORE,          POS_STANDING,
      &gsn_globe_darkness,
      100,
      PULSE_VIOLENCE,
      "",
      "Your power to call upon the darkness returns.",
      "The darkness lifts.",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "animate object",
      {53, 53, 53, 53, 53, 53, 53, 35, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_animate_object,
      TAR_OBJ_INV,         POS_STANDING,
      NULL,
      100,
      PULSE_VIOLENCE,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "lifebane",
      {53, 53, 53, 53, 53, 53, 53, 53, 34, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_lifebane,
      TAR_IGNORE,          POS_FIGHTING,
      NULL,
      25,
      PULSE_VIOLENCE,
      "lifebane",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "demand",
      {53, 53, 53, 53, 30, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_demand,
      0,
      PULSE_VIOLENCE,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "animate dead",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 25, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_animate_dead,
      TAR_IGNORE,          POS_STANDING,
      &gsn_animate_dead,
      100,
      PULSE_VIOLENCE * 3,
      "",
      "Your power to raise dead returns.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "lesser golem",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 20, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_lesser_golem,
      TAR_IGNORE,          POS_STANDING,
      NULL,
      70,
      PULSE_VIOLENCE * 2,
      "",
      "You feel up to fashioning another flesh golem.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "graft flesh",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_graft_flesh,
      TAR_CHAR_DEFENSIVE,  POS_STANDING,
      NULL,
      25,
      PULSE_VIOLENCE * 2,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "greater golem",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 33, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_greater_golem,
      TAR_IGNORE,          POS_STANDING,
      NULL,
      70,
      PULSE_VIOLENCE * 2,
      "",
      "You feel up to fashioning another iron golem.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "armor",
      {53, 53,  1,  5,  5,  7, 53,  1,  8,  1,  1},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_armor,
      TAR_CHAR_DEFENSIVE,  POS_STANDING,
      &gsn_armor,
      5,
      PULSE_VIOLENCE,
      "",
      "You feel less armored.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "enlarge",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_enlarge,
      TAR_CHAR_DEFENSIVE,  POS_STANDING,
      &gsn_enlarge,
      25,
      PULSE_VIOLENCE,
      "",
      "Your size returns to normal.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "shrink",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_shrink,
      TAR_CHAR_OFFENSIVE,  POS_STANDING,
      &gsn_shrink,
      25,
      PULSE_VIOLENCE,
      "",
      "Your size returns to normal.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "toad",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_toad,
      TAR_CHAR_OFFENSIVE,  POS_STANDING,
      &gsn_toad,
      100,
      PULSE_VIOLENCE * 2,
      "",
      "You return to normal.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "lower resistance",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_lower_resist,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_lower_resistance,
      25,
      PULSE_VIOLENCE,
      "",
      "Your magical resistance returns to normal.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "raise resistance",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_raise_resist,
      TAR_CHAR_DEFENSIVE,  POS_FIGHTING,
      &gsn_raise_resistance,
      25,
      PULSE_VIOLENCE,
      "",
      "Your magical resistance returns to normal.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "benediction",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_benediction,
      TAR_CHAR_DEFENSIVE,  POS_STANDING,
      &gsn_benediction,
      25,
      PULSE_VIOLENCE,
      "",
      "You feel the effects of your benediction subside.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "bless",
      {53, 53,  3,  4, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_bless,
      TAR_CHAR_DEFENSIVE,  POS_STANDING,
      &gsn_bless,
      5,
      PULSE_VIOLENCE,
      "",
      "You feel less righteous.",
      "$p's holy aura fades.",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "blindness",
      {53, 53, 11, 53,  9, 53, 53, 53, 12, 12, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_blindness,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_blindness,
      5,
      PULSE_VIOLENCE * 3 / 2,
      "",
      "You can see again.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "temporal shear",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_temporal_shear,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_temporal_shear,
      50,
      PULSE_VIOLENCE * 3 / 4,
      "temporal shearing",
      "You stop reliving the past.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "strabismus",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_strabismus,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_strabismus,
      25,
      PULSE_VIOLENCE * 3 / 2,
      "",
      "The images fade back together.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "burning hands",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_burning_hands,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      NULL,
      15,
      PULSE_VIOLENCE,
      "burning hands",
      "!Burning Hands!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "call lightning",
      {53, 53, 27, 53, 53, 22, 53, 53, 53, 53, 16},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_call_lightning,
      TAR_IGNORE,          POS_FIGHTING,
      NULL,
      15,
      PULSE_VIOLENCE,
      "lightning bolt",
      "!Call Lightning!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "call slayer",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_call_slayer,
      TAR_IGNORE,          POS_FIGHTING,
      NULL,
      25,
      PULSE_VIOLENCE,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "calm",
      {53, 53, 17, 14, 53, 53, 53, 53, 53, 53, 14},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_calm,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_calm,
      30,
      PULSE_VIOLENCE,
      "",
      "You have lost your peace of mind.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "cancellation",
      {53, 53, 19, 53, 53, 53, 53, 16, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_cancellation,
      TAR_CHAR_DEFENSIVE,  POS_FIGHTING,
      &gsn_cancellation,
      20,
      PULSE_VIOLENCE,
      "!cancellation!",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "cause critical",
      {53, 53, 18, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_cause_critical,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      NULL,
      20,
      PULSE_VIOLENCE,
      "divine strike",
      "!Cause Critical!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "cause light",
      {53, 53,  3, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_cause_light,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      NULL,
      15,
      PULSE_VIOLENCE,
      "divine strike",
      "!Cause Light!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "cause serious",
      {53, 53, 12, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_cause_serious,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      NULL,
      17,
      PULSE_VIOLENCE,
      "divine strike",
      "!Cause Serious!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "chain lightning",
      {53, 53, 53, 53, 53, 53, 53, 26, 53, 53, 29},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_chain_lightning,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_chain_lightning,
      25,
      PULSE_VIOLENCE,
      "lightning",
      "!Chain Lightning!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "change sex",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_change_sex,
      TAR_CHAR_DEFENSIVE,  POS_FIGHTING,
      &gsn_change_sex,
      15,
      PULSE_VIOLENCE,
      "",
      "Your body feels familiar again.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "rat call",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_SLEEPING,
      &gsn_vermin_timer,
      0,
      0,
      "",
      "You can once more call upon vermin.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "charm timer",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_SLEEPING,
      &gsn_charm_timer,
      0,
      0,
      "",
      "You feel susceptable to charm again.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "charm person",
      {53, 53, 53, 53, 34, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_charm_person,
      TAR_CHAR_OFFENSIVE,  POS_STANDING,
      &gsn_charm_person,
      5,
      PULSE_VIOLENCE,
      "",
      "You feel more self-confident.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "recruit",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_recruit,
      75,
      PULSE_VIOLENCE,
      "",
      "You feel ready to recruit again.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "propaganda",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_propaganda,
      TAR_CHAR_DEFENSIVE,  POS_FIGHTING,
      &gsn_propaganda,
      25,
      PULSE_VIOLENCE * 2,
      "",
      "Your rebellious frenzy subsides.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "kidnap",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_kidnap,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_kidnap,
      25,
      PULSE_VIOLENCE * 2,
      "",
      "You sense an abduction in the near future.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "aura of defiance",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_aura_defiance,
      TAR_CHAR_SELF,       POS_STANDING,
      &gsn_aura_defiance,
      75,
      PULSE_VIOLENCE * 2,
      "",
      "You are no longer showing your defiance",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "aura of presence",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_aura_of_presence,
      TAR_CHAR_SELF,       POS_STANDING,
      &gsn_aura_of_presence,
      75,
      PULSE_VIOLENCE * 2,
      "",
      "The aura of presence is not felt anymore.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "chill touch",
      {53, 53, 53, 53,  3, 53, 53, 53, 53,  3, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_chill_touch,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_chill_touch,
      15,
      PULSE_VIOLENCE,
      "chilling touch",
      "You feel less cold.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "colour spray",
      {53, 53, 53, 53, 53, 53, 53, 13, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_colour_spray,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_colour_spray,
      15,
      PULSE_VIOLENCE,
      "colour spray",
      "!Colour Spray!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "ice pox",
      {53, 53, 53, 53, 53, 53, 53, 53, 15, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_ice_pox,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      NULL,
      20,
      PULSE_VIOLENCE,
      "ice pox",
      "The chill lifts from your bones",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "firestream",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 15},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_firestream,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_firestream,
      20,
      PULSE_VIOLENCE,
      "firestream",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "fireshield",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 29},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_fireshield,
      TAR_IGNORE,          POS_STANDING,
      &gsn_fireshield,
      60,
      PULSE_VIOLENCE,
      "",
      "Your flaming shield dissipates.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "iceshield",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 29},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_iceshield,
      TAR_IGNORE,          POS_STANDING,
      &gsn_iceshield,
      60,
      PULSE_VIOLENCE,
      "",
      "Your frost shield dissipates.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "concatenate",
      {53, 53, 53, 53, 53, 53, 53, 29, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_concatenate,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_concatenate,
      30,
      PULSE_VIOLENCE,
      "blast of energy",
      "!Concatenate!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "heavenly wrath",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_heavenly_sceptre_frenzy,
      TAR_CHAR_SELF,       POS_FIGHTING,
      &gsn_heavenly_wrath,
      0,
      0,
      "",
      "The heavenly wrath bestowed upon you fades away.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "heavenly fire",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_heavenly_sceptre_fire,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_heavenly_fire,
      0,
      0,
      "heavenly fire",
      "You feel the power drained from you by the scetpre's heavenly fire"
      " return.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "wrath",
      {53, 53, 53, 21, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_wrath,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_wrath,
      20,
      PULSE_VIOLENCE,
      "heavenly wrath",
      "!wrath!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "spiritual wrath",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_spiritual_wrath,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      NULL,
      20,
      PULSE_VIOLENCE,
      "spiritual wrath",
      "!spiritual wrath!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "continual light",
      {53, 53, 53, 53, 53, 53, 53, 12, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_continual_light,
      TAR_IGNORE,          POS_STANDING,
      NULL,
      7,
      PULSE_VIOLENCE,
      "",
      "!Continual Light!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "control weather",
      {53, 53, 53, 53, 53, 20, 53, 53, 53, 53, 14},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_control_weather,
      TAR_IGNORE,          POS_STANDING,
      NULL,
      25,
      PULSE_VIOLENCE,
      "",
      "!Control Weather!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "create food",
      {53, 53,  7, 11, 53, 53, 53, 12, 53, 53,  9},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_create_food,
      TAR_IGNORE,          POS_STANDING,
      &gsn_create_food,
      5,
      PULSE_VIOLENCE,
      "",
      "!Create Food!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "create rose",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_create_rose,
      TAR_IGNORE,          POS_STANDING,
      NULL,
      30,
      PULSE_VIOLENCE,
      "",
      "!Create Rose!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "create red flower",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_create_red_flower,
      TAR_IGNORE,          POS_STANDING,
      NULL,
      30,
      PULSE_VIOLENCE,
      "",
      "!Create Red Flower!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "create blue flower",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_create_blue_flower,
      TAR_IGNORE,          POS_STANDING,
      NULL,
      30,
      PULSE_VIOLENCE,
      "",
      "!Create Blue Flower!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "create spring",
      {53, 53, 16, 18, 53, 53, 53, 53, 53, 53,  9},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_create_spring,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_create_spring,
      20,
      PULSE_VIOLENCE,
      "",
      "!Create Spring!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "create water",
      {53, 53,  8, 12, 53, 53, 53,  9, 53, 53,  8},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_create_water,
      TAR_OBJ_INV,         POS_STANDING,
      &gsn_create_water,
      5,
      PULSE_VIOLENCE,
      "",
      "!Create Water!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "cure blindness",
      {53, 53, 13, 15, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_cure_blindness,
      TAR_CHAR_DEFENSIVE,  POS_FIGHTING,
      &gsn_cure_blindness,
      5,
      PULSE_VIOLENCE,
      "",
      "!Cure Blindness!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "cure critical",
      {53, 53, 18, 20, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_cure_critical,
      TAR_CHAR_HEALING,    POS_FIGHTING,
      &gsn_cure_critical,
      20,
      PULSE_VIOLENCE,
      "",
      "!Cure Critical!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "cleanse",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_cleanse,
      TAR_CHAR_DEFENSIVE,  POS_STANDING,
      NULL,
      40,
      PULSE_VIOLENCE,
      "",
      "!Cleanse!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "cure disease",
      {53, 53, 15, 18, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_cure_disease,
      TAR_CHAR_DEFENSIVE,  POS_STANDING,
      &gsn_cure_disease,
      20,
      PULSE_VIOLENCE,
      "",
      "!Cure Disease!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "cure light",
      {53, 53,  3,  6, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_cure_light,
      TAR_CHAR_HEALING,    POS_FIGHTING,
      &gsn_cure_light,
      10,
      PULSE_VIOLENCE,
      "",
      "!Cure Light!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "cure poison",
      {53, 53, 13, 19, 53, 53, 53, 53, 53, 53, 16},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_cure_poison,
      TAR_CHAR_DEFENSIVE,  POS_STANDING,
      &gsn_cure_poison,
      5,
      PULSE_VIOLENCE,
      "",
      "!Cure Poison!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "cure serious",
      {53, 53,  9, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_cure_serious,
      TAR_CHAR_HEALING,    POS_FIGHTING,
      &gsn_cure_serious,
      15,
      PULSE_VIOLENCE,
      "",
      "!Cure Serious!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "curse",
      {53, 53, 20, 53, 17, 53, 53, 53, 53, 16, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_curse,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_curse,
      20,
      PULSE_VIOLENCE,
      "curse",
      "The curse wears off.",
      "$p is no longer impure.",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "dark wrath",
      {53, 53, 53, 53, 53, 53, 53, 53, 22, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_dark_wrath,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_dark_wrath,
      20,
      PULSE_VIOLENCE,
      "unholy dark wrath",
      "!DarkWrath!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "demonfire",
      {53, 53, 30, 53, 53, 53, 53, 53, 33, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_demonfire,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_demonfire,
      20,
      PULSE_VIOLENCE,
      "torments",
      "!Demonfire!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "detect evil",
      {53, 53, 53, 10, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_detect_evil,
      TAR_CHAR_SELF,       POS_STANDING,
      &gsn_detect_evil,
      5,
      PULSE_VIOLENCE,
      "",
      "The red in your vision disappears.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "detect good",
      {53, 53, 53, 53, 10, 53, 53, 53, 53,  2, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_detect_good,
      TAR_CHAR_SELF,       POS_STANDING,
      &gsn_detect_good,
      5,
      PULSE_VIOLENCE,
      "",
      "The gold in your vision disappears.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "detect hidden",
      {53, 15, 53, 53, 53, 53, 18, 53, 16, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_detect_hidden,
      5,
      PULSE_VIOLENCE,
      "",
      "You feel less aware of your surroundings.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "eye of the tiger",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_eye_tiger,
      5,
      PULSE_VIOLENCE,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "true sight",
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_true_sight,
      TAR_CHAR_SELF,       POS_STANDING,
      &gsn_true_sight,
      25,
      PULSE_VIOLENCE,
      "",
      "Your vision dulls and you see less truly.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "detect invis",
      {53, 53,  9, 10,  9, 53, 53,  8, 12,  8, 10},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_detect_invis,
      TAR_CHAR_SELF,       POS_STANDING,
      &gsn_detect_invis,
      5,
      PULSE_VIOLENCE,
      "",
      "You no longer see invisible objects.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "detect magic",
      {53, 53,  4, 53, 53, 53, 53,  1, 53, 53,  1},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_detect_magic,
      TAR_CHAR_SELF,       POS_STANDING,
      &gsn_detect_magic,
      5,
      PULSE_VIOLENCE,
      "",
      "The detect magic wears off.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "detect poison",
      {53, 53,  8, 53, 53, 53, 53, 53,  8, 53,  4},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_detect_poison,
      TAR_OBJ_INV,         POS_STANDING,
      NULL,
      5,
      PULSE_VIOLENCE,
      "",
      "!Detect Poison!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "dispel evil",
      {53, 53, 53,  7, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_dispel_evil,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_dispel_evil,
      15,
      PULSE_VIOLENCE,
      "dispel evil",
      "!Dispel Evil!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "dispel good",
      {53, 53, 53, 53,  7, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_dispel_good,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_dispel_good,
      15,
      PULSE_VIOLENCE,
      "dispel good",
      "!Dispel Good!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "dispel magic",
      {53, 53, 20, 53, 53, 53, 53,  9, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_dispel_magic,
      TAR_CHAR_OFFENSIVE,  POS_STANDING,
      &gsn_dispel_magic,
      15,
      PULSE_VIOLENCE,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "earthquake",
      {53, 53, 15, 53, 53, 53, 53, 53, 53, 53, 10},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_earthquake,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_earthquake,
      15,
      PULSE_VIOLENCE,
      "earthquake",
      "!Earthquake!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "alchemy",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_alchemy,
      100,
      PULSE_VIOLENCE * 2,
      "explosion",
      "",
      "The alchemy on $p fades.",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "dissolve",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_dissolve,
      TAR_OBJ_INV,         POS_STANDING,
      NULL,
      20,
      PULSE_VIOLENCE * 2 / 3,
      "",
      "!Dissolve!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "make armor",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_make_armor,
      TAR_OBJ_INV,         POS_STANDING,
      NULL,
      200,
      PULSE_VIOLENCE * 2,
      "",
      "!Make Armor!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "fade",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_fade,
      TAR_OBJ_INV,         POS_STANDING,
      NULL,
      300,
      PULSE_VIOLENCE * 2,
      "explosion",
      "!Fade!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "enchant armor",
      {53, 53, 53, 53, 53, 53, 53, 17, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_enchant_armor,
      TAR_OBJ_INV,         POS_STANDING,
      NULL,
      100,
      PULSE_VIOLENCE * 2,
      "",
      "!Enchant Armor!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "demonic enchant",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_demonic_enchant,
      TAR_OBJ_INV,         POS_STANDING,
      &gsn_demonic_enchant,
      50,
      PULSE_VIOLENCE * 2,
      "",
      "You feel up to entrapping another demon.",
      "The demon escapes from $p.",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         TRUE,    /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "enchant weapon",
      {53, 53, 53, 53, 53, 53, 53, 20, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_enchant_weapon,
      TAR_OBJ_INV,         POS_STANDING,
      NULL,
      100,
      PULSE_VIOLENCE * 2,
      "",
      "!Enchant Weapon!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "energy drain",
      {53, 53, 53, 53, 27, 53, 53, 53, 30, 19, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_energy_drain,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_energy_drain,
      50,
      PULSE_VIOLENCE,
      "energy drain",
      "The cold chill in your body fades away.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "syphon soul",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_syphon_soul,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      NULL,
      20,
      PULSE_VIOLENCE,
      "draining",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "evil eye",
      {53, 53, 53, 53, 53, 53, 53, 53, 37, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_evil_eye,
      TAR_CHAR_OFFENSIVE,  POS_STANDING,
      NULL,
      100,
      PULSE_VIOLENCE * 2,
      "deathly gaze",
      "!EvilEye!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "faerie fire",
      {53, 53, 11, 53,  6, 53, 53,  5,  4,  3,  5},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_faerie_fire,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_faerie_fire,
      5,
      PULSE_VIOLENCE,
      "faerie fire",
      "The pink aura around you fades away.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "faerie fog",
      {53, 53, 14, 53, 53, 53, 53, 16, 53, 17, 17},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_faerie_fog,
      TAR_IGNORE,          POS_STANDING,
      &gsn_faerie_fog,
      12,
      PULSE_VIOLENCE,
      "faerie fog",
      "The fog on your body fades away.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "farsight",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_farsight,
      TAR_IGNORE,          POS_STANDING,
      NULL,
      36,
      PULSE_VIOLENCE * 5 / 3,
      "farsight",
      "!Farsight!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "unholy fire",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_unholy_fire,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_unholy_fire,
      20,
      PULSE_VIOLENCE,
      "unholy fire",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "acid spit",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_acid_spit,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_acid_spit,
      15,
      PULSE_VIOLENCE * 2,
      "acidic spittle",
      "The acid in your eyes washes out.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "fireball",
      {53, 53, 53, 53, 22, 53, 53, 27, 53, 53, 27},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_fireball,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_fireball,
      15,
      PULSE_VIOLENCE,
      "fireball",
      "!Fireball!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "radiance",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_radiance,
      TAR_IGNORE,          POS_FIGHTING,
      NULL,
      15,
      PULSE_VIOLENCE,
      "radiant light",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "bang",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_bang,
      TAR_IGNORE,          POS_FIGHTING,
      NULL,
      25,
      PULSE_VIOLENCE * 2,
      "sonic boom",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "sunburst",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_sunburst,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_sunburst,
      35,
      PULSE_VIOLENCE * 3 / 2,
      "fiery burst",
      "!Sunburst!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "nap",
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_RESTING,
      &gsn_nap,
      0,
      0,
      "",
      "You wiggle about and stand up.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "restlessness",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_RESTING,
      &gsn_nap_timer,
      0,
      0,
      "",
      "You feel ready for naptime again.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   {
      "lethargy",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_RESTING,
      &gsn_nap_slow,
      0,
      0,
      "",
      "You feel ready to play with the world again.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   {
      "hyper",
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_RESTING,
      &gsn_hyper,
      0,
      0,
      "",
      "You suddenly feel the need to lie down. *yawn*",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   {
      "napalm",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_napalm,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_napalm,
      15,
      PULSE_VIOLENCE,
      "blast of fiery debris",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "incinerate",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_incinerate,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_incinerate,
      30,
      PULSE_VIOLENCE,
      "fierce flames",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "nova",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_nova,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_nova,
      25,
      PULSE_VIOLENCE,
      "fiery nova",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "inferno",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_inferno,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_inferno,
      20,
      PULSE_VIOLENCE,
      "wall of black flames",
      "Your scorched eyes feel better.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "flame form",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_flame_form,
      15,
      PULSE_VIOLENCE,
      "flames",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "imbue flame",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_imbue_flame,
      TAR_OBJ_INV,         POS_STANDING,
      &gsn_imbue_flame,
      50,
      PULSE_VIOLENCE * 3 / 2,
      "",
      "",
      "$p grows cold again.",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "ashes to ashes",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_ashes_to_ashes,
      TAR_CHAR_OFFENSIVE,  POS_STANDING,
      &gsn_ashes_to_ashes,
      200,
      PULSE_VIOLENCE * 3,
      "intense all consuming flames",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "raging fire",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_raging_fire,
      TAR_IGNORE,          POS_STANDING,
      &gsn_raging_fire,
      125,
      PULSE_VIOLENCE * 3 / 2,
      "burning forest",
      "You can burn another forest.",
      "The fire goes out.",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "charge",
      {30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_charge,
      0,
      PULSE_VIOLENCE * 2,
      "charge",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "charge weapon",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_charge_weapon,
      TAR_OBJ_INV,         POS_STANDING,
      &gsn_charge_weapon,
      50,
      PULSE_VIOLENCE * 3 / 2,
      "",
      "",
      "$p loses its charge.",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "tornado",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_tornado,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_tornado,
      50,
      PULSE_VIOLENCE * 2,
      "tornadic winds",
      "You can call another tornado.",
      "The tornadic winds recede.",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "airshield",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_airshield,
      TAR_CHAR_SELF,       POS_FIGHTING,
      &gsn_airshield,
      15,
      PULSE_VIOLENCE,
      "",
      "The current of air about you fades.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "suffocate",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_suffocate,
      TAR_CHAR_OFFENSIVE,  POS_STANDING,
      &gsn_suffocate,
      70,
      PULSE_VIOLENCE * 3,
      "suffocation",
      "The air returns to your lungs.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "gale winds",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_gale_winds,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_gale_winds,
      30,
      PULSE_VIOLENCE * 2,
      "frigid wind",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "air dagger",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_air_dagger,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_air_dagger,
      15,
      PULSE_VIOLENCE,
      "stabbing daggers of air",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "jet stream",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_jet_stream,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_jet_stream,
      15,
      PULSE_VIOLENCE,
      "blast of air",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "cyclone",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_cyclone,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_cyclone,
      25,
      PULSE_VIOLENCE,
      "air blast",
      "You rub the dirt and dust from your eyes.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "implosion",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_implosion,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_implosion,
      50,
      PULSE_VIOLENCE * 2,
      "crushing air flow",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "thunder",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_thunder,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_thunder,
      15,
      PULSE_VIOLENCE,
      "deafening thunder",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "mass fly",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_mass_fly,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_mass_fly,
      35,
      PULSE_VIOLENCE,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "dust devil",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_dust_devil,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_dust_devil,
      15,
      PULSE_VIOLENCE,
      "dust devil",
      "You rub the dust out of your eyes.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "vortex",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_vortex,
      TAR_IGNORE,          POS_STANDING,
      &gsn_vortex,
      65,
      PULSE_VIOLENCE * 2,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "wraithform",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_wraithform,
      15,
      PULSE_VIOLENCE,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "stone",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_stone,
      TAR_CHAR_OFFENSIVE,  POS_STANDING,
      &gsn_stone,
      100,
      PULSE_VIOLENCE * 2,
      "",
      "The rock crumbles and you return to flesh.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "stalagmite",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_stalagmite,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_stalagmite,
      20,
      PULSE_VIOLENCE,
      "stalagmite",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "tremor",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_tremor,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_tremor,
      12,
      PULSE_VIOLENCE,
      "tremor",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "avalanche",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_avalanche,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_avalanche,
      25,
      PULSE_VIOLENCE,
      "falling debris",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "cave-in",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_cave_in,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_cave_in,
      25,
      PULSE_VIOLENCE,
      "falling rock",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "crush",
      {16, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_crush,
      0,
      PULSE_VIOLENCE * 2,
      "body crush",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "crushing hands",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_crushing_hands,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_crushing_hands,
      25,
      PULSE_VIOLENCE * 2,
      "crushing hands",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "shield",
      {53, 53, 27, 53, 53, 27, 53, 25, 53, 53, 25},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_shield,
      TAR_CHAR_DEFENSIVE,  POS_STANDING,
      &gsn_shield,
      12,
      PULSE_VIOLENCE,
      "",
      "Your force shield shimmers then fades away.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "shield of earth",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_shield_of_earth,
      TAR_CHAR_SELF,       POS_STANDING,
      &gsn_shield_of_earth,
      15,
      PULSE_VIOLENCE,
      "",
      "Your shield of earth crumbles away.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "stoney grasp",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_stoney_grasp,
      TAR_CHAR_OFFENSIVE,  POS_STANDING,
      &gsn_stoney_grasp,
      75,
      PULSE_VIOLENCE * 2,
      "",
      "The stoney hands crumble to dust.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "burrow",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_burrow,
      15,
      PULSE_VIOLENCE,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "meteor",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_meteor,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_meteor,
      0,
      0,
      "flaming hot rocks",
      "",
      "The ground is no longer scorched.",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "meteor storm",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_meteor_storm,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_meteor_storm,
      40,
      PULSE_VIOLENCE * 2,
      "falling flaming rocks",
      "",
      "The ominous darkness lifts.",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "earth form",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_earth_form,
      15,
      PULSE_VIOLENCE,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "geyser",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_geyser,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_geyser,
      15,
      PULSE_VIOLENCE,
      "blast of water",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "water spout",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_water_spout,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_water_spout,
      50,
      PULSE_VIOLENCE * 2,
      "fierce winds and waves",
      "You can call another water spout.",
      "The water spout fades away and the waters calm.",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "deluge",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_deluge,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_deluge,
      20,
      PULSE_VIOLENCE,
      "deluge",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "whirlpool",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_whirlpool,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_whirlpool,
      25,
      PULSE_VIOLENCE,
      "whirlpool",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "blizzard",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_blizzard,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_blizzard,
      20,
      PULSE_VIOLENCE,
      "icy wind",
      "You feel less cold.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "spring rains",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_spring_rains,
      TAR_IGNORE,          POS_STANDING,
      &gsn_spring_rains,
      50,
      PULSE_VIOLENCE,
      "",
      "",
      "The gentle rain stops.",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "submerge",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_submerge,
      TAR_CHAR_SELF,       POS_STANDING,
      &gsn_submerge,
      50,
      PULSE_VIOLENCE * 2,
      "",
      "You emerge from the waves.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "freeze",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_freeze,
      TAR_CHAR_OFFENSIVE,  POS_STANDING,
      &gsn_freeze,
      75,
      PULSE_VIOLENCE * 2,
      "chilling",
      "The ice trapping you melts away.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "freeze person",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_freeze_person,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_freeze_person,
      75,
      PULSE_VIOLENCE * 3 / 2,
      "",
      "You are suspectable to time manipulation again.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "frost charge",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_frost_charge,
      TAR_OBJ_INV,         POS_STANDING,
      &gsn_frost_charge,
      50,
      PULSE_VIOLENCE * 3 / 2,
      "",
      "",
      "$p loses its frosty haze.",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "purify",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_purify,
      TAR_CHAR_DEFENSIVE,  POS_FIGHTING,
      NULL,
      45,
      PULSE_VIOLENCE * 2,
      "",
      "Water's purifying powers return to you.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "waterwalk",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_waterwalk,
      TAR_IGNORE,          POS_STANDING,
      &gsn_waterwalk,
      60,
      PULSE_VIOLENCE * 2,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "ice armor",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_ice_armor,
      TAR_CHAR_SELF,       POS_STANDING,
      &gsn_ice_armor,
      15,
      PULSE_VIOLENCE,
      "",
      "Your casing of ice melts away.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "water form",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_water_form,
      15,
      PULSE_VIOLENCE,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "water of life",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_water_of_life,
      TAR_IGNORE,          POS_STANDING,
      &gsn_water_of_life,
      100,
      PULSE_VIOLENCE * 2,
      "",
      "You can call upon the growing mist again.",
      "The magically enhanced plant life crumbles away.",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "fountain of youth",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_fountain_of_youth,
      TAR_IGNORE,          POS_STANDING,
      &gsn_fountain_of_youth,
      100,
      PULSE_VIOLENCE,
      "",
      "You can create another rejuvenating fountain.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "rip tide",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_rip_tide,
      TAR_IGNORE,          POS_STANDING,
      &gsn_rip_tide,
      15,
      PULSE_VIOLENCE,
      "",
      "You manage to free yourself of the riptide.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "flash",
      {53, 53, 53, 25, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_flash,
      TAR_IGNORE,          POS_FIGHTING,
      NULL,
      20,
      PULSE_VIOLENCE * 2,
      "flash",
      "!Flash!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "fireproof",
      {53, 53, 53, 53, 53, 53, 53, 25, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_fireproof,
      TAR_OBJ_INV,         POS_STANDING,
      NULL,
      10,
      PULSE_VIOLENCE,
      "",
      "",
      "$p's protective aura fades.",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "flamestrike",
      {53, 53, 53, 15, 53, 53, 53, 53, 53, 17, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_flamestrike,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      NULL,
      20,
      PULSE_VIOLENCE,
      "flamestrike",
      "!Flamestrike!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "fly",
      {53, 53, 21, 53, 53, 53, 53, 18, 53, 53, 16},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_fly,
      TAR_CHAR_DEFENSIVE,  POS_STANDING,
      &gsn_fly,
      10,
      PULSE_VIOLENCE * 3 / 2,
      "",
      "You slowly float to the ground.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "beastial command",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_beastial_command,
      30,
      PULSE_VIOLENCE * 2,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "canopy walk",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_canopy_walk,
      30,
      PULSE_VIOLENCE * 2,
      "",
      "You can walk the canopies again.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "snare trap",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_trapstun,
      30,
      PULSE_VIOLENCE * 2,
      "",
      "You escape from the snare.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "trap making",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_trapmaking,
      30,
      PULSE_VIOLENCE * 2,
      "",
      "",
      "The trap crumbles into dust.",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "call of the wild",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_call_of_the_wild,
      30,
      PULSE_VIOLENCE * 2,
      "",
      "You can call upon your wild friends again.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "beastial stance",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_beaststance,
      30,
      PULSE_VIOLENCE * 2,
      "",
      "You stop acting like a beast.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "war paint",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_warpaint,
      30,
      PULSE_VIOLENCE * 2,
      "",
      "Your war paint fades.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "frenzy",
      {53, 53, 25, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_frenzy,
      TAR_CHAR_DEFENSIVE,  POS_STANDING,
      &gsn_frenzy,
      30,
      PULSE_VIOLENCE * 2,
      "",
      "Your rage ebbs.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "gate",
      {53, 53, 20, 53, 53, 53, 53, 24, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_gate,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_gate,
      80,
      PULSE_VIOLENCE * 2,
      "",
      "!Gate!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "portal",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_portal,
      TAR_IGNORE,          POS_FIGHTING,
      NULL,
      90,
      PULSE_VIOLENCE * 2,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "time travel",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_time_travel,
      TAR_IGNORE,          POS_FIGHTING,
      NULL,
      70,
      PULSE_VIOLENCE * 2,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "giant strength",
      {53, 53, 29, 53, 53, 53, 53, 24, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_giant_strength,
      TAR_CHAR_DEFENSIVE,  POS_STANDING,
      &gsn_giant_strength,
      20,
      PULSE_VIOLENCE,
      "",
      "You feel weaker.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "harm",
      {53, 53, 24, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_harm,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      NULL,
      35,
      PULSE_VIOLENCE,
      "divine strike",
      "!Harm!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "haste",
      {53, 53, 53, 53, 53, 53, 53, 23, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_haste,
      TAR_CHAR_DEFENSIVE,  POS_FIGHTING,
      &gsn_haste,
      30,
      PULSE_VIOLENCE,
      "",
      "You feel yourself slow down.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "water breathing",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_waterbreathing,
      TAR_CHAR_DEFENSIVE,  POS_FIGHTING,
      &gsn_waterbreathing,
      35,
      PULSE_VIOLENCE,
      "drowning",
      "You lose the ability to breath under water.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "grace",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_grace,
      50,
      PULSE_VIOLENCE,
      "",
      "Your grace of extra time has left you.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "ultra heal",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_ultra_heal,
      TAR_CHAR_HEALING,    POS_FIGHTING,
      NULL,
      125,
      PULSE_VIOLENCE,
      "",
      "!Ultra Heal!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "heal",
      {53, 53, 26, 29, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_heal,
      TAR_CHAR_HEALING,    POS_FIGHTING,
      &gsn_heal,
      50,
      PULSE_VIOLENCE,
      "",
      "!Heal!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "heat metal",
      {53, 53, 53, 53, 53, 53, 53, 25, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_heat_metal,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      NULL,
      25,
      PULSE_VIOLENCE * 3 / 2,
      "spell",
      "!Heat Metal!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "curse weapon",
      {53, 53, 53, 53, 28, 53, 53, 53, 30, 25, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_curse_weapon,
      TAR_OBJ_INV,         POS_STANDING,
      NULL,
      75,
      PULSE_VIOLENCE,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "bless weapon",
      {53, 53, 53, 41, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_bless_weapon,
      TAR_OBJ_INV,         POS_STANDING,
      NULL,
      150,
      PULSE_VIOLENCE * 2,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "holy fire",
      {53, 53, 30, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_holy_fire,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_holy_fire,
      20,
      PULSE_VIOLENCE,
      "holy fire",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "mass protection",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_mass_protection,
      TAR_IGNORE,          POS_FIGHTING,
      NULL,
      300,
      PULSE_VIOLENCE * 2,
      "",
      "!Mass Protection!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "holy word",
      {53, 53, 40, 40, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_holy_word,
      TAR_IGNORE,          POS_FIGHTING,
      NULL,
      200,
      PULSE_VIOLENCE * 2,
      "divine wrath",
      "!Holy Word!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "need",
      {53, 53, 53, 30, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_need,
      TAR_CHAR_SELF,       POS_STANDING,
      &gsn_need,
      80,
      PULSE_VIOLENCE * 3 / 2,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "identify",
      {53, 53, 14, 16, 16, 53, 53, 15, 53, 14, 15},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_identify,
      TAR_OBJ_INV,         POS_STANDING,
      &gsn_identify,
      12,
      PULSE_VIOLENCE * 3 / 2,
      "",
      "!Identify!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "obscuration",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_obscure,
      TAR_OBJ_INV,         POS_STANDING,
      &gsn_obscure,
      200,
      PULSE_VIOLENCE * 3 / 2,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "infravision",
      {53, 53, 10, 53, 14, 53, 53, 53,  4,  6,  8},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_infravision,
      TAR_CHAR_DEFENSIVE,  POS_STANDING,
      &gsn_infravision,
      5,
      PULSE_VIOLENCE * 3 / 2,
      "",
      "You no longer see in the dark.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "invisibility",
      {53, 53, 53, 53, 12, 53, 53,  8, 13,  8, 10},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_invis,
      TAR_CHAR_DEFENSIVE,  POS_STANDING,
      &gsn_invis,
      5,
      PULSE_VIOLENCE,
      "",
      "You are no longer invisible.",
      "$p fades into view.",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "invisible mail",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_invis_mail,
      TAR_CHAR_DEFENSIVE,  POS_STANDING,
      &gsn_invisible_mail,
      15,
      PULSE_VIOLENCE,
      "",
      "Your invisible mail fades away.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "know alignment",
      {53, 53, 15, 18, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_know_alignment,
      TAR_CHAR_DEFENSIVE,  POS_FIGHTING,
      &gsn_know_alignment,
      9,
      PULSE_VIOLENCE,
      "",
      "!Know Alignment!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "know time",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_know_time,
      20,
      PULSE_VIOLENCE,
      "",
      "!Know time!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "brew",
      {12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_brew,
      TAR_OBJ_INV,         POS_STANDING,
      NULL,
      35,
      PULSE_VIOLENCE,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "lightning bolt",
      {53, 53, 53, 53, 16, 53, 53,  9, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_lightning_bolt,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_lightning_bolt,
      15,
      PULSE_VIOLENCE,
      "lightning bolt",
      "!Lightning Bolt!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "origin",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_origin,
      TAR_OBJ_INV,         POS_STANDING,
      NULL,
      80,
      PULSE_VIOLENCE * 3 / 2,
      "",
      "!Origin!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "locate object",
      {53, 53, 20, 20, 53, 53, 53, 20, 53, 20, 20},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_locate_object,
      TAR_IGNORE,          POS_STANDING,
      NULL,
      20,
      PULSE_VIOLENCE * 3 / 2,
      "",
      "!Locate Object!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "magic missile",
      {53, 53, 53, 53,  1, 53, 53,  1, 53,  1,  1},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_magic_missile,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_magic_missile,
      5,
      PULSE_VIOLENCE,
      "magic missile",
      "!Magic Missile!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "mass healing",
      {53, 53, 40, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_mass_healing,
      TAR_IGNORE,          POS_STANDING,
      &gsn_mass_healing,
      100,
      PULSE_VIOLENCE * 3,
      "",
      "!Mass Healing!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "mass invis",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 30, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_mass_invis,
      TAR_IGNORE,          POS_STANDING,
      &gsn_mass_invis,
      20,
      PULSE_VIOLENCE,
      "",
      "You are no longer invisible.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "mass haste",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_mass_haste,
      TAR_IGNORE,          POS_STANDING,
      &gsn_mass_haste,
      50,
      PULSE_VIOLENCE,
      "",
      "You feel yourself slow down.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "mass slow",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_mass_slow,
      TAR_IGNORE,          POS_STANDING,
      &gsn_mass_slow,
      50,
      PULSE_VIOLENCE,
      "",
      "You feel yourself speed up.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "pass door",
      {53, 53, 36, 53, 53, 53, 53, 30, 53, 23, 25},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_pass_door,
      TAR_CHAR_SELF,       POS_STANDING,
      &gsn_pass_door,
      20,
      PULSE_VIOLENCE,
      "",
      "You feel solid again.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "phasing",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_phasing,
      TAR_CHAR_DEFENSIVE,  POS_STANDING,
      &gsn_phasing,
      60,
      PULSE_VIOLENCE,
      "",
      "You return to normal.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "plague",
      {53, 53, 53, 53, 26, 53, 53, 53, 53, 25, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_plague,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_plague,
      20,
      PULSE_VIOLENCE,
      "sickness",
      "Your sores vanish.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "poison",
      {53, 53, 53, 53, 27, 53, 53, 53, 53, 17, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_poison,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_poison,
      10,
      PULSE_VIOLENCE,
      "poison",
      "You feel less sick.",
      "The poison on $p dries up.",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "nausea",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_nausea,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_nausea,
      10,
      PULSE_VIOLENCE,
      "nausea",
      "You feel less sick.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "power word kill",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 38, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_power_word_kill,
      TAR_CHAR_OFFENSIVE,  POS_STANDING,
      &gsn_power_word_kill,
      150,
      PULSE_VIOLENCE * 2,
      "word of power",
      "PowerWordKill!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         TRUE,    /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "power word stun",
      {53, 53, 53, 53, 38, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_power_word_stun,
      TAR_CHAR_OFFENSIVE,  POS_STANDING,
      &gsn_power_word_stun,
      100,
      PULSE_VIOLENCE * 3,
      "word of power",
      "You recover from the shock and stagger about slightly.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         TRUE,    /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "protection evil",
      {53, 53, 15, 12, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_protection_evil,
      TAR_CHAR_SELF,       POS_STANDING,
      &gsn_protection_evil,
      5,
      PULSE_VIOLENCE,
      "",
      "You feel less protected.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "protection good",
      {53, 53, 15, 53, 21, 53, 53, 53, 18, 15, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_protection_good,
      TAR_CHAR_SELF,       POS_STANDING,
      &gsn_protection_good,
      5,
      PULSE_VIOLENCE,
      "",
      "You feel less protected.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "ray of truth",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_ray_of_truth,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      NULL,
      20,
      PULSE_VIOLENCE,
      "ray of truth",
      "!Ray of Truth!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "recharge",
      {53, 53, 53, 53, 53, 53, 53, 27, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_recharge,
      TAR_OBJ_INV,         POS_STANDING,
      NULL,
      60,
      PULSE_VIOLENCE * 2,
      "",
      "!Recharge!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "refresh",
      {53, 53,  7, 14, 53, 53, 53, 53, 53, 53,  9},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_refresh,
      TAR_CHAR_DEFENSIVE,  POS_STANDING,
      &gsn_refresh,
      12,
      PULSE_VIOLENCE * 3 / 2,
      "refresh",
      "!Refresh!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "remove curse",
      {53, 53, 20, 21, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_remove_curse,
      TAR_OBJ_CHAR_DEF,    POS_STANDING,
      &gsn_remove_curse,
      5,
      PULSE_VIOLENCE,
      "",
      "!Remove Curse!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "super charge",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_super_charge,
      TAR_CHAR_SELF,       POS_STANDING,
      NULL,
      300,
      PULSE_VIOLENCE * 2,
      "",
      "Your super charged field fades away.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "halo of eyes",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_halo_of_eyes,
      TAR_CHAR_SELF,       POS_STANDING,
      &gsn_halo_of_eyes,
      100,
      PULSE_VIOLENCE,
      "",
      "Your halo of eyes fades away.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "temporal shield",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_temporal_shield,
      TAR_CHAR_SELF,       POS_STANDING,
      &gsn_temporal_shield,
      80,
      PULSE_VIOLENCE * 2,
      "",
      "Your state of temporal fluxing ends.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "barrier",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_barrier,
      TAR_CHAR_SELF,       POS_STANDING,
      &gsn_barrier,
      300,
      PULSE_VIOLENCE * 2,
      "",
      "Your barrier has fallen.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "dark armor",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_dark_armor,
      TAR_CHAR_SELF,       POS_STANDING,
      &gsn_dark_armor,
      100,
      PULSE_VIOLENCE,
      "",
      "Your dark armor fades away.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "sanctuary",
      {53, 53, 21, 28, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_sanctuary,
      TAR_CHAR_DEFENSIVE,  POS_STANDING,
      &gsn_sanctuary,
      75,
      PULSE_VIOLENCE,
      "",
      "The holy aura around your body fades.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "chromatic shield",
      {53, 53, 53, 53, 53, 53, 53, 29, 53, 53, 32},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_chromatic_shield,
      TAR_CHAR_DEFENSIVE,  POS_STANDING,
      &gsn_chromatic_shield,
      75,
      PULSE_VIOLENCE,
      "",
      "The chromatic aura around your body fades.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "divine protection",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_divine_protect,
      TAR_IGNORE,          POS_STANDING,
      &gsn_divine_protect,
      50,
      PULSE_VIOLENCE,
      "",
      "You feel less blessed of the gods.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "shocking grasp",
      {53, 53, 53, 53, 53, 53, 53,  5, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_shocking_grasp,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_shocking_grasp,
      15,
      PULSE_VIOLENCE,
      "shocking grasp",
      "!Shocking Grasp!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "sleep",
      {53, 53, 53, 53, 18, 53, 53, 53, 53, 16, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_sleep,
      TAR_CHAR_OFFENSIVE,  POS_STANDING,
      &gsn_sleep,
      15,
      PULSE_VIOLENCE,
      "",
      "You feel less tired.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "sleep timer",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_DEAD,
      &gsn_sleep_timer,
      0,
      0,
      "",
      "You no longer feel so vigorously awake.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "slow",
      {53, 53, 53, 53, 53, 53, 53, 25, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_slow,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_slow,
      30,
      PULSE_VIOLENCE,
      "",
      "You feel yourself speed up.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "stone skin",
      {53, 53, 29, 53, 53, 53, 53, 53, 53, 53, 26},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_stone_skin,
      TAR_CHAR_SELF,       POS_STANDING,
      &gsn_stone_skin,
      12,
      PULSE_VIOLENCE,
      "",
      "Your skin feels soft again.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "summon",
      {53, 53, 15, 20, 21, 53, 53, 53, 53, 27, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_summon,
      TAR_IGNORE,          POS_STANDING,
      &gsn_summon,
      50,
      PULSE_VIOLENCE * 3 / 2,
      "",
      "!Summon!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "teleport",
      {53, 53, 13, 53, 14, 53, 53, 15, 53, 15, 16},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_teleport,
      TAR_CHAR_SELF,       POS_STANDING,
      &gsn_teleport,
      35,
      PULSE_VIOLENCE * 3 / 2,
      "",
      "!Teleport!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "turn undead",
      {53, 53, 25, 28, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_turn_undead,
      TAR_IGNORE,          POS_STANDING,
      NULL,
      50,
      PULSE_VIOLENCE,
      "divine intervention",
      "!TurnUndead!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "undead drain",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_undead_drain,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_undead_drain,
      60,
      PULSE_VIOLENCE,
      "undead drain",
      "You feel your body and mind restored to normal.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "ventriloquate",
      {53, 53,  4, 53, 53, 53, 53,  4, 53, 53,  4},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_ventriloquate,
      TAR_IGNORE,          POS_STANDING,
      &gsn_ventriloquate,
      5,
      PULSE_VIOLENCE,
      "",
      "!Ventriloquate!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "team spirit",
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_team_spirit,
      TAR_CHAR_DEFENSIVE,  POS_STANDING,
      &gsn_team_spirit,
      5,
      PULSE_VIOLENCE,
      "energy transference",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "venueport",
      {29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_venueport,
      TAR_IGNORE,          POS_STANDING,
      &gsn_venueport,
      125,
      PULSE_VIOLENCE,
      "",
      "You are once again ready to venueport.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "weaken",
      {53, 53, 53, 53, 27, 53, 53, 53, 53, 29, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_weaken,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_weaken,
      20,
      PULSE_VIOLENCE,
      "spell",
      "You feel stronger.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "web",
      {53, 53, 53, 53, 53, 53, 53, 53, 18, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_web,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      NULL,
      20,
      PULSE_VIOLENCE,
      "corrosive webs",
      "The sticky strands about your body dissolve.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "word of recall",
      {53, 53, 28, 31, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_word_of_recall,
      TAR_CHAR_DEFENSIVE,  POS_FIGHTING,
      &gsn_word_recall,
      5,
      PULSE_VIOLENCE * 3 / 2,
      "",
      "!Word of Recall!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "power word recall",
      {53, 53, 53, 53, 53, 53, 53, 29, 53, 33, 30},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_power_word_recall,
      TAR_CHAR_DEFENSIVE,  POS_FIGHTING,
      &gsn_power_word_recall,
      5,
      PULSE_VIOLENCE * 3 / 2,
      "",
      "!Power Word Recall!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "summon fire elemental",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 27},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_summon_fire_elemental,
      TAR_IGNORE,          POS_STANDING,
      NULL,
      50,
      PULSE_VIOLENCE,
      "",
      "You feel up to calling another fire elemental.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "summon void elemental",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 40},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_summon_void_elemental,
      TAR_IGNORE,          POS_STANDING,
      NULL,
      50,
      PULSE_VIOLENCE,
      "",
      "You feel up to calling another void elemental.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "summon earth elemental",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 20},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_summon_earth_elemental,
      TAR_IGNORE,          POS_STANDING,
      NULL,
      50,
      PULSE_VIOLENCE,
      "",
      "You feel up to calling another earth elemental.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "hire crew",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_hire_crew,
      50,
      PULSE_VIOLENCE,
      "",
      "You can hire more crew.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "mental link",
      {20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_mentallink,
      0,
      PULSE_VIOLENCE * 2,
      "mental feedback",
      "Your mental link fades away.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "summon air elemental",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 23},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_summon_air_elemental,
      TAR_IGNORE,          POS_STANDING,
      NULL,
      50,
      PULSE_VIOLENCE,
      "",
      "You feel up to calling another air elemental.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "summon water elemental",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 26},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_summon_water_elemental,
      TAR_IGNORE,          POS_STANDING,
      NULL,
      50,
      PULSE_VIOLENCE,
      "",
      "You feel up to calling another water elemental.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "tsunami",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 34},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_tsunami,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_tsunami,
      30,
      PULSE_VIOLENCE,
      "raging tidal wave",
      "!Tsunami!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "earthmaw",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 36},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_earthmaw,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_earthmaw,
      30,
      PULSE_VIOLENCE,
      "earthmaw",
      "!earthmaw!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "exorcism",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_exorcism,
      TAR_IGNORE,          POS_FIGHTING,
      NULL,
      30,
      PULSE_VIOLENCE,
      "exorcism",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "windwall",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 22},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1},
      spell_windwall,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_windwall,
      20,
      PULSE_VIOLENCE,
      "air blast",
      "You rub the dirt and dust from your eyes.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "drain",
      {53, 53, 53, 53, 53, 53, 53, 15, 53, 53, 53},
      { 0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0},
      spell_drain,
      TAR_OBJ_INV,         POS_STANDING,
      NULL,
      30,
      PULSE_VIOLENCE,
      "",
      "!Drain!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "disenchant armor",
      {53, 53, 53, 53, 53, 53, 53, 16, 53, 53, 53},
      { 0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0},
      spell_disenchant_armor,
      TAR_OBJ_INV,         POS_STANDING,
      NULL,
      50,
      PULSE_VIOLENCE * 3,
      "",
      "!Disenchant!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "disenchant weapon",
      {53, 53, 53, 53, 53, 53, 53, 21, 53, 53, 53},
      { 0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0},
      spell_disenchant_weapon,
      TAR_OBJ_INV,         POS_STANDING,
      NULL,
      50,
      PULSE_VIOLENCE * 3,
      "",
      "!Disenchant!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "cone of cold",
      {53, 53, 53, 53, 53, 53, 53, 32, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1, 1,  1,  1},
      spell_cone_of_cold,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_cone_of_cold,
      30,
      PULSE_VIOLENCE,
      "blast of cold",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "protective shield",
      {53, 53, 16, 53, 53, 53, 53, 17, 53, 18, 18},
      { 0,  0,  1,  0,  0,  0,  0,  1,  0,  1,  1},
      spell_protective_shield,
      TAR_CHAR_SELF,       POS_STANDING,
      &gsn_protective_shield,
      75,
      PULSE_VIOLENCE,
      "",
      "Your protective shield fades away.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "accelerate time",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_accelerate_time,
      TAR_IGNORE,          POS_STANDING,
      NULL,
      100,
      PULSE_VIOLENCE * 2,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "timestop",
      {53, 53, 53, 53, 53, 53, 53, 40, 53, 53, 53},
      { 0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0},
      spell_timestop,
      TAR_IGNORE,          POS_STANDING,
      &gsn_timestop,
      100,
      PULSE_VIOLENCE * 2,
      "",
      "Everything around you starts moving fast again.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "timestop spell",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0},
      spell_timestop,
      TAR_IGNORE,          POS_STANDING,
      &gsn_timestop_done,
      100,
      PULSE_VIOLENCE * 2,
      "",
      "You are once again able to interfere with time.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "phoenix brand",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_phoenix,
      0,
      0,
      "",
      "The brand of the phoenix warms up again.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   {
      "consecrate",
      {53, 53, 53, 38, 53, 53, 53, 53, 53, 53, 53},
      { 0,  0,  0,  1,  0,  0,  0,  0,  0,  0,  0},
      spell_consecrate,
      TAR_IGNORE,          POS_STANDING,
      &gsn_consecrate,
      90,
      PULSE_VIOLENCE * 5 / 4,
      "",
      "You feel holy enough to consecrate more ground.",
      "The curse returns to this place.",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "illusionary wall",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_illusionary_wall,
      TAR_IGNORE,          POS_STANDING,
      &gsn_illusionary_wall,
      70,
      PULSE_VIOLENCE * 5 / 2,
      "",
      "You can create another illusionary wall.",
      "The illusionary wall fades away.",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "fog of concealment",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_fog_conceilment,
      TAR_IGNORE,          POS_STANDING,
      &gsn_fog_conceilment,
      100,
      PULSE_VIOLENCE * 5 / 2,
      "",
      "You can once again create fog.",
      "The fog fades away.",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "flood room",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 20},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_flood_room,
      TAR_IGNORE,          POS_STANDING,
      &gsn_flood_room,
      100,
      PULSE_VIOLENCE * 3 / 2,
      "",
      "You can once again call a flood.",
      "The waters recede.",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "fist of god",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_fist_of_god,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_fist_god,
      50,
      PULSE_VIOLENCE,
      "pound",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "visions",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_visions,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      NULL,
      20,
      PULSE_VIOLENCE,
      "visions",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "hall of mirrors",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_hall_mirrors,
      TAR_IGNORE,          POS_STANDING,
      &gsn_hall_mirrors,
      100,
      PULSE_VIOLENCE * 5 / 2,
      "",
      "You can create another hall of mirrors.",
      "The surroundings contort violently and return to normal.",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "curse room",
      {53, 53, 53, 53, 38, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1, 1 ,  1,  1},
      spell_curse_room,
      TAR_IGNORE,          POS_STANDING,
      NULL,
      90,
      PULSE_VIOLENCE * 5 / 2,
      "",
      "You feel the power to curse your surroundings again.",
      "The effects of the ritual subside.",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "nomagic toggle",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_no_magic_toggle,
      90,
      PULSE_VIOLENCE * 5 / 2,
      "",
      "",
      "Magic can once again be cast here.",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "hellforge",
      {53, 53, 53, 53, 27, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1, 1,  1,  1},
      spell_hellforge,
      TAR_OBJ_INV,         POS_STANDING,
      NULL,
      100,
      PULSE_VIOLENCE * 2,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "voodoo",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_voodoo,
      0,
      PULSE_VIOLENCE * 2,
      "voodoo",
      "You feel better.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "wither",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 32, 53},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0},
      spell_wither,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_wither,
      40,
      PULSE_VIOLENCE,
      "withering",
      "Your body feels less emaciated.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "shadowstrike",
      {53, 53, 53, 53, 53, 53, 53, 53, 36, 53, 53},
      { 0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0},
      spell_shadowstrike,
      TAR_CHAR_OFFENSIVE,  POS_STANDING,
      &gsn_shadowstrike,
      50,
      PULSE_VIOLENCE,
      "",
      "The shadowy arms grasping you fade away in a smokey haze.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "dark dream",
      {53, 53, 53, 53, 53, 53, 53, 53, 15, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_SLEEPING,
      &gsn_dark_dream,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "shadowself",
      {53, 53, 53, 53, 53, 53, 53, 53, 22, 29, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_shadowself,
      TAR_IGNORE,          POS_STANDING,
      NULL,
      20,
      PULSE_VIOLENCE,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "mirror image",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_mirror_image,
      TAR_CHAR_DEFENSIVE,  POS_STANDING,
      NULL,
      30,
      PULSE_VIOLENCE,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "nightwalk",
      {53, 53, 53, 53, 53, 53, 53, 53, 28, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_nightwalk,
      TAR_IGNORE,          POS_STANDING,
      NULL,
      75,
      PULSE_VIOLENCE,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "insight",
      {15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_insight,
      TAR_IGNORE,          POS_STANDING,
      &gsn_insight,
      50,
      PULSE_VIOLENCE,
      "",
      "You feel ready to find more people now.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         TRUE,    /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "blessing",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_blessing,
      0,
      0,
      "",
      "You feel your divine protection fade.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "blessing of darkness",
      {35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_bless_of_dark,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_blessing_of_darkness,
      30,
      PULSE_VIOLENCE * 2,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         TRUE,    /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "lightshield",
      {30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_lightshield,
      TAR_CHAR_DEFENSIVE,  POS_STANDING,
      &gsn_lightshield,
      20,
      PULSE_VIOLENCE * 3 / 2,
      "",
      "The glow of purity surrounding you fades.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "repent",
      {20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_repent,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_repent,
      30,
      PULSE_VIOLENCE * 3 / 2,
      "call upon the divine",
      "You are released by the light.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "defilement",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_defilement,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_defilement,
      30,
      PULSE_VIOLENCE * 2,
      "defilement",
      "!DEFILEMENT!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         TRUE,    /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "shroud",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_shroud,
      0,
      PULSE_VIOLENCE * 3,
      "",
      "The shadows fade from your body in faint wisps.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "shroud of pro",
      {15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_shroud_pro,
      TAR_CHAR_DEFENSIVE,  POS_STANDING,
      &gsn_shroud_of_pro,
      30,
      PULSE_VIOLENCE,
      "",
      "The shadows no longer protect you.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         TRUE,    /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "fire and ice",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 24},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_fire_and_ice,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_fire_and_ice,
      16,
      PULSE_VIOLENCE,
      "fire and ice",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "utter heal",
      {53, 53, 34, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_utter_heal,
      TAR_CHAR_HEALING,    POS_STANDING,
      NULL,
      75,
      PULSE_VIOLENCE * 2,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "atrophy",
      {53, 53, 38, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_atrophy,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_atrophy,
      25,
      PULSE_VIOLENCE,
      "atrophy",
      "Your body ceases to waste away.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   /*
   * Dragon breath
   */
   {
      "acid breath",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_acid_breath,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_acid_breath,
      100,
      PULSE_VIOLENCE * 2,
      "blast of acid",
      "!Acid Breath!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "fire breath",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_fire_breath,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_fire_breath,
      200,
      PULSE_VIOLENCE * 2,
      "blast of flame",
      "The smoke leaves your eyes.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "frost breath",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_frost_breath,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_frost_breath,
      125,
      PULSE_VIOLENCE * 2,
      "blast of frost",
      "!Frost Breath!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "gas breath",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_gas_breath,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_gas_breath,
      175,
      PULSE_VIOLENCE * 2,
      "blast of gas",
      "!Gas Breath!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "lightning breath",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_lightning_breath,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_lightning_breath,
      150,
      PULSE_VIOLENCE * 2,
      "blast of lightning",
      "!Lightning Breath!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   /* combat and weapons skills */
   {
      "axe",
      { 1, 53, 53,  1,  1,  1, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_axe,
      0,
      0,
      "",
      "!Axe!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "dagger",
      { 1,  1, 53,  1,  1,  1, 53,  1,  1,  1,  1},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_dagger,
      0,
      0,
      "",
      "!Dagger!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "flail",
      { 1, 53,  1,  1,  1, 53, 53,  1, 53,  1,  1},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_flail,
      0,
      0,
      "",
      "!Flail!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "flail arms",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_DEAD,
      &gsn_flail_arms,
      0,
      0,
      "flailing bone arms",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "mind sear",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_mind_sear,
      0,
      0,
      "mind sear",
      "The burning in your mind fades away.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "mace",
      { 1,  1,  1,  1,  1, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_mace,
      0,
      0,
      "",
      "!Mace!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "polearm",
      { 1, 53, 53,  1,  1,  1, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_polearm,
      0,
      0,
      "",
      "!Polearm!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "shield block",
      {10, 14, 18, 11, 11, 12, 53, 53, 15, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_shield_block,
      0,
      0,
      "",
      "!Shield!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "spear",
      { 1,  1, 53,  1,  1,  1, 53, 53,  1, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_spear,
      0,
      0,
      "",
      "!Spear!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "two-handed wielding",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_two_hand_wield,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "dual wield",
      {12, 25, 53, 18, 18, 16, 53, 53, 17, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_dual_wield,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "dual parry",
      {22, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_dual_parry,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "sword",
      { 1,  1, 53,  1,  1,  1, 53, 53,  1, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_sword,
      0,
      0,
      "",
      "!sword!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "archery",
      {1, 53, 53, 53, 53,  1, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_archery,
      0,
      0,
      "arrow",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "advanced archery",
      {53, 53, 53, 53, 53, 25, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_adv_archery,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "expert archery",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_exp_archery,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "whip",
      { 1,  1,  1, 53,  1,  1, 53,  1,  1,  1,  1},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_whip,
      0,
      0,
      "",
      "!Whip!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "advanced swords",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_adv_sword,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "expert swords",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_exp_sword,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "advanced axes",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_adv_axe,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "expert axes",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_exp_axe,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "advanced daggers",
      {53, 35, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_adv_dagger,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "expert daggers",
      {53, 45, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_exp_dagger,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "advanced staffs",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_adv_staff,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "expert staffs",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_exp_staff,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "advanced maces",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_adv_mace,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "expert maces",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_exp_mace,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "advanced whips",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_adv_whip,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "expert whips",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_exp_whip,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "advanced flails",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_adv_flail,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "expert flails",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_exp_flail,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "advanced pole-arms",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_adv_polearm,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "expert pole-arms",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_exp_polearm,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "advanced spears",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_adv_spear,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "expert spears",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_exp_spear,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "ranger staff",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_ranger_staff,
      TAR_IGNORE,          POS_FIGHTING,
      NULL,
      75,
      PULSE_VIOLENCE,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "hunters knife",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_hunters_knife,
      75,
      PULSE_VIOLENCE,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "rapier",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_rapier,
      75,
      PULSE_VIOLENCE,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "carving",
      {53, 53, 53, 53, 53, 30, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_carving,
      75,
      PULSE_VIOLENCE,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   /*  Werv -rs */
   {
      "skull cleaver",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_skull_cleaver,
      TAR_IGNORE,          POS_FIGHTING,
      NULL,
      75,
      PULSE_VIOLENCE,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "riposte",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_riposte,
      0,
      0,
      "riposte",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "counter",
      {25, 53, 53, 27, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_counter,
      0,
      0,
      "counter strike",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "tame",
      {53, 53, 53, 53, 53, 25, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_tame,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "tracking",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_track,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "find water",
      {53, 53, 53, 53, 53, 18, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_find_water,
      0,
      0,
      "Water Symbol's deluge",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "shield cleave",
      {18, 53, 53, 22, 22, 26, 53, 53, 23, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_shield_cleave,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "darkshield",
      {53, 53, 53, 53, 53, 53, 53, 53, 23, 11, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_dark_shield,
      TAR_CHAR_DEFENSIVE,  POS_STANDING,
      &gsn_darkshield,
      20,
      PULSE_VIOLENCE,
      "",
      "The dark shield surrounding you fades away.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "dark focus",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_darkfocus,
      35,
      PULSE_VIOLENCE,
      "",
      "You lose your dark focus.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "spellcraft",
      {53, 53, 53, 53, 53, 53, 53,  1, 53,  1,  1},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_SLEEPING,
      &gsn_spellcraft,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "fear",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_fear,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      NULL,
      30,
      PULSE_VIOLENCE,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "power word fear",
      {53, 53, 53, 53, 53, 53, 53, 53, 32, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_power_word_fear,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_power_word_fear,
      20,
      PULSE_VIOLENCE,
      "",
      "You feel the fear plaguing your mind fade away.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "nightfall",
      {53, 53, 53, 53, 53, 53, 53, 53, 38, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_nightfall,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      NULL,
      75,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "decay corpse",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 15, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_decay_corpse,
      TAR_OBJ,             POS_STANDING,
      NULL,
      15,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "preserve",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 20, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_preserve,
      TAR_OBJ_INV,         POS_STANDING,
      NULL,
      10,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "imbue regeneration",
      {53, 53, 38, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_regeneration,
      TAR_CHAR_DEFENSIVE,  POS_STANDING,
      &gsn_imbue_regeneration,
      50,
      PULSE_VIOLENCE,
      "",
      "Your body stops regenerating.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "restoration",
      {53, 53, 35, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_restoration,
      TAR_CHAR_DEFENSIVE,  POS_STANDING,
      &gsn_restoration,
      50,
      PULSE_VIOLENCE,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "quiet movement",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_quiet_movement,
      0,
      0,
      "",
      "You stop moving quietly.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "iron resolve",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_iron_resolve,
      0,
      0,
      "",
      "Your mind wanders from its resolve.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "ambush",
      {53, 53, 53, 53, 53, 12, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_ambush,
      0,
      PULSE_VIOLENCE * 2,
      "surprise attack",
      "!Ambush!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "animate skeleton",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 15, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_animate_skeleton,
      TAR_IGNORE,          POS_STANDING,
      NULL,
      50,
      PULSE_VIOLENCE,
      "",
      "You feel your power over the dead return.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "mummify",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 35, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_mummify,
      TAR_IGNORE,          POS_STANDING,
      NULL,
      150,
      PULSE_VIOLENCE * 2,
      "",
      "You feel your power over the dead return.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "moving ambush",
      {53, 53, 53, 53, 53, 35, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_moving_ambush,
      0,
      0,
      "surprise attack",
      "!Ambush!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "pugil",
      {22, 53, 53, 26, 53, 24, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_pugil,
      0,
      PULSE_VIOLENCE * 2,
      "pugil",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "lash",
      {15, 13, 24, 53, 16, 19, 53, 17, 18, 25, 25},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_lash,
      0,
      PULSE_VIOLENCE * 2,
      "lash",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "protection heat cold",
      {53, 53, 53, 53, 53, 18, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_protection_heat_cold,
      0,
      PULSE_VIOLENCE,
      "",
      "You no longer feel protected from the elements.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "iron scales of the dragon",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_iron_scales_dragon,
      0,
      PULSE_VIOLENCE,
      "",
      "Your focus upon impenetrability wavers.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "evaluation",
      { 1,  1,  1,  1,  1,  1,  1, 53,  1, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_evaluation,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "enhanced damage",
      { 1, 20, 53,  1,  1, 15,  1, 53, 16, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  0,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_enhanced_damage,
      0,
      0,
      "",
      "!Enhanced Damage!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "fire mastery",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_firemastery,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "water mastery",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_watermastery,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "air mastery",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_airmastery,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "earth mastery",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_earthmastery,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "enhanced damage II",
      {34, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_enhanced_damage_two,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "staff",
      { 1, 53,  1,  1, 53,  1, 53, 1,  1,  1,  1},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_staff,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "tactics",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_tactics,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "rally",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_rally,
      0,
      0,
      "",
      "Your confidence to incite others has returned.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "backstab",
      {53,  1, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 0,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_backstab,
      0,
      PULSE_VIOLENCE * 2,
      "backstab",
      "!Backstab!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "strange form",
      {60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_strange_form,
      0,
      0,
      "",
      "You feel your normal form returning.",
      "You feel your normal form returning.",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         TRUE,    /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   {
      "cloak",
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_cloak_form,
      0,
      0,
      "",
      "The wind pulls your cloak away from your face.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         TRUE,    /* Keep after death */
         TRUE,    /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   {
      "cloak of bravery",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_cloak_brave,
      TAR_CHAR_SELF,       POS_STANDING,
      &gsn_cloak_brave,
      60,
      PULSE_VIOLENCE * 2,
      "",
      "Your cloak of spiritual bravery falls away.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "target",
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_target,
      0,
      0,
      "",
      "You are ready to select a new target.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   {
      "bloodmist",
      {30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_bloodmist,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_bloodmist,
      50,
      PULSE_VIOLENCE * 2,
      "bloodmist",
      "Your pores stop bleeding, and your vision clears.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "darkforge",
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_darkforge,
      TAR_OBJ_INV,         POS_STANDING,
      &gsn_darkforge,
      100,
      PULSE_VIOLENCE * 2,
      "dark forging",
      "The Shadow lends you the power to forge another weapon.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "darkforged fists",
      {25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_darkforge_fists,
      100,
      PULSE_VIOLENCE * 2,
      "life drain",
      "Your fists are no longer forged with shadows.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "soulcraft",
      {25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_soulcraft,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_soulcraft,
      100,
      PULSE_VIOLENCE,
      "dark rite",
      "You once again feel ready to forge your heart into a weapon.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   /* tainted spring and food moved to avoid conflict with taint -werv */
   {
      "taint",
      {35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35},
      { 0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0},
      spell_taint,
      TAR_OBJ_INV,         POS_STANDING,
      &gsn_taint,
      150,
      PULSE_VIOLENCE * 3,
      "",
      "You feel ready to taint another object.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "tainted spring",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_tainted_spring,
      TAR_IGNORE,          POS_STANDING,
      NULL,
      20,
      PULSE_VIOLENCE,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "tainted food",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_tainted_food,
      TAR_IGNORE,          POS_STANDING,
      NULL,
      5,
      PULSE_VIOLENCE,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "cloak of the Void",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_void,
      0,
      0,
      "power surge",
      "Your calm dissipates, and the darkness lifts.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   {
      "peace",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_peace,
      0,
      0,
      "peace",
      "",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   {
      "eternal flame",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_peace_brand,
      0,
      0,
      "eternal flame",
      "You feel ready to spread peace again.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   {
      "anvil brand",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_anvil_brand,
      0,
      0,
      "anvil brand",
      "You can call upon Gryleth again.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   {
      "justice brand",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_justice_brand,
      0,
      0,
      "justice brand",
      "The Bronze Scales tilt back to the middle.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   {
      "blessing of justice",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_justice_brand2,
      0,
      0,
      "justice brand",
      "The Bronze Scales tilt back to the middle.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   {
      "wrath of justice",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_justice_brand_wrath,
      0,
      0,
      "wrath of justice",
      "You no longer fear the wrath of justice.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   {
      "Owl Brand",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_owl_brand,
      0,
      0,
      "owl brand",
      "The Owl feels colder.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   {
      "Worm Curse",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_worm_brand,
      0,
      0,
      "curse of the Worm",
      "The curse of the Worm lifts from you.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   {
      "Blessing of the Worm",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_worm_hide,
      0,
      0,
      "blessing of the worm",
      "Your presence can be noticed again.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   {
      "Toothless Aura",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_toothless,
      0,
      0,
      "toothless aura",
      "The toothless aura fades away.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   {
      "Lemniscate Brand",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_lemniscate_brand,
      0,
      0,
      "lemniscate brand",
      "You feel yourself speed up.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   {
      "Battleaxe Haste",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_battleaxe_haste,
      0,
      0,
      "battleaxe haste",
      "Your battle surge wears off.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   {
      "Battleaxe Defense",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_battleaxe_defense,
      0,
      0,
      "battleaxe defense",
      "Your strengthened defenses recede.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   {
      "bite",
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_chimera_lion,
      0,
      0,
      "crushing bite",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "ram",
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_chimera_goat,
      0,
      0,
      "ramming",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "ramming",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_ramming,
      0,
      0,
      "ramming",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "tail",
      {15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_tail,
      0,
      PULSE_VIOLENCE * 2,
      "tail attack",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "coilsnake",
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_coil_snakes,
      0,
      0,
      "suffocation",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "throw",
      {53, 53, 53, 53, 53, 53, 15, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_throw,
      0,
      PULSE_VIOLENCE * 2,
      "throw",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "endure",
      {53, 53, 53, 53, 53, 53, 10, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_endure,
      0,
      PULSE_VIOLENCE,
      "",
      "You feel more susceptible to magic.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "dragon spirit",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_dragon_spirit,
      0,
      PULSE_VIOLENCE * 2,
      "",
      "Your spirit is no longer that of a dragon.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "palm",
      {53, 53, 53, 53, 53, 53, 30, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_palm,
      0,
      PULSE_VIOLENCE * 2,
      "palm",
      "You stop concentrating on fighting open handed.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "poison dust",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_poison_dust,
      0,
      0,
      "poison",
      "The poison in your blood is neutralised.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "blindness dust",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_blindness_dust,
      0,
      0,
      "",
      "You rub the dust in your eyes away.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "roll",
      {53, 53, 53, 53, 53, 53, 25, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_roll,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "block",
      {53, 53, 53, 53, 53, 53, 16, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_block,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "strangle",
      {53, 53, 53, 53, 53, 53, 53, 53, 25, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_strangle,
      0,
      PULSE_VIOLENCE * 3 / 2,
      "strangulation attempt",
      "Your neck feels better.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "strangle timer",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_strangle_timer,
      0,
      0,
      "",
      "You stop guarding your neck so carefully.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "nerve",
      {53, 53, 53, 53, 53, 53, 13, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_nerve,
      0,
      0,
      "nerve",
      "Your arm loses the numbness within it.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "follow through",
      {53, 53, 53, 53, 53, 53, 26, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_follow_through,
      0,
      0,
      "follow through kick",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "shield bash",
      {53, 53, 53, 32, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_shieldbash,
      0,
      PULSE_VIOLENCE * 2,
      "shield bash",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "bash",
      {12, 53, 53, 53, 13, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_bash,
      0,
      PULSE_VIOLENCE * 2,
      "bash",
      "!Bash!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "berserk",
      {18, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_berserk,
      0,
      PULSE_VIOLENCE * 2,
      "",
      "You feel your pulse slow down.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "wild fury",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_wildfury,
      0,
      PULSE_VIOLENCE * 2,
      "",
      "You are no longer so mad.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "killer instinct",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_killer_instinct,
      0,
      PULSE_VIOLENCE * 2,
      "",
      "You are no longer focused on the kill.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "toughen",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_toughen,
      0,
      PULSE_VIOLENCE * 2,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "rage",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_rage,
      0,
      PULSE_VIOLENCE * 2,
      "",
      "Your temper returns to normal.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "skull bash",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_skull_bash,
      0,
      PULSE_VIOLENCE * 2,
      "bashing",
      "You no longer feel disoriented.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "crushing blow",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_crushingblow,
      0,
      PULSE_VIOLENCE * 2,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "ground control",
      {29, 53, 53, 53, 53, 53, 27, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_ground_control,
      0,
      0,
      "ground control",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "circle stab",
      {53, 14, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 0,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_circle,
      0,
      PULSE_VIOLENCE * 3 / 2,
      "circle stab",
      "!Circlestab!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "spellbane",
      {23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_spellbane,
      0,
      0,
      "spellbane",
      "Your focus recedes and you no longer reflect spells.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "warcry",
      {16, 53, 53, 53, 53, 20, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_warcry,
      0,
      0,
      "",
      "The effects of your warcry die out.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "focus",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_focus,
      0,
      0,
      "",
      "You feel your killing focus leave you.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "battlecry",
      {20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_battlecry,
      0,
      0,
      "",
      "Your battle fury dies down.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "battle scream",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_battlescream,
      0,
      0,
      "",
      "Your fear of battle subsides.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "battle tactics",
      {32, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_battle_tactics,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "whirlwind",
      {35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_whirlwind,
      0,
      PULSE_VIOLENCE * 2,
      "whirlwind",
      "!Whirlwind!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "dirt kicking",
      { 4,  3, 53, 53,  4,  5,  4, 53,  4, 53, 53},
      { 1,  1,  0,  1,  1,  1,  1,  1,  1,  0,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_dirt,
      0,
      PULSE_VIOLENCE * 2,
      "kicked dirt",
      "You rub the dirt out of your eyes.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "smoke screen",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_smoke_screen,
      0,
      PULSE_VIOLENCE,
      "smoke bomb",
      "You can use another smoke bomb.",
      "The smoke clears.",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "disarm",
      {15, 17, 53, 16, 16, 17, 53, 53, 16, 53, 53},
      { 1,  1,  0,  1,  1,  1,  0,  0,  1,  0,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_disarm,
      0,
      PULSE_VIOLENCE * 2,
      "",
      "!Disarm!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "offhand disarm",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_offhand_disarm,
      0,
      PULSE_VIOLENCE * 2,
      "",
      "!Disarm!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "dodge",
      {13,  1,  5,  5,  5, 14,  1,  5,  1,  5,  5},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_dodge,
      0,
      0,
      "",
      "!Dodge!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "dual backstab",
      {53, 32, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_dual_backstab,
      0,
      0,
      "second backstab",
      "!DualBackstab!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "envenom",
      {53, 10, 53, 53, 53, 53, 53, 53, 21, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_RESTING,
      &gsn_envenom,
      0,
      PULSE_VIOLENCE,
      "",
      "!Envenom!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "sharpen",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_RESTING,
      &gsn_sharpen,
      0,
      PULSE_VIOLENCE,
      "",
      "!Sharpen!",
      "$p seems duller.",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "hand to hand",
      { 5,  7, 11,  6,  6,  9,  1, 15,  9, 12,  5},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_hand_to_hand,
      0,
      0,
      "",
      "!Hand to Hand!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "storm",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_storm,
      TAR_IGNORE,          POS_STANDING,
      &gsn_storm,
      100,
      PULSE_VIOLENCE * 2,
      "lightning bolt",
      "You can call another storm.",
      "The winds subside.",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "kick",
      { 8, 14, 53, 10, 10, 11,  2, 53, 13, 18,  9},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_kick,
      0,
      PULSE_VIOLENCE,
      "kick",
      "!Kick!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "parry",
      { 1, 11, 53,  1,  1,  1, 53, 53, 16, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_parry,
      0,
      0,
      "",
      "!Parry!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "rescue",
      { 1, 53, 53,  5, 53, 53, 53, 53, 53, 53, 53},
      { 1,  0,  0,  1,  0,  1,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_rescue,
      0,
      PULSE_VIOLENCE,
      "",
      "!Rescue!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "downstrike",
      {53, 28, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_downstrike,
      0,
      0,
      "downstrike",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "trip",
      { 9,  5, 53, 53, 13, 11,  7, 53, 11, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  0,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_trip,
      0,
      PULSE_VIOLENCE * 2,
      "trip",
      "!Trip!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "second attack",
      { 5, 12, 24,  7,  7,  8,  5, 28,  6, 21, 26},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_second_attack,
      0,
      0,
      "",
      "!Second Attack!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "third attack",
      {19, 25, 53, 24, 24, 25, 20, 53, 23, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_third_attack,
      0,
      0,
      "",
      "!Third Attack!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "fourth attack",
      {30, 53, 53, 53, 53, 53, 25, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_fourth_attack,
      0,
      0,
      "",
      "!Fourth Attack!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   /* non-combat skills */
   {
      "acute vision",
      {53, 53, 53, 53, 53, 11, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_acute_vision,
      0,
      PULSE_VIOLENCE,
      "",
      "Your vision weakens.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "barkskin",
      {53, 53, 53, 53, 53, 18, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_barkskin,
      0,
      PULSE_VIOLENCE,
      "",
      "The bark on your skin peels off and falls away.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "enlist",
      {35, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_enlist,
      0,
      0,
      "",
      "You decide to follow your own destiny and leave your company.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "animal call",
      {53, 53, 53, 53, 53, 20, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_animal_call,
      0,
      PULSE_VIOLENCE * 3,
      "",
      "You feel up to calling more animals.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "bear call",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_bear_call,
      0,
      PULSE_VIOLENCE,
      "",
      "You feel up to calling more bears.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "camouflage",
      {53, 53, 53, 53, 53,  8, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_camouflage,
      0,
      0,
      "",
      "!Camouflage!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "camp",
      {53, 53, 53, 53, 53, 20, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_RESTING,
      &gsn_camp,
      0,
      PULSE_VIOLENCE * 4,
      "",
      "!Camp!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "fast healing",
      { 7, 12,  9, 14, 14, 15,  5, 17, 13, 14,  8},
      { 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2},
      spell_null,
      TAR_IGNORE,          POS_SLEEPING,
      &gsn_fast_healing,
      0,
      0,
      "",
      "!Fast Healing!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "recovery",
      {20, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_SLEEPING,
      &gsn_recovery,
      0,
      0,
      "",
      "!Recovery!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "haggle",
      {14,  8, 11, 15, 15, 14, 22, 13,  9, 10, 13},
      { 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2},
      spell_null,
      TAR_IGNORE,          POS_RESTING,
      &gsn_haggle,
      0,
      0,
      "",
      "!Haggle!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "hide",
      {53,  1, 53, 53, 53, 53, 10, 53,  5, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_RESTING,
      &gsn_hide,
      0,
      PULSE_VIOLENCE,
      "",
      "!Hide!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "advanced hide",
      {53, 30, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_RESTING,
      &gsn_adv_hide,
      0,
      PULSE_VIOLENCE,
      "",
      "!Hide!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "expert hide",
      {53, 42, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_RESTING,
      &gsn_exp_hide,
      0,
      PULSE_VIOLENCE,
      "",
      "!Hide!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "entrench",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_RESTING,
      &gsn_entrench,
      0,
      PULSE_VIOLENCE,
      "",
      "!Hide!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "ransack",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_ransack,
      0,
      PULSE_VIOLENCE,
      "ransacking",
      "You are no longer protected from ransacking.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "bushwhack",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_bushwhack,
      0,
      PULSE_VIOLENCE,
      "surprise attack",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "dobribery",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_RESTING,
      &gsn_bribe,
      0,
      PULSE_VIOLENCE,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "laying hands",
      {53, 53, 53, 35, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_laying_hands,
      35,
      PULSE_VIOLENCE * 3,
      "",
      "You feel able to heal with your touch again.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "chi healing",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_chi_healing,
      35,
      PULSE_VIOLENCE * 2,
      "",
      "Your chi returns to normal.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "meditation",
      {16, 17,  6,  8,  8, 13,  2,  7, 12, 11,  7},
      { 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2},
      spell_null,
      TAR_IGNORE,          POS_SLEEPING,
      &gsn_meditation,
      0,
      0,
      "",
      "Meditation",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "trance",
      {53, 53, 15, 53, 53, 53, 53, 15, 53, 14, 14},
      { 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2},
      spell_null,
      TAR_IGNORE,          POS_SLEEPING,
      &gsn_trance,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "morph black dragon",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_morph_black,
      100,
      PULSE_VIOLENCE * 14 / 3,
      "",
      "You return to your natural form.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "morph white dragon",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_morph_white,
      100,
      PULSE_VIOLENCE * 14 / 3,
      "",
      "You return to your natural form.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "morph blue dragon",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_morph_blue,
      100,
      PULSE_VIOLENCE * 14 / 3,
      "",
      "You return to your natural form.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "morph green dragon",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_morph_green,
      100,
      PULSE_VIOLENCE * 14 / 3,
      "",
      "You return to your natural form.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "morph red dragon",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_morph_red,
      100,
      PULSE_VIOLENCE * 14 / 3,
      "",
      "You return to your natural form.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "morph archangel",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_morph_archangel,
      100,
      PULSE_VIOLENCE * 14 / 3,
      "",
      "You return to your natural form.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "morph winged",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_morph_winged,
      100,
      PULSE_VIOLENCE * 14 / 3,
      "",
      "You return to your natural form.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "breath dragon",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_breath_morph,
      20,
      PULSE_VIOLENCE,
      "",
      "!Breathe!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "lunge",
      {14, 53, 53, 18, 24, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_lunge,
      0,
      PULSE_VIOLENCE * 2,
      "lunge",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "impale",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_impale,
      0,
      PULSE_VIOLENCE,
      "impalement",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "thrust",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_thrust,
      0,
      PULSE_VIOLENCE * 2,
      "impaling thrust",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "critical strike",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_critical_strike,
      0,
      PULSE_VIOLENCE * 2,
      "critical strike",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "taunt",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_taunt,
      0,
      PULSE_VIOLENCE * 2,
      "starvation",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "black guard",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_SLEEPING,
      &gsn_blackjack_timer,
      0,
      0,
      "",
      "You stop guarding your head so carefully.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "shackles",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_shackles,
      0,
      0,
      "shackle attempt",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "blackjack",
      {53, 21, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_blackjack,
      0,
      PULSE_VIOLENCE * 3 / 2,
      "blackjack",
      "You come to with a pounding headache.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "counter parry",
      {24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_counter_parry,
      0,
      0,
      "counter parry",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "vigilance",
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_vigilance,
      40,
      PULSE_VIOLENCE,
      "",
      "Your vigilance relaxes.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "guard call",
      {20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_guard_call,
      TAR_IGNORE,          POS_STANDING,
      &gsn_guard_call,
      35,
      PULSE_VIOLENCE,
      "",
      "You can commandeer a guard contingent once again.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "backup",
      {20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_backup,
      TAR_IGNORE,          POS_STANDING,
      &gsn_backup,
      35,
      PULSE_VIOLENCE,
      "",
      "You feel you are assured backup again.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "peek",
      {53,  6, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_peek,
      0,
      0,
      "",
      "!Peek!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "pick lock",
      {53, 11, 53, 53, 53, 53, 53, 53, 13, 53, 53},
      { 0,  1,  0,  0,  0,  0,  0,  0,  1,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_pick_lock,
      0,
      PULSE_VIOLENCE,
      "",
      "!Pick!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "skin",
      {53, 53, 53, 53, 53, 22, 53, 53, 53, 53, 53},
      { 0,  0,  0,  0,  0,  1,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_skin,
      0,
      PULSE_VIOLENCE * 2,
      "",
      "You feel up to skinning another corpse.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "tanning",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_tanning,
      0,
      PULSE_VIOLENCE * 2,
      "",
      "You feel up to tanning another hide.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "butcher",
      {53, 53, 53, 53, 53, 14, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_butcher,
      0,
      PULSE_VIOLENCE,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "forest walk",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_forestwalk,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "sneak",
      {53,  4, 53, 53, 53, 53,  5, 53,  6, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_sneak,
      0,
      PULSE_VIOLENCE,
      "",
      "You no longer feel stealthy.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "steal",
      {53,  6, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 0,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_steal,
      0,
      PULSE_VIOLENCE * 2,
      "",
      "!Steal!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "plant",
      {53, 16, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 0,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_plant,
      0,
      PULSE_VIOLENCE * 2,
      "",
      "!Plant!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "offer",
      {22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_offer,
      0,
      PULSE_VIOLENCE * 2,
      "",
      "You are no longer worried about unsolicited offers.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "lore",
      {22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_RESTING,
      &gsn_lore,
      0,
      PULSE_VIOLENCE * 2,
      "",
      "!Lore!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "soulscry",
      {22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_RESTING,
      &gsn_soulscry,
      0,
      PULSE_VIOLENCE * 2,
      "",
      "!Soulscry!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "diagnose",
      {22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_RESTING,
      &gsn_diagnose,
      0,
      PULSE_VIOLENCE * 35 / 6,
      "",
      "!Diagnose!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "steel nerves",
      {15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_steel_nerves,
      0,
      PULSE_VIOLENCE,
      "",
      "Your nerves begin to settle down and you don't feel as invincible.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "multistrike",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_multistrike,
      0,
      PULSE_VIOLENCE * 2,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "scrolls",
      {53,  1,  1,  1,  5,  1,  1,  1,  5,  1,  1},
      { 0,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_scrolls,
      0,
      PULSE_VIOLENCE * 2,
      "",
      "!Scrolls!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "staves",
      {53,  1,  1,  1,  5,  1,  1,  1,  5,  1,  1},
      { 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_staves,
      0,
      PULSE_VIOLENCE,
      "",
      "!Staves!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "wands",
      {53, 53,  1, 53,  5, 53,  1,  1,  5,  1,  1},
      { 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_wands,
      0,
      PULSE_VIOLENCE,
      "",
      "!Wands!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "recall",
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      { 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_recall,
      0,
      PULSE_VIOLENCE,
      "",
      "You are able to recall again.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "channel",
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_channel,
      TAR_IGNORE,          POS_STANDING,
      &gsn_channel,
      100,
      PULSE_VIOLENCE * 2,
      "",
      "Your mind loses its mental strength and you feel less healthy.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "faith healing",
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_faith_healing,
      TAR_CHAR_DEFENSIVE,  POS_STANDING,
      &gsn_faith_healing,
      35,
      PULSE_VIOLENCE,
      "faith healing",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "safety",
      {20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_safety,
      TAR_IGNORE,          POS_FIGHTING,
      NULL,
      75,
      PULSE_VIOLENCE * 2,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "lifeline",
      {18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_life_line,
      TAR_CHAR_DEFENSIVE,  POS_STANDING,
      &gsn_lifeline,
      50,
      PULSE_VIOLENCE,
      "",
      "You feel the strength of your life line diminish.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "insignia",
      {15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_badge,
      TAR_IGNORE,          POS_STANDING,
      NULL,
      75,
      PULSE_VIOLENCE,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "getaway",
      {18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_getaway,
      30,
      PULSE_VIOLENCE,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "protectorate",
      {20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_protectorate_plate,
      TAR_IGNORE,          POS_STANDING,
      NULL,
      75,
      PULSE_VIOLENCE,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "cloak of transgression",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_outlaw_cloak,
      TAR_IGNORE,          POS_STANDING,
      &gsn_cloak_of_transgression,
      75,
      PULSE_VIOLENCE,
      "",
      "You are ready to create another Cloak of Transgression.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "robe of the light",
      {20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_robe_of_the_light,
      TAR_IGNORE,          POS_STANDING,
      NULL,
      30,
      PULSE_VIOLENCE,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "decoy",
      {15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_decoy,
      TAR_CHAR_DEFENSIVE,  POS_STANDING,
      &gsn_decoy,
      40,
      PULSE_VIOLENCE,
      "",
      "You are informed that more decoys are ready for your use.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "alarm",
      {53, 53, 53, 53, 53, 53, 53, 30, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_alarm,
      TAR_IGNORE,          POS_STANDING,
      NULL,
      50,
      PULSE_VIOLENCE,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "lookout",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_lookout,
      50,
      PULSE_VIOLENCE,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "holyshield",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_holyshield,
      TAR_CHAR_DEFENSIVE,  POS_STANDING,
      &gsn_holyshield,
      20,
      PULSE_VIOLENCE,
      "",
      "Your divine shield fades away.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "blade barrier",
      {53, 53, 30, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_blade_barrier,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_blade_barrier,
      20,
      PULSE_VIOLENCE,
      "blade barrier",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "warding",
      {15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_warding,
      TAR_CHAR_SELF,       POS_STANDING,
      &gsn_warding,
      75,
      PULSE_VIOLENCE,
      "",
      "Your runes of warding begin to lose substance.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "prevent healing",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_prevent_healing,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_prevent_healing,
      20,
      0,
      "",
      "You feel your health returning.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "aura of sustenance",
      {20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_aura_of_sustenance,
      50,
      PULSE_VIOLENCE * 3,
      "",
      "Your body begins to feel the bite of hunger once again.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "halo of the sun",
      {20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_halo_of_the_sun,
      50,
      PULSE_VIOLENCE / 2,
      "",
      "You feel the glowing halo of light recede.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "spiritblade",
      {15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_spiritblade,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_spiritblade,
      35,
      PULSE_VIOLENCE,
      "spiritblade",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "iceball",
      {53, 53, 53, 53, 29, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_iceball,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_iceball,
      20,
      PULSE_VIOLENCE,
      "iceball",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "retribution",
      {20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_retribution,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_retribution,
      25,
      PULSE_VIOLENCE,
      "wrath of justice",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "hand of redemption",
      {20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_hand_of_vengeance,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_hand_of_vengeance,
      25,
      PULSE_VIOLENCE,
      "clenched fist",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "wanted",
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_SLEEPING,
      &gsn_wanted,
      0,
      0,
      "",
      "All guards have been notified of your recent criminal transgressions.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "rebel",
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_SLEEPING,
      &gsn_rebel,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "bandage",
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_bandage,
      0,
      PULSE_VIOLENCE * 3,
      "",
      "You are once more able to administer battle aid.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "trophy",
      {21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_RESTING,
      &gsn_trophy,
      0,
      0,
      "",
      "You feel up to making a new trophy.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "shadowplane",
      {60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_SLEEPING,
      &gsn_shadowplane,
      0,
      0,
      "presence in the shadowplane",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "request",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_request,
      0,
      0,
      "",
      "Your contemplation of the good you have seen is over.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "battleshield",
      {20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_battleshield,
      0,
      0,
      "",
      "You feel up to forging a new shield.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "herb",
      {53, 53, 53, 53, 53, 10, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_herb,
      0,
      PULSE_VIOLENCE * 2,
      "",
      "The herbs look more plentiful.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "cleave",
      {53, 53, 53, 53, 30, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_cleave,
      0,
      PULSE_VIOLENCE * 2,
      "cleave",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "chaos mind",
      {12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_chaos_mind,
      TAR_CHAR_DEFENSIVE,  POS_FIGHTING,
      &gsn_chaos_mind,
      20,
      PULSE_VIOLENCE * 3 / 2,
      "",
      "Your thoughts seem much less clouded.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "revolt",
      {25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_revolt,
      TAR_CHAR_OFFENSIVE,  POS_STANDING,
      &gsn_revolt,
      40,
      PULSE_VIOLENCE * 3 / 2,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "misdirection",
      {20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_misdirection,
      TAR_CHAR_DEFENSIVE,  POS_FIGHTING,
      &gsn_misdirection,
      50,
      PULSE_VIOLENCE * 3 / 2,
      "",
      "You feel less disoriented now.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "riot",
      {15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_riot,
      0,
      PULSE_VIOLENCE,
      "",
      "Your furious rioting emotions subside.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "vanish",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_vanish,
      0,
      PULSE_VIOLENCE,
      "",
      "You feel ready to vanish once again.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "caltrops",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_caltrops,
      0,
      PULSE_VIOLENCE,
      "caltrop",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "fence",
      {13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_fence,
      0,
      PULSE_VIOLENCE,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "familiar",
      {25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_familiar,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_familiar,
      75,
      PULSE_VIOLENCE,
      "",
      "You feel up to summoning a new familiar.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "shadowgate",
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_shadowgate,
      75,
      PULSE_VIOLENCE * 3,
      "",
      "",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "eye of the predator",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_eye_of_the_predator,
      50,
      PULSE_VIOLENCE,
      "",
      "Your predatory awareness receedes.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "frostbolt",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 22},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_frostbolt,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      NULL,
      25,
      PULSE_VIOLENCE,
      "frostbolt",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "icelance",
      {53, 53, 53, 53, 53, 53, 53, 23, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_icelance,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      NULL,
      24,
      PULSE_VIOLENCE,
      "icelance",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "earthfade",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 39},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_earthfade,
      TAR_CHAR_SELF,       POS_STANDING,
      &gsn_earthfade,
      100,
      PULSE_VIOLENCE,
      "",
      "You slowly emerge from the earth.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "stealth",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_stealth,
      10,
      0,
      "",
      "You stop blending into the crowds.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "forge",
      {25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_forge,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_forge,
      15,
      PULSE_VIOLENCE * 2,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "forget",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 29, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_forget,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_forget,
      50,
      PULSE_VIOLENCE,
      "",
      "Your mind becomes clearer once again.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "earthbind",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 20},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_earthbind,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_earthbind,
      20,
      PULSE_VIOLENCE,
      "",
      "Your feet no longer feel bound to the ground.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "cremate",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 26, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_cremate,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      NULL,
      20,
      PULSE_VIOLENCE,
      "blast of fire",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "divine touch",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_divine_touch,
      TAR_CHAR_SELF,       POS_STANDING,
      &gsn_divine_touch,
      30,
      PULSE_VIOLENCE,
      "blessed mace",
      "Your hands lose their divine touch.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "transfer object",
      {53, 53, 53, 53, 53, 53, 53, 32, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_transfer_object,
      TAR_IGNORE,          POS_STANDING,
      NULL,
      30,
      PULSE_VIOLENCE,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "disintegrate",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_disintegrate,
      TAR_CHAR_OFFENSIVE,  POS_STANDING,
      &gsn_disintegrate,
      100,
      PULSE_VIOLENCE * 2,
      "disintegration",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "grounding",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 29},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_grounding,
      TAR_CHAR_SELF,       POS_STANDING,
      &gsn_grounding,
      70,
      PULSE_VIOLENCE,
      "",
      "You lose your grounding against electricty.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "shock sphere",
      {53, 53, 53, 53, 53, 53, 53, 18, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_shock_sphere,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_shock_sphere,
      20,
      PULSE_VIOLENCE,
      "shock sphere",
      "The ringing in your ears dies away.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "forage",
      {53, 53, 53, 53, 53, 25, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_forage,
      0,
      0,
      "",
      "You can benefit from the application of herbs again.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "assassinate",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_assassinate,
      0,
      PULSE_VIOLENCE * 2,
      "assassination attempt",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "defend",
      {29, 53, 53, 24, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_defend,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "intimidate",
      {53, 53, 53, 53, 36, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_intimidate,
      0,
      0,
      "",
      "You are vulnerable to intimidate again.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "escape",
      {53, 27, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_escape,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "charging retreat",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_charging_retreat,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "block retreat",
      {38, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_block_retreat,
      0,
      0,
      "",
      "You are no longer blocking retreats.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "deathstrike",
      {32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32},
      { 2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_deathstrike,
      0,
      0,
      "deathstrike",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "empower",
      {20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_empower,
      0,
      PULSE_VIOLENCE * 2,
      "",
      "You feel able to reforge dark matter once again.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "emaciation",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_emaciation,
      TAR_CHAR_OFFENSIVE,  POS_STANDING,
      NULL,
      100,
      PULSE_VIOLENCE * 2,
      "emaciation",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "cause starvation",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_cause_starvation,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      NULL,
      40,
      PULSE_VIOLENCE * 2,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "cause dehydration",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_cause_dehydration,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      NULL,
      50,
      PULSE_VIOLENCE * 2,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "vermin",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_vermin,
      TAR_CHAR_DEFENSIVE,  POS_FIGHTING,
      NULL,
      100,
      PULSE_VIOLENCE * 2,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "spores",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_spores,
      TAR_IGNORE,          POS_STANDING,
      NULL,
      100,
      PULSE_VIOLENCE * 2,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "nether shroud",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_nether_shroud,
      TAR_CHAR_DEFENSIVE,  POS_FIGHTING,
      &gsn_nether_shroud,
      50,
      PULSE_VIOLENCE * 2,
      "",
      "The field of negative energy around you dissipates.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "black mantle",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_black_mantle,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_black_mantle,
      50,
      PULSE_VIOLENCE * 2,
      "",
      "The dark cloud about you drifts away.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "bane",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_bane,
      TAR_IGNORE,          POS_FIGHTING,
      NULL,
      40,
      PULSE_VIOLENCE * 2,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "epidemic",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_epidemic,
      TAR_IGNORE,          POS_FIGHTING,
      NULL,
      50,
      PULSE_VIOLENCE * 2,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "steed",
      {20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_summon_steed,
      50,
      PULSE_VIOLENCE * 2,
      "",
      "You feel ready to call another steed.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "health",
      {25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_health,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_health,
      20,
      PULSE_VIOLENCE * 2,
      "",
      "You feel less healthy.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "health blessing",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_health_bless,
      TAR_CHAR_DEFENSIVE,  POS_STANDING,
      NULL,
      50,
      PULSE_VIOLENCE,
      "",
      "You are no longer blessed with health.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "blitz",
      {30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_blitz,
      0,
      PULSE_VIOLENCE * 2,
      "blitz",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "jump",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_jump,
      0,
      PULSE_VIOLENCE,
      "jump",
      "You are less wary of jump attacks.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "doublestrike",
      {35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_doublestrike,
      0,
      0,
      "second strike",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "support",
      {30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_support,
      TAR_IGNORE,          POS_STANDING,
      &gsn_support,
      75,
      PULSE_VIOLENCE * 2,
      "",
      "You feel ready to call for support again.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "devote",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_devote,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_devote,
      50,
      PULSE_VIOLENCE * 2,
      "",
      "You feel less devoted.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "lurk",
      {20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_lurk,
      0,
      0,
      "",
      "You stop lurking in the shadows.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "capture",
      {30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_capture,
      TAR_CHAR_DEFENSIVE,  POS_STANDING,
      NULL,
      40,
      PULSE_VIOLENCE * 2,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "swing",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_swing,
      0,
      PULSE_VIOLENCE,
      "swing",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "whiplash",
      {53, 53, 53, 53, 53, 20, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_whiplash,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      NULL,
      20,
      PULSE_VIOLENCE * 2,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "surround",
      {30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_surround,
      0,
      PULSE_VIOLENCE * 2,
      "bashing",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "ameteor",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_ameteor,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_ameteor,
      0,
      0,
      "flaming hot rocks",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "rot",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_rot,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_rot,
      35,
      PULSE_VIOLENCE,
      "rotting",
      "Your body stops rotting away.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "ray of enfeeblement",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_ray_enfeeblement,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_ray_of_enfeeblement,
      30,
      PULSE_VIOLENCE * 2,
      "brilliant ray",
      "You no longer feel feeble.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "cripple",
      {15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_cripple,
      0,
      PULSE_VIOLENCE * 2,
      "crippling blow",
      "You no longer feel crippled.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "drain blood",
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_drain,
      0,
      PULSE_VIOLENCE,
      "drain",
      "You no longer feel drained.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "dim-mak",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_dimmak,
      0,
      PULSE_VIOLENCE * 2,
      "nerve attack",
      "Your nerves feel better.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "distort time",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_distort_time,
      TAR_IGNORE,          POS_STANDING,
      &gsn_distort_time,
      50,
      PULSE_VIOLENCE * 2,
      "",
      "Time returns to normal around you.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "distort time faster",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_distort_time_faster,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "distort time slower",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_distort_time_slower,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "smite",
      {60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_DEAD,
      &gsn_smite,
      0,
      0,
      "divine wrath",
      "The gods lift their curse from you.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         TRUE,    /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   {
      "silver dragon armor",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_DEAD,
      &gsn_sdragon_armor,
      0,
      0,
      "",
      "The silver blessing of Lord Dielantha recedes.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   {
      "silver dragon valor",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_DEAD,
      &gsn_sdragon_valor,
      0,
      0,
      "",
      "Lord Dielantha's blessing of valor recedes.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   {
      "demon swarm",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_demon_swarm,
      20,
      PULSE_VIOLENCE * 2,
      "swarm of demons",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "pact",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_pact,
      TAR_CHAR_DEFENSIVE,  POS_STANDING,
      NULL,
      100,
      PULSE_VIOLENCE * 2,
      "",
      "The duration of your demonic protection is over. Renegotiate.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "conjure",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_conjure,
      0,
      PULSE_VIOLENCE * 2,
      "",
      "You have recovered from your conjuration attempt.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "gout of maggots",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_gout_maggots,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_gout_of_maggots,
      30,
      PULSE_VIOLENCE,
      "gout of maggots",
      "You taste larval bile again.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         TRUE,    /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "ancient plague",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_ancient_plague,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_ancient_plague,
      100,
      PULSE_VIOLENCE * 2,
      "ancient plague",
      "The wasting illness lifts.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "kung fu",
      {53, 53, 53, 53, 53, 53, 15, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_kung_fu,
      0,
      0,
      "hit",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "ninjitsu",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_ninjitsu,
      0,
      0,
      "hit",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "healing trance",
      {53, 53, 53, 53, 53, 53, 15, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_healing_trance,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "chant",
      {53, 53, 53, 53, 53, 53, 18, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_chant,
      0,
      0,
      "",
      "Your concentration relaxes and you stop chanting.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "iron will",
      {53, 53, 53, 53, 53, 53, 20, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_iron_will,
      0,
      0,
      "",
      "You stop focusing on your inner strength.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "side step",
      {53, 53, 53, 53, 53, 53, 24, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_side_step,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "fifth attack",
      {53, 53, 53, 53, 53, 53, 35, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_fifth_attack,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "harmony",
      {53, 53, 53, 53, 53, 53, 35, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_harmony,
      0,
      0,
      "",
      "Your trance ends and you feeled purified by the harmony of your own"
      " inner strength.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "spirit walk",
      {53, 53, 53, 53, 53, 53, 40, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_spiritwalk,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "open claw fighting",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_open_claw,
      0,
      0,
      "claw",
      "You no longer wear your claws.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "fists of fury",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_fists_fury,
      0,
      0,
      "",
      "You stop applying your inner will to fighting.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "pouncing tiger",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_pouncing_tiger,
      0,
      0,
      "pounce",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "spinning kick",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_spin_kick,
      0,
      PULSE_VIOLENCE * 2,
      "kick",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "fists of poison",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_fists_poison,
      0,
      0,
      "fang fist",
      "You stop applying your inner will to fighting.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "sore neck",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_stunning_strike_timer,
      0,
      0,
      "",
      "Your neck muscles feel less tense.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "stunning strike",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_stunning_strike,
      0,
      0,
      "nerve punch",
      "The nerves in your neck feel better.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "dance of venom",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_dance_venom,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "sixth attack",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_sixth_attack,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "fists of fire",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_fists_fire,
      0,
      0,
      "flaming punch",
      "You stop applying your inner will to fighting",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "dragon chant",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_dchant,
      0,
      0,
      "",
      "You cease to chant.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "iron palm",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_iron_palm,
      0,
      PULSE_VIOLENCE * 2,
      "iron palm",
      "You regain your resistance to damage.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "fists of divinity",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_fists_divine,
      0,
      0,
      "divine punch",
      "You stop applying your inner will to fighting",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "counter defense",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_counter_defense,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "judo",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_judo,
      0,
      0,
      "judo",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "knockdown",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_knockdown,
      0,
      0,
      "throw",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "acrobatics",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_acrobatics,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "tumble",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_tumble,
      0,
      0,
      "uppercut punch",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "fists of ice",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_fists_ice,
      0,
      0,
      "freezing punch",
      "You stop applying your inner will to fighting",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "herb high",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_high_herb,
      0,
      0,
      "herb high",
      "You come down from your high..",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "evasive dodge",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_evasive_dodge,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "jump kick",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_jump_kick,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "chi attack",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_chi_attack,
      0,
      0,
      "chi attack",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "subrank",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_subrank,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "strengthen bone",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_strengthen_bone,
      TAR_CHAR_SELF,       POS_STANDING,
      &gsn_strengthen_bone,
      100,
      PULSE_VIOLENCE,
      "",
      "Your bones feel normal again.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "boiling blood",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_boiling_blood,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_boiling_blood,
      35,
      PULSE_VIOLENCE * 2,
      "boiling blood",
      "Your vital fluids return to normal.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "strengthen construct",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_strengthen_construct,
      TAR_CHAR_DEFENSIVE,  POS_STANDING,
      &gsn_strengthen_construct,
      75,
      PULSE_VIOLENCE * 2,
      "",
      "The augmentation wears off.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "bone golem",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_bone_golem,
      TAR_IGNORE,          POS_STANDING,
      NULL,
      100,
      PULSE_VIOLENCE * 2,
      "",
      "You feel up to fashioning another bone golem.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "dragonmount",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_dragonmount,
      0,
      0,
      "",
      "You feel strong enough to summon another dragonmount.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "blind fighting",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_blind_fighting,
      5,
      PULSE_VIOLENCE,
      "",
      "Your concentration breaks, and awareness fades away.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "hang",
      {35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_hang,
      0,
      PULSE_VIOLENCE * 2,
      "hang",
      "!Hang!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "tinker",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_tinker,
      100,
      PULSE_VIOLENCE * 2,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "forge weapon",
      {52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_forge_weapon,
      100,
      PULSE_VIOLENCE * 2,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "lightblast",
      {20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_lightblast,
      TAR_CHAR_DEFENSIVE,  POS_FIGHTING,
      &gsn_lightblast,
      30,
      PULSE_VIOLENCE * 3 / 2,
      "blast of Light",
      "!Lightblast!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "sense evil",
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_sense_evil,
      40,
      PULSE_VIOLENCE,
      "",
      "You relax from smelling the taint of evil.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "armor of god",
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_armor_of_god,
      50,
      PULSE_VIOLENCE,
      "",
      "The holy armor about you expands and shatters into dissolving"
      " fragments.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "guardian angel",
      {25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_guardian_angel,
      120,
      PULSE_VIOLENCE * 2,
      "",
      "You sense Heaven's gate open to you, allowing you a new guardian"
      " angel.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "lightbind",
      {20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_lightbind,
      TAR_CHAR_OFFENSIVE,  POS_STANDING,
      &gsn_lightbind,
      120,
      PULSE_VIOLENCE * 3 / 2,
      "",
      "The circle of Light around you dissolves!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         TRUE,    /* Absorb works when cast by player */
         TRUE,    /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   /* at the end so blindfold doesnt confuse with blind */
   {
      "bind",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_bind,
      0,
      0,
      "binding",
      "You manage to remove your bindings.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "gag",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_gag,
      0,
      0,
      "gagging",
      "You manage to remove your gag.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "blindfold",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_blindfold,
      0,
      0,
      "blind folding",
      "You manage to remove your blind fold.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "helper robe",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_helper_robe,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "worship",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_worship,
      0,
      0,
      "",
      "You are left with the memories of your faith.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "detriment",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_detriment,
      0,
      0,
      "",
      "You feel as if a weight has been lifted off of you.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         TRUE,    /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   {
      "punishment",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_punishment,
      0,
      0,
      "",
      "The heavens release you from the curse.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         TRUE,    /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   {
      "Plague of the Worm",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_plague_of_the_worm,
      0,
      0,
      "sickness",
      "Your sores vanish.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         TRUE,    /* Keep after death */
         TRUE,    /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   {
      "worm_damage",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_worm_damage,
      TAR_CHAR_OFFENSIVE,  POS_FIGHTING,
      &gsn_worm_damage,
      0,
      0,
      ""/* Damage noun is hard-coded */,
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   {
      "Divine Inferno",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_divine_inferno,
      0,
      0,
      "burning flesh",
      "The flames flicker and die out.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         TRUE,    /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   {
      "Will of Power",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_will_power,
      0,
      0,
      "essence",
      "Your mortal mind grows tired and you must once again\n\r"
      "focus upon the Dread-Lord's blessing of Power.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   {
      "Will of Dread",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_will_dread,
      0,
      0,
      "",
      "Your mortal mind grows tired and you must once again\n\r"
      "focus upon the Dread-Lord's blessing of Dread.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   {
      "Will of Oblivion",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_will_oblivion,
      0,
      0,
      "essence",
      "Your mortal mind grows tired and you must once again\n\r"
      "focus upon the Dread-Lord's blessing of Oblivion.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   {
      "Mantle of Oblivion",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_mantle_oblivion,
      0,
      0,
      "",
      "The mantle surrounding you begins to grow weak and\n\r"
      "you may once more heal normally.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   {
      "telekinesis",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_telekinesis,
      0,
      18,
      "telekinesis",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "wind shear",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_wind_shear,
      0,
      18,
      "wind shear",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "elemental gateway",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_elemental_gateway,
      TAR_IGNORE,          POS_STANDING,
      NULL,
      100,
      PULSE_VIOLENCE,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "Freedom",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_mob_timer,
      0,
      0,
      "",
      "You feel ready to throw off the bonds of oppression again.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   {
      "Freedom_pain",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_mob_pain,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   {
      "mark of purity",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_purity,
      0,
      PULSE_VIOLENCE / 2,
      "mark of purity",
      "The mark of purity dries and flakes off.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "resurrection",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_resurrection,
      0,
      0,
      "",
      "You resolidify into the being you once were.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   {
      "holy beacon",
      {53, 53, 53,  5, 53, 53, 53, 53, 53, 53, 53},
      { 0,  0,  0,  1,  0,  0,  0,  0,  0,  0,  0},
      spell_holy_beacon,
      TAR_IGNORE,          POS_FIGHTING,
      NULL,
      25,
      0,
      "",
      "You can once again conjure a holy beacon for help.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "scribe",
      {30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_scribe,
      30,
      PULSE_VIOLENCE,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "seize",
      {30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_seize,
      TAR_CHAR_DEFENSIVE,  POS_STANDING,
      &gsn_seize,
      50,
      PULSE_VIOLENCE,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "virulent cysts",
      {30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_virulent_cysts,
      TAR_CHAR_SELF,       POS_STANDING,
      &gsn_virulent_cysts,
      50,
      PULSE_VIOLENCE,
      "virulent cysts",
      "The diseased flesh sinks back into your skin.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "esurience",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_esurience,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_esurience,
      65,
      PULSE_VIOLENCE,
      "defiling",
      "You feel up to eating and drinking once more.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "psi storm",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_psistorm,
      0,
      PULSE_VIOLENCE,
      "psi storm",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "dance of staves",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_dancestaves,
      0,
      PULSE_VIOLENCE,
      "dance of staves",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "acidfire",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_acidfire,
      0,
      PULSE_VIOLENCE,
      "acidfire",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "siren screech",
      {30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_siren_screech,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_siren_screech,
      30,
      PULSE_VIOLENCE * 3 / 2,
      "shrill cry",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "wail wind",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_wail_wind,
      0,
      0,
      "chilling winds",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "devour",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_devour,
      0,
      PULSE_VIOLENCE,
      "devour",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "phantasmal force",
      {32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_phantasmal_force,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_phantasmal_force,
      100,
      0,
      "",
      "You feel ready to project phantasmal forces again.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "phantasmal images",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_phantasmal_force_area,
      0,
      0,
      "",
      "You feel ready to project phantasmal forces again.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "phantasmal illusion",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_phantasmal_force_illusion,
      0,
      0,
      "",
      "You feel ready to project phantasmal forces again.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "intangibility",
      {38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_intangibility,
      TAR_CHAR_SELF,       POS_FIGHTING,
      &gsn_intangibility,
      200,
      0,
      "",
      "Your feel sluggish as your body becomes once again tangible.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         TRUE,    /* Spellbane reflects when cast by player */
         TRUE,    /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "stress",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_intangibility_timer,
      0,
      0,
      "",
      "Your body has healed enough to suffer the stress of intangibility once more.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "smelly",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_smelly,
      0,
      0,
      "",
      "You do not smell like a sewer anymore.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "care bear stare",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_carebearstare,
      0,
      PULSE_VIOLENCE,
      "beam of light",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "krazy eyes",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_krazyeyes,
      0,
      PULSE_VIOLENCE,
      "bolts of energy",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   /*
      Placeholder only.  Skill is not coded,
      mana/lag is not set, spell_ is not set, nor is
      gsn
   */
   {
      "prismatic sphere",
      {45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_prismatic_sphere,
      TAR_IGNORE,          POS_STANDING,
      &gsn_prismatic_sphere,
      0,
      0,
      "",
      "",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "simulacrum",
      {45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_simulacrum,
      TAR_IGNORE,          POS_STANDING,
      &gsn_simulacrum,
      200,
      PULSE_VIOLENCE * 2,
      "",
      "",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "rite of darkness",
      {45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_rite_of_darkness,
      TAR_IGNORE,
      POS_STANDING,
      &gsn_rite_of_darkness,
      500,
      PULSE_VIOLENCE * 2,
      "Rite of Darkness",
      "As the Rite of Darkness abates, you rip your soul free of Shal'ne-Sir!",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "soul fatigue",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,
      POS_STANDING,
      &gsn_rite_of_darkness_timer,
      0,
      0,
      "",
      "You feel up to resisting the corruption of the planes of hell again.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "rite of darkness taint",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,
      POS_STANDING,
      &gsn_rite_of_darkness_taint,
      0,
      0,
      "",
      "You feel your soul slip free from the grasp of darkness.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "dracolich evil",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,
      POS_STANDING,
      &gsn_dracolich_evil,
      0,
      0,
      "bony claw",
      "Your claws feel ready to strike again.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "dracolich bloodlust",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,
      POS_STANDING,
      &gsn_dracolich_bloodlust,
      0,
      0,
      "bloodlust",
      "Your soul becomes your own once more.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "perception",
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_perception,
      0,
      0,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "muster",
      {20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_muster,
      35,
      PULSE_VIOLENCE,
      "",
      "You can commandeer a guard contingent once again.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "call of duty",
      {35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_call_of_duty,
      TAR_IGNORE,          POS_STANDING,
      &gsn_call_of_duty,
      35,
      PULSE_VIOLENCE,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "sonic belch",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_sonicbelch,
      0,
      18,
      "sonic belch",
      "You do not feel nauseated anymore.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "troll fart",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_trollfart,
      0,
      PULSE_VIOLENCE,
      "troll fart",
      "You do not feel sick anymore.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "tai chi",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_taichi,
      0,
      0,
      "tai chi",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "Wrath of Drinlinda",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_DEAD,
      &gsn_drinlinda_attack_brand,
      0,
      0,
      "wrath of Drinlinda",
      "",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   {
      "Shield of Innocence",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_drinlinda_defense_brand,
      0,
      0,
      "",
      "Your discipline lessens and the Shield of Innocence fades away.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   {
      "star shower",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_star_shower,
      0,
      0,
      "star shower",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "emblem of law",
      {20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_emblem_law,
      75,
      PULSE_VIOLENCE,
      "",
      "Your emblem crumbles into dust.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "Seal of Justice",
      {20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_seal_of_justice,
      TAR_IGNORE,          POS_STANDING,
      &gsn_seal_of_justice,
      75,
      PULSE_VIOLENCE,
      "",
      "Your seal has become worn and tarnished.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "avatar assault",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_davatar_assault,
      0,
      18,
      "avatar assault",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   {
      "stalking",
      {20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_stalking,
      0,
      0,
      "",
      "You cease stalking.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "field surgery",
      {30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_field_surgery,
      0,
      PULSE_VIOLENCE,
      "",
      "",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "deathfire missiles",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_deathfire_missiles,
      0,
      PULSE_VIOLENCE,
      "deathfire missiles",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "abyssal claws",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_abyssal_claws,
      0,
      PULSE_VIOLENCE,
      "abyssal claws",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "destruction shockwave",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_destruction_shockwave,
      0,
      PULSE_VIOLENCE,
      "destructive shockwave",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "destructive fury",
      {61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61},
      { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
      spell_null,
      TAR_IGNORE,          POS_DEAD,
      &gsn_drithentir_attack_brand,
      0,
      0,
      "destructive fury",
      "",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "Vampiric Aura",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_drithentir_vampiric_brand,
      0,
      0,
      "vampiric drain",
      "Your thirst for destruction fades and your vampiric aura evaporates.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   {
      "penetrating thrust",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_penetratingthrust,
      0,
      18,
      "penetrating thrust",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   {
      "wagon strike",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_wagon_strike,
      0,
      0,
      "wagon strike",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         FALSE,   /* Copy when Cloned? */
      },
   },
   {
      "megaslash",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_megaslash,
      0,
      18,
      "megaslash",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "cunning strike",
      {37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_cunning_strike,
      0,
      PULSE_VIOLENCE * 2,
      "cunning strike",
      "",  /* Wear off messages are handled seperately in update.c */
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "thugs stance",
      {40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_thugs_stance,
      0,
      PULSE_VIOLENCE * 2,
      "",
      "Your muscles tire and you come out of your stance.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
      {
      "thugs stance timer",
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_thugs_stance_timer,
      0,
      0,
      "",
      "Your muscles feel well enough to attempt your stance again.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "misinformation",
      {25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_misinformation,
      50,
      PULSE_VIOLENCE,
      "",
      "Your false trails have been covered, you may be found again.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "silver tongue",
      {45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_silver_tongue,
      75,
      PULSE_VIOLENCE * 2,
      "",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "bribe",
      {33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_marauder_bribe,
      0,
      0,
      "",
      "You are informed that your agreement with the guildmaster has expired.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "song of shadows",
      {38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_song_of_shadows,
      50,
      PULSE_VIOLENCE,
      "",
      "You are no longer faded into the shadows.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "soothing voice",
      {35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_soothing_voice,
      25,
      PULSE_VIOLENCE * 3 / 2,
      "",
      "You can once again sway the emotions of another.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "gate call",
      {30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_gate_call,
      75,
      PULSE_VIOLENCE,
      "",
      "Your senses become keen again to find another trapdoor.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "guild cloth",
      {30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_guild_cloth,
      75,
      PULSE_VIOLENCE,
      "",
      "",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "incendiary device",   /* Marauder Guildcloth Bomb */
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_incendiary_device,
      0,
      0,
      "incendiary blast",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "healing salve",   /* Marauder Guildcloth Healing Salve */
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_healing_salve,
      0,
      0,
      "healing salve",
      "The effects of the healing salve leave your body.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "sonic whistle",   /* Marauder Guildcloth Sonic Whistle */
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_sonic_whistle,
      0,
      0,
      "sonic whistle",
      "",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "itching powder",   /* Marauder Guildcloth Itching Powder */
      {53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_itching_powder,
      0,
      0,
      "itching powder",
      "The irritatation slowly fades away.",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "dance of the rogue",
      {27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_FIGHTING,
      &gsn_dance_of_the_rogue,
      15,
      PULSE_VIOLENCE * 3 / 2,
      "strike",
      "!Dance!",
      "",
      {
         FALSE,   /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
   {
      "eyes of the bandit",
      {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
      { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1},
      spell_null,
      TAR_IGNORE,          POS_STANDING,
      &gsn_eyes_of_the_bandit,
      30,
      PULSE_VIOLENCE,
      "",
      "You grow tired of constantly appraising others.",
      "",
      {
         TRUE,    /* Cannot be removed by check_dispel (harmony) */
         FALSE,   /* Keep after death */
         FALSE,   /* Ignores non-specific astrip */
      },
      {
         FALSE,   /* Spellbane reflects when cast by player */
         FALSE,   /* Spellbane reflects when cast by mob    */
         FALSE,   /* Absorb works when cast by player */
         FALSE,   /* Absorb works when cast by mob */
         FALSE,   /* Casting is silent */
         FALSE,   /* Double lag until 15 levels later */
         TRUE,    /* Copy when Cloned? */
      },
   },
};

const sh_int MAX_SKILL = sizeof(skill_table) / sizeof(struct skill_type);

const   struct  group_type      group_table     [MAX_GROUP]     =
{
   {
      "class basics",       { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
      { "sword", "axe", "dagger", "flail", "mace", "polearm",
      "shield block", "spear", "whip", "bash", "berserk", "dirt kicking",
      "disarm", "dodge", "enhanced damage", "hand to hand", "kick",
      "parry", "rescue", "trip", "second attack", "third attack",
      "firestream", "fireshield", "iceshield", "dark dream", "embalm",
      "shadowself", "bless weapon", "barkskin", "alarm", "nightwalk", "need",
      "blade barrier", "holy fire", "warcry", "lash", "evaluation", "flash",
      "circle stab", "turn undead", "concatenate", "camouflage", "ambush",
      "frostbolt",    "icelance",    "earthfade", "forget", /* "earthbind", */
      "cremate", "divine touch", "transfer object", "disintegrate",
      "grounding", "shock sphere", "forage", "assassinate",
      "staff", "pugil", "enhanced damage II", "animate skeleton",
      "mummify", "protection heat cold", "transfer object",
      "defend", "intimidate", "escape", "moving ambush", "soulbind",
      "absorb", "wrath", "dark wrath", "acute vision", "laying hands",
      "dual backstab", "power word kill", "power word stun", "evil eye",
      "web", "animate dead", "skin", "camp", "fourth attack",
      "animate object", "demand", "lifebane", "deathspell",
      "morph white dragon", "morph black dragon", "morph blue dragon",
      "morph green dragon", "morph red dragon", "shield bash",
      "morph archangel", "morph winged", "flood room",
      "breath dragon", "summon void elemental", "summon fire elemental", "summon water elemental",
      "crush", "ground control", "palm", "endure", "follow through",
      "throw", "nerve", "poison dust", "blindness dust", "block",
      "roll",    "strangle", "hellforge", "archery", "advanced archery",
      "summon air elemental", "summon earth elemental", "drain", "tsunami",
      "earthmaw", "disenchant armor", "disenchant weapon", "dual wield",
      "windwall", "butcher", "shadowself", "utter heal",
      "cone of cold", "timestop", "consecrate",
      "wither", "shadowstrike",
      "protective shield", "animal call", "lunge", "blackjack",
      "trance", "herb", "cleave", "wither", "iceball",
      "lesser golem", "ranger staff", "greater golem",
      "preserve", "power word fear", "nightfall", "prevent healing",
      "decay corpse", "imbue regeneration", "restoration",
      "undead drain", "atrophy", "fire and ice",
      "counter", "bear call", "enlist", "downstrike", "spellcraft",
      "tame", "darkshield", "find water", "shield cleave",
      "curse weapon", "talk to dead", "focus",

      "recharge", "recovery",
      "fast healing", "haggle", "meditation", "recall", "backstab",
      "envenom", "camouflage", "hide", "advanced hide", "expert hide",  "peek", "pick lock",
      "sneak", "steal", "plant", "scrolls",
      "acid blast", "armor", "bless", "blindness",
      "burning hands", "call lightning", "calm", "cancellation", "colour spray",
      "continual light", "control weather", "create food", "create rose",
      "create spring", "create water", "cure blindness", "cure critical",
      "cure disease", "cure light", "cure poison", "cure serious", "curse",
      "demonfire", "detect evil", "detect good", "detect hidden",
      "detect invis", "detect magic", "detect poison", "dispel evil",
      "dispel good", "dispel magic", "earthquake", "enchant armor",
      "enchant weapon", "energy drain", "faerie fire", "faerie fog",
      "farsight", "fireball", "fireproof", "flamestrike", "fly", "frenzy",
      "gate", "giant strength", "harm", "haste", "heal", "heat metal",
      "holy word", "identify", "infravision", "invisibility", "know alignment",
      "lightning bolt", "locate object", "magic missile", "mass healing",
      "mass invis", "pass door", "plague", "poison", "protection evil",
      "protection good", "ray of truth", "refresh", "remove curse",
      "sanctuary", "chromatic shield", "shield", "shocking grasp", "sleep", "slow",
      "stone skin", "summon", "teleport", "ventriloquate", "weaken",
      "word of recall", "power word recall", "cause critical", "cause light", "cause serious",
      "chain lightning", "change sex", "charm person", "chill touch",
      "ice pox", "staves", "wands", "kung fu", "healing trance",
      "side step", "harmony", "fifth attack", "spirit walk",
      "chant", "iron will", "subrank", "curse room", "advanced daggers",
      "expert daggers", "whiplash", "carving", "dual parry", "block retreat",
      "battle tactics", "holy beacon" }
   },
   {
      "class default",      { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
      { "" }
   },
   {
      "ancient", {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      { "insight", "cripple", "target", "shadowgate",
      "darkforge", "cloak" }
   },
   {
      "light",    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      { "lifeline", "sense evil", "lightblast", "armor of god", "guardian angel" }
      /*
      { "sunbolt", "aura of sustenance", "halo of the sun", "prevent",
      "life", "safety", "lifeline", "lightshield" }
      */
   },
   {
      "arcana",  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      { "brew", "familiar", "channel", "venueport", "spiritblade" ,
      "team spirit"}
   },
   {
      "crusader",  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      { "spellbane", "battlecry", "whirlwind", "steel nerves" , "vigilance",
      "bandage", "trophy"}
   },
   {
      "enforcer", {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {"trace", "true sight",  "guard call", "wanted" }
   },
   {
      "outlaw",  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      { "cloak of transgression", "aura of defiance", "stealth",
      "silence", "smoke screen", "fence", "backup", "decoy", "lurk" }
   },
   {
      "brethren", {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {"demon swarm", "pact", "conjure", "portal"}
   }
};

MOON_DATA moon_table[MAX_MOONS] =
{
   {
      "Lomenthiel",
      45 * HOURS_PER_DAY,
      0
   },
   {
      "Erohach   ",
      30 * HOURS_PER_DAY,
      0
   },
   {
      "Gu        ",
      15 * HOURS_PER_DAY,
      0
   },
};

const char* moon_phase[MAX_MOON_PHASES] =
{
   " is full.",
   " is in waning gibbous.",
   " is in last quarter.",
   " is in waning crescent.",
   " is new.",
   " is in waxing crescent.",
   " is in first quarter.",
   " is in waxing gibbous.",
};

/*
struct food_data
{
   const sh_int level;
   const sh_int mana;
   const char* god_limit;
   const char* clan_limit;
   const char* name;
   const char* short_desc;
   const char* long_desc;
   const char* material;
   const bool allow_magic;
   const sh_int align;
   const bool brand_only;
};
*/

const FOOD_DATA food_table[] =
{
   /*
   %s is replaced by holy or magic, depending on if
   they were supplicating when creating it
   */
   {
      0,  /* level */
      0,  /* extra mana */
      NULL,  /* God limit */
      NULL,  /* Clan limit */
      "mushroom", /* name */
      "a %s mushroom",  /* short desc */
      "A delicious %s mushroom is here.",  /* long desc */
      "plant",  /* material */
      TRUE,  /* Allow non supplicaters to make this */
      ALLOW_GOOD | ALLOW_NEUTRAL | ALLOW_EVIL,  /* Aligns */
      FALSE,  /* Branded only */
   },
   {
      20,  /* level */
      5,  /* extra mana */
      NULL,  /* God limit */
      NULL,  /* Clan limit */
      "bread", /* name */
      "half a loaf of %s waybread",  /* short desc */
      "A sliced loaf of %s bread is here.",  /* long desc */
      "bread",  /* material */
      TRUE,  /* Allow non supplicaters to make this */
      ALLOW_GOOD | ALLOW_NEUTRAL | ALLOW_EVIL,  /* Aligns */
      FALSE,  /* Branded only */
   },

   {
      0,  /* level */
      0,  /* extra mana */
      "'Krazar'",  /* God limit */
      NULL,  /* Clan limit */
      "chewing tobacco", /* name */
      "a bit of edible chewing tobacco",  /* short desc */
      "A bit of edible chewing tobacco is here.",  /* long desc */
      "food",  /* material */
      FALSE,  /* Allow non supplicaters to make this */
      ALLOW_GOOD | ALLOW_NEUTRAL | ALLOW_EVIL,  /* Aligns */
      FALSE,  /* Branded only */
   },

   {
      10,  /* level */
      5,  /* extra mana */
      "'Fizzfaldt'",  /* God limit */
      NULL,  /* Clan limit */
      "cheese", /* name */
      "a small chunk of %s cheese",  /* short desc */
      "A delicious wedge of %s cheese is here.",  /* long desc */
      "cheese",  /* material */
      TRUE,  /* Allow non supplicaters to make this */
      ALLOW_GOOD | ALLOW_NEUTRAL | ALLOW_EVIL,  /* Aligns */
      FALSE,  /* Branded only */
   },
   {
      25,  /* level */
      6,  /* extra mana */
      "'Aityhrein'",  /* God limit */
      NULL,  /* Clan limit */
      "sausage", /* name */
      "a %s sausage",  /* short desc */
      "A delicious %s sausage is here.",  /* long desc */
      "pork, fat, and onions",  /* material */
      TRUE,  /* Allow non supplicaters to make this */
      ALLOW_GOOD | ALLOW_NEUTRAL | ALLOW_EVIL,  /* Aligns */
      FALSE,  /* Branded only */
   },
   {
      15,  /* level */
      1,  /* extra mana */
      "'Drinlinda'Fizzfaldt'",  /* God limit */
      NULL,  /* Clan limit */
      "bunny cookie", /* name */
      "a bunny shaped cookie",  /* short desc */
      "A delicious %s cookie in the shape of a bunny is here.",  /* long desc */
      "food",  /* material */
      TRUE,  /* Allow non supplicaters to make this */
      ALLOW_GOOD | ALLOW_NEUTRAL | ALLOW_EVIL,  /* Aligns */
      FALSE,  /* Branded only */
   },
   {
      35,  /* level */
      10,  /* extra mana */
      "'Drinlinda'", /* God limit */
      NULL,  /* Clan limit */
      "triple chocolate cake", /* name */
      "a %s triple chocolate cream cake", /* short desc */
      "A %s triple chocolate cream cake lies here.", /* long desc */
      "bread", /* material */
      TRUE, /* Allow non supplicaters to make this */
      ALLOW_GOOD | ALLOW_NEUTRAL | ALLOW_EVIL,  /* Aligns */
      FALSE,  /* Branded only */
   },
   {
      45,  /* level */
      10,  /* extra mana */
      "'Drinlinda'",  /* God limit */
      NULL,  /* Clan limit */
      "square watermelon",  /* name */
      "a %s square watermelon",  /* short desc */
      "A %s square watermelon lies here.",  /* long desc */
      "plant",  /* material */
      TRUE,  /* Allow non supplicaters to make this */
      ALLOW_GOOD | ALLOW_NEUTRAL | ALLOW_EVIL,  /* Aligns */
      FALSE,  /* Branded only */
   },
   {
      51,  /* level */
      15,  /* extra mana */
      "'Fizzfaldt'Drinlinda'",  /* God limit */
      NULL,  /* Clan limit */
      "large orange",  /* name */
      "a %s large orange",  /* short desc */
      "A %s large orange sits here.",  /* long desc */
      "plant",  /* material */
      TRUE,  /* Allow non supplicaters to make this */
      ALLOW_GOOD | ALLOW_NEUTRAL | ALLOW_EVIL,  /* Aligns */
      FALSE,  /* Branded only */
   },
   {
      10,  /* level */
      15,  /* extra mana */
      "'Fizzfaldt'Drinlinda'",  /* God limit */
      NULL,  /* Clan limit */
      "carrot",  /* name */
      "a %s carrot",  /* short desc */
      "A %s carrot sits here.",  /* long desc */
      "plant",  /* material */
      TRUE,  /* Allow non supplicaters to make this */
      ALLOW_GOOD | ALLOW_NEUTRAL | ALLOW_EVIL,  /* Aligns */
      FALSE,  /* Branded only */
   },
   {
      23,  /* level */
      15,  /* extra mana */
      "'Drinlinda'",  /* God limit */
      NULL,  /* Clan limit */
      "lettuce",  /* name */
      "a %s head of lettuce",  /* short desc */
      "A %s head of lettuce sits here.",  /* long desc */
      "plant",  /* material */
      TRUE,  /* Allow non supplicaters to make this */
      ALLOW_GOOD | ALLOW_NEUTRAL | ALLOW_EVIL,  /* Aligns */
      FALSE,  /* Branded only */
   },
   {
      20,  /* level */
      5,  /* extra mana */
      "'Drithentir'",  /* God limit */
      NULL,  /* Clan limit */
      "cow shaped loaf of bread",  /* name */
      "a cow shaped loaf of %s bread",  /* short desc */
      "A loaf of %s bread shaped like a cow sits here.",  /* long desc */
      "bread",  /* material */
      TRUE,  /* Allow non supplicaters to make this */
      ALLOW_GOOD | ALLOW_NEUTRAL | ALLOW_EVIL,  /* Aligns */
      FALSE,  /* Branded only */
   },
   {
      25,  /* level */
      10,  /* extra mana */
      "'Drithentir'",  /* God limit */
      NULL,  /* Clan limit */
      "beating heart",  /* name */
      "a beating heart",  /* short desc */
      "A beating heart pulses here.",  /* long desc */
      "meat",  /* material */
      TRUE,  /* Allow non supplicaters to make this */
      ALLOW_GOOD | ALLOW_NEUTRAL | ALLOW_EVIL,  /* Aligns */
      FALSE,  /* Branded only */
   },
   {
      54,  /* level */
      0,  /* extra mana */
      "'Fizzfaldt'",  /* God limit */
      NULL,  /* Clan limit */
      "rock small granite", /* name */
      "a small rock",  /* short desc */
      "A small chunk of granite is here.",  /* long desc */
      "granite",  /* material */
      TRUE,  /* Allow non supplicaters to make this */
      ALLOW_GOOD | ALLOW_NEUTRAL | ALLOW_EVIL,  /* Aligns */
      TRUE,  /* Branded only */
   },
   {
      54,  /* level */
      0,  /* extra mana */
      "'Fizzfaldt'",  /* God limit */
      NULL,  /* Clan limit */
      "small pumice pebble rock", /* name */
      "a small pumice pebble",  /* short desc */
      "A small pebble of pumice is here.",  /* long desc */
      "pumice",  /* material */
      TRUE,  /* Allow non supplicaters to make this */
      ALLOW_GOOD | ALLOW_NEUTRAL | ALLOW_EVIL,  /* Aligns */
      TRUE,  /* Branded only */
   },
   {
      40,  /* level */
      5,  /* extra mana */
      "'Xyza'",  /* God limit */
      NULL,  /* Clan limit */
      "chocolate cookies recipe plate", /* name */
      "a plate's worth of chocolate chip cookies (Xyza's Special Recipe)",  /* short desc */
      "A plate of delicious newly baked chocolate chip cookies sits here.",  /* long desc */
      "flour and sugar and eggs and butter and chocolate and secret stuff",  /* material */
      FALSE,  /* Allow non supplicaters to make this */
      ALLOW_GOOD | ALLOW_NEUTRAL | ALLOW_EVIL,  /* Aligns */
      FALSE,  /* Branded only */
   },
   {
      1,  /* level */
      0,  /* extra mana */
      "'Riallus'",  /* God limit */
      NULL,  /* Clan limit */
      "twinkie", /* name */
      "a Twinkie",  /* short desc */
      "A golden, cream filled snack cake lies here.",  /* long desc */
      "heavenly cake with cream filling",  /* material */
      FALSE,  /* Allow non supplicaters to make this */
      ALLOW_GOOD | ALLOW_NEUTRAL | ALLOW_EVIL,  /* Aligns */
      FALSE,  /* Branded only */
   },
   {
      1,  /* level */
      5,  /* extra mana */
      NULL,  /* God limit */
      "'DRE'",  /* Clan limit */
      "elf burger elfburger charred", /* name */
      "a lightly charred elfburger",  /* short desc */
      "A lightly charred pile of ground elven meat.",  /* long desc */
      "elf meat",  /* material */
      TRUE,  /* Allow non supplicaters to make this */
      ALLOW_EVIL,  /* Aligns */
      FALSE,  /* Branded only */
   },
   {
      10,  /* level */
      0,  /* extra mana */
      "'Joja'",  /* God limit */
      NULL,  /* Clan limit */
      "gold apple golden", /* name */
      "a holy golden apple",  /* short desc */
      "You see a delicious holy golden apple here.",  /* long desc */
      "apple",  /* material */
      FALSE,  /* Allow non supplicaters to make this */
      ALLOW_GOOD | ALLOW_NEUTRAL | ALLOW_EVIL,  /* Aligns */
      FALSE,  /* Branded only */
   },
   {
      10,  /* level */
      0,  /* extra mana */
      "'Joja'",  /* God limit */
      NULL,  /* Clan limit */
      "pomegranate", /* name */
      "a holy pomegranate",  /* short desc */
      "You see a delicious holy pomegranate here.",  /* long desc */
      "pomegranate",  /* material */
      FALSE,  /* Allow non supplicaters to make this */
      ALLOW_GOOD | ALLOW_NEUTRAL | ALLOW_EVIL,  /* Aligns */
      FALSE,  /* Branded only */
   },
   {
      15,  /* level */
      1,  /* extra mana */
      "'Anthatal'",  /* God limit */
      NULL,  /* Clan limit */
      "sandwich", /* name */
      "a ham and cheese sandwich",  /* short desc */
      "A tasty %s ham and cheese sandwich is here.",  /* long desc */
      "food",  /* material */
      TRUE,  /* Allow non supplicaters to make this */
      ALLOW_GOOD | ALLOW_NEUTRAL,  /* Aligns */
      FALSE,  /* Branded only */
   },
   {
      10,  /* level */
      0,  /* extra mana */
      "'Illunus'",  /* God limit */
      NULL,  /* Clan limit */
      "steak", /* name */
      "a big juicy steak",  /* short desc */
      "A big juicy %s steak lies here.",  /* long desc */
      "meat",  /* material */
      TRUE,  /* Allow non supplicaters to make this */
      ALLOW_GOOD | ALLOW_NEUTRAL | ALLOW_EVIL,  /* Aligns */
      FALSE,  /* Branded only */
   },

   {
      35,  /* level */
      1,  /* extra mana */
      "'Nixalis'",  /* God limit */
      NULL,  /* Clan limit */
      "mage tenders", /* name */
      "a plate of fried mage tenders",  /* short desc */
      "A %s plate of fried mage tenders is here.",  /* long desc */
      "food",  /* material */
      FALSE,  /* Allow non supplicaters to make this */
      ALLOW_GOOD | ALLOW_NEUTRAL | ALLOW_EVIL,  /* Aligns */
      TRUE,  /* Branded only */
   },

   /* Final entry REQUIRED */
   {
      0,  /* level */
      0,  /* extra mana */
      NULL,  /* God limit */
      NULL,  /* Clan limit */
      NULL, /* name */
      NULL,  /* short desc */
      NULL,  /* long desc */
      NULL,  /* material */
      TRUE,  /* Allow non supplicaters to make this */
      ALLOW_GOOD | ALLOW_NEUTRAL | ALLOW_EVIL,  /* Aligns */
      FALSE,  /* Branded only */
   },
};

/*
struct spring_data
{
   const sh_int level;
   const sh_int mana;
   const char* god_limit;
   const char* clan_limit;
   const char* name;
   const char* liq_type;
   const char* short_desc;
   const char* long_desc;
   const char* short_desc_magic;
   const char* long_desc_magic;
   const bool allow_magic;
   const sh_int align;
   const bool brand_only;
};
*/

const SPRING_DATA spring_table[] =
{
   /*
   %s is replaced by holy or magic, depending on if
   they were supplicating when creating it
   */
   {
      0,  /* level */
      0,  /* extra mana */
      NULL,  /* God limit */
      NULL,  /* Clan limit */
      "water", /* name */
      "water",  /* liq_type */
      "a spring of holy water",  /* short desc */
      "A spring of holy water flows here.",  /* long desc */
      "a magical spring",  /* short desc magic */
      "A magical spring flows here.",  /* long desc magic */
      TRUE,  /* Allow non supplicaters to make this */
      ALLOW_GOOD | ALLOW_NEUTRAL | ALLOW_EVIL,  /* Aligns */
      FALSE,  /* Branded only */
   },
   {
      30,  /* level */
      15,  /* extra mana */
      "'Fizzfaldt'",  /* God limit */
      NULL,  /* Clan limit */
      "orange juice", /* name */
      "orange juice",  /* liq_type */
      "a spring of orange juice",  /* short desc */
      "A spring of orange juice flows here.",  /* long desc */
      NULL,  /* short desc magic */
      NULL,  /* long desc magic */
      TRUE,  /* Allow non supplicaters to make this */
      ALLOW_GOOD | ALLOW_NEUTRAL | ALLOW_EVIL,  /* Aligns */
      FALSE,  /* Branded only */
   },
   {
      40,  /* level */
      5,  /* extra mana */
      "'Fizzfaldt'Drinlinda'",  /* God limit */
      NULL,  /* Clan limit */
      "milk", /* name */
      "milk",  /* liq_type */
      "a spring of holy milk",  /* short desc */
      "A spring of holy milk flows here.",  /* long desc */
      "a magical spring of milk",  /* short desc magic */
      "A magical spring of milk flows here.",  /* long desc magic */
      TRUE,  /* Allow non supplicaters to make this */
      ALLOW_GOOD | ALLOW_NEUTRAL | ALLOW_EVIL,  /* Aligns */
      FALSE,  /* Branded only */
   },
   {
      20,  /* level */
      3,  /* extra mana */
      "'Drinlinda'",  /* God limit */
      NULL,  /* Clan limit */
      "lemonade", /* name */
      "lemonade",  /* liq_type */
      "a spring of holy lemonade",  /* short desc */
      "A spring of holy lemonade flows here.",  /* long desc */
      "a magical spring of lemonade",  /* short desc magic */
      "A magical spring of lemonade flows here.",  /* long desc magic */
      TRUE,  /* Allow non supplicaters to make this */
      ALLOW_GOOD | ALLOW_NEUTRAL | ALLOW_EVIL,  /* Aligns */
      FALSE,  /* Branded only */
   },
   {
      51,  /* level */
      20,  /* extra mana */
      "'Drinlinda'",  /* God limit */
      NULL,  /* Clan limit */
      "strawberry juice",  /* name */
      "strawberry juice",  /* liq_type */
      "a spring of holy strawberry juice",  /* short desc */
      "A spring of holy strawberry juice flows here.",  /* long desc */
      "a magical spring of strawberry juice",  /* short desc magic */
      "A magical spring of strawberry juice flows here.",  /* long desc magic */
      TRUE,  /* Allow non supplicaters to make this */
      ALLOW_GOOD | ALLOW_NEUTRAL | ALLOW_EVIL,  /* Aligns */
      FALSE,  /* Branded only */
   },
   {
      45,  /* level */
      20,  /* extra mana */
      "'Drinlinda'",  /* God limit */
      NULL,  /* Clan limit */
      "sacred blood",  /* name */
      "Drinlinda's sacred blood",  /* liq_type */
      "a spring of Drinlinda's sacred blood",  /* short desc */
      "A spring of Drinlinda's sacred blood flows like water here.",  /* long desc */
      NULL,  /* short desc magic */
      NULL,  /* long desc magic */
      FALSE,  /* Allow non supplicaters to make this */
      ALLOW_GOOD | ALLOW_NEUTRAL | ALLOW_EVIL,  /* Aligns */
      TRUE,  /* Branded only */
   },
   {
      25,  /* level */
      3,  /* extra mana */
      "'Drithentir'",  /* God limit */
      NULL,  /* Clan limit */
      "blood",  /* name */
      "blood",  /* liq_type */
      "a holy spring of blood",  /* short desc */
      "A holy spring of blood flows here.",  /* long desc */
      "a magical spring of blood",  /* short desc magic */
      "A magical spring of blood flows here.",  /* long desc magic */
      TRUE,  /* Allow non supplicaters to make this */
      ALLOW_GOOD | ALLOW_NEUTRAL | ALLOW_EVIL,  /* Aligns */
      FALSE,  /* Branded only */
   },
   {
      50,  /* level */
      50,  /* extra mana */
      "'Drinlinda'Fizzfaldt'",  /* God limit */
      NULL,  /* Clan limit */
      "strawberry milk",  /* name */
      "strawberry milk",  /* liq_type */
      "a spring of holy strawberry milk",  /* short desc */
      "A spring of holy strawberry milk flows here.",  /* long desc */
      "a magical spring of strawberry milk",  /* short desc magic */
      "A magical spring of strawberry milk flows here.",  /* long desc magic */
      TRUE,  /* Allow non supplicaters to make this */
      ALLOW_GOOD | ALLOW_NEUTRAL | ALLOW_EVIL,  /* Aligns */
      FALSE,  /* Branded only */
   },
   {
      51,  /* level */
      70,  /* extra mana */
      "'Fizzfaldt'",  /* God limit */
      NULL,  /* Clan limit */
      "chocolate milk",  /* name */
      "chocolate milk",  /* liq_type */
      "a spring of divine chocolate milk",  /* short desc */
      "A spring of divine chocolate milk flows here.",  /* long desc */
      "a spring of magical chocolate milk",  /* short desc magic */
      "A spring of magical chocolate milk flows here.",  /* long desc magic */
      TRUE,  /* Allow non supplicaters to make this */
      ALLOW_GOOD | ALLOW_NEUTRAL | ALLOW_EVIL,  /* Aligns */
      FALSE,  /* Branded only */
   },
   {
      40,  /* level */
      20,  /* extra mana */
      "'Fizzfaldt'",  /* God limit */
      NULL,  /* Clan limit */
      "steak water elemental",  /* name */
      "water",  /* liq_type */
      "a steak of a water elemental",  /* short desc */
      "A steak of a water elemental is here.",  /* long desc */
      NULL,  /* short desc magic */
      NULL,  /* long desc magic */
      TRUE,  /* Allow non supplicaters to make this */
      ALLOW_GOOD | ALLOW_NEUTRAL | ALLOW_EVIL,  /* Aligns */
      TRUE,  /* Branded only */
   },
   {
      20,  /* level */
      5,  /* extra mana */
      "'Xyza'",  /* God limit */
      NULL,  /* Clan limit */
      "spring wine elvish",  /* name */
      "elvish wine",  /* liq_type */
      "a holy spring of elvish wine",  /* short desc */
      "A holy spring filled with delicious elvish wine is here.",  /* long desc */
      NULL,  /* short desc magic */
      NULL,  /* long desc magic */
      FALSE,  /* Allow non supplicaters to make this */
      ALLOW_GOOD | ALLOW_NEUTRAL | ALLOW_EVIL,  /* Aligns */
      FALSE,  /* Branded only */
   },
   {
      0,  /* level */
      0,  /* extra mana */
      "'Joja'",  /* God limit */
      NULL,  /* Clan limit */
      "honey mead spring",  /* name */
      "honey mead",  /* liq_type */
      "a holy spring of honey mead",  /* short desc */
      "The sweet scent of honey mead fills the air from a holy spring of honey mead.",  /* long desc */
      NULL,  /* short desc magic */
      NULL,  /* long desc magic */
      FALSE,  /* Allow non supplicaters to make this */
      ALLOW_GOOD | ALLOW_NEUTRAL | ALLOW_EVIL,  /* Aligns */
      FALSE,  /* Branded only */
   },
   {
      0,  /* level */
      0,  /* extra mana */
      "'Krazar'",  /* God limit */
      NULL,  /* Clan limit */
      "honeysuckle nectar",  /* name */
      "honeysuckle nectar",  /* liq_type */
      "a spring of holy honeysuckle nectar",  /* short desc */
      "A spring of holy honeysuckle nectar fills the air with a sweet scent.",  /* long desc */
      NULL,  /* short desc magic */
      NULL,  /* long desc magic */
      FALSE,  /* Allow non supplicaters to make this */
      ALLOW_GOOD | ALLOW_NEUTRAL | ALLOW_EVIL,  /* Aligns */
      FALSE,  /* Branded only */
   },
   {
      15,  /* level */
      5,  /* extra mana */
      "'Anthatal'",  /* God limit */
      NULL,  /* Clan limit */
      "green tea",  /* name */
      "green tea",  /* liq_type */
      "a spring of holy green tea",  /* short desc */
      "A spring of holy green tea flows here.",  /* long desc */
      "a magical spring of green tea",  /* short desc magic */
      "A magical spring of green tea flows here.",  /* long desc magic */
      TRUE,  /* Allow non supplicaters to make this */
      ALLOW_GOOD | ALLOW_NEUTRAL,  /* Aligns */
      FALSE,  /* Branded only */
   },
   {
      35,  /* level */
      5,  /* extra mana */
      "'Nixalis'",  /* God limit */
      NULL,  /* Clan limit */
      "blood of the Asp",  /* name */
      "blood of the Asp",  /* liq_type */
      "a spring of the blood of the Asp",  /* short desc */
      "A spring of the blood of the Asp.",  /* long desc */
      "a magical spring of the blood of the Asp",  /* short desc magic */
      "A magical spring of the blood of the Asp.",  /* long desc magic */
      FALSE,  /* Allow non supplicaters to make this */
      ALLOW_GOOD | ALLOW_NEUTRAL | ALLOW_EVIL,  /* Aligns */
      TRUE,  /* Branded only */
   },


   /* Final entry REQUIRED */
   {
      0,  /* level */
      0,  /* extra mana */
      NULL,  /* God limit */
      NULL,  /* Clan limit */
      NULL, /* name */
      NULL, /* liq_type */
      NULL,  /* short desc */
      NULL,  /* long desc */
      NULL,  /* short desc magic */
      NULL,  /* long desc magic */
      TRUE,  /* Allow non supplicaters to make this */
      ALLOW_GOOD | ALLOW_NEUTRAL | ALLOW_EVIL,  /* Aligns */
      FALSE,  /* Branded only */
   },
};

const sh_int* scribe_spell_exclusion_table []   =
{
   /* &gsn_scribe, <- is a skill, not a spell */
   &gsn_flame_scorch,
   &gsn_siren_screech,
   &gsn_prismatic_sphere,
   &gsn_phantasmal_force,
   &gsn_intangibility,
   &gsn_venueport,
   &gsn_simulacrum,
   &gsn_seize,
   /* &gsn_hang, <- is a skill, not a spell */
   &gsn_virulent_cysts,
   &gsn_rite_of_darkness,
   &gsn_familiar,
   &gsn_spiritblade,
   &gsn_team_spirit,
   &gsn_channel,
   /* &gsn_soulscry, <- is a skill, not a spell */
   NULL
};

const int seize_spell_exception_table[]	=
{
   0,
   -1000
};

char*    pencolor_table[MAX_PEN_COLOR] =
{
   "black",
   "red",
   "blue",
   "green",
   "yellow",
   "orange",
   "purple",
   "pink",
   "brown",
   "white",
   NULL,
};

char*    inkwell_table[MAX_INKWELLS] =
{
   "ink",
   "paint",
   "lead",
   NULL,
};
