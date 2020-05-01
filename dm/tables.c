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

static const char rcsid[] = "$Id: tables.c,v 1.79 2004/11/25 08:52:20 fizzfaldt Exp $";

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <time.h>
#include "merc.h"
#include "tables.h"

/*  Just here for reference
struct house_type
{
   char*    name;
   char*    who_name;
   char*    who_list_name;
   char*    item_name;
   bool     independent;
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
};
*/

SKILL_LIST_TYPE old_house_skills[99] =
{
   /*
      Old house skills/spells.  Put here so they are
      removed with autopurges/house closings
      for anyone who does not show up for a very long time.
   */
   {"judgement",     -1, 75, FALSE, ALLOW_ALL, 0},
   {"silence",       -1, 75, FALSE, ALLOW_ALL, 0},
   {"propaganda",    -1, 75, FALSE, ALLOW_ALL, 0},
   {"kidnap",        -1, 75, FALSE, ALLOW_ALL, 0},
   {"true sight",    -1, 75, FALSE, ALLOW_ALL, 0},
   {"lightshield",   -1, 75, FALSE, ALLOW_ALL, 0},
   {"repent",        -1, 75, FALSE, ALLOW_ALL, 0},
   {"emblem of law", -1, 75, FALSE, ALLOW_ALL, 0},
   {NULL,             0,  0, FALSE, ALLOW_ALL, 0}
};

/*
   for houses
   Note: Do not delete entries that changes the HOUSE_X defines as well
   as the # required in the H room house sets in the house .are files.

   New entries can be appended to the bottom of the list without side
   effect
*/
struct house_type house_table[MAX_HOUSE] =
{
   /* independent should be FALSE if is a real house */
   {
      "",             "",           "",         "",        TRUE,
      0, 0, 0, 0, 0,
      {
         { NULL, 0, 0, TRUE, 0, 0 }
      },
      "", "", "", "", "", "", "", "", "", "",
      "", "", "", "", "", "",
      FALSE, 0, 0, "", FALSE, FALSE, FALSE, FALSE,
      (ROSTER*)NULL,
      0,
      0
   },
   {
      "ancient",      "[ANCIENT] ",  "ANCT",    "Dagger",        FALSE,
      KEY_ANCIENT, ROOM_VNUM_ANCIENT, MOB_VNUM_ANCIENT, OBJ_VNUM_ANCIENT, MOB_VNUM_ANCIENT_STATUE,
      {
         {"shroud of pro", -1, 75, TRUE,  ALLOW_EVIL, 0},
         {"shadowgate",    -1, 75, TRUE,  ALLOW_EVIL, 0},
         {"insight",       -1, 75, TRUE,  ALLOW_EVIL, 0},
         {"cloak",         -1, 75, TRUE,  ALLOW_EVIL, 0},
         {"target",        -1, 75, TRUE,  ALLOW_EVIL, 0},
         {"cripple",       -1, 75, TRUE,  ALLOW_EVIL, 0},
         {"darkforge",     -1, 75, TRUE,  ALLOW_EVIL, 0},
         {"drain blood",   -1, 75, TRUE,  ALLOW_EVIL, 0},
         /*
         Anything with FALSE gets removed automatically..
         {"dark focus",    -1, 75, FALSE, ALLOW_EVIL, 0},
         */
         {NULL,             0,  0, FALSE, ALLOW_EVIL, 0}
      },
      "[ANCIENT] : Greetings %s, the Hunt awaits you.\n\r",
      "[ANCIENT] : %s has joined the Hunt.\n\r",
      "[ANCIENT] : Farewell %s, the Hunt awaits your return.\n\r",
      "[ANCIENT] : %s has left from the Hunt.\n\r",
      "Someone dares to defile our Temple!  My soul thirsts for their blood!",
      "May the darkness conceal you.",
      "",
      "",
      "Another already holds the sacred item. Seek them.",
      "Another already holds the sacred item. Seek them.",
      "Another already holds the sacred item. Seek them.",
      "You have won this time. Take the Dagger and be gone.",
      "You have won this time. Take the Dagger.",
      "You have won this time. Take the Dagger.",
      "anc_balance", "anc_morale",
      TRUE, 0, 60, "Xurinos Drithentir Aityhrein", TRUE, TRUE, FALSE, FALSE,
      (ROSTER*)NULL,
      ANCIENT_ONLY,
      0
   },
   {
      "light",        "[LIGHT] ",       "LGHT",    "Heart",        FALSE,
      KEY_LIGHT, ROOM_VNUM_LIGHT, MOB_VNUM_LIGHT, OBJ_VNUM_LIGHT, MOB_VNUM_LIGHT_STATUE,
      {
         {"lifeline",         -1, 75, TRUE,  ALLOW_GOOD, 0},
         {"sense evil",       -1, 75, TRUE,  ALLOW_GOOD, 0},
         {"lightblast",       -1, 75, TRUE,  ALLOW_GOOD, 0},
         {"armor of god",     -1, 75, TRUE,  ALLOW_GOOD, 0},
         {"guardian angel",   -1, 75, TRUE,  ALLOW_GOOD, 0},
         {NULL,                0,  0, FALSE, ALLOW_GOOD, 0}
      },
      "[LIGHT] : Greetings, %s, Warrior of Light.\n\r",
      "[LIGHT] : Greetings, %s, Warrior of Light.\n\r",
      "[LIGHT] : The lands darken a little more as %s leaves the realms.\n\r",
      "[LIGHT] : The lands darken a little more as %s leaves the realms.\n\r",
      "Warriors unite!  The Sanctum of Light has been breached!",
      "Welcome, Warrior of the Light.",
      "folds his wings back and bows gracefully.",
      "",
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      "lif_balance", "lif_morale",
      TRUE, 0, 60, "Dielantha Melykurion Adorno Morrbway", TRUE, TRUE, FALSE, FALSE,
      (ROSTER*)NULL,
      LIGHT_ONLY,
      0
   },
   {
      "arcana",       "[ARCANA] ",     "ARCH",    "Crystal",        FALSE,
      KEY_ARCANA, ROOM_VNUM_ARCANA, MOB_VNUM_ARCANA, OBJ_VNUM_ARCANA, MOB_VNUM_ARCANA_STATUE,
      {
         {NULL,             0,  0, FALSE, ALLOW_ALL, 0}
      },
      "[ARCANA] : The crystal of power hums briefly as you enter the lands.\n\r",
      "[ARCANA] : The crystal of power hums briefly as %s enters the lands.\n\r",
      "[ARCANA] : The crystal of power hums briefly as you leave the lands.\n\r",
      "[ARCANA] : The crystal of power hums briefly as %s leaves the lands.\n\r",
      "Intruder!  Intruder!",
      "Greetings Magi of Arcana.",
      "",
      "",
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      "arc_balance", "arc_morale",
      TRUE, 0, 60, "Xyza", TRUE, TRUE, FALSE, FALSE,
      (ROSTER*)NULL,
      ARCANA_ONLY,
      0
   },
   {
      "crusader",     "[CRUSADER] ",   "CRUS",    "Gauntlet",        FALSE,
      KEY_CRUSADER, ROOM_VNUM_CRUSADER, MOB_VNUM_CRUSADER, OBJ_VNUM_CRUSADER, MOB_VNUM_CRUSADER_STATUE,
      {
         {"bandage",          -1, 75, TRUE,  ALLOW_ALL, 0},
         {"spellbane",        -1, 75, TRUE,  ALLOW_ALL, 0},
         {"trophy",           -1, 75, TRUE,  ALLOW_ALL, 0},
         {"vigilance",        -1, 75, TRUE,  ALLOW_ALL, 0},
         {"battlecry",        -1, 75, TRUE,  ALLOW_ALL, 0},
         {"whirlwind",        -1, 75, TRUE,  ALLOW_ALL, 0},
         {"steel nerves",     -1, 75, TRUE,  ALLOW_ALL, 0},
         {"recall",            1, 75, TRUE,  ALLOW_ALL, 0},
         /*
         Anything with FALSE gets removed automatically..
         {"mark of purity",   -1, 75, FALSE, ALLOW_ALL, 0},
         */
         {"lore",             -1, 75, TRUE,  ALLOW_ALL, 3},
         {NULL,                0,  0, FALSE, ALLOW_ALL, 0}
      },
      "[CRUSADER] : Welcome %s. You bring us strength.\n\r",
      "[CRUSADER] : Welcome %s. You bring us strength.\n\r",
      "[CRUSADER] : Farewell %s. May your strength return to us soon.\n\r",
      "[CRUSADER] : Farewell %s. May your strength return to us soon.\n\r",
      "Vanquish the Intruder!  To the Death!",
      "Welcome warrior of the Crusades.",
      "",
      "",
      "Another already holds the War Gauntlet.  If ye seek war, then find it.",
      NULL,
      NULL,
      "You have declared war on the house.  Go now.",
      "It is good that we have you on our side, $N.",
      "It is good that we have you on our side, $N.",
      "cru_balance", "cru_morale",
      TRUE, 0, 60, "Darkwood Joja Tyberius Restin Nixalis Krazar Maelstrom", TRUE, TRUE, FALSE, FALSE,
      (ROSTER*)NULL,
      CRUSADER_ONLY,
      0
   },
   {
      "justicar",     "[JUSTICAR] ",   "JSTC",    "Book",        FALSE,
      KEY_ENFORCER, ROOM_VNUM_ENFORCER, MOB_VNUM_ENFORCER, OBJ_VNUM_ENFORCER, MOB_VNUM_ENFORCER_STATUE,
      {
         {"hand of redemption",  -1, 75,  TRUE,  ALLOW_ALL, 0},
         {"trace",               -1, 75,  TRUE,  ALLOW_ALL, 0},
         {"muster",              -1, 75,  TRUE,  ALLOW_ALL, 0},
         {"seal of justice",     -1, 75,  TRUE,  ALLOW_ALL, 0},
         {"perception",          -1, 75,  TRUE,  ALLOW_ALL, 0},
         {"call of duty",        -1, 75,  TRUE,  ALLOW_ALL, 0},
         {NULL,                   0,  0,  FALSE, ALLOW_ALL, 0}
      },
      "[JUSTICAR] : The law grows stronger with your arrival.\n\r",
      "[JUSTICAR] : The law grows stronger with the arrival of %s.\n\r",
      "[JUSTICAR] : May you return to protect the lawful and protect the streets soon %s.\n\r",
      "[JUSTICAR] : %s signs off from duty and leaves the chaotic streets.\n\r",
      "Raiders attempt to breech the stronghold!  JUSTICARS DEFEND!",
      "Greetings, Lawman.",
      "",
      "",
      "The Book has been claimed already; you shall not find it here.",
      "The Book has been claimed already; you shall not find it here.",
      "The Book has been claimed already; you shall not find it here.",
      "Order's retribution is inevitable, $N. Your time draws near.",
      "The Law is now in your hands, $N; guard it well.",
      "The Law is now in your hands, $N; guard it well.",
      "enf_balance", "enf_morale",
      TRUE, 0, 60, "Kaleb Carenthir", TRUE, TRUE, TRUE, TRUE,
      (ROSTER*)NULL,
      ENFORCER_ONLY,
      0
   },
   {
      "outlaw",       "[OUTLAW] ",     "OUTL",    "Ring",        FALSE,
      KEY_OUTLAW, ROOM_VNUM_OUTLAW, MOB_VNUM_OUTLAW, OBJ_VNUM_OUTLAW, MOB_VNUM_OUTLAW_STATUE,
      {
         {"decoy",                  25, 75, TRUE,  ALLOW_ALL, 0},
         {"smoke screen",           18, 75, TRUE,  ALLOW_ALL, 0},
         {"lurk",                   30, 75, TRUE,  ALLOW_ALL, 0},
         {"stealth",                20, 75, TRUE,  ALLOW_ALL, 0},
         {"fence",                  20, 75, TRUE,  ALLOW_ALL, 0},
         {"aura of defiance",       30, 75, TRUE,  ALLOW_ALL, 0},
         {"cloak of transgression", 25, 75, TRUE,  ALLOW_ALL, 0},
         {NULL,                      0,  0, FALSE, ALLOW_ALL, 0}
      },
      "[OUTLAW] : Chit announces your presence.\n\r",
      "[OUTLAW] : Chit announces, 'Welcome home, %s, we gots lots to do, so get to work.'\n\r",
      "[OUTLAW] : Chit announces your departure. Live for Money.\n\r",
      "[OUTLAW] : '%s has retired from duty.  The rest of yus get to work, or the Godfather ain't gonna be happy.'",
      "Get back here you lazy bums, we're under attack!",
      "Oh, you again, don't just stand there, get to work!",
      "slaps you on your back, hard.",
      "",
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      "out_balance", "out_morale",
      TRUE, 0, 60, "Malignus Abasdaron Radik", TRUE, TRUE, FALSE, FALSE,
      (ROSTER*)NULL,
      MARAUDER_ONLY,
      0
   },
   {
      "valor",        "[VALOR] ",     "VALR",    "Crucifix",     FALSE,
      KEY_VALOR, ROOM_VNUM_VALOR, MOB_VNUM_VALOR, OBJ_VNUM_VALOR, MOB_VNUM_VALOR_STATUE,
      {
         {"steed",         20, 75, TRUE,  ALLOW_ALL, 0},
         {"health",        25, 75, TRUE,  ALLOW_ALL, 0},
         {"forge",         25, 75, TRUE,  ALLOW_ALL, 0},
         {"charge",        30, 75, TRUE,  ALLOW_ALL, 0},
         {"doublestrike",  35, 75, TRUE,  ALLOW_ALL, 0},
         {NULL,             0,  0, FALSE, ALLOW_ALL, 0}
      },
      "[VALOR] : The resistance grows stronger with your arrival.\n\r",
      "[VALOR] : The resistance grows stronger with the arrival of %s.\n\r",
      "[VALOR] : You leave the resistance for a while.\n\r",
      "[VALOR] : %s leaves the resistance for a while.\n\r",
      "Knights, unite! The sanctum is being invaded!",
      "Greetings, Knight!",
      "",
      "",
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      "val_balance", "val_morale",
      TRUE, 0, 60, "Cirdan Aldaron Liefdei", TRUE, TRUE, FALSE, FALSE,
      (ROSTER*)NULL,
      0,
      VALOR_ONLY
   },
   {
      "empire",       "[EMPIRE] ",    "EMPR",    "Sword",        FALSE,
      KEY_EMPIRE, ROOM_VNUM_EMPIRE, MOB_VNUM_EMPIRE, OBJ_VNUM_EMPIRE, MOB_VNUM_EMPIRE_STATUE,
      {
         {NULL,             0,  0,  FALSE, ALLOW_ALL, 0}
      },
      "[EMPIRE] : The empire welcomes your arrival.\n\r",
      "[EMPIRE] : The empire welcomes the arrival of %s.\n\r",
      "[EMPIRE] : The cause of the empire grows weaker as you leave.\n\r",
      "[EMPIRE] : The cause of the empire grows weaker as %s leaves.\n\r",
      "An invader has breached the gates!!  TO ARMS, TO ARMS!",
      "Welcome, Servant of the Empire.",
      "",
      "I WILL SEND YOU TO THE ABYSS!!",
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      "emp_balance", "emp_morale",
      TRUE, 0, 60, "Wervdon Kaleb", TRUE, TRUE, TRUE, TRUE,
      (ROSTER*)NULL,
      0,
      EMPIRE_ONLY
   },
   {
      "brethren",     "[BRETHREN] ",   "BRET",    "Shard",        FALSE,
      KEY_BRETHREN, ROOM_VNUM_BRETHREN, MOB_VNUM_BRETHREN, OBJ_VNUM_BRETHREN, MOB_VNUM_BRETHREN_STATUE,
      {
         {"conjure",          25, 75, TRUE,  ALLOW_ALL, 0},
         {"demonic enchant",  35, 75, TRUE,  ALLOW_ALL, 0},
         {"pact",             30, 75, TRUE,  ALLOW_ALL, 0},
         {"demon swarm",      35, 75, TRUE,  ALLOW_ALL, 0},
         {"portal",           30, 75, TRUE,  ALLOW_ALL, 0},
         {NULL,                0,  0, FALSE, ALLOW_ALL, 0}
      },
      "[BRETHREN] : The world darkens with your arrival.\n\r",
      "[BRETHREN] : The world darkens with the arrival of %s.\n\r",
      "[BRETHREN] : The Underworld has accepted your return.\n\r",
      "[BRETHREN] : The Underworld has accepted the return of %s.\n\r",
      "To the Gates, One seeks Death.",
      "",
      "bows in grudging respect.",
      "",
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      "bre_balance", "bre_morale",
      TRUE, 0, 60, "Rungekutta Vrrin", TRUE, TRUE, FALSE, FALSE,
      (ROSTER*)NULL,
      0,
      BRETHREN_ONLY
   },
   {
      "scholar",      "[SCHOLAR] ",     "SCHL",    "",        FALSE,
      0, 0, 0, 0, 0,
      {
         {NULL, 0, 0, FALSE, ALLOW_ALL, 0}
      },
      "[SCHOLAR] : Welcome, %s! Good to have you back!\n\r",
      "[SCHOLAR] : Welcome, %s! Good to have you back!\n\r",
      "[SCHOLAR] : Farewell, %s. Another Scholar departs the land.\n\r",
      "[SCHOLAR] : Farewell, %s. Another Scholar departs the land.\n\r",
      "",
      "",
      "",
      "",
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      "sch_balance", "sch_morale",
      TRUE, 0, 60, "Xyza", TRUE, TRUE, FALSE, FALSE,
      (ROSTER*)NULL,
      0,
      SCHOLAR_ONLY
   },
   {
      "life",         "[LIFE] ",       "LIFE",    "Seed",        FALSE,
      0, 0, 0, 0, 0,
      {
         {NULL, 0, 0, FALSE, ALLOW_GOOD, 0}
      },
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      "",
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      "", "",
      TRUE, 0, 60, "", TRUE, TRUE, FALSE, FALSE,
      (ROSTER*)NULL,
      0,
      LIFE_ONLY
   },
   {
      "covenant",         "[COVENANT] ",       "COVT",    "Broadsword",        FALSE,
      KEY_COVENANT, ROOM_VNUM_COVENANT, MOB_VNUM_COVENANT, OBJ_VNUM_COVENANT, MOB_VNUM_COVENANT_STATUE,
      {
         {"dark armor",       -1, 75, TRUE,  ALLOW_ALL, 0},
         {"jump",             -1, 75, TRUE,  ALLOW_ALL, 0},
         {"impale",           -1, 75, TRUE,  ALLOW_ALL, 0},
         {"devote",           -1, 75, TRUE,  ALLOW_ALL, 0},
         {"shackles",         -1, 75, TRUE,  ALLOW_ALL, 0},
         {"aura of presence", -1, 75, TRUE,  ALLOW_ALL, 0},
         {"recruit",          -1, 75, TRUE,  ALLOW_ALL, 0},
         {"swing",            -1, 75, TRUE,  ALLOW_ALL, 0},
         {NULL,                0,  0, FALSE, ALLOW_ALL, 0}
      },
      "[COVENANT] : The Knighthood salutes you as you return, %s.\n\r",
      "[COVENANT] : All salute %s, they have returned.\n\r",
      "[COVENANT] : The Knighthood salutes you as you depart, %s.\n\r",
      "[COVENANT] : The Knighthood salutes %s as they depart.\n\r",
      "We are under attack!  Return at once and defend!  MOVE!",
      "Hail, Covenant Knight!",
      "salutes proudly.",
      "You will pay for your intrusion!",
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      "leg_balance", "leg_morale",
      TRUE, 0, 60, "Ashanti Drinlinda", TRUE, TRUE, FALSE, FALSE,
      (ROSTER*)NULL,
      0,
      COVENANT_ONLY
   },
   {
      "conclave",     "[CONCLAVE] ",   "CONC",    "",        FALSE,
      KEY_CONCLAVE, ROOM_VNUM_CONCLAVE, MOB_VNUM_CONCLAVE, OBJ_VNUM_CONCLAVE, MOB_VNUM_CONCLAVE_STATUE,
      {
         {"Scribe",              -1, 75, TRUE,  ALLOW_GOOD,    0},
         {"Flame Scorch",        -1, 75, TRUE,  ALLOW_GOOD,    3},
         {"Siren Screech",       -1, 75, TRUE,  ALLOW_GOOD,    3},
         {"Prismatic Sphere",    -1, 75, TRUE,  ALLOW_GOOD,    5},
         {"Phantasmal Force",    -1, 75, TRUE,  ALLOW_NEUTRAL, 0},
         {"Intangibility",       -1, 75, TRUE,  ALLOW_NEUTRAL, 3},
         {"Venueport",           -1, 75, TRUE,  ALLOW_NEUTRAL, 3},
         {"Simulacrum",          -1, 75, TRUE,  ALLOW_NEUTRAL, 5},
         {"Seize",               -1, 75, TRUE,  ALLOW_EVIL,    0},
         {"Hang",                -1, 75, TRUE,  ALLOW_EVIL,    3},
         {"Virulent Cysts",      -1, 75, TRUE,  ALLOW_EVIL,    3},
         {"Rite of Darkness",    -1, 75, TRUE,  ALLOW_EVIL,    5},
         {"Familiar",            -1, 75, TRUE,  ALLOW_ALL,     0},
         {"SpiritBlade",         -1, 75, TRUE,  ALLOW_ALL,     0},
         {"Team Spirit",         -1, 75, TRUE,  ALLOW_ALL,     0},
         {"Channel",             -1, 75, TRUE,  ALLOW_ALL,     0},
         {"Soulscry",            -1, 75, TRUE,  ALLOW_ALL,     5},
         {NULL,                   0,  0, FALSE, ALLOW_ALL,     0}
      },
      "[CONCLAVE] : Welcome, Defender of Magic.\n\r",
      "[CONCLAVE] : Welcome back %s, the Conclave awaits you.\n\r",
      "[CONCLAVE] : Farewell %s, the Conclave will wait for your return.\n\r",
      "[CONCLAVE] : %s has left the tower to pursue studies beyond this plane.\n\r",
      "An Intruder seeks entrance to the tower!  Hurry!  Quickly to me!",
      "",
      "",
      "I will teach you the proper respect for magic!",
      "Another already holds the Cube of Sorcery.  If ye seek war, then find it.",
      "Another already holds the Cube of Sorcery.",
      "Another already holds the Cube of Sorcery.",
      "You have declared war on the house.  Go now.",
      "It is good that we have you on our side, $N.",
      "It is good that we have you on our side, $N.",
      "cnc_balance", "cnc_morale",
      TRUE, 0, 60, "Riallus Xurinos", TRUE, TRUE, FALSE, FALSE,
      (ROSTER*)NULL,
      0,
      CONCLAVE_ONLY
   },
   {
      "marauder",       "[MARAUDER] ",     "MARA",    "Glove",        FALSE,
      KEY_MARAUDER, ROOM_VNUM_MARAUDER, MOB_VNUM_MARAUDER, OBJ_VNUM_MARAUDER, MOB_VNUM_MARAUDER_STATUE,
      {
         {"misinformation",         -1, 75, TRUE,  ALLOW_ALL,       1},
         {"eyes of the bandit",     -1, 75, TRUE,  ALLOW_ALL,       1},
         {"bribe",                  -1, 75, TRUE,  ALLOW_ALL,       2},
         {"guild cloth",            -1, 75, TRUE,  ALLOW_ALL,       1},
         {"song of shadows",        -1, 75, TRUE,  ALLOW_ALL,       1},
         {"dance of the rogue",     -1, 75, TRUE,  ALLOW_ROGUE,     1},
         {"gate call",              -1, 75, TRUE,  ALLOW_ROGUE,     1},
         {"thugs stance",           -1, 75, TRUE,  ALLOW_THUG,      1},
         {"cunning strike",         -1, 75, TRUE,  ALLOW_THUG,      1},
         {"silver tongue",          -1, 75, TRUE,  ALLOW_TRICKSTER, 1},
         {"soothing voice",         -1, 75, TRUE,  ALLOW_TRICKSTER, 1},
         {NULL,                      0,  0, FALSE, ALLOW_ALL,       0}
      },
      "[MARAUDER] : The Guild has awaited your return. Welcome back.\n\r",
      "[MARAUDER] : %s returns and the Guildhall grows more crowded.\n\r",
      "[MARAUDER] : Crime mourns your depature. The Guild awaits your return.\n\r",
      "[MARAUDER] : The Guildhall grows quieter as %s takes his leave.\n\r",
      "An intruder attempts to enter our Guild!",
      "Welcome kin of Crime.",
      "",
      "",
      "You shall not have the Hand this day, intruder.",
      "The Hand is not here, look elsewhere.",
      "The Hand is not in the Guild, look elsewhere.",
      "Take the Hand of Crime, your blood the Guild shall have.",
      "$N holds the Hand of Crime, seek refuge with the Guild.",
      "You hold the Hand of Crime, $N, the Guild depends on you now.",
      "mar_balance", "mar_morale",
      TRUE, 0, 40, "Huerkin Malignus", TRUE, TRUE, FALSE, FALSE,
      (ROSTER*)NULL,
      0,
      MARAUDER_ONLY
   },
};

/* for position */
const struct position_type position_table[] =
{
   {    "dead",            "dead"    },
   {    "mortally wounded",    "mort"    },
   {    "incapacitated",    "incap"    },
   {    "stunned",        "stun"    },
   {    "sleeping",        "sleep"    },
   {    "resting",        "rest"    },
   {   "sitting",        "sit"   },
   {    "fighting",        "fight"    },
   {    "standing",        "stand"    },
   {    NULL,            NULL    }
};

/* for sex */
const struct sex_type sex_table[] =
{
   {    "none"        },
   {    "male"        },
   {    "female"    },
   {    "either"    },
   {    NULL        }
};

/* for sizes */
const struct   size_type   size_table  [] =
{
   {
      "tiny",
      WEAR_SIZE_TINY,
   },
   {
      "small",
      WEAR_SIZE_SMALL,
   },
   {
      "medium",
      WEAR_SIZE_MEDIUM,
   },
   {
      "large",
      WEAR_SIZE_LARGE,
   },
   {
      "huge",
      WEAR_SIZE_HUGE,
   },
   {
      "giant",
      WEAR_SIZE_GIANT,
   },
   {
      NULL,
      0
   }
};

/* various flag tables */
const FLAG_TYPE act_flags[] =
{
   {    "npc",             ACT_IS_NPC,       FALSE    },
   {    "sentinel",        ACT_SENTINEL,     TRUE     },
   {    "scavenger",       ACT_SCAVENGER,    TRUE     },
   {    "house_mob",       ACT_HOUSE_MOB,    TRUE     },
   {    "aggressive",      ACT_AGGRESSIVE,   TRUE     },
   {    "stay_area",       ACT_STAY_AREA,    TRUE     },
   {    "wimpy",           ACT_WIMPY,        TRUE     },
   {    "pet",             ACT_PET,          TRUE     },
   {    "train",           ACT_TRAIN,        TRUE     },
   {    "practice",        ACT_PRACTICE,     TRUE     },
   {    "holylight",       ACT_HOLYLIGHT,    TRUE     },
   {    "undead",          ACT_UNDEAD,       TRUE     },
   {    "cleric",          ACT_CLERIC,       TRUE     },
   {    "mage",            ACT_MAGE,         TRUE     },
   {    "thief",           ACT_THIEF,        TRUE     },
   {    "warrior",         ACT_WARRIOR,      TRUE     },
   {    "noalign",         ACT_NOALIGN,      TRUE     },
   {    "nopurge",         ACT_NOPURGE,      TRUE     },
   {    "outdoors",        ACT_OUTDOORS,     TRUE     },
   {    "indoors",         ACT_INDOORS,      TRUE     },
   {    "questor",         ACT_QUESTOR,      TRUE     },
   {    "healer",          ACT_IS_HEALER,    TRUE     },
   {    "gain",            ACT_GAIN,         TRUE     },
   {    "update_always",   ACT_UPDATE_ALWAYS,   TRUE  },
   {    "changer",         ACT_IS_CHANGER,   TRUE     },
   {    "banker",          ACT_BANKER,       TRUE     },
   {    NULL,              0,                FALSE    }
};

const FLAG_TYPE act2_flags[] =
{
   {    "rideable",        ACT_RIDEABLE,        TRUE     },
   {    "no_steal",        ACT_NO_STEAL,        TRUE     },
   {    "no_deathstrike",  ACT_NO_DEATHSTRIKE,  TRUE     },
   {    "ranger",          ACT_RANGER,          TRUE     },
   {    "intelligent",     ACT_INTELLIGENT,     TRUE     },
   {    "stray",           ACT_STRAY,           TRUE     },
   {    "nice_stray",      ACT_NICE_STRAY,      TRUE     },
   {    "pure_guildmaster", ACT_PURE_GUILDMASTER, TRUE     },
   {    "autoattack",      ACT2_AUTOATTACK,     TRUE     },
   {    "no_tame",         ACT_NO_TAME,         TRUE     },
   {    "no_seize",        ACT_NO_SEIZE,        TRUE     },
   {    NULL,              0,                   FALSE    }
};

const FLAG_TYPE plr_flags[] =
{
   {  "npc",               PLR_IS_NPC,       FALSE },
   {  "autoassist",        PLR_AUTOASSIST,   TRUE  },
   {  "autoexit",          PLR_AUTOEXIT,     TRUE  },
   {  "autoloot",          PLR_AUTOLOOT,     TRUE  },
   {  "autosac",           PLR_AUTOSAC,      TRUE  },
   {  "autogold",          PLR_AUTOGOLD,     TRUE  },
   {  "autosplit",         PLR_AUTOSPLIT,    TRUE  },
   {  "color",             PLR_COLOR,        TRUE  },
   {  "evaluation",        PLR_EVALUATION,   TRUE  },
   {  "no_summon",         PLR_NOSUMMON,     TRUE  },
   {  "no_follow",         PLR_NOFOLLOW,     TRUE  },
   {  "honorbound",        PLR_HONORBOUND,   TRUE  },
   {  "no_transfer",       PLR_NO_TRANSFER,  TRUE  },
   {  "permit",            PLR_PERMIT,       TRUE  },
   {  "log",               PLR_LOG,          TRUE  },
   {  "deny",              PLR_DENY,         TRUE  },
   {  "frozen",            PLR_FREEZE,       TRUE  },
   {  "thief",             PLR_THIEF,        TRUE  },
   {  "killer",            PLR_KILLER,       TRUE  },
   {  "criminal",          PLR_CRIMINAL,     TRUE  },
   {  "marked",            PLR_MARKED,       TRUE  },
   {  "evil",              PLR_EVIL,         TRUE  },
   {  "covenant_ally",     PLR_COVENANT_ALLY, TRUE  },
   {  NULL,                0,                0     }
};

const FLAG_TYPE plr2_flags[] =
{
   {  "ancient_mark",      PLR_ANCIENT_MARK,       TRUE  },
   {  "ancient_mark_perm", PLR_ANCIENT_MARK_PERM,  TRUE  },
   {  "guildmaster",       PLR_GUILDMASTER,        TRUE  },
   {  "guildless",         PLR_GUILDLESS,          TRUE  },
   {  "seebrand",          WIZ_SEEBRAND,           TRUE  },
   {  "seehouse",          WIZ_SEEHOUSE,           TRUE  },
   {  "novice",            PLR_NOVICE,             TRUE  },
   {  "log_spec",          PLR_LOG_SPEC,           TRUE  },
   {  "lich",              PLR_LICH,               TRUE  },
   {  "is_ancient",        PLR_IS_ANCIENT,         TRUE  },
   {  "is_ancient_known",  PLR_IS_ANCIENT_KNOWN,   TRUE  },
   {  "see_newbie",        WIZ_SEENEWBIE,          TRUE  },
   {  "see_worship",       WIZ_SEEWORSHIP,         TRUE  },
   {  "nofight",           PLR_NO_FIGHT,           TRUE  },
   {  "impure",            PLR_IMPURE,             TRUE  },
   {  "seeclan",           WIZ_SEECLAN,            TRUE  },
   {  "actively_branding", PLR_ACTIVELY_BRANDING,  TRUE  },
   {  "lawless",           PLR_LAWLESS,            TRUE  },
   {  "no_house_emote",    PLR_NO_HOUSE_EMOTE,     TRUE  },
   {  "magic_taken",       PLR_MAGIC_TAKEN,        TRUE  },
   {  NULL,                0,                      0     }
};

const FLAG_TYPE off_flags[] =
{
   {    "area_attack",        A,    TRUE    },
   {    "backstab",        B,    TRUE    },
   {    "bash",            C,    TRUE    },
   {    "berserk",        D,    TRUE    },
   {    "disarm",        E,    TRUE    },
   {    "dodge",        F,    TRUE    },
   {    "fade",            G,    TRUE    },
   {    "fast",            H,    TRUE    },
   {    "kick",            I,    TRUE    },
   {    "dirt_kick",        J,    TRUE    },
   {    "parry",        K,    TRUE    },
   {    "rescue",        L,    TRUE    },
   {    "tail",            M,    TRUE    },
   {    "trip",            N,    TRUE    },
   {    "crush",        O,    TRUE    },
   {    "assist_all",        P,    TRUE    },
   {    "assist_align",        Q,    TRUE    },
   {    "assist_race",        R,    TRUE    },
   {    "assist_players",    S,    TRUE    },
   {    "assist_guard",        T,    TRUE    },
   {    "assist_vnum",        U,    TRUE    },
   {    NULL,            0,    0    }
};

const FLAG_TYPE affect_flags[] =
{
   {"blind",         AFF_BLIND,           TRUE },
   {"invisible",     AFF_INVISIBLE,       TRUE },
   {"detect_evil",   AFF_DETECT_EVIL,     TRUE },
   {"detect_invis",  AFF_DETECT_INVIS,    TRUE },
   {"detect_magic",  AFF_DETECT_MAGIC,    TRUE },
   {"detect_hidden", AFF_DETECT_HIDDEN,   TRUE },
   {"detect_good",   AFF_DETECT_GOOD,     TRUE },
   {"sanctuary",     AFF_SANCTUARY,       TRUE },
   {"faerie_fire",   AFF_FAERIE_FIRE,     TRUE },
   {"infrared",      AFF_INFRARED,        TRUE },
   {"curse",         AFF_CURSE,           TRUE },
   {"acute_vision",  AFF_ACUTE_VISION,    TRUE },
   {"poison",        AFF_POISON,          TRUE },
   {"protect_evil",  AFF_PROTECT_EVIL,    TRUE },
   {"protect_good",  AFF_PROTECT_GOOD,    TRUE },
   {"sneak",         AFF_SNEAK,           TRUE },
   {"hide",          AFF_HIDE,            TRUE },
   {"sleep",         AFF_SLEEP,           TRUE },
   {"charm",         AFF_CHARM,           TRUE },
   {"flying",        AFF_FLYING,          TRUE },
   {"pass_door",     AFF_PASS_DOOR,       TRUE },
   {"haste",         AFF_HASTE,           TRUE },
   {"calm",          AFF_CALM,            TRUE },
   {"plague",        AFF_PLAGUE,          TRUE },
   {"weaken",        AFF_WEAKEN,          TRUE },
   {"dark_vision",   AFF_DARK_VISION,     TRUE },
   {"berserk",       AFF_BERSERK,         TRUE },
   {"swim",          AFF_SWIM,            TRUE },
   {"regeneration",  AFF_REGENERATION,    TRUE },
   {"slow",          AFF_SLOW,            TRUE },
   {"camouflage",    AFF_CAMOUFLAGE,      TRUE },
   {NULL,            0,                   FALSE}
};


const FLAG_TYPE affect2_flags[] =
{
   {"wizi",             AFF_WIZI,               TRUE },
   {"ahzra_bloodstain", AFF_AHZRA_BLOODSTAIN,   TRUE },
   {"herb_spell",       AFF_HERB_SPELL,         TRUE },
   {"no_sleep",         AFF_NO_SLEEP,           TRUE },
   {"no_rest",          AFF_NO_REST,            TRUE },
   {"supplicate",       AFF_SUPPLICATE,         TRUE },
   {"feign_death",      AFF_FEIGN_DEATH,        TRUE },
   {"intangibility",    AFF_INTANGIBILITY,      TRUE },
   {"riptide",          AFF_RIPTIDE,            TRUE },
   {"blindfold",        AFF_BLINDFOLD,          TRUE },
   {"gag",              AFF_GAG,                TRUE },
   {"bind",             AFF_BIND,               TRUE },
   {"hang",             AFF_HANG,               TRUE },
   {"awareness",        AFF_AWARENESS,          TRUE },
   {"hyper",            AFF_HYPER,              TRUE },
   {"nap",              AFF_NAP,                TRUE },
   {NULL,               0,                      FALSE}
};

const FLAG_TYPE imm_flags[] =
{
   {    "summon",       A,    TRUE    },
   {    "charm",        B,    TRUE    },
   {    "magic",        C,    TRUE    },
   {    "weapon",       D,    TRUE    },
   {    "bash",         E,    TRUE    },
   {    "pierce",       F,    TRUE    },
   {    "slash",        G,    TRUE    },
   {    "fire",         H,    TRUE    },
   {    "cold",         I,    TRUE    },
   {    "lightning",    J,    TRUE    },
   {    "acid",         K,    TRUE    },
   {    "poison",       L,    TRUE    },
   {    "negative",     M,    TRUE    },
   {    "holy",         N,    TRUE    },
   {    "energy",       O,    TRUE    },
   {    "mental",       P,    TRUE    },
   {    "disease",      Q,    TRUE    },
   {    "drowning",     R,    TRUE    },
   {    "light",        S,    TRUE    },
   {    "sound",        T,    TRUE    },
   /*
   Actually unused, not called that.
   {    "unused",       U,    TRUE    },
   */
   {    "harm",         V,    TRUE    },
   {    "wood",         X,    TRUE    },
   {    "silver",       Y,    TRUE    },
   {    "iron",         Z,    TRUE    },
   {    "metal",        aa,   TRUE    },
   {    "spells",       bb,   TRUE    },
   {    "nature",       cc,   TRUE    },
   {    NULL,           0,    FALSE   }
};

const FLAG_TYPE form_flags[] =
{
   {    "edible",        FORM_EDIBLE,        TRUE    },
   {    "poison",        FORM_POISON,        TRUE    },
   {    "magical",        FORM_MAGICAL,        TRUE    },
   {    "instant_decay",    FORM_INSTANT_DECAY,    TRUE    },
   {    "other",        FORM_OTHER,        TRUE    },
   {    "animal",        FORM_ANIMAL,        TRUE    },
   {    "sentient",        FORM_SENTIENT,        TRUE    },
   {    "undead",        FORM_UNDEAD,        TRUE    },
   {    "construct",        FORM_CONSTRUCT,        TRUE    },
   {    "mist",            FORM_MIST,        TRUE    },
   {    "intangible",        FORM_INTANGIBLE,    TRUE    },
   {    "biped",        FORM_BIPED,        TRUE    },
   {    "centaur",        FORM_CENTAUR,        TRUE    },
   {    "insect",        FORM_INSECT,        TRUE    },
   {    "spider",        FORM_SPIDER,        TRUE    },
   {    "crustacean",        FORM_CRUSTACEAN,    TRUE    },
   {    "worm",            FORM_WORM,        TRUE    },
   {    "blob",            FORM_BLOB,        TRUE    },
   {    "mammal",        FORM_MAMMAL,        TRUE    },
   {    "bird",            FORM_BIRD,        TRUE    },
   {    "reptile",        FORM_REPTILE,        TRUE    },
   {    "snake",        FORM_SNAKE,        TRUE    },
   {    "dragon",        FORM_DRAGON,        TRUE    },
   {    "amphibian",        FORM_AMPHIBIAN,        TRUE    },
   {    "fish",            FORM_FISH ,        TRUE    },
   {    "cold_blood",        FORM_COLD_BLOOD,    TRUE    },
   {    "plant",        FORM_PLANT,        TRUE    },
   {    NULL,            0,            0    }
};

const FLAG_TYPE part_flags[] =
{
   {    "head",            PART_HEAD,        TRUE    },
   {    "arms",            PART_ARMS,        TRUE    },
   {    "legs",            PART_LEGS,        TRUE    },
   {    "heart",        PART_HEART,        TRUE    },
   {    "brains",        PART_BRAINS,        TRUE    },
   {    "guts",            PART_GUTS,        TRUE    },
   {    "hands",        PART_HANDS,        TRUE    },
   {    "feet",            PART_FEET,        TRUE    },
   {    "fingers",        PART_FINGERS,        TRUE    },
   {    "ear",            PART_EAR,        TRUE    },
   {    "eye",            PART_EYE,        TRUE    },
   {    "long_tongue",        PART_LONG_TONGUE,    TRUE    },
   {    "eyestalks",        PART_EYESTALKS,        TRUE    },
   {    "tentacles",        PART_TENTACLES,        TRUE    },
   {    "fins",            PART_FINS,        TRUE    },
   {    "wings",        PART_WINGS,        TRUE    },
   {    "tail",            PART_TAIL,        TRUE    },
   {    "claws",        PART_CLAWS,        TRUE    },
   {    "fangs",        PART_FANGS,        TRUE    },
   {    "horns",        PART_HORNS,        TRUE    },
   {    "scales",        PART_SCALES,        TRUE    },
   {    "tusks",        PART_TUSKS,        TRUE    },
   {    NULL,            0,            0    }
};

const FLAG_TYPE comm_flags[] =
{
   {   "quiet",        COMM_QUIET,        TRUE    },
   {   "deaf",            COMM_DEAF,        TRUE    },
   {   "nowiz",        COMM_NOWIZ,        TRUE    },
   {   "noauction",            COMM_NOAUCTION,         TRUE    },
   {   "nogossip",        COMM_NOGOSSIP,        TRUE    },
   {   "noquestion",        COMM_NOQUESTION,    TRUE    },
   {   "nomusic",        COMM_NOMUSIC,        TRUE    },
   {   "nohouse",              COMM_NOHOUSE,           TRUE    },
   {   "noquote",        COMM_NOQUOTE,        TRUE    },
   {   "shoutsoff",        COMM_SHOUTSOFF,        TRUE    },
   {   "ansi",        COMM_ANSI,        TRUE    },
   {   "oocoff",        COMM_OOCOFF,        TRUE    },
   {   "compact",        COMM_COMPACT,        TRUE    },
   {   "brief",        COMM_BRIEF,        TRUE    },
   {   "prompt",        COMM_PROMPT,        TRUE    },
   {   "combine",        COMM_COMBINE,        TRUE    },
   {   "telnet_ga",        COMM_TELNET_GA,        TRUE    },
   {   "show_affects",        COMM_SHOW_AFFECTS,    TRUE    },
   {   "moron",        COMM_MORON,        TRUE    },
   {   "immortal",        COMM_IMMORTAL,        TRUE    },
   {   "limits",        COMM_LIMITS,        TRUE    },
   {   "noemote",        COMM_NOEMOTE,        FALSE    },
   {   "noshout",        COMM_NOSHOUT,        FALSE    },
   {   "notell",        COMM_NOTELL,        FALSE    },
   {   "nochannels",        COMM_NOCHANNELS,    FALSE    },
   {   "builder",        COMM_BUILDER,    TRUE    },
   {   "snoop_proof",        COMM_SNOOP_PROOF,    FALSE    },
   {   "afk",            COMM_AFK,        TRUE    },
   {   "house_global",            COMM_ALL_HOUSES,        TRUE    },
   {   "nopray",               COMM_NOPRAY,            FALSE   },
   {   "silence",              COMM_SILENCE,           FALSE   },
   {    NULL,            0,            0    }
};

const FLAG_TYPE comm2_flags[] =
{
   {   "brand_global",     COMM_BTGLOBAL,       TRUE  },
   {   "talk_mental",      COMM_WIZ_TALK,       TRUE  },
   {   "stat_time",        COMM_STAT_TIME,      TRUE  },
   {   "worship_stopped",  COMM_WORSHIP_STOPPED, TRUE  },
   {   "cast_silent",      COMM_CAST_SILENT,    TRUE  },
   {   "wiz_revoked",      COMM_WIZ_REVOKE,     TRUE  },
   {   "clan_global",      COMM_CTGLOBAL,       TRUE  },
   {   "no_clantell",      COMM_NOCLAN,         TRUE  },
   {   "music",            COMM_MUSIC,          TRUE  },
   {   "music_download",   COMM_MUSIC_DOWNLOAD, TRUE  },
   {   "see_surname",      COMM_SEE_SURNAME,    TRUE  },
   {   "surname_accepted", COMM_SURNAME_ACCEPTED,  TRUE  },
   {    NULL,              0,                0     }
};

const FLAG_TYPE mprog_flags[] =
{
   {     "act",                  TRIG_ACT,               TRUE    },
   {     "bribe",                TRIG_BRIBE,             TRUE    },
   {     "death",                TRIG_DEATH,             TRUE    },
   {     "entry",                TRIG_ENTRY,             TRUE    },
   {     "fight",                TRIG_FIGHT,             TRUE    },
   {     "give",                 TRIG_GIVE,              TRUE    },
   {     "greet",                TRIG_GREET,             TRUE    },
   {     "grall",                TRIG_GRALL,             TRUE    },
   {     "kill",                 TRIG_KILL,              TRUE    },
   {     "hpcnt",                TRIG_HPCNT,             TRUE    },
   {     "random",               TRIG_RANDOM,            TRUE    },
   {     "speech",               TRIG_SPEECH,            TRUE    },
   {     "exit",                 TRIG_EXIT,              TRUE    },
   {     "exall",                TRIG_EXALL,             TRUE    },
   {     "delay",                TRIG_DELAY,             TRUE    },
   {     "surr",                 TRIG_SURR,              TRUE    },
   {    NULL,            0,            0    }
};

const FLAG_TYPE extra_flags[] =
{
   {     "glowing",              ITEM_GLOW,              TRUE  },
   {     "humming",              ITEM_HUM,               TRUE  },
   {     "dark",                 ITEM_DARK,              TRUE  },
   {     "lock",                 ITEM_LOCK,              TRUE  },
   {     "evil",                 ITEM_EVIL,              TRUE  },
   {     "invis",                ITEM_INVIS,             TRUE  },
   {     "magic",                ITEM_MAGIC,             TRUE  },
   {     "nodrop",               ITEM_NODROP,            TRUE  },
   {     "bless",                ITEM_BLESS,             TRUE  },
   {     "anti_good",            ITEM_ANTI_GOOD,         TRUE  },
   {     "anti_evil",            ITEM_ANTI_EVIL,         TRUE  },
   {     "anti_neutral",         ITEM_ANTI_NEUTRAL,      TRUE  },
   {     "noremove",             ITEM_NOREMOVE,          TRUE  },
   {     "inventory",            ITEM_INVENTORY,         TRUE  },
   {     "nopurge",              ITEM_NOPURGE,           TRUE  },
   {     "rotdeath",             ITEM_ROT_DEATH,         TRUE  },
   {     "visdeath",             ITEM_VIS_DEATH,         TRUE  },
   {     "nonmetal",             ITEM_NONMETAL,          TRUE  },
   {     "nolocate",             ITEM_NOLOCATE,          TRUE  },
   {     "meltdrop",             ITEM_MELT_DROP,         TRUE  },
   {     "nobrains",             ITEM_NO_BRAINS,         TRUE  },
   {     "sellextract",          ITEM_SELL_EXTRACT,      TRUE  },
   {     "burnproof",            ITEM_BURN_PROOF,        TRUE  },
   {     "nouncurse",            ITEM_NOUNCURSE,         TRUE  },
   {     "brand",                ITEM_BRAND,             TRUE  },
   {     "noanimate",            CORPSE_NO_ANIMATE,      TRUE  },
   {     "anti_methodical",      ITEM_ANTI_METHODICAL,   TRUE  },
   {     "anti_neutral",         ITEM_ANTI_NEUT,         TRUE  },
   {     "anti_impulsive",       ITEM_ANTI_IMPULSIVE,    TRUE  },
   {     NULL,                   0,                      0     }
};

const FLAG_TYPE extra2_flags[] =
{
   {     "no_buy",               ITEM_NOBUY,             TRUE    },
   {     "rideable",             ITEM_RIDEABLE,          TRUE    },
   {     "halfling_allowed",     HALFLING_ONLY,          TRUE    },
   {     "arbor_allowed",        ARBOR_ONLY,             TRUE    },
   {     "no_origin",            ITEM_NOORIGIN,          TRUE    },
   {     "purified",             ITEM_PURIFIED,          TRUE    },
   {     "bloodstained",         ITEM_AHZRA_BLOODSTAIN,  TRUE    },
   {     "no_blood",             ITEM_NO_BLOOD,          TRUE    },
   {     "no_shield_cleave",     ITEM_NO_SHIELD_CLEAVE,  TRUE    },
   {     "female_only",          FEMALE_ONLY,            TRUE    },
   {     "male_only",            MALE_ONLY,              TRUE    },
   {     "genderless_only",      GENDERLESS_ONLY,        TRUE    },
   {     "book_allowed",         BOOK_ONLY,              TRUE    },
   {     "demon_allowed",        DEMON_ONLY,             TRUE    },
   {     "brethren_allowed",     BRETHREN_ONLY,          TRUE    },
   {     "covenant_allowed",     COVENANT_ONLY,          TRUE    },
   {     "conclave_allowed",     CONCLAVE_ONLY,          TRUE    },
   {     "ethereal_allowed",     ETHEREAL_ONLY,          TRUE    },
   {     "wear_size_tiny",       WEAR_SIZE_TINY,         TRUE    },
   {     "wear_size_small",      WEAR_SIZE_SMALL,        TRUE    },
   {     "wear_size_medium",     WEAR_SIZE_MEDIUM,       TRUE    },
   {     "wear_size_large",      WEAR_SIZE_LARGE,        TRUE    },
   {     "wear_size_huge",       WEAR_SIZE_HUGE,         TRUE    },
   {     "wear_size_giant",      WEAR_SIZE_GIANT,        TRUE    },
   {     NULL,                   0,                      FALSE   }
};

const FLAG_TYPE mount_flags[] =
{
   {     "Earth",                MOUNT_EARTH,            TRUE    },
   {     "Air",                  MOUNT_AIR,              TRUE    },
   {     "Fire",                 MOUNT_FIRE,             TRUE    },
   {     "On_Water",             MOUNT_ON_WATER,         TRUE    },
   {     "Under_Water",          MOUNT_UNDER_WATER,      TRUE    },
   {     "Forest",               MOUNT_FOREST,           TRUE    },
   {     "Mountain",             MOUNT_MOUNTAIN,         TRUE    },
   {    NULL,            0,            0    }
};

const FLAG_TYPE restrict_flags[] =
{
   {     "human_allowed",           HUMAN_ONLY,             TRUE   },
   {     "dwarf_allowed",           DWARF_ONLY,             TRUE   },
   {     "elf_allowed",             ELF_ONLY,               TRUE   },
   {     "grey_allowed",            GREY_ONLY,              TRUE   },
   {     "drow_allowed",            DARK_ONLY,              TRUE   },
   {     "giant_allowed",           GIANT_ONLY,             TRUE   },
   {     "centaur_allowed",         CENTAUR_ONLY,           TRUE   },
   {     "troll_allowed",           TROLL_ONLY,             TRUE   },
   {     "draconian_allowed",       DRACONIAN_ONLY,         TRUE   },
   {     "gnome_allowed",           GNOME_ONLY,             TRUE   },
   {     "minotaur_allowed",        MINOTAUR_ONLY,          TRUE   },
   {     "changeling_allowed",      CHANGELING_ONLY,        TRUE   },
   {     "illithid_allowed",        ILLITHID_ONLY,          TRUE   },
   {     "warrior_allowed",         WARRIOR_ONLY,           TRUE   },
   {     "thief_allowed",           THIEF_ONLY,             TRUE   },
   {     "cleric_allowed",          CLERIC_ONLY,            TRUE   },
   {     "paladin_allowed",         PALADIN_ONLY,           TRUE   },
   {     "AP_allowed",              ANTI_PALADIN_ONLY,      TRUE   },
   {     "ranger_allowed",          RANGER_ONLY,            TRUE   },
   {     "monk_allowed",            MONK_ONLY,              TRUE   },
   {     "channeler_allowed",       CHANNELER_ONLY,         TRUE   },
   {     "nightwalker_allowed",     NIGHTWALKER_ONLY,       TRUE   },
   {     "necromancer_allowed",     NECROMANCER_ONLY,       TRUE   },
   {     "elementalist_allowed",    ELEMENTALIST_ONLY,      TRUE   },
   {     "ancient_allowed",         ANCIENT_ONLY,           TRUE   },
   {     "magic_users_allowed",     MAGE_ONLY,              TRUE   },
   {     "arcana_allowed",          ARCANA_ONLY,            TRUE   },
   {     "crusader_allowed",        CRUSADER_ONLY,          TRUE   },
   {     "marauder_allowed",        MARAUDER_ONLY,          TRUE   },
   {     "light_allowed",           LIGHT_ONLY,             TRUE   },
   {     "justicar_allowed",        ENFORCER_ONLY,          TRUE   },
   {    NULL,            0,            0    }
};

const FLAG_TYPE weapon_flags[] =
{
   {     "flaming",        WEAPON_FLAMING,        TRUE    },
   {     "frost",                WEAPON_FROST,           TRUE    },
   {     "vampiric",             WEAPON_VAMPIRIC,        TRUE    },
   {     "sharp",                WEAPON_SHARP,           TRUE    },
   {     "vorpal",               WEAPON_VORPAL,          TRUE    },
   {     "twohanded",            WEAPON_TWO_HANDS,       TRUE    },
   {     "shocking",             WEAPON_SHOCKING,        TRUE    },
   {     "poison",               WEAPON_POISON,          TRUE    },
   {     "avenger",              WEAPON_AVENGER,         TRUE    },
   {     "shadowbane",           WEAPON_SHADOWBANE,      TRUE    },
   {     "lightbringer",         WEAPON_LIGHTBRINGER,    TRUE    },
   {     "drowning",             WEAPON_DROWNING,        TRUE    },
   {     "resonating",           WEAPON_RESONATING,      TRUE    },
   {     "draining",             WEAPON_DRAINING,        TRUE    },
   {     "fatiguing",            WEAPON_FATIGUING,       TRUE    },
   {    NULL,            0,            0    }
};

const FLAG_TYPE room_flags[] =
{
   {     "dark",                 ROOM_DARK,              TRUE    },
   {     "no_blitz",             ROOM_NO_BLITZ,          TRUE    },
   {     "no_mob",               ROOM_NO_MOB,            TRUE    },
   {     "indoors",              ROOM_INDOORS,           TRUE    },
   {     "house",                ROOM_HOUSE,             TRUE    },
   {     "shrine",               ROOM_SHRINE,            TRUE    },
   {     "private",              ROOM_PRIVATE,           TRUE    },
   {     "safe",                 ROOM_SAFE,              TRUE    },
   {     "solitary",             ROOM_SOLITARY,          TRUE    },
   {     "pet_shop",             ROOM_PET_SHOP,          TRUE    },
   {     "no_recall",            ROOM_NO_RECALL,         TRUE    },
   {     "imp_only",             ROOM_IMP_ONLY,          TRUE    },
   {     "gods_only",            ROOM_GODS_ONLY,         TRUE    },
   {     "heroes_only",          ROOM_HEROES_ONLY,       TRUE    },
   {     "newbies_only",         ROOM_NEWBIES_ONLY,      TRUE    },
   {     "law",                  ROOM_LAW,               TRUE    },
   {     "nowhere",              ROOM_NOWHERE,           TRUE    },
   {     "no_gate",              ROOM_NO_GATE,           TRUE    },
   {     "consecrated",          ROOM_CONSECRATED,       TRUE    },
   {     "no_summon",            ROOM_NO_SUMMON,         TRUE    },
   {     "no_consecrate",        ROOM_NO_CONSECRATE,     TRUE    },
   {     "no_teleport",          ROOM_NO_CONSECRATE,     TRUE    },
   {     "no_alarm",             ROOM_NO_ALARM,          TRUE    },
   {     "low_only",             ROOM_LOW_ONLY,          TRUE    },
   {     "no_magic",             ROOM_NO_MAGIC,          TRUE    },
   {     "bloody_timer",         ROOM_BLOODY_TIMER,      TRUE    },
   {     "no_track",             ROOM_NO_TRACK,          TRUE    },
   /*
   This is an extra room flag
   {     "dead_zone",            ROOM_DEAD_ZONE,         TRUE    },
   */
   {     NULL,                   0,                      0       }
};

const FLAG_TYPE extra_room_flags[] =
{
   {     "globe_darkness",       ROOM_GLOBE_DARKNESS,    TRUE    },
   {     "ap_cursed",            ROOM_AP_CURSED,         TRUE    },
   {     "1212",                 ROOM_1212,              TRUE    },
   {     "dead_zone",            ROOM_DEAD_ZONE,         TRUE    },
   {     "meteor_storm",         ROOM_METEOR_STORM,      TRUE    },
   {     "spring_rains",         ROOM_SPRING_RAINS,      TRUE    },
   {     NULL,                   0,                      0       }
};

const ASSOCIATION_TYPE wear_slot_table[] =
{
   {   "take",        ITEM_TAKE              },
   {   "finger",      ITEM_WEAR_FINGER       },
   {   "neck",        ITEM_WEAR_NECK         },
   {   "body",        ITEM_WEAR_BODY         },
   {   "head",        ITEM_WEAR_HEAD         },
   {   "legs",        ITEM_WEAR_LEGS         },
   {   "feet",        ITEM_WEAR_FEET         },
   {   "hands",       ITEM_WEAR_HANDS        },
   {   "arms",        ITEM_WEAR_ARMS         },
   {   "shield",      ITEM_WEAR_SHIELD       },
   {   "about",       ITEM_WEAR_ABOUT        },
   {   "waist",       ITEM_WEAR_WAIST        },
   {   "wrist",       ITEM_WEAR_WRIST        },
   {   "wield",       ITEM_WIELD             },
   {   "hold",        ITEM_HOLD              },
   {   "nosac",       ITEM_NO_SAC            },
   {   "float",       ITEM_WEAR_FLOAT        },
   {   "brand",       ITEM_WEAR_BRAND        },
   {   "keep",        ITEM_KEEP              },
   {   "horns",       ITEM_WEAR_HORNS        },
   {   "snout",       ITEM_WEAR_SNOUT        },
   {   "hooves",      ITEM_WEAR_HOOVES       },
   {   "fourlegs",    ITEM_WEAR_FOURLEGS     },
   {   "fourhooves",  ITEM_WEAR_FOURHOOVES   },
   {   "eyes",        ITEM_WEAR_EYES         },
   {   "ear",         ITEM_WEAR_EAR          },
   {   "bosom",       ITEM_WEAR_BOSOM        },
   {   "insidecover", ITEM_WEAR_INSIDE_COVER },
   {   NULL,          -1                     }
};

const struct search_type search_type_table[] =
{
   {   "none",        SEARCH_NONE,     SCOPE_NONE                        },
   {   "old",         SEARCH_OLD,      SCOPE_NONE                        },
   {   "name",        SEARCH_NAME,     SCOPE_OBJECT|SCOPE_MOB|SCOPE_ROOM },
   {   "material",    SEARCH_MATERIAL, SCOPE_OBJECT                      },
   {   "damclass",    SEARCH_DAMCLASS, SCOPE_OBJECT                      },
   {   "weapon",      SEARCH_WEAPON,   SCOPE_OBJECT                      },
   {   "item",        SEARCH_ITEM,     SCOPE_OBJECT                      },
   {   "slot",        SEARCH_SLOT,     SCOPE_OBJECT                      },
   {   "affect",      SEARCH_AFFECT,   SCOPE_OBJECT|SCOPE_MOB            },
   {   "flag",        SEARCH_FLAG,     SCOPE_OBJECT|SCOPE_MOB|SCOPE_ROOM },
   {   "origin",      SEARCH_ORIGIN,   SCOPE_OBJECT|SCOPE_MOB            },
   {   "owner",       SEARCH_OWNER,    SCOPE_OBJECT|SCOPE_MOB            },
   {   "spell",       SEARCH_SPELL,    SCOPE_OBJECT                      },
   {   "level",       SEARCH_LEVEL,    SCOPE_OBJECT|SCOPE_MOB            },
   {   "limit",       SEARCH_LIMIT,    SCOPE_OBJECT                      },
   {   "cost",        SEARCH_COST,     SCOPE_OBJECT                      },
   {   "vnum",        SEARCH_VNUM,     SCOPE_OBJECT|SCOPE_MOB            },
   {   "string",      SEARCH_STRING,   SCOPE_OBJECT|SCOPE_MOB|SCOPE_ROOM },
   {   NULL,          -1,              -1                                }
};

const ASSOCIATION_TYPE affect_type_table[] =
{
   {   "none",                  APPLY_NONE             },
   {   "strength",              APPLY_STR              },
   {   "dexterity",             APPLY_DEX              },
   {   "intelligence",          APPLY_INT              },
   {   "wisdom",                APPLY_WIS              },
   {   "constitution",          APPLY_CON              },
   {   "sex",                   APPLY_SEX              },
   {   "class",                 APPLY_CLASS            },
   {   "level",                 APPLY_LEVEL            },
   {   "size",                  APPLY_HEIGHT           },
   {   "age",                   APPLY_AGE              },
   {   "mana",                  APPLY_MANA             },
   {   "hp",                    APPLY_HIT              },
   {   "moves",                 APPLY_MOVE             },
   {   "gold",                  APPLY_GOLD             },
   {   "experience",            APPLY_EXP              },
   {   "size",                  APPLY_SIZE             },
   {   "armor class",           APPLY_AC               },
   {   "hit roll",              APPLY_HITROLL          },
   {   "damage roll",           APPLY_DAMROLL          },
   {   "sight",                 APPLY_SIGHT            },
   {   "holy power",            APPLY_HOLY_POWER       },
   {   "spell power",           APPLY_SPELL_POWER      },
   {   "saves",                 APPLY_SAVES            },
   {   "save vs maledictions",  APPLY_SAVING_MALEDICT  },
   {   "save vs transport",     APPLY_SAVING_TRANSPORT },
   {   "save vs breath",        APPLY_SAVING_BREATH    },
   {   "save vs spell",         APPLY_SAVING_SPELL     },
   {   "none2",                 APPLY_SPELL_AFFECT     },
   {   "form",                  APPLY_MORPH_FORM       },
   {   "regeneration",          APPLY_REGENERATION     },
   {   "accuracy",              APPLY_ACCURACY         },
   {   "attack speed",          APPLY_ATTACKS          },
   {   "mana regeneration",     APPLY_MN_REGENERATION  },
   {   "weight",                APPLY_WEIGHT           },
   {   NULL,                    -1                     }
};

const struct flag_info flag_type_table[] =
{
   {   "affects",        affect_flags,    affect2_flags, TO_AFFECTS, SCOPE_OBJECT|SCOPE_MOB   },
   {   "object",         extra_flags,     extra2_flags,  TO_OBJECT,  SCOPE_OBJECT|SCOPE_MOB   },
   {   "weapon",         weapon_flags,    NULL,          TO_WEAPON,  SCOPE_OBJECT },
   {   "immunity",       imm_flags,       NULL,          TO_IMMUNE,  SCOPE_OBJECT|SCOPE_MOB   },
   {   "resistance",     imm_flags,       NULL,          TO_RESIST,  SCOPE_OBJECT|SCOPE_MOB   },
   {   "vulnerability",  imm_flags,       NULL,          TO_VULN,    SCOPE_OBJECT|SCOPE_MOB   },
   {   NULL,             NULL,            NULL,          -1,         SCOPE_NONE   }
};

const char* wear_slot_names[] =
{
   "LIGHT",
   "FINGER_L",
   "FINGER_R",
   "NECK_1",
   "NECK_2",
   "BODY",
   "HEAD",
   "LEGS",
   "FEET",
   "HANDS",
   "ARMS",
   "SHIELD",
   "ABOUT",
   "WAIST",
   "WRIST_L",
   "WRIST_R",
   "WIELD",
   "HOLD",
   "DUAL_WIELD",
   "TERTIARY_WIELD",
   "BRAND",
   "HORNS",
   "SNOUT",
   "HOOVES",
   "FOURLEGS",
   "FOURHOOVES",
   "EYES",
   "EAR_L",
   "EAR_R",
   "BOSOM",
};

const ASSOCIATION_TYPE damage_class_table[] =
{
   { "none",      DAM_NONE      },
   { "bash",      DAM_BASH      },
   { "pierce",    DAM_PIERCE    },
   { "slash",     DAM_SLASH     },
   { "fire",      DAM_FIRE      },
   { "cold",      DAM_COLD      },
   { "lightning", DAM_LIGHTNING },
   { "acid",      DAM_ACID      },
   { "poison",    DAM_POISON    },
   { "negative",  DAM_NEGATIVE  },
   { "holy",      DAM_HOLY      },
   { "energy",    DAM_ENERGY    },
   { "mental",    DAM_MENTAL    },
   { "disease",   DAM_DISEASE   },
   { "drowning",  DAM_DROWNING  },
   { "light",     DAM_LIGHT     },
   { "other",     DAM_OTHER     },
   { "harm",      DAM_HARM      },
   { "charm",     DAM_CHARM     },
   { "sound",     DAM_SOUND     },
   { "iron",      DAM_IRON      },
   { "wood",      DAM_WOOD      },
   { "silver",    DAM_SILVER    },
   { "metal",     DAM_METAL     },
   { "spells",    DAM_SPELLS    },
   { "nature",    DAM_NATURE    },
   { NULL,        -1            }
};
