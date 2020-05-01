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

static const char rcsid[] = "$Id: skills.c,v 1.21 2004/09/08 15:37:02 xurinos Exp $";

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
#include "recycle.h"

/* command procedures needed */
DECLARE_DO_FUN(do_groups    );
DECLARE_DO_FUN(do_help      );
DECLARE_DO_FUN(do_say       );

int check_weapons(CHAR_DATA *ch, int sn);

bool has_skill(CHAR_DATA *ch, int sn)
{
   if
   (
      sn < 1 ||
      sn > MAX_SKILL
   )
   {
      sprintf
      (
         log_buf,
         "has_skill invalid sn: %d for %s",
         sn,
         ch->name
      );
      bug_trust(log_buf, 0, get_trust(ch));
      return FALSE;
   }
   if (IS_NPC(ch))
   {
      if (get_skill(ch, sn) > 0)
      {
         return TRUE;
      }
      return FALSE;
   }
   if (ch->pcdata->learnlvl[sn] <= 0)
   {
      sprintf
      (
         log_buf,
         "%s has learnlvl %d for skill %s: %d",
         ch->name,
         ch->pcdata->learnlvl[sn],
         skill_table[sn].name,
         sn
      );
      bug_trust(log_buf, 0, get_trust(ch));
      return FALSE;
   }
   if (ch->level < ch->pcdata->learnlvl[sn])
   {
      return FALSE;
   }
   if (ch->pcdata->learned[sn] < 1)
   {
      return FALSE;
   }
   return TRUE;
}

/* used to get new skills */
void do_gain(CHAR_DATA *ch, char *argument)
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *trainer;
   if (IS_NPC(ch))
   return;

   /* find a trainer */
   for ( trainer = ch->in_room->people;
   trainer != NULL;
   trainer = trainer->next_in_room)
   if ((IS_NPC(trainer) && (IS_SET(trainer->act, ACT_GAIN)
   || IS_SET(trainer->act, ACT_TRAIN)) ) ||
   (!IS_NPC(trainer) && IS_SET(trainer->act2, PLR_GUILDMASTER)
   && (trainer->class == ch->class)))
   break;

   if (trainer == NULL || !can_see(ch, trainer))
   {
      send_to_char("You can't do that here.\n\r", ch);
      return;
   }

   if (/* !IS_NPC(trainer) && */ IS_SET(ch->act2, PLR_GUILDLESS))
   {
      send_to_char( "No guildmaster will train an outcast.\n\r", ch);
      return;
   }

   one_argument(argument, arg);

   if (arg[0] == '\0')
   {
      do_say(trainer, "Pardon me?");
      return;
   }

   if (!str_prefix(arg, "convert"))
   {
      if (ch->practice < 10)
      {
         act("$N tells you 'You are not yet ready.'",
         ch, NULL, trainer, TO_CHAR);
         return;
      }

      act("$N helps you apply your practice to training.",
      ch, NULL, trainer, TO_CHAR);
      ch->practice -= 10;
      ch->train +=1 ;
      return;
   }
   if (!str_prefix(arg, "revert"))
   {
      if (ch->train < 1)
      {
         act("$N tells you 'You do not have the training to apply to your skill practices yet.'",
         ch, NULL, trainer, TO_CHAR);
         return;
      }

      act("$N helps you apply your training to skill practices.",
      ch, NULL, trainer, TO_CHAR);
      ch->practice += 10;
      ch->train -= 1 ;
      return;
   }

   act("$N tells you 'I do not understand...'", ch, NULL, trainer, TO_CHAR);
   return;
}




/* RT spells and skills show the players spells (or skills) */

void do_spells(CHAR_DATA *ch, char *argument)
{
   BUFFER *buffer;
   char arg[MAX_INPUT_LENGTH];
   char spell_list[LEVEL_HERO + 1][MAX_STRING_LENGTH * 4];
   char spell_columns[LEVEL_HERO + 1];
   int sn, level, min_lev = 1, max_lev = LEVEL_HERO, mana;
   bool fAll = FALSE, found = FALSE;
   char buf[MAX_STRING_LENGTH];
   bool show_percents = FALSE;
   bool ansi = IS_ANSI(ch);

   if (IS_NPC(ch))
   return;

   if (argument[0] != '\0')
   {
      fAll = TRUE;
      if (!str_prefix("list", argument))
      {
         argument = one_argument(argument, arg);
         show_percents = TRUE;
      }
      if (str_prefix(argument, "all"))
      {
         argument = one_argument(argument, arg);
         if (!is_number(arg))
         {
            send_to_char("Arguments must be numerical or all.\n\r", ch);
            return;
         }
         max_lev = atoi(arg);

         if (max_lev < 1 || max_lev > LEVEL_HERO)
         {
            sprintf(buf, "Levels must be between 1 and %d.\n\r", LEVEL_HERO);
            send_to_char(buf, ch);
            return;
         }

         if (argument[0] != '\0')
         {
            argument = one_argument(argument, arg);
            if (!is_number(arg))
            {
               send_to_char("Arguments must be numerical or all.\n\r", ch);
               return;
            }
            min_lev = max_lev;
            max_lev = atoi(arg);

            if (max_lev < 1 || max_lev > LEVEL_HERO)
            {
               sprintf(buf,
               "Levels must be between 1 and %d.\n\r", LEVEL_HERO);
               send_to_char(buf, ch);
               return;
            }

            if (min_lev > max_lev)
            {
               send_to_char("That would be silly.\n\r", ch);
               return;
            }
         }
      }
   }


   /* initialize data */
   for (level = 0; level < LEVEL_HERO + 1; level++)
   {
      spell_columns[level] = 0;
      spell_list[level][0] = '\0';
   }

   for (sn = 1; sn < MAX_SKILL; sn++)
   {
      if (skill_table[sn].name == NULL )
      break;

      if ((level = /* skill_table[sn].skill_level[ch->class] */
      ch->pcdata->learnlvl[sn]) < LEVEL_HERO + 1
      &&  level >= min_lev && level <= max_lev
      &&  skill_table[sn].spell_fun != spell_null
      &&  ch->pcdata->learned[sn] > 0)
      {
         found = TRUE;
         level = /* skill_table[sn].skill_level[ch->class] */ ch->pcdata->learnlvl[sn];
         if (!show_percents)
         {
            if (ch->level < level)
            {
               sprintf
               (
                  buf,
                  "%-18s n/a      ",
                  skill_table[sn].name
               );
            }
            else
            {
               mana = mana_cost(ch, skill_table[sn].min_mana, sn);
               sprintf
               (
                  buf,
                  "%-18s  %3d mana  ",
                  skill_table[sn].name,
                  mana
               );
            }
         }
         else
         {
            if (ch->level < level)
            {
               sprintf
               (
                  buf,
                  "%s%-18s%s n/a     ",
                  (
                     (
                        ansi &&
                        ch->pcdata->updated[sn]
                     ) ?
                     "\x01B[1;33m" :
                     ""
                  ),
                  skill_table[sn].name,
                  (
                     (
                        ansi &&
                        ch->pcdata->updated[sn]
                     ) ?
                     ANSI_NORMAL :
                     ""
                  )
               );
            }
            else
            {
               sprintf
               (
                  buf,
                  "%s%-18s%s %3d%%      ",
                  (
                     (
                        ansi &&
                        ch->pcdata->updated[sn]
                     ) ?
                     "\x01B[1;33m" :
                     ""
                  ),
                  skill_table[sn].name,
                  (
                     (
                        ansi &&
                        ch->pcdata->updated[sn]
                     ) ?
                     ANSI_NORMAL :
                     ""
                  ),
                  ch->pcdata->learned[sn]
               );
            }
            ch->pcdata->updated[sn] = FALSE;
         }
         if (spell_list[level][0] == '\0')
         sprintf(spell_list[level], "\n\rLevel %2d: %s", level, buf);
         else /* append */
         {
            if ( ++spell_columns[level] % 2 == 0)
            {
               strlcat
               (
                  spell_list[level],
                  "\n\r          ",
                  sizeof(spell_list[0])
               );
            }
            strlcat(spell_list[level], buf, sizeof(spell_list[0]));
         }
      }
   }

   /* return results */

   if (!found)
   {
      send_to_char("No spells found.\n\r", ch);
      return;
   }

   buffer = new_buf();
   for (level = 0; level < LEVEL_HERO + 1; level++)
   if (spell_list[level][0] != '\0')
   add_buf(buffer, spell_list[level]);
   add_buf(buffer, "\n\r");
   page_to_char(buf_string(buffer), ch);
   free_buf(buffer);
}

void do_skills(CHAR_DATA *ch, char *argument)
{
   BUFFER *buffer;
   char arg[MAX_INPUT_LENGTH];
   char skill_list[LEVEL_HERO + 1][MAX_STRING_LENGTH * 4];
   char skill_columns[LEVEL_HERO + 1];
   int sn, level, min_lev = 1, max_lev = LEVEL_HERO;
   bool fAll = FALSE, found = FALSE;
   char buf[MAX_STRING_LENGTH];
   bool ansi = IS_ANSI(ch);

   if (IS_NPC(ch))
   return;

   if (argument[0] != '\0')
   {
      fAll = TRUE;

      if (str_prefix(argument, "all"))
      {
         argument = one_argument(argument, arg);
         if (!is_number(arg))
         {
            send_to_char("Arguments must be numerical or all.\n\r", ch);
            return;
         }
         max_lev = atoi(arg);

         if (max_lev < 1 || max_lev > LEVEL_HERO)
         {
            sprintf(buf, "Levels must be between 1 and %d.\n\r", LEVEL_HERO);
            send_to_char(buf, ch);
            return;
         }

         if (argument[0] != '\0')
         {
            argument = one_argument(argument, arg);
            if (!is_number(arg))
            {
               send_to_char("Arguments must be numerical or all.\n\r", ch);
               return;
            }
            min_lev = max_lev;
            max_lev = atoi(arg);

            if (max_lev < 1 || max_lev > LEVEL_HERO)
            {
               sprintf(buf,
               "Levels must be between 1 and %d.\n\r", LEVEL_HERO);
               send_to_char(buf, ch);
               return;
            }

            if (min_lev > max_lev)
            {
               send_to_char("That would be silly.\n\r", ch);
               return;
            }
         }
      }
   }


   /* initialize data */
   for (level = 0; level < LEVEL_HERO + 1; level++)
   {
      skill_columns[level] = 0;
      skill_list[level][0] = '\0';
   }

   for (sn = 1; sn < MAX_SKILL; sn++)
   {
      if (skill_table[sn].name == NULL )
      break;

      if
      (
         (
            level = ch->pcdata->learnlvl[sn]
         ) < LEVEL_HERO + 1 &&
         level >= min_lev &&
         level <= max_lev &&
         skill_table[sn].spell_fun == spell_null &&
         ch->pcdata->learned[sn] > 0
      )
      {
         found = TRUE;
         level = ch->pcdata->learnlvl[sn];
         if (ch->level < level)
         {
            sprintf
            (
               buf,
               "%s%-18s%s n/a      ",
               (
                  (
                     ansi &&
                     ch->pcdata->updated[sn]
                  ) ?
                  "\x01B[1;33m" :
                  ""
               ),
               skill_table[sn].name,
               (
                  (
                     ansi &&
                     ch->pcdata->updated[sn]
                  ) ?
                  ANSI_NORMAL :
                  ""
               )
            );
         }
         else
         {
            sprintf
            (
               buf,
               "%s%-18s%s %3d%%      ",
               (
                  (
                     ansi &&
                     ch->pcdata->updated[sn]
                  ) ?
                  "\x01B[1;33m" :
                  ""
               ),
               skill_table[sn].name,
               (
                  (
                     ansi &&
                     ch->pcdata->updated[sn]
                  ) ?
                  ANSI_NORMAL :
                  ""
               ),
               ch->pcdata->learned[sn]
            );
         }
         ch->pcdata->updated[sn] = FALSE;

         if (skill_list[level][0] == '\0')
         sprintf(skill_list[level], "\n\rLevel %2d: %s", level, buf);
         else /* append */
         {
            if ( ++skill_columns[level] % 2 == 0)
            {
               strlcat
               (
                  skill_list[level],
                  "\n\r          ",
                  sizeof(skill_list[0])
               );
            }
            strlcat(skill_list[level], buf, sizeof(skill_list[0]));
         }
      }
   }

   /* return results */

   if (!found)
   {
      send_to_char("No skills found.\n\r", ch);
      return;
   }

   buffer = new_buf();
   for (level = 0; level < LEVEL_HERO + 1; level++)
   if (skill_list[level][0] != '\0')
   add_buf(buffer, skill_list[level]);
   add_buf(buffer, "\n\r");
   page_to_char(buf_string(buffer), ch);
   free_buf(buffer);
}

/* shows skills, groups and costs (only if not bought) */
void list_group_costs(CHAR_DATA *ch)
{
   return;
}


void list_group_chosen(CHAR_DATA *ch)
{
   if (IS_NPC(ch))
   return;
   return;
}

int exp_per_level(CHAR_DATA *ch)
{
   int epl;

   if (IS_NPC(ch))
   return 1500;

   /*
   epl = pc_race_table[ch->race].xpadd + class_table[ch->class].xpadd + 1500;
   */
   epl = ch->pcdata->xp_pen;
   epl += (ch->level -1) * epl * .08;
   return epl;
}

/* this procedure handles the input parsing for the skill generator */
bool parse_gen_groups(CHAR_DATA *ch, char *argument)
{
   char arg[MAX_INPUT_LENGTH];
   char buf[100];
   int gn, sn, i;

   if (argument[0] == '\0')
   return FALSE;

   argument = one_argument(argument, arg);

   if (!str_prefix(arg, "help"))
   {
      if (argument[0] == '\0')
      {
         do_help(ch, "group help");
         return TRUE;
      }

      do_help(ch, argument);
      return TRUE;
   }

   if (!str_prefix(arg, "add"))
   {
      if (argument[0] == '\0')
      {
         send_to_char("You must provide a skill name.\n\r", ch);
         return TRUE;
      }

      gn = group_lookup(argument);
      if (gn != -1)
      {
         if (ch->gen_data->group_chosen[gn]
         ||  ch->pcdata->group_known[gn])
         {
            send_to_char("You already know that group!\n\r", ch);
            return TRUE;
         }

         if (group_table[gn].rating[ch->class] < 1)
         {
            send_to_char("That group is not available.\n\r", ch);
            return TRUE;
         }

         sprintf(buf, "%s group added\n\r", group_table[gn].name);
         send_to_char(buf, ch);
         ch->gen_data->group_chosen[gn] = TRUE;
         ch->gen_data->points_chosen += group_table[gn].rating[ch->class];
         gn_add(ch, gn);
         return TRUE;
      }

      sn = skill_lookup(argument);
      if (sn != -1)
      {
         if (ch->gen_data->skill_chosen[sn]
         ||  ch->pcdata->learned[sn] > 0)
         {
            send_to_char("You already know that skill!\n\r", ch);
            return TRUE;
         }

         if (skill_table[sn].rating[ch->class] < 1
         ||  skill_table[sn].spell_fun != spell_null)
         {
            send_to_char("That skill is not available.\n\r", ch);
            return TRUE;
         }
         sprintf(buf, "%s skill added\n\r", skill_table[sn].name);
         send_to_char(buf, ch);
         ch->gen_data->skill_chosen[sn] = TRUE;
         ch->gen_data->points_chosen += skill_table[sn].rating[ch->class];
         ch->pcdata->learned[sn] = 1;
         return TRUE;
      }

      send_to_char("No skills or groups by that name...\n\r", ch);
      return TRUE;
   }

   if (!strcmp(arg, "drop"))
   {
      if (argument[0] == '\0')
      {
         send_to_char("You must provide a skill to drop.\n\r", ch);
         return TRUE;
      }

      gn = group_lookup(argument);
      if (gn != -1 && ch->gen_data->group_chosen[gn])
      {
         send_to_char("Group dropped.\n\r", ch);
         ch->gen_data->group_chosen[gn] = FALSE;
         ch->gen_data->points_chosen -= group_table[gn].rating[ch->class];
         gn_remove(ch, gn);
         for (i = 0; i < MAX_GROUP; i++)
         {
            if (ch->gen_data->group_chosen[gn])
            gn_add(ch, gn);
         }
         return TRUE;
      }

      sn = skill_lookup(argument);
      if (sn != -1 && ch->gen_data->skill_chosen[sn])
      {
         send_to_char("Skill dropped.\n\r", ch);
         ch->gen_data->skill_chosen[sn] = FALSE;
         ch->gen_data->points_chosen -= skill_table[sn].rating[ch->class];
         ch->pcdata->learned[sn] = 0;
         return TRUE;
      }

      send_to_char("You haven't bought any such skill or group.\n\r", ch);
      return TRUE;
   }

   if (!str_prefix(arg, "premise"))
   {
      do_help(ch, "premise");
      return TRUE;
   }

   if (!str_prefix(arg, "list"))
   {
      list_group_costs(ch);
      return TRUE;
   }

   if (!str_prefix(arg, "learned"))
   {
      list_group_chosen(ch);
      return TRUE;
   }

   if (!str_prefix(arg, "info"))
   {
      do_groups(ch, argument);
      return TRUE;
   }

   return FALSE;
}






/* shows all groups, or the sub-members of a group */
void do_groups(CHAR_DATA *ch, char *argument)
{
   if (IS_NPC(ch))
   return;
   return;
}

int check_weapons(CHAR_DATA *ch, int sn){
   int sn_adv = -1;
   int sn_exp = -1;

   if (ch->pcdata->learned[sn] < 100)
   return sn;

   if (sn == gsn_sword){
      sn_adv = gsn_adv_sword;
      sn_exp = gsn_exp_sword;
   }
   if (sn == gsn_axe){
      sn_adv = gsn_adv_axe;
      sn_exp = gsn_exp_axe;
   }
   if (sn == gsn_polearm){
      sn_adv = gsn_adv_polearm;
      sn_exp = gsn_exp_polearm;
   }
   if (sn == gsn_spear){
      sn_adv = gsn_adv_spear;
      sn_exp = gsn_exp_spear;
   }
   if (sn == gsn_staff){
      sn_adv = gsn_adv_staff;
      sn_exp = gsn_exp_staff;
   }
   if (sn == gsn_dagger){
      sn_adv = gsn_adv_dagger;
      sn_exp = gsn_exp_dagger;
   }
   if (sn == gsn_mace){
      sn_adv = gsn_adv_mace;
      sn_exp = gsn_exp_mace;
   }
   if (sn == gsn_whip){
      sn_adv = gsn_adv_whip;
      sn_exp = gsn_exp_whip;
   }
   if (sn == gsn_archery){
      sn_adv = gsn_adv_archery;
      sn_exp = gsn_exp_archery;
   }
   if (sn == gsn_flail){
      sn_adv = gsn_adv_flail;
      sn_exp = gsn_exp_flail;
   }
   if (sn_adv == -1)
   return sn;
   if (ch->pcdata->learned[sn_adv] < 100)
   return sn_adv;

   return sn_exp;
}

/* checks for skill improvement */
void check_improve( CHAR_DATA *ch, int sn, bool success, int multiplier )
{
   int chance;
   char buf[100];

   if
   (
      IS_NPC(ch) ||
      ch->pcdata->spam >= current_time ||
      (
         ch->in_room &&
         IS_SET(ch->in_room->extra_room_flags, ROOM_1212)
      ) ||
      is_affected(ch, gsn_smite) ||
      is_affected(ch, gsn_punishment)
   )
   {
      return;
   }

   sn = check_weapons(ch, sn);
   if (/* ch->level < skill_table[sn].skill_level[ch->class] */ !has_skill(ch, sn)
   ||  skill_table[sn].rating[ch->class] == 0
   ||  ch->pcdata->learned[sn] < 1
   ||  ch->pcdata->learned[sn] == 100)
   return;  /* skill is not known */

   /* check to see if the character has a chance to learn */
   chance = 10 * int_app[get_curr_stat(ch, STAT_INT)].learn;
   chance /=
   (
      multiplier *
      skill_table[sn].rating[ch->class] *
      4
   );
   chance += ch->level;

   if (number_range(1, 1000) > chance)
   return;

   /* now that the character has a CHANCE to learn, see if they really have */
   /* special cases for mis-used practice dummies */
   if (ch->fighting != NULL
   && IS_NPC(ch->fighting)
   && ch->fighting->pIndexData->vnum == 1504
   && ch->house != HOUSE_COVENANT)
   {
      if (number_percent() < 8 && ch->pcdata->learned[sn] > 1)
      {
         sprintf(buf, "You have become worse at %s.\n\r", skill_table[sn].name);
         send_to_char(buf, ch);
         ch->pcdata->learned[sn]--;
         return;
      }
      return;
   }

   if (success)
   {
      chance = URANGE(5, 100 - ch->pcdata->learned[sn], 95);
      if (number_percent() < chance)
      {
         sprintf(buf, "You have become better at %s!\n\r", skill_table[sn].name);
         send_to_char(buf, ch);
         ch->pcdata->learned[sn]++;
         ch->pcdata->updated[sn] = TRUE;
         gain_exp(ch, 2 * skill_table[sn].rating[ch->class]);
         if (ch->pcdata->learned[sn] == 100)
         {
            if (IS_SET(ch->comm, COMM_ANSI))
            sprintf(buf,
            "Congratulations! You have mastered \x01b[1;37m%s\x01b[0;37m!!\n\r",
            skill_table[sn].name);
            else
            sprintf(buf, "Congratulations! You have mastered %s!!\n\r",
            skill_table[sn].name);
            send_to_char(buf, ch);
         }
         save_char_obj( ch );
      }
   }
   else
   {
      chance = URANGE(5, ch->pcdata->learned[sn]/2, 30);
      if (number_percent() < chance)
      {
         if (skill_table[sn].spell_fun == spell_null ||
         skill_table[sn].spell_fun == NULL)
         {
            sprintf(buf,
            "You learn from your mistakes, and your %s skill improves.\n\r",
            skill_table[sn].name);
         }
         else
         {
            sprintf
            (
               buf,
               "You learn from your mistakes, and your %s %s improves.\n\r",
               skill_table[sn].name,
               is_supplicating ? "prayer" : "spell"
            );
         }
         send_to_char(buf, ch);
         ch->pcdata->learned[sn] += number_range(1, 3);
         ch->pcdata->updated[sn] = TRUE;
         ch->pcdata->learned[sn] = UMIN(ch->pcdata->learned[sn], 100);
         gain_exp(ch, 2 * skill_table[sn].rating[ch->class]);
         if (ch->pcdata->learned[sn] == 100)
         {
            if (IS_SET(ch->comm, COMM_ANSI))
            sprintf(buf, "Congratulations! You have mastered \x01b[1;37m%s\x01b[0;37m!!\n\r",
            skill_table[sn].name);
            else
            sprintf(buf, "Congratulations! You have mastered %s!!\n\r",
            skill_table[sn].name);
            send_to_char(buf, ch);
         }
         save_char_obj( ch );
      }
   }
}

/* returns a group index number given the name */
int group_lookup( const char *name )
{
   int gn;

   for ( gn = 0; gn < MAX_GROUP; gn++ )
   {
      if ( group_table[gn].name == NULL )
      break;
      if ( LOWER(name[0]) == LOWER(group_table[gn].name[0])
      &&   !str_prefix( name, group_table[gn].name ) )
      return gn;
   }

   return -1;
}

/* recursively adds a group given its number -- uses group_add */
void gn_add( CHAR_DATA *ch, int gn)
{
   int i;

   ch->pcdata->group_known[gn] = TRUE;
   for ( i = 0; i < MAX_IN_GROUP; i++)
   {
      if (group_table[gn].spells[i] == NULL)
      break;
      group_add(ch, group_table[gn].spells[i], FALSE);
   }
}

/* recusively removes a group given its number -- uses group_remove */
void gn_remove( CHAR_DATA *ch, int gn)
{
   int i;

   ch->pcdata->group_known[gn] = FALSE;

   for ( i = 0; i < MAX_IN_GROUP; i ++)
   {
      if (group_table[gn].spells[i] == NULL)
      break;
      group_remove(ch, group_table[gn].spells[i]);
   }
}

/* use for processing a skill or group for addition  */
void group_add( CHAR_DATA *ch, const char *name, bool deduct)
{
   int sn, gn;

   if (IS_NPC(ch)) /* NPCs do not have skills */
   return;

   sn = skill_lookup(name);
   if (sn != -1 && sn != gsn_corrupt("subrank", &gsn_subrank))
   {
      if (ch->pcdata->learned[sn] == 0) /* i.e. not known */
      {
         ch->pcdata->learned[sn] = 1;
         ch->pcdata->learnlvl[sn] = skill_table[sn].skill_level[ch->class];
      }
      if ((skill_table[sn].skill_level[ch->class] > 51) && (get_trust(ch)<=51)
      && ch->pcdata->learnlvl[sn] > 51)
      ch->pcdata->learned[sn] = -1;
      return;
   }

   /* now check groups */

   gn = group_lookup(name);

   if (gn != -1)
   {
      if (ch->pcdata->group_known[gn] == FALSE)
      {
         ch->pcdata->group_known[gn] = TRUE;
      }
      gn_add(ch, gn); /* make sure all skills in the group are known */
   }
}

/* used for processing a skill or group for deletion -- no points back! */

void group_remove(CHAR_DATA *ch, const char *name)
{
   int sn, gn;

   sn = skill_lookup(name);

   if (sn != -1)
   {
      ch->pcdata->learned[sn] = -1;
      return;
   }

   /* now check groups */

   gn = group_lookup(name);

   if (gn != -1 && ch->pcdata->group_known[gn] == TRUE)
   {
      ch->pcdata->group_known[gn] = FALSE;
      gn_remove(ch, gn);  /* be sure to call gn_add on all remaining groups */
   }
}

void do_specialize(CHAR_DATA *ch, char *argument)
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *mob;
   /*    char buf[MAX_STRING_LENGTH];
   int spec = 0;*/

   if (IS_NPC(ch))
   return;

   if (get_trust(ch) <= 51)
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }

   for ( mob = ch->in_room->people; mob != NULL; mob = mob->next_in_room )
   {
      if ( IS_NPC(mob) && IS_SET(mob->act, ACT_PRACTICE) )
      break;
   }

   if ( mob == NULL )
   {
      send_to_char( "You can't do that here.\n\r", ch );
      return;
   }

   if (ch->pcdata->special != 0)
   {
      send_to_char("You already have a specialization.\n\r", ch);
      return;
   }

   one_argument(argument, arg);
   if (arg[0] == '\0')
   {
      send_to_char("This will be implemented later.\n\r", ch);
      return;
   }

   send_to_char("This will be implemented later.\n\r", ch);
   return;
}
