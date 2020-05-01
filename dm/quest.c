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


static const char rcsid[] = "$Id: quest.c,v 1.3 2004/10/04 19:33:18 fizzfaldt Exp $";

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "merc.h"
#include "database.h"

void do_real_quest_help(CHAR_DATA* viewer, char* help_topic)
{
   if (help_topic[0] == '\0')
   {
      send_to_char(
         "NAME\n\r"
         "       quest - accesses the quest system\n\r"
         "\n\r"
         "SYNOPSIS\n\r"
         "       quest <command> <parameters>\n\r"
         "\n\r"
         "COMMANDS\n\r"
         "       help     Shows this help. You can get help on a particular command\n\r"
         "                by specifying the command in <parameters>.\n\r"
         "\n\r"
         "       current  Lists your current quests.\n\r"
         "\n\r"
         "       old      Lists your completed quests.\n\r"
         "\n\r"
         "       read     Views history of a quest specified by number in <parameters>.\n\r",
         viewer);
   }

   else if (!str_prefix(help_topic, "help"))
   {
      send_to_char(
         "NAME\n\r"
         "       quest help - shows help for the quest command\n\r"
         "\n\r"
         "SYNOPSIS\n\r"
         "       quest help <command>\n\r",
         viewer);
   }

   else if (!str_prefix(help_topic, "current"))
   {
      send_to_char(
         "NAME\n\r"
         "       quest current - lists your current quests\n\r",
         viewer);
   }

   else if (!str_prefix(help_topic, "old"))
   {
      send_to_char(
         "NAME\n\r"
         "       quest old - lists your old quests\n\r",
         viewer);
   }

   else if (!str_prefix(help_topic, "read"))
   {
      send_to_char(
         "NAME\n\r"
         "       quest read - views history of a specified quest\n\r"
         "\n\r"
         "SYNOPSIS\n\r"
         "       quest read <quest number>\n\r"
         "\n\r"
         "QUEST NUMBER\n\r"
         "\n\r"
         "       You can find the quest number in your list of current or old\n\r"
         "       quests. You may only read about quests you have discovered.\n\r",
         viewer);
   }
}

void do_real_quest(CHAR_DATA* viewer, CHAR_DATA* questor, char* argument)
{
   char quest_command[MAX_INPUT_LENGTH];

   argument = one_argument(argument, quest_command);

   if (quest_command[0] == '\0')
   {
      do_real_quest_help(viewer, "");
   }

   else if (!str_prefix(quest_command, "help"))
   {
      do_real_quest_help(viewer, argument);
   }

   else if (!str_prefix(quest_command, "current"))
   {
      /* Redirect to the database component */
      database_list_quests(viewer, questor, '0');
   }

   else if (!str_prefix(quest_command, "old"))
   {
      /* Redirect to the database component */
      database_list_quests(viewer, questor, '1');
   }

   else if (!str_prefix(quest_command, "read"))
   {
      /* Redirect to the database component */
      char quest_id_str[MAX_INPUT_LENGTH];
      int  quest_id_major;

      argument = one_argument(argument, quest_id_str);
      quest_id_major = atoi(quest_id_str);
      database_read_quest(viewer, questor, quest_id_major);
   }

   else
   {
      send_to_char("Unknown quest command.\n\r", viewer);
      do_real_quest_help(viewer, "");
   }
}

void do_quest(CHAR_DATA* ch, char* argument)
{
   do_real_quest(ch, ch, argument);
}

void do_getquest(CHAR_DATA* ch, char* argument)
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA* victim;

   argument = one_argument(argument, arg);
   victim = get_char_world(ch, arg);

   do_real_quest(ch, victim, argument);
}

