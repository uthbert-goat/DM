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

static const char rcsid[] = "$Id: magic2.c,v 1.168 2004/11/04 04:34:13 maelstrom Exp $";

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
#include "recycle.h"
#include "subskill.h"

DECLARE_DO_FUN( do_mount);
DECLARE_DO_FUN( do_dismount);
DECLARE_DO_FUN( do_say);
DECLARE_DO_FUN(do_look        );
DECLARE_DO_FUN( do_emote);
bool    remove_obj              args( ( CHAR_DATA *ch, int iWear, bool fReplace, bool show ) );
bool    check_room_protected    args( (ROOM_INDEX_DATA* room) );
void    make_obj_enchanted(OBJ_DATA *obj);

void    reset_area    args( ( AREA_DATA * pArea ) );
extern AREA_DATA *             area_first;
extern AREA_DATA *             area_last;

void make_demon(CHAR_DATA * demon, int dtype);
bool capsize_boat(CHAR_DATA *ch);

DECLARE_DO_FUN(do_north);
DECLARE_DO_FUN(do_south);
DECLARE_DO_FUN(do_east);
DECLARE_DO_FUN(do_west);
DECLARE_DO_FUN(do_up);
DECLARE_DO_FUN(do_down);
DECLARE_DO_FUN(do_flee);
DECLARE_DO_FUN(do_wake);
DECLARE_DO_FUN(do_scan);
DECLARE_DO_FUN(do_myell);
DECLARE_DO_FUN(do_conjure);
extern char *target_name;

void spell_farsight( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   if (IS_AFFECTED(ch, AFF_BLIND))
   {
      send_to_char("Maybe it would help if you could see?\n\r", ch);
      return;
   }

   do_scan(ch, target_name);
}

/******** TRUE BEGINNING OF THE NEW SET OF MAGIC ********/

void spell_pact(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   AFFECT_DATA af;
   CHAR_DATA *dcheck; /* - Wicket */
   int hp_gained=0;
   int mn_gained=0;
   bool found = FALSE; /* - Wicket */

   /*
   * Pact will only work if the casters demon exists. As per Rungekutta'a request.
   * - Wicket
   */
   if (house_down(ch, HOUSE_BRETHREN))
   return;

   for (dcheck = char_list; dcheck != NULL; dcheck = dcheck->next)
   {
      if (!IS_NPC(dcheck) || dcheck->master != ch)
      continue;

      if (dcheck->pIndexData->vnum == MOB_VNUM_DEMON1
      || dcheck->pIndexData->vnum == MOB_VNUM_DEMON2
      || dcheck->pIndexData->vnum == MOB_VNUM_DEMON3
      || dcheck->pIndexData->vnum == MOB_VNUM_DEMON4
      || dcheck->pIndexData->vnum == MOB_VNUM_DEMON5
      || dcheck->pIndexData->vnum == MOB_VNUM_DEMON6)
      found = TRUE;

      if (found) break;
   }

   if (!found)
   {
      send_to_char("You cannot strike a pact without your demon.\n\r", ch);
      return;
   }

   if (is_affected(ch, sn))
   {
      send_to_char("You have already struck a pact with your demon.\n\r", ch);
      return;
   }
   hp_gained = (get_curr_stat(ch, STAT_INT)+get_curr_stat(ch, STAT_CON))*(dcheck->level/4);
   mn_gained = (get_curr_stat(ch, STAT_INT)+get_curr_stat(ch, STAT_CON))*(dcheck->level/4)*1.5;
   af.where = TO_AFFECTS;
   af.type = sn;
   af.duration = 24;
   af.level = level;
   af.bitvector = 0;
   af.location = APPLY_HIT;
   af.modifier = hp_gained;
   affect_to_char(ch, &af);
   af.modifier = mn_gained;
   af.location = APPLY_MANA;
   affect_to_char(ch, &af);
   ch->hit += hp_gained;
   ch->mana += mn_gained;
   send_to_char("You feel infernal power flow through you.\n\r", ch);
   do_emote(dcheck, "grins knowingly....");
   return;
}
void do_conjure(CHAR_DATA *ch, char *argument)
{
   CHAR_DATA *demon;
   CHAR_DATA *dcheck;
   char arg[MAX_INPUT_LENGTH];
   AFFECT_DATA af;
   int chance = 0;
   int roll = 0;
   int dtype = -1;
   int danger = 0;
   char buf[MAX_STRING_LENGTH];

   if ( get_skill(ch, gsn_conjure) <= 0
   || !has_skill(ch, gsn_conjure))
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }

   if (house_down(ch, HOUSE_BRETHREN))
   return;

   if (ch->in_room != NULL && IS_SET(ch->in_room->extra_room_flags, ROOM_1212))
   {
      return;
   }
   one_argument( argument, arg );

   if ( arg[0] == '\0' )
   {
      send_to_char( "Conjure which type of demon?\n\r", ch );
      return;
   }

   if (ch->mana <100)
   {
      send_to_char( "You don't have enough mana.\n\r", ch);
      return;
   }

   if (ch->hit < 51)
   {
      send_to_char( "Your life force is too weak to conjure a demon.\n\r", ch);
      return;
   }

   if (is_affected(ch, gsn_conjure))
   {
      send_to_char("You have not yet recovered from your previous attempt at conjuration.\n\r", ch);
      return;
   }

   for (dcheck = char_list; dcheck != NULL; dcheck = dcheck->next)
   {
      if (IS_NPC(dcheck))
      if ( (dcheck->master == ch) &&
      (  dcheck->pIndexData->vnum == MOB_VNUM_DEMON1
      || dcheck->pIndexData->vnum == MOB_VNUM_DEMON2
      || dcheck->pIndexData->vnum == MOB_VNUM_DEMON3
      || dcheck->pIndexData->vnum == MOB_VNUM_DEMON4
      || dcheck->pIndexData->vnum == MOB_VNUM_DEMON5
      || dcheck->pIndexData->vnum == MOB_VNUM_DEMON6))
      {
         send_to_char("A demon is already bound to your service.\n\r", ch);
         return;
      }
   }

   /* added -werv */
   dtype = demon_lookup(NULL, arg);

   if (dtype == -1)
   {
      send_to_char("No demon of that type is known to exist!\n\r", ch);
      return;
   }

   ch->mana -=100;
   ch->hit -=50;

   /* next two lines are changed or added -werv */
   demon = create_mobile(get_mob_index(MOB_VNUM_DEMON1));
   make_demon(demon, dtype);

   danger = (ch->level)-(demon->level);
   chance = (get_skill(ch, gsn_conjure)+(danger*10));
   if (chance > 95)
   chance = 95;
   roll = number_percent();
   act("$n opens a flaming portal to the Abyss and a demon steps through!", ch, NULL, NULL, TO_ROOM);
   if (roll>chance) /* boom...you're screwed */
   {
      check_improve(ch, gsn_conjure, FALSE, 5);
      if (danger > -1)
      {
         af.where = TO_AFFECTS;
         af.level = demon->level;
         af.location = APPLY_SAVING_SPELL;
         af.modifier = 0-((demon->level)/10);
         af.duration = 12;
         af.bitvector = AFF_CURSE;
         af.type = gsn_conjure;
         affect_to_char(ch, &af);
         char_to_room(demon, ch->in_room);
         act_color("$n says, '{B{6You dare attempt to control me?!{n'", demon, NULL, NULL, TO_ROOM);
         sprintf(buf, "Help! I'm being attacked by %s!", PERS(demon, ch));
         do_myell(ch, buf);
         multi_hit(demon, ch, TYPE_UNDEFINED);
         af.where = TO_AFFECTS;
         af.level = ch->level;
         af.location = APPLY_STR;
         af.modifier = (demon->level)/10;
         af.duration = 12;
         af.bitvector = 0;
         af.type = gsn_conjure;
         affect_to_char(ch, &af);
         return;
      }
      if (danger < 0)
      {
         int vnum_to = ROOM_VNUM_ABYSS;
         send_to_char("A voice in your head booms...\n\r", ch);
         send_to_char("You are mine now, mortal!!!\n\r", ch);
         act("The demon drags $n back through the portal, and it snaps shut!", ch, NULL, NULL, TO_ROOM);
         send_to_char("With a vicious grin, the demon grabs you and drags you back through the portal!\n\r", ch);
         char_from_room(ch);
         vnum_to += number_range(0, 8);
         if (get_room_index(vnum_to) == NULL) vnum_to = 1;
         char_to_room(demon, get_room_index(vnum_to));
         char_to_room(ch, get_room_index(vnum_to));
         act_color("$n says, '{B{6Welcome to my world...now you die!{n'", demon, NULL, NULL, TO_ROOM);
         sprintf(buf, "Help! I'm being attacked by %s!", PERS(demon, ch));
         do_myell(ch, buf);
         multi_hit(demon, ch, TYPE_UNDEFINED);
         return;
      }
   }

   af.where = TO_AFFECTS;
   af.level = ch->level;
   af.location = APPLY_STR;
   af.modifier = (demon->level)/10;
   af.duration = 24;
   af.bitvector = 0;
   af.type = gsn_conjure;
   affect_to_char(ch, &af);

   check_improve(ch, gsn_conjure, TRUE, 1);
   char_to_room(demon, ch->in_room);
   SET_BIT(demon->affected_by, AFF_CHARM);
   add_follower(demon, ch);
   demon->leader = ch;
   demon->alignment = -1000;
   act
   (
      "$n binds $N to $s service, and snaps shut the portal!\n\r"
      "$N bows in grudging respect to $n.",
      ch,
      NULL,
      demon,
      TO_ROOM
   );
   act
   (
      "You bind $N to your service!\n\r"
      "$N bows in grudging respect to your power.",
      ch,
      NULL,
      demon,
      TO_CHAR
   );
   return;
}

/* returns the demon number from a name or real name or -1 for not found */
int  demon_lookup(char * name, char * real_name)
{
   int demon;
   demon = 0;

   while (demon_table[demon].name != NULL)
   {
      if ((name != NULL && !str_cmp(name, demon_table[demon].name)) ||
      (real_name != NULL && !str_cmp(real_name, demon_table[demon].real_name)))
      return demon;
      demon++;
   }
   return -1;
}

/* transform our demon mob to match whats in the demon table for dtype */
void make_demon(CHAR_DATA *demon, int dtype)
{
   if (dtype == -1)
   return;
   if (demon_table[dtype].name == NULL)
   return;
   free_string(demon->name);
   free_string(demon->short_descr);
   free_string(demon->long_descr);
   free_string(demon->description);
   demon->description = str_dup(demon_table[dtype].description);
   demon->name = str_dup(demon_table[dtype].name);
   demon->short_descr = str_dup(demon_table[dtype].short_descr);
   demon->long_descr = str_dup(demon_table[dtype].long_descr);
   demon->max_hit = demon_table[dtype].hp;
   demon->hit = demon->max_hit;
   demon->level = demon_table[dtype].min_level;
   demon->damroll = demon_table[dtype].dam;
   demon->spec_fun = spec_lookup("spec_brethren_demon");
   demon->armor[0] = demon_table[dtype].ac;
   demon->armor[1] = demon_table[dtype].ac;
   demon->armor[2] = demon_table[dtype].ac;
   demon->armor[3] = demon_table[dtype].ac * (.7);
   demon->imm_flags = demon_table[dtype].imm_flags;
   demon->res_flags = demon_table[dtype].res_flags;
   demon->vuln_flags = demon_table[dtype].vuln_flags;
   return;
}

void spell_barrier( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;
   int renewed = FALSE;
   int mod;
   OBJ_DATA * brand;

   if (((brand = get_eq_char(victim, WEAR_BRAND)) != NULL)
   && (brand->pIndexData->vnum == 433))
   {
      send_to_char("The gods have forsaken you.\n\r", victim);
      if (victim != ch)
      send_to_char("The gods have forsaken them.\n\r", ch);
      return;
   }

   if (is_affected(victim, sn) )
   {
      affect_strip(victim, sn);
      send_to_char("You renew your barrier.\n\r", victim);
      renewed = TRUE;
   }

   mod = 4;
   if (brand != NULL)
   mod = 6;
   af.where     = TO_AFFECTS;
   af.type     = gsn_barrier;
   af.level     = level;
   af.duration = level * mod * (race_adjust(ch)/25.0);
   af.modifier = 0;
   af.location = 0;
   af.bitvector = 0;
   affect_to_char(victim, &af);
   if (!renewed)
   {
      send_to_char("A barrier of spiritual energy forms around you.\n\r", victim);
      act("A barrier of spiritual energy forms around $n.", victim, NULL, NULL, TO_ROOM);
   }
   return;
}

void spell_ultra_heal( int sn, int level, CHAR_DATA *ch, void *vo, int target )
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

   heal = 200;
   if (brand != NULL)
   heal = 250;
   if
   (
      is_affected(victim, gsn_ancient_plague) ||
      is_affected(victim, gsn_mantle_oblivion) ||
      IS_SET(victim->act2, PLR_LICH)
   )
   heal = heal/2;
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

void spell_spiritual_wrath(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   CHAR_DATA* victim = (CHAR_DATA*) vo;
   int dam;
   OBJ_DATA* brand;
   OBJ_DATA* brand2;

   if
   (
      (brand = get_eq_char(ch, WEAR_BRAND)) &&
      brand->pIndexData->vnum == 433
   )
   {
      send_to_char("The gods have forsaken you.\n\r", ch);
      return;
   }
   brand2 = get_eq_char(victim, WEAR_BRAND);


   dam = dice(level, 8);

   if (brand != NULL)
   {
      dam += dice(3, level);
   }

   if
   (
      saves_spell(ch, level, victim, DAM_HOLY, SAVE_SPELL) ||
      saves_spell(ch, level + 5, victim, DAM_HOLY, SAVE_SPELL)
   )
   {
      dam /= 2;
   }
   if
   (
      brand2 != NULL &&
      brand != NULL &&
      brand2->pIndexData->vnum == brand->pIndexData->vnum &&
      !IS_IMMORTAL(ch)
   )
   {
      victim = ch;
   }
   if (IS_IMMORTAL(ch))
   {
      act
      (
         "You call down your wrath upon the heretic $N.",
         ch,
         NULL,
         victim,
         TO_CHAR
      );
      act
      (
         "$n calls down $s wrath upon the heretic $N.",
         ch,
         NULL,
         victim,
         TO_NOTVICT
      );
      act
      (
         "$n calls down $s wrath upon you.",
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
         "You call down the wrath of $z upon the heretic $N.",
         ch,
         NULL,
         victim,
         TO_CHAR
      );
      act
      (
         "$n calls down the wrath of $z upon the heretic $N.",
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
   }
   damage(ch, victim, dam * race_adjust(ch) / 25.0, sn, DAM_HOLY, TRUE);

   return;
}

/* cures: disease, poison, faerie fire, weaken, nerve, plague, curse, blind */
void spell_cleanse( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   OBJ_DATA *brand;
   int sn_faerie_fire, sn_faerie_fog, sn_weaken, sn_nerve, sn_ray;

   sn_faerie_fire = gsn_faerie_fire;
   sn_faerie_fog  = gsn_faerie_fog;
   sn_weaken      = gsn_weaken;
   sn_ray         = gsn_ray_of_enfeeblement;
   sn_nerve       = gsn_nerve;

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

   if (is_affected(victim, gsn_blindness_dust))
   if (check_dispel(level, victim, gsn_blindness_dust))
   {
      act("The dust in $n's eyes fades away.", victim, NULL, NULL, TO_ROOM);
   }

   if (is_affected(victim, gsn_blindness))
   if (check_dispel(level, victim, gsn_blindness))
   {
      act("$n is no longer blinded.", victim, NULL, NULL, TO_ROOM);
   }

   if (is_affected(victim, gsn_shadowstrike))
   if (check_dispel(level, victim, gsn_shadowstrike))
   {
      act("The shadowy arms grasping $n fade away in a smokey haze.", victim, NULL, NULL, TO_ROOM);
   }

   if (is_affected(victim, sn_nerve))
   if (check_dispel(level, victim, sn_nerve))
   {
      act("$n looks stronger.", victim, NULL, NULL, TO_ROOM);
   }

   if (is_affected(victim, sn_ray))
   if (check_dispel(level, victim, sn_ray))
   {
      act("$n looks less feeble.", victim, NULL, NULL, TO_ROOM);
   }

   if (is_affected(victim, sn_weaken))
   if (check_dispel(level, victim, sn_weaken))
   {
      act("$n looks stronger.", victim, NULL, NULL, TO_ROOM);
   }

   if (is_affected(victim, sn_faerie_fog))
   if (check_dispel(level, victim, sn_faerie_fog))
   {
      act("The purple aura about $n's body fades.", victim, NULL, NULL, TO_ROOM);
   }

   if (is_affected(victim, sn_faerie_fire))
   if (check_dispel(level, victim, sn_faerie_fire))
   {
      act("The pink aura about $n's body fades.", victim, NULL, NULL, TO_ROOM);
   }

   if (is_affected(victim, gsn_poison_dust))
   if (check_dispel(level, victim, gsn_poison_dust))
   {
      act("$n looks much better.", victim, NULL, NULL, TO_ROOM);
   }

   if (is_affected(victim, gsn_poison))
   if (check_dispel(level, victim, gsn_poison))
   {
      act("$n looks much better.", victim, NULL, NULL, TO_ROOM);
   }

   if (is_affected(victim, gsn_curse))
   if (check_dispel(level, victim, gsn_curse))
   {
      act("$n looks much better.", victim, NULL, NULL, TO_ROOM);
   }

   if (is_affected(victim, gsn_plague))
   if (check_dispel(level, victim, gsn_plague))
   {
      act("$n looks relieved as $s sores vanish.", victim, NULL, NULL, TO_ROOM);
   }
   if (victim != ch)
   send_to_char("You cleanse them of maledictions.\n\r", ch);
   else
   send_to_char("You cleanse yourself of maledictions.\n\r", victim);

}

void spell_health_bless(int sn, int level, CHAR_DATA *ch, void *vo, int target)
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
      /*        send_to_char("They are already healing at an improved rate.\n\r", ch);*/
      return;
   }
   af.where = TO_AFFECTS;
   af.type = sn;
   af.location = APPLY_REGENERATION;
   af.modifier = level/12;
   af.level = level;
   af.bitvector = 0;
   af.duration = level/3*race_adjust(ch)/25.0;
   affect_to_char(victim, &af);

   send_to_char("You feel an abundant warmth in a health blessing from the gods.\n\r", victim);
   if (victim != ch)
   act("You grant them a health blessing from $z.", ch, NULL, NULL, TO_CHAR);
   return;
}

void spell_benediction( int sn, int level, CHAR_DATA *ch, void *vo, int target)
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


   if ( victim->position == POS_FIGHTING || is_affected( victim, sn ) )
   {
      if (victim == ch)
      send_to_char("You already have a benediction.\n\r", ch);
      else
      act("$N has already received a benediction.", ch, NULL, victim, TO_CHAR);
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

   af.location  = APPLY_SAVING_SPELL;
   if ( level < 8 )
   af.modifier = -1;
   else
   af.modifier  = (0 - level / 8)*race_adjust(ch)/25.0;
   affect_to_char( victim, &af );
   send_to_char( "Your feel spiritual from your benediction.\n\r", victim );
   if ( ch != victim )
   act("You grant $N a spiritual benediction.", ch, NULL, victim, TO_CHAR);
   return;
}

void spell_cloak_brave(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;
   OBJ_DATA *brand;

   if (victim != ch)
   {
      send_to_char("Only you can wear a spiritual cloak of bravery.\n\r", ch);
      return;
   }

   if (((brand = get_eq_char(ch, WEAR_BRAND)) != NULL)
   && (brand->pIndexData->vnum == 433))
   {
      send_to_char("The gods have forsaken you.\n\r", ch);
      return;
   }

   if (is_affected(victim, sn))
   {
      send_to_char("You are as brave as you can be.\n\r", ch);
      return;
   }


   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.duration  = level / 8;
   af.modifier  = level / 6;
   af.bitvector = 0;

   af.location  = APPLY_HITROLL;
   af.modifier  *= (race_adjust(ch)/25.0);
   affect_to_char(victim, &af);

   af.location  = APPLY_DAMROLL;
   affect_to_char(victim, &af);

   af.modifier  = -level;
   af.modifier  *= (race_adjust(ch)/25.0);
   af.location  = APPLY_AC;
   /*    affect_to_char(victim, &af); */
   af.modifier = level;
   af.modifier  *= (race_adjust(ch)/25.0);
   af.location  = APPLY_HIT;
   affect_to_char(victim, &af);
   victim->hit = UMIN( victim->hit + level, victim->max_hit );


   send_to_char("Your loyalty to the faith will not be questioned!\n\r", victim);
   act("$n gets a wild look in $s eyes!", victim, NULL, NULL, TO_ROOM);
}

void spell_holyshield( int sn, int level, CHAR_DATA *ch, void *vo, int target ) {
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;


   if ( is_affected( victim, sn ) || is_affected(victim, gsn_darkshield) ||
   is_affected(victim, gsn_lightshield))
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
   af.modifier  = -level * (race_adjust(ch)/25.0);
   af.location  = APPLY_AC;
   af.bitvector = 0;
   affect_to_char( victim, &af );
   act("You are protected by $Z.", victim, NULL, ch, TO_CHAR);
   if ( ch != victim )
   act("$N is now protected by $z.", ch, NULL, victim, TO_CHAR);
   return;
}

void spell_phasing( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;

   if ( is_affected(victim, sn) )
   {
      if (victim == ch)
      send_to_char("You are already out of phase.\n\r", ch);
      else
      act("$N is already shifted out of phase.", ch, NULL, victim, TO_CHAR);
      return;
   }

   if (is_affected(victim, gsn_pass_door))
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
   af.duration  = level/3;
   af.location  = APPLY_NONE;
   af.modifier  = 0;
   if (!IS_AFFECTED(victim, AFF_PASS_DOOR))
   af.bitvector = AFF_PASS_DOOR;
   else
   af.bitvector = 0;
   affect_to_char( victim, &af );
   act( "$n turns translucent as $e goes a bit out of phase.", victim, NULL, NULL, TO_ROOM );
   send_to_char( "You turn translucent as you go a bit out of phase.\n\r", victim );
   return;
}

void spell_exorcism(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;
   char buf[MAX_STRING_LENGTH];
   int dam;
   OBJ_DATA * brand;
   OBJ_DATA * brand2;

   if (check_peace(ch)) return;

   if (((brand = get_eq_char(ch, WEAR_BRAND)) != NULL)
   && (brand->pIndexData->vnum == 433))
   {
      send_to_char("The gods have forsaken you.\n\r", ch);
      return;
   }

   send_to_char("You perform a ceremony and exorcise the demons from your foes.\n\r", ch);
   act("$n performs a religious ceremony attempting to remove the demons from $s foes!", ch, NULL, NULL, TO_ROOM);
   dam = dice(level, 5)*race_adjust(ch)/25.0;
   for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
   {
      brand2 = get_eq_char(vch, WEAR_BRAND);
      vch_next = vch->next_in_room;
      if (brand != NULL && brand2 != NULL)
      if (brand->pIndexData->vnum == brand2->pIndexData->vnum)
      continue;
      if
      (
         is_same_group(vch, ch)
      )
      {
         continue;
      }
      if
      (
         IS_IMMORTAL(vch) &&
         !can_see(ch, vch) &&
         wizi_to(vch, ch)
      )
      {
         continue;
      }
      if
      (
         is_safe(ch, vch, IS_SAFE_SILENT | IS_SAFE_AREA_ATTACK)
      )
      {
         continue;
      }
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
            sprintf(log_buf, "[%s] cast exorcism upon [%s] at %d", ch->name, vch->name, ch->in_room->vnum);
            log_string(log_buf);
         }
      }
      damage(ch, vch, dam, sn, DAM_HOLY, TRUE);
   }
   return;
}

void spell_hall_mirrors(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   int chance;
   AFFECT_DATA af;
   ROOM_AFFECT_DATA raf;
   ROOM_INDEX_DATA *was_in_room;
   int door;

   chance = ch->level * 2;
   if (IS_SET(ch->in_room->room_flags, ROOM_LOW_ONLY))
   chance = 0;
   if (ch->in_room->house != 0)
   chance = 0;
   if (is_affected(ch, sn)){
      send_to_char("You cant place another hall of mirrors yet.\n\r", ch);
      return;
   }
   if (is_affected_room(ch->in_room, sn))
   {
      send_to_char("There is already a hall of mirrors here.\n\r", ch);
      return;
   }
   if (number_percent() > chance)
   {
      send_to_char("You attempt to place a hall of mirrors and fail.\n\r", ch);
      return;
   }

   send_to_char("Your surroundings twist violently and a hall of mirrors forms.\n\r", ch);
   act("Your surroundings twist violently and a hall of mirrors forms.", ch, NULL, NULL, TO_ROOM);

   raf.where     = TO_ROOM;
   raf.duration = 8;
   raf.type = sn;
   raf.level = level;
   raf.bitvector = 0;
   raf.caster = ch;
   affect_to_room(ch->in_room, &raf);

   /* borrowed from deathcry -werv */
   was_in_room = ch->in_room;
   for ( door = 0; door <= 5; door++ )
   {
      EXIT_DATA *pexit;

      if ( ( pexit = was_in_room->exit[door] ) != NULL
      &&   pexit->u1.to_room != NULL
      &&   pexit->u1.to_room != was_in_room )
      {
         char_from_room(ch);
         char_to_room_1(ch, pexit->u1.to_room, TO_ROOM_AT);
         if (ch->in_room->house == 0)
         {
            affect_to_room(pexit->u1.to_room, &raf);
            act("Your surroundings twist violently and a hall of mirrors forms.", ch, NULL, NULL, TO_ROOM);
         }
      }
   }
   char_from_room(ch);
   char_to_room_1(ch, was_in_room, TO_ROOM_AT);

   af.where = TO_AFFECTS;
   af.type = sn;
   af.level = level;
   af.duration = 24;
   af.location = 0;
   af.modifier = 0;
   af.bitvector = 0;
   affect_to_char(ch, &af);
   return;
}

void spell_visions(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   CHAR_DATA* victim = (CHAR_DATA*) vo;
   int dam;

   act
   (
      "$n waves $s arms at $N and $N seems overcome with fright.",
      ch,
      NULL,
      victim,
      TO_NOTVICT
   );
   act
   (
      "$n waves $s arms at you and you see visions of your own death!",
      ch,
      NULL,
      victim,
      TO_VICT
   );
   act
   (
      "You cause $N to see visions of $S own death.",
      ch,
      NULL,
      victim,
      TO_CHAR
   );


   if (check_spellcraft(ch, sn))
   dam = spellcraft_dam(level, 11);
   else
   dam = dice(level, 11);


   if ( saves_spell(ch, level, victim, DAM_MENTAL, SAVE_MALEDICT)
   || saves_armor_of_god(ch, level, victim) )
   dam /= 2;

   damage( ch, victim, dam*race_adjust(ch)/25.0, sn, DAM_MENTAL, TRUE);
   return;
}

void spell_radiance(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;
   char buf[MAX_STRING_LENGTH];
   int dam;

   if (check_peace(ch)) return;


   send_to_char("With the utterance of arcane words you cause intense light to flood the room.\n\r", ch);
   act("$n utters some words and intense light floods the room from $s arms!", ch, NULL, NULL, TO_ROOM);
   dam = dice(level, 6)*race_adjust(ch)/25.0;
   for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
   {
      vch_next = vch->next_in_room;
      if (is_same_group(vch, ch))
      continue;
      if (IS_IMMORTAL(vch) && !can_see(ch, vch) && wizi_to(vch, ch)) continue;
      if (is_safe(ch, vch, IS_SAFE_SILENT | IS_SAFE_AREA_ATTACK))
      continue;
      if (oblivion_blink(ch, vch))
      {
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
         if (vch != ch)
         {
            do_myell(vch, buf);
            sprintf(log_buf, "[%s] cast radiance upon [%s] at %d", ch->name, vch->name, ch->in_room->vnum);
            log_string(log_buf);
         }
      }
      if (saves_spell(ch, level, vch, DAM_LIGHT, SAVE_SPELL) )
      damage(ch, vch, dam/2, sn, DAM_LIGHT, TRUE);
      else
      damage(ch, vch, dam, sn, DAM_LIGHT, TRUE);
   }
   return;
}

void spell_bang(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;
   AFFECT_DATA af;
   char buf[MAX_STRING_LENGTH];
   int dam;

   if (check_peace(ch)) return;


   act("The air above $n seems to explode with a deafening sound!", ch, NULL, NULL, TO_ROOM);
   dam = dice(level, 9)*race_adjust(ch)/25.0;
   for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
   {
      vch_next = vch->next_in_room;
      if (is_same_group(vch, ch))
      continue;
      if (IS_IMMORTAL(vch) && !can_see(ch, vch) && wizi_to(vch, ch)) continue;
      if (is_safe(ch, vch, IS_SAFE_SILENT | IS_SAFE_AREA_ATTACK))
      continue;

      if (oblivion_blink(ch, vch))
      {
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
         if (vch != ch)
         {
            do_myell(vch, buf);
            sprintf(log_buf, "[%s] cast bang upon [%s] at %d", ch->name, vch->name, ch->in_room->vnum);
            log_string(log_buf);
         }
      }
      if (saves_spell(ch, level, vch, DAM_SOUND, SAVE_SPELL) )
      damage(ch, vch, dam/2, sn, DAM_SOUND, TRUE);
      else
      {
         damage(ch, vch, dam, sn, DAM_SOUND, TRUE);
         if (!is_affected(vch, gsn_shock_sphere))
         {
            act("$n appears deafened.", vch, NULL, NULL, TO_ROOM);
            send_to_char("You can't hear a thing!\n\r", vch);

            af.where = TO_AFFECTS;
            af.modifier = -2;
            af.location = APPLY_HITROLL;
            af.bitvector = 0;
            af.type = gsn_shock_sphere;
            af.level = level;
            af.duration = level/8*race_adjust(ch)/25.0;
            affect_to_char(vch, &af);
         }
         else
         {
            send_to_char("The sound is so intense it cuts through your deafness!\n\r", vch);
         }

      }
   }
   return;
}

void spell_fist_of_god(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   int dam;

   act("A giant fist appears over $n and slams down upon $N.", ch, NULL, victim, TO_NOTVICT);
   act("A giant fist appears over $n and crashes down upon you!", ch, NULL, victim, TO_VICT);
   act("A giant fist appears over your head and crashes down upon $N.", ch, NULL, victim, TO_CHAR);


   if (check_spellcraft(ch, sn))
   dam = spellcraft_dam(level, 14);
   else
   dam = dice(level, 14);


   /*
   if (saves_spell(ch, level, victim, DAM_BASH, SAVE_SPELL));
   {
      dam /= 2;
   }
   Note above, the if statement has a semicolon, therefore dam /= 2 is always called.
   Should be replaced with
   if (saves_spell(ch, level, victim, DAM_BASH, SAVE_SPELL))
   {
      dam /= 2;
   }
   if we want the save to do anything
   */
   dam /= 2;   /* see above, making how the code works currently clear */

   damage( ch, victim, dam*race_adjust(ch)/25.0, sn, DAM_BASH, TRUE);
   return;
}

void spell_mirror_image(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *shadow;
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA *check;
   CHAR_DATA *victim = (CHAR_DATA *) vo;

   if
   (
      IS_NPC(victim) &&
      (
         victim->pIndexData->vnum == MOB_VNUM_SHADOW ||
         victim->pIndexData->vnum == MOB_VNUM_DECOY ||
         victim->pIndexData->vnum == MOB_VNUM_MIRROR
      )
   )
   {
      send_to_char("That would be redundant.\n\r", ch);
      return;
   }
   for (check = char_list; check != NULL; check = check->next)
   {
      if
      (
         IS_NPC(check) &&
         check->pIndexData->vnum == MOB_VNUM_MIRROR &&
         check->mprog_target == victim
      )
      {
         send_to_char("There is already a mirror image of that person.\n\r", ch);
         return;
      }
   }
   send_to_char("A second image of you slides from you and takes form.\n\r", victim);
   act("A second image of $n slides away from $m and takes form.", victim, NULL, NULL, TO_ROOM);
   shadow = create_mobile(get_mob_index(MOB_VNUM_MIRROR));
   shadow->alignment = 0;
   shadow->max_hit = 1;
   shadow->hit = 1;
   free_string(shadow->name);
   free_string(shadow->short_descr);
   free_string(shadow->long_descr);
   free_string(shadow->description);

   if ( IS_NPC( victim ) )
   {
      shadow->name = str_dup( victim->name );
      shadow->short_descr = str_dup( victim->short_descr );
      shadow->long_descr = str_dup( victim->long_descr );
      shadow->description = str_dup( victim->description );
   }
   else
   {
      strcpy(buf, get_name(victim, NULL));
      shadow->name = str_dup(buf);
      if (!is_affected(victim, gsn_cloak_form))
      sprintf(buf, "%s", victim->description);
      else
      sprintf(buf, "A figure shrouded in mystery.\n\r");
      shadow->description = str_dup(buf);
      sprintf(buf, "%s", get_longname(victim, NULL));
      shadow->short_descr = str_dup(buf);
      if (!IS_NPC(victim))
      sprintf(buf, "%s is here.\n\r",
      is_affected(ch, gsn_cloak_form) ? "A cloaked figure" : get_longname(victim, NULL));
      else
      sprintf(buf, "%s", victim->long_descr);
      shadow->long_descr = str_dup(buf);
   }
   char_to_room(shadow, victim->in_room);
   shadow->sex = victim->sex;
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
      shadow->mprog_target = victim->mprog_target;
   }
   else
   {
      shadow->mprog_target = victim;
   }
   shadow->master = victim;
   return;
}

void do_battlescream(CHAR_DATA *ch, char * argument)
{
   CHAR_DATA *victim;
   AFFECT_DATA af;

   if (get_skill(ch, gsn_battlescream) <= 0 ||
   !has_skill(ch, gsn_battlescream))
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }
   if (ch->move < 50)
   {
      send_to_char("You are too tired to battle scream.\n\r", ch);
      return;
   }
   WAIT_STATE(ch, 24);
   if (number_percent() >= get_skill(ch, gsn_battlescream))
   {
      send_to_char("You attempt a battle scream and fail.\n\r", ch);
      ch->move -= 25;
      check_improve(ch, gsn_battlescream, FALSE, 1);
      return;
   }
   ch->move -= 50;

   af.where     = TO_AFFECTS;
   af.type      = gsn_battlescream;
   af.level     = ch->level;
   af.location  = APPLY_AC;
   af.modifier  = ch->level;
   af.duration  = 6;
   af.bitvector = 0;

   act("$n bellows a blood curdling battle scream!", ch, NULL, NULL, TO_ROOM);
   send_to_char("You bellow a blood curdling battle scream.\n\r", ch);

   for (victim = ch->in_room->people; victim != NULL; victim = victim->next_in_room)
   {
      if (victim == ch) continue;
      if (is_same_group(ch, victim)) continue;
      if (victim->fighting == NULL) continue;
      if (!is_same_group(victim->fighting, ch)) continue;
      if (is_safe(ch, victim, IS_SAFE_SILENT | IS_SAFE_AREA_ATTACK)) continue;
      if (oblivion_blink(ch, victim))
      {
         continue;
      }
      if (IS_NPC(victim))
      {
         if (victim->fighting == NULL) continue;
         if (victim->house != 0    )
         {
            act("$n seems immune to fear.", victim, NULL, NULL, TO_ROOM);
            continue;
         }
      }
      if (IS_NPC(victim))
      if (victim->pIndexData->vnum == MOB_VNUM_DEMON1
      || victim->pIndexData->vnum == MOB_VNUM_DEMON2
      || victim->pIndexData->vnum == MOB_VNUM_DEMON3
      || victim->pIndexData->vnum == MOB_VNUM_DEMON4
      || victim->pIndexData->vnum == MOB_VNUM_DEMON5
      || victim->pIndexData->vnum == MOB_VNUM_DEMON6)
      {
         act("$n seems immune to fear.", victim, NULL, NULL, TO_ROOM);
         continue;

      }
      if (IS_NPC(victim) && (IS_SET(victim->imm_flags, IMM_MAGIC) ||
      IS_SET(victim->act, ACT_UNDEAD)))
      {
         act("$n seems immune to fear.", victim, NULL, NULL, TO_ROOM);
         continue;
      }
      if (number_percent() <= 30) continue;
      if (victim->level >= ch->level + 10) continue;
      if (is_affected(victim, gsn_battlescream)) continue;
      send_to_char("You are filled with fear from the blood curdling battle scream.\n\r", victim);
      do_flee(victim, "");
      do_flee(victim, "");
      do_flee(victim, "");
      affect_to_char(victim, &af);
   }
   check_improve(ch, gsn_battlescream, TRUE, 1);
   return;
}

void spell_fear(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;

   if (victim == ch)
   {
      send_to_char("That wouldn't work.\n\r", ch);
      return;
   }

   act("$n points at $N and fills $S head with nightmarish visions.", ch, NULL, victim, TO_NOTVICT);
   act("$n points at you and you begin to see nightmarish visions.", ch, NULL, victim, TO_VICT);
   act("You point at $N and force $M to see nightmarish visions.", ch, NULL, victim, TO_CHAR);

   if (!IS_AWAKE(victim))
   {
      act("You grin evilly knowing $N isn't having good dreams...", ch, NULL, victim, TO_CHAR);
      send_to_char("Your dreams turn terrifying and you wake in panic!\n\r", victim);
      do_wake(victim, "");
      if (!IS_AWAKE(victim))
      {
         act("Apparently $N can't wake up...oh what devilish fun....", ch, NULL, victim, TO_CHAR);
      }
      return;
   }

   if (IS_NPC(victim))
   {
      if (victim->house != 0)
      {
         act("$n seems immune to fear.", victim, NULL, NULL, TO_ROOM);
         return;
      }
   }
   if (IS_NPC(victim) && (IS_SET(victim->imm_flags, IMM_MAGIC) ||
   IS_SET(victim->act, ACT_UNDEAD)))
   {
      act("$n seems immune to fear.", victim, NULL, NULL, TO_ROOM);
      return;
   }

   if ( saves_spell(ch, level, victim, DAM_NEGATIVE, SAVE_MALEDICT)
   || saves_armor_of_god(ch, level, victim) )
   {
      send_to_char("You manage to conquer your fears.\n\r", victim);
      return;
   }


   act("A wild look comes over $N and $E seems to panic!", ch, NULL, victim, TO_CHAR);
   send_to_char("You panic in fear!\n\r", victim);
   if (victim->position == POS_FIGHTING)
   do_flee(victim, "");

   if (victim->position == POS_FIGHTING)
   {
      do_flee(victim, "");
   }
   if (victim->position == POS_FIGHTING)
   {
      do_flee(victim, "");
   }

   if (victim->daze == 0)
   DAZE_STATE(victim, 12);
   return;
}

void spell_fog_conceilment(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   AFFECT_DATA af;
   ROOM_AFFECT_DATA raf;

   if (is_affected(ch, sn)){
      send_to_char("You can't create more fog yet.\n\r", ch);
      return;
   }
   if (is_affected_room(ch->in_room, sn))
   {
      send_to_char("There is already a thick fog here.\n\r", ch);
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
   send_to_char( "A thick fog streams forth from your feet filling the room!\n\r", ch );
   act("A thick fog streams forth from $n's feet filling the room.", ch, NULL, NULL, TO_ROOM);
   raf.where  = TO_ROOM;
   raf.duration = 12;
   raf.type = sn;
   raf.level = level;
   raf.bitvector = ROOM_NOWHERE;
   raf.caster = ch;
   affect_to_room(ch->in_room, &raf);

   return;
}

void spell_strabismus( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;
   int dur;


   if (IS_AFFECTED(victim, AFF_BLIND))
   {
      if (victim == ch)
      send_to_char("You can't see illusions when blind.\n\r", ch);
      else
      act( "$N is blind and can't see illusions.", ch, NULL, victim, TO_CHAR );
      return;
   }
   if (victim == ch)
   {
      send_to_char("You can not create an illusion for yourself.\n\r", ch);
      return;
   }
   if (is_affected(victim, sn))
   {
      act("$E is already seeing illusions.", ch, NULL, victim, TO_CHAR);
      /*       send_to_char("They are already seeing illusions.\n\r", victim);*/
      return;
   }
   if (saves_spell(ch, (level-2), victim, DAM_MENTAL, SAVE_MALEDICT))
   {
      act( "Your attempt to cause $N to see illusions has failed.", ch, NULL, victim, TO_CHAR );
      return;
   }


   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.location  = APPLY_HITROLL;
   af.modifier  = -8;
   dur = (level/10)*race_adjust(ch)/25.0;
   if (dur > 5)
   dur = 5;
   af.duration  = dur;
   af.bitvector = 0;
   affect_to_char( victim, &af );
   send_to_char( "Everyone about you splits into many images!\n\r", victim );
   act("$n appears to be confused.", victim, NULL, NULL, TO_ROOM);
   return;
}

void spell_illusionary_wall(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   AFFECT_DATA af;
   ROOM_AFFECT_DATA raf;
   ROOM_INDEX_DATA *r2;
   EXIT_DATA *pexit;
   int dir_type = 0;

   if (target_name[0] == '\0')
   {
      send_to_char("You must specify a direction.\n\r", ch);
      return;
   }
   if (!str_cmp(target_name, "up"))
   dir_type = DIR_UP;
   if (!str_cmp(target_name, "down"))
   dir_type = DIR_DOWN;
   if (!str_cmp(target_name, "west"))
   dir_type = DIR_WEST;
   if (!str_cmp(target_name, "east"))
   dir_type = DIR_EAST;
   if (!str_cmp(target_name, "north"))
   dir_type = DIR_NORTH;
   if (!str_cmp(target_name, "south"))
   dir_type = DIR_SOUTH;

   pexit = ch->in_room->exit[dir_type];
   if (pexit == NULL)
   {
      send_to_char("Why create an illusion of a wall when you can have the real thing?\n\r", ch);
      return;
   }
   r2 = pexit->u1.to_room;
   if (r2 == NULL)
   {
      send_to_char("Why create an illusion of a wall when you can have the real thing?\n\r", ch);
      return;
   }
   if (is_affected(ch, sn)){
      send_to_char("You can't create another wall yet.\n\r", ch);
      return;
   }
   if (is_affected_room(ch->in_room, sn))
   {
      send_to_char("There is already an illusionary wall here.\n\r", ch);
      return;
   }
   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.location  = 0;
   af.modifier  = 0;
   af.duration  = 6;
   af.bitvector = 0;
   affect_to_char( ch, &af );
   send_to_char( "An illusionary wall forms at your command.\n\r", ch );
   act("An illusionary wall forms at $n's command.", ch, NULL, NULL, TO_ROOM);
   raf.where  = TO_ROOM;
   raf.duration = 12;
   raf.type = sn;
   raf.modifier = dir_type;
   raf.level = level;
   raf.bitvector = 0;
   raf.caster = ch;
   affect_to_room(ch->in_room, &raf);
   switch(raf.modifier)
   {
      case DIR_UP: raf.modifier = DIR_DOWN; break;
      case DIR_DOWN: raf.modifier = DIR_UP; break;
      case DIR_SOUTH: raf.modifier = DIR_NORTH; break;
      case DIR_NORTH: raf.modifier = DIR_SOUTH; break;
      case DIR_WEST: raf.modifier = DIR_EAST; break;
      case DIR_EAST: raf.modifier = DIR_WEST; break;
      default: break;
   }
   affect_to_room(r2, &raf);
   return;
}

void spell_invis_mail( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;

   if ( is_affected( victim, sn ) )
   {
      if (victim == ch)
      send_to_char("You are protected by an invisible mail.\n\r", ch);
      else
      act("$N is already protected by an invisible mail.", ch, NULL, victim, TO_CHAR);
      return;
   }
   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.duration  = 24*race_adjust(ch)/25.0;
   af.modifier  = -30;
   af.location  = APPLY_AC;
   af.bitvector = 0;
   affect_to_char( victim, &af );
   af.modifier = 0;
   af.location = 0;
   af.bitvector = AFF_INVISIBLE;
   affect_to_char(victim, &af);
   send_to_char( "You are surrounded by an invisible mail.\n\r", victim );
   if ( ch != victim )
   act("$N is surrounded by an invisible mail.", ch, NULL, victim, TO_CHAR);
   return;
}

void spell_lower_resist( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;

   if ( is_affected(victim, gsn_raise_resistance))
   {
      affect_strip(victim, gsn_raise_resistance);
      return;
   }
   if ( is_affected( victim, sn ) )
   {
      if (victim == ch)
      send_to_char("You have already had your resistance to magic lowered.\n\r", ch);
      else
      act("$N's resistance to magic has already been lowered.", ch, NULL, victim, TO_CHAR);
      return;
   }
   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.duration  = 12*race_adjust(ch)/25.0;
   af.modifier  = level/2*(race_adjust(ch)/25.0);
   af.location  = APPLY_SAVES;
   af.bitvector = 0;
   affect_to_char( victim, &af );
   send_to_char( "Your resistance to magic has been lowered.\n\r", victim );
   if ( ch != victim )
   act("$N's resistance to magic has been lowered.", ch, NULL, victim, TO_CHAR);
   return;
}

void spell_raise_resist( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;

   if ( is_affected(victim, gsn_lower_resistance))
   {
      affect_strip(victim, gsn_lower_resistance);
      return;
   }
   if ( is_affected( victim, sn ) )
   {
      if (victim == ch)
      send_to_char("You have already had your resistance to magic raised.\n\r", ch);
      else
      act("$N's resistance to magic has already been raised.", ch, NULL, victim, TO_CHAR);
      return;
   }
   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.duration  = 12*race_adjust(ch)/25.0;
   af.modifier  = -level/2*(race_adjust(ch)/25.0);
   af.location  = APPLY_SAVES;
   af.bitvector = 0;
   affect_to_char( victim, &af );
   send_to_char( "Your resistance to magic has been raised.\n\r", victim );
   if ( ch != victim )
   act("$N's resistance to magic has been raised.", ch, NULL, victim, TO_CHAR);
   return;
}

void spell_shrink( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;

   if ( is_affected(victim, gsn_enlarge))
   {
      affect_strip(victim, gsn_enlarge);
      send_to_char("You return to your normal size.\n\r", victim);
      if (victim != ch)
      send_to_char("You return them to their normal size.\n\r", ch);
      return;
   }
   if (victim->size == 0)
   {
      act("$E can't get any smaller!", ch, NULL, victim, TO_CHAR);
      /*      send_to_char("They can't get any smaller!\n\r", ch); */
      return;
   }
   if ( is_affected( victim, sn ) )
   {
      if (victim == ch)
      send_to_char("Your size has already been magically enhanced.\n\r", ch);
      else
      act("$N's size has already been magically enhanced.", ch, NULL, victim, TO_CHAR);
      return;
   }
   if (!is_same_group(ch, victim) && saves_spell(ch, level-5, victim, DAM_MENTAL, SAVE_MALEDICT))
   {
      act( "Your attempt to change $N's size has failed.", ch, NULL, victim, TO_CHAR );
      return;
   }
   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.duration  = 12*race_adjust(ch)/25.0;
   af.modifier  = -1;
   af.location  = APPLY_SIZE;
   af.bitvector = 0;
   affect_to_char( victim, &af );
   send_to_char( "You begin to shrink....\n\r", victim );
   if ( ch != victim )
   act("$N begins to shrink....", ch, NULL, victim, TO_CHAR);
   return;
}
void spell_enlarge( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;

   if ( is_affected(victim, gsn_shrink))
   {
      affect_strip(victim, gsn_shrink);
      send_to_char("You return to your normal size.\n\r", victim);
      if (victim != ch)
      send_to_char("You return them to their normal size.\n\r", ch);
      return;
   }
   if (victim->size == 5)
   {
      act("$E can't get any larger!", ch, NULL, victim, TO_CHAR);
      /*    send_to_char("They can't get any larger!\n\r", ch);*/
      return;
   }
   if ( is_affected( victim, sn ) )
   {
      if (victim == ch)
      send_to_char("Your size has already been magically enhanced.\n\r", ch);
      else
      act("$N's size has already been magically enhanced.", ch, NULL, victim, TO_CHAR);
      return;
   }
   /*    if (saves_spell(ch, level-5, victim, DAM_MENTAL, SAVE_MALEDICT))
   {
   act( "Your attempt to change $N's size has failed.", ch, NULL, victim, TO_CHAR );
   return;
   }*/
   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.duration  = 12*race_adjust(ch)/25.0;
   af.modifier  = 1;
   af.location  = APPLY_SIZE;
   af.bitvector = 0;
   affect_to_char( victim, &af );
   send_to_char( "You begin to grow....\n\r", victim );
   if ( ch != victim )
   act("$N begins to grow....", ch, NULL, victim, TO_CHAR);
   return;
}

void spell_halo_of_eyes( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;


   if (is_affected(victim, sn) )
   {
      send_to_char("You are already surrounded by a halo of eyes.\n\r", victim);
      return;
   }

   af.where     = TO_AFFECTS;
   af.type     = sn;
   af.level     = level;
   af.duration = level/4 * (race_adjust(ch)/25.0);
   af.modifier = 0;
   af.location = 0;
   af.bitvector = 0;
   affect_to_char(victim, &af);
   send_to_char("A halo of eyes forms around you.\n\r", victim);
   act("A halo of floating eyes form around $n.", victim, NULL, NULL, TO_ROOM);
   return;
}

void spell_mass_protection(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;


   send_to_char("You grant magical protection to all around you.\n\r", ch);

   for ( vch = ch->in_room->people; vch != NULL; vch = vch_next )
   {
      vch_next = vch->next_in_room;
      if
      (
         IS_IMMORTAL(vch) &&
         !can_see(ch, vch)
      )
      {
         continue;
      }
      if (!can_area_hit_rogue(ch, vch))
      {
         continue;
      }
      if (oblivion_blink(ch, vch))
      {
         continue;
      }
      magic_spell_vict
      (
         ch,
         vch,
         CAST_BITS_REUSE,
         level + 1,
         gsn_protective_shield
      );
      magic_spell_vict
      (
         ch,
         vch,
         CAST_BITS_REUSE,
         level,
         gsn_armor
      );
      magic_spell_vict
      (
         ch,
         vch,
         CAST_BITS_REUSE,
         level,
         gsn_shield
      );
      magic_spell_vict
      (
         ch,
         vch,
         CAST_BITS_REUSE,
         level,
         gsn_invisible_mail
      );
      magic_spell_vict
      (
         ch,
         vch,
         CAST_BITS_REUSE,
         level,
         gsn_chromatic_shield
      );
      magic_spell_vict
      (
         ch,
         vch,
         CAST_BITS_REUSE,
         level,
         gsn_raise_resistance
      );
   }

   return;
}

void spell_super_charge( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;


   if (is_affected(victim, sn) )
   {
      send_to_char("You are already surrounded by a super charged field.\n\r", victim);
      return;
   }

   af.where     = TO_AFFECTS;
   af.type     = sn;
   af.level     = level;
   af.duration = level/6 * (race_adjust(ch)/25.0);
   af.modifier = -150;
   af.location = APPLY_AC;
   af.bitvector = 0;
   affect_to_char(victim, &af);
   /*
   af.modifier = -30;
   af.location = APPLY_SAVES;
   affect_to_char(victim, &af);
   af.location = APPLY_MANA;
   af.modifier = -200;
   affect_to_char(victim, &af);
   */
   send_to_char("You are surrounded by a super charged field.\n\r", victim);
   act("A super charged field of energy forms around $n.", victim, NULL, NULL, TO_ROOM);
   return;
}


void spell_mass_haste( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   AFFECT_DATA af;
   CHAR_DATA *gch;

   for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
   {
      if
      (
         IS_IMMORTAL(gch) &&
         !can_see(ch, gch)
      )
      {
         continue;
      }
      if (!can_area_hit_rogue(ch, gch))
      {
         continue;
      }
      if (oblivion_blink(ch, gch))
      {
         continue;
      }
      if ( IS_AFFECTED(gch, AFF_HASTE)
      ||   IS_SET(gch->off_flags, OFF_FAST))
      {
         if (gch == ch)
         send_to_char("You can't move any faster!\n\r", ch);
         else
         act("$N is already moving as fast as $E can.", ch, NULL, gch, TO_CHAR);
         continue;
      }

      if (IS_AFFECTED(gch, AFF_SLOW))
      {
         if (is_affected(gch, gsn_slow))
         {
            if (gch == ch)
            send_to_char( "You feel yourself moving less slowly.\n\r", ch );
            else
            send_to_char( "You feel yourself moving less slowly.\n\r", gch );
            act("$n is moving less slowly.", gch, NULL, NULL, TO_ROOM);
            affect_strip(gch, gsn_slow);
         }
         if (is_affected(gch, gsn_mass_slow))
         {
            if (gch == ch)
            send_to_char( "You feel yourself moving less slowly.\n\r", ch );
            else
            send_to_char( "You feel yourself moving less slowly.\n\r", gch );
            act("$n is moving less slowly.", gch, NULL, NULL, TO_ROOM);
            affect_strip(gch, gsn_mass_slow);
         }
         continue;
      }

      send_to_char( "You feel yourself moving more quickly.\n\r", gch );
      act("$n is moving more quickly.", gch, NULL, NULL, TO_ROOM);

      af.where     = TO_AFFECTS;
      af.type      = sn;
      af.level     = level;
      af.duration  = 24;
      af.location  = APPLY_DEX;
      af.modifier  = 3;
      af.bitvector = AFF_HASTE;
      affect_to_char( gch, &af );
   }

   return;
}

void spell_mass_slow( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   AFFECT_DATA af;
   CHAR_DATA *gch;

   for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
   {
      if
      (
         IS_IMMORTAL(gch) &&
         !can_see(ch, gch)
      )
      {
         continue;
      }
      if (!can_area_hit_rogue(ch, gch))
      {
         continue;
      }
      if (oblivion_blink(ch, gch))
      {
         continue;
      }
      if (IS_AFFECTED(gch, AFF_SLOW))
      {
         if (gch == ch)
         send_to_char("You can't move any slower!\n\r", ch);
         else
         act("$N can't get any slower than that.", ch, NULL, gch, TO_CHAR);
         continue;
      }

      if (IS_AFFECTED(gch, AFF_HASTE))
      {
         if (is_affected(gch, gsn_haste))
         {
            if (gch == ch)
            send_to_char( "You feel yourself moving less quickly.\n\r", ch );
            else
            send_to_char( "You feel yourself moving less quickly.\n\r", gch );
            act("$n is moving less quickly.", gch, NULL, NULL, TO_ROOM);
            affect_strip(gch, gsn_haste);
         }
         if (is_affected(gch, gsn_mass_haste))
         {
            if (gch == ch)
            send_to_char( "You feel yourself moving less quickly.\n\r", ch );
            else
            send_to_char( "You feel yourself moving less quickly.\n\r", gch );
            act("$n is moving less quickly.", gch, NULL, NULL, TO_ROOM);
            affect_strip(gch, gsn_mass_haste);
         }
         continue;
      }

      if ((saves_spell(ch, level, gch, DAM_OTHER, SAVE_MALEDICT)
      ||  IS_SET(gch->imm_flags, IMM_MAGIC)))
      {
         if (gch != ch)
         send_to_char("Nothing seemed to happen.\n\r", ch);
         send_to_char("You feel momentarily lethargic.\n\r", gch);
         continue;
      }

      send_to_char( "You feel yourself slowing d o w n...\n\r", gch );
      act("$n starts to move in slow motion.", gch, NULL, NULL, TO_ROOM);

      af.where     = TO_AFFECTS;
      af.type      = sn;
      af.level     = level;
      af.duration  = 24;
      af.location  = APPLY_DEX;
      af.modifier  = -5;
      af.bitvector = AFF_SLOW;
      affect_to_char( gch, &af );
   }

   return;
}

void do_grace( CHAR_DATA *ch, char *argument)
{
   AFFECT_DATA af;

   if (get_skill(ch, gsn_grace) < 1 ||
   !has_skill(ch, gsn_grace))
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }
   if ( is_affected( ch, gsn_grace ) )
   {
      send_to_char("You are already experiencing the grace of extra time.\n\r", ch);
      return;
   }
   if (ch->mana < 50)
   {
      send_to_char("You do not have enough mana.\n\r", ch);
      return;
   }
   WAIT_STATE(ch, 12);
   if (number_percent() > get_skill(ch, gsn_grace))
   {
      ch->mana -= 25;
      check_improve(ch, gsn_grace, FALSE, 4);
      send_to_char("You fail.\n\r", ch);
      return;
   }
   ch->mana -= 50;
   af.where     = TO_AFFECTS;
   af.type      = gsn_grace;
   af.level     = ch->level;
   af.duration  = 24*race_adjust(ch)/25.0;
   af.modifier  = 3;
   af.location  = APPLY_DEX;
   af.bitvector = 0;
   affect_to_char( ch, &af );

   check_improve(ch, gsn_grace, TRUE, 4);
   send_to_char( "Everyone about you seems to slow down just a bit.\n\r", ch );
   act("$n seems to start moving just a bit faster...", ch, NULL, NULL, TO_ROOM);
   return;
}

void spell_freeze_person( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   CHAR_DATA *check = NULL;
   AFFECT_DATA af;
   int dur;


   if (is_affected(victim, gsn_freeze_person))
   {
      send_to_char("The Gods are protecting them from further temporal distortions.\n\r", ch);
      return;
   }
   if (is_affected(victim, gsn_timestop))
   {
      if (victim == ch)
      send_to_char("You are already frozen in time.\n\r", ch);
      else
      act( "$N is already frozen in time.", ch, NULL, victim, TO_CHAR );
      return;
   }
   if
   (
      (
         IS_NPC(victim) &&
         victim->house != 0
      ) ||
      saves_spell(ch, level, victim, DAM_ENERGY, SAVE_MALEDICT) ||
      saves_spell(ch, level, victim, DAM_ENERGY, SAVE_MALEDICT)
   )
   {
      act( "Your attempt to freeze $N in time has failed.", ch, NULL, victim, TO_CHAR );
      send_to_char("You feel a strange sensation as time flows in all directions around you.\n\r", victim);
      return;
   }

   af.where = TO_AFFECTS;
   af.location = 0;
   af.modifier = 0;
   af.duration = 6;
   af.bitvector = 0;
   af.level = level;
   af.type = sn;
   affect_to_char(victim, &af);

   af.where     = TO_AFFECTS;
   af.type      = gsn_timestop;
   af.level     = level;
   af.location  = 0;
   af.modifier  = 0;
   dur = 2;
   af.duration  = dur;
   af.bitvector = 0;
   affect_to_char( victim, &af );
   for (check = victim->in_room->people; check != NULL; check = check->next_in_room)
   if (check->fighting == victim)
   {
      check->fighting = NULL;
      check->position = POS_STANDING;
   }
   victim->fighting = NULL;

   send_to_char("You feel a strange sensation as everything around you freezes in time.\n\r", victim);
   act("$n appears to suddenly stop and freeze in time!", victim, NULL, NULL, TO_ROOM);
   send_to_char("You feel yourself suddenly slow down then everything stops.\n\r", victim);
   stop_fighting(victim, FALSE);
   return;
}

void spell_temporal_shield( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;


   if (is_affected(victim, sn) )
   {
      send_to_char("You are already in a temporal flux.\n\r", victim);
      return;
   }

   af.where     = TO_AFFECTS;
   af.type     = sn;
   af.level     = level;
   af.duration = level/6 * (race_adjust(ch)/25.0);
   af.modifier = 0;
   af.location = 0;
   af.bitvector = 0;
   affect_to_char(victim, &af);
   send_to_char("You place yourself in a state of temporal flux.\n\r", victim);
   act("$n begins to shimmer in and out of existence.", victim, NULL, NULL, TO_ROOM);
   return;
}

void do_know_time( CHAR_DATA *ch, char * argument)
{
   CHAR_DATA* victim;
   char buf[MAX_STRING_LENGTH];
   char arg1[MAX_INPUT_LENGTH];
   char* arg2;
   AFFECT_DATA *paf, *paf_last = NULL;
   bool ticks = FALSE;
   char time_string[MAX_INPUT_LENGTH];
   int skill;
   char affect_format[MAX_INPUT_LENGTH];
   char space_format[MAX_INPUT_LENGTH];
   unsigned int max_length = 0;

   if
   (
      (skill = get_skill(ch, gsn_know_time)) < 1 ||
      !has_skill(ch, gsn_know_time)
   )
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }
   if (argument[0] == '\0')
   {
      send_to_char
      (
         "You must specify who you wish to inspect in the temporal weave.\n\r",
         ch
      );
      return;
   }
   arg2 = one_argument(argument, arg1);
   if (!str_cmp(arg1, "ticks"))
   {
      ticks = TRUE;
   }
   else
   {
      arg2 = argument;
   }
   victim = get_char_room(ch, arg2);

   if (victim == NULL)
   {
      send_to_char("You cannot find them in the temporal weave.\n\r", ch);
      return;
   }
   if (ch->mana < 20)
   {
      send_to_char
      (
         "You do not have enough mental energy for the search.\n\r",
         ch
      );
      return;
   }
   WAIT_STATE(ch, 12);
   if (number_percent() > skill)
   {
      ch->mana -= 10;
      check_improve(ch, gsn_know_time, FALSE, 4);
      send_to_char
      (
         "You fail to see any discernable information in the weave.\n\r",
         ch
      );
      return;
   }
   ch->mana -= 20;
   if (oblivion_blink(ch, victim))
   {
      return;
   }
   /* calculate longest name of affect */
   for (paf = victim->affected; paf != NULL; paf = paf->next)
   {
      if (paf->type > 0 && paf->type < MAX_SKILL)
      {
         max_length =
         (
            UMAX
            (
               max_length,
               strlen(get_herb_spell_name(paf, TRUE, 0))
            )
         );
      }
   }
   max_length++;
   /* set up max_length spaces */
   sprintf(affect_format, "%%-%ds", max_length);
   sprintf(space_format, affect_format, "");

   if ( victim->affected != NULL )
   {
      act("$E is affected by the following:", ch, NULL, victim, TO_CHAR);
      for (paf = victim->affected; paf != NULL; paf = paf->next)
      {
         if (paf_last != NULL && paf->type == paf_last->type)
         {
            sprintf(buf, space_format);
         }
         else
         {
            sprintf(buf, affect_format, get_herb_spell_name(paf, TRUE, 0));
            buf[0] = UPPER(buf[0]);
         }
         send_to_char(buf, ch);
         sprintf
         (
            buf,
            ": modifies %s by %d%s\n\r",
            affect_loc_name( paf->location ),
            paf->modifier,
            get_time_string
            (
               ch,
               victim,
               paf,
               NULL,
               time_string,
               TYPE_TIME_KNOW_TIME,
               ticks
            )
         );
         send_to_char(buf, ch);
         paf_last = paf;
      }
   }
   else
   {
      act("$E is not affected by anything.", ch, NULL, victim, TO_CHAR);
   }
   check_improve(ch, gsn_know_time, TRUE, 4);
   return;
}

void spell_accelerate_time( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   AREA_DATA *pArea;
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;

   pArea = ch->in_room->area;

   if (ch->in_room->house != 0)
   {
      send_to_char("The gods prevent interference with the flow of time here.\n\r", ch);
      return;
   }

   pArea->age += level/3;
   send_to_char("You force time to accelerate all around you!\n\r", ch);
   if (pArea->age >= 30)
   {
      send_to_char("You succeed at accelerating the flow of time in this area.\n\r", ch);
      reset_area(pArea);
      pArea->age = number_range(0, 3);
   }
   for ( vch = char_list; vch != NULL; vch = vch_next )
   {
      vch_next    = vch->next;
      if (vch == NULL)
      continue;
      if ( vch->in_room == NULL )
      continue;
      if ( vch->in_room->area == ch->in_room->area )
      send_to_char( "The world seems to spin as time accelerates.\n\r", vch );
   }
   return;
}

void spell_temporal_shear(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;
   AFFECT_DATA *af2;
   int dam;

   act("$n creates a temporal vortex around you!", ch, NULL, victim, TO_VICT);
   act("You create a temporal vortex around $N.", ch, NULL, victim, TO_CHAR);

   if (check_spellcraft(ch, sn))
   dam = spellcraft_dam(level, 6);
   else
   dam = dice(level, 6);

   if (!is_affected(victim, sn))
   {
      af.where     = TO_AFFECTS;
      af.type     = sn;
      af.level     = level;
      af.duration = 6 * (race_adjust(ch)/25.0);
      af.modifier = 0;
      af.location = 0;
      af.bitvector = 0;
      affect_to_char(victim, &af);
      af2 = affect_find(victim->affected, gsn_temporal_shear);
      if (af2 != NULL)
      {
         free_string(af2->caster);
         af2->caster = str_dup(ch->name);
      }
   }
   else
   {
      dam /= 2;
   }
   act("The vortex engulfs you!", ch, NULL, victim, TO_VICT);
   act("The vortex engulfs $N.", ch, NULL, victim, TO_CHAR);


   damage( ch, victim, dam*race_adjust(ch)/25.0, sn, DAM_ENERGY, TRUE);
   return;
}

void spell_time_travel(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   OBJ_DATA *portal;

   for (portal = ch->in_room->contents;portal!=NULL;portal=portal->next_content)
   if (portal->item_type == ITEM_PORTAL_NEW &&
   IS_SET(portal->value[4], PORTAL_TIMEGATE))
   break;
   if (portal == NULL){
      send_to_char("There is not tear in the fabric of time here.\n\r", ch);
      return;
   }

   if (!IS_SET(portal->value[4], PORTAL_CLOSED))
   {
      send_to_char("This rift in the fabric of time is already wide open.\n\r", ch);
      return;
   }

   REMOVE_BIT(portal->value[4], PORTAL_CLOSED);
   send_to_char("You channel all of your magical energies into the rift and it opens.\n\r", ch);
   act("An intense beam of energy from $n causes a rift in time to open!", ch, NULL, NULL, TO_ROOM);

   return;
}

void spell_make_armor( int sn, int level, CHAR_DATA *ch, void *vo, int target){
   OBJ_DATA *obj = (OBJ_DATA *) vo;
   int chance;
   AFFECT_DATA *paf;

   if (obj == NULL)
   {
      send_to_char("Turn what into armor?\n\r", ch);
      return;
   }
   if (obj->item_type != ITEM_TRASH &&
   obj->item_type != ITEM_CLOTHING &&
   obj->item_type != ITEM_JEWELRY &&
   obj->item_type != ITEM_TOOL &&
   obj->item_type != ITEM_TREASURE &&
   obj->item_type != ITEM_GEM)
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
   if (obj->enchanted)
   chance = 0;
   if (obj->level > ch->level)
   chance = chance/2;

   if (number_percent() > chance){
      send_to_char("You fail the conversion.\n\r", ch);
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



   send_to_char("Success!!!\n\r", ch);
   act("$p flares and becomes armor.", ch, obj, NULL, TO_ROOM);
   act("$p flares and becomes armor.", ch, obj, NULL, TO_CHAR);
   obj->enchanted = TRUE;
   obj->item_type = ITEM_ARMOR;

   obj->value[0] = number_range(1, 5);
   obj->value[1] = number_range(1, 5);
   obj->value[2] = number_range(1, 5);
   obj->value[3] = number_range(1, 5);

   return;
}

void spell_fade(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   OBJ_DATA* obj = (OBJ_DATA*) vo;
   OBJ_DATA* obj_new;
   OBJ_DATA* iobj;
   int vnum;
   int timer;
   int chance;
   int cost;


   if (obj == NULL)
   {
      send_to_char("Return what to its original state?\n\r", ch);
      return;
   }

   if (obj->wear_loc != -1)
   {
      send_to_char("You must be carrying it.\n\r", ch);
      return;
   }
   /*
   if (is_affected_obj(obj, gsn_alchemy))
   {
   send_to_char("You cannot fade it due to the alchemy process.\n\r", ch);
   return;
   }
   */
   chance = get_skill(ch, sn);

   if (obj->pIndexData->vnum >= 2900 && obj->pIndexData->vnum <= 2999)
   {
      chance = 0;
   }

   if (number_percent() > chance)
   {
      send_to_char("You fail and the object is unchanged.\n\r", ch);
      return;
   }

   vnum = obj->pIndexData->vnum;
   timer = obj->timer;
   cost = obj->cost;
   /* Store contents. */
   obj_new = create_object(get_obj_index(vnum), 0);
   /*
   No need to check if obj->contains == NULL, for loop will
   not go into a single loop if it is.
   */
   for (iobj = obj->contains; iobj; iobj = obj->contains)
   {
      obj_from_obj(iobj);
      obj_to_obj(iobj, obj_new);
   }
   extract_obj(obj, FALSE);

   if (IS_SET(obj_new->extra_flags, ITEM_NODROP))
   {
      obj_to_room(obj_new, ch->in_room);
   }
   else
   {
      obj_to_char(obj_new, ch);
   }
   REMOVE_BIT(obj_new->extra_flags, ITEM_VIS_DEATH);
   /*
   Fade was changed to not modify timers on equipment.
   if
   (
   obj_new->pIndexData->condition < 0 &&
   obj_new->pIndexData->quitouts == FALSE
   )
   {
   obj_new->timer = (-1)*obj_new->pIndexData->condition;
   }
   */
   REMOVE_BIT(obj_new->extra_flags, ITEM_ROT_DEATH);
   obj_new->timer = timer;
   if (obj_new->cost > cost)
   {
      obj_new->cost = cost;
   }
   send_to_char("Success!!!\n\r", ch);
   act("$p shudders violently and then fades.", ch, obj_new, NULL, TO_ALL);
   return;
}

void spell_origin( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   OBJ_DATA *obj = (OBJ_DATA *) vo;
   char buf[MAX_STRING_LENGTH];
   MOB_INDEX_DATA *pMobIndex;
   AREA_DATA *pArea;
   RESET_DATA *pReset;
   int searchvnum;
   bool Found;
   bool Found2;
   int mob_vnum = 0;

   if (obj == NULL)
   {
      send_to_char("Try to discern the origins of what?\n\r", ch);
      return;
   }
   if (!IS_IMMORTAL(ch) &&
   (IS_OBJ_STAT(obj, ITEM_NOLOCATE) ||
   IS_SET(obj->extra_flags2, ITEM_NOORIGIN) ||
   ch->level < obj->level-5))
   {
      send_to_char("The gods cloud your vision.\n\r", ch);
      return;
   }

   searchvnum = obj->pIndexData->vnum;
   if (searchvnum >= 2900 && searchvnum <= 2999)
   {
      send_to_char("The gods cloud your vision.\n\r", ch);
      return;
   }
   Found = FALSE;
   for (pArea = area_first; pArea != NULL; pArea = pArea->next)
   {
      if (searchvnum >= pArea->min_vnum && searchvnum <= pArea->max_vnum)
      {
         sprintf(buf, "%s is from %-20s\n\r", obj->short_descr, pArea->name);
         send_to_char(buf, ch);
         Found = TRUE;
         break;
      }
   }

   if (Found == FALSE)
   {
      send_to_char("You can discern nothing about this thing.\n\r", ch);
      return;
   }

   Found2 = FALSE;
   for ( pReset = pArea->reset_first; pReset != NULL; pReset = pReset->next )
   {
      if (pReset->command == 'M')
      mob_vnum = pReset->arg1;
      if ((pReset->command == 'G' || pReset->command == 'E') && pReset->arg1 == searchvnum)
      {
         Found2 = TRUE;
         break;
      }
   }

   if (Found2)
   if ((pMobIndex = get_mob_index( mob_vnum ) ) != NULL)
   {
      sprintf(buf, "%s was once carried by %s\n\r", obj->short_descr, pMobIndex->short_descr);
      send_to_char(buf, ch);
   }

   if (Found == FALSE) send_to_char("The gods cloud your vision.\n\r", ch);

   return;
}

void spell_dissolve( int sn, int level, CHAR_DATA *ch, void *vo, int target){
   OBJ_DATA *obj = (OBJ_DATA *) vo;
   char material[MAX_STRING_LENGTH];
   char buf[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];
   int obj_level;
   int x;

   if (obj->pIndexData->vnum == OBJ_VNUM_GENOCIDE_AXE)
   {
      send_to_char("You do not the power to transform the axe.\n\r"
      "Ironically it possesses the power to transform you.\n\r", ch);
      return;
   }
   if (obj == NULL)
   {
      send_to_char("Disolve what into raw materials for alchemy?\n\r", ch);
      return;
   }

   if (obj->wear_loc != -1)
   {
      send_to_char("You must be carrying it.\n\r", ch);
      return;
   }
   if (is_affected_obj(obj, gsn_alchemy))
   {
      send_to_char("This object will not dissolve.\n\r", ch);
      return;
   }
   x = material_lookup(obj);
   if (material_table[x].lump_name == NULL)
   {
      act("$p vaporizes in a flash of light, leaving nothing.", ch, obj, ch, TO_CHAR);
      act("$p vaporizes in a flash of light, leaving nothing.", ch, obj, ch, TO_ROOM);
      extract_obj(obj, FALSE);
      return;
   }
   obj_level = 1;
   sprintf(material, "%s", material_table[material_lookup(obj)].name);
   sprintf(buf2, "%s slowly dissolves into $p.", obj->short_descr);
   extract_obj(obj, FALSE);

   obj = create_object(get_obj_index(OBJ_VNUM_MATERIAL_LUMP), 0);
   obj_to_char(obj, ch);
   obj->level = obj_level;
   sprintf( buf, obj->material, material );
   free_string( obj->material );
   obj->material = str_dup( buf );

   sprintf( buf, "%s", material_table[x].lump_name );
   free_string( obj->short_descr );
   obj->short_descr = str_dup( buf );

   sprintf( buf, "%s is here.", material_table[x].lump_name );
   free_string( obj->description );
   obj->description = str_dup( buf );
   sprintf( buf, obj->name, material );
   free_string( obj->name );
   obj->name = str_dup( buf );
   act(buf2, ch, obj, NULL, TO_ROOM);
   act(buf2, ch, obj, NULL, TO_CHAR);

   return;
}

void spell_demonic_enchant( int sn, int level, CHAR_DATA *ch, void *vo, int target){
   OBJ_DATA *obj = (OBJ_DATA *) vo;
   AFFECT_DATA af;
   CHAR_DATA *demon;
   CHAR_DATA *dcheck;
   int dtype = -1;

   if (house_down(ch, HOUSE_BRETHREN))
   return;

   if (is_affected(ch, sn))
   {
      send_to_char("You are still too weak to contain another demon.\n\r", ch);
      return;
   }
   demon = NULL;
   for (dcheck = char_list; dcheck != NULL; dcheck = dcheck->next)
   {
      if (IS_NPC(dcheck))
      if ( (dcheck->master == ch) &&
      (  dcheck->pIndexData->vnum == MOB_VNUM_DEMON1
      || dcheck->pIndexData->vnum == MOB_VNUM_DEMON2
      || dcheck->pIndexData->vnum == MOB_VNUM_DEMON3
      || dcheck->pIndexData->vnum == MOB_VNUM_DEMON4
      || dcheck->pIndexData->vnum == MOB_VNUM_DEMON5
      || dcheck->pIndexData->vnum == MOB_VNUM_DEMON6))
      {
         demon = dcheck;
      }
   }

   if (demon == NULL)
   {
      send_to_char("You must have a bound demon in order to entrap it.\n\r", ch);
      return;
   }

   af.where     = TO_AFFECTS;
   af.type     = sn;
   af.level     = level;
   af.duration = 48;
   af.modifier = 0;
   af.location = 0;
   af.bitvector = 0;
   affect_to_char(ch, &af);

   make_obj_enchanted(obj);

   if (number_percent() <= 5*(demon->level-level))
   {
      send_to_char("You fail to entrap the demon!\n\r", ch);
      do_say(demon, "You will die for trying to entrap me!");
      REMOVE_BIT(demon->affected_by, AFF_CHARM);
      multi_hit(demon, ch, TYPE_UNDEFINED);
      return;
   }
   dtype = demon_lookup(demon->name, NULL);

   if (dtype == -1)
   {
      log_string("Bug: demon not found in enchant demon.");
      return;
   }

   af.where = demon_table[dtype].apply_where;
   af.location = demon_table[dtype].apply_location;
   af.modifier = demon_table[dtype].apply_modifier;
   af.bitvector = demon_table[dtype].apply_bitvector;
   affect_to_obj(obj, &af);
   send_to_char("You entrap the demon within the object.\n\r", ch);
   extract_char(demon, TRUE);
   affect_strip(ch, gsn_conjure);
   return;
}

void spell_waterbreathing( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;


   if ( is_affected( victim, sn ) || IS_AFFECTED(victim, AFF_SWIM))
   {
      if (victim == ch)
      send_to_char("You can already breathe in the water!\n\r", ch);
      else
      act("$N can already breathe in the water.",
      ch, NULL, victim, TO_CHAR);
      return;
   }


   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.duration  = level/7;
   af.location  = 0;
   af.modifier  = 0;
   af.bitvector = AFF_SWIM;
   affect_to_char( victim, &af );
   send_to_char( "You acquire the ability to breathe under water.\n\r", victim );
   act("$n aquires the ability to breathe under water.", victim, NULL, NULL, TO_ROOM);
   return;
}

void spell_nausea( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *victim;
   AFFECT_DATA af;

   if (ch == NULL) return;

   victim = (CHAR_DATA *) vo;

   if (victim == NULL) return;

   if (is_affected(victim, sn))
   {
      act("$E is already nauseated.", ch, NULL, victim, TO_CHAR);
      /*       send_to_char("They are already nauseated.\n\r", ch); */
      return;
   }

   if ( saves_spell( ch, level, victim, DAM_OTHER, SAVE_MALEDICT) )
   {
      act("$n looks nauseated for a second, but it passes.", victim, NULL, NULL, TO_ROOM);
      send_to_char("You feel momentarily nauseated, but it passes.\n\r", victim);
      return;
   }

   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.duration  = level/12+number_range(0, 3);
   af.location  = APPLY_DEX;
   af.modifier  = -4;
   af.bitvector = 0;
   affect_join( victim, &af, 10 );
   send_to_char( "You feel nauseated.\n\r", victim );
   act("$n looks nauseated.", victim, NULL, NULL, TO_ROOM);
   return;
}

void spell_toad( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *victim;
   AFFECT_DATA af;

   if (ch == NULL) return;

   victim = (CHAR_DATA *) vo;

   if (victim == NULL) return;

   if (is_affected(victim, sn))
   {
      act("$E is no longer a toad.", ch, NULL, victim, TO_CHAR);
      /*     send_to_char("They are no longer a toad.\n\r", ch); */
      affect_strip(victim, sn);
      if (is_affected(victim, sn))
      affect_strip(victim, sn);
      if (is_affected(victim, sn))
      affect_strip(victim, sn);
      return;
   }
   if (IS_NPC(victim) && victim->house !=0)
   {
      act("$E seems immune to this magic.", ch, NULL, victim, TO_CHAR);
      /*        send_to_char("They seem immune to this magic.\n\r", ch); */
      return;
   }

   if ( saves_spell( ch, level, victim, DAM_ENERGY, SAVE_DEATH) ||
   saves_spell( ch, level, victim, DAM_ENERGY, SAVE_DEATH) ||
   saves_spell( ch, level, victim, DAM_ENERGY, SAVE_DEATH) )
   {
      act("$n turns green and shivers, but thats all.", victim, NULL, NULL, TO_ROOM);
      send_to_char("You feel very strange for a moment but it passes.\n\r", victim);
      return;
   }

   send_to_char( "You feel very different.\n\r", victim );
   act("$n turns green, shrinks, and becomes a toad!", victim, NULL, NULL, TO_ROOM);

   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   if (level < 46)  af.duration = 0;
   if (level < 48)  af.duration = 1;
   if (level < 52)  af.duration = 2;
   if (level > 51)  af.duration = 3;
   if (level > 55)  af.duration = 4;
   if (level >=60)  af.duration = 5;
   af.location  = APPLY_DEX;
   af.modifier  = -10;
   af.bitvector = 0;
   affect_join( victim, &af, 10 );
   af.location  = APPLY_STR;
   affect_join( victim, &af, 10 );
   af.modifier  = -ch->size;
   af.location  = APPLY_SIZE;
   affect_join( victim, &af, 10 );
   return;
}

void spell_whiplash( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *victim;

   victim = (CHAR_DATA *) vo;

   if (victim == NULL) return;

   if (ch->in_room->sector_type != SECT_FOREST)
   {
      send_to_char("You aren't in a suitable forest region to call vines.\n\r", ch);
      return;
   }

   if ( saves_spell( ch, level, victim, DAM_OTHER, SAVE_OTHER) )
   {
      send_to_char("The vines do not obey you.\n\r", ch);
      return;
   }
   DAZE_STATE(victim, 20);
   act("$n is tripped by vines.", victim, NULL, NULL, TO_ROOM);
   send_to_char("You are tripped by vines.\n\r", victim);
   return;
}

void lightblast_eq( CHAR_DATA *ch, CHAR_DATA *victim, int level )
{
   OBJ_DATA *obj, *obj_next;

   return; /* disabled -Werv */
   if ( IS_IMMORTAL(victim) )
   {
      act( "$n resists the effects of the Light on $s belongings.", victim, NULL, NULL, TO_ROOM );
      act( "You resist the effects of the Light on your belongings.", victim, NULL, NULL, TO_CHAR );
      return;
   }
   for ( obj = victim->carrying; obj != NULL; obj = obj_next )
   {
      obj_next = obj->next_content;

      if ( IS_SET(obj->extra_flags, ITEM_KEEP)) continue;
      if ( IS_SET(obj->extra_flags, ITEM_NOREMOVE) &&
      IS_SET(obj->extra_flags, ITEM_NOUNCURSE)) continue;
      if ( obj->pIndexData->vnum == 15903 ) continue;
      if ( obj->wear_loc == WEAR_BRAND ) continue;
      if ( IS_OBJ_STAT(obj, ITEM_ANTI_GOOD) || IS_OBJ_STAT(obj, ITEM_EVIL) )
      {
         if ( !saves_spell(ch, level+3, victim, DAM_LIGHT, SAVE_SPELL) )
         {
            if (( (obj->wear_loc != -1) && remove_obj(victim, obj->wear_loc, TRUE, TRUE))
            || (obj->wear_loc == -1))
            {
               act( "$n loses $s grasp on $p as the Light pulls it roughly away.", victim, obj, NULL, TO_ROOM );
               act( "You lose $s grasp on $p as the Light pulls it roughly away.", victim, obj, NULL, TO_CHAR );
               if ( !IS_SET(obj->extra_flags, ITEM_NODROP) ||
               !IS_SET(obj->extra_flags, ITEM_NOUNCURSE))
               {
                  obj_from_char( obj );
                  obj_to_room( obj, victim->in_room );
               }
            }
         }
      }
   }
}

void spell_lightblast(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   char limb[MAX_STRING_LENGTH], buf[MAX_STRING_LENGTH];
   int dam;

   if (house_down(ch, HOUSE_LIGHT))
   return;

   if ((victim == ch) && (ch->fighting == NULL))
   {
      send_to_char( "You feel comforted by the feel of the Light.\n\r", ch );
      return;
   }
   else
   {
      if ((victim == ch ) && (ch->fighting != NULL))
      victim = ch->fighting;
   }

   getLimb( ch, limb );

   sprintf( buf, "$n focuses between $s %ss, and a ball of pure white light forms.", limb );
   act( buf, ch, NULL, victim, TO_ROOM );
   sprintf( buf, "You draw Light into your body, the energy rushes like a powerful river.\n\rYou focus between your %ss, and a ball of pure white light forms.", limb );
   act( buf, ch, NULL, victim, TO_CHAR );

   sprintf( buf, "$n points $s %ss at $N and releases a blast of pure Light!", limb );
   act( buf, ch, NULL, victim, TO_NOTVICT );

   sprintf( buf, "$n points $s %ss at you, and you are struck by a blast of pure Light!", limb );
   act( buf, ch, NULL, victim, TO_VICT );

   sprintf( buf, "You point your %ss at $N and release the gathered Light.", limb );
   act( buf, ch, NULL, victim, TO_CHAR );

   if ( level <= 35 )
   {
      if ( check_spellcraft(ch, sn) )
      dam = spellcraft_dam( level, 11 );
      else
      dam = dice( level, 11 );
   }

   else
   if ( level <= 40 )
   {
      if ( check_spellcraft(ch, sn) )
      dam = spellcraft_dam( level, 12 );
      else
      dam = dice( level, 12 );
   }

   else
   if ( level <= 48 )
   {
      if ( check_spellcraft(ch, sn) )
      dam = spellcraft_dam( level, 13 );
      else
      dam = dice( level, 13 );
   }

   else
   {
      if ( check_spellcraft(ch, sn) )
      dam = spellcraft_dam( level, 13 );
      else
      dam = dice( level, 13 );

      dam += number_range( 0, level );
   }

   if ( IS_NPC(victim) )
   {
      if (level <= 35)
      dam += level;
      else if (level <= 40)
      dam += 3*level;
      else
      dam += 4*level;
   }

   dam /= 2;

   if
   (
      IS_NEUTRAL(victim) &&
      !EVIL_TO(ch, victim)
   )
   {
      dam /= 2;
   }

   if
   (
      IS_EVIL(ch) ||
      (
         !IS_NPC(ch) &&
         IS_SET(ch->act, PLR_EVIL)
      )
   )
   {
      send_to_char( "The Light burns you with its power, redirecting the energies into your body!\n\r", ch );
      act( "The Light turns back against $n!", ch, NULL, victim, TO_ROOM );
      damage( ch, ch, dam*race_adjust(ch)/25.0, sn, DAM_LIGHT, TRUE );
      send_to_char( "You stop channeling the forces of Light, and the energy dissipates.\n\r", ch );
      act( "$n's beam of pure white Light dissipates.", ch, NULL , NULL, TO_ROOM );
      return;
   }

   if ( IS_GOOD(victim) && !EVIL_TO(ch, victim))
   {
      OBJ_DATA *brand;
      int heal;
      int failHeal = 0;

      if (clergy_fails_help(ch, victim, gsn_heal))
      {
         return;
      }
      act( "The Light engulfs $n completely for a moment.", victim, NULL, ch, TO_ROOM );
      send_to_char( "A great sense of comfort eases your soul as the Light engulfs you.\n\r", victim );

      if (((brand = get_eq_char(victim, WEAR_BRAND)) != NULL)
      && (brand->pIndexData->vnum == 433))
      {
         send_to_char( "However, the gods have forsaken you, and the Light will not help.\n\r", victim );
         if ( victim != ch )
         send_to_char( "The gods have forsaken them.\n\r", ch );
         failHeal = 1;
      }

      if (((brand = get_eq_char(ch, WEAR_BRAND)) != NULL)
      && (brand->pIndexData->vnum == 433))
      {
         send_to_char( "The gods have forsaken you, causing your Light to do nothing.\n\r", ch );
         failHeal = 1;
      }

      if ( is_affected(victim, gsn_corrupt("smite", &gsn_smite)) )
      {
         send_to_char( "You feel better, but your wounds do not close.\n\r", victim );
         failHeal = 1;
      }

      heal = (dice(4, 8) + level - 6);  /* cure critical with 8 more possible bonus points */
      victim->hit = UMIN( victim->hit + heal, victim->max_hit );
      update_pos( victim );
      send_to_char( "You feel better!\n\r", victim );
      if ( number_percent() < 25 )
      lightblast_eq( ch, victim, level );
   }
   else
   {
      if (is_safe(ch, victim, 0))
      return;
      if (!IS_NPC(ch) && !IS_NPC(victim)
      && (ch->fighting == NULL || victim->fighting == NULL))
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
         if (victim != ch)
         {
            do_myell(victim, buf);
            sprintf(log_buf, "[%s] cast lightblast upon [%s] at %d",
            ch->name, victim->name, ch->in_room->vnum);
            log_string(log_buf);
         }
      }
      if (check_peace(ch))
      {
         send_to_char( "You stop channeling the forces of Light, and the energy dissipates.\n\r", ch );
         act( "$n's beam of pure white Light dissipates.", ch, NULL , NULL, TO_ROOM );
         return;
      }

      if ( number_percent() < 25 )
      {
         switch ( number_range(1, 3) )
         {
            case 1:
            magic_spell_vict
            (
               ch,
               victim,
               CAST_BITS_REUSE,
               level + 2,
               gsn_blindness
            );
            send_to_char( "The afterimage of that blast of pure Light flickers across your eyes.\n\r", victim );
            break;

            case 2:
            magic_spell_vict
            (
               ch,
               victim,
               CAST_BITS_REUSE,
               level + 2,
               gsn_curse
            );
            break;

            case 3:
            lightblast_eq( ch, victim, level );
            break;
         }
      }

      damage( ch, victim, dam*race_adjust(ch)/25.0, sn, DAM_LIGHT, TRUE );
   }

   send_to_char( "You stop channeling the forces of Light, and the energy dissipates.\n\r", ch );
   act( "$n's beam of pure white Light dissipates.", ch, NULL , NULL, TO_ROOM );

   return;
}

void spell_lightbind(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   if (house_down(ch, HOUSE_LIGHT))
   return;

   send_to_char( "Nothing happens when you lightbind.  Get someone to code this.\n\r", ch );

   return;
}

void spell_hand_of_vengeance(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   CHAR_DATA* master;
   int dam;
   int record;
   int ndice;

   if (house_down(ch, HOUSE_ENFORCER))
   return;

   master = victim;
   while
   (
      IS_NPC(master) &&
      master->master &&
      master->master != master &&
      master->master->master != master &&
      IS_AFFECTED(master, AFF_CHARM)
   )
   {
      master = master->master;
   }
   if (IS_NPC(master))
   {
      send_to_char("The clenched fist fails to form.\n\r", ch);
      return;
   }
   record = master->pcdata->wanteds;
   if (IS_SET(master->act2, PLR_LAWLESS))
   {
     record += 60;
   }
   ndice = 6;  /* min of ld11 */
   if (record >= 10)
   ndice++;
   if (record >= 15)
   ndice++;
   if (record >= 30)
   ndice++;
   if (record >= 40)
   ndice++;
   if (record >= 50)
   ndice++;
   if (record >= 60)  /* max of ld12 */
   ndice++;
   dam = dice(level, ndice);

   if
   (
      IS_NPC(master) ||
      (
         !IS_SET(master->act, PLR_CRIMINAL) &&
         !IS_SET(master->act2, PLR_LAWLESS)
      )
   )
   {
      act("A huge clenched fist appears above $n but fades without striking.", victim, NULL, NULL, TO_ROOM);
      send_to_char("A huge clenched fist appears above you but fades without striking.\n\r", victim);
      send_to_char("The Immortal of Justicar frowns upon the abuse of this power.\n\r", ch);
      return;
   }
   act("A huge clenched fist appears above $n and strikes down.", victim, NULL, NULL, TO_ROOM);
   send_to_char("A huge clenched fist appears above you and strikes down.\n\r", victim);

   if (saves_spell(ch, level, victim, DAM_BASH, SAVE_SPELL) )
   {
      dam /= 2;
   }
   else if (number_range(0, 3) == 0)
   {
      act("The blow hammers $n to the ground with savage force!", victim, NULL, NULL, TO_ROOM);
      send_to_char("The blow hammers you to the ground with savage force!\n\r", victim);
      dam += dice(level, 4);
      WAIT_STATE(victim, 24);
   }

   damage(ch, victim, dam, sn, DAM_BASH, TRUE);
   return;
}

void spell_seal_of_justice(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   AFFECT_DATA* paf;
   int status;

   if (house_down(ch, HOUSE_ENFORCER))
   {
      return;
   }
   if (is_affected(ch, sn))
   {
      send_to_char
      (
         "You are already proudly displaying your devotion to justice.\n\r",
         ch
      );
      return;
   }
   if (IS_NPC(ch))
   {
      status = 1000;
   }
   else
   {
      status = URANGE(1, ch->pcdata->targetkills, 5000);
   }
   paf = new_affect();
   paf->where     = TO_AFFECTS;
   paf->type      = gsn_seal_of_justice;
   paf->level     = level;
   paf->duration  = 12;

   paf->location  = APPLY_HIT;
   paf->modifier  = 50 + status * 5 / 100;
   affect_to_char(ch, paf);

   paf->location  = APPLY_DAMROLL;
   paf->modifier  = 2 + status * 26 / 10000;
   affect_to_char(ch, paf);

   paf->location  = APPLY_AC;
   paf->modifier  = -5 - status * 5 / 1000;
   affect_to_char(ch, paf);

   paf->modifier  = status * 6 / 10000;
   if (is_clergy(ch))
   {
      paf->modifier -= paf->modifier / 2;
      paf->location  = APPLY_HOLY_POWER;
   }
   else
   {
      paf->location  = APPLY_SPELL_POWER;
   }
   if (paf->modifier)
   {
      affect_to_char(ch, paf);
   }
   free_affect(paf);
   send_to_char("You don a seal of justice.\n\r", ch);
   return;
}

void spell_napalm(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA* victim = (CHAR_DATA* ) vo;
   int dam;
   int spins;
   int cnt;
   int n_dice = level / 8;

   if (n_dice > 5)
   {
      n_dice = 5;
   }

   act("Fiery debris explodes from your hands!", ch, NULL, victim, TO_CHAR);

   spins = number_range(n_dice - 3, n_dice);
   if (spins < 1)
   {
      spins = 1;
   }
   for (cnt = 1; cnt <= spins; cnt++)
   {
      if (check_spellcraft(ch, sn))
      {
         dam = spellcraft_dam(level, n_dice);
      }
      else
      {
         dam = dice(level, n_dice);
      }
      n_dice--;
      if (n_dice < 1)
      {
         n_dice = 1;
      }
      if
      (
         victim->ghost > 0 ||
         victim->in_room != ch->in_room
      )
      {
         return;
      }
      if (saves_spell(ch, level, victim, DAM_FIRE, SAVE_SPELL))
      {
         dam /= 2;
      }
      act("The fiery debris strikes $n.", victim, NULL, NULL, TO_ROOM);
      damage(ch, victim, dam * race_adjust(ch) / 25.0, sn, DAM_FIRE, TRUE);
   }
   return;
}

void spell_incinerate(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   int dam;

   if (check_spellcraft(ch, sn))
   dam = spellcraft_dam(level, 9);
   else
   dam = dice(level, 9);

   if ( saves_spell( ch, level, victim, DAM_FIRE, SAVE_SPELL) )
   dam /= 2;

   act("Fierce flames erupt from $n's hands and consume $N!", ch, NULL, victim, TO_NOTVICT);
   act("Fierce flames erupt from your hands and consume $N!", ch, NULL, victim, TO_CHAR);
   act("Fierce flames erupt from $n's hands and consume you!", ch, NULL, victim, TO_VICT);

   switch(check_immune(victim, DAM_FIRE))
   {
      case IS_IMMUNE:         dam -= dam/3;       break;
      case IS_RESISTANT:                          break;
      case IS_NORMAL:
      case IS_VULNERABLE:     dam += dam/3;       break;
   }

   damage(ch, victim, dam*race_adjust(ch)/25.0, sn, DAM_OTHER, TRUE);
   return;
}

void spell_nova(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   CHAR_DATA* vch;
   CHAR_DATA* vch_next;
   char buf[MAX_STRING_LENGTH];
   int dam;
   int true_dam;
   static const sh_int dam_each[] =
   {
      0,
      0,   0,   0,   0,   0,      0,   0,   0,   0,   0,
      0,   0,   0,   0,   30,     35,  40,  45,  50,  55,
      60,  65,  70,  75,  80,     82,  84,  86,  88,  90,
      92,  94,  96,  98,  100,    102, 104, 106, 108, 110,
      112, 114, 116, 118, 120,    122, 124, 126, 128, 130,
      132, 133, 134, 135, 136,    137, 138, 139, 140, 141
   };

   if (check_peace(ch))
   {
      return;
   }

   send_to_char("You conjure an intense nova of flames.\n\r", ch);
   act
   (
      "$n conjures an intense nova of flames!",
      ch,
      NULL,
      NULL,
      TO_ROOM
   );

   level = UMAX(0, level) * race_adjust(ch) / 25.0;
   level = UMIN(level, (int)(sizeof(dam_each) / sizeof(dam_each[0]) - 1));
   level = UMAX(0, level);
   dam   = number_range(dam_each[level] / 2, dam_each[level] * 2);
   if (dam > 282)
   {
      dam = 282;
   }

   if (check_spellcraft(ch, sn))
   {
      dam += spellcraft_dam(level, 3);
   }
   else
   {
      dam += dice(level, 3);
   }
   dam *= 8;
   dam /= 10;

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
            }
         }
         do_myell(vch, buf);
         sprintf
         (
            log_buf,
            "[%s] cast nova upon [%s] at %d",
            ch->name,
            vch->name,
            ch->in_room->vnum
         );
         log_string(log_buf);
      }

      true_dam = dam;
      /* If fail two saves, increase damage */
      if
      (
         !saves_spell(ch, level, vch, DAM_FIRE, SAVE_SPELL) &&
         !saves_spell(ch, level, vch, DAM_FIRE, SAVE_SPELL)
      )
      {
         dam += dice(level, 3);
      }

      damage(ch, vch, dam, sn, DAM_FIRE , TRUE);
      dam = true_dam; /* to avoid weird buildups in large groups */
   }
   return;
}

void spell_inferno(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;
   char buf[MAX_STRING_LENGTH];
   int dam;
   AFFECT_DATA af;

   if (check_peace(ch)) return;

   af.where = TO_AFFECTS;
   af.location = APPLY_HITROLL;
   af.modifier = -3;
   af.duration = 1;
   af.level = level;
   af.type = sn;
   af.bitvector = AFF_BLIND;

   send_to_char("You create a wall of black flames that races towards your foes!\n\r", ch);
   act("$n creates a wall of black flames!", ch, NULL, NULL, TO_ROOM);

   dam = dice(level, 5)*race_adjust(ch)/25.0;

   if (check_spellcraft(ch, sn))
   dam += spellcraft_dam(level, 3);

   for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
   {
      vch_next = vch->next_in_room;
      if (is_same_group(vch, ch))
      continue;
      if (IS_IMMORTAL(vch) && !can_see(ch, vch) && wizi_to(vch, ch)) continue;
      if (is_safe(ch, vch, IS_SAFE_SILENT | IS_SAFE_AREA_ATTACK))
      continue;

      if (oblivion_blink(ch, vch))
      {
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
         if (vch != ch)
         {
            do_myell(vch, buf);
            sprintf(log_buf, "[%s] cast inferno upon [%s] at %d", ch->name, vch->name, ch->in_room->vnum);
            log_string(log_buf);
         }
      }

      if ((number_range(0, 1) == 0) && !saves_spell(ch, level, vch, DAM_FIRE, SAVE_SPELL))
      {
         affect_strip(vch, sn);
         act("$n appears blinded by the smoke.", vch, NULL, NULL, TO_ROOM);
         send_to_char("Your eyes are scorched by fire and smoke!.\n\r", vch);
         affect_to_char(vch, &af);
      }
      damage(ch, vch, dam, sn, DAM_FIRE, TRUE);
   }
   return;
}

void spell_imbue_flame(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   OBJ_DATA *obj = (OBJ_DATA *) vo;
   AFFECT_DATA af;

   if (obj == NULL) return;

   if (obj->item_type != ITEM_WEAPON)
   {
      send_to_char("That object cannot be imbued with flames.\n\r", ch);
      return;
   }
   if (is_affected_obj(obj, gsn_alchemy))
   {
      send_to_char("This object is already imbued with a power you don't comprehend.\n\r", ch);
      return;
   }
   make_obj_enchanted(obj);
   af.where     = TO_WEAPON;
   af.type      = sn;
   af.level     = level;
   af.duration  = ch->level/6+5;
   af.location  = 0;
   af.modifier  = 0;
   af.bitvector = WEAPON_FLAMING;
   if (IS_WEAPON_STAT(obj, WEAPON_FLAMING))
   af.bitvector = 0;
   obj->value[3] =  29;
   affect_to_obj(obj, &af);
   act("$n imbues $p with the power of fire.", ch, obj, NULL, TO_ROOM);
   act("You imbue $p with the power of fire and it grows warm in your hands.", ch, obj, NULL, TO_CHAR);
   return;
}

void spell_ashes_to_ashes(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   int dam, modify, saves=0;

   for ( modify = 0; modify < 4; modify++)
   {
      if (saves_spell(ch, level, victim, DAM_FIRE, SAVE_DEATH) )
      saves++;
   }

   if ((!IS_NPC(victim) && saves > 0) ||
   (IS_NPC(victim) && saves > 1) ||
   !can_instakill(ch, victim))
   {
      act("$n is consumed in fire but survives.", victim, NULL, NULL, TO_ROOM);
      send_to_char("You are consumed in a raging fire!\n\r", victim);
      dam = dice(level, 10)*race_adjust(ch)/25.0;
      damage(ch, victim, saves_spell(ch, level, victim, DAM_FIRE, SAVE_SPELL) ? dam/2 : dam, sn, DAM_FIRE, TRUE);
      return;
   }
   act("$n is consumed in flames and soon nothing remains but ashes!", victim, NULL, NULL, TO_ROOM);
   send_to_char("You are consumed in flames and begin to writhe in agonizing pain as you are reduced to ashes!\n\r", victim);
   raw_kill(ch, victim);
   return;
}

void spell_raging_fire(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   AFFECT_DATA af;
   ROOM_AFFECT_DATA raf;
   CHAR_DATA *ich;

   if (is_affected(ch, sn))
   {
      send_to_char("You can't create another raging fire yet.\n\r", ch);
      return;
   }
   if (ch->in_room->sector_type != SECT_FOREST)
   {
      send_to_char("There is not enough wood here to burn.\n\r", ch);
      return;
   }
   if (room_sector_modified(ch->in_room, ch))
   return;
   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.location  = 0;
   af.modifier  = 0;
   af.duration  = 24;
   af.bitvector = 0;
   affect_to_char( ch, &af );
   send_to_char( "You raise your arms and flames shoot towards the surrounding trees!\n\r", ch );
   act("$n raises $s arms and flames shoot towards the surrounding trees!", ch, NULL, NULL, TO_ROOM);
   act("The forest blazes around you!", ch, NULL, NULL, TO_ALL);
   raf.where     = TO_ROOM_SECTOR;
   raf.sector_new = SECT_FIELD;
   raf.duration = 5;
   raf.type = sn;
   raf.level = level;
   raf.bitvector = 0;
   raf.caster = ch;
   affect_to_room(ch->in_room, &raf);
   for ( ich = ch->in_room->people; ich != NULL; ich = ich->next_in_room )
   {
      if (ich->invis_level > 0)
      continue;
      if (ich->trust > 51 )
      continue;
      if (IS_SET(ich->affected_by, AFF_CAMOUFLAGE))
      {
         affect_strip ( ich, gsn_camouflage        );
         REMOVE_BIT   ( ich->affected_by, AFF_CAMOUFLAGE );
         act( "$n is revealed as the forest burns!", ich, NULL, NULL, TO_ROOM );
         send_to_char( "You can no longer camouflage here!\n\r", ich );
      }
   }

   return;
}

void spell_charge_weapon(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   OBJ_DATA *obj = (OBJ_DATA *) vo;
   AFFECT_DATA af;

   if (obj == NULL) return;

   if (obj->item_type != ITEM_WEAPON)
   {
      send_to_char("That object cannot be imbued with lightning.\n\r", ch);
      return;
   }
   if (is_affected_obj(obj, gsn_alchemy))
   {
      send_to_char("This object is already imbued with a power you don't comprehend.\n\r", ch);
      return;
   }
   make_obj_enchanted(obj);
   af.where     = TO_WEAPON;
   af.type      = sn;
   af.level     = level;
   af.duration  = 5+ch->level/6;
   af.location  = 0;
   af.modifier  = 0;
   af.bitvector = WEAPON_SHOCKING;
   if (IS_WEAPON_STAT(obj, WEAPON_SHOCKING))
   af.bitvector = 0;
   obj->value[3] =  28;
   affect_to_obj(obj, &af);
   act("$n imbues $p with the power of lightning and it glows in $s hands.", ch, obj, NULL, TO_ROOM);
   act("You imbue $p with the power of lightning and it glows in your hands.", ch, obj, NULL, TO_CHAR);
   return;
}

void spell_tornado(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   CHAR_DATA* vch;
   CHAR_DATA* vch_next;
   char buf[MAX_STRING_LENGTH];
   int dam;
   AFFECT_DATA af;
   ROOM_AFFECT_DATA raf;

   if (check_peace(ch))
   {
      return;
   }

   if (is_affected(ch, sn))
   {
      send_to_char("You cannot call another tornado yet.\n\r", ch);
      return;
   }
   if (is_affected_room(ch->in_room, sn))
   {
      send_to_char("There is already a raging tornadic storm here.\n\r", ch);
      return;
   }
   if (!IS_OUTSIDE(ch))
   {
      send_to_char("You must be outdoors to call the tornadic winds.\n\r", ch);
      return;
   }
   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.location  = 0;
   af.modifier  = 0;
   af.duration  = 12;
   af.bitvector = 0;
   affect_to_char(ch, &af);
   send_to_char
   (
      "You call to the winds, and a tornadic storm forms around you!\n\r",
      ch
   );
   act
   (
      "$n calls to the winds, and a black tornadic storm forms above.",
      ch,
      NULL,
      NULL,
      TO_ROOM
   );

   for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
   {
      vch_next = vch->next_in_room;
      if
      (
         (
            IS_IMMORTAL(vch) &&
            !can_see(ch, vch) &&
            wizi_to(vch, ch)
         ) ||
         is_same_group(ch, vch) ||
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
            default:
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
            "[%s] cast tornado upon [%s] at %d",
            ch->name,
            vch->name,
            ch->in_room->vnum
         );
         log_string(log_buf);
      }
      if (number_range(0, 1) == 0)
      {
         act
         (
            "$n is struck by lightning.",
            vch,
            NULL,
            NULL,
            TO_ROOM
         );
         {
            send_to_char("You are struck by lightning.\n\r", vch);
         }
         if (check_spellcraft(ch, sn))
         {
            dam = spellcraft_dam(level, 7);
         }
         else
         {
            dam = dice(level, 7);
         }
         if (saves_spell(ch, level, vch, DAM_LIGHTNING, SAVE_SPELL))
         {
            dam /= 2;
         }
         damage
         (
            ch,
            vch,
            dam,
            gsn_lightning_bolt,
            DAM_LIGHTNING,
            TRUE
         );
      }
      else
      {
         int n_dice = 5;

         act
         (
            "$n is caught in the fierce winds.",
            vch,
            NULL,
            NULL,
            TO_ROOM
         );
         send_to_char("You are caught in the fierce winds.\n\r", vch);
         if (IS_FLYING(vch))
         {
            n_dice *= 2;
            act
            (
               "$n is tossed about by the winds.",
               vch,
               NULL,
               NULL,
               TO_ROOM
            );
         }
         if (check_spellcraft(ch, sn))
         {
            dam = spellcraft_dam(level, n_dice);
         }
         else
         {
            dam = dice(level, n_dice);
         }
         if (saves_spell(ch, level, vch, DAM_SLASH, SAVE_SPELL))
         {
            dam /= 2;
         }
         if
         (
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
         }
         else
         {
            damage(ch, vch, dam, sn, DAM_SLASH, TRUE);
         }
      }
   }
   raf.where     = TO_ROOM;
   raf.duration = 5;
   raf.type = sn;
   raf.level = level;
   raf.bitvector = 0;
   raf.caster = ch;
   affect_to_room(ch->in_room, &raf);
   return;
}

void spell_airshield(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   AFFECT_DATA af;

   if (is_affected(ch, sn))
   {
      send_to_char("An airshield already protects you.\n\r", ch);
      return;
   }
   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.location  = APPLY_DEX;
   af.modifier  = 2;
   af.duration  = level/5;
   af.bitvector = 0;
   affect_to_char( ch, &af );
   send_to_char("A steady current of air begins to flow about you.\n\r", ch);
   act("$n is surrounded by a steady current of air.", ch, NULL, NULL, TO_ROOM);
   return;
}

void spell_suffocate(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;
   AFFECT_DATA *af2;

   if (is_affected(victim, sn))
   {
      send_to_char("They are already deprived of air.\n\r", ch);
      return;
   }

   if (saves_spell(ch, level, victim, DAM_OTHER, SAVE_MALEDICT) ||
   saves_spell(ch, level, victim, DAM_OTHER, SAVE_MALEDICT))
   {
      send_to_char("You motion for the air to move, but it doesn't obey.\n\r", ch);
      return;
   }
   if (race_table[victim->race].breathes)
   {
      act
      (
         "With a swift motion of $s arms, $n draws the air away from you, and"
         " you begin to suffocate!",
         ch,
         NULL,
         victim,
         TO_VICT
      );
      act("With a swift motion of your arms, you draw the air away from $N.", ch, NULL, victim, TO_CHAR);

      af.where     = TO_AFFECTS;
      af.type      = sn;
      af.level     = level;
      af.duration  = 5;
      af.modifier  = 100;
      af.location  = APPLY_AC;
      af.bitvector = 0;
      affect_to_char(victim, &af);
      af.modifier  = -5;
      af.location  = APPLY_STR;
      affect_to_char(victim, &af);
      af.modifier  = -3;
      af.location  = APPLY_DEX;
      affect_to_char(victim, &af);
      af2 = affect_find(victim->affected, gsn_suffocate);
      if (af2 != NULL)
      {
         free_string(af2->caster);
         af2->caster = str_dup(ch->name);
      }
   }
   else
   {
      /* Does not breathe, just do damage */
      act
      (
         "With a swift motion of $s arms, $n forces the air to surround and "
         "crush you!",
         ch,
         NULL,
         victim,
         TO_VICT
      );
      act
      (
         "With a swift motion of your arms, you force the air to surround and "
         "crush $N!",
         ch,
         NULL,
         victim,
         TO_CHAR
      );
      damage(ch, victim, dice(level, 5) * race_adjust(ch) / 25.0, gsn_jet_stream, DAM_BASH, TRUE);
   }
   return;
}

void spell_jet_stream(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   int dam;
   int nd = 3;

   act("A jet of fast moving air flows from $n's arms and strikes $N.", ch, NULL, victim, TO_NOTVICT);
   act("A jet of fast moving air flows from $n's arms and strikes you.", ch, NULL, victim, TO_VICT);
   act("You motion at $N and release a jet of fast moving air to strike at $M.", ch, NULL, victim, TO_CHAR);

   if ((has_skill(victim, gsn_wraithform) && get_skill(victim, gsn_wraithform) >= 75)
   || (IS_NPC(victim) && victim->pIndexData->vnum == MOB_VNUM_AIR_ELEMENTAL))
   {
      act("$n seems amused by the winds.", victim, NULL, NULL, TO_ROOM);
      return;
   }

   if (level > 30) nd++;
   if (level > 40) nd++;

   if (check_spellcraft(ch, sn))
   dam = spellcraft_dam(level, nd);
   else
   dam = dice(level, nd);

   if ( saves_spell(ch, level, victim, DAM_BASH, SAVE_SPELL))
   dam /= 2;

   if (IS_FLYING(victim))
   {
      dam += dice(level, nd/2);
      if (!saves_spell(ch, level, victim, DAM_BASH, SAVE_SPELL) )
      {
         affect_strip(victim, gsn_fly);
         if (!IS_FLYING(victim))
         {
            act("$n is knocked to the ground by the blast of air!", victim, NULL, NULL, TO_ROOM);
            send_to_char("You are knocked to the ground by the blast of air!\n\r", victim);
         }
      }
   }

   damage( ch, victim, dam*race_adjust(ch)/25.0, sn, DAM_BASH, TRUE);
   return;
}

void spell_cyclone(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   int dam;
   AFFECT_DATA af;

   if (check_peace(ch)) return;

   af.where = TO_AFFECTS;
   af.location = APPLY_HITROLL;
   af.modifier = -3;
   af.duration = 1;
   af.level = level;
   af.type = sn;
   af.bitvector = AFF_BLIND;

   act("You raise a violent wall of wind to strike $N.", ch, NULL, victim, TO_CHAR);
   act("$n raises a violent wall of wind, sending debris flying at $N!", ch, NULL, victim, TO_ROOM);
   act("$n raises a violent wall of wind, sending debris flying at you!", ch, NULL, victim, TO_VICT);

   if ((has_skill(victim, gsn_wraithform) && get_skill(victim, gsn_wraithform) >= 75)
   || (IS_NPC(victim) && victim->pIndexData->vnum == MOB_VNUM_AIR_ELEMENTAL))
   {
      act("$n seems amused by the winds.", victim, NULL, NULL, TO_ROOM);
      return;
   }

   if (check_spellcraft(ch, sn))
   dam = spellcraft_dam(level, 6);
   else
   dam = dice(level, 6);

   if ((number_range(0, 1) == 0) && !saves_spell(ch, level, victim, DAM_BASH, SAVE_SPELL))
   {
      act("$n appears blinded by the debris.", victim, NULL, NULL, TO_ROOM);
      affect_to_char(victim, &af);
   }
   if (IS_FLYING(victim))
   {
      if (!saves_spell(ch, level, victim, DAM_BASH, SAVE_SPELL) )
      {
         act("$n is thrown wildly to the ground by the air blast!", victim, NULL, NULL, TO_ROOM);
         send_to_char("You are thrown down by the air blast!\n\r", victim);
         affect_strip(victim, gsn_fly);
      }
   }
   damage(ch, victim, dam, sn, DAM_BASH, TRUE);
   return;
}

void spell_implosion(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   int dam;

   act("$N is crushed by the pressure of the air around $M.", ch, NULL, victim, TO_NOTVICT);
   act("$n waves $s arms, and you suddenly feel crushed by the air around you!", ch, NULL, victim, TO_VICT);
   act("You force the air around $N to crush $M.", ch, NULL, victim, TO_CHAR);

   if ((has_skill(victim, gsn_wraithform) && get_skill(victim, gsn_wraithform) >= 75)
   || (IS_NPC(victim) && victim->pIndexData->vnum == MOB_VNUM_AIR_ELEMENTAL))
   {
      act("$n seems amused by the winds.", victim, NULL, NULL, TO_ROOM);
      return;
   }

   if (check_spellcraft(ch, sn))
   dam = spellcraft_dam(level, 13);
   else
   dam = dice(level, 13);


   if ( saves_spell(ch, level, victim, DAM_BASH, SAVE_SPELL))
   dam /= 2;

   damage( ch, victim, dam*race_adjust(ch)/25.0, sn, DAM_BASH, TRUE);
   return;
}

void spell_thunder(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;
   char buf[MAX_STRING_LENGTH];
   int dam;

   if (check_peace(ch)) return;

   act("A deafening clap of thunder erupts from the air.", ch, NULL, NULL, TO_ALL);
   dam = dice(level, 5)*race_adjust(ch)/25.0;
   for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
   {
      vch_next = vch->next_in_room;
      if (is_same_group(vch, ch))
      continue;
      if (IS_IMMORTAL(vch) && !can_see(ch, vch) && wizi_to(vch, ch)) continue;
      if (is_safe(ch, vch, IS_SAFE_SILENT | IS_SAFE_AREA_ATTACK))
      continue;

      if (oblivion_blink(ch, vch))
      {
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
         if (vch != ch)
         {
            do_myell(vch, buf);
            sprintf(log_buf, "[%s] cast thunder upon [%s] at %d", ch->name, vch->name, ch->in_room->vnum);
            log_string(log_buf);
         }
      }
      if (saves_spell(ch, level, vch, DAM_SOUND, SAVE_SPELL) )
      damage(ch, vch, dam/2, sn, DAM_SOUND, TRUE);
      else
      damage(ch, vch, dam, sn, DAM_SOUND, TRUE);
   }
   return;
}

void spell_mass_fly(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;

   send_to_char("You motion to the heavens and all around begin to magically fly.\n\r", ch);

   for ( vch = ch->in_room->people; vch != NULL; vch = vch_next )
   {
      vch_next = vch->next_in_room;
      if (IS_IMMORTAL(vch) && !can_see(ch, vch)) continue;
      if
      (
         ch != vch &&
         is_safe(ch, vch, IS_SAFE_SILENT | IS_SAFE_AREA_ATTACK)
      )
      {
         continue;
      }
      if (oblivion_blink(ch, vch))
      {
         continue;
      }
      if (is_affected(vch, gsn_earthbind))
      {
         affect_strip(vch, gsn_earthbind);
         send_to_char("You are no longer earthbound!\n\r", vch);
      }
      magic_spell_vict
      (
         ch,
         vch,
         CAST_BITS_REUSE,
         level,
         gsn_fly
      );
   }
   return;
}

void spell_air_dagger(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   int dam;

   if (check_peace(ch)) return;

   if (check_spellcraft(ch, sn))
   dam = spellcraft_dam(level, level/6);
   else
   dam = dice(level, level/6);

   if (saves_spell(ch, level, victim, DAM_SLASH, SAVE_SPELL))
   dam /= 2;
   damage(ch, victim, dam, sn, DAM_SLASH, TRUE);
   return;
}

void spell_gale_winds(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   int dam;

   if (check_peace(ch)) return;


   dam = dice(level, 3);

   act("Strong frigid winds strike $N and push $M away!", ch, NULL, victim, TO_NOTVICT);
   act("Strong frigid winds strike you and push you away!", ch, NULL, victim, TO_VICT);
   act("You call strong frigid winds to push $N away.", ch, NULL, victim, TO_CHAR);
   if (saves_spell(ch, level, victim, DAM_BASH, SAVE_SPELL) ||
   saves_spell(ch, level, victim, DAM_BASH, SAVE_SPELL) ||
   (IS_NPC(victim) && victim->house != 0))
   {
      act("$N is pushed by the winds but holds $S ground!", ch, NULL, victim, TO_CHAR);
      send_to_char("You are pushed by the winds but hold your ground!\n\r", victim);
      damage(ch, victim, dam, sn, DAM_COLD, TRUE);
      return;
   }

   damage(ch, victim, dam, sn, DAM_COLD, TRUE);
   act("$N is pushed away by the winds!", ch, NULL, victim, TO_CHAR);
   send_to_char("You are pushed away by the winds!\n\r", victim);
   if (victim == NULL || victim->in_room != ch->in_room)
   return;
   if (victim->position == POS_FIGHTING)
   do_flee(victim, "");

   if (victim->position == POS_FIGHTING)
   {
      do_flee(victim, "");
   }
   if (victim->position == POS_FIGHTING)
   {
      do_flee(victim, "");
   }
   return;
}

void spell_dust_devil(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   int dam;
   AFFECT_DATA af;

   if (check_peace(ch)) return;

   af.where = TO_AFFECTS;
   af.location = APPLY_HITROLL;
   af.modifier = -3;
   af.duration = 0;
   af.level = level;
   af.type = sn;
   af.bitvector = AFF_BLIND;

   if (check_spellcraft(ch, sn))
   dam = spellcraft_dam(level, 4);
   else
   dam = dice(level, 4);

   if ((number_range(0, 3) == 0) && !saves_spell(ch, level, victim, DAM_BASH, SAVE_SPELL))
   {
      act("$n appears blinded by dust.", victim, NULL, NULL, TO_ROOM);
      affect_to_char(victim, &af);
   }
   damage(ch, victim, dam, sn, DAM_BASH, TRUE);
   return;
}

void spell_vortex(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim;
   CHAR_DATA *group;
   CHAR_DATA *arrival;
   CHAR_DATA *g_next;
   CHAR_DATA *last_to_venue;
   int numb;
   bool gate_pet;

   if (is_affected(ch, gsn_trapstun))
   {
      send_to_char("You can't use vortex you're in a snare.\n\r", ch);
      return;
   }

   if
   (
      IS_SET(ch->in_room->extra_room_flags, ROOM_DEAD_ZONE) &&
      !IS_SET(ch->in_room->room_flags, ROOM_CONSECRATED)
   )
   {
      send_to_char("You cannot create a vortex here.\n\r", ch);
      return;
   }
   if (is_affected(ch, gsn_purity))
   {
      send_to_char
      (
         "The mark of pure blood on you breaks down your vortex!\n\r",
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
   if (victim == ch || victim == NULL)
   {
      send_to_char("You failed.\n\r", ch);
      return;
   }
   if (!IS_OUTSIDE(ch) || !IS_OUTSIDE(victim))
   {
      send_to_char("You can't use a vortex to travel from here to there.\n\r", ch);
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
   || (!IS_NPC(victim) && saves_spell(ch, level, victim, DAM_OTHER, SAVE_TRAVEL))
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

   if (ch->pet != NULL && ch->in_room == ch->pet->in_room)
   gate_pet = TRUE;
   else
   gate_pet = FALSE;

   numb = 1;
   for (group = ch->in_room->people; group != NULL; group = g_next)
   {
      g_next = group->next_in_room;
      if ( group->fighting != NULL )
      continue;
      if (group == ch )
      continue;
      if ( !IS_NPC(group)
      || !IS_AFFECTED(group, AFF_CHARM)
      ||  group->master == NULL
      ||  group->master != ch)
      continue;
      if (oblivion_blink(ch, group))
      {
         continue;
      }

      numb++;
      un_camouflage(group, NULL);
      un_earthfade(group, NULL);
      un_hide(group, NULL);
      un_forest_blend(group); /* - Wicket */
      if (ch->is_riding != NULL &&
      (group != ((CHAR_DATA *) ch->is_riding->mount))){
         send_to_char("You leave in a vortex of swirling winds.\n\r", group);
         act("$n leaves in a vortex of swirling winds.", group, NULL, NULL, TO_ROOM);
      }
      char_from_room(group);
      char_to_room(group, victim->in_room);
      if (ch->is_riding != NULL &&
      (group != ((CHAR_DATA *) ch->is_riding->mount)))
      act("$n arrives riding in a vortex of swirling air.", group, NULL, NULL, TO_ROOM);
      last_to_venue = group;
   }

   if (gate_pet)
   {
      group = ch->pet;
      un_camouflage(group, NULL);
      un_earthfade(group, NULL);
      un_hide(group, NULL);
      un_forest_blend(group); /* - Wicket */
      send_to_char("You leave in a vortex of swirling winds.\n\r", group);
      act("$n leaves in a vortex of swirling winds.", group, NULL, NULL, TO_ROOM);
      char_from_room(group);
      char_to_room(group, victim->in_room);
      act("$n arrives riding in a vortex of swirling air.", group, NULL, NULL, TO_ROOM);
      last_to_venue = group;
      numb++;
   }
   un_camouflage(ch, NULL);
   un_hide(ch, NULL);
   un_earthfade(ch, NULL);
   un_forest_blend(ch); /* - Wicket */

   if (ch->is_riding == NULL || ch->is_riding->mount == NULL){
      act("$n leaves in a vortex of swirling winds.", ch, NULL, NULL, TO_ROOM);
      send_to_char("You depart in a vortex of swirling wind.\n\r", ch);
      char_from_room(ch);
      char_to_room(ch, victim->in_room);
      act("$n arrives riding upon a vortex of swirling air.", ch, NULL, NULL, TO_ROOM);
   }
   else
   {
      act("$n leaves upon $N in a vortex of swirling air.", ch, NULL, (CHAR_DATA *) ch->is_riding->mount, TO_ROOM);
      send_to_char("You depart in a vortex of swirling wind.\n\r", ch);
      char_from_room(ch);
      char_to_room(ch, victim->in_room);
      act("$n arrives in a vortex of swirling air riding upon $N.", ch, NULL, (CHAR_DATA *) ch->is_riding->mount, TO_ROOM);
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

void spell_stone(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   CHAR_DATA *check = NULL;
   AFFECT_DATA af;

   if (is_affected(victim, gsn_stone))
   {
      send_to_char("They have already been turned to stone.\n\r", ch);
      return;
   }
   if
   (
      (
         IS_NPC(victim) &&
         victim->house != 0
      ) ||
      saves_spell(ch, level - 1, victim, DAM_OTHER, SAVE_MALEDICT) ||
      saves_spell(ch, level - 1, victim, DAM_OTHER, SAVE_MALEDICT)
   )
   {
      act( "You attempt to turn $N to stone, but fail.", ch, NULL, victim, TO_CHAR );
      send_to_char("You feel a painful lurch in your stomach as though you swallowed a stone, but thats all.\n\r", victim);
      return;
   }

   af.where = TO_AFFECTS;
   af.location = 0;
   af.modifier = 0;
   af.duration = 4;
   af.bitvector = 0;
   af.level = level;
   af.type = sn;
   affect_to_char(victim, &af);

   for (check = victim->in_room->people; check != NULL; check = check->next_in_room)
   if (check->fighting == victim)
   {
      check->fighting = NULL;
      check->position = POS_STANDING;
   }
   victim->fighting = NULL;

   send_to_char("You feel a sudden pain as your body solidifies into rock.\n\r", victim);
   act("$n is frozen in a final gasp as they turn to stone!", victim, NULL, NULL, TO_ROOM);
   stop_fighting(victim, FALSE);
   return;
}

void spell_stalagmite(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   int dam;
   int nd = 5;

   if (check_peace(ch)) return;

   if (ch->in_room->sector_type != SECT_UNDERGROUND )
   {
      send_to_char("You cannot create a stalagmite here.\n\r", ch);
      return;
   }

   act("A stalagmite erupts from the ground!", ch, NULL, NULL, TO_ROOM);

   if (level > 25) nd++;
   if (level > 30) nd++;
   if (level > 35) nd++;

   if (check_spellcraft(ch, sn))
   dam = spellcraft_dam(level, nd);
   else
   dam = dice(level, nd);

   if (saves_spell(ch, level, victim, DAM_BASH, SAVE_SPELL))
   dam /= 2;

   damage(ch, victim, dam, sn, DAM_BASH, TRUE);
   return;
}

void spell_tremor(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
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
   int dam;

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
      send_to_char("You can't cause a tremor here.\n\r", ch);
      return;
   }
   send_to_char( "The earth trembles beneath your feet!\n\r", ch );
   act( "$n makes the earth tremble and shiver beneath $N.", ch, NULL, victim, TO_ROOM );

   if (is_affected(victim, gsn_earthfade))
   dam += dam/3;
   if (IS_FLYING(victim))
   damage(ch, victim, 0, sn, DAM_BASH, TRUE);
   else
   damage( ch, victim, dam*race_adjust(ch)/25.0, sn, DAM_BASH, TRUE);
   return;
}

void spell_avalanche(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   CHAR_DATA* vch;
   CHAR_DATA* vch_next;
   char buf[MAX_STRING_LENGTH];
   int dam;
   int true_dam;

   if (check_peace(ch))
   {
      return;
   }

   if
   (
      ch->in_room->sector_type != SECT_MOUNTAIN &&
      ch->in_room->sector_type != SECT_HILLS
   )
   {
      send_to_char("You cannot cause an avalanche here.\n\r", ch);
      return;
   }
   send_to_char("You stomp and the hillside begins coming down!\n\r", ch);
   act
   (
      "$n stomps once and a loud rumble is heard from above...",
      ch,
      NULL,
      NULL,
      TO_ROOM
   );

   if (check_spellcraft(ch, sn))
   {
      dam = spellcraft_dam(level, 8);
   }
   else
   {
      dam = dice(level, 8);
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
         )
      )
      {
         switch (number_range(0, 2))
         {
            default:
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
            "[%s] cast avalanche upon [%s] at %d",
            ch->name,
            vch->name,
            ch->in_room->vnum
         );
         log_string(log_buf);
      }
      true_dam = dam;
      if (saves_spell(ch, level, vch, DAM_BASH, SAVE_SPELL))
      {
         dam /= 2;
      }
      damage(ch, vch, dam, sn, DAM_BASH, TRUE);
      dam = true_dam;  /* to avoid weird buildups in large groups */
   }
   return;
}

void spell_cave_in(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   CHAR_DATA* vch;
   CHAR_DATA* vch_next;
   char buf[MAX_STRING_LENGTH];
   int dam;
   int true_dam;
   AFFECT_DATA af;
   AFFECT_DATA* af2;

   if (check_peace(ch))
   {
      return;
   }

   if (ch->in_room->sector_type != SECT_UNDERGROUND
   /* && ch->in_room->sector_type != SECT_INSIDE*/)
   {
      send_to_char("You can't cause a cave-in here.\n\r", ch);
      return;
   }

   send_to_char("You stomp and the caverns begin to collapse!\n\r", ch);
   act
   (
      "$n stomps once and a loud rumble is heard from above...",
      ch,
      NULL,
      NULL,
      TO_ROOM
   );

   if (check_spellcraft(ch, sn))
   {
      dam = spellcraft_dam(level, 9);
   }
   else
   {
      dam = dice(level, 9);
   }

   for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
   {
      vch_next = vch->next_in_room;
      if (is_same_group(vch, ch))
      {
         continue;
      }
      if
      (
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
      if
      (
         !IS_NPC(ch)  &&
         !IS_NPC(vch) &&
         (
            ch->fighting == NULL ||
            vch->fighting == NULL
         )
      )
      {
         switch(number_range(0, 2))
         {
            default:  /* (0) | (1) */
            sprintf(buf, "Die, %s, you sorcerous dog!", PERS(ch, vch));
            break;
            case (2):
            sprintf
            (
               buf,
               "Help! %s is casting a spell on me!",
               PERS(ch, vch)
            );
            break;
         }
         if (vch != ch)
         {
            do_myell(vch, buf);
            sprintf
            (
               log_buf,
               "[%s] cast cave-in upon [%s] at %d",
               ch->name,
               vch->name,
               ch->in_room->vnum
            );
            log_string(log_buf);
         }
      }

      true_dam = dam;
      if (saves_spell(ch, level, vch, DAM_BASH, SAVE_SPELL))
      {
         dam /= 2;
      }
      else
      {
         if
         (
            !saves_spell(ch, level, vch, DAM_BASH, SAVE_MALEDICT) &&
            !is_affected(vch, gsn_suffocate)
         )
         {
            if (race_table[vch->race].breathes)
            {
               af.where     = TO_AFFECTS;
               af.type      = gsn_suffocate;
               af.level     = level;
               af.duration  = 2;
               af.modifier  = 100;
               af.location  = APPLY_AC;
               af.bitvector = 0;
               affect_to_char(vch, &af);
               af.modifier  = -5;
               af.location  = APPLY_STR;
               affect_to_char(vch, &af);
               af.modifier  = -3;
               af.location  = APPLY_DEX;
               affect_to_char(vch, &af);
               af2 = affect_find(vch->affected, gsn_suffocate);
               if (af2 != NULL)
               {
                  free_string(af2->caster);
                  af2->caster = str_dup(ch->name);
               }
               send_to_char("You are suffocated by the falling rocks!\n\r", vch);
               act
               (
                  "$n seems to be suffocating beneath the falling rocks.",
                  vch,
                  NULL,
                  NULL,
                  TO_ROOM
               );
            }
            else
            {
               /* Does not breathe, just do damage */
               act
               (
                  "The rocks pummel you mercilessly!",
                  ch,
                  NULL,
                  vch,
                  TO_VICT
               );
               act
               (
                  "The rocks pummel $n mercilessly!",
                  vch,
                  NULL,
                  NULL,
                  TO_ROOM
               );
               dam += dice(level, 3);
            }
         }
      }
      damage(ch, vch, dam, sn, DAM_BASH , TRUE);
      dam = true_dam; /* to avoid weird buildups in large groups */
   }
   return;
}

void spell_crushing_hands(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   int dam;

   act("A huge fist of earth forms and crushes $N.", ch, NULL, victim, TO_NOTVICT);
   act("A huge fist of earth forms and crushes you!", ch, NULL, victim, TO_VICT);
   act("A huge fist of earth forms and crushes $N.", ch, NULL, victim, TO_CHAR);


   if (check_spellcraft(ch, sn))
   dam = spellcraft_dam(level, 7);
   else
   dam = dice(level, 7);


   if ( saves_spell(ch, level, victim, DAM_BASH, SAVE_SPELL))
   {
      act("$n manages to keep $s balance.\n\r", victim, NULL, NULL, TO_ROOM);
      act("You manage to keep your balance.\n\r", ch, NULL, victim, TO_VICT);
      dam /= 2;
   }
   else if (victim->daze <= 0)
   {
      act("$n is knocked off balance!\n\r", victim, NULL, NULL, TO_ROOM);
      act("You are knocked off balance.\n\r", ch, NULL, victim, TO_VICT);
      DAZE_STATE(victim, 18);
   }
   damage( ch, victim, dam*race_adjust(ch)/25.0, sn, DAM_BASH, TRUE);
   return;
}

void spell_shield_of_earth(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   AFFECT_DATA af;

   if (is_affected(ch, sn))
   {
      send_to_char("You are already protected by a shield of earth.\n\r", ch);
      return;
   }
   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.location  = APPLY_HIT;
   af.modifier  = 150;
   af.duration  = 3+level/5;
   af.bitvector = 0;
   affect_to_char( ch, &af );
   send_to_char("Your body turns to hard stone.\n\r", ch);
   act("$n's body turns to hard stone.", ch, NULL, NULL, TO_ROOM);
   ch->hit += 150;
   if (ch->hit > ch->max_hit) ch->hit = ch->max_hit;

   return;
}

void spell_stoney_grasp(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;
   sh_int lev_mod;

   if (is_affected(victim, sn))
   {
      send_to_char("They are already being held by stoney arms.\n\r", ch);
      return;
   }

   if
   (
      ch->in_room->sector_type == SECT_WATER_SWIM ||
      ch->in_room->sector_type == SECT_WATER_NOSWIM ||
      ch->in_room->sector_type == SECT_UNDERWATER ||
      ch->in_room->sector_type == SECT_AIR
   )
   {
      send_to_char("You cannot call the hands of stone here.\n\r", ch);
      return;
   }
   if
   (
      saves_spell(ch, level, victim, DAM_OTHER, SAVE_MALEDICT) ||
      (
         (
            lev_mod = number_range(0, 10)
         ) &&
         saves_spell(ch, level + lev_mod, victim, DAM_OTHER, SAVE_MALEDICT)
      )
   )
   {
      send_to_char("The hands form but fail to grasp.\n\r", ch);
      return;
   }

   if (ch == victim)
   {
      send_to_char("A pair of stoney hands erupt from the earth and grasp your legs!\n\r", ch);
   }
   else
   {
      act
      (
         "A pair of stoney hands erupt from the earth and grasp your legs!",
         ch,
         NULL,
         victim,
         TO_VICT
      );
   }
   act
   (
      "A pair of stoney hands erupt from the earth and grasp $n's legs!",
      victim,
      NULL,
      NULL,
      TO_ROOM
   );

   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.duration  = 5;
   af.modifier  = 100;
   af.location  = APPLY_AC;
   af.bitvector = 0;
   affect_to_char(victim, &af);
   return;
}


void do_burrow(CHAR_DATA *ch, char *argument)
{
   AFFECT_DATA af;
   char arg[MAX_INPUT_LENGTH];

   if
   (
      !has_skill(ch, gsn_burrow) ||
      get_skill(ch, gsn_burrow) <= 1
   )
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }
   if (!is_affected(ch, gsn_earthfade))
   {
      send_to_char("But you are not earthfaded!\n\r", ch);
      return;
   }

   argument = one_argument(argument, arg);
   if (arg[0] == '\0')
   {
      send_to_char("You must select a direction to burrow.\n\r", ch);
      return;
   }

   if (number_percent() > get_skill(ch, gsn_burrow))
   {
      send_to_char("You try to dig but fail.\n\r", ch);
      check_improve(ch, gsn_burrow, FALSE, 1);
      return;
   }
   check_improve(ch, gsn_burrow, TRUE, 1);

   WAIT_STATE(ch, 12);
   af.where     = TO_AFFECTS;
   af.type      = gsn_burrow;
   af.level     = ch->level;
   af.duration  = 0;
   af.modifier  = 0;
   af.location  = 0;
   af.bitvector = 0;
   affect_to_char(ch, &af); /* makes move char leave earthfaded */

   if (!str_prefix(arg, "north"))
   {
      do_north(ch, "");
   }
   else if (!str_prefix(arg, "south"))
   {
      do_south(ch, "");
   }
   else if (!str_prefix(arg, "west"))
   {
      do_west(ch, "");
   }
   else if (!str_prefix(arg, "east"))
   {
      do_east(ch, "");
   }
   else if (!str_prefix(arg, "up"))
   {
      do_up(ch, "");
   }
   else if (!str_prefix(arg, "down"))
   {
      do_down(ch, "");
   }
   else
   {
      send_to_char("You must select a direction to burrow.\n\r", ch);
      return;
   }
   affect_strip(ch, gsn_burrow);
   return;
}

void spell_meteor_storm(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   ROOM_AFFECT_DATA raf;

   if (!IS_OUTSIDE(ch))
   {
      send_to_char("You cannot call a meteor storm indoors.\n\r", ch);
      return;
   }
   if (is_affected_room(ch->in_room, sn))
   {
      send_to_char("There is already a meteor storm here.\n\r", ch);
      return;
   }
   act("The sky turns ominously dark.", ch, NULL, NULL, TO_ALL);
   raf.where = TO_ROOM_EXTRA;
   raf.duration = number_range(6, 9);
   raf.type = sn;
   raf.level = level;
   raf.bitvector = ROOM_METEOR_STORM;
   raf.caster = ch;
   affect_to_room(ch->in_room, &raf);
   return;
}

void spell_geyser(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   int dam;
   int dtype = DAM_DROWNING;


   if (number_range(1, 3) == 1)
   dtype = DAM_FIRE;
   if (dtype == DAM_DROWNING)
   {
      act("A towering geyser erupts from the ground beneath $N.", ch, NULL, victim, TO_ROOM);
      act("A towering geyser erupts from the ground beneath $N.", ch, NULL, victim, TO_CHAR);
      act("A towering geyser erupts from the ground beneath you.", ch, NULL, victim, TO_VICT);
   }
   else
   {
      act("A steaming hot geyser erupts from the ground beneath $N.", ch, NULL, victim, TO_ROOM);
      act("A steaming hot geyser erupts from the ground beneath $N.", ch, NULL, victim, TO_CHAR);
      act("A steaming hot geyser erupts from the ground beneath you.", ch, NULL, victim, TO_VICT);
   }
   if (check_spellcraft(ch, sn))
   dam = spellcraft_dam(level, 5);
   else
   dam = dice(level, 5);


   if ( saves_spell(ch, level, victim, dtype, SAVE_SPELL))
   dam /= 2;

   damage( ch, victim, dam*race_adjust(ch)/25.0, sn, dtype, TRUE);
   return;
}

void spell_water_spout(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;
   char buf[MAX_STRING_LENGTH];
   int dam;
   AFFECT_DATA af;
   ROOM_AFFECT_DATA raf;


   if (check_peace(ch)) return;

   if (is_affected(ch, sn))
   {
      send_to_char("You can't call another water spout yet.\n\r", ch);
      return;
   }
   if (is_affected_room(ch->in_room, sn))
   {
      send_to_char("There is already a water spout here.\n\r", ch);
      return;
   }
   if ( !IS_OUTSIDE(ch) ||
   (ch->in_room->sector_type != SECT_WATER_SWIM &&
   ch->in_room->sector_type != SECT_WATER_NOSWIM))
   {
      send_to_char("You must be near water to create a water spout.\n\r", ch);
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
   send_to_char("You raise your arms, and a water spout forms on the horizon.\n\r", ch);
   act("$n raises $s arms, and a water spout forms on the horizon and approaches rapidly.", ch, NULL, NULL, TO_ROOM);

   for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
   {
      vch_next = vch->next_in_room;
      if (IS_IMMORTAL(vch) && !can_see(ch, vch) && wizi_to(vch, ch)) continue;
      if (is_safe(ch, vch, IS_SAFE_SILENT | IS_SAFE_AREA_ATTACK))
      continue;
      if (is_same_group(ch, vch))
      continue;
      if (oblivion_blink(ch, vch))
      {
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
         if (vch != ch)
         {
            do_myell(vch, buf);
            sprintf(log_buf, "[%s] cast water spout upon [%s] at %d", ch->name, vch->name, ch->in_room->vnum);
            log_string(log_buf);
         }
      }
      if (number_range(0, 1) == 0)
      {
         act("$n is deluged by huge waves.", vch, NULL, NULL, TO_ROOM);
         send_to_char("You are deluged by huge waves.\n\r", vch);
         if (check_spellcraft(ch, sn))
         dam = spellcraft_dam(level, 7);
         else
         dam = dice(level, 7);
         if (saves_spell(ch, level, vch, DAM_DROWNING, SAVE_SPELL))
         dam /= 2;
         damage(ch, vch, dam, gsn_tsunami, DAM_DROWNING, TRUE);
      }
      else
      {
         int n_dice = 5;
         act("$n is caught in the fierce winds.", vch, NULL, NULL, TO_ROOM);
         send_to_char("You are caught in the fierce winds.\n\r", vch);
         if (IS_FLYING(vch))
         {
            n_dice *= 2;
            act("$n is tossed about by the winds.", vch, NULL, NULL, TO_ROOM);
         }
         if (check_spellcraft(ch, sn))
         dam = spellcraft_dam(level, n_dice);
         else
         dam = dice(level, n_dice);
         if (saves_spell(ch, level, vch, DAM_SLASH, SAVE_SPELL))
         dam /= 2;
         if ((has_skill(vch, gsn_wraithform) && get_skill(vch, gsn_wraithform) >= 75)
         || (IS_NPC(vch) && vch->pIndexData->vnum == MOB_VNUM_AIR_ELEMENTAL))
         {
            act("$n seems amused by the winds.", vch, NULL, NULL, TO_ROOM);
         }
         else
         {
            damage(ch, vch, dam, sn, DAM_SLASH, TRUE);
         }
      }
   }
   raf.where     = TO_ROOM;
   raf.duration = 5;
   raf.type = sn;
   raf.level = level;
   raf.bitvector = 0;
   raf.caster = ch;
   affect_to_room(ch->in_room, &raf);
   return;
}

void spell_deluge(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   int dam;

   if (ch->in_room->sector_type != SECT_WATER_SWIM &&
   ch->in_room->sector_type != SECT_WATER_NOSWIM &&
   ch->in_room->sector_type != SECT_UNDERWATER)
   {
      send_to_char("There is not enough water here.\n\r", ch);
      return;
   }
   act("A huge wave rises up and deluges $N.", ch, NULL, victim, TO_ROOM);
   act("A huge wave rises up and deluges $N.", ch, NULL, victim, TO_CHAR);
   act("A huge wave rises up and deluges you.", ch, NULL, victim, TO_VICT);

   if (check_spellcraft(ch, sn))
   dam = spellcraft_dam(level, 7);
   else
   dam = dice(level, 7);


   if ( saves_spell(ch, level, victim, DAM_DROWNING, SAVE_SPELL))
   dam /= 2;

   damage( ch, victim, dam*race_adjust(ch)/25.0, sn, DAM_DROWNING, TRUE);
   return;
}

void spell_whirlpool(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   CHAR_DATA* vch;
   CHAR_DATA* vch_next;
   char buf[MAX_STRING_LENGTH];
   static const sh_int dam_each[] =
   {
      0,
      0,   0,   0,   0,   0,     0,   0,   0,   0,   0,
      0,   0,   0,   0,   30,    35,  40,  45,  50,  55,
      60,  65,  70,  75,  80,    82,  84,  86,  88,  90,
      92,  94,  96,  98,  100,   102, 104, 106, 108, 110,
      112, 114, 116, 118, 120,   122, 124, 126, 128, 130,
      132, 134, 136, 138, 140,   142, 144, 146, 148, 150
   };
   int dam, dam2;

   if (check_peace(ch))
   {
      return;
   }

   level = UMIN(level, (int)(sizeof(dam_each) / sizeof(dam_each[0]) - 1));
   level = UMAX(0, level);
   dam   = number_range( dam_each[level] / 2, dam_each[level] * 2 );
   if (dam > 300)
   {
      dam = 300;
   }

   if (check_spellcraft(ch, sn))
   {
      dam += spellcraft_dam(level, 3);
   }
   else
   {
      dam += dice(level, 3);
   }

   dam *= 7.0 / 10.0;
   if
   (
      ch->in_room->sector_type != SECT_WATER_SWIM &&
      ch->in_room->sector_type != SECT_WATER_NOSWIM &&
      ch->in_room->sector_type != SECT_UNDERWATER
   )
   {
      send_to_char("There is not enough water here.\n\r", ch);
      return;
   }
   send_to_char("The water begins to rotate beneath you!\n\r", ch);
   act("$n causes a humongous whirlpool to form.", ch, NULL, NULL, TO_ROOM);

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
            default:
            {
               sprintf(buf, "Die, %s, you sorcerous dog!", PERS(ch, vch));
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
            "[%s] cast whirlpool upon [%s] at %d",
            ch->name,
            vch->name,
            ch->in_room->vnum
         );
         log_string(log_buf);
      }
      dam2 = dam;
      if
      (
         IS_FLYING(vch) &&
         ch->in_room->sector_type != SECT_UNDERWATER
      )
      {
         damage(ch, vch, 0, sn, DAM_DROWNING, TRUE);
      }
      else
      {
         if (capsize_boat(vch))
         {
            dam2 *= 3;
            dam2 /= 2;
         }
         damage
         (
            ch,
            vch,
            dam2 * race_adjust(ch) / 25.0,
            sn,
            DAM_DROWNING,
            TRUE
         );
      }
      continue;
   }
   return;
}

void spell_blizzard(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;
   char buf[MAX_STRING_LENGTH];
   AFFECT_DATA af;

   static const sh_int dam_each[] =
   {
      0,
      0, 0, 0, 0, 0,  0, 0, 0, 0, 0,
      0, 0, 0, 0, 0,  0, 0, 0, 0, 0,
      0, 0, 0, 0, 0,  0, 0, 0, 0, 0,
      0, 90, 92, 94, 96,      98, 100, 102, 105, 130,
      132, 134, 136, 138, 150,        152, 154, 156, 158, 170,
      172, 173, 174, 175, 176,    177, 178, 179, 180, 181
   };
   int dam, tmp_dam;

   if (check_peace(ch)) return;

   if (!IS_OUTSIDE(ch))
   {
      send_to_char("You must be outdoors to call a blizzard.\n\r", ch);
      return;
   }
   act("A sudden storm forms and you are pelted with ice and sleet!", ch, NULL, NULL, TO_ROOM);
   send_to_char("A blizzard forms at your command!\n\r", ch);

   level = UMAX(0, level)*race_adjust(ch)/25.0;
   level = UMIN(level, (int)(sizeof(dam_each)/sizeof(dam_each[0]) - 1));
   level = UMAX(0, level);
   dam = number_range(dam_each[level]/2, dam_each[level]*2);
   if (dam > 362) dam = 362;
   if (dam < 100) dam = 100;
   if (check_spellcraft(ch, sn))
   dam += spellcraft_dam(level/2, 3);
   else
   dam += dice(level/2, 3);

   for (vch = ch->in_room->people;vch != NULL; vch = vch_next)
   {
      vch_next = vch->next_in_room;
      if (is_same_group(vch, ch))
      continue;
      if (IS_IMMORTAL(vch) && !can_see(ch, vch) && wizi_to(vch, ch)) continue;
      if (is_safe(ch, vch, IS_SAFE_SILENT | IS_SAFE_AREA_ATTACK)) continue;
      if (oblivion_blink(ch, vch))
      {
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
         if (vch != ch)
         {
            do_myell(vch, buf);
            sprintf(log_buf, "[%s] cast blizzard upon [%s] at %d", ch->name, vch->name, ch->in_room->vnum);
            log_string(log_buf);
         }
      }

      if (saves_spell(ch, level, vch, DAM_COLD, SAVE_SPELL) )
      {
         tmp_dam = dam/2;
      }
      else
      {
         tmp_dam = dam;
         if (number_range(1, 3) == 1 && !is_affected(vch, gsn_blizzard))
         {
            af.where     = TO_AFFECTS;
            af.type      = sn;
            af.level     = level;
            af.duration  = 3;
            af.modifier  = -5;
            af.location  = APPLY_STR;
            af.bitvector = 0;
            affect_to_char(vch, &af);
            af.modifier  = 30;
            af.location  = APPLY_AC;
            affect_to_char(vch, &af);
            send_to_char("You are chilled to the bone.\n\r", vch);
            act("$n seems chilled to the bone.", vch, NULL, NULL, TO_ROOM);
         }
      }
      damage(ch, vch, tmp_dam, sn, DAM_COLD, TRUE);
   }

   return;
}

void spell_spring_rains(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   ROOM_AFFECT_DATA raf;

   if (is_affected(ch, sn))
   {
      send_to_char("You can't call more healing rains yet.\n\r", ch);
      return;
   }
   if (is_affected_room(ch->in_room, sn))
   {
      send_to_char("A gentle rain already falls here.\n\r", ch);
      return;
   }
   act("A gentle soothing rain begins to fall.", ch, NULL, NULL, TO_ALL);
   raf.where = TO_ROOM_EXTRA;
   raf.bitvector = ROOM_SPRING_RAINS;
   raf.duration = 3;
   raf.type = sn;
   raf.level = level;
   raf.caster = ch;
   affect_to_room(ch->in_room, &raf);
   return;
}

void spell_submerge(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   AFFECT_DATA af;
   if (is_affected(ch, sn))
   {
      send_to_char("You are already submerged beneath the waves.\n\r", ch);
      return;
   }
   if (ch->in_room->sector_type != SECT_WATER_SWIM &&
   ch->in_room->sector_type != SECT_UNDERWATER  &&
   ch->in_room->sector_type != SECT_WATER_NOSWIM)
   {
      send_to_char("There aren't any waves here to submerge beneath.\n\r", ch);
      return;
   }

   act("$n dives deep under the waves!", ch, NULL, NULL, TO_ROOM);
   send_to_char("You dive deep into the water!\n\r", ch);

   af.where = TO_AFFECTS;
   af.type = gsn_submerge;
   af.level = level;
   af.modifier = 0;
   af.bitvector = 0;
   af.duration = (level/5)*race_adjust(ch)/25.0;
   af.location = 0;
   affect_to_char(ch, &af);
   return;
}

void spell_freeze(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   CHAR_DATA *check = NULL;
   AFFECT_DATA af;

   if (is_affected(victim, gsn_freeze))
   {
      send_to_char("They have already been encased in ice.\n\r", ch);
      return;
   }
   if
   (
      (
         IS_NPC(victim) &&
         victim->house != 0
      ) ||
      saves_spell(ch, level - 1, victim, DAM_COLD, SAVE_MALEDICT) ||
      saves_spell(ch, level - 1, victim, DAM_COLD, SAVE_MALEDICT)
   )
   {
      int dam;
      act( "You attempt to freeze $N in ice, but fail.", ch, NULL, victim, TO_CHAR );
      send_to_char("You feel a sudden chilling sensation, but thats all.\n\r", victim);
      dam = dice(level, 3);
      damage(ch, victim, dam, sn, DAM_COLD, TRUE);
      return;
   }

   af.where = TO_AFFECTS;
   af.location = 0;
   af.modifier = 0;
   af.duration = 2;
   af.bitvector = 0;
   af.level = level;
   af.type = sn;
   affect_to_char(victim, &af);

   for (check = victim->in_room->people; check != NULL; check = check->next_in_room)
   if (check->fighting == victim)
   {
      check->fighting = NULL;
      check->position = POS_STANDING;
   }
   victim->fighting = NULL;

   send_to_char("A sudden chill overcomes you and you are frozen in ice before you can gasp.\n\r", victim);
   act("$n is encased in ice!", victim, NULL, NULL, TO_ROOM);
   stop_fighting(victim, FALSE);
   return;
}

void spell_frost_charge(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   OBJ_DATA *obj = (OBJ_DATA *) vo;
   AFFECT_DATA af;

   if (obj == NULL) return;

   if (obj->item_type != ITEM_WEAPON)
   {
      send_to_char("That object cannot be imbued with ice.\n\r", ch);
      return;
   }
   if (is_affected_obj(obj, gsn_alchemy))
   {
      send_to_char("This object is already imbued with a power you don't comprehend.\n\r", ch);
      return;
   }
   make_obj_enchanted(obj);
   af.where     = TO_WEAPON;
   af.type      = sn;
   af.level     = level;
   af.duration  = 5+ch->level/6;
   af.location  = 0;
   af.modifier  = 0;
   af.bitvector = WEAPON_FROST;
   if (IS_WEAPON_STAT(obj, WEAPON_FROST))
   af.bitvector = 0;
   obj->value[3] =  30;
   affect_to_obj(obj, &af);
   act("$n imbues $p with the power of ice.", ch, obj, NULL, TO_ROOM);
   act("You imbue $p with the power of ice and it grows cold in your hands.", ch, obj, NULL, TO_CHAR);
   return;
}

void spell_purify(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   OBJ_DATA *brand;
   int sn_fog;
   AFFECT_DATA af;

   sn_fog    = gsn_faerie_fog;

   if (is_affected(ch, sn))
   {
      send_to_char("You can't purify through water again so soon.\n\r", ch);
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

   if (((brand = get_eq_char(ch, WEAR_BRAND)) != NULL)
   && (brand->pIndexData->vnum == 433))
   {
      send_to_char("The gods have forsaken you.\n\r", ch);
      return;
   }

   if (is_affected(victim, gsn_blindness))
   {
      act("$n is no longer blinded.", victim, NULL, NULL, TO_ROOM);
      affect_strip(victim, gsn_blindness);
   }

   if (is_affected(victim, sn_fog))
   {
      affect_strip(victim, sn_fog);
      act("The purple aura about $n's body fades.", victim, NULL, NULL, TO_ROOM);
   }

   if (is_affected(victim, gsn_poison_dust))
   {
      affect_strip(victim, gsn_poison_dust);
      act("$n looks much better.", victim, NULL, NULL, TO_ROOM);
   }

   if (is_affected(victim, gsn_poison))
   {
      affect_strip(victim, gsn_poison);
      act("$n looks much better.", victim, NULL, NULL, TO_ROOM);
   }

   if (is_affected(victim, gsn_plague))
   {
      affect_strip(victim, gsn_plague);
      act("$n looks relieved as $s sores vanish.", victim, NULL, NULL, TO_ROOM);
   }

   if (victim != ch)
   send_to_char("You purify their body through the power of water.\n\r", ch);
   else
   send_to_char("You purify your own body through the power of water.\n\r", victim);
   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.location  = 0;
   af.modifier  = 0;
   af.duration  = 24;
   af.bitvector = 0;
   affect_to_char( ch, &af );
   return;
}

void spell_waterwalk(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim;
   CHAR_DATA *group;
   CHAR_DATA *arrival;
   CHAR_DATA *g_next;
   CHAR_DATA *last_to_venue;
   int numb;
   bool gate_pet;

   if (is_affected(ch, gsn_trapstun))
   {
      send_to_char("You can't waterwalk you're in a snare.\n\r", ch);
      return;
   }

   if (is_affected(ch, gsn_purity))
   {
      send_to_char
      (
         "The mark of pure blood on you prevents you from waterwalking!\n\r",
         ch
      );
      return;
   }

   if
   (
      IS_SET(ch->in_room->extra_room_flags, ROOM_DEAD_ZONE) &&
      !IS_SET(ch->in_room->room_flags, ROOM_CONSECRATED)
   )
   {
      send_to_char("You cannot waterwalk here.\n\r", ch);
      return;
   }
   if (ch->fighting != NULL)
   {
      send_to_char("You can't concentrate enough.\n\r", ch);
      return;
   }

   last_to_venue = ch;
   victim = get_char_world( ch, target_name );
   if (victim == ch || victim == NULL)
   {
      send_to_char("You failed.\n\r", ch);
      return;
   }

   if (ch->in_room->sector_type != SECT_WATER_SWIM &&
   ch->in_room->sector_type != SECT_WATER_NOSWIM &&
   ch->in_room->sector_type != SECT_UNDERWATER)
   {
      OBJ_DATA *obj;
      int found = FALSE;
      for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
      {
         if ( obj->item_type == ITEM_FOUNTAIN )
         found = TRUE;
      }
      if (!found)
      {
         send_to_char("There is no water here.\n\r", ch);
         return;
      }
   }
   if (victim->in_room->sector_type != SECT_WATER_SWIM &&
   victim->in_room->sector_type != SECT_WATER_NOSWIM &&
   victim->in_room->sector_type != SECT_UNDERWATER)
   {
      OBJ_DATA *obj;
      int found = FALSE;
      for ( obj = victim->in_room->contents; obj; obj = obj->next_content )
      {
         if ( obj->item_type == ITEM_FOUNTAIN )
         found = TRUE;
      }
      if (!found)
      {
         send_to_char("There is no water there.\n\r", ch);
         return;
      }
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
      victim->level >= level + 4
   ||   (!IS_NPC(victim) &&
      is_safe
      (
         ch,
         victim,
         IS_SAFE_SILENT | IS_SAFE_IGNORE_ROOM | IS_SAFE_IGNORE_AFFECTS
      ) &&
      IS_SET(victim->act, PLR_NOSUMMON))
   || (!IS_NPC(victim) && saves_spell(ch, level+6, victim, DAM_OTHER, SAVE_TRAVEL))
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
      send_to_char("Your too bloody to gate to that place.\n\r", ch);
      return;
   }
   /* not sure but this check seems redudandant with above - werv */
   if (victim->in_room->house != 0
   && victim->in_room->house != ch->house)
   {
      send_to_char("You failed.\n\r", ch);
      return;
   }

   if (ch->pet != NULL && ch->in_room == ch->pet->in_room)
   gate_pet = TRUE;
   else
   gate_pet = FALSE;

   numb = 1;
   for (group = ch->in_room->people; group != NULL; group = g_next)
   {
      g_next = group->next_in_room;
      if ( group->fighting != NULL )
      continue;
      if (group == ch )
      continue;
      if ( !IS_NPC(group)
      || !IS_AFFECTED(group, AFF_CHARM)
      ||  group->master == NULL
      ||  group->master != ch)
      continue;

      numb++;
      un_camouflage(group, NULL);
      un_earthfade(group, NULL);
      un_hide(group, NULL);
      un_forest_blend(group); /* - Wicket */
      if (ch->is_riding != NULL &&
      (group != ((CHAR_DATA *) ch->is_riding->mount))){
         send_to_char("You dissolve into the water and leave.\n\r", group);
         act("$n dissolves into the water and is gone.", group, NULL, NULL, TO_ROOM);
      }
      char_from_room(group);
      char_to_room(group, victim->in_room);
      if (ch->is_riding != NULL &&
      (group != ((CHAR_DATA *) ch->is_riding->mount)))
      act("$n emerges from the water.", group, NULL, NULL, TO_ROOM);
      last_to_venue = group;
   }

   if (gate_pet)
   {
      group = ch->pet;
      un_camouflage(group, NULL);
      un_earthfade(group, NULL);
      un_hide(group, NULL);
      un_forest_blend(group); /* - Wicket */
      send_to_char("You dissolve into the water and leave.\n\r", group);
      act("$n dissolves into the water and is gone.", group, NULL, NULL, TO_ROOM);
      char_from_room(group);
      char_to_room(group, victim->in_room);
      act("$n emerges from the water.", group, NULL, NULL, TO_ROOM);
      last_to_venue = group;
      numb++;
   }
   un_camouflage(ch, NULL);
   un_hide(ch, NULL);
   un_earthfade(ch, NULL);
   un_forest_blend(ch); /* - Wicket */

   act("$n dissolves into the water and is gone.", ch, NULL, NULL, TO_ROOM);
   send_to_char("You dissolve into the water and leave.\n\r", ch);
   char_from_room(ch);
   char_to_room(ch, victim->in_room);
   act("$n emerges from the surrounding waters.", ch, NULL, NULL, TO_ROOM);

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

void spell_ice_armor(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   AFFECT_DATA af;

   if (is_affected(ch, sn))
   {
      send_to_char("You are already encased in ice.\n\r", ch);
      return;
   }
   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.location  = APPLY_AC;
   af.modifier  = -50;
   af.duration  = 5+level/5;
   af.bitvector = 0;
   affect_to_char( ch, &af );
   send_to_char("You encase your body in a shield of ice.\n\r", ch);
   act("$n encases $s body in a shield of ice.", ch, NULL, NULL, TO_ROOM);
   return;
}

void spell_water_of_life(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   AFFECT_DATA af;
   ROOM_AFFECT_DATA raf;

   if (is_affected(ch, sn))
   {
      send_to_char("You can't magically enhance the growth of vegetation again so soon.\n\r", ch);
      return;
   }
   if (ch->in_room->sector_type == SECT_FOREST)
   {
      send_to_char("This is already a forest.\n\r", ch);
      return;
   }
   if (ch->in_room->sector_type == SECT_WATER_NOSWIM ||
   ch->in_room->sector_type == SECT_UNDERWATER ||
   ch->in_room->sector_type == SECT_AIR)
   {
      send_to_char("A forest cannot grow here.\n\r", ch);
      return;
   }
   if (is_affected_room(ch->in_room, gsn_raging_fire))
   {
      affect_strip_room(ch->in_room, gsn_raging_fire);
      send_to_char("The fires go out.\n\r", ch);
   }
   if (room_sector_modified(ch->in_room, ch))
   return;
   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.location  = 0;
   af.modifier  = 0;
   af.duration  = 24;
   af.bitvector = 0;
   affect_to_char( ch, &af );
   send_to_char( "You call upon a gentle mist to enhance the growth of the surrounding plantlife.\n\r", ch );
   act("$n raises $s arms and nods, and a gentle mist fills the area, and trees begin to grow before your eyes.", ch, NULL, NULL, TO_ROOM);
   raf.where     = TO_ROOM_SECTOR;
   raf.sector_new = SECT_FOREST;
   raf.duration = 8;
   raf.type = sn;
   raf.level = level;
   raf.bitvector = 0;
   raf.caster = ch;
   affect_to_room(ch->in_room, &raf);
   return;
}

void spell_fountain_of_youth(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   OBJ_DATA *spring;
   AFFECT_DATA af;

   if (is_affected(ch, sn))
   {
      send_to_char("You cannot create another restorative spring right now.\n\r", ch);
      return;
   }
   /*    for (spring = ch->in_room->contents; spring != NULL; spring =
   spring->next_content)
   {
   if (spring->item_type == ITEM_FOUNTAIN)
   {
   send_to_char("A fountain already flows here.\n\r", ch);
   return;
   }
   } */

   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.location  = 0;
   af.modifier  = 0;
   af.duration  = 8;
   af.bitvector = 0;
   affect_to_char( ch, &af );

   spring = create_object( get_obj_index( OBJ_VNUM_CURING_SPRING ), 0 );
   spring->timer = level;
   spring->level = number_range(3, 5);
   obj_to_room( spring, ch->in_room );
   act( "$p begins to flow here.", ch, spring, NULL, TO_ROOM );
   act( "$p begins to flow here.", ch, spring, NULL, TO_CHAR );
   return;
}

void spell_rip_tide(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim;
   AFFECT_DATA* paf;

   if (ch->in_room->sector_type != SECT_WATER_SWIM &&
   ch->in_room->sector_type != SECT_WATER_NOSWIM &&
   ch->in_room->sector_type != SECT_UNDERWATER)
   {
      send_to_char("You must be in water.\n\r", ch);
      return;
   }
   victim= get_char_world(ch, target_name);
   if (victim == NULL)
   {
      send_to_char("They aren't here.\n\r", ch);
      return;
   }
   if (victim->in_room->sector_type != SECT_WATER_SWIM &&
   victim->in_room->sector_type != SECT_WATER_NOSWIM &&
   victim->in_room->sector_type != SECT_UNDERWATER)
   {
      send_to_char("They must be in water.\n\r", ch);
      return;
   }

   if (oblivion_blink(ch, victim))
   {
      return;
   }

   if ( victim == ch
   ||   victim->in_room == NULL
   ||   (!IS_NPC(victim) && victim->in_room->area != ch->in_room->area)
   ||   IS_SET(ch->in_room->room_flags, ROOM_SAFE)
   ||   IS_SET(victim->in_room->room_flags, ROOM_NO_SUMMON)
   ||   ch->in_room->guild != 0
   ||     victim->in_room->guild != 0
   ||   (IS_NPC(victim) && IS_AFFECTED(victim, AFF_CHARM) && victim->in_room->area != ch->in_room->area)
   ||   IS_SET(victim->in_room->room_flags, ROOM_SAFE)
   ||   IS_SET(victim->in_room->room_flags, ROOM_PRIVATE)
   ||   IS_SET(victim->in_room->room_flags, ROOM_SOLITARY) ||
      (
         (
            IS_SET(victim->in_room->room_flags, ROOM_NO_RECALL) ||
            IS_SET(victim->in_room->extra_room_flags, ROOM_DEAD_ZONE)
         ) &&
         !IS_SET(victim->in_room->room_flags, ROOM_CONSECRATED)
      ) ||
      (IS_NPC(victim) && IS_SET(victim->act, ACT_AGGRESSIVE))
   ||   (IS_NPC(victim) && IS_SET(victim->off_flags, SPAM_MURDER))
   ||   victim->level >= (level + 5)
   ||   (IS_NPC(victim) && victim->pIndexData->mprog_flags)
   ||   (!IS_NPC(victim) && victim->level >= LEVEL_IMMORTAL)
   ||    ( ch->in_room->house != victim->in_room->house && !IS_NPC(victim))
   ||   victim->fighting != NULL
   ||   (IS_NPC(victim) && IS_SET(victim->imm_flags, IMM_SUMMON))
   ||     (IS_NPC(victim) && victim->pIndexData->pShop != NULL)
   ||   (IS_NPC(victim) && victim->spec_fun != NULL)
   ||   (IS_NPC(victim) && IS_SET(victim->act, ACT_AGGRESSIVE))
   ||   (!IS_NPC(ch) &&
   !IS_NPC(victim) &&
   is_safe
   (
      ch,
      victim,
      IS_SAFE_SILENT | IS_SAFE_IGNORE_ROOM | IS_SAFE_AREA_ATTACK | IS_SAFE_IGNORE_AFFECTS
   ) &&
   IS_SET(victim->act, PLR_NOSUMMON))
   ||   (saves_spell( ch, level, victim, DAM_OTHER, SAVE_TRAVEL)) )
   {
      send_to_char( "You form the tide, but they seem to have resisted.\n\r", ch );
      act("You feel a sudden undertow pulling you beneath the waves, but swim out of it.", victim, NULL, NULL, TO_CHAR);
      return;
   }
   if (ch->in_room->vnum >= 1950 && ch->in_room->vnum <= 1962)
   {
      send_to_char( "You failed.\n\r", ch );
      return;
   }

   /* Remove unwanted affects */
   un_camouflage(victim, NULL);
   un_earthfade(victim, NULL);
   un_forest_blend(victim);
   un_hide(victim, NULL);

   if ( victim->is_riding )
   do_dismount( victim, NULL );

   act( "$n suddenly vanishes beneath the waves and is gone.", victim, NULL, NULL, TO_ROOM );
   char_from_room( victim );
   char_to_room( victim, ch->in_room );
   act( "$n arrives with the incoming tide.", victim, NULL, NULL, TO_ROOM );
   act( "You feel a sudden undertow and are whisked beneath the waves!", ch, NULL, victim,   TO_VICT );
   do_observe(victim, "", LOOK_AUTO);
   if (victim->in_room->sector_type == SECT_UNDERWATER)
   {
      paf = new_affect();
      paf->type = sn;
      paf->duration = -1;
      if (number_bits(2))
      {
         paf->level = 2;  /* 3 rounds including rest of this one */
      }
      else
      {
         paf->level = 3;  /* 4 rounds including rest of this one */
      }
      if (IS_AFFECTED(victim, AFF_SWIM))
      {
         paf->level -= 1 + number_bits(1);
      }
      paf->bitvector2 = AFF_RIPTIDE;
      affect_to_char_1(victim, paf);
      send_to_char("You are disoriented by the undertow!\n\r", victim);
      act("$n seems disoriented by the waves.", victim, NULL, NULL, TO_ROOM);
      free_affect(paf);
   }
   return;
}

void spell_earthbind(int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   AFFECT_DATA af;
   CHAR_DATA *victim = (CHAR_DATA *) vo;

   if (is_affected(victim, sn))
   {
      send_to_char("Their feet are already on the ground.\n\r", ch);
      return;
   }
   if (saves_spell(ch, level, victim, DAM_OTHER, SAVE_MALEDICT))
   {
      send_to_char("You failed.\n\r", ch);
      return;
   }
   af.where = TO_AFFECTS;
   af.type = gsn_earthbind;
   af.modifier = 0;
   af.level =  level;
   af.duration = level/5*race_adjust(ch)/25.0;
   af.location = 0;
   af.bitvector = 0;
   send_to_char("Your feet suddenly become heavy and earthbound.\n\r", victim);
   if (IS_FLYING(victim))
   {
      act("$n suddenly drops to the ground.", victim, NULL, NULL, TO_ROOM);
      affect_strip(victim, gsn_fly);
   }
   else
   {
      act("$n is suddenly earthbound to the ground.", victim, NULL, NULL, TO_ROOM);
   }
   /*
      Add the earthbind affect only after checking if the victim is flying.
      (earthbind affect on makes IS_FLYING return false)
   */
   affect_to_char(victim, &af);
   return;
}

void spell_sunburst(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   CHAR_DATA* victim = (CHAR_DATA*)vo;
   CHAR_DATA* vch;
   CHAR_DATA* vch_next;
   char buf[MAX_STRING_LENGTH];
   int dam;

   if (check_peace(ch))
   {
      return;
   }

   if (check_spellcraft(ch, sn))
   {
      dam = spellcraft_dam(level, 12);
   }
   else
   {
      dam = dice(level, 12);
   }
   if ( saves_spell( ch, level, victim, DAM_FIRE, SAVE_SPELL) )
   {
      dam /= 2;
   }
   damage
   (
      ch,
      victim,
      dam * race_adjust(ch) / 25.0,
      sn,
      DAM_FIRE,
      TRUE
   );

   /* now do the splash damage */
   for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
   {
      vch_next = vch->next_in_room;
      if
      (
         is_same_group(vch, ch) ||
         !is_same_group(victim, vch) ||
         vch == victim ||
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
            default:
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
            "[%s] cast sunburst upon [%s] at %d",
            ch->name,
            vch->name,
            ch->in_room->vnum
         );
         log_string(log_buf);
      }
      if (check_spellcraft(ch, sn))
      {
         dam = spellcraft_dam(level, 12);
      }
      else
      {
         dam = dice(level, 12);
      }
      if (saves_spell(ch, level, vch, DAM_FIRE, SAVE_SPELL))
      {
         dam /= 2;
      }
      damage(ch, vch, dam / 2, sn, DAM_FIRE , TRUE);
   }
   return;
}

bool capsize_boat(CHAR_DATA *ch)
{
   OBJ_DATA *obj;
   OBJ_DATA *next_obj;

   if (IS_IMMORTAL(ch))
   return FALSE;
   if (saves_spell(ch, ch->level-3, ch, DAM_OTHER, SAVE_SPELL))
   return FALSE;

   for (obj = ch->carrying; obj != NULL; obj = next_obj)
   {
      next_obj = obj->next_content;
      if ( obj->item_type != ITEM_BOAT )
      continue;
      extract_obj(obj, FALSE);
      act("$n's boat capsizes on the waves!", ch, NULL, NULL, TO_ROOM);
      act("Your boat capsizes on the waves!", ch, NULL, NULL, TO_CHAR);
      return TRUE;
   }
   return FALSE;
}

void spell_dark_armor( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;

   if (house_down(ch, HOUSE_COVENANT))
   return;

   if ( is_affected(victim, sn) )
   {
      if (victim == ch)
      send_to_char("You are already clad in dark armor.\n\r", ch);
      else
      act("$N is already clad in dark armor.", ch, NULL, victim, TO_CHAR);
      return;
   }

   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.duration  = level/6;
   af.location  = APPLY_NONE;
   af.modifier  = 0;
   af.bitvector = 0;
   affect_to_char( victim, &af );
   act( "$n clads $mself in the dark armor of a Covenant Knight.", victim, NULL, NULL, TO_ROOM );
   send_to_char( "You clad yourself in the dark armor of a Covenant Knight.\n\r", victim );
   return;
}

void spell_aura_of_presence( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   AFFECT_DATA af;
   CHAR_DATA *vch;
   DESCRIPTOR_DATA *d;

   if (house_down(ch, HOUSE_COVENANT))
   return;
   if (IS_NPC(ch)) return;
   if (ch->pcdata->house_rank < RANK_BRIGADIERGENERAL)
   {
      send_to_char("You lack the leadership to produce such an aura.\n\r", ch);
      return;
   }
   act("The aura of your presence boosts morale of all Covenant soldiers.", ch, NULL, NULL, TO_CHAR);

   for (d = descriptor_list; d != NULL; d = d->next)
   {
      vch = d->character;
      if (vch == NULL || d->connected != CON_PLAYING)
      continue;
      if (IS_IMMORTAL(vch)) continue;
      if (IS_NPC(vch))
      continue;
      if (vch->house != HOUSE_COVENANT) continue;
      if (is_affected(vch, sn)) continue;
      af.where     = TO_AFFECTS;
      af.type      = sn;
      af.level     = level;
      af.duration  = 20;
      af.modifier  = 4;
      af.location  = APPLY_DAMROLL;
      af.bitvector = 0;
      affect_to_char( vch, &af );
      af.location  = APPLY_HITROLL;
      affect_to_char( vch, &af);
      af.modifier  = 20;
      af.location  = APPLY_HIT;
      affect_to_char( vch, &af );
      af.location  = APPLY_MANA;
      affect_to_char( vch, &af );
      af.location  = APPLY_MOVE;
      affect_to_char( vch, &af );
      af.modifier  = -20;
      af.location  = APPLY_AC;
      affect_to_char( vch, &af );
      af.modifier  = -2;
      af.location  = APPLY_SAVES;
      affect_to_char( vch, &af );
      send_to_char( "Your morale is boosted by the General's presence.\n\r", vch );
   }
   return;
}

void spell_holy_beacon(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   AFFECT_DATA* paf;
   CHAR_DATA* vch;
   bool found = FALSE;
   DESCRIPTOR_DATA* d;
   DESCRIPTOR_DATA* d_next;
   bool immort = IS_IMMORTAL(ch) && !IS_NPC(ch);

   if (is_affected(ch, sn))
   {
      send_to_char
      (
         "You do not feel ready to conjure another holy beacon for help.\n\r",
         ch
      );
      return;
   }
   /* Do not send up beacons if need cannot bring you here */
   if
   (
      !check_room(ch, FALSE, NULL) ||
      IS_SET(ch->in_room->extra_room_flags, ROOM_1212) ||
      IS_SET(ch->in_room->room_flags, ROOM_SAFE) ||
      IS_SET(ch->in_room->room_flags, ROOM_PRIVATE) ||
      IS_SET(ch->in_room->room_flags, ROOM_SOLITARY) ||
      (
         (
            IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL) ||
            IS_SET(ch->in_room->extra_room_flags, ROOM_DEAD_ZONE)
         ) &&
         !IS_SET(ch->in_room->room_flags, ROOM_CONSECRATED)
      )
   )
   {
      send_to_char("You cannot conjure a holy beacon from here!\n\r", ch);
      return;
   }
   /* Need brings only to combat, so if there is no combat, no beacon */
   for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
   {
      if
      (
         vch->fighting == NULL ||
         vch->in_room != ch->in_room ||  /* sanity check */
         vch->fighting->in_room != ch->in_room ||  /* sanity check */
         (
            !IS_NPC(vch) &&
            vch->level >= LEVEL_IMMORTAL
         ) ||
         (
            !IS_NPC(vch->fighting) &&
            vch->fighting->level >= LEVEL_IMMORTAL
         ) ||
         (
            IS_NPC(vch) &&
            vch->pIndexData->pShop != NULL
         ) ||
         !IS_GOOD(vch) ||
         !IS_EVIL(vch->fighting)
      )
      {
         continue;
      }
      found = TRUE;
      break;
   }
   if (!found)
   {
      act
      (
         "$z will not grant you a beacon when there is no need of one.",
         ch,
         NULL,
         NULL,
         TO_CHAR
      );
      return;
   }
   /* SUCCESS! */
   act
   (
      "$n calls to $z and conjures a holy beacon!",
      ch,
      NULL,
      NULL,
      TO_ROOM
   );
   act
   (
      "You call to $z and conjure a holy beacon!",
      ch,
      NULL,
      NULL,
      TO_CHAR
   );
   /* Send emote to all in room */
   act
   (
      "A holy beacon bursts in the air, calling for help!",
      ch,
      NULL,
      NULL,
      TO_ALL
   );
   /*
   Send emote to all not in room (already did in room)
   that have the need spell.
   Do not send to immortals if the caster was mortal
   If caster was immortal, send to all immortals, even if they
   do not have need
   */
   for (d = descriptor_list; d != NULL; d = d_next)
   {
      d_next = d->next;
      vch = d->character;
      if
      (
         d->connected != CON_PLAYING ||
         (
            vch = d->character
         ) == NULL ||
         vch->in_room == ch->in_room ||
         (
            !immort &&
            IS_IMMORTAL(vch)  /* Immortals only if caster was immortal */
         ) ||
         (
            !IS_IMMORTAL(vch) &&
            (
               !has_skill(vch, gsn_need) ||
               get_skill(vch, gsn_need) <= 0
            )
         )
      )
      {
         continue;
      }
      if
      (
         immort &&
         IS_IMMORTAL(vch) &&
         get_trust(vch) >= get_trust(ch)
      )
      {
         send_to_char("beacon> ", vch);
      }
      send_to_char
      (
         "A holy beacon bursts in the air, calling for help!\n\r",
         vch
      );
   }
   /* Apply 12 hour (36 tick) timer  */
   paf           = new_affect();
   paf->where    = TO_AFFECTS;
   paf->level    = level;
   paf->duration = 36;
   paf->type     = sn;
   affect_to_char(ch, paf);
   free_affect(paf);
}

void spell_seize(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA*) vo;
   AFFECT_DATA af;
   CHAR_DATA *check;
   int count = 0;
   int counter = 0;
   int fail = 0;
   extern const int seize_spell_exception_table[];

   if (IS_NPC(ch))  /*Mobs can't seize other mobs*/
   {
      return;
   }
   if (house_down(ch, HOUSE_CONCLAVE))
   {
      return;
   }
   if
   (
      ch->house != HOUSE_CONCLAVE &&
      !IS_IMMORTAL(ch)
   )
   {
      wiznet
      (
         "$N has spell seize, not immortal and not CONCLAVE.",
         ch,
         NULL,
         WIZ_BUGS,
         0,
         0
      );
      return;
   }
   if (!victim)
   {
      bug("spell_seize: victim is null", 0);
      return;
      /*sanity check*/
   }
   if (!ch)
   {
      bug("spell_seize: ch is null", 0);
      return;
      /*sanity check*/
   }
   if (!check_room(ch, FALSE, "spell_seize: ch in null room"))
   {
      return;
      /*sanity check*/
   }
   if (target_name[0] == '\0')
   {
      send_to_char("Seize whom?\n\r", ch);
      return;
   }
   if (victim == ch)
   {
      send_to_char
      (
         "You seize yourself in a big bearhug.\n\r",
         ch
      );
      return;
   }
   if (!IS_AFFECTED(victim, AFF_CHARM))
   {
      send_to_char
      (
         "You can only seize charmed followers.\n\r",
         ch
      );
      return;
   }
   if (!IS_NPC(victim))
   {
      /*
      Wait on clarification for what happens with PK victims
      for now, just return with a message, assume they can't
      be.
      */
      act
      (
         "$N's will is too strong to be seized.",
         ch,
         NULL,
         victim,
         TO_CHAR
      );
      return;
   }
   if (IS_SET(victim->act2, ACT_NO_SEIZE))
   {
      send_to_char("There is no way you are going to seize that!\n\r", ch);
      return;
   }
   if
   (
      victim->master &&
      victim->master == ch
   )/*Your charmie*/
   {
      act
      (
         "$N is already following you.",
         ch,
         NULL,
         victim,
         TO_CHAR
      );
      return;
   }
   if (is_affected(victim, sn))
   {
      if (victim->master)
      {
         /*
         been seized, and has a master, we'll allow someone to reseize
         but only if they don't have a master.
         */
         act
         (
            "$N has already been seized by $t.",
            ch,
            get_descr_form(victim->master, ch, FALSE),
            victim,
            TO_CHAR
         );
         return;
      }
      affect_strip(victim, sn);
   }
   if (is_safe(ch, victim, 0))
   {
      /*If the gods protect you from them, we're done*/
      return;
   }
   /*
   We don't care about the number of charmies we have through
   seize, only the sum of the levels of charmies through seize
   We add it up and find the number
   */
   for (check = char_list; check != NULL; check = check->next)
   {
      if
      (
         check->master == ch &&
         IS_AFFECTED(check, AFF_CHARM) &&
         is_affected(check, sn)
      )
      {
         count += GET_LEVEL(check);
      }
   }
   if ((level * 3 / 2) < (count + GET_LEVEL(victim)))
   {
      /*
      The sum of levels of seized mobs you have
      cannot exceed 1.5 times your level
      */
      if (!count)
      {
         act
         (
            "You are not powerful enough to seize $N.",
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
            "You are not powerful enough to add $N to your army.",
            ch,
            NULL,
            victim,
            TO_CHAR
         );
      }
      return;
   }
   if (ch->in_room != victim->in_room)
   {
      act
      (
         "You are not in the same room as $N.",
         ch,
         NULL,
         victim,
         TO_CHAR
      );
      return;
      /*sanity check*/
   }
   /*--------------*/
   if (IS_NPC(victim))  /* In case PK's are allowed, another check */
   {
      /*
      Check for each mob thats not allowed to be seized.
      For instance, enforcer guards will probably
      not be allowed
      */
      for
      (
         counter = 0;
         seize_spell_exception_table[counter] != -1000;
         counter++
      )
      {
         if (victim->pIndexData->vnum == seize_spell_exception_table[counter])
         {
            /*
            The mob we're trying is in the exclusion list, thus not allowed
            to seize it
            */
            if (victim->master)  /* Has a master */
            {
               if (can_see(ch, victim->master))
               {
                  act
                  (
                     "$N would never follow anyone but $t.",
                     ch,
                     get_descr_form(victim->master, ch, FALSE),
                     victim,
                     TO_CHAR
                  );
               }
               else
               {
                  act
                  (
                     "$N would never follow anyone but $S master.",
                     ch,
                     NULL,
                     victim,
                     TO_CHAR
                  );
               }
            }
            else  /* Doesn't have a master */
            {
               act
               (
                  "$N would never follow you.",
                  ch,
                  NULL,
                  victim,
                  TO_CHAR
               );
            }
            return;
         }
      }
   }
   /*--------------*/
   /*
   At this point, we <can> seize, and are allowed to
   */
   fail += 50;
   if
   (
      !victim->master ||
      victim->master->in_room != victim->in_room
   )
   {
      fail -= 25;
   }
   if (magic_power(ch))
   {
      fail -= magic_power(ch) * 2;
   }
   else
   {
      fail -= magic_power(ch) * 1;
   }
   if (level < GET_LEVEL(victim))
   {
      fail += (GET_LEVEL(victim) - level) * 2;
   }
   else
   {
      fail -= (level - GET_LEVEL(victim)) * 1;
   }
   fail += 100;
   fail -= get_skill(ch, sn);
   if (number_percent() > fail)
   {
      /*
      SUCCESS!
      */
      if (victim->master)
      {
         if (victim->master->in_room == victim->in_room)
         {
            act
            (
               "You forcefully seize $N from $t!",
               ch,
               get_descr_form(victim->master, ch, FALSE),
               victim,
               TO_CHAR
            );
            act
            (
               "$t glares at you and runs over to $n.",
               ch,
               get_descr_form(victim, victim, FALSE),
               victim->master,
               TO_VICT
            );
            act
            (
               "$t scowls at $N and runs over to $n.",
               ch,
               get_descr_form(victim, ch, FALSE),
               victim->master,
               TO_NOTVICT
            );
         }
         else
         {
            act
            (
               "As you seize $N, $E looks around for $t.",
               ch,
               get_descr_form(victim->master, ch, FALSE),
               victim,
               TO_CHAR
            );
            act
            (
               "$t looks around for $N before running over to $n.",
               ch,
               get_descr_form(victim, ch, FALSE),
               victim->master,
               TO_NOTVICT
            );
         }
      }
      else
      {
         act
         (
            "You forcefully seize $N!",
            ch,
            NULL,
            victim,
            TO_CHAR
         );
         act
         (
            "$N is seized forcefully by $n!",
            ch,
            NULL,
            victim,
            TO_NOTVICT
         );
      }
      if
      (
         victim->master &&
         !IS_NPC(victim->master)
      )
      {
         if (victim->master->in_room == ch->in_room)
         {
            sprintf
            (
               log_buf,
               "Help! %s just stole %s from me!",
               PERS(ch, victim->master),
               PERS(victim, victim->master)
            );
            do_myell(victim->master, log_buf);
            if
            (
               !IS_NPC(ch) &&
               !IS_NPC(victim->master) &&
               !IS_IMMORTAL(ch) &&
               !IS_IMMORTAL(victim->master)
            )
            {
               QUIT_STATE(ch, 20);
               QUIT_STATE(victim->master, 20);
            }
         }
         sprintf
         (
            log_buf,
            "[%s] cast seize on [%s]'s [%s] at %d",
            ch->name,
            victim->master->name,
            PERS(victim, victim),
            ch->in_room->vnum
         );
         log_string(log_buf);
      }
      af.where     = TO_AFFECTS;
      af.type      = sn;
      af.level     = level;
      af.duration  = -1;  /* permament */
      af.location  = APPLY_HITROLL;
      /* Wrenched away, slightly less effective in battle */
      af.modifier  = -1;
      af.bitvector = AFF_CHARM;
      stop_follower(victim);
      affect_to_char(victim, &af);
      add_follower(victim, ch);
      victim->leader = ch;
   }
   else
   {
      /*
      FAILURE!
      */
      act
      (
         "You fail to seize $N and $E growls at you!",
         ch,
         NULL,
         victim,
         TO_CHAR
      );
      act
      (
         "$n just tried to seize $t from you!",
         ch,
         get_descr_form(victim, victim->master, FALSE),
         victim->master,
         TO_VICT
      );
      act
      (
         "$n just tried to seize $t from $N!",
         ch,
         get_descr_form(victim, ch, FALSE),
         victim->master,
         TO_NOTVICT
      );
      if
      (
         victim->master &&
         !IS_NPC(victim->master)
      )
      {
         if (victim->master->in_room == ch->in_room)
         {
            sprintf
            (
               log_buf,
               "Help! %s just tried to steal %s from me!",
               PERS(ch, victim->master),
               PERS(victim, victim->master)
            );
            do_myell(victim->master, log_buf);
         }
         sprintf
         (
            log_buf,
            "[%s] cast seize on [%s]'s [%s] at %d",
            ch->name,
            victim->master->name,
            PERS(victim, victim),
            ch->in_room->vnum
         );
         log_string(log_buf);
      }
   }
}

void spell_virulent_cysts(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   int chance = get_skill(ch, sn);
   AFFECT_DATA* paf;

   if (house_down(ch, HOUSE_CONCLAVE))
   {
      return;
   }
   if (is_affected(ch, sn))
   {
      send_to_char("You are already covered in deadly cysts.\n\r", ch);
      return;
   }
   if
   (
      !IS_NPC(ch) &&
      number_percent() > chance
   )
   {
      send_to_char("You fail to cover yourself in deadly cysts.\n\r", ch);
      act
      (
         "$n tries to cover $mself with deadly cysts, but they fall off.",
         ch,
         NULL,
         NULL,
         TO_ROOM
      );
      return;
   }
   send_to_char("You cover yourself in deadly cysts.\n\r", ch);
   act
   (
      "$n covers $mself with deadly cysts.",
      ch,
      NULL,
      NULL,
      TO_ROOM
   );
   paf = new_affect();
   paf->duration = -1;  /* Wears off from use */
   paf->type = sn;
   paf->level = level;
   affect_to_char(ch, paf);
   free_affect(paf);
}

void spell_esurience(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   CHAR_DATA* vch;
   CHAR_DATA* vch_next;
   char buf[MAX_STRING_LENGTH];
   int dam;
   bool spellcraft = FALSE;
   AFFECT_DATA* paf;
   AFFECT_DATA* paf_c;
   long id;
   bool npc;

   if (check_peace(ch))
   {
      return;
   }

   send_to_char
   (
      "A black bolt erupts from your hand, draining the area of life!\n\r",
      ch
   );
   act
   (
      "A black bolt erupts from $n's hand, draining the area of life!",
      ch,
      NULL,
      NULL,
      TO_ROOM
   );
   if (check_spellcraft(ch, sn))
   {
      spellcraft = TRUE;
   }
   paf = new_affect();
   paf->where = TO_AFFECTS;
   paf->type = sn;
   paf->level = level;
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
               break;
            }
         }
         do_myell(vch, buf);
         sprintf
         (
            log_buf,
            "[%s] cast esurience upon [%s] at %d",
            ch->name,
            vch->name,
            ch->in_room->vnum
         );
         log_string(log_buf);
      }
      if
      (
         check_immune(vch, DAM_DISEASE) == IS_IMMUNE &&
         (
            (
               !IS_NPC(vch) &&
               (
                  IS_SET(vch->act2, PLR_LICH) ||
                  vch->pcdata->special == SUBCLASS_KNIGHT_OF_FAMINE
               )
            ) ||
            check_immune(vch, DAM_OTHER) == IS_IMMUNE ||
            is_affected(vch, sn)
         )
      )
      {
         act
         (
            "$N resists your dark power of esurience.",
            ch,
            NULL,
            vch,
            TO_CHAR
         );
         send_to_char
         (
            "You resist the dark power from draining you of your nutrients."
            "\n\r",
            vch
         );
         continue;
      }
      if (spellcraft)
      {
         dam = spellcraft_dam(level, 6);
      }
      else
      {
         dam = dice(level, 6);
      }
      if (saves_spell(ch, level, vch, DAM_DISEASE, SAVE_SPELL))
      {
         dam /= 2;
      }
      id = vch->id;
      npc = IS_NPC(vch);
      damage(ch, vch, dam, sn, DAM_DISEASE , TRUE);
      if
      (
         /* Skip if dead/wimpied */
         !vch->valid ||
         vch->in_room != ch->in_room ||
         vch->ghost ||
         vch->id != id ||
         (
            !npc &&
            vch != id2name_room(id, ch->in_room, FALSE)
         )
      )
      {
         continue;
      }
      if
      (
         (
            !IS_NPC(vch) &&
            (
               IS_SET(vch->act2, PLR_LICH) ||
               vch->pcdata->special == SUBCLASS_KNIGHT_OF_FAMINE
            )
         ) ||
         check_immune(vch, DAM_OTHER) == IS_IMMUNE ||
         is_affected(vch, sn)
      )
      {
         act
         (
            "$N resists your dark power of esurience.",
            ch,
            NULL,
            vch,
            TO_CHAR
         );
         send_to_char
         (
            "You resist the dark power from draining you of your nutrients."
            "\n\r",
            vch
         );
         continue;
      }
      paf->duration = level / 10;
      if (!saves_spell(ch, level, vch, DAM_OTHER, SAVE_OTHER))
      {
         act
         (
            "$N succumbs to your power of esurience and starts wasting away!",
            ch,
            NULL,
            vch,
            TO_CHAR
         );
         act
         (
            "$N succumbs to $n's power of esurience and starts wasting away!",
            ch,
            NULL,
            vch,
            TO_NOTVICT
         );
         send_to_char
         (
            "You feel yourself withering away as thirst and hunger overcome"
            " you!\n\r",
            vch
         );
         if (!IS_NPC(vch))
         {
            /* Hunger for non arborians */
            if (vch->race != grn_arborian)
            {
               if (vch->pcdata->condition[COND_HUNGER] > 0)
               {
                  gain_condition
                  (
                     vch,
                     COND_HUNGER,
                     -vch->pcdata->condition[COND_HUNGER]
                  );
               }
               if
               (
                  !vch->pcdata->condition[COND_HUNGER] &&
                  vch->pcdata->condition[COND_STARVING] < 9
               )
               {
                  gain_condition
                  (
                     vch,
                     COND_STARVING,
                     9 - vch->pcdata->condition[COND_STARVING]
                  );
               }
            }

            if (!is_branded_by_lestregus(vch))
            {
               /* Thirst for all*/
               if (vch->pcdata->condition[COND_THIRST] > 0)
               {
                  gain_condition
                  (
                     vch,
                     COND_THIRST,
                     -vch->pcdata->condition[COND_THIRST]
                  );
               }
               if
               (
                  !vch->pcdata->condition[COND_THIRST] &&
                  vch->pcdata->condition[COND_DEHYDRATED] < 6
               )
               {
                  gain_condition
                  (
                     vch,
                     COND_DEHYDRATED,
                     6 - vch->pcdata->condition[COND_DEHYDRATED]
                  );
               }
            }
         }
      }
      else
      {
         act
         (
            "$N resists your dark power of esurience.",
            ch,
            NULL,
            vch,
            TO_CHAR
         );
         send_to_char
         (
            "You resist the dark power from draining you of your nutrients."
            "\n\r",
            vch
         );
         paf->duration /= 2;
      }
      send_to_char
      (
         "The sight of food and drink sickens you.\n\r",
         vch
      );
      affect_to_char(vch, paf);
      paf_c = affect_find(vch->affected, sn);
      if (paf_c != NULL)
      {
         paf_c->caster = str_dup(ch->name);
      }
   }
   free_affect(paf);
   return;
}

void spell_chromatic_shield(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   CHAR_DATA* victim = (CHAR_DATA*) vo;
   AFFECT_DATA af;
   OBJ_DATA* brand;

   if (is_affected(victim, gsn_chromatic_shield))
   {
      if (victim == ch)
      {
         send_to_char
         (
            "You are already protected by a chromatic shield.\n\r",
            ch
         );
      }
      else
      {
         act
         (
            "$N is already protected by a chromatic shield.",
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
   if
   (
      is_affected(victim, gsn_steel_nerves) ||
      is_affected(victim, gsn_sanctuary)
   )
   {
      if (ch != victim)
      {
         act
         (
            "$N is already protected.\n\rYour chromatic aura fails to surround $M.",
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
            "You are already protected.\n\rThe chromatic aura fails to surround you.",
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
      "$n is surrounded by a chromatic aura.",
      victim,
      NULL,
      NULL,
      TO_ROOM
   );
   act
   (
      "You are surrounded by a chromatic aura.",
      victim,
      NULL,
      NULL,
      TO_CHAR
   );
   return;
}

void spell_power_word_recall(int sn, int level, CHAR_DATA* ch, void* vo, int target)
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
         " nullifies the spell!",
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
         send_to_char("You cannot recall, you are in a snare.\n\r", ch);
      }
      else
      {
         act
         (
            "You cannot recall $N, $e is in a snare.",
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

void spell_siren_screech(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   CHAR_DATA* vch;
   CHAR_DATA* vch_next;
   char buf[MAX_INPUT_LENGTH];
   int dam;
   bool startled;
   long id;
   bool npc;

   if (house_down(ch, HOUSE_CONCLAVE))
   {
      return;
   }
   if (is_mental(ch))
   {
      send_to_char
      (
         "You summon a beautiful siren!\n\r"
         "A siren takes a deep breath and releases a high pitched scream into the"
         " air!\n\r",
         ch
      );
      act
      (
         "$n summons a wailing banshee!\n\r"
         "A siren takes a deep breath and releases a high pitched scream into the"
         " air!",
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
         "You let out a shrill cry!\n\r",
         ch
      );
      act
      (
         "$n takes a deep breath and releases a high pitch wail into the air!",
         ch,
         NULL,
         NULL,
         TO_ROOM
      );
   }
   for (vch = ch->in_room->people; vch; vch = vch_next)
   {
      vch_next = vch->next_in_room;
      if
      (
         ch == vch ||
         is_same_group(ch, vch) ||
         wizi_to(vch, ch) ||
         is_safe(ch, vch, IS_SAFE_SILENT | IS_SAFE_AREA_ATTACK)
      )
      {
         continue;
      }
      if
      (
         check_immune(vch, DAM_SOUND) == IS_IMMUNE &&
         (
            check_immune(vch, DAM_BASH) == IS_IMMUNE ||
            (
               (
                  has_skill(vch, gsn_wraithform) &&
                  get_skill(vch, gsn_wraithform) >= 75
               ) ||
               (
                  IS_NPC(vch) &&
                  vch->pIndexData->vnum == MOB_VNUM_AIR_ELEMENTAL
               )
            )
         )
      )
      {
         act("$n seems amused.", vch, NULL, NULL, TO_ROOM);
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
            default:  /* 0 */
            {
               sprintf
               (
                  buf,
                  "Help! %s will not shut up!",
                  PERS(ch, vch)
               );
               break;
            }
            case (1):
            {
               sprintf
               (
                  buf,
                  "Help! %s is casting a spell on me!",
                  PERS(ch, vch)
               );
               break;
            }
            case (2):
            {
               sprintf
               (
                  buf,
                  "Die, %s, you sorcerous dog!",
                  PERS(ch, vch)
               );
               break;
            }
         }
         do_myell(vch, buf);
         sprintf
         (
            log_buf,
            "[%s] cast siren screech upon [%s] at %d",
            ch->name,
            vch->name,
            ch->in_room->vnum
         );
         log_string(log_buf);
      }
      if (check_spellcraft(ch, sn))
      {
         dam = spellcraft_dam(6, level);
      }
      else
      {
         dam = dice(6, level);
      }
      if
      (
         !is_affected(vch, gsn_shock_sphere) &&
         !number_bits(2) &&
         check_immune(vch, DAM_SOUND) != IS_IMMUNE &&
         vch->daze <= 0
      )
      {
         startled = TRUE;
         dam += number_range(-vch->level, vch->level * 2);
         act("$n seems startled.", vch, NULL, NULL, TO_ROOM);
         send_to_char("The viscious shriek catches you off guard.\n\r", vch);
      }
      else
      {
         startled = FALSE;
      }
      if (is_affected(vch, gsn_shock_sphere))
      {
         /* Deaf, quarter damage */
         dam /= 4;
      }
      id = vch->id;
      npc = IS_NPC(vch);
      if
      (
         saves_spell(ch, level, vch, DAM_SOUND, SAVE_SPELL) &&
         saves_spell(ch, level, vch, DAM_SOUND, SAVE_SPELL)
      )
      {
         dam /= 2;
      }
      damage(ch, vch, dam * race_adjust(ch) / 25.0, sn, DAM_SOUND, TRUE);
      if
      (
         /* Skip if dead/wimpied */
         !vch->valid ||
         vch->in_room != ch->in_room ||
         vch->ghost ||
         vch->id != id ||
         (
            !npc &&
            vch != id2name_room(id, ch->in_room, FALSE)
         )
      )
      {
         continue;
      }
      if
      (
         check_immune(vch, DAM_BASH) == IS_IMMUNE ||
         (
            (
               has_skill(vch, gsn_wraithform) &&
               get_skill(vch, gsn_wraithform) >= 75
            ) ||
            (
               IS_NPC(vch) &&
               vch->pIndexData->vnum == MOB_VNUM_AIR_ELEMENTAL
            )
         )
      )
      {
         act("$n seems amused by the winds.", vch, NULL, NULL, TO_ROOM);
         continue;
      }
      if (check_spellcraft(ch, sn))
      {
         dam = spellcraft_dam(1, level * 2);
      }
      else
      {
         dam = dice(1, level * 2);
      }
      if (startled)
      {
         dam += number_range(vch->level / 2, vch->level);
         act("$n is knocked back by the winds!", vch, NULL, NULL, TO_ROOM);
         send_to_char("The winds force you backwards!\n\r", vch);
         DAZE_STATE(vch, PULSE_VIOLENCE);
      }
      damage(ch, vch, dam, gsn_wail_wind, DAM_BASH, TRUE);
   }
   if (is_mental(ch))
   {
      act
      (
         "The siren vanishes back from whence it came.",
         ch,
         NULL,
         NULL,
         TO_ALL
      );
   }
}

void spell_phantasmal_force(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   AFFECT_DATA* paf;
   CHAR_DATA* mob;
   CHAR_DATA* attacker;
   OBJ_DATA* obj;
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   char buf[MAX_INPUT_LENGTH];
   int count;
   int chance;

   if (house_down(ch, HOUSE_CONCLAVE))
   {
      return;
   }
   target_name = one_argument_cs(target_name, arg1);
   target_name = one_argument_cs(target_name, arg2);
   if
   (
      is_affected(ch, gsn_phantasmal_force) ||
      is_affected(ch, gsn_phantasmal_force_area) ||
      is_affected(ch, gsn_phantasmal_force_illusion)
   )
   {
      send_to_char
      (
         "You are still concentrating on your last projection.\n\r",
         ch
      );
      return;
   }
   if
   (
      (
         arg1[0] == '\0' ||
         !str_cmp(arg1, "decoy")
      ) &&
      ch->level >= 35
   )
   {
      if (ch->fighting == NULL)
      {
         send_to_char
         (
            "There is no one fighting you to fall for the illusion.\n\r",
            ch
         );
         return;
      }
      /*
         Chance = 80% (when mastered), -2% for every victim int over 20,
         +2% for every victim int under 20
      */
      chance = get_skill(ch, sn);
      chance += 20;
      chance -= 2 * get_curr_stat(ch->fighting, STAT_INT);
      if (number_percent() > chance)
      {
         act
         (
            "$N did not seem distracted enough to fall for your illusion.",
            ch,
            NULL,
            ch->fighting,
            TO_CHAR
         );
         return;
      }
      mob = create_mobile(get_mob_index(MOB_VNUM_PHANTOM));
      if (mob == NULL)
      {
         bug
         (
            "Phantasmal force, phantom [%d] does not exist!",
            MOB_VNUM_PHANTOM
         );
         send_to_char
         (
            "OOC: A bug has occured, please pray about it.\n\r",
            ch
         );
         return;
      }
      clone_character(ch, mob, !IS_NPC(ch));
      if
      (
         (
            obj = get_eq_char(ch, WEAR_WIELD)
         ) != NULL &&
         obj->item_type == ITEM_WEAPON
      )
      {
         mob->dam_type = obj->value[3];
      }
      else
      {
         mob->dam_type = ch->dam_type;
      }
      /* Make illusion weak */
      mob->damage[0] = 0;
      mob->damage[1] = 0;
      mob->damage[2] = 0;
      mob->hitroll = -10;
      mob->damroll = 0;
      mob->mod_stat[STAT_STR] -= 100;
      mob->mod_stat[STAT_DEX] -= 100;
      paf = new_affect();
      paf->type = gsn_phantasmal_force;
      paf->level = level;
      paf->duration = level / 10;  /* Actual phantom will die quickly though */
      affect_to_char(ch, paf);
      paf->duration = -1;
      /* rounds it will survive after caster flees */
      paf->level = number_range(1, 4);
      affect_to_char(mob, paf);  /* Mob has the affect so it can go poof */
      free_affect(paf);
      mob->mprog_target = ch;  /* Bind to person */
      char_to_room(mob, ch->in_room);
      /* Replace fighters! */
      for
      (
         attacker = ch->in_room->people;
         attacker;
         attacker = attacker->next_in_room
      )
      {
         if (attacker->fighting == ch)
         {
            attacker->fighting = mob;
         }
      }
      send_to_char
      (
         "You project a phantasmal force to fight for you.\n\r",
         ch
      );
      return;
   }
   if
   (
      (
         !str_cmp(arg1, "area") ||
         !str_cmp(arg1, "image") ||
         !str_cmp(arg1, "images")
      ) &&
      ch->level >= 40
   )
   {
      if
      (
         ch->fighting != NULL &&
         !IS_TRUSTED(ch, IMMORTAL)
      )
      {
         send_to_char
         (
            "You cannot concentrate enough to project images.\n\r",
            ch
         );
         return;
      }
      paf = new_affect();
      paf->type = gsn_phantasmal_force_area;
      paf->level = level;
      paf->duration = level / 10;
      affect_to_char(ch, paf);
      free_affect(paf);
      send_to_char
      (
         "You project phantasmal images to confuse your enemies.\n\r",
         ch
      );
      return;
   }
   if (!str_cmp(arg1, "mob"))
   {
      if
      (
         ch->fighting != NULL &&
         !IS_TRUSTED(ch, IMMORTAL)
      )
      {
         send_to_char
         (
            "You cannot concentrate enough to project an illusion.\n\r",
            ch
         );
         return;
      }
      if (!IS_IMMORTAL(ch))
      {
         count = 0;
         /* Max 2 illusions for non imms (2 at lev 40) */
         for (mob = char_list; mob != NULL; mob = mob->next)
         {
            if
            (
               IS_NPC(mob) &&
               mob->pIndexData->vnum == MOB_VNUM_PHANTOM &&
               mob->mprog_target == ch &&
               ++count >= level / 10
            )
            {
               send_to_char("You can project no more illusions.\n\r", ch);
               return;
            }
         }
      }
      paf = new_affect();
      paf->type = gsn_phantasmal_force_illusion;
      paf->level = level;
      paf->duration = level / 5;
      affect_to_char(ch, paf);
      free_affect(paf);
      mob = create_mobile(get_mob_index(MOB_VNUM_PHANTOM));
      if (mob == NULL)
      {
         bug
         (
            "Phantasmal force, phantom [%d] does not exist!",
            MOB_VNUM_PHANTOM
         );
         send_to_char
         (
            "OOC: A bug has occured, please pray about it.\n\r",
            ch
         );
         return;
      }
      if (arg2[0] != '\0')
      {
         free_string(mob->short_descr);
         mob->short_descr = str_dup(arg2);
      }
      if (target_name[0] != '\0')
      {
         free_string(mob->long_descr);
         sprintf
         (
            buf,
            "%s\n\r",
            target_name
         );
         mob->long_descr = str_dup(buf);
      }
      sprintf(buf, "%s %s %s", mob->name, target_name, arg2);
      free_string(mob->name);
      mob->name = str_dup(buf);
      send_to_char
      (
         "You project a phantasmal illusion!\n\r",
         ch
      );
      mob->mprog_target = ch;  /* Bind to person */
      char_to_room(mob, ch->in_room);
      act("$n fades into existence.", mob, NULL, NULL, TO_ROOM);
      return;
   }
   if
   (
      (
         IS_IMMORTAL(ch) ||
         (
            !IS_NPC(ch) &&
            ch->pcdata->induct >= 5
         )
      ) &&
      !str_cmp(arg1, "obj")
   )
   {
      if
      (
         ch->fighting != NULL &&
         !IS_TRUSTED(ch, IMMORTAL)
      )
      {
         send_to_char
         (
            "You cannot concentrate enough to project an illusion.\n\r",
            ch
         );
         return;
      }
      paf = new_affect();
      paf->type = gsn_phantasmal_force_illusion;
      paf->level = level;
      paf->duration = level / 5;
      affect_to_char(ch, paf);
      free_affect(paf);
      obj = create_object(get_obj_index(OBJ_VNUM_PHANTOM), 0);
      if (obj == NULL)
      {
         bug
         (
            "Phantasmal force, phantom obj [%d] does not exist!",
            OBJ_VNUM_PHANTOM
         );
         send_to_char
         (
            "OOC: A bug has occured, please pray about it.\n\r",
            ch
         );
         return;
      }
      obj_to_char(obj, ch);
      if (arg2[0] != '\0')
      {
         free_string(obj->short_descr);
         obj->short_descr = str_dup(arg2);
      }
      if (target_name[0] != '\0')
      {
         free_string(obj->description);
         obj->description = str_dup(target_name);
      }
      sprintf(buf, "%s %s %s", obj->name, target_name, arg2);
      free_string(obj->name);
      obj->name = str_dup(buf);
      send_to_char
      (
         "You project a phantasmal illusion!\n\r",
         ch
      );
      act("$p fades into existence.", NULL, obj, NULL, TO_ALL);
      return;
   }
   send_to_char("Project what form of phantasmal force?\n\r", ch);
}

void spell_intangibility(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   AFFECT_DATA* paf;

   if (house_down(ch, HOUSE_CONCLAVE))
   {
      return;
   }
   if (is_affected(ch, sn))
   {
      send_to_char("You are already intangible.\n\r", ch);
      return;
   }
   if (is_affected(ch, gsn_intangibility_timer))
   {
      send_to_char
      (
         "You could not survive the stress of intangibility so soon.\n\r",
         ch
      );
      return;
   }
   paf = new_affect();
   paf->type      = sn;
   paf->duration  = -1;    /* Special countdown */
   paf->level     = 4;     /* Round counter */
   paf->where     = TO_AFFECTS;
   affect_to_char_1(ch, paf);
   paf->duration  = 10;
   paf->type = gsn_intangibility_timer;
   paf->bitvector2 = AFF_INTANGIBILITY;
   affect_to_char_1(ch, paf);
   free_affect(paf);
   send_to_char("You feel a chill as you become intangible.\n\r", ch);
   act
   (
      "$n seems to shudder a bit as $e becomes intangible.",
      ch,
      NULL,
      NULL,
      TO_ROOM
   );
}

void spell_prismatic_sphere(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   if (house_down(ch, HOUSE_CONCLAVE))
   {
      return;
   }
   send_to_char
   (
      "OOC: This spell has not yet been coded.\n\r",
      ch
   );
   return;
}

void spell_simulacrum(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   AFFECT_DATA* paf;
   CHAR_DATA* gch;

   if (house_down(ch, HOUSE_CONCLAVE))
   {
      return;
   }
   if (check_simulacrum(ch) >= 0)
   {
      send_to_char("Reality is already distorted around you.\n\r", ch);
      return;
   }
   if (ch->fighting != NULL)
   {
      send_to_char
      (
         "You are causing too much of a commotion to distort reality.\n\r",
         ch
      );
      return;
   }
   for (gch = ch->in_room->people; gch; gch = gch->next_in_room)
   {
      if
      (
         is_same_group(ch, gch) &&
         gch->fighting != NULL
      )
      {
         act
         (
            "$N is causing too much of a commotion to distort reality.",
            ch,
            NULL,
            gch,
            TO_CHAR
         );
         return;
      }
   }
   paf = new_affect();
   paf->where = TO_AFFECTS;
   paf->modifier = ch->in_room->sector_type;
   paf->type = sn;
   paf->duration = -1;
   affect_to_char(ch, paf);
   free_affect(paf);
   act("Reality seems to become distorted.", ch, NULL, NULL, TO_ALL);
   for (gch = ch->in_room->people; gch; gch = gch->next_in_room)
   {
      if
      (
         !IS_NPC(gch) &&
         is_same_group(ch, gch)
      )
      {
         simulacrum_on(gch);
      }
   }
   return;
}

void spell_rite_of_darkness(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   AFFECT_DATA af;
   CHAR_DATA* vch;
   CHAR_DATA* vch_next;

   if (house_down(ch, HOUSE_CONCLAVE))
   {
      return;
   }

   if (is_affected(ch, gsn_rite_of_darkness_timer))
   {
      send_to_char(
         "You cannot yet bring yourself to cast your soul into hell again.\n\r",
         ch);
      return;
   }

   act(
      "A swirling gateway appears as you complete the Rite of Darkness!",
      ch,
      NULL,
      NULL,
      TO_CHAR);

   act(
      "A swirling gateway appears as $n completes the Rite of Darkness!",
      ch,
      NULL,
      NULL,
      TO_ROOM);

   act(
      "The soul of Shal'ne-Sir, Lord of DragonLiches, "
      "erupts forth to possess a body!",
      ch,
      NULL,
      NULL,
      TO_ALL);

   act(
      "$n's face and body contort violently into a half-dracolich form!",
      ch,
      NULL,
      NULL,
      TO_ROOM);

   act(
      "Your soul shudders as you merge with the Dracolich Shal'ne-Sir!",
      ch,
      NULL,
      NULL,
      TO_CHAR);

   af.where     = TO_AFFECTS;
   af.type      = sn;
   af.level     = level;
   af.duration  = 5;
   af.location  = APPLY_MORPH_FORM;
   af.modifier  = 19;
   af.bitvector = 0;
   affect_to_char(ch, &af);

   af.location  = APPLY_AC;
   af.modifier  = -200;
   af.bitvector = AFF_FLYING;
   affect_to_char(ch, &af);

   af.location  = APPLY_SPELL_POWER;
   af.modifier  = 10;
   af.bitvector = AFF_INFRARED;
   affect_to_char(ch, &af);

   af.location  = APPLY_STR;
   af.modifier  = 1 + (level >= 18) + (level >= 25) + (level >= 32);
   af.bitvector = 0;
   affect_to_char(ch, &af);

   af.location  = APPLY_MAX_STR;
   affect_to_char(ch, &af);

   af.type      = gsn_rite_of_darkness_timer;
   af.location  = APPLY_NONE;
   af.duration  = 48;
   af.modifier  = 0;
   affect_to_char(ch, &af);

   if (ch->in_room == NULL)
   {
      return;
   }

   af.type = gsn_rite_of_darkness_taint;
   af.duration = 5;

   for (vch = char_list; vch != NULL; vch = vch_next)
   {
      vch_next = vch->next;

      if (
            ch == vch ||
            vch->in_room == NULL
         )
      {
         continue;
      }

      if (vch->in_room->area == ch->in_room->area)
      {
         send_to_char(
            "Waves of unspeakable evil wash over you as the seal "
            "to Hell is broken!\n\r",
            vch);

         if (
               (IS_IMMORTAL(vch) && !can_see(ch,vch)) ||
               is_affected(vch, gsn_rite_of_darkness_taint)
            )
         {
            continue;
         }

         if (vch->in_room == ch->in_room)
         {
            if (is_same_group(vch,ch))
            {
               if (IS_EVIL(vch))
               {
                  send_to_char(
                     "You smile as the righteous evil of the "
                     "Rite of Darkness washes over you.\n\r",
                     vch);

                  af.location  = APPLY_AC;
                  af.modifier  = -30;
                  af.bitvector = AFF_FLYING;
                  affect_to_char(vch, &af);

                  af.location  = APPLY_SPELL_POWER;
                  af.modifier  = 3;
                  af.bitvector = AFF_INFRARED;
                  affect_to_char(vch, &af);
               }
               else if (IS_NEUTRAL(vch))
               {
                  send_to_char(
                     "You shudder and wince as the "
                     "Rite of Darkness washes over you.\n\r",
                     vch);

                  af.location  = APPLY_AC;
                  af.modifier  = -10;
                  af.bitvector = 0;
                  affect_to_char(vch, &af);

                  af.location  = APPLY_SPELL_POWER;
                  af.modifier  = 1;
                  af.bitvector = AFF_INFRARED;
                  affect_to_char(vch, &af);

                  if(number_percent() < 11)
                  {
                     act(
                       "$n suddenly gets an unexpectedly evil gleam in $s eye.",
                       vch,
                       NULL,
                       NULL,
                       TO_ROOM);
                     act(
                       "The deep evil of Shal'ne-Sir has corrupted your soul!",
                       vch,
                       NULL,
                       NULL,
                       TO_CHAR);

                     af.location = APPLY_ALIGN;
                     af.modifier = -ALIGN_STEP;
                     af.bitvector = 0;
                     affect_to_char(vch, &af);
                  }
               }
               else
               {
                  send_to_char(
                     "Your soul howls in agony as the "
                     "Rite of Darkness washes over you.\n\r",
                     vch);

                  damage(
                     ch,
                     vch,
                     dice(3, 50),
                     sn,
                     DAM_OTHER,
                     TRUE);

                  if(number_percent() < 31)
                  {
                     act(
                       "$n suddenly gets an unexpectedly evil gleam in $s eye.",
                       vch,
                       NULL,
                       NULL,
                       TO_ROOM);
                     act(
                       "The deep evil of Shal'ne-Sir has corrupted your soul!",
                       vch,
                       NULL,
                       NULL,
                       TO_CHAR);

                     af.location = APPLY_ALIGN;
                     af.modifier = -ALIGN_STEP;
                     af.bitvector = 0;
                     af.duration = 5;
                     affect_to_char(vch, &af);
                  }
               }
            }
            else
            {
               if (
                     vch != ch &&
                     !is_safe(ch, vch, IS_SAFE_SILENT | IS_SAFE_AREA_ATTACK)
                  )
               {
                  spell_power_word_fear(
                     gsn_rite_of_darkness,
                     level,
                     ch,
                     vch,
                     TAR_CHAR_OFFENSIVE);
               }
            }
         }
      }
   }
}

void spell_call_of_duty(int sn, int level, CHAR_DATA* ch, void* vo, int target)
{
   ROOM_INDEX_DATA* room;
   int cnt;
   int home;
   bool criminal;
   bool ride;
   bool charmies_came;
   CHAR_DATA* crim;
   CHAR_DATA* pet;
   CHAR_DATA* pet_next;
   AFFECT_DATA* paf;
   char buf[MAX_STRING_LENGTH];

   if (house_down(ch, HOUSE_ENFORCER))
   {
      return;
   }
   if (ch->fighting != NULL)
   {
      send_to_char("You cannot concentrate enough.\n\r", ch);
      return;
   }
   if (IS_SET(ch->in_room->extra_room_flags, ROOM_1212))
   {
      send_to_char("You cannot receives calls of duty from here.\n\r", ch);
      return;
   }
   if (target_name[0] == '\0')
   {
      send_to_char("Where does your duty call you?\n\r", ch);
      return;
   }
   if
   (
      !str_prefix(target_name, "set") ||
      !str_prefix(target_name, "mark")
   )
   {
      if
      (
         ch->in_room->guild ||
         (
            ch->in_room->house &&
            ch->in_room->house != ch->house
         )
      )
      {
         send_to_char("Duty will not call you back here.\n\r", ch);
         return;
      }
      send_to_char
      (
         "You make a mental note of the area in case duty calls you back.\n\r",
         ch
      );
      ch->pcdata->linked = ch->in_room;
      return;
   }
   /* Take you into cursed areas, not out of them */
   if
   (
      IS_SET(ch->in_room->room_flags, ROOM_NO_GATE) ||
      (
         (
            IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL) ||
            IS_SET(ch->in_room->extra_room_flags, ROOM_DEAD_ZONE)
         ) &&
         !IS_SET(ch->in_room->room_flags, ROOM_CONSECRATED)
      )
   )
   {
      send_to_char
      (
         "Duty will bring you into danger, not out of it.\n\r"
         "You must walk.\n\r",
         ch
      );
      return;
   }
   if
   (
      !str_prefix(target_name, "go") ||
      !str_prefix(target_name, "ride")
   )
   {
      room = ch->pcdata->linked;
      if (room == NULL)
      {
         send_to_char("You have not remembered any place to go.\n\r", ch);
         return;
      }
      ride = TRUE;
   }
   else
   {
      ride = FALSE;
      home = -1;
      for (cnt = 0; hometown_table[cnt].name != NULL; cnt++)
      {
         if
         (
            str_prefix(target_name, hometown_table[cnt].name) ||
            !check_room_protected
            (
               (room = get_room_index(hometown_table[cnt].vnum))
            )
         )
         {
            continue;
         }
         if
         (
            (
               ch->alignment > 0 &&
               hometown_table[cnt].allow_good
            ) ||
            (
               ch->alignment == 0 &&
               hometown_table[cnt].allow_neutral
            ) ||
            (
               ch->alignment < 0 &&
               hometown_table[cnt].allow_evil
            )
         )
         {
            home = cnt;
            break;  /* accept room */
         }
         if
         (
            home == -1 ||
            (
               (
                  ch->alignment != 0 &&
                  !hometown_table[home].allow_neutral
               ) &&
               hometown_table[cnt].allow_neutral
            )
         )
         {
            /* If good or evil, and can't find exact, try for 'neutral' */
            home = cnt;
         }
      }
      if
      (
         home == -1 ||
         (
            room = get_room_index(hometown_table[home].vnum)
         ) == NULL
      )
      {
         send_to_char("Answer the call of duty to where?\n\r", ch);
         return;
      }
   }
   if (room == NULL)  /* sanity check */
   {
      send_to_char("Answer the call of duty to where?\n\r", ch);
      return;
   }
   if (IS_SET(room->extra_room_flags, ROOM_1212))
   {
      send_to_char("You cannot receives calls of duty to there.\n\r", ch);
      return;
   }
   criminal = FALSE;
   for (crim = char_list; crim; crim = crim->next)
   {
      if
      (
         !check_room(crim, FALSE, "") ||
         IS_NPC(crim) ||
         IS_IMMORTAL(crim) ||
         crim->in_room->area != room->area ||
         (
            !IS_SET(crim->act, PLR_CRIMINAL) &&
            !IS_SET(crim->act2, PLR_LAWLESS)
         )
      )
      {
         continue;
      }
      criminal = TRUE;
      break;
   }
   if
   (
      ride &&
      !criminal
   )
   {
      send_to_char("Duty does not call you there at this moment.\n\r", ch);
      return;
   }
   if
   (
      !criminal &&
      ch->quittime >= 17
   )
   {
      send_to_char
      (
         "Duty will not save you; it will only bring you into more danger.\n\r",
         ch
      );
      return;
   }
   charmies_came = FALSE;
   paf = new_affect();
   paf->type = sn;
   for (pet = ch->in_room->people; pet; pet = pet_next)
   {
      pet_next = pet->next_in_room;
      if
      (
         IS_NPC(pet) &&
         IS_AFFECTED(pet, AFF_CHARM) &&
         pet->master == ch
      )
      {
         charmies_came = TRUE;
         affect_to_char_1(pet, paf);
         char_from_room(pet);
         passive_visible(pet);
         char_to_room(pet, room);
      }
   }
   free_affect(paf);
   sprintf
   (
      buf,
      "You respond to the Call of Duty.\n\r"
      "%s energy surrounds you, %sand you are whisked away.\n\r",
      (
         is_supplicating ?
         "Holy" :
         "Magical"
      ),
      (
         charmies_came ?
         "and your followers " :
         ""
      )
   );
   send_to_char
   (
      buf,
      ch
   );
   sprintf
   (
      buf,
      "%s energy surrounds $n, %sand $e is whisked away.",
      (
         is_supplicating ?
         "Holy" :
         "Magical"
      ),
      (
         charmies_came ?
         "and $s followers " :
         ""
      )
   );
   act
   (
      buf,
      ch,
      NULL,
      NULL,
      TO_ROOM
   );
   char_from_room(ch);
   passive_visible(ch);
   char_to_room(ch, room);
   for (pet = ch->in_room->people; pet; pet = pet_next)
   {
      pet_next = pet->next_in_room;
      if
      (
         IS_NPC(pet) &&
         IS_AFFECTED(pet, AFF_CHARM) &&
         pet->master == ch &&
         is_affected(pet, sn)
      )
      {
         affect_strip(pet, sn);
         if (pet->desc)
         {
            do_observe(pet, "", LOOK_AUTO);
         }
      }
   }
   do_observe(ch, "", LOOK_AUTO);
   sprintf
   (
      buf,
      "%s energy fills the room, and $n appears before you%s.",
      (
         is_supplicating ?
         "Holy" :
         "Magical"
      ),
      (
         charmies_came ?
         " with $s entourage" :
         ""
      )
   );
   act
   (
      buf,
      ch,
      NULL,
      NULL,
      TO_ROOM
   );
   return;
}
