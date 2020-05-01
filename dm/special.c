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

static const char rcsid[] = "$Id: special.c,v 1.68 2004/11/19 05:28:15 xurinos Exp $";

#if defined(macintosh)
#include <types.h>
#include <time.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "magic.h"
#include "interp.h"

/* command procedures needed */
bool    check_room_protected    args((ROOM_INDEX_DATA *room));

DECLARE_DO_FUN(do_cast);
DECLARE_DO_FUN(do_ram);
DECLARE_DO_FUN(do_bite);
DECLARE_DO_FUN(do_palm);
DECLARE_DO_FUN(do_kick);
DECLARE_DO_FUN(do_throw);
DECLARE_DO_FUN(do_nerve);
DECLARE_DO_FUN(do_crush);
DECLARE_DO_FUN(do_yell);
DECLARE_DO_FUN(do_open);
DECLARE_DO_FUN(do_close);
DECLARE_DO_FUN(do_say);
DECLARE_DO_FUN(do_tail);
DECLARE_DO_FUN(do_backstab);
DECLARE_DO_FUN(do_flee);
DECLARE_DO_FUN(do_murder);
DECLARE_DO_FUN(do_wanted);
DECLARE_DO_FUN(do_look);
DECLARE_DO_FUN(do_follow);
DECLARE_SPELL_FUN(spell_unholy_fire);
DECLARE_DO_FUN(do_emote);
DECLARE_DO_FUN(do_myell);
DECLARE_DO_FUN(do_rescue);
/*
* The following special functions are available for mobiles.
*/
DECLARE_SPEC_FUN(spec_brethren_demon); /* dont use, code does it*/
DECLARE_SPEC_FUN(spec_mtower_necro);
DECLARE_SPEC_FUN(spec_smart_cleric);
DECLARE_SPEC_FUN(spec_smart_mage);
DECLARE_SPEC_FUN(spec_revolutionary);
DECLARE_SPEC_FUN(spec_breath_any);
DECLARE_SPEC_FUN(spec_breath_acid);
DECLARE_SPEC_FUN(spec_breath_fire);
DECLARE_SPEC_FUN(spec_breath_frost);
DECLARE_SPEC_FUN(spec_breath_gas);
DECLARE_SPEC_FUN(spec_breath_lightning);
DECLARE_SPEC_FUN(spec_cast_adept);
DECLARE_SPEC_FUN(spec_bishop);
DECLARE_SPEC_FUN(spec_cast_cleric);
DECLARE_SPEC_FUN(spec_cast_mage);
DECLARE_SPEC_FUN(spec_cast_undead);
DECLARE_SPEC_FUN(spec_executioner);
DECLARE_SPEC_FUN(spec_fido);
DECLARE_SPEC_FUN(spec_guard);
DECLARE_SPEC_FUN(spec_empire);
DECLARE_SPEC_FUN(spec_janitor);
DECLARE_SPEC_FUN(spec_mayor);
DECLARE_SPEC_FUN(spec_poison);
DECLARE_SPEC_FUN(spec_thief);
DECLARE_SPEC_FUN(spec_nasty);
DECLARE_SPEC_FUN(spec_troll_member);
DECLARE_SPEC_FUN(spec_ogre_member);
DECLARE_SPEC_FUN(spec_patrolman);
DECLARE_SPEC_FUN(spec_enforcer);
DECLARE_SPEC_FUN(spec_preserver);
DECLARE_SPEC_FUN(spec_fallen_angel);
DECLARE_SPEC_FUN(spec_troll);
DECLARE_SPEC_FUN(spec_chimera);
DECLARE_SPEC_FUN(spec_bane);
DECLARE_SPEC_FUN(spec_monk);
DECLARE_SPEC_FUN(spec_justicar_monk);
DECLARE_SPEC_FUN(spec_elminster);
DECLARE_SPEC_FUN(spec_outlaw_guardian);
DECLARE_SPEC_FUN(spec_rakshasa);
DECLARE_SPEC_FUN(spec_rakshasa_lich);
DECLARE_SPEC_FUN(spec_rakshasa_royal);
DECLARE_SPEC_FUN(spec_beholder);
DECLARE_SPEC_FUN(spec_deathknight);
DECLARE_SPEC_FUN(spec_room_thow);
DECLARE_SPEC_FUN(spec_room_throw);
DECLARE_SPEC_FUN(spec_spectre);
DECLARE_SPEC_FUN(spec_spectre_king);
DECLARE_SPEC_FUN(spec_hydra);
DECLARE_SPEC_FUN(spec_bone_demon);
DECLARE_SPEC_FUN(spec_valere_knight);
DECLARE_SPEC_FUN(spec_drannor_wraith);
DECLARE_SPEC_FUN(spec_titan);
DECLARE_SPEC_FUN(spec_necromancer);
DECLARE_SPEC_FUN(spec_crusader);
DECLARE_SPEC_FUN(spec_demonologist);
DECLARE_SPEC_FUN(spec_legionlord);
DECLARE_SPEC_FUN(spec_gold_dragon);
DECLARE_SPEC_FUN(spec_black_dragon);
DECLARE_SPEC_FUN(spec_red_dragon);
DECLARE_SPEC_FUN(spec_legion);
DECLARE_SPEC_FUN(spec_eprot);
DECLARE_SPEC_FUN(spec_brethren_guardian);
DECLARE_SPEC_FUN(spec_outlaw_guard);
DECLARE_SPEC_FUN(spec_smart_archmage);
DECLARE_SPEC_FUN(spec_marauder_guardian);

bool area_act args((CHAR_DATA *ch, char *msg, int min_sect, int max_sector));

/* the function table */
const   struct  spec_type    spec_table[] =
{
   {    "spec_breath_any",        spec_breath_any        },
   {    "spec_breath_acid",        spec_breath_acid    },
   {    "spec_breath_fire",        spec_breath_fire    },
   {    "spec_breath_frost",        spec_breath_frost    },
   {    "spec_breath_gas",        spec_breath_gas        },
   {    "spec_breath_lightning",    spec_breath_lightning    },
   {    "spec_cast_adept",        spec_cast_adept        },
   {    "spec_cast_cleric",        spec_cast_cleric    },
   { "spec_bane",    spec_bane    },
   {    "spec_cast_mage",        spec_cast_mage        },
   {    "spec_cast_undead",        spec_cast_undead    },
   {    "spec_executioner",        spec_executioner    },
   {    "spec_fido",            spec_fido        },
/* EMPIRE is obsolete, bring all empire guards back to Justicar */
   {    "spec_empire",           spec_guard},
   {    "spec_guard",            spec_guard},
   {    "spec_janitor",            spec_janitor        },
   {    "spec_mayor",            spec_mayor        },
   {    "spec_poison",            spec_poison        },
   {    "spec_thief",            spec_thief        },
   {    "spec_nasty",            spec_nasty        },
   {    "spec_troll",            spec_troll        },
   {    "spec_troll_member",        spec_troll_member    },
   {    "spec_ogre_member",        spec_ogre_member    },
   {    "spec_patrolman",        spec_patrolman        },
   { "spec_bishop",    spec_bishop        },
   {   "spec_enforcer",        spec_enforcer        },
   {   "spec_preserver",        spec_preserver        },
   {  "spec_fallen_angel",        spec_fallen_angel    },
   {    "spec_monk",        spec_monk    },
   {    "spec_justicar_monk",        spec_justicar_monk    },
   {    "spec_chimera",    spec_chimera},
   {    "spec_elminster",    spec_elminster            },
   { "spec_outlaw_guardian",    spec_outlaw_guardian    },
   { "spec_rakshasa",    spec_rakshasa    },
   { "spec_rakshasa_royal",    spec_rakshasa_royal},
   { "spec_rakshasa_lich",    spec_rakshasa_lich},
   { "spec_room_throw",    spec_room_throw},
   { "spec_beholder",    spec_beholder },
   { "spec_deathknight",    spec_deathknight},
   { "spec_red_dragon", spec_red_dragon},
   {"spec_gold_dragon", spec_gold_dragon},
   { "spec_black_dragon", spec_black_dragon},
   {"spec_spectre",     spec_spectre},
   {"spec_spectre_king",    spec_spectre_king},
   { "spec_hydra",    spec_hydra    },
   { "spec_bone_demon",    spec_bone_demon},
   { "spec_valere_knight",    spec_valere_knight},
   { "spec_drannor_wraith",    spec_drannor_wraith},
   { "spec_necromancer", spec_necromancer},
   { "spec_demonologist", spec_demonologist},
   { "spec_crusader", spec_crusader},
   { "spec_titan", spec_titan},
   { "spec_legion", spec_legion},
   { "spec_legionlord", spec_legionlord},
   { "spec_eprot", spec_eprot},
   { "spec_revolutionary", spec_revolutionary},
   { "spec_smart_mage", spec_smart_mage},
   { "spec_smart_cleric", spec_smart_cleric},
   { "spec_mtower_necro", spec_mtower_necro},
   { "spec_brethren_demon", spec_brethren_demon},
   { "spec_brethren_guardian", spec_brethren_guardian},
   { "spec_outlaw_guard", spec_outlaw_guard},
   { "spec_smart_archmage", spec_smart_archmage},
   { "spec_marauder_guardian", spec_marauder_guardian},
   {    NULL,                NULL            }
};

/*
* Given a name, return the appropriate spec fun.
*/
SPEC_FUN *spec_lookup( const char *name )
{
   int i;

   for ( i = 0; spec_table[i].name != NULL; i++)
   {
      if (LOWER(name[0]) == LOWER(spec_table[i].name[0])
      &&  !str_prefix( name, spec_table[i].name))
      return spec_table[i].function;
   }

   return 0;
}

char *spec_name( SPEC_FUN *function)
{
   int i;

   for (i = 0; spec_table[i].function != NULL; i++)
   {
      if (function == spec_table[i].function)
      return spec_table[i].name;
   }

   return NULL;
}

bool spec_troll_member( CHAR_DATA *ch)
{
   CHAR_DATA *vch, *victim = NULL;
   int count = 0;
   char *message;

   if (!IS_AWAKE(ch) || IS_AFFECTED(ch, AFF_CALM) || ch->in_room == NULL
   ||  IS_AFFECTED(ch, AFF_CHARM) || ch->fighting != NULL)
   return FALSE;

   /* find an ogre to beat up */
   for (vch = ch->in_room->people;  vch != NULL;  vch = vch->next_in_room)
   {
      if (!IS_NPC(vch) || ch == vch)
      continue;

      if (vch->pIndexData->vnum == MOB_VNUM_PATROLMAN)
      return FALSE;

      if (vch->pIndexData->group == GROUP_VNUM_OGRES
      &&  ch->level > vch->level - 2 && !is_safe(ch, vch, IS_SAFE_SILENT | IS_SAFE_AREA_ATTACK))
      {
         if (number_range(0, count) == 0)
         victim = vch;

         count++;
      }
   }

   if (victim == NULL)
   return FALSE;

   /* say something, then raise hell */
   switch (number_range(0, 6))
   {
      default:  message = NULL;     break;
      case 0:    message = "$n yells '{B{4I've been looking for you, punk!{n'";
      break;
      case 1: message = "With a scream of rage, $n attacks $N.";
      break;
      case 2: message =
      "$n says '{B{6What's slimy Ogre trash like you doing around here?{n'";
      break;
      case 3: message = "$n cracks $s knuckles and says '{B{6Do ya feel lucky?{n'";
      break;
      case 4: message = "$n says '{B{6There's no cops to save you this time!{n'";
      break;
      case 5: message = "$n says '{B{6Time to join your brother, spud.{n'";
      break;
      case 6: message = "$n says '{B{6Let's rock.{n'";
      break;
   }

   if (message != NULL)
   act_color(message, ch, NULL, victim, TO_ALL);
   multi_hit( ch, victim, TYPE_UNDEFINED );
   return TRUE;
}

bool spec_ogre_member( CHAR_DATA *ch)
{
   CHAR_DATA *vch, *victim = NULL;
   int count = 0;
   char *message;

   if (!IS_AWAKE(ch) || IS_AFFECTED(ch, AFF_CALM) || ch->in_room == NULL
   ||  IS_AFFECTED(ch, AFF_CHARM) || ch->fighting != NULL)
   return FALSE;

   /* find an troll to beat up */
   for (vch = ch->in_room->people;  vch != NULL;  vch = vch->next_in_room)
   {
      if (!IS_NPC(vch) || ch == vch)
      continue;

      if (vch->pIndexData->vnum == MOB_VNUM_PATROLMAN)
      return FALSE;

      if (vch->pIndexData->group == GROUP_VNUM_TROLLS
      &&  ch->level > vch->level - 2 && !is_safe(ch, vch, IS_SAFE_SILENT | IS_SAFE_AREA_ATTACK))
      {
         if (number_range(0, count) == 0)
         victim = vch;

         count++;
      }
   }

   if (victim == NULL)
   return FALSE;

   /* say something, then raise hell */
   switch (number_range(0, 6))
   {
      default: message = NULL;    break;
      case 0: message = "$n yells '{B{4I've been looking for you, punk!{n'";
      break;
      case 1: message = "With a scream of rage, $n attacks $N.'";
      break;
      case 2: message =
      "$n says '{B{6What's Troll filth like you doing around here?{n'";
      break;
      case 3: message = "$n cracks $s knuckles and says '{B{6Do ya feel lucky?{n'";
      break;
      case 4: message = "$n says '{B{6There's no cops to save you this time!{n'";
      break;
      case 5: message = "$n says '{B{6Time to join your brother, spud.{n'";
      break;
      case 6: message = "$n says '{B{6Let's rock.{n'";
      break;
   }

   if (message != NULL)
   act_color(message, ch, NULL, victim, TO_ALL);
   multi_hit( ch, victim, TYPE_UNDEFINED );
   return TRUE;
}

bool spec_patrolman(CHAR_DATA *ch)
{
   CHAR_DATA *vch,*victim = NULL;
   OBJ_DATA *obj;
   char *message;
   int count = 0;

   if (!IS_AWAKE(ch) || IS_AFFECTED(ch, AFF_CALM) || ch->in_room == NULL
   ||  IS_AFFECTED(ch, AFF_CHARM) || ch->fighting != NULL)
   return FALSE;

   /* look for a fight in the room */
   for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
   {
      if (vch == ch)
      continue;

      if (vch->fighting != NULL)  /* break it up! */
      {
         if (number_range(0, count) == 0)
         victim = (vch->level > vch->fighting->level)
         ? vch : vch->fighting;
         count++;
      }
   }

   if (victim == NULL || (IS_NPC(victim) && victim->spec_fun == ch->spec_fun))
   return FALSE;

   if (((obj = get_eq_char(ch, WEAR_NECK_1)) != NULL
   &&   obj->pIndexData->vnum == OBJ_VNUM_WHISTLE)
   ||  ((obj = get_eq_char(ch, WEAR_NECK_2)) != NULL
   &&   obj->pIndexData->vnum == OBJ_VNUM_WHISTLE))
   {
      act("You blow down hard on $p.", ch, obj, NULL, TO_CHAR);
      act("$n blows on $p, ***WHEEEEEEEEEEEET***", ch, obj, NULL, TO_ROOM);

      for ( vch = char_list; vch != NULL; vch = vch->next )
      {
         if ( vch->in_room == NULL )
         continue;

         if (vch->in_room != ch->in_room
         &&  vch->in_room->area == ch->in_room->area)
         send_to_char( "You hear a shrill whistling sound.\n\r", vch );
      }
   }

   switch (number_range(0, 6))
   {
      default:    message = NULL;        break;
      case 0:    message = "$n yells '{B{4All roit! All roit! break it up!{n'";
      break;
      case 1: message =
      "$n says '{B{6Society's to blame, but what's a bloke to do?{n'";
      break;
      case 2: message =
      "$n mumbles '{B{6bloody kids will be the death of us all.{n'";
      break;
      case 3: message = "$n shouts '{B{4Stop that! Stop that!{n' and attacks.";
      break;
      case 4: message = "$n pulls out his billy and goes to work.";
      break;
      case 5: message =
      "$n sighs in resignation and proceeds to break up the fight.";
      break;
      case 6: message = "$n says '{B{6Settle down, you hooligans!{n'";
      break;
   }

   if (message != NULL)
   act(message, ch, NULL, NULL, TO_ALL);

   multi_hit(ch, victim, TYPE_UNDEFINED);

   return TRUE;
}


bool spec_nasty( CHAR_DATA *ch )
{
   CHAR_DATA *victim, *victim_next;
   long gold;

   if (!IS_AWAKE(ch)) {
      return FALSE;
   }

   if (ch->position != POS_FIGHTING) {
      for ( victim = ch->in_room->people; victim != NULL; victim = victim_next)
      {
         victim_next = victim->next_in_room;
         if (!IS_NPC(victim)
         && (victim->level > ch->level)
         && (victim->level < ch->level + 10))
         {
            do_backstab(ch, victim->name);
            if (ch->position != POS_FIGHTING)
            do_murder(ch, victim->name);
            /* should steal some coins right away? :) */
            return TRUE;
         }
      }
      return FALSE;    /*  No one to attack */
   }

   /* okay, we must be fighting.... steal some coins and flee */
   if ( (victim = ch->fighting) == NULL)
   return FALSE;   /* let's be paranoid.... */

   switch ( number_bits(2) )
   {
      case 0:  act( "$n rips apart your coin purse, spilling your gold!",
      ch, NULL, victim, TO_VICT);
      act( "You slash apart $N's coin purse and gather $S gold.",
      ch, NULL, victim, TO_CHAR);
      act( "$N's coin purse is ripped apart!",
      ch, NULL, victim, TO_NOTVICT);
      gold = victim->gold / 10;  /* steal 10% of his gold */
      victim->gold -= gold;
      ch->gold     += gold;
      return TRUE;

      case 1:  do_flee( ch, "");
      return TRUE;

      default: return FALSE;
   }
}

/*
* Core procedure for dragons.
*/
bool dragon( CHAR_DATA *ch, int spell_name)
{
   CHAR_DATA *victim;
   CHAR_DATA *victim_next;
   int sn;

   if ( ch->position != POS_FIGHTING )
   return FALSE;

   for ( victim = ch->in_room->people; victim != NULL; victim = victim_next )
   {
      victim_next = victim->next_in_room;
      if ( victim->fighting == ch && number_bits( 3 ) == 0 )
      break;
   }

   if ( victim == NULL )
   return FALSE;

   if
   (
      (
         sn = spell_name
      ) < 0 ||
      sn > MAX_SKILL ||
      skill_table[sn].spell_fun == spell_null
   )
   {
      return FALSE;
   }

   if
   (
      ch->race == grn_dragon ||
      ch->race == grn_draconian
   )
   {
      magic_spell_vict(ch, victim, CAST_BITS_MOB_SKILL, ch->level, sn);
   }
   else
   {
      magic_spell_vict(ch, victim, CAST_BITS_MOB_SPELL, ch->level, sn);
   }
   return TRUE;
}



/*
* Special procedures for mobiles.
*/
bool spec_breath_any( CHAR_DATA *ch )
{
   if ( ch->position != POS_FIGHTING )
   return FALSE;

   switch (number_range(0, 4))
   {
      case 0: return spec_breath_fire(ch);
      case 1: return spec_breath_frost(ch);
      case 2: return spec_breath_lightning(ch);
      case 3: return spec_breath_gas(ch);
      case 4: return spec_breath_acid(ch);
   }

   return FALSE;
}



bool spec_breath_acid( CHAR_DATA *ch )
{
   return dragon(ch, gsn_acid_breath);
}



bool spec_breath_fire( CHAR_DATA *ch )
{
   return dragon( ch, gsn_fire_breath);
}



bool spec_breath_frost( CHAR_DATA *ch )
{
   return dragon(ch, gsn_frost_breath);
}



bool spec_breath_gas( CHAR_DATA *ch )
{
   return dragon(ch, gsn_gas_breath);
}



bool spec_breath_lightning( CHAR_DATA *ch )
{
   return dragon(ch, gsn_lightning_breath);
}



bool spec_cast_adept( CHAR_DATA *ch )
{
   CHAR_DATA *victim;
   CHAR_DATA *victim_next;

   if ( !IS_AWAKE(ch) )
   return FALSE;

   for ( victim = ch->in_room->people; victim != NULL; victim = victim_next )
   {
      victim_next = victim->next_in_room;
      if
      (
         victim != ch &&
         can_see(ch, victim) &&
         number_bits( 1 ) == 0 &&
         !IS_NPC(victim) &&
         (
            (
               (victim->level < 11) &&
               (victim->house == 0)
            ) ||
            (
               (ch->in_room->house == victim->house) &&
               (ch->in_room->house != 0)
            ) ||
            (
               (ch->pIndexData->vnum == 101) &&
               (get_obj_wear(victim, "brand coin") != NULL)
            ) ||
            (
               (ch->pIndexData->vnum == 503) &&
               (get_obj_wear(victim, "brand lemniscate") != NULL)
            ) ||
            (
               (ch->pIndexData->vnum == 14600) &&
               (get_obj_wear(victim, "brand ball light") != NULL)
            )
         )
      )

      break;
   }

   if ( victim == NULL )
   return FALSE;

   is_supplicating = TRUE;
   switch ( number_bits( 4 ) )
   {
      case 0:
      magic_spell_vict
      (
         ch,
         victim,
         CAST_BITS_MOB_PRAYER,
         ch->level,
         gsn_armor
      );
      break;

      case 1:
      magic_spell_vict
      (
         ch,
         victim,
         CAST_BITS_MOB_PRAYER,
         ch->level,
         gsn_bless
      );
      break;

      case 2:
      magic_spell_vict
      (
         ch,
         victim,
         CAST_BITS_MOB_PRAYER,
         ch->level,
         gsn_cure_blindness
      );
      break;

      case 3:
      magic_spell_vict
      (
         ch,
         victim,
         CAST_BITS_MOB_PRAYER,
         ch->level,
         gsn_cure_light
      );
      break;

      case 4:
      magic_spell_vict
      (
         ch,
         victim,
         CAST_BITS_MOB_PRAYER,
         ch->level,
         gsn_cure_poison
      );
      break;

      case 5:
      magic_spell_vict
      (
         ch,
         victim,
         CAST_BITS_MOB_PRAYER,
         ch->level,
         gsn_refresh
      );
      break;

      case 6:
      magic_spell_vict
      (
         ch,
         victim,
         CAST_BITS_MOB_PRAYER,
         ch->level,
         gsn_cure_disease
      );
      break;

      default:
      is_supplicating = FALSE;
      return FALSE;
   }
   is_supplicating = FALSE;

   return FALSE;
}



bool spec_cast_cleric( CHAR_DATA *ch )
{
   CHAR_DATA *victim;
   CHAR_DATA *victim_next;
   char *spell;
   int sn;

   if ( ch->position != POS_FIGHTING )
   return FALSE;

   for ( victim = ch->in_room->people; victim != NULL; victim = victim_next )
   {
      victim_next = victim->next_in_room;
      if ( victim->fighting == ch && number_bits( 2 ) == 0 )
      break;
   }

   if ( victim == NULL )
   return FALSE;

   for ( ;; )
   {
      int min_level;

      switch ( number_bits( 4 ) )
      {
         case  0: min_level =  0; spell = "blindness";      break;
         case  1: min_level =  3; spell = "cause serious";  break;
         case  2: min_level =  7; spell = "earthquake";     break;
         case  3: min_level =  9; spell = "cause critical"; break;
         case  4: min_level = 10; spell = "dispel evil";    break;
         case  5: min_level = 12; spell = "curse";          break;
         case  6: min_level = 12; spell = "change sex";     break;
         case  7: min_level = 13; spell = "flamestrike";    break;
         case  8:
         case  9:
         case 10: min_level = 15; spell = "harm";           break;
         case 11: min_level = 15; spell = "plague";       break;
         default: min_level = 16; spell = "dispel magic";   break;
      }

      if ( ch->level >= min_level )
      break;
   }

   if ( ( sn = skill_lookup( spell ) ) < 0 )
   return FALSE;


   is_supplicating = TRUE;
   magic_spell_vict(ch, victim, CAST_BITS_MOB_PRAYER, ch->level, sn);
   is_supplicating = FALSE;
   return TRUE;
}

bool spec_mtower_necro( CHAR_DATA *ch)
{
   CHAR_DATA *victim;
   CHAR_DATA *victim_next;
   /*   OBJ_DATA *corpse; */
   char *spell;
   int sn;
   int try_rescue = FALSE;
   int can_rescue = FALSE;

   if ( ch->position != POS_FIGHTING ){
      /* gonna put corpse animating and golem making here */
      return FALSE;
   }

   if (number_range(0, 3) == 0)
   return FALSE;

   for ( victim = ch->in_room->people; victim != NULL; victim = victim_next )
   {
      victim_next = victim->next_in_room;
      if (victim->fighting == ch)
      try_rescue = TRUE;
      if (victim->master == ch)
      can_rescue = TRUE;
      if ( victim->fighting == ch && number_range(0, 2) == 0 )
      break;
   }

   if ( victim == NULL )
   victim = ch->fighting;

   if (try_rescue && can_rescue){
      /*    do_order(ch, "all rescue"); */
      return TRUE;
   }

   switch(number_range(0, 4)){
      default:
      case 0:
      case 1: spell = "acid blast"; break;
      case 2: spell = "weaken"; break;
      case 3: spell = "wither"; break;
      case 4: spell = "blindness"; break;
   }

   if ( ( sn = skill_lookup( spell ) ) < 0 )
   return FALSE;

   magic_spell_vict(ch, victim, CAST_BITS_MOB_SPELL, ch->level, sn);
   return TRUE;
}

bool spec_smart_cleric( CHAR_DATA *ch)
{
   CHAR_DATA *victim;
   CHAR_DATA *victim_next;
   char * spell;
   int sn;
   bool area_ok = TRUE;

   if ( ch->position != POS_FIGHTING )
   return FALSE;

   if (number_range(0, 3) == 0)
   return FALSE;
   for ( victim = ch->in_room->people; victim != NULL; victim = victim_next )
   {
      victim_next = victim->next_in_room;
      if (victim->fighting != ch && IS_NPC(victim))
      area_ok = FALSE;
   }

   for ( victim = ch->in_room->people; victim != NULL; victim = victim_next )
   {
      victim_next = victim->next_in_room;
      if (victim->fighting == ch && number_range(0, 2) == 0)
      break;
   }

   if ( victim == NULL )
   victim = ch->fighting;

   if (victim == NULL || victim->in_room != ch->in_room) return FALSE;


   if (IS_AFFECTED(ch, AFF_BLIND) && number_range(1, 3) == 3)
   {
      spell = "cure blindness";
      if ( ( sn = skill_lookup( spell ) ) < 0 )
      return FALSE;

      is_supplicating = TRUE;
      magic_spell(ch, "blah self", CAST_BITS_MOB_PRAYER, ch->level, sn);
      is_supplicating = FALSE;
      return TRUE;
   }

   switch(number_range(0, 4)){
      default:
      case 0:
      case 1: spell = "blade barrier"; break;
      case 2:
      spell = "faerie fire";
      if (!IS_AFFECTED(victim, AFF_BLIND))
      spell = "blind";
      if (!IS_AFFECTED(victim, AFF_CURSE))
      spell = "curse";
      break;
      case 3:
      spell = "blade barrier";
      if (IS_AFFECTED(victim, AFF_SANCTUARY))
      {
         spell = "dispel magic";
      }
      break;
      case 4:
      spell = "harm";
      if (number_range(0, 5) == 0)
      spell = "heal";
      break;
   }

   if ( ( sn = skill_lookup( spell ) ) < 0 )
   return FALSE;

   is_supplicating = TRUE;
   magic_spell_vict(ch, victim, CAST_BITS_MOB_SPELL, ch->level, sn);
   is_supplicating = FALSE;
   return TRUE;
}


bool spec_smart_mage( CHAR_DATA *ch)
{
   CHAR_DATA *victim;
   CHAR_DATA *victim_next;
   char *spell;
   int sn;
   int area_ok = TRUE;
   char buf[MAX_STRING_LENGTH];

   /* pick a 2nd victim for summon */
   for (victim = ch->in_room->people; victim != NULL; victim = victim->next_in_room)
   if (!IS_NPC(victim) && victim->id != ch->last_fought && victim->fighting == ch)
   ch->last_fought2 = victim->id;

   /* clear out our 2nd victim if he died */
   victim = id2name(ch->last_fought2, FALSE);
   if (victim != NULL && victim->ghost) ch->last_fought2 = -1;

   if ( ch->fighting == NULL && ch->last_fought != -1)
   {
      if (IS_AFFECTED(ch, AFF_BLIND))
      {
         do_cast(ch, " 'cure blind'");
      }
      victim = id2name(ch->last_fought, FALSE);
      if (ch->last_fought2 != -1 && number_range(1, 3) == 1)
      victim = id2name(ch->last_fought2, FALSE);
      if (victim != NULL && victim->in_room != NULL && ch->in_room != NULL
      &&  victim->in_room->area == ch->in_room->area)
      {
         int last_fought = ch->last_fought;
         sprintf(buf, " summon %s", get_name(victim, ch));
         do_cast(ch, buf);
         if (victim->in_room == ch->in_room && can_see(ch, victim))
         {
            sprintf(buf, "This isn't over yet %s!", PERS(victim, ch));
            do_yell(ch, buf);
            multi_hit(ch, victim, TYPE_UNDEFINED);
            if (ch->last_fought != last_fought)
            ch->last_fought2 = last_fought;
            sprintf(buf, " curse \"%s\"", get_name(victim, ch));
            do_cast(ch, buf);
         }
         return TRUE;
      }
   }

   if ( ch->position != POS_FIGHTING )
   return FALSE;

   if (number_range(0, 3) == 0)
   return FALSE;

   for ( victim = ch->in_room->people; victim != NULL; victim = victim_next )
   {
      victim_next = victim->next_in_room;
      if (victim->fighting != ch && IS_NPC(victim) && !IS_AFFECTED(victim, AFF_CHARM))
      area_ok = FALSE;
   }

   for ( victim = ch->in_room->people; victim != NULL; victim = victim_next )
   {
      victim_next = victim->next_in_room;
      if (victim->fighting == ch && number_range(0, 2) == 0)
      break;
   }

   if ( victim == NULL )
   victim = ch->fighting;
   if (victim == NULL || victim->in_room != ch->in_room) return FALSE;
   if (IS_NPC(victim) && victim->master &&
   victim->master->in_room == ch->in_room)
   {
      /* a rescue we can't have that */
      if (!can_see(ch, victim->master))
      {
         if (victim->master->fighting != ch)
         do_cast(ch, "windwall");
      }
      if (can_see(ch, victim->master))
      {
         ch->fighting = victim->master;
         victim = victim->master;
      }
   }
   if (IS_NPC(victim))
   {
      if (area_ok)
      sprintf(buf, " 'cone of cold'");
      else
      sprintf(buf, " 'icelance'");
      do_cast(ch, buf);
      return TRUE;
   }
   spell = "acid blast";
   if (!area_ok || number_range(0, 5) == 0){
      switch ( number_range( 0, 7 ) )
      {
         case  0: spell = "concat";      break;
         case  1: spell = "earthmaw";    break;
         case  2: spell = "fire and ice"; break;
         case  3: spell = "sunburst";    break;
         case  4: spell = "icelance";    break;
         case  5: spell = "temporal shear"; break;
         case  6: spell = "lower resistance"; break;
         case  7:
         default: spell = "acid blast";     break;
      }
   }
   else
   {
      switch (number_range( 0, 5 ))
      {
         default:
         case 0: spell = "fireball"; break;
         case 1: spell = "chain lightning"; break;
         case 2: spell = "earthquake"; break;
         case 3: spell = "iceball"; break;
         case 5: spell = "windwall"; break;
         case 4: spell = "tsunami"; break;
         if (ch->in_room->sector_type != SECT_WATER_SWIM &&
         ch->in_room->sector_type != SECT_UNDERWATER
         && ch->in_room->sector_type != SECT_WATER_NOSWIM)
         spell = "cone of cold";

      }
   }

   if (ch->pIndexData->vnum == 22117) /* drow master mage rivforst.are */
   {
      if (victim->class == CLASS_CLERIC || victim->class == CLASS_PALADIN)
      if (!is_affected(victim, gsn_black_mantle))
      spell = "black mantle";
      if (ch->hit < 2000)
      {
         switch(number_range(1, 4))
         {
            default:
            case 1: spell = "power word fear"; break;
            case 2: spell = "power word stun"; break;
            case 3: spell = "freeze person"; break;
            case 4:
            sn = gsn_sleep;
            magic_spell_vict(ch, victim, CAST_BITS_MOB_SPELL, ch->level, sn);
            if (IS_AFFECTED(victim, AFF_SLEEP))
            {
               sn = gsn_curse;
               magic_spell_vict(ch, victim, CAST_BITS_MOB_SPELL, ch->level, sn);
               sn = gsn_blindness;
               magic_spell_vict(ch, victim, CAST_BITS_MOB_SPELL, ch->level, sn);
               sn = gsn_weaken;
               magic_spell_vict(ch, victim, CAST_BITS_MOB_SPELL, ch->level, sn);
               sn = gsn_poison;
               magic_spell_vict(ch, victim, CAST_BITS_MOB_SPELL, ch->level, sn);
               sn = gsn_plague;
               magic_spell_vict(ch, victim, CAST_BITS_MOB_SPELL, ch->level, sn);
               sn = gsn_black_mantle;
               magic_spell_vict(ch, victim, CAST_BITS_MOB_SPELL, ch->level, sn);
            }
         }
      }
   }

   if ( ( sn = skill_lookup( spell ) ) < 0 )
   return FALSE;

   magic_spell_vict(ch, victim, CAST_BITS_MOB_SPELL, ch->level, sn);
   return TRUE;
}

bool spec_cast_mage( CHAR_DATA *ch )
{
   CHAR_DATA *victim;
   CHAR_DATA *victim_next;
   char *spell;
   int sn;

   if ( ch->position != POS_FIGHTING )
   return FALSE;

   for ( victim = ch->in_room->people; victim != NULL; victim = victim_next )
   {
      victim_next = victim->next_in_room;
      if ( victim->fighting == ch && number_bits( 2 ) == 0 )
      break;
   }

   if ( victim == NULL )
   return FALSE;

   for ( ;; )
   {
      int min_level;

      switch ( number_bits( 4 ) )
      {
         case  0: min_level =  0; spell = "blindness";      break;
         case  1: min_level =  3; spell = "chill touch";    break;
         case  2: min_level =  7; spell = "weaken";         break;
         case  3: min_level =  8; spell = "burning hands";  break;
         case  4: min_level = 11; spell = "colour spray";   break;
         case  5: min_level = 12; spell = "change sex";     break;
         case  6: min_level = 13; spell = "energy drain";   break;
         case  7:
         case  8: min_level = 15; spell = "fireball";       break;
         case  9: min_level = 18; spell = "teleport";       break;
         case 10: min_level = 20; spell = "plague";       break;
         default: min_level = 20; spell = "acid blast";     break;
      }

      if
      (
         ch->level >= min_level &&
         (
            spell != "teleport" ||
            !number_bits(2)
         )
      )
      {
         break;
      }
   }


   if ( ( sn = skill_lookup( spell ) ) < 0 )
   return FALSE;

   magic_spell_vict(ch, victim, CAST_BITS_MOB_SPELL, ch->level, sn);
   return TRUE;
}



bool spec_cast_undead( CHAR_DATA *ch )
{
   CHAR_DATA *victim;
   CHAR_DATA *victim_next;
   char *spell;
   int sn;

   if ( ch->position != POS_FIGHTING )
   return FALSE;

   for ( victim = ch->in_room->people; victim != NULL; victim = victim_next )
   {
      victim_next = victim->next_in_room;
      if ( victim->fighting == ch && number_bits( 2 ) == 0 )
      break;
   }

   if ( victim == NULL )
   return FALSE;

   for ( ;; )
   {
      int min_level;

      switch ( number_bits( 4 ) )
      {
         case  0: min_level =  0; spell = "curse";          break;
         case  1: min_level =  3; spell = "weaken";         break;
         case  2: min_level =  6; spell = "chill touch";    break;
         case  3: min_level =  9; spell = "blindness";      break;
         case  4: min_level = 12; spell = "poison";         break;
         case  5: min_level = 15; spell = "energy drain";   break;
         case  6: min_level = 18; spell = "harm";           break;
         case  7: min_level = 21; spell = "teleport";       break;
         case  8: min_level = 20; spell = "plague";       break;
         default: min_level = 18; spell = "harm";           break;
      }

      if
      (
         ch->level >= min_level &&
         (
            spell != "teleport" ||
            !number_bits(2)
         )
      )
      {
         break;
      }
   }

   if ( ( sn = skill_lookup( spell ) ) < 0 )
   return FALSE;

   magic_spell_vict(ch, victim, CAST_BITS_MOB_SPELL, ch->level, sn);
   return TRUE;
}


bool spec_executioner( CHAR_DATA *ch )
{
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA *victim;
   CHAR_DATA *victim_next;
   char *crime;

   if ( !IS_AWAKE(ch) || ch->fighting != NULL )
   return FALSE;

   crime = "";
   for ( victim = ch->in_room->people; victim != NULL; victim = victim_next )
   {
      victim_next = victim->next_in_room;

      if (is_affected(victim, gsn_stealth)) continue;

      if ( !IS_NPC(victim) && IS_SET(victim->act, PLR_CRIMINAL)
      &&   can_see(ch, victim))
      { crime = "KILLER"; break; }

      if ( !IS_NPC(victim) && IS_SET(victim->act, PLR_CRIMINAL)
      &&   can_see(ch, victim))
      { crime = "THIEF"; break; }
   }

   if ( victim == NULL )
   return FALSE;

   sprintf( buf, "%s is a criminal!  PROTECT THE INNOCENT!  MORE BLOOOOD!!!",
   PERS(victim, ch));
   REMOVE_BIT(ch->comm, COMM_NOSHOUT);
   do_yell( ch, buf );
   multi_hit( ch, victim, TYPE_UNDEFINED );
   return TRUE;
}



bool spec_fido( CHAR_DATA *ch )
{
   OBJ_DATA *corpse;
   OBJ_DATA *c_next;
   OBJ_DATA *obj;
   OBJ_DATA *obj_next;

   if ( !IS_AWAKE(ch) )
   return FALSE;

   for ( corpse = ch->in_room->contents; corpse != NULL; corpse = c_next )
   {
      c_next = corpse->next_content;
      if ( corpse->item_type != ITEM_CORPSE_NPC )
      continue;

      act( "$n savagely devours a corpse.", ch, NULL, NULL, TO_ROOM );
      for ( obj = corpse->contains; obj; obj = obj_next )
      {
         obj_next = obj->next_content;
         obj_from_obj( obj );
         obj_to_room( obj, ch->in_room );
      }
      extract_obj( corpse, FALSE );
      return TRUE;
   }

   return FALSE;
}



bool spec_guard( CHAR_DATA *ch )
{
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA *victim;
   CHAR_DATA *victim_next;
/*
   CHAR_DATA *ech;
   int max_evil;
*/
   if ( !IS_AWAKE(ch) || ch->fighting != NULL )
   return FALSE;

   if
   (
      ch->pIndexData->vnum != MOB_VNUM_ENFORCER_GUARD &&
      !check_room_protected(ch->in_room)
   )
   {
      return FALSE;
   }

/*
   max_evil = 300;
   ech      = NULL;
*/
   for ( victim = ch->in_room->people; victim != NULL; victim = victim_next )
   {
      victim_next = victim->next_in_room;

      if (is_affected(victim, gsn_stealth))
      {
         continue;
      }

      if
      (
         !IS_NPC(victim) &&
         (
            IS_SET(victim->act, PLR_CRIMINAL) ||
            IS_SET(victim->act2, PLR_LAWLESS)
         ) &&
         can_see(ch, victim) &&
         !is_safe(ch, victim, IS_SAFE_SILENT | IS_SAFE_AREA_ATTACK)
      )
      {
         break;
      }
      /* assisting now based on wanted -- used to be ethos -werv*/
/*
      if
      (
         victim->fighting != NULL &&
         victim->fighting != ch &&
         !IS_NPC(victim) &&
         !IS_NPC(victim->fighting) &&
         victim->pcdata->wanteds > max_evil
      )
      {
         max_evil = victim->pcdata->wanteds;
         ech = victim;
      }
*/
   }

   if ( victim != NULL )
   {
      sprintf( buf, "%s is a criminal!  PROTECT THE INNOCENT!!  BANZAI!!",
      PERS(victim, ch));
      REMOVE_BIT(ch->comm, COMM_NOSHOUT);
      do_yell( ch, buf );
      multi_hit( ch, victim, TYPE_UNDEFINED );
      return TRUE;
   }
/*
   if ( ech != NULL )
   {
      act( "$n screams 'PROTECT THE INNOCENT!!  BANZAI!!",
      ch, NULL, NULL, TO_ROOM );
      multi_hit( ch, ech, TYPE_UNDEFINED );
      return TRUE;
   }
*/
   return FALSE;
}

bool spec_eprot( CHAR_DATA *ch )
{
   CHAR_DATA *victim;
   CHAR_DATA *victim_next;
   char *spell;
   int sn, level;

   level = 56;

   if (ch->position != POS_FIGHTING)
   return FALSE;
   for (victim = ch->in_room->people; victim != NULL; victim = victim_next)
   {
      victim_next = victim->next_in_room;
      if (victim->fighting == ch && number_bits(2) == 0)
      break;
   }

   if (victim == NULL)
   return FALSE;
   spell = "acid blast";

   switch(number_range(0, 6))
   {
      case (0):
      case (1):
      level = 45;        spell = "spiritblade";    break;
      case (2):
      level = 48;     spell = "hand of redemption"; break;
      case (3):
      level = 50;        spell = "acid blast";    break;
      case (4):
      level = 53;        spell = "firestream";    break;
      case (5):
      level = 47;        spell = "earthmaw";    break;
      case (6):
      level = 45;        spell = "concatenate";    break;
   }

   if ( (sn = skill_lookup(spell)) < 0)
   return FALSE;

   magic_spell_vict(ch, victim, CAST_BITS_MOB_SPELL, level, sn);
   return TRUE;
}

bool spec_empire( CHAR_DATA *ch )
{
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA *victim;
   CHAR_DATA *victim_next;
   int ret_val = FALSE;
   int revolution = FALSE;

   return FALSE;

   if (immrp_revolt && ch->in_room &&
   (ch->in_room->vnum >= 10600 && ch->in_room->vnum <= 10799))
   revolution = TRUE;

   if ( !IS_AWAKE(ch) || ch->fighting != NULL )
   return FALSE;

   if (IS_AFFECTED(ch, AFF_CHARM) && !IS_SET(ch->act2, ACT2_AUTOATTACK))
   return FALSE;
   for ( victim = ch->in_room->people; victim != NULL; victim = victim_next )
   {
      victim_next = victim->next_in_room;

      if (!revolution)
      {
         if (!IS_SET(ch->act2, ACT2_AUTOATTACK)
         && is_affected(victim, gsn_stealth)) continue;
         if ( IS_NPC(victim)) continue;
         if ( !IS_SET(victim->act, PLR_CRIMINAL)) continue;
         if (ch->level >= 55)
         {
            if (IS_SET(victim->act, PLR_CRIMINAL))
            {
               sprintf( buf, "%s is a criminal!  PROTECT THE INNOCENT!!  BANZAI!!  MORE BLOOOOD!!!",
               PERS(victim, ch));
            }
            else
            {
               sprintf( buf, "%s is a rebel!  LONG LIVE THE EMPIRE!!!  MORE BLOOOOD!!!", PERS(victim, ch));
            }
         }
         else
         {
            if (IS_SET(victim->act, PLR_CRIMINAL))
            {
               sprintf( buf, "%s is a criminal!  PROTECT THE INNOCENT!!  BANZAI!!",
               PERS(victim, ch));
            }
            else
            {
               sprintf( buf, "%s is a rebel!  LONG LIVE THE EMPIRE!!!", PERS(victim, ch));
            }
         }

      }
      else
      {
         if (victim->house != HOUSE_EMPIRE || IS_IMMORTAL(victim))
         continue;
         sprintf(buf , "Get out of our city, %s.  Home rule for Glyndane! BANZAI!!!", PERS(victim, ch));
      }
      REMOVE_BIT(ch->comm, COMM_NOSHOUT);
      do_yell( ch, buf );
      multi_hit( ch, victim, TYPE_UNDEFINED );
      ret_val = TRUE;
   }
   return ret_val;
}


bool spec_legion( CHAR_DATA *ch )
{
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA *victim;
   CHAR_DATA *victim_next;
   int  crime;

   return FALSE; /* no longer active - werv */
   if ( !IS_AWAKE(ch) || ch->fighting != NULL )
   return FALSE;

   crime    = 0;

   for ( victim = ch->in_room->people; victim != NULL; victim = victim_next )
   {
      victim_next = victim->next_in_room;

      if (is_affected(victim, gsn_stealth)) continue;

      /*
      if ( !IS_NPC(victim) && IS_SET(victim->act, PLR_CRIMINAL)
      &&   can_see(ch, victim))
      { crime = 1; break; }

      if ( !IS_NPC(victim) && IS_SET(victim->act, PLR_REBEL)
      && can_see(ch, victim))
      { crime = 2; break; }
      */
   }


   if ( victim != NULL && crime == 1)
   {
      return FALSE; /* legion not agro to criminals */
      sprintf( buf, "%s is a criminal!  PROTECT THE INNOCENT!!  BANZAI!!",
      PERS(victim, ch));
      REMOVE_BIT(ch->comm, COMM_NOSHOUT);
      do_yell( ch, buf );
      multi_hit( ch, victim, TYPE_UNDEFINED );
      return TRUE;
   }

   if ( victim != NULL && crime == 2)
   {
      sprintf( buf, "%s is a rebel!  PROTECT THE EMPIRE!!  BANZAI!!",
      PERS(victim, ch));
      REMOVE_BIT(ch->comm, COMM_NOSHOUT);
      do_yell( ch, buf );
      multi_hit( ch, victim, TYPE_UNDEFINED );
      return TRUE;
   }


   return FALSE;
}



bool spec_janitor( CHAR_DATA *ch )
{
   OBJ_DATA *trash;
   OBJ_DATA *trash_next;

   if ( !IS_AWAKE(ch) )
   return FALSE;

   for ( trash = ch->in_room->contents; trash != NULL; trash = trash_next )
   {
      trash_next = trash->next_content;
      if
      (
         IS_SET( trash->wear_flags, ITEM_TAKE) &&
         can_loot(ch, trash) &&
         (
            trash->item_type == ITEM_DRINK_CON ||
            trash->item_type == ITEM_TRASH ||
            trash->cost < 10
         ) &&
         can_see_obj(ch, trash) &&
         count_users(trash) == 0 &&
         get_obj(ch, trash, NULL, 0) &&
         trash->pIndexData->house == 0 &&
         trash->carried_by == ch
      )
      {
         act("$n picks up some trash.", ch, NULL, NULL, TO_ROOM);
         return TRUE;
      }
   }

   return FALSE;
}



bool spec_mayor( CHAR_DATA *ch )
{
   static const char open_path[] =
   "W3a3003b33000c111d0d111Oe333333Oe22c222112212111a1S.";

   static const char close_path[] =
   "W3a3003b33000c111d0d111CE333333CE22c222112212111a1S.";

   static const char *path;
   static int pos;
   static bool move;

   if ( !move )
   {
      if (current_mud_time.hour ==  6)
      {
         path = open_path;
         move = TRUE;
         pos  = 0;
      }

      if (current_mud_time.hour == 20)
      {
         path = close_path;
         move = TRUE;
         pos  = 0;
      }
   }

   if ( ch->fighting != NULL )
   return spec_cast_mage( ch );
   if ( !move || ch->position < POS_SLEEPING )
   return FALSE;

   switch ( path[pos] )
   {
      case '0':
      case '1':
      case '2':
      case '3':
      move_char(ch, path[pos] - '0', 0);
      break;

      case 'W':
      ch->position = POS_STANDING;
      ch->on = NULL;
      act( "$n awakens and groans loudly.", ch, NULL, NULL, TO_ROOM );
      break;

      case 'S':
      ch->position = POS_SLEEPING;
      act( "$n lies down and falls asleep.", ch, NULL, NULL, TO_ROOM );
      break;

      case 'a':
      act_color( "$n says '{B{6Hello Honey!{n'", ch, NULL, NULL, TO_ROOM );
      break;

      case 'b':
      act_color( "$n says '{B{6What a view!  I must do something about that dump!{n'",
      ch, NULL, NULL, TO_ROOM );
      break;

      case 'c':
      act_color( "$n says '{B{6Vandals!  Youngsters have no respect for anything!{n'",
      ch, NULL, NULL, TO_ROOM );
      break;

      case 'd':
      act_color( "$n says '{B{6Good day, citizens!{n'", ch, NULL, NULL, TO_ROOM );
      break;

      case 'e':
      act_color( "$n says '{B{6I hereby declare the city of Midgaard open!{n'",
      ch, NULL, NULL, TO_ROOM );
      break;

      case 'E':
      act_color( "$n says '{B{6I hereby declare the city of Midgaard closed!{n'",
      ch, NULL, NULL, TO_ROOM );
      break;

      case 'O':
      /*    do_unlock( ch, "gate" ); */
      do_open( ch, "gate" );
      break;

      case 'C':
      do_close( ch, "gate" );
      /*    do_lock( ch, "gate" ); */
      break;

      case '.' :
      move = FALSE;
      break;
   }

   pos++;
   return FALSE;
}



bool spec_poison( CHAR_DATA *ch )
{
   CHAR_DATA *victim;

   if ( ch->position != POS_FIGHTING
   || ( victim = ch->fighting ) == NULL
   ||   number_percent( ) > 2 * ch->level )
   return FALSE;

   act( "You bite $N!",  ch, NULL, victim, TO_CHAR    );
   act( "$n bites $N!",  ch, NULL, victim, TO_NOTVICT );
   act( "$n bites you!", ch, NULL, victim, TO_VICT    );
   magic_spell
   (
      ch,
      "blah",
      CAST_BITS_MOB_SKILL,
      ch->level,
      gsn_poison
   );
   return TRUE;
}



bool spec_thief( CHAR_DATA *ch )
{
   CHAR_DATA *victim;
   CHAR_DATA *victim_next;
   long gold, silver;

   if ( ch->position != POS_STANDING )
   return FALSE;

   for ( victim = ch->in_room->people; victim != NULL; victim = victim_next )
   {
      victim_next = victim->next_in_room;

      if ( IS_NPC(victim)
      ||   victim->level >= LEVEL_IMMORTAL
      ||   number_bits( 5 ) != 0
      ||   !can_see(ch, victim))
      continue;

      if ( IS_AWAKE(victim) && number_range( 0, ch->level ) == 0 )
      {
         act( "You discover $n's hands in your wallet!",
         ch, NULL, victim, TO_VICT );
         act( "$N discovers $n's hands in $S wallet!",
         ch, NULL, victim, TO_NOTVICT );
         return TRUE;
      }
      else
      {
         gold = victim->gold * UMIN(number_range(1, 20), ch->level / 2) / 100;
         gold = UMIN(gold, ch->level * ch->level * 10 );
         ch->gold     += gold;
         victim->gold -= gold;
         silver = victim->silver * UMIN(number_range(1, 20), ch->level/2)/100;
         silver = UMIN(silver, ch->level*ch->level * 25);
         ch->silver    += silver;
         victim->silver -= silver;
         return TRUE;
      }
   }

   return FALSE;
}


bool spec_enforcer( CHAR_DATA *ch )
{
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA *victim;
   CHAR_DATA *victim_next;
   int count;
   int choice;
   int type;

   if (ch->fighting != NULL)
   {
      switch (guard_type(ch))
      {
         default:
         {
            return FALSE;
         }
         case (GUARD_CURATE):
         {
            if (!number_bits(4))
            {
               if (number_bits(1))
               {
                  is_supplicating = TRUE;
                  magic_spell
                  (
                     ch,
                     "blah",
                     CAST_BITS_MOB_PRAYER,
                     ch->level - 1,
                     gsn_curse
                  );
                  is_supplicating = FALSE;
                  return TRUE;
               }
               else
               {
                  is_supplicating = TRUE;
                  magic_spell
                  (
                     ch,
                     "blah",
                     CAST_BITS_MOB_PRAYER,
                     ch->level - 1,
                     gsn_blindness
                  );
                  is_supplicating = FALSE;
                  return TRUE;
               }
            }
            return FALSE;
         }
         case (GUARD_MAGUS):
         {
            if (!number_bits(4))
            {
               if
               (
                  IS_NPC(ch->fighting) ||
                  (
                     !IS_SET(ch->fighting->act, PLR_CRIMINAL) &&
                     !IS_SET(ch->fighting->act, PLR_LAWLESS)
                  ) ||
                  number_bits(1)
               )
               {
                  magic_spell
                  (
                     ch,
                     "blah",
                     CAST_BITS_MOB_SPELL,
                     ch->level - 1,
                     gsn_dispel_magic
                  );
                  return TRUE;
               }
               else
               {
                  magic_spell
                  (
                     ch,
                     "blah",
                     CAST_BITS_MOB_SPELL,
                     ch->level - 1,
                     gsn_hand_of_vengeance
                  );
                  return TRUE;
               }
            }
            return FALSE;
         }
         case (GUARD_SENTINEL):
         {
            for (victim = ch->in_room->people; victim; victim = victim->next_in_room)
            {
               if
               (
                  IS_NPC(victim) ||
                  victim->fighting == NULL ||
                  IS_NPC(victim->fighting) ||
                  (
                     !IS_SET(victim->fighting->act, PLR_CRIMINAL) &&
                     !IS_SET(victim->fighting->act, PLR_LAWLESS)
                  ) ||
                  !can_see(ch, victim) ||
                  is_safe(ch, victim->fighting, IS_SAFE_SILENT | IS_SAFE_AREA_ATTACK) ||
                  victim->fighting->fighting == ch ||
                  number_bits(1)
               )
               {
                  continue;
               }
               sprintf(buf, "\"%s\"", get_name(victim, ch));
               do_rescue(ch, buf);
               return TRUE;
            }
            return FALSE;
         }
      }
      return FALSE;
   }
   if
   (
      !IS_AWAKE(ch) ||
      ch->fighting != NULL ||
      /* Sentry does not autoattack */
      (
         type = guard_type(ch)
      ) == GUARD_SENTRY ||
      (
         type == -1 &&
         ch->pIndexData->vnum != MOB_VNUM_ENFORCER_GUARD
      ) ||
      (
         IS_AFFECTED(ch, AFF_CHARM) &&
         !IS_SET(ch->act2, ACT2_AUTOATTACK)
      )
   )
   {
      return FALSE;
   }

   if
   (
      (
         ch->leader != NULL &&
         !IS_NPC(ch->leader) &&
         (
            IS_SET(ch->leader->act, PLR_CRIMINAL) ||
            IS_SET(ch->leader->act2, PLR_LAWLESS)
         )
      ) ||
      (
         ch->master != NULL &&
         !IS_NPC(ch->master) &&
         (
            IS_SET(ch->master->act, PLR_CRIMINAL) ||
            IS_SET(ch->master->act2, PLR_LAWLESS)
         )
      )
   )
   {
      send_to_char("Guards will not follow a CRIMINAL!\n\r", ch->leader);
      REMOVE_BIT(ch->affected_by, AFF_CHARM);
      do_follow(ch, "self");
   }
   count = 0;
   for ( victim = ch->in_room->people; victim != NULL; victim = victim_next )
   {
      victim_next = victim->next_in_room;
      if
      (
         IS_NPC(victim) ||
         (
            !IS_SET(victim->act, PLR_CRIMINAL) &&
            !IS_SET(victim->act2, PLR_LAWLESS)
         ) ||
         is_affected(victim, gsn_stealth) ||
         !can_see(ch, victim) ||
         is_safe(ch, victim, IS_SAFE_SILENT | IS_SAFE_AREA_ATTACK)
      )
      {
         continue;
      }
      count++;
   }
   if (count == 0)
   {
      return FALSE;
   }
   choice = number_range(1, count);
   count = 0;
   for (victim = ch->in_room->people; victim != NULL; victim = victim_next)
   {
      victim_next = victim->next_in_room;
      if
      (
         IS_NPC(victim) ||
         (
            !IS_SET(victim->act, PLR_CRIMINAL) &&
            !IS_SET(victim->act2, PLR_LAWLESS)
         ) ||
         is_affected(victim, gsn_stealth) ||
         !can_see(ch, victim) ||
         is_safe(ch, victim, IS_SAFE_SILENT | IS_SAFE_AREA_ATTACK)
      )
      {
         continue;
      }
      if (++count == choice)
      {
         break;
      }
   }
   if (victim != NULL)
   {
      sprintf
      (
         buf,
         "%s is a criminal!  STOP IN THE NAME OF THE LAW!!  BANZAI!!",
         PERS(victim, ch)
      );
      REMOVE_BIT(ch->comm, COMM_NOSHOUT);
      do_yell(ch, buf);
      multi_hit(ch, victim, TYPE_UNDEFINED);
      return TRUE;
   }

   return FALSE;
}

/* Spec fun for preserver */

bool spec_preserver(CHAR_DATA *ch)
{
   CHAR_DATA *victim;
   CHAR_DATA *victim_next;
   char *spell;
   int sn, level;

   level = 56;

   if (ch->position != POS_FIGHTING)
   return FALSE;
   for (victim = ch->in_room->people; victim != NULL; victim = victim_next)
   {
      victim_next = victim->next_in_room;
      if (victim->fighting == ch && number_bits(2) == 0)
      break;
   }

   if (victim == NULL)
   return FALSE;
   spell = "acid blast";

   switch(number_range(0, 6))
   {
      case (0):
      case (1):
      level = 55;        spell = "spiritblade";    break;
      case (2):
      case (3):
      level = 60;        spell = "acid blast";    break;
      case (4):
      level = 63;        spell = "firestream";    break;
      case (5):
      level = 57;        spell = "earthmaw";    break;
      case (6):
      level = 55;        spell = "concatenate";    break;
   }

   if ( (sn = skill_lookup(spell)) < 0)
   return FALSE;

   magic_spell_vict(ch, victim, CAST_BITS_MOB_SPELL, level, sn);
   return TRUE;
}


bool spec_protector(CHAR_DATA *ch)
{
   char buf[MAX_STRING_LENGTH];
   int sn_hand, number;
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;

   if (ch->fighting == NULL)
   return FALSE;

   for (; ;)
   {
      for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
      {
         vch_next = vch->next_in_room;
         if (vch->fighting == ch && number_bits(2) == 0)
         break;
      }
      if (vch != NULL)
      break;
   }

   number = number_percent();
   if (number < 5)
   {
      do_say(ch, "You are breaking the Law! Leave now or be punished!");
      return TRUE;
   }
   else if (number < 10 && number_bits(2) == 0)
   {
      sprintf(buf, "%s, you must pay for your crimes!", vch->name);
      do_say(ch, buf);
      do_wanted(ch, vch->name);
   }

   if (IS_SET(vch->act, PLR_CRIMINAL))
   {
      sn_hand = gsn_retribution;
      act_color("$n points at $N and says, '{B{6Defend the Protectorate!{n'", ch, NULL, vch, TO_NOTVICT);
      act_color("$n points at you and says, '{B{6Defend the Protectorate!{n'", ch, NULL, vch, TO_VICT);
      magic_spell_vict(ch, vch, CAST_BITS_MOB_SPELL, ch->level, sn_hand);
      return TRUE;
   }

   return spec_breath_lightning(ch);
   return FALSE;
}



bool spec_fallen_angel(CHAR_DATA *ch)
{
   int num, sn_unholy_fire, sn_windwall;

   if (ch->fighting == NULL)
   return FALSE;
   num = number_percent();

   if (num >= 50)
   return FALSE;

   sn_unholy_fire = gsn_unholy_fire;
   sn_windwall = gsn_windwall;

   if (num < 10)
   {
      do_say(ch, "Burn in Hell!");
      magic_spell(ch, "blah", CAST_BITS_MOB_SPELL, 60, sn_unholy_fire);
      return TRUE;
   }
   else if (num < 30)
   {
      act("$n rise up on $s blackened wings and beats them into the air!", ch, NULL, NULL, TO_ROOM);
      magic_spell(ch, "blah", CAST_BITS_MOB_SPELL, 60, sn_windwall);
      return TRUE;
   }
   else if (num < 50)
   {
      act("$n hisses and a steaming green gas explodes forth!", ch, NULL, NULL, TO_ROOM);
      return spec_breath_gas(ch);
   }

   return FALSE;
}

void spell_unholy_fire(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   CHAR_DATA* vch;
   CHAR_DATA* vch_next;
   int dam;
   int tmp_dam;
   char buf[MAX_STRING_LENGTH];

   act
   (
      "Black flames erupt from the ground to engulf the room!",
      ch,
      NULL,
      NULL,
      TO_ALL
   );
   dam = dice(level, 6);

   for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
   {
      vch_next = vch->next_in_room;
      if
      (
         is_same_group(ch, vch) ||
         (
            IS_IMMORTAL(vch) &&
            !can_see(ch, vch)
         ) ||
         is_safe(ch, vch, IS_SAFE_SILENT | IS_SAFE_AREA_ATTACK) ||
         oblivion_blink(ch, vch)
      )
      {
         continue;
      }
      if
      (
         !IS_NPC(ch) &&
         !IS_NPC(vch) &&
         (
            ch->fighting == NULL ||
            vch->fighting == NULL
         ) &&
         vch != ch
      )
      {
         sprintf(buf, "Die, %s, you sorcerous dog!", PERS(ch, vch));
         do_myell(vch, buf);
         sprintf
         (
            log_buf,
            "[%s] cast unholy fire upon [%s] at %d",
            ch->name,
            vch->name,
            ch->in_room->vnum
         );
         log_string(log_buf);
      }
      tmp_dam = dam;
      if (saves_spell(ch, level, vch, DAM_NEGATIVE, SAVE_SPELL))
      {
         tmp_dam /= 2;
      }
      damage(ch, vch, dam, sn, DAM_NEGATIVE, TRUE);
   }
   return;
}




/* Spec_fun for Dead Forest area */

bool spec_troll(CHAR_DATA *ch)
{
   int action;
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;

   return FALSE;

   if (!IS_AWAKE(ch))
   return FALSE;

   action = number_range(0, 3);

   if (ch->fighting != NULL)
   action += 4;

   if (action <= 3 && number_percent() > 10)
   return FALSE;
   vch = NULL;
   if (action >= 4)
   {
      for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
      {
         vch_next = vch->next_in_room;
         if (vch->fighting == ch && number_bits(2) == 0)
         break;
      }
   }

   if (action >= 4 && vch == NULL)
   return FALSE;

   switch(action)
   {
      case (0):
      act("$n picks up a piece of dead bark and licks the ichor off it.", ch, NULL, NULL, TO_ROOM);
      break;
      case (1):
      act("$n snarls and sniffs the air expecantly.", ch, NULL, NULL, TO_ROOM);
      break;
      case (2):
      act("$n licks at a pool of dark green blood.", ch, NULL, NULL, TO_ROOM);
      break;
      case (3):
      act("$n scratches at one of its bloody scabs.", ch, NULL, NULL, TO_ROOM);
      break;
      case (4):  /* Combat */
      case (5):
      magic_spell(ch, "blah", CAST_BITS_MOB_SPELL, ch->level, gsn_acid_spit);
      break;
      case (6): /* Combat */
      case (7):
      do_crush(ch, "");
      break;
   }
   return TRUE;
}

/* For spec_troll  */
void spell_acid_spit(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   int dam;
   AFFECT_DATA af;
   CHAR_DATA *victim = (CHAR_DATA *) vo;

   af.where = TO_AFFECTS;
   af.type = sn;
   af.duration = 1;
   af.modifier = -2;
   af.location = APPLY_HITROLL;
   af.bitvector = AFF_BLIND;

   act("$n spits out a spray of burning acid!", ch, NULL, NULL, TO_ROOM);
   send_to_char("You spit forth a spray of acid!\n\r", ch);
   if (saves_spell(ch, level, victim, DAM_ACID, SAVE_SPELL))
   damage(ch, victim, 0, sn, DAM_ACID, TRUE);
   else
   {
      dam = dice(4, 5);
      damage(ch, victim, dam, sn, DAM_ACID, TRUE);
      if (!IS_AFFECTED(victim, AFF_BLIND))
      {
         affect_to_char(victim, &af);
         send_to_char("You are blinded by the searing acid!\n\r", ch);
         act("$n appears blinded by the acid.", victim, NULL, NULL, TO_ROOM);
      }
   }

   return;
}


bool spec_monk(CHAR_DATA *ch)
{
   CHAR_DATA *vch;
   int action;


   if ( (vch = ch->fighting) == NULL)
   return FALSE;

   action = number_range(0, 2);
   if (is_affected(ch, gsn_palm))
   action += 1;

   switch(action)
   {
      case (0):   do_palm(ch, ""); break;
      case (1):   do_throw(ch, "");    break;
      case (2):   case (3):
      do_kick(ch, ""); break;
   }
   return TRUE;
}

bool spec_justicar_monk(CHAR_DATA* ch)
{
   int count;
   int count_mob;
   int choice;
   bool mob;
   CHAR_DATA* vch;
   CHAR_DATA* master;

   if (!ch->fighting)
   {
      return FALSE;
   }
   if (!number_bits(2))
   {
      count = 0;
      count_mob = 0;
      for (vch = ch->in_room->people; vch; vch = vch->next_in_room)
      {
         if
         (
            ch == vch ||
            is_safe(ch, vch, IS_SAFE_SILENT)
         )
         {
            continue;
         }
         master = vch;
         while
         (
            master->master &&
            IS_AFFECTED(master, AFF_CHARM)
         )
         {
            master = master->master;
         }
         if
         (
            IS_NPC(master) ||
            (
               !IS_SET(master->act, PLR_CRIMINAL) &&
               !IS_SET(master->act2, PLR_LAWLESS)
            )
         )
         {
            continue;
         }
         if (IS_NPC(vch))
         {
            count_mob++;
         }
         else
         {
            count++;
         }
      }
      if
      (
         count > 0 ||
         count_mob > 0
      )
      {
         if (count > 0)
         {
            choice = number_range(1, count);
            mob = FALSE;
         }
         else
         {
            choice = number_range(1, count_mob);
            mob = TRUE;
         }
         count = 0;
         for (vch = ch->in_room->people; vch; vch = vch->next_in_room)
         {
            if
            (
               ch == vch ||
               is_safe(ch, vch, IS_SAFE_SILENT)
            )
            {
               continue;
            }
            master = vch;
            while
            (
               master->master &&
               IS_AFFECTED(master, AFF_CHARM)
            )
            {
               master = master->master;
            }
            if
            (
               IS_NPC(master) ||
               (
                  !IS_SET(master->act, PLR_CRIMINAL) &&
                  !IS_SET(master->act2, PLR_LAWLESS)
               )
            )
            {
               continue;
            }
            if
            (
               (
                  (
                     mob &&
                     IS_NPC(vch)
                  ) ||
                  (
                     !IS_NPC(vch) &&
                     !mob
                  )
               ) &&
               ++count == choice
            )
            {
               break;
            }
         }
         if (vch != NULL)
         {
            magic_spell_vict
            (
               ch,
               vch,
               CAST_BITS_MOB_SPELL,
               LEVEL_HERO,
               gsn_hand_of_vengeance
            );
            return TRUE;
         }
      }
   }
   return spec_monk(ch);
}

bool spec_chimera(CHAR_DATA *ch)
{
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;
   int action, sn;

   if (ch->fighting == NULL)
   return FALSE;

   action = number_range(0, 3);
   for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
   {
      vch_next = vch->next_in_room;
      if (vch->fighting == ch && number_bits(2) == 0)
      break;
   }
   if (vch == NULL)
   vch = ch->fighting;

   switch(action)
   {
      case (0):
      do_ram(ch, vch->name);
      break;
      case (1):
      act("$n's dragon head opens its giant maw.", ch, NULL, NULL, TO_ROOM);
      sn = gsn_fire_breath;
      magic_spell_vict(ch, vch, CAST_BITS_MOB_SPELL, ch->level, sn);
      break;
      case (2):
      do_bite(ch, vch->name);
      break;
      case (3):
      do_tail(ch, vch->name);
      break;
   }

   return TRUE;
}



bool evades(CHAR_DATA *ch, CHAR_DATA *victim)
{
   int chance;

   chance = 50 + ((ch->level - victim->level) * 2);
   chance += get_curr_stat(ch, STAT_DEX)/3;
   chance -= get_curr_stat(victim, STAT_DEX)/2;
   if (number_percent() > chance)
   return TRUE;
   return FALSE;
}

bool evades_ram(CHAR_DATA *ch, CHAR_DATA *victim)
{
   int chance;
   OBJ_DATA* obj;

   /*char buf[MAX_INPUT_LENGTH];  Used for testing and outputting chance */

   chance = 50 + ((ch->level - victim->level) * 2);
   if ((obj = get_eq_char(ch, WEAR_HORNS)))
   {
      chance += (obj->level) / 2;
   }
   chance += get_curr_stat(ch, STAT_DEX)/3;
   chance -= get_curr_stat(victim, STAT_DEX)/2;

   /* sprintf(buf, "chance = %d", chance);
   send_to_char(buf, ch); Used for testing and outputting chance*/

   if (number_percent() > chance)
   return TRUE;
   return FALSE;
}


void do_ram2(CHAR_DATA *ch, char *argument)
{
   CHAR_DATA *vch;
   char arg[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   int dam;
   OBJ_DATA* obj;


   if (IS_NPC(ch))
   {
      do_ram(ch, argument);
      return;
   }

   if
   (
      ch->pcdata->learned[gsn_chimera_goat] > 1 &&
      has_skill(ch, gsn_chimera_goat)
   )
   {
      do_ram(ch, argument);
      return;
   }

   one_argument(argument, arg);

   vch = get_char_room(ch, arg);
   if (arg[0] == '\0')
   vch = ch->fighting;


   if
   (
      get_skill(ch, gsn_ramming) <= 0 ||
      !has_skill(ch, gsn_ramming)
   )
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }

   if (vch == NULL)
   {
      send_to_char("You aren't fighting anyone.\n\r", ch);
      return;
   }
   if (!IS_NPC(ch) && is_safe(ch, vch, 0))
   return;

   if (oblivion_blink(ch, vch))
   {
      return;
   }
   if (check_shadowstrike(ch, TRUE, TRUE))
   {
      return;
   }

   if (vch == ch)
   {
       send_to_char("That is physically impossible.\n\r", ch);
       return;
   }

   if (vch->daze > 0)
   {
      act
      (
         "You will have to let $M get back up first.",
         ch,
         NULL,
         vch,
         TO_CHAR
      );
      return;
   }
   if
   (
      (
         ch->fighting == NULL ||
         vch->fighting == NULL
      ) && !IS_NPC(vch)
   )
   {
      sprintf(buf, "Help! %s is swinging horns at me!", PERS(ch, vch));
      do_myell(vch, buf);
      sprintf
      (
         log_buf,
         "%s rammed %s at Room [%d].",
         ch->name,
         vch->name,
         ch->in_room->vnum
      );
      log_string(log_buf);
   }

   if (evades_ram(ch, vch))
   {
      act("$n brings $s horns around in a violent swing but misses.", ch, NULL, NULL, TO_ROOM);
      send_to_char("You bring your horns around violently but miss.\n\r", ch);
      dam = 0;
   }
   else
   {
      act("$n slams $s horns into $N, sending $M sprawling!", ch, NULL, vch, TO_NOTVICT);
      act("$n slams $s horns into you, sending you sprawling!", ch, NULL, vch, TO_VICT);
      act("You slam your horns into $N, sending $M sprawling!", ch, NULL, vch, TO_CHAR);
      if ((obj = get_eq_char(ch, WEAR_HORNS)))
      {
         dam = number_range(ch->level, ch->level * 3 / 2) + obj->level;
      }
      else
      {
         dam = number_range(ch->level, ch->level * 3 / 2);
      }
      act("You furiously ram $N to the ground!", ch, NULL, vch, TO_CHAR);
      DAZE_STATE(vch, 2 * PULSE_VIOLENCE);
   }

   /* sprintf(buf, "dam = %d", dam);
   send_to_char(buf, ch); Used for testing and outputting dam */

   damage(ch, vch, dam, gsn_ramming, DAM_BASH, TRUE);
   WAIT_STATE(ch, 2*PULSE_VIOLENCE);
   return;
}

void do_ram(CHAR_DATA *ch, char *argument)
{
   CHAR_DATA *vch;
   char arg[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   int dam;

   one_argument(argument, arg);

   vch = get_char_room(ch, arg);
   if (arg[0] == '\0')
   vch = ch->fighting;

   if (IS_NPC(ch) && IS_AFFECTED(ch, AFF_CHARM))
   return;

   if (!IS_NPC(ch))
   {
      if (get_skill(ch, gsn_chimera_goat) <= 0)
      {
         send_to_char("Huh?\n\r", ch);
         return;
      }
   }
   if (vch == ch)
   {
      send_to_char("Amusing idea, but how?\n\r", ch);
      return;
   }
   if (vch == NULL)
   {
      send_to_char("You aren't fighting anyone.\n\r", ch);
      return;
   }
   if (!IS_NPC(ch) && is_safe(ch, vch, 0))
   return;

   if (oblivion_blink(ch, vch))
   {
      return;
   }

   if (check_shadowstrike(ch, TRUE, TRUE))
   {
      return;
   }

   if
   (
      (
         ch->fighting == NULL ||
         vch->fighting == NULL
      ) && !IS_NPC(vch)
   )
   {
      sprintf(buf, "Help! %s is ramming into me!", PERS(ch, vch));
      do_myell(vch, buf);
      sprintf
      (
         log_buf,
         "%s rammed %s at Room [%d].",
         ch->name,
         vch->name,
         ch->in_room->vnum
      );
      log_string(log_buf);
   }

   if (evades(ch, vch))
   {
      act("$n brings $s goat head around in a violent swing but misses.", ch, NULL, NULL, TO_ROOM);
      send_to_char("You bring your goat's head around violently but miss.\n\r", ch);
      dam = 0;
   }
   else
   {
      act("$n slams $s goat head into $N, sending $S sprawling!", ch, NULL, vch, TO_NOTVICT);
      act("$n slams $s goat head into you, sending you sprawling!", ch, NULL, vch, TO_VICT);
      act("You slam your goat head into $N, sending $M sprawling!", ch, NULL, vch, TO_CHAR);
      dam = dice(ch->level, 4);
      DAZE_STATE(vch, 2*PULSE_VIOLENCE);
   }
   damage(ch, vch, dam, gsn_chimera_goat, DAM_BASH, TRUE);
   WAIT_STATE(ch, 2*PULSE_VIOLENCE);
   return;
}

void do_bite(CHAR_DATA *ch, char *argument)
{
   CHAR_DATA *vch;
   char arg[MAX_INPUT_LENGTH];

   int dam;
   one_argument(argument, arg);

   if
   (
      IS_NPC(ch) &&
      IS_AFFECTED(ch, AFF_CHARM) &&
      !IS_SET(ch->act2, ACT_STRAY) &&
      !IS_SET(ch->act2, ACT_NICE_STRAY)
   )
   {
      return;
   }

   if (!IS_NPC(ch) && get_skill(ch, gsn_chimera_lion) <= 0)
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }
   vch = get_char_room(ch, arg);
   if (arg[0] == '\0')
   vch = ch->fighting;
   if (vch == NULL)
   {
      send_to_char("They aren't here.\n\r", ch);
      return;
   }

   if (oblivion_blink(ch, vch))
   {
      return;
   }

   act("$n brings $s head around to bite $N!", ch, NULL, vch, TO_NOTVICT);
   act("$n brings $s head around to bite you!", ch, NULL, vch, TO_VICT);
   act("You bite down savagely at $N!", ch, NULL, vch, TO_CHAR);
   if (evades(ch, vch))
   dam = 0;
   else
   dam = dice(ch->level, 5);
   damage(ch, vch, dam, gsn_chimera_lion, DAM_PIERCE, TRUE);
   WAIT_STATE(ch, 2*PULSE_VIOLENCE);
   return;
}


bool spec_elminster(CHAR_DATA *ch)
{
   CHAR_DATA *victim;
   CHAR_DATA *victim_next;
   char *spell;
   int sn;
   int sn_elemental;
   int val;
   char buf[MAX_STRING_LENGTH];

   sn_elemental = 0;
   val = number_range(0, 8);
   if (val < 6)
   sn_elemental = gsn_familiar;
   /*
   sn_elemental = skill_lookup("summon earth elemental");
   else if (val == 1)
   sn_elemental = skill_lookup("summon air elemental");
   else if (val == 2)
   sn_elemental = skill_lookup("summon fire elemental");
   else if (val == 3)
   sn_elemental = gsn_familiar;
   else
   sn_elemental = 0;
   */
   if (ch->position != POS_FIGHTING)
   {
      if (ch->last_fought <= 0)
      return FALSE;
      if (number_percent() < 20)
      return FALSE;
      victim = id2name(ch->last_fought, FALSE);
      if (!victim) return FALSE;
      if (victim->in_room->area != ch->in_room->area)
      return FALSE;
      if (victim->fighting != NULL)
      return FALSE;
      if (victim->position == POS_FIGHTING)
      return FALSE;
      if (saves_spell(ch, ch->level, victim, DAM_OTHER, SAVE_OTHER))
      return FALSE;
      act("$n has summoned you!", ch, NULL, victim, TO_VICT);
      act("$n disappears!", victim, NULL, NULL, TO_ROOM);
      char_from_room(victim);
      char_to_room(victim, ch->in_room);
      do_observe(victim, "", LOOK_AUTO);
      sprintf(buf, "Help! I'm being summoned by %s!", ch->short_descr);
      do_yell(victim, buf);
      multi_hit(ch, victim,-1);
      return TRUE;
   }


   for (victim = ch->in_room->people; victim != NULL; victim = victim_next)
   {
      victim_next = victim->next_in_room;
      if (victim->fighting == ch && number_bits(2)  == 0)
      break;
   }
   if (victim == NULL && sn_elemental == 0)
   return FALSE;
   if (victim == NULL)
   {
      magic_spell(ch, "blah", CAST_BITS_MOB_SPELL, ch->level, sn_elemental);
      return TRUE;
   }

   switch (number_range(0, 9))
   {
      default: spell = "concatenate"; break;
      case 0: case 1: spell = "concatenate";  break;
      case 2: case 3: spell = "fireball"; break;
      case 4: spell = "windwall"; break;
      case 5: spell = "blindness";    break;
      case 6: spell = "cone of cold"; break;
      case 7: spell = "acid blast";    break;
      case 8: case 9: spell = "firestream";   break;
   }

   if ( (sn = skill_lookup(spell)) < 0)
   return FALSE;
   magic_spell_vict(ch, victim, CAST_BITS_MOB_SPELL, ch->level, sn);
   return TRUE;
}


bool spec_outlaw_guardian(CHAR_DATA *ch)
{
   CHAR_DATA *victim;
   CHAR_DATA *victim_next;
   int sn;

   if ( ch->position != POS_FIGHTING )
   return FALSE;

   for ( victim = ch->in_room->people; victim != NULL; victim = victim_next )
   {
      victim_next = victim->next_in_room;
      if (IS_NPC(victim))
      continue;
      if ( victim->fighting == ch && number_bits(2) == 0)
      break;
   }

   if ( victim == NULL )
   return FALSE;

   sn = -1;

   sn = gsn_revolt;

   if ( sn < 0 )
   return FALSE;

   magic_spell_vict(ch, victim, CAST_BITS_MOB_SPELL, 52, sn);
   return TRUE;
}


bool spec_bishop(CHAR_DATA *ch)
{
   OBJ_DATA *sceptre;
   int sn_frenzy;

   if (ch->fighting == NULL)
   return FALSE;

   sceptre = get_eq_char(ch, WEAR_HOLD);

   if (sceptre != NULL && sceptre->pIndexData->vnum == OBJ_VNUM_HEAVENLY_SCEPTRE
   && number_bits(2) == 0)
   {
      sn_frenzy = gsn_heavenly_wrath;
      is_supplicating = TRUE;
      if (!is_affected(ch, sn_frenzy) && number_percent() < 50)
      do_say(ch, "I am the wrath of god");
      else
      do_say(ch, "Feel the force of god");
      is_supplicating = FALSE;
      return TRUE;
   }

   return (spec_cast_cleric(ch));

   return FALSE;
}



bool spec_bane(CHAR_DATA *ch)
{
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;
   int num, sn;
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA *mob;

   num = 0;
   sn = -1;
   if (ch->fighting == NULL && ch->last_fought <= 0)
   {
      if (number_percent() > 2
      && number_percent() > 10)
      return FALSE;

      for (vch = char_list;vch != NULL; vch = vch->next)
      {
         if (vch->in_room ==NULL)
         continue;
         if (vch->in_room->area != ch->in_room->area
         || IS_NPC(vch))
         continue;
         num = number_range(0, 40);
         if (num < 15)
         send_to_char("You feel a cold chill pass over your soul as something unspeakable stirs nearby.\n\r", vch);
         else if (num < 20)
         send_to_char("A demonic laugh echoes softly around you, only to fade as a blue light flares in the distance.\n\r", vch);
         else if (num <= 25)
         send_to_char("You feel something powerful and malevolent watching you from the darkness around you.\n\r", vch);
         else if (num < 30)
         send_to_char("An awesome force of pure darkness clutches your soul but then passes on...\n\r", vch);
         else if (num < 35)
         send_to_char("Something powerful demands your servitude, but it quickly fades as a green light flashes in a nearby tower.\n\r", vch);
         else if (num <= 40)
         send_to_char("You feel the prescence of a divine power seeking something in the ruins...\n\r", vch);
         return TRUE;
      }
   }

   if (ch->position == POS_SLEEPING)
   return FALSE;

   if (ch->fighting == NULL)
   {
      for (vch = char_list; vch != NULL; vch = vch_next)
      {
         vch_next = vch->next_in_room;
         if (vch->in_room != ch->in_room)
         continue;
         if (!is_same_group(ch, vch))
         continue;
         if (number_bits(2) != 0)
         continue;
         sprintf(buf, "Help! %s is attacking me!", PERS(ch, vch));
         do_myell(vch, buf);

         multi_hit(ch, vch, TYPE_UNDEFINED);
         return TRUE;
      }
   }

   if (ch->fighting == NULL && ch->last_fought > 0 && id2name(ch->last_fought, FALSE))
   {
      if (number_percent() > 5)
      return FALSE;
      for (vch = char_list; vch != NULL; vch = vch_next)
      {
         vch_next = vch->next;
         if (vch->in_room->area != ch->in_room->area)
         continue;
         if (is_same_group(vch, id2name(ch->last_fought, FALSE))
         && !saves_spell(ch, ch->level, vch, DAM_OTHER, SAVE_OTHER) )
         {
            act("$n suddenly disappears!", vch, NULL, NULL, TO_ROOM);
            send_to_char("You have been summoned!\n\r", vch);
            char_from_room(vch);
            char_to_room(vch, ch->in_room);
            do_observe(vch, "", LOOK_AUTO);
            act("$n arrives suddenly!", vch, NULL, NULL, TO_ROOM);
            num = number_percent();
            if (num < 30)
            sprintf(buf, "Help! I've been summoned by %s!", PERS(ch, vch));
            else if (num < 70)
            sprintf(buf, "Help! I'm being attacked by %s!", PERS(ch, vch));
            else
            sprintf(buf, "NO! I'm being slayed by %s!", PERS(ch, vch));
            do_myell(vch, buf);
            multi_hit(ch, vch, TYPE_UNDEFINED);
         }
      }
      return TRUE;
   }


   num = number_percent();
   if (num < 10)
   sn = gsn_deathspell;
   else if (num < 20)
   sn = gsn_acid_blast;
   else if (num < 30)
   sn = gsn_demonfire;
   else if (num < 40)
   sn = gsn_iceball;
   else if (num < 50)
   sn = gsn_fireball;
   else if (num < 80)
   sn = -1;
   else if (num < 90)
   {
      do_say(ch, "Burn in the fires of hell!");
      sn = gsn_unholy_fire;
   }

   if (sn == -1)
   {
      for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
      {
         if (IS_NPC(vch) && vch->pIndexData->vnum == MOB_VNUM_SLAYER)
         break;
      }
      if (vch != NULL)
      return FALSE;
      if (number_percent() > 80)
      {
         act("$n's eye's glow bright red but nothing happens.", ch, NULL, NULL, TO_ROOM);
         return TRUE;
      }
      act("$n's eyes glow bright red and a dark figure materialises beside $s!", ch, NULL, NULL, TO_ROOM);
      mob = create_mobile(get_mob_index(MOB_VNUM_SLAYER));
      char_to_room(mob, ch->in_room);
      mob->leader = ch;
      mob->master = ch;
      SET_BIT(mob->affected_by, AFF_CHARM);
      return TRUE;
   }


   for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
   {
      vch_next = vch->next_in_room;
      if (vch->fighting == ch && number_bits(2) == 0)
      break;
   }

   if (vch == NULL && sn != gsn_deathspell
   && sn != gsn_iceball && sn != gsn_fireball)
   return FALSE;

   magic_spell_vict(ch, vch, CAST_BITS_MOB_SPELL, ch->level, sn);
   return TRUE;
}



bool spec_rakshasa(CHAR_DATA *ch)
{
   CHAR_DATA *victim;
   CHAR_DATA *victim_next;

   if (ch->position != POS_FIGHTING)
   return FALSE;

   for ( victim = ch->in_room->people; victim != NULL; victim = victim_next )
   {
      victim_next = victim->next_in_room;
      if ( victim->fighting == ch && number_bits( 2 ) == 0 )
      break;
   }

   if ( victim == NULL )
   return FALSE;
   if (number_percent() < 20)
   do_bite(ch, victim->name);
   else return (spec_cast_cleric(ch));

   return TRUE;
}

bool spec_rakshasa_royal(CHAR_DATA *ch)
{
   CHAR_DATA *victim;
   CHAR_DATA *victim_next;
   int num;
   int sn, lvl;
   char buf[MAX_STRING_LENGTH];

   if (ch->position != POS_FIGHTING)
   return FALSE;
   sn = -1;
   return (spec_rakshasa(ch));
   for ( victim = ch->in_room->people; victim != NULL; victim = victim_next )
   {
      victim_next = victim->next_in_room;
      if ( victim->fighting == ch && number_bits( 2 ) == 0 )
      break;
   }

   if ( victim == NULL )
   return FALSE;

   if (number_percent() < 20)
   {
      do_bite(ch, victim->name);
      return TRUE;
   }
   buf[0] = '\0';
   for (; ;)
   {
      num = number_range(0, 11);
      if (num == 12)
      return FALSE;
      if ((num == 0 || num == 1) && !IS_AFFECTED(victim, AFF_CURSE) )
      break;
      if ((num == 2 || num == 3) && !IS_AFFECTED(victim, AFF_SLOW))
      break;
      if (num >= 4)
      break;
   }

   lvl = ch->level;
   if (num == 0 || num == 1)
   {
      do_say(ch, "I curse thee mortal fool!");
      sn = gsn_curse;
   }
   else if (num == 2 || num == 3)
   {
      sprintf(buf, "Stay still weakling mortal!");
      do_say(ch, buf);
      sn = gsn_slow;
      lvl += 5;
   }
   else if (num == 4 || num == 5 )
   {
      do_say(ch, "What a tasty meal you will make!");
      sn = gsn_acid_blast;
      lvl = 55;
   }
   else if (num == 6)
   {
      do_say(ch, "Begone from here intruder!");
      sn = gsn_teleport;
      lvl = 50;
   }
   else if (num == 7 || num == 8 )
   {
      act("$n opens $s mouth and a raging fireball explodes forth!", ch, NULL, NULL, TO_ROOM);
      sn = gsn_fireball;
      lvl = 55;
   }
   else if (num == 9 || num == 10)
   {
      sn = gsn_blindness;
      lvl = 55;
   }
   else if (num == 11)
   {
      sn = gsn_energy_drain;
   }

   if (sn < 0)
   return FALSE;

   magic_spell_vict(ch, victim, CAST_BITS_MOB_SPELL, ch->level, sn);
   return TRUE;
}

bool spec_rakshasa_lich(CHAR_DATA *ch)
{
   CHAR_DATA *victim;
   CHAR_DATA *victim_next;
   int num;
   int sn, lvl;
   char buf[MAX_STRING_LENGTH];

   if (ch->position != POS_FIGHTING)
   return FALSE;
   sn = -1;
   return (spec_rakshasa(ch));

   for ( victim = ch->in_room->people; victim != NULL; victim = victim_next )
   {
      victim_next = victim->next_in_room;
      if ( victim->fighting == ch && number_bits( 2 ) == 0 )
      break;
   }

   if ( victim == NULL )
   return FALSE;

   for (; ;)
   {
      num = number_range(0, 11);
      if (num == 12)
      return FALSE;
      if ((num == 0 || num == 1) && !IS_AFFECTED(victim, AFF_CURSE) )
      break;
      if ((num == 2 || num == 3) && !IS_AFFECTED(victim, AFF_SLOW))
      break;
      if (num >= 4)
      break;
   }
   buf[0] = '\0';

   lvl = ch->level;
   if (num == 0 || num == 1)
   {
      do_say(ch, "I curse thee mortal fool!");
      sn = gsn_curse;
   }
   else if (num == 2 || num == 3)
   {
      do_say(ch, "Stay still impudent mortal!");
      sn = gsn_slow;
      lvl += 5;
   }
   else if (num == 4 || num == 5 )
   {
      if (!IS_NPC(victim))
      {
         if (number_percent() < 50)
         sprintf(buf, "Your corpse shall make a fine servant!");
         else
         sprintf(buf, "It is time for you to be taught a lesson!");
      }

      do_say(ch, buf);
      sn = gsn_acid_blast;
      lvl = 60;
   }
   else if (num == 6)
   {
      sn = gsn_demonfire;
      lvl = 50;
   }
   else if (num == 7 || num == 8 )
   {
      act("$n opens $s mouth and a raging fireball explodes forth!", ch, NULL, NULL, TO_ROOM);
      sn = gsn_fireball;
      lvl = 55;
   }
   else if (num == 9 || num == 10)
   {
      sn = gsn_blindness;
      lvl = 55;
   }
   else if (num == 11)
   {
      sn = gsn_energy_drain;
   }

   if (sn < 0)
   return FALSE;

   magic_spell_vict(ch, victim, CAST_BITS_MOB_SPELL, lvl, sn);
   return TRUE;
}

/*
* spec_fun for beholders, can use up to 3 eyes per round. Had to
* mess with the spell selection to prevent same spell being used by mutliple
* eyes though. Messy chunky code.
*/
bool spec_beholder(CHAR_DATA *ch)
{
   CHAR_DATA* victim  = NULL;
   CHAR_DATA* victim1 = NULL;
   CHAR_DATA* victim2 = NULL;
   CHAR_DATA* victim3 = NULL;
   CHAR_DATA* victim_next  = NULL;
   char* spell1 = NULL;
   char* spell2 = NULL;
   char* spell3 = NULL;
   int i;
   sh_int sn1 = 0;
   sh_int sn2 = 0;
   sh_int sn3 = 0;
   sh_int target = TARGET_CHAR;

   if (ch->position != POS_FIGHTING)
   {
      return FALSE;
   }

   for (i = 0; i < 3; i++)
   {
      if (victim3 != NULL)
      {
         break;
      }
      for (victim = ch->in_room->people; victim != NULL; victim = victim_next)
      {
         victim_next = victim->next_in_room;
         if (victim->fighting == ch && number_percent() < 30)
         {
            if (victim1 == NULL)
            {
               victim1 = victim;
            }
            else if (victim2 == NULL)
            {
               victim2 = victim;
            }
            else if (victim3 == NULL)
            {
               victim3 = victim;
            }
            else
            {
               break;
            }
         }
      }
   }

   if ( victim1 == NULL )
   {
      return FALSE;
   }

   switch (number_bits(4))
   {
      case 0:  spell1 = "blindness";       break;
      case 1:  spell1 = "energy drain";    break;
      case 2:  spell1 = "harm";            break;
      case 3:  spell1 = "weaken";          break;
      case 4:  spell1 = "power word stun"; break;
      case 5:  spell1 = "harm";            break;
      case 6:  spell1 = "slow";            break;
      case 7:  spell1 = "change sex";      break;
      case 8:  spell1 = "fireball";        break;
      case 9:  spell1 = "iceball";         break;
      default: spell1 = "dispel magic";    break;
   }
   sn1 = skill_lookup(spell1);


   if (victim2 != NULL)
   {
      while (sn2 == 0)
      {
         switch(number_bits(4))
         {
            case 0:  spell2 = "blindness";       break;
            case 1:  spell2 = "energy drain";    break;
            case 2:  spell2 = "harm";            break;
            case 3:  spell2 = "weaken";          break;
            case 4:  spell2 = "power word stun"; break;
            case 5:  spell2 = "harm";            break;
            case 6:  spell2 = "slow";            break;
            case 7:  spell2 = "change sex";      break;
            case 8:  spell2 = "fireball";        break;
            case 9:  spell2 = "iceball";         break;
            default: spell2 = "dispel magic";    break;
         }
         if (str_cmp(spell1, spell2))
         {
            sn2 = skill_lookup(spell2);
         }
      }
   }

   if (victim3 != NULL)
   {
      while (sn3 == 0)
      {
         switch(number_bits(4))
         {
            case 0:  spell3 = "blindness";       break;
            case 1:  spell3 = "energy drain";    break;
            case 2:  spell3 = "harm";            break;
            case 3:  spell3 = "weaken";          break;
            case 4:  spell3 = "power word stun"; break;
            case 5:  spell3 = "harm";            break;
            case 6:  spell3 = "slow";            break;
            case 7:  spell3 = "change sex";      break;
            case 8:  spell3 = "fireball";        break;
            case 9:  spell3 = "iceball";         break;
            default: spell3 = "dispel magic";    break;
         }
         if (str_cmp(spell1, spell3) && str_cmp(spell2, spell3))
         {
            sn3 = skill_lookup(spell3);
         }
      }
   }

   act("$n turns one of its eyestalks upon $N.", ch, NULL, victim1, TO_NOTVICT);
   act("$n turns one of its eyestalks upon you.", ch, NULL, victim1, TO_VICT);

   if (sn1 >= 0)
   {
      if (skill_table[sn1].target == TAR_IGNORE)
      {
         target = TARGET_NONE;
      }
      else
      {
         target = TARGET_CHAR;
      }
      magic_spell_vict(ch, victim1, CAST_BITS_MOB_SPELL, ch->level, sn1);
   }

   if (victim2 != NULL && sn2 >= 0)
   {
      if (skill_table[sn2].target == TAR_IGNORE)
      {
         target = TARGET_NONE;
      }
      else
      {
         target = TARGET_CHAR;
      }
      act("$n turns one of its eyestalks upon $N.", ch, NULL, victim2, TO_NOTVICT);
      act("$n turns one of its eyestalks upon you.", ch, NULL, victim2, TO_VICT);
      magic_spell_vict(ch, victim2, CAST_BITS_MOB_SPELL, ch->level, sn2);
   }
   if (victim3 != NULL && sn3 >= 0)
   {
      if (skill_table[sn3].target == TAR_IGNORE)
      {
         target = TARGET_NONE;
      }
      else
      {
         target = TARGET_CHAR;
      }
      act("$n turns one of its eyestalks upon $N.", ch, NULL, victim3, TO_NOTVICT);
      act("$n turns one of its eyestalks upon you.", ch, NULL, victim3, TO_VICT);
      magic_spell_vict(ch, victim3, CAST_BITS_MOB_SPELL, ch->level, sn3);
   }

   return TRUE;
}

/* deathknight special */

bool spec_deathknight(CHAR_DATA *ch)
{
   CHAR_DATA *victim;
   CHAR_DATA *victim_next;
   int sn, lvl;
   sn = -1;

   if ( ch->position != POS_FIGHTING )
   return FALSE;

   for ( victim = ch->in_room->people; victim != NULL; victim = victim_next )
   {
      victim_next = victim->next_in_room;
      if ( victim->fighting == ch && number_bits( 2 ) == 0 )
      break;
   }

   if ( victim == NULL )
   return (spec_room_throw(ch));

   lvl = ch->level;

   switch(number_range(0, 16))
   {
      case (0):
      do_say(ch, "Die!");
      sn = gsn_power_word_kill;   lvl = 50;   break;
      case (1):
      case (2):
      case (3):
      case (4):
      case (5):
      do_say(ch, "Fire!");
      sn = gsn_fireball;   break;
      case (6):
      case (7):
      case (8):
      case (9):
      do_say(ch, "Ice!");
      sn = gsn_iceball;    break;
      case (10):
      case (11):
      case (12):
      sn = gsn_curse;  break;
      case (13):
      case (14):
      sn = gsn_energy_drain;   break;
      case (15):
      case (16):
      sn = gsn_wither; lvl = 51;   break;
   }

   if (sn < 0)
   return FALSE;
   magic_spell_vict(ch, victim, CAST_BITS_MOB_SPELL, lvl, sn);
   return TRUE;
}

/* for spectre king in Myth Drannor */
bool spec_spectre_king(CHAR_DATA *ch)
{
   CHAR_DATA *victim;
   CHAR_DATA *victim_next;
   char *spell;
   int sn;

   if ( ch->position != POS_FIGHTING )
   return FALSE;

   for ( victim = ch->in_room->people; victim != NULL; victim = victim_next )
   {
      victim_next = victim->next_in_room;
      if ( victim->fighting == ch && number_bits( 2 ) == 0 )
      break;
   }

   if ( victim == NULL )
   return FALSE;

   switch ( number_range(0, 8) )
   {
      case  0: spell = "blindness";      break;
      case  1: spell = "chill touch";    break;
      case  2: spell = "iceball";         break;
      case  3: spell = "energy drain";       break;
      case  4: spell = "cone of cold";   break;
      case  5: spell = "acid blast";   break;
      default: spell = "frost breath";     break;
   }


   if ( ( sn = skill_lookup( spell ) ) < 0 )
   return FALSE;
   magic_spell_vict(ch, victim, CAST_BITS_MOB_SPELL, ch->level, sn);
   return TRUE;
}

/* throws a victim out of room */

bool spec_room_throw(CHAR_DATA *ch)
{
   ROOM_INDEX_DATA *was_in;
   CHAR_DATA *victim;
   int attempt, chance, dam, level;
   /*    int rev_door; */


   if (ch->position != POS_FIGHTING)
   return FALSE;
   if (number_bits(2) != 0)
   return FALSE;
   return FALSE;
   victim = ch->fighting;
   level = ch->level + 3;
   chance = (ch->level - victim->level)*3;
   chance += number_range(20, 50);
   chance -= get_curr_stat(victim, STAT_DEX)/2;
   chance -= get_curr_stat(victim, STAT_STR)/3;
   chance += get_curr_stat(ch, STAT_STR);

   if (IS_AFFECTED(victim, AFF_HASTE))
   chance -= 30;
   if (IS_AFFECTED(ch, AFF_HASTE))
   chance += 15;
   if (IS_AFFECTED(victim, AFF_SLOW))
   chance += 20;
   if (IS_AFFECTED(ch, AFF_SLOW))
   chance -= 30;
   if (IS_FLYING(victim))
   chance += 5;
   chance += (ch->size - victim->size)*10;

   if (number_percent() > chance)
   {
      act("$n grabs at $N but looses hold.", ch, NULL, victim, TO_NOTVICT);
      act("$n grabs at you but looses hold.", ch, NULL, victim, TO_VICT);
      return FALSE;
   }

   was_in = victim->in_room;
   dam = dice(level, 5);

   for ( attempt = 0; attempt < 6; attempt++ )
   {
      EXIT_DATA *pexit;
      int door;

      door = number_door( );

      if ( ( pexit = was_in->exit[door] ) == 0
      ||   pexit->u1.to_room == NULL
      ||  ( IS_SET(pexit->exit_info, EX_CLOSED) )
      || ( IS_NPC(victim)
      &&   IS_SET(pexit->u1.to_room->room_flags, ROOM_NO_MOB) ) )
      continue;

      if ( ( was_in->sector_type == SECT_AIR)
      ||  ( (pexit->u1.to_room->sector_type == SECT_AIR )
      &&  ( !IS_FLYING(victim) ) ) )
      continue;

      if (IS_SET(pexit->exit_info, EX_CLOSED))
      {
         act(
         "$n grabs $N and sends $M crashing through the door with incredible force!", ch, 0, victim, TO_NOTVICT);
         act("$n grabs you and sends you crashing through the door with incredible force!", ch, NULL, victim, TO_VICT);
      }
      else
      {
         act( "$n grabs $N and hurls $M from the room with incredible force!", ch, NULL, victim, TO_NOTVICT);
         act("$n grabs you and hurls you from the room with incredible force!", ch, NULL, victim, TO_VICT);
         dam = dice(level, 7);
      }
      char_from_room(victim);
      char_to_room(victim, pexit->u1.to_room);
      do_observe(victim, "", LOOK_AUTO);
      if (IS_SET(pexit->exit_info, EX_CLOSED))
      {
         /*
         if (door == 0)
         rev_door = 2;
         else if (door == 1)
         rev_door = 3;
         else if (door == 2)
         rev_door = 0;
         else if (door == 3)
         rev_door = 1;
         else if (door == 4)
         rev_door = 5;
         else
         rev_door = 4;
         */
         REMOVE_BIT(pexit->exit_info, EX_CLOSED);
         /*
         if (victim->in_room->exit[rev_door] != NULL)
         REMOVE_BIT(victim->in_room->exit[rev_door]->exit_info, EX_CLOSED);
         */
         act("With a deafening crash the door explodes open and $n comes flying into the room!", ch, NULL, NULL, TO_ROOM);
      }
      else
      {
         act("$n comes flying into the room with incredible force!", ch, NULL, NULL, TO_ROOM);
      }
      if (saves_spell(ch, ch->level - 3, victim, DAM_BASH, SAVE_OTHER))
      dam /= 2;
      damage(ch, victim, dam, gsn_throw, DAM_BASH, TRUE);
      stop_fighting( ch, TRUE );
      return TRUE;
   }

   act("$n grabs $N and hurls $M brutally into a wall!", ch, NULL, victim, TO_NOTVICT);
   act("$n grabs you and hurtls you brutally into a wall!", ch, NULL, victim, TO_VICT);

   if (saves_spell(ch, ch->level - 3, victim, DAM_BASH, SAVE_OTHER))
   dam /= 2;
   damage(ch, victim, dam, gsn_throw, DAM_BASH, TRUE);

   return TRUE;
}





/* hydra spec fun for myth drannor */

bool spec_hydra(CHAR_DATA *ch)
{
   CHAR_DATA *victim;
   CHAR_DATA *victim1;
   CHAR_DATA *victim2;
   CHAR_DATA *victim3;
   CHAR_DATA *victim_next;
   char *spell1;
   char *spell2;
   char *spell3;
   int level, i;
   int sn1, sn2, sn3;

   if ( ch->position != POS_FIGHTING  && ch->last_fought > 0)
   return FALSE;

   if (ch->position != POS_FIGHTING)
   {
      if (number_percent() < 80)
      return FALSE;
      act("$n swings its heads about in anger then turns and slips back to where it came.", ch, NULL, NULL, TO_ROOM);
      extract_char(ch, TRUE);
      return TRUE;
   }

   sn1 = 0;    sn2 = 0;    sn3 = 0;
   victim1 = NULL;
   victim2 = NULL;
   victim3 = NULL;
   level = ch->level;

   for (i = 0; i < 3; i++)
   {
      if (victim3 != NULL)
      break;
      for ( victim = ch->in_room->people; victim != NULL; victim = victim_next )
      {
         victim_next = victim->next_in_room;
         if ( victim->fighting == ch && number_percent() < 30 )
         {
            if (victim1 == NULL)
            victim1 = victim;
            else if (victim2 == NULL)
            victim2 = victim;
            else if (victim3 == NULL)
            victim3 = victim;
            else
            break;
         }
      }
   }

   if ( victim1 == NULL )
   return FALSE;

   switch(number_range(0, 3))
   {
      case 0:
      spell1 = "acid blast"; break;
      case 1:
      spell1 = "fireball"; level = 50;   break;
      case 3:
      spell1 = "flame scorch"; break;
      default:
      spell1 = "flame scorch"; break;
   }
   sn1 = skill_lookup(spell1);
   spell2 = "weaken";

   if (victim2 != NULL && (sn1 != gsn_flame_scorch))
   {
      switch(number_range(0, 3))
      {
         case 1:
         spell2 = "weaken"; level = 53; break;
         case 2:
         spell2 = "slow"; level = 55;   break;
         case 3:
         spell2 = "weaken";  level = 55; break;
      }
      sn2 = skill_lookup(spell2);
   }

   if (victim3 != NULL && (sn1 != gsn_flame_scorch))
   {
      switch(number_range(0, 4))
      {
         case 1: spell3 = "harm"; level = 58; break;
         case 2: spell3 = "iceball";  level = 55; break;
         default: spell3 = "dispel magic"; level = 54;   break;
      }
      sn3 = skill_lookup(spell3);
   }

   if (sn1 == gsn_flame_scorch)
   {
      act("The hydra's heads swing around and rise up above you.", ch, NULL, NULL, TO_ROOM);
      magic_spell_vict(ch, victim1, CAST_BITS_MOB_SPELL, ch->level, sn1);
      return TRUE;
   }
   else if (  sn1 >= 0 )
   {
      act("$n brings its massive red head down to bear upon $N!", ch, NULL, victim1, TO_NOTVICT);
      act("$n brings its massive red head around to bear upon you!", ch, NULL, victim1, TO_VICT);
      magic_spell_vict(ch, victim1, CAST_BITS_MOB_SPELL, ch->level, sn1);
   }
   if (victim2 != NULL)
   {
      if ( ( sn2 >= 0 ) && victim2->in_room == ch->in_room)
      {
         act("$n brings its massive green head down to bear upon $N!", ch, NULL, victim2, TO_NOTVICT);
         act("$n brings its massive green head around to bear upon you!", ch, NULL, victim2, TO_VICT);
         magic_spell_vict(ch, victim2, CAST_BITS_MOB_SPELL, ch->level, sn2);
      }
   }
   if (victim3 != NULL && victim3->in_room == ch->in_room)
   {
      if (  sn3 >= 0 )
      {
         act("$n brings its massive blue head down to bear upon $N!", ch, NULL, victim3, TO_NOTVICT);
         act("$n brings its massive blue head around to bear upon you!", ch, NULL, victim3, TO_VICT);
         magic_spell_vict(ch, victim3, CAST_BITS_MOB_SPELL, ch->level, sn3);
      }
   }

   return TRUE;
}

void spell_flame_scorch(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;
   int dam;
   int tmp_dam;
   int avg;
   int dam_type;
   int cold;
   int energy;
   int fire;
   char buf[MAX_STRING_LENGTH];

   if
   (
      /* Allow neohydra to cast without being conclave */
      (
         !IS_NPC(ch) ||
         ch->pIndexData->spec_fun != spec_hydra
      ) &&
      house_down(ch, HOUSE_CONCLAVE)
   )
   {
      return;
   }
   act
   (
      "$n strafes the room in a crossfire of searing blue-green flames!",
      ch,
      NULL,
      NULL,
      TO_ROOM
   );
   send_to_char
   (
      "You strafe the room in a crossfire of searing blue-green flames!\n\r",
      ch
   );
   if (check_spellcraft(ch, sn))
   {
      dam = spellcraft_dam(level, 12);
   }
   else
   {
      dam = dice(level, 12);
   }

   for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
   {
      vch_next = vch->next_in_room;
      if
      (
         vch == ch ||
         is_same_group(ch, vch) ||
         is_safe(ch, vch, IS_SAFE_SILENT | IS_SAFE_AREA_ATTACK) ||
         oblivion_blink(ch, vch)
      )
      {
         continue;
      }
      if
      (
         IS_NPC(vch) &&
         vch->pIndexData->vnum == MOB_VNUM_BONES
      )
      {
         act
         (
            "$n explodes into bits of shattered bone!",
            vch,
            NULL,
            NULL,
            TO_ROOM
         );
         extract_char(vch, TRUE);
         continue;
      }
      dam_type = DAM_ENERGY;
      if (!IS_NPC(ch))
      {
         energy = check_immune(vch, DAM_ENERGY);
         fire = check_immune(vch, DAM_FIRE);
         cold = check_immune(vch, DAM_COLD);
         if (energy == IS_VULNERABLE)
         {
            dam_type = DAM_ENERGY;
         }
         else if (fire == IS_VULNERABLE)
         {
            dam_type = DAM_FIRE;
         }
         else if (cold == IS_VULNERABLE)
         {
            dam_type = DAM_COLD;
         }
         else if (energy == IS_NORMAL)
         {
            dam_type = DAM_ENERGY;
         }
         else if (fire == IS_NORMAL)
         {
            dam_type = DAM_FIRE;
         }
         /* skip normal to cold */
         else if (energy == IS_RESISTANT)
         {
            dam_type = DAM_ENERGY;
         }
         else if (fire == IS_RESISTANT)
         {
            dam_type = DAM_FIRE;
         }
         else if (cold == IS_RESISTANT)
         {
            dam_type = DAM_COLD;
         }
         else
         {
            /* Immune */
            act
            (
               "The blue-green fire seems to avoid $n.",
               vch,
               NULL,
               NULL,
               TO_ROOM
            );
            send_to_char("The blue-green fire seems to avoid you.\n\r", vch);
            continue;
         }
         buf[0] = '\0';
         if (dam_type == DAM_FIRE)
         {
            act
            (
               "$n is surrounded by green flames!",
               vch,
               NULL,
               NULL,
               TO_ROOM
            );
            send_to_char("You are scorched by the flames!\n\r", vch);
            if
            (
               !IS_NPC(vch) &&
               (
                  ch->fighting == NULL ||
                  vch->fighting == NULL
               )
            )
            {
               sprintf
               (
                  buf,
                  "Help! %s is scorching me with green flames!",
                  PERS(ch, vch)
               );
            }
         }
         else if (dam_type == DAM_COLD)
         {
            act
            (
               "$n is surrounded by blue flames!",
               vch,
               NULL,
               NULL,
               TO_ROOM
            );
            send_to_char("You are chilled by the flames!\n\r", vch);
            if
            (
               !IS_NPC(vch) &&
               (
                  ch->fighting == NULL ||
                  vch->fighting == NULL
               )
            )
            {
               sprintf
               (
                  buf,
                  "Help! %s is freezing me with blue flames!",
                  PERS(ch, vch)
               );
            }
         }
         else
         {
            if
            (
               !IS_NPC(vch) &&
               (
                  ch->fighting == NULL ||
                  vch->fighting == NULL
               )
            )
            {
               sprintf
               (
                  buf,
                  "Help! %s is assaulting me with blue-green flames!",
                  PERS(ch, vch)
               );
            }
         }
         if (buf[0] != '\0')
         {
            do_myell(vch, buf);
            sprintf
            (
               log_buf,
               "[%s] cast flame scorch upon [%s] at %d",
               ch->name,
               vch->name,
               ch->in_room->vnum
            );
            log_string(log_buf);
         }
      }
      if (saves_spell(ch, level, vch, dam_type, SAVE_SPELL))
      {
         tmp_dam = dam / 2;
      }
      else
      {
         tmp_dam = dam;
      }
      if (saves_spell(ch, level + 5, vch, DAM_ENERGY, SAVE_SPELL))
      {
         tmp_dam  -= 50;
      }
      if
      (
         dam_type != DAM_ENERGY &&
         (
            saves_spell(ch, level + 5, vch, dam_type, SAVE_SPELL) ||
            saves_spell(ch, level + 5, vch, dam_type, SAVE_SPELL)
         )
      )
      {
         tmp_dam -= 50;
      }
      if (dam_type != DAM_ENERGY)
      {
         avg = tmp_dam / 8;
      }
      else
      {
         avg = tmp_dam / 10;
      }
      tmp_dam += number_range(0, avg * 2) - avg;  /* +- 12.5% */
      if (dam_type == DAM_ENERGY)
      {
         sn = gsn_flame_scorch;
      }
      else if (dam_type == DAM_FIRE)
      {
         sn = gsn_flame_scorch_green;
      }
      else
      {
         sn = gsn_flame_scorch_blue;
      }
      damage(ch, vch, tmp_dam, sn, dam_type, TRUE);
   }
   return;
}

/* horn valere knight spec */
bool spec_valere_knight(CHAR_DATA *ch)
{
   CHAR_DATA *bane;
   int sn;

   if (ch->position != POS_FIGHTING)
   {
      if (number_percent() < 90)
      return FALSE;
      act("$n sighs softly and $s body slowly fades away once again.", ch, NULL, NULL, TO_ROOM);
      extract_char(ch, TRUE);
      return TRUE;
   }
   if (number_percent() > 5)
   return FALSE;

   bane = ch->fighting;
   if (!IS_NPC(bane))
   return FALSE;

   if (bane->pIndexData->vnum == 13718)
   {
      switch(number_range(0, 3))
      {
         case 0: case 1:
         do_say(ch, "Pay for your evil against my people Lord of Darkness!");
         multi_hit(ch, bane, TYPE_UNDEFINED);
         multi_hit(ch, bane, TYPE_UNDEFINED);
         break;
         case 2:
         act("$n is surrounded in a bright halo of burning white light!", ch, NULL, NULL, TO_ROOM);
         sn = gsn_concatenate;
         if (sn <= 0)
         return FALSE;
         magic_spell_vict(ch, bane, CAST_BITS_MOB_SPELL, ch->level, sn);
         break;
         case 3:
         sn = gsn_wrath;
         if (sn <= 0)
         return FALSE;
         magic_spell_vict(ch, bane, CAST_BITS_MOB_SPELL, ch->level, sn);
         break;
      }
   }
   return TRUE;
}


/* Bone demon in drannor */

bool spec_bone_demon(CHAR_DATA *ch)
{
   OBJ_DATA *obj;
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;
   int dam, sn;
   int chance;

   if (ch->position != POS_FIGHTING)
   return FALSE;

   for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
   {
      vch_next = vch->next_in_room;
      if (vch->fighting == ch && number_bits(2) == 0)
      break;
   }
   if (vch == NULL)
   return FALSE;

   chance = (ch->level - vch->level)* 5;
   chance += 50;

   switch(number_range(1, 7))
   {
      case 1: case 2:
      if (number_percent() > chance)
      {
         act("$n's arms reach out and flail at $N but miss.", ch, NULL, vch, TO_NOTVICT);
         act("$n's arms reach out and flail at you but miss.", ch, NULL, vch, TO_VICT);
         return FALSE;
      }
      act("$n's arms reach out and flail across $N's body!", ch, NULL, vch, TO_NOTVICT);
      act("$n's arms reach out and flail across your body!", ch, NULL, vch, TO_VICT);
      dam = dice(ch->level, 6);
      damage(ch, vch, (saves_spell(ch, ch->level, vch, DAM_SLASH, SAVE_OTHER) ? dam/2 : dam), gsn_flail_arms, DAM_SLASH, TRUE);
      if (ch->in_room == vch->in_room)
      {
         dam = dice(ch->level, 4);
         damage(ch, vch, (saves_spell(ch, ch->level, vch, DAM_SLASH, SAVE_OTHER) ? dam/2 : dam), gsn_flail_arms, DAM_SLASH, TRUE);
      }
      if (ch->in_room == vch->in_room)
      {
         dam = dice(ch->level, 2);
         damage(ch, vch, (saves_spell(ch, ch->level, vch, DAM_SLASH, SAVE_OTHER) ? dam/2 : dam), gsn_flail_arms, DAM_SLASH, TRUE);
      }
      return TRUE;
      break;
      case 3: case 4:
      do_kick(ch, "");
      break;
      case 5: case 6:
      act("$n raises its bone arms and calls forth the demons of hell!", ch, NULL, NULL, TO_ROOM);
      sn = gsn_demonfire;
      if (sn <= 0)
      return FALSE;
      dam = dice(ch->level, 7);
      if (saves_spell(ch, ch->level, vch, DAM_NEGATIVE, SAVE_OTHER))
      dam /= 2;
      damage(ch, vch, dam, sn, DAM_NEGATIVE, TRUE);
      break;
      case 7:
      if (ch->hit > 250)
      {
         do_say(ch, "Come share your bones mortal!");
         act("$n raises its bone arms and the bloody waters surge up around you!", ch, NULL, NULL, TO_ROOM);
         sn = gsn_blood_tide;
         dam = dice(ch->level, 7);
         for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
         {
            if (vch == ch)
            continue;
            if (is_safe(ch, vch, IS_SAFE_SILENT | IS_SAFE_AREA_ATTACK))    continue;
            damage(ch, vch, (saves_spell(ch, ch->level, vch, DAM_OTHER, SAVE_OTHER) ? dam/2 : dam), sn, DAM_COLD, TRUE);
         }
         break;
      }
      act("$n's body suddenly shudders and explodes into shards of shattered bone!", ch, NULL, NULL, TO_ROOM);
      dam = dice(ch->level, 12);
      sn = gsn_shattered_bone;
      for (vch = char_list; vch != NULL; vch = vch_next)
      {
         vch_next = vch->next;
         if (vch->in_room->area != ch->in_room->area)
         continue;
         if (vch == ch)  continue;
         if (vch->in_room == ch->in_room)
         damage(ch, vch, (saves_spell(ch, ch->level, vch, DAM_PIERCE, SAVE_OTHER) ?
         dam/2 : dam), sn, DAM_PIERCE, TRUE);
         else
         send_to_char("You hear an explosive detonation and shattered bits of bone fall around you.\n\r", vch);
      }
      for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
      obj_to_room(obj, ch->in_room);
      extract_char(ch, TRUE);
      break;
   }
   return TRUE;
}

bool spec_drannor_wraith(CHAR_DATA *ch)
{
   if (ch->position == POS_FIGHTING)
   return FALSE;

   if (ch->position != POS_FIGHTING
   && ch->last_fought <= 0
   && number_percent() > 20)
   {
      act("$n slowly sinks into the ground with a long sad sigh.", ch, NULL, NULL, TO_ROOM);
      extract_char(ch, TRUE);
      return TRUE;
   }
   return FALSE;
}

/* For arkham legion...Ceran */
bool spec_necromancer( CHAR_DATA *ch )
{
   CHAR_DATA *victim;
   CHAR_DATA *victim_next;
   char *spell;
   int sn;
   CHAR_DATA *zombie;
   OBJ_DATA *corpse;
   OBJ_DATA *obj_next;
   OBJ_DATA *obj;
   CHAR_DATA *search;
   AFFECT_DATA af;
   char *name;
   /*    char *last_name;    */
   char buf1[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];
   int z_level, count;

   if ( ch->position != POS_FIGHTING )
   {
      if (ch->level < 45)
      return FALSE;
      if (ch->pIndexData->vnum != 29723
      && ch->pIndexData->vnum != 29731)
      return FALSE;

      for (victim = ch->in_room->people; victim != NULL; victim = victim_next)
      {
         victim_next = victim->next_in_room;
         if (victim != ch && victim->leader != ch
         && victim->master != ch && can_see(victim, ch)
         && number_bits( 2) == 0)
         break;
      }
      if (victim == NULL)
      return FALSE;
      sn = gsn_power_word_kill;
      if (sn <= 0)
      return FALSE;
      act("$n raises $s cowled face and burning red eyes turn upon you in hatred!", ch, NULL, NULL, TO_ROOM);
      if (!IS_NPC(victim))
      {
         sprintf(buf1, "Help! %s is casting on me!", ch->short_descr);
         do_yell(victim, buf1);
      }

      magic_spell_vict(ch, victim, CAST_BITS_MOB_SPELL, IS_NPC(victim) ? 60 : ch->level - 2, sn);
      return TRUE;
   }

   for ( victim = ch->in_room->people; victim != NULL; victim = victim_next )
   {
      victim_next = victim->next_in_room;
      if ( victim->fighting == ch && number_bits( 2 ) == 0 )
      break;
   }

   if ( victim == NULL )
   return FALSE;

   for ( ;; )
   {
      int min_level;

      switch ( number_range(0, 8) )
      {
         case 1: min_level = 0;  spell = "blindness";    break;
         case 2: min_level = 5;  spell = "curse";        break;
         case 3: min_level = 15; spell = "energy drain"; break;
         case 4: min_level = 24; spell = "cremate";      break;
         case 5: min_level = 28; spell = "acid blast";   break;
         case 6: min_level = 35; spell = "frostbolt";    break;
         default: min_level = 28; spell = "acid blast";     break;
      }

      if ( ch->level >= min_level )
      break;
   }

   if ( ( sn = skill_lookup( spell ) ) < 0 )
   return FALSE;


   if (sn == gsn_animate_dead)
   {
      if (is_affected(ch, sn))
      {
         return FALSE;
      }
      count = 0;
      for (search = char_list; search != NULL; search = search->next)
      {
         if (IS_NPC(search) && (search->master == ch)
         && search->pIndexData->vnum == MOB_VNUM_ZOMBIE)
         count++;
      }

      if ((ch->level < 30 && count > 1) || (ch->level < 35 && count > 2)
      || (ch->level < 40 && count > 3) || (ch->level < 51 && count > 4)
      || count > 4)
      {
         return FALSE;
      }

      corpse = get_obj_here(ch, "corpse");

      if (corpse == NULL)
      {
         return FALSE;
      }

      if ((corpse->item_type != ITEM_CORPSE_NPC) && (corpse->item_type != ITEM_CORPSE_PC) )
      {
         return FALSE;
      }

      if (IS_SET(corpse->extra_flags, CORPSE_NO_ANIMATE))
      {
         return FALSE;
      }

      name = corpse->short_descr;
      for (obj = corpse->contains; obj != NULL; obj = obj_next)
      {
         obj_next = obj->next_content;
         obj_from_obj(obj);
         obj_to_room(obj, ch->in_room);
      }
      af.where = TO_AFFECTS;
      af.type = sn;
      af.level = ch->level;
      af.duration = 4;
      af.modifier = 0;
      af.location = 0;
      af.bitvector = 0;
      affect_to_char(ch, &af);

      if (number_percent() > 95)
      {
         act("$n tries to animate a corpse but destroys it.", ch, NULL, NULL, TO_ROOM);
         extract_obj(corpse, FALSE);
         return TRUE;
      }

      act("$n utters an incantation and a burning red glow flares into the eyes of $p.", ch, corpse, NULL, TO_ROOM);
      act("$p shudders and comes to life!", ch, corpse, NULL, TO_ROOM);
      act("You call upon the powers of the dark to give life to $p.", ch, corpse, NULL, TO_CHAR);

      zombie = create_mobile(get_mob_index(MOB_VNUM_ZOMBIE));
      char_to_room(zombie, ch->in_room);

      z_level = UMAX(1, corpse->level - 5);
      zombie->level = z_level;
      zombie->max_hit = (dice(z_level, 15));
      zombie->max_hit += (z_level * 18);
      zombie->hit = zombie->max_hit;
      zombie->damroll += z_level*2/3;
      zombie->alignment = -1000;
      /*
      last_name = name;

      last_name = one_argument(corpse->short_descr, name);
      last_name = one_argument(last_name, name);
      last_name = one_argument(last_name, name);
      name = last_name;
      */
      if (!str_prefix("The headless corpse of ", name))
      name = name + 23;
      else
      name = name + 14;  /* Skip: "The corpse of " */

      extract_obj(corpse, FALSE);

      sprintf( buf1, "the zombie of %s", name);
      sprintf( buf2, "a zombie of %s is standing here.\n\r", name);
      free_string(zombie->short_descr);
      free_string(zombie->long_descr);
      zombie->short_descr = str_dup(buf1);
      zombie->long_descr = str_dup(buf2);
      af.duration = -1;
      af.bitvector = AFF_CHARM;
      affect_to_char(zombie, &af);

      add_follower(zombie, ch);
      zombie->leader = ch;

      return TRUE;
   }
   else
   {
      magic_spell_vict(ch, victim, CAST_BITS_MOB_SPELL, ch->level, sn);
   }

   return TRUE;
}

/* For arkham legion...Ceran */
bool spec_crusader(CHAR_DATA *ch)
{
   AFFECT_DATA af;

   if (ch->position != POS_FIGHTING)
   return FALSE;

   if (number_bits(2) == 0)
   return FALSE;

   if (!is_affected(ch, gsn_steel_nerves))
   {
      af.where = TO_AFFECTS;
      af.duration = 3 + ch->level/10;
      af.location = 0;
      af.modifier = 0;
      af.level = ch->level;
      af.type = gsn_steel_nerves;
      af.bitvector = 0;
      affect_to_char(ch, &af);

      send_to_char("You build up an intense mental resolve and prepare for coming battles.\n\r", ch);
      act("$n seems to glower and look deadly.", ch, NULL, NULL, TO_ROOM);
      return TRUE;
   }
   else if (!is_affected(ch, gsn_battlecry))
   {
      af.where = TO_AFFECTS;
      af.level = ch->level;
      af.duration = ch->level/2;
      af.type = gsn_battlecry;
      af.modifier = 3;
      af.location = APPLY_HITROLL;
      af.bitvector = 0;
      affect_to_char(ch, &af);

      send_to_char("You are filled with total fury as you let out a battlecry!\n\r", ch);
      act("$n lets out a chilling battlecry and begins frothing at the mouth.", ch, NULL, NULL, TO_ROOM);
      return TRUE;
   }
   else if (!is_affected(ch, gsn_spellbane))
   {
      af.where = TO_AFFECTS;
      af.type = gsn_spellbane;
      af.level = ch->level;
      af.duration = (5 + ch->level/5);
      af.location = APPLY_SAVES;
      af.modifier = (-3 -ch->level/10);
      af.bitvector = 0;
      affect_to_char(ch, &af);

      act("$n fills the air with $s hatred of magic.", ch, NULL, NULL, TO_ROOM);
      send_to_char("You surround yourself with your hatred of magic.\n\r", ch);
      return TRUE;
   }
   else
   return FALSE;
}

/* For arkham legion...Ceran */
bool spec_demonologist(CHAR_DATA *ch)
{
   CHAR_DATA *victim;
   CHAR_DATA *victim_next;
   CHAR_DATA *mob;
   char buf1[MAX_STRING_LENGTH];
   char *spell;
   int sn;
   int lvl;

   spell = "bash";
   if ( ch->position != POS_FIGHTING )
   {
      if (ch->level < 51)
      return FALSE;
      if ( ch->pIndexData->vnum != 29732)
      return FALSE;

      for (victim = ch->in_room->people; victim != NULL; victim = victim_next)
      {
         victim_next = victim->next_in_room;
         if (victim != ch && victim->leader != ch
         && victim->master != ch && can_see(victim, ch)
         && !IS_NPC(victim) )
         break;
      }
      if (victim == NULL)
      return FALSE;
      sn = gsn_fireball;
      if (sn <= 0)
      return FALSE;
      act("$n intones a word of death and the pentegram flares up!", ch, NULL, NULL, TO_ROOM);

      for (victim = ch->in_room->people; victim != NULL; victim = victim_next)
      {
         victim_next = victim->next_in_room;
         if (victim != ch && victim->leader != ch
         && victim->master != ch)
         {
            sprintf(buf1, "Help! %s is casting on me!", ch->short_descr);
            do_yell(victim, buf1);
         }
      }

      magic_spell_vict(ch, victim, CAST_BITS_MOB_SPELL, ch->level, sn);
      /*
      PFILE CORRUPTION BUG:
      (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim + 4 , TAR_CHAR_OFFENSIVE);
      */
      return TRUE;
   }

   for ( victim = ch->in_room->people; victim != NULL; victim = victim_next )
   {
      victim_next = victim->next_in_room;
      if ( victim->fighting == ch && number_bits( 2 ) == 0 )
      break;
   }
   sn = 0;
   if ( victim == NULL )
   return FALSE;

   for ( ;; )
   {
      int min_level;

      switch ( number_range(0, 10) )
      {
         case 1: min_level = 0;  spell = "blindness";    break;
         case 2: min_level = 5; spell = "disease";   break;
         case 3: min_level = 25; spell = "demonfire"; break;
         case 4: min_level = 20; spell = "fireball"; break;
         case 5: min_level = 35; spell = "deathspell";   break;
         case 6: min_level = 10; spell = "power word fear"; break;
         case 7: case 8: min_level = 45; sn = -1; break;
         case 9 : min_level = 20; spell = "cone of cold"; break;
         default: min_level = 20; spell = "acid blast";     break;
      }

      if ( ch->level >= min_level )
      break;
   }

   if (sn == -1)
   {
      for (victim = ch->in_room->people; victim != NULL; victim = victim->next_in_room)
      {
         if (IS_NPC(victim) && victim->pIndexData->vnum == MOB_VNUM_SLAYER)
         break;
      }
      if (victim != NULL)
      return FALSE;

      mob = create_mobile(get_mob_index(MOB_VNUM_SLAYER));
      if (mob == NULL)
      return FALSE;
      act("$n raises $s arms and calls forth a demonic servant!", ch, NULL, NULL, TO_ROOM);

      char_to_room(mob, ch->in_room);
      mob->leader = ch;
      mob->master = ch;
      mob->level = ch->level;
      SET_BIT(mob->affected_by, AFF_CHARM);
      act("A gate opens up and $n steps forth.", mob, NULL, NULL, TO_ROOM);
      return TRUE;
   }

   if ( ( sn = skill_lookup( spell ) ) < 0 )
   return FALSE;
   lvl = ch->level;

   if (sn == gsn_deathspell)
   {
      act("$n gestures and a flaming hexagram burns into the air!", ch, NULL, NULL, TO_ROOM);
      send_to_char("You gesture and a flaming hexagram burns into the air!\n\r", ch);
      lvl += 3;
   }
   else if (sn == gsn_fireball && number_percent() > 65)
   {
      act("$n gestures and red runes etched into the floor explode!", ch, NULL, NULL, TO_ROOM);
      lvl += 4;
   }
   else if (sn == gsn_cone_of_cold && number_percent() > 75)
   {
      act("$n gestures and blue runes etched into the floor explode!", ch, NULL, NULL, TO_ROOM);
      lvl += 3;
   }

   magic_spell_vict(ch, victim, CAST_BITS_MOB_SPELL, lvl, sn);
   return TRUE;
}

/* For titan in Arkham legion..Ceran */
bool spec_titan(CHAR_DATA *ch)
{
   CHAR_DATA *victim;
   CHAR_DATA *victim_next;
   int num, dam;
   AFFECT_DATA af;
   int chance;
   int sn;

   if (ch->fighting == NULL)
   {
      if (ch->last_fought > 0)
      {
         if (number_percent() > 5
         || is_affected(ch, gsn_trophy))
         return FALSE;
         if (number_percent() > 50)
         do_yell(ch, "Come back puny mortals and feed my hungry stomach!");
         else
         do_yell(ch, "You fight like mortals and flee like children!");

         af.where = TO_AFFECTS;
         af.type = gsn_trophy;   /* Just using as trigger */
         af.modifier = 0;
         af.bitvector = 0;
         af.level = 0;
         af.duration = 2;
         af.location = 0;
         affect_to_char(ch, &af);
         return TRUE;
      }

      if (number_percent() > 5 || is_affected(ch, gsn_trophy))
      return FALSE;
      num = number_range(0, 5);
      if (num == 0 || num == 1)
      do_yell(ch, "Release me mortals or suffer my wrath!");
      else if (num == 3)
      do_yell(ch, "I shall crush you like twigs and feast on your corpses mortal fools!");
      else
      do_yell(ch, "Ni Sothoth Den Cael Yog Nar Sethan!");

      af.where = TO_AFFECTS;
      af.type = gsn_trophy;   /* Just using as trigger */
      af.modifier = 0;
      af.bitvector = 0;
      af.level = 0;
      af.duration = 2;
      af.location = 0;
      affect_to_char(ch, &af);
      return TRUE;

      return TRUE;
   }

   for ( victim = ch->in_room->people; victim != NULL; victim = victim_next )
   {
      victim_next = victim->next_in_room;
      if ( victim->fighting == ch && number_bits( 2 ) == 0 )
      break;
   }

   if ( victim == NULL )
   return FALSE;

   num = number_range(0, 4);

   switch( num )
   {
      case 1: sn = gsn_crush; break;
      case 2: sn = gsn_windwall; break;
      case 3: sn = gsn_acid_blast;    break;
      case 4: sn = -1;    break;
      default: sn = -1; break;
   }

   if (sn == -1)
   return FALSE;

   if (sn == gsn_acid_blast)
   {
      act("$n hurls a bolt of seething acid at $N!", ch, NULL, victim, TO_NOTVICT);
      act("$n hurls a bolt of seething acid at you!", ch, NULL, victim, TO_VICT);
      act("You hurl a bolt of seething acid at $N!", ch, NULL, victim, TO_CHAR);
   }
   else if (sn == gsn_windwall)
   {
      act("$n breathes out and releases a blast of gale force wind!", ch, NULL, NULL, TO_ROOM);
      act("You breath out and release a blast of gale force wind!", ch, NULL, NULL, TO_CHAR);
   }
   else
   {
      chance = 50 + (ch->level - victim->level) * 3;

      if (number_percent() > chance)
      return FALSE;

      dam = dice(ch->level, 8);
      if (saves_spell(ch, ch->level, victim, DAM_BASH, SAVE_OTHER))
      dam /= 2;
      act("$n grabs hold of $N and slams $M to the ground!", ch, NULL, victim, TO_NOTVICT);
      act("You grab hold of $N and slam $M to the ground!", ch, NULL, victim, TO_CHAR);
      act("$n grabs hold of you and slams you to the ground!", ch, NULL, victim, TO_VICT);

      damage(ch, victim, dam, gsn_crush, DAM_BASH, TRUE);
      WAIT_STATE(ch, 24);
      DAZE_STATE(victim, 12);
      return TRUE;
   }

   magic_spell_vict(ch, victim, CAST_BITS_MOB_SPELL, ch->level, sn);
   return TRUE;
}


/* Real nasty spec..Ceran */
bool spec_legionlord(CHAR_DATA *ch)
{
   CHAR_DATA *victim;
   char *msg;
   CHAR_DATA *victim_next;
   /*    int min_sector = 0, max_sector = 1; */
   int sn, dam, level;
   int num;
   num = number_range(0, 5);
   level = ch->level;
   msg = "none";

   if (ch->position == POS_SLEEPING)
   {
      act("$n shudders and slowly rises to $s feet.", ch, NULL, NULL, TO_ROOM);
      ch->position = POS_STANDING;
      ch->on = NULL;
   }
   if (ch->position != POS_FIGHTING)
   {
      for (victim = ch->in_room->people; victim != NULL; victim = victim_next)
      {
         victim_next = victim->next_in_room;
         if (victim == ch
         || ( IS_NPC(victim) && victim->pIndexData->vnum == 29782) )
         continue;
         if (number_percent() < 80)
         break;
      }
      if (victim == NULL)
      return FALSE;
      sn = gsn_demonfire;
      if (sn < 0)
      return FALSE;

      if (num == 0)
      do_say(ch, "When Judgement comes there is no fate but what you make.");
      else if (num == 2)
      do_say(ch, "Your time has come mortal!");
      else if (num == 3)
      do_say(ch, "I judge thee as forsaken!");
      else if (num == 4)
      do_say(ch, "Only fools come to greet death while still living!");
      else
      do_say(ch, "Time to die puny mortal!");

      dam = 400 + dice(ch->level, 5);
      if (saves_spell(ch, ch->level, victim, DAM_NEGATIVE, SAVE_OTHER))
      dam /= 2;
      damage(ch, victim, dam, sn, DAM_NEGATIVE, TRUE);
      return TRUE;
   }
   /*
   for (victim = ch->in_room->people; victim != NULL; victim = victim->next_in_room)
   {
   if (victim == ch
   || ( IS_NPC(victim) && victim->pIndexData->vnum == 29782) )
   continue;
   if (number_bits(2) == 0)
   break;
   }

   if (victim == NULL && is_affected(ch, gsn_trophy))
   return FALSE;

   if (victim == NULL && number_percent() > 2)
   return FALSE;

   if (victim == NULL)
   {
   switch(number_range(0, 10))
   {
   case 1:
   msg = "Dark clouds roll overhead and blue lightning crackles across the skies.";
   min_sector = SECT_FIELD;
   max_sector = SECT_WATER_NOSWIM;
   break;
   case 2:
   msg = "You catch a shadow moving in the corner of your eye but it vanishes without trace.";
   min_sector = SECT_INSIDE;
   max_sector = SECT_WATER_NOSWIM;
   break;
   case 3:
   msg = "A chill wind blows through you, freezing your soul briefly...";
   min_sector = SECT_FIELD;
   max_sector = SECT_WATER_NOSWIM;
   break;
   case 4:
   msg = "A trickle of blood drips from the roof and lands at your feet.";
   min_sector = SECT_INSIDE;
   max_sector = SECT_FIELD;
   break;
   case 5:
   msg = "A hollow demonic voice echoes around you then fades into nothingness.";
   min_sector = SECT_INSIDE;
   max_sector = SECT_FIELD;
   break;
   case 6:
   msg = "Something dark and small moves briefly beneath the mud before disappearing.";
   min_sector = SECT_FIELD;
   max_sector = SECT_WATER_NOSWIM;
   break;
   case 7:
   msg = "A corpse on the floor twitches before sinking back into the muck.";
   min_sector = SECT_FIELD;
   max_sector = SECT_WATER_NOSWIM;
   break;
   case 8:
   msg = "zombie call";
   min_sector = SECT_FIELD;
   max_sector = SECT_WATER_NOSWIM;
   break;
   case 9:
   break;
   case 10:
   break;
   default:
   break;
   }
   if (!str_cmp(msg, "none"))
   return FALSE;
   return (area_act(ch, msg, min_sector, max_sector));
   }

   sn = gsn_demonfire;
   if (sn < 0)
   return FALSE;
   do_say(ch, "When Judgement comes there is no fate but what you make.");
   dam = 400 + dice(level, 5);
   if (saves_spell(ch, level, victim, DAM_NEGATIVE, SAVE_OTHER))
   dam /= 2;
   damage(ch, victim, dam, sn, DAM_NEGATIVE, TRUE);
   return TRUE;
   }
   */
   switch(number_range(0, 4))
   {
      case 0:
      return (spec_necromancer(ch));  break;
      case 1:
      return (spec_demonologist(ch)); break;
      case 2:
      return (spec_breath_frost(ch)); break;
      case 3:
      return (spec_breath_acid(ch)); break;
      default:
      return (spec_breath_frost(ch)); break;
   }

   return FALSE;
}

/* For special functions that use area messaging .. Ceran */
bool area_act(CHAR_DATA *ch, char *msg, int min_sect, int max_sect)
{
   CHAR_DATA *ach;
   CHAR_DATA *ach_next;
   CHAR_DATA *mob;
   int count;
   bool found = FALSE;
   return FALSE;
   count = 0;

   if (!str_cmp(msg, "zombie call"))
   {
      for (ach = char_list; ach != NULL; ach = ach_next)
      {
         ach_next = ach->next;
         if (ach->in_room == NULL)
         continue;
         if (ach->in_room->area != ch->in_room->area)
         continue;
         if (ach == ch || IS_NPC(ach))
         continue;
         if (ach->in_room->sector_type >= min_sect
         && ach->in_room->sector_type < max_sect)
         found = TRUE;
      }
      if (!found)
      return FALSE;
      for (; ;)
      {
         count++;
         for (ach = char_list; ach != NULL; ach = ach_next)
         {
            ach_next = ach->next;
            if (ach->in_room == NULL)
            continue;
            if (ach->in_room->area != ch->in_room->area)
            continue;
            if (ach == ch || IS_NPC(ach))
            continue;
            if (ach->in_room->sector_type >= min_sect
            && ach->in_room->sector_type < max_sect
            && number_bits(2) == 0)
            break;
         }
         if ((ach != NULL) || count >= 20)
         break;
      }

      if (ach == NULL)
      return FALSE;
      mob = create_mobile(get_mob_index(29708));
      if (mob == NULL)
      return FALSE;
      char_to_room(mob, ach->in_room);
      act("The ground heaves and $n claws its way out!", mob, NULL, NULL, TO_ROOM);
      multi_hit(mob, ach, TYPE_UNDEFINED);
      return TRUE;
   }


   for (ach = char_list; ach != NULL; ach = ach_next)
   {
      ach_next = ach->next;
      if (ach->in_room == NULL)
      continue;
      if (ach->in_room->area != ch->in_room->area)
      continue;
      if (ach == ch || IS_NPC(ach))
      continue;
      if (ach->in_room->sector_type >= min_sect
      && ach->in_room->sector_type < max_sect
      && IS_AWAKE(ach))
      act(msg, ch, NULL, ach, TO_VICT);
   }

   return TRUE;
}

/* new specials for dragons .. Ceran */
bool spec_wings(CHAR_DATA *ch)
{
   CHAR_DATA *victim;
   CHAR_DATA *victim_next;
   int dam;
   int sn;

   sn = gsn_windwall;
   if (sn <= 0)
   return FALSE;

   act("$n spreads $s wings and beats violently at the air!", ch, NULL, NULL, TO_ROOM);
   dam = dice(ch->level, 6);
   for (victim = ch->in_room->people; victim != NULL; victim = victim_next)
   {
      victim_next = victim->next_in_room;
      if
      (
         is_safe(ch, victim, IS_SAFE_SILENT | IS_SAFE_AREA_ATTACK) ||
         victim == ch ||
         is_same_group(victim, ch)
      )
      {
         continue;
      }
      damage
      (
         ch,
         victim,
         (
            saves_spell(ch, ch->level, victim, DAM_OTHER, SAVE_OTHER) ?
            dam / 2 :
            dam
         ),
         sn,
         DAM_BASH,
         TRUE
      );
   }
   return TRUE;
}


bool spec_red_dragon(CHAR_DATA *ch)
{
   int num;
   CHAR_DATA *victim;
   CHAR_DATA *victim_next;
   int sn;
   int level;

   if (ch->position != POS_FIGHTING)
   return FALSE;

   level = ch->level;
   num = number_range(0, 5);
   sn = gsn_chain_lightning;
   if (sn <= 0)
   sn = gsn_lightning_bolt;

   if (num == 0)
   {
      for ( victim = ch->in_room->people; victim != NULL; victim = victim_next )
      {
         victim_next = victim->next_in_room;
         if ( victim != ch && can_see( ch, victim ) && number_bits( 1 ) == 0 )
         break;
      }

      if ( victim == NULL )
      return FALSE;

      if (sn <= 0)
      return FALSE;

      spell_chain_lightning(sn, level, ch, victim, 0);
   }
   else if (num == 1 || num == 2 || num == 3)
   return spec_breath_fire(ch);
   else if (num == 4)
   return spec_wings(ch);
   else
   return spec_cast_mage(ch);

   return TRUE;
}

bool spec_black_dragon(CHAR_DATA *ch)
{
   CHAR_DATA *victim;
   CHAR_DATA *victim_next;
   int sn, dam, num;
   int sn_p, sn_b, level;
   char buf[MAX_INPUT_LENGTH];

   if (ch->position != POS_FIGHTING)
   return FALSE;

   num = number_range(0, 5);
   level = ch->level;

   if (num == 0)
   {
      sn = gsn_acid_breath;
      sn_p = gsn_poison;
      sn_b = gsn_blindness;

      if ((sn <= 0)
      || (sn_p <= 0) || (sn_b <= 0))
      return FALSE;

      act("$n hisses out a word of draconic power!", ch, NULL, NULL, TO_ROOM);
      dam = dice(level, 5);
      for (victim = ch->in_room->people; victim != NULL; victim = victim_next)
      {
         victim_next = victim->next_in_room;
         if (is_safe(ch, victim, IS_SAFE_SILENT | IS_SAFE_AREA_ATTACK) || victim == ch)
         continue;
         damage(ch, victim, saves_spell(ch, level, victim, DAM_ACID, SAVE_OTHER) ? dam/2 : dam, sn, DAM_ACID, TRUE);
         sprintf(buf, "blah %ld", victim->id);
         magic_spell_vict(ch, victim, CAST_BITS_MOB_SPELL, level, sn_p);
         magic_spell_vict(ch, victim, CAST_BITS_MOB_SPELL, level, sn_b);
      }
      return TRUE;
   }
   else if (num == 1 || num == 2 || num== 3)
   return spec_breath_acid(ch);
   else if (num == 4)
   return spec_wings(ch);
   else
   return spec_cast_mage(ch);

   return TRUE;
}

bool spec_gold_dragon(CHAR_DATA *ch)
{
   CHAR_DATA *victim;
   CHAR_DATA *victim_next;
   int sn, dam;
   int num;

   if (ch->position != POS_FIGHTING)
   return FALSE;

   num = number_range(0, 5);

   if (num == 0)
   {
      sn = gsn_concatenate;

      if (sn <= 0)
      return FALSE;

      for ( victim = ch->in_room->people; victim != NULL; victim = victim_next )
      {
         victim_next = victim->next_in_room;
         if ( victim != ch && can_see( ch, victim ) && number_bits( 1 ) == 0 )
         break;
      }

      if ( victim == NULL )
      return FALSE;

      act("$n points a talon at $N and unleashes a blast of light!", ch, NULL, victim, TO_NOTVICT);
      act("You point a talon at $N and unleash a blast of light!", ch, NULL, victim, TO_CHAR);
      act("$n points a talon at you and unleashes a blast of light!", ch, NULL, victim, TO_VICT);
      dam = dice(ch->level, 13);
      damage(ch, victim, saves_spell(ch, ch->level, victim, DAM_LIGHT, SAVE_OTHER) ? dam/2 : dam, sn, DAM_LIGHT, TRUE);
      return TRUE;
   }
   else if (num == 1 || num == 2 || num == 3)
   return spec_breath_fire(ch);
   else if (num == 4)
   return spec_wings(ch);
   else
   return spec_cast_cleric(ch);

   return TRUE;
}


bool spec_revolutionary( CHAR_DATA *ch )
{
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA *victim;
   CHAR_DATA *victim_next;
   CHAR_DATA *ech;
   char *crime;
   int max_evil;

   if ( !IS_AWAKE(ch) || ch->fighting != NULL )
   return FALSE;

   max_evil = 300;
   ech      = NULL;
   crime    = "";

   for ( victim = ch->in_room->people; victim != NULL; victim = victim_next )
   {
      victim_next = victim->next_in_room;


      if ( !IS_NPC(victim) && IS_SET(victim->act, PLR_MARKED)
      &&   can_see(ch, victim))
      { break; }
      /*
      if ( victim->fighting != NULL
      &&   victim->fighting != ch
      &&   victim->alignment < max_evil )
      {
      max_evil = victim->alignment;
      ech      = victim;
      }
      */
   }

   if ( victim != NULL )
   {
      sprintf( buf, "%s is an enemy of freedom!  LONG LIVE THE REVOLUTION!!!",
      PERS(victim, ch));
      REMOVE_BIT(ch->comm, COMM_NOSHOUT);
      do_yell( ch, buf );
      multi_hit( ch, victim, TYPE_UNDEFINED );
      return TRUE;
   }

   /*
   if ( ech != NULL )
   {
   act( "$n screams 'LONG LIVE THE REVOLUTION!!!  BANZAI!!",
   ch, NULL, NULL, TO_ROOM );
   multi_hit( ch, ech, TYPE_UNDEFINED );
   return TRUE;
   }
   */
   return FALSE;
}

bool demonai_match(CHAR_DATA *ch, int id_check)
{
   int dtype;
   if (ch == NULL || id_check == 0) return FALSE;
   if (!IS_NPC(ch)) return FALSE;
   if (ch->pIndexData->vnum != MOB_VNUM_DEMON1) return FALSE;
   dtype = demon_lookup(ch->name, NULL);
   if (dtype == -1) return FALSE;
   if (demon_table[dtype].id_code == id_check) return TRUE;
   return FALSE;
}

bool spec_brethren_demon(CHAR_DATA *ch)
{
   int dtype = -1;
   int demonid = -1;
   int spell_sn = -1;
   int sn;
   int return_type = FALSE;
   CHAR_DATA *victim = NULL;
   CHAR_DATA *victim_next;
   char buf[MAX_STRING_LENGTH];
   int cnt;

   dtype = demon_lookup(ch->name, NULL);

   if (dtype == -1)
   {
      return FALSE;
   }

   if (ch->master == NULL)
   return FALSE;
   if (ch->master->in_room != ch->in_room)
   return FALSE;
   if (number_percent() > ch->level)
   return FALSE;

   demonid = demon_table[dtype].id_code;
   if (demon_table[dtype].spell != NULL)
   spell_sn = skill_lookup(demon_table[dtype].spell);

   sn = spell_sn;  /* just a convenience */

   /* special demon stuff here, and set return_type = TRUE, but don't
   return */

   /*example of special stuff */
   if (demonid == DEMON_DRAGKAGH)
   {
      if (!IS_IMMORTAL(ch->master) && number_percent() < 30)
      {
         if (ch->master->fighting)
         {
            do_say(ch, "I renounce you as my master, you puny mortal!");
            sprintf(buf, "picks up %s in its massive claws and prepares to have lunch.\n\r", ch->master->name);
            do_emote(ch, buf);
            victim = ch->master;
            REMOVE_BIT(ch->affected_by, AFF_CHARM);
            ch->leader = NULL;
            ch->master = NULL;
            cnt = 0;
            while(victim->ghost == 0)
            {
               cnt++;
               act("A loud crunch resonates from $N's body as $n eats $M.", ch, NULL, victim, TO_ROOM);
               damage(ch, victim, 500, gsn_crush, DAM_SLASH, TRUE);
               if (cnt > 20) break;
            }
            if (ch->in_room->vnum < 1035 || ch->in_room->vnum > 1043)
            {
               stop_fighting(ch, TRUE);
               act("$n opens a flaming portal to the Abyss and steps through, returning to $s world.", ch, NULL, NULL, TO_ROOM);
               extract_char(ch, TRUE);
            }
         }
      }
      return_type = TRUE;
   }

   if (demonid == DEMON_DRETCH)
   {
      if (number_range(1, 3) == 1)
      {
         do_say(ch, "Let us destroy something, master!");
      }
      if (number_percent() < 35)
      {
         if (ch->hit != ch->max_hit)
         {
            do_emote(ch, "regenerates itself.");
            ch->hit = UMAX(ch->max_hit, ch->hit + ch->hit/10);
         }
      }
      return_type = TRUE;
   }

   if (demonid == DEMON_WIMP)
   {
      if (number_range(1, 3) == 1)
      {
         if (ch->fighting == NULL && !IS_AFFECTED(ch, AFF_HIDE))
         {
            do_emote(ch, "peers around intently then hides itself from danger.");
            SET_BIT(ch->affected_by, AFF_HIDE);
            return_type = TRUE;
         }
         if (ch->fighting && ch->hit < ch->max_hit)
         {
            do_say(ch, "Master!  Im hurt!");
            return_type = TRUE;
         }
      }
   }

   if (demonid == DEMON_TENGWAR)
   {
      if (ch->fighting != NULL &&
      ch->hit < 3*ch->max_hit/4 &&
      number_percent() < 7)
      {
         return_type = TRUE;
         /* tengwar goes into a frenzy */
         do_emote(ch, "goes into a frenzy.");
         damage(ch, ch->fighting, number_range(30, 50), gsn_open_claw, DAM_SLASH, TRUE);
         damage(ch, ch->fighting, number_range(30, 50), gsn_open_claw, DAM_SLASH, TRUE);
         if (number_range(1, 2) == 1)
         damage(ch, ch->fighting, number_range(30, 50), gsn_open_claw, DAM_SLASH, TRUE);
         if (number_range(1, 2) == 1)
         damage(ch, ch->fighting, number_range(30, 50), gsn_open_claw, DAM_SLASH, TRUE);
         if (number_range(1, 2) == 1)
         {
            damage(ch, ch->fighting, number_range(50, 70), gsn_ramming, DAM_PIERCE, TRUE);
            WAIT_STATE(ch->fighting, 20);
         }
      }
   }

   /* aggressive ai */
   if (demonid == DEMON_AGGRESSIVE)
   {
      return_type = TRUE;
      if (ch->master->hit < ch->master->max_hit/5)
      {
         sprintf(buf, "smells the scent of death upon %s.", ch->master->name);
         do_emote(ch, buf);
         do_say(ch, "You are unfit to be my master!");
         REMOVE_BIT(ch->affected_by, AFF_CHARM);
         multi_hit(ch, ch->master, TYPE_UNDEFINED);
      }
      if (number_percent() < 30) do_emote(ch, "sniffs around for fresh meat.");
      if (ch->fighting == NULL)
      {
         if (ch->mprog_target == NULL)
         {
            if (number_percent() > 70)
            {
               do_say(ch, "Master, I want to kill something!");
               for ( victim = ch->in_room->people; victim != NULL; victim = victim_next )
               {
                  victim_next = victim->next_in_room;
                  if (IS_IMMORTAL(victim)) continue;
                  if ( victim != ch && victim != ch->master &&
                  victim->hit <= victim->max_hit)
                  break;
               }
               ch->mprog_target = victim;
               ch->mprog_delay = 4;
               if (victim != NULL)
               {
                  sprintf(buf, "How about I killed %s.", PERS(victim, ch));
                  do_say(ch, buf);
               }
            }
         }
         else
         {
            victim = ch->mprog_target;
            ch->mprog_delay--;
            if (ch->mprog_delay > 1)
            {
               return FALSE;
            }
            if (victim->in_room == ch->in_room)
            {
               if (!is_safe(ch, victim, IS_SAFE_SILENT)){
                  do_say(ch, "You are weak, I kill now!");
                  multi_hit(ch, victim, TYPE_UNDEFINED);
               }
               ch->mprog_target = NULL;
               ch->mprog_delay = 0;

            }
         }
      }
   }

   if (spell_sn > 0)
   {
      if ( ch->position < skill_table[sn].minimum_position )
      return FALSE;
      switch(skill_table[sn].target)
      {
         default:
         case TAR_CHAR_OFFENSIVE:
         if (ch->fighting == NULL)
         return FALSE;
         for ( victim = ch->in_room->people; victim != NULL; victim = victim_next )
         {
            victim_next = victim->next_in_room;
            if ( (victim->fighting == ch->master || victim->fighting == ch) && number_bits( 2 ) == 0 )
            break;
         }

         if ( victim == NULL )
         return FALSE;

         magic_spell_vict(ch, victim, CAST_BITS_MOB_SPELL, ch->level, sn);
         break;
         case TAR_CHAR_HEALING:
         case TAR_CHAR_DEFENSIVE:
         if (number_range(1, 5) < 3)
         {
            victim = ch;
            if (demonid == DEMON_CTHUL && !IS_AFFECTED(ch, AFF_SANCTUARY))
            do_emote(ch, "grins mischievously.");
         }
         else
         {
            if (number_range(1, 10) < 3) return TRUE;
            victim = ch->master;
            if (demonid == DEMON_CTHUL && !IS_AFFECTED(victim, AFF_SANCTUARY))
            do_say(ch, "I will protect you, Master.");
         }
         magic_spell_vict(ch, victim, CAST_BITS_MOB_SPELL, ch->level, sn);
         break;
         case TAR_CHAR_SELF:
         magic_spell(ch, "blah self", CAST_BITS_MOB_SPELL, ch->level, sn);
         break;
      }
      return TRUE;
   }
   return return_type;
}

/*Jord*/
bool spec_brethren_guardian(CHAR_DATA *ch)
{

   int breath = number_range(0, 4);
   int sn=0, dam, dt=0, times;
   CHAR_DATA *victim, *victim_next;

   if (ch == NULL) return FALSE;
   if ( ch->position != POS_FIGHTING )
   return FALSE;

   for ( victim = ch->in_room->people; victim != NULL; victim =victim_next )
   {
      victim_next = victim->next_in_room;
      if ( victim->fighting == ch && number_bits( 2 ) == 0 )
      break;
   }
   if (victim == NULL) victim = ch->fighting;
   if (victim == NULL) return FALSE;
   if (number_range(0, 4) == 0)
   {
      if (breath == 0)
      {
         act("A Gaseous Skull flickers into existence, its jaws open, spewing forth green hell.", ch, NULL, NULL, TO_ROOM);
         sn = gsn_gas_breath;
         dt = DAM_POISON;
      }

      if (breath == 1)
      {
         act("The Hellspawn spits forth a stream of burning acid, spewing forth green hell.", ch, NULL, NULL, TO_ROOM);
         sn = gsn_acid_breath;
         dt = DAM_ACID;
      }

      if (breath == 2)
      {
         act("The Hellspawn opens its mouth, an inferno of incendiary flames lashing outward!", ch, NULL, NULL, TO_ROOM);
         sn = gsn_fire_breath;
         dt = DAM_FIRE;
      }

      if (breath == 3)
      {
         act("A horrendous creature appears from a portal, breathing a blizzard upon the room.", ch, NULL, NULL, TO_ROOM);
         sn = gsn_frost_breath;
         dt = DAM_COLD;
      }

      if (breath == 4)
      {
         act("Suddenly, dark lightning fills the room originating from the Hellspawn.", ch, NULL, NULL, TO_ROOM);
         sn = gsn_lightning_breath;
         dt = DAM_LIGHTNING;
      }

      dam = dice(51, 10);
      if (saves_spell(ch, 51, victim, dt, SAVE_SPELL))
      dam /= 2;

      damage(ch, victim, dam, sn, dt, TRUE);
      return TRUE;
   }

   if (number_range(0, 4) == 0)
   {
      sn = gsn_demon_swarm;
      act("The Hellspawn opens a gate and several small demons rush out!", ch, NULL, victim, TO_ROOM);
      for (times = 0;times < 5; times++)
      {
         dam = (dice(ch->level + 6, 2));
         act("The demons tear into $N!", ch, NULL, victim, TO_ROOM);
         damage(ch, victim, dam, sn, DAM_OTHER, TRUE);
      }
      act("The Hellspawn snaps the gate shut and the demons disappear!", ch, NULL, NULL, TO_ROOM);
      return TRUE;
   }
   return FALSE;
}

/* Malignus */
bool spec_outlaw_guard( CHAR_DATA *ch )
{
   CHAR_DATA *victim;
   CHAR_DATA *victim_next;
   int sn;

   for ( victim = ch->in_room->people; victim != NULL; victim = victim_next )
   {
      victim_next = victim->next_in_room;

      if (IS_IMMORTAL(victim))
      {
         do_say(ch, "Imm here.");
         continue;
      }

      if (victim->house == HOUSE_OUTLAW)
      {
         do_say(ch, "Outlaw here.");
         continue;
      }


      if (!can_see(ch, victim) || IS_NPC(victim))
      {
         do_say(ch, "cant see");
         continue;
      }


      if (IS_SET(victim->act, PLR_CRIMINAL))
      {
         do_say(ch, "Crim here.");
         continue;
      }

      do_say(ch, "we have a victim?");
      if ( victim != NULL )
      {
         do_yell(ch, "These tunnels are a haven for the lawless! Begone!");
         multi_hit( ch, victim, TYPE_UNDEFINED );
         sn = gsn_revolt;
         magic_spell_vict(ch, victim, CAST_BITS_MOB_SPELL, 52, sn);
         return TRUE;
      }
   }




   return FALSE;
}

/* One round of spectre spells */
bool spec_spectre_1(CHAR_DATA *ch)
{
   CHAR_DATA* victim;
   CHAR_DATA* victim_next;
   char* spell;
   int sn;
   int level;
   int choice;
   int count;

   if (ch->position != POS_FIGHTING)
   {
      /* Only continue with 25% */
      return FALSE;
   }
   count = 0;
   for (victim = ch->in_room->people; victim != NULL; victim = victim_next)
   {
      victim_next = victim->next_in_room;
      if
      (
         victim->fighting == ch &&
         !is_safe(ch, victim, IS_SAFE_SILENT)
      )
      {
         count++;
      }
   }
   choice = number_range(1, count);
   count = 0;
   for (victim = ch->in_room->people; victim != NULL; victim = victim_next)
   {
      victim_next = victim->next_in_room;
      if
      (
         victim->fighting == ch &&
         !is_safe(ch, victim, IS_SAFE_SILENT) &&
         ++count == choice
      )
      {
         break;
      }
   }
   if (victim == NULL)
   {
      return FALSE;
   }
   switch(number_range(0, 6))
   {
      case (0):
      case (1):
      {
         level = 55;
         spell = "spiritblade";
         break;
      }
      case (2):
      {
         level = 63;
         spell = "fist of god";
         break;
      }
      case (3):
      {
         level = 57;
         spell = "earthmaw";
         break;
      }
      case (4):
      {
         level = 55;
         spell = "concatenate";
         break;
      }
      default:
      {
         level = 60;
         spell = "acid blast";
         break;
      }
   }
   if
   (
      (
         sn = skill_lookup(spell)
      ) < 0
   )
   {
      return FALSE;
   }
   magic_spell_vict(ch, victim, CAST_BITS_MOB_SPELL, level, sn);
   return TRUE;
}


/* Spec fun for spectre */
bool spec_spectre(CHAR_DATA *ch)
{
   CHAR_DATA* victim;
   CHAR_DATA* victim_next;
   int count;
   int calls = 0;
   bool ret_val = FALSE;

   if
   (
      ch->position != POS_FIGHTING ||
      number_bits(2)
   )
   {
      /* Only continue with 25% */
      return FALSE;
   }
   count = 0;
   for (victim = ch->in_room->people; victim != NULL; victim = victim_next)
   {
      victim_next = victim->next_in_room;
      if
      (
         victim->fighting == ch &&
         !is_safe(ch, victim, IS_SAFE_SILENT)
      )
      {
         count++;
      }
   }
   calls = UMAX(2, count / 5);
   for (count = 0; count < calls; count++)
   {
      ret_val = spec_spectre_1(ch) || ret_val;
   }
   return ret_val;
}

bool spec_guard_defender(CHAR_DATA* ch)
{
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA *victim;
   CHAR_DATA *victim_next;
/*
   CHAR_DATA *ech;
   int max_evil;
*/
   if ( !IS_AWAKE(ch) || ch->fighting != NULL )
   return FALSE;

   if
   (
      ch->pIndexData->vnum != MOB_VNUM_ENFORCER_GUARD &&
      !check_room_protected(ch->in_room)
   )
   {
      return FALSE;
   }

/*
   max_evil = 300;
   ech      = NULL;
*/
   for ( victim = ch->in_room->people; victim != NULL; victim = victim_next )
   {
      victim_next = victim->next_in_room;

      if (is_affected(victim, gsn_stealth))
      {
         continue;
      }

      if
      (
         !IS_NPC(victim) &&
         (
            IS_SET(victim->act, PLR_CRIMINAL) ||
            IS_SET(victim->act2, PLR_LAWLESS)
         ) &&
         can_see(ch, victim)
      )
      {
         break;
      }
      /* assisting now based on wanted -- used to be ethos -werv*/
/*
      if
      (
         victim->fighting != NULL &&
         victim->fighting != ch &&
         !IS_NPC(victim) &&
         !IS_NPC(victim->fighting) &&
         victim->pcdata->wanteds > max_evil
      )
      {
         max_evil = victim->pcdata->wanteds;
         ech = victim;
      }
*/
   }

   if ( victim != NULL )
   {
      sprintf( buf, "%s is a criminal!  PROTECT THE INNOCENT!!  BANZAI!!",
      PERS(victim, ch));
      REMOVE_BIT(ch->comm, COMM_NOSHOUT);
      do_yell( ch, buf );
      multi_hit( ch, victim, TYPE_UNDEFINED );
      return TRUE;
   }
/*
   if ( ech != NULL )
   {
      act( "$n screams 'PROTECT THE INNOCENT!!  BANZAI!!",
      ch, NULL, NULL, TO_ROOM );
      multi_hit( ch, ech, TYPE_UNDEFINED );
      return TRUE;
   }
*/
   return FALSE;
}

bool spec_smart_archmage(CHAR_DATA* ch)
{
   CHAR_DATA* victim;
   CHAR_DATA* victim_next;
   char* spell;
   int sn;
   int area_ok = TRUE;
   char buf[MAX_STRING_LENGTH];

   /* pick a 2nd victim for summon */
   for (victim = ch->in_room->people; victim != NULL; victim = victim->next_in_room)
   {
      if
      (
         !IS_NPC(victim) &&
         victim->id != ch->last_fought &&
         victim->fighting == ch
      )
      {
         ch->last_fought2 = victim->id;
      }
   }

   /* clear out our 2nd victim if he died */
   victim = id2name(ch->last_fought2, FALSE);

   if
   (
      victim != NULL &&
      victim->ghost
   )
   {
      ch->last_fought2 = -1;
   }

   if
   (
      ch->fighting == NULL &&
      ch->last_fought != -1
   )
   {
      if (IS_AFFECTED(ch, AFF_BLIND))
      {
         do_cast(ch, " 'cure blind'");
      }
      victim = id2name(ch->last_fought, FALSE);
      if
      (
         ch->last_fought2 != -1 &&
         number_range(1, 3) == 1
      )
      victim = id2name(ch->last_fought2, FALSE);
      if
      (
         victim != NULL &&
         victim->in_room != NULL &&
         ch->in_room != NULL &&
         victim->in_room->area == ch->in_room->area
      )
      {
         int last_fought = ch->last_fought;
         sprintf
         (
            buf,
            " summon %s",
            get_name(victim, ch)
         );
         do_cast(ch, buf);
         if
         (
            victim->in_room == ch->in_room &&
            can_see(ch, victim)
         )
         {
            sprintf
            (
               buf,
               "This isn't over yet %s!",
               PERS(victim, ch)
            );
            do_yell(ch, buf);
            multi_hit(ch, victim, TYPE_UNDEFINED);
            if
            (
               ch->last_fought != last_fought
            )
            {
               ch->last_fought2 = last_fought;
            }
            sprintf
            (
               buf,
               " curse \"%s\"",
               get_name(victim, ch)
            );
            do_cast(ch, buf);
         }
         return TRUE;
      }
   }
   if
   (
      victim != NULL &&
      victim->in_room != NULL &&
      ch->in_room != NULL &&
      victim->in_room->area == ch->in_room->area &&
      (
         ch->in_room->sector_type != SECT_WATER_SWIM ||
         ch->in_room->sector_type != SECT_UNDERWATER ||
         ch->in_room->sector_type != SECT_WATER_NOSWIM
      )
   )
   {
      int last_fought = ch->last_fought;
      sprintf
      (
         buf,
         " riptide %s",
         get_name(victim, ch)
      );
      do_cast(ch, buf);
      if
      (
         victim->in_room == ch->in_room &&
         can_see(ch, victim)
      )
      {
         sprintf
         (
            buf,
            "This isn't over yet %s!",
            PERS(victim, ch)
         );
         do_yell(ch, buf);
         multi_hit(ch, victim, TYPE_UNDEFINED);
         if
         (
            ch->last_fought != last_fought
         )
         {
            ch->last_fought2 = last_fought;
         }
         sprintf
         (
            buf,
            " curse \"%s\"",
            get_name(victim, ch)
         );
         do_cast(ch, buf);
      }
      return TRUE;
   }


   if (ch->position != POS_FIGHTING)
   {
      return FALSE;
   }

   if (number_range(0, 3) == 0)
   {
      return FALSE;
   }
   for (victim = ch->in_room->people; victim != NULL; victim = victim_next)
   {
      victim_next = victim->next_in_room;
      if
      (
         victim->fighting != ch &&
         IS_NPC(victim) &&
         !IS_AFFECTED(victim, AFF_CHARM)
      )
      {
         area_ok = FALSE;
      }
   }

   for (victim = ch->in_room->people; victim != NULL; victim = victim_next)
   {
      victim_next = victim->next_in_room;
      if
      (
         victim->fighting == ch &&
         number_range(0, 2) == 0
      )
      {
         break;
      }
   }

   if (victim == NULL)
   {
      victim = ch->fighting;
   }
   if
   (
      victim == NULL ||
      victim->in_room != ch->in_room
   )
   {
      return FALSE;
   }
   if
   (
      IS_NPC(victim) &&
      victim->master &&
      victim->master->in_room == ch->in_room
   )
   {
      /* a rescue we can't have that */
      if (!can_see(ch, victim->master))
      {
         if (victim->master->fighting != ch)
         {
            do_cast(ch, "windwall");
         }
      }
      if (can_see(ch, victim->master))
      {
         ch->fighting = victim->master;
         victim = victim->master;
      }
   }
   if (IS_NPC(victim))
   {
      if (area_ok)
      sprintf(buf, " 'cone of cold'");
      else
      sprintf(buf, " 'icelance'");
      do_cast(ch, buf);
      return TRUE;
   }
   spell = "acid blast";
   if
   (
      !area_ok ||
      number_range(0, 5) == 0
   )
   {
      switch (number_range(0, 12))
      {
         case  0: spell = "concat";
            break;
         case  1: spell = "earthmaw";
            break;
         case  2: spell = "fire and ice";
            break;
         case  3: spell = "sunburst";
            break;
         case  4: spell = "icelance";
            break;
         case  5: spell = "temporal shear";
            break;
         case  6: spell = "lower resistance";
            break;
         case  7: spell = "sunburst";
            break;
         case  8: spell = "implosion";
            break;
         case  9: spell = "crushing hands";
            break;
         case  10: spell = "stoney grasp";
            break;
         case  11: spell = "web";
            break;
         case  12: spell = "incinerate";
            break;
         default: spell = "acid blast";
            break;
      }
   }
   else
   {
      switch (number_range( 0, 8 ))
      {
         default:
         case 0: spell = "fireball";
            break;
         case 1: spell = "chain lightning";
            break;
         case 2: spell = "earthquake";
            break;
         case 3: spell = "iceball";
            break;
         case 4: spell = "tsunami";
            break;
         case 5: spell = "windwall";
            break;
         case 6: spell = "nova";
            break;
         case 7: spell = "blizzard";
            break;
         case 8: spell = "cone of cold";
            break;
         if
         (
            ch->in_room->sector_type != SECT_WATER_SWIM &&
            ch->in_room->sector_type != SECT_UNDERWATER &&
            ch->in_room->sector_type != SECT_WATER_NOSWIM
         )
         {
            spell = "cone of cold";
         }
      }
   }

   if
   (
      (sn = skill_lookup(spell)) < 0
   )
   {
      return FALSE;
   }

   magic_spell_vict(ch, victim, CAST_BITS_MOB_SPELL, ch->level, sn);
   return TRUE;
}

bool spec_marauder_guardian(CHAR_DATA* ch)
{
   CHAR_DATA* victim;
   CHAR_DATA* victim_next;
   OBJ_DATA* weapon;
   char buf[MAX_STRING_LENGTH];
   int index_of_victim_to_strike;
   int number_of_people_attacking_guardian;
   int victim_index;

   if (ch->position != POS_FIGHTING)
   {
      return FALSE;
   }

   number_of_people_attacking_guardian = 0;
   for (victim = ch->in_room->people; victim != NULL; victim = victim_next)
   {
      victim_next = victim->next_in_room;
      if
      (
         victim->fighting == ch &&
         !is_safe(ch, victim, IS_SAFE_SILENT)
      )
      {
         number_of_people_attacking_guardian++;
      }
   }
   index_of_victim_to_strike =
      number_range(1, number_of_people_attacking_guardian);

   victim_index = 0;
   for (victim = ch->in_room->people; victim != NULL; victim = victim_next)
   {
      victim_next = victim->next_in_room;
      if
      (
         victim->fighting == ch &&
         !is_safe(ch, victim, IS_SAFE_SILENT) &&
         ++victim_index == index_of_victim_to_strike
      )
      {
         break;
      }
   }

   /* Freaky sanity check: This should NEVER be true. */
   if (victim == NULL)
   {
      return FALSE;
   }

   weapon = get_eq_char(ch, WEAR_WIELD);
   if
   (
      weapon != NULL &&
      (number_range(0, 1) == 1) &&
      !IS_WEAPON_STAT(weapon, WEAPON_POISON)
   )
   {
      do_envenom(ch, "");
   }

   sprintf(buf, "%s", victim->name);

   switch (number_range(0, 8))
   {
      case (0):
      case (1):
      {
         do_cunning_strike(ch, buf);
         break;
      }
      case (2):
      case (3):
      {
         do_critical_strike(ch, buf);
         break;
      }
      case (4):
      case (5):
      case (6):
      case (7):
      {
         do_kick(ch, buf);
         break;
      }
      case (8):
      {
         do_throw(ch, buf);
         break;
      }
      default:
      {
         do_kick(ch, buf);
         break;
      }
   }
   return TRUE;
}


