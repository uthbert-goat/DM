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

static const char rcsid[] = "$Id: handler.c,v 1.264 2004/11/04 04:34:13 maelstrom Exp $";

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "magic.h"
#include "recycle.h"
#include "tables.h"
#include "music.h"

/* command procedures needed */
char *  get_ancient_title     args((CHAR_DATA *ch));
DECLARE_DO_FUN(do_return    );
DECLARE_DO_FUN(do_dismount      );
DECLARE_DO_FUN(do_say );
DECLARE_DO_FUN(do_myell );
DECLARE_DO_FUN(do_look);

int  check_advanced_weapons   args( (CHAR_DATA* ch, int sn) );
void house_entry_trigger      args( (CHAR_DATA* ch, bool update) );
bool check_tanking            args( (CHAR_DATA *ch) );

/*
* Local functions.
*/
void    affect_modify    args( ( CHAR_DATA *ch, AFFECT_DATA *paf, bool fAdd ) );
void    affect_modify_version    args( ( CHAR_DATA *ch, AFFECT_DATA *paf, bool fAdd, int version ) );
void    update_visible   args( (CHAR_DATA* ch) );
void    site_check_get   args( (CHAR_DATA* ch, OBJ_DATA* obj, int* output, int* woutput) );
void    site_check_from  args( (CHAR_DATA* ch, OBJ_DATA* obj) );
/*
void    wear_obj_channeling args(( CHAR_DATA *ch, OBJ_DATA *obj));
void    remove_obj_channeling args(( CHAR_DATA *ch, OBJ_DATA *obj));
void     wear_obj_dragonmage_feet args((CHAR_DATA *ch, OBJ_DATA *obj));
void     wear_obj_blue_hide     args((CHAR_DATA *ch, OBJ_DATA *obj));
void     wear_obj_green_hide     args((CHAR_DATA *ch, OBJ_DATA *obj));
void remove_obj_dragonmage_feet args((CHAR_DATA *ch, OBJ_DATA *obj));
void remove_obj_blue_hide     args((CHAR_DATA *ch, OBJ_DATA *obj));
void remove_obj_green_hide     args((CHAR_DATA *ch, OBJ_DATA *obj));
void wear_obj_19002 args( (CHAR_DATA *ch, OBJ_DATA *obj) );
void remove_obj_19002 args( ( CHAR_DATA *ch, OBJ_DATA *obj) );
*/

char* get_pers(CHAR_DATA* ch, CHAR_DATA* looker)
{
   /*   if (ch == NULL) return TRUE;
   if (victim == NULL) return FALSE;*/
   if (can_see( looker, ch ))
   {
      if (IS_NPC(ch))
      {
         return ch->short_descr;
      }
      else
      {
         if (is_affected(ch, gsn_cloak_form))
         {
            return "a cloaked figure";
         }
         else
         {
            return ch->name;
         }
      }
   }
   /* Looker is null, cansee returns true. not happening */
   /* if (looker == NULL) */
   if
   (
      !IS_NPC(ch) &&
      IS_IMMORTAL(ch) &&
      (
         ch->invis_level > get_trust(looker) ||
         (
            ch != NULL &&
            looker != NULL &&
            ch->incog_level > get_trust(looker) &&
            ch->in_room != looker->in_room
         ) ||
         ch->nosee == looker ||
         (
            ch != NULL &&
            looker != NULL &&
            !IS_NPC(ch) &&
            !IS_NPC(looker) &&
            IS_IMMORTAL(ch) &&
            looker->pcdata->nosee_perm != NULL &&
            looker->pcdata->nosee_perm[0] != '\0' &&
            is_name(ch->name, looker->pcdata->nosee_perm)
         )
      )
   )
   {
      return "A divine presence";
   }
   return "someone";
}

char* get_npers(CHAR_DATA* ch, CHAR_DATA* looker)
{
   /*   if (ch == NULL) return TRUE;
   if (victim == NULL) return FALSE;*/
   if (can_see( looker, ch ))
   {
      if (IS_NPC(ch))
      {
         return ch->short_descr;
      }
      else
      {
         return ch->name;
      }
   }
   /* Looker is null, cansee returns true. not happening */
   /* if (looker == NULL) */
   if
   (
      !IS_NPC(ch) &&
      IS_IMMORTAL(ch) &&
      (
         ch->invis_level > get_trust(looker) ||
         (
            ch != NULL &&
            looker != NULL &&
            ch->incog_level > get_trust(looker) &&
            ch->in_room != looker->in_room
         ) ||
         ch->nosee == looker ||
         (
            ch != NULL &&
            looker != NULL &&
            !IS_NPC(ch) &&
            !IS_NPC(looker) &&
            IS_IMMORTAL(ch) &&
            looker->pcdata->nosee_perm != NULL &&
            looker->pcdata->nosee_perm[0] != '\0' &&
            is_name(ch->name, looker->pcdata->nosee_perm)
         )
      )
   )
   {
      return "A divine presence";
   }
   return "someone";
}


int race_adjust(CHAR_DATA *ch)
{
   int statadj;
   if (IS_IMMORTAL(ch))
   {
      return 26;
   }
   if (IS_NPC(ch))
   {
      return 25;
   }
   if (!str_cmp(class_table[ch->class].name, "cleric"))
   {
      statadj = get_curr_stat(ch, STAT_WIS);
   }
   else if (!str_cmp(class_table[ch->class].name, "paladin"))
   {
      statadj = get_curr_stat(ch, STAT_WIS) + 4;
   }
   else
   {
      statadj = get_curr_stat(ch, STAT_INT);
   }
   statadj++;
   if (statadj > 26)
   {
      statadj = 26;
   }
   return statadj;
}

/* function to compute a number based on the # of limited objects held
-Wervdon */

int hoard_score(CHAR_DATA *ch)
{
   OBJ_DATA *obj;
   int score = 0;
   int limit;

   if (ch->carrying == NULL)
   return 0;
   obj = ch->carrying;
   for (obj = ch->carrying; obj != NULL; obj = obj->next_content){
      limit = obj->pIndexData->limtotal;
      if (limit == 0) continue;
      if (obj->wear_loc == WEAR_NONE && obj->wear_flags != 1)
      {
         if (limit == 1) score += 10;
         if (limit > 1  && limit <= 10) score += 9;
         if (limit > 10 && limit <= 25) score += 8;
         if (limit > 25 && limit <= 50) score += 7;
         if (limit > 50 && limit <= 100) score += 6;
         if (limit > 100) score +=5;
      }
      if (limit != obj->pIndexData->limcount) continue;
      if (limit == 1) score += 10;
      if (limit > 1  && limit <= 10) score += 9;
      if (limit > 10 && limit <= 25) score += 8;
      if (limit > 25 && limit <= 50) score += 7;
      if (limit > 50 && limit <= 100) score += 6;
      if (limit > 100) score +=5;
   }
   return score;
}

/* Handler.c for stristr VERSION 2.0 */
char* stristr(const char* string1, const char* string2)
{
   char* temp1 = upstr_long((char*)string1);
   char* temp2 = upstr_long((char*)string2);
   char* retval = strstr(temp1, temp2);

   if (retval != NULL)
   {
      /* Return pointer in original string */
      retval += string1 - temp1;
   }
   free_string(temp1);
   free_string(temp2);
   return retval;
}

/*
   is_vulgar VERSION 3.1
      Can now take a table as input,
      can be used for vulgar filter, as well
      as cyber filter
   returns TRUE
   if the input string has any curse in it
   (checks for common words that start with curses
   but are fine), ie shitagi starts with shit
   but isn't vulgar
   returns FALSE
   if no curse is found
   Also, this function is case insensitive
*/
bool is_vulgar(char* input, VULGAR_TYPE* table)
{
   sh_int   counter;
   char*    curse;
   char*    place;
   char     whole;
   bool     skip;

   if
   (
      input == NULL ||
      input[0] == '\0'
   )
   {
      return FALSE;
   }
   if (strlen(input) > MAX_STRING_LENGTH)
   {
      /* Probably unterminated string, lets not crash */
      bug("is_vulgar called with very large (%d) string.\n\r", strlen(input));
      return FALSE;
   }

   for (counter = 0; table[counter].word != NULL; counter++)
   {
      place    = input;
      skip     = FALSE;
      while (!skip && place[0] != '\0')
      {
         curse = stristr(place, table[counter].word);
         if (curse == NULL)
         {
            skip = TRUE;  /* This vulgar word is not in the string */
         }
         else if (table[counter].strong)
         {
            return TRUE;
         }
         else
         {
            whole = *(char*)(curse - 1);
            if
            (
               curse != input &&
               (
                  (
                     whole >= 'A' &&
                     whole <= 'Z'
                  ) ||
                  (
                     whole >= 'a' &&
                     whole <= 'z'
                  )
               )
            )
            {
               skip = TRUE;  /* Not whole word.. skip */
            }
            whole = *(char*)(curse + strlen(table[counter].word));
            /* Now one later */
            if
            (
               (
                  whole >= 'A' &&
                  whole <= 'Z'
               ) ||
               (
                  whole >= 'a' &&
                  whole <= 'z'
               )
            )
            {
               skip = TRUE;  /* Not whole word.. skip */
            }
            if (!skip)  /* This instance was vulgar */
            {
               return TRUE;
            }
            skip = FALSE;  /* Don't break out of outer loop */
            place = curse;
            place++;   /* Start looking at the next character */
         }
      }
   }
   return FALSE;
}

/* friend stuff -- for NPC's mostly */
bool is_friend(CHAR_DATA *ch, CHAR_DATA *victim)
{
   if (is_same_group(ch, victim))
   return TRUE;


   if (!IS_NPC(ch))
   return FALSE;

   if (!IS_NPC(victim))
   {
      if (IS_SET(ch->off_flags, ASSIST_PLAYERS))
      return TRUE;
      else
      return FALSE;
   }

   if (IS_AFFECTED(ch, AFF_CHARM))
   return FALSE;

   if (IS_SET(ch->off_flags, ASSIST_ALL))
   return TRUE;

   if (ch->group && ch->group == victim->group)
   return TRUE;

   if (IS_SET(ch->off_flags, ASSIST_VNUM)
   &&  ch->pIndexData == victim->pIndexData)
   return TRUE;

   if (IS_SET(ch->off_flags, ASSIST_RACE) && ch->race == victim->race)
   return TRUE;

   if (IS_SET(ch->off_flags, ASSIST_ALIGN)
   &&  !IS_SET(ch->act, ACT_NOALIGN) && !IS_SET(victim->act, ACT_NOALIGN)
   &&  ((IS_GOOD(ch) && IS_GOOD(victim))
   ||     (IS_EVIL(ch) && IS_EVIL(victim))
   ||   (IS_NEUTRAL(ch) && IS_NEUTRAL(victim))))
   return TRUE;

   return FALSE;
}

/* returns number of people on an object */
int count_users(OBJ_DATA *obj)
{
   CHAR_DATA *fch;
   int count = 0;

   if (obj->in_room == NULL)
   return 0;

   for (fch = obj->in_room->people; fch != NULL; fch = fch->next_in_room)
   if (fch->on == obj)
   count++;

   return count;
}

/* returns cloaked figure or a character's name -wervdon */
char * get_name(CHAR_DATA *ch, CHAR_DATA *victim)
{
   static char buf[MAX_STRING_LENGTH];

   if
   (
      IS_TRUSTED(victim, ANGEL) ||
      victim == ch
   )
   {
      if (ch->id == ID_BUK)
      {
         sprintf(buf, "%s buk book", ch->name);
         return buf;
      }
      if
      (
         !IS_NPC(ch) &&
         IS_SET(ch->act2, PLR_IS_ANCIENT) &&
         ch->pcdata->moniker[0] != '\0'
      )
      {
         sprintf
         (
            buf,
            "%s %s",
            ch->pcdata->moniker,
            ch->name
         );
         return buf;
      }
      return ch->name;
   }

   if (is_affected(ch, gsn_cloak_form))
   {
      buf[0] = '\0';
      if
      (
         !IS_NPC(ch) &&
         ch->pcdata->moniker[0] != '\0'
      )
      {
         sprintf(buf, "%s ", ch->pcdata->moniker);
      }
      strcat(buf, "cloaked figure");
      if (is_affected(ch, gsn_toad))
      {
         strcat(buf, " toad frog");
      }
      if (is_affected(ch, gsn_feign_death))
      {
         strcat(buf, " corpse");
      }
      if (victim != NULL && victim->house == HOUSE_ANCIENT)
      {
         strcat(buf, " ");
         strcat(buf, get_ancient_title(ch));
      }
      return buf;
   }
   if (is_affected(ch, gsn_toad))
   {
      sprintf(buf, "%s toad frog", ch->name);
      return buf;
   }
   if (is_affected(ch, gsn_feign_death))
   {
      sprintf(buf, "%s corpse", ch->name);
      return buf;
   }
   if ((ch->morph_form[0] == MORPH_MIMIC) && (ch->mprog_target != NULL))
   {
      sprintf(buf, "%s %s", ch->name, ch->mprog_target->name);
      return buf;
   }
   if (ch->id == ID_BUK)
   {
      sprintf(buf, "%s buk book", ch->name);
      return buf;
   }
   return ch->name;
}

/* returns cloaked figure or a character's name -wervdon */
char * get_longname(CHAR_DATA *ch, CHAR_DATA *victim)
{

   if (victim != NULL){
      if (IS_TRUSTED(victim, ANGEL))
      return ch->name;
   }
   if (is_affected(ch, gsn_cloak_form))
   return "A cloaked figure";
   if (IS_NPC(ch))
   return capitalize(ch->short_descr);

   if ((ch->morph_form[0] == MORPH_MIMIC) && (ch->mprog_target != NULL))
   {
      if (IS_NPC(ch->mprog_target))
      return ch->mprog_target->short_descr;
      else
      return ch->mprog_target->name;
   }

   return ch->name;
}

/* takes a word and returns a subclass number -werv */
int find_subclass(CHAR_DATA *ch, char *argument)
{
   /* this part is only used for who subclass commands */
   if (ch == NULL)
   {
      if (!str_cmp("none", argument) )
      return SUBCLASS_PICKED;
      if (!str_cmp("abjurer", argument) )
      return SUBCLASS_ABJURER;
      if ( !str_cmp("enchanter", argument) )
      return SUBCLASS_ENCHANTER;
      if ( !str_cmp("illusionist", argument) )
      return SUBCLASS_ILLUSIONIST;
      if ( !str_cmp("temporal", argument))
      return SUBCLASS_TEMPORAL_MASTER;
      if (!str_cmp("barbarian", argument) )
      return SUBCLASS_BARBARIAN;
      if ( !str_cmp("beast master", argument) )
      return SUBCLASS_BEASTMASTER;
      if ( !str_cmp("beast", argument) )
      return SUBCLASS_BEASTMASTER;
      if ( !str_cmp("hunter", argument))
      return SUBCLASS_HUNTER;
      if (!str_cmp("aquamancer", argument) )
      return SUBCLASS_AQUAMANCER;
      if ( !str_cmp("aeromancer", argument) )
      return SUBCLASS_AREOMANCER;
      if ( !str_cmp("geomancer", argument) )
      return SUBCLASS_GEOMANCER;
      if ( !str_cmp("pyromancer", argument))
      return SUBCLASS_PYROMANCER;
      if ( !str_cmp("tiger", argument) )
      return SUBCLASS_SCHOOL_TIGER;
      if ( !str_cmp("snake", argument) )
      return SUBCLASS_SCHOOL_SNAKE;
      if ( !str_cmp("dragon", argument) )
      return SUBCLASS_SCHOOL_DRAGON;
      if ( !str_cmp("praying mantis", argument)
      || !str_cmp("praying", argument)
      || !str_cmp("mantis", argument) )
      return SUBCLASS_SCHOOL_MANTIS;
      if ( !str_cmp("crane", argument) )
      return SUBCLASS_SCHOOL_CRANE;
      if (!str_cmp("healing", argument))
      return SUBCLASS_PRIEST_HEALING;
      if (!str_cmp("protection", argument))
      return SUBCLASS_PRIEST_PROTECTION;
      if (!str_cmp("zealot", argument))
      return SUBCLASS_ZEALOT_FAITH;
      if (!str_cmp("ninja", argument))
      return SUBCLASS_NINJA;
      if (!str_cmp("swashbuckler", argument) ||
      !str_cmp("pirate", argument))
      return SUBCLASS_SWASHBUCKLER;
      if (!str_cmp("highwayman", argument))
      return SUBCLASS_HIGHWAYMAN;
      if ( !str_cmp("anatomist", argument))
      return SUBCLASS_ANATOMIST;
      if ( !str_cmp("necrophile", argument))
      return SUBCLASS_NECROPHILE;
      if ( !str_cmp("corruptor", argument))
      return SUBCLASS_CORRUPTOR;
      if ( !str_cmp("knight of war", argument)
      || !str_cmp("war", argument) )
      return SUBCLASS_KNIGHT_OF_WAR;
      if ( !str_cmp("knight of famine", argument)
      || !str_cmp("famine", argument) )
      return SUBCLASS_KNIGHT_OF_FAMINE;
      if ( !str_cmp("knight of pestilence", argument)
      || !str_cmp("pestilence", argument) )
      return SUBCLASS_KNIGHT_OF_PESTILENCE;
      if ( !str_cmp("knight of death", argument)
      || !str_cmp("death", argument) )
      return SUBCLASS_KNIGHT_OF_DEATH;
      return -5;
   }
   if ( !str_cmp("none", argument))
   return SUBCLASS_PICKED;

   if ( !str_cmp("wait", argument))
   return SUBCLASS_WAITING;

   switch( ch->class )
   {
      case CLASS_CHANNELER:
      if (!str_cmp("abjurer", argument) )
      return SUBCLASS_ABJURER;
      if ( !str_cmp("enchanter", argument) )
      return SUBCLASS_ENCHANTER;
      if ( !str_cmp("illusionist", argument) )
      return SUBCLASS_ILLUSIONIST;
      if ( !str_cmp("temporal", argument))
      return SUBCLASS_TEMPORAL_MASTER;
      return -5;
      case CLASS_RANGER:
      if (!str_cmp("barbarian", argument) )
      return SUBCLASS_BARBARIAN;
      if ( !str_cmp("beast master", argument) )
      return SUBCLASS_BEASTMASTER;
      if ( !str_cmp("beast", argument) )
      return SUBCLASS_BEASTMASTER;
      if ( !str_cmp("hunter", argument))
      return SUBCLASS_HUNTER;
      return -5;
      case CLASS_ELEMENTALIST:
      if (!str_cmp("aquamancer", argument) ||
      !str_cmp("water", argument) ||
      !str_cmp("aqua", argument))
      return SUBCLASS_AQUAMANCER;
      if ( !str_cmp("aeromancer", argument) ||
      !str_cmp("air", argument) ||
      !str_cmp("aero", argument) )
      return SUBCLASS_AREOMANCER;
      if ( !str_cmp("geomancer", argument) ||
      !str_cmp("earth", argument) ||
      !str_cmp("geo", argument))
      return SUBCLASS_GEOMANCER;
      if ( !str_cmp("pyromancer", argument) ||
      !str_cmp("fire", argument) ||
      !str_cmp("pyro", argument))
      return SUBCLASS_PYROMANCER;
      return -5;
      case CLASS_MONK:
      if ( !str_cmp("tiger", argument) )
      return SUBCLASS_SCHOOL_TIGER;
      if ( !str_cmp("snake", argument) )
      return SUBCLASS_SCHOOL_SNAKE;
      if ( !str_cmp("dragon", argument) )
      return SUBCLASS_SCHOOL_DRAGON;
      if ( !str_cmp("praying mantis", argument)
      || !str_cmp("praying", argument)
      || !str_cmp("mantis", argument) )
      return SUBCLASS_SCHOOL_MANTIS;
      if ( !str_cmp("crane", argument) )
      return SUBCLASS_SCHOOL_CRANE;
      return -5;
      case CLASS_THIEF:
      if (!str_cmp("ninja", argument))
      return SUBCLASS_NINJA;
      if (!str_cmp("highwayman", argument))
      return SUBCLASS_HIGHWAYMAN;
      if (!str_cmp("swashbuckler", argument) ||
      !str_cmp("pirate", argument))
      return SUBCLASS_SWASHBUCKLER;
      return -5;
      case CLASS_CLERIC:
      if (!str_cmp("healing", argument))
      return SUBCLASS_PRIEST_HEALING;
      if (!str_cmp("protection", argument))
      return SUBCLASS_PRIEST_PROTECTION;
      if (!str_cmp("zealot", argument))
      return SUBCLASS_ZEALOT_FAITH;
      return -5;
      case CLASS_NECROMANCER:
      if ( !str_cmp("anatomist", argument))
      return SUBCLASS_ANATOMIST;
      if ( !str_cmp("necrophile", argument))
      return SUBCLASS_NECROPHILE;
      if ( !str_cmp("corruptor", argument))
      return SUBCLASS_CORRUPTOR;
      return -5;
      case CLASS_ANTI_PALADIN:
      if ( !str_cmp("knight of war", argument)
      || !str_cmp("war", argument) )
      return SUBCLASS_KNIGHT_OF_WAR;
      if ( !str_cmp("knight of famine", argument)
      || !str_cmp("famine", argument) )
      return SUBCLASS_KNIGHT_OF_FAMINE;
      if ( !str_cmp("knight of pestilence", argument)
      || !str_cmp("pestilence", argument) )
      return SUBCLASS_KNIGHT_OF_PESTILENCE;
      if ( !str_cmp("knight of death", argument)
      || !str_cmp("death", argument) )
      return SUBCLASS_KNIGHT_OF_DEATH;
      return -5;
      default:
      return -5;
   }
}

/* returns specialization name */
char * get_special_name(int class, int spec)
{
   /* commented out to provide for a different way of doing this -Wervdon
   if (((spec-1) < 256) && ((spec-1) >= 0))
   return special_table[class][spec-1];
   */
   if (spec == SUBCLASS_KNIGHT_OF_WAR)
   return "Knight of War";
   if (spec == SUBCLASS_KNIGHT_OF_FAMINE)
   return "Knight of Famine";
   if (spec == SUBCLASS_KNIGHT_OF_PESTILENCE)
   return "Knight of Pestilence";
   if (spec == SUBCLASS_KNIGHT_OF_DEATH)
   return "Knight of Death";
   if (spec == SUBCLASS_ENCHANTER)
   return "Enchanter";
   if (spec == SUBCLASS_ABJURER)
   return "Abjurer";
   if (spec == SUBCLASS_ILLUSIONIST)
   return "Illusionist";
   if (spec == SUBCLASS_PRIEST_HEALING)
   return "Priest of Healing";
   if (spec == SUBCLASS_TEMPORAL_MASTER)
   return "Master of Time";
   if (spec == SUBCLASS_ZEALOT_FAITH)
   return "Zealot of the Faith";
   if (spec == SUBCLASS_PRIEST_PROTECTION)
   return "Priest of Protection";
   if (spec == SUBCLASS_GEOMANCER)
   return "Geomancer";
   if (spec == SUBCLASS_PYROMANCER)
   return "Pyromancer";
   if (spec == SUBCLASS_AQUAMANCER)
   return "Aquamancer";
   if (spec == SUBCLASS_AREOMANCER)
   return "Aeromancer";
   if (spec == SUBCLASS_ANATOMIST)
   return "Anatomist";
   if (spec == SUBCLASS_CORRUPTOR)
   return "Corruptor";
   if (spec == SUBCLASS_NECROPHILE)
   return "Necrophile";
   if (spec == SUBCLASS_TEMPLAR_NIGHT)
   return "Templar of the Night";
   if (spec == SUBCLASS_NIGHTSPAWN)
   return "Nightspawn";
   if (spec == SUBCLASS_BEASTMASTER)
   return "Beast Master";
   if (spec == SUBCLASS_BARBARIAN)
   return "Barbarian";
   if (spec == SUBCLASS_HUNTER)
   return "Hunter";
   if (spec == SUBCLASS_KNIGHT_VIRTUE)
   return "Knight of Virtue";
   if (spec == SUBCLASS_KNIGHT_ORDER)
   return "Knight of Order";
   if (spec == SUBCLASS_AVENGER_FAITH)
   return "Avenger of the Faith";
   if (spec == SUBCLASS_NINJA)
   return "Ninja";
   if (spec == SUBCLASS_HIGHWAYMAN)
   return "Highwayman";
   if (spec == SUBCLASS_SWASHBUCKLER)
   return "Swashbuckler";
   if (spec == SUBCLASS_WEAPON_MASTER)
   return "Weapon Master";
   if (spec == SUBCLASS_TACTITIAN)
   return "Tactitian";
   if (spec == SUBCLASS_JUGGERNAUT)
   return "Juggernaut";
   if (spec == SUBCLASS_SCHOOL_TIGER)
   return "Student of the Tiger";
   if (spec == SUBCLASS_SCHOOL_SNAKE)
   return "Student of the Snake";
   if (spec == SUBCLASS_SCHOOL_DRAGON)
   return "Student of the Dragon";
   if (spec == SUBCLASS_SCHOOL_MANTIS)
   return "Student of the Praying Mantis";
   if (spec == SUBCLASS_SCHOOL_CRANE)
   return "Student of the Crane";

   return NULL;
}

/* returns specialization id */
int get_special_number(int class, char * spec_name)
{
   int i;

   for (i=0;i < 16;i++)
   {
      if (!str_prefix(spec_name, special_table[class][i]))
      return i;
   }

   return 0;
}

/* returns race number */
int race_lookup (const char *name)
{
   int race;

   for ( race = 0; race_table[race].name != NULL; race++)
   {
      if (LOWER(name[0]) == LOWER(race_table[race].name[0])
      &&  !str_prefix( name, race_table[race].name))
      return race;
   }

   return 0;
}

int pencolor_lookup(const char* name)
{
   int cnt;

   for (cnt = 0; cnt < MAX_PEN_COLOR; cnt++)
   {
      if (pencolor_table[cnt] == NULL)
      {
         pencolor_table[cnt] = str_dup(name);
         return cnt;
      }
      if (!str_cmp(pencolor_table[cnt], name))
      {
         return cnt;
      }
   }
   return 0;  /* black */
}

int inkwell_lookup(const char* name)
{
   int cnt;

   for (cnt = 0; cnt < MAX_INKWELLS; cnt++)
   {
      if (inkwell_table[cnt] == NULL)
      {
         inkwell_table[cnt] = str_dup(name);
         return cnt;
      }
      if (!str_cmp(inkwell_table[cnt], name))
      {
         return cnt;
      }
   }
   return 0;  /* ink */
}

int liq_lookup (const char *name)
{
   int liq;

   for ( liq = 0; liq_table[liq].liq_name != NULL; liq++)
   {
      if (LOWER(name[0]) == LOWER(liq_table[liq].liq_name[0])
      && !str_prefix(name, liq_table[liq].liq_name))
      return liq;
   }


   return 0;
}

int search_type_lookup(const char *name, long scope)
{
   int count;

   if (name == NULL)
   {
      return -1;
   }

   for (count = 0; search_type_table[count].name != NULL; count++)
   {
      if (
            LOWER(name[0]) == LOWER(search_type_table[count].name[0]) &&
            !str_prefix(name, search_type_table[count].name) &&
            (scope & search_type_table[count].scope)
         )
      {
         return search_type_table[count].type;
      }
   }

   return -1;
}

int weapon_lookup (const char *name)
{
   int type;

   for (type = 0; weapon_table[type].name != NULL; type++)
   {
      if (LOWER(name[0]) == LOWER(weapon_table[type].name[0])
      &&  !str_prefix(name, weapon_table[type].name))
      return type;
   }

   return -1;
}

int weapon_type (const char *name)
{
   int type;

   for (type = 0; weapon_table[type].name != NULL; type++)
   {
      if (LOWER(name[0]) == LOWER(weapon_table[type].name[0])
      &&  !str_prefix(name, weapon_table[type].name))
      return weapon_table[type].type;
   }

   return WEAPON_EXOTIC;
}


int item_lookup(const char *name)
{
   int type;

   for (type = 0; item_table[type].name != NULL; type++)
   {
      if (LOWER(name[0]) == LOWER(item_table[type].name[0])
      &&  !str_prefix(name, item_table[type].name))
      return item_table[type].type;
   }

   return -1;
}

char *item_name(int item_type)
{
   int type;

   for (type = 0; item_table[type].name != NULL; type++)
   if (item_type == item_table[type].type)
   return item_table[type].name;
   return "none";
}

char *weapon_name( int weapon_type)
{
   int type;

   for (type = 0; weapon_table[type].name != NULL; type++)
   if (weapon_type == weapon_table[type].type)
   return weapon_table[type].name;
   return "exotic";
}

int attack_lookup  (const char *name)
{
   int att;

   for ( att = 0; attack_table[att].name != NULL; att++)
   {
      if (LOWER(name[0]) == LOWER(attack_table[att].name[0])
      &&  !str_prefix(name, attack_table[att].name))
      return att;
   }

   return 0;
}

/* returns a flag for wiznet */
long wiznet_lookup (const char *name)
{
   int flag;

   for (flag = 0; wiznet_table[flag].name != NULL; flag++)
   {
      if (LOWER(name[0]) == LOWER(wiznet_table[flag].name[0])
      && !str_prefix(name, wiznet_table[flag].name))
      return flag;
   }

   return -1;
}

/* returns class number */
int class_lookup (const char *name)
{
   int class;

   for ( class = 0; class < MAX_CLASS; class++)
   {
      if (LOWER(name[0]) == LOWER(class_table[class].name[0])
      &&  !str_prefix( name, class_table[class].name))
      return class;
   }

   return -1;
}

char *color_value_string( int color, bool bold, bool flash )
{
   static char buf[64];
   static sh_int COLOR_INDEX[8] = { 30, 34, 32, 31, 36, 35, 33, 37 };

   if ( flash && bold )
   sprintf( buf, "\x01B[1;%dm%s", COLOR_INDEX[color%8], ANSI_BLINK );
   else if ( flash )
   sprintf( buf, "\x01B[0;%dm%s", COLOR_INDEX[color%8], ANSI_BLINK );
   else if ( bold )
   sprintf( buf, "\x01B[1;%dm", COLOR_INDEX[color%8] );
   else
   sprintf( buf, "\x01B[0;%dm", COLOR_INDEX[color%8] );

   return buf;
}

int strlen_color( char *argument )
{
   char     *str;
   sh_int    length;

   if ( argument==NULL || argument[0]=='\0' )
   return 0;

   length=0;
   str=argument;

   while ( *str != '\0' )
   {
      if ( *str != '{' )
      {
         str++;
         length++;
         continue;
      }

      switch ( *(++str) )
      {
         default:    length+=2;     break;
         case '-':
         case 'x':     length++;     break;
         case '<':
         case '0':
         case '1':
         case '2':
         case '3':
         case '4':
         case '5':
         case '6':
         case '7':
         case 'B':
         case 'b':
         case 'F':
         case 'f':
         case 'n':             break;
      }

      str++;
   }

   return length;
}

/*
   for immunity, vulnerabiltiy, and resistant
   the 'globals' (magic and weapons) may be overriden
*/

int check_immune(CHAR_DATA* ch, int dam_type)
{
   int immune;
   int def;
   int bit;
   OBJ_DATA* obj;
   bool eq_bits[3];

   eq_bits[0] = FALSE;
   eq_bits[1] = FALSE;
   eq_bits[2] = FALSE;
   immune = -1;
   def = IS_NORMAL;

   if (dam_type == DAM_NONE)
   {
      return immune;
   }
   if (dam_type == DAM_OTHER)
   {
      if
      (
         IS_SET(ch->imm_flags, IMM_WEAPON) &&
         IS_SET(ch->imm_flags, IMM_MAGIC) &&
         (ch->vuln_flags & ~(ch->imm_flags)) == 0 &&
         (ch->res_flags & ~(ch->imm_flags)) == 0 &&
         !is_affected(ch, gsn_iceshield) &&
         !is_affected(ch, gsn_fireshield) &&
         !is_affected(ch, gsn_blizzard)
      )
      {
         return IS_IMMUNE;
      }
      else
      {
         return IS_NORMAL;
      }
   }
   if
   (
      dam_type <= 3 ||
      dam_type == DAM_NATURE
   )
   {
      if (IS_SET(ch->imm_flags, IMM_WEAPON))
      {
         def = IS_IMMUNE;
      }
      else if (IS_SET(ch->res_flags, RES_WEAPON))
      {
         def = IS_RESISTANT;
      }
      if (IS_SET(ch->vuln_flags, VULN_WEAPON))
      {
         if (def == IS_NORMAL)
         {
            def = IS_VULNERABLE;
         }
         else if (def == IS_RESISTANT)
         {
            def = IS_NORMAL;
         }
      }
   }
   else /* magical attack */
   {
      if (IS_SET(ch->imm_flags, IMM_MAGIC))
      {
         def = IS_IMMUNE;
      }
      else if (IS_SET(ch->res_flags, RES_MAGIC))
      {
         def = IS_RESISTANT;
      }
      if (IS_SET(ch->vuln_flags, VULN_MAGIC))
      {
         if (def == IS_NORMAL)
         {
            def = IS_VULNERABLE;
         }
         else if (def == IS_RESISTANT)
         {
            def = IS_NORMAL;
         }
      }
   }
   /* material vulns dont come under magic - Werv */
   if
   (
      dam_type == DAM_WOOD ||
      dam_type == DAM_SILVER ||
      dam_type == DAM_IRON ||
      dam_type == DAM_SPELLS ||
      dam_type == DAM_METAL
   )
   {
      if (check_immune(ch, DAM_OTHER) == IS_IMMUNE)
      {
         def = IS_IMMUNE;
      }
      else
      {
         def = IS_NORMAL;
      }
   }
   /* set bits to check -- VULN etc. must ALL be the same or this will fail */
   switch (dam_type)
   {
      case(DAM_BASH):
      {
         bit = IMM_BASH;
         break;
      }
      case(DAM_PIERCE):
      {
         bit = IMM_PIERCE;
         break;
      }
      case(DAM_SLASH):
      {
         bit = IMM_SLASH;
         break;
      }
      case(DAM_FIRE):
      {
         bit = IMM_FIRE;
         break;
      }
      case(DAM_COLD):
      {
         bit = IMM_COLD;
         break;
      }
      case(DAM_LIGHTNING):
      {
         bit = IMM_LIGHTNING;
         break;
      }
      case(DAM_ACID):
      {
         bit = IMM_ACID;
         break;
      }
      case(DAM_POISON):
      {
         bit = IMM_POISON;
         break;
      }
      case(DAM_NEGATIVE):
      {
         bit = IMM_NEGATIVE;
         break;
      }
      case(DAM_HOLY):
      {
         bit = IMM_HOLY;
         break;
      }
      case(DAM_ENERGY):
      {
         bit = IMM_ENERGY;
         break;
      }
      case(DAM_MENTAL):
      {
         bit = IMM_MENTAL;
         break;
      }
      case(DAM_DISEASE):
      {
         bit = IMM_DISEASE;
         break;
      }
      case(DAM_DROWNING):
      {
         bit = IMM_DROWNING;
         break;
      }
      case(DAM_LIGHT):
      {
         bit = IMM_LIGHT;
         break;
      }
      case(DAM_CHARM):
      {
         bit = IMM_CHARM;
         break;
      }
      case(DAM_SOUND):
      {
         bit = IMM_SOUND;
         break;
      }
      case(DAM_IRON):
      {
         bit = IMM_IRON;
         break;
      }
      case(DAM_SILVER):
      {
         bit = IMM_SILVER;
         break;
      }
      case(DAM_WOOD):
      {
         bit = IMM_WOOD;
         break;
      }
      case(DAM_METAL):
      {
         bit = IMM_METAL;
         break;
      }
      case(DAM_SPELLS):
      {
         bit = IMM_SPELLS;
         break;
      }
      case (DAM_HARM):
      {
         bit = IMM_HARM;
         break;
      }
      case (DAM_NATURE):
      {
         bit = IMM_NATURE;
         break;
      }
      default:
      {
         return def;
      }
   }
   /* draco resists/vulns */
   if (ch->race == grn_draconian)
   {
      switch (ch->subrace)
      {
         default:
         case (SCALE_GOLD):
         case (SCALE_GREEN):
         {
            if (dam_type == DAM_COLD)
            {
               eq_bits[IS_VULNERABLE - 1] = TRUE;
            }
            else if (dam_type == DAM_POISON)
            {
               eq_bits[IS_IMMUNE - 1] = TRUE;
            }
            break;
         }
         case (SCALE_BRASS):
         case (SCALE_RED):
         {
            if (dam_type == DAM_FIRE)
            {
               eq_bits[IS_RESISTANT - 1] = TRUE;
            }
            else if (dam_type == DAM_COLD)
            {
               eq_bits[IS_VULNERABLE - 1] = TRUE;
            }
            break;
         }
         case (SCALE_COPPER):
         case (SCALE_BLACK):
         {
            if (dam_type == DAM_ACID)
            {
               eq_bits[IS_RESISTANT - 1] = TRUE;
            }
            else if (dam_type == DAM_COLD)
            {
               eq_bits[IS_VULNERABLE - 1] = TRUE;
            }
            break;
         }
         case (SCALE_SILVER):
         case (SCALE_WHITE):
         {
            if (dam_type == DAM_COLD)
            {
               eq_bits[IS_RESISTANT - 1] = TRUE;
            }
            if (dam_type == DAM_FIRE)
            {
               eq_bits[IS_VULNERABLE - 1] = TRUE;
            }
            break;
         }
         case (SCALE_BRONZE):
         case (SCALE_BLUE):
         {
            if (dam_type == DAM_LIGHTNING)
            {
               eq_bits[IS_RESISTANT - 1] = TRUE;
            }
            if (dam_type == DAM_COLD)
            {
               eq_bits[IS_VULNERABLE - 1] = TRUE;
            }
            break;
         }
      }
   }
   /* Changeling vulns */
   if
   (
      dam_type == DAM_FIRE &&
      ch->morph_form[0] == MORPH_WOLF
   )
   {
      eq_bits[IS_VULNERABLE - 1] = TRUE;
   }
   else if
   (
      dam_type == DAM_COLD &&
      ch->morph_form[0] == MORPH_BEAR
   )
   {
      eq_bits[IS_VULNERABLE - 1] = TRUE;
   }
   if
   (
      dam_type == DAM_BASH &&
      ch->morph_form[0] == MORPH_BAT
   )
   {
      eq_bits[IS_VULNERABLE - 1] = TRUE;
   }

   if (IS_SET(ch->imm_flags, bit))
   {
      eq_bits[IS_IMMUNE - 1] = TRUE;
   }
   if (IS_SET(ch->res_flags, bit))
   {
      eq_bits[IS_RESISTANT - 1] = TRUE;
   }
   if (IS_SET(ch->vuln_flags, bit))
   {
      eq_bits[IS_VULNERABLE - 1] = TRUE;
   }
   if (eq_bits[IS_IMMUNE - 1])  /* Immune to that directly overrides */
   {
      immune = IS_IMMUNE;
   }
   else if
   (
      eq_bits[IS_RESISTANT - 1] &&
      eq_bits[IS_VULNERABLE - 1]
   )
   {
      /* No change at all. */
   }
   else if (eq_bits[IS_RESISTANT - 1])
   {
      switch (immune)
      {
         case (IS_IMMUNE):
         {
            break;
         }
         case (IS_VULNERABLE):
         {
            immune = IS_NORMAL;
            break;
         }
         default:
         {
            immune = IS_RESISTANT;
            break;
         }
      }
   }
   else if (eq_bits[IS_VULNERABLE - 1])
   {
      switch (immune)
      {
         case (IS_IMMUNE):
         {
            immune = IS_RESISTANT;
            break;
         }
         case (IS_RESISTANT):
         {
            immune = IS_NORMAL;
            break;
         }
         default:
         {
            immune = IS_VULNERABLE;
            break;
         }
      }
   }

   /* war paint stuff */
   if
   (
      immune != IS_IMMUNE &&
      is_affected(ch, gsn_warpaint)
   )
   {
      AFFECT_DATA* af;
      int pn;

      pn = 0;
      for (af = ch->affected; af != NULL; af = af->next)
      {
         if
         (
            af->type == gsn_warpaint &&
            af->where == TO_WARPAINT
         )
         {
            pn = af->modifier;
            break;
         }
      }
      switch(pn)
      {
         default:
         {
            break;
         }
         case (1):
         {
            if (dam_type == DAM_BASH)
            {
               if (immune == IS_VULNERABLE)
               {
                  immune = IS_NORMAL;
               }
               else
               {
                  immune = IS_RESISTANT;
               }
            }
            break;
         }
         case (2):
         {
            if
            (
               dam_type == DAM_SLASH ||
               dam_type == DAM_LIGHTNING
            )
            {
               if (immune == IS_VULNERABLE)
               {
                  immune = IS_NORMAL;
               }
               else
               {
                  immune = IS_RESISTANT;
               }
            }
            break;
         }
         case (3):
         {
            if (dam_type == DAM_DROWNING)
            {
               immune = IS_IMMUNE;
            }
            else if (dam_type == DAM_COLD)
            {
               if (immune == IS_VULNERABLE)
               {
                  immune = IS_NORMAL;
               }
               else
               {
                  immune = IS_RESISTANT;
               }
            }
            break;
         }
         case (4):
         {
            if (dam_type == DAM_FIRE)
            {
               if (immune == IS_VULNERABLE)
               {
                  immune = IS_NORMAL;
               }
               else
               {
                  immune = IS_RESISTANT;
               }
            }
            break;
         }
         case (16):
         {
            if (dam_type == DAM_POISON)
            {
               if (immune == IS_VULNERABLE)
               {
                  immune = IS_NORMAL;
               }
               else
               {
                  immune = IS_RESISTANT;
               }
            }
            break;
         }
      }
   }

   if
   (
      immune != IS_IMMUNE &&
      is_affected(ch, gsn_protection_heat_cold) &&
      (
         dam_type == DAM_FIRE ||
         dam_type == DAM_COLD
      )
   )
   {
      if (immune == IS_VULNERABLE)
      {
         immune = IS_NORMAL;
      }
      else
      {
         immune = IS_RESISTANT;
      }
   }

   if
   (
      IS_AFFECTED(ch, AFF_SWIM) &&
      dam_type == DAM_DROWNING
   )
   {
      immune = IS_IMMUNE;
   }

   if
   (
      immune != IS_IMMUNE &&
      is_affected(ch, gsn_divine_protect)
   )
   {
      AFFECT_DATA* af;
      for (af = ch->affected; af != NULL; af = af->next)
      {
         if
         (
            af->type == gsn_divine_protect &&
            dam_type == af->modifier
         )
         {
            if (immune == IS_VULNERABLE)
            {
               immune = IS_NORMAL;
            }
            else
            {
               immune = IS_RESISTANT;
            }
         }
      }
   }

   if
   (
      immune != IS_IMMUNE &&
      is_affected(ch, gsn_blizzard) &&
      dam_type == DAM_COLD
   )
   {
      if (immune == IS_RESISTANT)
      {
         immune = IS_NORMAL;
      }
      else
      {
         immune = IS_VULNERABLE;
      }
   }
   if
   (
      immune != IS_IMMUNE &&
      has_skill(ch, gsn_flame_form) &&
      get_skill(ch, gsn_flame_form) >= 75 &&
      dam_type == DAM_FIRE
   )
   {
      immune = IS_IMMUNE;
   }
   if
   (
      immune != IS_IMMUNE &&
      has_skill(ch, gsn_wraithform) &&
      get_skill(ch, gsn_wraithform) >= 75 &&
      dam_type == DAM_LIGHTNING
   )
   {
      immune = IS_IMMUNE;
   }
   if
   (
      immune != IS_IMMUNE &&
      has_skill(ch, gsn_water_form) &&
      get_skill(ch, gsn_water_form) >= 75 &&
      (
         dam_type == DAM_DROWNING ||
         dam_type == DAM_COLD
      )
   )
   {
      immune = IS_IMMUNE;
   }

   if
   (
      dam_type == DAM_FIRE &&
      is_affected(ch, gsn_fireshield)
   )
   {
      immune = IS_VULNERABLE;
   }
   if
   (
      immune != IS_IMMUNE &&
      dam_type == DAM_COLD &&
      is_affected(ch, gsn_fireshield)
   )
   {
      immune = IS_IMMUNE;
   }
   if
   (
      immune != IS_IMMUNE &&
      dam_type == DAM_FIRE &&
      is_affected(ch, gsn_iceshield)
   )
   {
      immune = IS_IMMUNE;
   }
   if
   (
      dam_type == DAM_COLD &&
      is_affected(ch, gsn_iceshield)
   )
   {
      immune = IS_VULNERABLE;
   }
   if
   (
      immune != IS_IMMUNE &&
      dam_type == DAM_LIGHT &&
      is_affected(ch, gsn_shroud)
   )
   {
      if (immune == IS_VULNERABLE)
      {
         immune = IS_NORMAL;
      }
      else
      {
         immune = IS_RESISTANT;
      }
   }
   if
   (
      immune != IS_IMMUNE &&
      dam_type == DAM_HOLY &&
      is_affected(ch, gsn_shroud)
   )
   {
      if (immune == IS_VULNERABLE)
      {
         immune = IS_NORMAL;
      }
      else
      {
         immune = IS_RESISTANT;
      }
   }

   if
   (
      immune != IS_IMMUNE &&
      dam_type == DAM_MENTAL &&
      is_affected(ch, gsn_armor_of_god)
   )
   {
      if (immune == IS_VULNERABLE)
      {
         immune = IS_NORMAL;
      }
      else
      {
         immune = IS_RESISTANT;
      }
   }

   if
   (
      immune != IS_IMMUNE &&
      dam_type == DAM_ACID &&
      (
         obj = get_eq_char(ch, WEAR_BODY)
      ) != NULL &&
      obj->pIndexData->vnum == OBJ_VNUM_BLACK_HIDE
   )
   {
      if (immune == IS_VULNERABLE)
      {
         immune = IS_NORMAL;
      }
      else
      {
         immune = IS_RESISTANT;
      }
   }

   if
   (
      immune != IS_IMMUNE &&
      dam_type == DAM_LIGHTNING &&
      (
         obj = get_eq_char(ch, WEAR_ABOUT)
      ) != NULL &&
      obj->pIndexData->vnum == 23751
   )
   {
      if (immune == IS_VULNERABLE)
      {
         immune = IS_NORMAL;
      }
      else
      {
         immune = IS_RESISTANT;
      }
   }
   if
   (
      immune != IS_IMMUNE &&
      dam_type == DAM_POISON &&
      (
         obj = get_eq_char(ch, WEAR_ABOUT)
      ) != NULL &&
      obj->pIndexData->vnum == 23752
   )
   {
      immune = IS_IMMUNE;
   }

   if
   (
      immune != IS_IMMUNE &&
      dam_type == DAM_LIGHTNING &&
      is_affected(ch, gsn_grounding)
   )
   {
      immune = IS_IMMUNE;
   }
   if
   (
      dam_type == DAM_FIRE &&
      immune != IS_IMMUNE &&
      (
         (
            (
               obj = get_eq_char(ch, WEAR_NECK_1)
            ) != NULL &&
            obj->pIndexData->vnum == OBJ_VNUM_RED_DRAGONSTAR
         )
      )
   )
   {
      if (immune == IS_VULNERABLE)
      {
         immune = IS_NORMAL;
      }
      else
      {
         immune = IS_RESISTANT;
      }
   }
   if
   (
      dam_type == DAM_FIRE &&
      immune != IS_IMMUNE &&
      (
         (
            (
               obj = get_eq_char(ch, WEAR_NECK_2)
            ) != NULL &&
            obj->pIndexData->vnum == OBJ_VNUM_RED_DRAGONSTAR
         )
      )
   )
   {
      if (immune == IS_VULNERABLE)
      {
         immune = IS_NORMAL;
      }
      else
      {
         immune = IS_RESISTANT;
      }
   }
   if
   (
      dam_type == DAM_ACID &&
      immune != IS_IMMUNE &&
      (
         (
            (
               obj = get_eq_char(ch, WEAR_NECK_1)
            ) != NULL &&
            obj->pIndexData->vnum == OBJ_VNUM_BLACK_DRAGONSTAR
         )
      )
   )
   {
      if (immune == IS_VULNERABLE)
      {
         immune = IS_NORMAL;
      }
      else
      {
         immune = IS_RESISTANT;
      }
   }
   if
   (
      dam_type == DAM_ACID &&
      immune != IS_IMMUNE &&
      (
         (
            (
               obj = get_eq_char(ch, WEAR_NECK_2)
            ) != NULL &&
            obj->pIndexData->vnum == OBJ_VNUM_BLACK_DRAGONSTAR
         )
      )
   )
   {
      if (immune == IS_VULNERABLE)
      {
         immune = IS_NORMAL;
      }
      else
      {
         immune = IS_RESISTANT;
      }
   }
   if
   (
      (
         dam_type == DAM_POISON ||
         dam_type == DAM_DISEASE
      ) &&
      immune != IS_IMMUNE &&
      (
         obj = get_eq_char(ch, WEAR_ABOUT)
      ) != NULL &&
      obj->pIndexData->vnum == OBJ_VNUM_CLOAK_UNDERWORLD
   )
   {
      immune = IS_IMMUNE;
   }

   /*
   Leave earthform last on the list always
   If we are going to be immune if we skip this,
   do not lower us to resistant
   */
   if
   (
      immune != IS_IMMUNE &&
      (
         def != IS_IMMUNE ||
         immune != -1
      ) &&
      has_skill(ch, gsn_earth_form) &&
      get_skill(ch, gsn_earth_form) >= 75 &&
      (
         dam_type == DAM_BASH ||
         dam_type == DAM_PIERCE ||
         dam_type == DAM_SLASH
      )
   )
   {
      immune = IS_RESISTANT;
   }

   if (immune == -1)
   {
      return def;
   }
   else
   {
      return immune;
   }
}

bool is_house(CHAR_DATA *ch)
{
   return ch->house;
}

bool is_same_house(CHAR_DATA *ch, CHAR_DATA *victim)
{
   if (house_table[ch->house].independent)
   return FALSE;
   else
   return (ch->house == victim->house);
}

/* checks mob format */
bool is_old_mob(CHAR_DATA *ch)
{
   if (ch->pIndexData == NULL)
   return FALSE;
   else if (ch->pIndexData->new_format)
   return FALSE;
   return TRUE;
}

/* for returning skill information */
int get_skill(CHAR_DATA* ch, int sn)
{
   int skill;
   int skill_override = -1;
   LIST_DATA* list;
   NODE_DATA* node;
   SKILL_MOD* mod;

   if
   (
      (
         list = ch->skill_mods
      ) != NULL
   )
   {
      /* Specific is stronger than 'all' */
      for (node = list->first; node; node = node->next)
      {
         mod = (SKILL_MOD*)node->data;
         if (mod->sn == sn)
         {
            skill_override = mod->skill_percent;
            break;
         }
         else if (mod->sn == SKILL_MOD_ALL)
         {
            skill_override = mod->skill_percent;
         }
      }
   }

   if (sn == -1) /* shorthand for level based skills */
   {
      skill = ch->level * 5 / 2;
   }
   else if
   (
      sn < 1 ||
      sn >= MAX_SKILL
   )
   {
      bug("Bad sn %d in get_skill.", sn);
      skill = 0;
   }
   else if (!IS_NPC(ch))
   {
      if (!has_skill(ch, sn))
      {
         skill = 0;
      }
      else
      {
         skill = ch->pcdata->learned[sn];
      }
   }
   else /* mobiles */
   {
      if (skill_table[sn].spell_fun != spell_null)
      {
         skill = 40 + 2 * ch->level;
      }
      else if
      (
         sn == gsn_ambush ||
         sn == gsn_moving_ambush ||
         sn == gsn_killer_instinct ||
         sn == gsn_camouflage
      )
      {
         if (IS_SET(ch->act2, ACT_RANGER))
         {
            skill = 100;
         }
         else
         {
            skill = 0;
         }
      }
      else if (sn == gsn_lunge || sn == gsn_pugil)
      {
         skill = 100;
      }
      else if
      (
         sn == gsn_hide ||
         sn == gsn_adv_hide ||
         sn == gsn_exp_hide ||
         sn == gsn_pick_lock
      )
      {
         if (IS_SET(ch->act, ACT_THIEF))
         {
            skill = ch->level * 2 + 20;
         }
         else
         {
            skill = 0;
         }
      }
      else if (sn== gsn_sneak)
      {
         skill = ch->level * 2 + 20;
      }
      else if
      (
         (
            sn == gsn_dodge &&
            IS_SET(ch->off_flags, OFF_DODGE)
         ) ||
         (
            sn == gsn_parry &&
            IS_SET(ch->off_flags, OFF_PARRY)
         )
      )
      {
         skill = ch->level * 2;
      }
      else if (sn == gsn_shield_block)
      {
         skill = 10 + 2 * ch->level;
      }
      else if (sn == gsn_palm || sn == gsn_throw)
      {
         skill = 20 + 2 * ch->level;
      }
      else if (sn == gsn_cunning_strike)
      {
         skill = 20 + 2 * ch->level;
      }
      else if (sn == gsn_critical_strike)
      {
         skill = 20 + 2 * ch->level;
      }
      else if (sn == gsn_envenom)
      {
         skill = 20 + 2 * ch->level;
      }
      else if
      (
         sn == gsn_enhanced_damage_two &&
         IS_SET(ch->act, ACT_WARRIOR)
      )
      {
         if (ch->level < 35)
         {
            skill = 0;
         }
         else
         {
            skill = (2 * ch->level - 10);
         }
      }
      else if
      (
         sn == gsn_second_attack &&
         (
            IS_SET(ch->act, ACT_WARRIOR) ||
            IS_SET(ch->act, ACT_THIEF)
         )
      )
      {
         skill = 10 + 3 * ch->level;
      }
      else if
      (
         sn == gsn_third_attack &&
         IS_SET(ch->act, ACT_WARRIOR)
      )
      {
         skill = 4 * ch->level - 40;
      }
      else if (sn == gsn_hand_to_hand)
      {
         skill = 40 + 2 * ch->level;
      }
      else if
      (
         sn == gsn_trip &&
         IS_SET(ch->off_flags, OFF_TRIP)
      )
      {
         skill = 10 + 3 * ch->level;
      }
      else if
      (
         sn == gsn_bash &&
         IS_SET(ch->off_flags, OFF_BASH)
      )
      {
         skill = 10 + 3 * ch->level;
      }
      else if
      (
         sn == gsn_ninjitsu &&
         IS_SET(ch->off_flags, OFF_NINJITSU)
      )
      {
         skill = 5 * ch->level / 2;
      }
      else if
      (
         sn == gsn_kick &&
         IS_SET(ch->off_flags, OFF_KICK)
      )
      {
         skill = 10 + 3 * ch->level;
      }
      else if
      (
         sn == gsn_dirt &&
         IS_SET(ch->off_flags, OFF_KICK_DIRT) &&
         !IS_AFFECTED(ch, AFF_CHARM)
      )
      {
         skill = 100;
      }
      else if
      (
         sn == gsn_dirt &&
         IS_AFFECTED(ch, AFF_CHARM)
      )
      {
         skill = 0;
      }
      else if
      (
         sn == gsn_disarm &&
         (
            IS_SET(ch->off_flags, OFF_DISARM) ||
            IS_SET(ch->act, ACT_WARRIOR) ||
            IS_SET(ch->act, ACT_THIEF)
         )
      )
      {
         skill = 20 + 3 * ch->level;
      }
      else if
      (
         sn == gsn_berserk &&
         IS_SET(ch->off_flags, OFF_BERSERK)
      )
      {
         skill = 3 * ch->level;
      }
      else if
      (
         sn == gsn_tail &&
         IS_SET(ch->off_flags, OFF_TAIL)
      )
      {
         skill = (3*ch->level + 20);
      }
      else if
      (
         sn == gsn_backstab &&
         IS_SET(ch->act, ACT_THIEF)
      )
      {
         skill = 20 + 2 * ch->level;
      }
      else if (sn == gsn_rescue)
      {
         skill = 40 + ch->level;
      }
      else if (sn == gsn_recall)
      {
         skill = 40 + ch->level;
      }
      else if (sn == gsn_crush)
      {
         skill = 3 * ch->level;
      }
      else if (sn == gsn_rescue)
      {
         skill = 40 + 2 * ch->level;
      }
      else if
      (
         sn == gsn_ground_control &&
         ch->level > 28
      )
      {
         skill = 40 + 2* ch->level;
      }
      else if
      (
         sn == gsn_sword ||
         sn == gsn_dagger ||
         sn == gsn_spear ||
         sn == gsn_mace ||
         sn == gsn_axe ||
         sn == gsn_flail ||
         sn == gsn_whip ||
         sn == gsn_staff ||
         sn == gsn_archery ||
         sn == gsn_polearm
      )
      {
         skill = 40 + 5 * ch->level / 2;
      }
      else
      {
         skill = 0;
      }
   }

   if (skill_override != -1)
   {
      skill = skill_override;
   }

   /*    if (ch->daze > 0)
   {
   if (skill_table[sn].spell_fun != spell_null)
   skill /= 2;
   else
   skill = 2 * skill / 3;
   } */
   if
   (
      !IS_NPC(ch) &&
      ch->class == CLASS_MONK
   )
   {
      /* Monk skill adjusting for eq */
      if (get_eq_char(ch, WEAR_HOLD) != NULL)
      {
         if
         (
            sn == gsn_fourth_attack ||
            sn == gsn_fifth_attack ||
            sn == gsn_corrupt("sixth attack", &gsn_sixth_attack)
         )
         {
            skill = 0;
         }
         else if (sn == gsn_throw)
         {
            skill = skill / 2;
         }
      }
   }

   if
   (
      !IS_NPC(ch) &&
      ch->pcdata->condition[COND_DRUNK]  > 10
   )
   {
      skill = 9 * skill / 10;
   }

   if (is_affected(ch, gsn_block_retreat))
   {
      if
      (
         sn == gsn_fourth_attack ||
         sn == gsn_fifth_attack ||
         sn == gsn_corrupt("sixth attack", &gsn_sixth_attack)
      )
      {
         skill = 0;
      }
      else if (sn == gsn_third_attack)
      {
         skill /= 2;
      }
   }

   if
   (
      is_affected(ch, gsn_forget) &&
      number_percent() > skill / 2
   )
   {
      skill /= 3;
   }

   if (is_affected(ch, gsn_void))
   {
      if
      (
         skill < 95 &&
         skill > 1
      )
      {
         skill = 95;
      }
   }

   return URANGE(0, skill, 100);
}

/* for returning weapon information */
int get_weapon_sn(CHAR_DATA *ch)
{
   OBJ_DATA *wield;
   int sn;

   wield = get_eq_char( ch, WEAR_WIELD );
   if (wield == NULL || wield->item_type != ITEM_WEAPON)
   sn = gsn_hand_to_hand;
   else switch (wield->value[0])
   {
      default :               sn = -1;                break;
      case(WEAPON_SWORD):     sn = gsn_sword;         break;
      case(WEAPON_DAGGER):    sn = gsn_dagger;        break;
      case(WEAPON_SPEAR):     sn = gsn_spear;         break;
      case(WEAPON_MACE):      sn = gsn_mace;          break;
      case(WEAPON_AXE):       sn = gsn_axe;           break;
      case(WEAPON_FLAIL):     sn = gsn_flail;         break;
      case(WEAPON_WHIP):      sn = gsn_whip;          break;
      case(WEAPON_POLEARM):   sn = gsn_polearm;       break;
      case(WEAPON_BOW):    sn = gsn_archery;    break;
      case(WEAPON_STAFF):    sn = gsn_staff;    break;
   }
   return sn;
}

int get_weapon_skill(CHAR_DATA *ch, int sn)
{
   int skill;
   OBJ_DATA * obj;
   int genocide = FALSE;

   obj = get_eq_char(ch, WEAR_WIELD);
   if (obj != NULL && obj->pIndexData->vnum == OBJ_VNUM_GENOCIDE_AXE)
   genocide = TRUE;
   obj = get_eq_char(ch, WEAR_DUAL_WIELD);
   if (obj != NULL && obj->pIndexData->vnum == OBJ_VNUM_GENOCIDE_AXE)
   genocide = TRUE;

   if ( sn == gsn_axe && genocide) return 125;

   if
   (
      sn != -1 &&  /* Everyone has exotic weapon skill */
      !has_skill(ch, sn)
   )
   {
      return 0;
   }
   /* -1 is exotic */
   if (IS_NPC(ch))
   {
      if (sn == -1)
      skill = 3 * ch->level;
      else if (sn == gsn_hand_to_hand)
      skill = 40 + 2 * ch->level;
      else
      skill = 40 + 5 * ch->level / 2;
   }
   else
   {
      if (sn == -1)
      skill = 3 * ch->level;
      else
      {
         skill = ch->pcdata->learned[sn];
         if (skill >= 100)
         {
            skill += check_advanced_weapons(ch, sn);
            return skill;
         }
      }
   }

   return URANGE(0, skill, 100);
}

int check_advanced_weapons(CHAR_DATA *ch, int sn)
{
   int sn_adv = -1;
   int sn_exp = -1;
   int bonus = 0;

   if (IS_NPC(ch))
   return 0;

   if (sn == gsn_sword){
      sn_adv = gsn_adv_sword;
      sn_exp = gsn_exp_sword;
   }
   if (sn == gsn_dagger){
      sn_adv = gsn_adv_dagger;
      sn_exp = gsn_exp_dagger;
   }
   if (sn == gsn_staff){
      sn_adv = gsn_adv_staff;
      sn_exp = gsn_exp_staff;
   }
   if (sn == gsn_spear){
      sn_adv = gsn_adv_spear;
      sn_exp = gsn_exp_spear;
   }
   if (sn == gsn_axe){
      sn_adv = gsn_adv_axe;
      sn_exp = gsn_exp_axe;
   }
   if (sn == gsn_polearm){
      sn_adv = gsn_adv_polearm;
      sn_exp = gsn_exp_polearm;
   }
   if (sn == gsn_flail){
      sn_adv = gsn_adv_flail;
      sn_exp = gsn_exp_flail;
   }
   if (sn == gsn_whip){
      sn_adv = gsn_adv_whip;
      sn_exp = gsn_exp_whip;
   }
   if (sn == gsn_mace){
      sn_adv = gsn_adv_mace;
      sn_exp = gsn_exp_mace;
   }
   if (sn == gsn_archery){
      sn_adv = gsn_adv_archery;
      sn_exp = gsn_exp_archery;;
   }
   if (sn_exp == -1)
   return 0;
   if (has_skill(ch, sn_adv))
   {
      bonus = ch->pcdata->learned[sn_adv] / 10 + 5;
   }

   if (has_skill(ch, sn_exp))
   {
      bonus += ch->pcdata->learned[sn_exp] / 10;
   }

   return bonus;
}

/* used to de-screw characters */
void reset_char(CHAR_DATA *ch)
{
   int loc, mod, stat;
   OBJ_DATA *obj;
   AFFECT_DATA *af;
   int i;

   if (IS_NPC(ch))
   return;

   if (ch->pcdata->perm_hit == 0
   || ch->pcdata->perm_mana == 0
   || ch->pcdata->perm_move == 0
   || ch->pcdata->last_level == 0)
   {
      /* do a FULL reset */
      for (loc = 0; loc < MAX_WEAR; loc++)
      {
         obj = get_eq_char(ch, loc);
         if (obj == NULL)
         continue;
         if (!obj->enchanted)
         for ( af = obj->pIndexData->affected; af != NULL; af = af->next )
         {
            mod = af->modifier;
            switch(af->location)
            {
               case APPLY_SEX:    ch->sex        -= mod;
               if (ch->sex < 0 || ch->sex >3)
               ch->sex = IS_NPC(ch) ?
               0 :
               ch->pcdata->true_sex;
               break;
               case APPLY_MANA:    ch->max_mana    -= mod;        break;
               case APPLY_HIT:    ch->max_hit    -= mod;        break;
               case APPLY_MOVE:    ch->max_move    -= mod;        break;
            }
         }

         for ( af = obj->affected; af != NULL; af = af->next )
         {
            mod = af->modifier;
            switch(af->location)
            {
               case APPLY_SEX:     ch->sex         -= mod;         break;
               case APPLY_MANA:    ch->max_mana    -= mod;         break;
               case APPLY_HIT:     ch->max_hit     -= mod;         break;
               case APPLY_MOVE:    ch->max_move    -= mod;         break;
            }
         }
      }
      /* now reset the permanent stats */
      ch->pcdata->perm_hit   = ch->max_hit;
      ch->pcdata->perm_mana  = ch->max_mana;
      ch->pcdata->perm_move  = ch->max_move;
      ch->pcdata->last_level = ch->played/3600;

      if (ch->pcdata->true_sex < 0 || ch->pcdata->true_sex > 3)
      {
         if (ch->sex > 0 && ch->sex < 3)
         ch->pcdata->true_sex    = ch->sex;
         else
         ch->pcdata->true_sex     = 0;
      }
   }

   /* now restore the character to his/her true condition */
   for (stat = 0; stat < MAX_STATS; stat++)
   ch->mod_stat[stat] = 0;

   if (ch->pcdata->true_sex < 0 || ch->pcdata->true_sex > 3)
   ch->pcdata->true_sex = 0;
   ch->sex        = ch->pcdata->true_sex;
   ch->max_hit    = ch->pcdata->perm_hit;
   ch->max_mana   = ch->pcdata->perm_mana;
   ch->max_move   = ch->pcdata->perm_move;

   for (i = 0; i < 4; i++)
   ch->armor[i]    = 100;

   ch->hitroll          = 0;
   ch->damroll          = 0;
   ch->spell_power      = 0;
   ch->holy_power       = 0;
   ch->sight            = 0;
   ch->saving_throw     = 0;
   ch->saving_maledict  = 0;
   ch->saving_breath    = 0;
   ch->saving_spell     = 0;
   ch->saving_transport = 0;

   /* now start adding back the effects */
   for (loc = 0; loc < MAX_WEAR; loc++)
   {
      obj = get_eq_char(ch, loc);
      if (obj == NULL)
      continue;
      for (i = 0; i < 4; i++)
      ch->armor[i] -= apply_ac( obj, loc, i );

      if (!obj->enchanted)
      for ( af = obj->pIndexData->affected; af != NULL; af = af->next )
      {
         mod = af->modifier;
         switch(af->location)
         {
            case APPLY_STR:        ch->mod_stat[STAT_STR]    += mod;    break;
            case APPLY_DEX:        ch->mod_stat[STAT_DEX]    += mod; break;
            case APPLY_INT:        ch->mod_stat[STAT_INT]    += mod; break;
            case APPLY_WIS:        ch->mod_stat[STAT_WIS]    += mod; break;
            case APPLY_CON:        ch->mod_stat[STAT_CON]    += mod; break;
            case APPLY_LEVEL:       ch->drain_level         += mod; break;

            case APPLY_SEX:        ch->sex            += mod; break;
            case APPLY_MANA:    ch->max_mana        += mod; break;
            case APPLY_HIT:        ch->max_hit        += mod; break;
            case APPLY_MOVE:    ch->max_move        += mod; break;
            case APPLY_AC:
            for (i = 0; i < 4; i ++)
            ch->armor[i] += mod;
            break;
            case APPLY_HEIGHT:      ch->size                += mod; break;
            case APPLY_HITROLL:    ch->hitroll        += mod; break;
            case APPLY_DAMROLL:    ch->damroll        += mod; break;

            case APPLY_SIGHT:                ch->sight += mod; break;
            case APPLY_HOLY_POWER:           ch->holy_power += mod; break;
            case APPLY_SPELL_POWER:          ch->spell_power += mod; break;
            case APPLY_SAVES:                ch->saving_throw += mod; break;
            case APPLY_SAVING_MALEDICT:      ch->saving_maledict += mod; break;
            case APPLY_SAVING_TRANSPORT:     ch->saving_transport += mod; break;
            case APPLY_SAVING_BREATH:        ch->saving_breath+= mod; break;
            case APPLY_SAVING_SPELL:         ch->saving_spell += mod; break;
            case APPLY_MORPH_FORM:           ch->morph_form[0] += mod; break;
            case APPLY_REGENERATION:         ch->regen_rate += mod; break;
            case APPLY_MN_REGENERATION:      ch->mn_regen_rate += mod; break;
            case APPLY_ALIGN:                ch->align_mod +=
                                                URANGE(
                                                   -MAX_ALIGN_MOD,
                                                   mod,
                                                   MAX_ALIGN_MOD);
                                             break;
         }
      }

      for ( af = obj->affected; af != NULL; af = af->next )
      {
         mod = af->modifier;
         switch(af->location)
         {
            case APPLY_STR:         ch->mod_stat[STAT_STR]  += mod; break;
            case APPLY_DEX:         ch->mod_stat[STAT_DEX]  += mod; break;
            case APPLY_INT:         ch->mod_stat[STAT_INT]  += mod; break;
            case APPLY_WIS:         ch->mod_stat[STAT_WIS]  += mod; break;
            case APPLY_CON:         ch->mod_stat[STAT_CON]  += mod; break;
            case APPLY_SEX:         ch->sex                 += mod; break;
            case APPLY_MANA:        ch->max_mana            += mod; break;
            case APPLY_HIT:         ch->max_hit             += mod; break;
            case APPLY_MOVE:        ch->max_move            += mod; break;
            case APPLY_LEVEL:       ch->drain_level         += mod; break;
            case APPLY_HEIGHT:      ch->size                += mod; break;

            case APPLY_AC:
            for (i = 0; i < 4; i ++)
            ch->armor[i] += mod;
            break;
            case APPLY_HITROLL:     ch->hitroll             += mod; break;
            case APPLY_DAMROLL:     ch->damroll             += mod; break;

            case APPLY_SIGHT:         ch->sight += mod; break;
            case APPLY_HOLY_POWER:         ch->holy_power += mod; break;
            case APPLY_SPELL_POWER:   ch->spell_power += mod; break;
            case APPLY_SAVES:         ch->saving_throw += mod; break;
            case APPLY_SAVING_MALEDICT:     ch->saving_maledict += mod; break;
            case APPLY_SAVING_TRANSPORT:    ch->saving_transport += mod; break;
            case APPLY_SAVING_BREATH:       ch->saving_breath += mod; break;
            case APPLY_SAVING_SPELL:        ch->saving_spell += mod; break;
            case APPLY_MORPH_FORM:  ch->morph_form[0] += mod; break;
            case APPLY_REGENERATION:    ch->regen_rate += mod; break;
            case APPLY_MN_REGENERATION:    ch->mn_regen_rate += mod; break;
            case APPLY_ALIGN:               ch->align_mod +=
                                               URANGE(
                                                  -MAX_ALIGN_MOD,
                                                  mod,
                                                  MAX_ALIGN_MOD);
                                               break;
         }
      }
   }

   /* now add back spell effects */
   for (af = ch->affected; af != NULL; af = af->next)
   {
      mod = af->modifier;
      switch(af->location)
      {
         case APPLY_STR:         ch->mod_stat[STAT_STR]  += mod; break;
         case APPLY_DEX:         ch->mod_stat[STAT_DEX]  += mod; break;
         case APPLY_INT:         ch->mod_stat[STAT_INT]  += mod; break;
         case APPLY_WIS:         ch->mod_stat[STAT_WIS]  += mod; break;
         case APPLY_CON:         ch->mod_stat[STAT_CON]  += mod; break;
         case APPLY_SEX:         ch->sex                 += mod; break;
         case APPLY_MANA:        ch->max_mana            += mod; break;
         case APPLY_HIT:         ch->max_hit             += mod; break;
         case APPLY_MOVE:        ch->max_move            += mod; break;
         case APPLY_LEVEL:       ch->drain_level         += mod; break;
         case APPLY_HEIGHT:      ch->size                += mod; break;

         case APPLY_AC:
         for (i = 0; i < 4; i ++)
         ch->armor[i] += mod;
         break;
         case APPLY_HITROLL:     ch->hitroll             += mod; break;
         case APPLY_DAMROLL:     ch->damroll             += mod; break;

         case APPLY_SIGHT:         ch->sight += mod; break;
         case APPLY_HOLY_POWER:         ch->holy_power += mod; break;
         case APPLY_SPELL_POWER:   ch->spell_power  += mod; break;
         case APPLY_SAVES:         ch->saving_throw += mod; break;
         case APPLY_SAVING_MALEDICT:     ch->saving_maledict += mod; break;
         case APPLY_SAVING_TRANSPORT:    ch->saving_transport += mod; break;
         case APPLY_SAVING_BREATH:       ch->saving_breath += mod; break;
         case APPLY_SAVING_SPELL:        ch->saving_spell += mod; break;
         case APPLY_MORPH_FORM:          ch->morph_form[0] += mod; break;
         case APPLY_REGENERATION:    ch->regen_rate += mod; break;
         case APPLY_MN_REGENERATION:    ch->mn_regen_rate += mod; break;
         case APPLY_ALIGN:              ch->align_mod +=
                                           URANGE(
                                              -MAX_ALIGN_MOD,
                                              mod,
                                              MAX_ALIGN_MOD);
                                           break;
      }
   }

   /* make sure sex is RIGHT!!!! */
   if (ch->sex < 0 || ch->sex > 3)
   {
      ch->sex = ch->pcdata->true_sex;
   }
   irv_update(ch, FALSE, TRUE);
   update_alignment(ch);
}


/*
* Retrieve a character's trusted level for permission checking.
*/
int get_trust( CHAR_DATA *ch )
{
   if ( !ch ) return 0;

   if ( ch->desc != NULL && ch->desc->original != NULL )
   ch = ch->desc->original;

   if (ch->trust)
   return ch->trust;

   if ( IS_NPC(ch) && ch->level >= LEVEL_HERO )
   return LEVEL_HERO - 1;
   else
   return ch->level;
}


/*
   Retrieve a character's age.
*/

/* command for retrieving stats */
int get_curr_stat(CHAR_DATA *ch, int stat)
{
   int max;
   int iClass;
   OBJ_DATA* brand;
   sh_int bonus = 0;
   AFFECT_DATA* paf;

   iClass = (ch->class);

   if (IS_NPC(ch) || ch->level > LEVEL_IMMORTAL)
   {
      max = 25;
   }
   else
   {
      max = pc_race_table[ch->race].max_stats[stat];

      /*
      Humans can now choose their max.
      if (ch->race == grn_human)
      {
      if
      (
      (
      iClass == CLASS_WARRIOR ||
      iClass == CLASS_PALADIN ||
      iClass == CLASS_ANTI_PALADIN ||
      iClass == CLASS_RANGER ||
      iClass == CLASS_NIGHTWALKER
      ) &&
      stat == STAT_STR
      )
      {
      max = 23;
      }
      else if
      (
      (
      iClass == CLASS_THIEF ||
      iClass == CLASS_MONK
      ) &&
      stat == STAT_DEX
      )
      {
      max = 23;
      }
      else if
      (
      iClass == CLASS_CLERIC &&
      stat == STAT_WIS
      )
      {
      max = 23;
      }
      else if
      (
      (
      iClass == CLASS_CHANNELER ||
      iClass == CLASS_NECROMANCER ||
      iClass == CLASS_ELEMENTALIST
      ) &&
      stat == STAT_INT
      )
      {
      max = 23;
      }
      }
      */

      if (ch->race == grn_draconian)
      {
         switch(ch->subrace)
         {
            default:
            case (SCALE_GREEN):
            {
               switch(stat)
               {
                  case (STAT_STR):
                  {
                     max = 23;
                     break;
                  }
                  case (STAT_INT):
                  {
                     max = 23;
                     break;
                  }
                  case (STAT_WIS):
                  {
                     max = 20;
                     break;
                  }
                  case (STAT_DEX):
                  {
                     max = 18;
                     break;
                  }
                  case (STAT_CON):
                  {
                     max = 20;
                     break;
                  }
                  default:
                  {
                     break;
                  }
               }
               break;
            }
            case (SCALE_BLUE):
            {
               switch(stat)
               {
                  case (STAT_STR):
                  {
                     max = 21;
                     break;
                  }
                  case (STAT_INT):
                  {
                     max = 24;
                     break;
                  }
                  case (STAT_WIS):
                  {
                     max = 22;
                     break;
                  }
                  case (STAT_DEX):
                  {
                     max = 18;
                     break;
                  }
                  case (STAT_CON):
                  {
                     max = 19;
                     break;
                  }
                  default:
                  {
                     break;
                  }
               }
               break;
            }
            case (SCALE_RED):
            {
               switch(stat)
               {
                  case (STAT_STR):
                  {
                     max = 24;
                     break;
                  }
                  case (STAT_INT):
                  {
                     max = 20;
                     break;
                  }
                  case (STAT_WIS):
                  {
                     max = 19;
                     break;
                  }
                  case (STAT_DEX):
                  {
                     max = 19;
                     break;
                  }
                  case (STAT_CON):
                  {
                     max = 22;
                     break;
                  }
                  default:
                  {
                     break;
                  }
               }
               break;
            }
            case (SCALE_WHITE):
            {
               switch(stat)
               {
                  case (STAT_STR):
                  {
                     max = 22;
                     break;
                  }
                  case (STAT_INT):
                  {
                     max = 19;
                     break;
                  }
                  case (STAT_WIS):
                  {
                     max = 19;
                     break;
                  }
                  case (STAT_DEX):
                  {
                     max = 22;
                     break;
                  }
                  case (STAT_CON):
                  {
                     max = 22;
                     break;
                  }
                  default:
                  {
                     break;
                  }
               }
               break;
            }
            case (SCALE_BLACK):
            {
               switch(stat)
               {
                  case (STAT_STR):
                  {
                     max = 23;
                     break;
                  }
                  case (STAT_INT):
                  {
                     max = 24;
                     break;
                  }
                  case (STAT_WIS):
                  {
                     max = 20;
                     break;
                  }
                  case (STAT_DEX):
                  {
                     max = 19;
                     break;
                  }
                  case (STAT_CON):
                  {
                     max = 18;
                     break;
                  }
                  default:
                  {
                     break;
                  }
               }
               break;
            }
            case (SCALE_SILVER):
            {
               switch(stat)
               {
                  case (STAT_STR):
                  {
                     max = 22;
                     break;
                  }
                  case (STAT_INT):
                  {
                     max = 19;
                     break;
                  }
                  case (STAT_WIS):
                  {
                     max = 24;
                     break;
                  }
                  case (STAT_DEX):
                  {
                     max = 19;
                     break;
                  }
                  case (STAT_CON):
                  {
                     max = 20;
                     break;
                  }
                  default:
                  {
                     break;
                  }
               }
               break;
            }
            case (SCALE_GOLD):
            {
               switch(stat)
               {
                  case (STAT_STR):
                  {
                     max = 23;
                     break;
                  }
                  case (STAT_INT):
                  {
                     max = 24;
                     break;
                  }
                  case (STAT_WIS):
                  {
                     max = 19;
                     break;
                  }
                  case (STAT_DEX):
                  {
                     max = 18;
                     break;
                  }
                  case (STAT_CON):
                  {
                     max = 20;
                     break;
                  }
                  default:
                  {
                     break;
                  }
               }
               break;
            }
            case (SCALE_BRONZE):
            {
               switch(stat)
               {
                  case (STAT_STR):
                  {
                     max = 23;
                     break;
                  }
                  case (STAT_INT):
                  {
                     max = 21;
                     break;
                  }
                  case (STAT_WIS):
                  {
                     max = 19;
                     break;
                  }
                  case (STAT_DEX):
                  {
                     max = 19;
                     break;
                  }
                  case (STAT_CON):
                  {
                     max = 22;
                     break;
                  }
                  default:
                  {
                     break;
                  }
               }
               break;
            }
            case (SCALE_COPPER):
            {
               switch(stat)
               {
                  case (STAT_STR):
                  {
                     max = 21;
                     break;
                  }
                  case (STAT_INT):
                  {
                     max = 20;
                     break;
                  }
                  case (STAT_WIS):
                  {
                     max = 22;
                     break;
                  }
                  case (STAT_DEX):
                  {
                     max = 19;
                     break;
                  }
                  case (STAT_CON):
                  {
                     max = 22;
                     break;
                  }
                  default:
                  {
                     break;
                  }
               }
               break;
            }
            case (SCALE_BRASS):
            {
               switch(stat)
               {
                  case (STAT_STR):
                  {
                     max = 22;
                     break;
                  }
                  case (STAT_INT):
                  {
                     max = 19;
                     break;
                  }
                  case (STAT_WIS):
                  {
                     max = 19;
                     break;
                  }
                  case (STAT_DEX):
                  {
                     max = 23;
                     break;
                  }
                  case (STAT_CON):
                  {
                     max = 21;
                     break;
                  }
                  default:
                  {
                     break;
                  }
               }
               break;
            }
         }
      }


      /*
      if
      (
      !IS_NPC(ch) &&
      (
      ch->pcdata->special == SUBCLASS_KNIGHT_OF_DEATH
      ) &&
      (
      stat == STAT_STR ||
      stat == STAT_CON
      )
      )
      {
      max = max -1;
      }
      */

      /*
      if
      (
      !IS_NPC(ch) &&
      ch->pcdata->special == SUBCLASS_KNIGHT_OF_WAR &&
      stat == STAT_INT
      )
      {
      max = max -1;
      }
      */

      /* Reward, punish, human +3 */
      if
      (
         !IS_NPC(ch) &&
         IS_SET(ch->act2, PLR_LICH) &&
         stat == STAT_CON
      )
      {
         max = 20;
      }
      if (!IS_NPC(ch))
      {
         max += ch->pcdata->stat_bonus[stat];
      }

      if
      (
         is_affected(ch, gsn_grace) &&
         stat == STAT_DEX
      )
      {
         max += 3;
      }
      if (ch->morph_form[0] == MORPH_BEAR)
      {
         if (stat == STAT_DEX)
         {
            max -= 5;
            bonus -= 5;
         }
         else if (stat == STAT_STR)
         {
            max += 5;
            bonus += 5;
         }
      }
      if
      (
         (
            is_affected(ch, gsn_enlarge) &&
            stat == STAT_STR
         ) ||
         (
            is_affected(ch, gsn_shrink) &&
            stat == STAT_DEX
         )
      )
      {
         max++;
      }
      if
      (
         (
            is_affected(ch, gsn_enlarge) &&
            stat == STAT_DEX
         ) ||
         (
            is_affected(ch, gsn_shrink) &&
            stat == STAT_STR
         )
      )
      {
         max--;
      }

      if
      (
         (brand = get_eq_char(ch, WEAR_BRAND)) &&
         brand->pIndexData->vnum == 876 &&
         weather_info.sky < SKY_RAINING &&
         IS_OUTSIDE(ch) &&
         stat == STAT_STR &&
         !IS_NPC(ch)
      )
      {
         max++;
      }

      if (stat == STAT_STR)
      {
         for (paf = ch->affected; paf != NULL; paf = paf->next)
         {
            if (paf->where == TO_AFFECTS && paf->location == APPLY_MAX_STR)
            {
               max += paf->modifier;
            }
         }
      }

      max = UMIN(max, 25);
   }

   if (stat == STAT_STR && is_affected(ch, gsn_will_power))
   {
      bonus += 10;
   }

   return URANGE(3, ch->perm_stat[stat] + ch->mod_stat[stat] + bonus, max);
}

/* command for returning max training score */
int get_max_train( CHAR_DATA *ch, int stat )
{
   int max;
   /*
   int iClass = (ch->class);
   */

   if (IS_NPC(ch) || ch->level > LEVEL_IMMORTAL)
   {
      return 25;
   }
   max = pc_race_table[ch->race].max_stats[stat];

   /*
      Humans can now choose their max.
   */

   if (ch->race == grn_draconian)
   {
      switch(ch->subrace)
      {
         default:
         case (SCALE_GREEN):
         {
            switch(stat)
            {
               case (STAT_STR):
               {
                  max = 23;
                  break;
               }
               case (STAT_INT):
               {
                  max = 23;
                  break;
               }
               case (STAT_WIS):
               {
                  max = 20;
                  break;
               }
               case (STAT_DEX):
               {
                  max = 18;
                  break;
               }
               case (STAT_CON):
               {
                  max = 20;
                  break;
               }
               default:
               {
                  break;
               }
            }
            break;
         }
         case (SCALE_BLUE):
         {
            switch(stat)
            {
               case (STAT_STR):
               {
                  max = 21;
                  break;
               }
               case (STAT_INT):
               {
                  max = 24;
                  break;
               }
               case (STAT_WIS):
               {
                  max = 22;
                  break;
               }
               case (STAT_DEX):
               {
                  max = 18;
                  break;
               }
               case (STAT_CON):
               {
                  max = 19;
                  break;
               }
               default:
               {
                  break;
               }
            }
            break;
         }
         case (SCALE_RED):
         {
            switch(stat)
            {
               case (STAT_STR):
               {
                  max = 24;
                  break;
               }
               case (STAT_INT):
               {
                  max = 20;
                  break;
               }
               case (STAT_WIS):
               {
                  max = 19;
                  break;
               }
               case (STAT_DEX):
               {
                  max = 19;
                  break;
               }
               case (STAT_CON):
               {
                  max = 22;
                  break;
               }
               default:
               {
                  break;
               }
            }
            break;
         }
         case (SCALE_WHITE):
         {
            switch(stat)
            {
               case (STAT_STR):
               {
                  max = 22;
                  break;
               }
               case (STAT_INT):
               {
                  max = 19;
                  break;
               }
               case (STAT_WIS):
               {
                  max = 19;
                  break;
               }
               case (STAT_DEX):
               {
                  max = 22;
                  break;
               }
               case (STAT_CON):
               {
                  max = 22;
                  break;
               }
               default:
               {
                  break;
               }
            }
            break;
         }
         case (SCALE_BLACK):
         {
            switch(stat)
            {
               case (STAT_STR):
               {
                  max = 23;
                  break;
               }
               case (STAT_INT):
               {
                  max = 24;
                  break;
               }
               case (STAT_WIS):
               {
                  max = 20;
                  break;
               }
               case (STAT_DEX):
               {
                  max = 19;
                  break;
               }
               case (STAT_CON):
               {
                  max = 18;
                  break;
               }
               default:
               {
                  break;
               }
            }
            break;
         }
         case (SCALE_SILVER):
         {
            switch(stat)
            {
               case (STAT_STR):
               {
                  max = 22;
                  break;
               }
               case (STAT_INT):
               {
                  max = 19;
                  break;
               }
               case (STAT_WIS):
               {
                  max = 24;
                  break;
               }
               case (STAT_DEX):
               {
                  max = 19;
                  break;
               }
               case (STAT_CON):
               {
                  max = 20;
                  break;
               }
               default:
               {
                  break;
               }
            }
            break;
         }
         case (SCALE_GOLD):
         {
            switch(stat)
            {
               case (STAT_STR):
               {
                  max = 23;
                  break;
               }
               case (STAT_INT):
               {
                  max = 24;
                  break;
               }
               case (STAT_WIS):
               {
                  max = 19;
                  break;
               }
               case (STAT_DEX):
               {
                  max = 18;
                  break;
               }
               case (STAT_CON):
               {
                  max = 20;
                  break;
               }
               default:
               {
                  break;
               }
            }
            break;
         }
         case (SCALE_BRONZE):
         {
            switch(stat)
            {
               case (STAT_STR):
               {
                  max = 23;
                  break;
               }
               case (STAT_INT):
               {
                  max = 21;
                  break;
               }
               case (STAT_WIS):
               {
                  max = 19;
                  break;
               }
               case (STAT_DEX):
               {
                  max = 19;
                  break;
               }
               case (STAT_CON):
               {
                  max = 22;
                  break;
               }
               default:
               {
                  break;
               }
            }
            break;
         }
         case (SCALE_COPPER):
         {
            switch(stat)
            {
               case (STAT_STR):
               {
                  max = 21;
                  break;
               }
               case (STAT_INT):
               {
                  max = 20;
                  break;
               }
               case (STAT_WIS):
               {
                  max = 22;
                  break;
               }
               case (STAT_DEX):
               {
                  max = 19;
                  break;
               }
               case (STAT_CON):
               {
                  max = 22;
                  break;
               }
               default:
               {
                  break;
               }
            }
            break;
         }
         case (SCALE_BRASS):
         {
            switch(stat)
            {
               case (STAT_STR):
               {
                  max = 22;
                  break;
               }
               case (STAT_INT):
               {
                  max = 19;
                  break;
               }
               case (STAT_WIS):
               {
                  max = 19;
                  break;
               }
               case (STAT_DEX):
               {
                  max = 23;
                  break;
               }
               case (STAT_CON):
               {
                  max = 21;
                  break;
               }
               default:
               {
                  break;
               }
            }
            break;
         }
      }
   }
   /* Reward, punish, human +3 */
   if
   (
      !IS_NPC(ch) &&
      IS_SET(ch->act2, PLR_LICH) &&
      stat == STAT_CON
   )
   {
      max = 20;
   }
   if (!IS_NPC(ch))
   {
      max += ch->pcdata->stat_bonus[stat];
   }
   return UMIN(max, 25);
}


/*
* Retrieve a character's carry capacity.
*/
int can_carry_n( CHAR_DATA *ch )
{
   if (IS_NPC(ch) && !IS_AFFECTED(ch, AFF_CHARM))
   return 10000;
   if ( !IS_NPC(ch) && ch->level >= LEVEL_IMMORTAL )
   return 1000;

   /*    if ( IS_NPC(ch) && IS_SET(ch->act, ACT_PET) )
   return 0;*/
   /*
      Allow everyone to hold the maximum number of worn items, + an ammount
      Do not count imm only wear slots as part of the worn items
   */
   return MAX_WEAR - WEAR_IMM_ONLY + ch->level/5 + dex_app[get_curr_stat(ch, STAT_DEX)].carry;
}



/*
* Retrieve a character's carry capacity.
*/
int can_carry_w( CHAR_DATA *ch )
{
   if (IS_NPC(ch) && !IS_AFFECTED(ch, AFF_CHARM))
   return 10000000;

   if ( !IS_NPC(ch) && ch->level >= LEVEL_IMMORTAL )
   return 10000000;

   /*    if ( IS_NPC(ch) && IS_SET(ch->act, ACT_PET) )
   return 0;*/

   /*
      Allow everyone to hold the maximum number of worn items, + an ammount
      Do not count imm only wear slots as part of the worn items
   */
   return MAX_WEAR - WEAR_IMM_ONLY + ch->level*8 + str_app[get_curr_stat(ch, STAT_STR)].carry*10;
}

/*
* See if a string is one of the names of an object.
*/

bool is_name ( char *str, char *namelist )
{
   char name[MAX_INPUT_LENGTH], part[MAX_INPUT_LENGTH];
   char *list, *string;

   /* fix crash on NULL namelist */
   if (namelist == NULL || namelist[0] == '\0')
   return FALSE;

   /* fixed to prevent is_name on "" returning TRUE */
   if (str[0] == '\0')
   return FALSE;

   string = str;
   /* we need ALL parts of string to match part of namelist */
   for ( ; ; )  /* start parsing string */
   {
      str = one_argument(str, part);

      if (part[0] == '\0' )
      return TRUE;

      /* check to see if this is part of namelist */
      list = namelist;
      for ( ; ; )  /* start parsing namelist */
      {
         list = one_argument(list, name);
         if (name[0] == '\0')  /* this name was not found */
         return FALSE;

         if (!str_cmp(string, name))
         return TRUE; /* full pattern match */

         if (!str_cmp(part, name))
         break;
      }
   }
}

bool is_exact_name(char *str, char *namelist )
{
   char name[MAX_INPUT_LENGTH];

   if (namelist == NULL)
   return FALSE;

   for ( ; ; )
   {
      namelist = one_argument( namelist, name );
      if ( name[0] == '\0' )
      return FALSE;
      if ( !str_cmp( str, name ) )
      return TRUE;
   }
}

/* enchanted stuff for eq */
void affect_enchant(OBJ_DATA *obj)
{
   /* okay, move all the old flags into new vectors if we have to */
   if (!obj->enchanted)
   {
      AFFECT_DATA *paf, *af_new;
      obj->enchanted = TRUE;

      for (paf = obj->pIndexData->affected;
      paf != NULL; paf = paf->next)
      {
         af_new = new_affect();

         af_new->next = obj->affected;
         obj->affected = af_new;

         af_new->where    = paf->where;
         af_new->type        = UMAX(0, paf->type);
         af_new->level       = paf->level;
         af_new->duration    = paf->duration;
         af_new->location    = paf->location;
         af_new->modifier    = paf->modifier;
         af_new->bitvector   = paf->bitvector;
         af_new->bitvector2  = paf->bitvector2;
      }
   }
}

/*
* Apply or remove an affect to a character.
*/
void affect_modify( CHAR_DATA *ch, AFFECT_DATA *paf, bool fAdd )
{
   affect_modify_version(ch, paf, fAdd, 0);
}

void affect_modify_1( CHAR_DATA *ch, AFFECT_DATA *paf, bool fAdd )
{
   affect_modify_version(ch, paf, fAdd, 1);
}

void affect_modify_version(CHAR_DATA *ch, AFFECT_DATA *paf, bool fAdd, int version)
{
   extern bool merc_down;
   OBJ_DATA *wield;
   int mod, i;

   mod = paf->modifier;
   affect_update(paf, version);
   /* time averaging for affects -werv
   if (fAdd && paf->duration > 1){
   if (paf->duration > 0 && (pulse_point > (2*PULSE_TICK/3)))
   paf->duration++;
   if (paf->duration > -1 && (pulse_point < (PULSE_TICK/3)))
   paf->duration--;
   }
   */
   if ( fAdd )
   {
      switch (paf->where)
      {
         case TO_AFFECTS:
         SET_BIT(ch->affected_by, paf->bitvector);
         SET_BIT(ch->affected_by2, paf->bitvector2);
         REMOVE_BIT(ch->affected_by2, AFF_HERB_SPELL);
         REMOVE_BIT(ch->affected_by2, AFF_SUPPLICATE);
         if (!IS_NPC(ch))
         {
            REMOVE_BIT(ch->affected_by2, AFF_WIZI);
         }
         if (IS_SET(paf->bitvector, AFF_FAERIE_FIRE))
         {
            un_camouflage(ch, "");
            un_invis(ch, "");
         }
         break;
         case TO_IMMUNE:
         SET_BIT(ch->imm_flags, paf->bitvector);
         break;
         case TO_RESIST:
         SET_BIT(ch->res_flags, paf->bitvector);
         break;
         case TO_VULN:
         SET_BIT(ch->vuln_flags, paf->bitvector);
         break;
      }
   }
   else
   {
      switch (paf->where)
      {
         case TO_AFFECTS:
         REMOVE_BIT(ch->affected_by, paf->bitvector);
         REMOVE_BIT(ch->affected_by2, paf->bitvector2);
         break;
         case TO_IMMUNE:
         REMOVE_BIT(ch->imm_flags, paf->bitvector);
         break;
         case TO_RESIST:
         REMOVE_BIT(ch->res_flags, paf->bitvector);
         break;
         case TO_VULN:
         REMOVE_BIT(ch->vuln_flags, paf->bitvector);
         break;
      }
      mod = 0 - mod;
   }

   switch ( paf->location )
   {
      default:
      bug( "Affect_modify: unknown location %d.", paf->location );
      {
         char buf[MAX_STRING_LENGTH];
         sprintf(buf, "[%s] at bitvector [%d] type [%d] where [%d] level [%d] duration [%d]", ch->name, paf->bitvector, paf->type, paf->where,
         paf->level, paf->duration);
         log_string(buf);
      }
      return;

      case APPLY_NONE:                        break;
      case APPLY_ACCURACY:                    break;
      case APPLY_ATTACKS:                        break;
      case APPLY_STR:           ch->mod_stat[STAT_STR]    += mod;    break;
      case APPLY_DEX:           ch->mod_stat[STAT_DEX]    += mod;    break;
      case APPLY_INT:           ch->mod_stat[STAT_INT]    += mod;    break;
      case APPLY_WIS:           ch->mod_stat[STAT_WIS]    += mod;    break;
      case APPLY_CON:           ch->mod_stat[STAT_CON]    += mod;    break;
      case APPLY_SEX:           ch->sex            += mod;    break;
      case APPLY_CLASS:                        break;
      case APPLY_LEVEL:        ch->drain_level         += mod;    break;
      case APPLY_AGE:                        break;
      case APPLY_HEIGHT:      ch->size                    += mod; break;
      case APPLY_WEIGHT:                        break;
      case APPLY_MANA:          ch->max_mana        += mod;    break;
      case APPLY_HIT:           ch->max_hit        += mod;    break;
      case APPLY_MOVE:          ch->max_move        += mod;    break;
      case APPLY_GOLD:                        break;
      case APPLY_EXP:                        break;
      case APPLY_SIZE:          ch->size                  += mod; break;
      case APPLY_AC:
      for (i = 0; i < 4; i ++)
      ch->armor[i] += mod;
      break;
      case APPLY_HITROLL:       ch->hitroll        += mod;    break;
      case APPLY_DAMROLL:       ch->damroll        += mod;    break;
      case APPLY_SIGHT:          ch->sight           += mod;    break;
      case APPLY_HOLY_POWER:         ch->holy_power += mod; break;
      case APPLY_SPELL_POWER: ch->spell_power             += mod; break;
      case APPLY_SAVES:   ch->saving_throw        += mod;    break;
      case APPLY_SAVING_MALEDICT:  ch->saving_maledict      += mod; break;
      case APPLY_SAVING_TRANSPORT: ch->saving_transport     += mod; break;
      case APPLY_SAVING_BREATH:    ch->saving_breath        += mod; break;
      case APPLY_SAVING_SPELL:     ch->saving_spell         += mod; break;
      case APPLY_SPELL_AFFECT:                      break;
      case APPLY_MORPH_FORM:    ch->morph_form[0]     += mod; break;
      case APPLY_REGENERATION:    ch->regen_rate         += mod; break;
      case APPLY_MN_REGENERATION:    ch->mn_regen_rate         += mod; break;
      case APPLY_MAX_STR:         break;
      case APPLY_ALIGN:           ch->align_mod +=
                                        URANGE(
                                           -MAX_ALIGN_MOD,
                                           mod,
                                           MAX_ALIGN_MOD);
                                        break;
   }

   /*
   * Check for weapon wielding.
   * Guard against recursion (for weapons with affects).
   */
   if
   (
      !IS_NPC(ch) &&
      ch->on_line &&
      ch->quittime != -10 &&
      !merc_down &&
      (
         wield = get_eq_char(ch, WEAR_WIELD)
      ) != NULL &&
      get_obj_weight(wield) > (str_app[get_curr_stat(ch, STAT_STR)].wield * 10)
   )
   {
      static int depth;

      if ( depth == 0 )
      {
         depth++;
         act( "You drop $p.", ch, wield, NULL, TO_CHAR );
         act( "$n drops $p.", ch, wield, NULL, TO_ROOM );
         obj_from_char( wield );
         obj_to_room( wield, ch->in_room );
         depth--;
      }
   }
   if (!IS_NPC(ch) && (ch->quittime != -10) && !merc_down
   && ( wield = get_eq_char(ch, WEAR_DUAL_WIELD)) != NULL
   && get_obj_weight(wield) > (str_app[get_curr_stat(ch, STAT_STR)].wield*10))
   {
      static int depth;

      if ( depth == 0 )
      {
         depth++;
         act( "You drop $p.", ch, wield, NULL, TO_CHAR );
         act( "$n drops $p.", ch, wield, NULL, TO_ROOM );
         obj_from_char( wield );
         obj_to_room( wield, ch->in_room );
         depth--;
      }
   }
   if ((wield = get_eq_char(ch, WEAR_DUAL_WIELD)) != NULL)
   {
      if (get_eq_char(ch, WEAR_WIELD) == NULL)
      {
         unequip_char(ch, wield);
         equip_char(ch, wield, WEAR_WIELD);
      }
   }
   if ((ch->mprog_target != NULL) && (ch->morph_form[0] != MORPH_MIMIC) &&
   !IS_NPC(ch)) ch->mprog_target = NULL;

   update_alignment(ch);
   return;
}

ROOM_AFFECT_DATA * affect_find_room(ROOM_INDEX_DATA *room, int sn)
{
   ROOM_AFFECT_DATA * raf;
   for ( raf = room->affected; raf != NULL; raf = raf->next )
   {
      if (raf->type == sn)
      return raf;
   }
   return NULL;
}

/* find an effect in an affect list */
AFFECT_DATA  *affect_find(AFFECT_DATA *paf, int sn)
{
   AFFECT_DATA *paf_find;

   for ( paf_find = paf; paf_find != NULL; paf_find = paf_find->next )
   {
      if ( paf_find->type == sn )
      return paf_find;
   }

   return NULL;
}

/* fix object affects when removing one */
void affect_check(CHAR_DATA *ch, int where, int vector)
{
   AFFECT_DATA *paf;
   OBJ_DATA *obj;

   if (where == TO_OBJECT || where == TO_WEAPON || vector == 0)
   return;

   if ( where == TO_AFFECTS )
   {
      switch( vector )
      {
         case AFF_FLYING:
         if ( ch->race == grn_draconian)
         SET_BIT(ch->affected_by, AFF_FLYING);
         break;
         default:
         break;
      }
   }

   for (paf = ch->affected; paf != NULL; paf = paf->next)
   if (paf->where == where && paf->bitvector == vector)
   {
      switch (where)
      {
         case TO_AFFECTS:
         SET_BIT(ch->affected_by, vector);
         break;
         case TO_IMMUNE:
         SET_BIT(ch->imm_flags, vector);
         break;
         case TO_RESIST:
         SET_BIT(ch->res_flags, vector);
         break;
         case TO_VULN:
         SET_BIT(ch->vuln_flags, vector);
         break;
      }
      return;
   }

   for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
   {
      if (obj->wear_loc == -1)
      continue;

      for (paf = obj->affected; paf != NULL; paf = paf->next)
      if (paf->where == where && paf->bitvector == vector)
      {
         switch (where)
         {
            case TO_AFFECTS:
            SET_BIT(ch->affected_by, vector);
            break;
            case TO_IMMUNE:
            SET_BIT(ch->imm_flags, vector);
            break;
            case TO_RESIST:
            SET_BIT(ch->res_flags, vector);
            break;
            case TO_VULN:
            SET_BIT(ch->vuln_flags, vector);

         }
         return;
      }

      if (obj->enchanted)
      continue;

      for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
      if (paf->where == where && paf->bitvector == vector)
      {
         switch (where)
         {
            case TO_AFFECTS:
            SET_BIT(ch->affected_by, vector);
            break;
            case TO_IMMUNE:
            SET_BIT(ch->imm_flags, vector);
            break;
            case TO_RESIST:
            SET_BIT(ch->res_flags, vector);
            break;
            case TO_VULN:
            SET_BIT(ch->vuln_flags, vector);
            break;
         }
         return;
      }
   }

   for (paf = ch->affected; paf != NULL; paf = paf->next)
   {
      switch (paf->where)
      {
         case TO_AFFECTS:
         SET_BIT(ch->affected_by, paf->bitvector);
         break;
         case TO_IMMUNE:
         SET_BIT(ch->imm_flags, paf->bitvector);
         break;
         case TO_RESIST:
         SET_BIT(ch->res_flags, paf->bitvector);
         break;
         case TO_VULN:
         SET_BIT(ch->vuln_flags, paf->bitvector);
         break;
      }
   }
   return;
}

/* Update old affects to new form */
void affect_update(AFFECT_DATA *paf, int version)
{
   if (paf == NULL)
   {
      bug("affect_update called with null affect", 0);
      return;
   }
   if (version < 0)
   {
      bug("affect_update called with negative version '%d'", version);
      return;
   }
   if (version > AFFECT_CURRENT_VERSION)
   {
      bug("affect_update called with too high a version '%d'", version);
      return;
   }
   /*
   Handle Versions
   Make them go in increasing order
   */
   /* Version 1 added bitvector2 field */
   if (version < 1)
   {
      paf->bitvector2 = 0;
   }
   if (version < 2)
   {
      VALIDATE(paf);
   }
}

/*
* Give an affect to a char, handles old affects that didn't use
* bitvector2
*/
void affect_to_char( CHAR_DATA *ch, AFFECT_DATA *paf )
{
   affect_to_char_version(ch, paf, 0);  /* Version 0 is pre bitvector2 */
}

void affect_to_char_1( CHAR_DATA *ch, AFFECT_DATA *paf )
{
   affect_to_char_version(ch, paf, 1);  /* Version 1 includes bitvector2 */
}

/*
* Give an affect to a char, handles affect versions
*/
void affect_to_char_version(CHAR_DATA *ch, AFFECT_DATA *paf, int version)
{
   AFFECT_DATA *paf_new;
   OBJ_DATA *brand;
   int bonus;

   if
   (
      paf->type < 0 ||
      paf->type >= MAX_SKILL
   )
   {
      sprintf
      (
         log_buf,
         "affect_to_char: bad affect: %d on [%s] [%d]",
         paf->type,
         ch->name,
         (
            IS_NPC(ch) ?
            ch->pIndexData->vnum :
            0
         )
      );
      bug_trust(log_buf, 0, get_trust(ch));
      return;
   }
   /* Earthbind */
   /*
   Earthbind is handled differently
   if
   (
      is_affected(ch, gsn_earthbind) &&
      IS_SET(paf->bitvector, AFF_FLYING) &&
      paf->where == TO_AFFECTS
   )
   {
      send_to_char("You can't fly you are earthbound!\n\r", ch);
      return;
   }
   */
   /* Gryleth's brand */
   bonus = 0;
   brand = get_eq_char(ch, WEAR_BRAND);
   if (brand != NULL && brand->pIndexData->vnum == 26309)
   {
      if (paf->type == gsn_steel_nerves ||
      paf->type == gsn_protection_heat_cold ||
      paf->type == gsn_iron_will ||
      paf->type == gsn_palm ||
      (paf->location >= APPLY_STR && paf->location <=APPLY_CON && paf->modifier > 0) ||
      (paf->location >= APPLY_MANA && paf->location <= APPLY_MOVE && paf->modifier > 0) ||
      (paf->location == APPLY_AC && paf->modifier < 0) ||
      (paf->location >= APPLY_HITROLL && paf->location <= APPLY_DAMROLL && paf->modifier > 0) ||
      (paf->location >= APPLY_SAVES && paf->location <= APPLY_SAVING_SPELL && paf->modifier < 0) ||
      (paf->location >= APPLY_REGENERATION && paf->location <= APPLY_MN_REGENERATION && paf->modifier > 0) ||
      (paf->where == TO_AFFECTS && IS_SET(paf->bitvector, AFF_INVISIBLE)) ||
      (paf->where == TO_AFFECTS && IS_SET(paf->bitvector, AFF_DETECT_EVIL)) ||
      (paf->where == TO_AFFECTS && IS_SET(paf->bitvector, AFF_DETECT_INVIS)) ||
      (paf->where == TO_AFFECTS && IS_SET(paf->bitvector, AFF_DETECT_MAGIC)) ||
      (paf->where == TO_AFFECTS && IS_SET(paf->bitvector, AFF_DETECT_HIDDEN)) ||
      (paf->where == TO_AFFECTS && IS_SET(paf->bitvector, AFF_DETECT_GOOD)) ||
      (paf->where == TO_AFFECTS && IS_SET(paf->bitvector, AFF_SANCTUARY)) ||
      (paf->where == TO_AFFECTS && IS_SET(paf->bitvector, AFF_INFRARED)) ||
      (paf->where == TO_AFFECTS && IS_SET(paf->bitvector, AFF_ACUTE_VISION)) ||
      (paf->where == TO_AFFECTS && IS_SET(paf->bitvector, AFF_PROTECT_EVIL)) ||
      (paf->where == TO_AFFECTS && IS_SET(paf->bitvector, AFF_PROTECT_GOOD)) ||
      (paf->where == TO_AFFECTS && IS_SET(paf->bitvector, AFF_SNEAK)) ||
      (paf->where == TO_AFFECTS && IS_SET(paf->bitvector, AFF_HIDE)) ||
      (paf->where == TO_AFFECTS && IS_SET(paf->bitvector, AFF_FLYING)) ||
      (paf->where == TO_AFFECTS && IS_SET(paf->bitvector, AFF_PASS_DOOR)) ||
      (paf->where == TO_AFFECTS && IS_SET(paf->bitvector, AFF_HASTE)) ||
      (paf->where == TO_AFFECTS && IS_SET(paf->bitvector, AFF_BERSERK)) ||
      (paf->where == TO_AFFECTS && IS_SET(paf->bitvector, AFF_SWIM)) ||
      (paf->where == TO_AFFECTS && IS_SET(paf->bitvector, AFF_REGENERATION)) ||
      (paf->where == TO_AFFECTS && IS_SET(paf->bitvector, AFF_CAMOUFLAGE)))
      bonus = 3;
   }

   if ( paf->type == 0 )
   {
      sprintf(log_buf,  "Attempt to place reserved affect on ch [%s].", ch->name );
      log_string (log_buf);
      return;
   }

   paf_new = new_affect();

   *paf_new        = *paf;
   affect_update(paf_new, version);
   paf_new->duration += bonus;
   paf_new->caster     = NULL;
   paf_new->next    = ch->affected;
   ch->affected    = paf_new;

   /* Already ran affect_update, so we are at the newest version */
   affect_modify_version(ch, paf_new, TRUE, AFFECT_CURRENT_VERSION);
   return;
}

/* give an affect to an object */
void affect_to_obj(OBJ_DATA *obj, AFFECT_DATA *paf)
{
   affect_to_obj_version(obj, paf, 0);
}

void affect_to_obj_1(OBJ_DATA *obj, AFFECT_DATA *paf)
{
   affect_to_obj_version(obj, paf, 1);
}

void affect_to_obj_version(OBJ_DATA *obj, AFFECT_DATA *paf, int version)
{
   AFFECT_DATA *paf_new;

   if
   (
      paf->type < 0 ||
      paf->type >= MAX_SKILL
   )
   {
      sprintf
      (
         log_buf,
         "affect_to_obj: bad affect: %d in [%s] [%d]",
         paf->type,
         obj->short_descr,
         obj->pIndexData->vnum
      );
      bug_trust(log_buf, 0, get_trust(obj->carried_by));
      /* get_trust handles NULL */
      return;
   }
   paf_new = new_affect();

   *paf_new        = *paf;
   affect_update(paf_new, version);
   paf_new->caster     = NULL;
   paf_new->next    = obj->affected;
   obj->affected    = paf_new;

   /* apply any affect vectors to the object's extra_flags */
   if (paf->bitvector)
   switch (paf->where)
   {
      case TO_OBJECT:
      SET_BIT(obj->extra_flags, paf->bitvector);
      break;
      case TO_WEAPON:
      if (obj->item_type == ITEM_WEAPON)
      SET_BIT(obj->value[4], paf->bitvector);
      break;
   }


   return;
}

/* give an affect to a room (a work in progess wervdon)*/
void affect_to_room(ROOM_INDEX_DATA *room, ROOM_AFFECT_DATA *paf)
{
   ROOM_AFFECT_DATA *paf_new;
   ROOM_AFFECT_DATA *temp_ptr;

   if
   (
      paf->type < 0 ||
      paf->type >= MAX_SKILL
   )
   {
      sprintf
      (
         log_buf,
         "affect_to_room: bad affect: %d on [%s] [%d]",
         paf->type,
         room->name,
         room->vnum
      );
      bug(log_buf, 0);
      return;
   }
   paf_new = new_room_affect();

   temp_ptr = paf_new->next_global;

   *paf_new        = *paf;
   VALIDATE(paf_new);
   paf_new->next_global    = temp_ptr;
   paf_new->next    = room->affected;
   room->affected    = paf_new;
   paf_new->room        = room;

   /* simple hack to avoid future bugs */
   if (IS_SET(room->room_flags, paf->bitvector) && paf->where ==TO_ROOM)
   paf_new->bitvector = 0;
   if (IS_SET(room->extra_room_flags, paf->bitvector)
   &&paf->where==TO_ROOM_EXTRA)
   paf_new->bitvector = 0;

   if (paf->where == TO_ROOM_SECTOR){
      paf_new->sector_old = room->sector_type;
      if (room->affected != NULL)
      {
         temp_ptr = room->affected;
         while(temp_ptr != NULL)
         {
            if (temp_ptr->where == TO_ROOM_SECTOR)
            paf_new->sector_old = temp_ptr->sector_old;
            temp_ptr = temp_ptr->next;
         }
      }
      room->sector_type = paf->sector_new;
   }
   if (paf->bitvector)
   switch (paf->where)
   {
      default:
      case TO_ROOM:
      SET_BIT(room->room_flags, paf->bitvector);
      break;
      case TO_ROOM_EXTRA:
      SET_BIT(room->extra_room_flags, paf->bitvector);
      break;
   }


   return;
}

void update_visible(CHAR_DATA* ch)
{
   ROOM_AFFECT_DATA* paf;
   sh_int sector;
   sh_int sector_old;

   if (ch == NULL || ch->in_room == NULL)
   {
      return;
   }
   for (paf = ch->in_room->affected; paf; paf = paf->next)
   {
      if (paf->where == TO_ROOM_SECTOR)
      {
         break;
      }
   }
   sector = ch->in_room->sector_type;
   if (paf == NULL)
   {
      sector_old = sector;
   }
   else
   {
      sector_old = paf->sector_old;
   }

   if
   (
      is_affected(ch, gsn_submerge) &&
      sector != SECT_WATER_SWIM &&
      sector != SECT_UNDERWATER  &&
      sector != SECT_WATER_NOSWIM &&
      sector_old != SECT_WATER_SWIM &&
      sector_old != SECT_UNDERWATER  &&
      sector_old != SECT_WATER_NOSWIM
   )
   {
      send_to_char("You emerge from the receding waves.\n\r", ch);
      affect_strip(ch, gsn_submerge);
   }
   if
   (
      IS_AFFECTED(ch, AFF_CAMOUFLAGE) &&
      sector != SECT_FOREST &&
      sector != SECT_MOUNTAIN &&
      sector != SECT_HILLS &&
      sector != SECT_UNDERGROUND &&
      sector_old != SECT_FOREST &&
      sector_old != SECT_MOUNTAIN &&
      sector_old != SECT_HILLS &&
      sector_old != SECT_UNDERGROUND
   )
   {
      un_camouflage(ch, "");
   }
   if (IS_AFFECTED(ch, AFF_HIDE))
   {
      if
      (
         sector != SECT_CITY &&
         sector != SECT_INSIDE &&
         sector != SECT_UNDERGROUND &&
         sector_old != SECT_CITY &&
         sector_old != SECT_INSIDE &&
         sector_old != SECT_UNDERGROUND &&
         (
            (
               (
                  has_skill(ch, gsn_entrench) &&
                  get_skill(ch, gsn_entrench) > 0
               ) &&
               (
                  sector != SECT_FOREST ||
                  sector != SECT_MOUNTAIN ||
                  sector != SECT_HILLS ||
                  sector_old != SECT_FOREST ||
                  sector_old != SECT_MOUNTAIN ||
                  sector_old != SECT_HILLS
               )
            ) ||
            (
               !has_skill(ch, gsn_entrench) ||
               get_skill(ch, gsn_entrench) < 1
            )
         )
      )
      {
         un_hide(ch, "");
      }
   }
   if
   (
      is_affected(ch, gsn_earthfade) &&
      (
         sector == SECT_WATER_SWIM ||
         sector == SECT_UNDERWATER ||
         sector == SECT_WATER_NOSWIM ||
         sector == SECT_AIR ||
         sector_old == SECT_WATER_SWIM ||
         sector_old == SECT_UNDERWATER ||
         sector_old == SECT_WATER_NOSWIM ||
         sector_old == SECT_AIR
      )
   )
   {
      un_earthfade(ch, NULL);
   }
   if
   (
      is_affected(ch, gsn_forest_blending) &&
      sector != SECT_FOREST &&
      sector_old != SECT_FOREST
   )
   {
      un_forest_blend(ch);
   }
}

void affect_remove_room( ROOM_INDEX_DATA *room, ROOM_AFFECT_DATA *paf)
{
   if ( room->affected == NULL )
   {
      bug( "Affect_remove_room: no affect.", 0 );
      return;
   }


   if (paf->where == TO_ROOM_SECTOR && room->sector_type != paf->sector_old)
   {
      CHAR_DATA* ch;
      ROOM_AFFECT_DATA* raf;
      ROOM_AFFECT_DATA* raf_next;

      room->sector_type = paf->sector_old;
      for (ch = room->people; ch != NULL; ch = ch->next_in_room)
      {
         if
         (
            is_affected(ch, gsn_submerge) &&
            room->sector_type != SECT_WATER_SWIM &&
            room->sector_type != SECT_UNDERWATER  &&
            room->sector_type != SECT_WATER_NOSWIM
         )
         {
            send_to_char("You emerge from the receding waves.\n\r", ch);
            affect_strip(ch, gsn_submerge);
         }
         if
         (
            IS_AFFECTED(ch, AFF_CAMOUFLAGE) &&
            room->sector_type != SECT_FOREST &&
            room->sector_type != SECT_MOUNTAIN &&
            room->sector_type != SECT_HILLS &&
            room->sector_type != SECT_UNDERGROUND
         )
         {
            un_camouflage(ch, "");
         }
         if (IS_AFFECTED(ch, AFF_HIDE))
         {
            if
            (
               room->sector_type != SECT_CITY &&
               room->sector_type != SECT_INSIDE &&
               room->sector_type != SECT_UNDERGROUND &&
               (
                  (
                     (
                        has_skill(ch, gsn_entrench) &&
                        get_skill(ch, gsn_entrench) > 0
                     ) &&
                     (
                        room->sector_type != SECT_FOREST ||
                        room->sector_type != SECT_MOUNTAIN ||
                        room->sector_type != SECT_HILLS
                     )
                  ) ||
                  (
                     !has_skill(ch, gsn_entrench) ||
                     get_skill(ch, gsn_entrench) < 1
                  )
               )
            )
            {
               un_hide(ch, "");
            }
         }
         if
         (
            is_affected(ch, gsn_earthfade) &&
            (
               room->sector_type == SECT_WATER_SWIM ||
               room->sector_type == SECT_UNDERWATER ||
               room->sector_type == SECT_WATER_NOSWIM ||
               room->sector_type == SECT_AIR
            )
         )
         {
            un_earthfade(ch, NULL);
         }
         if (is_affected(ch, gsn_forest_blending) && room->sector_type != SECT_FOREST)
         {
            un_forest_blend(ch);
         }
      }
      for (raf = room->affected; raf != NULL; raf = raf_next)
      {
         raf_next = raf->next;
         if (raf->type == gsn_trapmaking)
         {
            switch (raf->modifier)
            {
               default:  /* 0, 1, 2 */
               {
                  if
                  (
                     room->sector_type != SECT_FOREST &&
                     room->sector_type != SECT_HILLS &&
                     room->sector_type != SECT_MOUNTAIN
                  )
                  {
                     if
                     (
                        raf->type > 0 &&
                        skill_table[raf->type].msg_obj &&
                        paf->room->people != NULL
                     )
                     {
                        act
                        (
                           skill_table[raf->type].msg_obj,
                           paf->room->people,
                           NULL,
                           NULL,
                           TO_ALL
                        );
                     }
                     affect_remove_room(room, raf);
                  }
                  break;
               }
               case 3:
               {
                  if
                  (
                     room->sector_type != SECT_FOREST &&
                     room->sector_type != SECT_HILLS &&
                     room->sector_type != SECT_UNDERGROUND &&
                     room->sector_type != SECT_DESERT &&
                     room->sector_type != SECT_FIELD &&
                     room->sector_type != SECT_MOUNTAIN
                  )
                  {
                     if
                     (
                        raf->type > 0 &&
                        skill_table[raf->type].msg_obj &&
                        paf->room->people != NULL
                     )
                     {
                        act
                        (
                           skill_table[raf->type].msg_obj,
                           paf->room->people,
                           NULL,
                           NULL,
                           TO_ALL
                        );
                     }
                     affect_remove_room(room, raf);
                  }
                  break;
               }
            }
         }
      }
   }

   if (paf->bitvector)
   switch( paf->where)
   {
      default:
      case TO_ROOM:
      REMOVE_BIT(room->room_flags, paf->bitvector);
      break;
      case TO_ROOM_EXTRA:
      REMOVE_BIT(room->extra_room_flags, paf->bitvector);
      break;
   }


   if ( paf == room->affected )
   {
      room->affected    = paf->next;
   }
   else
   {
      ROOM_AFFECT_DATA *prev;

      for ( prev = room->affected; prev != NULL; prev = prev->next )
      {
         if ( prev->next == paf )
         {
            prev->next = paf->next;
            break;
         }
      }

      if ( prev == NULL )
      {
         bug( "Affect_remove_room: cannot find paf.", 0 );
         return;
      }
   }

   free_room_affect(paf);

   return;
}


/*
   set a bit in bitvector2
   from an affect list
   returns true if there were any
   to change
*/
bool affect_set_bit2(AFFECT_DATA *paf, sh_int sn, int bit2)
{
   AFFECT_DATA *paf_find;
   bool changed = FALSE;

   for (paf_find = paf; paf_find != NULL; paf_find = paf_find->next)
   {
      if (paf_find->type == sn)
      {
         SET_BIT(paf_find->bitvector2, bit2);
         changed = TRUE;
      }
   }
   return changed;
}

/*
* Remove an affect from a char.
*/
void affect_remove( CHAR_DATA *ch, AFFECT_DATA *paf )
{
   int where;
   int vector;

   if ( ch->affected == NULL )
   {
      bug( "Affect_remove: no affect.", 0 );
      return;
   }
   else if (paf->type == gsn_shapeshift && paf->location == APPLY_MORPH_FORM)
   {
      switch_desc(ch, 0);
   }
   affect_modify_version(ch, paf, FALSE, AFFECT_CURRENT_VERSION);
   where = paf->where;
   vector = paf->bitvector;

   if ( paf == ch->affected )
   {
      ch->affected    = paf->next;
   }
   else
   {
      AFFECT_DATA *prev;

      for ( prev = ch->affected; prev != NULL; prev = prev->next )
      {
         if ( prev->next == paf )
         {
            prev->next = paf->next;
            break;
         }
      }

      if ( prev == NULL )
      {
         bug( "Affect_remove: cannot find paf.", 0 );
         return;
      }
   }

   free_affect(paf);

   affect_check(ch, where, vector);
   return;
}

void affect_remove_obj( OBJ_DATA *obj, AFFECT_DATA *paf)
{
   int where, vector;
   if ( obj->affected == NULL )
   {
      bug( "Affect_remove_object: no affect.", 0 );
      return;
   }

   if (paf->type == gsn_imbue_flame ||
   paf->type == gsn_charge_weapon ||
   paf->type == gsn_frost_charge)
   {
      obj->value[3] = obj->pIndexData->value[3];
   }
   if (obj->carried_by != NULL && obj->wear_loc != -1)
   affect_modify( obj->carried_by, paf, FALSE );

   where = paf->where;
   vector = paf->bitvector;

   /* remove flags from the object if needed */
   if (paf->bitvector)
   switch( paf->where)
   {
      case TO_OBJECT:
      REMOVE_BIT(obj->extra_flags, paf->bitvector);
      break;
      case TO_WEAPON:
      if (obj->item_type == ITEM_WEAPON)
      REMOVE_BIT(obj->value[4], paf->bitvector);
      break;
   }

   if ( paf == obj->affected )
   {
      obj->affected    = paf->next;
   }
   else
   {
      AFFECT_DATA *prev;

      for ( prev = obj->affected; prev != NULL; prev = prev->next )
      {
         if ( prev->next == paf )
         {
            prev->next = paf->next;
            break;
         }
      }

      if ( prev == NULL )
      {
         bug( "Affect_remove_object: cannot find paf.", 0 );
         return;
      }
   }

   free_affect(paf);

   if (obj->carried_by != NULL && obj->wear_loc != -1)
   affect_check(obj->carried_by, where, vector);
   return;
}



void affect_strip_room_single(ROOM_INDEX_DATA *room, int sn)
{
   ROOM_AFFECT_DATA *raf;
   ROOM_AFFECT_DATA *next_raf;

   raf = room->affected;
   while(raf != NULL){
      next_raf=raf->next;
      if (raf->type == sn)
      {
         affect_remove_room(room, raf);
         return;
      }
      raf=next_raf;
   }
   return;
}

void affect_strip_room(ROOM_INDEX_DATA *room, int sn)
{
   ROOM_AFFECT_DATA *raf;
   ROOM_AFFECT_DATA *next_raf;

   raf = room->affected;
   while(raf != NULL){
      next_raf=raf->next;
      if (raf->type == sn)
      affect_remove_room(room, raf);
      raf=next_raf;
   }
   return;
}
/*
* Strip all affects of a given sn.
*/
void affect_strip( CHAR_DATA *ch, int sn )
{
   AFFECT_DATA *paf;
   AFFECT_DATA *paf_next;

   for ( paf = ch->affected; paf != NULL; paf = paf_next )
   {
      paf_next = paf->next;
      if ( paf->type == sn )
      affect_remove( ch, paf );
   }

   return;
}



/*
*  is_affected for objects -Werv
*/
bool is_affected_obj(OBJ_DATA *obj, int sn)
{
   AFFECT_DATA *paf;

   for ( paf = obj->affected; paf != NULL; paf = paf->next )
   {
      if ( paf->type == sn )
      return TRUE;
   }

   return FALSE;
}


/*
* Return true if a char is affected by a spell.
*/
bool is_affected( CHAR_DATA *ch, int sn )
{
   AFFECT_DATA *paf;

   for ( paf = ch->affected; paf != NULL; paf = paf->next )
   {
      if ( paf->type == sn )
      return TRUE;
   }

   return FALSE;
}

bool room_sector_modified(ROOM_INDEX_DATA *room, CHAR_DATA *ch)
{
   ROOM_AFFECT_DATA *paf;
   for ( paf = room->affected; paf != NULL; paf = paf->next )
   {
      if (paf->where == TO_ROOM_SECTOR){
         if (ch != NULL)
         send_to_char("This room has already been magically altered.\n\r", ch);
         return TRUE;
      }
   }
   return FALSE;
}

bool is_affected_room( ROOM_INDEX_DATA * room, int sn)
{
   ROOM_AFFECT_DATA *paf;

   if (room == NULL) return FALSE;

   for ( paf = room->affected; paf != NULL; paf = paf->next )
   {
      if (paf->type == sn)
      return TRUE;
   }
   return FALSE;
}


/*
* Add or enhance an affect.
*/
void affect_join( CHAR_DATA *ch, AFFECT_DATA *paf, sh_int limit )
{
   AFFECT_DATA *paf_old;
   bool found;
   bool negative;

   negative = (paf->modifier < 0);

   found = FALSE;
   for ( paf_old = ch->affected; paf_old != NULL; paf_old = paf_old->next )
   {
      if ( paf_old->type == paf->type
      && paf_old->location == paf->location )
      {
         paf->level = (paf->level += paf_old->level) / 2;
         paf->duration += paf_old->duration;
         paf->modifier += paf_old->modifier;

         if ( limit > 0 && abs(paf->modifier) > limit )
         paf->modifier = (negative ? -limit:limit);

         affect_remove( ch, paf_old );
         break;
      }
   }

   affect_to_char( ch, paf );
   return;
}



/*
* Move a char out of a room.
*/
void char_from_room( CHAR_DATA *ch )
{
   OBJ_DATA *obj;

   if (!ch->valid)
   {
      return;
   }
   if ( ch->in_room == NULL )
   {
      sprintf
      (
         log_buf,
         "char_from_room: NULL room: %s",
         ch->name
      );
      bug_trust(log_buf, 0, get_trust(ch));
      return;
   }

   if ( !IS_NPC(ch) )
   {
      --ch->in_room->area->nplayer;
      if (ch->in_room->area->nplayer < 0)
      {
         sprintf
         (
            log_buf,
            "char_from_room: [%d] players in area, char [%s], room [%d]",
            ch->in_room->area->nplayer,
            ch->name,
            ch->in_room->vnum
         );
         bug_trust(log_buf, 0, get_trust(ch));
         ch->in_room->area->nplayer = 0;
      }
      if
      (
         IS_SET(ch->in_room->extra_room_flags, ROOM_1212) &&
         ch->level >= 30 &&
         !IS_IMMORTAL(ch) &&
         ch->pcdata->race_lottery > 0 &&
         ch->pcdata->race_lottery < MAX_PC_RACE &&
         !ch->house &&
         !IS_SET(ch->act2, PLR_IS_ANCIENT)
      )
      {
         remove_node_for(ch->name, lottery_race_players[ch->pcdata->race_lottery]);
         add_node(ch->name, lottery_race_players[ch->pcdata->race_lottery]);
      }
   }
   for ( obj = ch->carrying; obj != NULL; obj = obj->next_content)
   {
      if (obj->wear_loc != WEAR_NONE )
      {
         if
         (
            obj->item_type == ITEM_LIGHT ||
            (
               obj->item_type == ITEM_WEAPON &&
               IS_OBJ_STAT(obj, ITEM_GLOW)
            )
         )
         {
            ch->in_room->light -= 3;
            light_check
            (
               ch->in_room,
               ch,
               obj,
               "char_from_room"
            );
         }
         else if (IS_OBJ_STAT(obj, ITEM_GLOW))
         {
            ch->in_room->light -= 3;
            light_check
            (
               ch->in_room,
               ch,
               obj,
               "char_from_room"
            );
         }
      }
   }

   if ( ch == ch->in_room->people )
   {
      ch->in_room->people = ch->next_in_room;
   }
   else
   {
      CHAR_DATA *prev;

      for ( prev = ch->in_room->people; prev; prev = prev->next_in_room )
      {
         if ( prev->next_in_room == ch )
         {
            prev->next_in_room = ch->next_in_room;
            break;
         }
      }

      if ( prev == NULL )
      bug( "Char_from_room: ch not found.", 0 );
   }

   ch->in_room      = NULL;
   ch->next_in_room = NULL;
   ch->on          = NULL;  /* sanity check! */
   return;
}

/*
* Move a char into a room
*/
/*
void char_to_room(CHAR_DATA* ch, ROOM_INDEX_DATA* pRoomIndex)
{
   int flags;

   flags = TO_ROOM_NORMAL;
   char_to_room_1(ch, pRoomIndex, flags);
}
*/
/*
* Move a char into a room, look before anything happens
*/
/*
void char_to_room_look(CHAR_DATA* ch, ROOM_INDEX_DATA* pRoomIndex)
{
   int flags;

   flags = TO_ROOM_NORMAL | TO_ROOM_LOOK;
   char_to_room_1(ch, pRoomIndex, flags);
}
*/

/*
* Move a char into a room with options
*/
void char_to_room_1(CHAR_DATA* ch, ROOM_INDEX_DATA* pRoomIndex, int flags)
{
   OBJ_DATA* obj;
   long id = ch->id;
   AFFECT_DATA* paf;

   if (pRoomIndex == NULL)
   {
      ROOM_INDEX_DATA* room;

      sprintf(log_buf, "Char_to_room: %s->NULL", ch->name);
      bug_trust(log_buf, 0, get_trust(ch));
      if ((room = get_room_index(ROOM_VNUM_TEMPLE)) != NULL)
      {
         char_to_room_1(ch, room, flags);
      }
      else
      {
         char_to_room_1(ch, get_room_index(2), flags);
      }
      return;
   }
   if (!ch->on_line)
   {
      /* Characters are online as soon as they are put in a room */
      ch->on_line = TRUE;
      /* Add ch's id to the hash table for future lookups */
      add_id_hash(ch);
      if
      (
         !IS_NPC(ch) &&
         ch->race == grn_book
      )
      {
         add_node(ch, book_race_list);
      }
   }
   if
   (
      !IS_SET(flags, TO_ROOM_MOVE) &&
      ch->in_room != pRoomIndex
   )
   {
      sprintf
      (
         log_buf,
         "Char_to_room: Not moving, and not in right room.  Put in right room. %s [%d]->[%d]",
         ch->name,
         ch->in_room->vnum,
         pRoomIndex->vnum
      );
      bug_trust(log_buf, 0, get_trust(ch));
      if (ch->in_room)
      {
         char_from_room(ch);
      }
      char_to_room_1(ch, pRoomIndex, TO_ROOM_MOVE);
      /* No return, will continue on */
   }
   if
   (
      IS_SET(flags, TO_ROOM_MOVE) &&
      ch->in_room
   )
   {
      sprintf
      (
         log_buf,
         "Char_to_room: %s->in_room not null.  [%d]->[%d]",
         ch->name,
         ch->in_room->vnum,
         pRoomIndex->vnum
      );
      bug_trust(log_buf, 0, get_trust(ch));
      char_from_room(ch);
      /* No return, will continue on */
   }
   /* First two operations should be move and then light */
   if (IS_SET(flags, TO_ROOM_MOVE))
   {
      ch->in_room        = pRoomIndex;
      ch->next_in_room    = pRoomIndex->people;
      pRoomIndex->people    = ch;
   }
   /* Calculate light before looking, but after moving */
   for ( obj = ch->carrying; obj != NULL; obj = obj->next_content)
   {
      if (obj->wear_loc != WEAR_NONE)
      {
         if
         (
            (
               obj->item_type == ITEM_LIGHT ||
               (
                  obj->item_type == ITEM_WEAPON &&
                  IS_OBJ_STAT(obj, ITEM_GLOW)
               )
            ) &&
            ch->in_room != NULL
         )
         {
            ch->in_room->light += 3;
         }
         else if
         (
            IS_OBJ_STAT(obj, ITEM_GLOW) &&
            ch->in_room != NULL
         )
         {
            ch->in_room->light += 3;
         }
      }
   }
   /* set ancient tracking */
   if
   (
      IS_SET(flags, TO_ROOM_INSIGHT_TRACK) &&
      !IS_NPC(ch) &&
      pRoomIndex->people != NULL
   )
   {
      CHAR_DATA * vch;

      if (ch->in_room != pRoomIndex)
      {
         sprintf
         (
            log_buf,
            "char_to_room_1: ancient track, rooms: [%d] & [%d] do not match for [%s] [%d]",
            ch->in_room ?
            ch->in_room->vnum :
            0,
            pRoomIndex->vnum,
            IS_NPC(ch) ?
            ch->short_descr :
            ch->name,
            IS_NPC(ch) ?
            ch->pIndexData->vnum :
            0
         );
         bug_trust(log_buf, 0, get_trust(ch));
      }
      else
      {
         /*
            Is there a neutral or evil uncharmed mob or PC in the room with the
            target who could reveal the target's location?
         */
         for (vch = pRoomIndex->people; vch != NULL; vch = vch->next_in_room)
         {
            if
            (
               vch == ch ||
               IS_IMMORTAL(vch) ||
               vch->alignment > 0 ||
               (
                  IS_NPC(vch) &&
                  (
                     vch->pIndexData->vnum == MOB_VNUM_DECOY ||
                     vch->pIndexData->vnum == MOB_VNUM_MIRROR ||
                     vch->pIndexData->vnum == MOB_VNUM_SHADOW
                  )
               ) ||
               (
                  IS_NPC(vch) &&
                  IS_AFFECTED(vch, AFF_CHARM)
               ) ||
               !can_see(vch, ch)  /* the vch can't see the victim */
            )
            {
               continue;
            }
            ch->pcdata->last_seen_in = pRoomIndex;
            break;  /* no need to keep going when we found some eyes */
         }
      }
   }
   if (IS_SET(flags, TO_ROOM_LOOK))
   {
      do_observe(ch, "", LOOK_AUTO);
   }
   if
   (
      !IS_IMMORTAL(ch) &&
      pRoomIndex->vnum == ROOM_VNUM_BATHROOM &&
      IS_SET(flags, TO_ROOM_SMELLY) &&
      !is_affected(ch, gsn_smelly)
   )
   {
      act_color
      (
         "{B{2You land waist deep in something foul and squishy!{n\n\r"
         "{B{2You smell awful!{n",
         ch,
         NULL,
         NULL,
         TO_CHAR
      );
      act_color
      (
         "{B{2$n lands waist deep in something foul and squishy!{n\n\r"
         "{B{2$n smells awful!{n",
         ch,
         NULL,
         NULL,
         TO_ROOM
      );

      paf = new_affect();
      paf->where = TO_AFFECTS;
      paf->type = gsn_smelly;
      paf->duration = 5;
      affect_to_char(ch, paf);
      free_affect(paf);
   }
   if (!IS_NPC(ch))
   {
      ++ch->in_room->area->nplayer;
      if (ch->in_room->area->nplayer < 1)
      {
         sprintf
         (
            log_buf,
            "char_to_room: [%d] players in area, char [%s], room [%d]",
            ch->in_room->area->nplayer,
            ch->name,
            ch->in_room->vnum
         );
         bug_trust(log_buf, 0, get_trust(ch));
         ch->in_room->area->nplayer = 1;
      }
      ch->in_room->area->empty = FALSE;
      ch->in_room->area->empty_for = 0;
      if
      (
         IS_SET(ch->in_room->extra_room_flags, ROOM_1212) &&
         ch->pcdata->race_lottery > 0 &&
         ch->pcdata->race_lottery < MAX_PC_RACE
      )
      {
         remove_node_for(ch->name, lottery_race_players[ch->pcdata->race_lottery]);
      }
   }

   if
   (
      IS_SET(flags, TO_ROOM_TRAPS) &&
      is_affected_room(ch->in_room, gsn_trapmaking)
   )
   {
      trap_trigger_check(ch);
   }

   /* hound check */
   if
   (
      IS_SET(flags, TO_ROOM_TRACKING) &&
      IS_NPC(ch) &&
      ch->pIndexData->vnum == animal_table[20].vnum
   )
   {
      if (ch->mprog_target != NULL)
      {
         act("$n sniffs the ground for tracks.", ch, NULL, NULL, TO_ROOM);
         if (is_affected(ch->mprog_target, gsn_cloak_form))
         {
            act("$n looks perplexed.", ch, NULL, NULL, TO_ROOM);
         }
         else
         {
            switch (get_trackdir(ch, ch->mprog_target->id)-1)
            {
               case DIR_NORTH:
               act("$n pulls to the north.", ch, NULL, NULL, TO_ROOM); break;
               case DIR_EAST:
               act("$n pulls to the east.", ch, NULL, NULL, TO_ROOM); break;
               case DIR_SOUTH:
               act("$n pulls to the south.", ch, NULL, NULL, TO_ROOM); break;
               case DIR_WEST:
               act("$n pulls to the west.", ch, NULL, NULL, TO_ROOM); break;
               case DIR_UP:
               act("$n pulls upward.", ch, NULL, NULL, TO_ROOM); break;
               case DIR_DOWN:
               act("$n pulls downward.", ch, NULL, NULL, TO_ROOM); break;
               default:
               act("$n looks perplexed.", ch, NULL, NULL, TO_ROOM); break;
            }
         }
      }
   }

   if
   (
      IS_SET(flags, TO_ROOM_PLAGUE) &&
      IS_AFFECTED(ch, AFF_PLAGUE)
   )
   {
      AFFECT_DATA *af, plague;
      CHAR_DATA *vch;

      for ( af = ch->affected; af != NULL; af = af->next )
      {
         if (af->type == gsn_plague)
         break;
      }

      if (af == NULL)
      {
         REMOVE_BIT(ch->affected_by, AFF_PLAGUE);
         return;
      }

      if (af->level == 1)
      return;

      plague.where        = TO_AFFECTS;
      plague.type         = gsn_plague;
      plague.level         = af->level - 1;
      plague.duration     = number_range(1, 2 * plague.level);
      plague.location        = APPLY_STR;
      plague.modifier     = -5;
      plague.bitvector     = AFF_PLAGUE;

      for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
      {
         if (!saves_spell(ch, plague.level - 2, vch, DAM_DISEASE, SAVE_SPELL)
         &&  !IS_IMMORTAL(vch) &&
         !IS_AFFECTED(vch, AFF_PLAGUE) && number_bits(6) == 0)
         {
            send_to_char("You feel hot and feverish.\n\r", vch);
            act("$n shivers and looks very ill.", vch, NULL, NULL, TO_ROOM);
            affect_join(vch, &plague, 10);
         }
      }
   }

   if
   (
      IS_SET(flags, TO_ROOM_HOUSE_ENTRY) &&
      !IS_NPC(ch) &&
      ch->pcdata->spirit_room == NULL
   )
   {
      house_entry_trigger(ch, FALSE);
   }

   if
   (
      IS_SET(flags, TO_ROOM_BLOODLUST) &&
      ch->valid &&
      ch->id == id &&
      !ch->ghost &&
      ch->in_room == pRoomIndex
   )
   {
      check_bloodlust(ch);
   }

   if
   (
      IS_SET(flags, TO_ROOM_MUSIC) &&
      !IS_NPC(ch) &&
      IS_SET(ch->comm2, COMM_MUSIC) &&
      ch->pcdata->music_area != ch->in_room->area
   )
   {
      music_on(ch);
   }
   if
   (
      IS_SET(flags, TO_ROOM_RIPTIDE) &&
      pRoomIndex->sector_type != SECT_UNDERWATER &&
      is_affected(ch, gsn_rip_tide)
   )
   {
      if (ch->in_room != NULL)
      {
         act("$n has uprighted $mself.", ch, NULL, NULL, TO_ROOM);
      }
      send_to_char(get_herb_spell_name(NULL, FALSE, gsn_rip_tide), ch);
      send_to_char("\n\r", ch);
      affect_strip(ch, gsn_rip_tide);
   }
   /* 75% chance that moving stops you from feigning death */
   stop_feign(ch, 75);
   if
   (
      IS_SET(flags, TO_ROOM_SIMULACRUM_OFF)
   )
   {
      affect_strip(ch, gsn_simulacrum);
   }
   return;
}


bool logItem( OBJ_DATA * obj )
{
   if (
      ( obj )
      && ( obj->pIndexData )
      && ( obj->pIndexData->limtotal > 0 )
      && ( obj->pIndexData->vnum != 2928 ) /* ignore Sword of Demons */
   )
   return TRUE;

   return FALSE;
}

void site_check_from(CHAR_DATA* ch, OBJ_DATA* obj)
{
   OBJ_DATA* iobj;
   int cnt;

   if
   (
      ch->host == NULL ||
      obj->item_type == ITEM_FOOD ||
      obj->item_type == ITEM_DRINK_CON ||
      obj->item_type == ITEM_MAP
   )
   {
      return;
   }
   /* If I just had it, don't care */
   if
   (
      obj->prev_owners_id[0] != ch->id ||
      strcmp(obj->prev_owners[0], ch->name)
   )
   {
      for (cnt = MAX_OWNERS - 1; cnt > 0; cnt--)
      {
         free_string(obj->prev_owners[cnt]);
         free_string(obj->prev_owners_site[cnt]);
         obj->prev_owners_id[cnt] = obj->prev_owners_id[cnt - 1];
         if (obj->prev_owners[cnt - 1][0] == '\0')
         {
            obj->prev_owners[cnt] = &str_empty[0];
         }
         else
         {
            obj->prev_owners[cnt] = str_dup(obj->prev_owners[cnt - 1]);
         }
         if (obj->prev_owners_site[cnt - 1][0] == '\0')
         {
            obj->prev_owners_site[cnt] = &str_empty[0];
         }
         else
         {
            obj->prev_owners_site[cnt] = str_dup(obj->prev_owners_site[cnt - 1]);
         }
      }
      free_string(obj->prev_owners[0]);
      obj->prev_owners[0] = str_dup(ch->name);
   }
   /* Either way, update the site and id */
   free_string(obj->prev_owners_site[0]);
   obj->prev_owners_id[0] = ch->id;
   obj->prev_owners_site[0] = str_dup(ch->host);
   for (iobj = obj->contains; iobj; iobj = iobj->next_content)
   {
      site_check_from(ch, iobj);
   }
}

void site_check_get(CHAR_DATA* ch, OBJ_DATA* obj, int* output, int* woutput)
{
   CHAR_DATA* victims[MAX_OWNERS];
   OBJ_DATA* iobj;
   char buf[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];
   int cnt;
   bool first;
   bool site;

   if
   (
      ch->host == NULL ||
      obj->item_type == ITEM_FOOD ||
      obj->item_type == ITEM_DRINK_CON ||
      obj->item_type == ITEM_MAP ||
      obj->prev_owners[0][0] == '\0'
   )
   {
      return;
   }
   /* If I just had it, don't care */
   if
   (
      obj->prev_owners_id[0] != ch->id ||
      strcmp(obj->prev_owners[0], ch->name)
   )
   {
      for (cnt = 0; cnt < MAX_OWNERS; cnt++)
      {
         if (obj->prev_owners[cnt][0] != '\0')
         {
            if (obj->prev_owners_id[cnt] != -1)
            {
               victims[cnt] = id2name(obj->prev_owners_id[cnt], FALSE);
            }
            else
            {
               sprintf(buf, "+%s", obj->prev_owners[cnt]);
               victims[cnt] = get_char_world(NULL, buf);
            }
         }
         else
         {
            victims[cnt] = NULL;
         }
      }
      buf2[0] = '\0';
      if
      (
         ch->level < LEVEL_IMMORTAL &&
         obj->pIndexData->limtotal < 20 &&
         obj->pIndexData->limtotal != 0 &&
         *output <= 10
      )
      {
         buf2[0] = '\0';
         first = TRUE;
         for (cnt = MAX_OWNERS - 1; cnt >= 0; cnt--)
         {
            if (obj->prev_owners[cnt][0] == '\0')
            {
               continue;
            }
            if (!str_cmp(obj->prev_owners[cnt], ch->name))
            {
               first = TRUE;
               buf2[0] = '\0';
               continue;
            }
            if (first)
            {
               first = FALSE;
            }
            else
            {
               strcat(buf2, "->");
            }
            strcat(buf2, obj->prev_owners[cnt]);
         }
         sprintf
         (
            buf,
            "[%s] got %s previously owned %s->[%s] ROOM[%d]",
            ch->name,
            obj->short_descr,
            buf2,
            ch->name,
            (
               ch->in_room ?
               ch->in_room->vnum :
               0
            )
         );
         if (*output < 10)
         {
            log_string(buf);
         }
         else if (*output == 10)
         {
            sprintf(log_buf, "[%s] got too much to log", ch->name);
            log_string(log_buf);
         }
         (*output)++;
      }
      first = TRUE;
      site = FALSE;
      buf2[0] = '\0';

      if (*woutput <= 10)
      {
         for (cnt = MAX_OWNERS - 1; cnt >= 0; cnt--)
         {
            if (obj->prev_owners[cnt][0] == '\0')
            {
               continue;
            }
            if (!str_cmp(obj->prev_owners[cnt], ch->name))
            {
               first = TRUE;
               site = FALSE;
               buf2[0] = '\0';
               continue;
            }
            if (!str_cmp(obj->prev_owners_site[cnt], ch->host))
            {
               site = TRUE;
               first = TRUE;
               buf2[0] = '\0';
            }
            else if (!site)
            {
               continue;
            }
            if (first)
            {
               first = FALSE;
            }
            else
            {
               strcat(buf2, "->");
            }
            strcat(buf2, obj->prev_owners[cnt]);
         }
         if (site)
         {
            sprintf
            (
               buf,
               "[site] [%s] (%d) got [%s] (%d) previously owned %s->%s ROOM[%d]",
               ch->name,
               get_trust(ch),
               obj->short_descr,
               obj->level,
               buf2,
               ch->name,
               (
                  ch->in_room ?
                  ch->in_room->vnum :
                  0
               )
            );
            if (*woutput < 10)
            {
               log_string(buf);
            }
            else if (*woutput == 10)
            {
               sprintf(log_buf, "[site] [%s] got too much to log", ch->name);
               log_string(log_buf);
            }
            if (*woutput < 5)
            {
               wiznet(buf, ch, NULL, WIZ_TRANSFERS, 0, get_trust(ch));
            }
            else if (*woutput == 5)
            {
               sprintf(log_buf, "[site] [%s] got too much to log", ch->name);
               wiznet(log_buf, ch, NULL, WIZ_TRANSFERS, 0, get_trust(ch));
            }
            (*woutput)++;
         }
      }
   }
   if
   (
      *woutput > 10 &&
      *output > 10
   )
   {
      return;
   }
   for
   (
      iobj = obj->contains;
      (
         iobj != NULL &&
         (
            *woutput < 11 &&
            *output < 11
         )
      );
      iobj = iobj->next_content
   )
   {
      site_check_get(ch, iobj, output, woutput);
   }
}

/*
* Give an obj to a char.
*/
void obj_to_char(OBJ_DATA* obj, CHAR_DATA* ch)
{
   int output = 0;
   int woutput = 0;

   if (obj == NULL)
   {
      bug("obj_to_char null obj", 0);
      return;
   }
   if (ch == NULL)
   {
      bug("obj_to_char null ch", 0);
      return;
   }
   if (obj->carried_by)
   {
      sprintf
      (
         log_buf,
         "obj_to_char [%s] to [%s] (carried by [%s])",
         (
            obj->short_descr ?
            obj->short_descr :
            "something"
         ),
         ch->name,
         obj->carried_by->name
      );
      bug_trust(log_buf, 0, get_trust(ch));
      obj_from_char(obj);
   }
   if (obj->in_obj)
   {
      sprintf
      (
         log_buf,
         "obj_to_char [%s] to [%s] (in object [%s])",
         (
            obj->short_descr ?
            obj->short_descr :
            "something"
         ),
         ch->name,
         (
            obj->in_obj->short_descr ?
            obj->in_obj->short_descr :
            "something"
         )
      );
      bug_trust(log_buf, 0, get_trust(ch));
      obj_from_obj(obj);
   }
   if (obj->in_room)
   {
      sprintf
      (
         log_buf,
         "obj_to_char [%s] to [%s] (in room [%d])",
         (
            obj->short_descr ?
            obj->short_descr :
            "something"
         ),
         ch->name,
         obj->in_room->vnum
      );
      bug_trust(log_buf, 0, get_trust(ch));
      obj_from_room(obj);
   }
   obj->next_content = ch->carrying;
   ch->carrying      = obj;
   obj->carried_by   = ch;
   obj->in_room      = NULL;
   obj->in_obj       = NULL;
   ch->carry_number += get_obj_number(obj);
   ch->carry_weight += get_obj_weight(obj);
   /*
   if (logItem(obj))
   {
   sprintf
   (
   log_buf,
   "ITEM %d: to char %s",
   obj->pIndexData->vnum,
   ch->name
   );
   wiznet(log_buf, NULL, NULL, WIZ_ITEMS, 0, 0);
   log_string(log_buf);
   }
   */
   /* poison food/drink for Corruptor Necros - Runge */
   if
   (
      !IS_NPC(ch) &&
      ch->pcdata->special == SUBCLASS_CORRUPTOR &&
      (
         obj->item_type == ITEM_FOOD ||
         obj->item_type == ITEM_DRINK_CON
      )
   )
   {
      obj->value[3] = 1;
   }
   if
   (
      IS_NPC(ch) &&
      ch->master &&
      !IS_NPC(ch->master) &&
      IS_AFFECTED(ch, AFF_CHARM)
   )
   {
      site_check_get(ch->master, obj, &output, &woutput);
      site_check_from(ch->master, obj);
   }
   else if (!IS_NPC(ch))
   {
      site_check_get(ch, obj, &output, &woutput);
      site_check_from(ch, obj);
   }
   if
   (
      obj->pIndexData->vnum == OBJ_VNUM_BRETHREN &&
      ch->house == HOUSE_BRETHREN
   )
   {
      if (ch->in_room != NULL)
      {
         act
         (
            "$p melts and is absorbed into $n's body.",
            ch,
            obj,
            NULL,
            TO_ROOM
         );
         act
         (
            "$p melts and is absorbed into your body.",
            ch,
            obj,
            NULL,
            TO_CHAR
         );
      }
      extract_obj(obj, FALSE);
   }
}

/*
* Take an obj from its character.
*/
void obj_from_char( OBJ_DATA *obj )
{
   CHAR_DATA *ch;

   if ( !( ch = obj->carried_by ) )
   {
      bug("Obj_from_char: null ch.", 0);
      sprintf(log_buf, "Object was: %s\n", obj->name);
      log_string(log_buf);
      if (obj->in_obj)
      {
         bug("Obj_from_char: null ch, inside an obj!", 0);
      }
      if (obj->in_room)
      {
         bug("Obj_from_char: null ch, inside a room!", 0);
      }
      extract_obj(obj, FALSE);  /* Fizzfaldt */
      return;
   }

   if ( obj->wear_loc != WEAR_NONE )
   unequip_char( ch, obj );

   if ( ch->carrying == obj )
   ch->carrying = obj->next_content;
   else
   {
      OBJ_DATA *prev;

      for ( prev = ch->carrying; prev; prev = prev->next_content )
      if ( prev->next_content == obj )
      {
         prev->next_content = obj->next_content;
         break;
      }

      if ( !prev )
      {
         bug( "Obj_from_char: obj not in list.", 0 );
         return;
      }
   }

   obj->newcost         = 0;
   obj->carried_by      = NULL;
   obj->next_content    = NULL;
   ch->carry_number    -= get_obj_number( obj );
   ch->carry_weight    -= get_obj_weight( obj );

   if
   (
      IS_NPC(ch) &&
      ch->master &&
      !IS_NPC(ch->master) &&
      IS_AFFECTED(ch, AFF_CHARM)
   )
   {
      site_check_from(ch->master, obj);
   }
   else if (!IS_NPC(ch))
   {
      site_check_from(ch, obj);
   }

   /*
   if ( logItem(obj) )
   {
      sprintf(log_buf,  "ITEM %d: from char %s",
      obj->pIndexData->vnum,
      ch->name
   );
   wiznet( log_buf, NULL, NULL, WIZ_ITEMS, 0, 0 );
   log_string( log_buf );
   }
   */
   return;
}



/*
* Find the ac value of an obj, including position effect.
*/
int apply_ac( OBJ_DATA *obj, int iWear, int type )
{
   if ( obj->item_type != ITEM_ARMOR )
   return 0;

   switch ( iWear )
   {
      case WEAR_BODY:       return 3 * obj->value[type];
      case WEAR_HEAD:       return 2 * obj->value[type];
      case WEAR_LEGS:       return 2 * obj->value[type];
      case WEAR_FEET:       return     obj->value[type];
      case WEAR_HANDS:      return     obj->value[type];
      case WEAR_ARMS:       return     obj->value[type];
      case WEAR_SHIELD:     return     obj->value[type];
      case WEAR_NECK_1:     return     obj->value[type];
      case WEAR_NECK_2:     return     obj->value[type];
      case WEAR_ABOUT:      return 2 * obj->value[type];
      case WEAR_WAIST:      return     obj->value[type];
      case WEAR_WRIST_L:    return     obj->value[type];
      case WEAR_WRIST_R:    return     obj->value[type];
      case WEAR_HOLD:       return     obj->value[type];
      case WEAR_FINGER_L:   return     obj->value[type];
      case WEAR_FINGER_R:   return     obj->value[type];
      case WEAR_HORNS:      return   2 * obj->value[type];
      case WEAR_SNOUT:      return     obj->value[type];
      case WEAR_HOOVES:     return     obj->value[type];
      case WEAR_FOURLEGS:   return 2 * obj->value[type];
      case WEAR_FOURHOOVES: return     obj->value[type];
      case WEAR_EYES:       return 2 * obj->value[type];
      case WEAR_EAR_L:      return     obj->value[type];
      case WEAR_EAR_R:      return     obj->value[type];
      case WEAR_BRAND:      return     obj->value[type];
      case WEAR_BOSOM:      return     obj->value[type];
   }

   return 0;
}



/*
* Find a piece of eq on a character.
*/
OBJ_DATA *get_eq_char( CHAR_DATA *ch, int iWear )
{
   OBJ_DATA *obj;

   if (ch == NULL)
   return NULL;

   for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
   {
      if ( obj->wear_loc == iWear )
      return obj;
   }

   return NULL;
}

/* Recursively strip limited items */
void strip_limits(OBJ_DATA* obj)
{
   OBJ_DATA* obj_next;

   for (; obj != NULL; obj = obj_next)
   {
      obj_next = obj->next_content;
      if (obj->pIndexData->limtotal > 0)
      {
         extract_obj(obj, FALSE);
      }
      else if (obj->contains)
      {
         strip_limits(obj->contains);
      }
   }
}


/*
* Equip a char with an obj.
*/
void equip_char( CHAR_DATA *ch, OBJ_DATA *obj, int iWear )
{
   AFFECT_DATA *paf;
   int i;
   bool status;

   status = FALSE;
   if ( get_eq_char( ch, iWear ) != NULL )
   {
      char buf[MAX_STRING_LENGTH];
      sprintf(buf, "[%s] will not wear [%s]", ch->name, obj->name);
      log_string(buf);
      bug( "Equip_char: already equipped (%d).", iWear );
      return;
   }

   if ( ( IS_OBJ_STAT(obj, ITEM_ANTI_EVIL)    && IS_EVIL(ch)    )
   ||   ( IS_OBJ_STAT(obj, ITEM_ANTI_GOOD)    && IS_GOOD(ch)    )
   ||   ( IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL) && IS_NEUTRAL(ch) )
   ||   ( IS_OBJ_STAT(obj, ITEM_ANTI_METHODICAL)  && (!IS_NPC(ch) && ch->pcdata->ethos > 0))
   ||   ( IS_OBJ_STAT(obj, ITEM_ANTI_NEUT)    && (!IS_NPC(ch) && ch->pcdata->ethos == 0))
   ||   ( IS_OBJ_STAT(obj, ITEM_ANTI_IMPULSIVE) && (!IS_NPC(ch) && ch->pcdata->ethos < 0))
   || is_restricted(ch, obj))
   {
      /*
      * Thanks to Morgenes for the bug fix here!
      */
      act( "You are zapped by $p and drop it.", ch, obj, NULL, TO_CHAR );
      act( "$n is zapped by $p and drops it.",  ch, obj, NULL, TO_ROOM );
      obj_from_char( obj );
      obj_to_room( obj, ch->in_room );
      if (IS_OBJ_STAT(obj, ITEM_MELT_DROP))
      {
         act("$p dissolves into smoke.", ch, obj, NULL, TO_ROOM);
         act("$p dissolves into smoke.", ch, obj, NULL, TO_CHAR);
         extract_obj(obj, FALSE);
      }
      return;
   }

   for (i = 0; i < 4; i++)
   ch->armor[i]          -= apply_ac( obj, iWear, i );
   obj->wear_loc     = iWear;

   /*
   if (obj->pIndexData->vnum == 24418)
   wear_obj_girdle(ch, obj);

   if (obj->pIndexData->vnum == 19002)
   wear_obj_19002(ch, obj);

   if (obj->pIndexData->vnum == 15123)
   wear_obj_channeling(ch, obj);

   if (obj->pIndexData->vnum == 19445)
   wear_obj_bracers(ch, obj);

   if (obj->pIndexData->vnum == 18792)
   wear_obj_elven_armguards(ch, obj);

   if (obj->pIndexData->vnum == 23751)
   wear_obj_blue_hide(ch, obj);

   if (obj->pIndexData->vnum == 23752)
   wear_obj_green_hide(ch, obj);

   if (obj->pIndexData->vnum == 23706)
   wear_obj_dragonmage_feet(ch, obj);
   */

   if (!obj->enchanted)
   for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
   if ( paf->location != APPLY_SPELL_AFFECT )
   affect_modify( ch, paf, TRUE );
   for ( paf = obj->affected; paf != NULL; paf = paf->next )
   if ( paf->location == APPLY_SPELL_AFFECT )
   affect_to_char ( ch, paf );
   else
   affect_modify( ch, paf, TRUE );

   if
   (
      (
         obj->item_type == ITEM_LIGHT ||
         (
            obj->item_type == ITEM_WEAPON &&
            IS_OBJ_STAT(obj, ITEM_GLOW)
         )
      ) &&
      ch->in_room != NULL
   )
   {
      ch->in_room->light += 3;
   }
   else if
   (
      IS_OBJ_STAT(obj, ITEM_GLOW) &&
      ch->in_room != NULL
   )
   {
      ch->in_room->light += 3;
   }

   return;
}



/*
* Unequip a char with an obj.
*/
void unequip_char( CHAR_DATA *ch, OBJ_DATA *obj )
{
   AFFECT_DATA *paf = NULL;
   AFFECT_DATA *lpaf = NULL;
   AFFECT_DATA *lpaf_next = NULL;
   int i;

   if ( obj->wear_loc == WEAR_NONE )
   {
      bug( "Unequip_char: already unequipped.", 0 );
      return;
   }

   for (i = 0; i < 4; i++)
   ch->armor[i]    += apply_ac( obj, obj->wear_loc, i );
   obj->wear_loc     = -1;

   /*
   if (obj->pIndexData->vnum == 24418)
   remove_obj_girdle(ch, obj);

   if (obj->pIndexData->vnum == 15123)
   remove_obj_channeling(ch, obj);

   if (obj->pIndexData->vnum == 19002)
   remove_obj_19002(ch, obj);

   if (obj->pIndexData->vnum == 19445)
   remove_obj_bracers(ch, obj);

   if (obj->pIndexData->vnum == 18792)
   remove_obj_elven_armguards(ch, obj);

   if (obj->pIndexData->vnum == 23706)
   remove_obj_dragonmage_feet(ch, obj);
   if (obj->pIndexData->vnum == 23751)
   remove_obj_blue_hide(ch, obj);
   if (obj->pIndexData->vnum == 23752)
   remove_obj_green_hide(ch, obj);
   */

   if (!obj->enchanted)
   for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
   {
      if ( paf->location == APPLY_SPELL_AFFECT )
      {
         for ( lpaf = ch->affected; lpaf != NULL; lpaf = lpaf_next )
         {
            lpaf_next = lpaf->next;
            if ((lpaf->type == paf->type) &&
            (lpaf->level == paf->level) &&
            (lpaf->location == APPLY_SPELL_AFFECT))
            {
               affect_remove( ch, lpaf );
               lpaf_next = NULL;
            }
         }
      }
      else
      {
         affect_modify( ch, paf, FALSE );
         affect_check(ch, paf->where, paf->bitvector);
      }
   }

   for ( paf = obj->affected; paf != NULL; paf = paf->next )
   if ( paf->location == APPLY_SPELL_AFFECT )
   {
      bug ( "Norm-Apply: %d", 0 );
      for ( lpaf = ch->affected; lpaf != NULL; lpaf = lpaf_next )
      {
         lpaf_next = lpaf->next;
         if ((lpaf->type == paf->type) &&
         (lpaf->level == paf->level) &&
         (lpaf->location == APPLY_SPELL_AFFECT))
         {
            bug ( "location = %d", lpaf->location );
            bug ( "type = %d", lpaf->type );
            affect_remove( ch, lpaf );
            lpaf_next = NULL;
         }
      }
   }
   else
   {
      affect_modify( ch, paf, FALSE );
      affect_check(ch, paf->where, paf->bitvector);
   }

   if
   (
      (
         obj->item_type == ITEM_LIGHT ||
         (
            obj->item_type == ITEM_WEAPON &&
            IS_OBJ_STAT(obj, ITEM_GLOW)
         )
      ) &&
      ch->in_room != NULL
   )
   {
      ch->in_room->light -= 3;
      light_check
      (
         ch->in_room,
         ch,
         obj,
         "unequip_char"
      );
   }
   else if
   (
      IS_OBJ_STAT(obj, ITEM_GLOW) &&
      ch->in_room != NULL
   )
   {
      ch->in_room->light -= 3;
      light_check
      (
         ch->in_room,
         ch,
         obj,
         "unequip_char"
      );
   }

   return;
}



/*
* Count occurrences of an obj in a list.
*/
int count_obj_list( OBJ_INDEX_DATA *pObjIndex, OBJ_DATA *list )
{
   OBJ_DATA *obj;
   int nMatch;

   nMatch = 0;
   for ( obj = list; obj != NULL; obj = obj->next_content )
   {
      if ( obj->pIndexData == pObjIndex )
      nMatch++;
   }

   return nMatch;
}



/*
* Move an obj out of a room.
*/
void obj_from_room( OBJ_DATA *obj )
{
   ROOM_INDEX_DATA *in_room;
   CHAR_DATA *ch;

   if ( ( in_room = obj->in_room ) == NULL )
   {
      bug( "obj_from_room: NULL.", 0 );
      sprintf(log_buf, "Object was: %s\n", obj->name);
      if (obj->in_obj)
      {
         bug("Obj_from_room: null room, inside an obj!", 0);
      }
      if (obj->carried_by)
      {
         bug("Obj_from_room: null room, held by a char_data!", 0);
      }
      extract_obj(obj, FALSE);  /* Fizzfaldt */
      log_string(log_buf);
      return;
   }

   for (ch = in_room->people; ch != NULL; ch = ch->next_in_room)
   if (ch->on == obj)
   ch->on = NULL;

   if ( obj == in_room->contents )
   {
      in_room->contents = obj->next_content;
   }
   else
   {
      OBJ_DATA *prev;

      for ( prev = in_room->contents; prev; prev = prev->next_content )
      {
         if ( prev->next_content == obj )
         {
            prev->next_content = obj->next_content;
            break;
         }
      }

      if ( prev == NULL )
      {
         bug( "Obj_from_room: obj not found.", 0 );
         return;
      }
   }

   obj->in_room      = NULL;
   obj->next_content = NULL;
   /*
   if ( logItem(obj) )
   {
   sprintf(log_buf,  "ITEM %d: from room %d",
   obj->pIndexData->vnum,
   in_room->vnum
   );
   wiznet( log_buf, NULL, NULL, WIZ_ITEMS, 0, 0 );
   log_string( log_buf );
   }
   */
   return;
}



/*
* Move an obj into a room.
*/
void obj_to_room( OBJ_DATA *obj, ROOM_INDEX_DATA *pRoomIndex )
{
   if (obj->in_room)
   {
      sprintf
      (
         log_buf,
         "obj_to_room: %s->in_room not null.  [%d]->[%d]",
         obj->short_descr,
         obj->in_room->vnum,
         pRoomIndex->vnum
      );
      bug(log_buf, 0);
      obj_from_room(obj);
   }
   if (obj->carried_by)
   {
      sprintf
      (
         log_buf,
         "obj_to_room: %s->carried_by not null.  [%d]->[%d]",
         obj->short_descr,
         (
            obj->carried_by->in_room ?
            obj->carried_by->in_room->vnum :
            0
         ),
         pRoomIndex->vnum
      );
      bug_trust(log_buf, 0, get_trust(obj->carried_by));
      obj_from_char(obj);
   }
   if (obj->in_obj)
   {
      sprintf
      (
         log_buf,
         "obj_to_room: %s->in_obj not null.  [%s]->[%d]",
         obj->short_descr,
         obj->in_obj->short_descr,
         pRoomIndex->vnum
      );
      bug(log_buf, 0);
      obj_from_obj(obj);
   }
   obj->next_content        = pRoomIndex->contents;
   pRoomIndex->contents    = obj;
   obj->in_room        = pRoomIndex;
   obj->carried_by        = NULL;
   obj->in_obj            = NULL;
   /*
   if ( logItem(obj) )
   {
   sprintf(log_buf,  "ITEM %d: to room %d",
   obj->pIndexData->vnum,
   pRoomIndex->vnum
   );
   wiznet( log_buf, NULL, NULL, WIZ_ITEMS, 0, 0 );
   log_string( log_buf );
   }
   */
   return;
}



/*
* Move an object into an object.
*/
void obj_to_obj( OBJ_DATA *obj, OBJ_DATA *obj_to )
{
   OBJ_DATA *objsearch;

   if (obj_to == NULL)
   {
      bug
      (
         "obj_to_obj: Null obj_to, purged: %d",
         (
            obj != NULL ?
            obj->pIndexData->vnum :
            0
         )
      );
      extract_obj(obj, FALSE);
      return;
   }
   if (obj->in_room)
   {
      sprintf
      (
         log_buf,
         "obj_to_obj: %s->in_room not null.  [%d]->[%s]",
         obj->short_descr,
         obj->in_room->vnum,
         obj_to->short_descr
      );
      bug(log_buf, 0);
      obj_from_room(obj);
   }
   if (obj->in_obj)
   {
      sprintf
      (
         log_buf,
         "obj_to_obj [%s] to [%s] (in object [%s])",
         (
            obj->short_descr ?
            obj->short_descr :
            "something"
         ),
         (
            obj_to->short_descr ?
            obj_to->short_descr :
            "something"
         ),
         (
            obj->in_obj->short_descr ?
            obj->in_obj->short_descr :
            "something"
         )
      );
      bug(log_buf, 0);
      obj_from_obj(obj);
   }
   if (obj->carried_by)
   {
      sprintf
      (
         log_buf,
         "obj_to_obj: %s->carried_by not null.  [%d]->[%s]",
         obj->short_descr,
         (
            obj->carried_by->in_room ?
            obj->carried_by->in_room->vnum :
            0
         ),
         obj_to->short_descr
      );
      bug_trust(log_buf, 0, get_trust(obj->carried_by));
      obj_from_char(obj);
   }
   objsearch = obj_to; /* just to restore it later */
   for ( ; obj_to != NULL; obj_to = obj_to->in_obj )
   {
      if ( obj_to->carried_by != NULL )
      {
         obj_to->carried_by->carry_weight += (get_obj_weight( obj ) * obj_to->value[4]) / 100;
      }
   }
   obj_to = objsearch;

   obj->in_room      = NULL;
   obj->carried_by   = NULL;

   obj->next_content = obj_to->contains;
   obj_to->contains  = obj;
   obj->in_obj       = obj_to;

   if ( obj_to->pIndexData->vnum == OBJ_VNUM_PIT )
   obj->cost = 0;


   return;
}


/*
* Move an object out of an object.
*/
void obj_from_obj( OBJ_DATA *obj )
{
   OBJ_DATA *obj_from;
   OBJ_DATA *container;
   CHAR_DATA* ch;
   OBJ_DATA* in_obj;

   if ( ( obj_from = obj->in_obj ) == NULL )
   {
      bug( "Obj_from_obj: null obj_from.", 0 );
      sprintf(log_buf, "Object was: %s\n", obj->name);
      log_string(log_buf);
      if (obj->carried_by)
      {
         bug("Obj_from_obj: null obj_from, held by a char_data!", 0);
      }
      if (obj->in_room)
      {
         bug("Obj_from_obj: null obj_from, inside a room!", 0);
      }
      extract_obj(obj, FALSE);  /* Fizzfaldt */
      return;
   }

   if ( obj == obj_from->contains )
   {
      obj_from->contains = obj->next_content;
   }
   else
   {
      OBJ_DATA *prev;

      for ( prev = obj_from->contains; prev; prev = prev->next_content )
      {
         if ( prev->next_content == obj )
         {
            prev->next_content = obj->next_content;
            break;
         }
      }

      if ( prev == NULL )
      {
         bug( "Obj_from_obj: obj not found.", 0 );
         return;
      }
   }
   in_obj = obj;
   while (in_obj->in_obj)
   {
      in_obj = in_obj->in_obj;
   }
   if (in_obj->carried_by)
   {
      ch = in_obj->carried_by;
      if
      (
         IS_NPC(ch) &&
         ch->master &&
         !IS_NPC(ch->master) &&
         IS_AFFECTED(ch, AFF_CHARM)
      )
      {
         site_check_from(ch->master, obj);
      }
      else if (!IS_NPC(ch))
      {
         site_check_from(ch, obj);
      }
   }
   container = obj->in_obj;
   obj->next_content = NULL;
   obj->in_obj       = NULL;

   for ( ; obj_from != NULL; obj_from = obj_from->in_obj )
   {
      if ( obj_from->carried_by != NULL )
      {
         obj_from->carried_by->carry_weight -= (get_obj_weight( obj ) * obj_from->value[4]) / 100;
      }
   }
   return;
}



/*
* Extract an obj from the world.
*/
void extract_obj( OBJ_DATA *obj, bool DoLimits )
{
   OBJ_DATA *obj_content;
   OBJ_DATA *obj_next;

   if (!obj->valid)
   {
      return;
   }
   /*
   if ( logItem(obj) )
   {
   sprintf(log_buf,  "ITEM %d: Extracted", obj->pIndexData->vnum );
   wiznet( log_buf, NULL, NULL, WIZ_ITEMS, 0, 0 );
   log_string(log_buf);
   }
   */
   if ( obj->in_room != NULL )
   obj_from_room( obj );
   else if ( obj->carried_by != NULL )
   obj_from_char( obj );
   else if ( obj->in_obj != NULL )
   obj_from_obj( obj );

   remove_node_for(obj, value_1_list);

   if (!DoLimits)
   {
      obj->pIndexData->limcount -= 1;
   }
   --obj->pIndexData->in_game_count;


   for ( obj_content = obj->contains; obj_content; obj_content = obj_next )
   {
      obj_next = obj_content->next_content;
      extract_obj( obj_content, DoLimits );
   }

   if ( object_list == obj )
   {
      object_list = obj->next;
   }
   else
   {
      OBJ_DATA *prev;

      for ( prev = object_list; prev != NULL; prev = prev->next )
      {
         if ( prev->next == obj )
         {
            prev->next = obj->next;
            break;
         }
      }

      if ( prev == NULL )
      {
         sprintf
         (
            log_buf,
            "Extract_obj: obj %d [%s] [%s]'s not found.",
            obj->pIndexData->vnum,
            obj->short_descr,
            obj->owner
         );
         bug(log_buf, 0);
         return;
      }
   }


   free_obj(obj);
   return;
}



/* Global for use in new_char() */
CHAR_DATA* last_char_extract = NULL;
CHAR_DATA* last_char_extract_failed = NULL;

/*
* Extract a char from the world.
*/
void extract_char(CHAR_DATA* ch, bool fPull)
{
   CHAR_DATA* wch;
   CHAR_DATA* tch;
   OBJ_DATA* obj;
   OBJ_DATA* obj_next;
   char buf[MAX_STRING_LENGTH];
   static int fail_times = 0;
   static int repeat_times = 0;

   if (!ch->valid)
   {
      return;
   }
   /* remove all tracking */
   for (tch = char_list; tch != NULL; tch = tch->next)
   {
      if
      (
         tch->last_fought == ch->id &&
         fPull == FALSE
      )
      {
         tch->last_fought = -1;
      }
      if (tch->life_lined == ch)
      {
         tch->life_lined = NULL;
         send_to_char("You feel your life line suddenly break.\n\r", tch);
      }
      if (tch->mprog_target == ch)
      {
         if (!IS_NPC(tch))
         {
            send_to_char
            (
               "Your body abruptly alters to its more familiar form!\n\r",
               tch
            );
            act
            (
               "$n's body abruptly changes to its original form.",
               tch,
               NULL,
               NULL,
               TO_NOTVICT
            );
            act
            (
               "$n abruptly no longer looks like you.",
               tch,
               NULL,
               tch->mprog_target,
               TO_VICT
            );
            tch->morph_form[1] = 0;
            tch->morph_form[2] = 0;
            affect_strip(tch, gsn_shapeshift);
         }
         tch->mprog_target = NULL;
      }
      if
      (
         !IS_NPC(tch) &&
         tch->pcdata->target == ch
      )
      {
         tch->pcdata->target = NULL;
         if (is_affected(tch, gsn_target))
         {
            affect_strip(tch, gsn_target);
         }
         send_to_char("You suddenly lose track of your mark.\n\r", tch);
      }

      if
      (
         IS_NPC(ch) &&
         !IS_NPC(tch) &&
         tch->is_riding &&
         tch->is_riding->mount_type == MOUNT_MOBILE &&
         (CHAR_DATA *)tch->is_riding->mount == ch
      )
      {
         do_dismount(tch, "");
      }
   }

   nuke_pets(ch);
   ch->pet = NULL; /* just in case */

   if (fPull)
   {
      die_follower(ch);
   }

   stop_fighting(ch, TRUE);

   if
   (
      IS_NPC(ch) &&
      ch->spyfor != NULL
   )
   {
      if (ch->in_room == NULL)
      {
         send_to_char
         (
            "Your sources inform you that your lookout is gone.\n\r",
            ch->spyfor
         );
      }
      else
      {
         sprintf
         (
            buf,
            "Your sources inform you that your lookout at %s is gone.\n\r",
            ch->in_room->name
         );
         send_to_char(buf, ch->spyfor);
      }
      ch->spyfor = NULL;
   }
   if (!IS_NPC(ch))
   {
      for (wch = char_list; wch != NULL; wch = wch->next)
      {
         if
         (
            IS_NPC(wch) &&
            wch->spyfor == ch
         )
         {
            wch->spyfor = NULL;
         }
      }
   }

   for (obj = ch->carrying; obj != NULL; obj = obj_next)
   {
      obj_next = obj->next_content;

      if
      (
         !(
            obj->wear_loc == WEAR_BRAND ||
            (
               IS_SET(obj->wear_flags, ITEM_KEEP) &&
               !IS_NPC(ch)
            )
         )
      )
      {
         if
         (
            IS_NPC(ch) ||
            ch->pcdata->being_restored ||
            ch->level < 2 ||
            !fPull
         )
         {
            extract_obj(obj, FALSE);
         }
         else
         {
            extract_obj(obj, TRUE);
         }
      }
   }

   if
   (
      ch->is_riding &&
      ch->is_riding->mount_type == MOUNT_OBJECT &&
      IS_SET
      (
         (
            (OBJ_DATA*)ch->is_riding->mount
         )->mount_specs->mount_info_flags,
         MOUNT_INFO_INVENTORY
      ) &&
      (
         (OBJ_DATA*)ch->is_riding->mount
      )->in_room == ch->in_room
   )
   {
      extract_obj((OBJ_DATA*)ch->is_riding->mount, TRUE);
   }

   if (ch->in_room != NULL)
   {
      char_from_room(ch);
   }

   /* Death room is set in the house tabe now */
   if (!fPull)
   {
      return;
   }

   if (IS_NPC(ch))
   {
      --ch->pIndexData->count;
   }

   ch->mprog_target = NULL;
   for (wch = char_list; wch != NULL; wch = wch->next)
   {
      if (wch->reply == ch)
      {
         wch->reply = NULL;
      }
      if
      (
         wch->mprog_target == ch &&
         IS_NPC(wch)
      )
      {
         wch->mprog_target = NULL;
      }
      else if
      (
         fPull &&
         wch->mprog_target == ch
      )
      {
         send_to_char
         (
            "Your body fails to maintain its form and reverts to normal.\n\r",
            wch
         );
         act
         (
            "$n's body fails to maintain its form and reverts to normal.",
            wch,
            NULL,
            NULL,
            TO_ROOM
         );
         wch->mprog_target = NULL;
         affect_strip(wch, gsn_shapeshift);
      }
   }

   if
   (
      ch->desc != NULL &&
      ch->desc->original != NULL
   )
   {
      do_return(ch, "");
      ch->desc = NULL;
   }

   if (last_char_extract == ch)
   {
      repeat_times++;
      if (IS_NPC(ch))
      {
         sprintf
         (
            buf,
            "Extract_char repeat extract %%d times NPC: %s, %d",
            (
               ch->short_descr ?
               ch->short_descr :
               ch->name
            ),
            ch->pIndexData->vnum
         );
      }
      else
      {
         sprintf
         (
            buf,
            "Extract_char repeat extract %%d times PC: %s, %ld",
            ch->name,
            ch->id
         );
      }
      bug(buf, repeat_times);
   }
   if (ch == char_list)
   {
      char_list = ch->next;
   }
   else
   {
      CHAR_DATA* prev;

      for (prev = char_list; prev != NULL; prev = prev->next)
      {
         if (prev->next == ch)
         {
            prev->next = ch->next;
            break;
         }
      }

      if (prev == NULL)
      {
         if (last_char_extract_failed == ch)
         {
            fail_times++;
         }
         else
         {
            last_char_extract_failed = ch;
            fail_times = 1;
         }
         if (IS_NPC(ch))
         {
            sprintf
            (
               buf,
               "Extract_char ch not found %%d times NPC: %s, %d",
               (
                  ch->short_descr ?
                  ch->short_descr :
                  ch->name
               ),
               ch->pIndexData->vnum
            );
         }
         else
         {
            sprintf
            (
               buf,
               "Extract_char ch not found %%d times PC: %s, %ld",
               ch->name,
               ch->id
            );
         }
         bug(buf, fail_times);
         return;
      }
   }
   if (last_char_extract != ch)
   {
      last_char_extract = ch;
      repeat_times = 0;
   }


   if (ch->desc != NULL)
   {
      do_wizireport_update
      (
         ch,
         WIZI_UPDATE
      );
      ch->desc->character = NULL;
   }
   free_char(ch);
   return;
}

CHAR_DATA* get_char_room_holy_light(CHAR_DATA* ch, char* argument)
{
   if (!is_number(argument))
   {
      return NULL;
   }
   return id2name_room(atoi(argument), ch->in_room, TRUE);
}

CHAR_DATA* get_char_world_holy_light(CHAR_DATA* ch, char* argument)
{
   if (!is_number(argument))
   {
      return NULL;
   }
   return id2name(atoi(argument), TRUE);
}

/*
* Find a char in the world.
*/
CHAR_DATA* get_char_world(CHAR_DATA* ch, char* argument)
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA* wch;
   CHAR_DATA* tch;
   int number;
   int count;
   bool pconly = FALSE;
   bool mobonly = FALSE;

   if ( !argument )
   return NULL;

   if
   (
      ch != NULL &&
      (wch = get_char_room(ch, argument)) != NULL
   )
   {
      return wch;
   }

   if
   (
      argument[0] == '+' &&
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
      argument++;
      pconly = TRUE;
   }
   else if
   (
      (argument[0] == '-') &&
      (
         IS_IMMORTAL(ch) ||
         (
            IS_NPC(ch) &&
            !IS_AFFECTED(ch, AFF_CHARM)
         )
      )
   )
   {
      argument++;
      mobonly = TRUE;
   }

   number = number_argument(argument, arg);
   count  = 0;
   for ( wch = char_list; wch != NULL ; wch = wch->next )
   {
      if (ch == NULL)
      {
         tch = wch;
      }
      else
      {
         tch = ch;
      }
      if
      (
         wch->in_room == NULL ||
         !can_see(tch, wch) ||
         !is_name(arg, get_name(wch, tch)) ||
         (
            pconly &&
            IS_NPC(wch)
         ) ||
         (
            mobonly &&
            !IS_NPC(wch)
         ) ||
         (
            !mobonly &&
            IS_NPC(wch) &&
            IS_IMMORTAL(ch) &&
            (
               wch->pIndexData->vnum == MOB_VNUM_DECOY ||
               wch->pIndexData->vnum == MOB_VNUM_SHADOW ||
               wch->pIndexData->vnum == MOB_VNUM_MIRROR ||
               wch->pIndexData->vnum == MOB_VNUM_PHANTOM
            )
         )
      )
      {
         continue;
      }
      if (++count == number)
      {
         return wch;
      }
   }

   return NULL;
}

/*
* Find a pc char in the world.
*/
CHAR_DATA* get_pc_world(CHAR_DATA* ch, char* argument, bool ignore_blind)
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA* wch;
   int number;
   int count;

   if
   (
      !argument ||
      ch == NULL
   )
   {
      return NULL;
   }

   if
   (
      ch != NULL &&
      (wch = get_char_room(ch, argument)) != NULL
   )
   {
      return wch;
   }

   number = number_argument(argument, arg);
   count  = 0;
   for ( wch = char_list; wch != NULL ; wch = wch->next )
   {
      if
      (
         wch->in_room == NULL ||
         (
            !ignore_blind &&
            !can_see(ch, wch)
         ) ||
         !is_name(arg, get_name(wch, ch)) ||
         IS_NPC(wch)
      )
      {
         continue;
      }
      if (++count == number)
      {
         return wch;
      }
   }

   return NULL;
}

/* used by blitz */
CHAR_DATA *get_char_area( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *wch;
   int number;
   int count;
   int pconly = 0;

   if ( ( wch = get_char_room( ch, argument ) ) != NULL )
   return wch;

   if ( (argument[0] == '+') && ch && IS_IMMORTAL(ch) )
   {
      argument++;
      pconly = 1;
   }

   number = number_argument( argument, arg );
   count  = 0;
   for ( wch = char_list; wch != NULL ; wch = wch->next )
   {
      if ( wch->in_room == NULL || !can_see( ch, wch )
      ||   !is_name( arg, get_name(wch, ch) )
      ||   !(ch->in_room->area == wch->in_room->area) )
      continue;
      if ( pconly && IS_NPC(wch) ) continue;
      if ( ++count == number )
      return wch;
   }

   return NULL;
}



/*
* Find some object with a given index data.
* Used by area-reset 'P' command.
*/
OBJ_DATA *get_obj_type( OBJ_INDEX_DATA *pObjIndex )
{
   OBJ_DATA *obj;

   for ( obj = object_list; obj != NULL; obj = obj->next )
   {
      if ( obj->pIndexData == pObjIndex )
      return obj;
   }

   return NULL;
}


/*
* Find an obj in a list.
*/
OBJ_DATA *get_obj_list( CHAR_DATA *ch, char *argument, OBJ_DATA *list )
{
   char arg[MAX_INPUT_LENGTH];
   OBJ_DATA *obj;
   int number;
   int count;

   number = number_argument( argument, arg );
   count  = 0;
   for ( obj = list; obj != NULL; obj = obj->next_content )
   {
      if ( can_see_obj( ch, obj ) && (arg[0] == '\0' || is_name( arg, obj->name )) )
      {
         if ( ++count == number )
         return obj;
      }
   }

   return NULL;
}


/*
* Find an vnum on player, inventory or equipment
* Player doesn't have to be able to see it
*/
OBJ_DATA* get_obj_char_vnum(CHAR_DATA* ch, sh_int argument)
{
   OBJ_DATA *obj;
   for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
   {
      if (obj->pIndexData->vnum == argument)
      {
         return obj;
      }
   }
   return NULL;
}

/*
   Find an obj in player's inventory.
*/
OBJ_DATA *get_obj_carry( CHAR_DATA *ch, char *argument, CHAR_DATA *viewer )
{
   char arg[MAX_INPUT_LENGTH];
   OBJ_DATA *obj;
   int number;
   int count;

   number = number_argument( argument, arg );
   count  = 0;
   for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
   {
      if ( obj->wear_loc == WEAR_NONE
      &&   (can_see_obj( viewer, obj ) )
      &&   (arg[0] == '\0' || is_name( arg, obj->name )) )
      {
         if ( ++count == number )
         return obj;
      }
   }

   return NULL;
}



/*
* Find an obj in player's equipment.
*/
OBJ_DATA *get_obj_wear( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   OBJ_DATA *obj;
   int number;
   int count;

   number = number_argument( argument, arg );
   count  = 0;
   for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
   {
      if ( obj->wear_loc != WEAR_NONE
      &&   can_see_obj( ch, obj )
      &&   (arg[0] == '\0' || is_name( arg, obj->name )) )
      {
         if ( ++count == number )
         return obj;
      }
   }

   return NULL;
}



/*
   Find an obj in the room or in inventory.
   Look first at equipment (worn)
   then inventory
   then floor
*/
OBJ_DATA* get_obj_here(CHAR_DATA* ch, char* argument)
{
   char arg[MAX_INPUT_LENGTH];
   OBJ_DATA* obj;
   int number;
   int count;

   number = number_argument(argument, arg);
   count  = 0;
   for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
   {
      if
      (
         obj->wear_loc != WEAR_NONE &&
         can_see_obj(ch, obj) &&
         (
            arg[0] == '\0' ||
            is_name(arg, obj->name)
         ) &&
         ++count == number
      )
      {
         return obj;
      }
   }
   for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
   {
      if
      (
         obj->wear_loc == WEAR_NONE &&
         can_see_obj(ch, obj) &&
         (
            arg[0] == '\0' ||
            is_name(arg, obj->name)
         ) &&
         ++count == number
      )
      {
         return obj;
      }
   }
   for (obj = ch->in_room->contents; obj != NULL; obj = obj->next_content)
   {
      if
      (
         can_see_obj(ch, obj) &&
         (
            arg[0] == '\0' ||
            is_name(arg, obj->name)
         ) &&
         ++count == number
      )
      {
         return obj;
      }
   }
   return NULL;
}

OBJ_DATA* get_obj_inv_worn_room(CHAR_DATA* ch, char* argument)
{
   char arg[MAX_INPUT_LENGTH];
   OBJ_DATA* obj;
   int number;
   int count;

   number = number_argument(argument, arg);
   count  = 0;
   for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
   {
      if
      (
         obj->wear_loc == WEAR_NONE &&
         can_see_obj(ch, obj) &&
         (
            arg[0] == '\0' ||
            is_name(arg, obj->name)
         ) &&
         ++count == number
      )
      {
         return obj;
      }
   }
   for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
   {
      if
      (
         obj->wear_loc != WEAR_NONE &&
         can_see_obj(ch, obj) &&
         (
            arg[0] == '\0' ||
            is_name(arg, obj->name)
         ) &&
         ++count == number
      )
      {
         return obj;
      }
   }
   for (obj = ch->in_room->contents; obj != NULL; obj = obj->next_content)
   {
      if
      (
         can_see_obj(ch, obj) &&
         (
            arg[0] == '\0' ||
            is_name(arg, obj->name)
         ) &&
         ++count == number
      )
      {
         return obj;
      }
   }
   return NULL;
}

/*
* Find an obj in the world.
*/
OBJ_DATA *get_obj_world( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   OBJ_DATA *obj;
   int number;
   int count;

   if ( ( obj = get_obj_here( ch, argument ) ) != NULL )
   return obj;

   number = number_argument( argument, arg );
   count  = 0;
   for ( obj = object_list; obj != NULL; obj = obj->next )
   {
      if ( can_see_obj( ch, obj ) && is_name( arg, obj->name ) )
      {
         if ( ++count == number )
         return obj;
      }
   }

   return NULL;
}

/*
* Find an obj in the world.
*/
OBJ_DATA* get_obj_inv_worn_room_world( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   OBJ_DATA *obj;
   int number;
   int count;

   if ( ( obj = get_obj_inv_worn_room( ch, argument ) ) != NULL )
   {
      return obj;
   }

   number = number_argument( argument, arg );
   count  = 0;
   for ( obj = object_list; obj != NULL; obj = obj->next )
   {
      if ( can_see_obj( ch, obj ) && is_name( arg, obj->name ) )
      {
         if ( ++count == number )
         return obj;
      }
   }

   return NULL;
}

/* deduct cost from a character */

void deduct_cost(CHAR_DATA *ch, int cost)
{
   int silver = 0, gold = 0;

   silver = UMIN(ch->silver, cost);

   if (silver < cost)
   {
      gold = ((cost - silver + 99) / 100);
      silver = cost - 100 * gold;
   }

   ch->gold -= gold;
   ch->silver -= silver;

   if (ch->gold < 0)
   {
      bug("deduct costs: gold %d < 0", ch->gold);
      ch->gold = 0;
   }
   if (ch->silver < 0)
   {
      bug("deduct costs: silver %d < 0", ch->silver);
      ch->silver = 0;
   }
   collect_tax(ch, cost);
}

/*
* Create a 'money' obj.
*/
OBJ_DATA *create_money( int gold, int silver )
{
   char buf[MAX_STRING_LENGTH];
   OBJ_DATA *obj;

   if ( gold < 0 || silver < 0 || (gold == 0 && silver == 0) )
   {
      bug( "Create_money: zero or negative money.", UMIN(gold, silver));
      gold = UMAX(1, gold);
      silver = UMAX(1, silver);
   }

   if (gold == 0 && silver == 1)
   {
      obj = create_object( get_obj_index( OBJ_VNUM_SILVER_ONE ), 0 );
   }
   else if (gold == 1 && silver == 0)
   {
      obj = create_object( get_obj_index( OBJ_VNUM_GOLD_ONE), 0 );
   }
   else if (silver == 0)
   {
      obj = create_object( get_obj_index( OBJ_VNUM_GOLD_SOME ), 0 );
      sprintf( buf, obj->short_descr, gold );
      free_string( obj->short_descr );
      obj->short_descr        = str_dup( buf );
      obj->value[1]           = gold;
      obj->cost               = gold;
      obj->weight        = gold/5;
   }
   else if (gold == 0)
   {
      obj = create_object( get_obj_index( OBJ_VNUM_SILVER_SOME ), 0 );
      sprintf( buf, obj->short_descr, silver );
      free_string( obj->short_descr );
      obj->short_descr        = str_dup( buf );
      obj->value[0]           = silver;
      obj->cost               = silver;
      obj->weight        = silver/20;
   }

   else
   {
      obj = create_object( get_obj_index( OBJ_VNUM_COINS ), 0 );
      sprintf( buf, obj->short_descr, silver, gold );
      free_string( obj->short_descr );
      obj->short_descr    = str_dup( buf );
      obj->value[0]        = silver;
      obj->value[1]        = gold;
      obj->cost        = 100 * gold + silver;
      obj->weight        = gold / 5 + silver / 20;
   }

   return obj;
}



/*
* Return # of objects which an object counts as.
* Thanks to Tony Chamberlain for the correct recursive code here.
*/
int get_obj_number( OBJ_DATA *obj )
{
   int number;

   if (obj->wear_loc == WEAR_BRAND || obj->item_type == ITEM_MONEY)
   /*    ||  obj->item_type == ITEM_GEM || obj->item_type == ITEM_JEWELRY) */
   number = 0;
   else
   number = 1;

   return number;
}


/*
* Return weight of an object, including weight of contents.
*/
int get_obj_weight( OBJ_DATA *obj )
{
   int weight;
   OBJ_DATA *tobj;

   weight = obj->weight;
   for ( tobj = obj->contains; tobj != NULL; tobj = tobj->next_content )
   weight += (get_obj_weight( tobj ) * obj->value[4]) / 100;

   return weight;
}

int get_true_weight(OBJ_DATA *obj)
{
   int weight;

   weight = obj->weight;
   for ( obj = obj->contains; obj != NULL; obj = obj->next_content )
   weight += get_obj_weight( obj );

   return weight;
}

/*
* True if room is dark.
*/
bool room_is_dark( ROOM_INDEX_DATA *pRoomIndex )
{
   if (pRoomIndex == NULL){
      sprintf(log_buf,  "BUG: NULL room in room_is_dark");
      log_string(log_buf);
      return FALSE;
   }

   if (immrp_darkness)
   return TRUE;

   if (is_affected_room(pRoomIndex, gsn_globe_darkness))
   return TRUE;

   if ( pRoomIndex->light > 2 )
   return FALSE;

   if ( pRoomIndex->sector_type == SECT_UNDERGROUND)
   return TRUE;

   if ( IS_SET(pRoomIndex->room_flags, ROOM_DARK) )
   return TRUE;

   if ( pRoomIndex->sector_type == SECT_INSIDE
   ||   pRoomIndex->sector_type == SECT_CITY )
   return FALSE;

   if ( weather_info.sunlight == SUN_SET
   ||   weather_info.sunlight == SUN_DARK )
   return TRUE;

   return FALSE;
}


bool is_room_owner(CHAR_DATA *ch, ROOM_INDEX_DATA *room)
{
   if (room->owner == NULL || room->owner[0] == '\0')
   return FALSE;

   return is_name(ch->name, room->owner);
}

/*
* True if room is private.
*/
bool room_is_private( ROOM_INDEX_DATA *pRoomIndex )
{
   CHAR_DATA *rch;
   int count;

   return FALSE;

   if (pRoomIndex->owner != NULL && pRoomIndex->owner[0] != '\0')
   return TRUE;

   count = 0;
   for ( rch = pRoomIndex->people; rch != NULL; rch = rch->next_in_room )
   count++;

   if ( IS_SET(pRoomIndex->room_flags, ROOM_PRIVATE)  && count >= 2 )
   return TRUE;

   count = 0;
   for ( rch = pRoomIndex->people; rch != NULL; rch = rch->next_in_room )
   if (!IS_NPC(rch))
   count++;

   if ( IS_SET(pRoomIndex->room_flags, ROOM_SOLITARY) && count >= 1 )
   return TRUE;

   if ( IS_SET(pRoomIndex->room_flags, ROOM_IMP_ONLY) )
   return TRUE;

   return FALSE;
}

/* visibility on a room -- for entering and exits */
bool can_see_room( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex )
{
   if (IS_SET(pRoomIndex->room_flags, ROOM_IMP_ONLY)
   &&  get_trust(ch) < MAX_LEVEL)
   return FALSE;

   if (IS_SET(pRoomIndex->room_flags, ROOM_GODS_ONLY)
   &&  !IS_IMMORTAL(ch))
   return FALSE;

   if (IS_SET(pRoomIndex->room_flags, ROOM_HEROES_ONLY)
   &&  !IS_IMMORTAL(ch))
   return FALSE;

   if (IS_SET(pRoomIndex->room_flags, ROOM_NEWBIES_ONLY)
   &&  ch->level > 5 && !IS_IMMORTAL(ch) && !(ch->house == HOUSE_SCHOLAR))
   return FALSE;

   if (IS_SET(pRoomIndex->room_flags, ROOM_LOW_ONLY)
   &&  ch->level > 10
   && !IS_SET(ch->in_room->room_flags, ROOM_LOW_ONLY)
   && !IS_IMMORTAL(ch)
   && !(ch->house == HOUSE_SCHOLAR))
   return FALSE;

   return TRUE;
}



/*
* True if char can see victim.
*/
bool can_see( CHAR_DATA *ch, CHAR_DATA *victim )
{
   int blend;
   int forest_hide = FALSE;

   /* RT changed so that WIZ_INVIS has levels */

   if (ch == NULL) return TRUE;
   if (victim == NULL) return FALSE;

   if ( ch == victim )
   return TRUE;

   if (ch == victim->see)
   return TRUE;

   if (ch == victim->nosee)
   return FALSE;
   if
   (
      !IS_NPC(ch) &&
      !IS_NPC(victim) &&
      IS_IMMORTAL(victim) &&
      ch->pcdata->nosee_perm != NULL &&
      ch->pcdata->nosee_perm[0] != '\0' &&
      is_name(victim->name, ch->pcdata->nosee_perm)
   )
   {
      return FALSE;
   }


   /* seebrand and seehouse -Wervdon  the imm check at first is just a
   safety check*/
   if (IS_IMMORTAL(victim)){
      if (IS_SET(victim->act2, WIZ_SEENEWBIE) && victim->level <= 10)
      return TRUE;
      if
      (
         IS_SET(victim->act2, WIZ_SEEHOUSE) &&
         victim->house &&
         victim->house == ch->house
      )
      {
         return TRUE;
      }
      if
      (
         IS_SET(victim->act2, WIZ_SEECLAN) &&
         !IS_NPC(ch) &&
         !IS_NPC(victim) &&
         ch->pcdata->clan != 0 &&
         ch->pcdata->clan == victim->pcdata->clan
      )
      {
         return TRUE;
      }
      if
      (
         IS_SET(victim->act2, WIZ_SEEWORSHIP) &&
         !IS_NPC(ch) &&
         ch->pcdata->worship &&
         !str_cmp
         (
            ch->pcdata->worship,
            victim->name
         )
      )
      {
         return TRUE;
      }
      if (IS_SET(victim->act2, WIZ_SEEBRAND) &&
      get_eq_char(ch, WEAR_BRAND) != NULL &&
      get_eq_char(victim, WEAR_BRAND) != NULL &&
      get_eq_char(victim, WEAR_BRAND)->pIndexData->vnum == get_eq_char(ch, WEAR_BRAND)->pIndexData->vnum)
      return TRUE;
   }


   if ( get_trust(ch) < victim->invis_level)
   return FALSE;


   if (get_trust(ch) < victim->incog_level && ch->in_room != victim->in_room)
   return FALSE;

   if
   (
      IS_SET(ch->act, ACT_HOLYLIGHT) ||
      (
         IS_NPC(ch) &&
         IS_IMMORTAL(ch)
      )
   )
   {
      return TRUE;
   }
   if
   (
      !is_same_group(ch, victim) &&
      check_simulacrum(victim) != -1
   )
   {
      return FALSE;
   }

   if (!IS_NPC(ch)
   && ch->pcdata->death_status == HAS_DIED)
   return TRUE;
   /* Make sure House guardians can always see */
   if (IS_NPC(ch) && ch->house != 0)
   {
      return TRUE;
   }

   if (is_affected_room(ch->in_room, gsn_corrupt("smoke screen", &gsn_smoke_screen)))
   return FALSE;

   /* Malignus */
   /*
   if (is_affected(victim, gsn_stealth)){
   if (ch->spec_fun == spec_lookup("spec_guard") ||
   ch->spec_fun == spec_lookup("spec_executioner") ||
   ch->spec_fun == spec_lookup("spec_legion") ||
   ch->spec_fun == spec_lookup("spec_enforcer"))
   return FALSE;
   }
   */

   /*
   if ( is_affected(ch, skill_lookup("bloodmist")) )
   return FALSE;
   */
   if ( IS_AFFECTED(ch, AFF_BLIND) )
   return FALSE;

   if (is_affected(victim, gsn_earthfade) &&
   (!has_skill(ch, gsn_earth_form) || get_skill(ch, gsn_earth_form) < 75))
   return FALSE;

/* Adding Submerge - Maelstrom */

   if ( is_affected ( victim, gsn_submerge )
     && ch->in_room != victim->in_room )
   return FALSE;

/* end Submerge */


   if ( is_affected(victim, gsn_shroud)
   && !is_affected(ch, gsn_eye_of_the_predator) )
   return FALSE;

   if ( IS_AFFECTED(victim, AFF_INVISIBLE)
   &&   !IS_AFFECTED(ch, AFF_DETECT_INVIS) )
   return FALSE;

   if ( victim->in_room != NULL &&
   ((victim->in_room->sector_type == SECT_FOREST) ||
   (victim->in_room->sector_type == SECT_MOUNTAIN) ||
   (victim->in_room->sector_type == SECT_HILLS)) &&
   IS_AFFECTED(victim, AFF_HIDE) &&
   IS_AFFECTED(ch, AFF_ACUTE_VISION))
   forest_hide = TRUE;

   if ( IS_AFFECTED(victim, AFF_HIDE)
   &&   !IS_AFFECTED(ch, AFF_DETECT_HIDDEN)
   &&   !forest_hide
   &&   victim->fighting == NULL)
   return FALSE;

   /* advanced and expert hiding */
   if ( IS_AFFECTED(victim, AFF_HIDE) && has_skill(victim, gsn_adv_hide) &&
   get_skill(victim, gsn_adv_hide) > 1)
   {
      int chance, sn_improve, improve_rate;
      sn_improve = gsn_adv_hide;
      improve_rate = 3;
      chance = get_skill(victim, gsn_adv_hide)/2;
      chance += victim->level*3;
      chance -= ch->level*3;
      if (has_skill(victim, gsn_adv_hide) && get_skill(victim, gsn_adv_hide) == 100)
      {
         sn_improve = gsn_exp_hide;
         chance += get_skill(victim, gsn_exp_hide)/3;
         improve_rate = 5;
      }
      if (has_skill(ch, gsn_adv_hide) && get_skill(ch, gsn_adv_hide) > 60) chance -= 50;
      if (has_skill(ch, gsn_exp_hide) && get_skill(ch, gsn_exp_hide) > 60) chance -= 35;
      if (number_percent() < chance)
      {
         check_improve(victim, sn_improve, TRUE, improve_rate);
         return FALSE;
      }
   }

   if ( IS_AFFECTED(victim, AFF_CAMOUFLAGE)
   &&   !is_affected(ch, gsn_acute_vision)
   &&   !IS_AFFECTED(ch, AFF_ACUTE_VISION))
   return FALSE;

   if
   (
      !IS_AFFECTED2(ch, AFF_AWARENESS) &&
      is_affected(victim, gsn_forest_blending)
   )
   {
      blend = get_skill(victim, gsn_forest_blending)/2;
      blend += victim->level*2;
      blend -= ch->level*2;
      if (blend > 70)
      {
         blend = 70;
      }

      if (number_percent() < blend)
      return FALSE;
   }

   if (is_affected_room(ch->in_room, gsn_globe_darkness)
   && !IS_AFFECTED(ch, AFF_INFRARED))
   return FALSE;

   if (IS_NPC(ch)) return TRUE;

   if ( room_is_dark( ch->in_room ) && !IS_AFFECTED(ch, AFF_INFRARED) )
   return FALSE;

   return TRUE;
}

/*
* True if char can see the rogue - slight modification of can_see
*/
bool can_see_rogue(CHAR_DATA* ch, CHAR_DATA* victim)
{
   int blend;
   int forest_hide = FALSE;

   if (ch == NULL)
   {
      return TRUE;
   }
   if (victim == NULL)
   {
      return FALSE;
   }

   /* Song of shadows is for players only, not mobs */
   if (IS_NPC(victim))
   {
      return TRUE;
   }

   if (ch == victim || ch == victim->see)
   {
      return TRUE;
   }

   if (ch == victim->nosee)
   {
      return FALSE;
   }
   if
   (
      !IS_NPC(ch) && !IS_NPC(victim) &&
      IS_IMMORTAL(victim) &&
      ch->pcdata->nosee_perm != NULL &&
      ch->pcdata->nosee_perm[0] != '\0' &&
      is_name(victim->name, ch->pcdata->nosee_perm)
   )
   {
      return FALSE;
   }

   if (!IS_NPC(ch) && ch->pcdata->death_status == HAS_DIED)
   {
      return TRUE;
   }

   /* Make sure House guardians can always see */
   if (IS_NPC(ch) && ch->house != 0)
   {
      return TRUE;
   }

   if
   (
      is_affected(victim, gsn_earthfade) &&
      (!has_skill(ch, gsn_earth_form) || get_skill(ch, gsn_earth_form) < 75)
   )
   {
      return FALSE;
   }

   if (is_affected(victim, gsn_submerge) && ch->in_room != victim->in_room)
   {
      return FALSE;
   }

   if (IS_AFFECTED(victim, AFF_INVISIBLE) && !IS_AFFECTED(ch, AFF_DETECT_INVIS))
   {
      return FALSE;
   }

   if
   (
      victim->in_room != NULL &&
      ((victim->in_room->sector_type == SECT_FOREST) ||
      (victim->in_room->sector_type == SECT_MOUNTAIN) ||
      (victim->in_room->sector_type == SECT_HILLS)) &&
      IS_AFFECTED(victim, AFF_HIDE) &&
      IS_AFFECTED(ch, AFF_ACUTE_VISION)
   )
   {
      forest_hide = TRUE;
   }

   if
   (
      IS_AFFECTED(victim, AFF_HIDE) &&
      !IS_AFFECTED(ch, AFF_DETECT_HIDDEN) &&
      !forest_hide &&
      victim->fighting == NULL
   )
   {
      return FALSE;
   }

   /* advanced and expert hiding */
   if
   (
      IS_AFFECTED(victim, AFF_HIDE) && has_skill(victim, gsn_adv_hide) &&
      get_skill(victim, gsn_adv_hide) > 1
   )
   {
      int chance, sn_improve, improve_rate;
      sn_improve = gsn_adv_hide;
      improve_rate = 3;
      chance = get_skill(victim, gsn_adv_hide)/2;
      chance += victim->level*3;
      chance -= ch->level*3;
      if
      (
         has_skill(victim, gsn_adv_hide) &&
         get_skill(victim, gsn_adv_hide) == 100
      )
      {
         sn_improve = gsn_exp_hide;
         chance += get_skill(victim, gsn_exp_hide)/3;
         improve_rate = 5;
      }
      if (has_skill(ch, gsn_adv_hide) && get_skill(ch, gsn_adv_hide) > 60)
      {
         chance -= 50;
      }
      if (has_skill(ch, gsn_exp_hide) && get_skill(ch, gsn_exp_hide) > 60)
      {
         chance -= 35;
      }
      if (number_percent() < chance)
      {
         check_improve(victim, sn_improve, TRUE, improve_rate);
         return FALSE;
      }
   }

   if
   (
      IS_AFFECTED(victim, AFF_CAMOUFLAGE) &&
      !is_affected(ch, gsn_acute_vision) &&
      !IS_AFFECTED(ch, AFF_ACUTE_VISION)
   )
   {
      return FALSE;
   }

   if
   (
      !IS_AFFECTED2(ch, AFF_AWARENESS) &&
      is_affected(victim, gsn_forest_blending)
   )
   {
      blend = get_skill(victim, gsn_forest_blending)/2;
      blend += victim->level*2;
      blend -= ch->level*2;
      if (blend > 70)
      {
         blend = 70;
      }

      if (number_percent() < blend)
      {
         return FALSE;
      }
   }

   if
   (
      is_affected_room(ch->in_room, gsn_globe_darkness) &&
      !IS_AFFECTED(ch, AFF_INFRARED)
   )
   {
      return FALSE;
   }

   if (room_is_dark(ch->in_room) && !IS_AFFECTED(ch, AFF_INFRARED))
   {
      return FALSE;
   }

   return TRUE;
}

/*
* True if char can hit marauder with an area skill or spell.
*/
bool can_area_hit_rogue(CHAR_DATA* ch, CHAR_DATA* victim)
{
   if
   (
      is_affected(victim, gsn_song_of_shadows) &&
      !can_see_rogue(ch, victim)
   )
   {
      return FALSE;
   }
   return TRUE;
}

/*
* True if char can see obj.
*/
bool can_see_obj( CHAR_DATA *ch, OBJ_DATA *obj )
{
   if
   (
      !IS_NPC(ch) &&
      IS_SET(ch->act, ACT_HOLYLIGHT)
   )
   {
      return TRUE;
   }

   if ( IS_SET(obj->extra_flags, ITEM_VIS_DEATH))
   return FALSE;

   /* Holylight mobs see everything but visdeath */
   if (IS_SET(ch->act, ACT_HOLYLIGHT))
   {
      return TRUE;
   }
   if (obj->pIndexData->vnum == OBJ_VNUM_SHACKLES)
   return TRUE;

   if
   (
      obj->wear_loc == WEAR_INSIDE_COVER &&
      obj->carried_by &&
      !IS_NPC(obj->carried_by)
   )
   {
      if
      (
         obj->carried_by != ch &&
         (
            obj->carried_by->pcdata->current_desc == BOOK_RACE_CLOSED ||
            obj->carried_by->pcdata->current_desc == BOOK_RACE_LOCKED
         )
      )
      {
         return FALSE;
      }
      if (obj->carried_by == ch)
      {
         return TRUE;
      }
   }

   if
   (
      obj->item_type == ITEM_PORTAL_NEW &&
      IS_SET(obj->value[4], PORTAL_MOONGATE) &&
      IS_SET(obj->value[4], PORTAL_CLOSED)
   )
   {
      return FALSE;
   }

   if
   (
      obj->item_type == ITEM_PORTAL_NEW &&
      IS_SET(obj->value[4], PORTAL_TIMEGATE) &&
      IS_SET(obj->value[4], PORTAL_CLOSED) &&
      !IS_NPC(ch) &&
      ch->pcdata->special != SUBCLASS_TEMPORAL_MASTER
   )
   {
      return FALSE;
   }

   if
   (
      is_affected_room
      (
         ch->in_room,
         gsn_corrupt("smoke screen", &gsn_smoke_screen)
      ) &&
      obj->item_type != ITEM_POTION
   )
   {
      return FALSE;
   }

   if
   (
      IS_AFFECTED(ch, AFF_BLIND) &&
      obj->item_type != ITEM_POTION &&
      obj->item_type != ITEM_PILL &&
      obj->item_type != ITEM_HERB
   )
   {
      return FALSE;
   }

   if
   (
      obj->item_type == ITEM_LIGHT &&
      obj->value[2] != 0
   )
   {
      return TRUE;
   }

   if
   (
      IS_SET(obj->extra_flags, ITEM_INVIS) &&
      !IS_AFFECTED(ch, AFF_DETECT_INVIS)
   )
   {
      return FALSE;
   }

   if (IS_OBJ_STAT(obj, ITEM_GLOW))
   {
      return TRUE;
   }

   if
   (
      room_is_dark(ch->in_room) &&
      !IS_AFFECTED(ch, AFF_INFRARED)
   )
   {
      return FALSE;
   }

   return TRUE;
}



/*
* True if char can drop obj.
*/
bool can_drop_obj( CHAR_DATA *ch, OBJ_DATA *obj )
{
   if ( !IS_SET(obj->extra_flags, ITEM_NODROP) )
   return TRUE;

   if ( !IS_NPC(ch) && ch->level >= LEVEL_IMMORTAL )
   return TRUE;

   return FALSE;
}

/*
* True if char can remove obj.
*/
bool can_remove_obj( CHAR_DATA *ch, OBJ_DATA *obj )
{
   if ( !IS_SET(obj->extra_flags, ITEM_NOREMOVE) )
   return TRUE;

   if ( !IS_NPC(ch) && ch->level >= LEVEL_IMMORTAL )
   return TRUE;

   return FALSE;
}


/*
* Return ascii name of an affect location.
*/
char *affect_loc_name( int location )
{
   switch ( location )
   {
      case APPLY_NONE:             return "none";
      case APPLY_STR:              return "strength";
      case APPLY_DEX:              return "dexterity";
      case APPLY_INT:              return "intelligence";
      case APPLY_WIS:              return "wisdom";
      case APPLY_CON:              return "constitution";
      case APPLY_SEX:              return "sex";
      case APPLY_CLASS:            return "class";
      case APPLY_LEVEL:            return "level";
      case APPLY_HEIGHT:           return "size";
      case APPLY_AGE:              return "age";
      case APPLY_MANA:             return "mana";
      case APPLY_HIT:              return "hp";
      case APPLY_MOVE:             return "moves";
      case APPLY_GOLD:             return "gold";
      case APPLY_EXP:              return "experience";
      case APPLY_SIZE:             return "size";
      case APPLY_AC:               return "armor class";
      case APPLY_HITROLL:          return "hit roll";
      case APPLY_DAMROLL:          return "damage roll";
      case APPLY_SIGHT:            return "sight";
      case APPLY_HOLY_POWER:       return "holy power";
      case APPLY_SPELL_POWER:      return "spell power";
      case APPLY_SAVES:            return "saves";
      case APPLY_SAVING_MALEDICT:  return "save vs maledictions";
      case APPLY_SAVING_TRANSPORT: return "save vs transport";
      case APPLY_SAVING_BREATH:    return "save vs breath";
      case APPLY_SAVING_SPELL:     return "save vs spell";
      case APPLY_SPELL_AFFECT:     return "none";
      case APPLY_MORPH_FORM:       return "form";
      case APPLY_REGENERATION:     return "regeneration";
      case APPLY_ACCURACY:         return "accuracy";
      case APPLY_ATTACKS:          return "attack speed";
      case APPLY_MN_REGENERATION:  return "mana regeneration";
      case APPLY_MAX_STR:          return "max strength";
      case APPLY_ALIGN:            return "alignment";
   }

   bug( "Affect_location_name: unknown location %d.", location );
   return "(unknown)";
}

/* Get the ascii name of a single bit vector */
char* vector_bit_name_1(int vector, const FLAG_TYPE* flags)
{
   static char buf[1024];
   int cnt;

   buf[0] = '\0';
   /* Vector 1 */
   for (cnt = 0; flags[cnt].name != NULL; cnt++)
   {
      if (vector & flags[cnt].bit)
      {
         strlcat(buf, " ", 1024);
         strlcat(buf, flags[cnt].name, 1024);
      }
   }
   return (buf[0] != '\0') ? buf + 1 : "none";
}

/* Get the ascii name of a double bit vector */
char* vector_bit_name_2(int vector, int vector2, const FLAG_TYPE* flags, const FLAG_TYPE* flags2)
{
   static char buf[1024];
   static char* beginning = buf + 1;

   int cnt;

   buf[0] = '\0';
   /* Vector 1 */
   for (cnt = 0; flags[cnt].name != NULL; cnt++)
   {
      if (vector & flags[cnt].bit)
      {
         strlcat(buf, " ", 1024);
         strlcat(buf, flags[cnt].name, 1024);
      }
   }
   /* Vector 2 */
   for (cnt = 0; flags2[cnt].name != NULL; cnt++)
   {
      if (vector2 & flags2[cnt].bit)
      {
         strlcat(buf, " ", 1024);
         strlcat(buf, flags2[cnt].name, 1024);
      }
   }
   return (buf[0] != '\0') ? beginning : "none";
}

/*
* Return ascii name of an affect bit vector.
*/
char* affect_bit_name(int vector, int vector2)
{
   return vector_bit_name_2(vector, vector2, affect_flags, affect2_flags);
}

/*
* Return ascii name of extra flags vector.
*/
char *extra_bit_name(int vector, int vector2)
{
   return vector_bit_name_2(vector, vector2, extra_flags, extra2_flags);
}

/*
* Return ascii name of movement flags vector.
*/
char* movement_bit_name(int vector)
{
   return vector_bit_name_1(vector, mount_flags);
}

/* Return ascii name for restrict flags */
char* restrict_bit_name(int vector)
{
   return vector_bit_name_1(vector, restrict_flags);
}

/* return ascii name of an act vector */
char* act_bit_name(int vector, int vector2)
{
   static char buf[1024];
   char* bits;

   if (IS_SET(vector, ACT_IS_NPC))
   {
      return vector_bit_name_2(vector, vector2, act_flags, act2_flags);
   }
   strcpy(buf, "player");
   bits = vector_bit_name_2(vector, vector2, plr_flags, plr2_flags);
   if (str_cmp(bits, "none"))
   {
      strlcat(buf, " ", 1024);
      strlcat(buf, bits, 1024);
   }
   return buf;
}

char* comm_bit_name(int vector, int vector2)
{
   return vector_bit_name_2(vector, vector2, comm_flags, comm2_flags);
}

char* imm_bit_name(int vector)
{
   return vector_bit_name_1(vector, imm_flags);
}

/* TODO TODO all bit_name functions below */

char *wear_bit_name(int wear_flags)
{
   static char buf[512];

   buf [0] = '\0';
   if (wear_flags & ITEM_TAKE            ) strcat(buf, " take");
   if (wear_flags & ITEM_WEAR_FINGER     ) strcat(buf, " finger");
   if (wear_flags & ITEM_WEAR_NECK       ) strcat(buf, " neck");
   if (wear_flags & ITEM_WEAR_BODY       ) strcat(buf, " torso");
   if (wear_flags & ITEM_WEAR_HEAD       ) strcat(buf, " head");
   if (wear_flags & ITEM_WEAR_LEGS       ) strcat(buf, " legs");
   if (wear_flags & ITEM_WEAR_FEET       ) strcat(buf, " feet");
   if (wear_flags & ITEM_WEAR_HANDS      ) strcat(buf, " hands");
   if (wear_flags & ITEM_WEAR_ARMS       ) strcat(buf, " arms");
   if (wear_flags & ITEM_WEAR_SHIELD     ) strcat(buf, " shield");
   if (wear_flags & ITEM_WEAR_ABOUT      ) strcat(buf, " body");
   if (wear_flags & ITEM_WEAR_WAIST      ) strcat(buf, " waist");
   if (wear_flags & ITEM_WEAR_WRIST      ) strcat(buf, " wrist");
   if (wear_flags & ITEM_WIELD           ) strcat(buf, " wield");
   if (wear_flags & ITEM_HOLD            ) strcat(buf, " hold");
   if (wear_flags & ITEM_NO_SAC          ) strcat(buf, " nosac");
   if (wear_flags & ITEM_WEAR_FLOAT      ) strcat(buf, " float");
   if (wear_flags & ITEM_WEAR_BRAND      ) strcat(buf, " brand");
   if (wear_flags & ITEM_KEEP            ) strcat(buf, " keep");
   if (wear_flags & ITEM_WEAR_HORNS      ) strcat(buf, " horns");
   if (wear_flags & ITEM_WEAR_SNOUT      ) strcat(buf, " snout");
   if (wear_flags & ITEM_WEAR_HOOVES     ) strcat(buf, " hooves");
   if (wear_flags & ITEM_WEAR_FOURLEGS   ) strcat(buf, " fourlegs");
   if (wear_flags & ITEM_WEAR_FOURHOOVES ) strcat(buf, " fourhooves");
   if (wear_flags & ITEM_WEAR_EYES       ) strcat(buf, " eyes");
   if (wear_flags & ITEM_WEAR_EAR        ) strcat(buf, " ear");
   if (wear_flags & ITEM_WEAR_BOSOM      ) strcat(buf, " bosom");

   {
   }

   return ( buf[0] != '\0' ) ? buf+1 : "none";
}

char *form_bit_name(int form_flags)
{
   static char buf[512];

   buf[0] = '\0';
   if (form_flags & FORM_POISON    ) strcat(buf, " poison");
   else if (form_flags & FORM_EDIBLE    ) strcat(buf, " edible");
   if (form_flags & FORM_MAGICAL    ) strcat(buf, " magical");
   if (form_flags & FORM_INSTANT_DECAY    ) strcat(buf, " instant_rot");
   if (form_flags & FORM_OTHER        ) strcat(buf, " other");
   if (form_flags & FORM_ANIMAL    ) strcat(buf, " animal");
   if (form_flags & FORM_SENTIENT    ) strcat(buf, " sentient");
   if (form_flags & FORM_UNDEAD    ) strcat(buf, " undead");
   if (form_flags & FORM_CONSTRUCT    ) strcat(buf, " construct");
   if (form_flags & FORM_MIST        ) strcat(buf, " mist");
   if (form_flags & FORM_INTANGIBLE    ) strcat(buf, " intangible");
   if (form_flags & FORM_BIPED        ) strcat(buf, " biped");
   if (form_flags & FORM_CENTAUR    ) strcat(buf, " centaur");
   if (form_flags & FORM_INSECT    ) strcat(buf, " insect");
   if (form_flags & FORM_SPIDER    ) strcat(buf, " spider");
   if (form_flags & FORM_CRUSTACEAN    ) strcat(buf, " crustacean");
   if (form_flags & FORM_WORM        ) strcat(buf, " worm");
   if (form_flags & FORM_BLOB        ) strcat(buf, " blob");
   if (form_flags & FORM_MAMMAL    ) strcat(buf, " mammal");
   if (form_flags & FORM_BIRD        ) strcat(buf, " bird");
   if (form_flags & FORM_REPTILE    ) strcat(buf, " reptile");
   if (form_flags & FORM_SNAKE        ) strcat(buf, " snake");
   if (form_flags & FORM_DRAGON    ) strcat(buf, " dragon");
   if (form_flags & FORM_AMPHIBIAN    ) strcat(buf, " amphibian");
   if (form_flags & FORM_FISH        ) strcat(buf, " fish");
   if (form_flags & FORM_COLD_BLOOD     ) strcat(buf, " cold_blooded");
   if (form_flags & FORM_PLANT     ) strcat(buf, " plant");

   return ( buf[0] != '\0' ) ? buf+1 : "none";
}

char *part_bit_name(int part_flags)
{
   static char buf[512];

   buf[0] = '\0';
   if (part_flags & PART_HEAD        ) strcat(buf, " head");
   if (part_flags & PART_ARMS        ) strcat(buf, " arms");
   if (part_flags & PART_LEGS        ) strcat(buf, " legs");
   if (part_flags & PART_HEART        ) strcat(buf, " heart");
   if (part_flags & PART_BRAINS    ) strcat(buf, " brains");
   if (part_flags & PART_GUTS        ) strcat(buf, " guts");
   if (part_flags & PART_HANDS        ) strcat(buf, " hands");
   if (part_flags & PART_FEET        ) strcat(buf, " feet");
   if (part_flags & PART_FINGERS    ) strcat(buf, " fingers");
   if (part_flags & PART_EAR        ) strcat(buf, " ears");
   if (part_flags & PART_EYE        ) strcat(buf, " eyes");
   if (part_flags & PART_LONG_TONGUE    ) strcat(buf, " long_tongue");
   if (part_flags & PART_EYESTALKS    ) strcat(buf, " eyestalks");
   if (part_flags & PART_TENTACLES    ) strcat(buf, " tentacles");
   if (part_flags & PART_FINS        ) strcat(buf, " fins");
   if (part_flags & PART_WINGS        ) strcat(buf, " wings");
   if (part_flags & PART_TAIL        ) strcat(buf, " tail");
   if (part_flags & PART_CLAWS        ) strcat(buf, " claws");
   if (part_flags & PART_FANGS        ) strcat(buf, " fangs");
   if (part_flags & PART_HORNS        ) strcat(buf, " horns");
   if (part_flags & PART_SCALES    ) strcat(buf, " scales");
   if (part_flags & PART_TUSKS    ) strcat(buf, " tusks");
   if (part_flags & PART_STALKS    ) strcat(buf, " stalks");
   if (part_flags & PART_ROOTS    ) strcat(buf, " roots");

   return ( buf[0] != '\0' ) ? buf+1 : "none";
}

char *weapon_bit_name(int weapon_flags)
{
   static char buf[512];

   buf[0] = '\0';
   if (weapon_flags & WEAPON_FLAMING    ) strcat(buf, " flaming");
   if (weapon_flags & WEAPON_FROST    ) strcat(buf, " frost");
   if (weapon_flags & WEAPON_VAMPIRIC    ) strcat(buf, " vampiric");
   if (weapon_flags & WEAPON_SHARP    ) strcat(buf, " sharp");
   if (weapon_flags & WEAPON_PIERCING  ) strcat(buf, " armor-piercing");
   if (weapon_flags & WEAPON_VORPAL    ) strcat(buf, " vorpal");
   if (weapon_flags & WEAPON_TWO_HANDS ) strcat(buf, " two-handed");
   if (weapon_flags & WEAPON_SHOCKING     ) strcat(buf, " shocking");
   if (weapon_flags & WEAPON_POISON    ) strcat(buf, " poison");
   if (weapon_flags & WEAPON_DROWNING  ) strcat(buf, " drowning");
   if (weapon_flags & WEAPON_RESONATING) strcat(buf, " resonating");
   if (weapon_flags & WEAPON_DRAINING)   strcat(buf, " draining");
   if (weapon_flags & WEAPON_FATIGUING)  strcat(buf, " fatiguing");
   return ( buf[0] != '\0' ) ? buf+1 : "none";
}

char *cont_bit_name( int cont_flags)
{
   static char buf[512];

   buf[0] = '\0';

   if (cont_flags & CONT_CLOSEABLE    ) strcat(buf, " closable");
   if (cont_flags & CONT_PICKPROOF    ) strcat(buf, " pickproof");
   if (cont_flags & CONT_CLOSED    ) strcat(buf, " closed");
   if (cont_flags & CONT_LOCKED    ) strcat(buf, " locked");

   return (buf[0] != '\0' ) ? buf+1 : "none";
}


char *off_bit_name(int off_flags)
{
   static char buf[512];

   buf[0] = '\0';

   if (off_flags & OFF_AREA_ATTACK    ) strcat(buf, " area attack");
   if (off_flags & OFF_BACKSTAB    ) strcat(buf, " backstab");
   if (off_flags & OFF_BASH        ) strcat(buf, " bash");
   if (off_flags & OFF_BERSERK        ) strcat(buf, " berserk");
   if (off_flags & OFF_DISARM        ) strcat(buf, " disarm");
   if (off_flags & OFF_DODGE        ) strcat(buf, " dodge");
   if (off_flags & OFF_FADE        ) strcat(buf, " fade");
   if (off_flags & OFF_FAST        ) strcat(buf, " fast");
   if (off_flags & OFF_KICK        ) strcat(buf, " kick");
   if (off_flags & OFF_PUGIL       ) strcat(buf, " pugil");
   if (off_flags & OFF_LUNGE       ) strcat(buf, " lunge");
   if (off_flags & OFF_NINJITSU    ) strcat(buf, " ninjitsu");
   if (off_flags & OFF_FAST_TRACK  ) strcat(buf, " fast_tracker");
   if (off_flags & OFF_KILLER      ) strcat(buf, " killer_instinct");
   if (off_flags & OFF_KICK_DIRT    ) strcat(buf, " kick_dirt");
   if (off_flags & OFF_PARRY        ) strcat(buf, " parry");
   if (off_flags & OFF_RESCUE        ) strcat(buf, " rescue");
   if (off_flags & OFF_TAIL        ) strcat(buf, " tail");
   if (off_flags & OFF_TRIP        ) strcat(buf, " trip");
   if (off_flags & OFF_CRUSH        ) strcat(buf, " crush");
   if (off_flags & ASSIST_ALL        ) strcat(buf, " assist_all");
   if (off_flags & ASSIST_ALIGN    ) strcat(buf, " assist_align");
   if (off_flags & ASSIST_RACE        ) strcat(buf, " assist_race");
   if (off_flags & ASSIST_PLAYERS    ) strcat(buf, " assist_players");
   if (off_flags & ASSIST_GUARD    ) strcat(buf, " assist_guard");
   if (off_flags & ASSIST_VNUM        ) strcat(buf, " assist_vnum");
   if (off_flags & NO_TRACK        ) strcat(buf, " no_track");
   if (off_flags & STATIC_TRACKING    ) strcat(buf, " static_track");
   if (off_flags & SPAM_MURDER    ) strcat(buf, " spam_murder");
   return ( buf[0] != '\0' ) ? buf+1 : "none";
}

/*
* Return ascii name of room flags.
*/
char* room_bit_name(int vector, int vector2)
{
   return vector_bit_name_2(vector, vector2, room_flags, extra_room_flags);
}

/*
* Return ascii name of an affect bit vector.
*/
const char* wear_slot_name(int slot)
{
   if (slot == -1)
   {
      return "none";
   }
   else if
   (
      slot < 0 ||
      slot >= MAX_WEAR
   )
   {
      return "error";
   }
   return wear_slot_names[slot];
}

void wear_obj_19002(CHAR_DATA *ch, OBJ_DATA *obj)
{
   send_to_char("You feel more resistant to charm.\n\r", ch);
   return;
}

void remove_obj_19002(CHAR_DATA *ch, OBJ_DATA *obj)
{
   send_to_char("You feel less resistant to charm.\n\r", ch);
   return;
}

void wear_obj_channeling(CHAR_DATA *ch, OBJ_DATA *obj)
{
   send_to_char("You feel yourself becoming more ethereal.\n\r", ch);
   send_to_char("You begin to glow with a faint pink aura as the power surrounds you.\n\r", ch);
   return;
}

void remove_obj_channeling(CHAR_DATA *ch, OBJ_DATA *obj)
{
   int sn_faerie;
   int sn_passdoor;

   sn_passdoor = gsn_pass_door;
   sn_faerie = gsn_faerie_fire;

   if (is_affected(ch, sn_passdoor) )
   send_to_char("You feel more solid again.\n\r", ch);

   send_to_char("The aura of power around you fades.\n\r", ch);
   return;
}

void wear_obj_dragonmage_feet(CHAR_DATA *ch, OBJ_DATA *obj)
{
   AFFECT_DATA af;

   if (IS_AFFECTED(ch, AFF_FLYING))
   return;
   act("$n's feet slowly rise off the ground.", ch, NULL, NULL, TO_ROOM);
   act("Your feet slowly rise off the ground!", ch, NULL, NULL, TO_CHAR);

   af.type = gsn_fly;
   af.where = TO_AFFECTS;
   af.duration = -1;
   af.level = 51;
   af.modifier = 0;
   af.bitvector = AFF_FLYING;
   af.location = 0;
   affect_to_char(ch, &af);
   return;
}

void remove_obj_dragonmage_feet(CHAR_DATA *ch, OBJ_DATA *obj)
{
   int sn_fly;
   sn_fly = gsn_fly;

   if (!IS_AFFECTED(ch, AFF_FLYING))
   {
      affect_strip(ch, sn_fly);
      return;
   }
   act("You slowly float to the ground.", ch, NULL, NULL, TO_CHAR);
   act("$n slowly floats to the ground.", ch, NULL, NULL, TO_ROOM);
   affect_strip(ch, sn_fly);
   return;
}

void wear_obj_blue_hide(CHAR_DATA *ch, OBJ_DATA *obj)
{
   act("You feel resistant to electricity.", ch, NULL, NULL, TO_CHAR);
   return;
}

void remove_obj_blue_hide(CHAR_DATA *ch, OBJ_DATA *obj)
{
   act("You feel less resistant to electricity.", ch, NULL, NULL, TO_CHAR);

   return;
}

void wear_obj_green_hide(CHAR_DATA *ch, OBJ_DATA *obj)
{
   act("You feel much more resistant to poisons.", ch, NULL, NULL, TO_CHAR);
   return;
}

void remove_obj_green_hide(CHAR_DATA *ch, OBJ_DATA *obj)
{
   act("You feel less resistant to poisons.", ch, NULL, NULL, TO_CHAR);
   return;
}

/* Functions for messaging */

/*
* The message_to_xxx series of functions takes as arguments the target of
* the message and a pointer to the message, similar to the affect_to_xxx
* series of functions.  Based on the type of message and the function, it
* sets the target_type and target values of the message, duplicates the
* string if one exists, and adds the new message to the message_list of the
* target.  If the message is flagged MESSAGE_UNIQUE, then it does NOT add
* the new message of a message of that type already exists.  If the message
* is flagged both MESSAGE_UNIQUE and MESSAGE_REPLACE, it will remove the
* old message of that type (if one exists) before adding the new one.
* Also, before returning, the message is added to the message polling
* lists, based one MESSAGE_POLL_PULSE and MESSAGE_POLL_TICK flags.  Note
* that only one of these flags will apply to a given message, with a
* preference for POLL_PULSE.
*/


bool    message_to_obj( OBJ_DATA *obj, MESSAGE *message )
{
   MESSAGE *message_new;
   MESSAGE *message_old = NULL;

   if ( !(message_new = new_message()) )
   {
      bug("Message_to_obj: NULL new message address.", 0);
      return FALSE;
   }

   *message_new    = *message;

   if ( message->message_string )
   message_new->message_string = str_dup( message->message_string );

   message_new->next    = NULL;
   message_new->next_to_poll = NULL;

   if ( !(message = obj->message_list) )
   {
      obj->message_list = message_new;
   } else
   {

      if ( message->message_type == message_new->message_type
      && IS_SET(message_new->message_flags, MESSAGE_UNIQUE) )
      {
         if ( IS_SET(message_new->message_flags, MESSAGE_REPLACE) )
         {
            message_old = message;
         } else
         {
            free_message( message_new );
            return FALSE;
         }
      }

      while( message->next != NULL )
      {
         message = message->next;
         if ( message->message_type == message_new->message_type
         && IS_SET(message_new->message_flags, MESSAGE_UNIQUE) )
         {
            if ( IS_SET(message_new->message_flags, MESSAGE_REPLACE) )
            {
               message_old = message;
            } else
            {
               free_message( message_new );
               return FALSE;
            }
         }
      }
      message->next = message_new;

      if ( message_old )
      {
         message_remove( message_old );
      }

   }

   if ( IS_SET(message_new->message_flags, MESSAGE_POLL_PULSE) )
   {
      message_new->next_to_poll = message_pulse_list;
      message_pulse_list = message_new;
   } else
   if ( IS_SET(message_new->message_flags, MESSAGE_POLL_TICK) )
   {
      message_new->next_to_poll = message_tick_list;
      message_tick_list = message_new;
   }

   message_new->target = obj;
   message_new->target_type = MESSAGE_TARGET_OBJECT;

   return TRUE;
}

bool    message_to_char( CHAR_DATA *ch, MESSAGE *message )
{
   MESSAGE *message_new;
   MESSAGE *message_old = NULL;

   if ( !(message_new = new_message()) )
   {
      bug("Message_to_obj: NULL new message address.", 0);
      return FALSE;
   }

   *message_new    = *message;

   if ( message->message_string )
   message_new->message_string = str_dup( message->message_string );

   message_new->next    = NULL;
   message_new->next_to_poll = NULL;

   if ( !(message = ch->message_list) )
   {
      ch->message_list = message_new;
   } else
   {
      if ( message->message_type == message_new->message_type
      && IS_SET(message_new->message_flags, MESSAGE_UNIQUE) )
      {
         if ( IS_SET(message_new->message_flags, MESSAGE_REPLACE) )
         {
            message_old = message;
         } else
         {
            free_message( message_new );
            return FALSE;
         }
      }

      while( message->next != NULL )
      {
         message = message->next;
         if ( message->message_type == message_new->message_type
         && IS_SET(message_new->message_flags, MESSAGE_UNIQUE) )
         {
            if ( IS_SET(message_new->message_flags, MESSAGE_REPLACE) )
            {
               message_old = message;
            } else
            {
               free_message( message_new );
               return FALSE;
            }
         }
      }
      message->next = message_new;

      if ( message_old )
      {
         message_remove( message_old );
      }

   }

   if ( IS_SET(message_new->message_flags, MESSAGE_POLL_PULSE) )
   {
      message_new->next_to_poll = message_pulse_list;
      message_pulse_list = message_new;
   } else
   if ( IS_SET(message_new->message_flags, MESSAGE_POLL_TICK) )
   {
      message_new->next_to_poll = message_tick_list;
      message_tick_list = message_new;
   }

   message_new->target = ch;
   message_new->target_type = MESSAGE_TARGET_CHAR;

   return TRUE;
}

/*
* message_remove does exactly that.  It removes a message from existence.
* It takes a pointer to the message in question, removes it from the list
* of its target and removes it from the polling lists, then frees the
* memory associated with the message.
*/

void    message_remove( MESSAGE *message )
{
   OBJ_DATA *obj = NULL;
   CHAR_DATA *ch = NULL;
   MESSAGE *prev = NULL;

   if ( !message )
   {
      bug( "Message_remove: NULL message.", 0 );
      return;
   }

   switch( message->target_type )
   {
      case MESSAGE_TARGET_OBJECT:
      if ( ((obj = (OBJ_DATA *)message->target))->message_list == NULL )
      {
         bug( "Message_remove_object: no message list.", 0 );
         return;
      }

      if ( message == obj->message_list )
      {
         obj->message_list = message->next;
      } else
      {
         for ( prev = obj->message_list; prev != NULL; prev = prev->next )
         {
            if ( prev->next == message )
            {
               prev->next = message->next;
               break;
            }
         }

         if ( prev == NULL )
         {
            bug( "Message_remove_object: cannot find message.", 0 );
            return;
         }
      }
      break;

      case MESSAGE_TARGET_CHAR:
      if ( ((ch = (CHAR_DATA *)message->target))->message_list == NULL )
      {
         bug( "Message_remove_char: no message list.", 0 );
         return;
      }

      if ( message == ch->message_list )
      {
         ch->message_list = message->next;
      } else
      {
         for ( prev = ch->message_list; prev != NULL; prev = prev->next )
         {
            if ( prev->next == message )
            {
               prev->next = message->next;
               break;
            }
         }

         if ( prev == NULL )
         {
            bug( "Message_remove_char: cannot find message.", 0 );
            return;
         }
      }
      break;

      case MESSAGE_TARGET_ROOM:
      break;
   }

   if ( IS_SET(message->message_flags, MESSAGE_POLL_PULSE) )
   {
      if ( message_pulse_list == message )
      {
         message_pulse_list = message->next_to_poll;
      } else
      {
         for ( prev = message_pulse_list; prev != NULL; prev = prev->next_to_poll )
         {
            if ( prev->next_to_poll == message )
            {
               prev->next_to_poll = message->next_to_poll;
               break;
            }
         }
      }
   }

   if ( IS_SET(message->message_flags, MESSAGE_POLL_TICK) )
   {
      if ( message_tick_list == message )
      {
         message_tick_list = message->next_to_poll;
      } else
      {
         for ( prev = message_tick_list; prev != NULL; prev = prev->next_to_poll )
         {
            if ( prev->next_to_poll == message )
            {
               prev->next_to_poll = message->next_to_poll;
               break;
            }
         }
      }
   }

   free_message(message);

   return;
}

/*
* The message_strip_xxx series removes all messages of the passed message
* type from the passed object, character, or room.
*/

void    message_strip_obj( OBJ_DATA *obj, long int message_type )
{
   MESSAGE *message;
   MESSAGE *message_next;

   for ( message = obj->message_list; message != NULL; message = message_next )
   {
      message_next = message->next;
      if ( message->message_type == message_type )
      message_remove( message );
   }

   return;
}

/*
* The get_message_xxx series will return a pointer to the first message of
* the passed message_type from the passed target.
*/

MESSAGE* get_message_obj( OBJ_DATA *obj, long int message_type )
{
   MESSAGE *message;

   for ( message = obj->message_list; message != NULL; message = message->next )
   {
      if ( message->message_type == message_type )
      return message;
   }

   return NULL;
}

MESSAGE* get_message_char( CHAR_DATA *ch, long int message_type )
{
   MESSAGE *message;

   for ( message = ch->message_list; message != NULL; message = message->next )
   {
      if ( message->message_type == message_type )
      return message;
   }

   return NULL;
}

/*
* next_message and prev_message both take as arguments a pointer to a
* message, and will return the next message of the same type or the
* previous message with the same type.
*/

MESSAGE* next_message( MESSAGE* origin )
{
   MESSAGE *message;

   for ( message = origin->next; message != NULL; message = message->next )
   {
      if ( message->message_type == origin->message_type )
      return message;
   }

   return NULL;
}

MESSAGE* prev_message( MESSAGE* origin )
{
   MESSAGE *message;
   MESSAGE *last_valid;

   switch( origin->target_type )
   {
      case MESSAGE_TARGET_OBJECT:
      message = ((OBJ_DATA *)origin->target)->message_list;
      break;
      case MESSAGE_TARGET_CHAR:
      message = ((CHAR_DATA *)origin->target)->message_list;
      break;
      default:
      return NULL;
   }

   for ( last_valid = NULL; message != NULL; message = message->next )
   {
      if ( message == origin )
      return last_valid;

      if ( message->message_type == origin->message_type )
      last_valid = message;
   }

   return NULL;
}

/*
* The count_message_xxx series returns the number of messages of the passed
* message_type that are applied to the passed target.  Nothing is done to
* the messages other than counting them.
*/

int count_message_obj( OBJ_DATA *obj, long int message_type )
{
   int count = 0;
   MESSAGE *message;

   for ( message = obj->message_list; message != NULL; message = message->next )
   {
      if ( message->message_type == message_type )
      count++;
   }

   return count;
}

int count_message_char( CHAR_DATA *ch, long int message_type )
{
   int count = 0;
   MESSAGE *message;

   for ( message = ch->message_list; message != NULL; message = message->next )
   {
      if ( message->message_type == message_type )
      count++;
   }

   return count;
}

/*
* This function is mostly a throwback to affect handling, I'm not sure
* it will really be all that useful, though I suppose it could be.
* It looks for a message of the same type as itself and adds its
* message_data to it, restricted by the limit passed.  Note that right
* now, if there is no other message like it, the message does NOT get
* added.  I'll fix that later.
*/

void    message_join_obj( OBJ_DATA *obj, MESSAGE *message, sh_int limit )
{
   MESSAGE *message_old;
   bool negative;

   negative = (message->message_data < 0);

   for ( message_old = obj->message_list; message_old != NULL; message_old = message_old->next )
   {
      if ( message_old->message_type == message->message_type )
      {
         message_old->message_data += message->message_data;

         if ( limit > 0 && abs(message_old->message_data) > limit )
         message_old->message_data = (negative ? -limit:limit);

         break;
      }
   }

   free_message( message );
   return;
}

void    message_join_char( CHAR_DATA *ch, MESSAGE *message, sh_int limit )
{
   MESSAGE *message_old;
   bool negative;

   negative = (message->message_data < 0);

   for ( message_old = ch->message_list; message_old != NULL; message_old = message_old->next )
   {
      if ( message_old->message_type == message->message_type )
      {
         message_old->message_data += message->message_data;

         if ( limit > 0 && abs(message_old->message_data) > limit )
         message_old->message_data = (negative ? -limit:limit);

         break;
      }
   }

   free_message( message );
   return;
}

int see_state( CHAR_DATA *ch, CHAR_DATA *victim )
{
   if ( can_see(ch, victim) )
   return SEE_TARGET;

   if ( IS_AFFECTED(ch, AFF_BLIND)
   && is_affected(ch, gsn_blind_fighting) )
   {
      if ( !is_affected(victim, gsn_earthfade) )
      return SEE_TARGET_BF;
   }

   return NO_SEE_TARGET;
}

int hometown_lookup( int temple_vnum)
{
   int ret_val = 0;

   while(hometown_table[ret_val].vnum != 0)
   {
      if (hometown_table[ret_val].vnum == temple_vnum)
      return ret_val;
      ret_val++;
   }
   sprintf(log_buf, "Bug in hometown_lookup, hometown %d not found.",
   temple_vnum);
   log_string(log_buf);
   return 0;
}

void evaluate_wounds(CHAR_DATA *ch, CHAR_DATA *victim)
{
   char buf[MAX_STRING_LENGTH];
   int  percent;

   if ( victim->max_hit > 0 )
   percent = ( 100 * victim->hit ) / victim->max_hit;
   else
   percent = -1;
   strcpy( buf, get_descr_form(victim, ch, FALSE) );

   if (IS_SET(ch->act, PLR_EVALUATION))
   {
      if (percent >= 100)
      strcat(buf, " is in perfect condition.\n\r");
      else if (percent >= 95)
      strcat(buf, " has a few scratches.\n\r");
      else if (percent >= 90)
      strcat(buf, " has a few bruises.\n\r");
      else if (percent >= 80)
      strcat(buf, " has some small wounds.\n\r");
      else if (percent >= 70)
      strcat(buf, " has quite a few wounds.\n\r");
      else if (percent >= 60)
      strcat(buf, " is covered in bleeding wounds.\n\r");
      else if (percent >= 50)
      strcat(buf, " is bleeding profusely.\n\r");
      else if (percent >= 40)
      strcat(buf, " is gushing blood.\n\r");
      else if (percent >= 30)
      strcat(buf, " is screaming in pain.\n\r");
      else if (percent >= 20)
      strcat(buf, " is spasming in shock.\n\r");
      else if (percent >= 10)
      strcat(buf, " is writhing in agony.\n\r");
      else if (percent >= 1)
      strcat(buf, " is convulsing on the ground.\n\r");
      else
      strcat(buf, " is nearly dead.\n\r");
   }
   else
   {
      if (percent >= 100)
      strcat( buf, " is in perfect condition.\n\r");
      else if (percent >= 90)
      strcat( buf, " has a few scratches.\n\r");
      else if (percent >= 75)
      strcat( buf, " has some small wounds.\n\r");
      else if (percent >=  50)
      strcat( buf, " has some nasty cuts.\n\r");
      else if (percent >= 30)
      strcat( buf, " is bleeding profusely.\n\r");
      else if (percent >= 15)
      strcat( buf, " is screaming in pain.\n\r");
      else if (percent >= 0 )
      strcat( buf, " is in pretty bad shape.\n\r");
      else
      strcat(buf, " is nearly dead.\n\r");
   }

   buf[0] = UPPER(buf[0]);
   send_to_char( buf, ch );
   return;
}

void do_vote(CHAR_DATA *ch, char * argument)
{
   int is_empire_election = TRUE;
   int current_election = 1;  /*increment this each new vote*/
   int cnt;
   char buf[MAX_STRING_LENGTH];

   /*   send_to_char("There are no current issues for voting.\n\r", ch);
   return;
   */
   if (IS_NPC(ch)) return;
   if (IS_IMMORTAL(ch))
   {
      /* report current results */
      if (!str_cmp(argument, "clear"))
      {
         for (cnt = 0; cnt < 20; cnt++)
         votes[cnt] = 0;
         save_globals();
         send_to_char("All votes cleared.\n\r", ch);
         return;
      }

      sprintf(buf, "Tindos    %d\n\r", votes[0]);
      send_to_char(buf, ch);
      sprintf(buf, "Karakus   %d\n\r", votes[1]);
      send_to_char(buf, ch);
      sprintf(buf, "Allara    %d\n\r", votes[2]);
      send_to_char(buf, ch);
      sprintf(buf, "Kraikof   %d\n\r", votes[3]);
      send_to_char(buf, ch);
      sprintf(buf, "Dorian    %d\n\r", votes[4]);
      send_to_char(buf, ch);
      sprintf(buf, "Tiam      %d\n\r", votes[5]);
      send_to_char(buf, ch);
      sprintf(buf, "Lartanus  %d\n\r", votes[6]);
      send_to_char(buf, ch);
      sprintf(buf, "Illunus   %d\n\r", votes[7]);
      send_to_char(buf, ch);
      sprintf(buf, "Zzarchov  %d\n\r", votes[8]);
      send_to_char(buf, ch);
      sprintf(buf, "Newt      %d\n\r", votes[9]);
      send_to_char(buf, ch);
      sprintf(buf, "Tanissa   %d\n\r", votes[10]);
      send_to_char(buf, ch);
      sprintf(buf, "Baghib    %d\n\r", votes[11]);
      send_to_char(buf, ch);

      return;
   }
   if (ch->pcdata->last_voted >= current_election)
   {
      send_to_char("You have already voted in this election.\n\r", ch);
      return;
   }
   if (is_empire_election)
   {
      if (ch->house == HOUSE_OUTLAW)
      {
         send_to_char("The Empire does not recognize an Outlaw's right to vote.\n\r", ch);
         return;
      }
      if (IS_SET(ch->act, PLR_CRIMINAL))
      {
         send_to_char("Those who are criminals may not vote.\n\r", ch);
         return;
      }
      if (ch->pcdata->wanteds > 5)
      {
         send_to_char("Those with notorious criminal records can not vote.\n\r", ch);
         return;
      }
      if (ch->level < 35)
      {
         send_to_char("You must be of the 35th rank to vote.\n\r", ch);
         return;
      }
      if (!str_cmp(argument, "help"))
      {
         send_to_char("Vote list, lists candidates for your area, vote <name>, votes on one.\n\r", ch);
         return;
      }
      if (!str_cmp(argument, "list") || argument[0] == '\0')
      {
         switch(ch->temple)
         {
            default:
            send_to_char("The Empire does not hold elections in your hometown.\n\r", ch);
            return;
            case 10681:
            case 10699:
            case 10670:
            send_to_char("Candidates for Glyndane: Tindos Karakus Allara Kraikof abstain\n\r", ch);
            return;
            case 8839:
            case 8844:
            case 8848:
            send_to_char("Candidates for Elvenhame: Newt abstain\n\r", ch);
            return;
            case 9609:
            send_to_char("Candidates for New Thalos: Tiam Lartanus Dorian abstain\n\r", ch);
            return;
            case 699:
            send_to_char("Candidates for Ofcol: Baghib Tanissa abstain\n\r", ch);
            return;
            case 3054:
            send_to_char("Candidates for Ethshar: Illunus Zzarchov abstain\n\r", ch);
            return;
         }
      }

      if (!str_cmp(argument, "abstain"))
      {
         send_to_char("You chose to abstain.\n\r", ch);
         ch->pcdata->last_voted = current_election;
         return;
      }
      switch(ch->temple)
      {
         default:
         send_to_char("The Empire does not hold elections in your hometown.\n\r", ch);
         return;
         case 10681:
         case 10699:
         case 10670:
         if (!str_cmp(argument, "tindos"))
         {
            send_to_char("You have voted for Tindos.\n\r", ch);
            votes[0]++;
            save_globals();
            ch->pcdata->last_voted = current_election;
            return;
         }
         if (!str_cmp(argument, "karakus"))
         {
            send_to_char("You have voted for Karakus.\n\r", ch);
            votes[1]++;
            save_globals();
            ch->pcdata->last_voted = current_election;
            return;
         }
         if (!str_cmp(argument, "allara"))
         {
            send_to_char("You have voted for Allara.\n\r", ch);
            votes[2]++;
            save_globals();
            ch->pcdata->last_voted = current_election;
            return;
         }
         if (!str_cmp(argument, "kraikof"))
         {
            send_to_char("You have voted for Kraikof.\n\r", ch);
            votes[3]++;
            save_globals();
            ch->pcdata->last_voted = current_election;
            return;
         }
         send_to_char("No candidate in your area has that name.\n\r", ch);
         return;
         case 8839:
         case 8844:
         case 8848:
         if (!str_cmp(argument, "newt"))
         {
            send_to_char("You have voted for Newt.\n\r", ch);
            votes[9]++;
            save_globals();
            ch->pcdata->last_voted = current_election;
            return;
         }
         send_to_char("No candidate in your area has that name.\n\r", ch);
         return;
         case 9609:
         if (!str_cmp(argument, "dorian"))
         {
            send_to_char("You have voted for Dorian.\n\r", ch);
            votes[4]++;
            save_globals();
            ch->pcdata->last_voted = current_election;
            return;
         }
         if (!str_cmp(argument, "tiam"))
         {
            send_to_char("You have voted for Tiam.\n\r", ch);
            votes[5]++;
            save_globals();
            ch->pcdata->last_voted = current_election;
            return;
         }
         if (!str_cmp(argument, "lartanus"))
         {
            send_to_char("You have voted for Lartanus.\n\r", ch);
            votes[6]++;
            save_globals();
            ch->pcdata->last_voted = current_election;
            return;
         }
         send_to_char("No candidate in your area has that name.\n\r", ch);
         return;
         case 699:
         if (!str_cmp(argument, "tanissa"))
         {
            send_to_char("You have voted for Tanissa.\n\r", ch);
            votes[10]++;
            save_globals();
            ch->pcdata->last_voted = current_election;
            return;
         }
         if (!str_cmp(argument, "baghib"))
         {
            send_to_char("You have voted for Baghib.\n\r", ch);
            votes[11]++;
            save_globals();
            ch->pcdata->last_voted = current_election;
            return;
         }
         send_to_char("No candidate in your area has that name.\n\r", ch);
         return;
         case 3054:
         if (!str_cmp(argument, "illunus"))
         {
            send_to_char("You have voted for Illunus.\n\r", ch);
            votes[7]++;
            save_globals();
            ch->pcdata->last_voted = current_election;
            return;
         }
         if (!str_cmp(argument, "zzarchov"))
         {
            send_to_char("You have voted for Zzarchov.\n\r", ch);
            votes[8]++;
            save_globals();
            ch->pcdata->last_voted = current_election;
            return;
         }
         send_to_char("No candidate in your area has that name.\n\r", ch);
         return;
      }
   }
   return;
}

char * get_scale_color(CHAR_DATA *ch)
{
   int scale;
   if (IS_NPC(ch)) return "";
   scale = ch->subrace;
   if (ch->race != grn_draconian) return "";
   if (!str_cmp(ch->name, "Xeonauz")) return " (Obsidian Scales)";
   if (!str_cmp(ch->name, "Rungekutta")) return " Utter Darkness";
   if (!str_cmp(ch->name, "Illunus")) return " (Platinum & Onyx Scales)";
   if (!str_cmp(ch->name, "Dielantha")) return " (Platinum Scales)";

   if (scale == 0 && ch->pcdata->molting_into == 0) return "";
   if (scale == SCALE_GOLD)   return " (Gold Scales)";
   if (scale == SCALE_SILVER) return " (Silver Scales)";
   if (scale == SCALE_COPPER) return " (Copper Scales)";
   if (scale == SCALE_BRONZE) return " (Bronze Scales)";
   if (scale == SCALE_BRASS)  return " (Brass scales)";
   if (scale == SCALE_BLACK)  return " (Black Scales)";
   if (scale == SCALE_WHITE)  return " (White Scales)";
   if (scale == SCALE_GREEN)  return " (Green Scales)";
   if (scale == SCALE_RED)    return " (Red Scales)";
   if (scale == SCALE_BLUE)   return " (Blue Scales)";
   switch (ch->pcdata->molting_into)
   {
      default:
      case 0: return "";
      case SCALE_BLACK: return " (Molting into Black)";
      case SCALE_WHITE: return " (Molting into White)";
      case SCALE_BLUE:  return " (Molting into Blue)";
      case SCALE_RED:   return " (Molting into Red)";
      case SCALE_GREEN: return " (Molting into Green)";
      case SCALE_GOLD:  return " (Molting into Gold)";
      case SCALE_SILVER: return " (Molting into Silver)";
      case SCALE_COPPER: return " (Molting into Copper)";
      case SCALE_BRONZE: return " (Molting into Bronze)";
      case SCALE_BRASS:  return " (Molting into Brass)";
   }
   return "";
}

void do_scalepick(CHAR_DATA *ch, char *argument)
{
   if (IS_NPC(ch) || ch->subrace != 0 || ch->race != grn_draconian)
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }
   if (ch->pcdata->molting_into != 0)
   {
      send_to_char("You have already begun molting into a new color.\n\r", ch);
      return;
   }
   if (!str_cmp(argument, "gold") && ch->alignment > 0)
   {
      ch->pcdata->molting_into = SCALE_GOLD;
      ch->pcdata->molting_time = 250;
      send_to_char("You begin to molt into a gold draconian.\n\r", ch);
      return;
   }
   if (!str_cmp(argument, "silver") && ch->alignment > 0)
   {
      ch->pcdata->molting_into = SCALE_SILVER;
      ch->pcdata->molting_time = 250;
      send_to_char("You begin to molt into a silver draconian.\n\r", ch);
      return;
   }
   if (!str_cmp(argument, "brass") && ch->alignment > 0)
   {
      ch->pcdata->molting_into = SCALE_BRASS;
      ch->pcdata->molting_time = 250;
      send_to_char("You begin to molt into a brass draconian.\n\r", ch);
      return;
   }
   if (!str_cmp(argument, "copper") && ch->alignment > 0)
   {
      ch->pcdata->molting_into = SCALE_COPPER;
      ch->pcdata->molting_time = 250;
      send_to_char("You begin to molt into a copper draconian.\n\r", ch);
      return;
   }
   if (!str_cmp(argument, "bronze") && ch->alignment > 0)
   {
      ch->pcdata->molting_into = SCALE_BRONZE;
      ch->pcdata->molting_time = 250;
      send_to_char("You begin to molt into a bronze draconian.\n\r", ch);
      return;
   }
   if (!str_cmp(argument, "green") && ch->alignment < 0)
   {
      ch->pcdata->molting_into = SCALE_GREEN;
      ch->pcdata->molting_time = 10;
      send_to_char("You begin to molt into a green draconian.\n\r", ch);
      return;
   }
   if (!str_cmp(argument, "blue") && ch->alignment < 0)
   {
      ch->pcdata->molting_into = SCALE_BLUE;
      ch->pcdata->molting_time = 250;
      send_to_char("You begin to molt into a blue draconian.\n\r", ch);
      return;
   }
   if (!str_cmp(argument, "red") && ch->alignment < 0)
   {
      ch->pcdata->molting_into = SCALE_RED;
      ch->pcdata->molting_time = 250;
      send_to_char("You begin to molt into a red draconian.\n\r", ch);
      return;
   }
   if (!str_cmp(argument, "black") && ch->alignment < 0)
   {
      ch->pcdata->molting_into = SCALE_BLACK;
      ch->pcdata->molting_time = 250;
      send_to_char("You begin to molt into a black draconian.\n\r", ch);
      return;
   }
   if (!str_cmp(argument, "white") && ch->alignment < 0)
   {
      ch->pcdata->molting_into = SCALE_WHITE;
      ch->pcdata->molting_time = 250;
      send_to_char("You begin to molt into a white draconian.\n\r", ch);
      return;
   }

   send_to_char("That is not a valid color for you.\n\r", ch);
   return;
}

void getLimb( CHAR_DATA * ch, char * limb )
{
   if
   (
      ch->race == grn_draconian ||
      ch->morph_form[0] == MORPH_BAT
   )
   {
      sprintf(limb, "claw" );
   }
   else if (ch->race == grn_arborian)
   {
      sprintf(limb, "leafy tentacle");
   }
   else if
   (
      ch->morph_form[0] == MORPH_WOLF ||
      ch->morph_form[0] == MORPH_BEAR
   )
   {
      sprintf( limb, "paw" );
   }
   else
   {
      sprintf( limb, "hand" );
   }
}

bool check_bloodlust( CHAR_DATA * ch )
{
   OBJ_DATA * wield;

   /* Immortals are not controlled by bloodlust. */
   if (
         ch == NULL ||
         ch->in_room == NULL ||
         IS_IMMORTAL(ch)
      )
   {
      return FALSE;
   }

   if (
         (wield = get_eq_char(ch, WEAR_WIELD)) != NULL &&
         wield->pIndexData->vnum != OBJ_VNUM_GENOCIDE_AXE
      )
   {
      wield = NULL;
   }

   if (
         !wield &&
         (wield = get_eq_char(ch, WEAR_DUAL_WIELD)) != NULL &&
         wield->pIndexData->vnum != OBJ_VNUM_GENOCIDE_AXE
      )
   {
      wield = NULL;
   }

   if (
         (is_affected(ch, gsn_dracolich_bloodlust) || wield) &&
         ch->position != POS_FIGHTING &&
         ch->position > POS_SLEEPING
         /*
         Fights can start in bloodlust if
         ch is resting, sitting, or standing.
         */
      )
   {
      /* Is there a PC in the room?  PCs in PK always get targetted.  */
      CHAR_DATA * vch;
      int pccount = 0, mobcount = 0;
      int target_found = 0;

      for (vch = ch->in_room->people; vch; vch = vch->next_in_room)
      {
         if (
               vch == ch ||                /* Cannot attack self */
               IS_IMMORTAL(vch) ||         /* Cannot attack Immortals */
               is_safe(ch, vch, IS_SAFE_SILENT)
            )
         {
            continue;
         }

         /* Counting the number of in-PK PCs in the room */
         if (!IS_NPC(vch))
         {
            pccount++;
         }

         /* Counting the number of mobs, charmed and not */
         if (IS_NPC(vch))
         {
            if (IS_AFFECTED(vch, AFF_CHARM))
            {
               mobcount++;
            }

            /* Remove this increment below to prevent normal mob attacks. */
            if (!IS_AFFECTED(vch, AFF_CHARM))
            {
               mobcount++;
            }
         }

         target_found = 1;
      }

      if (target_found)  /* We have a target! */
      {
         vch = NULL;

         if (pccount > 0) /* If the target is a PC, we will definitely attack */
         {
            int which_target = number_range(1, pccount);

            pccount = 0;
            for (vch = ch->in_room->people; vch; vch = vch->next_in_room)
            {
               if (
                     vch == ch ||                /* Cannot attack self */
                     IS_IMMORTAL(vch) ||         /* Cannot attack Immortals */
                     is_safe(ch, vch, IS_SAFE_SILENT)
                  )
               {
                  continue;
               }

               if (!IS_NPC(vch))  /* Counting the number of PCs in the room */
               {
                  pccount++;
                  if (pccount == which_target)
                  break;
               }
            }
         }
         else  /* Random mob attack */
         if ((number_percent() < 50) && (mobcount > 0))
         {
            int which_target = number_range(1, mobcount);

            mobcount = 0;
            for (vch = ch->in_room->people; vch; vch = vch->next_in_room)
            {
               if (
                     vch == ch ||                /* Cannot attack self */
                     IS_IMMORTAL(vch) ||         /* Cannot attack Immortals */
                     is_safe(ch, vch, IS_SAFE_SILENT)
                  )
               {
                  continue;
               }

               /* Counting the number of mobs, charmed and not */
               if (IS_NPC(vch))
               {
                  if (IS_AFFECTED(vch, AFF_CHARM))
                  {
                     mobcount++;
                  }

                  /* Remove this increment below to prevent normal
                   * mob attacks.
                   */
                  if (!IS_AFFECTED(vch, AFF_CHARM))
                  {
                     mobcount++;
                  }
               }

               if (mobcount == which_target)
               break;
            }
         }

         if (vch != NULL) /* If we have a target... */
         {
            char buf[MAX_STRING_LENGTH];

            ch->position = POS_STANDING;

            if (wield)
            {
               switch (number_range(0,2))
               {
                  case 0:
                  {
                     sprintf(buf, "Argh, the Black Knight is killing me!");
                     break;
                  }
                  case 1:
                  {
                     sprintf(buf, "Help! The Black Knight just attacked me!");
                     break;
                  }
                  case 2:
                  {
                     sprintf(
                        buf,
                        "Run for your lives! The Black Knight is upon us!");
                     break;
                  }
               }
               switch (number_range(0,5))
               {
                  case 0:
                  {
                     do_say(ch, "And now you die, insect!");
                     break;
                  }
                  case 1:
                  {
                     do_say(ch, "Ha ha ha, DIE, SHEEP!");
                     break;
                  }
                  case 2:
                  {
                     do_say(ch, "What have we here?!?!?!");
                     break;
                  }
                  case 3:
                  {
                     do_say(ch, "You are NEXT!!!");
                     break;
                  }
                  case 4:
                  {
                     do_say(ch, "The heavens shall rain with BLOOOOOOOOOOD!");
                     break;
                  }
                  case 5:
                  {
                     break;
                  }
               }
            }
            else
            {
               switch (number_range(0,2))
               {
                  case 0:
                  {
                     sprintf(buf, "Argh, the Dracolich is killing me!");
                     break;
                  }
                  case 1:
                  {
                     sprintf(buf, "Help! The Dracolich just attacked me!");
                     break;
                  }
                  case 2:
                  {
                     sprintf(
                        buf,
                        "Run for your lives! The Dracolich is upon us!");
                     break;
                  }
               }
            }

            if (!IS_NPC(vch))
            {
               do_myell(vch, buf);
               sprintf(
                  log_buf,
                  "[%s] bloodlusted [%s] at %d",
                  ch->name,
                  vch->name,
                  ch->in_room->vnum);
               log_string(log_buf);
            }
            multi_hit(ch, vch, TYPE_UNDEFINED);
         }
      }
   }

   return FALSE;
}

/*
if paf is an herb spell, it gets the herb name
or herb fade.  if paf is NULL, it uses sn_in as the sn
this gets the name of the herb version
of the spell/skill
if 'name' is false, returns the fade message
of the spell/skill
*/
char * get_herb_spell_name(AFFECT_DATA* paf, bool name, sh_int sn_in)
{
   int counter = 0;
   char buf[MAX_INPUT_LENGTH];
   sh_int sn = 0;

   if (paf == NULL && sn_in == 0)
   {
      bug("get_herb_spell_name: null paf", 0);
      return "";
   }
   if (paf != NULL)
   {
      sn = paf->type;
   }
   else
   {
      sn = sn_in;
   }
   if
   (
      sn < 1 ||
      sn >= MAX_SKILL
   )
   {
      bug("get_herb_spell_name: bad sn %d.", sn);
      return NULL;
   }
   if (paf != NULL && !IS_SET(paf->bitvector2, AFF_HERB_SPELL))
   {
      if (name)
      {
         return skill_table[sn].name;
      }
      else
      {
         if (paf->type == gsn_trapmaking)
         {
            switch (paf->modifier)
            {
               default:
               {
                  return "You can make another vine trap.";
               }
               case (1):
               {
                  return "You can make another snare.\n\r";
               }
               case (2):
               {
                  return "You can make another log trap.\n\r";
               }
               case (3):
               {
                  return "You can make more punjie sticks.\n\r";
               }
            }
         }
         return skill_table[sn].msg_off;
      }
   }
   for (counter = 0; herb_spells[counter].sn[0] != gsn_forage; counter++)
   {
      if (sn == herb_spells[counter].sn[0])
      {
         if (name && herb_spells[counter].new_name == NULL)
         {
            if (skill_table[sn].name != NULL)/* just in case sanity check */
            {
               sprintf
               (
                  buf,
                  "get_herb_spell_name: '%s' doesn't have a new 'herb name'",
                  skill_table[sn].name
               );
               bug(buf, 0);
            }
            else
            {
               bug("get_herb_spell_name: '%d' doesn't have a name", sn);
            }
            return skill_table[sn].name;
            /*
            return original name for now
            */
         }
         else
         if (!name && herb_spells[counter].new_fade == NULL)
         {
            if (skill_table[sn].name != NULL)
            {
               sprintf
               (
                  buf,
                  "get_herb_spell_name: '%s' doesn't have a new 'herb fade'",
                  skill_table[sn].name
               );
               bug(buf, 0);
            }
            else
            {
               bug("get_herb_spell_name: '%d' doesn't have an 'herb fade'", sn);
            }
            return skill_table[sn].msg_off;
            /*
            return original name for now
            */
         }
         else
         if (name)
         {
            return herb_spells[counter].new_name;
            /*
            returning NULL here is not fine, checked up there
            that its not NULL
            */
         }
         else
         {
            return herb_spells[counter].new_fade;
         }
      }
   }
   /*
   if (skill_table[sn].name != NULL)
   {
   sprintf
   (
   buf,
   "get_herb_spell_name: '%s' doesn't have an entry in the herb_spells table",
   skill_table[sn].name
   );
   bug(buf, 0);
   }
   else
   {
   bug("get_herb_spell_name: '%d' doesn't have a name, or herb name", sn);
   }
   */
   /*
   if it gets to here, it didn't find the sn in the list
   it needs a name
   */
   if (name)
   {
      return skill_table[sn].name;
   }
   else
   {
      return skill_table[sn].msg_off;
   }
}

void switch_desc(CHAR_DATA* ch, sh_int new_desc)
{
   PC_DATA* pc;
   sh_int counter;

   if (IS_NPC(ch))
   {
      /* Only PCs can have multiple descriptions */
      return;
   }
   if (new_desc >= MAX_DESCRIPTIONS)
   {
      bug("switch_desc over max, %s", new_desc);
      return;
   }
   if (new_desc < -1)
   {
      bug("switch_desc invalid, %s", new_desc);
      return;
   }
   pc = ch->pcdata;
   if (new_desc == -1)
   {
      /* Clear all multiple descriptions */

      /* Keeps current ch->description */
      for (counter = 0; counter < MAX_DESCRIPTIONS; counter++)
      {
         free_string(pc->desc_list[counter]);
         pc->desc_list[counter] = &str_empty[0];
      }
      pc->current_desc = -1;
      return;
   }
   if (pc->current_desc == new_desc)
   {
      /* No change unless new_desc was -1 (already done above) */
      return;
   }
   if (pc->current_desc == -1)
   {
      /* Old character, or no extras defined */

      /* Put current description in slot 0 */
      pc->desc_list[0] = ch->description;
      pc->current_desc = new_desc;
      ch->description = pc->desc_list[new_desc];
      /* In case new_desc is 0 */
      pc->desc_list[new_desc] = &str_empty[0];
      return;
   }
   /* Already has multiple descriptions */

   /* Current one is always stored in ch->description */
   pc->desc_list[pc->current_desc] = ch->description;
   ch->description = pc->desc_list[new_desc];
   pc->desc_list[new_desc] = &str_empty[0];
   pc->current_desc = new_desc;
}

int find_spell(CHAR_DATA* ch, const char* name)
{
   /* finds a spell the character can cast if possible */
   sh_int sn;
   sh_int first_found = -1;

   if (IS_NPC(ch))
   {
      return skill_lookup(name);
   }

   for (sn = 1; sn < MAX_SKILL; sn++)
   {
      if (skill_table[sn].name == NULL)
      {
         break;
      }
      if
      (
         LOWER(name[0]) != LOWER(skill_table[sn].name[0]) ||
         str_prefix(name, skill_table[sn].name)
      )
      {
         continue;
      }
      if (first_found == -1)
      {
         first_found = sn;
      }
      if
      (
         !has_skill(ch, sn) ||
         ch->pcdata->learned[sn] <= 0 ||
         skill_table[sn].spell_fun == spell_null
      )
      {
         continue;
      }
      return sn;
   }
   return first_found;
}

int find_skill_spell(CHAR_DATA* ch, const char* name)
{
   /* finds a skill or spell the character can cast if possible */
   sh_int sn;
   sh_int first_found = -1;

   if (IS_NPC(ch))
   {
      return skill_lookup(name);
   }

   for (sn = 1; sn < MAX_SKILL; sn++)
   {
      if (skill_table[sn].name == NULL)
      {
         break;
      }
      if
      (
         LOWER(name[0]) != LOWER(skill_table[sn].name[0]) ||
         str_prefix(name, skill_table[sn].name)
      )
      {
         continue;
      }
      if (first_found == -1)
      {
         first_found = sn;
      }
      if
      (
         !has_skill(ch, sn) ||
         ch->pcdata->learned[sn] <= 0
      )
      {
         continue;
      }
      return sn;
   }
   return first_found;
}

bool oblivion_blink(CHAR_DATA* ch, CHAR_DATA* victim)
{
   if
   (
      ch == victim ||
      ch->in_room == NULL ||
      victim->in_room == NULL ||
      victim->fighting != NULL ||
      ch->fighting == victim ||
      (
         IS_IMMORTAL(ch) &&
         get_trust(ch) > get_trust(victim)
      ) ||
      !is_affected(victim, gsn_will_dread) ||
      number_percent() > 40
   )
   {
      return FALSE;
   }
   act
   (
      "$N blinks out of existence momentarily.",
      ch,
      NULL,
      victim,
      TO_CHAR
   );
   act
   (
      "$n blinks out of existence momentarily.",
      victim,
      NULL,
      ch,
      TO_NOTVICT
   );
   send_to_char("You blink out of existence momentarily.\n\r", victim);
   return TRUE;
}

int gsn_corrupt(const char* skill, sh_int* gsn)
{
   int sn = *gsn;

   if
   (
      sn > 0 &&
      sn < MAX_SKILL &&
      skill_table[sn].pgsn == gsn &&
      !str_cmp(skill_table[sn].name, skill)
   )
   {
      return sn;
   }
   sn = skill_lookup(skill);
   if
   (
      sn != -1 &&
      skill_table[sn].pgsn != NULL &&
      skill_table[sn].pgsn[0] != sn
   )
   {
      wiznet(log_buf, NULL, NULL, WIZ_BUGS, 0, 0);
      do_gsndump(NULL, "fix");
   }
   if
   (
      sn != -1 &&
      skill_table[sn].pgsn != gsn
   )
   {
      sprintf
      (
         log_buf,
         "%s's pgsn not pointing to the gsn! pgsn: %d, &gsn: %d",
         skill_table[sn].name,
         (int)skill_table[sn].pgsn,
         (int)gsn
      );
      wiznet(log_buf, NULL, NULL, WIZ_BUGS, 0, 0);
      log_string(log_buf);
      do_gsndump(NULL, "fix");
   }
   return sn;
}

bool contains_pulsing(OBJ_DATA* obj)
{
   OBJ_DATA* obj_inside;
   OBJ_DATA* obj_inside_next;
   static int depth = 0;

   depth++;  /* Prevent real/near infinite loops */
   /*
   Would be insane to have 10
   containers inside each other,
   assume this is enough depth
   */
   if (depth > 10)
   {
      depth--;
      return FALSE;
   }
   if (!obj->contains)
   {
      depth--;
      return FALSE;
   }
   for (obj_inside = obj->contains; obj_inside; obj_inside = obj_inside_next)
   {
      obj_inside_next = obj_inside->next_content;
      if (is_pulsing(obj_inside))
      {
         depth--;
         return TRUE;
      }
      if
      (
         obj_inside->contains &&
         contains_pulsing(obj_inside)
      )
      {
         depth--;
         return TRUE;
      }
   }
   depth--;
   return FALSE;
}

bool check_room(CHAR_DATA* ch, bool purge, char* reason)
{
   ROOM_INDEX_DATA* room;

   if
   (
      ch == NULL ||
      !ch->valid ||
      !ch->on_line ||
      (
         ch->in_room == NULL &&
         !IS_NPC(ch) &&
         ch->desc &&
         ch->desc->connected != CON_PLAYING
      )
   )
   {
      /* Players not yet logged in, already cleared, or null */
      return FALSE;
   }

   if (ch->in_room == NULL)
   {
      send_to_char
      (
         "You fall through a flaw in the realms and end up here!\n\r",
         ch
      );
      if (IS_NPC(ch))
      {
         if (!purge)
         {
            return FALSE;
         }
         sprintf
         (
            log_buf,
            "check_room NULL ROOM [%d] %s Purged",
            ch->pIndexData->vnum,
            ch->short_descr
         );
         wiznet(log_buf, ch, NULL, WIZ_BUGS, 0, LEVEL_IMMORTAL);
         extract_char(ch, TRUE);
         log_string(log_buf);
         if (reason)
         {
            bug(reason, 0);
         }
         return FALSE;
      }
      log_string(log_buf);
      char_to_room(ch, (room = get_room_index(ROOM_VNUM_LIMBO)));
      sprintf
      (
         log_buf,
         "check_room NULL ROOM %s sent to Limbo due to a bug.\n\r",
         ch->name
      );
      wiznet(log_buf, ch, NULL, WIZ_BUGS, 0, get_trust(ch));
      log_string(log_buf);
      if (room == NULL)
      {
         sprintf
         (
            log_buf,
            "check_room NULL ROOM %s LIMBO DOES NOT EXIST.\n\r",
            ch->name
         );
         wiznet(log_buf, ch, NULL, WIZ_BUGS, 0, get_trust(ch));
         log_string(log_buf);
         extract_char(ch, TRUE);
         if (reason)
         {
            bug(reason, 0);
         }
         return FALSE;
      }
   }
   return TRUE;
}

void impure_note(CHAR_DATA* ch, char* message)
{
   char subject[MAX_INPUT_LENGTH];
   char body[MAX_STRING_LENGTH];

   if
   (
      ch->house != HOUSE_CRUSADER ||
      IS_NPC(ch) ||
      !IS_SET(ch->act2, PLR_IMPURE)
   )
   {
      return;
   }
   sprintf(subject, "Impure Warrior: %s", ch->name);
   sprintf
   (
      body,
      "%s\n\r",
      message
   );
   make_note
   (
      (
         number_bits(1) ?
         "Druid of Darkwood" :
         "the Wise One"
      ),
      subject,
      house_table[HOUSE_CRUSADER].induct_note_to,
      body,
      NOTE_NEWS
   );
}

void check_impure(CHAR_DATA* ch, char* name, sh_int type)
{
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA* gch;
   bool first;

   if
   (
      IS_NPC(ch) ||
      IS_SET(ch->act2, PLR_IMPURE) ||
      IS_AFFECTED(ch, AFF_CHARM) ||
      IS_IMMORTAL(ch)
   )
   {
      return;
   }
   switch (type)
   {
      default:
      {
         bug("check_impure bad type %d", type);
         return;
      }
      case (IMPURE_PRACTICE):
      {
         sprintf
         (
            buf,
            "%s became impure by practicing %s",
            ch->name,
            name
         );
         break;
      }
      case (IMPURE_PRACTICE_INTENT):
      {
         sprintf
         (
            buf,
            "%s became impure by intending to practice %s",
            ch->name,
            name
         );
         break;
      }
      case (IMPURE_EAT):
      {
         sprintf
         (
            buf,
            "%s became impure by eating %s",
            ch->name,
            name
         );
         break;
      }
      case (IMPURE_DRINK):
      {
         sprintf
         (
            buf,
            "%s became impure by drinking from %s",
            ch->name,
            name
         );
         break;
      }
      case (IMPURE_CAST):
      {
         sprintf
         (
            buf,
            "%s became impure by casting %s",
            ch->name,
            name
         );
         break;
      }
      case (IMPURE_RECITE):
      {
         sprintf
         (
            buf,
            "%s became impure by reciting %s",
            ch->name,
            name
         );
         break;
      }
      case (IMPURE_EAT_PILL):
      {
         sprintf
         (
            buf,
            "%s became impure by eating %s",
            ch->name,
            name
         );
         break;
      }
      case (IMPURE_QUAFF):
      {
         sprintf
         (
            buf,
            "%s became impure by quaffing %s",
            ch->name,
            name
         );
         break;
      }
      case (IMPURE_BRANDISH):
      {
         sprintf
         (
            buf,
            "%s became impure by brandishing %s",
            ch->name,
            name
         );
         break;
      }
      case (IMPURE_WEAR):
      {
         sprintf
         (
            buf,
            "%s became impure by wearing %s",
            ch->name,
            name
         );
         break;
      }
      case (IMPURE_ZAP):
      {
         sprintf
         (
            buf,
            "%s became impure by zapping with %s",
            ch->name,
            name
         );
         break;
      }
      case (IMPURE_GROUP_MAGE):
      {
         first = TRUE;
         sprintf(buf, "%s became impure by grouping with (", ch->name);
         for (gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room)
         {
            if
            (
               gch != ch &&
               !IS_NPC(ch) &&
               is_same_group(gch, ch) &&
               !IS_IMMORTAL(gch) &&
               gch->house != HOUSE_CRUSADER &&
               IS_SET(gch->act2, PLR_IMPURE) &&
               (
                  gch->class == CLASS_ANTI_PALADIN ||
                  gch->class == CLASS_CHANNELER ||
                  gch->class == CLASS_NIGHTWALKER ||
                  gch->class == CLASS_NECROMANCER ||
                  gch->class == CLASS_ELEMENTALIST
               )
            )
            {
               if (!first)
               {
                  strcat(buf, ", ");
               }
               strcat(buf, gch->name);
               first = FALSE;
            }
         }
         strcat(buf, ")");
         if (first)
         {
            /* Don't make them impure for grouping with themselves. */
            return;
         }
         log_string(buf);
         /* Get bit ready for note */
         SET_BIT(ch->act2, PLR_IMPURE);
         impure_note(ch, buf);
         break;
      }
   }
   if (type != IMPURE_GROUP_MAGE)
   {
      log_string(buf);
      /* get bit ready for note */
      SET_BIT(ch->act2, PLR_IMPURE);
      impure_note(ch, buf);
   }
}

char* get_dossier_time(CHAR_DATA* ch, char* buf)
{
   struct tm* time_temp;
   CHAR_DATA* real_ch;

   if
   (
      ch->desc &&
      ch->desc->original
   )
   {
      real_ch = ch->desc->original;
   }
   else
   {
      real_ch = ch;
   }

   time_temp = localtime(&current_time);
   buf[0] = '\0';
   sprintf
   (
      buf,
      "[%-10s %s%d/%s%d/%s%d] ",
      real_ch->name,
      (time_temp->tm_mon < 9) ? "0" : "",
      time_temp->tm_mon + 1,
      (time_temp->tm_mday < 10) ? "0" : "",
      time_temp->tm_mday,
      (time_temp->tm_year < 110) ? "0" : "",
      time_temp->tm_year - 100
   );
   return buf;
}

bool check_shadowstrike(CHAR_DATA* ch, bool shadow, bool skull)
{
   if
   (
      skull &&
      is_affected(ch, gsn_skull_bash) &&
      number_percent() > 50 + get_curr_stat(ch, STAT_DEX) &&
      number_range(1, 2) == 1
   )
   {
      act
      (
         "$n looks very disoriented.",
         ch,
         NULL,
         NULL,
         TO_ROOM
      );
      send_to_char("You are too stunned to do that!\n\r", ch);
      DAZE_STATE(ch, 2 * PULSE_VIOLENCE);
      return TRUE;
   }
   if
   (
      shadow &&
      is_affected(ch, gsn_shadowstrike) &&
      number_percent() > 50 + get_curr_stat(ch, STAT_DEX)
   )
   {
      act
      (
         "A pair of shadowy arms coil around $n and block $s movements!",
         ch,
         NULL,
         NULL,
         TO_ROOM
      );
      send_to_char
      (
         "Shadowy arms coil around you, blocking your movements!\n\r",
         ch
      );
      DAZE_STATE(ch, 2 * PULSE_VIOLENCE);
      return TRUE;
   }
   return FALSE;
}

void fix_fighting(CHAR_DATA* ch)
{
   CHAR_DATA* victim = ch->fighting;
   CHAR_DATA* vch;
   CHAR_DATA* vch_next;

   /* Null room, and cannot fix */
   if (!check_room(ch, FALSE, NULL))
   {
      return;
   }
   /* if not fighting, but I am fighting/standing */
   if
   (
      victim == NULL &&
      ch->position >= POS_FIGHTING
   )
   {
      /* If someone is fighting me in the room, I'll start fighting them */
      for (vch = ch->in_room->people; vch; vch = vch_next)
      {
         vch_next = vch->next_in_room;
         if
         (
            vch->fighting == ch &&
            !is_safe(ch, vch, IS_SAFE_SILENT)
         )
         {
            ch->fighting = vch;
            ch->position = POS_FIGHTING;
            return;
         }
      }
   }
   /* Otherwise if no victim, stop */
   if (victim == NULL)
   {
      return;
   }
   /* Person I am fighting is not in my room */
   if (victim->in_room != ch->in_room)
   {
      /*
      Go through everyone that is fighting me and not in my
      room.  Fix their fighting status.
      */
      for (vch = char_list; vch; vch = vch_next)
      {
         vch_next = vch->next;
         if
         (
            vch->fighting == ch &&
            vch->in_room != ch->in_room
         )
         {
            vch->fighting = NULL;
            fix_fighting(vch);
         }
      }
      /* Fix my fighting status. */
      ch->fighting = NULL;
      if (ch->position >= POS_FIGHTING)
      {
         update_pos(ch);
      }
      /* Look for someone IN my room that is fighting me, to fight. */
      fix_fighting(ch);
   }
   else if
   (
      victim->position >= POS_FIGHTING &&
      victim->fighting == NULL &&
      !is_safe(victim, ch, IS_SAFE_SILENT)
   )
   {
      /* same room, someone is being attacked but is not fighting */
      victim->fighting = ch;
      victim->position = POS_FIGHTING;
   }
}

void remove_name(char* string, char* name)
{
   char* argument;
   char arg[MAX_STRING_LENGTH];
   char buf[MAX_STRING_LENGTH];
   bool first = TRUE;

   strcpy(buf, string);
   argument = buf;
   string[0] = '\0';
   argument = one_argument(argument, arg);
   while (arg[0] != '\0')
   {

      if (str_cmp(arg, name))
      {
         if (!first)
         {
            strcat(string, " ");
         }
         first = FALSE;
         strcat(string, arg);
      }
      argument = one_argument(argument, arg);
   }
}

void water_update(CHAR_DATA* ch, bool increase)
{
   int dam;

   if (ch == NULL)
   {
      return;
   }
   if
   (
      ch->ghost ||
      IS_AFFECTED(ch, AFF_SWIM) ||
      IS_IMMORTAL(ch) ||
      ch->in_room == NULL ||
      ch->in_room->sector_type != SECT_UNDERWATER ||
      check_immune(ch, DAM_DROWNING) == IS_IMMUNE
   )
   {
      ch->air_loss = 0;
      return;
   }
   if (increase)
   {
      (ch->air_loss)++;
   }
   if (ch->air_loss < 2)
   {
      return;
   }
   dam = ch->air_loss - 1;
   dam *= dam * 5;
   dam += 10;
   act("$n gasps for air!", ch, NULL, NULL, TO_ROOM);
   send_to_char("You gasp for air!\n\r", ch);
   damage(ch, ch, dam, gsn_waterbreathing, DAM_DROWNING, TRUE);
}

/*
   Lookup spell, but replace sanc and word of recall with
   non-holy equivalents
*/
int spell_match(const char* name)
{
   int skill;

   skill = skill_lookup(name);
   if (skill == gsn_sanctuary)
   {
      return gsn_chromatic_shield;
   }
   if (skill == gsn_word_recall)
   {
      return gsn_power_word_recall;
   }
   return skill;
}

int spell_match_num(int skill)
{
   if (skill == gsn_sanctuary)
   {
      return gsn_chromatic_shield;
   }
   if (skill == gsn_word_recall)
   {
      return gsn_power_word_recall;
   }
   return skill;
}

void light_check(ROOM_INDEX_DATA* room, CHAR_DATA* ch, OBJ_DATA* obj, const char* func)
{
   if (room->light >= 0)
   {
      return;
   }
   sprintf
   (
      log_buf,
      "Negative light: Room [%d], Char: [%s], Obj: [%d] in %s",
      room->vnum,
      (
         ch ?
         ch->name :
         "NULL"
      ),
      (
         obj ?
         obj->pIndexData->vnum :
         0
      ),
      func
   );
   bug_trust(log_buf, 0, get_trust(ch));
   /* Prevent ruined rooms */
   room->light = 0;
}

bool wizi_to(CHAR_DATA* ch, CHAR_DATA* looker)
{
   if
   (
      ch != NULL &&
      !IS_NPC(ch) &&
      IS_IMMORTAL(ch) &&
      (
         looker == NULL ||
         !can_see(looker, ch)
      ) &&
      (
         (
            looker != NULL &&
            (
               (
                  ch->invis_level > get_trust(looker) ||
                  (
                     ch->incog_level > get_trust(looker) &&
                     ch->in_room != looker->in_room
                  ) ||
                  ch->nosee == looker
               ) ||
               (
                  looker != NULL &&
                  !IS_NPC(looker) &&
                  looker->pcdata->nosee_perm != NULL &&
                  looker->pcdata->nosee_perm[0] != '\0' &&
                  is_name(ch->name, looker->pcdata->nosee_perm)
               )
            )
         ) ||
         (
            looker == NULL &&
            (ch)->invis_level >= LEVEL_IMMORTAL
         )
      )
   )
   {
      return TRUE;
   }
   return FALSE;
}

void check_perception(CHAR_DATA* ch, CHAR_DATA* victim, char* reason)
{
   CHAR_DATA* vch;
   CHAR_DATA* master_ch;
   CHAR_DATA* master_vch;
   int chance;

   if
   (
      reason == NULL ||
      ch == NULL ||
      victim == NULL
   )
   {
      return;
   }
   if (IS_AFFECTED(ch, AFF_CHARM))
   {
      switch (guard_type(ch))
      {
         default:
         {
            master_ch = ch->master;
            break;
         }
         case (GUARD_SENTRY):
         case (GUARD_SENTINEL):
         {
            master_ch = ch->mprog_target;
            break;
         }
      }
   }
   else
   {
      master_ch = NULL;
   }
   if (IS_AFFECTED(victim, AFF_CHARM))
   {
      switch (guard_type(victim))
      {
         default:
         {
            master_vch = victim->master;
            break;
         }
         case (GUARD_SENTRY):
         case (GUARD_SENTINEL):
         {
            /* Sentries and sentinels do not count as charmed for laws */
            return;
            /*
            master_vch = victim->mprog_target;
            */
            break;
         }
      }
   }
   else
   {
      master_vch = NULL;
   }
   /* Sanity check to avoid infinite loops */
   if (master_ch)
   {
      while
      (
         IS_NPC(master_ch) &&
         master_ch->master &&
         master_ch->master != master_ch &&
         master_ch->master->master != master_ch &&
         IS_AFFECTED(master_ch, AFF_CHARM)
      )
      {
         master_ch = master_ch->master;
      }
   }
   else
   {
      master_ch = ch;
   }
   if (master_vch)
   {
      while
      (
         IS_NPC(master_vch) &&
         master_vch->master &&
         master_vch->master != master_vch &&
         master_vch->master->master != master_vch &&
         IS_AFFECTED(master_vch, AFF_CHARM)
      )
      {
         master_vch = master_vch->master;
      }
   }
   else
   {
      master_vch = victim;
   }
   if
   (
      IS_NPC(master_ch) ||
      IS_NPC(master_vch) ||
      IS_IMMORTAL(master_ch) ||
      IS_IMMORTAL(master_vch) ||
      master_ch == master_vch ||
      IS_SET(master_vch->act2, PLR_LAWLESS) ||
      house_down_1(NULL, HOUSE_ENFORCER, TRUE)
   )
   {
      return;
   }
   for (vch = char_list; vch; vch = vch->next)
   {
      if
      (
         !vch->on_line ||
         IS_NPC(vch) ||
         vch->in_room == NULL ||
         vch->in_room->area != ch->in_room->area ||
         !IS_AWAKE(vch) ||
         vch->house != HOUSE_ENFORCER ||
         !has_skill(vch, gsn_perception) ||
         (
            chance = get_skill(vch, gsn_perception)
         ) < 1
      )
      {
         continue;
      }
      chance += 5 * (vch->level - ch->level);
      if (!can_see(vch, ch))
      {
         chance -= 5;
      }
      if (IS_AFFECTED(vch, AFF_BLIND))
      {
         chance -= chance / 3;
      }
      if (vch->in_room == ch->in_room)
      {
         chance += 10;
      }
      else
      {
         chance -= chance / 4;
      }
      if (number_percent() > chance)
      {
         check_improve(vch, gsn_perception, FALSE, 1);
         send_to_char("You feel as if something has happened.\n\r", vch);
         continue;
      }
      check_improve(vch, gsn_perception, TRUE, 1);
      if (ch->in_room == vch->in_room)
      {
         act
         (
            "You notice $N $t!",
            vch,
            reason,
            ch,
            TO_CHAR
         );
      }
      else
      {
         act
         (
            "You notice $N $t nearby!",
            vch,
            reason,
            ch,
            TO_CHAR
         );
      }
   }
}

int guard_type(CHAR_DATA* ch)
{
   if
   (
      !IS_NPC(ch) ||
      !IS_AFFECTED(ch, AFF_CHARM) ||
      (
         ch->mprog_target == NULL &&
         ch->master == NULL
      ) ||
      ch->pIndexData->vnum != MOB_VNUM_ENFORCER_GUARD
   )
   {
      return -1;
   }
   if (IS_SET(ch->off_flags, OFF_TRIP))  /* Normal */
   {
      return GUARD_NORMAL;
   }
   else if (IS_SET(ch->off_flags, OFF_CRUSH))
   {
      if (IS_SET(ch->off_flags, NO_TRACK))  /* Sentinel */
      {
         return GUARD_SENTINEL;
      }
      else  /* Defender */
      {
         return GUARD_DEFENDER;
      }
   }
   else if (IS_SET(ch->act, ACT_MAGE))  /* Magus */
   {
      return GUARD_MAGUS;
   }
   else if (IS_SET(ch->act, ACT_CLERIC))  /* Curate */
   {
      return GUARD_CURATE;
   }
   else  /* Sentry */
   {
      return GUARD_SENTRY;
   }
}

bool frog_dodge(CHAR_DATA* victim, CHAR_DATA* ch, int dt)
{
   int chance;
   char buf[MAX_INPUT_LENGTH];
   char attack[MAX_INPUT_LENGTH];
   OBJ_DATA* wield;
   OBJ_DATA* arrow;
   CHAR_DATA* book;
   NODE_DATA* node;
   bool house = FALSE;
   int num_books = 0;
   bool room = FALSE;
   CHAR_DATA* fizz = NULL;

   if
   (
      !victim ||
      !ch ||
      ch->race == grn_book ||
      victim->race == grn_illithid
   )
   {
      return FALSE;
   }
   if
   (
      victim->race != grn_book &&
      (
         !frog_house ||
         frog_house != victim->house
      )
   )
   {
      /* Check if in a book's group */
      for (node = book_race_list->first; node; node = node->next)
      {
         book = (CHAR_DATA*)node->data;
         if (book->id == ID_BUK)
         {
            fizz = book;
            if (fizz->in_room == victim->in_room)
            {
               room = TRUE;
            }
         }
         if (is_same_group(book, victim))
         {
            num_books++;
         }
      }
      if
      (
         !num_books &&
         !room
      )
      {
         return FALSE;
      }
   }
   if
   (
      victim->id == ID_BUK ||
      (
         frog_house &&
         victim->house == frog_house
      )
   )
   {
      house = TRUE;
   }
   if (house)
   {
      chance = victim->level / 17;
      if (chance < 1)
      {
         chance = 1;
      }
   }
   else
   {
      chance = 0;
   }
   if (victim->race != grn_book)
   {
      if (!num_books)
      {
         /* Check if in a book's group if didn't check already */
         for (node = book_race_list->first; node; node = node->next)
         {
            book = (CHAR_DATA*)node->data;
            if (book->id == ID_BUK)
            {
               fizz = book;
               if (fizz->in_room == victim->in_room)
               {
                  room = TRUE;
               }
            }
            if (is_same_group(book, victim))
            {
               num_books++;
            }
         }
      }
      if (num_books)
      {
         /*
            Not a book though
            Increases by 2%, but if a hero, only increases 1%
            (max of 4)
         */
         chance += 2;
         if (chance > 4)
         {
            chance = 4;
         }
      }
      else if
      (
         room &&
         !house
      )
      {
         chance++;
      }
      if
      (
         num_books > 1 &&
         number_percent() <= num_books * 5
      )
      {
         chance++;
      }
   }
   else
   {
      for (node = book_race_list->first; node; node = node->next)
      {
         book = (CHAR_DATA*)node->data;
         if (is_same_group(book, victim))
         {
            /* In group with another book */
            num_books++;
         }
      }
      /* num_books now num of books in group (counting self) */
      chance += 2;
      if (chance > 4)
      {
         chance = 4;
      }
      if (number_percent() <= num_books * 15)
      {
         chance++;
      }
      if
      (
         num_books > 1 &&
         number_percent() <= num_books * 15
      )
      {
         chance++;
      }
   }
   if (number_percent() > chance)
   {
      return FALSE;
   }
   if (dt == gsn_dual_wield)
   {
      wield = get_eq_char(ch, WEAR_DUAL_WIELD);
   }
   else if (dt == gsn_tertiary_wield)
   {
      wield = get_eq_char(ch, WEAR_TERTIARY_WIELD);
   }
   else if (dt == gsn_archery)
   {
      if
      (
         !check_tanking(ch) &&
         (
            arrow = get_eq_char(ch, WEAR_WIELD)
         ) != NULL &&
         arrow->item_type == ITEM_WEAPON &&
         arrow->value[0] == WEAPON_BOW &&
         (
            arrow = get_eq_char(ch, WEAR_ABOUT)
         ) != NULL &&
         (
            arrow = arrow->contains
         ) != NULL &&
         arrow->item_type == ITEM_WEAPON &&
         arrow->value[0] == WEAPON_ARROWS
      )
      {
         wield = arrow;
      }
      else
      {
         wield = get_eq_char(ch, WEAR_WIELD);
      }
   }
   else
   {
      wield = NULL;
   }
   if
   (
      wield != NULL &&
      wield->item_type == ITEM_WEAPON
   )
   {
      dt = TYPE_HIT + wield->value[3];
   }
   else if (dt == TYPE_HIT)
   {
      dt = TYPE_HIT + ch->dam_type;
   }
   if
   (
      dt > 0 &&
      dt < MAX_SKILL
   )
   {
      strcpy(attack, skill_table[dt].noun_damage);
   }
   else if
   (
      dt > TYPE_HIT &&
      dt < TYPE_HIT + MAX_DAMAGE_MESSAGE
   )
   {
      strcpy(attack, attack_table[dt - TYPE_HIT].noun);
   }
   else
   {
      bug("In frog_dodge, bad dam message: dt %d.", dt);
      strcpy(attack, attack_table[0].noun);
   }
   sprintf(buf, "Frogs push you out of the way of $N's %s!", attack);
   act(buf, victim, NULL, ch, TO_CHAR);
   sprintf(buf, "Frogs push $n out of the way of $N's %s!", attack);
   act(buf, victim, NULL, ch, TO_NOTVICT);
   sprintf(buf, "Frogs push $n out of the way of your %s!", attack);
   act(buf, victim, NULL, ch, TO_VICT);
   return TRUE;
}

bool house_closed(int house, int version)
{
   switch (house)
   {
      default:
      {
         return TRUE;
      }
      case (HOUSE_ANCIENT):
      {
         if (version < 0)
         {
            return TRUE;
         }
         return FALSE;
      }
      case (HOUSE_LIGHT):
      {
         if (version < 0)
         {
            return TRUE;
         }
         return FALSE;
      }
      case (HOUSE_ARCANA):
      {
         if (version < 0)
         {
            return TRUE;
         }
         return FALSE;
      }
      case (HOUSE_CRUSADER):
      {
         if (version < 0)
         {
            return TRUE;
         }
         return FALSE;
      }
      case (HOUSE_ENFORCER):
      {
         if (version < 32)
         {
            return TRUE;
         }
         return FALSE;
      }
      case (HOUSE_OUTLAW):
      {
         if (version < 35)
         {
            return TRUE;
         }
         return FALSE;
      }
      case (HOUSE_MARAUDER):
      {
         if (version < 0)
         {
            return TRUE;
         }
         return FALSE;
      }
      case (HOUSE_VALOR):
      {
         if (version < 37)
         {
            return TRUE;
         }
         return FALSE;
      }
      case (HOUSE_EMPIRE):
      {
         if (version < 25)
         {
            return TRUE;
         }
         return FALSE;
      }
      case (HOUSE_BRETHREN):
      {
         if (version < 0)
         {
            return TRUE;
         }
         return FALSE;
      }
      case (HOUSE_SCHOLAR):
      {
         if (version < 0)
         {
            return TRUE;
         }
         return FALSE;
      }
      case (HOUSE_LIFE):
      {
         if (version < 0)
         {
            return TRUE;
         }
         return FALSE;
      }
      case (HOUSE_COVENANT):
      {
         if (version < 38)
         {
            return TRUE;
         }
         return FALSE;
      }
      case (HOUSE_CONCLAVE):
      {
         if (version < 0)
         {
            return TRUE;
         }
         return FALSE;
      }
   }
}

int sql_house(int house)
{
   switch (house)
   {
      default:
      {
         return -1;
      }
      case (HOUSE_ANCIENT):
      {
         return 4;
      }
      case (HOUSE_BRETHREN):
      {
         return 5;
      }
      case (HOUSE_CRUSADER):
      {
         return 6;
      }
      case (HOUSE_ENFORCER):
      {
         return 7;
      }
      case (HOUSE_LIGHT):
      {
         return 9;
      }
      case (HOUSE_CONCLAVE):
      {
         return 25;
      }
      case (HOUSE_MARAUDER):
      {
         return 8;
      }
   }
}

/* Return terrain type that simulacrum is hiding in, or -1 if none */
int check_simulacrum(CHAR_DATA* ch)
{
   AFFECT_DATA* paf;
   CHAR_DATA* gch;

   if ((ch == NULL) || (ch->in_room == NULL)) {
      return -1;
   }

   for (gch = ch->in_room->people; gch; gch = gch->next_in_room)
   {
      if
      (
         is_same_group(ch, gch) &&
         (
            paf = affect_find(gch->affected, gsn_simulacrum)
         ) != NULL
      )
      {
         return paf->modifier;
      }
   }
   return -1;
}

void simulacrum_on(CHAR_DATA* ch)
{
   send_to_char("Reality begins to distort around you.\n\r", ch);
}

void simulacrum_off(CHAR_DATA* ch)
{
   act
   (
      "$n emerges through a distortion in the fabric of reality.",
      ch,
      NULL,
      NULL,
      TO_ROOM
   );
   send_to_char
   (
      "You emerge through a distortion in the fabric of reality.\n\r",
      ch
   );
}

void strip_simulacrum(CHAR_DATA* ch, CHAR_DATA* victim, bool force)
{
   CHAR_DATA* gch;

   if
   (
      ch != NULL &&
      check_room(ch, FALSE, NULL) &&
      (
         force ||
         check_simulacrum(ch) != -1
      )
   )
   {
      for (gch = ch->in_room->people; gch; gch = gch->next_in_room)
      {
         if
         (
            !IS_NPC(gch) &&
            is_same_group(ch, gch)
         )
         {
            simulacrum_off(gch);
            affect_strip(gch, gsn_simulacrum);
         }
      }
   }
   if
   (
      victim != NULL &&
      ch != victim &&
      check_room(victim, FALSE, NULL) &&
      !is_same_group(ch, victim) &&
      (
         force ||
         check_simulacrum(victim) != -1
      )
   )
   {
      for (gch = victim->in_room->people; gch; gch = gch->next_in_room)
      {
         if
         (
            !IS_NPC(gch) &&
            is_same_group(victim, gch)
         )
         {
            simulacrum_off(gch);
            affect_strip(gch, gsn_simulacrum);
         }
      }
   }
}

/*
 * Appends src to string dst of size siz (unlike strncat, siz is the
 * full size of dst, not space left).  At most siz-1 characters
 * will be copied.  Always NUL terminates (unless siz <= strlen(dst)).
 * Returns strlen(src) + MIN(siz, strlen(initial dst)).
 * If retval >= siz, truncation occurred.
 */
size_t strlcat(char* dst, const char* src, size_t siz)
{
   register char* d = dst;
   register const char* s = src;
   register size_t n = siz;
   size_t dlen;

   /* Find the end of dst and adjust bytes left but don't go past end */
   while
   (
      n-- != 0 &&
      *d != '\0'
   )
   {
      d++;
   }
   dlen = d - dst;
   n = siz - dlen;

   if (n == 0)
   {
      return (dlen + strlen(s));
   }
   while (*s != '\0')
   {
      if (n != 1)
      {
         *d++ = *s;
         n--;
      }
      s++;
   }
   *d = '\0';

   return (dlen + (s - src));  /* count does not include NUL */
}

void irv_update(CHAR_DATA* ch, bool tick, bool recalculate)
{
   OBJ_DATA* brand;
   OBJ_DATA* obj;
   AFFECT_DATA* paf;
   int aff_save = 0;
   int aff2_save = 0;

   if (IS_NPC(ch))
   {
      return;
   }
   if (recalculate)
   {
      /* These do not have affects to save them */
      aff_save = ch->affected_by & (AFF_HIDE | AFF_CAMOUFLAGE);
      aff2_save = ch->affected_by2 & (AFF_AHZRA_BLOODSTAIN | AFF_HANG);
      ch->imm_flags     = 0;
      ch->res_flags     = 0;
      ch->vuln_flags    = 0;
      ch->affected_by   = 0;
      ch->affected_by2  = 0;
      /* recalculate irv/affects from objects */
      for (obj = ch->carrying; obj; obj = obj->next_content)
      {
         if (obj->wear_loc == -1)
         {
            continue;
         }
         for (paf = obj->affected; paf; paf = paf->next)
         {
            if
            (
               !paf->bitvector &&
               !paf->bitvector2
            )
            {
               continue;
            }
            switch (paf->where)
            {
               default:
               {
                  continue;
               }
               case (TO_AFFECTS):
               {
                  ch->affected_by |= paf->bitvector;
                  ch->affected_by2 |= paf->bitvector2;
                  continue;
               }
               case (TO_IMMUNE):
               {
                  ch->imm_flags |= paf->bitvector;
                  continue;
               }
               case (TO_RESIST):
               {
                  ch->res_flags |= paf->bitvector;
                  continue;
               }
               case (TO_VULN):
               {
                  ch->vuln_flags |= paf->bitvector;
                  continue;
               }
            }
         }
         if (!obj->enchanted)
         {
            for (paf = obj->pIndexData->affected; paf; paf = paf->next)
            {
               if
               (
                  !paf->bitvector &&
                  !paf->bitvector2
               )
               {
                  continue;
               }
               switch (paf->where)
               {
                  default:
                  {
                     continue;
                  }
                  case (TO_AFFECTS):
                  {
                     ch->affected_by |= paf->bitvector;
                     ch->affected_by2 |= paf->bitvector2;
                     continue;
                  }
                  case (TO_IMMUNE):
                  {
                     ch->imm_flags |= paf->bitvector;
                     continue;
                  }
                  case (TO_RESIST):
                  {
                     ch->res_flags |= paf->bitvector;
                     continue;
                  }
                  case (TO_VULN):
                  {
                     ch->vuln_flags |= paf->bitvector;
                     continue;
                  }
               }
            }
         }
      }
      /* recalculate irv/affects from affects */
      for (paf = ch->affected; paf; paf = paf->next)
      {
         if
         (
            !paf->bitvector &&
            !paf->bitvector2
         )
         {
            continue;
         }
         switch (paf->where)
         {
            default:
            {
               continue;
            }
            case (TO_AFFECTS):
            {
               ch->affected_by |= paf->bitvector;
               ch->affected_by2 |= paf->bitvector2;
               continue;
            }
            case (TO_IMMUNE):
            {
               ch->imm_flags |= paf->bitvector;
               continue;
            }
            case (TO_RESIST):
            {
               ch->res_flags |= paf->bitvector;
               continue;
            }
            case (TO_VULN):
            {
               ch->vuln_flags |= paf->bitvector;
               continue;
            }
         }
      }
      if
      (
         has_skill(ch, gsn_wraithform) &&
         get_skill(ch, gsn_wraithform) >= 75
      )
      {
         SET_BIT(ch->affected_by, AFF_PASS_DOOR);
         SET_BIT(ch->affected_by, AFF_FLYING);
      }
      if
      (
         has_skill(ch, gsn_water_form) &&
         get_skill(ch, gsn_water_form) >= 75
      )
      {
         SET_BIT(ch->affected_by, AFF_SWIM);
      }
   }
   if (tick)
   {
      if (has_skill(ch, gsn_wraithform))
      {
         check_improve(ch, gsn_wraithform, TRUE, 1);
         if (get_skill(ch, gsn_wraithform) >= 75)
         {
            SET_BIT(ch->affected_by, AFF_PASS_DOOR);
            SET_BIT(ch->affected_by, AFF_FLYING);
         }
      }
      if (has_skill(ch, gsn_water_form))
      {
         check_improve(ch, gsn_water_form, TRUE, 1);
         if (get_skill(ch, gsn_water_form) >= 75)
         {
            SET_BIT(ch->affected_by, AFF_SWIM);
         }
      }
      if (has_skill(ch, gsn_earth_form))
      {
         check_improve(ch, gsn_earth_form, TRUE, 1);
      }
      if (has_skill(ch, gsn_flame_form))
      {
         check_improve(ch, gsn_flame_form, TRUE, 1);
      }
   }
   ch->affected_by   |= race_table[ch->race].aff;
   ch->affected_by2  |= race_table[ch->race].aff2;
   ch->affected_by   |= aff_save;
   ch->affected_by2  |= aff2_save;
   ch->imm_flags     |= race_table[ch->race].imm;
   ch->res_flags     |= race_table[ch->race].res;
   ch->vuln_flags    |= race_table[ch->race].vuln;
   brand = get_eq_char(ch, WEAR_BRAND);
   if (ch->pcdata->special == SUBCLASS_CORRUPTOR)
   {
      SET_BIT(ch->imm_flags, IMM_DISEASE);
      SET_BIT(ch->imm_flags, IMM_POISON);
   }
   if (ch->pcdata->special == SUBCLASS_NECROPHILE)
   {
      SET_BIT(ch->imm_flags, IMM_NEGATIVE);
   }
   if (ch->pcdata->special == SUBCLASS_KNIGHT_OF_PESTILENCE)
   {
      SET_BIT(ch->imm_flags, IMM_DISEASE);
      SET_BIT(ch->imm_flags, IMM_POISON);
   }
   if (IS_SET(ch->act2, PLR_LICH))
   {
      SET_BIT(ch->imm_flags, IMM_DISEASE);
      SET_BIT(ch->imm_flags, IMM_POISON);
      SET_BIT(ch->imm_flags, IMM_NEGATIVE);
      SET_BIT(ch->imm_flags, IMM_COLD);
      SET_BIT(ch->res_flags, RES_CHARM);
      SET_BIT(ch->res_flags, RES_MENTAL);
      SET_BIT(ch->res_flags, RES_WEAPON);
      SET_BIT(ch->vuln_flags, VULN_HOLY);
      SET_BIT(ch->affected_by, AFF_INFRARED);
      SET_BIT(ch->affected_by, AFF_DETECT_INVIS);
   }
   if
   (
      brand != NULL &&
      brand->pIndexData->vnum == 28903
   )
   {
      SET_BIT(ch->affected_by, AFF_DETECT_GOOD);
   }
   if
   (
      ch->pcdata->brand_rank > 0 &&
      brand != NULL &&
      brand->pIndexData->vnum == OBJ_VNUM_OBLIVION_BRAND
   )
   {
      SET_BIT(ch->affected_by, AFF_SWIM);
   }
   if
   (
      !IS_NPC(ch) &&
      ch->pcdata->death_status == HAS_DIED
   )
   {
      /* ghosts fly and can pass through doors */
      SET_BIT(ch->affected_by, AFF_FLYING);
      SET_BIT(ch->affected_by, AFF_PASS_DOOR);
   }
   /* Perm bonuses/penalties of irv/affects */
   ch->affected_by2  |= ch->pcdata->gained_affects2;
   ch->affected_by2  &= ~ch->pcdata->lost_affects2;
   ch->affected_by   |= ch->pcdata->gained_affects;
   ch->affected_by   &= ~ch->pcdata->lost_affects;
   ch->imm_flags     |= ch->pcdata->gained_imm_flags;
   ch->imm_flags     &= ~ch->pcdata->lost_imm_flags;
   ch->res_flags     |= ch->pcdata->gained_res_flags;
   ch->res_flags     &= ~ch->pcdata->lost_res_flags;
   ch->vuln_flags    |= ch->pcdata->gained_vuln_flags;
   ch->vuln_flags    &= ~ch->pcdata->lost_vuln_flags;
   /* Flying mount */
   if (ch->is_riding)
   {
      switch (ch->is_riding->mount_type)
      {
         case (MOUNT_OBJECT):
         {
            if
            (
               IS_SET
               (
                  (
                     (OBJ_DATA*)ch->is_riding->mount
                  )->mount_specs->move_flags,
                  MOUNT_AIR
               )
            )
            {
               SET_BIT(ch->affected_by, AFF_FLYING);
            }
            break;
         }
         case (MOUNT_MOBILE):
         {
            if
            (
               IS_FLYING
               (
                  (CHAR_DATA*)ch->is_riding->mount)
               )
            {
               SET_BIT(ch->affected_by, AFF_FLYING);
            }
         }
      }
   }
   /* Damage type */
   if (IS_SET(ch->act2, PLR_LICH))
   {
      ch->dam_type = 51;  /* Lich freezing punch */
   }
   else
   {
      ch->dam_type = pc_race_table[ch->race].dam_type;
   }

}

bool can_exit(CHAR_DATA* ch, EXIT_DATA* pexit)
{
   if
   (
      pexit == NULL ||
      pexit->u1.to_room == NULL
   )
   {
      return FALSE;
   }
   if
   (
      IS_SET(pexit->exit_info, EX_CLOSED) &&
      (
         !IS_AFFECTED(ch, AFF_PASS_DOOR) ||
         IS_SET(pexit->exit_info, EX_NOPASS) ||
         (
            ch->is_riding &&
            ch->is_riding->mount_type == MOUNT_MOBILE &&
            !IS_AFFECTED((CHAR_DATA*)ch->is_riding->mount, AFF_PASS_DOOR)
         )
      ) &&
      !IS_TRUSTED(ch, ANGEL) &&
      !(
         ch->house == HOUSE_EMPIRE &&
         pexit->u1.to_room != NULL &&
         pexit->u1.to_room->vnum == 1575
      )
   )
   {
      return FALSE;
   }
   return TRUE;
}

void gods_protect_emote(CHAR_DATA* ch, CHAR_DATA* victim)
{
   if
   (
      ch->in_room != victim->in_room ||
      ch == victim
   )
   {
      return;
   }
   if
   (
      IS_IMMORTAL(victim) &&
      get_trust(ch) < get_trust(victim)
   )
   {
      act
      (
         "You are unharmed by $n's feeble attack.",
         ch,
         NULL,
         victim,
         TO_VICT
      );
      act
      (
         "$N is unharmed by your feeble attack.",
         ch,
         NULL,
         victim,
         TO_CHAR
      );
      act
      (
         "$w is unharmed by $N's feeble attack.",
         victim,
         NULL,
         ch,
         TO_NOTVICT
      );
   }
   else
   {
      act
      (
         "The gods protect $N from $n.",
         ch,
         NULL,
         victim,
         TO_NOTVICT
      );
      act
      (
         "The gods protect you from $n.",
         ch,
         NULL,
         victim,
         TO_VICT
      );
      act
      (
         "The gods protect $N from you.",
         ch,
         NULL,
         victim,
         TO_CHAR
      );
   }
}

bool is_safe(CHAR_DATA* ch, CHAR_DATA* victim, int flags)
{
   OBJ_DATA* brand;

   if
   (
      !check_room(victim, FALSE, NULL) ||
      !check_room(ch, FALSE, NULL)
   )
   {
      /*
      Does not purge anymore
      bug("Purge in is_safe", 0);
      */
      return TRUE;
   }
   if
   (
      IS_AFFECTED2(victim, AFF_NAP) &&
      victim->position <= POS_SLEEPING &&
      !IS_SET(flags, IS_SAFE_RECURSIVE) &&
      (
         brand = get_eq_char(victim, WEAR_BRAND)
      ) != NULL &&
      brand->pIndexData->vnum == OBJ_VNUM_TWIG_BRAND
   )
   {
      if
      (
         ch != victim &&
         ch->in_room == victim->in_room
      )
      {
         act
         (
            "But $N is soo cuute!",
            ch,
            NULL,
            victim,
            TO_CHAR
         );
      }
      return TRUE;
   }
   if
   (
      victim == ch &&
      IS_SET(flags, IS_SAFE_AREA_ATTACK)
   )
   {
      return TRUE;
   }
   if
   (
      wizi_to(victim, ch) ||
      (
         IS_NPC(victim) &&
         IS_SET(victim->affected_by2, AFF_WIZI) &&
         !IS_IMMORTAL(ch)
      ) ||
      (
         IS_NPC(ch) &&
         IS_SET(ch->affected_by2, AFF_WIZI) &&
         !IS_IMMORTAL(victim)
      ) ||
      !can_area_hit_rogue(ch, victim)
   )
   {
      /* WIZI mob/wizi immortal */
      return TRUE;
   }

   if
   (
      /* perm ghosts */
      (
         !IS_NPC(ch) &&
         ch->pcdata->death_status == HAS_DIED
      ) ||
      (
         !IS_NPC(victim) &&
         victim->pcdata->death_status == HAS_DIED
      ) ||
      (
         /* link death */
         !IS_NPC(victim) &&
         !IS_NPC(ch) &&
         victim->fighting != ch &&
         ch->fighting != victim &&
         (
            (
               victim->desc == NULL &&
               victim->quittime < 17
            ) ||
            (
               ch->desc == NULL &&
               ch->quittime < 17
            )
         )
      ) ||
      (
         !IS_SET(flags, IS_SAFE_IGNORE_ROOM) &&
         ch->in_room != victim->in_room
      )
   )
   {
      if (!IS_SET(flags, IS_SAFE_SILENT))
      {
         gods_protect_emote(ch, victim);
      }
      return TRUE;
   }

   if (!IS_SET(flags, IS_SAFE_IGNORE_AFFECTS))
   {
      if (is_affected(victim, gsn_freeze))
      {
         if (!IS_SET(flags, IS_SAFE_SILENT))
         {
            act
            (
               "$n is trapped in ice and is not affected by attacks.",
               victim,
               NULL,
               NULL,
               TO_ROOM
            );
            act
            (
               "$N tries to harm you, but you are trapped in ice.",
               victim,
               NULL,
               ch,
               TO_CHAR
            );
         }
         if (victim->fighting != NULL)
         {
            victim->fighting = NULL;
         }
         return TRUE;
      }
      if (is_affected(victim, gsn_stone))
      {
         if (!IS_SET(flags, IS_SAFE_SILENT))
         {
            act
            (
               "$n is solid stone and is not affected by attacks.",
               victim,
               NULL,
               NULL,
               TO_ROOM
            );
            act
            (
               "$N tries to harm you, but you are solid stone.",
               victim,
               NULL,
               ch,
               TO_CHAR
            );
         }
         if (victim->fighting != NULL)
         {
            victim->fighting = NULL;
         }
         return TRUE;
      }
      if (is_affected(victim, gsn_timestop))
      {
         if (!IS_SET(flags, IS_SAFE_SILENT))
         {
            act
            (
               "$n is stuck in time and the gods protect $m.",
               victim,
               NULL,
               NULL,
               TO_ROOM
            );
            act
            (
               "$N tries to do something but you are stuck in time and unaffected.",
               victim,
               NULL,
               ch,
               TO_CHAR
            );
         }
         if (victim->fighting != NULL)
         {
            victim->fighting = NULL;
         }
         return TRUE;
      }
   }
   if
   (
      !(
         /* failed the exceptions */
         victim->fighting == ch ||
         victim == ch ||
         (
            IS_NPC(ch) &&
            ch->last_fought == victim->id
         )
      )
   )
   {
      bool safe = FALSE;
      bool ignore_pk = FALSE;
      CHAR_DATA* master_ch;
      CHAR_DATA* master_victim;
      CHAR_DATA* tmp_victim;

      if
      (
         /* ghosts */
         victim->ghost > 0 ||
         (
            ch->ghost > 0 &&
            (
               IS_NPC(ch) ||
               !IS_NPC(victim)
            )
         )
      )
      {
         safe = TRUE;
      }
      else if
      (
         !IS_IMMORTAL(ch) ||
         ch->level <= LEVEL_IMMORTAL
      )
      {
         switch (guard_type(ch))
         {
            default:
            {
               master_ch = ch->master;
               break;
            }
            case (GUARD_NORMAL):
            {
               master_ch = ch->master;
               tmp_victim = victim;
               while
               (
                  IS_NPC(tmp_victim) &&
                  tmp_victim->master &&
                  tmp_victim->master != tmp_victim &&
                  tmp_victim->master->master != tmp_victim &&
                  IS_AFFECTED(tmp_victim, AFF_CHARM)
               )
               {
                  tmp_victim = tmp_victim->master;
               }
               if
               (
                  !IS_NPC(tmp_victim) &&
                  (
                     IS_SET(tmp_victim->act, PLR_CRIMINAL) ||
                     IS_SET(tmp_victim->act, PLR_LAWLESS)
                  ) &&
                  tmp_victim->ghost <= 0
               )
               {
                  ignore_pk = TRUE;
               }
               break;
            }
            case (GUARD_SENTRY):
            case (GUARD_SENTINEL):
            {
               master_ch = ch->mprog_target;
               break;
            }
         }
         switch (guard_type(victim))
         {
            default:
            {
               master_victim = victim->master;
               break;
            }
            case (GUARD_NORMAL):
            {
               master_victim = victim->master;
               tmp_victim = ch;
               while
               (
                  IS_NPC(tmp_victim) &&
                  tmp_victim->master &&
                  tmp_victim->master != tmp_victim &&
                  tmp_victim->master->master != tmp_victim &&
                  IS_AFFECTED(tmp_victim, AFF_CHARM)
               )
               {
                  tmp_victim = tmp_victim->master;
               }
               if
               (
                  !IS_NPC(tmp_victim) &&
                  (
                     IS_SET(tmp_victim->act, PLR_CRIMINAL) ||
                     IS_SET(tmp_victim->act, PLR_LAWLESS)
                  ) &&
                  tmp_victim->ghost <= 0
               )
               {
                  ignore_pk = TRUE;
               }
               break;
            }
            case (GUARD_SENTRY):
            case (GUARD_SENTINEL):
            {
               master_victim = victim->mprog_target;
               break;
            }
         }
         /* Sanity check to avoid infinite loops */
         if (master_ch == ch)
         {
            master_ch = NULL;
         }
         if (master_victim == victim)
         {
            master_victim = NULL;
         }
         if (!ignore_pk)
         {
            int temp_flags;

            /*
               For checking against masters,
               must be silent, ignore affects (like timestop),
               and not be considered an area attack
            */
            temp_flags =
            (
               flags |
               IS_SAFE_SILENT |
               IS_SAFE_IGNORE_AFFECTS |
               IS_SAFE_IGNORE_ROOM |
               IS_SAFE_RECURSIVE
            );
            temp_flags &= ~IS_SAFE_AREA_ATTACK;

            if
            (
               IS_NPC(ch) &&
               IS_AFFECTED(ch, AFF_CHARM) &&
               master_ch
            )
            {
               /* killer is a charmed mob */
               if
               (
                  IS_NPC(victim) &&
                  IS_AFFECTED(victim, AFF_CHARM) &&
                  master_victim
               )
               {
                  /* victim is a charmed mob */
                  safe |= is_safe
                  (
                     master_ch,
                     master_victim,
                     temp_flags
                  );
               }
               else if (!IS_NPC(victim))
               {
                  /* victim is a player */
                  if (victim == master_ch)
                  {
                     return TRUE;
                  }
                  safe |=
                  (
                     victim == master_ch ||
                     is_safe
                     (
                        master_ch,
                        victim,
                        temp_flags
                     )
                  );
               }
            }
            else if (!IS_NPC(ch))
            {
               /* killer is a player */
               if
               (
                  IS_NPC(victim) &&
                  IS_AFFECTED(victim, AFF_CHARM) &&
                  master_victim
               )
               {
                  /* victim is a charmed mob */
                  safe |= is_safe
                  (
                     ch,
                     master_victim,
                     temp_flags
                  );
               }
               else if (!IS_NPC(victim))
               {
                  /* victim is a player */
                  safe |= !is_in_pk(ch, victim);
               }
            }
         }
      }
      if (safe)
      {
         if (!IS_SET(flags, IS_SAFE_SILENT))
         {
            gods_protect_emote(ch, victim);
         }
         return TRUE;
      }
   }
   if (!IS_SET(flags, IS_SAFE_RECURSIVE))
   {
      if
      (
         IS_IMMORTAL(ch) &&
         is_affected(ch, gsn_riot) &&
         !IS_NPC(victim) &&
         !IS_IMMORTAL(victim)
      )
      {
         if (!IS_SET(flags, IS_SAFE_SILENT))
         {
            send_to_char("You can't riot on mortals.\n\r", ch);
         }
         return TRUE;
      }
      if
      (
         !IS_TRUSTED(ch, ANGEL) &&
         (
            IS_SET(ch->in_room->room_flags, ROOM_SAFE) ||
            IS_SET(victim->in_room->room_flags, ROOM_SAFE)
         ) &&
         ch != victim
      )
      {
         if (!IS_SET(flags, IS_SAFE_SILENT))
         {
            send_to_char("The gods forbid it.\n\r", ch);
         }
         return TRUE;
      }
   }

   return FALSE;
}

bool is_safe_room(CHAR_DATA* ch, ROOM_AFFECT_DATA* raf, int flags)
{
   CHAR_DATA *caster;

   if
   (
      (
         caster = raf->caster
      ) == NULL ||
      ch->in_room != raf->room ||
      is_safe(caster, ch, IS_SAFE_SILENT | IS_SAFE_IGNORE_ROOM)
   )
   {
      return TRUE;
   }
   return FALSE;
}

/*
   See if someone is in pk..
*/
bool is_in_pk(CHAR_DATA* ch, CHAR_DATA* victim)
{
   OBJ_DATA *robe;
   int human_xp;
   int human_xp_vic;

   if
   (
      !ch ||
      !victim ||
      IS_NPC(ch) ||
      IS_NPC(victim)
   )
   {
      return FALSE;
   }
   if (ch == victim)
   {
      return TRUE;
   }
   if
   (
      ch->ghost > 0 ||
      victim->ghost > 0 ||
      ch->level < 10 ||
      victim->level < 10 ||
      /* Helper robe */
      (
         (
            robe = get_eq_char(ch, WEAR_ABOUT)
         ) != NULL &&
         robe->pIndexData->vnum == OBJ_VNUM_HELPER_ROBE &&
         ch->in_room != NULL &&
         ch->in_room->house == 0 &&
         ch->quittime == 0
      ) ||
      (
         (
            robe = get_eq_char(victim, WEAR_ABOUT)
         ) != NULL &&
         robe->pIndexData->vnum == OBJ_VNUM_HELPER_ROBE &&
         victim->in_room != NULL &&
         victim->in_room->house == 0 &&
         victim->quittime == 0
      )
   )
   {
      return FALSE;
   }
   if
   (
      /* Levels and pkexpand */
      (
         ch->level - victim->level <= 2 &&
         ch->level - victim->level >= -2
      ) ||
      (
         ch->pcdata->pk_expand >= victim->level &&
         ch->level < victim->level
      ) ||
      (
         victim->pcdata->pk_expand >= ch->level &&
         victim->level < ch->level
      )
   )
   {
      return TRUE;
   }

   /* start experimental pk range code */
   if
   (
      (
         victim->level > 40 &&
         victim->level < 51 &&
         (
            (
               (victim->played + current_time - victim->logon) /
               3600 -
               victim->pcdata->last_level
            ) > 50
         ) &&
         ch->level <= victim->level + 8 &&
         ch->level >= victim->level
      ) ||
      (
         ch->level > 40 &&
         ch->level < 51 &&
         (
            (
               (ch->played + current_time - ch->logon) /
               3600 -
               ch->pcdata->last_level
            ) > 50
         ) &&
         victim->level <= ch->level + 8 &&
         victim->level >= ch->level
      )
   )
   {
      return TRUE;
   }
   /* end experimental pk range code */

   human_xp = 0;
   human_xp_vic = 0;

   if (ch->race == grn_human)
   {
      human_xp = (ch->exp_total * 2);
      human_xp /= 10;
   }
   if (victim->race == grn_human)
   {
      human_xp_vic = (victim->exp_total * 2);
      human_xp_vic /= 10;
   }
   if
   (
      ch->level - victim->level < 7 &&
      ch->level - victim->level > -7 &&
      (
         (
            (
               (
                  ch->exp_total + human_xp
               ) * .75
            ) <
            (
               victim->exp_total +
               human_xp_vic
            ) &&
            (
               (
                  ch->exp_total + human_xp
               ) * 1.25
            ) >
            (
               human_xp_vic + victim->exp_total
            )
         ) ||
         (
            (
               (
                  human_xp_vic + victim->exp_total
               ) * .75
            ) <
            (
               human_xp + ch->exp_total
            ) &&
            (
               (
                  human_xp_vic + victim->exp_total
               ) * 1.25
            ) >
            (
               human_xp + ch->exp_total
            )
         )
      )
   )
   {
      return TRUE;
   }
   return FALSE;
}

CHAR_DATA * get_damager_room(CHAR_DATA *victim, ROOM_AFFECT_DATA * af)
{
   CHAR_DATA *caster;
   if (af->caster == NULL)
   return victim;
   if ((caster = af->caster) == NULL)
   return victim;
   if (victim->in_room != caster->in_room)
   return victim;
   if (IS_NPC(victim) && IS_AFFECTED(victim, AFF_CHARM)
   && victim->master == caster)
   return victim;
   if (is_same_group(caster, victim))
   return victim;
   if (IS_NPC(caster))
   return victim;
   if (IS_NPC(victim))
   return caster;
   if (is_safe(victim, caster, IS_SAFE_SILENT | IS_SAFE_IGNORE_ROOM))
   return victim;
   return caster;
}

CHAR_DATA* get_damager(CHAR_DATA *victim, AFFECT_DATA * af)
{
   CHAR_DATA *caster;
   char buf [MAX_INPUT_LENGTH];

   if (af->caster == NULL)
   {
      return victim;
   }
   if (strlen(af->caster) >= MAX_INPUT_LENGTH - 2)
   {
      bug("String too long in get_damager: %d", strlen(af->caster));
      return victim;
   }
   sprintf(buf, "+%s", af->caster);
   if
   (
      (
         caster = get_char_world(NULL, buf)
      ) == NULL
   )
   {
      return victim;
   }
   if (victim->in_room != caster->in_room)
   {
      return victim;
   }
   if
   (
      IS_NPC(victim) &&
      IS_AFFECTED(victim, AFF_CHARM) &&
      victim->master == caster
   )
   {
      return victim;
   }
   if (IS_NPC(caster))
   {
      return victim;
   }
   if (IS_NPC(victim))
   {
      return caster;
   }
   if (is_safe(victim, caster, IS_SAFE_SILENT | IS_SAFE_IGNORE_ROOM))
   {
      return victim;
   }
   return caster;
}

void store_death_affect(AFFECT_DATA* paf_old)
{
   AFFECT_DATA* paf;

   if
   (
      !paf_old ||
      !paf_old->valid ||
      paf_old->type < 1 ||
      paf_old->type >= MAX_SKILL ||
      !skill_table[paf_old->type].strip.death_keep
   )
   {
      return;
   }
   paf = new_affect();
   *paf = *paf_old;
   paf->caster = NULL;
   paf->next = death_affects[paf->type];
   death_affects[paf->type] = paf;
}

void restore_death_affects(CHAR_DATA* ch)
{
   AFFECT_DATA* paf;
   int cnt;

   for (cnt = 1; cnt < MAX_SKILL; cnt++)
   {
      while
      (
         (
            paf = death_affects[cnt]
         ) != NULL
      )
      {
         death_affects[cnt] = paf->next;
         paf->next = NULL;
         if (!IS_NPC(ch))
         {
            affect_to_char_version(ch, paf, AFFECT_CURRENT_VERSION);
         }
         free_affect(paf);
      }
   }
}

void stash_valuables(CHAR_DATA* ch)
{
   OBJ_DATA* obj;
   OBJ_DATA* obj_next;

   if
   (
      !IS_NPC(ch) ||
      ch->pIndexData->pShop == NULL
   )
   {
      return;
   }
   for (obj = ch->carrying; obj; obj = obj_next)
   {
      obj_next = obj->next_content;
      if
      (
         obj->cost < 10000 ||
         obj->wear_loc != WEAR_NONE
      )
      {
         /*
            Cheap item, under 100 gold,
            or is worn.
            Can leave in corpse
         */
         continue;
      }
      if (IS_SET(obj->extra_flags, ITEM_INVENTORY))
      {
         /* Just destroy inventory objects, they'll have it later */
         extract_obj(obj, FALSE);
         continue;
      }
      /* stash object */
      obj_from_char(obj);
      obj->next_content = ch->pIndexData->pShop->stashed;
      ch->pIndexData->pShop->stashed = obj;
   }
}

void unstash_valuables(CHAR_DATA* ch)
{
   OBJ_DATA* obj;
   OBJ_DATA* obj_next;

   if
   (
      !IS_NPC(ch) ||
      ch->pIndexData->pShop == NULL
   )
   {
      return;
   }
   for (obj = ch->pIndexData->pShop->stashed; obj; obj = obj_next)
   {
      obj_next = obj->next_content;
      ch->pIndexData->pShop->stashed = obj->next_content;
      obj->next_content = NULL;
      obj_to_keeper(obj, ch);
   }
}

void stop_feign(CHAR_DATA* ch, bool chance)
{
   int skill;

   if (!IS_AFFECTED2(ch, AFF_FEIGN_DEATH))
   {
      return;
   }
   skill = get_skill(ch, gsn_feign_death) * number_percent() / 100;
   if (skill > chance)
   {
      /* Don't stop feigning */
      return;
   }
   affect_strip(ch, gsn_feign_death);
   REMOVE_BIT(ch->affected_by2, AFF_FEIGN_DEATH);
   send_to_char("You are no longer feigning death.\n\r", ch);
   return;
}

void shuffle
(
   int size,
   const int* listToShuffle,
   int* shuffledList
)
{
   int i;
   int randomPos;

   /* Copy one list to the other list */
   for (i = 0; i < size; i++)
   {
      shuffledList[i] = listToShuffle[i];
   }

   /* Simple Shuffle! */
   for (i = 0; i < size; i++)
   {
      /*
         Grab a random position in the array  that is
         not my current index.  I can't use my swap trick
         below on the same number.  a XOR a = 0.
      */
      randomPos = number_range(0, size-2);
      if (randomPos == i)
      {
         randomPos = size-1;
      }

      /*
         Swap that random position with the current i index.
         I use a math trick I learned about how XOR will swap
         two numbers.  It's really simple and doesn't require
         a temp variable.
      */
      shuffledList[i] ^= shuffledList[randomPos];
      shuffledList[randomPos] ^= shuffledList[i];
      shuffledList[i] ^= shuffledList[randomPos];
   }
}

void update_alignment(CHAR_DATA *ch)
{
   if (ch == NULL || IS_NPC(ch))
   {
      return;
   }

   ch->alignment = URANGE(
                      MIN_ALIGN,
                      ch->true_alignment + ch->align_mod,
                      MAX_ALIGN);

   if (IS_AFFECTED2(ch, AFF_AHZRA_BLOODSTAIN))
   {
      ch->alignment = URANGE(
                         MIN_ALIGN,
                         ch->alignment - (5 * ALIGN_STEP),
                         MAX_ALIGN);
   }
}
