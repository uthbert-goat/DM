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

static const char rcsid[] = "$Id: act_move.c,v 1.184 2004/11/25 06:37:40 fizzfaldt Exp $";

#if defined(macintosh)
#include <types.h>
#include <time.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "merc.h"
#include "magic.h"
#include "subclass.h"
#include "interp.h"
#include "recycle.h"
#include "tables.h"

int house_lookup(char *);
/* command procedures needed */
DECLARE_DO_FUN(do_open);
DECLARE_DO_FUN(do_unlock);
DECLARE_DO_FUN(do_pick);
DECLARE_DO_FUN(do_ambush);
DECLARE_DO_FUN(do_wear);
DECLARE_DO_FUN(do_look        );
DECLARE_DO_FUN(do_housetalk    );
DECLARE_DO_FUN(do_recall    );
DECLARE_DO_FUN(do_stand        );
DECLARE_DO_FUN(do_freetell      );
DECLARE_DO_FUN(do_camp        );
DECLARE_DO_FUN(do_animal_call    );
DECLARE_DO_FUN(do_say        );
DECLARE_DO_FUN(do_emote    );
DECLARE_DO_FUN(do_yell        );
DECLARE_DO_FUN(do_lurk        );
DECLARE_DO_FUN(do_book_open);
DECLARE_DO_FUN(do_book_close);
DECLARE_DO_FUN(do_book_lock);
DECLARE_DO_FUN(do_book_unlock);
DECLARE_DO_FUN(do_book_pick);
DECLARE_DO_FUN(do_book_sign);
DECLARE_DO_FUN(do_book_read);
DECLARE_DO_FUN(do_book_contents);
CHAR_DATA* find_book       args( (CHAR_DATA* ch, char* argument) );
OBJ_DATA* find_book_key    args( (CHAR_DATA* ch, CHAR_DATA* book, OBJ_DATA* book_obj) );
OBJ_DATA* find_book_pen    args( (CHAR_DATA* ch) );
OBJ_DATA* find_book_item   args( (CHAR_DATA* ch, char* argument) );
void water_update args( (CHAR_DATA* ch, bool increase) );
void    disarm          args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );

bool check_room_noescape(CHAR_DATA *ch);  /* used for crusader recall */
void    one_hit         args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt) );

/* handler.c */
void    site_check_from  args( (CHAR_DATA* ch, OBJ_DATA* obj) );

char *    const    dir_name    []        =
{
   "north", "east", "south", "west", "up", "down"
};

const    sh_int    rev_dir        []        =
{
   2, 3, 0, 1, 5, 4
};

const    sh_int    movement_loss    [SECT_MAX]    =
{
   1, 2, 2, 3, 4, 6, 4, 1, 6, 10, 6, 8, 4
};



/*
   Local functions.
*/
int      powtwo                             args((int, int));
bool     is_occupied                        args((int));
int      find_door                          args((CHAR_DATA *ch, char *arg ) );
bool     has_key                            args((CHAR_DATA *ch, int key));
bool     bloody_shrine_entry                args((CHAR_DATA *ch, ROOM_INDEX_DATA *to_room));
void     house_entry_trigger                args((CHAR_DATA *ch, bool update));
void     house_key_trigger                  args((CHAR_DATA *ch, CHAR_DATA *statue, OBJ_DATA *key));
void     murder_entry_trigger               args((CHAR_DATA *ch));
void     move_prog_hydra                    args((CHAR_DATA *ch));
void     move_prog_bones                    args((CHAR_DATA *ch));
void     mob_entry_wraith                   args((CHAR_DATA *ch));
void     mob_entry_evil_area                args((CHAR_DATA *ch));
void     mob_entry_haunted_mine             args((CHAR_DATA *ch));
void     mob_slay                           args((CHAR_DATA *ch, CHAR_DATA *victim));
bool     check_room_protected               args((ROOM_INDEX_DATA *room));
bool     check_guild_entry(CHAR_DATA *ch);
void     mob_entry_zeth_woods               args((CHAR_DATA* ch));
bool     check_marauder_bribe(CHAR_DATA *ch, int guild_type);

bool check_marauder_bribe(CHAR_DATA* ch, int guild_type)
{
   AFFECT_DATA *paf;
   bool can_enter = FALSE;
   if
   (
      is_affected(ch, gsn_marauder_bribe)
   )
   {
      paf = ch->affected;
      while (paf != NULL)
      {
         if
         (
            paf->type == gsn_marauder_bribe
         )
         {
            if
            (
               paf->modifier == guild_type
            )
            {
               can_enter = TRUE;
            }
         }
      paf = paf->next;
      }
   }
   return can_enter;
}


bool check_guild_entry(CHAR_DATA *ch)
{
   if (IS_NPC(ch) || IS_IMMORTAL(ch)) return TRUE;
   if (ch->house == HOUSE_ENFORCER)
   {
      if (check_room_protected(ch->in_room))
      if (ch->pcdata->induct != 0)
      return TRUE;
   }
   return FALSE;
}

bool is_occupied( int vnum)
{
   /* Ofcol */
   if (vnum > 600 && vnum < 699)
   return TRUE;

   /* Arkham
   if (vnum > 700 && vnum < 875)
   return TRUE; */

   /* Ethshar */
   if (vnum > 3000 && vnum < 3199)
   return TRUE;

   /* Elvnehame */
   if (vnum > 8700 && vnum < 8899)
   return TRUE;

   /* New Thalos */
   /* if (vnum > 9500 && vnum < 9799)
   return TRUE; */

   /* Tyr-Zinet */
   if (vnum >= 70000 && vnum <= 70399)
   return TRUE;

   /* Glyndane */
   if (vnum > 10600 && vnum < 10799)
   return TRUE;

   return FALSE;
}

bool check_room_protected (ROOM_INDEX_DATA *room)
{
   int vnum = 0;

   if (room != NULL)
   {
      vnum = room->vnum;
   }

   /* Ofcol */
   if (vnum >= 600 && vnum <= 699)
   return TRUE;

   /* Elvenhame */
   if (vnum >= 8700 && vnum <= 8899)
   return TRUE;

   /* New Thalos */
   /* if (vnum >= 9500 && vnum <= 9799)
   return TRUE; */

   /* Tyr-Zinet */
   if (vnum >= 70000 && vnum <= 70399)
   return TRUE;

   /* Glyndane */
   if (vnum >= 10600 && vnum <= 10799)
   return TRUE;

   /* Daltigoth */
   if ( vnum > 8200 && vnum < 8299)
   return TRUE;

   return FALSE;
   /* All below is not protected */

   /* Ethshar */
   if (vnum >= 3000 && vnum <= 3199)
   return TRUE;

   /* World's Fair */
   if (vnum > 5800 && vnum < 5899)
   return TRUE;



   /* Dhaes Drae Aesilon */
   if (vnum > 27700 && vnum < 27899)
   return TRUE;

   /* Library of the Owl */
   if (vnum > 29000 && vnum < 29099)
   return TRUE;

   return FALSE;
}

void mob_slay( CHAR_DATA *ch, CHAR_DATA *victim )
{
   if ( ch == victim )
   {
      send_to_char( "Suicide is a mortal sin.\n\r", ch );
      return;
   }

   if ( !IS_NPC(victim) && (((victim->level >= get_trust(ch)) &&
   (!IS_NPC(ch))) || (IS_NPC(ch) && IS_IMMORTAL(victim))))
   {
      send_to_char( "You failed.\n\r", ch );
      return;
   }

   act( "You point at $M and $S heart stops!",  ch, NULL, victim, TO_CHAR );
   act( "$n points at you and your heart suddenly stops!", ch, NULL, victim, TO_VICT);
   act( "$n points at $N and $E falls over clenching $S chest!",  ch, NULL, victim, TO_NOTVICT);

   raw_kill(ch, victim );
   return;
}

void do_spirit_move(CHAR_DATA *ch, char * argument)
{
   ROOM_INDEX_DATA *in_room;
   ROOM_INDEX_DATA *to_room;
   ROOM_INDEX_DATA *home;
   CHAR_DATA *vch;
   EXIT_DATA *pexit;
   int door = 0;
   if (IS_NPC(ch))
   return;

   switch(argument[0]){
      case 'n': case 'N': door = DIR_NORTH; break;
      case 's': case 'S': door = DIR_SOUTH; break;
      case 'w': case 'W': door = DIR_WEST; break;
      case 'e': case 'E': door = DIR_EAST; break;
      case 'd': case 'D': door = DIR_DOWN; break;
      case 'u': case 'U': door = DIR_UP; break;
   }
   in_room = ch->pcdata->spirit_room;
   if ( ( pexit   = in_room->exit[door] ) == NULL
   ||   ( to_room = pexit->u1.to_room   ) == NULL
   ||     !can_see_room(ch, pexit->u1.to_room))
   {
      send_to_char( "Alas, you cannot go that way.\n\r", ch );
      return;
   }
   if (IS_SET(pexit->exit_info, EX_CLOSED)
   &&  IS_SET(pexit->exit_info, EX_NOPASS))
   {
      act( "The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR );
      return;
   }
   for (vch = to_room->people; vch != NULL; vch = vch->next_in_room)
   {
      if (IS_SET(vch->act, ACT_HOLYLIGHT))
      {
         act( "$n's spirit has arrived.", ch, NULL, vch, TO_VICT );
      }
   }

   if (number_percent() > get_skill(ch, gsn_spiritwalk))
   {
      send_to_char("You lose concentration and return to your body.\n\r", ch);
      check_improve(ch, gsn_spiritwalk, FALSE, 1);
      do_return(ch, "");
      return;
   }

   home = ch->in_room;
   char_from_room(ch);
   char_to_room(ch, to_room);
   ch->position = POS_STANDING;
   do_observe(ch, "", LOOK_AUTO);
   char_from_room(ch);
   char_to_room(ch, home);
   ch->position = POS_SLEEPING;
   ch->pcdata->spirit_room = to_room;
   return;
}

void move_char(CHAR_DATA* ch, int door, int flags)
{
   CHAR_DATA* fch;
   CHAR_DATA* vch;
   CHAR_DATA* fch_next;
   ROOM_INDEX_DATA* in_room;
   ROOM_INDEX_DATA* to_room;
   EXIT_DATA* pexit;
   CHAR_DATA* tch, *gch;
   CHAR_DATA* tch_next;
   char buf[MAX_STRING_LENGTH];
   int gas_chance;
   /* Added for Covenant's Portal Doorway - Wicket */
   CHAR_DATA* take_to;
   CHAR_DATA* take_next;
   ROOM_INDEX_DATA* from_room;
   OBJ_DATA* obj;
   bool shackles = FALSE;
   bool fsneak = FALSE;
   int house_temp;
   bool gaseous;
   bool feign;
   int simulacrum;
   static int sim_last = -1;
   LIST_DATA* follow_list;
   LIST_DATA* aggro_list;
   NODE_DATA* node;

   if (IS_SET(flags, MOVE_CHAR_SIMULACRUM_ON))
   {
      simulacrum = sim_last;
   }
   else if (!IS_SET(flags, MOVE_CHAR_SIMULACRUM_OFF))
   {
      simulacrum = check_simulacrum(ch);
      if (simulacrum != -1)
      {
         SET_BIT(flags, MOVE_CHAR_SIMULACRUM_ON);
      }
   }
   else
   {
      simulacrum = -1;
   }

   if (is_affected(ch, gsn_trapstun))
   {
      send_to_char("You cannot move, you are in a snare.\n\r", ch);
      return;
   }
   if (is_affected(ch, gsn_freeze))
   {
      send_to_char("You have been trapped in ice and cannot move.\n\r", ch);
      return;
   }
   /* stone */
   if (is_affected(ch, gsn_stone))
   {
      send_to_char
      (
         "You have been turned to stone and are incapable of moving.\n\r",
         ch
      );
      return;
   }
   /* Timestop implementation */
   if (is_affected(ch, gsn_timestop))
   {
      send_to_char("Time has frozen for you; moving is impossible.\n\r", ch);
      return;
   }

   fix_fighting(ch);
   if
   (
      (
         (
            obj = get_eq_char(ch, WEAR_FEET)
         ) != NULL ||
         (
            obj = get_eq_char(ch, WEAR_HOOVES)
         ) != NULL ||
         (
            obj = get_eq_char(ch, WEAR_FOURHOOVES)
         ) != NULL
      ) &&
      obj->pIndexData->vnum == OBJ_VNUM_SHACKLES
   )
   {
      shackles = TRUE;
   }

   if
   (
      is_affected_room
      (
         ch->in_room,
         gsn_corrupt("hall of mirrors", &gsn_hall_mirrors)
      )
   )
   {
      if
      (
         !IS_NPC(ch) &&
         !IS_IMMORTAL(ch) &&
         ch->pcdata->special != SUBCLASS_ILLUSIONIST
      )
      {
         door = (door + ch->in_room->vnum) % 6;
         if (door < 0)
         {
            door = 0;
         }
         in_room = ch->in_room;
         if
         (
            (
               pexit = in_room->exit[door]
            ) == NULL ||
            (
               to_room = pexit->u1.to_room
            ) == NULL ||
            !can_see_room(ch, pexit->u1.to_room)
         )
         {
            do_observe(ch, "", LOOK_AUTO);
            return;
         }

      }
   }
   if
   (
      is_affected( ch, gsn_misdirection) &&
      number_percent() < 50
   )
   {
      send_to_char("You feel dizzy!\n\r", ch);
      door = dice(1, 6) - 1;
   }

   if
   (
      door < 0 ||
      door > 5
   )
   {
      bug("Do_move: bad door %d.", door);
      return;
   }

   /* Exit trigger, if activated, bail out. Only PCs are triggered. */
   if
   (
      !IS_NPC(ch) &&
      mp_exit_trigger(ch, door)
   )
   {
      return;
   }
   in_room = ch->in_room;
   if
   (
      (
         pexit = in_room->exit[door]
      ) == NULL ||
      (
         to_room = pexit->u1.to_room
      ) == NULL ||
      !can_see_room(ch, pexit->u1.to_room)
   )
   {
      if (is_affected(ch, gsn_delusions))
      {
         do_observe(ch, "", LOOK_AUTO);
      }
      else
      {
         send_to_char("Alas, you cannot go that way.\n\r", ch);
      }
      return;
   }

   /* fectus */
   if
   (
      to_room->vnum == 29152 &&
      ch->level < 51
   )
   {
      send_to_char
      (
         "A voice echos in your mind, 'ReTuRn WhEn YoU aRe WoRtH DeVoUrInG!'"
         "\n\r",
         ch
      );
      return;
   }

   /* Special code for the door to Covenant's Portal Entrance */
   if
   (
      !can_exit
      (
         ch,
         pexit
      )
   )
   {
      if
      (
         ch->is_riding &&
         IS_AFFECTED(ch, AFF_PASS_DOOR)
      )
      {
         act
         (
            "Your animal cannot pass through the $d!",
            ch,
            NULL,
            pexit->keyword,
            TO_CHAR
         );
      }
      else
      {
         act
         (
            "The $d is closed.",
            ch,
            NULL,
            pexit->keyword,
            TO_CHAR
         );
      }
      return;
   }

   if
   (
      IS_AFFECTED(ch, AFF_CHARM) &&
      ch->master != NULL &&
      in_room == ch->master->in_room &&
      !IS_NPC(ch)
   )
   {
      send_to_char("What?  And leave your beloved master?\n\r", ch);
      return;
   }

   if
   (
      !is_room_owner(ch, to_room) &&
      room_is_private(to_room)
   )
   {
      send_to_char("That room is private right now.\n\r", ch);
      return;
   }
   if (to_room->guild >= 0)
   {

      if
      (
         to_room->guild != 0 &&
         (
            (to_room->guild - 1) != ch->class ||
            IS_SET(ch->act2, PLR_GUILDLESS)
         ) &&
         !check_guild_entry(ch) &&
         !check_marauder_bribe(ch, to_room->guild)
      )
      {
         if
         (
            !IS_NPC(ch) &&
            !IS_IMMORTAL(ch) &&
            !check_guild_entry(ch) &&
            !check_marauder_bribe(ch, to_room->guild)
         )
         {
            send_to_char("You are not allowed in there.\n\r", ch);
            return;
         }
      }
   }
   else
   {
      if
      (
         !IS_NPC(ch) &&
         (
            to_room->guild !=(-(ch->pcdata->special)) ||
            IS_SET(ch->act2, PLR_GUILDLESS)
         ) &&
         !check_guild_entry(ch) &&
         !check_marauder_bribe(ch, to_room->guild)
      )
      {
         if
         (
            !IS_NPC(ch) &&
            !IS_IMMORTAL(ch) &&
            !check_guild_entry(ch) &&
            !check_marauder_bribe(ch, to_room->guild)
         )
         {
            send_to_char("You are not allowed in there.\n\r", ch);
            return;
         }
      }
   }
   if
   (
      (
         (
            to_room->guild != 0 &&
            ch->in_room->guild != to_room->guild
         ) ||
         (
            IS_SET(to_room->room_flags, ROOM_BLOODY_TIMER) &&
            !IS_SET(ch->in_room->room_flags, ROOM_BLOODY_TIMER)
         )
      ) &&
      (
         ch->pause > 0 ||
         ch->quittime > 13
      )
   )
   {
      send_to_char("You feel too bloody to go in there.\n\r", ch);
      return;
   }

   if (!IS_NPC(ch))
   {
      int iClass = 0;
      int iGuild;
      int move;

      if
      (
         !IS_IMMORTAL(ch) &&
         !check_guild_entry(ch) &&
         !check_marauder_bribe(ch, to_room->guild)
      )
      {
         for (iClass = 0; iClass < MAX_CLASS; iClass++)
         {
            for (iGuild = 0; iGuild < MAX_GUILD; iGuild ++)
            {
               if
               (
                  (
                     iClass != ch->class &&
                     to_room->vnum == class_table[iClass].guild[iGuild]
                  ) ||
                  (
                     IS_SET(ch->act2, PLR_GUILDLESS) &&
                     to_room->guild != 0
                  )
               )
               {
                  send_to_char("You aren't allowed in there.\n\r", ch);
                  return;
               }
            }
         }
      }

      for (iGuild = 0; iGuild < MAX_GUILD; iGuild ++)
      {
         if
         (
            (
               ch->pause > 0 ||
               ch->quittime > 13
            ) &&
            to_room->vnum == class_table[iClass].guild[iGuild]
         )
         {
            send_to_char("You feel too bloody to go in there.\n\r", ch);
            return;
         }
      }

      if
      (
         (
            in_room->sector_type == SECT_UNDERWATER ||
            to_room->sector_type == SECT_UNDERWATER
         ) &&
         !IS_IMMORTAL(ch) &&
         is_affected(ch, gsn_rip_tide)
      )
      {
         send_to_char("You have not yet uprighted yourself.\n\r", ch);
         return;
      }

      if
      (
         in_room->sector_type == SECT_AIR ||
         to_room->sector_type == SECT_AIR
      )
      {
         if (ch->is_riding)
         {
            switch (ch->is_riding->mount_type)
            {
               case (MOUNT_MOBILE):
               {
                  if (!IS_FLYING((CHAR_DATA*)ch->is_riding->mount))
                  {
                     send_to_char("Your mount cannot fly.\n\r", ch);
                     return;
                  }
                  break;
               }
               case (MOUNT_OBJECT):
               {
                  if
                  (
                     !IS_SET
                     (
                        (
                           (OBJ_DATA*)ch->is_riding->mount
                        )->mount_specs->move_flags,
                        MOUNT_AIR
                     )
                  )
                  {
                     send_to_char("Your mount cannot fly.\n\r", ch);
                     return;
                  }
                  break;
               }
            }
         }
         else if
         (
            !IS_FLYING(ch) &&
            !IS_IMMORTAL(ch)
         )
         {
            send_to_char("You cannot fly.\n\r", ch);
            return;
         }
      }

      if
      (
         in_room->sector_type == SECT_WATER_NOSWIM ||
         to_room->sector_type == SECT_WATER_NOSWIM
      )
      {
         OBJ_DATA* obj;
         bool found;

         if (!ch->is_riding)
         {
            if
            (
               !IS_FLYING(ch) &&
               !IS_IMMORTAL(ch)
            )
            {
               /*
                  Look for a boat.
               */
               found = FALSE;

               for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
               {
                  if (obj->item_type == ITEM_BOAT)
                  {
                     found = TRUE;
                     break;
                  }
               }

               if
               (
                  !found &&
                  !IS_AFFECTED(ch, AFF_SWIM)
               )
               {
                  send_to_char("You need a boat to go there.\n\r", ch);
                  return;
               }
            }
         }
         else
         {
            switch (ch->is_riding->mount_type)
            {
               case (MOUNT_MOBILE):
               {
                  if (!IS_FLYING((CHAR_DATA*)ch->is_riding->mount))
                  {
                     send_to_char("Your mount cannot cross water.\n\r", ch);
                     return;
                  }
                  break;
               }
               case (MOUNT_OBJECT):
               {
                  if
                  (
                     !IS_SET
                     (
                        (
                           (OBJ_DATA*)ch->is_riding->mount
                        )->mount_specs->move_flags,
                        MOUNT_AIR
                     ) &&
                     !IS_SET
                     (
                        (
                           (OBJ_DATA*)ch->is_riding->mount
                        )->mount_specs->move_flags,
                        MOUNT_ON_WATER
                     )
                  )
                  {
                     send_to_char("Your mount cannot cross water.\n\r", ch);
                     return;
                  }
                  break;
               }
            }
         }
      }

      move =
      (
         movement_loss[UMIN(SECT_MAX - 1, in_room->sector_type)] +
         movement_loss[UMIN(SECT_MAX - 1, to_room->sector_type)]
      );
      move = (move * 2) / 3;

      if
      (
         (
            ch->in_room->sector_type == SECT_FOREST ||
            ch->in_room->sector_type == SECT_UNDERGROUND ||
            ch->in_room->sector_type == SECT_MOUNTAIN ||
            ch->in_room->sector_type == SECT_HILLS
         ) &&
         has_skill(ch, gsn_forestwalk)
      )
      {
         if (number_percent() < get_skill(ch, gsn_forestwalk))
         {
            move = move/4;
            check_improve(ch, gsn_forestwalk, TRUE, 1);
         }
         else
         {
            check_improve(ch, gsn_forestwalk, FALSE, 1);
         }
      }
      /* conditional effects */
      if
      (
         !shackles &&
         (
            IS_FLYING(ch) ||
            IS_AFFECTED(ch, AFF_HASTE)
         )
      )
      {
         move = move * 2 / 3;
      }

      if (IS_AFFECTED(ch, AFF_SLOW))
      {
         move = move * 3 / 2;
      }

      if (is_affected(ch, gsn_cripple))
      {
         AFFECT_DATA* paf;

         paf = affect_find(ch->affected, gsn_cripple);
         if (paf->location == APPLY_DEX)
         {
            move *= 4;
         }
      }

      if (shackles)
      {
         move *= 2;
      }

      if (is_affected(ch, gsn_high_herb))
      {
         move += 1;
      }

      /* riding stuff -werv */
      if (ch->is_riding)
      {
         switch (ch->is_riding->mount_type)
         {
            case (MOUNT_MOBILE):
            {
               if
               (
                  (
                     (CHAR_DATA*)ch->is_riding->mount
                  )->move < move
               )
               {
                  send_to_char("Your animal is exhausted.\n\r", ch);
                  return;
               }
               else
               {
                  ((CHAR_DATA *)ch->is_riding->mount)->move -= move;
               }
               if
               (
                  (
                     (CHAR_DATA*)ch->is_riding->mount
                  )->in_room != ch->in_room
               )
               {
                  do_dismount(ch, NULL);
               }
               break;
            }
            case (MOUNT_OBJECT):
            {
               if
               (
                  (
                     (OBJ_DATA*)ch->is_riding->mount
                  )->mount_specs->move < move
               )
               {
                  send_to_char("Your mount is exhausted.\n\r", ch);
                  return;
               }
               else
               {
                  (
                     (OBJ_DATA*)ch->is_riding->mount
                  )->mount_specs->move -= move;
               }
               if
               (
                  (
                     (OBJ_DATA*)ch->is_riding->mount
                  )->in_room != ch->in_room
               )
               {
                  do_dismount(ch, NULL);
               }
               break;
            }
            default:
            {
               if (ch->move < move)
               {
                  send_to_char("You are too exhausted.\n\r", ch);
                  return;
               }
               if
               (
                  IS_NPC(ch) ||
                  ch->pcdata->death_status != HAS_DIED
               )
               {
                  ch->move -= move;
               }
               break;
            }
         }
      }
      else
      {
         if (ch->move < move)
         {
            send_to_char("You are too exhausted.\n\r", ch);
            return;
         }
         if
         (
            IS_NPC(ch) ||
            ch->pcdata->death_status != HAS_DIED
         )
         {
            ch->move -= move;
         }
      }
      WAIT_STATE(ch, 1);
   }

   if (is_affected(ch, gsn_shroud))
   {
      un_shroud(ch, NULL);
   }
   if (is_affected(ch, gsn_forest_blending))
   {
      un_forest_blend(ch);
   }

   if ( IS_AFFECTED(ch, AFF_CAMOUFLAGE) )
   {
      if
      (
         !has_skill(ch, gsn_forestwalk) ||
         number_percent() > get_skill(ch, gsn_forestwalk) ||
         (
            to_room->sector_type != SECT_FOREST &&
            to_room->sector_type != SECT_MOUNTAIN &&
            to_room->sector_type != SECT_HILLS &&
            to_room->sector_type != SECT_UNDERGROUND
         )
      )
      {
         un_camouflage(ch, NULL);
      }
   }
   if (!is_affected(ch, gsn_burrow))
   {
      un_earthfade(ch, NULL);
   }
   else
   {
      if
      (
         to_room->sector_type == SECT_WATER_SWIM ||
         to_room->sector_type == SECT_UNDERWATER ||
         to_room->sector_type == SECT_WATER_NOSWIM ||
         to_room->sector_type == SECT_AIR
      )
      {
         send_to_char
         (
            "There is not enough natural earth to meld with here.\n\r",
            ch
         );
         un_earthfade(ch, NULL);
      }
      else if (!IS_SET(flags, MOVE_CHAR_SILENT))
      {
         ROOM_INDEX_DATA* tmp;
         tmp = ch->in_room;
         char_from_room(ch);
         char_to_room_1(ch, in_room, TO_ROOM_AT);
         act
         (
            "A small tremble is felt from the dirt below.",
            ch,
            NULL,
            NULL,
            TO_ROOM
         );
         char_from_room(ch);
         char_to_room_1(ch, to_room, TO_ROOM_AT);
         act
         (
            "A small tremble is felt from the dirt below.",
            ch,
            NULL,
            NULL,
            TO_ROOM
         );
         send_to_char("You burrow through the earth.\n\r", ch);
         char_from_room(ch);
         char_to_room_1(ch, tmp, TO_ROOM_AT);
      }
      affect_strip(ch, gsn_burrow);
   }
   if
   (
      IS_AFFECTED(ch, AFF_HIDE) &&
      to_room->sector_type != SECT_UNDERGROUND &&
      to_room->sector_type != SECT_CITY &&
      to_room->sector_type != SECT_INSIDE
   )
   {
      un_hide(ch, NULL);
   }

   fsneak = FALSE;
   if
   (
      (
         ch->in_room->sector_type == SECT_FOREST ||
         ch->in_room->sector_type == SECT_MOUNTAIN ||
         ch->in_room->sector_type == SECT_HILLS
      ) &&
      has_skill(ch, gsn_forestwalk) &&
      number_percent() < get_skill(ch, gsn_forestwalk)
   )
   {
      fsneak = TRUE;
   }
   if
   (
      IS_AFFECTED(ch, AFF_SNEAK) ||
      fsneak ||
      simulacrum != -1
   )
   {
      for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
      {
         if
         (
            ch == vch ||
            (
               !can_see(vch, ch) &&
               (
                  ch->invis_level > get_trust(vch) ||
                  ch->nosee == vch
               )
            ) ||
            (
               ch != NULL &&
               vch != NULL &&
               !IS_NPC(ch) &&
               !IS_NPC(vch) &&
               IS_IMMORTAL(ch) &&
               vch->pcdata->nosee_perm != NULL &&
               vch->pcdata->nosee_perm[0] != '\0' &&
               is_name(ch->name, vch->pcdata->nosee_perm)
            )
         )
         {
            continue;
         }
         if (IS_SET(vch->act, ACT_HOLYLIGHT))
         {
            if (is_affected(ch, gsn_door_bash))
            {
               act
               (
                  "$n goes crashing through the door $t.",
                  ch,
                  dir_name[door],
                  vch,
                  TO_VICT
               );
            }
            else
            {
               if
               (
                  !IS_SET(flags, MOVE_CHAR_SILENT) ||
                  IS_IMMORTAL(vch)
               )
               {
                  act("$n leaves $t.", ch, dir_name[door], vch, TO_VICT);
               }
            }
         }
      }
   }
   else
   {
      un_hide(ch, NULL);
      for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
      {
         if
         (
            ch == vch ||
            (
               !can_see(vch, ch) &&
               (
                  ch->invis_level > get_trust(vch) ||
                  ch->nosee == vch
               )
            ) ||
            (
               ch != NULL &&
               vch != NULL &&
               !IS_NPC(ch) &&
               !IS_NPC(vch) &&
               IS_IMMORTAL(ch) &&
               vch->pcdata->nosee_perm != NULL &&
               vch->pcdata->nosee_perm[0] != '\0' &&
               is_name(ch->name, vch->pcdata->nosee_perm)
            )
         )
         {
            continue;
         }
         if (is_affected(ch, gsn_door_bash))
         {
            act
            (
               "$n goes crashing through the door $t.",
               ch,
               dir_name[door],
               vch,
               TO_VICT
            );
         }
         else
         {
            if
            (
               !IS_SET(flags, MOVE_CHAR_SILENT) ||
               IS_IMMORTAL(vch)
            )
            {
               act("$n leaves $t.", ch, dir_name[door], vch, TO_VICT);
            }
         }
      }
   }
   /* tracking stuff */

   if (!IS_NPC(ch))
   {
      new_track(ch, door + 1);
   }
   if
   (
      ch->in_room != NULL &&
      ch->in_room->vnum == 13300 &&
      !IS_IMMORTAL(ch)
   )
   {
      act
      (
         "$n steps through the flames at the center of the Runed Circle.",
         ch,
         NULL,
         NULL,
         TO_ROOM
      );
      send_to_char
      (
         "You step through the flames at the center of the Runed Circle.\n\r",
         ch
      );
      char_from_room(ch);
      char_to_room_1(ch, to_room, TO_ROOM_MOVE_CHAR);
      act
      (
         "$n falls from a flaming portal above the ground.\n\r"
         "$n gets up and looks around, face mirroring surprise.",
         ch,
         NULL,
         NULL,
         TO_ROOM
      );
      send_to_char
      (
         "You fall for what seems an eternity until you land on something hard."
         "\n\r"
         "You get up and look around.  You are surrounded by a dark mist.\n\r",
         ch
      );
      do_observe(ch, "", LOOK_AUTO);
   }
   else if
   (
      ch->in_room != NULL &&
      ch->in_room->vnum == 1511 &&
      !IS_IMMORTAL(ch) &&
      to_room->vnum == 1575 &&
      ch->house == HOUSE_EMPIRE
   )
   {
      /* Emotes for Covenant Portal Entrance door */
      act
      (
         "The doorway opens swiftly as $n steps inside and vanishes into its"
         " depths.",
         ch,
         NULL,
         NULL,
         TO_ROOM
      );
      send_to_char
      (
         "The doorway opens swiftly as you step inside and vanish into its"
         " depths.\n\r",
         ch
      );
      from_room = ch->in_room;
      for (take_to = from_room->people; take_to != NULL; take_to = take_next)
      {
         take_next = take_to->next_in_room;
         if
         (
            is_same_group(take_to, ch) &&
            IS_NPC(take_to) &&
            take_to->master == ch
         )
         {
            char_from_room(take_to);
            char_to_room_1(take_to, to_room, TO_ROOM_MOVE_CHAR);
         }
      }

      char_from_room(ch);
      char_to_room_1(ch, to_room, TO_ROOM_MOVE_CHAR | TO_ROOM_LOOK);
      act
      (
         "A Chaplain of the Covenant can be seen briefly behind $n as $e steps into"
         " the portal.",
         ch,
         NULL,
         NULL,
         TO_ROOM
      );
   }
   else
   {
      char_from_room(ch);
      char_to_room_1(ch, to_room, TO_ROOM_MOVE_CHAR | TO_ROOM_LOOK);
      if
      (
         ch->is_riding &&
         ch->is_riding->mount_type == MOUNT_OBJECT
      )
      {
         obj_from_room((OBJ_DATA *)ch->is_riding->mount);
         obj_to_room((OBJ_DATA *)ch->is_riding->mount, ch->in_room);
      }
   }


   fsneak = FALSE;
   if
   (
      (
         ch->in_room->sector_type == SECT_FOREST ||
         ch->in_room->sector_type == SECT_MOUNTAIN ||
         ch->in_room->sector_type == SECT_HILLS
      ) &&
      has_skill(ch, gsn_forestwalk) &&
      number_percent() < get_skill(ch, gsn_forestwalk)
   )
   {
      fsneak = TRUE;
   }

   if
   (
      IS_AFFECTED(ch, AFF_SNEAK) ||
      fsneak ||
      simulacrum != -1
   )
   {
      for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
      {
         if
         (
            ch == vch ||
            (
               !can_see(vch, ch) &&
               (
                  ch->invis_level > get_trust(vch) ||
                  ch->nosee == vch
               )
            ) ||
            (
               ch != NULL &&
               vch != NULL &&
               !IS_NPC(ch) &&
               !IS_NPC(vch) &&
               IS_IMMORTAL(ch) &&
               vch->pcdata->nosee_perm != NULL &&
               vch->pcdata->nosee_perm[0] != '\0' &&
               is_name(ch->name, vch->pcdata->nosee_perm)
            )
         )
         {
            continue;
         }
         if (IS_SET(vch->act, ACT_HOLYLIGHT))
         {
            if (is_affected(ch, gsn_door_bash))
            {
               act
               (
                  "The $t door bursts open and $n comes crashing in!",
                  ch,
                  dir_name[door],
                  vch,
                  TO_VICT
               );
            }
            else
            {
               act("$n has arrived.", ch, NULL, vch, TO_VICT);
            }
         }
      }
   }
   else
   {
      for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
      {
         if
         (
            ch == vch ||
            (
               !can_see(vch, ch) &&
               (
                  ch->invis_level > get_trust(vch) ||
                  ch->nosee == vch
               )
            ) ||
            (
               ch != NULL &&
               vch != NULL &&
               !IS_NPC(ch) &&
               !IS_NPC(vch) &&
               IS_IMMORTAL(ch) &&
               vch->pcdata->nosee_perm != NULL &&
               vch->pcdata->nosee_perm[0] != '\0' &&
               is_name(ch->name, vch->pcdata->nosee_perm)
            )
         )
         {
            continue;
         }
         if (is_affected(ch, gsn_door_bash))
         {
            act
            (
               "The $t door bursts open and $n comes crashing in!",
               ch,
               dir_name[door],
               vch,
               TO_VICT
            );
         }
         else
         {
            act("$n has arrived.", ch, NULL, vch, TO_VICT);
         }
      }
   }

   if (in_room == to_room)  /* no circular follows */
   {
      return;
   }
   if
   (
      simulacrum != -1 &&
      to_room->sector_type != simulacrum
   )
   {
      if (!IS_SET(flags, MOVE_CHAR_SIMULACRUM_OFF))
      {
         strip_simulacrum(ch, NULL, TRUE);
      }
      else
      {
         simulacrum_off(ch);
         affect_strip(ch, gsn_simulacrum);
      }
      SET_BIT(flags, MOVE_CHAR_SIMULACRUM_OFF);
   }
   follow_list = NULL;
   for (fch = in_room->people; fch; fch = fch->next_in_room)
   {
      if
      (
         fch == ch ||
         fch->master != ch
      )
      {
         continue;
      }
      if (!follow_list)
      {
         follow_list = new_list();
      }
      add_node(fch, follow_list);
   }
   if (follow_list)  /* Find if anyone follows before searching entire list */
   {
      for (node = follow_list->first; node; node = node->next)
      {
         fch = (CHAR_DATA*)node->data;
         if
         (
            !check_room(fch, TRUE, "Purge in move_char, follow") ||
            fch->master != ch ||
            fch->in_room != in_room
         )
         {
            continue;
         }
         if
         (
            IS_AFFECTED(fch, AFF_CHARM) &&
            fch->position < POS_STANDING &&
            fch->daze <= 0 &&
            fch->wait <= 0
         )
         {
            do_stand(fch, "");
         }
         if
         (
            is_affected(ch, gsn_blitz) &&
            !IS_NPC(fch)
         )
         {
            send_to_char("You cannot keep up!\n\r", fch);
            continue;
         }
         if
         (
            fch->position == POS_STANDING &&
            can_see_room(fch, to_room)
         )
         {
            if
            (
               IS_SET(ch->in_room->room_flags, ROOM_LAW) &&
               (
                  IS_NPC(fch) &&
                  IS_SET(fch->act, ACT_AGGRESSIVE)
               )
            )
            {
               act
               (
                  "You cannot bring $N into the city.",
                  ch,
                  NULL,
                  fch,
                  TO_CHAR
               );
               act
               (
                  "You are not allowed in the city.",
                  fch,
                  NULL,
                  NULL,
                  TO_CHAR
               );
               continue;
            }
            act("You follow $N.", fch, NULL, ch, TO_CHAR);
            if (is_same_group(ch, fch))
            {
               sim_last = simulacrum;
               move_char(fch, door, flags | MOVE_CHAR_FOLLOW);
            }
            else
            {
               move_char
               (
                  fch,
                  door,
                  (
                     (
                        flags |
                        MOVE_CHAR_FOLLOW
                     ) &
                     ~MOVE_CHAR_SIMULACRUM_OFF &
                     ~MOVE_CHAR_SIMULACRUM_ON
                  )
              );
            }
         }
      }
      free_list(follow_list);
      follow_list = NULL;
   }

   if (!IS_NPC(ch))
   {
      delete_track(ch);
   }
   if (!IS_NPC(ch))
   {
      gaseous = is_affected(ch, gsn_gaseous_form);
      feign = is_affected(ch, gsn_feign_death);

      aggro_list = NULL;
      for (tch = ch->in_room->people; tch != NULL; tch = tch_next)
      {
         tch_next = tch->next_in_room;
         if
         (
            !IS_NPC(tch) ||
            ch->in_room != tch->in_room ||
            tch->last_fought != ch->id ||
            tch->fighting != NULL ||
            tch->position == POS_FIGHTING ||
            !can_see(tch, ch) ||
            IS_AFFECTED(ch, AFF_CALM)
         )
         {
            continue;
         }
         if (!aggro_list)
         {
            aggro_list = new_list();
         }
         add_node(tch, aggro_list);
      }
      /*
         Only check entire list if
         there is someone in the room
         that wants to attack
      */
      if (aggro_list)  /* Find if anyone follows before searching entire list */
      {
         for (node = aggro_list->first; node; node = node->next)
         {
            tch = (CHAR_DATA*)node->data;
            if
            (
               !IS_NPC(tch) ||
               ch->in_room != tch->in_room ||
               tch->last_fought != ch->id ||
               tch->fighting != NULL ||
               tch->position == POS_FIGHTING ||
               !can_see(tch, ch) ||
               IS_AFFECTED(ch, AFF_CALM)
            )
            {
               continue;
            }
            if (gaseous)
            {
               gas_chance = get_skill(ch, gsn_gaseous_form) / 2;
               gas_chance += (ch->level - tch->level) * 2;
               if (gas_chance > 75)
               {
                  gas_chance = 75;
               }
               if (number_percent() < gas_chance)
               {
                  continue;
               }
               act
               (
                  "$N manages to intercept you and forces you out of gaseous form.",
                  ch,
                  NULL,
                  tch,
                  TO_CHAR
               );
               un_gaseous(ch);
               gaseous = FALSE;
            }
            if (feign)
            {
               if
               (
                  (
                     get_skill(ch, gsn_feign_death) / 3 +
                     ch->level -
                     tch->level
                  ) >=
                  number_percent()
               )
               {
                  continue;
               }
               act
               (
                  "$N manages to see through your feint of death!",
                  ch,
                  NULL,
                  tch,
                  TO_CHAR
               );
               affect_strip(ch, gsn_feign_death);
               feign = FALSE;
            }

            sprintf(buf, "%s, now you die!", PERS(ch, tch));
            do_yell(tch, buf);
            if
            (
               IS_SET(tch->act2, ACT_RANGER) &&
               IS_AFFECTED(tch, AFF_CAMOUFLAGE) &&
               !can_see(ch, tch)
            )
            {
               sprintf(buf, "\"%s\"", get_name(ch, tch));
               do_ambush(tch, buf);
            }
            else
            {
               multi_hit(tch, ch, TYPE_UNDEFINED);
            }
         }
         free_list(aggro_list);
         aggro_list = NULL;
      }
   }
   /*
      If someone is following the char, these triggers get activated
      for the followers before the char, but it's safer this way...
   */
   if (!IS_SET(flags, MOVE_CHAR_FOLLOW))
   {
      /* Runs ONLY for the leader, and hits everyone */
      murder_entry_trigger(ch);
   }
   if (!IS_NPC(ch))
   {
      mob_entry_wraith(ch);
      mob_entry_evil_area(ch);
      mob_entry_zeth_woods(ch);
      mob_entry_haunted_mine(ch);
   }
   /*
      ch == NULL check does nothing, this is C,
      we cannot pass pointers by reference.
      However, if we set ch = NULL somewhere in this function
      sometime in the future, this could be useful. - Fizz
   */
   if (ch == NULL)
   {
      return;
   }
   /*
      code to catch weird crashes - wervdon
      Note: If char_list is NULL, we are going to crash
      very quickly very soon anyway - Fizz
   */
   if (char_list == NULL)
   {
      sprintf
      (
         log_buf,
         "BUG: NULL char_list in act_move. char = %s",
         ch->name
      );
      log_string(log_buf);
      return;
   }
   /* spy and spores code */
   if (!IS_NPC(ch))
   {
      bool detain;

      if
      (
         ch->in_room->house == 0 &&
         (
            IS_SET(ch->act, PLR_CRIMINAL) ||
            IS_SET(ch->act2, PLR_LAWLESS)
         ) &&
         !IS_IMMORTAL(ch)
      )
      {
         detain = TRUE;
      }
      else
      {
         detain = FALSE;
      }
      for (gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room)
      {
         if (gch->position <= POS_SLEEPING)
         {
            continue;
         }
         if
         (
            IS_NPC(gch) &&
            gch->spyfor != NULL &&
            ch != gch->spyfor &&
            (
               !IS_IMMORTAL(ch) ||
               get_trust(gch) >= get_trust(ch)
            )
         )
         {
            sprintf
            (
               buf,
               "Your lookout informs you that %s has entered %s.\n\r",
               PERS(ch, gch),
               ch->in_room->name
            );
            send_to_char(buf, gch->spyfor);
         }
         if
         (
            IS_NPC(gch) &&
            gch->pIndexData->vnum == MOB_VNUM_SPORE
         )
         {
            for (tch = ch->in_room->people; tch != NULL; tch = tch->next_in_room)
            {
               if
               (
                  !IS_IMMORTAL(tch) &&
                  (tch->level - gch->level) < 6 &&
                  (gch->level - tch->level) < 6
               )
               {
                  magic_spell_vict
                  (
                     gch,
                     tch,
                     CAST_BITS_MOB_SKILL,
                     gch->level,
                     gsn_plague
                  );
               }
            }
         }
         else if
         (
            detain &&
            guard_type(gch) == GUARD_SENTRY &&
            number_percent() <
            (
               51 +
               (
                  gch->level < ch->level ?
                  (gch->level - ch->level) * 5 :
                  0
               )
            )
         )
         {
            detain = FALSE;
            act
            (
               "$N moves to detain $n.",
               ch,
               NULL,
               gch,
               TO_ROOM
            );
            act
            (
               "$N blocks your path, slowing you down.",
               ch,
               NULL,
               gch,
               TO_CHAR
            );
            if (gch->mprog_target != NULL)
            {
               house_temp = gch->house;
               gch->house = HOUSE_ENFORCER;
               sprintf
               (
                  buf,
                  "%s, I have detained %s.  Hurry, for I can not hold %s for long.",
                  gch->mprog_target->name,
                  PERS(ch, gch),
                  (
                     ch->sex == SEX_FEMALE ?
                     "her" :
                     (
                        ch->sex == SEX_MALE ?
                        "him" :
                        "it"
                     )
                  )
               );
               do_housetalk(gch, buf);
               gch->house = house_temp;
            }
            DAZE_STATE(ch, 35);
         }
      }
   }
   /* alarms */
   if (!IS_NPC(ch))
   {
      ALARM_DATA* alarm;
      ALARM_DATA* alarm_next;
      ALARM_DATA* before;
      CHAR_DATA* tch;
      int trust = get_trust(ch);
      bool not_imm = !IS_IMMORTAL(ch);

      for (alarm = ch->in_room->alarm; alarm; alarm = alarm_next)
      {
         alarm_next = alarm->next;
         tch = alarm->caster;
         if
         (
            tch->on_line &&
            tch->alarm == alarm &&
            (
               not_imm ||
               get_trust(tch) >= trust
            )
         )
         {
            act_color
            (
               "{B{3BRRRINNGG!{n\n\r"
               "$N has entered $t.",
               tch,
               ch->in_room->name,
               ch,
               TO_CHAR
            );
            if (tch->alarm->room->alarm == tch->alarm)
            {
               tch->alarm->room->alarm = tch->alarm->room->alarm->next;
               free_alarm(tch->alarm);
            }
            else
            {
               before = tch->alarm->room->alarm;
               while
               (
                  before != NULL &&
                  before->next &&
                  before->next != tch->alarm
               )
               {
                  before = before->next;
               }
               if
               (
                  before &&
                  before->next == tch->alarm
               )
               {
                  before->next = before->next->next;
                  free_alarm(tch->alarm);
               }
            }
            tch->alarm = NULL;
         }
      }
   }

   if
   (
      IS_NPC(ch) &&
      HAS_TRIGGER(ch, TRIG_ENTRY) &&
      ch->last_fought <= 0
   )
   {
      mp_percent_trigger(ch, NULL, NULL, NULL, TRIG_ENTRY);
   }
   if
   (
      is_affected(ch, gsn_gaseous_form) &&
      number_percent() < 70
   )
   {
      return;
   }
   if (!IS_NPC(ch))
   {
      mp_greet_trigger(ch);
      move_prog_bones(ch);
      move_prog_hydra(ch);
   }

   if (ch->in_room)
   {
      for (fch = ch->in_room->people; fch; fch = fch_next)
      {
         fch_next = fch->next_in_room;
         if
         (
            (ch->in_room) &&
            fch->in_room == ch->in_room
         )
         {
            check_bloodlust(fch);
         }
      }
   }
   if (in_room->sector_type == SECT_UNDERWATER)
   {
      water_update(ch, FALSE);
   }
   return;
}



void do_north(CHAR_DATA* ch, char* argument)
{
   move_char(ch, DIR_NORTH, 0);
   return;
}

void do_east(CHAR_DATA* ch, char* argument)
{
   move_char(ch, DIR_EAST, 0);
   return;
}

void do_south(CHAR_DATA* ch, char* argument)
{
   move_char(ch, DIR_SOUTH, 0);
   return;
}

void do_west(CHAR_DATA* ch, char* argument)
{
   move_char(ch, DIR_WEST, 0);
   return;
}

void do_up(CHAR_DATA* ch, char* argument)
{
   move_char(ch, DIR_UP, 0);
   return;
}

void do_down(CHAR_DATA* ch, char* argument)
{
   move_char(ch, DIR_DOWN, 0);
   return;
}



int find_door( CHAR_DATA *ch, char *arg )
{
   EXIT_DATA *pexit;
   int door;

   if ( !str_cmp( arg, "n" ) || !str_cmp( arg, "north" ) ) door = 0;
   else if ( !str_cmp( arg, "e" ) || !str_cmp( arg, "east"  ) ) door = 1;
   else if ( !str_cmp( arg, "s" ) || !str_cmp( arg, "south" ) ) door = 2;
   else if ( !str_cmp( arg, "w" ) || !str_cmp( arg, "west"  ) ) door = 3;
   else if ( !str_cmp( arg, "u" ) || !str_cmp( arg, "up"    ) ) door = 4;
   else if ( !str_cmp( arg, "d" ) || !str_cmp( arg, "down"  ) ) door = 5;
   else
   {
      for ( door = 0; door <= 5; door++ )
      {
         if ( ( pexit = ch->in_room->exit[door] ) != NULL
         &&   IS_SET(pexit->exit_info, EX_ISDOOR)
         &&   pexit->keyword != NULL
         &&   is_name( arg, pexit->keyword ) )
         return door;
      }
      act( "I see no $T here.", ch, NULL, arg, TO_CHAR );
      return -1;
   }

   if ( ( pexit = ch->in_room->exit[door] ) == NULL )
   {
      act( "I see no door $T here.", ch, NULL, arg, TO_CHAR );
      return -1;
   }

   if ( !IS_SET(pexit->exit_info, EX_ISDOOR) )
   {
      send_to_char( "You can't do that.\n\r", ch );
      return -1;
   }

   return door;
}



void do_open( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   OBJ_DATA *obj;
   int door;
   char* orig_argument = argument;

   argument = one_argument( argument, arg );

   if
   (
      !str_cmp(arg, "Fizzfaldt") ||
      !str_cmp(arg, "Buk") ||
      (
         !str_cmp(arg, "self") &&
         ch->race == grn_book &&
         !IS_NPC(ch)
      )
   )
   {
      do_book_open(ch, orig_argument);
      return;
   }

   if ( arg[0] == '\0' )
   {
      send_to_char( "Open what?\n\r", ch );
      return;
   }


   if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
   {
      if (obj->item_type == ITEM_BOOK)
      {
         do_book_open(ch, orig_argument);
         return;
      }
      /* 'open object' */
      if ( obj->item_type != ITEM_CONTAINER)
      { send_to_char( "That's not a container.\n\r", ch ); return; }
      if ( !IS_SET(obj->value[1], CONT_CLOSED) )
      { send_to_char( "It's already open.\n\r",      ch ); return; }
      if ( !IS_SET(obj->value[1], CONT_CLOSEABLE) )
      { send_to_char( "You can't do that.\n\r",      ch ); return; }
      if ( IS_SET(obj->value[1], CONT_LOCKED) )
      { send_to_char( "It's locked.\n\r",            ch ); return; }

      REMOVE_BIT(obj->value[1], CONT_CLOSED);
      act("You open $p.", ch, obj, NULL, TO_CHAR);
      act( "$n opens $p.", ch, obj, NULL, TO_ROOM );
      return;
   }
   if (find_book(ch, arg) != NULL)
   {
      do_book_open(ch, orig_argument);
      return;
   }


   if ( ( door = find_door( ch, arg ) ) >= 0 )
   {
      /* 'open door' */
      ROOM_INDEX_DATA *to_room;
      EXIT_DATA *pexit;
      EXIT_DATA *pexit_rev;

      pexit = ch->in_room->exit[door];
      if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
      { send_to_char( "It's already open.\n\r",      ch ); return; }
      if (  IS_SET(pexit->exit_info, EX_LOCKED) )
      { send_to_char( "It's locked.\n\r",            ch ); return; }

      REMOVE_BIT(pexit->exit_info, EX_CLOSED);
      act( "$n opens the $d.", ch, NULL, pexit->keyword, TO_ROOM );
      send_to_char( "Ok.\n\r", ch );

      /* open the other side */
      if ( ( to_room   = pexit->u1.to_room            ) != NULL
      &&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
      &&   pexit_rev->u1.to_room == ch->in_room )
      {
         CHAR_DATA *rch;

         REMOVE_BIT( pexit_rev->exit_info, EX_CLOSED );
         for ( rch = to_room->people; rch != NULL; rch = rch->next_in_room)
         act( "The $d opens.", rch, NULL, pexit_rev->keyword, TO_CHAR );
      }
      return;
   }

   return;
}



void do_close( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   OBJ_DATA *obj;
   int door;
   char* orig_argument = argument;

   argument = one_argument( argument, arg );

   if
   (
      !str_cmp(arg, "Fizzfaldt") ||
      !str_cmp(arg, "Buk") ||
      (
         !str_cmp(arg, "self") &&
         ch->race == grn_book &&
         !IS_NPC(ch)
      )
   )
   {
      do_book_close(ch, orig_argument);
      return;
   }

   if ( arg[0] == '\0' )
   {
      send_to_char( "Close what?\n\r", ch );
      return;
   }


   if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
   {
      if (obj->item_type == ITEM_BOOK)
      {
         do_book_close(ch, orig_argument);
         return;
      }

      /* 'close object' */
      if ( obj->item_type != ITEM_CONTAINER )
      { send_to_char( "That's not a container.\n\r", ch ); return; }
      if ( IS_SET(obj->value[1], CONT_CLOSED) )
      { send_to_char( "It's already closed.\n\r",    ch ); return; }
      if ( !IS_SET(obj->value[1], CONT_CLOSEABLE) )
      { send_to_char( "You can't do that.\n\r",      ch ); return; }

      SET_BIT(obj->value[1], CONT_CLOSED);
      act("You close $p.", ch, obj, NULL, TO_CHAR);
      act( "$n closes $p.", ch, obj, NULL, TO_ROOM );
      return;
   }
   if (find_book(ch, arg) != NULL)
   {
      do_book_close(ch, orig_argument);
      return;
   }

   if ( ( door = find_door( ch, arg ) ) >= 0 )
   {
      /* 'close door' */
      ROOM_INDEX_DATA *to_room;
      EXIT_DATA *pexit;
      EXIT_DATA *pexit_rev;

      pexit    = ch->in_room->exit[door];
      if ( IS_SET(pexit->exit_info, EX_CLOSED) )
      { send_to_char( "It's already closed.\n\r",    ch ); return; }

      SET_BIT(pexit->exit_info, EX_CLOSED);
      act( "$n closes the $d.", ch, NULL, pexit->keyword, TO_ROOM );
      send_to_char( "Ok.\n\r", ch );

      /* close the other side */
      if ( ( to_room   = pexit->u1.to_room            ) != NULL
      &&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != 0
      &&   pexit_rev->u1.to_room == ch->in_room )
      {
         CHAR_DATA *rch;

         SET_BIT( pexit_rev->exit_info, EX_CLOSED );
         for ( rch = to_room->people; rch != NULL; rch = rch->next_in_room)
         act( "The $d closes.", rch, NULL, pexit_rev->keyword, TO_CHAR );
      }
      return;
   }

   return;
}



bool has_key( CHAR_DATA *ch, int key )
{
   OBJ_DATA *obj;

   for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
   {
      if ( obj->pIndexData->vnum == key )
      return TRUE;
   }

   return FALSE;
}



void do_lock( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   OBJ_DATA *obj;
   int door;
   char* orig_argument = argument;

   argument = one_argument( argument, arg );

   if
   (
      !str_cmp(arg, "Fizzfaldt") ||
      !str_cmp(arg, "Buk") ||
      (
         !str_cmp(arg, "self") &&
         ch->race == grn_book &&
         !IS_NPC(ch)
      )
   )
   {
      do_book_lock(ch, orig_argument);
      return;
   }

   if ( arg[0] == '\0' )
   {
      send_to_char( "Lock what?\n\r", ch );
      return;
   }

   if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
   {
      if (obj->item_type == ITEM_BOOK)
      {
         do_book_lock(ch, orig_argument);
         return;
      }

      /* 'lock object' */
      if ( obj->item_type != ITEM_CONTAINER )
      { send_to_char( "That's not a container.\n\r", ch ); return; }
      if ( !IS_SET(obj->value[1], CONT_CLOSED) )
      { send_to_char( "It's not closed.\n\r",        ch ); return; }
      if ( obj->value[2] < 0 )
      { send_to_char( "It can't be locked.\n\r",     ch ); return; }
      if ( !has_key( ch, obj->value[2] ) )
      { send_to_char( "You lack the key.\n\r",       ch ); return; }
      if ( IS_SET(obj->value[1], CONT_LOCKED) )
      { send_to_char( "It's already locked.\n\r",    ch ); return; }

      SET_BIT(obj->value[1], CONT_LOCKED);
      act("You lock $p.", ch, obj, NULL, TO_CHAR);
      act( "$n locks $p.", ch, obj, NULL, TO_ROOM );
      return;
   }
   if (find_book(ch, arg) != NULL)
   {
      do_book_lock(ch, orig_argument);
      return;
   }

   if ( ( door = find_door( ch, arg ) ) >= 0 )
   {
      /* 'lock door' */
      ROOM_INDEX_DATA *to_room;
      EXIT_DATA *pexit;
      EXIT_DATA *pexit_rev;

      pexit    = ch->in_room->exit[door];
      if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
      { send_to_char( "It's not closed.\n\r",        ch ); return; }
      if ( pexit->key < 0 )
      { send_to_char( "It can't be locked.\n\r",     ch ); return; }
      if ( !has_key( ch, pexit->key) )
      { send_to_char( "You lack the key.\n\r",       ch ); return; }
      if ( IS_SET(pexit->exit_info, EX_LOCKED) )
      { send_to_char( "It's already locked.\n\r",    ch ); return; }

      SET_BIT(pexit->exit_info, EX_LOCKED);
      send_to_char( "*Click*\n\r", ch );
      act( "$n locks the $d.", ch, NULL, pexit->keyword, TO_ROOM );

      /* lock the other side */
      if ( ( to_room   = pexit->u1.to_room            ) != NULL
      &&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != 0
      &&   pexit_rev->u1.to_room == ch->in_room )
      {
         SET_BIT( pexit_rev->exit_info, EX_LOCKED );
      }
   }

   return;
}



void do_unlock( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   OBJ_DATA *obj;
   int door;
   char* orig_argument = argument;

   argument = one_argument( argument, arg );

   if
   (
      !str_cmp(arg, "Fizzfaldt") ||
      !str_cmp(arg, "Buk") ||
      (
         !str_cmp(arg, "self") &&
         ch->race == grn_book &&
         !IS_NPC(ch)
      )
   )
   {
      do_book_unlock(ch, orig_argument);
      return;
   }

   if ( arg[0] == '\0' )
   {
      send_to_char( "Unlock what?\n\r", ch );
      return;
   }

   if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
   {
      if (obj->item_type == ITEM_BOOK)
      {
         do_book_unlock(ch, orig_argument);
         return;
      }

      /* 'unlock object' */
      if ( obj->item_type != ITEM_CONTAINER )
      { send_to_char( "That's not a container.\n\r", ch ); return; }
      if ( !IS_SET(obj->value[1], CONT_CLOSED) )
      { send_to_char( "It's not closed.\n\r",        ch ); return; }
      if ( obj->value[2] < 0 )
      { send_to_char( "It can't be unlocked.\n\r",   ch ); return; }
      if ( !has_key( ch, obj->value[2] ) )
      { send_to_char( "You lack the key.\n\r",       ch ); return; }
      if ( !IS_SET(obj->value[1], CONT_LOCKED) )
      { send_to_char( "It's already unlocked.\n\r",  ch ); return; }

      REMOVE_BIT(obj->value[1], CONT_LOCKED);
      act("You unlock $p.", ch, obj, NULL, TO_CHAR);
      act( "$n unlocks $p.", ch, obj, NULL, TO_ROOM );
      if (obj->pIndexData->vnum == 13309)
      {
         OBJ_DATA *relic, *relic_next;
         for ( relic = obj->contains; relic != NULL; relic = relic_next )
         {
            relic_next = relic->next_content;
            if (relic->pIndexData->vnum == 13307)
            extract_obj(relic, FALSE);
         }
         relic = create_object( get_obj_index(13307), 0 );
         if (relic != NULL)
         {
            obj_to_obj(relic, obj);
         }
      }
      return;
   }
   if (find_book(ch, arg) != NULL)
   {
      do_book_unlock(ch, orig_argument);
      return;
   }

   if ( ( door = find_door( ch, arg ) ) >= 0 )
   {
      /* 'unlock door' */
      ROOM_INDEX_DATA *to_room;
      EXIT_DATA *pexit;
      EXIT_DATA *pexit_rev;

      pexit = ch->in_room->exit[door];
      if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
      { send_to_char( "It's not closed.\n\r",        ch ); return; }
      if ( pexit->key < 0 )
      { send_to_char( "It can't be unlocked.\n\r",   ch ); return; }
      if ( !has_key( ch, pexit->key) )
      { send_to_char( "You lack the key.\n\r",       ch ); return; }
      if ( !IS_SET(pexit->exit_info, EX_LOCKED) )
      { send_to_char( "It's already unlocked.\n\r",  ch ); return; }

      REMOVE_BIT(pexit->exit_info, EX_LOCKED);
      send_to_char( "*Click*\n\r", ch );
      act( "$n unlocks the $d.", ch, NULL, pexit->keyword, TO_ROOM );

      /* unlock the other side */
      if ( ( to_room   = pexit->u1.to_room            ) != NULL
      &&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
      &&   pexit_rev->u1.to_room == ch->in_room )
      {
         REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );
      }
   }

   return;
}



void do_pick( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *gch;
   OBJ_DATA *obj;
   int door;
   char* orig_argument = argument;
   argument = one_argument( argument, arg );

   if ( (get_skill(ch, gsn_pick_lock)) <= 0
   ||   (!IS_NPC(ch)
   && /*ch->level < skill_table[gsn_pick_lock].skill_level[ch->class]*/
   !has_skill(ch, gsn_pick_lock)))
   {
      send_to_char("The only thing you know how to pick is your nose.\n\r", ch);
      return;
   }

   if
   (
      !str_cmp(arg, "Fizzfaldt") ||
      !str_cmp(arg, "Buk") ||
      (
         !str_cmp(arg, "self") &&
         ch->race == grn_book &&
         !IS_NPC(ch)
      )
   )
   {
      do_book_pick(ch, orig_argument);
      return;
   }

   if ( arg[0] == '\0' )
   {
      send_to_char( "Pick what?\n\r", ch );
      return;
   }
   if (IS_NPC(ch) && ch->pIndexData->vnum != MOB_VNUM_KEY)
   return;
   if (ch->fighting != NULL)
   {
      send_to_char( "Not while fighting!\n\r", ch );
      return;
   }
   WAIT_STATE( ch, skill_table[gsn_pick_lock].beats );

   /* look for guards */
   for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
   {
      if (   IS_NPC(gch)
      && IS_AWAKE(gch)
      && !IS_AFFECTED(gch, AFF_CHARM)
      && ch->level + 5 < gch->level )
      {
         act( "$N is standing too close to the lock.",
         ch, NULL, gch, TO_CHAR );
         return;
      }
   }

   if ( !IS_NPC(ch) && number_percent( ) > get_skill(ch, gsn_pick_lock))
   {
      send_to_char( "You failed.\n\r", ch);
      check_improve(ch, gsn_pick_lock, FALSE, 2);
      return;
   }

   if (!str_cmp(arg, "shackles"))
   {
      CHAR_DATA *victim;

      argument = one_argument(argument, arg);
      if (arg[0] == '\0') victim = ch;
      else victim = get_char_room(ch, arg);

      if (victim == NULL){
         send_to_char("They arent here.\n\r", ch);
         return;
      }
      obj = get_eq_char(victim, WEAR_FEET);
      if (obj == NULL)
      {
         obj = get_eq_char(victim, WEAR_HOOVES);
      }
      if (obj == NULL)
      {
         obj = get_eq_char(victim, WEAR_FOURHOOVES);
      }
      if (obj == NULL)
      {
         send_to_char("They aren't wearing shackles.\n\r", ch);
         return;
      }
      if (obj->pIndexData->vnum != OBJ_VNUM_SHACKLES)
      {
         send_to_char("They aren't wearing shackles.\n\r", ch);
         return;
      }

      if (oblivion_blink(ch, victim))
      {
         return;
      }

      send_to_char("Click!\n\r", ch);
      if (ch == victim)
      {
         act("$n picks the lock on $s shackles.", ch, NULL, victim, TO_ROOM);
      }
      else
      {
         act("$n picks the lock on $N's shackles.", ch, NULL, victim, TO_NOTVICT);
         act("$n picks the lock on your shackles.", ch, NULL, victim, TO_VICT);
      }

      check_improve(ch, gsn_pick_lock, TRUE, 2);
      extract_obj(obj, FALSE);
      return;
   }

   if ( ( obj = get_obj_here( ch, arg ) ) != NULL )
   {

      if (obj->item_type == ITEM_BOOK)
      {
         do_book_pick(ch, orig_argument);
         return;
      }

      /* 'pick object' */
      if ( obj->item_type != ITEM_CONTAINER )
      { send_to_char( "That's not a container.\n\r", ch ); return; }
      if ( !IS_SET(obj->value[1], CONT_CLOSED) )
      { send_to_char( "It's not closed.\n\r",        ch ); return; }
      if ( obj->value[2] < 0 )
      { send_to_char( "It can't be unlocked.\n\r",   ch ); return; }
      if ( !IS_SET(obj->value[1], CONT_LOCKED) )
      { send_to_char( "It's already unlocked.\n\r",  ch ); return; }
      if ( IS_SET(obj->value[1], CONT_PICKPROOF) )
      { send_to_char( "You failed.\n\r",             ch ); return; }

      REMOVE_BIT(obj->value[1], CONT_LOCKED);
      act("You pick the lock on $p.", ch, obj, NULL, TO_CHAR);
      act("$n picks the lock on $p.", ch, obj, NULL, TO_ROOM);
      check_improve(ch, gsn_pick_lock, TRUE, 2);

      /* Malignus */
      if ( IS_SET(obj->value[1], CONT_SMALLTRAP) ||
      IS_SET(obj->value[1], CONT_BIGTRAP)) {

         int sn_poison,  sn_curse;
         AFFECT_DATA af;

         sn_poison = gsn_poison;
         sn_curse = gsn_curse;


         act("$n cries out and jerks back from the lock.", ch, NULL, ch, TO_NOTVICT);
         for (ch = ch->in_room->people; ch != NULL; ch = ch->next_in_room) {
            act("A panel in $p pops open, and a flurry of tiny needles shoots out, embedding themselves in your flesh!\n\r", ch, obj, NULL, TO_CHAR);
            send_to_char("Poison courses through your veins, and you feel very uncomfortable.\n\r", ch );
            /* Poison Effect */
            af.where     = TO_AFFECTS;
            af.type      = sn_poison;
            if ( IS_SET(obj->value[1], CONT_SMALLTRAP) ) af.level = 30;
            if ( IS_SET(obj->value[1], CONT_BIGTRAP) ) af.level = 60;
            if ( IS_SET(obj->value[1], CONT_SMALLTRAP) ) af.duration = 30;
            if ( IS_SET(obj->value[1], CONT_BIGTRAP) ) af.duration = 60;
            af.location  = APPLY_STR;
            af.modifier  = -2;
            af.bitvector = AFF_POISON;
            affect_join( ch, &af, 10 );

            /* Curse Effect */
            af.where     = TO_AFFECTS;
            af.type      = sn_curse;
            if ( IS_SET(obj->value[1], CONT_SMALLTRAP) ) af.level = 30;
            if ( IS_SET(obj->value[1], CONT_BIGTRAP) ) af.level = 60;
            if ( IS_SET(obj->value[1], CONT_SMALLTRAP) ) af.duration = 30;
            if ( IS_SET(obj->value[1], CONT_BIGTRAP) ) af.duration = 60;
            af.location  = APPLY_HITROLL;
            af.modifier  = -6;
            af.bitvector = AFF_CURSE;
            affect_to_char(ch, &af);

            af.location = APPLY_SAVING_SPELL;
            af.modifier = 6;
            affect_to_char(ch, &af);
         }


      }

      return;
   }

   if (find_book(ch, arg) != NULL)
   {
      do_book_pick(ch, orig_argument);
      return;
   }
   if ( ( door = find_door( ch, arg ) ) >= 0 )
   {
      /* 'pick door' */
      ROOM_INDEX_DATA *to_room;
      EXIT_DATA *pexit;
      EXIT_DATA *pexit_rev;

      pexit = ch->in_room->exit[door];
      if ( !IS_SET(pexit->exit_info, EX_CLOSED) && !IS_IMMORTAL(ch))
      { send_to_char( "It's not closed.\n\r",        ch ); return; }
      if ( pexit->key < 0 && !IS_IMMORTAL(ch))
      { send_to_char( "It can't be picked.\n\r",     ch ); return; }
      if ( !IS_SET(pexit->exit_info, EX_LOCKED) )
      { send_to_char( "It's already unlocked.\n\r",  ch ); return; }
      if ( IS_SET(pexit->exit_info, EX_PICKPROOF) && !IS_IMMORTAL(ch))
      { send_to_char( "You failed.\n\r",             ch ); return; }

      REMOVE_BIT(pexit->exit_info, EX_LOCKED);
      send_to_char( "*Click*\n\r", ch );
      act( "$n picks the $d.", ch, NULL, pexit->keyword, TO_ROOM );
      check_improve(ch, gsn_pick_lock, TRUE, 2);

      /* pick the other side */
      if ( ( to_room   = pexit->u1.to_room            ) != NULL
      &&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
      &&   pexit_rev->u1.to_room == ch->in_room )
      {
         REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );
      }
   }

   return;
}




void do_stand( CHAR_DATA *ch, char *argument )
{
   OBJ_DATA *obj = NULL;

   if (is_affected(ch, gsn_healing_trance)){
      send_to_char("You cease to meditate.\n\r", ch);
      affect_strip(ch, gsn_healing_trance);
   }

   if (is_affected(ch, gsn_camp))
   {
      send_to_char("You break camp feeling more refreshed.\n\r", ch);
      affect_strip(ch, gsn_camp);
   }

   if (argument[0] != '\0')
   {
      if (ch->position == POS_FIGHTING)
      {
         send_to_char("Maybe you should finish fighting first?\n\r", ch);
         return;
      }
      obj = get_obj_list(ch, argument, ch->in_room->contents);
      if (obj == NULL)
      {
         send_to_char("You don't see that here.\n\r", ch);
      }
      else if (
                 obj->item_type != ITEM_FURNITURE ||
                 (
                    !IS_SET(obj->value[2], STAND_AT)  &&
                    !IS_SET(obj->value[2], STAND_ON)  &&
                    !IS_SET(obj->value[2], STAND_IN)
                 )
              )
              {
                 act_new("You can't seem to find a place to stand on $p.", ch, obj, NULL, TO_CHAR, POS_DEAD);
                 obj = NULL;
              }
      else if (
                 ch->on != obj &&
                 count_users(obj) >= obj->value[0]
              )
              {
                 act_new("There's no room to stand on $p.", ch, obj, NULL, TO_CHAR, POS_DEAD);
                 obj = NULL;
              }
      else
          {
            ch->on = obj;
          }
   }

   switch ( ch->position )
   {
      case POS_SLEEPING:
      if 
      (
         IS_AFFECTED(ch, AFF_SLEEP) || 
         is_affected(ch, gsn_blackjack) || 
         is_affected(ch, gsn_strangle)
      )
      { 
         send_to_char( "You can't wake up!\n\r", ch ); 
         return; 
      }

      if (IS_AFFECTED2(ch, AFF_NAP))
      {
         send_to_char("Five more minutes mommy...\n\r", ch);
         return;
      }

      if (obj == NULL)
      {
         send_to_char( "You wake and stand up.\n\r", ch );
         act( "$n wakes and stands up.", ch, NULL, NULL, TO_ROOM );
         ch->on = NULL;
      }
      else if (IS_SET(obj->value[2], STAND_AT))
      {
         act_new("You wake and stand at $p.", ch, obj, NULL, TO_CHAR, POS_DEAD);
         act("$n wakes and stands at $p.", ch, obj, NULL, TO_ROOM);
      }
      else if (IS_SET(obj->value[2], STAND_ON))
      {
         act_new("You wake and stand on $p.", ch, obj, NULL, TO_CHAR, POS_DEAD);
         act("$n wakes and stands on $p.", ch, obj, NULL, TO_ROOM);
      }
      else
      {
         act_new("You wake and stand in $p.", ch, obj, NULL, TO_CHAR, POS_DEAD);
         act("$n wakes and stands in $p.", ch, obj, NULL, TO_ROOM);
      }
      ch->position = POS_STANDING;
      break;

      case POS_RESTING: case POS_SITTING:
      if (obj == NULL)
      {
         send_to_char( "You stand up.\n\r", ch );
         act( "$n stands up.", ch, NULL, NULL, TO_ROOM );
         ch->on = NULL;
      }
      else if (IS_SET(obj->value[2], STAND_AT))
      {
         act("You stand at $p.", ch, obj, NULL, TO_CHAR);
         act("$n stands at $p.", ch, obj, NULL, TO_ROOM);
      }
      else if (IS_SET(obj->value[2], STAND_ON))
      {
         act("You stand on $p.", ch, obj, NULL, TO_CHAR);
         act("$n stands on $p.", ch, obj, NULL, TO_ROOM);
      }
      else
      {
         act("You stand in $p.", ch, obj, NULL, TO_CHAR);
         act("$n stands on $p.", ch, obj, NULL, TO_ROOM);
      }
      ch->position = POS_STANDING;
      break;

      case POS_STANDING:
      send_to_char( "You are already standing.\n\r", ch );
      break;

      case POS_FIGHTING:
      send_to_char( "You are already fighting!\n\r", ch );
      break;
   }

   return;
}



void do_rest( CHAR_DATA *ch, char *argument )
{
   OBJ_DATA *obj = NULL;

   if (IS_AFFECTED2(ch, AFF_NO_REST) && ch->position != POS_SLEEPING)
   {
      send_to_char("You cannot seem to stop and rest.\n\r", ch);
      return;
   }
   if (ch->position == POS_FIGHTING)
   {
      send_to_char("You are already fighting!\n\r", ch);
      return;
   }

   /* okay, now that we know we can rest, find an object to rest on */
   if (argument[0] != '\0')
   {
      obj = get_obj_list(ch, argument, ch->in_room->contents);
      if (obj == NULL)
      {
         send_to_char("You don't see that here.\n\r", ch);
         return;
      }
   }
   else obj = ch->on;
   if (obj != NULL)
   {
      if (!IS_SET(obj->item_type, ITEM_FURNITURE)
      ||  (!IS_SET(obj->value[2], REST_ON)
      &&   !IS_SET(obj->value[2], REST_IN)
      &&   !IS_SET(obj->value[2], REST_AT)))
      {
         send_to_char("You can't rest on that.\n\r", ch);
         return;
      }

      if (obj != NULL && ch->on != obj && count_users(obj) >= obj->value[0])
      {
         act_new("There's no more room on $p.", ch, obj, NULL, TO_CHAR, POS_DEAD);
         return;
      }

      ch->on = obj;
   }
   switch ( ch->position )
   {
      case POS_SLEEPING:
      if (IS_AFFECTED(ch, AFF_SLEEP)
      || is_affected(ch, gsn_blackjack)
      || is_affected(ch, gsn_strangle))
      {
         send_to_char("You can't wake up!\n\r", ch);
         return;
      }
      if (IS_AFFECTED2(ch, AFF_NAP))
      {
         send_to_char("Five more minutes mommy...\n\r", ch);
         return;
      }

      if (obj == NULL)
      {
         send_to_char( "You wake up and start resting.\n\r", ch );
         act ("$n wakes up and starts resting.", ch, NULL, NULL, TO_ROOM);
      }
      else if (IS_SET(obj->value[2], REST_AT))
      {
         act_new("You wake up and rest at $p.",
         ch, obj, NULL, TO_CHAR, POS_SLEEPING);
         act("$n wakes up and rests at $p.", ch, obj, NULL, TO_ROOM);
      }
      else if (IS_SET(obj->value[2], REST_ON))
      {
         act_new("You wake up and rest on $p.",
         ch, obj, NULL, TO_CHAR, POS_SLEEPING);
         act("$n wakes up and rests on $p.", ch, obj, NULL, TO_ROOM);
      }
      else
      {
         act_new("You wake up and rest in $p.",
         ch, obj, NULL, TO_CHAR, POS_SLEEPING);
         act("$n wakes up and rests in $p.", ch, obj, NULL, TO_ROOM);
      }
      ch->position = POS_RESTING;
      break;

      case POS_RESTING:
      send_to_char( "You are already resting.\n\r", ch );
      break;

      case POS_STANDING:
      if (obj == NULL)
      {
         send_to_char( "You rest.\n\r", ch );
         act( "$n sits down and rests.", ch, NULL, NULL, TO_ROOM );
      }
      else if (IS_SET(obj->value[2], REST_AT))
      {
         act("You sit down at $p and rest.", ch, obj, NULL, TO_CHAR);
         act("$n sits down at $p and rests.", ch, obj, NULL, TO_ROOM);
      }
      else if (IS_SET(obj->value[2], REST_ON))
      {
         act("You sit on $p and rest.", ch, obj, NULL, TO_CHAR);
         act("$n sits on $p and rests.", ch, obj, NULL, TO_ROOM);
      }
      else
      {
         act("You rest in $p.", ch, obj, NULL, TO_CHAR);
         act("$n rests in $p.", ch, obj, NULL, TO_ROOM);
      }
      ch->position = POS_RESTING;
      break;

      case POS_SITTING:
      if (obj == NULL)
      {
         send_to_char("You rest.\n\r", ch);
         act("$n rests.", ch, NULL, NULL, TO_ROOM);
      }
      else if (IS_SET(obj->value[2], REST_AT))
      {
         act("You rest at $p.", ch, obj, NULL, TO_CHAR);
         act("$n rests at $p.", ch, obj, NULL, TO_ROOM);
      }
      else if (IS_SET(obj->value[2], REST_ON))
      {
         act("You rest on $p.", ch, obj, NULL, TO_CHAR);
         act("$n rests on $p.", ch, obj, NULL, TO_ROOM);
      }
      else
      {
         act("You rest in $p.", ch, obj, NULL, TO_CHAR);
         act("$n rests in $p.", ch, obj, NULL, TO_ROOM);
      }
      ch->position = POS_RESTING;
      break;
   }

   if (ch->on && is_affected(ch, gsn_camp))
   {
      send_to_char("You break camp feeling more refreshed.\n\r", ch);
      affect_strip(ch, gsn_camp);
   }
   return;
}


void do_sit (CHAR_DATA *ch, char *argument )
{
   OBJ_DATA *obj = NULL;

   if (ch->position == POS_FIGHTING)
   {
      send_to_char("Maybe you should finish this fight first?\n\r", ch);
      return;
   }

   /* okay, now that we know we can sit, find an object to sit on */
   if (argument[0] != '\0')
   {
      obj = get_obj_list(ch, argument, ch->in_room->contents);
      if (obj == NULL)
      {
         send_to_char("You don't see that here.\n\r", ch);
         return;
      }
   }
   else obj = ch->on;

   if (obj != NULL)
   {
      if (!IS_SET(obj->item_type, ITEM_FURNITURE)
      ||  (!IS_SET(obj->value[2], SIT_ON)
      &&   !IS_SET(obj->value[2], SIT_IN)
      &&   !IS_SET(obj->value[2], SIT_AT)))
      {
         send_to_char("You can't sit on that.\n\r", ch);
         return;
      }

      if (obj != NULL && ch->on != obj && count_users(obj) >= obj->value[0])
      {
         act_new("There's no more room on $p.", ch, obj, NULL, TO_CHAR, POS_DEAD);
         return;
      }

      ch->on = obj;
   }
   switch (ch->position)
   {
      case POS_SLEEPING:
      if (IS_AFFECTED(ch, AFF_SLEEP)
      || is_affected(ch, gsn_blackjack)
      || is_affected(ch, gsn_strangle))
      {
         send_to_char("You can't wake up!\n\r", ch);
         return;
      }
      if (IS_AFFECTED2(ch, AFF_NAP))
      {
         send_to_char("Five more minutes mommy...\n\r", ch);
         return;
      }

      if (obj == NULL)
      {
         send_to_char( "You wake and sit up.\n\r", ch );
         act( "$n wakes and sits up.", ch, NULL, NULL, TO_ROOM );
      }
      else if (IS_SET(obj->value[2], SIT_AT))
      {
         act_new("You wake and sit at $p.", ch, obj, NULL, TO_CHAR, POS_DEAD);
         act("$n wakes and sits at $p.", ch, obj, NULL, TO_ROOM);
      }
      else if (IS_SET(obj->value[2], SIT_ON))
      {
         act_new("You wake and sit on $p.", ch, obj, NULL, TO_CHAR, POS_DEAD);
         act("$n wakes and sits at $p.", ch, obj, NULL, TO_ROOM);
      }
      else
      {
         act_new("You wake and sit in $p.", ch, obj, NULL, TO_CHAR, POS_DEAD);
         act("$n wakes and sits in $p.", ch, obj, NULL, TO_ROOM);
      }

      ch->position = POS_SITTING;
      break;
      case POS_RESTING:
      if (obj == NULL)
      send_to_char("You stop resting.\n\r", ch);
      else if (IS_SET(obj->value[2], SIT_AT))
      {
         act("You sit at $p.", ch, obj, NULL, TO_CHAR);
         act("$n sits at $p.", ch, obj, NULL, TO_ROOM);
      }

      else if (IS_SET(obj->value[2], SIT_ON))
      {
         act("You sit on $p.", ch, obj, NULL, TO_CHAR);
         act("$n sits on $p.", ch, obj, NULL, TO_ROOM);
      }
      ch->position = POS_SITTING;
      break;
      case POS_SITTING:
      send_to_char("You are already sitting down.\n\r", ch);
      break;
      case POS_STANDING:
      if (obj == NULL)
      {
         send_to_char("You sit down.\n\r", ch);
         act("$n sits down on the ground.", ch, NULL, NULL, TO_ROOM);
      }
      else if (IS_SET(obj->value[2], SIT_AT))
      {
         act("You sit down at $p.", ch, obj, NULL, TO_CHAR);
         act("$n sits down at $p.", ch, obj, NULL, TO_ROOM);
      }
      else if (IS_SET(obj->value[2], SIT_ON))
      {
         act("You sit on $p.", ch, obj, NULL, TO_CHAR);
         act("$n sits on $p.", ch, obj, NULL, TO_ROOM);
      }
      else
      {
         act("You sit down in $p.", ch, obj, NULL, TO_CHAR);
         act("$n sits down in $p.", ch, obj, NULL, TO_ROOM);
      }
      ch->position = POS_SITTING;
      break;
   }
   if (ch->on && is_affected(ch, gsn_camp))
   {
      send_to_char("You break camp feeling more refreshed.\n\r", ch);
      affect_strip(ch, gsn_camp);
   }
   return;
}


void do_sleep( CHAR_DATA *ch, char *argument )
{
   OBJ_DATA *obj = NULL;

   /*
      if (is_affected(ch, gsn_healing_trance)){
      send_to_char("You cease to meditate.\n\r", ch);
      affect_strip(ch, gsn_healing_trance);
      }
   */
   if (IS_AFFECTED2(ch, AFF_NO_SLEEP))
   {
      send_to_char("You cannot seem to fall asleep.\n\r", ch);
      return;
   }
   if (ch->race == grn_arborian && !IS_IMMORTAL(ch))
   {
      obj = get_eq_char(ch, WEAR_BRAND);
      if (obj == NULL || obj->pIndexData->vnum != OBJ_VNUM_SHAM_BRAND)
      {
         send_to_char("You do not require sleep.\n\r", ch);
         return;
      }
      send_to_char("Your mosaic dove emits a violet aura and you fall asleep.\n\r", ch);
   }

   switch ( ch->position )
   {
      case POS_SLEEPING:
      send_to_char( "You are already sleeping.\n\r", ch );
      break;

      case POS_RESTING:
      case POS_SITTING:
      case POS_STANDING:
      if (argument[0] == '\0' && ch->on == NULL)
      {
         send_to_char( "You go to sleep.\n\r", ch );
         act( "$n goes to sleep.", ch, NULL, NULL, TO_ROOM );
         ch->position = POS_SLEEPING;
      }
      else  /* find an object and sleep on it */
      {
         if (argument[0] == '\0')
         obj = ch->on;
         else
         obj = get_obj_list( ch, argument,  ch->in_room->contents );

         if (obj == NULL)
         {
            send_to_char("You don't see that here.\n\r", ch);
            return;
         }
         if (obj->item_type != ITEM_FURNITURE
         ||  (!IS_SET(obj->value[2], SLEEP_ON)
         &&   !IS_SET(obj->value[2], SLEEP_IN)
         &&     !IS_SET(obj->value[2], SLEEP_AT)))
         {
            send_to_char("You can't sleep on that!\n\r", ch);
            return;
         }

         if (ch->on != obj && count_users(obj) >= obj->value[0])
         {
            act_new("There is no room on $p for you.",
            ch, obj, NULL, TO_CHAR, POS_DEAD);
            return;
         }

         ch->on = obj;
         if (IS_SET(obj->value[2], SLEEP_AT))
         {
            act("You go to sleep at $p.", ch, obj, NULL, TO_CHAR);
            act("$n goes to sleep at $p.", ch, obj, NULL, TO_ROOM);
         }
         else if (IS_SET(obj->value[2], SLEEP_ON))
         {
            act("You go to sleep on $p.", ch, obj, NULL, TO_CHAR);
            act("$n goes to sleep on $p.", ch, obj, NULL, TO_ROOM);
         }
         else
         {
            act("You go to sleep in $p.", ch, obj, NULL, TO_CHAR);
            act("$n goes to sleep in $p.", ch, obj, NULL, TO_ROOM);
         }
         ch->position = POS_SLEEPING;
      }
      break;

      case POS_FIGHTING:
      send_to_char( "You are already fighting!\n\r", ch );
      break;
   }
   if (ch->on && is_affected(ch, gsn_camp))
   {
      send_to_char("You break camp feeling more refreshed.\n\r", ch);
      affect_strip(ch, gsn_camp);
   }

   return;
}



void do_wake( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;

   one_argument( argument, arg );
   if ( arg[0] == '\0' )
   { do_stand( ch, argument ); return; }

   if ( !IS_AWAKE(ch) )
   { send_to_char( "You are asleep yourself!\n\r",       ch ); return; }

   if ( ( victim = get_char_room( ch, arg ) ) == NULL )
   { send_to_char( "They aren't here.\n\r",              ch ); return; }

   if ( IS_AWAKE(victim) )
   { act( "$N is already awake.", ch, NULL, victim, TO_CHAR ); return; }

   act("You try to wake $N.", ch, NULL, victim, TO_CHAR);
   if
   (
      IS_AFFECTED(victim, AFF_SLEEP) ||
      is_affected(victim, gsn_blackjack) ||
      is_affected(victim, gsn_strangle)
   )
   {
      act("You cannot wake $M!", ch, NULL, victim, TO_CHAR );
      return;
   }
   if (IS_AFFECTED2(victim, AFF_NAP))
   {
      act("$N does not seem to want to wake up.", ch, NULL, victim, TO_CHAR);
      send_to_char("\n\r", ch);
      return;
   }
   act_new( "$n wakes you.", ch, NULL, victim, TO_VICT, POS_SLEEPING );
   do_stand(victim, "");
   return;
}


void do_sneak( CHAR_DATA *ch, char *argument )
{
   AFFECT_DATA af;
   OBJ_DATA *obj;
   int sn_fog, sn_fire;

   if (get_skill(ch, gsn_sneak) <= 0)
   {
      send_to_char("You stumble about and finally give up. Sneaking is just not your forte.\n\r", ch);
      return;
   }
   obj = get_eq_char(ch, WEAR_FEET);
   if (obj == NULL)
   {
      obj = get_eq_char(ch, WEAR_HOOVES);
   }
   if (obj == NULL)
   {
      obj = get_eq_char(ch, WEAR_FOURHOOVES);
   }
   if (obj != NULL && obj->pIndexData->vnum == OBJ_VNUM_SHACKLES)
   {
      send_to_char("Your chains make too much noise to let you sneak.\n\r", ch);
      return;
   }

   sn_fog = gsn_faerie_fog;
   sn_fire = gsn_faerie_fire;
   if (is_affected(ch, sn_fog) || is_affected(ch, sn_fire))
   {
      send_to_char("You can't hide while glowing.\n\r", ch);
      return;
   }

   send_to_char( "You attempt to move silently.\n\r", ch );
   if (IS_AFFECTED(ch, AFF_SNEAK))
   return;

   if ( number_percent( ) < get_skill(ch, gsn_sneak))
   {
      check_improve(ch, gsn_sneak, TRUE, 3);
      af.where     = TO_AFFECTS;
      af.type      = gsn_sneak;
      af.level     = ch->level;
      af.duration  = ch->level;
      af.location  = APPLY_NONE;
      af.modifier  = 0;
      af.bitvector = AFF_SNEAK;
      affect_to_char( ch, &af );
      send_to_char( "You begin sneaking.\n\r", ch);
   }
   else
   check_improve(ch, gsn_sneak, FALSE, 3);

   return;
}

void do_cloak( CHAR_DATA *ch, char *argument )
{
   AFFECT_DATA af;
   long temp_flags;

   if (get_skill(ch, gsn_cloak_form) <= 0)
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }

   /* flag fliping -werv does criminal/rebel/outlaw mark/forsaken */
   /* NOTE: the saved_flags combines wiznet and act flags, this will work
   only if no flags match in letter, fortunately none of the above do
   match. */

   if (is_affected(ch, gsn_cloak_form))
   {
      affect_strip(ch, gsn_cloak_form);
      /* to remove old cloaked names -wervdon */
      if
      (
         !IS_NPC(ch) &&
         !IS_SET(ch->act2, PLR_IS_ANCIENT_KNOWN)
      )
      {
         /* store current saved flags to temp */
         temp_flags = ch->pcdata->saved_flags;
         /* change saved_flags to match current flags */
         if (IS_SET(ch->act, PLR_CRIMINAL))
         SET_BIT(ch->pcdata->saved_flags, PLR_CRIMINAL);
         else
         REMOVE_BIT(ch->pcdata->saved_flags, PLR_CRIMINAL);
         if (IS_SET(ch->act, PLR_COVENANT_ALLY))
         SET_BIT(ch->pcdata->saved_flags, PLR_COVENANT_ALLY);
         else
         REMOVE_BIT(ch->pcdata->saved_flags, PLR_COVENANT_ALLY);
         if (IS_SET(ch->act, PLR_MARKED))
         SET_BIT(ch->pcdata->saved_flags, PLR_MARKED);
         else
         REMOVE_BIT(ch->pcdata->saved_flags, PLR_MARKED);
         if (IS_SET(ch->act, PLR_EVIL))
         SET_BIT(ch->pcdata->saved_flags, PLR_EVIL);
         else
         REMOVE_BIT(ch->pcdata->saved_flags, PLR_EVIL);
         if (IS_SET(ch->wiznet, PLR_FORSAKEN))
         SET_BIT(ch->pcdata->saved_flags, PLR_FORSAKEN);
         else
         REMOVE_BIT(ch->pcdata->saved_flags, PLR_FORSAKEN);
         /* change current flags to match temp */
         if (IS_SET(temp_flags, PLR_CRIMINAL))
         SET_BIT(ch->act, PLR_CRIMINAL);
         else
         REMOVE_BIT(ch->act, PLR_CRIMINAL);
         if (IS_SET(temp_flags, PLR_COVENANT_ALLY))
         SET_BIT(ch->act, PLR_COVENANT_ALLY);
         else
         REMOVE_BIT(ch->act, PLR_COVENANT_ALLY);
         if (IS_SET(temp_flags, PLR_MARKED))
         SET_BIT(ch->act, PLR_MARKED);
         else
         REMOVE_BIT(ch->act, PLR_MARKED);
         if (IS_SET(temp_flags, PLR_EVIL))
         SET_BIT(ch->act, PLR_EVIL);
         else
         REMOVE_BIT(ch->act, PLR_EVIL);
         if (IS_SET(temp_flags, PLR_FORSAKEN))
         SET_BIT(ch->wiznet, PLR_FORSAKEN);
         else
         REMOVE_BIT(ch->wiznet, PLR_FORSAKEN);
      }
      if (ch->house == HOUSE_ANCIENT)
      {
         if
         (
            !IS_NPC(ch) &&
            !IS_SET(ch->act2, PLR_IS_ANCIENT_KNOWN) &&
            !IS_IMMORTAL(ch)
         )
         {
            OBJ_DATA *obj, *obj_next;
            for ( obj = ch->carrying; obj != NULL; obj = obj_next )
            {
               obj_next = obj->next_content;
               if (obj->pIndexData->vnum == OBJ_VNUM_ANCIENT_FORGE)
               {
                  act("$p fades into the shadows and vanishes.", ch, obj, NULL, TO_CHAR);
                  extract_obj( obj, FALSE );
               }
            }
            ch->house = 0;
            do_freetell(ch, "");
         }
      }
      else
      {
         do_freetell(ch, "");
      }
      send_to_char("You throw your cloak to the side, exposing yourself.\n\r", ch);
      return;
   }

   if (ch->morph_form[0] == 11 || ch->morph_form[0] == MORPH_MIMIC)
   {
      send_to_char("You cannot cloak yourself while in that morphed form.\n\r", ch);
      return;
   }
   if (number_percent() < get_skill(ch, gsn_cloak_form))
   {
      af.where    = TO_AFFECTS;
      af.type     = gsn_cloak_form;
      af.level    = ch->level;
      af.location = 0;
      af.modifier = 0;
      af.bitvector = AFF_SNEAK;
      af.duration = -1;
      if
      (
         !IS_NPC(ch) &&
         !IS_SET(ch->act2, PLR_IS_ANCIENT_KNOWN)
      )
      {
         do_freetell(ch, "");
         /* store current saved flags to temp */
         temp_flags = ch->pcdata->saved_flags;
         /* change saved_flags to match current flags */
         if (IS_SET(ch->act, PLR_CRIMINAL))
         SET_BIT(ch->pcdata->saved_flags, PLR_CRIMINAL);
         else
         REMOVE_BIT(ch->pcdata->saved_flags, PLR_CRIMINAL);
         if (IS_SET(ch->act, PLR_COVENANT_ALLY))
         SET_BIT(ch->pcdata->saved_flags, PLR_COVENANT_ALLY);
         else
         REMOVE_BIT(ch->pcdata->saved_flags, PLR_COVENANT_ALLY);
         if (IS_SET(ch->act, PLR_MARKED))
         SET_BIT(ch->pcdata->saved_flags, PLR_MARKED);
         else
         REMOVE_BIT(ch->pcdata->saved_flags, PLR_MARKED);
         if (IS_SET(ch->act, PLR_EVIL))
         SET_BIT(ch->pcdata->saved_flags, PLR_EVIL);
         else
         REMOVE_BIT(ch->pcdata->saved_flags, PLR_EVIL);
         if (IS_SET(ch->wiznet, PLR_FORSAKEN))
         SET_BIT(ch->pcdata->saved_flags, PLR_FORSAKEN);
         else
         REMOVE_BIT(ch->pcdata->saved_flags, PLR_FORSAKEN);
         /* change current flags to match temp */
         if (IS_SET(temp_flags, PLR_CRIMINAL))
         SET_BIT(ch->act, PLR_CRIMINAL);
         else
         REMOVE_BIT(ch->act, PLR_CRIMINAL);
         if (IS_SET(temp_flags, PLR_COVENANT_ALLY))
         SET_BIT(ch->act, PLR_COVENANT_ALLY);
         else
         REMOVE_BIT(ch->act, PLR_COVENANT_ALLY);
         if (IS_SET(temp_flags, PLR_MARKED))
         SET_BIT(ch->act, PLR_MARKED);
         else
         REMOVE_BIT(ch->act, PLR_MARKED);
         if (IS_SET(temp_flags, PLR_EVIL))
         SET_BIT(ch->act, PLR_EVIL);
         else
         REMOVE_BIT(ch->act, PLR_EVIL);
         if (IS_SET(temp_flags, PLR_FORSAKEN))
         SET_BIT(ch->wiznet, PLR_FORSAKEN);
         else
         REMOVE_BIT(ch->wiznet, PLR_FORSAKEN);
      }

      if (IS_SET(ch->act2, PLR_IS_ANCIENT))
      {
         ch->house = HOUSE_ANCIENT;
      }
      affect_to_char( ch, &af );
      if (ch->house == HOUSE_OUTLAW)
      {
         af.where = TO_AFFECTS;
         af.type = gsn_cloak_form;
         af.duration = -1;
         af.bitvector = 0;
         af.level = ch->level;
         af.location = APPLY_HITROLL;
         af.modifier = UMIN((2+ch->level/20), 4);
         affect_to_char(ch, &af);
         af.location = APPLY_DAMROLL;
         affect_to_char(ch, &af);
         af.location = APPLY_HIT;
         af.modifier = URANGE(20, 2*ch->level, 75);
         affect_to_char(ch, &af);
         af.location = APPLY_DEX;
         af.modifier = 1;
         affect_to_char(ch, &af);
      }

      check_improve(ch, gsn_cloak_form, TRUE, 6);
      send_to_char("You cloak your presence.\n\r", ch);
      if (!IS_NPC(ch))
      {
         ch->pcdata->called_by = NULL;
      }
      return;
   }
   else
   {
      check_improve(ch, gsn_cloak_form, FALSE, 3);
      send_to_char("You fail to cloak yourself.\n\r", ch);
      return;
   }
}

void do_vigilance(CHAR_DATA *ch, char *argument)
{
   AFFECT_DATA af;

   if ( (get_skill(ch, gsn_vigilance) <= 0)
   || /*(ch->level < skill_table[gsn_vigilance].skill_level[ch->class]) */
   !has_skill(ch, gsn_vigilance))
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }

   if (house_down(ch, HOUSE_CRUSADER))
   return;

   if (is_affected(ch, gsn_vigilance) )
   {
      send_to_char("You are already as vigilant as you can be.\n\r", ch);
      return;
   }
   if (ch->mana < 30)
   {
      send_to_char("You do not have the mana.\n\r", ch);
      return;
   }

   if (!IS_NPC(ch) && (number_percent() > ch->pcdata->learned[gsn_vigilance]))
   {
      send_to_char("You try to watch things more carefully but lose concentration.\n\r", ch);
      check_improve(ch, gsn_vigilance, FALSE, 1);
      ch->mana -= 15;
      return;
   }
   af.where = TO_AFFECTS;
   af.level = ch->level;
   af.location = 0;
   af.duration = 48;
   af.modifier = 0;
   af.type = gsn_vigilance;
   af.bitvector = AFF_DETECT_INVIS;
   affect_to_char(ch, &af);
   ch->mana -= 30;

   send_to_char("Your eyes sharpen as you concentrate on the taint of magic.\n\r", ch);
   return;
}


void do_sense_evil(CHAR_DATA *ch, char *argument)
{
   AFFECT_DATA af;

   if
   (
      !has_skill(ch, gsn_sense_evil) ||
      get_skill(ch, gsn_sense_evil) <= 0
   )
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }

   if ( house_down(ch, HOUSE_LIGHT) )
   return;

   if ( is_affected(ch, gsn_sense_evil) )
   {
      send_to_char( "You already smell the taint of evil.\n\r", ch );
      return;
   }
   if ( ch->mana < 30 )
   {
      send_to_char( "You do not have the mana.\n\r", ch );
      return;
   }

   if ( !IS_NPC(ch) && (number_percent() > ch->pcdata->learned[gsn_sense_evil]) )
   {
      send_to_char( "The mixture of scents in the air is too confusing.\n\r", ch );
      check_improve( ch, gsn_sense_evil, FALSE, 1 );
      ch->mana -= 15;
      return;
   }
   af.where = TO_AFFECTS;
   af.level = ch->level;
   af.location = 0;
   af.duration = 48;
   af.modifier = 0;
   af.type = gsn_sense_evil;
   af.bitvector = AFF_DETECT_EVIL;
   affect_to_char(ch, &af);
   af.bitvector = AFF_DETECT_GOOD;
   affect_to_char(ch, &af);
   ch->mana -= 30;

   send_to_char( "Your senses are overwhelmed by the smell of evil in the land.\n\r", ch );
   return;
}



void do_acute_vision( CHAR_DATA *ch, char *argument )
{
   AFFECT_DATA  af;

   if (get_skill(ch, gsn_acute_vision) <= 0
   || /*ch->level < skill_table[gsn_acute_vision].skill_level[ch->class]*/
   !has_skill(ch, gsn_acute_vision))
   {
      send_to_char("You don't know which bushes to look at.\n\r", ch);
      return;
   }

   if (is_affected(ch, gsn_acute_vision) )
   {
      send_to_char("Your eyes are already as sharp as they'll get.\n\r", ch);
      return;
   }

   if (ch->mana < 25)
   {
      send_to_char("You don't have the mana.\n\r", ch);
      return;
   }

   if (number_percent() > get_skill(ch, gsn_acute_vision))
   {
      send_to_char("Your vision sharpens but then dulls.\n\r", ch);
      check_improve(ch, gsn_acute_vision, FALSE, 1);
      ch->mana -= 12;
      return;
   }

   af.where = TO_AFFECTS;
   af.type = gsn_acute_vision;
   af.location = 0;
   af.modifier = 0;
   af.level = ch->level;
   af.duration = ch->level;
   af.bitvector = AFF_ACUTE_VISION;
   affect_to_char(ch, &af);

   ch->mana -= 25;
   send_to_char( "Your vision sharpens.\n\r", ch);
   return;
   check_improve(ch, gsn_acute_vision, TRUE, 1);
}

void do_healtrance(CHAR_DATA *ch, char *argument)
{
   AFFECT_DATA af;
   if ( get_skill(ch, gsn_healing_trance) <= 0 ||
   !has_skill(ch, gsn_healing_trance)){
      send_to_char("Huh?\n\r", ch);
      return;
   }

   if (ch->fighting != NULL){
      send_to_char("You are too busy fighting to meditate.\n\r", ch);
      return;
   }
   if (ch->mana < 75)
   {
      send_to_char("You don't feel mentally prepared to meditate.\n\r", ch);
      return;
   }
   ch->mana -= 75;
   do_rest(ch, "");

   WAIT_STATE(ch, 24);
   if (number_percent() > get_skill(ch, gsn_healing_trance)){
      send_to_char("You start to meditate then lose your concentration.\n\r", ch);
      check_improve(ch, gsn_healing_trance, FALSE, 1);
      return;
   }
   send_to_char("You relax and begin to concentrate on healing.\n\r", ch);
   act("$n kneels and begins to silently meditate.", ch, NULL, NULL, TO_ROOM);

   check_improve(ch, gsn_healing_trance, TRUE, 1);

   af.where = TO_AFFECTS;
   af.level = ch->level;
   af.type  = gsn_healing_trance;
   af.location = APPLY_REGENERATION;
   af.modifier = ch->level/5;
   af.bitvector = 0;
   af.duration = 23;

   affect_to_char(ch, &af);
   return;
}

void do_camp(CHAR_DATA *ch, char *argument)
{
   bool rest = FALSE;
   OBJ_DATA* obj = NULL;
   AFFECT_DATA af;
   if
   (
      get_skill(ch, gsn_camp) <= 0 ||
      !has_skill(ch, gsn_camp)
   )
   {
      send_to_char("You don't know how to effectively camp.\n\r", ch);
      return;
   }
   if (!str_cmp(argument, "rest"))
   {
      rest = TRUE;
   }
   if (ch->position == POS_SLEEPING )
   {
      send_to_char("You are already asleep.", ch);
      return;
   }
   if
   (
      ch->in_room->sector_type != SECT_FOREST      &&
      ch->in_room->sector_type != SECT_HILLS       &&
      ch->in_room->sector_type != SECT_UNDERGROUND &&
      ch->in_room->sector_type != SECT_MOUNTAIN
   )
   {
      send_to_char("This land is not wild enough for you to camp out in.\n\r", ch);
      return;
   }

   if (ch->on != NULL)
   {
      send_to_char("You need to stand first. Camping can only be done on wild land.\n\r", ch);
      return;
   }
   obj = get_eq_char(ch, WEAR_BRAND);
   if
   (
      !rest &&
      !(
         (
            obj != NULL &&
            obj->pIndexData->vnum == OBJ_VNUM_SHAM_BRAND
         ) ||
         ch->race != grn_arborian ||
         IS_IMMORTAL(ch)
      )
   )
   {
      rest = TRUE;
   }
   af.where = TO_AFFECTS;
   af.location = 0;
   af.duration = -1;
   af.modifier = 0;
   af.level = ch->level;
   af.type = gsn_camp;
   af.bitvector = 0;
   if (!is_affected(ch, gsn_camp))
   {
      affect_to_char(ch, &af);
   }
   act("$n settles down and sets up camp.", ch, NULL, NULL, TO_ROOM);
   send_to_char("You settle down and set up camp.\n\r", ch);
   if (rest)
   {
      do_rest(ch, "");
   }
   else
   {
      do_sleep(ch, "");
   }
   return;
}

void do_shroud(CHAR_DATA *ch, char *argument)
{
   AFFECT_DATA af;
   int sn_fog, sn_fire;

   if ( (get_skill(ch, gsn_shroud) <= 0)
   || /*( ch->level < skill_table[gsn_shroud].skill_level[ch->class])*/
   !has_skill(ch, gsn_shroud) )
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }

   sn_fog = gsn_faerie_fog;
   sn_fire = gsn_faerie_fire;
   if (is_affected(ch, sn_fog) || is_affected(ch, sn_fire))
   {
      send_to_char("You can't shroud while glowing.\n\r", ch);
      return;
   }

   if (house_down(ch, HOUSE_ANCIENT))
   return;

   if (is_affected(ch, gsn_shroud))
   {
      send_to_char("You are already enshrouded in shadow.\n\r", ch);
      return;
   }
   send_to_char("You attempt to enshroud yourself in shadows.\n\r", ch);
   if (!IS_NPC(ch) && (number_percent() > ch->pcdata->learned[gsn_shroud]))
   {
      check_improve(ch, gsn_shroud, FALSE, 1);
      return;
   }
   check_improve(ch, gsn_shroud, TRUE, 1);

   af.where = TO_AFFECTS;
   af.level = ch->level;
   af.type = gsn_shroud;
   af.location = 0;
   af.modifier = 0;
   af.bitvector = 0;
   af.duration = 10;
   affect_to_char(ch, &af);
   return;
}

void do_camouflage( CHAR_DATA *ch, char *argument )
{
   int sn_fog;
   int sn_fire;

   if
   (
      (get_skill(ch, gsn_camouflage)) <= 0 ||
      (
         !IS_NPC(ch) &&
         /*ch->level < skill_table[gsn_camouflage].skill_level[ch->class]*/
         !has_skill(ch, gsn_camouflage)
      )
   )
   {
      send_to_char("A four-century oak would not conceal you even if it wanted to.\n\r", ch);
      return;
   }

   sn_fog = gsn_faerie_fog;
   sn_fire = gsn_faerie_fire;
   if
   (
      is_affected(ch, sn_fog) ||
      is_affected(ch, sn_fire) ||
      IS_AFFECTED(ch, AFF_FAERIE_FIRE)
   )
   {
      send_to_char("You can't camouflage while glowing.\n\r", ch);
      return;
   }

   if
   (
      (ch->in_room->sector_type != SECT_FOREST) &&
      (ch->in_room->sector_type != SECT_MOUNTAIN) &&
      (ch->in_room->sector_type != SECT_HILLS) &&
      (ch->in_room->sector_type != SECT_UNDERGROUND)
   )
   {
      send_to_char("There is no cover here.\n\r", ch);
      act("$n tries to cover $mself on the single leaf on the ground.", ch, NULL, NULL, TO_ROOM);
      return;
   }

   /*
   if ( IS_AFFECTED(ch, AFF_CAMOUFLAGE) || IS_NPC(ch) )
   {
   send_to_char("You are already blending into your surroundings.\n\r", ch);
   return;
   }
   */

   send_to_char("You attempt to blend in with your surroundings.\n\r", ch);

   if ( number_percent( ) < get_skill(ch, gsn_camouflage) )
   {
      SET_BIT(ch->affected_by, AFF_CAMOUFLAGE);
      check_improve(ch, gsn_camouflage, TRUE, 2);
      WAIT_STATE(ch, 3);
   }
   else
   {
      check_improve(ch, gsn_camouflage, FALSE, 2);
      REMOVE_BIT(ch->affected_by, AFF_CAMOUFLAGE);
      WAIT_STATE(ch, 3);
   }

   return;
}

void do_lurk( CHAR_DATA *ch, char *argument )
{
   int sn_fog, sn_fire;
   AFFECT_DATA af;

   if ( (get_skill(ch, gsn_lurk)) <= 0
   ||   (!IS_NPC(ch)
   && /*ch->level < skill_table[gsn_hide].skill_level[ch->class]*/
   !has_skill(ch, gsn_lurk)))
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }

   if (house_down(ch, HOUSE_OUTLAW))
   return;

   sn_fog = gsn_faerie_fog;
   sn_fire = gsn_faerie_fire;
   if (is_affected(ch, sn_fog) || is_affected(ch, sn_fire)
   || IS_AFFECTED(ch, AFF_FAERIE_FIRE))
   {
      send_to_char("You can't lurk in the shadows while glowing.\n\r", ch);
      return;
   }


   send_to_char( "You attempt to lurk within the shadows.\n\r", ch );

   if ( is_affected(ch, gsn_lurk) )
   return;

   if ( number_percent( ) < get_skill(ch, gsn_lurk))
   {
      af.where    = TO_AFFECTS;
      af.type     = gsn_lurk;
      af.level    = ch->level;
      af.location = 0;
      af.modifier = 0;
      af.bitvector = 0;
      af.duration = (ch->level)/10;
      affect_to_char( ch, &af );

      check_improve(ch, gsn_lurk, TRUE, 3);
   }
   else
   check_improve(ch, gsn_lurk, FALSE, 3);

   return;
}

void do_hide( CHAR_DATA *ch, char *argument )
{
   int sn_fog, sn_fire;

   if ( (get_skill(ch, gsn_hide)) <= 0
   ||   (!IS_NPC(ch)
   && /*ch->level < skill_table[gsn_hide].skill_level[ch->class]*/
   !has_skill(ch, gsn_hide)))
   {
      send_to_char("What are YOU afraid of?\n\r", ch);
      return;
   }

   sn_fog = gsn_faerie_fog;
   sn_fire = gsn_faerie_fire;
   if (is_affected(ch, sn_fog) || is_affected(ch, sn_fire)
   || IS_AFFECTED(ch, AFF_FAERIE_FIRE))
   {
      send_to_char("You can't hide while glowing.\n\r", ch);
      return;
   }

   if ((ch->in_room->sector_type != SECT_CITY) && (ch->in_room->sector_type != SECT_INSIDE) &&
   (ch->in_room->sector_type != SECT_UNDERGROUND))
   {
      if (!has_skill(ch, gsn_entrench) || get_skill(ch, gsn_entrench) < 1)
      {
         send_to_char("The shadows here are too natural to blend with.\n\r", ch);
         return;
      }
      else
      {
         if ( ((ch->in_room->sector_type == SECT_FOREST) ||
         (ch->in_room->sector_type == SECT_MOUNTAIN) ||
         (ch->in_room->sector_type == SECT_HILLS) ))
         {
            send_to_char("You attempt to entrench yourself and await a victim.\n\r", ch);
            if (number_percent() < UMAX(get_skill(ch, gsn_entrench), 2))
            {
               check_improve(ch, gsn_entrench, TRUE, 1);
            }
            else
            {
               return;
            }
         }
         else
         {
            send_to_char("You cannot entrench yourself here.\n\r", ch);
            return;
         }
      }
   }

   send_to_char( "You attempt to hide.\n\r", ch );

   if ( IS_AFFECTED(ch, AFF_HIDE) )
   return;

   if ( number_percent( ) < get_skill(ch, gsn_hide))
   {
      SET_BIT(ch->affected_by, AFF_HIDE);
      check_improve(ch, gsn_hide, TRUE, 3);
   }
   else
   check_improve(ch, gsn_hide, FALSE, 3);

   return;
}

void un_camouflage( CHAR_DATA *ch, char *argument )
{
   if ( IS_AFFECTED(ch, AFF_CAMOUFLAGE) )
   {
      REMOVE_BIT( ch->affected_by, AFF_CAMOUFLAGE );
      act("$n steps out from $s cover.", ch, NULL, NULL, TO_ROOM);
      send_to_char("You step out from your cover.\n\r", ch);
   }
   return;
}

void un_blackjack( CHAR_DATA *ch, char *argument )
{
   affect_strip( ch, gsn_blackjack );
   return;
}

void un_strangle( CHAR_DATA *ch, char *argument )
{
   affect_strip( ch, gsn_strangle );
   return;
}

void un_hide( CHAR_DATA *ch, char *argument )
{
   if ( IS_AFFECTED(ch, AFF_HIDE) )
   {
      REMOVE_BIT   ( ch->affected_by, AFF_HIDE        );
      act("$n steps out of the shadows.", ch, NULL, NULL, TO_ROOM);
      send_to_char("You step out of the shadows.\n\r", ch);
   }
   return;
}

void un_shroud(CHAR_DATA *ch, char *argument)
{
   if (is_affected(ch, gsn_shroud) )
   {
      affect_strip(ch, gsn_shroud);
      act("$n fades out of the shadows.", ch, NULL, NULL, TO_ROOM);
      send_to_char("You fade out of the shadows enshrouding you.\n\r", ch);
   }
   return;
}

void un_invis( CHAR_DATA *ch, char *argument )
{
   if ( IS_AFFECTED(ch, AFF_INVISIBLE))
   {
      affect_strip ( ch, gsn_invis);
      affect_strip ( ch, gsn_mass_invis);
      REMOVE_BIT   ( ch->affected_by, AFF_INVISIBLE    );
      act("$n fades into existence.\n\r", ch, NULL, NULL, TO_ROOM);
      send_to_char("You fade into existence.\n\r", ch);
   }
   return;
}
void un_sneak( CHAR_DATA *ch, char *argument )
{
   /* elf, grey-elf, dark-elf, halfling */
   if ( ch->race == 3 || ch->race == 4 || ch->race == 5 || ch->race ==
   14)
   return;

   if ( IS_AFFECTED(ch, AFF_SNEAK))
   {
      affect_strip ( ch, gsn_sneak);
      REMOVE_BIT   ( ch->affected_by, AFF_SNEAK        );
      send_to_char("You trample around loudly again.\n\r", ch);
   }
   return;
}

void do_visible( CHAR_DATA *ch, char *argument )
{
   un_invis(ch, NULL);
   un_sneak(ch, NULL);
   affect_strip(ch, gsn_lurk);
   passive_visible(ch);
   return;
}

/*
   Removes all forms of hiding that generally do not stay with all terrain
   Examples of exceptions are
   invisible
   sneak
   lurk
*/
void passive_visible(CHAR_DATA* ch)
{
   un_shroud(ch, NULL);
   un_earthfade(ch, NULL);
   un_camouflage(ch, NULL);
   un_hide(ch, NULL);
   un_forest_blend(ch); /* - Wicket */
}

void un_earthfade(CHAR_DATA *ch, char *argument)
{
   if (is_affected(ch, gsn_submerge))
   {
      affect_strip(ch, gsn_submerge);
      act("$n emerges from beneath the waves.", ch, NULL, NULL, TO_ROOM);
      send_to_char("You emerge from beneath the waves.\n\r", ch);
   }
   if (!is_affected(ch, gsn_earthfade))
   return;
   affect_strip(ch, gsn_earthfade);

   act("$n slowly emerges from the ground.", ch, NULL, NULL, TO_ROOM);
   send_to_char("You slowly emerge from the ground.\n\r", ch);
   WAIT_STATE(ch, 8);
   return;
}


void do_recall(CHAR_DATA* ch, char* argument)
{
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA* victim;
   ROOM_INDEX_DATA* location;
   AFFECT_DATA af;

   if (get_skill(ch, gsn_recall) <= 0)
   {
      send_to_char("You do not know how to recall.\n\r", ch);
      return;
   }

   if
   (
      IS_NPC(ch) &&
      !IS_SET(ch->act, ACT_PET)
   )
   {
      send_to_char("Only players can recall.\n\r", ch);
      return;
   }
   if
   (
      ch->level > 10 &&
      ch->house != HOUSE_CRUSADER
   )
   {
      send_to_char("Those over level 10 cannot recall.\n\r", ch);
      return;
   }


   if
   (
      ch->in_room->house != 0 &&
      ch->in_room->house != ch->house
   )
   {
      send_to_char("Not while in an enemy house.\n\r", ch);
      return;
   }

   if (IS_SET(ch->in_room->extra_room_flags, ROOM_DEAD_ZONE))
   {
      send_to_char("You failed.\n\r", ch);
      return;
   }

   if
   (
      ch->level > 10 &&
      ch->house == HOUSE_CRUSADER
   )
   {

      if (house_down(ch, HOUSE_CRUSADER))
      {
         return;
      }

      if (is_affected(ch, gsn_recall))
      {
         send_to_char
         (
            "You have already been granted recall by your Immortal recently."
            "\n\r",
            ch
         );
         return;
      }
      if
      (
         !check_room_noescape(ch) &&
         (
            ch->fighting != NULL ||
            ch->position == POS_FIGHTING
         )
      )
      {
         send_to_char
         (
            "You are a Crusader..you don't ask to be taken from battles!\n\r",
            ch
         );
         return;
      }
      send_to_char("You pray to the Immortal of Crusaders and recall!\n\r", ch);
      if
      (
         IS_SET(ch->in_room->extra_room_flags, ROOM_DEAD_ZONE) ||
         (
            IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL) &&
            !IS_SET(ch->in_room->room_flags, ROOM_CONSECRATED)
         ) ||
         IS_AFFECTED(ch, AFF_CURSE)
      )
      {
         send_to_char("The gods have forsaken you.\n\r", ch);
         return;
      }
      act
      (
         "$n calls to $s Immortal leader and vanishes!",
         ch,
         NULL,
         NULL,
         TO_ROOM
      );
      location = get_room_index(ROOM_VNUM_CRUSADER);
      if (location == NULL)
      {
         location = get_room_index(ROOM_VNUM_TEMPLE);
      }
      if (location == NULL)
      {
         send_to_char
         (
            "Your temple has been destroyed..pray to the Gods for help!\n\r",
            ch
         );
         return;
      }

      un_forest_blend(ch);
      un_camouflage(ch, NULL);
      un_hide(ch, NULL);

      char_from_room(ch);
      char_to_room_look(ch, location);

      af.where = TO_AFFECTS;
      af.type = gsn_recall;
      af.duration = 12;
      af.modifier = 0;
      af.bitvector = 0;
      af.location = 0;
      af.level=ch->level;
      if (ch->quittime != 0)
      {
         affect_to_char(ch, &af);
      }
      return;
   }

   /*act("$n prays for transportation!", ch, NULL, NULL, TO_ROOM); Old one*/

   act
   (
      "$n focuses on the residual energy from the portal of the mists!",
      ch,
      NULL,
      NULL,
      TO_ROOM
   );
   location = get_room_index(ch->temple);
   if (location == NULL)
   {
      location = get_room_index(ROOM_VNUM_TEMPLE);
   }
   if (location == NULL)
   {
      send_to_char("You are completely lost.\n\r", ch);
      return;
   }

   if (ch->in_room == location)
   {
      return;
   }

   if
   (
      (
         IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL) &&
         !IS_SET(ch->in_room->room_flags, ROOM_CONSECRATED)
      ) ||
      IS_AFFECTED(ch, AFF_CURSE)
   )
   {
      send_to_char("The gods have forsaken you.\n\r", ch);
      return;
   }

   if
   (
      (
         victim = ch->fighting
      ) != NULL
   )
   {
      int lose;
      int skill;

      skill = get_skill(ch, gsn_recall);

      if (number_percent() < 80 * skill / 100)
      {
         check_improve(ch, gsn_recall, FALSE, 6);
         WAIT_STATE(ch, 4);
         send_to_char("You failed!\n\r", ch);
         return;
      }

      lose = (ch->desc != NULL) ? 25 : 50;
      gain_exp(ch, 0 - lose);
      check_improve(ch, gsn_recall, TRUE, 4);
      sprintf(buf, "You recall from combat!  You lose %d exps.\n\r", lose);
      send_to_char(buf, ch);
      stop_fighting(ch, TRUE);
   }
   check_improve(ch, gsn_recall, TRUE, 1);
   un_earthfade(ch, NULL);
   un_camouflage(ch, NULL);
   un_hide(ch, NULL);
   un_forest_blend(ch);  /* - Wicket */
   ch->move /= 2;
   act("$n disappears.", ch, NULL, NULL, TO_ROOM);
   char_from_room(ch);
   char_to_room_look(ch, location);
   act("$n appears in the room.", ch, NULL, NULL, TO_ROOM);

   if (ch->pet != NULL)
   {
      do_recall(ch->pet, "");
   }

   return;
}

void do_learn_adv( CHAR_DATA *ch, char *argument)
{
   CHAR_DATA *mob;
   char buf[MAX_STRING_LENGTH];
   int sn_adv = -1;
   int sn_exp = -1;
   int sn_norm = -1;
   int sn_learn = -1;
   int skill_count = 0;

   if ( IS_NPC(ch) )
   return;


   if (ch->class != (GUILD_WARRIOR -1)){
      send_to_char("Huh?\n\r", ch);
      return;
   }

   /*
      Check for trainer.
   */
   for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
   {
      if (( IS_NPC(mob) && IS_SET(mob->act, ACT_TRAIN) ) ||
      (!IS_NPC(mob) && IS_SET(mob->act2, PLR_GUILDMASTER)
      && (mob->class == ch->class)))
      break;
   }

   if (mob == NULL){
      send_to_char("You can't do that here.\n\r", ch);
      return;
   }

   if ( argument[0] == '\0' )
   {
      sprintf( buf, "You have to request a specific weapon.\n\r");
      send_to_char( buf, ch );
      return;
   }
   if ( !str_cmp( argument, "sword" ) )
   {
      sprintf(buf, "You have become more experienced with swords.\n\r");
      sn_norm = gsn_sword;
      sn_adv = gsn_adv_sword;
      sn_exp = gsn_exp_sword;
   }
   if ( !str_cmp( argument, "axe" ) )
   {
      sprintf(buf, "You have become more experienced with axes.\n\r");
      sn_norm = gsn_axe;
      sn_adv = gsn_adv_axe;
      sn_exp = gsn_exp_axe;
   }
   if ( !str_cmp( argument, "spear" ) )
   {
      sprintf(buf, "You have become more experienced with spears.\n\r");
      sn_norm = gsn_spear;
      sn_adv = gsn_adv_spear;
      sn_exp = gsn_exp_spear;
   }
   if ( !str_cmp( argument, "polearm" ) )
   {
      sprintf(buf, "You have become more experienced with polearms.\n\r");
      sn_norm = gsn_polearm;
      sn_adv = gsn_adv_polearm;
      sn_exp = gsn_exp_polearm;
   }
   if ( !str_cmp( argument, "staff" ) )
   {
      sprintf(buf, "You have become more experienced with staffs.\n\r");
      sn_norm = gsn_staff;
      sn_adv = gsn_adv_staff;
      sn_exp = gsn_exp_staff;
   }
   if ( !str_cmp( argument, "mace" ) )
   {
      sprintf(buf, "You have become more experienced with maces.\n\r");
      sn_norm = gsn_mace;
      sn_adv = gsn_adv_mace;
      sn_exp = gsn_exp_mace;
   }
   if ( !str_cmp( argument, "whip" ) )
   {
      sprintf(buf, "You have become more experienced with whips.\n\r");
      sn_norm = gsn_whip;
      sn_adv = gsn_adv_whip;
      sn_exp = gsn_exp_whip;
   }
   if ( !str_cmp( argument, "flail" ) )
   {
      sprintf(buf, "You have become more experienced with flails.\n\r");
      sn_norm = gsn_flail;
      sn_adv = gsn_adv_flail;
      sn_exp = gsn_exp_flail;
   }
   if ( !str_cmp( argument, "dagger" ) )
   {
      sprintf(buf, "You have become more experienced with daggers.\n\r");
      sn_norm = gsn_dagger;
      sn_adv = gsn_adv_dagger;
      sn_exp = gsn_exp_dagger;
   }
   if ( !str_cmp( argument, "archery" ) )
   {
      sprintf(buf, "You have become more experienced in archery.\n\r");
      sn_norm = gsn_archery;
      sn_adv = gsn_adv_archery;
      sn_exp = gsn_exp_archery;
   }

   if (sn_norm == -1){
      send_to_char("The guildmaster can't figure out what you want to learn.\n\r", ch);
      return;
   }
   if (ch->pcdata->learnlvl[gsn_adv_sword] < 52)
   skill_count++;
   if (ch->pcdata->learnlvl[gsn_exp_sword] < 52)
   skill_count++;
   if (ch->pcdata->learnlvl[gsn_adv_axe] < 52)
   skill_count++;
   if (ch->pcdata->learnlvl[gsn_exp_axe] < 52)
   skill_count++;
   if (ch->pcdata->learnlvl[gsn_adv_dagger] < 52)
   skill_count++;
   if (ch->pcdata->learnlvl[gsn_exp_dagger] < 52)
   skill_count++;
   if (ch->pcdata->learnlvl[gsn_adv_spear] < 52)
   skill_count++;
   if (ch->pcdata->learnlvl[gsn_exp_spear] < 52)
   skill_count++;
   if (ch->pcdata->learnlvl[gsn_adv_staff] < 52)
   skill_count++;
   if (ch->pcdata->learnlvl[gsn_exp_staff] < 52)
   skill_count++;
   if (ch->pcdata->learnlvl[gsn_adv_polearm] < 52)
   skill_count++;
   if (ch->pcdata->learnlvl[gsn_exp_polearm] < 52)
   skill_count++;
   if (ch->pcdata->learnlvl[gsn_adv_mace] < 52)
   skill_count++;
   if (ch->pcdata->learnlvl[gsn_exp_mace] < 52)
   skill_count++;
   if (ch->pcdata->learnlvl[gsn_adv_whip] < 52)
   skill_count++;
   if (ch->pcdata->learnlvl[gsn_exp_whip] < 52)
   skill_count++;
   if (ch->pcdata->learnlvl[gsn_adv_flail] < 52)
   skill_count++;
   if (ch->pcdata->learnlvl[gsn_exp_flail] < 52)
   skill_count++;
   if (ch->pcdata->learnlvl[gsn_adv_archery] < 52)
   skill_count++;
   if (ch->pcdata->learnlvl[gsn_exp_archery] < 52)
   skill_count++;
   if (ch->pcdata->learned[sn_norm] < 100){
      send_to_char("You should master what you know first.\n\r", ch);
      return;
   }
   if (skill_count == 20)
   {
      send_to_char("Try again after you have quit out and come back once with this character.\n\r", ch);
      return;
   }
   if (ch->level < 20){
      send_to_char("You are too inexperienced to learn more.\n\r", ch);
      return;
   }
   if (ch->level < 35 && skill_count >= 1){
      send_to_char("You are too inexperienced to learn more.\n\r", ch);
      return;
   }
   if (ch->level < 45 && skill_count >= 2){
      send_to_char("You are too inexperienced to learn more.\n\r", ch);
      return;
   }
   if (skill_count >= 3){
      send_to_char("You are too inexperienced to learn more.\n\r", ch);
      return;
   }
   sn_learn = sn_adv;
   if (ch->pcdata->learnlvl[sn_learn] < 52){
      if (ch->pcdata->learned[sn_learn] < 100){
         send_to_char("You should master what you know first.\n\r", ch);
         return;
      }
      sn_learn = sn_exp;
      if (ch->pcdata->learnlvl[sn_learn] < 52){
         send_to_char("You can learn no more about this weapon.\n\r", ch);
         return;
      }
   }
   do_say(mob, "Yes, I think I can teach you.");
   send_to_char(buf, ch);
   ch->pcdata->learned[sn_learn] = 1;
   ch->pcdata->learnlvl[sn_learn] = ch->level;
   return;
}

void do_give_belt( CHAR_DATA *ch, char *argument )
{
   CHAR_DATA *mob;
   AFFECT_DATA af;
   int current_belt;
   int new_belt;
   OBJ_DATA *obj;
   OBJ_DATA *belt;
   char buf[MAX_STRING_LENGTH];
   char save_title[MAX_STRING_LENGTH];
   bool empty_title;
   int cnt;
   int lock= FALSE;

   if ( IS_NPC(ch) )
   return;

   if (ch->class != (GUILD_MONK -1)){
      send_to_char("Huh?\n\r", ch);
      return;
   }

   if (ch->in_room->guild != GUILD_MONK){
      send_to_char("You must be within a monk guild.\n\r", ch);
      return;
   }
   /*
      Check for trainer.
   */
   for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
   {
      if (( IS_NPC(mob) && IS_SET(mob->act, ACT_TRAIN) ) ||
      (!IS_NPC(mob) && IS_SET(mob->act2, PLR_GUILDMASTER)
      && (mob->class == ch->class)))
      break;
   }

   if (mob == NULL){
      send_to_char("You can't do that here.\n\r", ch);
      return;
   }

   current_belt = ch->pcdata->learned[gsn_corrupt("subrank", &gsn_subrank)];
   if (current_belt >= 90){
      current_belt -= 90;
      new_belt = current_belt;
      current_belt--;
      lock = TRUE;
   }
   else
   {
      new_belt = check_belt(ch);
   }
   if (new_belt == current_belt){
      send_to_char("You request a new belt from the guildmaster.\n\r", ch);
   }
   else
   {
      send_to_char("You have been promoted in your school!\n\r", ch);
      do_say(mob, "You have done well, my pupil.");
      if (!lock)
      ch->pcdata->learned[gsn_corrupt("subrank", &gsn_subrank)] = new_belt;
   }
   /* load belt and give to guildmaster, then do_give master to monk */
   if (new_belt == 0){
      send_to_char("You have more training to do.\n\r", ch);
      return;
   }
   /* extract the old belt if they got it */
   if (ch->pcdata->extitle == NULL)
   empty_title = TRUE;
   else
   empty_title = FALSE;

   sprintf(save_title, "%s", ch->pcdata->extitle);
   monk_retitle(ch, new_belt);
   obj = get_obj_carry(ch, "monkskillbelt", ch);
   if (obj == NULL){
      obj = get_eq_char(ch, WEAR_WAIST);
   }
   if (obj != NULL && obj->pIndexData->vnum == OBJ_VNUM_MONK_BELT)
   extract_obj(obj, FALSE);
   /* load a new belt and fix it up */
   belt = create_object(get_obj_index(OBJ_VNUM_MONK_BELT), 0);
   cnt = 0;
   while(ch->pcdata->extitle[cnt] != '\0'){
      buf[cnt] = ch->pcdata->extitle[cnt];
      cnt++;
   }
   buf[cnt] = '\0';
   cnt = 2;
   while (buf[cnt] != '\0'){
      buf[cnt-2] = buf[cnt];
      cnt++;
   }
   buf[cnt-2] = '\0';
   free_string(belt->short_descr);
   belt->short_descr = str_dup(buf);
   sprintf(buf, "%s", ch->name);
   free_string(belt->owner);
   belt->owner = str_dup(buf);
   af.where = TO_OBJECT;
   af.type = gsn_reserved;
   af.duration = -1;
   af.bitvector = 0;
   af.level = ch->level;
   af.location = APPLY_HITROLL;
   af.modifier = new_belt + 1;
   if (af.modifier > 6)
   af.modifier = 6;
   affect_to_obj(belt, &af);
   if (new_belt > 2){
      af.location = APPLY_DAMROLL;
      af.modifier = (new_belt)/2;
      if (new_belt == 9)
      af.modifier = 5;
      affect_to_obj(belt, &af);
   }
   if (new_belt == 9){
      af.location = APPLY_SAVES;
      af.modifier = -3;
      affect_to_obj(belt, &af);
   }
   if (new_belt > 2){
      af.location = APPLY_HIT;
      af.modifier = 5 + new_belt*5;
      affect_to_obj(belt, &af);
      af.location = APPLY_AC;
      af.modifier = -2 + -2*new_belt;
      affect_to_obj(belt, &af);
   }

   obj_to_char(belt, ch);
   send_to_char("The guildmaster gives you a new belt.\n\r", ch);

   /* restore their old extitle */
   free_string(ch->pcdata->extitle);
   if (!empty_title)
   ch->pcdata->extitle = str_dup(save_title);
   else
   ch->pcdata->extitle = NULL;
   return;
}

void do_train( CHAR_DATA *ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA *mob;
   sh_int stat = - 1;
   char *pOutput = NULL;
   int cost;

   if ( IS_NPC(ch) )
   return;

   /*
      Check for trainer.
   */
   for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
   {
      if (( IS_NPC(mob) && IS_SET(mob->act, ACT_TRAIN) ) ||
      (!IS_NPC(mob) && IS_SET(mob->act2, PLR_GUILDMASTER)
      && (mob->class == ch->class)))
      break;
   }

   if ( mob == NULL )
   {
      send_to_char( "You can't do that here.\n\r", ch );
      return;
   }

   if (/* !IS_NPC(mob) && */ IS_SET(ch->act2, PLR_GUILDLESS))
   {
      send_to_char( "No guildmaster will train an outcast.\n\r", ch);
      return;
   }

   if ( argument[0] == '\0' )
   {
      sprintf( buf, "You have %d training sessions.\n\r", ch->train );
      send_to_char( buf, ch );
      argument = "foo";
   }

   cost = 1;

   if ( !str_cmp( argument, "str" ) )
   {
      stat        = STAT_STR;
      pOutput     = "strength";
   }

   else if ( !str_cmp( argument, "int" ) )
   {
      stat        = STAT_INT;
      pOutput     = "intelligence";
   }

   else if ( !str_cmp( argument, "wis" ) )
   {
      stat        = STAT_WIS;
      pOutput     = "wisdom";
   }

   else if ( !str_cmp( argument, "dex" ) )
   {
      stat          = STAT_DEX;
      pOutput     = "dexterity";
   }

   else if ( !str_cmp( argument, "con" ) )
   {
      stat        = STAT_CON;
      pOutput     = "constitution";
   }

   else if ( !str_cmp(argument, "hp" ) )
   cost = 1;

   else if ( !str_cmp(argument, "mana" ) )
   cost = 1;

   else
   {
      strcpy( buf, "You can train:" );
      if ( ch->perm_stat[STAT_STR] < get_max_train(ch, STAT_STR))
      strcat( buf, " str" );
      if ( ch->perm_stat[STAT_INT] < get_max_train(ch, STAT_INT))
      strcat( buf, " int" );
      if ( ch->perm_stat[STAT_WIS] < get_max_train(ch, STAT_WIS))
      strcat( buf, " wis" );
      if ( ch->perm_stat[STAT_DEX] < get_max_train(ch, STAT_DEX))
      strcat( buf, " dex" );
      if (( ch->perm_stat[STAT_CON] < get_max_train(ch, STAT_CON))
      && (!IS_SET(ch->act2, PLR_LICH)))
      strcat( buf, " con" );
      strcat( buf, " hp mana");

      if ( buf[strlen(buf)-1] != ':' )
      {
         strcat( buf, ".\n\r" );
         send_to_char( buf, ch );
      }
      else
      {
         act( "You have nothing left to train, you $T!",
         ch, NULL,
         ch->sex == SEX_MALE   ? "big stud" :
         ch->sex == SEX_FEMALE ? "hot babe" :
         "wild thing",
         TO_CHAR );
      }

      return;
   }

   if (!str_cmp("hp", argument))
   {
      if ( cost > ch->train )
      {
         send_to_char( "You don't have enough training sessions.\n\r", ch );
         return;
      }

      ch->train -= cost;
      ch->pcdata->perm_hit += 10;
      ch->max_hit += 10;
      ch->hit +=10;
      act( "Your durability increases!", ch, NULL, NULL, TO_CHAR);
      act( "$n's durability increases!", ch, NULL, NULL, TO_ROOM);
      return;
   }

   if (!str_cmp("mana", argument))
   {
      if ( cost > ch->train )
      {
         send_to_char( "You don't have enough training sessions.\n\r", ch );
         return;
      }

      ch->train -= cost;
      ch->pcdata->perm_mana += 10;
      ch->max_mana += 10;
      ch->mana += 10;
      act( "Your power increases!", ch, NULL, NULL, TO_CHAR);
      act( "$n's power increases!", ch, NULL, NULL, TO_ROOM);
      return;
   }

   if ( ch->perm_stat[stat]  >= get_max_train(ch, stat) )
   {
      act( "Your $T is already at maximum.", ch, NULL, pOutput, TO_CHAR );
      return;
   }

   if ((stat==STAT_CON) && (IS_SET(ch->act2, PLR_LICH)))
   {
      send_to_char("The constitution of a lich cannot be trained.\n\r", ch);
      return;
   }

   if ( cost > ch->train )
   {
      send_to_char( "You don't have enough training sessions.\n\r", ch );
      return;
   }

   ch->train        -= cost;

   ch->perm_stat[stat]        += 1;
   act( "Your $T increases!", ch, NULL, pOutput, TO_CHAR );
   act( "$n's $T increases!", ch, NULL, pOutput, TO_ROOM );
   return;
}



void do_shadowgate(CHAR_DATA *ch, char *argument)
{
   AFFECT_DATA af;
   ROOM_INDEX_DATA *watcher, *sourceroom;
   CHAR_DATA *och, *och_next;
   int camewith = 0;

   if (IS_NPC(ch))
   {
      send_to_char("Mobs don't shadowgate.\n\r", ch);
      return;
   }

   if ( (get_skill(ch, gsn_shadowgate) <= 0)
   || !has_skill(ch, gsn_shadowgate) )
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }

   if (ch->in_room->vnum > 3820 && ch->in_room->vnum < 3826)
   {
      do_portal(ch, "");
      return;
   }
   if (house_down(ch, HOUSE_ANCIENT))
   return;

   if ((watcher = get_room_index(3821)) == NULL) {
      send_to_char("It seems the shadows will not accept you! (pray about a bug)\n\r", ch);
      return;
   }

   if (ch->mana < 100)
   {
      send_to_char("You do not have the mana.\n\r", ch);
      return;
   }

   /*
   if (is_affected(ch, gsn_shadowgate))
   {
      send_to_char("The shadowplane is still realigning for your body.\n\r", ch);
      return;
   }
   */
   if (is_affected(ch, gsn_trapstun))
   {
      send_to_char
      (
         "The snare holds you tight, preventing your passage to the"
         " shadowplanes.\n\r",
         ch
      );
      return;
   }

   if (IS_SET(ch->in_room->extra_room_flags, ROOM_DEAD_ZONE))
   {
      send_to_char("You cannot shadowgate here.\n\r", ch);
      return;
   }

   if
   (
      (number_percent() > ch->pcdata->learned[gsn_shadowgate])
      || (ch->in_room->house != 0 && ch->in_room->house != ch->house)
      ||   ch->in_room->guild != 0
      ||   (ch->in_room->vnum >= 1200 && ch->in_room->vnum <= 1240)
   )
   {
      send_to_char("You fail to pass through the shadowplanes.\n\r", ch);
      check_improve(ch, gsn_shadowgate, FALSE, 1);
      ch->mana -= 50;
      WAIT_STATE(ch, 24);
      return;
   }

   ch->mana -= 100;
   act("$n becomes enshrouded in darkness and disappears!", ch, NULL, NULL, TO_ROOM);

   sourceroom = ch->in_room;
   char_from_room(ch);
   passive_visible(ch);
   char_to_room(ch, watcher);
   send_to_char("You step into the shadowplane and vanish.\n\r", ch);
   act("A vortex of shadows blasts into the room and $n steps forth!", ch, NULL, NULL, TO_ROOM);
   do_observe(ch, "", LOOK_AUTO);
   for ( och = sourceroom->people; och != NULL; och = och_next )
   {
      och_next = och->next_in_room;
      if ( IS_AFFECTED(och, AFF_CHARM)
      &&   och->master == ch
      && (och->in_room != NULL))
      {
         if ( och->fighting != NULL )
         stop_fighting( och, TRUE );
         char_from_room(och);
         char_to_room(och, watcher);
         camewith++;
      }
   }
   if (camewith > 0)
   {
      send_to_char("Your followers faithfully follow.\n\r", ch);
      act("$s followers step forth behind!", ch, NULL, NULL, TO_ROOM);
   }

   af.where=TO_AFFECTS;
   af.type=gsn_shadowgate;
   af.location=0;
   af.bitvector=0;
   af.duration= 1;
   af.modifier=0;
   af.level=ch->level;
   if (is_affected(ch, gsn_shadowgate))
   affect_strip(ch, gsn_shadowgate);
   affect_to_char(ch, &af);

   WAIT_STATE(ch, 24);
   return;
}


void do_bear_call( CHAR_DATA *ch, char *argument)
{
   CHAR_DATA *animal;
   AFFECT_DATA af;
   int a_level;
   char arg1[MAX_INPUT_LENGTH];
   CHAR_DATA *check;
   bool found = FALSE;
   int count;

   one_argument(argument, arg1);

   if ( (get_skill(ch, gsn_bear_call) <= 0)
   || /*(ch->level < skill_table[gsn_bear_call].skill_level[ch->class]) */
   !has_skill(ch, gsn_bear_call))
   {
      send_to_char("You don't know how to call bears?\n\r", ch);
      return;
   }

   if (is_affected(ch, gsn_bear_call))
   {
      send_to_char("You can't call more bears yet.\n\r", ch);
      return;
   }

   if (ch->mana <= 70)
   {
      send_to_char("You do not have the mana.\n\r", ch);
      return;
   }

   for (check = char_list; check != NULL; check = check->next)
   {
      if  ( (check->master == ch)
      && check->pIndexData->vnum == MOB_VNUM_BEAR )
      found = TRUE;
   }

   if (found)
   {
      send_to_char("You already have bears to care for.\n\r", ch);
      return;
   }

   if ( (ch->in_room->sector_type != SECT_FOREST)
   && (ch->in_room->sector_type != SECT_HILLS)
   && (ch->in_room->sector_type != SECT_UNDERGROUND)
   && (ch->in_room->sector_type != SECT_MOUNTAIN) )
   {
      act("$n calls out into the surroundings but nothing comes.", ch, NULL, NULL, TO_ROOM);
      send_to_char("You call a bear call but nothing responds.\n\r", ch);
      return;
   }

   if (!IS_NPC(ch) && (number_percent() > ch->pcdata->learned[gsn_bear_call]))
   {
      send_to_char("You call out for bears but none respond.\n\r", ch);
      check_improve(ch, gsn_bear_call, FALSE, 1);
      ch->mana -= 35;
      return;
   }

   ch->mana -= 70;

   a_level = ch->level;

   act("$n calls out to the wild and is heard!.", ch, NULL, NULL, TO_ROOM);
   send_to_char("Your call to the wild is heard!\n\r", ch);

   for (count = 0; count < 2; count++)
   {
      animal = create_mobile(get_mob_index(MOB_VNUM_BEAR) );
      animal->level = a_level;
      animal->max_hit = ch->max_hit + dice(a_level, 5);
      animal->damroll += a_level*3/4;
      animal->hitroll += a_level/3;
      animal->alignment = ch->alignment;
      char_to_room(animal, ch->in_room);
      SET_BIT(animal->affected_by, AFF_CHARM);
      add_follower(animal, ch);
      animal->leader = ch;
      act("$n arrives to journey with $N.", animal, NULL, ch, TO_NOTVICT);
      animal->hit = animal->max_hit;
   }
   af.where = TO_AFFECTS;
   af.level = ch->level;
   af.modifier = 0;
   af.duration = 24;
   af.bitvector = 0;
   af.location = 0;
   af.type = gsn_bear_call;
   affect_to_char(ch, &af);
   check_improve(ch, gsn_bear_call, TRUE, 1);
   return;
}

void    delete_track( CHAR_DATA *ch)
{
   ROOM_INDEX_DATA *room = ch->in_room;
   int i;

   for (i=0;i<10;i++)
   if (room->track_dir[i][0] == ch->id)
   {
      room->track_dir[i][0] = 0;
      room->track_dir[i][1] = -1;
   }

   return;
}

void    new_track( CHAR_DATA *ch, int direction)
{
   ROOM_INDEX_DATA *room = ch->in_room;
   int i;
   bool found = FALSE;

   if (IS_NPC(ch)) return;

   for (i=0;i<10;i++)
   if (room->track_dir[i][0] == ch->id)
   {
      found = TRUE;
      break;
   }

   if (found)
   {
      room->track_dir[i][1] = direction;
   } else
   {
      room->trackhead = (room->trackhead + 1) % 10;
      room->track_dir[room->trackhead][0] = ch->id;
      room->track_dir[room->trackhead][1] = direction;
   }

   return;
}

int get_trackdir(CHAR_DATA *ch, int ID)
{
   ROOM_INDEX_DATA *room = ch->in_room;
   int i;
   bool found = FALSE;
   int direction = -1;
   CHAR_DATA *victim;

   for (i=0;i<10;i++)
   if
   (
      room->track_dir[i][0] == ID
   )
   {
      found = TRUE;
      break;
   }

   if
   (
      found
   )
   {
      victim = id2name(ID, FALSE);
      if
      (
         victim != NULL &&
         number_percent() < MARAUDER_MISINFORM_CHANCE &&
         is_affected(victim, gsn_misinformation)
      )
      {
         direction = number_range(1, 6);
      }
      else
      {
         direction = room->track_dir[i][1];
      }
   }
   else
   {
      direction = -1;
   }

   return direction;
}

void track_char(CHAR_DATA *ch)
{
   int track_dir;
   ROOM_AFFECT_DATA *raf;
   EXIT_DATA *pexit = NULL;
   int dskip[6] = {0, 0, 0, 0, 0, 0};

   track_dir = get_trackdir(ch, ch->last_fought);
   track_dir -= 1;

   if (IS_AFFECTED(ch, AFF_CHARM))
   return;
   if ( is_affected(ch, gsn_timestop) )
   return;

   act("$n checks the ground for tracks.", ch, NULL, NULL, TO_ROOM);
   if (IS_SET(ch->off_flags, STATIC_TRACKING) || track_dir <= -1)
   return;

   if ((ch == NULL) || (ch->in_room == NULL) ||
   (ch->in_room->exit[track_dir] == NULL) ||
   (ch->in_room->exit[track_dir]->u1.to_room == NULL))
   return;

   if (is_affected_room(ch->in_room, gsn_illusionary_wall))
   {
      raf = ch->in_room->affected;
      while(raf != NULL)
      {
         if (raf->type == gsn_illusionary_wall)
         dskip[raf->modifier] = 1;
         raf = raf->next;
      }
   }
   if (dskip[track_dir] == 1)
   return;
   if (IS_SET(ch->in_room->exit[track_dir]->u1.to_room->room_flags, ROOM_NO_TRACK))
   return;
   pexit = ch->in_room->exit[track_dir];
   if (pexit != NULL && IS_SET(pexit->exit_info, EX_CLOSED) && IS_SET(ch->act2, ACT_INTELLIGENT))
   {
      do_unlock(ch, dir_name[track_dir]);
      if (IS_SET(ch->act, ACT_THIEF))
      do_pick(ch, dir_name[track_dir]);
      do_open(ch, dir_name[track_dir]);
   }
   if (ch->in_room->exit[track_dir]->u1.to_room->area == ch->in_room->area)
   {
      move_char(ch, track_dir, 0);
   }
   if (IS_SET(ch->act2, ACT_RANGER))
   {
      do_camouflage(ch, "");
   }
   return;
}


void do_aura_of_sustenance(CHAR_DATA *ch, char *argument)
{
   AFFECT_DATA af;

   if ((get_skill(ch, gsn_aura_of_sustenance)) <= 0
   || /* ch->level < skill_table[gsn_aura_of_sustenance].skill_level[ch->class]*/
   !has_skill(ch, gsn_aura_of_sustenance))
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }

   if (house_down(ch, HOUSE_LIFE))
   return;

   if (is_affected(ch, gsn_aura_of_sustenance))
   {
      send_to_char("You are already as sustained as you can get.\n\r", ch);
      return;
   }
   if (ch->mana < 40)
   {
      send_to_char("You do not have the mana.\n\r", ch);
      return;
   }

   if (number_percent() > get_skill(ch, gsn_aura_of_sustenance))
   {
      send_to_char("You fail to assume the right trance.\n\r", ch);
      ch->mana -= 20;
      check_improve(ch, gsn_aura_of_sustenance, FALSE, 1);
      return;
   }

   af.where = TO_AFFECTS;
   af.location = 0;
   af.level = ch->level;
   af.duration = ch->level;
   af.bitvector = 0;
   af.type = gsn_aura_of_sustenance;
   af.modifier = 0;
   affect_to_char(ch, &af);
   if (!IS_NPC(ch))
   {
      if (!is_branded_by_lestregus(ch))
      {
         ch->pcdata->condition[COND_THIRST] = 38;
      }
      ch->pcdata->condition[COND_HUNGER] = 38;
   }

   send_to_char("Your mind goes into a trance as you sustain your body in a state of health.\n\r", ch);
   ch->mana -= 40;
   check_improve(ch, gsn_aura_of_sustenance, TRUE, 1);
   return;
}

bool worthytest(CHAR_DATA *ch, char *sk)
{
   sh_int sn;
   if (IS_NPC(ch))
   {
      return FALSE;
   }
   sn = skill_lookup(sk);
   if (ch->pcdata->learnlvl[sn] > ch->level)
   {
      return FALSE;
   }
   return (ch->pcdata->learned[sn] >= 95);
}

void do_vanish(CHAR_DATA *ch, char *argument)
{
   ROOM_INDEX_DATA *pRoomIndex;
   int chance;
   AFFECT_DATA af;

   if ( (chance = get_skill(ch, gsn_vanish)) <= 0
   || /*ch->level < skill_table[gsn_vanish].skill_level[ch->class]*/
   !has_skill(ch, gsn_vanish))
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }

   if (is_affected(ch, gsn_vanish)) {
      send_to_char("You cannot vanish again so soon.\n\r", ch);
      return;
   }

   if (is_affected(ch, gsn_trapstun))
   {
      send_to_char("You can't vanish you're in a snare.\n\r", ch);
      return;
   }

   if (ch->mana < 20)
   {
      send_to_char("You don't have the mana.", ch);
      return;
   }
   if (IS_SET(ch->in_room->extra_room_flags, ROOM_1212))
   {
      send_to_char("You cannot vanish from here.\n\r", ch);
      return;
   }

   if (IS_SET(ch->in_room->extra_room_flags, ROOM_DEAD_ZONE))
   {
      send_to_char("You cannot vanish from here.\n\r", ch);
      return;
   }

   chance -= 5;
   if (number_percent() > chance)
   {
      send_to_char("You attempt to vanish without trace but fail.\n\r", ch);
      act("$n attempts to slide into the shadows but fails.", ch, NULL, NULL, TO_ROOM);
      check_improve(ch, gsn_vanish, FALSE, 2);
      ch->mana -= 10;
      return;
   }
   ch->mana -= 20;
   if (ch->in_room->vnum == 23610
   || IS_SET(ch->in_room->extra_room_flags, ROOM_1212)
   || ch->in_room->vnum == 5400)
   {
      send_to_char("You fail.\n\r", ch);
      return;
   }

   if ( (pRoomIndex = get_random_room(ch, RANDOM_AREA)) == NULL )
   {
      bug("NULL room from get_random_room.", 0);
      return;
   }

   act("$n suddenly vanishes into the shadows!", ch, NULL, NULL, TO_ROOM);
   send_to_char("You slip into the shadows and vanish!\n\r", ch);
   char_from_room(ch);
   char_to_room(ch, pRoomIndex);
   act("$n appears from the shadows.", ch, NULL, NULL, TO_ROOM);
   do_observe(ch, "", LOOK_AUTO);

   af.type=gsn_vanish;
   af.where=TO_AFFECTS;
   af.level=ch->level;
   af.location=0;
   af.duration=8;
   af.modifier=0;
   af.bitvector=0;
   affect_to_char(ch, &af);

   return;
}


void murder_entry_trigger(CHAR_DATA *ch)
{
   char buf[MAX_STRING_LENGTH];
   unsigned int count;
   unsigned int choice;
   unsigned int cnt;
   ROOM_INDEX_DATA* room;
   CHAR_DATA* last;
   CHAR_DATA* aggro;
   CHAR_DATA* aggro_next;
   CHAR_DATA* vch;
   CHAR_DATA* vch_next;
   AFFECT_DATA* paf;

   room = ch->in_room;
   last = ch->next_in_room;  /* stop processing after this one */
   if (IS_SET(room->room_flags, ROOM_SAFE))
   {
      return;
   }
   for (aggro = room->people; aggro; aggro = aggro_next)
   {
      aggro_next = aggro->next_in_room;

      if
      (
         aggro->in_room != room ||  /* sanity check */
         !IS_NPC(aggro) ||
         aggro->fighting != NULL ||
         IS_AFFECTED(aggro, AFF_CALM) ||
         IS_AFFECTED(aggro, AFF_CHARM) ||
         !IS_AWAKE(aggro) ||
         !(
            IS_SET(aggro->act, ACT_AGGRESSIVE) ||
            (
               aggro->last_fought <= 0 &&
               IS_SET(aggro->off_flags, SPAM_MURDER)
            ) ||
            aggro->pIndexData->vnum == MOB_VNUM_DOPPLEGANGER
         )
      )
      {
         continue;
      }
      count = 0;
      for (vch = ch->in_room->people; vch; vch = vch->next_in_room)
      {
         if (vch == last)
         {
            break;
         }
         if
         (
            (
               /* Goods do not attack goods */
               vch->alignment <= 0 ||
               aggro->alignment <= 0
            ) &&
            !IS_IMMORTAL(vch) &&
            vch->in_room == room &&  /* sanity check */
            !IS_NPC(vch) &&
            vch->ghost <= 0 &&
            vch->pcdata->death_status != HAS_DIED &&
            aggro->level >= vch->level - 5 &&
            can_see(aggro, vch) &&
            (
               /* Wimpy aggroes only attack sleepers */
               !IS_SET(aggro->act, ACT_WIMPY) ||
               !IS_AWAKE(vch)
            ) &&
            !is_safe(aggro, vch, IS_SAFE_SILENT)
         )
         {
            count++;
         }
      }
      if (count == 0)
      {
         continue;
      }
      choice = number_range(1, count);
      count = 0;
      for (vch = ch->in_room->people; vch; vch = vch_next)
      {
         vch_next = vch->next_in_room;
         if (vch == last)
         {
            break;
         }
         if
         (
            (
               /* Goods do not attack goods */
               vch->alignment <= 0 ||
               aggro->alignment <= 0
            ) &&
            !IS_IMMORTAL(vch) &&
            vch->in_room == room &&  /* sanity check */
            !IS_NPC(vch) &&
            vch->ghost <= 0 &&
            vch->pcdata->death_status != HAS_DIED &&
            aggro->level >= vch->level - 5 &&
            can_see(aggro, vch) &&
            (
               /* Wimpy aggroes only attack sleepers */
               !IS_SET(aggro->act, ACT_WIMPY) ||
               !IS_AWAKE(vch)
            ) &&
            !is_safe(aggro, vch, IS_SAFE_SILENT) &&
            ++count == choice
         )
         {
            if (aggro->pIndexData->vnum == MOB_VNUM_DOPPLEGANGER)
            {
               /* Remove all affects */
               while (aggro->affected)
               {
                  affect_strip(aggro, aggro->affected->type);
               }
               for (paf = vch->affected; paf != NULL; paf = paf->next)
               {
                  affect_to_char_version(aggro, paf, AFFECT_CURRENT_VERSION);
               }
               act("$n shifts its form suddenly.", aggro, NULL, NULL, TO_ROOM);
               send_to_char("You shift your form!\n\r", aggro);
               sprintf(buf, "doppelganger %s", get_longname(ch, NULL));
               free_string(aggro->name);
               aggro->name = str_dup(buf);
               free_string(aggro->short_descr);
               aggro->short_descr  = str_dup(get_longname(ch, NULL));
               free_string(aggro->long_descr);
               aggro->long_descr   = str_dup(vch->long_descr);
               free_string(aggro->description);
               aggro->description  = str_dup(vch->description);
               aggro->sex          = vch->sex;
               aggro->level        = vch->level;
               aggro->trust        = 0;
               aggro->hit          = vch->hit;
               aggro->max_hit      = vch->max_hit;
               aggro->mana         = vch->mana;
               aggro->max_mana     = vch->max_mana;
               aggro->move         = vch->move;
               aggro->max_move     = vch->max_move;
               aggro->exp          = vch->exp;
               aggro->affected_by  = vch->affected_by;
               aggro->sight        = vch->sight;
               aggro->spell_power  = vch->spell_power;
               aggro->holy_power  = vch->holy_power;
               aggro->saving_throw = vch->saving_throw;
               aggro->saving_spell = vch->saving_spell;
               aggro->saving_breath = vch->saving_breath;
               aggro->saving_maledict = vch->saving_maledict;
               aggro->saving_transport= vch->saving_transport;
               aggro->hitroll      = vch->hitroll;
               aggro->damroll      = vch->damroll;
               aggro->form         = vch->form;
               aggro->parts        = vch->parts;
               aggro->size         = vch->size;
               for (cnt = 0; cnt < 4; cnt++)
               aggro->armor[cnt] = vch->armor[cnt];
               for (cnt = 0; cnt < MAX_STATS; cnt++)
               {
                  aggro->perm_stat[cnt]     = vch->perm_stat[cnt];
                  aggro->mod_stat[cnt]      = vch->mod_stat[cnt];
               }
               for (cnt = 0; cnt < 3; cnt++)
               aggro->damage[cnt]        = vch->damage[cnt];
               if
               (
                  IS_AFFECTED(aggro, AFF_SANCTUARY) &&
                  !is_affected(aggro, gsn_sanctuary) &&
                  !is_affected(aggro, gsn_chromatic_shield)
               )
               {
                  paf = new_affect();
                  paf->where     = TO_AFFECTS;
                  paf->level     = aggro->level;
                  paf->duration  = -1;
                  paf->bitvector = AFF_SANCTUARY;
                  if (is_clergy(aggro))
                  {
                     paf->type = gsn_sanctuary;
                  }
                  else
                  {
                     paf->type = gsn_chromatic_shield;
                  }
                  affect_to_char_1(aggro, paf);
               }
               act
               (
                  "$n grins wickedly and attacks!",
                  aggro,
                  NULL,
                  NULL,
                  TO_ROOM
               );
               send_to_char("You grin wickedly and attack.\n\r", aggro);
               do_myell(vch, "Ack! A doppelganger!");
               switch (number_range(0, 7))
               {
                  default:
                  {
                     do_yell(aggro, "No, no, no, YOU are the doppelganger!");
                     break;
                  }
                  case (1):
                  {
                     do_yell(aggro, "Who, me?");
                     break;
                  }
                  case (2):
                  {
                     do_yell(aggro, "So, what's it like to fight yourself, mortal?");
                     break;
                  }
                  case (3):
                  {
                     do_yell(aggro, "Wow, you're ugly! And so am I!");
                     break;
                  }
                  case (4):
                  {
                     do_yell(aggro, "No wait, my mistake. Just my reflection.");
                     break;
                  }
                  case (5):
                  {
                     do_yell(aggro, "Yeah, a doppelganger! Everyone please kill the one with my name!");
                     break;
                  }
                  case (6):
                  {
                     do_yell(aggro, "No, I was wrong. Go away. Nobody help me.");
                     break;
                  }
                  case (7):
                  {
                     do_yell(aggro, "Actually, I'm going insane and killing myself.");
                     break;
                  }
               }
            }
            else if (IS_SET(aggro->off_flags, SPAM_MURDER))
            {
               sprintf
               (
                  buf,
                  "Help! I'm being attacked by %s!",
                  aggro->short_descr
               );
               do_myell(vch, buf);
            }
            multi_hit(aggro, vch, TYPE_UNDEFINED);
            break;
         }
      }
   }
   return;
}

/* MOBProgs for House Guardians */
void house_entry_trigger(CHAR_DATA *ch, bool update)
{
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA* protector;
   int house;

   if
   (
      IS_NPC(ch) ||
      !ch->in_room ||
      !(
         house = ch->in_room->house
      ) ||
      (
         ch->house != house &&
         IS_SET(ch->pcdata->allied_with, 1 << house)
      ) ||
      IS_IMMORTAL(ch)
   )
   {
      return;
   }
   for (protector = ch->in_room->people; protector != NULL; protector =  protector->next_in_room)
   {
      if
      (
         !IS_NPC(protector) ||
         protector->house != house
      )
      {
         continue;
      }
      break;
   }
   if (protector == NULL)
   {
      return;
   }
   if (ch->house == protector->house)
   {
      if (update)
      {
         return;
      }
      if (house_table[ch->house].guardian_greeting[0] != '\0')
      {
         do_say(protector, house_table[ch->house].guardian_greeting);
      }
      if (house_table[protector->house].greeting_emote[0] != '\0')
      {
         do_emote(protector, house_table[protector->house].greeting_emote);
      }
      return;
   }
   else
   {
      un_gaseous(ch);
      if (ch->ghost > 0)
      {
         do_say(protector, "Ghosts aren't allowed in here!");
         mob_slay(protector, ch);
         return;
      }
      if (is_safe(protector, ch, IS_SAFE_SILENT))
      {
         return;
      }
   }


   sprintf(buf, "Help! I'm being attacked by %s!", protector->short_descr);
   do_yell(ch, buf);

   do_housetalk(protector, house_table[protector->house].raid_ht);
   if (house_table[protector->house].raid_say[0] != '\0')
   {
      do_say(protector, house_table[protector->house].raid_say);
   }

   multi_hit(protector, ch, TYPE_UNDEFINED);
   return;
}

void house_key_trigger(CHAR_DATA *ch, CHAR_DATA *statue, OBJ_DATA *key)
{
   char buf[MAX_STRING_LENGTH];
   OBJ_DATA* book;
   OBJ_INDEX_DATA* index;
   CHAR_DATA *hch;
   bool is_gone;
   OBJ_DATA *check;
   int house;
   sh_int vnum;
   char* format;

   for (house = 0; house < MAX_HOUSE; house++)
   {
      if (key->pIndexData->vnum == house_table[house].vnum_key)
      {
         break;
      }
   }

   if
   (
      house == MAX_HOUSE ||
      !IS_NPC(statue) ||
      statue->pIndexData->vnum != house_table[house].vnum_statue ||
      (
         index = get_obj_index(vnum = house_table[house].vnum_item)
      ) == NULL
   )
   {
      return;
   }

   is_gone = FALSE;
   if (index->in_game_count > 0)
   {
      for (check = object_list; check != NULL; check = check->next)
      {
         if (check->pIndexData->vnum == vnum)
         {
            if (check->carried_by == statue)
            {
               extract_obj(check, FALSE);
            }
            else
            {
               is_gone = TRUE;
            }
         }
      }
   }

   book = create_object(index, 0);
   if (book == NULL)
   {
      return;
   }

   if (is_gone)
   {
      if
      (
         ch->house == house ||
         (
            house == HOUSE_ANCIENT &&
            IS_SET(ch->act2, PLR_IS_ANCIENT_KNOWN)
         )
      )
      {
         if
         (
            (
               format = house_table[house].item_exists_house_say
            ) == NULL
         )
         {
            format = "You are too late! $p has been stolen!";
         }
         if (format[0] != '\0')
         {
            sprintf(buf, "$n says '{B{6%s{n'", format);
            act_color(buf, statue, book, ch, TO_ROOM);
         }
      }
      else if (IS_SET(ch->pcdata->allied_with, 1 << house))
      {
         if
         (
            (
               format = house_table[house].item_exists_ally_say
            ) == NULL
         )
         {
            format = "Thank you but $p is already gone.";
         }
         if (format[0] != '\0')
         {
            sprintf(buf, "$n says '{B{6%s{n'", format);
            act_color(buf, statue, book, ch, TO_ROOM);
         }
      }
      else
      {
         if
         (
            (
               format = house_table[house].item_exists_enemy_say
            ) == NULL
         )
         {
            format = "$p has already been stolen, search elsewhere thief!";
         }
         if (format[0] != '\0')
         {
            sprintf(buf, "$n says '{B{6%s{n'", format);
            act_color(buf, statue, book, ch, TO_ROOM);
         }
      }
      extract_obj(key, FALSE);
      extract_obj(book, FALSE);
      return;
   }


   if
   (
      ch->house == house ||
      (
         house == HOUSE_ANCIENT &&
         IS_SET(ch->act2, PLR_IS_ANCIENT_KNOWN)
      )
   )
   {
      if
      (
         (
            format = house_table[house].give_house_say
         ) == NULL
      )
      {
         format = "To the holder of the key I give $p.";
      }
      if (format[0] != '\0')
      {
         sprintf(buf, "$n says '{B{6%s{n'", format);
         act_color(buf, statue, book, ch, TO_ROOM);
      }
   }
   else if (IS_SET(ch->pcdata->allied_with, 1 << house))
   {
      if
      (
         (
            format = house_table[house].give_ally_say
         ) == NULL
      )
      {
         format = "To the holder of the key I give $p.";
      }
      if (format[0] != '\0')
      {
         sprintf(buf, "$n says '{B{6%s{n'", format);
         act_color(buf, statue, book, ch, TO_ROOM);
      }
   }
   else
   {
      if
      (
         (
            format = house_table[house].give_enemy_say
         ) == NULL
      )
      {
         format = "To the holder of the key I give $p.";
      }
      if (format[0] != '\0')
      {
         sprintf(buf, "$n says '{B{6%s{n'", format);
         act_color(buf, statue, book, ch, TO_ROOM);
      }
   }


   if
   (
      ch->house == house ||
      (
         house == HOUSE_ANCIENT &&
         IS_SET(ch->act2, PLR_IS_ANCIENT_KNOWN)
      ) ||
      house_member_on(house)
   )
   {
      raids_defended[house]++;
   }
   else
   {
      raids_nodefender[house]++;
   }

   obj_to_char(book, statue);
   sprintf
   (
      buf,
      "\"%s\" \"%c%s\"",
      book->name,
      (
         IS_NPC(ch) ?
         '-' :
         '+'
      ),
      get_name(ch, statue)
   );
   do_give(statue, buf);
   if (book->carried_by == statue)
   {
      extract_obj(book, FALSE);
      sprintf(buf, "The key is worthless if you cannot hold %s.", index->short_descr);
      do_say(statue, buf);
      do_drop(statue, key->name);
      if (key->carried_by == statue)
      {
         extract_obj(key, FALSE);
      }
      return;
   }
   extract_obj(key, FALSE);

   if
   (
      ch->house == house ||
      (
         house == HOUSE_ANCIENT &&
         IS_SET(ch->act2, PLR_IS_ANCIENT_KNOWN)
      )
   )
   {
      format = NULL;
   }
   else if (IS_SET(ch->pcdata->allied_with, 1 << house))
   {
      format = "%s%s: %s has obtained %s!\n\r";
   }
   else
   {
      format = "%s%s: %s has stolen %s!\n\r";
   }
   if (format != NULL)
   {
      sprintf
      (
         buf,
         format,
         house_table[house].who_name,
         statue->short_descr,
         PERS(ch, NULL),
         book->short_descr
      );
      for (hch = char_list; hch != NULL; hch = hch->next)
      {
         if
         (
            hch->house == house ||
            (
               house == HOUSE_ANCIENT &&
               IS_SET(ch->act2, PLR_IS_ANCIENT_KNOWN)
            )
         )
         {
            send_to_char(buf, hch);
         }
      }
   }
   return;
}

void do_forest_blending(CHAR_DATA *ch, char *argument)
{
   int chance, sn_fog, sn_fire;
   AFFECT_DATA af;

   chance = get_skill(ch, gsn_forest_blending);
   if (chance < 5 || !has_skill(ch, gsn_forest_blending))
   {
      send_to_char("You don't know how to blend in with the forests.\n\r", ch);
      return;
   }

   sn_fog = gsn_faerie_fog;
   sn_fire = gsn_faerie_fire;
   if (is_affected(ch, sn_fog) || is_affected(ch, sn_fire))
   {
      send_to_char("You can't blend in with your surroundings while glowing.\n\r", ch);
      return;
   }

   if (ch->in_room->sector_type != SECT_FOREST)
   {
      send_to_char("You aren't within a suitable forest environment.\n\r", ch);
      return;
   }

   if (is_affected(ch, gsn_forest_blending))
   {
      send_to_char("You're already trying to blend in with your surroundings.\n\r", ch);
      return;
   }

   send_to_char("You attempt to blend in with the forests.\n\r", ch);
   if (number_percent() < chance)
   {
      check_improve(ch, gsn_forest_blending, TRUE, 1);
      af.where = TO_AFFECTS;
      af.level = ch->level;
      af.modifier = 0;
      af.location = 0;
      af.bitvector = 0;
      af.duration = 5;
      af.type = gsn_forest_blending;
      affect_to_char(ch, &af);
   }
   else
   {
      check_improve(ch, gsn_forest_blending, FALSE, 1);
      affect_strip(ch, gsn_forest_blending);
   }

   WAIT_STATE(ch, 6);
   return;
}

void un_forest_blend(CHAR_DATA *ch)
{
   if (!is_affected(ch, gsn_forest_blending))
   return;
   act("$n steps into the open from the surrounding forest.", ch, NULL, NULL, TO_ROOM);
   act("You step into the open from the surrounding forest.", ch, NULL, NULL, TO_CHAR);
   affect_strip(ch, gsn_forest_blending);
   return;
}

void do_awareness(CHAR_DATA *ch, char *argument)
{
   int chance;
   AFFECT_DATA af;

   if (IS_AFFECTED2(ch, AFF_AWARENESS))
   {
      send_to_char("You are already alert to the forests.\n\r", ch);
      return;
   }
   chance = get_skill(ch, gsn_awareness);
   if (chance <= 0
   || /*ch->level < skill_table[gsn_awareness].skill_level[ch->class]*/
   !has_skill(ch, gsn_awareness))
   {
      send_to_char("You don't know which part of the forests to be alert to.\n\r", ch);
      return;
   }
   if (ch->mana < 10)
   {
      send_to_char("You don't have the mana.\n\r", ch);
      return;
   }

   if (number_percent() > chance)
   {
      send_to_char("You try to watch your surroundings but become distracted.\n\r", ch);
      check_improve(ch, gsn_awareness, FALSE, 1);
      ch->mana -= 5;
      return;
   }
   send_to_char("You start watching your surroundings more carefully.\n\r", ch);
   check_improve(ch, gsn_awareness, TRUE, 1);
   ch->mana -= 10;
   af.where = TO_AFFECTS;
   af.location = 0;
   af.modifier = 0;
   af.type = gsn_awareness;
   af.bitvector = 0;
   af.bitvector2 = AFF_AWARENESS;
   af.level = ch->level;
   af.duration = ch->level;
   affect_to_char_1(ch, &af);
   return;
}

/*
   This ethereal ability lets a player pass through a room in gaseous form.
   In this state they can pass aggro and tracking mobs if they make a check
   while passing through the room. House guardians are never passed, nor are
   aggros who's level is more than 10 above the player's.
   gaseous <dir1> <dir2>
   Sends character into room in <dir1> then into room at <dir2> from there.
   if only 1 direction is used the player will automatically try to go
   through the same direction two time..ie gaseous n   ... player will try to
   go north into next room north, then north again from there.
   Followers still follow the player and will be hit by aggros/tracking mobs.
*/
void do_gaseous_form(CHAR_DATA *ch, char *argument)
{
   int dir1, dir2, chance;
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   AFFECT_DATA af;

   chance = get_skill(ch, gsn_gaseous_form);
   dir1 = -1;
   dir2 = -1;

   argument = one_argument(argument, arg1);
   one_argument(argument, arg2);

   if (chance < 1
   || /* ch->level < skill_table[gsn_gaseous_form].skill_level[ch->class] */
   !has_skill(ch, gsn_gaseous_form))
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }

   if (is_affected(ch, gsn_cloak_form)) {
      send_to_char("Your disguise would be lost in the process.\n\r", ch);
      return;
   }

   if (arg1[0] == '\0')
   {
      send_to_char("Attempt to go through which direction in gaseous form?\n\r", ch);
      return;
   }

   if (!str_cmp(arg1, "n") || !str_cmp(arg1, "north"))
   dir1 = 0;
   else if (!str_cmp(arg1, "e") || !str_cmp(arg1, "east"))
   dir1 = 1;
   else if (!str_cmp(arg1, "s") || !str_cmp(arg1, "south"))
   dir1 = 2;
   else if (!str_cmp(arg1, "w") || !str_cmp(arg1, "west"))
   dir1 = 3;
   else if (!str_cmp(arg1, "u") || !str_cmp(arg1, "up"))
   dir1 = 4;
   else if (!str_cmp(arg1, "d") || !str_cmp(arg1, "down"))
   dir1 = 5;

   if (dir1 == -1)
   {
      send_to_char("Which direction do you want to try to pass through in gaseous form?\n\r", ch);
      return;
   }

   if (arg2[0] == '\0')
   dir2 = dir1;
   else if (!str_cmp(arg2, "n") || !str_cmp(arg2, "north"))
   dir2 = 0;
   else if (!str_cmp(arg2, "e") || !str_cmp(arg2, "east"))
   dir2 = 1;
   else if (!str_cmp(arg2, "s") || !str_cmp(arg2, "south"))
   dir2 = 2;
   else if (!str_cmp(arg2, "w") || !str_cmp(arg2, "west"))
   dir2 = 3;
   else if (!str_cmp(arg2, "u") || !str_cmp(arg2, "up"))
   dir2 = 4;
   else if (!str_cmp(arg2, "d") || !str_cmp(arg2, "down"))
   dir2 = 5;

   if (ch->mana < 10)
   {
      send_to_char("You don't have the mana.\n\r", ch);
      return;
   }
   if (ch->move < 20)
   {
      send_to_char("You don't have the movement.\n\r", ch);
      return;
   }
   if (number_percent() > chance)
   {
      send_to_char("You try to force your body into its lower form of gas but fail.\n\r", ch);
      check_improve(ch, gsn_gaseous_form, FALSE, 1);
      ch->mana -= 5;
      return;
   }

   if (is_affected(ch, gsn_shapeshift))
   {
      act("You come out of your shapeshifted form.", ch, NULL, NULL, TO_CHAR);
      affect_strip(ch, gsn_shapeshift);
   }

   if (is_affected(ch, gsn_strange_form))
   {
      act("You cannot force your body into a lower form of gas for some reason.", ch, NULL, NULL, TO_CHAR);
      return;
   }

   af.where = TO_AFFECTS;
   af.type = gsn_gaseous_form;
   af.duration = 1;
   af.location = APPLY_MORPH_FORM;
   af.modifier = MORPH_GASEOUS;
   af.bitvector = 0;
   af.level = ch->level;
   affect_to_char(ch, &af);

   ch->mana -= 10;
   act("$n suddenly vaporises into a cloud of boiling gas!", ch, NULL, NULL, TO_ROOM);
   send_to_char("You force your body into a cloud of boiling gas!\n\r", ch);
   check_improve(ch, gsn_gaseous_form, TRUE, 1);
   WAIT_STATE(ch, 12);

   ch->move -= 10;
   move_char(ch, dir1, 0);
   if (!is_affected(ch, gsn_gaseous_form))
   return;
   move_char(ch, dir2, 0);
   if (!is_affected(ch, gsn_gaseous_form))
   return;
   un_gaseous(ch);
}

void un_gaseous(CHAR_DATA *ch)
{
   if (!is_affected(ch, gsn_gaseous_form))
   return;
   act("$n forms again as $s body solidifies from gaseous form.", ch, NULL, NULL, TO_ROOM);
   send_to_char("You reform into your normal body as you leave gaseous form.\n\r", ch);
   affect_strip(ch, gsn_gaseous_form);
   return;
}

/* Door bash for giants/trolls/centaurs */

/*
   In move_char:
   if (is_affected(ch, gsn_door_bash))
   act("$n goes crashing through the door $T.", ch, NULL, dir_name[door], TO_ROOM);
   else


   if (is_affected(ch, gsn_door_bash))
   act("The $T door bursts open and $n comes crashing in!", ch, NULL, dir_name[door], TO_ROOM);
*/

void do_door_bash(CHAR_DATA *ch, char *argument)
{
   char arg[MAX_INPUT_LENGTH];
   int door;
   int chance;
   AFFECT_DATA af;

   one_argument(argument, arg);
   if (arg[0] == '\0')
   {
      send_to_char("Bash door which door?\n\r", ch);
      return;
   }
   chance = get_skill(ch, gsn_door_bash);
   if (chance <= 0
   || /* ch->level < skill_table[gsn_door_bash].skill_level[ch->class] */
   !has_skill(ch, gsn_door_bash))
   {
      send_to_char("You'd hurt yourself doing that.\n\r", ch);
      return;
   }
   if (ch->move < 5)
   {
      send_to_char("You are too exhausted.\n\r", ch);
      return;
   }

   if ( (door = find_door(ch, arg) ) >= 0)
   {
      ROOM_INDEX_DATA *to_room;
      EXIT_DATA *pexit;
      EXIT_DATA *pexit_rev;
      pexit = ch->in_room->exit[door];
      if (!IS_SET(pexit->exit_info, EX_CLOSED))
      {
         send_to_char("It's not closed.\n\r", ch);
         return;
      }

      if (!IS_SET(pexit->exit_info, EX_LOCKED))
      {
         send_to_char("It's already unlocked, why not just use the knob?\n\r", ch);
         return;
      }

      chance /= 4;
      chance += ch->level/5;
      chance += get_curr_stat(ch, STAT_STR)/2;

      if (ch->race == grn_giant)
      chance += 15;

      do_visible(ch, "");
      if (number_percent() > chance || IS_SET(pexit->exit_info, EX_NOBASH) )
      {
         act("$n flies into the $T door and rebounds with a great lack of dignity!", ch, NULL, dir_name[door], TO_ROOM);
         act("You fly into the door $T but simply bounce off it like a lump of rock!", ch, NULL, dir_name[door], TO_CHAR);
         damage(ch, ch, dice(3, 5), gsn_door_bash, DAM_BASH, TRUE);
         check_improve(ch, gsn_door_bash, FALSE, 1);
         WAIT_STATE(ch, 12);
         return;
      }
      act("$n slams into the $T door and throws it open with a mighty crash!", ch, NULL, dir_name[door], TO_ROOM);
      act("You slam into the $T door and it cracks open with a deafening sound!", ch, NULL, dir_name[door], TO_CHAR);
      check_improve(ch, gsn_door_bash, TRUE, 1);
      WAIT_STATE(ch, 12);
      REMOVE_BIT(pexit->exit_info, EX_LOCKED);
      REMOVE_BIT(pexit->exit_info, EX_CLOSED);

      if ( ( (to_room = pexit->u1.to_room) != NULL)
      && ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
      && pexit_rev->u1.to_room == ch->in_room)
      {
         REMOVE_BIT(pexit_rev->exit_info, EX_LOCKED);
         REMOVE_BIT(pexit_rev->exit_info, EX_CLOSED);
      }

      if (number_percent() < get_curr_stat(ch, STAT_DEX))
      return;

      /*
         Affect to char so in move_char you relay the right move message, then
         strip affect once moved.
      */
      af.where = TO_AFFECTS;
      af.type = gsn_door_bash;
      af.location = 0;
      af.modifier = 0;
      af.duration = -1;
      af.bitvector = 0;
      af.level = ch->level;
      affect_to_char(ch, &af);

      move_char(ch, door, 0);
      affect_strip(ch, gsn_door_bash);
   }

   return;
}


/* For selecting new home */
/* this is almost totaly recoded 11-8-99 -werv */

void do_hometown(CHAR_DATA *ch, char *argument)
{
   int align;
   int cost;
   int hometown;
   int ht_choice;
   char arg[MAX_STRING_LENGTH];
   char buf[MAX_STRING_LENGTH];

   if (IS_NPC(ch))
   return;

   if (ch->in_room->vnum != ROOM_VNUM_HOMETOWNS)
   {
      send_to_char("You can't do that here.\n\r", ch);
      return;
   }

   one_argument(argument, arg);

   align = ch->alignment;

   /* show options */
   if (arg[0] == '\0' || !str_cmp(arg, "list")){
      hometown = 0;
      send_to_char("Hometowns available to you:\n\r", ch);
      while(hometown_table[hometown].vnum != 0)
      {
         if (hometown_table[hometown].vnum != ch->temple)
         if ((align == 1000 && hometown_table[hometown].allow_good) ||
         (align == 0    && hometown_table[hometown].allow_neutral) ||
         (align == -1000 && hometown_table[hometown].allow_evil)){
            sprintf(buf, "%-20s (%-4s) \t%d\n\r", hometown_table[hometown].name,
            hometown_table[hometown].abr_name, hometown_table[hometown].cost
            *
            ch->level);
            send_to_char(buf, ch);
         }
         hometown++;
      }
      return;
   }

   /* figure out which they tried to choose */
   hometown = 0;
   ht_choice = -1;
   while(hometown_table[hometown].vnum != 0)
   {
      if ((align == 1000 && hometown_table[hometown].allow_good) ||
      (align == 0    && hometown_table[hometown].allow_neutral) ||
      (align == -1000 && hometown_table[hometown].allow_evil)){
         if (!str_cmp(hometown_table[hometown].name, arg) ||
         !str_cmp(hometown_table[hometown].abr_name, arg))
         ht_choice = hometown;
      }
      hometown++;
   }
   if (ht_choice == -1)
   {
      send_to_char("That's not a hometown choice for you.\n\r", ch);
      return;
   }
   cost = hometown_table[ht_choice].cost * ch->level;
   if (ch->gold < cost)
   {
      send_to_char("You have to be able to pay in gold.\n\r", ch);
      return;
   }
   ch->gold -= cost;
   ch->temple = hometown_table[ht_choice].vnum;
   sprintf(buf, "Your home town is now %s.\n\r", hometown_table[ht_choice].name);
   send_to_char(buf, ch);
   return;
}


/* mob prog */

void move_prog_bones(CHAR_DATA *ch)
{
   CHAR_DATA *bone1;
   CHAR_DATA *bone2;
   CHAR_DATA *bone3;
   CHAR_DATA *bone4;
   int num;
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA *vch1;
   CHAR_DATA *vch2;
   CHAR_DATA *vch3;

   if (ch->in_room->vnum != 13868
   && ch->in_room->vnum != 13869
   && ch->in_room->vnum != 13871
   && ch->in_room->vnum != 13872
   && ch->in_room->vnum != 13884
   && ch->in_room->vnum != 13873)
   return;
   if (number_percent() > 20)
   return;
   bone1 = create_mobile(get_mob_index(MOB_VNUM_BONES));
   if (bone1 == NULL)
   return;

   act("The bones around you suddenly stir and rise up!", ch, NULL, NULL, TO_CHAR);
   act("The bones around you suddenly stir and rise up!", ch, NULL, NULL, TO_ROOM);

   switch(number_range(0, 5))
   {
      case 1:
      sprintf(buf, "Die, thou who invades our rest!"); break;
      case 2:
      sprintf(buf, "%s, you shall pay for your desecration of our bones!", ch->name);
      break;
      case 3:
      sprintf(buf, "Flesh...flesh for us at last...");break;
      case 4:
      sprintf(buf, "I feel the warmth of life...give me your body so I may live!");
      break;
      case 5:
      sprintf(buf, "I sense your warm blood...let me feed...");
      break;
   }

   for (vch1 = ch->in_room->people; vch1 != NULL; vch1 = vch1->next_in_room)
   {
      if (is_same_group(vch1, ch) && number_bits(2) == 0)
      break;
   }
   if (vch1 == NULL)
   vch1 = ch;

   for (vch2 = ch->in_room->people; vch2 != NULL; vch2 = vch2->next_in_room)
   {
      if (is_same_group(vch2, ch) && number_bits(2) == 0)
      break;
   }
   if (vch2 == NULL)
   vch2 = ch;

   for (vch3 = ch->in_room->people; vch3 != NULL; vch3 = vch3->next_in_room)
   {
      if (is_same_group(vch3, ch) && number_bits(2) == 0)
      break;
   }
   if (vch3 == NULL)
   vch3 = ch;

   num = number_range(1, 4);

   char_to_room(bone1, ch->in_room);

   do_say(bone1, buf);
   multi_hit(bone1, ch, TYPE_UNDEFINED);

   if (num >= 2)
   {
      bone2 = create_mobile(get_mob_index(MOB_VNUM_BONES));
      char_to_room(bone2, ch->in_room);
      act("$n rises up and attacks!", bone2, NULL, NULL, TO_ROOM);
      multi_hit(bone2, vch1, TYPE_UNDEFINED);
   }
   if (num >= 3)
   {
      bone3 = create_mobile(get_mob_index(MOB_VNUM_BONES));
      char_to_room(bone3, ch->in_room);
      act("$n rises up and attacks!", bone3, NULL, NULL, TO_ROOM);
      multi_hit(bone3, vch2, TYPE_UNDEFINED);
   }
   if (num >= 4)
   {
      bone4 = create_mobile(get_mob_index(MOB_VNUM_BONES));
      char_to_room(bone4, ch->in_room);
      act("$n rises up and attacks!", bone4, NULL, NULL, TO_ROOM);
      multi_hit(bone4, vch3, TYPE_UNDEFINED);
   }
   return;
}

void move_prog_hydra(CHAR_DATA *ch)
{
   CHAR_DATA *hydra;
   CHAR_DATA *vch;
   char buf[MAX_STRING_LENGTH];

   if (IS_NPC(ch))
   return;
   if (ch->in_room->vnum != 13874)
   return;

   for (hydra = char_list; hydra != NULL; hydra = hydra->next)
   {
      if (!IS_NPC(hydra)) continue;
      if (hydra->pIndexData->vnum == MOB_VNUM_HYDRA)
      break;
   }
   if (hydra != NULL)
   return;
   hydra = create_mobile(get_mob_index(MOB_VNUM_HYDRA));
   if (hydra == NULL)
   return;
   char_to_room(hydra, ch->in_room);

   act("The broken wood of the ships explodes as $n rises up in fury!", hydra, NULL, NULL, TO_ROOM);


   for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
   {
      if (is_same_group(vch, ch) && number_bits(2) == 0)
      break;
   }

   if (vch == NULL)
   vch = ch;

   sprintf(buf, "Help! %s is attacking me!", PERS(hydra, vch));
   do_yell(vch, buf);
   multi_hit(hydra, vch, TYPE_UNDEFINED);
   return;
}


void mob_entry_wraith(CHAR_DATA *ch)
{
   CHAR_DATA *wraith;

   if (IS_NPC(ch))
   {
      return;
   }
   if
   (
      IS_IMMORTAL(ch) ||
      str_cmp(ch->in_room->area->name, "mythforest.are") ||
      number_percent() > 4
   )
   {
      return;
   }
   wraith = create_mobile(get_mob_index(13732));
   if (wraith == NULL)
   {
      return;
   }
   char_to_room(wraith, ch->in_room);
   act("A shimmering wraith rises from the damp forest ground in a swirling mist.", wraith, NULL, NULL, TO_ROOM);
   do_yell(ch, "Help! I'm being attacked by a mist wraith!");
   multi_hit(wraith, ch, TYPE_UNDEFINED);
   return;
}

/* This is for riallus's evil forest mobs */
void mob_entry_evil_area(CHAR_DATA* ch)
{
   int count;
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA* mob;
   MOB_INDEX_DATA* mob_index;
   OBJ_DATA* obj;
   OBJ_INDEX_DATA* obj_index = NULL;

   if
   (
      IS_NPC(ch) ||
      str_cmp(ch->in_room->area->name, "Forest of Darkness") ||
      number_percent() > 15 ||
      IS_IMMORTAL(ch)
   )
   {
      return;
   }

   count = number_range(0, 8);
   obj = NULL;
   if (count <= 3)
   {
      mob_index = get_mob_index(6400);
      obj_index = get_obj_index(6400);
   }
   else if (count <= 6)
   {
      mob_index = get_mob_index(6401);
      if (number_percent() > 80)
      {
         obj_index = get_obj_index(6401);
      }
   }
   else
   {
      mob_index = get_mob_index(6405);
   }


   if (mob_index == NULL)
   {
      return;
   }

   count = mob_index->count;

   if
   (
      (
         mob_index->vnum == 6400 &&
         count >= 8
      ) ||
      (
         mob_index->vnum == 6401 &&
         count >= 5
      ) ||
      (
         mob_index->vnum == 6405 &&
         count >= 5
      )
   )
   {
      return;
   }
   mob = create_mobile(mob_index);
   if (mob == NULL)
   {
      return;
   }

   char_to_room(mob, ch->in_room);
   if
   (
      obj_index != NULL &&
      (
         obj = create_object(obj_index, 20)
      ) != NULL
   )
   {
      obj_to_char(obj, mob);
   }
   switch (mob_index->vnum)
   {
      default:
      {
         act
         (
            "$n pushes through the ground and grabs at you!",
            mob,
            NULL,
            ch,
            TO_VICT
         );
         act
         (
            "$n pushes through the ground and grabs at $N!",
            mob,
            NULL,
            ch,
            TO_NOTVICT
         );
         break;
      }
      case (6400):
      {
         act
         (
            "$n rises up from the ground in an explosion of dirt!",
            mob,
            NULL,
            NULL,
            TO_ROOM
         );
         if (obj != NULL)
         {
            do_wear(mob, obj->name);
         }
         break;
      }
      case (6401):
      {
         act
         (
            "$n swoops in from the darkened air above you!",
            mob,
            NULL,
            ch,
            TO_VICT
         );
         act
         (
            "$n swoops in from the darkened air above $N!",
            mob,
            NULL,
            ch,
            TO_NOTVICT
         );
         break;
      }
   }

   sprintf(buf, "Help! I'm being attacked by %s!", PERS(mob, ch));
   do_yell(ch, buf);

   multi_hit(mob, ch, TYPE_UNDEFINED);
   return;
}

/* This is for Xurinos's haunted mine spirits */
void mob_entry_haunted_mine(CHAR_DATA* ch)
{
   const sh_int k_spirit_vnum        = 30001;  /* ghost miner */
   const sh_int k_starting_room_vnum = 30004;  /* a few steps into the mine */
   const sh_int k_ending_room_vnum   = 30118;  /* last room in the mine */

   MOB_INDEX_DATA* spirit_data;
   CHAR_DATA* spirit;
   char myell_text[MAX_STRING_LENGTH];

   if
   (
      !ch ||
      IS_NPC(ch) ||
      !ch->in_room ||
      ch->in_room->vnum < k_starting_room_vnum ||
      ch->in_room->vnum > k_ending_room_vnum ||
      number_percent() > 15 ||
      IS_IMMORTAL(ch)
   )
   {
      return;
   }

   spirit_data = get_mob_index(k_spirit_vnum);

   if (
         spirit_data == NULL ||   /* spirit does not exist */
         spirit_data->count > 10  /* 10 spirits wander */
      )
   {
      return;
   }

   spirit = create_mobile(spirit_data);
   if (spirit == NULL)
   {
      return;
   }

   char_to_room(spirit, ch->in_room);
   act
   (
      "$n drifts into the room and spots you.",
      spirit,
      NULL,
      ch,
      TO_VICT
   );
   act
   (
      "$n drifts in the room and eyes $N.",
      spirit,
      NULL,
      ch,
      TO_NOTVICT
   );

   switch (number_range(1,8))
   {
      case 1:
         do_say(spirit, "Leave!");
         break;
      case 2:
         do_say(spirit, "Leave now, ye fool!");
         break;
      case 3:
         do_say(spirit, "Heed our warning, an' leave now!");
         break;
      case 4:
         do_say(spirit, "Leave before it be too late!");
         break;
      case 5:
         do_say(spirit, "Go no further, ye blasted fool!");
         break;
      case 6:
         do_say(spirit, "Continue, and die!");
         break;
      case 7:
         do_say(spirit, "Ye don'ta know what yer doin'!  Get out!");
         break;
      default:
         do_say(spirit, "If ye value yer skin, ye'd be leavin' now!");
   }

   act
   (
      "$n is attacking you!",
      spirit,
      NULL,
      ch,
      TO_VICT
   );
   act
   (
      "$n is attacking $N!",
      spirit,
      NULL,
      ch,
      TO_NOTVICT
   );

   sprintf(myell_text, "Help! I'm being attacked by %s!", PERS(spirit, ch));
   do_myell(ch, myell_text);

   multi_hit(spirit, ch, TYPE_UNDEFINED);
}


bool bloody_shrine_entry(CHAR_DATA *ch, ROOM_INDEX_DATA *to_room)
{
   if (IS_NPC(ch))
   return FALSE;
   if (str_cmp(to_room->area->name, "Thryms Wonderful World of Herbal Surprises")
   && str_cmp(to_room->area->name, "Shrine of Dominance")
   && str_cmp(to_room->area->name, "Shrine of Peace")
   && str_cmp(to_room->area->name, "Den of Delusion")
   && str_cmp(to_room->area->name, "Shrine of Freedom")
   && str_cmp(to_room->area->name, "The Hall of Corruption")
   && str_cmp(to_room->area->name, "Shrine of Magic"))
   return FALSE;

   /* Can still move around if already inside the shrine area */
   if (!str_cmp(to_room->area->name, ch->in_room->area->name))
   return FALSE;

   if (ch->pause > 0
   || ch->hit < ch->max_hit
   || ch->bloody_shrine > 0)
   {
      send_to_char("You feel too bloody to enter there.\n\r", ch);
      return TRUE;
   }
   return FALSE;
}

void do_mount(CHAR_DATA *ch, char *argument)
{
   CHAR_DATA *mob;
   OBJ_DATA  *obj;
   CHAR_DATA *wch;
   AFFECT_DATA flying;
   char arg[MAX_INPUT_LENGTH];
   sh_int     mount_type = MOUNT_NONE;
   bool inventory = FALSE;

   if (IS_NPC(ch)) return;

   one_argument(argument, arg);

   if ( ch->is_riding )
   {
      do_dismount(ch, argument);
      return;
   }

   if ( arg[0] == '\0')
   {
      send_to_char("Mount what?\n\r", ch);
      return;

   }

   mob = get_char_room(ch, arg);
   if
   (
      !(obj = get_obj_list(ch, arg, ch->in_room->contents))
   )
   {
      obj = get_obj_carry(ch, argument, ch);
      inventory = TRUE;
   }

   if ( mob
   && mob != ch )
   {
      mount_type = MOUNT_MOBILE;

      if ( !IS_NPC(mob) )
      {
         send_to_char("Have you asked nicely first?\n\r", ch);
         return;
      }
   } else
   if ( obj )
   {
      mount_type = MOUNT_OBJECT;
   } else
   {
      send_to_char("They aren't here!\n\r", ch);
      return;
   }


   /* No centaurs riding white ponies :)  they will be able to charge as
   valor without riding */

   if
   (
      ch->race == grn_centaur &&
      mount_type == MOUNT_MOBILE
   )
   {
      send_to_char("Why do you need to ride someone?\n\r", ch);
      return;
   }

   switch( mount_type )
   {
      case MOUNT_MOBILE:
      if ( !IS_SET(mob->act2, ACT_RIDEABLE) )
      {
         send_to_char("You can't ride that!!!\n\r", ch);
         return;
      }

      if (mob->master != ch)
      {
         send_to_char("It refuses to let you ride it.\n\r", ch);
         return;
      }
      break;
      case MOUNT_OBJECT:
      if ( !IS_SET(obj->extra_flags2, ITEM_RIDEABLE)
      || !obj->mount_specs )
      {
         send_to_char("You can't ride that!!!\n\r", ch);
         return;
      }
      break;
      default:
      return;
   }

   for (wch=char_list; wch != NULL; wch=wch->next)
   {
      if ( wch->is_riding )
      {
         switch( wch->is_riding->mount_type )
         {
            case MOUNT_MOBILE:
            if ( mount_type == MOUNT_MOBILE
            && (CHAR_DATA *)wch->is_riding->mount == mob )
            {
               send_to_char("Someone is already riding it!\n\r", ch);
               return;
            }
            break;
            case MOUNT_OBJECT:
            if ( mount_type == MOUNT_OBJECT
            && (OBJ_DATA *)wch->is_riding->mount == obj )
            {
               send_to_char("Someone is already riding it!\n\r", ch);
               return;
            }
            break;
            default:
            return;
         }
      }
   }

   send_to_char("You begin riding upon it.\n\r", ch);
   /*
      free_string(ch->long_descr);
      sprintf(buf, "%s is here riding upon %s.\n\r", ch->name, mob->short_descr);
      ch->long_descr = str_dup(buf);
   */
   ch->is_riding = new_mount_info();
   ch->is_riding->mount_type = mount_type;
   switch( mount_type )
   {
      case MOUNT_MOBILE:
      ch->is_riding->mount = mob;
      if (IS_FLYING(mob))
      {
         flying.where = TO_AFFECTS;
         flying.location = APPLY_NONE;
         flying.duration = -1;
         flying.bitvector = AFF_FLYING;
         affect_modify( ch, &flying, TRUE );
      }
      break;
      case MOUNT_OBJECT:
      if ( inventory )
      {
         obj_from_char( obj );
         obj_to_room( obj, ch->in_room );
         SET_BIT( obj->mount_specs->mount_info_flags, MOUNT_INFO_INVENTORY );
      }
      ch->is_riding->mount = obj;
      if ( IS_SET(obj->mount_specs->move_flags, MOUNT_AIR) )
      {
         flying.where = TO_AFFECTS;
         flying.location = APPLY_NONE;
         flying.duration = -1;
         flying.bitvector = AFF_FLYING;
         affect_modify( ch, &flying, TRUE );
      }
      break;
   }

   return;
}

void do_dismount(CHAR_DATA *ch, char *argument)
{
   void* mount;
   AFFECT_DATA flying;
   /*
      free_string(ch->long_descr);
      ch->long_descr = "\0";
   */
   if ( ch->is_riding )
   {
      mount = ch->is_riding->mount;
      send_to_char("You dismount.\n\r", ch);
      switch( ch->is_riding->mount_type )
      {
         case MOUNT_MOBILE:
         if (IS_FLYING((CHAR_DATA*)mount))
         {
            flying.where = TO_AFFECTS;
            flying.location = APPLY_NONE;
            flying.duration = -1;
            flying.bitvector = AFF_FLYING;
            affect_modify( ch, &flying, FALSE );
            affect_check( ch, TO_AFFECTS, AFF_FLYING );
         }
         free_mount_info(ch->is_riding);
         ch->is_riding = NULL;
         break;
         case MOUNT_OBJECT:
         if ( IS_SET(((OBJ_DATA *)mount)->mount_specs->move_flags, MOUNT_AIR) )
         {
            flying.where = TO_AFFECTS;
            flying.location = APPLY_NONE;
            flying.duration = -1;
            flying.bitvector = AFF_FLYING;
            affect_modify( ch, &flying, FALSE );
            affect_check( ch, TO_AFFECTS, AFF_FLYING );
         }
         free_mount_info(ch->is_riding);
         ch->is_riding = NULL;
         if ( IS_SET(((OBJ_DATA *)mount)->mount_specs->mount_info_flags, MOUNT_INFO_INVENTORY) )
         {
            REMOVE_BIT( ((OBJ_DATA *)mount)->mount_specs->mount_info_flags, MOUNT_INFO_INVENTORY );
            get_obj( ch, (OBJ_DATA *)mount, NULL, 0 );
         }
      }
   } else
   {
      send_to_char("You aren't mounted!\n\r", ch);
   }
   return;
}

void do_charge(CHAR_DATA *ch, char *argument)
{
   CHAR_DATA *victim;
   ROOM_INDEX_DATA *was_in_room;
   char arg[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   int found, door, dam, direction, chance;

   one_argument(argument, arg);

   /* mobs can't charge for now because of references to pcdata below
   -wervdon */

   if (IS_NPC(ch))
   return;

   if (get_skill(ch, gsn_charge) <= 0)
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }

   if (house_down(ch, HOUSE_VALOR))
   return;


   if ( ch->is_riding )
   {
      switch( ch->is_riding->mount_type )
      {
         case MOUNT_MOBILE:
         if ( ((CHAR_DATA *)ch->is_riding->mount)->move < 150)
         {
            send_to_char("Your animal is too exhausted for that!\n\r", ch);
            return;
         }
         break;
         case MOUNT_OBJECT:
         if ( ((OBJ_DATA *)ch->is_riding->mount)->mount_specs->move < 150)
         {
            send_to_char("Your animal is too exhausted for that!\n\r", ch);
            return;
         }
         break;
         default:
         return;
      }
   } else
   {
      if (ch->race != grn_centaur)
      {
         send_to_char("You must be mounted to charge.\n\r", ch);
         return;
      } else
      if (ch->move < 75)
      {
         send_to_char("You are too exhausted for that!\n\r", ch);
         return;
      }
   }

   if (arg[0] == '\0'){
      if (ch->fighting == NULL){
         send_to_char("Charge who?\n\r", ch);
         return;
      }
      else
      victim = ch->fighting;
   }

   found = FALSE;
   direction = -1;
   was_in_room = ch->in_room;

   victim = get_char_room(ch, arg);
   for (door = 0; door <= 5; door++ )
   {
      EXIT_DATA *pexit;

      if ( ( pexit = was_in_room->exit[door] ) != NULL
      && pexit->u1.to_room != NULL && victim == NULL){
         char_from_room(ch);
         char_to_room_1(ch, pexit->u1.to_room, TO_ROOM_AT);
         victim = get_char_room(ch, arg);
         if (victim != NULL)
         {
            direction = door;
            found = TRUE;
         }
      }
   }
   char_from_room(ch);
   char_to_room_1(ch, was_in_room, TO_ROOM_AT);


   /* Is this following segment necessary?  Looks duplicated to me - Mael */
   if (arg[0] == '\0'){
      if (ch->fighting != NULL)
      victim = ch->fighting;
   }


   if (victim == NULL || victim == ch || (!can_see(ch, victim) && victim != ch->fighting)    )
   {
      send_to_char("They aren't here!\n\r", ch);
      return;
   }

   if (is_safe(ch, victim, IS_SAFE_IGNORE_ROOM))
   {
      return;
   }
   if ( ch->is_riding
   && ch->is_riding->mount_type == MOUNT_MOBILE
   && victim == (CHAR_DATA *)ch->is_riding->mount)
   {
      send_to_char("Amusing but impossible.\n\r", ch);
      return;
   }

   if (found == FALSE && victim->in_room != ch->in_room){
      send_to_char("They are too far away to charge.\n\r", ch);
      return;
   }

   if (oblivion_blink(ch, victim))
   {
      return;
   }
   /*
      if (victim->in_room->guild != 0 &&
      (victim->in_room->guild - 1) != ch->class)
      {
      send_to_char("You can't charge into their guild!\n\r", ch);
      return;
      }
   */
   dam = dice(ch->level, 8);
   if (ch->in_room == victim->in_room){
      dam = dam/2;
   }
   else
   {
      if (direction != -1)
      move_char(ch, direction, 0);
      if (ch->in_room != victim->in_room)
      return;
      act("$n charges in and attacks $N!", ch, NULL, victim, TO_NOTVICT);
      act("You charge in and attack $N!", ch, NULL, victim, TO_CHAR);
      act("$n charges in and attacks you!", ch, NULL, victim, TO_VICT);
   }

   if (!IS_NPC(ch) && !IS_NPC(victim)
   && (victim->fighting == NULL || ch->fighting == NULL))
   {
      sprintf(buf, "Help! %s is charging me!", PERS(ch, victim));
      do_myell(victim, buf);
      sprintf(log_buf, "[%s] charged [%s] at %d", ch->name, victim->name, ch->in_room->vnum);
      log_string(log_buf);
   }

   chance = get_skill(ch, gsn_charge);
   if (number_percent() > chance)
   dam = 0;

   damage(ch, victim, dam, gsn_charge, DAM_BASH, TRUE);

   if (dam == 0)
   check_improve(ch, gsn_charge, FALSE, 1);
   else
   check_improve(ch, gsn_charge, TRUE, 1);

   WAIT_STATE(ch, skill_table[gsn_charge].beats);

   if (ch->is_riding == NULL)
   ch->move -= 75;
   else
   switch( ch->is_riding->mount_type )
   {
      case MOUNT_MOBILE:
      ((CHAR_DATA *)ch->is_riding->mount)->move -= 150;
      break;
      case MOUNT_OBJECT:
      ((OBJ_DATA *)ch->is_riding->mount)->mount_specs->move -= 150;
      break;
      default:
      break;
   }

   return;
}

void do_blitz(CHAR_DATA *ch, char * argument)
{
   CHAR_DATA *victim;
   ROOM_INDEX_DATA *from_room;
   ROOM_INDEX_DATA *scan_room;
   EXIT_DATA *pExit;
   char arg[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   int dam, chance, cnt, direction, d2;
   int move_init = 0;
   AFFECT_DATA af;
   int found;

   one_argument(argument, arg);

   if (IS_NPC(ch))
   return;

   if (get_skill(ch, gsn_blitz) <= 0
   || !has_skill(ch, gsn_blitz))
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }

   if (house_down(ch, HOUSE_EMPIRE))
   return;

   if (ch->fighting != NULL)
   {
      send_to_char("You can't blitz out of a fight.\n\r", ch);
      return;
   }

   if (IS_SET(ch->in_room->extra_room_flags, ROOM_1212))
   {
      send_to_char("You failed.\n\r", ch);
      return;
   }

   if (IS_SET(ch->in_room->extra_room_flags, ROOM_DEAD_ZONE))
   {
      send_to_char("You failed.\n\r", ch);
      return;
   }

   if (arg[0] == '\0')
   {
      send_to_char("Blitz who?\n\r", ch);
      return;
   }

   from_room = ch->in_room;

   victim = get_char_world(ch, arg);

   if (victim == NULL || victim == ch)
   {
      send_to_char("They aren't here!\n\r", ch);
      return;
   }

   if ( IS_NPC(victim) )
   {
      if ( victim->master != NULL && can_see(ch, victim->master))
      victim = victim->master;
      else
      {
         send_to_char("You can't blitz to them!\n\r", ch);
         return;
      }
   }
   if (IS_NPC(victim))
   {
      send_to_char("You can't blitz to them!\n\r", ch);
      return;
   }
   if (ch->house != victim->house && !IS_SET(victim->act, PLR_NOSUMMON))
   if (is_safe(ch, victim, IS_SAFE_IGNORE_ROOM))
   return;

   if (victim->in_room == ch->in_room)
   {
      send_to_char("But they are right in front of you!\n\r", ch);
      return;
   }
   chance = get_skill(ch, gsn_blitz);
   if (IS_SET(victim->in_room->room_flags, ROOM_NO_BLITZ))
   chance = 0;
   chance -= victim->level*3;
   chance += ch->level*3;
   if (chance > 85) chance = 85;
   if (number_percent() > chance){
      WAIT_STATE(ch, 12);
      check_improve(ch, gsn_blitz, FALSE, 1);

      send_to_char("You try to blitz and fail.\n\r", ch);
      return;
   }
   move_init = ch->move;
   dam = number_range(ch->level * 3/2, ch->level *2);
   /* try the scan algorith first */
   d2 = -1;
   direction = -1;
   for (direction = 0; direction <= 5; direction++)
   {
      scan_room = ch->in_room;
      for (cnt = 1; cnt <= 6; cnt++)
      {
         if (scan_room == NULL) continue;
         if ((pExit = scan_room->exit[direction]) != NULL)
         scan_room = pExit->u1.to_room;
         if (victim->in_room == scan_room)
         d2 = direction;
      }
   }
   if (d2 == -1)
   {
      found = FALSE;
      from_room = ch->in_room;
      scan_room = ch->in_room;
      for (cnt = 1; cnt <= 9; cnt++)
      {
         if (victim->in_room == scan_room)
         {
            found = TRUE;
            continue;
         }
         char_from_room(ch);
         char_to_room_1(ch, scan_room, TO_ROOM_AT);
         direction = get_trackdir(ch, victim->id)-1;
         char_from_room(ch);
         char_to_room_1(ch, from_room, TO_ROOM_AT);
         if (direction < 0 || direction > 5)
         {
            send_to_char("They aren't here!\n\r", ch);
            return;
         }
         pExit = scan_room->exit[direction];
         if (pExit != NULL)
         scan_room = pExit->u1.to_room;
         if (scan_room == NULL || pExit == NULL)
         {
            send_to_char("They aren't here!\n\r", ch);
            return;
         }
      }
      if (found == FALSE)
      {
         send_to_char("They aren't here!\n\r", ch);
         return;
      }
   }
   af.where     = TO_AFFECTS;
   af.type      = gsn_blitz;
   af.level     = ch->level;
   af.duration  = 0;
   af.location  = APPLY_NONE;
   af.modifier  = 0;
   af.bitvector = 0;
   affect_to_char( ch, &af );

   if (d2 != -1)
   {
      direction = d2;
      for (cnt = 0; cnt <= 5; cnt++)
      {
         if (ch->in_room == victim->in_room)
         continue;
         if (direction < 0 || direction > 5)
         {
            send_to_char("You seem to have lost track of them.", ch);
            affect_strip(ch, gsn_blitz);
            return;
         }
         from_room = ch->in_room;
         if (ch->move < 6)
         {
            send_to_char("You're too tired to blitz.\n\r", ch);
            affect_strip(ch, gsn_blitz);
            return;
         }
         else
         {
            ch->move -= 6;
         }
         move_char(ch, direction, 0);
         if (ch->fighting)
         {
            affect_strip(ch, gsn_blitz);
            return;
         }
         if (!IS_AWAKE(ch))
         {
            affect_strip(ch, gsn_blitz);
            return;
         }
         if (ch->in_room == from_room)
         cnt = 10;
         act("$n leaves in a stunning blitz.", ch, NULL, NULL, TO_ROOM);
      }
   }
   else
   {
      for ( cnt = 1; cnt <= 8; cnt++)
      {
         if (ch->in_room == victim->in_room)
         continue;
         direction = get_trackdir(ch, victim->id)-1;
         if (direction < 0 || direction > 5)
         {
            send_to_char("You seem to have lost track of them.", ch);
            if ((move_init-ch->move) < 40)
            ch->move -= 40;
            if (ch->move < 0) ch->move = 0;
            affect_strip(ch, gsn_blitz);
            return;
         }
         from_room = ch->in_room;
         if (ch->move < 4)
         {
            send_to_char("You're too tired to blitz.\n\r", ch);
            affect_strip(ch, gsn_blitz);
            return;
         }
         else
         {
            ch->move -= 4;
         }
         move_char(ch, direction, 0);
         if (ch->fighting)
         {
            affect_strip(ch, gsn_blitz);
            return;
         }
         if (!IS_AWAKE(ch))
         {
            affect_strip(ch, gsn_blitz);
            return;
         }
         if (ch->in_room == from_room)
         cnt = 10;
         act("$n leaves in a stunning blitz.", ch, NULL, NULL, TO_ROOM);
      }
   }
   affect_strip(ch, gsn_blitz);
   WAIT_STATE(ch, 18);
   send_to_char("You chase after them in a stunning blitz.\n\r", ch);
   act("$n arrives in a stunning blitz.", ch, NULL, NULL, TO_ROOM);
   if (ch->in_room != victim->in_room || is_safe(ch, victim, 0))
   return;
   if (IS_SET(victim->act, PLR_CRIMINAL)){
      if (!IS_NPC(ch) && !IS_NPC(victim)
      && (victim->fighting == NULL || ch->fighting == NULL))
      {
         sprintf(buf, "Help! %s is blitzing me!", PERS(ch, victim));
         do_myell(victim, buf);
         sprintf(log_buf, "[%s] blitzed [%s] at %d", ch->name, victim->name, ch->in_room->vnum);
         log_string(log_buf);
      }
      /*    damage(ch, victim, dam, gsn_blitz, DAM_BASH, TRUE); */
      one_hit(ch, victim, gsn_blitz);
   }
   check_improve(ch, gsn_blitz, TRUE, 1);
   if ((move_init-ch->move) < 40)
   ch->move -= 40;
   if (ch->move < 0) ch->move = 0;

   return;
}

void do_ally(CHAR_DATA *ch, char * argument){
   CHAR_DATA *victim;
   char arg[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];

   one_argument(argument, arg);

   if
   (
      IS_NPC(ch) ||
      ch->house == 0 ||
      ch->pcdata->induct == 0 ||
      (
         ch->house == HOUSE_CONCLAVE &&
         !IS_IMMORTAL(ch)
      )
   )
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }
   if (ch->house == HOUSE_COVENANT)
   {
      DESCRIPTOR_DATA *d;
      int house;
      house = house_lookup(arg);
      if (house != 0)
      {
         covenant_allies[house] = !covenant_allies[house];
         if (covenant_allies[house] == 1)
         {
            send_to_char("The house is allied.\n\r", ch);
         }
         else
         {
            send_to_char("The houses ally status is removed.\n\r", ch);
         }
         for (d = descriptor_list; d != NULL; d = d->next)
         {
            victim = d->character;
            if (victim == NULL || d->connected != CON_PLAYING)
            continue;
            if (IS_IMMORTAL(victim)) continue;
            if (IS_NPC(victim)) continue;
            if (victim->house != house) continue;
            if (covenant_allies[house] == 0)
            {
               REMOVE_BIT(victim->pcdata->allied_with, powtwo(2, ch->house));
               send_to_char("Your house is no longer allied with the Covenant.\n\r", victim);
            }
            else
            {
               SET_BIT(victim->pcdata->allied_with, powtwo(2, ch->house));
               send_to_char("Your house is now allied with the Covenant.\n\r", victim);
            }
         }

         return;
      }
   }
   if ((victim = get_char_world(ch, arg)) == NULL){
      send_to_char("Make who an ally?\n\r", ch);
      return;
   }
   if (IS_NPC(victim)){
      send_to_char("You can't make them an ally.\n\r", ch);
      return;
   }
   if (ch->house == HOUSE_COVENANT)
   {
      if (IS_SET(victim->act, PLR_COVENANT_ALLY))
      {
         send_to_char("They are no longer allied with your house.\n\r", ch);
         sprintf(buf, "You are no longer allied with %s's house.\n\r", get_longname(ch, NULL));
         send_to_char(buf, victim);
         REMOVE_BIT(victim->pcdata->allied_with, powtwo(2, ch->house));
         REMOVE_BIT(victim->act, PLR_COVENANT_ALLY);
      }
      else
      {
         send_to_char("They are now allied with your house.\n\r", ch);
         sprintf(buf, "You are now allied with %s's house.\n\r", get_longname(ch, NULL));
         send_to_char(buf, victim);
         SET_BIT(victim->pcdata->allied_with, powtwo(2, ch->house));
         SET_BIT(victim->act, PLR_COVENANT_ALLY);
      }
      return;
   }
   if (IS_SET(victim->pcdata->allied_with, powtwo(2, ch->house))){
      send_to_char("They are no longer allied with your house.\n\r", ch);
      sprintf(buf, "You are no longer allied with %s's house.\n\r", get_longname(ch, NULL));
      send_to_char(buf, victim);
      REMOVE_BIT(victim->pcdata->allied_with, powtwo(2, ch->house));
   }
   else
   {
      send_to_char("They are now allied with your house.\n\r", ch);
      sprintf(buf, "You are now allied with %s's house.\n\r", get_longname(ch, NULL));
      send_to_char(buf, victim);
      SET_BIT(victim->pcdata->allied_with, powtwo(2, ch->house));
   }
   return;

}

void do_shackles(CHAR_DATA *ch, char * argument)
{
   CHAR_DATA *victim;
   OBJ_DATA  *shackles;
   OBJ_DATA  *boots;
   AFFECT_DATA af;
   int chance;
   char buf[MAX_STRING_LENGTH];

   if (IS_NPC(ch)) return;

   if (get_skill(ch, gsn_shackles) < 1
   || ch->pcdata->learnlvl[gsn_shackles] > ch->level)
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }

   if (house_down(ch, HOUSE_COVENANT))
   return;
   victim = NULL;
   if (argument[0] != '\0')
   victim = get_char_room(ch, argument);
   if (victim == NULL) victim = ch->fighting;
   if (victim == NULL)
   {
      send_to_char("Attempt to chain whose legs?\n\r", ch);
      return;
   }
   WAIT_STATE(ch, 24);
   if (is_safe(ch, victim, 0))
   {
      return;
   }

   if (oblivion_blink(ch, victim))
   {
      return;
   }

   if (ch->fighting == NULL || victim->fighting == NULL)
   {
      sprintf(buf, "Help!  %s is trying to put me in chains!", PERS(ch, victim));
      do_myell(victim, buf);
      sprintf(log_buf, "[%s] used shackles on %s at %d", ch->name, victim->name,
      ch->in_room->vnum);
      log_string(log_buf);
      multi_hit(victim, ch, TYPE_UNDEFINED);
   }
   if
   (
      (
         !IS_IMMORTAL(ch) &&
         !IS_IMMORTAL(victim)
      ) &&
      (

         !IS_NPC(victim) ||
         (
            IS_AFFECTED(victim, AFF_CHARM) &&
            victim->master
         )
      )
   )
   {
      QUIT_STATE(ch, 20);
      QUIT_STATE(victim, 20);
   }

   if (number_percent() > get_skill(ch, gsn_shackles))
   {
      send_to_char("You failed.\n\r", ch);
      check_improve(ch, gsn_shackles, FALSE, 1);
      return;
   }
   if
   (
      (
         shackles = get_obj_char_vnum(victim, OBJ_VNUM_SHACKLES)
      ) != NULL
   )
   {
      if (shackles->wear_loc != -1)
      {
         act("$E is already in chains!", ch, NULL, victim, TO_CHAR);
      }
      else
      {
         act("$E is already dragging shackles.", ch, NULL, victim, TO_CHAR);
      }
      return;
   }
   chance =  65 + (ch->level - victim->level) * 3;
   chance += get_curr_stat(ch, STAT_DEX) - get_curr_stat(victim, STAT_DEX);
   chance += get_curr_stat(ch, STAT_STR) - get_curr_stat(victim, STAT_STR);
   if (!can_see(victim, ch)) chance *= 2;
   if (!can_see(ch, victim)) chance /= 2;
   if (IS_AFFECTED(ch, AFF_HASTE)) chance += 20;
   if (IS_AFFECTED(victim, AFF_HASTE)) chance -= 20;
   if (IS_AFFECTED(ch, AFF_SLOW)) chance -= 30;
   if (IS_AFFECTED(victim, AFF_SLOW)) chance += 10;

   if (number_percent() > chance)
   {
      send_to_char("You failed.\n\r", ch);
      check_improve(ch, gsn_shackles, FALSE, 1);
      return;
   }

   shackles = create_object(get_obj_index(OBJ_VNUM_SHACKLES), 0);
   act("$n has placed shackles around $N's legs!", ch, NULL, victim, TO_ROOM);
   send_to_char("You place shackles around their legs!\n\r", ch);
   send_to_char("You are locked in chains!\n\r", victim);

   shackles->level = ch->level;
   af.where = TO_OBJECT;
   af.type = gsn_shackles;
   af.duration = -1;
   af.bitvector = 0;
   af.level = ch->level;
   af.location = APPLY_DEX;
   af.modifier = number_range(-5,-3);
   affect_to_obj(shackles, &af);
   af.location = APPLY_AC;
   af.modifier = ch->level;
   affect_to_obj(shackles, &af);
   SET_BIT(shackles->extra_flags, ITEM_NOREMOVE);
   SET_BIT(shackles->extra_flags, ITEM_NOUNCURSE);
   SET_BIT(shackles->extra_flags, ITEM_BURN_PROOF);
   SET_BIT(shackles->extra_flags, ITEM_ROT_DEATH);

   SET_BIT(shackles->wear_flags, ITEM_WEAR_FEET);
   SET_BIT(shackles->wear_flags, ITEM_WEAR_HOOVES);
   SET_BIT(shackles->wear_flags, ITEM_WEAR_FOURHOOVES);
   shackles->timer = 48;

   check_improve(ch, gsn_shackles, TRUE, 1);

   obj_to_char(shackles, victim);

   boots = get_eq_char(victim, WEAR_FEET);
   if (boots == NULL)
   {
      boots = get_eq_char(victim, WEAR_HOOVES);
   }
   if (boots == NULL)
   {
      boots = get_eq_char(victim, WEAR_FOURHOOVES);
   }


   if
   (
      boots != NULL &&
      IS_SET(boots->extra_flags, ITEM_NOREMOVE)
   )
   {
      if (!IS_SET(boots->extra_flags, ITEM_NOUNCURSE))
      {
         REMOVE_BIT(boots->extra_flags, ITEM_NOREMOVE);
      }
      do_wear(victim, "enfshackles");
      if (shackles->wear_loc == -1)
      {
         extract_obj(shackles, FALSE);
      }
      SET_BIT(boots->extra_flags, ITEM_NOREMOVE);
      return;
   }

   do_wear(victim, "enfshackles");
   return;
}

int powtwo(int x, int y)
{
   int cnt, ret_val;
   ret_val = 1;
   for (cnt = 1; cnt <= y; cnt++)
   ret_val *= x;
   return ret_val;
}

bool check_room_noescape(CHAR_DATA *ch)
{
   int door;
   EXIT_DATA *pexit;

   if (ch->in_room == NULL) return TRUE;
   for (door = 0; door < 6; door++)
   {
      if ( ( pexit   = ch->in_room->exit[door] ) == NULL
      ||   ( pexit->u1.to_room   ) == NULL
      ||   !can_see_room(ch, pexit->u1.to_room))
      continue;
      return FALSE;
   }
   return TRUE;
}

void do_animal_call(CHAR_DATA *ch, char *argument)
{
   char arg[MAX_INPUT_LENGTH];
   MOB_INDEX_DATA *pMobIndex;
   CHAR_DATA *animal = NULL;
   CHAR_DATA *mob;
   ROOM_INDEX_DATA *pRoomIndex;
   int chance = 100;  /* For immortals */
   int skill;
   int type;
   int i;
   AFFECT_DATA af;
   int cnt = 0;
   int count= 0;
   int num_pets= 0;
   int fn, fn2;

   one_argument(argument, arg);

   if ((skill = get_skill(ch, gsn_animal_call)) <= 0
   || !has_skill(ch, gsn_animal_call))
   {
      send_to_char("You don't know how to call upon animals for aid.\n\r", ch);
      return;
   }

   /* find the forest */
   fn = 0; fn2 = 0;
   while (forest_table[fn].name != NULL)
   {
      if (forest_table[fn].vnum_min <= ch->in_room->vnum &&
      forest_table[fn].vnum_max >= ch->in_room->vnum)
      fn2 = fn;
      fn++;
   }
   fn = fn2;

   if (arg[0] == '\0')
   {
      send_to_char("You scan the area to see what animals might be found.\n\r", ch);
      type = 0;
      while(animal_table[type].name != NULL)
      {
         if
         (
            !(
               (
                  (animal_table[type].restrict_r_bit & any_race_r) ||
                  (animal_table[type].restrict_e2_bit & any_race_e2)
               ) &&
               !IS_SET
               (
                  animal_table[type].restrict_r_bit,
                  pc_race_table[ch->race].restrict_r_bit
               ) &&
               !IS_SET
               (
                  animal_table[type].restrict_e2_bit,
                  pc_race_table[ch->race].restrict_e2_bit
               )
            )
         )
         {
            /* Race can call it. */
            if (forest_table[fn].animals[type] == -1) fn = 0;
            send_to_char(animal_table[type].name, ch);
            if (forest_table[fn].animals[type] > 70)
            send_to_char(" is extremely common here.\n\r", ch);
            else if (forest_table[fn].animals[type] > 40)
            send_to_char(" is common here.\n\r", ch);
            else if (forest_table[fn].animals[type] > 15)
            send_to_char(" is rare here.\n\r", ch);
            else if (forest_table[fn].animals[type] > 0)
            send_to_char(" is very rare here.\n\r", ch);
            else
            send_to_char(" is non-existant here.\n\r", ch);
         }

         type++;
         fn = fn2;
      }
      return;
   }

   if (is_affected(ch, gsn_animal_call))
   {
      send_to_char("You aren't able to call upon anymore animals yet.\n\r", ch);
      return;
   }

   if (argument[0] == '\0')
   {
      send_to_char("Call out for which animal?\n\r", ch);
      return;
   }

   type = 0;
   while( animal_table[type].name != NULL)
   {
      if (!str_cmp(animal_table[type].name, argument))
      break;
      type++;
   }
   if
   (
      animal_table[type].name == NULL ||
      (
         (
            (animal_table[type].restrict_r_bit & any_race_r) ||
            (animal_table[type].restrict_e2_bit & any_race_e2)
         ) &&
         !IS_SET
         (
            animal_table[type].restrict_r_bit,
            pc_race_table[ch->race].restrict_r_bit
         ) &&
         !IS_SET
         (
            animal_table[type].restrict_e2_bit,
            pc_race_table[ch->race].restrict_e2_bit
         )
      )
   )
   {
      send_to_char("You can't call that animal.\n\r", ch);
      return;
   }

   if (ch->mana < 50)
   {
      send_to_char("You don't have the mana.\n\r", ch);
      return;
   }

   if (animal_table[type].level > ch->level)
   {
      send_to_char("You are not skilled enough at woodsmanship to call that animal.\n\r", ch);
      return;
   }

   if (!IS_IMMORTAL(ch))
   {
      switch(ch->pcdata->special)
      {
         default:
         if (type != 2 && type != 4 && type != 5 && type != 16)
         {
            send_to_char("You are not skilled enough at woodsmanship to call that animal.\n\r", ch);
            return;
         }
         break;
         case SUBCLASS_HUNTER:
         if (type != 16 && type != 21)
         {
            send_to_char("You are not skilled enough at woodsmanship to call that animal.\n\r", ch);
            return;
         }
         break;
         case SUBCLASS_BEASTMASTER:
         if (type == 21)
         {
            send_to_char("Those animals are too domesticated for you to call.\n\r", ch);
            return;
         }
         break;
      }

      switch(type)
      {
         default: break;
         case 11:
         case 13:
         case 1:
         if (ch->alignment > -1000)
         {
            send_to_char("That animal will not answer one of your path.\n\r", ch);
            return;
         }
         break;
         case 0:
         case 9:
         case 20:
         if (ch->alignment < 1000)
         {
            send_to_char("That animal will not answer one of your path.\n\r", ch);
            return;
         }
         break;
      }
      chance = forest_table[fn].animals[type];
      if (chance == -1) chance = forest_table[0].animals[type];
      if (number_percent() > chance)
      {
         send_to_char("Those animals just aren't responding here.\n\r", ch);
         return;
      }
   }
   if ( (ch->in_room->sector_type != SECT_FOREST)
   && ( ch->in_room->sector_type != SECT_HILLS)
   && ( ch->in_room->sector_type != SECT_MOUNTAIN) )
   {
      send_to_char("You are not within the right environment to call animals.\n\r", ch);
      return;
   }

   count = 0;
   for (mob = char_list; mob != NULL; mob = mob->next)
   {
      if (IS_NPC(mob) && IS_AFFECTED(mob, AFF_CHARM)
      && (mob->master == ch))
      {
         cnt = 0;
         while(animal_table[cnt].name != NULL)
         {
            if (animal_table[cnt].vnum == mob->pIndexData->vnum)
            {
               if (ch->pcdata->special == SUBCLASS_BEASTMASTER)
               {
                  count += mob->size;
                  if (mob->size == 0) count++;
               }
               else
               {
                  count++;
               }
            }
            cnt++;
         }
      }
   }

   if (!IS_IMMORTAL(ch))
   {
      if (ch->pcdata->special == SUBCLASS_BEASTMASTER && count >= 8)
      {
         send_to_char("You already have animals you should care for.\n\r", ch);
         return;
      }
      if (ch->pcdata->special != SUBCLASS_BEASTMASTER && count >= 2)
      {
         send_to_char("You already have animals you should care for.\n\r", ch);
         return;
      }
   }
   else if (count >= 10)
   {
      send_to_char("You already have animals you should care for.\n\r", ch);
      return;
   }


   if
   (
      number_percent() > chance ||
      number_percent() > skill
   )
   {
      act("$n calls out to the wild but nothing responds.", ch, NULL, NULL, TO_ROOM);
      send_to_char("You call out to the wild but nothing responds.\n\r", ch);
      ch->mana -= 25;
      check_improve(ch, gsn_animal_call, FALSE, 1);
      return;
   }

   pRoomIndex = ch->in_room;
   pMobIndex = get_mob_index(animal_table[type].vnum);

   if (pMobIndex == NULL)
   {
      bug("Animal call: Bad mob vnum call %d.", type);
      send_to_char("You call out to the wild but nothing responds.\n\r", ch);
      act("$n calls out to the wild but nothing responds.", ch, NULL, NULL, TO_ROOM);
      ch->mana -= 25;
      return;
   }

   if (ch->pcdata->special == SUBCLASS_BEASTMASTER)
   num_pets = 1;
   else
   num_pets = 2-count;
   for (cnt = 1; cnt <= num_pets; cnt++)
   {
      animal = create_mobile( pMobIndex );
      if (animal == NULL)
      return;

      for (i = 0; i < 4; i++)
      {
         animal->armor[i] -= (3*ch->level);
      }
      switch(animal_table[type].vnum)
      {
         case 2947: /* rat */
         animal->max_hit = ch->max_hit/3;
         animal->hit = ch->max_hit/3;
         animal->damroll = (5 + ch->level/3);
         animal->hitroll = ch->level/3;
         break;
         case 2953: /* viper  */
         animal->spec_fun = spec_lookup   ("spec_poison");
         case 2939: /* eagle*/
         case 2946: /* python */
         case 2952: /* parrot */
         animal->max_hit = ch->max_hit/2+250;
         animal->hit = ch->max_hit/2+250;
         animal->damroll = (8 + ch->level/3)+5;
         animal->hitroll = ch->level/3+5;
         break;
         case 2948: /* cobra  */
         animal->spec_fun = spec_lookup   ("spec_poison");
         case 2940: /* crocodile */
         case 2941: /* monkey */
         case 2951: /* boar   */
         case 2955: /* hounds */
         animal->max_hit = ch->max_hit+300;
         animal->hit = ch->max_hit+300;
         animal->damroll = (2*ch->level/3)+15;
         animal->hitroll = ch->level/3+15;
         break;
         case 2942: /*rhino  */
         animal->max_hit = ch->max_hit+1000;
         animal->hit = ch->max_hit+1000;
         animal->damroll = (2*ch->level/3)+25;
         animal->hitroll = ch->level/3+25;
         break;
         case 2943: /*gorilla*/
         case 2944: /*unicorn*/
         case 2945: /*panther*/
         animal->max_hit = ch->max_hit+500;
         animal->hit = ch->max_hit+500;
         animal->damroll = (2*ch->level/3)+25;
         animal->hitroll = ch->level/3+25;
         break;
         case 2949: /* elephant */
         animal->max_hit = ch->max_hit+2000;
         animal->hit =  ch->max_hit+2000;
         animal->damroll = ch->level+30;
         animal->hitroll = ch->level+30;
         break;
         case 2950: /* tiger */
         case 2954: /* golden */
         animal->max_hit = ch->max_hit+1000;
         animal->hit =  ch->max_hit+1000;
         animal->damroll = ch->level+30;
         animal->hitroll = ch->level+30;
         break;
         case (MOB_VNUM_FALCON):
         {
            animal->max_hit = ch->max_hit/3;
            animal->hit = ch->max_hit/3;
            animal->damroll = (5 + ch->level/3);
            animal->hitroll = ch->level/3;
            break;
         }
         case (MOB_VNUM_WOLF):
         {
            animal->max_hit = ch->max_hit/2;
            animal->hit = ch->max_hit/2;
            animal->damroll = (8 + ch->level/3);
            animal->hitroll = ch->level/3;
            break;
         }
         case (MOB_VNUM_BEAR):
         {
            animal->max_hit = ch->max_hit;
            animal->hit = ch->max_hit;
            animal->damroll = (2*ch->level/3);
            animal->hitroll = ch->level/3;
            break;
         }
         case (MOB_VNUM_LION):
         case (2963): /* spider */
         {
            animal->max_hit = (3 * ch->max_hit / 4);
            animal->hit = (3 * ch->max_hit / 4);
            animal->damroll = (-2 + ch->level);
            animal->hitroll = ch->level/3;
            break;
         }
      }
      if (ch->pcdata->special == SUBCLASS_BEASTMASTER &&
      animal->size > 2)
      SET_BIT(animal->act2, ACT_RIDEABLE);
      char_to_room(animal, pRoomIndex);
      animal->level = ch->level;
      SET_BIT(animal->affected_by, AFF_CHARM);
      SET_BIT(animal->affected_by, AFF_ACUTE_VISION);
      animal->leader = ch;
      animal->master = ch;
   }

   af.where = TO_AFFECTS;
   af.type = gsn_animal_call;
   af.level = ch->level;
   af.duration = 24;
   if (ch->pcdata->special == SUBCLASS_BEASTMASTER)
   af.duration = 3;
   af.location = 0;
   af.modifier = 0;
   af.bitvector = 0;
   affect_to_char(ch, &af);

   ch->mana -= 50;
   act("Animals respond to $n's call!", ch, NULL, animal, TO_ROOM);
   act("Animals respond to your call!", ch, NULL, animal, TO_CHAR);
   check_improve(ch, gsn_animal_call, TRUE, 2);

   return;
}


void do_getaway(CHAR_DATA *ch, char *argument)
{
   AFFECT_DATA af;
   ROOM_INDEX_DATA *watcher, *sourceroom;
   CHAR_DATA *och, *och_next;
   int camewith = 0;
   int vnum_to = 0;
   int chance = 0;

   if ( (get_skill(ch, gsn_getaway) <= 0)
   || !has_skill(ch, gsn_getaway) )
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }

   if (house_down(ch, HOUSE_OUTLAW))
   return;
   if (ch->in_room == NULL) return;
   if (ch->in_room->vnum >= 10600 && ch->in_room->vnum <= 10799)
   vnum_to = 1950;
   if (ch->in_room->vnum >= 3500 && ch->in_room->vnum <= 3599)
   vnum_to = 1951;
   if (ch->in_room->vnum >= 3200 && ch->in_room->vnum <= 3299)
   vnum_to = 1952;
   if (ch->in_room->vnum >= 9442 && ch->in_room->vnum <= 9799)
   vnum_to = 1953;
   if (ch->in_room->vnum >= 3000 && ch->in_room->vnum <= 3199)
   vnum_to = 1954;
   if (ch->in_room->vnum >= 300 && ch->in_room->vnum <= 399)
   vnum_to = 1955;
   if (ch->in_room->vnum >= 27700 && ch->in_room->vnum <= 27899)
   vnum_to = 1956;
   if (ch->in_room->vnum >= 700 && ch->in_room->vnum <= 873)
   vnum_to = 1957;
   if (ch->in_room->vnum >= 600 && ch->in_room->vnum <= 699)
   vnum_to = 1958;
   if (ch->in_room->vnum >= 8700 && ch->in_room->vnum <= 8899)
   vnum_to = 1959;
   if (ch->in_room->vnum >= 6800 && ch->in_room->vnum <= 6899)
   vnum_to = 1960;
   if (ch->in_room->vnum >= 26200 && ch->in_room->vnum <= 26299)
   vnum_to = 1961;
   /*        if (ch->in_room->vnum >= 9800 && ch->in_room->vnum <= 9899)
   vnum_to = 1962; */

   if (vnum_to == 0 ||
   (watcher = get_room_index(vnum_to)) == NULL) {
      send_to_char("There is no secret entrance around here!\n\r", ch);
      return;
   }

   WAIT_STATE(ch, 24);

   if (ch->quittime > 12)
   {
      switch(ch->quittime)
      {
         case 20: chance = 0; break;
         case 19: chance = 0; break;
         case 18: chance = 15; break;
         case 17: chance = 20; break;
         case 16: chance = 25; break;
         case 15: chance = 35; break;
         case 14: chance = 50; break;
         case 13: chance = 70; break;
      }
      if (number_percent() >= chance )
      {
         send_to_char("No way, you're being watched, you'd give away the hideout's location!\n\r", ch);
         return;
      }
   }

   if (IS_AFFECTED(ch, AFF_BLIND) && number_range(1, 10) < 8)
   {
      send_to_char("You can't seem to find the secret entrance.\n\r", ch);
      return;
   }

   if ((number_percent() > get_skill(ch, gsn_getaway))
   || (ch->in_room->house != 0 && ch->in_room->house != ch->house)
   ||   (ch->in_room->vnum >= 1200 && ch->in_room->vnum <= 1240))
   {
      send_to_char("You can't seem to remember where the entrance was.\n\r", ch);
      check_improve(ch, gsn_getaway, FALSE, 1);
      return;
   }
   check_improve(ch, gsn_getaway, TRUE, 1);

   act("$n vanishes into a secret exit!", ch, NULL, NULL, TO_ROOM);

   sourceroom = ch->in_room;
   char_from_room(ch);
   char_to_room(ch, watcher);
   send_to_char("You open the secret exit and vanish into it.\n\r", ch);
   act("$n arrives from the secret entrance!", ch, NULL, NULL, TO_ROOM);
   do_observe(ch, "", LOOK_AUTO);
   for ( och = sourceroom->people; och != NULL; och = och_next )
   {
      och_next = och->next_in_room;
      if ( IS_AFFECTED(och, AFF_CHARM)
      &&   och->master == ch
      && (och->in_room != NULL))
      {
         if ( och->fighting != NULL )
         stop_fighting( och, TRUE );
         char_from_room(och);
         char_to_room(och, watcher);
         camewith++;
      }
   }
   if (camewith > 0)
   {
      send_to_char("Your followers faithfully follow you.\n\r", ch);
      act("$s followers step forth behind $m!", ch, NULL, NULL, TO_ROOM);
   }

   af.where=TO_AFFECTS;
   af.type=gsn_getaway;
   af.location=0;
   af.bitvector=0;
   af.duration= 0;
   af.modifier=0;
   af.level=ch->level;
   if (!is_affected(ch, gsn_getaway))
   affect_to_char(ch, &af);

   WAIT_STATE(ch, 24);
   return;
}

void do_armor_of_god( CHAR_DATA *ch, char *argument)
{
   char armorLimb[MAX_STRING_LENGTH];
   char buf[MAX_STRING_LENGTH];
   int chance;

   if ( (chance = get_skill(ch, gsn_armor_of_god)) <= 0
   ||  (!IS_NPC(ch)
   && !has_skill(ch, gsn_armor_of_god)))
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }

   if (house_down(ch, HOUSE_LIGHT))
   return;

   if ( is_affected(ch, gsn_armor_of_god) )
   {
      send_to_char( "Your holy armor reveals itself, glowing a little brighter and then vanishes.\n\r", ch );
      return;
   }

   if (ch->mana < 50)
   {
      send_to_char( "You do not have enough mana.\n\r", ch );
      return;
   }

   getLimb( ch, armorLimb );

   if (number_percent() < chance)
   {
      AFFECT_DATA af;

      WAIT_STATE(ch, PULSE_VIOLENCE);
      ch->mana -= 50;

      sprintf( buf, "%s%s%s%s%s%s%s%s%s%s%s%s",
      "$n strikes a pose and declares, \"The forces of the Night may flood the land,\n\r",
      "but they can never penetrate the Light!\"  $n lifts $s ",
      armorLimb,
      "s to the sky and\n\r",
      "shouts triumphantly, \"Armor...of...GOD!\"\n\r\n\r",
      "$n leaps into the air, spinning slowly as wind blows hard against $m.\n\r\n\r",
      "The clouds part, and a ray of Light shoots down upon $m, bathing $m in its\n\r",
      "glow.  Other tendrils of the Light swirl around $n, moving near $m, until\n\r",
      "they suddenly clench into the form of a translucent set of full plate mail!\n\r",
      "$n descends to the ground again, the wind easing, and strikes another pose,\n\r",
      "the holy armor sparkling for a moment.  Then, the holy armor fades out of\n\r",
      "existence, leaving only an afterimage of its glory.");
      act( buf, ch, NULL, NULL, TO_ROOM );

      sprintf( buf, "%s%s%s%s%s%s%s%s%s%s%s%s",
      "You strike a pose and declare, \"The forces of the Night may flood the land,\n\r",
      "but they can never penetrate the Light!\"  You lift your ",
      armorLimb,
      "s to the sky and\n\r",
      "shout triumphantly, \"Armor...of...GOD!\"\n\r\n\r",
      "You leap into the air, spinning slowly as wind blows hard against you.\n\r\n\r",
      "The clouds part, and a ray of Light shoots down upon you, bathing you in its\n\r",
      "glow.  Other tendrils of the Light swirl around you, moving near you, until\n\r",
      "they suddenly clench into the form of a translucent set of full plate mail!\n\r",
      "You descend to the ground again, the wind easing, and strike another pose,\n\r",
      "the holy armor sparkling for a moment.  Then, the holy armor fades out of\n\r",
      "existence, leaving only an afterimage of its glory.");
      act( buf, ch, NULL, NULL, TO_CHAR );

      af.where        = TO_AFFECTS;
      af.type         = gsn_armor_of_god;
      af.level        = GET_LEVEL(ch);
      af.duration     = (GET_LEVEL(ch) /2);
      af.location     = APPLY_HIT;
      af.bitvector    = 0;
      af.modifier     = ch->level*5;
      affect_to_char( ch, &af );
      ch->hit += ch->level*5;
      if (ch->hit > ch->max_hit) ch->hit = ch->max_hit;

      check_improve( ch, gsn_armor_of_god, TRUE, 2 );
   }
   else
   {
      WAIT_STATE( ch, PULSE_VIOLENCE );
      ch->mana -= 25;

      sprintf( buf, "%s%s%s%s%s%s",
      "$n strikes a pose and declares, \"The forces of the Night may flood the land,\n\r",
      "but they can never penetrate the Light!\"  $n lifts $s ",
      armorLimb,
      "s to the sky and\n\r",
      "shouts triumphantly, \"Armor...of...GOD!\"\n\r\n\r",
      "...and nothing happens except a slight fizzling sound of unknown origin." );
      act( buf, ch, NULL, NULL, TO_ROOM );

      sprintf( buf, "%s%s%s%s%s%s",
      "You strike a pose and declare, \"The forces of the Night may flood the land,\n\r",
      "but they can never penetrate the Light!\"  You lift your ",
      armorLimb,
      "s to the sky and\n\r",
      "shout triumphantly, \"Armor...of...GOD!\"\n\r\n\r",
      "...and nothing happens except a slight fizzling sound of unknown origin." );
      act( buf, ch, NULL, NULL, TO_CHAR );

      check_improve( ch, gsn_armor_of_god, FALSE, 2 );
   }
}

void do_jump(CHAR_DATA* ch, char* argument)
{
   CHAR_DATA* victim;
   CHAR_DATA* mount;
   OBJ_DATA*  spear;
   bool ally = FALSE;
   int chance;
   int mana;
   int move;
   AFFECT_DATA* paf;
   sh_int bonus = 0;
   int dam_type;
   int dam;

   if
   (
      !has_skill(ch, gsn_jump) ||
      (
         chance = get_skill(ch, gsn_jump)
      ) <= 1
   )
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }
   if (house_down(ch, HOUSE_COVENANT))
   {
      return;
   }
   if (ch->fighting)
   {
      send_to_char("You are still fighting!\n\r", ch);
      return;
   }
   if (argument[0] == '\0')
   {
      send_to_char("Jump to whom?\n\r", ch);
      return;
   }
   victim = get_char_world(ch, argument);
   if
   (
      victim == NULL ||
      victim->in_room == NULL
   )
   {
      send_to_char("They are not here.\n\r", ch);
      return;
   }
   if (IS_NPC(victim))
   {
      send_to_char("You cannot jump to them.\n\r", ch);
      return;
   }

   if (IS_SET(ch->in_room->extra_room_flags, ROOM_DEAD_ZONE))
   {
      send_to_char("You failed.\n\r", ch);
      return;
   }

   if (victim == ch)
   {
      send_to_char("You jump up in the air and land on yourself.\n\r", ch);
      return;
   }
   if
   (
      victim->house == HOUSE_COVENANT ||
      IS_SET(victim->act, PLR_COVENANT_ALLY) ||
      IS_SET
      (
         victim->pcdata->allied_with,
         powtwo(2, HOUSE_COVENANT)
      ) ||
      (
         victim->house &&
         covenant_allies[victim->house]
      )
   )
   {
      ally = TRUE;
   }
   if
   (
      (
         ch->in_room->house &&
         ch->in_room->house != HOUSE_COVENANT
      ) ||
      IS_SET(ch->in_room->extra_room_flags, ROOM_1212) ||
      IS_SET(ch->in_room->room_flags, ROOM_NO_BLITZ)
   )
   {
      send_to_char("You cannot jump from here.\n\r", ch);
      return;
   }
   if
   (
      IS_IMMORTAL(victim) &&
      !IS_IMMORTAL(ch)
   )
   {
      send_to_char("You failed to jump.\n\r", ch);
      return;
   }
   if
   (
      !ally &&
      is_safe(ch, victim, IS_SAFE_SILENT | IS_SAFE_IGNORE_ROOM)
   )
   {
      send_to_char("The gods protect them from the jump.\n\r", ch);
      return;
   }
   if
   (
      victim->in_room->area != ch->in_room->area &&
      !ally
   )
   {
      send_to_char("They are too far away.\n\r", ch);
      return;
   }
   if (victim->in_room == ch->in_room)
   {
      send_to_char("They are too close.\n\r", ch);
      return;
   }
   if (ally)
   {
      mana = 65;
      move = 25;
      if (victim->in_room->area != ch->in_room->area)
      {
         mana += 20;
         move += 20;
      }
   }
   else
   {
      mana = 50;
      move = 15;
   }
   if
   (
      ch->mana < mana ||
      ch->move < move
   )
   {
      send_to_char("You must rest before you can jump.\n\r", ch);
      return;
   }
   if (is_affected(victim, gsn_jump))
   {
      if (ally)
      {
         send_to_char("They are not ready for you to jump to yet.\n\r", ch);
      }
      else
      {
         send_to_char("They are too wary for you to jump to.\n\r", ch);
      }
      return;
   }
   WAIT_STATE(ch, skill_table[gsn_jump].beats);
   if (number_percent() > chance)
   {
      ch->move -= move;
      ch->mana -= mana / 2;
      send_to_char("You fail to jump.\n\r", ch);
      check_improve(ch, gsn_jump, FALSE, 1);
      return;
   }
   if
   (
      IS_SET(victim->in_room->room_flags, ROOM_NO_BLITZ) ||
      IS_SET(victim->in_room->room_flags, ROOM_PRIVATE) ||
      IS_SET(victim->in_room->room_flags, ROOM_SAFE) ||
      IS_SET(victim->in_room->room_flags, ROOM_SOLITARY) ||
      IS_SET(victim->in_room->room_flags, ROOM_NO_GATE) ||
      IS_SET(victim->in_room->extra_room_flags, ROOM_1212) ||
      is_shrine(victim->in_room) ||
      victim->in_room->guild ||
      !can_see_room(ch, victim->in_room) ||
      IS_SET(victim->imm_flags, IMM_SUMMON) ||
      (
         ally &&
         (
            victim->in_room->house &&
            victim->in_room->house != HOUSE_COVENANT
         )
      ) ||
      (
         !ally &&
         (
            victim->in_room->house != ch->in_room->house ||
            saves_spell(ch, ch->level, victim, DAM_OTHER, SAVE_TRAVEL)
         )
      )
   )
   {
      ch->move -= move;
      ch->mana -= mana / 2;
      check_improve(ch, gsn_jump, FALSE, 1);
      send_to_char("You fail to jump to them.\n\r", ch);
      return;
   }
   if
   (
      IS_SET(victim->in_room->room_flags, ROOM_BLOODY_TIMER) &&
      (
         ch->pause > 0 ||
         ch->quittime > 13
      )
   )
   {
      ch->move -= move / 2;
      ch->mana -= mana / 2;
      send_to_char("You are too bloody to jump to that place.\n\r", ch);
      check_improve(ch, gsn_jump, FALSE, 2);
      return;
   }
   /* Vis, minus invis/sneak */
   un_shroud(ch, NULL);
   un_earthfade(ch, NULL);
   un_camouflage(ch, NULL);
   un_hide(ch, NULL);
   un_forest_blend(ch);
   affect_strip(ch, gsn_lurk);
   paf = new_affect();
   paf->type = gsn_jump;
   if (!ally)
   {
      paf->duration = 4;
   }
   affect_to_char(victim, paf);
   free_affect(paf);
   if (ch->is_riding)
   {
      switch (ch->is_riding->mount_type)
      {
         case (MOUNT_MOBILE):
         {
            mount = (CHAR_DATA*)ch->is_riding->mount;
            if
            (
               mount->in_room != ch->in_room ||
               (
                  mount->pIndexData->vnum != MOB_VNUM_COVENANT_DRAGON &&
                  mount->pIndexData->vnum != MOB_VNUM_COVENANT_NIGHTMARE
               )
            )
            {
               do_dismount(ch, "");
               mount = NULL;
               break;
            }
            break;
         }
         case (MOUNT_OBJECT):
         {
            do_dismount(ch, "");
            mount = NULL;
            break;
         }
         default:
         {
            mount = NULL;
            break;
         }
      }
   }
   else
   {
      mount = NULL;
   }

   if (mount)
   {
      act
      (
         "$N jumps into the air on $n and disappears.",
         mount,
         NULL,
         ch,
         TO_ROOM
      );
      send_to_char
      (
         "You jump into the air on your mount and disappear.\n\r",
         ch
      );
   }
   else
   {
      act
      (
         "$n jumps into the air and disappears.",
         ch,
         NULL,
         NULL,
         TO_ROOM
      );
      send_to_char
      (
         "You jump into the air and disappear.\n\r",
         ch
      );
   }
   char_from_room(ch);
   char_to_room(ch, victim->in_room);
   do_observe(ch, "", LOOK_AUTO);
   if (mount)
   {
      char_from_room(mount);
      char_to_room(mount, victim->in_room);
   }
   if (ally)
   {
      if (mount)
      {
         if (mount->pIndexData->vnum == MOB_VNUM_COVENANT_DRAGON)
         {
            act
            (
               "$n's dragon suddenly appears above you, landing beside you.",
               ch,
               NULL,
               victim,
               TO_VICT
            );
            act
            (
               "$n's dragon suddenly appears above $N, landing beside $M.",
               ch,
               NULL,
               victim,
               TO_NOTVICT
            );
         }
         else
         {
            act
            (
               "$n's nightmare suddenly appears above you, landing beside you.",
               ch,
               NULL,
               victim,
               TO_VICT
            );
            act
            (
               "$n's nightmare steed suddenly appears above $N, landing beside $M.",
               ch,
               NULL,
               victim,
               TO_NOTVICT
            );
         }
      }
      else
      {
         act
         (
            "$n suddenly appears above you, landing beside you.",
            ch,
            NULL,
            victim,
            TO_VICT
         );
         act
         (
            "$n suddenly appears above $N, landing beside $M.",
            ch,
            NULL,
            victim,
            TO_NOTVICT
         );
      }
      act
      (
         "You appear above $N, landing beside $M.",
         ch,
         NULL,
         victim,
         TO_CHAR
      );
      ch->move -= move;
      ch->mana -= mana;
      check_improve(ch, gsn_jump, TRUE, 1);
      return;
   }
   if (number_percent() > chance)
   {
      if (mount)
      {
         if (mount->pIndexData->vnum == MOB_VNUM_COVENANT_DRAGON)
         {
            act
            (
               "$n's dragon suddenly appears above you, but you swiftly dodge"
               " their attack.",
               ch,
               NULL,
               victim,
               TO_VICT
            );
            act
            (
               "$n's dragon suddenly appears above $N, landing beside $M.",
               ch,
               NULL,
               victim,
               TO_NOTVICT
            );
         }
         else
         {
            act
            (
               "$n's nightmare steed suddenly appears above you, but you"
               " swiftly dodge their attack.",
               ch,
               NULL,
               victim,
               TO_VICT
            );
            act
            (
               "$n's nightmare steed suddenly appears above $N, landing beside"
               " $M.",
               ch,
               NULL,
               victim,
               TO_NOTVICT
            );
         }
      }
      else
      {
         act
         (
            "$n suddenly appears above you, but you swiftly dodge their attack.",
            ch,
            NULL,
            victim,
            TO_VICT
         );
         act
         (
            "You appear above $N, but they swiftly dodge your attack.",
            ch,
            NULL,
            victim,
            TO_CHAR
         );
         act
         (
            "$n suddenly appears above $N, landing beside $M.",
            ch,
            NULL,
            victim,
            TO_NOTVICT
         );
      }
      ch->move -= move / 2;
      ch->mana -= mana / 2;
      check_improve(ch, gsn_jump, FALSE, 1);
      return;
   }
   check_improve(ch, gsn_jump, TRUE, 1);
   ch->move -= move;
   ch->mana -= mana;
   if (mount)
   {
      if (mount->pIndexData->vnum == MOB_VNUM_COVENANT_DRAGON)
      {
         act
         (
            "$n's dragon suddenly appears above you, and comes crashing down"
            " upon you.",
            ch,
            NULL,
            victim,
            TO_VICT
         );
         act
         (
            "$n's dragon suddenly appears above $N, and comes crashing down"
            " upon $M.",
            ch,
            NULL,
            victim,
            TO_NOTVICT
         );
         bonus += 3;
      }
      else
      {
         act
         (
            "$n's nightmare suddenly appears above you, and comes crashing"
            " down upon you.",
            ch,
            NULL,
            victim,
            TO_VICT
         );
         act
         (
            "$n's nightmare suddenly appears above $N, and comes crashing down"
            " upon $M.",
            ch,
            NULL,
            victim,
            TO_NOTVICT
         );
         bonus += 2;
      }
   }
   else
   {
      act
      (
         "$n suddenly appears above you, and comes crashing down upon you.",
         ch,
         NULL,
         victim,
         TO_VICT
      );
      act
      (
         "$n suddenly appears above $N, and comes crashing down upon $M.",
         ch,
         NULL,
         victim,
         TO_NOTVICT
      );
   }
   act
   (
      "You appear above $N, and come crashing down upon $M.",
      ch,
      NULL,
      victim,
      TO_CHAR
   );
   spear = get_eq_char(ch, WEAR_WIELD);
   if
   (
      spear &&
      spear->item_type == ITEM_WEAPON &&
      spear->value[0] == WEAPON_SPEAR
   )
   {
      bonus++;
   }
   if
   (
      spear &&
      spear->item_type == ITEM_WEAPON
   )
   {
      dam_type = attack_table[spear->value[3]].damage;
   }
   else
   {
      dam_type = DAM_BASH;
   }
   dam = dice(5, (GET_LEVEL(ch)) / 4);
   /* Reduce damage on non heroes */
   if (GET_LEVEL(ch) < LEVEL_HERO)
   {
      dam *= GET_LEVEL(ch);
      dam /= LEVEL_HERO;
   }
   dam += dam * bonus * 4 / 10;
   if (mount)
   {
      act
      (
         "You are crushed under the weight of $n.",
         mount,
         NULL,
         victim,
         TO_VICT
      );
   }
   if
   (
      get_skill(ch, gsn_enhanced_damage) > 0 &&
      has_skill(ch, gsn_enhanced_damage)
   )
   {
      int diceroll = number_percent();

      if (diceroll <= get_skill(ch, gsn_enhanced_damage))
      {
         check_improve(ch, gsn_enhanced_damage, TRUE, 6);
         dam += 2 * (dam * (diceroll + ch->level) / 300);
      }
   }
   if
   (
      get_skill(ch, gsn_enhanced_damage_two) > 0 &&
      has_skill(ch, gsn_enhanced_damage_two)
   )
   {
      int diceroll2 = number_percent();

      if ((diceroll2 <= get_skill(ch, gsn_enhanced_damage_two)))
      {
         check_improve(ch, gsn_enhanced_damage_two, TRUE, 3);
         diceroll2 = diceroll2 / 2;
         diceroll2 = UMAX(diceroll2, 20);
         dam += 2 * (dam * diceroll2 / 200);
      }
   }
   /* Skills and items */
   /* crushing blow */
   if
   (
      spear != NULL &&
      (
         spear->value[0] == WEAPON_STAFF ||
         spear->value[0] == WEAPON_MACE ||
         spear->value[0] == WEAPON_AXE
      ) &&
      has_skill(ch, gsn_crushingblow) &&
      number_percent() <= UMAX(get_skill(ch, gsn_crushingblow) / 8, 2)
   )
   {
      dam = 2 * dam + (dam * 2 * number_percent() / 100);
      send_to_char("You land a crushing blow!\n\r", ch);
      act("$n lands a crushing blow!", ch, NULL, NULL, TO_ROOM);
      check_improve(ch, gsn_crushingblow, TRUE, 1);
   }
   /* the bash */
   if (number_percent() < get_skill(ch, gsn_jump) / 2)
   {
      send_to_char("Your jump knocks them from their feet!\n\r", ch);
      send_to_char("The jump knocks you from your feet!\n\r", victim);
      WAIT_STATE(victim, 20);
   }
   if (mount)
   {
      /* the disarm */
      if
      (
         number_percent() < get_skill(ch, gsn_jump) / 4 &&
         (
            spear = get_eq_char(victim, WEAR_WIELD)
         ) != NULL
      )
      {
         disarm(ch, victim);
         if (get_eq_char(victim, WEAR_WIELD) != spear)
         {
            send_to_char
            (
               "Your jump knocks the weapon from their hand!\n\r",
               ch
            );
         }
      }
   }
   damage(ch, victim, dam, gsn_jump, dam_type, TRUE);
   return;
}

void do_book_open(CHAR_DATA* ch, char* argument)
{
   CHAR_DATA* book;
   OBJ_DATA* book_obj;
   sh_int chapter = 0;
   char arg1[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];
   bool was_closed = FALSE;

   argument = one_argument(argument, arg1);
   book = find_book(ch, arg1);
   book_obj = find_book_item(ch, arg1);
   if
   (
      book_obj == NULL &&
      book == NULL
   )
   {
      send_to_char("Open what book?\n\r", ch);
      return;
   }
   if (book_obj != NULL)
   {
      int chapter = 0;
      int cnt;
      bool title = FALSE;

      if (book_obj->value[3] == BOOK_LOCKED)
      {
         act("$p is locked.", ch, book_obj, NULL, TO_CHAR);
         return;
      }
      if (argument[0] == '\0')
      {
         if (book_obj->value[3] == BOOK_CLOSED)
         {
            chapter = 1;
         }
         else
         {
            act("$p is already open.", ch, book_obj, NULL, TO_CHAR);
            return;
         }
      }
      else if (is_number(argument))
      {
         chapter = atoi(argument);
         if
         (
            chapter < 1 ||
            chapter > BOOK_MAX_PAGES ||  /* Just in case.. for crashes */
            chapter > book_obj->value[0]
         )
         {
            sprintf
            (
               buf,
               "$p does not have that many %ss.",
               (
                  IS_SET(book_obj->value[4], BOOK_PAGES) ?
                  "page" :
                  "chapter"
               )
            );
            act(buf, ch, book_obj, NULL, TO_CHAR);
            return;
         }
      }
      else
      {
         for
         (
            cnt = 0;
            (
               cnt < book_obj->value[0] &&
               cnt < BOOK_MAX_PAGES
            );
            cnt++
         )
         {
            if (is_name(argument, book_obj->book_info->title[cnt]))
            {
               chapter = cnt + 1;
               title = TRUE;
               break;
            }
         }
         if (chapter == 0)
         {
            sprintf
            (
               buf,
               "$p does not have that %s.",
               (
                  IS_SET(book_obj->value[4], BOOK_PAGES) ?
                  "page" :
                  "chapter"
               )
            );
            act(buf, ch, book_obj, NULL, TO_CHAR);
            return;
         }
      }
      if (chapter == 0)  /* sanity check, shouldn't be possible */
      {
         sprintf
         (
            buf,
            "Open $p to what %s?",
            (
               IS_SET(book_obj->value[4], BOOK_PAGES) ?
               "page" :
               "chapter"
            )
         );
         act(buf, ch, book_obj, NULL, TO_CHAR);
         return;
      }
      chapter--;
      if (title)
      {
         sprintf
         (
            buf,
            "You open $p %sto %s.",
            (
               book_obj->value[3] == BOOK_CLOSED ?
               "and flip " :
               ""
            ),
            book_obj->book_info->title[chapter + 1]
         );
         sprintf
         (
            buf2,
            "$n opens $p %sto %s.",
            (
               book_obj->value[3] == BOOK_CLOSED ?
               "and flips " :
               ""
            ),
            book_obj->book_info->title[chapter + 1]
         );
      }
      else
      {
         sprintf
         (
            buf,
            "You open $p %sto %s %d.",
            (
               book_obj->value[3] == BOOK_CLOSED ?
               "and flip " :
               ""
            ),
            (
               IS_SET(book_obj->value[4], BOOK_PAGES) ?
               "page" :
               "chapter"
            ),
            chapter + 1
         );
         sprintf
         (
            buf2,
            "$n opens $p %sto %s %d.",
            (
               book_obj->value[3] == BOOK_CLOSED ?
               "and flip " :
               ""
            ),
            (
               IS_SET(book_obj->value[4], BOOK_PAGES) ?
               "page" :
               "chapter"
            ),
            chapter + 1
         );
      }
      act(buf, ch, book_obj, NULL, TO_CHAR);
      act(buf2, ch, book_obj, NULL, TO_ROOM);
      book_obj->value[3] = chapter;
      return;
   }
   if (book->position == POS_FIGHTING)
   {
      if (book != ch)
      {
         act
         (
            "$N is moving around too much.",
            ch,
            NULL,
            book,
            TO_CHAR
         );
      }
      else
      {
         send_to_char("You are moving around too much.\n\r", ch);
      }
      return;
   }
   if (book->pcdata->current_desc == BOOK_RACE_LOCKED)
   {
      /* Fizzfaldt is locked */
      if (ch != book)
      {
         act
         (
            "$N is locked.",
            ch,
            NULL,
            book,
            TO_CHAR
         );
      }
      else
      {
         send_to_char("You are locked.\n\r", ch);
      }
      return;
   }
   was_closed = book->pcdata->current_desc == BOOK_RACE_CLOSED;
   if
   (
      (
         argument[0] == '\0' &&
         !was_closed
      ) ||
      (
         argument[0] != '\0' &&
         !is_number(argument)
      )
   )
   {
      if (ch != book)
      {
         act
         (
            "Open $N to what chapter?",
            ch,
            NULL,
            book,
            TO_CHAR
         );
      }
      else
      {
         send_to_char("Open yourself to what chapter?\n\r", ch);
      }
      return;
   }
   if (argument[0] == '\0')
   {
      chapter = 1;
   }
   else
   {
      chapter = atoi(argument);
   }
   if (chapter < 1)
   {
      send_to_char("That chapter does not exist!\n\r", ch);
      return;
   }
   chapter++;
   /*
      chapter 1 is desc_list[2]
      [0] is closed
      [1] is locked
   */
   if (chapter >= MAX_DESCRIPTIONS)
   {
      if (ch != book)
      {
         act
         (
            "$N does not have that many chapters.",
            ch,
            NULL,
            book,
            TO_CHAR
         );
      }
      else
      {
         send_to_char("You do not have that many chapters.\n\r", ch);
      }
      return;
   }
   if (book->pcdata->current_desc == chapter)
   {
      if (ch != book)
      {
         act
         (
            "$N is already open to that chapter.",
            ch,
            NULL,
            book,
            TO_CHAR
         );
      }
      else
      {
         send_to_char("You are already open to that chapter.\n\r", ch);
      }
      return;
   }
   if
   (
      strlen(book->pcdata->desc_list[chapter]) == 0 &&
      book != ch
   )
   {
      /* Only Fizzfaldt can start a new chapter */
      act
      (
         "$N does not want to open to that chapter.",
         ch,
         NULL,
         book,
         TO_CHAR
      );
      return;
   }
   switch_desc(book, chapter);
   chapter--;  /* Output the correct chapter */
   if (was_closed)
   {
      if (book != ch)
      {
         sprintf
         (
            buf,
            "$n opens $N up and flips to chapter %d.",
            chapter
         );
         act
         (
            buf,
            ch,
            NULL,
            book,
            TO_NOTVICT
         );
         sprintf
         (
            buf,
            "You open $N up and flip to chapter %d.",
            chapter
         );
         act
         (
            buf,
            ch,
            NULL,
            book,
            TO_CHAR
         );
         sprintf
         (
            buf,
            "$n opens you up and flips to chapter %d.",
            chapter
         );
         act
         (
            buf,
            ch,
            NULL,
            book,
            TO_VICT
         );
      }
      else
      {
         sprintf
         (
            buf,
            "$n opens $mself up and flips to chapter %d.",
            chapter
         );
         act
         (
            buf,
            ch,
            NULL,
            book,
            TO_NOTVICT
         );
         sprintf
         (
            buf,
            "You open yourself up and flip to chapter %d.",
            chapter
         );
         act
         (
            buf,
            ch,
            NULL,
            book,
            TO_CHAR
         );
      }
   }
   else
   {
      if (book != ch)
      {
         sprintf
         (
            buf,
            "$n flips through $N to chapter %d.",
            chapter
         );
         act
         (
            buf,
            ch,
            NULL,
            book,
            TO_NOTVICT
         );
         sprintf
         (
            buf,
            "You flip through $N to chapter %d.",
            chapter
         );
         act
         (
            buf,
            ch,
            NULL,
            book,
            TO_CHAR
         );
         sprintf
         (
            buf,
            "$n flips through you to chapter %d.",
            chapter
         );
         act
         (
            buf,
            ch,
            NULL,
            book,
            TO_VICT
         );
      }
      else
      {
         sprintf
         (
            buf,
            "$n flaps $s wings as $e flips to chapter %d.",
            chapter
         );
         act
         (
            buf,
            ch,
            NULL,
            book,
            TO_NOTVICT
         );
         sprintf
         (
            buf,
            "You flap your wings as you flip to chapter %d.",
            chapter
         );
         act
         (
            buf,
            ch,
            NULL,
            book,
            TO_CHAR
         );
      }
   }
}

void do_book_close(CHAR_DATA* ch, char* argument)
{
   CHAR_DATA* book;
   OBJ_DATA* book_obj;

   book_obj = find_book_item(ch, argument);
   book = find_book(ch, argument);
   if
   (
      book_obj == NULL &&
      book == NULL
   )
   {
      send_to_char("Close what book?\n\r", ch);
      return;
   }
   if (book_obj != NULL)
   {
      if (book_obj->value[3] == BOOK_LOCKED)
      {
         act("$p is locked.", ch, book_obj, NULL, TO_CHAR);
         return;
      }
      if (book_obj->value[3] == BOOK_CLOSED)
      {
         act("$p is already closed.", ch, book_obj, NULL, TO_CHAR);
         return;
      }
      act("You close $p.", ch, book_obj, NULL, TO_CHAR);
      act("$n closes $p.", ch, book_obj, NULL, TO_ROOM);
      book_obj->value[3] = BOOK_CLOSED;
      return;
   }
   if (book->position == POS_FIGHTING)
   {
      if (book != ch)
      {
         act
         (
            "$N is moving around too much.",
            ch,
            NULL,
            book,
            TO_CHAR
         );
      }
      else
      {
         send_to_char("You are moving around too much.\n\r", ch);
      }
      return;
   }
   if
   (
      book->pcdata->current_desc == BOOK_RACE_CLOSED ||
      book->pcdata->current_desc == BOOK_RACE_LOCKED
   )
   {
      /* Fizzfaldt is already closed, or locked*/
      if (ch != book)
      {
         act
         (
            "$N is already closed.",
            ch,
            NULL,
            book,
            TO_CHAR
         );
      }
      else
      {
         send_to_char("You are already closed.\n\r", ch);
      }
      return;
   }
   /*
      chapter 1 is desc_list[2]
      [0] is closed
      [1] is locked
   */
   if
   (
      strlen(book->pcdata->desc_list[BOOK_RACE_CLOSED]) == 0 &&
      book != ch
   )
   {
      /* If closed description is missing, do not let mortals close */
      act
      (
         "$N does not want to open to close.",
         ch,
         NULL,
         book,
         TO_CHAR
      );
      return;
   }
   switch_desc(book, BOOK_RACE_CLOSED);
   if (book != ch)
   {
      act
      (
         "$n closes $N.",
         ch,
         NULL,
         book,
         TO_NOTVICT
      );
      act
      (
         "You close $N.",
         ch,
         NULL,
         book,
         TO_CHAR
      );
      act
      (
         "$n closes you.",
         ch,
         NULL,
         book,
         TO_VICT
      );
   }
   else
   {
      act
      (
         "$n closes $mself.",
         ch,
         NULL,
         book,
         TO_NOTVICT
      );
      act
      (
         "You close yourself.",
         ch,
         NULL,
         book,
         TO_CHAR
      );
   }
}

void do_book_lock(CHAR_DATA* ch, char* argument)
{
   OBJ_DATA* key;
   char full_keyname[MAX_INPUT_LENGTH];
   char* keyname;
   CHAR_DATA* book = find_book(ch, argument);
   OBJ_DATA* book_obj;

   book_obj = find_book_item(ch, argument);
   if
   (
      book_obj == NULL &&
      book == NULL
   )
   {
      send_to_char("Lock what book?\n\r", ch);
      return;
   }
   if (book_obj != NULL)
   {
      if (book_obj->value[3] == BOOK_LOCKED)
      {
         act("$p is already locked.", ch, book_obj, NULL, TO_CHAR);
         return;
      }
      if (book_obj->value[3] != BOOK_CLOSED)
      {
         act("$p is not closed.", ch, book_obj, NULL, TO_CHAR);
         return;
      }
      if (book_obj->value[2] == -1)
      {
         act("$p does not have a lock.", ch, book_obj, NULL, TO_CHAR);
         return;
      }
      key = find_book_key(ch, NULL, book_obj);
      if (key == NULL)
      {
         return;
      }
      if (key->short_descr)
      {
         strcpy(full_keyname, key->short_descr);
         if
         (
            UPPER(full_keyname[0]) == 'A' &&
            full_keyname[1] == ' '
         )
         {
            keyname = full_keyname + 2;
         }
         else if
         (
            UPPER(full_keyname[0]) == 'A' &&
            UPPER(full_keyname[1]) == 'N' &&
            full_keyname[2] == ' '
         )
         {
            keyname = full_keyname + 3;
         }
         else if
         (
            UPPER(full_keyname[0]) == 'T' &&
            UPPER(full_keyname[1]) == 'H' &&
            UPPER(full_keyname[1]) == 'E' &&
            full_keyname[3] == ' '
         )
         {
            keyname = full_keyname + 4;
         }
         else
         {
            keyname = full_keyname;
         }
      }
      else
      {
         strcpy(full_keyname, "key");
         keyname = full_keyname;
      }
      act("You lock $p with your $T.", ch, book_obj, keyname, TO_CHAR);
      act("$n locks $p with $s $T.", ch, book_obj, keyname, TO_ROOM);
      book_obj->value[3] = BOOK_LOCKED;
      return;
   }
   if (book->position == POS_FIGHTING)
   {
      if (book != ch)
      {
         act
         (
            "$N is moving around too much.",
            ch,
            NULL,
            book,
            TO_CHAR
         );
      }
      else
      {
         send_to_char("You are moving around too much.\n\r", ch);
      }
      return;
   }
   key = find_book_key(ch, book, NULL);
   if (key == NULL)
   {
      return;
   }
   if (book->pcdata->current_desc == BOOK_RACE_LOCKED)
   {
      /* Fizzfaldt is already locked */
      if (ch != book)
      {
         act
         (
            "$N is already locked.",
            ch,
            NULL,
            book,
            TO_CHAR
         );
      }
      else
      {
         send_to_char("You are already locked.\n\r", ch);
      }
      return;
   }
   if (book->pcdata->current_desc != BOOK_RACE_CLOSED)
   {
      /* Fizzfaldt is not closed */
      if (ch != book)
      {
         act
         (
            "$N is not closed.",
            ch,
            NULL,
            book,
            TO_CHAR
         );
      }
      else
      {
         send_to_char("You are not closed.\n\r", ch);
      }
      return;
   }
   /*
      chapter 1 is desc_list[2]
      [0] is closed
      [1] is locked
   */
   if
   (
      strlen(book->pcdata->desc_list[BOOK_RACE_LOCKED]) == 0 &&
      book != ch
   )
   {
      /* If locked description is missing, do not let mortals lock */
      send_to_char("Fizzfaldt does not want to be locked.\n\r", ch);
      return;
   }
   switch_desc(book, BOOK_RACE_LOCKED);
   if (key->short_descr)
   {
      strcpy(full_keyname, key->short_descr);
      if
      (
         UPPER(full_keyname[0]) == 'A' &&
         full_keyname[1] == ' '
      )
      {
         keyname = full_keyname + 2;
      }
      else if
      (
         UPPER(full_keyname[0]) == 'A' &&
         UPPER(full_keyname[1]) == 'N' &&
         full_keyname[2] == ' '
      )
      {
         keyname = full_keyname + 3;
      }
      else if
      (
         UPPER(full_keyname[0]) == 'T' &&
         UPPER(full_keyname[1]) == 'H' &&
         UPPER(full_keyname[1]) == 'E' &&
         full_keyname[3] == ' '
      )
      {
         keyname = full_keyname + 4;
      }
      else
      {
         keyname = full_keyname;
      }
   }
   else
   {
      strcpy(full_keyname, "key");
      keyname = full_keyname;
   }
   if (book != ch)
   {
      act
      (
         "$n locks $N with $s $t.",
         ch,
         keyname,
         book,
         TO_NOTVICT
      );
      act
      (
         "You lock $N with your $t.",
         ch,
         keyname,
         book,
         TO_CHAR
      );
      act
      (
         "$n locks you with $s $t.",
         ch,
         keyname,
         book,
         TO_VICT
      );
   }
   else
   {
      act
      (
         "$n locks $mself with $s $t.",
         ch,
         keyname,
         book,
         TO_NOTVICT
      );
      act
      (
         "You lock yourself with your $t.",
         ch,
         keyname,
         book,
         TO_CHAR
      );
   }
}

void do_book_unlock(CHAR_DATA* ch, char* argument)
{
   OBJ_DATA* key;
   char full_keyname[MAX_INPUT_LENGTH];
   char* keyname;
   CHAR_DATA* book = find_book(ch, argument);
   OBJ_DATA* book_obj;

   book_obj = find_book_item(ch, argument);
   if
   (
      book_obj == NULL &&
      book == NULL
   )
   {
      send_to_char("Unlock what book?\n\r", ch);
      return;
   }
   if (book_obj != NULL)
   {
      if (book_obj->value[2] == -1)
      {
         act("$p does not have a lock.", ch, book_obj, NULL, TO_CHAR);
         return;
      }
      if (book_obj->value[3] != BOOK_LOCKED)
      {
         act("$p is not locked.", ch, book_obj, NULL, TO_CHAR);
         return;
      }
      key = find_book_key(ch, NULL, book_obj);
      if (key == NULL)
      {
         return;
      }
      if (key->short_descr)
      {
         strcpy(full_keyname, key->short_descr);
         if
         (
            UPPER(full_keyname[0]) == 'A' &&
            full_keyname[1] == ' '
         )
         {
            keyname = full_keyname + 2;
         }
         else if
         (
            UPPER(full_keyname[0]) == 'A' &&
            UPPER(full_keyname[1]) == 'N' &&
            full_keyname[2] == ' '
         )
         {
            keyname = full_keyname + 3;
         }
         else if
         (
            UPPER(full_keyname[0]) == 'T' &&
            UPPER(full_keyname[1]) == 'H' &&
            UPPER(full_keyname[1]) == 'E' &&
            full_keyname[3] == ' '
         )
         {
            keyname = full_keyname + 4;
         }
         else
         {
            keyname = full_keyname;
         }
      }
      else
      {
         strcpy(full_keyname, "key");
         keyname = full_keyname;
      }
      act("You unlock $p with your $T.", ch, book_obj, keyname, TO_CHAR);
      act("$n unlocks $p with $s $T.", ch, book_obj, keyname, TO_ROOM);
      book_obj->value[3] = BOOK_CLOSED;
      return;
   }
   if (book->position == POS_FIGHTING)
   {
      if (book != ch)
      {
         act
         (
            "$N is moving around too much.",
            ch,
            NULL,
            book,
            TO_CHAR
         );
      }
      else
      {
         send_to_char("You are moving around too much.\n\r", ch);
      }
      return;
   }
   key = find_book_key(ch, book, NULL);
   if (key == NULL)
   {
      return;
   }
   if (book->pcdata->current_desc != BOOK_RACE_LOCKED)
   {
      /* Fizzfaldt is not locked */
      if (ch != book)
      {
         act
         (
            "$N is not locked.",
            ch,
            NULL,
            book,
            TO_CHAR
         );
      }
      else
      {
         send_to_char("You are not locked.\n\r", ch);
      }
      return;
   }
   /*
      chapter 1 is desc_list[2]
      [0] is closed
      [1] is locked
   */
   if
   (
      strlen(book->pcdata->desc_list[0]) == BOOK_RACE_CLOSED &&
      book != ch
   )
   {
      /* If closed description is missing, do not let mortals unlock */
      act
      (
         "$N does not want to be unlocked.",
         ch,
         NULL,
         book,
         TO_CHAR
      );
      return;
   }
   switch_desc(book, 0);
   if (key->short_descr)
   {
      strcpy(full_keyname, key->short_descr);
      if
      (
         UPPER(full_keyname[0]) == 'A' &&
         full_keyname[1] == ' '
      )
      {
         keyname = full_keyname + 2;
      }
      else if
      (
         UPPER(full_keyname[0]) == 'A' &&
         UPPER(full_keyname[1]) == 'N' &&
         full_keyname[2] == ' '
      )
      {
         keyname = full_keyname + 3;
      }
      else if
      (
         UPPER(full_keyname[0]) == 'T' &&
         UPPER(full_keyname[1]) == 'H' &&
         UPPER(full_keyname[1]) == 'E' &&
         full_keyname[3] == ' '
      )
      {
         keyname = full_keyname + 4;
      }
      else
      {
         keyname = full_keyname;
      }
   }
   else
   {
      strcpy(full_keyname, "key");
      keyname = full_keyname;
   }
   if (book != ch)
   {
      act
      (
         "$n unlocks $N with $s $t.",
         ch,
         keyname,
         book,
         TO_NOTVICT
      );
      act
      (
         "You unlock $N with your $t.",
         ch,
         keyname,
         book,
         TO_CHAR
      );
      act
      (
         "$n unlocks you with $s $t.",
         ch,
         keyname,
         book,
         TO_VICT
      );
   }
   else
   {
      act
      (
         "$n unlocks $mself with $s $t.",
         ch,
         keyname,
         book,
         TO_NOTVICT
      );
      act
      (
         "You unlock yourself with your $t.",
         ch,
         keyname,
         book,
         TO_CHAR
      );
   }
}

void do_book_pick(CHAR_DATA* ch, char* argument)
{
   CHAR_DATA* book = find_book(ch, argument);
   OBJ_DATA* book_obj;

   book_obj = find_book_item(ch, argument);
   if
   (
      book_obj == NULL &&
      book == NULL
   )
   {
      send_to_char("Pick the lock on what book?\n\r", ch);
      return;
   }
   if (book_obj != NULL)
   {
      if (book_obj->value[2] == -1)
      {
         act("$p does not have a lock.", ch, book_obj, NULL, TO_CHAR);
         return;
      }
      if (book_obj->value[3] != BOOK_LOCKED)
      {
         act("$p is not locked.", ch, book_obj, NULL, TO_CHAR);
         return;
      }
      act("You pick the lock on $p.", ch, book_obj, NULL, TO_CHAR);
      act("$n picks the lock on $p.", ch, book_obj, NULL, TO_ROOM);
      book_obj->value[3] = BOOK_CLOSED;
      return;
   }
   if (book->position == POS_FIGHTING)
   {
      if (book != ch)
      {
         act
         (
            "$N is moving around too much.",
            ch,
            NULL,
            book,
            TO_CHAR
         );
      }
      else
      {
         send_to_char("You are moving around too much.\n\r", ch);
      }
      return;
   }
   if (book->pcdata->current_desc != BOOK_RACE_LOCKED)
   {
      /* Fizzfaldt is not locked */
      if (ch != book)
      {
         act
         (
            "$N is not locked.",
            ch,
            NULL,
            book,
            TO_CHAR
         );
      }
      else
      {
         send_to_char("You are not locked.\n\r", ch);
      }
      return;
   }
   /*
      chapter 1 is desc_list[2]
      [0] is closed
      [1] is locked
   */
   if
   (
      strlen(book->pcdata->desc_list[0]) == BOOK_RACE_CLOSED &&
      book != ch
   )
   {
      /* If closed description is missing, do not let mortals unlock */
      act
      (
         "$N does not want to be unlocked.",
         ch,
         NULL,
         book,
         TO_CHAR
      );
      return;
   }
   if (get_skill(ch, gsn_pick_lock) * 8 / 10 < number_percent())
   {
      send_to_char("You cannot manage to pick the tiny lock.\n\r", ch);
      return;
   }
   switch_desc(book, 0);
   if (book != ch)
   {
      act
      (
         "$n picks the lock on $N.",
         ch,
         NULL,
         book,
         TO_NOTVICT
      );
      act
      (
         "You pick the lock on $N.",
         ch,
         NULL,
         book,
         TO_CHAR
      );
      act
      (
         "$n picks your lock.",
         ch,
         NULL,
         book,
         TO_VICT
      );
   }
   else
   {
      act
      (
         "$n picks $s own lock.",
         ch,
         NULL,
         book,
         TO_NOTVICT
      );
      act
      (
         "You pick your lock.",
         ch,
         NULL,
         book,
         TO_CHAR
      );
   }
}

void do_book_sign(CHAR_DATA* ch, char* argument)
{
   OBJ_DATA* book_obj;
   CHAR_DATA* book;
   OBJ_DATA* pen = find_book_pen(ch);
   char buf[MAX_INPUT_LENGTH];
   char full_penname[MAX_INPUT_LENGTH];
   char arg[MAX_INPUT_LENGTH];
   char* penname;
   int lines;
   char* pchar;

   argument = one_argument_space(argument, arg);
   book = find_book(ch, arg);
   book_obj = find_book_item(ch, arg);

   if (argument[0] == '\0')
   {
      send_to_char("Sign what?\n\r", ch);
      return;
   }
   if
   (
      book_obj == NULL &&
      book == NULL
   )
   {
      send_to_char("Sign what book?\n\r", ch);
      return;
   }
   if
   (
      strlen(argument) > 52 ||
      (
         (
            !IS_IMMORTAL(ch) ||
            book_obj == NULL
         ) &&
         strlen(argument) > 50
      )
   )
   {
      send_to_char
      (
         "That will not fit on a single line.\n\r",
         ch
      );
      return;
   }
   if (pen == NULL)
   {
      send_to_char("You do not have anything to write with.\n\r", ch);
      return;
   }
   if
   (
      !IS_IMMORTAL(ch) &&
      pen->value[1] == 0
   )
   {
      sprintf
      (
         buf,
         "$p does not have enough %s left to write that.",
         inkwell_table[pen->value[2]]
      );
      act(buf, ch, pen, NULL, TO_CHAR);
      return;
   }
   /* Check closed/locked */
   if (book_obj == NULL)
   {
      if (book->pcdata->current_desc == BOOK_RACE_LOCKED)
      {
         if (ch != book)
         {
            act
            (
               "$N is locked.",
               ch,
               NULL,
               book,
               TO_CHAR
            );
         }
         else
         {
            send_to_char("You are locked.\n\r", ch);
         }
         return;
      }
      if (book->pcdata->current_desc == BOOK_RACE_CLOSED)
      {
         if (ch != book)
         {
            act
            (
               "$N is closed.",
               ch,
               NULL,
               book,
               TO_CHAR
            );
         }
         else
         {
            send_to_char("You are closed.\n\r", ch);
         }
         return;
      }
   }
   else
   {
      if (book_obj->value[3] == BOOK_LOCKED)
      {
         act("$p is locked.", ch, book_obj, NULL, TO_CHAR);
         return;
      }
      if (book_obj->value[3] == BOOK_CLOSED)
      {
         act("$p is closed.", ch, book_obj, NULL, TO_CHAR);
         return;
      }
   }
   /* Check fighting */
   if
   (
      book_obj == NULL &&
      book->position == POS_FIGHTING
   )
   {
      if (book != ch)
      {
         act
         (
            "$N is moving around too much.",
            ch,
            NULL,
            book,
            TO_CHAR
         );
      }
      else
      {
         send_to_char("You are moving around too much.\n\r", ch);
      }
      return;
   }
   /* Check writability */
   if
   (
      book_obj != NULL &&
      (
         !IS_SET(book_obj->value[4], BOOK_WRITABLE) ||
         (
            !IS_IMMORTAL(ch) &&
            IS_SET(book_obj->value[4], BOOK_IMM_ONLY)
         )
      )
   )
   {
      act("You cannot write upon $p.", ch, book_obj, NULL, TO_CHAR);
      return;
   }
   /* Check length */
   if (book_obj != NULL)
   {
      int max_lines;

      lines = 0;
      pchar = book_obj->book_info->page[book_obj->value[3]];
      for (; *pchar != '\0'; pchar++)
      {
         if (*pchar == '\n')
         {
            lines++;
         }
      }
      max_lines = MAX_DESC_LENGTH / 52 * book_obj->value[1] / 4;
      if
      (
         max_lines < 0 ||
         max_lines > MAX_DESC_LENGTH / 52
      )
      {
         bug("do_book_sign: max_lines %d", max_lines);
         return;
      }
      if (lines > max_lines)
      {
         sprintf
         (
            buf,
            "There is not enough room on this %s.\n\r",
            (
               IS_SET(book_obj->value[4], BOOK_PAGES) ?
               "page" :
               "chapter"
            )
         );
         send_to_char(buf, ch);
         return;
      }
   }
   else
   {
      if (strlen(book->description) + strlen(buf) + 2 >= MAX_DESC_LENGTH)
      {
         send_to_char("There is not enough room in this chapter.\n\r", ch);
         return;
      }
      if
      (
         book->pcdata->current_desc + 1 < MAX_DESCRIPTIONS &&
         stristr
         (
            book->pcdata->desc_list[book->pcdata->current_desc + 1],
            "The book seems to be open to Chapter"
         ) != NULL
      )
      {
         send_to_char("This chapter is already finished!\n\r", ch);
         return;
      }
      if
      (
         stristr
         (
            book->description,
            "The book seems to be open to Chapter"
         ) == NULL
      )
      {
         send_to_char("This chapter is not started yet.\n\r", ch);
         return;
      }
   }
   if (pen->short_descr)
   {
      strcpy(full_penname, pen->short_descr);
      if
      (
         UPPER(full_penname[0]) == 'A' &&
         full_penname[1] == ' '
      )
      {
         penname = full_penname + 2;
      }
      else if
      (
         UPPER(full_penname[0]) == 'A' &&
         UPPER(full_penname[1]) == 'N' &&
         full_penname[2] == ' '
      )
      {
         penname = full_penname + 3;
      }
      else if
      (
         UPPER(full_penname[0]) == 'T' &&
         UPPER(full_penname[1]) == 'H' &&
         UPPER(full_penname[2]) == 'E' &&
         full_penname[3] == ' '
      )
      {
         penname = full_penname + 4;
      }
      else
      {
         penname = full_penname;
      }
   }
   else
   {
      strcpy(full_penname, "pen");
      penname = full_penname;
   }
   if (book_obj == NULL)
   {
      sprintf
      (
         buf,
         "+ %s",
         argument
      );
      do_book_description(book, buf);
      if (pen->value[1] != -1)
      {
         --pen->value[1];
      }
      if (book != ch)
      {
         sprintf
         (
            buf,
            "$n writes on $N with $s %s in %s %s.",
            penname,
            pencolor_table[pen->value[0]],
            inkwell_table[pen->value[2]]
         );
         act
         (
            buf,
            ch,
            NULL,
            book,
            TO_NOTVICT
         );
         sprintf
         (
            buf,
            "You write on $N with your %s in %s %s.",
            penname,
            pencolor_table[pen->value[0]],
            inkwell_table[pen->value[2]]
         );
         act
         (
            buf,
            ch,
            NULL,
            book,
            TO_CHAR
         );
         sprintf
         (
            buf,
            "$n writes on you with $s %s in %s %s.",
            penname,
            pencolor_table[pen->value[0]],
            inkwell_table[pen->value[2]]
         );
         act
         (
            buf,
            ch,
            NULL,
            book,
            TO_VICT
         );
      }
      else
      {
         sprintf
         (
            buf,
            "$n begins to scribble on $mself with $s %s in %s %s.",
            penname,
            pencolor_table[pen->value[0]],
            inkwell_table[pen->value[2]]
         );
         act
         (
            buf,
            ch,
            NULL,
            NULL,
            TO_ROOM
         );
         sprintf
         (
            buf,
            "You scribble on yourself with your %s in %s %s.",
            penname,
            pencolor_table[pen->value[0]],
            inkwell_table[pen->value[2]]
         );
         act
         (
            buf,
            ch,
            penname,
            NULL,
            TO_CHAR
         );
      }
   }
   else
   {
      char* temp_desc;

      /* Use description command */
      temp_desc = ch->description;
      ch->description = book_obj->book_info->page[book_obj->value[3]];
      if (IS_IMMORTAL(ch))
      {
         /* Can delete lines and stuff */
         do_description(ch, argument);
      }
      else
      {
         if
         (
            !str_cmp(argument, "<blank>") ||
            !str_cmp(argument, "+")
         )
         {
            do_description(ch, "+");
            /* next two lines restore description */
            book_obj->book_info->page[book_obj->value[3]] = ch->description;
            ch->description = temp_desc;
            if (IS_SET(book_obj->value[4], BOOK_SAVE))
            {
               (
                  book_obj->pIndexData->book_info->book_save_info
               )->modified[book_obj->value[3]] = TRUE;
               save_modified_book(book_obj);
            }
            return;
         }
         else
         {
            sprintf
            (
               buf,
               "+ %s",
               argument
            );
            do_description(ch, buf);
            if (pen->value[1] != -1)
            {
               --pen->value[1];
            }
         }
      }
      /* Keep track of this object. */
      site_check_from(ch, book_obj);
      /* next two lines restore description */
      book_obj->book_info->page[book_obj->value[3]] = ch->description;
      ch->description = temp_desc;
      if (IS_SET(book_obj->value[4], BOOK_SAVE))
      {
         (
            book_obj->pIndexData->book_info->book_save_info
         )->modified[book_obj->value[3]] = TRUE;
         save_modified_book(book_obj);
      }
      sprintf
      (
         buf,
         "$n writes on $p with $s %s in %s %s.",
         penname,
         pencolor_table[pen->value[0]],
         inkwell_table[pen->value[2]]
      );
      act
      (
         buf,
         ch,
         book_obj,
         NULL,
         TO_ROOM
      );
      sprintf
      (
         buf,
         "You write on $p with your %s in %s %s.",
         penname,
         pencolor_table[pen->value[0]],
         inkwell_table[pen->value[2]]
      );
      act
      (
         buf,
         ch,
         book_obj,
         NULL,
         TO_CHAR
      );
   }
}

void do_book_read(CHAR_DATA* ch, char* argument)
{
   CHAR_DATA* book = find_book(ch, argument);
   OBJ_DATA* book_obj = find_book_item(ch, argument);
   char* start;
   char buf[MAX_STRING_LENGTH];

   if
   (
      book_obj == NULL &&
      book == NULL
   )
   {
      send_to_char("Read what?\n\r", ch);
      return;
   }
   if (book_obj != NULL)
   {
      if (book_obj->value[3] == BOOK_CLOSED)
      {
         act("$p is closed.", ch, book_obj, NULL, TO_CHAR);
         return;
      }
      if (book_obj->value[3] == BOOK_LOCKED)
      {
         act("$p is closed and locked shut.", ch, book_obj, NULL, TO_CHAR);
         return;
      }
      if (book_obj->book_info->title[book_obj->value[3]][0] != '\0')
      {
         sprintf
         (
            buf,
            "You begin reading %s from $p.",
            book_obj->book_info->title[book_obj->value[3]]
         );
         act(buf, ch, book_obj, NULL, TO_CHAR);
         sprintf
         (
            buf,
            "$n begins reading %s from $p.",
            book_obj->book_info->title[book_obj->value[3]]
         );
         act(buf, ch, book_obj, NULL, TO_ROOM);
      }
      else
      {
         sprintf
         (
            buf,
            "You begin reading %s %d from $p.",
            (
               IS_SET(book_obj->value[4], BOOK_PAGES) ?
               "page" :
               "chapter"
            ),
            book_obj->value[3] + 1
         );
         act(buf, ch, book_obj, NULL, TO_CHAR);
         sprintf
         (
            buf,
            "$n begins reading %s %d from $p.",
            (
               IS_SET(book_obj->value[4], BOOK_PAGES) ?
               "page" :
               "chapter"
            ),
            book_obj->value[3] + 1
         );
         act(buf, ch, book_obj, NULL, TO_ROOM);
      }
      page_to_char(book_obj->book_info->page[book_obj->value[3]], ch);
      return;
   }
   if (book->position == POS_FIGHTING)
   {
      if (book != ch)
      {
         act
         (
            "$N is moving around too much.",
            ch,
            NULL,
            book,
            TO_CHAR
         );
      }
      else
      {
         send_to_char("You are moving around too much.\n\r", ch);
      }
      return;
   }
   if (book->pcdata->current_desc == BOOK_RACE_LOCKED)
   {
      /* Fizzfaldt is locked */
      if (ch != book)
      {
         act
         (
            "$N is locked.",
            ch,
            NULL,
            book,
            TO_CHAR
         );
      }
      else
      {
         send_to_char("You are locked.\n\r", ch);
      }
      return;
   }
   if (book->pcdata->current_desc == BOOK_RACE_CLOSED)
   {
      /* Fizzfaldt is locked */
      if (ch != book)
      {
         act
         (
            "$N is closed.",
            ch,
            NULL,
            book,
            TO_CHAR
         );
      }
      else
      {
         send_to_char("You are closed.\n\r", ch);
      }
      return;
   }
   start = stristr(book->description, "The book seems to be open to Chapter");
   if (start == NULL)
   {
      send_to_char("You cannot make out the writing on the page.\n\r", ch);
      return;
   }
   if (book != ch)
   {
      act
      (
         "You begin to read $N.",
         ch,
         NULL,
         book,
         TO_CHAR
      );
      act
      (
         "$n begins to read $N.",
         ch,
         NULL,
         book,
         TO_NOTVICT
      );
      act
      (
         "$n begins to read you.",
         ch,
         NULL,
         book,
         TO_VICT
      );
   }
   else
   {
      send_to_char("You try to read yourself.\n\r", ch);
   }
   send_to_char(start, ch);
}

OBJ_DATA* find_book_item(CHAR_DATA* ch, char* argument)
{
   OBJ_DATA* obj;

   if
   (
      (
         (
            obj = get_obj_list(ch, argument, ch->carrying)
         ) == NULL ||
         obj->item_type != ITEM_BOOK
      ) &&
      (
         (
            obj = get_obj_list(ch, argument, ch->in_room->contents)
         ) == NULL ||
         obj->item_type != ITEM_BOOK
      )
   )
   {
      return NULL;
   }
   return obj;
}

CHAR_DATA* find_book(CHAR_DATA* ch, char* argument)
{
   CHAR_DATA* book;

   for (book = ch->in_room->people; book; book = book->next_in_room)
   {
      if
      (
         IS_NPC(book) ||
         book->race != grn_book ||
         !can_see(ch, book) ||
         (
            !str_cmp(argument, "self") &&
            ch != book
         ) ||
         (
            str_cmp(argument, "self") &&
            !is_name(argument, get_name(book, ch))
         )
      )
      {
         continue;
      }
      return book;
   }
   return NULL;
}

OBJ_DATA* find_book_key(CHAR_DATA* ch, CHAR_DATA* book, OBJ_DATA* book_obj)
{
   OBJ_DATA* key;
   int vnum;

   if (book)
   {
      switch (book->id)
      {
         case (1080493653):  /* Daenim */
         {
            vnum = 22474;  /* Key for Daenim sold in GML */
            break;
         }
         case (ID_BUK):
         default:
         {
            vnum = -1;
            break;
         }
      }
   }
   else if (book_obj)
   {
      if (book_obj->value[2] != 0)
      {
         vnum = book_obj->value[2];
      }
      else
      {
         vnum = -1;
      }
   }
   else
   {
      bug("Find_book_key(ch, NULL, NULL)!", 0);
      return NULL;
   }

   for (key = ch->carrying; key != NULL; key = key->next_content )
   {
      if
      (
         can_see_obj(ch, key) &&
         (
            (
               vnum != -1 &&
               key->pIndexData->vnum == vnum
            ) ||
            (
               vnum == -1 &&
               key->item_type == ITEM_KEY
            )
         )
      )
      {
         return key;
      }
   }
   if (book_obj)
   {
      act("You do not have the key for $p.", ch, book_obj, NULL, TO_CHAR);
   }
   else
   {
      if (vnum == -1)
      {
         send_to_char("You do not have any keys!\n\r", ch);
      }
      else
      {
         if (ch == book)
         {
            send_to_char
            (
               "You do not have the right key to unlock yourself.\n\r",
               ch
            );
         }
         else
         {
            act
            (
               "You do not have the right key to unlock $N.",
               ch,
               NULL,
               book,
               TO_CHAR
            );
         }
      }
   }
   return NULL;
}

OBJ_DATA* find_book_pen(CHAR_DATA* ch)
{
   OBJ_DATA* pen;

   for (pen = ch->carrying; pen != NULL; pen = pen->next_content )
   {
      if
      (
         can_see_obj(ch, pen) &&
         pen->item_type == ITEM_PEN
      )
      {
         return pen;
      }
   }
   return NULL;
}

void do_recruit(CHAR_DATA* ch, char* argument)
{
   char arg1[MAX_INPUT_LENGTH];
   AFFECT_DATA af;
   sh_int chance;
   sh_int count;
   CHAR_DATA* victim = NULL;

   if
   (
      ch->house != HOUSE_COVENANT ||
      IS_NPC(ch) ||
      !has_skill(ch, gsn_recruit) ||
      (
         chance = get_skill(ch, gsn_recruit)
      ) < 1
   )
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }
   if (ch->pcdata->house_rank == RANK_SQUIRE)
   {
      send_to_char("You cannot recruit as a squire.\n\r", ch);
      return;
   }
   if
   (
      house_down(ch, HOUSE_COVENANT) ||
      check_peace(ch)
   )
   {
      return;
   }
   argument = one_argument(argument, arg1);
   if (arg1[0] == '\0')
   {
      send_to_char("Recruit whom?\n\r", ch);
      return;
   }
   if (!check_room(ch, TRUE, "purge in recruit"))
   {
      return;
   }
   if (ch->mana < skill_table[gsn_recruit].min_mana)
   {
      send_to_char
      (
         "Your mind is too tired to convince others to join you.\n\r",
         ch
      );
      return;
   }
   if (!str_cmp(arg1, "rally"))
   {
      sh_int mob_out = 0;
      sh_int mob_in  = 0;

      for (victim = char_list; victim != NULL; victim = victim->next)
      {
         if
         (
            IS_NPC(victim) &&
            IS_AFFECTED(victim, AFF_CHARM) &&
            victim->master == ch
         )
         {
            if (victim->in_room != ch->in_room)
            {
               mob_out++;
            }
            else
            {
               mob_in++;
            }
         }
      }
      if (mob_out + mob_in == 0)
      {
         send_to_char("You have no soldiers to rally.\n\r", ch);
         return;
      }
      if (mob_out == 0)
      {
         send_to_char("Your soldiers are rallied here already.\n\r", ch);
         return;
      }
      if (number_percent() > chance)
      {
         send_to_char("You failed to rally your troops.\n\r", ch);
         check_improve(ch, gsn_recruit, FALSE, 1);
         ch->mana -= skill_table[gsn_recruit].min_mana / 2;
         WAIT_STATE(ch, skill_table[gsn_recruit].beats / 2);
         return;
      }
      check_improve(ch, gsn_recruit, TRUE, 1);
      ch->mana -= skill_table[gsn_recruit].min_mana;
      WAIT_STATE(ch, skill_table[gsn_recruit].beats);
      act("$n calls out to rally $s troops.", ch, NULL, NULL, TO_ROOM );
      send_to_char("You call out to rally your troops.\n\r", ch);
      for (victim = char_list; victim != NULL; victim = victim->next)
      {
         if
         (
            IS_NPC(victim) &&
            IS_AFFECTED(victim, AFF_CHARM) &&
            victim->master == ch &&
            victim->in_room != ch->in_room
         )
         {
            if (victim->in_room != NULL)
            {
               act
               (
                  "$n rushes off with great morale!",
                  victim,
                  NULL,
                  NULL,
                  TO_ROOM
               );
            }
            char_from_room(victim);
            char_to_room(victim, ch->in_room);
            act
            (
               "$N rallies together with $n.",
               ch,
               NULL,
               victim,
               TO_ROOM
            );
            act
            (
               "$n rallies with you.",
               victim,
               NULL,
               ch,
               TO_VICT
            );
         }
      }
      return;
   }
   if (is_affected(ch, gsn_recruit))
   {
      send_to_char
      (
         "You need more time before you can recruit again.\n\r",
         ch
      );
      return;
   }

   for (victim = char_list; victim != NULL; victim = victim->next)
   {
      if
      (
         IS_NPC(victim) &&
         victim->master == ch &&
         (
            victim->pIndexData->vnum == MOB_VNUM_COVENANT_AUXILIARY ||
            victim->pIndexData->vnum == MOB_VNUM_COVENANT_NIGHTMARE ||
            victim->pIndexData->vnum == MOB_VNUM_COVENANT_DRAGON ||
            is_affected(victim, gsn_recruit)
         )
      )
      {
         send_to_char
         (
            "Take care of your soldiers before recruiting more.\n\r",
            ch
         );
         return;
      }
   }
   if (number_percent() > chance)
   {
      act
      (
         "$n makes a fool of $mself trying to recruit aid.",
         ch,
         NULL,
         NULL,
         TO_ROOM
      );
      send_to_char("Your efforts to recruit aid fall upon deaf ears.\n\r", ch);
      check_improve(ch, gsn_recruit, FALSE, 1);
      ch->mana -= skill_table[gsn_recruit].min_mana / 2;
      WAIT_STATE(ch, skill_table[gsn_recruit].beats / 2);
      return;
   }
   af.where      = TO_AFFECTS;
   af.level      = ch->level;
   af.location   = APPLY_NONE;
   af.modifier   = 0;
   af.duration   = 12;
   af.bitvector  = 0;
   af.bitvector2 = 0;
   af.type       = gsn_recruit;
   if (!str_cmp(arg1, "auxiliaries"))
   {
      send_to_char("A pair of Covenant Auxiliaries arrive to serve you.\n\r", ch);
      send_to_char("A pair of Covenant Auxiliaries salute you.\n\r", ch);
      for (count = 0; count < 2; count++)
      {
         victim = create_mobile(get_mob_index(MOB_VNUM_COVENANT_AUXILIARY));
         victim->level     = ch->level;
         victim->damroll  += ch->level / 2;
         victim->hitroll  += ch->level / 2;
         victim->max_hit   = ch->level * 16 + number_range(-150, 150);
         victim->hit       = victim->max_hit;
         victim->max_move  = ch->max_move;
         victim->move      = victim->max_move;
         victim->alignment = ch->alignment;
         char_to_room(victim, ch->in_room);
         SET_BIT(victim->affected_by, AFF_CHARM);
         add_follower(victim, ch);
         victim->leader = ch;
      }
      act
      (
         "$n attempts to recruit a pair of Covenant Auxiliaries!'",
         ch,
         NULL,
         NULL,
         TO_ROOM
      );
      act
      (
         "A pair of Covenant Auxiliaries arrive to serve $n.\n\r"
         "A pair of Covenant Auxiliaries salute $n.",
         ch,
         NULL,
         NULL,
         TO_ROOM
      );
      send_to_char("A pair of Covenant Auxiliaries are ready to heed your orders.\n\r", ch);
      check_improve(ch, gsn_recruit, TRUE, 1);
      ch->mana -= skill_table[gsn_recruit].min_mana;
      WAIT_STATE(ch, skill_table[gsn_recruit].beats);
      affect_to_char(ch, &af);
      return;
   }
   if (!str_cmp(arg1, "mount"))
   {
      if (ch->pcdata->house_rank >= RANK_BRIGADIERGENERAL)
      {
         victim = create_mobile(get_mob_index(MOB_VNUM_COVENANT_DRAGON));
      }
      else
      {
         victim = create_mobile(get_mob_index(MOB_VNUM_COVENANT_NIGHTMARE));
      }
      victim->level     = ch->level;
      victim->damroll  += ch->level / 2;
      victim->hitroll  += ch->level / 2;
      victim->max_hit   = ch->level * 16 + number_range(-150, 150);
      victim->hit       = victim->max_hit;
      victim->max_move  = ch->max_move;
      victim->move      = victim->max_move;
      victim->alignment = ch->alignment;
      victim->armor[0]  = ch->level * -2;
      victim->armor[1]  = ch->level * -2;
      victim->armor[2]  = ch->level * -2;
      victim->armor[3]  = ch->level * -1;
      char_to_room(victim, ch->in_room);
      SET_BIT(victim->affected_by, AFF_CHARM);
      add_follower(victim, ch);
      act
      (
         "$N arrives to obey $n's commands.",
         ch,
         NULL,
         victim,
         TO_ROOM
      );
      victim->leader = ch;
      SET_BIT(victim->act2, ACT_RIDEABLE);
      /* Can mount the steed even when blind. */
      victim->see = ch;
      do_mount(ch, "covenantsteed");
      victim->see = NULL;
      affect_to_char(ch, &af);
      check_improve(ch, gsn_recruit, TRUE, 1);
      ch->mana -= skill_table[gsn_recruit].min_mana;
      WAIT_STATE(ch, skill_table[gsn_recruit].beats);
      return;
   }
   victim = get_char_room(ch, arg1);
   if (victim == NULL)
   {
      send_to_char("Attempt to recruit whom?\n\r", ch);
      return;
   }


   act
   (
      "$n makes an attempt to recruit others to $s side!'",
      ch,
      NULL,
      NULL,
      TO_ROOM
   );
   send_to_char("You make an attempt to recruit others to your side.\n\r", ch);

   if (!IS_NPC(victim))
   {
      act
      (
         "You try to recruit $N to your side, but $E ignores you.",
         ch,
         NULL,
         victim,
         TO_CHAR
      );
      return;
   }
   if
   (
      IS_NPC(victim) &&
      (
         victim->pIndexData->mprog_flags ||
         victim->spec_fun != NULL
      )
   )
   {
      act
      (
         "$N ignores you.",
         ch,
         NULL,
         victim,
         TO_CHAR
      );
      return;
   }
   if (IS_AFFECTED(victim, AFF_CHARM))
   {
      act
      (
         "$N is under another's command.",
         ch,
         NULL,
         victim,
         TO_CHAR
      );
      return;
   }
   if (victim->level > ch->level)
   {
      act
      (
         "$N is too powerful to be swayed by your words.",
         ch,
         NULL,
         victim,
         TO_CHAR
      );
      return;
   }
   if
   (
      IS_SET(victim->act, ACT_PRACTICE) ||
      IS_SET(victim->act, ACT_TRAIN)
   )
   {
      send_to_char("Guildmasters and trainers have other business.\n\r", ch);
      return;
   }
   if
   (
      IS_SET(victim->act, ACT_AGGRESSIVE) ||
      IS_SET(victim->off_flags, SPAM_MURDER)
   )
   {
      send_to_char("They are far too violent to command.\n\r", ch);
      return;
   }
   if (check_immune(victim, DAM_CHARM) == IS_IMMUNE)
   {
      act
      (
         "$n shouts out orders, but is ignored.",
         ch,
         NULL,
         NULL,
         TO_ROOM
      );
      act
      (
         "You try to recruit $N, but $E completely ignores you.",
         ch,
         NULL,
         victim,
         TO_CHAR
      );
      check_improve(ch, gsn_recruit, FALSE, 2);
      ch->mana -= skill_table[gsn_recruit].min_mana / 2;
      WAIT_STATE(ch, skill_table[gsn_recruit].beats / 2);
      return;
   }
   if (IS_EVIL(victim))
   {
      if (IS_EVIL(ch))
      {
         chance *= 9;
         chance /= 10;
      }
      else
      {
         chance *= 7;
         chance /= 10;
      }
   }
   else if (IS_NEUTRAL(victim))
   {
      if (IS_EVIL(ch))
      {
         chance *= 7;
         chance /= 10;
      }
      else
      {
         chance *= 9;
         chance /= 10;
      }
   }
   else
   {
      if (IS_EVIL(ch))
      {
         chance = 0;
      }
      else
      {
         chance = 50;
      }
   }
   if (ch->level == victim->level)  /* Max level */
   {
      chance -= 10;
   }
   else
   {
      chance += (GET_LEVEL(ch) - GET_LEVEL(victim)) * 5;
   }
   if (IS_EVIL(ch) && IS_GOOD(victim))
   {
      chance /= 2;
      if (chance > 15)
      {
         chance = 15;
      }
   }
   else
   {
      chance *= 3;
      chance /= 2;
   }
   chance = URANGE(1, chance, 70);

   if (number_percent() > chance)
   {
      act
      (
         "$n shouts out orders, but is ignored.",
         ch,
         NULL,
         NULL,
         TO_ROOM
      );
      act
      (
         "You try to recruit $N, but $E ignores you.",
         ch,
         NULL,
         victim,
         TO_CHAR
      );
      check_improve(ch, gsn_recruit, FALSE, 1);
      ch->mana -= skill_table[gsn_recruit].min_mana / 2;
      WAIT_STATE(ch, skill_table[gsn_recruit].beats / 2);
      af.duration /= 4;
      affect_to_char(ch, &af);
      return;
   }
   check_improve(ch, gsn_recruit, TRUE, 1);
   ch->mana -= skill_table[gsn_recruit].min_mana;
   WAIT_STATE(ch, skill_table[gsn_recruit].beats);
   affect_to_char(ch, &af);

   victim->leader = ch;
   victim->master = ch;
   af.bitvector   = AFF_CHARM;
   af.level       = GET_LEVEL(ch);
   af.duration    = GET_LEVEL(ch);
   affect_to_char(victim, &af);
   act
   (
      "You shout your orders, and $N obeys.",
      ch,
      NULL,
      victim,
      TO_CHAR
   );
   act
   (
      "$n shouts orders and $N obeys.",
      ch,
      NULL,
      victim,
      TO_NOTVICT
   );
   return;
}

void do_book_contents(CHAR_DATA* ch, char* argument)
{
   CHAR_DATA* book = find_book(ch, argument);
   OBJ_DATA* book_obj = find_book_item(ch, argument);
   char buf[MAX_STRING_LENGTH];
   int cnt;
   int titles;
   BUFFER* output;
   static char* length[] =
   {
      "very short ",
      "short ",
      "medium-length ",
      "",
      "full sized "
   };
   char* pag_chap;

   if
   (
      book_obj == NULL &&
      book == NULL
   )
   {
      send_to_char("Read the contents of what?\n\r", ch);
      return;
   }
   if (book_obj == NULL)
   {
      sprintf
      (
         buf,
         "$N has %d untitled chapters.\n\r",
         MAX_DESCRIPTIONS - 2
      );
      act
      (
         buf,
         ch,
         NULL,
         book,
         TO_CHAR
      );
      return;
   }
   /* book_obj != NULL, done with race_book */
   if (book_obj->value[3] == BOOK_LOCKED)
   {
      act("$p is locked.", ch, book_obj, NULL, TO_CHAR);
      return;
   }
   if (book_obj->value[3] == BOOK_CLOSED)
   {
      act("$p is closed.", ch, book_obj, NULL, TO_CHAR);
      return;
   }
   titles = 0;
   for (cnt = 0; cnt < book_obj->value[0]; cnt++)
   {
      if (book_obj->book_info->title[cnt][0] != '\0')
      {
         titles++;
      }
   }
   if (IS_SET(book_obj->value[4], BOOK_PAGES))
   {
      pag_chap = "page";
   }
   else
   {
      pag_chap = "chapter";
   }
   sprintf
   (
      buf,
      "%s has %d titled and %d untitled %s%s%s.\n\r",
      book_obj->short_descr,
      titles,
      book_obj->value[0] - titles,
      length[book_obj->value[1]],
      pag_chap,
      (
         book_obj->value[0] != 1 ?
         "s" :
         ""
      )
   );
   output = new_buf();
   add_buf(output, buf);
   for (cnt = 0; cnt < book_obj->value[0]; cnt++)
   {
      if (book_obj->book_info->title[cnt][0] != '\0')
      {
         sprintf
         (
            buf,
            "%-2d : %s\n\r",
            cnt + 1,
            book_obj->book_info->title[cnt]
         );
         add_buf(output, buf);
      }
   }
   page_to_char(buf_string(output), ch);
   free_buf(output);
}

void do_pen_fill(CHAR_DATA* ch, char* argument)
{
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   OBJ_DATA* pen;
   OBJ_DATA* inkwell;
   bool dip = FALSE;
   int fill;

   argument = one_argument(argument, arg1);
   argument = one_argument(argument, arg2);
   pen = get_obj_list(ch, arg1, ch->carrying);
   inkwell = get_obj_list(ch, arg2, ch->carrying);
   if (inkwell == NULL)
   {
      inkwell = get_obj_list(ch, arg2, ch->in_room->contents);
   }
   if
   (
      pen == NULL ||
      pen->item_type != ITEM_PEN
   )
   {
      send_to_char("Fill what writing instrument?\n\r", ch);
      return;
   }
   if (!IS_SET(pen->value[4], PEN_FILLABLE))
   {
      act
      (
         "Once the $t from $P runs out, it is useless.",
         ch,
         inkwell_table[pen->value[2]],
         pen,
         TO_CHAR
      );
      return;
   }
   if
   (
      pen->value[1] == -1 ||
      pen->value[1] >= pen->value[3]
   )
   {
      act("$p is full of $T.", ch, pen, inkwell_table[pen->value[2]], TO_CHAR);
      return;
   }
   if (IS_SET(pen->value[4], PEN_FILL_DIP))
   {
      dip = TRUE;
   }
   if
   (
      inkwell == NULL ||
      inkwell->item_type != ITEM_INKWELL
   )
   {
      if (dip)
      {
         act("Dip $p into what?", ch, pen, NULL, TO_CHAR);
      }
      else
      {
         act("Fill $p from what?", ch, pen, NULL, TO_CHAR);
      }
      return;
   }
   if (inkwell->value[2] != pen->value[2])
   {
      if (dip)
      {
         sprintf
         (
            buf,
            "You must dip $p into %s, not %s.",
            inkwell_table[pen->value[2]],
            inkwell_table[inkwell->value[2]]
         );
         act(buf, ch, pen, NULL, TO_CHAR);
      }
      else
      {
         sprintf
         (
            buf,
            "You must fill $p with %s, not %s.",
            inkwell_table[pen->value[2]],
            inkwell_table[inkwell->value[2]]
         );
         act(buf, ch, pen, NULL, TO_CHAR);
      }
      return;
   }
   if
   (
      pen->value[1] != 0 &&
      pen->value[0] != inkwell->value[0]
   )
   {
      act
      (
         "$p has the wrong color $T.",
         ch,
         inkwell,
         inkwell_table[inkwell->value[2]],
         TO_CHAR
      );
      return;
   }
   fill = pen->value[3] - pen->value[1];
   if (fill > inkwell->value[1])
   {
      fill = inkwell->value[1];
   }
   pen->value[1] += fill;
   inkwell->value[1] -= fill;
   pen->value[0] = inkwell->value[0];
   if (dip)
   {
      act("You dip $p into $P.", ch, pen, inkwell, TO_CHAR);
      act("$n dips $p into $P.", ch, pen, inkwell, TO_ROOM);
   }
   else
   {
      act("You fill $p from $P.", ch, pen, inkwell, TO_CHAR);
      act("$n fills $p from $P.", ch, pen, inkwell, TO_ROOM);
   }
}

void do_muster(CHAR_DATA* ch, char* argument)
{
   CHAR_DATA* guard;
   AFFECT_DATA af;
   CHAR_DATA* check;
   int spec;
   int count;
   bool sentry;
   bool sentinel;
   bool mage;
   int chance;

   if
   (
      IS_NPC(ch) ||
      !has_skill(ch, gsn_muster) ||
      (
         chance = get_skill(ch, gsn_muster)
      ) < 1
   )
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }
   if (house_down(ch, HOUSE_ENFORCER))
   {
      return;
   }
   if (check_peace(ch))
   {
      return;
   }
   if (is_affected(ch, gsn_muster))
   {
      send_to_char("You cannot call more guards yet.\n\r", ch);
      return;
   }
   if (argument[0] == '\0')
   {
      send_to_char("Muster what kind of guard to your side?\n\r", ch);
      return;
   }
   count = 0;
   sentry = FALSE;
   sentinel = FALSE;
   mage = FALSE;
   for (check = char_list; check != NULL; check = check->next)
   {
      if
      (
         IS_NPC(check) &&
         IS_AFFECTED(check, AFF_CHARM) &&
         (
            check->mprog_target == ch ||
            check->master == ch
         ) &&
         check->pIndexData->vnum == MOB_VNUM_ENFORCER_GUARD
      )
      {
         switch (guard_type(check))
         {
            default:
            {
               bug("do_muster: Null Guard Type %d", guard_type(check));
               continue;
            }
            case (GUARD_NORMAL):
            {
               count += 2;
               break;
            }
            case (GUARD_MAGUS):
            case (GUARD_CURATE):
            {
               mage = TRUE;
               count += 3;
               break;
            }
            case (GUARD_DEFENDER):
            {
               count += 3;
               break;
            }
            case (GUARD_SENTINEL):
            {
               sentinel = TRUE;
               count += 1;
               break;
            }
            case (GUARD_SENTRY):
            {
               sentry = TRUE;
               count += 1;
               break;
            }
         }
      }
   }
   if (count >= 7)
   {
      send_to_char
      (
         "You have a full guard contingency assigned to you.\n\r",
         ch
      );
      return;
   }
   spec = -1;
   if (!str_prefix(argument, "guard"))
   {
      spec = GUARD_NORMAL;
      count += 2;
   }
   else if (!str_prefix(argument, "defender"))
   {
      spec = GUARD_DEFENDER;
      count += 3;
   }
   else if (!str_prefix(argument, "curate"))
   {
      spec = GUARD_CURATE;
      count += 3;
      if (mage)
      {
         send_to_char
         (
            "You may only have one mage or curate assigned to you.\n\r",
            ch
         );
         return;
      }
   }
   else if (!str_prefix(argument, "magus"))
   {
      spec = GUARD_MAGUS;
      count += 3;
      if (mage)
      {
         send_to_char
         (
            "You may only have one mage or curate assigned to you.\n\r",
            ch
         );
         return;
      }
   }
   else if (!str_prefix(argument, "sentry"))
   {
      spec = GUARD_SENTRY;
      count += 1;
      if (sentry)
      {
         send_to_char
         (
            "You may only have one sentry assigned to you.\n\r",
            ch
         );
         return;
      }
   }
   else if (!str_prefix(argument, "sentinel"))
   {
      spec = GUARD_SENTINEL;
      count += 1;
      if (sentinel)
      {
         send_to_char
         (
            "You may only have one sentinel assigned to you.\n\r",
            ch
         );
         return;
      }
   }
   if (spec == -1)
   {
      send_to_char("Muster what kind of guard to your side?\n\r", ch);
      return;
   }
   if (count > 7)
   {
      send_to_char
      (
         "You cannot command that type of guard in addition"
         " to your guards.\n\r",
         ch
      );
      return;
   }
   WAIT_STATE(ch, skill_table[gsn_muster].beats);
   if (number_percent() > chance)
   {
      send_to_char("No one responded to your call.\n\r", ch);
      check_improve(ch, gsn_muster, FALSE, 1);
      return;
   }
   check_improve(ch, gsn_muster, TRUE, 1);

   af.where = TO_AFFECTS;
   af.level = ch->level;
   af.location = 0;
   af.modifier = 0;
   af.duration = 2;
   af.bitvector = 0;
   af.type = gsn_muster;
   affect_to_char(ch, &af);
   guard = create_mobile(get_mob_index(MOB_VNUM_ENFORCER_GUARD));
   guard->level = ch->level;
   guard->damroll += ch->level / 2;
   guard->max_hit = ch->level * 16 + number_range(-150, 150);
   guard->max_move = ch->max_move;
   guard->alignment = ch->alignment;
   char_to_room(guard, ch->in_room);
   REMOVE_BIT(guard->off_flags, OFF_TRIP);
   /* Guards start in autoattack mode */
   SET_BIT(guard->act2, ACT2_AUTOATTACK);
   switch (spec)
   {
      default:
      {
         return;
      }
      case (GUARD_NORMAL):
      {
         SET_BIT(guard->off_flags, OFF_TRIP);
         guard->name = str_dup("guard justicar normal");
         free_string(guard->short_descr);
         guard->short_descr = str_dup("a Justicar guard");
         free_string(guard->long_descr);
         guard->long_descr = str_dup("A Justicar guard is standing here.\n\r");
         free_string(guard->description);
         guard->description = str_dup
         (
            "A young man stands here, clad in the armor of the Justicars. The\n\r"
            "armor is simple but well cared for. In the center of the armor is\n\r"
            "the Seal of the Justicars. A simple scabbard hangs loosely at his\n\r"
            "side. His lean, defined muscles show that he has spent many hours\n\r"
            "training, but his young, delicate features show little battle\n\r"
            "hardened experience. Still, he stands here stalwart, ready to\n\r"
            "defend the populace and to bring criminals to swift justice.\n\r"
         );
         break;
      }
      case (GUARD_DEFENDER):
      {
         SET_BIT(guard->off_flags, OFF_CRUSH);
         guard->damroll *= 2;
         guard->max_hit = guard->max_hit * 3 / 2;
         free_string(guard->name);
         guard->name = str_dup("guard justicar defender");
         free_string(guard->short_descr);
         guard->short_descr = str_dup("a Justicar defender");
         free_string(guard->long_descr);
         guard->long_descr = str_dup("A Justicar defender is standing here.\n\r");
         free_string(guard->description);
         guard->description = str_dup
         (
            "Surveying the area with steely-eyed intensity, this battle hardened\n\r"
            "warrior exudes confidence. He wears finely made armor bearing the\n\r"
            "Seal of the Justicars. His bulging muscles show a body honed to\n\r"
            "perfection and ready for battle. Though he shows signs of some age\n\r"
            "there is no indication that he is anything but quick and nimble and,\n\r"
            "where age may have slowed him, experience will surely compensate.\n\r"
         );
         break;
      }
      case (GUARD_CURATE):
      {
         guard->damroll = guard->damroll * 2 / 3;
         af.type = gsn_sanctuary;
         af.bitvector = AFF_SANCTUARY;
         af.duration = -1;
         affect_to_char(guard, &af);
         SET_BIT(guard->act, ACT_CLERIC);
         guard->name = str_dup("guard justicar curate");
         free_string(guard->short_descr);
         guard->short_descr = str_dup("a Justicar curate");
         free_string(guard->long_descr);
         guard->long_descr = str_dup("A Justicar curate is standing here.\n\r");
         free_string(guard->description);
         guard->description = str_dup
         (
            "Clad in fine, clerical robes, this man radiates a holy light. His\n\r"
            "calm demeanor only slightly masks an intensity that lies behind his\n\r"
            "pale eyes. He wears a simple piece of cloth on the back part of the\n\r"
            "top of his head. His robes are sinched loosely about his waist with\n\r"
            "entwined strips of tanned leather. On the front of his robes he\n\r"
            "bears the Seal of the Justicars. He uses his clerical abilities to\n\r"
            "help protect the populace from any criminals that would theaten them.\n\r"
         );
         break;
      }
      case (GUARD_MAGUS):
      {
         guard->damroll /= 2;
         af.type = gsn_chromatic_shield;
         af.bitvector = AFF_SANCTUARY;
         af.duration = -1;
         affect_to_char(guard, &af);
         SET_BIT(guard->act, ACT_MAGE);
         guard->name = str_dup("guard justicar magus mage");
         free_string(guard->short_descr);
         guard->short_descr = str_dup("a Justicar magus");
         free_string(guard->long_descr);
         guard->long_descr = str_dup("A Justicar magus is standing here.\n\r");
         free_string(guard->description);
         guard->description = str_dup
         (
            "Though a member of the guild of channelers, this mage's training has\n\r"
            "emphasized combat. His muscles are taut and defined and his magics\n\r"
            "are tailored to both weaken and damage his foes. He wears fine robes\n\r"
            "and wields a gnarled wooden staff. The robes he wears bear the Seal\n\r"
            "of the Justicars. A fine, leather belt wraps around his waist. He\n\r"
            "stands ready to defend the citizens and to bring his power to bear\n\r"
            "should the need arise.\n\r"
         );
         break;
      }
      case (GUARD_SENTRY):
      {
         guard->spec_fun = NULL;
         SET_BIT(guard->off_flags, NO_TRACK);
         /* Sentry does not autoattack */
         REMOVE_BIT(guard->act2, ACT2_AUTOATTACK);
         guard->max_hit = guard->max_hit * 2 / 3;
         guard->name = str_dup("guard justicar sentry");
         free_string(guard->short_descr);
         guard->short_descr = str_dup("a Justicar sentry");
         free_string(guard->long_descr);
         guard->long_descr = str_dup("A Justicar sentry is standing here.\n\r");
         free_string(guard->description);
         guard->description = str_dup
         (
            "Standing a vigilant guard, this Justicar sentry stands ready to\n\r"
            "detain criminals that try to pass him by. He is a strong and able\n\r"
            "guard with taut cables for muscles. His armor is well kept, bearing\n\r"
            "the Seal of the Justicar. A young looking guard, his athletic\n\r"
            "appearance shows that what he lacks in battle experience, he surely\n\r"
            "makes up for in training.\n\r"
         );
         break;
      }
      case (GUARD_SENTINEL):
      {
         SET_BIT(guard->off_flags, NO_TRACK);
         SET_BIT(guard->off_flags, OFF_CRUSH);
         guard->damroll *= 2;
         guard->max_hit = guard->max_hit * 3 / 2;
         guard->name = str_dup("guard justicar sentinel");
         free_string(guard->short_descr);
         guard->short_descr = str_dup("a Justicar sentinel");
         free_string(guard->long_descr);
         guard->long_descr = str_dup("A Justicar sentinel is standing here.\n\r");
         free_string(guard->description);
         guard->description = str_dup
         (
            "Standing just ahead of you is a member of the Justicar guard. He\n\r"
            "appears to be standing guard over the very spot he stands upon. His\n\r"
            "eyes survey the area, his weapon at the ready. His armor is in fine\n\r"
            "condition as is his body. He appears to be a dedicated and\n\r"
            "disciplined warrior whose time is spent training and preparing. On\n\r"
            "the front of his armor, you can make out a symbol. It is the Seal of\n\r"
            "the Justicars.\n\r"
         );
         break;
      }
   }
   guard->hit = guard->max_hit;
   guard->move = guard->max_move;
   guard->mprog_target = ch;

   act
   (
      "$n shouts, 'Guards! Guards!'",
      ch,
      NULL,
      NULL,
      TO_ROOM
   );
   send_to_char("You shout out for guards!\n\r", ch);
   act
   (
      "$N arrives to aid you.",
      ch,
      NULL,
      guard,
      TO_CHAR
   );
   act
   (
      "$N arrives to answer $n's call.",
      ch,
      NULL,
      guard,
      TO_ROOM
   );
   SET_BIT(guard->affected_by, AFF_CHARM);
   if (!IS_SET(guard->off_flags, NO_TRACK))
   {
      add_follower(guard, ch);
      guard->leader = ch;
   }
   return;
}

void mob_entry_zeth_woods(CHAR_DATA* ch)
{
   int count;
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA* mob;
   MOB_INDEX_DATA* mob_index;
   OBJ_DATA* obj;
   OBJ_DATA* obj2;
   OBJ_INDEX_DATA* obj_index = NULL;
   OBJ_INDEX_DATA* obj_index2 = NULL;
   if
   (
      IS_NPC(ch) ||
      str_cmp(ch->in_room->area->name, "Forest of Zeth") ||
      number_percent() > 15 ||
      IS_IMMORTAL(ch)
   )
   {
      return;
   }

   count = number_range(0, 17);
   obj = NULL;
   obj2 = NULL;
   if (count <= 3)
   {
      mob_index = get_mob_index(14463);
   }
   else if (count <= 6)
   {
      mob_index = get_mob_index(14464);
   }
   else if (count <= 9)
   {
      mob_index = get_mob_index(14465);
   }
   else if (count <= 12)
   {
      mob_index = get_mob_index(14466);
   }
   else if (count <= 14)
   {
      mob_index = get_mob_index(14467);
   }
   else if (count <= 16)
   {
      mob_index = get_mob_index(14469);
   }
   else
   {
      mob_index = get_mob_index(14470);
      obj_index = get_obj_index(14462);
      obj_index2 = get_obj_index(14463);
   }


   if (mob_index == NULL)
   {
      return;
   }

   count = mob_index->count;

   if
   (
      (
         mob_index->vnum == 14463 &&
         count >= 3
      ) ||
      (
         mob_index->vnum == 14464 &&
         count >= 2
      ) ||
      (
         mob_index->vnum == 14465 &&
         count >= 3
      ) ||
      (
         mob_index->vnum == 14466 &&
         count >= 3
      ) ||
      (
         mob_index->vnum == 14467 &&
         count >= 2
      ) ||
      (
         mob_index->vnum == 14469 &&
         count >= 1
      ) ||
      (
         mob_index->vnum == 14470 &&
         count >= 1
      )
   )
   {
      return;
   }
   mob = create_mobile(mob_index);
   if (mob == NULL)
   {
      return;
   }

   char_to_room(mob, ch->in_room);
   if
   (
      obj_index != NULL &&
      (
         obj = create_object(obj_index, 20)
      ) != NULL
   )
   {
      obj_to_char(obj, mob);
   }
   if
   (
      obj_index2 != NULL &&
      (
         obj2 = create_object(obj_index2, 20)
      ) != NULL
   )
   {
      obj_to_char(obj2, mob);
   }
   switch (mob_index->vnum)
   {
      default:
      {
         act
         (
            "$n jumps out at you!",
            mob,
            NULL,
            ch,
            TO_VICT
         );
         act
         (
            "$n jumps out at $N!",
            mob,
            NULL,
            ch,
            TO_NOTVICT
         );
         break;
      }
      case (14463):
      {
         act
         (
            "$n leaps out from the bushes!",
            mob,
            NULL,
            NULL,
            TO_ROOM
         );
         break;
      }
      case (14464):
      {
         act
         (
            "$n swoops down at you from the leafy canopy!",
            mob,
            NULL,
            ch,
            TO_VICT
         );
         act
         (
            "$n swoops down at $N from the leafy canopy!",
            mob,
            NULL,
            ch,
            TO_NOTVICT
         );
         break;
      }
      case (14465):
      {
         act
         (
            "$n scuttles out from some decaying vegetable matter!",
            mob,
            NULL,
            NULL,
            TO_ROOM
         );
         break;
      }
      case (14466):
      {
         act
         (
            "$n hoots at you from a branch and swoops down at you!",
            mob,
            NULL,
            ch,
            TO_VICT
         );
         act
         (
            "$n hoots at $N from a branch and swoops down at $N!",
            mob,
            NULL,
            ch,
            TO_NOTVICT
         );
         break;
      }
      case (14467):
      {
         act
         (
            "$n roars as it crashes through some small trees!",
            mob,
            NULL,
            NULL,
            TO_ROOM
         );
         break;
      }
      case (14469):
      {
         act
         (
            "$n howls and leaps out from some bushes!",
            mob,
            NULL,
            NULL,
            TO_ROOM
         );
         break;
      }
      case (14470):
      {
         act
         (
            "$n charges in from the surrounding vegetation, screaming for blood!",
            mob,
            NULL,
            NULL,
            TO_ROOM
         );
         if (obj != NULL)
         {
            do_wear(mob, obj->name);
         }
         break;
      }
   }

   sprintf(buf, "Help! I'm being attacked by %s!", PERS(mob, ch));
   do_yell(ch, buf);

   multi_hit(mob, ch, TYPE_UNDEFINED);
   return;
}

void do_stalking(CHAR_DATA* ch, char* argument )
{
   int sn_fog;
   int sn_fire;
   AFFECT_DATA af;

   if
   (
      (get_skill(ch, gsn_stalking)) <= 0 ||
      (
         !IS_NPC(ch) &&
         /*ch->level < skill_table[gsn_hide].skill_level[ch->class]*/
         !has_skill(ch, gsn_stalking)
      )
   )
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }

   if (house_down(ch, HOUSE_COVENANT))
   {
      return;
   }

   sn_fog = gsn_faerie_fog;
   sn_fire = gsn_faerie_fire;
   if
   (
      is_affected(ch, sn_fog) ||
      is_affected(ch, sn_fire) ||
      IS_AFFECTED(ch, AFF_FAERIE_FIRE)
   )
   {
      send_to_char("You cannot even stalk a half blind old man while you glow.\n\r", ch);
      return;
   }


   send_to_char("You begin stalking.\n\r", ch);

   if (is_affected(ch, gsn_lurk))
   {
      return;
   }

   if (number_percent( ) < get_skill(ch, gsn_stalking))
   {
      af.where    = TO_AFFECTS;
      af.type     = gsn_stalking;
      af.level    = ch->level;
      af.location = 0;
      af.modifier = 0;
      af.bitvector = 0;
      af.duration = (ch->level)/10;
      affect_to_char(ch, &af);

      check_improve
      (
         ch,
         gsn_stalking,
         TRUE,
         3
      );
   }
   else
   {
      check_improve
      (
         ch,
         gsn_stalking,
         FALSE,
         3
      );
   }

   return;
}

void nap_start(CHAR_DATA* ch)
{
   OBJ_DATA* brand;
   int skill;
   AFFECT_DATA* paf;

   skill = get_skill(ch, gsn_nap);
   if
   (
      (
         brand = get_eq_char(ch, WEAR_BRAND)
      ) != NULL &&
      brand->pIndexData->vnum != OBJ_VNUM_TWIG_BRAND
   )
   {
      brand = NULL;
   }
   /* brand != NULL iff brand vnum is twig brand */
   affect_strip(ch, gsn_hyper);
   affect_strip(ch, gsn_nap);
   affect_strip(ch, gsn_nap_slow);
   /* If just a book, they can re-attack him */
   stop_fighting(ch, TRUE);
   paf = new_affect();
   paf->type = gsn_nap;
   paf->where = TO_AFFECTS;
   paf->bitvector2 = AFF_NAP;
   paf->duration = -1;
   paf->level = number_range(3, 5);  /* Number of rounds */
   skill = skill * 3 / 4;
   if (skill < 5)
   {
      skill = 5;
   }
   while (number_percent() > skill)
   {
      paf->level++;
   }
   affect_to_char_version(ch, paf, AFFECT_CURRENT_VERSION);
   paf->type = gsn_nap_slow;
   paf->bitvector2 = 0;
   if
   (
      ch->race == grn_book &&
      !IS_AFFECTED(ch, AFF_SLOW) &&
      !IS_AFFECTED(ch, AFF_HASTE)
   )
   {
      /* Books get slow from nap */
      paf->bitvector = AFF_SLOW;
   }
   paf->level = 0;
   paf->duration = 36;
   affect_to_char_version(ch, paf, AFFECT_CURRENT_VERSION);
   free_affect(paf);
   act
   (
      "$n's eyes flutter for a moment before $e curls up into a ball.\n\r"
      "$n takes a nap.",
      ch,
      NULL,
      NULL,
      TO_ROOM
   );
   send_to_char
   (
      "Your eyes flutter a moment before you curl up into a ball.\n\r"
      "You take a nap.\n\r",
      ch
   );
   check_improve(ch, gsn_nap, TRUE, 1);
   ch->position = POS_SLEEPING;
   ch->on = NULL;
   update_pos(ch);
}

void do_nap(CHAR_DATA* ch, char* arg)
{
   OBJ_DATA* brand;
   OBJ_DATA* obj;
   int skill;
   char* liq_name;
   int thirst;
   AFFECT_DATA* paf;

   if
   (
      (
         skill = get_skill(ch, gsn_nap)
      ) < 1 ||
      (
         (
            (
               brand = get_eq_char(ch, WEAR_BRAND)
            ) == NULL ||
            brand->pIndexData->vnum != OBJ_VNUM_TWIG_BRAND
         ) &&
         ch->race != grn_book
      )
   )
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }
   if (is_affected(ch, gsn_nap_timer))
   {
      send_to_char("You feel too restless to take a nap.\n\r", ch);
      return;
   }
   if (!brand)
   {
      brand = get_eq_char(ch, WEAR_BRAND);
   }
   if
   (
      brand &&
      brand->pIndexData->vnum != OBJ_VNUM_TWIG_BRAND
   )
   {
      brand = NULL;
   }
   /* brand != NULL iff brand vnum is twig brand */
   if (arg[0] == '\0')
   {
      for (obj = ch->in_room->contents; obj; obj = obj->next_content)
      {
         if
         (
            obj->item_type == ITEM_FOUNTAIN &&
            liq_table[obj->value[2]].is_milk
         )
         {
            break;
         }
      }
      if (obj == NULL)
      {
         send_to_char("Drink what milk before going to sleep?\n\r", ch);
         return;
      }
   }
   else if
   (
      (
         obj = get_obj_here(ch, arg)
      ) == NULL
   )
   {
      send_to_char("You cannot find it.\n\r", ch);
      return;
   }
   switch (obj->item_type)
   {
      default:
      {
         send_to_char("You cannot drink from that.\n\r", ch);
         return;
      }
      case (ITEM_FOUNTAIN):
      {
         if (!liq_table[obj->value[2]].is_milk)
         {
            send_to_char("That is not milk in there!\n\r", ch);
            return;
         }
         if
         (
            obj->pIndexData->vnum == OBJ_VNUM_SPRING &&
            IS_SET(obj->extra_flags, ITEM_MAGIC)
         )
         {
            check_impure(ch, obj->short_descr, IMPURE_DRINK);
         }
         break;
      }
      case (ITEM_DRINK_CON):
      {
         if (obj->value[0] <= 0)
         {
            send_to_char("It is already empty.\n\r", ch);
         }
         if (!liq_table[obj->value[2]].is_milk)
         {
            send_to_char("That is not milk in there!\n\r", ch);
            return;
         }
         break;
      }
   }
   /* Reduce amount */
   if (obj->value[0] > 0)
   {
      obj->value[0] -= liq_table[obj->value[2]].liq_affect[4];
   }
   liq_name = liq_table[obj->value[2]].liq_name;
   if (ch->race != grn_arborian)
   {
      act("$n drinks $T from $p.", ch, obj, liq_name, TO_ROOM);
      act("You drink $T from $p.", ch, obj, liq_name, TO_CHAR);
   }
   else
   {
      if (obj->carried_by != ch)
      {
         act
         (
            "$n dips $s roots into $p and absorbs $T from it.",
            ch,
            obj,
            liq_name,
            TO_ROOM
         );
         act
         (
            "You dip your roots into $p and absorb $T from it.",
            ch,
            obj,
            liq_name,
            TO_CHAR
         );
      }
      else
      {
         act
         (
            "$n pours $T from $p over $s roots and looks refreshed.",
            ch,
            obj,
            liq_name,
            TO_ROOM
         );
         act
         (
            "You pour $T from $p over your roots and feel refreshed.",
            ch,
            obj,
            liq_name,
            TO_CHAR
         );
      }
   }

   if (!is_branded_by_lestregus(ch))
   {
      gain_condition(ch, COND_THIRST, 10);
   }

   if (IS_NPC(ch))
   {
      thirst = ch->level / 2;
   }
   else
   {
      thirst = ch->pcdata->condition[COND_THIRST];
   }
   if (number_percent() / 2 > thirst)
   {
      send_to_char("You are still too thirsty to take a nap.\n\r", ch);
      return;
   }
   if
   (
      IS_AFFECTED(ch, AFF_HASTE) &&
      !IS_AFFECTED2(ch, AFF_HYPER)
   )
   {
      send_to_char("You are moving about too fast to take a nap.\n\r", ch);
      return;
   }
   if
   (
      IS_AFFECTED(ch, AFF_SLOW) &&
      !is_affected(ch, gsn_nap_slow)
   )
   {
      send_to_char("You are moving about too slow to take a nap.\n\r", ch);
      return;
   }
   check_improve(ch, gsn_nap, TRUE, 1);
   if (is_affected(ch, gsn_hyper))
   {
      if (number_percent() > skill * 3 / 4)
      {
         send_to_char("You feel too hyper to take a nap.\n\r", ch);
         WAIT_STATE(ch, PULSE_VIOLENCE);
         check_improve(ch, gsn_nap, FALSE, 1);
         return;
      }
   }
   paf = new_affect();
   paf->type = gsn_nap_timer;
   paf->where = TO_AFFECTS;
   paf->duration = 12;
   affect_to_char_version(ch, paf, AFFECT_CURRENT_VERSION);
   free_affect(paf);
   nap_start(ch);
}

void do_hyper(CHAR_DATA* ch, char* arg)
{
   OBJ_DATA* brand;
   OBJ_DATA* obj;
   int skill;
   int hunger;
   AFFECT_DATA* paf;

   if
   (
      (
         skill = get_skill(ch, gsn_hyper)
      ) < 1 ||
      (
         (
            (
               brand = get_eq_char(ch, WEAR_BRAND)
            ) == NULL ||
            brand->pIndexData->vnum != OBJ_VNUM_TWIG_BRAND
         ) &&
         ch->race != grn_book
      )
   )
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }
   if (is_affected(ch, gsn_hyper))
   {
      send_to_char("Wee!\n\r", ch);
      return;
   }
   if (!brand)
   {
      brand = get_eq_char(ch, WEAR_BRAND);
   }
   if
   (
      brand &&
      brand->pIndexData->vnum != OBJ_VNUM_TWIG_BRAND
   )
   {
      brand = NULL;
   }
   /* brand != NULL iff brand vnum is twig brand */
   if (arg[0] == '\0')
   {
      obj = NULL;
      if (ch->race != grn_arborian)
      {
         /* Arborians can go hyper without food */
         send_to_char
         (
            "You need energy to become hyper!\n\r"
            "What do you want to eat?\n\r",
            ch
         );
         return;
      }
   }
   else if
   (
      (
         obj = get_obj_here(ch, arg)
      ) == NULL
   )
   {
      send_to_char("You cannot find it.\n\r", ch);
      return;
   }
   if (ch->race == grn_arborian)
   {
      obj = NULL;
      if (room_is_dark(ch->in_room))
      {
         send_to_char
         (
            "You need energy to become hyper!\n\r"
            "You need sunlight.\n\r",
            ch
         );
         return;
      }
   }
   if (obj)
   {
      switch (obj->item_type)
      {
         default:
         {
            send_to_char("You cannot eat that!\n\r", ch);
            return;
         }
         case (ITEM_PILL):
         {
            send_to_char("That does not look tasty.\n\r", ch);
            return;
         }
         case (ITEM_FOOD):
         {
            if
            (
               obj->value[3] != 0 &&
               check_immune(ch, DAM_POISON) != IS_IMMUNE
            )
            {
               /* Prevent poison */
               send_to_char("That does not look tasty.\n\r", ch);
               return;
            }
            if (is_pulsing(obj))
            {
               check_impure(ch, obj->short_descr, IMPURE_EAT);
            }
            break;
         }
         case (ITEM_DRINK_CON):
         {
            if (obj->value[0] <= 0)
            {
               send_to_char("It is already empty.\n\r", ch);
            }
            if (!liq_table[obj->value[2]].is_milk)
            {
               send_to_char("That is not milk in there!\n\r", ch);
               return;
            }
            break;
         }
      }
   }
   if
   (
      IS_AFFECTED(ch, AFF_HASTE) &&
      !IS_AFFECTED2(ch, AFF_HYPER)
   )
   {
      send_to_char("You are moving about too fast to bounce around.\n\r", ch);
      return;
   }
   if
   (
      IS_AFFECTED(ch, AFF_SLOW) &&
      !is_affected(ch, gsn_nap_slow)
   )
   {
      send_to_char("You are moving about too slow to bounce around.\n\r", ch);
      return;
   }
   if (obj)
   {
      gain_condition( ch, COND_HUNGER, obj->value[1] * 2);
      act("$n gobbles down $p.",  ch, obj, NULL, TO_ROOM);
      act("You gobble down $p.", ch, obj, NULL, TO_CHAR);
      extract_obj(obj, FALSE);
   }
   /* Reduce amount */
   if (IS_NPC(ch))
   {
      hunger = ch->level / 2;
   }
   else
   {
      hunger = ch->pcdata->condition[COND_HUNGER];
   }
   if
   (
      ch->race != grn_arborian &&
      number_percent() / 2 > hunger
   )
   {
      send_to_char("You are still too hungry to go bouncing around.\n\r", ch);
      WAIT_STATE(ch, PULSE_VIOLENCE);
      check_improve(ch, gsn_hyper, FALSE, 1);
      return;
   }
   check_improve(ch, gsn_hyper, TRUE, 1);
   affect_strip(ch, gsn_nap);
   affect_strip(ch, gsn_nap_timer);
   affect_strip(ch, gsn_nap_slow);
   affect_strip(ch, gsn_hyper);
   paf = new_affect();
   paf->type = gsn_hyper;
   paf->where = TO_AFFECTS;
   paf->duration = -1;
   paf->level = number_range(1, 3);  /* Number of rounds */
   skill = skill * 5 / 6;
   paf->bitvector2 = AFF_HYPER;
   if
   (
      !IS_AFFECTED(ch, AFF_HASTE) &&
      !IS_AFFECTED(ch, AFF_SLOW) &&
      ch->race == grn_book
   )
   {
      paf->bitvector = AFF_HASTE;
   }
   if (skill < 10)
   {
      skill = 10;
   }
   while (number_percent() <= skill)
   {
      paf->level++;
   }
   paf->level *= number_range(5, 10);
   affect_to_char_version(ch, paf, AFFECT_CURRENT_VERSION);
   free_affect(paf);
   act
   (
      "$n starts bouncing around and giggling like a child on a sugar rush.",
      ch,
      NULL,
      NULL,
      TO_ROOM
   );
   send_to_char
   (
      "You feel rubbery as you begin bouncing off the walls.\n\r",
      ch
   );
}

