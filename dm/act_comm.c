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
static const char rcsid[] = "$Id: act_comm.c,v 1.184 2004/11/04 04:34:12 maelstrom Exp $";

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "merc.h"
#include "recycle.h"
#include "tables.h"
#include "subclass.h"
#include "worship.h"
#include "music.h"

/* command procedures needed */
DECLARE_DO_FUN(do_save);
DECLARE_DO_FUN(do_permnameban);
DECLARE_DO_FUN(do_dismount);
DECLARE_DO_FUN(do_quit);
DECLARE_DO_FUN(do_look);
DECLARE_DO_FUN(do_yell);
DECLARE_DO_FUN(do_report);
DECLARE_DO_FUN(do_murder);
DECLARE_DO_FUN(do_myell);
DECLARE_DO_FUN(do_gtell);
void    site_check_from  args( (CHAR_DATA* ch, OBJ_DATA* obj) );

/* spell functions needed */
DECLARE_SPELL_FUN(spell_identify);

char* get_ancient_title        args( (CHAR_DATA* ch) );
void  obj_say_lordly           args( (CHAR_DATA* ch, OBJ_DATA* obj, char* argument) );
void  obj_say_heavenly_sceptre args( (CHAR_DATA* ch, OBJ_DATA* obj, char* argument) );
void  mob_say_lady             args( (CHAR_DATA* ch, char* argument) );
void  mob_say_standing         args( (CHAR_DATA* ch, char* argument) );
void  obj_say_traitor          args( (CHAR_DATA* ch, char* argument) );
void  mob_say_guardian_angel   args( (CHAR_DATA* ch, CHAR_DATA*  mob, char* argument) );
void  mob_say_house_statue     args( (CHAR_DATA* ch, char* argument) );

void  obj_say_430       args( (CHAR_DATA* ch, char* argument, OBJ_DATA* obj)) ;
void  obj_say_23740     args( (CHAR_DATA* ch, char* argument, OBJ_DATA* obj)) ;
void  obj_say_2109      args( (CHAR_DATA* ch, char* argument, OBJ_DATA* obj));
void  obj_say_1888      args( (CHAR_DATA* ch, char* argument, OBJ_DATA* obj));
void  obj_say_15888     args( (CHAR_DATA* ch, char* argument, OBJ_DATA* obj));
void  obj_say_15901     args( (CHAR_DATA* ch, char* argument, OBJ_DATA* obj));
void  obj_say_27513     args( (CHAR_DATA* ch, char* argument, OBJ_DATA* obj));
void  obj_say_12500     args( (CHAR_DATA* ch, char* argument, OBJ_DATA* obj));
void  obj_say_12500_2   args( (CHAR_DATA* ch, char* argument, OBJ_DATA* obj));
void  obj_say_21670     args( (CHAR_DATA* ch, char* argument, OBJ_DATA* obj));
void  obj_say_20025     args( (CHAR_DATA* ch, char* argument, OBJ_DATA* obj));
void  obj_say_17641     args( (CHAR_DATA* ch, char* argument));
void  obj_say_17641_2   args( (CHAR_DATA* ch, char* argument));
void  obj_say_temporal  args( (CHAR_DATA* ch, char* argument, OBJ_DATA* obj));
void  obj_say_26309     args( (CHAR_DATA* ch, char* argument));
void  obj_say_mob       args( (CHAR_DATA* ch, char* argument));
void  obj_say_oblivion  args( (CHAR_DATA* ch, char* argument));
void  obj_trigger_425   args( (CHAR_DATA* ch, char* argument));
void  obj_trigger_15300 args( (CHAR_DATA* ch, char* argument));
void  obj_trigger_6712  args( (CHAR_DATA* ch, char* argument, OBJ_DATA* obj));
void  say_prog_summon_demon     args( (CHAR_DATA* ch, char* argument) );
void  say_prog_raise_shadowlord args( (CHAR_DATA* ch, char* argument) );
void  say_prog_karguska         args( (CHAR_DATA* ch, char* argument) );
void  say_prog_cru_blacksmith   args( (CHAR_DATA* ch, char* argument) );
bool  is_ignored                args( (CHAR_DATA* ch, CHAR_DATA* victim) );
bool  is_pulsing                args( (OBJ_DATA* obj) );
void  brand_where               args( (CHAR_DATA* ch) );
void  obj_trigger_sham_brand    args( (CHAR_DATA* ch, char* argument) );
void  obj_say_sentient_sword    args( (CHAR_DATA* ch, char* argument, OBJ_DATA* obj) );
void  obj_trigger_7359          args( (CHAR_DATA* ch, char* argument) );

/*
   return TRUE if a character communicates
   by projecting thoughts.
   False if they use normal voice
*/
bool is_mental(CHAR_DATA* ch)
{
   if (!IS_NPC(ch) && IS_IMMORTAL(ch))
   {
      return IS_SET(ch->comm2, COMM_WIZ_TALK);
   }
   if
   (
      race_table[ch->race].mental ||
      ch->id == 945578206  /* Zaebos */
   )
   {
      return TRUE;
   }
   return FALSE;
}

/*
   wiz command
   allows immortals to choose
   if they talk using voice,
   or mentally projecting
*/
void do_talk(CHAR_DATA* ch, char* argument)
{
   if (!str_cmp(argument, "mental") || !str_cmp(argument, "project"))
   {
      send_to_char
      (
         "You will mentally project your thoughts from now on.\n\r",
         ch
      );
      SET_BIT(ch->comm2, COMM_WIZ_TALK);
      return;
   }
   if (!str_cmp(argument, "aural") || !str_cmp(argument, "voice"))
   {
      send_to_char("You will use your voice to speak from now on.\n\r", ch);
      REMOVE_BIT(ch->comm2, COMM_WIZ_TALK);
      return;
   }
   if (!str_cmp(argument, "status"))
   {
      if (IS_SET(ch->comm2, COMM_WIZ_TALK))
      {
         send_to_char("You use your thoughts for speech.\n\r", ch);
      }
      else
      {
         send_to_char("You use your voice for speech.\n\r", ch);
      }
      return;
   }
   send_to_char
   (
      "Syntax:\n\r"
      "talk mental\n\r"
      "talk project\n\r"
      "  Either will make you talk like an illithid.\n\r"
      "talk voice\n\rtalk aural\n\r"
      "Either will make you talk normally.\n\r"
      "talk status\n\r"
      "  This will tell you your current talk status.\n\r"
      "talk <nothing or anything else>\n\r"
      "  This will tell you syntax of the talk command.\n\r",
      ch
   );
}

/* storing a hero - Wervdon */
void do_storag(CHAR_DATA* ch, char* argument)
{
   send_to_char("You must type the full command to store yourself.\n\r", ch);
}

void do_storage(CHAR_DATA* ch, char* argument)
{
   /*
   All send_to_char calls that might be called in rage delete
   were changed to act_new with POS_STANDING required
   You need to be POS_STANDING to use storage through the normal interpreter
   if POS_STUNNED, that means
   */
   char strsave  [MAX_INPUT_LENGTH];
   char strsave2 [MAX_INPUT_LENGTH];
   long wiznet_temp = ch->wiznet;
   long wiznet2_temp = ch->wiznet2;

   /*
   NPC's can't store
   Switched immortals must return to their own body
   before storing
   */
   if (IS_NPC(ch))
   {
      return;
   }
   if (ch->level < 20)
   {
      send_to_char("You are nowhere near famous enough to be stored.\n\r", ch);
      return;
   }
   if ((ch->quittime > 0) || (ch->pause > 0))
   {
      send_to_char
      (
         "You have to wait to be able to quit to go into storage.\n\r",
         ch
      );
      return;
   }
   if
   (
      ch->in_room != NULL &&
      IS_SET(ch->in_room->extra_room_flags, ROOM_1212) &&
      !IS_IMMORTAL(ch)
   )
   {
      send_to_char
      (
         "You may not store while you are in the Realm of the Dead.\n\r",
         ch
      );
      return;
   }
   if (ch->pcdata->confirm_storage)
   {
      if (argument[0] != '\0')
      {
         send_to_char("Storage status removed.\n\r", ch);
         ch->pcdata->confirm_storage = FALSE;
         return;
      }
      else
      {
         ch->wiznet = 0;
         ch->wiznet2 = 0;
         if (IS_IMMORTAL(ch))
         {
            wiznet
            (
               "$N has gone into storage.",
               ch,
               NULL,
               WIZ_DELETES,
               0,
               UMAX(55, get_trust(ch))
            );
         }
         else
         {
            wiznet
            (
               "$N has gone into storage.",
               ch,
               NULL,
               WIZ_DELETES,
               0,
               0
            );
         }
         if (IS_IMMORTAL(ch))
         {
            (
               find_wizi_slot(ch->name, &ch->pcdata->worship_slot)
            )->wiz_info.stored = TRUE;
         }
         /*
         Limpurge anyone who is being stored
         */
         strip_limits(ch->carrying);
         stop_fighting(ch, TRUE);
         do_permnameban(ch, ch->name);
         ch->position = POS_STANDING;
         /*
         If we are storing from aggravated delete, we want to be standing when we
         restore the character, no confusing stuns for no reason.
         */
         ch->wiznet = wiznet_temp;
         ch->wiznet2 = wiznet2_temp;
         /*
         do_quit saves you, no need to do it twice-Fizzfaldt
         save_char_obj(ch);
         */
         ch->pcdata->being_restored = TRUE;
         if
         (
            ch->level >= 30 &&
            !IS_IMMORTAL(ch) &&
            (
               ch->in_room == NULL ||
               !IS_SET(ch->in_room->extra_room_flags, ROOM_1212)
            ) &&
            ch->pcdata->race_lottery > 0 &&
            ch->pcdata->race_lottery < MAX_PC_RACE &&
            !ch->house &&
            !IS_SET(ch->act2, PLR_IS_ANCIENT)
         )
         {
            remove_node_for
            (
               ch->name,
               lottery_race_players[ch->pcdata->race_lottery]
            );
         }
         /* Keep track, so quit knows how to update the roster */
         do_quit(ch, "");
         sprintf(strsave,  "%s%s.plr", PLAYER_DIR,  capitalize(ch->name));
         sprintf(strsave2, "%s%s.plr", STORAGE_DIR, capitalize(ch->name));
         rename(strsave, strsave2);
         return;
      }
   }
   if (argument[0] != '\0')
   {
      send_to_char("Just type storage. No argument.\n\r", ch);
      return;
   }
   send_to_char
   (
      "Type storage again to confirm this command.\n\r"
      "Use this command only if you will not be playing any of your characters"
      " for a\n\r"
      "long time.\n\r"
      "Type: 'restore' at the name prompt and follow the directions to reverse"
      " storage.\n\r"
      "Typing storage with an argument will undo storage status.\n\r",
      ch
   );
   ch->pcdata->confirm_storage = TRUE;
   wiznet
   (
      "$N is contemplating storage.",
      ch,
      NULL,
      WIZ_DELETES,
      0,
      get_trust(ch)
   );
}

/* RT code to delete yourself */

void do_delet(CHAR_DATA* ch, char* argument)
{
   send_to_char("You must type the full command to delete yourself.\n\r", ch);
}

void do_delete(CHAR_DATA* ch, char* argument)
{
   char strsave [MAX_INPUT_LENGTH];
   long wiznet_temp = ch->wiznet;
   long wiznet2_temp = ch->wiznet2;
   char buf     [MAX_INPUT_LENGTH];

   if (IS_NPC(ch))
   {
      return;
   }
   if (ch->pcdata->confirm_delete)
   {
      if (argument[0] != '\0')
      {
         send_to_char("Delete status removed.\n\r", ch);
         ch->pcdata->confirm_delete = FALSE;
         return;
      }
      else
      {
         sprintf(strsave, "%s%s.plr", PLAYER_DIR, capitalize(ch->name));
         ch->pause    = 0;
         ch->quittime = 0;
         stop_fighting(ch, TRUE);
         /*
         Many players delete out of anger after dying.
         This code will store characters level 20 or
         higher, if they delete within one hour
         of dying.
         Do test to see if has died within past hour, AND is level 20 or
         higher.  mortals never allowed to store in 1212
         Always make backups of immortals

         this line:
         ((int) (current_time - ch->pcdata->last_death)) < 3600 &&
         the 3600 means amount of seconds rage delete works on.
         3600 seconds in 1 hour
         */
         if
         (
            (
               ((int) (current_time - ch->pcdata->last_death)) < 3600 &&
               ch->level >= 20 &&
               !(
                  ch->in_room != NULL &&
                  IS_SET(ch->in_room->extra_room_flags, ROOM_1212)
               )
            ) ||
            IS_IMMORTAL(ch)
         )
         {
            /*
            The storage command can normally only be done when
            POS_STANDING.
            Using POS_STUNNED and act_new as a way to hide the
            storage process from the character.
            (An angry person seeing they're being stored,
            might come back and start cursing/killing people,
            yelling if you won't let me delete, I'll be a jerk
            till I get banned.
            false if they're in the ROTD, so no store
            Immortals store anyway
            You can't store in the ROTD normally, no slack given here
            set stunned
            */
            ch->position = POS_STUNNED;
            ch->wiznet = 0;
            ch->wiznet2 = 0;
            if (IS_IMMORTAL(ch))
            {
               sprintf
               (
                  buf,
                  "%s is an Immortal trying to delete.  Pfile sent to storage"
                  " instead.",
                  ch->name
               );
               wiznet
               (
                  buf,
                  ch,
                  NULL,
                  WIZ_DELETES,
                  0,
                  55
               );
            }
            else
            {
               sprintf
               (
                  buf,
                  "%s is recently dead and trying to delete."
                  "  Pfile sent to storage instead.",
                  ch->name
               );
               wiznet
               (
                  buf,
                  ch,
                  NULL,
                  WIZ_DELETES,
                  0,
                  0
               );
            }
            log_string(buf);
            ch->wiznet = wiznet_temp;
            ch->wiznet2 = wiznet2_temp;
            /* Set storage confirmed, (no running store twice) */
            ch->pcdata->confirm_storage = TRUE;
            ch->pcdata->confirm_delete = FALSE;
            /* When they restore themselves, no accidental deletions */
            if
            (
               ch->in_room &&
               ch->in_room->house &&
               ch->in_room->house != ch->house
            )
            {
               /* Make sure they are not in an enemy house. */
               char_from_room(ch);
               char_to_room(ch, NULL);  /* Move them to a temple */
            }
            affect_strip(ch, gsn_blackjack);
            affect_strip(ch, gsn_strangle);
            do_storage(ch, "");
            /*
            storage (before saving/quitting the character,
            restore to POS_STANDING
            */
            return;
         }
         /*
         Character hasn't died in the past hour, (non aggravated delete)
         or, has died in the past hour, but less than level 20.
         (not famous enough to store anyway)
         */
         wiznet
         (
            "$N turns $Mself into line noise.",
            ch,
            NULL,
            WIZ_DELETES,
            0,
            0
         );
         if
         (
            !(
               check_nameban(ch->name, BAN_ALL) &&
               check_nameban(ch->name, BAN_PERMANENT)
            ) &&
            (
               ch->level >= 25 ||
               ((ch->played + (int)(current_time - ch->logon)) / 3600) >= 100
            )
         )
         {
            do_permnameban(ch, ch->name);
         }
         if
         (
            ch->level >= 10 &&
            !IS_IMMORTAL(ch)
         )
         {
            lottery_race_count[ch->race]--;
         }
         if
         (
            ch->level >= 30 &&
            !IS_IMMORTAL(ch) &&
            (
               ch->in_room == NULL ||
               !IS_SET(ch->in_room->extra_room_flags, ROOM_1212)
            ) &&
            ch->pcdata->race_lottery > 0 &&
            ch->pcdata->race_lottery < MAX_PC_RACE &&
            !ch->house &&
            !IS_SET(ch->act2, PLR_IS_ANCIENT)
         )
         {
            remove_node_for
            (
               ch->name,
               lottery_race_players[ch->pcdata->race_lottery]
            );
         }
         /*
            Do not add back into rosters, remove all objects,
            saves time by not saving character again.
         */
         ch->level = 0;
         do_quit(ch, "");
         remove(strsave);
         return;
      }
   }

   if (argument[0] != '\0')
   {
      send_to_char("Just type delete. No argument.\n\r", ch);
      return;
   }

   send_to_char
   (
      "Type delete again to confirm this command.\n\r"
      "WARNING: this command is irreversible.\n\r"
      "Typing delete with an argument will undo delete status.\n\r",
      ch
   );
   ch->pcdata->confirm_delete = TRUE;
   wiznet
   (
      "$N is contemplating deletion.",
      ch,
      NULL,
      WIZ_DELETES,
      0,
      get_trust(ch)
   );
}

bool is_ignored(CHAR_DATA* ch, CHAR_DATA* victim)
{
   if
   (
      ch == NULL ||
      victim == NULL ||
      victim->pcdata == NULL ||
      IS_IMMORTAL(ch)
   )
   {
      return FALSE;
   }
   if (is_name(ch->name, victim->pcdata->ignore))
   {
      return TRUE;
   }
   return FALSE;
}

void do_see(CHAR_DATA* ch, char* argument)
{
   char arg[MAX_INPUT_LENGTH];
   char arg2[MAX_STRING_LENGTH];
   CHAR_DATA* victim;
   int count;
   int number;

   argument = one_argument(argument, arg);

   if (arg[0] == '\0')
   {
      if (ch->see == NULL)
      {
         send_to_char("Allow whom to see you?\n\r", ch);
         return;
      }
      act
      (
         "You are no longer seen by $N",
         ch,
         NULL,
         ch->see,
         TO_CHAR
      );
      ch->see = NULL;
      return;
   }

   /* This added for decoy tells */
   strcpy(arg2, arg);

   /*
   Can tell to PC's anywhere, but NPC's only in same room.
   -- Furey
   */

   number = number_argument(arg2, arg);
   count = 0;

   for (victim = char_list; victim != NULL; victim = victim->next)
   {
      if
      (
         (
            IS_NPC(victim) &&
            (
               victim->pIndexData->vnum == MOB_VNUM_DECOY ||
               victim->pIndexData->vnum == MOB_VNUM_SHADOW ||
               victim->pIndexData->vnum == MOB_VNUM_MIRROR ||
               victim->pIndexData->vnum == MOB_VNUM_PHANTOM
            )
         ) ||
         victim->in_room == NULL ||
         !is_name(arg, victim->name) ||
         !can_see(ch, victim)
      )
      {
         continue;
      }
      if (++count == number)
      {
         break;
      }
   }
   if
   (
      victim == NULL ||
      (
         IS_NPC(victim) &&
         victim->in_room != ch->in_room
      )
   )
   {
      send_to_char("They are not here.\n\r", ch);
      return;
   }
   if (ch->nosee && ch->nosee == victim)
   {
      act
      (
         "You cannot do that.  Make up your mind whether you want $N to 'see'"
         " you or 'nosee' you.\n\r",
         ch,
         NULL,
         victim,
         TO_CHAR
      );
      return;
   }
   ch->see = victim;
   act
   (
      "$N can see you now.",
      ch,
      NULL,
      victim,
      TO_CHAR
   );
   return;
}

void do_nosee(CHAR_DATA* ch, char* argument)
{
   char arg[MAX_INPUT_LENGTH];
   char arg2[MAX_STRING_LENGTH];
   CHAR_DATA* victim;
   int count;
   int number;

   argument = one_argument(argument, arg);

   if (arg[0] == '\0')
   {
      if (ch->nosee == NULL)
      {
         send_to_char("Become unseen to whom?\n\r", ch);
         return;
      }
      act
      (
         "You are no longer unseen by $N",
         ch,
         NULL,
         ch->nosee,
         TO_CHAR
      );
      ch->nosee = NULL;
      return;
   }

   /* This added for decoy tells */
   strcpy(arg2, arg);

   /*
   Can tell to PC's anywhere, but NPC's only in same room.
   -- Furey
   */

   number = number_argument(arg2, arg);
   count = 0;

   for (victim = char_list; victim != NULL; victim = victim->next)
   {
      if
      (
         (
            IS_NPC(victim) &&
            (
               victim->pIndexData->vnum == MOB_VNUM_DECOY ||
               victim->pIndexData->vnum == MOB_VNUM_SHADOW ||
               victim->pIndexData->vnum == MOB_VNUM_MIRROR ||
               victim->pIndexData->vnum == MOB_VNUM_PHANTOM
            )
         ) ||
         victim->in_room == NULL ||
         !is_name(arg, victim->name) ||
         !can_see(ch, victim)
      )
      {
         continue;
      }
      if (++count == number)
      {
         break;
      }
   }
   if
   (
      victim == NULL ||
      (
         IS_NPC(victim) &&
         victim->in_room != ch->in_room
      )
   )
   {
      send_to_char("They are not here.\n\r", ch);
      return;
   }
   if (ch->see && ch->see == victim)
   {
      act
      (
         "You cannot do that.  Make up your mind whether you want $N to 'see'"
         " you or 'nosee' you.\n\r",
         ch,
         NULL,
         victim,
         TO_CHAR
      );
      return;
   }
   if (get_trust(ch) <= get_trust(victim))
   {
      send_to_char("You cannot nosee them.\n\r", ch);
      return;
   }
   ch->nosee = victim;
   act
   (
      "$N cannot see you now.",
      ch,
      NULL,
      victim,
      TO_CHAR
   );
   return;
}

/* RT code to display channel status */
void do_channels(CHAR_DATA* ch, char* argument)
{
   char buf[MAX_STRING_LENGTH];

   /* lists all channels and their status */
   strcpy
   (
      buf,
      "   channel     status\n\r"
      "---------------------\n\r"
      "auction        "
   );
   if (!IS_SET(ch->comm, COMM_NOAUCTION))
   {
      strcat(buf, "ON\n\r");
   }
   else
   {
      strcat(buf, "OFF\n\r");
   }

   strcat(buf, "Q/A            ");
   if (!IS_SET(ch->comm, COMM_NOQUESTION))
   {
      strcat(buf, "ON\n\r");
   }
   else
   {
      strcat(buf, "OFF\n\r");
   }

   if (IS_IMMORTAL(ch))
   {
      strcat(buf, "god channel    ");
      if (!IS_SET(ch->comm, COMM_NOWIZ))
      {
         strcat(buf, "ON\n\r");
      }
      else
      {
         strcat(buf, "OFF\n\r");
      }
   }

   strcat(buf, "tells          ");
   if (!IS_SET(ch->comm, COMM_DEAF))
   {
      strcat(buf, "ON\n\r");
   }
   else
   {
      strcat(buf, "OFF\n\r");
   }

   strcat(buf, "quiet mode     ");
   if (IS_SET(ch->comm, COMM_QUIET))
   {
      strcat(buf, "ON\n\r");
   }
   else
   {
      strcat(buf, "OFF\n\r");
   }

   if (IS_SET(ch->comm, COMM_SNOOP_PROOF))
   {
      strcat(buf, "You are immune to snooping.\n\r");
   }

   if (ch->lines != PAGELEN)
   {
      if (ch->lines)
      {
         sprintf
         (
            buf,
            "%sYou display %d lines of scroll.\n\r",
            buf,
            ch->lines + 2
         );
      }
      else
      {
         strcat(buf, "Scroll buffering is off.\n\r");
      }
   }

   if (IS_SET(ch->comm, COMM_NOTELL))
   {
      strcat(buf, "You cannot use tell.\n\r");
   }

   if (IS_SET(ch->comm, COMM_NOPRAY))
   {
      strcat(buf, "You cannot use pray.\n\r");
   }

   if (IS_SET(ch->comm, COMM_NOCHANNELS))
   {
      strcat(buf, "You cannot use channels.\n\r");
   }

   if (IS_SET(ch->comm, COMM_NOEMOTE))
   {
      strcat(buf, "You cannot show emotions.\n\r");
   }

   if (IS_SET(ch->comm, COMM_SILENCE))
   {
      strcat(buf, "You are completely silenced.\n\r");
   }
   send_to_char(buf, ch);
}

/* RT deaf blocks out all shouts */
void do_deaf(CHAR_DATA* ch, char* argument)
{
   if (IS_SET(ch->comm, COMM_DEAF))
   {
      send_to_char("You can hear tells again.\n\r", ch);
      REMOVE_BIT(ch->comm, COMM_DEAF);
   }
   else
   {
      send_to_char("From now on, you will not hear tells.\n\r", ch);
      SET_BIT(ch->comm, COMM_DEAF);
   }
}

void do_limits(CHAR_DATA* ch, char* argument)
{
   if (IS_SET(ch->comm, COMM_LIMITS))
   {
      send_to_char("You will no longer see limited item auras.\n\r", ch);
      REMOVE_BIT(ch->comm, COMM_LIMITS);
   }
   else
   {
      send_to_char("You will now see limited item auras.\n\r", ch);
      SET_BIT(ch->comm, COMM_LIMITS);
   }
}

/* RT quiet blocks out all communication */
void do_quiet(CHAR_DATA* ch, char* argument)
{
   if (IS_SET(ch->comm, COMM_QUIET))
   {
      send_to_char("Quiet mode removed.\n\r", ch);
      REMOVE_BIT(ch->comm, COMM_QUIET);
   }
   else
   {
      send_to_char("From now on, you will only hear says and emotes.\n\r", ch);
      SET_BIT(ch->comm, COMM_QUIET);
   }
}

void do_replay(CHAR_DATA* ch, char* argument)
{
   if (IS_NPC(ch))
   {
      send_to_char("You cannot replay.\n\r", ch);
      return;
   }

   if (buf_string(ch->pcdata->buffer)[0] == '\0')
   {
      send_to_char("You have no tells to replay.\n\r", ch);
      return;
   }

   page_to_char(buf_string(ch->pcdata->buffer), ch);
   clear_buf(ch->pcdata->buffer);
}

/* RT auction rewritten in ROM style */
void do_auction(CHAR_DATA* ch, char* argument)
{
   char buf[MAX_STRING_LENGTH];
   DESCRIPTOR_DATA* d;
   OBJ_DATA* brand;
   char original[MAX_INPUT_LENGTH];

   if (argument[0] == '\0')
   {
      if (IS_SET(ch->comm, COMM_NOAUCTION))
      {
         send_to_char("Auction channel is now ON.\n\r", ch);
         REMOVE_BIT(ch->comm, COMM_NOAUCTION);
      }
      else
      {
         send_to_char("Auction channel is now OFF.\n\r", ch);
         SET_BIT(ch->comm, COMM_NOAUCTION);
      }
      return;
   }
   if (!IS_AWAKE(ch))
   {
      send_to_char("In your dreams, or what?\n\r", ch);
      return;
   }
   /* auction message sent, turn auction on if it is off */
   strcpy(original, argument);
   argument = original;
   if (is_affected(ch, gsn_toad))
   {
      strcpy(argument, "<ribbit>");
   }
   if (IS_SET(ch->comm, COMM_SILENCE))
   {
      send_to_char("You are unable to communicate.\n\r", ch);
      return;
   }
   if (is_affected(ch, gsn_mute))
   {
      send_to_char
      (
         "You try to yell, but only manage a weak rasping noise.\n\r",
         ch
      );
      return;
   }
   if (is_affected(ch, gsn_gag))
   {
      send_to_char("You cannot you are gagged.\n\r", ch);
      return;
   }
   if (IS_SET(ch->comm, COMM_QUIET))
   {
      send_to_char("You must turn off quiet mode first.\n\r", ch);
      return;
   }

   if (IS_SET(ch->comm, COMM_NOCHANNELS))
   {
      send_to_char("The gods have revoked your channel priviliges.\n\r", ch);
      return;
   }

   if
   (
      is_affected(ch, gsn_chaos_mind) ||
      is_affected(ch, gsn_dancestaves)
   )
   {
      strcpy(argument, "Um....Huh?");
   }
   if (is_affected(ch, gsn_silence))
   {
      send_to_char("You cannot find the words within you.\n\r", ch);
      return;
   }
   brand = get_eq_char(ch, WEAR_BRAND);
   if
   (
      brand != NULL &&
      brand->pIndexData->vnum == 15300
   )
   {
      send_to_char
      (
         "Your brand of the silent parrot burns as your voice carries across"
         " the region.\n\r",
         ch
      );
      damage(ch, ch, 5, gsn_parrot, DAM_PIERCE, TRUE);
      ch->exp -= 5;
      ch->move -= 25;
   }
   REMOVE_BIT(ch->comm, COMM_NOAUCTION);
   if (IS_SET(ch->comm, COMM_ANSI))
   {
      sprintf(buf, "You auction '\x01B[1;36m%s\x01B[0;37m'\n\r", argument);
   }
   else
   {
      sprintf(buf, "You auction '%s'\n\r", argument);
   }
   send_to_char(buf, ch);
   for (d = descriptor_list; d != NULL; d = d->next)
   {
      CHAR_DATA* victim;

      victim = d->original ? d->original : d->character;

      /* Added awake check to auction - Werv */

      if
      (
         d->connected == CON_PLAYING &&
         d->character != ch &&
         ch->in_room->area == victim->in_room->area &&
         !IS_SET(victim->comm, COMM_NOAUCTION) &&
         !IS_SET(victim->comm, COMM_QUIET) &&
         !is_ignored(ch, victim) &&
         IS_AWAKE(victim)
      )
      {
         if
         (
            is_affected(d->character, gsn_shock_sphere) &&
            !is_mental(ch)
         )
         {
            continue;
         }
         if (IS_SET(d->character->comm, COMM_ANSI))
         {
            act_new
            (
               "$n auctions '\x01B[1;36m$t\x01B[0;37m'",
               ch,
               argument,
               d->character,
               TO_VICT,
               POS_DEAD
            );
         }
         else
         {
            act_new
            (
               "$n auctions '$t'",
               ch,
               argument,
               d->character,
               TO_VICT,
               POS_DEAD
            );
         }
      }
   }
}

/* RT question channel */
void do_question(CHAR_DATA* ch, char* argument)
{
   char buf[MAX_STRING_LENGTH];
   DESCRIPTOR_DATA* d;
   char original[MAX_INPUT_LENGTH];

   if (is_affected(ch, gsn_mute))
   {
      send_to_char
      (
         "You try to yell, but only manage a weak rasping noise.\n\r",
         ch
      );
      return;
   }
   if (is_affected(ch, gsn_gag))
   {
      send_to_char("You cannot you are gagged.\n\r", ch);
      return;
   }

   if (argument[0] == '\0')
   {
      if (IS_SET(ch->comm, COMM_NOQUESTION))
      {
         send_to_char("Q/A channel is now ON.\n\r", ch);
         REMOVE_BIT(ch->comm, COMM_NOQUESTION);
      }
      else
      {
         send_to_char("Q/A channel is now OFF.\n\r", ch);
         SET_BIT(ch->comm, COMM_NOQUESTION);
      }
      return;
   }
   /* question sent, turn Q/A on if it isn't already */
   if (IS_SET(ch->comm, COMM_SILENCE))
   {
      send_to_char("You are unable to communicate.\n\r", ch);
      return;
   }

   if (IS_SET(ch->comm, COMM_QUIET))
   {
      send_to_char("You must turn off quiet mode first.\n\r", ch);
      return;
   }

   if (IS_SET(ch->comm, COMM_NOCHANNELS))
   {
      send_to_char
      (
         "The gods have revoked your channel priviliges.\n\r",
         ch
      );
      return;
   }

   strcpy(original, argument);
   argument = original;
   if (is_affected(ch, gsn_toad))
   {
      strcpy(argument,  "<ribbit>");
   }

   REMOVE_BIT(ch->comm, COMM_NOQUESTION);
   if (IS_SET(ch->comm, COMM_ANSI))
   {
      sprintf(buf, "You question '\x01B[1;36m%s\x01B[0;37m'\n\r", argument);
   }
   else
   {
      sprintf(buf, "You question '%s'\n\r", argument);
   }
   send_to_char(buf, ch);
   for (d = descriptor_list; d != NULL; d = d->next)
   {
      CHAR_DATA* victim;

      victim = d->original ? d->original : d->character;

      if
      (
         d->connected == CON_PLAYING &&
         d->character != ch &&
         d->character->in_room->area == ch->in_room->area &&
         !IS_SET(victim->comm, COMM_NOQUESTION) &&
         !IS_SET(victim->comm, COMM_QUIET) &&
         !is_ignored(ch, victim)
      )
      {
         if
         (
            is_affected(d->character, gsn_shock_sphere) &&
            !is_mental(ch)
         )
         {
            continue;
         }
         if (IS_SET(d->character->comm, COMM_ANSI))
         {
            act_new
            (
               "$n questions '\x01B[1;36m$t\x01B[0;37m'",
               ch,
               argument,
               d->character,
               TO_VICT,
               POS_SLEEPING
            );
         }
         else
         {
            act_new
            (
               "$n questions '$t'",
               ch,
               argument,
               d->character,
               TO_VICT,
               POS_SLEEPING
            );
         }
      }
   }
}

/* RT answer channel - uses same line as questions */
void do_answer(CHAR_DATA* ch, char* argument)
{
   char buf[MAX_STRING_LENGTH];
   DESCRIPTOR_DATA* d;
   char original[MAX_INPUT_LENGTH];

   if (argument[0] == '\0')
   {
      if (IS_SET(ch->comm, COMM_NOQUESTION))
      {
         send_to_char("Q/A channel is now ON.\n\r", ch);
         REMOVE_BIT(ch->comm, COMM_NOQUESTION);
      }
      else
      {
         send_to_char("Q/A channel is now OFF.\n\r", ch);
         SET_BIT(ch->comm, COMM_NOQUESTION);
      }
      return;
   }

   /* answer sent, turn Q/A on if it isn't already */
   if (IS_SET(ch->comm, COMM_SILENCE))
   {
      send_to_char("You are unable to communicate.\n\r", ch);
      return;
   }

   if (IS_SET(ch->comm, COMM_QUIET))
   {
      send_to_char("You must turn off quiet mode first.\n\r", ch);
      return;
   }

   if (IS_SET(ch->comm, COMM_NOCHANNELS))
   {
      send_to_char("The gods have revoked your channel priviliges.\n\r", ch);
      return;
   }

   strcpy(original, argument);
   argument = original;


   if
   (
      is_affected(ch, gsn_chaos_mind) ||
      is_affected(ch, gsn_dancestaves)
   )
   {
      strcpy(argument, "Um...Huh?");
   }

   if (is_affected(ch, gsn_toad))
   {
      strcpy(argument, "<ribbit>");
   }

   REMOVE_BIT(ch->comm, COMM_NOQUESTION);
   if (IS_SET(ch->comm, COMM_ANSI))
   {
      sprintf(buf, "You answer '\x01B[1;36m%s\x01B[0;37m'\n\r", argument);
   }
   else
   {
      sprintf(buf, "You answer '%s'\n\r", argument);
   }
   send_to_char(buf, ch);
   for (d = descriptor_list; d != NULL; d = d->next)
   {
      CHAR_DATA* victim;

      victim = d->original ? d->original : d->character;

      if
      (
         d->connected == CON_PLAYING &&
         d->character != ch &&
         d->character->in_room->area == ch->in_room->area &&
         !IS_SET(victim->comm, COMM_NOQUESTION) &&
         !IS_SET(victim->comm, COMM_QUIET) &&
         !is_ignored(ch, victim)
      )
      {
         if (IS_SET(d->character->comm, COMM_ANSI))
         {
            act_new
            (
               "$n answers '\x01B[1;36m$t\x01B[0;37m'",
               ch,
               argument,
               d->character,
               TO_VICT,
               POS_SLEEPING
            );
         }
         else
         {
            act_new
            (
               "$n answers '$t'",
               ch,
               argument,
               d->character,
               TO_VICT,
               POS_SLEEPING
            );
         }
      }
   }
}

void do_houtlaw(CHAR_DATA* ch, char* argument)
{
   char buf[MAX_STRING_LENGTH];
   DESCRIPTOR_DATA* d;
   int house = HOUSE_OUTLAW;

   if (!IS_IMMORTAL(ch))
   {
      return;
   }

   if (IS_SET(ch->comm, COMM_ANSI))
   {
      if (IS_SET(ch->comm2, COMM_REDUCED_COLOR))
      {
         sprintf
         (
            buf,
            "[\x01B[1;37m%s\x01B[0;37m] Someone: %s\n\r",
            full_capitalize(house_table[house].name),
            argument
         );
      }
      else
      {
         sprintf
         (
            buf,
            "\x01B[1;37m%sSomeone: %s\x01B[0;37m\n\r",
            house_table[house].who_name,
            argument
         );
      }
   }
   else
   {
      sprintf(buf, "%sSomeone: %s\n\r", house_table[house].who_name, argument);
   }
   send_to_char(buf, ch);
   for (d = descriptor_list; d != NULL; d = d->next)
   {
      if
      (
         d->connected == CON_PLAYING &&
         d->character != ch &&
         !is_ignored(ch, d->character) &&
         (
            d->character->house == HOUSE_OUTLAW ||
            IS_SET(d->character->comm, COMM_ALL_HOUSES)
         )
      )
      {
         if (IS_SET(d->character->comm, COMM_ANSI))
         {
            if (IS_SET(d->character->comm2, COMM_REDUCED_COLOR))
            {
               sprintf
               (
                  buf,
                  "[\x01B[1;37m%s\x01B[0;37m] Someone: %s\n\r",
                  full_capitalize(house_table[house].name),
                  argument
               );
            }
            else
            {
               sprintf
               (
                  buf,
                  "\x01B[1;37m%sSomeone: %s\x01B[0;37m\n\r",
                  house_table[house].who_name,
                  argument
               );
            }
         }
         else
         {
            sprintf
            (
               buf,
               "%sSomeone: %s\n\r",
               house_table[house].who_name,
               argument
            );
         }
         send_to_char(buf, d->character);
         if (IS_SET(d->character->act2, PLR_LOG_SPEC))
         {
            log_convo(d->character, ch, TYPE_HOUSE2, argument);
         }
      }
   }
   return;
}

void do_housetalk(CHAR_DATA* ch, char* argument)
{
   char buf[MAX_STRING_LENGTH];
   DESCRIPTOR_DATA* d;
   char original[MAX_INPUT_LENGTH];

   if (!is_house(ch) || house_table[ch->house].independent)
   {
      send_to_char("You are not in a house.\n\r", ch);
      return;
   }
   if (argument[0] == '\0')
   {
      if (IS_SET(ch->comm, COMM_NOHOUSE))
      {
         send_to_char("House channel is now ON\n\r", ch);
         REMOVE_BIT(ch->comm, COMM_NOHOUSE);
      }
      else
      {
         send_to_char("House channel is now OFF\n\r", ch);
         SET_BIT(ch->comm, COMM_NOHOUSE);
      }
      return;
   }
   if
   (
      !IS_NPC(ch) &&
      (
         ch->pcdata->induct <= INDUCT_APPRENTICE &&
         ch->house != HOUSE_MARAUDER
      ) &&
      !IS_IMMORTAL(ch)
   )
   {
      send_to_char("You may not speak to the house in that fashion.\n\r", ch);
      return;
   }
   if (is_affected(ch, gsn_mute))
   {
      send_to_char
      (
         "You try to tell them something, but only manage a weak rasping noise."
         "\n\r",
         ch
      );
      return;
   }
   if (is_affected(ch, gsn_gag))
   {
      send_to_char("You cannot you are gagged.\n\r", ch);
      return;
   }

   if (is_affected(ch, gsn_silence))
   {
      send_to_char("You cannot find the words within you.\n\r", ch);
      return;
   }

   if (IS_SET(ch->comm, COMM_SILENCE))
   {
      send_to_char("You are unable to communicate.\n\r", ch);
      return;
   }

   if (IS_SET(ch->comm, COMM_NOCHANNELS))
   {
      send_to_char("The gods have revoked your channel priviliges.\n\r", ch);
      return;
   }

   REMOVE_BIT(ch->comm, COMM_NOHOUSE);

   original[0] = '\0';
   /* place ancient titles in ht */
   if (!IS_NPC(ch) && ch->house == HOUSE_ANCIENT)
   {
      sprintf(original, "(%s) ", get_ancient_title(ch));
   }
   if (is_affected(ch, gsn_toad))
   {
      strcat(original,  "<ribbit>");
   }
   else
   {
      strcat(original, argument);
   }
   argument = original;

   if (IS_SET(ch->act2, PLR_LOG_SPEC))
   {
      log_convo(ch, NULL, TYPE_HOUSE, argument);
   }

   if (IS_SET(ch->comm, COMM_ANSI))
   {
      if (IS_SET(ch->comm2, COMM_REDUCED_COLOR))
      {
         sprintf
         (
            buf,
            "[\x01B[1;37m%s\x01B[0;37m] %s: %s\n\r",
            full_capitalize(house_table[ch->house].name),
            IS_NPC(ch) ? ch->short_descr : ch->name,
            argument
         );
      }
      else
      {
         sprintf
         (
            buf,
            "\x01B[1;37m%s%s: %s\x01B[0;37m\n\r",
            house_table[ch->house].who_name,
            IS_NPC(ch) ? ch->short_descr : ch->name,
            argument
         );
      }
   }
   else
   {
      sprintf
      (
         buf,
         "%s%s: %s\n\r",
         house_table[ch->house].who_name,
         IS_NPC(ch) ? ch->short_descr : ch->name,
         argument
      );
   }
   send_to_char(buf, ch);

   for (d = descriptor_list; d != NULL; d = d->next)
   {
      if
      (
         d->character &&
         is_affected(d->character, gsn_shock_sphere) &&
         !is_mental(ch)
      )
      {
         continue;
      }

      if
      (
         d->connected == CON_PLAYING &&
         d->character != ch &&
         !is_ignored(ch, d->character) &&
         (
            (
               (
                  is_same_house(ch, d->character) ||
                  (
                     IS_SET(d->character->act2, PLR_IS_ANCIENT) &&
                     ch->house == HOUSE_ANCIENT
                  )
               ) &&
               (
                  IS_NPC(d->character) ||
                  (
                     d->character->pcdata->induct > INDUCT_APPRENTICE ||
                     d->character->house == HOUSE_MARAUDER
                  ) ||
                  IS_IMMORTAL(d->character)
               ) &&
               !IS_SET(d->character->comm, COMM_NOHOUSE)
            ) ||
            IS_SET(d->character->comm, COMM_ALL_HOUSES)
         )
      )
      {
         if (IS_SET(d->character->comm2, COMM_REDUCED_COLOR))
         {
            sprintf
            (
               buf,
               "[{B{7%s{n] $n: $t",
               full_capitalize(house_table[ch->house].name)
            );
         }
         else
         {
            sprintf
            (
               buf,
               "{B{7%s$n: $t",
               house_table[ch->house].who_name
            );
         }
         act_new_color
         (
            buf,
            ch,
            argument,
            d->character,
            TO_VICT,
            POS_DEAD,
            TRUE
         );
         if (IS_SET(d->character->act2, PLR_LOG_SPEC))
         {
            log_convo(d->character, ch, TYPE_HOUSE2, argument);
         }

      }
   }

   return;
}

void do_global(CHAR_DATA* ch, char* argument)
{
   if (argument[0] == '\0' || !str_cmp(argument, "house"))
   {
      if (IS_SET(ch->comm, COMM_ALL_HOUSES))
      {
         send_to_char("You will no longer hear all House channels.\n\r", ch);
         REMOVE_BIT(ch->comm, COMM_ALL_HOUSES);
      }
      else
      {
         send_to_char("You will now hear all House channels.\n\r", ch);
         SET_BIT(ch->comm, COMM_ALL_HOUSES);
      }
      return;
   }
   if (!str_cmp(argument, "clan"))
   {
      if (IS_SET(ch->comm2, COMM_CTGLOBAL))
      {
         send_to_char("You will no longer hear all Clan channels.\n\r", ch);
         REMOVE_BIT(ch->comm2, COMM_CTGLOBAL);
      }
      else
      {
         send_to_char("You will now hear all Clan channels.\n\r", ch);
         SET_BIT(ch->comm2, COMM_CTGLOBAL);
      }
      return;
   }
   if (!str_cmp(argument, "brand"))
   {
      if (!IS_TRUSTED(ch, IMMORTAL))
      {
         send_to_char
         (
            "You are not powerful enough to listen in on brand channels.\n\r",
            ch
         );
         return;
      }
      if (IS_SET(ch->comm2, COMM_BTGLOBAL))
      {
         send_to_char("You will no longer hear all Brand channels.\n\r", ch);
         REMOVE_BIT(ch->comm2, COMM_BTGLOBAL);
      }
      else
      {
         send_to_char("You will now hear all Brand channels.\n\r", ch);
         SET_BIT(ch->comm2, COMM_BTGLOBAL);
      }
      return;
   }
   send_to_char("Listen in on what kind of channels?\n\r", ch);
}

void do_immtalk(CHAR_DATA* ch, char* argument)
{
   CHAR_DATA* wch;
   DESCRIPTOR_DATA* d;
   int trust;

   trust = get_trust(ch);

   if
   (
      trust < 52 &&
      !IS_SET(ch->comm, COMM_IMMORTAL)
   )
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }

   if (argument[0] == '\0')
   {
      if (IS_SET(ch->comm, COMM_NOWIZ))
      {
         send_to_char("Immortal channel is now ON.\n\r", ch);
         REMOVE_BIT(ch->comm, COMM_NOWIZ);
      }
      else
      {
         send_to_char("Immortal channel is now OFF.\n\r", ch);
         SET_BIT(ch->comm, COMM_NOWIZ);
      }
      return;
   }

   if (IS_SET(ch->comm, COMM_SILENCE))
   {
      send_to_char("You are unable to communicate.\n\r", ch);
      return;
   }


   if (IS_SET(ch->act2, PLR_LOG_SPEC))
   {
      log_convo(ch, NULL, TYPE_IMM, argument);
   }

   REMOVE_BIT(ch->comm, COMM_NOWIZ);
   if (IS_SET(ch->comm, COMM_ANSI))
   {
      if (IS_SET(ch->comm2, COMM_REDUCED_COLOR))
      {
         act_new
         (
            "[\x01B[1;34mIMM\x01B[0;37m] $n: $t",
            ch,
            argument,
            NULL,
            TO_CHAR,
            POS_DEAD
         );
      }
      else
      {
         act_new
         (
            "\x01B[1;34m[IMM] $n: $t\x01B[0;37m",
            ch,
            argument,
            NULL,
            TO_CHAR,
            POS_DEAD
         );
      }
   }
   else
   {
      act_new("[IMM] $n: $t", ch, argument, NULL, TO_CHAR, POS_DEAD);
   }

   for (d = descriptor_list; d != NULL; d = d->next)
   {
      wch = d->character;
      if
      (
         d->connected == CON_PLAYING &&
         (
            IS_IMMORTAL(wch) ||
            IS_SET(wch->comm, COMM_IMMORTAL)
         ) &&
         !IS_SET(wch->comm, COMM_NOWIZ)
      )
      {
         if (IS_SET(wch->comm, COMM_ANSI))
         {
            if (IS_SET(wch->comm2, COMM_REDUCED_COLOR))
            {
               act_new
               (
                  "[\x01B[1;34mIMM\x01B[0;37m] $n: $t",
                  ch,
                  argument,
                  wch,
                  TO_VICT,
                  POS_DEAD
               );
            }
            else
            {
               act_new
               (
                  "\x01B[1;34m[IMM] $n: $t\x01B[0;37m",
                  ch,
                  argument,
                  wch,
                  TO_VICT,
                  POS_DEAD
               );
            }
         }
         else
         {
            act_new("[IMM] $n: $t", ch, argument, wch, TO_VICT, POS_DEAD);
         }
         if (IS_SET(wch->act2, PLR_LOG_SPEC))
         {
            log_convo(wch, ch, TYPE_IMM2, argument);
         }
      }
   }

   return;
}

void do_builder(CHAR_DATA* ch, char* argument)
{
   CHAR_DATA* wch;
   DESCRIPTOR_DATA* d;

   if (!IS_SET(ch->comm, COMM_BUILDER))
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }
   if (argument[0] == '\0')
   {
      return;
   }
   /*
   {
   if (IS_SET(ch->comm, COMM_BUILDER))
   {
   send_to_char("Builder channel is now ON\n\r", ch);
   REMOVE_BIT(ch->comm, COMM_BUILDER);
   }
   else
   {
   send_to_char("Builder channel is now OFF\n\r", ch);
   SET_BIT(ch->comm, COMM_BUILDER);
   }
   return;
   }
   */
   if (IS_SET(ch->comm2, COMM_REDUCED_COLOR))
   {
      act_new_color
      (
         "[{B{7BUILDER{n] $n: $t",
         ch,
         argument,
         NULL,
         TO_CHAR,
         POS_DEAD,
         TRUE
      );
   }
   else
   {
      act_new_color
      (
         "{B{7[BUILDER] $n: $t{n",
         ch,
         argument,
         NULL,
         TO_CHAR,
         POS_DEAD,
         TRUE
      );
   }

   for (d = descriptor_list; d != NULL; d = d->next)
   {
      wch = d->original ? d->original : d->character;
      if
      (
         d->connected == CON_PLAYING &&
         !IS_NPC(wch) &&
         wch != ch &&
         IS_SET(wch->comm, COMM_BUILDER) &&
         !is_ignored(ch, wch)
      )
      {
         if (IS_SET(wch->comm2, COMM_REDUCED_COLOR))
         {
            act_new_color
            (
               "[{B{7BUILDER{n] $n: $t",
               ch,
               argument,
               wch,
               TO_VICT,
               POS_DEAD,
               TRUE
            );
         }
         else
         {
            act_new_color
            (
               "{B{7[BUILDER] $n: $t{n",
               ch,
               argument,
               wch,
               TO_VICT,
               POS_DEAD,
               TRUE
            );
         }
      }
   }

   return;
}

void obj_trigger_6712(CHAR_DATA* ch, char* argument, OBJ_DATA* obj)
{
   CHAR_DATA* gch;

   if (stristr(argument, "Harp, cure my ills through song"))
   {
      act
      (
         "A singing harp sings, 'Tis good for the soul to hear glad song.'\n\r"
         "'Throughout the day and all night long.'\n\r"
         "'So raise your voice in song and cheer,'\n\r"
         "'And be of good health throughout the year.'\n\r"
         "The harp's voice fades, and it smiles.\n\r",
         ch,
         NULL,
         NULL,
         TO_ALL
      );
      WAIT_STATE(ch, 24);
      for (gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room)
      {
         if (is_same_group(gch, ch))
         {
            obj_cast_spell
            (
               gsn_cure_critical,
               ch->level,
               ch,
               gch,
               obj
            );
         }
      }
   }
   return;
}

void obj_trigger_14718(CHAR_DATA* ch, char* argument)
{
   int dam;

   if (ch->fighting != NULL && stristr(argument, "burn with the blaze of the tower!"))
   {
      dam = dice(ch->level / 2, 5) + dice(ch->level / 2, 4);
      send_to_char("A sudden blaze erupts from your mace!\n\r", ch);
      send_to_char("You are struck by a sudden blaze of fire!\n\r", ch->fighting);
      damage(ch, ch->fighting, dam, gsn_firestream, DAM_FIRE, TRUE);
      WAIT_STATE(ch, 3 * PULSE_VIOLENCE);
   }
   return;
}

void obj_trigger_15300(CHAR_DATA* ch, char* argument)
{
   AFFECT_DATA af;

   if
   (
      ch->fighting != NULL &&
      strstr(argument, "let Julive take your words") != NULL &&
      !is_affected(ch->fighting, gsn_silence) &&
      !is_affected(ch, gsn_parrot)
   )
   {
      send_to_char("Julive hears your prayer.\n\r", ch);
      send_to_char("You feel at a loss of words.\n\r", ch->fighting);
      af.type      = gsn_parrot;
      af.where     = TO_AFFECTS;
      af.level     = ch->level;
      af.duration  = 24;
      af.location  = 0;
      af.modifier  = 0;
      af.bitvector = 0;
      affect_to_char(ch, &af);
      af.duration  = 10;
      af.type      = gsn_silence;
      affect_to_char(ch->fighting, &af);
   }
   return;
}

void obj_trigger_425(CHAR_DATA* ch, char* argument)
{
   AFFECT_DATA af;
   OBJ_DATA* obj = get_eq_char(ch, WEAR_BRAND);

   if
   (
      ch->fighting != NULL &&
      stristr(argument, "Power within breeds influence throughout") &&
      !is_affected(ch->fighting, gsn_curse) &&
      !is_affected(ch, gsn_parrot)
   )
   {
      act
      (
         "Styx hears your prayer.\n\rYour $p glows briefly.",
         ch,
         obj,
         NULL,
         TO_CHAR
      );
      act("$n's $p glows briefly.", ch, obj, NULL, TO_ROOM);
      send_to_char("You feel weak and worthless.\n\r", ch->fighting);
      af.type       = gsn_parrot;
      af.where      = TO_AFFECTS;
      af.level      = ch->level;
      af.duration   = 24;
      af.location   = APPLY_NONE;
      af.modifier   = 0;
      af.bitvector  = 0;
      affect_to_char(ch, &af);
      af.bitvector  = AFF_CURSE;
      af.type       = gsn_curse;
      af.location   = APPLY_DAMROLL;
      af.modifier   = -10;
      af.duration   = 10;
      affect_to_char(ch->fighting, &af);
      af.location   = APPLY_SAVES;
      af.modifier   = 5;
      affect_to_char(ch->fighting, &af);
   }
   return;
}

/* Triggers for Hector's brand - Wicket */
void obj_trigger_7359(CHAR_DATA* ch, char* argument)
{
   AFFECT_DATA af;
   OBJ_DATA* obj;
   OBJ_DATA* obj_next;
   char* target_object;

   if (stristr(argument, "Grant me the spark of Innovation!"))
   {
      if (is_affected(ch, gsn_hector_brand_spark))
      {
         send_to_char
         (
            "You are already surrounded by an aura of brilliance.\n\r",
            ch
         );
         return;
      }
      send_to_char("You are surrounded by a brilliant aura!\n\r", ch);
      act("$n is surrounded by a brilliant aura!", ch, NULL, NULL, TO_ROOM);
      af.type      = gsn_hector_brand_spark;
      af.where     = TO_AFFECTS;
      af.level     = ch->level;
      af.duration  = 24;
      af.location  = APPLY_NONE;
      af.modifier  = 0;
      af.bitvector = 0;
      affect_to_char(ch, &af);
      return;
   }
   if (stristr(argument, "I wish to view the essence of "))
   {
      char buf[MAX_INPUT_LENGTH];

      if (is_affected(ch, gsn_hector_brand_essence))
      {
         send_to_char
         (
            "You have already peered into the essence of an object recently."
            "\n\r",
            ch
         );
         return;
      }
      strcpy
      (
         buf,
         stristr(argument, "I wish to view the essence of ") +
         30 /* 30 == strlen("I wish to view the essence of ") */
      );
      argument = buf;

      /* Get rid of any unwanted end characters */
      if (strlen(argument) > 0)
      {
         switch (argument[strlen(argument) - 1])
         {
            default:
            {
               break;
            }
            case ('.'):
            case ('!'):
            case (','):
            {
               argument[strlen(argument) - 1] = '\0';
            }
         }
      }
      target_object = argument;

      /* Compare's the descriptions of the item if found */
      for (obj = ch->carrying; obj != NULL; obj = obj_next)
      {
         obj_next = obj->next_content;
         if
         (
            !str_cmp(target_object, obj->short_descr) &&
            can_see_obj(ch, obj)
         )
         {
            break;
         }
      }

      if (obj == NULL)
      {
         send_to_char("You don't have that within your possession.\n\r", ch);
         return;
      }

      if (obj->wear_loc != WEAR_NONE)
      {
         send_to_char("You are not carrying that.\n\r", ch);
         return;
      }

      act("$p glows with a bright yellow aura!", ch, obj, NULL, TO_ALL);
      spell_identify
      (
         gsn_hector_brand_essence,
         ch->level + 4,
         ch,
         (void*)obj,
         TARGET_OBJ
      );
      af.type  = gsn_hector_brand_essence;
      af.where = TO_AFFECTS;
      af.level = ch->level;
      af.duration  = 24;
      af.location  = APPLY_NONE;
      af.modifier  = 0;
      af.bitvector = 0;
      affect_to_char(ch, &af);
   }
   return;
}

/* Triggers for Lestregus' brand - Maelstrom */
void obj_say_7608(CHAR_DATA* ch, char* argument, OBJ_DATA *brand)
{
   AFFECT_DATA af;
   OBJ_DATA* obj;
   char buf[MAX_STRING_LENGTH];

   if (!brand)
   {
      return;
   }

   if (stristr(argument, "I am Eternal!"))
   {
      if (is_affected(ch, gsn_lestregus_brand_eternal))
      {
         act(
            "$p glows softly for a moment, then fades.",
            ch,
            brand,
            NULL,
            TO_CHAR);
         act(
            "$p glows softly for a moment, then fades.",
            ch,
            brand,
            NULL,
            TO_ROOM);

         return;
      }

      act(
         "$p flares as your eyes glow a soft red.",
         ch,
         brand,
         NULL,
         TO_CHAR);
      act(
         "$p glows softly.",
         ch,
         brand,
         NULL,
         TO_ROOM);

      af.type      = gsn_lestregus_brand_eternal;
      af.where     = TO_AFFECTS;
      af.level     = GET_LEVEL(ch);
      af.duration  = GET_LEVEL(ch);
      af.location  = APPLY_NONE;
      af.modifier  = 0;
      af.bitvector = AFF_DETECT_GOOD;
      affect_to_char(ch, &af);

      damage(ch, ch, 50, gsn_lestregus_brand_eternal, DAM_OTHER, TRUE);
      return;
   }

   if (stristr(argument, "Darkness, release me from my prison."))
   {
      if (is_affected(ch, gsn_lestregus_brand_taint))
      {
         act(
            "$p glows softly for a moment, then fades.",
            ch,
            brand,
            NULL,
            TO_CHAR);
         act(
            "$p glows softly for a moment, then fades.",
            ch,
            brand,
            NULL,
            TO_ROOM);

         return;
      }

      for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
      {
         if (
               IS_OBJ_STAT(obj, ITEM_ANTI_EVIL) ||
               IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL)
            )
         {

            if (!obj_can_change_align_restrictions(obj))
            {
               act(
                  "This object cannot be imbued with affliction.",
                  ch,
                  NULL,
                  NULL,
                  TO_CHAR);
               return;
            }

            act(
               "$p glows red, infused with the taint of affliction.",
               ch,
               obj,
               NULL,
               TO_CHAR);
            act(
               "$p flares brightly!",
               ch,
               brand,
               NULL,
               TO_ROOM);

            af.type = gsn_lestregus_brand_taint;
            af.where = TO_AFFECTS;
            af.level = GET_LEVEL(ch);
            af.duration  = 24;
            af.location  = APPLY_NONE;
            af.modifier  = 0;
            af.bitvector = 0;
            affect_to_char(ch, &af);

            REMOVE_BIT(obj->extra_flags, ITEM_ANTI_EVIL);
            REMOVE_BIT(obj->extra_flags, ITEM_ANTI_NEUTRAL);
            SET_BIT(obj->extra_flags, ITEM_ROT_DEATH);
            SET_BIT(obj->extra_flags, ITEM_NOUNCURSE);
            SET_BIT(obj->extra_flags, ITEM_NODROP);
            SET_BIT(obj->extra_flags, ITEM_EVIL);
            sprintf(buf, "%s", ch->name);
            free_string(obj->owner);
            obj->owner = str_dup(buf);

            af.where = TO_OBJECT;
            af.duration = -1;

            switch(number_range(1, 5))
            {
               case 1:
                  switch(number_range(1, 5))
                  {
                     case 1:
                        af.location = APPLY_STR;
                        break;
                     case 2:
                        af.location = APPLY_INT;
                        break;
                     case 3:
                        af.location = APPLY_WIS;
                        break;
                     case 4:
                        af.location = APPLY_DEX;
                        break;
                     case 5:
                     default:
                        af.location = APPLY_CON;
                        break;
                  }
                  af.modifier = -1;
                  break;
               case 2:
                  af.location = APPLY_HITROLL;
                  af.modifier = -2;
                  break;
               case 3:
                  af.location = APPLY_DAMROLL;
                  af.modifier = -2;
                  break;
               case 4:
                  af.location = APPLY_HIT;
                  af.modifier = -10;
                  break;
               case 5:
               default:
                  af.location = APPLY_MANA;
                  af.modifier = -10;
                  break;
            }
            affect_to_obj(obj, &af);

            damage(ch, ch, 50, gsn_lestregus_brand_taint, DAM_OTHER, TRUE);
            return;
         }
      }

      if (!obj)
      {
         act(
            "You have nothing to imbue with the power of affliction!",
            ch,
            NULL,
            NULL,
            TO_CHAR);

         return;
      }
   }
   return;
}


void obj_trigger_sham_brand(CHAR_DATA* ch, char* argument)
{
   AFFECT_DATA af;

   if (stristr(argument, "Sentinel protect my dreams"))
   {
      if (is_affected(ch, gsn_sham_brand1))
      {
         send_to_char("The Mosaic Dove guides your dreams already.\n\r", ch);
         return;
      }
      send_to_char("Your Mosaic Dove glows violet.\n\r", ch);
      af.type  = gsn_sham_brand1;
      af.where = TO_AFFECTS;
      af.level = ch->level;
      af.duration  = 24;
      af.location  = APPLY_NONE;
      af.modifier  = 0;
      af.bitvector = 0;
      affect_to_char(ch, &af);
      return;
   }
   if (stristr(argument, "May your path be cursed by delusions"))
   {
      if (ch->fighting == NULL)
      {
         send_to_char("Your prayers are not understood.\n\r", ch);
         return;
      }
      if (is_affected(ch, gsn_sham_brand2))
      {
         send_to_char("You've already called upon the dove.\n\r", ch);
         return;
      }
      send_to_char
      (
         "Your brand emits a violet aura that consumes your foes.\n\r",
         ch
      );
      act("$n's Mosaic Dove flashes violet.", ch, NULL, NULL, TO_ROOM);
      act
      (
         "Your world is consumed by an illusion.",
         ch,
         NULL,
         ch->fighting,
         TO_VICT
      );
      af.where = TO_AFFECTS;
      af.type  = gsn_delusions;
      af.level = ch->level;
      af.duration  = 2;
      af.location  = APPLY_NONE;
      af.modifier  = 0;
      af.bitvector = 0;
      affect_to_char(ch->fighting, &af);
      af.duration  = 18;
      af.type      = gsn_sham_brand2;
      affect_to_char(ch, &af);
      return;
   }
   return;
}

/* Triggers for Utara's brand - Wicket */
void obj_trigger_7585( CHAR_DATA* ch, char* argument )
{
   AFFECT_DATA af;

   if
   (
      stristr
      (
         argument,
         "Let the realms suffer, that the Gremlin may prosper!"
      )
   )
   {
      if (is_affected(ch, gsn_utara_brand_blur))
      {
         send_to_char
         (
            "Your body already distorts with the blessing of the"
            " Dancing Gremlin.\n\r",
            ch
         );
         return;
      }
      send_to_char
      (
         "Your brand of the Dancing Gremlin glows a bright emerald.\n\r",
         ch
      );
      act
      (
         "$n's brand of the Dancing Gremlin glows a bright emerald.",
         ch,
         NULL,
         NULL,
         TO_ROOM
      );
      af.type      = gsn_utara_brand_blur;
      af.where     = TO_AFFECTS;
      af.level     = ch->level;
      af.duration  = 24;
      af.location  = APPLY_NONE;
      af.modifier  = 0;
      af.bitvector = 0;
      affect_to_char(ch, &af);
      return;
   }
   if (stristr(argument, "Let Lady Utara guide your way!"))
   {
      if (ch->fighting == NULL)
      {
         send_to_char("Your prayers are not understood.\n\r", ch);
         return;
      }
      if (is_affected(ch, gsn_utara_brand2))
      {
         send_to_char("You have already called upon the gremlin.\n\r", ch);
         return;
      }
      act
      (
         "Your brand of the Dancing Gremlin glows a bright emerald.\n\r"
         "You spin $N around several times and they become disoriented!",
         ch,
         NULL,
         ch->fighting,
         TO_CHAR
      );
      act
      (
         "$n's brand of the Dancing Gremlin glows a bright emerald.",
         ch,
         NULL,
         NULL,
         TO_ROOM
      );
      act
      (
         "$n spins you around several times and disorients you!",
         ch,
         NULL,
         ch->fighting,
         TO_VICT
      );

      af.where = TO_AFFECTS;
      af.type  = gsn_misdirection;
      af.level = ch->level;
      af.duration  = 3;
      af.location  = APPLY_NONE;
      af.modifier  = 0;
      af.bitvector = 0;
      affect_to_char(ch->fighting, &af);
      af.duration  = 24;
      af.type      = gsn_utara_brand2;
      affect_to_char(ch, &af);
      return;
   }
   return;
}

void obj_trigger_429(CHAR_DATA* ch, char* argument)
{
   AFFECT_DATA af;
   CHAR_DATA* victim;
   CHAR_DATA* follower;
   CHAR_DATA* follower_next;
   ROOM_INDEX_DATA* pRoomIndex;
   int chance;

   if (strstr(argument, "I love my gums"))
   {
      if (is_affected(ch, gsn_toothless))
      {
         send_to_char("Your gums need to rest.\n\r", ch);
         return;
      }
      else
      {
         act("A toothless aura surrounds $n.", ch, NULL, NULL, TO_ROOM);
         send_to_char("A toothless aura surrounds you!\n\r", ch);

         af.where     = TO_AFFECTS;
         af.location  = APPLY_NONE;
         af.modifier  = 0;
         af.type      = gsn_toothless;
         af.level     = ch->level;
         af.duration  = 15;
         af.bitvector = 0;
         affect_to_char(ch, &af);
         WAIT_STATE(ch, 24);

         victim = ch->in_room->people;
         for (;;)
         {
            if (victim == NULL)
            {
               return;
            }
            if
            (
               (
                  IS_NPC(victim) &&
                  (
                     IS_SET(victim->imm_flags, IMM_CHARM) ||
                     IS_AFFECTED(victim, AFF_CHARM)
                  )
               ) ||
               (
                  !IS_NPC(victim) &&
                  (
                     (
                        is_safe
                        (
                           ch,
                           victim,
                           IS_SAFE_SILENT | IS_SAFE_IGNORE_AFFECTS
                        ) &&
                        !IS_IMMORTAL(ch)
                     ) ||
                     IS_IMMORTAL(victim)
                  )
               ) ||
               victim == ch
            )
            {
               victim = victim->next_in_room;
               continue;
            }
            chance  = 90;
            chance += (ch->level - victim->level) * 3;
            if (victim->level - ch->level > 8)
            {
               chance += (ch->level - victim->level) * 8;
            }
            if
            (
               number_percent() > chance ||
               saves_spell(ch, ch->level, victim, DAM_OTHER, SAVE_SPELL)
            )
            {
               victim = victim->next_in_room;
               continue;
            }
            if ((pRoomIndex = get_random_room(victim, RANDOM_NORMAL)) == NULL)
            {
               bug("NULL room from get_random_room", 0);
               victim = victim->next_in_room;
               continue;
            }
            act
            (
               "$n is sucked into the toothless vortex!",
               victim,
               NULL,
               NULL,
               TO_ROOM
            );
            send_to_char("You are sucked into the toothless vortex!\n\r", victim);
            char_from_room(victim);
            char_to_room(victim, pRoomIndex);
            do_observe(victim, "", LOOK_AUTO);
            if (victim->fighting != NULL)
            {
               stop_fighting(victim, TRUE);
            }
            DAZE_STATE(victim, 24);

            if (!IS_NPC(victim))
            {
               bool found;
               found = FALSE;

               for (follower = ch->in_room->people; follower; follower = follower_next)
               {
                  follower_next = follower->next_in_room;
                  if (!IS_NPC(follower) || !IS_AFFECTED(follower, AFF_CHARM))
                  {
                     continue;
                  }
                  if (follower->leader == victim)
                  {
                     char_from_room(follower);
                     char_to_room(follower, pRoomIndex);
                     if (follower->fighting != NULL)
                     {
                        stop_fighting(victim, TRUE);
                     }
                     found = TRUE;
                     follower = ch->in_room->people;
                  }
               }
               if (found)
               {
                  act
                  (
                     "$n suddenly appears along with $s followers!",
                     victim,
                     NULL,
                     NULL,
                     TO_ROOM
                  );
                  send_to_char("Your followers are sucked along with you.\n\r", victim);
               }
               else
               {
                  act("$n suddenly appears!", victim, NULL, NULL, TO_ROOM);
               }
            }
            victim = ch->in_room->people;
         }
      }
   }
   return;
}

void obj_say_heavenly_sceptre(CHAR_DATA* ch, OBJ_DATA* obj, char* argument)
{
   int sn;
   sn = -1;

   if
   (
      stristr(argument, "Feel now the wrath of god") ||
      stristr(argument, "I strike you down with the force of god")
   )
   {
      sn = gsn_heavenly_wrath;
   }

   if (sn == -1)
   {
      return;
   }

   magic_spell
   (
      ch,
      "blah self",
      CAST_BITS_OBJ_SPELL,
      obj->level,
      sn
   );
   return;
}

void obj_say_oblivion(CHAR_DATA* ch, char* argument)
{
   AFFECT_DATA* paf;

   if
   (
      IS_NPC(ch) ||
      is_affected(ch, gsn_will_power) ||
      is_affected(ch, gsn_will_dread) ||
      is_affected(ch, gsn_will_oblivion)
   )
   {
      return;
   }
   if
   (
      stristr
      (
         argument,
         "Torn and broken, I walk the path"
      ) != NULL
   )
   {
      act
      (
         "$n begins to harness the power of the Dread-Lord, but falters in $s"
         " prayer.",
         ch,
         NULL,
         NULL,
         TO_ROOM
      );
      send_to_char
      (
         "You try to harness the power of your faith, but have not the focus to"
         " find Oblivion's Embrace.\n\r",
         ch
      );
      return;
   }
   if
   (
      ch->pcdata->brand_rank > 0 &&
      stristr
      (
         argument,
         "The way is made clear, the circle must break"
      ) != NULL
   )
   {
      paf = new_affect();
      paf->where = TO_AFFECTS;
      paf->level = ch->level;
      paf->duration = 12;
      paf->type = gsn_will_power;
      affect_to_char(ch, paf);
      free_affect(paf);
      act
      (
         "$n focuses $s will and $s body seems to course with power.",
         ch,
         NULL,
         NULL,
         TO_ROOM
      );
      send_to_char
      (
         "You clear all thoughts from your mind, focusing your will as the"
         " power of the Dread-Lord suffuses your being.\n\r",
         ch
      );
      return;
   }
   if
   (
      ch->pcdata->brand_rank > 1 &&
      stristr
      (
         argument,
         "Dread not the shadow, for it holds no truth"
      ) != NULL
   )
   {
      paf = new_affect();
      paf->where = TO_AFFECTS;
      paf->level = ch->level;
      paf->duration = 12;
      paf->type = gsn_will_dread;
      affect_to_char(ch, paf);
      free_affect(paf);
      act
      (
         "$n focuses $s will on the blessing of the Dread-Lord and begins to"
         " fade from existence.",
         ch,
         NULL,
         NULL,
         TO_ROOM
      );
      send_to_char
      (
         "The Dread-Lord shrouds your body in a cloak of Oblivion.\n\r",
         ch
      );
      return;
   }
   if
   (
      ch->pcdata->brand_rank > 2 &&
      stristr
      (
         argument,
         "One must find Oblivion within"
      ) != NULL
   )
   {
      paf = new_affect();
      paf->where = TO_AFFECTS;
      paf->level = ch->level;
      paf->duration = 12;
      paf->type = gsn_will_oblivion;
      affect_to_char(ch, paf);
      free_affect(paf);
      act
      (
         "$n focuses $s mind inward as the runic markings along $s body begin"
         " to pulse with power.",
         ch,
         NULL,
         NULL,
         TO_ROOM
      );
      send_to_char
      (
         "You put your mind to the task of attaining Oblivion within and feel"
         " the Dread-Lords blessing flow through you.\n\r",
         ch
      );
      return;
   }
}

bool do_mob_pain(CHAR_DATA* ch, CHAR_DATA* victim, bool* take)
{
   AFFECT_DATA* paf;
   AFFECT_DATA* paf_2;
   sh_int count_pain = 0;
   CHAR_DATA* vch;
   CHAR_DATA* vch_next;
   sh_int percent;
   bool found = FALSE;
   int dam;

   affect_strip(victim, gsn_mob_pain);
   paf = new_affect();
   paf_2 = new_affect();
   paf->type = gsn_corrupt("freedom", &gsn_mob_timer);
   paf->duration = 12;
   paf_2->type = gsn_mob_pain;
   for (vch = ch->in_room->people; vch; vch = vch_next)
   {
      vch_next = vch->next_in_room;
      /* Skip house mobs */
      if
      (
         !IS_NPC(vch) ||
         !IS_AFFECTED(vch, AFF_CHARM) ||
         vch->master != victim ||
         vch->pIndexData->vnum == MOB_VNUM_ENFORCER_GUARD ||
         vch->pIndexData->vnum == MOB_VNUM_COVENANT_AUXILIARY ||
         vch->pIndexData->vnum == MOB_VNUM_COVENANT_NIGHTMARE ||
         vch->pIndexData->vnum == MOB_VNUM_COVENANT_DRAGON ||
         vch->pIndexData->vnum == MOB_VNUM_UNICORN ||
         vch->pIndexData->vnum == MOB_VNUM_DRAGON ||
         vch->pIndexData->vnum == MOB_VNUM_OUTLAW_GIANT ||
         vch->pIndexData->vnum == MOB_VNUM_STEED ||
         vch->pIndexData->vnum == MOB_VNUM_STEED_PEGASUS ||
         vch->pIndexData->vnum == MOB_VNUM_ABYSS_DEMON ||
         vch->pIndexData->vnum == MOB_VNUM_GUARDIAN_ANGEL
      )
      {
         continue;
      }
      percent = number_percent();
      if (percent <= 10)
      {
         if
         (
            *take ||
            (
               IS_GOOD(ch) &&
               IS_EVIL(vch)
            ) ||
            (
               IS_GOOD(vch) &&
               IS_EVIL(ch)
            )
         )
         {
            /*
            Do nothing, already took,
            or alignment conflicts
            */
         }
         else
         {
            *take = TRUE;
            found = TRUE;
            act
            (
               "$n begans chanting with the mob, breaking away from your"
               " control.",
               vch,
               NULL,
               victim,
               TO_VICT
            );
            act
            (
               "$n begans chanting with the mob, breaking away from $N's"
               " control.",
               vch,
               NULL,
               victim,
               TO_NOTVICT
            );
            do_yell(vch, "I demand my freedom!");
            act
            (
               "$n screams in defiance, throwing off the mantle of oppression"
               " and rising to fight!",
               vch,
               NULL,
               NULL,
               TO_ROOM
            );
            stop_follower(vch);
            SET_BIT(vch->affected_by, AFF_CHARM);
            add_follower(vch, ch);
            vch->leader = ch;
            affect_to_char(vch, paf);
            /* STEAL EMOTE */
         }
         continue;
      }
      if (percent <= 30)
      {
         found = TRUE;
         stop_follower(vch);
         /* uncharm emote */
         act
         (
            "$n begans chanting with the mob, breaking away from your control.",
            vch,
            NULL,
            victim,
            TO_VICT
         );
         act
         (
            "$n begans chanting with the mob, breaking away from $N's control.",
            vch,
            NULL,
            victim,
            TO_NOTVICT
         );
         do_yell(vch, "I demand my freedom!");
         continue;
      }
      affect_to_char(vch, paf_2);
      count_pain++;
      found = TRUE;
   }
   free_affect(paf);
   free_affect(paf_2);
   if (!count_pain)
   {
      return found;
   }
   for (vch = ch->in_room->people; vch; vch = vch_next)
   {
      vch_next = vch->next_in_room;
      if
      (
         !IS_NPC(vch) ||
         !IS_AFFECTED(vch, AFF_CHARM) ||
         vch->master != victim ||
         !is_affected(vch, gsn_mob_pain)
      )
      {
         continue;
      }
      affect_strip(vch, gsn_mob_pain);
      /* damage emote */
      act
      (
         "$n screams in defiance, and lunges madly at you!",
         vch,
         NULL,
         victim,
         TO_VICT
      );
      act
      (
         "$n screams in defiance, and lunges madly at $N!",
         vch,
         NULL,
         victim,
         TO_NOTVICT
      );
   }
   dam = 75 + (25 * (count_pain - 1));
   /* Emote for damage if any */
   damage(victim, victim, dam, gsn_mob_pain, DAM_OTHER, MOB_DAM_TYPE);
   return TRUE;
}

void obj_say_mob(CHAR_DATA* ch, char* argument)
{
   AFFECT_DATA* paf;
   bool found = FALSE;
   bool take = FALSE;
   CHAR_DATA* vch;
   CHAR_DATA* vch_next;
   OBJ_DATA*  brand;

   if
   (
      IS_NPC(ch) ||
      is_affected(ch, gsn_corrupt("freedom", &gsn_mob_timer))
   )
   {
      return;
   }
   if (!check_room(ch, TRUE, "Purge in obj_say_mob"))
   {
      return;
   }
   if
   (
      stristr
      (
         argument,
         "Live for Freedom"
      ) == NULL
   )
   {
      return;
   }
   act
   (
      "$n's brand flashes briefly, as the sound of an angry mob fills your ears.",
      ch,
      NULL,
      NULL,
      TO_ROOM
   );
   send_to_char
   (
      "Your brand flashes briefly, as the sound of an angry mob fills your"
      " ears.\n\r",
      ch
   );
   paf = new_affect();
   paf->type = gsn_mob_pain;
   for (vch = char_list; vch; vch = vch_next)
   {
      vch_next = vch->next_in_room;
      if
      (
         !IS_NPC(vch) ||
         !IS_AFFECTED(vch, AFF_CHARM) ||
         vch->master != ch ||
         !is_affected(vch, gsn_corrupt("freedom", &gsn_mob_timer))
      )
      {
         continue;
      }
      take = TRUE;
      break;
   }
   /* put an affect on all victims */
   for (vch = ch->in_room->people; vch; vch = vch_next)
   {
      vch_next = vch->next_in_room;
      if
      (
         IS_NPC(vch) ||
         ch == vch ||
         (
            (
               brand = get_eq_char(vch, WEAR_BRAND)
            ) != NULL &&
            brand->pIndexData->vnum == OBJ_VNUM_MOB_BRAND
         ) ||
         is_same_group(ch, vch) ||
         (
            ch->id != 886532190 &&  /* Malignus can hit out of pk */
            is_safe
            (
               ch,
               vch,
               IS_SAFE_SILENT |
               IS_SAFE_AREA_ATTACK |
               IS_SAFE_IGNORE_AFFECTS
            )
         )
      )
      {
         continue;
      }
      affect_to_char(vch, paf);
   }
   for (vch = ch->in_room->people; vch; vch = vch_next)
   {
      vch_next = vch->next_in_room;
      if
      (
         IS_NPC(vch) ||
         ch == vch ||
         !is_affected(vch, gsn_mob_pain)
      )
      {
         continue;
      }
      if (do_mob_pain(ch, vch, &take))
      {
         found = TRUE;
         /*
         Doing this to someone can kill multiple people,
         need to start the loop over.
         */
         vch_next = ch->in_room->people;
      }
   }
   if (found)
   {
      free_affect(paf);
      paf = new_affect();
      paf->duration = 12;
      paf->type = gsn_corrupt("freedom", &gsn_mob_timer);
      affect_to_char(ch, paf);
   }
   free_affect(paf);
   return;
}

void obj_say_26309(CHAR_DATA* ch, char* argument)
{
   AFFECT_DATA af;
   OBJ_DATA* obj2 = NULL;
   CHAR_DATA* victim;
   int which = 0;

   if
   (
      !stristr(argument, "persistance shatters") &&
      !stristr(argument, "patience shatters")
   )
   {
      return;
   }

   if (is_affected(ch, gsn_anvil_brand))
   {
      send_to_char("You have already called upon Gryleth.\n\r", ch);
      return;
   }
   victim = ch->fighting;
   if (victim == NULL)
   {
      send_to_char("You are not fighting anyone!\n\r", ch);
      return;
   }
   if (number_range(1, 2) == 1)
   {
      obj2 = get_eq_char(victim, WEAR_WIELD);
      which = 0;
   }
   if (obj2 == NULL)
   {
      obj2 = get_eq_char(victim, WEAR_DUAL_WIELD);
      which = 0;
   }
   if (obj2 == NULL)
   {
      obj2 = get_eq_char(victim, WEAR_SHIELD);
      which = 1;
   }
   if (obj2 == NULL)
   {
      send_to_char("They are not wearing anything to shatter.\n\r", ch);
      return;
   }
   if (which == 0)
   {
      send_to_char
      (
         "You call upon Gryleth to shatter the weapon of your foe.\n\r",
         ch
      );
   }
   else
   {
      send_to_char
      (
         "You call upon Gryleth to shatter the shield of your foe.\n\r",
         ch
      );
   }

   send_to_char("Your anvil glows hot with fiery intensity!\n\r", ch);
   act("$n's anvil glows hot with fiery intensity.", ch, NULL, NULL, TO_ROOM);

   if (which == 0)
   {
      act("$N's weapon shatters into dust!", ch, NULL, victim, TO_NOTVICT);
      act("You shatter $N's weapon into dust!", ch, NULL, victim, TO_CHAR);
      act("Your weapon glows hot and shatters!", victim, NULL, NULL, TO_CHAR);
   }
   else
   {
      act("$N's shield shatters into dust!", ch, NULL, victim, TO_NOTVICT);
      act("You shatter $N's shield into dust!", ch, NULL, victim, TO_CHAR);
      act("Your shield glows hot and shatters!", victim, NULL, NULL, TO_CHAR);
   }

   extract_obj(obj2, FALSE);
   af.where     = TO_AFFECTS;
   af.location  = APPLY_NONE;
   af.modifier  = 0;
   af.type      = gsn_anvil_brand;
   af.level     = ch->level;
   af.duration  = 24;
   af.bitvector = 0;
   affect_to_char(ch, &af);
   return;
}

void obj_say_17641_2(CHAR_DATA* ch, char* argument)
{
   int sn;
   AFFECT_DATA af;

   if (!stristr(argument, "blessing of justice"))
   {
      return;
   }

   if (is_affected(ch, gsn_justice_brand2))
   {
      send_to_char("You have already feel the blessing of justice.\n\r", ch);
      return;
   }
   send_to_char
   (
      "You call upon Malistien to show you favor for your just ways.\n\r"
      "Your bronze scales tilt a bit to the side of justice!\n\r",
      ch
   );
   act
   (
      "$n's bronze scales tilt to the side of justice.",
      ch,
      NULL,
      NULL,
      TO_ROOM
   );

   sn           = gsn_justice_brand2;
   af.where     = TO_AFFECTS;
   af.location  = 0;
   af.modifier  = 0;
   af.type      = sn;
   af.level     = ch->level;
   af.duration  = 15;
   af.bitvector = 0;
   affect_to_char(ch, &af);
   return;
}

void obj_say_17641(CHAR_DATA* ch, char* argument)
{
   int sn;
   AFFECT_DATA af;

   if (!stristr(argument, "wrath of justice"))
   {
      return;
   }

   if (is_affected(ch, gsn_justice_brand))
   {
      send_to_char("You have already called upon justice.\n\r", ch);
      return;
   }
   if (ch->fighting == NULL)
   {
      send_to_char("Your prayers are not understood.\n\r", ch);
      return;
   }
   send_to_char
   (
      "You call upon Malistien to show your foes the wrath of justice.\n\r"
      "Your bronze scales tilt a bit to the side of justice!\n\r",
      ch
   );
   act
   (
      "$n's bronze scales tilt to the side of justice.",
      ch,
      NULL,
      NULL,
      TO_ROOM
   );

   sn           = gsn_justice_brand;
   af.where     = TO_AFFECTS;
   af.location  = APPLY_NONE;
   af.modifier  = 0;
   af.type      = sn;
   af.level     = ch->level;
   af.duration  = 24;
   af.bitvector = 0;
   affect_to_char(ch, &af);

   af.type = gsn_justice_brand_wrath;
   af.duration  = 3;
   affect_to_char(ch->fighting, &af);
   send_to_char("The wrath of justice is upon you!\n\r", ch->fighting);
   return;
}

void obj_say_28903(CHAR_DATA* ch, char* argument, OBJ_DATA* obj)
{
   AFFECT_DATA af;

   if (!stristr(argument, "My secrets are mine alone!"))
   {
      return;
   }

   send_to_char("You focus on your devotion to the Lord of Worms...\n\r", ch);

   if (!is_name("level3", obj->name))
   {
      send_to_char
      (
         "You have not yet proven yourself worthy.\n\r"
         "Your waning crescent brand glows dimly, then fades.\n\r",
         ch
      );
      act
      (
         "$n's waning crescent brand glows dimly, then fades.",
         ch,
         NULL,
         NULL,
         TO_ROOM
      );
      return;
   }

   send_to_char
   (
      "Your waning crescent brand flares a brilliant amber, then fades!\n\r"
      "He grants you protection from the prying of others!\n\r",
      ch
   );
   act
   (
      "$n's waning crescent brand flares a brilliant amber, then fades.",
      ch,
      NULL,
      NULL,
      TO_ROOM
   );

   af.where     = TO_AFFECTS;
   af.location  = APPLY_NONE;
   af.modifier  = 0;
   af.type      = gsn_worm_hide;
   af.level     = ch->level;
   af.duration  = 6;
   af.bitvector = AFF_SNEAK;
   affect_strip(ch, gsn_worm_hide);  /* prevent multiple affects */
   affect_to_char(ch, &af);
   return;
}

void obj_say_28211(CHAR_DATA* ch, char* argument, OBJ_DATA* obj)
{
   AFFECT_DATA af;

   if (!stristr(argument, "Lord Dielantha, Grant me the armor of the Silver Dragon!"))
   {
      return;
   }
   if (is_affected(ch, gsn_sdragon_armor))
   {
      send_to_char("Your brand shimmers briefly.\n\r", ch);
      return;
   }
   af.where     = TO_AFFECTS;
   af.location  = APPLY_AC;
   af.modifier  = 0 - (ch->level);
   af.type      = gsn_sdragon_armor;
   af.level     = ch->level;
   af.duration  = 10;
   af.bitvector = 0;
   affect_to_char(ch, &af);
   act
   (
      "Your $p flashes brilliantly!\n\r"
      "Shimmering silver scales cover your body!",
      ch,
      obj,
      NULL,
      TO_CHAR
   );
   act
   (
      "$n's silver dragon brand flashes brilliantly, and silver scales cover"
      " their body!",
      ch,
      obj,
      NULL,
      TO_ROOM
   );
   return;
}

void obj_say_12500_2(CHAR_DATA* ch, char* argument, OBJ_DATA* obj)
{
   ROOM_INDEX_DATA* room;
   ROOM_INDEX_DATA* target;

   if (!stristr(argument, "grant me wisdom"))
   {
      return;
   }
   if (ch->mana < 50)
   {
      send_to_char("The Owl remains cold.\n\r", ch);
      return;
   }
   if (!IS_NPC(ch) && ch->pcdata->brand_rank < 0)
   {
      send_to_char("Wervdon will not answer your prayers.\n\r", ch);
      return;
   }

   ch->mana = ch->mana - 50;  /* costs 50 mana for this one */

   act
   (
      "The Owl upon $n's brand glows brightly.",
      ch,
      obj,
      NULL,
      TO_ROOM
   );
   send_to_char
   (
      "The Owl grows warm.\n\r"
      "You are granted the wisdom to see the lands cities.\n\r",
      ch
   );

   room = ch->in_room;
   char_from_room(ch);
   target = get_room_index(10601);
   if (target)
   {
      send_to_char("\n\rThe Owl shows you Glyndane.\n\r", ch);
      char_to_room_1(ch, target, TO_ROOM_AT);
      brand_where(ch);
      char_from_room(ch);
   }

   target = get_room_index(601);
   if (target)
   {
      send_to_char("\n\rOfcol...\n\r", ch);
      char_to_room_1(ch, target, TO_ROOM_AT);
      brand_where(ch);
      char_from_room(ch);
   }

   target = get_room_index(8807);
   if (target)
   {
      send_to_char("\n\r", ch);
      send_to_char("Elvenhame...\n\r", ch);
      char_to_room_1(ch, target, TO_ROOM_AT);
      brand_where(ch);
      char_from_room(ch);
   }

   /* target = get_room_index(9525);
   if (target)
   {
      send_to_char("\n\rNew Thalos...\n\r", ch);
      char_to_room_1(ch, target, TO_ROOM_AT);
      brand_where(ch);
      char_from_room(ch);
   } */

   target = get_room_index(70151);
   if (target)
   {
      send_to_char("\n\rTyr-Zinet...\n\r", ch);
      char_to_room_1(ch, target, TO_ROOM_AT);
      brand_where(ch);
      char_from_room(ch);
   }

   target = get_room_index(711);
   if (target)
   {
      send_to_char("\n\rArkham...\n\r", ch);
      char_to_room_1(ch, target, TO_ROOM_AT);
      brand_where(ch);
      char_from_room(ch);
   }

   target = get_room_index(6827);
   if (target)
   {
      send_to_char("\n\rSheqeston...\n\r", ch);
      char_to_room_1(ch, target, TO_ROOM_AT);
      brand_where(ch);
      char_from_room(ch);
   }

   target = get_room_index(27701);
   if (target)
   {
      send_to_char("\n\rDhaes Drae Aesilon...\n\r", ch);
      char_to_room_1(ch, target, TO_ROOM_AT);
      brand_where(ch);
      char_from_room(ch);
   }

   target = get_room_index(3014);
   if (target)
   {
      send_to_char("\n\rEthshar...\n\r", ch);
      char_to_room_1(ch, target, TO_ROOM_AT);
      brand_where(ch);
      char_from_room(ch);
   }

   target = get_room_index(8243);
   if (target)
   {
      send_to_char("\n\rand Daltigoth...\n\r", ch);
      char_to_room_1(ch, target, TO_ROOM_AT);
      brand_where(ch);
      char_from_room(ch);
   }

   send_to_char("\n\r", ch);

   char_to_room_1(ch, room, TO_ROOM_AT);
   /*
   af.where     = TO_AFFECTS;
   af.location  = APPLY_SAVING_SPELL;
   af.modifier  = 2;
   af.type      = gsn_owl_brand;
   af.level     = ch->level;
   af.bitvector = 0;
   af.duration  = 3;
   affect_to_char(ch, &af);
   */
   return;
}

/* function to perform the specialized where's for my brand- wervdon */
void brand_where(CHAR_DATA* ch)
{
   char buf[MAX_STRING_LENGTH];
   bool found;
   CHAR_DATA* victim;
   DESCRIPTOR_DATA* d;

   found = FALSE;
   for (d = descriptor_list; d; d = d->next)
   {
      if
      (
         d->connected == CON_PLAYING &&
         (victim = d->character) != NULL &&
         !IS_NPC(victim) &&
         victim->in_room != NULL &&
         !IS_SET(victim->in_room->room_flags, ROOM_NOWHERE) &&
         (
            is_room_owner(ch, victim->in_room) ||
            !room_is_private(victim->in_room)
         ) &&
         victim->in_room->area == ch->in_room->area &&
         !IS_IMMORTAL(victim) &&
         victim != ch &&
         (
            /* 25% chance of failing to see people you dont normally */
            can_see(ch, victim) ||
            number_percent() > 75
         )
      )
      {
         found = TRUE;
         if (is_affected(victim, gsn_cloak_form) && !IS_TRUSTED(ch, ANGEL))
         {
            sprintf
            (
               buf,
               "%s%-28s %s\n\r",
               is_in_pk(ch, victim) ?
               "(PK) " :
               "",
               "cloaked figure",
               victim->in_room->name
            );
         }
         else
         {
            sprintf
            (
               buf,
               "%s%-28s %s\n\r",
               is_in_pk(ch, victim) ?
               "(PK) " :
               "",
               victim->name,
               victim->in_room->name
            );
         }
         send_to_char(buf, ch);
      }
   }
   if (!found)
   {
      send_to_char("No one appears to be in this city.\n\r", ch);
   }
   return;
}

void obj_say_traitor(CHAR_DATA* ch, char* argument)
{
   CHAR_DATA* victim;
   OBJ_DATA*  obj;

   if (!stristr(argument, "traitor"))
   {
      return;
   }

   for (victim = ch->in_room->people; victim != NULL; victim =victim->next_in_room)
   {
      obj = get_eq_char(victim, WEAR_SHIELD);
      if (obj != NULL && obj->pIndexData->vnum == OBJ_VNUM_TRAITOR_SHIELD)
      {
         break;
      }
   }
   if (victim == NULL)
   {
      return;
   }

   if (victim->daze != 0)
   {
      return;
   }

   send_to_char("Your ears burn as you hear the dreaded word!\n\r", victim);
   act("$n is stunned by the word!", victim, NULL, NULL, TO_ROOM);
   act
   (
      "$E must not like that word.\n\r",
      ch,
      NULL,
      victim,
      TO_CHAR
   );

   DAZE_STATE(victim, 60);
   return;
}

void obj_say_12500(CHAR_DATA* ch, char* argument, OBJ_DATA* obj)
{
   AFFECT_DATA af;

   if (!stristr(argument, "order is essential to society"))
   {
      return;
   }
   if (is_affected(ch, gsn_owl_brand))
   {
      send_to_char("The Owl remains cold.\n\r", ch);
      return;
   }
   if (!IS_NPC(ch) && ch->pcdata->brand_rank < 0)
   {
      send_to_char("Wervdon will not answer your prayers.\n\r", ch);
      return;
   }
   act("The Owl upon $n's brand glows brightly.", ch, obj, NULL, TO_ROOM);
   send_to_char
   (
      "The Owl glows brightly.\n\r"
      "Your mind feels more orderly.\n\r",
      ch
   );
   af.where     = TO_AFFECTS;
   af.location  = APPLY_SAVING_SPELL;
   af.modifier  = -2;
   af.type      = gsn_owl_brand;
   af.level     = ch->level;
   af.bitvector = 0;
   af.duration  = 11;
   affect_to_char(ch, &af);
   af.duration  = 9;
   affect_to_char(ch, &af);
   af.duration  = 5;
   affect_to_char(ch, &af);
   af.duration  = 1;
   affect_to_char(ch, &af);
   return;
}

void obj_say_1888(CHAR_DATA* ch, char* argument, OBJ_DATA* obj)
{
   ROOM_INDEX_DATA* pRoomIndex;
   CHAR_DATA* vch;
   AFFECT_DATA af;
   int chaosact;

   if (IS_NPC(ch))
   {
      return;
   }

   if (!stristr(argument, "chaos"))
   {
      return;
   }

   WAIT_STATE(ch, 60);

   chaosact = number_range(1, 100);

   /* User teleports */
   if ((chaosact >= 1) && (chaosact <= 5))
   {
      if ((pRoomIndex = get_random_room(ch, RANDOM_NORMAL)) == NULL)
      {
         bug("NULL room from get_random_room.", 0);
         return;
      }

      send_to_char("You have been whisked away on the winds of chaos!\n\r", ch);

      act("$n is whisked away on the winds of chaos!", ch, NULL, NULL, TO_ROOM);
      char_from_room(ch);
      char_to_room(ch, pRoomIndex);
      act("The winds of chaos blow through, depositing $n.", ch, NULL, NULL, TO_ROOM);
      do_observe(ch, "", LOOK_AUTO);

      return;
   }

   /* Spring created */
   if ((chaosact >= 6) && (chaosact <= 10))
   {
      obj_cast_spell(gsn_create_spring, 60, ch, ch, obj);
      return;
   }

   /* Mushroom created */
   if ((chaosact >= 11) && (chaosact <= 15))
   {
      obj_cast_spell(gsn_create_food, 60, ch, ch, obj);
      return;
   }

   /* User healed */
   if ((chaosact >= 16) && (chaosact <= 18))
   {
      obj_cast_spell(gsn_mass_healing, 60, ch, ch, obj);
      return;
   }

   /* Force all in room to report */
   if ((chaosact >= 19) && (chaosact <= 20))
   {
      for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
      {
         if (IS_IMMORTAL(vch))
         {
            continue;
         }
         do_report(vch, "");
      }
      return;
   }

   /* Force all in room to yell "Chaos rules!" */
   if ((chaosact >= 21) && (chaosact <= 23))
   {
      for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
      {
         if (IS_IMMORTAL(vch))
         {
            continue;
         }
         do_yell(vch, "-<<-<<-CHAOS RULES!!!->>->>-");
      }
      return;
   }

   /* Timestop */
   if ((chaosact >= 24) && (chaosact <= 25))
   {
      obj_cast_spell(gsn_timestop, 60, ch, ch, obj);
      return;
   }

   /* Room sleep (1 tick) */
   if ((chaosact >= 26) && (chaosact <= 30))
   {

      af.where     = TO_AFFECTS;
      af.type      = gsn_sleep;
      af.level     = 60;
      af.duration  = 0;
      af.location  = APPLY_NONE;
      af.modifier  = 0;
      af.bitvector = AFF_SLEEP;

      for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
      {
         if (IS_IMMORTAL(vch) || !IS_AWAKE(vch))
         {
            continue;
         }
         send_to_char
         (
            "Hey, who turned out the lights?\n\rI need another mmfmmfmmmm..."
            " Zzzzzz...\n\r",
            vch
         );
         act
         (
            "$n goes to sleep.",
            vch,
            NULL,
            NULL,
            TO_ROOM
         );
         vch->position = POS_SLEEPING;
         affect_join(vch, &af, -1);
         if
         (
            !IS_NPC(ch) &&
            !IS_IMMORTAL(ch) &&
            !IS_IMMORTAL(vch) &&
            (
               !IS_NPC(vch) ||
               IS_AFFECTED(vch, AFF_CHARM)
            )
         )
         {
            QUIT_STATE(ch, 20);
            QUIT_STATE(vch, 20);
         }
      }
      return;
   }

   /* User suckered 500 hp */
   if ((chaosact >= 31) && (chaosact <= 35))
   {
      damage(ch, ch, 500, gsn_acid_spit, DAM_ACID, TRUE);
      return;
   }

   /* Chaos wind */
   if ((chaosact >= 36) && (chaosact <= 40))
   {
      act("The winds of chaos burst into the room!", ch, NULL, NULL, TO_ALL);
      obj_cast_spell(gsn_windwall, 60, ch, ch, obj);
      return;
   }

   /* Area shielding (ice) */
   if ((chaosact >= 41) && (chaosact <= 42))
   {
      for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
      {
         if (IS_IMMORTAL(vch))
         {
            continue;
         }
         obj_cast_spell(gsn_iceshield, 60, vch, vch, obj);
      }
      return;
   }

   /* Area shielding (fire) */
   if ((chaosact >= 43) && (chaosact <= 44))
   {
      for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
      {
         if (IS_IMMORTAL(vch))
         {
            continue;
         }
         obj_cast_spell(gsn_fireshield, 60, vch, vch, obj);
      }
      return;
   }

   /* PK area teleport */
   if ((chaosact >= 45) && (chaosact <= 46))
   {
      for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
      {
         if (IS_IMMORTAL(vch))
         {
            continue;
         }
         obj_cast_spell(gsn_teleport, 60, ch, vch, obj);
      }
      return;
   }

   /* Area aggressive to user */
   if ((chaosact >= 47) && (chaosact <= 48))
   {
      CHAR_DATA* vch;
      send_to_char
      (
         "Do you ever get that feeling where the whole world is against you?"
         "\n\r",
         ch
      );
      do_myell(ch, "Can't we all just get along?!");
      for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
      {
         if (IS_IMMORTAL(vch) || ch == vch)
         {
            continue;
         }
         do_yell(vch, "Die!");
         do_murder(vch, ch->name);
      }
      return;
   }

   /* Nada */
   send_to_char("Silence meets your question.\n\r", ch);
   return;
}

void obj_say_2109(CHAR_DATA* ch, char* argument, OBJ_DATA* obj)
{
   AFFECT_DATA af;
   CHAR_DATA* vch;

   if (!stristr(argument, "peace brings joy"))
   {
      return;
   }

   if (is_affected(ch, gsn_peace_brand))
   {
      send_to_char("You are reaffirmed in your belief.\n\r", ch);
      return;
   }

   af.where     = TO_AFFECTS;
   af.location  = 0;
   af.modifier  = MODIFY_PEACE_BY_CIRDAN_BRAND;
   af.type      = gsn_peace_brand;
   af.level     = ch->level;
   af.duration  = 20;
   af.bitvector = 0;
   affect_to_char(ch, &af);
   act
   (
      "Your Mystical Blue Flame flares blindingly!",
      ch,
      obj,
      NULL,
      TO_CHAR
   );
   act
   (
      "$n's Mystical Blue Flame flares blindingly!",
      ch,
      obj,
      NULL,
      TO_ROOM
   );
   af.type      = gsn_peace;
   af.duration  = 1;
   for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
   {
      if (!is_affected(vch, gsn_peace))
      {
         affect_to_char(vch, &af);
         if (vch->fighting != NULL)
         {
            stop_fighting(vch, TRUE);
         }
         send_to_char
         (
            "Your heart is warmed by the light of the Eternal Flame.\n\r",
            vch
         );
      }
   }
   return;
}

void obj_say_430(CHAR_DATA* ch, char* argument, OBJ_DATA* obj)
{
   AFFECT_DATA af;
   OBJ_DATA* bobj, *obj_next;

   if (!stristr(argument, "magic is forever"))
   {
      return;
   }

   if
   (
      stristr(argument, "i don't believe magic is forever") ||
      stristr(argument, "i do not believe magic is forever") ||
      stristr(argument, "i do not believe that magic is forever") ||
      stristr(argument, "i don't believe that magic is forever") ||
      stristr(argument, "i do not think that magic is forever") ||
      stristr(argument, "i don't think that magic is forever") ||
      stristr(argument, "i do not think magic is forever") ||
      stristr(argument, "i don't think magic is forever")
   )
   {
      if (IS_AFFECTED(ch, AFF_CHARM))
      {
         /*
         No forced destruction,
         but should not get brand power for saying this
         */
         return;
      }
      act
      (
         "$n's equipment vaporises in a cloud of blue-green dust!",
         ch,
         NULL,
         NULL,
         TO_ROOM
      );
      send_to_char
      (
         "Your equipment vaporises in a cloud of blue-green dust!\n\r",
         ch
      );
      for (bobj = ch->carrying; bobj != NULL; bobj = obj_next)
      {
         obj_next = bobj->next_content;
         if (bobj->wear_loc != WEAR_BRAND)
         {
            act("$p melts away and turns to dust!", ch, bobj, NULL, TO_ALL);
            extract_obj(bobj, FALSE);
         }
      }
      return;
   }

   if (is_affected(ch, gsn_lemniscate_brand))
   {
      act
      (
         "$p glows briefly but nothing happens to you.",
         ch,
         obj,
         NULL,
         TO_CHAR
      );
      act
      (
         "$n's $p glows brief but nothing seems to happen.",
         ch,
         obj,
         NULL,
         TO_ROOM
      );
      return;
   }
   af.where     = TO_AFFECTS;
   af.location  = 0;
   af.modifier  = -1 - (ch->level >= 18) - (ch->level >= 25) - (ch->level >= 32);
   af.type      = gsn_lemniscate_brand;
   af.level     = ch->level;
   af.duration  = 3;
   af.bitvector = 0;
   affect_to_char(ch, &af);
   act
   (
      "Your $p glows briefly.\n\r"
      "You feel yourself moving slower.",
      ch,
      obj,
      NULL,
      TO_CHAR
   );
   act
   (
      "$n's $p glows briefly.\n\r"
      "$n starts to move more slowly.",
      ch,
      obj,
      NULL,
      TO_ROOM
   );
   return;
}

void obj_say_15888(CHAR_DATA* ch, char* argument, OBJ_DATA* obj)
{
   AFFECT_DATA af;

   if (stristr(argument, "Lord Riallus, grant me sight beyond sight."))
   {
      if
      (
         IS_AFFECTED(ch, AFF_DETECT_EVIL) ||
         is_affected(ch, gsn_detect_evil) ||
         IS_AFFECTED(ch, AFF_DETECT_GOOD) ||
         is_affected(ch, gsn_detect_good)
      )
      {
         act
         (
            "$p begins to flare...but nothing happens to you.",
            ch,
            obj,
            NULL,
            TO_CHAR
         );
         act
         (
            "$n's $p begins to flare but dims suddenly.",
            ch,
            obj,
            NULL,
            TO_ROOM
         );
         return;
      }
      af.where     = TO_AFFECTS;
      af.location  = 0;
      af.modifier  = 0;
      af.type      = gsn_detect_good;
      af.level     = ch->level;
      af.duration  = 24;
      af.bitvector = AFF_DETECT_GOOD;
      affect_to_char(ch, &af);
      af.type      = gsn_detect_evil;
      af.bitvector = AFF_DETECT_EVIL;
      affect_to_char(ch, &af);
      act
      (
         "The brand of the Phoenix sears your skin as it works its magic.",
         ch,
         obj,
         NULL,
         TO_CHAR
      );
      act("$n's brand flares blindingly!", ch, obj, NULL, TO_ROOM);
      return;
   }
   if (stristr(argument, "I call upon the awesome power of the Phoenix!"))
   {
      if
      (
         is_affected(ch, gsn_corrupt("phoenix brand", &gsn_phoenix)) ||
         IS_SET(ch->in_room->extra_room_flags, ROOM_1212)
      )
      {
         send_to_char("The brand of the Phoenix remains cold.\n\r", ch);
         return;
      }
      af.where     = TO_AFFECTS;
      af.location  = 0;
      af.modifier  = 0;
      af.type      = gsn_corrupt("phoenix brand", &gsn_phoenix);
      af.level     = ch->level;
      af.duration  = 24;
      af.bitvector = 0;
      affect_to_char(ch, &af);
      ch->mana     = 0;
      ch->move     = 0;
      ch->hit      = ch->hit * 3 / 4;
      act
      (
         "$n's brand of the Phoenix bursts into flame!!\n\r$n calls upon the"
         " power of the Phoenix and vanishes in a great"" conflagration!!",
         ch,
         NULL,
         NULL,
         TO_ROOM
      );
      send_to_char
      (
         "Your brand of the Phoenix bursts into flame!!\n\r"
         "You feel drained but safe.\n\r",
         ch
      );
      if (ch->fighting)
      {
         stop_fighting(ch, TRUE);
      }
      char_from_room(ch);
      char_to_room(ch, get_room_index(15890));
      do_observe(ch, "", LOOK_AUTO);
      return;
   }
   return;
}

void obj_say_15901(CHAR_DATA* ch, char* argument, OBJ_DATA* obj)
{
   /* Do nothing. */
   return;
}

void obj_say_temporal(CHAR_DATA* ch, char* argument, OBJ_DATA* obj)
{
   int room = 0;
   ROOM_INDEX_DATA* proom = NULL;
   int tries = 0;

   if (obj == NULL) /* Sanity check */
   {
      return;
   }

   if (stristr(argument, "look not back and regret"))
   {
      if
      (
         ch->in_room &&
         IS_SET(ch->in_room->extra_room_flags, ROOM_1212)
      )
      {
         send_to_char
         (
            "Bad idea...\n\rYour Amulet of Temporal Distortion vaporizes!\n\r",
            ch
         );
         extract_obj(obj, FALSE);
         return;
      }
      if (ch->in_room->vnum >= 1035 && ch->in_room->vnum <= 1050)
      {
         send_to_char("Your amulet remains cold.\n\r", ch);
         return;
      }

      if ((ch->hit < 300) || (ch->mana < 300) || (ch->move < 100))
      {
         send_to_char("Your Amulet of Temporal Distortion remains cold.\n\r", ch);
         return;
      }
      while (!proom && tries < 10000)
      {
         switch (number_range(1, 3))
         {
            default: /* 1 */
            {
               room = number_range(15700, 15899);
               break;
            }
            case (2):
            {
               room = number_range(6400, 6469);
               break;
            }
            case (3):
            {
               room = number_range(5300, 5399);
               break;
            }
         }
         proom = get_room_index(room);
         tries++;
      }
      if (tries >= 10000)
      {
         send_to_char("Your Amulet of Temporal Distortion remains cold.\n\r", ch);
         return;
      }
      if (ch->fighting != NULL)
      {
         stop_fighting(ch, TRUE);
      }
      send_to_char
      (
         "Your Amulet of Temporal Distortion grows warm.\n\r"
         "You are surrounded by a cloud of blue-green sparkles.\n\r",
         ch
      );
      act
      (
         "$n vanishes in a cloud of blue-green sparkles.",
         ch,
         NULL,
         NULL,
         TO_ROOM
      );
      ch->hit  /= 2;
      ch->mana /= 2;
      ch->move /= 2;
      char_from_room(ch);
      char_to_room(ch, proom);
      act
      (
         "A cloud of blue-green sparkles materializes, and $n steps forth.",
         ch,
         NULL,
         NULL,
         TO_ROOM
      );
      do_observe(ch, "", LOOK_AUTO);
      return;
   }

   if (stristr(argument, "the future looks promising"))
   {
      if
      (
         ch->in_room &&
         IS_SET(ch->in_room->extra_room_flags, ROOM_1212)
      )
      {
         send_to_char
         (
            "Bad idea...\n\rYour Amulet of Temporal Distortion vaporizes!\n\r",
            ch
         );
         extract_obj(obj, FALSE);
         return;
      }

      if (ch->in_room->vnum >= 1035 && ch->in_room->vnum <= 1050)
      {
         send_to_char("Your amulet remains cold.\n\r", ch);
         return;
      }

      if ((ch->hit < 300) || (ch->mana < 300) || (ch->move < 100))
      {
         send_to_char
         (
            "Your Amulet of Temporal Distortion remains cold.\n\r",
            ch
         );
         return;
      }
      while (!proom && tries < 10000)
      {
         room = number_range(27500, 27699);
         proom = get_room_index(room);
         tries++;
      }
      if (tries >= 10000)
      {
         send_to_char
         (
            "Your Amulet of Temporal Distortion remains cold.\n\r",
            ch
         );
         return;
      }
      if (ch->fighting != NULL)
      {
         stop_fighting(ch, TRUE);
      }
      send_to_char
      (
         "Your Amulet of Temporal Distortion grows warm.\n\r"
         "You are surrounded by a cloud of blue-green sparkles.\n\r",
         ch
      );
      act("$n vanishes in a cloud of blue-green sparkles.", ch, NULL, NULL, TO_ROOM);
      ch->hit  /= 2;
      ch->mana /= 2;
      ch->move /= 2;
      char_from_room(ch);
      char_to_room(ch, proom);
      act
      (
         "A cloud of blue-green sparkles materializes, and $n steps forth.",
         ch,
         NULL,
         NULL,
         TO_ROOM
      );
      do_observe(ch, "", LOOK_AUTO);
      return;
   }
   return;
}

void obj_say_27513(CHAR_DATA* ch, char* argument, OBJ_DATA* obj)
{
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   int count;
   int number;
   CHAR_DATA* victim;
   OBJ_DATA* helm;
   int wear_slot;

   if (!stristr(argument, "banish arcane thought"))
   {
      return;
   }

   while (stristr(argument, "casting"))
   {
      argument = one_argument(argument, arg1);
   }

   argument = one_argument(argument, arg1);


   if (arg1[0] == '\0')
   {
      send_to_char
      (
         "The words are said, but nothing happens.\n\r"
         "Perhaps you need to specify a target?\n\r",
         ch
      );
      return;
   }

   arg2[0] = '\0';
   sprintf(arg2, "%s", arg1);
   number = number_argument(arg2, arg1);
   count = 0;

   for (victim = char_list; victim != NULL; victim = victim->next)
   {
      if
      (
         IS_NPC(victim) ||
         victim->in_room == NULL ||
         !is_name(arg1, get_name(victim, ch)) ||
         !can_see(ch, victim) ||
         ++count != number
      )
      {
         continue;
      }
      break;
   }
   if
   (
      victim == NULL ||
      IS_NPC(victim)
   )
   {
      send_to_char("The iron mask does not seem to be drawn anywhere.\n\r", ch);
      return;
   }
   if
   (
      IS_IMMORTAL(victim) &&
      !IS_IMMORTAL(ch)
   )
   {
      send_to_char("The iron mask glows, and your mind is wracked in pain!\n\r", ch);
      damage(ch, ch, ch->hit / 2, gsn_starvation, DAM_MENTAL, TRUE);
      return;
   }

   if (victim == ch)
   {
      send_to_char("Uh, why not just wear it?\n\r", ch);
      return;
   }

   if
   (
      victim->class == CLASS_WARRIOR ||
      victim->class == CLASS_THIEF ||
      victim->class == CLASS_RANGER ||
      victim->class == CLASS_MONK
   )
   {
      send_to_char
      (
         "The mask does not glow brightly for this person.\n\r"
         "It occurs to you that maybe it only works on mages.\n\r",
         ch
      );
      return;
   }
   if (ch->race != grn_minotaur)
   {
      wear_slot = WEAR_HEAD;
   }
   else
   {
      wear_slot = WEAR_HORNS;
   }
   if (obj == get_eq_char(ch, wear_slot))
   {
      unequip_char(ch, obj);
   }
   if (victim->race != grn_minotaur)
   {
      wear_slot = WEAR_HEAD;
   }
   else
   {
      wear_slot = WEAR_HORNS;
   }

   helm = get_eq_char(victim, wear_slot);
   if (helm != NULL)
   {
      if
      (
         obj->pIndexData->vnum == 12161 &&
         is_affected(ch, gsn_blackjack_timer)
      )
      {
         affect_strip(ch, gsn_blackjack_timer);
      }
      unequip_char(victim, helm);
   }
   obj_from_char(obj);
   obj_to_char(obj, victim);
   SET_BIT(obj->extra_flags, ITEM_NOREMOVE);
   equip_char(victim, obj, wear_slot);

   send_to_char("The iron mask glows and vanishes.\n\r", ch);
   send_to_char("Sudden pain clenches your head as an iron mask appears on your head!\n\r", victim);

   return;
}

void obj_say_431(CHAR_DATA* ch, char* argument, OBJ_DATA* obj) {
   AFFECT_DATA af;
   AFFECT_DATA af2;
   int sn;

   sn = gsn_haste;


   if (!strstr(argument, "fury of war"))
   return;

   if (is_affected(ch, gsn_wolverine_brand))
   {
      send_to_char("Your blood is still hot from your last coursing.\n\r", ch);
      return;
   }

   if (IS_AFFECTED(ch, AFF_HASTE))
   {
      act("You feel hot rage build inside you, but nothing happens.",
      ch, obj, 0, TO_CHAR);
      act("$n's $p glows briefly but nothing seems to happen.",
      ch, obj, 0, TO_ROOM);
      return;
   }

   af.where = TO_AFFECTS;
   af.location = APPLY_DEX;
   af.modifier = 3;
   af.type = gsn_wolverine_brand;
   af.level = ch->level;
   af.duration = 7;
   af.bitvector = AFF_HASTE;
   affect_to_char(ch, &af);

   af2.where=TO_AFFECTS;
   af2.location=APPLY_HITROLL;
   af2.modifier=2;
   af2.type = gsn_wolverine_brand;
   af2.level=ch->level;
   af2.duration=20;
   af2.bitvector = 0;
   affect_to_char(ch, &af2);

   af2.location=APPLY_DAMROLL;
   affect_to_char(ch, &af2);
   act("Your $p glows briefly.", ch, obj, NULL, TO_CHAR);
   send_to_char("You are filled with a divine fury!\n\r", ch);
   act("$n's $p glows briefly.", ch, obj, NULL, TO_ROOM);
   act("$n begins to move faster and seethes with anger.", ch, NULL, NULL, TO_ROOM);
   return;
}

void obj_say_14250(CHAR_DATA* ch, char* argument, OBJ_DATA* obj)
{
   AFFECT_DATA af;
   int sn;

   if ((strstr(argument, "Wind of the eagle"))
   || (strstr(argument, "wind of the eagle")))
   {
      if (ch->mana<10)
      {
         send_to_char("You don't have enough mana.\n\r", ch);
         return;
      }
      ch->mana-=10;
      ch->move = UMIN( ch->move+ch->level, ch->max_move);
      send_to_char("You feel an invigorating breeze cross your face.\n\r", ch);
      act("$n's $p glows briefly.", ch, obj, NULL, TO_ROOM);
      return;
   }
   sn = gsn_fly;
   if ((strstr(argument, "Grant me flight, eagle"))
   || (strstr(argument, "grant me flight, eagle")))
   {
      if (ch->mana<20)
      {
         send_to_char("You don't have enough mana.\n\r", ch);
         return;
      }
      ch->mana-=20;
      if (IS_AFFECTED(ch, AFF_FLYING))
      {
         act("You feel the pull of gravity lessen, but nothing happens",
         ch, obj, 0, TO_CHAR);
         act("$n's $p glows briefly but nothing seems to happen.",
         ch, obj, 0, TO_ROOM);
         return;
      }

      af.where = TO_AFFECTS;
      af.location = 0;
      af.modifier = 0;
      af.type = sn;
      af.level = ch->level;
      af.duration = 15;
      af.bitvector = AFF_FLYING;
      affect_to_char(ch, &af);
      act("Your $p glows briefly.", ch, obj, NULL, TO_CHAR);
      send_to_char("You feel yourself break free of the bonds of gravity. \n\r" , ch);
      act("$n's $p glows briefly.", ch, obj, NULL, TO_ROOM);
      act("$n starts to float in mid-air.", ch, NULL, NULL, TO_ROOM);
      return;
   }
   return;
}

void obj_say_14600(CHAR_DATA* ch, char* argument)
{
   AFFECT_DATA af;
   int sn;

   sn = gsn_bless;

   if ((!stristr(argument, "Adorno, bless all life with light")))
   return;

   if (ch->fighting != NULL)
   {
      send_to_char("You can't be fighting and expect to protect life at the same time!\n\r", ch);
      return;
   }

   if (ch->mana < 50)
   {
      send_to_char("Your mana reserves are not high enough to power the brand.\n\r", ch);
      return;
   }
   ch->mana -=50;

   if (is_affected(ch, gsn_plague))
   if (number_percent()>50)
   {
      affect_strip(ch, gsn_plague);
      send_to_char("Adorno's benevolent touch purges the disease from your veins.\n\r", ch);
   }
   if (is_affected(ch, gsn_poison))
   if (number_percent()>50)
   {
      affect_strip(ch, gsn_poison);
      send_to_char("Adorno's benevolent touch purges the poison from your veins.\n\r", ch);
   }
   if (is_affected(ch, gsn_poison_dust))
   if (number_percent()>50)
   {
      affect_strip(ch, gsn_poison_dust);
      send_to_char("Adorno's benevolent touch purges the poison from your veins.\n\r", ch);
   }
   if (is_affected(ch, sn))
   {
      send_to_char("Your brand glows briefly.\n\r", ch);
      act("$n's brand glows briefly.", ch, NULL, NULL, TO_ROOM);
      return;
   }
   af.where = TO_AFFECTS;
   af.type = sn;
   af.level = 51;
   af.duration = (ch->level);
   af.location = APPLY_HITROLL;
   af.modifier = (ch->level)/8;
   af.bitvector = 0;
   affect_to_char(ch, &af);
   af.location = APPLY_SAVING_SPELL;
   af.modifier = 0-(ch->level)/8;
   affect_to_char(ch, &af);
   send_to_char("You feel Adorno's benevolent touch.", ch);
   act("$n is surrounded by a healthy white glow.", ch, NULL, NULL, TO_ROOM);
   return;
}

void do_flip( CHAR_DATA* ch, char*argument )
{
   switch (number_range(1, 2))
   {
      case 1:
      if (ch->gold > 0)
      {
         send_to_char("You toss a gold coin into the air ... Heads!\n\r", ch);
         act("$n tosses a gold coin into the air ... Heads!", ch, NULL, NULL, TO_ROOM);
      } else
      if (ch->silver > 0)
      {
         send_to_char("You toss a silver coin into the air ... Heads!\n\r", ch);
         act("$n tosses a silver coin into the air ... Heads!", ch, NULL, NULL, TO_ROOM);
      } else
      {
         send_to_char("You have no coins to flip.\n", ch);
      }
      break;
      default:
      if (ch->gold > 0)
      {
         send_to_char("You toss a gold coin into the air ... Tails!\n\r", ch);
         act("$n tosses a gold coin into the air ... Tails!", ch, NULL, NULL, TO_ROOM);
      } else
      if (ch->silver > 0)
      {
         send_to_char("You toss a silver coin into the air ... Tails!\n\r", ch);
         act("$n tosses a silver coin into the air ... Tails!", ch, NULL, NULL, TO_ROOM);
      } else
      {
         send_to_char("You have no coins to flip.\n\r", ch);
      }
   }
   return;
}

void do_say(CHAR_DATA* ch, char* argument)
{
   CHAR_DATA* victim;
   OBJ_DATA*  obj;
   OBJ_DATA*  brand;
   char original[MAX_INPUT_LENGTH];
   int chance;
   OBJ_DATA* cowsuit;

   cowsuit = get_eq_char(ch, WEAR_ABOUT);
   chance = (number_range(1, 10));

   strcpy(original, argument);
   argument = original;

   if (is_affected(ch, gsn_mute))
   {
      send_to_char("You try to say something, but only manage a weak rasping noise.\n\r", ch);
      return;
   }
   if
   (
      (
         cowsuit != NULL &&
         cowsuit->pIndexData->vnum == 80000
      ) &&
      chance == 1
   )
   {
      strcpy(argument, "Moo.");
   }
   if (is_affected(ch, gsn_toad))
   {
      strcpy(argument, "<ribbit>");
   }

   if
   (
      !IS_NPC(ch) &&
      is_mental(ch)
   )
   {
      if (ch->mana < 2 && !IS_IMMORTAL(ch))
      {
         send_to_char
         (
            "You do not have the mental energy to project thoughts.\n\r",
            ch
         );
         return;
      }
      else
      {
         ch->mana -= 2;
      }
   }

   if (argument[0] == '\0')
   {
      send_to_char("Say what?\n\r", ch);
      return;
   }

   if (is_affected(ch, gsn_silence))
   {
      send_to_char("You cannot find the words within you.\n\r", ch);
      return;
   }
   if (is_affected(ch, gsn_gag))
   {
      send_to_char("You cannot speak, you are gagged.\n\r", ch);
      return;
   }

   if (IS_SET(ch->comm, COMM_SILENCE))
   {
      send_to_char("You are unable to communicate.\n\r", ch);
      return;
   }

   if (is_affected(ch, gsn_healing_trance))
   {
      send_to_char("You cease to meditate.\n\r", ch);
      affect_strip(ch, gsn_healing_trance);
   }

   if (IS_SET(ch->comm, COMM_NOCHANNELS))
   {
      send_to_char("The gods have revoked your channel priviliges.\n\r", ch);
      return;
   }

   /* Stop feigning death, 90% chance */
   stop_feign(ch, 90);
   if (IS_SET(ch->act2, PLR_LOG_SPEC))
   {
      log_convo(ch, NULL, TYPE_SAY, argument);
   }

   brand = get_eq_char(ch, WEAR_BRAND);
   if (brand != NULL && !IS_IMMORTAL(ch))
   {
      if
      (
         strlen(argument) >= 11 &&
         brand->pIndexData->vnum == 15300
      )
      {
         send_to_char
         (
            "Your brand of the silent parrot burns as you speak out and break"
            " the silence.\n\r",
            ch
         );
         damage(ch, ch, 5, gsn_parrot, DAM_PIERCE, TRUE);
         ch->move -= 10;
      }
   }
   for (victim = ch->in_room->people; victim != NULL; victim = victim->next_in_room)
   {
      if (!IS_AWAKE(victim) || is_ignored(ch, victim))
      {
         continue;
      }
      if (victim == ch)
      {
         continue;
      }
      if
      (
         is_affected(victim, gsn_shock_sphere) &&
         !is_mental(ch)
      )
      {
         act
         (
            "$n says something you can't quite hear.",
            ch,
            NULL,
            victim,
            TO_VICT
         );
         continue;
      }
      if
      (
         victim != NULL &&
         !IS_NPC(victim) &&
         IS_SET(victim->act2, PLR_LOG_SPEC)
      )
      {
         log_convo(victim, ch, TYPE_SAY2, argument);
      }
      if (is_mental(ch))
      {
         act_color
         (
            "$n mentally projects '{B{6$t{n'",
            ch,
            argument,
            victim,
            TO_VICT
         );
      }
      else
      {
         act_color
         (
            "$n says '{B{6$t{n'",
            ch,
            argument,
            victim,
            TO_VICT
         );
      }
   }

   if (is_mental(ch))
   {
      act_color
      (
         "You mentally project '{B{6$T{n'",
         ch,
         NULL,
         argument,
         TO_CHAR
      );
   }
   else
   {
      act_color
      (
         "You say '{B{6$T{n'",
         ch,
         NULL,
         argument,
         TO_CHAR
      );
   }

   if (!IS_NPC(ch))
   {
      CHAR_DATA* mob;
      for (mob = ch->in_room->people; mob != NULL; mob = mob->next_in_room)
      {
         if
         (
            IS_NPC(mob) &&
            HAS_TRIGGER(mob, TRIG_SPEECH) &&
            mob->position == mob->pIndexData->default_pos
         )
         {
            mp_act_trigger(argument, mob, ch, NULL, NULL, TRIG_SPEECH);
         }
         mob_say_guardian_angel(ch, mob, argument);
      }
   }
   mob_say_lady(ch, argument);
   mob_say_standing(ch, argument);
   mob_say_house_statue(ch, argument);

   if (brand != NULL)
   {
      switch (brand->pIndexData->vnum)
      {
         case (OBJ_VNUM_SHAM_BRAND):
         {
            obj_trigger_sham_brand(ch, argument);
            break;
         }
         case (425):
         {
            obj_trigger_425(ch, argument);
            break;
         }
         case (15300):
         {
            obj_trigger_15300(ch, argument);
            break;
         }
         case (OBJ_VNUM_HECTOR_BRAND):  /* Wicket */
         {
            obj_trigger_7359(ch, argument);
            break;
         }
         case (OBJ_VNUM_UTARA_BRAND):  /* Wicket */
         {
            obj_trigger_7585(ch, argument);
            break;
         }
         case (430):
         {
            obj_say_430(ch, argument, brand);  /* Xyza's brand */
            break;
         }
         case (2109):
         {
            obj_say_2109(ch, argument, brand);  /* Cirdan's Brand */
            break;
         }
         case (15888):
         {
            obj_say_15888(ch, argument, brand);  /* Riallus's brand */
            break;
         }
         case (15901):
         {
            obj_say_15901(ch, argument, brand);  /* Xurinos's brand */
            break;
         }
         case (14600):
         {
            obj_say_14600(ch, argument);  /* adornos brand - runge */
            break;
         }
         case (14250):
         {
            obj_say_14250(ch, argument, brand);  /* barsaks brand - runge */
            break;
         }
         case (12500):  /* Wervdon's brand */
         {
            obj_say_12500(ch, argument, brand);
            obj_say_12500_2(ch, argument, brand);
            break;
         }
         case (980):
         {
            obj_say_431(ch, argument, brand);  /* awrathre's killer badger */
            break;
         }
         case (28903):
         {
            obj_say_28903(ch, argument, brand);  /* Runge's brand - Runge */
            break;
         }
         case (28211):
         {
            obj_say_28211(ch, argument, brand);  /* Dielantha's brand - Runge */
            break;
         }
         case (OBJ_VNUM_OBLIVION_BRAND):
         {
            obj_say_oblivion(ch, argument);
            /* Vrrin's Brand - Fizz */
            break;
         }
         case (OBJ_VNUM_MOB_BRAND):
         {
            obj_say_mob(ch, argument);
            /* Malignus's Brand - Fizz */
            break;
         }
         case (OBJ_VNUM_DRINLINDA_BRAND):
         {
            obj_say_21670(ch, argument, brand);  /* Drinlinda's brand - Drinny */
            break;
         }
         case (OBJ_VNUM_DRITHENTIR_BRAND):
         {
            obj_say_20025(ch, argument, brand);  /* Drithentir's brand - Drinny */
            break;
         }
         case (OBJ_VNUM_LESTREGUS_BRAND):
         {
            obj_say_7608(ch, argument, brand);   /* Lestregus' brand - Drinny  */
            break;
         }

         default:
         {
            break;
         }
      }
   }

   for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
   {
      /* Objects that always run */

      /* No objects for now, all should be worn */

      if (obj->wear_loc != WEAR_NONE)
      {
         /* Objects that must be worn */
         switch (obj->pIndexData->vnum)
         {
            case (27514):
            case (27515):
            case (27516):
            {
               obj_say_temporal(ch, argument, obj);
               break;
            }
            case (429):
            {
               obj_trigger_429(ch, argument);
               break;
            }
            case (17641):
            {
               obj_say_17641(ch, argument);
               obj_say_17641_2(ch, argument);
               break;
            }
            case (26309):
            {
               obj_say_26309(ch, argument);
               break;
            }
            case (6712):
            {
               obj_trigger_6712(ch, argument, obj);
               break;
            }
            case (OBJ_VNUM_FLAME_TOWER):
            {
               obj_trigger_14718(ch, argument);  /* mace of the tower */
               break;
            }
            case (OBJ_VNUM_ROD_LORDLY):
            {
               obj_say_lordly(ch, obj, argument);
               break;
            }
            case (OBJ_VNUM_HEAVENLY_SCEPTRE):
            {
               obj_say_heavenly_sceptre(ch, obj, argument);
               break;
            }
            case (OBJ_VNUM_SENTIENT_SWORD):
            {
               obj_say_sentient_sword(ch, argument, obj);
               /* The sentient sword - Mael */
               break;
            }
            default:
            {
               break;
            }
         }
      }
      /* Objects that do not have to be worn */
      switch (obj->pIndexData->vnum)
      {
         default:
         {
            break;
         }
         case (1888):  /* Chaos artifact */
         {
            obj_say_1888(ch, argument, obj);
            break;
         }
         case (27513):
         {
            obj_say_27513(ch, argument, obj);
            break;
         }
         case (13307):
         {
            if (stristr(argument, "marfu baldash corr"))
            {
               act("$n's hermit's relic flashes!", ch, NULL, NULL, TO_ROOM);
               act("Your hermit's relic flashes!", ch, NULL, NULL, TO_CHAR);
            }
            break;
         }
      }
   }

   obj_say_traitor(ch, argument);
   /*
   Replaced with mobprogs by Drinlinda
   Commented out by Fizz
   say_prog_karguska(ch, argument);
   */
   say_prog_summon_demon(ch, argument);
   say_prog_raise_shadowlord(ch, argument);
   say_prog_cru_blacksmith(ch, argument);

   return;
}



void do_pray(CHAR_DATA* ch, char* argument)
{
   DESCRIPTOR_DATA* d;

   if (argument[0] == '\0' )
   {
      send_to_char("Pray what?\n\r", ch);
      return;
   }

   if (IS_SET(ch->comm, COMM_SILENCE))
   {
      send_to_char("You are unable to communicate.\n\r", ch);
      return;
   }

   if (IS_SET(ch->comm, COMM_NOCHANNELS))
   {
      send_to_char("You can't pray.\n\r", ch);
      return;
   }

   if (IS_SET(ch->comm, COMM_NOPRAY))
   {
      send_to_char
      (
         "Your prayers fall on deaf ears, the gods have forsaken you.\n\r",
         ch
      );
      return;
   }

   if (!IS_IMMORTAL(ch))
   {
      WAIT_STATE(ch, 12);
   }
   if (IS_SET(ch->comm, COMM_ANSI))
   {
      act
      (
         "\x01B[1;31mYou pray to the gods...  Don't abuse this.\x01B[0;37m",
         ch,
         NULL,
         argument,
         TO_CHAR
      );
   }
   else
   {
      act
      (
         "You pray to the gods...  Don't abuse this.",
         ch,
         NULL,
         argument,
         TO_CHAR
      );
   }

   /* Anti-Swear code - Fizzfaldt */
   if (is_vulgar(argument, vulgar_table))
   {
      send_to_char("Kindly keep a reign on your tongue.\n\r", ch);
      if (!IS_IMMORTAL(ch))
      {
         WAIT_STATE(ch, 360);
      }
   }

   for (d = descriptor_list; d != NULL; d = d->next)
   {
      CHAR_DATA* victim;

      victim = d->original ? d->original : d->character;

      if
      (
         d->connected == CON_PLAYING &&
         d->character != ch &&
         !IS_SET(victim->comm, COMM_SHOUTSOFF) &&
         !IS_SET(victim->comm, COMM_QUIET) &&
         victim->level >= 52 &&
         !is_ignored(ch, victim)
      )
      {
         if (can_see(victim, ch))
         {
            if (IS_SET(d->character->comm, COMM_ANSI))
            {
               act
               (
                  "\x01B[1;31m$f PRAYS: $t\x01B[0;37m",
                  ch,
                  argument,
                  d->character,
                  TO_VICT
               );
            }
            else
            {
               act("$f PRAYS: $t", ch, argument, d->character, TO_VICT);
            }
         }
         else
         {
            if (IS_SET(d->character->comm, COMM_ANSI))
            {
               act
               (
                  "\x01B[1;31m$n PRAYS: $t\x01B[0;37m",
                  ch,
                  argument,
                  d->character,
                  TO_VICT
               );
            }
            else
            {
               act("$n PRAYS: $t", ch, argument, d->character, TO_VICT);
            }
         }
      }
   }

   return;
}

void do_ignore(CHAR_DATA* ch, char* argument)
{
   char arg[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];

   if (IS_NPC(ch))
   {
      send_to_char("You are incapable of ignoring.", ch);
      return;
   }

   one_argument(argument, arg);

   if (arg[0] == '\0')
   {
      sprintf(buf, "You are ignoring: %s\n\r", ch->pcdata->ignore);
      send_to_char(buf, ch);
   }
   else
   {
      sprintf(buf, "%s", argument);
      free_string(ch->pcdata->ignore);
      ch->pcdata->ignore = str_dup(buf);
      sprintf(buf, "You are now ignoring: %s\n\r", ch->pcdata->ignore);
      send_to_char(buf, ch);
   }

   return;
}

void do_tell(CHAR_DATA* ch, char* argument )
{
   char arg [MAX_INPUT_LENGTH];
   char* name;
   char buf [MAX_STRING_LENGTH];
   CHAR_DATA* victim;
   OBJ_DATA* brand;
   int count = 0;
   int number = 0;
   char arg2 [MAX_INPUT_LENGTH];
   char original [MAX_INPUT_LENGTH];
   bool pc_only = FALSE;
   bool mob_only = FALSE;
   int chance;
   OBJ_DATA* cowsuit;

   cowsuit = get_eq_char(ch, WEAR_ABOUT);
   chance = (number_range(1, 10));

   if
   (
      !IS_NPC(ch) &&
      is_mental(ch)
   )
   {
      if (ch->mana < 4 && !IS_IMMORTAL(ch))
      {
         send_to_char
         (
            "You do not have the mental energy to project thoughts.\n\r",
            ch
         );
         return;
      }
      else
      {
         ch->mana -= 4;
      }
   }

   if (is_affected(ch, gsn_mute))
   {
      send_to_char
      (
         "You try to tell them something, but only manage a weak rasping noise."
         "\n\r",
         ch
      );
      return;
   }

   if (is_affected(ch, gsn_gag))
   {
      send_to_char("You cannot speak, you are gagged.\n\r", ch);
      return;
   }

   if (is_affected(ch, gsn_stalking))
   {
      send_to_char("You stop stalking.\n\r", ch);
      affect_strip(ch, gsn_stalking);
   }

   if
   (
      is_affected(ch, gsn_silence) &&
      !is_mental(ch)
   )
   {
      send_to_char("You cannot find the words within you.\n\r", ch);
      return;
   }

   if (IS_SET(ch->comm, COMM_SILENCE))
   {
      send_to_char("You are unable to communicate.\n\r", ch);
      return;
   }

   if (is_affected(ch, gsn_healing_trance))
   {
      send_to_char("You cease to meditate.\n\r", ch);
      affect_strip(ch, gsn_healing_trance);
   }

   if (is_affected(ch, gsn_stalking))
   {
      send_to_char("You stop stalking.\n\r", ch);
      affect_strip(ch, gsn_stalking);
   }

   if (IS_SET(ch->comm, COMM_NOTELL))
   {
      send_to_char( "Your message didn't get through.\n\r", ch );
      return;
   }

   if (IS_SET(ch->comm, COMM_QUIET))
   {
      send_to_char( "You must turn off quiet mode first.\n\r", ch);
      return;
   }

   if (IS_SET(ch->comm, COMM_DEAF))
   {
      send_to_char("You must turn off deaf mode first.\n\r", ch);
      return;
   }

   argument = one_argument(argument, arg);

   if (arg[0] == '\0' || argument[0] == '\0')
   {
      send_to_char("Tell whom what?\n\r", ch);
      return;
   }

   strcpy(original, argument);
   argument = original;

   if (is_affected(ch, gsn_toad))
   {
      sprintf(argument, "<ribbit>");
   }


   if
   (
      (
         cowsuit != NULL &&
         cowsuit->pIndexData->vnum == 80000
      ) &&
      chance == 1
   )
   {
      sprintf(argument, "Moo.");
   }

   /* This added for decoy tells */
   strcpy(arg2, arg);

   /*
   Can tell to PC's anywhere, but NPC's only in same room.
   -- Furey
   */
   name = arg2;
   if
   (
      name[0] == '+' &&
      (
         ch == NULL ||
         (
            IS_IMMORTAL(ch) ||
            (
               IS_NPC(ch) &&
               !IS_AFFECTED(ch, AFF_CHARM)
            )
         )
      )
   )
   {
      name++;
      pc_only = TRUE;
   }
   else if
   (
      name[0] == '-' &&
      (
         ch == NULL ||
         (
            IS_IMMORTAL(ch) ||
            (
               IS_NPC(ch) &&
               !IS_AFFECTED(ch, AFF_CHARM)
            )
         )
      )
   )
   {
      name++;
      mob_only = TRUE;
   }

   number = number_argument(name, arg);
   count = 0;

   for (victim = char_list; victim != NULL; victim = victim->next)
   {
      if
      (
         victim->in_room == NULL ||
         (
            IS_NPC(victim) &&
            (
               pc_only ||
               (
                  victim->pIndexData->vnum == MOB_VNUM_DECOY ||
                  victim->pIndexData->vnum == MOB_VNUM_SHADOW ||
                  victim->pIndexData->vnum == MOB_VNUM_MIRROR ||
                  victim->pIndexData->vnum == MOB_VNUM_PHANTOM
               )
            )
         ) ||
         (
            !IS_NPC(victim) &&
            mob_only
         ) ||
         !is_name(arg, get_name(victim, ch)) ||
         !can_see(ch, victim) ||
         ++count != number
      )
      {
         continue;
      }
      break;
   }

   if
   (
      victim == NULL ||
      (
         IS_NPC(victim) &&
         victim->in_room != ch->in_room
      )
   )
   {
      send_to_char("They are not here.\n\r", ch);
      return;
   }

   brand = get_eq_char(ch, WEAR_BRAND);
   if
   (
      brand != NULL &&
      (!IS_IMMORTAL(ch))
   )
   {
      if (strlen(argument) >= 11 && brand->pIndexData->vnum == 15300)
      {
         send_to_char
         (
            "Your brand of the silent parrot burns as you break the"
            " silence with so many words.\n\r",
            ch
         );
         ch->move -= 15;
      }
   }

   /***** originally the only ignore check *****
   if (is_ignored(ch, victim))
   {
   act( "$E is ignoring you.", ch, NULL, victim, TO_CHAR );
   return;
   }
   ******/

   if (is_ignored(victim, ch))
   {
      send_to_char
      (
         "If you are ignoring them, it would hardly be fair for you"
         " to speak to them.\n\rYou decide not to talk after all.\n\r",
         ch
      );
      return;
   }

   /* Stop feigning death, 40% chance */
   stop_feign(ch, 40);

   if
   (
      !(
         IS_IMMORTAL(ch) &&
         ch->level > LEVEL_IMMORTAL
      ) &&
      !is_mental(ch) &&
      !IS_AWAKE(victim)
   )
   {
      act("$E can't hear you.", ch, NULL, victim, TO_CHAR);
      return;
   }
   if
   (
      is_affected(victim, gsn_shock_sphere) &&
      !is_mental(ch)
   )
   {
      act("$E can't hear you.", ch, NULL, victim, TO_CHAR);
      return;
   }


   if
   (
      !IS_IMMORTAL(ch) &&
      is_affected(victim, gsn_healing_trance)
   )
   {
      act("$E is meditating and thus could not hear you.", ch, NULL, victim, TO_CHAR);
      return;
   }

   if
   (
      (
         IS_SET(victim->comm, COMM_QUIET) ||
         IS_SET(victim->comm, COMM_DEAF)
      ) &&
      !(
         IS_IMMORTAL(ch) &&
         ch->level > victim->level
      )
   )
   {
      act("$E is not receiving tells.", ch, NULL, victim, TO_CHAR);
      return;
   }

   if (IS_SET(ch->act2, PLR_LOG_SPEC))
   {
      log_convo(ch, victim, TYPE_TELL, argument);
   }

   if (victim != NULL && IS_SET(victim->act2, PLR_LOG_SPEC))
   {
      log_convo(victim, ch, TYPE_TELL2, argument);
   }

   if (victim->desc == NULL && !IS_NPC(victim))
   {
      act
      (
         "$N seems to have lost consciousness...try again later.",
         ch,
         NULL,
         victim,
         TO_CHAR
      );
      if
      (
         !is_ignored(ch, victim) &&
         (
            !(
               IS_SET(victim->comm, COMM_QUIET) ||
               IS_SET(victim->comm, COMM_DEAF)
            ) ||
            (
               IS_IMMORTAL(ch) &&
               ch->level > victim->level
            )
         )
      )
      {
         if (is_mental(ch))
         {
            if (IS_SET(victim->comm, COMM_ANSI))
            {
               sprintf
               (
                  buf,
                  "%s mentally projects to you '\x01b[1;32m%s\x01b[0;37m'\n\r",
                  PERS(ch, victim),
                  argument
               );
            }
            else
            {
               sprintf
               (
                  buf,
                  "%s mentally projects to you '%s'\n\r",
                  PERS(ch, victim),
                  argument
               );
            }
         }
         else
         {
            if (IS_SET(victim->comm, COMM_ANSI))
            {
               sprintf
               (
                  buf,
                  "%s tells you '\x01b[1;32m%s\x01b[0;37m'\n\r",
                  PERS(ch, victim),
                  argument
               );
            }
            else
            {
               sprintf
               (
                  buf,
                  "%s tells you '%s'\n\r",
                  PERS(ch, victim),
                  argument
               );
            }
         }
         buf[0] = UPPER(buf[0]);
         add_buf(victim->pcdata->buffer, buf);
      }
      return;
   }

   if (IS_IMMORTAL(ch) && IS_SET(ch->comm, COMM_ANSI))
   {
      if (is_mental(ch))
      {
         if (can_see(ch, victim))
         {
            act
            (
               "You mentally project to $F '\x01b[1;32m$t\x01B[0;37m'",
               ch,
               argument,
               victim,
               TO_CHAR
            );
         }
         else
         {
            act
            (
               "You mentally project to $N '\x01b[1;32m$t\x01B[0;37m'",
               ch,
               argument,
               victim,
               TO_CHAR
            );
         }
      }
      else
      {
         if (can_see(ch, victim))
         {
            act
            (
               "You tell $F '\x01b[1;32m$t\x01B[0;37m'",
               ch,
               argument,
               victim,
               TO_CHAR
            );
         }
         else
         {
            act
            (
               "You tell $N '\x01b[1;32m$t\x01B[0;37m'",
               ch,
               argument,
               victim,
               TO_CHAR
            );
         }
      }
   }
   else if (IS_IMMORTAL(ch))
   {
      if (is_mental(ch))
      {
         if (can_see(ch, victim))
         {
            act("You mentally project to $F '$t'", ch, argument, victim, TO_CHAR);
         }
         else
         {
            act("You mentally project to $N '$t'", ch, argument, victim, TO_CHAR);
         }
      }
      else
      {
         if (can_see(ch, victim))
         {
            act("You tell $F '$t'", ch, argument, victim, TO_CHAR);
         }
         else
         {
            act("You tell $N '$t'", ch, argument, victim, TO_CHAR);
         }
      }
   }
   else if (IS_SET(ch->comm, COMM_ANSI))
   {
      if (is_mental(ch))
      {
         act
         (
            "You mentally project to $N '\x01B[1;32m$t\x01B[0;37m'",
            ch,
            argument,
            victim,
            TO_CHAR
         );
      }
      else
      {
         act
         (
            "You tell $N '\x01B[1;32m$t\x01B[0;37m'",
            ch,
            argument,
            victim,
            TO_CHAR
         );
      }
   }
   else
   {
      if (is_mental(ch))
      {
         act("You mentally project to $N '$t'", ch, argument, victim, TO_CHAR);
      }
      else
      {
         act("You tell $N '$t'", ch, argument, victim, TO_CHAR);
      }
   }

   if (is_ignored(ch, victim))
   {
      return;
   }

   if (IS_IMMORTAL(victim) && IS_SET(victim->comm, COMM_ANSI))
   {
      if (is_mental(ch))
      {
         if (can_see(victim, ch))
         {
            act_new
            (
               "$f mentally projects to you '\x01B[1;32m$t\x01B[0;37m'",
               ch,
               argument,
               victim,
               TO_VICT,
               POS_DEAD
            );
         }
         else
         {
            act_new
            (
               "$n mentally projects to you '\x01B[1;32m$t\x01B[0;37m'",
               ch,
               argument,
               victim,
               TO_VICT,
               POS_DEAD
            );
         }
      }
      else
      {
         if (can_see(victim, ch))
         {
            act_new
            (
               "$f tells you '\x01B[1;32m$t\x01B[0;37m'",
               ch,
               argument,
               victim,
               TO_VICT,
               POS_DEAD
            );
         }
         else
         {
            act_new
            (
               "$n tells you '\x01B[1;32m$t\x01B[0;37m'",
               ch,
               argument,
               victim,
               TO_VICT,
               POS_DEAD
            );
         }
      }
   }
   else if (IS_IMMORTAL(victim))
   {
      if (is_mental(ch))
      {
         if (can_see(victim, ch))
         {
            act_new
            (
               "$f mentally projects to you '$t'",
               ch,
               argument,
               victim,
               TO_VICT,
               POS_DEAD
            );
         }
         else
         {
            act_new
            (
               "$n mentally projects to you '$t'",
               ch,
               argument,
               victim,
               TO_VICT,
               POS_DEAD
            );
         }
      }
      else
      {
         if (can_see(victim, ch))
         {
            act_new("$f tells you '$t'", ch, argument, victim, TO_VICT, POS_DEAD);
         }
         else
         {
            act_new("$n tells you '$t'", ch, argument, victim, TO_VICT, POS_DEAD);
         }
      }
   }
   else if (IS_SET(victim->comm, COMM_ANSI))
   {
      if (is_mental(ch))
      {
         act_new
         (
            "$n mentally projects to you '\x01B[1;32m$t\x01B[0;37m'",
            ch,
            argument,
            victim,
            TO_VICT,
            POS_DEAD
         );
      }
      else
      {
         act_new
         (
            "$n tells you '\x01B[1;32m$t\x01B[0;37m'",
            ch,
            argument,
            victim,
            TO_VICT,
            POS_DEAD
         );
      }
   }
   else
   {
      if (is_mental(ch))
      {
         act_new
         (
            "$n mentally projects to you '$t'",
            ch,
            argument,
            victim,
            TO_VICT,
            POS_DEAD
         );
      }
      else
      {
         act_new
         (
            "$n tells you '$t'",
            ch,
            argument,
            victim,
            TO_VICT,
            POS_DEAD
         );
      }
   }
   victim->reply    = ch;

   if
   (
      !IS_NPC(ch) &&
      IS_NPC(victim) &&
      HAS_TRIGGER(victim, TRIG_SPEECH)
   )
   mp_act_trigger(argument, victim, ch, NULL, NULL, TRIG_SPEECH);

   return;
}


void do_ooctell(CHAR_DATA* ch, char* argument)
{
   char arg [MAX_INPUT_LENGTH];
   char buf [MAX_STRING_LENGTH];
   char* name;
   CHAR_DATA* victim;
   int count  = 0;
   int number = 0;
   char arg2 [MAX_INPUT_LENGTH];
   bool pc_only = FALSE;
   bool mob_only = FALSE;

   argument = one_argument(argument, arg);

   if (is_affected(ch, gsn_toad))
   {
      send_to_char("You cannot do this while in toad form.\n\r", ch);
      return;
   }
   if (is_affected(ch, gsn_mute))
   {
      send_to_char("You cannot send ooc tells while muted.\n\r", ch);
      return;
   }
   if (is_affected(ch, gsn_gag))
   {
      send_to_char("You cannot send ooc tells while gagged.\n\r", ch);
      return;
   }
   /* Toggle OOC channel on/off */
   if ( arg[0] == '\0' || argument[0] == '\0' )
   {
      if (IS_SET(ch->comm, COMM_OOCOFF))
      {
         send_to_char("You are allowing OOC tells.\n\r", ch);
         REMOVE_BIT(ch->comm, COMM_OOCOFF);
      }
      else
      {
         send_to_char("You no longer want OOC tells.\n\r", ch);
         SET_BIT(ch->comm, COMM_OOCOFF);
      }
      return;
   }

   if (IS_SET(ch->comm, COMM_SILENCE))
   {
      send_to_char("You are unable to communicate.\n\r", ch);
      return;
   }

   if (is_affected(ch, gsn_stalking))
   {
      send_to_char("You stop stalking.\n\r", ch);
      affect_strip(ch, gsn_stalking);
   }

   if ( IS_SET(ch->comm, COMM_NOTELL))
   {
      send_to_char("Your message didn't get through.\n\r", ch );
      return;
   }

   if ( IS_SET(ch->comm, COMM_QUIET))
   {
      send_to_char("You must turn off quiet mode first.\n\r", ch);
      return;
   }

   if (IS_SET(ch->comm, COMM_DEAF))
   {
      send_to_char("You must turn off deaf mode first.\n\r", ch);
      return;
   }

   if (IS_SET(ch->comm, COMM_OOCOFF))
   {
      send_to_char("You are currently not interested in OOC.\n\r", ch);
      return;
   }

   /* This added for decoy tells */
   strcpy(arg2, arg);

   /*
   Can tell to PC's anywhere, but NPC's only in same room.
   -- Furey
   */

   name = arg2;
   if
   (
      name[0] == '+' &&
      (
         ch == NULL ||
         (
            IS_IMMORTAL(ch) ||
            (
               IS_NPC(ch) &&
               !IS_AFFECTED(ch, AFF_CHARM)
            )
         )
      )
   )
   {
      name++;
      pc_only = TRUE;
   }
   else if
   (
      name[0] == '-' &&
      (
         ch == NULL ||
         (
            IS_IMMORTAL(ch) ||
            (
               IS_NPC(ch) &&
               !IS_AFFECTED(ch, AFF_CHARM)
            )
         )
      )
   )
   {
      name++;
      mob_only = TRUE;
   }

   number = number_argument(name, arg);
   count = 0;

   for (victim = char_list; victim != NULL; victim = victim->next)
   {
      if
      (
         victim->in_room == NULL ||
         (
            IS_NPC(victim) &&
            (
               pc_only ||
               (
                  victim->pIndexData->vnum == MOB_VNUM_DECOY ||
                  victim->pIndexData->vnum == MOB_VNUM_SHADOW ||
                  victim->pIndexData->vnum == MOB_VNUM_MIRROR ||
                  victim->pIndexData->vnum == MOB_VNUM_PHANTOM
               )
            )
         ) ||
         (
            !IS_NPC(victim) &&
            mob_only
         ) ||
         !is_name(arg, get_name(victim, ch)) ||
         !can_see(ch, victim) ||
         ++count != number
      )
      {
         continue;
      }
      break;
   }

   if
   (
      victim == NULL ||
      (
         IS_NPC(victim) &&
         victim->in_room != ch->in_room
      )
   )
   {
      send_to_char("They are not here.\n\r", ch);
      return;
   }

   /***** originally the only ignore check *****
   if (is_ignored(ch, victim))
   {
   act( "$E is ignoring you.", ch, NULL, victim, TO_CHAR );
   return;
   }
   *****/

   if (is_ignored(victim, ch))
   {
      send_to_char
      (
         "If you are ignoring them, it would hardly be fair for you"
         " to speak to them.\n\rYou decide not to talk after all.\n\r",
         ch
      );
      {
         return;
      }
   }

   if
   (
      !(
         IS_IMMORTAL(ch) &&
         ch->level > LEVEL_IMMORTAL
      ) &&
      !is_mental(ch) &&
      !IS_AWAKE(victim)
   )
   {
      act("$E cannot hear you.", ch, NULL, victim, TO_CHAR);
      return;
   }
   if
   (
      is_affected(victim, gsn_shock_sphere) &&
      !is_mental(ch)
   )
   {
      act("$E cannot hear you.", ch, NULL, victim, TO_CHAR);
      return;
   }


   if
   (
      (
         IS_SET(victim->comm, COMM_QUIET) ||
         IS_SET(victim->comm, COMM_DEAF)
      ) &&
      !(
         IS_IMMORTAL(ch) &&
         ch->level > victim->level
      )
   )
   {
      act("$E is not receiving tells.", ch, NULL, victim, TO_CHAR);
      return;
   }

   if (
      IS_SET(victim->comm, COMM_OOCOFF) &&
      !(
         IS_IMMORTAL(ch) &&
         ch->level > victim->level
      )
   )
   {
      act("$E would like to roleplay only. Do not speak OOC in $S presence.", ch, NULL, victim, TO_CHAR);
      return;
   }

   if (IS_SET(ch->act2, PLR_LOG_SPEC))
   {
      log_convo(ch, victim, TYPE_OOC, argument);
   }

   if (victim != NULL && IS_SET(victim->act2, PLR_LOG_SPEC))
   {
      log_convo(victim, ch, TYPE_OOC2, argument);
   }

   if (victim->desc == NULL && !IS_NPC(victim))
   {
      act
      (
         "$N seems to have lost consciousness...try again later.",
         ch,
         NULL,
         victim,
         TO_CHAR
      );
      if
      (
         !is_ignored(ch, victim) &&
         (
            !(
               IS_SET(victim->comm, COMM_QUIET) ||
               IS_SET(victim->comm, COMM_DEAF)
            ) ||
            (
               IS_IMMORTAL(ch) &&
               ch->level > victim->level
            )
         )
      )
      {
         if (IS_SET(victim->comm, COMM_ANSI))
         {
            sprintf
            (
               buf,
               "\x01b[0;36m[OOC] %s: %s\x01B[0;37m\n\r",
               PERS(ch, victim),
               argument
            );
         }
         else
         {
            sprintf(buf, "[OOC] %s: %s\n\r", PERS(ch, victim), argument);
         }
         buf[0] = UPPER(buf[0]);
         add_buf(victim->pcdata->buffer, buf);
      }
      return;
   }

   if
   (
      IS_IMMORTAL(ch) &&
      IS_SET(ch->comm, COMM_ANSI)
   )
   {
      if (can_see(ch, victim))
      {
         act
         (
            "\x01b[0;36m[OOC] to $F: $t\x01B[0;37m",
            ch,
            argument,
            victim,
            TO_CHAR
         );
      }
      else
      {
         act
         (
            "\x01b[0;36m[OOC] to $F: $t\x01B[0;37m",
            ch,
            argument,
            victim,
            TO_CHAR
         );
      }
   }
   else if (IS_IMMORTAL(ch))
   {
      if (can_see(ch, victim))
      {
         act("[OOC] to $F: $t", ch, argument, victim, TO_CHAR);
      }
      else
      {
         act("[OOC] to $F: $t", ch, argument, victim, TO_CHAR);
      }
   }
   else if (IS_SET(ch->comm, COMM_ANSI))
   {
      act
      (
         "\x01b[0;36m[OOC] to $N: $t\x01B[0;37m",
         ch,
         argument,
         victim,
         TO_CHAR
      );
   }
   else
   {
      act("[OOC] to $N: $t", ch, argument, victim, TO_CHAR);
   }

   if (is_ignored(ch, victim))
   {
      return;
   }

   if
   (
      IS_IMMORTAL(victim) &&
      IS_SET(victim->comm, COMM_ANSI)
   )
   {
      if (can_see(victim, ch))
      {
         act_new
         (
            "\x01b[0;36m[OOC] $f: $t\x01B[0;37m",
            ch,
            argument,
            victim,
            TO_VICT,
            POS_DEAD
         );
      }
      else
      {
         act_new
         (
            "\x01b[0;36m[OOC] $n: $t\x01B[0;37m",
            ch,
            argument,
            victim,
            TO_VICT,
            POS_DEAD
         );
      }
   }
   else if (IS_IMMORTAL(victim))
   {
      if (can_see(victim, ch))
      {
         act_new("[OOC] $f: $t", ch, argument, victim, TO_VICT, POS_DEAD);
      }
      else
      {
         act_new("[OOC] $n: $t", ch, argument, victim, TO_VICT, POS_DEAD);
      }
   }
   else if (IS_SET(victim->comm, COMM_ANSI))
   {
      act_new
      (
         "\x01b[0;36m[OOC] $n: $t\x01B[0;37m",
         ch,
         argument,
         victim,
         TO_VICT,
         POS_DEAD
      );
   }
   else
   {
      act_new("[OOC] $n: $t", ch, argument, victim, TO_VICT, POS_DEAD);
   }

   return;
}


void do_reply(CHAR_DATA* ch, char* argument)
{
   CHAR_DATA* victim;
   char buf [MAX_STRING_LENGTH];
   char original[MAX_INPUT_LENGTH];
   int chance;
   OBJ_DATA* cowsuit;

   cowsuit = get_eq_char(ch, WEAR_ABOUT);
   chance = (number_range(1, 10));

   if (is_affected(ch, gsn_mute))
   {
      send_to_char
      (
         "You try to tell them something, but only manage a weak"
         " rasping noise.\n\r",
         ch
      );
      return;
   }
   if (is_affected(ch, gsn_gag))
   {
      send_to_char("You cannot reply, you are gagged.\n\r", ch);
      return;
   }

   if
   (
      !IS_NPC(ch) &&
      is_mental(ch)
   )
   {
      if
      (
         ch->mana < 4 &&
         !IS_IMMORTAL(ch)
      )
      {
         send_to_char
         (
            "You do not have the mental energy to project thoughts.\n\r",
            ch
         );
         return;
      }
      else
      {
         ch->mana -= 4;
      }
   }

   if (IS_SET(ch->comm, COMM_SILENCE))
   {
      send_to_char("You are unable to communicate.\n\r", ch);
      return;
   }

   if (IS_SET(ch->comm, COMM_NOTELL))
   {
      send_to_char("Your message didn't get through.\n\r", ch);
      return;
   }

   victim = ch->reply;

   if (victim  == NULL)
   {
      send_to_char("They are not here.\n\r", ch);
      return;
   }

   /***** originally the only ignore check *****
   if (is_ignored(ch, victim))
   {
   act( "$E is ignoring you.", ch, NULL, victim, TO_CHAR );
   return;
   }
   *****/

   if (is_ignored(victim, ch))
   {
      send_to_char
      (
         "If you are ignoring them, it would hardly be fair for you to"
         "speak to them.\n\rYou decide not to talk after all.\n\r",
         ch
      );
      return;
   }

   if (is_affected(ch, gsn_stalking))
   {
      send_to_char("You stop stalking.\n\r", ch);
      affect_strip(ch, gsn_stalking);
   }

   /* Stop feigning death, 33% chance */
   stop_feign(ch, 33);

   if (!IS_IMMORTAL(ch) && !IS_AWAKE(victim) && !is_mental(ch))
   {
      act("$E cannot hear you.", ch, NULL, victim, TO_CHAR);
      return;
   }

   if
   (
      (
         IS_SET(victim->comm, COMM_QUIET) ||
         IS_SET(victim->comm, COMM_DEAF)
      ) &&
      !(
         IS_IMMORTAL(ch) &&
         ch->level > victim->level
      )
   )
   {
      act_new("$E is not receiving tells.", ch, 0, victim, TO_CHAR, POS_DEAD);
      return;
   }

   if (!IS_IMMORTAL(victim) && !IS_AWAKE(ch))
   {
      send_to_char( "In your dreams, or what?\n\r", ch );
      return;
   }

   strcpy(original, argument);
   argument = original;

   if (is_affected(ch, gsn_toad))
   {
      sprintf(argument, "<ribbit>");
   }

   if
   (
      (
         cowsuit != NULL &&
         cowsuit->pIndexData->vnum == 80000
      ) &&
      chance == 1
   )
   {
      sprintf(argument, "Moo.");
   }


   if (IS_SET(ch->act2, PLR_LOG_SPEC))
   {
      log_convo(ch, victim, TYPE_TELL, argument);
   }

   if (victim != NULL && IS_SET(victim->act2, PLR_LOG_SPEC))
   {
      log_convo(victim, ch, TYPE_TELL2, argument);
   }

   if (victim->desc == NULL && !IS_NPC(victim))
   {
      act
      (
         "$N seems to have lost consciousness...try again later.",
         ch,
         NULL,
         victim,
         TO_CHAR
      );
      if
      (
         !is_ignored(ch, victim) &&
         (
            !(
               IS_SET(victim->comm, COMM_QUIET) ||
               IS_SET(victim->comm, COMM_DEAF)
            ) ||
            (
               IS_IMMORTAL(ch) &&
               ch->level > victim->level
            )
         )
      )
      {
         if (is_mental(ch))
         {
            if (IS_SET(victim->comm, COMM_ANSI))
            {
               sprintf
               (
                  buf,
                  "%s tells you '\x01b[1;32m%s\x01b[0;37m'\n\r",
                  PERS(ch, victim),
                  argument
               );
            }
            else
            {
               sprintf(buf, "%s tells you '%s'\n\r", PERS(ch, victim), argument);
            }
         }
         else
         {
            if (IS_SET(victim->comm, COMM_ANSI))
            {
               sprintf
               (
                  buf,
                  "%s tells you '\x01b[1;32m%s\x01b[0;37m'\n\r",
                  PERS(ch, victim),
                  argument
               );
            }
            else
            {
               sprintf
               (
                  buf,
                  "%s tells you '%s'\n\r",
                  PERS(ch, victim),
                  argument
               );
            }
         }
         buf[0] = UPPER(buf[0]);
         add_buf(victim->pcdata->buffer, buf);
      }
      return;
   }

   if
   (
      IS_IMMORTAL(ch) &&
      IS_SET(ch->comm, COMM_ANSI)
   )
   {
      if (is_mental(ch))
      {
         if (can_see(ch, victim))
         {
            act
            (
               "You mentally project to $F '\x01b[1;32m$t\x01B[0;37m'",
               ch,
               argument,
               victim,
               TO_CHAR
            );
         }
         else
         {
            act
            (
               "You mentally project to $N '\x01b[1;32m$t\x01B[0;37m'",
               ch,
               argument,
               victim,
               TO_CHAR
            );
         }
      }
      else
      {
         if (can_see(ch, victim))
         {
            act
            (
               "You tell $F '\x01b[1;32m$t\x01B[0;37m'",
               ch,
               argument,
               victim,
               TO_CHAR
            );
         }
         else
         {
            act
            (
               "You tell $N '\x01b[1;32m$t\x01B[0;37m'",
               ch,
               argument,
               victim,
               TO_CHAR
            );
         }
      }
   }
   else if (IS_IMMORTAL(ch))
   {
      if (is_mental(ch))
      {
         if (can_see(ch, victim))
         {
            act("You mentally project to $F '$t'", ch, argument, victim, TO_CHAR);
         }
         else
         {
            act("You mentally project to $N '$t'", ch, argument, victim, TO_CHAR);
         }
      }
      else
      {
         if (can_see(ch, victim))
         {
            act("You tell $F '$t'", ch, argument, victim, TO_CHAR);
         }
         else
         {
            act("You tell $N '$t'", ch, argument, victim, TO_CHAR);
         }
      }
   }
   else if (IS_SET(ch->comm, COMM_ANSI))
   {
      if (is_mental(ch))
      {
         act
         (
            "You mentally project to $N '\x01B[1;32m$t\x01B[0;37m'",
            ch,
            argument,
            victim,
            TO_CHAR
         );
      }
      else
      {
         act
         (
            "You tell $N '\x01B[1;32m$t\x01B[0;37m'",
            ch,
            argument,
            victim,
            TO_CHAR
         );
      }
   }
   else
   {
      if (is_mental(ch))
      {
         act("You mentally project to $N '$t'", ch, argument, victim, TO_CHAR);
      }
      else
      {
         act("You tell $N '$t'", ch, argument, victim, TO_CHAR);
      }
   }

   if (is_ignored(ch, victim))
   {
      return;
   }

   if
   (
      IS_IMMORTAL(victim) &&
      IS_SET(victim->comm, COMM_ANSI)
   )
   {
      if (is_mental(ch))
      {
         if (can_see(victim, ch))
         {
            act_new
            (
               "$f mentally projects to you '\x01B[1;32m$t\x01B[0;37m'",
               ch,
               argument,
               victim,
               TO_VICT,
               POS_DEAD
            );
         }
         else
         {
            act_new
            (
               "$n mentally projects to you '\x01B[1;32m$t\x01B[0;37m'",
               ch,
               argument,
               victim,
               TO_VICT,
               POS_DEAD
            );
         }

      }
      else
      {
         if (can_see(victim, ch))
         {
            act_new
            (
               "$f tells you '\x01B[1;32m$t\x01B[0;37m'",
               ch,
               argument,
               victim,
               TO_VICT,
               POS_DEAD
            );
         }
         else
         {
            act_new
            (
               "$n tells you '\x01B[1;32m$t\x01B[0;37m'",
               ch,
               argument,
               victim,
               TO_VICT,
               POS_DEAD
            );
         }
      }
   }
   else if (IS_IMMORTAL(victim))
   {
      if (is_mental(ch))
      {
         if (can_see(victim, ch))
         {
            act_new
            (
               "$f mentally projects to you '$t'",
               ch,
               argument,
               victim,
               TO_VICT,
               POS_DEAD
            );
         }
         else
         {
            act_new
            (
               "$n mentally projects to you '$t'",
               ch,
               argument,
               victim,
               TO_VICT,
               POS_DEAD
            );
         }
      }
      else
      {
         if (can_see(victim, ch))
         {
            act_new
            (
               "$f tells you '$t'",
               ch,
               argument,
               victim,
               TO_VICT,
               POS_DEAD
            );
         }
         else
         {
            act_new
            (
               "$n tells you '$t'",
               ch,
               argument,
               victim,
               TO_VICT,
               POS_DEAD
            );
         }
      }
   }
   else if (IS_SET(victim->comm, COMM_ANSI))
   {
      if (is_mental(ch))
      {
         act_new
         (
            "$n mentally projects to you '\x01B[1;32m$t\x01B[0;37m'",
            ch,
            argument,
            victim,
            TO_VICT,
            POS_DEAD
         );
      }
      else
      {
         act_new
         (
            "$n tells you '\x01B[1;32m$t\x01B[0;37m'",
            ch,
            argument,
            victim,
            TO_VICT,
            POS_DEAD
         );
      }
   }
   else
   {
      if (is_mental(ch))
      {
         act_new
         (
            "$n mentally projects to you '$t'",
            ch,
            argument,
            victim,
            TO_VICT,
            POS_DEAD
         );
      }
      else
      {
         act_new
         (
            "$n tells you '$t'",
            ch,
            argument,
            victim,
            TO_VICT,
            POS_DEAD
         );
      }
   }
   victim->reply    = ch;

   return;
}

void do_freetell(CHAR_DATA* ch, char* argument )
{
   DESCRIPTOR_DATA* d;
   int count = 0;
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA* victim = NULL;

   argument = one_argument(argument, arg);

   if (arg[0] != '\0')
   {
      if
      (
         (
            victim = get_char_world(ch, arg)
         ) == NULL ||
         (
            IS_NPC(victim) &&
            victim->in_room != ch->in_room
         )
      )
      {
         send_to_char("They are not here.\n\r", ch);
         return;
      }
   }

   for (d = descriptor_list; d != NULL; d = d->next)
   {
      if
      (
         d->character != NULL &&
         d->character->reply == ch &&
         (
            d->character == victim ||
            victim == NULL
         )
      )
      {
         d->character->reply = NULL;
         count++;
      }
   }
   if (!IS_IMMORTAL(ch))
   {
      return;
   }
   if (count > 0)
   {
      send_to_char("All replies freed.\n\r", ch);
   }
   else
   {
      send_to_char("No replies to free.\n\r", ch);
   }
   return;
}


void do_yell(CHAR_DATA* ch, char* argument )
{
   DESCRIPTOR_DATA* d;
   CHAR_DATA* mob;
   CHAR_DATA* mob_next;
   ROOM_INDEX_DATA* room = ch->in_room;
   OBJ_DATA* brand;
   char original[MAX_INPUT_LENGTH];
   int chance;
   OBJ_DATA* cowsuit;

   cowsuit = get_eq_char(ch, WEAR_ABOUT);
   chance = (number_range(1, 10));

   if (is_affected(ch, gsn_mute))
   {
      send_to_char
      (
         "You try to yell, but only manage a weak rasping noise.\n\r",
         ch
      );
      return;
   }
   if (is_affected(ch, gsn_gag))
   {
      send_to_char("You cannot yell, you are gagged.\n\r", ch);
      return;
   }

   if (!IS_NPC(ch) && is_mental(ch))
   {
      if (ch->mana < 8 && !IS_IMMORTAL(ch))
      {
         send_to_char
         (
            "You do not have the mental energy to project thoughts.\n\r",
            ch
         );
         return;
      }
      else
      {
         ch->mana -= 8;
      }
   }

   if
   (
      is_affected(ch, gsn_silence) &&
      !is_mental(ch)
   )
   {
      send_to_char("You cannot find the words within you.\n\r", ch);
      return;
   }
   if
   (
      !IS_AWAKE(ch) &&
      IS_NPC(ch)
   )
   {
      return;
   }

   if (
      is_affected(ch, gsn_timestop) ||
      is_affected(ch, gsn_stone) ||
      is_affected(ch, gsn_freeze)
   )
   {
      send_to_char("You are frozen.\n\r", ch);
      return;
   }


   if (is_affected(ch, gsn_healing_trance))
   {
      send_to_char("You cease to meditate.\n\r", ch);
      affect_strip(ch, gsn_healing_trance);
   }

   if (is_affected(ch, gsn_stalking))
   {
      send_to_char("You stop stalking.\n\r", ch);
      affect_strip(ch, gsn_stalking);
   }

   if (IS_SET(ch->comm, COMM_NOSHOUT))
   {
      send_to_char("You cannot yell.\n\r", ch);
      return;
   }

   strcpy(original, argument);
   argument = original;

   if (is_affected(ch, gsn_toad))
   {
      strcpy(argument, "<ribbit>");
   }

   if
   (
      (
         cowsuit != NULL &&
         cowsuit->pIndexData->vnum == 80000
      ) &&
      chance == 1
   )
   {
      strcpy(argument, "Moo.");
   }

   if
   (
      is_affected(ch, gsn_chaos_mind) ||
      is_affected(ch, gsn_dancestaves)
   )
   {
      switch (dice(1, 6))
      {
         default:
         {
            break;
         }
         case (1):
         {
            strcpy(argument, "Help! My head is made of cheese!");
            break;
         }
         case (2):
         {
            strcpy(argument, "Um....What? Huh?");
            break;
         }
         case (3):
         {
            strcpy(argument, "What the hell was that!?!");
            break;
         }
         case (4):
         {
            strcpy(argument, "Hey, um...Where am I?");
            break;
         }
      }
   }

   if (argument[0] == '\0')
   {
      send_to_char( "Yell what?\n\r", ch );
      return;
   }

   un_hide(ch, "");

   /* Stop feigning death, 100% chance */
   stop_feign(ch, 100);
   if (IS_SET(ch->act2, PLR_LOG_SPEC))
   {
      log_convo(ch, NULL, TYPE_YELL, argument);
   }

   if
   (
      (brand = get_eq_char(ch, WEAR_BRAND)) &&
      brand->pIndexData->vnum == 15300
   )
   {
      send_to_char
      (
         "Your brand of the silent parrot burns as your voice yells"
         " across the region.\n\r",
         ch
      );
      damage(ch, ch, 5, gsn_parrot, DAM_PIERCE, TRUE);
      ch->exp -= 5;
      ch->move -= 25;
   }

   if (IS_SET(ch->comm, COMM_ANSI))
   {
      if (is_mental(ch))
      {
         act
         (
            "You mentally blast '\x01B[1;36m$t\x01B[0;37m'",
            ch,
            argument,
            NULL,
            TO_CHAR
         );
      }
      else
      {
         act("You yell '\x01B[1;36m$t\x01B[0;37m'", ch, argument, NULL, TO_CHAR);
      }
   }
   else
   {
      if (is_mental(ch))
      {
         act("You mentally blast '$t'", ch, argument, NULL, TO_CHAR);
      }
      else
      {
         act("You yell '$t'", ch, argument, NULL, TO_CHAR);
      }
   }
   for (d = descriptor_list; d != NULL; d = d->next)
   {
      if
      (
         !(
            d->connected == CON_PLAYING &&
            d->character != ch &&
            d->character->in_room != NULL &&
            d->character->in_room->area == ch->in_room->area &&
            !IS_SET(d->character->comm, COMM_QUIET) &&
            !is_ignored(ch, d->character)
         )
      )
      {
         continue;
      }
      if
      (
         is_affected(d->character, gsn_shock_sphere) &&
         !is_mental(ch)
      )
      {
         continue;
      }
      if (IS_SET(d->character->act2, PLR_LOG_SPEC))
      {
         log_convo(d->character, ch, TYPE_YELL2, argument);
      }

      if (is_mental(ch))
      {
         act_color
         (
            "$n mentally blasts '{B{4$t{n'",
            ch,
            argument,
            d->character,
            TO_VICT
         );
      }
      else
      {
         act_color
         (
            "$n yells '{B{4$t{n'",
            ch,
            argument,
            d->character,
            TO_VICT
         );
      }
   }
   if
   (
      !IS_NPC(ch) &&
      room != NULL
   )
   {
      for (mob = char_list; mob != NULL; mob = mob_next)
      {
         mob_next = mob->next;
         if
         (
            IS_NPC(mob) &&
            mob->in_room == room &&
            ch->in_room == room &&
            HAS_TRIGGER(mob, TRIG_SPEECH) &&
            mob->position == mob->pIndexData->default_pos
         )
         {
            mp_act_trigger(argument, mob, ch, NULL, NULL, TRIG_SPEECH);
         }
      }
   }

   return;
}


void do_myell(CHAR_DATA* ch, char* argument)
{
   DESCRIPTOR_DATA* d;
   CHAR_DATA* changelingswap;
   char buf [MAX_STRING_LENGTH];
   char original [MAX_INPUT_LENGTH];
   CHAR_DATA* mob;
   CHAR_DATA* mob_next;
   ROOM_INDEX_DATA* room = ch->in_room;

   if (ch->position <= POS_STUNNED)
   {
      return;
   }

   if (IS_SET(ch->comm, COMM_SILENCE))
   {
      send_to_char("You are unable to communicate.\n\r", ch);
      return;
   }

   if (IS_SET(ch->comm, COMM_NOSHOUT))
   {
      send_to_char("You cannot yell.\n\r", ch);
      return;
   }

   if (is_affected(ch, gsn_mute))
   {
      send_to_char
      (
         "You try to yell, but only manage a weak rasping noise.\n\r",
         ch
      );
      return;
   }
   if (is_affected(ch, gsn_gag))
   {
      send_to_char("You cannot yell, you are gagged.\n\r", ch);
      return;
   }

   if
   (
      !IS_NPC(ch) &&
      is_mental(ch)
   )
   {
      if (ch->mana < 8 && !IS_IMMORTAL(ch))
      {
         send_to_char
         (
            "You do not have the mental energy to project thoughts.\n\r",
            ch
         );
         return;
      }
      else
      {
         ch->mana -= 8;
      }
   }
   if
   (
      !IS_AWAKE(ch) &&
      IS_NPC(ch)
   )
   {
      return;
   }

   if
   (
      is_affected(ch, gsn_timestop) ||
      is_affected(ch, gsn_stone) ||
      is_affected(ch, gsn_freeze)
   )
   {
      send_to_char("You are frozen.\n\r", ch);
      return;
   }

   if
   (
      is_affected(ch, gsn_silence) &&
      !is_mental(ch)
   )
   {
      send_to_char("You cannot find the words within you.\n\r", ch);
      return;
   }
   strcpy(original, argument);
   argument = original;  /* No modifying the original argument */

   if
   (
      is_affected(ch, gsn_chaos_mind) ||
      is_affected(ch, gsn_dancestaves)
   )
   {
      switch (dice(1, 5))
      {
         default:
         {
            break;
         }
         case (1):
         {
            strcpy(argument, "Help! My head is made of cheese!");
            break;
         }
         case (2):
         {
            strcpy(argument, "Um....What? Huh?");
            break;
         }
         case (3):
         {
            strcpy(argument, "What the hell was that!?!");
            break;
         }
      }
   }

   if (argument[0] == '\0')
   {
      send_to_char("Yell what?\n\r", ch);
      return;
   }

   if (is_affected(ch, gsn_toad))
   {
      strcpy(argument, "<ribbit>");
   }
   un_hide(ch, "");
   /* Stop feigning death, 100% chance */
   stop_feign(ch, 100);
   if (IS_SET(ch->comm, COMM_ANSI))
   {
      if (is_mental(ch))
      {
         act
         (
            "You mentally blast in panic '\x01B[1;36m$t\x01B[0;37m'",
            ch,
            argument,
            NULL,
            TO_CHAR
         );
      }
      else
      {
         act
         (
            "You yell in panic '\x01B[1;36m$t\x01B[0;37m'",
            ch,
            argument,
            NULL,
            TO_CHAR
         );
      }
   }
   else
   {
      if (is_mental(ch))
      {
         act("You mentally blast '$t'", ch, argument, NULL, TO_CHAR);
      }
      else
      {
         act("You yell '$t'", ch, argument, NULL, TO_CHAR);
      }
   }
   changelingswap = ch->mprog_target;
   ch->mprog_target = NULL;

   for (d = descriptor_list; d != NULL; d = d->next)
   {
      if
      (
         !(
            d->connected == CON_PLAYING &&
            d->character != NULL &&
            d->character != ch &&
            d->character->in_room != NULL &&
            d->character->in_room->area == ch->in_room->area &&
            !IS_SET(d->character->comm, COMM_QUIET) &&
            !is_ignored(ch, d->character) &&
            (
               !is_affected(d->character, gsn_shock_sphere) ||
               is_mental(ch)
            )
         )
      )
      {
         continue;
      }

      if (IS_SET(d->character->act2, PLR_LOG_SPEC))
      {
         log_convo(d->character, ch, TYPE_YELL2, argument);
      }

      if
      (
         !IS_IMMORTAL(d->character) &&
         d->character->house != HOUSE_EMPIRE &&
         (
            d->character->house != HOUSE_ENFORCER ||
            (
               !IS_NPC(ch) &&
               IS_SET(ch->act2, PLR_LAWLESS)
            )
         )
      )
      {
         if (IS_SET(d->character->comm, COMM_ANSI))
         {
            if (is_mental(ch))
            {
               act
               (
                  "$n mentally blasts '\x01B[1;36m$t\x01B[0;37m'",
                  ch,
                  argument,
                  d->character,
                  TO_VICT
               );
            }
            else
            {
               act
               (
                  "$n yells '\x01B[1;36m$t\x01B[0;37m'",
                  ch,
                  argument,
                  d->character,
                  TO_VICT
               );
            }
         }
         else
         {
            if (is_mental(ch))
            {
               act
               (
                  "$n mentally blasts '$t'",
                  ch,
                  argument,
                  d->character,
                  TO_VICT
               );
            }
            else
            {
               act("$n yells '$t'", ch, argument, d->character, TO_VICT);
            }
         }
      }
      else
      {
         buf[0] = '\0';
         if
         (
            IS_SET(d->character->comm, COMM_ANSI) &&
            (
               !IS_NPC(ch) ||
               IS_AFFECTED(ch, AFF_CHARM)
            )
         )
         {
            if (is_mental(ch))
            {
               sprintf
               (
                  buf,
                  "$n mentally blasts in panic '\x01B[1;36m$t\x01B[0;37m'"
               );
            }
            else
            {
               sprintf(buf, "$n yells in panic '\x01B[1;36m$t\x01B[0;37m'");
            }
         }
         else
         {
            if (is_mental(ch))
            {
               sprintf(buf, "$n mentally blasts in panic '$t'");
            }
            else
            {
               sprintf(buf, "$n yells in panic '$t'");
            }
         }
         act(buf, ch, argument, d->character, TO_VICT);
      }
   }
   ch->mprog_target = changelingswap;
   if
   (
      !IS_NPC(ch) &&
      room != NULL
   )
   {
      for (mob = char_list; mob != NULL; mob = mob_next)
      {
         mob_next = mob->next;
         if
         (
            IS_NPC(mob) &&
            mob->in_room == room &&
            ch->in_room == room &&
            HAS_TRIGGER(mob, TRIG_SPEECH) &&
            mob->position == mob->pIndexData->default_pos
         )
         {
            mp_act_trigger(argument, mob, ch, NULL, NULL, TRIG_SPEECH);
         }
      }
   }
   return;
}


void do_emote(CHAR_DATA* ch, char* argument )
{
   if (IS_SET(ch->comm, COMM_SILENCE))
   {
      send_to_char("You are unable to communicate.\n\r", ch);
      return;
   }

   if (is_affected(ch, gsn_toad))
   {
      send_to_char("You cannot do this while in toad form.\n\r", ch);
      return;
   }

   if (is_affected(ch, gsn_healing_trance))
   {
      send_to_char("You cease to meditate.\n\r", ch);
      affect_strip(ch, gsn_healing_trance);
   }

   if (!IS_NPC(ch) && IS_SET(ch->comm, COMM_NOEMOTE))
   {
      send_to_char("You can't show your emotions.\n\r", ch);
      return;
   }

   if (argument[0] == '\0')
   {
      send_to_char("Emote what?\n\r", ch);
      return;
   }

   MOBtrigger = FALSE;
   if (argument[0] == '\'' && argument[1] == 's')
   {
      act("$n$T", ch, NULL, argument, TO_ROOM);
      act("$n$T", ch, NULL, argument, TO_CHAR);
   }
   else
   {
      act("$n $T", ch, NULL, argument, TO_ROOM);
      act("$n $T", ch, NULL, argument, TO_CHAR);
   }
   MOBtrigger = TRUE;
   return;
}


void do_pmote(CHAR_DATA* ch, char* argument)
{
   CHAR_DATA* vch;
   char* letter,*name;
   char last[MAX_INPUT_LENGTH], temp[MAX_STRING_LENGTH];
   unsigned int matches = 0;

   if (IS_SET(ch->comm, COMM_SILENCE))
   {
      send_to_char("You are unable to communicate.\n\r", ch);
      return;
   }

   if ( !IS_NPC(ch) && IS_SET(ch->comm, COMM_NOEMOTE) )
   {
      send_to_char( "You can't show your emotions.\n\r", ch );
      return;
   }

   if ( argument[0] == '\0' )
   {
      send_to_char( "Emote what?\n\r", ch );
      return;
   }

   if (argument[0] == '\'' && argument[1] == 's')
   act( "$n$t", ch, argument, NULL, TO_CHAR );
   else
   act( "$n $t", ch, argument, NULL, TO_CHAR );

   for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
   {
      if (vch->desc == NULL || vch == ch)
      continue;

      if (is_ignored(ch, vch)) continue;

      if ((letter = strstr(argument, vch->name)) == NULL)
      {
         MOBtrigger = FALSE;
         if (argument[0] == '\'' && argument[1] == 's')
         act("$N$t", vch, argument, ch, TO_CHAR);
         else
         act("$N $t", vch, argument, ch, TO_CHAR);
         MOBtrigger = TRUE;
         continue;
      }

      strcpy(temp, argument);
      temp[strlen(argument) - strlen(letter)] = '\0';
      last[0] = '\0';
      name = vch->name;

      for (; *letter != '\0'; letter++)
      {
         if (*letter == '\'' && matches == strlen(vch->name))
         {
            strcat(temp, "r");
            continue;
         }

         if (*letter == 's' && matches == strlen(vch->name))
         {
            matches = 0;
            continue;
         }

         if (matches == strlen(vch->name))
         {
            matches = 0;
         }

         if (*letter == *name)
         {
            matches++;
            name++;
            if (matches == strlen(vch->name))
            {
               strcat(temp, "you");
               last[0] = '\0';
               name = vch->name;
               continue;
            }
            strncat(last, letter, 1);
            continue;
         }

         matches = 0;
         strcat(temp, last);
         strncat(temp, letter, 1);
         last[0] = '\0';
         name = vch->name;
      }

      MOBtrigger = FALSE;
      if (argument[0] == '\'' && argument[1] == 's')
      act("$N$t", vch, temp, ch, TO_CHAR); else
      act("$N $t", vch, temp, ch, TO_CHAR);
      MOBtrigger = TRUE;
   }

   return;
}


/*****************************[ CLASS POSES ]**************************

The Pose Table is structured, by class, starting with class "0"
which is Warrior. Here is a list indicating the class order within
the table, for future reference, and also the credits for the new
poses that were created:

# Class Name      Pose Credits
------------------------------------------------------------------
0 WARRIOR       ROM Default (Modified/expanded by Wicket Warrick)
1 THIEF         ROM Default (Modified/expanded by Wicket Warrick)
2 CLERIC        ROM Default (Modified/expanded by Wicket Warrick)
3 PALADIN       Xurinos, Hector Gellidus & Wicket Warrick
4 ANTI-PALADIN  Hector Gellidus & Wicket Warrick
5 RANGER        Hector Gellidus
6 MONK          Shamutanti Theceta & Wicket Warrick
7 CHANNELER     Xyza & Wicket Warrick
8 NIGHTWALKER   Wicket Warrick
9 NECROMANCER   Utara Stonehands & Wicket Warrick
10 ELEMENTALIST  Wicket Warrick

*******************[ Revised by Wicket Warrick (1999) ]***************/

struct    pose_table_type
{
   char*     message[2*MAX_CLASS];
};

const    struct    pose_table_type    pose_table    []    =
{
   {
      {
         "You show your bulging muscles.",
         "$n shows $s bulging muscles.",
         "You perform a small card trick.",
         "$n performs a small card trick.",
         "You feel very holy.",
         "$n looks very holy.",
         "A halo of light surrounds you as the divine show their favor.",
         "A halo of light surrounds $n as the divine show their favor.",
         "You draw a pentagram in the ground with a bloody finger and utter arcane words to your dark lord.",
         "$n draws a pentagram in the ground with a bloody finger and utters arcane words to $s dark lord.",
         "A sense of joy comes over you as Thera's woodlands call to you from afar.",
         "A sense of joy comes over $n as Thera's woodlands call to $m from afar.",
         "Knowledge of the martial arts dances in your mind.",
         "Knowledge of the martial arts dances in $n's mind.",
         "You sizzle with energy.",
         "$n sizzles with energy.",
         "An aura of darkness surrounds you.",
         "An aura of darkness surrounds $n.",
         "You emanate a dark red aura.",
         "$n emanates a dark red aura.",
         "Sparks fly from your fingers, and fizzle as they fall to the ground.",
         "Sparks fly from $n's fingers, and fizzle as they fall to the ground."
      }
   },

   {
      {
         "You crack nuts between your fingers.",
         "$n cracks nuts between $s fingers.",
         "You wiggle your ears alternately.",
         "$n wiggles $s ears alternately.",
         "You nonchalantly turn wine into water.",
         "$n nonchalantly turns wine into water.",
         "A divine hand points out to you the path to righteousness.",
         "A divine hand points out to $n the path to righteousness.",
         "A shroud of darkness surrounds you as you draw upon dark energy.",
         "A shroud of darkness surrounds $n as $e draws upon dark energy.",
         "You touch your finger to the ground and a small flower sprouts.",
         "$n touches $s finger to the ground and a small flower sprouts.",
         "You sit cross legged and begin to mediate, slowly rising an inch off the ground.",
         "$n sits cross legged and begins to mediate, slowly rising an inch off the ground.",
         "You glow brightly with a blue-green hue.",
         "$n glows brightly with a blue-green hue.",
         "The sun temporarily blinds you with its foul rays of light.",
         "The sun temporarily blinds $n with its foul rays of light.",
         "Flowers begin to wilt from your mere presence.",
         "Flowers begin to wilt from $n's mere presence.",
         "Water sprites splash you playfully.",
         "Water sprites splash $n playfully."
      }
   },

   {
      {
         "You grizzle your teeth and look mean.",
         "$n grizzles $s teeth and looks mean.",
         "You nimbly tie yourself into a knot.",
         "$n nimbly ties $mself into a knot.",
         "A halo appears over your head.",
         "A halo appears over $n's head.",
         "A peasant comes forth, asking you for a blessing.",
         "A peasant comes forth, asking $n for a blessing.",
         "You quench your thirst with the blood of innocents.",
         "$n quenches $s thirst with the blood of innocents.",
         "Small birds circle overhead as you commune with nature.",
         "Small birds circle overhead as $n communes with nature.",
         "You kick into the air, your foot just missing the faces of those around you.",
         "$n kicks into the air, $s foot just missing your face!",
         "Your hands are wreathed with lightning.",
         "$n's hands are wreathed with lightning.",
         "Your head disappears into the shadows, but you seem to have left your body behind?",
         "$n's head disappears into the shadows, but $e seems to have left $s body behind.",
         "You quaff a vial of poison and watch in delight as your body erupts with sores.",
         "$n quaffs a vial of poison and watches in delight as $s body erupts with sores.",
         "A baby air elemental forms before you, licks its finger, and sticks it in your ear!",
         "A baby air elemental forms in front of $n, licks its finger, and sticks it in $s ear!"
      }
   },

   {
      {
         "You hit your head, and your eyes roll.",
         "$n hits $s head, and $s eyes roll.",
         "You steal the underwear off every person in the room.",
         "Your underwear is gone!  $n stole it!",
         "You recite words of wisdom.",
         "$n recites words of wisdom.",
         "Your armor blazes brightly in the sunlight.",
         "$n's armor blazes brightly in the sunlight.",
         "You draw your dark cloak about you and a chill permeates the air.",
         "$n draws $s dark cloak about $mself and a chill permeates the air.",
         "Your eyes flare brightly with inner health.",
         "$n's eyes flare brightly with inner health.",
         "You quietly sneak behind everyone and tickle them behind the ear.",
         "Someone is tickling you behind the ear!",
         "A small ball of light dances on your fingertips.",
         "A small ball of light dances on $n's fingertips.",
         "You purposely drop your blade and slay a lost kitten.",
         "$n purposely drops $s blade and slays a lost kitten.",
         "With an old rusty dagger, you slice open your forearm splattering everyone with blood.",
         "With an old rusty dagger, $n slices open $s forearm splattering you with blood.",
         "Tiny earth sprites tie your laces together, and tickle you mercilessly.",
         "Tiny earth sprites tie $n's laces together, and tickle $m mercilessly."
      }
   },

   {
      {
         "Crunch, crunch -- you munch a bottle.",
         "Crunch, crunch -- $n munches a bottle.",
         "The dice roll ... and you win again.",
         "The dice roll ... and $n wins again.",
         "Deep in prayer, you levitate.",
         "Deep in prayer, $n levitates.",
         "The world almost seems to turn inside out as you pass by, dispelling the evil within the shadows.",
         "The world almost seems to turn inside out as $n passes by, dispelling the evil within the shadows.",
         "The candles within the local brothel seem to flicker furiously as you pass by.",
         "The candles within the local brothel seem to flicker furiously as $n passes by.",
         "You hum a small tune and the insects around you hum along.",
         "$n hums a small tune and the insects around $m hum along.",
         "You leap into the air and do a double backflip.",
         "$n leaps into the air and does a double backflip.",
         "With a single word, spells seem to liquify and slide away from you.",
         "With a single word, spells seem to liquify and slide away from $n.",
         "A trail of rats seem to follow you as you stalk your next victim.",
         "A trail of rats seem to follow $n as $e stalks $s next victim... you!",
         "You take a deep draw from a flask of blood.",
         "$n takes a deep draw from a flask of what appears to be blood.",
         "A wild flame erupts before your eyes, searing your eyebrows!",
         "A wild flame erupts in $n's face and sears off $s eyebrows!"
      }
   },

   {
      {
         "... 98, 99, 100 ... you do pushups.",
         "... 98, 99, 100 ... $n does pushups.",
         "You count the money in everyone's pockets.",
         "Check your money, $n is counting it.",
         "Your spell goes awry and you are blinded! It quickly passes, and you can see again.",
         "$n's spell goes awry and $e is blinded! It quickly passes, and $e can see again.",
         "A shield of stars surrounds you as you prepare to face the darkness.",
         "A shield of stars surrounds $n as $e prepares to face the darkness.",
         "You scream in frustration thinking of all the cursed paladins roaming the lands.",
         "$n screams in frustration thinking of all the cursed paladins roaming the lands.",
         "You mix together a herbal poultice and heal the wounds of a fallen sparrow.",
         "$n mixes together a herbal poultice and heals the wounds of a fallen sparrow.",
         "Your karate chop splits a tree.",
         "$n's karate chop splits a tree.",
         "You wave your hands and create a show of colorful lights swirling above everyone's head.",
         "$n waves $s hands and creates a show of colorful lights swirling above your head.",
         "The shadows of the night crawl upon you and hide your presence.",
         "The shadows of the night crawl upon $n and hide $s presence.",
         "A cute little bunny begins to convulse and foam at the mouth after meeting your gaze.",
         "A cute little bunny meets $n's gaze and begins to foam at the mouth and convulse.",
         "You sneeze and release a searing stream of flame beneath a grazing cow, sending it running up the nearest tree!",
         "$n sneezes and releases a searing stream of flame beneath a grazing cow, sending it running up the nearest tree!"
      }
   },

   {
      {
         "Watch your feet, you are juggling granite boulders.",
         "Watch your feet, $n is juggling granite boulders.",
         "You balance a pocket knife on your tongue.",
         "$n balances a pocket knife on $s tongue.",
         "Your body glows with an unearthly light.",
         "$n's body glows with an unearthly light.",
         "An elderly blind man beckons to you to free him from his ailment.",
         "An elderly blind man beckons to $n to free him from his ailment.",
         "You curse a brand new quill and hand it a young student in need, burning it into her hands.",
         "$n curses a brand new quill and hands it a young student in need, burning it into her hands.",
         "A forest sprite flies up to you and whispers secrets in your ear.",
         "A forest sprite flies up to $n and whispers secrets in $s ear.",
         "A shadow grabs hold of your leg, beckoning you to hide with it.",
         "A shadow grabs hold of $n's leg, beckoning $m to hide with it.",
         "You smile with delight as lightning dances between two wands in your hands.",
         "$n smiles with delight as lightning dances between two wands in $s hands.",
         "A small green flame erupts in your palm, summoning shadows to dance upon your darkened face.",
         "A small green flame erupts in $n's palm, summoning shadows to dance upon $s darkened face.",
         "The skeletal remains of past victims appear before you, and begin to chant rhymes of death!",
         "The skeletal remains of past victims appear before $n, and begin to chant rhymes of death!",
         "Fire sprites dance wildly in your eyes.",
         "Fire sprites dance wildly in $n's eyes."
      }
   },

   {
      {
         "Everyone is swept off their feet by your hug.",
         "You are swept off your feet by $n's hug.",
         "... 63, 64, 65 ... you count the stolen keys on your key ring.",
         "... 63, 64, 65 ... $n counts the stolen keys on $s key ring.",
         "Everyone levitates as you pray.",
         "You levitate as $n prays.",
         "A beautiful merchant girl offers you a white rose from her cart.",
         "A beautiful merchant girl offers $n a white rose from her cart.",
         "A decrepit old wench offers you the stem of a diseased black rose from her basket.",
         "A decrepit old wench offers $n the stem of a diseased black rose from her basket.",
         "The wind picks up as you concentrate upon natural order.",
         "The wind picks up as $n concentrates upon natural order.",
         "With ease, you drop and do the splits.",
         "With ease, $n drops and does the splits.",
         "A young student of channeling asks you to teach her the proper method of empowering weapons.",
         "A young student of channeling asks $n to teach her the proper method of empowering weapons.",
         "You are entangled in a web of silken darkness.",
         "$n is entangled in a web of silken darkness.",
         "You drink a bottle of embalming fluid and fall over drunk. *hic*",
         "$n drinks a bottle of embalming fluid, and falls over drunk.",
         "Shards of ice instantly vaporize at your touch.",
         "Shards of ice instantly vaporize at $n's touch."
      }
   },

   {
      {
         "Oomph!  You squeeze water out of a granite boulder.",
         "Oomph!  $n squeezes water out of a granite boulder.",
         "A heavy looking sack slips out of your pocket, but you swiftly catch it in your hand.",
         "A heavy looking sack slips out of $n's pocket, but $e swiftly catches it in $s hand.",
         "A cool breeze refreshes you.",
         "A cool breeze refreshes $n.",
         "You remove the curse from a brand new quill and return it to a young student, instantly healing her burnt hands.",
         "$n removes the curse from a brand new quill and returns it to a young student, instantly healing her burnt hands.",
         "A finger of one of your past training mates falls from your robes.",
         "A finger of one of $n's past training mates falls from $s robes.",
         "A large bear lumbers up to you and does tricks as you toss it fish!",
         "A large bear lumbers up to $n and does tricks as $e tosses it fish!",
         "Everyone's nerve points tingle as you focus your mind.",
         "Your nerve points tingle as $n focuses $s mind.",
         "You throw an icelance high into the air, causing it to explode with a single burst of flame.",
         "$n throws an icelance high into the air, causing it to explode with a single burst of flame.",
         "You lick the excess venom off of your poisoned blade.",
         "$n licks the excess venom off of $s poisoned blade.",
         "You casually summon a lesser demon from the astral plane to wipe the sweat from your brow.",
         "$n casually summons a lesser demon from the astral plane to wipe the sweat from $s brow.",
         "With the wave of your hand, your water jug crackles loudly as it turns to ice.",
         "With the wave of $s hand, $n's water jug crackles loudly as it turns to ice."
      }
   },

   {
      {
         "You pick your teeth with a spear.",
         "$n picks $s teeth with a spear.",
         "You produce a coin from everyone's ear.",
         "$n produces a coin from your ear.",
         "A barrier of blades appear above your head and spin wildy, chopping fresh vegetables for everyone.",
         "A barrier of blades appear above $n's head and spin wildy, chopping fresh vegetables for you.",
         "A man in a monkey suit approaches you and bows.",
         "A man in a monkey suit approaches $n and bows.",
         "You draw your sword across your palm and taint it with your blood.",
         "$n draws $s sword across $s palm and taints it with $s blood.",
         "You proudly display your collection of animal skins!",
         "$n proudly displays $s collection of animal skins!",
         "Your Kaiden decides to test your focus and hits you in the stomach with a pole.",
         "A Kaiden decides to test $n's focus and hits $m in the stomach with a pole.",
         "A bundle of powerless wands suddenly sparkle to life as you channel your mind and charge them once again.",
         "A bundle of powerless wands suddenly sparkle to life as $n channels $s mind and charges them once again.",
         "You play hide and seek with your shadowself.",
         "$n plays hide and seek with $s shadowself.",
         "Gazing at a darkened sky, you envision a world of pure death and drool on everyone near you.",
         "Gazing at a darkened sky, $n envisions a world of pure death and drools all over you.",
         "The winds howl loudly as your arcane words fill the air.",
         "The winds howl loudly as $n's arcane words fill the air."
      }
   },

   {
      {
         "You shake a large apple tree, causing all the apples to fall to the ground.",
         "$n shakes a large apple tree, causing all the apples to fall to the ground.",
         "The air is filled with the sound of locks falling from the very doors you just blessed with your presence.",
         "The air is filled with the sound of locks falling from the very doors $n just blessed with $s presence.",
         "The sun pierces through the clouds to illuminate you.",
         "The sun pierces through the clouds to illuminate $n.",
         "Your sword flashes brightly, driving back the darkness!",
         "$n's sword flashes brightly, driving back the darkness!",
         "The icy chill of despair fills the heart of those surrounding you as you glare at them hauntingly.",
         "The icy chill of despair fills your heart as $n glares at you hauntingly.",
         "You tame a raging baboon that wanders into your presence!",
         "$n tames a raging baboon that wanders into $s presence!",
         "You karate chop a 10 inch thick board, sending splinters everywhere.",
         "$n karate chops a 10 inch thick board, sending splinters everywhere.",
         "Everything around you begins to glow as you recite a familiar spell.",
         "Everything around you begins to glow as $n recites a familiar spell.",
         "You hear someone's death cry and grin evilly at the thought of one less lightwalker.",
         "You hear someone's death cry and $n grins evilly at the thought of one less lightwalker.",
         "Darkness envelops you, even in the open daylight.",
         "Darkness envelops $n, even in the open daylight.",
         "Your body begins to glow brightly, fueled with the energy of the Elements.",
         "$n's body begins to glow brightly, fueled with the energy of the Elements."
      }
   },

   {
      {
         "A strap of your armor breaks over your mighty thews.",
         "A strap of $n's armor breaks over $s mighty thews.",
         "You step behind your shadow.",
         "$n steps behind $s shadow.",
         "The ocean parts before you.",
         "The ocean parts before $n.",
         "An aura of light surrounds you, warming everything nearby.",
         "An aura of light surrounds $n, warming everything nearby.",
         "You whisper words of deception to an innocent child.",
         "$n whispers words of deception to an innocent child.",
         "You produce healthy fruit from everyone's ear.",
         "$n produces healthy fruit from your ear!",
         "A dwarven child throws a rock at your head, but you block it with little effort.",
         "A dwarven child throws a rock at $n's head, but $e blocks it with little effort.",
         "Everyone's clothing beckons to you, seeking to offer their energy for your wielding.",
         "Your clothing beckons to $n, seeking to offer their energy for $s wielding.",
         "The shadow plane beckons to you.",
         "The shadow plane beckons to $n.",
         "You glare at everyone in the room and ponder who controls such odd looking corpses.",
         "$n glares at you, as if you were a corpse, and seems to be pondering who controls you.",
         "Air sprites swirl around you, blowing the dust from your arcane books.",
         "Air sprites swirl around $n, blowing the dust from $s arcane books."
      }
   },

   {
      {
         "A boulder cracks at your frown.",
         "A boulder cracks at $n's frown.",
         "Your eyes dance with greed.",
         "$n's eyes dance with greed.",
         "A thunder cloud kneels to you.",
         "A thunder cloud kneels to $n.",
         "Thunder sounds in answer to your blessed prayer.",
         "Thunder sounds in answer to $n's blessed prayer.",
         "Hoards of young paladins run from your path, intimidated by your mere presence.",
         "Hoards of young paladins run from $n's path, intimidated by $s mere presence.",
         "You sing to a savage beast and tame it with your voice.",
         "$n sings to a savage beast and tames it with $s voice.",
         "You chant softly into the wind.",
         "$n chants softly into the wind.",
         "Rain falls from the skies above, turning into streams of flame as you absorb their energy.",
         "Rain falls from the skies above, turning into streams of flame as $n absorbs their energy.",
         "You thirst for the supple energy of those cursed souls who call themselves lightwalkers.",
         "$n thirsts for the supple energy of those cursed souls who call themselves lightwalkers.",
         "Your acid experiment boils over spewing onto everyone around you, leaving large welts in their skin.",
         "$n's acid experiment boils over spewing onto you, leaving large welts in your skin!",
         "The Elemental Guardian appears before you, bows deeply, and returns to the canyon.",
         "The Elemental Guardian appears before $n, bows deeply, and returns to the canyon."
      }
   },

   {
      {
         "The gods admire your physique.",
         "The gods admire $n's physique.",
         "You deftly steal everyone's weapon.",
         "$n deftly steals your weapon.",
         "A healer adept whispers secrets of the healing arts in your ear.",
         "A healer adept whispers secrets of the healing arts in $n's ear.",
         "A maiden drops her morning laundry upon your head from above, and blushes innocently realizing what she had done.",
         "A maiden drops her morning laundry upon $n's head from above, and blushes innocently realizing what she had done.",
         "Your eyes become pools of darkness, absorbing the light ruthlessly.",
         "$n's eyes become pools of darkness, absorbing the light ruthlessly.",
         "The forest beckons to you for consultation.",
         "The forest beckons to $n for consultation.",
         "A gust of wind catches your robe and flings it over your head revealing *ALL*",
         "A gust of wind catches $n's robe and flings it over $s head revealing *ALL*",
         "A small army of animated ice-cream and cookies arrives to serve you.",
         "A small army of animated ice-cream and cookies arrives to serve $n.",
         "The forces of evil lurk from within every alley, and behind every turn, ready to do your bidding.",
         "The forces of evil lurk from within every alley, and behind every turn, ready to do $n's bidding.",
         "You casually imbue a bag of food with magical poison before donating it to the poor.",
         "$n imbues a bag of food with magical poison before donating it to the poor.",
         "The rains ease, and fireworks fill the skies at your command.",
         "The rains ease, and fireworks fill the skies at $n's command."
      }
   },

   {
      {
         "You stomp your foot hard, sending granite boulders leaping high into the air.",
         "$n stomps $s foot hard, sending granite boulders leaping high into the air.",
         "Dorgar, the minotaur thief, buys you a beer.",
         "Dorgar, the minotaur thief, buys $n a beer.",
         "A young child with acid burns beckons you for aid.. you place your palm upon her head and she is instantly healed.",
         "A young child with acid burns beckons $n for aid.. $e places $s palm upon her head and she is instantly healed.",
         "Townsfolk meet your eyes with a smile, thankful for your honorable ways.",
         "Townsfolk meet $n's eyes with a smile, thankful for $s honorable ways.",
         "You give forth a great battlecry and demons of damned souls echo you.",
         "$n gives forth a great battlecry and demons of damned souls echo $m.",
         "Druids form a circle around you and chant softly.",
         "Druids form a circle around $n and chant softly.",
         "You dazzle everyone as you perform one of your school's katas.",
         "$n dazzles you as $e performs one of $s school's katas.",
         "You snicker in amusement as a small ball of light dances at everyone's ear.",
         "$n snickers with amusement as a small ball of light dances at your ear.",
         "A mirror shatters as you gaze into it with eyes of pure evil.",
         "A mirror shatters as $n gazes into it with eyes of pure evil.",
         "You create a fleshy golem from the severed body parts of your closest friends.",
         "$n creates a fleshy golem from the severed body parts of $s closest friends.",
         "A small rain cloud forms above you, washing away the soil from your robes.",
         "A small rain cloud forms above $n, washing away the soil from $s robes."
      }
   },

   {
      {
         "A flock of seagulls erupt from a nearby tree, as your warcry shakes the ground beneath it.",
         "A flock of seagulls erupt from a nearby tree, as $n's warcry shakes the ground beneath it.",
         "Everyone's pocket explodes with your fireworks.",
         "Your pocket explodes with $n's fireworks!",
         "An angel consults you.",
         "An angel consults $n.",
         "Demons leap out of the ground in fear and scatter before your gaze.",
         "Demons leap out of the ground in fear and scatter before $n's gaze.",
         "You glare balefully at a young paladin that walks by, causing him to soil his breeches.",
         "$n glares balefully at a young paladin that walks by, causing him to soil his breeches.",
         "A soft rain begins as you sing a song of nature.",
         "A soft rain begins as $n sings a song of nature.",
         "Your eyes crackle with your inner Chi.",
         "$n's eyes crackle with $s inner Chi.",
         "With a mischievous look, you utter a word, and everything around you stops as time suddenly grinds to a halt.",
         "With a mischievous look, $n utters a word, and everything around you stops as time suddenly grinds to a halt.",
         "You slay a young elven child in honor of the Gods of Darkness.",
         "$n slays a young elven child in honor of the Gods of Darkness.",
         "You magically heat an iron poker before slowly taking the eyes from a young elf.",
         "$n heats an iron poker with magical heat before slowly taking the eyes from a young elf.",
         "The earth trembles in tune with your voice.",
         "The earth trembles in tune with $n's voice."
      }
   },

   {
      {
         "Mercenaries arrive to do your bidding.",
         "Mercenaries arrive to do $n's bidding.",
         "Everyone discovers your dagger a centimeter from their eye.",
         "You discover $n's dagger a centimeter from your eye!",
         "Sylvia offers you a glass of water.",
         "Sylvia offers $n a glass of water.",
         "The ground rumbles in agreement with your holy proclamation.",
         "The ground rumbles in agreement with $n's holy proclamation.",
         "Your sword glows a hateful red as you seek to rid the world of putrid honor.",
         "$n's sword glows a hateful red as $e seeks to rid the world of putrid honor.",
         "Flowers grow in your tracks after you walk by.",
         "Flowers grow in $n's tracks after $e walks by.",
         "You move with the fluidity and grace of running water.",
         "$n moves with the fluidity and grace of running water.",
         "To your delight, you utter a few words, sending a rainbow of light arcing from your open palm.",
         "To $s delight, $n utters a few words, sending a rainbow of light arcing from $s open palm.",
         "A cold smile envelops your face as you remember the horrid nightmare that you experienced last eve.",
         "A cold smile envelops $n's face as $e remembers the horrid nightmare that $e experienced last eve.",
         "The sky grows a little darker as you begin to concentrate on your dark magics.",
         "The sky grows a little darker as $n begins to concentrate on $s dark magics.",
         "You pay tribute to the great Elemancers of the past.",
         "$n pays tribute to the great Elemancers of the past."
      }
   },

   {
      {
         "Four matched Percherons bring in your chariot.",
         "Four matched Percherons bring in $n's chariot.",
         "Where did you go?",
         "Where did $n go?",
         "With a single brandish of your staff, a bottle of deadly poison turns into vapor and vanishes.",
         "With a single brandish of $s staff, $n turns a bottle of deadly poison into vapor and it vanishes.",
         "A horned demon drops his sword in awe of your presence and fades out of existence.",
         "A horned demon drops his sword in awe of $n's presence and fades out of existence.",
         "Your jaw begins to grind with disgust as you see an elderly woman being aided by a young lightwalker.",
         "$n's jaw begins to grind with disgust as $e sees an elderly woman being aided by a young lightwalker.",
         "Your staff blazes brightly as you gesture toward the correct path.",
         "$n's staff blazes brightly as $e gestures toward the correct path.",
         "Your palms quiver loudly, attracting the attention of your peers.",
         "$n's palms quiver loudly, attracting the attention of everyone around $m.",
         "You hover and meditate within a sphere of pure energy.",
         "$n hovers and meditates within a sphere of pure energy.",
         "You glare around the room, searching for a soul to entrap within your evil grasp.",
         "$n glares around the room, searching for a soul to entrap within $s evil grasp.",
         "You summon a greater demon from the abyss to do your bidding, then dismiss it with a casual wave.",
         "$n summons a greater demon from the abyss to do $s bidding, then casually dismisses it with a casual wave.",
         "Tiny elemental sprites swirl around you, singing praise to Lady Xyza.",
         "Tiny elemental sprites swirl around $n, singing praise to Lady Xyza."
      }
   },

   {
      {
         "The sun reflects off of your blade and illuminates your entire body, revealing your absolute power!",
         "The sun reflects off of $n's blade and illuminates $s entire body, revealing $s absolute power!",
         "Click.",
         "Click.",
         "Everything around you begins to glow brightly with good health as you pass by.",
         "Everything around you begins to glow brightly with good health as $n passes by.",
         "Your eyes glow pure white with righteous energy!",
         "$n's eyes glow pure white with righteous energy!",
         "A white dove falls to the ground dead as you gaze toward the skies, interrupting its peaceful flight.",
         "A white dove falls to the ground dead as $n gazes toward the skies, interrupting its peaceful flight.",
         "Thunder rumbles in the sky as you stand up tall.",
         "Thunder rumbles in the sky as $n stands up tall.",
         "The world around you seems to quiver as your mind and spirit harmonize within.",
         "The world around you seems to quiver as $n's mind and spirit harmonize within.",
         "Thera's magical energies surge through you, encasing you in raw arcane power!",
         "Thera's magical energies surge through $n, encasing $m in raw arcane power!",
         "Your dark magics surge from within, and fill your every word with utter fear.",
         "$n's dark magics surge from within, and fill $s every word with utter fear.",
         "An entire legion of undead rise from the graveyard to do your bidding.",
         "An entire legion of undead rise from the graveyard to do $n's bidding.",
         "The world begins to shiver as you surge with elemental power!",
         "The world begins to shiver as $n surges with elemental power!"
      }
   }
};

void do_pose( CHAR_DATA* ch, char* argument )
{
   int level;
   int pose;
   int x;

   if (IS_NPC(ch))
   return;

   if (ch->level > 51)
   level = 18;
   else if (ch->level < 3)
   level = 1;
   else
   {
      x = ch->level;
      while (x %3 != 0)
      {
         x--;
      }
      level = (x / 3) + 1;
   }

   pose  = number_range(0, level);

   act( pose_table[pose].message[2*ch->class+0], ch, NULL, NULL, TO_CHAR );
   act( pose_table[pose].message[2*ch->class+1], ch, NULL, NULL, TO_ROOM );

   return;
}

/***************************[ End of Posing Section ]************************/


void do_bug( CHAR_DATA* ch, char* argument )
{
   append_file( ch, BUG_FILE, argument );
   send_to_char( "Bug logged.\n\r", ch );
   return;
}

void do_typo( CHAR_DATA* ch, char* argument )
{
   append_file( ch, TYPO_FILE, argument );
   send_to_char( "Typo logged.\n\r", ch );
   return;
}

void do_rent( CHAR_DATA* ch, char* argument )
{
   send_to_char( "There is no rent here.  Just save and quit.\n\r", ch );
   return;
}


void do_qui( CHAR_DATA* ch, char* argument )
{
   send_to_char( "If you want to QUIT, you have to spell it out.\n\r", ch );
   return;
}


void extract_quitting_objects(CHAR_DATA* ch)
{
   OBJ_DATA* obj;
   OBJ_DATA* obj_next;

   /*
      Remove any House items, owners
   */
   for (obj = object_list; obj != NULL; obj = obj_next)
   {
      obj_next = obj->next;

      if
      (
         obj->pIndexData->vnum == OBJ_VNUM_PHYLACTERY &&
         !str_cmp(obj->owner, ch->name)
      )
      {
         extract_obj(obj, FALSE);
         continue;
      }
      if (obj->carried_by == ch)
      {
         if
         (
            obj->pIndexData->house ||
            (
               str_cmp(obj->owner, "none") &&
               str_cmp(obj->owner, ch->name)
            )
         )
         {
            extract_obj(obj, FALSE);
         }
         /*handle - crumble timers for quit out timers -werv */
         else if (obj->timer < 0)
         {
            obj->timer++;
            if (obj->timer == 0)
            {
               if (obj->pIndexData->vnum == OBJ_VNUM_RING_PROTECT)
               {
                  affect_strip(ch, gsn_protective_shield);
               }
               extract_obj(obj, FALSE);
            }
         }
         else
         {
            site_check_from(ch, obj);
         }
      }
   }
}

void do_quit(CHAR_DATA* ch, char* argument)
{
   DESCRIPTOR_DATA* d;
   DESCRIPTOR_DATA* d_next;
   int id;
   CHAR_DATA* wch;
   CHAR_DATA* ich;
   CHAR_DATA* wch_next;
   int pos;

   if (IS_NPC(ch))
   {
      return;
   }

   if (IS_AFFECTED(ch, AFF_CHARM))
   {
      send_to_char("Now why would you want to leave your master?\n\r", ch);
      return;
   }

   if
   (
      /* Level is 0 when deleting */
      ch->level > 0 &&
      ch->desc != NULL &&
      !IS_IMMORTAL(ch) &&
      !IS_SET(ch->act, PLR_DENY) &&
      ch->pcdata->death_status != HAS_DIED &&
      ch->quittime >= 0  /* voiding out will be -1 */
   )
   {
      if (ch->position == POS_FIGHTING)
      {
         send_to_char("No way! You are fighting.\n\r", ch);
         return;
      }

      if (ch->position  < POS_STUNNED)
      {
         send_to_char("You are not DEAD yet.\n\r", ch);
         return;
      }
      if (ch->pause > 0)
      {
         send_to_char("Your adrenaline is gushing too much to quit!\n\r", ch);
         return;
      }
      if (ch->quittime > 0)
      {
         send_to_char("You do not feel safe enough to quit yet!\n\r", ch);
         return;
      }
      if (is_affected(ch, gsn_blackjack))
      {
         send_to_char("You are blackjacked, you cannot quit!\n\r", ch);
         return;
      }
      if (is_affected(ch, gsn_strangle))
      {
         send_to_char("You are strangled, you cannot quit!\n\r", ch);
         return;
      }
      if
      (
         ch->in_room->house != 0 &&
         ch->in_room->house != ch->house &&
         (
            ch->in_room->house != HOUSE_ANCIENT ||
            !IS_SET(ch->act2, PLR_IS_ANCIENT_KNOWN)
         )
      )
      {
         send_to_char("You cannot quit out in an enemy House!\n\r", ch);
         return;
      }
   }
   /* Remember position, stop_fighting makes you stand */
   pos = ch->position;
   stop_fighting(ch, TRUE);
   ch->position = pos;
   if (ch->morph_form[0] == MORPH_MIMIC)
   {
      affect_strip(ch, gsn_shapeshift);
      ch->morph_form[0] = 0;
      ch->morph_form[1] = 0;
      ch->morph_form[2] = 0;
      ch->mprog_target = NULL;
   }

   if (ch->house != 0)
   {
      announce_logout(ch);
   }
   send_to_char
   (
      "Alas, all good things must come to an end.\n\r",
      ch
   );
   for (ich = ch->in_room->people; ich != NULL; ich = ich->next_in_room)
   {
      if
      (
         !(
            !can_see(ich, ch) &&
            IS_IMMORTAL(ch)
         )
      )
      {
         act_new
         (
            "$n has left the realm.",
            ch,
            ch,
            ich,
            TO_VICT,
            POS_RESTING
         );
      }
   }
   sprintf
   (
      log_buf,
      "%s@%s has quit.",
      ch->name,
      (
         ch->desc ?
         ch->desc->host :
         ch->host ?
         ch->host :
         "???"
      )
   );
   log_string(log_buf);
   wiznet
   (
      "$N rejoins the real world.",
      ch,
      NULL,
      WIZ_LOGINS,
      0,
      get_trust(ch)
   );
   /*
   Update roster, if quitting for storage, being_restored is true
   so should tell the roster storage is happening
   */
   remove_from_rosters(ch);
   if (ch->level > 1)
   {
      if (ch->level < 10)
      {
         /* Remove any limiteds on levels 10 and below */
         strip_limits(ch->carrying);
      }
      /*
      Level 1 characters voiding out/quitting are deleted
      Remove them from rosters, and do not put them back in
      */
      update_roster(ch, ch->pcdata->being_restored);
   }
   else
   {
      /* Level <= 1, remove all items */
      while (ch->carrying)
      {
         extract_obj(ch->carrying, FALSE);
      }
   }
   /*
   After extract_char the ch is no longer valid!
   */

   extract_quitting_objects(ch);

   /* extract all decoys */
   for (wch = char_list; wch != NULL; wch = wch_next)
   {
      wch_next = wch->next;
      if
      (
         wch->defending!= NULL &&
         wch->defending == ch
      )
      {
         /* Cleardefend */
         wch->defending  =  NULL;
      }
      /* Clear nightfall targets */
      if
      (
         wch->nightfall != NULL &&
         wch->nightfall == ch
      )
      {
         wch->nightfall = NULL;
      }

      if (!IS_NPC(wch))
      {
         continue;
      }
      if (wch->mprog_target == ch)
      {
         switch (wch->pIndexData->vnum)
         {
            default:
            {
               break;
            }
            case (MOB_VNUM_DECOY):
            case (MOB_VNUM_SHADOW):
            case (MOB_VNUM_MIRROR):
            {
               act("$n crumbles to dust.", wch, NULL, NULL, TO_ROOM);
               extract_char(wch, TRUE);
               continue;
            }
            case (MOB_VNUM_PHANTOM):
            {
               act("$n fades into nothingness.", wch, NULL, NULL, TO_ROOM);
               extract_char(wch, TRUE);
               continue;
            }
            case (MOB_VNUM_ENFORCER_GUARD):
            {
               if
               (
                  wch->master == NULL ||
                  !IS_AFFECTED(wch, AFF_CHARM)
               )
               {
                  act("$n hurries off to return to $s duties.", wch, NULL, NULL, TO_ROOM);
                  extract_char(wch, TRUE);
                  continue;
               }
            }
         }
      }
   }

   if
   (
      !IS_IMMORTAL(ch) &&
      ch->in_room->house != ch->house &&
      ch->in_room->house != 0
   )
   {
      char_from_room(ch);
      char_to_room(ch, get_room_index(10601));
   }

   if (ch->is_riding != NULL)
   {
      do_dismount(ch, NULL);
   }

   save_char_obj_1(ch, 0);
   id = ch->id;
   d = ch->desc;
   ch->quittime = -10;
   if (IS_SET(ch->comm2, COMM_MUSIC))
   {
      music_off(ch);
   }
   extract_char(ch, TRUE);
   if (d != NULL)
   {
      close_socket(d);
   }

   /* toast evil cheating bastards */
   for (d = descriptor_list; d != NULL; d = d_next)
   {
      CHAR_DATA* tch;

      d_next = d->next;
      tch = d->original ? d->original : d->character;
      if
      (
         tch &&
         tch->id == id
      )
      {
         extract_char(tch, TRUE);
         close_socket(d);
      }
   }
   return;
}



void do_save(CHAR_DATA* ch, char* argument)
{
   if (IS_NPC(ch))
   {
      return;
   }
   remove_from_rosters(ch);
   update_roster(ch, FALSE);
   save_char_obj(ch);
   if (ch->level == 1)
   {
      send_to_char("Sorry, level 1 characters are not saved.  Advance to level 2.\n\r", ch);
   }
   else
   {
      send_to_char("Saving. Remember that Dark Mists has automatic saving.\n\r", ch);
   }
   return;
}



void do_follow( CHAR_DATA* ch, char* argument )
{
   /* RT changed to allow unlimited following and follow the NOFOLLOW rules */
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA* victim;

   one_argument( argument, arg );

   if ( arg[0] == '\0' )
   {
      send_to_char( "Follow whom?\n\r", ch );
      return;
   }

   if ( ( victim = get_char_room( ch, arg ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master != NULL )
   {
      act( "But you'd rather follow $N!", ch, NULL, ch->master, TO_CHAR );
      return;
   }

   if ( victim == ch )
   {
      if ( ch->master == NULL )
      {
         send_to_char( "You already follow yourself.\n\r", ch );
         return;
      }
      stop_follower(ch);
      return;
   }

   if (!IS_NPC(victim) && IS_SET(victim->act, PLR_NOFOLLOW) && !IS_IMMORTAL(ch))
   {
      act("$N doesn't seem to want any followers.\n\r",
      ch, NULL, victim, TO_CHAR);
      return;
   }

   REMOVE_BIT(ch->act, PLR_NOFOLLOW);

   if ( ch->master != NULL )
   stop_follower( ch );

   add_follower( ch, victim );
   return;
}


void add_follower(CHAR_DATA* ch, CHAR_DATA* master)
{
   if (ch->master != NULL)
   {
      bug("Add_follower: non-null master.", 0);
      return;
   }

   ch->master        = master;
   ch->leader        = NULL;

   if (can_see(master, ch))
   {
      act("$n now follows you.", ch, NULL, master, TO_VICT);
   }
   if (IS_AFFECTED(ch, AFF_CHARM))
   {
      ch->last_fought = -1;
   }


   act( "You now follow $N.",  ch, NULL, master, TO_CHAR );

   return;
}



void stop_follower(CHAR_DATA* ch)
{
   int simulacrum;
   if (ch->master == NULL)
   {
      bug( "Stop_follower: null master.", 0 );
      return;
   }

   if
   (
      !IS_NPC(ch->master) &&
      IS_NPC(ch) &&
      ch->master->is_riding != NULL &&
      ch->master->is_riding->mount_type == MOUNT_MOBILE &&
      (CHAR_DATA* )ch->master->is_riding->mount == ch
   )
   {
      do_dismount(ch->master, NULL);
   }

   if (IS_AFFECTED(ch, AFF_CHARM))
   {
      REMOVE_BIT(ch->affected_by, AFF_CHARM);
      affect_strip(ch, gsn_charm_person);
      affect_strip(ch, gsn_seize);
      affect_strip(ch, gsn_recruit);
      affect_strip(ch, gsn_corrupt("freedom", &gsn_mob_timer));
      affect_strip(ch, gsn_enlist);
      affect_strip(ch, gsn_silver_tongue);
   }

   if
   (
      can_see(ch->master, ch) &&
      ch->in_room != NULL
   )
   {
      act("$n stops following you.", ch, NULL, ch->master, TO_VICT);
   }
   act("You stop following $N.", ch, NULL, ch->master, TO_CHAR);
   if (ch->master->pet == ch)
   {
      ch->master->pet = NULL;
   }
   simulacrum = check_simulacrum(ch);
   ch->master = NULL;
   ch->leader = NULL;
   if
   (
      simulacrum != -1 &&
      check_simulacrum(ch) == -1
   )
   {
      simulacrum_off(ch);
   }
   return;
}

/* nukes charmed monsters and pets */
void nuke_pets( CHAR_DATA* ch )
{
   CHAR_DATA* pet;

   if ((pet = ch->pet) != NULL)
   {
      stop_follower(pet);
      if (pet->in_room != NULL)
      act("$N slowly fades away.", ch, NULL, pet, TO_NOTVICT);
      extract_char(pet, TRUE);
   }
   ch->pet = NULL;

   return;
}



void die_follower(CHAR_DATA* ch)
{
   CHAR_DATA* fch;
   CHAR_DATA* fch_next;

   if (ch->master != NULL)
   {
      if (ch->master->pet == ch)
      {
         ch->master->pet = NULL;
      }
      stop_follower(ch);
   }

   ch->leader = NULL;

   for ( fch = char_list; fch != NULL; fch = fch_next )
   {
      /* NPC fading added by Ceran */
      fch_next = fch->next;  /* Need this new line to traverse list
      properly */
      if
      (
         IS_NPC(fch) &&
         (
            is_affected(fch, gsn_animate_dead) ||
            IS_AFFECTED(fch, AFF_CHARM)
         )
      )
      {
         if (fch->master == ch)
         {
            REMOVE_BIT(fch->affected_by, AFF_CHARM);
            affect_strip(fch, gsn_animate_dead);
            if (fch->on_line)
            {
               act("$n slowly fades away.", fch, NULL, NULL, TO_ROOM);
            }
            extract_char(fch, TRUE);
         }
      }
      else
      {
         if (fch->master == ch)
         {
            stop_follower(fch);
         }
         if (fch->leader == ch)
         {
            fch->leader = fch;
         }
      }
   }

   return;
}



void do_order(CHAR_DATA* ch, char* argument)
{
   char buf[MAX_STRING_LENGTH];
   char arg[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
   CHAR_DATA* victim;
   CHAR_DATA* och;
   CHAR_DATA* och_next;
   bool found;
   bool fAll;
   bool not_imm = !IS_IMMORTAL(ch);

   argument = one_argument(argument, arg);
   one_argument(argument, arg2);

   if
   (
      !str_cmp(arg2, "delete") ||
      !str_cmp(arg2, "mob") ||
      !str_cmp(arg2, "quit")
   )
   {
      send_to_char("That will NOT be done.\n\r", ch);
      return;
   }

   if
   (
      arg[0] == '\0' ||
      argument[0] == '\0'
   )
   {
      send_to_char("Order whom to do what?\n\r", ch);
      return;
   }

   if (IS_AFFECTED(ch, AFF_CHARM))
   {
      send_to_char("You feel like taking, not giving, orders.\n\r", ch);
      return;
   }
   if
   (
      (
         !str_prefix(arg2, "desc") &&
         arg2[1] == 'e'  /* allow "d" which means down */
      ) ||
      !str_prefix(arg2, "psych") ||
      !str_prefix(arg2, "pray")
   )
   {
      return;
   }
   if (!str_cmp(arg, "all"))
   {
      fAll   = TRUE;
      victim = NULL;
   }
   else
   {
      fAll = FALSE;
      if ((victim = get_char_room(ch, arg)) == NULL)
      {
         send_to_char("They are not here.\n\r", ch);
         return;
      }

      if (victim == ch)
      {
         send_to_char("Aye aye, right away!\n\r", ch);
         return;
      }

      if
      (
         !IS_AFFECTED(victim, AFF_CHARM) ||
         victim->master != ch ||
         (
            IS_IMMORTAL(victim) &&
            victim->trust >= ch->trust
         )
      )
      {
         send_to_char("Do it yourself!\n\r", ch);
         return;
      }
   }

   found = FALSE;

   for (och = ch->in_room->people; och != NULL; och = och_next)
   {
      och_next = och->next_in_room;

      if
      (
         IS_AFFECTED(och, AFF_CHARM) &&
         och->master == ch &&
         (
            fAll ||
            och == victim
         ) &&
         och->in_room != NULL
      )   /* Additional Check by Xurinos */
      {
         if
         (
            not_imm &&
            IS_NPC(och) &&
            (
               !str_prefix(arg2, "crush") ||
               !str_prefix(arg2, "palm") ||
               !str_prefix(arg2, "throw") ||
               !str_prefix(arg2, "tail") ||
               !str_prefix(arg2, "cast") ||
               (
                  arg2[0] == 'b' &&
                  arg2[1] == 'i' &&
                  arg2[2] == 't' &&
                  (
                     arg2[3] == '\0' ||
                     (
                        arg2[3] == 'e' &&
                        arg2[4] == '\0'
                     )
                  )
               ) ||
               !str_prefix(arg2, "trip") ||
               !str_prefix(arg2, "bash") ||
               !str_prefix(arg2, "cast") ||
               (
                  arg2[0] == 's' &&
                  arg2[1] == 'u' &&
                  !str_prefix(arg2, "supplicate")
               )
            )
         )
         {
            continue;
         }
         found = TRUE;
         /* wimpy demon check for brethren */
         if
         (
            demonai_match(och, DEMON_WIMP) &&
            !str_prefix(arg2, "rescue")
         )
         {
            do_emote(och, "grimaces at the thought of pain and refuses.");
            continue;
         }

         if
         (
            IS_GUARDIAN_ANGEL(och) &&
            not_imm
         )
         {
            do_gtell
            (
               och,
               "I am here to aid you of my own free will.  Trust in me."
            );
            continue;
         }
         act("$n orders you to '$t'.", ch, argument, och, TO_VICT);
         strcpy(buf, argument);
         interpret(och, buf);
      }
   }

   if (found)
   {
      WAIT_STATE(ch, PULSE_VIOLENCE);
      send_to_char("Ok.\n\r", ch);
   }
   else if (fAll)
   {
      send_to_char("You have no followers here.\n\r", ch);
   }
   return;
}



void do_group( CHAR_DATA* ch, char* argument )
{
   int simulacrum;
   char buf[MAX_STRING_LENGTH];
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA* victim;

   one_argument( argument, arg );

   if ( arg[0] == '\0' )
   {
      CHAR_DATA* gch;
      CHAR_DATA* leader;

      leader = (ch->leader != NULL) ? ch->leader : ch;
      sprintf(buf,  "%s's group:\n\r", PERS(leader, ch) );
      send_to_char( buf, ch );

      for ( gch = char_list; gch != NULL; gch = gch->next )
      {
         if ( is_same_group( gch, ch ) && gch->in_room != NULL)
         {
            sprintf(buf,
            "[%2d %s] %-16s %3d%%hp %3d%%mn %3d%%mv %5d tnl\n\r",
            gch->level,
            IS_NPC(gch) ? "Mob" : class_table[gch->class].who_name,
            capitalize(get_descr_form(gch, ch, FALSE)),
            (gch->max_hit == 0) ? 0 : (gch->hit*100)/gch->max_hit,
            (gch->max_mana == 0) ? 0 :
            (gch->mana*100)/gch->max_mana,
            (gch->max_move == 0) ? 0 :
            (gch->move*100)/gch->max_move,
            gch->level * exp_per_level(gch) - gch->exp );
            send_to_char( buf, ch );
         }
      }
      if (ch->life_lined != NULL)
      {
         gch = ch->life_lined;
         if (!is_same_group(ch, gch))
         {
            sprintf(buf, "Life lined to %s.\n\r", gch->name);
            send_to_char(buf, ch);
            sprintf(buf,
            "[%2d %s] %-16s %3d%%hp %3d%%mn %3d%%mv %5d tnl\n\r",
            gch->level,
            IS_NPC(gch) ? "Mob" : class_table[gch->class].who_name,
            capitalize( PERS(gch, ch) ),
            (gch->max_hit == 0) ? 0 :
            (gch->hit*100)/gch->max_hit,
            (gch->max_mana == 0) ? 0 :
            (gch->mana*100)/gch->max_mana,
            (gch->max_move == 0) ? 0 :
            (gch->move*100)/gch->max_move,
            gch->level * exp_per_level(gch) - gch->exp );
            send_to_char( buf, ch );
         }
      }
      return;
   }

   if ( ( victim = get_char_room( ch, arg ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if ( ch->master != NULL || ( ch->leader != NULL && ch->leader != ch ) )
   {
      send_to_char( "But you are following someone else!\n\r", ch );
      return;
   }

   if ( victim->master != ch && ch != victim )
   {
      act_new("$N isn't following you.", ch, NULL, victim, TO_CHAR, POS_SLEEPING);
      return;
   }

   if (IS_AFFECTED(victim, AFF_CHARM))
   {
      send_to_char("You can't remove charmed mobs from your group.\n\r", ch);
      return;
   }

   if (IS_AFFECTED(ch, AFF_CHARM))
   {
      act_new("You like your master too much to leave $m!",
      ch, NULL, victim, TO_VICT, POS_SLEEPING);
      return;
   }

   if ( is_same_group( victim, ch ) && ch != victim )
   {
      simulacrum = check_simulacrum(victim);
      victim->leader = NULL;
      act_new("$n removes $N from $s group.",
      ch, NULL, victim, TO_NOTVICT, POS_RESTING);
      act_new("$n removes you from $s group.",
      ch, NULL, victim, TO_VICT, POS_SLEEPING);
      act_new("You remove $N from your group.",
      ch, NULL, victim, TO_CHAR, POS_SLEEPING);
      if
      (
         simulacrum != -1 &&
         check_simulacrum(victim) == -1
      )
      {
         simulacrum_off(victim);
      }
      return;
   }

   if (( victim->level - ch->level > 8 || victim->level - ch->level < -8 ) && !IS_NPC(victim))
   {
      send_to_char( "That person is not within decent grouping range.\n\r", ch );
      return;
   }

   if (IS_GOOD(victim) && IS_EVIL(ch))
   {
      send_to_char( "That person is too good to group with you.\n\r", ch );
      return;
   }

   if (IS_EVIL(victim) && IS_GOOD(ch))
   {
      send_to_char( "That person is too evil to group with you.\n\r", ch );
      return;
   }

   simulacrum = check_simulacrum(victim);
   victim->leader = ch;
   act_new("$N joins $n's group.", ch, NULL, victim, TO_NOTVICT, POS_RESTING);
   act_new("You join $n's group.", ch, NULL, victim, TO_VICT, POS_SLEEPING);
   act_new("$N joins your group.", ch, NULL, victim, TO_CHAR, POS_SLEEPING);
   if
   (
      simulacrum == -1 &&
      check_simulacrum(victim) != -1
   )
   {
      simulacrum_on(victim);
   }
   return;
}



/*
   'Split' originally by Gnort, God of Chaos.
*/
void do_split( CHAR_DATA* ch, char* argument )
{
   char buf[MAX_STRING_LENGTH];
   char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
   CHAR_DATA* gch;
   int members;
   int amount_gold = 0, amount_silver = 0;
   int share_gold, share_silver;
   int extra_gold, extra_silver;

   argument = one_argument( argument, arg1 );
   one_argument( argument, arg2 );

   if ( arg1[0] == '\0' )
   {
      send_to_char( "Split how much?\n\r", ch );
      return;
   }

   amount_silver = atoi( arg1 );

   if (arg2[0] != '\0')
   amount_gold = atoi(arg2);

   if ( amount_gold < 0 || amount_silver < 0)
   {
      send_to_char( "Your group wouldn't like that.\n\r", ch );
      return;
   }

   if ( amount_gold == 0 && amount_silver == 0 )
   {
      send_to_char( "You hand out zero coins, but no one notices.\n\r", ch );
      return;
   }

   if ( ch->gold <  amount_gold || ch->silver < amount_silver)
   {
      send_to_char( "You don't have that much to split.\n\r", ch );
      return;
   }

   members = 0;
   for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
   {
      if ( is_same_group( gch, ch ) && !IS_AFFECTED(gch, AFF_CHARM))
      members++;
   }

   if ( members < 2 )
   {
      send_to_char( "Just keep it all.\n\r", ch );
      return;
   }

   share_silver = amount_silver / members;
   extra_silver = amount_silver % members;

   share_gold   = amount_gold / members;
   extra_gold   = amount_gold % members;

   if ( share_gold == 0 && share_silver == 0 )
   {
      send_to_char( "Don't even bother, cheapskate.\n\r", ch );
      return;
   }

   ch->silver    -= amount_silver;
   ch->silver    += share_silver + extra_silver;
   ch->gold     -= amount_gold;
   ch->gold     += share_gold + extra_gold;

   if (share_silver > 0)
   {
      sprintf(buf,
      "You split %d silver coins. Your share is %d silver.\n\r",
      amount_silver, share_silver + extra_silver);
      send_to_char(buf, ch);
   }

   if (share_gold > 0)
   {
      sprintf(buf,
      "You split %d gold coins. Your share is %d gold.\n\r",
      amount_gold, share_gold + extra_gold);
      send_to_char(buf, ch);
   }

   if (share_gold == 0)
   {
      sprintf(buf, "$n splits %d silver coins. Your share is %d silver.",
      amount_silver, share_silver);
   }
   else if (share_silver == 0)
   {
      sprintf(buf, "$n splits %d gold coins. Your share is %d gold.",
      amount_gold, share_gold);
   }
   else
   {
      sprintf(buf,
      "$n splits %d silver and %d gold coins, giving you %d silver and %d gold.\n\r",
      amount_silver, amount_gold, share_silver, share_gold);
   }

   for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
   {
      if ( gch != ch && is_same_group(gch, ch) && !IS_AFFECTED(gch, AFF_CHARM))
      {
         act( buf, ch, NULL, gch, TO_VICT );
         gch->gold += share_gold;
         gch->silver += share_silver;
      }
   }

   return;
}



void do_gtell( CHAR_DATA* ch, char* argument )
{
   CHAR_DATA* gch;
   char original[MAX_INPUT_LENGTH];
   int chance;
   OBJ_DATA* cowsuit;

   cowsuit = get_eq_char(ch, WEAR_ABOUT);
   chance = (number_range(1, 10));


   strcpy(original, argument);
   argument = original;

   if (is_affected(ch, gsn_toad))
   {
      strcpy(argument,  "<ribbit>");
   }

   if
   (
      (
         cowsuit != NULL &&
         cowsuit->pIndexData->vnum == 80000
      ) &&
      chance == 1
   )
   {
      strcpy(argument, "Moo.");
   }

   if (is_affected(ch, gsn_mute))
   {
      send_to_char("You try to tell them something, but only manage a weak rasping noise.\n\r", ch);
      return;
   }
   if (is_affected(ch, gsn_gag))
   {
      send_to_char("You can't your gagged.\n\r", ch);
      return;
   }

   if (is_affected(ch, gsn_silence)) {
      send_to_char("You cannot find the words within you.\n\r", ch);
      return;
   }

   if ( argument[0] == '\0' )
   {
      send_to_char( "Tell your group what?\n\r", ch );
      return;
   }

   if (IS_SET(ch->comm, COMM_SILENCE))
   {
      send_to_char("You are unable to communicate.\n\r", ch);
      return;
   }

   if ( IS_SET( ch->comm, COMM_NOTELL ) )
   {
      send_to_char( "Your message didn't get through!\n\r", ch );
      return;
   }

   if (IS_SET(ch->act2, PLR_LOG_SPEC)){
      log_convo(ch, NULL, TYPE_GTELL, argument);
   }

   for ( gch = char_list; gch != NULL; gch = gch->next )
   {
      if ( is_same_group( gch, ch ) && !is_ignored(ch, gch))
      {
         if (IS_SET(gch->act2, PLR_LOG_SPEC)){
            log_convo(gch, ch, TYPE_GTELL2, argument);
         }

         if (IS_SET(gch->comm, COMM_ANSI))
         {
            act_new
            (
               "$n tells the group '\x01B[1;35m$t\x01B[0;37m'",
               ch,
               argument,
               gch,
               TO_VICT,
               POS_SLEEPING
            );
         }
         else
         act_new("$n tells the group '$t'",
         ch, argument, gch, TO_VICT, POS_SLEEPING);
         /*            act_new("$n tells the group '{B{5$t{n'", ch, argument, gch, TO_VICT, POS_SLEEPING);*/
      }
   }
   if (IS_SET(ch->comm, COMM_ANSI))
   act_new("You tell the group '\x01B[1;35m$t\x01B[0;37m'", ch, argument, NULL, TO_CHAR, POS_SLEEPING);
   else
   act_new("You tell the group '$t'", ch, argument, NULL, TO_CHAR, POS_SLEEPING);
   return;
}


void do_ctell(CHAR_DATA* ch, char* argument)
{
   CHAR_DATA* gch;
   char buf[MAX_STRING_LENGTH];

   if (is_affected(ch, gsn_silence))
   {
      send_to_char("You cannot find the words within you.\n\r", ch);
      return;
   }

   if (argument[0] == '\0')
   {
      send_to_char("Say what to the conference?\n\r", ch);
      return;
   }

   if (IS_SET(ch->comm, COMM_SILENCE))
   {
      send_to_char("You are unable to communicate.\n\r", ch);
      return;
   }

   if (ch->conference == 0)
   {
      send_to_char("You do not belong to a conference.\n\r", ch);
      return;
   }


   if (!IS_NPC(ch) && IS_SET(ch->act2, PLR_LOG_SPEC))
   {
      log_convo(ch, NULL, TYPE_CONFERENCE, argument);
   }

   for ( gch = char_list; gch != NULL; gch = gch->next )
   {
      if (gch->conference == ch->conference)
      {
         if (!IS_NPC(gch) && IS_SET(gch->act2, PLR_LOG_SPEC))
         {
            log_convo(gch, ch, TYPE_CONFERENCE2, argument);
         }

         if (IS_SET(gch->comm, COMM_ANSI))
         {
            if (IS_SET(gch->comm2, COMM_REDUCED_COLOR))
            {
               sprintf
               (
                  buf,
                  "[\x01B[1;31mConference #%d\x01B[0;37m] $n: $t",
                  gch->conference
               );
            }
            else
            {
               sprintf
               (
                  buf,
                  "\x01B[1;31m[Conference #%d] $n: \x01B[1;37m$t\x01B[0;37m",
                  gch->conference
               );
            }
            act_new(buf, ch, argument, gch, TO_VICT, POS_SLEEPING);
         }
         else
         {
            sprintf(buf, "[Conference #%d] $n: $t", gch->conference);
            act_new(buf, ch, argument, gch, TO_VICT, POS_SLEEPING);
         }
      }
   }

   if ( IS_SET(ch->comm, COMM_ANSI) )
   {
      if (IS_SET(ch->comm2, COMM_REDUCED_COLOR))
      {
         sprintf
         (
            buf,
            "[\x01B[1;31mConference #%d\x01B[0;37m] $n: $t",
            ch->conference
         );
      }
      else
      {
         sprintf
         (
            buf,
            "\x01B[1;31m[Conference #%d] $n: \x01B[1;37m$t\x01B[0;37m",
            ch->conference
         );
      }
      act_new(buf, ch, argument, ch, TO_CHAR, POS_SLEEPING);
   }
   else
   {
      sprintf(buf,  "[Conference #%d] $n: $t", ch->conference);
      act_new(buf, ch, argument, ch, TO_CHAR, POS_SLEEPING);
   }

   return;
}



/*
   It is very important that this be an equivalence relation:
   (1) A ~ A
   (2) if A ~ B then B ~ A
   (3) if A ~ B  and B ~ C, then A ~ C
*/
bool is_same_group( CHAR_DATA* ach, CHAR_DATA* bch )
{
   if
   (
      ach == NULL ||
      bch == NULL
   )
   {
      return FALSE;
   }
   if (ach->leader != NULL)
   {
      ach = ach->leader;
   }
   if (bch->leader != NULL)
   {
      bch = bch->leader;
   }
   return ach == bch;
}

bool release_follower(CHAR_DATA* ch, CHAR_DATA* victim, bool quiet)
{
   int guard = guard_type(victim);

   switch (guard)
   {
      default:
      {
         if
         (
            !IS_AFFECTED(victim, AFF_CHARM) ||
            victim->master != ch
         )
         {
            if (!quiet)
            {
               send_to_char("They are not under your control.\n\r", ch);
            }
            return FALSE;
         }
         break;
      }
      case (GUARD_SENTINEL):
      case (GUARD_SENTRY):
      {
         if
         (
            !IS_AFFECTED(victim, AFF_CHARM) ||
            victim->mprog_target != ch
         )
         {
            if (!quiet)
            {
               send_to_char("They are not under your control.\n\r", ch);
            }
            return FALSE;
         }
         break;
      }
   }

   if (IS_NPC(victim))
   {
      act("$n slowly fades away.", victim, NULL, NULL, TO_ROOM);
      extract_char(victim, TRUE);
   }
   else
   {
      if (!quiet)
      {
         send_to_char("You can't release a player into the void..maybe nofollow them?\n\r", ch);
      }
      return FALSE;
   }
   return TRUE;
}

void do_release(CHAR_DATA* ch, char* argument)
{
   CHAR_DATA* victim;
   CHAR_DATA* v_next;
   char arg[MAX_INPUT_LENGTH];
   bool found = FALSE;

   one_argument(argument, arg);
   if (arg[0] == '\0')
   {
      send_to_char("Release which follower?\n\r", ch);
      return;
   }
   if (!str_cmp(arg, "room"))
   {
      for (victim = char_list; victim; victim = v_next)
      {
         v_next = victim->next;
         if
         (
            victim->in_room != ch->in_room ||
            !IS_NPC(victim) ||
            (
               victim->master != ch &&
               victim->mprog_target != ch
            ) ||
            !IS_AFFECTED(victim, AFF_CHARM)
         )
         {
            continue;
         }
         found = release_follower(ch, victim, TRUE) || found;
      }
      if (!found)
      {
         send_to_char("You have no followers here.\n\r", ch);
      }
      return;
   }
   if (!str_cmp(arg, "all"))
   {
      for (victim = char_list; victim; victim = v_next)
      {
         v_next = victim->next;
         if
         (
            !IS_NPC(victim) ||
            (
               victim->master != ch &&
               victim->mprog_target != ch
            ) ||
            !IS_AFFECTED(victim, AFF_CHARM)
         )
         {
            continue;
         }
         found = release_follower(ch, victim, TRUE) || found;
      }
      if (!found)
      {
         send_to_char("You have no followers.\n\r", ch);
      }
      return;
   }
   if (!str_cmp(arg, "missing"))
   {
      for (victim = char_list; victim; victim = v_next)
      {
         v_next = victim->next;
         if
         (
            victim->in_room == ch->in_room ||
            !IS_NPC(victim) ||
            (
               victim->master != ch &&
               victim->mprog_target != ch
            ) ||
            !IS_AFFECTED(victim, AFF_CHARM)
         )
         {
            continue;
         }
         found = release_follower(ch, victim, TRUE) || found;
      }
      if (!found)
      {
         send_to_char("You have no followers missing.\n\r", ch);
      }
      return;
   }

   victim = get_char_room(ch, arg);
   if (victim == NULL)
   {
      send_to_char("They aren't here.\n\r", ch);
      return;
   }
   release_follower(ch, victim, FALSE);
   return;
}



/*
   Rod of lordly might
   in do _say :

   if ( (wield = get_eq_char(ch, WEAR_WIELD)) != NULL
   && wield->pIndexData->vnum == OBJ_VNUM_LORDLY)
   obj_say_lordly(ch, wield, argument);
   wield = get_eq_char(ch, WEAR_DUAL_WIELD);
   if (wield != NULL && wield->pIndexData->vnum == OBJ_VNUM_LORDLY)
   obj_say_lordly(ch, wield, argument):
*/
void obj_say_lordly(CHAR_DATA* ch, OBJ_DATA* obj, char* argument)
{
   if (stristr(argument, "are not rulers figureheads?"))
   {
      if (obj->item_type == ITEM_TRASH)
      {
         send_to_char("Your rod warms a little but nothing happens.\n\r", ch);
         WAIT_STATE(ch, 12);
         return;
      }
      obj->value[0] = 0;
      obj->value[1] = 0;
      obj->value[2] = 0;
      if
      (
         obj->item_type == ITEM_WEAPON &&
         (
            obj->wear_loc == WEAR_WIELD ||
            obj->wear_loc == WEAR_DUAL_WIELD
         )
      )
      {
         unequip_char(ch, obj);
         if (get_eq_char(ch, WEAR_HOLD) == NULL)
         {
            equip_char(ch, obj, WEAR_HOLD);
         }
         reslot_weapon(ch);
      }
      obj->item_type = ITEM_TRASH;
      REMOVE_BIT(obj->wear_flags, ITEM_WIELD);
      SET_BIT(obj->wear_flags, ITEM_HOLD);
      obj->value[3] = 0;
      free_string(obj->short_descr);
      obj->short_descr = str_dup("the rod of lordly might");
      act("$n's rod of lordly might clicks and changes form into a nondescript staff.", ch, NULL, NULL, TO_ROOM);
      send_to_char("Your rod of lordly might clicks and changes into a nondescript staff.\n\r", ch);
      WAIT_STATE(ch, 12);
      return;
   }


   if (stristr(argument, "an iron fist establishes rule"))
   {
      if
      (
         obj->item_type == ITEM_WEAPON &&
         obj->value[0] == WEAPON_MACE
      )
      {
         send_to_char("Your rod warms a little but nothing happens.\n\r", ch);
         WAIT_STATE(ch, 12);
         return;
      }
      obj->value[0] = WEAPON_MACE;
      obj->value[1] = 7;
      obj->value[2] = 7;
      obj->item_type = ITEM_WEAPON;
      REMOVE_BIT(obj->wear_flags, ITEM_HOLD);
      SET_BIT(obj->wear_flags, ITEM_WIELD);
      obj->value[3] = attack_lookup("crush");
      free_string(obj->short_descr);
      obj->short_descr = str_dup("a rod with a large heavy mace head");
      act("$n's rod of lordly might clicks and changes form into a mace!", ch, NULL, NULL, TO_ROOM);
      send_to_char("Your rod of lordly might clicks and changes into a mace!\n\r", ch);
      WAIT_STATE(ch, 12);
      return;
   }

   if (stristr(argument, "the weak are always subjugated"))
   {
      if
      (
         obj->item_type == ITEM_WEAPON &&
         obj->value[0] == WEAPON_SWORD
      )
      {
         send_to_char("Your rod warms a little but nothing happens.\n\r", ch);
         WAIT_STATE(ch, 12);
         return;
      }
      obj->value[0] = WEAPON_SWORD;
      obj->value[1] = 7;
      obj->value[2] = 7;
      obj->item_type = ITEM_WEAPON;
      REMOVE_BIT(obj->wear_flags, ITEM_HOLD);
      SET_BIT(obj->wear_flags, ITEM_WIELD);
      obj->value[3] = attack_lookup("slash");
      free_string(obj->short_descr);
      obj->short_descr = str_dup("a rod with long slim blade");
      act("$n's rod of lordly might clicks and changes form into a sword!", ch, NULL, NULL, TO_ROOM);
      send_to_char("Your rod of lordly might clicks and changes into a sword!\n\r", ch);
      WAIT_STATE(ch, 12);
      return;
   }

   if (stristr(argument, "the divine alone are meant to rule"))
   {
      if
      (
         obj->item_type == ITEM_WEAPON &&
         obj->value[0] == WEAPON_EXOTIC
      )
      {
         send_to_char("Your rod warms a little but nothing happens.\n\r", ch);
         WAIT_STATE(ch, 12);
         return;
      }
      obj->value[0] = WEAPON_EXOTIC;
      obj->value[1] = 7;
      obj->value[2] = 7;
      obj->item_type = ITEM_WEAPON;
      REMOVE_BIT(obj->wear_flags, ITEM_HOLD);
      SET_BIT(obj->wear_flags, ITEM_WIELD);
      obj->value[3] = attack_lookup("divine");
      free_string(obj->short_descr);
      obj->short_descr = str_dup("a rod wreathed in blue flames");
      act("$n's rod of lordly might elongates and suddenly becomes wreathed in blue flames!", ch, NULL, NULL, TO_ROOM);
      send_to_char("Your rod of lordly might elongates and suddenly becomes wreathed in blue flames!\n\r", ch);
      WAIT_STATE(ch, 12);
      return;
   }

   return;
}

void skill_gain(CHAR_DATA* ch, char* sk, sh_int lvl)
{
   int sn;

   sn = skill_lookup(sk);
   if (sn == -1)
   {
      sprintf(log_buf, "skill_gain, invalid skill: %s", sk);
      bug(log_buf, 0);
      return;
   }
   if (lvl == -1)
   lvl = skill_table[sn].skill_level[ch->class];

   ch->pcdata->learnlvl[sn] = lvl;
   if (ch->pcdata->learned[sn] > 1)
   return;
   if (ch->level <= lvl)
   ch->pcdata->learned[sn] = 1;
   else
   ch->pcdata->learned[sn] = 75;

   return;
}

void skill_lose(CHAR_DATA* ch, char* sk)
{
   int sn = skill_lookup(sk);
   if (sn == -1)
   {
      sprintf(log_buf, "skill_lose, invalid skill: %s", sk);
      bug(log_buf, 0);
      return;
   }
   ch->pcdata->learned[sn] = -1;
   return;
}

/* a mob_say_trigger */

void mob_say_lady(CHAR_DATA* ch, char* argument)
{
   CHAR_DATA* lady;

   if (IS_NPC(ch))
   return;

   for (lady = ch->in_room->people; lady != NULL; lady = lady->next_in_room)
   {
      if (!IS_NPC(lady))    continue;
      if (lady->pIndexData->vnum == MOB_VNUM_DRANNOR_LADY)
      break;
   }
   if (lady == NULL)
   return;
   if (lady->position == POS_FIGHTING || lady->position == POS_SLEEPING)
   return;


   if (!str_cmp(argument, "yes"))
   {
      /*
      if (IS_GOOD(ch))
      {
      do_say(lady, "Take the key and destroy that which destroyed my people.");
      key = create_object(get_obj_index(OBJ_VNUM_BLACK_KEY), 0);
      obj_to_char(key, ch);
      act("$n gives $N $p.", lady, key, ch, TO_NOTVICT);
      act("$n gives you $p.", lady, key, ch, TO_VICT);
      act("$n slowly fades away.", lady, NULL, NULL, TO_ROOM);
      extract_char(lady, TRUE);
      return;
      }
      */
      do_say(lady, "Bring me the head of the beast that slew my people as they fled to the seas.");
      do_say(lady, "It lives in the bloody waters, feeding off the bones of the elves it killed as they tried to flee.");
      return;
   }
   if (strstr(argument, "beast"))
   {
      do_say(lady, "The beast is dangerous beyond mortal comprehension.");
      do_say(lady, "Find the fallen knight in the towers and retrieve the slayer to kill the beast.");
      act("$n flickers for a moment then resumes $s tranquil position of waiting.", lady, NULL, NULL, TO_ROOM);
      return;
   }
   return;
}

void mob_say_guardian_angel(CHAR_DATA* ch, CHAR_DATA*  mob, char* argument)
{
   ROOM_INDEX_DATA*  pRoomIndex;

   if (!( IS_GUARDIAN_ANGEL(mob) && (ch == mob->master) ))
   return;

   if ( !IS_AWAKE(mob) )
   return;

   if (!stristr(argument, "save me"))
   return;

   if (ch->in_room != mob->in_room)
   return;

   if ( ch->position == POS_FIGHTING )
   {
      do_gtell( mob, "When you are no longer busy, I will." );
      return;
   }

   if ( mob->position == POS_FIGHTING )
   {
      do_gtell( mob, "I would, but I am in the midst of combat." );
      return;
   }

   if (
      ch->in_room->exit[0]
      || ch->in_room->exit[1]
      || ch->in_room->exit[2]
      || ch->in_room->exit[3]
      || ch->in_room->exit[4]
      || ch->in_room->exit[5]
   )
   {
      do_gtell( mob, "This place has some form of escape." );
      return;
   }

   if ( (IS_SET(ch->in_room->extra_room_flags, ROOM_1212)) || (ch->in_room->house != 0) )

   {
      do_gtell( mob, "The gods do not wish me to take you from here." );
      return;
   }
   if (IS_SET(ch->in_room->extra_room_flags, ROOM_DEAD_ZONE))
   {
      do_gtell(mob, "A force within this place prevents our escape.");
      return;
   }

   if ( !(pRoomIndex = get_random_room(ch, RANDOM_AREA)) )
   {
      bug( "NULL room from get_random_room in RANDOM_AREA.", 0 );
      return;
   }

   act( "$N spreads $s wings, grabs $n, and flies away.", ch, NULL, mob, TO_ROOM );
   act( "$n spreads $s wings, grabs you, and flies away with you.", mob, NULL, ch, TO_VICT );
   char_from_room( ch );
   char_to_room( ch, pRoomIndex );
   char_from_room( mob );
   char_to_room( mob, pRoomIndex );
   act( "$N flies into the area, descends, and sets $n down.", ch, NULL, mob, TO_ROOM );
   send_to_char( "You and your guardian angel land.\n\r", ch );
   do_observe(ch, "", LOOK_AUTO);
   WAIT_STATE( ch, 48 );

   return;
}

void mob_say_house_statue(CHAR_DATA* ch, char* argument)
{
   CHAR_DATA* statue;
   sh_int vnum;
   OBJ_DATA* item;
   MESSAGE* message;
   char buf[MAX_STRING_LENGTH];
   char item_string[25];

   if ( IS_NPC(ch) )
   return;

   if
   (
      ch->house == 0 ||
      ch->house != ch->in_room->house
   )
   {
      return;
   }
   for (statue = ch->in_room->people; statue != NULL; statue = statue->next_in_room)
   {
      if ( !IS_NPC(statue) )
      continue;

      if
      (
         (
            vnum = statue->pIndexData->vnum
         ) == house_table[ch->house].vnum_statue
      )
      {
         break;
      }
   }

   if (statue == NULL)
   {
      return;
   }

   if
   (
      statue->position == POS_FIGHTING ||
      statue->position == POS_SLEEPING
   )
   {
      return;
   }

   if (!str_cmp(argument, "Who has taken our item?"))
   {
      if (!(message = get_message_char(statue, MM_HOUSE_ITEM_TAKEN)))
      {
         do_say(statue, "I hold the item.");
         return;
      }
      strcpy(item_string, house_table[ch->house].item_name);

      vnum = house_table[ch->house].vnum_item;
      for (item = object_list; item != NULL; item = item->next )
      {
         if (item->pIndexData->vnum == vnum)
         break;
      }

      if
      (
         item &&
         item->carried_by != statue
      )
      {
         sprintf( buf,  "I had to give the %s to %s %ld hour%s ago.", item_string, message->message_string, message->message_data, message->message_data != 1 ? "s":"" );
         do_say( statue, buf );
      }
      else
      {
         message_remove(message);
         do_say( statue, "I hold our item." );
      }
   }
   return;
}


void mob_say_standing(CHAR_DATA* ch, char* argument)
{
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA* statue, *ancientpc, *greatancient=NULL;
   int count=0;

   if (!(strstr(argument, "standing") || strstr(argument, "Standing")))
   return;

   if (IS_NPC(ch))
   return;

   if (ch->house != HOUSE_ANCIENT)
   return;

   for (statue = ch->in_room->people; statue != NULL; statue = statue->next_in_room)
   {
      if (!IS_NPC(statue))    continue;
      if (statue->pIndexData->vnum == 3806) /* Ancient statue */
      break;
   }
   if (statue == NULL)
   return;
   if (statue->position == POS_FIGHTING || statue->position == POS_SLEEPING)
   return;

   for (ancientpc = char_list; ancientpc != NULL; ancientpc = ancientpc->next)
   {
      if (IS_NPC(ancientpc) || ancientpc->house != HOUSE_ANCIENT) continue;
      if (greatancient == NULL) greatancient = ancientpc;
      if (ancientpc->pcdata->targetkills > greatancient->pcdata->targetkills)
      greatancient = ancientpc;
   }
   for (ancientpc = char_list; ancientpc != NULL; ancientpc = ancientpc->next)
   {
      if (IS_NPC(ancientpc) || ancientpc->house != HOUSE_ANCIENT) continue;
      if ((ancientpc->pcdata->targetkills == greatancient->pcdata->targetkills)
      && (ancientpc != greatancient))
      {
         count++;
         if (count > 1)
         sprintf(buf,  "%s%s%s", buf,", ", ancientpc->name);
         else
         sprintf(buf,  "%s", ancientpc->name);
      }
   }
   if (count > 0)
   sprintf(buf,  "%s and %s", buf, greatancient->name);
   else
   sprintf(buf,  "%s", greatancient->name);
   count++;
   sprintf(buf,  "%s %s the greatest hunter%s currently prowling.",
   buf, (count > 1) ? "are" : "is", (count > 1) ? "s" : "");
   /*      do_say(statue, buf); */
   sprintf(buf,  "'%s' You have a standing rating of %ld.", get_name(ch, NULL), ch->pcdata->targetkills);
   do_tell(statue, buf);
   return;
}


void get_age_mod(CHAR_DATA* ch)
{
   int mod;
   int race;

   if (IS_NPC(ch))
   return;

   race = ch->race;

   if (race == grn_human)
   {
      mod = dice(2, 6);
   }
   else if
   (
      race == grn_elf ||
      race == grn_grey_elf ||
      race == grn_dark_elf
   )
   {
      mod = dice(6, 8);
      mod += dice(6, 10);
   }
   else if
   (
      race == grn_giant ||
      race == grn_centaur ||
      race == grn_troll
   )
   {
      mod = dice(4, 5);
   }
   else if (race == grn_gnome)
   {
      mod = dice(6, 8);
      mod += dice(10, 11);
   }
   else if (race == grn_dwarf)
   {
      mod = dice(5, 6);
   }
   else
   {
      mod = dice(4, 5);
   }

   ch->pcdata->age_mod = mod;
   return;
}

void do_notransfer(CHAR_DATA* ch, char* argument)
{
   if (IS_NPC(ch))
   return;

   if (IS_SET(ch->act, PLR_NO_TRANSFER))
   {
      REMOVE_BIT(ch->act, PLR_NO_TRANSFER);
      send_to_char("You now accept transfered objects.\n\r", ch);
      return;
   }

   SET_BIT(ch->act, PLR_NO_TRANSFER);
   send_to_char("You no longer accept transfered objects.\n\r", ch);
   return;
}

void do_evaluation(CHAR_DATA* ch, char* argument)
{
   if (IS_NPC(ch))
   return;

   if (IS_SET(ch->act, PLR_EVALUATION))
   {
      send_to_char("You no longer attempt to evaluate your opponent's wounds.\n\r", ch);
      REMOVE_BIT(ch->act, PLR_EVALUATION);
      return;
   }
   if (get_skill(ch, gsn_evaluation) > 0)
   {
      send_to_char("You now attempt to evaluate your opponent's wounds.\n\r", ch);
      SET_BIT(ch->act, PLR_EVALUATION);
   } else
   {
      send_to_char("You don't know how to evaluate wounds.\n\r", ch);
   }
   return;
}


void say_prog_summon_demon(CHAR_DATA* ch, char* argument)
{
   CHAR_DATA* mob;
   char buf[MAX_STRING_LENGTH];
   if (ch->in_room == NULL
   || ch->in_room->vnum != ROOM_VNUM_DEMOGORGON_SUMMON)
   return;

   if (!stristr(argument, "Yargh Dagon Nahr Sothoth"))
   return;

   for (mob = char_list;mob != NULL; mob = mob->next)
   if (IS_NPC(mob) && mob->pIndexData->vnum == MOB_VNUM_DEMOGORGON
   && mob->in_room != NULL
   && mob->in_room->vnum != ROOM_VNUM_SHAD_DEMOGORGON)
   break;

   if (mob != NULL)
   return;

   for (mob = char_list;mob != NULL; mob = mob->next)
   if (IS_NPC(mob) && mob->pIndexData->vnum == MOB_VNUM_DEMOGORGON
   && mob->in_room != NULL
   && mob->in_room->vnum == ROOM_VNUM_SHAD_DEMOGORGON)
   break;

   if (mob == NULL)
   return;
   char_from_room(mob);
   act("The pentegram sizzles and steams and a huge demon steps out from a gate!", ch, NULL, NULL, TO_ROOM);
   act("The pentegram sizzles and steams and a huge demon steps out from a gate!", ch, NULL, NULL, TO_CHAR);

   char_to_room(mob, ch->in_room);
   do_yell(mob, "You dare to summon me mortal!");
   sprintf(buf,  "Help! I'm being attacked by %s!", mob->short_descr);
   do_yell(ch, buf);
   multi_hit(mob, ch,-1);

   return;


}

void say_prog_raise_shadowlord(CHAR_DATA* ch, char* argument)
{
   CHAR_DATA* mob;
   OBJ_DATA* obj;
   OBJ_DATA* obj_next;
   char buf[MAX_STRING_LENGTH];

   if (ch->in_room == NULL
   || ch->in_room->vnum != ROOM_VNUM_SHADOWLORD_SUMMON)
   return;

   if (!stristr(argument, "yargh nosferatus vampyrius sothoth"))
   return;

   for (mob = mob = char_list;mob != NULL; mob = mob->next)
   if (IS_NPC(mob) && mob->pIndexData->vnum == MOB_VNUM_SHADOWLORD
   && mob->in_room != NULL
   && mob->in_room->vnum != ROOM_VNUM_SHAD_DEMOGORGON)
   break;

   if (mob != NULL)
   return;

   for (mob = char_list;mob != NULL; mob = mob->next)
   if (IS_NPC(mob) && mob->pIndexData->vnum == MOB_VNUM_SHADOWLORD
   && mob->in_room != NULL
   && mob->in_room->vnum == ROOM_VNUM_SHAD_DEMOGORGON)
   break;

   if (mob == NULL)
   return;

   for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
   {
      obj_next = obj->next_content;
      if (obj->pIndexData->vnum == OBJ_VNUM_PILE_BODIES_RITUAL)
      break;
   }
   if (obj == NULL)
   return;
   act("The corpses explode into a bloody mist and something dark forms out of it!", ch, NULL, NULL, TO_ROOM);
   act("The corpses explode into a bloody mist and something dark forms out of it!", ch, NULL, NULL, TO_CHAR);
   extract_obj(obj, FALSE);
   char_from_room(mob);

   char_to_room(mob, ch->in_room);
   do_say(mob, "Feeeed me mortal!");
   sprintf(buf,  "Help! I'm being attacked by %s!", mob->short_descr);
   do_yell(ch, buf);

   multi_hit(mob, ch,-1);
   return;
}

void say_prog_cru_blacksmith(CHAR_DATA* ch, char* argument)
{
   CHAR_DATA* smith;
   OBJ_DATA* obj;
   sh_int success=0;
   bool found=FALSE;

   if ( ch == NULL
   ||  ch->in_room == NULL
   ||  ch->in_room->vnum != 5723
   ||  ch->house != HOUSE_CRUSADER)
   return;

   smith = ch->in_room->people;

   while (smith)
   {
      if ( IS_NPC(smith)
      &&  (smith->pIndexData->vnum == 5702
      || smith->pIndexData->vnum == 5704
      || smith->pIndexData->vnum == 5711) )
      {
         found=TRUE;
         break;
      }

      smith=smith->next_in_room;
   }

   if (found==FALSE) return;
   found=FALSE;

   if (stristr(argument, "Undo the wretched grip of magics, blacksmith!"))
   {
      if ( house_down(smith, HOUSE_CRUSADER) )
      {
         do_say(smith, "I cannot do that without my house powers.");
         return;
      }

      obj = ch->carrying;

      while (obj)
      {
         if ( (IS_OBJ_STAT(obj, ITEM_NODROP)
         || IS_OBJ_STAT(obj, ITEM_NOREMOVE))
         && !IS_OBJ_STAT(obj, ITEM_NOUNCURSE)
         && !CAN_WEAR(obj, ITEM_KEEP) )
         {
            found = TRUE;
            break;
         }

         obj = obj->next_content;
      }

      if (found == FALSE)
      {
         do_say(smith, "Doesn't look to me like you've got anything I can work with.");
         return;
      }

      if ((success=number_percent()) < 20)
      {
         act("The blacksmith runs his hands over $p and shakes $s head solemnly.", ch, obj, NULL, TO_ALL);
         return;
      } else if (success < (20+(obj->level/2)))
      {
         act("The blacksmith touches $p and it explodes in a shower of sparks!", ch, obj, NULL, TO_ALL);
         do_say(smith, "Oops.  Sorry." );
         extract_obj(obj, FALSE);
         return;
      }

      if ( ((ch->gold * 100) + ch->silver) < 10000)
      {
         do_say(smith, "I'm afraid you can't afford my services.");
         return;
      }

      deduct_cost( ch, 10000 );

      REMOVE_BIT(obj->extra_flags, ITEM_NODROP);
      REMOVE_BIT(obj->extra_flags, ITEM_NOREMOVE);
      act("The blacksmith runs $s calloused hands over $p and nods in satisfaction.", ch, obj, NULL, TO_ALL);
      return;

   }
   else if (stristr(argument, "Make pure this tainted gear, blacksmith!"))
   {
      if (house_down(smith, HOUSE_CRUSADER))
      {
         do_say(smith, "I cannot do that without my house powers.");
         return;
      }

      obj = ch->carrying;

      while (obj)
      {
         if
         (
            (
               is_pulsing(obj) ||
               (
                  IS_OBJ_STAT(obj, ITEM_INVIS) &&
                  obj->item_type != ITEM_WEAPON
               )
            ) &&
            !CAN_WEAR(obj, ITEM_KEEP)
         )
         {
            found = TRUE;
            break;
         }

         obj = obj->next_content;
      }

      if (found == FALSE)
      {
         do_say(smith, "Your gear looks fine to me.");
         return;
      }

      if (obj->item_type == ITEM_PILL
      || obj->item_type == ITEM_SCROLL
      || obj->item_type == ITEM_STAFF
      || obj->item_type == ITEM_POTION
      || obj->item_type == ITEM_WAND)
      {
         extract_obj(obj, FALSE);
         act("The blacksmith smirks, and reaches out to touch $p.", ch, obj, NULL, TO_ALL);
         act("In a flash of light and smoke, $p explodes!", ch, obj, NULL, TO_ALL);
         do_say(smith, "All set.");
         return;
      }

      act("The Blacksmith takes $p and begins the cleansing process.", ch, obj, NULL, TO_ALL);

      if ((success=number_percent()) < 20)
      {
         act("As the Blacksmith hammers upon $p, a deep crack appears, destroying it.", ch, obj, NULL, TO_ALL);
         extract_obj(obj, FALSE);
         return;
      }

      if ( ((ch->gold * 100) + ch->silver) < 10000)
      {
         do_say(smith, "I'm afraid you can't afford my services.");
         return;
      }

      un_pulse(obj);
      REMOVE_BIT(obj->extra_flags, ITEM_INVIS);
      if ( is_pulsing(obj) )
      {
         extract_obj(obj, FALSE);
         act("The blacksmith frowns, and grips $p tightly.", ch, obj, NULL, TO_ALL);
         act("In a flash of light and smoke, $p explodes!", ch, obj, NULL, TO_ALL);
         do_say(smith, "All set.");
         return;
      }

      deduct_cost( ch, 10000 );

      act("The reforging process cleanses $p!", ch, obj, NULL, TO_ALL);
      return;
   }
}

void un_pulse(OBJ_DATA* obj)
{
   AFFECT_DATA* paf      = NULL;
   AFFECT_DATA* paf_next = NULL;
   sh_int value_index    = 0;
   sh_int charges        = 0;
   bool rot              = FALSE;
   bool changed_type     = FALSE;
   OBJ_DATA* primary = NULL;
   OBJ_DATA* secondary = NULL;
   int worn = WEAR_NONE;
   CHAR_DATA* ch = NULL;

   if (!is_pulsing(obj))
   {
      return;
   }
   /* store worn location if it is worn */
   if (obj->wear_loc != WEAR_NONE)
   {
      ch = obj->carried_by;
      worn = obj->wear_loc;
      switch (obj->wear_loc)
      {
         case (WEAR_WIELD):
         {
            primary = get_eq_char(ch, WEAR_WIELD);
            secondary = get_eq_char(ch, WEAR_DUAL_WIELD);
            unequip_char(ch, primary);
            if (secondary)
            {
               unequip_char(ch, secondary);
            }
            break;
         }
         case (WEAR_DUAL_WIELD):
         {
            secondary = get_eq_char(ch, WEAR_DUAL_WIELD);
            unequip_char(ch, secondary);
            break;
         }
         default:
         {
            unequip_char(ch, obj);
            break;
         }
      }
   }


   /*
   Store Rot_death status
   */
   rot = IS_SET(obj->extra_flags, ITEM_ROT_DEATH);
   /*
   Store # of charges
   */
   if ((obj->item_type == ITEM_WAND) || (obj->item_type == ITEM_STAFF))
   {
      charges = obj->value[2];
   }
   /* fade it */
   for (paf = obj->affected; paf; paf = paf_next)
   {
      paf_next = paf->next;
      free_affect(paf);
   }
   obj->affected     = NULL;
   if (obj->item_type != obj->pIndexData->item_type)
   {
      obj->item_type = obj->pIndexData->item_type;
      changed_type   = TRUE;
   }
   obj->extra_flags  = obj->pIndexData->extra_flags;
   obj->extra_flags2 = obj->pIndexData->extra_flags2;
   obj->level        = obj->pIndexData->level;
   obj->condition    = obj->pIndexData->condition;
   obj->cost         = obj->pIndexData->cost;
   obj->weight       = obj->pIndexData->weight;
   for (value_index = 0; value_index < 5; value_index++)
   {
      obj->value[value_index]  = obj->pIndexData->value[value_index];
   }
   free_string(obj->material);
   obj->material = str_dup(obj->pIndexData->material);
   if (obj->mount_specs != obj->pIndexData->mount_specs)
   {
      free_mem(obj->mount_specs, sizeof(MOUNT_DATA));
      obj->mount_specs = obj->pIndexData->mount_specs;
   }
   obj->enchanted = FALSE;
   /*
   restore Rot_death status
   */
   if (rot)
   {
      SET_BIT(obj->extra_flags, ITEM_ROT_DEATH);
   }
   else
   {
      REMOVE_BIT(obj->extra_flags, ITEM_ROT_DEATH);
   }
   if
   (
      !changed_type &&  /* If it changed types it can keep the new charges */
      (
         obj->item_type == ITEM_WAND ||
         obj->item_type == ITEM_STAFF
      )
   )
   {
      /*
      Restore # of charges
      */
      obj->value[2] = charges;
   }
   /* Mobs and PC's need to be able to see it */
   REMOVE_BIT(obj->extra_flags, ITEM_VIS_DEATH);

   /* restore worn location */
   if (ch != NULL)
   {
      if (primary != NULL)
      {
         equip_char(ch, primary, WEAR_WIELD);
      }
      if (secondary != NULL)
      {
         equip_char(ch, secondary, WEAR_DUAL_WIELD);
      }
      if
      (
         primary == NULL &&
         secondary == NULL
      )
      {
         equip_char(ch, obj, worn);
      }
      else
      {
         reslot_weapon(ch);
      }
   }
   return;
}

void obj_say_sentient_sword( CHAR_DATA* ch, char* argument, OBJ_DATA* obj )
{
   int temp;

   if ( str_prefix( "My sword,", argument ) )
   return;

   if ( !str_cmp(argument, "My sword, burst into flames and burn our foe!") )
   {
      if ( obj->value[3] != (temp = attack_lookup("flbite")) )
      {
         obj->value[3] = temp;
         send_to_char( "The Sentient Sword bursts into flames!\n\r", ch );
         act( "The sword in $n's hand bursts into flames!", ch, NULL, NULL, TO_ROOM );
      }
      return;
   }

   if ( !str_cmp(argument, "My sword, grow cold and freeze our foe!") )
   {
      if ( obj->value[3] != (temp = attack_lookup("frbite")) )
      {
         obj->value[3] = temp;
         send_to_char( "The Sentient Sword turns blue and coats with ice!\n\r", ch );
         act( "The sword in $n's hand turns blue and coats with ice!", ch, NULL, NULL, TO_ROOM );
      }
      return;
   }

   if ( !str_cmp(argument, "My sword, gather your energies and rain lightning upon our foe!") )
   {
      if ( obj->value[3] != (temp = attack_lookup("shbite")) )
      {
         obj->value[3] = temp;
         send_to_char( "The Sentient Sword crackles with energy!\n\r", ch );
         act( "The sword in $n's hand crackles with energy!", ch, NULL, NULL, TO_ROOM );
      }
      return;
   }

   if ( !str_cmp(argument, "My sword, unleash your caustic acids and consume our foe!") )
   {
      if ( obj->value[3] != (temp = attack_lookup("acbite")) )
      {
         obj->value[3] = temp;
         send_to_char( "The Sentient Sword coats itself with corrosive acid!\n\r", ch );
         act( "The sword in $n's hand coats itself with corrosive acid!", ch, NULL, NULL, TO_ROOM );
      }
      return;
   }

   if ( !str_cmp(argument, "My sword, draw the life from our foe!") )
   {
      if ( obj->value[3] != (temp = attack_lookup("drain")) )
      {
         obj->value[3] = temp;
         send_to_char( "The Sentient Sword reverts to its original form!\n\r", ch );
         act( "The sword in $n's hand looks normal once more!", ch, NULL, NULL, TO_ROOM );
      }
      return;
   }
   return;
}

void do_limpurge(CHAR_DATA* ch, void *argument)
{
   OBJ_DATA* obj;
   OBJ_DATA* next_obj;
   CHAR_DATA* victim;
   int take = FALSE;
   int max_only = FALSE;
   int full_strip = FALSE;
   char arg[MAX_STRING_LENGTH];

   argument = one_argument( argument, arg );

   victim = get_char_room(ch, arg);
   if (victim == NULL)
   {
      send_to_char("They aren't here.\n\r", ch);
      return;
   }
   if (IS_NPC(victim))
   {
      send_to_char("Not on NPCs.\n\r", ch);
      return;
   }
   if (get_trust(victim) >= get_trust(ch))
   {
      send_to_char("You failed.\n\r", ch);
      return;
   }
   while(arg[0] != '\0')
   {
      argument = one_argument( argument, arg );
      if (!str_cmp(arg, "all")) full_strip = TRUE;
      if (!str_cmp(arg, "max")) max_only = TRUE;
      if (!str_cmp(arg, "take")) take = TRUE;
   }

   for (obj = victim->carrying; obj != NULL; obj = next_obj)
   {
      next_obj = obj->next_content;
      if (full_strip ||
      (!max_only && obj->pIndexData->limtotal > 0) ||
      ( obj->pIndexData->limtotal != 0 &&
      obj->pIndexData->limtotal <= obj->pIndexData->limcount))
      {
         if (!take)
         {
            extract_obj(obj, FALSE);
         }
         else
         {
            obj_from_char(obj);
            obj_to_char(obj, ch);
         }
      }
   }
   if (!full_strip && !max_only)
   {
      send_to_char("You have been purged of limited possessions.\n\r", victim);
      send_to_char("You have purged their limited possessions.\n\r", ch);
   }
   else if (full_strip)
   {
      send_to_char("You have been purged of all possessions.\n\r", victim);
      send_to_char("You have purged all their possessions.\n\r", ch);
   }
   else
   {
      send_to_char("You have been purged of maxxed out possessions.\n\r", victim);
      send_to_char("You have purged their maxxed out possessions.\n\r", ch);
   }
   return;
}

void do_cctell(CHAR_DATA* ch, char* argument)
{
   char buf[MAX_STRING_LENGTH];
   sh_int clan;
   DESCRIPTOR_DATA* d;
   char original[MAX_INPUT_LENGTH];

   if
   (
      IS_NPC(ch) ||
      (clan = ch->pcdata->clan) == 0 ||
      clan_table[clan].rank < 2
   )
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }

   if (argument[0] == '\0')
   {
      if (IS_SET(ch->comm2, COMM_NOCLAN))
      {
         send_to_char("Clan channel is now ON\n\r", ch);
         REMOVE_BIT(ch->comm2, COMM_NOCLAN);
      }
      else
      {
         send_to_char("Clan channel is now OFF\n\r", ch);
         SET_BIT(ch->comm2, COMM_NOCLAN);
      }
      return;
   }

   if (is_affected(ch, gsn_mute))
   {
      send_to_char
      (
         "You try to tell them something, but only manage a weak rasping noise."
         "\n\r",
         ch
      );
      return;
   }

   if (is_affected(ch, gsn_gag))
   {
      send_to_char("You cannot you are gagged.\n\r", ch);
      return;
   }

   if (is_affected(ch, gsn_silence))
   {
      send_to_char("You cannot find the words within you.\n\r", ch);
      return;
   }

   if (IS_SET(ch->comm, COMM_SILENCE))
   {
      send_to_char("You are unable to communicate.\n\r", ch);
      return;
   }

   if (IS_SET(ch->comm, COMM_NOCHANNELS))
   {
      send_to_char("The gods have revoked your channel priviliges.\n\r", ch);
      return;
   }

   original[0] = '\0';
   /* place ancient titles in cct */
   if (!IS_NPC(ch) && ch->house == HOUSE_ANCIENT)
   {
      sprintf(original, "(%s) ", get_ancient_title(ch));
   }
   if (is_affected(ch, gsn_toad))
   {
      strcat(original,  "<ribbit>");
   }
   else
   {
      strcat(original, argument);
   }
   argument = original;

   if (IS_SET(ch->act2, PLR_LOG_SPEC))
   {
      log_convo(ch, NULL, TYPE_CLAN, argument);
   }

   if (IS_SET(ch->comm2, COMM_REDUCED_COLOR))
   {
      sprintf
      (
         buf,
         "[{B{6%s{n] $n: $t",
         clan_table[clan].who_name
      );
   }
   else
   {
      sprintf
      (
         buf,
         "{B{6[%s] $n:{n $t",
         clan_table[clan].who_name
      );
   }
   act_new_color
   (
      buf,
      ch,
      argument,
      NULL,
      TO_CHAR,
      POS_DEAD,
      TRUE
   );

   REMOVE_BIT(ch->comm2, COMM_NOCLAN);
   for (d = descriptor_list; d != NULL; d = d->next)
   {
      if
      (
         d->character &&
         is_affected(d->character, gsn_shock_sphere) &&
         !is_mental(ch)
      )
      {
         continue;
      }

      if
      (
         d->connected == CON_PLAYING &&
         d->character != ch &&
         !is_ignored(ch, d->character) &&
         (
            (
               !IS_NPC(d->character) &&
               d->character->pcdata->clan == clan
            ) ||
            IS_SET(d->character->comm2, COMM_CTGLOBAL)
         ) &&
         !IS_SET(d->character->comm2, COMM_NOCLAN)
      )
      {
         if (IS_SET(d->character->comm2, COMM_REDUCED_COLOR))
         {
            sprintf
            (
               buf,
               "[{B{6%s{n] $N: $t",
               clan_table[clan].who_name
            );
         }
         else
         {
            sprintf
            (
               buf,
               "{B{6[%s] $N:{n $t",
               clan_table[clan].who_name
            );
         }
         act_new_color
         (
            buf,
            d->character,
            argument,
            ch,
            TO_CHAR,
            POS_DEAD,
            TRUE
         );
         if (IS_SET(d->character->act2, PLR_LOG_SPEC))
         {
            log_convo(d->character, ch, TYPE_CLAN2, argument);
         }
      }
   }
   return;
}

void obj_say_21670(CHAR_DATA* ch, char* argument, OBJ_DATA* obj)
{
   AFFECT_DATA af;

   if
   (
      (IS_NPC(ch) || ch->pcdata->brand_rank > 1) &&
      stristr(argument, "I call upon the Shield of Innocence")
   )
   {
      if (is_affected(ch, gsn_drinlinda_defense_brand))
      {
         act
         (
            "The Shield of Innocence appears before you, flickers, and disappears.",
            ch,
            obj,
            NULL,
            TO_CHAR
         );
         act
         (
            "The Shield of Innocence appears before $n, flickers, and disappears.",
            ch,
            obj,
            NULL,
            TO_ROOM
         );
         return;
      }

      af.where     = TO_AFFECTS;
      af.location  = APPLY_NONE;
      af.modifier  = 0;
      af.type      = gsn_drinlinda_defense_brand;
      af.level     = ch->level;
      af.duration  = 10;
      af.bitvector = 0;
      affect_to_char(ch, &af);
      send_to_char
      (
         "The Shield of Innocence appears before you and glows brightly before disappearing.\n\r",
         ch
      );
      act
      (
         "The Shield of Innocence appears before $n and glows brightly before disappearing.",
         ch,
         obj,
         NULL,
         TO_ROOM
      );
   }
}

void obj_say_20025(CHAR_DATA* ch, char* argument, OBJ_DATA* obj)
{
   AFFECT_DATA af;

   if (stristr(argument, "Slaughter and destruction rain upon the land"))

   {
      if (is_affected(ch, gsn_drithentir_vampiric_brand))
      {
         act
         (
            "A ravenous vampiric aura settles over you for a moment before flowing to your feet and fading.",
            ch,
            obj,
            NULL,
            TO_CHAR
         );
         act
         (
            "A ravenous vampiric aura settles over $n for a moment before flowing to $s feet and fading.",
            ch,
            obj,
            NULL,
            TO_ROOM
         );
         return;
      }
      af.where     = TO_AFFECTS;
      af.location  = APPLY_NONE;
      af.modifier  = 0;
      af.type      = gsn_drithentir_vampiric_brand;
      af.level     = ch->level;
      af.duration  = 10;
      af.bitvector = 0;
      affect_to_char(ch, &af);
      send_to_char
      (
         "A ravenous vampiric aura settles over you.\n\r",
         ch
      );
      act
      (
         "A ravenous vampiric aura settles over $n.",
         ch,
         obj,
         NULL,
         TO_ROOM
      );
      return;
   }
}
