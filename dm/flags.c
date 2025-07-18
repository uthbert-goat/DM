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

static const char rcsid[] = "$Id: flags.c,v 1.8 2004/09/09 03:42:35 xurinos Exp $";

#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"
#include "tables.h"

int flag_lookup args( ( const char *name, const struct flag_type *flag_table) );
extern ROOM_INDEX_DATA *find_location( CHAR_DATA *ch, char *arg );

void do_flag(CHAR_DATA *ch, char *argument)
{
   char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH], arg3[MAX_INPUT_LENGTH];
   char word[MAX_INPUT_LENGTH];
   CHAR_DATA *victim = NULL;
   OBJ_DATA  *target;
   ROOM_INDEX_DATA *location;
   long *flag, old = 0, new = 0, marked = 0, pos;
   char type;
   const struct flag_type *flag_table;

   argument = one_argument(argument, arg1);
   argument = one_argument(argument, arg2);
   argument = one_argument(argument, arg3);

   type = argument[0];

   if (type == '=' || type == '-' || type == '+')
   argument = one_argument(argument, word);

   if (arg1[0] == '\0')
   {
      send_to_char("Syntax:\n\r", ch);
      send_to_char("  flag mob      <name> <field> <flags>\n\r", ch);
      send_to_char("  flag char     <name> <field> <flags>\n\r", ch);
      send_to_char("  flag obj      <name> <field> <flags>\n\r", ch);
      send_to_char("  flag room <location> <field> <flags>\n\r", ch);
      send_to_char("  mob  flags: act, aff, off, imm, res, vuln, form, part\n\r", ch);
      send_to_char("  char flags: plr, comm, aff, imm, res, vuln\n\r", ch);
      send_to_char("  obj  flags: extra, weapon\n\r", ch);
      send_to_char("  room flags: flags\n\r", ch);
      send_to_char("  +: add flag, -: remove flag, = set equal to\n\r", ch);
      send_to_char("  otherwise flag toggles the flags listed.\n\r", ch);
      return;
   }

   if (arg2[0] == '\0')
   {
      send_to_char("What do you wish to set flags on?\n\r", ch);
      return;
   }

   if (arg3[0] == '\0')
   {
      send_to_char("You need to specify a flag to set.\n\r", ch);
      return;
   }

   if (argument[0] == '\0')
   {
      send_to_char("Which flags do you wish to change?\n\r", ch);
      return;
   }

   if (!str_prefix(arg1, "mob") || !str_prefix(arg1, "char"))
   {
      victim = get_char_world(ch, arg2);
      if (victim == NULL)
      {
         send_to_char("You can't find them.\n\r", ch);
         return;
      }

      /* select a flag to set */
      if (!str_prefix(arg3, "act"))
      {
         if (!IS_NPC(victim))
         {
            send_to_char("Use plr for PCs.\n\r", ch);
            return;
         }

         flag = &victim->act;
         flag_table = act_flags;
      }
      else if (!str_prefix(arg3, "plr"))
      {
         if (IS_NPC(victim))
         {
            send_to_char("Use act for NPCs.\n\r", ch);
            return;
         }

         flag = &victim->act;
         flag_table = plr_flags;
      }
      else if (!str_prefix(arg3, "aff"))
      {
         flag = &victim->affected_by;
         flag_table = affect_flags;
      }
      else if (!str_prefix(arg3, "immunity"))
      {
         flag = &victim->imm_flags;
         flag_table = imm_flags;
      }
      else if (!str_prefix(arg3, "resist"))
      {
         flag = &victim->res_flags;
         flag_table = imm_flags;
      }
      else if (!str_prefix(arg3, "vuln"))
      {
         flag = &victim->vuln_flags;
         flag_table = imm_flags;
      }
      else if (!str_prefix(arg3, "form"))
      {
         if (!IS_NPC(victim))
         {
            send_to_char("Form can't be set on PCs.\n\r", ch);
            return;
         }

         flag = &victim->form;
         flag_table = form_flags;
      }
      else if (!str_prefix(arg3, "parts"))
      {
         if (!IS_NPC(victim))
         {
            send_to_char("Parts can't be set on PCs.\n\r", ch);
            return;
         }

         flag = &victim->parts;
         flag_table = part_flags;
      }
      else if (!str_prefix(arg3, "comm"))
      {
         if (IS_NPC(victim))
         {
            send_to_char("Comm can't be set on NPCs.\n\r", ch);
            return;
         }

         flag = &victim->comm;
         flag_table = comm_flags;
      }
      else
      {
         send_to_char("That's not an acceptable flag.\n\r", ch);
         return;
      }
   }
   else
   if ( !str_prefix(arg1, "obj") )
   {
      target = get_obj_world(ch, arg2);

      if (target == NULL)
      {
         send_to_char("You can't find that.\n\r", ch);
         return;
      }
      /* select a flag to set */
      if (!str_prefix(arg3, "extra"))
      {
         flag = (long *)&target->extra_flags;
         flag_table = extra_flags;
      }
      else if (!str_prefix(arg3, "weapon"))
      {
         flag = (long *)&target->value[4];
         flag_table = weapon_flags;
      }
      else
      {
         send_to_char("That's not an acceptable flag.\n\r", ch);
         return;
      }
   }
   else
   if ( !str_prefix(arg1, "room") )
   {
      location = find_location(ch, arg2);

      if ( location == NULL )
      {
         send_to_char("No such location.\n\r", ch);
         return;
      }

      if ( !is_room_owner(ch, location)
      &&  ch->in_room != location
      &&  room_is_private(location)
      && !IS_TRUSTED(ch, IMPLEMENTOR))
      {
         send_to_char("That room is private right now.\n\r", ch);
         return;
      }

      /* select a flag to set */
      if ( !str_prefix(arg3, "flags"))
      {
         flag = (long *)&location->room_flags;
         flag_table = room_flags;
      }
      else
      {
         send_to_char("That's not an acceptable flag.\n\r", ch);
         return;
      }
   }
   else
   {
      send_to_char("That is not an acceptable target.\n\r", ch);
      return;
   }

   old = *flag;
   if ( victim )
   victim->zone = NULL;

   if (type != '=')
   new = old;

   /* mark the words */
   for (; ;)
   {
      argument = one_argument(argument, word);

      if (word[0] == '\0')
      break;

      pos = flag_lookup(word, flag_table);
      if (pos == 0)
      {
         send_to_char("That flag doesn't exist!\n\r", ch);
         return;
      }
      else
      SET_BIT(marked, pos);
   }

   for (pos = 0; flag_table[pos].name != NULL; pos++)
   {
      if (!flag_table[pos].settable && IS_SET(old, flag_table[pos].bit))
      {
         SET_BIT(new, flag_table[pos].bit);
         continue;
      }

      if (IS_SET(marked, flag_table[pos].bit))
      {
         switch(type)
         {
            case '=':
            case '+':
            SET_BIT(new, flag_table[pos].bit);
            break;
            case '-':
            REMOVE_BIT(new, flag_table[pos].bit);
            break;
            default:
            if (IS_SET(new, flag_table[pos].bit))
            REMOVE_BIT(new, flag_table[pos].bit);
            else
            SET_BIT(new, flag_table[pos].bit);
         }
      }
   }

   *flag = new;
   return;
}
