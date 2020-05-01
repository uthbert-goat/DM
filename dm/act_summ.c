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

static const char rcsid[] = "$Id: act_summ.c,v 1.8 2004/09/08 02:46:07 xurinos Exp $";

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

DECLARE_DO_FUN(do_say);
DECLARE_DO_FUN(do_cast);

/* List of summon prototypes */
void summon_angel(CHAR_DATA* ch);
void summon_null(CHAR_DATA* ch);


/* List of summonable denizens */
typedef void SUMMON_FUN(CHAR_DATA* ch);

typedef struct
{
   char*          name;
   SUMMON_FUN*    summonFunction;
} summon_type;

const summon_type summon_table[] =
{
   {"angel", summon_angel},
   {NULL, summon_null}
};

/* The Main Summon Command */
void do_summon( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   int summon_index = 0;

   /* Check for valid users of this command. */
   if
   (
      !(
         has_skill(ch, gsn_guardian_angel) &&
         get_skill(ch, gsn_guardian_angel) > 0
      )
   )
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }

   /* Identify thing to be summoned */
   one_argument(argument, arg);
   if (arg[0] == '\0')
   {
      send_to_char("What would you like to summon?\n\r", ch);
      return;
   }

   while (summon_table[summon_index].name != NULL)
   {
      if (!str_prefix(arg, summon_table[summon_index].name))
      {
         break;
      }
      summon_index++;
   }

   if (summon_table[summon_index].summonFunction)
   {
      (*summon_table[summon_index].summonFunction)(ch);
   }

   return;
}


/* Summon functions */
void summon_null(CHAR_DATA* ch)
{
   send_to_char( "You do not know how to summon that.\n\r", ch);
   return;
}

void summon_angel(CHAR_DATA* ch)
{
   CHAR_DATA* mob;
   int chance;

   /* Doublecheck to make sure the summoner has the skill. */
   if
   (
      (
         chance = get_skill(ch, gsn_guardian_angel)
      ) < 1
   )
   {
      send_to_char
      (
         "You do not have the power to summon a guardian angel.\n\r",
         ch
      );
      return;
   }

   /* Check to make sure the House power is there. */
   if (house_down(ch, HOUSE_LIGHT))
   {
      return;
   }

   /* Mana check. */
   if (ch->mana < 120)
   {
      send_to_char
      (
         "You do not have enough power to send a summons to the heavens.\n\r",
         ch
      );
      return;
   }

   /* Check to see if a new angel is ready. */
   if (is_affected(ch, gsn_guardian_angel))
   {
      send_to_char
      (
         "You do not wish to intrude upon the heavens for their services so"
         " soon.\n\r", ch
      );
      return;
   }

   /* Existing angel check. */
   for (mob = char_list; mob != NULL; mob = mob->next)
   {
      if ((mob->master == ch) && IS_GUARDIAN_ANGEL(mob))
      {
         send_to_char
         (
            "A guardian angel has already responded to your call.\n\r",
            ch
         );
         return;
      }
   }

   send_to_char("You mentally reach out to the heavens for aid.\n\r", ch);

   if (number_percent() < chance)
   /* Summon an angel. */
   {
      AFFECT_DATA af;

      send_to_char("You sense a response to your summons!\n\r", ch);
      ch->mana -= 120;

      /* Create the mob. */
      mob = create_mobile( get_mob_index(MOB_VNUM_GUARDIAN_ANGEL) );
      mob->level = ch->level;
      mob->max_hit = (ch->max_hit * 3) / 2;
      mob->hit = mob->max_hit;
      mob->damroll += ch->level / 2;
      char_to_room( mob, ch->in_room );

      /* Attach the effect. */
      af.where = TO_AFFECTS;
      af.type = gsn_guardian_angel;
      af.location = 0;
      af.level = ch->level;
      af.modifier = 0;
      af.duration = 15;
      af.bitvector = 0;
      affect_to_char( ch, &af );

      /* Finish role-play. */
      act( "$n flies into the area and lands on $s feet, wings spread majestically.", mob, NULL, NULL, TO_ROOM );
      act( "$N smiles at $n.", ch, NULL, mob, TO_ROOM );
      act( "$N smiles at you.", ch, NULL, mob, TO_CHAR );
      do_say( mob, "Hello, friend.  The gods have sent me in answer to your prayer." );
      do_cast(mob, "sanctuary");
      do_cast( mob, "haste" );
      act( "$n folds $s wings back into a kind of grand cape.", mob, NULL, NULL, TO_ROOM );

      /* Wrap up mob */
      SET_BIT( mob->affected_by, AFF_CHARM );
      add_follower( mob, ch );
      mob->leader = ch;

      check_improve( ch, gsn_guardian_angel, TRUE, 1 );
      WAIT_STATE( ch, skill_table[gsn_guardian_angel].beats );
   }
   else
   /* Fail to summon. */
   {
      ch->mana -= 60;
      send_to_char( "You sense nothing coming through the gate to Heaven.\n\r", ch );
      check_improve( ch, gsn_guardian_angel, FALSE, 1 );
      WAIT_STATE( ch, skill_table[gsn_guardian_angel].beats );
   }

   return;
}

