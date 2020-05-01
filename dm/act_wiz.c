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

static const char rcsid[] = "$Id: act_wiz.c,v 1.257 2004/11/25 08:52:20 fizzfaldt Exp $";

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "merc.h"
#include "recycle.h"
#include "tables.h"
#include "lookup.h"
#include "clan.h"
#include "worship.h"
#include "interp.h"
#include "subclass.h"

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#if     defined(unix)
#define __STRICT_ANSI__ 1
#include <netdb.h>
#include <netinet/in.h>
#endif

typedef struct multdata MULTDATA;

struct multdata
{
   DESCRIPTOR_DATA *des;
};

/* command procedures needed */
DECLARE_DO_FUN(do_rstat        );
DECLARE_DO_FUN(do_mstat        );
DECLARE_DO_FUN(do_ostat        );
DECLARE_DO_FUN(do_rset         );
DECLARE_DO_FUN(do_mset         );
DECLARE_DO_FUN(do_maxset       );
DECLARE_DO_FUN(do_oset         );
DECLARE_DO_FUN(do_sset         );
DECLARE_DO_FUN(do_dset         );
DECLARE_DO_FUN(do_exitset      );
DECLARE_DO_FUN(do_learnlvl     );
DECLARE_DO_FUN(do_mfind        );
DECLARE_DO_FUN(do_ofind        );
DECLARE_DO_FUN(do_slookup      );
DECLARE_DO_FUN(do_mload        );
DECLARE_DO_FUN(do_oload        );
DECLARE_DO_FUN(do_loadarea     );
DECLARE_DO_FUN(do_quit         );
DECLARE_DO_FUN(do_look         );
DECLARE_DO_FUN(do_stand        );
DECLARE_DO_FUN(do_educate      );
DECLARE_DO_FUN(do_last_logon   );
DECLARE_DO_FUN(do_nofight      );
DECLARE_SPELL_FUN( spell_null );
void do_dossier_modify  args( (CHAR_DATA* ch, char* argument, CHAR_DATA* vch) );
int compare_wizi args ( (const void* input1, const void* input2) );
int compare_wizi_type = -1;
bool compare_wizi_descend = FALSE;
void do_house_guardian_talk   args( (int house, char* argument) );
void grant_house_skill     args( (CHAR_DATA* ch, SKILL_LIST_TYPE* skill) );
void grant_house_skills    args( (CHAR_DATA* ch) );
void lose_all_house_skills args( (CHAR_DATA* ch) );
void lose_house_skills     args( (CHAR_DATA* ch, int house) );
void racial_skills   args( (CHAR_DATA* ch) );

/*
* Local functions.
*/
ROOM_INDEX_DATA *    find_location    args( ( CHAR_DATA *ch, char *arg ) );
char * convert_color args( ( char * strbuf, bool ISANSI ) );
char * get_ip_compare_string args( (char * string, char * ret_str ) );
int host_comp_wrapper args ( (const void* d1, const void* d2) );
int host_comp args ( (MULTDATA* d1, MULTDATA* d2) );

void make_demon(CHAR_DATA * demon, int dtype);


char * get_ip_compare_string(char * string, char * ret_str)
{
   int    dot_count = 0;
   int    letter;

   for (letter = 0; string[letter] != '\0'; letter++)
   {
      if (string[letter] == '.')
      dot_count++;
      if (dot_count < 3)
      ret_str[letter] = string[letter];
   }
   ret_str[letter+1] = '\0';
   return ret_str;
}


void do_wiznet( CHAR_DATA *ch, char *argument )
{
   int flag;
   char buf[MAX_STRING_LENGTH];

   if ( argument[0] == '\0' )
   {
      if (IS_SET(ch->wiznet, WIZ_ON))
      {
         send_to_char("Signing off of Wiznet.\n\r", ch);
         REMOVE_BIT(ch->wiznet, WIZ_ON);
      }
      else
      {
         send_to_char("Welcome to Wiznet!\n\r", ch);
         SET_BIT(ch->wiznet, WIZ_ON);
      }
      return;
   }

   if (!str_prefix(argument, "on"))
   {
      send_to_char("Welcome to Wiznet!\n\r", ch);
      SET_BIT(ch->wiznet, WIZ_ON);
      return;
   }

   if (!str_prefix(argument, "off"))
   {
      send_to_char("Signing off of Wiznet.\n\r", ch);
      REMOVE_BIT(ch->wiznet, WIZ_ON);
      return;
   }

   /* show wiznet status */
   if (!str_prefix(argument, "status"))
   {
      buf[0] = '\0';

      if (!IS_SET(ch->wiznet, WIZ_ON))
      strcat(buf, "off ");

      for (flag = 0; wiznet_table[flag].name != NULL; flag++)
      {
         if (flag <= 28)
         {
            if (IS_SET(ch->wiznet, wiznet_table[flag].flag))
            {
               strcat(buf, wiznet_table[flag].name);
               strcat(buf, " ");
            }
         } else
         {
            if (IS_SET(ch->wiznet2, wiznet_table[flag].flag))
            {
               strcat(buf, wiznet_table[flag].name);
               strcat(buf, " ");
            }
         }
      }

      strcat(buf, "\n\r");

      send_to_char("Wiznet status:\n\r", ch);
      send_to_char(buf, ch);
      return;
   }

   if (!str_prefix(argument, "show"))
   /* list of all wiznet options */
   {
      buf[0] = '\0';

      for (flag = 0; wiznet_table[flag].name != NULL; flag++)
      {
         if (wiznet_table[flag].level <= get_trust(ch))
         {
            strcat(buf, wiznet_table[flag].name);
            strcat(buf, " ");
         }
      }

      strcat(buf, "\n\r");

      send_to_char("Wiznet options available to you are:\n\r", ch);
      send_to_char(buf, ch);
      return;
   }

   flag = wiznet_lookup(argument);

   if (flag == -1 || get_trust(ch) < wiznet_table[flag].level)
   {
      send_to_char("No such option.\n\r", ch);
      return;
   }

   if (
         (flag <= 28 && IS_SET(ch->wiznet, wiznet_table[flag].flag)) ||
         (flag > 28 && IS_SET(ch->wiznet2, wiznet_table[flag].flag))
      )
   {
      sprintf(buf, "You will no longer see %s on wiznet.\n\r",
      wiznet_table[flag].name);
      send_to_char(buf, ch);
      if (flag <= 28)
      {
         REMOVE_BIT(ch->wiznet, wiznet_table[flag].flag);
      } else
      {
         REMOVE_BIT(ch->wiznet2, wiznet_table[flag].flag);
      }
      return;
   }
   else
   {
      sprintf(buf, "You will now see %s on wiznet.\n\r",
      wiznet_table[flag].name);
      send_to_char(buf, ch);
      if (flag <= 28)
      {
         SET_BIT(ch->wiznet, wiznet_table[flag].flag);
      } else
      {
         SET_BIT(ch->wiznet2, wiznet_table[flag].flag);
      }
      return;
   }

}

void wiznet(char* string, CHAR_DATA* ch, OBJ_DATA* obj, long flag, long flag2, int min_level)
{
   DESCRIPTOR_DATA* d;

   for ( d = descriptor_list; d != NULL; d = d->next )
   {
      if
      (
         d->connected == CON_PLAYING &&
         (d->character) &&
         IS_IMMORTAL(d->character) &&
         IS_SET(d->character->wiznet, WIZ_ON) &&
         (
            !flag ||
            IS_SET(d->character->wiznet, flag)
         ) &&
         (
            !flag2 ||
            IS_SET(d->character->wiznet2, flag2)
         ) &&
         get_trust(d->character) >= min_level &&
         d->character != ch
      )
      {
         if (
               flag2 == WIZ_NAME &&
               (
                  !stristr((char *) obj, d->character->name) &&
                  (
                     d->original == NULL ||
                     !stristr((char *) obj, d->original->name)
                  )
               )
            )
         {
            continue;
         }

         if (IS_SET(d->character->wiznet, WIZ_PREFIX))
         {
            send_to_char("--> ", d->character);
         }
         act_nnew(string, d->character, obj, ch, TO_CHAR, POS_DEAD);
      }
   }

   return;
}

void do_makelich( CHAR_DATA *ch, char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   OBJ_DATA *phyl;
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA *victim;

   if (IS_NPC(ch))
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }

   argument = one_argument( argument, arg1 );

   if ( arg1[0] == '\0' )
   {
      send_to_char( "Syntax: makelich <char>\n\r", ch);
      return;
   }

   if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
   {
      send_to_char("They aren't playing.\n\r", ch);
      return;
   }

   if (IS_NPC(victim))
   {
      send_to_char("Mobs can't become liches.\n\r", ch);
      return;
   }

   if (IS_SET(victim->act2, PLR_LICH))
   {
      REMOVE_BIT(victim->act2, PLR_LICH);
      send_to_char("That person is no longer a lich.\n\r", ch);
      send_to_char("You are no longer a lich.\n\r", victim);
      return;
   }

   phyl = create_object(get_obj_index(OBJ_VNUM_PHYLACTERY), 0);
   sprintf(buf, "%s", victim->name);
   free_string(phyl->owner);
   phyl->owner = str_dup(buf);
   obj_to_char(phyl, victim);
   SET_BIT(victim->act2, PLR_LICH);
   victim->perm_stat[STAT_CON] = 20;
   send_to_char("That person is now a lich.\n\r", ch);
   send_to_char("You are now a lich.\n\r", victim);
   return;
}

void do_promote(CHAR_DATA* ch, char* argument)
{
   char arg1[MAX_STRING_LENGTH];
   char arg2[MAX_STRING_LENGTH];
   char arg3[MAX_STRING_LENGTH];
   char  buf[MAX_STRING_LENGTH];
   bool brand_promote;
   CHAR_DATA* victim;
   OBJ_DATA*  brand;
   OBJ_DATA*  brand2;
   int rank = 0;

   if (IS_NPC(ch))
   {
      return;
   }

   argument = one_argument(argument, arg1);
   argument = one_argument(argument, arg2);
   argument = one_argument(argument, arg3);

   if (arg1[0] == '\0' && ch->class == CLASS_MONK)
   {
      do_give_belt(ch, argument);
      return;
   }
   if (!str_cmp(arg1, "belt"))
   {
      do_give_belt(ch, argument);
      return;
   }

   /* intelligent picking of house/brand promote type*/
   if (!str_cmp(arg1, "brand"))
   {
      brand_promote = TRUE;
   }
   else
   {
      brand_promote = FALSE;
   }

   if (!brand_promote)
   {
      if ((victim = get_char_world(ch, arg1)) == NULL)
      {
         send_to_char("Attempt to promote who?\n\r", ch);
         return;
      }
   }
   else
   {
      if ((victim = get_char_world(ch, arg2)) == NULL)
      {
         send_to_char("Attempt to promote who?\n\r", ch);
         return;
      }
   }

   if (IS_NPC(victim))
   {
      send_to_char("Trying to promote a mob?\n\r", ch);
      return;
   }

   if (!IS_IMMORTAL(ch) && victim == ch)
   {
      send_to_char("You can't promote yourself!\n\r", ch);
      return;
   }

   if (arg3 == '\0')
   {
      sprintf(buf, "[%s] attempted to promote [%s].", ch->name, victim->name);
   }
   else
   {
      sprintf(buf, "[%s] attempted to promote [%s] to [%s].", ch->name, victim->name, arg3);
   }
   log_string(buf);


   if (brand_promote)
   {
      if (!IS_IMMORTAL(ch))
      {
         send_to_char("You cannot do that.\n\r", ch);
         return;
      }
      brand = get_eq_char(ch, WEAR_BRAND);
      if (brand == NULL)
      {
         send_to_char("You have no brand.\n\r", ch);
         return;
      }
      brand2 = get_eq_char(victim, WEAR_BRAND);
      if
      (
         brand2 == NULL ||
         brand->pIndexData->vnum != brand2->pIndexData->vnum
      )
      {
         send_to_char("They do not wear your brand.\n\r", ch);
         return;
      }
      rank = atoi(arg3);
      if (brand->pIndexData->vnum == OBJ_VNUM_TWIG_BRAND)
      {
         if (rank > victim->pcdata->brand_rank)
         {
            send_to_char("You feel younger.\n\r", victim);
         }
         else if (rank < victim->pcdata->brand_rank)
         {
            send_to_char("You feel older.\n\r", victim);
         }
      }
      if (brand->pIndexData->vnum == OBJ_VNUM_DRINLINDA_BRAND)
      {
         if (rank > victim->pcdata->brand_rank)
         {
            send_to_char("You feel Drinlinda's power surge through you with greater intensity!\n\r", victim);
         }
         else if (rank < victim->pcdata->brand_rank)
         {
            send_to_char("You feel Drinlinda's power surge through you with a lesser intensity.\n\r", victim);
         }
      }
      victim->pcdata->brand_rank = rank;
      send_to_char("Their religious rank has been set.\n\r", ch);
      return;
   }

   /* wasn't brand must be house */
   if (ch->house == 0)
   {
      send_to_char("You are not in a house.\n\r", ch);
      return;
   }
   if (victim->house != ch->house)
   {
      send_to_char("They are not part of your house.\n\r", ch);
      return;
   }

   if
   (
      ch->house == HOUSE_MARAUDER
   )
   {
      if
      (
        !IS_IMMORTAL(ch)
      )
      {
         if
         (
            ch->pcdata->induct < RANK_GUILDMASTER
         )
         {
            send_to_char("Huh?\n\r", ch);
            return;
         }
         else if
         (
            victim->house != HOUSE_MARAUDER
         )
         {
            send_to_char("You can only promote within the Guild.\n\r", ch);
            return;
         }
         else if
         (
            ch->pcdata->induct == RANK_GUILDMASTER &&
            victim->pcdata->induct >= ch->pcdata->induct
         )
         {
            send_to_char("Guild is ashamed of your stupid promotion speeches.\n\r", ch);
            return;
         }
         else if
         (
            ch->pcdata->induct == RANK_GUILDMASTER &&
            ch->pcdata->house_rank2 != victim->pcdata->house_rank2
         )
         {
            send_to_char("You can only promote within your own sect.\n\r", ch);
            return;
         }
         else if
         (
            (
               ch->pcdata->induct - victim->pcdata->induct
            ) == 1 ||  /* Difference between Journeyman and Guildmaster */
            (
               ch->pcdata->induct == RANK_HEADMASTER &&
               victim->pcdata->induct == RANK_GUILDMASTER
            )
         )
         {
            send_to_char("What are you? Insane? Promoting them to your own position!\n\r", ch);
            return;
         }
      }
      act("$N has proven $mself. You elevate $N's position within the Guild.", ch, NULL, victim, TO_CHAR);
      if
      (
         victim->pcdata->induct == RANK_APPRENTICE
      )
      {
         send_to_char("You have proven yourself to the Guild and are elevated to Marauder.\n\r", victim);
         victim->pcdata->induct = RANK_MARAUDER;
      }
      else if
      (
         victim->pcdata->induct == RANK_MARAUDER
      )
      {
         send_to_char("You have proven yourself to the Guild and are elevated to Journeyman.\n\r", victim);
         victim->pcdata->induct = RANK_JOURNEYMAN;
      }
      else if
      (
         victim->pcdata->induct == RANK_JOURNEYMAN
      )
      {
         send_to_char("You have proven yourself to the Guild and are elevated to Guildmaster.\n\r", victim);
         victim->pcdata->induct = RANK_GUILDMASTER;
      }
      else if
      (
         victim->pcdata->induct == RANK_GUILDMASTER
      )
      {
         send_to_char("You have proven yourself to the Guild and are elevated to Headmaster.\n\r", victim);
         victim->pcdata->induct = RANK_HEADMASTER;
      }
      else if
      (
         victim->pcdata->induct == RANK_HEADMASTER
      )
      {
         act("$E is already at the highest rank of the Guild.", ch, NULL, victim, TO_CHAR);
         return;
      }
      else
      {
         bug("BUG from Marauder promote in do_promote", 0);
         return;
      }
      grant_house_skills(victim);
      return;
   }

   /* Covenant ranks for info titles */
   if (ch->house == HOUSE_COVENANT)
   {
      /* new Covenant ranks -- Drinlinda */
      rank = -1;
      if (!str_cmp(arg2, "Squire"))
      {
         rank = RANK_SQUIRE;
      }
      else if (!str_cmp(arg2, "FM"))
      {
         rank = RANK_FOOTMAN;
      }
      else if (!str_cmp(arg2, "MFM"))
      {
         rank = RANK_MASTERFOOTMAN;
      }
      else if (!str_cmp(arg2, "CPL"))
      {
         rank = RANK_CORPORAL;
      }
      else if (!str_cmp(arg2, "SPC"))
      {
         rank = RANK_SPECIALIST;
      }
      else if (!str_cmp(arg2, "MSPC"))
      {
         rank = RANK_MASTERSPECIALIST;
      }
      else if (!str_cmp(arg2, "MCPL"))
      {
         rank = RANK_MASTERCORPORAL;
      }
      else if (!str_cmp(arg2, "SGT"))
      {
         rank = RANK_SERGEANT;
      }
      else if (!str_cmp(arg2, "MSG"))
      {
         rank = RANK_MASTERSERGEANT;
      }
      else if (!str_cmp(arg2, "SGM"))
      {
         rank = RANK_SERGEANTMAJOR;
      }
      else if (!str_cmp(arg2, "WO"))
      {
         rank = RANK_WARRANTOFFICER;
      }
      else if (!str_cmp(arg2, "MWO"))
      {
         rank = RANK_MASTERWARRANTOFFICER;
      }
      else if (!str_cmp(arg2, "CWO"))
      {
         rank = RANK_CHIEFWARRANTOFFICER;
      }
      else if (!str_cmp(arg2, "2LT"))
      {
         rank = RANK_SECONDLIEUTENANT;
      }
      else if (!str_cmp(arg2, "LT"))
      {
         rank = RANK_LIEUTENANT;
      }
      else if (!str_cmp(arg2, "CPT"))
      {
         rank = RANK_CAPTAIN;
      }
      else if (!str_cmp(arg2, "MAJ"))
      {
         rank = RANK_MAJOR;
      }
      else if (!str_cmp(arg2, "LTC"))
      {
         rank = RANK_LIEUTENANTCOLONEL;
      }
      else if (!str_cmp(arg2, "COL"))
      {
         rank = RANK_COLONEL;
      }
      else if (!str_cmp(arg2, "BG"))
      {
         rank = RANK_BRIGADIERGENERAL;
      }
      else if (!str_cmp(arg2, "MG"))
      {
         rank = RANK_MAJORGENERAL;
      }
      else if (!str_cmp(arg2, "LTG"))
      {
         rank = RANK_LIEUTENANTGENERAL;
      }
      else if (!str_cmp(arg2, "GEN"))
      {
         rank = RANK_GENERAL;
      }
      if (rank == -1)
      {
         send_to_char
         (
            "Rank must be (enlisted): Squire, FM, MFM, CPL, SPC, MCPL, MSPC,"
            " SGT, MSG, SGM, WO, MWO, or CWO.\n\r",
            ch
         );
         send_to_char
         (
            "Rank must be (officer): 2LT, 1LT, CPT, MAJ, LTC, COL, BG, MG, LTG,"
            " or GEN.\n\r",
            ch
         );
         return;
      }
      if (!IS_IMMORTAL(ch) && ch->pcdata->house_rank != RANK_GENERAL)
      {
         send_to_char("You cannot promote in this house.\n\r", ch);
         return;
      }
      if (!IS_IMMORTAL(ch) && rank == RANK_GENERAL)
      {
         send_to_char
         (
            "Let those who are higher in rank than you handle this business, Knight!.\n\r",
            ch
         );
         return;
      }
      if (victim->house != HOUSE_COVENANT)
      {
         send_to_char("You must induct them first.\n\r", ch);
         return;
      }

      /* Covenant branch title restrictions -- Drinlinda */
      if
      (
         victim->class == CLASS_MONK ||
         victim->class == CLASS_NIGHTWALKER ||
         victim->class == CLASS_THIEF
      )
      {
         switch (rank)
         {
            case (RANK_CORPORAL):
            {
               send_to_char
               (
                  "Covert Operations knights cannot be Corporals, promote them"
                  " to Specialists instead.\n\r",
                  ch
               );
               return;
            }
            case (RANK_MASTERCORPORAL):
            {
               send_to_char
               (
                  "Covert Operations knights cannot be Master Corporals,"
                  " promote them to Master Specialists instead.\n\r",
                  ch
               );
               return;
            }
            default:
            {
               break;
            }
         }
      }
      else if (victim->class == CLASS_RANGER)
      {
         switch (rank)
         {
            case (RANK_SPECIALIST):
            {
               send_to_char
               (
                  "Pathfinder knights cannot be Specialists, promote them to"
                  " Corporal instead.\n\r", ch
               );
               return;
            }
            case (RANK_MASTERSPECIALIST):
            {
               send_to_char
               (
                  "Pathfinder knights cannot be Master Specialists, promote them to"
                  " Master Corporal instead.\n\r",
                  ch
               );
               return;
            }

            default:
            {
               break;
            }
         }
      }
      else if (victim->class == CLASS_WARRIOR || victim->class == CLASS_ANTI_PALADIN)
      {
         switch (rank)
         {
            case (RANK_SPECIALIST):
            {
               send_to_char
               (
                  "Infantry knights cannot be Specialists, promote them to"
                  " Corporal instead.\n\r",
                  ch
               );
               return;
            }
            case (RANK_MASTERSPECIALIST):
            {
               send_to_char
               (
                  "Infantry knights cannot be Master Specialists, promote them to"
                  " Master Corporal instead.\n\r",
                  ch
               );
               return;
            }
            default:
            {
               break;
            }
         }
      }
      else if (victim->class == CLASS_CLERIC)
      {
         switch (rank)
         {
            case (RANK_SPECIALIST):
            {
               send_to_char
               (
                  "Chaplains cannot be Specialists, promote them to"
                  " Corporal instead.\n\r",
                  ch
               );
               return;
            }
            case (RANK_MASTERSPECIALIST):
            {
               send_to_char
               (
                  "Chaplains cannot be Master Specialists, promote them to"
                  " Master Corporal instead.\n\r",
                  ch
               );
               return;
            }
            default:
            {
               break;
            }
         }
         if (victim->pcdata->special != SUBCLASS_ZEALOT_FAITH)
         {
            send_to_char
            (
               "Clerics of their specialization should not be Chaplains.\n\r",
               ch
            );
            return;
         }
         if (rank > RANK_CHIEFWARRANTOFFICER)
         {
            send_to_char("Chaplains cannot achieve officer ranks.\n\r", ch);
            return;
         }
      }

      /* new Covenant rank skill assignment -- Drinlinda */
      switch (rank)
      {
         case (RANK_SQUIRE):
         {
            skill_lose(victim, "jump");
            skill_lose(victim, "recruit");
            skill_lose(victim, "swing");
            skill_lose(victim, "shackles");
            skill_lose(victim, "aura of presence");
            skill_lose(victim, "impale");
            skill_gain(victim, "devote", 15);
            skill_gain(victim, "dark armor", 15);

            victim->pcdata->induct = 0;
            break;
         }
         case RANK_FOOTMAN:
         case RANK_MASTERFOOTMAN:
         case RANK_CORPORAL:
         case RANK_SPECIALIST:
         case RANK_MASTERCORPORAL:
         case RANK_MASTERSPECIALIST:
         case RANK_SERGEANT:
         case RANK_MASTERSERGEANT:
         case RANK_SERGEANTMAJOR:
         case RANK_WARRANTOFFICER:
         case RANK_MASTERWARRANTOFFICER:
         case RANK_CHIEFWARRANTOFFICER:
         {
            skill_lose(victim, "impale");
            skill_lose(victim, "aura of presence");
            skill_gain(victim, "shackles", 15);
            skill_gain(victim, "devote", 15);
            skill_gain(victim, "jump", 15);
            if (victim->class == CLASS_CLERIC)
            {
               skill_lose(victim, "recruit");
               skill_lose(victim, "swing");
               skill_lose(victim, "dark armor");
            }
            else
            {
               skill_gain(victim, "recruit", 15);
               skill_gain(victim, "swing", 15);
               skill_gain(victim, "dark armor", 15);
            }
            victim->pcdata->induct = 0;
            break;
         }
         case RANK_SECONDLIEUTENANT:
         case RANK_LIEUTENANT:
         case RANK_CAPTAIN:
         case RANK_MAJOR:
         case RANK_LIEUTENANTCOLONEL:
         case RANK_COLONEL:
         {
            skill_lose(victim, "aura of presence");
            skill_gain(victim, "impale", 15);
            skill_gain(victim, "jump", 15);
            skill_gain(victim, "recruit", 15);
            skill_gain(victim, "swing", 15);
            skill_gain(victim, "shackles", 15);
            skill_gain(victim, "dark armor", 15);
            skill_gain(victim, "devote", 15);
            victim->pcdata->induct = 0;
            break;
         }
         case RANK_BRIGADIERGENERAL:
         case RANK_MAJORGENERAL:
         case RANK_LIEUTENANTGENERAL:
         {
            skill_gain(victim, "impale", 15);
            skill_gain(victim, "aura of presence", 15);
            skill_gain(victim, "jump", 15);
            skill_gain(victim, "recruit", 15);
            skill_gain(victim, "swing", 15);
            skill_gain(victim, "shackles", 15);
            skill_gain(victim, "dark armor", 15);
            skill_gain(victim, "devote", 15);
            victim->pcdata->induct = 4;
            break;
         }
         case RANK_GENERAL:
         {
            skill_gain(victim, "impale", 15);
            skill_gain(victim, "aura of presence", 15);
            skill_gain(victim, "jump", 15);
            skill_gain(victim, "recruit", 15);
            skill_gain(victim, "swing", 15);
            skill_gain(victim, "shackles", 15);
            skill_gain(victim, "dark armor", 15);
            skill_gain(victim, "devote", 15);
            victim->pcdata->induct = 5;
            break;
         }
         default:
         send_to_char("Promote to what rank?\n\r", ch);
         break;
      }
      victim->pcdata->house_rank = rank;
      switch (victim->pcdata->house_rank)
      {
         case (RANK_SQUIRE):
         {
            send_to_char
            (
               "You are now a Squire.\n\rYour chevron is an X cross.\n\r",
               victim
            );
            break;
         }
         case (RANK_FOOTMAN):
         {
            send_to_char
            (
               "You are now a Footman.\n\rYour chevron is one hook.\n\r",
               victim
            );
            break;
         }
         case (RANK_MASTERFOOTMAN):
         {
            send_to_char
            (
               "You are now a Master Footman.\n\r"
               "Your chevron is one hook below a dragon",
               victim
            );
            break;
         }
         case (RANK_CORPORAL):
         {
            if (victim->class == CLASS_RANGER)
            {
               send_to_char
               (
                  "You are now a Corporal.\n\rYour chevron is two hooks.\n\r",
                  victim
               );
            }
            break;
         }
         case (RANK_MASTERCORPORAL):
         {
            send_to_char
            (
               "You are now a Master Corporal.\n\rYour chevron is two"
               " hooks below a dragon.\n\r",
               victim
            );
            break;
         }
         case (RANK_SPECIALIST):
         {
            send_to_char
            (
               "You are now a Specialist.\n\rYour chevron is two"
               " hooks.\n\r",
               victim
            );
            break;
         }
         case (RANK_MASTERSPECIALIST):
         {
            send_to_char
            (
               "You are now a Specialist.\n\rYour chevron is two"
               " hooks below a dagger.\n\r",
               victim
            );
            break;
         }
         case (RANK_SERGEANT):
         {
            send_to_char
            (
               "You are now a Sergeant.\n\rYour chevron is three hooks.\n\r",
               victim
            );
            break;
         }
         case (RANK_MASTERSERGEANT):
         {
            send_to_char
            (
               "You are now a Master Sergeant.\n\rYour chevron is three hooks"
               " below a dragon\n\r",
               victim
            );
            break;
         }
         case (RANK_SERGEANTMAJOR):
         {
            send_to_char
            (
               "You are now a Sergeant Major.\n\rYour chevron is three"
               " hooks below a dragon and a broadsword.\n\r",
               victim
            );
            break;
         }
         case (RANK_WARRANTOFFICER):
         {
            send_to_char
            (
               "You are now a Warrant Officer.\n\rYour chevron is a pair of"
               " crossed battleaxes.\n\r",
               victim
            );
            break;
         }
         case (RANK_MASTERWARRANTOFFICER):
         {
            send_to_char
            (
               "You are now a Master Warrant Officer.\n\rYour chevron is a pair of"
               " crossed battleaxes wreathed with daggers.\n\r",
               victim
            );
            break;
         }
         case (RANK_CHIEFWARRANTOFFICER):
         {
            send_to_char
            (
               "You are now a Chief Warrant Officer.\n\rYour chevron is a dragon"
               " clutching a broadsword.\n\r",
               victim
            );
            break;
         }
         case (RANK_SECONDLIEUTENANT):
         {
            send_to_char
            (
               "You are now a Second Lieutenant.\n\rYour chevron is one thin silver bar.\n\r"
               "You have been taught how to impale your foes.\n\r",
               victim
            );
            break;
         }
         case (RANK_LIEUTENANT):
         {
            send_to_char
            (
               "You are now a Lieutenant.\n\rYour chevron is one thick silver bar.\n\r",
               victim
            );
            break;
         }
         case (RANK_CAPTAIN):
         {
            send_to_char
            (
               "You are now a Captain.\n\r"
               "Your chevron is two thick gold bars.\n\r",
               victim
            );
            break;
         }
         case (RANK_MAJOR):
         {
            send_to_char
            (
               "You are now a Major.\n\r"
               "Your chevron is one thin gold bar between two thick gold bars.\n\r",
               victim
            );
            break;
         }
         case (RANK_LIEUTENANTCOLONEL):
         {
            send_to_char
            (
               "You are now a Lieutenant Colonel.\n\rYour chevron is three thick"
               " gold bars.\n\r",
               victim
            );
            break;
         }
         case (RANK_COLONEL):
         {
            send_to_char
            (
               "You are now a Colonel.\n\rYour chevron is four thick gold bars.\n\r",
               victim
            );
            break;
         }
         case (RANK_BRIGADIERGENERAL):
         {
            send_to_char
            (
               "You are now a Brigadier General.\n\rYour chevron is one silver star.\n\r"
               "You have been issued a battledragon to replace your nightmare.\n\r"
               "You have been taught to project an aura of presence.\n\r"
               "You can now ally.\n\r",
               victim
            );
            break;
         }
         case (RANK_MAJORGENERAL):
         {
            send_to_char
            (
               "You are now a Major General.\n\rYour chevron is two silver stars.\n\r",
               victim
            );
            break;
         }
         case (RANK_LIEUTENANTGENERAL):
         {
            send_to_char
            (
               "You are now a Lieutenant General.\n\rYour chevron is three silver stars.\n\r",
               victim
            );
            break;
         }
         case (RANK_GENERAL):
         {
            send_to_char
            (
               "You are now a General.\n\rYour chevron is four silver stars.\n\r"
               "You can now induct, uninduct, and promote.\n\r",
               victim
            );
            break;
         }
      }
      act
      (
         "$N's rank has been changed to $t.",
         ch,
         get_house_title(victim),
         victim,
         TO_CHAR
      );
      if (ch != victim)
      {
         send_to_char("Your rank within the Covenant Army has changed.\n\r", victim);
      }
      return;
   }

   send_to_char("You cannot do that.\n\r", ch);
   return;
}

void do_leader( CHAR_DATA *ch, char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA *victim;

   if (IS_NPC(ch))
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }

   argument = one_argument( argument, arg1 );

   if ( arg1[0] == '\0' )
   {
      send_to_char( "Syntax: leader <char>\n\r", ch);
      return;
   }

   if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
   {
      send_to_char("They aren't playing.\n\r", ch);
      return;
   }

   if (IS_NPC(victim))
   {
      send_to_char("Can't make mobs leaders.\n\r", ch);
      return;
   }

   if (victim->pcdata->induct == 5)
   {
      send_to_char("Your power to induct has been taken away!\n\r", victim);
      sprintf(buf, "You have taken away %s's power to induct.\n\r", victim->name);
      send_to_char(buf, ch);
      victim->pcdata->induct = 0;
   }
   else
   {
      send_to_char("You have been given the power to induct!\n\r", victim);
      sprintf(buf, "You have given %s the power to induct.\n\r", victim->name);
      send_to_char(buf, ch);
      victim->pcdata->induct = 5;
   }
   grant_house_skills(victim);
   return;
}

/*
   House Guardian Talk for imms. Designed for RP purposes.
   Will allow an imm to talk from the mouth of a house guard
   without switching or forcing the guardian.  -  Konti
*/
void do_hgtalk( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   int house;

   if (ch->level < ANGEL || IS_NPC(ch))
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }
   argument = one_argument(argument, arg);

   if (arg[0] == '\0' || argument[0] == '\0')
   {
      send_to_char("Syntax: hgtalk <house> <say what>\n\r", ch);
      send_to_char
      (
         "      : hgtalk justicar Hello there!\n\r"
         "      : hgtalk marauder Do it, just do it!\n\r",
         ch
      );
      return;
   }
   if
   (
      (house = house_lookup(arg)) == 0
   )
   {
      send_to_char("No such house exists.\n\r", ch);
      return;
   }
   do_house_guardian_talk(house, argument);
}

/*
   do_house_guardian_talk is used by do_hgtalk and also within
   induction code, for whenever house guard needs to say something
   this piece of code can find the guardian and make it talk. - Konti
*/
void do_house_guardian_talk(int house, char* argument)
{
   CHAR_DATA* h_guardian;

   if
   (
      house < 0 ||
      house >= MAX_HOUSE ||
      house_table[house].vnum_guardian == 0
   )
   {
      return;
   }
   else
   {
      for ( h_guardian = char_list; h_guardian != NULL; h_guardian = h_guardian->next )
      {
         if
         (
            IS_NPC(h_guardian) &&
            h_guardian->pIndexData->vnum == house_table[house].vnum_guardian
         )
         {
            do_housetalk(h_guardian, argument);
            break;
         }
      }
   }
}

void do_conference( CHAR_DATA *ch, char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   int conferenceNumber;

   if (ch->level < 52 || IS_NPC(ch))
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }

   argument = one_argument( argument, arg1 );

   if ( arg1[0] == '\0' )
   {
      send_to_char( "Syntax: conference <0-255> <char>\n\r", ch );
      send_to_char( "0: <char> has no conference\n\r", ch );
      return;
   }

   conferenceNumber = atoi( arg1 );

   argument = one_argument( argument, arg2 );

   if ( arg2[0] == '\0' )
   {
      send_to_char( "Syntax: conference <0-255> <char>\n\r", ch );
      send_to_char( "0: <char> has no conference\n\r", ch );
      return;
   }

   if ( ( victim = get_char_world( ch, arg2 ) ) == NULL )
   {
      send_to_char("They aren't playing.\n\r", ch);
      return;
   }

   victim->conference = conferenceNumber;
   if ( conferenceNumber )
   {
      send_to_char( "They are now part of the conference.\n\r", ch );
      send_to_char( "You have been added to a conference!\n\r", victim );
   }
   else
   {
      send_to_char( "They are removed from the conference.\n\r", ch );
      send_to_char( "You have been removed from the conference.\n\r", victim );
   }

   return;
}

void do_guildmaster( CHAR_DATA *ch, char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA *victim;

   argument = one_argument( argument, arg1 );

   if ( arg1[0] == '\0' )
   {
      send_to_char( "Syntax: guildmaster <char>\n\r", ch);
      return;
   }

   if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
   {
      send_to_char("They aren't playing.\n\r", ch);
      return;
   }

   if (IS_NPC(victim))
   {
      send_to_char("Use 'flag' for mobs.\n\r", ch);
      return;
   }

   if (IS_SET(victim->act2, PLR_GUILDMASTER))
   {
      send_to_char("You are no longer a Guildmaster!\n\r", victim);
      sprintf(buf, "You have taken away %s's Guildmaster position.\n\r", victim->name);
      send_to_char(buf, ch);
      REMOVE_BIT(victim->act2, PLR_GUILDMASTER);
      return;
   }
   else
   {
      send_to_char("You have become a Guildmaster!\n\r", victim);
      sprintf(buf, "You have given %s Guildmaster status.\n\r", victim->name);
      send_to_char(buf, ch);
      SET_BIT(victim->act2, PLR_GUILDMASTER);
      return;
   }
   return;
}


void do_elder( CHAR_DATA *ch, char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA *victim;

   if
   (
      IS_NPC(ch) ||
      (
         get_trust(ch) < 53 &&
         ch->pcdata->induct != 5
      )
   )
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }

   argument = one_argument( argument, arg1 );

   if ( arg1[0] == '\0' )
   {
      send_to_char( "Syntax: elder <char>\n\r", ch);
      return;
   }

   if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
   {
      send_to_char("They aren't playing.\n\r", ch);
      return;
   }

   if (IS_NPC(victim))
   {
      send_to_char("Can't make mobs elders.\n\r", ch);
      return;
   }

   if (victim->pcdata->induct == 3)
   {
      if (victim->house == HOUSE_CONCLAVE)
      {
         send_to_char
         (
            "You are a normal member of the Conclave member.\n\r",
            victim
         );
         if (victim->pcdata->true_sex == SEX_FEMALE)
         {
            act(
               "You have taken away $N's power as an ArchWizardess of Conclave.",
               ch,
               NULL,
               victim,
               TO_CHAR);
         }
         else
         {
            act(
               "You have taken away $N's power as an ArchWizard of Conclave.",
               ch,
               NULL,
               victim,
               TO_CHAR);
         }
      }
      else
      {
         send_to_char("Your power to novice people has been taken away!\n\r", victim);
         sprintf(buf, "You have taken away %s's power to novice people.\n\r", victim->name);
         send_to_char(buf, ch);
      }
      victim->pcdata->induct = 0;
   }
   else
   {
      if (victim->house == HOUSE_CONCLAVE)
      {
         if (victim->pcdata->true_sex == SEX_FEMALE)
         {
            send_to_char(
               "You are now an ArchWizardess of the Conclave!\n\r",
               victim);
            act(
               "You have made $N an ArchWizardess of the Conclave!",
               ch,
               NULL,
               victim,
               TO_CHAR);
         }
         else
         {
            send_to_char(
               "You are now an ArchWizard of the Conclave!\n\r",
               victim);
            act(
               "You have made $N an ArchWizard of the Conclave!",
               ch,
               NULL,
               victim,
               TO_CHAR);
         }
      }
      else
      {
         send_to_char("You have been given the power to novice people!\n\r", victim);
         sprintf(buf, "You have given %s the power to novice people.\n\r", victim->name);
         send_to_char(buf, ch);
      }
      victim->pcdata->induct = 3;
   }
   grant_house_skills(victim);
   return;
}


void do_smite(CHAR_DATA* ch, char* argument)
{
   CHAR_DATA* victim;
   char arg1[MAX_INPUT_LENGTH];
   AFFECT_DATA smite_effect;
   char arg2[MAX_STRING_LENGTH];

   if (IS_NPC(ch))
   {
      send_to_char("Mobs cannot smite.\n\r", ch);
      return;
   }

   argument = one_argument(argument, arg1);
   argument = one_argument(argument, arg2);


   if (arg1[0] == '\0')
   {
      send_to_char("Syntax: smite <char> [<location>]\n\r", ch);
      return;
   }

   if ((victim = get_char_world(ch, arg1)) == NULL)
   {
      send_to_char("They are not playing.\n\r", ch);
      return;
   }

   if (IS_NPC(victim))
   {
      send_to_char(
         "Smiting a mob is like yelling at a wall: no impression made.\n\r",
         ch);
      return;
   }

   if (ch == victim)
   {
      send_to_char("Masochist.\n\r", ch);
      return;
   }

   if (!str_cmp(arg2, "foot"))
   {
      act(
         "A bolt from the heavens strikes $n's foot!",
         victim,
         NULL,
         ch,
         TO_NOTVICT);
      act(
         "A bolt from the heavens strikes your foot!\n\r"
         "Ow!  Ow!  Hotfoot!!!",
         ch,
         NULL,
         victim,
         TO_VICT);
      act(
         "You smite $N on the foot!",
         ch,
         NULL,
         victim,
         TO_CHAR);
   }
   else if (!str_cmp(arg2, "head"))
   {
      act(
         "A bolt from the heavens strikes the back of $n's head!",
         victim,
         NULL,
         ch,
         TO_NOTVICT);
      act(
         "A bolt from the heavens strikes the back of your head!",
         ch,
         NULL,
         victim,
         TO_VICT);
      act(
         "You smite $N in the back of the head!",
         ch,
         NULL,
         victim,
         TO_CHAR);
   }
   else if (arg2[0] == '\0')
   {
      act(
         "A bolt from the heavens smites $n!",
         victim,
         NULL,
         ch,
         TO_NOTVICT);
      act(
         "A bolt from the heavens smites you!",
         ch,
         NULL,
         victim,
         TO_VICT);
      act("You smite $N!", ch, NULL, victim, TO_CHAR);
   }
   else
   {
      send_to_char("Valid targets are: foot head\n\r", ch);
   }

   victim->hit /= 2;
   smite_effect.where     = TO_AFFECTS;
   smite_effect.type      = gsn_corrupt("smite", &gsn_smite);
   smite_effect.duration  = 24;
   smite_effect.modifier  = 0;
   smite_effect.location  = 0;
   smite_effect.level     = ch->level;
   smite_effect.bitvector = AFF_CURSE;
   affect_to_char(victim, &smite_effect);
}

void do_forsake( CHAR_DATA *ch, char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   OBJ_DATA *obj, *mybrand;
   OBJ_DATA *objj;

   /*
   Level check explanation:
   Only level 55 imms may have brands,
   so even 60 trust will not allow you to use this
   */
   if (ch->level < 55 || IS_NPC(ch))
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }
   argument = one_argument( argument, arg1 );

   if ( arg1[0] == '\0' )
   {
      send_to_char("Syntax: forsake <char>\n\r", ch);
      return;
   }

   if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
   {
      send_to_char( "They aren't playing.\n\r", ch );
      return;
   }

   if ( IS_NPC(victim) )
   {
      send_to_char( "Trying to forsake a mob? Don't you think unintelligence is enough?\n\r", ch );
      return;
   }

   if (ch == victim)
   {
      send_to_char( "Trying to forsake yourself? Heh heh...\n\r", ch );
      return;
   }

   if ( (mybrand = get_eq_char(ch, WEAR_BRAND) ) == NULL )
   {
      send_to_char("Only gods who have religions may forsake others.\n\r", ch);
      return;
   }

   if (( (obj = get_eq_char(victim, WEAR_BRAND) ) != NULL )
   && (obj->pIndexData != mybrand->pIndexData))
   {
      send_to_char("You cannot forsake another god's chosen.\n\r", ch);
      return;
   }

   if (obj != NULL)
   {
      remove_from_rosters(victim);
      unequip_char(victim, obj);
      obj_from_char(obj);
      extract_obj(obj, FALSE);
      update_roster(victim, FALSE);
      send_to_char("Your brand is painfully ripped off!\n\r", victim);
      send_to_char("You rip the brand painfully off their skin.\n\r", ch);
   }

   objj = create_object( get_obj_index(433), 60 );
   obj_to_char( objj, victim );
   equip_char( victim, objj, WEAR_BRAND );
   send_to_char("That person is now forsaken.\n\r", ch);
   send_to_char("You scream with intense pain as you are seared, forsaken by the gods!\n\r", victim);
   damage(victim, victim, number_range(19, 23),-1, DAM_OTHER, FALSE);

   return;
}

void do_brands( CHAR_DATA *ch, char *argument ) {
   char arg1[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   OBJ_DATA *obj;
   OBJ_DATA *objj;

   /*
   Level check explanation:
   Only level 55 imms may have brands,
   so even 60 trust will not allow you to use this
   */
   if (ch->level < 55 || IS_NPC(ch))
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }
   argument = one_argument( argument, arg1 );

   if ( arg1[0] == '\0' )
   {
      send_to_char("Syntax: brands <char>\n\r", ch);
      return;
   }

   if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
   {
      send_to_char( "They aren't playing.\n\r", ch );
      return;
   }

   if ( IS_NPC(victim) )
   {
      send_to_char( "Trying to brand a mob?\n\r", ch );
      return;
   }

   if (ch == victim)
   {
      send_to_char( "Trying to brand yourself?\n\r", ch );
      return;
   }
   if ( (obj = get_eq_char(victim, WEAR_BRAND) ) != NULL )
   {
      send_to_char("That person already has a brand.\n\r", ch);
      return;
   }

   if ( (obj = get_eq_char(ch, WEAR_BRAND) ) != NULL )
   {
      remove_from_rosters(victim);
      objj = create_object(obj->pIndexData, 0);
      clone_object(obj, objj);
      obj_to_char( objj, victim );
      equip_char( victim, objj, WEAR_BRAND );

      send_to_char("That person now has your brand.\n\r", ch);
      if (obj->pIndexData->vnum == OBJ_VNUM_TWIG_BRAND)
      {
         send_to_char
         (
            "You feel a bit icky as something sticks onto you with sap.\n\r",
            victim
         );
         if (victim->pcdata->learned[gsn_hyper] < 1)
         {
            victim->pcdata->learned[gsn_hyper] = 1;
         }
         victim->pcdata->learnlvl[gsn_hyper] = skill_table[gsn_hyper].skill_level[victim->class];
         if (victim->pcdata->learned[gsn_nap] < 1)
         {
            victim->pcdata->learned[gsn_nap] = 1;
         }
         victim->pcdata->learnlvl[gsn_nap] = skill_table[gsn_nap].skill_level[victim->class];
      }
      else if
      (
         obj->pIndexData->vnum == OBJ_VNUM_DRINLINDA_BRAND &&
         (
            victim->pcdata->true_sex == SEX_MALE ||
            victim->pcdata->true_sex == SEX_NEUTRAL
         )
      )
      {
         send_to_char
         (
            "You feel a tingle on your forehead as the holy symbol etches itself there.\n\r",
            victim
         );
      }
      else if
      (
         obj->pIndexData->vnum == OBJ_VNUM_DRINLINDA_BRAND &&
         victim->pcdata->true_sex == SEX_FEMALE
      )
      {
         switch(number_range(1, 3))
         {
            case(1):
            {
               send_to_char
               (
                  "You feel a tingle on your forehead as the holy symbol etches itself there.\n\r",
                  victim
               );
               break;
            }
            case(2):
            {
               send_to_char
               (
                  "You feel a tingle on the top of your left breast as the holy symbol etches itself there.\n\r",
                  victim
               );
               break;
            }
            case(3):
            {
               send_to_char
               (
                  "You feel a tingle on the top of your right breast as the holy symbol etches itself there.\n\r",
                  victim
               );
               break;
            }
         }
      }
      else
      {
         send_to_char("You wince in agony as the hot metal brands you!\n\r", victim);
      }
      update_roster(victim, FALSE);
      return;
   }
   send_to_char("You don't have a brand yourself.\n\r", ch);
   return;
}

void do_unbrands( CHAR_DATA *ch, char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   OBJ_DATA *obj;
   bool twig = FALSE;

   /*
   Level check explanation:
   Only level 55 imms may have brands,
   so even 60 trust will not allow you to use this
   */
   if (ch->level < 55 || IS_NPC(ch))
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }
   argument = one_argument( argument, arg1 );

   if ( arg1[0] == '\0' )
   {
      send_to_char("Syntax: unbrand <char>\n\r", ch);
      return;
   }
   if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
   {
      send_to_char( "They aren't playing.\n\r", ch );
      return;
   }
   if (IS_NPC(victim))
   {
      send_to_char( "Trying to unbrand a mob?\n\r", ch );
      return;
   }
   if (ch == victim)
   {
      send_to_char( "Trying to unbrand yourself?\n\r", ch );
      return;
   }
   if ((obj = get_eq_char(victim, WEAR_BRAND)) == NULL)
   {
      send_to_char( "That person doesn't have a brand.\n\r", ch );
      return;
   }
   remove_from_rosters(victim);
   if (obj->pIndexData->vnum == OBJ_VNUM_TWIG_BRAND)
   {
      twig = TRUE;
   }
   unequip_char(victim, obj);
   obj_from_char(obj);
   extract_obj(obj, FALSE);
   update_roster(victim, FALSE);
   send_to_char("You have unbranded that person.\n\r", ch);
   if (twig)
   {
      send_to_char
      (
         "The sap on a twig seems to melt as it falls to the ground.\n\r",
         victim
      );
      if (victim->race != grn_book)
      {
         victim->pcdata->learned[gsn_hyper] = 0;
         victim->pcdata->learned[gsn_nap] = 0;
      }
   }
   else
   {
      send_to_char("Your brand is painfully removed.\n\r", victim);
   }
   return;
}


void do_instruct(CHAR_DATA *ch, char *argument)
{
   char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA *victim;
   sh_int sn=0;

   if (IS_NPC(ch)) return;
   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );

   if ( arg1[0] == '\0' || arg2[0] == '\0' )
   {
      send_to_char( "Instruct whom in what skill?\n\r", ch);
      return;
   }
   if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }
   if ( IS_NPC(victim))
   {
      send_to_char("You can't teach them anything.\n\r", ch);
      return;
   }
   if ( ch->class != victim->class ||
   ch->pcdata->special != victim->pcdata->special){
      send_to_char("Their training is too different from yours to learn your techniques.\n\r", ch);
      return;
   }
   /* skill by skill because not everything is instructable */
   if (!str_cmp(arg2, "blind fighting"))
   sn = gsn_blind_fighting;
   if (!str_cmp(arg2, "eye of the tiger"))
   sn = gsn_eye_tiger;
   if (!str_cmp(arg2, "dragon spirit"))
   sn = gsn_dragon_spirit;
   if (!str_cmp(arg2, "chi healing"))
   sn = gsn_chi_healing;
   if (!str_cmp(arg2, "dim-mak"))
   sn = gsn_dimmak;
   if (sn == 0){
      send_to_char("You can't teach that technique.\n\r", ch);
      return;
   }
   if (ch->pcdata->learned[gsn_corrupt("subrank", &gsn_subrank)] < 9)
   {
      send_to_char("You are too inexperienced to instruct in this technique.\n\r", ch);
      return;
   }
   if (victim->pcdata->learned[gsn_corrupt("subrank", &gsn_subrank)] < 9)
   {
      send_to_char("They are too inexperienced to learn the technique.\n\r", ch);
      return;
   }

   if (get_skill(ch, sn) < 100 || !has_skill(ch, sn)){
      send_to_char("You do not know the technique well enough yourself yet.\n\r", ch);
      return;
   }
   if (get_skill(victim, sn) > 0){
      send_to_char("They already know the technique.\n\r", ch);
      return;
   }
   if (victim->level < 20)
   {
      send_to_char("They are much too young to learn advanced techniques.\n\r", ch);
      return;
   }
   sprintf(buf, "You instruct %s in the art of %s.\n\r", get_longname(victim, ch), skill_table[sn].name);
   send_to_char(buf, ch);
   sprintf(buf, "You have been instructed in the art of %s.\n\r", skill_table[sn].name);
   send_to_char(buf, victim);
   sprintf(log_buf, "%s instructed %s.", ch->name, victim->name);
   log_string(log_buf);
   victim->pcdata->learnlvl[sn] = victim->level;
   victim->pcdata->learned[sn] = 1;

   return;
}

void lose_house_skills(CHAR_DATA* ch, int house)
{
   int cnt;

   cnt = 0;
   while (house_table[house].skills[cnt].name != NULL)
   {
      skill_lose(ch, house_table[house].skills[cnt].name);
      cnt++;
   }
}

void lose_all_house_skills(CHAR_DATA* ch)
{
   int house;
   int cnt;

   for (house = 0; house < MAX_HOUSE; house++)
   {
      lose_house_skills(ch, house);
   }

   /* Lose old house skills */
   cnt = 0;
   while (old_house_skills[cnt].name != NULL)
   {
      skill_lose(ch, old_house_skills[cnt].name);
      cnt++;
   }
}

void grant_house_skill(CHAR_DATA* ch, SKILL_LIST_TYPE* skill)
{
   int sn;

   sn = skill_lookup(skill->name);
   if
   (
      sn < 1 ||
      sn >= MAX_SKILL
   )
   {
      sprintf
      (
         log_buf,
         "grant_house_skill: Invalid skill [%s] for %s",
         skill->name,
         ch->name
      );
      bug_trust(log_buf, 0, get_trust(ch));
      return;
   }

   if
   (
      !skill->enabled ||
      (
         !IS_GOOD(ch) &&
         skill->alignment == ALLOW_GOOD
      ) ||
      (
         !IS_EVIL(ch) &&
         skill->alignment == ALLOW_EVIL
      ) ||
      (
         !IS_NEUTRAL(ch) &&
         skill->alignment == ALLOW_NEUTRAL
      ) ||
      (
         ch->pcdata->house_rank2 != SECT_ROGUE &&
         skill->alignment == ALLOW_ROGUE
      ) ||
      (
         ch->pcdata->house_rank2 != SECT_THUG &&
         skill->alignment == ALLOW_THUG
      ) ||
      (
         ch->pcdata->house_rank2 != SECT_TRICKSTER &&
         skill->alignment == ALLOW_TRICKSTER
      ) ||
      ch->pcdata->induct < skill->min_induct
   )
   {
      ch->pcdata->learned[sn] = -1;
   }
   else
   {
      skill_gain
      (
         ch,
         skill->name,
         skill->level
      );
      if (ch->pcdata->learned[sn] < skill->percent)
      {
         ch->pcdata->learned[sn] = skill->percent;
      }
   }
}

void grant_house_skills(CHAR_DATA* ch)
{
   int cnt;
   int house;

   /* Lose skills for all houses except own. */
   for (house = 0; house < MAX_HOUSE; house++)
   {
      if
      (
         ch->house == house ||
         (
            ch->house == 0 &&
            IS_SET(ch->act2, PLR_IS_ANCIENT) &&
            house == HOUSE_ANCIENT
         )
      )
      {
         cnt = 0;
         while (house_table[house].skills[cnt].name != NULL)
         {
            grant_house_skill(ch, &house_table[house].skills[cnt]);
            cnt++;
         }
         continue;
      }
      lose_house_skills(ch, house);
   }
}

void do_induct(CHAR_DATA* ch, char* argument)
{
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   char arg3[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];
   char buf3[MAX_STRING_LENGTH];
   char buf4[MAX_STRING_LENGTH];
   CHAR_DATA* victim;
   int house;
   int position;

   if (IS_NPC(ch))
   {
      return;
   }

   if
   (
      !(
         IS_TRUSTED(ch, DEMI) ||
         ch->pcdata->induct == 5 ||
         (
            ch->house != HOUSE_CONCLAVE &&
            ch->pcdata->induct == 3
         )
      )
   )
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }
   argument = one_argument(argument, arg1);
   argument = one_argument(argument, arg2);
   argument = one_argument(argument, arg3);

   if (arg1[0] == '\0' || arg2[0] == '\0')
   {
      if
      (
         ch->house == HOUSE_MARAUDER
      )
      {
         send_to_char("Syntax: induct <char> <house name> <sect name> (rogue thug trickster)\n\r", ch);
      }
      else
      {
         send_to_char("Syntax: induct <char> <house name>\n\r", ch);
         if
         (
            ch->house == HOUSE_CONCLAVE &&
            ch->pcdata->induct == 5
         )
         {
            send_to_char(
               "      : induct <char> Conclave emeritus\n\r"
               "      : induct <char> Conclave apprentice\n\r",
               ch);
         }
      }

      if (IS_IMMORTAL(ch))
      {
         send_to_char
         (
            "      : induct <char> <house name> emeritus\n\r"
            "      : induct <char> <house name> apprentice\n\r"
            "      : induct <char> <house name> rogue\n\r"
            "      : induct <char> <house name> thug\n\r"
            "      : induct <char> <house name> trickster\n\r",
            ch
         );
      }
      return;
   }

   if
   (
      (
         house = house_lookup(arg2)
      ) == 0 &&
      str_prefix(arg2, "none") &&
      str_prefix(arg2, "ancientknown")
   )
   {
      send_to_char("No such house exists.\n\r", ch);
      return;
   }

   if (arg3[0] != '\0')
   {
      if
      (
         !IS_IMMORTAL(ch) &&
         str_prefix(arg2, "none") &&
         str_cmp(arg2, "ancientknown") &&
         ch->house != HOUSE_MARAUDER &&   /* Marauder have sects to induct into */
         !(
            ch->house == HOUSE_CONCLAVE &&
            ch->pcdata->induct == 5 &&
            (
               !str_cmp(arg3, "emeritus") ||
               !str_cmp(arg3, "apprentice")
            )
         )
      )
      {
         do_induct(ch, "");
         return;
      }
      if (!str_prefix(arg3, "emeritus"))
      {
         position = INDUCT_EMERITUS;
      }
      else if (!str_prefix(arg3, "apprentice"))
      {
         position = INDUCT_APPRENTICE;
      }
      else if (!str_prefix(arg3, "rogue"))
      {
         position = INDUCT_ROGUE;
      }
      else if (!str_prefix(arg3, "thug"))
      {
         position = INDUCT_THUG;
      }
      else if (!str_prefix(arg3, "trickster"))
      {
         position = INDUCT_TRICKSTER;
      }
      else
      {
         do_induct(ch, "");
         return;
      }
   }
   else
   {
      position = INDUCT_NORMAL;
   }
   if ((victim = get_char_world(ch, arg1)) == NULL)
   {
      send_to_char("They are not here.\n\r", ch);
      return;
   }

   if (IS_SET(victim->comm, COMM_MORON))
   {
      send_to_char("They are not worthy of being in a house.\n\r", ch);
      return;
   }

   if (IS_NPC(victim))
   {
      send_to_char("Trying to induct a mob?\n\r", ch);
      return;
   }

   if
   (
      IS_IMMORTAL(ch) &&
      !str_cmp(arg2, "ancientknown")
   )
   {
      if (!IS_SET(victim->act2, PLR_IS_ANCIENT))
      {
         send_to_char("They are not an ancient.\n\r", ch);
         return;
      }
      skill_gain(victim, "dark focus", 10);
      send_to_char("You are now allowed to be known as ancient.\n\r", victim);
      send_to_char("You grant them permission to be known as ancient.\n\r", ch);
      SET_BIT(victim->act2, PLR_IS_ANCIENT_KNOWN);
      return;
   }
   skill_lose(victim, "dark focus");
   REMOVE_BIT(victim->act2, PLR_IS_ANCIENT_KNOWN);

   victim->pcdata->autopurge = 0;
   if (!str_prefix(arg2, "none"))
   {
      if
      (
         victim->house == 0 &&
         (
            !IS_SET(victim->act2, PLR_IS_ANCIENT) ||
            (
               ch->house != HOUSE_ANCIENT &&
               !IS_IMMORTAL(ch)
            )
         )
      )
      {
         send_to_char("They do not belong to a house.\n\r", ch);
      }
      if
      (
         ch->house != victim->house &&
         !IS_TRUSTED(ch, DEMI)
      )
      {
         send_to_char
         (
            "You cannot do that.  They are not a member of your house.\n\r",
            ch
         );
         return;
      }
      if
      (
         ch != victim &&
         !IS_IMMORTAL(victim) &&
         !IS_IMMORTAL(ch)
      )
      {
         sprintf(buf2, "Uninduction Information: %s", victim->name);
         sprintf(buf3, "%s", house_table[ch->house].induct_note_to);
         sprintf
         (
            buf4,
            "%s(%s) uninducted %s(%s) from %s.\n\r",
            ch->name,
            ch->pcdata->last_site,
            victim->name,
            victim->pcdata->last_site,
            house_table[victim->house].name
         );
         make_note("Automated uninduction note", buf2, buf3, buf4, NOTE_NOTE);
      }
      if (is_affected(victim, gsn_cloak_form))
      {
         /* uncloak */
         do_cloak(victim, "");
      }
      /* Strip leader abilities */
      REMOVE_BIT(victim->act2, PLR_IS_ANCIENT);
      REMOVE_BIT(victim->act2, PLR_NOVICE);
      REMOVE_BIT(victim->act, PLR_HONORBOUND);
      remove_from_rosters(victim);
      victim->pcdata->induct = 0;
      victim->pcdata->house_rank = 0;
      house = victim->house;
      victim->house = 0;
      if (victim->id == ID_BUK)
      {
         frog_house = victim->house;
         save_globals();
      }
      if (ch != victim)
      {
         send_to_char("They are now homeless.\n\r", ch);
      }
      if
      (
         !IS_IMMORTAL(victim) &&
         house == HOUSE_MARAUDER
      )
      {
         send_to_char("You have betrayed the Guild and Crime.\n\r", victim);
         send_to_char("Forever will you be hunted by those true to the cause.\n\r", victim);
         sprintf(buf, "%s has betrayed the Guild and Crime.", victim->name);
         do_house_guardian_talk(HOUSE_MARAUDER, buf);
         sprintf(buf, "No longer shall the Guild support the fool.");
         do_house_guardian_talk(HOUSE_MARAUDER, buf);
      }
      else
      {
         send_to_char("You are now homeless!\n\r", victim);
      }
      lose_all_house_skills(victim);
      update_roster(victim, FALSE);
      if
      (
         !IS_IMMORTAL(victim) &&
         victim->level >= 30 &&
         (
            victim->in_room == NULL ||
            !IS_SET(victim->in_room->extra_room_flags, ROOM_1212)
         ) &&
         victim->pcdata->race_lottery > 0 &&
         victim->pcdata->race_lottery < MAX_PC_RACE &&
         !victim->house &&
         !IS_SET(victim->act2, PLR_IS_ANCIENT)
      )
      {
         /* add to lottery */
         remove_node_for(victim->name, lottery_race_players[victim->pcdata->race_lottery]);
         add_node(victim->name, lottery_race_players[victim->pcdata->race_lottery]);
      }
      return;
   }

   if
   (
      house != ch->house &&
      !IS_TRUSTED(ch, DEMI)
   )
   {
      send_to_char
      (
         "You may only induct into the house which you belong.\n\r",
         ch
      );
      return;
   }
   if
   (
      house == HOUSE_MARAUDER
   )
   {
      if
      (
         position == INDUCT_ROGUE
      )
      {
         victim->pcdata->house_rank2 = SECT_ROGUE;
      }
      else if
      (
         position == INDUCT_THUG
      )
      {
         victim->pcdata->house_rank2 = SECT_THUG;
      }
      else if
      (
         position == INDUCT_TRICKSTER
      )
      {
         victim->pcdata->house_rank2 = SECT_TRICKSTER;
      }
      else if
      (
         IS_IMMORTAL(ch) &&
         position == INDUCT_APPRENTICE
      )
      {
         victim->pcdata->house_rank2 = SECT_NONE;
      }
      else
      {
         send_to_char("You must specify a sect within the Guild.\n\r", ch);
         return;
      }
   }

   if (house == HOUSE_CRUSADER)
   {
      if (IS_SET(victim->act2, PLR_IMPURE))
      {
         send_to_char("You cannot induct those who are impure.\n\r", ch);
         return;
      }
   }
   /* Covenant no goodie restriction and Covenant no mages or clerics restriction -- Drinlinda */
   if (house == HOUSE_COVENANT)
   {
      bool fail = FALSE;
      if (IS_GOOD(victim))
      {
         send_to_char
         (
            "They are too pure hearted to be a Knight of the Dark"
            " Covenant.\n\r",
            ch
         );
         send_to_char
         (
            "You are too pure hearted to be a Knight of the Dark"
            " Covenant.\n\r",
            victim
         );
         fail = TRUE;
      }
      if
      (
         victim->class == CLASS_CHANNELER ||
         victim->class == CLASS_NECROMANCER ||
         victim->class == CLASS_ELEMENTALIST
      )
      {
         send_to_char("Mages cannot be Covenant Knights.\n\r", ch);
         send_to_char
         (
            "You lack the physical prowess to be a Knight of the Dark"
            " Covenant.\n\r",
            victim
         );
         fail = TRUE;
      }
      if
      (
         victim->class == CLASS_CLERIC &&
         victim->pcdata->special != SUBCLASS_ZEALOT_FAITH
      )
      {
         send_to_char
         (
            "This cleric lacks the training of a zealot of the faith to be a"
            " Knight of the Dark Covenant.\n\r",
            ch
         );
         send_to_char
         (
            "Your cleric specialization lacks the training of a zealot of the"
            " faith to be a Knight of the Dark Covenant.\n\r",
            victim
         );
         fail = TRUE;
      }
      if
      (
         fail &&
         (
            !IS_IMMORTAL(ch) ||
            !IS_IMMORTAL(victim)
         )
      )
      {
         return;
      }
   }
   if (house_table[house].induct_on == FALSE && !IS_IMMORTAL(ch))
   {
      send_to_char
      (
         "Inductions into your house are not being allowed right now.\n\r",
         ch
      );
      return;
   }

   if (!IS_IMMORTAL(ch))
   {
      if
      (
         house_table[house].min_induct != 0 &&
         victim->level < house_table[house].min_induct
      )
      {
         send_to_char
         (
            "That person is too low of a rank to be inducted into your house.\n\r",
            ch
         );
         return;
      }
      if
      (
         house_table[house].max_induct != 0 &&
         victim->level > house_table[house].max_induct
      )
      {
         send_to_char
         (
            "That person is too high of a rank to be inducted into your house.\n\r",
            ch
         );
         return;
      }
      if (strlen(victim->pcdata->psych_desc) < 50)
      {
         send_to_char("Their psych description is inadequate.\n\r", ch);
         return;
      }
      if (strlen(victim->description) < 50)
      {
         send_to_char("Their description is inadequate.\n\r", ch);
         return;
      }
   }

   /*    if ((((int) (victim->played + current_time - victim->logon) / 3600) < 30) &&
   (!IS_IMMORTAL(ch)))
   {
   send_to_char("This person is too inexperienced in the world right now.\n\r", ch);
   send_to_char("You feel too inexperienced in the world right now to join a House.\n\r", victim);
   return;
   } */
   if
   (
      !IS_IMMORTAL(victim) &&
      !IS_IMMORTAL(ch)
   )
   {
      sprintf(buf2, "Induction Information: %s", victim->name);
      sprintf(buf3, "%s", house_table[ch->house].induct_note_to);
      sprintf
      (
         buf4,
         "%s(%s) inducted %s(%s) into %s.\n\r",
         ch->name,
         ch->pcdata->last_site,
         victim->name,
         victim->pcdata->last_site,
         house_table[ch->house].name
      );
      make_note("Automated induction note", buf2, buf3, buf4, NOTE_NOTE);
   }

   remove_from_rosters(victim);
   if
   (
      ch->pcdata->induct == 3 &&
      ch->house != HOUSE_MARAUDER
   )
   {
      if (arg3[0] != '\0')
      {
         /* no novice+apprentice/emeritus */
         do_induct(ch, "");
         update_roster(victim, FALSE);
         return;
      }
      SET_BIT(victim->act2, PLR_NOVICE);
      sprintf
      (
         buf, "%s has been noviced into %s.\n\r",
         victim->name,
         capitalize(house_table[house].name)
      );
      send_to_char(buf, ch);
      sprintf
      (
         buf,
         "You have been noviced into %s.\n\r",
         capitalize(house_table[house].name)
      );
      send_to_char(buf, victim);
      send_to_char
      (
         "You will not be granted house powers until a full leader deems you a"
         " full member.\n\r",
         victim
      );
      victim->house = house;
      update_roster(victim, FALSE);
      if (victim->id == ID_BUK)
      {
         frog_house = victim->house;
         save_globals();
      }
      victim->pcdata->house_rank = 0;
      victim->pcdata->induct = 0;
      if (is_affected(victim, gsn_cloak_form))
      {
         /* uncloak */
         do_cloak(victim, "");
      }
      lose_all_house_skills(victim);
      if
      (
         victim->pcdata->race_lottery > 0 &&
         victim->pcdata->race_lottery < MAX_PC_RACE
      )
      {
         /* remove from lottery */
         remove_node_for(victim->name, lottery_race_players[victim->pcdata->race_lottery]);
      }
      victim->pcdata->targetkills = 0;
      return;
   }
   victim->pcdata->targetkills = 0;
   if
   (
      victim->house &&
      !IS_IMMORTAL(victim)
   )
   {
      lose_all_house_skills(victim);
   }
   victim->house = house;
   if
   (
      victim->pcdata->race_lottery > 0 &&
      victim->pcdata->race_lottery < MAX_PC_RACE
   )
   {
      /* remove from lottery */
      remove_node_for(victim->name, lottery_race_players[victim->pcdata->race_lottery]);
   }
   if (victim->id == ID_BUK)
   {
      frog_house = victim->house;
      save_globals();
   }

   if (house != HOUSE_MARAUDER)
   {
      victim->pcdata->induct = position;
   }
   else
   {
      victim->pcdata->induct = RANK_APPRENTICE;
   }
   victim->pcdata->house_rank = 0;
   update_roster(victim, FALSE);

   if (house == HOUSE_ANCIENT)
   {
      SET_BIT(victim->act2, PLR_IS_ANCIENT);
   }
   else
   {
      REMOVE_BIT(victim->act2, PLR_IS_ANCIENT);
   }
   REMOVE_BIT(victim->act2, PLR_NOVICE);

   switch (position)
   {
      default:
      {
         sprintf
         (
            buf,
            "%s has been inducted into %s.\n\r",
            victim->name, capitalize(house_table[house].name)
         );
         break;
      }
      case (INDUCT_EMERITUS):
      {
         sprintf
         (
            buf,
            "%s has been retired from %s.\n\r",
            victim->name, capitalize(house_table[house].name)
         );
         break;
      }
      case (INDUCT_APPRENTICE):
      {
         sprintf
         (
            buf,
            "%s has been inducted as an apprentice of %s.\n\r",
            victim->name, capitalize(house_table[house].name)
         );
         break;
      }
   }
   send_to_char(buf, ch);
   switch (position)
   {
      default:
      {
         sprintf
         (
            buf,
            "You have been inducted into %s.\n\r",
            capitalize(house_table[house].name)
         );
         break;
      }
      case (INDUCT_EMERITUS):
      {
         sprintf
         (
            buf,
            "You have retired from %s.\n\r",
            capitalize(house_table[house].name)
         );
         break;
      }
      case (INDUCT_APPRENTICE):
      {
         sprintf
         (
            buf,
            "You have been inducted as an apprentice into %s.\n\r",
            capitalize(house_table[house].name)
         );
         break;
      }
   }
   send_to_char(buf, victim);

   grant_house_skills(victim);

   if
   (
      house == HOUSE_MARAUDER &&
      !IS_IMMORTAL(victim)
   )
   {
      sprintf(buf, "%s belongs to Crime now.", victim->name);
      do_house_guardian_talk(HOUSE_MARAUDER, buf);
      send_to_char("Crime has called you, the Guild opens itself to you.\n\r", victim);
      send_to_char("You answer the call and the secrets of the Guild flow within your reach.\n\r", victim);
      act("You welcome $N into the Guild.", ch, NULL, victim, TO_CHAR);
   }

   switch (position)
   {
      case (INDUCT_APPRENTICE):
      case (INDUCT_EMERITUS):
      {
         break;
      }
      default:
      {
         switch (house)
         {
            default:
            {
               break;
            }
            case (HOUSE_CRUSADER):
            {
               act
               (
                  "$n slices an arm neatly and allows some blood to dribble to the ground.",
                  victim,
                  NULL,
                  NULL,
                  TO_ROOM
               );
               act
               (
                  "You slice your arm neatly and allow some blood to dribble to the ground.",
                  victim,
                  NULL,
                  NULL,
                  TO_CHAR
               );
               damage(victim, victim, number_range(19, 23), -1, DAM_OTHER, FALSE);
               act
               (
                  "$n swears a blood oath, rooted as firmly as the freshly blooded earth.",
                  victim,
                  NULL,
                  NULL,
                  TO_ROOM
               );
               act
               (
                  "You swear a blood oath, rooted as firmly as the earth your blood has touched.",
                  victim,
                  NULL,
                  NULL,
                  TO_CHAR
               );
               break;
            }
            case (HOUSE_COVENANT):
            {
               act
               (
                  "$n kneels and swears fealty to the Covenant.\n\r"
                  "A disembodied hand appears in the air, wielding a sword.\n\r"
                  "The hand taps $n on both shoulders with the sword and disappears.",
                  victim,
                  NULL,
                  NULL,
                  TO_ROOM
               );
               send_to_char
               (
                  "You kneel and swear fealty to Covenant.\n\r"
                  "A voice whispers in your ear, \"Fail us and you will face a crossbow squad\".\n\r"
                  "A disembodied hand appears in the air, wielding a sword.\n\r"
                  "The hand taps you on both shoulders with the sword and disappears.\n\r",
                  victim
               );
            /*   if
               (
                  victim->class = CLASS_WARRIOR ||
                  victim->class = CLASS_ANTI_PALADIN
               )
               {
                  send_to_char
                  (
                     "You are part of the Infantry.  Your division symbol is a pair of crossed"
                     " swords.  Wear it proudly.\n\r",
                     victim
                  );
               }
               if
               (
                  victim->class = CLASS_MONK ||
                  victim->class = CLASS_NIGHTWALKER ||
                  victim->class = CLASS_THIEF
               )
               {
                  send_to_char
                  (
                     "You are part of the Covert Operations.  Your division symbol is a"
                     " hooded skull.  Wear it proudly.\n\r",
                     victim
                  );
               }
               if
               (
                  victim->class = CLASS_RANGER
               )
               {
                  send_to_char
                  (
                     "You are part of the Pathfinders.  Your division symbol is a"
                     " flame wreathed axe.  Wear it proudly.\n\r",
                     victim
                  );
               }
               if
               (
                  victim->class = CLASS_CLERIC
               )
               {
                  send_to_char
                  (
                     "You are part of the Chaplain Corps.  Your division symbol is a"
                     " glowing mace.  Wear it proudly.\n\r",
                     victim
                  );
               } */
               sprintf
               (
                  buf,
                  "%s Squire",
                  victim->name
               );
               do_promote(ch, buf);
               break;
            }
            case (HOUSE_ANCIENT):
            {
               act
               (
                  "A phantom dagger appears in the air and drives itself into $n's chest!",
                  victim,
                  NULL,
                  NULL,
                  TO_ROOM
               );
               act
               (
                  "A phantom dagger appears in the air and drives itself into your chest!",
                  victim,
                  NULL,
                  NULL,
                  TO_CHAR
               );
               damage(victim, victim, number_range(19, 23), -1, DAM_OTHER, FALSE);
               act
               (
                  "$n's lifeblood spills out and turns into smoke as it hits the ground.",
                  victim,
                  NULL,
                  NULL,
                  TO_ROOM
               );
               act
               (
                  "Your lifeblood spills out and turns into smoke as it hits the ground.\n\r"
                  "A voice speaks in your mind: \"You now belong to Ancient.  Fail and"
                  " be destroyed.\"",
                  victim,
                  NULL,
                  NULL,
                  TO_CHAR
               );
               act
               (
                  "",
                  victim,
                  NULL,
                  NULL,
                  TO_CHAR
               );
               break;
            }
         }
         break;
      }
   }
   if
   (
      IS_IMMORTAL(victim) &&
      !IS_SET(victim->act2, PLR_IS_ANCIENT_KNOWN) &&
      victim->house == HOUSE_ANCIENT
   )
   {
      SET_BIT(victim->act2, PLR_IS_ANCIENT_KNOWN);
   }
   if
   (
      !IS_IMMORTAL(victim) &&
      !IS_SET(victim->act2, PLR_IS_ANCIENT_KNOWN) &&
      victim->house == HOUSE_ANCIENT
   )
   {
      victim->house = 0;  /* PLR_IS_ANCIENT is all that is used */
   }
}

/* equips a character */
void do_outfit ( CHAR_DATA *ch, char *argument )
{
   OBJ_DATA *obj;
   int i, sn, vnum;

   if (ch->level > 5 || IS_NPC(ch))
   {
      send_to_char("Find it yourself!\n\r", ch);
      return;
   }

   if ( ( obj = get_eq_char( ch, WEAR_LIGHT ) ) == NULL )
   {
      obj = create_object( get_obj_index(OBJ_VNUM_SCHOOL_BANNER), 0 );
      obj->cost = 0;
      obj_to_char( obj, ch );
      wear_obj( ch, obj, FALSE );
   }

   if (ch->class != GUILD_MONK -1){

      if ( ( obj = get_eq_char( ch, WEAR_BODY ) ) == NULL )
      {
         /*    if (str_cmp(race_table[ch->race].name, "changeling"))*/
         {
            obj = create_object( get_obj_index(OBJ_VNUM_SCHOOL_VEST), 0 );
            obj->cost = 0;
            obj_to_char( obj, ch );
            wear_obj( ch, obj, FALSE );
         }
      }

      /* do the weapon thing */
      if ((obj = get_eq_char(ch, WEAR_WIELD)) == NULL)
      {
         sn = 0;
         vnum = OBJ_VNUM_SCHOOL_SWORD; /* just in case! */

         for (i = 0; weapon_table[i].name != NULL; i++)
         {
            if (ch->pcdata->learned[sn] <
            ch->pcdata->learned[*weapon_table[i].gsn])
            {
               if (*weapon_table[i].gsn != gsn_archery){
                  sn = *weapon_table[i].gsn;
                  vnum = weapon_table[i].vnum;
               }
            }
         }

         obj = create_object(get_obj_index(vnum), 0);
         obj_to_char(obj, ch);
         wear_obj(ch, obj, FALSE);
      }

      if (((obj = get_eq_char(ch, WEAR_WIELD)) == NULL
      ||   !IS_WEAPON_STAT(obj, WEAPON_TWO_HANDS))
      &&  (obj = get_eq_char( ch, WEAR_SHIELD ) ) == NULL )
      {
         obj = create_object( get_obj_index(OBJ_VNUM_SCHOOL_SHIELD), 0 );
         obj->cost = 0;
         obj_to_char( obj, ch );
         wear_obj( ch, obj, FALSE );
      }
   }else /* eq monks differently -Wervdon */{
      /* eventually load them with a robe and a belt instead of weapon and vest */
      obj = create_object(get_obj_index(OBJ_VNUM_SCHOOL_ROBE), 0);
      obj->cost = 0;
      obj_to_char( obj, ch);
      wear_obj(ch, obj, FALSE);
      obj = create_object(get_obj_index(OBJ_VNUM_SCHOOL_BELT), 0);
      obj->cost = 0;
      obj_to_char(obj, ch);
      wear_obj(ch, obj, FALSE);
   }
   obj = create_object(get_obj_index(OBJ_VNUM_MAP), 0);
   if (obj != NULL)
   {
      obj->cost = 0;
      obj_to_char(obj, ch);
   }
   obj = create_object(get_obj_index(OBJ_VNUM_MAP_THERA), 0);
   {
      obj->cost = 0;
      obj_to_char(obj, ch);
   }
   obj = create_object(get_obj_index(10688), 0);
   {
      obj->cost = 0;
      obj_to_char(obj, ch);
   }

   /*
   obj = create_object(get_obj_index(OBJ_VNUM_MAP_N_THERA), 0);
   obj->cost = 0;
   obj_to_char(obj, ch);
   obj = create_object(get_obj_index(OBJ_VNUM_MAP_E_THERA), 0);
   obj->cost = 0;
   obj_to_char(obj, ch);
   obj = create_object(get_obj_index(OBJ_VNUM_MAP_W_THERA), 0);
   obj->cost = 0;
   obj_to_char(obj, ch);
   */
   send_to_char("You have been equipped by the gods.\n\r", ch);
}

/* moron flag code - serving idiots since 1997 - runge */
void do_moron( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
   CHAR_DATA *victim;
   one_argument( argument, arg );
   if ( arg[0] == '\0' )
   {
      send_to_char("Brand whom a moron?\n\r", ch);
      return;
   }

   if ( ( victim = get_char_world( ch, arg ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if ( get_trust( victim ) >= get_trust( ch ) )
   {
      send_to_char( "You failed.\n\r", ch );
      return;
   }

   if ( IS_SET(victim->comm, COMM_MORON) )
   {
      REMOVE_BIT(victim->comm, COMM_MORON);
      send_to_char( "MORON status removed.\n\r", ch );
      sprintf(buf, "$N removes MORON status from %s", victim->name);
      wiznet(buf, ch, NULL, WIZ_PENALTIES, 0, 0);
   }
   else
   {
      SET_BIT(victim->comm, COMM_MORON);
      send_to_char( "MORON status set.\n\r", ch );
      sprintf(buf, "$N brands %s as a MORON.", victim->name);
      wiznet(buf, ch, NULL, WIZ_PENALTIES, 0, 0);
   }

   return;
}

/* RT nochannels command, for those spammers */
void do_nochannels( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
   CHAR_DATA *victim;

   one_argument( argument, arg );

   if ( arg[0] == '\0' )
   {
      send_to_char( "Nochannel whom?", ch );
      return;
   }
   if ( ( victim = get_char_world( ch, arg ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if ( get_trust( victim ) >= get_trust( ch ) )
   {
      send_to_char( "You failed.\n\r", ch );
      return;
   }

   if ( IS_SET(victim->comm, COMM_NOCHANNELS) )
   {
      REMOVE_BIT(victim->comm, COMM_NOCHANNELS);
      send_to_char( "The gods have restored your channel priviliges.\n\r",
      victim );
      send_to_char( "NOCHANNELS removed.\n\r", ch );
      sprintf(buf, "$N restores channels to %s", victim->name);
      wiznet(buf, ch, NULL, WIZ_PENALTIES, 0, 0);
   }
   else
   {
      SET_BIT(victim->comm, COMM_NOCHANNELS);
      send_to_char( "The gods have revoked your channel priviliges.\n\r",
      victim );
      send_to_char( "NOCHANNELS set.\n\r", ch );
      sprintf(buf, "$N revokes %s's channels.", victim->name);
      wiznet(buf, ch, NULL, WIZ_PENALTIES, 0, 0);
   }

   return;
}



void do_smote(CHAR_DATA *ch, char *argument )
{
   CHAR_DATA *vch;
   char *letter,*name;
   char last[MAX_INPUT_LENGTH], temp[MAX_STRING_LENGTH];
   unsigned int matches = 0;

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

   if (strstr(argument, ch->name) == NULL)
   {
      send_to_char("You must include your name in an smote.\n\r", ch);
      return;
   }

   send_to_char(argument, ch);
   send_to_char("\n\r", ch);

   for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
   {
      if (vch->desc == NULL || vch == ch)
      continue;

      if ((letter = strstr(argument, vch->name)) == NULL)
      {
         send_to_char(argument, vch);
         send_to_char("\n\r", vch);
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

      send_to_char(temp, vch);
      send_to_char("\n\r", vch);
   }

   return;
}

void do_bamfin( CHAR_DATA *ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];

   if ( !IS_NPC(ch) )
   {
      smash_tilde( argument );

      if (argument[0] == '\0')
      {
         sprintf(buf, "Your poofin is %s\n\r", ch->pcdata->bamfin);
         send_to_char(buf, ch);
         return;
      }

      if ( strstr(argument, ch->name) == NULL)
      {
         send_to_char("You must include your name.\n\r", ch);
         return;
      }

      free_string( ch->pcdata->bamfin );
      ch->pcdata->bamfin = str_dup( argument );

      sprintf(buf, "Your poofin is now %s\n\r", ch->pcdata->bamfin);
      send_to_char(buf, ch);
   }
   return;
}



void do_bamfout( CHAR_DATA *ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];

   if ( !IS_NPC(ch) )
   {
      smash_tilde( argument );

      if (argument[0] == '\0')
      {
         sprintf(buf, "Your poofout is %s\n\r", ch->pcdata->bamfout);
         send_to_char(buf, ch);
         return;
      }

      if ( strstr(argument, ch->name) == NULL)
      {
         send_to_char("You must include your name.\n\r", ch);
         return;
      }

      free_string( ch->pcdata->bamfout );
      ch->pcdata->bamfout = str_dup( argument );

      sprintf(buf, "Your poofout is now %s\n\r", ch->pcdata->bamfout);
      send_to_char(buf, ch);
   }
   return;
}



void do_deny( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
   CHAR_DATA *victim;
   OBJ_DATA *obj;

   one_argument( argument, arg );
   if ( arg[0] == '\0' )
   {
      send_to_char( "Deny whom?\n\r", ch );
      return;
   }

   if ( ( victim = get_char_world( ch, arg ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if ( IS_NPC(victim) )
   {
      send_to_char( "Not on NPC's.\n\r", ch );
      return;
   }

   if ( get_trust( victim ) >= get_trust( ch ) )
   {
      send_to_char( "You failed.\n\r", ch );
      return;
   }

   while ((obj = victim->carrying) != NULL)
   {
      obj_from_char( obj );
      extract_obj( obj, FALSE );
   }
   victim->quittime = 0;
   victim->pause = 0;

   SET_BIT(victim->act, PLR_DENY);
   send_to_char( "You are denied access!\n\r", victim );
   sprintf(buf, "$N denies access to %s", victim->name);
   wiznet(buf, ch, NULL, WIZ_PENALTIES, 0, 0);
   send_to_char( "OK.\n\r", ch );
   save_char_obj(victim);
   stop_fighting(victim, TRUE);
   do_quit( victim, "" );

   return;
}

void do_badname(CHAR_DATA *ch, char *argument)
{
   int sock_n;
   char arg[MAX_INPUT_LENGTH];
   DESCRIPTOR_DATA *d;
   DESCRIPTOR_DATA *d_save;


   argument = one_argument( argument, arg );
   if ( arg[0] == '\0' )
   {
      send_to_char("Which name is bad?\n\r", ch);
      return;
   }
   d_save = NULL;
   sock_n = atoi(arg);
   for ( d = descriptor_list; d != NULL; d = d->next )
   {
      if ( d->character != NULL &&
      !str_cmp(d->character->name, arg) )
      {
         d_save = d;
      }
   }
   if (d_save != NULL)
   {
      if (d_save->character->level > 1)
      {
         send_to_char("This can only be used on those below level 2.\n\r", ch);
         return;
      }
      sprintf(arg, "Your name is not acceptable on Darkmists. This is a role-playing mud, and thus we attempt to maintain a certain atmosphere.\n\rPlease read our greeting screen and select a new name.\n\r");
      write_to_buffer( d_save, arg, strlen(arg) );
      close_socket( d_save );
      send_to_char( "Ok.\n\r", ch );
      return;
   }else
   send_to_char("Character not found.\n\r", ch);
   return;
}


void do_sockwrite(CHAR_DATA *ch, char *argument)
{
   int sock_n;
   char arg[MAX_INPUT_LENGTH];
   DESCRIPTOR_DATA *d;
   DESCRIPTOR_DATA *d_save;


   argument = one_argument( argument, arg );
   if ( arg[0] == '\0' )
   {
      send_to_char( "Write to which socket?\n\r", ch );
      return;
   }
   if (argument[0] == '\0')
   {
      send_to_char("Write what to the socket?\n\r", ch);
      return;
   }

   d_save = NULL;
   sock_n = atoi(arg);
   for ( d = descriptor_list; d != NULL; d = d->next )
   {
      if ( d->descriptor == sock_n )
      {
         d_save = d;
      }
   }
   if (d_save != NULL)
   {
      write_to_buffer( d_save, argument, strlen(argument));
      write_to_buffer( d_save, "\n\r", 3);
      send_to_char("Text Written to the socket.\n\r", ch);
   }else
   send_to_char("Socket not in use.\n\r", ch);
   return;
}



void do_disconnect( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   DESCRIPTOR_DATA *d;
   CHAR_DATA *victim;

   one_argument( argument, arg );
   if ( arg[0] == '\0' )
   {
      send_to_char( "Disconnect whom?\n\r", ch );
      return;
   }
   if (is_number(arg))
   {
      int desc;

      desc = atoi(arg);
      for ( d = descriptor_list; d != NULL; d = d->next )
      {
         if ( d->descriptor == desc )
         {
            close_socket( d );
            send_to_char( "Ok.\n\r", ch );
            return;
         }
      }
   }

   if ( ( victim = get_char_world( ch, arg ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if ( victim->desc == NULL )
   {
      act( "$N doesn't have a descriptor.", ch, NULL, victim, TO_CHAR );
      return;
   }

   for ( d = descriptor_list; d != NULL; d = d->next )
   {
      if ( d == victim->desc )
      {
         close_socket( d );
         send_to_char( "Ok.\n\r", ch );
         return;
      }
   }

   bug( "Do_disconnect: desc not found.", 0 );
   send_to_char( "Descriptor not found!\n\r", ch );
   return;
}



void do_pardon( CHAR_DATA *ch, char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );

   if ( arg1[0] == '\0' || arg2[0] == '\0' )
   {
      send_to_char( "Syntax: pardon <character> <killer|thief>.\n\r", ch );
      return;
   }

   if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if ( IS_NPC(victim) )
   {
      send_to_char( "Not on NPC's.\n\r", ch );
      return;
   }

   if ( !str_cmp( arg2, "killer" ) )
   {
      if ( IS_SET(victim->act, PLR_KILLER) )
      {
         REMOVE_BIT( victim->act, PLR_KILLER );
         send_to_char( "Killer flag removed.\n\r", ch );
         send_to_char( "You are no longer a KILLER.\n\r", victim );
      }
      return;
   }

   if ( !str_cmp( arg2, "thief" ) )
   {
      if ( IS_SET(victim->act, PLR_THIEF) )
      {
         REMOVE_BIT( victim->act, PLR_THIEF );
         send_to_char( "Thief flag removed.\n\r", ch );
         send_to_char( "You are no longer a THIEF.\n\r", victim );
      }
      return;
   }

   send_to_char( "Syntax: pardon <character> <killer|thief>.\n\r", ch );
   return;
}

char * convert_color( char * strbuf, bool ISANSI )
{
   char        buf[MAX_STRING_LENGTH];
   char        buf2[128];
   const char  *str;
   char        *i;
   char        *point;
   bool        flash=FALSE, o_flash,
   bold=FALSE, o_bold;
   bool        act=FALSE, color_code=FALSE;
   int         color=7, o_color;

   point=buf;
   str = strbuf;
   o_color=color;
   o_bold=bold;
   o_flash=flash;

   while ( (*str != '\0') && (str-strbuf < MAX_STRING_LENGTH-32) )
   {
      if ( *str != '{' )
      {
         color_code=FALSE;
         *point++ = *str++;
         continue;
      }

      if ( !color_code && *(str+1)!='<' )
      {
         o_color=color;
         o_bold=bold;
         o_flash=flash;
      }
      color_code=TRUE;

      act=FALSE;
      str++;
      switch ( *str )
      {
         default:    sprintf( buf2, "{%c", *str );                  break;
         case '{':
         case 'x':   sprintf( buf2, "{" );                          break;
         case 'e':   sprintf( buf2, "\n\r");                        break;
         case '-':   sprintf( buf2, "~" );                          break;
         case '<':   color=o_color; bold=o_bold; flash=o_flash;
         act=TRUE; break;
         case '0':   color=0;                             act=TRUE; break;
         case '1':   color=1;                             act=TRUE; break;
         case '2':   color=2;                             act=TRUE; break;
         case '3':   color=3;                             act=TRUE; break;
         case '4':   color=4;                             act=TRUE; break;
         case '5':   color=5;                             act=TRUE; break;
         case '6':   color=6;                             act=TRUE; break;
         case '7':   color=7;                             act=TRUE; break;
         case 'B':   bold=TRUE;                           act=TRUE; break;
         case 'b':   bold=FALSE;                          act=TRUE; break;
         case 'F':   flash=TRUE;                          act=TRUE; break;
         case 'f':   flash=FALSE;                         act=TRUE; break;
         case 'n':   if ( ISANSI )
         sprintf( buf2, "%s", ANSI_NORMAL );
         else
         buf2[0]='\0';
         bold=FALSE; color=7; flash=FALSE;       break;
      }
      if ( act )
      {
         if ( ISANSI )
         {
            sprintf( buf2, "%s", color_value_string( color, bold, flash ) );
            color_code=TRUE;
         }
         else
         buf2[0]='\0';
      }

      i=buf2;
      str++;
      while ( ( *point = *i ) != '\0' )
      ++point, ++i;
   }

   *point++='\0';
   if (ISANSI) sprintf(buf, "%s%s", buf, ANSI_NORMAL);

   return str_dup(buf);
}

void do_echo( CHAR_DATA *ch, char *argument )
{
   DESCRIPTOR_DATA *d;
   char * buf;

   if ( argument[0] == '\0' )
   {
      send_to_char( "Global echo what?\n\r", ch );
      return;
   }

   for ( d = descriptor_list; d; d = d->next )
   {
      if ( d->connected == CON_PLAYING )
      {
         if
         (
            get_trust(d->character) >= get_trust(ch) &&
            get_trust(d->character) > LEVEL_HERO
         )
         send_to_char( "global> ", d->character);
         buf = convert_color(argument, IS_ANSI(d->character));
         send_to_char(buf, d->character );
         free_string(buf);
         send_to_char( "\n\r",   d->character );
      }
   }

   return;
}



void do_recho( CHAR_DATA *ch, char *argument )
{
   DESCRIPTOR_DATA *d;
   char * buf;

   if ( argument[0] == '\0' )
   {
      send_to_char( "Local echo what?\n\r", ch );

      return;
   }

   for ( d = descriptor_list; d; d = d->next )
   {
      if ( d->connected == CON_PLAYING
      &&   d->character->in_room == ch->in_room )
      {
         if (get_trust(d->character) >= get_trust(ch) &&
         get_trust(d->character) > LEVEL_HERO)
         send_to_char( "local> ", d->character);
         buf = convert_color(argument, IS_ANSI(d->character));
         send_to_char( buf, d->character );
         free_string(buf);
         send_to_char( "\n\r",   d->character );
      }
   }

   return;
}

void do_zecho(CHAR_DATA *ch, char *argument)
{
   DESCRIPTOR_DATA *d;
   char * buf;

   if (argument[0] == '\0')
   {
      send_to_char("Zone echo what?\n\r", ch);
      return;
   }

   for (d = descriptor_list; d; d = d->next)
   {
      if (d->connected == CON_PLAYING
      &&  d->character->in_room != NULL && ch->in_room != NULL
      &&  d->character->in_room->area == ch->in_room->area)
      {
         if
         (
            get_trust(d->character) >= get_trust(ch) &&
            get_trust(d->character) > LEVEL_HERO
         )
         send_to_char("zone> ", d->character);
         buf = convert_color(argument, IS_ANSI(d->character));
         send_to_char(buf, d->character);
         free_string(buf);
         send_to_char("\n\r", d->character);
      }
   }
}

void do_pecho( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   char * buf;
   CHAR_DATA *victim;

   argument = one_argument(argument, arg);

   if ( argument[0] == '\0' || arg[0] == '\0' )
   {
      send_to_char("Personal echo what?\n\r", ch);
      return;
   }

   if  ( (victim = get_char_world(ch, arg) ) == NULL )
   {
      send_to_char("Target not found.\n\r", ch);
      return;
   }

   if (get_trust(victim) >= get_trust(ch) && get_trust(ch) != MAX_LEVEL)
   send_to_char( "personal> ", victim);

   buf = convert_color(argument, IS_ANSI(victim));
   send_to_char(buf, victim);
   send_to_char("\n\r", victim);
   free_string(buf);
   buf = convert_color(argument, IS_ANSI(ch));
   send_to_char( "personal> ", ch);
   send_to_char(buf, ch);
   send_to_char("\n\r", ch);
   free_string(buf);
}

void do_hecho(CHAR_DATA *ch, char *argument)
{
   /* ability to echo all of your house -wervdon */
   char * buf;
   DESCRIPTOR_DATA *d;

   if (!is_house(ch) || house_table[ch->house].independent){
      send_to_char("You aren't in a house.\n\r", ch);
      return;
   }

   if ( argument[0] == '\0' )
   {
      send_to_char("House echo what?\n\r", ch);
      return;
   }

   for ( d = descriptor_list; d; d = d->next )
   {
      if ( d->connected == CON_PLAYING )
      {
         if
         (
            ch->house == d->character->house &&
            (
               IS_NPC(d->character) ||
               d->character->pcdata->induct > INDUCT_APPRENTICE ||
               IS_IMMORTAL(d->character)
            )
         )
         {
            if
            (
               get_trust(d->character) >= get_trust(ch) &&
               get_trust(d->character) > LEVEL_HERO
            )
            send_to_char( "House> ", d->character);
            buf = convert_color(argument, IS_ANSI(d->character));
            send_to_char(buf, d->character );
            free_string(buf);
            send_to_char( "\n\r",   d->character );
         }
      }
   }
   return;
}

void do_wecho(CHAR_DATA* ch, char* argument)
{
   /* ability to echo all of your worshippers - Fizz */
   char* buf;
   char* worship;
   DESCRIPTOR_DATA* d;
   CHAR_DATA* vch;
   sh_int min_trust = LEVEL_IMMORTAL;

   if
   (
      !IS_IMMORTAL(ch) &&
      IS_NPC(ch)
   )
   {
      return;
   }
   if (IS_NPC(ch))
   {
      worship = ch->desc->original->name;
      min_trust = get_trust(ch);
   }
   else if (IS_IMMORTAL(ch))
   {
      min_trust = get_trust(ch);
      worship = ch->name;
   }
   else
   {
      worship = ch->pcdata->worship;
   }
   if (worship == NULL)
   {
      return;
   }
   if (argument[0] == '\0')
   {
      send_to_char("Worship echo what?\n\r", ch);
      return;
   }

   for (d = descriptor_list; d; d = d->next)
   {
      if (d->connected != CON_PLAYING)
      {
         continue;
      }
      vch = d->original ? d->original : d->character;
      if
      (
         (
            IS_IMMORTAL(vch) &&
            !strcmp(worship, vch->name)
         ) ||
         (
            !IS_IMMORTAL(vch) &&
            vch->pcdata->worship != NULL &&
            !strcmp(worship, vch->pcdata->worship)
         )
      )
      {
         if (get_trust(vch) >= min_trust)
         {
            send_to_char("Worship> ", d->character);
         }
         buf = convert_color(argument, IS_ANSI(d->character));
         send_to_char(buf, d->character);
         free_string(buf);
         send_to_char("\n\r", d->character);
      }
   }
   return;
}

void do_clecho(CHAR_DATA* ch, char* argument)
{
   /* ability to echo all of your clan - Fizz */
   char* buf;
   sh_int clan = 0;
   DESCRIPTOR_DATA* d;
   CHAR_DATA* vch;
   sh_int min_trust = LEVEL_IMMORTAL;

   if
   (
      !IS_IMMORTAL(ch) &&
      IS_NPC(ch)
   )
   {
      return;
   }
   if (IS_NPC(ch))
   {
      clan = ch->desc->original->pcdata->clan;
      min_trust = get_trust(ch);
   }
   else if (IS_IMMORTAL(ch))
   {
      min_trust = get_trust(ch);
      clan = ch->pcdata->clan;
   }
   else
   {
      clan = ch->pcdata->clan;
   }
   if (clan == 0)
   {
      return;
   }
   if (argument[0] == '\0')
   {
      send_to_char("Clan echo what?\n\r", ch);
      return;
   }

   for (d = descriptor_list; d; d = d->next)
   {
      if (d->connected != CON_PLAYING)
      {
         continue;
      }
      vch = d->original ? d->original : d->character;
      if (vch->pcdata->clan == clan)
      {
         if (get_trust(vch) >= min_trust)
         {
            send_to_char("Clan> ", d->character);
         }
         buf = convert_color(argument, IS_ANSI(d->character));
         send_to_char(buf, d->character);
         free_string(buf);
         send_to_char("\n\r", d->character);
      }
   }
   return;
}

void do_cecho(CHAR_DATA *ch, char *argument)
{
   /* ability to echo all of your house -wervdon */
   char * buf;
   DESCRIPTOR_DATA *d;

   if ( ch->conference == 0 )
   {
      send_to_char("You aren't in a conference.\n\r", ch);
      return;
   }

   if ( argument[0] == '\0' )
   {
      send_to_char("Conference echo what?\n\r", ch);
      return;
   }

   for ( d = descriptor_list; d; d = d->next )
   {
      if ( d->connected == CON_PLAYING )
      {
         if (ch->conference == d->character->conference)
         {
            if (get_trust(d->character) >= get_trust(ch))
            send_to_char( "Conference> ", d->character);
            buf = convert_color(argument, IS_ANSI(d->character));
            send_to_char(buf, d->character );
            free_string(buf);
            send_to_char( "\n\r",   d->character );
         }
      }
   }
   return;
}

void do_brandtalk(CHAR_DATA *ch, char *argument)
{
   char name[MAX_INPUT_LENGTH];
   char cloak_name[MAX_INPUT_LENGTH];
   char words[MAX_INPUT_LENGTH];
   char buf_plain[MAX_STRING_LENGTH];
   char buf_color[MAX_STRING_LENGTH];
   char buf_reduced[MAX_STRING_LENGTH];
   char buf_plain_cloak[MAX_STRING_LENGTH];
   char buf_color_cloak[MAX_STRING_LENGTH];
   char buf_reduced_cloak[MAX_STRING_LENGTH];
   char* brand_channel;
   char* temp;
   char brand_color[MAX_INPUT_LENGTH];
   OBJ_DATA* brand = NULL;
   DESCRIPTOR_DATA* d = NULL;
   sh_int vnum;
   sh_int god;
   bool allow_global = TRUE;
   char original[MAX_INPUT_LENGTH];
   bool cloak = FALSE;
   char* line;

   brand = get_eq_char(ch, WEAR_BRAND);
   /*
   Can't use brand talk if you're
   not branded, your brand does not fit any god,
   or the god disabled his/her/its brand
   */
   if
   (
      brand == NULL ||
      (
         god = god_lookup(ch)
      ) == -1 ||
      brand->pIndexData->vnum != worship_table[god].vnum_brand ||
      (
         !IS_IMMORTAL(ch) &&
         worship_table[god].brand_info.bt_disable
      )
   )
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }
   vnum = brand->pIndexData->vnum;
   /*
      Should add check for arborians/illithids
      or immortals using mentally project
      with new do_talk command
      Mute never affects mentally projecting,
      so it shouldn't here
   */
   if
   (
      !is_mental(ch) &&
      is_affected(ch, gsn_mute)
   )
   {
      send_to_char
      (
         "You try to tell them something, but only manage a weak rasping"
         " noise.\n\r",
         ch
      );
      return;
   }

   if (worship_table[god].brand_info.brand_channel == NULL)
   {
      brand_channel = "BRAND";
   }
   else
   {
      brand_channel = worship_table[god].brand_info.brand_channel;
   }
   if (worship_table[god].brand_info.brand_color == NULL)
   {
      strcpy(brand_color, "\x01B[1;36m");
   }
   else
   {
      temp = convert_color
      (
         worship_table[god].brand_info.brand_color,
         TRUE
      );
      strcpy(brand_color, temp);
      /* Strip the normalizing color at the end */
      brand_color[strlen(temp) - strlen(ANSI_NORMAL)] = '\0';
      free_string(temp);
   }

   if
   (
      !IS_IMMORTAL(ch) &&
      !IS_NPC(ch) &&
      ch->pcdata->brand_rank < 0
   )
   {
      send_to_char("You cannot speak over the brand channel.\n\r", ch);
      return;
   }
   /*
   Immortals can turn their brand channel on or off.
   Mortals can only talk (when not disabled)
   */
   if (argument[0] == '\0')
   {
      if (IS_IMMORTAL(ch))
      {
         worship_table[god].brand_info.bt_disable =
         (
            !worship_table[god].brand_info.bt_disable
         );
         if (worship_table[god].brand_info.bt_disable)
         {
            send_to_char("Your brand channel is now off.\n\r", ch);
         }
         else
         {
            send_to_char("Your brand channel is now on.\n\r", ch);
         }
         save_globals();
         return;
      }
      else
      {
         send_to_char("You have to say something.\n\r", ch);
         return;
      }
   }

   strcpy(original, argument);
   argument = original;

   if (is_affected(ch, gsn_toad))
   {
      sprintf(argument, "<ribbit>");
   }

   sprintf
   (
      name,
      "%s",
      (
         IS_NPC(ch) ?
         ch->short_descr :
         ch->name
      )
   );
   sprintf
   (
      cloak_name,
      "Cloaked Figure (%s)",
      (
         IS_NPC(ch) ?
         ch->short_descr :
         (
            ch->pcdata->moniker[0] != '\0' ?
            ch->pcdata->moniker :
            "unknown"
         )
      )
   );
   sprintf
   (
      words,
      ": %s",
      argument
   );

   sprintf
   (
      buf_color,
      "%s[%s] %s%s%s\n\r",
      brand_color,
      brand_channel,
      name,
      words,
      ANSI_NORMAL
   );
   sprintf
   (
      buf_plain,
      "[%s] %s%s\n\r",
      brand_channel,
      name,
      words
   );
   sprintf
   (
      buf_reduced,
      "[%s%s%s] %s%s\n\r",
      brand_color,
      brand_channel,
      ANSI_NORMAL,
      name,
      words
   );
   if (is_affected(ch, gsn_cloak_form))
   {
      cloak = TRUE;
      sprintf
      (
         buf_color_cloak,
         "%s[%s] %s%s%s\n\r",
         brand_color,
         brand_channel,
         cloak_name,
         words,
         ANSI_NORMAL
      );
      sprintf
      (
         buf_plain_cloak,
         "[%s] %s%s\n\r",
         brand_channel,
         cloak_name,
         words
      );
      sprintf
      (
         buf_reduced_cloak,
         "[%s%s%s] %s%s\n\r",
         brand_color,
         brand_channel,
         ANSI_NORMAL,
         cloak_name,
         words
      );
   }

   /*
      If the immortal with the brand has seebrand
      on, global brand can't be used to listen in
   */

   if (worship_table[god].brand_info.prevent_snoop)
   {
      allow_global = FALSE;
   }
   else
   {
      for (d = descriptor_list; d; d = d->next)
      {
         if
         (
            d->connected == CON_PLAYING &&
            IS_SET(d->character->act2, WIZ_SEEBRAND) &&
            IS_IMMORTAL(d->character) &&
            (
               brand = get_eq_char(d->character, WEAR_BRAND)
            ) != NULL &&
            brand->pIndexData->vnum == vnum
         )
         {
            allow_global = FALSE;
            break;
         }
      }
   }
   for (d = descriptor_list; d; d = d->next)
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
         d->connected != CON_PLAYING ||
         (
            !(
               (
                  IS_IMMORTAL(d->character) &&
                  allow_global &&
                  IS_SET(d->character->comm2, COMM_BTGLOBAL)
               ) ||
               (
                  (
                     brand = get_eq_char(d->character, WEAR_BRAND)
                  ) != NULL &&
                  brand->pIndexData->vnum == vnum
               )
            )
         )
      )
      {
         continue;
      }
      if
      (
         cloak &&
         ch != d->character &&
         !IS_TRUSTED(d->character, ANGEL)
      )
      {
         /* Send cloak */
         if (IS_SET(d->character->comm, COMM_ANSI))
         {
            if (IS_SET(d->character->comm2, COMM_REDUCED_COLOR))
            {
               line = buf_reduced_cloak;
            }
            else
            {
               line = buf_color_cloak;
            }
         }
         else
         {
            line = buf_plain_cloak;
         }
      }
      else if (IS_SET(d->character->comm, COMM_ANSI))
      {
         if (IS_SET(d->character->comm2, COMM_REDUCED_COLOR))
         {
            line = buf_reduced;
         }
         else
         {
            line = buf_color;
         }
      }
      else
      {
         line = buf_plain;
      }
      send_to_char(line, d->character);
   }
   return;
}

void do_becho(CHAR_DATA *ch, char *argument)
{
   /* ability to echo to all of your branded -wervdon */
   char * buf;
   OBJ_DATA *brand;
   DESCRIPTOR_DATA *d;
   int vnum;

   brand = get_eq_char(ch, WEAR_BRAND);

   if (brand == NULL){
      send_to_char("You don't have a brand.\n\r", ch);
      return;
   }
   vnum = brand->pIndexData->vnum;

   if ( argument[0] == '\0' )
   {
      send_to_char("Brand echo what?\n\r", ch);
      return;
   }

   for ( d = descriptor_list; d; d = d->next )
   {
      if ( d->connected == CON_PLAYING )
      {
         brand = get_eq_char(d->character, WEAR_BRAND);
         if (brand != NULL && brand->pIndexData->vnum == vnum){
            if
            (
               get_trust(d->character) >= get_trust(ch) &&
               get_trust(d->character) > LEVEL_HERO
            )
            send_to_char( "Brand> ", d->character);
            buf = convert_color(argument, IS_ANSI(d->character));
            send_to_char(buf, d->character );
            free_string(buf);
            send_to_char( "\n\r",   d->character );
         }
      }
   }
   return;
}




ROOM_INDEX_DATA *find_location( CHAR_DATA *ch, char *arg )
{
   CHAR_DATA *victim;
   OBJ_DATA *obj;

   if ( is_number(arg) )
   return get_room_index( atoi( arg ) );

   if ( ( victim = get_char_world( ch, arg ) ) != NULL )
   return victim->in_room;

   if ( ( obj = get_obj_world( ch, arg ) ) != NULL )
   return obj->in_room;

   return NULL;
}



void do_transfer( CHAR_DATA *ch, char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   ROOM_INDEX_DATA *location;
   DESCRIPTOR_DATA *d;
   CHAR_DATA *victim, *ich;

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );

   if ( arg1[0] == '\0' )
   {
      send_to_char( "Transfer whom (and where)?\n\r", ch );
      return;
   }

   if ( !str_cmp( arg1, "all" ) )
   {
      for ( d = descriptor_list; d != NULL; d = d->next )
      {
         if ( d->connected == CON_PLAYING
         &&   d->character != ch
         &&   d->character->in_room != NULL
         &&   can_see( ch, d->character ) )
         {
            char buf[MAX_STRING_LENGTH];
            sprintf( buf, "%s %s", d->character->name, arg2 );
            do_transfer( ch, buf );
         }
      }
      return;
   }

   /*
   * Thanks to Grodyn for the optional location parameter.
   */
   if ( arg2[0] == '\0' )
   {
      location = ch->in_room;
   }
   else
   {
      if ( ( location = find_location( ch, arg2 ) ) == NULL )
      {
         send_to_char( "No such location.\n\r", ch );
         return;
      }

      if ( !is_room_owner(ch, location) && room_is_private( location )
      &&  get_trust(ch) < MAX_LEVEL)
      {
         send_to_char( "That room is private right now.\n\r", ch );
         return;
      }
   }

   if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if ( victim->in_room == NULL )
   {
      send_to_char( "They are in limbo.\n\r", ch );
      return;
   }

   if (get_trust(victim) >= get_trust(ch) && !IS_NPC(victim))
   {
      send_to_char( "They are too high for you to mess with.\n\r", ch );
      return;
   }

   if ( get_trust(ch) < 55 && IS_NPC(victim) && IS_SET(victim->in_room->extra_room_flags, ROOM_1212))
   {
      send_to_char("You shouldn't be transfering them.\n\r", ch);
      return;
   }
   if
   (
      get_trust(ch) < 55 &&
      (
         location->vnum != 1212 &&
         location->vnum != 10601 &&
         !is_god_room(ch, location)
      )
   )
   {
      send_to_char("You can only send people to 1212 and 10601.\n\r", ch);
      return;
   }

   if ( victim->fighting != NULL )
   stop_fighting( victim, TRUE );
   for ( ich = victim->in_room->people; ich != NULL; ich = ich->next_in_room)
   {
      if (can_see(ich, victim) && IS_IMMORTAL(ich))
      act("$N disappears in a mushroom cloud.", ich, victim, victim, TO_CHAR);
   }
   /*    act( "$n disappears in a mushroom cloud.", victim, NULL, NULL, TO_ROOM );*/
   char_from_room( victim );
   char_to_room( victim, location );
   if
   (
      !IS_NPC(victim) &&
      !IS_IMMORTAL(victim)
   )
   {
      update_visible(victim);
   }
   for ( ich = victim->in_room->people; ich != NULL; ich = ich->next_in_room)
   {
      if (can_see(ich, victim) && IS_IMMORTAL(ich))
      act("$N arrives from a puff of smoke.", ich, victim, victim, TO_CHAR);
   }
   /*    act( "$n arrives from a puff of smoke.", victim, NULL, NULL, TO_ROOM );*/
   if ( ch != victim )
   act( "$n has transferred you.", ch, NULL, victim, TO_VICT );
   do_observe(victim, "", LOOK_AUTO);
   send_to_char( "Ok.\n\r", ch );
}



void do_at( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   ROOM_INDEX_DATA *location;
   ROOM_INDEX_DATA *original;
   OBJ_DATA *on;
   CHAR_DATA *wch;

   argument = one_argument( argument, arg );

   if ( arg[0] == '\0' )
   {
      send_to_char( "At where what?\n\r", ch );
      return;
   }

   if ( ( location = find_location( ch, arg ) ) == NULL )
   {
      send_to_char( "No such location.\n\r", ch );
      return;
   }

   if (!is_room_owner(ch, location) && room_is_private( location )
   &&  get_trust(ch) < MAX_LEVEL)
   {
      send_to_char( "That room is private right now.\n\r", ch );
      return;
   }

   original = ch->in_room;
   on = ch->on;
   char_from_room(ch);
   char_to_room_1(ch, location, TO_ROOM_AT);

   if ( argument[0] == '\0' ) /* Wicket */
   {
      do_observe(ch, "", LOOK_AUTO);
   }

   interpret( ch, argument );

   /*
   * See if 'ch' still exists before continuing!
   * Handles 'at XXXX quit' case.
   */
   for ( wch = char_list; wch != NULL; wch = wch->next )
   {
      if ( wch == ch )
      {
         char_from_room(ch);
         char_to_room_1(ch, original, TO_ROOM_AT);
         ch->on = on;
         break;
      }
   }

   return;
}



void do_goto( CHAR_DATA *ch, char *argument )
{
   ROOM_INDEX_DATA *location;
   CHAR_DATA *rch;
   int count = 0;

   if ( argument[0] == '\0' )
   {
      send_to_char( "Goto where?\n\r", ch );
      return;
   }

   if ( ( location = find_location( ch, argument ) ) == NULL )
   {
      send_to_char( "No such location.\n\r", ch );
      return;
   }

   count = 0;
   for ( rch = location->people; rch != NULL; rch = rch->next_in_room )
   count++;

   if (!is_room_owner(ch, location) && room_is_private(location)
   &&  (count > 1 || get_trust(ch) < MAX_LEVEL))
   {
      send_to_char( "That room is private right now.\n\r", ch );
      return;
   }

   if ( ch->fighting != NULL )
   stop_fighting( ch, TRUE );

   for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room)
   {
      if
      (
         get_trust(rch) >=
         ch->invis_level &&
         rch != ch->nosee &&
         !(
            ch != NULL &&
            rch != NULL &&
            !IS_NPC(ch) &&
            !IS_NPC(rch) &&
            IS_IMMORTAL(ch) &&
            rch->pcdata->nosee_perm != NULL &&
            rch->pcdata->nosee_perm[0] != '\0' &&
            is_name(ch->name, rch->pcdata->nosee_perm)
         )
      )
      {
         if
         (
            ch->pcdata != NULL &&
            ch->pcdata->bamfout != NULL &&
            ch->pcdata->bamfout[0] != '\0'
         )
         {
            act("$t", ch, ch->pcdata->bamfout, rch, TO_VICT);
         }
         else if
         (
            can_see(rch, ch) &&
            IS_IMMORTAL(rch)
         )
         {
            act("$n leaves in a swirling mist.", ch, NULL, rch, TO_VICT);
         }
      }
   }

   char_from_room( ch );
   char_to_room( ch, location );


   for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room)
   {
      if
      (
         get_trust(rch) >= ch->invis_level &&
         rch != ch->nosee &&
         !(
            ch != NULL &&
            rch != NULL &&
            !IS_NPC(ch) &&
            !IS_NPC(rch) &&
            IS_IMMORTAL(ch) &&
            rch->pcdata->nosee_perm != NULL &&
            rch->pcdata->nosee_perm[0] != '\0' &&
            is_name(ch->name, rch->pcdata->nosee_perm)
         )
      )
      {
         if
         (
            ch->pcdata != NULL &&
            ch->pcdata->bamfin != NULL &&
            ch->pcdata->bamfin[0] != '\0'
         )
         {
            act("$t", ch, ch->pcdata->bamfin, rch, TO_VICT);
         }
         else if
         (
            can_see(rch, ch) &&
            IS_IMMORTAL(rch)
         )
         {
            act("$n appears in a swirling mist.", ch, NULL, rch, TO_VICT);
         }
      }
   }

   do_observe(ch, "", LOOK_AUTO);
   return;
}

void do_violate( CHAR_DATA *ch, char *argument )
{
   ROOM_INDEX_DATA *location;
   CHAR_DATA *rch;

   if ( argument[0] == '\0' )
   {
      send_to_char( "Goto where?\n\r", ch );
      return;
   }

   if ( ( location = find_location( ch, argument ) ) == NULL )
   {
      send_to_char( "No such location.\n\r", ch );
      return;
   }

   if (!room_is_private( location ))
   {
      send_to_char( "That room isn't private, use goto.\n\r", ch );
      return;
   }

   if ( ch->fighting != NULL )
   stop_fighting( ch, TRUE );

   for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room)
   {
      if (get_trust(rch) >= ch->invis_level)
      {
         if (ch->pcdata != NULL && ch->pcdata->bamfout[0] != '\0')
         act("$t", ch, ch->pcdata->bamfout, rch, TO_VICT);
         else
         act("$n leaves in a swirling mist.", ch, NULL, rch, TO_VICT);
      }
   }

   char_from_room( ch );
   char_to_room( ch, location );


   for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room)
   {
      if (get_trust(rch) >= ch->invis_level)
      {
         if (ch->pcdata != NULL && ch->pcdata->bamfin[0] != '\0')
         act("$t", ch, ch->pcdata->bamfin, rch, TO_VICT);
         else
         act("$n appears in a swirling mist.", ch, NULL, rch, TO_VICT);
      }
   }

   do_observe(ch, "", LOOK_AUTO);
   return;
}

/* RT to replace the 3 stat commands */

void do_stat ( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   char *string;
   OBJ_DATA *obj;
   ROOM_INDEX_DATA *location;
   CHAR_DATA *victim;

   string = one_argument(argument, arg);
   if ( arg[0] == '\0')
   {
      send_to_char("Syntax:\n\r", ch);
      send_to_char("  stat <name>\n\r", ch);
      send_to_char("  stat obj <name>\n\r", ch);
      send_to_char("  stat mob <name>\n\r", ch);
      send_to_char("  stat room <number>\n\r", ch);
      return;
   }

   if (!str_cmp(arg, "room"))
   {
      do_rstat(ch, string);
      return;
   }

   if (!str_cmp(arg, "char")  || !str_cmp(arg, "mob"))
   {
      do_mstat(ch, string);
      return;
   }

   if (!str_cmp(arg, "obj"))
   {
      do_ostat(ch, string);
      return;
   }


   /* do it the old way */


   victim = get_char_world(ch, argument);
   if (victim != NULL)
   {
      do_mstat(ch, argument);
      return;
   }

   obj = get_obj_world(ch, argument);
   if (obj != NULL)
   {
      do_ostat(ch, argument);
      return;
   }

   location = find_location(ch, argument);
   if (location != NULL)
   {
      do_rstat(ch, argument);
      return;
   }

   send_to_char("Nothing by that name found anywhere.\n\r", ch);
}





void do_rstat( CHAR_DATA *ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   char arg[MAX_INPUT_LENGTH];
   ROOM_INDEX_DATA *location;
   ROOM_AFFECT_DATA *paf;
   OBJ_DATA *obj;
   CHAR_DATA *rch;
   int door;
   char time_string[MAX_INPUT_LENGTH];

   one_argument( argument, arg );
   location = ( arg[0] == '\0' ) ? ch->in_room : find_location( ch, arg );
   if ( location == NULL )
   {
      send_to_char( "No such location.\n\r", ch );
      return;
   }

   if (!is_room_owner(ch, location) && ch->in_room != location
   &&  room_is_private( location ) && !IS_TRUSTED(ch, IMPLEMENTOR))
   {
      send_to_char( "That room is private right now.\n\r", ch );
      return;
   }

   sprintf( buf, "Name: '%s'\n\rArea: '%s'\n\r",
   location->name,
   location->area->name );
   send_to_char( buf, ch );

   sprintf( buf,
   "Vnum: %d  Sector: %d  Light: %d  Healing: %d  Mana: %d\n\r",
   location->vnum,
   location->sector_type,
   location->light,
   location->heal_rate,
   location->mana_rate );
   send_to_char( buf, ch );
   if (IS_SET(location->extra_room_flags, ROOM_1212))
   {
      send_to_char("Is a punishment room.  ROOM_1212 flag set.\n\r", ch);
   }
   if (location-> guild != 0){
      sprintf( buf, "Guild type: %d\n\r", location->guild);
      send_to_char( buf, ch );
   }

   sprintf( buf,
   "Room flags: %s.\n\rDescription:\n\r%s",
   room_bit_name(location->room_flags, location->extra_room_flags),
   location->description );
   send_to_char( buf, ch );

   if ( location->extra_descr != NULL )
   {
      EXTRA_DESCR_DATA *ed;

      send_to_char( "Extra description keywords: '", ch );
      for ( ed = location->extra_descr; ed; ed = ed->next )
      {
         send_to_char( ed->keyword, ch );
         if ( ed->next != NULL )
         send_to_char( " ", ch );
      }
      send_to_char( "'.\n\r", ch );
   }

   send_to_char( "Characters:", ch );
   for ( rch = location->people; rch; rch = rch->next_in_room )
   {
      if (can_see(ch, rch))
      {
         send_to_char( " ", ch );
         one_argument( rch->name, buf );
         send_to_char( buf, ch );
      }
   }

   send_to_char( ".\n\rObjects:   ", ch );
   for ( obj = location->contents; obj; obj = obj->next_content )
   {
      send_to_char( " ", ch );
      one_argument( obj->name, buf );
      send_to_char( buf, ch );
   }
   send_to_char( ".\n\r", ch );

   for ( door = 0; door <= 5; door++ )
   {
      EXIT_DATA *pexit;

      if ( ( pexit = location->exit[door] ) != NULL )
      {
         sprintf( buf,
         "Door: %d.  To: %d.  Key: %d.  Exit flags: %d.\n\rKeyword: '%s'.  Description: %s",

         door,
         (pexit->u1.to_room == NULL ? -1 : pexit->u1.to_room->vnum),
         pexit->key,
         pexit->exit_info,
         pexit->keyword,
         pexit->description[0] != '\0'
         ? pexit->description : "(none).\n\r" );
         send_to_char( buf, ch );
      }
   }

   send_to_char("Room Affects:\n\r", ch);
   for ( paf = location->affected; paf != NULL; paf = paf->next )
   {
      if (paf->where == TO_ROOM || paf->where == TO_ROOM_EXTRA)
      {
         sprintf
         (
            buf,
            "Affect: '%s' placed by %s%s with bits %s, level %d.\n\r",
            skill_table[(int) paf->type].name,
            (
               paf->caster ?
               paf->caster->name :
               ""
            ),
            get_time_string
            (
               ch,
               ch,
               NULL,
               paf,
               time_string,
               TYPE_TIME_STAT,
               !(IS_SET(ch->comm2, COMM_STAT_TIME))
            ),
            (
               paf->where == TO_ROOM ?
               room_bit_name(paf->bitvector, 0) :
               room_bit_name(0, paf->bitvector)
            ),
            paf->level
         );
      }
      else
      {
         sprintf
         (
            buf,
            "Affect: '%s' changes sector type from %d to %d%s. Level %d\n\r",
            skill_table[(int) paf->type].name,
            paf->sector_old,
            paf->sector_new,
            get_time_string
            (
               ch,
               ch,
               NULL,
               paf,
               time_string,
               TYPE_TIME_STAT,
               !(IS_SET(ch->comm2, COMM_STAT_TIME))
            ),
            paf->level
         );
      }
      send_to_char(buf, ch);
   }

   return;
}



void do_ostat( CHAR_DATA *ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   char arg[MAX_INPUT_LENGTH];
   AFFECT_DATA *paf;
   OBJ_DATA *obj;
   OBJ_INDEX_DATA *pObjIndex;
   int vnum = -1;
   char time_string[MAX_INPUT_LENGTH];
   int counter;

   one_argument( argument, arg );

   if ( arg[0] == '\0' )
   {
      send_to_char( "Stat what?\n\r", ch );
      return;
   }

   if ( is_number(arg) ){
      vnum =atoi( arg );
      if ((pObjIndex = get_obj_index(vnum)) == NULL){
         send_to_char("No Object with that vnum.\n\r", ch);
         return;
      }
      obj = create_object(pObjIndex, 0);
      obj_to_char(obj, ch);
   }
   else if ( ( obj = get_obj_world( ch, argument ) ) == NULL )
   {
      send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch );
      return;
   }

   sprintf( buf, "Name(s): %s\n\r",
   obj->name );
   send_to_char( buf, ch );

   sprintf
   (
      buf,
      "Vnum: %d  Format: %s  Type: %s  Resets: %d  Pulsing: %s\n\r",
      obj->pIndexData->vnum,
      obj->pIndexData->new_format ?
      "new" :
      "old",
      item_name(obj->item_type),
      obj->pIndexData->reset_num,
      (
         is_pulsing(obj) ?
         "Yes" :
         "No"
      )
   );
   send_to_char( buf, ch );

   sprintf( buf, "Short description: %s\n\rLong description: %s\n\r",
   obj->short_descr, obj->description );
   send_to_char( buf, ch );

   sprintf( buf, "Material is: %s (%s)\n\r",
   obj->material, is_metal(obj) ? "metal": "non-metal");
   send_to_char( buf, ch );

   sprintf( buf, "Owner: %s\n\r", obj->owner);
   send_to_char(buf, ch);
   for (counter = 0; counter < MAX_OWNERS; counter++)
   {
      if (obj->prev_owners[counter][0] != '\0')
      {
         sprintf
         (
            buf,
            "Previous owner %2d: %-14s%s",
            counter + 1,
            obj->prev_owners[counter],
            ((counter + 1) % 2 == 0) ? "\n\r" : ""
         );
         send_to_char(buf, ch);
         if (
               (counter + 1) % 2 != 0 &&
               (
                  counter + 1 == MAX_OWNERS ||
                  obj->prev_owners[counter + 1][0] == '\0'
               )
            )
         {
            send_to_char("\n\r", ch);
         }
      }
      else
      {
         break;
      }
   }

   sprintf( buf, "Wear bits: %s\n\rExtra bits: %s\n\r",
   wear_bit_name(obj->wear_flags), extra_bit_name( obj->extra_flags, 0) );
   send_to_char( buf, ch );

   sprintf(buf, "Extra2 bits: %s\n\r", extra_bit_name(0, obj->extra_flags2));
   send_to_char(buf, ch);

   sprintf( buf, "Restrict_flags: %s\n\r",
   restrict_bit_name(obj->pIndexData->restrict_flags));
   send_to_char(buf, ch);

   sprintf( buf, "Number: %d/%d  Weight: %d/%d/%d (10th pounds)\n\r",
   1,           get_obj_number( obj ),
   obj->weight, get_obj_weight( obj ), get_true_weight(obj) );
   send_to_char( buf, ch );

   sprintf( buf, "Level: %d  Cost: %d  Condition: %d  Timer: %d\n\r",
   obj->level, obj->cost, obj->condition, obj->timer );
   send_to_char( buf, ch );

   sprintf( buf,
   "In room: %d  In object: %s  Carried by: %s  Wear_loc: %d\n\r",
   obj->in_room    == NULL    ?        0 : obj->in_room->vnum,
   obj->in_obj     == NULL    ? "(none)" : obj->in_obj->short_descr,
   obj->carried_by == NULL    ? "(none)" :
   can_see(ch, obj->carried_by) ? obj->carried_by->name
   : "someone",
   obj->wear_loc );
   send_to_char( buf, ch );

   if ( obj->mount_specs )
   {
      sprintf( buf, "Mount information:  Moves: %d   Mobility: %d   Movement Flags: %s\n\r",
      obj->mount_specs->move, obj->mount_specs->mobility, movement_bit_name( obj->mount_specs->move_flags ) );
      send_to_char( buf, ch );
   }

   sprintf( buf, "Values: %d %d %d %d %d\n\r",
   obj->value[0], obj->value[1], obj->value[2], obj->value[3],
   obj->value[4] );
   send_to_char( buf, ch );

   /* Give moon phases for moongates on stat - Wervdon  6-20-2004 */
   if
   (
      obj->item_type == ITEM_PORTAL_NEW &&
      IS_SET(obj->value[4], PORTAL_MOONGATE)
   )
   {
      sprintf
      (
         buf,
         "Phases: %d %d %d\n\r",
         obj->moonphases[0],
         obj->moonphases[1],
         obj->moonphases[2]
      );
      send_to_char(buf, ch);
   }

   /* Report object limit and count and say if maxxed */
   sprintf( buf, "COUNT_DATA: Limit is %d, Count is %d -->%s.\n\r", obj->pIndexData->limtotal, obj->pIndexData->limcount,
   (obj->pIndexData->limcount < obj->pIndexData->limtotal) ? "Not maxxed" : (obj->pIndexData->limtotal == 0) ? "Non-limited" : "Maxxed");
   send_to_char(buf, ch);

   /* now give out vital statistics as per identify */

   switch ( obj->item_type )
   {
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

      case ITEM_SCROLL:
      case ITEM_POTION:
      case ITEM_PILL:
      case ITEM_HERB:
      sprintf( buf, "Level %d spells of:", obj->value[0] );
      send_to_char( buf, ch );

      if
      (
         obj->value[1] > 0 &&
         obj->value[1] < MAX_SKILL
      )
      {
         send_to_char( " '", ch );
         send_to_char( skill_table[obj->value[1]].name, ch );
         send_to_char( "'", ch );
      }

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
         send_to_char( " '", ch );
         send_to_char( skill_table[obj->value[3]].name, ch );
         send_to_char( "'", ch );
      }

      if
      (
         obj->value[4] > 0 &&
         obj->value[4] < MAX_SKILL
      )
      {
         send_to_char(" '", ch);
         send_to_char(skill_table[obj->value[4]].name, ch);
         send_to_char("'", ch);
      }

      send_to_char( ".\n\r", ch );
      break;

      case ITEM_WAND:
      case ITEM_STAFF:
      sprintf( buf, "Has %d(%d) charges of level %d",
      obj->value[1], obj->value[2], obj->value[0] );
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

      send_to_char( ".\n\r", ch );
      break;

      case ITEM_DRINK_CON:
      sprintf(buf, "It holds %s-colored %s.\n\r",
      liq_table[obj->value[2]].liq_color,
      liq_table[obj->value[2]].liq_name);
      send_to_char(buf, ch);
      break;


      case ITEM_WEAPON:
      send_to_char("Weapon type is ", ch);
      switch (obj->value[0])
      {
         case(WEAPON_EXOTIC):
         send_to_char("exotic\n\r", ch);
         break;
         case(WEAPON_BOW):
         send_to_char("bow\n\r", ch);
         break;
         case(WEAPON_ARROWS):
         send_to_char("arrows\n\r", ch);
         break;
         case(WEAPON_SWORD):
         send_to_char("sword\n\r", ch);
         break;
         case(WEAPON_DAGGER):
         send_to_char("dagger\n\r", ch);
         break;
         case(WEAPON_SPEAR):
         send_to_char("spear/staff\n\r", ch);
         break;
         case(WEAPON_MACE):
         send_to_char("mace/club\n\r", ch);
         break;
         case(WEAPON_AXE):
         send_to_char("axe\n\r", ch);
         break;
         case(WEAPON_FLAIL):
         send_to_char("flail\n\r", ch);
         break;
         case(WEAPON_WHIP):
         send_to_char("whip\n\r", ch);
         break;
         case(WEAPON_POLEARM):
         send_to_char("polearm\n\r", ch);
         break;
         case (WEAPON_STAFF):
         send_to_char("staff.\n\r", ch);
         break;
         default:
         send_to_char("unknown\n\r", ch);
         break;
      }
      if (obj->pIndexData->new_format)
      sprintf(buf, "Damage is %dd%d (average %d)\n\r",
      obj->value[1], obj->value[2],
      (1 + obj->value[2]) * obj->value[1] / 2);
      else
      sprintf( buf, "Damage is %d to %d (average %d)\n\r",
      obj->value[1], obj->value[2],
      ( obj->value[1] + obj->value[2] ) / 2 );
      send_to_char( buf, ch );

      sprintf(buf, "Damage noun is %s.\n\r",
      (obj->value[3] > 0 && obj->value[3] < MAX_DAMAGE_MESSAGE) ?
      attack_table[obj->value[3]].noun : "undefined");
      send_to_char(buf, ch);

      if (obj->value[4])  /* weapon flags */
      {
         sprintf(buf, "Weapons flags: %s\n\r",
         weapon_bit_name(obj->value[4]));
         send_to_char(buf, ch);
      }
      break;

      case ITEM_ARMOR:
      sprintf( buf,
      "Armor class is %d pierce, %d bash, %d slash, and %d vs. magic\n\r",
      obj->value[0], obj->value[1], obj->value[2], obj->value[3] );
      send_to_char( buf, ch );
      break;

      case ITEM_CONTAINER:
      sprintf(buf, "Actual weight to carrier: %d# (Multiplier: %d%%)\n\r",
      get_obj_weight(obj) / 10, obj->value[4]);
      send_to_char(buf, ch);
      sprintf(buf, "Capacity: %d/%d#  Maximum item weight: %d#  Flags: %s\n\r",
      (get_true_weight(obj) - obj->weight) / 10, obj->value[0], obj->value[3],
      cont_bit_name(obj->value[1]));
      send_to_char(buf, ch);
      break;
   }


   if ( obj->extra_descr != NULL || obj->pIndexData->extra_descr != NULL )
   {
      EXTRA_DESCR_DATA *ed;

      send_to_char( "Extra description keywords: '", ch );

      for ( ed = obj->extra_descr; ed != NULL; ed = ed->next )
      {
         send_to_char( ed->keyword, ch );
         if ( ed->next != NULL )
         send_to_char( " ", ch );
      }

      for ( ed = obj->pIndexData->extra_descr; ed != NULL; ed = ed->next )
      {
         send_to_char( ed->keyword, ch );
         if ( ed->next != NULL )
         send_to_char( " ", ch );
      }

      send_to_char( "'\n\r", ch );
   }

   for ( paf = obj->affected; paf != NULL; paf = paf->next )
   {
      sprintf( buf, "Affects %s by %d, level %d",
      affect_loc_name( paf->location ), paf->modifier, paf->level );
      send_to_char(buf, ch);
      if ( paf->duration > -1)
      {
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
               TYPE_TIME_STAT,
               !(IS_SET(ch->comm2, COMM_STAT_TIME))
            )
         );
      }
      else
      {
         sprintf(buf, ".\n\r");
      }
      send_to_char( buf, ch );
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
            case TO_WEAPON:
            sprintf(buf, "Adds %s weapon flags.\n",
            weapon_bit_name(paf->bitvector));
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
         send_to_char(buf, ch);
      }
   }

   if (!obj->enchanted)
   for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
   {
      sprintf( buf, "Affects %s by %d, level %d.\n\r",
      affect_loc_name( paf->location ), paf->modifier, paf->level );
      send_to_char( buf, ch );
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
         send_to_char(buf, ch);
      }
   }
   if (vnum != -1){
      extract_obj(obj, FALSE);
   }

   return;
}

void do_probation( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;

   one_argument( argument, arg );

   if ( arg[0] == '\0' )
   {
      send_to_char( "Set whom on probation?\n\r", ch );
      return;
   }

   if ( ( victim = get_char_world( ch, argument ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if (IS_NPC(victim))
   {
      send_to_char("I don't think you need to worry about mobs.\n\r", ch);
      return;
   }

   if (victim->pcdata->probation < 0)
   {
      victim->pcdata->probation = 0;
      victim->pcdata->prob_time = current_time;
      send_to_char("Player put on probation.\n\r", ch);
   }
   else
   {
      victim->pcdata->probation = -1;
      victim->pcdata->prob_time = -1;
      send_to_char("Player no longer on probation.\n\r", ch);
   }
   return;
}

void do_mstat( CHAR_DATA *ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   char arg[MAX_INPUT_LENGTH];
   AFFECT_DATA *paf;
   CHAR_DATA *victim;
   time_t probt;
   char time_string[MAX_INPUT_LENGTH];
   MOB_INDEX_DATA *pMobIndex;
   int vnum = -1;
   bool mob_created = FALSE;
   LIST_DATA* list;
   NODE_DATA* node;
   SKILL_MOD* mod;

   one_argument(argument, arg);

   if (arg[0] == '\0')
   {
      send_to_char("Stat whom?\n\r", ch);
      return;
   }

   if (is_number(arg))
   {
      vnum =atoi(arg);
      if
      (
         (pMobIndex = get_mob_index(vnum)) == NULL
      )
      {
         send_to_char("No Mobile with that vnum.\n\r", ch);
         return;
      }
      victim = create_mobile(pMobIndex);
      char_to_room(victim, ch->in_room);
      mob_created = TRUE;
   }
   else if
   (
      (victim = get_char_world(ch, argument)) == NULL
   )
   {
      send_to_char("No such mobile exists in the world.\n\r", ch );
      return;
   }

   sprintf(buf, "Name: %s\n\r", victim->name);
   send_to_char(buf, ch);
   if
   (
      !IS_NPC(victim) &&
      victim->pcdata->moniker[0] != '\0'
   )
   {
      sprintf(buf, "Moniker: %s\n\r", victim->pcdata->moniker);
      send_to_char(buf, ch);
   }
   sprintf
   (
      buf,
      "Vnum: %d  Format: %s  Race: %s%s  Group: %d  Sex: %s  Room: %d\n\r",
      IS_NPC(victim) ? victim->pIndexData->vnum : 0,
      IS_NPC(victim) ? victim->pIndexData->new_format ? "new" : "old" : "pc",
      race_table[victim->race].name, get_scale_color(victim),
      IS_NPC(victim) ? victim->group : 0,
      sex_table[URANGE(0, victim->sex, 3)].name,
      victim->in_room == NULL ? 0 : victim->in_room->vnum
   );
   send_to_char(buf, ch);

   if (IS_NPC(victim))
   {
      sprintf
      (
         buf,
         "Count: %d  Killed: %d\n\r",
         victim->pIndexData->count,
         victim->pIndexData->killed
      );
      send_to_char(buf, ch);
   }
   else
   {
      if (victim->pcdata->last_death != 0)
      {
         sprintf
         (
            buf,
            "Last time pkilled: %d hours ago.\n\r",
            (int) (current_time-victim->pcdata->last_death) / (3600)
         );
         send_to_char(buf, ch);
      }

      if (victim->ghost)
      {
         sprintf(buf, "Ghost timer: %d.\n\r", victim->ghost);
         send_to_char(buf, ch);
      }

      sprintf
      (
         buf,
         "Standing: %ld%s\n\r",
         victim->pcdata->targetkills,
         (
            (
               victim->house == HOUSE_ANCIENT ||
               victim->house == HOUSE_ENFORCER
            ) ?
            "" :
            " (not Ancient or Enforcer)"
         )
      );
      send_to_char(buf, ch);
      if (victim->pcdata->induct != 0)
      {
         sprintf(buf, "Induct: %d\n\r", victim->pcdata->induct);
         send_to_char(buf, ch);
      }
   }

   sprintf
   (
      buf,
      "Str: %-2d(%-2d)  Int: %-2d(%-2d)  Wis: %-2d(%-2d)  Dex: %-2d(%-2d)"
      "  Con: %-2d(%-2d)\n\r",
      victim->perm_stat[STAT_STR],
      get_curr_stat(victim, STAT_STR),
      victim->perm_stat[STAT_INT],
      get_curr_stat(victim, STAT_INT),
      victim->perm_stat[STAT_WIS],
      get_curr_stat(victim, STAT_WIS),
      victim->perm_stat[STAT_DEX],
      get_curr_stat(victim, STAT_DEX),
      victim->perm_stat[STAT_CON],
      get_curr_stat(victim, STAT_CON)
   );
   send_to_char( buf, ch );
   if
   (
      !IS_NPC(victim) &&
      (
         victim->pcdata->stat_bonus[0] ||
         victim->pcdata->stat_bonus[1] ||
         victim->pcdata->stat_bonus[2] ||
         victim->pcdata->stat_bonus[3] ||
         victim->pcdata->stat_bonus[4]
      )
   )
   {
      sprintf
      (
         buf,
         "Str: %s%-5d  Int: %s%-5d  Wis: %s%-5d  Dex: %s%-5d  Con: %s%-5d\n\r",
         victim->pcdata->stat_bonus[STAT_STR] > -1 ? "+" : "",
         victim->pcdata->stat_bonus[STAT_STR],
         victim->pcdata->stat_bonus[STAT_INT] > -1 ? "+" : "",
         victim->pcdata->stat_bonus[STAT_INT],
         victim->pcdata->stat_bonus[STAT_WIS] > -1 ? "+" : "",
         victim->pcdata->stat_bonus[STAT_WIS],
         victim->pcdata->stat_bonus[STAT_DEX] > -1 ? "+" : "",
         victim->pcdata->stat_bonus[STAT_DEX],
         victim->pcdata->stat_bonus[STAT_CON] > -1 ? "+" : "",
         victim->pcdata->stat_bonus[STAT_CON]
      );
      send_to_char(buf, ch);
   }
   if (!IS_NPC(victim))
   {
      if (victim->pcdata->gained_imm_flags)
      {
         sprintf
         (
            buf,
            "{B{3PGI{n: (%s) %s",
            print_flags(victim->pcdata->gained_imm_flags),
            imm_bit_name(victim->pcdata->gained_imm_flags)
         );
         act_new_color
         (
            buf,
            ch,
            NULL,
            NULL,
            TO_CHAR,
            POS_DEAD,
            TRUE
         );
      }
      if (victim->pcdata->gained_res_flags)
      {
         sprintf
         (
            buf,
            "{B{3PGR{n: (%s) %s",
            print_flags(victim->pcdata->gained_res_flags),
            imm_bit_name(victim->pcdata->gained_res_flags)
         );
         act_new_color
         (
            buf,
            ch,
            NULL,
            NULL,
            TO_CHAR,
            POS_DEAD,
            TRUE
         );
      }
      if (victim->pcdata->gained_vuln_flags)
      {
         sprintf
         (
            buf,
            "{B{3PGV{n: (%s) %s",
            print_flags(victim->pcdata->gained_vuln_flags),
            imm_bit_name(victim->pcdata->gained_vuln_flags)
         );
         act_new_color
         (
            buf,
            ch,
            NULL,
            NULL,
            TO_CHAR,
            POS_DEAD,
            TRUE
         );
      }
      if (victim->pcdata->gained_affects)
      {
         sprintf
         (
            buf,
            "{B{3PGA{n: (%s) %s",
            print_flags(victim->pcdata->gained_affects),
            affect_bit_name(victim->pcdata->gained_affects, 0)
         );
         act_new_color
         (
            buf,
            ch,
            NULL,
            NULL,
            TO_CHAR,
            POS_DEAD,
            TRUE
         );
      }
      if (victim->pcdata->gained_affects2)
      {
         sprintf
         (
            buf,
            "{B{3PGA2{n: (%s) %s",
            print_flags(victim->pcdata->gained_affects2),
            affect_bit_name(0, victim->pcdata->gained_affects2)
         );
         act_new_color
         (
            buf,
            ch,
            NULL,
            NULL,
            TO_CHAR,
            POS_DEAD,
            TRUE
         );
      }
      if (victim->pcdata->lost_imm_flags)
      {
         sprintf
         (
            buf,
            "{B{3PLI{n: (%s) %s",
            print_flags(victim->pcdata->lost_imm_flags),
            imm_bit_name(victim->pcdata->lost_imm_flags)
         );
         act_new_color
         (
            buf,
            ch,
            NULL,
            NULL,
            TO_CHAR,
            POS_DEAD,
            TRUE
         );
      }
      if (victim->pcdata->lost_res_flags)
      {
         sprintf
         (
            buf,
            "{B{3PLR{n: (%s) %s",
            print_flags(victim->pcdata->lost_res_flags),
            imm_bit_name(victim->pcdata->lost_res_flags)
         );
         act_new_color
         (
            buf,
            ch,
            NULL,
            NULL,
            TO_CHAR,
            POS_DEAD,
            TRUE
         );
      }
      if (victim->pcdata->lost_vuln_flags)
      {
         sprintf
         (
            buf,
            "{B{3PLV{n: (%s) %s",
            print_flags(victim->pcdata->lost_vuln_flags),
            imm_bit_name(victim->pcdata->lost_vuln_flags)
         );
         act_new_color
         (
            buf,
            ch,
            NULL,
            NULL,
            TO_CHAR,
            POS_DEAD,
            TRUE
         );
      }
      if (victim->pcdata->lost_affects)
      {
         sprintf
         (
            buf,
            "{B{3PLA{n: (%s) %s",
            print_flags(victim->pcdata->lost_affects),
            affect_bit_name(victim->pcdata->lost_affects, 0)
         );
         act_new_color
         (
            buf,
            ch,
            NULL,
            NULL,
            TO_CHAR,
            POS_DEAD,
            TRUE
         );
      }
      if (victim->pcdata->lost_affects2)
      {
         sprintf
         (
            buf,
            "{B{3PLA2{n: (%s) %s",
            print_flags(victim->pcdata->lost_affects2),
            affect_bit_name(0, victim->pcdata->lost_affects2)
         );
         act_new_color
         (
            buf,
            ch,
            NULL,
            NULL,
            TO_CHAR,
            POS_DEAD,
            TRUE
         );
      }
   }
   sprintf
   (
      buf,
      "Hp: %d/%d  Mana: %d/%d  Move: %d/%d  Practices: %d\n\r",
      victim->hit,
      victim->max_hit,
      victim->mana,
      victim->max_mana,
      victim->move,
      victim->max_move,
      IS_NPC(ch) ? 0 : victim->practice
   );
   send_to_char(buf, ch);

   sprintf
   (
      buf,
      "Lv: %d (%d) Class: %s  Align: %d  Gold: %ld  Silver: %ld  Exp: %d"
      " (%5d tnl)\n\r",
      victim->level,
      GET_LEVEL(victim),
      IS_NPC(victim) ? "mobile" : class_table[victim->class].name,
      victim->alignment,
      victim->gold,
      victim->silver,
      victim->exp,
      victim->level * exp_per_level(victim) - victim->exp
   );
   send_to_char(buf, ch);
   if (get_trust(victim) != victim->level && get_trust(ch) == MAX_LEVEL)
   {
      sprintf(buf, "Trusted to %d\n\r", get_trust(victim));
      send_to_char(buf, ch);
   }
   if (!IS_NPC(victim))
   {
      sprintf
      (
         buf,
         "Bank balance: %ld gold, %ld silver.\n\r",
         victim->gold_bank,
         victim->silver_bank
      );
      send_to_char(buf, ch);
   }
   sprintf(buf, "Exp total: %d, ", victim->exp_total);
   send_to_char(buf, ch);

   sprintf
   (
      buf,
      "Hometown: %s\n\rSpecialization: %s\n\r",
      hometown_table[hometown_lookup(victim->temple)].name,
      IS_NPC(victim) ?
      "none" :
      (
         get_special_name(victim->class, victim->pcdata->special) == NULL
      ) ? "No specialization" :
      get_special_name(victim->class, victim->pcdata->special)
   );
   send_to_char(buf, ch);

   if (!IS_NPC(victim))
   {
      sprintf(buf, "Ethos: %d ", victim->pcdata->ethos);
      send_to_char(buf, ch);
   }
   if (!IS_NPC(victim))
   {
      sprintf(buf, "Death count: %d\n\r", victim->pcdata->death_count);
      send_to_char(buf, ch);
   }

   sprintf
   (
      buf,
      "Armor: pierce: %d  bash: %d  slash: %d  magic: %d\n\r",
      GET_AC(victim, AC_PIERCE),
      GET_AC(victim, AC_BASH),
      GET_AC(victim, AC_SLASH),
      GET_AC(victim, AC_EXOTIC)
   );
   send_to_char(buf, ch);

   sprintf
   (
      buf,
      "Hit: %d  Dam: %d  Size: %s  Position: %s  Wimpy: %d\n\r",
      GET_HITROLL(victim),
      GET_DAMROLL(victim),
      size_table[URANGE(0, victim->size, 5)].name,
      position_table[URANGE(0, victim->position, 8)].name,
      victim->wimpy
   );
   send_to_char(buf, ch);
   if
   (
      victim->spell_power ||
      victim->holy_power
   )
   {
      sprintf
      (
         buf,
         "Spell Power: %2d  Holy Power: %2d\n\r",
         victim->spell_power,
         victim->holy_power
      );
      send_to_char(buf, ch);
   }
   sprintf
   (
      buf,
      "Saves: normal (%d) breath (%d) malediction (%d) transport (%d) spell"
      " (%d)\n\r",
      victim->saving_throw,
      victim->saving_breath,
      victim->saving_maledict,
      victim->saving_transport,
      victim->saving_spell
   );
   send_to_char(buf, ch);
   if (victim->sight)
   {
      sprintf(buf, "Sight: %d\n\r", victim->sight);
      send_to_char(buf, ch);
   }

   if (IS_NPC(victim) && victim->pIndexData->new_format)
   {
      sprintf
      (
         buf,
         "Damage: %dd%d  Message:  %s\n\r",
         victim->damage[DICE_NUMBER],
         victim->damage[DICE_TYPE],
         attack_table[victim->dam_type].noun
      );
      send_to_char(buf, ch);
   }
   sprintf
   (
      buf, "Fighting: %s\n\r",
      victim->fighting ?
      victim->fighting->name :
      "(none)"
   );
   send_to_char(buf, ch);

   if (!IS_NPC(victim))
   {
      sprintf
      (
         buf,
         "Thirst: %d  Hunger: %d  Full: %d  Drunk: %d  Starve: %d  Dehydrate:"
         " %d\n\r",
         victim->pcdata->condition[COND_THIRST],
         victim->pcdata->condition[COND_HUNGER],
         victim->pcdata->condition[COND_FULL],
         victim->pcdata->condition[COND_DRUNK],
         victim->pcdata->condition[COND_STARVING],
         victim->pcdata->condition[COND_DEHYDRATED]
      );
      send_to_char(buf, ch);
   }

   sprintf
   (
      buf,
      "Carry number: %d  Carry weight: %ld\n\r",
      victim->carry_number,
      get_carry_weight(victim) / 10
   );
   send_to_char(buf, ch);


   if (!IS_NPC(victim))
   {
      OBJ_DATA *brand;

      if (victim->pcdata->death_status == HAS_DIED)
      {
         sprintf
         (
            buf,
            "Character is a ghost with %d ticks remaining.\n\r",
            victim->pcdata->death_timer
         );
         send_to_char(buf, ch);
      }
      brand = get_eq_char(ch, WEAR_BRAND);
      sprintf
      (
         buf,
         "Age: %d  Played: %d  Last Level: %d  Timer: %d  Quittime: %d.\n\r",
         get_real_age(victim),
         (int) (victim->played + current_time - victim->logon) / 3600,
         victim->pcdata->last_level,
         victim->timer, victim->quittime
      );
      send_to_char(buf, ch);
      sprintf
      (
         buf,
         "New age: %d (%s), Death_age: %d (age_mod is %d).\n\r",
         get_age(victim),
         get_age_name(victim),
         get_death_age(victim),
         victim->pcdata->age_mod
      );
      send_to_char(buf, ch);
      if (victim->is_riding)
      {
         switch(victim->is_riding->mount_type)
         {
            case (MOUNT_MOBILE):
            {
               sprintf
               (
                  buf,
                  "Player is riding %s.\n\r",
                  ((CHAR_DATA*)victim->is_riding->mount)->name
               );
               break;
            }
            case (MOUNT_OBJECT):
            {
               sprintf
               (
                  buf,
                  "Player is riding %s.\n\r",
                  ((OBJ_DATA *)victim->is_riding->mount)->short_descr
               );
               break;
            }
            default:
            {
               sprintf
               (
                  buf,
                  "Player is mounted.\n\r"
               );
               break;
            }
         }
         send_to_char(buf, ch);
      }
      if (victim->pcdata->probation > -1)
      {
         if (victim->pcdata->prob_time != -1)
         {
            sprintf
            (
               buf,
               "Character has been on Probation for %d days\n\r",
               (int) ((current_time - victim->pcdata->prob_time) / (3600 * 24))
            );
            send_to_char(buf, ch);
         }
         probt = current_time - victim->logon + victim->pcdata->probation;
         sprintf
         (
            buf,
            "Probation: %d hours %d minutes %d seconds\n\r",
            (int) probt / 3600,
            (int) (probt - ((int) probt / 3600) * 3600) / 60,
            (int) probt % 60
         );
      }
      else
      {
         sprintf(buf, "Probation: None\n\r");
      }
      send_to_char(buf, ch);
      sprintf(buf, "Hoard score: %d\n\r", hoard_score(victim));
      send_to_char(buf, ch);

      /*
      if (IS_SET(victim->act2, PLR_LOG_SPEC))
      {
      if (!IS_IMMORTAL(victim))
      send_to_char("Conversation is being logged.\n\r", ch);
      }
      */

      if
      (
         IS_IMMORTAL(victim) &&
         (
            victim->invis_level ||
            victim->incog_level
         )
      )
      {
         if (victim->invis_level)
         {
            sprintf(buf, "Invisible: %d  ", victim->invis_level);
            send_to_char(buf, ch);
         }
         if ( victim->incog_level )
         {
            sprintf(buf, "Incognito: %d", victim->incog_level);
            send_to_char(buf, ch);
         }
         send_to_char("\n\r", ch);
      }
   }
   sprintf(buf, "Act: %s\n\r", act_bit_name(victim->act, victim->act2));
   send_to_char(buf, ch);

   if (victim->comm)
   {
      sprintf(buf, "Comm: %s\n\r", comm_bit_name(victim->comm, 0));
      send_to_char(buf, ch);
   }

   if (victim->comm2)
   {
      sprintf(buf, "Comm2: %s\n\r", comm_bit_name(0, victim->comm2));
      send_to_char(buf, ch);
   }

   if (IS_NPC(victim) && victim->off_flags)
   {
      sprintf(buf, "Offense: %s\n\r", off_bit_name(victim->off_flags));
      send_to_char(buf, ch);
   }

   if (victim->imm_flags)
   {
      sprintf(buf, "Immune: %s\n\r", imm_bit_name(victim->imm_flags));
      send_to_char(buf, ch);
   }

   if (victim->res_flags)
   {
      sprintf(buf, "Resist: %s\n\r", imm_bit_name(victim->res_flags));
      send_to_char(buf, ch);
   }

   if (victim->vuln_flags)
   {
      sprintf(buf, "Vulnerable: %s\n\r", imm_bit_name(victim->vuln_flags));
      send_to_char(buf, ch);
   }

   sprintf
   (
      buf,
      "Form: %s\n\rParts: %s\n\r",
      form_bit_name(victim->form),
      part_bit_name(victim->parts)
   );
   send_to_char(buf, ch);

   if
   (
      victim->affected_by ||
      victim->affected_by2
   )
   {
      sprintf
      (
         buf,
         "Affected by %s\n\r",
         affect_bit_name(victim->affected_by, victim->affected_by2)
      );
      send_to_char(buf, ch);
   }

   sprintf
   (
      buf,
      "Master: %s  Leader: %s  Pet: %s\n\r",
      victim->master ? victim->master->name : "(none)",
      victim->leader ? victim->leader->name : "(none)",
      victim->pet ? victim->pet->name : "(none)"
   );
   send_to_char(buf, ch);

   sprintf
   (
      buf,
      "Short description: %s\n\rLong  description: %s%s",
      victim->short_descr,
      (
         victim->long_descr[0] != '\0' ?
         victim->long_descr :
         "(none)"
      ),
      (
         IS_NPC(victim) ?
         "" :
         "\n\r"
      )
   );

   send_to_char(buf, ch);

   if (IS_NPC(victim) && victim->spec_fun != 0)
   {
      sprintf
      (
         buf,
         "Mobile has special procedure %s.\n\r",
         spec_name(victim->spec_fun)
      );
      send_to_char(buf, ch);
   }


   /* tracking stuff */
   {
      CHAR_DATA *fightme = NULL;
      if (IS_NPC(victim) && victim->last_fought > 0)
      {
         fightme = id2name(victim->last_fought, FALSE);
         sprintf
         (
            buf,
            "TRACKING : Player %s.\n\r",
            fightme == NULL ? "<NOT IN REALM>" : fightme->name
         );
      }
      else
      {
         sprintf(buf, "TRACKING: Not tracking.\n\r");
      }

      send_to_char(buf, ch);
   }

   for (paf = victim->affected; paf != NULL; paf = paf->next)
   {
      sprintf
      (
         buf,
         "%s: '%s' modifies %s by %d%s",
         IS_SET(paf->bitvector2, AFF_HERB_SPELL) ? "Herb " :
         (
            skill_table[paf->type].spell_fun == spell_null ?
            "Skill" :
            "Spell"
         ),
         skill_table[paf->type].name,
         affect_loc_name(paf->location),
         paf->modifier,
         get_time_string
         (
            ch,
            victim,
            paf,
            NULL,
            time_string,
            TYPE_TIME_STAT,
            !(IS_SET(ch->comm2, COMM_STAT_TIME))
         )
      );
      send_to_char(buf, ch);
      sprintf
      (
         buf,
         " with bits %s, level %d.\n\r",
         affect_bit_name(paf->bitvector, paf->bitvector2),
         paf->level
      );
      send_to_char(buf, ch);
   }

   if
   (
      (
         list = victim->skill_mods
      ) != NULL
   )
   {
      send_to_char
      (
         "\n\r"
         "Skill modifiers:\n\r",
         ch
      );
      for (node = list->first; node; node = node->next)
      {
         mod = (SKILL_MOD*)node->data;

         sprintf
         (
            buf,
            "Skill [%10s] overrided to [%3d]%%\n\r",
            (
               mod->sn == SKILL_MOD_ALL ?
               "all" :
               skill_table[mod->sn].name
            ),
            mod->skill_percent
         );
         send_to_char(buf, ch);
      }
   }

   /* This should make sure the mob is gone when you're done with stat */
   if (mob_created)
   {
      extract_char(victim, TRUE);
   }
   return;
}

/* ofind and mfind replaced with vnum, vnum skill also added */

void do_vnum(CHAR_DATA *ch, char *argument)
{
   char arg[MAX_INPUT_LENGTH];
   char *string;

   string = one_argument(argument, arg);

   if (arg[0] == '\0')
   {
      send_to_char("Syntax:\n\r", ch);
      send_to_char("  vnum obj <name>\n\r", ch);
      send_to_char("  vnum mob <name>\n\r", ch);
      send_to_char("  vnum skill <skill or spell>\n\r", ch);
      return;
   }

   if (!str_cmp(arg, "obj"))
   {
      do_ofind(ch, string);
      return;
   }

   if (!str_cmp(arg, "mob") || !str_cmp(arg, "char"))
   {
      do_mfind(ch, string);
      return;
   }

   if (!str_cmp(arg, "skill") || !str_cmp(arg, "spell"))
   {
      do_slookup(ch, string);
      return;
   }
   /* do both */
   do_mfind(ch, argument);
   do_ofind(ch, argument);
}


void do_mfind( CHAR_DATA *ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   char arg[MAX_INPUT_LENGTH];
   MOB_INDEX_DATA *pMobIndex;
   bool fAll;
   bool found;
   AREA_DATA* parea;

   one_argument( argument, arg );
   if ( arg[0] == '\0' )
   {
      send_to_char( "Find whom?\n\r", ch );
      return;
   }

   fAll    = FALSE; /* !str_cmp( arg, "all" ); */
   found    = FALSE;

   for (parea = area_first; parea; parea = parea->next)
   {
      for (pMobIndex = parea->mobs; pMobIndex; pMobIndex = pMobIndex->next_in_area)
      {
         if ( fAll || is_name( argument, pMobIndex->player_name ) )
         {
            found = TRUE;
            sprintf( buf, "[%5d] %s\n\r",
            pMobIndex->vnum, pMobIndex->short_descr );
            send_to_char( buf, ch );
         }
      }
   }

   if ( !found )
   send_to_char( "No mobiles by that name.\n\r", ch );

   return;
}



void do_ofind( CHAR_DATA *ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   char arg[MAX_INPUT_LENGTH];
   OBJ_INDEX_DATA *pObjIndex;
   bool fAll;
   bool found;
   AREA_DATA* parea;


   one_argument( argument, arg );
   if ( arg[0] == '\0' )
   {
      send_to_char( "Find what?\n\r", ch );
      return;
   }

   fAll    = FALSE; /* !str_cmp( arg, "all" ); */
   found    = FALSE;

   for (parea = area_first; parea; parea = parea->next)
   {
      for (pObjIndex = parea->objects; pObjIndex; pObjIndex = pObjIndex->next_in_area)
      {
         if ( fAll || is_name( argument, pObjIndex->name ) )
         {
            found = TRUE;
            sprintf( buf, "[%5d] %s\n\r",
            pObjIndex->vnum, pObjIndex->short_descr );
            send_to_char( buf, ch );
         }
      }
   }

   if ( !found )
   send_to_char( "No objects by that name.\n\r", ch );

   return;
}

/*
 * tokenize () and parse_tokens(), the next two functions, are helper
 * functions to go with new versions of owhere and mwhere.  tokenize()
 * parses the argument into segments, and parse_tokense() turns those
 * segments into meaningful information.
 */

bool tokenize(
   CHAR_DATA *ch,
   const char *argument,
   char token[MAX_TOKENS][MAX_TOKEN_ELEMENTS][MAX_STRING_LENGTH])
{
   char argument_copy[MAX_STRING_LENGTH];
   char buf[MAX_STRING_LENGTH];
   int token_count;
   int element_count;
   char *comma_pos;
   char *equal_or_hyphen_pos;
   char *tokencrawler;
   bool finished;

   /* Quick sanity check */
   if (argument == NULL)
   {
      return FALSE;
   }

   finished = FALSE;

   /*
    * Copy the argument so that strtok can alter the string if it so
    * desires.  This allows us to bail gracefully if we hit something
    * unexpected
    */
   strcpy(argument_copy, argument);

   /* Switch to a pointer so that we can march through the string later */
   tokencrawler = argument_copy;

   /* Initialize the passed array */
   for (token_count = 0; token_count < MAX_TOKENS; token_count++)
   {
      for (
            element_count = 0;
            element_count < MAX_TOKEN_ELEMENTS;
            element_count++
         )
      {
         strcpy(token[token_count][element_count], "");
      }
   }

   /* Check for commas and equal signs to determine the order of tokenization
       and assure that this is a new style argument.  This is also considered
       a new style argument if the first character is a colon. */
   if (strpbrk(tokencrawler, ",=") == NULL)
   {
      if (tokencrawler[0] != ':' || tokencrawler[1] == '\0')
      {
         /* No equal, comma, or colon with argument: old style argument */
         return FALSE;
      }

      /* New style argument, but with only one token */
      strcpy(token[0][0], (tokencrawler +1));
      return TRUE;
   }

   if (tokencrawler[0] == ':')
   {
      tokencrawler++;
   }

   for (token_count = 0; token_count < MAX_TOKENS; token_count++)
   {
      for (
            element_count = 0;
            element_count < MAX_TOKEN_ELEMENTS;
            element_count++
         )
      {
         /*
          * Initialize the critical tokenizing variables.  The values of these
          * variables determine the next step in the tokenizing process.
          */
         comma_pos = strchr(tokencrawler, ',');
         equal_or_hyphen_pos = strpbrk(tokencrawler, "=-");

         /* Just a quick safety check here - we don't want NULLs later */
         if (comma_pos == tokencrawler || equal_or_hyphen_pos == tokencrawler)
         {
             /* We've got nothing to grab! */
             send_to_char("Unable to tokenize your search string.\n\r", ch);
             return FALSE;
         }

         if (comma_pos)
         {
            if (equal_or_hyphen_pos && equal_or_hyphen_pos < comma_pos)
            {
               /*
                * First we'll make sure that we aren't going to run out of
                * space within this token when we grab the NEXT element.
                */

               if (element_count + 1 == MAX_TOKEN_ELEMENTS)
               {
                  /* Too many elements in this token */
                  sprintf(
                     buf,
                     "You are only allowed %d elements per search type.\n\r",
                     MAX_TOKEN_ELEMENTS);
                  send_to_char(buf, ch);
                  return FALSE;
               }

               /*
                * We've got an equal or a hyphen before the next comma,
                * so we'll grab to that, reposition our pointer to
                * just after it, and loop to get the next token element.
                */

               strcpy(
                  token[token_count][element_count],
                  strtok(tokencrawler, "=-"));
               tokencrawler = equal_or_hyphen_pos + 1;
               continue;
            } else
            {
               /*
                * First we'll make sure that we aren't going to run out of
                * space when we move on to the NEXT token.
                */

               if (token_count + 1 == MAX_TOKENS)
               {
                  /* Too many tokens within this argument */
                  sprintf(
                     buf,
                     "You are only allowed %d search criteria.\n\r",
                     MAX_TOKENS);
                  send_to_char(buf, ch);
                  return FALSE;
               }

               /*
                * We've got a comma before the next equal or hyphen,
                * so we'll grab to that, reposition our pointer to
                * just after it, and break out of this loop to grab the
                * next token.
                */

               strcpy(
                  token[token_count][element_count],
                  strtok(tokencrawler, ","));
               tokencrawler = comma_pos + 1;
               break;
            }
         } else
         {
            if (equal_or_hyphen_pos)
            {
               /*
                * First we'll make sure that we aren't going to run out of
                * space within this token when we grab the NEXT element.
                */

               if (element_count + 1 == MAX_TOKEN_ELEMENTS)
               {
                  /* Too many elements in this token */
                  sprintf(
                     buf,
                     "You are only allowed %d elements per search type.\n\r",
                     MAX_TOKEN_ELEMENTS);
                  send_to_char(buf, ch);
                  return FALSE;
               }

               /*
                * We've got an equal or a hyphen, so we'll grab to that,
                * reposition our pointer to just after it,
                * and loop to get the next token element.
                */

               strcpy(
                  token[token_count][element_count],
                  strtok(tokencrawler, "=-"));
               tokencrawler = equal_or_hyphen_pos + 1;
               continue;
            } else
            {
               /*
                * This is the last little chunk of information before
                * the end of the string, so we'll grab it and quit.
                */

               strcpy(
                  token[token_count][element_count],
                  tokencrawler);
               finished = TRUE;
               break;
            }
         }
      }

      if (finished)
      {
         break;
      }
   }

   return TRUE;
}

bool parse_tokens(
   CHAR_DATA *ch,
   char token[MAX_TOKENS][MAX_TOKEN_ELEMENTS][MAX_STRING_LENGTH],
   int search[MAX_TOKENS][MAX_TOKEN_ELEMENTS + 1],
   long scope)
{
   char buf[MAX_STRING_LENGTH];
   char buf2[MAX_INPUT_LENGTH];
   int token_count;
   int temp, temp2;
   const char *tokencrawler;
   AREA_DATA *pArea;

   /*
    * Loop through available tokens to make sure they are meaningful, and
    *  turn the words in token[] into meaningful values stored in search[].
    */

   for (
         token_count = 0;
         (
            strcmp(token[token_count][0], "") &&
            token_count < MAX_TOKENS
         );
         token_count++
      )
   {
      /*
       * Make sure each set of token elements refers to a real search first.
       */
      tokencrawler = token[token_count][0];

      /* Scrap leading white space */
      while (tokencrawler[0] == ' ' || tokencrawler[0] == '\t')
      {
         tokencrawler++;
      }

      temp = search_type_lookup(tokencrawler, scope);

      if (temp == -1)
      {
         /* No search type found, return list of legal searches */
         sprintf(buf,"No such search type: %s.\n\r", tokencrawler);
         send_to_char(buf, ch);
         send_to_char("Allowed search types are:\n\r", ch);
         for (temp = 0; search_type_table[temp].name != NULL; temp++)
         {
            if (!(scope & search_type_table[temp].scope))
            {
               continue;
            }
            sprintf(buf, "%s\n\r", search_type_table[temp].name);
            send_to_char(buf, ch);
         }
         return FALSE;
      }

      /* Finally, we are confident this is a legal search. */
      search[token_count][0] = temp;
   };

   /* Determine validation criteria for searches, store info in search[]. */
   for (
         token_count = 0;
         (
            search[token_count][0] != SEARCH_NONE &&
            token_count < MAX_TOKENS
         );
         token_count++
      )
   {
      if (!strcmp(token[token_count][1], ""))
      {
         /*
          * Indicates a search type without a corresponding search value.
          * This may be legal in the future (ie. owhere :unique), but for
          * now it is not.
          */
         send_to_char(
            "Unable to process search without valid search criteria.\n\r",
            ch);
         return FALSE;
      }

      switch (search[token_count][0])
      {
         case SEARCH_MATERIAL:
         case SEARCH_NAME:
         case SEARCH_OWNER:
         case SEARCH_STRING:
         {
            /* Entirely dependent on argument string stored in token[]. */
            continue;
         }
         case SEARCH_DAMCLASS:
         {
            temp =
               association_table_lookup(
                  token[token_count][1],
                  damage_class_table);

            if (temp == -1)
            {
               /* Invalid damage class, help the people out. */
               sprintf(
                  buf,
                  "No such damage class: %s.\n\r",
                  token[token_count][1]);
               send_to_char(buf, ch);
               send_to_char("Valid damage classes are:\n\r", ch);
               for (
                     temp=0;
                     damage_class_table[temp].name != NULL;
                     temp++
                  )
               {
                  sprintf(buf, "%s\n\r", damage_class_table[temp].name);
                  send_to_char(buf, ch);
               }
               return FALSE;
            }

            /* We have a valid damage class. */
            search[token_count][1] = temp;
            continue;
         }
         case SEARCH_WEAPON:
         {
            /*
             * Note the use of the existing weapon_lookup function, which
             * relies on the pre-existing weapon_table.  This is fine, but
             * it doesn't have any information on exotic weapons, so you
             * can't currently search explicitly for exotics.  We'll fix it
             * later.
             */

            temp = weapon_lookup(token[token_count][1]);

            if (temp == -1)
            {
               /* No such weapon class, help the people out. */
               sprintf(
                  buf,
                  "No such weapon class: %s.\n\r",
                  token[token_count][1]);
               send_to_char(buf, ch);
               send_to_char("Valid weapon classes are:\n\r", ch);
               for (temp=0; weapon_table[temp].name != NULL; temp++)
               {
                  sprintf(buf, "%s\n\r", weapon_table[temp].name);
                  send_to_char(buf, ch);
               }
               return FALSE;
            }

            /* We have a valid weapon class. */
            search[token_count][1] = weapon_table[temp].type;
            continue;
         }
         case SEARCH_ITEM:
         {
            /*
             * Note use of pre-existing item_lookup function.  Maybe this
             *  should be switched to an association_lookup for all
             *  instances, rather than having this oddball.
             */

            temp = item_lookup(token[token_count][1]);

            if (temp == -1)
            {
               /* No such item type, help the people out. */
               sprintf(
                  buf,
                  "No such item type: %s.\n\r",
                  token[token_count][1]);
               send_to_char(buf, ch);
               send_to_char("Valid item types are:\n\r", ch);
               for (temp=0; item_table[temp].name != NULL; temp++)
               {
                  sprintf(buf, "%s\n\r", item_table[temp].name);
                  send_to_char(buf, ch);
               }
               return FALSE;
            }

            /* We have a valid item type. */
            search[token_count][1] = temp;
            continue;
         }
         case SEARCH_SLOT:
         {
            temp =
               association_table_lookup(
                  token[token_count][1],
                  wear_slot_table);

            if (temp == -1)
            {
               /* No such wear slot, help the people out. */
               sprintf(buf, "No such slot: %s.\n\r", token[token_count][1]);
               send_to_char(buf, ch);
               send_to_char("Valid slots are:\n\r", ch);
               for (temp=0; wear_slot_table[temp].name != NULL; temp++)
               {
                  sprintf(buf, "%s\n\r", wear_slot_table[temp].name);
                  send_to_char(buf, ch);
               }
               return FALSE;
            }

            /* We have a valid wear slot. */
            search[token_count][1] = temp;
            continue;
         }
         case SEARCH_AFFECT:
         {
            temp =
               association_table_lookup(
                  token[token_count][1],
                  affect_type_table);

            if (temp == -1)
            {
               /* No such affect, help the people out. */
               sprintf(buf, "No such affect: %s.\n\r", token[token_count][1]);
               send_to_char(buf, ch);
               send_to_char("Valid affects are:\n\r", ch);
               for (temp=0; affect_type_table[temp].name != NULL; temp++)
               {
                  sprintf(buf, "%s\n\r", affect_type_table[temp].name);
                  send_to_char(buf, ch);
               }
               return FALSE;
            }

            /* We have a valid affect. */
            search[token_count][1] = temp;
            continue;
         }
         case SEARCH_FLAG:
         {
            /*
             * Flag requires separation of the flag type and the flag itself.
             */
            strcpy(
               token[token_count][1],
               one_argument(token[token_count][1], buf2));

            if (!strcmp(buf2, ""))
            {
               /* Woops, we've got no specific flag type requested. */
               send_to_char(
                  "What kind of flag do you want to search for?\n\r",
                  ch);
               send_to_char("Valid flag types are:\n\r", ch);
               for (temp=0; flag_type_table[temp].name != NULL; temp++)
               {
                  sprintf(buf, "%s\n\r", flag_type_table[temp].name);
                  send_to_char(buf, ch);
               }
               return FALSE;
            }

            for (temp = 0; flag_type_table[temp].name != NULL; temp++)
            {
               if (!str_prefix(buf2, flag_type_table[temp].name))
               {
                  break;
               }
            }

            if (flag_type_table[temp].name == NULL)
            {
               /* No such flag type, help the people out. */
               sprintf(buf, "No such flag type: %s.\n\r", buf2);
               send_to_char(buf, ch);
               send_to_char("Valid flag types are:\n\r", ch);
               for (temp=0; flag_type_table[temp].name != NULL; temp++)
               {
                  sprintf(buf, "%s\n\r", flag_type_table[temp].name);
                  send_to_char(buf, ch);
               }
               return FALSE;
            }

            /*
             * We have a valid flag type.  Now we just have to set up the
             *  appropriate search criteria.
             */
            search[token_count][1] = flag_type_table[temp].flag_location;
            strcpy(buf2, flag_type_table[temp].name);

            if (flag_type_table[temp].flag != NULL)
            {
               search[token_count][2] =
                  flag_lookup(
                     token[token_count][1],
                     flag_type_table[temp].flag);
            }

            if (
                  search[token_count][2] == 0 &&
                  flag_type_table[temp].flag2 != NULL
               )
            {
               search[token_count][3] =
                  flag_lookup(
                     token[token_count][1],
                     flag_type_table[temp].flag2);
            }

            if (search[token_count][2] == 0 && search[token_count][3] == 0)
            {
               /* No such flag of that type, help the people out. */
               sprintf(
                  buf,
                  "No such %s flag %s.\n\r",
                  buf2,
                  token[token_count][1]);
               send_to_char(buf, ch);
               sprintf(buf, "Valid %s flags are:\n\r", buf2);
               send_to_char(buf, ch);
               for (
                     temp2=0;
                     (flag_type_table[temp].flag)[temp2].name != NULL;
                     temp2++
                   )
               {
                  sprintf(
                     buf,
                     "%s\n\r",
                     (flag_type_table[temp].flag)[temp2].name);
                  send_to_char(buf, ch);
               }
               if (flag_type_table[temp].flag2 != NULL)
               {
                  for (
                        temp2=0;
                        (flag_type_table[temp].flag2)[temp2].name != NULL;
                        temp2++
                     )
                  {
                     sprintf(
                        buf,
                        "%s\n\r",
                        (flag_type_table[temp].flag2)[temp2].name);
                     send_to_char(buf, ch);
                  }
               }
               return FALSE;
            }
            continue;
         }
         case SEARCH_ORIGIN:
         {
            for (pArea = area_first; pArea != NULL; pArea = pArea->next)
            {
               /* Loop through areas and capture vnums of area that matches */
               if (!str_prefix(token[token_count][1], pArea->name))
               {
                  search[token_count][1] = pArea->min_vnum;
                  search[token_count][2] = pArea->max_vnum;
                  break;
               }
            }

            if (pArea == NULL)
            {
               /*
                * No matching area, but can't help here because the list is
                * too long.
                */
               sprintf(buf, "No such area: %s\n\r", token[token_count][1]);
               send_to_char(buf, ch);
               send_to_char(
                  "Use the Area command to see available areas.\n\r",
                  ch);
               return FALSE;
            }
            continue;
         }
         case SEARCH_SPELL:
         {
            temp = skill_lookup(token[token_count][1]);

            if
            (
               temp >= MAX_SKILL ||
               temp < 1          ||
               skill_table[temp].name == NULL
            )
            {
               sprintf(buf, "No such spell: %s\n\r", token[token_count][1]);
               send_to_char(buf, ch);
               return FALSE;
            }

            if (skill_table[temp].spell_fun == spell_null)
            {
               sprintf(
                  buf,
                  "%s is a skill, not a spell!\n\r",
                  token[token_count][1]);
               send_to_char(buf, ch);
               return FALSE;
            }

            search[token_count][1] = temp;
            continue;
         }
         case SEARCH_LEVEL:
         case SEARCH_LIMIT:
         case SEARCH_COST:
         case SEARCH_VNUM:
         {
            search[token_count][1] = atoi(token[token_count][1]);

            if (strcmp(token[token_count][2], ""))
            {
               search[token_count][2] = atoi(token[token_count][2]);
            } else
            {
               /* Only one argument */
               search[token_count][2] = -1;
            }

            if (
                  (
                     search[token_count][1] == 0 &&
                     strcmp(token[token_count][1], "0")
                  ) ||
                  (
                     search[token_count][2] == 0 &&
                     strcmp(token[token_count][2], "0")
                  )
               )
            {
               send_to_char("Bad argument in search.\n\r", ch);
               send_to_char(
                  "Please use only numbers in the range: ie. level=23-30\n\r",
                  ch);
               return FALSE;
            }

            switch (search[token_count][0])
            {
               case SEARCH_LEVEL:
               {
                  /* Level specific validation step */
                  if (
                        search[token_count][1] < 1 ||
                        search[token_count][1] > MAX_LEVEL ||
                        search[token_count][2] < -1 ||
                        search[token_count][2] > MAX_LEVEL
                     )
                  {
                     send_to_char("Illegal levels in argument.\n\r", ch);
                     return FALSE;
                  }

                  break;
               }
               case SEARCH_LIMIT:
               case SEARCH_COST:
               case SEARCH_VNUM:
               {
                  /* Limit/Cost specific validation step */
                  if (search[token_count][1] < 0)
                  {
                     search[token_count][1] = 0;
                  }

                  break;
               }
            }

            if (
                  search[token_count][2] != -1 &&
                  search[token_count][2] < search[token_count][1]
               )
            {
               /* They put the bigger number first */
               sprintf(
                  buf,
                  "Your %ss are backwards, please put "
                  "the smaller number first.\n\r",
                  search_type_table[search[token_count][0]].name);
               send_to_char(buf, ch);
               return FALSE;
            }
            continue;
         }
         default:
         {
            send_to_char("Bad search type.  Please notify coders.\n\r", ch);
            return FALSE;
         }
      }
   }

   return TRUE;
}

void do_owhere(CHAR_DATA *ch, char *argument )
{
   char buf[MAX_INPUT_LENGTH];
   char token[MAX_TOKENS][MAX_TOKEN_ELEMENTS][MAX_STRING_LENGTH];
   BUFFER *buffer;
   OBJ_DATA *obj;
   OBJ_DATA *in_obj;
   AFFECT_DATA *paf;
   bool newfound;
   bool found;
   int search[MAX_TOKENS][MAX_TOKEN_ELEMENTS + 1];
   int count, count2;
   int number, max_found;

   found = FALSE;
   newfound = FALSE;
   number = 0;
   max_found = 200;

   /* Initialize variables */
   for (count = 0; count < MAX_TOKENS; count++)
   {
      for (count2 = 0; count2 < MAX_TOKEN_ELEMENTS; count2++)
      {
         strcpy(token[count][count2], "");
         if (count2 == 0)
         {
            search[count][count2] = SEARCH_NONE;
         } else
         {
            search[count][count2] = -1;
         }
      }
   };

   search[MAX_TOKENS][MAX_TOKEN_ELEMENTS + 1] = -1;
   count = 0;
   count2 = 0;

   buffer = new_buf();

   if (argument[0] == '\0')
   {
      send_to_char("Find what?\n\r",ch);
      return;
   }

   /* Old Style vs. New Style */
   if (tokenize(ch, argument, token))
   {
      /* New style, now process the tokens into meaningful search types. */
      if (!parse_tokens(ch, token, search, SCOPE_OBJECT))
      {
         return;
      }
   } else
   {
      /* Old Style */
      search[0][0] = SEARCH_OLD;
   }

   for (obj = object_list; obj != NULL; obj = obj->next)
   {
      if (!can_see_obj(ch, obj))
      {
         /* Can't find it if you can't see it! */
         continue;
      }

      if (search[0][0] == SEARCH_OLD || search[0][0] == SEARCH_NONE)
      {
         /* Old style name only check */
         if (!is_name(argument, obj->name))
         {
            continue;
         }
      } else
      {
         /* New style searching */
         for (
               count = 0;
               (
                  search[count][0] != SEARCH_NONE &&
                  count < MAX_TOKENS
               );
            count++
            )
         {
            /*
             * Always assume a given test will fail.  Let an obj earn a spot
             *  on the list.  That's more efficient than explicitly failing
             *  things.
             */
            newfound = FALSE;

            switch (search[count][0])
            {
               case SEARCH_MATERIAL:
               {
                  /* We might want to add "metal", "wood", etc */
                  if (stristr(obj->material,token[count][1]))
                  {
                     newfound = TRUE;
                  }
                  break;
               };
               case SEARCH_DAMCLASS:
               {
                  if
                  (
                     obj->item_type == ITEM_WEAPON &&
                     attack_table[obj->value[3]].damage == search[count][1]
                  )
                  {
                     newfound = TRUE;
                  }
                  break;
               }
               case SEARCH_NAME:
               {
                  if (is_name(token[count][1], obj->name))
                  {
                     newfound = TRUE;
                  }
                  break;
               }
               case SEARCH_WEAPON:
               {
                  if
                  (
                     obj->item_type == ITEM_WEAPON &&
                     obj->value[0] == search[count][1]
                  )
                  {
                     newfound = TRUE;
                  }
                  break;
               }
               case SEARCH_ITEM:
               {
                  if (obj->item_type == search[count][1])
                  {
                     newfound = TRUE;
                  }
                  break;
               }
               case SEARCH_SLOT:
               {
                  if (obj->wear_flags & search[count][1])
                  {
                     newfound = TRUE;
                  }
                  break;
               }
               case SEARCH_AFFECT:
               {
                  /* Figure out which set of affects to scan through */
                  if (obj->enchanted)
                  {
                     paf = obj->affected;
                  } else
                  {
                     paf = obj->pIndexData->affected;
                  }

                  /* Now sift through them looking for one that matches */
                  for ( ; paf != NULL; paf = paf->next)
                  {
                     if (paf->location == search[count][1])
                     {
                        newfound = TRUE;
                        break;
                     }
                  }
                  break;
               }
               case SEARCH_FLAG:
               {
                  /* SEARCH_FLAG could look in affects or, in the case of
                   *  weapons, in value[4] for the weapon flags.  Check
                   *  value[4] first since it's a special case and easily
                   *  tested.
                   */
                  if (
                        search[count][1] == TO_WEAPON &&
                        obj->item_type == ITEM_WEAPON &&
                        obj->value[4] &&
                        search[count][2] & obj->value[4]
                     )
                  {
                     newfound = TRUE;
                     break;
                  }

                  /* Same sort of thing for Extra flags, could be in there as
                   *  an affect OR in the extra flags
                   */
                  if (
                        search[count][1] == TO_OBJECT &&
                        (
                           (
                              obj->extra_flags &&
                              search[count][2] & obj->extra_flags
                           ) ||
                           (
                              obj->extra_flags2 &&
                              search[count][3] & obj->extra_flags2
                           )
                        )
                     )
                  {
                     newfound = TRUE;
                     break;
                  }

                  /* If not a weapon flag check, this is very similar to
                   *  SEARCH_AFFECT - start by picking a set of affects
                   */
                  if (obj->enchanted)
                  {
                     paf = obj->affected;
                  } else
                  {
                     paf = obj->pIndexData->affected;
                  }

                  for ( ; paf != NULL; paf= paf->next)
                  {
                     /* Unlike SEARCH_AFFECT, we have to see if the location of
                      *  the bitvectors on the affect matches, THEN test them
                      *  for a bit match
                      */
                     if (paf->where == search[count][1])
                     {
                        if (
                              search[count][2] != 0 &&
                              search[count][2] & paf->bitvector
                           )
                        {
                            newfound = TRUE;
                            break;
                        } else
                        if (
                              search[count][3] != 0 &&
                              search[count][3] & paf->bitvector2
                           )
                        {
                           newfound = TRUE;
                           break;
                        }
                     }
                  }
                  break;
               }
               case SEARCH_ORIGIN:
               {
                  if (
                        obj->pIndexData->vnum >= search[count][1] &&
                        obj->pIndexData->vnum <= search[count][2]
                     )
                  {
                     newfound = TRUE;
                  }
                  break;
               }
               case SEARCH_OWNER:
               {
                  /* str_prefix is used here so that capitals don't matter */
                  if (!str_prefix(token[count][1], obj->owner))
                  {
                     newfound = TRUE;
                     break;
                  }

                  for (count2 = 0; count2 < MAX_OWNERS; count2++)
                  {
                     if (
                           (obj->prev_owners[count2][0] != '\0') &&
                           !str_prefix(
                              token[count][1],
                              obj->prev_owners[count2])
                        )
                     {
                        newfound = TRUE;
                        break;
                     } else
                     {
                        /* We've run out of previous owners */
                        break;
                     }
                  }
                  break;
               }
               case SEARCH_SPELL:
               {
                  /* Spell searching requires different handling for the
                   *  different types of magical items.  Icky.
                   */
                  if (
                        obj->item_type == ITEM_SCROLL ||
                        obj->item_type == ITEM_POTION ||
                        obj->item_type == ITEM_PILL   ||
                        obj->item_type == ITEM_HERB
                     )
                  {
                     for (count2 = 1; count2 < 5; count2++)
                     {
                        if (obj->value[count2] == search[count][1])
                        {
                           newfound = TRUE;
                           break;
                        }
                     }

                  } else
                  if (
                        (
                           obj->item_type == ITEM_STAFF ||
                           obj->item_type == ITEM_WAND
                        ) &&
                        obj->value[3] == search[count][1]
                     )
                  {
                     newfound = TRUE;
                  }
                  break;
               }
               case SEARCH_LEVEL:
               {
                  if (search[count][2] > 0)
                  {
                     /* Test against a range. */
                     if (
                           obj->level >= search[count][1] &&
                           obj->level <= search[count][2]
                        )
                     {
                        newfound = TRUE;
                     }
                  } else
                  {
                     /* Just checking for a single value match */
                     if(obj->level == search[count][1])
                     {
                        newfound = TRUE;
                     }
                  }
                  break;
               }
               case SEARCH_LIMIT:
               {
                  if (search[count][2] > 0)
                  {
                     /* Test against a range. */
                     if (
                           obj->pIndexData->limtotal >= search[count][1] &&
                           obj->pIndexData->limtotal <= search[count][2]
                        )
                     {
                        newfound = TRUE;
                     }
                  } else
                  {
                     /* Just checking for a single value match */
                     if (obj->pIndexData->limtotal == search[count][1])
                     {
                        newfound = TRUE;
                     }
                  }
                  break;
               }
               case SEARCH_COST:
               {
                  if (search[count][2] > 0)
                  {
                     /* Test against a range. */
                     if (
                           obj->cost >= search[count][1] &&
                           obj->cost <= search[count][2]
                        )
                     {
                        newfound = TRUE;
                     }
                  } else
                  {
                     /* Just checking for a single value match */
                     if (obj->cost == search[count][1])
                     {
                        newfound = TRUE;
                     }
                  }
                  break;
               }
               case SEARCH_VNUM:
               {
                  if (search[count][2] > 0)
                  {
                     /* Test against a range. */
                     if (
                           obj->pIndexData->vnum >= search[count][1] &&
                           obj->pIndexData->vnum <= search[count][2]
                        )
                     {
                        newfound = TRUE;
                     }
                  } else
                  {
                     /* Just checking for a single value match */
                     if (obj->pIndexData->vnum == search[count][1])
                     {
                        newfound = TRUE;
                     }
                  }
                  break;
               }
               case SEARCH_STRING:
               {
                  if (
                        stristr( obj->short_descr, token[count][1] ) ||
                        stristr( obj->description, token[count][1] )
                     )
                  {
                     newfound = TRUE;
                  }
                  break;
               }
               default:
               {
                  send_to_char("Unknown search type.  Please notify coders.\n\r", ch);
                  newfound = FALSE;
                  break;
               }
            }

            if (!newfound)
            {
               /* Item failed this check, no need to keep testing. */
               break;
            }
         }

         if (!newfound)
         {
            /* Item failed, move on to next item */
            continue;
         }
      }

      /* From here on, owhere is unchanged */

      found = TRUE;
      number++;

      for ( in_obj = obj; in_obj->in_obj != NULL; in_obj = in_obj->in_obj )
      ;

      if ( in_obj->carried_by != NULL && can_see(ch, in_obj->carried_by)
      &&   in_obj->carried_by->in_room != NULL)
      sprintf( buf, "%3d) %s is carried by %s [Room %d]\n\r",
      number, obj->short_descr, PERS(in_obj->carried_by, ch),
      in_obj->carried_by->in_room->vnum );
      else if (in_obj->in_room != NULL && can_see_room(ch, in_obj->in_room))
      sprintf( buf, "%3d) %s is in %s [Room %d]\n\r",
      number, obj->short_descr, in_obj->in_room->name,
      in_obj->in_room->vnum);
      else
      sprintf( buf, "%3d) %s is somewhere\n\r", number, obj->short_descr);

      buf[0] = UPPER(buf[0]);
      add_buf(buffer, buf);

      if (number >= max_found)
      break;
   }

   if ( !found )
   send_to_char( "Nothing like that in heaven or earth.\n\r", ch );
   else
   page_to_char(buf_string(buffer), ch);

   free_buf(buffer);
}

void do_mwhere( CHAR_DATA *ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   BUFFER *buffer;
   CHAR_DATA *victim;
   bool found;
   int count = 0;

   if ( argument[0] == '\0' )
   {
      DESCRIPTOR_DATA *d;

      /* show characters logged */

      buffer = new_buf();
      for (d = descriptor_list; d != NULL; d = d->next)
      {
         if (d->character != NULL && d->connected == CON_PLAYING
         &&  d->character->in_room != NULL && can_see(ch, d->character)
         &&  can_see_room(ch, d->character->in_room))
         {
            victim = d->character;
            count++;
            if (d->original != NULL)
            sprintf(buf, "%3d) %s (in the body of %s) is in %s [%d]\n\r",
            count, d->original->name, victim->short_descr,
            victim->in_room->name, victim->in_room->vnum);
            else
            sprintf(buf, "%3d) %s is in %s [%d]\n\r",
            count, victim->name, victim->in_room->name,
            victim->in_room->vnum);
            add_buf(buffer, buf);
         }
      }

      page_to_char(buf_string(buffer), ch);
      free_buf(buffer);
      return;
   }

   found = FALSE;
   buffer = new_buf();
   for ( victim = char_list; victim != NULL; victim = victim->next )
   {
      if ( victim->in_room != NULL
      &&   can_see(ch, victim)
      &&   (is_name( argument, victim->name ) ||
      (house_lookup(argument) == victim->house
      && victim->house != 0)))
      {
         found = TRUE;
         count++;
         sprintf( buf, "%3d) [%5d] %-28s [%5d] %s\n\r", count,
         IS_NPC(victim) ? victim->pIndexData->vnum : 0,
         IS_NPC(victim) ? victim->short_descr : victim->name,
         victim->in_room->vnum,
         victim->in_room->name );
         add_buf(buffer, buf);
      }
   }

   if ( !found )
   act( "You didn't find any $T.", ch, NULL, argument, TO_CHAR );
   else
   page_to_char(buf_string(buffer), ch);

   free_buf(buffer);

   return;
}



void do_reboo( CHAR_DATA *ch, char *argument )
{
   send_to_char( "If you want to REBOOT, spell it out.\n\r", ch );
   return;
}



void do_reboot(CHAR_DATA* ch, char* argument)
{
   extern bool merc_down;
   DESCRIPTOR_DATA* d;
   DESCRIPTOR_DATA* d_next;
   CHAR_DATA* vch;
   CHAR_DATA* vch_next;

   /*
   This will enable coders using cygwin to determine if the game
   did a normal reboot, or it crashed. If no reboot.txt file is
   present, then it crashed. Useful for use with a batch file.
   - Wicket
   */
#if defined(MSDOS) || defined(MSWINDOWS)
   char buf[MAX_STRING_LENGTH];
   append_file(ch, "reboot.txt", buf);
#endif

   save_globals();
   merc_down = TRUE;
   /* Save all pc's even if linkdead */
   for (vch = char_list; vch!= NULL; vch = vch_next)
   {
      vch_next = vch->next;
      if
      (
         IS_NPC(vch) ||
         vch->level < 1 ||
         (
            vch->desc != NULL &&
            vch->desc->connected != CON_PLAYING
         )
      )
      {
         continue;
      }
      affect_strip(vch, gsn_plague);
      affect_strip(vch, gsn_poison);
      affect_strip(vch, gsn_blindness);
      affect_strip(vch, gsn_sleep);
      affect_strip(vch, gsn_curse);
      vch->hit  = vch->max_hit;
      vch->mana = vch->max_mana;
      vch->move = vch->max_move;
      update_pos(vch);
      send_to_char("*** You have been restored! ***\n\r", vch);

      /*
       * For shutdown and reboot, we will not screw the players by
       * removing things like objects with crumble quit-out code.
       * See do_quit() and extract_quitting_objects().
       */
      save_char_obj(vch);
      send_to_char("*** You are now saved. ***\n\r", vch);
      send_to_char("*** REBOOTING! ***\n\r", vch);
   }
   for (d = descriptor_list; d != NULL; d = d_next)
   {
      d_next = d->next;
      close_socket(d);
   }
   return;
}

void do_autoreboot( CHAR_DATA *ch, char *argument )
{
   int  hour;

   if (argument[0] != '\0')
   {
      if (argument[0] < 47 || argument[0] > 57)
      {
         char buf[MAX_STRING_LENGTH];
         sprintf(buf, "Autoreboot hour is %d\n\r", autoreboot_hour);
         send_to_char(buf, ch);
         return;
      }
      hour = atoi(argument);
      if (hour < 0 || hour > 23)
      {
         send_to_char("Must use a number from 0 to 23.\n\r", ch);
         return;
      }
      autoreboot_hour = hour;
      autoreboot_warning_hour = hour - 1;
      if (autoreboot_warning_hour == -1)
      autoreboot_warning_hour = 23;
      send_to_char("Autoreboot time set.\n\r", ch);
      save_globals();
      return;
   }
   if (reboot_ok)
   {
      reboot_ok = FALSE;
      send_to_char("AutoReboot disabled.\n\r", ch);
   }
   else
   {
      reboot_ok = TRUE;
      send_to_char("AutoReboot enabled.\n\r", ch);
   }
   return;
}

void do_checkip( CHAR_DATA *ch, char *argument)
{
   char buf[MAX_STRING_LENGTH];
   char arg[MAX_INPUT_LENGTH];
   BUFFER *buffer;
   BAN_DATA *pban;

   one_argument( argument, arg );

   if (!str_cmp(arg, "all")){
      resolve_no_ip = !resolve_no_ip;
      if (resolve_no_ip == FALSE)
      send_to_char("No longer resolving any ips\n\r", ch);
      else
      send_to_char("Now resolving ips again.\n\r", ch);
      return;
   }

   if ( arg[0] == '\0' )
   {
      if (ban_list == NULL)
      {
         send_to_char("No IPs not being resolved at this time.\n\r", ch);
         return;
      }
      buffer = new_buf();

      add_buf(buffer, "Unchecked IPs\n\r");
      for (pban = ban_list;pban != NULL;pban = pban->next)
      {
         if (IS_SET(pban->ban_flags, BAN_IPCHECKING))
         {
            sprintf(buf, "%s%s%s\n\r",
            IS_SET(pban->ban_flags, BAN_PREFIX) ? "*" : "",
            pban->name,
            IS_SET(pban->ban_flags, BAN_SUFFIX) ? "*" : "");
            add_buf(buffer, buf);
         }
      }

      page_to_char( buf_string(buffer), ch );
      free_buf(buffer);

      return;
   }

   sprintf(buf, "%s ipcheck", arg);
   ban_site(ch, buf, TRUE);
   /*
   if (check_ip)
   {
   check_ip = FALSE;
   send_to_char("IP resolving disabled.\n\r", ch);
   }
   else
   {
   check_ip = TRUE;
   send_to_char("IP resolving enabled.\n\r", ch);
   }*/
   return;
}

void do_getip( CHAR_DATA *ch, char *argument)
{
#if defined(MSDOS) || defined(MSWINDOWS)
   send_to_char("You cannot use this function in the DOS port.\n\r", ch);
#else

   DESCRIPTOR_DATA *d;
   struct hostent *from;
   char buf[MAX_STRING_LENGTH];
   char arg[MAX_INPUT_LENGTH];
   struct sockaddr_in sock;
   int addrs, ssa, ssb, ssc, ssd;
   char sse;

   one_argument(argument, arg);

   if (arg[0] == '\0')
   {
      send_to_char("Resolve IP for whom?\n\r", ch);
      return;
   }

   for ( d = descriptor_list; d != NULL; d = d->next )
   {
      if ( d->character != NULL && can_see( ch, d->character )
      && (    is_name(arg, d->character->name)
      || (d->original &&
      is_name(arg, d->original->name))))
      {
         sprintf(buf, "%s", d->host);
         if (sscanf(buf, "%d%c%d%c%d%c%d", &ssa, &sse, &ssb, &sse, &ssc, &sse, &ssd) == 7)
         {
            addrs = (ssa << 24) | (ssb << 16) | (ssc << 8) | (ssd);
            sock.sin_addr.s_addr = htonl(addrs); /* Reverse */
            from = gethostbyaddr( (char *) &sock.sin_addr, sizeof(sock.sin_addr), AF_INET );
         }
         else
         from = NULL;
         free_string(d->host);
         d->host = str_dup( from ? from->h_name : buf );
         sprintf
         (
            buf,
            "(%s) retrieved for %s.\n\r",
            d->host,
            (
               d->original ?
               d->original->name :
               (
                  d->character ?
                  d->character->name :
                  "(none)"
               )
            )
         );
         send_to_char(buf, ch);
         return;
      }
   }

   send_to_char("No one by that name is connected.\n\r", ch);
#endif

   return;
}

void do_shutdow( CHAR_DATA *ch, char *argument )
{
   send_to_char( "If you want to SHUTDOWN, spell it out.\n\r", ch );
   return;
}



void do_shutdown( CHAR_DATA *ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   extern bool merc_down;
   DESCRIPTOR_DATA* d;
   DESCRIPTOR_DATA* d_next;
   CHAR_DATA* vch;
   CHAR_DATA* vch_next;

   sprintf(buf, "Shutting down now!");
   append_file(ch, SHUTDOWN_FILE, buf);
   strcat(buf, "\n\r");
   if (ch->invis_level < LEVEL_HERO)
   {
      do_echo(ch, buf);
   }
   merc_down = TRUE;
   /* Save all pc's, including linkdeads */
   for (vch = char_list; vch!= NULL; vch = vch_next)
   {
      vch_next = vch->next;
      if
      (
         IS_NPC(vch) ||
         vch->level < 1 ||
         (
            vch->desc != NULL &&
            vch->desc->connected != CON_PLAYING
         )
      )
      {
         continue;
      }
      /*
       * For shutdown and reboot, we will not screw the players by
       * removing things like objects with crumble quit-out code.
       * See do_quit() and extract_quitting_objects().
       */
      save_char_obj(vch);
   }
   for (d = descriptor_list; d != NULL; d = d_next)
   {
      d_next = d->next;
      close_socket(d);
   }
   return;
}

void do_protect( CHAR_DATA *ch, char *argument)
{
   CHAR_DATA *victim;

   if (argument[0] == '\0')
   {
      send_to_char("Protect whom from snooping?\n\r", ch);
      return;
   }

   if ((victim = get_char_world(ch, argument)) == NULL)
   {
      send_to_char("You can't find them.\n\r", ch);
      return;
   }

   if (IS_SET(victim->comm, COMM_SNOOP_PROOF))
   {
      act_new("$N is no longer snoop-proof.", ch, NULL, victim, TO_CHAR, POS_DEAD);
      send_to_char("Your snoop-proofing was just removed.\n\r", victim);
      REMOVE_BIT(victim->comm, COMM_SNOOP_PROOF);
   }
   else
   {
      act_new("$N is now snoop-proof.", ch, NULL, victim, TO_CHAR, POS_DEAD);
      send_to_char("You are now immune to snooping.\n\r", victim);
      SET_BIT(victim->comm, COMM_SNOOP_PROOF);
   }
}



void do_snoop( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   DESCRIPTOR_DATA *d;
   CHAR_DATA *victim;
   char buf[MAX_STRING_LENGTH];
   SNOOP_DATA *snoops;
   SNOOP_DATA *snoop_next;

   one_argument( argument, arg );

   if ( arg[0] == '\0' )
   {
      send_to_char( "Snoop whom?\n\r", ch );
      return;
   }

   if ( ( victim = get_char_world( ch, arg ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if ( victim->desc == NULL )
   {
      send_to_char( "No descriptor to snoop.\n\r", ch );
      return;
   }

   if ( victim == ch )
   {
      send_to_char( "Cancelling all snoops.\n\r", ch );
      wiznet("$N stops being such a snoop.",
      ch, NULL, WIZ_SNOOPS, 0, get_trust(ch));
      for ( d = descriptor_list; d != NULL; d = d->next )
      {
         snoops = d->snoops;
         while(snoops != NULL)
         {
            snoop_next = snoops->next;
            if ( snoops->snoop_by == ch->desc )
            {
               free_snoop(d, snoops);
            }
            snoops = snoop_next;
         }
      }
      return;
   }

   if (!is_room_owner(ch, victim->in_room) && ch->in_room != victim->in_room
   &&  room_is_private(victim->in_room) && !IS_TRUSTED(ch, IMPLEMENTOR))
   {
      send_to_char("That character is in a private room.\n\r", ch);
      return;
   }

   if ( get_trust( victim ) >= get_trust( ch )
   ||   IS_SET(victim->comm, COMM_SNOOP_PROOF))
   {
      send_to_char( "You failed.\n\r", ch );
      return;
   }

   if ( victim->desc != NULL )
   {
      snoops = victim->desc->snoops;
      while(snoops != NULL)
      {
         if (snoops->snoop_by == ch->desc)
         {
            send_to_char("You are already snooping them.\n\r", ch);
            return;
         }
         snoops = snoops->next;
      }
   }
   if ( ch->desc != NULL )
   {
      snoops = ch->desc->snoops;
      while(snoops != NULL)
      {
         if (snoops->snoop_by == victim->desc)
         {
            send_to_char("They are snooping you.\n\r", ch);
            return;
         }
         snoops = snoops->next;
      }
   }

   snoops = new_snoop(victim->desc);
   snoops->snoop_by = ch->desc;
   sprintf(buf, "$N starts snooping on %s",
   (IS_NPC(ch) ? victim->short_descr : victim->name));
   wiznet(buf, ch, NULL, WIZ_SNOOPS, 0, get_trust(ch));
   send_to_char( "Ok.\n\r", ch );
   return;
}



void do_switch(CHAR_DATA* ch, char* argument)
{
   char buf[MAX_STRING_LENGTH];
   char find[MAX_INPUT_LENGTH];
   CHAR_DATA* victim;

   if (argument[0] == '\0')
   {
      send_to_char("Switch into whom?\n\r", ch);
      return;
   }

   if (ch->desc == NULL)
   {
      return;
   }

   if (ch->desc->original != NULL)
   {
      send_to_char("You are already switched.\n\r", ch);
      return;
   }
   /* NPC Only */
   find[0] = '\0';
   if (argument[0] != '-')
   {
      strcat(find, "-");
   }
   strcat(find, argument);
   if ((victim = get_char_world(ch, find)) == NULL)
   {
      send_to_char("They are not here.\n\r", ch);
      return;
   }

   if (victim == ch)
   {
      send_to_char("You switch into yourself.\n\r", ch);
      return;
   }

   if (!IS_NPC(victim))
   {
      send_to_char("You can only switch into mobiles.\n\r", ch);
      return;
   }

   if
   (
      !is_room_owner(ch, victim->in_room) &&
      ch->in_room != victim->in_room &&
      room_is_private(victim->in_room) &&
      !IS_TRUSTED(ch, IMPLEMENTOR)
   )
   {
      send_to_char("That character is in a private room.\n\r", ch);
      return;
   }

   if (victim->desc != NULL)
   {
      send_to_char("Character in use.\n\r", ch);
      return;
   }

   sprintf(buf, "$N switches into %s", victim->short_descr);
   wiznet(buf, ch, NULL, WIZ_SWITCHES, 0, get_trust(ch));

   ch->desc->character = victim;
   ch->desc->original  = ch;
   victim->desc        = ch->desc;
   ch->desc            = NULL;
   /* change communications to match */
   if (ch->prompt != NULL)
   {
      free_string(victim->prompt);
      victim->prompt = str_dup(ch->prompt);
   }
   victim->comm       = ch->comm;
   victim->comm2      = ch->comm2;
   victim->lines      = ch->lines;
   victim->wiznet     = ch->wiznet;
   victim->wiznet2    = ch->wiznet2;
   victim->conference = ch->conference;
   send_to_char( "Ok.\n\r", victim );
   return;
}



void do_return(CHAR_DATA* ch, char* argument)
{
   char buf[MAX_STRING_LENGTH];

   if
   (
      !IS_NPC(ch) &&
      (
         ch->pcdata->spirit_room != NULL ||
         is_affected(ch, gsn_spiritwalk)
      )
   )
   {
      send_to_char("You return to your body.\n\r", ch);
      affect_strip(ch, gsn_spiritwalk);
      if (ch->pcdata->spirit_room != ch->in_room)
      {
         WAIT_STATE(ch, 60);
      }
      ch->pcdata->spirit_room = NULL;
      return;
   }

   if (!IS_IMMORTAL(ch))
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }

   if (ch->desc == NULL)
   {
      return;
   }

   if (ch->desc->original == NULL)
   {
      send_to_char("You are not switched.\n\r", ch);
      return;
   }

   send_to_char
   (
      "You return to your original body. Type replay to see any missed tells."
      "\n\r",
      ch
   );
   if (ch->prompt != NULL)
   {
      free_string(ch->prompt);
      ch->prompt = NULL;
   }

   sprintf(buf, "$N returns from %s.", ch->short_descr);
   wiznet
   (
      buf,
      ch->desc->original,
      NULL,
      WIZ_SWITCHES,
      0,
      get_trust(ch)
   );
   ch->desc->character       = ch->desc->original;
   ch->desc->original        = NULL;
   ch->desc->character->desc = ch->desc;
   ch->desc                  = NULL;
   if (IS_NPC(ch))
   {
      ch->conference = 0;
      ch->comm       = 0;
      ch->comm2      = 0;
      ch->lines      = 0;
      ch->conference = 0;
      ch->wiznet     = 0;
      ch->wiznet2    = 0;
   }

   return;
}

/* trust levels for load and clone */
bool obj_check (CHAR_DATA *ch, OBJ_DATA *obj)
{
   if (IS_TRUSTED(ch, GOD)
   || (IS_TRUSTED(ch, IMMORTAL) && obj->level <= 20 && obj->cost <= 1000)
   || (IS_TRUSTED(ch, DEMI)        && obj->level <= 10 && obj->cost <= 500)
   || (IS_TRUSTED(ch, ANGEL)    && obj->level <=  5 && obj->cost <= 250)
   || (IS_TRUSTED(ch, AVATAR)   && obj->level ==  0 && obj->cost <= 100))
   return TRUE;
   else
   return FALSE;
}

/* for clone, to insure that cloning goes many levels deep */
void recursive_clone(CHAR_DATA *ch, OBJ_DATA *obj, OBJ_DATA *clone)
{
   OBJ_DATA *c_obj, *t_obj;


   for (c_obj = obj->contains; c_obj != NULL; c_obj = c_obj->next_content)
   {
      if (obj_check(ch, c_obj))
      {
         t_obj = create_object(c_obj->pIndexData, 0);
         clone_object(c_obj, t_obj);
         obj_to_obj(t_obj, clone);
         recursive_clone(ch, c_obj, t_obj);
      }
   }
}

/* command that is similar to load */
void do_clone(CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   char *rest;
   CHAR_DATA *mob;
   OBJ_DATA  *obj;
   char find[MAX_INPUT_LENGTH];


   rest = one_argument(argument, arg);

   if (arg[0] == '\0')
   {
      send_to_char("Clone what?\n\r", ch);
      return;
   }

   if (!str_prefix(arg, "object"))
   {
      mob = NULL;
      obj = get_obj_here(ch, rest);
      if (obj == NULL)
      {
         send_to_char("You don't see that here.\n\r", ch);
         return;
      }
   }
   else if (!str_prefix(arg, "mobile") || !str_prefix(arg, "character"))
   {
      obj = NULL;

      /* NPC Only */
      find[0] = '\0';
      if (rest[0] != '-')
      {
         strcat(find, "-");
      }
      strcat(find, rest);
      mob = get_char_room(ch, find);
      if (mob == NULL)
      {
         send_to_char("You don't see that here.\n\r", ch);
         return;
      }
   }
   else /* find both */
   {
      /* NPC Only */
      find[0] = '\0';
      if (argument[0] != '-')
      {
         strcat(find, "-");
      }
      strcat(find, argument);
      mob = get_char_room(ch, find);
      obj = get_obj_here(ch, argument);
      if (mob == NULL && obj == NULL)
      {
         send_to_char("You don't see that here.\n\r", ch);
         return;
      }
   }

   /* clone an object */
   if (obj != NULL)
   {
      OBJ_DATA *clone;

      if (!obj_check(ch, obj))
      {
         send_to_char(
         "Your powers are not great enough for such a task.\n\r", ch);
         return;
      }

      clone = create_object(obj->pIndexData, 0);
      clone_object(obj, clone);
      if (obj->carried_by != NULL)
      obj_to_char(clone, ch);
      else
      obj_to_room(clone, ch->in_room);
      recursive_clone(ch, obj, clone);

      act("$n has created $p.", ch, clone, NULL, TO_ROOM);
      act("You clone $p.", ch, clone, NULL, TO_CHAR);
      wiznet("$N clones $p.", ch, clone, WIZ_LOAD, 0, get_trust(ch));
      return;
   }
   else if (mob != NULL)
   {
      CHAR_DATA *clone;
      OBJ_DATA *new_obj;
      char buf[MAX_STRING_LENGTH];

      if (!IS_NPC(mob))
      {
         send_to_char("You can only clone mobiles.\n\r", ch);
         return;
      }

      if ((mob->level > 20 && !IS_TRUSTED(ch, GOD))
      ||  (mob->level > 10 && !IS_TRUSTED(ch, IMMORTAL))
      ||  (mob->level >  5 && !IS_TRUSTED(ch, DEMI))
      ||  (mob->level >  0 && !IS_TRUSTED(ch, ANGEL))
      ||  !IS_TRUSTED(ch, AVATAR))
      {
         send_to_char(
         "Your powers are not great enough for such a task.\n\r", ch);
         return;
      }

      clone = create_mobile(mob->pIndexData);
      clone_mobile(mob, clone);

      for (obj = mob->carrying; obj != NULL; obj = obj->next_content)
      {
         if (obj_check(ch, obj))
         {
            new_obj = create_object(obj->pIndexData, 0);
            clone_object(obj, new_obj);
            recursive_clone(ch, obj, new_obj);
            obj_to_char(new_obj, clone);
            new_obj->wear_loc = obj->wear_loc;
         }
      }
      char_to_room(clone, ch->in_room);
      act("$n has created $N.", ch, NULL, clone, TO_ROOM);
      act("You clone $N.", ch, NULL, clone, TO_CHAR);
      sprintf(buf, "$N clones %s.", clone->short_descr);
      wiznet(buf, ch, NULL, WIZ_LOAD, 0, get_trust(ch));
      return;
   }
}

/* RT to replace the two load commands */

void do_load(CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];

   argument = one_argument(argument, arg);

   if (arg[0] == '\0')
   {
      send_to_char("Syntax:\n\r", ch);
      send_to_char("  load mob <vnum>\n\r", ch);
      send_to_char("  load obj <vnum> <level>\n\r", ch);
      if (IS_TRUSTED(ch, IMPLEMENTOR))
      {
         send_to_char("  load area <area name>\n\r", ch);
      }
      return;
   }

   if (!str_cmp(arg, "mob") || !str_cmp(arg, "char"))
   {
      do_mload(ch, argument);
      return;
   }

   if (!str_cmp(arg, "obj"))
   {
      do_oload(ch, argument);
      return;
   }

   if (
         !str_cmp(arg, "area") &&
         IS_TRUSTED(ch, IMPLEMENTOR)
      )
   {
      do_loadarea(ch, argument);
      return;
   }

   /* echo syntax */
   do_load(ch, "");
}


void do_mload( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   MOB_INDEX_DATA *pMobIndex;
   CHAR_DATA *victim;
   char buf[MAX_STRING_LENGTH];

   one_argument( argument, arg );

   if ( arg[0] == '\0' || !is_number(arg) )
   {
      send_to_char( "Syntax: load mob <vnum>.\n\r", ch );
      return;
   }

   if ( ( pMobIndex = get_mob_index( atoi( arg ) ) ) == NULL )
   {
      send_to_char( "No mob has that vnum.\n\r", ch );
      return;
   }

   victim = create_mobile( pMobIndex );
   char_to_room( victim, ch->in_room );
   act( "$n has created $N!", ch, NULL, victim, TO_ROOM );
   sprintf(buf, "$N loads %s.", victim->short_descr);
   wiznet(buf, ch, NULL, WIZ_LOAD, 0, get_trust(ch));
   send_to_char( "Ok.\n\r", ch );
   return;
}



void do_oload( CHAR_DATA *ch, char *argument )
{
   char arg1[MAX_INPUT_LENGTH] , arg2[MAX_INPUT_LENGTH];
   OBJ_INDEX_DATA *pObjIndex;
   OBJ_DATA *obj;
   int level;

   argument = one_argument( argument, arg1 );
   one_argument( argument, arg2 );

   if ( arg1[0] == '\0' || !is_number(arg1))
   {
      send_to_char( "Syntax: load obj <vnum> <level>.\n\r", ch );
      return;
   }

   level = get_trust(ch); /* default */

   if ( arg2[0] != '\0')  /* load with a level */
   {
      if (!is_number(arg2))
      {
         send_to_char( "Syntax: oload <vnum> <level>.\n\r", ch );
         return;
      }
      level = atoi(arg2);
      if (level < 0 || level > get_trust(ch))
      {
         send_to_char( "Level must be be between 0 and your level.\n\r", ch);
         return;
      }
   }

   if ( ( pObjIndex = get_obj_index( atoi( arg1 ) ) ) == NULL )
   {
      send_to_char( "No object has that vnum.\n\r", ch );
      return;
   }

   obj = create_object( pObjIndex, level );
   if ( CAN_WEAR(obj, ITEM_TAKE) )
   obj_to_char( obj, ch );
   else
   obj_to_room( obj, ch->in_room );
   act( "$n has created $p!", ch, obj, NULL, TO_ROOM );
   wiznet("$N loads $p.", ch, obj, WIZ_LOAD, 0, get_trust(ch));
   send_to_char( "Ok.\n\r", ch );
   return;
}

void do_loadarea( CHAR_DATA *ch, char *argument )
{
   char  area_name[MAX_INPUT_LENGTH];
   AREA_DATA* pArea;

   one_argument(argument, area_name);

   /* Syntax */
   if (area_name[0] == '\0')
   {
      BUFFER* buffer_ptr;
      buffer_ptr = new_buf();
      add_buf(
         buffer_ptr,
         "NAME\n\r"
         "       load area - Loads an area out of the area directory.\n\r"
         "\n\r");
      add_buf(
         buffer_ptr,
         "SYNOPSIS\n\r"
         "       load area <area name>\n\r"
         "\n\r");
      add_buf(
         buffer_ptr,
         "DESCRIPTION\n\r"
         "       The \"load area\" command loads an area out of the area directory.\n\r"
         "       The area cannot already be loaded.\n\r"
         "\n\r");
      add_buf(
         buffer_ptr,
         "EXAMPLE\n\r"
         "       load area frogland.are\n\r"
         "\n\r"
         "       This command will load the area \"frogland.are\" into the MUD.\n\r");
      page_to_char(buf_string(buffer_ptr), ch);
      free_buf(buffer_ptr);
      return;
   }


   /* Verify area_name is not the name of an area already loaded. */
   for (pArea = area_first; pArea != NULL; pArea = pArea->next)
   {
      if (!str_cmp(pArea->file_name, area_name))
      {
         send_to_char("Specified area is already loaded.\n\r", ch);
         return;
      }
   }


   /* Load the area by calling into db.c */
   dynamically_load_area(ch, area_name);

   return;
}



void do_purge( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   char buf[100];
   CHAR_DATA *victim;
   OBJ_DATA *obj;
   DESCRIPTOR_DATA *d;
   ROOM_AFFECT_DATA *raf;
   ROOM_AFFECT_DATA *next_raf;

   one_argument( argument, arg );

   if ( arg[0] == '\0' )
   {
      /* 'purge' */
      CHAR_DATA *vnext;
      OBJ_DATA  *obj_next;

      for ( victim = ch->in_room->people; victim != NULL; victim = vnext )
      {
         vnext = victim->next_in_room;
         if ( IS_NPC(victim) && !IS_SET(victim->act, ACT_NOPURGE)
         &&   victim != ch /* safety precaution */ )
         extract_char( victim, TRUE );
      }

      for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
      {
         obj_next = obj->next_content;
         if (!IS_OBJ_STAT(obj, ITEM_NOPURGE))
         extract_obj( obj, FALSE );
      }

      raf = ch->in_room->affected;
      while(raf != NULL){
         next_raf=raf->next;
         affect_remove_room(ch->in_room, raf);
         raf=next_raf;
      }
      act( "$n purges the room!", ch, NULL, NULL, TO_ROOM);
      send_to_char( "Ok.\n\r", ch );
      return;
   }

   if ( ( victim = get_char_world( ch, arg ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if ( !IS_NPC(victim) )
   {

      if (ch == victim)
      {
         send_to_char("Ho ho ho.\n\r", ch);
         return;
      }

      if (get_trust(ch) <= get_trust(victim))
      {
         send_to_char("Maybe that wasn't a good idea...\n\r", ch);
         sprintf(buf, "%s tried to purge you!\n\r", ch->name);
         send_to_char(buf, victim);
         return;
      }

      act("$n disintegrates $N.", ch, NULL, victim, TO_NOTVICT);

      if (victim->level > 1)
      save_char_obj( victim );
      d = victim->desc;
      extract_char( victim, TRUE );
      if ( d != NULL )
      close_socket( d );

      return;
   }

   act( "$n purges $N.", ch, NULL, victim, TO_NOTVICT );
   extract_char( victim, TRUE );
   return;
}



void do_advance( CHAR_DATA *ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   int level;
   int iLevel;

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );

   if ( arg1[0] == '\0' || arg2[0] == '\0' || !is_number( arg2 ) )
   {
      send_to_char( "Syntax: advance <char> <level>.\n\r", ch );
      return;
   }

   if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
   {
      send_to_char( "That player is not here.\n\r", ch);
      return;
   }

   if ( IS_NPC(victim) )
   {
      send_to_char( "Not on NPC's.\n\r", ch );
      return;
   }

   if ( ( level = atoi( arg2 ) ) < 1 || level > 60 )
   {
      send_to_char( "Level must be 1 to 60.\n\r", ch );
      return;
   }

   if ( level > get_trust( ch ) )
   {
      send_to_char( "Limited to your trust level.\n\r", ch );
      return;
   }

   /*
   * Lower level:
   *   Reset to level 1.
   *   Then raise again.
   *   Currently, an imp can lower another imp.
   *   -- Swiftest
   */
   if ( level <= victim->level )
   {
      int temp_prac;

      if
      (
         victim->pcdata->race_lottery > 0 &&
         victim->pcdata->race_lottery < MAX_PC_RACE
      )
      {
         remove_node_for(victim->name, lottery_race_players[victim->pcdata->race_lottery]);
      }
      if
      (
         victim->level >= 10 &&
         !IS_IMMORTAL(victim)
      )
      {
         /* Currently counted in race counts, remove */
         lottery_race_count[victim->race]--;
      }
      send_to_char( "Lowering a player's level!\n\r", ch );
      send_to_char( "**** OOOOHHHHHHHHHH  NNNNOOOO ****\n\r", victim );
      temp_prac = victim->practice;
      victim->level    = 1;
      victim->exp_total = 1;
      victim->exp      = exp_per_level(victim);
      victim->max_hit  = 10;
      victim->max_mana = 100;
      victim->max_move = 100;
      victim->pcdata->perm_hit = 10;
      victim->pcdata->perm_mana = 100;
      victim->pcdata->perm_move = 100;
      victim->practice = 0;
      victim->train      = 0;
      victim->hit      = victim->max_hit;
      victim->mana     = victim->max_mana;
      victim->move     = victim->max_move;
      advance_level( victim, TRUE );
   }
   else
   {
      send_to_char( "Raising a player's level!\n\r", ch );
      send_to_char( "**** OOOOHHHHHHHHHH  YYYYEEEESSS ****\n\r", victim );
   }

   for ( iLevel = victim->level ; iLevel < level; iLevel++ )
   {
      victim->level += 1;
      advance_level( victim, TRUE);
   }
   sprintf(buf, "You are now level %d.\n\r", victim->level);
   send_to_char(buf, victim);
   victim->exp   = exp_per_level(victim)
   * UMAX( 1, victim->level );
   save_char_obj(victim);
   return;
}


void do_allowrp( CHAR_DATA *ch, char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;

   argument = one_argument( argument, arg1 );
   if ( arg1[0] == '\0')
   {
      send_to_char( "Allow whom powers to rp.\n\r", ch );
      return;
   }

   if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
   {
      send_to_char( "That player is not here.\n\r", ch);
      return;
   }

   if (!IS_IMMORTAL(victim))
   {
      send_to_char("Granting mortals god powers is a bad idea.\n\r", ch);
      return;
   }
   if (!IS_SET(victim->wiznet, WIZ_ALLOWRP))
   SET_BIT(victim->wiznet, WIZ_ALLOWRP);
   else
   REMOVE_BIT(victim->wiznet, WIZ_ALLOWRP);
   if (!IS_SET(victim->wiznet, WIZ_ALLOWRP))
   {
      send_to_char("Their rp powers have been taken away.\n\r", ch);
      send_to_char("Your allowrp has been removed.\n\r", victim);
   } else
   {
      send_to_char("You have been allowed special powers to rp with.\n\r", victim);
      send_to_char("You grant them special powers to rp with.\n\r", ch);
   }
   return;
}


void do_trust( CHAR_DATA *ch, char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   int level;

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );
   if ( arg1[0] == '\0' || arg2[0] == '\0' || !is_number( arg2 ) )
   {
      send_to_char( "Syntax: trust <char> <level>.\n\r", ch );
      return;
   }

   if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
   {
      send_to_char( "That player is not here.\n\r", ch);
      return;
   }

   if ( ( level = atoi( arg2 ) ) < 0 || level > 60 )
   {
      send_to_char( "Level must be 0 (reset) or 1 to 60.\n\r", ch );
      return;
   }

   if ( level > get_trust( ch ) )
   {
      send_to_char( "Limited to your trust.\n\r", ch );
      return;
   }
   if
   (
      victim->level >= 10 &&
      victim->level < LEVEL_IMMORTAL &&
      victim->trust >= LEVEL_IMMORTAL &&
      level < LEVEL_IMMORTAL
   )
   {
      /* Becoming mortal, level 10+ */
      lottery_race_count[victim->race]++;
   }
   else if
   (
      !IS_IMMORTAL(victim) &&
      level >= LEVEL_IMMORTAL
   )
   {
      /* Becoming immortal */
      lottery_race_count[victim->race]--;
   }
   victim->trust = level;
   if (IS_IMMORTAL(victim))
   {
      do_wizireport_update(victim, WIZI_UPDATE);
      remove_node_for(victim->name, lottery_race_players[victim->pcdata->race_lottery]);
   }
   else if
   (
      victim->level >= 30 &&
      (
         victim->in_room == NULL ||
         !IS_SET(victim->in_room->extra_room_flags, ROOM_1212)
      ) &&
      victim->pcdata->race_lottery > 0 &&
      victim->pcdata->race_lottery < MAX_PC_RACE &&
      !victim->house &&
      !IS_SET(victim->act2, PLR_IS_ANCIENT)
   )
   {
      remove_node_for(victim->name, lottery_race_players[victim->pcdata->race_lottery]);
      add_node(victim->name, lottery_race_players[victim->pcdata->race_lottery]);
   }
   return;
}



void do_restore( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
   CHAR_DATA *victim;
   CHAR_DATA *vch;
   DESCRIPTOR_DATA *d;

   one_argument( argument, arg );
   if (arg[0] == '\0' || !str_cmp(arg, "room"))
   {
      /* cure room */

      for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
      {
         affect_strip(vch, gsn_plague);
         affect_strip(vch, gsn_poison);
         affect_strip(vch, gsn_blindness);
         affect_strip(vch, gsn_sleep);
         affect_strip(vch, gsn_curse);

         vch->hit     = vch->max_hit;
         vch->mana    = vch->max_mana;
         vch->move    = vch->max_move;
         update_pos( vch);
         act("$n has restored you.", ch, NULL, vch, TO_VICT);
      }

      sprintf(buf, "$N restored room %d.", ch->in_room->vnum);
      wiznet(buf, ch, NULL, WIZ_RESTORE, 0, get_trust(ch));

      send_to_char("Room restored.\n\r", ch);
      return;

   }

   if ( get_trust(ch) >=  MAX_LEVEL - 3 && !str_cmp(arg, "all"))
   {
      /* cure all */

      for (d = descriptor_list; d != NULL; d = d->next)
      {
         victim = d->character;

         if (victim == NULL || IS_NPC(victim))
         continue;

         affect_strip(victim, gsn_plague);
         affect_strip(victim, gsn_poison);
         affect_strip(victim, gsn_blindness);
         affect_strip(victim, gsn_sleep);
         affect_strip(victim, gsn_curse);

         victim->hit     = victim->max_hit;
         victim->mana    = victim->max_mana;
         victim->move    = victim->max_move;
         update_pos( victim);
         if (victim->in_room != NULL)
         act("$n has restored you.", ch, NULL, victim, TO_VICT);
      }
      send_to_char("All active players restored.\n\r", ch);
      return;
   }

   if ( ( victim = get_char_world( ch, arg ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   affect_strip(victim, gsn_plague);
   affect_strip(victim, gsn_poison);
   affect_strip(victim, gsn_blindness);
   affect_strip(victim, gsn_sleep);
   affect_strip(victim, gsn_curse);
   victim->hit  = victim->max_hit;
   victim->mana = victim->max_mana;
   victim->move = victim->max_move;
   update_pos( victim );
   act( "$n has restored you.", ch, NULL, victim, TO_VICT );
   sprintf(buf, "$N restored %s",
   IS_NPC(victim) ? victim->short_descr : victim->name);
   wiznet(buf, ch, NULL, WIZ_RESTORE, 0, get_trust(ch));
   send_to_char( "Ok.\n\r", ch );
   return;
}


void do_freeze( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
   CHAR_DATA *victim;

   one_argument( argument, arg );

   if ( arg[0] == '\0' )
   {
      send_to_char( "Freeze whom?\n\r", ch );
      return;
   }

   if ( ( victim = get_char_world( ch, arg ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if ( IS_NPC(victim) )
   {
      send_to_char( "Not on NPC's.\n\r", ch );
      return;
   }

   if ( get_trust( victim ) >= get_trust( ch ) )
   {
      send_to_char( "You failed.\n\r", ch );
      return;
   }

   if ( IS_SET(victim->act, PLR_FREEZE) )
   {
      REMOVE_BIT(victim->act, PLR_FREEZE);
      send_to_char( "You can play again.\n\r", victim );
      send_to_char( "FREEZE removed.\n\r", ch );
      sprintf(buf, "$N thaws %s.", victim->name);
      wiznet(buf, ch, NULL, WIZ_PENALTIES, 0, 0);
   }
   else
   {
      SET_BIT(victim->act, PLR_FREEZE);
      send_to_char( "You can't do ANYthing!\n\r", victim );
      send_to_char( "FREEZE set.\n\r", ch );
      sprintf(buf, "$N puts %s in the deep freeze.", victim->name);
      wiznet(buf, ch, NULL, WIZ_PENALTIES, 0, 0);
   }

   save_char_obj( victim );

   return;
}



void do_log( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;

   argument = one_argument( argument, arg );

   if ( arg[0] == '\0' )
   {
      send_to_char( "Log whom?\n\r", ch );
      return;
   }

   if ( !str_cmp( arg, "all" ) )
   {
      if ( fLogAll )
      {
         fLogAll = FALSE;
         send_to_char( "Log ALL off.\n\r", ch );
      }
      else
      {
         fLogAll = TRUE;
         send_to_char( "Log ALL on.\n\r", ch );
      }
      return;
   }

   if (!str_cmp(arg, "command"))
   {
      int cmd;
      bool found = FALSE;
      char command[MAX_INPUT_LENGTH];

      one_argument( argument, command );

      for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
      {
         if ( command[0] == cmd_table[cmd].name[0]
         &&   !str_prefix( command, cmd_table[cmd].name ))
         {
            found = TRUE;
            break;
         }
      }

      if (found && !str_cmp(cmd_table[cmd].name, "log"))
      {
         send_to_char("LOG cannot be unlogged.\n\r", ch);
         return;
      }

      if (found && !str_cmp(cmd_table[cmd].name, "password"))
      {
         send_to_char("PASSWORD cannot be logged.\n\r", ch);
         return;
      }

      if (!found)
      {
         send_to_char("That command does not exist.\n\r", ch);
      } else
      {
         if (cmd_table[cmd].log == 1)
         {
            cmd_table[cmd].log = 0;
            send_to_char("Logging it as normal now.\n\r", ch);
         } else
         {
            cmd_table[cmd].log = 1;
            send_to_char("Logging it always now.\n\r", ch);
         }
      }

      return;
   }

   if (!str_cmp(arg, "mob"))
   {
      log_mobs = !log_mobs;
      if (log_mobs)
      {
         send_to_char("Now logging mobprogs.\n\r", ch);
      }
      else
      {
         send_to_char("No longer logging mobprogs.\n\r", ch);
      }
      save_globals();
      return;
   }

   if ( ( victim = get_char_world( ch, arg ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if ( IS_NPC(victim) )
   {
      send_to_char( "Not on NPC's.\n\r", ch );
      return;
   }

   /*
   * No level check, gods can log anyone.
   */
   if ( IS_SET(victim->act, PLR_LOG) )
   {
      REMOVE_BIT(victim->act, PLR_LOG);
      send_to_char( "LOG removed.\n\r", ch );
   }
   else
   {
      SET_BIT(victim->act, PLR_LOG);
      send_to_char( "LOG set.\n\r", ch );
   }

   return;
}



void do_noemote( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
   CHAR_DATA *victim;

   one_argument( argument, arg );

   if ( arg[0] == '\0' )
   {
      send_to_char( "Noemote whom?\n\r", ch );
      return;
   }

   if ( ( victim = get_char_world( ch, arg ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }


   if ( get_trust( victim ) >= get_trust( ch ) )
   {
      send_to_char( "You failed.\n\r", ch );
      return;
   }

   if ( IS_SET(victim->comm, COMM_NOEMOTE) )
   {
      REMOVE_BIT(victim->comm, COMM_NOEMOTE);
      send_to_char( "You can emote again.\n\r", victim );
      send_to_char( "NOEMOTE removed.\n\r", ch );
      sprintf(buf, "$N restores emotes to %s.", victim->name);
      wiznet(buf, ch, NULL, WIZ_PENALTIES, 0, 0);
   }
   else
   {
      SET_BIT(victim->comm, COMM_NOEMOTE);
      send_to_char( "You can't emote!\n\r", victim );
      send_to_char( "NOEMOTE set.\n\r", ch );
      sprintf(buf, "$N revokes %s's emotes.", victim->name);
      wiznet(buf, ch, NULL, WIZ_PENALTIES, 0, 0);
   }

   return;
}

void do_noguild( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
   CHAR_DATA *victim;

   one_argument( argument, arg );

   if ( arg[0] == '\0' )
   {
      send_to_char( "Noguild whom?\n\r", ch );
      return;
   }

   if ( ( victim = get_char_world( ch, arg ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }


   if ( get_trust( victim ) >= get_trust( ch ) )
   {
      send_to_char( "You failed.\n\r", ch );
      return;
   }

   if ( IS_SET(victim->act2, PLR_GUILDLESS) )
   {
      REMOVE_BIT(victim->act2, PLR_GUILDLESS);
      send_to_char( "You are welcomed back into your guild.\n\r", victim );
      send_to_char( "NOGUILD removed.\n\r", ch );
      sprintf(buf, "$N restores %s to the guild.", victim->name);
      wiznet(buf, ch, NULL, WIZ_PENALTIES, 0, 0);
   }
   else
   {
      SET_BIT(victim->act2, PLR_GUILDLESS);
      send_to_char( "You are kicked out of the guild!\n\r", victim );
      send_to_char( "NOGUILD set.\n\r", ch );
      sprintf(buf, "$N removes %s from the guild.", victim->name);
      wiznet(buf, ch, NULL, WIZ_PENALTIES, 0, 0);
   }

   return;
}


void do_noshout( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
   CHAR_DATA *victim;

   one_argument( argument, arg );

   if ( arg[0] == '\0' )
   {
      send_to_char( "Noshout whom?\n\r", ch);
      return;
   }

   if ( ( victim = get_char_world( ch, arg ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if ( IS_NPC(victim) )
   {
      send_to_char( "Not on NPC's.\n\r", ch );
      return;
   }

   if ( get_trust( victim ) >= get_trust( ch ) )
   {
      send_to_char( "You failed.\n\r", ch );
      return;
   }

   if ( IS_SET(victim->comm, COMM_NOSHOUT) )
   {
      REMOVE_BIT(victim->comm, COMM_NOSHOUT);
      send_to_char( "You can shout again.\n\r", victim );
      send_to_char( "NOSHOUT removed.\n\r", ch );
      sprintf(buf, "$N restores shouts to %s.", victim->name);
      wiznet(buf, ch, NULL, WIZ_PENALTIES, 0, 0);
   }
   else
   {
      SET_BIT(victim->comm, COMM_NOSHOUT);
      send_to_char( "You can't shout!\n\r", victim );
      send_to_char( "NOSHOUT set.\n\r", ch );
      sprintf(buf, "$N revokes %s's shouts.", victim->name);
      wiznet(buf, ch, NULL, WIZ_PENALTIES, 0, 0);
   }

   return;
}



void do_notell( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
   CHAR_DATA *victim;

   one_argument( argument, arg );

   if ( arg[0] == '\0' )
   {
      send_to_char( "Notell whom?", ch );
      return;
   }

   if ( ( victim = get_char_world( ch, arg ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if ( get_trust( victim ) >= get_trust( ch ) )
   {
      send_to_char( "You failed.\n\r", ch );
      return;
   }

   if ( IS_SET(victim->comm, COMM_NOTELL) )
   {
      REMOVE_BIT(victim->comm, COMM_NOTELL);
      send_to_char( "You can tell again.\n\r", victim );
      send_to_char( "NOTELL removed.\n\r", ch );
      sprintf(buf, "$N restores tells to %s.", victim->name);
      wiznet(buf, ch, NULL, WIZ_PENALTIES, 0, 0);
   }
   else
   {
      SET_BIT(victim->comm, COMM_NOTELL);
      send_to_char( "You can't tell!\n\r", victim );
      send_to_char( "NOTELL set.\n\r", ch );
      sprintf(buf, "$N revokes %s's tells.", victim->name);
      wiznet(buf, ch, NULL, WIZ_PENALTIES, 0, 0);
   }

   return;
}


void do_nopray( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
   CHAR_DATA *victim;

   one_argument( argument, arg );

   if ( arg[0] == '\0' )
   {
      send_to_char( "Nopray whom?", ch );
      return;
   }

   if ( ( victim = get_char_world( ch, arg ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if ( get_trust( victim ) >= get_trust( ch ) )
   {
      send_to_char( "You failed.\n\r", ch );
      return;
   }

   if ( IS_SET(victim->comm, COMM_NOPRAY) )
   {
      REMOVE_BIT(victim->comm, COMM_NOPRAY);
      send_to_char( "The gods no longer forsake you, and your prayers will be heard.\n\r", victim );
      send_to_char( "NOPRAY removed.\n\r", ch );
      sprintf(buf, "$N restores prays to %s.", victim->name);
      wiznet(buf, ch, NULL, WIZ_PENALTIES, 0, 0);
   }
   else
   {
      SET_BIT(victim->comm, COMM_NOPRAY);
      send_to_char( "The gods have forsaken you, and your prayers will not be heard.\n\r", victim );
      send_to_char( "NOPRAY set.\n\r", ch );
      sprintf(buf, "$N revokes %s's prays.", victim->name);
      wiznet(buf, ch, NULL, WIZ_PENALTIES, 0, 0);
   }

   return;
}


void do_silence( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
   CHAR_DATA *victim;

   one_argument( argument, arg );

   if ( arg[0] == '\0' )
   {
      send_to_char( "Silence whom?\n\r", ch );
      return;
   }

   if ( ( victim = get_char_world( ch, arg ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if ( get_trust( victim ) >= get_trust( ch ) )
   {
      send_to_char( "You failed.\n\r", ch );
      return;
   }

   if ( IS_SET(victim->comm, COMM_SILENCE) )
   {
      REMOVE_BIT(victim->comm, COMM_SILENCE);
      send_to_char( "You may once again communicate with others.", victim );
      send_to_char( "SILENCE removed.\n\r", ch );
      sprintf(buf, "$N removes silence from %s.", victim->name);
      wiznet(buf, ch, NULL, WIZ_PENALTIES, 0, 0);
   }
   else
   {
      SET_BIT(victim->comm, COMM_SILENCE);
      send_to_char( "You may no longer communicate with anyone.", victim );
      send_to_char( "SILENCE set.\n\r", ch );
      sprintf(buf, "$N silences %s.", victim->name);
      wiznet(buf, ch, NULL, WIZ_PENALTIES, 0, 0);
   }

   return;
}


void do_peace( CHAR_DATA *ch, char *argument )
{
   CHAR_DATA *rch;

   for ( rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room )
   {
      if ( rch->fighting != NULL )
      stop_fighting( rch, TRUE );
      if (IS_NPC(rch) && IS_SET(rch->act, ACT_AGGRESSIVE))
      REMOVE_BIT(rch->act, ACT_AGGRESSIVE);
      if (IS_NPC(rch))
      rch->last_fought = -1;
   }

   send_to_char( "Ok.\n\r", ch );
   return;
}

void do_wizlock( CHAR_DATA *ch, char *argument )
{
   extern bool wizlock;
   wizlock = !wizlock;

   if ( wizlock )
   {
      wiznet("$N has wizlocked the game.", ch, NULL, 0, 0, 0);
      send_to_char( "Game wizlocked.\n\r", ch );
   }
   else
   {
      wiznet("$N removes wizlock.", ch, NULL, 0, 0, 0);
      send_to_char( "Game un-wizlocked.\n\r", ch );
   }

   return;
}

/* RT anti-newbie code */

void do_newlock( CHAR_DATA *ch, char *argument )
{
   extern bool newlock;
   newlock = !newlock;

   if ( newlock )
   {
      wiznet("$N locks out new characters.", ch, NULL, 0, 0, 0);
      send_to_char( "New characters have been locked out.\n\r", ch );
   }
   else
   {
      wiznet("$N allows new characters back in.", ch, NULL, 0, 0, 0);
      send_to_char( "Newlock removed.\n\r", ch );
   }

   return;
}


void do_slookup( CHAR_DATA *ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   char arg[MAX_INPUT_LENGTH];
   int sn;

   one_argument( argument, arg );
   if ( arg[0] == '\0' )
   {
      send_to_char( "Lookup which skill or spell?\n\r", ch );
      return;
   }

   if ( !str_cmp( arg, "all" ) )
   {
      for ( sn = 0; sn < MAX_SKILL; sn++ )
      {
         if ( skill_table[sn].name == NULL )
         break;
         sprintf( buf, "Sn: %3d  Skill/spell: '%s'\n\r", sn, skill_table[sn].name );
         send_to_char( buf, ch );
      }
   }
   else
   {
      if ( is_number( arg ) )
      {
         sn = atoi(arg);
         /* On purpose allows 0 */
         if ( sn < 0 || sn >= MAX_SKILL)
         {
            sprintf( buf, "Skill or Spell number must be between 0 and %d.\n\r", MAX_SKILL - 1 );
            send_to_char( buf, ch );
            return;
         }
      }
      else if ( ( sn = skill_lookup( arg ) ) < 0 )  /* On purpose allows 0 */
      {
         send_to_char( "No such skill or spell.\n\r", ch );
         return;
      }

      sprintf( buf, "Sn: %3d Skill/spell: '%s'\n\r", sn, skill_table[sn].name );
      send_to_char( buf, ch );
   }

   return;
}

/* RT set replaces sset, mset, oset, and rset */

void do_set( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];

   argument = one_argument(argument, arg);

   if (arg[0] == '\0')
   {
      send_to_char
      (
         "Syntax:\n\r"
         "  set mob      <name> <field> <value>\n\r"
         "  set obj      <name> <field> <value>\n\r"
         "  set room     <room> <field> <value>\n\r"
         "  set skill    <name> <spell or skill> <value>\n\r"
         "  set maxstat  <name> <stat> <value>\n\r"
         "  set learnlvl <name> <spell or skill> <value>\n\r"
         "  set duration <name> <spell or skill> <value>\n\r"
         "  set exit     <direction> <vnum>\n\r"
         "  set ticks\n\r",
         ch
      );
      return;
   }

   if (!str_cmp(arg, "ticks"))
   {
      if
      (
         !str_cmp(argument, "on") ||
         IS_SET(ch->comm2, COMM_STAT_TIME)
      )
      {
         REMOVE_BIT(ch->comm2, COMM_STAT_TIME);
         send_to_char("Durations in 'stat' defaulting to ticks.\n\r", ch);
      }
      else
      {
         SET_BIT(ch->comm2, COMM_STAT_TIME);
         send_to_char
         (
            "Durations in 'stat' defaulting to hours and minutes.\n\r",
            ch
         );
      }
      return;
   }

   if
   (
      !str_prefix(arg, "mobile") ||
      !str_prefix(arg, "character")
   )
   {
      do_mset(ch, argument);
      return;
   }

   if
   (
      !str_prefix(arg, "skill") ||
      !str_prefix(arg, "spell")
   )
   {
      do_sset(ch, argument);
      return;
   }

   if (!str_prefix(arg, "duration"))
   {
      do_dset(ch, argument);
      return;
   }

   if (!str_prefix(arg, "learnlvl"))
   {
      do_learnlvl(ch, argument);
      return;
   }

   if (!str_prefix(arg, "object"))
   {
      do_oset(ch, argument);
      return;
   }

   if (!str_prefix(arg, "room"))
   {
      do_rset(ch, argument);
      return;
   }

   if (!str_prefix(arg, "maxstat"))
   {
      do_maxset(ch, argument);
      return;
   }

   if (!str_prefix(arg, "exit"))
   {
      do_exitset(ch, argument);
      return;
   }

   /* echo syntax */
   do_set(ch, "");
}

void do_dset( CHAR_DATA *ch, char *argument )
{
   char arg1 [MAX_INPUT_LENGTH];
   char arg2 [MAX_INPUT_LENGTH];
   char arg3 [MAX_INPUT_LENGTH];
   char arg_name [MAX_INPUT_LENGTH];
   AFFECT_DATA *af;
   CHAR_DATA *victim;
   int value;
   int sn;

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );
   argument = one_argument( argument, arg3 );

   if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
   {
      send_to_char( "Syntax:\n\r", ch);
      send_to_char( "  set duration <name> <spell or skill> <value>\n\r", ch);
      return;
   }

   if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }
   if
   (
      IS_NPC(victim) &&
      (
         victim->pIndexData->vnum == MOB_VNUM_DECOY ||
         victim->pIndexData->vnum == MOB_VNUM_MIRROR ||
         victim->pIndexData->vnum == MOB_VNUM_SHADOW
      ) &&
      arg1[0] != '-'
   )
   {
      sprintf(arg_name, "+%s", arg1);
      if
      (
         (
            victim = get_char_world(ch, arg_name)
         ) == NULL
      )
      {
         send_to_char( "They aren't here.\n\r", ch );
         return;
      }
   }

   sn   = 0;
   if ( ( sn = skill_lookup( arg2 ) ) < 0 )
   {
      send_to_char( "No such skill or spell.\n\r", ch );
      return;
   }

   if ( !is_number( arg3 ) )
   {
      send_to_char( "Value must be numeric.\n\r", ch );
      return;
   }

   value = atoi(arg3);

   if (is_affected(victim, sn))
   {
      for ( af = victim->affected; af != NULL; af = af->next )
      {
         if ( af->type == sn )
         {
            af->duration = value;
         }
      }
      send_to_char("Duration changed.\n\r", ch);
   }
   else
   {
      send_to_char( "Affect not found.\n\r", ch);
   }
   return;
}

void do_sset( CHAR_DATA *ch, char *argument )
{
   char arg1 [MAX_INPUT_LENGTH];
   char arg2 [MAX_INPUT_LENGTH];
   char arg3 [MAX_INPUT_LENGTH];
   char arg_name [MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   int value;
   int old_val;
   int sn;
   bool fAll;
   bool fKnown;

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );
   argument = one_argument( argument, arg3 );

   if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
   {
      send_to_char( "Syntax:\n\r", ch);
      send_to_char( "  set skill <name> <spell or skill> <value>\n\r", ch);
      send_to_char( "  set skill <name> all <value>\n\r", ch);
      send_to_char( "  set skill <name> known <value>\n\r", ch);
      send_to_char("   (use the name of the skill, not the number)\n\r", ch);
      return;
   }

   if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }
   if
   (
      IS_NPC(victim) &&
      (
         victim->pIndexData->vnum == MOB_VNUM_DECOY ||
         victim->pIndexData->vnum == MOB_VNUM_MIRROR ||
         victim->pIndexData->vnum == MOB_VNUM_SHADOW
      ) &&
      arg1[0] != '-'
   )
   {
      sprintf(arg_name, "+%s", arg1);
      if
      (
         (
            victim = get_char_world(ch, arg_name)
         ) == NULL
      )
      {
         send_to_char( "They aren't here.\n\r", ch );
         return;
      }
   }

   if ( IS_NPC(victim) )
   {
      send_to_char( "Not on NPC's.\n\r", ch );
      return;
   }
   fKnown = FALSE;
   fAll = !str_cmp( arg2, "all" );
   if (!str_cmp(arg2, "known"))
   {
      fAll = TRUE;
      fKnown = TRUE;
   }
   sn   = 0;
   if ( !fAll && ( sn = skill_lookup( arg2 ) ) < 0 )
   {
      send_to_char( "No such skill or spell.\n\r", ch );
      return;
   }

   /*
   * Snarf the value.
   */
   if ( !is_number( arg3 ) )
   {
      send_to_char( "Value must be numeric.\n\r", ch );
      return;
   }

   value = atoi( arg3 );
   if ( value < 0 || value > 100 )
   {
      send_to_char( "Value range is 0 to 100.\n\r", ch );
      return;
   }

   if ( fAll )
   {
      for (sn = 1; sn < MAX_SKILL; sn++)
      {
         if (fKnown && (victim->pcdata->learned[sn] < 1 || !has_skill(victim, sn))) continue;
         if ( skill_table[sn].name != NULL )
         {
            old_val = victim->pcdata->learned[sn];
            victim->pcdata->learned[sn] = (value == 0) ? -1 : value;
            if (victim->pcdata->learned[sn] != old_val)
            {
               victim->pcdata->updated[sn] = TRUE;
            }
         }
      }
   }
   else
   {
      old_val = victim->pcdata->learned[sn];
      victim->pcdata->learned[sn] = (value == 0) ? -1 : value;
      if (victim->pcdata->learned[sn] != old_val)
      {
         victim->pcdata->updated[sn] = TRUE;
      }
   }

   return;
}


void do_learnlvl( CHAR_DATA *ch, char *argument )
{
   char arg1 [MAX_INPUT_LENGTH];
   char arg2 [MAX_INPUT_LENGTH];
   char arg3 [MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   int value;
   int sn;
   bool fAll;
   char arg_name [MAX_INPUT_LENGTH];

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );
   argument = one_argument( argument, arg3 );

   if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
   {
      send_to_char( "Syntax:\n\r", ch);
      send_to_char( "  set skill learnlvl <spell or skill> <value>\n\r", ch);
      send_to_char( "  set skill learnlvl all <value>\n\r", ch);
      send_to_char("   (use the name of the skill, not the number)\n\r", ch);
      return;
   }

   if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }
   if
   (
      IS_NPC(victim) &&
      (
         victim->pIndexData->vnum == MOB_VNUM_DECOY ||
         victim->pIndexData->vnum == MOB_VNUM_MIRROR ||
         victim->pIndexData->vnum == MOB_VNUM_SHADOW
      ) &&
      arg1[0] != '-'
   )
   {
      sprintf(arg_name, "+%s", arg1);
      if
      (
         (
            victim = get_char_world(ch, arg_name)
         ) == NULL
      )
      {
         send_to_char( "They aren't here.\n\r", ch );
         return;
      }
   }

   if ( IS_NPC(victim) )
   {
      send_to_char( "Not on NPC's.\n\r", ch );
      return;
   }

   fAll = !str_cmp( arg2, "all" );
   sn   = 0;
   if ( !fAll && ( sn = skill_lookup( arg2 ) ) < 0 )
   {
      send_to_char( "No such skill or spell.\n\r", ch );
      return;
   }

   /*
   * Snarf the value.
   */
   if ( !is_number( arg3 ) )
   {
      send_to_char( "Value must be numeric.\n\r", ch );
      return;
   }

   value = atoi( arg3 );
   if ( value < 1 || value > 61 )
   {
      send_to_char( "Value range is 1 to 61.\n\r", ch );
      return;
   }

   if ( fAll )
   {
      for (sn = 1; sn < MAX_SKILL; sn++)
      {
         if ( skill_table[sn].name != NULL )
         victim->pcdata->learnlvl[sn]     = value;
      }
   }
   else
   {
      victim->pcdata->learnlvl[sn] = value;
   }

   return;
}

void do_mset( CHAR_DATA *ch, char *argument )
{
   char arg1 [MAX_INPUT_LENGTH];
   char arg2 [MAX_INPUT_LENGTH];
   char arg3 [MAX_INPUT_LENGTH];
   char arg_name [MAX_INPUT_LENGTH];
   char buf[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   int value;
   unsigned int cnt;

   smash_tilde( argument );
   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );
   strcpy( arg3, argument );

   if
   (
      arg1[0] == '\0' ||
      arg2[0] == '\0' ||
      (
         arg3[0] == '\0' &&
         str_cmp(arg2, "pure") &&
         str_cmp(arg2, "impure") &&
         str_cmp(arg2, "PGI") &&
         str_cmp(arg2, "PGR") &&
         str_cmp(arg2, "PGV") &&
         str_cmp(arg2, "PGA") &&
         str_cmp(arg2, "PGA2") &&
         str_cmp(arg2, "PLI") &&
         str_cmp(arg2, "PLR") &&
         str_cmp(arg2, "PLV") &&
         str_cmp(arg2, "PLA") &&
         str_cmp(arg2, "PLA2")
      )
   )
   {
      send_to_char
      (
         "Syntax:\n\r"
         "  set char <name> <field> <value>\n\r"
         "  Field being one of:\n\r"
         "    str int wis dex con sex class level\n\r"
         "    race group gold silver hp mana move prac\n\r"
         "    align train thirst hunger drunk full\n\r"
         "    home xptotal starvation dehydration\n\r"
         "    quittime special tracking standing record\n\r"
         "    ghost bounty deathtime deathage pkexpand subrace\n\r"
         "    impure pure\n\r"
         "    PGI PGR PGV PGA PGA2\n\r"
         "    PLI PLR PLV PLA PLA2\n\r",
         ch
      );
      return;
   }

   if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }
   if
   (
      IS_NPC(victim) &&
      (
         victim->pIndexData->vnum == MOB_VNUM_DECOY ||
         victim->pIndexData->vnum == MOB_VNUM_MIRROR ||
         victim->pIndexData->vnum == MOB_VNUM_SHADOW
      ) &&
      arg1[0] != '-'
   )
   {
      sprintf(arg_name, "+%s", arg1);
      if
      (
         (
            victim = get_char_world(ch, arg_name)
         ) == NULL
      )
      {
         send_to_char( "They aren't here.\n\r", ch );
         return;
      }
   }
   /* clear zones for mobs */
   victim->zone = NULL;

   /*
   * Snarf the value (which need not be numeric).
   */
   value = is_number( arg3 ) ? atoi( arg3 ) : -5000;

   if
   (
      !str_cmp(arg2, "PGI") ||
      !str_cmp(arg2, "PGR") ||
      !str_cmp(arg2, "PGV") ||
      !str_cmp(arg2, "PGA") ||
      !str_cmp(arg2, "PGA2") ||
      !str_cmp(arg2, "PLI") ||
      !str_cmp(arg2, "PLR") ||
      !str_cmp(arg2, "PLV") ||
      !str_cmp(arg2, "PLA") ||
      !str_cmp(arg2, "PLA2")
   )
   {
      int bits;

      if (IS_NPC(victim))
      {
         send_to_char("You cannot permanently change an NPC.\n\r", ch);
         return;
      }
      if (!IS_TRUSTED(ch, SUPREME))
      {
         send_to_char
         (
            "You are not powerful enough to permanently change\n\r"
            "someone like that.\n\r",
            ch
         );
         return;
      }
      if (arg3[0] == '\0')
      {
         char letter[2];
         char* flag_name;

         send_to_char
         (
            "Syntax:\n\r"
            "  set char <name> <type> <flags>\n\r"
            "  Type is one of\n\r"
            "     PGI  Permanently Gain Immunity\n\r"
            "     PGR  Permanently Gain Resistance\n\r"
            "     PGV  Permanently Gain Vulnerability\n\r"
            "     PGA  Permanently Gain Affect\n\r"
            "     PGA2 Permanently Gain Affect2\n\r"
            "     PLI  Permanently Lose Immunity\n\r"
            "     PLR  Permanently Lose Resistance\n\r"
            "     PLV  Permanently Lose Vulnerability\n\r"
            "     PLA  Permanently Lose Affect\n\r"
            "     PLA2 Permanently Lose Affect2\n\r"
            "  Set flags to 0 to clear\n\r"
            "  Flags for ",
            ch
         );
         bits = 0;
         letter[1] = '\0';
         if (UPPER(arg2[2]) == 'A')
         {
            send_to_char("Affects", ch);
            if (arg2[3] == '\0')
            {
               /* Affects (normal) */
               send_to_char(" are:\n\r", ch);
               /* sizeof(int) * 8 - 1 is the number of useful bits in an int */
               for (cnt = 0; cnt < sizeof(int) * 8 - 1; cnt++)
               {
                  if (cnt < 26)
                  {
                     letter[0] = 'A' + cnt;
                  }
                  else
                  {
                     letter[0] = 'a' + cnt - 26;
                  }
                  flag_name = affect_bit_name(read_flag(letter), 0);
                  if (!str_cmp(flag_name, "none"))
                  {
                     if (letter[0] == 'e')
                     {
                        /* We don't go over 'e' for now anyway */
                        break;
                     }
                     continue;
                  }
                  sprintf
                  (
                     buf,
                     "    %c : %s\n\r",
                     letter[0],
                     flag_name
                  );
                  send_to_char(buf, ch);
                  if (letter[0] == 'e')
                  {
                     /* We don't go over 'e' for now anyway */
                     break;
                  }
               }
            }
            else
            {
               /* Affects2 (normal) */
               send_to_char("2 are:\n\r", ch);
               /* sizeof(int) * 8 - 1 is the number of useful bits in an int */
               for (cnt = 0; cnt < sizeof(int) * 8 - 1; cnt++)
               {
                  if (cnt < 26)
                  {
                     letter[0] = 'A' + cnt;
                  }
                  else
                  {
                     letter[0] = 'a' + cnt - 26;
                  }
                  flag_name = affect_bit_name(0, read_flag(letter));
                  if (!str_cmp(flag_name, "none"))
                  {
                     if (letter[0] == 'e')
                     {
                        /* We don't go over 'e' for now anyway */
                        break;
                     }
                     continue;
                  }
                  sprintf
                  (
                     buf,
                     "    %c : %s\n\r",
                     letter[0],
                     flag_name
                  );
                  send_to_char(buf, ch);
                  if (letter[0] == 'e')
                  {
                     /* We don't go over 'e' for now anyway */
                     break;
                  }
               }
            }
            return;
         }
         send_to_char("Immunities/Resistances/Vulnerabilities are: \n\r", ch);
         /* sizeof(int) * 8 - 1 is the number of useful bits in an int */
         for (cnt = 0; cnt < sizeof(int) * 8 - 1; cnt++)
         {
            if (cnt < 26)
            {
               letter[0] = 'A' + cnt;
            }
            else
            {
               letter[0] = 'a' + cnt - 26;
            }
            flag_name = imm_bit_name(read_flag(letter));
            if (!str_cmp(flag_name, "none"))
            {
               if (letter[0] == 'e')
               {
                  /* We don't go over 'e' for now anyway */
                  break;
               }
               continue;
            }
            sprintf
            (
               buf,
               "    %c : %s\n\r",
               letter[0],
               flag_name
            );
            send_to_char(buf, ch);
            if (letter[0] == 'e')
            {
               /* We don't go over 'e' for now anyway */
               break;
            }
         }
         return;
      }
      if (is_number(arg3))
      {
         int num = atoi(arg3);
         if (num == 0)
         {
            /* Going to clear, read_flags will show 0 */
            arg3[0] = '\0';
         }
         else
         {
            /* If its a number other than 0, show syntax */
            sprintf(buf, "char self %s", arg2);
            do_set(ch, buf);
            return;
         }
      }
      for (cnt = 0; arg3[cnt] != '\0'; cnt++)
      {
         if
         (
            UPPER(arg3[cnt]) < 'A' ||
            UPPER(arg3[cnt]) > 'Z'
         )
         {
            /* invalid input, show syntax */
            sprintf(buf, "char self %s", arg2);
            do_set(ch, buf);
            return;
         }
      }
      bits = read_flag(arg3);
      switch (UPPER(arg2[1]))
      {
         case ('G'):
         {
            switch (UPPER(arg2[2]))
            {
               case ('I'):
               {
                  victim->pcdata->gained_imm_flags = bits;
                  break;
               }
               case ('R'):
               {
                  victim->pcdata->gained_res_flags = bits;
                  break;
               }
               case ('V'):
               {
                  victim->pcdata->gained_vuln_flags = bits;
                  break;
               }
               case ('A'):
               {
                  if (arg2[3] == '2')
                  {
                     /* AFF2 */
                     victim->pcdata->gained_affects2 = bits;
                  }
                  else
                  {
                     /* AFF */
                     victim->pcdata->gained_affects = bits;
                  }
                  break;
               }
               default:
               {
                  /* Something really wrong here */
                  send_to_char("OOC a bug [do_mset-0] occured, please report it.\n\r", ch);
                  return;
               }
            }
            break;
         }
         case ('L'):
         {
            switch (UPPER(arg2[2]))
            {
               case ('I'):
               {
                  victim->pcdata->lost_imm_flags = bits;
                  break;
               }
               case ('R'):
               {
                  victim->pcdata->lost_res_flags = bits;
                  break;
               }
               case ('V'):
               {
                  victim->pcdata->lost_vuln_flags = bits;
                  break;
               }
               case ('A'):
               {
                  if (arg2[3] == '2')
                  {
                     /* AFF2 */
                     victim->pcdata->lost_affects2 = bits;
                  }
                  else
                  {
                     /* AFF */
                     victim->pcdata->lost_affects = bits;
                  }
                  break;
               }
               default:
               {
                  /* Something really wrong here */
                  send_to_char("OOC a bug [do_mset-1] occured, please report it.\n\r", ch);
                  return;
               }
            }
            break;
         }
         default:
         {
            /* Something really wrong here */
            send_to_char("OOC a bug [do_mset-2] occured, please report it.\n\r", ch);
            return;
         }
      }
      /* Recalculate immunity, resist, vulns, and recheck armor */
      irv_update(victim, FALSE, TRUE);
      send_to_char("Ok.\n\r", ch);
      return;
   }
   /*
   * Set something.
   */
   if (!str_cmp(arg2, "impure"))
   {
      if (IS_NPC(victim))
      {
         send_to_char("Not on an NPC\n\r", ch);
         return;
      }
      if (IS_SET(victim->act2, PLR_IMPURE))
      {
         send_to_char("They are already impure.\n\r", ch);
         return;
      }
      send_to_char("They are now impure.\n\r", ch);
      SET_BIT(victim->act2, PLR_IMPURE);
      return;
   }
   if (!str_cmp(arg2, "pure"))
   {
      if (IS_NPC(victim))
      {
         send_to_char("Not on an NPC\n\r", ch);
         return;
      }
      if (!IS_SET(victim->act2, PLR_IMPURE))
      {
         send_to_char("They are already pure.\n\r", ch);
         return;
      }
      send_to_char("They are now pure.\n\r", ch);
      REMOVE_BIT(victim->act2, PLR_IMPURE);
      return;
   }
   if ( !str_cmp(arg2, "subrace"))
   {
      if (victim->race != grn_draconian || IS_NPC(victim))
      {
         send_to_char("You should only set this on draconian players.\n\r", ch);
         return;
      }
      victim->subrace = value;
      return;
   }
   if ( !str_cmp( arg2, "pkexpand"))
   {
      if (value < 0)
      send_to_char("Pk expansion must be greater or equal to 0.\n\r", ch);
      else if (victim->pcdata == NULL)
      send_to_char("Not on mobs.\n\r", ch);
      else
      victim->pcdata->pk_expand = value;
      return;
   }

   if ( !str_cmp( arg2, "dehydration"))
   {
      if (value < 0)
      send_to_char("Dehydration must be greater or equal to 0.\n\r", ch);
      else if (victim->pcdata == NULL)
      send_to_char("Not on mobs.\n\r", ch);
      else
      victim->pcdata->condition[COND_DEHYDRATED] = value;
      return;
   }
   if ( !str_cmp( arg2, "starvation"))
   {
      if (value < 0)
      send_to_char("Starvation must be greater or equal to 0.\n\r", ch);
      else if (victim->pcdata == NULL)
      send_to_char("Not on mobs.\n\r", ch);
      else
      victim->pcdata->condition[COND_STARVING] = value;
      return;
   }
   if ( !str_cmp( arg2, "quittime"))
   {
      if (value < 0)
      send_to_char("Quittime can't be less than 0.\n\r", ch);
      else
      {
         victim->quittime = value;
         if (value == 0)
         {
            victim->pause = 0;
         }
      }
      return;
   }
   if ( !str_cmp(arg2, "deathage"))
   {
      if (value < 0)
      {
         send_to_char("Death age should be positive.\n\r", ch);
         return;
      }
      if (IS_NPC(victim)){
         send_to_char("Not on mobs.\n\r", ch);
         return;
      }
      if
      (
         victim->pcdata->age_mod < 0 &&
         !IS_TRUSTED(ch, IMPLEMENTOR)
      )
      {
         send_to_char("Only one life extension per player.\n\r", ch);
         return;
      }
      victim->pcdata->age_mod += get_death_age(victim) - value;
      if (victim->pcdata->age_mod == 0)
      {
         victim->pcdata->age_mod--;
      }
      return;
   }

   if ( !str_cmp(arg2, "deathtime"))
   {
      if (value < 0)
      {
         send_to_char("Char ghost time remaining has to be positive.\n\r", ch);
         return;
      }
      if (IS_NPC(victim)){
         send_to_char("Not on mobs.\n\r", ch);
         return;
      }
      if (victim->pcdata->death_status != HAS_DIED)
      {
         act
         (
            "$N is not dead, set $S deathage instead.",
            ch,
            NULL,
            victim,
            TO_CHAR
         );
         return;
      }
      victim->pcdata->death_timer = value;
      if (value == 0)
      victim->pcdata->death_status = 0;
      else
      victim->pcdata->death_status = HAS_DIED;
      return;
   }
   if ( !str_cmp(arg2, "ghost"))
   {
      if (value < 0) {
         send_to_char("Ghost timer has to be positive.\n\r", ch);
         return;
      }
      if (IS_NPC(victim)){
         send_to_char("Not on mobs.\n\r", ch);
         return;
      }
      victim->ghost = value;
      return;
   }
   if ( !str_cmp(arg2, "rebels"))
   {
      if (value < 0){
         send_to_char("Records must be positive.\n\r", ch);
         return;
      }
      if (IS_NPC(victim)){
         send_to_char("Not on mobs.\n\r", ch);
         return;
      }
      victim->pcdata->rebels = value;
      return;
   }
   if ( !str_cmp(arg2, "record"))
   {
      if (value < 0){
         send_to_char("Records must be positive.\n\r", ch);
         return;
      }
      if (IS_NPC(victim)){
         send_to_char("Not on mobs.\n\r", ch);
         return;
      }
      victim->pcdata->wanteds = value;
      return;
   }
   if ( !str_cmp(arg2, "standing"))
   {
      if
      (
         (
            victim->house != HOUSE_ANCIENT &&
            !IS_SET(victim->act2, PLR_IS_ANCIENT)
         ) &&
         victim->house != HOUSE_ENFORCER
      )
      {
         send_to_char("Only Ancients ands Justicars have standing.\n\r", ch);
         return;
      }
      if (value < 0){
         send_to_char("Standings must be positive.\n\r", ch);
         return;
      }
      if (IS_NPC(victim)){
         send_to_char("Not on mobs.\n\r", ch);
         return;
      }
      victim->pcdata->targetkills = value;
      return;
   }
   if ( !str_cmp( arg2, "xptotal"))
   {
      if ( value < 0)
      {
         send_to_char("Xp total must be greater than 0.\n\r", ch);
         return;
      }
      victim->exp_total = value;
      return;
   }
   if ( !str_cmp( arg2, "home"))
   {
      int home;

      if (IS_NPC(victim))
      {
         send_to_char("Not on an NPC.\n\r", ch);
         return;
      }
      for (home = 0; hometown_table[home].name != NULL; home++)
      {
         if
         (
            !hometown_table[home].allow_good &&
            !hometown_table[home].allow_neutral &&
            !hometown_table[home].allow_evil
         )
         {
            continue;
         }
         if (value == hometown_table[home].vnum)
         {
            victim->temple = value;
            return;
         }
      }
      send_to_char("Home must be one of:\n\r", ch);
      for (home = 0; hometown_table[home].name != NULL; home++)
      {
         if
         (
            !hometown_table[home].allow_good &&
            !hometown_table[home].allow_neutral &&
            !hometown_table[home].allow_evil
         )
         {
            continue;
         }
         sprintf
         (
            buf,
            "%-17s:[%05d](",
            hometown_table[home].name,
            hometown_table[home].vnum
         );
         if (hometown_table[home].allow_good)
         {
            strcat(buf, "Good");
         }
         if (hometown_table[home].allow_neutral)
         {
            if (hometown_table[home].allow_good)
            {
               strcat(buf, "/");
            }
            strcat(buf, "Neutral");
         }
         if (hometown_table[home].allow_evil)
         {
            if
            (
               hometown_table[home].allow_good ||
               hometown_table[home].allow_neutral
            )
            {
               strcat(buf, "/");
            }
            strcat(buf, "Evil");
         }
         strcat(buf, ")\n\r");
         send_to_char(buf, ch);
      }
      return;
   }

   if ( !str_cmp( arg2, "str" ) )
   {
      if ( value < 3 || value > get_max_train(victim, STAT_STR) )
      {
         sprintf(buf,
         "Strength range is 3 to %d\n\r.",
         get_max_train(victim, STAT_STR));
         send_to_char(buf, ch);
         return;
      }

      victim->perm_stat[STAT_STR] = value;
      return;
   }

   if ( !str_cmp( arg2, "int" ) )
   {
      if ( value < 3 || value > get_max_train(victim, STAT_INT) )
      {
         sprintf(buf,
         "Intelligence range is 3 to %d.\n\r",
         get_max_train(victim, STAT_INT));
         send_to_char(buf, ch);
         return;
      }

      victim->perm_stat[STAT_INT] = value;
      return;
   }

   if ( !str_cmp( arg2, "wis" ) )
   {
      if ( value < 3 || value > get_max_train(victim, STAT_WIS) )
      {
         sprintf(buf,
         "Wisdom range is 3 to %d.\n\r", get_max_train(victim, STAT_WIS));
         send_to_char( buf, ch );
         return;
      }

      victim->perm_stat[STAT_WIS] = value;
      return;
   }

   if ( !str_cmp( arg2, "dex" ) )
   {
      if ( value < 3 || value > get_max_train(victim, STAT_DEX) )
      {
         sprintf(buf,
         "Dexterity ranges is 3 to %d.\n\r",
         get_max_train(victim, STAT_DEX));
         send_to_char( buf, ch );
         return;
      }

      victim->perm_stat[STAT_DEX] = value;
      return;
   }

   if ( !str_cmp( arg2, "con" ) )
   {
      if ( value < 3 || value > get_max_train(victim, STAT_CON) )
      {
         sprintf(buf,
         "Constitution range is 3 to %d.\n\r",
         get_max_train(victim, STAT_CON));
         send_to_char( buf, ch );
         return;
      }

      victim->perm_stat[STAT_CON] = value;
      return;
   }

   if ( !str_prefix(arg2, "special") )
   {
      if ( IS_NPC(victim) )
      {
         send_to_char("Mobs do not specialize in subclasses.\n\r", ch);
         return;
      }

      if ( value == -5000 )
      {
         value = find_subclass( victim, arg3 );
         if ( value == -5 )
         {
            send_to_char( "No such subclass.  Choices for special are:\n\r", ch);
            send_to_char( "Anti-Paladin: war, famine, pestilence, or death.\n\r", ch);
            send_to_char( "Necromancer: anatomist, corruptor, or necrophile.\n\r", ch);
            send_to_char( "Monk: tiger, dragon, mantis, crane, or snake.\n\r", ch);
            send_to_char( "Note: \"none\" and \"wait\" may be used for anyone.\n\r", ch);
            send_to_char( "No other subclasses have been defined yet.\n\r", ch);
            return;
         }
      } else
      if ( value < -2 || value > MAX_SUBCLASS )
      {
         sprintf( buf, "Special range is -2 to %d.\n\r", MAX_SUBCLASS );
         send_to_char( buf, ch );
         return;
      }

      if ( value != victim->pcdata->special )
      {
         strip_char_subclass( victim );
         if ( value < 1 )
         {
            victim->pcdata->special = (value == SUBCLASS_WAITING ? SUBCLASS_NONE : value);
            send_to_char( "Done, player has no subclass now.\n\r", ch );
         }
         else
         {
            victim->pcdata->special = SUBCLASS_NONE;
            if ( value == SUBCLASS_SCHOOL_TIGER
            || value == SUBCLASS_SCHOOL_MANTIS
            || value == SUBCLASS_SCHOOL_CRANE
            || value == SUBCLASS_SCHOOL_SNAKE
            || value == SUBCLASS_SCHOOL_DRAGON )
            make_char_monksub( victim, value );
            else
            make_char_subclass( victim, NULL, value );

            send_to_char( "Done, player has new subclass.\n\r", ch );
         }
         racial_skills(victim);
      }
      else
      send_to_char( "That person is already that subclass.\n\r", ch );

      return;
   }

   if ( !str_prefix( arg2, "sex" ) )
   {
      if ( value < 0 || value > 2 )
      {
         send_to_char( "Sex range is 0 to 2.\n\r", ch );
         return;
      }
      victim->sex = value;
      if (!IS_NPC(victim))
      victim->pcdata->true_sex = value;
      return;
   }

   if ( !str_prefix( arg2, "class" ) )
   {
      int class;

      if (IS_NPC(victim))
      {
         send_to_char("Mobiles have no class.\n\r", ch);
         return;
      }

      class = class_lookup(arg3);
      if ( class == -1 )
      {
         char buf[MAX_STRING_LENGTH];

         strcpy( buf, "Possible classes are: " );
         for ( class = 0; class < MAX_CLASS; class++ )
         {
            if ( class > 0 )
            strcat( buf, " " );
            strcat( buf, class_table[class].name );
         }
         strcat( buf, ".\n\r" );

         send_to_char(buf, ch);
         return;
      }

      victim->class = class;
      victim->pcdata->special = 0;
      return;
   }

   if ( !str_prefix( arg2, "level" ) )
   {
      if ( !IS_NPC(victim) )
      {
         send_to_char( "Not on PC's.\n\r", ch );
         return;
      }

      if ( value < 0 || value > 60 )
      {
         send_to_char( "Level range is 0 to 60.\n\r", ch );
         return;
      }
      victim->level = value;
      return;
   }

   if ( !str_prefix( arg2, "gold" ) )
   {
      victim->gold = value;
      return;
   }

   if ( !str_prefix(arg2, "silver" ) )
   {
      victim->silver = value;
      return;
   }

   if ( !str_prefix( arg2, "hp" ) )
   {
      if ( value < -10 || value > 30000 )
      {
         send_to_char( "Hp range is -10 to 30, 000 hit points.\n\r", ch );
         return;
      }
      victim->max_hit = value;
      if (!IS_NPC(victim))
      victim->pcdata->perm_hit = value;
      return;
   }

   if ( !str_prefix( arg2, "mana" ) )
   {
      if ( value < 0 || value > 30000 )
      {
         send_to_char( "Mana range is 0 to 30, 000 mana points.\n\r", ch );
         return;
      }
      victim->max_mana = value;
      if (!IS_NPC(victim))
      victim->pcdata->perm_mana = value;
      return;
   }

   if ( !str_prefix( arg2, "move" ) )
   {
      if ( value < 0 || value > 30000 )
      {
         send_to_char( "Move range is 0 to 30, 000 move points.\n\r", ch );
         return;
      }
      victim->max_move = value;
      if (!IS_NPC(victim))
      victim->pcdata->perm_move = value;
      return;
   }

   if ( !str_prefix( arg2, "practice" ) )
   {
      if ( value < 0 || value > 250 )
      {
         send_to_char( "Practice range is 0 to 250 sessions.\n\r", ch );
         return;
      }
      victim->practice = value;
      return;
   }

   if ( !str_prefix( arg2, "train" ))
   {
      if (value < 0 || value > 50 )
      {
         send_to_char("Training session range is 0 to 50 sessions.\n\r", ch);
         return;
      }
      victim->train = value;
      return;
   }

   if ( !str_prefix( arg2, "align" ) )
   {
      if ( value < -1000 || value > 1000 )
      {
         send_to_char( "Alignment range is -1000 to 1000.\n\r", ch );
         return;
      }
      victim->alignment = value;
      victim->true_alignment = value;
      return;
   }

   if ( !str_prefix( arg2, "ethos" ) )
   {
      if ( value < -1000 || value > 1000 )
      {
         send_to_char( "Ethos range is -1000 to 1000.\n\r", ch );
         return;
      }
      if (!IS_NPC(victim))
      victim->pcdata->ethos = value;
      return;
   }

   if ( !str_prefix( arg2, "thirst" ) )
   {
      if ( IS_NPC(victim) )
      {
         send_to_char( "Not on NPC's.\n\r", ch );
         return;
      }

      if ( value < -1 || value > 100 )
      {
         send_to_char( "Thirst range is -1 to 100.\n\r", ch );
         return;
      }

      victim->pcdata->condition[COND_THIRST] = value;
      return;
   }

   if ( !str_prefix( arg2, "drunk" ) )
   {
      if ( IS_NPC(victim) )
      {
         send_to_char( "Not on NPC's.\n\r", ch );
         return;
      }

      if ( value < -1 || value > 100 )
      {
         send_to_char( "Drunk range is -1 to 100.\n\r", ch );
         return;
      }

      victim->pcdata->condition[COND_DRUNK] = value;
      return;
   }

   if ( !str_prefix( arg2, "full" ) )
   {
      if ( IS_NPC(victim) )
      {
         send_to_char( "Not on NPC's.\n\r", ch );
         return;
      }

      if ( value < -1 || value > 100 )
      {
         send_to_char( "Full range is -1 to 100.\n\r", ch );
         return;
      }

      victim->pcdata->condition[COND_FULL] = value;
      return;
   }

   if ( !str_prefix( arg2, "hunger" ) )
   {
      if ( IS_NPC(victim) )
      {
         send_to_char( "Not on NPC's.\n\r", ch );
         return;
      }

      if ( value < -1 || value > 100 )
      {
         send_to_char( "Full range is -1 to 100.\n\r", ch );
         return;
      }

      victim->pcdata->condition[COND_HUNGER] = value;
      return;
   }

   if (!str_prefix( arg2, "race" ) )
   {
      int race;

      race = race_lookup(arg3);

      if ( race == 0)
      {
         send_to_char("That is not a valid race.\n\r", ch);
         return;
      }

      if
      (
         !IS_NPC(victim) &&
         race >= MAX_PC_RACE
      )
      {
         send_to_char("That is not a valid player race.\n\r", ch);
         return;
      }
      if (victim->subrace)
      {
         send_to_char("Clearing subrace.\n\r", ch);
         victim->subrace = 0;
      }
      victim->race = race;
      racial_skills(victim);
      return;
   }

   if (!str_prefix(arg2, "tracking"))
   {
      CHAR_DATA * tracktarget;

      if (!IS_NPC(victim))
      {
         send_to_char("Only on NPCs.\n\r", ch);
         return;
      }

      if ( (( tracktarget = get_char_world( ch, arg3 ) ) == NULL) &&
      (str_cmp(arg3, "none")))
      {
         send_to_char("Target not found.\n\r", ch );
         return;
      }

      if (tracktarget != NULL && IS_NPC(tracktarget))
      {
         send_to_char("Can't set a mob as the target of tracking.\n\r", ch);
         return;
      }

      if (!str_cmp(arg3, "none"))
      victim->last_fought = -1;
      else
      victim->last_fought = tracktarget->id;
      send_to_char("New tracking set.\n\r", ch);
      return;
   }

   if (!str_prefix(arg2, "group"))
   {
      if (!IS_NPC(victim))
      {
         send_to_char("Only on NPCs.\n\r", ch);
         return;
      }
      victim->group = value;
      return;
   }

   if ( !str_prefix(arg2, "bounty" ) )
   {
      if (!IS_SET(victim->act2, PLR_ANCIENT_MARK))
      {
         send_to_char("You cannot adjust bounties on unmarked people.\n\r", ch);
         return;
      }
      if (value >= 0)
      victim->pcdata->mark_price = value;
      else
      send_to_char("Ha ha, funny.\n\r", ch);
      return;
   }

   /*
   * Generate usage message.
   */
   do_mset( ch, "" );
   return;
}

void free_exit(ROOM_INDEX_DATA* room, int direction)
{
   EXIT_DATA* exit_data = NULL;

   if (room->exit[direction] != room->old_exit[direction])
   {
      if (room->exit[direction])
      {
         free_mem(room->exit[direction], sizeof(*exit_data));
      }
   }

   room->exit[direction] = exit_data;
}

void new_exit(ROOM_INDEX_DATA* room, int direction)
{
   EXIT_DATA* exit_data;

   free_exit(room, direction);
   exit_data = alloc_mem(sizeof(*exit_data));
   exit_data->u1.vnum = 0;
   exit_data->u1.to_room = NULL;
   exit_data->exit_info = 0;
   exit_data->key = 0;
   exit_data->keyword = &str_empty[0];
   exit_data->description = &str_empty[0];
   room->exit[direction] = exit_data;
}

void do_exitset( CHAR_DATA *ch, char *argument )
{
   char    direction_str[MAX_INPUT_LENGTH];
   char    new_exit_vnum_str[MAX_INPUT_LENGTH];
   int     direction = -1;
   BUFFER* buffer_ptr;
   ROOM_INDEX_DATA* room_ptr;

   smash_tilde( argument );
   argument = one_argument(argument, direction_str);
   one_argument(argument, new_exit_vnum_str);

   if (direction_str[0] == '\0')
   {
      buffer_ptr = new_buf();
      add_buf(
         buffer_ptr,
         "NAME\n\r"
         "       set exit - changes the room to which an exit leads\n\r"
         "\n\r");
      add_buf(
         buffer_ptr,
         "SYNOPSIS\n\r"
         "       set exit <direction> <command>\n\r"
         "\n\r");
      add_buf(
         buffer_ptr,
         "DESCRIPTION\n\r"
         "       The \"set exit\" command changes a given exit to lead to a requested\n\r"
         "       room.  The exit may be restored or deleted.\n\r"
         "\n\r");
      add_buf(
         buffer_ptr,
         "DIRECTION\n\r"
         "       You must specify a direction in order to refer to a particular exit.\n\r"
         "       Valid directions (may be partial names) are:\n\r"
         "\n\r"
         "          north south east west up down\n\r"
         "\n\r");
      add_buf(
         buffer_ptr,
         "COMMANDS\n\r"
         "       <vnum>  Specify a room vnum.  You can only specify vnums that are\n\r"
         "               associated with existing rooms.\n\r"
         "\n\r"
         "       delete  Delete the specified exit.  Type this command fully.\n\r"
         "\n\r"
         "       restore Restore the specified exit to the original state defined by \n\r"
         "               the area file.  Type this command fully.\n\r");
      page_to_char(buf_string(buffer_ptr), ch);
      free_buf(buffer_ptr);
      return;
   }

   if (!str_prefix(direction_str, "north"))
   {
      direction = DIR_NORTH;
   }
   else
   if (!str_prefix(direction_str, "south"))
   {
      direction = DIR_SOUTH;
   }
   else
   if (!str_prefix(direction_str, "east"))
   {
      direction = DIR_EAST;
   }
   else
   if (!str_prefix(direction_str, "west"))
   {
      direction = DIR_WEST;
   }
   else
   if (!str_prefix(direction_str, "up"))
   {
      direction = DIR_UP;
   }
   else
   if (!str_prefix(direction_str, "down"))
   {
      direction = DIR_DOWN;
   }
   else
   {
      send_to_char("set exit: Invalid direction.\n\r\n\r", ch);
      do_exitset(ch, "");
      return;
   }

   if (!str_cmp(new_exit_vnum_str, "restore"))
   {
      free_exit(ch->in_room, direction);
      ch->in_room->exit[direction] = ch->in_room->old_exit[direction];
      send_to_char("Exit restored.\n\r", ch);
      return;
   }
   else
   if (!str_cmp(new_exit_vnum_str, "delete"))
   {
      if (ch->in_room->exit[direction] != NULL)
      {
         free_exit(ch->in_room, direction);
         send_to_char("Exit deleted.\n\r", ch);
      }
      else
      {
         send_to_char("That exit does not exist.\n\r", ch);
      }
      return;
   }
   else
   if (!isdigit(new_exit_vnum_str[0]))
   {
      send_to_char("set exit: Invalid command.\n\r\n\r", ch);
      do_exitset(ch, "");
      return;
   }

   /* If we get to this point, we have a vnum in new_exit_vnum_str. */
   room_ptr = find_location(ch, new_exit_vnum_str);
   if (room_ptr == NULL)
   {
      send_to_char("set exit: Specified vnum is not a valid location.\n\r", ch);
      return;
   }

   if (
         ch->in_room->exit[direction] &&
         ch->in_room->exit[direction]->u1.to_room == room_ptr
      )
   {
      send_to_char("The exit remains the same.\n\r", ch);
   }
   else
   {
      new_exit(ch->in_room, direction);
      ch->in_room->exit[direction]->u1.vnum = room_ptr->vnum;
      ch->in_room->exit[direction]->u1.to_room = room_ptr;
      send_to_char("The exit has changed.\n\r", ch);
   }

   return;
}

void do_string( CHAR_DATA *ch, char *argument )
{
   char type [MAX_INPUT_LENGTH];
   char arg1 [MAX_INPUT_LENGTH];
   char arg2 [MAX_INPUT_LENGTH];
   char arg3 [MAX_INPUT_LENGTH];
   char arg_name [MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   OBJ_DATA *obj;

   smash_tilde( argument );
   argument = one_argument( argument, type );
   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );
   strcpy( arg3, argument );

   if ( type[0] == '\0' || arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
   {
      send_to_char
      (
         "Syntax:\n\r"
         "  string char <name> <field> <string>\n\r"
         "    fields: name short long desc title spec\n\r"
         "  string obj  <name> <field> <string>\n\r"
         "    fields: name short long extended owner\n\r"
         "            material\n\r",
         ch
      );
      return;
   }

   if (!str_prefix(type, "character") || !str_prefix(type, "mobile"))
   {
      if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
      {
         send_to_char( "They aren't here.\n\r", ch );
         return;
      }

      if
      (
         IS_NPC(victim) &&
         (
            victim->pIndexData->vnum == MOB_VNUM_DECOY ||
            victim->pIndexData->vnum == MOB_VNUM_MIRROR ||
            victim->pIndexData->vnum == MOB_VNUM_SHADOW
         ) &&
         arg1[0] != '-'
      )
      {
         sprintf(arg_name, "+%s", arg1);
         if
         (
            (
               victim = get_char_world(ch, arg_name)
            ) == NULL
         )
         {
            send_to_char( "They aren't here.\n\r", ch );
            return;
         }
      }
      /* clear zone for mobs */
      victim->zone = NULL;

      /* string something */

      if ( !str_prefix( arg2, "name" ) )
      {
         if (IS_NPC(victim))
         {
            free_string( victim->name );
            victim->name = str_dup( arg3 );
         }
         else
         {
            send_to_char("Not on PCs, use rename.\n\r", ch);
         }
         return;
      }

      if ( !str_prefix( arg2, "description" ) )
      {
         sprintf(arg3, "%s\n\r", arg3); /* Allows descs to be on separate line. */
         free_string(victim->description);
         victim->description = str_dup(arg3);
         return;
      }

      if ( !str_prefix( arg2, "short" ) )
      {
         free_string( victim->short_descr );
         if (!str_cmp(arg3, "none"))
         {
            send_to_char("Short Desc cleared.\n\r", ch);
            victim->long_descr = &str_empty[0];
            return;
         }
         victim->short_descr = str_dup( arg3 );
         send_to_char("Short Desc set.\n\r", ch);
         return;
      }

      if ( !str_prefix( arg2, "long" ) )
      {
         free_string( victim->long_descr );
         if (!str_cmp(arg3, "none"))
         {
            send_to_char("Long Desc cleared.\n\r", ch);
            victim->long_descr = &str_empty[0];
            return;
         }
         if (IS_NPC(victim))
         {
            strcat(arg3, "\n\r");
         }
         victim->long_descr = str_dup( arg3 );
         send_to_char("Long Desc set.\n\r", ch);
         return;
      }

      if ( !str_prefix( arg2, "title" ) )
      {
         if ( IS_NPC(victim) )
         {
            send_to_char( "Not on NPC's.\n\r", ch );
            return;
         }

         set_title( victim, arg3 );
         return;
      }

      if ( !str_prefix( arg2, "spec" ) )
      {
         if ( !IS_NPC(victim) )
         {
            send_to_char( "Not on PC's.\n\r", ch );
            return;
         }

         if ( ( victim->spec_fun = spec_lookup( arg3 ) ) == 0 )
         {
            send_to_char( "No such spec fun.\n\r", ch );
            return;
         }

         return;
      }
   }

   if (!str_prefix(type, "object"))
   {
      /* string an obj */

      if ( ( obj = get_obj_world( ch, arg1 ) ) == NULL )
      {
         send_to_char( "Nothing like that in heaven or earth.\n\r", ch );
         return;
      }

      if ( !str_prefix( arg2, "name" ) )
      {
         free_string( obj->name );
         obj->name = str_dup( arg3 );
         return;
      }

      if ( !str_prefix( arg2, "short" ) )
      {
         free_string( obj->short_descr );
         obj->short_descr = str_dup( arg3 );
         return;
      }

      if (!str_prefix(arg2, "owner"))
      {
         free_string(obj->owner);
         obj->owner = str_dup(arg3);
         return;
      }

      if (!str_prefix(arg2, "material"))
      {
         free_string(obj->material);
         obj->material = str_dup(arg3);
         return;
      }

      if ( !str_prefix( arg2, "long" ) )
      {
         free_string( obj->description );
         obj->description = str_dup( arg3 );
         return;
      }

      if ( !str_prefix( arg2, "ed" ) || !str_prefix( arg2, "extended"))
      {
         EXTRA_DESCR_DATA *ed;

         argument = one_argument( argument, arg3 );
         if ( argument == NULL )
         {
            send_to_char( "Syntax: oset <object> ed <keyword> <string>\n\r",
            ch );
            return;
         }

         strcat(argument, "\n\r");

         ed = new_extra_descr();

         ed->keyword        = str_dup( arg3     );
         ed->description    = str_dup( argument );
         ed->next        = obj->extra_descr;
         obj->extra_descr    = ed;
         return;
      }
   }


   /* echo bad use message */
   do_string(ch, "");
}

void do_oset( CHAR_DATA *ch, char *argument )
{
   char arg1 [MAX_INPUT_LENGTH];
   char arg2 [MAX_INPUT_LENGTH];
   char arg3 [MAX_INPUT_LENGTH];
   OBJ_DATA *obj;
   int value;
   bool glowing;

   smash_tilde( argument );
   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );
   strcpy( arg3, argument );

   if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
   {
      send_to_char("Syntax:\n\r", ch);
      send_to_char("  set obj <object> <field> <value>\n\r", ch);
      send_to_char("  Field being one of:\n\r",                ch );
      send_to_char("    value0 value1 value2 value3 value4\n\r",    ch );
      send_to_char("    extra wear level weight cost timer\n\r",        ch );
      send_to_char("    phase0 phase1 phase2\n\r", ch);
      return;
   }

   if ( ( obj = get_obj_world( ch, arg1 ) ) == NULL )
   {
      send_to_char( "Nothing like that in heaven or earth.\n\r", ch );
      return;
   }

   /*
   * Snarf the value (which need not be numeric).
   */
   value = atoi( arg3 );

   /*
   * Set something.
   */
   if (!str_cmp(arg2, "phase0"))
   {
      obj->moonphases[0] = value;
      return;
   }

   if (!str_cmp(arg2, "phase1"))
   {
      obj->moonphases[1] = value;
      return;
   }

   if (!str_cmp(arg2, "phase2"))
   {
      obj->moonphases[2] = value;
      return;
   }

   if (!str_cmp(arg2, "value0"))
   {
      obj->value[0] = value;
      return;
   }

   if (!str_cmp(arg2, "value1"))
   {
      obj->value[1] = value;
      return;
   }

   if (!str_cmp(arg2, "value2"))
   {
      obj->value[2] = value;
      return;
   }

   if (!str_cmp(arg2, "value3"))
   {
      obj->value[3] = value;
      return;
   }

   if (!str_cmp(arg2, "value4"))
   {

      if (is_number(arg3))
      {
         obj->value[4] = value;
      }
      else
      {
         obj->value[4] = read_flag(arg3);
      }
      if
      (
         obj->item_type == ITEM_BOOK &&
         IS_SET(obj->value[4], BOOK_SAVE) &&
         obj->pIndexData->book_info->book_save_info == NULL
      )
      {
         REMOVE_BIT(obj->value[4], BOOK_SAVE);
         send_to_char
         (
            "You may not give save flags to books here.\n\r"
            "Please modify the area file.\n\r",
            ch
         );
      }
      return;
   }

   if ( !str_prefix( arg2, "extra" ) )
   {
      if (IS_SET(obj->extra_flags, ITEM_GLOW))
      {
         glowing = TRUE;
      }
      else
      {
         glowing = FALSE;
      }
      if ( is_number(arg3) )
      {
         obj->extra_flags = value;
      }
      else
      {
         obj->extra_flags = read_flag(arg3);
      }
      /* Take care of light in room */
      if
      (
         obj->wear_loc != WEAR_NONE &&
         obj->item_type != ITEM_LIGHT &&
         obj->carried_by != NULL &&
         obj->carried_by->in_room != NULL
      )
      {
         if
         (
            glowing &&
            !IS_SET(obj->extra_flags, ITEM_GLOW)
         )
         {
            obj->carried_by->in_room->light -= 3;
            light_check
            (
               obj->carried_by->in_room,
               obj->carried_by,
               obj,
               "set"
            );
         }
         else if
         (
            !glowing &&
            IS_SET(obj->extra_flags, ITEM_GLOW)
         )
         {
            obj->carried_by->in_room->light += 3;
         }
      }
      return;
   }

   if ( !str_prefix( arg2, "wear" ) )
   {
      if ( is_number(arg3) )
      {
         obj->wear_flags = value;
      } else
      {
         obj->wear_flags = read_flag(arg3);
      }
      return;
   }

   if ( !str_prefix( arg2, "level" ) )
   {
      obj->level = value;
      return;
   }

   if ( !str_prefix( arg2, "weight" ) )
   {
      obj->weight = value;
      return;
   }

   if ( !str_prefix( arg2, "cost" ) )
   {
      obj->cost = value;
      return;
   }

   if ( !str_prefix( arg2, "timer" ) )
   {
      obj->timer = value;
      return;
   }

   /*
   * Generate usage message.
   */
   do_oset( ch, "" );
   return;
}



void do_rset( CHAR_DATA *ch, char *argument )
{
   char arg1 [MAX_INPUT_LENGTH];
   char arg2 [MAX_INPUT_LENGTH];
   char arg3 [MAX_INPUT_LENGTH];
   ROOM_INDEX_DATA *location;
   int value;

   smash_tilde( argument );
   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );
   strcpy( arg3, argument );

   if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
   {
      send_to_char( "Syntax:\n\r", ch);
      send_to_char( "  set room <location> <field> <value>\n\r", ch);
      send_to_char( "  Field being one of:\n\r",            ch );
      send_to_char( "    flags sector\n\r",                ch );
      return;
   }

   if ( ( location = find_location( ch, arg1 ) ) == NULL )
   {
      send_to_char( "No such location.\n\r", ch );
      return;
   }

   if (!is_room_owner(ch, location) && ch->in_room != location
   &&  room_is_private(location) && !IS_TRUSTED(ch, IMPLEMENTOR))
   {
      send_to_char("That room is private right now.\n\r", ch);
      return;
   }

   /*
   * Snarf the value.
   */
   /*
   This value no longer needs to be numeric - Mael

   if ( !is_number( arg3 ) )
   {
   send_to_char( "Value must be numeric.\n\r", ch );
   return;
   }
   */
   value = atoi( arg3 );

   /*
   * Set something.
   */
   if ( !str_prefix( arg2, "flags" ) )
   {
      if ( is_number(arg3) )
      {
         location->room_flags = value;
      } else
      {
         location->room_flags = read_flag(arg3);
      }
      return;
   }

   if ( !str_prefix( arg2, "sector" ) )
   {
      if ( !is_number( arg3 ) )
      {
         send_to_char( "Value must be numeric.\n\r", ch );
         return;
      }
      location->sector_type    = value;
      return;
   }

   /*
   * Generate usage message.
   */
   do_rset( ch, "" );
   return;
}


void do_sockets( CHAR_DATA *ch, char *argument )
{
   char buf[2 * MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];
   char arg[MAX_INPUT_LENGTH];
   DESCRIPTOR_DATA *d;
   int count;
   char* connected_string;
   static char* connected[] =
   {
      "Playing",
      "Getting name",
      "Getting password",
      "Confirming name",
      "Getting new pwd",
      "Confirm new pwd",
      "Getting race",
      "Getting sex",
      "Getting class",
      "Rolling stats",
      "Getting alignment",
      "Getting ethos",
      "CON_DEFAULT_CHOICE",
      "CON_GEN_GROUPS",
      "CON_PICK_WEAPON",
      "Getting Imotd",
      "Getting motd",
      "CON_BREAK_CONNECT",
      "Choosing subclass",
      "Getting storage name",
      "Picking scale color",
      "Picking human bonuses",
      "Getting surname",
      "Confirming surname",
   };

   count    = 0;
   buf[0]    = '\0';

   one_argument(argument, arg);
   for ( d = descriptor_list; d != NULL; d = d->next )
   {
      if
      (
         d->character != NULL &&
         can_see(ch, d->character) &&
         (
            arg[0] == '\0' ||
            is_name(arg, d->character->name) ||
            (
               d->original &&
               is_name(arg, d->original->name)
            )
         )
      )
      {
         count++;
         if (connected[d->connected] != NULL)
         {
            connected_string = connected[d->connected];
         }
         else
         {
            connected_string = "null";
         }
         sprintf
         (
            buf + strlen(buf), "[%3d %21s] %s@%s\n\r",
            d->descriptor,
            connected_string,
            d->original  ? d->original->name  :
            d->character ? d->character->name : "(none)",
            (get_trust(ch) >= 55) ? d->host : "unknown"
         );
      }
   }
   if (count == 0)
   {
      send_to_char("No one by that name is connected.\n\r", ch);
      return;
   }

   sprintf( buf2, "%d user%s\n\r", count, count == 1 ? "" : "s" );
   strcat(buf, buf2);
   page_to_char( buf, ch );
   return;
}

char* get_end_host(char* host)
{
   int i=0;
   while(host[i]!='.'&&host[i]!='\0') {
      i++;
   }
   if (host[i]!='\0')
   i++;
   return &host[i];
}

/* XUR - for new multicheck */
int net_comp(MULTDATA* d1, MULTDATA* d2) {
   int d1class=0, d2class=0;
   unsigned long d1net=0, d2net=0;

   /* Resolve into class */
   if ((d1->des->hostip & 0x80000000) == 0)
   d1class = 2; /* should be 1, but I'm narrowing the search */
   else
   if ((d1->des->hostip & 0xC0000000) == 0x80000000)
   d1class = 2;
   else
   d1class = 3;
   if ((d2->des->hostip & 0x80000000) == 0)
   d2class = 2; /* should be 1, but I'm narrowing the search */
   else
   if ((d2->des->hostip & 0xC0000000) == 0x80000000)
   d2class = 2;
   else
   d2class = 3;

   /* Compare */
   if (d1class == d2class) /* Classes match, compare networks */
   {
      switch (d1class)
      {
         case 1:
         d1net = d1->des->hostip & 0x7F000000;
         d2net = d2->des->hostip & 0x7F000000;
         break;
         case 2:
         d1net = d1->des->hostip & 0x3FFF0000;
         d2net = d2->des->hostip & 0x3FFF0000;
         break;
         case 3:
         d1net = d1->des->hostip & 0x1FFFFF00;
         d2net = d2->des->hostip & 0x1FFFFF00;
      }
      return d1net - d2net;
   } else                 /* Classes don't match, order by class */
   return d1class - d2class;
}

/* Fizzfaldt - wrapper for host_comp */

int host_comp_wrapper(const void* d1, const void* d2)
{
   return host_comp((MULTDATA*)d1, (MULTDATA*)d2);
}


int host_comp(MULTDATA* d1, MULTDATA* d2) {
   /*    return strcmp(get_end_host(d1->des->host), get_end_host(d2->des->host)); XUR - modified */
   int myresult = net_comp(d1, d2);

   if (myresult == 0) /* nets are the same, so let's check local host */
   myresult = d1->des->hostip - d2->des->hostip;

   return myresult;
}


void do_multicheck( CHAR_DATA *ch, char *argument)
{
   /* duh */
   DESCRIPTOR_DATA *d;
   MULTDATA CHARLIST[100];
   char buf[2 * MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];
   int i = 0, j, count = 0, multcount = 0;
   bool first = TRUE;

   buf[0] = '\0';

   /* Construct a master player list */
   for (d=descriptor_list; (d!=NULL) && (i <= 100);d=d->next)
   if (d->character!=NULL&&can_see(ch, d->character))
   {
      CHARLIST[i].des=d;
      i++;
   }

   if (i<2) {
      send_to_char("This command is not usable with only 1 player.\n\r", ch);
      return;
   }

   /* Sort the master player list by net */
   qsort(CHARLIST, i, sizeof(MULTDATA), host_comp_wrapper);

   for (j=0;j<i-1;j++) {
      if (net_comp(&CHARLIST[j], &CHARLIST[j+1]) == 0)
      {
         count++;
         if (first)
         {
            first = FALSE;
            multcount++;
            count++;
            sprintf
            (
               buf, "%s[%3d %2d] %s@%s (%d)%s\n\r", buf,
               CHARLIST[j].des->descriptor,
               CHARLIST[j].des->connected,
               CHARLIST[j].des->original  ?
               CHARLIST[j].des->original->name  :
               CHARLIST[j].des->character ?
               CHARLIST[j].des->character->name : "(none)",
               CHARLIST[j].des->host,
               multcount,
               (CHARLIST[j+1].des->hostip == CHARLIST[j].des->hostip) ? " <--" : ""
            );
         }
         sprintf
         (
            buf,
            "%s[%3d %2d] %s@%s (%d)%s\n\r",
            buf,
            CHARLIST[j+1].des->descriptor,
            CHARLIST[j+1].des->connected,
            CHARLIST[j+1].des->original  ?
            CHARLIST[j+1].des->original->name  :
            CHARLIST[j+1].des->character ?
            CHARLIST[j+1].des->character->name : "(none)",
            CHARLIST[j+1].des->host,
            multcount,
            (CHARLIST[j+1].des->hostip == CHARLIST[j].des->hostip) ? " <--" : ""
         );
      }
      else
      first = TRUE;
   }

   if (count == 0)
   {
      send_to_char("No matches were found.\n\r", ch);
      return;
   }

   sprintf( buf2, "%d user%s, %d possible multichar%s\n\r",
   count, count == 1 ? "" : "s",
   multcount, multcount == 1 ? "" : "s" );
   strcat(buf, buf2);
   page_to_char( buf, ch );
   return;
}


/* Use this to lag out spammers and force them to stop moving */
void do_lagout(CHAR_DATA *ch, char *argument)
{
   char arg[MAX_STRING_LENGTH];
   CHAR_DATA *victim;

   one_argument(argument, arg);
   if ( arg[0] == '\0' || argument[0] == '\0' )
   {
      return;
   }
   victim = get_char_world(ch, arg);
   if (victim == NULL)
   {
      return;
   }
   WAIT_STATE(victim, 24);
   return;
}


/*
* Thanks to Grodyn for pointing out bugs in this function.
*/
void do_force( CHAR_DATA *ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   char arg[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   char interp[MAX_INPUT_LENGTH];

   argument = one_argument(argument, arg);

   if (arg[0] == '\0' || argument[0] == '\0')
   {
      send_to_char("Force whom to do what?\n\r", ch);
      return;
   }

   one_argument(argument, arg2);

   if (!str_cmp(arg2, "delete") || !str_prefix(arg2, "mob"))
   {
      send_to_char("That will NOT be done.\n\r", ch);
      return;
   }

   sprintf(buf, "$n forces you to '%s'.", argument);

   if (!str_cmp(arg, "all"))
   {
      DESCRIPTOR_DATA* d;
      DESCRIPTOR_DATA* d_next;
      CHAR_DATA *vch;

      if (get_trust(ch) < MAX_LEVEL - 3)
      {
         send_to_char("Not at your level!\n\r", ch);
         return;
      }

      for (d = descriptor_list; d != NULL; d = d_next)
      {
         d_next = d->next;

         vch = d->original ? d->original : d->character;

         /* Added awake check to auction - Werv */

         if
         (
            d->connected == CON_PLAYING &&
            d->character != ch &&
            get_trust(vch) < get_trust(ch)
         )
         {
            act(buf, ch, NULL, vch, TO_VICT);
            strcpy(interp, argument);
            interpret(vch, interp);
         }
      }
   }
   else if (!str_cmp(arg, "mobs"))
   {
      CHAR_DATA *vch;
      CHAR_DATA *vch_next;

      if (get_trust(ch) < MAX_LEVEL - 3)
      {
         send_to_char("Not at your level!\n\r", ch);
         return;
      }
      if (!IS_IMMORTAL(ch))
      {
         return;
      }
      for (vch = char_list; vch != NULL; vch = vch_next)
      {
         vch_next = vch->next;

         if (!IS_NPC(vch))
         {
            continue;
         }
         strcpy(interp, argument);
         interpret(vch, interp);
      }
   }
   else if (!str_cmp(arg, "players"))
   {
      DESCRIPTOR_DATA* d;
      DESCRIPTOR_DATA* d_next;
      CHAR_DATA *vch;

      if (get_trust(ch) < MAX_LEVEL - 2)
      {
         send_to_char("Not at your level!\n\r", ch);
         return;
      }

      for (d = descriptor_list; d != NULL; d = d_next)
      {
         d_next = d->next;

         vch = d->original ? d->original : d->character;

         /* Added awake check to auction - Werv */

         if
         (
            d->connected == CON_PLAYING &&
            d->character != ch &&
            get_trust(vch) < get_trust(ch) &&
            vch->level <= LEVEL_HERO
         )
         {
            act(buf, ch, NULL, vch, TO_VICT);
            strcpy(interp, argument);
            interpret(vch, interp);
         }
      }
   }
   else if (!str_cmp(arg, "gods"))
   {
      DESCRIPTOR_DATA* d;
      DESCRIPTOR_DATA* d_next;
      CHAR_DATA *vch;

      if (get_trust(ch) < MAX_LEVEL - 2)
      {
         send_to_char("Not at your level!\n\r", ch);
         return;
      }

      for (d = descriptor_list; d != NULL; d = d_next)
      {
         d_next = d->next;

         vch = d->original ? d->original : d->character;

         /* Added awake check to auction - Werv */

         if
         (
            d->connected == CON_PLAYING &&
            d->character != ch &&
            get_trust(vch) < get_trust(ch) &&
            vch->level > LEVEL_HERO
         )
         {
            act(buf, ch, NULL, vch, TO_VICT);
            strcpy(interp, argument);
            interpret(vch, interp);
         }
      }
   }
   else
   {
      CHAR_DATA *victim;

      if (( victim = get_char_world(ch, arg)) == NULL)
      {
         send_to_char("They aren't here.\n\r", ch);
         return;
      }

      if (victim == ch)
      {
         send_to_char("Aye aye, right away!\n\r", ch);
         return;
      }

      if
      (
         !is_room_owner(ch, victim->in_room) &&
         ch->in_room != victim->in_room &&
         room_is_private(victim->in_room) &&
         !IS_TRUSTED(ch, IMPLEMENTOR)
      )
      {
         send_to_char("That character is in a private room.\n\r", ch);
         return;
      }

      if (get_trust(victim) >= get_trust(ch))
      {
         send_to_char("Do it yourself!\n\r", ch);
         return;
      }

      if (!IS_NPC(victim) && get_trust(ch) < MAX_LEVEL - 3)
      {
         send_to_char("Not at your level!\n\r", ch);
         return;
      }

      act(buf, ch, NULL, victim, TO_VICT);
      interpret(victim, argument);
   }
   send_to_char("Ok.\n\r", ch);
   return;
}



/*
* New routines by Dionysos.
*/
void do_invis( CHAR_DATA *ch, char *argument )
{
   int level;
   char arg[MAX_STRING_LENGTH];

   do_wizireport_update
   (
      ch,
      WIZI_UPDATE
   );
   /* RT code for taking a level argument */
   one_argument( argument, arg );

   if ( arg[0] == '\0' )
   /* take the default path */
   {
      if ( ch->invis_level)
      {
         ch->invis_level = 0;
         act( "$n slowly fades into existence.", ch, NULL, NULL, TO_ROOM );
         send_to_char( "You slowly fade back into existence.\n\r", ch );
      }
      else
      {
         ch->invis_level = get_trust(ch);
         act( "$n slowly fades into thin air.", ch, NULL, NULL, TO_ROOM );
         send_to_char( "You slowly vanish into thin air.\n\r", ch );
      }
   }
   else
   /* do the level thing */
   {
      level = atoi(arg);
      if (level < 2 || level > get_trust(ch))
      {
         send_to_char("Invis level must be between 2 and your level.\n\r", ch);
         return;
      }
      else
      {
         ch->invis_level = level;
         act( "$n slowly fades into thin air.", ch, NULL, NULL, TO_ROOM );
         send_to_char( "You slowly vanish into thin air.\n\r", ch );
      }
   }

   return;
}


void do_incognito( CHAR_DATA *ch, char *argument )
{
   int level;
   char arg[MAX_STRING_LENGTH];

   do_wizireport_update
   (
      ch,
      WIZI_UPDATE
   );
   /* RT code for taking a level argument */
   one_argument( argument, arg );

   if ( arg[0] == '\0' )
   /* take the default path */
   {
      if ( ch->incog_level)
      {
         ch->incog_level = 0;
         act( "$n is no longer cloaked.", ch, NULL, NULL, TO_ROOM );
         send_to_char( "You are no longer cloaked.\n\r", ch );
      }
      else
      {
         ch->incog_level = get_trust(ch);
         act( "$n cloaks $s presence.", ch, NULL, NULL, TO_ROOM );
         send_to_char( "You cloak your presence.\n\r", ch );
      }
   }
   else
   /* do the level thing */
   {
      level = atoi(arg);
      if (level < 2 || level > get_trust(ch))
      {
         send_to_char("Incog level must be between 2 and your level.\n\r", ch);
         return;
      }
      else
      {
         ch->incog_level = level;
         act( "$n cloaks $s presence.", ch, NULL, NULL, TO_ROOM );
         send_to_char( "You cloak your presence.\n\r", ch );
      }
   }

   return;
}



void do_holylight(CHAR_DATA* ch, char* argument)
{
   if (IS_NPC(ch))
   {
      return;
   }

   if (IS_SET(ch->act, ACT_HOLYLIGHT))
   {
      REMOVE_BIT(ch->act, ACT_HOLYLIGHT);
      send_to_char("Holy light mode off.\n\r", ch);
   }
   else
   {
      SET_BIT(ch->act, ACT_HOLYLIGHT);
      send_to_char("Holy light mode on.\n\r", ch);
   }

   return;
}

/* prefix command: it will put the string typed on each line typed */

void do_prefi (CHAR_DATA *ch, char *argument)
{
   send_to_char("You cannot abbreviate the prefix command.\r\n", ch);
   return;
}

void do_prefix (CHAR_DATA *ch, char *argument)
{
   char buf[MAX_INPUT_LENGTH];

   if (argument[0] == '\0')
   {
      if (ch->prefix[0] == '\0')
      {
         send_to_char("You have no prefix to clear.\r\n", ch);
         return;
      }

      send_to_char("Prefix removed.\r\n", ch);
      free_string(ch->prefix);
      ch->prefix = str_dup("");
      return;
   }

   if (ch->prefix[0] != '\0')
   {
      sprintf(buf, "Prefix changed to %s.\r\n", argument);
      free_string(ch->prefix);
   }
   else
   {
      sprintf(buf, "Prefix set to %s.\r\n", argument);
      free_string(ch->prefix);
   }

   ch->prefix = str_dup(argument);
}

void do_astrip(CHAR_DATA *ch, char *argument)
{
   CHAR_DATA *victim;
   AFFECT_DATA* af;
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   int sn;
   bool skipped;
   bool found;
   bool found_loop;

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );

   if ( arg1[0] == '\0' && arg2[0] == '\0')
   {
      strcpy(arg1, "self");
      strcpy(arg2, "all");
   }

   if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if ( !IS_NPC(victim) && (get_trust(victim) > get_trust(ch)) )
   {
      send_to_char( "You failed.\n\r", ch );
      return;
   }

   found_loop = FALSE;
   found = FALSE;
   skipped = FALSE;
   if ( arg2[0] == '\0' )
   {
      while ((af = victim->affected) != NULL)
      {
         while
         (
            !found_loop &&
            af != NULL
         )
         {
            if
            (
               af->type < 1 ||
               af->type >= MAX_SKILL ||
               !skill_table[af->type].strip.astrip_keep
            )
            {
               found_loop = TRUE;
            }
            else
            {
               skipped = TRUE;
               af = af->next;
            }
         }
         if (af == NULL)
         {
            break;
         }
         affect_strip(victim, af->type);
         found = TRUE;
      }
      if (found)
      {
         if (skipped)
         {
            if (victim != ch)
            {
               act
               (
                  "Most affects stripped from $N.",
                  ch,
                  NULL,
                  victim,
                  TO_CHAR
               );
            }
            else
            {
               send_to_char("Most affects stripped from yourself.\n\r", ch);
            }
         }
         else
         {
            if (victim != ch)
            {
               act
               (
                  "All affects stripped from $N.",
                  ch,
                  NULL,
                  victim,
                  TO_CHAR
               );
            }
            else
            {
               send_to_char("All affects stripped from yourself.\n\r", ch);
            }
         }
      }
      else if (victim->affected == NULL)
      {
         send_to_char("No affects found.\n\r", ch);
      }
      else
      {
         send_to_char("No weak affects found to remove.\n\r", ch);
      }
   }
   else if (!str_cmp(arg2, "all"))
   {
      while ((af = victim->affected) != NULL)
      {
         while
         (
            !found_loop &&
            af != NULL
         )
         {
            if
            (
               af->type != gsn_cloak_form &&
               af->type != gsn_strange_form
            )
            {
               found_loop = TRUE;
            }
            else
            {
               skipped = TRUE;
               af = af->next;
            }
         }
         if (af == NULL)
         {
            break;
         }
         affect_strip(victim, af->type);
         found = TRUE;
      }
      if (found)
      {
         if (skipped)
         {
            if (victim != ch)
            {
               act
               (
                  "All affects except cloak/strange form stripped from $N.",
                  ch,
                  NULL,
                  victim,
                  TO_CHAR
               );
            }
            else
            {
               send_to_char("All affects except cloak/strange form stripped from yourself.\n\r", ch);
            }
         }
         else
         {
            if (victim != ch)
            {
               act
               (
                  "All affects stripped from $N.",
                  ch,
                  NULL,
                  victim,
                  TO_CHAR
               );
            }
            else
            {
               send_to_char("All affects stripped from yourself.\n\r", ch);
            }
         }
      }
      else if (victim->affected == NULL)
      {
         send_to_char("No affects found.\n\r", ch);
      }
      else
      {
         send_to_char("No affects found to remove.\n\r", ch);
      }
   }
   else if (is_affected(victim, sn=skill_lookup(arg2)))
   {
      for ( af = victim->affected; af != NULL; af = af->next )
      {
         if ( af->type == sn )
         {
            affect_strip(victim, sn);
            if (victim != ch)
            {
               act
               (
                  "Affect $t stripped from $N.",
                  ch,
                  skill_table[sn].name,
                  victim,
                  TO_CHAR
               );
            }
            else
            {
               send_to_char("Affect ", ch);
               send_to_char(skill_table[sn].name, ch);
               send_to_char(" stripped from yourself.\n\r", ch);
            }
            return;
         }
      }
   }
   else
   {
      send_to_char( "Affect not found.\n\r", ch);
   }
   return;
}

void do_limcounter(CHAR_DATA *ch, char *argument)
{
   OBJ_INDEX_DATA *pObjIndex;
   char buf[200];
   int vnum;

   if (!is_number(argument))
   {
      send_to_char("Only limstat by vnums.\n\r", ch);
      return;
   }

   vnum = atoi(argument);

   pObjIndex = get_obj_index(vnum);
   if (pObjIndex == NULL)
   {
      send_to_char("Not found.\n\r", ch);
      return;
   }

   sprintf(buf, "Obj vnum %d, Max: %d, Count %d.", vnum, pObjIndex->limtotal,
   pObjIndex->limcount);
   send_to_char(buf, ch);
   send_to_char("\n\r", ch);
   return;
}


void do_classes(CHAR_DATA *ch, char *argument)
{
   char buf[MAX_STRING_LENGTH];
   int iRace;
   int iClass;
   send_to_char("\n\r", ch);
   send_to_char("       War Thi Cle Pal A-P Ran Mon Cha N-W Nec Ele", ch);
   send_to_char("\n\r---------------------------------------------------", ch);
   for (iRace = 1;  race_table[iRace].name != NULL; iRace++)
   {
      if (!race_table[iRace].pc_race) continue;
      /* break; */
      sprintf(buf, "\n\r%5s |", pc_race_table[iRace].who_name);
      send_to_char(buf, ch);
      for (iClass = 0; iClass < MAX_CLASS; iClass++)
      {
         if (pc_race_table[iRace].classes[iClass] == 1)
         {
            send_to_char(" X |", ch);
         }else
         {
            send_to_char("   |", ch);
         }
      }
   }
   send_to_char("\n\r", ch);
   return;
}


void do_access(CHAR_DATA *ch, char *argument)
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;

   one_argument(argument, arg);
   if (arg[0] == '\0')
   {
      send_to_char("Allow who to access Builder channel?\n\r", ch);
      return;
   }

   if ( (victim = get_char_world(ch, arg)) == NULL)
   {
      send_to_char("They aren't here.\n\r", ch);
      return;
   }

   if (IS_NPC(victim))
   {
      send_to_char("Not on mobiles.\n\r", ch);
      return;
   }

   if (IS_SET(victim->comm, COMM_BUILDER))
   {
      REMOVE_BIT(victim->comm, COMM_BUILDER);
      act("Builder channel removed from $N.", ch, NULL, victim, TO_CHAR);
      return;
   }

   SET_BIT(victim->comm, COMM_BUILDER);
   act("Builder channel given to $N.", ch, NULL, victim, TO_CHAR);
   send_to_char("You now have Builder channel access.\n\r", victim);
   return;
}


void do_honorbound(CHAR_DATA *ch, char *argument)
{
   char arg[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA *victim;

   one_argument(argument, arg);
   if (arg[0] == '\0')
   {
      send_to_char("Honorbind which player?\n\r", ch);
      return;
   }

   if ( (victim = get_char_world(ch, arg)) == NULL)
   {
      send_to_char("They aren't here.\n\r", ch);
      return;
   }

   if (IS_NPC(victim))
   {
      send_to_char("Not on mobiles.\n\r", ch);
      return;
   }

   if (IS_SET(victim->act, PLR_HONORBOUND))
   {
      sprintf(buf, "%s is no longer honorbound.\n\r", victim->name);
      send_to_char(buf, ch);
      REMOVE_BIT(victim->act, PLR_HONORBOUND);
      send_to_char("You are no longer an honorbound.\n\r", victim);
      victim->pcdata->learned[gsn_multistrike] = 0;
      victim->pcdata->learned[gsn_bandage] = 0;
      victim->pcdata->learned[gsn_counter_parry] = 0;
      return;
   }
   sprintf(buf, "%s is now honorbound.\n\r", victim->name);
   send_to_char(buf, ch);
   SET_BIT(victim->act, PLR_HONORBOUND);
   send_to_char("You are now an honorbound member of Crusader.\n\r", victim);
   victim->pcdata->learned[gsn_multistrike] = 75;
   victim->pcdata->learned[gsn_bandage] = 75;
   victim->pcdata->learned[gsn_counter_parry] = 75;

   return;
}


void do_deathmessage(CHAR_DATA *ch, char *argument)
{
   char buf[MAX_STRING_LENGTH];

   if ( !IS_NPC(ch) )
   {
      smash_tilde( argument );

      if (argument[0] == '\0')
      {
         sprintf(buf, "Your death message is %s\n\r", ch->pcdata->imm_death);
         send_to_char(buf, ch);
         return;
      }

      if ( strstr(argument, ch->name) == NULL)
      {
         send_to_char("You must include your name.\n\r", ch);
         return;
      }

      free_string( ch->pcdata->imm_death );
      ch->pcdata->imm_death = str_dup( argument );

      sprintf(buf, "Your death message is now %s\n\r", ch->pcdata->imm_death);
      send_to_char(buf, ch);
   }
   return;
}

/* seebrand and seehouse (allows morts to see their house or branded imm -Wervdon*/
void do_seehouse(CHAR_DATA *ch, char *argument)
{
   if (IS_SET(ch->act2, WIZ_SEEHOUSE))
   {
      REMOVE_BIT(ch->act2, WIZ_SEEHOUSE);
      send_to_char("Your house no longer sees you.\n\r", ch);
   }
   else
   {
      SET_BIT(ch->act2, WIZ_SEEHOUSE);
      send_to_char("Your house now sees you.\n\r", ch);
   }
   return;
}

void do_seenewbie(CHAR_DATA *ch, char *argument)
{
   if (IS_SET(ch->act2, WIZ_SEENEWBIE)){
      REMOVE_BIT(ch->act2, WIZ_SEENEWBIE);
      send_to_char("Newbies no longer see you.\n\r", ch);
   }
   else
   {
      SET_BIT(ch->act2, WIZ_SEENEWBIE);
      send_to_char("Newbies can now see you.\n\r", ch);
   }
   return;
}

void do_seeworship(CHAR_DATA* ch, char* argument)
{
   if (IS_SET(ch->act2, WIZ_SEEWORSHIP))
   {
      REMOVE_BIT(ch->act2, WIZ_SEEWORSHIP);
      send_to_char("Your worshippers no longer see you.\n\r", ch);
   }
   else
   {
      SET_BIT(ch->act2, WIZ_SEEWORSHIP);
      send_to_char("Your worshippers can now see you.\n\r", ch);
   }
   return;
}

void do_seeclan(CHAR_DATA* ch, char* argument)
{
   if (IS_NPC(ch))
   {
      return;
   }
   if (ch->pcdata->clan == 0)
   {
      REMOVE_BIT(ch->act2, WIZ_SEECLAN);
      send_to_char("You are not part of a clan.\n\r", ch);
      return;
   }
   if (IS_SET(ch->act2, WIZ_SEECLAN))
   {
      REMOVE_BIT(ch->act2, WIZ_SEECLAN);
      send_to_char("Your clan no longer see you.\n\r", ch);
   }
   else
   {
      SET_BIT(ch->act2, WIZ_SEECLAN);
      send_to_char("Your clan can now see you.\n\r", ch);
   }
   return;
}

void do_seebrand(CHAR_DATA *ch, char *argument)
{
   if (ch->level < 55 || IS_NPC(ch))
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }

   if (IS_SET(ch->act2, WIZ_SEEBRAND)){
      REMOVE_BIT(ch->act2, WIZ_SEEBRAND);
      send_to_char("Your followers no longer see you.\n\r", ch);
   }
   else
   {
      SET_BIT(ch->act2, WIZ_SEEBRAND);
      send_to_char("Your followers now see you.\n\r", ch);
   }
   return;
}

void do_max_limits(CHAR_DATA *ch, char *argument)
{
   /*
   OBJ_DATA *obj;
   OBJ_DATA *o_next;
   int count; */
   char arg[MAX_INPUT_LENGTH];
   OBJ_INDEX_DATA *pobj;
   int vnum;
   one_argument(argument, arg);

   if (arg[0] == '\0')
   {
      send_to_char("Max limit on which items?\n\r", ch);
      return;
   }
   vnum = 0;

   if (is_number(arg))
   vnum = atoi(arg);

   if (vnum != 0)
   {
      pobj = get_obj_index(vnum);
      if (pobj == NULL)
      {
         send_to_char("No object by that vnum exists.\n\r", ch);
         return;
      }
      if (pobj->limtotal == 0)
      {
         send_to_char("That item is not limited.\n\r", ch);
         return;
      }
      /*
      count = 0;
      for (obj = object_list; obj != NULL; obj = o_next)
      {
      o_next = obj->next;
      if (obj->pIndexData->vnum == pobj->vnum)
      count++;
      }
      */

      if (pobj->limcount >= pobj->limtotal )
      {
         send_to_char("That item is already at its max count.\n\r", ch);
         return;
      }

      pobj->limcount = pobj->limtotal;
      send_to_char("Item is now maxxed.\n\r", ch);
      return;
   }
   else
   {
      send_to_char("You must give the vnum of the object to be maxxed.\n\r", ch);
      return;
   }

   return;
}

void do_newbie_report(CHAR_DATA *ch, char *argument){
   char buf[MAX_STRING_LENGTH];
   int cnt;
   int total = 0;

   if ( !str_cmp(argument, "reset")){
      if (ch->level < 55){
         send_to_char("You can't do that.\n\r", ch);
         return;
      }
      for (cnt = 0; cnt < MAX_CLASS;cnt++)
      class_count[cnt] = 0;
      save_globals();
      send_to_char("All newbie counts cleared.\n\r", ch);
      return;
   }
   for (cnt = 0; cnt < MAX_CLASS;cnt++)
   total += class_count[cnt];

   send_to_char("\n\rNewbies rolled since last <reset> by class:\n\r", ch);
   sprintf(buf, "Total New Characters: %d\n\r\n\r", total);
   send_to_char(buf, ch);
   for (cnt = 0; cnt < MAX_CLASS;cnt++){
      sprintf(buf, "%12s:   %8ld   (%2.1f%%)\n\r",
      class_table[cnt].name, class_count[cnt],
      ((float)class_count[cnt]/(float)total)*100);
      send_to_char(buf, ch);
   }
   /*    send_to_char("\n\rBy race:\n\r", ch);
   for (cnt = 0; cnt < MAX_PC_RACE;cnt++){
   sprintf(buf, "%12s %ld.\n\r", race_table[cnt].name, race_count[cnt]);
   send_to_char(buf, ch);
   }
   */
   return;
}

void do_lastsite(CHAR_DATA *ch, char *argument){
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA *victim;
   char arg[MAX_INPUT_LENGTH];

   one_argument(argument, arg);
   if (arg[0] == '\0')
   {
      send_to_char("Get whose last site?\n\r", ch);
      return;
   }
   victim = get_char_world(ch, argument);

   if (victim == NULL){
      send_to_char("No such person.\n\r", ch);
      return;
   }

   if (IS_NPC(victim)){
      send_to_char("NPC's dont have sites.\n\r", ch);
      return;
   }
   if (victim->pcdata->last_site[0] != '\0'){
      sprintf(buf, "%s last came from %s.\n\r", victim->name, victim->pcdata->last_site);
      send_to_char(buf, ch);
   }
   if (victim->pcdata->first_site[0] != '\0'){
      sprintf(buf, "%s was rolled from %s.\n\r", victim->name, victim->pcdata->first_site);
      send_to_char(buf, ch);
   }
   if (check_ban(victim->pcdata->last_site, BAN_NEWBIES))
   send_to_char("Coming from a newbie banned site.\n\r", ch);
   if (check_ban(victim->pcdata->first_site, BAN_ALL) ||
   check_ban(victim->pcdata->first_site, BAN_PERMIT))
   send_to_char("Rolled from a fully banned site.\n\r", ch);
   return;
}

void do_log_spec(CHAR_DATA *ch, char *argument){
   CHAR_DATA *victim;
   char arg[MAX_INPUT_LENGTH];

   one_argument(argument, arg);
   if (arg[0] == '\0')
   {
      send_to_char("Log whose conversations?\n\r", ch);
      /* XURI/DRITH - Getting rid of the listing
      int Found = 0;
      CHAR_DATA *vch;

      for ( vch = char_list; vch != NULL; vch = vch->next) {
      if (IS_NPC(vch)) continue;
      if (IS_SET(vch->act2, PLR_LOG_SPEC)) {
      if (!IS_IMMORTAL(vch)) {
      Found = 1;
      send_to_char(vch->name, ch);
      send_to_char("\n\r", ch);
      }
      }
      }
      if (Found == 0) send_to_char("No one is being conversation logged.\n\r", ch);
      */
      return;
   }
   victim = get_char_world(ch, argument);

   if (victim == NULL){
      send_to_char("No such person.\n\r", ch);
      return;
   }

   if (IS_NPC(victim)){
      send_to_char("NPC's cant be logged this way.\n\r", ch);
      return;
   }
   if (IS_SET(victim->act2, PLR_LOG_SPEC)){
      send_to_char("No longer logging their conversations.\n\r", ch);
      REMOVE_BIT(victim->act2, PLR_LOG_SPEC);
   }
   else
   {
      send_to_char("Their conversations are now logged.\n\r", ch);
      SET_BIT(victim->act2, PLR_LOG_SPEC);
   }
   return;
}

void do_allow_rang(CHAR_DATA *ch, char * argument)
{
   allow_rang = !allow_rang;

   if (allow_rang)
   send_to_char("Ranger subclasses can now be chosen.\n\r", ch);
   else
   send_to_char("Ranger subclasses can no longer be chosen.\n\r", ch);
   return;
}

void do_allow_thief(CHAR_DATA *ch, char * argument)
{
   allow_thief = !allow_thief;

   if (allow_thief)
   send_to_char("Thief subclasses can now be chosen.\n\r", ch);
   else
   send_to_char("Thief subclasses can no longer be chosen.\n\r", ch);
   return;
}

void do_allow_elem(CHAR_DATA *ch, char * argument)
{
   allow_elem = !allow_elem;

   if (allow_elem)
   send_to_char("Elementalist subclasses can now be chosen.\n\r", ch);
   else
   send_to_char("Elementalist subclasses can no longer be chosen.\n\r", ch);
   return;
}

void do_new_mem(CHAR_DATA *ch, char *arg){
   char buf[MAX_STRING_LENGTH];
   int cnt = 0;

   sprintf(buf, "NOTES   : %ld\n\r", mem_counts[cnt]);
   send_to_char(buf, ch);
   cnt++;
   sprintf(buf, "BANS    : %ld\n\r", mem_counts[cnt]);
   send_to_char(buf, ch);
   cnt++;
   sprintf(buf, "NAMEBANS: %ld\n\r", mem_counts[cnt]);
   send_to_char(buf, ch);
   cnt++;
   sprintf(buf, "DESCRTOR: %ld\n\r", mem_counts[cnt]);
   send_to_char(buf, ch);
   cnt++;
   sprintf(buf, "GEN_DATA: %ld\n\r", mem_counts[cnt]);
   send_to_char(buf, ch);
   cnt++;
   sprintf(buf, "EX_DESCR: %ld\n\r", mem_counts[cnt]);
   send_to_char(buf, ch);
   cnt++;
   sprintf(buf, "AFFECTS : %ld\n\r", mem_counts[cnt]);
   send_to_char(buf, ch);
   cnt++;
   sprintf(buf, "OBJECTS : %ld\n\r", mem_counts[cnt]);
   send_to_char(buf, ch);
   cnt++;
   sprintf(buf, "CHAR_DAT: %ld\n\r", mem_counts[cnt]);
   send_to_char(buf, ch);
   cnt++;
   sprintf(buf, "PC_DATA : %ld\n\r", mem_counts[cnt]);
   send_to_char(buf, ch);
   cnt++;
   sprintf(buf, "BUFFERS : %ld\n\r", mem_counts[cnt]);
   send_to_char(buf, ch);
   cnt++;
   sprintf(buf, "MOUNTS : %ld\n\r", mem_counts[cnt]);
   send_to_char(buf, ch);
   cnt++;
   sprintf(buf, "ALARMS : %ld\n\r", mem_counts[cnt]);
   send_to_char(buf, ch);
   cnt++;
   sprintf(buf, "NODES : %ld\n\r", mem_counts[cnt]);
   send_to_char(buf, ch);
   cnt++;
   sprintf(buf, "SKILL_MODS : %ld\n\r", mem_counts[cnt]);
   send_to_char(buf, ch);
   sprintf(buf, "Mem used by buffers thus far: %ld\n\r", mem_used[30]);
   send_to_char(buf, ch);
   return;
}

void do_reward( CHAR_DATA *ch, char *argument )
{
   CHAR_DATA *victim;
   char arg1[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   int xp;

   argument = one_argument( argument, arg1 );

   if ( arg1[0] == '\0' )
   {
      send_to_char("Syntax: reward <char> <experience> <reason>\n\r", ch);
      return;
   }

   if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
   {
      send_to_char( "They aren't playing.\n\r", ch );
      return;
   }

   if ( IS_NPC(victim) )
   {
      send_to_char( "Trying to give a mob experience?\n\r", ch );
      return;
   }

   if (ch == victim)
   {
      send_to_char( "You are already quite experienced.\n\r", ch );
      return;
   }

   argument = one_argument( argument, arg1 );

   if ( arg1[0] == '\0' )
   {
      send_to_char("Syntax: reward <char> <experience> <reason>\n\r", ch);
      return;
   }

   xp = atoi(arg1);
   if ((xp < -1000) || (xp > 1000))
   {
      send_to_char( "You can grant anywhere from -1000 to 1000 experience points.\n\r", ch );
      return;
   }

   if ( argument[0] == '\0' )
   {
      send_to_char("Syntax: reward <char> <experience> <reason>\n\r", ch);
      return;
   }

   sprintf(buf, "You %s %d experience %s", xp < 0 ? "lose" : "earn", abs(xp), argument);
   act( buf, ch, NULL, victim, TO_VICT );

   sprintf(buf, "$N %s %d experience %s", xp < 0 ? "loses" : "earns", abs(xp), argument);
   act( buf, victim, NULL, victim, TO_NOTVICT );

   sprintf(buf, "You grant $N %d experience %s", xp, argument);
   act( buf, ch, NULL, victim, TO_CHAR );

   gain_exp(victim, xp);

   return;
}

void do_notick(CHAR_DATA *ch, char * argument)
{
   if (IS_NPC(ch))
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }
   if (ch->pcdata->special != SUBCLASS_TEMPORAL_MASTER)
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }
   if (!IS_SET(ch->wiznet, WIZ_TICKS))
   SET_BIT(ch->wiznet, WIZ_TICKS);
   else
   REMOVE_BIT(ch->wiznet, WIZ_TICKS);

   if (IS_SET(ch->wiznet, WIZ_TICKS))
   send_to_char("You now sense the passage of time.\n\r", ch);
   else
   send_to_char("You no longer sense the passage of time.\n\r", ch);

   return;
}

void do_immrp(CHAR_DATA *ch, char * argument)
{
   char buf[MAX_STRING_LENGTH];
   char arg1[MAX_STRING_LENGTH];
   int dam;

   if (!IS_IMMORTAL(ch) || IS_NPC(ch))
   return;
   if (argument[0] == '\0')
   {
      send_to_char("Current immortal rp commands, use carefully.\n\r", ch);
      sprintf(buf, "Darkness: %d\n\r", immrp_darkness);
      send_to_char(buf, ch);
      send_to_char("Blood: changes all water to blood.\n\r", ch);
      send_to_char("Quake: not a toggle, careful with this.\n\r", ch);
      send_to_char("Meteor: not a toggle, careful with this.\n\r", ch);
      send_to_char("Rift: not a toggle, careful with this.\n\r", ch);
      send_to_char("Swarm: attacks everyone with a demon.\n\r", ch);
      send_to_char("Kidnap: Causes the victim to be kidnapped by a demon.\n\r", ch);
      send_to_char("Revolt: Causes glyndane to go into revolt.\n\r", ch);
      return;
   }
   if (!str_cmp(argument, "revolt"))
   {
      immrp_revolt = !immrp_revolt;
      if (immrp_revolt)
      send_to_char("Revolution toggled on.\n\r", ch);
      else
      send_to_char("Revolution toggled off.\n\r", ch);
      return;
   }
   if (!str_cmp(argument, "blood"))
   {
      OBJ_DATA* obj;
      immrp_blood = !immrp_blood;
      if (immrp_blood)
      for ( obj = object_list; obj != NULL; obj = obj->next )
      {
         if ( obj->item_type == ITEM_FOUNTAIN || obj->item_type == ITEM_DRINK_CON)
         obj->value[2] = 14;
      }
   }

   if (!str_cmp(argument, "darkness"))
   immrp_darkness = !immrp_darkness;

   if (!str_cmp(argument, "quake"))
   {
      CHAR_DATA *vch;
      CHAR_DATA *vch_next;
      int sn;

      sn = gsn_earthquake;

      send_to_char( "All of Thera trembles beneath your feet!\n\r", ch );

      for ( vch = char_list; vch != NULL; vch = vch_next )
      {
         vch_next        = vch->next;
         if (IS_IMMORTAL(vch)) continue;
         if ( vch->in_room == NULL )
         continue;
         if (vch->level < 10)
         {
            send_to_char("The gods protect you from Thera's rumbling.\n\r", vch);
            continue;
         }
         if (IS_FLYING(vch))
         damage(vch, vch, 0, sn, DAM_BASH, GLOBAL_QUAKE_DAM_TYPE);
         else
         damage(vch, vch, vch->max_hit/5, sn, DAM_BASH, GLOBAL_QUAKE_DAM_TYPE);
         if (vch != NULL)
         send_to_char( "Thera trembles and shivers violently.\n\r", vch );
      }
   }

   if (!str_cmp(argument, "rift"))
   {
      CHAR_DATA *vch;
      CHAR_DATA *vch_next;
      int sn;

      sn = gsn_meteor;

      send_to_char( "The ground splits in volcanic rifts!\n\r", ch );

      for ( vch = char_list; vch != NULL; vch = vch_next )
      {
         vch_next        = vch->next;
         if (IS_IMMORTAL(vch)) continue;
         if ( vch->in_room == NULL )
         continue;
         if (vch->level < 10)
         {
            send_to_char("The gods protect you from the volcanic ash.\n\r", vch);
            continue;
         }
         dam = vch->hit/2;
         if (dam < 50) dam = 50;
         if (dam > 300) dam = 300;
         send_to_char("A volcanic rift opens beneath your feat!\n\r", vch);
         damage(vch, vch, dam, sn, DAM_BASH, GLOBAL_RIFT_DAM_TYPE);
      }
   }

   if (!str_cmp(argument, "meteor"))
   {
      CHAR_DATA *vch;
      CHAR_DATA *vch_next;
      int sn;

      sn = gsn_meteor;

      send_to_char( "Meteors rain down from the skies!\n\r", ch );

      for ( vch = char_list; vch != NULL; vch = vch_next )
      {
         vch_next        = vch->next;
         if (IS_IMMORTAL(vch)) continue;
         if ( vch->in_room == NULL )
         continue;
         if (vch->level < 10)
         {
            send_to_char("The gods protect you from the meteors.\n\r", vch);
            continue;
         }
         dam = vch->hit/2;
         if (dam < 50) dam = 50;
         if (dam > 300) dam = 300;
         damage(vch, vch, dam, sn, DAM_BASH, GLOBAL_METEOR_DAM_TYPE);
      }
   }
   argument = one_argument( argument, arg1 );
   if (!str_cmp(arg1, "kidnap"))
   {
      CHAR_DATA *victim;
      CHAR_DATA *demon;
      ROOM_INDEX_DATA *pRoomIndex;

      victim = get_char_world(ch, argument);
      if (victim == NULL)
      {
         send_to_char("Character not found.\n\r", ch);
         return;
      }
      demon = create_mobile(get_mob_index(MOB_VNUM_DEMON1));
      make_demon(demon, number_range(1, 8));
      char_to_room(demon, victim->in_room);
      act("$n arrives from the air.", demon, NULL, NULL, TO_ROOM);
      act("The demon grabs $n in its claws and flies away!", victim, demon, NULL, TO_ROOM);
      send_to_char("The demon grabs you in its claws and flies away!\n\r", victim);

      if ( (pRoomIndex = get_random_room(victim, RANDOM_NORMAL)) == NULL )
      {
         bug("NULL room from get_random_room.", 0);
         return;
      }
      char_from_room(demon);
      char_from_room(victim);
      char_to_room(demon, pRoomIndex);
      char_to_room(victim, pRoomIndex);
      send_to_char("The demon drops you from its claws and prepares to have lunch.\n\r", victim);
      do_observe(victim, "", LOOK_AUTO);
      sprintf(buf, "Help! I'm being attacked by %s!", PERS(demon, victim));
      do_myell(victim, buf);
      multi_hit(demon, victim, TYPE_UNDEFINED);

      return;
   }

   return;
}

void do_scheck(CHAR_DATA *ch, char *argument)
{
   CHAR_DATA *victim;
   DESCRIPTOR_DATA *d;
   int save_mal;
   int save_breath;
   int save_trans;
   int save_spells;
   int save_death;
   char buf[MAX_STRING_LENGTH];

   if (!IS_IMMORTAL(ch))
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }
   victim = NULL;
   if (argument[0] != '\0')
   {
      victim = get_char_world(ch, argument);
      if (victim == NULL)
      {
         send_to_char("Character not found.\n\r", ch);
         return;
      }
   }
   send_to_char("\n\r", ch);
   send_to_char("Name        mal  brt  tra  spl  death\n\r", ch);
   send_to_char("-------------------------------------\n\r", ch);
   if (victim != NULL)
   {
      save_spells = 50 - victim->saving_throw * 1;
      save_spells -= victim->saving_spell*2;
      if (save_spells > 65 && save_spells < 100)
      save_spells = 65 + (save_spells-65)/2;
      if (save_spells >= 100) save_spells = 75 + (save_spells-65)/3;
      save_spells = URANGE( 5, save_spells, 92 );
      save_mal = 50 - victim->saving_throw * 1;
      save_mal -= victim->saving_maledict*2;
      if (save_mal > 65 && save_mal < 100)
      save_mal = 65 + (save_mal-65)/2;
      if (save_mal >= 100) save_mal = 75 + (save_mal-65)/3;
      save_mal = URANGE( 5, save_mal, 92 );
      save_trans = 50 - victim->saving_throw * 1;
      save_trans -= victim->saving_transport*2;
      if (save_trans > 65 && save_trans < 100)
      save_trans = 65 + (save_trans-65)/2;
      if (save_trans >= 100) save_trans = 75 + (save_trans-65)/3;
      save_trans = URANGE( 5, save_trans, 92 );
      save_breath = 50 - victim->saving_throw * 1;
      save_breath -= victim->saving_breath*2;
      if (save_breath > 65 && save_breath < 100)
      save_breath = 65 + (save_breath-65)/2;
      if (save_breath >= 100) save_breath = 75 + (save_breath-65)/3;
      save_breath = URANGE( 5, save_breath, 92 );
      save_death = 50 - victim->saving_throw * 1;
      save_death -= victim->saving_spell*2;
      save_death -= victim->saving_transport*2;
      save_death -= victim->saving_maledict*2;
      save_death -= victim->saving_breath*2;
      if (save_death > 65 && save_death < 100)
      save_death = 65 + (save_death-65)/2;
      if (save_death >= 100) save_death = 75 + (save_death-65)/3;
      save_death = URANGE( 5, save_death, 92 );
      sprintf(buf, "%10s %3d  %3d  %3d  %3d  %3d\n\r", victim->name,
      save_mal, save_breath, save_trans, save_spells, save_death);
      send_to_char(buf, ch);
      return;
   }
   for ( d = descriptor_list; d != NULL; d = d->next )
   {
      if ( d->connected != CON_PLAYING || !can_see( ch, d->character ) )
      continue;
      victim   = ( d->original != NULL ) ? d->original : d->character;
      if (victim->level != 51) continue;
      save_spells = 35 - victim->saving_throw * 1;
      save_spells -= victim->saving_spell*2;
      if (save_spells > 65)
      save_spells = 65 + (save_spells-65)/2;
      save_spells = URANGE( 5, save_spells, 92 );
      save_mal = 35 - victim->saving_throw * 1;
      save_mal -= victim->saving_maledict*2;
      if (save_mal > 65)
      save_mal = 65 + (save_mal-65)/2;
      save_mal = URANGE( 5, save_mal, 92 );
      save_trans = 35 - victim->saving_throw * 1;
      save_trans -= victim->saving_transport*2;
      if (save_trans > 65)
      save_trans = 65 + (save_trans-65)/2;
      save_trans = URANGE( 5, save_trans, 92 );
      save_breath = 50 - victim->saving_throw * 1;
      save_breath -= victim->saving_breath*2;
      if (save_breath > 65)
      save_breath = 65 + (save_breath-65)/2;
      save_breath = URANGE( 5, save_breath, 92 );
      save_death = 50 - victim->saving_throw * 1;
      save_death -= victim->saving_spell*2;
      save_death -= victim->saving_transport*2;
      save_death -= victim->saving_maledict*2;
      save_death -= victim->saving_breath*2;
      if (save_death > 65)
      save_death = 65 + (save_death-65)/2;
      save_death = URANGE( 5, save_death, 92 );
      sprintf(buf, "%10s %3d  %3d  %3d  %3d  %3d\n\r", victim->name,
      save_mal, save_breath, save_trans, save_spells, save_death);
      send_to_char(buf, ch);
   }
   return;
}

void show_roster(CHAR_DATA* ch, ROSTER* proster)
{
   sh_int count = 0;
   char buf [MAX_INPUT_LENGTH];
   bool shown = FALSE;

   if (proster == NULL)
   {
      send_to_char("Empty.\n\r", ch);
      return;
   }
   while (proster)
   {
      if
      (
         IS_TRUSTED(ch, ANGEL) ||
         strstr(proster->data, "(Denied)") == NULL
      )
      {
         shown = TRUE;
         sprintf(buf, "%2d %s\r", ++count, proster->data);
         send_to_char(buf, ch);
      }
      proster = proster->next;
   }
   if (!shown)
   {
      send_to_char("Empty.\n\r", ch);
      return;
   }
}

void do_roster(CHAR_DATA* ch, char* argument)
{
   sh_int num;
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   char buf [MAX_STRING_LENGTH];

   if (IS_NPC(ch))
   {
      send_to_char("Not with an NPC.\n\r", ch);
      return;
   }
   if
   (
      !IS_TRUSTED(ch, ANGEL) &&
      (
         ch->house == 0 ||
         ch->pcdata->induct != 5
      ) &&
      (
         ch->pcdata->clan == 0 ||
         clan_table[ch->pcdata->clan].leader_id != ch->id
      )
   )
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }

   argument = one_argument(argument, arg1);
   argument = one_argument(argument, arg2);
   if (argument[0] != '\0')
   {
      send_to_char("Too many parameters.\n\r", ch);
      do_roster(ch, "");
      return;
   }
   if
   (
      arg2[0] != '\0' &&
      !IS_TRUSTED(ch, ANGEL)
   )
   {
      send_to_char("Invalid parameters.\n\r", ch);
      do_roster(ch, "");
      return;
   }
   if
   (
      !str_cmp(arg1, "house") &&
      (
         IS_TRUSTED(ch, ANGEL) ||
         (
            ch->house != 0 &&
            ch->pcdata->induct == 5
         )
      )
   )
   {
      if (arg2[0] == '\0')
      {
         num = ch->house;
         if (num == 0)
         {
            send_to_char("You are not housed.\n\r", ch);
         }
      }
      else
      {
         num = house_lookup(arg2);
      }
      if (num)
      {
         sprintf
         (
            buf,
            "%s's houseroster:\n\r",
            capitalize(house_table[num].name)
         );
         send_to_char(buf, ch);
         show_roster(ch, house_table[num].proster);
         return;
      }
      if (arg2[0] == '\0')
      {
         send_to_char("Show the roster for which House?\n\r", ch);
      }
      else
      {
         send_to_char("No such house exists.\n\r", ch);
      }
      return;
   }
   if
   (
      !str_cmp(arg1, "clan") &&
      (
         IS_TRUSTED(ch, ANGEL) ||
         (
            ch->pcdata->clan != 0 &&
            clan_table[ch->pcdata->clan].leader_id == ch->id
         )
      )
   )
   {
      if (arg2[0] == '\0')
      {
         num = ch->pcdata->clan;
         if (num < 1)
         {
            send_to_char("You are not a member of a clan.\n\r", ch);
         }
      }
      else
      {
         num = clan_lookup(arg2);
      }
      if (num > 0)
      {
         sprintf(buf, "%s's clanroster:\n\r", clan_table[num].who_name);
         send_to_char(buf, ch);
         show_roster(ch, clan_table[num].proster);
         return;
      }
      if (arg2[0] == '\0')
      {
         send_to_char("Show the roster for which Clan?\n\r", ch);
      }
      else
      {
         send_to_char("No such clan exists.\n\r", ch);
      }
      return;
   }
   if
   (
      IS_TRUSTED(ch, ANGEL) &&
      !str_cmp(arg1, "brand")
   )
   {
      if (!IS_TRUSTED(ch, IMMORTAL))
      {
         send_to_char
         (
            "You are not powerful enough to see brandrosters.\n\r",
            ch
         );
         return;
      }
      if (arg2[0] == '\0')
      {
         /* Check for brand */
         for (num = 0; worship_table[num].name != NULL; num++)
         {
            if (!str_cmp(worship_table[num].name, ch->name))
            {
               break;
            }
         }
         if (worship_table[num].name == NULL)
         {
            sprintf
            (
               buf,
               "You (%s) are not in the worship table.\n\r"
               "Let the IMPS and/or coders know.\n\r"
               "If (%s) is not your name, report this bug.\n\r",
               ch->name,
               ch->name
            );
            send_to_char(buf, ch);
            num = -1;
         }
      }
      else
      {
         /* Check for brand */
         for (num = 0; worship_table[num].name != NULL; num++)
         {
            if (!str_cmp(worship_table[num].name, arg2))
            {
               break;
            }
         }
         if (worship_table[num].name == NULL)
         {
            num = -1;
         }
      }
      if (num > -1)
      {
         if
         (
            !IS_TRUSTED(ch, IMPLEMENTOR) &&
            !strcmp(worship_table[num].name, "Styx")
         )
         {
            /* Only imps can view Styx's brandroster */
            send_to_char
            (
               "The Dread Lord's followers are kept in strictest secrecy.\n\r",
               ch
            );
            return;
         }
         sprintf(buf, "%s's brandroster:\n\r", worship_table[num].name);
         send_to_char(buf, ch);
         show_roster(ch, worship_table[num].proster);
         return;
      }
      if (arg2[0] == '\0')
      {
         send_to_char("Show the roster for which God or Goddess?\n\r", ch);
      }
      else
      {
         send_to_char("No such God or Goddess exists.\n\r", ch);
      }
      return;
   }
   if
   (
      IS_IMMORTAL(ch) &&
      /*
      Changed from angel to just being immortal.
      It already refuses 2nd parameter for < angels,
      so this will only let you see your own
      worship list at 52
      IS_TRUSTED(ch, ANGEL) &&
      */
      !str_cmp(arg1, "worship")
   )
   {
      if (arg2[0] == '\0')
      {
         /* Check for name */
         for (num = 0; worship_table[num].name != NULL; num++)
         {
            if (!str_cmp(worship_table[num].name, ch->name))
            {
               break;
            }
         }
         if (worship_table[num].name == NULL)
         {
            sprintf
            (
               buf,
               "You (%s) are not in the worship table.\n\r"
               "Let the IMPS and/or coders know.\n\r"
               "If (%s) is not your name, report this bug.\n\r",
               ch->name,
               ch->name
            );
            send_to_char(buf, ch);
            num = -1;
         }
      }
      else
      {
         /* Check for name */
         for (num = 0; worship_table[num].name != NULL; num++)
         {
            if (!str_cmp(worship_table[num].name, arg2))
            {
               break;
            }
         }
         if (worship_table[num].name == NULL)
         {
            num = -1;
         }
      }
      if (num > -1)
      {
         sprintf(buf, "%s's worshiproster:\n\r", worship_table[num].name);
         send_to_char(buf, ch);
         show_roster(ch, worship_table[num].pwroster);
         return;
      }
      if (arg2[0] == '\0')
      {
         send_to_char
         (
            "Show the worship roster for which God or Goddess?\n\r",
            ch
         );
      }
      else
      {
         send_to_char("No such God or Goddess exists.\n\r", ch);
      }
      return;
   }
   if (IS_TRUSTED(ch, ANGEL))
   {
      send_to_char
      (
         "View what kind of roster?\n\rHouse, Clan, Worship, or brand?\n\r"
         "Syntax:\n\r"
         "  roster clan\n\r"
         "  roster clan clanalias\n\r"
         "  roster house\n\r"
         "  roster house housename\n\r"
         "  roster brand\n\r"
         "  roster brand godname\n\r"
         "  roster worship\n\r"
         "  roster worship godname\n\r",
         ch
      );
   }
   else
   {
      sprintf
      (
         buf,
         "View what roster?\n\r"
         "Syntax:\n\r"
         "%s%s",
         ch->house != 0 && ch->pcdata->induct == 5 ? "  roster house\n\r" : "",
         (
            ch->pcdata->clan != 0 &&
            clan_table[ch->pcdata->clan].leader_id == ch->id
         ) ? "  roster clan\n\r" : ""
      );
      send_to_char(buf, ch);
   }
}

void do_irvcheck(CHAR_DATA *ch, char *argument)
{
   CHAR_DATA *victim;
   int cnt;
   int skip = TRUE;
   char arg[MAX_STRING_LENGTH];
   char * arg2;

   arg2 = one_argument(argument, arg);
   if (!str_cmp(arg, "all"))
   skip = FALSE;
   if (!skip)
   argument = arg2;
   victim = get_char_world(ch, argument);
   if (victim == NULL)
   {
      send_to_char("They are not present.\n\r", ch);
      return;
   }
   send_to_char("\n\rImmunities, Resists, and Vulnerabilities for ", ch);
   if (!IS_NPC(victim))
   send_to_char(victim->name, ch);
   else
   send_to_char(victim->short_descr, ch);
   send_to_char(":\n\r", ch);
   for (cnt = 1; cnt <= DAM_SPELLS; cnt++)
   {
      if (skip && check_immune(victim, cnt) == IS_NORMAL) continue;
      switch(cnt)
      {
         default: send_to_char("Unknown: ", ch); break;
         case DAM_BASH: send_to_char("Bashing: ", ch); break;
         case DAM_SLASH: send_to_char("Slashing: ", ch); break;
         case DAM_PIERCE: send_to_char("Piercing: ", ch); break;
         case DAM_FIRE: send_to_char("Fire: ", ch); break;
         case DAM_COLD: send_to_char("Cold: ", ch); break;
         case DAM_LIGHTNING: send_to_char("Lightning: ", ch); break;
         case DAM_ACID: send_to_char("Acid: ", ch); break;
         case DAM_POISON: send_to_char("Poison: ", ch); break;
         case DAM_NEGATIVE: send_to_char("Negative: ", ch); break;
         case DAM_HOLY: send_to_char("Holy: ", ch); break;
         case DAM_ENERGY: send_to_char("Energy: ", ch); break;
         case DAM_MENTAL: send_to_char("Mental: ", ch); break;
         case DAM_DISEASE: send_to_char("Disease: ", ch); break;
         case DAM_DROWNING: send_to_char("Drowning: ", ch); break;
         case DAM_LIGHT: send_to_char("Light: ", ch); break;
         case DAM_OTHER: send_to_char("Other: ", ch); break;
         case DAM_HARM: send_to_char("Harm: ", ch); break;
         case DAM_CHARM: send_to_char("Charm: ", ch); break;
         case DAM_SOUND: send_to_char("Sound: ", ch); break;
         case DAM_IRON: send_to_char("Iron: ", ch); break;
         case DAM_WOOD: send_to_char("Wood: ", ch); break;
         case DAM_METAL: send_to_char("Metal: ", ch); break;
         case DAM_SILVER: send_to_char("Silver: ", ch); break;
         case DAM_SPELLS: send_to_char("Spells: ", ch); break;
         case DAM_NATURE: send_to_char("Nature: ", ch); break;
      }
      switch(check_immune(victim, cnt))
      {
         default: send_to_char("Unknown\n\r", ch); break;
         case IS_IMMUNE: send_to_char("Immune\n\r", ch); break;
         case IS_RESISTANT: send_to_char("Resistant\n\r", ch); break;
         case IS_VULNERABLE: send_to_char("Vulnerable\n\r", ch); break;
         case IS_NORMAL:  send_to_char("Normal\n\r", ch); break;
      }
   }
   send_to_char("\n\rNotes: Material types: wood, silver, metal, iron and spells stack with all other types.  The rest do not stack.  Weapon and Magic types are already factored into individual types and thus do not show.\n\r", ch);
   return;
}

void do_material_check(CHAR_DATA* ch, char* argument)
{
   int cnt;
   OBJ_DATA* obj;
   int number;

   char buf[MAX_STRING_LENGTH];

   if (!IS_IMMORTAL(ch))
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }

   if (argument[0] != '\0' && IS_TRUSTED(ch, ANGEL))
   {
      BUFFER* buffer;
      OBJ_DATA* in_obj;
      bool found;
      int number = 0;
      int max_found;
      char* name = NULL;
      char* name2 = NULL;
      char* name3 = NULL;

      if (is_number(argument))
      {
         number = atoi(argument) + 1;
         if
         (
            number < 1 ||
            number > material_length - 1 ||
            material_table[number].lump_name == NULL ||
            material_table[number].name == NULL
         )
         {
            send_to_char("That is not a valid alchemy material.\n\r", ch);
            return;
         }
         name = material_table[number].name;
      }
      else
      {
         for (cnt = 0; cnt < material_length - 1; cnt++)
         {
            if
            (
               material_table[cnt].lump_name != NULL &&
               material_table[cnt].name != NULL &&
               !str_cmp(material_table[cnt].name, argument)
            )
            {
               name = material_table[cnt].name;
               break;
            }
         }
         if (name == NULL)
         {
            send_to_char("That is not a valid alchemy material.\n\r", ch);
            return;
         }
      }
      /* Some materials are equivalent */
      if (!str_cmp(name, "sandstone") || !str_cmp(name, "sand"))
      {
         name  = "sand";
         name2 = "sandstone";
      }
      else if (!str_cmp(name, "adamantium") || !str_cmp(name, "adamantite"))
      {
         name  = "adamantite";
         name2 = "adamantium";
      }
      else if
      (
         !str_cmp(name, "skin") ||
         !str_cmp(name, "meat") ||
         !str_cmp(name, "flesh")
      )
      {
         name  = "flesh";
         name2 = "skin";
         name3 = "meat";
      }
      else if (!str_cmp(name, "rock") || !str_cmp(name, "stone"))
      {
         name  = "stone";
         name2 = "rock";
      }
      else if
      (
         !str_cmp(name, "dirt") ||
         !str_cmp(name, "earth") ||
         !str_cmp(name, "clay")
      )
      {
         name  = "earth";
         name2 = "dirt";
         name3 = "clay";
      }
      else if (!str_cmp(name, "corundum") || !str_cmp(name, "ruby"))
      {
         name  = "ruby";
         name2 = "corundum";
      }

      found = FALSE;
      number = 0;
      max_found = 200;

      buffer = new_buf();

      sprintf
      (
         buf,
         "Searching for %s%s%s%s%s:\n\r",
         name,
         name2 == NULL ?
         "" :
         name3 == NULL ? " and " : ", ",
         name2 == NULL ? "" : name2,
         name3 == NULL ? "" : ", and ",
         name3 == NULL ? "" : name3
      );
      add_buf(buffer, buf);
      for (obj = object_list; obj != NULL; obj = obj->next)
      {
         if
         (
            !can_see_obj(ch, obj) ||
            (
               str_cmp(name, obj->material) &&
               (
                  name2 == NULL ||
                  str_cmp(name2, obj->material)
               ) &&
               (
                  name3 == NULL ||
                  str_cmp(name3, obj->material)
               )
            )
         )
         {
            continue;
         }

         found = TRUE;
         number++;
         for (in_obj = obj; in_obj->in_obj != NULL; in_obj = in_obj->in_obj)
         {
         }

         if
         (
            in_obj->carried_by != NULL &&
            can_see(ch, in_obj->carried_by) &&
            in_obj->carried_by->in_room != NULL
         )
         {
            sprintf
            (
               buf,
               "%3d) %s is carried by %s [Room %d]\n\r",
               number,
               obj->short_descr,
               PERS(in_obj->carried_by, ch),
               in_obj->carried_by->in_room->vnum
            );
         }
         else if
         (
            in_obj->in_room != NULL &&
            can_see_room(ch, in_obj->in_room)
         )
         {
            sprintf
            (
               buf,
               "%3d) %s is in %s [Room %d]\n\r",
               number,
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
               "%3d) %s is somewhere\n\r",
               number,
               obj->short_descr
            );
         }

         add_buf(buffer, buf);

         if (number >= max_found)
         {
            break;
         }
      }

      if (!found)
      {
         send_to_char("Nothing made out of that in heaven or earth.\n\r", ch);
      }
      else
      {
         page_to_char(buf_string(buffer), ch);
      }

      free_buf(buffer);
      return;
   }


   for (cnt = 0; cnt < material_length - 1; cnt++)
   {
      if (material_table[cnt].lump_name == NULL)
      {
         continue;
      }
      number = 0;
      for (obj = object_list; obj != NULL; obj = obj->next)
      {
         if
         (
            !can_see_obj(ch, obj) ||
            str_cmp(material_table[cnt].name, obj->material)
         )
         {
            continue;
         }
         number++;
      }
      sprintf(buf, "%s  Count is %d.\n", material_table[cnt].name, number);
      send_to_char(buf, ch);
   }
   return;
}

void do_novice(CHAR_DATA *ch, char *argument)
{
   int induct;
   if (IS_NPC(ch)) return;

   induct = ch->pcdata->induct;
   if (induct < 3)
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }
   ch->pcdata->induct = 3;
   do_induct(ch, argument);
   ch->pcdata->induct = induct;
   return;
}

void do_educate(CHAR_DATA* ch, char* argument)
{
   char arg[MAX_INPUT_LENGTH];
   DESCRIPTOR_DATA* d;
   DESCRIPTOR_DATA* d_next;
   CHAR_DATA* vch = NULL;

   argument = one_argument(argument, arg);

   if (arg[0] == '\0')
   {
      send_to_char("Educate whom?\n\r", ch);
      return;
   }
   for (d = descriptor_list; d != NULL; d = d_next)
   {
      d_next = d->next;
      vch = d->original ? d->original : d->character;
      if
      (
         d->connected == CON_PLAYING &&
         d->character != ch &&
         can_see(ch, vch) &&
         !str_cmp(arg, vch->name)
      )
      {
         break;
      }
      vch = NULL;
   }
   if (vch == NULL)
   {
      send_to_char("They are not here.\n\r", ch);
      return;
   }
   if (get_trust(ch) < get_trust(vch))
   {
      send_to_char("They should be educating you.\n\r", ch);
      return;
   }
   if (get_trust(ch) == get_trust(vch))
   {
      send_to_char("Just form a study group.\n\r", ch);
      return;
   }
   if (argument[0] == '\0')
   {
      send_to_char("Educate them in what?\n\r", ch);
      return;
   }
   act("$n educates you in '$t'.", ch, argument, vch, TO_VICT);
   do_help(vch, argument);
   act("$N ends up a little wiser.", ch, NULL, vch, TO_CHAR);
   return;
}

void do_cast_silent(CHAR_DATA* ch, char* argument)
{
   if (IS_NPC(ch))
   {
      send_to_char("NPCs cannot cast silently.\n\r", ch);
      return;
   }
   if (IS_SET(ch->comm2, COMM_CAST_SILENT))
   {
      REMOVE_BIT(ch->comm2, COMM_CAST_SILENT);
      send_to_char("You no longer cast silently.\n\r", ch);
   }
   else
   {
      SET_BIT(ch->comm2, COMM_CAST_SILENT);
      send_to_char("You now cast silently.\n\r", ch);
   }
}

void do_last_logon(CHAR_DATA *ch, char *argument)
{
   char buf[MAX_INPUT_LENGTH];
   char arg[MAX_INPUT_LENGTH];
   char strsave[MAX_INPUT_LENGTH];
   FILE* fp;
   DESCRIPTOR_DATA* d;
   CHAR_DATA* vch;
   char* name;
   int trust = 0;
   int level = 0;
   int logon = 0;
   char* suf;
   int curr_day;
   char* word;
   bool last_loop = FALSE;
   MUD_TIME time_m;
   TIMEVAL time_val;
   bool storage = FALSE;

   argument = one_argument(argument, arg);
   if (arg[0] == '\0')
   {
      send_to_char("Lookup whose last logon?\n\r", ch);
      return;
   }
   for (d = descriptor_list; d != NULL; d = d->next)
   {
      if (d->connected != CON_PLAYING)
      {
         continue;
      }
      vch = d->original ? d->original : d->character;
      name = vch->name;
      if (str_cmp(name, arg))
      {
         continue;
      }
      if
      (
         can_see(ch, vch) ||
         get_trust(ch) >= get_trust(vch)
      )
      {
         act
         (
            "$N is online right now!",
            ch,
            NULL,
            vch,
            TO_CHAR
         );
         return;
      }
      else
      {
         act
         (
            "$N is too high for you to mess with.",
            ch,
            NULL,
            vch,
            TO_CHAR
         );
         return;
      }
   }
   /* Not Logged On */
   fclose(fpReserve);
#if defined(unix)
   /* decompress if .gz file exists */
   sprintf(strsave, "%s%s%s", PLAYER_DIR, capitalize(arg), ".gz");
   if ((fp = fopen(strsave, "r")) != NULL)
   {
      fclose(fp);
      sprintf(buf, "gzip -dfq %s", strsave);
      system(buf);
   }
#endif

   sprintf(strsave, "%s%s.plr", PLAYER_DIR, capitalize(arg));
   if ((fp = fopen(strsave, "r")) == NULL)
   {
      /* Does not exist in player directory, check storage */
#if defined(unix)
      /* decompress if .gz file exists */
      sprintf(strsave, "%s%s%s", STORAGE_DIR, capitalize(arg), ".gz");
      if ((fp = fopen(strsave, "r")) != NULL)
      {
         fclose(fp);
         sprintf(buf, "gzip -dfq %s", strsave);
         system(buf);
      }
#endif

      sprintf(strsave, "%s%s.plr", STORAGE_DIR, capitalize(arg));
      fp = fopen(strsave, "r");
      storage = TRUE;
   }
   if (fp != NULL)
   {
      while (TRUE)
      {
         if (feof(fp))
         {
            bug("last_login reached EOF on fileread", 0);
            break;
         }
         word = fread_word(fp);
         if (!strcmp(word, "Levl"))
         {
            level = fread_number(fp);
         }
         else if (!strcmp(word, "Tru"))
         {
            trust = fread_number(fp);
         }
         else if (!strcmp(word, "LogO"))
         {
            logon = fread_number(fp);
         }
         else
         {
            fread_to_eol(fp);
         }
         free_string(word);
         if
         (
            logon &&
            level &&
            (
               trust ||
               last_loop
            )
         )
         {
            break;
         }
         if (level)
         {
            last_loop = TRUE;
         }
      }
      fclose(fp);
      if (!trust)
      {
         trust = level;
      }
      if (get_trust(ch) < trust)
      {
         act
         (
            "$t is too high for you to mess with.",
            ch,
            capitalize(arg),
            NULL,
            TO_CHAR
         );
         fpReserve = fopen(NULL_FILE, "r");
         return;
      }
      if (!logon)
      {
         sprintf
         (
            buf,
            "Unable to find %s last logon.",
            capitalize(arg)
         );
         send_to_char(buf, ch);
         fpReserve = fopen(NULL_FILE, "r");
         return;
      }
      if (storage)
      {
         send_to_char("(Stored) ", ch);
      }
      sprintf
      (
         buf,
         "%s last logged in (%d days ago) at\n\r%s\r",
         /* \r because ctime ends only with \n */
         capitalize(arg),
         (int)(current_time - logon) / (3600 * 24),
         (char*)ctime((time_t*)&logon)
      );
      send_to_char(buf, ch);
      if ((time_t)logon >= TIME_0)
      {
         time_val.tv_sec = (int)logon;
         time_val.tv_usec = 0;
         timeval_to_mudtime(&time_val, &time_m);
         curr_day = (time_m.day + time_m.week * 7 + 1);

         if (curr_day >= 10 && curr_day <= 20)
         {
            suf = "th";
         }
         else
         {
            switch (curr_day % 10)
            {
               default:
               {
                  suf = "th";
                  break;
               }
               case (1):
               {
                  suf = "st";
                  break;
               }
               case (2):
               {
                  suf = "nd";
                  break;
               }
               case (3):
               {
                  suf = "rd";
                  break;
               }
            }
         }
         sprintf
         (
            buf,
            "Mud time at time of logon was: %s%d:%s%d:%s%d, Day of %s, %d%s the"
            " Month of %s, in the year %d.\n\r",
            time_m.hour < 10 ? "0" : "",
            time_m.hour,
            time_m.minute < 10 ? "0" : "",
            time_m.minute,
            time_m.second < 10 ? "0" : "",
            time_m.second,
            day_name[time_m.day],
            curr_day,
            suf,
            month_name[time_m.month],
            time_m.year
         );
         send_to_char(buf, ch);
      }
      fpReserve = fopen(NULL_FILE, "r");
      return;
   }
   fpReserve = fopen(NULL_FILE, "r");
   send_to_char(capitalize(arg), ch);
   send_to_char(" not found.\n\r", ch);
}

void do_nofight(CHAR_DATA* ch, char* argument)
{
   DESCRIPTOR_DATA* d;
   CHAR_DATA* vch = NULL;
   char* name = NULL;
   char arg1 [MAX_INPUT_LENGTH];

   one_argument(argument, arg1);
   if (IS_NPC(ch))
   {
      send_to_char("Not with an NPC.\n\r", ch);
      return;
   }
   /*
   d->next is safe to use instead of a temp variable
   d_next, as nothing here can possibly destroy
   a descriptor.
   */
   for (d = descriptor_list; d != NULL; d = d->next)
   {
      if (d->connected != CON_PLAYING)
      {
         continue;
      }
      vch = d->original ? d->original : d->character;
      name = vch->name;
      if (str_cmp(name, arg1))
      {
         vch = NULL;
         continue;
      }
      if (!can_see(ch, vch))
      {
         send_to_char("They are not here.\n\r", ch);
         return;
      }
      break;
   }
   if (vch == NULL)
   {
      send_to_char("They are not here.\n\r", ch);
      return;
   }
   if
   (
      ch != vch &&
      get_trust(vch) == get_trust(ch) &&
      get_trust(ch) < MAX_LEVEL
   )
   {
      send_to_char("You should let someone else handle that.\n\r", ch);
      return;
   }
   if (get_trust(vch) > get_trust(ch))
   {
      send_to_char("They are too high to mess with.\n\r", ch);
      return;
   }
   if (!IS_SET(vch->act2, PLR_NO_FIGHT))
   {
      SET_BIT(vch->act2, PLR_NO_FIGHT);
      act
      (
         "You take away $N's ability to fight.",
         ch,
         NULL,
         vch,
         TO_CHAR
      );
   }
   else
   {
      REMOVE_BIT(vch->act2, PLR_NO_FIGHT);
      act
      (
         "You restore $N's ability to fight.",
         ch,
         NULL,
         vch,
         TO_CHAR
      );
   }
}

void do_revoke(CHAR_DATA* ch, char* argument)
{
   DESCRIPTOR_DATA* d;
   CHAR_DATA* real_ch = NULL;
   CHAR_DATA* vch = NULL;
   char* name = NULL;
   char arg1 [MAX_INPUT_LENGTH];

   one_argument(argument, arg1);
   if (IS_NPC(ch))
   {
      if
      (
         ch->desc &&
         ch->desc->original &&
         !IS_NPC(ch->desc->original)
      )
      {
         real_ch = ch->desc->original;
      }
      else
      {
         send_to_char("Not with an NPC.\n\r", ch);
         return;
      }
   }
   else
   {
      real_ch = ch;
   }
   /*
   d->next is safe to use instead of a temp variable
   d_next, as nothing here can possibly destroy
   a descriptor.
   */
   for (d = descriptor_list; d != NULL; d = d->next)
   {
      if (d->connected != CON_PLAYING)
      {
         continue;
      }
      vch = d->original ? d->original : d->character;
      name = vch->name;
      if (str_cmp(name, arg1))
      {
         vch = NULL;
         continue;
      }
      if (!can_see(ch, vch))
      {
         send_to_char("They are not here.\n\r", ch);
         return;
      }
      break;
   }
   if (vch == NULL)
   {
      send_to_char("They are not here.\n\r", ch);
      return;
   }
   if (!IS_IMMORTAL(vch))
   {
      act
      (
         "$N has no immortal powers.",
         ch,
         NULL,
         vch,
         TO_CHAR
      );
      return;
   }
   if (get_trust(vch) > get_trust(ch))
   {
      send_to_char("They are too high to mess with.\n\r", ch);
      return;
   }
   if (real_ch == vch)
   {
      send_to_char("You cannot revoke your own powers.\n\r", ch);
      return;
   }
   if (!IS_SET(vch->comm2, COMM_WIZ_REVOKE))
   {
      SET_BIT(vch->comm2, COMM_WIZ_REVOKE);
      act
      (
         "You take away $N's immortal powers.",
         ch,
         NULL,
         vch,
         TO_CHAR
      );
      act
      (
         "$N has revoked your immortal powers!",
         vch,
         NULL,
         ch,
         TO_CHAR
      );
   }
   else
   {
      REMOVE_BIT(vch->comm2, COMM_WIZ_REVOKE);
      act
      (
         "You restore $N's immortal powers.",
         ch,
         NULL,
         vch,
         TO_CHAR
      );
      act
      (
         "$N has restored your immortal powers.",
         vch,
         NULL,
         ch,
         TO_CHAR
      );
   }
}

void do_maxset(CHAR_DATA* ch, char* argument)
{
   char buf  [MAX_INPUT_LENGTH];
   char arg1 [MAX_INPUT_LENGTH];
   char arg2 [MAX_INPUT_LENGTH];
   char arg3 [MAX_INPUT_LENGTH];

   CHAR_DATA* victim;
   sh_int value;
   sh_int stat = -1;

   if (get_trust(ch) < DEITY)
   {
      send_to_char("You are not powerful enough for that.\n\r", ch);
      return;
   }
   smash_tilde(argument);
   argument = one_argument(argument, buf);
   argument = one_argument(argument, arg2);
   strcpy(arg3, argument);

   if
   (
      buf[0] == '\0' ||
      arg2[0] == '\0' ||
      arg3[0] == '\0'
   )
   {
      send_to_char
      (
         "Syntax:\n\r"
         "  set maxstat <name> <field> <value>\n\r"
         "  Field being one of:\n\r"
         "    str int wis dex con\n\r",
         ch
      );
      return;
   }

   if (buf[0] != '+')
   {
      /* Only PCs */
      sprintf(arg1, "+%s", buf);
   }
   else
   {
      strcpy(arg1, buf);
   }
   if ((victim = get_char_world(ch, arg1)) == NULL)
   {
      send_to_char("They are not here.\n\r", ch);
      return;
   }
   if (IS_NPC(victim))  /* sanity check, use + syntax above already */
   {
      send_to_char("Not with an NPC.\n\r", ch);
      return;
   }

   /*
   * Snarf the value (which need not be numeric).
   */
   if (!is_number(arg3))
   {
      send_to_char("Set max stat to what number?\n\r", ch);
      return;
   }
   value = atoi(arg3);

   if (!str_cmp(arg2, "str"))
   {
      stat = STAT_STR;
   }
   else if (!str_cmp(arg2, "int"))
   {
      stat = STAT_INT;
   }
   else if (!str_cmp(arg2, "wis"))
   {
      stat = STAT_WIS;
   }
   else if (!str_cmp(arg2, "dex"))
   {
      stat = STAT_DEX;
   }
   else if (!str_cmp(arg2, "con"))
   {
      stat = STAT_CON;
   }
   if (stat != -1)
   {
      if (value < 3 || value > 25)
      {
         send_to_char("Range is 3 to 25.\n\r", ch);
         return;
      }
      victim->pcdata->stat_bonus[stat] += value - get_max_train(victim, stat);
      send_to_char("Ok.\n\r", ch);
      return;
   }
   /*
   * Generate usage message.
   */
   do_maxset( ch, "" );
   return;
}

void do_dossier(CHAR_DATA* ch, char* argument)
{
   char char_buf[MAX_INPUT_LENGTH];
   char arg1[MAX_INPUT_LENGTH];
   CHAR_DATA* vch;

   argument = one_argument_space(argument, arg1);
   if (arg1[0] == '\0')
   {
      send_to_char("Modify or look at whose dossier?\n\r", ch);
      return;
   }
   if
   (
      !str_cmp(arg1, "help") ||
      !str_cmp(arg1, "syntax")
   )
   {
      do_dossier_modify(ch, "syntax", ch);
      return;
   }

   /* Players only */
   if (arg1[0] != '+')
   {
      sprintf(char_buf, "+%s", arg1);
   }
   else
   {
      strcpy(char_buf, arg1);
   }
   if
   (
      (
         vch = get_char_world(ch, char_buf)
      ) == NULL
   )
   {
      send_to_char("They are not here.\n\r", ch);
      return;
   }
   if (IS_NPC(vch))  /* sanity check */
   {
      send_to_char("Mobs cannot have dossiers.\n\r", ch);
      return;
   }
   if (get_trust(vch) > get_trust(ch))
   {
      act
      (
         "$N might get angry if you wrote on $M.",
         ch,
         NULL,
         vch,
         TO_CHAR
      );
      return;
   }
   if
   (
      !str_cmp(argument, "help") ||
      !str_cmp(argument, "syntax")
   )
   {
      argument = "syntax";
   }
   do_dossier_modify(ch, argument, vch);
}

void do_resurrec(CHAR_DATA* ch, char* argument)
{
   send_to_char("If you want to RESURRECT, spell it out!\n\r", ch);
}

void do_resurrect(CHAR_DATA* ch, char* argument)
{
   CHAR_DATA* victim;
   char buf[MAX_STRING_LENGTH];
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   AFFECT_DATA* paf;
   sh_int new_age;
   sh_int timer;

   argument = one_argument(argument, arg1);
   one_argument(argument, arg2);
   if (arg1[0] == '\0')
   {
      send_to_char("Resurrect whom?\n\r", ch);
      return;
   }
   /* +victim gives pc only */
   sprintf
   (
      buf,
      "%s%s",
      arg1[0] == '+' ? "" : "+",
      arg1
   );
   victim = get_char_world(ch, buf);
   if (victim == NULL)
   {
      send_to_char("They are not here.\n\r", ch);
      return;
   }
   if (IS_NPC(victim))
   {
      /* Sanity check */
      send_to_char("You cannot resurrect NPCs.\n\r", ch);
      return;
   }
   if (victim->pcdata->death_status != HAS_DIED)
   {
      act
      (
         "$N is not dead!",
         ch,
         NULL,
         victim,
         TO_CHAR
      );
      return;
   }
   if (arg2[0] == '\0')
   {
      timer = 50;
   }
   else
   {
      if (!is_number(arg2))
      {
         send_to_char
         (
            "Syntax:\n\r"
            "  resurrect <victim>\n\r"
            "  resurrect <victim> #\n\r"
            "If # is left out, will default to 50 ticks.\n\r",
            ch
         );
         return;
      }
      timer = atoi(arg2);
   }
   /* RESURRECT */
   victim->pcdata->death_status = 0;
   victim->pcdata->death_timer = 0;
   /*
   Automatically grant 5 years of life
   (in case resurrection timer is not long enough to
   set new age)
   */
   new_age = UMAX(get_age(victim), get_death_age(victim)) + 5;
   victim->pcdata->age_mod += get_death_age(victim) - new_age;
   if (victim->pcdata->age_mod == 0)
   {
      victim->pcdata->age_mod--;
   }
   /* EMOTES */
   act
   (
      "$n slowly solidifies and becomes whole once more!",
      victim,
      NULL,
      NULL,
      TO_ROOM
   );
   send_to_char
   (
      "You slowly solidify and become whole once more!\n\r",
      victim
   );
   act
   (
      "$N's life has been extended for five years.\n\r"
      "Remember to grant $M more.",
      ch,
      NULL,
      victim,
      TO_CHAR
   );
   paf = new_affect();
   paf->type = gsn_resurrection;
   paf->duration = timer;
   affect_to_char(victim, paf);
   free_affect(paf);
   irv_update(victim, FALSE, TRUE);
}

void reset_wizireport(WORSHIP_TYPE* worship)
{
   if (worship->history.total_start == 0)
   {
      /* Already reset */
      return;
   }
   worship->history.last_change = current_time;
   worship->history.start = current_time;
   worship->history.played = 0;
   worship->history.vis = 0;
}

void reset_wizireport_total(WORSHIP_TYPE* worship)
{
   if (worship->history.total_start == 0)
   {
      /* Already reset */
      return;
   }
   worship->history.total_start = current_time;
   worship->history.total_played = 0;
   worship->history.total_vis = 0;
   reset_wizireport(worship);
}

char* get_wizi_report(CHAR_DATA* ch, WORSHIP_TYPE* worship)
{
   static char buf[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];

   long percent;
   long total_percent;
   long percent_dec = -1;
   long total_percent_dec = -1;
   char digit[2];
   char total_digit[2];

   if (worship->history.total_start == 0)
   {
      sprintf
      (
         buf,
         "%s has no report.\n\r",
         capitalize(worship->name)
      );
      return buf;
   }
   if (worship->history.played == 0)
   {
      percent = 0;
      percent_dec = 0;
      digit[0] = '0';
      digit[1] = '\0';
   }
   else
   {
      percent = worship->history.vis * 100 / worship->history.played;
   }
   if (worship->history.total_played == 0)
   {
      total_percent = 0;
      total_percent_dec = 0;
      total_digit[0] = '0';
      total_digit[1] = '\0';
   }
   else
   {
      total_percent =
      (
         worship->history.total_vis * 100 / worship->history.total_played
      );
   }
   if (percent_dec == -1)
   {
      percent_dec =
      (
         (
            (worship->history.vis * 100) %
            worship->history.played
         ) *
         100 /
         worship->history.played
      );
      if (percent_dec < 10)
      {
         digit[0] = '0';
         digit[1] = '\0';
      }
      else
      {
         digit[0] = '\0';
      }
   }
   if (total_percent_dec == -1)
   {
      total_percent_dec =
      (
         (
            (worship->history.total_vis * 100) %
            worship->history.total_played
         ) *
         100 /
         worship->history.total_played
      );
      if (total_percent_dec < 10)
      {
         total_digit[0] = '0';
         total_digit[1] = '\0';
      }
      else
      {
         total_digit[0] = '\0';
      }
   }
   sprintf
   (
      buf,
      "%-10s : Level %d  Last Reset  (%4ld Days ago) %s\r",
      worship->name,
      worship->wiz_info.level,
      (current_time - worship->history.start) / (3600 * 24),
      ctime(&worship->history.start)
   );
   sprintf
   (
      buf2,
      "           : Trust %d  Last Login  (%4ld Days ago) %s\r"
      "  Played   : %5ld Hours, %2ld Minutes, %2ld Seconds\n\r"
      "  Visible  : %5ld Hours, %2ld Minutes, %2ld Seconds\n\r"
      "             %3ld.%s%ld%% Visible\n\r",
      worship->wiz_info.trust,
      (current_time - worship->wiz_info.login) / (3600 * 24),
      ctime(&worship->wiz_info.login),
      (worship->history.played / 3600),
      ((worship->history.played / 60) % 60),
      (worship->history.played % 60),
      (worship->history.vis / 3600),
      ((worship->history.vis / 60) % 60),
      (worship->history.vis % 60),
      percent,
      digit,
      percent_dec
   );
   strcat(buf, buf2);
   sprintf
   (
      buf2,
      "  Total    : Last Reset  (%4ld Days ago) %s\r"
      "  Played   : %5ld Hours, %2ld Minutes, %2ld Seconds\n\r"
      "  Visible  : %5ld Hours, %2ld Minutes, %2ld Seconds\n\r"
      "             %3ld.%s%ld%% Visible\n\r",
      (current_time - worship->history.total_start) / (3600 * 24),
      ctime(&worship->history.total_start),
      (worship->history.total_played / 3600),
      ((worship->history.total_played / 60) % 60),
      (worship->history.total_played % 60),
      (worship->history.total_vis / 3600),
      ((worship->history.total_vis / 60) % 60),
      (worship->history.total_vis % 60),
      total_percent,
      total_digit,
      total_percent_dec
   );
   strcat(buf, buf2);
   return buf;
}

#define COMPARE_VIS   1
#define COMPARE_LEVEL 2
#define COMPARE_TRUST 3
#define COMPARE_TIME  4

int compare_wizi (const void* input1, const void* input2)
{
   WORSHIP_TYPE* worship_1 = *(WORSHIP_TYPE**)input1;
   WORSHIP_TYPE* worship_2 = *(WORSHIP_TYPE**)input2;

   if (compare_wizi_descend)
   {
      WORSHIP_TYPE* worship_t = worship_2;

      worship_2 = worship_1;
      worship_1 = worship_t;
   }
   if
   (
      worship_1->name == NULL &&
      worship_2->name == NULL
   )
   {
      return 0;
   }
   if (worship_1->name == NULL)
   {
      return 1;
   }
   if (worship_2->name == NULL)
   {
      return -1;
   }
   switch (compare_wizi_type)
   {
      default:
      {
         return 0;
      }
      case (COMPARE_LEVEL):
      {
         if (worship_1->wiz_info.level < worship_2->wiz_info.level)
         {
            return -1;
         }
         else if (worship_1->wiz_info.level == worship_2->wiz_info.level)
         {
            if (worship_1->wiz_info.trust < worship_2->wiz_info.trust)
            {
               return -1;
            }
            else if (worship_1->wiz_info.trust == worship_2->wiz_info.trust)
            {
               /* Names are always alphabetical */
               if (compare_wizi_descend)
               {
                  return strcmp(worship_2->name, worship_1->name);
               }
               return strcmp(worship_1->name, worship_2->name);
            }
            return 1;
         }
         return 1;
      }
      case (COMPARE_TRUST):
      {
         if (worship_1->wiz_info.trust < worship_2->wiz_info.trust)
         {
            return -1;
         }
         else if (worship_1->wiz_info.trust == worship_2->wiz_info.trust)
         {
            if (worship_1->wiz_info.level < worship_2->wiz_info.level)
            {
               return -1;
            }
            else if (worship_1->wiz_info.level == worship_2->wiz_info.level)
            {
               /* Names are always alphabetical */
               if (compare_wizi_descend)
               {
                  return strcmp(worship_2->name, worship_1->name);
               }
               return strcmp(worship_1->name, worship_2->name);
            }
            return 1;
         }
         return 1;
      }
      case (COMPARE_TIME):
      {
         if (worship_1->history.played < worship_2->history.played)
         {
            return -1;
         }
         else if (worship_1->history.played == worship_2->history.played)
         {
            /* Names are always alphabetical */
            if (compare_wizi_descend)
            {
               return strcmp(worship_2->name, worship_1->name);
            }
            return strcmp(worship_1->name, worship_2->name);
         }
         return 1;
      }
      case (COMPARE_VIS):
      {
         /* Cross multiply to compare fractions */
         long first;
         long second;

         if (worship_1->history.played == 0)
         {
            first = 0;
         }
         else
         {
            first = worship_1->history.vis * 100 / worship_1->history.played;
         }
         if (worship_2->history.played == 0)
         {
            second = 0;
         }
         else
         {
            second = worship_2->history.vis * 100 / worship_2->history.played;
         }
         if (first < second)
         {
            return -1;
         }
         else if (first > second)
         {
            return 1;
         }
         if (worship_1->history.played == 0)
         {
            first = 0;
         }
         else
         {
            first =
            (
               (worship_1->history.vis * 100) %
               worship_1->history.played
            ) * 100 / worship_1->history.played;
         }
         if (worship_2->history.played == 0)
         {
            second = 0;
         }
         else
         {
            second =
            (
               (worship_2->history.vis * 100) %
               worship_2->history.played
            ) * 100 / worship_2->history.played;
         }
         if (first < second)
         {
            return -1;
         }
         else if (first > second)
         {
            return 1;
         }
         /* Names are always alphabetical */
         if (compare_wizi_descend)
         {
            return strcmp(worship_2->name, worship_1->name);
         }
         return strcmp(worship_1->name, worship_2->name);
      }
   }
}

void do_wizireport(CHAR_DATA* ch, char* argument)
{
   char buf[MAX_STRING_LENGTH];
   char arg[MAX_INPUT_LENGTH];
   bool all = FALSE;
   bool reset = FALSE;
   bool total_reset = FALSE;
   WORSHIP_TYPE* worship;
   BUFFER* buffer;
   bool found = FALSE;
   bool skip = FALSE;
   sh_int god;
   int hours = 1;
   int slot;
   int num_gods = 0;
   bool check_lev = FALSE;
   int low_lev = 100;
   int high_lev = 0;
   time_t time_trip = 0;
   time_t time_life = 0;
   time_t vis_trip = 0;
   time_t vis_life = 0;
   long percent;
   long total_percent;
   long percent_dec = -1;
   long total_percent_dec = -1;
   char digit[3];
   char total_digit[3];
   char hour_digit[3];
   char total_hour_digit[3];
   bool sort = FALSE;
   WORSHIP_TYPE** arr = NULL;
   int arr_size = 0;
   int counter = 0;
   bool aggregate_only = FALSE;

   for (; ;)
   {
      argument = one_argument(argument, arg);
      if (!str_cmp(arg, "all"))
      {
         all = TRUE;
         continue;
      }
      if (!str_cmp(arg, "short"))
      {
         all = TRUE;
         aggregate_only = TRUE;
         continue;
      }
      if (!str_cmp(arg, "sort"))
      {
         char* temp;

         all = TRUE;
         sort = TRUE;
         compare_wizi_type = COMPARE_TIME;
         compare_wizi_descend = TRUE;
         while (argument[0] != '\0')
         {
            temp = argument;
            argument = one_argument(argument, arg);

            if (!str_prefix(arg, "descending"))
            {
               compare_wizi_descend = TRUE;
               continue;
            }
            if (!str_prefix(arg, "ascending"))
            {
               compare_wizi_descend = FALSE;
               continue;
            }
            if (!str_prefix(arg, "level"))
            {
               compare_wizi_type = COMPARE_LEVEL;
               continue;
            }
            if (!str_prefix(arg, "trust"))
            {
               compare_wizi_type = COMPARE_LEVEL;
               continue;
            }
            if (!str_prefix(arg, "time"))
            {
               compare_wizi_type = COMPARE_TIME;
               continue;
            }
            if (!str_prefix(arg, "visible"))
            {
               compare_wizi_type = COMPARE_VIS;
               continue;
            }
            argument = temp;
            break;
         }
         continue;
      }
      if (!str_cmp(arg, "skip"))
      {
         all = TRUE;
         skip = TRUE;
         if
         (
            argument[0] != '\0' &&
            is_number(argument)
         )
         {
            hours = atoi(argument);
            if (hours < 1)
            {
               send_to_char
               (
                  "You must pick no less than 1 hour.\n\r",
                  ch
               );
               do_wizireport(ch, "");
               return;
            }
         }
         else if (argument[0] != '\0')
         {
            send_to_char
            (
               "Skip can only be used in conjunction with a number.\n\r",
               ch
            );
            do_wizireport(ch, "");
            return;
         }
         break;
      }
      if
      (
         !str_cmp(arg, "level") ||
         !str_cmp(arg, "levels")
      )
      {
         all = TRUE;
         check_lev = TRUE;
         argument = one_argument(argument, arg);
         if
         (
            arg[0] != '\0' &&
            is_number(arg)
         )
         {
            low_lev = atoi(arg);
            if
            (
               low_lev < LEVEL_IMMORTAL ||
               low_lev > MAX_LEVEL
            )
            {
               send_to_char
               (
                  "Level must be between immortal level and max level.\n\r",
                  ch
               );
               do_wizireport(ch, "");
               return;
            }
         }
         else
         {
            send_to_char
            (
               "You must provide a level, or level range.\n\r",
               ch
            );
            do_wizireport(ch, "");
            return;
         }
         if (argument[0] != '\0')
         {
            char* temp = argument;

            argument = one_argument(argument, arg);
            if
            (
               arg[0] != '\0' &&
               is_number(arg)
            )
            {
               high_lev = atoi(arg);
               if
               (
                  high_lev > MAX_LEVEL ||
                  high_lev < low_lev
               )
               {
                  send_to_char
                  (
                     "Level must be between max level and low level.\n\r",
                     ch
                  );
                  do_wizireport(ch, "");
                  return;
               }
            }
            else
            {
               high_lev = low_lev;
               argument = temp;
            }
         }
         else
         {
            high_lev = low_lev;
         }
         continue;
      }
      if (!str_cmp(arg, "reset"))
      {
         if (total_reset)
         {
            send_to_char
            (
               "Reset total timers, or only current?\n\r",
               ch
            );
            return;
         }
         reset = TRUE;
         continue;
      }
      if (!str_cmp(arg, "totalreset"))
      {
         if (reset)
         {
            send_to_char
            (
               "Reset total timers, or only current?\n\r",
               ch
            );
            return;
         }
         total_reset = TRUE;
         continue;
      }
      break;
   }
   if
   (
      skip &&
      (
         reset ||
         total_reset
      )
   )
   {
      send_to_char("Skip cannot be used in conjunction with reset.\n\r", ch);
      do_wizireport(ch, "");
      return;
   }
   if
   (
      arg[0] == '\0' &&
      !all
   )
   {
      if (reset)
      {
         send_to_char("Reset whose timer?\n\r", ch);
         return;
      }
      if (total_reset)
      {
         send_to_char("Reset whose total timer?\n\r", ch);
         return;
      }
      send_to_char
      (
         "Syntax:\n\r"
         " wizireport [reset|totalreset] (all|name)\n\r"
         " wizireport skip [number]\n\r"
         " wizireport (level|levels) (low) [high]\n\r"
         " () = required, [] = optional\n\r"
         " reset: clear current timer, leave the total timer alone.\n\r"
         " totalresett: clear both current and total timers.\n\r"
         " all: Apply reset/totalreset, or view to all records.\n\r"
         " skip: Shows everyone with at least, [number] hours since\n\r"
         "       last reset.  [number] defaults to 1.\n\r",
         ch
      );
      send_to_char
      (
         " level: Shows only immortals of range low to high level.\n\r"
         "        if high is omitted, only immortals of level low.\n\r"
         "  sort: (combine this with anything but 'name')\n\r"
         "  sort [descend|ascend] [time|level|trust|vis]\n\r"
         "  sort with no parameters defaults to descending time\n\r"
         "  short: using short will suppress all output except for\n\r"
         "         the aggregate report\n\r",
         ch
      );
      return;
   }
   if
   (
      all &&
      arg[0] != '\0'
   )
   {
      send_to_char("Either use a name, or all, not both.\n\r", ch);
      return;
   }
   if (!all)
   {
      slot = -500;
      worship = find_wizi_slot(arg, &slot);
      if
      (
         worship == NULL ||
         worship->name == NULL
      )
      {
         send_to_char("God not found.\n\r", ch);
         return;
      }
      if (reset)
      {
         send_to_char("Resetting.\n\r", ch);
         reset_wizireport(worship);
         return;
      }
      if (total_reset)
      {
         send_to_char("Resetting totals.\n\r", ch);
         reset_wizireport_total(worship);
         return;
      }
      send_to_char
      (
         get_wizi_report(ch, worship),
         ch
      );
      if (reset)
      {
         send_to_char("Resetting.\n\r", ch);
         reset_wizireport(worship);
      }
      if (total_reset)
      {
         send_to_char("Resetting totals.\n\r", ch);
         reset_wizireport_total(worship);
      }
      return;
   }
   buffer = new_buf();
   for (god = 0; worship_table[god].name != NULL; god++)
   {
      arr_size++;
   }
   for
   (
      god = 0;
      (
         god < MAX_TEMP_GODS &&
         temp_worship_table[god].name != NULL
      );
      god++
   )
   {
      arr_size++;
   }
   arr = (WORSHIP_TYPE**) malloc(arr_size * sizeof(WORSHIP_TYPE*));
   counter = 0;
   for (god = 0; worship_table[god].name != NULL; god++)
   {
      arr[counter] = &worship_table[god];
      counter++;
   }
   for
   (
      god = 0;
      (
         god < MAX_TEMP_GODS &&
         temp_worship_table[god].name != NULL
      );
      god++
   )
   {
      arr[counter] = &temp_worship_table[god];
      counter++;
   }
   if (sort)
   {
      qsort(arr, arr_size, sizeof(WORSHIP_TYPE*), compare_wizi);
   }
   for (counter = 0; counter < arr_size; counter++)
   {
      worship = arr[counter];
      if
      (
         worship &&
         worship->name &&
         worship->history.total_start &&
         (
            !skip ||
            worship->history.played >= 3600 * hours
         ) &&
         (
            !check_lev ||
            (
               worship->wiz_info.level >= low_lev &&
               worship->wiz_info.level <= high_lev
            )
         )
      )
      {
         num_gods++;
         if (worship->wiz_info.level < low_lev)
         {
            low_lev = worship->wiz_info.level;
         }
         if (worship->wiz_info.level > high_lev)
         {
            high_lev = worship->wiz_info.level;
         }
         time_trip += worship->history.played;
         time_life += worship->history.total_played;
         vis_trip  += worship->history.vis;
         vis_life  += worship->history.total_vis;
         found = TRUE;
         if (reset)
         {
            if (!aggregate_only)
            {
               sprintf(buf, "Resetting report for %s.\n\r", worship->name);
               add_buf(buffer, buf);
            }
            reset_wizireport(worship);
         }
         else if (total_reset)
         {
            if (!aggregate_only)
            {
               sprintf(buf, "Resetting totals for %s.\n\r", worship->name);
               add_buf(buffer, buf);
            }
            reset_wizireport_total(worship);
         }
         else if (!aggregate_only)
         {
            add_buf
            (
               buffer,
               get_wizi_report(ch, worship)
            );
         }
      }
   }
   if (!found)
   {
      if (reset)
      {
         send_to_char("No reports reset.\n\r", ch);
      }
      else if (total_reset)
      {
         send_to_char("No reports totals reset.\n\r", ch);
      }
      else
      {
         send_to_char("No reports found.\n\r", ch);
      }
   }
   else
   {
      if (time_trip == 0)
      {
         percent = 0;
         percent_dec = 0;
         digit[0] = '0';
         digit[1] = '0';
         digit[2] = '\0';
      }
      else
      {
         percent = vis_trip * 100 / time_trip;
      }
      if (time_life == 0)
      {
         total_percent = 0;
         total_percent_dec = 0;
         total_digit[0] = '0';
         total_digit[1] = '0';
         total_digit[2] = '\0';
      }
      else
      {
         total_percent =
         (
            vis_life * 100 / time_life
         );
      }
      if (percent_dec == -1)
      {
         percent_dec =
         (
            (
               (vis_trip * 100) %
               time_trip
            ) *
            100 /
            time_trip
         );
         digit[0] = '0' + (percent_dec / 10);
         digit[1] = '0' + (percent_dec % 10);
         digit[2] = '\0';
      }
      if (total_percent_dec == -1)
      {
         total_percent_dec =
         (
            (
               (vis_life * 100) %
               time_life
            ) *
            100 /
            time_life
         );
         total_digit[0] = '0' + (percent_dec / 10);
         total_digit[1] = '0' + (percent_dec % 10);
         total_digit[2] = '\0';
      }
      hour_digit[0] = '0' + (((time_trip / num_gods) % 3600) * 100 / 3600) / 10;
      hour_digit[1] = '0' + (((time_trip / num_gods) % 3600) * 100 / 3600) % 10;
      hour_digit[2] = '\0';
      total_hour_digit[0] = '0' + (((time_life / num_gods) % 3600) * 100 / 3600) / 10;
      total_hour_digit[1] = '0' + (((time_life / num_gods) % 3600) * 100 / 3600) % 10;
      total_hour_digit[2] = '\0';
      sprintf
      (
         buf,
         "\n\r"
         "Aggregate report: %d Immortals  Levels %d - %d, \n\r"
         "\n\r"
         "Trip     : Total Time: %2ld hrs, %2ld min, %2ld sec.\n\r"
         "           Avg per Imm: %ld.%s hours, Visibility: %3ld.%s%%\n\r",
         num_gods,
         low_lev,
         high_lev,
         time_trip / 3600,
         (time_trip % 3600) / 60,
         time_trip % 60,
         (time_trip / num_gods) / 3600,
         hour_digit,
         percent,
         digit
      );
      add_buf(buffer, buf);
      sprintf
      (
         buf,
         "Lifetime : Total Time: %2ld hrs, %2ld min, %2ld sec.\n\r"
         "           Avg per Imm: %ld.%s hours, Visibility: %3ld.%s%%\n\r",
         time_life / 3600,
         (time_life % 3600) / 60,
         time_life % 60,
         (time_life / num_gods) / 3600,
         total_hour_digit,
         total_percent,
         total_digit
      );
      add_buf(buffer, buf);
      page_to_char(buf_string(buffer), ch);
   }
   free_buf(buffer);
   free(arr);
}

void do_email(CHAR_DATA* ch, char* argument)
{
   WORSHIP_TYPE* worship;

   if (IS_NPC(ch))
   {
      send_to_char("Not with an NPC.\n\r", ch);
      return;
   }
   worship = find_wizi_slot(ch->name, &ch->pcdata->worship_slot);
   if
   (
      argument[0] == '\0' ||
      !str_cmp(argument, "none")
   )
   {
      free_string(ch->pcdata->email);
      free_string(worship->wiz_info.email);
      ch->pcdata->email = NULL;
      worship->wiz_info.email = NULL;
      send_to_char("Email cleared.\n\r", ch);
      return;
   }
   free_string(ch->pcdata->email);
   free_string(worship->wiz_info.email);
   ch->pcdata->email = str_dup(argument);
   worship->wiz_info.email = str_dup(argument);
   send_to_char("New email set.\n\r", ch);
   return;
}

void do_charm(CHAR_DATA* ch, char* argument)
{
   CHAR_DATA* charmy = NULL;
   CHAR_DATA* victim = NULL;
   CHAR_DATA* master = NULL;
   char arg[MAX_INPUT_LENGTH];
   char charmy_arg[MAX_INPUT_LENGTH];
   bool pet = FALSE;

   if (argument[0] == '\0')
   {
      send_to_char("Charm who?\n\r", ch);
      return;
   }
   charmy_arg[0] = '\0';
   argument = one_argument(argument, arg);
   while (arg[0] != '\0')
   {
      if (!str_cmp(arg, "pet"))
      {
         pet = TRUE;
         argument = one_argument(argument, arg);
         continue;
      }
      if (charmy == NULL)
      {
         if
         (
            (
               charmy = get_char_room(ch, arg)
            ) == NULL
         )
         {
            send_to_char("They are not here.\n\r", ch);
            return;
         }
         else
         {
            strcat(charmy_arg, arg);
            argument = one_argument(argument, arg);
            continue;
         }
      }
      if (master == NULL)
      {
         if
         (
            (
               master = get_char_room(ch, arg)
            ) == NULL
         )
         {
            send_to_char("They are not here.\n\r", ch);
            return;
         }
         else
         {
            argument = one_argument(argument, arg);
            continue;
         }
      }
      send_to_char
      (
         "Too many arguments.\n\r"
         "See help 'charm'.\n\r",
         ch
      );
      return;
   }
   if (charmy == NULL)
   {
      send_to_char("Charm who?\n\r", ch);
      return;
   }
   if (master == NULL)
   {
      master = ch;
   }
   if (master == charmy)
   {
      if (master == ch)
      {
         send_to_char("You cannot charm yourself!\n\r", ch);
      }
      else
      {
         act("$N cannot charm $Mself!\n\r", ch, NULL, master, TO_CHAR);
      }
      return;
   }
   victim = charmy->master;
   if
   (
      IS_AFFECTED(charmy, AFF_CHARM) &&
      victim != NULL &&
      get_trust(victim) > get_trust(ch)
   )
   {
      if (can_see(ch, victim))
      {
         act("You cannot mess with $N.", ch, NULL, victim, TO_CHAR);
      }
      else
      {
         act("You cannot mess with $N's master.", ch, NULL, charmy, TO_CHAR);
      }
      return;
   }
   if
   (
      !IS_NPC(charmy) &&
      get_trust(victim) > get_trust(ch)
   )
   {
      act("You cannot mess with $N.", ch, NULL, charmy, TO_CHAR);
      return;
   }
   if
   (
      victim != NULL &&
      master != victim
   )
   {
      stop_follower(charmy);
   }
   SET_BIT(charmy->affected_by, AFF_CHARM);
   if (charmy->master != master)
   {
      add_follower(charmy, master);
   }
   charmy->leader = master;
   if (pet)
   {
      master->pet = charmy;
   }
   else if (master->pet == charmy)
   {
      master->pet = NULL;
   }
   act
   (
      "$N now belongs to $t.",
      ch,
      get_descr_form(master, ch, FALSE),
      charmy,
      TO_CHAR
   );
}

void do_uncharm(CHAR_DATA* ch, char* argument)
{
   CHAR_DATA* charmy;
   CHAR_DATA* victim;

   if (argument[0] == '\0')
   {
      send_to_char("Uncharm who?\n\r", ch);
      return;
   }
   if
   (
      (
         charmy = get_char_room(ch, argument)
      ) == NULL
   )
   {
      send_to_char("They are not here.\n\r", ch);
      return;
   }
   if (!IS_AFFECTED(charmy, AFF_CHARM))
   {
      act("$N is not charmed.", ch, NULL, charmy, TO_CHAR);
      return;
   }
   if
   (
      (
         victim = charmy->master
      ) != NULL &&
      get_trust(victim) > get_trust(ch)
   )
   {
      if (can_see(ch, victim))
      {
         act("You cannot mess with $N.", ch, NULL, victim, TO_CHAR);
      }
      else
      {
         act("You cannot mess with $N's master.", ch, NULL, charmy, TO_CHAR);
      }
      return;
   }
   if (victim != NULL)
   {
      stop_follower(charmy);
   }
   else
   {
      REMOVE_BIT(ch->affected_by, AFF_CHARM);
      affect_strip(ch, gsn_charm_person);
      affect_strip(ch, gsn_seize);
      affect_strip(ch, gsn_recruit);
      affect_strip(ch, gsn_corrupt("freedom", &gsn_mob_timer));
      affect_strip(ch, gsn_enlist);
      affect_strip(ch, gsn_silver_tongue);
   }
}

void do_nosee_perm(CHAR_DATA* ch, char* argument)
{
   CHAR_DATA* victim;
   CHAR_DATA* god;
   char buf[MAX_STRING_LENGTH];
   char arg[MAX_INPUT_LENGTH + 1];

   if (argument[0] == '\0')
   {
      send_to_char("Make certain whom can never see you again?\n\r", ch);
      return;
   }
   argument = one_argument(argument, arg + 1);
   /* PC's only */
   if (arg[1] != '+')
   {
      arg[0] = '+';
      victim = get_char_world(ch, arg);
   }
   else
   {
      victim = get_char_world(ch, arg + 1);
   }
   if (victim == NULL)
   {
      send_to_char("They are not here.\n\r", ch);
      return;
   }
   if (IS_NPC(victim))
   {
      send_to_char("You can only do that to mortals.\n\r", ch);
      return;
   }
   argument = one_argument(argument, arg + 1);
   if (arg[1] == '\0')
   {
      god = ch;
   }
   else if (arg[1] != '+')
   {
      arg[0] = '+';
      god = get_char_world(ch, arg);
   }
   else
   {
      god = get_char_world(ch, arg + 1);
   }
   if (god == NULL)
   {
      send_to_char("That god is not here.\n\r", ch);
      return;
   }
   if (IS_NPC(god))
   {
      send_to_char("Only gods may become unseen to mortals.\n\r", ch);
      return;
   }
   if (get_trust(victim) >= get_trust(ch))
   {
      act("You cannot mess with $N.", ch, NULL, victim, TO_CHAR);
      return;
   }
   if (god == victim)
   {
      send_to_char("That makes no sense!\n\r", ch);
      return;
   }
   if
   (
      god != ch &&
      !(
         ch->desc &&
         ch->desc->original &&
         god == ch->desc->original
      )
   )
   {
      if (get_trust(ch) < GOD)
      {
         send_to_char
         (
            "You are not powerful enough to nosee for someone else.\n\r",
            ch
         );
         return;
      }
      if (get_trust(ch) <= get_trust(god))
      {
         act("You cannot mess with $N.", ch, NULL, god, TO_CHAR);
         return;
      }
      if (get_trust(god) >= DEMI)
      {
         act
         (
            "$N can nosee $t $Mself.",
            ch,
            victim->name,
            god,
            TO_CHAR
         );
         return;
      }
   }
   if (victim->pcdata->nosee_perm == NULL)
   {
      /* add new */
      victim->pcdata->nosee_perm = str_dup(god->name);
      if (ch != god)
      {
         act
         (
            "$N will never see $t again.",
            ch,
            god->name,
            victim,
            TO_CHAR
         );
      }
      else
      {
         act
         (
            "$N will never see you again.",
            ch,
            NULL,
            victim,
            TO_CHAR
         );
      }
      return;
   }
   strcpy(buf, victim->pcdata->nosee_perm);
   if (is_name(god->name, victim->pcdata->nosee_perm))
   {
      /* In there, toggle off */
      remove_name(buf, god->name);
      free_string(victim->pcdata->nosee_perm);
      if (buf[0] != '\0')
      {
         victim->pcdata->nosee_perm = str_dup(buf);
      }
      else
      {
         victim->pcdata->nosee_perm = NULL;
      }
      if (ch != god)
      {
         act
         (
            "$N is no longer prevented from seeing $t.",
            ch,
            god->name,
            victim,
            TO_CHAR
         );
      }
      else
      {
         act
         (
            "$N is no longer prevented from seeing you.",
            ch,
            NULL,
            victim,
            TO_CHAR
         );
      }
      return;
   }
   strcat(buf, " ");
   strcat(buf, god->name);
   free_string(victim->pcdata->nosee_perm);
   if (buf[0] != '\0')  /* sanity check */
   {
      victim->pcdata->nosee_perm = str_dup(buf);
   }
   else
   {
      victim->pcdata->nosee_perm = NULL;
   }
   if (ch != god)
   {
      act
      (
         "$N will never see $t again.",
         ch,
         god->name,
         victim,
         TO_CHAR
      );
   }
   else
   {
      act
      (
         "$N will never see you again.",
         ch,
         NULL,
         victim,
         TO_CHAR
      );
   }
}

int num_lottery_race_winners(int race)
{
   int living = lottery_race_count[race];

   if (living < 200)
   {
      return 1;
   }
   return living / 100;
}

void do_lottery(CHAR_DATA* ch, char* argument)
{
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   char arg3[MAX_INPUT_LENGTH];
   char arg4[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   char subject[MAX_STRING_LENGTH];
   char note_to[MAX_STRING_LENGTH];
   char body[MAX_STRING_LENGTH];

   int race;
   int choice;
   int count;
   time_t temp_time;
   int winners;
   LIST_DATA* winner_list;
   NODE_DATA* node;
   char* name;
   bool lottery_test;

   if (IS_NPC(ch))
   {
      send_to_char("Mobs cannot participate in the lottery.\n\r", ch);
      return;
   }
   argument = one_argument(argument, arg1);
   argument = one_argument(argument, arg2);
   argument = one_argument(argument, arg3);
   argument = one_argument(argument, arg4);
   if (arg1[0] == '\0')
   {
      send_to_char
      (
         "Syntax for help:\n\r"
         "  lottery race\n\r"
         "    Show help for the racial lottery.\n\r",
         ch
      );
      return;
   }
   if (!str_prefix(arg1, "race"))
   {
      if (arg2[0] == '\0')
      {
         send_to_char
         (
            "Syntax of use/Syntax for help:\n\r"
            "  lottery race choose\n\r"
            "    Choose a race for the lottery.\n\r"
            "  lottery race choices\n\r"
            "    List the valid choices in the racial lottery.\n\r"
            "  lottery race check\n\r"
            "    Check what race you chose for the lottery.\n\r",
            ch
         );
         if (IS_IMMORTAL(ch))
         {
            send_to_char
            (
               "  lottery race run\n\r"
               "    Run the lottery for a race.\n\r"
               "  lottery race reset\n\r"
               "    Reset all the race lotteries.\n\r"
               "  lottery race stats\n\r"
               "    Show the statistics for the racial lottery.\n\r",
               ch
            );
         }
         return;
      }
      if (!str_prefix(arg2, "choose"))
      {
         if (arg3[0] == '\0')
         {
            send_to_char
            (
               "Syntax for choosing a race for the lottery:\n\r"
               "  lottery race choose <race>\n\r"
               "Example:\n\r"
               "  lottery race choose dark-elf\n\r"
               "  lottery race choose gnome\n\r",
               ch
            );
            return;
         }
         race = race_lookup(arg3);
         if
         (
            race <= 0 ||
            race >= MAX_PC_RACE
         )
         {
            send_to_char
            (
               "That is not a valid race.\n\r"
               "See 'lottery race choices'\n\r",
               ch
            );
            do_lottery(ch, "race choose");  /* show syntax */
            return;
         }
         if
         (
            ch->pcdata->race_lottery > 0 &&
            ch->pcdata->race_lottery < MAX_PC_RACE
         )
         {
            remove_node_for(ch->name, lottery_race_players[ch->pcdata->race_lottery]);
         }
         ch->pcdata->race_lottery = race;
         if
         (
            !IS_IMMORTAL(ch) &&
            ch->level >= 30 &&
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
            /* Became level 30, added to lottery */
            add_node(ch->name, lottery_race_players[ch->pcdata->race_lottery]);
         }
         do_lottery(ch, "race check");
         return;
      }
      if (!str_prefix(arg2, "choices"))
      {
         send_to_char("The valid choices for the racial lottery are:\n\r", ch);
         for (race = 1; race < MAX_PC_RACE; race++)
         {
            sprintf
            (
               buf,
               "  %s\n\r",
               race_table[race].name
            );
            send_to_char(buf, ch);
         }
         send_to_char
         (
            "\n\r"
            "As a reminder:\n\r",
            ch
         );
         do_lottery(ch, "race check");
         return;
      }
      if (!str_prefix(arg2, "check"))
      {
         sprintf
         (
            buf,
            "You have chosen %s for the racial lottery.\n\r",
            race_table[ch->pcdata->race_lottery].name
         );
         send_to_char(buf, ch);
         if (IS_IMMORTAL(ch))
         {
            send_to_char
            (
               "Immortals cannot win the racial lottery.\n\r"
               "Only your mortal characters can.\n\r",
               ch
            );
         }
         if (ch->level < 30)
         {
            send_to_char
            (
               "You must reach level 30 to have a chance to win the racial"
               " lottery.\n\r",
               ch
            );
         }
         if
         (
            ch->house ||
            IS_SET(ch->act2, PLR_IS_ANCIENT)
         )
         {
            send_to_char
            (
               "Members of houses cannot win the racial lottery.\n\r",
               ch
            );
         }
         /* tell if 0 alive, no chance to win */
         if (!num_lottery_race_winners(ch->pcdata->race_lottery))
         {
            sprintf
            (
               buf,
               "There are no mortal %ss in Thera;"
               " no lottery will be run for them.\n\r",
               race_table[ch->pcdata->race_lottery].name
            );
            send_to_char(buf, ch);
         }
         return;
      }
      if
      (
         IS_IMMORTAL(ch) &&
         !str_prefix(arg2, "run")
      )
      {
         if (!IS_TRUSTED(ch, IMPLEMENTOR))
         {
            send_to_char("You cannot run race lotteries.\n\r", ch);
            return;
         }
         if (arg3[0] == '\0')
         {
            send_to_char
            (
               "Syntax for running a race lottery:\n\r"
               "  lottery race run <race>\n\r"
               "    Does a test run of a racial lottery for <race>.\n\r"
               "  lottery race run <race> now\n\r"
               "    Does a real run of a racial lottery for <race>.\n\r"
               "Example:\n\r"
               "  lottery race run dark-elf\n\r"
               "  lottery race run gnome\n\r"
               "  lottery race run dark-elf now\n\r"
               "  lottery race run gnome now\n\r",
               ch
            );
            return;
         }
         race = race_lookup(arg3);
         if
         (
            race <= 0 ||
            race >= MAX_PC_RACE
         )
         {
            send_to_char("That is not a race.\n\r", ch);
            do_lottery(ch, "race run");  /* show syntax */
            return;
         }
         if (lottery_race_last_run[race] > lottery_race_reset)
         {
            /* Already run since last reset */
            sprintf
            (
               buf,
               "The %s lottery has been run since the last reset.\n\r",
               race_table[race].name
            );
            send_to_char(buf, ch);
            return;
         }
         /* 1% rounded up */
         winners = num_lottery_race_winners(race);
         if (!winners)
         {
            /* No mortals alive */
            sprintf
            (
               buf,
               "There are no living mortal %ss.\n\r"
               "Lottery skipped.\n\r",
               race_table[race].name
            );
            send_to_char(buf, ch);
            return;
         }
         /* No more winners than people in the running */
         if (winners > lottery_race_players[race]->size)
         {
            winners = lottery_race_players[race]->size;
         }
         if (!winners)
         {
            /* No mortals in the running for this race */
            sprintf
            (
               buf,
               "There are no mortals in the running for the %s lottery.\n\r"
               "Lottery skipped.\n\r",
               race_table[race].name
            );
            send_to_char(buf, ch);
            return;
         }
         lottery_test = TRUE;
         if (arg4[0] != '\0')
         {
            if (str_cmp(arg4, "now"))
            {
               do_lottery(ch, "race run");
               return;
            }
            lottery_test = FALSE;
         }
         sprintf
         (
            buf,
            "Running a %slottery for %ss.\n\r"
            "\n\r",
            (
               lottery_test ?
               "FAKE " :
               ""
            ),
            race_table[race].name
         );
         send_to_char(buf, ch);
         winner_list = new_string_list();
         while (winners)
         {
            count = 0;
            choice = number_range(1, lottery_race_players[race]->size);
            name = NULL;
            for (node = lottery_race_players[race]->first; node; node = node->next)
            {
               if (++count == choice)
               {
                  name = (char*)node->data;
                  break;
               }
            }
            if (name == NULL)
            {
               free_list(winner_list);
               send_to_char("OOC Error running lottery.\n\r", ch);
               bug("OOC Error running lottery.\n\r", 0);
               return;
            }
            if (node_exists(name, winner_list))
            {
               continue;
            }
            add_node(name, winner_list);
            --winners;
         }
         for (node = winner_list->first; node; node = node->next)
         {
            sprintf
            (
               buf,
               "%s has won the lottery!\n\r",
               (char*)node->data
            );
            send_to_char(buf, ch);
         }
         while (winner_list->size)
         {
            name = (char*)winner_list->first->data;
            sprintf
            (
               subject,
               "CONGRATULATIONS %s, YOU ARE THE %s LOTTERY WINNER!!",
               name,
               capitalize(race_table[race].name)
            );
            if (lottery_test)
            {
               sprintf
               (
                  note_to,
                  "%s WINNER_PLUS_IMMORTAL_IF_WAS_REAL",
                  ch->name
               );
            }
            else
            {
               sprintf
               (
                  note_to,
                  "%s Immortal %s",
                  name,
                  ch->name
               );
            }
            body[0] = '\0';
            if (lottery_test)
            {
               sprintf
               (
                  body,
                  "This is only a test run.\n\r"
                  "If this was real, the 'to' field would be:\n\r"
                  "%s immortal %s\n\r"
                  "\n\r",
                  name,
                  ch->name
               );
            }
            sprintf
            (
               buf,
               "You have been chosen as the luck winner in the latest drawing of the\n\r"
               "Dark Mists 'Unique Combo' %s lottery!\n\r"
               "\n\r"
               "This means you will be granted a special class combination character for the\n\r"
               "%s race.\n\r"
               "To accept this prize, you must contact %s within 2 weeks of the\n\r"
               "date of this note via a PERSONAL FORUM MESSAGE.\n\r"
               "A personal forum message ensures that the response is not lost within\n\r",
               capitalize(race_table[race].name),
               capitalize(race_table[race].name),
               ch->name
            );
            strcat(body, buf);
            sprintf
            (
               buf,
               "the note system of the game, and also verifies your identity (as only you\n\r"
               "know your forum password).\n\r"
               "\n\r"
               "Failure to claim your prize within 2 weeks of the date of this note will\n\r"
               "constitute irrevocable forfeit of the opportunity given to you.\n\r"
               "You are encouraged to reply immediately, if at all possible.\n\r"
               "\n\r"
               "Congratulations again on your winning, and thank you for playing Dark Mists!!\n\r"
               "\n\r"
               "Sincerely,\n\r"
               "%s\n\r"
               "\n\r"
               "p.s. There are some rules and restrictions that apply, you can find them\n\r"
               "by 'help racial lottery'\n\r",
               ch->name
            );
            strcat(body, buf);
            make_note(ch->name, subject, note_to, body, NOTE_IDEA);
            sprintf
            (
               buf,
               "%s idea has been sent to %s!\n\r",
               (
                  lottery_test ?
                  "A fake" :
                  "An"
               ),
               name
            );
            send_to_char(buf, ch);
            remove_node_for(name, winner_list);
         }
         if (!lottery_test)
         {
            sprintf
            (
               buf,
               "[%s] ran a racial lottery for [%s]s.",
               ch->name,
               race_table[race].name
            );
            log_string(buf);
            lottery_race_last_run[race] = current_time;
            save_globals();
         }
         free_list(winner_list);
         return;
      }
      if
      (
         IS_IMMORTAL(ch) &&
         !str_prefix(arg2, "reset")
      )
      {
         if (!IS_TRUSTED(ch, IMPLEMENTOR))
         {
            send_to_char("You cannot reset race lotteries.\n\r", ch);
            return;
         }
         if (str_cmp(arg3, "now"))
         {
            send_to_char
            (
               "Syntax for resetting the race lotteries:\n\r"
               "  lottery race reset now\n\r",
               ch
            );
            return;
         }
         sprintf
         (
            buf,
            "[%s] reset the racial lotteries.",
            ch->name
         );
         log_string(buf);
         lottery_race_reset = current_time;
         send_to_char("Racial lotteries reset.\n\r", ch);
         save_globals();
         return;
      }
      if
      (
         IS_IMMORTAL(ch) &&
         !str_prefix(arg2, "stats")
      )
      {
         if (!IS_TRUSTED(ch, IMMORTAL))
         {
            send_to_char
            (
               "You cannot see the statistics for the racial lotteries.\n\r",
               ch
            );
            return;
         }
         {
            send_to_char
            (
               "Racial Lottery Statistics: (X means run since last reset)\n\r"
               "     Race        Living Mortals    Mortals in Running   Winners if Run Now  Last Run\n\r",
               ch
            );
         }
         for (race = 1; race < MAX_PC_RACE; race++)
         {
            temp_time = lottery_race_last_run[race];
            sprintf
            (
               buf,
               "  %c  %-12s%-18d%-21d%-20d%s\r",
               (
                  lottery_race_last_run[race] > lottery_race_reset ?
                  'X' :
                  ' '
               ),
               race_table[race].name,
               lottery_race_count[race],
               lottery_race_players[race]->size,
               num_lottery_race_winners(race),
               (
                  temp_time ?
                  ctime(&temp_time) :
                  "Never\n"
               )
            );
            send_to_char(buf, ch);
         }
         temp_time = lottery_race_reset;
         sprintf
         (
            buf,
            "Racial Lotteries last reset: %s\r",
            (
               temp_time ?
               ctime(&temp_time) :
               "Never\n"
            )
         );
         send_to_char(buf, ch);
         return;
      }
      do_lottery(ch, "race");
      return;
   }
   do_lottery(ch, "");
}

void do_takemagic(CHAR_DATA* ch, char* argument)
{
   CHAR_DATA* victim;
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   char vict_name[MAX_INPUT_LENGTH];
   char* real_name = NULL;

   argument = one_argument(argument, arg1);
   argument = one_argument(argument, arg2);

   if (!IS_NPC(ch))
   {
      real_name = ch->name;
   }
   else if
   (
      ch->desc &&
      ch->desc->original &&
      !IS_NPC(ch->desc->original)
   )
   {
      real_name = ch->desc->original->name;
   }
   if (real_name == NULL)
   {
      /* Sanity check */
      send_to_char("Error obtaining your real name.\n\r", ch);
      return;
   }

   if (arg1[0] == '\0')
   {
      send_to_char
      (
         "Syntax: takemagic <char>\n\r"
         "  Toggle ability to use magic.\n\r"
         "Syntax: takemagic <char> yes\n\r"
         "  Remove ability to use magic.\n\r"
         "Syntax: takemagic <char> no\n\r"
         "  Restore ability to use magic.\n\r"
         "Syntax: takemagic all\n\r"
         "  Toggle global ability to use magic.\n\r"
         "Syntax: takemagic all yes\n\r"
         "  Remove global ability to use magic.\n\r"
         "Syntax: takemagic all no\n\r"
         "  Restore global ability to use magic.\n\r",
         ch
      );
      return;
   }

   if (!str_cmp(arg1, "all"))
   {
      if (!IS_TRUSTED(ch, CREATOR))
      {
         send_to_char
         (
            "You are not powerful enough to affect magic so greatly.\n\r",
            ch
         );
         return;
      }
      if (!str_cmp(arg2, "yes"))
      {
         if (all_magic_gone)
         {
            send_to_char("All magic is already gone.\n\r", ch);
            return;
         }
      }
      else if (!str_cmp(arg2, "no"))
      {
         if (all_magic_gone)
         {
            send_to_char("All magic was not taken away.\n\r", ch);
            return;
         }
      }
      else if (arg2[0] != '\0')
      {
         do_takemagic(ch, "");
         return;
      }
      if (all_magic_gone)
      {
         send_to_char("Magic has been restored.\n\r", ch);
         sprintf(log_buf, "%s turned all magic ON.", real_name);
         /* GLOBAL MESSAGE */
      }
      else
      {
         send_to_char("Magic has disappeared from Thera.\n\r", ch);
         sprintf(log_buf, "%s turned all magic OFF.", real_name);
         /* GLOBAL MESSAGE */
      }
      all_magic_gone = !all_magic_gone;
      log_string(log_buf);
      save_globals();
      return;
   }

   /* Players only */
   if (arg1[0] == '+')
   {
      strcpy(vict_name, arg1);
   }
   else
   {
      vict_name[0] = '+';
      vict_name[1] = '\0';
      strcat(vict_name, arg1);
   }

   if ((victim = get_char_world(ch, vict_name)) == NULL)
   {
      send_to_char("They are not playing.\n\r", ch);
      return;
   }

   if (IS_NPC(victim))
   {
      /* Sanity check */
      send_to_char
      (
         "You cannot do that to a mob.\n\r",
         ch
      );
      return;
   }

   if (get_trust(ch) < get_trust(victim))
   {
      act("You cannot mess with $N.", ch, NULL, victim, TO_CHAR);
      return;
   }

   if (!str_cmp(arg2, "yes"))
   {
      if (IS_SET(victim->act2, PLR_MAGIC_TAKEN))
      {
         act("$N's magic is already gone.", ch, NULL, victim, TO_CHAR);
         send_to_char("All magic is already gone.\n\r", ch);
         return;
      }
   }
   else if (!str_cmp(arg2, "no"))
   {
      if (IS_SET(victim->act2, PLR_MAGIC_TAKEN))
      {
         act("$N already has $S magic.", ch, NULL, victim, TO_CHAR);
         return;
      }
   }
   else if (arg2[0] != '\0')
   {
      do_takemagic(ch, "");
      return;
   }

   if (IS_SET(victim->act2, PLR_MAGIC_TAKEN))
   {
      REMOVE_BIT(victim->act2, PLR_MAGIC_TAKEN);
      act("You have restored $N's magic.", ch, NULL, victim, TO_CHAR);
      if (is_clergy(victim))
      {
         send_to_char
         (
            "You suddenly feel reconnected to the patronage of your God.\n\r",
            victim
         );
      }
      else
      {
         send_to_char
         (
            "You feel the tingle of magic return to your body.\n\r",
            victim
         );
      }
      sprintf(log_buf, "%s turned %s's magic ON.", real_name, victim->name);
   }
   else
   {
      SET_BIT(victim->act2, PLR_MAGIC_TAKEN);
      act("You have taken away $N's magic.", ch, NULL, victim, TO_CHAR);
      if (is_clergy(victim))
      {
         send_to_char
         (
            "You suddenly feel shut off from the patronage of your God.\n\r",
            victim
         );
      }
      else
      {
         send_to_char
         (
            "You feel the tingle of magic ebb from your body.\n\r",
            victim
         );
      }
      sprintf(log_buf, "%s turned %s's magic OFF.", real_name, victim->name);
   }
   log_string(log_buf);
   save_char_obj(victim);
}
