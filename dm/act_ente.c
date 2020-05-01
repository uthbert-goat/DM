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

static const char rcsid[] = "$Id: act_ente.c,v 1.31 2004/10/04 19:33:17 fizzfaldt Exp $";

#if defined(macintosh)
#include <types.h>
#include <time.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"

/* command procedures needed */
DECLARE_DO_FUN(do_look        );
DECLARE_DO_FUN(do_stand        );
void portal_enter(CHAR_DATA *ch, OBJ_DATA *portal, ROOM_INDEX_DATA *location);

bool can_teleport(CHAR_DATA* ch, ROOM_INDEX_DATA* room)
{
   if (room == NULL)
   {
      return FALSE;
   }
   if
   (
      ch == NULL ||
      room == ch->in_room
   )
   {
      return TRUE;
   }
   if
   (
      room == NULL ||
      (
         room->guild &&
         (
            room->guild - 1 != ch->class ||
            IS_SET(ch->act2, PLR_GUILDLESS)
         )
      ) ||
      IS_SET(room->extra_room_flags, ROOM_1212) ||
      (
         room->house &&
         room->house != ch->house &&
         ch->in_room->house != room->house
      ) ||
      (
         is_shrine(room) &&
         (
            !is_shrine(ch->in_room) ||
            ch->in_room->area != room->area
         )
      ) ||
      (
         (
            room->vnum >= 1035 &&
            room->vnum <= 1050
         ) &&
         (
            ch->in_room->vnum < 1035 ||
            ch->in_room->vnum > 1050
         )
      ) ||
      !can_see_room(ch, room) ||
      IS_SET(room->room_flags, ROOM_PET_SHOP) ||
      IS_SET(room->room_flags, ROOM_NO_TELEPORT) ||
      (
         IS_SET(room->extra_room_flags, ROOM_DEAD_ZONE) &&
         !IS_SET(room->room_flags, ROOM_CONSECRATED)
      )
   )
   {
      return FALSE;
   }
   return TRUE;
}

ROOM_INDEX_DATA* get_random_room(CHAR_DATA* ch, sh_int scope)
{
   CHAR_DATA* temp;
   ROOM_INDEX_DATA* room;
   AREA_DATA* parea;
   AREA_DATA* area;
   unsigned int low_vnum = 0;
   unsigned int high_vnum = MAX_VNUM - 1;
   int choice;
   unsigned int curr;
   int counter;
   int total;
   static char* areas[] =
   {
      "Western Midgaard",
      "City of Ofcol",
      "Plains",
      "Haon Dor",
      "Elemental Canyon",
      "Thalos",
      "Goblin Village",
      "Marsh",
      "Miden'nir",
      "Tyr-Zinet",
      "Sewers of Glyndane",
      "Grove of Despair",
      "Elvenhame",
      "Tarot Tower",
      "Arkham",
      NULL
      /*
      No longer exists
      "Shire",
      "New Thalos",
      "Ofcol",
      "In the Air",
      "Troll Den",
      */
   };
   static char* areas_search[20] =
   {
      NULL,
   };
   static unsigned int total_areas = 0;

   if (ch == NULL)
   {
      bug("NULL character in get_random_room.", 0);
      return NULL;
   }
   if (ch->in_room == NULL)
   {
      bug("NULL ch->in_room in get_random_room.", 0);
      return NULL;
   }
   if (ch->in_room->area == NULL)
   {
      bug("NULL ch->in_room->area in get_random_room.", 0);
      return NULL;
   }
   if
   (
      scope == RANDOM_AREA ||
      scope == RANDOM_AREA_FOREST ||
      scope == RANDOM_AREA_IMAGE
   )
   {
      low_vnum = ch->in_room->area->min_vnum;
      high_vnum = ch->in_room->area->max_vnum;
      room = ch->in_room;
      area = ch->in_room->area;
   }
   else
   {
      area = NULL;
   }
   if (scope == RANDOM_MOB_TELEPORT)
   {
      /*
         Cape of the other side quest
         Only certain areas allowed.
         Randomly pick one of the areas allowed,
         and then a room from it.
         First will make certain all (or some)
         of the areas exist.
         Any that don't will be ignored in the future,
         if none exist, and we're on main
         it will report a bug.
      */
      if
      (
         total_areas &&
         areas_search[0] == NULL
      )
      {
         if (!is_test)
         {
            bug("No areas to teleport to for cape of other side quest.", 0);
         }
         return NULL;
      }
      else if (!total_areas)
      {
         /* Run only once */
         /* Find all areas that exist */
         for (curr = 0; areas[curr] != NULL; curr++)
         {
            for (parea = area_first; parea; parea = parea->next)
            {
               if (!str_cmp(parea->name, areas[curr]))
               {
                  areas_search[total_areas] = areas[curr];
                  total_areas++;
                  break;
               }
            }
         }
         if (areas_search[0] == NULL)  /* Found none */
         {
            total_areas = -1;  /* Will trigger the return NULL above */
         }
      }
      curr = number_range(0, total_areas - 1);
      for (parea = area_first; parea; parea = parea->next)
      {
         if (!str_cmp(parea->name, areas_search[curr]))
         {
            break;
         }
      }
      if (parea)
      {
         low_vnum = parea->min_vnum;
         high_vnum = parea->max_vnum;
         scope = RANDOM_AREA_IMAGE;
         area = parea;
      }
      else
      {
         sprintf
         (
            log_buf,
            "Area [%s] not found in RANDOM_MOB_TELEPORT",
            areas_search[curr]
         );
         bug(log_buf, 0);
         return NULL;
      }
   }
   if
   (
      low_vnum >= MAX_VNUM ||
      high_vnum >= MAX_VNUM
   )
   {
      sprintf
      (
         log_buf,
         "get_random_room([%s] [%d], [%d]) range is [%ud] to [%ud].  In area [%s]",
         (
            IS_NPC(ch) ?
            ch->short_descr :
            ch->name
         ),
         (
            IS_NPC(ch) ?
            ch->pIndexData->vnum :
            0
         ),
         scope,
         low_vnum,
         high_vnum,
         ch->in_room->area->name
      );
      bug_trust(log_buf, 0, get_trust(ch));
      return NULL;
   }
   total = 0;
   temp = ch;  /* area images uses NULL, room for can_teleport */
   if
   (
      scope == RANDOM_AREA_IMAGE ||
      scope == RANDOM_ALL
   )
   {
      ch = NULL;
   }
   for (parea = area_first; parea; parea = parea->next)
   {
      if
      (
         area == NULL ||
         parea == area
      )
      {
         for (room = parea->rooms; room; room = room->next_in_area)
         {
            if
            (
               can_teleport(ch, room) &&
               (
                  scope != RANDOM_AREA_FOREST ||
                  room->sector_type == SECT_FOREST ||
                  room->sector_type == SECT_UNDERGROUND ||
                  room->sector_type == SECT_HILLS ||
                  room->sector_type == SECT_MOUNTAIN
               )
            )
            {
               total++;
            }
         }
      }
   }
   ch = temp;  /* For bug output */
   if (total == 0)
   {
      snprintf
      (
         log_buf,
         sizeof(log_buf),
         "No rooms to choose from in get_random_room(%s, %d)",
         (
            ch != NULL ?
            ch->name :
            "NULL"
         ),
         scope
      );
      bug_trust(log_buf, 0, get_trust(ch));
      return (ch->in_room);
   }
   if
   (
      scope == RANDOM_AREA_IMAGE ||
      scope == RANDOM_ALL
   )
   {
      ch = NULL;
   }
   choice = number_range(1, total);
   counter = 0;
   for (parea = area_first; parea; parea = parea->next)
   {
      if
      (
         area == NULL ||
         parea == area
      )
      {
         for (room = parea->rooms; room; room = room->next_in_area)
         {
            if
            (
               can_teleport(ch, room) &&
               (
                  scope != RANDOM_AREA_FOREST ||
                  room->sector_type == SECT_FOREST ||
                  room->sector_type == SECT_UNDERGROUND ||
                  room->sector_type == SECT_HILLS ||
                  room->sector_type == SECT_MOUNTAIN
               ) &&
               ++counter == choice
            )
            {
               return room;
            }
         }
      }
   }
   ch = temp;
   snprintf
   (
      log_buf,
      sizeof(log_buf),
      "Can't find random room in get_random_room(%s, %d)",
      (
         ch != NULL ?
         ch->name :
         "NULL"
      ),
      scope
   );
   bug_trust(log_buf, 0, get_trust(ch));
   return (ch->in_room);
}

bool is_shrine(ROOM_INDEX_DATA *room)
{
   int vnum;
   if (room == NULL) return FALSE;
   if (IS_SET(room->room_flags, ROOM_SHRINE)) return TRUE;
   vnum = room->vnum;
   if ((vnum >=12500 && vnum <=12599) ||
   (vnum >=800   && vnum <= 899)  ||
   (vnum >=14600 && vnum <=14699) ||
   (vnum >=2100  && vnum <=2199)  ||
   (vnum >=14250 && vnum <=14260) ||
   (vnum >=7350  && vnum <=7399)  ||
   (vnum >=1200  && vnum <=1299)  ||
   (vnum >=15300 && vnum <=15399) ||
   (vnum >=17639 && vnum <=17654) ||
   (vnum >=876   && vnum <=899)   ||
   (vnum >=15890 && vnum <=15899) ||
   (vnum >=28900 && vnum <=28999) ||
   (vnum >=14900 && vnum <=14999) ||
   (vnum >=28200 && vnum <=28220) ||
   (vnum >=100   && vnum <=199)   ||
   (vnum >=5400  && vnum <=5499)  ||
   (vnum >=500   && vnum <=520)   ||
   (vnum >=15900 && vnum <=15999) ||
   (vnum >=3870  && vnum <=3899)  ||
   (vnum >=17801 && vnum <=17806) ||
   (vnum >=7572  && vnum <=7599)  ||
   (vnum >=18100 && vnum <=18110) ||
   (vnum >=21670 && vnum <=21699))
   return TRUE;
   return FALSE;
}

void do_portal(CHAR_DATA* ch, char* argument)
{
   ROOM_INDEX_DATA* location;
   ROOM_INDEX_DATA* old_room;
   OBJ_DATA* portal;
   CHAR_DATA* move_too;
   CHAR_DATA* next_move;

   if (ch == NULL)
   {
      return;
   }
   fix_fighting(ch);
   if (ch->fighting != NULL)
   {
      send_to_char("Not while you are fighting.\n\r", ch);
      return;
   }

   if
   (
      (
         old_room = ch->in_room
      ) == NULL
   )
   {
      return;
   }

   for
   (
      portal = old_room->contents;
      portal != NULL;
      portal = portal->next_content
   )
   {
      if (portal->item_type == ITEM_PORTAL_NEW)
      {
         break;
      }
   }
   if (portal == NULL)
   {
      send_to_char("You do not see anything like that here.\n\r", ch);
      return;
   }
   if
   (
      (
         location = get_room_index(portal->value[0])
      ) == NULL
   )
   {
      send_to_char("The portal does not lead anywhere!\n\r", ch);
      return;
   }
   if
   (
      IS_SET(location->room_flags, ROOM_BLOODY_TIMER) &&
      (
         ch->pause > 0 ||
         ch->quittime > 13
      )
   )
   {
      send_to_char("You are too bloody to enter that portal.\n\r", ch);
      return;
   }

   if (IS_SET(portal->value[4], PORTAL_CLOSED))
   {
      send_to_char("You do not see anything like that here.\n\r", ch);
      return;
   }

   if (portal->value[1] > 0)
   {
      send_to_char("The portal is still forming.\n\r", ch);
      return;
   }

   portal_enter(ch, portal, location);
   fix_fighting(ch);
   if (location == old_room)
   {
      return;
   }
   for (move_too = char_list; move_too != NULL; move_too = next_move)
   {
      next_move = move_too->next;
      if
      (
         move_too->in_room != old_room ||
         move_too->master != ch ||
         (
            !IS_NPC(move_too) &&
            move_too->position < POS_STANDING
         )
      )
      {
         continue;
      }
      if (move_too->fighting != NULL)
      {
         stop_fighting(move_too, FALSE);
      }
      if
      (
         IS_AFFECTED(move_too, AFF_CHARM) &&
         move_too->position < POS_STANDING &&
         move_too->daze <= 0 &&
         move_too->wait <= 0
      )
      {
         do_stand(move_too, "");
      }
      if (move_too->position < POS_STANDING)
      {
         continue;
      }
      do_portal(move_too, argument);
   }

   return;
}

void portal_enter(CHAR_DATA *ch, OBJ_DATA *portal, ROOM_INDEX_DATA *location)
{
   un_camouflage(ch, NULL);
   un_hide(ch, NULL);
   un_earthfade(ch, NULL);

   act("$w leaves through $p.", ch, portal, NULL, TO_ROOM);

   if (portal->pIndexData->vnum == OBJ_VNUM_MARAUDER_TRAPDOOR)
   {
      act("You descend through $p.", ch, portal, NULL, TO_CHAR);
   }
   else
   {
      act("You step through $p.", ch, portal, NULL, TO_CHAR);
   }

   char_from_room(ch);
   char_to_room(ch, location);
   act("$w has arrived.", ch, NULL, NULL, TO_ROOM);
   do_observe(ch, "", LOOK_AUTO);
   return;
}

/* The get_random_mob function has been written for Marauder misinformation-summon combination.
   If you intend to use for other purposes examine carefully - Kontina */

CHAR_DATA* get_random_mob(CHAR_DATA* ch, sh_int scope)
{
   bool found = FALSE;
   CHAR_DATA* rch;
   int vnum_range_start;

   while (found == FALSE)
   {
      vnum_range_start = number_range(0, 7) * RANDOM_MOB_VNUM_RANGE * scope;
      for ( rch = char_list; rch != NULL; rch = rch->next )
      {
          if
          (
             IS_NPC( rch ) &&
             rch->in_room != NULL &&
             rch->in_room->vnum >= vnum_range_start &&
             rch->in_room->vnum < (vnum_range_start + RANDOM_MOB_VNUM_RANGE * scope) &&
             number_range(1, 100) > RANDOM_MOB_GRAB_CHANCE &&
             rch != ch
          )
          {
             found = TRUE;
             break;
          }
      }
   }
   if
   (
      rch == NULL &&
      ch == NULL
   )
   {
      bug("NULL room from get_random_mob", 0);
      return NULL;
   }
   else if
   (
      rch == NULL
   )
   {
      return ch;  /* To make sure we return a valid target */
   }
   else
   {
      return rch;
   }
}
