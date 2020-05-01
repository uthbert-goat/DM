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

static const char rcsid[] = "$Id: update.c,v 1.124 2004/11/04 04:34:13 maelstrom Exp $";

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "worship.h"
#include "magic.h"
#include "music.h"
#include "recycle.h"

/* command procedures needed */
DECLARE_DO_FUN(do_ambush     );
DECLARE_DO_FUN(do_help        );
DECLARE_DO_FUN(do_stand         );
DECLARE_DO_FUN(do_permnameban   );
DECLARE_DO_FUN(do_quit        );
DECLARE_DO_FUN(do_myell         );
DECLARE_DO_FUN(do_yell        );
DECLARE_DO_FUN(do_follow    );
DECLARE_DO_FUN(do_recho         );
DECLARE_DO_FUN(do_portal    );
DECLARE_DO_FUN(do_up            );
DECLARE_DO_FUN(do_cast);
DECLARE_DO_FUN(do_wake);
DECLARE_DO_FUN(do_bite);
DECLARE_SPEC_FUN(spec_legion);
DECLARE_SPEC_FUN(spec_guard);
DECLARE_SPEC_FUN(spec_executioner);
void water_update args( (CHAR_DATA* ch, bool increase) );
bool    check_room_protected    args( ( ROOM_INDEX_DATA *room ) );
/*
* Local functions.
*/


void   track_update(bool update_all);
int    hit_gain    args( ( CHAR_DATA *ch ) );
int    mana_gain    args( ( CHAR_DATA *ch ) );
int    move_gain    args( ( CHAR_DATA *ch ) );
void    mobile_update    args( ( void ) );
/* void    weather_update    args( ( bool controlled ) ); */
void    char_update    args( ( void ) );
void    update_wizireport args( (void) );
void    obj_update    args( ( void ) );
void     room_aff_update args( ( void ) );
void    aggr_update    args( ( void ) );
void    riot_update    args( ( void ) );
void    age_update    args( ( void ) );
ROOM_INDEX_DATA * get_cloud_exit args( (void) );
void    cloudcty_update args( ( void ) );
void    ancient_update  args( ( void ) );
void    room_update     args( ( void ) );
void    reboot_update   args( ( void ) );
void    sword_demon_update args( (OBJ_DATA *obj) );
void    genocide_update  args( ( OBJ_DATA *obj) );
void    skull_update     args( ( OBJ_DATA *obj) );
void    sentient_sword_update args( (OBJ_DATA *obj) );
bool    is_odd_hour args( (CHAR_DATA *ch) );
sh_int wears_coiled_snakes( CHAR_DATA *ch );
void    doggie_toy_run    args( ( OBJ_DATA *toy ) );
void    doggie_toy_update args( ( OBJ_DATA *toy ) );
void    message_poll_tick args( ( void ) );
void    message_poll_pulse args( ( void ) );
void    moon_update args( (void) );
bool    stray_update args( (CHAR_DATA* stray) );
/* used for saving */

int    save_number = 0;




/*
* Advancement stuff.
*/
void advance_level( CHAR_DATA *ch, bool hide )
{
   char buf[MAX_STRING_LENGTH];
   OBJ_DATA *hermit;
   int add_hp;
   int add_mana;
   int add_move;
   int add_prac;
   int int_mod;

   ch->pcdata->last_level =
   ( ch->played + (int) (current_time - ch->logon) ) / 3600;

   sprintf( buf, "the %s",
   title_table [ch->class] [ch->level] [ch->sex == SEX_FEMALE ? 1 : 0] );
   set_title( ch, buf );

   add_hp    = con_app[get_curr_stat(ch, STAT_CON)].hitp + number_range(
   class_table[ch->class].hp_min,
   class_table[ch->class].hp_max );
   /*
   add_mana     = number_range(2, (2*get_curr_stat(ch, STAT_INT)
   + get_curr_stat(ch, STAT_WIS))/5);

   if (!class_table[ch->class].fMana)
   add_mana /= 2;
   */
   int_mod = get_curr_stat(ch, STAT_INT) - 2;

   /* NOTE: This mana gain code is bugged.  I didnt fix it because its been
   this way since day 1 and it'd be unfair, but if mana gain is ever
   redone, it should be fixed.
   The problem: UMIN() is not a function, its a define, and results
   in direct substitution, therefore the mana gain maxes do not hold.
   UMIN(number_range(), X) translates into this:
   if (number_range() < X) then do another identical number_range else X
   The second number_range will not be forced to be less than X.
   Whoever coded this originally obviously thought UMIN worked like
   a function and the number_range call would only be done once.
   -Werv
   */
   if (!str_cmp(class_table[ch->class].name, "cleric") )
   add_mana = UMIN(1 + number_range(int_mod/2, int_mod), 17);
   else if (!str_cmp(class_table[ch->class].name, "channeler") )
   add_mana = UMIN(1 + number_range(int_mod*2/3, int_mod), 24);
   else if (!str_cmp(class_table[ch->class].name, "necromancer") )
   add_mana = UMIN(1 + number_range(int_mod*2/3, int_mod), 22);
   else if (!str_cmp(class_table[ch->class].name, "elementalist") )
   add_mana = UMIN(1 + number_range(int_mod*2/3, int_mod), 20);
   else if (!str_cmp(class_table[ch->class].name, "paladin") )
   add_mana = UMIN(1 + number_range(int_mod/3, int_mod), 16);
   else if (!str_cmp(class_table[ch->class].name, "nightwalker") )
   add_mana = UMIN(1 + number_range(int_mod/3, int_mod*3/4), 15);
   else if (!str_cmp(class_table[ch->class].name, "anti-paladin") )
   add_mana = UMIN(1 + number_range(int_mod/3, int_mod*3/4), 15);
   else add_mana = UMIN(1 + number_range(int_mod/3, int_mod/2), 11);

   add_move    = number_range( 1, (get_curr_stat(ch, STAT_CON)
   + get_curr_stat(ch, STAT_DEX))/6 );
   add_prac    = wis_app[get_curr_stat(ch, STAT_WIS)].practice;

   for ( hermit = ch->carrying; hermit != NULL; hermit = hermit->next_content)
   {
      if ((hermit->pIndexData->vnum == 1890) &&
      (hermit->wear_loc != WEAR_NONE))
      add_prac++;
   }

   /*
   add_hp = add_hp * 9/10;
   add_mana = add_mana * 9/10;
   add_move = add_move * 9/10;
   */
   if (!str_cmp(class_table[ch->class].name, "warrior"))
   {
      add_hp += number_range(1, 4);
      add_hp = UMIN(add_hp, 23 );
   }
   else if (!str_cmp(class_table[ch->class].name, "paladin"))
   add_hp = UMIN(add_hp, 20 );
   else if (!str_cmp(class_table[ch->class].name, "anti-paladin"))
   add_hp = UMIN(add_hp, 20 );
   else if (!str_cmp(class_table[ch->class].name, "cleric"))
   add_hp = UMIN(add_hp, 16);
   else if (!str_cmp(class_table[ch->class].name, "ranger"))
   add_hp = UMIN(add_hp, 19);
   else if (!str_cmp(class_table[ch->class].name, "necromancer"))
   add_hp = UMIN(add_hp, 13);
   else if (!str_cmp(class_table[ch->class].name, "elementalist"))
   add_hp = UMIN(add_hp, 12);
   else if (!str_cmp(class_table[ch->class].name, "channeler"))
   add_hp = UMIN(add_hp, 13);
   else if (!str_cmp(class_table[ch->class].name, "monk"))
   add_hp = UMIN(add_hp, 20 );
   else if (!str_cmp(class_table[ch->class].name, "thief"))
   add_hp = UMIN(add_hp, 18 );
   else if (!str_cmp(class_table[ch->class].name, "nightwalker"))
   add_hp = UMIN(add_hp, 19);
   else
   add_hp = UMIN(add_hp, 15);

   add_hp    = UMAX(  2, add_hp   );
   add_mana    = UMAX(  2, add_mana );
   add_move    = UMAX(  6, add_move );

   ch->max_hit     += add_hp;
   ch->max_mana    += add_mana;
   ch->max_move    += add_move;
   ch->practice    += add_prac;
   if (ch->level == 5 || ch->level == 10 || ch->level == 15 || ch->level == 20 ||
   ch->level == 25 || ch->level == 30 || ch->level == 35 || ch->level == 40 ||
   ch->level == 45 || ch->level == 50)
   ch->train        += 1;

   if (ch->level == 5)
   {
      if (strlen(ch->description) < 60*5)
      send_to_char("With this rank, you need a description.\n\r", ch);

   }
   if (ch->level == 30)
   {
      if (strlen(ch->pcdata->psych_desc) < 60*5)
      send_to_char("With this rank, you need a psych description.\n\r", ch);
   }
   if (ch->level == 30){
      class_count[ch->class]++;
      /*              race_count[ch->race]++; */
      save_globals();
   }

   ch->pcdata->perm_hit    += add_hp;
   ch->pcdata->perm_mana    += add_mana;
   ch->pcdata->perm_move    += add_move;
   /*
   if (!hide)
   {
   sprintf(buf,
   "You gain %d hit point%s, %d mana, %d move, and %d practice%s.\n\r",
   add_hp, add_hp == 1 ? "" : "s", add_mana, add_move,
   add_prac, add_prac == 1 ? "" : "s");
   send_to_char( buf, ch );
   }
   */
   if (!hide)
   {
      if (IS_SET(ch->comm, COMM_ANSI))
      sprintf(buf,
      "You gain \x01b[1;37m%d/%d\x01b[0;37m hp, \x01b[1;37m%d/%d\x01b[0;37m mana, \x01b[1;37m%d/%d\x01b[0;37m move, and \x01b[1;37m%d/%d\x01b[0;37m practice%s.\n\r",
      add_hp, ch->max_hit, add_mana, ch->max_mana,
      add_move, ch->max_move, add_prac, ch->practice, add_prac == 1 ? "" : "s");
      else
      sprintf(buf,
      "You gain %d/%d hp, %d/%d mana, %d/%d move, and %d/%d practice%s.\n\r",
      add_hp, ch->max_hit, add_mana, ch->max_mana,
      add_move, ch->max_move, add_prac, ch->practice, add_prac == 1 ? "" : "s");
      send_to_char(buf, ch);
   }

   /*
   if (ch->level > 5 && strlen(ch->description) < 2){
   send_to_char("\n\rNow that you have surpassed the fifth rank, a description is mandatory.\n\r", ch);
   send_to_char("\n\r", ch);
   do_help(ch, "description");
   WAIT_STATE(ch, 48);
   send_to_char("\n\rTo access this help again type: Help description\n\rFor further assistance pray.\n\r", ch);
   }
   */
   if (ch->pcdata->worship && IS_IMMORTAL(ch))
   {
      free_string(ch->pcdata->worship);
      ch->pcdata->worship = NULL;
      REMOVE_BIT(ch->comm2, COMM_WORSHIP_STOPPED);
   }
   if (IS_IMMORTAL(ch))
   {
      do_wizireport_update(ch, WIZI_UPDATE);
   }
   if
   (
      ch->race != grn_book &&
      (
         IS_IMMORTAL(ch) ||
         ch->race != grn_changeling
      )
   )
   {
      switch_desc(ch, -1);
   }
   if (IS_IMMORTAL(ch))
   {
      if
      (
         ch->level == LEVEL_IMMORTAL &&
         ch->trust < LEVEL_IMMORTAL
      )
      {
         /* Just Became imm, no longer in lottery */
         remove_node_for(ch->name, lottery_race_players[ch->pcdata->race_lottery]);
         lottery_race_count[ch->race]--;
      }
   }
   else if
   (
      !IS_IMMORTAL(ch) &&
      ch->level == 30 &&
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
      remove_node_for(ch->name, lottery_race_players[ch->pcdata->race_lottery]);
      add_node(ch->name, lottery_race_players[ch->pcdata->race_lottery]);
   }
   else if
   (
      !IS_IMMORTAL(ch) &&
      ch->level == 10 &&
      ch->race > 0 &&
      ch->race < MAX_PC_RACE
   )
   {
      lottery_race_count[ch->race]++;
   }
   return;
}



void gain_exp( CHAR_DATA *ch, int gain )
{
   char buf[MAX_STRING_LENGTH];

   if ( IS_NPC(ch) )
   return;

   /*ch->exp = UMAX( exp_per_level(ch, ch->pcdata->points), ch->exp + gain );*/
   if (ch->level < LEVEL_HERO)
   ch->exp += gain;

   if (ch->exp > ch->exp_total)
   ch->exp_total = ch->exp;

   while ( ch->level < LEVEL_HERO && ch->exp >=
   exp_per_level(ch) * (ch->level) )
   {
      send_to_char( "You raise a level!!\n\r", ch );
      ch->level += 1;
      sprintf(buf, "%s gained level %d", ch->name, ch->level);
      log_string(buf);
      sprintf(buf, "$N has attained level %d!", ch->level);
      wiznet(buf, ch, NULL, WIZ_LEVELS, 0, 0);
      advance_level(ch, FALSE);
      if ((ch->level > 10) && !IS_IMMORTAL(ch) && (ch->house != HOUSE_CRUSADER))
      {
         ch->pcdata->learned[gsn_recall] = -1;
         ch->pcdata->learnlvl[gsn_recall] = 53;
      }
      save_char_obj(ch);
   }

   return;
}



/*
* Regeneration stuff.
*/
int hit_gain( CHAR_DATA *ch )
{
   int gain;
   int number;
   OBJ_DATA *brand;
   int sham_brand = FALSE;
   int haste_reduce;
   int hyper;

   brand = get_eq_char(ch, WEAR_BRAND);

   if (brand != NULL && brand->pIndexData->vnum == OBJ_VNUM_SHAM_BRAND)
   sham_brand = TRUE;

   if (ch->in_room == NULL)
   return 0;

   if (ch->in_room->vnum == ROOM_VNUM_NIGHTWALK
   || ch->in_room->vnum == 2901)
   {
      number = gsn_corrupt("shadowplane", &gsn_shadowplane);
      damage(ch, ch, 40, number, DAM_NEGATIVE, TRUE);
      return 0;
   }
   if (ch->in_room->vnum > 3820 && ch->in_room->vnum < 3826){
      return 0;
   }

   if (is_affected(ch, gsn_atrophy) || is_affected(ch, gsn_prevent_healing))
   return 0;

   if (!IS_NPC(ch))
   {
      if (ch->pcdata->condition[COND_STARVING] > 6)
      return 0;
      if (ch->pcdata->condition[COND_DEHYDRATED] > 4)
      return 0;
   }

   if ( IS_NPC(ch) )
   {
      gain =  5 + ch->level;
      /*
      if (IS_AFFECTED(ch, AFF_REGENERATION))
      gain *= 2;
      */
      switch(ch->position)
      {
         default :         gain /= 2;            break;
         case POS_SLEEPING:     gain = 3 * gain/2;        break;
         case POS_RESTING:                      break;
         case POS_FIGHTING:    gain /= 3;             break;
      }


   }
   else
   {
      gain = UMAX(3, get_curr_stat(ch, STAT_CON) -3 + ch->level/2);
      gain += class_table[ch->class].hp_max;
      number = number_percent();
      if ((number < get_skill(ch, gsn_fast_healing)) &&
      !(is_affected(ch, gsn_corrupt("smite", &gsn_smite))))
      {
         gain += number * gain / 100;
         if (ch->hit < ch->max_hit)
         check_improve(ch, gsn_fast_healing, TRUE, 8);
         if (number_percent() < (get_skill(ch, gsn_recovery) - 30))
         {
            gain = gain * 2;
            check_improve(ch, gsn_recovery, TRUE, 8);
         }
      }

      if (sham_brand)
      {
         gain = gain * 3 / 2;
      }
      switch ( ch->position )
      {
         default:           gain /= 4;            break;
         case POS_SLEEPING:                     break;
         case POS_RESTING:      gain /= 2;            break;
         case POS_FIGHTING:     gain /= 6;            break;
      }

      if ( ch->pcdata->condition[COND_HUNGER]   == 0 )
      gain /= 2;

      if ( ch->pcdata->condition[COND_THIRST] == 0 )
      gain /= 2;

   }

   /*
   gain = gain * ch->in_room->heal_rate / 100;
   */
   if (ch->on != NULL && ch->on->item_type == ITEM_FURNITURE)
   gain = (gain * 7/5);

   /*
   gain = gain * ch->on->value[3] /1 00;
   */
   if ( IS_AFFECTED(ch, AFF_POISON) )
   gain /= 4;

   if (IS_AFFECTED(ch, AFF_PLAGUE))
   gain /= 8;

   if (is_affected(ch, gsn_ancient_plague))
   gain /=8;

   if (is_affected(ch, gsn_cunning_strike))
   gain /=10;

   if (is_affected(ch, gsn_nether_shroud))
   {
      if (IS_EVIL(ch))
      gain *= 2;
      else
      gain /= 2;
   }

   /*    if (is_affected(ch, gsn_healing_trance))
   gain *=2; */
   if (ch->position == POS_SLEEPING && get_skill(ch, gsn_dark_dream) > 5)
   {
      if (number_percent() < get_skill(ch, gsn_dark_dream))
      {
         check_improve(ch, gsn_dark_dream, TRUE, 7);
         gain *=3;
         gain /=2;
      }
   }

   if (IS_AFFECTED(ch, AFF_HASTE))
   {
      if
      (
         IS_AFFECTED2(ch, AFF_HYPER) &&
         (
            hyper = get_skill(ch, gsn_hyper)
         ) > 0
      )
      {
         haste_reduce = gain - (gain / 2);
         haste_reduce = haste_reduce * (100 - hyper) / 100;
         gain -= haste_reduce;
      }
      else
      {
         gain /=2;
      }
   }

   /*    if ( IS_AFFECTED(ch, AFF_SLOW))
   gain /=2; */

   /* regen moved here so it works for players wearing apply regen items */
   if ((IS_AFFECTED(ch, AFF_REGENERATION)) &&
   !(is_affected(ch, gsn_corrupt("smite", &gsn_smite))))
   gain *= 2;

   if (is_affected(ch, gsn_camp))
   {
      if (number_percent() < ch->pcdata->learned[gsn_camp])
      {
         gain *=2;
         check_improve(ch, gsn_camp, TRUE, 7);
      } else
      check_improve(ch, gsn_camp, FALSE, 7);
   }

   if (is_affected(ch, gsn_distort_time)){
      if (is_affected(ch, gsn_distort_time_faster)){
         gain *= 2; /* two ticks instead of one */
      }
      else
      {
         if (is_odd_hour(ch))
         return 0; /*only gain on odd ticks if slower */
      }
   }

   if (ch->race == grn_arborian)
   {
      if (current_mud_time.hour >= 5 && current_mud_time.hour < 20)
      gain *=4;
      else if (sham_brand)
      gain /= 2;
      else
      gain = 0;
   }

   return UMIN(gain, ch->max_hit - ch->hit);
}



int mana_gain( CHAR_DATA *ch )
{
   int gain;
   int number;
   int hyper;
   int haste_reduce;
   OBJ_DATA *brand;
   int sham_brand = FALSE;

   brand = get_eq_char(ch, WEAR_BRAND);

   if (brand != NULL && brand->pIndexData->vnum == OBJ_VNUM_SHAM_BRAND)
   sham_brand = TRUE;

   if (ch->in_room == NULL)
   return 0;
   if (is_affected(ch, gsn_atrophy) || is_affected(ch, gsn_prevent_healing))
   return 0;

   if (!IS_NPC(ch))
   {
      if (ch->pcdata->condition[COND_STARVING] > 6)
      return 0;
      if (ch->pcdata->condition[COND_DEHYDRATED] > 4)
      return 0;
   }

   if (ch->in_room->vnum > 3820 && ch->in_room->vnum < 3826){
      if (!is_affected(ch, gsn_shadowgate))
      {
         if (ch->mana >99)
         {
            send_to_char("Your stay in the shadows begins to drain you.\n\r", ch);
            ch->mana -= 100;
            return 0;
         }
         else
         {
            send_to_char("You are forced to leave the shadows.\n\r", ch);
            ch->mana = 0;
            do_portal(ch, "");
            return 0;
         }
      }
      return 0;
   }

   if ( IS_NPC(ch) )
   {
      gain = 5 + ch->level;
      switch (ch->position)
      {
         default:        gain /= 2;        break;
         case POS_SLEEPING:    gain = 3 * gain/2;    break;
         case POS_RESTING:                break;
         case POS_FIGHTING:    gain /= 3;        break;
      }
   }
   else
   {
      gain = (get_curr_stat(ch, STAT_WIS)/2 - 9
      + get_curr_stat(ch, STAT_INT)*2 + ch->level);
      number = number_percent();
      if (number < get_skill(ch, gsn_meditation))
      {
         gain += number * gain / 100;
         if (ch->mana < ch->max_mana)
         check_improve(ch, gsn_meditation, TRUE, 4);
      }
      number = number_percent();
      if (number < get_skill(ch, gsn_trance))
      {
         gain += number * gain / 100;
         if (ch->mana < ch->max_mana)
         check_improve(ch, gsn_trance, TRUE, 4);
      }
      if (sham_brand)
      {
         gain = gain * 3 / 2;
      }

      /*
      if (!class_table[ch->class].fMana)
      gain /= 2;
      */
      switch ( ch->position )
      {
         default:        gain /= 4;            break;
         case POS_SLEEPING:                     break;
         case POS_RESTING:    gain /= 2;            break;
         case POS_FIGHTING:    gain /= 6;            break;
      }

      if ( ch->pcdata->condition[COND_HUNGER]   == 0 )
      gain /= 2;

      if ( ch->pcdata->condition[COND_THIRST] == 0 )
      gain /= 2;

   }
   /*
   gain = gain * ch->in_room->mana_rate / 100;
   */
   if (ch->on != NULL && ch->on->item_type == ITEM_FURNITURE)
   gain = gain * 7/5;
   /*
   gain = gain * ch->on->value[4] / 100;
   */
   if ( IS_AFFECTED( ch, AFF_POISON ) )
   gain /= 4;
   /*
   if (is_affected(ch, gsn_healing_trance))
   gain *=2;
   */
   if (ch->position == POS_SLEEPING && get_skill(ch, gsn_dark_dream) > 5)
   {
      if (number_percent() < get_skill(ch, gsn_dark_dream))
      {
         check_improve(ch, gsn_dark_dream, TRUE, 5);
         gain *= 3;
         gain /= 2;
      }

   }

   if (IS_AFFECTED(ch, AFF_PLAGUE))
   gain /= 8;

   if (IS_AFFECTED(ch, AFF_HASTE))
   {
      if
      (
         IS_AFFECTED2(ch, AFF_HYPER) &&
         (
            hyper = get_skill(ch, gsn_hyper)
         ) > 0
      )
      {
         haste_reduce = gain - (gain / 2);
         haste_reduce = haste_reduce * (100 - hyper) / 100;
         gain -= haste_reduce;
      }
      else
      {
         gain /=2;
      }
   }
   if ( IS_AFFECTED(ch, AFF_SLOW))
   gain *=2;
   if ( is_affected(ch, gsn_lemniscate_brand))
   gain *=2;


   if (is_affected(ch, gsn_camp))
   {
      if (number_percent() < ch->pcdata->learned[gsn_camp])
      {
         check_improve(ch, gsn_camp, TRUE, 7);
         gain *=2;
      } else
      check_improve(ch, gsn_camp, FALSE, 7);
   }


   if (is_affected(ch, gsn_distort_time)){
      if (is_affected(ch, gsn_distort_time_faster)){
         gain *= 2; /* two ticks instead of one */
      }
      else
      {
         if (is_odd_hour(ch))
         return 0; /*only gain on odd ticks if slower */
      }
   }

   if (ch->race == grn_arborian)
   {
      if (current_mud_time.hour >= 5 && current_mud_time.hour < 20)
      gain *=4;
   }
   return UMIN(gain, ch->max_mana - ch->mana);
}



int move_gain( CHAR_DATA *ch )
{
   int gain;

   if (ch->in_room == NULL)
   return 0;
   if (is_affected(ch, gsn_atrophy) || is_affected(ch, gsn_prevent_healing))
   return 0;
   if (!IS_NPC(ch))
   {
      if (ch->pcdata->condition[COND_STARVING] > 6)
      return 0;
      if (ch->pcdata->condition[COND_DEHYDRATED] > 4)
      return 0;
   }

   if ( IS_NPC(ch) )
   {
      gain = ch->level;
   }
   else
   {
      gain = UMAX( 15, ch->level );

      switch ( ch->position )
      {
         case POS_SLEEPING: gain += get_curr_stat(ch, STAT_DEX);        break;
         case POS_RESTING:  gain += get_curr_stat(ch, STAT_DEX) / 2;    break;
      }

      if ( ch->pcdata->condition[COND_HUNGER]   == 0 )
      gain /= 2;

      if ( ch->pcdata->condition[COND_THIRST] == 0 )
      gain /= 2;
   }

   gain = gain * ch->in_room->heal_rate/100;

   if (ch->on != NULL && ch->on->item_type == ITEM_FURNITURE)
   gain = gain * 6/5;

   /*
   gain = gain * ch->on->value[3] / 100;
   */

   if (ch->position == POS_SLEEPING && get_skill(ch, gsn_dark_dream) > 5)
   {
      if (number_percent() < get_skill(ch, gsn_dark_dream))
      {
         check_improve(ch, gsn_dark_dream, TRUE, 8);
         gain *=3;
         gain /=2;
      }
   }

   if ( IS_AFFECTED(ch, AFF_POISON) )
   gain /= 4;

   if (IS_AFFECTED(ch, AFF_PLAGUE))
   gain /= 8;
   /*
   if (is_affected(ch, gsn_healing_trance))
   gain *=2;
   */

   if (is_affected(ch, gsn_ancient_plague))
   gain /= 8;

   if (is_affected(ch, gsn_cunning_strike))
   gain /= 4;

   if (is_affected(ch, gsn_nether_shroud))
   {
      if (IS_EVIL(ch))
      gain *= 2;
      else
      gain /= 2;
   }

   if
   (
      IS_AFFECTED(ch, AFF_HASTE) ||
      IS_AFFECTED(ch, AFF_SLOW)
   )
   {
      gain *= 2;
   }

   if (is_affected(ch, gsn_camp))
   {
      if (number_percent() < ch->pcdata->learned[gsn_camp])
      {
         check_improve(ch, gsn_camp, TRUE, 7);
         gain *= 2;
      } else
      check_improve(ch, gsn_camp, FALSE, 7);
   }


   if (is_affected(ch, gsn_distort_time)){
      if (is_affected(ch, gsn_distort_time_faster)){
         gain *= 2; /* two ticks instead of one */
      }
      else
      {
         if (is_odd_hour(ch))
         return 0; /*only gain on odd ticks if slower */
      }
   }

   if (ch->race == grn_arborian)
   {
      if (current_mud_time.hour >= 5 && current_mud_time.hour < 20)
      gain *=4;
   }

   return UMIN(gain, ch->max_move - ch->move);
}



void gain_condition( CHAR_DATA *ch, int iCond, int value )
{
   int condition;
   int counter;
   int les_branded;
   OBJ_DATA *brand;

   if (
         value == 0 ||
         IS_NPC(ch) ||
         ch->level >= LEVEL_IMMORTAL
      )
   {
      return;
   }

   if (ch == NULL || ch->in_room == NULL)
   {
      return;
   }

   if (ch->desc == NULL || ch->desc->connected != CON_PLAYING)
   {
      return;
   }

   if (value < 0 && ch->pcdata->death_status == HAS_DIED)
   {
      return;
   }

   les_branded = is_branded_by_lestregus(ch);

   if
   (
      (
         iCond == COND_HUNGER ||
         iCond == COND_STARVING ||
         (
            (
               iCond == COND_THIRST ||
               iCond == COND_DEHYDRATED
            ) &&
            !les_branded
         )
      ) &&
      value < 0  &&
      (
         ch->pcdata->special == SUBCLASS_KNIGHT_OF_FAMINE ||
         IS_SET(ch->act2, PLR_LICH) ||
         ch->race == grn_demon ||
         ch->race == grn_book
      )
   )
   {
      return;
   }

   if (ch->race == grn_arborian)
   {
      if
      (
         (
            iCond == COND_HUNGER ||
            iCond == COND_STARVING
         ) &&
         value < 0
      )
      {
         return;
      }
      if (
            iCond == COND_THIRST &&
            !les_branded &&
            value < 0 &&
            ch->in_room->sector_type != SECT_DESERT
         )
      {
         value *= 3;
         if (weather_info.sky <= SKY_RAINING && !IS_SET(ch->in_room->room_flags, ROOM_INDOORS))
         value = 0;
      }
   }

   if ((iCond==COND_HUNGER || iCond==COND_THIRST) && value <0 &&
   IS_SET(ch->in_room->extra_room_flags, ROOM_1212))
   return;

   condition                = ch->pcdata->condition[iCond];
   if (condition == -1)
   return;

   if (
         value < 0 &&
         iCond == COND_THIRST &&
         !les_branded &&
         ch->in_room->sector_type == SECT_DESERT
      )
   {
      value *= 5;
   }

   if (
         ch->pcdata->condition[iCond] == 0 &&
         iCond == COND_THIRST &&
         !les_branded &&
         ch->in_room->sector_type == SECT_DESERT &&
         number_range(1, 4) == 1
      )
   {
      send_to_char("The hot desert sun parches your mouth!\n\r", ch);
   }

   if (
         !les_branded ||
         value == -1 ||
         (
            iCond != COND_THIRST &&
            iCond != COND_DEHYDRATED
         ) ||
         (
            ch->pcdata->condition[COND_THIRST] == 0 &&
            ch->in_room->vnum == 30103
         )
      )
   {
      ch->pcdata->condition[iCond] = URANGE(0, condition + value, 48);
   }

   if
   (
      ch->pcdata->brand_rank > 0 &&
      (
         iCond == COND_THIRST ||
         iCond == COND_HUNGER
      ) &&
      ch->pcdata->condition[iCond] <= 4
   )
   {
      brand = get_eq_char(ch, WEAR_BRAND);

      if
      (
         brand != NULL &&
         brand->pIndexData->vnum == OBJ_VNUM_OBLIVION_BRAND
      )
      {
         ch->pcdata->condition[iCond] = 5;
      }
   }

   if (ch->level > 10)
   {
      if (ch->pcdata->condition[iCond] == 0 && iCond == COND_HUNGER)
      ch->pcdata->condition[COND_STARVING]++;
      if (ch->pcdata->condition[iCond] == 0 && iCond == COND_THIRST)
      ch->pcdata->condition[COND_DEHYDRATED]++;

      if (
            ch->pcdata->condition[iCond] == 0 &&
            iCond == COND_THIRST &&
            !les_branded &&
            (ch->in_room->sector_type == SECT_DESERT)
         )
      {
         ch->pcdata->condition[COND_DEHYDRATED] += 2;
      }
   }
   else
   {
      ch->pcdata->condition[COND_STARVING] = 0;
      ch->pcdata->condition[COND_DEHYDRATED] = 0;
   }

   if (iCond == COND_HUNGER && value > 0
   && ch->pcdata->condition[COND_STARVING] > 0)
   {
      counter = ch->pcdata->condition[COND_STARVING];
      if (counter <= 4)
      send_to_char("You are no longer famished.\n\r", ch);
      else
      send_to_char("You are no longer starving.\n\r", ch);
      ch->pcdata->condition[COND_STARVING] = 0;
      ch->pcdata->condition[COND_HUNGER] = 2;
   }

   if (
         iCond == COND_THIRST &&
         value > 0 &&
         ch->pcdata->condition[COND_DEHYDRATED] > 0
      )
   {
      if (!les_branded)
      {
         counter = ch->pcdata->condition[COND_DEHYDRATED];
         if (counter <= 5)
         send_to_char("You are no longer dehydrated.\n\r", ch);
         else
         send_to_char("You are no longer dying of thirst.\n\r", ch);
         ch->pcdata->condition[COND_DEHYDRATED] = 0;
         ch->pcdata->condition[COND_THIRST] = 2;
      } else
      {
         send_to_char(
            "Your flesh calms as the blood of the eternal "
            "passes over your lips.\n\r",
            ch);
         ch->pcdata->condition[COND_DEHYDRATED] = 0;
         ch->pcdata->condition[COND_THIRST] = 48;
      }
   }

   if ( ch->pcdata->condition[iCond] <= 4 )
   {
      switch ( iCond )
      {
         case COND_HUNGER:
         if (ch->pcdata->condition[COND_STARVING] < 2)
         send_to_char( "You are hungry.\n\r",  ch );
         break;

         case COND_THIRST:
         if (
               ch->pcdata->condition[COND_DEHYDRATED] < 2 &&
               !les_branded
            )
         {
            if (ch->race != grn_arborian)
            {
               send_to_char( "You are thirsty.\n\r", ch );
            }
            else
            {
               send_to_char("You are beginning to dry up.\n\r", ch);
            }
         }
         break;

         case COND_DRUNK:
         if ( condition != 0 )
         send_to_char( "You are sober.\n\r", ch );
         break;
      }
   }

   if (ch->pcdata->condition[COND_STARVING] > 1
   && iCond == COND_HUNGER)
   {
      counter = ch->pcdata->condition[COND_STARVING];
      if (counter <= 5)
      send_to_char("You are famished!\n\r", ch);
      else if (counter <= 8)
      send_to_char("You are beginning to starve!\n\r", ch);
      else
      {
         send_to_char("You are starving!\n\r", ch);
         if
         (
            ch->level > 10 &&
            !IS_AFFECTED(ch, AFF_SLEEP) &&
            !is_affected(ch, gsn_blackjack) &&
            !is_affected(ch, gsn_strangle)
         )
         {
            damage(ch, ch, number_range(counter - 3, 2*(counter - 3)), gsn_starvation, DAM_OTHER, TRUE);
         }
      }
   }

   if (
         iCond == COND_THIRST &&
         (
            ch->pcdata->condition[COND_DEHYDRATED] > 1 ||
            (
               les_branded &&
               ch->pcdata->condition[COND_DEHYDRATED] > 0
            )
         )
      )
   {
      counter = ch->pcdata->condition[COND_DEHYDRATED];

      if (!les_branded)
      {
         if (counter <= 2)
         {
            if (ch->race != grn_arborian)
            send_to_char("Your mouth is parched!\n\r", ch);
            else
            send_to_char("Your leaves feel parched!\n\r", ch);
         }
         else if (counter <= 5)
         send_to_char("You are beginning to dehydrate!\n\r", ch);
         else
         {
            send_to_char("You are dying of thirst!\n\r", ch);
            if
            (
               ch->level > 10 &&
               !IS_AFFECTED(ch, AFF_SLEEP) &&
               !is_affected(ch, gsn_blackjack) &&
               !is_affected(ch, gsn_strangle)
            )
            {
               damage(ch, ch, number_range(counter, 2*counter), gsn_dehydrated, DAM_OTHER, TRUE);
            }
         }
      } else
      {
         switch(counter)
         {
            case 1:
               act(
                  "You hear the cackle of the LichLord as your skin begins "
                  "to dry and crack.",
                  ch,
                  NULL,
                  NULL,
                  TO_CHAR);
               act(
                  "You hear the cackle of the LichLord as $n's skin begins "
                  "to dry and crack.",
                  ch,
                  NULL,
                  NULL,
                  TO_ROOM);
               break;
            case 2:
               act(
                  "A burning sensation stings your eyes as they "
                  "start to wither.",
                  ch,
                  NULL,
                  NULL,
                  TO_CHAR);
               act(
                  "$n's eyes begin to wither and seep.",
                  ch,
                  NULL,
                  NULL,
                  TO_ROOM);
               break;
            case 3:
               act(
                  "Your hair begins to fall out in clumps as your skull "
                  "begins to rot.",
                  ch,
                  NULL,
                  NULL,
                  TO_CHAR);
               act(
                  "$n's hair begins falling out as $s skull begins to rot.",
                  ch,
                  NULL,
                  NULL,
                  TO_ROOM);
               break;
            case 4:
               act(
                  "You feel your muscles weaken and begin to atrophy.",
                  ch,
                  NULL,
                  NULL,
                  TO_CHAR);
               act(
                  "$n's body sags and droops as $s muscles weaken and "
                  "begin to atrophy.",
                  ch,
                  NULL,
                  NULL,
                  TO_ROOM);
               break;
            case 5:
               act(
                  "Your skin splits open and the gashes leak puss and blood.",
                  ch,
                  NULL,
                  NULL,
                  TO_CHAR);
               act(
                  "$n's skin splits open and the gashes leak puss and blood.",
                  ch,
                  NULL,
                  NULL,
                  TO_ROOM);
               break;
            case 6:
               act(
                  "Your bones begin to break and crumble under "
                  "their own weight.",
                  ch,
                  NULL,
                  NULL,
                  TO_CHAR);
               act(
                  "$n's bones begin to break and crumble under "
                  "their own weight.",
                  ch,
                  NULL,
                  NULL,
                  TO_ROOM);
               break;
            case 7:
               act(
                  "Your eyes sink into your skull and you begin "
                  "gasping for air.",
                  ch,
                  NULL,
                  NULL,
                  TO_CHAR);
               act(
                  "$n gasps for air as $s eyes sink into $s skull.",
                  ch,
                  NULL,
                  NULL,
                  TO_ROOM);
               break;
            case 8:
               act(
                  "Darkness envelopes your vision and blood begins "
                  "leaking from your ears.",
                  ch,
                  NULL,
                  NULL,
                  TO_CHAR);
               act(
                  "$n gasps as blood begins leaking from $s ears.",
                  ch,
                  NULL,
                  NULL,
                  TO_ROOM);
               break;
            case 9:
               act(
                  "Your hands begin to rot and curl inward, and two of "
                  "your fingers fall off.",
                  ch,
                  NULL,
                  NULL,
                  TO_CHAR);
               act(
                  "Two fingers fall off as $n's hands begin to rot and "
                  "curl inward.",
                  ch,
                  NULL,
                  NULL,
                  TO_ROOM);
               break;
            case 10:
               act(
                  "Your heart palpitates as your veins rise to the "
                  "surface and burst!",
                  ch,
                  NULL,
                  NULL,
                  TO_CHAR);
               act(
                  "$n's veins rise to the surface and burst!",
                  ch,
                  NULL,
                  NULL,
                  TO_ROOM);
               break;
            default:
               send_to_char("What just happened?\n\r", ch);
               break;
         }

         if (counter < 10)
         {
            damage(
               ch,
               ch,
               ch->hit * ((counter * 8) + 12) / 100,
               gsn_lestregus_brand_thirst,
               DAM_OTHER,
               TRUE);
         }
         else
         {
            damage(
               ch,
               ch,
               ch->max_hit * 5,
               gsn_lestregus_brand_thirst,
               DAM_OTHER,
               TRUE);
         }
      }
   }
   return;
}

void spec_update( CHAR_DATA *ch )
{
   CHAR_DATA *mob;
   CHAR_DATA *mob_next;

   for ( mob = ch->in_room->people; mob != NULL; mob = mob_next )
   {
      mob_next = mob->next;

      if ( IS_NPC(mob) && mob->spec_fun != 0 )
      {
         if (mob->wait > 0 || mob->daze > 0 ) continue;
         if ( (*mob->spec_fun) ( mob ) )
         return;
      }
   }
   return;
}


/*
* Mob autonomous action.
* This function takes 25% to 35% of ALL Merc cpu time.
* -- Furey
*/
void mobile_update( void )
{
   CHAR_DATA *ch,*gch;
   CHAR_DATA *ch_next;
   char buf[MAX_STRING_LENGTH];
   EXIT_DATA *pexit;
   int door, i;
   AFFECT_DATA* paf;

   /* Examine all mobs. */
   for ( ch = char_list; ch != NULL; ch = ch_next )
   {
      ch_next = ch->next;

      if
      (
         !IS_NPC(ch) ||
         !ch->on_line ||
         (
            IS_NPC(ch) &&
            ch->in_room == NULL &&
            ch->master &&
            !IS_NPC(ch->master) &&
            ch->master->pet == ch &&
            IS_AFFECTED(ch, AFF_CHARM) &&
            ch->master->desc &&
            ch->master->desc->connected != CON_PLAYING
         )
      )
      {
         continue;
      }
      /* cause sleeping mobs affected by timers blackjack and strangle to wake
      -Wervdon*/

      if
      (
         IS_NPC(ch) &&
         !IS_AWAKE(ch) &&
         (
            is_affected(ch, gsn_corrupt("strangle timer", &gsn_strangle_timer)) ||
            is_affected(ch, gsn_blackjack_timer) ||
            is_affected(ch, gsn_stunning_strike)
         )
      )
      {
         do_stand(ch, "");
      }

      /*  Removed at Malignus' request and Xyza's approval
      if (IS_NPC(ch) &&
      ch->pIndexData->vnum == MOB_VNUM_OUTLAW_GIANT &&
      ch->pause == 0)
      {
      act("$n gets bored and wanders off.", ch, NULL, NULL, TO_ROOM);
      extract_char(ch, TRUE);
      continue;
      }
      */



      if
      (
         IS_NPC(ch) &&
         ch->pIndexData->vnum == MOB_VNUM_OUTLAW_GIANT &&
         ch->pause == 0
      )
      {
         act("$n gets bored and wanders off.", ch, NULL, NULL, TO_ROOM);
         extract_char(ch, TRUE);
         continue;
      }




      if
      (
         IS_NPC(ch) &&
         ch->pIndexData->vnum == MOB_VNUM_GNOME_SPY &&
         ch->pause == 0
      )
      {
         act("$n disappears into the night.", ch, NULL, NULL, TO_ROOM);
         extract_char(ch, TRUE);
         continue;
      }

      if (IS_NPC(ch) &&
      ch->pIndexData->vnum == MOB_VNUM_SPORE &&
      ch->pause == 0)
      {
         act("$n shrivels up and dies.", ch, NULL, NULL, TO_ROOM);
         extract_char(ch, TRUE);
         continue;
      }

      if
      (
         ch->race == grn_doppelganger &&
         (ch->position != POS_FIGHTING) &&
         (number_range(0, 9)==0)
      )
      {
         for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
         {
            if
            (
               IS_NPC(gch) &&
               gch != ch &&
               number_range(1, 4) == 1
            )
            {
               act("$n shifts its form suddenly.", ch, NULL, NULL, TO_ROOM);
               send_to_char("You shift your form!\n\r", ch);
               sprintf(buf, "doppelganger %s", gch->name);
               free_string(ch->name);
               ch->name = str_dup(buf);
               free_string(ch->short_descr);
               ch->short_descr  = str_dup(gch->short_descr);
               free_string(ch->long_descr);
               ch->long_descr   = str_dup(gch->long_descr);
               free_string(ch->description);
               ch->description  = str_dup(gch->description);
               ch->sex          = gch->sex;
               ch->level        = gch->level;
               ch->trust        = 0;
               if (ch->last_fought <= 0)
               {
                  ch->hit          = gch->hit;
                  ch->max_hit      = gch->max_hit;
               }
               ch->mana         = gch->mana;
               ch->max_mana     = gch->max_mana;
               ch->move         = gch->move;
               ch->max_move     = gch->max_move;
               ch->exp          = gch->exp;
               ch->affected_by  = gch->affected_by;
               ch->saving_throw = gch->saving_throw;
               ch->sight        = gch->sight;
               ch->spell_power  = gch->spell_power;
               ch->holy_power   = gch->holy_power;
               ch->saving_spell = gch->saving_spell;
               ch->saving_breath= gch->saving_breath;
               ch->saving_maledict = gch->saving_maledict;
               ch->saving_transport= gch->saving_transport;
               ch->hitroll      = gch->hitroll;
               ch->damroll      = gch->damroll;
               ch->form         = gch->form;
               ch->parts        = gch->parts;
               ch->size         = gch->size;
               for (i = 0; i < 4; i++)
               ch->armor[i] = gch->armor[i];
               for (i = 0; i < MAX_STATS; i++)
               {
                  ch->perm_stat[i]     = gch->perm_stat[i];
                  ch->mod_stat[i]      = gch->mod_stat[i];
               }
               for (i = 0; i < 3; i++)
               ch->damage[i]        = gch->damage[i];
               if
               (
                  IS_AFFECTED(ch, AFF_SANCTUARY) &&
                  !is_affected(ch, gsn_sanctuary) &&
                  !is_affected(ch, gsn_chromatic_shield)
               )
               {
                  paf = new_affect();
                  paf->where     = TO_AFFECTS;
                  paf->level     = ch->level;
                  paf->duration  = -1;
                  paf->bitvector = AFF_SANCTUARY;
                  if (is_clergy(ch))
                  {
                     paf->type = gsn_sanctuary;
                  }
                  else
                  {
                     paf->type = gsn_chromatic_shield;
                  }
                  affect_to_char_1(ch, paf);
                  free_affect(paf);
               }
               act("$n looks around for prey.", ch, NULL, NULL, TO_ROOM);
               send_to_char("You look around for prey.\n\r", ch);
               break;
            }
         }
         continue;
      }

      if ( !IS_NPC(ch) || ch->in_room == NULL )
      /*        || (IS_NPC(ch) && ch->pIndexData->vnum != MOB_VNUM_ENFORCER
      && ch->pIndexData->vnum  != MOB_VNUM_DEMON1
      &&IS_AFFECTED(ch, AFF_CHARM))) */
      continue;

      if (ch->in_room->area->empty && !IS_SET(ch->act, ACT_UPDATE_ALWAYS))
      continue;

      /* Examine call for special procedure */
      if ( ch->spec_fun != 0 )
      {
         if ( (*ch->spec_fun) ( ch ) )
         continue;
      }

      /*
      * Check triggers only if mobile still in default position
      */
      if ( ch->position == ch->pIndexData->default_pos )
      {

         /* Delay */
         if ( HAS_TRIGGER( ch, TRIG_DELAY)
         &&   ch->mprog_delay > 0 )
         {

            if ( --ch->mprog_delay <= 0 )
            {

               mp_percent_trigger( ch, NULL, NULL, NULL, TRIG_DELAY );
               continue;
            }
         }

         if ( HAS_TRIGGER( ch, TRIG_RANDOM) )
         {
            if ( mp_percent_trigger( ch, NULL, NULL, NULL, TRIG_RANDOM ) )
            continue;
         }
      }

      if (ch->pIndexData->pShop != NULL) /* give him some gold */
      if ((ch->gold * 100 + ch->silver) < ch->pIndexData->wealth)
      {
         ch->gold += ch->pIndexData->wealth * number_range(1, 20)/5000000;
         ch->silver += ch->pIndexData->wealth * number_range(1, 20)/50000;
      }


      /* That's all for sleeping / busy monster, and empty zones */
      if ( ch->position != POS_STANDING )
      continue;

      /* Scavenge */
      if ( IS_SET(ch->act, ACT_SCAVENGER)
      &&   ch->in_room->contents != NULL
      &&   number_bits( 6 ) == 0 )
      {
         OBJ_DATA *obj;
         OBJ_DATA *obj_best;
         int max;

         max         = 1;
         obj_best    = NULL;
         for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
         {
            if
            (
               CAN_WEAR(obj, ITEM_TAKE) &&
               can_loot(ch, obj) &&
               can_see_obj(ch, obj) &&
               count_users(obj) == 0 &&
               obj->cost > max &&
               obj->pIndexData->house == 0 &&
               obj->cost > 0
            )
            {
               obj_best    = obj;
               max         = obj->cost;
            }
         }

         if
         (
            obj_best &&
            get_obj(ch, obj_best, NULL, 0) &&
            obj_best->carried_by == ch
         )
         {
            REMOVE_BIT(obj_best->extra_flags, ITEM_INVENTORY);
         }
      }

      /* Wander */
      if ( !IS_SET(ch->act, ACT_SENTINEL)
      && number_bits(3) == 0
      && ( door = number_bits( 5 ) ) <= 5
      && ( pexit = ch->in_room->exit[door] ) != NULL
      &&   pexit->u1.to_room != NULL
      &&   !IS_SET(pexit->exit_info, EX_CLOSED)
      &&   !IS_SET(pexit->u1.to_room->room_flags, ROOM_NO_MOB)
      && ( !IS_SET(ch->act, ACT_STAY_AREA)
      ||   pexit->u1.to_room->area == ch->in_room->area )
      && ( !IS_SET(ch->act, ACT_OUTDOORS)
      ||   !IS_SET(pexit->u1.to_room->room_flags, ROOM_INDOORS))
      && ( !IS_SET(ch->act, ACT_INDOORS)
      ||   IS_SET(pexit->u1.to_room->room_flags, ROOM_INDOORS)))
      {
         move_char(ch, door, 0);
      }
   }

   return;
}



/*
* Update the weather.
*/
void weather_update( bool controlled )
{
   char buf[MAX_STRING_LENGTH];
   DESCRIPTOR_DATA *d;
   int diff;

   buf[0] = '\0';

   switch (current_mud_time.hour)
   {
      case  (4):
      {
         if (weather_info.sunlight == SUN_LIGHT)
         {
            break;
         }
         weather_info.sunlight = SUN_LIGHT;
         if (immrp_darkness)
         {
            break;
         }
         strcat( buf, "The day has begun.\n\r" );
         break;
      }
      case  (5):
      {
         if (weather_info.sunlight == SUN_RISE)
         {
            break;
         }
         weather_info.sunlight = SUN_RISE;
         if (immrp_darkness)
         {
            break;
         }
         strcat( buf, "The sun rises in the east.\n\r" );
         break;
      }
      case (18):
      {
         if (weather_info.sunlight == SUN_SET)
         {
            break;
         }
         weather_info.sunlight = SUN_SET;
         if (immrp_darkness)
         {
            break;
         }
         strcat( buf, "The sun slowly disappears in the west.\n\r" );
         break;
      }
      case (19):
      {
         if (weather_info.sunlight == SUN_DARK)
         {
            break;
         }
         weather_info.sunlight = SUN_DARK;
         if (immrp_darkness)
         {
            break;
         }
         strcat( buf, "The night has begun.\n\r" );
         break;
      }
      default:
      {
         break;
      }
   }

   /*
   * Weather change.
   */
   if (current_mud_time.month >= 9 && current_mud_time.month <= 12)
   {
      diff = weather_info.mmhg >  985 ? -2 : 2;
   }
   else
   {
      diff = weather_info.mmhg > 1015 ? -2 : 2;
   }

   weather_info.change   += diff * dice(1, 4) + dice(2, 6) - dice(2, 6);
   weather_info.change    = UMAX(weather_info.change, -12);
   weather_info.change    = UMIN(weather_info.change,  12);

   weather_info.mmhg += weather_info.change;
   weather_info.mmhg  = UMAX(weather_info.mmhg,  960);
   weather_info.mmhg  = UMIN(weather_info.mmhg, 1040);

   switch ( weather_info.sky )
   {
      default:
      bug( "Weather_update: bad sky %d.", weather_info.sky );
      weather_info.sky = SKY_CLOUDLESS;
      break;

      case SKY_CLOUDLESS:
      if ( weather_info.mmhg <  990
      || ( weather_info.mmhg < 1010 && number_bits( 2 ) == 0 ) )
      {
         strcat( buf, "The sky is getting cloudy.\n\r" );
         weather_info.sky = SKY_CLOUDY;
      }
      break;

      case SKY_CLOUDY:
      if ( weather_info.mmhg <  970
      || ( weather_info.mmhg <  990 && number_bits( 2 ) == 0 ) )
      {
         strcat( buf, "It starts to rain.\n\r" );
         weather_info.sky = SKY_RAINING;
      }

      if ( weather_info.mmhg > 1030 && number_bits( 2 ) == 0 )
      {
         strcat( buf, "The clouds disappear.\n\r" );
         weather_info.sky = SKY_CLOUDLESS;
      }
      break;

      case SKY_RAINING:
      if ( weather_info.mmhg <  970 && number_bits( 2 ) == 0 )
      {
         strcat( buf, "Lightning flashes in the clouds above you.\n\r");
         weather_info.sky = SKY_LIGHTNING;
      }

      if ( weather_info.mmhg > 1030
      || ( weather_info.mmhg > 1010 && number_bits( 2 ) == 0 ) )
      {
         strcat( buf, "The rain eases up and finally stops.\n\r" );
         weather_info.sky = SKY_CLOUDY;
      }
      break;

      case SKY_LIGHTNING:
      if ( weather_info.mmhg > 1010
      || ( weather_info.mmhg >  990 && number_bits( 2 ) == 0 ) )
      {
         strcat( buf, "The lightning has stopped.\n\r" );
         weather_info.sky = SKY_RAINING;
         break;
      }
      break;
   }

   if ( buf[0] != '\0' )
   {
      for ( d = descriptor_list; d != NULL; d = d->next )
      {
         if ( d->connected == CON_PLAYING
         &&   IS_OUTSIDE(d->character)
         &&   IS_AWAKE(d->character) )
         send_to_char( buf, d->character );
      }
   }

   return;
}



/*
* Update all chars, including mobs.
*/
void char_update( void )
{
   CHAR_DATA *ch;
   CHAR_DATA *ch_next;
   CHAR_DATA *ch_quit;
   OBJ_DATA *helm;
   int noregen, hgain;
   int sn;
   char chbuf[MAX_STRING_LENGTH];
   OBJ_DATA* brand;
   char* wear_off;

   ch_quit    = NULL;

   /* update save counter */
   save_number++;

   if (save_number > 29)
   save_number = 0;

   for ( ch = char_list; ch != NULL; ch = ch_next )
   {
      CHAR_DATA *master;
      AFFECT_DATA *paf;
      AFFECT_DATA *paf_next;
      OBJ_DATA *obj;
      bool charm_gone;
      bool recruit_gone;
      bool mob_gone;
      bool enlist_gone;
      bool distort;

      ch_next = ch->next;

      if
      (
         !ch->on_line ||
         (
            ch->in_room == NULL &&
            (
               (
                  !IS_NPC(ch) &&
                  ch->desc &&
                  ch->desc->connected != CON_PLAYING
               ) ||
               (
                  IS_NPC(ch) &&
                  ch->master &&
                  !IS_NPC(ch->master) &&
                  ch->master->pet == ch &&
                  IS_AFFECTED(ch, AFF_CHARM) &&
                  ch->master->desc &&
                  ch->master->desc->connected != CON_PLAYING
               )
            )
         )
      )
      {
         continue;
      }
      if (!check_room(ch, TRUE, "Purge in char_update()"))
      {
         continue;
      }
      if ( ch->timer > 30 )
      ch_quit = ch;
      master = NULL;
      if ( ch->pause > 0 )
      ch->pause--;
      if ( ch->quittime > 0 )
      ch->quittime--;
      if (ch->bloody_shrine > 0)
      ch->bloody_shrine--;

      if ( ch->ghost > 0 )
      {
         ch->ghost--;
      }
      else if
      (
         !IS_NPC(ch) &&
         ch->house == HOUSE_ENFORCER &&
         ch->in_room != NULL &&
         ch->in_room->guild == 0 &&
         check_room_protected(ch->in_room)
      )
      {
         ch->pcdata->targetkills++;
      }
      if ( !IS_NPC(ch) && ch->pcdata->molting_into)
      {
         int cnt;
         ch->pcdata->molting_time--;
         if (ch->pcdata->molting_time == 250 || ch->pcdata->molting_time == 200 ||
         ch->pcdata->molting_time == 150 || ch->pcdata->molting_time == 100 ||
         ch->pcdata->molting_time == 50  || ch->pcdata->molting_time == 40 ||
         ch->pcdata->molting_time == 30  || ch->pcdata->molting_time == 20 ||
         ch->pcdata->molting_time <=10)
         {
            send_to_char("You shed a few of your old scales, and your transformation continues.\n\r", ch);
         }
         if (ch->pcdata->molting_time <= 0)
         {
            ch->subrace = ch->pcdata->molting_into;
            ch->pcdata->molting_into = 0;
            ch->pcdata->molting_time = 0;
            send_to_char("Your transformation into a different scale color is complete.\n\r", ch);
            for (cnt = STAT_STR; cnt <= STAT_CON; cnt++)
            if (ch->perm_stat[cnt] > get_max_train(ch, cnt))
            {
               ch->train += ch->perm_stat[cnt] - get_max_train(ch, cnt);
               ch->perm_stat[cnt] = get_max_train(ch, cnt);
            }
         }
      }

      brand = get_eq_char(ch, WEAR_BRAND);
      irv_update(ch, TRUE, FALSE);
      /*
      IS_FLYING(ch) macro checks for earthbind
      if (is_affected(ch, gsn_earthbind))
      REMOVE_BIT(ch->affected_by, AFF_FLYING);
      */

      obj = get_eq_char(ch, WEAR_FEET);
      if (obj == NULL)
      {
         obj = get_eq_char(ch, WEAR_HOOVES);
      }
      if (obj == NULL)
      {
         obj = get_eq_char(ch, WEAR_FOURHOOVES);
      }

      if (obj != NULL)
      if (obj->pIndexData->vnum == OBJ_VNUM_SHACKLES)
      {
         REMOVE_BIT(ch->affected_by, AFF_SNEAK);
         if (ch->quittime < 14 &&
         ch->fighting == NULL &&
         ch->move >= (ch->max_move - ch->max_move/10) &&
         number_percent() < 50)
         {
            send_to_char("You manage to break the lock on your shackles and are free of them!\n\r", ch);
            extract_obj(obj, FALSE);
         }
      }

      /*
      if (ch->race == 3 || ch->race == 4 || ch->race == 5)
      {
      SET_BIT(ch->affected_by, AFF_SNEAK);
      SET_BIT(ch->affected_by, AFF_INFRARED);
      }

      if (ch->race == 2 || ch->race == 9)
      {
      SET_BIT(ch->affected_by, AFF_INFRARED);
      }

      if (ch->race == 10)
      {
      SET_BIT(ch->affected_by, AFF_FLYING);
      }

      if (ch->race == 11)
      {
      SET_BIT(ch->affected_by, AFF_PASS_DOOR);
      SET_BIT(ch->affected_by, AFF_INFRARED);
      SET_BIT(ch->affected_by, AFF_FLYING);
      }
      */
      if ( ch->position >= POS_STUNNED )
      {
         /* check to see if we need to go home */
         /*
         if (IS_NPC(ch) && ch->zone != NULL && ch->zone != ch->in_room->area
         && ch->desc == NULL &&  ch->fighting == NULL
         && !IS_AFFECTED(ch, AFF_CHARM) && number_percent() < 5)
         {
         act("$n wanders on home.", ch, NULL, NULL, TO_ROOM);
         extract_char(ch, TRUE);
         continue;
         }
         */
         /* room sound updates -werv */
         if (!IS_NPC(ch)){
            if (ch->in_room->sound != NULL
            && number_percent() <ch->in_room->sound_rate)
            send_to_char(ch->in_room->sound, ch);
         }

         if ( !wears_coiled_snakes(ch) )
         {
            if ( ch->hit < ch->max_hit )
            {
               noregen = ch->noregen_dam;
               hgain = hit_gain(ch);
               ch->hit  += hgain;
               if (noregen > hgain)
               ch->noregen_dam -= hgain;
               else
               ch->noregen_dam = 0;
            }
            else
            ch->hit = ch->max_hit;

            if ( ch->mana < ch->max_mana )
            ch->mana += mana_gain(ch);
            else
            ch->mana = ch->max_mana;

            if ( ch->move < ch->max_move )
            ch->move += move_gain(ch);
            else
            ch->move = ch->max_move;
         } else
         {
            ch->hit = UMIN(ch->hit, ch->max_hit);
            ch->mana = UMIN(ch->mana, ch->max_mana);
            ch->move = UMIN(ch->move, ch->max_move);
         }
      }

      if ( ch->position == POS_STUNNED )
      update_pos( ch );

      if
      (
         ch->hit < 0 &&
         ch->position >= POS_STUNNED &&
         (
            IS_NPC(ch) ||
            !(
               ch->pcdata->brand_rank > 0 &&
               brand != NULL &&
               brand->pIndexData->vnum == OBJ_VNUM_OBLIVION_BRAND &&
               ch->hit >= (ch->max_hit * -5 / 100)
            )
         )
      )
      {
         raw_kill(ch, ch);
      }

      if ( !IS_NPC(ch) && ch->level < LEVEL_IMMORTAL )
      {
         OBJ_DATA *obj;

         if ( ( obj = get_eq_char( ch, WEAR_LIGHT ) ) != NULL
         &&   obj->item_type == ITEM_LIGHT
         &&   obj->value[2] > 0 )
         {
            if ( --obj->value[2] == 0 && ch->in_room != NULL )
            {
               /*
               No need to remove.  Light will be reduced when
               the object is extraced (Fizzfaldt)
               ch->in_room->light -= 3;
               */
               act( "$p goes out.", ch, obj, NULL, TO_ROOM );
               act( "$p flickers and goes out.", ch, obj, NULL, TO_CHAR );
               extract_obj( obj, FALSE );
            }
            else if ( obj->value[2] <= 5 && ch->in_room != NULL)
            act("$p flickers.", ch, obj, NULL, TO_CHAR);
         }

         if (IS_IMMORTAL(ch))
         ch->timer = 0;

         if ( ++ch->timer >= 12 )
         {
            if ( ch->was_in_room == NULL && ch->in_room != NULL )
            {
               ch->was_in_room = ch->in_room;
               if ( ch->fighting != NULL )
               stop_fighting( ch, TRUE );
               act( "$n disappears into the void.",
               ch, NULL, NULL, TO_ROOM );
               send_to_char( "You disappear into the void.\n\r", ch );
               if (ch->level > 1)
               {
                  save_char_obj(ch);
               }
               ch->pause = 0;
               ch->quittime = -1;  /* Can void in enemy house */
               do_quit(ch, "");
               continue;
            }
         }

         gain_condition( ch, COND_DRUNK,  -1 );
         gain_condition( ch, COND_FULL, -3 );
         gain_condition( ch, COND_THIRST, -1 );
         gain_condition( ch, COND_HUNGER, -1);
      }

      /* Ancient blood reminder */
      if (IS_NPC(ch) && (ch->pIndexData->vnum == 3807))
      {
         OBJ_DATA* obj;
         for ( obj = object_list; obj != NULL; obj = obj->next )
         if (obj->pIndexData->vnum == 3827)
         {
            if (obj->value[1] <= 0)
            switch (number_range(1, 3))
            {
               case 1:
               do_yell(ch, "BLOOOOOOD! MOOOOORE BLOOOOOOOOD! WHERE IS MY BLOOOOOOOD????");
               break;
               case 2:
               do_yell(ch, "WHERE IS MY OFFERING?! FIND BLOOD! REEEEAAAL BLOOD!!!");
               break;
               case 3:
               do_yell(ch, "MORE BLOOOOOD!! PERHAPS I TASTE ANCIENT BLOOD?!");
               break;
            }
            else
            obj->value[1] = UMAX(obj->value[1] - 2, 0);
            break;
         }
      }

      /* Outlaw fence reminder */
      /* Xurinos - OUTLAW no longer has fence

      if (!IS_NPC(ch) && !IS_IMMORTAL(ch) && ch->house == HOUSE_OUTLAW && ch->level >= 13
      && (get_skill(ch, gsn_fence) > 1))
      {
      OBJ_DATA *fenceobj;
      bool fencing=FALSE;
      for ( fenceobj = ch->carrying; fenceobj != NULL; fenceobj = fenceobj->next_content )
      {
      if (fenceobj->newcost > 0) { fencing = TRUE; break; }
      }
      if (!fencing)
      {
      if (IS_SET(ch->comm, COMM_ANSI))
      send_to_char("\x01B[31mThe Black Market is waiting for something to fence for you.\x01B[0;37m\n\r", ch);
      else
      send_to_char("The Black Market is waiting for something to fence for you.\n\r", ch);
      }
      }
      */
      if
      (
         IS_NPC(ch) &&
         (
            IS_SET(ch->act2, ACT_STRAY) ||
            IS_SET(ch->act2, ACT_NICE_STRAY)
         ) &&
         IS_AFFECTED(ch, AFF_CHARM) &&
         ch->master &&
         ch->in_room != NULL &&
         IS_AWAKE(ch) &&
         !stray_update(ch)
      )
      {
         /* if stray_update == FALSE, ch is DEAD */
         continue;
      }

      if
      (
         is_affected(ch, gsn_song_of_shadows)
      )
      {
         if
         (
            IS_AFFECTED(ch, AFF_FAERIE_FIRE)
         )
         {
            affect_strip(ch, gsn_faerie_fire);
            REMOVE_BIT(ch->affected_by, AFF_FAERIE_FIRE);
            send_to_char("You feel the shadows absorbing the faerie fire around you.\n\r", ch);
         }
         if
         (
            is_affected(ch, gsn_faerie_fog)
         )
         {
            affect_strip(ch, gsn_faerie_fog);
            send_to_char("You feel the shadows absorbing the faerie fog around you.\n\r", ch);
         }
         if
         (
            !IS_AFFECTED(ch, AFF_SNEAK)
         )
         {
            SET_BIT(ch->affected_by, AFF_SNEAK);
         }
      }

      mob_gone = FALSE;
      recruit_gone = FALSE;
      enlist_gone = FALSE;
      for ( paf = ch->affected; paf != NULL; paf = paf_next )
      {
         paf_next    = paf->next;
         charm_gone = FALSE;

         if (is_affected(ch, gsn_distort_time))
         distort = TRUE;
         else
         distort = FALSE;

         if ( paf->duration > 0 )
         {
            if (distort){
               if (paf->type == gsn_distort_time || paf->type ==
               gsn_distort_time_faster
               || paf->type == gsn_distort_time_slower)
               {
                  paf->duration--;
               }
               else
               {
                  if (is_affected(ch, gsn_distort_time_faster)){
                     if ( paf->duration > 1)
                     paf->duration = paf->duration - 2;
                     else
                     paf->duration--;
                  }
                  else
                  if (is_odd_hour(ch))
                  paf->duration--;
               }
            }
            else
            paf->duration--;

            if (number_range(0, 4) == 0 && paf->level > 0)
            paf->level--;  /* spell strength fades with time */
         }
         else if ( paf->duration < 0 )
         ;
         else
         {
            if ( paf_next == NULL
            ||   paf_next->type != paf->type
            ||   paf_next->duration > 0 )
            {
               if
               (
                  paf->type > 0 &&
                  (
                     wear_off = get_herb_spell_name(paf, FALSE, 0)
                  ) != NULL &&
                  wear_off[0] != '\0'
               )
               {
                  if
                  (
                     paf->type == gsn_corrupt("freedom", &gsn_mob_timer) &&
                     IS_NPC(ch)
                  )
                  {
                     mob_gone = TRUE;
                  }
                  if
                  (
                     paf->type == gsn_recruit &&
                     IS_NPC(ch)
                  )
                  {
                     recruit_gone = TRUE;
                  }
                  if
                  (
                     paf->type == gsn_enlist &&
                     IS_NPC(ch)
                  )
                  {
                     enlist_gone = TRUE;
                  }
                  send_to_char(wear_off, ch );
                  send_to_char( "\n\r", ch );
                  if (!str_cmp(skill_table[paf->type].name, "lifeline"))
                  ch->life_lined = NULL;
                  /* Spark wear off message for Hector's brand - Wicket */
                  if (!str_cmp(skill_table[paf->type].name, "Spark of Innovation"))
                  act("The brilliant aura surrounding $n subsides.", ch, NULL, NULL, TO_ROOM);
               }
               if (paf->type == gsn_shapeshift
               && (ch->mprog_target != NULL))
               {
                  act("$n resumes $s natural form.", ch, NULL, NULL, TO_ROOM);
                  ch->mprog_target = NULL;
                  ch->morph_form[1] = 0;
                  ch->morph_form[2] = 0;
               }
            }
            if (paf->type == gsn_charm_person
            && IS_NPC(ch))
            charm_gone = TRUE;
            if (IS_NPC(ch) &&
            paf->type == gsn_propaganda)
            ch->spec_fun = ch->pIndexData->spec_fun;

            if (paf->type == gsn_peace)
            {
               switch (paf->modifier)
               {
                  case MODIFY_PEACE_BY_SOOTHING_VOICE:
                     send_to_char("You shake off the feeling of serenity.\n\r", ch);
                     break;

                  case MODIFY_PEACE_BY_CIRDAN_BRAND:
                  default:
                     send_to_char("Your sense of overwhelming peace dissipates.\n\r", ch);
               }
            }

            if (paf->type == gsn_cunning_strike)
               {
	               if (paf->location == APPLY_NONE)
		            {
	                  send_to_char("Your wound clots and stops bleeding.\n\r", ch);
		            }
		            else if (paf->location == APPLY_MOVE)
		            {
			            send_to_char("You feel that your bones have knitted back together.\n\r", ch);
		            }
		            else
		            {
			            send_to_char("The nerves in your body have recovered and function properly again.\n\r", ch);
		            }
	            }
            if (paf->type == gsn_thugs_stance)
            {
               AFFECT_DATA th_af;
               th_af.type = gsn_corrupt("thugs stance timer", &gsn_thugs_stance_timer);
               th_af.level = GET_LEVEL(ch);
               th_af.duration = 0;
               th_af.modifier = 0;
               th_af.bitvector = 0;
               th_af.location = APPLY_NONE;
               affect_to_char(ch, &th_af);
            }
            affect_remove( ch, paf );
         }
         if (charm_gone)
         master = ch->master;
         if (master == NULL)
         continue;
         if (charm_gone)
         ch->last_fought = master->id;
         if ((charm_gone) && ch->position != POS_FIGHTING
         && (number_percent() >= master->level)
         && (ch->in_room == master->in_room))
         {
            sprintf(chbuf, "Help! I'm being attacked by %s!", PERS(ch, master));
            do_myell(master, chbuf);
            multi_hit(ch, master, TYPE_UNDEFINED);
            ch->master = NULL;
            ch->leader = NULL;
         }
      }
      if (recruit_gone)
      {
         act
         (
            "$n leaves pursue $s own business.",
            ch,
            NULL,
            ch->master,
            TO_NOTVICT
         );
         if (ch->master == NULL)
         {
            extract_char(ch, TRUE);
            continue;
         }
         act
         (
            "$n leaves you to pursue $s own business.",
            ch,
            NULL,
            ch->master,
            TO_VICT
         );
         if (ch->fighting == NULL)
         {
            extract_char(ch, TRUE);
            continue;
         }
         REMOVE_BIT(ch->affected_by, AFF_CHARM);
         stop_follower(ch);
      }
      if (enlist_gone)
      {
         act
         (
            "$n leaves pursue $s own business.",
            ch,
            NULL,
            ch->master,
            TO_NOTVICT
         );
         stop_follower(ch);
      }
      if (mob_gone)
      {
         /* EMOTE */
         if (ch->master)
         {
            act
            (
               "$n leaves you to pursue $s own business.",
               ch,
               NULL,
               ch->master,
               TO_VICT
            );
         }
         act
         (
            "$n leaves to pursue $s own business.",
            ch,
            NULL,
            ch->master,
            TO_ROOM
         );
         extract_char(ch, TRUE);
         continue;
      }

      if
      (
         !IS_NPC(ch) &&
         (
            (
               (
                  helm = get_eq_char(ch, WEAR_HEAD)
               ) != NULL &&
               helm->pIndexData->vnum == 27513
            ) ||
            (
               (
                  helm = get_eq_char(ch, WEAR_HORNS)
               ) != NULL &&
               helm->pIndexData->vnum == 27513
            )
         ) &&
         number_range(1, 20) == 1
      )
      {
         for (sn = 1; sn < MAX_SKILL; sn++ )
         {
            if
            (
               skill_table[sn].name != NULL &&
               ch->pcdata->learned[sn] > 1 &&
               skill_table[sn].spell_fun != spell_null
            )
            {
               ch->pcdata->learned[sn]--;
            }
         }
         if (is_clergy(ch))
         {
            send_to_char
            (
               "Your knowledge of prayers slowly drains away.\n\r",
               ch
            );
         }
         else
         {
            send_to_char("Your magic slowly drains away.\n\r", ch);
         }
      }

      /*
      * Careful with the damages here,
      *   MUST NOT refer to ch after damage taken,
      *   as it may be lethal damage (on NPC).
      */
      if (ch != NULL && is_affected(ch, gsn_plague_of_the_worm))
      {
         AFFECT_DATA *af;
         for (af = ch->affected; af != NULL; af = af->next)
         {
            if (af->type == gsn_plague_of_the_worm)
            {
               break;
            }
         }
         if (af != NULL)
         {
            act
            (
               "$n writhes in agony as plague sores erupt from $s skin.",
               ch,
               NULL,
               NULL,
               TO_ROOM
            );
            send_to_char("You writhe in agony from the plague.\n\r", ch);
            damage(ch, ch, ch->level + 1, gsn_plague_of_the_worm, DAM_DISEASE, TRUE);
         }
      }

      if (ch != NULL && is_affected(ch, gsn_cunning_strike))
      {
         AFFECT_DATA *af;
         CHAR_DATA *damager;
         for (af = ch->affected; af != NULL; af = af->next)
         {
            if (af->type == gsn_cunning_strike)
            {
               break;
            }
         }
         if
         (
            af != NULL &&
            af->location == APPLY_NONE
         )
         {
            act
            (
               "$n bleeds from a deep cut.",
               ch,
               NULL,
               NULL,
               TO_ROOM
            );
            send_to_char("You bleed out of your wound.\n\r", ch);
            damager = get_damager(ch, af);
            if
            (
               !IS_AFFECTED(ch, AFF_SLEEP) &&
               !is_affected(ch, gsn_blackjack) &&
               !is_affected(ch, gsn_strangle)
            )
            {
	            damage(damager, ch, af->modifier, gsn_cunning_strike, DAM_OTHER, BLEEDING_DAM_TYPE);
            }
            af->modifier += number_range(2, 5);
         }
      }


      if (ch != NULL && is_affected(ch, gsn_divine_inferno))
      {
         AFFECT_DATA *af;
         for (af = ch->affected; af != NULL; af = af->next)
         {
            if (af->type == gsn_divine_inferno)
            {
               break;
            }
         }
         if (af != NULL)
         {
            act
            (
               "$n is consumed in flame.",
               ch,
               NULL,
               NULL,
               TO_ROOM
            );
            send_to_char("You are consumed in flame.\n\r", ch);
            damage(ch, ch, ch->level * 2, gsn_divine_inferno, DAM_OTHER, TRUE);
         }
      }
      if (is_affected(ch, gsn_plague) && ch != NULL)
      {
         AFFECT_DATA *af, plague;
         CHAR_DATA *vch;
         int dam;
         CHAR_DATA *damager;

         if (ch->in_room == NULL)
         return;

         act("$n writhes in agony as plague sores erupt from $s skin.",
         ch, NULL, NULL, TO_ROOM);
         send_to_char("You writhe in agony from the plague.\n\r", ch);
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
            if (!saves_spell(ch, plague.level - 2, vch, DAM_DISEASE, SAVE_OTHER)
            &&  !IS_IMMORTAL(vch)
            &&  !IS_AFFECTED(vch, AFF_PLAGUE) && number_bits(4) == 0)
            {
               send_to_char("You feel hot and feverish.\n\r", vch);
               act("$n shivers and looks very ill.", vch, NULL, NULL, TO_ROOM);
               affect_join(vch, &plague, 10);
            }
         }

         dam = UMIN(ch->level, af->level);
         /*
         ch->mana -= dam;
         ch->move -= dam;
         */
         damager = get_damager(ch, af);
         if
         (
            !IS_AFFECTED(ch, AFF_SLEEP) &&
            !is_affected(ch, gsn_blackjack) &&
            !is_affected(ch, gsn_strangle)
         )
         {
            damage(damager, ch, dam, gsn_plague, DAM_DISEASE, PLAGUE_DAM_TYPE);
         }
      }

      if (is_affected(ch, gsn_ancient_plague) && ch != NULL)
      {
         AFFECT_DATA ap;
         CHAR_DATA *vch;

         send_to_char("A feverish wave of nausea jolts you!\n\r", ch);
         ch->position = POS_STANDING;
         ap.where    = TO_AFFECTS;
         ap.type     = gsn_ancient_plague;
         ap.level     = 20;
         ap.duration     = 10;
         ap.location    = APPLY_STR;
         ap.modifier     = -2;
         ap.bitvector     = 0;
         for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
         {
            if (!saves_spell(ch, ap.level, vch, DAM_DISEASE, SAVE_OTHER)
            &&  !IS_IMMORTAL(vch)
            &&  !is_affected(vch, gsn_ancient_plague)
            &&  number_percent() < 20)
            {
               send_to_char("You feel sick, and a profound fatigue overcomes you.\n\r", vch);
               act("$n turns pale, and looks very sick.", vch, NULL, NULL, TO_ROOM);
               affect_to_char(vch, &ap);
               ap.location = APPLY_CON;
               affect_to_char(vch, &ap);
            }
         }
      }

      if (is_affected(ch, gsn_boiling_blood) && ch != NULL)
      {
         AFFECT_DATA *boil;
         CHAR_DATA* damager;
         int dam;

         dam = 0;
         boil = affect_find(ch->affected, gsn_boiling_blood);
         if (boil != NULL)
         {
            dam = (dice(1, (boil->level)*2));
            act("$n swoons in anguish as their vital fluids boil!", ch, NULL, NULL, TO_ROOM);
            send_to_char("You swoon in anguish as your vital fluids boil!\n\r", ch);
            damager = get_damager(ch, boil);
            if
            (
               !IS_AFFECTED(ch, AFF_SLEEP) &&
               !is_affected(ch, gsn_blackjack) &&
               !is_affected(ch, gsn_strangle)
            )
            {
               damage(damager, ch, dam, gsn_boiling_blood, DAM_FIRE, BOILING_BLOOD_DAM_TYPE);
            }
         }
      }
      if
      (
         ch != NULL &&
         IS_NPC(ch) &&
         is_affected(ch, gsn_esurience)
      )
      {
         AFFECT_DATA* esur;
         CHAR_DATA* damager;
         int dam;

         esur = affect_find(ch->affected, gsn_esurience);
         if (esur != NULL)
         {
            dam = esur->level / 2;
            damager = get_damager(ch, esur);
            if
            (
               !IS_AFFECTED(ch, AFF_SLEEP) &&
               !is_affected(ch, gsn_blackjack) &&
               !is_affected(ch, gsn_strangle)
            )
            {
               damage
               (
                  damager,
                  ch,
                  dam,
                  gsn_starvation,
                  DAM_OTHER,
                  STARVATION_DAM_TYPE
               );
               damage
               (
                  damager,
                  ch,
                  dam,
                  gsn_dehydrated,
                  DAM_OTHER,
                  DEHYDRATION_DAM_TYPE
               );
            }
         }
      }

      if (is_affected(ch, gsn_bloodmist) && ch != NULL)
      {
         AFFECT_DATA *bm;
         CHAR_DATA* damager;

         bm = affect_find(ch->affected, gsn_bloodmist);
         if (bm != NULL)
         {
            act( "$n screams and hemmorages uncontrollably!", ch, NULL, NULL, TO_ROOM );
            send_to_char( "You scream as blood erupts from your pores!\n\r", ch );
            damager = get_damager(ch, bm);
            if
            (
               !IS_AFFECTED(ch, AFF_SLEEP) &&
               !is_affected(ch, gsn_blackjack) &&
               !is_affected(ch, gsn_strangle)
            )
            {
               damage(damager, ch, bm->level, gsn_bloodmist, DAM_HARM, BLOODMIST_DAM_TYPE);
            }
         }
      }


      if ( wears_coiled_snakes(ch) )
      {
         send_to_char( "The coil of snakes squeezes mercilessly!\n\r", ch);
         damage(ch, ch, 50, gsn_coil_snakes, DAM_BASH, TRUE);
      }


      /* Shoppies get gold back each tick to simulate sells -Wervdon
      Only shoppies, and only 1/60th of their reset value for wealth.
      All in silver cause its easier to code that way and one less condition
      to test on ticks.
      */

      if (IS_NPC(ch) && ch->pIndexData->pShop !=NULL
      && ch->pIndexData->wealth != 0
      && ch->gold < 10000){
         ch->silver =  ch->silver + (ch->pIndexData->wealth)/60;
         if (ch->silver > 1000){
            ch->silver = ch->silver - 500;
            ch->gold = ch->gold + 5;
         }
      }

      water_update(ch, TRUE);

      if ( IS_AFFECTED(ch, AFF_POISON) && ch != NULL
      &&   !IS_AFFECTED(ch, AFF_SLOW))

      {
         AFFECT_DATA *poison;
         CHAR_DATA *damager;

         poison = affect_find(ch->affected, gsn_poison);

         if (poison != NULL)
         {
            act( "$n shivers and suffers.", ch, NULL, NULL, TO_ROOM );
            send_to_char( "You shiver and suffer.\n\r", ch );
            /*
            ch->mana -= poison->level;
            ch->move -= poison->level;
            */
            damager = get_damager(ch, poison);
            if
            (
               !IS_AFFECTED(ch, AFF_SLEEP) &&
               !is_affected(ch, gsn_blackjack) &&
               !is_affected(ch, gsn_strangle)
            )
            {
               damage(damager, ch, poison->level, gsn_poison, DAM_POISON, POISON_DAM_TYPE);
            }
         }
      }

      if ( ch != NULL &&
      !IS_IMMORTAL(ch) &&
      is_affected_room(ch->in_room, gsn_storm) &&
      number_range(1, 4) == 1)
      {
         ROOM_AFFECT_DATA *storm;
         CHAR_DATA *damager;

         storm = affect_find_room(ch->in_room, gsn_storm);

         if (storm != NULL && storm->caster != NULL && !is_safe_room(ch, storm, IS_SAFE_SILENT))
         {
            int dam;
            if ( IS_AFFECTED( ch, gsn_camp  ) ) affect_strip( ch, gsn_camp  );
            damager = get_damager_room(ch, storm);
            act("$n is struck by lightning.", ch, NULL, NULL, TO_ROOM);
            send_to_char("You are struck by lightning.\n\r", ch);
            dam = dice(storm->level, 3);
            damage(damager, ch, dam, gsn_storm, DAM_LIGHTNING, TORNADO_LIT_DAM_TYPE);
         }
      }

      if ( ch != NULL &&
      !IS_IMMORTAL(ch) &&
      is_affected_room(ch->in_room, gsn_water_spout))
      {
         ROOM_AFFECT_DATA *tornado;
         CHAR_DATA *damager;

         tornado = affect_find_room(ch->in_room, gsn_water_spout);

         if (tornado != NULL && tornado->caster != NULL && !is_safe_room(ch, tornado, IS_SAFE_SILENT))
         {
            int dam;
            if ( IS_AFFECTED( ch, gsn_camp  ) ) affect_strip( ch, gsn_camp  );
            damager = get_damager_room(ch, tornado);
            if (number_range(0, 1) == 0)
            {
               act("$n is deluged by huge waves.", ch, NULL, NULL, TO_ROOM);
               send_to_char("You are deluged by huge waves.\n\r", ch);
               dam = dice(tornado->level, 4);
               damage(damager, ch, dam, gsn_tornado, DAM_DROWNING, TORNADO_DROWN_DAM_TYPE);
            }
            else
            {
               int n_dice = 3;
               act("$n is caught in the fierce winds.", ch, NULL, NULL, TO_ROOM);
               send_to_char("You are caught in the fierce winds.\n\r", ch);
               if (IS_FLYING(ch))
               {
                  n_dice *= 2;
                  act("$n is tossed about by the winds.", ch, NULL, NULL, TO_ROOM);
               }
               dam = dice(tornado->level, n_dice);
               if ((has_skill(ch, gsn_wraithform) && get_skill(ch, gsn_wraithform) >= 75)
               || (IS_NPC(ch) && ch->pIndexData->vnum == MOB_VNUM_AIR_ELEMENTAL))
               {
                  act("$n seems amused by the winds.", ch, NULL, NULL, TO_ROOM);
               }
               else
               {
                  damage(damager, ch, dam, gsn_tornado, DAM_SLASH, TORNADO_WIND_DAM_TYPE);
               }
            }
         }
      }


      if ( ch != NULL &&
      !IS_IMMORTAL(ch) &&
      is_affected_room(ch->in_room, gsn_tornado))
      {
         ROOM_AFFECT_DATA *tornado;
         CHAR_DATA *damager;

         tornado = affect_find_room(ch->in_room, gsn_tornado);

         if (tornado != NULL && tornado->caster != NULL && !is_safe_room(ch, tornado, IS_SAFE_SILENT))
         {
            int dam;
            if ( IS_AFFECTED( ch, gsn_camp  ) ) affect_strip( ch, gsn_camp  );
            damager = get_damager_room(ch, tornado);
            if (number_range(0, 1) == 0)
            {
               act("$n is struck by lightning.", ch, NULL, NULL, TO_ROOM);
               send_to_char("You are struck by lightning.\n\r", ch);
               dam = dice(tornado->level, 4);
               damage(damager, ch, dam, gsn_tornado, DAM_LIGHTNING, TORNADO_LIT_DAM_TYPE);
            }
            else
            {
               int n_dice = 3;
               act("$n is caught in the fierce winds.", ch, NULL, NULL, TO_ROOM);
               send_to_char("You are caught in the fierce winds.\n\r", ch);
               if (IS_FLYING(ch))
               {
                  n_dice *= 2;
                  act("$n is tossed about by the winds.", ch, NULL, NULL, TO_ROOM);
               }
               dam = dice(tornado->level, n_dice);
               if ((has_skill(ch, gsn_wraithform) && get_skill(ch, gsn_wraithform) >= 75)
               || (IS_NPC(ch) && ch->pIndexData->vnum == MOB_VNUM_AIR_ELEMENTAL))
               {
                  act("$n seems amused by the winds.", ch, NULL, NULL, TO_ROOM);
               }
               else
               {
                  damage(damager, ch, dam, gsn_tornado, DAM_SLASH, TORNADO_WIND_DAM_TYPE);
               }
            }
         }
      }

      if ( ch != NULL &&
      !IS_IMMORTAL(ch) &&
      is_affected_room(ch->in_room, gsn_raging_fire))
      {
         ROOM_AFFECT_DATA *fire;
         CHAR_DATA *damager;
         AFFECT_DATA af;

         fire = affect_find_room(ch->in_room, gsn_raging_fire);

         if (fire != NULL && fire->caster != NULL && !is_safe_room(ch, fire, IS_SAFE_SILENT))
         {
            int dam;
            if ( IS_AFFECTED( ch, gsn_camp  ) ) affect_strip( ch, gsn_camp  );
            act( "The raging fire burns $n.", ch, NULL, NULL, TO_ROOM );
            send_to_char( "You are burned by the raging fire!\n\r", ch );

            damager = get_damager_room(ch, fire);
            dam = fire->level*(5-fire->duration);
            dam = number_range(dam/2, dam);
            if (dam < 20) dam = 20;
            if (!saves_spell(damager, ch->level, ch, DAM_FIRE, SAVE_SPELL)
            && !is_affected(ch, gsn_inferno))
            {
               af.where = TO_AFFECTS;
               af.location = APPLY_HITROLL;
               af.modifier = -3;
               af.duration = 1;
               af.level = ch->level;
               af.type = gsn_inferno;
               af.bitvector = AFF_BLIND;
               act("$n appears blinded by the smoke.", ch, NULL, NULL, TO_ROOM);
               affect_to_char(ch, &af);
            }
            damage(damager, ch, dam, gsn_raging_fire, DAM_FIRE, RAGING_FIRE_DAM_TYPE);
         }
      }


      if ( is_affected(ch, gsn_suffocate) && ch != NULL)

      {
         AFFECT_DATA *suffocate;
         CHAR_DATA *damager;

         suffocate = affect_find(ch->affected, gsn_suffocate);

         if (suffocate != NULL)
         {
            if ( IS_AFFECTED( ch, gsn_camp  ) ) affect_strip( ch, gsn_camp  );
            act( "$n gasps and gags as $e is deprived of air.", ch, NULL, NULL, TO_ROOM );
            send_to_char( "You gasp as you are robbed of air!\n\r", ch );
            damager = get_damager(ch, suffocate);
            if
            (
               !IS_AFFECTED(ch, AFF_SLEEP) &&
               !is_affected(ch, gsn_blackjack) &&
               !is_affected(ch, gsn_strangle)
            )
            {
               damage(damager, ch, dice(suffocate->level, 2), gsn_suffocate, DAM_OTHER, SUFFOCATE_DAM_TYPE);
            }
         }
      }

      if ( is_affected(ch, gsn_temporal_shear) && ch != NULL)

      {
         AFFECT_DATA *temp_shear;
         CHAR_DATA *damager;

         temp_shear = affect_find(ch->affected, gsn_temporal_shear);

         if (temp_shear != NULL)
         {
            if ( IS_AFFECTED( ch, gsn_camp  ) ) affect_strip( ch, gsn_camp  );
            act( "$n is caught in a temporal vortex.", ch, NULL, NULL, TO_ROOM );
            send_to_char( "The vortex shudders and you relive the past!\n\r", ch );
            damager = get_damager(ch, temp_shear);
            if
            (
               !IS_AFFECTED(ch, AFF_SLEEP) &&
               !is_affected(ch, gsn_blackjack) &&
               !is_affected(ch, gsn_strangle)
            )
            {
               damage(damager, ch, dice(temp_shear->level, 2), gsn_temporal_shear, DAM_ENERGY, TEMP_SHEER_DAM_TYPE);
            }
         }
      }

      if (ch != NULL)
      {
         if (ch->position == POS_INCAP && number_range(0, 1) == 0)
         {
            damage( ch, ch, 1, TYPE_UNDEFINED, DAM_OTHER, FALSE);
         }
         else if (ch->position == POS_MORTAL)
         {
            damage( ch, ch, 1, TYPE_UNDEFINED, DAM_OTHER, FALSE);
         }
      }

      if (ch != NULL && ch->morph_form[0] == MORPH_DRACOLICH)
      {
         AFFECT_DATA* af;
         AFFECT_DATA  bloodlust_effect;
         int chance;

         af = affect_find(ch->affected, gsn_rite_of_darkness);

         if (af != NULL && !is_affected(ch, gsn_dracolich_bloodlust))
         {
            chance = UMAX(10 * (5 - af->duration), 10);

            if (number_percent() < chance)
            {
               act(
                  "Your soul flees from Shal'ne-Sir, and his "
                  "spirit takes possession of your body!",
                  ch,
                  NULL,
                  NULL,
                  TO_CHAR);
               act(
                  "$n's face twists abruptly into an evil snarl.",
                  ch,
                  NULL,
                  NULL,
                  TO_ROOM);

               bloodlust_effect.where     = TO_AFFECTS;
               bloodlust_effect.level     = GET_LEVEL(ch);
               bloodlust_effect.duration  = af->duration;
               bloodlust_effect.location  = APPLY_NONE;
               bloodlust_effect.modifier  = 0;
               bloodlust_effect.bitvector = 0;
               bloodlust_effect.type      = gsn_dracolich_bloodlust;
               affect_to_char(ch, &bloodlust_effect);
            }
         }
      }

      /* VOID Elemental XUR */
      if (IS_NPC(ch) && (ch->pIndexData->vnum == MOB_VNUM_VOID_ELEMENTAL)
      && (number_range(1, 24) == 1))
      {
         CHAR_DATA *fch, *fch_next;
         int ElemNumber = 0, ThisOne;

         for (fch = ch->in_room->people; fch; fch = fch->next_in_room)
         {
            if
            (
               (IS_NPC(fch)) &&
               (ch->master == fch->master) &&
               (
                  (fch->pIndexData->vnum == MOB_VNUM_FIRE_ELEMENTAL) ||
                  (fch->pIndexData->vnum == MOB_VNUM_AIR_ELEMENTAL) ||
                  (fch->pIndexData->vnum == MOB_VNUM_WATER_ELEMENTAL) ||
                  (fch->pIndexData->vnum == MOB_VNUM_EARTH_ELEMENTAL)
               )
            )
            ElemNumber++;
         }

         if (ElemNumber > 0)
         {
            ThisOne = number_range(1, ElemNumber);
            ElemNumber = 0;
            for (fch = ch->in_room->people; fch; fch = fch_next)
            {
               fch_next = fch->next_in_room;

               if
               (
                  (IS_NPC(fch)) && (ch->master == fch->master) &&
                  (
                     (fch->pIndexData->vnum == MOB_VNUM_FIRE_ELEMENTAL) ||
                     (fch->pIndexData->vnum == MOB_VNUM_AIR_ELEMENTAL) ||
                     (fch->pIndexData->vnum == MOB_VNUM_WATER_ELEMENTAL) ||
                     (fch->pIndexData->vnum == MOB_VNUM_EARTH_ELEMENTAL)
                  )
               )
               {
                  if (++ElemNumber == ThisOne)
                  {
                     act("$N screams in horror as $E is consumed by $n!", ch, NULL, fch, TO_ROOM);
                     extract_char(fch, TRUE);
                     break;
                  }
               }
            }
         }
      }

   }

   /*
   * Autosave and autoquit.
   * Check that these chars still exist.
   */
   for ( ch = char_list; ch != NULL; ch = ch_next )
   {
      ch_next = ch->next;

      if
      (
         ch->desc != NULL &&
         ch->desc->connected == CON_PLAYING &&
         ch->desc->descriptor % 30 == save_number
      )
      {
         save_char_obj(ch);
      }

      if ( ch == ch_quit )
      do_quit( ch, "" );
   }

   return;
}


sh_int wears_coiled_snakes( CHAR_DATA *ch )
{
   OBJ_DATA *snakes;

   if ( (snakes = get_eq_char(ch, WEAR_NECK_1)) != NULL
   && snakes->pIndexData->vnum == OBJ_VNUM_COIL_SNAKES )
   return WEAR_NECK_1;

   if ( (snakes = get_eq_char(ch, WEAR_NECK_2)) != NULL
   && snakes->pIndexData->vnum == OBJ_VNUM_COIL_SNAKES )
   return WEAR_NECK_2;

   if ( (snakes = get_eq_char(ch, WEAR_BODY)) != NULL
   && snakes->pIndexData->vnum == OBJ_VNUM_COIL_SNAKES )
   return WEAR_BODY;

   if ( (snakes = get_eq_char(ch, WEAR_WAIST)) != NULL
   && snakes->pIndexData->vnum == OBJ_VNUM_COIL_SNAKES )
   return WEAR_WAIST;

   if ( (snakes = get_eq_char(ch, WEAR_HEAD)) != NULL
   && snakes->pIndexData->vnum == OBJ_VNUM_COIL_SNAKES )
   return WEAR_HEAD;

   return FALSE;
}


/*
* Update all objs.
* This function is performance sensitive.
*/
void obj_update( void )
{
   OBJ_DATA *obj;
   OBJ_DATA *obj_next;
   OBJ_INDEX_DATA* index;
   AFFECT_DATA *paf, *paf_next;
   ROOM_INDEX_DATA *room;
   CHAR_DATA* owner;
   bool reslot;

   for ( obj = object_list; obj != NULL; obj = obj_next )
   {
      CHAR_DATA *rch;
      char *message;

      obj_next = obj->next;

      if
      (
         obj->carried_by &&
         !obj->carried_by->on_line
      )
      {
         continue;
      }
      /* thief's gloves */
      if ((obj->pIndexData->vnum == 1889) && (number_range(1, 9) <= 3))
      {
         CHAR_DATA *ch;
         OBJ_DATA *sobj;
         char thisitem [MAX_INPUT_LENGTH];
         sprintf(thisitem, "1.");

         if (((ch = obj->carried_by) != NULL) && (!IS_NPC(ch)))
         {
            CHAR_DATA *victim;
            int countperson=0, whichperson=0;

            for (victim = ch->in_room->people; victim != NULL; victim = victim->next_in_room)
            {
               if (ch == victim) continue;
               if (IS_IMMORTAL(victim)) continue;
               if (ch->pcdata->death_status == HAS_DIED) continue;
               if (is_safe(ch, victim, IS_SAFE_SILENT))
               {
                  continue;
               }

               if ((!IS_NPC(victim)) && (victim->pcdata->death_status == HAS_DIED)) continue;

               /* Handle Link dead players , only saves from PKS, not mobs */
               if (!IS_NPC(victim) && victim->desc == NULL
               && victim->fighting != ch && ch->fighting != victim
               && victim->quittime < 17) continue;

               if (IS_NPC(victim) && IS_SET(victim->act, ACT_IS_CHANGER) ) continue;

               if (is_affected(victim, gsn_timestop) ||
               is_affected(victim, gsn_stone))
               continue;

               if (victim->ghost > 0) continue;
               if (ch->ghost > 0)
               {
                  if (!IS_NPC(victim)) continue;
                  if (IS_AFFECTED(victim, AFF_CHARM) && victim->master != NULL
                  && !IS_NPC(victim->master)) continue;
               }

               whichperson++;
            }

            if (whichperson != 0)
            {
               whichperson = number_range(1, whichperson);
               for (victim = ch->in_room->people; victim != NULL; victim = victim->next_in_room)
               {
                  if (ch == victim) continue;
                  if (IS_IMMORTAL(victim)) continue;

                  if (ch->pcdata->death_status == HAS_DIED) continue;
                  if (is_safe(ch, victim, IS_SAFE_SILENT))
                  {
                     continue;
                  }

                  if ((!IS_NPC(victim)) && (victim->pcdata->death_status == HAS_DIED)) continue;

                  /* Handle Link dead players , only saves from PKS, not mobs */
                  if (!IS_NPC(victim) && victim->desc == NULL
                  && victim->fighting != ch && ch->fighting != victim
                  && victim->quittime < 17) continue;

                  if (IS_NPC(victim) && IS_SET(victim->act, ACT_IS_CHANGER) ) continue;

                  if (is_affected(victim, gsn_timestop) ||
                  is_affected(victim, gsn_stone))
                  continue;

                  if (victim->ghost > 0) continue;
                  if (ch->ghost > 0)
                  {
                     if (!IS_NPC(victim)) continue;
                     if (IS_AFFECTED(victim, AFF_CHARM) && victim->master != NULL
                     && !IS_NPC(victim->master)) continue;
                  }

                  countperson++;
                  if (countperson == whichperson)
                  {
                     if (!IS_NPC(victim))
                     {
                        if (!IS_IMMORTAL(ch)) QUIT_STATE(ch, 20);
                        sprintf(log_buf, "[%s] used the thieves gloves on [%s] at %d",
                        ch->name, victim->name, ch->in_room->vnum);
                        log_string(log_buf);
                     }
                     if (number_range(0, 1) == 1)
                     {
                        /* steal gold */
                        int gold, silver;

                        gold = victim->gold * number_range(1, ch->level) / 60;
                        silver = victim->silver * number_range(1, ch->level) / 60;

                        if ( gold > 0 || silver > 0 )
                        {
                           ch->gold        += gold;
                           ch->silver      += silver;
                           victim->silver  -= silver;
                           victim->gold    -= gold;
                        }
                     }
                     else
                     /* steal item */
                     if ((( ( sobj = get_obj_carry( victim, thisitem, ch ) ) !=NULL )
                     && (!( !can_drop_obj( victim, sobj )
                     ||   (IS_SET(sobj->extra_flags, ITEM_INVENTORY)
                     && IS_NPC(victim))))) &&
                     (!( ch->carry_number + get_obj_number( sobj ) > can_carry_n( ch ) )) &&
                     (!( ch->carry_weight + get_obj_weight( sobj ) > can_carry_w( ch ) )))
                     {
                        if (get_obj_weight(sobj) >
                        ((str_app[get_curr_stat(ch, STAT_STR)].wield * 10)))
                        {
                           send_to_char("You stumble under the weight of your possessions.\n\r", ch);
                           un_hide(ch, NULL);  /* XUR */
                        }

                        obj_from_char( sobj );
                        obj_to_char( sobj, ch );
                     }
                  }
               }
            }
         }
      }

      /* go through affects and decrement */
      for ( paf = obj->affected; paf != NULL; paf = paf_next )
      {
         paf_next    = paf->next;
         if ( paf->duration > 0 )
         {
            paf->duration--;
            if (number_range(0, 4) == 0 && paf->level > 0)
            paf->level--;  /* spell strength fades with time */
         }
         else if ( paf->duration < 0 )
         ;
         else
         {
            if ( paf_next == NULL
            ||   paf_next->type != paf->type
            ||   paf_next->duration > 0 )
            {
               if ( paf->type > 0 && skill_table[paf->type].msg_obj )
               {
                  if (obj->carried_by != NULL)
                  {
                     rch = obj->carried_by;
                     act(skill_table[paf->type].msg_obj,
                     rch, obj, NULL, TO_CHAR);
                  }
                  if (obj->in_room != NULL
                  && obj->in_room->people != NULL)
                  {
                     rch = obj->in_room->people;
                     act(skill_table[paf->type].msg_obj,
                     rch, obj, NULL, TO_ALL);
                  }
               }
            }

            affect_remove_obj( obj, paf );
         }
      }

      if (obj->pIndexData->vnum == OBJ_VNUM_SWORD_DEMONS)
      sword_demon_update(obj);

      if (obj->pIndexData->vnum == OBJ_VNUM_GENOCIDE_AXE)
      genocide_update(obj);
      if (obj->pIndexData->vnum == OBJ_VNUM_SKULL_NECROMANCY)
      skull_update(obj);
      if (obj->pIndexData->vnum == OBJ_VNUM_SENTIENT_SWORD)
      sentient_sword_update(obj);

      if ( obj->pIndexData->vnum == OBJ_VNUM_DOGGIE_TOY )
      doggie_toy_update(obj);

      if
      (
         obj->item_type == ITEM_PORTAL_NEW &&
         IS_SET(obj->value[4], PORTAL_MOONGATE)
      )
      {
         if
         (
            (
               obj->moonphases[0] == -1 ||
               moon_table[0].phase == obj->moonphases[0]
            ) &&
            (
               obj->moonphases[1] == -1 ||
               moon_table[1].phase == obj->moonphases[1]
            ) &&
            (
               obj->moonphases[2] == -1 ||
               moon_table[2].phase == obj->moonphases[2]
            ) &&
            IS_SET(obj->value[4], PORTAL_CLOSED)
         )
         {
            REMOVE_BIT(obj->value[4], PORTAL_CLOSED);
            if (obj->in_room != NULL && obj->in_room->people != NULL)
            {
               act(
                  "$p slowly fades into existance.",
                  obj->in_room->people,
                  obj,
                  NULL,
                  TO_ALL);
            }
         }
         else if (!IS_SET(obj->value[4], PORTAL_CLOSED))
         {
            SET_BIT(obj->value[4], PORTAL_CLOSED);
            if (obj->in_room != NULL && obj->in_room->people != NULL)
            {
               act(
                  "$p eerily fades away.",
                  obj->in_room->people,
                  obj,
                  NULL,
                  TO_ALL);
            }
         }
      }

      if
      (
         obj->item_type == ITEM_PORTAL_NEW &&
         IS_SET(obj->value[4], PORTAL_TIMEGATE) &&
         !IS_SET(obj->value[4], PORTAL_CLOSED) &&
         number_range(1, 3) == 1
      )
      {
         SET_BIT(obj->value[4], PORTAL_CLOSED);
      }

      if (obj->timer <= 0 || --obj->timer > 0)
      {
         continue;
      }

      /*        if (obj->pIndexData->vnum == OBJ_VNUM_SACK &&
      obj->carried_by != NULL &&
      !IS_NPC(obj->carried_by) &&
      obj->carried_by->pcdata->special == SUBCLASS_HUNTER)
      {
      if (obj->carried_by->fighting == NULL)
      {
      obj->timer = obj->level * 8;
      act( "Your $p begins to look frayed so you retan it.",
      obj->carried_by, obj, NULL, TO_CHAR );
      act( "$n retans $p to increase its life.", obj->carried_by,
      obj, NULL, TO_ROOM );
      continue;
      }
      else
      {
      obj->timer = 10;
      continue;
      }
      }
      */
      if
      (
         obj->item_type == ITEM_WEAPON &&
         obj->carried_by &&
         obj->wear_loc
      )
      {
         reslot = TRUE;
         owner = obj->carried_by;
      }
      else
      {
         reslot = FALSE;
         owner = NULL;
      }
      switch ( obj->item_type )
      {
         default:              message = "$p crumbles into dust.";         break;
         case ITEM_FOUNTAIN:   message = "$p dries up.";                   break;
         case ITEM_CORPSE_NPC: message = "$p decays into dust.";           break;
         case ITEM_CORPSE_PC:  message = "$p decays into dust.";           break;
         case ITEM_FOOD:       message = "$p decomposes.";                 break;
         case ITEM_POTION:     message = "$p has evaporated from disuse."; break;
         case ITEM_PORTAL_NEW: message = "$p collapses in on itself.";     break;

         /* When the timer on the 'Legion Rebel Pike' runs out, a new
         * pike will be created to simulate the flesh rotting off of
         * the impaled skull -- Wicket
         */
         case ITEM_TRASH:
         if (obj->pIndexData->vnum == OBJ_VNUM_EMPIRE_CRIM_PIKE)
         {
            OBJ_DATA *pike;

            pike = create_object( get_obj_index( 27 ), 0 );
            message = "The flesh upon the impaled skull rots, revealing the bone beneath.";
            pike->timer = number_range(9, 15);
            obj_to_room( pike, obj->in_room );
         }
         else
         message = "$p crumbles into dust.";
         break;
         /* End of Addition */

         case ITEM_CONTAINER:
         if (CAN_WEAR(obj, ITEM_WEAR_FLOAT))
         {
            if (obj->contains)
            message =
            "$p flickers and vanishes, spilling its contents on the floor.";
            else
            message = "$p flickers and vanishes.";
         }
         else
         message = "$p crumbles into dust.";
         break;
      }

      if ( obj->carried_by != NULL )
      {
         if (IS_NPC(obj->carried_by)
         &&  obj->carried_by->pIndexData->pShop != NULL)
         obj->carried_by->silver += obj->cost/5;
         else
         {
            act( message, obj->carried_by, obj, NULL, TO_CHAR );
            if ( obj->wear_loc == WEAR_FLOAT)
            act(message, obj->carried_by, obj, NULL, TO_ROOM);
         }
         if ( obj->wear_loc != WEAR_NONE )
         {
            rch=obj->carried_by;
            if (obj->pIndexData->vnum == 24418)
            remove_obj_girdle(rch, obj); else
            if (obj->pIndexData->vnum == 16022)
            remove_obj_troll_skin(rch, obj); else
            if (obj->pIndexData->vnum == OBJ_VNUM_SYMBOL_MAGIC)
            remove_obj_symbol_magic(rch, obj); else

            if (obj->pIndexData->vnum == 15123)
            remove_obj_channeling(rch, obj); else
            if (obj->pIndexData->vnum == OBJ_VNUM_WIZARDRY_1
            || obj->pIndexData->vnum == OBJ_VNUM_WIZARDRY_2)
            remove_obj_wizardry(rch, obj); else
            if (obj->pIndexData->vnum == OBJ_VNUM_TIARA)
            remove_obj_tiara(rch, obj); else
            if (obj->pIndexData->vnum == 19002)
            remove_obj_19002(rch, obj); else
            if (obj->pIndexData->vnum == 19445)
            remove_obj_bracers(rch, obj); else
            if (obj->pIndexData->vnum == 18792)
            remove_obj_elven_armguards(rch, obj); else
            if (obj->pIndexData->vnum == 23706)
            remove_obj_dragonmage_feet(rch, obj); else
            if (obj->pIndexData->vnum == 23751)
            remove_obj_blue_hide(rch, obj); else
            if (obj->pIndexData->vnum == 23752)
            remove_obj_green_hide(rch, obj); else
            if (obj->pIndexData->vnum == OBJ_VNUM_HEAVENLY_SCEPTRE)
            remove_obj_sceptre(rch, obj); else
            if (obj->pIndexData->vnum == 12161)
            remove_obj_whitehelm(rch, obj); else
            if (obj->pIndexData->vnum == 13711)
            remove_obj_fallen_wings(rch, obj); else
            if (obj->pIndexData->vnum == OBJ_VNUM_BLACK_HIDE) /* black dragon hide */
            remove_obj_black_hide(rch, obj); else
            if (obj->pIndexData->vnum == OBJ_VNUM_MISTY_ESSENCE) /* misty cloak */
            remove_obj_misty_cloak(rch, obj); else
            if (obj->pIndexData->vnum == OBJ_VNUM_CLOAK_UNDERWORLD) /* cloak of underworld */
            remove_obj_cloak_underworld(rch, obj); else
            if (obj->pIndexData->vnum == OBJ_VNUM_RED_DRAGONSTAR) /* red dragonstar */
            remove_obj_red_dragonstar(rch, obj); else
            if (obj->pIndexData->vnum == OBJ_VNUM_BLACK_DRAGONSTAR) /* black dragonstar */
            remove_obj_black_dragonstar(rch, obj); else
            if (obj->pIndexData->vnum == 14731) /* shadow dragon hide */
            remove_obj_shadow_hide(rch, obj); else
            if (obj->pIndexData->vnum == OBJ_VNUM_RING_PROTECT)
            remove_obj_ring_protection(rch, obj); else
            if (obj->pIndexData->vnum == OBJ_VNUM_ROBE_AVENGER)
            remove_obj_robe_avenger(rch, obj);
            unequip_char( rch, obj );
         }
      }
      else if ( obj->in_room != NULL
      &&      ( rch = obj->in_room->people ) != NULL )
      {
         if (! (obj->in_obj && obj->in_obj->pIndexData->vnum == OBJ_VNUM_PIT
         && !CAN_WEAR(obj->in_obj, ITEM_TAKE)))
         {
            act( message, rch, obj, NULL, TO_ROOM );
            act( message, rch, obj, NULL, TO_CHAR );
            if (obj->pIndexData->vnum == 24418 && is_affected(rch, gsn_strange_form))
            {
               affect_strip(rch, gsn_strange_form);
            }
         }
      }

      /* corpses in 1212 rot with their objects! */
      if (obj->item_type == ITEM_CORPSE_PC && obj->contains &&
      obj->in_room != NULL &&  IS_SET(obj->in_room->extra_room_flags, ROOM_1212))
      {
         OBJ_DATA *t_obj, *next_obj;
         for (t_obj = obj->contains; t_obj != NULL; t_obj = next_obj)
         {
            next_obj = t_obj->next_content;
            extract_obj(t_obj, FALSE);
         }
      }

      if (obj->item_type == ITEM_CORPSE_PC && obj->contains)
      {
         /* objects go to pit */
         OBJ_DATA *t_obj, *next_obj, *pit_obj;
         ROOM_INDEX_DATA *room;

         room = get_room_index(obj->value[4]);

         pit_obj = NULL;
         if (room != NULL && room->contents != NULL)
         {
            pit_obj = room->contents;
            while(pit_obj != NULL && !is_name("pit", pit_obj->name))
            pit_obj = pit_obj->next_content;
         }

         if (pit_obj == NULL)
         pit_obj = get_obj_type(get_obj_index(PIT_GLYNDANE_N));

         if (pit_obj == NULL){
            bug("NULL PIT: %d", obj->value[4]);
         }
         else
         {
            for (t_obj = obj->contains; t_obj != NULL; t_obj = next_obj)
            {
               next_obj = t_obj->next_content;
               obj_from_obj(t_obj);
               obj_to_obj(t_obj, pit_obj);
            }
         }
      }

      extract_obj( obj, FALSE );
      if (reslot && owner)
      {
         reslot_weapon(owner);
      }
   }
   if
   (
      (
         index = get_obj_index(OBJ_VNUM_SWORD_DEMONS)
      ) != NULL &&
      index->limcount < index->limtotal &&
      (
         obj = create_object(index, 0)
      ) != NULL
   )
   {
      int total = top_room;
      AREA_DATA* parea;
      int count;
      int choice;

      choice = number_range(1, total);
      count = 0;
      room = NULL;
      for (parea = area_first; parea; parea = parea->next)
      {
         for
         (
            room = parea->rooms;
            room;
            room = room->next_in_area
         )
         {
            if (++count == choice)
            {
               break;
            }
         }
         if (room)
         {
            break;
         }
      }
      if (room != NULL)
      {
         obj_to_room(obj, room);
      }
   }

   return;
}


void track_update( bool update_all )
{
   CHAR_DATA *tch, *fightme;
   CHAR_DATA *tch_next;
   char buf[MAX_STRING_LENGTH];

   for (tch = char_list; tch != NULL; tch = tch_next)
   {
      tch_next = tch->next;
      if
      (
         !IS_NPC(tch) ||
         tch->last_fought <= 0 ||
         !tch->on_line ||
         tch->fighting != NULL ||
         tch->position != POS_STANDING ||
         (
            !update_all &&
            !IS_SET(tch->off_flags, OFF_FAST_TRACK)
         ) ||
         is_affected(tch, gsn_power_word_stun)
      )
      {
         continue;
      }

      track_char(tch);
      fightme = id2name_room(tch->last_fought, tch->in_room, FALSE);
      if (fightme == NULL)
      {
         continue;
      }

      /* Sanity check */
      if (tch->in_room != fightme->in_room)
      {
         continue;
      }
      /* Malignus - guards dont track stealthed outlaws */
      if (is_affected(fightme, gsn_stealth))
      {
         if
         (
            tch->spec_fun == spec_guard ||
            tch->spec_fun == spec_executioner ||
            tch->spec_fun == spec_legion
         )
         {
            continue;
         }
      }
      if (is_safe(tch, fightme, IS_SAFE_SILENT))
      {
         continue;
      }
      if (is_affected(fightme, gsn_feign_death))
      {
         if
         (
            (
               get_skill(fightme, gsn_feign_death) / 3 +
               fightme->level -
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
            fightme,
            NULL,
            tch,
            TO_CHAR
         );
         affect_strip(fightme, gsn_feign_death);
      }
      if
      (
         IS_SET(tch->act2, ACT_RANGER) &&
         IS_AFFECTED(tch, AFF_CAMOUFLAGE) &&
         !can_see(fightme, tch) &&
         tch->fighting == NULL
      )
      {
         sprintf(buf, "\"%s\"", get_name(fightme, tch));
         do_ambush(tch, buf);
      }
      else if
      (
         can_see(tch, fightme) &&
         !IS_AFFECTED(tch, AFF_CALM)
      )
      {
         sprintf(buf, "%s, now you die!", PERS(fightme, tch));
         do_yell(tch, buf);
         multi_hit(tch, fightme, TYPE_UNDEFINED);
      }
   }
   return;
}


void riot_update( void )
{
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA *rioter;
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;
   CHAR_DATA *rioter_next;

   for (rioter = char_list; rioter != NULL; rioter = rioter_next)
   {
      rioter_next = rioter->next;

      if
      (
         !rioter->on_line ||
         (
            rioter->in_room == NULL &&
            (
               !IS_NPC(rioter) &&
               rioter->desc &&
               rioter->desc->connected != CON_PLAYING
            )
         )
      )
      {
         continue;
      }
      if ( IS_GUARDIAN_ANGEL(rioter) )
      {
         if ( IS_AFFECTED(rioter, AFF_SLEEP) )
         {
            REMOVE_BIT(rioter->affected_by, AFF_SLEEP);
            do_wake( rioter, "" );
         }

         if ( rioter->master && (rioter->master->in_room != rioter->in_room) )
         {
            act( "$n spreads $s wings and flies away.", rioter, NULL, NULL, TO_ROOM );
            stop_fighting( rioter, TRUE );
            char_from_room( rioter );
            char_to_room( rioter, rioter->master->in_room );
            act( "$n flies into the area and lands on $s feet, wings spread majestically.", rioter, NULL, NULL, TO_ROOM );
         }

         if ( (rioter->position != POS_FIGHTING) && (number_range(1, 20) == 1) )
         {
            if ( !IS_AFFECTED(rioter, AFF_SANCTUARY) )
            {
               do_cast( rioter, "sanctuary" );
            }
            else
            if ( (number_percent() < 30) && rioter->master && !IS_AFFECTED(rioter->master, AFF_SANCTUARY) )
            {
               char masterBuf[MAX_STRING_LENGTH];

               sprintf( masterBuf, "sanctuary %s", rioter->master->name );
               do_cast( rioter, masterBuf );
            }
            else
            if ( !IS_AFFECTED(rioter, AFF_HASTE) )
            {
               do_cast( rioter, "haste" );
            }
            else
            if ( rioter->master && (rioter->master->hit < rioter->master->max_hit) )
            {
               char masterBuf[MAX_STRING_LENGTH];

               sprintf( masterBuf, "'cure serious' %s", rioter->master->name );
               do_cast( rioter, masterBuf );
            }
            else
            if ( rioter->hit < rioter->max_hit )
            {
               do_cast( rioter, "'cure serious'" );
            }
         }
      }

      if (!is_affected(rioter, gsn_riot) || rioter->position != POS_STANDING)
      continue;
      for (vch = rioter->in_room->people; vch != NULL; vch = vch_next)
      {
         vch_next = vch->next_in_room;
         if (is_same_group(vch, rioter))
         continue;
         if (!can_see(rioter, vch)) continue;
         sprintf(buf, "Help! %s is rioting!", PERS(rioter, vch));
         do_myell(vch, buf);
         multi_hit(rioter, vch, TYPE_UNDEFINED);
         break;
      }
   }
   return;
}
/*
* Aggress.
*
* for each mortal PC
*     for each mob in room
*         aggress on some random PC
*
* This function takes 25% to 35% of ALL Merc cpu time.
* Unfortunately, checking on each PC move is too tricky,
*   because we don't the mob to just attack the first PC
*   who leads the party into the room.
*
* -- Furey
*/
/*
COMMENTED OUT by Fizzfaldt.
Replaced after every move.
void aggr_update( void )
{
   CHAR_DATA *wch;
   CHAR_DATA *wch_next;
   CHAR_DATA *ch;
   CHAR_DATA *ch_next;
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;
   CHAR_DATA *victim;
   return;
   for ( wch = char_list; wch != NULL; wch = wch_next )
   {
      wch_next = wch->next;
      if ( IS_NPC(wch)
      ||   wch->level >= LEVEL_IMMORTAL
      ||   wch->in_room == NULL
      ||   wch->in_room->area->empty)
      continue;

      for ( ch = wch->in_room->people; ch != NULL; ch = ch_next )
      {
         int count;

         ch_next    = ch->next_in_room;

         if ( !IS_NPC(ch)
         ||   !IS_SET(ch->act, ACT_AGGRESSIVE)
         ||   IS_SET(ch->in_room->room_flags, ROOM_SAFE)
         ||   IS_AFFECTED(ch, AFF_CALM)
         ||   ch->fighting != NULL
         ||   IS_AFFECTED(ch, AFF_CHARM)
         ||   !IS_AWAKE(ch)
         ||   ( IS_SET(ch->act, ACT_WIMPY) && IS_AWAKE(wch) )
         ||   !can_see( ch, wch )
         ||   number_bits(1) == 0)
         continue;

         / *
         * Ok we have a 'wch' player character and a 'ch' npc aggressor.
         * Now make the aggressor fight a RANDOM pc victim in the room,
         *   giving each 'vch' an equal chance of selection.
         * /
         count    = 0;
         victim    = NULL;
         for ( vch = wch->in_room->people; vch != NULL; vch = vch_next )
         {
            vch_next = vch->next_in_room;

            if ( !IS_NPC(vch)
            &&   vch->level < LEVEL_IMMORTAL
            &&   ch->level >= vch->level - 5
            &&   ( !IS_SET(ch->act, ACT_WIMPY) || !IS_AWAKE(vch) )
            &&   can_see( ch, vch ) )
            {
               if ( number_range( 0, count ) == 0 )
               victim = vch;
               count++;
            }
         }

         if ( victim == NULL )
         continue;

         multi_hit( ch, victim, TYPE_UNDEFINED );
      }
   }


   return;
}
*/

/* returns age in years */

int get_real_age(CHAR_DATA *ch)
{
   int age;
   int race;

   if (IS_NPC(ch))
   return 17;

   age = ((ch->played + (int) (current_time - ch->logon) ) / 5000);

   race = ch->race;
   if (race == grn_human)
   {
      age /= 6;   age += 17;
   }
   else if
   (
      race == grn_elf ||
      race == grn_grey_elf ||
      race == grn_dark_elf ||
      race == grn_arborian
   )
   {
      age += 70;
   }
   else if (race == grn_gnome)
   age += 80;
   else if
   (
      race == grn_dwarf ||
      race == grn_illithid ||
      race == grn_demon
   )
   {
      age += 40;
   }
   else if
   (
      race == grn_giant ||
      race == grn_troll ||
      race == grn_centaur ||
      race == grn_minotaur
   )
   {
      age /= 3;   age += 30;
   }
   else if (race == grn_draconian)
   {
      age /= 6;   age += 17;
   }
   else if (race == grn_ethereal)
   {
      age /= 6;   age += 18;
   }
   else if (race == grn_changeling)
   {
      age /= 6;   age += 15;
   }
   else if (race == grn_book)
   {
      return 2;
   }
   else
   {
      age /= 6;   age += 17;
   }

   return age;
}

int get_age(CHAR_DATA* ch)
{
   OBJ_DATA* brand = get_eq_char(ch, WEAR_BRAND);

   if
   (
      IS_NPC(ch) ||
      !brand ||
      brand->pIndexData->vnum != OBJ_VNUM_TWIG_BRAND
   )
   {
      return get_real_age(ch);
   }
   if (ch->race == grn_book)
   {
      return 2;
   }
   else
   {
      return 10 - ch->pcdata->brand_rank;
   }
}

/* returns death age in years  */
int get_death_age(CHAR_DATA *ch)
{
   int age, con;
   int race;
   OBJ_DATA *brand;

   if (IS_NPC(ch))
   return 1000;

   race = ch->race;
   con = ch->perm_stat[STAT_CON];
   age = 1000;

   if (race == grn_human)
   {
      age = (28 + 3*con);
   }
   else if
   (
      race == grn_elf ||
      race == grn_grey_elf ||
      race == grn_dark_elf ||
      race == grn_arborian
   )
   {
      age = (320 + 25*con);
   }
   else if (race == grn_gnome)
   {
      age = (230 + 22*con);
   }
   else if
   (
      race == grn_dwarf ||
      race == grn_illithid ||
      race == grn_demon
   )
   {
      age = (270 + 13*con);
   }
   else if
   (
      race == grn_giant ||
      race == grn_troll ||
      race == grn_centaur ||
      race == grn_minotaur
   )
   {
      age = (80 + 6*con);
   }
   else if (race == grn_draconian)
   {
      age = (45 + 3*con);
   }
   else if (race == grn_ethereal)
   {
      age = (47 + 3*con);
   }
   else if (race == grn_changeling)
   {
      age = (53 + 3*con);
   }
   else
   {
      age = (40 + 3*con);
   }
   age -= ch->pcdata->age_mod;

   if (IS_SET(ch->act2, PLR_LICH))
   {
      age += 1000;
   }

   brand = get_eq_char(ch, WEAR_BRAND);

   if
   (
      brand != NULL &&
      brand->pIndexData->vnum == 28903 &&
      !IS_SET(ch->act2, PLR_LICH)
   )
   {
      age = (age*3)/2;
   }

   return age;
}


/* returns the age name type */
char * get_age_name(CHAR_DATA *ch)
{
   char *name;
   int race;
   int age;
   OBJ_DATA *brand;

   if (IS_NPC(ch))
   return "young";

   brand = get_eq_char(ch, WEAR_BRAND);
   if
   (
      brand &&
      brand->pIndexData->vnum == OBJ_VNUM_TWIG_BRAND
   )
   {
      return "a child";
   }
   race = ch->race;
   age = get_age(ch);

   if (race == grn_human)
   {
      if (age <= 20)  name = "young";
      else if (age <= 29) name = "mature";
      else if (age <= 59) name = "middle aged";
      else if (age <= 69) name = "old";
      else    name = "very old";
   }
   else if
   (
      race == grn_elf ||
      race == grn_dark_elf ||
      race == grn_grey_elf ||
      race == grn_arborian
   )
   {
      if (age <= 100) name = "young";
      else if (age <= 240)    name = "mature";
      else if (age <= 550)    name = "middle aged";
      else if (age <= 650)    name = "old";
      else if (age <= 700)    name = "very old";
      else                    name = "ancient";
   }
   else if
   (
      race == grn_dwarf ||
      race == grn_illithid ||
      race == grn_demon
   )
   {
      if (age <= 68)  name = "young";
      else if (age <= 120) name = "mature";
      else if (age <= 400) name = "middle aged";
      else if (age <= 500) name = "old";
      else    name = "very old";
   }
   else if (race == grn_gnome)
   {
      if (age <= 140) name = "mature";
      else if (age <= 500)    name = "middle aged";
      else if (age <= 600)    name = "old";
      else if (age <= 700)    name = "very old";
      else                    name = "ancient";
   }
   else if
   (
      race == grn_giant ||
      race == grn_troll ||
      race == grn_centaur ||
      race == grn_minotaur
   )
   {
      if (age <= 60)  name = "young";
      else if (age <= 75) name = "mature";
      else if (age <= 150) name = "middle aged";
      else if (age <= 180) name = "old";
      else    name = "very old";
   }
   else if (race == grn_book)
   {
      name = "a child";
   }
   else
   {
      if (age <= 20)  name = "young";
      else if (age <= 29) name = "mature";
      else if (age <= 49) name = "middle aged";
      else if (age <= 69) name = "old";
      else    name = "very old";
   }

   return name;
}

void age_update_char(CHAR_DATA *ch)
{
   int age;
   int death_age;
   char strsave[MAX_STRING_LENGTH];

   if (ch == NULL)
   {
      return;
   }

   if (
         !ch->on_line ||
         IS_NPC(ch) ||
         IS_IMMORTAL(ch) ||
         is_affected(ch, gsn_resurrection)
      )
   {
      return;
   }

   if (ch->pcdata->death_status == HAS_DIED)
   {
      ch->pcdata->death_timer -= 1;
      if (ch->pcdata->death_timer > 0)
      {
         return;
      } else
      {
         act(
            "$n slowly fades away as $s soul departs the mortal planes.",
            ch,
            NULL,
            NULL,
            TO_ROOM);
         send_to_char(
            "Your soul finally departs the mortal planes.\n\r",
            ch);
         sprintf(strsave, "%s%s.plr", PLAYER_DIR, capitalize(ch->name));

         if (ch->perm_stat[STAT_CON] <= 3)
         {
            wiznet(
               "$N has finally died of con loss.",
               ch,
               NULL,
               WIZ_DEATHS,
               0,
               0);
         } else
         {
            wiznet(
               "$N has finally died of old age.",
               ch,
               NULL,
               WIZ_DEATHS,
               0,
               0);
         }

         ch->pause = 0;
         ch->quittime = 0;
         stop_fighting(ch, TRUE);
         do_permnameban(ch, ch->name);
         remove_from_rosters(ch);
         do_quit(ch, "");
         remove(strsave);
      }
      return;
   }

   if (ch->perm_stat[STAT_CON] > 3)
   {
      age = get_real_age(ch);
      death_age = get_death_age(ch);
      if (age < death_age)
      {
         return;
      }
      if (number_percent() > (age - death_age) * 5)
      {
         return;
      }

      act(
         "$n's heart suddenly gives out and $e keels over dead!",
         ch,
         NULL,
         NULL,
         TO_ROOM);
      send_to_char(
         "Your heart finally gives out from your hard life and you keel over"
         " dead!\n\r",
         ch);
      age_death(ch);
      ch->pcdata->death_timer = 150;
      ch->pcdata->death_status = HAS_DIED;
   }
   else
   {
      act(
         "$n's body crumples up into a lifeless husk.",
         ch,
         NULL,
         NULL,
         TO_ROOM);
      send_to_char("Your body crumples up into a useless husk.\n\r", ch);
      age_death(ch);
      ch->pcdata->death_timer = 150;
      ch->pcdata->death_status = HAS_DIED;
   }
}

void age_update(void)
{
   CHAR_DATA* ch;
   CHAR_DATA* ch_next;

   for (ch = char_list; ch != NULL; ch = ch_next)
   {
      ch_next = ch->next;
      age_update_char(ch);
   }
   return;
}

ROOM_INDEX_DATA * get_cloud_exit(void)
{
   ROOM_INDEX_DATA *proom = NULL;
   int num = 0;

   for (;;)
   {
      switch (number_range(1, 20))
      {
         case 1: num = number_range(8700, 8999); break;
         case 2: num = number_range(20000, 20031); break;
         case 3: num = number_range(10000, 10200); break;
         case 4: num = number_range(9200, 9299); break;
         case 5: num = number_range(29900, 29999); break;
         case 6: num = number_range(6000, 6199); break;
         case 7: num = number_range(201, 399); break;
         case 8: num = number_range(6800, 6899); break;
         case 9: num = number_range(27400, 27499); break;
         case 10: num = number_range(27700, 27899); break;
         case 11: num = number_range(9500, 9799); break;
         case 12: num = number_range(5200, 5399); break;
         case 13: num = number_range(26000, 26099); break;
         case 14: num = number_range(28100, 28199); break;
         case 15: num = number_range(19000, 19099); break;
         case 16: num = number_range(7800, 7899); break;
         case 17: num = number_range(22000, 22999); break;
         case 18: num = number_range(5500, 5599); break;
         case 19: num = number_range(600, 699); break;
         case 20: num = number_range(2400, 2699); break;
      }
      proom = get_room_index(num);
      if ((proom != NULL)
      && (proom->exit[DIR_UP] == NULL)
      && (proom->sector_type != SECT_UNDERGROUND)
      && (proom->sector_type != SECT_INSIDE))
      { break; }
   }

   return proom;
}

void room_update(void)
{
   ROOM_INDEX_DATA *room;
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;

   if (number_percent() < 50)
   {
      if ((room = get_room_index(27573)) == NULL)
      return;

      for ( vch = room->people; vch != NULL; vch = vch_next )
      {
         vch_next        = vch->next_in_room;
         if (IS_NPC(vch) &&
         (vch->pIndexData->vnum == 27527 ||
         vch->pIndexData->vnum == 27528 ||
         vch->pIndexData->vnum == 27529 ||
         vch->pIndexData->vnum == 27530 ||
         vch->pIndexData->vnum == 27535 ||
         vch->pIndexData->vnum == 27536))
         continue;
         send_to_char( "The earth trembles and shivers.\n\r", vch );
         if (IS_IMMORTAL(vch)) continue;
         if (IS_FLYING(vch))
         {
            send_to_char("You manage to evade the earthquake.\n\r", vch);
            damage(vch, vch, 0, gsn_bash, DAM_BASH, FALSE);
         }
         else
         {
            send_to_char("The earthquake strikes you to the ground!\n\r", vch);
            damage(vch, vch, number_range(1, 20)+dice(2, 8), gsn_bash, DAM_BASH, FALSE);
         }
      }
   }

   if (number_percent() < 50)
   {
      if ((room = get_room_index(27574)) == NULL)
      return;

      for ( vch = room->people; vch != NULL; vch = vch_next )
      {
         vch_next        = vch->next_in_room;
         if (IS_NPC(vch) &&
         (vch->pIndexData->vnum == 27527 ||
         vch->pIndexData->vnum == 27528 ||
         vch->pIndexData->vnum == 27529 ||
         vch->pIndexData->vnum == 27530 ||
         vch->pIndexData->vnum == 27535 ||
         vch->pIndexData->vnum == 27536))
         continue;
         send_to_char( "A great flood washes over you.\n\r", vch );
         if (IS_IMMORTAL(vch)) continue;
         send_to_char("The flood pulls you underwater, and you gasp for air!\n\r", vch);
         damage(vch, vch, number_range(1, 20)+dice(2, 8), gsn_bash, DAM_DROWNING, FALSE);
      }
   }

   if (number_percent() < 50)
   {
      if ((room = get_room_index(27575)) == NULL)
      return;

      for ( vch = room->people; vch != NULL; vch = vch_next )
      {
         vch_next        = vch->next_in_room;
         if (IS_NPC(vch) &&
         (vch->pIndexData->vnum == 27527 ||
         vch->pIndexData->vnum == 27528 ||
         vch->pIndexData->vnum == 27529 ||
         vch->pIndexData->vnum == 27530 ||
         vch->pIndexData->vnum == 27535 ||
         vch->pIndexData->vnum == 27536))
         continue;
         send_to_char( "A tornado blows through the room.\n\r", vch );
         if (IS_IMMORTAL(vch)) continue;
         if (IS_FLYING(vch))
         {
            send_to_char("You are flung about uncontrollably!\n\r", vch);
            damage(vch, vch, number_range(20, 40)+dice(4, 8), gsn_bash, DAM_BASH, FALSE);
         }
         else
         {
            send_to_char("The high winds whip you into the walls!\n\r", vch);
            damage(vch, vch, number_range(1, 20)+dice(2, 8), gsn_bash, DAM_BASH, FALSE);
         }
      }
   }

   if (number_percent() < 50)
   {
      if ((room = get_room_index(27576)) == NULL)
      return;

      for ( vch = room->people; vch != NULL; vch = vch_next )
      {
         vch_next        = vch->next_in_room;
         if (IS_NPC(vch) &&
         (vch->pIndexData->vnum == 27527 ||
         vch->pIndexData->vnum == 27528 ||
         vch->pIndexData->vnum == 27529 ||
         vch->pIndexData->vnum == 27530 ||
         vch->pIndexData->vnum == 27535 ||
         vch->pIndexData->vnum == 27536))
         continue;
         send_to_char( "The room explodes in searing flame.\n\r", vch );
         if (IS_IMMORTAL(vch)) continue;
         send_to_char("The fire burns you!\n\r", vch);
         damage(vch, vch, number_range(1, 20)+dice(2, 8), gsn_bash, DAM_FIRE, FALSE);
      }
   }

   if (number_percent() < 4)
   {
      if ((room = get_room_index(2731)) == NULL)
      return;

      for ( vch = room->people; vch != NULL; vch = vch_next )
      {
         vch_next        = vch->next_in_room;
         if (IS_NPC(vch))
         continue;
         if (IS_IMMORTAL(vch)) continue;
         send_to_char( "The forge's heat stifles you.\n\r", vch );
         damage(vch, vch, number_range(1, 10)+dice(3, 2), gsn_bash, DAM_FIRE, FALSE);
      }
   }

   return;
}

void ancient_update(void)
{
   ROOM_INDEX_DATA *room, *pexit;
   static int ancientticker = -1;

   /* temporarily ancient only movs once per reboot so the -- on the tiemr is
   gone- wervdon */

   if (ancientticker >= 0)
   return;

   ancientticker = number_range(80, 120);
   pexit = get_room_index(24167);

   if ((room = get_room_index(3801)) == NULL)
   return;

   switch(number_range(1, 5)){
      case 1: pexit = get_room_index(10032); break;
      case 2: pexit = get_room_index(29412); break;
      case 3: pexit = get_room_index(29918); break;
      case 4: pexit = get_room_index(24617); break;
      case 5: pexit = get_room_index(17301); break;
   }
   room = room->exit[DIR_UP]->u1.to_room;
   pexit->exit[DIR_DOWN] = room->exit[DIR_DOWN];
   room->exit[DIR_DOWN] = NULL;
   room = get_room_index(3801);

   room->exit[DIR_UP]->u1.vnum = pexit->vnum;
   room->exit[DIR_UP]->u1.to_room = pexit;
   room->exit[DIR_UP]->exit_info = 0;
   room->exit[DIR_UP]->key = 0;
   if (room->exit[DIR_UP]->keyword != NULL)
   free_string(room->exit[DIR_UP]->keyword);
   if (room->exit[DIR_UP]->description != NULL)
   free_string(room->exit[DIR_UP]->description);

   return;
}


void cloudcty_update(void)
{
   ROOM_INDEX_DATA *room, *pexit;
   static int cloudctyticker = 1;


   /* is test is determinined in comm.c (main function) by the port number,
   if the game is using port 2222, is_test is false, otherwise its
   true.   This is so test can use less areas */

   if (is_test)
   return;
   if (--cloudctyticker >= 0)
   return;

   cloudctyticker = number_range(4, 7);

   if ((room = get_room_index(28001)) == NULL)
   return;

   room = room->exit[DIR_DOWN]->u1.to_room;
   pexit = get_cloud_exit();
   pexit->exit[DIR_UP] = room->exit[DIR_UP];
   room->exit[DIR_UP] = NULL;
   room = get_room_index(28001);
   room->exit[DIR_DOWN]->u1.vnum = pexit->vnum;
   room->exit[DIR_DOWN]->u1.to_room = pexit;
   room->exit[DIR_DOWN]->exit_info = 0;
   room->exit[DIR_DOWN]->key = 0;
   if (room->exit[DIR_DOWN]->keyword != NULL)
   free_string(room->exit[DIR_DOWN]->keyword);
   if (room->exit[DIR_DOWN]->description != NULL)
   free_string(room->exit[DIR_DOWN]->description);

   room = get_room_index(28012);
   room = room->exit[DIR_DOWN]->u1.to_room;
   pexit = get_cloud_exit();
   pexit->exit[DIR_UP] = room->exit[DIR_UP];
   room->exit[DIR_UP] = NULL;
   room = get_room_index(28012);
   room->exit[DIR_DOWN]->u1.vnum = pexit->vnum;
   room->exit[DIR_DOWN]->u1.to_room = pexit;
   room->exit[DIR_DOWN]->exit_info = 0;
   room->exit[DIR_DOWN]->key = 0;
   if (room->exit[DIR_DOWN]->keyword != NULL)
   free_string(room->exit[DIR_DOWN]->keyword);
   if (room->exit[DIR_DOWN]->description != NULL)
   free_string(room->exit[DIR_DOWN]->description);

   room = get_room_index(28016);
   room = room->exit[DIR_DOWN]->u1.to_room;
   pexit = get_cloud_exit();
   pexit->exit[DIR_UP] = room->exit[DIR_UP];
   room->exit[DIR_UP] = NULL;
   room = get_room_index(28016);
   room->exit[DIR_DOWN]->u1.vnum = pexit->vnum;
   room->exit[DIR_DOWN]->u1.to_room = pexit;
   room->exit[DIR_DOWN]->exit_info = 0;
   room->exit[DIR_DOWN]->key = 0;
   if (room->exit[DIR_DOWN]->keyword != NULL)
   free_string(room->exit[DIR_DOWN]->keyword);
   if (room->exit[DIR_DOWN]->description != NULL)
   free_string(room->exit[DIR_DOWN]->description);

   room = get_room_index(28036);
   room = room->exit[DIR_DOWN]->u1.to_room;
   pexit = get_cloud_exit();
   pexit->exit[DIR_UP] = room->exit[DIR_UP];
   room->exit[DIR_UP] = NULL;
   room = get_room_index(28036);
   room->exit[DIR_DOWN]->u1.vnum = pexit->vnum;
   room->exit[DIR_DOWN]->u1.to_room = pexit;
   room->exit[DIR_DOWN]->exit_info = 0;
   room->exit[DIR_DOWN]->key = 0;
   if (room->exit[DIR_DOWN]->keyword != NULL)
   free_string(room->exit[DIR_DOWN]->keyword);
   if (room->exit[DIR_DOWN]->description != NULL)
   free_string(room->exit[DIR_DOWN]->description);

   return;
}

void reboot_update()
{
   extern bool merc_down;
   DESCRIPTOR_DATA* d;
   DESCRIPTOR_DATA* d_next;
   CHAR_DATA* vch;
   CHAR_DATA* vch_next;
   struct tm* reboot_time = localtime(&current_time);
   static int lastmin;
   int FirstMin = 55;

   if
   (
      reboot_ok &&
      reboot_time->tm_hour == autoreboot_warning_hour &&
      (
         reboot_time->tm_min >= FirstMin &&
         reboot_time->tm_min <= FirstMin + 4
      ) &&
      lastmin != reboot_time->tm_min
   )
   {
      for ( d = descriptor_list; d; d = d->next )
      {
         if ( d->connected == CON_PLAYING )
         {
            if (reboot_time->tm_min == FirstMin)
            {
               send_to_char
               (
                  "*** FIVE MINUTES TO AUTOREBOOT ***\n\r",
                  d->character
               );
               vch = d->original ? d->original : d->character;
               if
               (
                  vch != NULL &&
                  d->connected == CON_PLAYING
               )
               {
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
               }
            }
            else if (reboot_time->tm_min == FirstMin + 1)
            {
               send_to_char
               (
                  "*** FOUR MINUTES TO AUTOREBOOT ***\n\r",
                  d->character
               );
            }
            else if (reboot_time->tm_min == FirstMin + 2)
            {
               send_to_char
               (
                  "*** THREE MINUTES TO AUTOREBOOT ***\n\r",
                  d->character
               );
            }
            else if (reboot_time->tm_min == FirstMin + 3)
            {
               send_to_char
               (
                  "*** TWO MINUTES TO AUTOREBOOT ***\n\r",
                  d->character
               );
            }
            else if (reboot_time->tm_min == FirstMin + 4)
            {
               send_to_char
               (
                  "*** ONE MINUTE TO AUTOREBOOT ***\n\r",
                  d->character
               );
            }
         }
      }
   }
   else if
   (
      reboot_time->tm_hour == autoreboot_hour &&
      reboot_time->tm_min == 0 &&
      lastmin != reboot_time->tm_min
   )
   {
      if (reboot_ok)
      {
         merc_down = TRUE;
         sprintf(log_buf, "AutoReboot initiated.");

         save_globals();
         log_string(log_buf);
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
            save_char_obj(vch);
            send_to_char("*** You are now saved. ***\n\r", vch);
            send_to_char("*** REBOOTING! ***\n\r", vch);
         }
         for ( d = descriptor_list; d != NULL; d = d_next )
         {
            d_next = d->next;
            close_socket(d);
         }
      }
      else
      {
         for (d = descriptor_list; d; d = d->next)
         {
            if
            (
               d->connected == CON_PLAYING &&
               IS_IMMORTAL(d->character)
            )
            {
               send_to_char
               (
                  "*** AUTOREBOOT SKIPPED ***\n\r",
                  d->character
               );
            }
         }
         reboot_ok = TRUE;
      }
   }
   lastmin = reboot_time->tm_min;
   return;
}

LIST_DATA* value_1_list;

/*
* Handle all kinds of updates.
* Called once per pulse from game loop.
* Random times to defeat tick-timing clients and players.
*/
void update_handler( void )
{
   DESCRIPTOR_DATA *d;
   OBJ_DATA* obj;
   NODE_DATA* node;
   static int     pulse_mobile;
   static int     pulse_violence;
   static int     pulse_riot;
   static int     pulse_message;
   static int     wizireport_timer;
   /*
      Try to get these to not line up
      (handle things spread over pulses instead of
      all at once)
   */
   extern int     pulse_point;  /* = PULSE_TICK + 1 */
   static int     pulse_area = PULSE_AREA + 2;
   static int     pulse_track = PULSE_TRACK + 3;
   static int     pulse_fasttrack = PULSE_TRACK / 5;

   reboot_update();

   /* Can probally be replaced with messages, just for portal timers */
   for (node = value_1_list->first; node; node = node->next)
   {
      obj = (OBJ_DATA*)node->data;
      if (obj->value[1] > 0)
      {
         obj->value[1]--;
      }
   }

   if ( --pulse_area     <= 0 )
   {
      pulse_area    = PULSE_AREA;
      /* number_range( PULSE_AREA / 2, 3 * PULSE_AREA / 2 ); */
      area_update    ( );
   }

   if ( --pulse_track <= 0)
   {
      pulse_track = PULSE_TRACK;
      track_update    (TRUE);
   }
   if ( --pulse_fasttrack <=0)
   {
      pulse_fasttrack = PULSE_TRACK/5;
      track_update    (FALSE);
   }
   if ( --pulse_riot <= 0)
   {
      pulse_riot = PULSE_RIOT;
      riot_update    ( );
   }

   if ( --pulse_mobile   <= 0 )
   {
      pulse_mobile    = PULSE_MOBILE;
      mobile_update    ( );
   }

   if ( --pulse_violence <= 0 )
   {
      pulse_violence    = PULSE_VIOLENCE;
      violence_update    ( );
   }

   if ( --pulse_message <= 0 )
   {
      pulse_message = PULSE_MESSAGE;
      message_poll_pulse();
   }


   if ( --pulse_point    <= 0 )
   {
      /* to count the number of ticks house items are down */
      int cnt;
      for (cnt = 0; cnt < MAX_HOUSE; cnt++)
      {
         house_down(NULL, cnt);
      }
      wiznet("TICK!", NULL, NULL, WIZ_TICKS, 0, 0);
      pulse_point     = PULSE_TICK;
      /* number_range( PULSE_TICK / 2, 3 * PULSE_TICK / 2 ); */
      weather_update    ( FALSE );
      char_update    ( );
      obj_update    ( );
      room_aff_update    ( );
      age_update    ( );
      cloudcty_update ( );
      /*        ancient_update  ( ); ancient no long moves */
      room_update     ( );
      message_poll_tick();
      moon_update();
      for ( d = descriptor_list; d != NULL; d = d->next )
      {
         if (d->connected == CON_PLAYING
         &&  !IS_NPC(d->character)
         &&  d->character->pcdata->special == SUBCLASS_TEMPORAL_MASTER
         &&  IS_SET(d->character->wiznet, WIZ_TICKS))
         {
            send_to_char("You sense the passage of time.\n\r", d->character);
         }
      }
      update_wizireport();  /* Update status for everyone every tick */
      if (--wizireport_timer <= 0)
      {
         wizireport_timer = 45;  /* save status every 45 ticks (half hour) */
         save_wizireport();
      }

   }

   /*
   Commented out by Fizzfaldt, replaced by cleaner function.
   aggr_update( );
   */
   tail_chain( );
   return;
}

void sentient_sword_update(OBJ_DATA *obj)
{
   int which_one = number_percent();

   if (which_one < 25 || obj->carried_by == NULL)
   return;

   if (which_one < 50){
      do_recho(obj->carried_by,
      "The sentient sword says '{B{6I thirst for blood, Master!{n'");
      return;
   }
   if (which_one < 75){
      do_recho(obj->carried_by,
      "The sentient sword says '{B{6Lead me to battle, Master!{n'");
      return;
   }

   do_recho(obj->carried_by,
   "The sentient sword says '{B{6Our foes will fall to our might, Master!{n'");
   return;
}

void skull_update(OBJ_DATA *obj)
{
   if (number_percent() > 5) return;
   if (obj->wear_loc == WEAR_NONE) return;
   if (obj->carried_by == NULL) return;
   if (number_range(1, 2) == 1)
   {
      do_recho(obj->carried_by, "The skull of necromancy comes to life and says '{B{6Elghinn plynnza naun popyl.{n'");
   }
   else
   {
      do_recho(obj->carried_by, "The skull of necromancy comes to life and says '{B{6Elghinn zhah nau mzild.{n'");
   }
   return;
}

void genocide_update(OBJ_DATA *obj)
{
   CHAR_DATA *victim;

   if (obj->carried_by == NULL) return;
   if (obj->wear_loc == WEAR_NONE) return;
   if (IS_NPC(obj->carried_by)) return;

   victim = obj->carried_by->in_room->people;
   while (victim != NULL)
   {
      if (IS_IMMORTAL(victim))
      {
         victim = victim->next_in_room;
         continue;
      }
      if
      (
         victim != obj->carried_by &&
         !is_safe(victim, obj->carried_by, IS_SAFE_SILENT) &&
         number_bits(1)
      )
      {
         break;
      }
      victim = victim->next_in_room;
   }
   if (victim == NULL) return;

   act_color("$p tells you '{B{2$N must die, slay $M now.{n'", obj->carried_by, obj, victim, TO_CHAR);
   return;
}

void sword_demon_update(OBJ_DATA *obj)
{
   CHAR_DATA* ch;

   if (number_percent() > 60)
   return;

   if (obj->carried_by == NULL)
   return;

   ch = obj->carried_by;

   if (obj->carried_by != NULL)
   {
      if (obj->wear_loc == WEAR_NONE)
      {
         /* carried but not wielded */
         if (!IS_SET(obj->carried_by->comm, COMM_ANSI)){
            act( "$p tells you 'Wear me and power will be yours!'",
            obj->carried_by, obj, NULL, TO_CHAR );
         }
         else
         {
            act( "$p tells you '\x01b[1;32mWear me and power will be yours!\x01b[0;37m'", obj->carried_by, obj, NULL, TO_CHAR);
         }
      }
      else
      {
         /* ought oh, it's wielded - bad stuff here */
         act("The demons within $p torture you.", obj->carried_by, obj, NULL, TO_CHAR);

         switch(number_range(0, 4)){
            case(0):
            magic_spell
            (
               ch,
               "blah self",
               CAST_BITS_PLR_SKILL,
               obj->level + 4,
               gsn_curse
            );
            break;
            case(1):
            magic_spell
            (
               ch,
               "blah self",
               CAST_BITS_PLR_SKILL,
               obj->level,
               gsn_demonfire
            );
            break;
            case(2):
            magic_spell
            (
               ch,
               "blah self",
               CAST_BITS_PLR_SKILL,
               obj->level + 6,
               gsn_blindness
            );
            break;
            case(3):
            magic_spell
            (
               ch,
               "blah self",
               CAST_BITS_PLR_SKILL,
               obj->level + 4,
               gsn_plague
            );
            break;
            case(4):
            magic_spell
            (
               ch,
               "blah self",
               CAST_BITS_PLR_SKILL,
               obj->level + 3,
               gsn_poison
            );
            break;
         }
         act( "You hear evil laughter from $p.", obj->carried_by, obj, NULL, TO_ROOM );
         act( "You hear evil laughter from $p.", obj->carried_by, obj, NULL, TO_CHAR );
      }
   }

   return;
}


bool is_odd_hour(CHAR_DATA *ch)
{
   AFFECT_DATA *paf;
   AFFECT_DATA *paf_next;
   int temp = 0;

   for ( paf = ch->affected; paf != NULL; paf = paf_next )
   {
      paf_next    = paf->next;
      if (paf->type == gsn_distort_time)
      temp = paf->duration;
   }
   if (temp%2)
   return TRUE;
   else
   return FALSE;
}

void doggie_toy_run( OBJ_DATA *toy )
{
   ROOM_INDEX_DATA *was_in;
   sh_int attempt;

   if ( !(was_in=toy->in_room) )
   {
      return;
   }

   for ( attempt = 0; attempt < 6; attempt++ )
   {
      EXIT_DATA *pexit;
      int door;

      door = number_door( );
      if ( (pexit = was_in->exit[door]) == NULL
      || pexit->u1.to_room == NULL
      || IS_SET(pexit->exit_info, EX_CLOSED) )
      continue;

      if ( was_in->people )
      act( "The doggie toy scampers away!", was_in->people, NULL, NULL, TO_ALL );

      obj_from_room( toy );
      obj_to_room( toy, pexit->u1.to_room );

      if ( toy->in_room->people )
      act( "The doggie toy scampers into the room!", toy->in_room->people, NULL, NULL, TO_ALL );
      break;
   }
}

void doggie_toy_update( OBJ_DATA *toy )
{
   CHAR_DATA* rch;

   if ( toy->wear_loc == WEAR_HOLD
   && (rch = toy->carried_by) != NULL
   && !IS_NPC(rch)
   && rch->in_room != NULL )
   {
      if ( number_percent() < 20 )
      {
         MESSAGE run;
         run.message_type = OM_DOGGIE_TOY_RUN;
         run.message_data = 9;
         run.message_string = NULL;
         run.message_flags = MESSAGE_POLL_PULSE | MESSAGE_UNIQUE;

         send_to_char("Your doggie toy squirms out of your grasp!\n\r", rch);
         obj_from_char( toy );
         obj_to_room( toy, rch->in_room );
         message_to_obj( toy, &run );
         doggie_toy_run( toy );
      } else
      if ( number_percent() < 3 )
      {
         send_to_char("Your doggie toy yips happily!\n\r", rch);
         obj_cast_spell(gsn_heal, toy->level, rch, rch, toy);
      }
   }
}

void message_poll_tick( void )
{
   MESSAGE* message;
   MESSAGE* message_next;

   for ( message = message_tick_list; message; message = message_next )
   {
      message_next = message->next_to_poll;

      switch( message->target_type )
      {
         case MESSAGE_TARGET_OBJECT:
         switch( message->message_type )
         {
            default:
            continue;
         }
         case MESSAGE_TARGET_CHAR:
         if
         (
            message->target != NULL &&
            !(
               (CHAR_DATA*)message->target
            )->on_line
         )
         {
            continue;
         }
         switch( message->message_type )
         {
            case MM_HOUSE_ITEM_TAKEN:
            (message->message_data)++;
            break;
            default:
            continue;
         }
         case MESSAGE_TARGET_ROOM:
         switch( message->message_type )
         {
            default:
            continue;
         }
         default:
         continue;
      }
   }
}

void message_poll_pulse( void )
{
   MESSAGE* message;
   MESSAGE* message_next;

   for ( message = message_pulse_list; message; message = message_next )
   {
      message_next = message->next_to_poll;

      switch( message->target_type )
      {
         case MESSAGE_TARGET_OBJECT:
         switch( message->message_type )
         {
            case OM_DOGGIE_TOY_RUN:
            obj_message_doggie_toy_run(message);
            break;
            default:
            message->time_to_live--;
            if (message->time_to_live == 0)
            message_remove(message);
            continue;
         }
         case MESSAGE_TARGET_CHAR:
         if
         (
            message->target != NULL &&
            !(
               (CHAR_DATA*)message->target
            )->on_line
         )
         {
            continue;
         }
         switch( message->message_type )
         {
            default:
            message->time_to_live--;
            if (message->time_to_live <= 0)
            message_remove(message);
            continue;
         }
         case MESSAGE_TARGET_ROOM:
         switch( message->message_type )
         {
            default:
            message->time_to_live--;
            if (message->time_to_live <= 0)
            message_remove(message);
            continue;
         }
         default:
         continue;
      }
   }
}

void obj_message_doggie_toy_run( MESSAGE* run )
{
   if ( number_percent() < 50 )
   return;

   if ( !run )
   return;

   if ( ((OBJ_DATA *)run->target)->carried_by
   || !(((OBJ_DATA *)run->target)->in_room) )
   {
      message_remove( run );
      return;
   }

   doggie_toy_run( (OBJ_DATA *)run->target );

   if ( --(run->message_data) < 1 )
   message_remove( run );

   return;
}

/* go through all room_affects and update them */
void room_aff_update(void)
{
   ROOM_AFFECT_DATA *paf;
   ROOM_AFFECT_DATA *paf_next;
   CHAR_DATA * rch;
   if (first_room_affect == NULL)
   return;
   for ( paf = first_room_affect; paf != NULL; paf = paf_next )
   {
      paf_next    = paf->next_global;
      if ( paf->duration > 0 )
      {
         paf->duration--;
         if (number_range(0, 4) == 0 && paf->level > 0)
         paf->level--;  /* spell strength fades with time */
      }
      else if ( paf->duration < 0 )
      ;
      else
      {
         if ( paf->type > 0 && skill_table[paf->type].msg_obj )
         {
            if (paf->room->people != NULL)
            {
               rch = paf->room->people;
               act(skill_table[paf->type].msg_obj,
               rch, NULL, NULL, TO_ALL);
            }
         }
         affect_remove_room( paf->room, paf );
      }
   }

}

void moon_update()
{
   sh_int counter;
   int hours;
   int days;
   int hours_into;

   days = (current_mud_time.day) +
   (current_mud_time.week  * DAYS_PER_WEEK) +
   (current_mud_time.month * DAYS_PER_MONTH) +
   (current_mud_time.year  * DAYS_PER_YEAR);
   for (counter = 0; counter < MAX_MOONS; counter++)
   {
      hours = days * HOURS_PER_DAY + current_mud_time.hour;
      hours_into = hours % moon_table[counter].cycle_length;
      moon_table[counter].phase = hours_into * MAX_MOON_PHASES /
      moon_table[counter].cycle_length;
   }
   tail_chain();
   return;
}

bool imm_visible(CHAR_DATA* ch)
{
   return
   (
      ch->invis_level == 0 &&
      ch->incog_level == 0
   );
}

void update_wizireport_time(CHAR_DATA* ch, WORSHIP_TYPE* worship, int type)
{
   worship->wiz_info.level = ch->level;
   worship->wiz_info.trust = get_trust(ch);
   if
   (
      ch->desc != NULL &&
      ch->desc->connected == CON_PLAYING
   )
   {
      worship->wiz_info.login = current_time;
   }
   if (worship->history.total_start == 0)
   {
      worship->history.last_change = current_time;
      worship->history.start = current_time;
      worship->history.played = 0;
      worship->history.vis = 0;
      worship->history.total_start = current_time;
      worship->history.total_played = 0;
      worship->history.total_vis = 0;
      return;
   }
   if
   (
      type == WIZI_UPDATE &&
      ch->desc != NULL &&
      ch->desc->connected == CON_PLAYING
   )
   {
      worship->history.played += current_time - worship->history.last_change;
      worship->history.total_played +=
      (
         current_time - worship->history.last_change
      );
      if (imm_visible(ch))
      {
         worship->history.vis += current_time - worship->history.last_change;
         worship->history.total_vis +=
         (
            current_time - worship->history.last_change
         );
      }
   }
   if
   (
      type == WIZI_LOGON ||
      type == WIZI_UPDATE
   )
   {
      worship->history.last_change = current_time;
   }
}

void do_wizireport_update(CHAR_DATA* ch, int type)
{
   if
   (
      ch == NULL ||
      IS_NPC(ch) ||
      !IS_IMMORTAL(ch) ||
      !ch->on_line
   )
   {
      return;
   }
   update_wizireport_time
   (
      ch,
      find_wizi_slot(ch->name, &ch->pcdata->worship_slot),
      type
   );
}

void update_wizireport()
{
   CHAR_DATA* ch;
   CHAR_DATA* ch_next;

   for (ch = char_list; ch; ch = ch_next)
   {
      ch_next = ch->next;
      do_wizireport_update(ch, WIZI_UPDATE);
   }
}

bool stray_update(CHAR_DATA* stray)
{
   CHAR_DATA* second;
   CHAR_DATA* second_next;
   int percent;
   bool retval = FALSE;
   char buf[MAX_INPUT_LENGTH];

   percent = 100 - (stray->hit * 100 / stray->max_hit);
   percent /= 2;
   if (IS_SET(stray->act2, ACT_NICE_STRAY))
   {
      percent -= 10;
   }
   if
   (
      number_percent() < percent &&
      !is_affected(stray, gsn_create_food) &&
      !is_affected(stray, gsn_create_food)
   )
   {
      stop_follower(stray);
      act("$n goes off in search of food.", stray, NULL, NULL, TO_ROOM);
      return TRUE;
   }
   if (IS_SET(stray->act2, ACT_NICE_STRAY))
   {
      percent += 20;  /**/
   }
   if
   (
      number_percent() < percent + 10 &&
      number_bits(1) &&
      !is_affected(stray, gsn_create_food)
   )
   {
      act("$n sniffs about, looking for food.", stray, NULL, NULL, TO_ROOM);
      retval = TRUE;
   }
   if
   (
      number_percent() < percent + 10 &&
      number_bits(1) &&
      !is_affected(stray, gsn_create_water)
   )
   {
      act("$n sniffs about, looking for water.", stray, NULL, NULL, TO_ROOM);
      retval = TRUE;
   }

   for (second = char_list; second; second = second_next)
   {
      second_next = second->next;
      if
      (
         second == stray ||
         !IS_NPC(second) ||
         second->in_room != stray->in_room ||
         !IS_AFFECTED(second, AFF_CHARM) ||
         second->master == NULL ||
         second->master != stray->master ||
         !IS_SET(second->act2, ACT_STRAY)
      )
      {
         continue;
      }
      if
      (
         number_bits(2) == 0 ||
         (
            stray->hit < 9 * stray->max_hit / 10 &&
            number_bits(1) != 0
         ) ||
         (
            stray->hit < 8 * stray->max_hit / 10 &&
            number_bits(2) != 0
         ) ||
         (
            stray->hit < 7 * stray->max_hit / 10 &&
            number_bits(3) != 0
         )
      )
      {
         if (IS_SET(stray->act2, ACT_NICE_STRAY))
         {
            act
            (
               "$n walks off, dejected.",
               stray,
               NULL,
               NULL,
               TO_ROOM
            );
            stop_follower(stray);
         }
         else
         {
            act
            (
               "$n strikes at $N out of jealousy!",
               stray,
               NULL,
               second,
               TO_NOTVICT
            );
            stray->fighting = NULL;
            second->fighting = NULL;
            if (number_bits(1))
            {
               stop_follower(stray);
            }
            else
            {
               stop_follower(second);
            }
            sprintf(buf, "\"%s\"", get_name(second, stray));
            do_bite(stray, buf);
            if (second->valid)
            {
               if (IS_SET(stray->act2, ACT_NICE_STRAY))
               {
                  if (number_percent() > 70)
                  {
                     act
                     (
                        "$n runs away from $N!",
                        second,
                        NULL,
                        stray,
                        TO_NOTVICT
                     );
                  }
               }
               else
               {
                  act
                  (
                     "$n strikes back at $N!",
                     second,
                     NULL,
                     stray,
                     TO_NOTVICT
                  );
                  sprintf(buf, "\"%s\"", get_name(stray, second));
                  do_bite(second, buf);
               }
            }
            if (stray->valid == FALSE)
            {
               return FALSE;
            }
         }
         return TRUE;
      }
   }
   if
   (
      stray->in_room != stray->master->in_room &&
      number_bits(2)
   )
   {
      stop_follower(stray);
      act("$n sniffs about, looking for food.", stray, NULL, NULL, TO_ROOM);
      retval = TRUE;
   }
   return retval;
}
