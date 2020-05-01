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

static const char rcsid[] = "$Id: magic.c,v 1.291 2004/11/25 08:52:20 fizzfaldt Exp $";

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
#include "magic.h"
#include "worship.h"
#include "recycle.h"
#include "subskill.h"

/* command procedures needed */
DECLARE_DO_FUN(do_bash        );
DECLARE_DO_FUN(do_sleep       );
DECLARE_DO_FUN(do_gtell       );
DECLARE_DO_FUN(do_follow      );
DECLARE_DO_FUN(do_look        );
DECLARE_DO_FUN(do_laying_hands);
DECLARE_DO_FUN(do_barkskin    );
DECLARE_DO_FUN(do_emote       );
DECLARE_DO_FUN(do_yell        );
DECLARE_DO_FUN(do_myell       );
DECLARE_DO_FUN(do_say         );
DECLARE_DO_FUN(do_flee        );
DECLARE_DO_FUN(do_mount       );
DECLARE_DO_FUN(do_chant       );
DECLARE_DO_FUN(do_dismount    );
DECLARE_DO_FUN(do_cast_non_clergy);
DECLARE_DO_FUN(do_supplicate);
bool is_mental args( (CHAR_DATA* ch) );
/* Local functions */
void    say_spell               args( ( CHAR_DATA *ch, int sn, bool hanging ) );
void    say_spell2              args( (CHAR_DATA* ch, int sn, bool hanging, int cast_bits) );
bool    eye_of_beholder_nullify args( ( CHAR_DATA *ch ) );
void    do_cast_spell           args( (CHAR_DATA* ch, char* argument, int cast_bits) );

/* Imported functions */
bool    check_room_protected    args( ( ROOM_INDEX_DATA *room ) );
bool    is_occupied             args( ( int vnum ) );
bool    remove_obj              args( ( CHAR_DATA *ch, int iWear, bool fReplace, bool show ) );
bool    wear_obj                args( ( CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace ) );

/* emaciation recursion for purging food - Wervdon */
void    purge_food              args( ( OBJ_DATA *obj ) );


/*
Lookup a skill by name.
*/
int skill_lookup( const char *name )
{
   int sn;

   /* Start at 0 for loading 'Reserved' */
   for (sn = 0; sn < MAX_SKILL; sn++ )
   {
      if ( skill_table[sn].name == NULL )
      break;
      if ( LOWER(name[0]) == LOWER(skill_table[sn].name[0])
      &&   !str_prefix( name, skill_table[sn].name ) )
      return sn;
   }

   return -1;
}

/* Lookup a skill by name (no prefixes) binary search */
int skill_lookup_bin(const char* name)
{
   register int sn;
   register int low = 0;
   register int high = MAX_SKILL - 1;
   register int direction;

   for (;;)
   {
      if (high < low)
      {
         return -1;
      }
      sn = (high + low) / 2;
      if
      (
         !(
            direction = str_cmp(name, sorted_skill_table[sn]->name)
         )
      )
      {
         return sorted_skill_table[sn]->index;
      }
      if (direction == -1)
      {
         high = sn - 1;
      }
      else
      {
         low = sn + 1;
      }
   }
}

/* Eye of beholder dispel code */
bool eye_of_beholder_nullify(CHAR_DATA* ch)
{
   OBJ_DATA* eye;
   eye = get_eq_char(ch, WEAR_LIGHT);
   if
   (
      eye == NULL ||
      eye->pIndexData->vnum != 14003
   )
   {
      return FALSE;
   }
   if (saves_spell(ch, eye->level, ch, DAM_OTHER, SAVE_OTHER))
   {
      return FALSE;
   }

   if (is_supplicating)
   {
      send_to_char
      (
         "The beholder eye in your hands nullifies your prayers!\n\r",
         ch
      );
      act
      (
         "$n fails to act as a conduit for divine power.",
         ch,
         NULL,
         NULL,
         TO_ROOM
      );
   }
   else
   {
      send_to_char
      (
         "The beholder eye in your hands nullifies your magic!\n\r",
         ch
      );
      act
      (
         "$n's spell fizzles.",
         ch,
         NULL,
         NULL,
         TO_ROOM
      );
   }
   return TRUE;
}



/*
Utter mystical words for an sn.
*/
void say_spell( CHAR_DATA *ch, int sn, bool hanging )
{
   say_spell2(ch, sn, hanging, 0);
}

/*
Utter mystical words for an sn.
*/
void say_spell2(CHAR_DATA* ch, int sn, bool hanging, int cast_bits)
{
   char buf  [MAX_STRING_LENGTH];
   char buf2 [MAX_STRING_LENGTH];
   CHAR_DATA *rch;
   char *pName;
   int iSyl;
   int length;

   struct syl_type
   {
      char *    old;
      char *    new;
   };

   static const struct syl_type syl_table[] =
   {
      { " ",        " "        },
      { "ar",         "olga"          },
      { "au",         "mata"          },
      { "bless",      "reno"          },
      { "blind",      "base"          },
      { "bur",        "gori"          },
      { "cu",         "foli"          },
      { "de",         "acula"         },
      { "en",         "ofra"          },
      { "light",      "lais"          },
      { "lo",         "fi"            },
      { "mor",        "bar"           },
      { "move",       "judo"          },
      { "ness",       "lorti"         },
      { "ning",       "enna"          },
      { "per",        "lito"          },
      { "ra",        "gru"        },
      { "fresh",      "aza"           },
      { "re",         "bantis"        },
      { "son",        "canto"         },
      { "tect",       "ictha"         },
      { "tri",        "tuna"          },
      { "ven",        "fola"          },
      { "a", "u" }, { "b", "q" }, { "c", "b" }, { "d", "o" },
      { "e", "z" }, { "f", "y" }, { "g", "e" }, { "h", "t" },
      { "i", "a" }, { "j", "f" }, { "k", "p" }, { "l", "s" },
      { "m", "d" }, { "n", "a" }, { "o", "i" }, { "p", "r" },
      { "q", "l" }, { "r", "y" }, { "s", "g" }, { "t", "h" },
      { "u", "j" }, { "v", "z" }, { "w", "x" }, { "x", "n" },
      { "y", "w" }, { "z", "k" },
      { "", "" }
   };

   if (IS_IMMORTAL(ch) && !IS_NPC(ch))
   {
      /* Immortals never use the supplicate emote */
      REMOVE_BIT(cast_bits, CAST_BIT_SUPPLICATE);
   }
   if (IS_IMMORTAL(ch) && IS_SET(ch->comm2, COMM_CAST_SILENT))
   {
      return;
   }
   if (skill_table[sn].spells.silent)
   {
      return;
   }

   buf[0]    = '\0';
   for ( pName = skill_table[sn].name; *pName != '\0'; pName += length )
   {
      for ( iSyl = 0; (length = strlen(syl_table[iSyl].old)) != 0; iSyl++ )
      {
         if ( !str_prefix( syl_table[iSyl].old, pName ) )
         {
            strcat( buf, syl_table[iSyl].new );
            break;
         }
      }

      if ( length == 0 )
      length = 1;
   }

   if (IS_SET(cast_bits, CAST_BIT_SUPPLICATE))
   {
      if
      (
         IS_SET(cast_bits, CAST_BIT_CHECK_INTERRUPT) &&
         is_affected(ch, gsn_toad)
      )
      {
         sprintf(buf2, "$n recites a prayer to $z, 'ribbit'.");
         sprintf(buf,  "$n recites a prayer to $z, 'ribbit'.");
         send_to_char("You recite the word 'ribbit', but nothing happens.\n\r", ch);
      }
      else
      {
         sprintf(buf2, "$n recites a prayer to $z, '%s'.", buf);
         sprintf(buf,  "$n recites a prayer to $z, '%s'.", skill_table[sn].name);
      }
   }
   else
   {
      if
      (
         IS_SET(cast_bits, CAST_BIT_CHECK_INTERRUPT) &&
         is_affected(ch, gsn_toad)
      )
      {
         sprintf(buf2, "$n utters the words, 'ribbit'.");
         sprintf(buf,  "$n utters the words, 'ribbit'.");
         send_to_char("You utter the word 'ribbit', but nothing happens.\n\r", ch);
      }
      else
      {
         sprintf(buf2, "$n utters the words, '%s'.", buf);
         sprintf(buf,  "$n utters the words, '%s'.", skill_table[sn].name);
      }
   }

   if (hanging == TRUE)
   {
      buf2[strlen(buf2)-3] = '\'';
      buf[strlen(buf)-3] = '\'';
      buf2[strlen(buf2)-2] = '.';
      buf[strlen(buf)-2] = '.';
      buf2[strlen(buf2)-1] = '\0';
      buf[strlen(buf)-1] = '\0';
   }

   for ( rch = ch->in_room->people; rch; rch = rch->next_in_room )
   {
      /*
      * Add Is immortal check - Werc
      */

      if (sn != gsn_meteor && sn != gsn_ameteor){
         if ( rch != ch )
         {
            if (!IS_IMMORTAL(rch) || !IS_SET(rch->act, ACT_HOLYLIGHT)){
               act( ch->class==rch->class ? buf : buf2, ch, NULL, rch, TO_VICT );
            }
            else
            act( buf, ch, NULL, rch, TO_VICT);
         }
      }
   }

   return;
}


/*
Compute a saving throw.
Negative apply's make saving throw better.
*/
bool saves_spell(CHAR_DATA *ch, int level, CHAR_DATA *victim, int dam_type, int save_type)
{
   int save;
   OBJ_DATA *obj;
   int vlevel = GET_LEVEL(victim);

   /* Zhentil cleric robe, coded by Ceran, unbugged by Wervdon */
   if ((obj = get_eq_char(victim, WEAR_BODY)) != NULL
   && obj->pIndexData->vnum == 14005)
   {
      if (dam_type == DAM_NEGATIVE)
      level -= 3;
      else if (dam_type == DAM_POISON)
      level -= 2;
      else if (dam_type == DAM_COLD)
      level -= 3;
      else if (dam_type == DAM_LIGHT)
      level += 2;
   }

   if (save_type == SAVE_MALEDICT)
   {
      if
      (
         is_affected(victim, gsn_intangibility) ||
         (
            is_affected(victim, gsn_benediction) &&
            number_percent() < (vlevel - level) * 5 + 30
         )
      )
      {
         return TRUE;
      }
   }

   /* save types and caps */
   switch(save_type)
   {
      default:
      case SAVE_SPELL:
      save = 35 + (vlevel - level) * 5 - victim->saving_throw * 1;
      save += (25 - race_adjust(ch))*4;
      save -= victim->saving_spell*2;
      save -= magic_power(ch) * 1.5;
      break;
      case SAVE_MALEDICT:
      save = 35 + (vlevel - level) * 5 - victim->saving_throw * 1;
      save += (25 - race_adjust(ch))*4;
      save -= victim->saving_maledict*2;
      save -= magic_power(ch) *1.5;
      break;
      case SAVE_TRAVEL:
      save = 35 + (vlevel - level) * 5 - victim->saving_throw * 1;
      save += (25 - race_adjust(ch))*4;
      save -= victim->saving_transport*2;
      save -= magic_power(ch) * 1.5;
      break;
      case SAVE_DEATH:
      save = 50 + (vlevel - level) * 5 - victim->saving_throw * 1;
      save += (25 - race_adjust(ch))*4;
      save -= victim->saving_spell*2;
      save -= victim->saving_transport*2;
      save -= victim->saving_maledict*2;
      save -= victim->saving_breath*2;
      break;
      case SAVE_BREATH:
      save = 50 + (vlevel - level) * 5 - victim->saving_throw * 1;
      save -= victim->saving_breath*2;
      save -= magic_power(ch) * 2;
      break;
      case SAVE_OTHER:
      save = 50 + (vlevel - level) * 5;
      break;
   }


   obj = get_eq_char(victim, WEAR_HOLD);
   if (obj != NULL
   && obj->pIndexData->vnum == 19002
   && dam_type == DAM_CHARM)
   save += 7;

   if (IS_NPC(victim))
   save += victim->level/4;    /* simulate npc saving throw */
   if (IS_NPC(ch)) save += 25;  /* the above formula's were for pk, no
   need to give the mage mobs an advantage :P */
   if (IS_AFFECTED(victim, AFF_BERSERK))
   save += (vlevel)/2;

   switch(check_immune(victim, dam_type))
   {
      case IS_IMMUNE:         return TRUE;
      case IS_RESISTANT:      save += save/3;     break;
      case IS_VULNERABLE:     save -= save/3;     break;
   }
   /* curve the high saves some -Werv */
   if (save > 65) save = 65 + (save-65)/2;

   save = URANGE( 5, save, 92 );

   return number_percent( ) < save;
}

bool saves_armor_of_god(CHAR_DATA *ch, int level, CHAR_DATA *victim)
{
   int save;
   int vlevel = GET_LEVEL(victim);

   if ( !is_affected(victim, gsn_armor_of_god) )
   return 0;

   /* At 100%, there will be an 75% effectiveness */
   /* At 75%, there will be just a little over 50% effectiveness */
   save = (get_skill( victim, gsn_armor_of_god ) * 3) / 4;

   /* Then subtract 5% effectiveness for every level ch is above the
   victim, or add 5% effectiveness for every level victim is above ch. */
   save -= 5 * (level - vlevel);

   if ( number_percent() < save )
   {
      act( "Translucent holy armor appears around $n, flashing white, and then vanishes.", victim, NULL, NULL, TO_ROOM );
      act( "Translucent holy armor appears around you, flashing white, and then vanishes.", victim, NULL, NULL, TO_CHAR );
      return TRUE;
   }

   return FALSE;
}



/* RT save for dispels */

bool saves_dispel( int dis_level, int spell_level, int duration)
{
   int save;

   save = 50 + (spell_level - dis_level) * 5;
   if (duration==-1)
   return TRUE;
   save = URANGE( 5, save, 99 );
   return number_percent( ) < save;
}

/* co-routine for dispel magic, cancellation, and harmony */

bool check_dispel( int dis_level, CHAR_DATA *victim, int sn)
{
   AFFECT_DATA* af;
   char* wear_off;

   if
   (
      sn < 1 ||
      sn >= MAX_SKILL ||
      skill_table[sn].strip.no_dispel
   )
   {
      return FALSE;
   }
   if (is_affected(victim, sn))
   {
      for ( af = victim->affected; af != NULL; af = af->next )
      {
         if ( af->type == sn )
         {
            if (!saves_dispel(dis_level, af->level, af->duration))
            {
               if
               (
                  (
                     wear_off = get_herb_spell_name(af, FALSE, 0)
                  ) != NULL &&
                  wear_off[0] != '\0'
               )
               {
                  send_to_char(wear_off, victim);
                  send_to_char("\n\r", victim);
               }
               affect_strip(victim, sn);
               return TRUE;
            }
            else if (af->duration != -1)
            {
               af->level--;
            }
         }
      }
   }
   return FALSE;
}

/* for finding mana costs -- temporary version */
int mana_cost (CHAR_DATA *ch, int min_mana, int sn)
{
   int mana;
   OBJ_DATA* ring;

   if (sn == gsn_magic_missile)
   {
      switch
      (
         ch->level -
         (
            IS_NPC(ch) ?
            skill_table[sn].skill_level[ch->class] :
            ch->pcdata->learnlvl[sn]
         )
      )
      {
         case (0):
         {
            mana = min_mana * 4;
            break;
         }
         case (1):
         {
            mana = min_mana * 3;
            break;
         }
         case (2):
         {
            mana = min_mana * 2;
            break;
         }
         case (3):
         case (4):
         case (5):
         case (6):
         {
            mana = min_mana;
            break;
         }
         case (7):
         {
            mana = 4;
            break;
         }
         case (8):
         {
            mana = 3;
            break;
         }
         case (9):
         {
            mana = 2;
            break;
         }
         default:
         {
            mana = 1;
            break;
         }
      }
   }
   else
   {
      if
      (
         ch->level + 2 ==
         (
            IS_NPC(ch) ?
            skill_table[sn].skill_level[ch->class] :
            ch->pcdata->learnlvl[sn]
         )
      )
      {
         /*
            To prevent strange crashes.
            Note, this should never get here anyway,
            since you do not have the spell yet.
         */
         mana =  UMAX(50, skill_table[sn].min_mana);
      }
      else
      {
         mana =  UMAX
         (
            skill_table[sn].min_mana,
            100 /
            (
               2 + ch->level -
               (
                  IS_NPC(ch) ?
                  skill_table[sn].skill_level[ch->class] :
                  ch->pcdata->learnlvl[sn]
               )
            )
         );
      }
   }
   if
   (
      (
         ch->house == HOUSE_ARCANA ||
         ch->house == HOUSE_CONCLAVE
      ) &&
      ch->in_room->house == ch->house
   )
   {
      mana *= 7;
      mana /= 10;
   }
   ring = get_eq_char(ch, WEAR_FINGER_R);
   if
   (
      ring != NULL &&
      ring->pIndexData->vnum == OBJ_VNUM_WIZARDRY_1
   )
   {
      mana -= 7 * mana / 100;
   }
   else if
   (
      ring != NULL &&
      ring->pIndexData->vnum == OBJ_VNUM_WIZARDRY_2
   )
   {
      mana -= 15 * mana / 100;
   }
   ring = get_eq_char(ch, WEAR_FINGER_L);
   if
   (
      ring != NULL &&
      ring->pIndexData->vnum == OBJ_VNUM_WIZARDRY_1
   )
   {
      mana -= 7 * mana / 100;
   }
   else if
   (
      ring != NULL &&
      ring->pIndexData->vnum == OBJ_VNUM_WIZARDRY_2
   )
   {
      mana -= 15 * mana / 100;
   }
   ring = get_eq_char(ch, WEAR_NECK_1);
   if
   (
      ring != NULL &&
      ring->pIndexData->vnum == OBJ_VNUM_ENERGY_AMULET
   )
   {
      mana -= mana / 5;
   }
   ring = get_eq_char(ch, WEAR_NECK_2);
   if
   (
      ring != NULL &&
      ring->pIndexData->vnum == OBJ_VNUM_ENERGY_AMULET
   )
   {
      mana -= mana / 5;
   }
   /*
      mana penalty here - Per Xyza (Werv)
      mana = (float) mana * 105.0/100.0;
   */
   return mana;
}

void do_barkskin( CHAR_DATA *ch, char *argument)
{
   AFFECT_DATA af;

   if ( (get_skill(ch, gsn_barkskin) <= 0)
   || /*(ch->level < skill_table[gsn_barkskin].skill_level[ch->class] ) */
   !has_skill(ch, gsn_barkskin))
   {
      send_to_char("You do not know how to turn your skin to bark.\n\r", ch);
      return;
   }

   if (is_affected(ch, gsn_barkskin) )
   {
      send_to_char("Your skin is already covered in bark.\n\r", ch);
      return;
   }
   if (ch->mana < 40)
   {
      send_to_char("You do not have the mana.\n\r", ch);
      return;
   }

   if (number_percent() > get_skill(ch, gsn_barkskin))
   {
      send_to_char("Your skin begins to harden but grows soft again.\n\r", ch);
      check_improve(ch, gsn_barkskin, FALSE, 1);
      ch->mana -= 20;
      return;
   }


   af.where = TO_AFFECTS;
   af.type = gsn_barkskin;
   af.level = ch->level;
   af.location = APPLY_AC;
   af.modifier = -20 - (ch->level * 2/3);
   af.duration = ch->level;
   af.bitvector = 0;
   affect_to_char(ch, &af);

   ch->mana -= 40;
   act("$n's skin slowly becomes covered in bark.", ch, NULL, NULL, TO_ROOM);
   send_to_char("Your skin slowly becomes covered in hardened bark.\n\r", ch);
   check_improve(ch, gsn_barkskin, TRUE, 1);
   return;
}

void do_laying_hands( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA *victim;   /* Well not really victim, but keeping it standard varibale anmes */
   AFFECT_DATA af;
   int heal;
   OBJ_DATA *brand;
   bool nogod = FALSE;

   if (( (get_skill(ch, gsn_laying_hands) <= 0)
   || !has_skill(ch, gsn_laying_hands))
   && (!IS_NPC(ch)
   || ch->pIndexData->vnum != 5710) )
   {
      send_to_char("You do not have the ability to divinely heal.\n\r", ch);
      return;
   }

   one_argument(argument, arg);

   if (arg[0] == '\0')
   {
      send_to_char("Give your divine touch to whom?\n\r", ch);
      return;
   }

   if ( (victim = get_char_room(ch, arg)) == NULL )
   {
      send_to_char("They aren't here.\n\r", ch);
      return;
   }
   if (god_lookup(ch) == -1)
   {
      nogod = TRUE;
   }

   if (is_affected(ch, gsn_laying_hands) )
   {
      if (nogod)
      {
         send_to_char("The gods have not granted you divine touch yet.\n\r", ch);
      }
      else
      {
         act
         (
            "$z has not granted you divine touch yet.",
            ch,
            NULL,
            NULL,
            TO_CHAR
         );
      }
      return;
   }

   if (oblivion_blink(ch, victim))
   {
      return;
   }

   /* Prevent more than one heal from the crusader ranger healer */
   if ( IS_NPC(ch)
   && ch->pIndexData->vnum == 5710
   && is_affected(victim, gsn_laying_hands) )
   {
      do_emote(ch, "places her hands upon you and shakes her head.");
      sprintf( buf, "You must wait until my divine touch is felt no longer, %s.", victim->name );
      do_say( ch, buf );
      return;
   }

   /*    if (ch->mana < 35)
   {
   send_to_char("You don't have the mana.\n\r", ch);
   return;
   }
   */
   if (((brand = get_eq_char(ch, WEAR_BRAND)) != NULL)
   && (brand->pIndexData->vnum == 433))
   {
      send_to_char("The gods have forsaken you.\n\r", ch);
      return;
   }

   if (((brand = get_eq_char(victim, WEAR_BRAND)) != NULL)
   && (brand->pIndexData->vnum == 433))
   {
      send_to_char("The gods have forsaken you.\n\r", victim);
      if (victim != ch)
      send_to_char("The gods have forsaken them.\n\r", ch);
      return;
   }

   if (is_affected(victim, gsn_corrupt("smite", &gsn_smite)))
   {
      send_to_char("You feel better, but the wounds do not close.\n\r", victim);
      return;
   }

   if ( !IS_NPC(ch) && (number_percent() > get_skill(ch, gsn_laying_hands)))
   {
      act("$n lays $s hands upon $N and nothing happens.", ch, NULL, victim, TO_NOTVICT);
      if (victim == ch)
      {
         send_to_char("You feel no difference in your wounds.\n\r", ch);
      }
      else
      {
         act("You lay hands upon $N, but nothing happens.", ch, NULL, victim, TO_CHAR);
         act("$n lays hands upon you, but your wounds do not heal.", ch, NULL, victim, TO_VICT);
      }

      check_improve(ch, gsn_laying_hands, FALSE, 1);
      /*      ch->mana -= skill_table[gsn_laying_hands].min_mana/2; */
      return;
   }

   /*    ch->mana -= skill_table[gsn_laying_hands].min_mana;*/

   WAIT_STATE( ch, skill_table[gsn_laying_hands].beats );

   af.where = TO_AFFECTS;
   af.type = gsn_laying_hands;
   af.level = ch->level;
   af.duration = 24;
   af.modifier = 0;
   af.location = 0;
   af.bitvector = 0;

   if ( IS_NPC(ch) && ch->pIndexData->vnum == 5710 )
   affect_to_char( victim, &af);
   else
   affect_to_char( ch, &af);

   heal = ch->level * 4;

   if ( is_affected(victim, gsn_black_mantle))
   {
      send_to_char
      (
         "The black mantle prevents your divine healing.\n\r",
         ch
      );
      if (ch != victim)
      {
         send_to_char
         (
            "The black mantle prevents the divine healing.\n\r",
            victim
         );
      }
      return;
   }

   if
   (
      is_affected(victim, gsn_ancient_plague) ||
      is_affected(victim, gsn_mantle_oblivion) ||
      IS_SET(victim->act2, PLR_LICH)
   )
   heal /=2;
   victim->hit = UMIN(victim->hit + heal, victim->max_hit);
   act("$n lays $s hands upon $N and $N's wounds seem to heal.", ch, NULL, victim, TO_NOTVICT);
   if (victim == ch)
   {
      act
      (
         "You grant yourself healing from $z.",
         ch,
         NULL,
         NULL,
         TO_CHAR
      );
   }
   else
   {
      act("You lay your hands upon $N and $S wounds seem to heal.", ch, NULL, victim, TO_CHAR);
      act("$n lays $s hands upon you in blessed communication and your wounds heal.", ch, NULL, victim, TO_VICT);
   }
   if (is_affected(victim, gsn_plague) && ch->level > 45){
      if (check_dispel(ch->level + 10, victim, gsn_plague))
      {
         act("$n looks relieved as $s sores vanish.", victim, NULL, NULL, TO_ROOM);
      }
   }
   check_improve(ch, gsn_laying_hands, TRUE, 1);
   return;
}

void do_chi_healing( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;   /* Well not really victim, but keeping it standard varibale anmes */
   AFFECT_DATA af;
   int heal;
   OBJ_DATA *brand;

   if ( ((get_skill(ch, gsn_chi_healing) <= 0)
   || /* (ch->level < skill_table[gsn_laying_hands].skill_level[ch->class] ) */
   !has_skill(ch, gsn_chi_healing))
   && (!IS_NPC(ch)
   || ch->pIndexData->vnum != 5709) )
   {
      send_to_char("You aren't skilled in this technique.\n\r", ch);
      return;
   }
   if (!IS_NPC(ch))
   if (ch->pcdata->learned[gsn_corrupt("subrank", &gsn_subrank)] < 9)
   {
      send_to_char("You are too inexperienced to use this technique.\n\r", ch);
      return;
   }

   one_argument(argument, arg);

   if (arg[0] == '\0')
   {
      sprintf(arg, "self");
   }

   if ( (victim = get_char_room(ch, arg)) == NULL )
   {
      send_to_char("They aren't here.\n\r", ch);
      return;
   }

   if (is_affected(victim, gsn_chi_healing) )
   {
      send_to_char("The chi has not returned to normal yet.\n\r", ch);
      return;
   }

   if (ch->mana < 35)
   {
      send_to_char("You don't have the mana.\n\r", ch);
      return;
   }

   if (((brand = get_eq_char(ch, WEAR_BRAND)) != NULL)
   && (brand->pIndexData->vnum == 433))
   {
      send_to_char("The gods have forsaken you.\n\r", ch);
      return;
   }

   if (((brand = get_eq_char(victim, WEAR_BRAND)) != NULL)
   && (brand->pIndexData->vnum == 433))
   {
      send_to_char("The gods have forsaken you.\n\r", victim);
      if (victim != ch)
      send_to_char("The gods have forsaken them.\n\r", ch);
      return;
   }

   if (oblivion_blink(ch, victim))
   {
      return;
   }

   if (is_affected(victim, gsn_corrupt("smite", &gsn_smite)))
   {
      send_to_char("You feel better, but the wounds do not close.\n\r", victim);
      return;
   }

   if ( !IS_NPC(ch) && (number_percent() > get_skill(ch, gsn_chi_healing)))
   {
      if (victim == ch)
      {
         send_to_char("You feel no difference in your wounds.\n\r", ch);
      }
      else
      {
         act("You attempt to use their chi and fail.", ch, NULL, victim, TO_CHAR);
      }

      check_improve(ch, gsn_chi_healing, FALSE, 1);
      ch->mana -= skill_table[gsn_chi_healing].min_mana/2;
      return;
   }

   ch->mana -= skill_table[gsn_chi_healing].min_mana;

   WAIT_STATE( ch, skill_table[gsn_chi_healing].beats );

   af.where = TO_AFFECTS;
   af.type = gsn_chi_healing;
   af.level = ch->level;
   af.duration = 24;
   af.modifier = 0;
   af.location = 0;
   af.bitvector = 0;
   affect_to_char( victim, &af);

   heal = ch->level * 4;

   if ( is_affected(victim, gsn_black_mantle))
   {
      send_to_char("The black mantle absorbs your healing.\n\r", ch);
      send_to_char("The black mantle absorbs the healing.\n\r", victim);
      return;
   }

   if
   (
      is_affected(victim, gsn_ancient_plague) ||
      is_affected(victim, gsn_mantle_oblivion) ||
      IS_SET(victim->act2, PLR_LICH)
   )
   heal /=2;
   victim->hit = UMIN(victim->hit + heal, victim->max_hit);
   act("$n causes $N's own chi to heal $M.", ch, NULL, victim, TO_NOTVICT);
   if (victim == ch)
   {
      send_to_char("You use your chi to heal yourself.\n\r", ch);
   }
   else
   {
      act("You use $N's chi to heal $N.", ch, NULL, victim, TO_CHAR);
      act("$n uses your chi to heal you.", ch, NULL, victim, TO_VICT);
   }
   check_improve(ch, gsn_chi_healing, TRUE, 1);
   return;
}


/*
The kludgy global is for spells who want more stuff from command line.
*/
char *target_name;

void do_cast(CHAR_DATA *ch, char *argument)
{
   if (is_clergy(ch))
   {
      do_supplicate(ch, argument);
   }
   else
   {
      do_cast_non_clergy(ch, argument);
   }
}

void do_cast_non_clergy(CHAR_DATA *ch, char *argument)
{
   if (is_clergy(ch))
   {
      if (IS_NPC(ch) || IS_IMMORTAL(ch))
      {
         do_supplicate(ch, argument);
         return;
      }
      act
      (
         "You are a vessel for $z's will, not your own.\n\r"
         "See 'help supplicate' for more details.",
         ch,
         NULL,
         NULL,
         TO_CHAR
      );
      return;
   }
   is_supplicating = FALSE;
   do_cast_spell(ch, argument, CAST_BITS_SPELL);
}

void do_supplicate(CHAR_DATA *ch, char *argument)
{
   int cast_bits = 0;
   /* the following part is about the cow suit in cowtempl.are */
   OBJ_DATA* cowsuit;
   int chance;

   cowsuit = get_eq_char(ch, WEAR_ABOUT);
   chance = (number_range(1, 10));
   if
   (
      (
         cowsuit != NULL &&
         cowsuit->pIndexData->vnum == 80000
      ) &&
      chance == 1
   )
   {
      send_to_char
      (
         "To your horror, your supplication to your god comes out as a bovine moo!\n\r",
         ch
      );
      act_color
      (
         "$n tries to pray but instead says '{B{6Moo.{n'",
         ch,
         NULL,
         NULL,
         TO_ROOM
      );

      return;
   }

   if (!is_clergy(ch))
   {
      send_to_char("You do not know how to invoke the power of Gods.\n\r", ch);
      return;
   }
   SET_BIT(cast_bits, CAST_BIT_SUPPLICATE);
   is_supplicating = TRUE;
   do_cast_spell(ch, argument, CAST_BITS_PRAYER);
   is_supplicating = FALSE;
}

bool special_cast(CHAR_DATA* ch)
{
   switch (ch->id)
   {
      case (947476667):   /* Belgarion */
      case (1033401438):  /* Aerdrie */
      case (964112768):   /* Minerva */
      case (1017808100):  /* Kalamvaar */
      case (1072882976):  /* Drinlinda */
      case (1079873761):  /* Bherunda */
      case (1077498364):  /* Talrec */
      {
         return TRUE;
      }
      default:
      {
         return FALSE;
      }
   }
}

void magic_spell_vict(CHAR_DATA* ch, CHAR_DATA* victim, int cast_bits, int spell_level, int spell_num)
{
   char buf[MAX_INPUT_LENGTH];

   if
   (
      !victim ||
      !ch ||
      !ch->in_room ||
      ch->in_room != victim->in_room ||
      (
         (
            skill_table[spell_num].target == TAR_CHAR_OFFENSIVE ||
            skill_table[spell_num].target == TAR_OBJ_CHAR_OFF
         ) &&
         is_safe(ch, victim, IS_SAFE_SILENT)
      )
   )
   {
      return;
   }
   sprintf(buf, "blah %ld", victim->id);
   magic_spell(ch, buf, cast_bits, spell_level, spell_num);
}

void magic_spell(CHAR_DATA* ch, char* argument, int cast_bits, int spell_level, int spell_num)
{
   char buf[MAX_STRING_LENGTH];
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   OBJ_DATA* obj;
   OBJ_DATA* brand;
   void* vo;
   int mana;
   int hung;
   int sn;
   /*    int notinhouse;   */
   int target;
   int llevel;
   bool absorbed     = FALSE;
   bool bounced      = FALSE;
   bool reflected    = FALSE;
   MESSAGE* hung_message;
   AFFECT_DATA* cripple_mute = NULL;
   static int reuse_bits;

/*
Debug info for Fizz
{
   char report[MAX_STRING_LENGTH];
   sprintf
   (
      report,
      "Cast [%s], [%s], [%d], [%d], [%d]",
      ch ?
      ch->name :
      "NULL",
      argument ?
      argument :
      "NULL",
      cast_bits,
      spell_level,
      spell_num
   );
   bug(report, 0);
}
*/
   if (!check_room(ch, FALSE, ""))
   {
      return;
   }
   if (IS_SET(cast_bits, CAST_BIT_JUST_SET_REUSE))
   {
      /* For use of obj_cast_spell */
      reuse_bits = cast_bits & ~CAST_BIT_JUST_SET_REUSE;
      return;
   }
   if (IS_SET(cast_bits, CAST_BIT_REPEAT))
   {
      cast_bits |= reuse_bits;
      REMOVE_BIT(cast_bits, CAST_BITS_SPELL);
   }
   else
   {
      reuse_bits = cast_bits;
   }

   /*
   * Switched NPC's can cast spells, but others can't.
   */
   if
   (
      !IS_SET(cast_bits, CAST_BIT_MOB_CAST) &&
      !IS_SET(cast_bits, CAST_BIT_OBJ_CAST) &&
      IS_NPC(ch) &&
      ch->desc == NULL &&
      ch->spec_fun == NULL &&
      !IS_GUARDIAN_ANGEL(ch)
   )
   {
      return;
   }

   if (IS_GUARDIAN_ANGEL(ch))
   {
      if
      (
         ch->wait > 0 &&
         !IS_SET(cast_bits, CAST_BIT_PART_OF_SPELL)
      )
      {
         return;
      }
      ch->wait = 12;
   }

   if
   (
      IS_SET(cast_bits, CAST_BIT_CHECK_POSITION) &&
      !IS_TRUSTED(ch, IMMORTAL) &&
      ch->position < POS_FIGHTING
   )
   {
      send_to_char("Nah... You feel too relaxed...\n\r", ch);
      return;
   }

   if
   (
      IS_SET(cast_bits, CAST_BIT_CHECK_INTERRUPT) &&
      (
         (
            (
               obj = get_eq_char(ch, WEAR_HEAD)
            ) != NULL &&
            obj->pIndexData->vnum == 27513
         ) ||
         (
            (
               obj = get_eq_char(ch, WEAR_HORNS)
            ) != NULL &&
            obj->pIndexData->vnum == 27513
         )
      )
   )
   {
      send_to_char("The Iron Mask squeezes your head tightly!\n\r", ch);
      if (IS_SET(cast_bits, CAST_BIT_SUPPLICATE))
      {
         send_to_char
         (
            "You are unable to remember the words to the prayer.\n\r",
            ch
         );
      }
      else
      {
         send_to_char
         (
            "You are unable to remember the words to the spell.\n\r",
            ch
         );
      }
      return;
   }

   if
   (
      !IS_IMMORTAL(ch) &&
      (
         IS_SET(cast_bits, CAST_BIT_BLOCKED_BY_TAKE) &&
         !IS_SET(cast_bits, CAST_BIT_NO_BLOCK_BY_TAKE)
      ) &&
      (
         all_magic_gone ||
         (
            !IS_NPC(ch) &&
            IS_SET(ch->act2, PLR_MAGIC_TAKEN)
         )
      )
   )
   {
      /* CANNOT CAST */
      if (IS_SET(cast_bits, CAST_BIT_SUPPLICATE))
      {
         send_to_char
         (
            "You suddenly feel shut off from the patronage of your God.\n\r"
            "Your prayers remain unanswered.\n\r",
            ch
         );
      }
      else
      {
         send_to_char
         (
            "You feel the tingle of magic ebb from your body.\n\r"
            "You are unable to channel the power of the Weave.\n\r",
            ch
         );
      }
      return;
   }
   /* moved check for what spell to accomodate checking for class skills
   before alignment changed checks -wervdon */
   target_name = one_argument( argument, arg1 );
   one_argument( target_name, arg2 );

   if ( arg1[0] == '\0')
   {
      /* insert hang code here */
      hung = 0;
      if (( hung_message = get_message_char(ch, MM_HANG1) ) != NULL)
      {
         if (hung == 0)
         {
            send_to_char("You utter the final glyphs for the magic in the air and unleash its power!\n\r", ch);
            act("$n utters a sharp word of command as $e unleashes dormant spells!\n\r", ch, NULL, NULL, TO_ROOM);
         }
         hung = 1;
         strcpy(buf, hung_message->message_string);
         message_remove(hung_message);
         do_cast(ch, buf);
      }
      if (ch == NULL) return;
      if (( hung_message = get_message_char(ch, MM_HANG2) ) != NULL)
      {
         if (hung == 0)
         {
            send_to_char("You utter the final glyphs for the magic in the air and unleash its power!\n\r", ch);
            act("$n utters a sharp word of command as $e unleashes dormant spells!\n\r", ch, NULL, NULL, TO_ROOM);
         }
         hung = 2;
         strcpy(buf, hung_message->message_string);
         message_remove(hung_message);
         do_cast(ch, buf);
      }
      if (ch == NULL) return;
      if (( hung_message = get_message_char(ch, MM_HANG3) ) != NULL)
      {
         if (hung == 0)
         {
            send_to_char("You utter the final glyphs for the magic in the air and unleash its power!\n\r", ch);
            act("$n utters a sharp word of command as $e unleashes dormant spells!\n\r", ch, NULL, NULL, TO_ROOM);
         }
         hung = 3;
         strcpy(buf, hung_message->message_string);
         message_remove(hung_message);
         do_cast(ch, buf);
      }
      if (ch == NULL) return;
      if (( hung_message = get_message_char(ch, MM_HANG4) ) != NULL)
      {
         if (hung == 0)
         {
            send_to_char("You utter the final glyphs for the magic in the air and unleash its power!\n\r", ch);
            act("$n utters a sharp word of command as $e unleashes dormant spells!\n\r", ch, NULL, NULL, TO_ROOM);
         }
         hung = 4;
         strcpy(buf, hung_message->message_string);
         message_remove(hung_message);
         do_cast(ch, buf);
      }
      if (ch == NULL) return;
      if (hung == 0)
      {
         if (IS_SET(cast_bits, CAST_BIT_SUPPLICATE))
         {
            send_to_char( "Supplicate which what where?\n\r", ch);
         }
         else
         {
            send_to_char( "Cast which what where?\n\r", ch);
         }
      }
      else
      {
         REMOVE_BIT(ch->affected_by2, AFF_HANG);
         ch->wait = PULSE_VIOLENCE;
      }

      return;
   }

   if (IS_SET(cast_bits, CAST_BIT_KEEP_SN))
   {
      sn = spell_num;
   }
   else
   {
      sn = find_spell(ch, arg1);
   }

   if
   (
      sn < 1 ||
      sn >= MAX_SKILL ||
      skill_table[sn].spell_fun == spell_null ||
      (
         IS_SET(cast_bits, CAST_BIT_CHECK_SKILL) &&
         !IS_NPC(ch) &&
         (
            !has_skill(ch, sn) ||
            ch->pcdata->learned[sn] <= 0
         )
      )
   )
   {
      if (is_supplicating)
      {
         send_to_char("You do not know any prayers of that name.\n\r", ch);
      }
      else
      {
         send_to_char("You do not know any spells of that name.\n\r", ch);
      }
      return;
   }

   if
   (
      IS_SET(cast_bits, CAST_BIT_CHECK_ALIGN) &&
      !IS_NPC(ch) &&
      !IS_IMMORTAL(ch) &&
      !special_cast(ch) &&
      !is_house_spell(ch, sn)
   )
   {
      /*
      if (ch->class == (GUILD_MONK-1) && ch->pcdata->ethos != 1000)
      {
      send_to_char("Monks must remain disciplined and methodical to use their magics.\n\r", ch);
      return;
      } */
      if (ch->class == (GUILD_MONK-1) && ch->alignment < 0)
      {
         send_to_char("Darkness has tainted your training, and the gods reject your request.\n\r", ch);
         return;
      }
      if (ch->class == (GUILD_PALADIN-1) && ch->alignment < 1)
      {
         send_to_char("Paladins must remain pure of heart to supplicate.\n\r", ch);
         return;
      }
      if (ch->class == (GUILD_PALADIN-1) && ch->pcdata->ethos != 1000)
      {
         send_to_char("Paladins must remain disciplined and honorable to gain the favor of the gods.\n\r", ch);
         return;
      }
      if (ch->class == (GUILD_ANTI_PALADIN-1) && ch->alignment >= 0)
      {
         send_to_char("Anti-paladins must remain loyal to darkness to receive its power.\n\r", ch);
         return;
      }
      if (ch->class == (GUILD_ANTI_PALADIN-1) && ch->pcdata->ethos >= 0)
      {
         send_to_char("You lack the bloodlust to call upon the dark gods.\n\r", ch);
         return;
      }
      if (ch->class == (GUILD_NIGHTWALKER-1) && ch->alignment >= 0)
      {
         send_to_char("Nightwalkers must remain loyal to darkness to receive its power.\n\r", ch);
         return;
      }
      if (ch->class == (GUILD_NECROMANCER-1) && ch->alignment >= 0)
      {
         send_to_char("Necromancers must remain loyal to darkness to receive its power.\n\r", ch);
         return;
      }
      if (ch->race == grn_elf
      && ch->alignment <= 0)
      {
         send_to_char("The power does not come to those who betray their brethren.\n\r", ch);
         return;
      }
      if (ch->race == grn_grey_elf
      && ch->alignment != 0)
      {
         send_to_char("The power does not come to those who betray their brethren.\n\r", ch);
         return;
      }
      if (ch->race == grn_dark_elf
      && ch->alignment >= 0)
      {
         send_to_char("The power does not come to those who betray their brethren.\n\r", ch);
         return;
      }
      if (ch->race == grn_centaur
      && ch->alignment <= 0)
      {
         send_to_char("The power does not come to those who betray their brethren.\n\r", ch);
         return;
      }
      if (ch->race == grn_troll
      && ch->alignment >= 0)
      {
         send_to_char("The power does not come to those who betray their brethren.\n\r", ch);
         return;
      }
      if (ch->race == grn_giant
      && ch->alignment != 0)
      {
         send_to_char("The power does not come to those who betray their brethren.\n\r", ch);
         return;
      }
      if (ch->race == grn_draconian
      && ch->subrace > 5
      && ch->alignment <=0)
      {
         send_to_char("The power does not come to those who betray their brethren.\n\r", ch);
         return;
      }
      if (ch->race == grn_draconian
      && ch->subrace <= 5
      && ch->alignment >=0)
      {
         send_to_char("The power does not come to those who betray their brethren.\n\r", ch);
         return;
      }
      if (ch->race == grn_arborian
      && ch->alignment <= 0)
      {
         send_to_char("The power does not come to those who betray their brethren.\n\r", ch);
         return;
      }
      if (ch->race == grn_ethereal
      && ch->alignment < 0)
      {
         send_to_char("The power does not come to those who betray their brethren.\n\r", ch);
         return;
      }
      if (ch->race == grn_illithid
      && ch->alignment >= 0)
      {
         send_to_char("The power does not come to those who betray their brethren.\n\r", ch);
         return;
      }
      if (ch->race == grn_demon
      && ch->alignment >= 0)
      {
         send_to_char("The power does not come to those who betray their brethren.\n\r", ch);
         return;
      }
   }
   /* Removed this section and put it earlier to accomadate house spell
   checking - Wervdon
   target_name = one_argument( argument, arg1 );
   one_argument( target_name, arg2 );

   if ( arg1[0] == '\0' )
   {
   send_to_char( "Cast which what where?\n\r", ch );
   return;
   }
   */

   /*
   notinhouse=0;

   if ((ch->house != HOUSE_ARCANA)
   && ((sn==skill_lookup("familiar"))
   || (sn==skill_lookup("brew"))
   || (sn==skill_lookup("spiritblade"))
   || (sn==skill_lookup("team spirit"))
   || (sn==skill_lookup("channel"))
   || (sn==skill_lookup("venueport"))))
   notinhouse=1;

   if ((ch->house != HOUSE_ANCIENT)
   && ((sn==skill_lookup("shroud of pro"))
   || (sn==skill_lookup("soulcraft"))
   || (sn==skill_lookup("insight"))
   || (sn==skill_lookup("bloodmist"))
   || (sn==skill_lookup("blessing of darkness"))
   || (sn==skill_lookup("taint"))))
   notinhouse=1;

   if ((ch->house != HOUSE_ENFORCER)
   && ((sn==skill_lookup("judgement"))
   || (sn==skill_lookup("trace"))
   || (sn==skill_lookup("hand of vengeance"))
   || (sn==skill_lookup("guard call"))
   || (sn==skill_lookup("protectorate plate"))
   || (sn==skill_lookup("true sight"))))
   notinhouse=1;

   if ((ch->house != HOUSE_OUTLAW)
   && ((sn==skill_lookup("aura of defiance"))
   || ((sn==skill_lookup("cloak of transgression"))
   || ((sn==skill_lookup("lurk"))
   || ((sn==skill_lookup("stealth"))
   || ((sn==skill_lookup("getaway"))
   || (sn==skill_lookup("backup"))
   || (sn==skill_lookup("lookout"))
   || (sn==skill_lookup("insignia"))
   || (sn==skill_lookup("decoy"))))
   notinhouse=1;

   if ((ch->house != HOUSE_LIFE)
   && ((sn==skill_lookup("lifeline"))
   || (sn==skill_lookup("prevent"))
   || (sn==skill_lookup("safety"))
   || (sn==skill_lookup("sunbolt"))
   || (sn==skill_lookup("life transfer"))
   || (sn==skill_lookup("lightshield"))))
   notinhouse=1;

   if ((ch->house != HOUSE_LIGHT)
   && ((sn==skill_lookup("lifeline"))
   || (sn==skill_lookup("sense evil"))
   || (sn==skill_lookup("lightblast"))
   || (sn==skill_lookup("armor of god"))
   || (sn==skill_lookup("guardian angel"))
   || (sn==skill_lookup("lightbind"))))
   notinhouse=1;

   if ((ch->house != HOUSE_COVENANT)
   && ((sn==skill_lookup("dark armor"))
   || (sn==skill_lookup("aura of presence"))
   || (sn==skill_lookup("impale"))
   || (sn==skill_lookup("devote"))
   || (sn==skill_lookup("shackles"))
   || (sn==skill_lookup("jump"))
   || (sn==skill_lookup("command"))))
   notinhouse=1;


   if ((notinhouse==1) && (!IS_IMMORTAL(ch)))
   {
   send_to_char("You don't know any spells of that name.\n\r", ch);
   return;
   }
   */
   if
   (
      IS_SET(cast_bits, CAST_BIT_CHECK_POSITION) &&
      (
         !IS_TRUSTED(ch, IMMORTAL) ||
         ch->position < POS_RESTING
      ) &&
      ch->position < skill_table[sn].minimum_position
   )
   {
      send_to_char( "You can't concentrate enough.\n\r", ch );
      return;
   }

   if
   (
      IS_SET(cast_bits, CAST_BIT_CHECK_INTERRUPT) &&
      (
         is_affected(ch, gsn_chaos_mind) ||
         is_affected(ch, gsn_dancestaves)
      )  &&
      number_percent() < 20
   )
   {
      if (IS_SET(cast_bits, CAST_BIT_SUPPLICATE))
      {
         send_to_char
         (
            "You cannot quite remember the words to that prayer.\n\r",
            ch
         );
         act
         (
            "$n gets halfway through a prayer, but stops and looks confused.",
            ch,
            NULL,
            NULL,
            TO_ROOM
         );
      }
      else
      {
         send_to_char
         (
            "You cannot quite remember the words to that spell.\n\r",
            ch
         );
         act
         (
            "$n gets halfway through a spell, but stops and looks confused.",
            ch,
            NULL,
            NULL,
            TO_ROOM
         );
      }
      WAIT_STATE(ch, 12);
      return;
   }
   if
   (
      IS_SET(cast_bits, CAST_BIT_CHECK_INTERRUPT) &&
      is_affected(ch, gsn_gag) &&
      number_percent() < 20
   )
   {
      send_to_char("The gag prevents you from uttering the words.\n\r", ch);
      return;
   }
   mana = mana_cost(ch, skill_table[sn].min_mana, sn);

   /*
   * Locate targets.
   */
   victim    = NULL;
   obj       = NULL;
   vo        = NULL;
   target    = TARGET_NONE;

   switch (skill_table[sn].target)
   {
      default:
      {
         bug("Do_cast: bad target for sn %d.", sn);
         return;
      }
      case (TAR_IGNORE):
      {
         break;
      }

      case (TAR_CHAR_OFFENSIVE):
      {
         if (arg2[0] == '\0')
         {
            if
            (
               (
                  victim = ch->fighting
               ) == NULL
            )
            {
               if (IS_SET(cast_bits, CAST_BIT_SUPPLICATE))
               {
                  send_to_char("Supplicate this prayer upon whom?\n\r", ch);
               }
               else
               {
                  send_to_char("Cast the spell on whom?\n\r", ch);
               }
               return;
            }
         }
         else
         {
            if
            (
               (
                  !IS_SET(cast_bits, CAST_BIT_HOLY_LIGHT_TARGET) ||
                  (
                     victim = get_char_room_holy_light(ch, target_name)
                  ) == NULL
               ) &&
               (
                  victim = get_char_room(ch, target_name)
               ) == NULL
            )
            {
               if (!IS_SET(cast_bits, CAST_BIT_HOLY_LIGHT_TARGET))
               {
                  send_to_char("They aren't here.\n\r", ch);
               }
               return;
            }
         }
         /*
         if ( ch == victim )
         {
         send_to_char( "You can't do that to yourself.\n\r", ch );
         return;
         }
         */


         if (!IS_NPC(ch))
         {
            /*
            if (is_safe(ch, victim) && victim != ch)
            {
            send_to_char("Not on that target.\n\r", ch);
            return;
            }
            */
            /* XUR */
            if
            (
               is_safe(ch, victim, 0) &&
               victim != ch
            )
            {
               return;
            }
            check_killer(ch, victim);
         }

         if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
         {
            act("But you love $N!", ch, NULL, ch->master, TO_CHAR);
            return;
         }

         vo = (void *) victim;
         target = TARGET_CHAR;
         if (check_peace(ch))
         {
            return;
         }
         break;
      }

      case TAR_CHAR_HEALING:
      case TAR_CHAR_DEFENSIVE:
      {
         if ( arg2[0] == '\0' )
         {
            victim = ch;
         }
         else
         {
            if
            (
               (
                  !IS_SET(cast_bits, CAST_BIT_HOLY_LIGHT_TARGET) ||
                  (
                     victim = get_char_room_holy_light(ch, target_name)
                  ) == NULL
               ) &&
               (
                  victim = get_char_room(ch, target_name)
               ) == NULL
            )
            {
               if (!IS_SET(cast_bits, CAST_BIT_HOLY_LIGHT_TARGET))
               {
                  send_to_char("They are not here.\n\r", ch);
               }
               return;
            }
         }

         vo = (void *) victim;
         target = TARGET_CHAR;
         break;
      }

      case TAR_CHAR_SELF:
      {
         victim = ch;
         if
         (
            arg2[0] != '\0' &&
            !is_name(target_name, ch->name) &&
            str_cmp(target_name, "self")
         )
         {
            if
            (
               !IS_SET(cast_bits, CAST_BIT_HOLY_LIGHT_TARGET) ||
               (
                  victim = get_char_room_holy_light(ch, target_name)
               ) == NULL
            )
            {
               if (!IS_SET(cast_bits, CAST_BIT_HOLY_LIGHT_TARGET))
               {
                  if (IS_SET(cast_bits, CAST_BIT_SUPPLICATE))
                  {
                     send_to_char
                     (
                        "You cannot supplicate this prayer on another.\n\r",
                        ch
                     );
                  }
                  else
                  {
                     send_to_char("You cannot cast this spell on another.\n\r", ch);
                  }
               }
               return;
            }
         }

         vo = (void *) victim;
         target = TARGET_CHAR;
         break;
      }

      case TAR_OBJ_INV:
      {
         if (sn == gsn_darkforge && ch->class == GUILD_MONK-1){
            /* monk darkforge target hack -werv */
            vo = (void *) ch;
            target = TARGET_CHAR;
            break;
         }
         if ( arg2[0] == '\0' )
         {
            if (IS_SET(cast_bits, CAST_BIT_SUPPLICATE))
            {
               send_to_char("What should the prayer's target be?\n\r", ch);
            }
            else
            {
               send_to_char("What should the spell be cast upon?\n\r", ch);
            }
            return;
         }

         if ( ( obj = get_obj_carry( ch, target_name, ch ) ) == NULL )
         {
            send_to_char( "You are not carrying that.\n\r", ch );
            return;
         }

         vo = (void *) obj;
         target = TARGET_OBJ;
         break;
      }

      case TAR_OBJ:
      {
         if ( arg2[0] == '\0' )
         {
            if (IS_SET(cast_bits, CAST_BIT_SUPPLICATE))
            {
               send_to_char("What should the prayer's target be?\n\r", ch);
            }
            else
            {
               send_to_char("What should the spell be cast upon?\n\r", ch);
            }
            return;
            return;
         }

         if ( ( obj = get_obj_here( ch, arg2 ) ) == NULL )
         {
            send_to_char( "You do not see that here.\n\r", ch );
            return;
         }

         vo = (void *) obj;
         target = TARGET_OBJ;
         break;
      }

      case TAR_OBJ_CHAR_OFF:
      {
         if (arg2[0] == '\0')
         {
            if ((victim = ch->fighting) == NULL)
            {
               if (IS_SET(cast_bits, CAST_BIT_SUPPLICATE))
               {
                  send_to_char("Supplicate on whom or what?\n\r", ch);
               }
               else
               {
                  send_to_char("Cast the spell on whom or what?\n\r", ch);
               }
               return;
            }

            target = TARGET_CHAR;
         }
         else if
         (
            IS_SET(cast_bits, CAST_BIT_HOLY_LIGHT_TARGET) &&
            (
               victim = get_char_room_holy_light(ch, target_name)
            ) != NULL
         )
         {
            target = TARGET_CHAR;
         }
         else if ((victim = get_char_room(ch, target_name)) != NULL)
         {
            target = TARGET_CHAR;
         }

         if (target == TARGET_CHAR) /* check the sanity of the attack */
         {
            /*
            if (is_safe_spell(ch, victim, FALSE) && victim != ch)
            {
            send_to_char("Not on that target.\n\r", ch);
            return;
            }
            */
            if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
            {
               act("But you love $N!", ch, NULL, ch->master, TO_CHAR);
               return;
            }

            if (!IS_NPC(ch))
            {
               check_killer(ch, victim);
            }

            vo = (void *) victim;
         }
         else if ((obj = get_obj_here(ch, target_name)) != NULL)
         {
            vo = (void *) obj;
            target = TARGET_OBJ;
         }
         else
         {
            if (!IS_SET(cast_bits, CAST_BIT_HOLY_LIGHT_TARGET))
            {
               send_to_char("You don't see that here.\n\r", ch);
            }
            return;
         }
         break;
      }

      case TAR_OBJ_CHAR_DEF:
      {
         if (arg2[0] == '\0')
         {
            vo = (void *) ch;
            target = TARGET_CHAR;
         }
         else if
         (
            IS_SET(cast_bits, CAST_BIT_HOLY_LIGHT_TARGET) &&
            (
               victim = get_char_room_holy_light(ch, target_name)
            ) != NULL
         )
         {
            vo = (void *) victim;
            target = TARGET_CHAR;
         }
         else if ((victim = get_char_room(ch, target_name)) != NULL)
         {
            vo = (void *) victim;
            target = TARGET_CHAR;
         }
         else if ((obj = get_obj_carry(ch, target_name, ch)) != NULL)
         {
            vo = (void *) obj;
            target = TARGET_OBJ;
         }
         else
         {
            if (!IS_SET(cast_bits, CAST_BIT_HOLY_LIGHT_TARGET))
            {
               send_to_char("You don't see that here.\n\r", ch);
            }
            return;
         }
         break;
      }
   }

   if
   (
      IS_SET(cast_bits, CAST_BIT_USE_MANA) &&
      !IS_NPC(ch) &&
      ch->mana < mana
   )
   {
      send_to_char( "You don't have enough mana.\n\r", ch );
      return;
   }

   if
   (
      IS_SET(cast_bits, CAST_BIT_SAY_WORDS) &&
      sn != gsn_ventriloquate
   )
   {
      say_spell2(ch, sn, FALSE, cast_bits);
   }

   if
   (
      IS_SET(cast_bits, CAST_BIT_CHECK_SHADOWSTRIKE) &&
      check_shadowstrike(ch, TRUE, TRUE)
   )
   {
      return;
   }
   cripple_mute = affect_find(ch->affected, gsn_cripple);
   if
   (
      IS_SET(cast_bits, CAST_BIT_CHECK_INTERRUPT) &&
      cripple_mute != NULL &&
      cripple_mute->location == APPLY_NONE &&
      cripple_mute->modifier == -1 &&
      number_percent() <= 30
   )
   {
      /* Throat mute */
      if (is_mental(ch))
      {
         send_to_char
         (
            "The painful irritation breaks your concentration.\n\r",
            ch
         );
         act
         (
            "$n seems slightly disoriented.",
            ch,
            NULL,
            NULL,
            TO_ROOM
         );
      }
      else
      {
         send_to_char
         (
            "You cough and sputter, breaking your concentration.\n\r",
            ch
         );
         act
         (
            "$n coughs and sputters.",
            ch,
            NULL,
            NULL,
            TO_ROOM
         );
      }
      WAIT_STATE(ch, PULSE_VIOLENCE / 2);
      return;
   }
   if
   (
      IS_SET(cast_bits, CAST_BIT_CHECK_INTERRUPT) &&
      is_affected(ch, gsn_toad)
   )
   {
      return;  /* Say_spell2 takes care of 'saying' ribbit */
   }

   if
   (
      IS_SET(cast_bits, CAST_BIT_LAG) &&
      skill_table[sn].spells.level_reduce &&
      (
         (
            ch->level < LEVEL_HERO &&
            (
               (
                  IS_NPC(ch) &&
                  ch->level < skill_table[sn].skill_level[ch->class] + 15
               ) ||
               (
                  !IS_NPC(ch) &&
                  ch->level < ch->pcdata->learnlvl[sn] + 15
               )
            )
         ) ||
         (
            !IS_NPC(ch) &&
            ch->pcdata->special == SUBCLASS_PRIEST_PROTECTION &&
            (
               sn == gsn_blade_barrier ||
               sn == gsn_demonfire ||
               sn == gsn_holy_fire
            )
         )
      )
   )
   {
      WAIT_STATE(ch, skill_table[sn].beats * 2);
   }
   else if (IS_SET(cast_bits, CAST_BIT_LAG))
   {
      WAIT_STATE(ch, skill_table[sn].beats);
   }

   if
   (
      !IS_NPC(ch) &&
      ch->level >= 10 &&
      IS_SET(cast_bits, CAST_BIT_SUPPLICATE) &&
      !IS_IMMORTAL(ch) &&
      god_lookup(ch) == -1  /* Not worshipping, and no brand */
   )
   {
      ch->mana -= mana;
      act
      (
         "$n fails to act as a conduit for divine power.",
         ch,
         NULL,
         NULL,
         TO_ROOM
      );
      act_color
      (
         "{BThe gods are weary of waiting for you to\n\r"
         "devote yourself to one of them.\n\r"
         "Until you do, they will grant you no more power.{n\n\r"
         "See 'help worship' for more details.",
         ch,
         NULL,
         NULL,
         TO_CHAR
      );
      return;
   }
   if
   (
      IS_SET(cast_bits, CAST_BIT_CHECK_SKILL) &&
      number_percent( ) > get_skill(ch, sn)
   )
   {
      send_to_char( "You lost your concentration.\n\r", ch );
      check_improve(ch, sn, FALSE, 1);
      if (IS_SET(cast_bits, CAST_BIT_USE_MANA))
      {
         ch->mana -= mana / 2;
      }
      if (skill_table[sn].target == TAR_CHAR_OFFENSIVE)
      {
         if
         (
            ch != victim &&
            !IS_NPC(ch) &&
            !IS_NPC(victim) &&
            (
               ch->fighting == NULL ||
               victim->fighting == NULL
            )
         )
         {
            if
            (
               is_supplicating &&
               (
                  IS_NPC(ch) ||
                  !IS_IMMORTAL(ch)
               )
            )
            {
               switch (number_range(0, 2))
               {
                  default:  /* 0, 1 */
                  {
                     sprintf
                     (
                        buf,
                        "Die, %s, you holy terror!",
                        PERS(ch, victim)
                     );
                     break;
                  }
                  case (2):
                  {
                     sprintf
                     (
                        buf,
                        "Help! %s is helping %s hurt me!",
                        get_god_name(ch, victim),
                        PERS(ch, victim)
                     );
                     break;
                  }
               }
            }
            else
            {
               switch (number_range(0, 2))
               {
                  default:  /* 0, 1 */
                  {
                     sprintf
                     (
                        buf,
                        "Die, %s, you sorcerous dog!",
                        PERS(ch, victim)
                     );
                     break;
                  }
                  case (2):
                  {
                     sprintf
                     (
                        buf,
                        "Help! %s is casting a spell on me!",
                        PERS(ch, victim)
                     );
                     break;
                  }
               }
            }
            if (IS_SET(cast_bits, CAST_BIT_MYELL))
            {
               do_myell(victim, buf);
               sprintf
               (
                  log_buf,
                  "[%s] cast %s upon [%s] at %d",
                  ch->name,
                  skill_table[sn].name,
                  victim->name,
                  ch->in_room->vnum
               );
            }
            else
            {
               sprintf
               (
                  log_buf,
                  "[%s] used %s upon [%s] at %d",
                  ch->name,
                  skill_table[sn].name,
                  victim->name,
                  ch->in_room->vnum
               );
            }
            log_string(log_buf);
         }
      }

   }
   else
   {
      if (IS_SET(cast_bits, CAST_BIT_USE_MANA))
      {
         ch->mana -= mana;
      }
      if
      (
         !IS_IMMORTAL(ch) &&
         !IS_NPC(ch) &&
         ch->in_room &&
         (
            IS_SET(ch->in_room->extra_room_flags, ROOM_1212) ||
            (
               IS_SET(ch->in_room->room_flags, ROOM_NO_MAGIC) &&
               !IS_SET(cast_bits, CAST_BIT_IGNORE_NO_MAGIC)
            )
         )
      )
      {
         if (IS_SET(cast_bits, CAST_BIT_SUPPLICATE))
         {
            act
            (
               "$n fails to act as a conduit for divine power.",
               ch,
               NULL,
               NULL,
               TO_ROOM
            );
            send_to_char
            (
               "You fail to act as a conduit for divine power.\n\r",
               ch
            );
         }
         else
         {
            act
            (
               "$n's spell fizzles.",
               ch,
               NULL,
               NULL,
               TO_ROOM
            );
            send_to_char("Your spell fizzles and dies.\n\r", ch);
         }
         return;
      }

      if
      (
         IS_SET(cast_bits, CAST_BIT_CHECK_INTERRUPT) &&
         eye_of_beholder_nullify(ch)
      )
      {
         return;
      }
      if
      (
         skill_table[sn].target == TAR_CHAR_OFFENSIVE &&
         is_safe(ch, victim, 0)
      )
      {
         return;
      }
      if
      (
         (
            skill_table[sn].target == TAR_CHAR_DEFENSIVE ||
            skill_table[sn].target == TAR_CHAR_HEALING
         ) &&
         is_affected(victim, gsn_timestop)
      )
      {
         if (IS_SET(cast_bits, CAST_BIT_SUPPLICATE))
         {
            act
            (
               "$N is stuck in time and is unaffected by your prayers.",
               ch,
               NULL,
               victim,
               TO_CHAR
            );
            act
            (
               "$N is untouched by $n's prayers.",
               ch,
               NULL,
               victim,
               TO_NOTVICT
            );
            act
            (
               "$n tries to supplicate something on you but you are stuck in"
               " time and unaffected.",
               ch,
               NULL,
               victim,
               TO_VICT
            );
         }
         else
         {
            act
            (
               "$N is stuck in time and is unaffected by your spell.",
               ch,
               NULL,
               victim,
               TO_CHAR
            );
            act
            (
               "$N is untouched by $n's spell.",
               ch,
               NULL,
               victim,
               TO_NOTVICT
            );
            act
            (
               "$n tries to cast something on you but you are stuck in time"
               " and unaffected.",
               ch,
               NULL,
               victim,
               TO_VICT
            );
         }
         return;
      }

      if
      (
         (
            (
               skill_table[sn].target == TAR_CHAR_OFFENSIVE &&
               target == TARGET_CHAR
            ) ||
            (
               skill_table[sn].target == TAR_OBJ_CHAR_OFF &&
               target == TARGET_CHAR
            )
         ) &&
         !IS_NPC(ch) &&
         !IS_NPC(victim) &&
         (
            ch->fighting == NULL ||
            victim->fighting == NULL
         ) &&
         victim != ch
      )
      {
         if
         (
            is_supplicating &&
            (
               IS_NPC(ch) ||
               !IS_IMMORTAL(ch)
            )
         )
         {
            switch (number_range(0, 2))
            {
               default:  /* 0, 1 */
               {
                  sprintf
                  (
                     buf,
                     "Die, %s, you holy terror!",
                     PERS(ch, victim)
                  );
                  break;
               }
               case (2):
               {
                  sprintf
                  (
                     buf,
                     "Help! %s is helping %s hurt me!",
                     get_god_name(ch, victim),
                     PERS(ch, victim)
                  );
                  break;
               }
            }
         }
         else
         {
            switch (number_range(0, 2))
            {
               default:  /* 0, 1 */
               {
                  sprintf
                  (
                     buf,
                     "Die, %s, you sorcerous dog!",
                     PERS(ch, victim)
                  );
                  break;
               }
               case (2):
               {
                  sprintf
                  (
                     buf,
                     "Help! %s is casting a spell on me!",
                     PERS(ch, victim)
                  );
                  break;
               }
            }
         }
         if (IS_SET(cast_bits, CAST_BIT_MYELL))
         {
            do_myell(victim, buf);
            sprintf
            (
               log_buf,
               "[%s] cast %s upon [%s] at %d",
               ch->name,
               skill_table[sn].name,
               victim->name,
               ch->in_room->vnum
            );
         }
         else
         {
            sprintf
            (
               log_buf,
               "[%s] used %s upon [%s] at %d",
               ch->name,
               skill_table[sn].name,
               victim->name,
               ch->in_room->vnum
            );
         }
         log_string(log_buf);
      }

      absorbed = FALSE;
      reflected = FALSE;
      if (target == TARGET_CHAR)
      {
         victim = (CHAR_DATA*) vo;
         if
         (
            !IS_SET(cast_bits, CAST_BIT_PART_OF_SPELL) &&
            ch->in_room == victim->in_room &&
            !IS_SET(cast_bits, CAST_BIT_IGNORE_SPELLBANE) &&
            (
               (
                  !IS_NPC(ch) &&
                  skill_table[sn].spells.spellbane_pc
               ) ||
               (
                  IS_NPC(ch) &&
                  skill_table[sn].spells.spellbane_npc
               )
            ) &&
            spellbaned(ch, victim, sn)
         )
         {
            reflected = TRUE;
         }
         if
         (
            !reflected &&
            !IS_SET(cast_bits, CAST_BIT_IGNORE_CAN_HELP) &&
            clergy_fails_help(ch, victim, sn)
         )
         {
            return;
         }
         if
         (
            !IS_SET(cast_bits, CAST_BIT_IGNORE_ABSORB) &&
            (
               (
                  !IS_NPC(ch) &&
                  skill_table[sn].spells.absorb_pc
               ) ||
               (
                  IS_NPC(ch) &&
                  skill_table[sn].spells.absorb_npc
               )
            ) &&
            check_absorb(ch, victim, sn)
         )
         {
            absorbed = TRUE;
         }
      }
      /* lich check */
      /* class has spells */
      if (IS_SET(cast_bits, CAST_BIT_KEEP_LEVEL))
      {
         llevel = spell_level;
      }
      else
      {
         llevel = GET_LEVEL(ch);
         if (IS_SET(ch->act2, PLR_LICH))
         {
            llevel =
            (
               ch->level +
               (
                  (
                     ch->played + current_time - ch->logon
                  ) / (3600 * 150)
               )
            );
         }
         /* spellbooks */
         if
         (
            (
               obj = get_eq_char(ch, WEAR_HOLD)
            ) != NULL &&
            obj->item_type == ITEM_SPELLBOOK &&
            (
               obj->value[2] == sn ||
               obj->value[3] == sn ||
               obj->value[4] == sn
            ) &&
            llevel < obj->value[1]
         )
         {
            llevel = URANGE(llevel, llevel + obj->value[0], obj->value[1]);
         }
      }


      if
      (
         !IS_SET(cast_bits, CAST_BIT_PART_OF_SPELL) &&
         (
            skill_table[sn].target == TAR_CHAR_OFFENSIVE ||
            (
               skill_table[sn].target == TAR_OBJ_CHAR_OFF &&
               target == TARGET_CHAR
            )
         ) &&
         victim != ch &&
         !absorbed &&
         !reflected &&
         (
            brand = get_eq_char(victim, WEAR_BRAND)
         ) != NULL &&
         brand->pIndexData->vnum == 430 &&
         (
            number_range(1, 10) == 1 ||
            (
               number_range(1, 20) == 1 &&
               (
                  sn == gsn_disintegrate ||
                  sn == gsn_power_word_kill ||
                  sn == gsn_power_word_stun ||
                  sn == gsn_power_word_fear
               )
            )
         )
      )
      {
         bounced = TRUE;
         act
         (
            "$n is surrounded by blue and green sparkles.",
            victim,
            NULL,
            NULL,
            TO_ROOM
         );
         if (IS_SET(cast_bits, CAST_BIT_SUPPLICATE))
         {
            send_to_char
            (
               "You are surrounded by blue and green sparkles, and your"
               " enemy's prayers are reflected!\n\r",
               victim
            );
            send_to_char("Your prayers are reflected back at you!\n\r", ch);
         }
         else
         {
            send_to_char
            (
               "You are surrounded by blue and green sparkles, and your"
               " enemy's spell reflects!\n\r",
               victim
            );
            send_to_char("Your spell is reflected back at you!\n\r", ch);
         }
         (*skill_table[sn].spell_fun)
         (
            sn,
            llevel,
            vo,
            ch,
            target
         );
      }

      if
      (
         !absorbed &&
         !reflected &&
         !bounced
      )
      {
         /* Vrrin's brand can at times prevent spells */
         if
         (
            target == TARGET_CHAR &&
            oblivion_blink(ch, (CHAR_DATA*)vo)
         )
         {
            return;
         }
         if
         (
            !IS_SET(cast_bits, CAST_BIT_SUPPLICATE) &&
            !IS_SET(cast_bits, CAST_BIT_PART_OF_SPELL) &&
            !IS_SET(cast_bits, CAST_BIT_IGNORE_IMPURE)
         )
         {
            check_impure(ch, skill_table[sn].name, IMPURE_CAST);
         }
         (*skill_table[sn].spell_fun) (sn, llevel, ch, vo, target);
      }
      if (IS_SET(cast_bits, CAST_BIT_CHECK_SKILL))
      {
         check_improve(ch, sn, TRUE, 1);
      }
   }

   if
   (
      ch->in_room &&
      IS_SET(cast_bits, CAST_BIT_ATTACK) &&
      (
         skill_table[sn].target == TAR_CHAR_OFFENSIVE ||
         (
            skill_table[sn].target == TAR_OBJ_CHAR_OFF &&
            target == TARGET_CHAR
         )
      ) &&
      victim != ch &&
      (
         bounced ? ch->master != victim : victim->master != ch
      )
   )
   {
      CHAR_DATA* vch;
      CHAR_DATA* vch_next;

      for (vch = ch->in_room->people; vch; vch = vch_next)
      {
         vch_next = vch->next_in_room;
         if (bounced)
         {
            if ( ch == vch && ch->fighting == NULL )
            {
               check_killer(ch, victim);
               multi_hit(ch, victim, TYPE_UNDEFINED);
               break;
            }
         }
         else
         {
            if ( victim == vch && victim->fighting == NULL )
            {
               check_killer(victim, ch);
               multi_hit( victim, ch, TYPE_UNDEFINED );
               break;
            }
         }
      }
   }
   return;
}

void do_cast_spell(CHAR_DATA* ch, char* argument, int cast_bits)
{
   /* Most of this has to do with cowtempl.are's cow suit. */
   int chance;
   OBJ_DATA* cowsuit;

   cowsuit = get_eq_char(ch, WEAR_ABOUT);
   chance = (number_range(1, 10));
   if
   (
      (
         cowsuit != NULL &&
         cowsuit->pIndexData->vnum == 80000
      ) &&
      chance == 1
   )
   {
      send_to_char
      (
         "To your horror, your spell comes out as a bovine moo!\n\r",
         ch
      );
      act_color
      (
         "$n tries to utter a spell but instead says '{B{6Moo.{n'",
         ch,
         NULL,
         NULL,
         TO_ROOM
      );

      return;
   }

   magic_spell(ch, argument, cast_bits, 0, 0);
}

void do_hang( CHAR_DATA *ch, char *argument )
{
   /*
   Initially, it looks like do_cast. We will always need the first few lines in
   do_cast to do this properly.
   */
   char buf[MAX_STRING_LENGTH];
   char message_string[MAX_STRING_LENGTH];
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   OBJ_DATA *obj;
   int mana, sn, compsn, hung, maxhang;
   MESSAGE *hung_message, new_message;

   if ( IS_NPC(ch) && ch->desc == NULL)
   {
      return;
   }

   if (!has_skill(ch, gsn_hang))
   {
      send_to_char("You do not know how to hang spells.\n\r", ch);
      return;
   }
   if (house_down(ch, HOUSE_CONCLAVE))
   {
      return;
   }

   obj = get_eq_char(ch, WEAR_HEAD);
   if
   (
      (
         (
            obj = get_eq_char(ch, WEAR_HEAD)
         ) != NULL &&
         obj->pIndexData->vnum == 27513
      ) ||
      (
         (
            obj = get_eq_char(ch, WEAR_HORNS)
         ) != NULL &&
         obj->pIndexData->vnum == 27513
      )
   )
   {
      send_to_char
      (
         "The Iron Mask squeezes your head tightly!\n\r"
         "You are unable to remember the words to the spell.\n\r",
         ch
      );
      return;
   }

   target_name = one_argument(argument, arg1);
   one_argument(target_name, arg2);

   if ( arg1[0] == '\0')
   {
      send_to_char( "Hang what?\n\r", ch);
      return;
   }

   sn = find_spell(ch, arg1);
   hung = 0;
   if ( (hung_message = get_message_char(ch, MM_HANG1)) != NULL)
   {
      hung = 1;
      strcpy(buf, hung_message->message_string);
      one_argument(buf, arg2);
      compsn = find_spell(ch, arg2);
      if (sn == compsn)
      {
         send_to_char("Oops! You have already hung that spell in the air.\n\r", ch);
         return;
      }
   }
   if ( (hung_message = get_message_char(ch, MM_HANG2))  != NULL)
   {
      hung = 2;
      strcpy(buf, hung_message->message_string);
      one_argument(buf, arg2);
      compsn = find_spell(ch, arg2);
      if (sn == compsn)
      {
         send_to_char("Oops! You have already hung that spell in the air.\n\r", ch);
         return;
      }
   }
   if ( (hung_message = get_message_char(ch, MM_HANG3)) != NULL)
   {
      hung = 3;
      strcpy(buf, hung_message->message_string);
      one_argument(buf, arg2);
      compsn = find_spell(ch, arg2);
      if (sn == compsn)
      {
         send_to_char("Oops! You have already hung that spell in the air.\n\r", ch);
         return;
      }
   }
   if ( (hung_message = get_message_char(ch, MM_HANG4)) != NULL)
   {
      hung = 4;
      strcpy(buf, hung_message->message_string);
      one_argument(buf, arg2);
      compsn = find_spell(ch, arg2);
      if (sn == compsn)
      {
         send_to_char("Oops! You have already hung that spell in the air.\n\r", ch);
         return;
      }
   }

   strcpy(buf, arg1);
   if ( !str_cmp(buf, "NONE") )
   {
      if ( (hung_message = get_message_char(ch, MM_HANG1)) != NULL)  message_remove(hung_message);
      if ( (hung_message = get_message_char(ch, MM_HANG2))  != NULL) message_remove(hung_message);
      if ( (hung_message = get_message_char(ch, MM_HANG3))  != NULL) message_remove(hung_message);
      if ( (hung_message = get_message_char(ch, MM_HANG4)) != NULL)  message_remove(hung_message);
      send_to_char("The spells you hung in the air lose their power and dissipate.\n\r", ch);
      act("$n relaxes as the tension in the air lifts.", ch, NULL, NULL, TO_ROOM);
      REMOVE_BIT(ch->affected_by2, AFF_HANG);
      WAIT_STATE( ch, PULSE_VIOLENCE / 2);
      return;
   }

   if
   (
      IS_NPC(ch) ||
      IS_IMMORTAL(ch) ||
      ch->pcdata->induct >= 5
   )
   {
      maxhang = 4;
   }
   else if (ch->pcdata->induct >= 3)
   {
      maxhang = 3;
   }
   else
   {
      maxhang = 2;
   }
   if (hung >= maxhang)
   {
      send_to_char
      (
         "You have as many spells hung in the air as you can handle.\n\r",
         ch
      );
      return;
   }

   if
   (
      (
         sn = find_spell(ch, arg1)
      ) < 1 ||
      skill_table[sn].spell_fun == spell_null ||
      (
         !IS_NPC(ch) &&
         (
            !has_skill(ch, sn) ||
            ch->pcdata->learned[sn] <= 0
         )
      )
   )
   {
      send_to_char("You do not know any spells of that name.\n\r", ch);
      return;
   }

   if (ch->position < skill_table[sn].minimum_position)
   {
      send_to_char("You cannot concentrate enough.\n\r", ch);
      return;
   }


   if
   (
      (
         is_affected(ch, gsn_chaos_mind) ||
         is_affected(ch, gsn_dancestaves)
      )  &&
      number_percent() < 20
   )
   {
      send_to_char
      (
         "You cannot quite remember the words to that spell.\n\r",
         ch
      );
      act
      (
         "$n gets halfway through a spell, but stops and looks confused.",
         ch,
         NULL,
         NULL,
         TO_ROOM
      );
      WAIT_STATE(ch, 12);
      return;
   }
   mana = mana_cost(ch, skill_table[sn].min_mana, sn);

   /* ----- End (1/2) of copy from do_cast ----- */
   mana /= 4;  /* 25% cost to hang */

   /* ----- Resume (2/2) copy from do_cast after target lookup ----- */

   if ( !IS_NPC(ch) && ch->mana < mana )
   {
      send_to_char( "You don't have enough mana.\n\r", ch );
      return;
   }
   /* ----- End (2/2) copy from do_cast ----- */

   if ( str_cmp( skill_table[sn].name, "ventriloquate" ) )
   say_spell( ch, sn, TRUE );

   /* The rest of this is heavily modified though copied. */

   if
   (
      skill_table[sn].spells.level_reduce &&
      (
         (
            ch->level < LEVEL_HERO &&
            (
               (
                  IS_NPC(ch) &&
                  ch->level < skill_table[sn].skill_level[ch->class] + 15
               ) ||
               (
                  !IS_NPC(ch) &&
                  ch->level < ch->pcdata->learnlvl[sn] + 15
               )
            )
         ) ||
         (
            !IS_NPC(ch) &&
            ch->pcdata->special == SUBCLASS_PRIEST_PROTECTION &&
            (
               sn == gsn_blade_barrier ||
               sn == gsn_demonfire ||
               sn == gsn_holy_fire
            )
         )
      )
   )
   {
      WAIT_STATE(ch, skill_table[sn].beats / 2);
   }
   else
   {
      WAIT_STATE(ch, skill_table[sn].beats / 4);
   }
   /* 25% lag */

   if
   (
      number_percent() > get_skill(ch, sn) ||
      number_percent() > get_skill(ch, gsn_hang)
   )
   {
      send_to_char("You lost your concentration.\n\r", ch);
      check_improve(ch, sn, FALSE, 1);
      check_improve(ch, gsn_hang, FALSE, 1);
      ch->mana -= mana / 2;
      return;
   }
   else
   {
      ch->mana -= mana;
      if (IS_SET(ch->in_room->room_flags, ROOM_NO_MAGIC) && !IS_IMMORTAL(ch))
      {
         if (is_clergy(ch))
         {
            act
            (
               "$n fails to act as a conduit for divine power.",
               ch,
               NULL,
               NULL,
               TO_ROOM
            );
            send_to_char
            (
               "You fail to act as a conduit for divine power.\n\r",
               ch
            );
         }
         else
         {
            act
            (
               "$n's spell fizzles.",
               ch,
               NULL,
               NULL,
               TO_ROOM
            );
            send_to_char("Your spell fizzles and dies.\n\r", ch);
         }
         return;
      }
      if (eye_of_beholder_nullify(ch)) return;
      check_improve(ch, sn, TRUE, 1);
      check_improve(ch, gsn_hang, TRUE, 1);
   }

   /* Okay, mana taken, spell learned from and "cast", time to add it to the pool */
   switch (hung+1)
   {
      case 1: new_message.message_type = MM_HANG1; break;
      case 2: new_message.message_type = MM_HANG2; break;
      case 3: new_message.message_type = MM_HANG3; break;
      case 4: new_message.message_type = MM_HANG4; break;
      default: new_message.message_type = MM_HANG1;
   }
   new_message.message_data = 0;
   new_message.message_flags = MESSAGE_UNIQUE | MESSAGE_REPLACE;
   new_message.message_string = message_string;
   strcpy( message_string, argument );
   message_to_char( ch, &new_message );
   send_to_char("You stop before pronouncing the final glyph, causing the spell to hang in the air.\n\r", ch);
   act("$n moves to cast a spell but does not finish!\n\rThe air is charged with bridled power.", ch, NULL, NULL, TO_ROOM);
   SET_BIT(ch->affected_by2, AFF_HANG);
   return;
}


/* function to return true or false depending on if sn is a house ability
- Wervdon */
bool is_house_spell(CHAR_DATA* ch, int sn)
{
   /* archmages lose spells on align/ethos change per Xyza's request
   if ( (ch->house == HOUSE_ARCANA)
   && ((sn == skill_lookup("familiar"))
   || (sn == skill_lookup("brew"))
   || (sn == skill_lookup("spiritblade"))
   || (sn == skill_lookup("team spirit"))
   || (sn == skill_lookup("channel"))
   || (sn == skill_lookup("venueport"))))
   return TRUE;
   */
   if ( (ch->house == HOUSE_ANCIENT)
   && (
      /*           (sn == skill_lookup("shroud of pro"))
      || (sn == skill_lookup("soulcraft"))
      || (sn == skill_lookup("insight"))
      || (sn == skill_lookup("bloodmist"))
      || (sn == skill_lookup("blessing of darkness"))
      || (sn == skill_lookup("taint")) */
      (sn == gsn_insight)
      || (sn == gsn_shroud_of_pro)
      || (sn == gsn_darkforge)
   ))
   return TRUE;

   /*
   if ( (ch->house == HOUSE_LIFE)
   && ((sn == skill_lookup("prevent"))
   || (sn == skill_lookup("life transfer"))
   || (sn == skill_lookup("robe of the light"))
   || (sn == skill_lookup("safety"))
   || (sn == skill_lookup("sunbolt"))
   || (sn == skill_lookup("lifeline"))))
   return TRUE;
   */

   if ( (ch->house == HOUSE_LIGHT)
   && ((sn == gsn_lifeline)
   || (sn == gsn_sense_evil)
   || (sn == gsn_lightblast)
   || (sn == gsn_armor_of_god)
   || (sn == gsn_guardian_angel)
   || (sn == gsn_lightbind)))
   return TRUE;


   if ( (ch->house == HOUSE_OUTLAW)
   && ((sn == gsn_getaway)
   || (sn == gsn_backup)
   || (sn == gsn_lookout)
   || (sn == gsn_aura_defiance)
   || (sn == gsn_cloak_of_transgression)
   || (sn == gsn_decoy)))
   return TRUE;

   if ( (ch->house == HOUSE_VALOR)
   && ((sn == gsn_health)
   || (sn == gsn_forge)))
   return TRUE;


   if ( (ch->house == HOUSE_COVENANT)
   && ((sn == gsn_devote)
   || (sn == gsn_dark_armor)
   || (sn == gsn_aura_of_presence)
   || (sn == gsn_command)))
   return TRUE;
   return FALSE;
}

bool check_absorb(CHAR_DATA* ch, CHAR_DATA* victim, int dt)
{
   int mana;

   if
   (
      dt < 1 ||
      dt >= MAX_SKILL ||
      !is_affected(victim, gsn_absorb)
   )
   {
      return FALSE;
   }

   mana = mana_cost(ch, skill_table[dt].min_mana, dt);

   if (victim == ch)
   {
      mana /=2;
   }

   if
   (
      (
         UMIN
         (
            (
               (
                  IS_NPC(victim) ?
                  100 :
                  victim->pcdata->learned[gsn_absorb]
               ) -
               (
                  GET_LEVEL(ch) - GET_LEVEL(victim)
               ) * 9
            ),
            95
         ) > number_percent()
      ) ||
      (
         /* Scroll of mana for Players without absorb skill */
         !IS_NPC(victim) &&
         victim->pcdata->learned[gsn_absorb] < 1 &&
         50 > number_percent()
      )
   )
   {
      if (is_supplicating)
      {
         act
         (
            "The air about $N shimmers as it absorbs $n's prayers.",
            ch,
            NULL,
            victim,
            TO_NOTVICT
         );
         act
         (
            "The air about you shimmers as it absorbs $n's prayers.",
            ch,
            NULL,
            victim,
            TO_VICT
         );
         act
         (
            "The air about $N shimmers as it absorbs your prayers.",
            ch,
            NULL,
            victim,
            TO_CHAR
         );
      }
      else
      {
         act
         (
            "The air about $N shimmers as it absorbs $n's spell.",
            ch,
            NULL,
            victim,
            TO_NOTVICT
         );
         act
         (
            "The air about you shimmers as it absorbs $n's spell.",
            ch,
            NULL,
            victim,
            TO_VICT
         );
         act
         (
            "The air about $N shimmers as it absorbs your spell.",
            ch,
            NULL,
            victim,
            TO_CHAR
         );
      }
      victim->mana += mana;
      send_to_char("You feel mana surge through you!\n\r", victim);
      return TRUE;
   }

   return FALSE;
}

void obj_cast_spell( int sn, int level, CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA *obj )
{
   /*
   Used for compatibility with other skills
   uses no cast_bits
   */
   obj_cast_spell_2(sn, level, ch, victim, obj, 0);
}

/*
* Cast spells at targets using a magical object.
*/
void obj_cast_spell_2(int sn, int level, CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA *obj, int cast_bits)
{
   char buf[MAX_STRING_LENGTH];
   void *vo;
   int target = TARGET_NONE;
   bool was_affected = FALSE;

   if (sn == -1)
   {
      /*
         Used as a sentinel for 'no spell' in scrolls/pills/potions, this is not
         an error condition.
      */
      return;
   }
   if
   (
      sn <= 0 ||
      sn >= MAX_SKILL
   )
   {
      bug("obj_cast_spell_2: Invalid sn [%d]", sn);
      return;
   }
   if (ch->in_room == NULL)
   {
      if (IS_NPC(ch))
      {
         sprintf
         (
            log_buf,
            "obj_cast_spell_2: Null room [%s] [%d] [%s]",
            ch->short_descr,
            ch->pIndexData->vnum,
            skill_table[sn].name
         );
      }
      else
      {
         sprintf
         (
            log_buf,
            "obj_cast_spell_2: Null room [%s] [%s]",
            ch->name,
            skill_table[sn].name
         );
      }
      bug_trust(log_buf, 0, get_trust(ch));
      return;
   }

   if
   (
      sn < 1 ||
      sn >= MAX_SKILL ||
      skill_table[sn].spell_fun == NULL
   )
   {
      bug( "Obj_cast_spell: bad sn %d.", sn );
      return;
   }

   /*
   normal magic fizzles in no_magic rooms
   exceptions are IMMORTALS
   and if the casting specifically ignores no magic rooms
   then it will not fizzle in no magic rooms
   */
   if
   (
      IS_SET(ch->in_room->room_flags, ROOM_NO_MAGIC) &&
      !IS_SET(cast_bits, OBJ_CAST_IGNORE_NO_MAGIC)   &&
      !IS_IMMORTAL(ch)
   )
   {
      act("$n's spell fizzles.", ch, NULL, NULL, TO_ROOM);
      send_to_char("Your spell fizzles and dies.\n\r", ch);
      return;
   }


   switch ( skill_table[sn].target )
   {
      default:
      bug( "Obj_cast_spell: bad target for sn %d.", sn );
      return;

      case TAR_IGNORE:
      vo = NULL;
      break;

      case TAR_CHAR_OFFENSIVE:
      if ( victim == NULL )
      victim = ch->fighting;
      if ( victim == NULL )
      {
         send_to_char( "You can't do that.\n\r", ch );
         return;
      }
      if (is_safe(ch, victim, 0) && ch != victim)
      {
         send_to_char("Something isn't right...\n\r", ch);
         return;
      }
      vo = (void *) victim;
      target = TARGET_CHAR;
      break;

      case TAR_CHAR_HEALING:
      case TAR_CHAR_DEFENSIVE:
      case TAR_CHAR_SELF:
      if ( victim == NULL )
      victim = ch;
      vo = (void *) victim;
      target = TARGET_CHAR;
      break;

      case TAR_OBJ_INV:
      case TAR_OBJ:
      if ( obj == NULL )
      {
         send_to_char( "You can't do that.\n\r", ch );
         return;
      }
      vo = (void *) obj;
      target = TARGET_OBJ;
      break;

      case TAR_OBJ_CHAR_OFF:
      if ( victim == NULL && obj == NULL)
      {
         if (ch->fighting != NULL)
         victim = ch->fighting;
         else
         {
            send_to_char("You can't do that.\n\r", ch);
            return;
         }
      }

      if (victim != NULL)
      {
         if (is_safe(ch, victim, 0) && ch != victim)
         {
            send_to_char("Something isn't right...\n\r", ch);
            return;
         }

         vo = (void *) victim;
         target = TARGET_CHAR;
      }
      else
      {
         vo = (void *) obj;
         target = TARGET_OBJ;
      }
      break;


      case TAR_OBJ_CHAR_DEF:
      if (victim == NULL && obj == NULL)
      {
         vo = (void *) ch;
         target = TARGET_CHAR;
      }
      else if (victim != NULL)
      {
         vo = (void *) victim;
         target = TARGET_CHAR;
      }
      else
      {
         vo = (void *) obj;
         target = TARGET_OBJ;
      }

      break;
   }

   if (skill_table[sn].target == TAR_CHAR_OFFENSIVE)
   {
      if (!IS_NPC(ch) && !IS_NPC(victim)
      && (ch->fighting == NULL || victim->fighting == NULL))
      {


         if (IS_SET(cast_bits, OBJ_CAST_HERB)&&!IS_SET(cast_bits, OBJ_CAST_MAGICAL_HERB))
         {
            switch(number_range(0, 2))
            {
               case (0):
               sprintf(buf, "Die, %s you herb using fool!", PERS(ch, victim));
               break;
               case (1):
               case (2):
               sprintf(buf, "Help! %s is using painful herbs on me!", PERS(ch, victim));
               break;
            }
         }
         else
         {
            switch(number_range(0, 2))
            {
               case (0):
               sprintf(buf, "Die, %s, you sorcerous dog!", PERS(ch, victim));
               break;
               case (1):
               case (2):
               sprintf(buf, "Help! %s is using magic on me!", PERS(ch, victim));
               break;
            }
         }
         if (victim != ch)
         {
            do_myell(victim, buf);
            sprintf(log_buf, "[%s] object cast %s upon [%s] %d", ch->name,
            skill_table[sn].name, victim->name, ch->in_room->vnum);
            log_string(log_buf);
         }
      }
   }

   target_name = "";

   /*
   THIS IS FOR HERBS
   If you're affected by the spell version,
   the natural version isn't going to work.
   So if you're affected by it now,
   it will still be the magical version
   after the casting is done
   */
   if (target == TARGET_CHAR)
   {
      was_affected = is_affected(victim, sn);
   }
   /*
      Do not cast it, but alert magic_spell in case this has to cast
      something later.
   */
   magic_spell(NULL, NULL, CAST_BITS_OBJ_CAST_SPELL | cast_bits, level, sn);
   (*skill_table[sn].spell_fun) (sn, level, ch, vo, target);
   /*
   If you weren't affected by this sn before,
   you are affeted by it now,
   and an herb cast it, then you were affected by an
   herb-spell, change the affect to know this.
   */
   if
   (
      target == TARGET_CHAR &&
      !was_affected &&
      is_affected(victim, sn) &&
      IS_SET(cast_bits, OBJ_CAST_HERB)
   )
   {
      /*
      change all instances of affects of this sn
      to be herb spells
      (there were none before we started, so its
      safe to change all of them)
      */
      affect_set_bit2(victim->affected, sn, AFF_HERB_SPELL);
   }



   if ( (skill_table[sn].target == TAR_CHAR_OFFENSIVE
   ||   (skill_table[sn].target == TAR_OBJ_CHAR_OFF && target == TARGET_CHAR))
   &&   victim != ch
   &&   victim->master != ch )
   {
      CHAR_DATA *vch;
      CHAR_DATA *vch_next;

      for ( vch = ch->in_room->people; vch; vch = vch_next )
      {
         vch_next = vch->next_in_room;
         if ( victim == vch && victim->fighting == NULL )
         {
            check_killer(victim, ch);
            multi_hit( victim, ch, TYPE_UNDEFINED );
            break;
         }
      }
   }

   return;
}



/*
* Spell functions.
*/
void spell_absorb( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;

   if (is_affected(victim, gsn_absorb) )
   {
      send_to_char("You are already absorbing spells.\n\r", victim);
      return;
   }

   af.where     = TO_AFFECTS;
   af.type     = gsn_absorb;
   af.level     = level;
   af.duration = (3 + level/5)*race_adjust(ch)/25.0;
   af.modifier = 0;
   af.location = 0;
   af.bitvector = 0;
   affect_to_char(victim, &af);
   send_to_char("A shimmering force field crackles around you.\n\r", victim);
   act("A sizzling force field shimmers into existence around $n.", victim, NULL, NULL, TO_ROOM);
   return;
}

void do_chi_attack(CHAR_DATA *ch, char * argument){
   CHAR_DATA *victim;
   int dam;
   char buf[MAX_STRING_LENGTH];
   char arg[MAX_INPUT_LENGTH];

   one_argument(argument, arg);
   if (IS_NPC(ch))
   return;

   if (!has_skill(ch, gsn_chi_attack)||ch->pcdata->learned[gsn_chi_attack]<0){
      send_to_char("Huh?\n\r", ch);
      return;
   }

   if (ch->mana < 60){
      send_to_char("You dont have the mental energy.\n\r", ch);
      return;
   }
   ch->mana -= 60;

   if (arg[0] == '\0')
   victim = ch->fighting;
   else
   victim = get_char_room(ch, arg);

   if (victim == NULL/* && ch->fighting == NULL*/){
      send_to_char("Focus your chi on who?\n\r", ch);
      return;
   }
   /*
   if (victim == NULL)
   victim = ch->fighting;
   */
   if (number_percent() > get_skill(ch, gsn_chi_attack)){
      send_to_char("You fail to focus your chi.\n\r", ch);
      check_improve(ch, gsn_chi_attack, FALSE, 1);
      return;
   }

   if (oblivion_blink(ch, victim))
   {
      return;
   }

   if (victim->fighting == NULL || ch->fighting == NULL){
      if (!IS_NPC(ch) && !IS_NPC(victim)){
         sprintf(buf, "Help! %s is focusing their chi on me!", PERS(ch, victim));
         do_myell(victim, buf);
         sprintf(log_buf, "%s chi attacked %s at %d", ch->name, victim->name, ch->in_room->vnum);
         log_string(log_buf);
      }
   }
   send_to_char("You focus your chi energy assaulting your foes.\n\r", ch);
   dam = dice(ch->level, 11);
   if (saves_spell(ch, ch->level, victim, DAM_MENTAL, SAVE_SPELL))
   dam = dam/ 2;
   damage(ch, victim, dam, gsn_chi_attack, DAM_MENTAL, TRUE);
   check_improve(ch, gsn_chi_attack, TRUE, 1);
   WAIT_STATE(ch, 24);
   if (victim != NULL && ch != NULL)
   if (ch->fighting == NULL || victim->fighting == NULL)
   multi_hit(victim, ch, TYPE_UNDEFINED);

   return;
}

void spell_acid_blast( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   int dam;

   if (check_spellcraft(ch, sn))
   dam = spellcraft_dam(level, 11);
   else
   dam = dice( level, 11 );

   if ( saves_spell( ch, level, victim, DAM_ACID, SAVE_SPELL ) )
   dam /= 2;
   damage( ch, victim, dam*race_adjust(ch)/25.0, sn, DAM_ACID, TRUE);
   return;
}

void spell_devote(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   AFFECT_DATA af;

   if (house_down(ch, HOUSE_COVENANT))
   return;
   if (is_affected(ch, sn)){
      send_to_char("You are already concentrating on your devotion to the knighthood.\n\r", ch);
      return;
   }
   af.where = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.duration  = 12;
   af.modifier  = -level;
   af.location  = APPLY_AC;
   af.bitvector = 0;
   affect_to_char( ch, &af );
   af.modifier  = level/7;
   af.location  = APPLY_DAMROLL;
   affect_to_char( ch, &af);
   af.location  = APPLY_HITROLL;
   affect_to_char( ch, &af);
   af.modifier  = level*5;
   af.location  = APPLY_HIT;
   ch->hit += level*5;
   affect_to_char( ch, &af);
   send_to_char( "You concentrate on your devotion to the knighthood and feel stronger.\n\r", ch );
   return;
}

void spell_armor(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   CHAR_DATA* victim = (CHAR_DATA*) vo;
   AFFECT_DATA af;

   if (is_affected(victim, sn))
   {
      if (victim == ch)
      {
         send_to_char("You are already armored.\n\r", ch);
      }
      else
      {
         act("$N is already armored.", ch, NULL, victim, TO_CHAR);
      }
      return;
   }
   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.duration  = 24 * race_adjust(ch) / 25.0;
   af.modifier  = -20;
   if
   (
      !IS_NPC(ch) &&
      ch->pcdata->special == SUBCLASS_PRIEST_PROTECTION
   )
   {
      af.modifier = -30;
   }
   af.location  = APPLY_AC;
   af.bitvector = 0;
   affect_to_char(victim, &af);
   send_to_char("You feel someone protecting you.\n\r", victim);
   if (ch != victim)
   {
      if (is_supplicating)
      {
         act("$N is protected by $z.", ch, NULL, victim, TO_CHAR);
      }
      else
      {
         act("$N is protected by your magic.", ch, NULL, victim, TO_CHAR);
      }
   }
   return;
}

/* Awaken - Cirdan
void spell_awaken(...)
{
}
*/

void spell_bless( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim;
   AFFECT_DATA af;
   OBJ_DATA *brand;

   if (target == TARGET_OBJ)
   {
      send_to_char("Uh, what are you thinking?\n\r", ch);
      return;
   }
   /* character target */
   victim = (CHAR_DATA *) vo;


   if (is_affected(victim, sn))
   {
      if (victim == ch)
      send_to_char("You are already blessed.\n\r", ch);
      else
      act("$N already has divine favor.", ch, NULL, victim, TO_CHAR);
      return;
   }

   if (((brand = get_eq_char(ch, WEAR_BRAND)) != NULL)
   && (brand->pIndexData->vnum == 433))
   {
      send_to_char("The gods have forsaken you.\n\r", ch);
      return;
   }

   if (((brand = get_eq_char(victim, WEAR_BRAND)) != NULL)
   && (brand->pIndexData->vnum == 433))
   {
      send_to_char("The gods have forsaken you.\n\r", victim);
      if (victim != ch)
      send_to_char("The gods have forsaken them.\n\r", ch);
      return;
   }

   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.duration  = (6+level)*race_adjust(ch)/25.0;
   af.location  = APPLY_HITROLL;
   if ( level < 8 )
   af.modifier = 1;
   else
   af.modifier  = (level / 8)*race_adjust(ch)/25.0;
   af.bitvector = 0;
   affect_to_char( victim, &af );

   af.location  = APPLY_SAVES;
   if ( level < 8 )
   af.modifier = -1;
   else
   af.modifier  = (0 - level / 8)*race_adjust(ch)/25.0;
   affect_to_char( victim, &af );
   send_to_char( "You feel righteous.\n\r", victim );
   if ( ch != victim )
   act("You grant $N the favor of $z.", ch, NULL, victim, TO_CHAR);
   return;
}

void spell_storm(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   AFFECT_DATA af;
   ROOM_AFFECT_DATA raf;

   if (is_affected(ch, sn)){
      send_to_char("You can't call another storm so soon.\n\r", ch);
      return;
   }
   if (is_affected_room(ch->in_room, sn)){
      send_to_char("It's already storming here.\n\r", ch);
      return;
   }
   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.location  = 0;
   af.modifier  = 0;
   af.duration  = 12;
   af.bitvector = 0;
   affect_to_char( ch, &af );
   send_to_char( "You summon a mighty storm!\n\r", ch );
   act("$n summons forth a mighty storm!", ch, NULL, NULL, TO_ROOM);
   raf.where     = TO_ROOM;
   raf.duration = 4;
   raf.type = sn;
   raf.level = level;
   raf.bitvector = 0;
   raf.caster = ch;
   affect_to_room(ch->in_room, &raf);

   return;
}

void spell_flood_room(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   AFFECT_DATA af;
   ROOM_AFFECT_DATA raf;

   if (is_affected(ch, sn)){
      send_to_char("You can't call another flood yet.\n\r", ch);
      return;
   }
   if (ch->in_room->sector_type == SECT_AIR){
      send_to_char("It's impossible to call a flood here.\n\r", ch);
      return;
   }
   if (ch->in_room->sector_type == SECT_WATER_SWIM ||
   ch->in_room->sector_type == SECT_WATER_NOSWIM ||
   ch->in_room->sector_type == SECT_UNDERWATER)
   {
      send_to_char("There is already a flood here.\n\r", ch);
      return;
   }
   if (is_affected_room(ch->in_room, gsn_raging_fire))
   {
      send_to_char("Your flood puts the fire out!\n\r", ch);
      act("$n's flood puts out the raging fire!", ch, NULL, NULL, TO_ROOM);
      affect_strip_room(ch->in_room, gsn_raging_fire);
   }
   if (room_sector_modified(ch->in_room, ch))
   return;
   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.location  = 0;
   af.modifier  = 0;
   af.duration  = 6;
   if (!IS_NPC(ch) && ch->pcdata->special == SUBCLASS_AQUAMANCER)
   af.duration /= 2;
   af.bitvector = 0;
   affect_to_char( ch, &af );
   send_to_char( "You raise your arms and water springs forth from the ground!\n\r", ch );
   act("$n raises $s arms and water begins flowing from the ground!", ch, NULL, NULL, TO_ROOM);
   act("The room is flooded!", ch, NULL, NULL, TO_ALL);
   raf.where     = TO_ROOM_SECTOR;
   raf.sector_new = SECT_WATER_SWIM;
   raf.duration = 5;
   if (!IS_NPC(ch) && ch->pcdata->special == SUBCLASS_AQUAMANCER)
   raf.duration += 4;
   raf.type = sn;
   raf.level = level;
   if (!IS_NPC(ch))
   if
   (
      str_cmp(target_name, "swim") &&
      (
         IS_IMMORTAL(ch) ||
         (
            has_skill(ch, gsn_water_form) &&
            get_skill(ch, gsn_water_form) >= 75
         )
      )
   )
   {
      raf.sector_new = SECT_WATER_NOSWIM;
   }
   raf.bitvector = 0;
   raf.caster = ch;
   affect_to_room(ch->in_room, &raf);

   return;
}

void spell_blindness( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;
   int dur;
   OBJ_DATA *obj;

   if ((obj = get_eq_char(victim, WEAR_BODY)) != NULL
   && obj->pIndexData->vnum == 14005)
   level -= 20;

   if (!IS_NPC(ch) && ch->pcdata->special == SUBCLASS_ZEALOT_FAITH)
   level += 5;
   if (IS_AFFECTED(victim, AFF_BLIND))
   {
      if (victim == ch)
      send_to_char("You are already blind.\n\r", ch);
      else
      act( "$N has already been blinded.", ch, NULL, victim, TO_CHAR );
      return;
   }

   if (saves_spell(ch, level, victim, DAM_OTHER, SAVE_MALEDICT))
   {
      if (victim == ch)
      send_to_char("Your attempt to blind yourself fails.\n\r", ch);
      else
      act( "Your attempt to blind $N fails.", ch, NULL, victim, TO_CHAR );
      return;
   }


   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.location  = APPLY_HITROLL;
   af.modifier  = -4;
   dur = (level/3)*race_adjust(ch)/25.0;
   if (dur > 9)
   dur = 9;
   af.duration  = dur;
   af.bitvector = AFF_BLIND;
   affect_to_char( victim, &af );
   send_to_char( "You are blinded!\n\r", victim );
   act("$n appears to be blinded.", victim, NULL, NULL, TO_ROOM);
   return;
}



void spell_burning_hands(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   static const sh_int dam_each[] =
   {
      0,
      0,  0,  0,  0,    14,    17, 20, 23, 26, 29,
      29, 29, 30, 30,    31,    31, 32, 32, 33, 33,
      34, 34, 35, 35,    36,    36, 37, 37, 38, 38,
      39, 39, 40, 40,    41,    41, 42, 42, 43, 43,
      44, 44, 45, 45,    46,    46, 47, 47, 48, 48,
      49, 49, 50, 50, 51,    51, 52, 52, 53, 53
   };
   int dam;

   level    = UMIN(level, (int)(sizeof(dam_each)/sizeof(dam_each[0]) - 1));
   level    = UMAX(0, level);
   dam         = number_range( dam_each[level] / 2, dam_each[level] * 2 )*race_adjust(ch)/25.0;
   if (dam > 106) dam = 106;
   if ( saves_spell( ch, level, victim, DAM_FIRE, SAVE_SPELL) )
   dam /= 2;
   damage( ch, victim, dam, sn, DAM_FIRE, TRUE);
   return;
}



void spell_call_lightning(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   CHAR_DATA* vch;
   CHAR_DATA* vch_next;
   char buf[MAX_STRING_LENGTH];
   int dam;
   OBJ_DATA* brand;
   bool mael_brand = FALSE;

   if (check_peace(ch))
   {
      return;
   }


   if
   (
      !IS_OUTSIDE(ch) &&
      !is_affected_room(ch->in_room, gsn_storm)
   )
   {
      send_to_char("You must be out of doors.\n\r", ch);
      return;
   }

   if
   (
      weather_info.sky < SKY_RAINING &&
      !is_affected_room(ch->in_room, gsn_storm)
   )
   {
      for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
      {
         if
         (
            (
               brand = get_eq_char(vch, WEAR_BRAND)
            ) != NULL &&
            brand->pIndexData->vnum == 876
         )
         {
            mael_brand = TRUE;
            break;
         }
      }

      if (!mael_brand)
      {
         send_to_char("You need bad weather.\n\r", ch);
         return;
      }
   }

   dam = dice(level / 2, 8);
   if (check_spellcraft(ch, sn))
   {
      dam += spellcraft_dam(level, 3);
   }
   else
   {
      dam += dice(level, 3);
   }

   send_to_char("Lightning from above strikes your foes!\n\r", ch);
   act
   (
      "$n calls lightning from above to strike $s foes!",
      ch,
      NULL,
      NULL,
      TO_ROOM
   );

   for (vch = char_list; vch != NULL; vch = vch_next)
   {
      vch_next = vch->next;
      if
      (
         vch == ch ||
         vch->in_room == NULL ||
         (
            vch->in_room != ch->in_room &&
            vch->in_room->area != ch->in_room->area
         ) ||
         is_same_group(vch, ch) ||
         (
            vch->in_room == ch->in_room &&
            is_safe(ch, vch, IS_SAFE_SILENT | IS_SAFE_AREA_ATTACK)
         )
      )
      {
         continue;
      }
      if (vch->in_room == ch->in_room)
      {
         if (oblivion_blink(ch, vch))
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
            )
         )
         {
            if
            (
               is_supplicating &&
               (
                  IS_NPC(ch) ||
                  !IS_IMMORTAL(ch)
               )
            )
            {
               switch (number_range(0, 2))
               {
                  default:  /* 0, 1 */
                  {
                     sprintf
                     (
                        buf,
                        "Die, %s, you holy terror!",
                        PERS(ch, vch)
                     );
                     break;
                  }
                  case (2):
                  {
                     sprintf
                     (
                        buf,
                        "Help! %s is helping %s hurt me!",
                        get_god_name(ch, vch),
                        PERS(ch, vch)
                     );
                     break;
                  }
               }
            }
            else
            {
               switch (number_range(0, 2))
               {
                  default:  /* 0, 1 */
                  {
                     sprintf
                     (
                        buf,
                        "Die, %s, you sorcerous dog!",
                        PERS(ch, vch)
                     );
                     break;
                  }
                  case (2):
                  {
                     sprintf
                     (
                        buf,
                        "Help! %s is casting a spell on me!",
                        PERS(ch, vch)
                     );
                     break;
                  }
               }
            }
            do_myell(vch, buf);
            sprintf
            (
               log_buf,
               "[%s] cast call lightning upon [%s] at %d",
               ch->name,
               vch->name,
               ch->in_room->vnum
            );
            log_string(log_buf);
         }

         damage
         (
            ch,
            vch,
            saves_spell
            (
               ch,
               level,
               vch,
               DAM_LIGHTNING,
               SAVE_SPELL
            ) ?
            (
               dam * race_adjust(ch) / 25.0
            ) / 2 :
            dam * race_adjust(ch) / 25.0,
            sn,
            DAM_LIGHTNING,
            TRUE
         );
         continue;
      }
      if
      (
         IS_OUTSIDE(vch) &&
         IS_AWAKE(vch)
      )
      {
         send_to_char("Lightning flashes in the sky.\n\r", vch);
      }
   }
   return;
}

/* RT calm spell stops all fighting in the room */

void spell_calm( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *vch;
   int mlevel = 0;
   int count = 0;
   int high_level = 0;
   int chance;
   AFFECT_DATA af;

   /* get sum of all mobile levels in the room */
   for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
   {
      if (vch->position == POS_FIGHTING && vch != ch)
      {
         count++;
         /*    if (IS_NPC(vch))*/
         mlevel += vch->level;
         /*    else
         mlevel += vch->level/2;*/
         high_level = UMAX(high_level, vch->level);
      }
   }

   /* compute chance of stopping combat */
   chance = 4 * level - high_level + 2 * count;

   if (IS_IMMORTAL(ch)) /* always works */
   mlevel = 0;

   if (number_range(0, chance) >= mlevel)  /* hard to stop large fights */
   {
      for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
      {
         if (IS_NPC(vch) && (IS_SET(vch->imm_flags, IMM_MAGIC) ||
         IS_SET(vch->act, ACT_UNDEAD)))
         continue;

         if (IS_IMMORTAL(vch) && !can_see(ch, vch)) continue;

         if (IS_AFFECTED(vch, AFF_CALM) || IS_AFFECTED(vch, AFF_BERSERK)
         ||  is_affected(vch, gsn_frenzy))
         continue;

         if (IS_NPC(vch) && vch->house != 0) {
            continue;
         }
         if (oblivion_blink(ch, vch))
         {
            continue;
         }

         send_to_char("A wave of calmness passes over you.\n\r", vch);

         /*        if (vch->fighting || vch->position == POS_FIGHTING)
         vch->fighting=NULL;*/
         stop_fighting(vch, FALSE);

         if ( IS_AFFECTED( ch, gsn_camp  ) )
         affect_strip( ch, gsn_camp  );

         af.where = TO_AFFECTS;
         af.type = sn;
         af.level = level;
         af.duration = (level/4)*race_adjust(ch)/25.0;
         af.location = APPLY_HITROLL;
         if (IS_NPC(vch)) {
            af.modifier=-5;
         } else {
            af.modifier=-2;
            af.duration=2;
         }
         af.bitvector = AFF_CALM;
         affect_to_char(vch, &af);

         af.location = APPLY_DAMROLL;
         affect_to_char(vch, &af);

      }
   }
}

void spell_cancellation( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   bool found = FALSE;


   if (victim != ch && victim->master != ch)
   {
      send_to_char("You can only cast cancel on followers.\n\r", ch);
      return;
   }

   level += 2;

   level *= race_adjust(ch)/25.0;

   if ((!IS_NPC(ch) && IS_NPC(victim) &&
   !(IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim) ) ||
   (IS_NPC(ch) && !IS_NPC(victim)) )
   {
      send_to_char("You failed, try dispel magic.\n\r", ch);
      return;
   }

   /* unlike dispel magic, the victim gets NO save */

   /* begin running through the spells */

   if (check_dispel(level, victim, gsn_armor))
   found = TRUE;

   if (check_dispel(level, victim, gsn_bless))
   found = TRUE;

   if (check_dispel(level, victim, gsn_blindness))
   {
      found = TRUE;
      act("$n is no longer blinded.", victim, NULL, NULL, TO_ROOM);
   }

   if (check_dispel(level, victim, gsn_grace))
   {
      found = TRUE;
      act("$n no longer enjoys the grace of extra time.", victim, NULL, NULL, TO_ROOM);
   }
   if (check_dispel(level, victim, gsn_ray_of_enfeeblement))
   {
      found = TRUE;
      act("$n no longer looks feeble...", victim, NULL, NULL, TO_ROOM);
   }

   if (check_dispel(level, victim, gsn_calm))
   {
      found = TRUE;
      act("$n no longer looks so peaceful...", victim, NULL, NULL, TO_ROOM);
   }

   if (check_dispel(level, victim, gsn_change_sex))
   {
      found = TRUE;
      act("$n looks more like $mself again.", victim, NULL, NULL, TO_ROOM);
   }

   if
   (
      /* Cannot remove your own charm */
      ch != victim &&
      check_dispel(level, victim, gsn_charm_person)
   )
   {
      found = TRUE;
      act("$n regains $s free will.", victim, NULL, NULL, TO_ROOM);
   }

   if (check_dispel(level, victim, gsn_chill_touch))
   {
      found = TRUE;
      act("$n looks warmer.", victim, NULL, NULL, TO_ROOM);
   }

   if (check_dispel(level, victim, gsn_curse))
   found = TRUE;

   if (check_dispel(level, victim, gsn_detect_evil))
   found = TRUE;

   if (check_dispel(level, victim, gsn_detect_good))
   found = TRUE;

   if (check_dispel(level, victim, gsn_detect_invis))
   found = TRUE;

   if (check_dispel(level, victim, gsn_detect_magic))
   found = TRUE;

   if (check_dispel(level, victim, gsn_faerie_fire))
   {
      act("$n's outline fades.", victim, NULL, NULL, TO_ROOM);
      found = TRUE;
   }

   if (check_dispel(level, victim, gsn_fly))
   {
      act("$n falls to the ground!", victim, NULL, NULL, TO_ROOM);
      found = TRUE;
   }

   if (check_dispel(level, victim, gsn_frenzy))
   {
      act("$n no longer looks so wild.", victim, NULL, NULL, TO_ROOM);;
      found = TRUE;
   }

   if (check_dispel(level, victim, gsn_giant_strength))
   {
      act("$n no longer looks so mighty.", victim, NULL, NULL, TO_ROOM);
      found = TRUE;
   }

   if (check_dispel(level, victim, gsn_haste))
   {
      act("$n is no longer moving so quickly.", victim, NULL, NULL, TO_ROOM);
      found = TRUE;
   }

   if (check_dispel(level, victim, gsn_infravision))
   found = TRUE;

   if (check_dispel(level, victim, gsn_invis))
   {
      act("$n fades into existence.", victim, NULL, NULL, TO_ROOM);
      found = TRUE;
   }

   if (check_dispel(level, victim, gsn_mass_invis))
   {
      act("$n fades into existence.", victim, NULL, NULL, TO_ROOM);
      found = TRUE;
   }

   if (check_dispel(level, victim, gsn_pass_door))
   found = TRUE;

   if (check_dispel(level, victim, gsn_protection_evil))
   found = TRUE;

   if (check_dispel(level, victim, gsn_protection_good))
   found = TRUE;

   if (check_dispel(level, victim, gsn_sanctuary))
   {
      act
      (
         "The white aura around $n's body vanishes.",
         victim,
         NULL,
         NULL,
         TO_ROOM
      );
      found = TRUE;
   }

   if (check_dispel(level, victim, gsn_chromatic_shield))
   {
      act
      (
         "The chromatic aura around $n's body vanishes.",
         victim,
         NULL,
         NULL,
         TO_ROOM
      );
      found = TRUE;
   }

   if (check_dispel(level, victim, gsn_shield))
   {
      act("The shield protecting $n vanishes.", victim, NULL, NULL, TO_ROOM);
      found = TRUE;
   }

   if (check_dispel(level, victim, gsn_sleep))
   found = TRUE;

   if (check_dispel(level, victim, gsn_slow))
   {
      act("$n is no longer moving so slowly.", victim, NULL, NULL, TO_ROOM);
      found = TRUE;
   }

   if (check_dispel(level, victim, gsn_stone_skin))
   {
      act("$n's skin regains its normal texture.", victim, NULL, NULL, TO_ROOM);
      found = TRUE;
   }

   if (check_dispel(level, victim, gsn_weaken))
   {
      act("$n looks stronger.", victim, NULL, NULL, TO_ROOM);
      found = TRUE;
   }

   if (found)
   send_to_char("Your cancellation attempt was a success.\n\r", ch);
   else
   send_to_char("You failed.\n\r", ch);
}

void spell_cause_light( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   damage( ch, (CHAR_DATA *) vo, (dice(1, 8) + level / 3)*race_adjust(ch)/25.0, sn, DAM_HARM, TRUE);
   return;
}



void spell_cause_critical(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   damage( ch, (CHAR_DATA *) vo, (dice(3, 8) + level - 6)*race_adjust(ch)/25.0, sn, DAM_HARM, TRUE);
   return;
}



void spell_cause_serious(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   damage( ch, (CHAR_DATA *) vo, (dice(2, 8) + level / 2)*race_adjust(ch)/25.0, sn, DAM_HARM, TRUE);
   return;
}

void spell_chain_lightning(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   CHAR_DATA* victim = (CHAR_DATA*) vo;
   CHAR_DATA* tmp_vict;
   CHAR_DATA* last_vict;
   CHAR_DATA* next_vict;
   bool found;
   int dam;
   char buf[MAX_INPUT_LENGTH];

   if (check_peace(ch))
   {
      return;
   }

   /* first strike */
   if (is_affected(ch, gsn_grounding))
   {
      send_to_char("The electricity fizzles at your feet.\n\r", ch);
      act
      (
         "A lightning bolt leaps from $n's hand but fizzles into the ground.",
         ch,
         NULL,
         NULL,
         TO_ROOM
      );
      return;
   }

   act
   (
      "A lightning bolt leaps from $n's hand and arcs to $N.",
      ch,
      NULL,
      victim,
      TO_ROOM
   );
   act
   (
      "A lightning bolt leaps from your hand and arcs to $N.",
      ch,
      NULL,
      victim,
      TO_CHAR
   );
   act
   (
      "A lightning bolt leaps from $n's hand and hits you!",
      ch,
      NULL,
      victim,
      TO_VICT
   );

   dam = dice(level, 6) * race_adjust(ch) / 25.0;
   if (saves_spell(ch, level, victim, DAM_LIGHTNING, SAVE_SPELL))
   {
      dam /= 3;
   }
   damage(ch, victim, dam, sn, DAM_LIGHTNING, TRUE);
   last_vict = victim;
   level -= 4;   /* decrement damage */

   /* new targets */
   while (level > 0)
   {
      found = FALSE;
      for
      (
         tmp_vict = ch->in_room->people;
         tmp_vict != NULL;
         tmp_vict = next_vict
      )
      {
         next_vict = tmp_vict->next_in_room;
         if
         (
            /*
            !is_safe_spell(ch, tmp_vict, TRUE) &&
            */
            !is_same_group(ch, tmp_vict) &&
            tmp_vict != last_vict
         )
         {
            if
            (
               IS_IMMORTAL(tmp_vict) &&
               !can_see(ch, tmp_vict) &&
               wizi_to(tmp_vict, ch)
            )
            {
               continue;
            }
            if
            (
               victim == tmp_vict &&
               is_safe(ch, tmp_vict, 0)
            )
            {
               continue;
            }

            if
            (
               victim != tmp_vict &&
               is_safe(ch, tmp_vict, IS_SAFE_SILENT | IS_SAFE_AREA_ATTACK)
            )
            {
               continue;
            }
            if (oblivion_blink(ch, tmp_vict))
            {
               continue;
            }

            found = TRUE;
            last_vict = tmp_vict;
            /*
            This code does nothing because of the group check earlier,
            but it was not approved to delete it completely.
            if (last_vict == ch)
            {
            if (number_percent() >= ch->level)
            {
            act("The bolt arcs back to $n, and it stabilizes!", tmp_vict, NULL, NULL, TO_ROOM);
            act("The bolt arcs back to you and you stabilize its energy!", tmp_vict, NULL, NULL, TO_CHAR);
            level+=7;
            }
            else
            {
            act("The bolt arcs back to $n!", tmp_vict, NULL, NULL, TO_ROOM);
            act("The bolt arcs back to you!", tmp_vict, NULL, NULL, TO_CHAR);
            }
            }
            else
            {
            */
            act
            (
               "The bolt arcs to $n!",
               tmp_vict,
               NULL,
               NULL,
               TO_ROOM
            );
            act
            (
               "The bolt hits you!",
               tmp_vict,
               NULL,
               NULL,
               TO_CHAR
            );
            dam = dice(level, 6);
            if (saves_spell(ch, level, tmp_vict, DAM_LIGHTNING, SAVE_SPELL))
            {
               dam /= 3;
            }
            /* Not safe from the damage, already gave (gods protect) message */
            if
            (
               !IS_NPC(ch) &&
               !IS_NPC(tmp_vict) &&
               (
                  ch->fighting == NULL ||
                  tmp_vict->fighting == NULL
               )
            )
            {
               if
               (
                  is_supplicating &&
                  (
                     IS_NPC(ch) ||
                     !IS_IMMORTAL(ch)
                  )
               )
               {
                  switch (number_range(0, 2))
                  {
                     default:  /* 0, 1 */
                     {
                        sprintf
                        (
                           buf,
                           "Die, %s, you holy terror!",
                           PERS(ch, tmp_vict)
                        );
                        break;
                     }
                     case (2):
                     {
                        sprintf
                        (
                           buf,
                           "Help! %s is helping %s hurt me!",
                           get_god_name(ch, tmp_vict),
                           PERS(ch, tmp_vict)
                        );
                        break;
                     }
                  }
               }
               else
               {
                  switch (number_range(0, 2))
                  {
                     default:  /* 0, 1 */
                     {
                        sprintf
                        (
                           buf,
                           "Die, %s, you sorcerous dog!",
                           PERS(ch, tmp_vict)
                        );
                        break;
                     }
                     case (2):
                     {
                        sprintf
                        (
                           buf,
                           "Help! %s is casting a spell on me!",
                           PERS(ch, tmp_vict)
                        );
                        break;
                     }
                  }
               }
               do_myell(tmp_vict, buf);
            }
            /* Was not safe, so do the damage */
            damage(ch, tmp_vict, dam, sn, DAM_LIGHTNING, TRUE);
            level -= 4;  /* decrement damage */
            /*
            Read above about innefective code, this is the ending bracket
            to close that scope
            }
            */
         }
      }   /* end target searching loop */

      if (!found) /* no target found, hit the caster */
      {
         if (ch == NULL)
         {
            return;
         }

         if (last_vict == ch) /* no double hits */
         {
            act
            (
               "The bolt seems to have fizzled out.",
               ch,
               NULL,
               NULL,
               TO_ROOM
            );
            act
            (
               "The bolt grounds out harmlessly through your body.",
               ch,
               NULL,
               NULL,
               TO_CHAR
            );
            return;
         }

         last_vict = ch;
         act
         (
            "The bolt arcs back to $n!",
            ch,
            NULL,
            NULL,
            TO_ROOM
         );
         send_to_char("The bolt arcs harmlessly back to you.\n\r", ch);
         level -= 4;  /* decrement damage */
         if (ch == NULL)
         {
            return;
         }
      }
      /* now go back and find more targets */
   }
}


void spell_change_sex( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;

   if ( is_affected( victim, sn ))
   {
      if (victim == ch)
      send_to_char("You've already been changed.\n\r", ch);
      else
      act("$N has already had $s(?) sex changed.", ch, NULL, victim, TO_CHAR);
      return;
   }
   if (saves_spell(ch, level, victim, DAM_OTHER, SAVE_MALEDICT))
   return;
   send_to_char( "You feel different.\n\r", victim );
   act("$n doesn't look like $mself anymore...", victim, NULL, NULL, TO_ROOM);
   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.duration  = 2*level/3;
   af.location  = APPLY_SEX;
   do
   {
      af.modifier  = number_range( 0, 2 ) - victim->sex;
   }
   while ( af.modifier == 0 );
   af.bitvector = 0;
   affect_to_char( victim, &af );
   return;
}



void spell_charm_person( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;
   CHAR_DATA *check;
   int count;

   if (check_peace(ch)) return;

   count = 0;
   if (is_safe(ch, victim, 0)) return;

   if (IS_IMMORTAL(victim) && !IS_IMMORTAL(ch))
   {
      send_to_char( "The gods frown at your arrogance.\n\r", ch );
      return;
   }
   /*
   if (is_affected(ch, gsn_charm_timer))
   {
   send_to_char( "You don't feel ready to charm again.\n\r", ch );
   return;
   }
   Timer is going to be on victim now*/

   if (is_affected(victim, gsn_charm_timer))
   {
      act("$E has already resisted your magical charm.", ch, NULL, victim, TO_CHAR);
      /*    send_to_char("They have already resisted your magic charms.\n\r", ch);*/
      return;
   }

   if ( victim == ch )
   {
      send_to_char( "You like yourself even better!\n\r", ch );
      return;
   }

   /* Charm timer - Wervdon */
   af.where = TO_AFFECTS;
   af.level = level;
   af.location = 0;
   af.modifier = 0;
   af.bitvector = 0;
   af.type = gsn_charm_timer;
   af.duration = 3;
   SET_BIT(victim->act, ACT_SENTINEL); /* Added at Werv's request -- Wicket */
   affect_to_char(victim, &af);/* moved to victim -werv */


   /* New saves - Wervdon */
   if (!IS_NPC(victim)){ /*mobiles use normal saves since there int and
      wis is unrealistic */
      if ( IS_AFFECTED(victim, AFF_CHARM)
      ||   IS_SET(victim->imm_flags, IMM_CHARM)
      ||   IS_AFFECTED(ch, AFF_CHARM)
      ||   (IS_NPC(victim) && victim->spec_fun != NULL)
      ||   (IS_NPC(victim) && IS_SET(victim->act, ACT_AGGRESSIVE))
      ||   level < (GET_LEVEL(victim) - 2)
      ||   saves_armor_of_god(ch, level, victim)
      ||   saves_spell(ch, (level + ch->perm_stat[STAT_INT] - victim->perm_stat[STAT_INT]), victim, DAM_CHARM, SAVE_DEATH)
      ||   saves_spell(ch, (level + (ch->perm_stat[STAT_WIS] - victim->perm_stat[STAT_WIS])/2), victim, DAM_CHARM, SAVE_DEATH) )
      return;
   }
   else
   {
      if ( IS_AFFECTED(victim, AFF_CHARM)
      ||   IS_SET(victim->imm_flags, IMM_CHARM)
      ||   IS_AFFECTED(ch, AFF_CHARM)
      ||   (IS_NPC(victim) && victim->spec_fun != NULL)
      ||   (IS_NPC(victim) && IS_SET(victim->act, ACT_AGGRESSIVE))
      ||   level < (GET_LEVEL(victim) - 2)
      ||   (IS_NPC(victim) && victim->pIndexData->mprog_flags)
      ||   saves_spell(ch, level, victim, DAM_CHARM, SAVE_MALEDICT)
      ||   saves_spell(ch, level, victim, DAM_CHARM, SAVE_MALEDICT)
      ||   saves_armor_of_god(ch, level, victim) )
      return;
   }
   for (check = char_list; check != NULL; check = check->next)
   {
      if
      (
         IS_NPC(check) &&
         check->master == ch &&
         IS_AFFECTED(check, AFF_CHARM) &&
         is_affected(check, sn) &&
         !is_affected(check, gsn_seize)
      )
      {
         count++;
      }
   }

   if ((ch->level < 40 && count > 0)
   || (ch->level < 52 && count > 1) || count > 2)
   {
      send_to_char("You already control as many charmies as you can.\n\r", ch);
      return;
   }

   if ( victim->master )
   stop_follower( victim );
   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.duration  = number_fuzzy( level / 4 )*race_adjust(ch)/25.0;
   if (IS_NPC(victim))
   af.duration *= 2;
   af.location  = 0;
   af.modifier  = 0;
   af.bitvector = AFF_CHARM;
   affect_to_char( victim, &af );
   add_follower( victim, ch );
   victim->leader = ch;
   act( "Isn't $n just so nice?", ch, NULL, victim, TO_VICT );
   if ( ch != victim )
   act("$N looks at you with adoring eyes.", ch, NULL, victim, TO_CHAR);
   return;
}

void spell_chill_touch( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   static const sh_int dam_each[] =
   {
      0,
      0,  0,  6,  7,  8,     9, 12, 13, 13, 13,
      14, 14, 14, 15, 15,    15, 16, 16, 16, 17,
      17, 17, 18, 18, 18,    19, 19, 19, 20, 20,
      20, 21, 21, 21, 22,    22, 22, 23, 23, 23,
      24, 24, 24, 25, 25,    25, 26, 26, 26, 27,
      27, 27, 28, 28, 28,    29, 29, 29, 30, 30
   };
   AFFECT_DATA af;
   int dam;

   level = UMAX(0, level)*race_adjust(ch)/25.0;
   level = UMIN(level, (int)(sizeof(dam_each)/sizeof(dam_each[0]) - 1));
   level = UMAX(0, level);
   dam        = number_range( dam_each[level] / 2, dam_each[level] * 2 );
   if (dam > 60) dam = 60;
   dam *= 2;
   if (check_spellcraft(ch, sn))
   dam *= 2;

   if ( !saves_spell( ch, level, victim, DAM_COLD, SAVE_SPELL ) )
   {
      act("$n turns blue and shivers.", victim, NULL, NULL, TO_ROOM);
      af.where     = TO_AFFECTS;
      af.type      = sn;
      af.level     = level;
      af.duration  = 6;
      af.location  = APPLY_STR;
      af.modifier  = -1;
      af.bitvector = 0;
      affect_join( victim, &af, 10 );
   }
   else
   {
      dam /= 2;
   }

   damage( ch, victim, dam, sn, DAM_COLD, TRUE );
   return;
}

void spell_ice_pox( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;
   int dam;

   dam = dice(level, 4);
   if (check_spellcraft(ch, sn))
   dam *= 2;

   if ( !saves_spell( ch, level, victim, DAM_COLD, SAVE_SPELL ) )
   {
      act("$n shivers violently and freezes.", victim, NULL, NULL, TO_ROOM);
      af.where     = TO_AFFECTS;
      af.type      = sn;
      af.level     = level;
      af.duration  = 6;
      af.location  = APPLY_CON;
      af.modifier  = -((ch->level)/5)*race_adjust(ch)/25.0;
      af.bitvector = 0;
      affect_join( victim, &af, 10 );
      magic_spell_vict
      (
         ch,
         victim,
         CAST_BITS_REUSE,
         level,
         gsn_plague
      );
   }
   else
   {
      dam /= 2;
   }

   damage( ch, victim, dam*race_adjust(ch)/25.0, sn, DAM_COLD, TRUE );
   return;
}




void spell_colour_spray( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   static const sh_int dam_each[] =
   {
      0,
      0,  0,  0,  0,  0,     0,  0,  0,  0,  0,
      30, 35, 40, 45, 50,    55, 55, 55, 56, 57,
      58, 58, 59, 60, 61,    61, 62, 63, 64, 64,
      65, 66, 67, 67, 68,    69, 70, 70, 71, 72,
      73, 73, 74, 75, 76,    76, 77, 78, 79, 79,
      80, 80, 81, 81, 82,    82, 83, 83, 84, 84
   };
   int dam;

   level = UMAX(0, level)*race_adjust(ch)/25.0;
   level = UMIN(level, (int)(sizeof(dam_each)/sizeof(dam_each[0]) - 1));
   level = UMAX(0, level);
   dam        = number_range( dam_each[level] / 2,  dam_each[level] * 2 );
   if (dam > 168) dam = 168;
   if ( saves_spell( ch, level, victim, DAM_LIGHT, SAVE_SPELL) )
   dam /= 2;
   else
   {
      magic_spell_vict
      (
         ch,
         victim,
         CAST_BITS_REUSE,
         level / 2,
         gsn_blindness
      );
   }

   damage( ch, victim, dam, sn, DAM_LIGHT, TRUE );
   return;
}


void spell_concatenate(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   int dam;

   act("$n points an arm at $N and releases a blast of pure energy.", ch, NULL, victim, TO_NOTVICT);
   act("$n gestures at you and a blast of white light engulfs you!", ch, NULL, victim, TO_VICT);
   act("You concatenate the magical energies around you and channel them at $N.", ch, NULL, victim, TO_CHAR);


   if (level <= 35)
   {
      if (check_spellcraft(ch, sn))
      dam = spellcraft_dam(level, 11);
      else
      dam = dice(level, 11);
   }
   else if (level <= 40)
   {
      if (check_spellcraft(ch, sn))
      dam = spellcraft_dam(level, 12);
      else
      dam = dice(level, 12);
   }
   else if (level <= 48)
   {
      if (check_spellcraft(ch, sn))
      dam = spellcraft_dam(level, 13);
      else
      dam = dice(level, 13);
   }
   else
   {
      if (check_spellcraft(ch, sn))
      dam = spellcraft_dam(level, 13);
      else
      dam = dice(level, 13);

      dam += number_range(0, level);
   }


   if (IS_NPC(victim))
   {
      if (level <= 35)
      dam += level;
      else if (level <=40)
      dam += (3*level);
      else
      dam += 4*level;
   }

   /*
   if (saves_spell(ch, level, victim, DAM_ENERGY, SAVE_SPELL));
   {
      dam /= 2;
   }
   Note above, the if statement has a semicolon, therefore dam /= 2 is always called.
   Should be replaced with
   if (saves_spell(ch, level, victim, DAM_ENERGY, SAVE_SPELL))
   {
      dam /= 2;
   }
   if we want the save to do anything
   */
   dam /= 2;   /* see above, making how the code works currently clear */


   if (number_bits(2) == 0)
   {
      magic_spell_vict
      (
         ch,
         victim,
         CAST_BITS_REUSE,
         level * 3 / 4,
         gsn_blindness
      );
   }

   damage( ch, victim, dam*race_adjust(ch)/25.0, sn, DAM_ENERGY, TRUE);

   send_to_char("You stop channeling the energy and the beam of light dissipates.\n\r", ch);
   act("$n's beam of light dissipates.", ch, NULL, NULL, TO_ROOM);

   return;

}


void spell_wrath(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   CHAR_DATA* victim = (CHAR_DATA*) vo;
   int dam;
   OBJ_DATA* brand;
   bool is_neutral = FALSE;

   if
   (
      ((brand = get_eq_char(ch, WEAR_BRAND)) != NULL) &&
      (brand->pIndexData->vnum == 433)
   )
   {
      send_to_char("The gods have forsaken you.\n\r", ch);
      return;
   }
   if
   (
      IS_NEUTRAL(victim) &&
      !EVIL_TO(ch, victim)
   )
   {
      is_neutral = TRUE;
   };

   if
   (
      !IS_EVIL(victim) &&
      !EVIL_TO(ch, victim) &&
      !is_neutral
   )
   {
      act
      (
         "$N is unaffected by $n's heavenly wrath.",
         ch,
         NULL,
         victim,
         TO_NOTVICT
      );
      act
      (
         "You are unaffected by $n's heavenly wrath.",
         ch,
         NULL,
         victim,
         TO_VICT
      );
      if (god_lookup(ch) != -1)
      {
         act
         (
            "$z does not enhance your wrath and frowns on your actions.",
            ch,
            NULL,
            NULL,
            TO_CHAR
         );
      }
      else
      {
         send_to_char
         (
            "The Gods do not enhance your wrath and frown on your actions.\n\r",
            ch
         );
      }
      return;
   }

   dam = dice(level, 8);

   if (level <= 25)
   {
      dam += dice(level, 2);
   }
   else if (level <= 30)
   {
      dam += dice(level / 2, 2) + dice(level / 2, 3);
   }
   else if (level < 35)
   {
      dam += dice(level, 3);
   }
   else if (level < 40)
   {
      dam += dice(level / 2, 3) + dice(level / 2, 4);
   }
   else if (level < 46)
   {
      dam += dice(level, 4);
   }
   else
   {
      dam += dice(level, 5);
   }


   if
   (
      saves_spell(ch, level, victim, DAM_HOLY, SAVE_SPELL) ||
      saves_spell(ch, (level + 5), victim, DAM_HOLY, SAVE_SPELL)
   )
   {
      dam /= 2;
   }
   if (is_neutral)
   {
      dam /= 2;
   }

   act
   (
      "You call down the wrath of $z upon $N.",
      ch,
      NULL,
      victim,
      TO_CHAR
   );
   act
   (
      "$n calls down the wrath of $z upon $N.",
      ch,
      NULL,
      victim,
      TO_NOTVICT
   );
   act
   (
      "$n calls down the wrath of $z upon you.",
      ch,
      NULL,
      victim,
      TO_VICT
   );
   if (number_range(0, 3) == 0)
   {
      magic_spell_vict
      (
         ch,
         victim,
         CAST_BITS_REUSE,
         level,
         gsn_curse
      );
   }
   damage(ch, victim, dam * race_adjust(ch) / 25.0, sn, DAM_HOLY, TRUE);
   return;
}

void spell_portal(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   OBJ_DATA *portal;
   OBJ_DATA *portal2;
   CHAR_DATA *victim;
   bool found_ch_room = FALSE;
   bool found_vict_room = FALSE;
   ROOM_INDEX_DATA *pRoomIndex;

   if (house_down(ch, HOUSE_BRETHREN))
   return;

   if
   (
      is_affected(ch, gsn_purity)
   )
   {
      send_to_char("The mark of purity prevents you from opening a portal!\n\r", ch);
      return;
   }
   if
   (
      IS_SET(ch->in_room->extra_room_flags, ROOM_DEAD_ZONE) &&
      !IS_SET(ch->in_room->room_flags, ROOM_CONSECRATED)
   )
   {
      send_to_char("You fail to open a portal here.\n\r", ch);
      return;
   }
   if (target_name[0] == '\0' ||
   !str_cmp(target_name, "close") ||
   !str_cmp(target_name, "collapse") ||
   !str_cmp(target_name, "shut") ||
   !str_cmp(target_name, "dispel"))
   {
      ROOM_INDEX_DATA *room_to;
      bool found;
      OBJ_DATA *pnext;
      found = FALSE;
      room_to = NULL;
      for (portal = ch->in_room->contents;portal !=NULL;
      portal=pnext){
         pnext = portal->next_content;
         if (portal->item_type == ITEM_PORTAL_NEW
         && portal->pIndexData->vnum == OBJ_VNUM_PORTAL_ABYSS){
            room_to = get_room_index(portal->value[0]);
            extract_obj(portal, FALSE);
            found = TRUE;
            send_to_char("You collapse the portal.\n\r", ch);
         }
      }
      if (room_to != NULL)
      {
         for (portal = room_to->contents;portal !=NULL;portal=pnext)
         {
            pnext = portal->next_content;
            if (portal->item_type == ITEM_PORTAL_NEW
            && portal->pIndexData->vnum == OBJ_VNUM_PORTAL_ABYSS)
            extract_obj(portal, FALSE);
         }
      }
      if (!found) send_to_char("There is no portal here to close.\n\r", ch);
      return;
   }

   victim = get_char_world( ch, target_name );

   if (victim==NULL){
      send_to_char("You fail to make the portal.\n\r", ch);
      return;
   }

   if (oblivion_blink(ch, victim))
   {
      return;
   }
   if
   (
      IS_SET(victim->in_room->room_flags, ROOM_NO_BLITZ) ||
      (
         IS_SET(victim->in_room->extra_room_flags, ROOM_DEAD_ZONE) &&
         !IS_SET(victim->in_room->room_flags, ROOM_CONSECRATED)
      )
   )
   {
      send_to_char("You fail to make the portal.\n\r", ch);
      return;
   }

   if ((victim->in_room->vnum >= 1035 && victim->in_room->vnum <= 1050) ||
   (ch->in_room->vnum >= 1035 && ch->in_room->vnum <= 1050))
   {
      send_to_char("You fail to make the portal.\n\r", ch);
      return;
   }

   if (victim->in_room->vnum == ch->in_room->vnum){
      send_to_char("You fail to make the portal.\n\r", ch);
      return;
   }
   if
   (
      !IS_NPC(ch) &&
      !IS_NPC(victim) &&
      is_safe
      (
         ch,
         victim,
         IS_SAFE_SILENT | IS_SAFE_IGNORE_ROOM | IS_SAFE_IGNORE_AFFECTS
      ) &&
      IS_SET(victim->act, PLR_NOSUMMON)
   )
   {
      send_to_char("You fail to make the portal.\n\r", ch);
      return;
   }
   if (is_shrine(victim->in_room))
   {
      send_to_char("You fail to make the portal.\n\r", ch);
      return;
   }

   if ( ch->in_room->guild != 0 ||
   victim->in_room->guild != 0)
   {
      send_to_char("You fail to make the portal.\n\r", ch);
      return;
   }

   if (ch->in_room->house != 0 && ch->in_room->house != ch->house)
   {
      send_to_char("You fail to make the portal.\n\r", ch);
      return;
   }
   if (victim->in_room->house != 0 && victim->in_room->house != ch->house)
   {
      send_to_char("You fail to make the portal.\n\r", ch);
      return;
   }

   if (saves_spell(ch, level, victim, DAM_OTHER, SAVE_TRAVEL)){
      send_to_char("You fail to make the portal.\n\r", ch);
      return;
   }

   if ((IS_NPC(victim)) && ( IS_SET(victim->res_flags, RES_SUMMON) ))
   level -= 5;
   if ((IS_NPC(victim)) && ( IS_SET(victim->vuln_flags, VULN_SUMMON) ))
   level += 5;
   if ((IS_NPC(victim)) && ( IS_SET(victim->imm_flags, IMM_SUMMON) ))
   {
      send_to_char("You fail to make the portal.\n\r", ch);
      return;
   }
   if (IS_NPC(victim) && IS_SET(victim->act, ACT_AGGRESSIVE))
   {
      send_to_char("You fail to make the portal.\n\r", ch);
      return;
   }

   if (IS_SET(ch->in_room->extra_room_flags, ROOM_1212) ||
   IS_SET(victim->in_room->extra_room_flags, ROOM_1212))
   {
      send_to_char("You fail to make the portal.\n\r", ch);
      return;
   }

   if
   (
      number_percent() < MARAUDER_MISINFORM_CHANCE &&
      is_affected(victim, gsn_misinformation)
   )
   {
      if
      (
         (
            pRoomIndex = get_random_room(victim, RANDOM_NORMAL)
         ) == NULL
      )
      {
         bug("NULL room from get_random_room.", 0);
         return;
      }
   }
   else
   {
      pRoomIndex = victim->in_room;
   }


   for (portal = ch->in_room->contents;portal !=NULL;
   portal=portal->next_content){
      if (portal->item_type == ITEM_PORTAL_NEW
      && portal->pIndexData->vnum == OBJ_VNUM_PORTAL_ABYSS){
         portal->timer = 3;
         portal->value[0] = pRoomIndex->vnum;
         portal->value[1] = 24;
         found_ch_room = TRUE;
      }
   }

   for (portal2 = pRoomIndex->contents;portal2 !=NULL;
   portal2=portal2->next_content){
      if (portal2->item_type == ITEM_PORTAL_NEW
      && portal2->pIndexData->vnum == OBJ_VNUM_PORTAL_ABYSS){
         portal2->timer = 3;
         portal2->value[0] = ch->in_room->vnum;
         portal2->value[1] = 24;
         found_vict_room = TRUE;
      }
   }

   if (!found_ch_room)
   {
      portal = create_object( get_obj_index( OBJ_VNUM_PORTAL_ABYSS ), 0 );
      portal->timer = 3;
      portal->value[0] = pRoomIndex->vnum;
      portal->value[1] = 24;
      obj_to_room( portal, ch->in_room );
      add_node(portal, value_1_list);
   }
   if (!found_vict_room)
   {
      portal2 = create_object(get_obj_index(OBJ_VNUM_PORTAL_ABYSS), 0);
      portal2->timer = 3;
      portal2->value[0] = ch->in_room->vnum;
      portal2->value[1] = 24;
      obj_to_room(portal2, pRoomIndex);
      add_node(portal2, value_1_list);
   }

   act( "$n tears a hole in space, creating a fiery portal!",   ch, portal, NULL, TO_ROOM );
   send_to_char("Calling upon infernal powers, you tear a rift through space to another location.\n\r", ch);
   if
   (
      pRoomIndex == victim->in_room
   )
   {
      act( "A dark, fiery rift in space appears next to you!", victim, portal, NULL, TO_ROOM);
      send_to_char("A dark, fiery rift in space appears next to you!\n\r", victim);
   }

   return;
}


void spell_continual_light(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   OBJ_DATA *light;

   if (target_name[0] != '\0')  /* do a glow on some object */
   {
      light = get_obj_carry(ch, target_name, ch);

      if (light == NULL)
      {
         send_to_char("You don't see that here.\n\r", ch);
         return;
      }

      if (IS_OBJ_STAT(light, ITEM_GLOW))
      {
         act("$p is already glowing.", ch, light, NULL, TO_CHAR);
         return;
      }

      SET_BIT(light->extra_flags, ITEM_GLOW);
      act("$p glows with a white light.", ch, light, NULL, TO_ALL);
      return;
   }

   light = create_object( get_obj_index( OBJ_VNUM_LIGHT_BALL ), 0 );
   obj_to_room( light, ch->in_room );
   act( "$n twiddles $s thumbs and $p appears.",   ch, light, NULL, TO_ROOM );
   act( "You twiddle your thumbs and $p appears.", ch, light, NULL, TO_CHAR );
   return;
}

void spell_distort_time(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   AFFECT_DATA af;

   if (is_affected(ch, sn)){
      send_to_char("Time is already distorted around you.\n\r", ch);
      return;
   }

   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.duration  = level/6;
   af.location  = APPLY_NONE;
   af.modifier  = 0;
   af.bitvector = 0;

   if ( !str_cmp (target_name, "faster") )
   {
      affect_to_char( ch, &af );
      send_to_char("Time begins to distort around you.\n\r", ch);
      af.type = gsn_distort_time_faster;
      affect_to_char( ch, &af);
   }
   else if (!str_cmp( target_name, "slower") )
   {
      affect_to_char( ch, &af );
      send_to_char("Time begins to distort around you.\n\r", ch);
      af.type = gsn_distort_time_slower;
      affect_to_char(ch, &af);
   }
   else
   {
      send_to_char("You have to specify if you want time to be faster or slower.\n\r", ch);
   }
   return;
}

void spell_divine_protect(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   AFFECT_DATA af;
   OBJ_DATA *brand;
   int dtype = -1;
   bool nogod = FALSE;

   if (god_lookup(ch) == -1)
   {
      nogod = TRUE;
   }

   if (is_affected(ch, sn))
   {
      if (nogod)
      {
         send_to_char("The gods are already blessing you for your prayers.\n\r", ch);
      }
      else
      {
         act
         (
            "$z is already blessing you for your prayers.",
            ch,
            NULL,
            NULL,
            TO_CHAR
         );
      }
      return;
   }
   if (((brand = get_eq_char(ch, WEAR_BRAND)) != NULL)
   && (brand->pIndexData->vnum == 433))
   {
      send_to_char("The gods have forsaken you.\n\r", ch);
      return;
   }

   if (!str_prefix(target_name, "bash") || !str_prefix(target_name, "blunt"))
   dtype = DAM_BASH;
   if (!str_prefix(target_name, "pierce"))
   dtype = DAM_PIERCE;
   if (!str_prefix(target_name, "slash"))
   dtype = DAM_SLASH;
   if (!str_prefix(target_name, "iron"))
   dtype = DAM_IRON;
   if (!str_prefix(target_name, "fire"))
   dtype = DAM_FIRE;
   if (!str_prefix(target_name, "cold"))
   dtype = DAM_COLD;
   if (!str_prefix(target_name, "lightning"))
   dtype = DAM_LIGHTNING;
   if (!str_prefix(target_name, "acid"))
   dtype = DAM_ACID;
   if (!str_prefix(target_name, "energy"))
   dtype = DAM_ENERGY;
   if (!str_prefix(target_name, "mental"))
   dtype = DAM_MENTAL;
   if (!str_prefix(target_name, "drowning"))
   dtype = DAM_DROWNING;
   if (!str_prefix(target_name, "holy"))
   dtype = DAM_HOLY;
   if (!str_prefix(target_name, "negative"))
   dtype = DAM_NEGATIVE;
   if (dtype == -1)
   {
      if (nogod)
      {
         send_to_char("The gods do not understand your prayers.\n\r", ch);
      }
      else
      {
         act
         (
            "$z does not understand your prayers.",
            ch,
            NULL,
            NULL,
            TO_CHAR
         );
      }
      return;
   }

   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.duration  = 6+(level / 4)*race_adjust(ch)/25.0;
   af.location  = APPLY_NONE;
   af.modifier  = dtype;
   af.bitvector = 0;
   affect_to_char( ch, &af );
   act( "$n prays to $z and is answered.", ch, NULL, NULL, TO_ROOM);
   send_to_char( "Your prayers are answered.\n\r", ch );


   return;
}

void spell_control_weather(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   if ( !str_cmp( target_name, "better" ) )
   weather_info.change += (level/2 + dice( level / 3, 4 ))*race_adjust(ch)/25.0;
   else if ( !str_cmp( target_name, "worse" ) )
   weather_info.change -= (level/2 + dice( level / 3, 4 ))*race_adjust(ch)/25.0;
   else
   send_to_char ("Do you want it to get better or worse?\n\r", ch );
   weather_update(TRUE);
   send_to_char( "You attempt to magically alter the weather.\n\r", ch );
   return;
}


void spell_tainted_food( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   OBJ_DATA *mushroom;

   mushroom = create_object( get_obj_index( OBJ_VNUM_MUSHROOM ), 0 );
   mushroom->value[0] = level/2;
   mushroom->value[1] = 0;
   mushroom->value[3] = 1;
   obj_to_char( mushroom, ch );
   act( "$p suddenly appears.", ch, mushroom, NULL, TO_ROOM );
   act( "$p suddenly appears.", ch, mushroom, NULL, TO_CHAR );
   return;
}

void spell_create_food(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   OBJ_DATA* mushroom;
   sh_int god;
   char godname [MAX_INPUT_LENGTH];
   char clanname[MAX_INPUT_LENGTH];
   sh_int food;
   char* type;
   char buf[MAX_INPUT_LENGTH];
   OBJ_DATA* brand = get_eq_char(ch, WEAR_BRAND);

   if (is_supplicating)
   {
      type = "holy";
   }
   else
   {
      type = "magic";
   }

   food = 0;
   if (!str_cmp(target_name, "list"))
   {
      bool found = FALSE;

      god = god_lookup(ch);
      if (god != -1)
      {
         sprintf(godname, "'%s'", worship_table[god].name);
      }
      else if
      (
         IS_IMMORTAL(ch) &&
         !IS_NPC(ch)
      )
      {
         sprintf(godname, "'%s'", ch->name);
      }
      else
      {
         strcpy(godname, "'none'");
      }
      if
      (
         !IS_NPC(ch) &&
         ch->pcdata->clan != 0
      )
      {
         sprintf(clanname, "'%s'", clan_table[ch->pcdata->clan].who_name);
      }
      else
      {
         strcpy(clanname, "'none'");
      }
      for (food = 0; food_table[food].name != NULL; food++)
      {
         if
         (
            !IS_TRUSTED(ch, IMMORTAL) &&
            (
               (
                  (
                     IS_GOOD(ch) &&
                     !IS_SET(food_table[food].align, ALLOW_GOOD)
                  ) ||
                  (
                     IS_NEUTRAL(ch) &&
                     !IS_SET(food_table[food].align, ALLOW_NEUTRAL)
                  ) ||
                  (
                     IS_EVIL(ch) &&
                     !IS_SET(food_table[food].align, ALLOW_EVIL)
                  )
               ) ||
               GET_LEVEL(ch) < food_table[food].level ||
               (
                  food_table[food].god_limit != NULL &&
                  stristr
                  (
                     food_table[food].god_limit,
                     godname
                  ) == NULL
               ) ||
               (
                  food_table[food].clan_limit != NULL &&
                  stristr
                  (
                     food_table[food].clan_limit,
                     clanname
                  ) == NULL
               ) ||
               (
                  !is_supplicating &&
                  !food_table[food].allow_magic
               ) ||
               (
                  food_table[food].brand_only &&
                  !IS_IMMORTAL(ch) &&
                  brand == NULL
               )
            )
         )
         {
            continue;
         }
         if (!found)
         {
            act
            (
               "$z will grant you the following foods:",
               ch,
               NULL,
               NULL,
               TO_CHAR
            );
         }
         found = TRUE;
         sprintf
         (
            buf,
            food_table[food].short_desc,
            type
         );
         strcat(buf, "\n\r");
         send_to_char(buf, ch);
      }
      if (!found)
      {
         send_to_char("You can create no foods.\n\r", ch);
      }
      return;
   }
   if (target_name[0] != '\0')
   {
      god = god_lookup(ch);
      if (god != -1)
      {
         sprintf(godname, "'%s'", worship_table[god].name);
      }
      else if
      (
         IS_IMMORTAL(ch) &&
         !IS_NPC(ch)
      )
      {
         sprintf(godname, "'%s'", ch->name);
      }
      else
      {
         strcpy(godname, "'none'");
      }
      if
      (
         !IS_NPC(ch) &&
         ch->pcdata->clan != 0
      )
      {
         sprintf(clanname, "'%s'", clan_table[ch->pcdata->clan].who_name);
      }
      else
      {
         strcpy(clanname, "'none'");
      }
      for (food = 0; food_table[food].name != NULL; food++)
      {
         if
         (
            !is_name(target_name, (char*)food_table[food].name) ||
            (
               !IS_TRUSTED(ch, IMMORTAL) &&
               (
                  (
                     (
                        IS_GOOD(ch) &&
                        !IS_SET(food_table[food].align, ALLOW_GOOD)
                     ) ||
                     (
                        IS_NEUTRAL(ch) &&
                        !IS_SET(food_table[food].align, ALLOW_NEUTRAL)
                     ) ||
                     (
                        IS_EVIL(ch) &&
                        !IS_SET(food_table[food].align, ALLOW_EVIL)
                     )
                  ) ||
                  GET_LEVEL(ch) < food_table[food].level ||
                  (
                     food_table[food].god_limit != NULL &&
                     stristr
                     (
                        food_table[food].god_limit,
                        godname
                     ) == NULL
                  ) ||
                  (
                     food_table[food].clan_limit != NULL &&
                     stristr
                     (
                        food_table[food].clan_limit,
                        clanname
                     ) == NULL
                  ) ||
                  (
                     !is_supplicating &&
                     !food_table[food].allow_magic
                  ) ||
                  (
                     food_table[food].brand_only &&
                     !IS_IMMORTAL(ch) &&
                     brand == NULL
                  )
               )
            )
         )
         {
            continue;
         }
         break;
      }
      if (food_table[food].name == NULL)
      {
         send_to_char("Create what form of food?\n\r", ch);
         target_name = "list";  /* Show list of choices */
         /* Reuse, just recalling same thing.  It is fine */
         spell_create_food(sn, level, ch, vo, target);
         return;
      }
   }
   mushroom = create_object(get_obj_index(OBJ_VNUM_MUSHROOM), 0);
   mushroom->value[0] = level / 2;
   mushroom->value[1] = level;
   sprintf
   (
      buf,
      "%s %s",
      food_table[food].name,
      type
   );
   free_string(mushroom->name);
   mushroom->name = str_dup(buf);
   sprintf
   (
      buf,
      food_table[food].short_desc,
      type
   );
   free_string(mushroom->short_descr);
   mushroom->short_descr = str_dup(buf);
   sprintf
   (
      buf,
      food_table[food].long_desc,
      type
   );
   free_string(mushroom->description);
   mushroom->description = str_dup(buf);
   free_string(mushroom->material);
   mushroom->material = str_dup(food_table[food].material);
   ch->mana = UMAX(ch->mana - food_table[food].mana, 0);
   /* Extra mana for some foods */
   act("$p suddenly appears.", ch, mushroom, NULL, TO_ROOM);
   act("$p suddenly appears.", ch, mushroom, NULL, TO_CHAR);
   if (get_carry_weight(ch) + get_obj_weight(mushroom) > can_carry_w(ch))
   {
      send_to_char("You cannot carry that much weight!\n\r", ch);
      obj_to_room(mushroom, ch->in_room);
   }
   else
   {
      obj_to_char(mushroom, ch);
   }
   return;
}

void spell_create_rose( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   OBJ_DATA *rose;
   rose = create_object(get_obj_index(OBJ_VNUM_ROSE), 0);
   act("$n has created a beautiful red rose.", ch, rose, NULL, TO_ROOM);
   send_to_char("You create a beautiful red rose.\n\r", ch);
   obj_to_char(rose, ch);
   return;
}


void spell_create_red_flower( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   OBJ_DATA *scarlet_pimpernel;
   scarlet_pimpernel = create_object(get_obj_index(OBJ_VNUM_SCARLET_PIMPERNEL), 0);
   act("$n has created a beautiful scarlet flower.", ch, scarlet_pimpernel, NULL, TO_ROOM);
   send_to_char("You create a beautiful scarlet flower.\n\r", ch);
   obj_to_char(scarlet_pimpernel, ch);
   return;
}

void spell_create_blue_flower( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   OBJ_DATA *blue_pimpernel;
   blue_pimpernel = create_object(get_obj_index(OBJ_VNUM_BLUE_PIMPERNEL), 0);
   act("$n has created a beautiful blue flower.", ch, blue_pimpernel, NULL, TO_ROOM);
   send_to_char("You create a beautiful blue flower.\n\r", ch);
   obj_to_char(blue_pimpernel, ch);
   return;
}

void spell_tainted_spring(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   OBJ_DATA* spring;


   for (spring = ch->in_room->contents; spring != NULL; spring = spring->next_content)
   {
      if (spring->item_type == ITEM_FOUNTAIN)
      {
         send_to_char("A fountain already flows here.\n\r", ch);
         return;
      }
   }

   spring = create_object(get_obj_index(OBJ_VNUM_SPRING), 0);
   spring->timer = level;
   spring->value[3] = 1;

   if
   (
      ch->in_room->sector_type == SECT_UNDERWATER ||
      ch->in_room->sector_type == SECT_WATER_NOSWIM ||
      ch->in_room->sector_type == SECT_WATER_SWIM
   )
   {
      act
      (
         "$p begins to flow here but is diluted and dispersed into the water.",
         ch,
         spring,
         NULL,
         TO_ROOM
      );
      act
      (
         "$p begins to flow here but is diluted and dispersed into the water.",
         ch,
         spring,
         NULL,
         TO_CHAR
      );
      return;
   }

   obj_to_room(spring, ch->in_room);
   act
   (
      "$p begins to flow here.",
      ch,
      spring,
      NULL,
      TO_ROOM
   );
   act
   (
      "$p begins to flow here.",
      ch,
      spring,
      NULL,
      TO_CHAR
   );
   return;
}

void spell_create_spring(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   OBJ_DATA* spring;
   sh_int god;
   char godname [MAX_INPUT_LENGTH];
   char clanname[MAX_INPUT_LENGTH];
   sh_int liquid;
   sh_int liquid_slot;
   char* type;
   char buf[MAX_INPUT_LENGTH];
   OBJ_DATA* brand = get_eq_char(ch, WEAR_BRAND);

   if (is_supplicating)
   {
      type = "holy";
   }
   else
   {
      type = "magic";
   }

   if (!str_cmp(target_name, "list"))
   {
      bool found = FALSE;

      god = god_lookup(ch);
      if (god != -1)
      {
         sprintf(godname, "'%s'", worship_table[god].name);
      }
      else if
      (
         IS_IMMORTAL(ch) &&
         !IS_NPC(ch)
      )
      {
         sprintf(godname, "'%s'", ch->name);
      }
      else
      {
         strcpy(godname, "'none'");
      }
      if
      (
         !IS_NPC(ch) &&
         ch->pcdata->clan != 0
      )
      {
         sprintf(clanname, "'%s'", clan_table[ch->pcdata->clan].who_name);
      }
      else
      {
         strcpy(clanname, "'none'");
      }
      for (liquid = 0; spring_table[liquid].name != NULL; liquid++)
      {
         if
         (
            !IS_TRUSTED(ch, IMMORTAL) &&
            (
               (
                  (
                     IS_GOOD(ch) &&
                     !IS_SET(spring_table[liquid].align, ALLOW_GOOD)
                  ) ||
                  (
                     IS_NEUTRAL(ch) &&
                     !IS_SET(spring_table[liquid].align, ALLOW_NEUTRAL)
                  ) ||
                  (
                     IS_EVIL(ch) &&
                     !IS_SET(spring_table[liquid].align, ALLOW_EVIL)
                  )
               ) ||
               GET_LEVEL(ch) < spring_table[liquid].level ||
               (
                  spring_table[liquid].god_limit != NULL &&
                  stristr
                  (
                     spring_table[liquid].god_limit,
                     godname
                  ) == NULL
               ) ||
               (
                  spring_table[liquid].clan_limit != NULL &&
                  stristr
                  (
                     spring_table[liquid].clan_limit,
                     clanname
                  ) == NULL
               ) ||
               (
                  !is_supplicating &&
                  !spring_table[liquid].allow_magic
               ) ||
               (
                  spring_table[liquid].brand_only &&
                  !IS_IMMORTAL(ch) &&
                  brand == NULL
               )
            )
         )
         {
            continue;
         }
         if (!found)
         {
            act
            (
               "$z will grant you the following springs:",
               ch,
               NULL,
               NULL,
               TO_CHAR
            );
         }
         found = TRUE;
         sprintf
         (
            buf,
            (
               (
                  is_supplicating ||
                  spring_table[liquid].short_desc_magic == NULL
               ) ?
               spring_table[liquid].short_desc :
               spring_table[liquid].short_desc_magic
            ),
            type
         );
         strcat(buf, "\n\r");
         send_to_char(buf, ch);
      }
      if (!found)
      {
         send_to_char("You can create no springs.\n\r", ch);
      }
      return;
   }

   for
   (
      spring = ch->in_room->contents;
      spring != NULL;
      spring = spring->next_content
   )
   {
      if (spring->item_type == ITEM_FOUNTAIN)
      {
         send_to_char("A fountain already flows here.\n\r", ch);
         return;
      }
   }

   liquid = 0;
   if (target_name[0] != '\0')
   {
      god = god_lookup(ch);
      if (god != -1)
      {
         sprintf(godname, "'%s'", worship_table[god].name);
      }
      else if
      (
         IS_IMMORTAL(ch) &&
         !IS_NPC(ch)
      )
      {
         sprintf(godname, "'%s'", ch->name);
      }
      else
      {
         strcpy(godname, "'none'");
      }
      if
      (
         !IS_NPC(ch) &&
         ch->pcdata->clan != 0
      )
      {
         sprintf(clanname, "'%s'", clan_table[ch->pcdata->clan].who_name);
      }
      else
      {
         strcpy(clanname, "'none'");
      }
      for (liquid = 0; spring_table[liquid].name != NULL; liquid++)
      {
         if
         (
            !is_name(target_name, (char*)spring_table[liquid].name) ||
            (
               !IS_TRUSTED(ch, IMMORTAL) &&
               (
                  (
                     (
                        IS_GOOD(ch) &&
                        !IS_SET(spring_table[liquid].align, ALLOW_GOOD)
                     ) ||
                     (
                        IS_NEUTRAL(ch) &&
                        !IS_SET(spring_table[liquid].align, ALLOW_NEUTRAL)
                     ) ||
                     (
                        IS_EVIL(ch) &&
                        !IS_SET(spring_table[liquid].align, ALLOW_EVIL)
                     )
                  ) ||
                  GET_LEVEL(ch) < spring_table[liquid].level ||
                  (
                     spring_table[liquid].god_limit != NULL &&
                     stristr
                     (
                        spring_table[liquid].god_limit,
                        godname
                     ) == NULL
                  ) ||
                  (
                     spring_table[liquid].clan_limit != NULL &&
                     stristr
                     (
                        spring_table[liquid].clan_limit,
                        clanname
                     ) == NULL
                  ) ||
                  (
                     !is_supplicating &&
                     !spring_table[liquid].allow_magic
                  ) ||
                  (
                     spring_table[liquid].brand_only &&
                     !IS_IMMORTAL(ch) &&
                     brand == NULL
                  )
               )
            )
         )
         {
            continue;
         }
         break;
      }
      if (spring_table[liquid].name == NULL)
      {
         send_to_char("Create what form of spring?\n\r", ch);
         target_name = "list";  /* Show list of choices */
         /* Reuse, just recalling same thing.  It is fine */
         spell_create_spring(sn, level, ch, vo, target);
         return;
      }
   }
   spring = create_object(get_obj_index(OBJ_VNUM_SPRING), 0);
   spring->timer = level;
   if
   (
      (
         liquid_slot = liq_lookup(spring_table[liquid].liq_type)
      ) == 0 &&
      liquid != 0 &&
      str_cmp(spring_table[liquid].liq_type, "water")
   )
   {
      /* buggy liquids.. make water */
      bug("create_spring liquid (%d) does not exist", liquid);
      act
      (
         "$z grants you a different spring.",
         ch,
         NULL,
         NULL,
         TO_CHAR
      );
      liquid = 0;
   }
   strcpy(buf, spring_table[liquid].name);
   if (!is_name(type, buf))
   {
      strcat(buf, " ");
      strcat(buf, type);
   }
   if (!is_name("spring", buf))
   {
      strcat(buf, " spring");
   }
   free_string(spring->name);
   spring->name = str_dup(buf);
   free_string(spring->description);
   free_string(spring->short_descr);
   if (is_supplicating)
   {
      REMOVE_BIT(spring->extra_flags, ITEM_MAGIC);
   }
   if
   (
      is_supplicating ||
      spring_table[liquid].short_desc_magic == NULL
      /* If they are the same desc, don't double */
   )
   {
      spring->short_descr = str_dup(spring_table[liquid].short_desc);
      spring->description = str_dup(spring_table[liquid].long_desc);
   }
   else
   {
      spring->short_descr = str_dup(spring_table[liquid].short_desc_magic);
      spring->description = str_dup(spring_table[liquid].long_desc_magic);
   }
   ch->mana = UMAX(ch->mana - spring_table[liquid].mana, 0);
   /* Extra mana for some liquids */
   /*
   free_string(spring->short_descr);
   free_string(spring->description);
   spring->short_descr = str_dup("a spring of holy water");
   spring->description = str_dup
   (
   "A spring of holy water flows here."
   );
   }
   */
   if
   (
      immrp_blood &&
      liquid == 0
   )
   {
      spring->value[2] = 14;
   }
   else
   {
      spring->value[2] = liquid_slot;
   }
   if
   (
      ch->in_room->sector_type == SECT_UNDERWATER ||
      ch->in_room->sector_type == SECT_WATER_NOSWIM ||
      ch->in_room->sector_type == SECT_WATER_SWIM
   )
   {
      act
      (
         "$p begins to flow here but is diluted and dispersed into the water.",
         ch,
         spring,
         NULL,
         TO_ROOM
      );
      act
      (
         "$p begins to flow here but is diluted and dispersed into the water.",
         ch,
         spring,
         NULL,
         TO_CHAR
      );
      return;
   }
   obj_to_room(spring, ch->in_room);
   act
   (
      "$p begins to flow here.",
      ch,
      spring,
      NULL,
      TO_ROOM
   );
   act
   (
      "$p begins to flow here.",
      ch,
      spring,
      NULL,
      TO_CHAR
   );
   return;
}



void spell_create_water( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   OBJ_DATA *obj = (OBJ_DATA *) vo;
   int water;

   if ( obj->item_type != ITEM_DRINK_CON )
   {
      send_to_char( "It is unable to hold water.\n\r", ch );
      return;
   }

   if ( obj->value[2] != LIQ_WATER && obj->value[1] != 0 )
   {
      send_to_char( "It contains some other liquid.\n\r", ch );
      return;
   }

   water = UMIN
   (
      level * (weather_info.sky >= SKY_RAINING ? 4 : 2),
      obj->value[0] - obj->value[1]
   );

   if ( water > 0 )
   {
      obj->value[2] = LIQ_WATER;
      if (immrp_blood) obj->value[2] = 14;
      obj->value[1] += water;
      if ( !is_name( "water", obj->name ) )
      {
         char buf[MAX_STRING_LENGTH];

         sprintf( buf, "%s water", obj->name );
         free_string( obj->name );
         obj->name = str_dup( buf );
      }
      act( "$p is filled.", ch, obj, NULL, TO_CHAR );
   }

   return;
}



void spell_cure_blindness(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   OBJ_DATA *brand;

   if (((brand = get_eq_char(victim, WEAR_BRAND)) != NULL)
   && (brand->pIndexData->vnum == 433))
   {
      send_to_char("The gods have forsaken you.\n\r", victim);
      if (victim != ch)
      send_to_char("The gods have forsaken them.\n\r", ch);
      return;
   }

   if (((brand = get_eq_char(ch, WEAR_BRAND)) != NULL)
   && (brand->pIndexData->vnum == 433))
   {
      send_to_char("The gods have forsaken you.\n\r", ch);
      return;
   }

   if ( !is_affected( victim, gsn_blindness )
   && !is_affected(victim, gsn_blindness_dust))
   {
      if (victim == ch)
      send_to_char("You aren't blind.\n\r", ch);
      else
      act("$N doesn't appear to be blinded.", ch, NULL, victim, TO_CHAR);
      return;
   }

   if (is_affected(victim, gsn_blindness_dust))
   {
      if (check_dispel(level, victim, gsn_blindness_dust))
      {
         send_to_char("The dust in your eyes fades away.\n\r", victim);
         act("The dust in $n's eyes fades away.", victim, NULL, NULL, TO_ROOM);
         if (!is_affected(victim, gsn_blindness))
         return;
      }
   }

   if (check_dispel(level, victim, gsn_blindness))
   {
      send_to_char( "Your vision returns!\n\r", victim );
      act("$n is no longer blinded.", victim, NULL, NULL, TO_ROOM);
   }
   else
   send_to_char("You failed.\n\r", ch);
}



void spell_cure_critical(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   CHAR_DATA* victim = (CHAR_DATA*) vo;
   OBJ_DATA* brand;
   int heal;

   if
   (
      (
         (
            brand = get_eq_char(victim, WEAR_BRAND)
         ) != NULL
      ) &&
      brand->pIndexData->vnum == 433
   )
   {
      send_to_char("The gods have forsaken you.\n\r", victim);
      if (victim != ch)
      {
         act("The gods have forsaken $M.", ch, NULL, victim, TO_CHAR);
      }
      return;
   }

   if
   (
      (
         (
            brand = get_eq_char(ch, WEAR_BRAND)
         ) != NULL
      ) &&
      brand->pIndexData->vnum == 433
   )
   {
      send_to_char("The gods have forsaken you.\n\r", ch);
      return;
   }

   heal = (dice(3, 8) + level - 6);
   if (is_affected(victim, gsn_corrupt("smite", &gsn_smite)))
   {
      send_to_char("You feel better, but the wounds do not close.\n\r", victim);
      return;
   }
   if (is_affected(victim, gsn_black_mantle))
   {
      if (is_supplicating)
      {
         send_to_char
         (
            "The black mantle prevents your divine healing.\n\r",
            ch
         );
      }
      else
      {
         send_to_char
         (
            "The black mantle absorbs your healing magics.\n\r",
            ch
         );
      }
      if (ch != victim)
      {
         if (is_supplicating)
         {
            send_to_char
            (
               "The black mantle prevents the divine healing.\n\r",
               victim
            );
         }
         else
         {
            send_to_char
            (
               "The black mantle absorbs the healing magics.\n\r",
               victim
            );
         }
      }
      return;
   }

   if
   (
      is_affected(victim, gsn_ancient_plague) ||
      is_affected(victim, gsn_mantle_oblivion) ||
      IS_SET(victim->act2, PLR_LICH)
   )
   {
      heal /= 2;
   }

   if
   (
      !IS_NPC(ch) &&
      ch->pcdata->special == SUBCLASS_PRIEST_HEALING
   )
   {
      heal += heal / 2;
   }

   victim->hit = UMIN(victim->hit + heal, victim->max_hit);
   update_pos(victim);
   send_to_char("You feel better!\n\r", victim);
   if (ch != victim)
   {
      act
      (
         "You grant them healing from $z.",
         ch,
         NULL,
         NULL,
         TO_CHAR
      );
      evaluate_wounds(ch, victim);
   }
   return;
}

/* RT added to cure plague */
void spell_cure_disease( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   OBJ_DATA *brand;

   if (((brand = get_eq_char(victim, WEAR_BRAND)) != NULL)
   && (brand->pIndexData->vnum == 433))
   {
      send_to_char("The gods have forsaken you.\n\r", victim);
      if (victim != ch)
      send_to_char("The gods have forsaken them.\n\r", ch);
      return;
   }

   if (((brand = get_eq_char(ch, WEAR_BRAND)) != NULL)
   && (brand->pIndexData->vnum == 433))
   {
      send_to_char("The gods have forsaken you.\n\r", ch);
      return;
   }

   if (!str_cmp(class_table[ch->class].name, "monk")
   && victim != ch)
   {
      send_to_char("You are only able to purge your own diseases.\n\r", ch);
      return;
   }

   if ( !is_affected( victim, gsn_plague ) )
   {
      if (victim == ch)
      send_to_char("You aren't ill.\n\r", ch);
      else
      act("$N doesn't appear to be diseased.", ch, NULL, victim, TO_CHAR);
      return;
   }

   if (check_dispel(level, victim, gsn_plague))
   {
      act("$n looks relieved as $s sores vanish.", victim, NULL, NULL, TO_ROOM);
   }
   else
   send_to_char("You failed.\n\r", ch);
}



void spell_cure_light( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   OBJ_DATA *brand;
   int heal;

   if (((brand = get_eq_char(victim, WEAR_BRAND)) != NULL)
   && (brand->pIndexData->vnum == 433))
   {
      send_to_char("The gods have forsaken you.\n\r", victim);
      if (victim != ch)
      send_to_char("The gods have forsaken them.\n\r", ch);
      return;
   }

   if (((brand = get_eq_char(ch, WEAR_BRAND)) != NULL)
   && (brand->pIndexData->vnum == 433))
   {
      send_to_char("The gods have forsaken you.\n\r", ch);
      return;
   }

   heal = (dice(1, 8) + level / 3)*race_adjust(ch)/25.0;
   if (is_affected(victim, gsn_corrupt("smite", &gsn_smite)))
   {
      send_to_char("You feel better, but the wounds do not close.\n\r", victim);
      return;
   }
   if ( is_affected(victim, gsn_black_mantle))
   {
      if (is_supplicating)
      {
         send_to_char
         (
            "The black mantle prevents your divine healing.\n\r",
            ch
         );
      }
      else
      {
         send_to_char
         (
            "The black mantle absorbs your healing magics.\n\r",
            ch
         );
      }
      if (ch != victim)
      {
         if (is_supplicating)
         {
            send_to_char
            (
               "The black mantle prevents the divine healing.\n\r",
               victim
            );
         }
         else
         {
            send_to_char
            (
               "The black mantle absorbs the healing magics.\n\r",
               victim
            );
         }
      }
      return;
   }

   if
   (
      is_affected(victim, gsn_ancient_plague) ||
      is_affected(victim, gsn_mantle_oblivion) ||
      IS_SET(victim->act2, PLR_LICH)
   )
   heal /=2;
   if (!IS_NPC(ch) && ch->pcdata->special == SUBCLASS_PRIEST_HEALING)
   heal = heal + heal/2;
   victim->hit = UMIN( victim->hit + heal, victim->max_hit );
   update_pos( victim );
   send_to_char( "You feel better!\n\r", victim );
   if ( ch != victim )
   {
      act
      (
         "You grant them healing from $z.",
         ch,
         NULL,
         NULL,
         TO_CHAR
      );
      evaluate_wounds(ch, victim);
   }
   return;
}



void spell_cure_poison( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   OBJ_DATA *brand;

   if (((brand = get_eq_char(victim, WEAR_BRAND)) != NULL)
   && (brand->pIndexData->vnum == 433))
   {
      send_to_char("The gods have forsaken you.\n\r", victim);
      if (victim != ch)
      send_to_char("The gods have forsaken them.\n\r", ch);
      return;
   }

   if (((brand = get_eq_char(ch, WEAR_BRAND)) != NULL)
   && (brand->pIndexData->vnum == 433))
   {
      send_to_char("The gods have forsaken you.\n\r", ch);
      return;
   }

   if (!str_cmp(class_table[ch->class].name, "monk")
   && victim != ch)
   {
      send_to_char("You are only able to purge toxins from your own body.\n\r", ch);
      return;
   }

   if ( !is_affected( victim, gsn_poison )
   && !is_affected(victim, gsn_poison_dust))
   {
      if (victim == ch)
      send_to_char("You aren't poisoned.\n\r", ch);
      else
      act("$N doesn't appear to be poisoned.", ch, NULL, victim, TO_CHAR);
      return;
   }

   if (is_affected(victim, gsn_poison_dust))
   {
      if (check_dispel(level, victim, gsn_poison_dust))
      send_to_char("A warm feelings runs through you as the poison dust vanishes.\n\r", victim);
      act("$n looks much better.", victim, NULL, NULL, TO_ROOM);
      if (!is_affected(victim, gsn_poison))
      return;
   }

   if (check_dispel(level, victim, gsn_poison))
   {
      send_to_char("A warm feeling runs through your body.\n\r", victim);
      act("$n looks much better.", victim, NULL, NULL, TO_ROOM);
   }
   else
   send_to_char("You failed.\n\r", ch);
}

void spell_cure_serious( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   OBJ_DATA *brand;
   int heal;

   if (((brand = get_eq_char(victim, WEAR_BRAND)) != NULL)
   && (brand->pIndexData->vnum == 433))
   {
      send_to_char("The gods have forsaken you.\n\r", victim);
      if (victim != ch)
      send_to_char("The gods have forsaken them.\n\r", ch);
      return;
   }

   if (((brand = get_eq_char(ch, WEAR_BRAND)) != NULL)
   && (brand->pIndexData->vnum == 433))
   {
      send_to_char("The gods have forsaken you.\n\r", ch);
      return;
   }

   heal = (dice(2, 8) + level /2)*race_adjust(ch)/25.0;
   if (is_affected(victim, gsn_corrupt("smite", &gsn_smite)))
   {
      send_to_char("You feel better, but the wounds do not close.\n\r", victim);
      return;
   }
   if ( is_affected(victim, gsn_black_mantle))
   {
      if (is_supplicating)
      {
         send_to_char
         (
            "The black mantle prevents your divine healing.\n\r",
            ch
         );
      }
      else
      {
         send_to_char
         (
            "The black mantle absorbs your healing magics.\n\r",
            ch
         );
      }
      if (ch != victim)
      {
         if (is_supplicating)
         {
            send_to_char
            (
               "The black mantle prevents the divine healing.\n\r",
               victim
            );
         }
         else
         {
            send_to_char
            (
               "The black mantle absorbs the healing magics.\n\r",
               victim
            );
         }
      }
      return;
   }

   if
   (
      is_affected(victim, gsn_ancient_plague) ||
      is_affected(victim, gsn_mantle_oblivion) ||
      IS_SET(victim->act2, PLR_LICH)
   )
   heal /=2;

   if (!IS_NPC(ch) && ch->pcdata->special == SUBCLASS_PRIEST_HEALING)
   heal = heal + heal/2;
   victim->hit = UMIN( victim->hit + heal, victim->max_hit );
   update_pos( victim );
   send_to_char( "You feel better!\n\r", victim );
   if ( ch != victim )
   {
      act
      (
         "You grant them healing from $z.",
         ch,
         NULL,
         NULL,
         TO_CHAR
      );
      evaluate_wounds(ch, victim);
   }
   return;
}



void spell_curse(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   CHAR_DATA* victim;
   OBJ_DATA* obj;
   OBJ_DATA* brand;
   AFFECT_DATA af;

   if
   (
      !IS_NPC(ch) &&
      (
         ch->pcdata->special == SUBCLASS_NECROPHILE ||
         ch->pcdata->special == SUBCLASS_CORRUPTOR ||
         ch->pcdata->special == SUBCLASS_ZEALOT_FAITH
      )
   )
   {
      level += 5;
   }

   if (((brand = get_eq_char(ch, WEAR_BRAND)) != NULL)
   && (brand->pIndexData->vnum == 433))
   {
      send_to_char("The gods have forsaken you.\n\r", ch);
      return;
   }

   /* character curses */
   victim = (CHAR_DATA*) vo;
   if (is_safe(ch, victim, 0))
   {
      return;
   }
   if
   (
      (
         obj = get_eq_char(victim, WEAR_BODY)
      ) != NULL &&
      obj->pIndexData->vnum == 14005
   )
   {
      level -= 20;
   }

   if (IS_AFFECTED(victim, AFF_CURSE))
   {
      act("$E has already been cursed.", ch, NULL, victim, TO_CHAR);
      return;
   }
   if (saves_spell(ch, level, victim, DAM_NEGATIVE, SAVE_MALEDICT))
   {
      act("Your attempt at cursing $M has failed.", ch, NULL, victim, TO_CHAR);
      return;
   }
   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.duration  = (2 + level / 5) * race_adjust(ch) / 25.0;
   af.location  = APPLY_HITROLL;
   af.modifier  = -1 * (level / 8);
   af.bitvector = AFF_CURSE;
   affect_to_char(victim, &af);
   af.location  = APPLY_SAVING_SPELL;
   af.modifier  = (level / 8) * race_adjust(ch) / 25.0;
   affect_to_char(victim, &af);

   send_to_char("You feel unclean.\n\r", victim);
   if (ch != victim)
   {
      act("$N looks very uncomfortable.", ch, NULL, victim, TO_CHAR);
   }
   return;
}


void spell_dark_wrath(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   int dam;

   if (!IS_GOOD(victim) )
   {
      act("$N is unaffected by $n's dark wrath.", ch, NULL, victim, TO_NOTVICT);
      act("$N is unaffected by your dark wrath.", ch, NULL, victim, TO_CHAR);
      act("You are unaffected by $n's dark wrath.", ch, NULL, victim, TO_VICT);
      return;
   }

   dam = dice(level, 9)*race_adjust(ch)/25.0;
   if (saves_spell(ch, level, victim, DAM_NEGATIVE, SAVE_SPELL) )
   dam /= 2;
   if (number_range(0, 3) == 0)
   {
      magic_spell_vict
      (
         ch,
         victim,
         CAST_BITS_REUSE,
         level,
         gsn_curse
      );
   }
   damage(ch, victim, dam, sn, DAM_NEGATIVE, TRUE);

   return;
}

/* RT replacement demonfire spell */

void spell_demonfire(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   int dam, dam_mod, chance;

   chance = 100;
   dam_mod = 10;
   if (IS_GOOD(ch))
   {
      dam_mod = 5;
      chance = 0;
   }
   else if (IS_NEUTRAL(ch))
   {
      dam_mod = 7;
      chance = 50;
   }

   dam = dice(level, 7);
   if (victim != ch)
   {
      act("$n calls forth the demons of Hell upon $N!",
      ch, NULL, victim, TO_ROOM);
      act("$n has assailed you with the demons of Hell!",
      ch, NULL, victim, TO_VICT);
      send_to_char("You conjure forth the demons of hell!\n\r", ch);
   }

   if ( saves_spell( ch, level, victim, DAM_NEGATIVE, SAVE_SPELL) )
   dam /= 2;
   magic_spell_vict
   (
      ch,
      victim,
      CAST_BITS_REUSE,
      3 * level / 4,
      gsn_curse
   );
   damage( ch, victim, dam*race_adjust(ch)/25.0, sn, DAM_NEGATIVE , TRUE);
}

void spell_detect_evil( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;

   if ( IS_AFFECTED(victim, AFF_DETECT_EVIL) )
   {
      if (victim == ch)
      send_to_char("You can already sense evil.\n\r", ch);
      else
      act("$N can already detect evil.", ch, NULL, victim, TO_CHAR);
      return;
   }
   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.duration  = level;
   af.modifier  = 0;
   af.location  = APPLY_NONE;
   af.bitvector = AFF_DETECT_EVIL;
   affect_to_char( victim, &af );
   send_to_char( "Your eyes tingle.\n\r", victim );
   if ( ch != victim )
   send_to_char( "You grant them the ability to sense evil.\n\r", ch );
   return;
}


void spell_detect_good( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;

   if ( IS_AFFECTED(victim, AFF_DETECT_GOOD) )
   {
      if (victim == ch)
      send_to_char("You can already sense good.\n\r", ch);
      else
      act("$N can already detect good.", ch, NULL, victim, TO_CHAR);
      return;
   }
   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.duration  = level;
   af.modifier  = 0;
   af.location  = APPLY_NONE;
   af.bitvector = AFF_DETECT_GOOD;
   affect_to_char( victim, &af );
   send_to_char( "Your eyes tingle.\n\r", victim );
   if ( ch != victim )
   send_to_char( "You grant them the ability to sense good.\n\r", ch );
   return;
}



void do_detect_hidden(CHAR_DATA *ch, char *argument)
{
   AFFECT_DATA af;
   int number;

   if ( IS_AFFECTED(ch, AFF_DETECT_HIDDEN) )
   {
      send_to_char("You are already as alert as you can be. \n\r", ch);
      return;
   }

   if ( (number = get_skill(ch, gsn_detect_hidden)) <= 0 ||
   /*ch->level < skill_table[gsn_detect_hidden].skill_level[ch->class] */
   !has_skill(ch, gsn_detect_hidden))
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }
   if ( (number = number_percent()) > get_skill(ch, gsn_detect_hidden) )
   {
      send_to_char("You peer into the shadows but your vision stays the same.\n\r", ch);
      check_improve(ch, gsn_detect_hidden, FALSE, 2);
      return;
   }

   af.where     = TO_AFFECTS;
   af.type      = gsn_detect_hidden;
   af.level     = ch->level;
   af.duration  = ch->level;
   af.location  = APPLY_NONE;
   af.modifier  = 0;
   af.bitvector = AFF_DETECT_HIDDEN;
   affect_to_char( ch, &af );
   send_to_char( "Your awareness improves.\n\r", ch );
   check_improve( ch, gsn_detect_hidden, TRUE, 2 );
   return;
}


void spell_true_sight( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   AFFECT_DATA af;

   if (house_down(ch, HOUSE_ENFORCER))
   return;

   if (is_affected(ch, gsn_true_sight))
   {
      send_to_char("You already see truly.\n\r", ch);
      return;
   }

   af.where = TO_AFFECTS;
   af.type = gsn_true_sight;
   af.location = 0;
   af.level = level;
   af.duration = ch->level/2;
   af.modifier = 0;
   af.bitvector = AFF_DETECT_INVIS;
   affect_to_char(ch, &af);
   af.bitvector = AFF_DETECT_HIDDEN;
   affect_to_char(ch, &af);
   af.bitvector = AFF_DETECT_MAGIC;
   affect_to_char(ch, &af);
   af.bitvector = AFF_INFRARED;
   affect_to_char(ch, &af);
   send_to_char("Your vision sharpens!\n\r", ch);
   return;
}

void spell_detect_invis( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;

   if ( IS_AFFECTED(victim, AFF_DETECT_INVIS) )
   {
      if (victim == ch)
      send_to_char("You can already see invisible.\n\r", ch);
      else
      act("$N can already see invisible things.", ch, NULL, victim, TO_CHAR);
      return;
   }

   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.duration  = level;
   af.modifier  = 0;
   af.location  = APPLY_NONE;
   af.bitvector = AFF_DETECT_INVIS;
   affect_to_char( victim, &af );
   send_to_char( "Your eyes tingle.\n\r", victim );
   if ( ch != victim )
   send_to_char( "You grant them the ability to see invisible.\n\r", ch );
   return;
}



void spell_detect_magic( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;

   if ( IS_AFFECTED(victim, AFF_DETECT_MAGIC) )
   {
      if (victim == ch)
      send_to_char("You can already sense magical auras.\n\r", ch);
      else
      act("$N can already detect magic.", ch, NULL, victim, TO_CHAR);
      return;
   }

   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.duration  = level;
   af.modifier  = 0;
   af.location  = APPLY_NONE;
   af.bitvector = AFF_DETECT_MAGIC;
   affect_to_char( victim, &af );
   send_to_char( "Your eyes tingle.\n\r", victim );
   if ( ch != victim )
   send_to_char( "You grant them the ability to sense magical auras.\n\r", ch );
   return;
}



void spell_detect_poison( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   OBJ_DATA *obj = (OBJ_DATA *) vo;

   if ( obj->item_type == ITEM_DRINK_CON || obj->item_type == ITEM_FOOD )
   {
      if ( obj->value[3] != 0 )
      send_to_char( "You smell poisonous fumes.\n\r", ch );
      else
      send_to_char( "It looks delicious.\n\r", ch );
   }
   else
   {
      send_to_char( "It doesn't look poisoned.\n\r", ch );
   }

   return;
}


void spell_dispel_evil( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   int dam;
   OBJ_DATA *brand;

   if
   (
      (
         brand = get_eq_char(ch, WEAR_BRAND)
      ) != NULL &&
      brand->pIndexData->vnum == 433
   )
   {
      send_to_char( "The gods have forsaken you.\n\r", ch);
      return;
   }

   if ( !IS_NPC(ch) && IS_EVIL(ch) )
   victim = ch;

   if
   (
      IS_GOOD(victim) &&
      !EVIL_TO(ch, victim)
   )
   {
      act( "$N is protected by $S good nature.", ch, NULL, victim, TO_NOTVICT );
      return;
   }

   if
   (
      IS_NEUTRAL(victim) &&
      !EVIL_TO(ch, victim)
   )
   {
      act( "$N does not seem to be affected.", ch, NULL, victim, TO_CHAR );
      return;
   }

   if ( victim->hit > (ch->level * 4) )
   dam = dice( level, 4 );
   else
   dam = UMAX(victim->hit, dice(level, 4));

   if ( saves_spell( ch, level, victim, DAM_HOLY, SAVE_SPELL) )
   dam /= 2;

   damage( ch, victim, dam*race_adjust(ch)/25.0, sn, DAM_HOLY , TRUE);

   return;
}


void spell_dispel_good( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   int dam;
   OBJ_DATA *brand;

   if (((brand = get_eq_char(ch, WEAR_BRAND)) != NULL)
   && (brand->pIndexData->vnum == 433))
   {
      send_to_char("The gods have forsaken you.\n\r", ch);
      return;
   }

   if ( !IS_NPC(ch) && IS_GOOD(ch) )
   victim = ch;

   if ( IS_EVIL(victim) )
   {
      act( "$N is protected by $S evil nature.", ch, NULL, victim, TO_NOTVICT );
      return;
   }

   if ( IS_NEUTRAL(victim) )
   {
      act( "$N does not seem to be affected.", ch, NULL, victim, TO_CHAR );
      return;
   }

   if (victim->hit > (ch->level * 4))
   dam = dice( level, 4 );
   else
   dam = UMAX(victim->hit, dice(level, 4));
   if ( saves_spell( ch, level, victim, DAM_NEGATIVE, SAVE_SPELL) )
   dam /= 2;
   damage( ch, victim, dam*race_adjust(ch)/25.0, sn, DAM_NEGATIVE , TRUE);
   return;
}


/* modified for enhanced use */

void spell_dispel_magic( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   bool found = FALSE;


   if (IS_NPC(victim) && victim->house != 0)
   {
      return;
   }

   if (saves_spell(ch, level, victim, DAM_OTHER, SAVE_SPELL))
   {
      send_to_char( "You feel a brief tingling sensation.\n\r", victim);
      send_to_char( "You failed.\n\r", ch);
      return;
   }

   /* begin running through the spells */

   if (check_dispel(level, victim, gsn_armor))
   found = TRUE;

   if (check_dispel(level, victim, gsn_phasing))
   found = TRUE;

   if (check_dispel(level, victim, gsn_holyshield))
   found = TRUE;

   if (check_dispel(level, victim, gsn_bless))
   found = TRUE;

   if (check_dispel(level, victim, gsn_blindness))
   {
      found = TRUE;
      act("$n is no longer blinded.", victim, NULL, NULL, TO_ROOM);
   }

   if (check_dispel(level, victim, gsn_calm))
   {
      found = TRUE;
      act("$n no longer looks so peaceful...", victim, NULL, NULL, TO_ROOM);
   }

   if (check_dispel(level, victim, gsn_change_sex))
   {
      found = TRUE;
      act("$n looks more like $mself again.", victim, NULL, NULL, TO_ROOM);
   }

   if
   (
      /* Cannot remove your own charm */
      ch != victim &&
      check_dispel(level, victim, gsn_charm_person)
   )
   {
      found = TRUE;
      act("$n regains $s free will.", victim, NULL, NULL, TO_ROOM);
   }

   if (check_dispel(level, victim, gsn_chill_touch))
   {
      found = TRUE;
      act("$n looks warmer.", victim, NULL, NULL, TO_ROOM);
   }

   if (check_dispel(level, victim, gsn_curse))
   found = TRUE;

   if (check_dispel(level, victim, gsn_detect_evil))
   found = TRUE;

   if (check_dispel(level, victim, gsn_detect_good))
   found = TRUE;

   if (check_dispel(level, victim, gsn_detect_hidden))
   found = TRUE;

   if (check_dispel(level, victim, gsn_detect_invis))
   found = TRUE;

   if (check_dispel(level, victim, gsn_detect_magic))
   found = TRUE;

   if (check_dispel(level, victim, gsn_faerie_fire))
   {
      act("$n's outline fades.", victim, NULL, NULL, TO_ROOM);
      found = TRUE;
   }

   if (check_dispel(level, victim, gsn_fly))
   {
      act("$n falls to the ground!", victim, NULL, NULL, TO_ROOM);
      found = TRUE;
   }

   if (check_dispel(level, victim, gsn_frenzy))
   {
      act("$n no longer looks so wild.", victim, NULL, NULL, TO_ROOM);;
      found = TRUE;
   }

   if (check_dispel(level, victim, gsn_giant_strength))
   {
      act("$n no longer looks so mighty.", victim, NULL, NULL, TO_ROOM);
      found = TRUE;
   }

   if (check_dispel(level, victim, gsn_haste))
   {
      act("$n is no longer moving so quickly.", victim, NULL, NULL, TO_ROOM);
      found = TRUE;
   }

   if (check_dispel(level, victim, gsn_infravision))
   found = TRUE;

   if (check_dispel(level, victim, gsn_invis))
   {
      act("$n fades into existence.", victim, NULL, NULL, TO_ROOM);
      found = TRUE;
   }

   if (check_dispel(level, victim, gsn_mass_invis))
   {
      act("$n fades into existence.", victim, NULL, NULL, TO_ROOM);
      found = TRUE;
   }

   if (check_dispel(level, victim, gsn_pass_door))
   found = TRUE;


   if (check_dispel(level, victim, gsn_protection_evil))
   found = TRUE;

   if (check_dispel(level, victim, gsn_protection_good))
   found = TRUE;

   if (check_dispel(level, victim, gsn_sanctuary))
   {
      act
      (
         "The holy aura around $n's body vanishes.",
         victim,
         NULL,
         NULL,
         TO_ROOM
      );
      found = TRUE;
   }
   if (check_dispel(level, victim, gsn_chromatic_shield))
   {
      act
      (
         "The chromatic aura around $n's body vanishes.",
         victim,
         NULL,
         NULL,
         TO_ROOM
      );
      found = TRUE;
   }

   if (check_dispel(level, victim, gsn_shield))
   {
      act("The shield protecting $n vanishes.", victim, NULL, NULL, TO_ROOM);
      found = TRUE;
   }

   if (check_dispel(level, victim, gsn_sleep))
   found = TRUE;

   if (check_dispel(level, victim, gsn_slow))
   {
      act("$n is no longer moving so slowly.", victim, NULL, NULL, TO_ROOM);
      found = TRUE;
   }

   if (check_dispel(level, victim, gsn_stone_skin))
   {
      act("$n's skin regains its normal texture.", victim, NULL, NULL, TO_ROOM);
      found = TRUE;
   }

   if (check_dispel(level, victim, gsn_weaken))
   {
      act("$n looks stronger.", victim, NULL, NULL, TO_ROOM);
      found = TRUE;
   }

   if (found)
   send_to_char("Your dispel attempt worked.\n\r", ch);
   else
   send_to_char("You failed.\n\r", ch);
   return;
}

void spell_earthquake( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;
   char buf[MAX_STRING_LENGTH];
   ROOM_INDEX_DATA* room;
   long id;
   static const sh_int dam_each[] =
   {
      0,
      0,   0,   0,   0,   0,          0,   0,   0,   0,   0,
      0,   0,   0,   0,  30,         35,  40,  45,  50,  55,
      60,  65,  70,  75,  80,         82,  84,  86,  88,  90,
      92,  94,  96,  98, 100,        102, 104, 106, 108, 110,
      112, 114, 116, 118, 120,        122, 124, 126, 128, 130,
      132, 134, 136, 138, 140,    142, 144, 146, 148, 150
   };
   int dam, dam2;
   if (check_peace(ch)) return;

   level       = UMIN(level, (int)(sizeof(dam_each)/sizeof(dam_each[0]) - 1));
   level       = UMAX(0, level);
   dam         = number_range( dam_each[level] / 2, dam_each[level] * 2 );
   if (dam > 300) dam = 300;

   if (check_spellcraft(ch, sn))
   dam += spellcraft_dam(level, 3);
   else
   dam += dice(level, 3);

   dam *= 7.0/10.0;
   if (ch->in_room->sector_type == SECT_WATER_SWIM ||
   ch->in_room->sector_type == SECT_WATER_NOSWIM ||
   ch->in_room->sector_type == SECT_UNDERWATER ||
   ch->in_room->sector_type == SECT_AIR)
   {
      send_to_char("You can't cause an earthquake here.\n\r", ch);
      return;
   }
   send_to_char( "The earth trembles beneath your feet!\n\r", ch );
   act( "$n makes the earth tremble and shiver.", ch, NULL, NULL, TO_ROOM );
   room = ch->in_room;
   id = ch->id;
   for ( vch = char_list; vch != NULL; vch = vch_next )
   {
      vch_next    = vch->next;
      if
      (
         ch == NULL ||
         ch->in_room == NULL ||
         ch->in_room != room ||
         ch->id != id
      )
      {
         continue;
      }
      /*
         this should get us out of the loop safely if
         the ch dies to spellbane -werv
      */
      /*
         (ch == NULL) <- original
         will not do anything if ch dies to spellbane - Fizz
      */
      if (is_same_group(vch, ch) )
      continue;
      if (IS_IMMORTAL(vch) && !can_see(ch, vch)) continue;
      if ( vch->in_room == NULL )
      continue;
      if ( vch->in_room == ch->in_room )
      {
         if ( vch != ch && !is_safe(ch, vch, IS_SAFE_SILENT | IS_SAFE_AREA_ATTACK))
         {
            if (oblivion_blink(ch, vch))
            {
               continue;
            }
            if (!IS_NPC(ch) && !IS_NPC(vch)
            && (ch->fighting == NULL || vch->fighting == NULL))
            {
               if
               (
                  is_supplicating &&
                  (
                     IS_NPC(ch) ||
                     !IS_IMMORTAL(ch)
                  )
               )
               {
                  switch (number_range(0, 2))
                  {
                     default:  /* 0, 1 */
                     {
                        sprintf
                        (
                           buf,
                           "Die, %s, you holy terror!",
                           PERS(ch, vch)
                        );
                        break;
                     }
                     case (2):
                     {
                        sprintf
                        (
                           buf,
                           "Help! %s is helping %s hurt me!",
                           get_god_name(ch, vch),
                           PERS(ch, vch)
                        );
                        break;
                     }
                  }
               }
               else
               {
                  switch (number_range(0, 2))
                  {
                     default:  /* 0, 1 */
                     {
                        sprintf
                        (
                           buf,
                           "Die, %s, you sorcerous dog!",
                           PERS(ch, vch)
                        );
                        break;
                     }
                     case (2):
                     {
                        sprintf
                        (
                           buf,
                           "Help! %s is casting a spell on me!",
                           PERS(ch, vch)
                        );
                        break;
                     }
                  }
               }
               if (vch != ch)
               {
                  do_myell(vch, buf);
                  sprintf(log_buf, "[%s] cast earthquake upon [%s] at %d", ch->name, vch->name, ch->in_room->vnum);
                  log_string(log_buf);
               }
            }
            dam2 = dam;
            if (is_affected(vch, gsn_earthfade))
            dam2 += dam2/3;
            if (IS_FLYING(vch))
            damage(ch, vch, 0, sn, DAM_BASH, TRUE);
            else
            damage( ch, vch, dam2*race_adjust(ch)/25.0, sn, DAM_BASH, TRUE);
         }
         continue;
      }

      if ( vch->in_room->area == ch->in_room->area )
      send_to_char( "The earth trembles and shivers.\n\r", vch );
   }

   return;
}

void spell_hellforge( int sn, int level, CHAR_DATA *ch, void *vo, int target){
   OBJ_DATA *obj = (OBJ_DATA *) vo;
   AFFECT_DATA *paf;
   int chance;
   int dam, hit, obj_dam, obj_hit;
   char new_name[MAX_STRING_LENGTH];

   obj_dam = 0;
   obj_hit = 0;
   if (obj == NULL)
   return;
   if (obj->item_type != ITEM_WEAPON)
   {
      send_to_char("The fires of hell refuse to forge this!\n\r", ch);
      act("$p implodes on itself in a fiery display of evil.", ch, obj, NULL, TO_ROOM);
      act("$p implodes on itself in a fiery display of evil.", ch, obj, NULL, TO_CHAR);
      extract_obj(obj, FALSE);
      return;
   }
   if    ((obj->pIndexData->vnum == 2904)
   || (obj->pIndexData->vnum == 2905)
   || (obj->pIndexData->vnum == 2906)
   || (obj->pIndexData->vnum == 2908)
   || (obj->pIndexData->vnum == 2912)
   || (obj->pIndexData->vnum == 2913)
   || (obj->pIndexData->vnum == 2920)
   || (obj->pIndexData->vnum == 2921)
   || (obj->pIndexData->vnum == 2922)
   || (obj->pIndexData->vnum == 2923)
   || (str_cmp(obj->owner, "none")))
   {
      send_to_char("This just can't be done.\n\r", ch);
      return;
   }

   if (obj->wear_loc != -1)
   {
      send_to_char("You must be carrying it.\n\r", ch);
      return;
   }
   chance = get_skill(ch, sn);
   if (IS_OBJ_STAT(obj, ITEM_BLESS))
   chance -= 75;
   if (IS_OBJ_STAT(obj, ITEM_EVIL))
   chance += 75;
   if (is_affected_obj(obj, sn))
   chance = 0;
   if (obj->level > ch->level)
   chance = chance/2;

   if (number_percent() > chance){
      send_to_char("The fires of hell refuse to forge this!\n\r", ch);
      act("$p explodes on itself in a fiery display of evil.", ch, obj, NULL, TO_ROOM);
      act("$p explodes in a fiery display of evil.", ch, obj, NULL, TO_CHAR);
      extract_obj(obj, FALSE);
      return;
   }
   if ((ch->level + obj->level) > 65){
      act("$p seems to retain some of the fires.", ch, obj, NULL, TO_CHAR);
      SET_BIT(obj->value[4], WEAPON_FLAMING);
   }
   if ((ch->level + obj->level) > 90){
      act("$p begins to glow evilly.", ch, obj, NULL, TO_CHAR);
      SET_BIT(obj->value[4], WEAPON_VAMPIRIC);
   }
   REMOVE_BIT(obj->extra_flags, ITEM_BLESS);
   SET_BIT(obj->extra_flags, ITEM_HUM);
   SET_BIT(obj->extra_flags, ITEM_GLOW);
   SET_BIT(obj->extra_flags, ITEM_MAGIC);
   SET_BIT(obj->extra_flags, ITEM_DARK);
   SET_BIT(obj->extra_flags, ITEM_EVIL);

   /* okay, move all the old flags into new vectors if we have to */
   if (!obj->enchanted)
   {
      AFFECT_DATA *af_new;
      obj->enchanted = TRUE;

      for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
      {
         af_new = new_affect();

         af_new->next = obj->affected;
         obj->affected = af_new;

         af_new->where    = paf->where;
         af_new->type     = UMAX(0, paf->type);
         af_new->level    = paf->level;
         af_new->duration    = paf->duration;
         af_new->location    = paf->location;
         af_new->modifier    = paf->modifier;
         af_new->bitvector    = paf->bitvector;
      }
   }


   for (paf = obj->affected; paf != NULL; paf = paf->next){
      if (paf->location == APPLY_DAMROLL)
      obj_dam += paf->modifier;
      if (paf->location == APPLY_HITROLL)
      obj_hit += paf->modifier;
   }

   hit = ch->level/5;
   dam = ch->level/5;
   if (obj_dam < 8){
      if ((obj_dam + dam)>8)
      dam = 8 - obj_dam;
   }else dam = 0;
   if (obj_hit < 8){
      if ((obj_hit + hit)>8)
      hit = 8 - obj_hit;
   }else hit = 0;

   paf = new_affect();

   obj->enchanted = TRUE;
   paf->where      = TO_OBJECT;
   paf->type       = sn;
   paf->level      = level;
   paf->duration   = -1;
   paf->location   = APPLY_DAMROLL;
   paf->modifier   =  dam;
   paf->bitvector  = 0;
   paf->next       = obj->affected;
   obj->affected   = paf;
   paf = new_affect();

   paf->where      = TO_OBJECT;
   paf->type       = sn;
   paf->level      = level;
   paf->duration   = -1;
   paf->location   = APPLY_HITROLL;
   paf->modifier   =  hit;
   paf->bitvector  = 0;
   paf->next       = obj->affected;
   obj->affected   = paf;
   paf = new_affect();

   paf->where      = TO_OBJECT;
   paf->type       = sn;
   paf->level      = level;
   paf->duration   = -1;
   paf->location   = APPLY_HIT;
   paf->modifier   =  -(ch->level / 2);
   paf->bitvector  = 0;
   paf->next       = obj->affected;
   obj->affected   = paf;

   send_to_char("The fires of hell have reforged it.\n\r", ch);
   act("$p flares in a blinding display of hell fire.", ch, obj, NULL, TO_ROOM);
   act("$p flares in a blinding display of hell fire.", ch, obj, NULL, TO_CHAR);

   sprintf(new_name, "%s", ch->name);
   free_string(obj->owner);
   obj->owner = str_dup(new_name);

   sprintf(new_name, "(hellforged) %s", obj->short_descr);
   free_string( obj->short_descr );
   obj->short_descr = str_dup( new_name );

   return;
}


void spell_enchant_armor( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   OBJ_DATA *obj = (OBJ_DATA *) vo;
   AFFECT_DATA *paf;
   int result, fail;
   int ac_bonus, added;
   bool ac_found = FALSE;
   bool enchanter = FALSE;

   if (!IS_NPC(ch) &&
   ch->pcdata->special == SUBCLASS_ENCHANTER)
   enchanter = TRUE;

   if (obj->item_type != ITEM_ARMOR)
   {
      send_to_char("That isn't an armor.\n\r", ch);
      return;
   }

   if (obj->wear_loc != -1)
   {
      send_to_char("The item must be carried to be enchanted.\n\r", ch);
      return;
   }
   if (is_affected_obj(obj, gsn_tinker))
   {
      send_to_char("The armor seems unaffected by your magics.\n\r", ch);
      return;
   }



   /* this means they have no bonus */
   ac_bonus = 0;
   fail = 25;    /* base 25% chance of failure */

   /* find the bonuses */

   if (!obj->enchanted)
   for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
   {
      if ( paf->location == APPLY_AC )
      {
         ac_bonus = paf->modifier;
         ac_found = TRUE;
         fail += 5 * (ac_bonus * ac_bonus);
      }

      else  /* things get a little harder */
      fail += 20;
   }

   for ( paf = obj->affected; paf != NULL; paf = paf->next )
   {
      if ( paf->location == APPLY_AC )
      {
         ac_bonus = paf->modifier;
         ac_found = TRUE;
         fail += 5 * (ac_bonus * ac_bonus);
      }

      else /* things get a little harder */
      fail += 20;
   }

   /* apply other modifiers */
   fail -= level;

   if (IS_OBJ_STAT(obj, ITEM_BLESS))
   fail -= 15;
   if (IS_OBJ_STAT(obj, ITEM_GLOW))
   fail -= 5;

   if (enchanter) fail /= 2;

   fail = URANGE(5, fail, 85);

   result = number_percent()*race_adjust(ch)/25.0;

   /* the moment of truth */
   if (result < (fail / 5))  /* item destroyed */
   {
      act("$p flares blindingly... and evaporates!", ch, obj, NULL, TO_CHAR);
      act("$p flares blindingly... and evaporates!", ch, obj, NULL, TO_ROOM);
      extract_obj(obj, FALSE);
      return;
   }

   if (result < (fail / 3)) /* item disenchanted */
   {
      AFFECT_DATA *paf_next;

      act("$p glows brightly, then fades...oops.", ch, obj, NULL, TO_CHAR);
      act("$p glows brightly, then fades.", ch, obj, NULL, TO_ROOM);
      obj->enchanted = TRUE;

      /* remove all affects */
      for (paf = obj->affected; paf != NULL; paf = paf_next)
      {
         paf_next = paf->next;
         if (paf->type != gsn_alchemy)
         free_affect(paf);
      }
      obj->affected = NULL;

      REMOVE_BIT(obj->extra_flags, ITEM_GLOW);
      REMOVE_BIT(obj->extra_flags, ITEM_HUM);
      REMOVE_BIT(obj->extra_flags, ITEM_MAGIC);
      REMOVE_BIT(obj->extra_flags, ITEM_INVIS);
      REMOVE_BIT(obj->extra_flags, ITEM_NODROP);
      REMOVE_BIT(obj->extra_flags, ITEM_NOREMOVE);
      return;
   }

   if ( result <= fail )  /* failed, no bad result */
   {
      send_to_char("Nothing seemed to happen.\n\r", ch);
      return;
   }

   /* okay, move all the old flags into new vectors if we have to */
   if (!obj->enchanted)
   {
      AFFECT_DATA *af_new;
      obj->enchanted = TRUE;

      for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
      {
         af_new = new_affect();

         *af_new = *paf;
         af_new->next = obj->affected;
         obj->affected = af_new;
      }
   }

   if (result <= (90 - level/5))  /* success! */
   {
      act("$p shimmers with a gold aura.", ch, obj, NULL, TO_CHAR);
      act("$p shimmers with a gold aura.", ch, obj, NULL, TO_ROOM);
      SET_BIT(obj->extra_flags, ITEM_MAGIC);
      added = -1;
   }

   else  /* exceptional enchant */
   {
      act("$p glows a brilliant gold!", ch, obj, NULL, TO_CHAR);
      act("$p glows a brilliant gold!", ch, obj, NULL, TO_ROOM);
      SET_BIT(obj->extra_flags, ITEM_MAGIC);
      added = -2;
   }

   if ( (result = number_percent()) <= 25 && !IS_SET(obj->extra_flags, ITEM_HUM) )
   {
      SET_BIT(obj->extra_flags, ITEM_HUM);
      REMOVE_BIT(obj->extra_flags, ITEM_GLOW);
   }
   if ( (result = number_percent()) <= 25 && !IS_SET(obj->extra_flags, ITEM_GLOW) )
   SET_BIT(obj->extra_flags, ITEM_GLOW);

   /* now add the enchantments */

   if (obj->level < ch->level)
   obj->level = UMIN(ch->level, obj->level + 1);

   if (ac_found)
   {
      for ( paf = obj->affected; paf != NULL; paf = paf->next)
      {
         if ( paf->location == APPLY_AC)
         {
            paf->type = sn;
            paf->modifier += added;
            paf->level = UMAX(paf->level, level);
            break;
         }
      }
   }
   else /* add a new affect */
   {
      paf = new_affect();

      paf->where    = TO_OBJECT;
      paf->type    = sn;
      paf->level    = level;
      paf->duration    = -1;
      paf->location    = APPLY_AC;
      paf->modifier    =  added;
      paf->bitvector  = 0;
      paf->next    = obj->affected;
      obj->affected    = paf;
   }

}




void spell_enchant_weapon(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   OBJ_DATA *obj = (OBJ_DATA *) vo;
   AFFECT_DATA *paf;
   int result, fail;
   int hit_bonus, dam_bonus, added;
   bool hit_found = FALSE, dam_found = FALSE;
   bool enchanter = FALSE;

   if (obj->pIndexData->vnum == OBJ_VNUM_GENOCIDE_AXE)
   {
      send_to_char("You do not have the power to make the cursed axe submit to you.\n\r", ch);
      return;
   }

   if (!IS_NPC(ch) &&
   ch->pcdata->special == SUBCLASS_ENCHANTER)
   enchanter = TRUE;

   if (obj->item_type != ITEM_WEAPON)
   {
      send_to_char("That isn't a weapon.\n\r", ch);
      return;
   }

   if (obj->wear_loc != -1)
   {
      send_to_char("The item must be carried to be enchanted.\n\r", ch);
      return;
   }

   if (is_affected_obj(obj, gsn_forge_weapon))
   {
      send_to_char("The weapon disappears in a poof of smoke.\n\r", ch);
      extract_obj(obj, FALSE);
      return;
   }
   if (is_affected_obj(obj, gsn_tinker))
   {
      send_to_char("This weapon seems unaffected by your magics.\n\r", ch);
      return;
   }


   if (obj->pIndexData->vnum == OBJ_VNUM_STAR_DAGGER){
      AFFECT_DATA af;
      if (level > 60) level = 60;
      if (obj->enchanted){
         send_to_char("The dagger is reclaimed by the heavens.\n\r", ch);
         extract_obj(obj, FALSE);
         return;
      }
      obj->enchanted = TRUE;
      send_to_char("The dagger is bathed in pure white light emenating from the heavens.\n\r", ch);
      act("$p is bathed in pure white light emenating from the heavens.", ch, obj, NULL, TO_ROOM);

      af.where = TO_OBJECT;
      af.type = sn;
      af.level = level;
      af.duration = -1;
      af.bitvector = 0;
      af.location = APPLY_HITROLL;
      af.modifier = level/6 +2;
      affect_to_obj(obj, &af);
      af.location = APPLY_DAMROLL;
      affect_to_obj(obj, &af);
      obj->level = level;

      /* note: this makes it a freezing bite noun, if anyone ever changes the damage table, it'll be wrong */
      obj->value[3] = 30;
      if (level < 30){
         obj->value[1] = 6;
         obj->value[2] = 7;
         return;
      }
      if (level < 40){
         obj->value[1] = 8;
         obj->value[2] = 6;
         return;
      }
      SET_BIT(obj->value[4], WEAPON_FROST);
      obj->value[1] = 9;
      obj->value[2] = 6;
      return;
   }
   /* this means they have no bonus */
   hit_bonus = 0;
   dam_bonus = 0;
   fail = 25;    /* base 25% chance of failure */

   /* find the bonuses */

   if (!obj->enchanted)
   for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
   {
      if ( paf->location == APPLY_HITROLL )
      {
         hit_bonus = paf->modifier;
         hit_found = TRUE;
         fail += 2 * (hit_bonus * hit_bonus);
      }

      else if (paf->location == APPLY_DAMROLL )
      {
         dam_bonus = paf->modifier;
         dam_found = TRUE;
         fail += 2 * (dam_bonus * dam_bonus);
      }

      else  /* things get a little harder */
      fail += 25;
   }

   for ( paf = obj->affected; paf != NULL; paf = paf->next )
   {
      if ( paf->location == APPLY_HITROLL )
      {
         hit_bonus = paf->modifier;
         hit_found = TRUE;
         fail += 2 * (hit_bonus * hit_bonus);
      }

      else if (paf->location == APPLY_DAMROLL )
      {
         dam_bonus = paf->modifier;
         dam_found = TRUE;
         fail += 2 * (dam_bonus * dam_bonus);
      }

      else /* things get a little harder */
      fail += 25;
   }

   /* apply other modifiers */
   fail -= 3 * level/2;

   if (IS_OBJ_STAT(obj, ITEM_BLESS))
   fail -= 15;
   if (IS_OBJ_STAT(obj, ITEM_GLOW))
   fail -= 5;

   if (enchanter) fail /= 2;
   fail = URANGE(5, fail, 95);

   result = number_percent()*race_adjust(ch)/25.0;

   /* the moment of truth */
   if (result < (fail / 5))  /* item destroyed */
   {
      act("$p shivers violently and explodes!", ch, obj, NULL, TO_CHAR);
      act("$p shivers violently and explodes!", ch, obj, NULL, TO_ROOM);
      extract_obj(obj, FALSE);
      return;
   }

   if (result < (fail / 2)) /* item disenchanted */
   {
      AFFECT_DATA *paf_next;

      act("$p glows brightly, then fades...oops.", ch, obj, NULL, TO_CHAR);
      act("$p glows brightly, then fades.", ch, obj, NULL, TO_ROOM);
      obj->enchanted = TRUE;

      /* remove all affects */
      for (paf = obj->affected; paf != NULL; paf = paf_next)
      {
         paf_next = paf->next;
         if (paf->type != gsn_alchemy)
         free_affect(paf);
      }
      obj->affected = NULL;

      REMOVE_BIT(obj->extra_flags, ITEM_GLOW);
      REMOVE_BIT(obj->extra_flags, ITEM_HUM);
      REMOVE_BIT(obj->extra_flags, ITEM_MAGIC);
      REMOVE_BIT(obj->extra_flags, ITEM_INVIS);
      REMOVE_BIT(obj->extra_flags, ITEM_NODROP);
      REMOVE_BIT(obj->extra_flags, ITEM_NOREMOVE);
      return;
   }

   if ( result <= fail )  /* failed, no bad result */
   {
      send_to_char("Nothing seemed to happen.\n\r", ch);
      return;
   }

   /* okay, move all the old flags into new vectors if we have to */
   if (!obj->enchanted)
   {
      AFFECT_DATA *af_new;
      obj->enchanted = TRUE;

      for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
      {
         af_new = new_affect();

         *af_new = *paf;
         af_new->next = obj->affected;
         obj->affected = af_new;
      }
   }

   if (result <= (100 - level/5))  /* success! */
   {
      act("$p glows blue.", ch, obj, NULL, TO_CHAR);
      act("$p glows blue.", ch, obj, NULL, TO_ROOM);
      SET_BIT(obj->extra_flags, ITEM_MAGIC);
      added = 1;
   }

   else  /* exceptional enchant */
   {
      act("$p glows a brilliant blue!", ch, obj, NULL, TO_CHAR);
      act("$p glows a brilliant blue!", ch, obj, NULL, TO_ROOM);
      SET_BIT(obj->extra_flags, ITEM_MAGIC);
      added = 2;
   }

   if ( (result = number_percent()) <= 25 && !IS_SET(obj->extra_flags, ITEM_HUM) )
   {
      SET_BIT(obj->extra_flags, ITEM_HUM);
      REMOVE_BIT(obj->extra_flags, ITEM_GLOW);
   }
   if ( (result = number_percent()) <= 25 && !IS_SET(obj->extra_flags, ITEM_GLOW) )
   SET_BIT(obj->extra_flags, ITEM_GLOW);

   if (level >= 100)
   {
      if (obj->pIndexData->condition >= 0)
      SET_BIT(obj->extra_flags, ITEM_ROT_DEATH);
   }

   /* now add the enchantments */

   if (obj->level < ch->level)
   obj->level = UMIN(ch->level, obj->level + 1);

   if (dam_found)
   {
      for ( paf = obj->affected; paf != NULL; paf = paf->next)
      {
         if ( paf->location == APPLY_DAMROLL)
         {
            paf->type = sn;
            paf->modifier += added;
            paf->level = UMAX(paf->level, level);
            break;
         }
      }
   }
   else /* add a new affect */
   {
      paf = new_affect();

      paf->where    = TO_OBJECT;
      paf->type    = sn;
      paf->level    = level;
      paf->duration    = -1;
      paf->location    = APPLY_DAMROLL;
      paf->modifier    =  added;
      paf->bitvector  = 0;
      paf->next    = obj->affected;
      obj->affected    = paf;
   }

   if (hit_found)
   {
      for ( paf = obj->affected; paf != NULL; paf = paf->next)
      {
         if ( paf->location == APPLY_HITROLL)
         {
            paf->type = sn;
            paf->modifier += added;
            paf->level = UMAX(paf->level, level);
            break;
         }
      }
   }
   else /* add a new affect */
   {
      paf = new_affect();

      paf->where      = TO_OBJECT;
      paf->type       = sn;
      paf->level      = level;
      paf->duration   = -1;
      paf->location   = APPLY_HITROLL;
      paf->modifier   =  added;
      paf->bitvector  = 0;
      paf->next       = obj->affected;
      obj->affected   = paf;
   }

   return;

}


void spell_syphon_soul(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   OBJ_DATA *orb;
   char buf[MAX_STRING_LENGTH];
   int dam, take_hp, take_mana, take_move;
   int num;
   num = 2 + level/10;
   dam = dice(level, num);
   if (check_spellcraft(ch, sn))
   dam = spellcraft_dam(level, num);

   if (IS_IMMORTAL(victim) && !IS_IMMORTAL(ch)){
      send_to_char("You fail.\n\r", ch);
      return;
   }
   if (IS_NPC(victim) && IS_SET(victim->act, ACT_UNDEAD)){
      act("$E has not soul to syphon.", ch, NULL, victim, TO_CHAR);
      /*    send_to_char("They have no soul to syphon.\n\r", ch); */
      return;
   }

   if ( saves_spell(ch, level+8, victim, DAM_NEGATIVE, SAVE_SPELL) || victim == ch
   || saves_armor_of_god(ch, level, victim))
   {
      damage(ch, victim, dam, sn, DAM_NEGATIVE, TRUE);
      return;
   }

   level = victim->level;

   send_to_char("You take part of their soul and store in a magical orb.\n\r", ch);
   send_to_char("You feel as though part of your soul has been taken from you.\n\r", victim);

   take_mana = number_range(level, level * 2);
   take_move = number_range(level, level*3 - take_mana);
   take_hp   = level * 3 - take_mana - take_move;
   if (take_hp < 0)
   take_hp = 0;
   victim->move -= take_move;
   if (victim->move < 0 && !IS_NPC(victim)){
      take_mana -= victim->move;
      take_move += victim->move;
      victim->move = 0;
   }
   victim->mana -= take_mana;
   if (victim->mana < 0 && !IS_NPC(victim)){
      take_hp -= victim->mana;
      take_mana += victim->mana;
      victim->mana = 0;
   }
   dam = dam + take_hp;

   damage(ch, victim, dam, sn, DAM_NEGATIVE, TRUE);

   orb = create_object( get_obj_index( OBJ_VNUM_SOUL_ORB ), 0 );
   orb->value[0] = take_hp;
   orb->value[1] = take_mana+20;
   orb->value[2] = take_move;
   orb->level = ch->level;
   free_string(orb->short_descr);

   if (!IS_NPC(victim))
   sprintf(buf, "A orb containing part of %s's soul", get_longname(victim, NULL));
   else
   sprintf(buf, "A orb containing part of %s's soul", victim->short_descr);

   orb->short_descr = str_dup(buf);
   free_string(orb->name);
   sprintf(buf, "orb soul %s", get_name(victim, NULL));
   orb->name = str_dup(buf);

   if (number_percent() > get_skill(ch, sn)){
      send_to_char("You make a mistake forming the orb and it shatters.\n\r", ch);
      extract_obj(orb, FALSE);
      return;
   }

   if (ch->carry_number + get_obj_number(orb)>can_carry_n(ch))
   {
      send_to_char( "You can't carry that many items.\n\r", ch );
      send_to_char("You drop the orb and it shatters.\n\r", ch);
      extract_obj(orb, FALSE);
      return;
   }

   obj_to_char( orb, ch );

   return;
}

void spell_fireball( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;
   char buf[MAX_STRING_LENGTH];
   static const sh_int dam_each[] =
   {
      0,
      0,   0,   0,   0,   0,      0,   0,   0,   0,   0,
      0,   0,   0,   0,  30,     35,  40,  45,  50,  55,
      60,  65,  70,  75,  80,     82,  84,  86,  88,  90,
      92,  94,  96,  98, 100,    102, 104, 106, 108, 110,
      112, 114, 116, 118, 120,    122, 124, 126, 128, 130,
      132, 133, 134, 135, 136,        137, 138, 139, 140, 141
   };
   int dam;
   bool spellcraft = FALSE;

   if (check_peace(ch))
   {
      return;
   }

   send_to_char("You conjure a raging fireball to scorch your foes.\n\r", ch);
   act("$n conjures a raging fireball that scorches everything in its path!", ch, NULL, NULL, TO_ROOM);

   level    = UMAX(0, level) * race_adjust(ch) / 25.0;
   level    = UMIN(level, (int)(sizeof(dam_each)/sizeof(dam_each[0]) - 1));
   level    = UMAX(0, level);
   if (check_spellcraft(ch, sn))
   {
      spellcraft = TRUE;
   }

   for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
   {
      vch_next = vch->next_in_room;
      if
      (
         is_same_group(vch, ch) ||
         is_safe(ch, vch, IS_SAFE_SILENT | IS_SAFE_AREA_ATTACK) ||
         oblivion_blink(ch, vch)
      )
      {
         continue;
      }
      if (!IS_NPC(ch) && !IS_NPC(vch)
      && (ch->fighting == NULL || vch->fighting == NULL))
      {
         if
         (
            is_supplicating &&
            (
               IS_NPC(ch) ||
               !IS_IMMORTAL(ch)
            )
         )
         {
            switch (number_range(0, 2))
            {
               default:  /* 0, 1 */
               {
                  sprintf
                  (
                     buf,
                     "Die, %s, you holy terror!",
                     PERS(ch, vch)
                  );
                  break;
               }
               case (2):
               {
                  sprintf
                  (
                     buf,
                     "Help! %s is helping %s hurt me!",
                     get_god_name(ch, vch),
                     PERS(ch, vch)
                  );
                  break;
               }
            }
         }
         else
         {
            switch (number_range(0, 2))
            {
               default:  /* 0, 1 */
               {
                  sprintf
                  (
                     buf,
                     "Die, %s, you sorcerous dog!",
                     PERS(ch, vch)
                  );
                  break;
               }
               case (2):
               {
                  sprintf
                  (
                     buf,
                     "Help! %s is casting a spell on me!",
                     PERS(ch, vch)
                  );
                  break;
               }
            }
         }
         if (vch != ch)
         {
            do_myell(vch, buf);
            sprintf
            (
               log_buf,
               "[%s] cast fireball upon [%s] at %d",
               ch->name,
               vch->name,
               ch->in_room->vnum
            );
            log_string(log_buf);
         }
      }

      dam = number_range(dam_each[level] / 2, dam_each[level] * 2);
      if (dam > 282)
      {
         dam = 282;
      }
      if (spellcraft)
      {
         dam += spellcraft_dam(level, 3);
      }
      else
      {
         dam += dice(level, 3);
      }
      dam *= 8;
      dam /= 10;
      if (saves_spell(ch, level, vch, DAM_FIRE, SAVE_SPELL))
      {
         dam /= 2;
      }
      damage(ch, vch, dam, sn, DAM_FIRE , TRUE);
   }

   return;
}


void spell_fireproof(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   OBJ_DATA *obj = (OBJ_DATA *) vo;
   AFFECT_DATA af;

   if (IS_OBJ_STAT(obj, ITEM_BURN_PROOF))
   {
      act("$p is already protected from burning.", ch, obj, NULL, TO_CHAR);
      return;
   }

   af.where     = TO_OBJECT;
   af.type      = sn;
   af.level     = level;
   af.duration  = number_fuzzy(level / 4)*race_adjust(ch)/25.0;
   if (!IS_NPC(ch) && ch->pcdata->special == SUBCLASS_PYROMANCER)
   af.duration = -1;
   af.location  = APPLY_NONE;
   af.modifier  = 0;
   af.bitvector = ITEM_BURN_PROOF;

   affect_to_obj(obj, &af);

   act("You protect $p from fire.", ch, obj, NULL, TO_CHAR);
   act("$p is surrounded by a protective aura.", ch, obj, NULL, TO_ROOM);
}



void spell_flamestrike( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   int dam;

   if (check_spellcraft(ch, sn))
   dam = spellcraft_dam(6 + level / 2, 8);
   else
   dam = dice(6 + level / 2, 8);
   if ( saves_spell( ch, level, victim, DAM_FIRE, SAVE_SPELL) )
   dam /= 2;
   damage( ch, victim, dam*race_adjust(ch)/25.0, sn, DAM_FIRE , TRUE);
   return;
}



void spell_faerie_fire( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;

   if ( IS_AFFECTED(victim, AFF_FAERIE_FIRE) )
   {
      if (victim == ch)
      send_to_char("You are already surrounded by a pink outline.\n\r", ch);
      else
      act( "$N is already surrounded by a pink outline.", ch, NULL, victim, TO_CHAR );
      return;
   }

   if (saves_spell(ch, (level+15), victim, DAM_OTHER, SAVE_MALEDICT))
   {
      send_to_char( "Your faerie fire fails to outline its target.\n\r", ch );
      return;
   }

   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.duration  = (6 + (level/8))*race_adjust(ch)/25.0;
   af.location  = APPLY_AC;
   af.modifier  = 2 * level;
   af.bitvector = AFF_FAERIE_FIRE;
   affect_to_char( victim, &af );
   send_to_char( "You are surrounded by a pink outline.\n\r", victim );
   act( "$n is surrounded by a pink outline.", victim, NULL, NULL, TO_ROOM );
   un_shroud(victim, NULL);
   un_earthfade(victim, NULL);
   un_camouflage(victim, NULL);
   un_hide(victim, NULL);
   un_invis(victim, NULL);
   un_sneak(victim, NULL);
   un_forest_blend(victim);
   affect_strip(ch, gsn_stalking);

   return;
}



void spell_faerie_fog( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *ich;
   AFFECT_DATA af;

   if (check_peace(ch)) return;

   act( "$n conjures a cloud of purple smoke.", ch, NULL, NULL, TO_ROOM );
   send_to_char( "You conjure a cloud of purple smoke.\n\r", ch );

   af.where = TO_AFFECTS;
   af.type = sn;
   af.level = level;
   af.location = 0;
   af.duration = (4 + (level/8))*race_adjust(ch)/25.0;
   af.bitvector = 0;
   af.modifier = 0;

   for ( ich = ch->in_room->people; ich != NULL; ich = ich->next_in_room )
   {
      if (ich->invis_level > 0)
      continue;

      if (ich->trust > 51 )
      continue;

      if (oblivion_blink(ch, ich))
      {
         continue;
      }

      if (!can_area_hit_rogue(ch, ich))
      {
         continue;
      }

      if ( ich == ch || saves_spell( ch, level, ich, DAM_OTHER, SAVE_SPELL) )
      continue;
      affect_strip( ich, sn );
      affect_to_char( ich, &af);

      affect_strip(ich, gsn_stalking);
      affect_strip ( ich, gsn_shroud            );
      affect_strip ( ich, gsn_camouflage        );
      affect_strip ( ich, gsn_invis            );
      affect_strip ( ich, gsn_mass_invis        );
      affect_strip ( ich, gsn_sneak            );
      REMOVE_BIT   ( ich->affected_by, AFF_CAMOUFLAGE );
      REMOVE_BIT   ( ich->affected_by, AFF_HIDE    );
      REMOVE_BIT   ( ich->affected_by, AFF_INVISIBLE    );
      REMOVE_BIT   ( ich->affected_by, AFF_SNEAK    );
      act( "$n is revealed!", ich, NULL, NULL, TO_ROOM );
      send_to_char( "You are revealed!\n\r", ich );
   }

   return;
}

void spell_fly(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;

   if (IS_FLYING(victim))
   {
      if (victim == ch)
      send_to_char("You are already airborne.\n\r", ch);
      else
      act("$N doesn't need your help to fly.", ch, NULL, victim, TO_CHAR);
      return;
   }
   if (IS_AFFECTED(victim, AFF_FLYING))
   {
      if (victim == ch)
      {
         send_to_char("Another spell will not help lift you up.\n\r", ch);
      }
      else
      {
         act("Another spell will not help lift $N up.", ch, NULL, victim, TO_CHAR);
      }
      return;
   }
   if (is_affected(victim, gsn_earthbind))
   {
      if (ch != victim)
      {
         act("$N's feet are binded to the earth.", ch, NULL, victim, TO_CHAR);
      }
      else
      {
         send_to_char("Your feet are binded to the earth.\n\r", ch);
      }
      return;
   }
   if (is_affected(victim, gsn_earthfade))
   {
      if (ch != victim)
      {
         act("$N's feet are stuck inside the earth.", ch, NULL, victim, TO_CHAR);
      }
      else
      {
         send_to_char("Your feet are stuck inside the earth.\n\r", ch);
      }
      return;
   }
   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.duration  = (level + 3)*race_adjust(ch)/25.0;
   af.location  = 0;
   af.modifier  = 0;
   af.bitvector = AFF_FLYING;
   affect_to_char( victim, &af );
   if (IS_FLYING(victim))
   {
      send_to_char( "Your feet rise off the ground.\n\r", victim );
      act( "$n's feet rise off the ground.", victim, NULL, NULL, TO_ROOM );
   }
   else
   {
      send_to_char("You failed.\n\r", ch);
   }
   return;
}

/* RT clerical berserking spell */

void spell_frenzy(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   CHAR_DATA* victim = (CHAR_DATA*) vo;
   AFFECT_DATA af;
   OBJ_DATA* brand;
   sh_int god_ch = god_lookup(ch);
   sh_int god_victim = god_lookup(victim);

   if
   (
      (
         (
            brand = get_eq_char(victim, WEAR_BRAND)
         ) != NULL
      ) &&
      brand->pIndexData->vnum == 433
   )
   {
      send_to_char("The gods have forsaken you.\n\r", victim);
      if (victim != ch)
      {
         act("The gods have forsaken $M.", ch, NULL, victim, TO_CHAR);
      }
      return;
   }

   if
   (
      (
         (
            brand = get_eq_char(ch, WEAR_BRAND)
         ) != NULL
      ) &&
      brand->pIndexData->vnum == 433
   )
   {
      send_to_char("The gods have forsaken you.\n\r", ch);
      return;
   }

   if
   (
      is_affected(victim, sn) ||
      IS_AFFECTED(victim, AFF_BERSERK)
   )
   {
      if (victim == ch)
      {
         send_to_char("You are already in a frenzy.\n\r", ch);
      }
      else
      {
         act("$N is already in a frenzy.", ch, NULL, victim, TO_CHAR);
      }
      return;
   }

   if (is_affected(victim, gsn_calm))
   {
      if (victim == ch)
      {
         send_to_char("Why don't you just relax for a while?\n\r", ch);
      }
      else
      {
         act
         (
            "$N doesn't look like $e wants to fight anymore.",
            ch,
            NULL,
            victim,
            TO_CHAR
         );
      }
      return;
   }

   if (is_god_enemy(ch, victim))
   {
      return;
   }
   if
   (
      god_ch == -1 &&
      !IS_IMMORTAL(ch) &&
      (
         (
            god_victim == god_ch &&
            (
               (
                  IS_GOOD(ch) &&
                  !IS_GOOD(victim)
               ) ||
               (
                  IS_NEUTRAL(ch) &&
                  !IS_NEUTRAL(victim)
               ) ||
               (
                  IS_EVIL(ch) &&
                  !IS_EVIL(victim)
               )
            )
         ) ||
         god_victim != god_ch
      )
   )
   {
      act("Your god does not seem to like $N", ch, NULL, victim, TO_CHAR);
      return;
   }

   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.duration  = level / 3;
   af.modifier  = level / 6;
   af.bitvector = 0;

   af.location  = APPLY_HITROLL;
   affect_to_char(victim, &af);

   af.location  = APPLY_DAMROLL;
   affect_to_char(victim, &af);

   af.modifier  = 10 * (level / 12) + 25 - race_adjust(ch);
   af.location  = APPLY_AC;
   if
   (
      IS_NPC(ch) ||
      ch->pcdata->special != SUBCLASS_ZEALOT_FAITH
   )
   {
      affect_to_char(victim, &af);
   }

   send_to_char("You are filled with holy wrath!\n\r", victim);
   act("$n gets a wild look in $s eyes!", victim, NULL, NULL, TO_ROOM);
}

void spell_capture( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   CHAR_DATA *check, *check_next;
   ROOM_INDEX_DATA *room = get_room_index(ROOM_VNUM_COVENANT_PRISON);
   char buf[MAX_STRING_LENGTH];

   if (room == NULL)
   return;

   if (house_down(ch, HOUSE_EMPIRE))
   return;

   if (is_safe(ch, victim, IS_SAFE_SILENT)){
      send_to_char("You can't do that.\n\r", ch);
      return;
   }
   if (check_peace(ch)) return;

   if (IS_NPC(victim) /* || !IS_SET(victim->act, PLR_REBEL) */){
      send_to_char("You can only capture rebels.\n\r", ch);
      return;
   }
   /* special case for towns */
   if (check_room_protected(ch->in_room) ||
   is_occupied(ch->in_room->vnum)){
      if (saves_spell(ch, level, victim, DAM_OTHER, SAVE_TRAVEL)){
         act("$E evades your attempt to capture $M.", ch, NULL, victim, TO_CHAR);
         /*    send_to_char("They evade your attempt to capture them.\n\r", ch); */
         send_to_char("You evade an attempt to capture you.\n\r", victim);
         return;

      }

      for (check = ch->in_room->people; check != NULL; check = check_next)
      {
         check_next = check->next_in_room;
         if (IS_NPC(check) && (check->master == ch) && (check->pIndexData->vnum == MOB_VNUM_EMPIRE_G) )
         {
            char_from_room(check);
            char_to_room(check, room);
         }
      }

      char_from_room(ch);
      char_to_room(ch, room);
      char_from_room(victim);
      char_to_room(victim, room);
      do_observe(ch, "", LOOK_AUTO);
      do_observe(victim, "", LOOK_AUTO);
      DAZE_STATE(victim, 24);

      return;

   }
   sprintf(buf, "Help! %s is trying to capture me!", PERS(ch, NULL));
   do_myell(victim, buf);

   if (saves_spell(ch, level, victim, DAM_OTHER, SAVE_TRAVEL) ||
   saves_spell(ch, level, victim, DAM_OTHER, SAVE_TRAVEL)){
      send_to_char("You failed.\n\r", ch);
      return;
   }


   for (check = ch->in_room->people; check != NULL; check = check_next)
   {
      check_next = check->next_in_room;
      if (IS_NPC(check) && (check->master == ch) && (check->pIndexData->vnum == MOB_VNUM_EMPIRE_G) )
      {
         char_from_room(check);
         char_to_room(check, room);
      }
   }

   char_from_room(ch);
   char_to_room(ch, room);
   char_from_room(victim);
   char_to_room(victim, room);
   do_observe(ch, "", LOOK_AUTO);
   do_observe(victim, "", LOOK_AUTO);
   DAZE_STATE(victim, 24);

   return;
}

void spell_giant_strength(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;

   if ( is_affected( victim, sn ) )
   {
      if (victim == ch)
      send_to_char("You are already as strong as you can get!\n\r", ch);
      else
      act("$N can't get any stronger.", ch, NULL, victim, TO_CHAR);
      return;
   }

   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.duration  = level;
   af.location  = APPLY_STR;
   af.modifier  = 1 + (level >= 18) + (level >= 25) + (level >= 32);
   af.bitvector = 0;
   affect_to_char( victim, &af );
   send_to_char( "Your muscles surge with heightened power!\n\r", victim );
   act("$n's muscles surge with heightened power.", victim, NULL, NULL, TO_ROOM);
   return;
}



void spell_harm( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   int dam;
   OBJ_DATA *brand;

   if (((brand = get_eq_char(ch, WEAR_BRAND)) != NULL)
   && (brand->pIndexData->vnum == 433))
   {
      send_to_char("The gods have forsaken you.\n\r", ch);
      return;
   }

   dam = UMAX(  20, victim->hit - dice(1, 4) );
   if ( saves_spell( ch, level, victim, DAM_HARM, SAVE_SPELL) )
   dam = UMIN( 50, dam / 2 );
   dam = UMIN( 100, dam );
   damage( ch, victim, dam*race_adjust(ch)/25.0, sn, DAM_HARM , TRUE);
   return;
}

/* RT haste spell */

void spell_haste( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;

   if (!str_cmp(class_table[ch->class].name, "monk")
   && victim != ch)
   {
      send_to_char("You are only able to enhance your own physical abilities.\n\r", ch);
      return;
   }

   if ( is_affected( victim, sn ) || IS_AFFECTED(victim, AFF_HASTE)
   ||   IS_SET(victim->off_flags, OFF_FAST))
   {
      if (victim == ch)
      send_to_char("You can't move any faster!\n\r", ch);
      else
      act("$N is already moving as fast as $E can.",
      ch, NULL, victim, TO_CHAR);
      return;
   }

   if (IS_AFFECTED(victim, AFF_SLOW))
   {
      if (!check_dispel(level, victim, gsn_slow) &&
      !check_dispel(level, victim, gsn_mass_slow))
      {
         if (victim != ch)
         send_to_char("You failed.\n\r", ch);
         send_to_char("You feel momentarily faster.\n\r", victim);
         return;
      }
      act("$n is moving less slowly.", victim, NULL, NULL, TO_ROOM);
      return;
   }

   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   if (victim == ch)
   af.duration  = level/2;
   else
   af.duration  = level/4;
   af.location  = APPLY_DEX;
   af.modifier  = 1 + (level >= 18) + (level >= 25) + (level >= 32);
   af.bitvector = AFF_HASTE;
   affect_to_char( victim, &af );
   send_to_char( "You feel yourself moving more quickly.\n\r", victim );
   act("$n is moving more quickly.", victim, NULL, NULL, TO_ROOM);
   return;
}



void spell_heal( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   OBJ_DATA *brand;
   int heal;

   if (((brand = get_eq_char(victim, WEAR_BRAND)) != NULL)
   && (brand->pIndexData->vnum == 433))
   {
      send_to_char("The gods have forsaken you.\n\r", victim);
      if (victim != ch)
      send_to_char("The gods have forsaken them.\n\r", ch);
      return;
   }

   if (((brand = get_eq_char(ch, WEAR_BRAND)) != NULL)
   && (brand->pIndexData->vnum == 433))
   {
      send_to_char("The gods have forsaken you.\n\r", ch);
      return;
   }

   if (is_affected(victim, gsn_corrupt("smite", &gsn_smite)))
   {
      send_to_char("You feel better, but the wounds do not close.\n\r", victim);
      return;
   }
   if ( is_affected(victim, gsn_black_mantle))
   {
      if (is_supplicating)
      {
         send_to_char
         (
            "The black mantle prevents your divine healing.\n\r",
            ch
         );
      }
      else
      {
         send_to_char
         (
            "The black mantle absorbs your healing magics.\n\r",
            ch
         );
      }
      if (ch != victim)
      {
         if (is_supplicating)
         {
            send_to_char
            (
               "The black mantle prevents the divine healing.\n\r",
               victim
            );
         }
         else
         {
            send_to_char
            (
               "The black mantle absorbs the healing magics.\n\r",
               victim
            );
         }
      }
      return;
   }
   heal = 100;

   if
   (
      is_affected(victim, gsn_ancient_plague) ||
      is_affected(victim, gsn_mantle_oblivion) ||
      IS_SET(victim->act2, PLR_LICH)
   )
   heal = heal/2;

   if (!IS_NPC(ch) && ch->pcdata->special == SUBCLASS_PRIEST_HEALING)
   heal = heal + heal/2;
   victim->hit = UMIN( victim->hit + heal, victim->max_hit );
   update_pos( victim );
   send_to_char( "A warm feeling fills your body.\n\r", victim );
   if ( ch != victim )
   {
      act
      (
         "You grant them healing from $z.",
         ch,
         NULL,
         NULL,
         TO_CHAR
      );
      evaluate_wounds(ch, victim);
   }
   return;
}

void spell_heat_metal( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   OBJ_DATA *obj_lose, *obj_next;
   int dam = 0;
   bool fail = TRUE;

   if (!saves_spell(ch, (level + 2), victim, DAM_FIRE, SAVE_SPELL)
   &&  !IS_SET(victim->imm_flags, IMM_FIRE))
   {
      for ( obj_lose = victim->carrying;
      obj_lose != NULL;
      obj_lose = obj_next)
      {
         obj_next = obj_lose->next_content;
         if ( number_range(1, 2 * level) > obj_lose->level
         &&   !saves_spell(ch, level, victim, DAM_FIRE, SAVE_SPELL)
         &&   !IS_OBJ_STAT(obj_lose, ITEM_NONMETAL)
         &&   is_metal(obj_lose)
         &&   !IS_OBJ_STAT(obj_lose, ITEM_BURN_PROOF))
         {
            switch ( obj_lose->item_type )
            {
               case ITEM_ARMOR:
               if (obj_lose->wear_loc != -1) /* remove the item */
               {
                  if (can_remove_obj(victim, obj_lose)
                  &&  (obj_lose->weight / 10) <
                  number_range(1, 2 * get_curr_stat(victim, STAT_DEX))
                  &&  remove_obj( victim, obj_lose->wear_loc, TRUE, TRUE ))
                  {
                     act("$n yelps and throws $p to the ground!",
                     victim, obj_lose, NULL, TO_ROOM);
                     act("You remove and drop $p before it burns you.",
                     victim, obj_lose, NULL, TO_CHAR);
                     dam += (number_range(1, obj_lose->level) / 3);
                     obj_from_char(obj_lose);
                     obj_to_room(obj_lose, victim->in_room);
                     fail = FALSE;
                  }
                  else /* stuck on the body! ouch! */
                  {
                     act("Your skin is seared by $p!",
                     victim, obj_lose, NULL, TO_CHAR);
                     dam += (number_range(1, obj_lose->level));
                     fail = FALSE;
                  }

               }
               else /* drop it if we can */
               {
                  if (can_drop_obj(victim, obj_lose))
                  {
                     act("$n yelps and throws $p to the ground!",
                     victim, obj_lose, NULL, TO_ROOM);
                     act("You yelp with surprise and drop $p before it burns you.",
                     victim, obj_lose, NULL, TO_CHAR);
                     dam += (number_range(1, obj_lose->level) / 6);
                     obj_from_char(obj_lose);
                     obj_to_room(obj_lose, victim->in_room);
                     fail = FALSE;
                  }
                  else /* cannot drop */
                  {
                     act("Your skin is seared by $p!",
                     victim, obj_lose, NULL, TO_CHAR);
                     dam += (number_range(1, obj_lose->level) / 2);
                     fail = FALSE;
                  }
               }
               break;
               case ITEM_WEAPON:
               if (obj_lose->wear_loc != -1) /* try to drop it */
               {
                  if (IS_WEAPON_STAT(obj_lose, WEAPON_FLAMING))
                  continue;

                  if (can_remove_obj(victim, obj_lose)
                  &&  remove_obj(victim, obj_lose->wear_loc, TRUE, TRUE))
                  {
                     act("$n is burned by $p, and throws it to the ground.",
                     victim, obj_lose, NULL, TO_ROOM);
                     send_to_char(
                     "You throw your red-hot weapon to the ground!\n\r",
                     victim);
                     dam += 1;
                     obj_from_char(obj_lose);
                     obj_to_room(obj_lose, victim->in_room);
                     fail = FALSE;
                  }
                  else /* YOWCH! */
                  {
                     send_to_char("Your weapon sears your flesh!\n\r",
                     victim);
                     dam += number_range(1, obj_lose->level);
                     fail = FALSE;
                  }
                  if (IS_WEAPON_STAT(obj_lose, WEAPON_DROWNING))
                  {
                     send_to_char("Steam rises from your weapon and scalds you!\n\r",
                     victim);
                     dam += number_range(1, obj_lose->level);
                  }
               }
               else /* drop it if we can */
               {
                  if (can_drop_obj(victim, obj_lose))
                  {
                     act("$n throws a burning hot $p to the ground!",
                     victim, obj_lose, NULL, TO_ROOM);
                     act("You cry out with dismay and drop $p before it burns you.",
                     victim, obj_lose, NULL, TO_CHAR);
                     dam += (number_range(1, obj_lose->level) / 6);
                     obj_from_char(obj_lose);
                     obj_to_room(obj_lose, victim->in_room);
                     fail = FALSE;
                  }
                  else /* cannot drop */
                  {
                     act("Your skin is seared by $p!",
                     victim, obj_lose, NULL, TO_CHAR);
                     dam += (number_range(1, obj_lose->level) / 2);
                     fail = FALSE;
                  }
               }
               break;
            }
         }
      }
   }
   if (fail)
   {
      send_to_char("Your spell had no effect.\n\r", ch);
      send_to_char("You feel momentarily warmer.\n\r", victim);
   }
   else /* damage! */
   {
      if (saves_spell(ch, level, victim, DAM_FIRE, SAVE_SPELL))
      dam = 2 * dam / 3;
      damage(ch, victim, dam*race_adjust(ch)/25.0, sn, DAM_FIRE, TRUE);
   }
}

void spell_flash( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;
   AFFECT_DATA af;
   char buf[MAX_STRING_LENGTH];
   int gsn;

   if (check_peace(ch)) return;

   send_to_char("You open your fist and flood the area with dazzling light!\n\r", ch);
   act("$n opens $s fist and floods the area with dazzling light!", ch, NULL, NULL, TO_ROOM);

   for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
   {
      vch_next = vch->next_in_room;
      if
      (
         IS_IMMORTAL(vch) &&
         !can_see(ch, vch) &&
         wizi_to(vch, ch)
      ) continue;
      if (oblivion_blink(ch, vch))
      {
         continue;
      }
      if
      (
         !IS_NPC(ch) &&
         !IS_NPC(vch) &&
         EVIL_TO(ch, vch) &&
         (
            ch->fighting == NULL ||
            vch->fighting == NULL
         )
      )
      {
         if (is_safe(ch, vch, IS_SAFE_SILENT | IS_SAFE_AREA_ATTACK))
         {
            continue;
         }
         sprintf(buf, "Help! %s is blasting me with Light!", PERS(ch, vch));
         if (vch != ch)
         {
            do_myell(vch, buf);
            sprintf(log_buf, "[%s] cast flash upon [%s] at %d", ch->name, vch->name, ch->in_room->vnum);
            log_string(log_buf);
         }
      }
      if
      (
         vch->alignment > 0 &&
         !EVIL_TO(ch, vch)
      )
      {
         /* one hour infravision */
         if ( !IS_AFFECTED(vch, AFF_INFRARED) )
         {
            gsn = gsn_infravision;
            act( "$n's eyes glow red.", vch, NULL, NULL, TO_ROOM );
            af.where     = TO_AFFECTS;
            af.type      = gsn;
            af.level     = level;
            af.duration  = 1;
            af.location  = APPLY_NONE;
            af.modifier  = 0;
            af.bitvector = AFF_INFRARED;
            affect_to_char( vch, &af );
            send_to_char( "Your eyes glow red.\n\r", vch );
         }
         continue;
      }
      if (is_safe(ch, vch, IS_SAFE_SILENT | IS_SAFE_AREA_ATTACK))
      {
         continue;
      }
      if (EVIL_TO(ch, vch))
      {
         if ( saves_spell( ch, level, vch, DAM_LIGHT, SAVE_MALEDICT) )
         {
            damage( ch, vch, 0, sn, DAM_LIGHT , TRUE);
            continue;
         }
         /* one hour blindness */
         if (!IS_AFFECTED(vch, AFF_BLIND))
         {
            gsn = gsn_blindness;
            af.where     = TO_AFFECTS;
            af.type      = gsn;
            af.level     = level;
            af.location  = APPLY_HITROLL;
            af.modifier  = -1-level/10.0;
            af.duration  = 1;
            af.bitvector = AFF_BLIND;
            affect_to_char( vch, &af );
            send_to_char( "You are blinded by the Light!\n\r", vch );
            act("$n appears to be blinded by the heavenly Light.", vch, NULL, NULL, TO_ROOM);
         }
         if (vch->daze == 0)
         {
            send_to_char( "The shock of the flash stuns you briefly.\n\r", vch );
            act("$n is stunned briefly by the flash.", vch, NULL, NULL, TO_ROOM);
            DAZE_STATE(vch, 24);
         }
         damage( ch, vch, number_range(2, 10), sn, DAM_LIGHT , TRUE);
      }
   }
   return;
}

void spell_need(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   CHAR_DATA* vch;
   CHAR_DATA* vch_next;
   int count       = 0;
   int mobvsmob    = 0;
   int pkvspk      = 0;
   int pkvsmob     = 0;
   int whichtarget = 0;

   if (ch->pause > 0)
   {
      send_to_char
      (
         "To surrender to the gods to do their bidding, you must first be at"
         " peace.\n\r",
         ch
      );
      return;
   }
   if
   (
      (
         IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL) ||
         IS_SET(ch->in_room->extra_room_flags, ROOM_DEAD_ZONE)
      ) &&
      !IS_SET(ch->in_room->room_flags, ROOM_CONSECRATED)
   )
   {
      send_to_char("You failed to go where you are needed.\n\r", ch);
      return;
   }

   for (vch = char_list; vch != NULL; vch = vch_next)
   {
      vch_next = vch->next;
      if
      (
         vch->fighting == NULL ||
         IS_SET(vch->in_room->extra_room_flags, ROOM_1212) ||
         IS_SET(vch->in_room->room_flags, ROOM_SAFE) ||
         IS_SET(vch->in_room->room_flags, ROOM_PRIVATE) ||
         IS_SET(vch->in_room->room_flags, ROOM_SOLITARY) ||
         (
            (
               IS_SET(vch->in_room->room_flags, ROOM_NO_RECALL) ||
               IS_SET(vch->in_room->extra_room_flags, ROOM_DEAD_ZONE)
            ) &&
            !IS_SET(vch->in_room->room_flags, ROOM_CONSECRATED)
         ) ||
         (
            !IS_NPC(vch) &&
            vch->level >= LEVEL_IMMORTAL
         ) ||
         (
            vch->in_room->house != 0 &&
            vch->in_room->house != ch->house
         ) ||
         (
            IS_NPC(vch) &&
            vch->pIndexData->pShop != NULL
         ) ||
         !(
            (
               vch->alignment > 0 &&
               vch->fighting->alignment < 0
            ) ||
            (
               vch->alignment < 0 &&
               vch->fighting->alignment > 0
            )
         )
      )
      {
         continue;
      }
      if
      (
         IS_NPC(vch) &&
         IS_NPC(vch->fighting)
      )
      {
         mobvsmob++;
      }
      else if
      (
         !IS_NPC(vch) &&
         IS_NPC(vch->fighting)
      )
      {
         pkvsmob++;
      }
      else if
      (
         !IS_NPC(vch) &&
         !IS_NPC(vch->fighting)
      )
      {
         pkvspk++;
      }
   }

   if (pkvspk > 0)
   {
      whichtarget = number_range(1, pkvspk);
      mobvsmob = 0;
      pkvsmob  = 0;
   }
   else if (pkvsmob > 0)
   {
      whichtarget = number_range(1, pkvsmob);
      pkvspk   = 0;
      mobvsmob = 0;
   }
   else if (mobvsmob > 0)
   {
      whichtarget = number_range(1, mobvsmob);
      pkvspk   = 0;
      pkvsmob  = 0;
   }

   if (whichtarget == 0)
   {
      send_to_char
      (
         "You can detect no immediate need of your services.\n\r",
         ch
      );
      send_to_char("You failed.\n\r", ch);
      return;
   }

   for (vch = char_list; vch != NULL; vch = vch_next)
   {
      vch_next = vch->next;
      if
      (
         vch->fighting == NULL ||
         IS_SET(vch->in_room->extra_room_flags, ROOM_1212) ||
         IS_SET(vch->in_room->room_flags, ROOM_SAFE) ||
         IS_SET(vch->in_room->room_flags, ROOM_PRIVATE) ||
         IS_SET(vch->in_room->room_flags, ROOM_SOLITARY) ||
         (
            (
               IS_SET(vch->in_room->room_flags, ROOM_NO_RECALL) ||
               IS_SET(vch->in_room->extra_room_flags, ROOM_DEAD_ZONE)
            ) &&
            !IS_SET(vch->in_room->room_flags, ROOM_CONSECRATED)
         ) ||
         (
            !IS_NPC(vch) &&
            vch->level >= LEVEL_IMMORTAL
         ) ||
         (
            vch->in_room->house != 0 &&
            vch->in_room->house != ch->house
         ) ||
         (
            IS_NPC(vch) &&
            vch->pIndexData->pShop != NULL
         ) ||
         !(
            (
               vch->alignment > 0 &&
               vch->fighting->alignment < 0
            ) ||
            (
               vch->alignment < 0 &&
               vch->fighting->alignment > 0
            )
         ) ||
         (
            pkvspk > 0 &&
            (
               IS_NPC(vch) ||
               IS_NPC(vch->fighting)
            )
         ) ||
         (
            pkvsmob > 0 &&
            (
               IS_NPC(vch) ||
               !IS_NPC(vch->fighting)
            )
         ) ||
         (
            mobvsmob > 0 &&
            (
               !IS_NPC(vch) ||
               !IS_NPC(vch->fighting)
            )
         )
      )
      {
         continue;
      }
      count++;
      if (count == whichtarget)
      {
         break;
      }
   }

   send_to_char
   (
      "You are surrounded by a large beam of white light!\n\r"
      "The light around you fades, and you are elsewhere.\n\r",
      ch
   );
   act
   (
      "$n is surrounded by a large beam of white light and vanishes!",
      ch,
      NULL,
      NULL,
      TO_ROOM
   );
   char_from_room(ch);
   char_to_room(ch, vch->in_room);
   act
   (
      "A beam of white light from the sky strikes the ground, and $n steps"
      " forth!",
      ch,
      NULL,
      NULL,
      TO_ROOM
   );
   do_observe(ch, "", LOOK_AUTO);
   return;
}

/* RT really nasty high-level attack spell */
void spell_holy_word(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;
   int dam;
   int sanc_num;
   int wrath_num;

   if (check_peace(ch)) return;

   if (IS_GOOD(ch))
   wrath_num = gsn_wrath;
   else
   wrath_num = gsn_dark_wrath;

   if (is_supplicating)
   {
      sanc_num = gsn_sanctuary;
   }
   else
   {
      sanc_num = gsn_chromatic_shield;
   }

   act("$n utters a word of divine power!", ch, NULL, NULL, TO_ROOM);
   send_to_char("You utter a word of divine power.\n\r", ch);

   for ( vch = ch->in_room->people; vch != NULL; vch = vch_next )
   {
      vch_next = vch->next_in_room;
      if (IS_IMMORTAL(vch) && !can_see(ch, vch)) continue;
      if (oblivion_blink(ch, vch))
      {
         continue;
      }

      if ((IS_GOOD(ch) && IS_GOOD(vch)) ||
      (IS_EVIL(ch) && IS_EVIL(vch)) ||
      (IS_NEUTRAL(ch) && IS_NEUTRAL(vch)) )
      {
         if (clergy_fails_help(ch, vch, sanc_num))
         {
            continue;
         }
         send_to_char("You feel more powerful.\n\r", vch);
         magic_spell_vict
         (
            ch,
            vch,
            CAST_BITS_REUSE,
            level + 1,
            gsn_frenzy
         );
         magic_spell_vict
         (
            ch,
            vch,
            CAST_BITS_REUSE,
            level + 1,
            gsn_bless
         );
         magic_spell_vict
         (
            ch,
            vch,
            CAST_BITS_REUSE,
            level + 1,
            sanc_num
         );
      }

      else if ((IS_GOOD(ch) && IS_EVIL(vch)) ||
      (IS_EVIL(ch) && IS_GOOD(vch)) )
      {
         if (!is_safe(ch, vch, IS_SAFE_SILENT | IS_SAFE_AREA_ATTACK))
         {
            if (!IS_NPC(ch) && !IS_NPC(vch)
            && (ch->fighting == NULL || vch->fighting == NULL))
            {
               switch(number_range(0, 2))
               {
                  case (0):
                  case (1):
                  sprintf(buf, "Die, %s, you holy terror!", PERS(ch, vch));
                  break;
                  case (2):
                  sprintf(buf, "Help! %s is calling down the wrath of %s upon me!", PERS(ch, vch), get_god_name(ch, vch));
               }
               if (vch != ch)
               {
                  do_myell(vch, buf);
                  sprintf(log_buf, "[%s] cast holy word upon [%s] at %d", ch->name, vch->name, ch->in_room->vnum);
                  log_string(log_buf);
               }
            }

            magic_spell_vict
            (
               ch,
               vch,
               CAST_BITS_REUSE,
               level + 1,
               gsn_curse
            );
            send_to_char("You are struck down!\n\r", vch);
            magic_spell_vict
            (
               ch,
               vch,
               CAST_BITS_REUSE,
               level + 1,
               wrath_num
            );
            /*
            dam = dice(level, 6);
            damage(ch, vch, dam, sn, DAM_ENERGY, TRUE);
            */
         }
      }

      else if (IS_NEUTRAL(ch))
      {
         if (!is_safe(ch, vch, IS_SAFE_SILENT | IS_SAFE_AREA_ATTACK))
         {
            magic_spell_vict
            (
               ch,
               vch,
               CAST_BITS_REUSE,
               level / 2,
               gsn_bless
            );
            send_to_char("You are struck down!\n\r", vch);
            dam = dice(level, 4);
            damage(ch, vch, dam, sn, DAM_ENERGY, TRUE);
         }
      }
   }

   send_to_char("You feel drained.\n\r", ch);
   /*    ch->move /= 2;
   ch->hit /= 2;*/
   ch->hit = ch->hit*9.0/10;
}

void spell_identify( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   OBJ_DATA *obj = (OBJ_DATA *) vo;
   CHAR_DATA *dcheck; /* - Wicket */
   char buf[MAX_STRING_LENGTH];
   AFFECT_DATA *paf;
   int bonus = 0;
   sh_int counter = 0;
   char time_string[MAX_INPUT_LENGTH];

   if (ch->perm_stat[STAT_WIS] < 16)
   bonus = -1;
   if (ch->perm_stat[STAT_WIS] == 25)
   bonus = 3;
   if (ch->perm_stat[STAT_WIS] > 22 && ch->perm_stat[STAT_WIS] < 25)
   bonus = 2;
   if (ch->perm_stat[STAT_WIS] > 19 && ch->perm_stat[STAT_WIS] < 23)
   bonus = 1;

   if (!IS_NPC(ch) && ch->pcdata->special == SUBCLASS_ENCHANTER)
   bonus += 15;

   /*
   * If the caster has a house demon with them, they get a bonus of +5.
   * As per Rungekutta's request.
   * - Wicket
   */
   if ( !IS_NPC(ch) && ch->house == HOUSE_BRETHREN )
   for ( dcheck = char_list; dcheck != NULL; dcheck = dcheck->next )
   {
      if ( IS_NPC(dcheck)
      && (dcheck->master == ch)
      && (dcheck->pIndexData->vnum == MOB_VNUM_DEMON1
      ||  dcheck->pIndexData->vnum == MOB_VNUM_DEMON2
      ||  dcheck->pIndexData->vnum == MOB_VNUM_DEMON3
      ||  dcheck->pIndexData->vnum == MOB_VNUM_DEMON4
      ||  dcheck->pIndexData->vnum == MOB_VNUM_DEMON5
      ||  dcheck->pIndexData->vnum == MOB_VNUM_DEMON6) )
      {
         bonus += 5;
      }
   }

   if (obj->level - 3 > (level + bonus))
   {
      sprintf( buf,
      "Object '%s' is type %s.\n\rWeight is %d, value is %d.\n\rMaterial is %s.\n\r",
      obj->short_descr,
      item_name(obj->item_type),
      obj->weight / 10,
      obj->cost,
      (is_affected_obj(obj, gsn_obscure)) ?
      "unknown mixture":obj->material);
      send_to_char(buf, ch);
      return;
   }

   sprintf
   (
      buf,
      "Object '%s' is type %s, extra flags %s.\n\rWeight is %d, value is %d, level is %d.\n\rMaterial is %s.\n\r",

      obj->short_descr,
      item_name(obj->item_type),
      extra_bit_name( obj->extra_flags, obj->extra_flags2 ),
      obj->weight / 10,
      obj->cost, obj->level, (is_affected_obj(obj, gsn_obscure))?
      "unknown mixture":obj->material
   );
   send_to_char( buf, ch );

   sprintf
   (
      buf, "restrict flags %s.\n\r",
      restrict_bit_name(obj->pIndexData->restrict_flags)
   );
   send_to_char(buf, ch);

   switch ( obj->item_type )
   {
      case ITEM_SCROLL:
      case ITEM_POTION:
      case ITEM_HERB:
      case ITEM_PILL:
      if (obj->item_type==ITEM_HERB)
      {
         sprintf( buf, "Effects are:" );
      }
      else
      {
         sprintf( buf, "Level %d spells of:", obj->value[0] );
      }

      send_to_char( buf, ch );
      for (counter = 1; counter < 5; counter++)
      {
         if
         (
            obj->value[counter] > 0 &&
            obj->value[counter] < MAX_SKILL
         )
         {
            send_to_char( " '", ch );
            /*
            if the item is an herb, use the 'herb name' for the spell
            or skill, otherwise use the normal 'spell/skill' name
            for the spell/skill
            */
            if (obj->item_type == ITEM_HERB)
            {
               send_to_char(get_herb_spell_name(NULL, TRUE, obj->value[counter]), ch);
            }
            else
            {
               send_to_char(skill_table[obj->value[counter]].name, ch);
            }
            send_to_char( "'", ch );
         }
      }
      send_to_char( ".\n\r", ch );
      break;

      case ITEM_SPELLBOOK:
      send_to_char( "Boosts", ch);

      if
      (
         obj->value[2] > 0 &&
         obj->value[2] < MAX_SKILL
      )
      {
         send_to_char( " '", ch );
         send_to_char( skill_table[obj->value[2]].name, ch );
         send_to_char( "'", ch );
      }
      if
      (
         obj->value[3] > 0 &&
         obj->value[3] < MAX_SKILL
      )
      {
         send_to_char( ",  '", ch );
         send_to_char( skill_table[obj->value[3]].name, ch );
         send_to_char( "'", ch );
      }
      if
      (
         obj->value[4] > 0 &&
         obj->value[4] < MAX_SKILL
      )
      {
         send_to_char( ",  '", ch );
         send_to_char( skill_table[obj->value[4]].name, ch );
         send_to_char( "'", ch );
      }
      sprintf(buf, " spells by %d levels (Max level of %d).\n\r", obj->value[0], obj->value[1]);
      send_to_char( buf, ch );
      break;
      case ITEM_WAND:
      case ITEM_STAFF:
      if (obj->pIndexData->vnum == OBJ_VNUM_SOUL_ORB){
         sprintf( buf, "The soul orb contains %d hp, %d mana, and %d moves", obj->value[0], obj->value[1], obj->value[2]);
         send_to_char(buf, ch);
      }
      else
      {
         sprintf( buf, "Has %d charges of level %d",
         obj->value[2], obj->value[0] );
         send_to_char( buf, ch );


         if
         (
            obj->value[3] > 0 &&
            obj->value[3] < MAX_SKILL
         )
         {
            send_to_char( " '", ch );
            send_to_char( skill_table[obj->value[3]].name, ch );
            send_to_char( "'", ch );
         }
      }
      send_to_char( ".\n\r", ch );

      break;

      case ITEM_DRINK_CON:
      sprintf(buf, "It holds %s-colored %s.\n\r",
      liq_table[obj->value[2]].liq_color,
      liq_table[obj->value[2]].liq_name);
      send_to_char(buf, ch);
      break;

      case ITEM_CONTAINER:
      sprintf(buf, "Capacity: %d#  Maximum weight: %d#  flags: %s\n\r",
      obj->value[0], obj->value[3], cont_bit_name(obj->value[1]));
      send_to_char(buf, ch);
      break;

      case ITEM_WEAPON:
      send_to_char("Weapon type is ", ch);
      switch (obj->value[0])
      {
         case(WEAPON_BOW)    : send_to_char("bow.\n\r", ch);        break;
         case(WEAPON_ARROWS) : send_to_char("arrows.\n\r", ch);    break;
         case(WEAPON_EXOTIC) : send_to_char("exotic.\n\r", ch);    break;
         case(WEAPON_SWORD)  : send_to_char("sword.\n\r", ch);    break;
         case(WEAPON_DAGGER) : send_to_char("dagger.\n\r", ch);    break;
         case(WEAPON_SPEAR)    : send_to_char("spear/staff.\n\r", ch);    break;
         case(WEAPON_MACE)     : send_to_char("mace/club.\n\r", ch);    break;
         case(WEAPON_AXE)    : send_to_char("axe.\n\r", ch);        break;
         case(WEAPON_FLAIL)    : send_to_char("flail.\n\r", ch);    break;
         case(WEAPON_WHIP)    : send_to_char("whip.\n\r", ch);        break;
         case(WEAPON_POLEARM): send_to_char("polearm.\n\r", ch);    break;
         case (WEAPON_STAFF) : send_to_char("staff.\n\r", ch);    break;
         default        : send_to_char("unknown.\n\r", ch);    break;
      }
      if (obj->pIndexData->new_format)
      sprintf(buf, "Damage is %dd%d (average %d).\n\r",
      obj->value[1], obj->value[2],
      (1 + obj->value[2]) * obj->value[1] / 2);
      else
      sprintf( buf, "Damage is %d to %d (average %d).\n\r",
      obj->value[1], obj->value[2],
      ( obj->value[1] + obj->value[2] ) / 2 );
      send_to_char( buf, ch );
      if (obj->value[4])  /* weapon flags */
      {
         sprintf(buf, "Weapons flags: %s\n\r", weapon_bit_name(obj->value[4]));
         send_to_char(buf, ch);
      }
      break;

      case ITEM_ARMOR:
      sprintf( buf,
      "Armor class is %d pierce, %d bash, %d slash, and %d vs. magic.\n\r",
      obj->value[0], obj->value[1], obj->value[2], obj->value[3] );
      send_to_char( buf, ch );
      break;
   }

   if (!obj->enchanted)
   for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
   {
      if ( paf->location != APPLY_NONE && paf->modifier != 0 )
      {
         sprintf( buf, "Affects %s by %d.\n\r",
         affect_loc_name( paf->location ), paf->modifier );
         send_to_char(buf, ch);
         if
         (
            paf->bitvector ||
            paf->bitvector2
         )
         {
            switch(paf->where)
            {
               case TO_AFFECTS:
               sprintf(buf, "Adds %s affect.\n",
               affect_bit_name(paf->bitvector, paf->bitvector2));
               break;
               case TO_OBJECT:
               sprintf(buf, "Adds %s object flag.\n",
               extra_bit_name(paf->bitvector, paf->bitvector2));
               break;
               case TO_IMMUNE:
               sprintf(buf, "Adds immunity to %s.\n",
               imm_bit_name(paf->bitvector));
               break;
               case TO_RESIST:
               sprintf(buf, "Adds resistance to %s.\n\r",
               imm_bit_name(paf->bitvector));
               break;
               case TO_VULN:
               sprintf(buf, "Adds vulnerability to %s.\n\r",
               imm_bit_name(paf->bitvector));
               break;
               default:
               sprintf(buf, "Unknown bit %d: %d\n\r",
               paf->where, paf->bitvector);
               break;
            }
            send_to_char( buf, ch );
         }
      }
   }

   for ( paf = obj->affected; paf != NULL; paf = paf->next )
   {
      if ( paf->location != APPLY_NONE && paf->modifier != 0 )
      {
         sprintf( buf, "Affects %s by %d",
         affect_loc_name( paf->location ), paf->modifier );
         send_to_char( buf, ch );
         if ( paf->duration > -1)
         sprintf
         (
            buf,
            ",%s.\n\r",
            get_time_string
            (
               ch,
               ch,
               paf,
               NULL,
               time_string,
               TYPE_TIME_IDENTIFY,
               !(IS_SET(ch->comm2, COMM_STAT_TIME))
            )
         );
         else
         sprintf(buf, ".\n\r");
         send_to_char(buf, ch);
         if
         (
            paf->bitvector ||
            paf->bitvector2
         )
         {
            switch(paf->where)
            {
               case TO_AFFECTS:
               sprintf(buf, "Adds %s affect.\n",
               affect_bit_name(paf->bitvector, paf->bitvector2));
               break;
               case TO_OBJECT:
               sprintf(buf, "Adds %s object flag.\n",
               extra_bit_name(paf->bitvector, paf->bitvector2));
               break;
               case TO_WEAPON:
               sprintf(buf, "Adds %s weapon flags.\n",
               weapon_bit_name(paf->bitvector));
               break;
               case TO_IMMUNE:
               sprintf(buf, "Adds immunity to %s.\n",
               imm_bit_name(paf->bitvector));
               break;
               case TO_RESIST:
               sprintf(buf, "Adds resistance to %s.\n\r",
               imm_bit_name(paf->bitvector));
               break;
               case TO_VULN:
               sprintf(buf, "Adds vulnerability to %s.\n\r",
               imm_bit_name(paf->bitvector));
               break;
               default:
               sprintf(buf, "Unknown bit %d: %d\n\r",
               paf->where, paf->bitvector);
               break;
            }
            send_to_char(buf, ch);
         }
      }
   }

   return;
}



void spell_infravision( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;

   if ( IS_AFFECTED(victim, AFF_INFRARED) )
   {
      if (victim == ch)
      send_to_char("You can already see in the dark.\n\r", ch);
      else
      act("$N already has infravision.", ch, NULL, victim, TO_CHAR);
      return;
   }
   act( "$n's eyes glow red.", victim, NULL, NULL, TO_ROOM );

   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.duration  = 2 * level;
   af.location  = APPLY_NONE;
   af.modifier  = 0;
   af.bitvector = AFF_INFRARED;
   affect_to_char( victim, &af );
   send_to_char( "Your eyes glow red.\n\r", victim );
   return;
}



void spell_invis( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *victim;
   AFFECT_DATA af;
   int sn_fog, sn_fire;

   if (target == TARGET_OBJ)
   {
      send_to_char("You must be crazy.\n\r", ch);
      return;
   }
   /* character invisibility */
   victim = (CHAR_DATA *) vo;

   sn_fog = gsn_faerie_fog;
   sn_fire = gsn_faerie_fire;
   if (is_affected(victim, sn_fog) || is_affected(victim, sn_fire) || IS_AFFECTED(victim, AFF_FAERIE_FIRE))
   {
      if (victim == ch)
      send_to_char("You can't turn invisible while glowing.\n\r", ch);
      else
      act("$E can't turn invisible while glowing.", ch, NULL, victim, TO_CHAR);
      /*        send_to_char("They can't turn invisible while glowing.\n\r", ch); */
      return;
   }

   if ( IS_AFFECTED(victim, AFF_INVISIBLE) )
   {
      if (victim == ch)
      send_to_char("You are already invisible.\n\r", ch);
      else
      act("$E is already invisible.", ch, NULL, victim, TO_CHAR);
      /*        send_to_char("They are already invisible.\n\r", ch); */
      return;
   }

   act( "$n fades out of existence.", victim, NULL, NULL, TO_ROOM );

   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.duration  = level + 12;
   af.location  = APPLY_NONE;
   af.modifier  = 0;
   af.bitvector = AFF_INVISIBLE;
   affect_to_char( victim, &af );
   send_to_char( "You fade out of existence.\n\r", victim );
   return;
}



void spell_know_alignment(int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   char *msg;
   int ap;

   ap = victim->alignment;

   if ( ap >  0 ) msg = "$N has a pure and good aura.";
   else if ( ap == 0 ) msg = "$N doesn't have a firm moral commitment.";
   else msg = "$N is the embodiment of pure evil!.";

   act( msg, ch, NULL, victim, TO_CHAR );

   if (!IS_NPC(victim))
   ap = victim->pcdata->ethos;

   if ( ap > 0 ) msg = "$N uses methodical planning.";
   else if ( ap == 0 ) msg = "$N neutrally makes decisions.";
   else msg = "$N acts on impulse.";

   act( msg, ch, NULL, victim, TO_CHAR );
   return;
}

void spell_health(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   AFFECT_DATA af;

   if (house_down(ch, HOUSE_VALOR))
   return;

   if (is_affected(ch, sn) )
   {
      send_to_char("You've already received the blessing of the resistance.\n\r", ch);
      return;
   }
   /*
   if (ch->mana < (ch->max_mana-5)){
   send_to_char("You are not mentally prepared to receive the health blessing.\n\r", ch);
   return;
   }
   */

   af.where = TO_AFFECTS;

   af.location = APPLY_HIT;
   af.type = sn;
   af.duration = 12;
   af.modifier = (IS_NPC(ch) ? ch->max_mana : ch->pcdata->perm_mana)/2;
   af.bitvector = 0;
   af.level = level;
   affect_to_char(ch, &af);

   af.location = APPLY_MANA;
   af.duration = 12;
   /* Perm mana loss is now at 25% instead of 50% - Wicket */
   af.modifier = -(IS_NPC(ch) ? ch->max_mana : ch->pcdata->perm_mana)*25/100;
   affect_to_char(ch, &af);

   ch->hit += (IS_NPC(ch) ? ch->max_mana : ch->pcdata->perm_mana)/2;
   ch->mana -= ch->mana*25/100;
   send_to_char("The blessing of the resistance boosts your health!\n\r", ch);
   return;
}


void spell_channel(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   AFFECT_DATA af;

   if (house_down(ch, HOUSE_CONCLAVE))
   {
      return;
   }

   if (is_affected(ch, sn))
   {
      send_to_char("You are already as healthy as you can get.\n\r", ch);
      return;
   }
   if (ch->hit > ch->max_hit)
   {
      send_to_char("Your mind is already overflowing with health.\n\r", ch);
      return;
   }

   af.where = TO_AFFECTS;
   af.location = APPLY_HIT;
   af.type = sn;
   af.duration = 24;
   af.modifier = (IS_NPC(ch) ? ch->max_mana : ch->pcdata->perm_mana) * 75 / 100;
   af.bitvector = 0;
   af.level = level;
   ch->hit += af.modifier;
   affect_to_char(ch, &af);

   send_to_char
   (
      "You feel your health improve as you control your body with your mind."
      "\n\r",
      ch
   );
   return;
}


void spell_lightning_bolt(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   static const sh_int dam_each[] =
   {
      0,
      0,  0,  0,  0,  0,     0,  0,  0, 25, 28,
      31, 34, 37, 40, 40,    41, 42, 42, 43, 44,
      44, 45, 46, 46, 47,    48, 48, 49, 50, 50,
      51, 52, 52, 53, 54,    54, 55, 56, 56, 57,
      58, 58, 59, 60, 60,    61, 62, 62, 63, 64,
      65, 66, 67, 68, 69,    70, 71, 72, 73, 74
   };
   int dam;

   if (check_peace(ch)) return;

   if (is_affected(ch, gsn_grounding))
   {
      send_to_char("The electricity fizzles at your feet.\n\r", ch);
      return;
   }

   level = UMAX(0, level)*race_adjust(ch)/25.0;
   level = UMIN(level, (int)(sizeof(dam_each)/sizeof(dam_each[0]) - 1));
   level = UMAX(0, level);
   dam        = number_range( dam_each[level] / 2, dam_each[level] * 2 );
   if (dam > 148) dam = 148;

   if (check_spellcraft(ch, sn))
   dam += level;

   if ( saves_spell( ch, level, victim, DAM_LIGHTNING, SAVE_SPELL) )
   dam /= 2;
   damage( ch, victim, dam, sn, DAM_LIGHTNING , TRUE);
   return;
}



void spell_locate_object(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   char buf[MAX_INPUT_LENGTH];
   BUFFER* buffer;
   OBJ_DATA* obj;
   OBJ_DATA* in_obj;
   bool found;
   int number = 0;
   int max_found;

   found = FALSE;
   number = 0;
   max_found = IS_IMMORTAL(ch) ? 200 : 2 * level;

   buffer = new_buf();

   for (obj = object_list; obj != NULL; obj = obj->next)
   {
      if
      (
         !can_see_obj(ch, obj) ||
         !is_name(target_name, obj->name) ||
         IS_OBJ_STAT(obj, ITEM_NOLOCATE) ||
         number_percent() > 2 * level ||
         ch->level < obj->level ||
         (
            obj->carried_by != NULL &&
            IS_IMMORTAL(obj->carried_by) &&
            get_trust(ch) < get_trust(obj->carried_by)
         ) ||
         obj->short_descr == NULL ||
         obj->short_descr[0] == '\0' ||
         obj->description == NULL ||
         obj->description[0] == '\0'
      )
      {
         continue;
      }

      if (obj->newcost > 0)
      {
         continue;
      }


      for ( in_obj = obj; in_obj->in_obj != NULL; in_obj = in_obj->in_obj )
      {
         /* Find the outermost object */
      }
      if
      (
         in_obj->carried_by != NULL &&
         IS_IMMORTAL(in_obj->carried_by) &&
         get_trust(ch) < get_trust(in_obj->carried_by)
      )
      {
         /* Prevent locating objects on immortals */
         continue;
      }
      if
      (
         in_obj->carried_by == NULL ||
         in_obj->carried_by == ch ||
         (
            IS_TRUSTED(ch, ANGEL) &&
            get_trust(ch) >= get_trust(in_obj->carried_by)
         ) ||
         !is_affected(in_obj->carried_by, gsn_cloak_form)
      )
      {
         found = TRUE;
         number++;

         if (in_obj->carried_by != NULL && can_see(ch, in_obj->carried_by))
         {
            sprintf( buf, "%s is carried by %s\n\r",
            obj->short_descr, PERS(in_obj->carried_by, ch) );
         }
         else
         {
            if
            (
               IS_IMMORTAL(ch) &&
               in_obj->in_room != NULL
            )
            {
               sprintf
               (
                  buf,
                  "%s is in %s [Room %d]\n\r",
                  obj->short_descr,
                  in_obj->in_room->name,
                  in_obj->in_room->vnum
               );
            }
            else
            {
               sprintf
               (
                  buf,
                  "%s is in %s\n\r",
                  obj->short_descr,
                  (
                     in_obj->in_room == NULL ?
                     "somewhere" :
                     in_obj->in_room->name
                  )
               );
            }
         }
         buf[0] = UPPER(buf[0]);
         add_buf(buffer, buf);
      }
      if (number >= max_found)
      {
         break;
      }
   }

   if (!found)
   {
      send_to_char("Nothing like that in heaven or earth.\n\r", ch);
   }
   else
   {
      page_to_char(buf_string(buffer), ch);
   }
   free_buf(buffer);
   return;
}



void spell_magic_missile( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   int dam;
   int dtype = DAM_ENERGY;

   /* new damage formula - werv */
   dam = number_range(1+level/5, level+5);
   if (check_spellcraft(ch, sn))
   dam = dam + 25;

   if (!IS_NPC(ch))
   {
      if (ch->class == CLASS_ELEMENTALIST)
      switch(number_range(1, 6))
      {
         case 1: dtype = DAM_FIRE; break;
         case 2: dtype = DAM_COLD; break;
         case 3: dtype = DAM_SLASH; break;
         case 4: dtype = DAM_LIGHTNING; break;
         case 5: dtype = DAM_BASH; break;
         case 6: dtype = DAM_DROWNING; break;
      }
      if (ch->pcdata->special == SUBCLASS_PYROMANCER)
      dtype = DAM_FIRE;
      if (ch->pcdata->special == SUBCLASS_AREOMANCER)
      {
         if (number_range(1, 2) == 1)
         dtype = DAM_SLASH;
         else
         dtype = DAM_LIGHTNING;
      }
      if (ch->pcdata->special == SUBCLASS_AQUAMANCER)
      {
         if (number_range(1, 2) == 1)
         dtype = DAM_DROWNING;
         else
         dtype = DAM_COLD;
      }
      if (ch->pcdata->special == SUBCLASS_GEOMANCER)
      dtype = DAM_BASH;
   }
   if ( saves_spell( ch, level, victim, dtype, SAVE_SPELL) )
   dam /= 2;
   damage( ch, victim, dam, sn, dtype , TRUE);
   return;
}

void spell_mass_healing(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *gch;
   int heal_num, refresh_num;

   heal_num = gsn_heal;
   refresh_num = gsn_refresh;

   for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
   {
      if (IS_IMMORTAL(gch) && !can_see(ch, gch)) continue;
      if
      (
         ch != gch &&
         is_safe(ch, gch, IS_SAFE_SILENT | IS_SAFE_AREA_ATTACK)
      )
      {
         continue;
      }
      if ((IS_NPC(ch) && IS_NPC(gch)) ||
      (!IS_NPC(ch) && !IS_NPC(gch)))
      {
         if (clergy_fails_help(ch, gch, heal_num))
         {
            continue;
         }
         if (oblivion_blink(ch, gch))
         {
            continue;
         }
         magic_spell_vict
         (
            ch,
            gch,
            CAST_BITS_REUSE,
            level,
            gsn_heal
         );
         magic_spell_vict
         (
            ch,
            gch,
            CAST_BITS_REUSE,
            level,
            gsn_refresh
         );
      }
   }
}


void spell_mass_invis( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   AFFECT_DATA af;
   CHAR_DATA *gch;
   int sn_fog, sn_fire;

   sn_fog = gsn_faerie_fog;
   sn_fire = gsn_faerie_fire;

   for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
   {
      if ( !is_same_group( gch, ch ) || IS_AFFECTED(gch, AFF_INVISIBLE) )
      continue;
      if
      (
         ch != gch &&
         is_safe(ch, gch, IS_SAFE_SILENT | IS_SAFE_AREA_ATTACK)
      )
      {
         continue;
      }
      if (is_affected(gch, sn_fog) || is_affected(gch, sn_fire))
      {
         send_to_char("You can't turn invisible while glowing.\n\r", gch);
         continue;
      }
      if (oblivion_blink(ch, gch))
      {
         continue;
      }

      act( "$n slowly fades out of existence.", gch, NULL, NULL, TO_ROOM );
      send_to_char( "You slowly fade out of existence.\n\r", gch );

      af.where     = TO_AFFECTS;
      af.type      = sn;
      af.level     = level/2;
      af.duration  = 24;
      af.location  = APPLY_NONE;
      af.modifier  = 0;
      af.bitvector = AFF_INVISIBLE;
      affect_to_char( gch, &af );
   }

   return;
}



void spell_null( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   send_to_char( "That's not a spell!\n\r", ch );
   return;
}



void spell_pass_door( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;

   if ( IS_AFFECTED(victim, AFF_PASS_DOOR) )
   {
      if (victim == ch)
      send_to_char("You are already out of phase.\n\r", ch);
      else
      act("$N is already shifted out of phase.", ch, NULL, victim, TO_CHAR);
      return;
   }

   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.duration  = number_fuzzy( level / 4 )*race_adjust(ch)/25.0;
   af.location  = APPLY_NONE;
   af.modifier  = 0;
   af.bitvector = AFF_PASS_DOOR;
   affect_to_char( victim, &af );
   act( "$n turns translucent.", victim, NULL, NULL, TO_ROOM );
   send_to_char( "You turn translucent.\n\r", victim );
   return;
}

/* RT plague spell, very nasty */

void spell_plague( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;
   AFFECT_DATA *af2;
   int lev;

   if (!IS_NPC(ch) && ch->pcdata->special == SUBCLASS_CORRUPTOR)
   lev = level+5;
   else
   lev = level;

   if (saves_spell(ch, lev, victim, DAM_DISEASE, SAVE_MALEDICT) ||
   (IS_NPC(victim) && IS_SET(victim->act, ACT_UNDEAD))
   || (IS_NPC(victim) && victim->pIndexData->vnum == MOB_VNUM_CRUSADER))
   {
      if (ch == victim)
      send_to_char("You feel momentarily ill, but it passes.\n\r", ch);
      else
      act("$N seems to be unaffected.", ch, NULL, victim, TO_CHAR);
      return;
   }

   af.where     = TO_AFFECTS;
   af.type       = sn;
   af.level      = lev * 3/4;
   af.duration  = lev;
   af.location  = APPLY_STR;
   af.modifier  = -5;
   af.bitvector = AFF_PLAGUE;
   affect_join(victim, &af, 10);
   af2 = affect_find(victim->affected, gsn_plague);
   if (af2 != NULL)
   {
      free_string(af2->caster);
      af2->caster = str_dup(ch->name);
   }

   send_to_char
   ("You scream in agony as plague sores erupt from your skin.\n\r", victim);
   act("$n screams in agony as plague sores erupt from $s skin.",
   victim, NULL, NULL, TO_ROOM);
}

void spell_poison( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *victim;
   OBJ_DATA *obj;
   AFFECT_DATA af;
   AFFECT_DATA *af2;
   char buf[MAX_STRING_LENGTH];
   int lev;

   if (ch == NULL) return;

   if (!IS_NPC(ch) && ch->pcdata->special == SUBCLASS_CORRUPTOR)
   lev = level+5;
   else
   lev = level;


   if (target == TARGET_OBJ)
   {
      obj = (OBJ_DATA *) vo;

      if (obj == NULL) return;

      sprintf(buf, "%s poisons %s [%d]", ch->name, obj->short_descr,
      obj->pIndexData->vnum);
      log_string(buf);


      if (obj->item_type == ITEM_FOOD || obj->item_type == ITEM_DRINK_CON)
      {
         if (IS_OBJ_STAT(obj, ITEM_BLESS) || IS_OBJ_STAT(obj, ITEM_BURN_PROOF))
         {
            act("Your fail to corrupt $p.", ch, obj, NULL, TO_CHAR);
            return;
         }
         obj->value[3] = 1;
         act("$p is infused with poisonous vapors.", ch, obj, NULL, TO_ALL);
         return;
      }

      if (obj->item_type == ITEM_WEAPON)
      {
         if
         (   IS_WEAPON_STAT(obj, WEAPON_FLAMING) ||
             IS_WEAPON_STAT(obj, WEAPON_FROST) ||
             IS_WEAPON_STAT(obj, WEAPON_VAMPIRIC) ||
             IS_WEAPON_STAT(obj, WEAPON_SHARP) ||
             IS_WEAPON_STAT(obj, WEAPON_VORPAL) ||
             IS_WEAPON_STAT(obj, WEAPON_SHOCKING) ||
             IS_WEAPON_STAT(obj, WEAPON_DROWNING) ||
             IS_WEAPON_STAT(obj, WEAPON_RESONATING) ||
             IS_OBJ_STAT(obj, ITEM_BLESS) ||
             IS_OBJ_STAT(obj, ITEM_BURN_PROOF)
         )
         {
            act
            (
               "You cannot seem to envenom $p.",
                ch,
                obj,
                NULL,
                TO_CHAR
            );
            return;
         }

         if (IS_WEAPON_STAT(obj, WEAPON_POISON))
         {
            act("$p is already envenomed.", ch, obj, NULL, TO_CHAR);
            return;
         }

         af.where     = TO_WEAPON;
         af.type     = sn;
         af.level     = lev / 2;
         af.duration     = lev / 8;
         af.location     = 0;
         af.modifier     = 0;
         af.bitvector = WEAPON_POISON;
         affect_to_obj(obj, &af);

         act("$p is coated with deadly venom.", ch, obj, NULL, TO_ALL);
         return;
      }

      act("You can't poison $p.", ch, obj, NULL, TO_CHAR);
      return;
   }

   victim = (CHAR_DATA *) vo;

   if (victim == NULL) return;

   if ( saves_spell( ch, lev, victim, DAM_POISON, SAVE_MALEDICT) )
   {
      act("$n turns slightly green, but it passes.", victim, NULL, NULL, TO_ROOM);
      send_to_char("You feel momentarily ill, but it passes.\n\r", victim);
      return;
   }

   if (IS_NPC(victim) && victim->pIndexData->vnum == MOB_VNUM_CRUSADER)
   {
      act("$n turns slightly green, but it passes.", victim, NULL, NULL, TO_ROOM);
      return;
   }

   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = lev;
   af.duration  = lev/8;
   af.location  = APPLY_STR;
   af.modifier  = -2;
   af.bitvector = AFF_POISON;
   affect_join( victim, &af, 10 );
   af2 = affect_find(victim->affected, gsn_poison);
   if (af2 != NULL)
   {
      free_string(af2->caster);
      if
      (
         IS_NPC(ch) &&
         IS_AFFECTED(ch, AFF_CHARM) &&
         ch->master &&
         !IS_NPC(ch->master)
      )
      {
         af2->caster = str_dup(ch->master->name);
      }
      else
      {
         af2->caster = str_dup(ch->name);
      }
   }

   send_to_char( "You feel very sick.\n\r", victim );
   act("$n looks very ill.", victim, NULL, NULL, TO_ROOM);
   return;
}

void spell_power_word_stun(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;

   if (is_affected(victim, gsn_power_word_stun) )
   {
      act("$E is already stunned from a word of power.", ch, NULL, victim, TO_CHAR);
      /*    send_to_char("They are already stunned from another word of power.\n\r", ch); */
      return;
   }
   act("$n gestures at $N and utters the word, 'Stun'.", ch, NULL, victim, TO_NOTVICT);
   act("$n gestures at you and utters the word, 'Stun'.", ch, NULL, victim, TO_VICT);
   act("You gesture at $N and invoke a word of dark power.", ch, NULL, victim, TO_CHAR);

   if (saves_spell(ch, level, victim, DAM_NEGATIVE, SAVE_DEATH) ||
   saves_spell(ch, level, victim, DAM_NEGATIVE, SAVE_DEATH) ||
   saves_spell(ch, level, victim, DAM_NEGATIVE, SAVE_DEATH) )
   {
      act("$n reels from the dark energy but resists the power.", victim, NULL, NULL, TO_ROOM);
      act("You feel the power blast at you, but you resist the stun.", victim, NULL, NULL, TO_CHAR);
      return;
   }

   af.where = TO_AFFECTS;
   af.type = gsn_power_word_stun;
   af.location = APPLY_HITROLL;
   af.modifier = -4;
   af.duration = 1;
   af.bitvector = 0;
   af.level = level;
   affect_to_char(victim, &af);
   af.location = APPLY_DEX;
   af.modifier = -3;
   affect_to_char(victim, &af);

   act("The word of power sends $N to the ground with stunning force!", ch, NULL, victim, TO_NOTVICT);
   act("$N is crushed to the floor by your word of power!", ch, NULL, victim, TO_CHAR);
   send_to_char
   (
      "You are sent crashing to the ground as the word hits you!\n\r",
      victim
   );
   victim->position = POS_RESTING;
   return;
}



void spell_protection_evil(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;
   OBJ_DATA *brand;

   if (((brand = get_eq_char(ch, WEAR_BRAND)) != NULL)
   && (brand->pIndexData->vnum == 433))
   {
      send_to_char("The gods have forsaken you.\n\r", ch);
      return;
   }

   if (((brand = get_eq_char(victim, WEAR_BRAND)) != NULL)
   && (brand->pIndexData->vnum == 433))
   {
      send_to_char("The gods have forsaken you.\n\r", victim);
      if (victim != ch)
      send_to_char("The gods have forsaken them.\n\r", ch);
      return;
   }

   if (IS_EVIL(victim)) {
      send_to_char("Your soul rejects the purity.\n\r", victim);
      return;
   }

   if ( IS_AFFECTED(victim, AFF_PROTECT_EVIL)
   ||   IS_AFFECTED(victim, AFF_PROTECT_GOOD))
   {
      if (victim == ch)
      send_to_char("You are already protected.\n\r", ch);
      else
      act("$N is already protected.", ch, NULL, victim, TO_CHAR);
      return;
   }

   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.duration  = 24*race_adjust(ch)/25.0;
   af.location  = APPLY_SAVING_SPELL;
   af.modifier  = -1;
   af.bitvector = AFF_PROTECT_EVIL;
   affect_to_char( victim, &af );
   send_to_char( "You feel holy and pure.\n\r", victim );
   if ( ch != victim )
   act("$N is protected from evil.", ch, NULL, victim, TO_CHAR);
   return;
}

void spell_protection_good(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;
   OBJ_DATA *brand;

   if (((brand = get_eq_char(ch, WEAR_BRAND)) != NULL)
   && (brand->pIndexData->vnum == 433))
   {
      send_to_char("The gods have forsaken you.\n\r", ch);
      return;
   }

   if (((brand = get_eq_char(victim, WEAR_BRAND)) != NULL)
   && (brand->pIndexData->vnum == 433))
   {
      send_to_char("The gods have forsaken you.\n\r", victim);
      if (victim != ch)
      send_to_char("The gods have forsaken them.\n\r", ch);
      return;
   }

   if (IS_GOOD(victim)) {
      send_to_char("Your soul rejects the corruption.\n\r", victim);
      return;
   }

   if ( IS_AFFECTED(victim, AFF_PROTECT_GOOD)
   ||   IS_AFFECTED(victim, AFF_PROTECT_EVIL))
   {
      if (victim == ch)
      send_to_char("You are already protected.\n\r", ch);
      else
      act("$N is already protected.", ch, NULL, victim, TO_CHAR);
      return;
   }

   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.duration  = 24*race_adjust(ch)/25.0;
   af.location  = APPLY_SAVING_SPELL;
   af.modifier  = -1;
   af.bitvector = AFF_PROTECT_GOOD;
   affect_to_char( victim, &af );
   send_to_char( "You feel aligned with darkness.\n\r", victim );
   if ( ch != victim )
   act("$N is protected from good.", ch, NULL, victim, TO_CHAR);
   return;
}


void spell_ray_of_truth (int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   int dam, align;

   if (IS_EVIL(ch) )
   {
      victim = ch;
      send_to_char("The energy explodes inside you!\n\r", ch);
   }

   if (victim != ch)
   {
      act("$n raises $s hand, and a blinding ray of light shoots forth!",
      ch, NULL, NULL, TO_ROOM);
      send_to_char(
      "You raise your hand and a blinding ray of light shoots forth!\n\r",
      ch);
   }

   if (IS_GOOD(victim))
   {
      act("$n seems unharmed by the light.", victim, NULL, victim, TO_ROOM);
      send_to_char("The light seems powerless to affect you.\n\r", victim);
      return;
   }

   dam = dice( level, 10 );
   if ( saves_spell( ch, level, victim, DAM_HOLY, SAVE_SPELL) )
   dam /= 2;

   align = victim->alignment;
   align -= 350;

   if (align < -1000)
   align = -1000 + (align + 1000) / 3;

   dam = (dam * align * align) / 1000000;

   magic_spell_vict
   (
      ch,
      victim,
      CAST_BITS_REUSE,
      3 * level / 4,
      gsn_blindness
   );
   damage( ch, victim, dam*race_adjust(ch)/25.0, sn, DAM_HOLY , TRUE);
}


void spell_recharge( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   OBJ_DATA *obj = (OBJ_DATA *) vo;
   int chance, percent;
   int charges_restored;

   /* recoded Recharge - Werv */

   if (obj->item_type != ITEM_WAND && obj->item_type != ITEM_STAFF)
   {
      send_to_char("That item does not carry charges.\n\r", ch);
      return;
   }

   if (obj->level > level)
   {
      /* Cant recharge items higher than yourself */
      send_to_char("Your skills are not great enough for that.\n\r", ch);
      return;
   }

   chance = 50 + 3 * (level - obj->level);
   percent = number_percent();

   if (obj->value[0] <= 1)
   chance = 0;

   if (percent < chance*race_adjust(ch)/25.0)
   {
      charges_restored = number_percent();
      charges_restored = UMAX(4, charges_restored);

      if (charges_restored > 95){
         charges_restored = 3;
         act("$p glows intensely.", ch, obj, NULL, TO_CHAR);
         act("$p glows intensely.", ch, obj, NULL, TO_ROOM);
      }

      if (charges_restored > 80){
         charges_restored = 2;
         act("$p glows brightly.", ch, obj, NULL, TO_CHAR);
         act("$p glows brightly.", ch, obj, NULL, TO_ROOM);
      }

      if (charges_restored > 3){
         charges_restored = 1;
         act("$p glows softly.", ch, obj, NULL, TO_CHAR);
         act("$p glows softly.", ch, obj, NULL, TO_ROOM);
      }

      /*
      * Decrement spell level, increment stave/wand level and
      * do the recharge
      */

      obj->value[0] = obj->value[0] - 1;
      obj->level = obj->level + 3 + charges_restored;
      obj->value[2] = obj->value[2] + charges_restored;

      if ( obj->value[2] > obj->value[1])
      /* Do not allow more than max charges */
      obj->value[2] = obj->value[1];
      return;
   }
   else /* whoops! */
   {
      act("$p glows brightly and explodes!", ch, obj, NULL, TO_CHAR);
      act("$p glows brightly and explodes!", ch, obj, NULL, TO_ROOM);
      extract_obj(obj, FALSE);
   }
}

void spell_refresh( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   int move_add = 50;
   int chance = level;

   if (!IS_NPC(ch)){
      chance += ch->pcdata->learned[sn]/2;
   }
   if (number_percent() < chance) move_add += 50;
   victim->move = UMIN( victim->move + move_add*race_adjust(ch)/25.0, victim->max_move );
   if (victim->max_move == victim->move)
   send_to_char("You feel fully refreshed!\n\r", victim);
   else
   send_to_char( "You feel less tired.\n\r", victim );
   if ( ch != victim )
   send_to_char( "You cause them to feel less tired.\n\r", ch );
   return;
}

void spell_remove_curse(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   CHAR_DATA* victim;
   OBJ_DATA* obj;
   OBJ_DATA* brand;
   bool found = FALSE;
   AFFECT_DATA* paf;

   if
   (
      (
         (
            brand = get_eq_char(ch, WEAR_BRAND)
         ) != NULL
      ) &&
      brand->pIndexData->vnum == 433
   )
   {
      send_to_char("The gods have forsaken you.\n\r", ch);
      return;
   }

   /* do object cases first */
   if (target == TARGET_OBJ)
   {
      obj = (OBJ_DATA*) vo;

      if (obj->pIndexData->vnum == OBJ_VNUM_SWORD_DEMONS)
      {
         if (ch->class == CLASS_PALADIN)
         {
            act("$p shatters!.", ch, obj, NULL, TO_ALL);
            extract_obj(obj, FALSE);
         }
         else
         {
            send_to_char
            (
               "The evil within the Sword can only be destroyed by one of"
               " purity.\n\r",
               ch
            );
            send_to_char
            (
               "The demons within the Sword laugh at your efforts!\n\r",
               ch
            );
         }
         return;
      }
      if
      (
         IS_OBJ_STAT(obj, ITEM_NODROP) ||
         IS_OBJ_STAT(obj, ITEM_NOREMOVE)
      )
      {
         if
         (
            !IS_OBJ_STAT(obj, ITEM_NOUNCURSE) &&
            !saves_dispel((level + 2), obj->level, 0)
         )
         {
            REMOVE_BIT(obj->extra_flags, ITEM_NODROP);
            REMOVE_BIT(obj->extra_flags, ITEM_NOREMOVE);
            act("$p glows blue.", ch, obj, NULL, TO_ALL);
            return;
         }
         act("The curse on $p is beyond your power.", ch, obj, NULL, TO_CHAR);
         return;
      }
      act("There does not seem to be a curse on $p.", ch, obj, NULL, TO_CHAR);
      return;
   }

   /* characters */
   victim = (CHAR_DATA*) vo;

   paf = victim->affected;
   while (paf)
   {
      if
      (
         paf->type == gsn_curse &&
         paf->duration != -1
      )
      {
         break;
      }
      paf = paf->next;
   }
   if (paf != NULL)
   {
      if (check_dispel(level, victim, gsn_curse))
      {
         send_to_char("You feel better.\n\r", victim);
         act("$n looks more relaxed.", victim, NULL, NULL, TO_ROOM);
      }
      else
      {
         send_to_char("You failed to remove the curse.\n\r", ch);
      }
      return;
   }

   for
   (
      obj = victim->carrying;
      (
         obj != NULL &&
         !found
      );
      obj = obj->next_content
   )
   {
      if (obj->pIndexData->vnum == OBJ_VNUM_SWORD_DEMONS)
      {
         if (ch->class == CLASS_PALADIN)
         {
            act("$p shatters!.", victim, obj, NULL, TO_ALL);
            extract_obj(obj, FALSE);
            found = TRUE;
         }
         else
         {
            send_to_char
            (
               "The evil within the Sword can only be destroyed by one of"
               " purity.\n\r",
               ch
            );
            send_to_char
            (
               "The demons within the Sword laugh at your efforts!\n\r",
               ch
            );
         }
         continue;
      }
      if
      (
         (
            IS_OBJ_STAT(obj, ITEM_NODROP) ||
            IS_OBJ_STAT(obj, ITEM_NOREMOVE)
         ) &&
         !IS_OBJ_STAT(obj, ITEM_NOUNCURSE) &&
         !saves_dispel(level, obj->level, 0)
      )
      {
         /* attempt to remove curse */
         found = TRUE;
         REMOVE_BIT(obj->extra_flags, ITEM_NODROP);
         REMOVE_BIT(obj->extra_flags, ITEM_NOREMOVE);
         act("Your $p glows blue.", victim, obj, NULL, TO_CHAR);
         act("$n's $p glows blue.", victim, obj, NULL, TO_ROOM);
      }
   }
}


void spell_sanctuary(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   CHAR_DATA* victim = (CHAR_DATA*) vo;
   AFFECT_DATA af;
   OBJ_DATA* brand;

   if (!is_supplicating)
   {
      if (!IS_IMMORTAL(ch))
      {
         send_to_char("Only clergy may use this prayer.\n\r", ch);
         sprintf(log_buf, "spell_sanctuary, not supplicating: %s", ch->name);
         bug(log_buf, 0);
      }
      else
      {
         send_to_char
         (
            "Only clergy may use this prayer,\n\r"
            "Use Chromatic Shield instead.\n\r",
            ch
         );
      }
      return;
   }
   if (is_affected(victim, gsn_sanctuary))
   {
      if (victim == ch)
      {
         send_to_char("You are already in sanctuary.\n\r", ch);
      }
      else
      {
         act("$N is already in sanctuary.", ch, NULL, victim, TO_CHAR);
      }
      return;
   }

   if
   (
      (
         brand = get_eq_char(ch, WEAR_BRAND)
      ) != NULL &&
      brand->pIndexData->vnum == 433
   )
   {
      send_to_char("The gods have forsaken you.\n\r", ch);
      return;
   }

   if
   (
      (
         brand = get_eq_char(victim, WEAR_BRAND)
      ) != NULL &&
      brand->pIndexData->vnum == 433
   )
   {
      send_to_char("The gods have forsaken you.\n\r", victim);
      if (victim != ch)
      {
         send_to_char("The gods have forsaken them.\n\r", ch);
      }
      return;
   }
   if (clergy_fails_help(ch, victim, sn))
   {
      return;
   }
   if
   (
      is_affected(victim, gsn_steel_nerves) ||
      is_affected(victim, gsn_chromatic_shield)
   )
   {
      if (ch != victim)
      {
         act
         (
            "$N is already protected.\n\rYour holy aura fails to surround $M.",
            ch,
            NULL,
            victim,
            TO_CHAR
         );
      }
      else
      {
         act
         (
            "You are already protected.\n\rThe holy aura fails to surround you.",
            ch,
            NULL,
            victim,
            TO_CHAR
         );
      }
      return;
   }
   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.duration  = (level / 6)*race_adjust(ch)/25.0;
   af.location  = APPLY_NONE;
   af.modifier  = 0;
   af.bitvector = AFF_SANCTUARY;
   affect_to_char(victim, &af);
   act
   (
      "$n is surrounded by a holy aura.",
      victim,
      NULL,
      NULL,
      TO_ROOM
   );
   act
   (
      "You are surrounded by a holy aura.",
      victim,
      NULL,
      NULL,
      TO_CHAR
   );
   return;
}



void spell_shield( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;

   if ( is_affected( victim, sn ) )
   {
      if (victim == ch)
      send_to_char("You are already shielded from harm.\n\r", ch);
      else
      act("$N is already protected by a shield.", ch, NULL, victim, TO_CHAR);
      return;
   }

   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.duration  = (8 + level)*race_adjust(ch)/25.0;
   af.location  = APPLY_AC;
   af.modifier  = -20;
   if (!IS_NPC(ch) && ch->pcdata->special == SUBCLASS_PRIEST_PROTECTION)
   af.modifier = -30;
   af.bitvector = 0;
   affect_to_char( victim, &af );
   act( "$n is surrounded by a force shield.", victim, NULL, NULL, TO_ROOM );
   send_to_char( "You are surrounded by a force shield.\n\r", victim );
   return;
}





void spell_shocking_grasp(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   static const int dam_each[] =
   {
      0,
      0,  0,  0,  16, 18,     20, 20, 25, 29, 33,
      36, 39, 39, 39, 40,    40, 41, 41, 42, 42,
      43, 43, 44, 44, 45,    45, 46, 46, 47, 47,
      48, 48, 49, 49, 50,    50, 51, 51, 52, 52,
      53, 53, 54, 54, 55,    55, 56, 56, 57, 57,
      58, 58, 59, 59, 60,    60, 61, 61, 62, 62
   };
   int dam;

   level = UMAX(0, level)*race_adjust(ch)/25.0;
   level = UMIN(level, (int)(sizeof(dam_each)/sizeof(dam_each[0]) - 1));
   level = UMAX(0, level);
   dam        = number_range( dam_each[level] / 2, dam_each[level] * 2 );
   if (dam > 124) dam = 124;
   if ( saves_spell( ch, level, victim, DAM_LIGHTNING, SAVE_SPELL) )
   dam /= 2;
   damage( ch, victim, dam, sn, DAM_LIGHTNING , TRUE);
   return;
}



void spell_sleep(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   CHAR_DATA* victim = (CHAR_DATA*) vo;
   AFFECT_DATA af;

   if (is_affected(victim, gsn_sleep_timer))
   {
      if (ch == victim)
      {
         send_to_char("You are too wide awake for that.\n\r", ch);
      }
      else
      {
         act
         (
            "$N is wide awake from adrenaline.",
            ch,
            NULL,
            victim,
            TO_CHAR
         );
      }
      return;
   }
   if
   (
      IS_AFFECTED(victim, AFF_SLEEP) ||
      (
         IS_NPC(victim) &&
         IS_SET(victim->act, ACT_UNDEAD)
      ) ||
      victim->race == grn_arborian ||
      saves_armor_of_god(ch, level, victim)
   )
   {
      send_to_char("You fail to induce sleep magically.\n\r", ch);
      return;
   }

   if
   (
      victim->fighting != NULL ||
      victim->position == POS_FIGHTING
   )
   {
      act
      (
         "$N's eyelids flutter, but the adrenaline of the immediate fight keeps"
         " $M wide awake.",
         ch,
         NULL,
         victim,
         TO_CHAR
      );
      send_to_char
      (
         "Your eyelids flutter, but the adrenaline of the immediate fight keeps"
         " you wide awake.\n\r",
         victim
      );
      return;
   }

   if (saves_spell(ch, level - 2, victim, DAM_CHARM, SAVE_MALEDICT))
   {
      send_to_char("You fail to induce sleep magically.\n\r", ch);
      if
      (
         number_bits(2) &&  /* 75% chance */
         check_immune(victim, DAM_CHARM) != IS_IMMUNE &&
         IS_AWAKE(victim)
      )
      {
         af.where     = TO_AFFECTS;
         af.type      = gsn_sleep_timer;
         af.level     = level;
         af.duration  = 0;
         af.location  = APPLY_NONE;
         af.modifier  = 0;
         af.bitvector = 0;
         affect_to_char(victim, &af);
         act
         (
            "$N's eyelids flutter, but $S adrenaline forces $M wide awake.",
            ch,
            NULL,
            victim,
            TO_CHAR
         );
         send_to_char
         (
            "Just as your eyelids flutter, you feel a surge of adrenaline.\n\r"
            "You feel wide awake!\n\r",
            victim
         );
      }
      return;
   }

   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.duration  = 2;
   af.location  = APPLY_NONE;
   af.modifier  = 0;
   af.bitvector = AFF_SLEEP;
   affect_join(victim, &af, -1);

   if (IS_AWAKE(victim))
   {
      victim->fighting = NULL;
      send_to_char("You feel very sleepy ..... zzzzzz.\n\r", victim);
      act("$n goes to sleep.", victim, NULL, NULL, TO_ROOM);
      victim->position = POS_SLEEPING;
   }
   if
   (
      !IS_NPC(ch) &&
      !IS_IMMORTAL(ch) &&
      !IS_IMMORTAL(victim) &&
      (
         !IS_NPC(victim) ||
         (
            IS_AFFECTED(victim, AFF_CHARM) &&
            victim->master != ch
         )
      )
   )
   {
      QUIT_STATE(ch, 20);
      QUIT_STATE(victim, 20);
   }
   return;
}

void spell_slow( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;

   if (!str_cmp(class_table[ch->class].name, "monk")
   && victim != ch)
   {
      send_to_char("You are only able to slow your own metabolism.\n\r", ch);
      return;
   }

   if ( is_affected( victim, sn ) || IS_AFFECTED(victim, AFF_SLOW))
   {
      if (victim == ch)
      send_to_char("You can't move any slower!\n\r", ch);
      else
      act("$N can't get any slower than that.",
      ch, NULL, victim, TO_CHAR);
      return;
   }

   if (victim != ch &&
   (saves_spell(ch, level, victim, DAM_OTHER, SAVE_MALEDICT)
   ||  IS_SET(victim->imm_flags, IMM_MAGIC)))
   {
      if (victim != ch)
      send_to_char("Nothing seemed to happen.\n\r", ch);
      send_to_char("You feel momentarily lethargic.\n\r", victim);
      return;
   }

   if (IS_AFFECTED(victim, AFF_HASTE))
   {
      if (!check_dispel(level, victim, gsn_haste) &&
      !check_dispel(level, victim, gsn_mass_haste))
      {
         if (victim != ch)
         send_to_char("You failed.\n\r", ch);
         send_to_char("You feel momentarily slower.\n\r", victim);
         return;
      }

      act("$n is moving less quickly.", victim, NULL, NULL, TO_ROOM);
      return;
   }


   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.duration  = level/2;
   af.location  = APPLY_DEX;
   af.modifier  = -1 - (level >= 18) - (level >= 25) - (level >= 32);
   af.bitvector = AFF_SLOW;
   affect_to_char( victim, &af );
   send_to_char( "You feel yourself slowing d o w n...\n\r", victim );
   act("$n starts to move in slow motion.", victim, NULL, NULL, TO_ROOM);
   return;
}




void spell_stone_skin( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;

   if (is_affected(victim, sn))
   {
      if (victim == ch)
      {
         send_to_char("Your skin is already as hard as a rock.\n\r", ch);
      }
      else
      {
         act("$N is already as hard as can be.", ch, NULL, victim, TO_CHAR);
      }
      return;
   }

   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.duration  = level*race_adjust(ch)/25.0;
   af.location  = APPLY_AC;
   af.modifier  = -40;
   if (!IS_NPC(ch) && ch->pcdata->special == SUBCLASS_PRIEST_PROTECTION)
   af.modifier = -50;
   af.bitvector = 0;
   affect_to_char(victim, &af);
   act("$n's skin turns to stone.", victim, NULL, NULL, TO_ROOM);
   send_to_char("Your skin turns to stone.\n\r", victim);
   return;
}


void spell_ventriloquate( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   char buf1[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];
   char cbuf1[MAX_STRING_LENGTH];
   char cbuf2[MAX_STRING_LENGTH];
   char speaker[MAX_INPUT_LENGTH];
   CHAR_DATA *vch;
   bool no_fail = FALSE;

   target_name = one_argument( target_name, speaker );

   if
   (
      !IS_NPC(ch) &&
      (
         ch->pcdata->special == SUBCLASS_ILLUSIONIST ||
         /* Book lottery winner has illusionist ventriloquate */
         ch->id == 1080493653
      )
   )
   {
      no_fail = TRUE;
   }

   sprintf( buf1, "%s says '%s'\n\r",              speaker, target_name );
   sprintf( buf2, "Someone makes %s say '%s'\n\r", speaker, target_name );
   buf1[0] = UPPER(buf1[0]);
   sprintf( cbuf1, "%s says '\x01B[1;33m%s\x01B[0;37m'\n\r", speaker, target_name);
   sprintf( cbuf2, "Someone makes %s say '\x01B[1;33m%s\x01B[0;37m'\n\r", speaker, target_name);
   cbuf1[0] = UPPER(cbuf1[0]);

   for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room )
   {
      if (IS_SET(vch->comm, COMM_ANSI))
      {
         if (!is_exact_name( speaker, vch->name) && IS_AWAKE(vch))
         send_to_char( (!saves_spell(ch, level, vch, DAM_OTHER, SAVE_MALEDICT) || no_fail) ? cbuf1 : cbuf2, vch );
      }
      else
      {
         if (!is_exact_name( speaker, vch->name) && IS_AWAKE(vch))
         send_to_char( (!saves_spell(ch, level, vch, DAM_OTHER, SAVE_MALEDICT) || no_fail) ? buf1 : buf2, vch );
      }
   }

   return;
}



void spell_weaken( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;

   if ( is_affected( victim, sn ))
   {
      act("$E is already in a weakened state.", ch, NULL, victim, TO_CHAR);
      /*      send_to_char("They are already in a weakened state.\n\r", ch); */
      return;
   }
   if ( saves_spell( ch, level, victim, DAM_OTHER, SAVE_MALEDICT) )
   {
      send_to_char("Your attempt to weaken them has failed.\n\r", ch);
      return;
   }
   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.duration  = level / 2;
   af.location  = APPLY_STR;
   af.modifier  = (-1 * (level / 5))*race_adjust(ch)/25.0;
   af.bitvector = AFF_WEAKEN;
   affect_to_char( victim, &af );
   send_to_char( "You feel your strength slip away.\n\r", victim );
   act("$n looks tired and weak.", victim, NULL, NULL, TO_ROOM);
   return;
}


void spell_web(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;
   int succ=1;

   if (is_affected(victim, sn) )
   {
      if (victim == ch)
      send_to_char("You are already covered in sticky webs.\n\r", victim);
      else
      act("$E is already covered in sticky webs.", ch, NULL, victim, TO_CHAR);
      /*    send_to_char("They are already covered in sticky webs.\n\r", ch); */

      return;
   }

   act("$n points a finger at $N and strands of sticky web spew forth.", ch, NULL, victim, TO_NOTVICT);
   act("$n points at you and strands of sticky webs spew forth.", ch, NULL, victim, TO_VICT);
   act("You point at $N and send a stream of sticky webs spewing forth.", ch, NULL, victim, TO_CHAR);

   /*    if (get_curr_stat(victim, STAT_DEX) > 23)
   {
   act("$N evades $n's webs with ease.", ch, NULL, victim, TO_NOTVICT);
   send_to_char("You evade the sticky webs with ease.\n\r", victim);
   act("$N evades your sticky webs with ease.", ch, NULL, victim, TO_CHAR);
   victim->move=UMAX(0, victim->move-30);
   return;
   }

   if (saves_spell(ch, level, victim, DAM_OTHER, SAVE_MALEDICT) )
   {
   act("Your webs miss $N.", ch, NULL, victim, TO_CHAR);
   act("$n's webs miss $N.", ch, NULL, victim, TO_NOTVICT);
   act("$n's webs miss you.", ch, NULL, victim, TO_VICT);
   victim->move=UMAX(0, victim->move-30);
   return;
   }
   */
   if (get_curr_stat(victim, STAT_DEX)>23||saves_spell(ch, level, victim, DAM_OTHER, SAVE_MALEDICT))
   succ=0;

   af.where = TO_AFFECTS;
   af.type = sn;
   af.level = level;
   af.duration = (level/5)*race_adjust(ch)/25.0;
   af.location = APPLY_DEX;
   af.modifier = -level/10;
   af.bitvector = 0;
   if (succ==0) {
      af.modifier=-2;
   } else {
      affect_to_char(victim, &af);
      af.location = APPLY_HITROLL;
      af.modifier = -level/8;
   }
   affect_to_char(victim, &af);
   victim->move = UMAX(0, victim->move - 60);

   act("$n is covered in sticky webs.", victim, NULL, NULL, TO_ROOM);
   send_to_char("You are covered in sticky webs.\n\r", victim);
   return;
}


void spell_word_of_recall(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   CHAR_DATA* victim = (CHAR_DATA*)vo;
   CHAR_DATA* mount = NULL;
   ROOM_INDEX_DATA* location;
   OBJ_DATA* brand;
   bool camewith = 0;

   if (IS_NPC(victim))
   {
      send_to_char("You failed.\n\r", ch);
      return;
   }

   if
   (
      IS_SET(ch->in_room->extra_room_flags, ROOM_DEAD_ZONE) &&
      !IS_SET(ch->in_room->room_flags, ROOM_CONSECRATED)
   )
   {
      send_to_char("You failed.\n\r", ch);
      return;
   }
   if (!is_supplicating)
   {
      if (!IS_IMMORTAL(ch))
      {
         send_to_char("Only clergy may use this prayer.\n\r", ch);
         sprintf
         (
            log_buf,
            "spell_word_of_recall, not supplicating: %s",
            ch->name
         );
         bug(log_buf, 0);
      }
      else
      {
         send_to_char
         (
            "Only clergy may use this prayer,\n\r"
            "use power word recall instead.\n\r",
            ch
         );
      }
      return;
   }

   if (is_affected(ch, gsn_purity))
   {
      if
      (
         ch == victim
      )
      {
         send_to_char("The mark of purity on you nullifies your word of recall!\n\r", ch);
         return;
      }
   }
   if (is_affected(victim, gsn_purity))
   {
      act
      (
         "You cannot use a word of recall on $N, $E has been struck with a mark"
         " of purity.",
         ch,
         NULL,
         victim,
         TO_CHAR
      );

      act
      (
         "$n tries to use a word of recall on you but the mark of purity"
         " binds you to the earth!",
         ch,
         NULL,
         victim,
         TO_VICT
      );
      return;
   }

   if (is_affected(victim, gsn_trapstun))
   {
      if (ch == victim)
      {
         send_to_char("You cannot recall: You are in a snare.\n\r", ch);
      }
      else
      {
         act
         (
            "You cannot recall $N: $e is in a snare.",
            ch,
            NULL,
            victim,
            TO_CHAR
         );
         act
         (
            "$n tries to recall you, but the snare holds tight.",
            ch,
            NULL,
            victim,
            TO_VICT
         );
      }
      return;
   }

   if (!is_same_group(ch, victim))
   {
      send_to_char("You can only recall those in your own group.\n\r", ch);
      return;
   }

   if
   (
      is_affected(victim, gsn_cloak_brave) &&
      victim->fighting != NULL
   )
   {
      brand = get_eq_char(victim, WEAR_BRAND);
      if
      (
         victim->hit > (victim->max_hit / 5) &&
         brand == NULL
      )
      {
         send_to_char
         (
            "You are much too loyal to the faith to be running!\n\r",
            ch
         );
         return;
      }
   }

   location = get_room_index(victim->temple);
   if (location == NULL)
   {
      if ((location = get_room_index( ROOM_VNUM_TEMPLE)) == NULL)
      {
         send_to_char("You are completely lost.\n\r", victim);
         return;
      }
   }

   if
   (
      (
         (
            IS_SET(victim->in_room->room_flags, ROOM_NO_RECALL) ||
            IS_SET(victim->in_room->extra_room_flags, ROOM_DEAD_ZONE)
         ) &&
         !IS_SET(victim->in_room->room_flags, ROOM_CONSECRATED)
      ) ||
      IS_AFFECTED(victim, AFF_CURSE)
   )
   {
      send_to_char("You failed.\n\r", victim);
      return;
   }

   if
   (
      (
         (
            brand = get_eq_char(victim, WEAR_BRAND)
         ) != NULL
      ) &&
      brand->pIndexData->vnum == 433
   )
   {
      send_to_char("The gods have forsaken you.\n\r", victim);
      if (victim != ch)
      {
         send_to_char("The gods have forsaken them.\n\r", ch);
      }
      return;
   }

   if
   (
      victim->in_room->house != 0 &&
      victim->in_room->house != victim->house
   )
   {
      send_to_char("You failed.\n\r", victim);
      return;
   }

   if (victim->fighting != NULL)
   {
      stop_fighting(victim, TRUE);
   }

   ch->move /= 2;

   /* Remove unwanted affects */
   un_camouflage(victim, NULL);
   un_earthfade(victim, NULL);
   un_forest_blend(victim);
   un_hide(victim, NULL);

   act("$n disappears.", victim, NULL, NULL, TO_ROOM);
   char_from_room(victim);
   char_to_room(victim, location);

   /* If the victim is riding a mount, and the mount is a mob, the mount will recall as well -- Wicket */
   if
   (
      victim->is_riding &&
      victim->is_riding->mount_type == MOUNT_MOBILE
   )
   {
      mount = (CHAR_DATA *)victim->is_riding->mount;
      char_from_room(mount);
      char_to_room(mount, victim->in_room);
      camewith = TRUE;
   }

   if (camewith)
   {
      act
      (
         "$n appears in the room, riding upon $N.",
         victim,
         NULL,
         mount,
         TO_ROOM
      );
   }
   else
   {
      act("$n appears in the room.", victim, NULL, NULL, TO_ROOM);
   }

   do_observe(victim, "", LOOK_AUTO);
   return;
}

void spell_teleport( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *victim = (CHAR_DATA *) vo, *mount;
   ROOM_INDEX_DATA *pRoomIndex;
   int camewith = 0;

   if (is_affected(ch, gsn_trapstun))
   {
      send_to_char("You cannot teleport: You are in a snare.\n\r", ch);
      return;
   }

   if
   (
      is_affected(ch, gsn_purity)
   )
   {
      send_to_char("The mark of purity binds you to the earth!\n\r", ch);
      return;
   }

   if
   (
      victim->in_room == NULL ||
      (
         !IS_NPC(ch) &&
         (
            (
               IS_SET(victim->in_room->room_flags, ROOM_NO_RECALL) ||
               IS_SET(victim->in_room->extra_room_flags, ROOM_DEAD_ZONE)
            ) &&
            !IS_SET(victim->in_room->room_flags, ROOM_CONSECRATED)
         )
      ) ||
      (
         victim != ch &&
         IS_SET(victim->imm_flags, IMM_SUMMON)
      ) ||
      (
         victim != ch &&
         (
            saves_spell(ch, (level - 5), victim, DAM_OTHER, SAVE_TRAVEL) &&
            level != 1000  /* sword of marcil */
         )
      )
   )
   {
      send_to_char( "You failed.\n\r", ch );
      return;
   }
   if (is_safe(ch, victim, IS_SAFE_SILENT))
   {
      send_to_char("You failed.\n\r", ch);
      return;
   }
   if (victim->in_room->house != 0
   && victim->in_room->house != victim->house && !IS_NPC(victim))
   {
      send_to_char("You failed.\n\r", victim);
      return;
   }

   if
   (
      IS_NPC(victim) &&
      (
         victim->pIndexData->vnum > 15399 &&
         victim->pIndexData->vnum < 15499
      )
   )
   {
      if
      (
         (
            pRoomIndex = get_random_room(victim, RANDOM_MOB_TELEPORT)
         ) == NULL
      )
      {
         bug("NULL room from get_random_room.", 0);
         return;
      }
   }
   else
   {
      if
      (
         (
            pRoomIndex = get_random_room(victim, RANDOM_NORMAL)
         ) == NULL
      )
      {
         bug("NULL room from get_random_room.", 0);
         return;
      }
   }

   if (victim != ch)
   send_to_char("You have been teleported!\n\r", victim);

   /* Remove unwanted affects */
   un_camouflage(victim, NULL);
   un_earthfade(victim, NULL);
   un_forest_blend(victim);
   un_hide(victim, NULL);

   act( "$n vanishes!", victim, NULL, NULL, TO_ROOM );
   char_from_room( victim );
   char_to_room( victim, pRoomIndex );

   /* If the victim is riding a mount, and the mount is a mob, the mount will be summoned as well -- Wicket */
   if (victim->is_riding && victim->is_riding->mount_type == MOUNT_MOBILE)
   for (mount = char_list; mount != NULL; mount = mount->next)
   {
      if (IS_NPC(mount)
      && IS_AFFECTED(mount, AFF_CHARM)
      && (mount->master == victim)
      && (mount == (CHAR_DATA *)victim->is_riding->mount))
      {
         if (mount->in_room != victim->in_room)
         {
            char_from_room(mount);
            char_to_room(mount, victim->in_room);
            camewith++;
         }
      }
   }

   if (camewith > 0)
   act("$n slowly fades into existence, riding upon $N.", victim, NULL, (CHAR_DATA *)victim->is_riding->mount, TO_ROOM);
   else
   act( "$n slowly fades into existence.", victim, NULL, NULL, TO_ROOM );

   do_observe(victim, "", LOOK_AUTO);
   return;
}


void spell_summon( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *victim;
   OBJ_DATA *brand;

   if (clergy_fails_help(ch, ch, sn))
   {
      /* Crusaders cannot summon */
      return;
   }
   if
   (
      (
         (
            brand = get_eq_char(ch, WEAR_BRAND)
         ) != NULL
      ) &&
      brand->pIndexData->vnum == 433
   )
   {
      send_to_char("The gods have forsaken you.\n\r", ch);
      return;
   }

   if
   (
      !IS_NPC(ch) &&
      ch->pcdata->special == SUBCLASS_ZEALOT_FAITH
   )
   {
      level += 5;
   }

   if
   (
      (
         victim = get_char_world(ch, target_name)
      ) == NULL ||
      victim == ch ||
      victim->in_room == NULL
   )
   {
      send_to_char( "You failed.\n\r", ch );
      return;
   }

   if
   (
      number_percent() < MARAUDER_MISINFORM_CHANCE &&
      is_affected(victim, gsn_misinformation)
   )
   {
      if
      (
         (
            victim = get_random_mob(victim, RANDOM_MOB_VNUM_RANGE_LARGE)
         ) == NULL
      )
      {
         send_to_char( "You failed.\n\r", ch );
         return;
      }
   }

   if
   (
      (
         !IS_NPC(victim) &&
         victim->in_room->area != ch->in_room->area
      ) ||
      IS_SET(ch->in_room->room_flags, ROOM_SAFE) ||
      IS_SET(victim->in_room->room_flags, ROOM_NO_SUMMON) ||
      ch->in_room->guild != 0 ||
      victim->in_room->guild != 0 ||
      (
         IS_NPC(victim) &&
         IS_AFFECTED(victim, AFF_CHARM) &&
         victim->in_room->area != ch->in_room->area
      ) ||
      IS_SET(victim->in_room->room_flags, ROOM_SAFE) ||
      IS_SET(victim->in_room->room_flags, ROOM_PRIVATE) ||
      IS_SET(victim->in_room->room_flags, ROOM_SOLITARY) ||
      (
         (
            IS_SET(victim->in_room->room_flags, ROOM_NO_RECALL) ||
            (
               IS_SET(victim->in_room->extra_room_flags, ROOM_DEAD_ZONE) &&
               !IS_NPC(ch)
            )
         ) &&
         !IS_SET(victim->in_room->room_flags, ROOM_CONSECRATED)
      ) ||
      (
         IS_NPC(victim) &&
         IS_SET(victim->act, ACT_AGGRESSIVE)
      ) ||
      (
         IS_NPC(victim) &&
         IS_SET(victim->off_flags, SPAM_MURDER)
      ) ||
      victim->level >= (level + 5) ||
      (
         !IS_NPC(victim) &&
         victim->level >= LEVEL_IMMORTAL
      ) ||
      (
         ch->in_room->house != victim->in_room->house &&
         !IS_NPC(victim)
      ) ||
      victim->fighting != NULL ||
      (
         IS_NPC(victim) &&
         IS_SET(victim->imm_flags, IMM_SUMMON)
      ) ||
      (
         IS_NPC(victim) &&
         victim->pIndexData->pShop != NULL
      ) ||
      (
         IS_NPC(victim) &&
         victim->spec_fun != NULL
      ) ||
      (
         IS_NPC(victim) &&
         victim->pIndexData->mprog_flags
      ) ||
      (
         IS_NPC(victim) &&
         IS_SET(victim->act, ACT_AGGRESSIVE)
      ) ||
      (
         !IS_NPC(ch) &&
         !IS_NPC(victim) &&
         is_safe
         (
            ch,
            victim,
            IS_SAFE_SILENT | IS_SAFE_IGNORE_ROOM | IS_SAFE_IGNORE_AFFECTS
         ) &&
         IS_SET(victim->act, PLR_NOSUMMON)
      ) ||
      saves_spell( ch, level, victim, DAM_OTHER, SAVE_TRAVEL)
   )
   {
      send_to_char( "You failed.\n\r", ch );
      return;
   }
   if
   (
      ch->in_room->vnum >= 1950 &&
      ch->in_room->vnum <= 1962
   )
   {
      send_to_char( "You failed.\n\r", ch );
      return;
   }

   if (oblivion_blink(ch, victim))
   {
      return;
   }

   /* Remove unwanted affects */
   un_camouflage(victim, NULL);
   un_earthfade(victim, NULL);
   un_forest_blend(victim);
   un_hide(victim, NULL);

   if ( victim->is_riding )
   do_dismount( victim, NULL );

   act( "$n disappears suddenly.", victim, NULL, NULL, TO_ROOM );
   char_from_room( victim );
   char_to_room( victim, ch->in_room );
   act( "$n arrives suddenly.", victim, NULL, NULL, TO_ROOM );
   act( "$n has summoned you!", ch, NULL, victim,   TO_VICT );
   do_observe(victim, "", LOOK_AUTO);
   return;
}


void spell_gate(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim;
   CHAR_DATA *group;
   CHAR_DATA *arrival;
   CHAR_DATA *g_next;
   CHAR_DATA *last_to_venue;
   int numb;
   ROOM_INDEX_DATA *pRoomIndex;

   if (clergy_fails_help(ch, ch, sn))
   {
      /* Crusaders cannot gate */
      return;
   }

   if
   (
      IS_SET(ch->in_room->extra_room_flags, ROOM_DEAD_ZONE) &&
      !IS_SET(ch->in_room->room_flags, ROOM_CONSECRATED)
   )
   {
      send_to_char
      (
         "You failed.\n\r",
         ch
      );
      return;
   }
   if (is_affected(ch, gsn_trapstun))
   {
      send_to_char("You cannot gate: You are in a snare.\n\r", ch);
      return;
   }
   if (is_affected(ch, gsn_purity))
   {
      send_to_char
      (
         "The mark of purity binds you to the earth!\n\r",
         ch
      );
      return;
   }
   if (ch->fighting != NULL)
   {
      send_to_char("You can't concentrate enough.\n\r", ch);
      return;
   }

   last_to_venue = ch;
   victim = get_char_world( ch, target_name );
   if (victim == ch)
   {
      send_to_char("You failed.\n\r", ch);
      return;
   }

   if (!(victim==NULL))
   {
      if ((IS_NPC(victim)) && ( IS_SET(victim->res_flags, RES_SUMMON) ))
      level -= 5;
      if ((IS_NPC(victim)) && ( IS_SET(victim->vuln_flags, VULN_SUMMON) ))
      level += 5;
   }
   if ( victim == NULL
   ||   victim == ch
   ||   victim->in_room == ch->in_room
   ||   victim->in_room == NULL
   ||   !can_see_room(ch, victim->in_room)
   ||   IS_SET(victim->in_room->room_flags, ROOM_NO_GATE)
   ||   IS_SET(victim->in_room->room_flags, ROOM_SAFE)
   ||   IS_SET(victim->in_room->room_flags, ROOM_PRIVATE)
   ||   ch->in_room->guild != 0
   ||   (ch->in_room->house != 0 && ch->in_room->house != ch->house)
   ||     victim->in_room->guild != 0
   ||     (victim->in_room->house != 0 && victim->in_room->house != ch->house)
   ||   IS_SET(victim->in_room->room_flags, ROOM_SOLITARY) ||
      (
         (
            IS_SET(victim->in_room->room_flags, ROOM_NO_RECALL) ||
            IS_SET(victim->in_room->extra_room_flags, ROOM_DEAD_ZONE)
         ) &&
         !IS_SET(victim->in_room->room_flags, ROOM_CONSECRATED)
      ) ||
      (
         (
            IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL) ||
            IS_SET(ch->in_room->extra_room_flags, ROOM_DEAD_ZONE)
         ) &&
         !IS_SET(ch->in_room->room_flags, ROOM_CONSECRATED)
      ) ||
      victim->level >= level + 4 ||
      (
         !IS_NPC(victim) &&
         is_safe
         (
            ch,
            victim,
            IS_SAFE_SILENT | IS_SAFE_IGNORE_ROOM | IS_SAFE_IGNORE_AFFECTS
         ) &&
         IS_SET(victim->act, PLR_NOSUMMON)
      ) ||
      (!IS_NPC(victim) && saves_spell(ch, level, victim, DAM_OTHER, SAVE_TRAVEL))
   ||   (!IS_NPC(victim) && victim->level > LEVEL_HERO)  /* NOT trust */
   ||   (IS_NPC(victim) && IS_SET(victim->imm_flags, IMM_SUMMON))
   ||   (IS_NPC(victim) && saves_spell( ch, level, victim, DAM_OTHER, SAVE_TRAVEL) ) )
   {
      send_to_char( "You failed.\n\r", ch );
      return;
   }
   if ( IS_SET(victim->in_room->room_flags, ROOM_BLOODY_TIMER) &&
   (ch->pause > 0 || ch->quittime > 13))
   {
      send_to_char("You are too bloody to gate to that place.\n\r", ch);
      return;
   }
   /* not sure but this check seems redudandant with above - werv */
   if (victim->in_room->house != 0
   && victim->in_room->house != ch->house)
   {
      send_to_char("You failed.\n\r", ch);
      return;
   }
   if (oblivion_blink(ch, victim))
   {
      return;
   }

   /* gate_pet disabled, it will move via the 'move all charmy' code below */
   /*
   if (ch->pet != NULL && ch->in_room == ch->pet->in_room)
   {
      gate_pet = TRUE;
   }
   else
   {
      gate_pet = FALSE;
   }
   */
   if
   (
      number_percent() < MARAUDER_MISINFORM_CHANCE &&
      is_affected(victim, gsn_misinformation)
   )
   {
      if
      (
         (
            pRoomIndex = get_random_room(victim, RANDOM_NORMAL)
         ) == NULL
      )
      {
         bug("NULL room from get_random_room.", 0);
         return;
      }
   }
   else
   {
      pRoomIndex = victim->in_room;
   }

   numb = 1;
   for (group = ch->in_room->people; group != NULL; group = g_next)
   {
      g_next = group->next_in_room;
      if (group->fighting != NULL)
      {
         continue;
      }
      if (group == ch )
      {
         continue;
      }
      if
      (
         !IS_NPC(group) ||
         !IS_AFFECTED(group, AFF_CHARM) ||
         group->master == NULL ||
         group->master != ch
      )
      {
         continue;
      }

      numb++;
      un_camouflage(group, NULL);
      un_earthfade(group, NULL);
      un_hide(group, NULL);
      un_forest_blend(group); /* - Wicket */
      if
      (
         ch->is_riding == NULL ||
         group != ((CHAR_DATA*)ch->is_riding->mount)
      )
      {
         send_to_char("You step through a gate and vanish.\n\r", group);
         act("$n steps through a gate and vanishes.", group, NULL, NULL, TO_ROOM);
      }
      char_from_room(group);
      char_to_room(group, pRoomIndex);
      if
      (
         ch->is_riding == NULL ||
         group != ((CHAR_DATA *) ch->is_riding->mount)
      )
      {
         act("$n has arrived through a gate.", group, NULL, NULL, TO_ROOM);
      }
      last_to_venue = group;
   }
   /* gate_pet disabled, it will move via the 'move all charmy' code above */
   /*
   if (gate_pet)
   {
      group = ch->pet;
      un_camouflage(group, NULL);
      un_earthfade(group, NULL);
      un_hide(group, NULL);
      un_forest_blend(group);  / * - Wicket * /
      send_to_char("You step through a gate and vanish.\n\r", group);
      act("$n steps through a gate and vanishes.", group, NULL, NULL, TO_ROOM);
      char_from_room(group);
      char_to_room(group, victim->in_room);
      act("$n has arrived through a gate.", group, NULL, NULL, TO_ROOM);
      last_to_venue = group;
      numb++;
   }
   */
   un_camouflage(ch, NULL);
   un_hide(ch, NULL);
   un_earthfade(ch, NULL);
   un_forest_blend(ch); /* - Wicket */

   if
   (
      ch->is_riding == NULL ||
      ch->is_riding->mount == NULL
   )
   {
      act("$n steps through a gate and vanishes.", ch, NULL, NULL, TO_ROOM);
      send_to_char("You step through a gate and vanish.\n\r", ch);
      char_from_room(ch);
      char_to_room(ch, pRoomIndex);
      act("$n has arrived through a gate.", ch, NULL, NULL, TO_ROOM);
   }
   else
   {
      act("$n directs $N through a gate and vanishes.", ch, NULL, (CHAR_DATA *) ch->is_riding->mount, TO_ROOM);
      send_to_char("You ride through a gate and vanish.\n\r", ch);
      char_from_room(ch);
      char_to_room(ch, pRoomIndex);
      act("$n has arrived through a gate riding upon $N.", ch, NULL, (CHAR_DATA *) ch->is_riding->mount, TO_ROOM);
   }

   if (last_to_venue == ch)
   {
      do_observe(ch, "", LOOK_AUTO);
      return;
   }


   for (arrival = ch->in_room->people; arrival != NULL; arrival = arrival->next_in_room)
   {
      do_observe(arrival, "", LOOK_AUTO);
      if (--numb == 0)
      break;
   }

   return;
}


void spell_turn_undead( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim;
   CHAR_DATA *v_next;
   OBJ_DATA *obj, *brand;
   char *vname;
   int oflag;
   int dam;
   int value;

   if (check_peace(ch)) return;

   if (((brand = get_eq_char(ch, WEAR_BRAND)) != NULL)
   && (brand->pIndexData->vnum == 433))
   {
      send_to_char("The gods have forsaken you.\n\r", ch);
      return;
   }

   if (IS_EVIL(ch))
   {
      send_to_char
      (
         "The gods do not grant this power to clerics of darkness.\n\r",
         ch
      );
      return;
   }

   act("$n raises $s hands and calls upon $z to destroy the unholy.", ch, NULL, NULL, TO_ROOM);
   /*
   is !IS_GOOD(ch)
   {
   send_to_char("You are too impure to smite down the unholy.\n\r", ch);
   act("In response a faint hollow laughter sounds but nothing happens.", ch, NULL, NULL, TO_ROOM);
   return;
   }
   */

   act("You raise your hands and call upon $z to destroy the unholy.", ch, NULL, NULL, TO_CHAR);

   for (victim = ch->in_room->people; victim != NULL; victim = v_next)
   {
      v_next = victim->next_in_room;
      if (is_same_group(victim, ch) || !IS_SET(victim->act, ACT_UNDEAD) )
      continue;
      value = UMAX(1, level - victim->level + 10);
      value = UMAX(13, value - 4);
      if (IS_GOOD(ch))
      dam = dice(level, value);
      else
      dam = dice(level - 6, value - 1 );

      if (!is_safe(ch, victim, IS_SAFE_SILENT | IS_SAFE_AREA_ATTACK))
      {
         if ( ( ( (ch->level > (victim->level + 18) ) && IS_NPC(victim) && IS_GOOD(ch) )
         || ( (ch->level > (victim->level + 24) ) && IS_NPC(victim) )
         || (number_percent() > 96 && IS_GOOD(ch))
         || (number_percent() < 2 && !IS_GOOD(ch)))
         && can_instakill(ch, victim))
         {
            act("$N crumbles to dust from the power of $n's turning.", ch, NULL, victim, TO_ROOM);
            act("$N's body crumbles to dust from the power of your turning.", ch, NULL, victim, TO_CHAR);
            /* revised turn undead mob removal */
            vname = victim->short_descr;
            raw_kill(ch, victim);
            obj = ch->in_room->contents;
            oflag=1;
            if (obj == NULL )
            oflag=0;
            while ( oflag==1 )
            {
               if (strstr(obj->short_descr, vname))
               {
                  oflag=0;
                  extract_obj(obj, FALSE);
               }
               obj = obj->next_content;
               if ( obj==NULL )
               oflag=0;
            }
         }
         else
         {
            dam *= race_adjust(ch)/25.0;
            damage( ch, victim, saves_spell( ch, level, victim, DAM_HOLY, SAVE_SPELL) ? dam / 2 :dam, sn, DAM_HOLY, TRUE);
         }
      }
   }


   return;
}


/*
NPC spells.
*/
void spell_acid_breath( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   int dam, hp_dam, dice_dam, hpch;

   if (check_peace(ch)) return;

   act("$n spits acid at $N.", ch, NULL, victim, TO_NOTVICT);
   act("$n spits a stream of corrosive acid at you.", ch, NULL, victim, TO_VICT);
   act("You spit acid at $N.", ch, NULL, victim, TO_CHAR);

   hpch = ch->hit/9;
   dice_dam = dice(level, 4);
   dam = hpch + dice_dam;
   if (dam > ch->hit)
   dam = ch->hit;
   hp_dam = 0;
   /*
   hpch = UMAX(12, ch->hit);
   hp_dam = number_range(hpch/11 + 1, hpch/6);
   dice_dam = dice(level, 16);

   dam = UMAX(hp_dam + dice_dam/10, dice_dam + hp_dam/10);
   */

   if (saves_spell(ch, level, victim, DAM_ACID, SAVE_BREATH))
   {
      acid_effect(victim, level/2, dam/4, TARGET_CHAR);
      damage(ch, victim, dam/2, sn, DAM_ACID, TRUE);
   }
   else
   {
      acid_effect(victim, level, dam, TARGET_CHAR);
      damage(ch, victim, dam, sn, DAM_ACID, TRUE);
   }
}



void spell_fire_breath( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   CHAR_DATA *vch, *vch_next;
   int dam, hp_dam, dice_dam;
   int hpch;

   if (check_peace(ch)) return;

   act("$n breathes forth a cone of fire.", ch, NULL, victim, TO_NOTVICT);
   act("$n breathes a cone of hot fire over you!", ch, NULL, victim, TO_VICT);
   act("You breath forth a cone of fire.", ch, NULL, NULL, TO_CHAR);

   hpch = ch->hit/9;
   dice_dam = dice(level, 5);
   dam = hpch + dice_dam;
   if (dam > ch->hit)
   dam = ch->hit;

   hp_dam  = number_range( hpch/9+1, hpch/5 );
   /*
   dice_dam = dice(level, 20);

   dam = UMAX(hp_dam + dice_dam /10, dice_dam + hp_dam / 10);
   */

   fire_effect(victim->in_room, level, dam/2, TARGET_ROOM);

   for (vch = victim->in_room->people; vch != NULL; vch = vch_next)
   {
      vch_next = vch->next_in_room;
      /*
      if (is_safe_spell(ch, vch, TRUE)
      ||  (IS_NPC(vch) && IS_NPC(ch)
      &&   (ch->fighting != vch || vch->fighting != ch)))
      continue;
      */
      if (is_same_group(vch, ch))
      continue;
      if (is_safe(ch, vch, IS_SAFE_SILENT | IS_SAFE_AREA_ATTACK) || vch == ch)
      continue;
      if (IS_IMMORTAL(vch) && !can_see(ch, vch) && wizi_to(vch, ch)) continue;
      if (oblivion_blink(ch, vch))
      {
         continue;
      }
      if (vch == victim) /* full damage */
      {
         if (saves_spell(ch, level, vch, DAM_FIRE, SAVE_BREATH))
         {
            fire_effect(vch, level/2, dam/4, TARGET_CHAR);
            damage(ch, vch, dam/2, sn, DAM_FIRE, TRUE);
         }
         else
         {
            fire_effect(vch, level, dam, TARGET_CHAR);
            damage(ch, vch, dam, sn, DAM_FIRE, TRUE);
         }
      }
      else /* partial damage */
      {
         if (saves_spell(ch, level - 2, vch, DAM_FIRE, SAVE_BREATH))
         {
            fire_effect(vch, level/4, dam/8, TARGET_CHAR);
            damage(ch, vch, dam/4, sn, DAM_FIRE, TRUE);
         }
         else
         {
            fire_effect(vch, level/2, dam/4, TARGET_CHAR);
            damage(ch, vch, dam/2, sn, DAM_FIRE, TRUE);
         }
      }
   }
}

void spell_frost_breath( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   CHAR_DATA *vch, *vch_next;
   int dam, hp_dam, dice_dam, hpch;

   if (check_peace(ch)) return;

   act("$n breathes out a freezing cone of frost!", ch, NULL, victim, TO_NOTVICT);
   act("$n breathes a freezing cone of frost over you!",
   ch, NULL, victim, TO_VICT);
   act("You breath out a cone of frost.", ch, NULL, NULL, TO_CHAR);

   hpch = ch->hit/9;
   dice_dam = dice(level, 5);
   dam = hpch + dice_dam;
   if (dam > ch->hit)
   dam = ch->hit;
   hp_dam = 0;
   /*
   hpch = UMAX(12, ch->hit);
   hp_dam = number_range(hpch/11 + 1, hpch/6);
   dice_dam = dice(level, 16);

   dam = UMAX(hp_dam + dice_dam/10, dice_dam + hp_dam/10);
   */
   cold_effect(victim->in_room, level, dam/2, TARGET_ROOM);

   for (vch = victim->in_room->people; vch != NULL; vch = vch_next)
   {
      vch_next = vch->next_in_room;
      /*
      if (is_safe_spell(ch, vch, TRUE)
      ||  (IS_NPC(vch) && IS_NPC(ch)
      &&   (ch->fighting != vch || vch->fighting != ch)))
      continue;
      */
      if (is_safe(ch, vch, IS_SAFE_SILENT | IS_SAFE_AREA_ATTACK) || vch == ch)
      continue;
      if (IS_IMMORTAL(vch) && !can_see(ch, vch) && wizi_to(vch, ch)) continue;

      if (oblivion_blink(ch, vch))
      {
         continue;
      }
      if (vch == victim) /* full damage */
      {
         if (saves_spell(ch, level, vch, DAM_COLD, SAVE_BREATH))
         {
            cold_effect(vch, level/2, dam/4, TARGET_CHAR);
            damage(ch, vch, dam/2, sn, DAM_COLD, TRUE);
         }
         else
         {
            cold_effect(vch, level, dam, TARGET_CHAR);
            damage(ch, vch, dam, sn, DAM_COLD, TRUE);
         }
      }
      else
      {
         if (saves_spell(ch, level - 2, vch, DAM_COLD, SAVE_BREATH))
         {
            cold_effect(vch, level/4, dam/8, TARGET_CHAR);
            damage(ch, vch, dam/4, sn, DAM_COLD, TRUE);
         }
         else
         {
            cold_effect(vch, level/2, dam/4, TARGET_CHAR);
            damage(ch, vch, dam/2, sn, DAM_COLD, TRUE);
         }
      }
   }
}


void spell_gas_breath( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;
   int dam, hp_dam, dice_dam, hpch;

   if (check_peace(ch)) return;

   act("$n breathes out a cloud of poisonous gas!", ch, NULL, NULL, TO_ROOM);
   act("You breath out a cloud of poisonous gas.", ch, NULL, NULL, TO_CHAR);

   hpch = ch->hit/9;
   dice_dam = dice(level, 5);
   dam = hpch + dice_dam;
   if (dam > ch->hit)
   dam = ch->hit;


   hp_dam = 0;
   /*
   hpch = UMAX(16, ch->hit);
   hp_dam = number_range(hpch/15+1, 8);
   dice_dam = dice(level, 12);

   dam = UMAX(hp_dam + dice_dam/10, dice_dam + hp_dam/10);
   */
   poison_effect(ch->in_room, level, dam, TARGET_ROOM);

   for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
   {
      vch_next = vch->next_in_room;
      /*
      if (is_safe_spell(ch, vch, TRUE)
      ||  (IS_NPC(ch) && IS_NPC(vch)
      &&   (ch->fighting == vch || vch->fighting == ch)))
      continue;
      */
      if (IS_NPC(vch) && vch->pIndexData->vnum == MOB_VNUM_DEMON1)
      continue;

      if (is_same_group(ch, vch))
      continue;
      if (is_safe(ch, vch, IS_SAFE_SILENT | IS_SAFE_AREA_ATTACK) || vch == ch)
      continue;
      if (oblivion_blink(ch, vch))
      {
         continue;
      }
      if (IS_IMMORTAL(vch) && !can_see(ch, vch) && wizi_to(vch, ch)) continue;

      if (saves_spell(ch, level, vch, DAM_POISON, SAVE_BREATH))
      {
         poison_effect(vch, level/2, dam/4, TARGET_CHAR);
         damage(ch, vch, dam/2, sn, DAM_POISON, TRUE);
      }
      else
      {
         poison_effect(vch, level, dam, TARGET_CHAR);
         damage(ch, vch, dam, sn, DAM_POISON, TRUE);
      }
   }
}

void spell_lightning_breath(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   int dam, hp_dam, dice_dam, hpch;

   if (check_peace(ch)) return;

   act("$n breathes a bolt of lightning at $N.", ch, NULL, victim, TO_NOTVICT);
   act("$n breathes a bolt of lightning at you!", ch, NULL, victim, TO_VICT);
   act("You breathe a bolt of lightning at $N.", ch, NULL, victim, TO_CHAR);

   hpch = ch->hit/9;
   dice_dam = dice(level, 5);
   dam = hpch + dice_dam;
   if (dam > ch->hit)
   dam = ch->hit;
   hp_dam = 0;
   /*
   hpch = UMAX(10, ch->hit);
   hp_dam = number_range(hpch/9+1, hpch/5);
   dice_dam = dice(level, 20);

   dam = UMAX(hp_dam + dice_dam/10, dice_dam + hp_dam/10);
   */
   if (saves_spell(ch, level, victim, DAM_LIGHTNING, SAVE_BREATH))
   {
      shock_effect(victim, level/2, dam/4, TARGET_CHAR);
      damage(ch, victim, dam/2, sn, DAM_LIGHTNING, TRUE);
   }
   else
   {
      shock_effect(victim, level, dam, TARGET_CHAR);
      damage(ch, victim, dam, sn, DAM_LIGHTNING, TRUE);
   }
}

/*
Spells for mega1.are from Glop/Erkenbrand.
*/
void spell_general_purpose(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   int dam;

   dam = number_range( 25, 100 );
   if ( saves_spell( ch, level, victim, DAM_PIERCE, SAVE_SPELL) )
   dam /= 2;
   damage( ch, victim, dam, sn, DAM_PIERCE , TRUE);
   return;
}

void spell_high_explosive(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   int dam;

   dam = number_range( 30, 120 );
   if ( saves_spell( ch, level, victim, DAM_PIERCE, SAVE_SPELL) )
   dam /= 2;
   damage( ch, victim, dam, sn, DAM_PIERCE , TRUE);
   return;
}



/* This is a big block of elementalist spells all at once */

void spell_windwall(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;
   char buf[MAX_STRING_LENGTH];
   int dam;
   AFFECT_DATA af;

   if (check_peace(ch))
   {
      return;
   }

   if (sn == gsn_dracolich_evil)
   {
      send_to_char(
         "You beat your mighty wings, creating a violent wall of wind.\n\r",
         ch);
      act(
         "$n beats $s mighty wings, creating a violent wall of wind!",
         ch,
         NULL,
         NULL,
         TO_ROOM);

      sn = gsn_windwall;
   }
   else
   {
      send_to_char(
         "You raise a violent wall of wind to strike your foes.\n\r",
         ch);
      act(
         "$n raises a violent wall of wind, sending debris flying!",
         ch,
         NULL,
         NULL,
         TO_ROOM);
   }

   af.where = TO_AFFECTS;
   af.location = APPLY_HITROLL;
   af.modifier = -3;
   af.duration = 1;
   af.level = level;
   af.type = sn;
   af.bitvector = AFF_BLIND;

   dam = dice(level, 5)*race_adjust(ch)/25.0;

   for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
   {
      vch_next = vch->next_in_room;
      if (
            (
               has_skill(vch, gsn_wraithform) &&
               get_skill(vch, gsn_wraithform) >= 75
            ) ||
            (
               IS_NPC(vch) &&
               vch->pIndexData->vnum == MOB_VNUM_AIR_ELEMENTAL
            )
         )
      {
         act("$n seems amused by the winds.", vch, NULL, NULL, TO_ROOM);
         continue;
      }

      if (is_same_group(vch, ch))
      {
         continue;
      }

      if (
            IS_IMMORTAL(vch) &&
            !can_see(ch, vch) &&
            wizi_to(vch, ch)
         )
      {
         continue;
      }

      if (is_safe(ch, vch, IS_SAFE_SILENT | IS_SAFE_AREA_ATTACK))
      {
         continue;
      }

      if (oblivion_blink(ch, vch))
      {
         continue;
      }

      if (
            !IS_NPC(ch) &&
            !IS_NPC(vch) &&
            (ch->fighting == NULL || vch->fighting == NULL)
         )
      {
         switch(number_range(0, 2))
         {
            case (0):
            case (1):
            {
               sprintf(buf, "Die, %s, you sorcerous dog!", PERS(ch, vch));
               break;
            }
            case (2):
            {
               sprintf(
                  buf,
                  "Help! %s is casting a spell on me!",
                  PERS(ch, vch));
            }
         }

         if (vch != ch)
         {
            do_myell(vch, buf);
            sprintf(
               log_buf,
               "[%s] cast windwall upon [%s] at %d",
               ch->name,
               vch->name,
               ch->in_room->vnum);
            log_string(log_buf);
         }
      }

      if (
            number_range(0, 1) == 0 &&
            !saves_spell(ch, level, vch, DAM_BASH, SAVE_SPELL)
         )
      {
         affect_strip(vch, sn);
         act("$n appears blinded by the debris.", vch, NULL, NULL, TO_ROOM);
         send_to_char
         (
            "Debris gets in your eyes, blocking your vision.\n\r",
            vch
         );
         affect_to_char(vch, &af);
      }

      if (IS_FLYING(vch))
      {
         if (!saves_spell(ch, level, vch, DAM_BASH, SAVE_SPELL) )
         {
            act(
               "$n is thrown wildly to the ground by the air blast!",
               vch,
               NULL,
               NULL,
               TO_ROOM);
            send_to_char("You are thrown down by the air blast!\n\r", vch);
            affect_strip(vch, gsn_fly);
         }
      }

      damage(ch, vch, dam, sn, DAM_BASH, TRUE);
   }
}

void spell_summon_fire_elemental(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *elemental;
   AFFECT_DATA af;
   int count;
   bool gotvoid;
   CHAR_DATA *check;
   int pyro = 0;

   if (!IS_NPC(ch) && ch->pcdata->special == SUBCLASS_PYROMANCER)
   pyro = 1;

   if (check_peace(ch)) return;

   if (is_affected(ch, sn))
   {
      send_to_char("You can't call upon a fire elemental yet.\n\r", ch);
      return;
   }
   count = 0;
   gotvoid = 0;

   for (check = char_list; check != NULL; check = check->next)
   {
      if
      (
         !IS_NPC(check) ||
         check->master != ch ||
         is_affected(check, gsn_seize)
      )
      {
         continue;
      }
      if
      (
         check->pIndexData->vnum == MOB_VNUM_FIRE_ELEMENTAL ||
         check->pIndexData->vnum  == MOB_VNUM_AIR_ELEMENTAL ||
         check->pIndexData->vnum  == MOB_VNUM_EARTH_ELEMENTAL ||
         check->pIndexData->vnum  == MOB_VNUM_WATER_ELEMENTAL
      )
      {
         count++;
      }
      else if (check->pIndexData->vnum == MOB_VNUM_VOID_ELEMENTAL)
      {
         gotvoid = TRUE;
      }
   }
   /*
   if (gotvoid == 1) {
   send_to_char("Your void elemental would surely consume .\n\r", ch);
   return;
   }
   */

   if ( (count == 1 && ch->level < 25) || (count == 2 && ch->level < 32)
   || (count == 3 && ch->level < 40) || (count == 4) )
   {
      send_to_char("You already control as many elementals as you can.\n\r", ch);
      return;
   }

   if ( (ch->in_room->sector_type == SECT_WATER_SWIM)
   || (ch->in_room->sector_type == SECT_UNDERWATER)
   || (ch->in_room->sector_type == SECT_WATER_NOSWIM) )
   {
      send_to_char("There is too much water here to summon a fire elemental.\n\r", ch);
      return;
   }
   if ((weather_info.sky > SKY_CLOUDY) && IS_OUTSIDE(ch))
   {

      send_to_char("The weather is too wet to summon a fire elemental.\n\r", ch);
      return;
   }

   elemental = create_mobile(get_mob_index(MOB_VNUM_FIRE_ELEMENTAL) );
   elemental->level = level;
   elemental->max_hit = ch->max_hit + dice(level, 10);
   if (pyro) elemental->max_hit -= elemental->max_hit/4;
   elemental->hit = elemental->max_hit;
   elemental->damroll += level/2;
   if (pyro) elemental->damroll += elemental->damroll/4;
   elemental->alignment = ch->alignment;
   af.where = TO_AFFECTS;
   af.type = sn;
   af.location = 0;
   af.level = level;
   af.modifier = 0;
   af.duration = 15;
   if (pyro) af.duration /= 2;
   af.bitvector = 0;
   affect_to_char(ch, &af);
   char_to_room(elemental, ch->in_room);

   act("$n arrives in a column of searing flames.", elemental, NULL, NULL, TO_ROOM);

   if ((!IS_NPC(ch)) && (number_percent() > ch->pcdata->learned[sn]))
   {
      act_color("$n says, '{B{6How dare you call me!?!{n'", elemental, NULL, NULL, TO_ROOM);
      elemental->alignment = 0;
      multi_hit(elemental, ch, TYPE_UNDEFINED);
      return;
   }
   else
   {
      SET_BIT(elemental->affected_by, AFF_CHARM);
      add_follower(elemental, ch);
      elemental->leader = ch;
   }
   return;
}

void spell_summon_void_elemental(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *elemental;
   AFFECT_DATA af;
   int count;
   CHAR_DATA *check;
   CHAR_DATA *fch;
   CHAR_DATA *fch_next;

   int gotearth;
   int gotair;
   int gotfire;
   int gotwater;

   if (is_affected(ch, sn))
   {
      send_to_char("You aren't mentally prepared to open the Void yet.\n\r", ch);
      return;
   }

   count = 0;
   gotearth = 0;
   gotair = 0;
   gotfire = 0;
   gotwater = 0;


   for (check = char_list; check != NULL; check = check->next)
   {
      if
      (
         !IS_NPC(check) ||
         check->master != ch
      )
      {
         continue;
      }
      switch (check->pIndexData->vnum)
      {
         default:
         {
            break;
         }
         case (MOB_VNUM_VOID_ELEMENTAL):
         {
            if (!is_affected(check, gsn_seize))
            {
               send_to_char
               (
                  "You can only control one void elemental at a time.\n\r",
                  ch
               );
               return;
            }
            break;
         }
         case (MOB_VNUM_FIRE_ELEMENTAL):
         {
            gotfire = TRUE;
            break;
         }
         case (MOB_VNUM_AIR_ELEMENTAL):
         {
            gotair = TRUE;
            break;
         }
         case (MOB_VNUM_WATER_ELEMENTAL):
         {
            gotwater = TRUE;
            break;
         }
         case (MOB_VNUM_EARTH_ELEMENTAL):
         {
            gotearth = TRUE;
            break;
         }
      }
   }

   if
   (
      !(
         gotearth &&
         gotair &&
         gotfire &&
         gotwater
      )
   )
   {
      send_to_char
      (
         "You must control each of the elements to open the Void.\n\r",
         ch
      );
      return;
   }


   /* lose the charmies */
   for ( fch = char_list; fch != NULL; fch = fch_next )
   {
      fch_next = fch->next;
      if
      (
         !IS_NPC(fch) ||
         (
            fch->pIndexData->vnum != MOB_VNUM_EARTH_ELEMENTAL &&
            fch->pIndexData->vnum != MOB_VNUM_FIRE_ELEMENTAL  &&
            fch->pIndexData->vnum != MOB_VNUM_WATER_ELEMENTAL &&
            fch->pIndexData->vnum != MOB_VNUM_AIR_ELEMENTAL
         )
      )
      {
         continue;
      }
      if
      (
         IS_NPC(fch) &&
         (
            is_affected(fch, gsn_animate_dead) ||
            IS_AFFECTED(fch, AFF_CHARM)
         ) &&
         fch->master == ch
      )
      {
         REMOVE_BIT(fch->affected_by, AFF_CHARM);
         affect_strip(fch, gsn_animate_dead);
         extract_char(fch, TRUE);
      }
   }

   elemental = create_mobile(get_mob_index(MOB_VNUM_VOID_ELEMENTAL) );
   elemental->level = level;
   elemental->max_hit = ch->max_hit + 3*(dice(level, 10));
   elemental->hit = elemental->max_hit;
   elemental->damroll += level/2+10;
   elemental->alignment = 0;
   af.where = TO_AFFECTS;
   af.type = sn;
   af.location = 0;
   af.level = level;
   af.modifier = 0;
   af.duration = 15;
   af.bitvector = 0;
   affect_to_char(ch, &af);
   char_to_room(elemental, ch->in_room);

   act("\n\rThe elementals merge and explode! A savage rift in reality appears,\n\rand a strange being steps forth!\n\r", elemental, NULL, NULL, TO_ROOM);

   if ((!IS_NPC(ch)) && (number_percent() > ch->pcdata->learned[sn]))
   {
      act_color("$n says, '{B{6How dare you call me!?!{n'", elemental, NULL, NULL, TO_ROOM);
      multi_hit(elemental, ch, TYPE_UNDEFINED);
      return;
   }
   else
   {
      SET_BIT(elemental->affected_by, AFF_CHARM);
      add_follower(elemental, ch);
      elemental->leader = ch;
   }
   return;
}


void spell_summon_water_elemental(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *elemental;
   AFFECT_DATA af;
   int count;
   int gotvoid;
   CHAR_DATA *check;
   int aqua = 0;

   if (!IS_NPC(ch) && ch->pcdata->special == SUBCLASS_AQUAMANCER)
   aqua = 1;

   if (check_peace(ch)) return;

   if (is_affected(ch, sn))
   {
      send_to_char("You can't call upon a water elemental yet.\n\r", ch);
      return;
   }
   count = 0;
   gotvoid = 0;

   for (check = char_list; check != NULL; check = check->next)
   {
      if
      (
         !IS_NPC(check) ||
         check->master != ch ||
         is_affected(check, gsn_seize)
      )
      {
         continue;
      }
      if
      (
         check->pIndexData->vnum == MOB_VNUM_FIRE_ELEMENTAL ||
         check->pIndexData->vnum  == MOB_VNUM_AIR_ELEMENTAL ||
         check->pIndexData->vnum  == MOB_VNUM_EARTH_ELEMENTAL ||
         check->pIndexData->vnum  == MOB_VNUM_WATER_ELEMENTAL
      )
      {
         count++;
      }
      else if (check->pIndexData->vnum == MOB_VNUM_VOID_ELEMENTAL)
      {
         gotvoid = TRUE;
      }
   }
   /*
   if (gotvoid == 1) {
   send_to_char("Your void elemental would surely consume any other elementals you summon.\n\r", ch);
   return;
   }
   */
   if ( (count == 1 && ch->level < 25) || (count == 2 && ch->level < 32)
   || (count == 3 && ch->level < 40) || (count == 4) )
   {
      send_to_char("You already control as many elementals as you can.\n\r", ch);
      return;
   }

   if ( (ch->in_room->sector_type != SECT_WATER_SWIM) &&
   (ch->in_room->sector_type != SECT_UNDERWATER)
   && (ch->in_room->sector_type != SECT_WATER_NOSWIM) )
   {
      send_to_char("There is not enough water here to summon a water elemental.\n\r", ch);
      return;
   }


   elemental = create_mobile(get_mob_index(MOB_VNUM_WATER_ELEMENTAL) );

   elemental->level = level;
   elemental->max_hit = ch->max_hit + dice(level, 10);
   elemental->hit = elemental->max_hit;
   elemental->damroll += level/2;
   elemental->alignment = ch->alignment;
   if (aqua)
   {
      af.where = TO_AFFECTS;
      af.type = gsn_imbue_regeneration;
      af.location = APPLY_REGENERATION;
      af.modifier = level/11;
      af.level = level;
      af.bitvector = 0;
      af.duration = -1;
      affect_to_char(elemental, &af);
      SET_BIT(elemental->affected_by, AFF_SWIM);
   }
   af.where = TO_AFFECTS;
   af.type = sn;
   af.location = 0;
   af.level = level;
   af.modifier = 0;
   af.duration = 15;
   if (aqua) af.duration /= 2;
   af.bitvector = 0;
   affect_to_char(ch, &af);

   char_to_room(elemental, ch->in_room);

   act("Water surges around you as $n arrives.", elemental, NULL, NULL, TO_ROOM);

   if ((!IS_NPC(ch)) && (number_percent() > ch->pcdata->learned[sn]))
   {
      act("You dare to call upon me!?!", elemental, NULL, NULL, TO_ROOM);
      elemental->alignment = 0;
      multi_hit(elemental, ch, TYPE_UNDEFINED);
      return;
   }
   else
   {
      SET_BIT(elemental->affected_by, AFF_CHARM);
      add_follower(elemental, ch);
      elemental->leader = ch;
   }
   return;
}


void spell_summon_earth_elemental(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *elemental;
   AFFECT_DATA af;
   int count;
   int gotvoid;
   CHAR_DATA *check;
   int geo = 0;

   if (!IS_NPC(ch) && ch->pcdata->special == SUBCLASS_GEOMANCER)
   geo = 1;

   if (check_peace(ch)) return;

   if (is_affected(ch, sn))
   {
      send_to_char("You can't call upon an earth elemental yet.\n\r", ch);
      return;
   }
   count = 0;
   gotvoid = 0;
   for (check = char_list; check != NULL; check = check->next)
   {
      if
      (
         !IS_NPC(check) ||
         check->master != ch ||
         is_affected(check, gsn_seize)
      )
      {
         continue;
      }
      if
      (
         check->pIndexData->vnum == MOB_VNUM_FIRE_ELEMENTAL ||
         check->pIndexData->vnum  == MOB_VNUM_AIR_ELEMENTAL ||
         check->pIndexData->vnum  == MOB_VNUM_EARTH_ELEMENTAL ||
         check->pIndexData->vnum  == MOB_VNUM_WATER_ELEMENTAL
      )
      {
         count++;
      }
      else if (check->pIndexData->vnum == MOB_VNUM_VOID_ELEMENTAL)
      {
         gotvoid = TRUE;
      }
   }
   /*
   if (gotvoid == 1) {
   send_to_char("Your void elemental would surely consume any other elementals you summon.\n\r", ch);
   return;
   }
   */
   if ( (count == 1 && ch->level < 25) || (count == 2 && ch->level < 32)
   || (count == 3 && ch->level < 40) || (count == 4) )
   {
      send_to_char("You already control as many elementals as you can.\n\r", ch);
      return;
   }

   if ( (ch->in_room->sector_type == SECT_WATER_SWIM) ||
   (ch->in_room->sector_type == SECT_UNDERWATER)
   || (ch->in_room->sector_type == SECT_WATER_NOSWIM)
   || (ch->in_room->sector_type == SECT_AIR) )
   {
      send_to_char("You are not in an area with enough raw earth.\n\r", ch);
      return;
   }

   elemental = create_mobile(get_mob_index(MOB_VNUM_EARTH_ELEMENTAL) );
   elemental->level = ch->level;
   elemental->max_hit = ch->max_hit + dice(ch->level, 10);
   if (geo) elemental->max_hit += elemental->max_hit/4;
   elemental->hit = elemental->max_hit;
   elemental->damroll += ch->level/2;
   if (geo) elemental->damroll -= elemental->damroll/4;
   elemental->alignment = ch->alignment;
   if (geo)
   {
      af.where     = TO_AFFECTS;
      af.type      = gsn_stone_skin;
      af.level     = level;
      af.duration  = -1;
      af.location  = APPLY_AC;
      af.modifier  = -40;
      af.bitvector = 0;
      affect_to_char( elemental, &af );
   }
   af.where = TO_AFFECTS;
   af.type = sn;
   af.location = 0;
   af.level = level;
   af.modifier = 0;
   af.duration = 15;
   if (geo) af.duration /= 2;
   af.bitvector = 0;
   affect_to_char(ch, &af);
   char_to_room(elemental, ch->in_room);

   act("The earth rumbles and $n bursts out from the ground.", elemental, NULL, NULL, TO_ROOM);

   if ((!IS_NPC(ch)) && (number_percent() > ch->pcdata->learned[sn]))
   {
      act("You dare to call upon me!?!", elemental, NULL, NULL, TO_ROOM);
      elemental->alignment = 0;
      multi_hit(elemental, ch, TYPE_UNDEFINED);
      return;
   }
   else
   {
      SET_BIT(elemental->affected_by, AFF_CHARM);
      add_follower(elemental, ch);
      elemental->leader = ch;
   }
   return;
}

void spell_summon_air_elemental(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *elemental;
   AFFECT_DATA af;
   int count;
   int gotvoid;
   CHAR_DATA *check;
   int aero = 0;

   if (!IS_NPC(ch) && ch->pcdata->special == SUBCLASS_AREOMANCER)
   aero = 1;

   if (check_peace(ch)) return;

   if (is_affected(ch, sn))
   {
      send_to_char("You can't call upon an air elemental yet.\n\r", ch);
      return;
   }
   count = 0;
   gotvoid = 0;
   for (check = char_list; check != NULL; check = check->next)
   {
      if
      (
         !IS_NPC(check) ||
         check->master != ch ||
         is_affected(check, gsn_seize)
      )
      {
         continue;
      }
      if
      (
         check->pIndexData->vnum == MOB_VNUM_FIRE_ELEMENTAL ||
         check->pIndexData->vnum  == MOB_VNUM_AIR_ELEMENTAL ||
         check->pIndexData->vnum  == MOB_VNUM_EARTH_ELEMENTAL ||
         check->pIndexData->vnum  == MOB_VNUM_WATER_ELEMENTAL
      )
      {
         count++;
      }
      else if (check->pIndexData->vnum == MOB_VNUM_VOID_ELEMENTAL)
      {
         gotvoid = TRUE;
      }
   }
   /*
   if (gotvoid == 1) {
   send_to_char("Your void elemental would surely consume any other elementals you summon.\n\r", ch);
   return;
   }
   */
   if ( (count == 1 && ch->level < 25) || (count == 2 && ch->level < 32)
   || (count == 3 && ch->level < 40) || (count == 4) )
   {
      send_to_char("You already control as many elementals as you can.\n\r", ch);
      return;
   }
   /*
   if  (ch->in_room->sector_type != SECT_AIR)
   {
   send_to_char("You need to be in the air to find an air elemental.\n\r", ch);
   return;
   }
   */
   elemental = create_mobile(get_mob_index(MOB_VNUM_AIR_ELEMENTAL) );
   elemental->level = level;
   elemental->max_hit = ch->max_hit + dice(level, 10);
   elemental->hit = elemental->max_hit;
   elemental->damroll += level/2;
   elemental->alignment = ch->alignment;
   if (aero)
   {
      SET_BIT(elemental->affected_by, AFF_PASS_DOOR);
      SET_BIT(elemental->affected_by, AFF_FLYING);
   }
   af.where = TO_AFFECTS;
   af.type = sn;
   af.location = 0;
   af.level = level;
   af.modifier = 0;
   af.duration = 15;
   if (aero) af.duration /= 2;
   af.bitvector = 0;
   affect_to_char(ch, &af);
   char_to_room(elemental, ch->in_room);

   act("$n forms before you in a violent vortex of wind.", elemental, NULL, NULL, TO_ROOM);

   if ((!IS_NPC(ch)) && (number_percent() > ch->pcdata->learned[sn]))
   {
      act_color("$n says, '{B{6You dare to call upon me!?!{n'", elemental, NULL, NULL, TO_ROOM);
      elemental->alignment = 0;
      multi_hit(elemental, ch, TYPE_UNDEFINED);
      return;
   }
   else
   {
      SET_BIT(elemental->affected_by, AFF_CHARM);
      add_follower(elemental, ch);
      elemental->leader = ch;
   }
   return;
}

void spell_earthmaw(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   int dam, save_num;
   int count;
   if ( (ch->in_room->sector_type == SECT_AIR) ||
   ( ch->in_room->sector_type == SECT_UNDERWATER)
   || ( ch->in_room->sector_type == SECT_WATER_SWIM)
   || ( ch->in_room->sector_type == SECT_WATER_NOSWIM) )
   {
      send_to_char("You can't do that in this environment.\n\r", ch);
      return;
   }
   /*
   *   if (IS_SET(ch->in_room->room_flags, ROOM_INDOORS) )
   *   {
   *       send_to_char("You can not create an earthmaw indoors.\n\r", ch);
   *       return;
   *   }
   */
   act("$n sunders the ground beneath $N.", ch, NULL, victim, TO_NOTVICT);
   act("$n sunders the ground beneath you.", ch, NULL, victim, TO_VICT);
   act("You sunder the ground beneath $N.", ch, NULL, victim, TO_CHAR);
   save_num = 0;
   if (check_spellcraft(ch, sn))
   dam = spellcraft_dam(level, 10);
   else
   dam = dice(level, 10);

   dam += dice(level, 4);
   for (count = 0; count < 2; count++)
   {
      if (saves_spell(ch, level, victim, DAM_BASH, SAVE_SPELL) )
      save_num++;
   }
   if (save_num == 0)
   {
      act("$n cries out as $e is crushed savagely within the rift!", victim, NULL, NULL, TO_ROOM);
      send_to_char("You fall into the rift and scream in agony as it crushes you!\n\r", victim);
      damage(ch, victim, dam*race_adjust(ch)/25.0, sn, DAM_BASH, TRUE);
      return;
   }
   dam /= save_num * 2;
   damage(ch, victim, dam*race_adjust(ch)/25.0, sn, DAM_BASH, TRUE);

   return;
}

void spell_flesh_to_stone(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   return;
}

void spell_tsunami(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   CHAR_DATA* vch;
   CHAR_DATA* vch_next;
   int dam;

   if (check_peace(ch))
   {
      return;
   }

   if
   (
      ch->in_room->sector_type != SECT_WATER_SWIM &&
      ch->in_room->sector_type != SECT_UNDERWATER &&
      ch->in_room->sector_type != SECT_WATER_NOSWIM
   )
   {
      send_to_char("You need to be on water to do that.\n\r", ch);
      return;
   }

   act
   (
      "$n raises $s arms and causes the waters to rise up in violence.",
      ch,
      NULL,
      NULL,
      TO_ROOM
   );
   send_to_char
   (
      "You call upon the water around you to surge at your foes.\n\r",
      ch
   );
   for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
   {
      vch_next = vch->next_in_room;
      if
      (
         is_same_group(vch, ch) ||
         (
            IS_IMMORTAL(vch) &&
            !can_see(ch, vch) &&
            wizi_to(vch, ch)
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
         )
      )
      {
         switch(number_range(0, 2))
         {
            default:
            {
               sprintf
               (
                  log_buf,
                  "Die, %s, you sorcerous dog!",
                  PERS(ch, vch)
               );
               break;
            }
            case (2):
            {
               sprintf
               (
                  log_buf,
                  "Help! %s is casting a spell on me!",
                  PERS(ch, vch)
               );
               break;
            }
         }
         do_myell(vch, log_buf);
         sprintf
         (
            log_buf,
            "[%s] cast tsunami upon [%s] at %d",
            ch->name,
            vch->name,
            ch->in_room->vnum
         );
         log_string(log_buf);
      }

      dam = dice(level, 11);
      if (saves_spell(ch, level, vch, DAM_DROWNING, SAVE_SPELL))
      {
         dam /= 2;
      }
      if (saves_spell(ch, level, vch, DAM_DROWNING, SAVE_SPELL))
      {
         dam /= 2;
      }
      damage(ch, vch, dam * race_adjust(ch) / 25.0, sn, DAM_DROWNING, TRUE);
   }
   act("$n's tsunami washes past and subsides.", ch, NULL, NULL, TO_ROOM);
   send_to_char("Your tsunami washes past and subsides.\n\r", ch);
   return;
}


/* big block of channeler spells */

void spell_drain(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   OBJ_DATA *obj = (OBJ_DATA *) vo;
   AFFECT_DATA *paf;
   int drain, fail;

   if (obj->wear_loc != WEAR_NONE)
   {
      send_to_char("You can't do that on a worn object.\n\r", ch);
      return;
   }
   if (!IS_SET(obj->extra_flags, ITEM_MAGIC))
   {
      send_to_char("That item is not magical.\n\r", ch);
      return;
   }
   switch (obj->item_type)
   {
      default:
      {
         /*
         sprintf(log_buf, "[*****] BUG: Invalid item drain type: %s (%d).",
         item_name(obj->item_type), obj->item_type);
         log_string(log_buf);
         */
         /* bug("Invalid item drain type: %s (%d).", item_name(obj->item_type), obj->item_type); */
         drain = 1;
         break;
      }

      case ITEM_LIGHT     : if (obj->value[2] == -1) drain = 9;
      else drain = 4;
      break;
      case ITEM_SCROLL    :
      case ITEM_WAND      :
      case ITEM_STAFF     : drain = obj->value[0]/10;                     break;
      case ITEM_WEAPON    : drain = (obj->value[1] + obj->value[2])/3;    break;
      case ITEM_TREASURE  : drain = 4;     break;
      case ITEM_ARMOR     : drain = 12;    break;
      case ITEM_POTION    : drain = 5;     break;
      case ITEM_SPELLBOOK : drain = 15;    break;
   }

   for (paf = obj->affected; paf != NULL; paf = paf->next)
   {
      if (paf->location > APPLY_NONE && paf->location < APPLY_SEX)
      drain += paf->modifier;
      else if ( (paf->location == APPLY_HITROLL) || (paf->location == APPLY_DAMROLL) )
      drain += paf->modifier;
      else continue;
   }

   drain *= dice(3, 3);
   drain *= obj->level/2;

   drain = UMIN(drain*race_adjust(ch)/25.0, 200);

   fail = 95 * (IS_NPC(ch) ? 100 : ch->pcdata->learned[sn]);
   act("$p vaporises in a flash of light!", ch, obj, NULL, TO_ROOM);
   if (number_percent() > fail)
   {
      act("$p vaporises in a flash of light but you fail to channel the energy.", ch, obj, NULL, TO_CHAR);
      extract_obj(obj, FALSE);
      return;
   }
   act("$p vaporises in a flash of light and you feel the energy surge through you.", ch, obj, NULL, TO_CHAR);
   extract_obj(obj, FALSE);
   ch->mana = UMIN(ch->mana + drain, ch->max_mana);
   return;
}

void spell_disenchant_weapon(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   OBJ_DATA *obj = (OBJ_DATA *) vo;
   AFFECT_DATA *paf;
   AFFECT_DATA *paf_next;
   int fail;
   int check;

   if (obj->pIndexData->vnum == OBJ_VNUM_GENOCIDE_AXE)
   {
      send_to_char("The only disenchanting you will do is make yourself miserable"
      " when you try to alter the axe.\n\rIt cannot be done.\n\r", ch);
      return;
   }

   if (obj->item_type != ITEM_WEAPON)
   {
      send_to_char("But that item is not a weapon.\n\r", ch);
      return;
   }

   if (is_affected_obj(obj, gsn_alchemy))
   {
      send_to_char("A simple disenchantment will not work, try fade.\n\r", ch);
      return;
   }
   if (obj->wear_loc != -1)
   {
      send_to_char("The item must be in your inventory.\n\r", ch);
      return;
   }
   check = number_percent();

   fail = URANGE(5, 25 + level * (IS_NPC(ch) ? 100 : ch->pcdata->learned[sn]), 95*race_adjust(ch)/25.0);
   if ((check > 3/2*fail ) || (check > 94) )
   {
      act("$p shudders and explodes!", ch, obj, NULL, TO_ROOM);
      act("$p shudders and explodes!", ch, obj, NULL, TO_CHAR);
      extract_obj(obj, FALSE);
      return;
   }
   if (check > fail)
   {
      act("$p glows faintly but then fades.", ch, obj, NULL, TO_ROOM);
      act("$p glows faintly but nothing happens.", ch, obj, NULL, TO_CHAR);
      return;
   }
   act("$p glows brightly and then fades to a dull lustre.", ch, obj, NULL, TO_ROOM);
   act("$p glows brightly and then fades to a dull lustre.", ch, obj, NULL, TO_CHAR);

   for (paf = obj->affected; paf != NULL; paf = paf_next)
   {
      paf_next = paf->next;
      if (paf->type != gsn_alchemy)
      free_affect(paf);
   }
   obj->affected = NULL;

   REMOVE_BIT(obj->extra_flags, ITEM_GLOW);
   REMOVE_BIT(obj->extra_flags, ITEM_HUM);
   REMOVE_BIT(obj->extra_flags, ITEM_MAGIC);
   REMOVE_BIT(obj->extra_flags, ITEM_INVIS);
   REMOVE_BIT(obj->extra_flags, ITEM_NODROP);
   REMOVE_BIT(obj->extra_flags, ITEM_NOREMOVE);
   return;
}

void spell_disenchant_armor(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   OBJ_DATA *obj = (OBJ_DATA *) vo;
   AFFECT_DATA *paf;
   AFFECT_DATA *paf_next;
   int fail;
   int check;

   if (obj->item_type != ITEM_ARMOR)
   {
      send_to_char("But that item is not a piece of armor.\n\r", ch);
      return;
   }
   if (is_affected_obj(obj, gsn_alchemy))
   {
      send_to_char("A simple disenchantment will not work, try fade.\n\r", ch);
      return;
   }
   if (obj->wear_loc != -1)
   {
      send_to_char("The item must be in your inventory.\n\r", ch);
      return;
   }
   check = number_percent();

   fail = URANGE(5, 25 + level * (IS_NPC(ch) ? 100 : ch->pcdata->learned[sn]), 95*race_adjust(ch)/25.0);
   if ((check > 3/2*fail ) || (check > 94) )
   {
      act("$p shudders and explodes!", ch, obj, NULL, TO_ROOM);
      act("$p shudders and explodes!", ch, obj, NULL, TO_CHAR);
      extract_obj(obj, FALSE);
      return;
   }
   if (check > fail)
   {
      act("$p glows faintly but then fades.", ch, obj, NULL, TO_ROOM);
      act("$p glows faintly but nothing happens.", ch, obj, NULL, TO_CHAR);
      return;
   }
   act("$p glows brightly and then fades to a dull lustre.", ch, obj, NULL, TO_ROOM);
   act("$p glows brightly and then fades to a dull lustre.", ch, obj, NULL, TO_CHAR);

   for (paf = obj->affected; paf != NULL; paf = paf_next)
   {
      paf_next = paf->next;
      if (paf->type != gsn_alchemy)
      free_affect(paf);
   }
   obj->affected = NULL;

   REMOVE_BIT(obj->extra_flags, ITEM_GLOW);
   REMOVE_BIT(obj->extra_flags, ITEM_HUM);
   REMOVE_BIT(obj->extra_flags, ITEM_MAGIC);
   REMOVE_BIT(obj->extra_flags, ITEM_INVIS);
   REMOVE_BIT(obj->extra_flags, ITEM_NODROP);
   REMOVE_BIT(obj->extra_flags, ITEM_NOREMOVE);

   obj->enchanted = FALSE;
   return;
}

void spell_warding(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   AFFECT_DATA af;

   if (house_down(ch, HOUSE_LIFE))
   return;

   if (is_affected(ch, sn))
   {
      send_to_char("You are already guarded by runes of warding.\n\r", ch);
      return;
   }
   if (ch->hit > ch->max_hit)
   {
      send_to_char("The runes of warding have not yet faded from the air around you.\n\r", ch);
      return;
   }

   af.where = TO_AFFECTS;
   af.type = sn;
   af.duration = 24;
   af.level = level;
   af.bitvector = 0;
   af.location = APPLY_HIT;
   af.modifier = ch->level * 4;
   affect_to_char(ch, &af);
   ch->hit += af.modifier;

   af.modifier = ch->level * -1;
   af.location = APPLY_AC;
   affect_to_char(ch, &af);

   send_to_char("You draw holy symbols of warding around yourself.\n\r", ch);
   act("$n draws holy symbols of warding around $mself.", ch, NULL, NULL, TO_ROOM);
   return;
}

void spell_repent(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   int dam;

   if (house_down(ch, HOUSE_LIFE))
   return;

   act("$n calls upon the heavens for $N to repent!", ch, NULL, victim, TO_NOTVICT);
   act("$n calls upon the heavens for you to repent!", ch, NULL, victim, TO_VICT);
   act("You call upon the heavens for $N to repent!", ch, NULL, victim, TO_CHAR);

   if (!EVIL_TO(ch, victim))
   {
      act("$n is unaffected.", victim, NULL, NULL, TO_ROOM);
      act("You are unaffected.", victim, NULL, NULL, TO_CHAR);
      return;
   }
   dam = dice(level, 7);

   if ((IS_SET(victim->act, ACT_UNDEAD) && number_percent() > 70)
   || (IS_SET(victim->act2, PLR_LICH) && number_percent() > 90))
   {
      act("The divine appeal begins to vaporise $n!", victim, NULL, NULL, TO_ROOM);
      act("The divine appeal begins to vaporise you!", victim, NULL, NULL, TO_VICT);
      dam += dice(level, 9);
   }

   if (!(saves_spell(ch, level, victim, DAM_LIGHT, SAVE_SPELL)))
   if (!(saves_spell(ch, level, victim, DAM_LIGHT, SAVE_SPELL)))
   {
      if (!is_affected(victim, sn)){
         AFFECT_DATA af;
         af.where = TO_AFFECTS;
         af.type = sn;
         af.modifier = 0;
         af.level = level;

         if ( ch->level < 31 )
         af.duration = 0;
         else if ( ch->level < 41 )
         af.duration = 1;
         else
         af.duration = 2;

         af.location = APPLY_NONE;
         af.bitvector = AFF_CURSE;
         affect_to_char(victim, &af);
         act("$N is lightbound!", ch, NULL, victim, TO_ROOM);
         send_to_char("You are lightbound!\n\r", victim);
         act("$E is lightbound!", ch, NULL, victim, TO_CHAR);
         /*        send_to_char("They are lightbound!\n\r", ch); */
      }
   }
   if (saves_spell(ch, level, victim, DAM_LIGHT, SAVE_SPELL))
   dam /= 2;

   damage(ch, victim, dam, sn, DAM_LIGHT, TRUE);

   return;
}

void spell_faith_healing(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   int hp;

   if (house_down(ch, HOUSE_LIFE))
   return;

   if (victim == ch)
   {
      send_to_char("You can't heal yourself.\n\r", ch);
      return;
   }

   if (is_affected(victim, gsn_corrupt("smite", &gsn_smite)))
   {
      act("$n feels better, but $s wounds do not close.", ch, NULL, victim, TO_CHAR);
      send_to_char("You feel better, but your wounds do not close.\n\r", victim);
      hp = (ch->level / 2) + number_range(-10, 10);
      damage( ch, ch, hp, gsn_faith_healing, DAM_LIGHT, TRUE );
      return;
   }

   hp = ch->level * 2;

   act("Warmth passes out of your body and into $N's.", ch, NULL, victim, TO_CHAR);
   act("A feeling of warmth enters your body as $n grants you healing.", ch, NULL, victim, TO_VICT);
   victim->hit = UMIN(victim->hit + hp, victim->max_hit);

   hp = (ch->level / 2) + number_range(-10, 10);
   damage( ch, ch, hp, gsn_faith_healing, DAM_LIGHT, TRUE );

   return;
}

/* Commented out, 7/26/99, Maelstrom

void spell_prevent(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   AFFECT_DATA af;

   if (house_down(ch, HOUSE_LIFE))
   return;

   if (is_affected(ch, sn))
   {
      send_to_char("You are already as one with life as you can be.\n\r", ch);
      return;
   }
   if (ch->hit > ch->max_hit)
   {
      send_to_char("You are already overflowing with health.\n\r", ch);
      return;
   }

   af.where = TO_AFFECTS;
   af.type = sn;
   af.duration = 24;
   af.level = level;
   af.bitvector = 0;
   af.location = APPLY_HIT;
   af.modifier =
   IS_NPC(ch) ? (ch->max_hit+ch->max_mana)/4 :
   (ch->pcdata->perm_hit+ch->pcdata->perm_mana)/4;
   affect_to_char(ch, &af);
   af.modifier = ch->level*3;
   af.location = APPLY_MOVE;
   affect_to_char(ch, &af);
   ch->hit +=
   IS_NPC(ch) ? (ch->max_hit+ch->max_mana)/4 :
   (ch->pcdata->perm_hit+ch->pcdata->perm_mana)/4;
   send_to_char("You feel yourself grow healthier as you come closer to life.\n\r", ch);
   return;

}

void spell_life_transfer(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   int hp;

   if (house_down(ch, HOUSE_LIFE))
   return;

   if (victim == ch)
   {
      send_to_char("You can't grant life to yourself.\n\r", ch);
      return;
   }

   if (ch->hit < 10)
   hp = ch->hit;
   else hp = 10;
   act("$n transfers part of $s life to $N.", ch, NULL, victim, TO_NOTVICT);
   act("You transfer part of your life to $N.", ch, NULL, victim, TO_CHAR);
   act("$n transfers part of $s life to you.", ch, NULL, victim, TO_VICT);

   ch->hit -= hp;
   hp += ch->level/2;
   hp += number_range(0, ch->level);
   victim->hit = UMIN(victim->hit + hp, victim->max_hit);
   return;
}

void spell_sunbolt(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   int dam;

   if (house_down(ch, HOUSE_LIFE))
   return;

   act("$n prays to Adorno and a beam of sunlight erupts from $s hands!", ch, NULL, NULL, TO_ROOM);
   send_to_char("You pray to Adorno and a beam of sunlight erupts from your hands!\n\r", ch);

   if (IS_GOOD(victim))
   {
      act("$n is unaffected by the blast of light.", victim, NULL, NULL, TO_ROOM);
      act("You are unaffected by the blast of light.", victim, NULL, NULL, TO_CHAR);
      return;
   }
   dam = dice(level, 7);
   if ((IS_SET(victim->act, ACT_UNDEAD) && number_percent() > 70)
   || (IS_SET(victim->act2, PLR_LICH) && number_percent() > 90))
   {
      act("The positive energy begins to vaporise $n!", victim, NULL, NULL, TO_ROOM);
      act("The positive energy begins to vaporise you!", victim, NULL, NULL, TO_VICT);
      dam += dice(level, 9);
   }

   if (!(saves_spell(ch, level, victim, DAM_LIGHT, SAVE_SPELL)))
   if (!(saves_spell(ch, level, victim, DAM_LIGHT, SAVE_SPELL)))
   {
      if (!is_affected(victim, sn)){
         AFFECT_DATA af;
         af.where = TO_AFFECTS;
         af.type = sn;
         af.modifier = 0;
         af.level = level;
         af.duration = 2;
         af.location = APPLY_NONE;
         af.bitvector = AFF_BLIND;
         affect_to_char(victim, &af);
         act("$N is blinded by the light!", ch, NULL, victim, TO_ROOM);
         send_to_char("You are blinded by the light!\n\r", victim);
         act("$E is blinded by the light!", ch, NULL, victim, TO_CHAR);
      }
   }
   if (saves_spell(ch, level, victim, DAM_LIGHT, SAVE_SPELL))
   dam /= 2;

   damage(ch, victim, dam, sn, DAM_LIGHT, TRUE);

   return;
}
*/

struct blade_type
{
   const char* name;
   const int   dam_type;
};

const struct blade_type blades[] =
{
   {
      "A shadowy blade",
      DAM_MENTAL
   },
   {
      "A fiery blade",
      DAM_FIRE
   },
   {
      "A frosty blade",
      DAM_COLD
   },
   {
      "A watery blade",
      DAM_DROWNING
   },
   {
      "A blunted blade",
      DAM_BASH
   },
   {
      "A blade of lightning",
      DAM_LIGHTNING
   },
   {
      "An acidic blade",
      DAM_ACID
   },
   {
      "An ebon blade",
      DAM_NEGATIVE
   },
   {
      "A blade of blessedness",
      DAM_HOLY
   },
   {
      "A pulsing blade",
      DAM_ENERGY
   },
   {
      "A brightly glowing blade",
      DAM_LIGHT
   },
   {
      "A screaming blade",
      DAM_SOUND
   },
   {
      "A poisonous blade",
      DAM_POISON
   },
   {
      "A pestulant blade",
      DAM_DISEASE
   },
   {
      "A spike-edged blade",
      DAM_PIERCE
   },
   {
      "A razor-edged blade",
      DAM_SLASH
   },
   {
      "A singing blade",
      DAM_CHARM
   },
   {
      "A vorpal blade",
      DAM_HARM
   },
   {
      "A blade of earth",
      DAM_NATURE
   },
   {
      "",  /* Placeholder, following cannot be used for IS_NORMAL */
      DAM_NONE
   },
   {
      "A wooden blade",
      DAM_WOOD
   },
   {
      "An iron blade",
      DAM_IRON
   },
   {
      "A silver blade",
      DAM_SILVER
   },
   {
      "A shining metallic blade",
      DAM_METAL
   },
   {
      "A translucent blade",
      DAM_SPELLS
   },
   {
      NULL,
      DAM_NONE
   }
};

void spell_spiritblade(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   CHAR_DATA* victim = (CHAR_DATA*)vo;
   int dam;
   char buf[MAX_STRING_LENGTH];
   int slot = -1;
   int counter;

   if
   (
      (
         !IS_NPC(ch) ||
         (
            ch->pIndexData->vnum != MOB_VNUM_DEMON1 &&
            ch->pIndexData->vnum != MOB_VNUM_ENFORCER
         )
      ) &&
      house_down(ch, HOUSE_CONCLAVE)
   )
   {
      /* Griekul and Khalshar cast spiritblade */
      return;
   }

   if (number_percent() < 20)
   {
      if (is_supplicating)
      {
         act
         (
            "$z rips through $N's mind, probing for a weakness!",
            ch,
            NULL,
            victim,
            TO_CHAR
         );
         act
         (
            "$z rips through your mind, probing for a weakness!",
            ch,
            NULL,
            victim,
            TO_VICT
         );
      }
      else
      {
         act
         (
            "Your magic rips through $N's mind, probing for a weakness!",
            ch,
            NULL,
            victim,
            TO_CHAR
         );
         act
         (
            "$n's magic rips through your mind, probing for a weakness!",
            ch,
            NULL,
            victim,
            TO_VICT
         );
      }
      for (counter = 0; blades[counter].name != NULL; counter++)
      {
         if (check_immune(victim, blades[counter].dam_type) == IS_VULNERABLE)
         {
            slot = counter;
            break;
         }
      }
      if (slot == -1)
      {
         for
         (
            counter = 0;
            (
               blades[counter].name != NULL &&
               blades[counter].name[0] != '\0'
            );
            counter++
         )
         {
            if (check_immune(victim, blades[counter].dam_type) == IS_NORMAL)
            {
               slot = counter;
               break;
            }
         }
      }
      if (slot == -1)
      {
         for (counter = 0; blades[counter].name != NULL; counter++)
         {
            if (check_immune(victim, blades[counter].dam_type) == IS_RESISTANT)
            {
               slot = counter;
               break;
            }
         }
      }
      if (slot == -1)
      {
         slot = 0;
      }
   }
   else
   {
      slot = 0;
   }
   strcpy(buf, blades[slot].name);
   act
   (
      strcat(buf, " appears above $n and strikes down!"),
      victim,
      NULL,
      NULL,
      TO_ROOM
   );
   strcpy(buf, blades[slot].name);
   send_to_char
   (
      strcat(buf, " manifests above you and suddenly descends!\n\r"),
      victim
   );

   dam = dice(level, 5);
   if (saves_spell(ch, level, victim, blades[slot].dam_type, SAVE_SPELL))
   {
      dam /= 2;
   }
   if
   (
      number_percent() <
      (
         level +
         (
            IS_NPC(ch) ?
            100 :
            ch->pcdata->learned[sn]
         )
      ) / 10
   )
   {
      act
      (
         "$n's spiritblade brutally cleaves $N!",
         ch,
         NULL,
         victim,
         TO_NOTVICT
      );
      act("Your spiritblade brutally cleaves $N!", ch, NULL, victim, TO_CHAR);
      act("$n's spiritblade brutally cleaves you!", ch, NULL, victim, TO_VICT);
      dam += dice(level, 2);
      dam += dice(level / 2, 2);
   }
   damage(ch, victim, dam, sn, blades[slot].dam_type, TRUE);
   return;
}




void spell_guard_call(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *guard;
   AFFECT_DATA af;
   CHAR_DATA *check;
   int i;

   if (check_peace(ch)) return;

   if (house_down(ch, HOUSE_ENFORCER))
   return;

   if (is_affected(ch, sn))
   {
      send_to_char("You can't call more guard's yet.\n\r", ch);
      return;
   }

   for (check = char_list; check != NULL; check = check->next)
   {
      if (IS_NPC(check))
      if ( (check->master == ch) && (check->pIndexData->vnum == MOB_VNUM_ENFORCER_GUARD) )
      {
         send_to_char("You still have guards under your command!\n\r", ch);
         return;
      }
   }
   if (ch->in_room->house == HOUSE_OUTLAW)
   {
      send_to_char("There are no guards available within this place of outlaws!\n\r", ch);
      return;
   }

   af.where = TO_AFFECTS;
   af.level = level;
   af.location = 0;
   af.modifier = 0;
   af.duration = 5;
   af.bitvector = 0;
   af.type = sn;
   affect_to_char(ch, &af);
   act("$n shouts, 'Guards! Guards!'", ch, NULL, NULL, TO_ROOM);
   send_to_char("You shout out for guards!\n\r", ch);
   send_to_char("A guard contingent arrives to aid you.\n\r", ch);

   for ( i = 0; i < 3; i++)
   {
      guard = create_mobile(get_mob_index(MOB_VNUM_ENFORCER_GUARD) );
      guard->level = ch->level;
      guard->damroll += level/2;
      guard->max_hit = ch->level*16+number_range(-150, 150);
      guard->hit = guard->max_hit;
      guard->max_move = ch->max_move;
      guard->move = guard->max_move;
      guard->alignment = ch->alignment;
      char_to_room(guard, ch->in_room);
      SET_BIT(guard->affected_by, AFF_CHARM);
      add_follower(guard, ch);
      guard->leader = ch;
   }
   act("A guard contingent arrives to $n's call.", ch, NULL, NULL, TO_ROOM);
   return;
}

void spell_support(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *guard;
   AFFECT_DATA af;
   CHAR_DATA *check;
   int i;
   int n_guards;

   if (check_peace(ch))
   return;

   sprintf(log_buf, "[%s] summoned support at %d.", ch->name, ch->in_room->vnum);
   log_string(log_buf);

   if (house_down(ch, HOUSE_EMPIRE))
   return;

   if (!IS_NPC(ch) && ch->pcdata->house_rank == 6)
   {
      send_to_char("You can not use this.\n\r", ch);
      return;
   }
   if (is_affected(ch, sn))
   {
      send_to_char("You can't call more support yet.\n\r", ch);
      return;
   }

   for (check = char_list; check != NULL; check = check->next)
   {
      if (IS_NPC(check))
      if ( (check->master == ch) && (check->pIndexData->vnum == MOB_VNUM_EMPIRE_G) )
      {
         send_to_char("You still have knights serving you.\n\r", ch);
         return;
      }
   }

   af.where = TO_AFFECTS;
   af.level = level;
   af.location = 0;
   af.modifier = 0;
   af.duration = 12;
   af.bitvector = 0;
   af.type = sn;
   affect_to_char(ch, &af);
   act("$n calls out for support!'", ch, NULL, NULL, TO_ROOM);
   send_to_char("You call out for support!\n\r", ch);

   if ( !IS_NPC(ch) && (ch->pcdata->house_rank == 4 || ch->pcdata->house_rank == 5) )
   {
      n_guards = 3;
      send_to_char("Three Imperial Guards arrive to aid you.\n\r", ch);
   }
   else
   {
      n_guards = 2;
      send_to_char("Two Imperial Guards arrive to aid you.\n\r", ch);
   }

   for ( i = 0; i < n_guards; i++)
   {
      guard = create_mobile(get_mob_index(MOB_VNUM_EMPIRE_G) );
      guard->level = ch->level;
      guard->damroll += level/2;
      guard->max_hit = ch->level*16+number_range(-150, 150);
      guard->hit = guard->max_hit;
      guard->max_move = ch->max_move;
      guard->move = guard->max_move;
      guard->alignment = ch->alignment;
      char_to_room(guard, ch->in_room);
      SET_BIT(guard->affected_by, AFF_CHARM);
      add_follower(guard, ch);
      act("An Imperial Guard now serves $n.", ch, NULL, NULL, TO_ROOM);
      guard->leader = ch;
   }

   return;
}

void spell_backup(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *guard;
   AFFECT_DATA af;
   CHAR_DATA *check;
   int i;

   if (check_peace(ch)) return;

   if (house_down(ch, HOUSE_OUTLAW))
   return;

   if (is_affected(ch, sn))
   {
      send_to_char("You can't call for more backup yet.\n\r", ch);
      return;
   }

   for (check = char_list; check != NULL; check = check->next)
   {
      if (IS_NPC(check))
      if ( (check->master == ch) && (check->pIndexData->vnum == MOB_VNUM_OUTLAW_GIANT) )
      {
         send_to_char("You still have giants under your control!\n\r", ch);
         return;
      }
   }

   if (ch->in_room->house == HOUSE_ENFORCER)
   {
      send_to_char("Your backup refuses to come here.\n\r", ch);
      return;
   }

   act("$n calls for backup!", ch, NULL, NULL, TO_ROOM);
   send_to_char("You call out, and a pair of strong giants come to your aid.\n\r", ch);

   if (ch->gold < ch->level*2)
   {
      send_to_char("You search your pockets for the fee but find nothing.\n\r", ch);
      send_to_char("The giants frown, shrug, and leave in search of a richer master.\n\r", ch);
      act("Two giants answer the call, frown, and run off in disappointment.", ch, NULL, NULL, TO_ROOM);
      return;
   }
   send_to_char("You pay the giants for their services.\n\r", ch);
   ch->gold -= ch->level*2;

   af.where = TO_AFFECTS;
   af.level = level;
   af.location = 0;
   af.modifier = 0;
   af.duration = 5;
   af.bitvector = 0;
   af.type = sn;
   affect_to_char(ch, &af);

   for ( i = 0; i < 2; i++)
   {
      guard = create_mobile(get_mob_index(MOB_VNUM_OUTLAW_GIANT) );
      guard->level = ch->level;
      guard->damroll += level/2;
      guard->max_hit = ch->level*16+number_range(-150, 150);
      guard->hit = guard->max_hit;
      guard->max_move = ch->max_move;
      guard->move = guard->max_move;
      guard->pause = 30;
      char_to_room(guard, ch->in_room);
      SET_BIT(guard->affected_by, AFF_CHARM);
      add_follower(guard, ch);
      guard->leader = ch;
   }
   act("Two giants answer the call and await orders from $n.", ch, NULL, NULL, TO_ROOM);
   return;
}

void spell_iceball(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   CHAR_DATA* vch;
   CHAR_DATA* vch_next;
   char buf[MAX_STRING_LENGTH];
   int dam;

   if (check_peace(ch))
   {
      return;
   }

   act("$n creates a freezing ball of ice!", ch, NULL, NULL, TO_ROOM);
   send_to_char("You draw heat from the room to create a ball of ice!\n\r", ch);

   for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
   {
      vch_next = vch->next_in_room;
      if
      (
         is_same_group(vch, ch) ||
         (
            IS_IMMORTAL(vch) &&
            !can_see(ch, vch) &&
            wizi_to(vch, ch)
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
         )
      )
      {
         switch (number_range(0, 2))
         {
            default:  /* 0, 1 */
            {
               sprintf
               (
                  buf,
                  "Die, %s, you sorcerous dog!",
                  PERS(ch, vch)
               );
               break;
            }
            case (2):
            {
               sprintf
               (
                  buf,
                  "Help! %s is casting a spell on me!",
                  PERS(ch, vch)
               );
            }
         }
         if (vch!= ch)
         {
            do_myell(vch, buf);
            sprintf
            (
               log_buf,
               "[%s] cast iceball upon [%s] at %d",
               ch->name,
               vch->name,
               ch->in_room->vnum
            );
            log_string(log_buf);
         }
      }
      dam = dice(level, 6);
      dam *= race_adjust(ch) / 25.0;
      damage
      (
         ch,
         vch,
         (saves_spell(ch, level, vch, DAM_COLD, SAVE_SPELL) ? dam / 2 : dam),
         sn,
         DAM_COLD,
         TRUE
      );
   }
   return;
}

void spell_cone_of_cold(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   CHAR_DATA* vch;
   CHAR_DATA* vch_next;
   char buf[MAX_STRING_LENGTH];
   int dam;
   int tmp_dam;
   static const sh_int dam_each[] =
   {
      0,
      0,   0,   0,   0,   0,    0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,    0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,    0,   0,   0,   0,   0,
      0,   90,  92,  94,  96,   98,  100, 102, 105, 130,
      132, 134, 136, 138, 150,  152, 154, 156, 158, 170,
      172, 173, 174, 175, 176,  177, 178, 179, 180, 181
   };

   if (check_peace(ch))
   {
      return;
   }

   act
   (
      "$n creates a freezing blast of air!",
      ch,
      NULL,
      NULL,
      TO_ROOM
   );
   send_to_char
   (
      "You draw heat from the room to create a blast of freezing air!\n\r",
      ch
   );

   level = UMAX(0, level) * race_adjust(ch) / 25.0;
   level = UMIN(level, (int)(sizeof(dam_each) / sizeof(dam_each[0]) - 1));
   level = UMAX(0, level);
   dam = number_range(dam_each[level] / 2, dam_each[level] * 2);
   if (dam > 362)
   {
      dam = 362;
   }
   for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
   {
      vch_next = vch->next_in_room;
      if
      (
         is_same_group(vch, ch) ||
         (
            IS_IMMORTAL(vch) &&
            !can_see(ch, vch) &&
            wizi_to(vch, ch)
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
         switch(number_range(0, 2))
         {
            default:  /* 0, 1 */
            {
               sprintf
               (
                  buf,
                  "Die, %s, you sorcerous dog!",
                  PERS(ch, vch)
               );
               break;
            }
            case (2):
            {
               sprintf
               (
                  buf,
                  "Help! %s is casting a spell on me!",
                  PERS(ch, vch)
               );
               break;
            }
         }
         do_myell(vch, buf);
         sprintf
         (
            log_buf,
            "[%s] cast cone of cold upon [%s] at %d",
            ch->name,
            vch->name,
            ch->in_room->vnum
         );
         log_string(log_buf);
      }

      if (saves_spell(ch, level, vch, DAM_COLD, SAVE_SPELL))
      {
         tmp_dam = dam/2;
      }
      else
      {
         tmp_dam = dam;
      }
      damage(ch, vch, tmp_dam, sn, DAM_COLD, TRUE);
   }
   return;
}

void spell_protective_shield(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   AFFECT_DATA af;
   CHAR_DATA* vch = (CHAR_DATA*) vo;

   if (is_affected(vch, sn))
   {
      if (ch == vch)
      {
         send_to_char("You are already protected by a protective shield.\n\r", ch);
      }
      else
      {
         act
         (
            "$N is already protected by a protective shield.",
            ch,
            NULL,
            vch,
            TO_CHAR
         );
      }
      return;
   }
   af.where = TO_AFFECTS;
   af.location = 0;
   af.modifier = 0;
   af.duration = (3 + level / 9) * race_adjust(ch) / 25.0;
   af.bitvector = 0;
   af.type = sn;
   af.level = level;
   affect_to_char(vch, &af);
   act
   (
      "$n is surrounded by a protective shield.",
      vch,
      NULL,
      NULL,
      TO_ROOM
   );
   send_to_char("You are surrounded by a protective shield.\n\r", vch);
   return;
}

void spell_timestop(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;
   AFFECT_DATA af;
   CHAR_DATA *check = NULL;

   if (check_peace(ch)) return;

   if (is_affected(ch, gsn_timestop_done))
   {
      send_to_char("The Gods prevent you from rupturing the fabric of time in such periodic bursts.\n\r", ch);
      return;
   }
   af.where = TO_AFFECTS;
   af.location = 0;
   af.modifier = 0;
   af.duration = 24;
   af.bitvector = 0;
   af.level = level;
   af.type = gsn_timestop_done;
   affect_to_char(ch, &af);
   af.duration = level/8;
   af.type = gsn_timestop;
   send_to_char("You draw upon the essence of time, causing a temporal break!\n\r", ch);
   act("The room shifts and warps as $n ruptures the flow of time within it!\n\r", ch, NULL, NULL, TO_ROOM);
   send_to_char("You feel a strange sensation as time flows in all directions around you.\n\r", ch);

   for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
   {

      vch_next = vch->next_in_room;
      if (vch == ch)
      continue;
      if
      (
         is_safe
         (
            ch,
            vch,
            IS_SAFE_SILENT | IS_SAFE_AREA_ATTACK | IS_SAFE_IGNORE_AFFECTS
         )
      )
      continue;
      if (IS_IMMORTAL(vch) && !can_see(ch, vch) && wizi_to(vch, ch)) continue;

      if (is_affected(vch, gsn_timestop) )
      continue;
      if (oblivion_blink(ch, vch))
      {
         continue;
      }
      if ( saves_spell(ch, level, vch, DAM_OTHER, SAVE_MALEDICT)  )
      {
         send_to_char("You feel a strange sensation as time flows in all directions around you.\n\r", vch);
      }
      else
      {
         send_to_char("You feel a strange sensation as everything around you freezes in time.\n\r", vch);
         act("$n appears to suddenly stop and freeze in time!", vch, NULL, NULL, TO_ROOM);
         send_to_char("You feel yourself suddenly slow down then everything stops.\n\r", vch);
         affect_to_char(vch, &af);
         for (check = vch->in_room->people; check != NULL; check = check->next_in_room)
         if (check->fighting == vch)
         {
            check->fighting = NULL;
            check->position = POS_STANDING;
         }
         vch->fighting = NULL;

      }
   }
   return;
}


void spell_consecrate(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   ROOM_INDEX_DATA* in_room;
   ROOM_INDEX_DATA* room_check;
   ROOM_AFFECT_DATA raf;
   int count;
   int door;
   EXIT_DATA* pexit;
   AFFECT_DATA af;
   int chance;

   if (is_affected(ch, gsn_consecrate))
   {
      send_to_char("You do not feel up to purging a room yet.\n\r", ch);
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

   if (IS_SET(ch->in_room->room_flags, ROOM_CONSECRATED))
   {
      send_to_char("This room is already consecrated.\n\r", ch);
      return;
   }
   if
   (
      !IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL) &&
      !IS_SET(ch->in_room->extra_room_flags, ROOM_DEAD_ZONE)
   )
   {
      send_to_char("This room is not in need of holy blessing.\n\r", ch);
      return;
   }
   if
   (
      !IS_TRUSTED(ch, ANGEL) &&
      IS_SET(ch->in_room->room_flags, ROOM_NO_CONSECRATE)
   )
   {
      send_to_char("The curse upon this room is beyond your power.\n\r", ch);
      return;
   }

   chance = ch->level + ((IS_NPC(ch) ? 100 : ch->pcdata->learned[sn]) / 2);

   in_room = ch->in_room;
   count = 0;
   for ( door =0; door < 6; door ++)
   {
      if ( (pexit = in_room->exit[door]) == NULL
      || (room_check = pexit->u1.to_room ) == NULL)
      continue;
      if (room_check == in_room)
      continue;
      if
      (
         (
            IS_SET(room_check->room_flags, ROOM_NO_RECALL) ||
            IS_SET(room_check->extra_room_flags, ROOM_DEAD_ZONE)
         ) &&
         !IS_SET(room_check->room_flags, ROOM_CONSECRATED)
      )
      {
         count++;
      }
   }
   chance -= count*10;
   act("$n gestures around the room and utters a blessing of holy rites.", ch, NULL, NULL, TO_ROOM);
   send_to_char("You chant a blessing and give the divine somatic motions of consecration.\n\r", ch);
   if (number_percent() > chance)
   {
      act("You feel the atmosphere lighten for a moment but it passes.", ch, NULL, NULL, TO_ROOM);
      send_to_char("Your holy rites are invoked but fail to affect the room's evil.\n\r", ch);
      return;
   }
   act("The atmosphere in the room lightens", ch, NULL, NULL, TO_ROOM);
   send_to_char("You succeed in consecrating the ground for a brief period!\n\r", ch);
   raf.where  = TO_ROOM;
   raf.duration = 24;
   raf.type =  sn;
   raf.level = ch->level;
   raf.bitvector = ROOM_CONSECRATED;
   raf.caster = ch;
   affect_to_room(ch->in_room, &raf);
   af.where = TO_AFFECTS;
   af.type = gsn_consecrate;
   af.location = 0;
   af.modifier =  0;
   af.bitvector = 0;
   af.level = level;
   af.duration = 10;
   affect_to_char(ch, &af);
   return;
}

void spell_retribution(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   int dam;
   int dt = DAM_OTHER;

   if (house_down(ch, HOUSE_EMPIRE))
   return;

   if (!IS_NPC(ch) && ch->pcdata->house_rank != 4 &&
   ch->pcdata->house_rank != 5 &&
   ch->pcdata->house_rank != 3)
   {
      send_to_char("You can not use this.\n\r", ch);
      return;
   }
   if (IS_NPC(victim))
   {
      act("The wrath of justice does not harm $n.", victim, NULL, NULL, TO_ROOM);
      send_to_char("The wrath of justice does not harm you.\n\r", victim);
      return;
   }

   if (!IS_SET(victim->act, PLR_CRIMINAL) )
   {
      act("The wrath of justice does not harm $n.", victim, NULL, NULL, TO_ROOM);
      send_to_char("The wrath of justice does not harm you.\n\r", victim);
      return;
   }

   dam = level * 4;
   dam = dam + number_range(-25, 25);
   damage(ch, victim, dam, sn, dt, TRUE);
   return;
}

void spell_wither(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   int dam, chance;
   AFFECT_DATA af;


   chance = (IS_NPC(ch) ? 100 : ch->pcdata->learned[sn]) +
   (level - victim->level)*3;
   dam = dice(level, 5);
   if (is_affected(victim, sn))
   chance = 0;
   chance = URANGE(5, chance*race_adjust(ch)/25.0, 90);
   if ((number_percent() > chance)
   || saves_spell(ch, level, victim, DAM_NEGATIVE, SAVE_SPELL)
   || saves_spell(ch, level, victim, DAM_NEGATIVE, SAVE_SPELL) )
   {
      send_to_char("You feel an intense pain in your body.\n\r", victim);
      act("$n jerks in sudden pain.", victim, NULL, NULL, TO_ROOM);
      if (saves_spell(ch, level, victim, DAM_HARM, SAVE_SPELL) )
      dam /= 2;
      damage(ch, victim, dam*race_adjust(ch)/25.0, sn, DAM_HARM, TRUE);
      return;
   }

   af.where = TO_AFFECTS;
   af.level = level;
   af.duration = level/4;
   af.type = sn;
   af.bitvector = 0;

   switch(number_range(0, 9))
   {
      case (0):
      case (1):
      case (2):
      case (3): /* arms */
      send_to_char("You feel a sudden intense pain as your arms wither!\n\r", victim);
      act("$n screams in agony as $s arms seem to shrivel up!", victim, NULL, NULL, TO_ROOM);
      af.location = APPLY_STR;
      af.modifier = -(level/5);
      affect_join(victim, &af, 10);
      af.location = APPLY_HITROLL;
      af.modifier = -8;
      affect_join(victim, &af, 10);
      af.location = APPLY_DAMROLL;
      af.modifier = -10;
      affect_join(victim, &af, 10);
      break;
      case (4):
      case (5):
      case (6):
      case (7): /* legs */
      send_to_char("You feel a sudden intense pain as your legs wither!\n\r", victim);
      act("$n screams in agony as $s legs crumple beneath $m!", victim, NULL, NULL, TO_ROOM);
      af.location = APPLY_STR;
      af.modifier = -(level/10);
      affect_join(victim, &af, 10);
      af.location = APPLY_DEX;
      af.modifier = -(level/5);
      affect_join(victim, &af, 10);
      af.location = APPLY_HITROLL;
      af.modifier = -5;
      affect_join(victim, &af, 10);
      af.location = APPLY_DAMROLL;
      af.modifier = -5;
      affect_join(victim, &af, 10);
      af.location = APPLY_MOVE;
      af.modifier = -3*level;
      affect_join(victim, &af, 300);
      break;
      case(8): /* body */
      act("$n's body suddenly seems to crumple up and wither!", victim, NULL, NULL, TO_ROOM);
      send_to_char("You feel a sudden intense pain as your body gives out and withers up!\n\r", victim);
      af.location = APPLY_STR;
      af.modifier = -8;
      affect_join(victim, &af, 10);
      af.location = APPLY_DEX;
      af.modifier = -5;
      affect_join(victim, &af, 10);
      af.location = APPLY_HITROLL;
      af.modifier = -6;
      affect_join(victim, &af, 10);
      af.location = APPLY_DAMROLL;
      af.modifier = -10;
      affect_join(victim, &af, 10);
      dam *= 2;
      break;
      case (9): /* head */
      send_to_char("Your head ruptures and then shrivels as it undergoes a sudden withering!\n\r", victim);
      act("$n's skull seems to just wither and shrivel up!", victim, NULL, NULL, TO_ROOM);
      dam *= 4;
      af.location = APPLY_STR;
      af.modifier = -8;
      affect_join(victim, &af, 10);
      af.location = APPLY_DEX;
      af.modifier = -5;
      affect_join(victim, &af, 10);
      af.location = APPLY_HITROLL;
      af.modifier = -6;
      affect_join(victim, &af, 10);
      af.location = APPLY_DAMROLL;
      af.modifier = -10;
      af.bitvector = AFF_BLIND;
      affect_join(victim, &af, 10);
      send_to_char("Your eyes are desicated...you are blinded!\n\r", victim);
      break;
   }

   damage(ch, victim, dam*race_adjust(ch)/25.0, sn, DAM_HARM, TRUE);
   return;
}


void spell_shadowstrike(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;

   if (is_affected(victim, gsn_shadowstrike)) {
      act("$E is already being held by shadowy arms.", ch, NULL, victim, TO_CHAR);
      /*    send_to_char("They are already held by shadowy arms.\n\r", ch); */
      return;
   }

   if (saves_spell(ch, level, victim, DAM_OTHER, SAVE_MALEDICT)) {
      act("A pair of misty arms fade into existence around $n, but fail to grasp on.", victim, NULL, NULL, TO_ROOM);
      act("A pair of misty arms fade into existence around you but cannot make a grasp.", victim, NULL, NULL, TO_CHAR);
      return;
   }
   af.where = TO_AFFECTS;
   af.modifier = -2;
   af.level = level;
   af.type = gsn_shadowstrike;
   af.duration = (level/5)*race_adjust(ch)/25.0;
   af.bitvector = 0;
   af.location = APPLY_DEX;
   affect_to_char(victim, &af);
   act("A pair of intangible arms form beside $n and embrace $m.", victim, NULL, NULL, TO_ROOM);
   act("A pair of intangible arms form beside you and embrace you!", victim, NULL, NULL, TO_CHAR);
   send_to_char("You feel yourself being grasped in a deadly touch.\n\r", victim);
   return;
}

void spell_insight(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   CHAR_DATA* victim;
   CHAR_DATA* vch;
   ROOM_INDEX_DATA* pRoomIndex;
   ROOM_INDEX_DATA* target_room;
   int mod = 15;
   AFFECT_DATA af;
   bool charmy;
   bool found;

   if (house_down(ch, HOUSE_ANCIENT))
   {
      return;
   }

   if (is_affected(ch, sn))
   {
      send_to_char("You need time to regain your insight....\n\r", ch);
      return;
   }

   if (target_name[0] == '\0')
   {
      send_to_char("You have no clue where that person may be.\n\r", ch);
      return;
   }
   victim = get_char_world(ch, target_name);
   if
   (
      victim == NULL ||
      IS_IMMORTAL(victim)
   )
   {
      send_to_char("You have no clue where that person may be.\n\r", ch);
      return;
   }
   if (victim == ch)
   {
      send_to_char("You already know your own location.\n\r", ch);
      return;
   }
   /* If its a decoy/shadow, make sure first of being marked */
   if
   (
      IS_NPC(victim) &&
      (
         victim->pIndexData->vnum == MOB_VNUM_DECOY ||
         victim->pIndexData->vnum == MOB_VNUM_MIRROR ||
         victim->pIndexData->vnum == MOB_VNUM_SHADOW
      ) &&
      victim->mprog_target != NULL &&  /* mprog_target means decoy/shadow OF */
      !IS_NPC(victim->mprog_target) &&
      !IS_SET(victim->mprog_target->act2, PLR_ANCIENT_MARK) &&
      !IS_SET(victim->mprog_target->act2, PLR_ANCIENT_MARK_PERM)
   )
   {
      act
      (
         "$E is not marked for assassination.",
         ch,
         NULL,
         victim->mprog_target,
         TO_CHAR
      );
      return;
   }
   if
   (
      !IS_NPC(victim) &&
      !IS_SET(victim->act2, PLR_ANCIENT_MARK) &&
      !IS_SET(victim->act2, PLR_ANCIENT_MARK_PERM)
   )
   {
      act
      (
         "$E is not marked for assassination.",
         ch,
         NULL,
         victim,
         TO_CHAR
      );
      return;
   }
   /* Decoy/shadow at this point means failure */
   if (IS_NPC(victim))
   {
      send_to_char("You have no clue where that person may be.\n\r", ch);
      return;
   }

   if (is_affected(victim, gsn_spellbane))
   {
      mod = 5;
   }

   if (oblivion_blink(ch, victim))
   {
      return;
   }
   /* Victim must fail a very hard save to continue */
   if (saves_spell(ch, (level + mod), victim, DAM_OTHER, SAVE_TRAVEL))
   {
      send_to_char("You have no clue where that person may be.\n\r", ch);
      return;
   }
   charmy = FALSE;
   found = FALSE;
   /* Is there a neutral or evil uncharmed mob or PC in the room with the
   target who could reveal the target's location?
   */
   for (vch = victim->in_room->people; vch != NULL; vch = vch->next_in_room)
   {
      if
      (
         vch == victim ||
         IS_IMMORTAL(vch) ||
         vch->alignment > 0 ||
         (
            IS_NPC(vch) &&
            (
               vch->pIndexData->vnum == MOB_VNUM_DECOY ||
               vch->pIndexData->vnum == MOB_VNUM_MIRROR ||
               vch->pIndexData->vnum == MOB_VNUM_SHADOW
            )
         )
      )
      {
         continue;
      }
      /* Only charmies allowed are charmies of the ancient */
      if
      (
         IS_AFFECTED(vch, AFF_CHARM) &&
         vch->master != ch
      )
      {
         if
         (
            vch->master == victim &&
            saves_spell(ch, (level + mod), vch, DAM_OTHER, SAVE_TRAVEL)
         )
         /* Can't look through charmies. */
         {
            charmy = TRUE;
         }
         continue;
      }
      if ( !IS_NPC(vch) && saves_spell(ch, (level+mod), vch, DAM_OTHER, SAVE_TRAVEL) )
      {
         continue;  /* Looking through player eyes requires save throw. */
      }
      found = TRUE;
   }

   if (charmy && !found)      /* Inform victim if their mentally bound help is touched. */
   {
      send_to_char
      (
         "You feel a slight struggle of mental forces as something's attention"
         " passes by.\n\r",
         victim
      );
   }

   if (found)  /* Have to rely on most recent information */
   {
      if
      (
         IS_NPC(victim) ||
         victim->pcdata->last_seen_in == NULL
      )
      {
         send_to_char("You have no clue where that person may be.\n\r", ch);
         return;
      }
      target_room = victim->pcdata->last_seen_in;
   }
   else
   {
      target_room = victim->in_room;
   }

   if
   (
      number_percent() < MARAUDER_MISINFORM_CHANCE &&
      is_affected(victim, gsn_misinformation)
   )
   {
      if
      (
         (
            target_room = get_random_room(victim, RANDOM_NORMAL)
         ) == NULL
      )
      {
         bug("NULL room from get_random_room.", 0);
         return;
      }
   }

   af.where = TO_AFFECTS;
   af.type = sn;
   af.level = level;
   af.duration = 1;
   af.location = 0;
   af.modifier = 0;
   af.bitvector = 0;
   affect_to_char(ch, &af);

   pRoomIndex = ch->in_room;
   char_from_room(ch);
   char_to_room_1(ch, target_room, TO_ROOM_AT);
   do_observe(ch, "", LOOK_AUTO);
   char_from_room(ch);
   char_to_room_1(ch, pRoomIndex, TO_ROOM_AT);
   return;
}


void spell_defilement(int sn, int level, CHAR_DATA *ch, void *vo, int target) {
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   int dam;

   if (house_down(ch, HOUSE_ANCIENT))
   return;
   if (!is_affected(ch, gsn_cloak_form)) {
      send_to_char("You may only invoke that power while concealed.\n\r", ch);
      return;
   }

   dam = dice( level, 8 );

   if ( saves_spell( ch, level, victim, DAM_NEGATIVE, SAVE_TRAVEL ) )
   dam /=2 ;
   damage( ch, victim, dam, sn, DAM_NEGATIVE, TRUE);
   return;
}

void spell_bless_of_dark(int sn, int level, CHAR_DATA *ch, void *vo, int target) {
   CHAR_DATA *vic=(CHAR_DATA*) vo;


   if (vic->position==POS_SLEEPING||IS_AFFECTED(vic, AFF_BLIND)) {
      send_to_char("One must be able to notice the absence of light to receive the blessing.\n\r", ch);
      return;
   }
   /*    if (number_percent()<15) {
   send_to_char("Your dark blessing has been refused.\n\r", ch);
   return;
   }
   */
   magic_spell_vict
   (
      ch,
      vic,
      CAST_BITS_REUSE,
      level + 2,
      gsn_dispel_magic
   );
   return;
}

void spell_shroud_pro(int sn, int level, CHAR_DATA *ch, void *vo, int target) {
   AFFECT_DATA af;

   if (house_down(ch, HOUSE_ANCIENT))
   return;

   if (IS_AFFECTED(ch, AFF_PROTECT_GOOD) || is_affected(ch, sn)) {
      send_to_char("You are already protected by darkness.\n\r", ch);
      return;
   }

   af.where=TO_AFFECTS;
   af.type=sn;
   af.level=level*2/3;
   af.duration=level/2;
   af.location=APPLY_AC;
   af.modifier=-2*level;
   af.bitvector=AFF_PROTECT_GOOD;
   affect_to_char(ch, &af);
   send_to_char("You seek solace in the darkness.\n\r", ch);
   return;
}

void spell_familiar(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *familiar;
   AFFECT_DATA af;
   CHAR_DATA *check;

   if (check_peace(ch)) return;

   if (house_down(ch, HOUSE_CONCLAVE))
   {
      return;
   }

   if (is_affected(ch, sn))
   {
      send_to_char("You aren't up to calling another familiar yet.\n\r", ch);
      return;
   }

   for (check = char_list; check != NULL; check = check->next)
   {
      if
      (
         IS_NPC(check) &&
         check->master == ch &&
         IS_AFFECTED(check, AFF_CHARM) &&
         (
            check->pIndexData->vnum == MOB_VNUM_UNICORN ||
            check->pIndexData->vnum == MOB_VNUM_DRAGON
         ) &&
         !is_affected(check, gsn_seize)
      )
      {
         send_to_char("You already have a familiar under your command.\n\r", ch);
         return;
      }
   }
   af.where = TO_AFFECTS;
   af.level = level;
   af.location = 0;
   af.modifier = 0;
   af.duration = 24;
   af.bitvector = 0;
   af.type = sn;
   affect_to_char(ch, &af);

   if (IS_GOOD(ch))
   familiar = create_mobile(get_mob_index(MOB_VNUM_UNICORN));
   else
   familiar = create_mobile(get_mob_index(MOB_VNUM_DRAGON));

   familiar->alignment = ch->alignment;
   act("$n calls forth a familiar to aid $m!", ch, NULL, NULL, TO_ROOM);
   send_to_char("You call forth for a familiar from the astral planes!\n\r", ch);

   familiar->level = ch->level;
   familiar->damroll += level/2;
   familiar->max_hit = ch->max_hit;
   familiar->hit = familiar->max_hit;
   familiar->max_move = ch->max_move;
   familiar->move = familiar->max_move;
   char_to_room(familiar, ch->in_room);

   SET_BIT(familiar->affected_by, AFF_CHARM);
   add_follower(familiar, ch);
   familiar->leader = ch;
   return;
}


void spell_firestream(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   OBJ_DATA *obj_lose;
   OBJ_DATA *obj_next;
   int dam;
   int dice_sz;

   dice_sz = 4;
   if (ch->level < 20)
   dice_sz = 4;
   else if (ch->level < 25)
   dice_sz = 5;
   else if (ch->level < 30)
   dice_sz = 5;
   else if (ch->level < 40)
   dice_sz = 6;
   else dice_sz = 6;

   if (number_percent() < level*race_adjust(ch)/25.0)
   {
      for (obj_lose = victim->carrying; obj_lose != NULL; obj_lose = obj_next)
      {
         char *msg;
         obj_next = obj_lose->next_content;
         if (number_bits(2) != 0)
         continue;
         switch(obj_lose->item_type)
         {
            default: continue;
            case ITEM_SCROLL: msg = "$p bursts into flames and turns to ash."; break;
            case ITEM_POTION: msg = "$p bubbles and vaporises."; break;
         }
         act(msg, victim, obj_lose, NULL, TO_CHAR);
         extract_obj(obj_lose, FALSE);
      }
   }
   dam = dice(level/2, dice_sz - 1) + dice(level/2, dice_sz);
   if (check_spellcraft(ch, sn))
   {
      dam *= 3;
      dam /= 2;
   }

   if (saves_spell(ch, level, victim, DAM_FIRE, SAVE_SPELL))
   dam /= 2;
   act("$n clenches a fist and releases a stream of searing flames!", ch, NULL, NULL, TO_ROOM);
   act("You gesture at $N and release a stream of searing fire!", ch, NULL, victim, TO_CHAR);
   damage(ch, victim, dam*race_adjust(ch)/25.0, sn, DAM_FIRE, TRUE);
   return;
}

void spell_fireshield(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   AFFECT_DATA af;

   if (is_affected(ch, gsn_fireshield))
   {
      send_to_char("You are already wreathed in a flaming halo.\n\r", ch);
      return;
   }
   if (is_affected(ch, gsn_iceshield))
   {
      send_to_char("Your iceshield vaporises in a steaming explosion!\n\r", ch);
      act("$n's iceshield vaporises in a blast of steam!", ch, NULL, NULL, TO_ROOM);
      affect_strip(ch, gsn_iceshield);
      return;
   }

   af.where = TO_AFFECTS;
   af.type = gsn_fireshield;
   af.modifier = 0;
   af.level = level;
   af.duration = (3 + ch->level/10)*race_adjust(ch)/25.0;
   af.location = 0;
   af.bitvector = 0;
   affect_to_char(ch, &af);
   send_to_char("You are surrounded in a flaming halo of heat.\n\r", ch);
   act("$n is surrounded by a flaming halo of heat.", ch, NULL, NULL, TO_ROOM);
   return;
}

void spell_iceshield(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   AFFECT_DATA af;

   if (is_affected(ch, gsn_iceshield))
   {
      send_to_char("You are already surrounded by a frozen aura.\n\r", ch);
      return;
   }
   if (is_affected(ch, gsn_fireshield))
   {
      send_to_char("Your fireshield vaporises in a steaming explosion!\n\r", ch);
      act("$n's fireshield vaporises in a blast of steam!", ch, NULL, NULL, TO_ROOM);
      affect_strip(ch, gsn_fireshield);
      return;
   }
   af.where = TO_AFFECTS;
   af.type = gsn_iceshield;
   af.modifier = 0;
   af.level = level;
   af.duration = (3 + ch->level/10)*race_adjust(ch)/25.0;
   af.location = 0;
   af.bitvector = 0;
   affect_to_char(ch, &af);
   send_to_char("You are surrounded by an aura of freezing air.\n\r", ch);
   act("$n is surrounded by a freezing aura of air.", ch, NULL, NULL, TO_ROOM);
   return;
}

void spell_aura_defiance(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   AFFECT_DATA af;
   if (house_down(ch, HOUSE_OUTLAW))
   return;

   if (is_affected(ch, gsn_aura_defiance)){
      send_to_char("You are already showing your defiance.\n\r", ch);
      return;
   }
   af.where = TO_AFFECTS;
   af.type = gsn_aura_defiance;
   af.modifier = ch->level * 3;
   af.level = level;
   af.duration = 6 + ch->level/10;
   af.location = APPLY_HIT;
   af.bitvector = 0;
   affect_to_char(ch, &af);
   ch->hit += ch->level * 3;
   update_pos(ch);
   send_to_char("You begin to project your defiance.\n\r", ch);
   act("$n begins to show $s defiance.", ch, NULL, NULL, TO_ROOM);
   return;
}

void spell_shadowself(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *shadow;
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA *check;

   for (check = char_list; check != NULL; check = check->next)
   {
      if
      (
         IS_NPC(check) &&
         check->pIndexData->vnum == MOB_VNUM_SHADOW &&
         check->mprog_target == ch
      )
      {
         send_to_char("Your shadow is already separated from you.\n\r", ch);
         return;
      }
   }

   send_to_char("You sever your shadow and give it substance.\n\r", ch);
   act("The shadows around $n seem to shift and take form.", ch, NULL, NULL, TO_ROOM);
   shadow = create_mobile(get_mob_index(MOB_VNUM_SHADOW));
   shadow->alignment = -1000;
   shadow->max_hit = 1;
   shadow->hit = 1;
   strcpy(buf, get_name(ch, NULL));
   free_string(shadow->name);
   free_string(shadow->short_descr);
   free_string(shadow->long_descr);
   shadow->name = str_dup(buf);
   sprintf(buf, "%s", get_longname(ch, NULL));
   shadow->short_descr = str_dup(buf);
   sprintf(buf, "%s is here.\n\r", is_affected(ch, gsn_cloak_form) ? "A cloaked figure" : get_longname(ch, NULL));
   shadow->long_descr = str_dup(buf); /*get_longname(ch, NULL);*/
   char_to_room(shadow, ch->in_room);
   shadow->sex = ch->sex;
   shadow->master = ch;
   /* Take care of nested shadows/decoys */
   if
   (
      IS_NPC(ch) &&
      (
         ch->pIndexData->vnum == MOB_VNUM_SHADOW ||
         ch->pIndexData->vnum == MOB_VNUM_MIRROR ||
         ch->pIndexData->vnum == MOB_VNUM_DECOY
      ) &&
      ch->mprog_target
   )
   {
      shadow->mprog_target = ch->mprog_target;
   }
   else
   {
      shadow->mprog_target = ch;
   }
   return;
}

/* Werv rs */

void spell_skull_cleaver(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   OBJ_DATA *sword;
   int bonus, dice, number;
   int ch_level;
   char buf[MAX_STRING_LENGTH];
   AFFECT_DATA af;

   sword = create_object(get_obj_index(OBJ_VNUM_SKULL_CLEAVER), 0);
   ch_level = level;
   if (number_percent() < 50)
   ch_level -= (number_range(0, 3));
   else
   ch_level += (number_range(0, 3));

   if (ch_level < 32)
   {
      dice = 5; number = 6;
   }
   else if (ch_level < 34)
   {
      dice = 3; number = 11;
   }
   else if (ch_level < 36)
   {
      number = 5; dice = 8;
   }
   else if (ch_level < 38)
   {
      number = 6; dice = 7;
   }
   else if (ch_level < 40)
   {
      number = 6; dice = 7;
   }
   else if (ch_level < 42)
   {
      number = 5; dice = 9;
   }
   else if (ch_level < 44)
   {
      number = 5; dice = 9;
   }
   else if (ch_level < 46)
   {
      number = 5; dice = 9;
   }
   else if (ch_level < 47)
   {
      number = 13; dice = 3;
   }
   else if (ch_level < 48)
   {
      number = 6; dice = 8;
   }
   else if (ch_level < 49)
   {
      number = 8; dice = 6;
   }
   else if (ch_level < 50)
   {
      number = 8; dice = 6;
   }
   else
   {
      number = 6; dice = 9;
   }

   bonus = ch->level/4;
   af.where = TO_OBJECT;
   af.type = sn;
   af.level = level;
   af.duration = -1;
   af.location = APPLY_HITROLL;
   af.modifier = bonus;
   af.bitvector = 0;
   affect_to_obj(sword, &af);
   af.location = APPLY_DAMROLL;
   affect_to_obj(sword, &af);
   sword->level = level;
   sword->timer = (20 + number_range(0, 10))*race_adjust(ch)/25.0;
   sword->value[1] = number;
   sword->value[2] = dice;
   sprintf(buf, "%s", ch->name);
   free_string(sword->owner);
   sword->owner = str_dup(buf);
   SET_BIT(sword->value[4], WEAPON_SHARP);

   obj_to_char(sword, ch);
   act("$n calls upon the darkness and creates $p.", ch, sword, NULL, TO_ROOM);
   act("You call upon the darkness and create $p.", ch, sword, NULL, TO_CHAR);
   return;
}

void do_hunters_knife(CHAR_DATA *ch, char * argument)
{
   OBJ_DATA *staff;
   char buf[MAX_STRING_LENGTH];
   AFFECT_DATA af;
   int sn;

   sn = gsn_hunters_knife;

   if (get_skill(ch, sn) <= 0 ||
   !has_skill(ch, sn))
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }
   WAIT_STATE(ch, 24);
   if (get_skill(ch, sn) <= number_percent())
   {
      send_to_char("You fail to find your hunter's knife.\n\r", ch);
      check_improve(ch, sn, FALSE, 1);
      return;
   }
   staff = create_object(get_obj_index(OBJ_VNUM_RANGER_STAFF), 0);
   free_string(staff->name);
   staff->name = str_dup("hunters knife");
   free_string(staff->description);
   staff->description = str_dup("A small steel knife lies here gleaming.");
   free_string(staff->short_descr);
   staff->short_descr = str_dup("A small steel hunter's knife");
   staff->extra_flags = 0;
   staff->weight = 10;
   staff->value[0] = WEAPON_DAGGER;
   staff->value[3] = 11;
   staff->value[4] = 0;
   SET_BIT(staff->value[4], WEAPON_SHARP);
   SET_BIT(staff->value[4], WEAPON_PIERCING);
   free_string(staff->material);
   staff->material = str_dup("steel");
   check_improve(ch, sn, TRUE, 1);

   af.where = TO_OBJECT;
   af.type = sn;
   af.level = ch->level;
   af.duration = -1;
   af.bitvector = 0;
   staff->level = ch->level;
   staff->value[1] = 6;
   staff->value[2] = 7;
   af.location = APPLY_HITROLL;
   af.modifier = ch->level/6;
   affect_to_obj(staff, &af);
   af.location = APPLY_DAMROLL;
   af.modifier = ch->level/6;
   affect_to_obj(staff, &af);
   staff->timer = 90;
   sprintf(buf, "%s", ch->name);
   free_string(staff->owner);
   staff->owner = str_dup(buf);
   obj_to_char(staff, ch);
   act("$n pulls $p from $s belt!", ch, staff, NULL, TO_ROOM);
   act("You pull $p from your belt.", ch, staff, NULL, TO_CHAR);
   return;
}

void do_carving(CHAR_DATA *ch, char * argument)
{
   OBJ_DATA *staff;
   OBJ_DATA *wood;
   char buf[MAX_STRING_LENGTH];
   AFFECT_DATA af;

   if (get_skill(ch, gsn_carving) <= 0 ||
   !has_skill(ch, gsn_carving))
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }
   if (argument[0] == '\0')
   {
      send_to_char("Carve what into a staff?\n\r", ch);
      return;
   }
   if ( ( wood = get_obj_carry( ch, argument, ch ) ) == NULL )
   {
      send_to_char("You don't have that to carve into a staff.\n\r", ch);
      return;
   }
   if (wood->pIndexData->vnum != OBJ_VNUM_BERRY || wood->level != 1)
   {
      send_to_char("That isn't something that can be used for carving.\n\r", ch);
      return;
   }
   WAIT_STATE(ch, 24);
   if (get_skill(ch, gsn_carving) <= number_percent())
   {
      send_to_char("You make a mistake ruining the wood.\n\r", ch);
      extract_obj(wood, FALSE);
      check_improve(ch, gsn_carving, FALSE, 1);
      return;
   }
   staff = create_object(get_obj_index(OBJ_VNUM_RANGER_STAFF), 0);
   staff->extra_flags = wood->extra_flags;
   staff->weight = wood->weight-10;
   free_string(staff->material);
   staff->material = str_dup(wood->material);
   check_improve(ch, gsn_carving, TRUE, 1);
   sprintf(buf, "%s %s", staff->name, staff->material);
   free_string(staff->name);
   staff->name = str_dup(buf);
   af.where = TO_OBJECT;
   af.type = gsn_carving;
   af.level = ch->level;
   af.duration = -1;
   af.bitvector = 0;
   staff->level = ch->level;
   staff->value[1] = 6;
   staff->value[2] = 4;
   if (!str_cmp(wood->material, "magic root wood"))
   {
      free_string(staff->short_descr);
      staff->short_descr = str_dup("a magic root wood staff");
      staff->value[1] = 6;
      staff->value[2] = 9;
      af.location = APPLY_HITROLL;
      af.modifier = ch->level/4;
      affect_to_obj(staff, &af);
      af.location = APPLY_DAMROLL;
      af.modifier = ch->level/4;
      affect_to_obj(staff, &af);
      staff->timer = 150;
   }

   if (!str_cmp(wood->material, "redwood"))
   {
      free_string(staff->short_descr);
      staff->short_descr = str_dup("a long staff made from redwood");
      staff->value[1] = 6;
      staff->value[2] = 9;
      af.location = APPLY_HITROLL;
      af.modifier = ch->level/4;
      affect_to_obj(staff, &af);
      af.location = APPLY_DAMROLL;
      af.modifier = ch->level/4+2;
      affect_to_obj(staff, &af);
      af.location = APPLY_HIT;
      af.modifier = ch->level;
      affect_to_obj(staff, &af);
      staff->timer = 200;
   }

   if (!str_cmp(wood->material, "oak"))
   {
      free_string(staff->short_descr);
      staff->short_descr = str_dup("a long staff made from oak");
      staff->value[1] = 6;
      staff->value[2] = 8;
      af.location = APPLY_HITROLL;
      af.modifier = ch->level/4;
      affect_to_obj(staff, &af);
      af.location = APPLY_DAMROLL;
      af.modifier = ch->level/4;
      affect_to_obj(staff, &af);
      staff->timer = 200;
   }

   if (!str_cmp(wood->material, "cherry"))
   {
      free_string(staff->short_descr);
      staff->short_descr = str_dup("a long staff made from cherry");
      staff->value[1] = 6;
      staff->value[2] = 7;
      af.location = APPLY_HITROLL;
      af.modifier = ch->level/4;
      affect_to_obj(staff, &af);
      af.location = APPLY_DAMROLL;
      af.modifier = ch->level/4;
      affect_to_obj(staff, &af);
      af.location = APPLY_DEX;
      af.modifier = 3;
      affect_to_obj(staff, &af);
      staff->timer = 150;
   }

   if (!str_cmp(wood->material, "maple"))
   {
      free_string(staff->short_descr);
      staff->short_descr = str_dup("a long staff made from maple");
      staff->value[1] = 6;
      staff->value[2] = 7;
      af.location = APPLY_HITROLL;
      af.modifier = ch->level/3;
      affect_to_obj(staff, &af);
      af.location = APPLY_DAMROLL;
      af.modifier = ch->level/4;
      affect_to_obj(staff, &af);
      staff->timer = 150;
   }

   if (!str_cmp(wood->material, "cypress"))
   {
      free_string(staff->short_descr);
      staff->short_descr = str_dup("a staff made from cypress");
      staff->value[1] = 6;
      staff->value[2] = 6;
      af.location = APPLY_HITROLL;
      af.modifier = 8;
      affect_to_obj(staff, &af);
      af.location = APPLY_DAMROLL;
      af.modifier = 8;
      affect_to_obj(staff, &af);
      staff->timer = 100;
   }


   if (!str_cmp(wood->material, "bamboo"))
   {
      free_string(staff->short_descr);
      staff->short_descr = str_dup("a staff made from a shaft of bamboo");
      staff->value[1] = 6;
      staff->value[2] = 5;
      af.location = APPLY_HITROLL;
      af.modifier = 6;
      affect_to_obj(staff, &af);
      af.location = APPLY_DAMROLL;
      af.modifier = 4;
      affect_to_obj(staff, &af);
      staff->timer = 100;
   }
   if (!str_cmp(wood->material, "pine"))
   {
      free_string(staff->short_descr);
      staff->short_descr = str_dup("a staff made from pine wood");
      staff->value[1] = 6;
      staff->value[2] = 4;
      af.location = APPLY_HITROLL;
      af.modifier = 4;
      affect_to_obj(staff, &af);
      af.location = APPLY_DAMROLL;
      af.modifier = 4;
      affect_to_obj(staff, &af);
      staff->timer = 24;
   }

   sprintf(buf, "%s", ch->name);
   free_string(staff->owner);
   staff->owner = str_dup(buf);
   obj_to_char(staff, ch);
   act("$n carves $p!", ch, staff, NULL, TO_ROOM);
   act("You carve $p!", ch, staff, NULL, TO_CHAR);
   extract_obj(wood, FALSE);
   return;
}


void spell_ranger_staff(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   OBJ_DATA *staff;
   int bonus, dice, number;
   int ch_level;
   char buf[MAX_STRING_LENGTH];
   AFFECT_DATA af;

   staff = create_object(get_obj_index(OBJ_VNUM_RANGER_STAFF), 0);
   ch_level = level;
   if (number_percent() < 50)
   ch_level -= (number_range(0, 3));
   else
   ch_level += (number_range(0, 2));

   if (ch_level < 32)
   {
      dice = 5; number = 6;
   }
   else if (ch_level < 34)
   {
      dice = 3; number = 11;
   }
   else if (ch_level < 36)
   {
      number = 5; dice = 8;
   }
   else if (ch_level < 38)
   {
      number = 6; dice = 7;
   }
   else if (ch_level < 40)
   {
      number = 6; dice = 7;
   }
   else if (ch_level < 42)
   {
      number = 5; dice = 9;
   }
   else if (ch_level < 44)
   {
      number = 5;     dice = 9;
   }
   else if (ch_level < 46)
   {
      number = 5;     dice = 9;
   }
   else if (ch_level < 47)
   {
      number = 13;    dice = 3;
   }
   else if (ch_level < 48)
   {
      number = 6;     dice = 8;
   }
   else if (ch_level < 49)
   {
      number = 8;     dice = 6;
   }
   else if (ch_level < 50)
   {
      number = 8;     dice = 6;
   }
   else
   {
      number = 6;     dice = 9;
   }

   bonus = ch->level/4;
   af.where = TO_OBJECT;
   af.type = sn;
   af.level = level;
   af.duration = -1;
   af.location = APPLY_HITROLL;
   af.modifier = bonus;
   af.bitvector = 0;
   affect_to_obj(staff, &af);
   af.location = APPLY_DAMROLL;
   affect_to_obj(staff, &af);
   staff->level = level;
   staff->timer = (20 + number_range(0, 10))*race_adjust(ch)/25.0;
   staff->value[1] = number;
   staff->value[2] = dice;
   sprintf(buf, "%s", ch->name);
   free_string(staff->owner);
   staff->owner = str_dup(buf);
   obj_to_char(staff, ch);
   act("$n creates $p!", ch, staff, NULL, TO_ROOM);
   act("You create $p!", ch, staff, NULL, TO_CHAR);
   return;
}


void spell_bless_weapon(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   AFFECT_DATA af;
   AFFECT_DATA* paf;
   AFFECT_DATA* paf_new;
   char buf [MAX_STRING_LENGTH];
   bool is_two_hands = FALSE;
   OBJ_DATA* obj     = (OBJ_DATA *) vo;
   OBJ_DATA* avenger = NULL;
   OBJ_DATA* brand   = NULL;
   char* format      = 0;
   char* handname    = 0;
   char* weaponname  = 0;
   char* material    = 0;
   int counter       = 0;
   int choice        = 0;
   int dice          = 0;
   int number        = 0;
   int dam_bonus     = 0;
   int hit_bonus     = 0;
   int fail          = 0;
   int result        = 0;
   int weapon_type   = 0;
   int weight        = 0;
   int old_dam       = 0;
   int old_hit       = 0;
   int old_accuracy  = 0;
   int old_attacks   = 0;
   bool branded      = FALSE;
   bool is_bow       = FALSE;
   bool is_arrow     = FALSE;
   bool thin_air     = FALSE;  /* create arrows out of thin air */


   /*
   burntmark paladins can't bless weapons
   */
   if
   (
      (
         brand = get_eq_char(ch, WEAR_BRAND)
      ) != NULL &&
      brand->pIndexData->vnum == 433
   )
   {
      send_to_char("The gods have forsaken you.\n\r", ch);
      return;
   }
   if (obj->item_type != ITEM_WEAPON && obj->item_type != ITEM_CONTAINER)
   {
      send_to_char("That is not a weapon.\n\r", ch);
      return;
   }
   if (obj->item_type == ITEM_CONTAINER)
   {
      if (!IS_SET( obj->value[1], CONT_IS_QUIVER))
      {
         send_to_char("That container is not a quiver.\n\r", ch);
         return;
      }
      if ( obj->contains )
      {
         send_to_char("You can only put one set of arrows in your quiver at a time.\n\r", ch);
         return;
      }
      thin_air = TRUE;
   }
   if
   (
      !thin_air &&
      (
         IS_WEAPON_STAT(obj, WEAPON_AVENGER) ||
         obj->pIndexData->vnum == OBJ_VNUM_AVENGER
         /* blessed weapons cannot be reblessed */
      )
   )
   {
      send_to_char("It is already empowered by holy blessing.\n\r", ch);
      return;
   }
   if
   (
      !thin_air &&
      obj->timer
   )
   {
      send_to_char("That weapon is in disrepair.\n\r", ch);
      return;
   }
   if
   (
      !thin_air &&
      obj->owner &&
      str_cmp(obj->owner, "none") &&
      str_cmp(obj->owner, ch->name)
   )
   {
      send_to_char("That does not belong to you.\n\r", ch);
      return;
   }
   if (obj->pIndexData->vnum == OBJ_VNUM_GENOCIDE_AXE)
   {
      send_to_char("You do not have the power to lift the curse on the axe.\n\r", ch);
      return;
   }
   if (thin_air)
   {
      /*
      make 100 arrows.
      We're putting them in a quiver though,
      if the quiver can't hold 100 arrows,
      make it the max that the quiver CAN hold
      */
      weight      = UMIN(obj->value[3] * 10, 100);
      weapon_type = WEAPON_ARROWS;
      is_arrow    = TRUE;
   }
   else
   {
      weight = obj->weight;  /* Only used for arrows */
      /*
      store the weapon type
      */
      weapon_type = obj->value[0];
      /*
      bows are different
      */
      is_bow   = (weapon_type == WEAPON_BOW);
      /*
      arrows are different
      */
      is_arrow = (weapon_type == WEAPON_ARROWS);
      /*
      store two handed status
      */
      is_two_hands = IS_WEAPON_STAT(obj, WEAPON_TWO_HANDS);
   }
   /*
   branded paladins get better weapons
   */
   branded = (brand != NULL);
   fail = 275;
   /*
   failure chance reduced by your skill in bless weapon
   npc's assumed 100 skill
   */
   fail -= IS_NPC(ch) ? 100 : ch->pcdata->learned[sn];
   /*
   failure chance reduced by your level *3
   */
   fail -= 3 * ch->level;
   /*
   evil weapons are hard to bless
   */
   if (IS_SET(obj->extra_flags, ITEM_EVIL))
   {
      fail += 200;
   }
   /*
   bonus for weapons that have bless or glow flag
   */
   if (IS_SET(obj->extra_flags, ITEM_BLESS))
   {
      fail -= 20;
   }
   if (IS_SET(obj->extra_flags, ITEM_GLOW))
   {
      fail -= 10;
   }
   /*
   Fail-=Average of weapon;
   stronger weapon=easier to bless
   if making things out of thin air,
   the higher the level of the quiver
   the easier to do it
   */
   if (!thin_air)
   {
      fail -= obj->level;
   }
   else
   {
      fail -= ( (obj->value[2] + 1) * obj->value[1]/2);
   }
   result = number_percent() * race_adjust(ch) / 25.0;
   /*
   only evil weapons can be destroyed.
   If creating arrows out of thin air,
   the quiver will only be destroyed if its evil
   if failed miserably, destroy weapon
   */
   if
   (
      IS_SET(obj->extra_flags, ITEM_EVIL) &&
      (
         thin_air ||
         result < (fail / 3)
      )
   )
   {
      act("$p flares blindingly... and vaporizes!", ch, obj, NULL, TO_ROOM);
      act("$p flares blindingly... and vaporizes!", ch, obj, NULL, TO_CHAR);
      extract_obj(obj, FALSE);
      return;
   }
   /*
   just failed a bit, nothing happens
   */
   if (result < fail)
   {
      send_to_char("Nothing seemed to happen.\n\r", ch);
      return;
   }
   /*
   SUCCESS
   */
   act
   (
      "$p flares blindingly, and a divine power seems to flow into it!",
      ch,
      obj,
      NULL,
      TO_CHAR
   );
   if (!thin_air)
   {
      act
      (
         "$p flares a brilliant white and appears more perfect!",
         ch,
         obj,
         NULL,
         TO_ROOM
      );
   }
   else
   {
      act
      (
         "$p flares blindingly, and a divine power seems to flow into it!",
         ch,
         obj,
         NULL,
         TO_ROOM
      );
   }
   if (!is_arrow)
   {
      if (level < 42)
      {
         number = 5;     dice = 9;
      }
      else if (level < 44)
      {
         number = 5;     dice = 9;
      }
      else if (level < 46)
      {
         number = 5;     dice = 9;
      }
      else if (level < 47)
      {
         number = 13;    dice = 3;
      }
      else if (level < 48)
      {
         number = 6;     dice = 8;
      }
      else if (level < 49)
      {
         number = 8;     dice = 6;
      }
      else if (level < 50)
      {
         number = 8;     dice = 6;
      }
      else if (level <52 )
      {
         number = 6;     dice = 9;
      }
      else
      {
         /*
         Immortals get stronger weapons
         */
         number = 2;     dice = level - 22;
      }
   }
   else
   {
      /*
      arrows are stronger than normal weapons
      */
      if (level < 42)
      {
         number = 10;     dice = 4;
      }
      else if (level < 44)
      {
         number = 10;     dice = 4;
      }
      else if (level < 46)
      {
         number = 11;     dice = 4;
      }
      else if (level < 47)
      {
         number = 12;    dice = 4;
      }
      else if (level < 48)
      {
         number = 13;     dice = 4;
      }
      else if (level < 49)
      {
         number = 14;     dice = 4;
      }
      else if (level < 50)
      {
         number = 15;     dice = 4;
      }
      else if (level <52 )
      {
         number = 16;     dice = 4;
      }
      else
      {
         /*
         Immortals get stronger weapons
         */
         number = level - 35;  dice = 4;
         /*
         starts at 17 at level 52,
         and goes up by 1 each level
         */
      }
   }
   avenger = create_object(get_obj_index(OBJ_VNUM_AVENGER), 0);
   /*
   branded stay original strength,
   unbranded lose a bit of hit/dam
   */
   if (!branded)
   {
      dam_bonus = ch->level / 9;
      hit_bonus = ch->level / 9;
   }
   else
   {
      dam_bonus = ch->level / 8;
      hit_bonus = ch->level / 8;
   }

   /* Do not weaken weapons */
   if (!thin_air)
   {
      old_hit = 0;
      old_dam = 0;
      paf_new = NULL;
      for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
      {
         switch (paf->location)
         {
            case (APPLY_HITROLL):
            {
               old_hit += paf->modifier;
               break;
            }
            case (APPLY_DAMROLL):
            {
               old_dam += paf->modifier;
               break;
            }
            case (APPLY_ACCURACY):
            {
               old_accuracy += paf->modifier;
               break;
            }
            case (APPLY_ATTACKS):
            {
               old_attacks += paf->modifier;
               break;
            }
            case (APPLY_SAVING_MALEDICT):
            case (APPLY_SAVING_TRANSPORT):
            case (APPLY_SAVING_BREATH):
            case (APPLY_SAVING_SPELL):
            case (APPLY_AC):
            case (APPLY_SAVES):
            {
               if (paf->modifier >= 0)
               {
                  /* Only negative is good here */
                  break;
               }
               if (paf_new == NULL)
               {
                  /* Make sure we have an affect */
                  paf_new = new_affect();
               }
               *paf_new = *paf;
               /* Do not copy major affects */
               paf_new->bitvector = 0;
               affect_to_obj(avenger, paf_new);
               break;
            }
            case (APPLY_STR):
            case (APPLY_DEX):
            case (APPLY_INT):
            case (APPLY_WIS):
            case (APPLY_CON):
            case (APPLY_LEVEL):
            case (APPLY_HEIGHT):
            case (APPLY_WEIGHT):
            case (APPLY_MANA):
            case (APPLY_HIT):
            case (APPLY_MOVE):
            case (APPLY_REGENERATION):
            case (APPLY_MN_REGENERATION):
            case (APPLY_SIZE):
            case (APPLY_SPELL_POWER):
            case (APPLY_SIGHT):
            case (APPLY_HOLY_POWER):
            {
               if (paf->modifier <= 0)
               {
                  /* Only positive is good here */
                  break;
               }
               if (paf_new == NULL)
               {
                  /* Make sure we have an affect */
                  paf_new = new_affect();
               }
               *paf_new = *paf;
               /* Do not copy major affects */
               paf_new->bitvector = 0;
               affect_to_obj(avenger, paf_new);
               break;
            }
            default:
            {
               break;
            }
         }
      }
      if (IS_SET(obj->pIndexData->extra_flags, ITEM_BURN_PROOF))
      {
         SET_BIT(avenger->extra_flags, ITEM_BURN_PROOF);
      }
      /* Do not lower hitroll or damroll bonusses */
      dam_bonus = UMAX(dam_bonus, old_dam);
      hit_bonus = UMAX(hit_bonus, old_hit);
      /* Do not lower average */
      if
      (
         number + number * dice <
         obj->value[1] + obj->value[1] * obj->value[2]
      )
      {
         number = obj->value[1];
         dice = obj->value[2];
      }
      /* Preserve all weapon flags */
      if
      (
         obj->pIndexData->item_type == ITEM_WEAPON &&
         obj->pIndexData->value[4]
      )
      {
         avenger->value[4] |= obj->pIndexData->value[4];
         REMOVE_BIT(avenger->value[4], WEAPON_TWO_HANDS);
      }
      if (paf_new)
      {
         free_affect(paf_new);
      }
   }

   avenger->value[0] = weapon_type;
   avenger->value[1] = number;
   avenger->value[2] = dice;
   avenger->weight = weight;  /* Keeps weight of original weapon */
   avenger->level = level;
   af.where = TO_OBJECT;
   af.type = sn;
   af.level = level;
   af.duration = -1;
   af.bitvector = 0;
   /*
   arrows don't get hit or dam
   */
   if (!is_arrow)
   {
      af.location = APPLY_HITROLL;
      af.modifier = hit_bonus;
      affect_to_obj(avenger, &af);
      af.location = APPLY_DAMROLL;
      af.modifier = dam_bonus;
      affect_to_obj(avenger, &af);
   }
   choice = number_range(0, 2);
   if (is_bow)
   {
      /*
      add bow affects
      */
      af.location = APPLY_ACCURACY;
      af.modifier = UMAX(95, old_accuracy);
      affect_to_obj(avenger, &af);
      af.location = APPLY_ATTACKS;
      af.modifier = UMAX(4, old_attacks);
      affect_to_obj(avenger, &af);
   }
   for (counter = 0; bless_weapon_type_names[counter].weapon_type != -1; counter++)
   {
      if (bless_weapon_type_names[counter].weapon_type == weapon_type)
      {
         format = bless_weapon_type_names[counter].format;
         if (is_two_hands)
         {
            handname = bless_weapon_type_names[counter].two_hands;
         }
         else
         {
            handname = bless_weapon_type_names[counter].one_hand;
         }
         switch(choice)
         {
            default:  /* (0) */
            {
               weaponname = bless_weapon_type_names[counter].name_one;
               break;
            }
            case (1):
            {
               weaponname = bless_weapon_type_names[counter].name_two;
               break;
            }
            case (2):
            {
               weaponname = bless_weapon_type_names[counter].name_three;
               break;
            }
         }
         material = bless_weapon_type_names[counter].material;
         if (!str_cmp(material, "steel") && !thin_air)
         {
            /*
            any weapon that defaults to steel
            can change materials
            if the original weapon was not alchemied,
            it will use that material,
            else stay steel
            (arrows made out of thin air don't have an
            original material)
            */
            if (!is_affected_obj(obj, gsn_alchemy))
            {
               material = obj->material;
               if (IS_OBJ_STAT(obj, ITEM_NONMETAL))
               {
                  SET_BIT(avenger->extra_flags, ITEM_NONMETAL);
               }
            }
         }
         else if (!str_cmp(material, "crystal") && !thin_air)
         {
            SET_BIT(avenger->extra_flags, ITEM_NONMETAL);
         }
         break;
      }
   }
   if (is_two_hands || is_bow)
   {
      /*
      Bows are always two handed
      if you bless a two handed weapon
      you end up with a two handed weapon
      */
      SET_BIT(avenger->value[4], WEAPON_TWO_HANDS);
   }

   if (is_arrow)
   {
      SET_BIT(avenger->value[4], WEAPON_SHARP);
      REMOVE_BIT(avenger->extra_flags, ITEM_NODROP);
   }
   else switch (choice)
   {
      case (0):
      af.location = APPLY_AC;
      af.modifier = -20;
      affect_to_obj(avenger, &af);
      break;

      case (1):
      af.location = APPLY_HIT;
      af.modifier = 25;
      affect_to_obj(avenger, &af);
      SET_BIT(avenger->value[4], WEAPON_LIGHTBRINGER);
      break;
      default:  /* (2) */
      SET_BIT(avenger->value[4], WEAPON_SHADOWBANE);
      break;
   }

   free_string(avenger->short_descr);
   free_string(avenger->name);
   free_string(avenger->description);
   free_string(avenger->material);

   avenger->material = str_dup(material);
   sprintf(buf, format, handname, weaponname);
   avenger->short_descr = str_dup(buf);
   sprintf(buf, "avenger %s %s", handname, weaponname);
   avenger->name = str_dup(buf);
   sprintf(buf, "A beautiful %s of divine make lies here.", handname);
   avenger->description = str_dup(buf);
   free_string(avenger->owner);
   avenger->owner = str_dup(ch->name);

   if (!thin_air)
   {
      obj_to_char(avenger, ch);
      extract_obj(obj, FALSE);
   }
   else
   {
      /*
      if made the arrows from thin air, put them inside the quiver
      */
      obj_to_obj(avenger, obj);
   }
   return;
}


void spell_chaos_mind(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   char buf[MAX_STRING_LENGTH];
   AFFECT_DATA af;
   int dur;

   if (check_peace(ch))
   return;
   if (is_safe(ch, victim, 0))
   return;

   if
   (
      is_affected(victim, gsn_chaos_mind) ||
      is_affected(victim, gsn_dancestaves)
   )
   {
      if (victim != ch)
      act("$E is confused enough as it is.", ch, NULL, victim, TO_CHAR);
      /*            send_to_char("They are confused enough as it is.\n\r", ch); */
      else
      send_to_char("You are confused enough as it is.\n\r", ch);
      return;
   }

   if (!IS_NPC(ch) && !IS_NPC(victim)
   && (ch->fighting == NULL || victim->fighting == NULL)
   && victim != ch)
   {
      switch(number_range(0, 2))
      {
         case (0):
         sprintf(buf, "Help! %s is trying to confuse me!", PERS(ch, victim));
         break;
         case (1):
         sprintf(buf, "Help! %s is trying to wreak chaos with my mind!", PERS(ch, victim));
         break;
         case (2):
         sprintf(buf, "Help! %s is trying to twist my mind with foul magics!",
         PERS(ch, victim));
      }
      do_myell(victim, buf);
      sprintf(log_buf, "[%s] cast chaos mind upon [%s] at %d", ch->name,
      victim->name, victim->in_room->vnum);
      log_string(log_buf);
   }

   if ( saves_spell(ch, level, victim, DAM_OTHER, SAVE_MALEDICT)
   || saves_armor_of_god(ch, level, victim) )
   {
      send_to_char("Your mind starts to wander off, but you are able to catch it.\n\r", victim);
      send_to_char("You failed.\n\r", ch);
      return;
   }

   dur = level - victim->perm_stat[STAT_WIS] - victim->perm_stat[STAT_INT];
   dur = dur / 3;
   if ( dur < 3 )
   dur = 3;
   af.where = TO_AFFECTS;
   af.type = gsn_chaos_mind;
   af.location = 0;
   af.bitvector = 0;
   af.duration = dur;
   af.modifier = 0;
   af.level = level;
   affect_to_char(victim, &af);
   send_to_char("You become very absentminded and confused!\n\r", victim);
   if (victim != ch)
   act("$N appears to be quite confused.", ch, NULL, victim, TO_CHAR);
   return;
}


void spell_revolt(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   CHAR_DATA *charmie;
   CHAR_DATA *charmie_next;
   char buf[MAX_STRING_LENGTH];

   if (check_peace(ch)) return;

   if (house_down(ch, HOUSE_OUTLAW))
   return;

   for (charmie = ch->in_room->people; charmie != NULL; charmie = charmie_next)
   {
      charmie_next = charmie->next_in_room;
      if (!IS_AFFECTED(charmie, AFF_CHARM) || charmie->leader != victim
      || saves_spell(ch, level, victim, DAM_OTHER, SAVE_MALEDICT) )
      continue;
      act("$n suddenly looks very angry.", charmie, NULL, NULL, TO_ROOM);
      act("You suddenly feel incited by $n's words and turn on your master!", charmie, NULL, ch, TO_CHAR);
      sprintf(buf, "I refuse to follow a tyrant like you, %s!", victim->name);
      do_gtell(charmie, buf);
      REMOVE_BIT(charmie->affected_by, AFF_CHARM);
      do_follow(charmie, "self");
      sprintf(buf, "Help! %s is revolting!", PERS(charmie, victim));
      do_myell(victim, buf);
      sprintf(log_buf, "[%s] revolted against [%s] at %d", ch->name, victim->name, ch->in_room->vnum);
      log_string(log_buf);
      charmie->fighting = victim;
      multi_hit(charmie, victim,-1);
   }

   return;
}


void spell_team_spirit(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   CHAR_DATA* victim = (CHAR_DATA*)vo;
   int amount;
   int diff;
   int dam = 60;

   if (house_down(ch, HOUSE_CONCLAVE))
   {
      return;
   }

   if (victim == ch)
   {
      send_to_char("That is not possible with yourself.\n\r", ch);
      return;
   }
   if (victim->house != HOUSE_CONCLAVE)
   {
      send_to_char("You can only team up with House Conclave members.\n\r", ch);
      return;
   }
   amount = dice(4, 5) + level / 2;

   diff = ch->level - victim->level;
   if (diff > 30)
   {
      amount *= 3;
   }
   else if (diff > 20)
   {
      amount = amount * 8 / 3;
   }
   else if (diff > 10)
   {
      amount *= 2;
   }
   else
   {
      amount = amount * 4 / 3;
   }

   amount += dice(ch->level, 2) * 3 / 2;
   if (is_mental(ch))
   {
      dam -= dam / 3;
   }
   damage(ch, ch, dam, sn, DAM_OTHER, TRUE);

   victim->mana = UMIN(victim->mana + amount, victim->max_mana);
   send_to_char("You feel energy surge through you!\n\r", victim);
   return;
}


/* misdirection for Outlaw */

void spell_misdirection(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA * victim = (CHAR_DATA *) vo;
   bool cant_misdirect = FALSE;
   AFFECT_DATA af;

   if (check_peace(ch)) return;

   if (house_down(ch, HOUSE_OUTLAW))
   return;

   if ( is_affected(victim, sn) )
   {
      act("$E is already disoriented enough as it is.", ch, NULL, victim, TO_CHAR);
      /*    send_to_char("They are already disoriented enough as it is.\n\r", ch); */
      return;
   }

   if (IS_NPC(victim))
   {
      if (victim->house != 0
      || IS_SET(victim->act, ACT_AGGRESSIVE))
      cant_misdirect = TRUE;
   }

   if (is_safe(ch, victim, 0))
   return;

   if ( saves_spell(ch, level, victim, DAM_OTHER, SAVE_MALEDICT) || (cant_misdirect)
   || saves_armor_of_god(ch, level, victim) )
   {
      act("$E resists your misdirection attempt!", ch, NULL, victim, TO_CHAR);
      /*        send_to_char("They resist your misdirection attempt!\n\r", ch); */
      act("$n tries to disorient you, but fails.", ch, NULL, victim, TO_VICT);
      return;
   }
   act("You spin $N around several times and $E becomes disoriented!", ch, NULL, victim, TO_CHAR);
   act("$n spins you around several times and disorients you!", ch, NULL, victim, TO_VICT);

   af.where=TO_AFFECTS;
   af.type=sn;
   af.level=level;
   af.duration=(ch->level)/5;
   af.location=0;
   af.modifier=0;
   af.bitvector=0;
   affect_to_char(victim, &af);
   return;
}



void spell_greater_golem(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *golem;
   AFFECT_DATA af;
   CHAR_DATA *check;
   int aflag, lev, gcount;

   aflag = 0;
   lev = level;
   gcount = 0;

   if ((!IS_NPC(ch)) && (ch->pcdata->special == SUBCLASS_ANATOMIST))
   {
      aflag = 1;
      lev = (level) + 5;
   }

   if (is_affected(ch, sn))
   {
      send_to_char("You aren't up to fashioning another golem yet.\n\r", ch);
      return;
   }

   for (check = char_list; check != NULL; check = check->next)
   {
      if
      (
         IS_NPC(check) &&
         check->master == ch &&
         check->pIndexData->vnum == MOB_VNUM_G_GOLEM &&
         !is_affected(check, gsn_seize)
      )
      {
         gcount += 1;
      }
   }

   if ((IS_NPC(ch)) && (gcount >=1))
   {
      send_to_char("You already control as many greater golems as possible.\n\r", ch);
      return;
   }

   if (((gcount >= 1) && (ch->pcdata->special != SUBCLASS_ANATOMIST))
   || ((gcount >= 2) && (ch->pcdata->special == SUBCLASS_ANATOMIST)))
   {
      send_to_char("You already control as many greater golems as possible.\n\r", ch);
      return;
   }

   af.where = TO_AFFECTS;
   af.level = level;
   af.location = 0;
   af.modifier = 0;
   af.duration = 30;
   af.bitvector = 0;
   af.type = sn;
   affect_to_char(ch, &af);
   act("$n fashions an iron golem!", ch, NULL, NULL, TO_ROOM);
   send_to_char("You fashion an iron golem to serve you!\n\r", ch);

   golem = create_mobile(get_mob_index(MOB_VNUM_G_GOLEM) );
   golem->level = lev;
   golem->damroll = (5 + (2*lev)/3);
   golem->max_hit = lev*lev;
   golem->hit = golem->max_hit;
   golem->max_move = ch->max_move;
   golem->move = golem->max_move;
   char_to_room(golem, ch->in_room);
   SET_BIT(golem->affected_by, AFF_CHARM);
   add_follower(golem, ch);
   golem->leader = ch;
   return;
}

void spell_bone_golem(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *golem;
   AFFECT_DATA af;
   CHAR_DATA *check;
   OBJ_DATA *skel;
   OBJ_DATA *skel_next;
   int bones;

   bones = 0;

   if (is_affected(ch, sn))
   {
      send_to_char("You aren't up to fashioning another golem yet.\n\r", ch);
      return;
   }

   for (check = char_list; check != NULL; check = check->next)
   {
      if
      (
         IS_NPC(check) &&
         check->master == ch &&
         check->pIndexData->vnum == MOB_VNUM_B_GOLEM &&
         !is_affected(check, gsn_seize)
      )
      {
         send_to_char("You already control a bone golem.\n\r", ch);
         return;
      }
   }

   for (skel = ch->carrying; skel != NULL; skel = skel_next)
   {
      skel_next = skel->next_content;
      if (skel->pIndexData->vnum != OBJ_VNUM_SKELETON)
      continue;
      bones += skel->level;
   }

   if (bones < 200)
   {
      send_to_char("You need more, and better bones to construct your golem.\n\r", ch);
      return;
   }

   for (skel = ch->carrying; skel != NULL; skel = skel_next)
   {
      skel_next = skel->next_content;
      if (skel->pIndexData->vnum != OBJ_VNUM_SKELETON)
      continue;
      extract_obj(skel, FALSE);
   }

   af.where = TO_AFFECTS;
   af.level = level;
   af.location = 0;
   af.modifier = 0;
   af.duration = 38;
   af.bitvector = 0;
   af.type = sn;
   affect_to_char(ch, &af);
   act("$n fashions a bone golem!", ch, NULL, NULL, TO_ROOM);
   send_to_char("You fashion a bone golem to serve you!\n\r", ch);

   golem = create_mobile(get_mob_index(MOB_VNUM_B_GOLEM) );
   golem->level = level;
   golem->damroll = level+5;
   golem->max_hit = (level+5)*(level+5);
   golem->hit = golem->max_hit;
   golem->max_move = ch->max_move;
   golem->move = golem->max_move;
   char_to_room(golem, ch->in_room);
   SET_BIT(golem->affected_by, AFF_CHARM);
   add_follower(golem, ch);
   golem->leader = ch;
   SET_BIT(golem->imm_flags, IMM_PIERCE);
   SET_BIT(golem->imm_flags, IMM_NEGATIVE);
   SET_BIT(golem->imm_flags, IMM_COLD);
   return;
}


void spell_safety(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *gch;
   CHAR_DATA *gch_next;
   ROOM_INDEX_DATA *pRoomIndex;

   if (house_down(ch, HOUSE_LIFE))
   return;

   if
   (
      is_affected(ch, gsn_purity)
   )
   {
      send_to_char("The mark of purity binds you to the earth!\n\r", ch);
      return;
   }

   if ((pRoomIndex = get_room_index(ch->temple)) == NULL)
   pRoomIndex = get_room_index(TEMPLE_GLYNDANE_G);

   if
   (
      (
         (
            IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL) ||
            IS_SET(ch->in_room->extra_room_flags, ROOM_DEAD_ZONE)
         ) &&
         !IS_SET(ch->in_room->room_flags, ROOM_CONSECRATED)
      ) ||
      (ch->in_room->house != 0 && ch->in_room->house != ch->house)
   )
   {
      send_to_char("You failed.\n\r", ch);
      return;
   }

   for (gch = ch->in_room->people; gch != NULL; gch = gch_next)
   {
      gch_next = gch->next_in_room;
      if (!is_same_group(ch, gch))
      continue;

      if (oblivion_blink(ch, gch))
      {
         continue;
      }
      if ( IS_AFFECTED(gch, AFF_CURSE)) {
         send_to_char("You failed.\n\r", gch);
         continue;
      }

      if (gch->fighting != NULL)
      stop_fighting(gch, TRUE);
      un_camouflage(gch, NULL);
      un_hide(gch, NULL);
      un_earthfade(gch, NULL);
      un_forest_blend(gch); /* - Wicket */
      if ( !IS_NPC(gch) )
      act("$n suddenly vanishes!", gch, NULL, NULL, TO_ROOM);
      char_from_room(gch);
      char_to_room(gch, pRoomIndex);
      act("$n suddenly appears in the room.", gch, NULL, NULL, TO_ROOM);
      do_observe(gch, "", LOOK_AUTO);
   }

   ch->move /=2;
   return;
}


/* some extra skills */

void spell_life_line(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;

   if (house_down(ch, HOUSE_LIGHT))
   return;

   if
   (
      IS_SET(ch->in_room->extra_room_flags, ROOM_DEAD_ZONE) &&
      !IS_SET(ch->in_room->room_flags, ROOM_CONSECRATED)
   )
   {
      send_to_char("You failed.\n\r", ch);
      return;
   }
   if
   (
      is_affected(ch, gsn_purity)
   )
   {
      send_to_char("The mark of purity causes your lifeline to fail!\n\r", ch);
      return;
   }

   if (victim == ch && ch->life_lined == NULL)
   {
      send_to_char("Create a life line to who?\n\r", ch);
      return;
   }

   if (victim == ch)
   {
      victim = ch->life_lined;
      if ( victim->in_room == NULL
      /* || !can_see_room(ch, victim->in_room)
      || IS_SET(victim->in_room->room_flags, ROOM_NO_GATE) */
      || IS_SET(victim->in_room->room_flags, ROOM_PRIVATE)
      || IS_SET(victim->in_room->room_flags, ROOM_SOLITARY) ||
      (
         IS_SET(victim->in_room->extra_room_flags, ROOM_DEAD_ZONE) &&
         !IS_SET(victim->in_room->room_flags, ROOM_CONSECRATED)
      ) ||
         ch->in_room->guild != 0
      || victim->in_room->guild != 0
      || (victim->in_room->house != 0 && victim->in_room->house != ch->house)
      || (ch->in_room->house != 0 && ch->in_room->house != ch->house)
      || is_shrine(victim->in_room) )
      {
         send_to_char( "You failed.\n\r", ch );
         return;
      }

      if (is_affected(ch, gsn_trapstun))
      {
         send_to_char
         (
            "You cannot ride your lifeline: You are in a snare.\n\r",
            ch
         );
         return;
      }
      if (ch->pet != NULL && ch->in_room == ch->pet->in_room)
      if (!is_affected(ch, sn) && number_percent() < 50)
      {
         act("You try to travel the life line to $N but fail.\n\r", ch, NULL, victim, TO_CHAR);
         return;
      }

      /* Remove unwanted affects */
      un_camouflage(ch, NULL);
      un_earthfade(ch, NULL);
      un_forest_blend(ch);
      un_hide(ch, NULL);

      act("You ride your life line to $N!", ch, NULL, victim, TO_CHAR);
      act("$n shimmers in a halo of blue light and disappears!", ch, NULL, NULL, TO_ROOM);
      char_from_room(ch);
      char_to_room(ch, victim->in_room);
      act("A blue light shimmers beside $N and $n steps forth from it!", ch, NULL, victim, TO_NOTVICT);
      act("A blue light shimmers beside you and $n steps forth from it!", ch, NULL, victim, TO_VICT);
      do_observe(ch, "", LOOK_AUTO);
      return;
   }

   if (IS_NPC(victim))
   {
      send_to_char("You can't create a life line to them.\n\r", ch);
      return;
   }

   if (EVIL_TO(ch, victim))
   {
      send_to_char( "Their evil nature resists your lifeline.\n\r", ch );
      return;
   }

   if (ch->life_lined != NULL && ch->life_lined != victim)
   {
      send_to_char("You suddenly feel a cold chill pass over you.\n\r", ch->life_lined);
      act("You break your life line to $N.", ch, NULL, ch->life_lined, TO_CHAR);
   }
   else if (ch->life_lined != NULL)
   {
      act("You renew your life line to $N.", ch, NULL, ch->life_lined, TO_CHAR);
   }
   else
   {
      act("You build a life line to $N.", ch, NULL, victim, TO_CHAR);
      send_to_char("You feel a strange sensation and a warm feeling goes through you.\n\r", victim);
   }

   ch->life_lined = victim;
   affect_strip(ch, sn);
   af.where = TO_AFFECTS;
   af.type = sn;
   af.modifier = 0;
   af.duration = ch->level;
   af.location = 0;
   af.bitvector = 0;
   af.level = level;
   affect_to_char(ch, &af);
   return;
}

void spell_decoy(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   CHAR_DATA* decoy;
   CHAR_DATA* decoy_next;
   int num;
   int max;
   int count;
   int decoys;
   int destroy;
   AFFECT_DATA af;
   CHAR_DATA* victim = (CHAR_DATA*) vo;
   char buf[MAX_INPUT_LENGTH];

   if (house_down(ch, HOUSE_OUTLAW))
   {
      return;
   }
   if (is_affected(ch, sn))
   {
      send_to_char
      (
         "The Godfather has not yet prepared more decoys for you yet.\n\r",
         ch
      );
      return;
   }
   if (IS_NPC(victim))
   {
      act
      (
         "$E doesn't need decoys to elude the law.",
         ch,
         NULL,
         victim,
         TO_CHAR
      );
      return;
   }
   decoys = 0;
   for (decoy = char_list; decoy; decoy = decoy->next)
   {
      if
      (
         IS_NPC(decoy) &&
         decoy->pIndexData->vnum == MOB_VNUM_DECOY &&
         decoy->mprog_target == victim
      )
      {
         decoys++;
      }
   }
   num = number_range(2, 5);
   max = UMIN(level / 10 * 3, 15);  /* max 15, based on level */
   if (decoys + num > max)
   {
      destroy = decoys + num - max;
      for (decoy = char_list; decoy; decoy = decoy_next)
      {
         decoy_next = decoy->next;
         if
         (
            IS_NPC(decoy) &&
            decoy->pIndexData->vnum == MOB_VNUM_DECOY &&
            decoy->mprog_target == victim
         )
         {
            raw_kill(decoy, decoy);
            if (--destroy == 0)
            {
               break;
            }
         }
      }
   }
   for (count = 0; count < num; count++)
   {
      decoy = create_mobile(get_mob_index(MOB_VNUM_DECOY));

      if (victim->morph_form[0] == MORPH_MIMIC)
      sprintf(buf, "%s is here.\n\r", victim->name);
      else if (victim->morph_form[0] != 0 || is_affected(victim, gsn_cloak_form))
      sprintf(buf, "%s", get_descr_form(victim, NULL, TRUE));
      else
      sprintf(buf, "%s is here.\n\r", victim->name);
      free_string(decoy->long_descr);
      decoy->long_descr = str_dup(buf);

      if ((victim->morph_form[0] < 6) && (victim->morph_form[0] > 0))
      sprintf(buf, "dragon %s", get_name(victim, NULL));
      else if (victim->morph_form[0] == 6)
      sprintf(buf, "goat %s", get_name(victim, NULL));
      else if (victim->morph_form[0] == 7)
      sprintf(buf, "lion %s", get_name(victim, NULL));
      else if (victim->morph_form[0] == 8)
      sprintf(buf, "dragon %s", get_name(victim, NULL));
      else if (victim->morph_form[0] == MORPH_BAT)
      sprintf(buf, "bat %s", get_name(victim, NULL));
      else if (victim->morph_form[0] == MORPH_WOLF)
      sprintf(buf, "wolf %s", get_name(victim, NULL));
      else if (victim->morph_form[0] == MORPH_BEAR)
      sprintf(buf, "bear %s", get_name(victim, NULL));
      else
      sprintf(buf, "%s", get_name(victim, NULL));

      free_string(decoy->name);
      decoy->name = str_dup(buf);
      if (victim->morph_form[0] == MORPH_MIMIC)
      sprintf(buf, "%s", victim->name);
      else if (victim->morph_form[0] != 0 || is_affected(victim, gsn_cloak_form))
      sprintf(buf, "%s", get_descr_form(victim, NULL, FALSE));
      else
      sprintf(buf, "%s", get_name(victim, NULL));
      free_string(decoy->short_descr);
      decoy->short_descr = str_dup(buf);
      decoy->max_hit = 1;
      decoy->hit = 1;
      decoy->sex = victim->sex;
      decoy->master = victim;
      /* Take care of nested shadows/decoys */
      if
      (
         IS_NPC(victim) &&
         (
            victim->pIndexData->vnum == MOB_VNUM_SHADOW ||
            victim->pIndexData->vnum == MOB_VNUM_MIRROR ||
            victim->pIndexData->vnum == MOB_VNUM_DECOY
         ) &&
         victim->mprog_target
      )
      {
         decoy->mprog_target = victim->mprog_target;
      }
      else
      {
         decoy->mprog_target = victim;
      }
      char_to_room(decoy, victim->in_room);
   }
   send_to_char("You feel a little disoriented as images of you appear all around!\n\r", victim);
   act("Suddenly there are more of $n all around!", victim, NULL, NULL, TO_ROOM);
   af.where = TO_AFFECTS;
   af.type = sn;
   af.level = level;
   af.modifier = 0;
   af.duration = 4;
   af.bitvector = 0;
   af.location = 0;
   affect_to_char(ch, &af);
   return;
}

void spell_forge(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   OBJ_DATA *plate;
   AFFECT_DATA af;
   char buf[MAX_STRING_LENGTH];
   int mod;

   if (house_down(ch, HOUSE_VALOR))
   return;

   plate = create_object(get_obj_index(2927), 0);
   plate->level = ch->level;
   plate->timer = 66;

   mod = (2 + ch->level/20);
   if (mod > 4)    mod = 4;

   if (ch->class == GUILD_MONK -1){
      free_string(plate->name);
      free_string(plate->description);
      free_string(plate->short_descr);
      plate->name = str_dup("valor robe");
      plate->description = str_dup("A Robe engraved with a Blue Cross");
      plate->short_descr = str_dup("A Robe engraved with a Blue Cross");
      plate->wear_flags = 1025; /* A (take 1) + K (about 1024) */
   }

   af.where = TO_OBJECT;
   af.type = sn;
   af.duration = -1;
   af.bitvector = 0;
   af.level = level;
   af.location = APPLY_HITROLL;
   af.modifier = mod;
   affect_to_obj(plate, &af);
   af.location = APPLY_DAMROLL;
   affect_to_obj(plate, &af);
   if (ch->level > 35)
   SET_BIT(plate->extra_flags, ITEM_HUM);
   if (ch->level > 40)
   SET_BIT(plate->extra_flags, ITEM_GLOW);
   /* used owner for now to avoid immediate need for valor only flags */
   sprintf(buf, "%s", ch->name);
   free_string(plate->owner);
   plate->owner = str_dup(buf);

   obj_to_char(plate, ch);
   act("$n creates a $p!", ch, plate, NULL, TO_ROOM);
   act("You create a $p!", ch, plate, NULL, TO_CHAR);
   return;
}

void spell_robe_of_the_light(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   OBJ_DATA *plate;
   AFFECT_DATA af;
   int mod;
   char buf[MAX_STRING_LENGTH];

   if (house_down(ch, HOUSE_LIFE))
   return;

   plate = create_object(get_obj_index(OBJ_VNUM_ROBE_LIGHT), 0);
   plate->level = ch->level;
   plate->timer = 48;


   mod = UMAX(1, (ch->level/10)-1);
   if (mod > 3)    mod = 3;

   af.where = TO_OBJECT;
   af.type = sn;
   af.duration = -1;
   af.bitvector = 0;
   af.level = level;
   af.location = APPLY_HITROLL;
   af.modifier = mod;
   affect_to_obj(plate, &af);
   af.location = APPLY_DAMROLL;
   affect_to_obj(plate, &af);
   af.location = APPLY_SAVES;
   mod = UMAX(1, ch->level/10);
   if (mod > 5)    mod = 5;
   af.modifier = -mod;
   affect_to_obj(plate, &af);
   if (ch->level > 35)
   SET_BIT(plate->extra_flags, ITEM_HUM);
   if (ch->level > 40)
   SET_BIT(plate->extra_flags, ITEM_GLOW);
   sprintf(buf, "%s", ch->name);
   free_string(plate->owner);
   plate->owner = str_dup(buf);
   obj_to_char(plate, ch);
   act("$n creates $p!", ch, plate, NULL, TO_ROOM);
   act("You create $p!", ch, plate, NULL, TO_CHAR);
   return;
}

void spell_outlaw_cloak(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   OBJ_DATA *cloak;
   AFFECT_DATA af;
   int mod;
   char buf[MAX_STRING_LENGTH];

   if (house_down(ch, HOUSE_OUTLAW))
   return;

   /*
   The spell does not need a timer anymore
   since they can only wear one cloak at a
   time - Wicket

   if (is_affected(ch, sn)){
   send_to_char("You can't create another cloak yet.\n\r", ch);
   return;
   }
   */
   cloak = create_object(get_obj_index(OBJ_VNUM_OUTLAW_CLOAK), 0);
   cloak->level = ch->level;
   cloak->timer = 48;
   mod = (4+ch->level/10);
   if (mod > 8)    mod = 8;

   af.where = TO_OBJECT;
   af.type = sn;
   af.duration = -1;
   af.bitvector = 0;
   af.level = level;
   af.location = APPLY_HITROLL;
   af.modifier = mod;
   affect_to_obj(cloak, &af);
   af.location = APPLY_DAMROLL;
   affect_to_obj(cloak, &af);

   /*
   af.where = TO_CHAR;
   af.duration  = 24;
   af.location = 0;
   af.modifier = 0;
   affect_to_char(ch, &af);
   */
   if (ch->level > 35)
   SET_BIT(cloak->extra_flags, ITEM_HUM);
   if (ch->level > 40)
   SET_BIT(cloak->extra_flags, ITEM_GLOW);
   sprintf(buf, "%s", ch->name);
   free_string(cloak->owner);
   cloak->owner = str_dup(buf);
   obj_to_char(cloak, ch);
   act("$n creates an $p!", ch, cloak, NULL, TO_ROOM);
   act("You have created an $p!", ch, cloak, NULL, TO_CHAR);
   return;
}



void spell_protectorate_plate(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   OBJ_DATA *plate;
   AFFECT_DATA af;
   char buf[MAX_STRING_LENGTH];
   int type;

   if (house_down(ch, HOUSE_EMPIRE))
   return;

   if (!IS_NPC(ch) && ch->pcdata->house_rank != 6)
   {
      send_to_char("You can not do this.\n\r", ch);
      return;
   }
   if (!IS_NPC(ch) && ch->pcdata->house_rank == 6)
   type = 2;
   else
   type = 1;
   plate = create_object(get_obj_index(OBJ_VNUM_PROTECTORATE), 0);
   plate->level = ch->level;
   plate->timer = 48;
   plate->weight = 0;

   free_string(plate->name);
   free_string(plate->description);
   free_string(plate->short_descr);
   if (type == 2){
      plate->name = str_dup("senator's senators robe");
      plate->description = str_dup("A black senator's robe is here.");
      plate->short_descr = str_dup("Black Senator's Robe");
      plate->wear_flags = 1025; /* A (take 1) + K (about 1024) */
   }
   else
   {
      plate->name = str_dup("shield honor");
      plate->description = str_dup("A shield bearing the crest of the Imperial Honor Guard is here.");
      plate->short_descr = str_dup("Crested Shield of the Honor Guard");
   }

   af.where = TO_OBJECT;
   af.type = sn;
   af.duration = -1;
   af.bitvector = 0;
   af.level = level;
   if (type == 1)
   {
      af.location = APPLY_HITROLL;
      af.modifier = 5;
      affect_to_obj(plate, &af);
      af.location = APPLY_DAMROLL;
      affect_to_obj(plate, &af);
      af.location = APPLY_HIT;
      af.modifier = level * 4;
      affect_to_obj(plate, &af);
      af.location = APPLY_AC;
      af.modifier = -40;
      affect_to_obj(plate, &af);
   }
   else
   {
      af.location = APPLY_HIT;
      af.modifier = 350;
      affect_to_obj(plate, &af);
      af.location = APPLY_SAVES;
      af.modifier = -10;
      affect_to_obj(plate, &af);
      af.location = APPLY_AC;
      af.modifier = -75;
      affect_to_obj(plate, &af);
   }
   SET_BIT(plate->extra_flags, ITEM_HUM);
   SET_BIT(plate->extra_flags, ITEM_GLOW);
   sprintf(buf, "%s", ch->name);
   free_string(plate->owner);
   plate->owner = str_dup(buf);

   obj_to_char(plate, ch);
   act("$n creates $p!", ch, plate, NULL, TO_ROOM);
   act("You create $p!", ch, plate, NULL, TO_CHAR);
   return;
}



void spell_nightwalk(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   ROOM_INDEX_DATA *pRoomIndex;
   int chance;
   bool is_imm_area;
   char buf[MAX_STRING_LENGTH];

   if (is_affected(ch, gsn_trapstun))
   {
      send_to_char("You cannot nightwalk: You are in a snare.\n\r", ch);
      return;
   }
   if
   (
      IS_SET(ch->in_room->extra_room_flags, ROOM_DEAD_ZONE) &&
      !IS_SET(ch->in_room->room_flags, ROOM_CONSECRATED)
   )
   {
      send_to_char("You cannot reach the planes of night.\n\r", ch);
      return;
   }

   if
   (
      is_affected(ch, gsn_purity)
   )
   {
      send_to_char
      (
         "The mark of purity prevents you from nightwalking!\n\r",
         ch
      );
      return;
   }

   strcpy(buf, ch->in_room->area->name);
   is_imm_area = !( str_cmp(buf, "Limbo")
   && str_cmp(buf, "Shrine of Magic")
   && str_cmp(buf, "Shrine of Dominance")
   && str_cmp(buf, "Shrine of Peace")
   && str_cmp(buf, "Shrine of Darkness and Delusion v2.1")
   && str_cmp(buf, "Shrine of Freedom")
   && str_cmp(buf, "Shrine of the Silver Dragon")
   && str_cmp(buf, "Shrine of the Void")
   && str_cmp(buf, "Thryms Wonderful World of Herbal Surprises")
   && str_cmp(buf, "The Shrine of Silence")
   && str_cmp(buf, "Special Stuff")
   && str_cmp(buf, "Theran Library Collection")
   && str_cmp(buf, "The Tower of Riallus")
   && str_cmp(buf, "Cirdan's Shrine")
   && str_cmp(buf, "Shrine of Order")
   && str_cmp(buf, "Valhalla")
   && str_cmp(buf, "Hall of Corruption") );

   pRoomIndex = get_room_index(ROOM_VNUM_NIGHTWALK);

   chance = get_skill(ch, sn);
   if ((ch->in_room->house != 0 && ch->in_room->house != ch->house)
   || (ch->in_room->guild != 0)
   || is_imm_area)
   {
      chance = -1;
   }

   if (number_percent() > chance
   || pRoomIndex == NULL)
   {
      send_to_char("You fail to enter the planes of night.\n\r", ch);
      return;
   }
   if ((ch->in_room->vnum >= 1035 && ch->in_room->vnum <= 1050))
   {
      send_to_char("You fail to enter the planes of the night.\n\r", ch);
      return;
   }

   act("$n is surrounded in shadows and disappears!.", ch, NULL, NULL, TO_ROOM);
   send_to_char("You overcome your ties to the material plane and step into the shadow world.\n\r", ch);
   char_from_room(ch);
   char_to_room(ch, pRoomIndex);
   act("$n fades into existence.", ch, NULL, NULL, TO_ROOM);
   do_observe(ch, "", LOOK_AUTO);
   return;
}

void spell_alarm(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   ROOM_INDEX_DATA *pRoomIndex;
   char buf[MAX_STRING_LENGTH];
   ALARM_DATA* alarm;
   ALARM_DATA* before;

   pRoomIndex = ch->in_room;
   if (pRoomIndex->house != 0
   && pRoomIndex->house != ch->house)
   {
      send_to_char("You can't place alarms in an enemy House.\n\r", ch);
      return;
   }
   if (IS_SET(pRoomIndex->room_flags, ROOM_NO_ALARM))
   {
      send_to_char("Your spell fizzles and dies.\n\r", ch);
      act("$n's spell fizzles and dies.", ch, NULL, NULL, TO_ROOM);
      return;
   }

   if (ch->alarm != NULL && ch->alarm->room == ch->in_room)
   {
      send_to_char("You already have an alarm placed here.\n\r", ch);
      return;
   }
   else if (ch->alarm != NULL)
   {
      sprintf(buf, "You relocate your alarm from %s and place it here.\n\r", ch->alarm->room->name);
      send_to_char(buf, ch);
      if (ch->alarm->room->alarm == ch->alarm)
      {
         ch->alarm->room->alarm = ch->alarm->room->alarm->next;
         free_alarm(ch->alarm);
      }
      else
      {
         before = ch->alarm->room->alarm;
         while
         (
            before != NULL &&
            before->next &&
            before->next != ch->alarm
         )
         {
            before = before->next;
         }
         if
         (
            before &&
            before->next == ch->alarm
         )
         {
            before->next = before->next->next;
            free_alarm(ch->alarm);
         }
      }
      ch->alarm = NULL;
   }
   else
   {
      send_to_char("You create a new alarm and place it within the room.\n\r", ch);
   }

   alarm = new_alarm();
   alarm->caster = ch;
   alarm->room = ch->in_room;
   alarm->next = ch->in_room->alarm;
   ch->in_room->alarm = alarm;
   ch->alarm = alarm;
   return;
}


void spell_blade_barrier(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   int dam, spins;
   int chance, i;

   chance = 100;

   dam = dice(level, 3);
   spins = number_range(2, 3);
   dam *= 9;
   dam /= 10;

   if (number_percent() > chance || victim == ch)
   {
      act("$n creates a whirlwind of spinning blades which turn and strike $m down!", ch, NULL, NULL, TO_ROOM);
      act("Your blade barrier turns and strikes you down!", ch, NULL, NULL, TO_CHAR);
      dam /= 2;
      damage(ch, ch, dam*race_adjust(ch)/25.0, sn, DAM_SLASH, TRUE);
      for (i = 0; i < spins; i++)
      {
         if (ch->ghost > 0)
         return;
         if (saves_spell(ch, level, ch, DAM_SLASH, SAVE_SPELL))
         dam /= 2;
         dam *= 3;
         dam /=4;
         act("The blades spin and slice away at $n.", ch, NULL, NULL, TO_ROOM);
         act("The blades spin and slice away at you.", ch, NULL, NULL, TO_CHAR);
         damage(ch, ch, dam*race_adjust(ch)/25.0, sn, DAM_SLASH, TRUE);
      }
   }
   else
   {
      act("You create a whirlwind of spinning blades to strike down $N!", ch, NULL, victim, TO_CHAR);
      act("$n creates a deadly blade barrier that tears into $N!", ch, NULL, victim, TO_NOTVICT);
      act("$n creates a deadly blade barrier that tears into you!", ch, NULL, victim, TO_VICT);
      damage(ch, victim, dam*race_adjust(ch)/25.0, sn, DAM_SLASH, TRUE);

      for (i = 0; i < spins; i++)
      {
         if (victim->ghost > 0  ||  (victim->in_room != ch->in_room))
         return;
         if (saves_spell(ch, level, victim, DAM_SLASH, SAVE_SPELL))
         dam /= 2;
         dam *= 3;
         dam /=4;
         act("The blades spin and slice away at $n.", victim, NULL, NULL, TO_ROOM);
         act("The blades spin and slice away at you.", victim, NULL, NULL, TO_CHAR);

         if (victim->ghost > 0)
         return;

         damage(ch, victim, dam*race_adjust(ch)/25.0, sn, DAM_SLASH, TRUE);
      }
   }

   act("The blade barrier fades away.", ch, NULL, NULL, TO_ROOM);
   act("Your blade barrier fades away.", ch, NULL, NULL, TO_CHAR);
   return;
}

void spell_holy_fire(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   int dam;
   int chance, dam_mod;

   chance = 100;
   dam_mod = 10;
   if (IS_GOOD(ch) && IS_GOOD(victim))
   {
      dam_mod = 5;
      chance = 0;
   }
   else if (IS_EVIL(ch))
   {
      dam_mod = 7;
      chance = 0;
   }
   else if (IS_GOOD(ch) && IS_NEUTRAL(victim))
   dam_mod = 8;
   else if (IS_NEUTRAL(ch))
   {
      dam_mod = 6;
      chance = 70;
   }

   dam = dice(level, 7);
   if (number_percent() > chance)
   {
      act("$n's heavenly fire turns on $m!", ch, NULL, NULL, TO_ROOM);
      act("Your heavenly fire turns on you for your sins!", ch, NULL, NULL, TO_CHAR);
      dam *= dam_mod;
      dam /= 10;
      if (saves_spell(ch, level, ch, DAM_HOLY, SAVE_SPELL))
      dam /= 2;
      damage(ch, ch, dam*race_adjust(ch)/25.0, sn, DAM_HOLY, TRUE);
      return;
   }

   if ( IS_SET(victim->act, ACT_UNDEAD) && number_percent() < 25)
   dam += dam/3;

   act("$n calls down fire from the heavens!", ch, NULL, NULL, TO_ROOM);
   act("You call down fire from the heavens!", ch, NULL, NULL, TO_CHAR);
   dam *= dam_mod;
   dam /= 10;
   damage(ch, victim, dam*race_adjust(ch)/25.0, sn, DAM_HOLY, TRUE);
   return;
}

void spell_getaway(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   ROOM_INDEX_DATA *pRoomIndex;
   int chance;
   char log_buf[MAX_STRING_LENGTH];

   sprintf(log_buf, "%s is casting getaway in area %s.", ch->name, ch->in_room->area->name);
   log_string( log_buf );

   if (house_down(ch, HOUSE_OUTLAW))
   return;

   chance = IS_NPC(ch) ? 105 : ch->pcdata->learned[sn];
   chance -= 5;
   if (number_percent() > chance)
   {
      send_to_char("You scan the horizon and see no sign of your griffin.\n\r", ch);
      act("$n peers intently toward the sky and frowns, disappointed.", ch, NULL, NULL, TO_ROOM);
      return;
   }

   if (IS_SET(ch->in_room->extra_room_flags, ROOM_1212))
   {
      send_to_char("You fail.\n\r", ch);
      return;
   }

   do
   if ( (pRoomIndex = get_random_room(ch, RANDOM_AREA)) == NULL )
   {
      bug("NULL room from get_random_room.", 0);
      return;
   }
   while
   (
      pRoomIndex->guild != 0
      && !can_see_room(ch, pRoomIndex)
   );

   act("A griffin swoops down and carries $n away!", ch, NULL, NULL, TO_ROOM);
   send_to_char("A griffin swoops down and carries you to safety!\n\r", ch);
   char_from_room(ch);
   char_to_room(ch, pRoomIndex);
   act("A griffin swoops down, and $n dismounts, grinning.", ch, NULL, NULL, TO_ROOM);
   do_observe(ch, "", LOOK_AUTO);
   return;
}

void spell_badge(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   OBJ_DATA *plate;
   AFFECT_DATA af;
   int mod;

   if (house_down(ch, HOUSE_OUTLAW))
   return;

   plate = create_object(get_obj_index(OBJ_VNUM_BADGE), 0);
   plate->level = ch->level;
   plate->timer = 45;

   mod = (2 + ch->level/20);
   if (mod > 4)    mod = 4;

   af.where = TO_OBJECT;
   af.type = sn;
   af.duration = -1;
   af.bitvector = 0;
   af.level = level;
   af.location = APPLY_HITROLL;
   af.modifier = mod;
   affect_to_obj(plate, &af);
   af.location = APPLY_DAMROLL;
   affect_to_obj(plate, &af);
   af.location = APPLY_HIT;
   af.modifier = URANGE(20, 2*ch->level, 75);
   affect_to_obj(plate, &af);
   af.location = APPLY_STR;
   af.modifier = 1;
   affect_to_obj(plate, &af);
   af.location = APPLY_WIS;
   affect_to_obj(plate, &af);
   if (ch->level > 35)
   SET_BIT(plate->extra_flags, ITEM_HUM);
   if (ch->level > 40)
   SET_BIT(plate->extra_flags, ITEM_GLOW);
   obj_to_char(plate, ch);
   act("$n creates $p!", ch, plate, NULL, TO_ROOM);
   act("You create $p!", ch, plate, NULL, TO_CHAR);
   return;
}



void spell_talk_to_dead(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   OBJ_DATA *corpse, *brand;
   char buf[MAX_STRING_LENGTH];

   if (((brand = get_eq_char(ch, WEAR_BRAND)) != NULL)
   && (brand->pIndexData->vnum == 433))
   {
      send_to_char("The gods have forsaken you.\n\r", ch);
      return;
   }

   if (target_name[0] == '\0')
   {
      send_to_char("Attempt to commune with which corpse?\n\r", ch);
      return;
   }
   corpse = get_obj_here(ch, target_name);
   if (corpse == NULL)
   {
      send_to_char("You can't find that here.\n\r", ch);
      return;
   }
   if ( (corpse->item_type != ITEM_CORPSE_PC) && (corpse->item_type != ITEM_CORPSE_NPC))
   {
      send_to_char("That object is not a corpse you can commune with.\n\r", ch);
      return;
   }
   if (IS_SET(corpse->extra_flags, CORPSE_NO_ANIMATE))
   {
      act("$p shudders slightly and a soft moan reaches your ears before it goes silent.", ch, corpse, NULL, TO_CHAR);
      return;
   }

   act("You draw back the spirit of the fallen and $p speaks briefly to you.", ch, corpse, NULL, TO_CHAR);

   sprintf(buf, "The spirit tells you 'I was slain by %s, now allow me my peace.'\n\r", corpse->talked);
   send_to_char(buf, ch);
   act("A faint glow surrounds $p and then fades away.", ch, corpse, NULL, TO_ROOM);
   return;
}

void spell_energy_drain(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   int type;
   int amount, amounthp;
   AFFECT_DATA af;

   if (victim == ch)
   {
      send_to_char("You can't drain your own life force.\n\r", ch);
      return;
   }

   if (!IS_NPC(ch) && ch->pcdata->special == SUBCLASS_NECROPHILE)
   level += 5;

   switch(number_range(0, 3))
   {
      default: type = 1; amount = dice(level, 3); break;
      case (0): case (1):          /* HP */
      type = 1;       amount = dice(level, 2); break;
      case (2):       /* move */
      type = 2;       amount = dice(level, 2); break;
      case (3):       /* mana */
      type = 3;       amount = dice(level, 2); break;
   }
   amount *= race_adjust(ch)/25.0;
   amounthp = dice(level, 2)*race_adjust(ch)/25.0;
   send_to_char("You feel an icy hand brush against your soul.\n\r", victim);

   if (saves_spell(ch, level, victim, DAM_NEGATIVE, SAVE_SPELL))
   {
      act("$n turns pale and shivers briefly.", victim, NULL, NULL, TO_ROOM);
      damage(ch, victim, amounthp, sn, DAM_NEGATIVE, TRUE);
      return;
   }
   act("$n gets a horrible look of pain in $s face and shudders in shock.", victim, NULL, NULL, TO_ROOM);

   af.where = TO_AFFECTS;
   af.type = sn;
   af.level = level;
   af.duration = level/2;
   af.bitvector = 0;

   switch(type)
   {
      default:
      case (1):
      act("You drain $N's vitality with vampiric magic.", ch, NULL, victim, TO_CHAR);
      send_to_char("You feel your body being mercilessly drained.\n\r", victim);
      ch->hit = URANGE(0, ch->hit + amount/3, ch->max_hit);
      af.location = APPLY_STR;
      af.modifier = -2;
      if (!is_affected(victim, sn))
      {
         affect_to_char(victim, &af);
         af.location = APPLY_CON;
         af.modifier = -3;
         affect_to_char(victim, &af);
      }
      break;
      case (2):
      send_to_char("Your energy draining invigorates you!\n\r", ch);
      victim->move = URANGE(0, victim->move - amount, victim->max_move);
      ch->move = URANGE(0, ch->move + amount/2, ch->max_move);
      send_to_char("You feel tired and weakened.\n\r", victim);
      af.location = APPLY_DEX;
      af.modifier = -2;
      if (!is_affected(victim, sn))
      {
         affect_to_char(victim, &af);
         af.location = APPLY_STR;
         af.modifier = -1;
         af.location = APPLY_MOVE;
         af.modifier = -amount/2;
         affect_to_char(victim, &af);
      }
      break;
      case (3):
      victim->mana = URANGE(0, victim->mana - amount, victim->max_mana);
      send_to_char("Your draining sends warm energy through you!\n\r", ch);

      ch->mana = URANGE(0, ch->mana + amount/3, ch->max_mana);
      send_to_char("You feel part of your mind being savagely drained.\n\r", victim);
      if (!is_affected(victim, sn))
      {
         af.location = APPLY_INT;
         af.modifier = -3;
         affect_to_char(victim, &af);
         af.location = APPLY_WIS;
         af.modifier = -2;
         affect_to_char(victim, &af);
      }
      break;
   }

   damage(ch, victim, amount + amounthp, sn, DAM_NEGATIVE, TRUE);
   return;
}

void spell_dark_shield(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;

   if (is_affected(victim, sn))
   {
      if (victim == ch)
      send_to_char("You are already surrounded by a dark shield.\n\r", ch);
      else
      act("$E is already surrounded by a dark shield.", ch, NULL, victim, TO_CHAR);
      /*                send_to_char("They are already surrounded by a dark shield.\n\r", ch); */
      return;
   }
   if (is_affected(victim, gsn_holyshield))
   {
      send_to_char("Their holyshield prevents it.\n\r", ch);
      return;
   }

   if (IS_GOOD(victim))
   {
      act("$N is too pure for such a spell.", ch, NULL, victim, TO_CHAR);
      return;
   }
   af.where = TO_AFFECTS;
   af.type = sn;
   af.level = level;
   af.duration = (IS_EVIL(victim) ? level : level/2);
   af.modifier = (IS_EVIL(victim) ? -level : -(level/2));
   af.location = APPLY_AC;
   af.bitvector = 0;
   affect_to_char(victim, &af);
   af.location = APPLY_SAVING_SPELL;
   af.modifier = (IS_EVIL(victim) ? -4: -2);
   affect_to_char(victim, &af);

   act("$n is surrounded by a darkened shield.", victim, NULL, NULL, TO_ROOM);
   send_to_char("You are surrounded by a darkened shield.\n\r", victim);

   return;
}


bool check_spellcraft(CHAR_DATA *ch, int sn)
{
   OBJ_DATA *ring;
   OBJ_DATA *symbol;
   int chance;

   chance = get_skill(ch, gsn_spellcraft);
   if
   (
      !chance &&
      is_supplicating &&
      ch->holy_power > 0
   )
   {
   }
   else if
   (
      chance == 0 ||
      !has_skill(ch, gsn_spellcraft)
   )
   {
      return FALSE;
   }


   chance /= 5;
   if (sn == gsn_windwall
   || sn == gsn_tsunami)
   chance = 0;
   if (sn == gsn_fireball
   || sn == gsn_iceball
   || sn == gsn_cone_of_cold)
   chance -= 12;
   if (sn == gsn_acid_blast
   || sn == gsn_concatenate)
   chance -= 10;
   if (sn == gsn_magic_missile
   || sn == gsn_chill_touch
   || sn == gsn_firestream
   || sn == gsn_colour_spray)
   chance += 10;
   if (sn == gsn_fire_and_ice)
   chance -= 6;
   if (sn == gsn_earthmaw)
   chance -= 11;


   if
   (
      (
         symbol = get_eq_char(ch, WEAR_HOLD)
      ) != NULL &&
      symbol->pIndexData->vnum == OBJ_VNUM_SYMBOL_MAGIC
   )
   {
      chance += 15;
   }


   ring = get_eq_char(ch, WEAR_FINGER_L);
   if (ring != NULL && ring->pIndexData->vnum == OBJ_VNUM_WIZARDRY_1)
   chance += 2;
   else if (ring != NULL && ring->pIndexData->vnum == OBJ_VNUM_WIZARDRY_2)
   chance += 5;
   ring = get_eq_char(ch, WEAR_FINGER_R);
   if (ring != NULL && ring->pIndexData->vnum == OBJ_VNUM_WIZARDRY_1)
   chance += 2;
   else if (ring != NULL && ring->pIndexData->vnum == OBJ_VNUM_WIZARDRY_2)
   chance += 5;

   chance += magic_power(ch);
   if (number_percent() > chance*race_adjust(ch)/25.0)
   {
      check_improve(ch, gsn_spellcraft, FALSE, 6);
      return FALSE;
   }
   check_improve(ch, gsn_spellcraft, TRUE, 6);
   if (IS_IMMORTAL(ch))
   send_to_char("Spellcraft boost now!\n\r", ch);

   return TRUE;
}

/*
Booster function finds the spellcraft damage..higher average damage and
much higher min. Max is same per spell though
*/
int spellcraft_dam(int num, int die)
{
   int dam;

   if
   (
      num == 0 ||
      die == 0
   )
   {
      return 0;
   }

   if (die == 1)
   {
      return num;
   }
   else if (die == 2)
   {
      return num * 2;
   }
   else if (die == 3)
   {
      return num * number_range(2, 3);
   }
   dam = num / 2 * die;
   if (num % 2)
   {
      dam += die;
   }
   dam += dice(num / 2, die);
   return dam;
}




void spell_animate_object(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   OBJ_DATA *obj = (OBJ_DATA *) vo;
   CHAR_DATA *mob;
   CHAR_DATA *check;
   int hp = 1, damroll = 1, move = 1;
   int ilevel, type, chance, ac, acm;
   char buf[MAX_STRING_LENGTH];
   int  count = 0;

   if
   (
      obj->pIndexData->vnum == OBJ_VNUM_GENOCIDE_AXE &&
      !IS_IMMORTAL(ch)
   )
   {
      send_to_char("You do not have the power to make the cursed axe submit to you.\n\r", ch);
      return;
   }
   for (check = char_list; check != NULL; check = check->next)
   {
      if
      (
         IS_NPC(check) &&
         check->master == ch &&
         (
            check->pIndexData->vnum == MOB_VNUM_ARMOR ||
            check->pIndexData->vnum == MOB_VNUM_WEAPON ||
            check->pIndexData->vnum == MOB_VNUM_KEY
         ) &&
         !is_affected(check, gsn_seize)
      )
      {
         count++;
      }
   }

   if (!IS_NPC(ch) && ch->pcdata->special == SUBCLASS_ENCHANTER)
   {
      if (count > 1)
      {
         send_to_char("You already have as many animated objects as you can.\n\r", ch);
         return;
      }
   }
   else
   {
      if (count > 0)
      {
         send_to_char("You already have an animated object following you.\n\r", ch);
         return;
      }
   }
   if (obj->wear_loc != WEAR_NONE)
   {
      send_to_char("You can only animate objects you are carrying.\n\r", ch);
      return;
   }
   ilevel = UMAX(obj->level, 1);

   if (obj->pIndexData->limtotal > 0)
   level += (25/obj->pIndexData->limtotal);

   type = obj->item_type;
   if (type != ITEM_WEAPON
   && type != ITEM_ARMOR
   && type != ITEM_KEY
   && type != ITEM_FURNITURE)
   {
      send_to_char("You can't animate that kind of object.\n\r", ch);
      return;
   }
   if (type == ITEM_ARMOR
   && (!IS_SET(obj->wear_flags, ITEM_WEAR_BODY)
   && !IS_SET(obj->wear_flags, ITEM_WEAR_HANDS)
   && !IS_SET(obj->wear_flags, ITEM_WEAR_SHIELD) ) )
   {
      send_to_char("You can only animate armor that is body, hands, or shields.\n\r", ch);
      return;
   }

   chance = IS_NPC(ch) ? 100 : ch->pcdata->learned[sn];
   chance = URANGE(2, chance + (level - ilevel)*3, 95);

   if (number_percent() > chance*race_adjust(ch)/25.0)
   {
      act("$p shudders for a moment then flares up and vaporises!", ch, obj, NULL, TO_ROOM);
      act("$p shudders for a moment then flares up and vaporises!", ch, obj, NULL, TO_CHAR);
      extract_obj(obj, FALSE);
      return;
   }

   if (type == ITEM_ARMOR || type == ITEM_FURNITURE)
   mob = create_mobile(get_mob_index(MOB_VNUM_ARMOR));
   else if (type == ITEM_WEAPON)
   mob = create_mobile(get_mob_index(MOB_VNUM_WEAPON));
   else
   mob = create_mobile(get_mob_index(MOB_VNUM_KEY));

   free_string(mob->short_descr);
   mob->short_descr = str_dup(obj->short_descr);
   free_string(mob->name);
   mob->name = str_dup(obj->name);
   sprintf(buf, "%s is here awaiting its orders.\n\r", capitalize(mob->short_descr));
   free_string(mob->long_descr);
   mob->long_descr = str_dup(buf);
   mob->level = ilevel;

   if (type == ITEM_ARMOR)
   {
      hp = (50*ilevel);
      ac = (50 - 5*ilevel);
      move = ilevel*10;
      acm = (-ilevel*2);
      damroll = ilevel/3;
      if (IS_SET(obj->wear_flags, ITEM_WEAR_BODY))
      SET_BIT(mob->off_flags, OFF_BASH);
      if (IS_SET(obj->wear_flags, ITEM_WEAR_HANDS)
      || IS_SET(obj->wear_flags, ITEM_WEAR_SHIELD))
      SET_BIT(mob->off_flags, OFF_PARRY);
   }
   else if (type == ITEM_FURNITURE)
   {
      hp = (40*ilevel);
      ac = (100 - 4*ilevel);
      move = ilevel;
      acm = -(ilevel);
      damroll = ilevel/4;
      SET_BIT(mob->off_flags, OFF_BASH);
   }
   else if (type == ITEM_KEY)
   {
      hp = ilevel;
      ac = 100;
      move = 400;
      acm = 100;
      damroll = 0;
   }
   else
   {
      hp = ilevel*25;
      ac = 100 - (4*ilevel);
      acm = 100 - (4*ilevel);
      move = ilevel*3;
      damroll = (ilevel);
   }
   mob->max_move = move;
   mob->move = move;
   mob->damroll = damroll;
   mob->max_hit = hp;
   mob->hit = hp;
   mob->alignment = ch->alignment;
   SET_BIT(mob->affected_by, AFF_CHARM);
   char_to_room(mob, ch->in_room);
   act("$p shudders and slowly rises into the air to follow $n!", ch, obj, NULL, TO_ROOM);
   act("$p shudders for a moment then slowly rises up beside you!", ch, obj, NULL, TO_CHAR);
   act("$N now follows you.", ch, NULL, mob, TO_CHAR);
   extract_obj(obj, FALSE);

   mob->leader = ch;
   mob->master = ch;
   return;
}

void spell_deathspell(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;
   int klevel, dam, bonus;
   char buf[MAX_STRING_LENGTH];
   klevel = level - 5;

   if (check_peace(ch)) return;

   act("$n utters a word of power and the negative energy explodes in the room!", ch, NULL, NULL, TO_ROOM);
   send_to_char("You utter a word of power and negative energy explodes into the room!\n\r", ch);
   bonus = 0;
   dam = dice(level, 7);
   for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
   {
      vch_next=  vch->next_in_room;
      if (is_safe(ch, vch, IS_SAFE_SILENT | IS_SAFE_AREA_ATTACK) || ch == vch)
      continue;
      if (IS_IMMORTAL(vch) && !can_see(ch, vch) && wizi_to(vch, ch)) continue;
      if (oblivion_blink(ch, vch))
      {
         continue;
      }
      if
      (
         ch->master &&
         vch == ch->master &&
         IS_AFFECTED(ch, AFF_CHARM)
      )
      {
         /*
            deathspell cast by the charmed
            does not help you get out of charm
         */
         continue;
      }
      if (!IS_NPC(ch) && !IS_NPC(vch)
      && (ch->fighting == NULL || vch->fighting == NULL))
      {
         switch(number_range(0, 2))
         {
            case (0):
            case (1):
            sprintf(buf, "Die, %s, you sorcerous dog!", PERS(ch, vch));
            break;
            case (2):
            sprintf(buf, "Help! %s is casting a spell on me!", PERS(ch, vch));
         }
         do_myell(vch, buf);
         sprintf(log_buf, "[%s] cast deathspell upon [%s] at %d", ch->name, vch->name, ch->in_room->vnum);
         log_string(log_buf);
      }

      if ( IS_SET(vch->act, ACT_UNDEAD)
      || IS_SET(vch->imm_flags, IMM_NEGATIVE)
      || (vch == ch))
      {
         act("$n is unaffected by the negative energy field.", vch, NULL, NULL, TO_ROOM);
         act("You are unaffected by the negative energy field.", vch, NULL, NULL, TO_CHAR);
      }
      else
      {

         if (vch->level > klevel || (!IS_NPC(vch)) ||
         saves_spell(ch, (klevel - bonus-3), vch, DAM_NEGATIVE, SAVE_DEATH)
         || saves_spell(ch, (klevel-3), vch, DAM_NEGATIVE, SAVE_DEATH)
         || !can_instakill(ch, vch))
         {
            if (saves_spell(ch, (level - bonus), vch, DAM_NEGATIVE, SAVE_DEATH))
            damage(ch, vch, dam/2*race_adjust(ch)/25.0, sn, DAM_NEGATIVE, TRUE);
            else
            damage(ch, vch, dam*race_adjust(ch)/25.0, sn, DAM_NEGATIVE, TRUE);
         }
         else
         {
            act("$n gets a horrible look in $s eye's then keels over dead!", vch, NULL, NULL, TO_ROOM);
            send_to_char("You feel your an intense pain in your head as the energy ruptures your skull!\n\r", vch);
            raw_kill(ch, vch);
         }
      }
   }
   return;
}

void spell_lifebane(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;
   char buf[MAX_STRING_LENGTH];

   int dam, tmp_dam;
   /*   bool inside = FALSE; */

   if (check_peace(ch)) return;


   /*        if (IS_SET(ch->in_room->room_flags, ROOM_INDOORS)
   || ch->in_room->sector_type == SECT_INSIDE)
   inside = TRUE; */

   dam = dice(level, 4);
   for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
   {
      vch_next = vch->next_in_room;
      if (IS_IMMORTAL(vch) && !can_see(ch, vch) && wizi_to(vch, ch)) continue;
      if (is_safe(ch, vch, IS_SAFE_SILENT | IS_SAFE_AREA_ATTACK))    continue;
      if ( /* (!inside || number_percent() < 50) && */ is_same_group(ch, vch)) continue;
      if (oblivion_blink(ch, vch))
      {
         continue;
      }
      tmp_dam = dam;
      if (vch == ch)
      tmp_dam /= 2;
      if (!IS_NPC(ch) && !IS_NPC(vch)
      && (ch->fighting == NULL || vch->fighting == NULL))
      {
         switch(number_range(0, 2))
         {
            case (0):
            case (1):
            sprintf(buf, "Die, %s, you sorcerous dog!", PERS(ch, vch));
            break;
            case (2):
            sprintf(buf, "Help! %s is casting a spell on me!", PERS(ch, vch));
         }
         if (vch!= ch)
         {
            do_myell(vch, buf);
            sprintf(log_buf, "[%s] cast lifebane upon [%s] at %d", ch->name, vch->name, ch->in_room->vnum);
            log_string(log_buf);
         }
      }

      if (saves_spell(ch, level, vch, DAM_NEGATIVE, SAVE_SPELL))
      damage(ch, vch, dam/2*race_adjust(ch)/25.0, sn, DAM_NEGATIVE, TRUE);
      else
      {
         magic_spell_vict
         (
            ch,
            vch,
            CAST_BITS_REUSE,
            level - 5,
            gsn_poison
         );
         magic_spell_vict
         (
            ch,
            vch,
            CAST_BITS_REUSE,
            level - 5,
            gsn_weaken
         );
         magic_spell_vict
         (
            ch,
            vch,
            CAST_BITS_REUSE,
            level - 8,
            gsn_curse
         );
         damage(ch, vch, dam*race_adjust(ch)/25.0, sn, DAM_NEGATIVE, TRUE);
      }
   }
   return;
}

void spell_curse_weapon(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   OBJ_DATA *obj = (OBJ_DATA *) vo;
   AFFECT_DATA *paf;
   AFFECT_DATA af;
   int result, fail, mod2;
   int hit_bonus, dam_bonus, added;
   bool hit_found = FALSE, dam_found = FALSE;

   if (obj->item_type != ITEM_WEAPON)
   {
      send_to_char("That isn't a weapon.\n\r", ch);
      return;
   }

   if (obj->wear_loc != -1)
   {
      send_to_char("You must be carrying the weapon to curse it.\n\r", ch);
      return;
   }

   /* this means they have no bonus */
   hit_bonus = 0;
   dam_bonus = 0;
   fail = 25;    /* base 25% chance of failure */

   /* find the bonuses */

   if (!obj->enchanted)
   for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
   {
      if ( paf->location == APPLY_HITROLL )
      {
         hit_bonus = paf->modifier;
         hit_found = TRUE;
         fail += 2 * (hit_bonus * hit_bonus);
      }

      else if (paf->location == APPLY_DAMROLL )
      {
         dam_bonus = paf->modifier;
         dam_found = TRUE;
         fail += 2 * (dam_bonus * dam_bonus);
      }

      else  /* things get a little harder */
      fail += 25;
   }

   for ( paf = obj->affected; paf != NULL; paf = paf->next )
   {
      if ( paf->location == APPLY_HITROLL )
      {
         hit_bonus = paf->modifier;
         hit_found = TRUE;
         fail += 2 * (hit_bonus * hit_bonus);
      }

      else if (paf->location == APPLY_DAMROLL )
      {
         dam_bonus = paf->modifier;
         dam_found = TRUE;
         fail += 2 * (dam_bonus * dam_bonus);
      }

      else /* things get a little harder */
      fail += 25;
   }

   /* apply other modifiers */
   fail -= 3 * level/2;

   if (IS_OBJ_STAT(obj, ITEM_BLESS))
   fail += 15;
   if (IS_OBJ_STAT(obj, ITEM_GLOW))
   fail += 10;

   fail = URANGE(5, fail, 95);

   result = number_percent()*race_adjust(ch)/25.0;

   /* the moment of truth */
   if (result < (fail / 5))  /* item destroyed */
   {
      act("$p shivers violently and explodes!", ch, obj, NULL, TO_CHAR);
      act("$p shivers violently and explodeds!", ch, obj, NULL, TO_ROOM);
      extract_obj(obj, FALSE);
      return;
   }

   if (result < (fail / 2)) /* item disenchanted */
   {
      AFFECT_DATA *paf_next;

      act("$p is surrounded in a sickly red aura, then fades.", ch, obj, NULL, TO_CHAR);
      act("$p is surrounded in a sickly red aura, then fades.", ch, obj, NULL, TO_ROOM);
      obj->enchanted = TRUE;

      /* remove all affects */
      for (paf = obj->affected; paf != NULL; paf = paf_next)
      {
         paf_next = paf->next;
         free_affect(paf);
      }
      obj->affected = NULL;

      REMOVE_BIT(obj->extra_flags, ITEM_GLOW);
      REMOVE_BIT(obj->extra_flags, ITEM_HUM);
      REMOVE_BIT(obj->extra_flags, ITEM_MAGIC);
      REMOVE_BIT(obj->extra_flags, ITEM_INVIS);
      REMOVE_BIT(obj->extra_flags, ITEM_NODROP);
      REMOVE_BIT(obj->extra_flags, ITEM_NOREMOVE);
      return;
   }

   if ( result <= fail )  /* failed, no bad result */
   {
      send_to_char("Nothing seemed to happen.\n\r", ch);
      return;
   }

   /* okay, move all the old flags into new vectors if we have to */
   if (!obj->enchanted)
   {
      AFFECT_DATA *af_new;
      obj->enchanted = TRUE;

      for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
      {
         af_new = new_affect();

         *af_new = *paf;
         af_new->next = obj->affected;
         obj->affected = af_new;
      }
   }

   af.type = sn;
   af.where = TO_OBJECT;
   af.modifier = -1;
   af.duration = -1;
   af.bitvector = 0;
   af.level = level;
   mod2 = number_range(0, 6);
   if (mod2 == 0)
   af.location = APPLY_STR;
   else if (mod2 == 1)
   af.location = APPLY_DEX;
   else if (mod2 == 2)
   af.location = APPLY_CON;
   else if (mod2 == 3)
   af.location = APPLY_INT;
   else if (mod2 == 4)
   af.location = APPLY_WIS;
   else if (mod2 == 5)
   {
      af.modifier = -(number_range(5, 20));
      af.location = APPLY_HIT;
   }
   else
   {
      af.location = APPLY_MANA;
      af.modifier = -(number_range(5, 20));
   }



   if (result <= (100 - level/5))  /* success! */
   {
      act("$p glows with a sickly green aura.", ch, obj, NULL, TO_CHAR);
      act("$p glows with a sickly green aura.", ch, obj, NULL, TO_ROOM);
      affect_to_obj(obj, &af);
      added = -1;
   }

   else  /* exceptional enchant */
   {
      act("$p burns a fierce red!", ch, obj, NULL, TO_CHAR);
      act("$p glows a brilliant blue!", ch, obj, NULL, TO_ROOM);
      affect_to_obj(obj, &af);
      SET_BIT(obj->extra_flags, ITEM_NOREMOVE);
      added = -2;
   }

   if (number_percent() <= level)
   {
      SET_BIT(obj->extra_flags, ITEM_NOREMOVE);
      send_to_char("The weapon begins to stick to your hands.\n\r", ch);
   }

   if ( (result = number_percent()) <= 25 && IS_SET(obj->extra_flags, ITEM_HUM) )
   {
      REMOVE_BIT(obj->extra_flags, ITEM_HUM);
      REMOVE_BIT(obj->extra_flags, ITEM_GLOW);
   }
   if ( (result = number_percent()) <= 25 && IS_SET(obj->extra_flags, ITEM_GLOW) )
   REMOVE_BIT(obj->extra_flags, ITEM_GLOW);

   /* now add the enchantments */

   if (obj->level < LEVEL_HERO - 1)
   obj->level = UMIN(LEVEL_HERO - 1, obj->level - 1);

   if (dam_found)
   {
      for ( paf = obj->affected; paf != NULL; paf = paf->next)
      {
         if ( paf->location == APPLY_DAMROLL)
         {
            paf->type = sn;
            paf->modifier += added;
            paf->level = UMAX(paf->level, level);
         }
      }
   }
   else /* add a new affect */
   {
      paf = new_affect();

      paf->where    = TO_OBJECT;
      paf->type    = sn;
      paf->level    = level;
      paf->duration    = -1;
      paf->location    = APPLY_DAMROLL;
      paf->modifier    =  added;
      paf->bitvector  = 0;
      paf->next    = obj->affected;
      obj->affected    = paf;
   }

   if (hit_found)
   {
      for ( paf = obj->affected; paf != NULL; paf = paf->next)
      {
         if ( paf->location == APPLY_HITROLL)
         {
            paf->type = sn;
            paf->modifier += added;
            paf->level = UMAX(paf->level, level);
         }
      }
   }
   else /* add a new affect */
   {
      paf = new_affect();

      paf->where      = TO_OBJECT;
      paf->type       = sn;
      paf->level      = level;
      paf->duration   = -1;
      paf->location   = APPLY_HITROLL;
      paf->modifier   =  added;
      paf->bitvector  = 0;
      paf->next       = obj->affected;
      obj->affected   = paf;
   }

}

void do_nomagic(CHAR_DATA *ch, char *argument)
{
   ROOM_AFFECT_DATA raf;

   if (!IS_IMMORTAL(ch))
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }

   if (is_affected_room(ch->in_room, gsn_no_magic_toggle))
   {
      send_to_char("No magic flag removed.\n\r", ch);
      affect_strip_room(ch->in_room, gsn_no_magic_toggle);
      sprintf(log_buf, "%s removed no magic toggle in [%d].", ch->name, ch->in_room->vnum);
      log_string(log_buf);
      return;
   }

   raf.where     = TO_ROOM;
   raf.duration = 24;
   raf.type = gsn_no_magic_toggle;
   raf.level = ch->level;
   raf.bitvector = ROOM_NO_MAGIC;
   raf.caster = ch;
   affect_to_room(ch->in_room, &raf);

   sprintf(log_buf, "%s made room [%d] no magic.", ch->name, ch->in_room->vnum);
   log_string(log_buf);
   send_to_char("Room is now no magic.\n\r", ch);

   return;
}

void spell_curse_room(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   int chance;
   AFFECT_DATA af;
   ROOM_AFFECT_DATA raf;

   chance = ch->level * 2;
   if (IS_SET(ch->in_room->room_flags, ROOM_DARK))
   chance += 15;
   if (IS_SET(ch->in_room->room_flags, ROOM_NO_CONSECRATE))
   chance += 15;
   if (IS_SET(ch->in_room->room_flags, ROOM_CONSECRATED))
   chance -= 30;
   if (IS_SET(ch->in_room->room_flags, ROOM_LOW_ONLY))
   chance = 0;
   if (ch->in_room->house != 0)
   if (ch->in_room->house != ch->house)
   chance = 0;
   if (is_affected(ch, sn)){
      send_to_char("You can't perform the ritual again so soon.\n\r", ch);
      return;
   }
   if
   (
      IS_SET(ch->in_room->extra_room_flags, ROOM_AP_CURSED) ||
      IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL) ||
      IS_SET(ch->in_room->extra_room_flags, ROOM_DEAD_ZONE)
   )
   {
      send_to_char("This room is already cursed.\n\r", ch);
      return;
   }

   if (number_percent() > chance)
   {
      send_to_char("You try to call upon the dark gods to curse this room, but fail.\n\r", ch);
      return;
   }

   send_to_char("The dark gods answer your age old ritual, and the room is cursed.\n\r", ch);
   act("$n performs an evil ritual cursing this place.", ch, NULL, NULL, TO_ROOM);

   /* SET_BIT(ch->in_room->extra_room_flags, ROOM_AP_CURSED);
   SET_BIT(ch->in_room->room_flags, ROOM_NO_RECALL); */
   raf.where     = TO_ROOM;
   raf.duration = level/6;
   raf.type = sn;
   raf.level = level;
   raf.bitvector = ROOM_NO_RECALL;
   raf.caster = ch;
   affect_to_room(ch->in_room, &raf);

   af.where = TO_AFFECTS;
   af.type = sn;
   af.level = level;
   af.duration = 4;
   af.location = 0;
   af.modifier = 0;
   af.bitvector = 0;
   affect_to_char(ch, &af);
   return;
}

void spell_globe_of_darkness(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   int chance;
   AFFECT_DATA af;
   ROOM_AFFECT_DATA raf;

   if (is_affected(ch, sn))
   {
      send_to_char("You are not yet able to call upon your power of the darkness.\n\r", ch);
      return;
   }

   af.where = TO_AFFECTS;
   af.type = sn;
   af.level = level;
   af.duration = 8;
   af.location = 0;
   af.modifier = 0;
   af.bitvector = 0;
   chance = get_skill(ch, sn);
   chance *= ch->level;
   chance /= 50;

   if (IS_SET(ch->in_room->room_flags, ROOM_DARK))
   chance += 15;
   if (IS_SET(ch->in_room->room_flags, ROOM_NO_CONSECRATE))
   chance += 5;
   if (IS_SET(ch->in_room->room_flags, ROOM_CONSECRATED))
   chance -= 15;
   if (IS_SET(ch->in_room->room_flags, ROOM_LAW))
   chance = 0;
   if (IS_SET(ch->in_room->room_flags, ROOM_LOW_ONLY))
   chance = 0;

   if (is_affected_room(ch->in_room, sn))
   {
      send_to_char("This room is already shrouded in a globe of darkness.\n\r", ch);
      return;
   }
   if (number_percent() > chance)
   {
      send_to_char("Night shrouds the room briefly but it disperses.\n\r", ch);
      act("Shadows and darkness briefly shroud the room but then disperse.", ch, NULL, NULL, TO_ROOM);
      affect_to_char(ch, &af);
      return;
   }

   act("$n utters a word of drow power and the room becomes as black as darkest night.", ch, NULL, NULL, TO_ROOM);
   send_to_char("You utter a word of drow power and darkness enshrouds the room!\n\r", ch);
   raf.where     = TO_ROOM;
   raf.duration = level/5;
   raf.type = sn;
   raf.level = level;
   raf.bitvector = 0;
   raf.caster = ch;
   affect_to_room(ch->in_room, &raf);
   af.duration = 24;
   affect_to_char(ch, &af);
   return;
}


/* Green tower obj progs */


/* sceptre of heavens...say 'i am the wrath of god' */
void spell_heavenly_sceptre_frenzy(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   AFFECT_DATA af;
   if (is_affected(ch, sn) || (!IS_GOOD(ch) && !IS_EVIL(ch)))
   {
      act("$n's sceptre hums softly but nothing seems to happen.", ch, NULL, NULL, TO_ROOM);
      act("Your sceptre hums softly but nothing seems to happen.", ch, NULL, NULL, TO_CHAR);
      return;
   }
   af.type = sn;
   af.where = TO_AFFECTS;
   af.duration = 10;
   af.modifier = ch->level/7;
   af.location = APPLY_DAMROLL;
   af.bitvector = 0;
   af.level = level;
   affect_to_char(ch, &af);
   af.location = APPLY_HITROLL;
   affect_to_char(ch, &af);
   if (ch->level > 40)
   af.location = APPLY_SAVING_SPELL;
   af.modifier = -ch->level/10;

   if (IS_GOOD(ch))
   {
      act("$n's sceptre glows white and a look of righteous anger in $s eyes.", ch, NULL, NULL, TO_ROOM);
      send_to_char("Your sceptre glows white and you feel a surge of righteous anger!\n\r", ch);
   }
   else if (IS_EVIL(ch))
   {
      act("$n's sceptre darkens and a look of righteous anger enters $s eyes.", ch, NULL, NULL, TO_ROOM);
      send_to_char("Your sceptre darkens and you feel a surge of righteous anger!\n\r", ch);
   }

   return;
}


/* sceptre of heavens, say 'Feel the force of god' */
void spell_heavenly_sceptre_fire(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   AFFECT_DATA af;
   OBJ_DATA *sceptre = NULL;
   int dam;
   CHAR_DATA *victim;
   /*
   sceptre = get_eq_char(ch, WEAR_HOLD);

   if (sceptre == NULL
   || sceptre->pIndexData->vnum != OBJ_VNUM_HEAVENLY_SCEPTRE)
   return;
   */

   victim = ch->fighting;
   if (is_affected(ch, sn) || victim == NULL || ch->level < 30)
   {
      act("$n's sceptre hums softly but nothing seems to happen.", ch, NULL, NULL, TO_ROOM);
      send_to_char("Your sceptre hums softly but nothing seems to happen.\n\r", ch);
      WAIT_STATE(ch, 12);
      return;
   }

   dam = dice(15, 20);

   if (saves_spell(ch, ch->level, victim, DAM_HOLY, SAVE_OTHER))
   dam /= 2;
   if (saves_spell(ch, 65, victim, DAM_OTHER, SAVE_OTHER))
   dam /= 2;

   damage(ch, victim, dam, sn, DAM_HOLY, TRUE);

   if (IS_NPC(ch))
   return;
   if (number_percent() > ch->level * 2)
   {
      act("Your sceptre of heavenly orders crumbles to dust.", ch, NULL, NULL, TO_ROOM);
      send_to_char("Your sceptre of heavenly orders crumbles to dust.\n\r", ch);
      extract_obj(sceptre, FALSE);
      return;
   }
   af.where = TO_AFFECTS;
   af.type = sn;
   af.modifier = 0;
   af.duration = 1;
   af.location = 0;
   af.bitvector = 0;
   af.level = ch->level;
   affect_to_char(ch, &af);
   return;
}

void spell_soulbind(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;
   /*    CHAR_DATA *check;*/

   if (!IS_NPC(victim))
   {
      send_to_char("You can't bind them to your soul.\n\r", ch);
      return;
   }
   if ((victim->pIndexData->vnum != MOB_VNUM_ZOMBIE
   && victim->pIndexData->vnum != MOB_VNUM_MUMMY
   && victim->pIndexData->vnum != MOB_VNUM_SKELETON)
   || victim->master != ch)
   {
      send_to_char("You can't bind them to your soul.\n\r", ch);
      return;
   }

   if (is_affected(victim, sn))
   {
      send_to_char("That zombie is already soulbound.\n\r", ch);
      return;
   }
   if (is_affected(victim, gsn_seize))
   {
      send_to_char("They cannot be soulbinded.\n\r", ch);
   }
   /*    for (check = char_list; check != NULL; check = check->next)
   {
   if (!IS_NPC(check))
   continue;
   if (check->pIndexData->vnum == MOB_VNUM_ZOMBIE
   && check->master == ch && is_affected(check, sn))
   {
   send_to_char("You already have a zombie bound to your soul.\n\r", ch);
   return;
   }
   }*/

   if (ch->pet != NULL)
   {
      if (is_affected(ch->pet, sn))
      send_to_char("You already have a soulbound minion.\n\r", ch);
      else
      send_to_char("You already have a permanent follower.\n\r", ch);
      return;
   }

   af.where = TO_AFFECTS;
   af.location = APPLY_DAMROLL;
   af.modifier = 3;
   af.duration = -1;
   af.bitvector = 0;
   af.level = level;
   af.type = sn;
   affect_to_char(victim, &af);
   SET_BIT(victim->act, ACT_PET);
   ch->pet = victim;
   act("$N's eyes burn bright red as its energy is bound to your soul.", ch, NULL, victim, TO_CHAR);
   act("$N's eyes burn bright red for a moment.", ch, NULL, victim, TO_NOTVICT);
   return;
}

void spell_lightshield( int sn, int level, CHAR_DATA *ch, void *vo, int target ) {
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;

   if (house_down(ch, HOUSE_LIFE))
   return;

   if (!IS_GOOD(victim))
   {
      if (victim == ch)
      send_to_char("You are not pure enough to receive the gift of light.\n\r", ch);
      else
      act("$N is not pure enough to receive the gift of light.", ch, NULL, victim, TO_CHAR);
      return;
   }

   if ( is_affected( victim, sn ) )
   {
      if (victim == ch)
      send_to_char("You are already protected.\n\r", ch);
      else
      act("$N is already protected.", ch, NULL, victim, TO_CHAR);
      return;
   }
   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.duration  = level;
   af.location  = APPLY_NONE;
   af.modifier  = 0;
   af.bitvector = AFF_DETECT_EVIL;
   affect_to_char( victim, &af);
   af.modifier  = -level;
   af.location  = APPLY_AC;
   af.bitvector = 0;
   affect_to_char( victim, &af );
   af.location = APPLY_SAVING_SPELL;
   af.modifier = -level/10;
   af.bitvector = AFF_INFRARED;
   affect_to_char( victim, &af );
   send_to_char( "You are surrounded by a glowing afflatus of purity.\n\r", victim );
   if ( ch != victim )
   act("$N is surrounded by a glowing afflatus of purity.", ch, NULL, victim, TO_CHAR);
   return;
}


/* Some new skills for classes */

void spell_frostbolt(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   int dam;

   if (victim == ch)
   {
      act("$n is blasted with a bolt of ice!", ch, NULL, NULL, TO_ROOM);
      send_to_char("You are blasted by a bolt of ice!\n\r", ch);
   }
   else
   {
      act("$n points at $N and a bolt of ice flies forth!", ch, NULL, victim, TO_NOTVICT);
      act("$n points at you and a bolt of ice flies forth!", ch, NULL, victim, TO_VICT);
      act("You point at $N and a bolt of ice flies forth!", ch, NULL, victim, TO_CHAR);
   }
   dam = dice(level, 7);
   if (saves_spell(ch, level, victim, DAM_COLD, SAVE_SPELL))
   dam /= 2;
   damage(ch, victim, dam*race_adjust(ch)/25.0, sn, DAM_COLD, TRUE);
   return;
}

void spell_icelance(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   int dam;

   if (victim == ch)
   {
      act("$n throws out a shard of sharp ice!", ch, NULL, NULL, TO_ROOM);
      send_to_char("You are struck by your shard of ice!\n\r", ch);
   }
   else
   {
      act("$n throws forth a shard of sharp ice at $N!", ch, NULL, victim, TO_NOTVICT);
      act("$n throws forth a shard of sharp ice at you!", ch, NULL, victim, TO_VICT);
      act("You point at $N and throw forth a shard of ice!", ch, NULL, victim, TO_CHAR);
   }
   dam = dice(level, 6);
   if (saves_spell(ch, level, victim, DAM_COLD, SAVE_SPELL))
   dam /= 2;
   damage(ch, victim, dam*race_adjust(ch)/25.0, sn, DAM_COLD, TRUE);
   return;
}

void do_stealth(CHAR_DATA *ch, char * argument)
{
   AFFECT_DATA af;
   if (house_down(ch, HOUSE_OUTLAW))
   return;

   if (!has_skill(ch, gsn_stealth) || get_skill(ch, gsn_stealth) < 1)
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }

   if (number_percent() > get_skill(ch, gsn_stealth))
   {
      send_to_char("You fail to become more stealthy.\n\r", ch);
      check_improve(ch, gsn_stealth, FALSE, 1);
      return;
   }
   check_improve(ch, gsn_stealth, TRUE, 1);

   if (is_affected(ch, gsn_stealth))
   {
      send_to_char("You are being as stealthy as you know how.\n\r", ch);
      return;
   }
   act("$n vanishes into the crowds!", ch, NULL, NULL, TO_ROOM);
   send_to_char("You begin to blend in with the crowds.\n\r", ch);

   af.where = TO_AFFECTS;
   af.type = gsn_stealth;
   af.level = ch->level;
   af.modifier = 0;
   af.bitvector = 0;
   af.duration = 12;
   af.location = 0;
   affect_to_char(ch, &af);

   return;
}



void spell_earthfade(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   AFFECT_DATA af;
   if (is_affected(ch, sn))
   {
      send_to_char("You are already melded with the ground.\n\r", ch);
      return;
   }
   if (ch->in_room->sector_type == SECT_WATER_SWIM ||
   ch->in_room->sector_type == SECT_UNDERWATER
   || ch->in_room->sector_type == SECT_WATER_NOSWIM
   || ch->in_room->sector_type == SECT_AIR)
   {
      send_to_char("There isn't enough natural earth to meld with here.\n\r", ch);
      return;
   }

   act("$n vanishes into the ground!", ch, NULL, NULL, TO_ROOM);
   send_to_char("You vanish into the ground!\n\r", ch);

   af.where = TO_AFFECTS;
   af.type = gsn_earthfade;
   af.level = level;
   af.modifier = 0;
   af.bitvector = 0;
   af.duration = (level/5)*race_adjust(ch)/25.0;
   af.location = 0;
   affect_to_char(ch, &af);

   return;
}

void spell_graft_flesh(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   OBJ_DATA *part;
   OBJ_DATA *part_found = NULL;
   int repair;


   for (part = ch->carrying; part != NULL; part = part->next_content)
   {
      if ((part->pIndexData->vnum == OBJ_VNUM_SEVERED_HEAD
      || part->pIndexData->vnum == OBJ_VNUM_TORN_HEART
      || part->pIndexData->vnum == OBJ_VNUM_SLICED_ARM
      || part->pIndexData->vnum == OBJ_VNUM_SLICED_LEG
      || part->pIndexData->vnum == OBJ_VNUM_GUTS
      || part->pIndexData->vnum == OBJ_VNUM_BRAINS))
      if (part_found == NULL)        part_found = part;
   }
   if (part_found == NULL){
      send_to_char("You lack fresh body parts from which to gather flesh.\n\r", ch);
      return;
   }
   if (IS_NPC(victim) && victim->pIndexData->vnum != MOB_VNUM_L_GOLEM){
      send_to_char("You cannot repair them with your spare flesh.\n\r", ch);
      return;
   }
   if (!IS_NPC(victim) && victim != ch){
      send_to_char("You cannot repair them with your spare flesh.\n\r", ch);
      return;
   }
   act("$n gruesomely takes flesh from $p and applies it to $N.", ch, part_found, victim, TO_ROOM);
   if (victim != ch){
      act("$n gruesomely takes flesh from $p and applies it to you.", ch, part_found, victim, TO_VICT);
      act("You gruesomely take flesh from $p and apply it to $N.", ch, part_found, victim, TO_CHAR);
   }
   else
   {
      act("You gruesomely take flesh from $p and apply it to yourself.", ch, part_found, victim, TO_CHAR);
   }
   extract_obj(part_found, FALSE);
   repair = ch->level + 25;
   if (IS_NPC(victim)) repair *= 2;
   victim->hit = UMIN( victim->hit + repair, victim->max_hit );
   return;
}

void spell_lesser_golem(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *golem;
   AFFECT_DATA af;
   CHAR_DATA *check;
   OBJ_DATA *part;
   OBJ_DATA *part_next;
   int parts, lvl, aflag, gcount;

   parts = 0;
   aflag = 0;
   gcount = 0;

   if ((!IS_NPC(ch)) && (ch->pcdata->special == SUBCLASS_ANATOMIST))
   aflag = 1;

   if (is_affected(ch, sn))
   {
      send_to_char("You aren't up to fashioning another golem yet.\n\r", ch);
      return;
   }

   for (check = char_list; check != NULL; check = check->next)
   {
      if
      (
         IS_NPC(check) &&
         check->master == ch &&
         check->pIndexData->vnum == MOB_VNUM_L_GOLEM &&
         !is_affected(check, gsn_seize)
      )
      {
         gcount += 1;
      }
   }

   if ((IS_NPC(ch)) && (gcount >=1))
   {
      send_to_char("You already control as many greater golems as possible.\n\r", ch);
      return;
   }

   if (((gcount >= 1) && (ch->pcdata->special != SUBCLASS_ANATOMIST))
   || ((gcount >= 3) && (ch->pcdata->special == SUBCLASS_ANATOMIST)))
   {
      send_to_char("You already control as many lesser golems as possible.\n\r", ch);
      return;
   }

   for (part = ch->carrying; part != NULL; part = part_next)
   {
      part_next = part->next_content;
      if (part->pIndexData->vnum != OBJ_VNUM_SEVERED_HEAD
      && part->pIndexData->vnum != OBJ_VNUM_TORN_HEART
      && part->pIndexData->vnum != OBJ_VNUM_SLICED_ARM
      && part->pIndexData->vnum != OBJ_VNUM_SLICED_LEG
      && part->pIndexData->vnum != OBJ_VNUM_GUTS
      && part->pIndexData->vnum != OBJ_VNUM_BRAINS)
      continue;
      parts++;
   }


   if (parts == 0)
   {
      send_to_char("You don't have any body parts to animate.\n\r", ch);
      return;
   }
   else if (parts <= 1)
   {
      send_to_char("You don't have enough parts to build a flesh golem.\n\r", ch);
      return;
   }

   for (part = ch->carrying; part != NULL; part = part_next)
   {
      part_next = part->next_content;
      if (part->pIndexData->vnum != OBJ_VNUM_SEVERED_HEAD
      && part->pIndexData->vnum != OBJ_VNUM_TORN_HEART
      && part->pIndexData->vnum != OBJ_VNUM_SLICED_ARM
      && part->pIndexData->vnum != OBJ_VNUM_SLICED_LEG
      && part->pIndexData->vnum != OBJ_VNUM_GUTS
      && part->pIndexData->vnum != OBJ_VNUM_BRAINS)
      continue;
      extract_obj(part, FALSE);
   }
   if (parts >= ch->level/5)
   parts = ch->level/5;

   lvl = (level - 3 + parts);
   if (aflag == 1)
   lvl += 5;

   af.where = TO_AFFECTS;
   af.level = level;
   af.location = 0;
   af.modifier = 0;
   af.duration = 30;
   af.bitvector = 0;
   af.type = sn;
   affect_to_char(ch, &af);
   act("$n fashions a flesh golem!", ch, NULL, NULL, TO_ROOM);
   send_to_char("You fashion a flesh golem to serve you!\n\r", ch);

   golem = create_mobile(get_mob_index(MOB_VNUM_L_GOLEM) );
   golem->level = lvl;
   golem->damroll += level/2 + parts;
   if (parts <= 3)
   golem->max_hit = ((lvl*lvl/2) + dice(lvl, 10));
   else if (parts == 4)
   golem->max_hit = ((lvl*lvl/2) + dice(lvl, 12));
   else if (parts == 5)
   golem->max_hit = ((lvl*lvl/2) + dice(lvl, 13));
   else if (parts == 6)
   golem->max_hit = ((lvl*lvl/2) + dice(lvl, 15));
   else if (parts == 7)
   golem->max_hit = ((lvl*lvl/2) + dice(lvl, 18));
   else
   golem->max_hit = ((lvl*lvl/2) + dice(lvl, 20));

   golem->hit = golem->max_hit;
   golem->max_move = ch->max_move;
   golem->move = golem->max_move;
   char_to_room(golem, ch->in_room);
   SET_BIT(golem->affected_by, AFF_CHARM);
   add_follower(golem, ch);
   golem->leader = ch;
   return;
}

void spell_forget(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   AFFECT_DATA af;
   CHAR_DATA *victim = (CHAR_DATA *) vo;

   if (is_affected(victim, sn))
   {
      if (victim == ch)
      send_to_char("You are already as senile as you can get.\n\r", ch);
      else
      act("$E is already affected by a forget spell.", ch, NULL, victim, TO_CHAR);
      /*        send_to_char("They are already affected by a forget spell.\n\r", ch); */
      return;
   }

   if ( saves_spell(ch, (level - 3), victim, DAM_OTHER, SAVE_MALEDICT)
   || saves_armor_of_god(ch, level, victim) )
   {
      send_to_char("You failed.\n\r", ch);
      return;
   }
   af.type = sn;
   af.modifier = 0;
   af.duration = level/10*race_adjust(ch)/25.0;
   af.location = 0;
   af.bitvector = 0;
   af.level = level;
   af.where = TO_AFFECTS;
   affect_to_char(victim, &af);
   send_to_char("Your mind suddenly seems all fuzzy and disoriented.\n\r", victim);
   act("$n suddenly looks disoriented.", victim, NULL, NULL, TO_ROOM);
   return;
}

void spell_cremate(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   int dam;

   dam = dice(level, 7);

   act("$n is engulfed in raging fire!", victim, NULL, NULL, TO_ROOM);
   send_to_char("You are engulfed in raging fire!\n\r", victim);

   if ( saves_spell( ch, level, victim, DAM_FIRE, SAVE_SPELL ) )
   dam /= 2;
   damage( ch, victim, dam*race_adjust(ch)/25.0, sn, DAM_FIRE, TRUE);
   return;
}

void spell_divine_touch(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   AFFECT_DATA af;
   if (is_affected(ch, sn))
   {
      send_to_char("You are already blessed with a divine touch.\n\r", ch);
      return;
   }
   af.type = sn;
   af.modifier = 0;
   af.location = 0;
   af.bitvector = 0;
   af.duration = level/5*race_adjust(ch)/25.0;
   af.level = level;
   affect_to_char(ch, &af);
   send_to_char("Your hands are surrounded in holy energy.\n\r", ch);
   act("$n's hands seem to glow with an inner energy.", ch, NULL, NULL, TO_ROOM);
   return;
}

void spell_transfer_object(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim;
   OBJ_DATA *obj;
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];

   target_name = one_argument(target_name, arg1);
   one_argument(target_name, arg2);

   if (arg1[0] == '\0'
   || arg2[0] == '\0')
   {
      send_to_char("Send what to whom?\n\r", ch);
      return;
   }

   if ( ( obj = get_obj_carry( ch, arg1, ch ) ) == NULL )
   {
      send_to_char( "You are not carrying that.\n\r", ch );
      return;
   }


   victim = get_char_world(ch, arg2);
   if (victim == NULL)
   {
      send_to_char("They aren't here.\n\r", ch);
      return;
   }

   if (!strcmp(ch->in_room->area->name, "Haunted Mine"))
   {
      send_to_char("You failed.\n\r", ch);
      return;
   }

   if (
         !IS_IMMORTAL(ch) &&
         IS_SET(ch->in_room->extra_room_flags, ROOM_1212)
      )
   {
      send_to_char("You failed.\n\r", ch);
      sprintf(
         log_buf,
         "[%s] attempted to transfer %s to %s out of 1212.\n\r",
         ch->name,
         obj->short_descr,
         victim->name);
      log_string(log_buf);
      return;
   }

   if (IS_NPC(victim))
   {
      act("$E wouldn't want that sent to $M.", ch, NULL, victim, TO_CHAR);
      /*      send_to_char("They wouldn't want that sent to them.\n\r", ch); */
      return;
   }
   if (victim == ch)
   {
      send_to_char("Why not just hold it in your other hand?\n\r", ch);
      return;
   }
   if (IS_IMMORTAL(victim) && !IS_IMMORTAL(ch))
   {
      send_to_char("You can't send to an immortal.\n\r", ch);
      return;
   }
   if (IS_SET(obj->extra_flags, ITEM_NODROP))
   {
      send_to_char("You can't seem to remove it from yourself to prepare for transfer.\n\r", ch);
      return;
   }
   if
   (
      victim->carry_number >= can_carry_n(victim)
      || victim->carry_weight >= can_carry_w(victim)
      || obj->pIndexData->house
   )
   {
      act("$p glows softly but nothing happens.", ch, obj, NULL, TO_CHAR);
      act("$p glows softly but nothing happens.", ch, obj, NULL, TO_ROOM);
      return;
   }

   if (IS_SET(victim->act, PLR_NO_TRANSFER))
   {
      act("$E wouldn't want that sent to $M.", ch, NULL, victim, TO_CHAR);
      /*      send_to_char("They wouldn't want them sent to them.\n\r", ch); */
      return;
   }

   act("$p glows softly and vanishes!", ch, obj, NULL, TO_CHAR);
   act("$p glows softly and vanishes!", ch, obj, NULL, TO_ROOM);
   obj_from_char(obj);
   obj_to_char(obj, victim);
   act("You suddenly feel heavier as $p pops into your inventory!", victim, obj, NULL, TO_CHAR);
   act("$p suddenly appears from nowhere and pops into $n's possession!", victim, obj, NULL, TO_ROOM);
   if (!IS_NPC(ch) && !IS_NPC(victim))
   {
      if (!strcmp(ch->host, victim->host))
      {
         sprintf(buf, "[site] %s (%d) sent %s (%d) to %s (%d)",
         ch->name, get_trust(ch), obj->short_descr, obj->level,
         victim->name, get_trust(victim));
         wiznet(buf, ch, NULL, WIZ_TRANSFERS, 0, get_trust(ch));
         log_string(buf);
      } else if ((get_trust(victim) <= 10) && (get_trust(ch) > 10)
      && (get_trust(ch) > get_trust(victim) + 8)
      && (obj->level > get_trust(victim) + 8)
      && (ch->house != victim->house))
      {
         sprintf(buf, "[newbie] %s (%d) sent %s (%d) to %s (%d)",
         ch->name, get_trust(ch), obj->short_descr, obj->level,
         victim->name, get_trust(victim));
         wiznet(buf, ch, NULL, WIZ_TRANSFERS, 0, get_trust(ch));
         log_string(buf);
      }
   }
   return;
}

void spell_disintegrate(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   OBJ_DATA *obj;
   OBJ_DATA *obj_next;
   int dam, obj_save, modify, saves=0;

   for ( modify = 0; modify < 4; modify++)
   {
      if (saves_spell(ch, level, victim, DAM_ENERGY, SAVE_DEATH) )
      saves++;
   }

   if (((!IS_NPC(victim) && /* (saves_spell(ch, (level - 1), victim, DAM_ENERGY, SAVE_DEATH)
   || saves_spell(ch, (level - 4), victim, DAM_ENERGY, SAVE_DEATH)
   || saves_spell(ch, (level + 2), victim, DAM_ENERGY, SAVE_DEATH)*/
   saves > 0) ||
   (IS_NPC(victim) &&(saves_spell(ch, level-2, victim, DAM_ENERGY, SAVE_DEATH) ||
   saves_spell(ch, level-2, victim, DAM_ENERGY, SAVE_DEATH))))
   || !can_instakill(ch, victim))
   {
      act("$n shudders and spasms momentarily.", victim, NULL, NULL, TO_ROOM);
      send_to_char("You feel a sudden intense pain throughout your body!\n\r", victim);
      dam = dice(level, 10)*race_adjust(ch)/25.0;
      damage(ch, victim, saves_spell(ch, level, victim, DAM_ENERGY, SAVE_DEATH) ? dam/2 : dam, sn, DAM_ENERGY, TRUE);
      return;
   }

   act("$n vaporises in a cloud of bloody red mist!", victim, NULL, NULL, TO_ROOM);
   send_to_char("You feel a sudden intense pain as your body vaporises into a red mist!\n\r", victim);
   for (obj = victim->carrying; obj != NULL; obj = obj_next)
   {
      obj_next = obj->next_content;
      obj_save = obj->level;
      if (obj->pIndexData->limtotal == 1)
      obj_save += 10;
      else if (obj->pIndexData->limtotal <= 3
      && obj->pIndexData->limtotal > 0)
      obj_save += 7;
      else if (obj->pIndexData->limtotal <= 15
      && obj->pIndexData->limtotal > 0)
      obj_save += 4;
      else if (obj->pIndexData->limtotal <= 20
      && obj->pIndexData->limtotal > 0)
      obj_save += 2;
      else if (obj->pIndexData->limtotal > 0)
      obj_save += 1;

      obj_save *= 2;
      obj_save -= (2 * level);
      obj_save += 50;
      if ((number_percent()*race_adjust(ch)/25.0 > obj_save) && (obj->wear_loc != WEAR_BRAND))
      {
         act("$p disintegrates in a flash of light!", ch, obj, NULL, TO_ROOM);
         act("$p disintegrates in a flash of light!", ch, obj, NULL, TO_CHAR);
         extract_obj(obj, FALSE);
      }
   }

   raw_kill(ch, victim);
   return;

}

void spell_grounding(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   AFFECT_DATA af;

   if (is_affected(ch, sn))
   {
      send_to_char("Your body is already at a ground potential.\n\r", ch);
      return;
   }

   send_to_char("Your body loses all affinity to electricity.\n\r", ch);
   af.where = TO_AFFECTS;
   af.type = sn;
   af.modifier = 0;
   af.level = level;
   af.duration = ch->level/8;
   af.bitvector = 0;
   af.location = 0;
   affect_to_char(ch, &af);
   return;
}

void spell_freedom_cry(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   return;
}


void spell_shock_sphere(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;
   int dam;

   act("A sphere of crackling energy detonates around $n with explosive sound!", victim, NULL, NULL, TO_ROOM);
   send_to_char("A sphere of fierce energy detonates around you with deafening sound!\n\r", victim);

   if (is_affected(victim, gsn_shock_sphere))
   {
      dam = dice(level, 3)*race_adjust(ch)/25.0;
      damage(ch, victim, saves_spell(ch, level, victim, DAM_SOUND, SAVE_SPELL) ? dam/2 : dam, sn, DAM_SOUND, TRUE);
      return;
   }
   if (saves_spell(ch, level, victim, DAM_SOUND, SAVE_SPELL))
   {
      dam = dice(level, 5)*race_adjust(ch)/25.0;
      damage(ch, victim, saves_spell(ch, level, victim, DAM_SOUND, SAVE_SPELL) ? dam/2 : dam, sn, DAM_SOUND, TRUE);
      return;
   }

   dam = dice(level, 7)*race_adjust(ch)/25.0;

   act("$n appears deafened.", victim, NULL, NULL, TO_ROOM);
   send_to_char("You can't hear a thing!\n\r", victim);

   af.where = TO_AFFECTS;
   af.modifier = -2;
   af.location = APPLY_HITROLL;
   af.bitvector = 0;
   af.type = gsn_shock_sphere;
   af.level = level;
   af.duration = level/8*race_adjust(ch)/25.0;
   affect_to_char(victim, &af);

   damage(ch, victim, saves_spell(ch, level, victim, DAM_SOUND, SAVE_SPELL) ? dam/2 : dam, sn, DAM_SOUND, TRUE);


   return;
}

void spell_animate_dead(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   CHAR_DATA* zombie;
   OBJ_DATA* corpse;
   OBJ_DATA* obj_next;
   OBJ_DATA* obj;
   CHAR_DATA* search;
   AFFECT_DATA af;
   char* name;
   char buf1[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];
   int chance;
   int z_level;
   int control;
   int total;
   OBJ_DATA* skull;


   if
   (
      (
         skull = get_eq_char(ch, WEAR_HEAD)
      ) != NULL &&
      skull->pIndexData->vnum != OBJ_VNUM_SKULL_NECROMANCY
   )
   {
      skull = NULL;
   }

   if (level == 72)  /* necromancy scroll */
   {
      target_name = "corpse";
   }

   if
   (
      is_affected(ch, sn) &&
      level < 70
   )
   {
      send_to_char("You have not yet regained your powers over the dead.\n\r", ch);
      return;
   }

   control = 0;
   total = 0;

   for (search = char_list; search != NULL; search = search->next)
   {
      if
      (
         search->in_room == NULL ||
         !IS_NPC(search) ||
         search->master != ch ||
         !IS_AFFECTED(search, AFF_CHARM) ||
         is_affected(search, gsn_seize)
      )
      {
         continue;
      }
      if (search->pIndexData->vnum == MOB_VNUM_ZOMBIE)
      {
         control += 6;
         total++;
      }
      else if (search->pIndexData->vnum == MOB_VNUM_SKELETON)
      {
         control += 4;
         total++;
      }
      else if (search->pIndexData->vnum == MOB_VNUM_MUMMY)
      {
         total++;
         control += 12;
      }
   }
   control += 6;

   if
   (
      (
         level < 30 &&
         control > 12
      ) ||
      (
         level < 35 &&
         control > 18
      ) ||
      (
         level < 40 &&
         control > 24
      ) ||
      (
         level < 52 &&
         control > 30
      ) ||
      control >= 36
   )
   {
      send_to_char("You already control as many undead as you can.\n\r", ch);
      return;
   }

   if (total >= 7)
   {
      send_to_char("You already control as many undead as you can.\n\r", ch);
      return;
   }
   if (target_name[0] == '\0')
   {
      send_to_char("Animate which corpse?\n\r", ch);
      return;
   }
   corpse = get_obj_here(ch, target_name);

   if
   (
      corpse == NULL ||
      (
         corpse->item_type != ITEM_CORPSE_NPC &&
         corpse->item_type != ITEM_CORPSE_PC
      )
   )
   {
      send_to_char("You cannot animate that.\n\r", ch);
      return;
   }

   if (IS_SET(corpse->extra_flags, CORPSE_NO_ANIMATE))
   {
      send_to_char
      (
         "That corpse cannot sustain further life beyond the grave.\n\r",
         ch
      );
      return;
   }
   /*
   -Fizz
   Riallus decides we can animate without brains
   if (IS_SET(corpse->extra_flags, ITEM_NO_BRAINS))
   {
      send_to_char
      (
         "Zombies need brains to enter undeath.\n\r",
         ch
      );
      return;
   }
   */
   /*
      Add levels after control check,
      so order does not matter
   */
   if
   (
      !IS_NPC(ch) &&
      ch->pcdata->special == SUBCLASS_NECROPHILE
   )
   {
      level += 5;
   }

   for (obj = corpse->contains; obj != NULL; obj = obj_next)
   {
      obj_next = obj->next_content;
      obj_from_obj(obj);
      obj_to_room(obj, ch->in_room);
   }
   af.where = TO_AFFECTS;
   af.type = sn;
   af.level = level;
   af.duration = 12;
   af.modifier = 0;
   af.location = 0;
   af.bitvector = 0;
   chance = get_skill(ch, sn);
   if (level < corpse->level)
   {
      chance += 3 * level;
      chance -= 3 * corpse->level;
   }
   chance = URANGE(20, chance, 90);
   if
   (
      skull == NULL &&
      number_percent() * 25.0 / race_adjust(ch) > chance
   )
   {
      act("You fail and destroy $p.", ch, corpse, NULL, TO_CHAR);
      act
      (
         "$n tries to animate a corpse but destroys it.",
         ch,
         NULL,
         NULL,
         TO_ROOM
      );
      extract_obj(corpse, FALSE);
      if (level < 70)
      {
         af.duration /= 8;
         affect_to_char(ch, &af);
      }
      return;
   }
   if (level < 70)
   {
      affect_to_char(ch, &af);
   }
   act
   (
      "$n utters an incantation and a burning red glow flares into the eyes of $p.",
      ch,
      corpse,
      NULL,
      TO_ROOM
   );
   act
   (
      "$p shudders and comes to life!",
      ch,
      corpse,
      NULL,
      TO_ROOM
   );
   act
   (
      "You call upon the powers of the dark to give life to $p.",
      ch,
      corpse,
      NULL,
      TO_CHAR
   );
   zombie = create_mobile(get_mob_index(MOB_VNUM_ZOMBIE));
   char_to_room(zombie, ch->in_room);
   z_level = UMAX(1, corpse->level - number_range(3, 6));
   if (z_level > (level + 10))
   {
      z_level = level + 10;
   }
   if
   (
      !IS_NPC(ch) &&
      ch->pcdata->special == SUBCLASS_NECROPHILE
   )
   {
      z_level += 5;
   }
   if (skull != NULL)
   {
      z_level++;
   }
   zombie->level = z_level;
   zombie->max_hit = dice(z_level, 15) + (z_level * 20);
   zombie->hit = zombie->max_hit;
   zombie->damroll += (z_level * 7 / 10);
   zombie->alignment = -1000;

   name = corpse->short_descr;
   if (!str_prefix("The headless corpse of ", name))
   {
      name = name + 23;
      sprintf(buf1, "the headless zombie of %s", name);
      sprintf(buf2, "A headless zombie of %s is standing here.\n\r", name);
   }
   else
   {
      name = name + 14;
      sprintf(buf1, "the zombie of %s", name);
      sprintf(buf2, "A zombie of %s is standing here.\n\r", name);
   }

   extract_obj(corpse, FALSE);

   free_string(zombie->short_descr);
   free_string(zombie->long_descr);
   zombie->short_descr = str_dup(buf1);
   zombie->long_descr = str_dup(buf2);

   af.duration = -1;
   af.bitvector = AFF_CHARM;
   affect_to_char(zombie, &af);
   add_follower(zombie, ch);
   zombie->leader = ch;

   if (skull != NULL)
   {
      act
      (
         "$n's eyes glow bright red for a moment.",
         zombie,
         NULL,
         NULL,
         TO_ROOM
      );
      SET_BIT(zombie->affected_by, AFF_INFRARED);
   }
   return;
}

void spell_animate_skeleton(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   CHAR_DATA* skeleton;
   OBJ_DATA* corpse;
   OBJ_DATA* obj_next;
   OBJ_DATA* obj;
   CHAR_DATA* search;
   AFFECT_DATA af;
   bool headless = FALSE;
   char* name;
   char buf1[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];
   int chance;
   int z_level;
   int control;
   int total;
   int total_skeleton;
   OBJ_DATA* skull_necr;
   bool skull;

   if
   (
      (
         (
            skull_necr = get_eq_char(ch, WEAR_HEAD)
         ) != NULL &&
         skull_necr->pIndexData->vnum == OBJ_VNUM_SKULL_NECROMANCY
      ) ||
      (
         IS_NPC(ch) &&
         ch->pIndexData->vnum == 18740  /* Spy necromancer */
      )
   )
   {
      skull = TRUE;
   }
   else
   {
      skull = FALSE;
   }


   if (is_affected(ch, sn))
   {
      send_to_char
      (
         "You have not yet regained your powers to animate bones.\n\r",
         ch
      );
      return;
   }

   control = 0;
   total = 0;
   total_skeleton = 0;
   for (search = char_list; search != NULL; search = search->next)
   {
      if
      (
         search->in_room == NULL ||
         !IS_NPC(search) ||
         search->master != ch ||
         !IS_AFFECTED(search, AFF_CHARM) ||
         is_affected(search, gsn_seize)
      )
      {
         continue;
      }
      if (search->pIndexData->vnum == MOB_VNUM_ZOMBIE)
      {
         control += 6;
         total++;
      }
      else if (search->pIndexData->vnum == MOB_VNUM_SKELETON)
      {
         control += 4;
         total_skeleton++;
         total++;
      }
      else if (search->pIndexData->vnum == MOB_VNUM_MUMMY)
      {
         total++;
         control += 12;
      }
   }
   control += 4;
   if
   (
      IS_NPC(ch) &&
      ch->pIndexData->vnum == 18740 /* Spy necromancer */
   )
   {
      if (total_skeleton >= 3)
      {
         return;
      }
   }
   else
   {
      if
      (
         ch->class != CLASS_NECROMANCER &&
         control > 12
      )
      {
         send_to_char("You already control as many undead as you can.\n\r", ch);
         return;
      }
      if (total_skeleton >= 5)
      {
         send_to_char
         (
            "You already control as many skeletons as you can.\n\r",
            ch
         );
         return;
      }
      if (total >= 7)
      {
         send_to_char("You already control as many undead as you can.\n\r", ch);
         return;
      }
      if
      (
         (
            level < 30 &&
            control > 12
         ) ||
         (
            level < 35 &&
            control > 18
         ) ||
         (
            level < 40 &&
            control > 24
         ) ||
         (
            level < 52 &&
            control > 30
         ) ||
         control >= 36
      )
      {
         send_to_char("You already control as many undead as you can.\n\r", ch);
         return;
      }
   }

   if (target_name[0] == '\0')
   {
      send_to_char("Animate which skeleton?\n\r", ch);
      return;
   }

   corpse = get_obj_here(ch, target_name);

   if
   (
      corpse == NULL ||
      corpse->item_type != ITEM_SKELETON
   )
   {
      send_to_char("You cannot animate that.\n\r", ch);
      return;
   }

   if (IS_SET(corpse->extra_flags, CORPSE_NO_ANIMATE))
   {
      send_to_char
      (
         "That skeleton does not have the stability to be animated anymore."
         "\n\r",
         ch
      );
      return;
   }

   for (obj = corpse->contains; obj != NULL; obj = obj_next)
   {
      obj_next = obj->next_content;
      obj_from_obj(obj);
      obj_to_room(obj, ch->in_room);
   }


   af.where = TO_AFFECTS;
   af.type = sn;
   af.level = level;
   af.duration = 8;
   af.modifier = 0;
   af.location = 0;
   af.bitvector = 0;

   chance = get_skill(ch, sn);

   if (level < corpse->level)
   {
      chance += 4 * level;
      chance -= 3 * corpse->level;
   }

   chance = URANGE(10, chance, 95);

   if
   (
      skull == FALSE &&
      number_percent() * 25.0 / race_adjust(ch) > chance
   )
   {
      act
      (
         "You fail and destroy $p.",
         ch,
         corpse,
         NULL,
         TO_CHAR
      );
      act
      (
         "$n tries to animate a skeleton but destroys it.",
         ch,
         NULL,
         NULL,
         TO_ROOM
      );
      extract_obj(corpse, FALSE);
      if (ch->class == CLASS_NECROMANCER)
      {
         af.duration /= 8;
      }
      affect_to_char(ch, &af);
      return;
   }
   if
   (
      !IS_NPC(ch) ||
      ch->pIndexData->vnum != 18740 /* Spy necromancer */
   )
   {
      affect_to_char(ch, &af);
   }

   act
   (
      "$n utters an incantation and $p slowly stumbles to its feet.",
      ch,
      corpse,
      NULL,
      TO_ROOM
   );
   act
   (
      "$p shudders and slowly stumbles to its feet!",
      ch,
      corpse,
      NULL,
      TO_ROOM
   );
   act
   (
      "You invoke the powers of death and $p slowly rises to its feet.",
      ch,
      corpse,
      NULL,
      TO_CHAR
   );

   skeleton = create_mobile(get_mob_index(MOB_VNUM_SKELETON));
   char_to_room(skeleton, ch->in_room);

   z_level = UMAX(1, corpse->level - number_range(6, 11));
   if (z_level > level + 10)
   {
      z_level = level + 10;
   }
   if (skull)
   {
      z_level++;
   }
   skeleton->level = z_level;
   skeleton->max_hit = dice(z_level, 12) + (z_level * 15);
   skeleton->hit = skeleton->max_hit;
   skeleton->damroll += z_level * 2 / 3;
   skeleton->alignment = -1000;

   name = corpse->short_descr;
   if (!str_prefix("The headless skeleton of ", name))
   {
      name = name + 25;
      headless = TRUE;
   }
   else
   {
      name = name + 16;
   }

   extract_obj(corpse, FALSE);

   if (headless)
   {
      sprintf(buf1, "the headless skeleton of %s", name);
      sprintf(buf2, "A headless skeleton of %s is standing here.\n\r", name);
   }
   else
   {
      sprintf(buf1, "the skeleton of %s", name);
      sprintf(buf2, "A skeleton of %s is standing here.\n\r", name);
   }
   free_string(skeleton->short_descr);
   free_string(skeleton->long_descr);
   skeleton->short_descr = str_dup(buf1);
   skeleton->long_descr = str_dup(buf2);
   if (headless)
   {
      free_string(skeleton->description);
      skeleton->description = str_dup
      (
         "The bones of this undead are pristine clean and move with an impossible\n\r"
         "life of their own.  The neckbone has been shattered, not at all stealing\n\r"
         "attention from the missing head.\n\r"
      );
   }

   af.type = gsn_animate_dead;
   af.duration = -1;
   af.bitvector = AFF_CHARM;
   affect_to_char(skeleton, &af);
   add_follower(skeleton, ch);
   skeleton->leader = ch;

   if (skull)
   {
      act
      (
         "$n's eyes glow bright red for a moment.",
         skeleton,
         NULL,
         NULL,
         TO_ROOM
      );
      SET_BIT(skeleton->affected_by, AFF_INFRARED);
   }

   return;
}


void spell_mummify(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   CHAR_DATA* mummy;
   OBJ_DATA* corpse;
   OBJ_DATA* obj_next;
   OBJ_DATA* obj;
   CHAR_DATA* search;
   AFFECT_DATA af;
   char* name;
   char buf1[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];
   int chance;
   int z_level;
   int control;
   int total;
   OBJ_DATA* skull;


   if
   (
      (
         skull = get_eq_char(ch, WEAR_HEAD)
      ) != NULL &&
      skull->pIndexData->vnum != OBJ_VNUM_SKULL_NECROMANCY
   )
   {
      skull = NULL;
   }


   if
   (
      is_affected(ch, sn) &&
      level < 70
   )
   {
      send_to_char
      (
         "You have not yet regained your powers over the dead.\n\r",
         ch
      );
      return;
   }

   control = 0;
   total = 0;

   for (search = char_list; search != NULL; search = search->next)
   {
      if
      (
         search->in_room == NULL ||
         !IS_NPC(search) ||
         search->master != ch ||
         !IS_AFFECTED(search, AFF_CHARM) ||
         is_affected(search, gsn_seize)
      )
      {
         continue;
      }
      if (search->pIndexData->vnum == MOB_VNUM_ZOMBIE)
      {
         control += 6;
         total++;
      }
      else if (search->pIndexData->vnum == MOB_VNUM_SKELETON)
      {
         control += 4;
         total++;
      }
      else if (search->pIndexData->vnum == MOB_VNUM_MUMMY)
      {
         total++;
         control += 12;
      }
   }
   control += 12;

   if
   (
      (
         level < 30 &&
         control > 12
      ) ||
      (
         level < 35 &&
         control > 18
      ) ||
      (
         level < 40 &&
         control > 24
      ) ||
      (
         level < 52 &&
         control > 30
      ) ||
      control >= 36
   )
   {
      send_to_char("You already control as many undead as you can.\n\r", ch);
      return;
   }

   if (total >= 7)
   {
      send_to_char("You already control as many undead as you can.\n\r", ch);
      return;
   }

   if (level >= 70)
   {
      target_name = "corpse";
   }

   if (target_name[0] == '\0')
   {
      send_to_char("Attempt to mummify which corpse?\n\r", ch);
      return;
   }

   corpse = get_obj_here(ch, target_name);

   if
   (
      corpse == NULL ||
      (
         corpse->item_type != ITEM_CORPSE_NPC &&
         corpse->item_type != ITEM_CORPSE_PC
      )
   )
   {
      send_to_char("You cannot mummify that.\n\r", ch);
      return;
   }

   if (IS_SET(corpse->extra_flags, CORPSE_NO_ANIMATE))
   {
      send_to_char
      (
         "That corpse does not have the stability to be animated anymore.\n\r",
         ch
      );
      return;
   }
/*
   -Fizz
   Riallus decides we can animate without brains
   if (IS_SET(corpse->extra_flags, ITEM_NO_BRAINS))
   {
      send_to_char
      (
         "Mummies need brains to enter undeath.\n\r",
         ch
      );
      return;
   }
*/
   for (obj = corpse->contains; obj != NULL; obj = obj_next)
   {
      obj_next = obj->next_content;
      obj_from_obj(obj);
      obj_to_room(obj, ch->in_room);
   }

   af.where = TO_AFFECTS;
   af.type = sn;
   af.level = level;
   af.duration = 17;
   af.modifier = 0;
   af.location = 0;
   af.bitvector = 0;

   chance = get_skill(ch, sn);

   if (level < corpse->level)
   {
      chance += 2 * level;
      chance -= 2 * corpse->level;
   }

   chance = URANGE(10, chance, 90);

   if
   (
      skull == NULL &&
      number_percent() * 25.0 / race_adjust(ch) > chance
   )
   {
      act("You fail and destroy $p.", ch, corpse, NULL, TO_CHAR);
      act("$n tries to mummify $p but destroys it.", ch, corpse, NULL, TO_ROOM);
      extract_obj(corpse, FALSE);
      if (level < 70)
      {
         af.duration /= 8;
         affect_to_char(ch, &af);
      }
      return;
   }
   if (level < 70)
   {
      affect_to_char(ch, &af);
   }

   act
   (
      "$n utters an incantation and $p slowly stumbles to its feet.",
      ch,
      corpse,
      NULL,
      TO_ROOM
   );
   act
   (
      "$p shudders and slowly stumbles to its feet!",
      ch,
      corpse,
      NULL,
      TO_ROOM
   );
   act
   (
      "You invoke the powers of death and $p slowly rises to its feet.",
      ch,
      corpse,
      NULL,
      TO_CHAR
   );

   mummy = create_mobile(get_mob_index(MOB_VNUM_MUMMY));
   char_to_room(mummy, ch->in_room);

   z_level = UMAX(1, corpse->level - number_range(0, 2));
   if (z_level > (level + 10))
   {
      z_level = level + 10;
   }
   if (skull != NULL)
   {
      z_level++;
   }
   mummy->level = z_level;
   mummy->max_hit = dice(z_level, 25) + (z_level * 25);
   mummy->hit = mummy->max_hit;
   mummy->damroll += z_level * 4 / 5;
   mummy->alignment = -1000;

   name = corpse->short_descr;
   if (!str_prefix("The headless corpse of ", name))
   {
      name = name + 23;
      sprintf(buf1, "the headless mummy of %s", name);
      sprintf
      (
         buf2,
         "A torn, shredded, and headless mummy of %s is standing here.\n\r",
         name
      );
   }
   else
   {
      name = name + 14;  /* Skip: "The corpse of " */
      sprintf(buf1, "the mummy of %s", name);
      sprintf
      (
         buf2,
         "A torn and shredded mummy of %s is standing here.\n\r",
         name
      );
   }

   extract_obj(corpse, FALSE);

   free_string(mummy->short_descr);
   free_string(mummy->long_descr);
   mummy->short_descr = str_dup(buf1);
   mummy->long_descr = str_dup(buf2);

   af.type = gsn_animate_dead;
   af.duration = -1;
   af.bitvector = AFF_CHARM;
   affect_to_char(mummy, &af);
   add_follower(mummy, ch);
   mummy->leader = ch;

   if (skull != NULL)
   {
      act
      (
         "$n's eyes glow bright red for a moment.",
         mummy,
         NULL,
         NULL,
         TO_ROOM
      );
      SET_BIT(mummy->affected_by, AFF_INFRARED);
   }

   return;
}



void spell_decay_corpse(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   OBJ_DATA* corpse;
   OBJ_DATA* obj;
   OBJ_DATA* obj_next;
   OBJ_DATA* skeleton;
   bool headless = FALSE;
   char* name;
   int chance;
   char buf1[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];


   corpse = (OBJ_DATA*) vo;

   if (corpse == NULL)
   {
      send_to_char("You cannot find that object.\n\r", ch);
      return;
   }

   if
   (
      corpse->item_type != ITEM_CORPSE_NPC &&
      corpse->item_type != ITEM_CORPSE_PC
   )
   {
      send_to_char("You cannot decay that.\n\r", ch);
      return;
   }


   for (obj = corpse->contains; obj != NULL; obj = obj_next)
   {
      obj_next = obj->next_content;
      obj_from_obj(obj);
      obj_to_room(obj, ch->in_room);
   }

   chance = get_skill(ch, sn);

   if
   (
      (
         !IS_NPC(ch) ||
         ch->pIndexData->vnum != 18740  /* Spy necromancer */
      ) &&
      number_percent() * 25.0 / race_adjust(ch) > chance
   )
   {
      act
      (
         "Your decaying becomes uncontrolled and you destroy $p.",
         ch,
         corpse,
         NULL,
         TO_CHAR
      );
      act
      (
         "$n tries to decay $p but reduces it to a puddle of slime.",
         ch,
         corpse,
         NULL,
         TO_ROOM
      );
      extract_obj(corpse, FALSE);
      return;
   }

   act
   (
      "$n decays the flesh off $p.",
      ch,
      corpse,
      NULL,
      TO_ROOM
   );
   act
   (
      "You decay the flesh off $p and reduce it to a skeleton.",
      ch,
      corpse,
      NULL,
      TO_CHAR
   );

   skeleton = create_object(get_obj_index(OBJ_VNUM_SKELETON), 1);
   obj_to_room(skeleton, ch->in_room);

   if (IS_SET(corpse->extra_flags, CORPSE_NO_ANIMATE))
   {
      SET_BIT(skeleton->extra_flags, CORPSE_NO_ANIMATE);
   }
   skeleton->level = corpse->level;

   name = corpse->short_descr;
   if (!str_prefix("The headless corpse of ", name))
   {
      name = name + 23;
      headless = TRUE;
   }
   else
   {
      name = name + 14;
   }
   if (!str_prefix("the shadow of ", name))
   {
      name = name + 14;
   }
   if (!str_prefix("the skeleton of ", name))
   {
      name = name + 16;
   }
   if (!str_prefix("the headless skeleton of ", name))
   {
      name = name + 25;
   }
   if (headless)
   {
      sprintf(buf1, "the headless skeleton of %s", name);
      sprintf
      (
         buf2,
         "A headless skeleton of %s is lying here in a puddle of decayed flesh.",
         name
      );
   }
   else
   {
      sprintf(buf1, "the skeleton of %s", name);
      sprintf
      (
         buf2,
         "A skeleton of %s is lying here in a puddle of decayed flesh.",
         name
      );
   }
   extract_obj(corpse, FALSE);

   free_string(skeleton->short_descr);
   free_string(skeleton->description);
   skeleton->short_descr = str_dup(buf1);
   skeleton->description = str_dup(buf2);

   return;
}


void spell_preserve(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   OBJ_DATA *obj = (OBJ_DATA *) vo;
   int vnum, chance;

   vnum = obj->pIndexData->vnum;
   if (vnum != OBJ_VNUM_SEVERED_HEAD
   && vnum != OBJ_VNUM_TORN_HEART
   && vnum != OBJ_VNUM_SLICED_ARM
   && vnum != OBJ_VNUM_SLICED_LEG
   && vnum != OBJ_VNUM_GUTS
   && vnum != OBJ_VNUM_BRAINS)
   {
      send_to_char("You can't preserve that.\n\r", ch);
      return;
   }

   if (obj->timer > 10)
   {
      send_to_char("It's already in very well preserved condition.\n\r", ch);
      return;
   }
   chance = get_skill(ch, sn);
   chance *= 90;

   if (number_percent() * 25.0 / race_adjust(ch) > chance)
   {
      act("$n destroys $p.", ch, obj, NULL, TO_ROOM);
      act("You fail and destroy $p.", ch, obj, NULL, TO_CHAR);
      extract_obj(obj, FALSE);
      return;
   }

   act("You surround $p with necromantic magic to slow its decay.", ch, obj, NULL, TO_CHAR);
   obj->timer += number_range(level/2, level)*race_adjust(ch)/25.0;
   return;
}


void spell_power_word_fear(
   int sn,
   int level,
   CHAR_DATA *ch,
   void *vo,
   int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;
   bool bad_fail, utter_fail;
   int range;

   bad_fail = FALSE;
   utter_fail = FALSE;

   if (victim == ch)
   {
      send_to_char("That wouldn't work.\n\r", ch);
      return;
   }

   if (sn != gsn_rite_of_darkness)
   {
      act(
         "$n points at $N and utters the word 'Fear!'",
         ch,
         0,
         victim,
         TO_NOTVICT);
      act(
         "$n points at you and utters the word 'Fear!'",
         ch,
         0,
         victim,
         TO_VICT);
      act(
         "You point at $N and utter the word 'Fear!'",
         ch,
         0,
         victim,
         TO_CHAR);
   }

   if (!IS_AWAKE(victim))
   {
      act("$n shivers momentarily but it passes.", victim, NULL, NULL, TO_ROOM);
      send_to_char(
         "You feel a brief terror, but it passes away in your dreams.\n\r",
         victim);
      return;
   }

   if (is_affected(victim, sn))
   {
      if (sn == gsn_rite_of_darkness)
      {
         act(
            "$N has already faced the horrors of hell.",
            ch,
            NULL,
            victim,
            TO_CHAR);
      }
      else
      {
         act(
            "$E is already affected by a word of power.",
            ch,
            NULL,
            victim,
            TO_CHAR);
      }

      send_to_char(
         "You feel a shiver pass through you but it has "
         "no further affect.\n\r",
         victim);
      return;
   }

   if (IS_NPC(victim) && victim->house != 0)
   {
      act("$n shivers momentarily but it passes.", victim, NULL, NULL, TO_ROOM);
      send_to_char("You feel a brief terror, but it passes.\n\r", victim);
      return;
   }

   if (
         saves_spell(ch, level, victim, DAM_OTHER, SAVE_MALEDICT) ||
         saves_armor_of_god(ch, level, victim)
      )
   {
      act("$n shivers momentarily but it passes.", victim, NULL, NULL, TO_ROOM);
      send_to_char("You feel a brief terror, but it passes.\n\r", victim);
      return;
   }

   if (
         !saves_spell(ch, (level - 2), victim, DAM_OTHER, SAVE_DEATH) &&
         !saves_armor_of_god(ch, level, victim)
      )
   {
      bad_fail = TRUE;
      if (
            !saves_spell(ch, (level - 5), victim, DAM_OTHER, SAVE_DEATH) &&
            number_range(1, 100) <= 4
         )
      {
         utter_fail = TRUE;
      }
   }

   if (utter_fail && can_instakill(ch, victim))
   {
      act(
         "$n's eyes widen and $s heart ruptures from shock!",
         victim,
         0,
         0,
         TO_ROOM);
      send_to_char(
         "You feel a terror so intense your heart stops dead!\n\r",
         victim);
      raw_kill(ch, victim);
      return;
   }

   act(
      "$n's eyes widen in shock and $s entire body freezes "
      "in momentary terror.",
      victim,
      NULL,
      NULL,
      TO_ROOM);
   send_to_char(
      "You feel an overwhelming terror and you shudder in "
      "momentary shock.\n\r",
      victim);

   range = level/10;

   af.where = TO_AFFECTS;
   af.type = sn;
   af.level = level;
   af.bitvector = 0;
   af.duration = (number_range(1, 5) + range);
   af.location = APPLY_CON;
   af.modifier = -number_range(2, range + 1);
   affect_to_char(victim, &af);

   af.location = APPLY_STR;
   af.modifier = -number_range(2, range + 1);
   affect_to_char(victim, &af);

   af.location = APPLY_DEX;
   af.modifier = -number_range(1, range);
   affect_to_char(victim, &af);

   af.location = APPLY_HIT;
   af.modifier = -number_range(2, range);
   affect_to_char(victim, &af);

   af.location = APPLY_DAMROLL;
   af.modifier = -number_range(2, range);
   affect_to_char(victim, &af);

   if (victim->position == POS_FIGHTING)
   {
      do_flee(victim, "");
   }

   if (victim->position == POS_FIGHTING)
   {
      do_flee(victim, "");
   }

   if (victim->position == POS_FIGHTING)
   {
      do_flee(victim, "");
   }

   if (bad_fail)
   {
      DAZE_STATE(victim, 12);
   }
}

void spell_nightfall(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   CHAR_DATA *check;

   if (victim->nightfall != NULL
   || (IS_NPC(victim) && victim->pIndexData->vnum == MOB_VNUM_NIGHTFALL))
   {
      send_to_char("That victim's soul is already tainted.\n\r", ch);
      return;
   }
   for (check = char_list; check != NULL; check = check->next)
   {
      if (IS_NPC(check) && check->pIndexData->vnum == MOB_VNUM_NIGHTFALL
      && check->master == ch)
      {
         send_to_char("You already have a nightfallen shadow following you.\n\r", ch);
         return;
      }
   }
   check = NULL;

   for (check = char_list; check != NULL; check = check->next)
   {
      if (check->nightfall != NULL
      && check->nightfall == ch)
      break;
   }

   if (check != NULL)
   {
      if (check == victim)
      {
         send_to_char("You are already working on the fall of their soul.\n\r", ch);
         return;
      }
      act("You stop forging $N's soul in the nightfall.\n\r", ch, NULL, check, TO_CHAR);
      check->nightfall = NULL;
   }

   if (saves_spell(ch, level, victim, DAM_OTHER, SAVE_MALEDICT))
   {
      act("You try to bind your will to $N's soul and twist it to your desires, but fail.", ch, NULL, victim, TO_CHAR);
      send_to_char("You feel a horrible presence brush against your soul.\n\r", victim);
      return;
   }

   act("You bind your will to $N's soul and begin twisting it to your desires.", ch, NULL, victim, TO_CHAR);
   send_to_char("You feel a deathly presence enshrouding your soul.\n\r", victim);
   victim->nightfall = ch;

   return;
}


/*
Causes a target's hp to regenerate at a constant rate, plus removes the
effects of a wither prevent healing spell
*/
void spell_regeneration(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;

   if (is_affected(victim, gsn_atrophy))
   {
      act("$n's body stops wasting away.", victim, NULL, NULL, TO_ROOM);
      send_to_char("Your body stops wasting away.\n\r", victim);
      affect_strip(victim, gsn_atrophy);
      return;
   }

   if (is_affected(victim, gsn_prevent_healing))
   {
      act("$n's sickly looking complexion clears up.", victim, NULL, NULL, TO_ROOM);
      send_to_char("You no longer feel so sick and weary.\n\r", victim);
      affect_strip(victim, gsn_prevent_healing);
      return;
   }

   if (is_affected(victim, gsn_wither))
   {
      if (victim != ch)
      {
         act("$N's emanciated body heals up with your touch.", ch, NULL, victim, TO_CHAR);
         act("Your emanciated body heals up with $n's touch.", ch, NULL, victim, TO_VICT);
         act("$N's emanciated body heals up with $n's touch.", ch, NULL, victim, TO_NOTVICT);
      }
      else
      {
         send_to_char("Your emaciated body parts heal up.\n\r", ch);
         act("$n's emaciated body heals up.", ch, NULL, NULL, TO_ROOM);
      }
      affect_strip(victim, gsn_wither);
      return;
   }

   if (is_affected(victim, sn))
   {
      if (victim == ch)
      send_to_char("You are already healing at an improved rate.\n\r", ch);
      else
      act("$E is already healing at an improved rate.", ch, NULL, victim, TO_CHAR);
      /*        send_to_char("They are already healing at an improved rate.\n\r", ch); */
      return;
   }
   af.where = TO_AFFECTS;
   af.type = sn;
   af.location = APPLY_REGENERATION;
   af.modifier = level/11;
   af.level = level;
   af.bitvector = 0;
   af.duration = level/5*race_adjust(ch)/25.0;
   affect_to_char(victim, &af);

   send_to_char("You feel your body warm with an inner health.\n\r", victim);
   if (victim != ch)
   send_to_char("You boost their recuperation ability.\n\r", ch);

   return;
}

/*
Restores forget, wither, prevent healing, atrophy.
Also restores lost levels due to energy drain by powerful undead
*/

void spell_restoration(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   int sn_forget, sn_wither, sn_drain, sn_prevent_healing;
   bool success = FALSE;

   level *= race_adjust(ch)/25.0;

   sn_prevent_healing = gsn_prevent_healing;
   sn_forget = gsn_forget;
   sn_wither = gsn_wither;
   sn_drain = gsn_undead_drain;

   if (is_affected(victim, sn_forget))
   {
      send_to_char("You feel your mind becoming clearer.\n\r", victim);
      act("$n looks less confused.", victim, NULL, NULL, TO_ROOM);
      affect_strip(victim, sn_forget);
      success = TRUE;
   }
   if (is_affected(victim, sn_wither))
   {
      send_to_char("Your emaciated body is restored.\n\r", victim);
      act("$n's emaciated body looks healthier.", victim, NULL, NULL, TO_ROOM);
      affect_strip(victim, sn_wither);
      success = TRUE;
   }
   if (is_affected(victim, sn_prevent_healing))
   {
      send_to_char("You no longer feel so sickly and weary.\n\r", victim);
      act("$n's sickly complexion clears up.", victim, NULL, NULL, TO_ROOM);
      affect_strip(victim, sn_prevent_healing);
      success = TRUE;
   }

   if (is_affected(victim, gsn_atrophy))
   {
      act("$n's body stops wasting away.", victim, NULL, NULL, TO_ROOM);
      send_to_char("Your body stops wasting away.\n\r", victim);
      affect_strip(victim, gsn_atrophy);
   }

   if (is_affected(victim, sn_drain)
   && check_dispel(level + 15, victim, sn_drain))
   {
      act("$n looks much better.", victim, NULL, NULL, TO_ROOM);
      affect_strip(victim, sn_drain);
      success = TRUE;
   }

   if (!success)
   send_to_char("Spell had no effect.\n\r", ch);

   return;
}

void spell_undead_drain(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA *paf_old, af;
   OBJ_DATA *obj;

   int dam;
   if (victim == ch)
   {
      send_to_char("You can't drain your own lifeforce.\n\r", ch);
      return;
   }

   if ( saves_spell(ch, level + 4, victim, DAM_NEGATIVE, SAVE_OTHER)
   || saves_armor_of_god(ch, level, victim) )
   return;

   if (IS_SET(victim->imm_flags, IMM_NEGATIVE))
   {
      act("$N is unaffected by $n's draining.", ch, NULL, victim, TO_NOTVICT);
      act("You are unaffected by $n's draining.", ch, NULL, victim, TO_VICT);
      act("$N is unaffected by your draining.", ch, NULL, victim, TO_CHAR);
      return;
   }
   if ((obj = get_eq_char(victim, WEAR_BODY)) != NULL
   && obj->pIndexData->vnum == OBJ_VNUM_ARMOR_UNDEAD)
   {
      act("$N's Armor of the Undead Knights absorbs $n's draining.", ch, NULL, victim, TO_NOTVICT);
      act("Your Armor of the Undead Knights absorbs $n's draining.", ch, NULL, victim, TO_VICT);
      act("$N's Armor of the Undead Knights absorbs your draining.", ch, NULL, victim, TO_CHAR);
      return;
   }

   if ( GET_LEVEL(victim) <= 1)
   {
      act("$n's draining begins to crumble $N!", ch, NULL, victim, TO_NOTVICT);
      act("$n's draining begins to destroy you!", ch, NULL, victim, TO_VICT);
      act("Your draining begins to destroy $N!", ch, NULL, victim, TO_CHAR);

      dam = dice(level, 15);
      if (saves_spell(ch, level, victim, DAM_NEGATIVE, SAVE_OTHER))
      dam /= 2;

      damage(ch, victim, dam, sn, DAM_NEGATIVE, TRUE);
      return;
   }

   af.where = TO_AFFECTS;
   af.type = sn;
   af.level = level;
   af.bitvector = 0;
   af.duration = UMIN(level, 48);
   af.location = APPLY_LEVEL;
   af.modifier = UMAX(-(number_range(1, level/18)), -25);

   for ( paf_old = victim->affected; paf_old != NULL; paf_old = paf_old->next )
   {
      if ( paf_old->type == af.type
      && paf_old->location == af.location )
      {
         af.level = (af.level += paf_old->level) / 2;
         af.duration += paf_old->duration;
         if (af.duration > 48)
         af.duration = 48;
         af.modifier += paf_old->modifier;
         if (af.modifier < -25 )
         af.modifier = -25;

         affect_remove( victim, paf_old );
         break;
      }
   }

   affect_to_char( victim, &af );

   send_to_char("You feel drained of life!\n\r", victim);
   act("$n turns ghastly white for a moment.", victim, NULL, NULL, TO_ROOM);

   return;
}



void spell_prevent_healing(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;

   if (is_affected(victim, sn))
   {
      send_to_char("Their body is already deficient in healing ability.\n\r", ch);
      return;
   }

   if (saves_spell(ch, level, victim, DAM_OTHER, SAVE_MALEDICT)
   || victim->race == grn_troll)
   {
      send_to_char("You failed.\n\r", ch);
      return;
   }
   send_to_char("You feel your body losing its ability to heal.\n\r", victim);
   act("$n looks very sick.", victim, NULL, NULL, TO_ROOM);
   af.where = TO_AFFECTS;
   af.type = gsn_prevent_healing;
   af.level = level;
   af.location = 0;
   af.modifier = 0;
   af.duration = level/6;
   af.bitvector = 0;
   affect_to_char(victim, &af);

   return;
}

/* Fire and Ice spell for elementalists */
void spell_fire_and_ice(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   int dam, sn_frost, sn_fire, d_type, sn_type;
   int vict_id;
   int ch_id;
   int ch_ghost;
   int vict_ghost;
   ROOM_INDEX_DATA* room;


   if (check_peace(ch)) return;

   sn_frost = gsn_frost_breath;
   sn_fire = gsn_fire_breath;
   if (sn_fire == -1 || sn_frost == -1)
   {
      send_to_char("The elements fail to combine.\n\r", ch);
      return;
   }

   if (number_percent() > 50)
   {
      sn_type = sn_frost;
      d_type = DAM_COLD;
   }
   else
   {
      sn_type = sn_fire;
      d_type = DAM_FIRE;
   }
   if (check_spellcraft(ch, sn))
   dam = spellcraft_dam(level, 4);
   else
   dam = dice(level, 4);
   act("$n unleashes a blast of fire and ice!", ch, NULL, NULL, TO_ROOM);
   send_to_char("You unleash a blast of fire and ice!\n\r", ch);

   if (saves_spell(ch, level, victim, d_type, SAVE_SPELL))
   dam /= 2;

   vict_id = victim->id;
   ch_id = ch->id;
   ch_ghost = ch->ghost;
   vict_ghost = victim->ghost;
   room = ch->in_room;
   damage(ch, victim, dam*race_adjust(ch)/25.0, sn_type, d_type, TRUE);
   if
   (
      victim->id != vict_id ||
      ch->id != ch_id ||
      ch->ghost != ch_ghost ||
      victim->ghost != vict_ghost ||
      ch->in_room != room ||
      victim->in_room != room
   )
   {
      return;
   }


   if (check_spellcraft(ch, sn))
   dam = spellcraft_dam(level, 3);
   else
   dam = dice(level, 3);

   if (d_type == DAM_COLD)
   {
      d_type = DAM_FIRE;
      sn_type = sn_fire;
   }
   else
   {
      d_type = DAM_COLD;
      sn_type = sn_frost;
   }
   damage(ch, victim, dam*race_adjust(ch)/25.0, sn_type, d_type, TRUE);
   return;

}

void spell_atrophy(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;

   if (IS_GOOD(ch))
   {
      if (is_supplicating)
      {
         act
         (
            "$z frowns upon your attempt to ask for this affliction.",
            ch,
            NULL,
            NULL,
            TO_CHAR
         );
      }
      else
      {
         send_to_char
         (
            "Your God frowns upon your attempt to use this magic!\n\r",
            ch
         );
      }
      return;
   }

   if (is_affected(victim, sn))
   {
      if (victim == ch)
      send_to_char("You are already wasting away.\n\r", ch);
      else
      act("$E is already wasting away.", ch, NULL, victim, TO_CHAR);
      /*            send_to_char("They are already wasting away.\n\r", ch); */
      return;
   }

   if (saves_spell(ch, level, victim, DAM_OTHER, SAVE_MALEDICT)
   || victim->race == grn_troll)
   {
      send_to_char("You failed.\n\r", ch);
      return;
   }
   af.where = TO_AFFECTS;
   af.type = sn;
   af.duration = level/7;
   af.location = APPLY_REGENERATION;
   af.modifier = number_range(-2,-level/12);
   af.bitvector = 0;
   af.level = level;
   affect_to_char(victim, &af);

   send_to_char("You feel an intense pain as your body begins to waste away.\n\r", victim);
   act("$n looks very sick as $s body starts wasting away before your eyes!", victim, NULL, NULL, TO_ROOM);
   return;
}

/*
Heal spell, but also for only slightly more cost can cure poison and disease.
Can stop wasting, but no heal benefit is gained if used this way. Won't
restore undead_drains or wither etc.
*/
void spell_utter_heal(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   int sn_atrophy, sn_poison;
   int heal;

   if (is_affected(victim, gsn_corrupt("smite", &gsn_smite)))
   {
      send_to_char("You feel better, but the wounds do not close.\n\r", victim);
      return;
   }

   if (IS_EVIL(ch))
   {
      send_to_char("The Gods do not do such for clerics of darkness.\n\r", ch);
      return;
   }
   sn_atrophy = gsn_atrophy;
   sn_poison = gsn_poison;
   if (is_affected(victim, sn_atrophy))
   {
      affect_strip(victim, sn_atrophy);
      send_to_char("Your body stops wasting away.\n\r", victim);
      act("$n's body stops wasting away.", victim, NULL, NULL, TO_ROOM);
      return;
   }
   if (is_affected(ch, sn_poison)
   || is_affected(ch, gsn_poison_dust))
   {
      act("$n looks better.", victim, NULL, NULL, TO_ROOM);
      send_to_char("You feel a warm sensation running through you.\n\r", victim);
      affect_strip(victim, sn_poison);
      affect_strip(victim, gsn_poison_dust);
   }
   if (is_affected(ch, gsn_plague))
   {
      act("The sores on $n's body disappear.", victim, NULL, NULL, TO_ROOM);
      send_to_char("The sores on your body disappear.\n\r", victim);
      affect_strip(victim, gsn_plague);
   }
   send_to_char("You feel better!\n\r", victim);

   if (is_affected(victim, gsn_cunning_strike) && number_percent() < 30)
   {
      AFFECT_DATA *paf;
      for (paf = victim->affected; paf != NULL; paf = paf->next)
      {
         if (paf->type == gsn_cunning_strike)
         {
            break;
         }
      }
      if (paf->location == APPLY_NONE)
         {
            send_to_char("Your wound clots and stops bleeding.\n\r", victim);
         }
      else if (paf->location == APPLY_MOVE)
         {
            send_to_char("You feel that your bones have knitted back together.\n\r", victim);
         }
      else
         {
            send_to_char("The nerves in your body have recovered and function properly again.\n\r", victim);
         }
      if (victim != ch)
         {
	         act("$N's wounds knit under your administration.", ch, NULL, victim, TO_CHAR);
         }
      affect_strip(victim, gsn_cunning_strike);
   }


   if ( is_affected(victim, gsn_black_mantle))
   {
      if (is_supplicating)
      {
         send_to_char
         (
            "The black mantle prevents your divine healing.\n\r",
            ch
         );
      }
      else
      {
         send_to_char
         (
            "The black mantle absorbs your healing magics.\n\r",
            ch
         );
      }
      if (ch != victim)
      {
         if (is_supplicating)
         {
            send_to_char
            (
               "The black mantle prevents the divine healing.\n\r",
               victim
            );
         }
         else
         {
            send_to_char
            (
               "The black mantle absorbs the healing magics.\n\r",
               victim
            );
         }
      }
      return;
   }

   heal = 100;
   if
   (
      is_affected(victim, gsn_ancient_plague) ||
      is_affected(victim, gsn_mantle_oblivion) ||
      IS_SET(victim->act2, PLR_LICH)
   )
   heal = heal/2;

   if (!IS_NPC(ch) && ch->pcdata->special == SUBCLASS_PRIEST_HEALING)
   heal = heal + heal/2;
   victim->hit = UMIN(victim->hit + heal, victim->max_hit);

   if (victim != ch)
   {
      act
      (
         "You grant them healing from $z.",
         ch,
         NULL,
         NULL,
         TO_CHAR
      );
      evaluate_wounds(ch, victim);
   }

   return;
}

void do_steed( CHAR_DATA *ch, char *argument )
{
   CHAR_DATA *mob, *check;
   AFFECT_DATA af;
   int chance;
   MOB_INDEX_DATA *steed;

   if ((chance = get_skill(ch, gsn_summon_steed)) <= 0
   || !has_skill(ch, gsn_summon_steed) )
   {
      send_to_char("You don't have access to a good stable.\n\r", ch);
      return;
   }

   if (house_down(ch, HOUSE_VALOR))
   return;

   if (is_affected(ch, gsn_summon_steed)){
      send_to_char("You can't call another steed yet.\n\r", ch);
      return;
   };

   if (IS_NPC(ch))
   return;

   if ( ch->is_riding )
   {
      send_to_char("You are already riding something!\n\r", ch);
      return;
   }

   if (ch->mana < 50)
   {
      send_to_char("You do not have enough mana.\n\r", ch);
      return;
   }

   for (check = char_list; check != NULL; check = check->next)
   {
      if (IS_NPC(check))
      if ( ch == check->master
      && (check->pIndexData->vnum == MOB_VNUM_STEED
      || check->pIndexData->vnum == MOB_VNUM_STEED_PEGASUS) )
      {
         send_to_char("You already have a steed under your command.\n\r", ch);
         return;
      }
   }


   if (number_percent() > chance)
   {
      send_to_char("Nothing answers your call.\n\r", ch);
      check_improve(ch, gsn_summon_steed, FALSE, 3);
      ch->mana -= 25;
      return;
   }

   ch->mana -= 50;
   check_improve(ch, gsn_summon_steed, TRUE, 3);

   if ( ch->level < 38 )
   {
      if ( (steed = get_mob_index(MOB_VNUM_STEED)) == NULL
      || (mob = create_mobile(steed)) == NULL )
      {
         send_to_char("Nothing happens.\n\r", ch);
         return;
      }
      else
      {
         mob->max_hit = ch->level * 10;
         /*            mob->damroll = ch->level/10; */
      }
   } else
   {
      if ( (steed = get_mob_index(MOB_VNUM_STEED_PEGASUS)) == NULL
      || (mob = create_mobile(steed)) == NULL )
      {
         if ( (steed = get_mob_index(MOB_VNUM_STEED)) == NULL
         || (mob = create_mobile(steed)) == NULL )
         {
            send_to_char("Nothing happens.\n\r", ch);
            return;
         }
         else
         {
            mob->max_hit = ch->level * 10;
            /*                mob->damroll = ch->level/10; */
         }
      } else
      {
         mob->max_hit = ch->level * 12;
         /*            mob->damroll = ch->level/8; */
      }
   }

   af.where = TO_AFFECTS;
   af.level = ch->level;
   af.location = 0;
   af.modifier = 0;
   af.duration = 6;
   af.bitvector = 0;
   af.type = gsn_summon_steed;
   affect_to_char(ch, &af);

   char_to_room(mob, ch->in_room);

   mob->damroll += ch->level/2;
   mob->leader = ch;
   mob->master = ch;
   mob->level = ch->level;
   mob->max_move = ch->level * 20;
   mob->move = mob->max_move;
   mob->max_hit = ch->level*16 + number_range(-150, 150);
   mob->hit = mob->max_hit;
   mob->armor[0] = -ch->level*2;
   mob->armor[1] = -ch->level*2;
   mob->armor[2] = -ch->level*2;
   mob->armor[3] = -ch->level*1;
   SET_BIT(mob->affected_by, AFF_CHARM);

   send_to_char("A pure white steed answers your call!\n\r", ch);
   act("$n gallantly trots into the room!", mob, NULL, NULL, TO_ROOM);
   do_mount(ch, "steed");

   return;
}

void spell_call_slayer(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *mob;
   CHAR_DATA *check;
   int gcount = 0;

   mob = create_mobile(get_mob_index(MOB_VNUM_SLAYER));
   if (mob == NULL)
   {
      send_to_char("Nothing happens.\n\r", ch);
      return;
   }

   for (check = char_list; check != NULL; check = check->next)
   {
      if (IS_NPC(check))
      if ( (check->master == ch) && (check->pIndexData->vnum == MOB_VNUM_SLAYER) )
      {
         gcount += 1;
      }
   }

   if ( gcount >= 3)
   {
      send_to_char("You already control as many slayers as possible.\n\r", ch);
      return;
   }

   char_to_room(mob, ch->in_room);
   mob->leader = ch;
   mob->master = ch;
   mob->level = level;
   mob->max_hit = level * level;
   mob->hit = mob->max_hit;
   mob->damroll = level/2;
   mob->armor[0] = -level*5;
   mob->armor[1] = -level*5;
   mob->armor[2] = -level*5;
   mob->armor[3] = -level*3;

   SET_BIT(mob->affected_by, AFF_CHARM);
   act("A gate opens up and $n steps forth.", mob, NULL, NULL, TO_ROOM);
   return;
}

void spell_judgement(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{

   CHAR_DATA *victim;
   char buf[MAX_STRING_LENGTH];

   if (house_down(ch, HOUSE_ENFORCER))
   return;

   if (target_name[0] == '\0')
   {
      send_to_char("Judge who?\n\r", ch);
      return;
   }

   victim = get_char_world(ch, target_name);
   if (victim == NULL)
   {
      send_to_char("You can't find anyone of that name.\n\r", ch);
      return;
   }

   if (IS_NPC(victim))
   {
      send_to_char("You can't tell anything about them.\n\r", ch);
      return;
   }

   if ((((int) (victim->played + current_time - victim->logon) / 3600) <30))
   act("$E is too young to be considered a witness.", ch, NULL, victim, TO_CHAR);
   /*    send_to_char("They are too young to be considered a witness.\n\r", ch); */

   if (victim->pcdata->wanteds == 0)
   act("$E does not have a criminal record.", ch, NULL, victim, TO_CHAR);
   /*    send_to_char("They do not have a criminal record.\n\r", ch); */
   else
   {
      sprintf(buf, "$M's record stands at %d.\n\r", victim->pcdata->wanteds);
      act(buf, ch, NULL, victim, TO_CHAR);
   }
   return;
}

void spell_bloodmist( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;
   AFFECT_DATA* paf;
   int dam;

   if (house_down(ch, HOUSE_ANCIENT))
   return;

   if (is_affected(victim, gsn_bloodmist))
   {
      if (victim == ch)
      send_to_char("You are already hemmoraging uncontrollably.\n\r", ch);
      else
      act("$N is already hemmoraging uncontrollably.", ch, NULL, victim, TO_CHAR);
      return;
   }

   if (saves_spell(ch, level, victim, DAM_HARM, SAVE_SPELL))
   {
      send_to_char("Your eyes cloud with red, but it passes.", victim);
      act("$n swoons in pain, but it passes.", victim, NULL, NULL, TO_ROOM);
      ch->mana-=25;
      return;
   }

   if (IS_NPC(victim) && IS_SET(victim->act, ACT_UNDEAD))
   {
      act("$n grins at your feeble attempt.", victim, NULL, ch, TO_CHAR);
      ch->mana-=25;
      return;
   }

   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.duration  = level/15;
   af.modifier  = -level/10;
   af.location  = APPLY_HITROLL;
   af.bitvector = AFF_BLIND;
   affect_to_char( victim, &af );
   paf = affect_find(victim->affected, sn);
   if (paf != NULL)
   {
      free_string(paf->caster);
      paf->caster = str_dup(ch->name);
   }
   send_to_char( "You scream in agony as blood streams from your every pore!\n\r", victim );
   act("$n screams as blood streams from every pore!", victim, NULL, NULL, TO_ROOM);

   dam = dice(level, 4);
   damage( ch, victim, dam, sn, DAM_HARM, TRUE);

   return;
}

void spell_darkforge(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   OBJ_DATA *obj = (OBJ_DATA *) vo;
   OBJ_DATA *gauntlet;
   int bonus, dice, number, fail, result;
   char buf[MAX_STRING_LENGTH];
   AFFECT_DATA af;

   if (house_down(ch, HOUSE_ANCIENT))
   return;

   if (!is_affected(ch, gsn_cloak_form))
   {
      send_to_char("You cannot work with the Shadow without your cloak.\n\r", ch);
      return;
   }

   if (is_affected(ch, gsn_darkforge))
   {
      send_to_char("The Shadow is still gathering for your next summons.\n\r", ch);
      return;
   }

   if (ch->class == (GUILD_MONK-1)){
      /* monks darkforge their fists -wervdon */
      af.where     = TO_AFFECTS;
      af.type      = gsn_darkforge_fists;
      af.level     = level;
      af.duration  = 12;
      af.modifier  = 0;
      af.location  = 0;
      af.bitvector = 0;
      affect_to_char( ch, &af );
      send_to_char("Your fists are forged with the shadows.\n\r", ch);
      return;
   }

   if ((obj->item_type != ITEM_WEAPON)
   || ((obj->value[0] != WEAPON_DAGGER)
   && (obj->value[0] != WEAPON_WHIP)
   && (obj->value[0] != WEAPON_MACE)
   && (obj->value[0] != WEAPON_SWORD)
   && (obj->value[0] != WEAPON_STAFF)))
   {
      send_to_char("The Shadow will only reforge daggers, whips, maces, swords, and staffs.\n\r", ch);
      return;
   }

   fail = 275;
   fail -= IS_NPC(ch) ? 100 : ch->pcdata->learned[sn];
   fail -= 3 * ch->level;
   if (IS_SET(obj->extra_flags, ITEM_BLESS))
   fail += 100;
   if (IS_SET(obj->extra_flags, ITEM_ANTI_EVIL))
   fail += 200;
   if (IS_SET(obj->extra_flags, ITEM_EVIL))
   fail -= 20;
   if (IS_SET(obj->extra_flags, ITEM_DARK))
   fail -= 5;
   if (IS_SET(obj->extra_flags, ITEM_ANTI_GOOD))
   fail -= 10;
   fail -= ( (obj->value[2] + 1) * obj->value[1]/2);
   result = number_percent();

   if (result < (fail/3))
   {
      act("$p is infused with darkness, becomes brittle, and shatters!", ch, obj, NULL, TO_ROOM);
      act("$p is infused with darkness, becomes brittle, and shatters!", ch, obj, NULL, TO_CHAR);
      extract_obj(obj, FALSE);
      return;
   }
   if (result < fail)
   {
      send_to_char("Nothing seemed to happen.\n\r", ch);
      return;
   }
   switch (obj->value[0])
   {
      case WEAPON_DAGGER:
      act("The darkness engulfs your hand to form a dagger radiating with evil.", ch, obj, NULL, TO_CHAR);
      break;
      case WEAPON_WHIP:
      act("The darkness engulfs your hand to form a whip radiating with evil.", ch, obj, NULL, TO_CHAR);
      break;
      case WEAPON_MACE:
      act("The darkness engulfs your hand to form a mace radiating with evil.", ch, obj, NULL, TO_CHAR);
      break;
      case WEAPON_SWORD:
      act("The darkness engulfs your hand to form a sword radiating with evil.", ch, obj, NULL, TO_CHAR);
      break;
      case WEAPON_STAFF:
      act("The darkness engulfs your hand to form a staff radiating with evil.", ch, obj, NULL, TO_CHAR);
      break;
      default:
      act("The darkness engulfs your hand to form a blade radiating with evil.", ch, obj, NULL, TO_CHAR);
   }
   act("The surrounding darkness engulfs $n and dissipates.", ch, obj, NULL, TO_ROOM);

   number=8;
   dice=4;
   if ((level>=25) && (level<35))
   {
      number=9;
      dice=4;
   }
   if ((level>=35) && (level<45))
   {
      number=10;
      dice=4;
   }
   if (level>=45)
   {
      number=13;
      dice=3;
   }
   bonus = UMIN(ch->level/8, 6);

   gauntlet = create_object(get_obj_index(OBJ_VNUM_ANCIENT_FORGE), 0);
   gauntlet->value[0] = obj->value[0];
   gauntlet->value[1] = number;
   gauntlet->value[2] = dice;
   af.where = TO_OBJECT;
   af.type = sn;
   af.level = level;
   af.duration = -1;
   af.bitvector = 0;
   af.location = APPLY_HITROLL;
   af.modifier = bonus;
   affect_to_obj(gauntlet, &af);
   af.location = APPLY_DAMROLL;
   affect_to_obj(gauntlet, &af);
   gauntlet->level = level;
   gauntlet->timer = -1;
   gauntlet->weight = obj->weight;

   switch (obj->value[0])
   {
      case WEAPON_DAGGER:
      sprintf(buf, "a vile dagger surrounded by shadows"); break;
      case WEAPON_WHIP:
      sprintf(buf, "a vile whip surrounded by shadows"); break;
      case WEAPON_MACE:
      sprintf(buf, "a vile mace surrounded by shadows"); break;
      case WEAPON_SWORD:
      sprintf(buf, "a vile sword surrounded by shadows"); break;
      case WEAPON_STAFF:
      sprintf(buf, "a vile staff surrounded by shadows"); break;
      default:
      sprintf(buf, "a vile blade surrounded by shadows");
   }
   free_string(gauntlet->short_descr);
   gauntlet->short_descr = str_dup(buf);

   switch (obj->value[0])
   {
      case WEAPON_DAGGER:
      sprintf(buf, "vile dagger shadow"); break;
      case WEAPON_WHIP:
      sprintf(buf, "vile whip shadow"); break;
      case WEAPON_MACE:
      sprintf(buf, "vile mace shadow"); break;
      case WEAPON_SWORD:
      sprintf(buf, "vile sword shadow"); break;
      case WEAPON_STAFF:
      sprintf(buf, "vile staff shadow"); break;
      default:
      sprintf(buf, "vile blade shadow"); break;
   }
   free_string( gauntlet->name );
   gauntlet->name = str_dup( buf );

   sprintf(buf, "%s", ch->name);
   free_string(gauntlet->owner);
   gauntlet->owner = str_dup(buf);

   obj_to_char(gauntlet, ch);
   extract_obj(obj, FALSE);

   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.duration  = 12;
   af.modifier  = 0;
   af.location  = 0;
   af.bitvector = 0;
   affect_to_char( ch, &af );

   return;
}

void spell_soulcraft(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   OBJ_DATA *gauntlet;
   int bonus, dice, number, dam;
   char buf[MAX_STRING_LENGTH];
   AFFECT_DATA af;

   if (house_down(ch, HOUSE_ANCIENT))
   return;

   if (is_affected(ch, gsn_soulcraft))
   {
      send_to_char("You are not up to forging your heart again.\n\r", ch);
      return;
   }


   dice=0;
   number=0;
   if ((level>24) && (level<35))
   {
      number=10;
      dice=4;
   }
   if ((level>34) && (level<45))
   {
      number=11;
      dice=4;
   }
   if (level>44)
   {
      number=12;
      dice=4;
   }

   gauntlet = create_object(get_obj_index(OBJ_VNUM_ANCIENT_GAUNTLET), 0);


   bonus = ch->level/6;
   if (ch->level == 51)
   bonus=9;

   af.where = TO_OBJECT;
   af.type = sn;
   af.level = level;
   af.duration = -1;
   af.location = APPLY_HITROLL;
   af.modifier = bonus;
   af.bitvector = 0;
   affect_to_obj(gauntlet, &af);
   af.location = APPLY_DAMROLL;
   affect_to_obj(gauntlet, &af);
   af.location = APPLY_HIT;
   af.modifier = level*3;
   affect_to_obj(gauntlet, &af);
   gauntlet->level = level;
   gauntlet->timer = 24;
   gauntlet->value[1] = number;
   gauntlet->value[2] = dice;
   sprintf(buf, "%s", ch->name);
   free_string(gauntlet->owner);
   gauntlet->owner = str_dup(buf);
   obj_to_char(gauntlet, ch);
   act("$n reaches into $s own chest and grins.", ch, NULL, ch, TO_ROOM);
   send_to_char("You reach into your chest and forge your heart into a vile weapon.\n\r", ch);

   dam=number_range(75, 150);
   damage( ch, ch, dam, sn, DAM_HARM, TRUE);

   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.duration  = 24;
   af.modifier  = 0;
   af.location  = 0;
   af.bitvector = 0;
   affect_to_char( ch, &af );

   return;
}

void spell_taint(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   OBJ_DATA *obj = (OBJ_DATA *) vo;
   AFFECT_DATA af;
   int success;
   int check;
   char buf[MAX_STRING_LENGTH];

   if (house_down(ch, HOUSE_ANCIENT))
   return;

   if (obj->wear_loc != -1)
   {
      send_to_char("The item must be in your inventory.\n\r", ch);
      return;
   }

   if (is_affected(ch, gsn_taint))
   {
      send_to_char("You must regain your strength before tainting another object.\n\r", ch);
      return;
   }


   if (!obj_can_change_align_restrictions(obj))
   {
      send_to_char("This object is beyond your ability to corrupt.\n\r", ch);
      return;
   }

   check = number_percent();

   success = IS_NPC(ch) ? 100-(obj->level/2) : ch->pcdata->learned[sn]-(obj->level/2);
   if (IS_OBJ_STAT(obj, ITEM_BLESS))
   success-=10;
   if (IS_OBJ_STAT(obj, ITEM_EVIL))
   success+=10;
   if (check>success)
   {
      send_to_char("The object darkens and crumbles into nothingness.\n\r", ch);
      act("$n grasps $p, and it crumbles into nothingness.\n\r", ch, obj, NULL, TO_ROOM);
      extract_obj(obj, FALSE);
      return;
   }
   send_to_char("You grasp the object, and infuse it with the power of Ancient.\n\r", ch);
   act("$n grasps $p, and infuses it with the power of Ancient.", ch, obj, NULL, TO_ROOM);
   REMOVE_BIT(obj->extra_flags, ITEM_BLESS);
   REMOVE_BIT(obj->extra_flags, ITEM_GLOW);
   REMOVE_BIT(obj->extra_flags, ITEM_ANTI_EVIL);
   REMOVE_BIT(obj->extra_flags, ITEM_ANTI_NEUTRAL);
   SET_BIT(obj->extra_flags, ITEM_DARK);
   SET_BIT(obj->extra_flags, ITEM_EVIL);
   if (obj->pIndexData->condition >= 0)
   SET_BIT(obj->extra_flags, ITEM_ROT_DEATH);
   sprintf(buf, "%s", ch->name);
   free_string(obj->owner);
   obj->owner = str_dup(buf);

   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.duration  = 24;
   af.modifier  = 0;
   af.location  = 0;
   af.bitvector = 0;
   affect_to_char( ch, &af );

   return;
}

/* Knight of Famine Starvation and Dehydration  -Werv */

void spell_cause_starvation(int sn, int level, CHAR_DATA *ch, void *vo, int target) {
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   int dam=0, leftover=0;

   level *= race_adjust(ch)/25.0;

   if ( saves_spell(ch, level, victim, DAM_OTHER, SAVE_MALEDICT)
   || IS_IMMORTAL(victim) )
   {
      act("$N seems unaffected.", ch, NULL, victim, TO_CHAR);
      act("Your stomach lurches for a moment.", ch, NULL, victim, TO_VICT);
      return;
   }

   /* At L51, 16-48 pts = 3 to 1 hits; L20:12-24:4-2 */
   /* Using linear math, that comes out to: */
   dam = number_range((4.0/31)*(level-20)+12, (24.0/31)*(level-20)+24);

   act("$n looks uncomfortable and a bit queasy.", victim, NULL, NULL, TO_ROOM);
   act("Your stomach lurches as a sudden hunger sets in.", ch, NULL, victim, TO_VICT);

   if (IS_NPC(victim))
   {
      damage(victim, victim, dam, gsn_starvation, DAM_OTHER, TRUE);
   }
   else
   {
      leftover = dam - victim->pcdata->condition[COND_HUNGER];
      dam = UMAX(dam, victim->pcdata->condition[COND_HUNGER]);
      gain_condition(victim, COND_HUNGER, -dam);
      if (victim->pcdata->condition[COND_HUNGER] == 0)
      {
         gain_condition(victim, COND_STARVING, leftover / 8 + 1);
         gain_condition(victim, COND_HUNGER, -1);
      }
      else
      {
         damage(victim, victim, leftover / 16, gsn_starvation, DAM_OTHER, TRUE);
      }
   }

   return;
}

void spell_cause_dehydration(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   int dam=0, leftover=0;

   level *= race_adjust(ch)/25.0;

   if (
         saves_spell(ch, level, victim, DAM_OTHER, SAVE_MALEDICT) ||
         is_branded_by_lestregus(victim) ||
         IS_IMMORTAL(victim)
      )
   {
      act("$N seems unaffected.", ch, NULL, victim, TO_CHAR);
      act("Your skin begins to feel dry, but it passes.", ch, NULL, victim, TO_VICT);
      return;
   }

   /* At L51, 16-48 pts = 3 to 1 hits; L20:12-24:4-2 */
   /* Using linear math, that comes out to: */
   dam = number_range((4.0/31)*(level-20)+12, (24.0/31)*(level-20)+24);

   act("$n looks uncomfortable and a bit queasy.", victim, NULL, NULL, TO_ROOM);
   act("Your skin begins to feel dry, and your mouth parches.", ch, NULL, victim, TO_VICT);

   if (IS_NPC(victim))
   {
      damage(victim, victim, dam, gsn_dehydrated, DAM_OTHER, TRUE);
   }
   else
   {
      leftover = dam - victim->pcdata->condition[COND_THIRST];
      dam = UMAX(dam, victim->pcdata->condition[COND_THIRST]);
      gain_condition(victim, COND_THIRST, -dam);
      if (victim->pcdata->condition[COND_THIRST] == 0)
      {
         gain_condition(victim, COND_DEHYDRATED, leftover / 8);
         gain_condition(victim, COND_THIRST, -1);
      }
      else
      {
         damage(victim, victim, leftover / 16, gsn_dehydrated, DAM_OTHER, TRUE);
      }
   }

   return;
}

/* Spores - Werv */
void spell_spores(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *spore;
   int chance;

   chance = get_skill(ch, sn);

   if ( number_percent() > chance){
      send_to_char("You attempt to create a spore but fail.\n\r", ch);
      return;
   }

   spore = create_mobile(get_mob_index(MOB_VNUM_SPORE));
   spore->level = level;
   spore->pause = level;
   char_to_room(spore, ch->in_room);
   act("$n suddenly begins to grow here!", spore, NULL, NULL, TO_ROOM);
   return;
}

/* Black Mantle */
void spell_nether_shroud(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA*) vo;
   AFFECT_DATA af;

   if (is_affected(victim, gsn_nether_shroud))
   {
      act("$E is already surrounded by a shroud of negative energy.", ch, NULL, victim, TO_CHAR);
      /*    send_to_char("They are already surrounded by a shroud of negative energy.\n\r", ch); */
      return;
   }

   if (IS_GOOD(victim))
   if (saves_spell(ch, level, victim, DAM_NEGATIVE, SAVE_MALEDICT))
   {
      send_to_char("Your victim shrugs off the shroud of negative energy!\n\r", ch);
      return;
   }

   af.where = TO_AFFECTS;
   af.level = level;
   af.location = 0;
   af.modifier = 0;
   af.bitvector = 0;
   af.type = gsn_nether_shroud;
   af.duration = level/5;
   affect_to_char(victim, &af);

   act("$n begins to glow with negative energy!", victim, NULL, NULL, TO_ROOM);
   if (IS_EVIL(victim))
   send_to_char("A field of negative energy surrounds you, and you feel invigorated!\n\r", victim);
   else
   send_to_char("You feel weakened as a field of negative energy surrounds you!\n\r", victim);
   return;
}

void spell_black_mantle(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA*) vo;
   AFFECT_DATA af;

   if (is_affected(victim, gsn_black_mantle))
   {
      act("$E is already under a mantle of death.", ch, NULL, victim, TO_CHAR);
      /*  send_to_char("They are already under the mantle of death.\n\r", ch);   */
      return;
   }

   if (!IS_NPC(ch) && !IS_IMMORTAL(ch) && !IS_IMMORTAL(victim))
   {
      if (!IS_NPC(victim) || (IS_AFFECTED(victim, AFF_CHARM) &&
      victim->master != ch))
      {
         QUIT_STATE(ch, 20);
         QUIT_STATE(victim, 20);
      }
   }

   if (saves_spell(ch, level, victim, DAM_NEGATIVE, SAVE_MALEDICT))
   {
      send_to_char("Your victim's spirit is too strong for you to overcome!\n\r", ch);
      send_to_char("You feel a momentary chill, but it subsides.\n\r", victim);
      return;
   }

   af.where = TO_AFFECTS;
   af.level = level;
   af.location = 0;
   af.modifier = 0;
   af.bitvector = 0;
   af.type = gsn_black_mantle;
   af.duration = level/4;
   affect_to_char(victim, &af);

   act("A dark cloud forms about $n's body!", victim, NULL, NULL, TO_ROOM);
   send_to_char("You feel a chill as a dark cloud surrounds you!\n\r", victim);

   return;
}


void spell_rot( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA*) vo;
   AFFECT_DATA af;
   int dam;

   dam = dice(level, 6);

   if (saves_spell(ch, level, victim, DAM_NEGATIVE, SAVE_SPELL))
   dam = dam/2;

   if (is_affected(victim, gsn_rot) || saves_spell(ch, (level-2), victim, DAM_NEGATIVE, SAVE_SPELL))
   {
      damage(ch, victim, dam*race_adjust(ch)/25.0, sn, DAM_NEGATIVE, TRUE);
      return;
   }
   act("$n's flesh begins to rot away!", victim, NULL, NULL, TO_ROOM);

   send_to_char("Your flesh begins to rot away!\n\r", victim);
   af.where = TO_AFFECTS;
   af.level = level;
   af.duration = level/10;
   af.type = gsn_rot;
   af.bitvector = 0;
   af.location = APPLY_CON;
   af.modifier = (-1)*number_range(1, level/12+1);
   affect_to_char(victim, &af);
   af.location = APPLY_STR;
   af.modifier = (-1)*number_range(1, level/10+1);
   affect_to_char(victim, &af);
   if (number_range(0, 3) == 0)
   {
      magic_spell_vict
      (
         ch,
         victim,
         CAST_BITS_REUSE,
         level,
         gsn_atrophy
      );
   }
   damage(ch, victim, dam*race_adjust(ch)/25.0, sn, DAM_NEGATIVE, TRUE);
   return;
}


/* Vermin spell -Werv */
void spell_vermin(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA*) vo;
   CHAR_DATA *vermin;
   int cnt;
   int nrats;
   char buf[MAX_STRING_LENGTH];
   AFFECT_DATA af;

   if (is_affected(ch, gsn_vermin_timer)){
      send_to_char("You can't call upon vermin yet.\n\r", ch);
      return;
   }

   if (target_name[0] == '\0')
   {
      if (ch->fighting != NULL)
      victim = ch->fighting;
      else
      {
         send_to_char("Summon vermin to attack whom?\n\r", ch);
         return;
      }
   }

   if (!IS_NPC(victim) && is_safe(ch, victim, IS_SAFE_SILENT)){
      send_to_char("The Gods frown upon this abuse of dark power.\n\r", ch);
      return;
   }


   if (number_percent() > get_skill(ch, sn)){
      sprintf(buf, "Help! %s tried to call vermin to attack me!", PERS(ch, victim));
      do_myell(victim, buf);
      multi_hit(victim, ch, TYPE_UNDEFINED);
      return;
   }
   af.where = TO_AFFECTS;
   af.level = level;
   af.location = 0;
   af.modifier = 0;
   af.bitvector = 0;
   af.type = gsn_vermin_timer;
   af.duration = 4;
   affect_to_char(ch, &af);



   sprintf(buf, "Help! I'm being attacked by vermin!");
   do_myell(victim, buf);

   nrats = level/6 + number_range(0, 4);
   for (cnt=1;cnt <= nrats;cnt++){
      vermin = create_mobile(get_mob_index(MOB_VNUM_VERMIN) );
      vermin->max_hit = level*3 + 50;
      vermin->hit = vermin->max_hit;
      vermin->level = ch->level;
      char_to_room(vermin, ch->in_room);
      act("$n arrives from the shadows and attacks $N!", vermin, NULL, victim, TO_ROOM);
      multi_hit(vermin, victim, TYPE_UNDEFINED);
   }
   return;
}

void spell_epidemic(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;
   char buf[MAX_STRING_LENGTH];

   for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
   {
      vch_next = vch->next_in_room;
      if (IS_IMMORTAL(vch) && !can_see(ch, vch) && wizi_to(vch, ch)) continue;
      if (is_safe(ch, vch, IS_SAFE_SILENT | IS_SAFE_AREA_ATTACK)) continue;
      if (is_same_group(ch, vch)) continue;
      if (!IS_NPC(ch) && !IS_NPC(vch)  && (ch->fighting == NULL ||
      vch->fighting == NULL)){
         sprintf(buf, "Die, %s, you sorcerous dog!", PERS(ch, vch));
         do_myell(vch, buf);
         sprintf(log_buf, "[%s] cast epidemic upon [%s] at %d", ch->name, vch->name, ch->in_room->vnum);
         log_string(log_buf);
      }
      magic_spell_vict
      (
         ch,
         vch,
         CAST_BITS_REUSE,
         level + 10,
         gsn_plague
      );
      if (vch->fighting == NULL)
      multi_hit(vch, ch, TYPE_UNDEFINED);
   }
   return;
}

void spell_bane(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;
   char buf[MAX_STRING_LENGTH];

   for (vch=ch->in_room->people; vch != NULL; vch = vch_next)
   {
      vch_next = vch->next_in_room;
      if (IS_IMMORTAL(vch) && !can_see(ch, vch) && wizi_to(vch, ch)) continue;
      if (is_safe(ch, vch, IS_SAFE_SILENT | IS_SAFE_AREA_ATTACK)) continue;
      if (is_same_group(ch, vch)) continue;
      if (!IS_NPC(ch) && !IS_NPC(vch) && (ch->fighting == NULL ||
      vch->fighting == NULL)){
         sprintf(buf, "Die, %s, you sorcerous dog!", PERS(ch, vch));
         do_myell(vch, buf);
         sprintf(log_buf, "[%s} cast bane upon [%s] at %d", ch->name, vch->name, ch->in_room->vnum);
         log_string(log_buf);
      }
      magic_spell_vict
      (
         ch,
         vch,
         CAST_BITS_REUSE,
         level + 15,
         gsn_poison
      );
      if (vch->fighting == NULL)
      multi_hit(vch, ch, TYPE_UNDEFINED);
   }
   return;
}

/* Emaciation code -werv */

void spell_emaciation(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   int dam;

   if (saves_spell(ch, level, victim, DAM_OTHER, SAVE_MALEDICT))
   {
      act("You attempt to call upon the dark gods, but fail.", ch, NULL, victim, TO_CHAR);
      dam = number_range(level/2, level);
      damage(ch, victim, dam*race_adjust(ch)/25.0, sn, DAM_NEGATIVE, TRUE);
      return;
   }

   dam = number_range(level, 2*level);

   if (victim->carrying != NULL)
   purge_food(victim->carrying);

   act("$n calls upon the dark gods and emaciates $N.", ch, NULL, victim, TO_ROOM);
   act("You call upon the dark gods and emaciate $N.", ch, NULL, victim, TO_CHAR);
   damage(ch, victim, dam*race_adjust(ch)/25.0, sn, DAM_NEGATIVE, TRUE);

   return;
}

/*
Recursive subroutine to handle purging food from containers
in spell_emaciation -Werv
*/

void purge_food(OBJ_DATA *obj)
{
   OBJ_DATA *next;

   next = obj->next_content;


   if (obj->item_type == ITEM_FOOD)
   extract_obj(obj, FALSE);

   if (obj->item_type == ITEM_CONTAINER)
   {
      if (obj->contains != NULL)
      purge_food(obj->contains);
   }

   if (next != NULL)
   purge_food(next);
   return;
}

void spell_meteor( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;
   ROOM_AFFECT_DATA raf;
   int dam;

   act("Rocks rain down from the sky!", ch, ch, NULL, TO_ROOM);
   for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
   {
      vch_next = vch->next_in_room;
      if (IS_IMMORTAL(vch)) continue;
      dam = vch->hit/2+dice(3, 6);
      if (!IS_NPC(vch)){
         if (dam > vch-> hit)
         dam = vch->hit - 10;
      }
      else
      {
         dam = dam + 50;
      }
      if (dam > 0)
      damage( vch, vch, dam, sn, DAM_FIRE , ROOM_METEOR_DAM_TYPE);
      vch->fighting = NULL;
      ch->fighting = NULL;
      vch->position = POS_STANDING;
      update_pos(vch);
   }
   ch->position = POS_STANDING;
   update_pos(ch);
   if (!is_affected_room(ch->in_room, sn))
   {
      raf.where     = TO_ROOM;
      raf.duration = -1;
      raf.type = sn;
      raf.level = level;
      raf.bitvector = 0;
      raf.caster = ch;
      raf.where     = TO_ROOM_SECTOR;
      raf.sector_new = SECT_DESERT;
      affect_to_room(ch->in_room, &raf);
   }

   return;
}

void spell_ameteor( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;
   ROOM_AFFECT_DATA raf;
   int dam;

   for (vch = char_list; vch != NULL; vch = vch_next)
   {
      vch_next = vch->next;
      if (vch->in_room == NULL) continue;
      if ( vch->in_room->area != ch->in_room->area )
      continue;
      if (IS_IMMORTAL(vch)) continue;
      dam = vch->hit/2+dice(3, 6);
      if (!IS_NPC(vch)){
         if (dam > vch-> hit)
         dam = vch->hit - 10;
      }
      else
      {
         dam = dam + 50;
      }
      send_to_char("Rocks rain down from the sky!\n\r", vch);
      if (dam > 0)
      damage( vch, vch, dam, sn, DAM_FIRE , ROOM_METEOR_DAM_TYPE);
      vch->fighting = NULL;
      vch->position = POS_STANDING;
      update_pos(vch);
      ch->fighting = NULL;
      if (!is_affected_room(vch->in_room, gsn_meteor))
      {
         raf.where     = TO_ROOM;
         raf.duration = -1;
         raf.type = gsn_meteor;
         raf.level = level;
         raf.bitvector = 0;
         raf.caster = ch;
         raf.where     = TO_ROOM_SECTOR;
         raf.sector_new = SECT_DESERT;
         affect_to_room(ch->in_room, &raf);
      }
   }
   ch->position = POS_STANDING;
   update_pos(ch);
   return;
}

/* demon swarm as a skill - Werv */
void do_demon_swarm(CHAR_DATA *ch, char * argument)
{
   /* demon swarm spell for Abyss - Runge               */
   /* does 3+(level/20) hits, each hit does (level)d2 damage */
   /* each hit gives a save for 3/4 damage, type fire      */
   CHAR_DATA *victim;
   char buf[MAX_STRING_LENGTH];
   int damn;  /* yes, an intentional pun */
   int numdice;
   int d;
   int sn;

   if (house_down(ch, HOUSE_BRETHREN))
   return;
   sn = gsn_demon_swarm;
   if (get_skill(ch, sn) < 1 || !has_skill(ch, sn))
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }
   victim = NULL;
   if (argument[0] != '\0')
   victim = get_char_room(ch, argument);
   if (victim == NULL)
   victim = ch->fighting;
   if (victim == NULL)
   {
      send_to_char("They aren't here.\n\r", ch);
      return;
   }
   WAIT_STATE(ch, 24);
   if (ch->mana < 20)
   {
      send_to_char("You are too weak to open a gate.\n\r", ch);
      return;
   }
   ch->mana -= 20;

   if (number_percent() > get_skill(ch, sn))
   {
      send_to_char("You fail to open the gate.\n\r", ch);
      check_improve(ch, sn, FALSE, 1);
      return;
   }

   if (oblivion_blink(ch, victim))
   {
      return;
   }

   if (!IS_NPC(victim))
   {
      if (victim->fighting == NULL || ch->fighting == NULL)
      {
         sprintf(log_buf, "%s used demon swarm on %s. Room[%d].", ch->name,
         victim->name, ch->in_room->vnum);
         log_string(log_buf);
         sprintf(buf, "Help! %s is summoning demons to attack me!", PERS(ch, victim));
         do_myell(victim, buf);
      }
   }
   check_improve(ch, sn, TRUE, 1);
   numdice = 3+(ch->level/20);
   damn = 0;
   act("$n opens a gate and several small demons rush out!.", ch, NULL, NULL, TO_ROOM);
   send_to_char("You open a gate and several small demons rush out!.\n\r", ch);
   for (d=0; d<numdice; d++)
   {
      damn = dice(ch->level, 2);
      if (IS_NPC(victim) && victim->house == 0)
      damn += damn/2;
      else
      damn -= damn/3;
      act("The demons tear into $n!", victim, NULL, NULL, TO_ROOM);
      send_to_char("The demons tear at your flesh!\n\r", victim);
      damage( ch, victim, damn, sn, DAM_OTHER , TRUE);
      if (victim->position == POS_DEAD)
      return;
   }
   act("$n snaps the gate shut and the demons disappear!", ch, NULL, NULL, TO_ROOM);
   send_to_char("You snap shut the gate, and the demons disappear!\n\r", ch);
   return;
}


void spell_demon_swarm(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   /* demon swarm spell for Abyss - Runge               */
   /* does 3+(level/20) hits, each hit does (level)d2 damage */
   /* each hit gives a save for 3/4 damage, type fire      */
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   int damn;  /* yes, an intentional pun */
   int numdice;
   int d;

   if (house_down(ch, HOUSE_BRETHREN))
   return;

   numdice = 3+(ch->level/20);
   damn = 0;
   act("$n opens a gate and several small demons rush out!.", ch, NULL, NULL, TO_ROOM);
   send_to_char("You open a gate and several small demons rush out!.\n\r", ch);
   for (d=0; d<numdice; d++)
   {
      damn = dice(ch->level, 2);
      if (saves_spell(ch, level, victim, DAM_OTHER, SAVE_SPELL))
      damn=(damn*3)/4;
      act("The demons tear into $n!", victim, NULL, NULL, TO_ROOM);
      send_to_char("The demons tear at your flesh!\n\r", victim);
      damage( ch, victim, damn, sn, DAM_FIRE , TRUE);
      if (victim->position == POS_DEAD)
      return;
   }
   act("$n snaps the gate shut and the demons disappear!", ch, NULL, NULL, TO_ROOM);
   send_to_char("You snap shut the gate, and the demons disappear!\n\r", ch);
   return;
}

void spell_gout_maggots(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   /* gout of maggots spell for corruptor spec. - Runge                */
   /* does (level)d9 damage and a 30% chance of a plague spell            */
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   /*   AFFECT_DATA af;*/
   int dam;
   int gm;

   gm = gsn_gout_of_maggots;
   /*
   if (is_affected(ch, gm))
   {
   send_to_char("Your powers of corruption need time to replenish.\n\r", ch);
   return;
   }
   */
   act("$n vomits forth a stream of maggots at $N!", ch, NULL, victim, TO_ROOM);
   send_to_char("You vomit forth a stream of maggots!\n\r", ch);

   dam = dice(level, 9)*race_adjust(ch)/25.0;
   if (check_spellcraft(ch, sn))
   dam = spellcraft_dam(level, 9)*race_adjust(ch)/25.0;

   if (saves_spell(ch, level, victim, DAM_DISEASE, SAVE_SPELL) )
   dam /= 2;
   if (number_percent() < 30)
   {
      magic_spell_vict
      (
         ch,
         victim,
         CAST_BITS_REUSE,
         level,
         gsn_plague
      );
   }
   damage(ch, victim, dam, gm, DAM_DISEASE, TRUE);
   /*
   af.where = TO_AFFECTS;
   af.type = gm;
   af.level = ch->level;
   af.location = 0;
   af.modifier = 0;
   af.duration = 0;
   af.bitvector = 0;
   affect_to_char(ch, &af);
   */
   return;
}

void spell_ancient_plague( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   /* ancient plague spell for corruptor necros - Runge             */
   /* non curable, halves magical healing, wakes up like plague, slows */
   /* hp regen like plague, does no tick damage, spreads like plague   */
   /* lowers Str/Con                             */
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;
   int ap;

   ap = gsn_ancient_plague;
   if (is_affected(victim, ap))
   {
      send_to_char("That one is already infected by the ancient plague.\n\r", ch);
      return;
   }
   if (saves_spell(ch, level, victim, DAM_DISEASE, SAVE_MALEDICT)
   || (IS_NPC(victim) && IS_SET(victim->act, ACT_UNDEAD))
   || (IS_NPC(victim) && victim->pIndexData->vnum == MOB_VNUM_CRUSADER))
   {
      if (ch == victim)
      send_to_char("You feel a profound sense of fatigue, but it passes.\n\r", ch);
      else
      act("$N seems to be unaffected.", ch, NULL, victim, TO_CHAR);
      return;
   }

   af.where     = TO_AFFECTS;
   af.type     = ap;
   af.level    = level;
   af.duration  = (level/4);
   af.location  = APPLY_STR;
   af.modifier  = -(level/20)+1;
   af.bitvector = 0;
   affect_to_char(victim, &af);
   af.location  = APPLY_CON;
   af.modifier  = -(level/20)+1;
   affect_to_char(victim, &af);

   send_to_char("You turn pale, and a profound fatigue overcomes you.\n\r", victim);
   act("$n turns pale, and looks very sick.", victim, NULL, NULL, TO_ROOM);
   return;
}

void spell_strengthen_bone( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   /* strengthen bone spell for anatomist necros - Runge       */
   /* caster only spell, adds caster's level in hp, and grants */
   /* resistance to bash/slash in fight.c. lasts 12 ticks max  */

   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;

   if ( is_affected( victim, sn ) )
   {
      if (victim == ch)
      send_to_char("Your skeleton is already fortified.\n\r", ch);
      else
      act("$N's skeleton is already fortified.", ch, NULL, victim, TO_CHAR);
      return;
   }
   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = ch->level;
   af.duration  = 12*race_adjust(ch)/25.0;
   af.modifier  = ch->level;
   af.location  = APPLY_HIT;
   af.bitvector = 0;
   affect_to_char( victim, &af );
   send_to_char( "You feel strength flow through your bones.\n\r", victim );
   if ( ch != victim )
   send_to_char("You fortify their skeleton with your magic.\n\r", ch);
   return;
}

void spell_boiling_blood( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   /* boiling blood spell for anatomist necros - Runge          */
   /* aggro spell, lowers str by (caster's level)/10, lasts     */
   /* (caster's level)/10 ticks. does (1 - (caster's level)*2)  */
   /* fire damage per tick                                      */
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;
   AFFECT_DATA* paf;

   if (saves_spell(ch, level, victim, DAM_FIRE, SAVE_MALEDICT))
   {
      if (ch == victim)
      send_to_char("Your vital fluids heat up uncomfortably, but it fades quickly.\n\r", ch);
      else
      act("$N seems to be unaffected.", ch, NULL, victim, TO_CHAR);
      return;
   }

   if ((IS_NPC(victim) && IS_SET(victim->act, ACT_UNDEAD))
   || (IS_SET(victim->act2, PLR_LICH)))
   {
      send_to_char("Your spell has no effect on the unliving.\n\r", ch);
      return;
   }

   if ( is_affected( victim, sn ) )
   {
      if (victim == ch)
      send_to_char("Your vital fluids are already searing!\n\r", ch);
      else
      act("$N's vital fluids are already boiling!", ch, NULL, victim, TO_CHAR);
      return;
   }

   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = ch->level;
   af.duration  = ((ch->level)/10)*race_adjust(ch)/25.0;
   af.modifier  = -(ch->level)/10;
   af.location  = APPLY_STR;
   af.bitvector = 0;
   affect_to_char( victim, &af );
   paf = affect_find(victim->affected, sn);
   if (paf != NULL)
   {
      free_string(paf->caster);
      paf->caster = str_dup(ch->name);
   }

   send_to_char( "Your vital fluids heat up to an incredible level!\n\r", victim );
   if ( ch != victim )
   act("$N's vital fluids begin to boil!", ch, NULL, victim, TO_CHAR);
   return;
}

void spell_strengthen_construct( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   /* Strengthen construct spell for Anatomist necros - Runge         */
   /* only works on flesh golems, lasts until the golem is destroyed. */
   /* adds (level*3) to the golems hp, (level/5) to the golems        */
   /* hit and damroll, and gives it resistance to weapons             */
   /* can only be cast once per flesh golem                   */

   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;

   if (is_affected(ch, gsn_strengthen_construct))
   {
      send_to_char("You are not yet ready to strengthen another golem.\n\r", ch);
      return;
   }

   if ((!IS_NPC(victim)) || (victim->pIndexData->vnum != MOB_VNUM_L_GOLEM)
   || ((victim->master != NULL) && (victim->master != ch)))
   {
      send_to_char("This magic can only affect your own flesh golems.\n\r", ch);
      return;
   }

   if ( is_affected( victim, sn ) )
   {
      send_to_char("That construct has already been augmented.\n\r", ch);
      return;
   }

   victim->max_hit += (ch->level)*3;
   victim->hitroll += (ch->level)/5;
   victim->damroll += (ch->level)/5;
   SET_BIT(victim->res_flags, RES_WEAPON);

   af.where    = TO_AFFECTS;
   af.type      = sn;
   af.level    = level;
   af.duration  = -1;
   af.modifier  = 0;
   af.location  = 0;
   af.bitvector = 0;
   affect_to_char(victim, &af);
   af.duration     = 12;
   affect_to_char(ch, &af);
   send_to_char("You feel augmented.\n\r", victim );
   act("You augment the structure of $N.", ch, NULL, victim, TO_CHAR);
   return;
}


void do_chant( CHAR_DATA *ch, char *argument)
{
   AFFECT_DATA af;

   if ( (get_skill(ch, gsn_chant) <= 0)
   || !has_skill(ch, gsn_chant))
   {
      send_to_char("You do not know the proper rhythms of the chant.\n\r", ch);
      return;
   }

   if (is_affected(ch, gsn_chant) )
   {
      send_to_char("You are already chanting.\n\r", ch);
      return;
   }

   if (ch->mana < 40)
   {
      send_to_char("You do not have the mana.\n\r", ch);
      return;
   }

   if (number_percent() > get_skill(ch, gsn_chant))
   {
      send_to_char("You begin the chant, but your rhythm falters.\n\r", ch);
      check_improve(ch, gsn_chant, FALSE, 1);
      ch->mana -= 20;
      return;
   }

   af.where = TO_AFFECTS;
   af.type = gsn_chant;
   af.level = ch->level;
   af.location = APPLY_AC;
   af.modifier = -(ch->level * 2);
   af.duration = ch->level;
   af.bitvector = 0;
   affect_to_char(ch, &af);
   af.location = APPLY_HITROLL;
   af.modifier = 5 + (ch->level/5);
   affect_to_char(ch, &af);
   ch->mana -= 40;
   act("$n begins a rhythmic, mystical chant.", ch, NULL, NULL, TO_ROOM);
   send_to_char("You feel your spirit hardening as you begin your chant.\n\r", ch);
   check_improve(ch, gsn_chant, TRUE, 1);
   return;
}

void do_dchant( CHAR_DATA *ch, char *argument)
{
   AFFECT_DATA af;

   if ( (get_skill(ch, gsn_dchant) <= 0)
   || !has_skill(ch, gsn_dchant))
   {
      send_to_char("You do not know the proper rhythms of the dragon's chant.\n\r", ch);
      return;
   }

   if (is_affected(ch, gsn_dchant) )
   {
      send_to_char("You are already chanting.\n\r", ch);
      return;
   }

   if (ch->mana < 50)
   {
      send_to_char("You do not have the mana.\n\r", ch);
      return;
   }

   if (number_percent() > get_skill(ch, gsn_dchant))
   {
      send_to_char("You begin the dragon chant, but your rhythm falters.\n\r", ch);
      check_improve(ch, gsn_dchant, FALSE, 1);
      ch->mana -= 25;
      return;
   }

   af.where = TO_AFFECTS;
   af.type = gsn_dchant;
   af.level = ch->level;
   af.location = APPLY_AC;
   af.modifier = -(ch->level * 3);
   af.duration = ch->level;
   af.bitvector = 0;
   affect_to_char(ch, &af);
   ch->mana -= 40;
   act("$n begins a bizarre chant.", ch, NULL, NULL, TO_ROOM);
   send_to_char("You feel yourself protected by your chanting.\n\r", ch);
   check_improve(ch, gsn_dchant, TRUE, 1);
   return;
}

void do_harmony(CHAR_DATA* ch, char* argument)
{
   AFFECT_DATA* af;
   AFFECT_DATA* af_next;
   AFFECT_DATA* af_temp;
   sh_int af_type;

   if (IS_NPC(ch))
   {
      return;
   }

   if (!has_skill(ch, gsn_harmony) || ch->pcdata->learned[gsn_harmony] < 0)
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }

   if (ch->mana < 60)
   {
      send_to_char
      (
         "You do not have the mental resolve to meditate.\n\r",
         ch
      );
      return;
   }

   if (number_percent() > get_skill(ch, gsn_harmony))
   {
      WAIT_STATE(ch, 12);
      ch->mana -= 30;
      send_to_char("You fail to go into the trance.\n\r", ch);
      check_improve(ch, gsn_harmony, FALSE, 1);
      return;
   }

   ch->mana -=60;
   act
   (
      "$n falls into a deep trance.",
      ch,
      NULL,
      NULL,
      TO_ROOM
   );
   send_to_char
   (
      "You fall into a deep trance as you contemplate your inner harmony.\n\r",
      ch
   );
   check_improve(ch, gsn_harmony, TRUE, 1);

   /* new harmony code */
   for ( af = ch->affected; af != NULL; af = af_next )
   {
      af_type = af->type;
      af_next = af->next;
      af_temp = af_next;
      while
      (
         af_temp != NULL &&
         af_temp->type == af_type
      )
      {
         af_temp = af_temp->next;
         /*
         find first affect that is NOT the same type
         since affect_strip removes all of one type
         */
      }
      if
      (
         af_type < 1 ||
         af_type >= MAX_SKILL ||
         skill_table[af_type].name == NULL
      )
      {
         bug("do_harmony bad af->type %d", af_type);
         continue;
      }
      if
      (
         af->type == gsn_charm_person ||
         (
            af->where == TO_AFFECTS &&
            IS_SET(af->bitvector, AFF_CHARM)
         )
      )
      {
         /* Cannot remove charm with harmony */
         continue;
      }
      if (check_dispel(ch->level + 15, ch, af->type))
      {
         af_next = af_temp;
      }
   }

   WAIT_STATE(ch, 24);
   return;
}


void spell_propaganda( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{


   int vnum;
   AFFECT_DATA af;
   int dtype = DAM_CHARM;
   CHAR_DATA *victim = (CHAR_DATA *) vo;



   switch(number_range(0, 3))
   {
      case (0): {
         do_say(ch, "Refuse to live your life by another mortal's rules!");
         break;
      }
      case (1): {
         do_say(ch, "Live free, or die as a slave!");
         break;
      }
      case (2): {
         do_say(ch, "May you be blessed by the wild spirit of Malignus!");
         break;
      }
      case (3): {
         do_say(ch, "Long live the Revolution! Overthrow the tyrannous forces of Law!");
         break;
      }
   }

   if (house_down(ch, HOUSE_OUTLAW))
   return;


   if (is_affected(ch, gsn_propaganda)){
      send_to_char("You dont have more propaganda ready yet.\n\r", ch);
      return;
   }

   if (!IS_NPC(victim))  /* Xurinos's addition to fix crashes due to vnum */
   {
      send_to_char("Okay, that's nice. Now convince them.\n\r", ch);
      return;
   }

   if (is_affected(victim, gsn_propaganda)){
      act("$E does not need anymore persuasion.", ch, NULL, victim, TO_CHAR);
      /*    send_to_char("They dont need anymore persuasion.\n\r", ch); */
      return;
   }
   if (victim == ch && ch->fighting != NULL)
   victim = ch->fighting;

   if (victim == NULL)
   return;

   if (is_same_group(victim, ch)) {

      af.where = TO_AFFECTS;
      af.level = level;
      af.location = 0;
      af.modifier = level / 6;
      af.bitvector = 0;
      af.type = gsn_propaganda;
      af.duration = 24;
      affect_to_char(ch, &af);

      af.duration = 12;
      af.location  = APPLY_HITROLL;
      affect_to_char(victim, &af);
      af.location  = APPLY_DAMROLL;
      affect_to_char(victim, &af);

      send_to_char("Your spirit is set ablaze by the words of Revolution!\n\r", victim);
      act("$n is charged by the spirit of Outlaw!", victim, NULL, NULL, TO_ROOM);
      return;

   }

   af.where = TO_AFFECTS;
   af.level = level;
   af.location = 0;
   af.modifier = 0;
   af.bitvector = 0;
   af.type = gsn_propaganda;
   af.duration = 6;
   affect_to_char(ch, &af);

   if (victim->spec_fun != 0 &&
   victim->spec_fun != spec_lookup("spec_guard") &&
   victim->spec_fun != spec_lookup("spec_legion") &&
   victim->spec_fun != spec_lookup("spec_executioner"))
   {
      act("$E does not seem interested in your propaganda.", ch, NULL, victim, TO_CHAR);
      /*    send_to_char("They don't seem interested in your propaganda.\n\r", ch);*/
      return;
   }

   vnum = victim->pIndexData->vnum;
   if (victim->house != 0 ||
   vnum == MOB_VNUM_UNICORN ||
   vnum == MOB_VNUM_DRAGON ||
   vnum == MOB_VNUM_STEED ||
   vnum == MOB_VNUM_PROTECTOR ||
   (ch->in_room->house != 0 &&
   ch->in_room->house !=ch->house)){
      act("$E seems to be beyond reproach.", ch, NULL, victim, TO_CHAR);
      /*    send_to_char("They seem beyond reproach.\n\r", ch); */
      return;
   }

   if (IS_SET(victim->off_flags, ASSIST_GUARD) ||
   victim->spec_fun == spec_lookup("spec_guard")
   ||victim->spec_fun == spec_lookup("spec_legion"))
   dtype = DAM_OTHER;

   if ( saves_spell( ch, level, victim, dtype, SAVE_OTHER))
   {
      if (victim->level < 60){
         send_to_char("You give them your propaganda, but they dont seem to care.\n\r", ch);
         return;
      }
      do_yell(victim, "How dare you suggest I be a traitor!");
      multi_hit( victim, ch, TYPE_UNDEFINED );
      return;
   }

   act("$N accepts $n's propaganda.", ch, NULL, victim, TO_NOTVICT);
   send_to_char("They agree to fight for freedom!\n\r", ch);

   stop_fighting(victim, TRUE);
   victim->last_fought = -1;
   stop_fighting(ch, TRUE);
   REMOVE_BIT(victim->act, ACT_AGGRESSIVE);
   REMOVE_BIT(victim->off_flags, SPAM_MURDER);

   victim->spec_fun = spec_lookup   ("spec_revolutionary");

   af.duration = 12;
   affect_to_char(victim, &af);
   return;
}

void spell_kidnap(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA * victim = (CHAR_DATA *) vo;
   ROOM_INDEX_DATA *pRoomIndex;
   char buf[MAX_STRING_LENGTH];
   bool cant_kidnap = FALSE;
   CHAR_DATA * och;
   CHAR_DATA * och_next;
   ROOM_INDEX_DATA *sourceroom;
   AFFECT_DATA af;

   if (house_down(ch, HOUSE_OUTLAW))
   return;

   if
   (
      is_affected(ch, gsn_purity)
   )
   {
      send_to_char("The mark of purity prevents you from kidnapping!\n\r", ch);
      return;
   }

   if
   (
      IS_SET(ch->in_room->extra_room_flags, ROOM_DEAD_ZONE) &&
      !IS_SET(ch->in_room->room_flags, ROOM_CONSECRATED)
   )
   {
      send_to_char("Something about this place is causing you to fail at kidnapping!\n\r", ch);
      return;
   }
   if (IS_SET(ch->in_room->extra_room_flags, ROOM_1212)){
      send_to_char("The immortals frown at your arrogance.\n\r", ch);
      return;
   }
   if (is_affected(ch, sn)) {
      send_to_char("You must regain your teleportive power first.\n\r", ch);
      return;
   }

   if (victim == ch)
   {
      send_to_char("Why not just blindfold yourself and ask someone else to do it?\n\r", ch);
      return;
   }
   if (is_safe(ch, victim, 0))
   return;
   if (victim->fighting != NULL)
   {
      send_to_char("They are moving too much to attempt a kidnap.\n\r", ch);
      return;
   }

   if (IS_NPC(victim))
   {
      if
      (
         victim->house != 0
         || IS_SET(victim->act, ACT_AGGRESSIVE)
         || ((victim->in_room->house != 0
         && ch->in_room->house != ch->house))
      )
      cant_kidnap = TRUE;
   }
   else
   if ((victim->in_room->house != 0) && (ch->in_room->house != ch->house))
   cant_kidnap = TRUE;

   if (IS_NPC(victim) && victim->pIndexData->vnum == 10646)
   {
      do_yell(victim, "How dare you try to kidnap me!");
      do_bash(victim, ch->name);
      return;
   }

   if (saves_spell(ch, level-3, victim, DAM_OTHER, SAVE_TRAVEL) || (cant_kidnap))
   {
      act("$n tries to kidnap $N but fails.", ch, NULL, victim, TO_NOTVICT);
      act("Your kidnap attempt on $N failed.", ch, NULL, victim, TO_CHAR);
      act("$n tried to kidnap you!", ch, NULL, victim, TO_VICT);
      if (number_percent() < 50)
      sprintf(buf, "Help! %s tried to kidnap me!", PERS(ch, victim));
      else
      sprintf(buf, "Die %s, you abducting mongrel!", PERS(ch, victim));
      do_myell(victim, buf);
      multi_hit(victim, ch, TYPE_UNDEFINED);
      return;
   }

   act("You kidnap $N and push them into a shimmering portal!", ch, NULL, victim, TO_CHAR);
   act("$n encloses you in a magical field and you find yourself elsewhere!", ch, NULL, victim, TO_VICT);
   act("$n and $N suddenly vanish into a shimmering field of magic!", ch, NULL, victim, TO_NOTVICT);

   af.where=TO_AFFECTS;
   af.type=sn;
   af.level=level;
   af.duration=3;
   af.location=0;
   af.modifier=0;
   af.bitvector=0;
   affect_to_char(ch, &af);

   pRoomIndex = get_random_room(ch, RANDOM_NORMAL);
   if (pRoomIndex == NULL)
   {
      return;
   }

   sourceroom = ch->in_room;
   char_from_room(victim);
   char_from_room(ch);
   char_to_room(ch, pRoomIndex);
   act("A shimmering portal appears and $n steps out.", ch, NULL, NULL, TO_ROOM);
   char_from_room(ch);
   char_to_room(victim, pRoomIndex);
   act("$n comes tumbling out of the shimmering portal.", victim, NULL, NULL, TO_ROOM);
   char_to_room(ch, pRoomIndex);
   do_observe(ch, "", LOOK_AUTO);
   do_observe(victim, "", LOOK_AUTO);
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
         char_to_room(och, ch->in_room);
      }
   }

   return;
}

/*

void spell_glamour( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   OBJ_DATA *obj = (OBJ_DATA *) vo;
   MESSAGE glamour;
   char full[MAX_STRING_LENGTH];
   int random;

   if ( !obj )
   return;

   if (obj->pIndexData->house)
   {
      send_to_char( "The power of that item outshines your ability.\n\r", ch );
      return;
   }

   strcpy("A", full);

   random = number_range(1, 20);

   switch( random )
   {
      case 1:
      strcat( full, "n old" );
      break;
      case 2:
      strcat( full, " wet" );
      break;
      case 3:
      strcat( full, " purple" );
      break;
      case 4:
      strcat( full, " red" );
      break;
      case 5:
      strcat( full, " green" );
      break;
      case 6:
      strcat( full, " blue" );
      break;
      case 7:
      strcat( full, " worn" );
      break;
      case 8:
      strcat( full, "n ugly" );
      break;
      case 9:
      strcat( full, " nasty" );
      break;
      case 10:
      strcat( full, " wicker" );
      break;
      case 11:
      strcat( full, " leather" );
      break;
      case 12:
      strcat( full, " rusty" );
      break;
      case 13:
      strcat( full, " faded" );
      break;
      case 14:
      strcat( full, " dirty" );
      break;
      case 15:
      strcat( full, " shiny" );
      break;
      case 16:
      strcat( full, " shimmering" );
      break;
      case 17:
      strcat( full, " silk" );
      break;
      case 18:
      strcat( full, " cracking" );
      break;
      case 19:
      strcat( full, " brocaded" );
      break;
      case 20:
      strcat( full, " slimy" );
      break;
   }

   random = number_range(1, 20);

   switch( random )
   {
      case 1:
      strcat( full, " pair of boots" );
      break;
      case 2:
      strcat( full, " sleeping bag" );
      break;
      case 3:
      strcat( full, " potion" );
      break;
      case 4:
      strcat( full, " pill" );
      break;
      case 5:
      strcat( full, " dagger" );
      break;
      case 6:
      strcat( full, " tunic" );
      break;
      case 7:
      strcat( full, " piece of jerky" );
      break;
      case 8:
      strcat( full, " bunny rabbit" );
      break;
      case 9:
      strcat( full, " statuette" );
      break;
      case 10:
      strcat( full, " weasel" );
      break;
      case 11:
      strcat( full, " shield" );
      break;
      case 12:
      strcat( full, " leggings" );
      break;
      case 13:
      strcat( full, " cap" );
      break;
      case 14:
      strcat( full, " wand" );
      break;
      case 15:
      strcat( full, " blanket" );
      break;
      case 16:
      strcat( full, " butterfly" );
      break;
      case 17:
      strcat( full, " quiver" );
      break;
      case 18:
      strcat( full, " bow" );
      break;
      case 19:
      strcat( full, " basket" );
      break;
      case 20:
      strcat( full, " rose" );
      break;
   }

   glamour.message_type = OM_GLAMOUR_SHORT;
   glamour.message_data = 0;
   glamour.message_string = full;
   glamour.message_flags = MESSAGE_POLL_TICK | MESSAGE_UNIQUE | MESSAGE_REPLACE;
   glamour.time_to_live = 3;

   message_to_obj( obj, &glamour );

   strcat( full, " is here.");

   glamour.message_type = OM_GLAMOUR_LONG;
   glamour.message_data = 0;
   glamour.message_string = full;
   glamour.message_flags = MESSAGE_POLL_TICK | MESSAGE_UNIQUE | MESSAGE_REPLACE;
   glamour.time_to_live = 3;

   message_to_obj( obj, &glamour );

   send_to_char( "A brief ripple flutters through the air.\n\r", ch );

   return;
}

*/

void do_lookout(CHAR_DATA *ch, char *argument)
{
   CHAR_DATA *guard;
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA *scan;
   CHAR_DATA *oldest_spy;
   int spy_count;
   int oldest_time;

   if (!has_skill(ch, gsn_lookout) ||
   get_skill(ch, gsn_lookout) < 1)
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }
   if (IS_NPC(ch))
   {
      send_to_char("Only players may own spies.\n\r", ch);
      return;
   }
   spy_count = 0;
   oldest_spy = NULL;
   oldest_time = 4000;
   for (scan = char_list; scan != NULL; scan = scan->next)
   {
      if (!IS_NPC(scan)) continue;
      if (scan->spyfor != ch) continue;
      spy_count++;
      if (scan->pause < oldest_time)
      {
         oldest_time = scan->pause;
         oldest_spy = scan;
      }
   }
   if (spy_count >= 3 && oldest_spy != NULL)
   {
      extract_char(oldest_spy, TRUE);
   }

   if (ch->gold < ch->level*2)
   {
      send_to_char("You do not have enough gold to pay off a lookout.\n\r", ch);
      return;
   }

   if (number_percent() > get_skill(ch, gsn_lookout))
   {
      send_to_char("No spies seem ready to accept your money.\n\r", ch);
      check_improve(ch, gsn_lookout, FALSE, 1);
      return;
   }
   send_to_char("A gnome spy shows up, accepts your fee, and disguises himself.\n\r", ch);
   ch->gold -= ch->level*2;

   guard = create_mobile(get_mob_index(MOB_VNUM_GNOME_SPY) );
   guard->level = ch->level;
   guard->pause = 36;

   switch ( number_bits(3) )
   {
      case 0: sprintf(buf, "A gnome merchant is here, peddling his wares.\n\r"); break;
      case 1: sprintf(buf, "A gnome astronomer stares into the sky.\n\r"); break;
      case 2: sprintf(buf, "A gnomish inventor looks around for new ideas.\n\r"); break;
      case 3: sprintf(buf, "A gnome traveller wanders about.\n\r"); break;
      case 4: sprintf(buf, "A gnome jeweller is seeking a bargain.\n\r"); break;
      case 5: sprintf(buf, "A gnome miner is searching for the perfect vein.\n\r"); break;
      case 6: sprintf(buf, "A gnome man appears to be lost.\n\r"); break;
      case 7: sprintf(buf, "A gnome alchemist gathers new ingredients for his work.\n\r");
   }
   free_string( guard->long_descr );
   guard->long_descr = str_dup( buf );

   guard->spyfor = ch;
   char_to_room(guard, ch->in_room);
   check_improve(ch, gsn_lookout, TRUE, 1);
   return;
}

void do_rapier(CHAR_DATA *ch, char * argument)
{
   OBJ_DATA *staff;
   char buf[MAX_STRING_LENGTH];
   AFFECT_DATA af;

   if (get_skill(ch, gsn_rapier) <= 0 ||
   !has_skill(ch, gsn_rapier))
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }
   WAIT_STATE(ch, 24);
   if (get_skill(ch, gsn_rapier) <= number_percent())
   {
      send_to_char("You can't find your rapier.\n\r", ch);
      check_improve(ch, gsn_rapier, FALSE, 1);
      return;
   }
   staff = create_object(get_obj_index(OBJ_VNUM_RAPIER), 0);
   free_string(staff->name);
   staff->name = str_dup("rapier");
   free_string(staff->description);
   staff->description = str_dup("A shiny steel rapier lies here.");
   free_string(staff->short_descr);
   staff->short_descr = str_dup("rapier");
   staff->extra_flags = 0;
   staff->weight = 30;
   staff->value[0] = WEAPON_SWORD;
   staff->value[3] = 2; /* pierce */
   staff->value[4] = 0;
   if (number_range(1, 3) == 1 && ch->level > 30)
   SET_BIT(staff->value[4], WEAPON_SHARP);
   if (number_range(1, 5) == 1 && ch->level > 45)
   SET_BIT(staff->value[4], WEAPON_PIERCING);
   free_string(staff->material);
   staff->material = str_dup("steel");
   check_improve(ch, gsn_rapier, TRUE, 1);

   af.where = TO_OBJECT;
   af.type = gsn_rapier;
   af.level = ch->level;
   af.duration = -1;
   af.bitvector = 0;
   staff->level = ch->level;
   staff->value[1] = 6;
   staff->value[2] = 6;
   if (ch->level > 35)
   staff->value[2]++;
   if (ch->level > 50)
   staff->value[2]++;
   af.location = APPLY_HITROLL;
   af.modifier = ch->level/6;
   affect_to_obj(staff, &af);
   af.location = APPLY_DAMROLL;
   af.modifier = ch->level/6;
   affect_to_obj(staff, &af);
   staff->timer = 90;
   sprintf(buf, "%s", ch->name);
   free_string(staff->owner);
   staff->owner = str_dup(buf);
   obj_to_char(staff, ch);
   act("$n pulls out $s rapier and prepares for a brawl!", ch, staff, NULL, TO_ROOM);
   act("You pull out your rapier and prepare for a brawl!", ch, staff, NULL, TO_CHAR);
   return;
}

void do_hire_crew(CHAR_DATA *ch, char *argument)
{
   CHAR_DATA *crew;
   AFFECT_DATA af;
   CHAR_DATA *check;
   int count;

   if (get_skill(ch, gsn_hire_crew) <= 0 ||
   !has_skill(ch, gsn_hire_crew))
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }
   if (check_peace(ch)) return;

   if (is_affected(ch, gsn_hire_crew))
   {
      send_to_char("You cannot hire more crew yet.\n\r", ch);
      return;
   }
   for (check = char_list; check != NULL; check = check->next)
   {
      if (IS_NPC(check))
      if ( (check->master == ch)
      && (check->pIndexData->vnum == MOB_VNUM_PIRATE_CREW))
      {
         send_to_char("You already have crew under your command.\n\r", ch);
         return;
      }
   }
   if (ch->gold < 100)
   {
      if (number_range(1, 2) == 1)
      send_to_char("You can't afford to be hiring crew right now!\n\r", ch);
      else
      send_to_char("You need some gold to hire crew!\n\r", ch);
      return;
   }
   if (number_percent() > get_skill(ch, gsn_hire_crew))
   {
      ch->gold -= 10;
      send_to_char("You try to hire some crew, but none seem available.\n\r", ch);
      check_improve(ch, gsn_hire_crew, FALSE, 1);
      return;
   }
   ch->gold -= 100;
   send_to_char("You hire some crew.\n\r", ch);
   act("$n hires some crew!\n\r", ch, NULL, NULL, TO_ROOM);

   for (count = 0; count < 2; count++)
   {
      crew = create_mobile(get_mob_index(MOB_VNUM_PIRATE_CREW) );
      crew->level = ch->level - number_range(7, 15);
      crew->max_hit = number_range(50, 100) + crew->level*5;
      crew->hit = crew->max_hit;
      crew->damroll += crew->level/2;
      crew->alignment = ch->alignment;
      af.where = TO_AFFECTS;
      af.type = gsn_hire_crew;
      af.location = 0;
      af.level = ch->level;
      af.modifier = 0;
      af.duration = 15;
      af.bitvector = 0;
      affect_to_char(ch, &af);
      char_to_room(crew, ch->in_room);

      SET_BIT(crew->affected_by, AFF_CHARM);
      add_follower(crew, ch);
      crew->leader = ch;
   }
   check_improve(ch, gsn_hire_crew, TRUE, 1);
   return;
}


void do_helper_robe(CHAR_DATA *ch, char * argument)
{
   OBJ_DATA *robe;
   char buf[MAX_STRING_LENGTH];

   if (get_skill(ch, gsn_corrupt("helper robe", &gsn_helper_robe)) <= 0 ||
   !has_skill(ch, gsn_corrupt("helper robe", &gsn_helper_robe)))
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }
   robe = create_object(get_obj_index(OBJ_VNUM_HELPER_ROBE), 0);
   robe->timer = -1;
   sprintf(buf, "%s", ch->name);
   free_string(robe->owner);
   robe->owner = str_dup(buf);
   obj_to_char(robe, ch);
   act("You create $p and prepare to help someone.", ch, robe, NULL, TO_CHAR);
   return;
}

void spell_worm_damage(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   CHAR_DATA* victim = (CHAR_DATA*) vo;
   if (target != TARGET_CHAR)
   {
      return;
   }
   act_color
   (
      "$n's faith in the Dark Father {B{6*** DESECRATES ***{n $N!",
      ch,
      NULL,
      victim,
      TO_NOTVICT
   );
   act_new_color
   (
      "$n's faith in the Dark Father {B{6*** DESECRATES ***{n you!",
      ch,
      NULL,
      victim,
      TO_VICT,
      POS_DEAD,
      TRUE
   );
   act_new_color
   (
      "Your faith in the Dark Father {B{6*** DESECRATES ***{n $N!",
      ch,
      NULL,
      victim,
      TO_CHAR,
      POS_DEAD,
      TRUE
   );
   damage(ch, victim, 15, sn, DAM_OTHER, FALSE);
   return;
}

/* Riallus's ultra portal.  Do not give to mortals */
void spell_elemental_gateway(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   OBJ_DATA* portal;
   OBJ_DATA* portal2;
   CHAR_DATA* victim;
   bool found_ch_room = FALSE;
   bool found_vict_room = FALSE;

   if (!IS_IMMORTAL(ch))
   {
      send_to_char("You do not have the power.\n\r", ch);
      return;
   }
   if
   (
      target_name[0] == '\0' ||
      !str_cmp(target_name, "close") ||
      !str_cmp(target_name, "collapse") ||
      !str_cmp(target_name, "shut") ||
      !str_cmp(target_name, "dispel")
   )
   {
      ROOM_INDEX_DATA* room_to;
      bool found;
      OBJ_DATA* pnext;
      found = FALSE;
      room_to = NULL;
      for (portal = ch->in_room->contents; portal != NULL; portal = pnext)
      {
         pnext = portal->next_content;
         if
         (
            portal->item_type == ITEM_PORTAL_NEW &&
            portal->pIndexData->vnum == OBJ_VNUM_ELEMENTAL_GATEWAY
         )
         {
            room_to = get_room_index(portal->value[0]);
            extract_obj(portal, FALSE);
            found = TRUE;
            send_to_char("You collapse the gateway.\n\r", ch);
         }
      }
      if (room_to != NULL)
      {
         for (portal = room_to->contents; portal != NULL; portal = pnext)
         {
            pnext = portal->next_content;
            if
            (
               portal->item_type == ITEM_PORTAL_NEW &&
               portal->pIndexData->vnum == OBJ_VNUM_ELEMENTAL_GATEWAY
            )
            extract_obj(portal, FALSE);
         }
      }
      if (!found)
      {
         send_to_char("There is no gateway here to close.\n\r", ch);
      }
      return;
   }

   victim = get_char_world(ch, target_name);

   if (victim == NULL)
   {
      send_to_char("You cannot find your target.\n\r", ch);
      return;
   }

   if (victim->in_room->vnum == ch->in_room->vnum)
   {
      send_to_char("Your victim is right next to you!\n\r", ch);
      return;
   }
   /*
   if
   (
   IS_SET(ch->in_room->extra_room_flags, ROOM_1212) ||
   IS_SET(victim->in_room->extra_room_flags, ROOM_1212)
   )
   {
   send_to_char
   (
   "You decide not to let mortals leave the Realm of the Dead.\n\r",
   ch
   );
   return;
   }
   */


   for (portal = ch->in_room->contents; portal != NULL; portal = portal->next_content)
   {
      if
      (
         portal->item_type == ITEM_PORTAL_NEW &&
         portal->pIndexData->vnum == OBJ_VNUM_ELEMENTAL_GATEWAY
      )
      {
         portal->timer = 5;  /* Lasts 5 ticks */
         portal->value[0] = victim->in_room->vnum;
         portal->value[1] = PULSE_VIOLENCE;  /* One round forming time */
         found_ch_room = TRUE;
      }
   }

   for
   (
      portal2 = victim->in_room->contents;
      portal2 != NULL;
      portal2 = portal2->next_content
   )
   {
      if
      (
         portal2->item_type == ITEM_PORTAL_NEW &&
         portal2->pIndexData->vnum == OBJ_VNUM_ELEMENTAL_GATEWAY
      )
      {
         portal2->timer = 5;  /* Lasts 5 ticks */
         portal2->value[0] = ch->in_room->vnum;
         portal2->value[1] = PULSE_VIOLENCE;  /* One round forming time */
         found_vict_room = TRUE;
      }
   }

   if (!found_ch_room)
   {
      portal = create_object(get_obj_index(OBJ_VNUM_ELEMENTAL_GATEWAY), 0);
      if (portal == NULL)
      {
         send_to_char
         (
            "A flaw in the realms prevents you from creating the gateway.\n\r",
            ch
         );
         bug
         (
            "elemental_gateway, elemental gateway object %d missing",
            OBJ_VNUM_ELEMENTAL_GATEWAY
         );
         return;
      }
      portal->timer = 5;  /* Lasts 5 ticks */
      portal->value[0] = victim->in_room->vnum;
      portal->value[1] = PULSE_VIOLENCE;  /* One round forming time */
      obj_to_room( portal, ch->in_room );
      add_node(portal, value_1_list);
   }
   if (!found_vict_room)
   {
      portal2 = create_object(get_obj_index(OBJ_VNUM_ELEMENTAL_GATEWAY), 0);
      if (portal2 == NULL)
      {
         send_to_char
         (
            "A flaw in the realms prevents you from creating the gateway.\n\r",
            ch
         );
         bug
         (
            "elemental_gateway, elemental gateway object %d missing",
            OBJ_VNUM_ELEMENTAL_GATEWAY
         );
         return;
      }
      portal2->timer = 5;  /* Lasts 5 ticks */
      portal2->value[0] = ch->in_room->vnum;
      portal2->value[1] = PULSE_VIOLENCE;  /* One round forming time */
      obj_to_room(portal2, victim->in_room);
      add_node(portal2, value_1_list);
   }

   send_to_char
   (
      "You call upon the power of the elements and create a portal in time-space!\n\r",
      ch
   );
   act
   (
      "$n invokes the powers of the elemental planes and creates a rift in time"
      " and space!",
      ch,
      NULL,
      NULL,
      TO_ROOM
   );
   act
   (
      "An icy wind blows and suddenly a gateway bursts open before you!",
      victim,
      NULL,
      NULL,
      TO_ALL
   );
   return;
}

int magic_power(CHAR_DATA* ch)
{
   if (!is_supplicating)
   {
      return ch->spell_power;
   }
   if (IS_IMMORTAL(ch))
   {
      return ch->spell_power + ch->holy_power * 2;
   }
   return ch->holy_power * 2;
}
