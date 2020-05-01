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

static const char rcsid[] = "$Id: healer.c,v 1.14 2004/10/18 05:02:13 fizzfaldt Exp $";

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
#include "magic.h"

/* command procedures needed */
DECLARE_DO_FUN(do_emote);
DECLARE_DO_FUN(do_say);

/* Local functions. */
bool    is_occupied args( ( int vnum) );

void do_heal(CHAR_DATA *ch, char *argument)
{
   CHAR_DATA *mob;
   char arg[MAX_INPUT_LENGTH];
   int cost, sn;
   char *words;
   OBJ_DATA *brand;

   /* check for healer */
   for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
   {
      if ( IS_NPC(mob) && IS_SET(mob->act, ACT_IS_HEALER) )
      break;
   }

   if ( mob == NULL )
   {
      send_to_char( "You can't do that here.\n\r", ch );
      return;
   }

   if (mob->position==POS_SLEEPING) {
      send_to_char("The healer must be awake to heal you.\n\r", ch);
      return;
   }
   if (ch->fighting != NULL)
   {
      send_to_char("The healer doesn't want to get in the middle of your fight.\n\r", ch);
      return;
   }
   if (((brand = get_eq_char(ch, WEAR_BRAND)) != NULL)
   && (brand->pIndexData->vnum == 433))
   {
      do_say(mob, "I will never heal you, Forsaken One.");
      return;
   }

   /* Healers will not heal Rebels within Ethshar - Wicket
   if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_REBEL)
   && is_occupied(ch->in_room->vnum)
   && can_see(mob, ch) )
   {
   if (number_range(1, 5) == 1)
   do_emote(mob, "peers around looking for Imperials.");
   else
   do_say(mob, "This is between you and me.");
   }
   */
   one_argument(argument, arg);

   if (ch->in_room->house != 0)
   {
      if (!IS_NPC(ch) && ch->in_room->house != ch->house)
      {
         do_say(mob, "I do not heal intruders in my House!");
         return;
      }
   }

   if (arg[0] == '\0')
   {
      /* display price list */
      act_color
      (
         "$N says '{B{6I offer the following spells:{n'",
         ch,
         NULL,
         mob,
         TO_CHAR
      );
      send_to_char("\n\r", ch);
      send_to_char("  light       : cure light wounds     10 gold\n\r", ch);
      send_to_char("  serious     : cure serious wounds   15 gold\n\r", ch);
      send_to_char("  critic      : cure critical wounds  25 gold\n\r", ch);
      send_to_char("  heal        : healing spell         50 gold\n\r", ch);
      send_to_char("  blind       : cure blindness        20 gold\n\r", ch);
      send_to_char("  disease     : cure disease          15 gold\n\r", ch);
      send_to_char("  poison      : cure poison           25 gold\n\r", ch);
      send_to_char("  uncurse     : remove curse          50 gold\n\r", ch);
      send_to_char("  restoration : restoration          100 gold\n\r", ch);
      send_to_char("  refresh     : restore movement       5 gold\n\r", ch);
      send_to_char("  mana        : restore mana          10 gold\n\r", ch);
      send_to_char("\n\r", ch);
      send_to_char(" Type heal <type> to be healed.\n\r", ch);
      return;
   }

   if (!str_prefix(arg, "light"))
   {
      sn    = gsn_cure_light;
      words = "cure light";
      cost  = 1000;
   }

   else if (!str_prefix(arg, "serious"))
   {
      sn    = gsn_cure_serious;
      words = "cure serious";
      cost  = 1500;
   }

   else if (!str_prefix(arg, "critical"))
   {
      sn    = gsn_cure_critical;
      words = "cure critical";
      cost  = 2500;
   }

   else if (!str_prefix(arg, "heal"))
   {
      sn = gsn_heal;
      words = "heal";
      cost  = 5000;
   }

   else if (!str_prefix(arg, "blindness"))
   {
      sn    = gsn_cure_blindness;
      words = "cure blindness";
      cost  = 2000;
   }

   else if (!str_prefix(arg, "disease"))
   {
      sn    = gsn_cure_disease;
      words = "cure disease";
      cost = 1500;
   }

   else if (!str_prefix(arg, "poison"))
   {
      sn    = gsn_cure_poison;
      words = "cure poison";
      cost  = 2500;
   }

   else if (!str_prefix(arg, "uncurse") || !str_prefix(arg, "curse"))
   {
      sn    = gsn_remove_curse;
      words = "remove curse";
      cost  = 5000;
   }

   else if (!str_prefix(arg, "restoration") || !str_prefix(arg, "restore"))
   {
      sn = gsn_restoration;
      words = "restoration";
      cost = 10000;
   }

   else if (!str_prefix(arg, "mana") || !str_prefix(arg, "energize"))
   {
      sn = -2;
      words = "energizer";
      cost = 1000;
   }


   else if (!str_prefix(arg, "refresh") || !str_prefix(arg, "moves"))
   {
      sn    = gsn_refresh;
      words = "refresh";
      cost  = 500;
   }

   else
   {
      act_color("$N says '{B{6Type 'heal' for a list of spells.{n'",
      ch, NULL, mob, TO_CHAR);
      return;
   }

   if (cost > (ch->gold * 100 + ch->silver))
   {
      act_color("$N says '{B{6You do not have enough gold for my services.{n'",
      ch, NULL, mob, TO_CHAR);
      return;
   }

   WAIT_STATE(ch, PULSE_VIOLENCE);

   deduct_cost(ch, cost);
   /*    mob->gold += cost;*/

   if (sn == -2)  /* restore mana trap...kinda hackish */
   {
      act("$n recites a prayer to $z, '$T'.", mob, NULL, words, TO_ROOM);
      is_supplicating = TRUE;
      if (clergy_fails_help(mob, ch, gsn_heal))
      {
         return;
      }
      is_supplicating = FALSE;
      ch->mana += dice(2, 8) + mob->level / 3;
      ch->mana = UMIN(ch->mana, ch->max_mana);
      send_to_char("A warm glow passes through you.\n\r", ch);
      return;
   }

   if (sn == -1)
   {
      return;
   }

   is_supplicating = TRUE;
   magic_spell_vict
   (
      mob,
      ch,
      CAST_BITS_MOB_PRAYER,
      mob->level,
      sn
   );
   is_supplicating = FALSE;
}
