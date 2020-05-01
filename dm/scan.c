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

static const char rcsid[] = "$Id: scan.c,v 1.11 2004/09/29 16:18:44 xurinos Exp $";

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"

void show_char_to_char_0 args((CHAR_DATA *victim, CHAR_DATA *ch));
void scan_list           args((ROOM_INDEX_DATA *scan_room, CHAR_DATA *ch,
sh_int depth, sh_int door));
bool ill_wall(ROOM_INDEX_DATA *room, int door);

void do_scan(CHAR_DATA *ch, char *argument)
{
   extern char *const dir_name[];
   char arg1[MAX_INPUT_LENGTH], buf[MAX_INPUT_LENGTH];
   ROOM_INDEX_DATA *scan_room;
   EXIT_DATA *pExit;
   sh_int door, depth, mod, max_depth;
   CHAR_DATA *ich;
   CHAR_DATA *rch;
   char buf2[MAX_STRING_LENGTH];
   char buf3[MAX_STRING_LENGTH];

   argument = one_argument(argument, arg1);

   if (!str_cmp(arg1, "n") || !str_cmp(arg1, "north")) door = 0;
   else if (!str_cmp(arg1, "e") || !str_cmp(arg1, "east"))  door = 1;
   else if (!str_cmp(arg1, "s") || !str_cmp(arg1, "south")) door = 2;
   else if (!str_cmp(arg1, "w") || !str_cmp(arg1, "west"))  door = 3;
   else if (!str_cmp(arg1, "u") || !str_cmp(arg1, "up" ))   door = 4;
   else if (!str_cmp(arg1, "d") || !str_cmp(arg1, "down"))  door = 5;
   else if (!str_cmp(arg1, "p") || !str_cmp(arg1, "portal")) door = 6;
   else { send_to_char("Which way do you want to scan?\n\r", ch); return; }

   if (is_affected(ch, gsn_bloodmist))
   {
      send_to_char("Your eyes are clouded with blood!\n\r", ch);
      return;
   }

   if (is_affected(ch, gsn_delusions))
   {
      send_to_char("You try to scan but only see purple mist.\n\r", ch);
      return;
   }
   if (IS_AFFECTED(ch, AFF_BLIND))
   {
      send_to_char("You can't see a thing!\n\r", ch);
      return;
   }

   if (door == 6){
      OBJ_DATA * portal;
      ROOM_INDEX_DATA * room;
      for (portal =ch->in_room->contents;portal!=NULL;portal=portal->next_content)
      if (portal->item_type == ITEM_PORTAL_NEW)
      break;
      if (portal == NULL ||
      IS_SET(portal->value[4], PORTAL_CLOSED))
      {
         send_to_char("Nothing like that here.\n\r", ch);
         return;
      }
      act("$n peers through $p.", ch, portal, NULL, TO_ROOM);
      act("You peer through $p.", ch, portal, NULL, TO_CHAR);
      room = get_room_index(portal->value[0]);
      if (room == NULL){
         send_to_char("You don't see the other end.\n\r", ch);
         return;
      }
      send_to_char("Looking through it you see:\n\r", ch);
      scan_list(room, ch, 1, 6);
      send_to_char("\n\r", ch);
      return;
   }
   act("You peer intently $T.", ch, NULL, dir_name[door], TO_CHAR);

   /* Original code
   act("$n peers intently $T.", ch, NULL, dir_name[door], TO_ROOM);
   */
   /* Mortals dont see wizi imm's scan - werv  */
   sprintf(buf2, "$n peers intently %s.", dir_name[door]);
   for ( ich = ch->in_room->people;ich != NULL; ich = ich->next_in_room)
   {
      if (!((!can_see(ich, ch)) && (IS_IMMORTAL(ch))))
      act(buf2, ch, NULL, ich, TO_VICT);
   }

   sprintf(buf, "Looking %s you see:\n\r", dir_name[door]);
   sprintf(buf3, "Looking %s you see:\n\r", dir_name[door]);

   if (is_affected_room(ch->in_room, gsn_corrupt("hall of mirrors", &gsn_hall_mirrors)))
   {
      sprintf(buf2, "=== 1 %s ===", dir_name[door]);
      strcat(buf3, buf2);
      send_to_char(buf3, ch);
      send_to_char("\n\r", ch);
      for (rch=ch->in_room->people; rch != NULL; rch=rch->next_in_room)
      {
         if (rch == ch) continue;
         if (!IS_NPC(rch) && rch->invis_level > get_trust(ch)) continue;
         if (can_see(ch, rch)) show_char_to_char_0(rch, ch);
      }
      return;
   }

   scan_room = ch->in_room;
   mod = 1;
   if (is_affected(ch, gsn_high_herb) && ch->level > 20)
   mod -= 1;
   if (is_affected(ch, gsn_high_herb) && ch->level > 10)
   mod -= 1;
   mod += ch->sight;
   max_depth = URANGE(1, (ch->level / 10) + mod, 20);

   for (depth = 1; depth <= max_depth; depth++)
   {
      if ((pExit = scan_room->exit[door]) != NULL)
      {
         if (!IS_SET(pExit->exit_info, EX_CLOSED) && !ill_wall(scan_room, door))
         {
            scan_room = pExit->u1.to_room;
            if (scan_room == NULL) return;
            scan_list(pExit->u1.to_room, ch, depth, door);
            send_to_char("\n\r", ch);
         }
      }
   }
   return;
}

bool ill_wall(ROOM_INDEX_DATA *room, int door)
{
   ROOM_AFFECT_DATA *raf;
   if (is_affected_room(room, gsn_illusionary_wall))
   {
      raf = room->affected;
      while(raf != NULL)
      {
         if (raf->type == gsn_illusionary_wall && raf->modifier == door)
         return TRUE;
         raf = raf->next;
      }
   }
   return FALSE;
}

void scan_list(ROOM_INDEX_DATA *scan_room, CHAR_DATA *ch, sh_int depth,
sh_int door)
{
   CHAR_DATA *rch;
   extern char *const dir_name[];
   char buf[MAX_INPUT_LENGTH];

   buf[0] = '\0';

   if (scan_room == NULL) return;
   if (door != 6)
   {
      sprintf(buf, "=== %d %s ===\n\r", depth, dir_name[door]);
      send_to_char(buf, ch);
   }
   else
   {
      sprintf(buf, "=== through the portal ===\n\r");
      send_to_char(buf, ch);
   }
   for (rch=scan_room->people; rch != NULL; rch=rch->next_in_room)
   {
      if (rch == ch) continue;
      if (!IS_NPC(rch) && rch->invis_level > get_trust(ch)) continue;
      if (can_see(ch, rch)) show_char_to_char_0(rch, ch);
   }
   return;
}
