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

static const char rcsid[] = "$Id: act_obj.c,v 1.200 2004/11/25 08:52:20 fizzfaldt Exp $";

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
#include "recycle.h"
#include "magic.h"
#include "tables.h"
#include "brandpower.h"

/* Command Procedures Needed */
DECLARE_DO_FUN    ( do_rest     );
DECLARE_DO_FUN    ( do_sleep    );
DECLARE_DO_FUN    ( do_auction  );
DECLARE_DO_FUN    ( do_split    );
DECLARE_DO_FUN    ( do_myell    );
DECLARE_DO_FUN    ( do_yell     );
DECLARE_DO_FUN    ( do_say      );
DECLARE_DO_FUN    ( do_wake     );
DECLARE_DO_FUN    ( do_emote    );
DECLARE_DO_FUN    ( do_bite    );
void  house_key_trigger    args( (CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA *obj) );
void  skill_gain           args( (CHAR_DATA* ch, char* sk, sh_int lvl) );

/* Local Functions */
#define CD CHAR_DATA
#define OD OBJ_DATA
bool trigger_desire            args(( CHAR_DATA *ch, OBJ_DATA *desire ));
bool can_loot                  args(( CHAR_DATA *ch, OBJ_DATA *obj ));
bool get_obj                   args(( CHAR_DATA *ch, OBJ_DATA *obj, OBJ_DATA *container, long objCount ));
void get_obj_sword_demon       args(( CHAR_DATA *ch, OBJ_DATA *obj ));
bool check_social              args(( CHAR_DATA *ch, char *command, char *argument ));
bool remove_obj                args(( CHAR_DATA *ch, int iWear, bool fReplace, bool show ));
bool wear_obj                  args(( CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace ));
int  get_cost                  args(( CHAR_DATA *keeper, OBJ_DATA *obj, bool fBuy ));
CD * find_keeper               args( (CHAR_DATA* ch, char* argument) );
bool is_occupied               args(( int vnum ));
OD * get_obj_keeper            args(( CHAR_DATA *ch, CHAR_DATA *keeper, char *argument ));
bool sacrifice_obj_bracelet    args(( CHAR_DATA *ch, OBJ_DATA *obj ));
void wear_obj_tiara            args(( CHAR_DATA *ch, OBJ_DATA *obj ));
void wear_obj_wizardry         args(( CHAR_DATA *ch, OBJ_DATA *obj ));
void obj_brandish_horn_valere  args(( CHAR_DATA *ch, OBJ_DATA *obj ));
void obj_give_hydra_head       args(( CHAR_DATA *ch, CHAR_DATA *lady, OBJ_DATA *obj ));
void wear_obj_zhentil_robe     args(( CHAR_DATA *ch, OBJ_DATA *obj ));
void wear_obj_whitehelm        args(( CHAR_DATA *ch, OBJ_DATA *obj ));
void wear_obj_fallen_wings     args(( CHAR_DATA *ch, OBJ_DATA *obj ));
void wear_obj_black_hide       args(( CHAR_DATA *ch, OBJ_DATA *obj ));
void wear_obj_shadow_hide      args(( CHAR_DATA *ch, OBJ_DATA *obj ));
void wear_obj_symbol_magic     args(( CHAR_DATA *ch, OBJ_DATA *obj ));
void wear_obj_red_dragonstar   args(( CHAR_DATA *ch, OBJ_DATA *obj ));
void wear_obj_black_dragonstar args(( CHAR_DATA *ch, OBJ_DATA *obj ));
void wear_obj_genocide         args(( CHAR_DATA *ch, OBJ_DATA *obj ));
void remove_obj_genocide       args(( CHAR_DATA *ch, OBJ_DATA *obj ));
void recite_libram_conjuration args(( CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA *scroll, OBJ_DATA *obj, bool fRead ));
void brandish_sceptre_dead     args(( CHAR_DATA *ch, OBJ_DATA *obj ));
void brandish_staff_fectus     args(( CHAR_DATA *ch, OBJ_DATA *obj ));
void brandish_soul_orb         args(( CHAR_DATA *ch, OBJ_DATA *staff ));
void wear_obj_cloak_underworld args(( CHAR_DATA *ch, OBJ_DATA *obj ));
void wear_obj_misty_cloak      args(( CHAR_DATA *ch, OBJ_DATA *obj ));
void wear_obj_troll_skin       args(( CHAR_DATA *ch, OBJ_DATA *obj ));
void wear_obj_ring_protection  args(( CHAR_DATA *ch, OBJ_DATA *obj ));
void wear_obj_robe_avenger     args(( CHAR_DATA *ch, OBJ_DATA *obj ));
void wear_obj_sword_demon      args(( CHAR_DATA *ch, OBJ_DATA *obj ));
void wear_obj_physical_focus   args(( CHAR_DATA *ch, OBJ_DATA *obj ));
void wear_obj_mental_focus     args(( CHAR_DATA *ch, OBJ_DATA *obj ));
void hold_summoning_stone      args(( CHAR_DATA *ch, OBJ_DATA *obj ));
int  hoard_check               args(( ROOM_INDEX_DATA *room ));
int  hoard_container_check     args(( OBJ_DATA *container, int count ));
void crusader_druid_prog       args(( CHAR_DATA *ch, CHAR_DATA *smith,
OBJ_DATA *weapon));
#undef CD
#undef OD
void stray_eat   args( (CHAR_DATA* stray, OBJ_DATA* food, CHAR_DATA* giver) );
void stray_drink args( (CHAR_DATA* stray, OBJ_DATA* food, CHAR_DATA* giver) );


bool check_absorb_cast(CHAR_DATA* ch, CHAR_DATA* victim, int sn)
{
   if
   (
      sn < 1 ||
      sn >= MAX_SKILL ||
      (
         !IS_NPC(ch) &&
         !skill_table[sn].spells.absorb_pc
      ) ||
      (
         IS_NPC(ch) &&
         !skill_table[sn].spells.absorb_npc
      ) ||
      !check_absorb(ch, victim, sn)
   )
   {
      return FALSE;
   }
   return TRUE;
}

bool spellbaned_cast(CHAR_DATA* ch, CHAR_DATA* victim, int sn)
{
   if
   (
      sn < 1 ||
      sn >= MAX_SKILL ||
      (
         !IS_NPC(ch) &&
         !skill_table[sn].spells.spellbane_pc
      ) ||
      (
         IS_NPC(ch) &&
         !skill_table[sn].spells.spellbane_npc
      ) ||
      !spellbaned(ch, victim, sn)
   )
   {
      return FALSE;
   }
   return TRUE;
}

bool trigger_desire( CHAR_DATA *ch, OBJ_DATA *desire )
{
   AFFECT_DATA af;

   if ( !desire || !desire->pIndexData )
   return FALSE;
   if ( desire->pIndexData->vnum != 511 )
   return FALSE;
   if ( IS_IMMORTAL(ch) )
   return FALSE;

   af.where     = TO_AFFECTS;
   af.type      = gsn_sleep;
   af.level     = 60;
   af.duration  = 20;
   af.location  = APPLY_NONE;
   af.modifier  = 0;
   af.bitvector = AFF_SLEEP;
   affect_join( ch, &af, -1 );

   if ( IS_AWAKE(ch) )
   {
      send_to_char( "You feel very sleepy ..... zzzzzz.\n\r", ch );
      act( "$n goes to sleep.", ch, NULL, NULL, TO_ROOM );
      ch->position = POS_SLEEPING;
   }

   return TRUE;
}

/* RT part of the corpse looting code */
bool can_loot( CHAR_DATA *ch, OBJ_DATA *obj )
{
   char buf[MAX_STRING_LENGTH];

   /* always loot own corpse*/
   if (!str_cmp(obj->owner, ch->name)) return TRUE;
   if ( !IS_IMMORTAL(ch) )
   {
      /* Players without proper descriptions are not allowed to loot PC corpses */
      if ( !IS_NPC(ch)
      && !IS_AFFECTED(ch, AFF_CHARM)
      && (obj->item_type == ITEM_CORPSE_PC)
      && ((strlen(ch->description) < 100)
      || ((strlen(ch->pcdata->psych_desc) < 2) && (ch->level >= 30))) )
      return FALSE;

      /* Players without proper descriptions are not allowed to loot PC corpses
      by using their charmed mob's, -or- their charmed PC's */
      if ( IS_AFFECTED(ch, AFF_CHARM)
      && (ch->master != NULL && !IS_NPC(ch->master))
      && (obj->item_type == ITEM_CORPSE_PC)
      && ((strlen(ch->master->description) < 100)
      || ((strlen(ch->master->pcdata->psych_desc) < 2) && (ch->master->level >= 30))) )
      return FALSE;
   }

   if ( (obj->item_type == ITEM_CORPSE_PC) && (obj->contains) )
   {
      if ( !IS_NPC(ch) && IS_AFFECTED(ch, AFF_CHARM) )
      {
         sprintf( buf, "%s (charmed) looted %s [room %d]",
         IS_NPC(ch) ? ( !ch->master ? "Unknown mob" : ch->master->name) : ch->name,
         obj->short_descr, ch->in_room->vnum);
         log_string(buf);
      }
      else
      {
         sprintf( buf, "%s looted %s [room %d]",
         IS_NPC(ch) ? ( !ch->master ? "Unknown mob" : ch->master->name) : ch->name,
         obj->short_descr, ch->in_room->vnum);
         log_string(buf);
      }

      if (!IS_IMMORTAL(ch))
      {
         if
         (
            IS_NPC(ch) &&
            ch->master &&
            obj->owner &&
            str_cmp(obj->owner, ch->master->name)
         )
         {
            if (!IS_IMMORTAL(ch->master))
            {
               QUIT_STATE(ch->master, 20);
            }
         }
         else if ( !IS_NPC(ch)  && (obj->owner) && str_cmp(ch->name, obj->owner) )
         QUIT_STATE(ch, 20);
      }
   }
   return TRUE;
}

/* returns FALSE if a loop should end */
bool get_obj(CHAR_DATA* ch, OBJ_DATA* obj, OBJ_DATA* container, long objCount)
{
   /* variables for AUTOSPLIT */
   CHAR_DATA* vch;
   int      members;
   char     buffer[100];
   char     buf[MAX_STRING_LENGTH];
   OBJ_DATA* tempObj = NULL;
   int      num;

   if
   (
      !obj->valid ||
      obj->carried_by
   )
   {
      return FALSE;
   }
   if (!CAN_WEAR(obj, ITEM_TAKE))
   {
      send_to_char("You cannot take that.\n\r", ch);
      return TRUE;
   }
   if
   (
      obj->pIndexData->vnum == OBJ_VNUM_PHYLACTERY &&
      str_cmp(obj->owner, ch->name)
   )
   {
      send_to_char("Some malevolent force prevents you from picking that up.\n\r", ch);
      return TRUE;
   }

   if
   (
      (
         num = get_obj_number(obj)
      ) &&
      num + ch->carry_number > can_carry_n(ch)
   )
   {
      act("$P: you cannot carry that many items.", ch, NULL, obj, TO_CHAR);
      return FALSE;
   }

   if ( (obj->item_type == ITEM_MONEY) && (objCount != 0) )
   {
      long silver = 0,
      gold   = 0;

      if ( objCount > 0 )
      gold   = objCount;
      else
      silver = -1*objCount;

      if ( (gold > obj->value[1]) || (silver > obj->value[0]) )
      {
         send_to_char( "You can't find that much here.\n\r", ch );
         return FALSE;
      }

      tempObj = obj;
      obj = create_money( gold, silver );
   }

   if
   (
      (
         !obj->in_obj ||
         obj->in_obj->carried_by != ch
      ) &&
      (
         get_carry_weight(ch) + get_obj_weight(obj) > can_carry_w(ch)
      )
   )
   {
      act( "$P: you can't carry that much weight.", ch, NULL, obj, TO_CHAR );
      if (tempObj && obj)
      {
         extract_obj( obj, FALSE );
      }
      return get_carry_weight(ch) < can_carry_w(ch);
   }

   if ( !can_loot(ch, obj) )
   {
      act( "Corpse looting is not permitted.", ch, NULL, NULL, TO_CHAR );
      if ( tempObj && obj )
      {
         extract_obj( obj, FALSE );
      }
      return FALSE;
   }

   if ( obj->in_room )
   {
      for ( vch = obj->in_room->people; vch; vch = vch->next_in_room )
      if ( (vch->on == obj)
      || (vch->is_riding
      && (vch->is_riding->mount_type == MOUNT_OBJECT)
      && ((OBJ_DATA*) vch->is_riding->mount == obj)) )
      {
         act("$N appears to be using $p.", ch, obj, vch, TO_CHAR);
         if ( tempObj && obj )
         {
            extract_obj( obj, FALSE );
         }
         return TRUE;
      }
   }

   if ( container )
   {
      act( "You get $p from $P.", ch, obj, container, TO_CHAR );
      if ( IS_AFFECTED(ch, AFF_SNEAK)
      && (ch->invis_level < LEVEL_HERO)
      && (ch->in_room) )
      for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
      {
         if (IS_SET(vch->act, ACT_HOLYLIGHT))
         {
            sprintf( buf, "$n gets %s from $p.", obj->short_descr );
            act( buf, ch, container, vch, TO_VICT );
         }
      }
      if ( !IS_AFFECTED(ch, AFF_SNEAK) )
      act( "$n gets $p from $P.", ch, obj, container, TO_ROOM );
      if ( !tempObj )
      obj_from_obj( obj );
   }
   else
   {
      act( "You get $p.", ch, obj, container, TO_CHAR );

      if ( IS_AFFECTED(ch, AFF_SNEAK) && (ch->invis_level < LEVEL_HERO) )
      for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
      if (IS_SET(vch->act, ACT_HOLYLIGHT))
      act( "$n gets $p.", ch, obj, vch, TO_VICT );

      if ( !IS_AFFECTED(ch, AFF_SNEAK) )
      act( "$n gets $p.", ch, obj, container, TO_ROOM );

      for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
      {
         if
         (
            IS_NPC(vch) &&
            vch->spyfor &&
            ch != vch->spyfor
         )
         {
            sprintf
            (
               buf, "Your lookout informs you that %s was taken from %s.\n\r",
               (
                  can_see_obj(vch, obj) ?
                  (
                     obj->short_descr ?
                     obj->short_descr :
                     "something"
                  ) :
                  "something"
               ),
               vch->in_room->name
            );
            send_to_char( buf, vch->spyfor );
         }
      }

      if ( !tempObj )
      obj_from_room( obj );
   }

   if ( obj->item_type == ITEM_MONEY)
   {
      ch->silver += obj->value[0];
      ch->gold += obj->value[1];
      if ( tempObj )
      {
         tempObj->value[0] -= obj->value[0];
         tempObj->value[1] -= obj->value[1];
         if ( (tempObj->value[0] > 0)
         || (tempObj->value[1] > 0) )
         {
            if ( container )
            obj_to_obj(create_money(tempObj->value[1], tempObj->value[0]), container);
            else
            obj_to_room(create_money(tempObj->value[1], tempObj->value[0]), ch->in_room );
         }
         extract_obj( tempObj, FALSE );
         tempObj = NULL;
      }

      /* AUTOSPLIT code */
      if ( IS_SET(ch->act, PLR_AUTOSPLIT) )
      {
         members = 0;
         for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
         if ( !IS_AFFECTED(vch, AFF_CHARM) && is_same_group( vch, ch ) )
         members++;
         if ( (members > 1)
         && ( (obj->value[0] > 1) || obj->value[1] ) )
         {
            sprintf( buffer, "%d %d", obj->value[0], obj->value[1] );
            do_split( ch, buffer );
         }
      }
      extract_obj( obj, FALSE );
   }
   else
   {
      if ( (obj->pIndexData->condition < 0) && (!obj->pIndexData->quitouts) )
      {
         obj->timer = (-1)*obj->pIndexData->condition;
         REMOVE_BIT(obj->extra_flags, ITEM_ROT_DEATH);
      }
      obj_to_char( obj, ch );
   }

   if ( obj->pIndexData->vnum == OBJ_VNUM_SWORD_DEMONS )
   get_obj_sword_demon(ch, obj);

   if ( tempObj && obj )
   extract_obj( obj, FALSE );
   return TRUE;
   /* Coil of snakes disabled... Why? Who? - Fizzfaldt */
   if (obj->pIndexData->vnum == OBJ_VNUM_COIL_SNAKES)
   {
      if (!wear_obj(ch, obj , TRUE))
      {
         return TRUE;
      }
   }
}

void get_obj_sword_demon( CHAR_DATA *ch, OBJ_DATA *obj )
{
   send_to_char( "The Sword of Demons grins at you.\n\r", ch );
   return;
}

void do_hordelist( CHAR_DATA *ch, char *argument )
{
   char            arg[MAX_INPUT_LENGTH];
   OBJ_DATA        *pobj, *obj;
   ROOM_INDEX_DATA *room;
   char            buf   [MAX_STRING_LENGTH],
   bufcat[MAX_STRING_LENGTH];
   int             count, limitedobjs;
   bool            Found = FALSE;
   AREA_DATA* parea;

   argument = one_argument( argument, arg );

   for (parea = area_first; parea; parea = parea->next)
   {
      for (room = parea->rooms; room; room = room->next_in_area)
      {
         count = 0;
         limitedobjs = 0;
         pobj = NULL;
         for ( obj = room->contents; obj; obj = obj->next_content )
         {
            if
            (
               CAN_WEAR(obj, ITEM_TAKE) &&
               (
                  (
                     pobj &&
                     (
                        pobj->pIndexData->vnum != obj->pIndexData->vnum
                     )
                  ) ||
                  !pobj
               ) &&
               obj->level > 10 &&
               (
                  obj->pIndexData->vnum < room->area->min_vnum ||
                  obj->pIndexData->vnum > room->area->max_vnum
               ) &&
               !(
                  obj->pIndexData->vnum >= 28100 &&
                  obj->pIndexData->vnum &&
                  room->area->min_vnum >= 28100 &&
                  room->area->max_vnum <= 28199
               ) &&
               (
                  obj->pIndexData->vnum < 3000 ||
                  obj->pIndexData->vnum > 3399
               ) &&
               (
                  obj->item_type == ITEM_WEAPON ||
                  obj->item_type == ITEM_ARMOR ||
                  obj->pIndexData->limtotal > 0 ||
                  obj->item_type == ITEM_CLOTHING
               ) &&
               (
                  strcmp(arg, "limited") ||
                  (
                     !strcmp(arg, "limited") &&
                     obj->pIndexData->limtotal > 0
                  )
               )
            )
            {
               count++;
               if (obj->pIndexData->limtotal > 0) limitedobjs++;
            }
            if
            (
               obj->item_type == ITEM_CONTAINER &&
               CAN_WEAR(obj, ITEM_TAKE) &&
               (
                  obj->pIndexData->vnum < room->area->min_vnum ||
                  obj->pIndexData->vnum > room->area->max_vnum
               )
            )
            count = hoard_container_check( obj, count );
            pobj = obj;
         }

         if (count >= 3)
         {
            if (!Found)
            {
               Found = TRUE;
               send_to_char( "Room#  Limited  Item Levels\n\r", ch );
            }
            sprintf( buf, "%5d  %7d ", room->vnum, limitedobjs );
            pobj = NULL;
            for ( obj = room->contents; obj; obj = obj->next_content )
            {
               if
               (
                  CAN_WEAR(obj, ITEM_TAKE) &&
                  (
                     (
                        pobj &&
                        pobj->pIndexData->vnum != obj->pIndexData->vnum
                     ) ||
                     !pobj
                  ) &&
                  obj->level > 10 &&
                  (
                     obj->pIndexData->vnum < 3000 ||
                     obj->pIndexData->vnum > 3399
                  ) &&
                  (
                     obj->item_type == ITEM_WEAPON ||
                     obj->item_type == ITEM_ARMOR ||
                     obj->pIndexData->limtotal > 0 ||
                     obj->item_type == ITEM_CLOTHING
                  ) &&
                  (
                     strcmp(arg, "limited") ||
                     (
                        !strcmp(arg, "limited") &&
                        obj->pIndexData->limtotal > 0
                     )
                  )
               )
               {
                  sprintf( bufcat, " %d", obj->level );
                  strcat( buf, bufcat );
               }
               pobj = obj;
            }
            strcat( buf, "\n\r" );
            send_to_char( buf, ch );
         }
      }
   }

   if ( !Found )
   send_to_char( "No hoards found.\n\r", ch );

   return;
}

int hoard_check( ROOM_INDEX_DATA *room )
{
   OBJ_DATA *pobj,
   *obj;
   int      count = 0;

   if ( room )
   {
      count = 0;
      pobj = NULL;
      for ( obj = room->contents; obj; obj = obj->next_content )
      {
         if
         (
            CAN_WEAR(obj, ITEM_TAKE) &&
            (
               (
                  pobj &&
                  pobj->pIndexData->vnum != obj->pIndexData->vnum
               ) ||
               !pobj
            ) &&
            obj->level > 10 &&
            (
               obj->pIndexData->vnum < room->area->min_vnum ||
               obj->pIndexData->vnum > room->area->max_vnum
            ) &&
            !(
               obj->pIndexData->vnum >= 28100 &&
               obj->pIndexData->vnum &&
               room->area->min_vnum >= 28100 &&
               room->area->max_vnum <= 28199
            ) &&
            (
               obj->pIndexData->vnum < 3000 ||
               obj->pIndexData->vnum > 3399
            ) &&
            (
               obj->item_type == ITEM_WEAPON ||
               obj->item_type == ITEM_ARMOR ||
               obj->pIndexData->limtotal > 0 ||
               obj->item_type == ITEM_CLOTHING
            )
         )
         count++;
         if
         (
            obj->item_type == ITEM_CONTAINER &&
            CAN_WEAR(obj, ITEM_TAKE)
         )
         count = hoard_container_check( obj, count );
         pobj = obj;
      }
   }

   if ( count >= 3 )
   return TRUE;
   return FALSE;
}

/* recursive container checking for hoards -Wervdon*/
int hoard_container_check( OBJ_DATA *container, int count )
{
   OBJ_DATA *obj;

   for ( obj = container->contains; obj; obj = obj->next_content )
   {
      count++;
      if ( (obj->item_type == ITEM_CONTAINER) && (count < 3) )
      count = hoard_container_check(obj, count);
   }

   return count;
}

void do_get( CHAR_DATA *ch, char *argument )
{
   char     arg1[MAX_INPUT_LENGTH];
   char     arg2[MAX_INPUT_LENGTH];
   char     buf [MAX_STRING_LENGTH];
   OBJ_DATA *obj;
   OBJ_DATA *obj_next;
   OBJ_DATA *container;
   bool     found;
   int      hoard_found;
   long     getAmount = 0;
   int     number = 1;

   found = FALSE;
   argument = one_argument( argument, arg1 );
   if ( is_number(arg1) )
   {
      getAmount = UMAX( atoi(arg1), 0 );
      argument = one_argument( argument, arg1 );
      if (getAmount > 0)
      {
         if ( !str_prefix( arg1, "silver" ) )
         getAmount *= -1;
         else if ( str_prefix( arg1, "gold" ) )
         {
            sprintf( argument, "%s %s", argument, arg1 );
            sprintf( arg1, "%ld", getAmount );
         }
      }
   }
   argument = one_argument( argument, arg2 );

   /* Log character's that have in_room set to NULL - Wervdon */
   if ( !ch->in_room )
   {
      if ( !ch->master )
      {
         sprintf( buf, "BUG: %s in NULL room.", ch->name );
         log_string( buf );
         char_to_room(ch, NULL);  /* Will put in a temple of some kind */
         return;
      }
      else
      {
         sprintf( buf, "BUG: %s following %s in NULL room. Master is in room: %d",
         ch->name,
         ch->master->name,
         ch->master->in_room->vnum);
         log_string( buf );
         char_to_room(ch, ch->master->in_room); /* put char in master's room */
      }
   }

   if (!str_cmp(arg2, "from"))
   argument = one_argument(argument, arg2);

   /* Get type. */
   if ( arg1[0] == '\0' )
   {
      send_to_char( "Get what?\n\r", ch );
      return;
   }

   if ( !IS_NPC(ch) )
   {
      if ( ch->pcdata->death_status == HAS_DIED )
      {
         send_to_char("You are a hovering spirit, you can't pick up anything!\n\r", ch);
         return;
      }
   }

   number = mult_argument(arg1, arg1);
   if (arg1[0] == '\0')
   {
      send_to_char( "Get what?\n\r", ch );
      return;
   }
   if
   (
      number > 1 &&
      number <= ch->level
   )
   {
      int count;
      AFFECT_DATA* shadow_orig;
      AFFECT_DATA* shadow = NULL;
      AFFECT_DATA* skull_orig;
      AFFECT_DATA* skull = NULL;
      char buf2[MAX_INPUT_LENGTH];

      shadow_orig = affect_find(ch->affected, gsn_shadowstrike);
      skull_orig = affect_find(ch->affected, gsn_skull_bash);

      if (mult_argument(arg1, arg1) != 1)
      {
         /* Someone tried to nest */
         send_to_char("You get confused trying to move it.\n\r", ch);
         return;
      }
      if
      (
         (
            shadow_orig != NULL ||
            skull_orig != NULL
         ) &&
         check_shadowstrike(ch, TRUE, TRUE)
      )
      {
         /* Only check shadowstrike/skull bash once */
         return;
      }
      if (shadow_orig != NULL)
      {
         shadow = new_affect();
         *shadow = *shadow_orig;
         affect_strip(ch, gsn_shadowstrike);
      }
      if (skull_orig != NULL)
      {
         skull = new_affect();
         *skull = *skull_orig;
         affect_strip(ch, gsn_skull_bash);
      }
      if (arg2[0] != '\0')
      {
         sprintf(buf, "'%s' '%s'", arg1, arg2);
      }
      else
      {
         sprintf(buf, "'%s'", arg1);
      }
      for (count = 0; count < number; count++)
      {
         strcpy(buf2, buf);
         do_get(ch, buf2);
      }
      if (shadow)
      {
         affect_to_char(ch, shadow);
         free_affect(shadow);
      }
      if (skull)
      {
         affect_to_char(ch, skull);
         free_affect(skull);
      }
      return;
   }
   if (number > ch->level)
   {
      send_to_char("You cannot move that many at once.\n\r", ch);
      return;
   }

   hoard_found = hoard_check( ch->in_room );  /* XUR FIX moved for gets that do nothing */

   if ( arg2[0] == '\0' )
   {
      if ( str_cmp( arg1, "all" ) && str_prefix( "all.", arg1 ) )
      {
         /* 'get obj' */
         obj = get_obj_list( ch, arg1, ch->in_room->contents );
         if ( !obj )
         {
            act( "I see no $T here.", ch, NULL, arg1, TO_CHAR );
            return;
         }

         if
         (
            trigger_desire(ch, obj) ||
            !get_obj( ch, obj, NULL, getAmount) ||
            getAmount ||
            obj->carried_by == NULL
         )
         {
            return;
         }

         if (obj->pIndexData->house)
         {
            house_shudder(obj->pIndexData->house);
         }

         if ( cant_carry(ch, obj) )
         {
            act( "$n is burnt by $p and drops it.", ch, obj, NULL, TO_ROOM );
            act( "You are burnt by $p and drop it.", ch, obj, NULL, TO_CHAR);
            obj_from_char( obj );
            obj_to_room( obj, ch->in_room );
            if ( IS_NPC(ch) && IS_AFFECTED(ch, AFF_CHARM) )
            {
               act( "$n is vaporized by the power of $p!", ch, obj, NULL, TO_ROOM );
               raw_kill( ch, ch );
               return;
            }
         }
      }
      else
      {
         /* 'get all' or 'get all.obj' */
         found = FALSE;
         for ( obj = ch->in_room->contents; obj; obj = obj_next )
         {
            obj_next = obj->next_content;
            if ( !CAN_WEAR(obj, ITEM_TAKE) ) continue;

            if
            (
               (
                  arg1[3] == '\0' ||
                  is_name(&arg1[4], obj->name)
               ) &&
               can_see_obj(ch, obj)
            )
            {
               if
               (
                  trigger_desire(ch, obj)
               )
               {
                  return;
               }
               found = TRUE;
               if (!get_obj(ch, obj, NULL, 0))
               {
                  return;
               }
               if
               (
                  !obj->valid ||
                  obj->carried_by == NULL
               )
               {
                  continue;
               }
               if (obj->pIndexData->house)
               {
                  house_shudder(obj->pIndexData->house);
               }

               if ( cant_carry(ch, obj) )
               {
                  act( "$n is burnt by $p and drops it.", ch, obj, NULL, TO_ROOM );
                  act( "You are burnt by $p and drop it.", ch, obj, NULL, TO_CHAR );
                  obj_from_char( obj );
                  obj_to_room( obj, ch->in_room );
                  if ( IS_NPC(ch) )
                  {
                     act( "$n is vaporized by the power of $p!", ch, obj, NULL, TO_ROOM );
                     raw_kill( ch, ch );
                     return;
                  }
               }
            }
         }

         if ( !found )
         {
            if ( arg1[3] == '\0' )
            send_to_char( "I see nothing here.\n\r", ch );
            else
            act( "I see no $T here.", ch, NULL, &arg1[4], TO_CHAR );
         }
      }
   }
   else
   {
      /* 'get ... container' */
      if ( !str_cmp( arg2, "all" ) || !str_prefix( "all.", arg2 ) )
      {
         send_to_char( "You can't do that.\n\r", ch );
         return;
      }

      if ( !(container = get_obj_here(ch, arg2)) )
      {
         act( "I see no $T here.", ch, NULL, arg2, TO_CHAR );
         return;
      }

      if
      (
         container->wear_loc == WEAR_INSIDE_COVER &&
         container->carried_by == ch &&
         !IS_NPC(ch) &&
         ch->race == grn_book
      )
      {
         if (ch->pcdata->current_desc == BOOK_RACE_CLOSED)
         {
            send_to_char("You are closed right now.\n\r", ch);
            return;
         }
         if (ch->pcdata->current_desc == BOOK_RACE_LOCKED)
         {
            send_to_char("You are closed and locked right now.\n\r", ch);
            return;
         }
      }
      switch ( container->item_type )
      {
         default:
         send_to_char( "That's not a container.\n\r", ch );
         return;

         case ITEM_CONTAINER:
         case ITEM_CORPSE_NPC:
         break;

         case ITEM_CORPSE_PC:
         if ( !can_loot(ch, container) )
         {
            if ( !IS_NPC(ch) )   /* Message for players without descriptions who try to loot -- Wicket */
            {
               if ( (strlen(ch->description) < 100) )
               {
                  send_to_char( "Corpse looting is not permitted without a physical description.\n", ch );
                  send_to_char( "See 'HELP DESC' for more info.\n\r", ch );
               }
               else
               if ( (strlen(ch->pcdata->psych_desc) < 2) && (ch->level >= 30) )
               {
                  send_to_char( "Corpse looting is not permitted without a psychological description.\n", ch );
                  send_to_char( "See 'HELP PSYCH' for more info.\n\r", ch );
               }
               return;
            }
            else
            if ( (IS_AFFECTED(ch, AFF_CHARM))   /* Message for players without descriptions who try to use charmies to loot -- Wicket */
            && (ch->master != NULL)
            && !IS_NPC(ch->master) )
            {
               if ( (strlen(ch->master->description) < 100) )
               {
                  send_to_char( "Corpse looting is not permitted without a physical description.\n", ch->master );
                  send_to_char( "See 'HELP DESC' for more info.\n\r", ch->master );
               }
               else
               if ( (strlen(ch->master->pcdata->psych_desc) < 2) && (ch->master->level >= 30) )
               {
                  send_to_char( "Corpse looting is not permitted without a psychological description.\n", ch->master );
                  send_to_char( "See 'HELP PSYCH' for more info.\n\r", ch->master );
               }
               return;
            }
            else
            {
               send_to_char( "You can't loot it.\n\r", ch );
               return;
            }
         }
      }

      if ( IS_SET(container->value[1], CONT_CLOSED) )
      {
         act( "The $d is closed.", ch, NULL, container->name, TO_CHAR );
         return;
      }

      if ( str_cmp(arg1, "all") && str_prefix("all.", arg1) )
      {
         /* 'get obj container' */
         obj = get_obj_list( ch, arg1, container->contains );
         if ( !obj )
         {
            act( "I see nothing like that in the $T.", ch, NULL, arg2, TO_CHAR );
            return;
         }
         if ( is_affected(ch, gsn_high_herb) && (number_percent() > 75) )
         {
            send_to_char("You have trouble finding it.\n\r", ch);
            return;
         }
         if
         (
            trigger_desire(ch, container) ||
            trigger_desire(ch, obj) ||
            !get_obj( ch, obj, container, getAmount) ||
            obj->carried_by == NULL
         )
         {
            return;
         }

         if (obj->pIndexData->house)
         {
            house_shudder(obj->pIndexData->house);
         }

         if ( cant_carry(ch, obj) )
         {
            act( "$n is burnt by $p and drops it.", ch, obj, NULL, TO_ROOM );
            act( "You are burnt by $p and drop it.", ch, obj, NULL, TO_CHAR );
            obj_from_char( obj );
            obj_to_room( obj, ch->in_room );
            if ( IS_NPC(ch) )
            {
               act( "$n is vaporized by the power of $p!", ch, obj, NULL, TO_ROOM);
               raw_kill( ch, ch );
               return;
            }
         }

      }
      else
      {
         /* 'get all container' or 'get all.obj container' */
         found = FALSE;
         for ( obj = container->contains; obj; obj = obj_next )
         {
            obj_next = obj->next_content;
            if
            (
               (
                  arg1[3] == '\0' ||
                  is_name(&arg1[4], obj->name)
               ) &&
               can_see_obj(ch, obj)
            )
            {
               found = TRUE;
               if
               (
                  trigger_desire(ch, container) ||
                  trigger_desire(ch, obj) ||
                  !get_obj(ch, obj, container, 0)
               )
               {
                  return;
               }
               if (obj->carried_by == NULL)
               {
                  continue;
               }
               if (obj->pIndexData->house)
               {
                  house_shudder(obj->pIndexData->house);
               }
               if ( cant_carry(ch, obj) )
               {
                  act( "$n is burnt by $p and drops it.", ch, obj, NULL, TO_ROOM);
                  act( "You are burnt by $p and drop it.", ch, obj, NULL, TO_CHAR);
                  obj_from_char( obj );
                  obj_to_room( obj, container->in_room );
                  if ( ch && IS_NPC(ch) )
                  {
                     act( "$n is vaporized by the power of $p!", ch, obj, NULL, TO_ROOM );
                     raw_kill( ch, ch );
                     return;
                  }
               }
            }
         }
         if ( !found )
         {
            if ( arg1[3] == '\0' )
            act( "I see nothing in the $T.", ch, NULL, arg2, TO_CHAR );
            else
            act( "I see nothing like that in the $T.", ch, NULL, arg2, TO_CHAR );
         }
      }
   }

   if ( hoard_found && !hoard_check(ch->in_room) )
   {
      if (IS_AFFECTED(ch, AFF_CHARM) && ch->master)
      {
         sprintf
         (
            log_buf,
            "(%s's) %s got a hoard at %s [%d]",
            ch->master->name,
            ch->name,
            ch->in_room ? ch->in_room->name : "<unknown>",
            ch->in_room ? ch->in_room->vnum : 0
         );
      }
      else
      {
         sprintf
         (
            log_buf,
            "%s got a hoard at %s [%d]",
            ch->name,
            ch->in_room ? ch->in_room->name : "<unknown>",
            ch->in_room ? ch->in_room->vnum : 0
         );
      }
      wiznet( log_buf, ch, NULL, WIZ_TRANSFERS, 0, get_trust(ch) );
      log_string( log_buf );
   }

   return;
}



void do_put( CHAR_DATA *ch, char *argument )
{
   char arg1         [MAX_INPUT_LENGTH];
   char arg2         [MAX_INPUT_LENGTH];
   char containerbuf [MAX_STRING_LENGTH];
   EXTRA_DESCR_DATA* ed;
   EXTRA_DESCR_DATA* ed_next;
   EXTRA_DESCR_DATA* ed_new;
   AFFECT_DATA* paf;
   OBJ_DATA* container;
   OBJ_DATA* obj;
   OBJ_DATA* obj_next;
   int number = 1;

   argument = one_argument(argument, arg1);
   argument = one_argument(argument, arg2);

   if
   (
      !str_cmp(arg2, "in") ||
      !str_cmp(arg2, "on") ||
      !str_cmp(arg2, "into")
   )
   {
      argument = one_argument(argument, arg2);
   }

   if
   (
      arg1[0] == '\0' ||
      arg2[0] == '\0'
   )
   {
      send_to_char( "Put what in what?\n\r", ch );
      return;
   }

   if
   (
      !str_cmp(arg2, "all") ||
      !str_prefix("all.", arg2)
   )
   {
      send_to_char( "You can't do that.\n\r", ch );
      return;
   }

   number = mult_argument(arg1, arg1);
   if (arg1[0] == '\0')
   {
      send_to_char("Put what in what?\n\r", ch);
      return;
   }
   if
   (
      number > 1 &&
      number <= ch->level
   )
   {
      int count;
      AFFECT_DATA* shadow_orig;
      AFFECT_DATA* shadow = NULL;
      AFFECT_DATA* skull_orig;
      AFFECT_DATA* skull = NULL;
      char buf [MAX_INPUT_LENGTH];
      char buf2[MAX_INPUT_LENGTH];

      shadow_orig = affect_find(ch->affected, gsn_shadowstrike);
      skull_orig = affect_find(ch->affected, gsn_skull_bash);

      if (mult_argument(arg1, arg1) != 1)
      {
         /* Someone tried to nest */
         send_to_char("You get confused trying to move it.\n\r", ch);
         return;
      }
      if
      (
         (
            shadow_orig != NULL ||
            skull_orig != NULL
         ) &&
         check_shadowstrike(ch, TRUE, TRUE)
      )
      {
         /* Only check shadowstrike/skull bash once */
         return;
      }
      if (shadow_orig != NULL)
      {
         shadow = new_affect();
         *shadow = *shadow_orig;
         affect_strip(ch, gsn_shadowstrike);
      }
      if (skull_orig != NULL)
      {
         skull = new_affect();
         *skull = *skull_orig;
         affect_strip(ch, gsn_skull_bash);
      }
      if (arg2[0] != '\0')
      {
         sprintf(buf, "'%s' '%s'", arg1, arg2);
      }
      else
      {
         sprintf(buf, "'%s'", arg1);
      }
      for (count = 0; count < number; count++)
      {
         strcpy(buf2, buf);
         do_put(ch, buf2);
      }
      if (shadow)
      {
         affect_to_char(ch, shadow);
         free_affect(shadow);
      }
      if (skull)
      {
         affect_to_char(ch, skull);
         free_affect(skull);
      }
      return;
   }
   if (number > ch->level)
   {
      send_to_char("You cannot move that many at once.\n\r", ch);
      return;
   }



   if
   (
      !(
         container = get_obj_here(ch, arg2)
      )
   )
   {
      act("I see no $T here.", ch, NULL, arg2, TO_CHAR);
      return;
   }

   if (container->item_type != ITEM_CONTAINER)
   {
      send_to_char("That is not a container.\n\r", ch);
      return;
   }
   if
   (
      container->wear_loc == WEAR_INSIDE_COVER &&
      container->carried_by == ch &&
      !IS_NPC(ch) &&
      ch->race == grn_book
   )
   {
      if (ch->pcdata->current_desc == BOOK_RACE_CLOSED)
      {
         send_to_char("You are closed right now.\n\r", ch);
         return;
      }
      if (ch->pcdata->current_desc == BOOK_RACE_LOCKED)
      {
         send_to_char("You are closed and locked right now.\n\r", ch);
         return;
      }
   }

   if (IS_SET(container->value[1], CONT_CLOSED))
   {
      /* XUR FIX was looking at object name */
      act("$P is closed.", ch, NULL, container, TO_CHAR);
      return;
   }

   if
   (
      str_cmp(arg1, "all") &&
      str_prefix("all.", arg1)
   )
   {
      /* 'put obj container' */
      if
      (
         !(
            obj = get_obj_carry(ch, arg1, ch)
         )
      )
      {
         send_to_char("You do not have that item.\n\r", ch);
         return;
      }

      if (obj == container)
      {
         send_to_char("You cannot fold it into itself.\n\r", ch);
         return;
      }

      if (!can_drop_obj(ch, obj))
      {
         send_to_char("You cannot let go of it.\n\r", ch);
         return;
      }

      if
      (
         container->pIndexData->vnum == 511 &&
         IS_IMMORTAL(ch)
      )
      {
         free_string(container->name);
         free_string(container->short_descr);
         free_string(container->description);
         for (ed = container->extra_descr; ed; ed = ed_next)
         {
            ed_next = ed->next;
            free_extra_descr(ed);
         }
         sprintf(containerbuf, "desire %s", obj->name);
         container->name         = str_dup(containerbuf);
         container->short_descr  = str_dup(obj->short_descr);
         container->description  = str_dup(obj->description);
         container->extra_flags  = obj->extra_flags;

         for (paf = obj->affected; paf; paf = paf->next)
         {
            affect_to_obj(container, paf);
         }

         for ( ed = obj->extra_descr; ed; ed = ed->next )
         {
            ed_new                  = new_extra_descr();
            ed_new->keyword         = str_dup( ed->keyword );
            ed_new->description     = str_dup( ed->description );
            ed_new->next            = container->extra_descr;
            container->extra_descr  = ed_new;
         }
         send_to_char("Desire assumes a new facade.\n\r", ch);
         return;
      }
      if (trigger_desire(ch, container))
      {
         return;
      }
      if
      (
         container->pIndexData->vnum == OBJ_VNUM_ALCHEMY_BOOK_CONTAINER &&
         obj->pIndexData->vnum != OBJ_VNUM_MATERIAL_LUMP
      )
      {
         act("Only alchemical materials can go in $p.", ch, container, NULL, TO_CHAR);
         return;
      }
      if
      (
         (
            (
               obj->pIndexData->limtotal > 0 &&
               obj->pIndexData->limtotal <= 50
            ) &&
            (
               obj->item_type != ITEM_WEAPON ||
               obj->value[0] != WEAPON_ARROWS
            )
         ) ||
         obj->pIndexData->vnum == OBJ_VNUM_PHYLACTERY ||
         obj->pIndexData->house
      )
      {
         send_to_char("That item is too powerful to be contained.\n\r", ch);
         return;
      }

      if
      (
         obj->item_type == ITEM_TOOL &&
         container->pIndexData->vnum != OBJ_VNUM_MARAUDER_GCLOTH
      )
      {
         send_to_char("Those tools can only be organized into a guildcloth.\n\r", ch);
         return;
      }

      if (IS_SET( container->value[1], CONT_IS_QUIVER))
      {
         if
         (
            obj->item_type != ITEM_WEAPON ||
            obj->value[0] != WEAPON_ARROWS
         )
         {
            send_to_char("Only arrows may go into quivers.\n\r", ch);
            return;
         }
         if (container->contains)
         {
            send_to_char("You can only put one set of arrows in your quiver at a time.\n\r", ch);
            return;
         }
      }
      else
      {
         if
         (
            obj->item_type == ITEM_WEAPON &&
            obj->value[0] == WEAPON_ARROWS
         )
         {
            send_to_char("Arrows may only go into quivers.\n\r", ch);
            return;
         }
      }

      if (WEIGHT_MULT(obj) != 100)
      {
         send_to_char("You have a feeling that would be a bad idea.\n\r", ch);
         return;
      }

      if
      (
         (
            get_obj_weight(obj) + get_true_weight(container) - container->weight >
            container->value[0] * 10
         ) ||
         (
            get_obj_weight(obj) >
            container->value[3] * 10
         )
      )
      {
         send_to_char("It will not fit.\n\r", ch);
         return;
      }

      obj_from_char(obj);
      obj_to_obj(obj, container);

      if (IS_SET(container->value[1], CONT_PUT_ON))
      {
         act("$n puts $p on $P.", ch, obj, container, TO_ROOM);
         act("You put $p on $P.", ch, obj, container, TO_CHAR);
      }
      else
      {
         act("$n puts $p in $P.", ch, obj, container, TO_ROOM);
         act("You put $p in $P.", ch, obj, container, TO_CHAR);
      }
   }
   else
   {
      bool objFound = FALSE;

      /* 'put all container' or 'put all.obj container' */
      for (obj = ch->carrying; obj; obj = obj_next)
      {
         obj_next = obj->next_content;

         if
         (
            (
               arg1[3] == '\0' ||
               is_name(&arg1[4], obj->name)
            ) &&
            can_see_obj(ch, obj) &&
            WEIGHT_MULT(obj) == 100 &&
            obj->wear_loc == WEAR_NONE &&
            obj != container &&
            !(
               (
                  obj->pIndexData->limtotal > 0 &&
                  obj->pIndexData->limtotal <= 50
               ) &&
               (
                  obj->item_type != ITEM_WEAPON ||
                  obj->value[0] != WEAPON_ARROWS
               )
            ) &&
            (
               (
                  IS_SET(container->value[1], CONT_IS_QUIVER) &&
                  obj->item_type == ITEM_WEAPON &&
                  obj->value[0] == WEAPON_ARROWS &&
                  container->contains == NULL
               ) ||
               (
                  !IS_SET(container->value[1], CONT_IS_QUIVER) &&
                  (
                     obj->item_type != ITEM_WEAPON ||
                     obj->value[0] != WEAPON_ARROWS
                  )
               )
            ) &&
            can_drop_obj(ch, obj) &&
            (
               get_obj_weight(obj) +
               get_true_weight(container) -
               container->weight
            ) <= container->value[0] * 10 &&
            get_obj_weight(obj) <= (container->value[3] * 10) &&
            obj->pIndexData->house == 0 &&
            obj->pIndexData->vnum != OBJ_VNUM_PHYLACTERY &&
            (
               container->pIndexData->vnum != OBJ_VNUM_ALCHEMY_BOOK_CONTAINER ||
               obj->pIndexData->vnum == OBJ_VNUM_MATERIAL_LUMP
            )
         )
         {
            objFound = TRUE;

            if ( trigger_desire(ch, container) )
            {
               return;
            }

            obj_from_char(obj);
            obj_to_obj(obj, container);

            if (IS_SET( container->value[1], CONT_PUT_ON))
            {
               act("$n puts $p on $P.", ch, obj, container, TO_ROOM);
               act("You put $p on $P.", ch, obj, container, TO_CHAR);
            }
            else
            {
               act("$n puts $p in $P.", ch, obj, container, TO_ROOM );
               act("You put $p in $P.", ch, obj, container, TO_CHAR );
            }
         }
      }
      if (!objFound)
      {
         act
         (
            "You have nothing you can put into $P.",
            ch,
            obj,
            container,
            TO_CHAR
         );
      }
   }

   return;
}

/* duh */

void do_drop( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   OBJ_DATA *obj;
   OBJ_DATA *obj_next;
   bool found;
   CHAR_DATA *vch, *tch;
   int carrynum;
   char buf[MAX_STRING_LENGTH];
   OBJ_DATA *carriedobj;
   int hoard_found;
   int number = 1;

   hoard_found = hoard_check(ch->in_room);

   argument = one_argument( argument, arg );

   if ( arg[0] == '\0' )
   {
      send_to_char( "Drop what?\n\r", ch );
      return;
   }

   number = mult_argument(arg, arg);
   if (arg[0] == '\0')
   {
      send_to_char("Drop what?\n\r", ch);
      return;
   }
   if
   (
      number > 1 &&
      number <= ch->level
   )
   {
      int count;
      char buf2[MAX_INPUT_LENGTH];

      if (mult_argument(arg, arg) != 1)
      {
         /* Someone tried to nest */
         send_to_char("You get confused trying to move it.\n\r", ch);
         return;
      }
      if (argument[0] != '\0')
      {
         sprintf(buf, "'%s' %s", arg, argument);
      }
      else
      {
         sprintf(buf, "'%s'", arg);
      }
      for (count = 0; count < number; count++)
      {
         strcpy(buf2, buf);
         do_drop(ch, buf2);
      }
      return;
   }
   if (number > ch->level)
   {
      send_to_char("You cannot move that many at once.\n\r", ch);
      return;
   }
   if ( is_number( arg ) )
   {
      /* 'drop NNNN coins' */
      int amount, gold = 0, silver = 0;

      amount   = atoi(arg);
      argument = one_argument( argument, arg );
      if
      (
         amount <= 0 ||
         (
            str_cmp(arg, "coins") &&
            str_cmp(arg, "coin") &&
            str_cmp(arg, "gold") &&
            str_cmp(arg, "silver")
         )
      )
      {
         send_to_char( "Sorry, you can't do that.\n\r", ch );
         return;
      }

      if
      (
         !str_cmp(arg, "coins") ||
         !str_cmp(arg, "coin") ||
         !str_cmp(arg, "silver")
      )
      {
         if (ch->silver < amount)
         {
            send_to_char("You don't have that much silver.\n\r", ch);
            return;
         }

         ch->silver -= amount;
         silver = amount;
      }

      else
      {
         if (ch->gold < amount)
         {
            send_to_char("You don't have that much gold.\n\r", ch);
            return;
         }

         ch->gold -= amount;
         gold = amount;
      }

      for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
      {
         obj_next = obj->next_content;

         switch ( obj->pIndexData->vnum )
         {
            case OBJ_VNUM_SILVER_ONE:
            silver += 1;
            extract_obj(obj, FALSE);
            break;

            case OBJ_VNUM_GOLD_ONE:
            gold += 1;
            extract_obj( obj, FALSE );
            break;

            case OBJ_VNUM_SILVER_SOME:
            silver += obj->value[0];
            extract_obj(obj, FALSE);
            break;

            case OBJ_VNUM_GOLD_SOME:
            gold += obj->value[1];
            extract_obj( obj, FALSE );
            break;

            case OBJ_VNUM_COINS:
            silver += obj->value[0];
            gold += obj->value[1];
            extract_obj(obj, FALSE);
            break;
         }
      }

      obj_to_room( create_money( gold, silver ), ch->in_room );
      if (IS_AFFECTED(ch, AFF_SNEAK) && (ch->invis_level < LEVEL_HERO))
      for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
      {
         if (IS_SET(vch->act, ACT_HOLYLIGHT))
         {
            act( "$n drops some coins.", ch, NULL, vch, TO_VICT );
         }
      }
      if (!IS_AFFECTED(ch, AFF_SNEAK))
      act( "$n drops some coins.", ch, NULL, NULL, TO_ROOM );
      send_to_char( "OK.\n\r", ch );
      return;
   }

   if ( str_cmp( arg, "all" ) && str_prefix( "all.", arg ) )
   {
      /* 'drop obj' */
      if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
      {
         send_to_char( "You do not have that item.\n\r", ch );
         return;
      }

      if ( !can_drop_obj( ch, obj ) )
      {
         send_to_char( "You can't let go of it.\n\r", ch );
         return;
      }

      obj_from_char( obj );
      obj_to_room( obj, ch->in_room );
      if (IS_AFFECTED(ch, AFF_SNEAK) && (ch->invis_level < LEVEL_HERO))
      for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
      {
         if (IS_SET(vch->act, ACT_HOLYLIGHT))
         {
            act( "$n drops $p.", ch, obj, vch, TO_VICT );
         }
      }
      if (!IS_AFFECTED(ch, AFF_SNEAK))
      act( "$n drops $p.", ch, obj, NULL, TO_ROOM );
      act( "You drop $p.", ch, obj, NULL, TO_CHAR );
      if (IS_OBJ_STAT(obj, ITEM_MELT_DROP))
      {
         act("$p dissolves into smoke.", ch, obj, NULL, TO_ROOM);
         act("$p dissolves into smoke.", ch, obj, NULL, TO_CHAR);
         extract_obj(obj, FALSE);
      }

      for ( tch = ch->in_room->people; tch != NULL; tch = tch->next_in_room )
      {
         if
         (
            IS_NPC(tch) &&
            tch->spyfor != NULL &&
            tch->spyfor != ch
         )
         {
            sprintf
            (
               buf,
               "Your lookout informs you that %s was dropped in %s.\n\r",
               (
                  can_see_obj(tch, obj) ?
                  (
                     obj->short_descr ?
                     obj->short_descr :
                     "something"
                  ) :
                  "something"
               ),
               ch->in_room->name
            );
            send_to_char(buf, tch->spyfor);
         }
      }

   }
   else
   {
      /* 'drop all.gold' or silver or coins */
      bool moneyfound = FALSE;
      if
      (
         (arg[3] != '\0') &&
         (is_name( &arg[4], "coins") ||
         is_name( &arg[4], "silver") ||
         is_name( &arg[4], "gold"))
      )
      {
         int gold = 0, silver = 0;

         if ( !str_cmp( &arg[4], "coins")
         ||   !str_cmp( &arg[4], "silver"))
         {
            if (ch->silver > 0)
            {
               moneyfound = TRUE;
               silver = ch->silver;
               ch->silver = 0;
            }
         }
         else
         if ( !str_cmp( &arg[4], "gold") )
         {
            if (ch->gold > 0)
            {
               moneyfound = TRUE;
               gold = ch->gold;
               ch->gold = 0;
            }
         }

         if (moneyfound == TRUE)
         {
            for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
            {
               obj_next = obj->next_content;

               switch ( obj->pIndexData->vnum )
               {
                  case OBJ_VNUM_SILVER_ONE:
                  silver += 1;
                  extract_obj(obj, FALSE);
                  break;

                  case OBJ_VNUM_GOLD_ONE:
                  gold += 1;
                  extract_obj( obj, FALSE );
                  break;

                  case OBJ_VNUM_SILVER_SOME:
                  silver += obj->value[0];
                  extract_obj(obj, FALSE);
                  break;

                  case OBJ_VNUM_GOLD_SOME:
                  gold += obj->value[1];
                  extract_obj( obj, FALSE );
                  break;

                  case OBJ_VNUM_COINS:
                  silver += obj->value[0];
                  gold += obj->value[1];
                  extract_obj(obj, FALSE);
                  break;
               }
            }

            obj_to_room( create_money( gold, silver ), ch->in_room );
            if (IS_AFFECTED(ch, AFF_SNEAK) && (ch->invis_level < LEVEL_HERO))
            for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
            if (IS_SET(vch->act, ACT_HOLYLIGHT))
            act( "$n drops some coins.", ch, NULL, vch, TO_VICT );
            if (!IS_AFFECTED(ch, AFF_SNEAK))
            act( "$n drops some coins.", ch, NULL, NULL, TO_ROOM );
            send_to_char( "You drop all that money.\n\r", ch );
         }
      }
      {
         /* 'drop all' or 'drop all.obj' */
         found = FALSE;
         for ( obj = ch->carrying; obj != NULL; obj = obj_next )
         {
            obj_next = obj->next_content;
            if ( ( arg[3] == '\0' || is_name( &arg[4], obj->name ) )
            &&   can_see_obj( ch, obj )
            &&   obj->wear_loc == WEAR_NONE
            &&   can_drop_obj( ch, obj ) )
            {
               found = TRUE;
               obj_from_char( obj );
               obj_to_room( obj, ch->in_room );
               if (IS_AFFECTED(ch, AFF_SNEAK) && (ch->invis_level < LEVEL_HERO))
               for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
               {
                  if (IS_SET(vch->act, ACT_HOLYLIGHT))
                  {
                     act( "$n drops $p.", ch, obj, vch, TO_VICT );
                  }
               }
               if (!IS_AFFECTED(ch, AFF_SNEAK))
               act( "$n drops $p.", ch, obj, NULL, TO_ROOM );
               act( "You drop $p.", ch, obj, NULL, TO_CHAR );
               if (IS_OBJ_STAT(obj, ITEM_MELT_DROP))
               {
                  act("$p dissolves into smoke.", ch, obj, NULL, TO_ROOM);
                  act("$p dissolves into smoke.", ch, obj, NULL, TO_CHAR);
                  extract_obj(obj, FALSE);
               }
               for ( tch = ch->in_room->people; tch != NULL; tch = tch->next_in_room )
               {
                  if
                  (
                     IS_NPC(tch) &&
                     tch->spyfor != NULL &&
                     ch != tch
                  )
                  {
                     sprintf
                     (
                        buf,
                        "Your lookout informs you that %s was dropped in %s.\n\r",
                        (
                           can_see_obj(tch, obj) ?
                           (
                              obj->short_descr ?
                              obj->short_descr :
                              "something"
                           ) :
                           "something"
                        ),
                        ch->in_room->name
                     );
                     send_to_char(buf, tch->spyfor);
                  }
               }
            }
         }

         if ( !found && (moneyfound == FALSE) )
         {
            if ( arg[3] == '\0' )
            act( "You are not carrying anything.",
            ch, NULL, arg, TO_CHAR );
            else
            act( "You are not carrying any $T.",
            ch, NULL, &arg[4], TO_CHAR );
         }
         else
         {
            /*
               Log instances of drop all and send them to wiznet that
               leave char near naked -Werv
            */

            if ( arg[3] == '\0' && !IS_NPC(ch))
            {
               carrynum = 0;
               for ( carriedobj = ch->carrying; carriedobj !=NULL; carriedobj = carriedobj->next_content )
               if (carriedobj->wear_loc != WEAR_NONE ) carrynum++;
               if (carrynum <= 5)
               {
                  sprintf(buf, "%s dropped most of $S possessions at %s [room %d]", ch->name, ch->in_room->name, ch->in_room->vnum);
                  wiznet(buf, ch, NULL, WIZ_TRANSFERS, 0, get_trust(ch));
                  sprintf(log_buf, "%s", buf);
                  log_string(log_buf);
               }
            }
         }
      }
   }

   if (hoard_found == FALSE && hoard_check(ch->in_room))
   {
      if (IS_AFFECTED(ch, AFF_CHARM) && ch->master)
      {
         sprintf
         (
            log_buf,
            "(%s's) %s has created a hoard at %s [%d]",
            ch->master->name,
            ch->name,
            ch->in_room ? ch->in_room->name : "<unknown>",
            ch->in_room ? ch->in_room->vnum : 0
         );
      }
      else
      {
         sprintf
         (
            log_buf,
            "%s has created a hoard at %s [%d]",
            ch->name,
            ch->in_room ? ch->in_room->name : "<unknown>",
            ch->in_room ? ch->in_room->vnum : 0
         );
      }
      wiznet(log_buf, ch, NULL, WIZ_TRANSFERS, 0, get_trust(ch));
      log_string(log_buf);
   }

   return;
}

void do_give( CHAR_DATA *ch, char *argument )
{
   char arg1 [MAX_INPUT_LENGTH];
   char arg2 [MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA *victim;
   OBJ_DATA  *obj;
   sh_int vnum;
   MESSAGE message;
   char message_string[MAX_STRING_LENGTH];

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );

   if ( arg1[0] == '\0' || arg2[0] == '\0' )
   {
      send_to_char( "Give what to whom?\n\r", ch );
      return;
   }

   if ( is_number( arg1 ) )
   {
      /* 'give NNNN coins victim' */
      int amount;
      bool silver;

      amount   = atoi(arg1);
      if ( amount <= 0
      || ( str_cmp( arg2, "coins" ) && str_cmp( arg2, "coin" ) &&
      str_cmp( arg2, "gold"  ) && str_cmp( arg2, "silver")) )
      {
         send_to_char( "Sorry, you can't do that.\n\r", ch );
         return;
      }

      silver = str_cmp(arg2, "gold");

      argument = one_argument( argument, arg2 );
      if ( arg2[0] == '\0' )
      {
         send_to_char( "Give what to whom?\n\r", ch );
         return;
      }

      if ( ( victim = get_char_room( ch, arg2 ) ) == NULL )
      {
         send_to_char( "They aren't here.\n\r", ch );
         return;
      }

      if (victim == ch)
      {
         send_to_char("Uh, you already have it.\n\r", ch);
         return;
      }

      if ( (!silver && ch->gold < amount) || (silver && ch->silver < amount) )
      {
         send_to_char( "You haven't got that much.\n\r", ch );
         return;
      }

      if (silver)
      {
         ch->silver                -= amount;
         victim->silver         += amount;
      }
      else
      {
         ch->gold                -= amount;
         victim->gold        += amount;
      }

      sprintf(buf, "$n gives you %d %s.", amount, silver ? "silver" : "gold");
      act( buf, ch, NULL, victim, TO_VICT    );
      act( "$n gives $N some coins.",  ch, NULL, victim, TO_NOTVICT );
      sprintf(buf, "You give $N %d %s.", amount, silver ? "silver" : "gold");
      act( buf, ch, NULL, victim, TO_CHAR    );
      /*
         Bribe trigger
      */
      if ( IS_NPC(victim) && HAS_TRIGGER( victim, TRIG_BRIBE ) )
      mp_bribe_trigger( victim, ch, silver ? amount : amount * 100 );

      if (IS_NPC(victim) && IS_SET(victim->act, ACT_IS_CHANGER))
      {
         int change;

         change = (silver ? 95 * amount / 100 / 100
         : 95 * amount);


         if (!silver && change > victim->silver)
         victim->silver += change;

         if (silver && change > victim->gold)
         victim->gold += change;

         if (change < 1 && can_see(victim, ch))
         {
            act(
            "$n tells you 'I'm sorry, you did not give me enough to change.'"
            , victim, NULL, ch, TO_VICT);
            ch->reply = victim;
            sprintf(buf, "%d %s \"%s\"",
            amount, silver ? "silver" : "gold", get_name(ch, victim));
            do_give(victim, buf);
         }
         else if (can_see(victim, ch))
         {
            sprintf(buf, "%d %s \"%s\"",
            change, silver ? "gold" : "silver", get_name(ch, victim));
            do_give(victim, buf);
            if (silver)
            {
               sprintf(buf, "%d silver \"%s\"",
               (95 * amount / 100 - change * 100), get_name(ch, victim));
               do_give(victim, buf);
            }
            act("$n tells you 'Thank you, come again.'",
            victim, NULL, ch, TO_VICT);
            ch->reply = victim;
            victim->gold = 0;
            victim->silver = 0;
         }
      }
      return;
   }

   if ( ( obj = get_obj_carry( ch, arg1, ch ) ) == NULL )
   {
      send_to_char( "You do not have that item.\n\r", ch );
      return;
   }



   if ( obj->wear_loc != WEAR_NONE )
   {
      send_to_char( "You must remove it first.\n\r", ch );
      return;
   }

   if ( ( victim = get_char_room( ch, arg2 ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if (victim == ch)
   {
      send_to_char("Uh, you already have it.\n\r", ch);
      return;
   }

   if (IS_NPC(victim) && victim->pIndexData->pShop != NULL)
   {
      act("$N tells you 'Sorry, you'll have to sell that.'",
      ch, NULL, victim, TO_CHAR);
      ch->reply = victim;
      return;
   }

   if ((obj->pIndexData->vnum == OBJ_VNUM_PHYLACTERY)
   && (!str_cmp(obj->owner, ch->name)))
   {
      send_to_char( "You cannot bring yourself to trust your phylactery to another.\n\r", ch);
      return;
   }
   if ( !can_drop_obj( ch, obj ) )
   {
      send_to_char( "You can't let go of it.\n\r", ch );
      return;
   }

   if ( victim->carry_number + get_obj_number( obj ) > can_carry_n( victim ) )
   {
      act( "$N has $S hands full.", ch, NULL, victim, TO_CHAR );
      return;
   }

   if (get_carry_weight(victim) + get_obj_weight(obj) > can_carry_w( victim ) )
   {
      act( "$N can't carry that much weight.", ch, NULL, victim, TO_CHAR );
      return;
   }

   if ( !can_see_obj( victim, obj ) )
   {
      act( "$N can't see it.", ch, NULL, victim, TO_CHAR );
      return;
   }

   obj_from_char( obj );
   obj_to_char( obj, victim );
   MOBtrigger = FALSE;
   act( "$n gives $p to $N.", ch, obj, victim, TO_NOTVICT );
   act( "$n gives you $p.",   ch, obj, victim, TO_VICT    );
   act( "You give $p to $N.", ch, obj, victim, TO_CHAR    );
   if
   (
      obj->item_type == ITEM_FOOD &&
      IS_NPC(victim) &&
      (
         IS_SET(victim->act2, ACT_STRAY) ||
         IS_SET(victim->act2, ACT_NICE_STRAY)
      ) &&
      ch != victim
   )
   {
      stray_eat(victim, obj, ch);
   }
   if
   (
      IS_NPC(victim) &&
      obj->item_type == ITEM_DRINK_CON &&
      obj->value[1] > 0 &&
      (
         IS_SET(victim->act2, ACT_STRAY) ||
         IS_SET(victim->act2, ACT_NICE_STRAY)
      ) &&
      ch != victim
   )
   {
      stray_drink(victim, obj, ch);
   }
   if (!IS_NPC(ch) && !IS_NPC(victim) && (victim->desc == NULL))
   {
      sprintf(buf, "[nolink] %s (%d) gave %s (%d) to %s (%d)",
      ch->name, get_trust(ch), obj->short_descr, obj->level,
      victim->name, get_trust(victim));
      wiznet(buf, ch, NULL, WIZ_TRANSFERS, 0, get_trust(ch));
      sprintf(log_buf, "%s", buf);
      log_string(log_buf);
   } else
   if (!IS_NPC(ch) && !IS_NPC(victim))
   {
      if (!strcmp(ch->host, victim->host))
      {
         sprintf(buf, "[site] %s (%d) gave %s (%d) to %s (%d)",
         ch->name, get_trust(ch), obj->short_descr, obj->level,
         victim->name, get_trust(victim));
         wiznet(buf, ch, NULL, WIZ_TRANSFERS, 0, get_trust(ch));
         sprintf(log_buf, "%s", buf);
         log_string(log_buf);
      } else if ((get_trust(victim) <= 10) && (get_trust(ch) > 10)
      && (get_trust(ch) > get_trust(victim) + 8)
      && (obj->level > get_trust(victim) + 8))
      {
         sprintf(buf, "[newbie] %s (%d) gave %s (%d) to %s (%d)",
         ch->name, get_trust(ch), obj->short_descr, obj->level,
         victim->name, get_trust(victim));
         wiznet(buf, ch, NULL, WIZ_TRANSFERS, 0, get_trust(ch));
         sprintf(log_buf, "%s", buf);
         log_string(log_buf);
      }
   }
   if (obj->pIndexData->house)
   {
      house_shudder(obj->pIndexData->house);
      if
      (
         IS_NPC(ch) &&
         (
            (
               vnum = ch->pIndexData->vnum
            ) == house_table[obj->pIndexData->house].vnum_statue
         ) &&
         !IS_NPC(victim)
      )
      {
         message.message_type = MM_HOUSE_ITEM_TAKEN;
         message.message_data = 0;
         message.message_flags = MESSAGE_UNIQUE | MESSAGE_REPLACE | MESSAGE_POLL_TICK;
         message.message_string = message_string;
         strcpy( message_string, get_name(victim, ch) );
         message_to_char( ch, &message );
      }
   }

   if (IS_NPC(victim) && !IS_NPC(ch) && !((IS_AFFECTED(victim, AFF_CHARM)) &&
   (victim->master == ch))){
      sprintf(log_buf, "[MOB GIVE] %s gave %s to %s", ch->name, obj->short_descr, victim->short_descr);
      wiznet(log_buf, ch, NULL, WIZ_TRANSFERS, 0, get_trust(ch));
      log_string(log_buf);
   }

   if (cant_carry(victim, obj))
   {
      act("$n is burnt by $p and drops it.", victim, obj, NULL, TO_ROOM);
      act("You are burnt by $p and drop it.", victim, obj, NULL, TO_CHAR);
      obj_from_char(obj);
      obj_to_room(obj, victim->in_room);
   }

   MOBtrigger = TRUE;

   /*
      Give trigger
   */
   if
   (
      !obj->valid ||
      obj->carried_by != victim
   )
   {
      return;
   }
   obj_give_hydra_head(ch, victim, obj);
   if
   (
      !obj->valid ||
      obj->carried_by != victim
   )
   {
      return;
   }
   house_key_trigger(ch, victim, obj);
   if
   (
      !obj->valid ||
      obj->carried_by != victim
   )
   {
      return;
   }
   crusader_druid_prog(ch, victim, obj);
   if
   (
      !obj->valid ||
      obj->carried_by != victim
   )
   {
      return;
   }
   if
   (
      IS_NPC(victim) &&
      HAS_TRIGGER(victim, TRIG_GIVE)
   )
   {
      mp_give_trigger(victim, ch, obj);
   }
   return;
}

void do_empower( CHAR_DATA *ch, char *argument )
{
   OBJ_DATA *ring;
   AFFECT_DATA af;
   AFFECT_DATA paf;
   int chance;

   send_to_char("Huh?\n\r", ch);
   return;

   chance = IS_NPC(ch) ? 100 : ch->pcdata->learned[gsn_empower];

   if ( (chance <= 20) || !has_skill(ch, gsn_empower) )
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }

   if (house_down(ch, HOUSE_ANCIENT))
   return;

   if (ch->mana < 80 )
   {
      send_to_char("You don't have enough mana.\n\r", ch);
      return;
   }
   if (is_affected(ch, gsn_empower) )
   {
      send_to_char("You can't reforge any more negative energy right now.\n\r", ch);
      return;
   }
   ring = create_object(get_obj_index(OBJ_VNUM_RING_EMPOWER), ch->level);

   ring->timer = ch->level / 2;
   paf.where = TO_OBJECT;
   paf.type = gsn_empower;
   paf.level = ch->level;
   paf.duration = -1;
   paf.location = APPLY_DAMROLL;
   paf.modifier = (ch->level / 8) + 2;
   paf.bitvector = 0;
   affect_to_obj(ring, &paf);

   paf.location = APPLY_HITROLL;
   affect_to_obj(ring, &paf);

   send_to_char("You concentrate and use your powers to forge the negative energy around you.\n\r", ch);
   act("A halo of searing darkness burns into your hand and a ring appears.", ch, ring, NULL, TO_CHAR);
   obj_to_char(ring, ch);
   act("Something in $n's hand glows briefly before fading.", ch, NULL, NULL, TO_ROOM);
   ch->mana -= 80;

   af.where = TO_AFFECTS;
   af.type = gsn_empower;
   af.duration = ch->level / 2;
   af.modifier = 0;
   af.location = 0;
   af.bitvector = 0;
   af.level = ch->level;
   affect_to_char(ch, &af);

   return;
}

void do_tanning(CHAR_DATA *ch, char *argument)
{
   OBJ_DATA *obj;
   char arg1[MAX_INPUT_LENGTH];

   if ( (get_skill(ch, gsn_tanning) <= 0) ||
   !has_skill(ch, gsn_tanning) )
   {
      send_to_char("But you don't know how to tan hides.\n\r", ch);
      return;
   }

   argument = one_argument(argument, arg1);
   if ( ( obj = get_obj_carry( ch, arg1, ch ) ) == NULL )
   {
      send_to_char("You don't have that.\n\r", ch);
      return;
   }

   if (obj->pIndexData->vnum != OBJ_VNUM_SACK || obj->timer == 0)
   {
      send_to_char("You can't tan that.\n\r", ch);
      return;
   }

   if (!IS_NPC(ch) && (number_percent() > ch->pcdata->learned[gsn_tanning]))
   {
      act("You fail to tan $p and destroy it.", ch, obj, NULL, TO_CHAR);
      act("$n tries to tan $p to repair it but fails.", ch, obj, NULL, TO_ROOM);
      extract_obj(obj, FALSE);
      check_improve(ch, gsn_tanning, FALSE, 1);
      return;
   }
   act("You tan $p and it is repaired.", ch, obj, NULL, TO_CHAR);
   act("$n tans $p and it looks like new.", ch, obj, NULL, TO_ROOM);
   check_improve(ch, gsn_tanning, TRUE, 1);

   obj->timer = obj->level * 8;
   return;
}

void do_skin( CHAR_DATA *ch, char *argument)
{
   OBJ_DATA *corpse;
   OBJ_DATA *sack;
   OBJ_DATA *obj;
   OBJ_DATA *obj_next;
   AFFECT_DATA af;
   char *name;
   /*    char *last_name; */
   char arg1[MAX_STRING_LENGTH];
   char buf1[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];
   int type = 0;
   int chance;

   corpse = NULL;
   if
   (
      (
         chance = get_skill(ch, gsn_skin)
      ) <= 0 ||
      !has_skill(ch, gsn_skin)
   )
   {
      send_to_char("But you don't know how to skin a corpse.\n\r", ch);
      return;
   }
   if (ch->mana < 25)
   {
      send_to_char("You don't have the mana.\n\r", ch);
      return;
   }

   argument = one_argument(argument, arg1);
   if ( ( corpse = get_obj_carry( ch, arg1, ch ) ) == NULL )
   {
      send_to_char("You don't have that.\n\r", ch);
      return;
   }

   if (corpse->item_type != ITEM_CORPSE_NPC)
   {
      send_to_char("You can't skin that.\n\r", ch);
      return;
   }

   type = -1;
   if (argument[0] != '\0')
   {
      if (!str_prefix(argument, "sack"))
      {
         type = 0;
      }
      if (!str_prefix(argument, "pelt"))
      {
         type = 1;
      }
      if (!str_prefix(argument, "cap"))
      {
         type = 2;
      }
      if (!str_prefix(argument, "leggings"))
      {
         type = 3;
      }
      if (!str_prefix(argument, "sleeves"))
      {
         type = 4;
      }
      if (!str_prefix(argument, "armor"))
      {
         type = 5;
      }
      if (!str_prefix(argument, "horns"))
      {
         type = 6;
      }
      if (!str_prefix(argument, "fourlegs"))
      {
         type = 7;
      }
   }

   if (type == -1)
   {
      send_to_char("Skin into what?  (sack, pelt, cap, sleeves, leggings, armor, horns, fourlegs)\n\r", ch);
      return;
   }
   if (is_affected(ch, gsn_skin) && type != 1)
   {
      send_to_char("You do not feel up to fashioning a new skin yet.\n\r", ch);
      return;
   }
   if
   (
      !IS_NPC(ch) &&
      number_percent() > chance
   )
   {
      act("You fail and destroy $p.", ch, corpse, NULL, TO_CHAR);
      act("$n tries to skin a corpse but fails and destroys it.", ch, NULL, NULL, TO_ROOM);
      extract_obj(corpse, FALSE);
      ch->mana -= 25;
      check_improve(ch, gsn_skin, FALSE, 4);
      return;
   }

   name = corpse->short_descr;
   if (!str_prefix("The headless corpse of ", name))
   name = name + 23;
   else
   name = name + 14;  /* Skip: "The corpse of " */

   for (obj = corpse->contains; obj != NULL; obj = obj_next)
   {
      obj_next = obj->next_content;
      obj_from_obj(obj);
      obj_to_room(obj, ch->in_room);
   }
   sack = create_object(get_obj_index(OBJ_VNUM_SACK), 1);
   sack->level = corpse->level;
   free_string(sack->short_descr);
   free_string(sack->description);
   free_string(sack->name);
   af.where = TO_OBJECT;
   af.type = gsn_skin;
   af.duration = -1;
   af.bitvector = 0;
   af.level = ch->level;

   switch (type)
   {
      case 0:
      act("$n brutally skins $p and fashions $mself a bloody sack.", ch, corpse, NULL, TO_ROOM);
      act("You skin $p and make a new rangers sack!", ch, corpse, NULL, TO_CHAR);
      sprintf(buf1, "a sack made from the skin of %s", name);
      sprintf(buf2, "a bloody sack fashioned from the torn skin of %s is here.", name);
      sack->short_descr = str_dup(buf1);
      sack->description = str_dup(buf2);
      sack->value[3] = corpse->level;
      sack->value[0] = corpse->level*10;
      sack->value[4] = 100-corpse->level/5;
      sack->name = str_dup("sack");
      break;
      case 1:
      act("$n brutally skins $p and fashions $mself a pelt.", ch, corpse, NULL, TO_ROOM);
      act("You skin $p and make a new pelt!", ch, corpse, NULL, TO_CHAR);
      sprintf(buf1, "a pelt from the skin of %s", name);
      sprintf(buf2, "a pelt from the skin of %s is here.", name);
      sack->short_descr = str_dup(buf1);
      sack->description = str_dup(buf2);
      sack->name = str_dup("pelt");
      sack->item_type = ITEM_TREASURE;
      break;
      case 2:
      act("$n brutally skins $p and fashions $mself a cap.", ch, corpse, NULL, TO_ROOM);
      act("You skin $p and make a new cap!", ch, corpse, NULL, TO_CHAR);
      sprintf(buf1, "a leather cap made from the skin of %s", name);
      sprintf(buf2, "a leather cap made from the skin of %s is here.", name);
      sack->short_descr = str_dup(buf1);
      sack->description = str_dup(buf2);
      sack->name = str_dup("cap");
      sack->item_type = ITEM_ARMOR;
      sack->wear_flags = 0;
      SET_BIT(sack->wear_flags, ITEM_TAKE);
      SET_BIT(sack->wear_flags, ITEM_WEAR_HEAD);
      break;
      case 3:
      act("$n brutally skins $p and fashions $mself some leggings.", ch, corpse, NULL, TO_ROOM);
      act("You skin $p and make some leggings!", ch, corpse, NULL, TO_CHAR);
      sprintf(buf1, "some leggings made from the skin of %s", name);
      sprintf(buf2, "Some leggings made from the skin of %s is here.", name);
      sack->short_descr = str_dup(buf1);
      sack->description = str_dup(buf2);
      sack->name = str_dup("leggings");
      sack->item_type = ITEM_ARMOR;
      sack->wear_flags = 0;
      SET_BIT(sack->wear_flags, ITEM_TAKE);
      SET_BIT(sack->wear_flags, ITEM_WEAR_LEGS);
      break;
      case 4:
      act("$n brutally skins $p and fashions $mself some sleeves.", ch, corpse, NULL, TO_ROOM);
      act("You skin $p and make some sleeves!", ch, corpse, NULL, TO_CHAR);
      sprintf(buf1, "some sleeves made from the skin of %s", name);
      sprintf(buf2, "Some sleeves made from the skin of %s is here.", name);
      sack->short_descr = str_dup(buf1);
      sack->description = str_dup(buf2);
      sack->name = str_dup("sleeves");
      sack->item_type = ITEM_ARMOR;
      sack->wear_flags = 0;
      SET_BIT(sack->wear_flags, ITEM_TAKE);
      SET_BIT(sack->wear_flags, ITEM_WEAR_ARMS);
      break;
      case 5:
      act("$n brutally skins $p and fashions $mself some armor.", ch, corpse, NULL, TO_ROOM);
      act("You skin $p and make some armor!", ch, corpse, NULL, TO_CHAR);
      sprintf(buf1, "Leather Armor made from the skin of %s", name);
      sprintf(buf2, "Leather Armor made from the skin of %s is here.", name);
      sack->short_descr = str_dup(buf1);
      sack->description = str_dup(buf2);
      sack->name = str_dup("armor");
      sack->item_type = ITEM_ARMOR;
      sack->wear_flags = 0;
      SET_BIT(sack->wear_flags, ITEM_TAKE);
      SET_BIT(sack->wear_flags, ITEM_WEAR_BODY);
      break;
      case 6:
      act("$n brutally skins $p and fashions $mself horn wraps.", ch, corpse, NULL, TO_ROOM);
      act("You skin $p and make a new cap!", ch, corpse, NULL, TO_CHAR);
      sprintf(buf1, "a horn wrap made from the skin of %s", name);
      sprintf(buf2, "a horn wrap made from the skin of %s is here.", name);
      sack->short_descr = str_dup(buf1);
      sack->description = str_dup(buf2);
      sack->name = str_dup("horn wrap wraps");
      sack->item_type = ITEM_ARMOR;
      sack->wear_flags = 0;
      SET_BIT(sack->wear_flags, ITEM_TAKE);
      SET_BIT(sack->wear_flags, ITEM_WEAR_HORNS);
      break;
      case 7:
      act("$n brutally skins $p and fashions $mself two pairs of leggings.", ch, corpse, NULL, TO_ROOM);
      act("You skin $p and make some leggings!", ch, corpse, NULL, TO_CHAR);
      sprintf(buf1, "some leggings made from the skin of %s", name);
      sprintf(buf2, "Some leggings made from the skin of %s is here.", name);
      sack->short_descr = str_dup(buf1);
      sack->description = str_dup(buf2);
      sack->name = str_dup("leggings two fourlegs fourleggings four");
      sack->item_type = ITEM_ARMOR;
      sack->wear_flags = 0;
      SET_BIT(sack->wear_flags, ITEM_TAKE);
      SET_BIT(sack->wear_flags, ITEM_WEAR_FOURLEGS);
      break;
   }

   if (type >= 2)
   {
      af.modifier = 1;
      if (ch->pcdata->special == SUBCLASS_HUNTER)
      af.modifier = 2;
      af.location = APPLY_HITROLL;
      affect_to_obj(sack, &af);
      af.location = APPLY_DAMROLL;
      affect_to_obj(sack, &af);
      af.location = APPLY_HIT;
      af.modifier = sack->level/3;
      if (ch->pcdata->special == SUBCLASS_HUNTER)
      af.modifier += 10;
      affect_to_obj(sack, &af);
      sack->value[0] = sack->level/8;
      sack->value[1] = sack->level/8;
      sack->value[2] = sack->level/8;
      sack->value[3] = sack->level/12;
      if (type == 5)
      {
         sack->value[0] += 5;
         sack->value[1] += 5;
         sack->value[2] += 5;
         sack->value[3] += 2;
      }
      sack->timer = sack->level * 8;
      if (ch->pcdata->special == SUBCLASS_HUNTER)
      {
         sack->value[0] += 5;
         sack->value[1] += 5;
         sack->value[2] += 5;
         sack->value[3] += 2;
         sack->timer = sack->timer + sack->timer/2;
      }
   }

   obj_to_char(sack, ch);
   extract_obj(corpse, FALSE);
   ch->mana -= 25;
   check_improve(ch, gsn_skin, TRUE, 4);

   af.where = TO_AFFECTS;
   af.type = gsn_skin;
   af.modifier = 0;
   af.location = 0;
   af.duration = 12;
   if (ch->pcdata->special == SUBCLASS_HUNTER)
   af.duration = 6;
   af.level = ch->level;
   af.bitvector = 0;
   if (type != 1)
   affect_to_char(ch, &af);

   return;
}

void do_butcher( CHAR_DATA *ch, char *argument)
{
   OBJ_DATA *find_corpse;
   OBJ_DATA *corpse;
   OBJ_DATA *steak;
   OBJ_DATA *obj;
   OBJ_DATA *obj_next;
   int number, count;
   char *name;
   /*    char *last_name;   */
   char arg1[MAX_STRING_LENGTH];
   char buf1[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];

   corpse = NULL;
   if ( (get_skill(ch, gsn_butcher) <= 0)
   || /*(ch->level < skill_table[gsn_butcher].skill_level[ch->class]) */
   !has_skill(ch, gsn_butcher))
   {
      send_to_char("But you don't know how to butcher.\n\r", ch);
      return;
   }

   one_argument(argument, arg1);
   number = number_argument(argument, arg1);
   count = 0;
   for (find_corpse = ch->in_room->contents;  find_corpse != NULL; find_corpse = find_corpse->next_content)
   {
      if ((can_see_obj(ch, find_corpse) )
      && ( arg1[0] == '\0' || is_name(arg1, find_corpse->name)) )
      {
         if (++count == number )
         corpse = find_corpse;
      }
   }

   if (corpse == NULL)
   {
      send_to_char("You don't have that.\n\r", ch);
      return;
   }
   if ((corpse->item_type != ITEM_CORPSE_NPC) && (corpse->item_type != ITEM_CORPSE_PC))
   {
      send_to_char("You can only butcher a corpse.\n\r", ch);
      return;
   }

   if (!IS_NPC(ch) && (number_percent() > ch->pcdata->learned[gsn_butcher]))
   {
      act("You fail and destroy $p.", ch, corpse, NULL, TO_CHAR);
      act("$n tries to butcher a corpse but fails and destroys it.", ch, NULL, NULL, TO_ROOM);
      extract_obj(corpse, FALSE);
      check_improve(ch, gsn_butcher, FALSE, 4);
      return;
   }

   act("$n butchers $p.", ch, corpse, NULL, TO_ROOM);
   act("You butcher $p.", ch, corpse, NULL, TO_CHAR);
   check_improve(ch, gsn_butcher, TRUE, 4);
   name = corpse->short_descr;
   for (obj = corpse->contains; obj != NULL; obj = obj_next)
   {
      obj_next = obj->next_content;
      obj_from_obj(obj);
      obj_to_room(obj, ch->in_room);
   }

   steak = create_object(get_obj_index(OBJ_VNUM_STEAK), 1);
   steak->level = corpse->level;


   if (!str_prefix("The headless corpse of ", name))
   name = name + 23;
   else
   name = name + 14;  /* Skip: "The corpse of " */

   sprintf(buf1, "a steak of %s", name);
   sprintf(buf2, "a steak of %s is here.", name);
   free_string(steak->short_descr);
   free_string(steak->description);
   steak->short_descr = str_dup(buf1);
   steak->description = str_dup(buf2);

   obj_to_room(steak, ch->in_room);
   extract_obj(corpse, FALSE);
   return;
}


/* for sharpening weapons */
void do_sharpen(CHAR_DATA *ch, char *argument)
{
   OBJ_DATA *obj;
   AFFECT_DATA af;
   int percent, skill;

   if ((skill = get_skill(ch, gsn_sharpen)) < 1 ||!has_skill(ch, gsn_sharpen))
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }

   /* find out what */
   if (argument == '\0')
   {
      send_to_char("Sharpen which weapon?\n\r", ch);
      return;
   }

   obj =  get_obj_list(ch, argument, ch->carrying);

   if (obj== NULL)
   {
      send_to_char("You don't have that weapon.\n\r", ch);
      return;
   }
   if (obj->item_type != ITEM_WEAPON)
   {
      send_to_char("That isn't a weapon.\n\r", ch);
      return;
   }
   if (IS_WEAPON_STAT(obj, WEAPON_SHARP))
   {
      act("$p is already as sharp as it can be.", ch, obj, NULL, TO_CHAR);
      return;
   }
   if (obj->value[3] < 0
   || !is_metal(obj)
   || ( attack_table[obj->value[3]].damage != DAM_SLASH &&
   attack_table[obj->value[3]].damage != DAM_PIERCE))
   {
      send_to_char("You can't sharpen this weapon.\n\r", ch);
      return;
   }
   percent = number_percent();
   if (percent < skill)
   {
      af.where     = TO_WEAPON;
      af.type      = gsn_sharpen;
      af.level     = ch->level * percent / 100;
      af.duration  = ch->level/4;
      af.location  = 0;
      af.modifier  = 0;
      af.bitvector = WEAPON_SHARP;
      affect_to_obj(obj, &af);
      act("$n sharpens $p's edges.", ch, obj, NULL, TO_ROOM);
      act("You sharpen $p's edges.", ch, obj, NULL, TO_CHAR);
      check_improve(ch, gsn_sharpen, TRUE, 3);
      WAIT_STATE(ch, skill_table[gsn_sharpen].beats);
      return;
   }
   else
   {
      act("You fail to sharpen $p's edges.", ch, obj, NULL, TO_CHAR);
      check_improve(ch, gsn_sharpen, FALSE, 3);
      WAIT_STATE(ch, skill_table[gsn_sharpen].beats);
      return;
   }
   return;
}


/* for poisoning weapons and food/drink */
void do_envenom(CHAR_DATA *ch, char *argument)
{
   OBJ_DATA *obj = NULL;
   AFFECT_DATA af;
   int percent, skill;

   /* find out what */
   if (argument[0] == '\0')
   {
      obj = get_eq_char(ch, WEAR_WIELD);
      if (obj == NULL)
      {
         send_to_char("Envenom what item?\n\r", ch);
         return;
      }
   }

   if (obj == NULL)
   obj =  get_obj_list(ch, argument, ch->carrying);

   if (obj== NULL)
   {
      send_to_char("You don't have that item.\n\r", ch);
      return;
   }

   if ((skill = get_skill(ch, gsn_envenom)) < 1)
   {
      send_to_char("Are you crazy? You'd poison yourself!\n\r", ch);
      return;
   }

   if (obj->item_type == ITEM_FOOD || obj->item_type == ITEM_DRINK_CON)
   {
      if (IS_OBJ_STAT(obj, ITEM_BLESS) || IS_OBJ_STAT(obj, ITEM_BURN_PROOF))
      {
         act("You fail to poison $p.", ch, obj, NULL, TO_CHAR);
         return;
      }

      if (number_percent() < skill)  /* success! */
      {
         act("$n treats $p with deadly poison.", ch, obj, NULL, TO_ROOM);
         act("You treat $p with deadly poison.", ch, obj, NULL, TO_CHAR);
         if (!obj->value[3])
         {
            obj->value[3] = 1;
            check_improve(ch, gsn_envenom, TRUE, 4);
         }
         WAIT_STATE(ch, skill_table[gsn_envenom].beats);
         return;
      }

      act("You fail to poison $p.", ch, obj, NULL, TO_CHAR);
      if (!obj->value[3])
      check_improve(ch, gsn_envenom, FALSE, 4);
      WAIT_STATE(ch, skill_table[gsn_envenom].beats);
      return;
   }

   if (obj->item_type == ITEM_WEAPON)
   {
      if (IS_OBJ_STAT(obj, ITEM_BLESS))
      {
         act("You can't seem to envenom $p.", ch, obj, NULL, TO_CHAR);
         return;
      }

      if (obj->value[3] < 0
      ||  attack_table[obj->value[3]].damage == DAM_BASH)
      {
         send_to_char("You can only envenom edged weapons.\n\r", ch);
         return;
      }

      if (IS_WEAPON_STAT(obj, WEAPON_POISON))
      {
         act("$p is already envenomed.", ch, obj, NULL, TO_CHAR);
         return;
      }

      percent = number_percent();
      if (percent < skill)
      {

         af.where     = TO_WEAPON;
         af.type      = gsn_poison;
         af.level     = ch->level * percent / 100;
         af.duration  = ch->level/2 * percent / 100;
         af.location  = 0;
         af.modifier  = 0;
         af.bitvector = WEAPON_POISON;
         affect_to_obj(obj, &af);

         act("$n coats $p with deadly venom.", ch, obj, NULL, TO_ROOM);
         act("You coat $p with venom.", ch, obj, NULL, TO_CHAR);
         check_improve(ch, gsn_envenom, TRUE, 3);
         WAIT_STATE(ch, skill_table[gsn_envenom].beats);
         return;
      }
      else
      {
         act("You fail to envenom $p.", ch, obj, NULL, TO_CHAR);
         check_improve(ch, gsn_envenom, FALSE, 3);
         WAIT_STATE(ch, skill_table[gsn_envenom].beats);
         return;
      }
   }

   act("You can't poison $p.", ch, obj, NULL, TO_CHAR);
   return;
}

void do_fill( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   OBJ_DATA *obj;
   OBJ_DATA *fountain;
   bool found;

   one_argument( argument, arg );

   if ( arg[0] == '\0' )
   {
      send_to_char( "Fill what?\n\r", ch );
      return;
   }

   if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
   {
      send_to_char( "You do not have that item.\n\r", ch );
      return;
   }

   found = FALSE;
   for ( fountain = ch->in_room->contents; fountain != NULL;
   fountain = fountain->next_content )
   {
      if ( fountain->item_type == ITEM_FOUNTAIN )
      {
         found = TRUE;
         break;
      }
   }

   if ( !found )
   {
      send_to_char( "There is no fountain here!\n\r", ch );
      return;
   }

   if ( obj->item_type != ITEM_DRINK_CON )
   {
      send_to_char( "You can't fill that.\n\r", ch );
      return;
   }

   if ( obj->value[1] != 0 && obj->value[2] != fountain->value[2] )
   {
      send_to_char( "There is already another liquid in it.\n\r", ch );
      return;
   }

   if ( obj->value[1] >= obj->value[0] )
   {
      send_to_char( "Your container is full.\n\r", ch );
      return;
   }

   sprintf(buf, "You fill $p with %s from $P.",
   liq_table[fountain->value[2]].liq_name);
   act( buf, ch, obj, fountain, TO_CHAR );
   sprintf(buf, "$n fills $p with %s from $P.",
   liq_table[fountain->value[2]].liq_name);
   act(buf, ch, obj, fountain, TO_ROOM);
   obj->value[2] = fountain->value[2];
   obj->value[1] = obj->value[0];
   return;
}

void do_pour (CHAR_DATA *ch, char *argument)
{
   char arg[MAX_STRING_LENGTH], buf[MAX_STRING_LENGTH];
   OBJ_DATA *out, *in;
   CHAR_DATA *vch = NULL;
   int amount;

   argument = one_argument(argument, arg);

   if (arg[0] == '\0' || argument[0] == '\0')
   {
      send_to_char("Pour what into what?\n\r", ch);
      return;
   }


   if ((out = get_obj_carry(ch, arg, ch)) == NULL)
   {
      send_to_char("You don't have that item.\n\r", ch);
      return;
   }

   if (out->item_type != ITEM_DRINK_CON)
   {
      send_to_char("That's not a drink container.\n\r", ch);
      return;
   }

   if (!str_cmp(argument, "out"))
   {
      if (out->value[1] == 0)
      {
         send_to_char("It is already empty.\n\r", ch);
         return;
      }

      out->value[1] = 0;
      out->value[3] = 0;
      sprintf(buf, "You invert $p, spilling %s all over the ground.",
      liq_table[out->value[2]].liq_name);
      act(buf, ch, out, NULL, TO_CHAR);

      sprintf(buf, "$n inverts $p, spilling %s all over the ground.",
      liq_table[out->value[2]].liq_name);
      act(buf, ch, out, NULL, TO_ROOM);
      return;
   }

   if ((in = get_obj_here(ch, argument)) == NULL)
   {
      vch = get_char_room(ch, argument);

      if (vch == NULL)
      {
         send_to_char("Pour into what?\n\r", ch);
         return;
      }

      in = get_eq_char(vch, WEAR_HOLD);

      if (in == NULL)
      {
         send_to_char("They aren't holding anything.", ch);
         return;
      }
   }

   if (in->item_type != ITEM_DRINK_CON)
   {
      send_to_char("You can only pour into other drink containers.\n\r", ch);
      return;
   }

   if (in == out)
   {
      send_to_char("You cannot change the laws of physics!\n\r", ch);
      return;
   }

   if (in->value[1] != 0 && in->value[2] != out->value[2])
   {
      send_to_char("They don't hold the same liquid.\n\r", ch);
      return;
   }

   if (out->value[1] == 0)
   {
      act("There's nothing in $p to pour.", ch, out, NULL, TO_CHAR);
      return;
   }

   if (in->value[1] >= in->value[0])
   {
      act("$p is already filled to the top.", ch, in, NULL, TO_CHAR);
      return;
   }

   if ((in->pIndexData->vnum == 3827) &&
   (liq_lookup("blood") == out->value[2]))
   {
      act("That blood does not have the right flavor of life.", ch, in, NULL, TO_CHAR);
      return;
   }

   if ((in->pIndexData->vnum == 3827) &&
   (in->pIndexData->value[2] != out->value[2])) /* Golden offering bowl */
   {
      act("You sense it would be a very BAD idea to offer impurities from $p.", ch, out, NULL, TO_CHAR);
      return;
   }

   amount = UMIN(out->value[1], in->value[0] - in->value[1]);

   in->value[1] += amount;
   out->value[1] -= amount;
   in->value[2] = out->value[2];

   if (vch == NULL)
   {
      sprintf(buf, "You pour %s from $p into $P.",
      liq_table[out->value[2]].liq_name);
      act(buf, ch, out, in, TO_CHAR);
      sprintf(buf, "$n pours %s from $p into $P.",
      liq_table[out->value[2]].liq_name);
      act(buf, ch, out, in, TO_ROOM);
   }
   else
   {
      sprintf(buf, "You pour some %s for $N.",
      liq_table[out->value[2]].liq_name);
      act(buf, ch, NULL, vch, TO_CHAR);
      sprintf(buf, "$n pours you some %s.",
      liq_table[out->value[2]].liq_name);
      act(buf, ch, NULL, vch, TO_VICT);
      sprintf(buf, "$n pours some %s for $N.",
      liq_table[out->value[2]].liq_name);
      act(buf, ch, NULL, vch, TO_NOTVICT);

   }
}

void do_toke( CHAR_DATA *ch, char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   OBJ_DATA *obj;
   OBJ_DATA *obj_pipe;

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );

   if ( arg1[0] == '\0' || arg2[0] == '\0' )
   {
      send_to_char( "Toke <herb> <pipe>\n\r", ch );
      return;
   }

   if ( ( obj_pipe = get_obj_here( ch, arg2 ) ) == NULL )
   {
      act( "I see no $T here.", ch, NULL, arg2, TO_CHAR );
      return;
   }
   if ( obj_pipe->item_type != ITEM_CONTAINER ||
   ( obj_pipe->pIndexData->vnum < 5400 || obj_pipe->pIndexData->vnum > 5499 ))
   {
      send_to_char( "That can't be used for smoking herb.\n\r", ch );
      return;
   }

   obj = get_obj_list( ch, arg1, obj_pipe->contains );
   if ( obj == NULL )
   {
      act( "I see nothing like that in the $T.", ch, NULL, arg2, TO_CHAR );
      return;
   }
   if ( obj->item_type != ITEM_HERB )
   {
      send_to_char( "You wouldn't want to smoke that.\n\r", ch );
      return;
   }

   if (ch->fighting != NULL)
   {
      send_to_char( "You're too busy fighting to smoke anything.\n\r", ch);
      return;
   }
   if ( !IS_NPC(ch) && ch->pcdata->condition[COND_HUNGER] < 10 )
   {
      send_to_char( "You should eat something before smoking anything.\n\r", ch );
      return;
   }
   if ( !IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] < 10 )
   {
      send_to_char( "Your mouth is too dry to smoke anything.\n\r", ch );
      return;
   }
   if (!IS_NPC(ch))
   {
      AFFECT_DATA *af;
      AFFECT_DATA paf;

      if ( (af = affect_find(ch->affected, gsn_high_herb)) != NULL )
      {
         if (af->duration > 18)
         {
            send_to_char( "You'd better come down a little before smoking anymore!\n\r", ch );
            return;
         }
      }
      gain_condition( ch, COND_HUNGER, -obj->value[1]);

      if (!is_branded_by_lestregus(ch))
      {
         gain_condition( ch, COND_THIRST, -obj->value[1]);
      }

      paf.where         = TO_AFFECTS;
      paf.type         = gsn_high_herb;
      paf.level         = ch->level;
      paf.location         = APPLY_AC;
      paf.modifier         = obj->value[1]/4;
      paf.duration         = obj->value[1]/2;
      paf.bitvector         = 0;
      affect_join(ch, &paf, -1);
      act( "$n smokes $p out of $P.", ch, obj, obj_pipe, TO_ROOM );
      act( "You smoke $p out of $P.", ch, obj, obj_pipe, TO_CHAR );
      if ( (af = affect_find(ch->affected, gsn_high_herb)) != NULL )
      {
         if ( af->duration > 18 )
         {
            act( "$n is really high!", ch, NULL, NULL, TO_ROOM );
            act( "You are really high!", ch, NULL, NULL, TO_CHAR );
         }
         else if (number_percent() > 80)
         {
            act( "$n gets a little buzz.", ch, NULL, NULL, TO_ROOM );
            act( "You get a little buzz.", ch, NULL, NULL, TO_CHAR );
         }
      }
      WAIT_STATE(ch, 12);
   }
   extract_obj(obj, FALSE);
   return;
}

void do_drink( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   OBJ_DATA *obj;
   int amount;
   int liquid;
   bool les_branded;

   one_argument( argument, arg );

   if ( arg[0] == '\0' )
   {
      for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
      {
         if ( obj->item_type == ITEM_FOUNTAIN )
         break;
      }

      if ( obj == NULL )
      {
         send_to_char( "Drink what?\n\r", ch );
         return;
      }
   }
   else
   {
      if ( ( obj = get_obj_here( ch, arg ) ) == NULL )
      {
         send_to_char( "You can't find it.\n\r", ch );
         return;
      }
   }

   if (ch->fighting != NULL)
   {
      send_to_char("You're too busy fighting to drink anything.\n\r", ch);
      return;
   }

   les_branded = is_branded_by_lestregus(ch);

   if (
         !les_branded ||
         obj->pIndexData->vnum != OBJ_VNUM_LESTREGUS_FOUNTAIN
      )
   {
      if (!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 10)
      {
         if (ch->race != grn_arborian)
         send_to_char( "You fail to reach your mouth.  *Hic*\n\r", ch );
         else
         send_to_char(
            "You can absorb no more alcohol through your roots "
            "at this time.\n\r",
            ch);
         return;
      }
      if (is_affected(ch, gsn_nausea))
      {
         send_to_char("You are far too nauseated to drink anything.\n\r", ch);
         return;
      }
      if (is_affected(ch, gsn_esurience))
      {
         send_to_char("You are sickened by the sight of drink.\n\r", ch);
         return;
      }
   }

   switch ( obj->item_type )
   {
      default:
      send_to_char( "You can't drink from that.\n\r", ch );
      return;

      case ITEM_FOUNTAIN:
      if ( ( liquid = obj->value[2] )  < 0 )
      {
         sprintf(buf, "Do_drink: bad liquid number, %d, on %s.",
         liquid, obj->short_descr);
         bug(buf, 0);
         liquid = obj->value[2] = 0;
      }
      amount = liq_table[liquid].liq_affect[4] * 3;
      if
      (
         obj->pIndexData->vnum == OBJ_VNUM_SPRING &&
         IS_SET(obj->extra_flags, ITEM_MAGIC)
      )
      {
         check_impure(ch, obj->short_descr, IMPURE_DRINK);
      }
      break;

      case ITEM_DRINK_CON:
      if (( obj->pIndexData->vnum == 3827 )
      && !(IS_NPC(ch) && ch->pIndexData->vnum == 15906)
      /* && (not the vampire lord) */ )
      {
         send_to_char(
            "Something about taking from this bowl chills you to the bone.\n\r",
            ch);
         return;
      }
      if ( obj->value[1] <= 0 )
      {
         send_to_char( "It is already empty.\n\r", ch );
         return;
      }

      if ( ( liquid = obj->value[2] )  < 0 )
      {
         sprintf(buf, "Do_drink: bad liquid number, %d, on %s.",
         liquid, obj->short_descr);
         bug(buf, 0);
         liquid = obj->value[2] = 0;
      }

      amount = liq_table[liquid].liq_affect[4];
      amount = UMIN(amount, obj->value[1]);
      break;
   }
   if (ch->race != grn_arborian)
   {
      act( "$n drinks $T from $p.",
      ch, obj, liq_table[liquid].liq_name, TO_ROOM );
      act( "You drink $T from $p.",
      ch, obj, liq_table[liquid].liq_name, TO_CHAR );
   }
   else
   {
      if (obj->carried_by != ch)
      {
         act( "$n dips $s roots into $p and absorbs $T from it.",
         ch, obj, liq_table[liquid].liq_name, TO_ROOM );
         act( "You dip your roots into $p and absorb $T from it.",
         ch, obj, liq_table[liquid].liq_name, TO_CHAR );
      }
      else
      {
         act( "$n pours $T from $p over $s roots and looks refreshed.",
         ch, obj, liq_table[liquid].liq_name, TO_ROOM );
         act( "You pour $T from $p over your roots and feel refreshed.",
         ch, obj, liq_table[liquid].liq_name, TO_CHAR );
      }
   }
   if (obj->pIndexData->vnum == OBJ_VNUM_CURING_SPRING)
   {
      send_to_char("Your drink from the spring heals you!\n\r", ch);
      ch->hit += 100;
      if (ch->hit > ch->max_hit) ch->hit = ch->max_hit;

      if (is_affected(ch, gsn_poison))
      if (check_dispel(ch->level, ch, gsn_poison))
      {
         act("$n looks much better.", ch, NULL, NULL, TO_ROOM);
      }
      if (is_affected(ch, gsn_plague))
      if (check_dispel(ch->level, ch, gsn_plague))
      {
         act("$n looks relieved as $s sores vanish.", ch, NULL, NULL, TO_ROOM);
      }
      obj->level--;
      if (obj->level <= 0)
      {
         act("$p dries up.", ch, obj, NULL, TO_ALL);
         extract_obj(obj, FALSE);
         obj = NULL;
      }
   }

   if (les_branded)
   {
      if (obj && obj->pIndexData->vnum == 30101)
      {
         if (ch->pcdata->condition[COND_THIRST] > 0)
         {
            send_to_char(
               "Ahh, that tastes nice.  A pity you were not thirsty.\n\r",
               ch);
         }
         else
         {
            /* Satisfy their lethal thirst */
            gain_condition(ch, COND_THIRST, 48);
         }
      }
      else
      {
         send_to_char("That was not very refreshing.\n\r", ch);
      }
   }
   else
   {
      gain_condition(
         ch,
         COND_DRUNK,
         amount * liq_table[liquid].liq_affect[COND_DRUNK] / 36);
      gain_condition(
         ch,
         COND_THIRST,
         amount * liq_table[liquid].liq_affect[COND_THIRST] / 3);
      gain_condition(
         ch,
         COND_HUNGER,
         amount * liq_table[liquid].liq_affect[COND_HUNGER] / 3);

      if (!IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] > 40)
      {
         send_to_char("Your thirst is quenched.\n\r", ch);
      }
   }

   if (!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK]  > 10)
   {
      send_to_char("You feel drunk.\n\r", ch);
   }

   if
   (
      obj != NULL &&
      obj->value[3] != 0 &&
      !check_immune(ch, DAM_POISON)
   )
   {
      /* The drink was poisoned ! */
      AFFECT_DATA af;

      act( "$n chokes and gags.", ch, NULL, NULL, TO_ROOM );
      send_to_char( "You choke and gag.\n\r", ch );
      af.where     = TO_AFFECTS;
      af.type      = gsn_poison;
      af.level         = number_fuzzy(amount);
      af.duration  = 3 * amount;
      af.location  = APPLY_NONE;
      af.modifier  = 0;
      af.bitvector = AFF_POISON;
      affect_join( ch, &af, -1 );
   }

   if
   (
      obj != NULL &&
      obj->value[0] > 0
   )
   {
      obj->value[1] -= amount;
   }

   return;
}

/* feed for illithids - runge */
void do_feed( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   OBJ_DATA *obj;
   int condition;
   int brain = 0;

   one_argument(argument, arg);

   if (arg[0] == '\0')
   {
      sprintf(arg, "corpse");
   }

   if
   (
      !IS_SET(ch->act2, PLR_LICH) &&
      ch->race != grn_illithid &&
      ch->race != grn_demon
   )
   {
      send_to_char
      (
         "Only illithids may feed upon the brains of the fallen.\n\r",
         ch
      );
      return;
   }

   if (is_affected(ch, gsn_nausea))
   {
      send_to_char("You are far too nauseated to eat.\n\r", ch);
      return;
   }
   if (is_affected(ch, gsn_esurience))
   {
      send_to_char("You are sickened by the sight of food.\n\r", ch);
      return;
   }

   obj = get_obj_list(ch, arg, ch->in_room->contents);
   if (obj == NULL)
   {
      send_to_char("You cannot find it.\n\r", ch);
      return;
   }

   if (ch->fighting != NULL)
   {
      send_to_char("You are too busy fighting to feed.\n\r", ch);
      return;
   }
   if
   (
      obj->item_type != ITEM_CORPSE_PC &&
      obj->item_type != ITEM_CORPSE_NPC &&
      obj->pIndexData->vnum != OBJ_VNUM_BRAINS
   )
   {
      send_to_char("That contains no brain upon which to feed!\n\r", ch);
      return;
   }
   if
   (
      (
         obj->item_type == ITEM_CORPSE_PC ||
         obj->item_type != ITEM_CORPSE_NPC
      ) &&
      obj->value[0] > 0 &&
      !IS_SET(race_table[obj->value[0]].parts, PART_BRAINS)
   )
   {
      act("$ts do not have brains.", ch, race_table[obj->value[0]].name, NULL, TO_CHAR);
      return;
   }
   if
   (
      obj->pIndexData->vnum != OBJ_VNUM_BRAINS &&  /* Last short enough already */
      (
         IS_SET(obj->extra_flags, CORPSE_NO_ANIMATE) ||
         obj->timer < 4
      )
   )
   {
      send_to_char("The brain there is far too old to feed upon.\n\r", ch);
      return;
   }
   if (IS_SET(obj->extra_flags2, ITEM_NO_BLOOD)
   )
   {
      send_to_char("The brain is too dried up to consume.\n\r", ch);
      return;
   }
   if (IS_SET(obj->extra_flags, ITEM_NO_BRAINS))
   {
      send_to_char("There are no brains in that corpse.\n\r", ch);
      return;
   }
   if
   (
      !IS_NPC(ch) &&
      !IS_SET(ch->act2, PLR_LICH) &&
      ch->pcdata->condition[COND_HUNGER] > 40
   )
   {
      send_to_char("You are too satiated to feed right now.\n\r", ch);
      return;
   }
   if (obj->pIndexData->vnum == OBJ_VNUM_BRAINS)
   {
      brain = obj->value[1] * 2;
      if
      (
         IS_SET(ch->act2, PLR_LICH) ||
         ch->race == grn_demon
      )
      {
         act
         (
            "$n visciously devours $p, sending bits of flesh everywhere.",
            ch,
            obj,
            NULL,
            TO_ROOM
         );
         act
         (
            "You visciously devour $p, sending bits of flesh everywhere.",
            ch,
            obj,
            NULL,
            TO_CHAR
         );
      }
      else
      {
         act
         (
            "$n sinks its tentacles into $p.",
            ch,
            obj,
            NULL,
            TO_ROOM
         );
         act
         (
            "You sink your tentacles into $p.",
            ch,
            obj,
            NULL,
            TO_CHAR
         );
      }
      extract_obj(obj, FALSE);
   }
   else
   {
      if
      (
         IS_SET(ch->act2, PLR_LICH) ||
         ch->race == grn_demon
      )
      {
         act
         (
            "$n visciously rips $p apart and savagely devours the brain.",
            ch,
            obj,
            NULL,
            TO_ROOM
         );
         act
         (
            "You savagely rip $p apart and devour the brain.",
            ch,
            obj,
            NULL,
            TO_CHAR
         );
      }
      else
      {
         act
         (
            "$n sinks its tentacles into $p and devours the brain.",
            ch,
            obj,
            NULL,
            TO_ROOM
         );
         act
         (
            "You sink your tentacles into $p and devour the brain.",
            ch,
            obj,
            NULL,
            TO_CHAR
         );
      }
      SET_BIT(obj->extra_flags, ITEM_NO_BRAINS);
   }
   if
   (
      IS_NPC(ch) ||
      IS_SET(ch->act2, PLR_LICH)
   )
   {
      return;
   }
   condition = ch->pcdata->condition[COND_HUNGER];
   gain_condition
   (
      ch,
      COND_HUNGER,
      brain ? brain : 30
   );
   if
   (
      condition <= 4 &&
      ch->pcdata->condition[COND_HUNGER] > 4
   )
   {
      send_to_char("You are no longer hungry.\n\r", ch);
   }
   else if (ch->pcdata->condition[COND_HUNGER] > 40)
   {
      send_to_char("You are full.\n\r", ch);
   }
   return;
}

void do_eat(CHAR_DATA *ch, char *argument)
{
   char arg[MAX_INPUT_LENGTH];
   OBJ_DATA *obj;

   one_argument(argument, arg);
   if (arg[0] == '\0')
   {
      send_to_char("Eat what?\n\r", ch);
      return;
   }

   if ((obj = get_obj_carry(ch, arg, ch)) == NULL)
   {
      send_to_char("You do not have that item.\n\r", ch);
      return;
   }

   if (ch->fighting != NULL)
   {
      send_to_char("You're too busy fighting to worry about food.\n\r", ch);
      return;
   }

   if (IS_SET(ch->act2, PLR_LICH))
   {
      send_to_char(
         "Your undead metabolism has no need for food or drink.\n\r",
         ch);
      return;
   }


   /*
    * Apple of Life code.  Note that the redundant Lich check is in case
    * of future reorganization of this code. - Maelstrom
    */

   if (
         obj->pIndexData->vnum == OBJ_VNUM_APPLE_OF_LIFE &&
         !IS_SET(ch->act2, PLR_LICH)
      )
   {
      if (number_percent() <= 20)
      {
         act(
            "$p glows warmly and slowly evaporates.",
            ch,
            obj,
            NULL,
            TO_ROOM);
         act(
            "$p glows warmly as its energy flows into you, granting long life.",
            ch,
            obj,
            NULL,
            TO_CHAR);

         /* Extend their death_age by 30% */
         if (!IS_NPC(ch))
         {
            ch->pcdata->age_mod -= ((get_death_age(ch) * 30)/100);
            return;
         }
      }
      else
      {
         act(
            "$p flares and disappears in a scorching ball of light!",
            ch,
            obj,
            NULL,
            TO_ROOM);
         act(
            "You quake as $p overwhelms your system with energy!",
            ch,
            obj,
            NULL,
            TO_CHAR);

         if (!IS_NPC(ch))
         {
            act(
               "You watch in horror as light pours from $n's eyes and mouth!",
               ch,
               obj,
               NULL,
               TO_ROOM);
            act(
               "$p has scorched away your remaining life in one instant!",
               ch,
               obj,
               NULL,
               TO_CHAR);

            /* Set their death_age to 25 years ago and then kill them */
            ch->pcdata->age_mod += (get_death_age(ch) - get_age(ch)) + 25;
            age_update_char(ch);

            return;
         }
      }
   }

   if (!IS_IMMORTAL(ch))
   {
      if (ch->race == grn_arborian)
      {
         send_to_char("You have no mouth with which to eat anything.\n\r", ch);
         return;
      }

      if (obj->item_type != ITEM_FOOD && obj->item_type != ITEM_PILL)
      {
         send_to_char("That's not edible.\n\r", ch);
         return;
      }

      if (!IS_NPC(ch) && ch->pcdata->condition[COND_HUNGER] > 40)
      {
         send_to_char("You are too full to eat more.\n\r", ch);
         return;
      }

      if (is_affected(ch, gsn_nausea))
      {
         send_to_char("You are far too nauseated to eat.\n\r", ch);
         return;
      }

      if (is_affected(ch, gsn_esurience))
      {
         send_to_char("You are sickened by the sight of food.\n\r", ch);
         return;
      }
   }

   if (
         (ch->race == grn_illithid || ch->race == grn_demon) &&
         !IS_IMMORTAL(ch)
      )
   {
      if (obj->pIndexData->vnum == OBJ_VNUM_BRAINS)
      {
         if (ch->race == grn_demon)
         {
            send_to_char("Feed upon it.\n\r", ch);
         }
         else
         {
            send_to_char("Sink your tentacles into it.\n\r", ch);
         }
         return;
      }

      act("$n chokes down $p.",  ch, obj, NULL, TO_ROOM);
      act(
         "Your system doesn't like $p, but you eat it anyway.",
         ch,
         obj,
         NULL,
         TO_CHAR);
   }
   else
   {
      act("$n eats $p.",  ch, obj, NULL, TO_ROOM);
      act("You eat $p.", ch, obj, NULL, TO_CHAR);
   }

   switch (obj->item_type)
   {
      case ITEM_FOOD:
      {
         if (!IS_NPC(ch))
         {
            int condition;

            condition = ch->pcdata->condition[COND_HUNGER];
            if (ch->race == grn_illithid || ch->race == grn_demon)
            {
               gain_condition(ch, COND_HUNGER, dice(1, 2));
            }
            else
            {
               gain_condition(ch, COND_HUNGER, obj->value[1]*2);
            }
            if (condition <= 4 && ch->pcdata->condition[COND_HUNGER] > 4)
            {
               send_to_char("You are no longer hungry.\n\r", ch);
            }
            else if (ch->pcdata->condition[COND_HUNGER] > 40)
            {
               send_to_char("You are full.\n\r", ch);
            }

            if (is_pulsing(obj))
            {
               check_impure(ch, obj->short_descr, IMPURE_EAT);
            }
         }

         if (obj->value[3] != 0 && check_immune(ch, DAM_POISON) != IS_IMMUNE)
         {
            /* The food was poisoned! */
            AFFECT_DATA af;

            act("$n chokes and gags.", ch, NULL, NULL, TO_ROOM);
            send_to_char("You choke and gag.\n\r", ch );

            af.where         = TO_AFFECTS;
            af.type          = gsn_poison;
            af.level         = number_fuzzy(obj->value[0]);
            af.duration      = 2 * obj->value[0];
            if (af.duration < 1)
            {
               af.duration   = 1;
            }
            af.location      = APPLY_NONE;
            af.modifier      = 0;
            af.bitvector     = AFF_POISON;
            affect_join(ch, &af, -1);
         }
         break;
      }
      case ITEM_PILL:
      {
         check_impure(ch, obj->short_descr, IMPURE_EAT_PILL);
         obj_cast_spell(obj->value[1], obj->value[0], ch, ch, NULL);
         if (ch->in_room != NULL)
         {
            obj_cast_spell(obj->value[2], obj->value[0], ch, ch, NULL);
         }
         if (ch->in_room != NULL)
         {
            obj_cast_spell(obj->value[3], obj->value[0], ch, ch, NULL);
         }
         if (ch->in_room != NULL)
         {
            obj_cast_spell(obj->value[4], obj->value[0], ch, ch, NULL);
         }
         break;
      }
   }

   extract_obj(obj, FALSE);
   if (IS_IMMORTAL(ch))
   {
      sprintf(log_buf, "%s eats: %s.", ch->name, obj->short_descr);
      log_string(log_buf);
   }
   return;
}



/*
   Remove an object.
*/
bool remove_obj( CHAR_DATA *ch, int iWear, bool fReplace, bool show )
{
   OBJ_DATA *obj;
   OBJ_DATA *secondary;
   /* There's a pile of dual wield and trinal wield crap in here all over the
   * place...Very messy, but i was coding it in a rush...Ceran.
   */
   if ( ( obj = get_eq_char( ch, iWear ) ) == NULL )
   return TRUE;

   if ( !fReplace )
   return FALSE;

   if
   (
      ch->race == grn_book &&
      iWear == WEAR_INSIDE_COVER &&
      !IS_NPC(ch)
   )
   {
      if (ch->pcdata->current_desc == BOOK_RACE_CLOSED)
      {
         send_to_char("You are closed right now.\n\r", ch);
         return FALSE;
      }
      if (ch->pcdata->current_desc == BOOK_RACE_LOCKED)
      {
         send_to_char("You are closed and locked right now.\n\r", ch);
         return FALSE;
      }
   }
   if ( IS_SET(obj->extra_flags, ITEM_NOREMOVE) && !IS_IMMORTAL(ch) )
   {
      if ( show )
      act( "You can't remove $p.", ch, obj, NULL, TO_CHAR );
      return FALSE;
   }

   if (obj->pIndexData->vnum == OBJ_VNUM_GENOCIDE_AXE)
   remove_obj_genocide(ch, obj);

   if (obj->pIndexData->vnum == 24418)
   remove_obj_girdle(ch, obj); else
   if (obj->pIndexData->vnum == 16022)
   remove_obj_troll_skin(ch, obj); else
   if (obj->pIndexData->vnum == OBJ_VNUM_SYMBOL_MAGIC)
   remove_obj_symbol_magic(ch, obj); else

   if (obj->pIndexData->vnum == 15123)
   remove_obj_channeling(ch, obj); else
   if (obj->pIndexData->vnum == OBJ_VNUM_WIZARDRY_1
   || obj->pIndexData->vnum == OBJ_VNUM_WIZARDRY_2)
   remove_obj_wizardry(ch, obj); else

   if (obj->pIndexData->vnum == OBJ_VNUM_TIARA)
   remove_obj_tiara(ch, obj); else
   if (obj->pIndexData->vnum == 19002)
   remove_obj_19002(ch, obj); else
   if (obj->pIndexData->vnum == 19445)
   remove_obj_bracers(ch, obj); else
   if (obj->pIndexData->vnum == 18792)
   remove_obj_elven_armguards(ch, obj); else
   if (obj->pIndexData->vnum == 23706)
   remove_obj_dragonmage_feet(ch, obj); else
   if (obj->pIndexData->vnum == 23751)
   remove_obj_blue_hide(ch, obj); else
   if (obj->pIndexData->vnum == 23752)
   remove_obj_green_hide(ch, obj); else
   if (obj->pIndexData->vnum == OBJ_VNUM_HEAVENLY_SCEPTRE)
   remove_obj_sceptre(ch, obj); else
   if (obj->pIndexData->vnum == 12161)
   remove_obj_whitehelm(ch, obj); else
   if (obj->pIndexData->vnum == 13711)
   remove_obj_fallen_wings(ch, obj); else
   if (obj->pIndexData->vnum == OBJ_VNUM_BLACK_HIDE) /* black dragon hide */
   remove_obj_black_hide(ch, obj); else
   if (obj->pIndexData->vnum == OBJ_VNUM_MISTY_ESSENCE) /* misty cloak */
   remove_obj_misty_cloak(ch, obj); else
   if (obj->pIndexData->vnum == OBJ_VNUM_CLOAK_UNDERWORLD) /* cloak of underworld */
   remove_obj_cloak_underworld(ch, obj); else
   if (obj->pIndexData->vnum == OBJ_VNUM_RED_DRAGONSTAR) /* red dragonstar */
   remove_obj_red_dragonstar(ch, obj); else
   if (obj->pIndexData->vnum == OBJ_VNUM_BLACK_DRAGONSTAR) /* black dragonstar */
   remove_obj_black_dragonstar(ch, obj); else
   if (obj->pIndexData->vnum == OBJ_VNUM_SHADOW_HIDE) /* shadow dragon hide */
   remove_obj_shadow_hide(ch, obj); else
   if (obj->pIndexData->vnum == OBJ_VNUM_RING_PROTECT)
   remove_obj_ring_protection(ch, obj); else
   if (obj->pIndexData->vnum == OBJ_VNUM_ROBE_AVENGER)
   remove_obj_robe_avenger(ch, obj);
   unequip_char( ch, obj );
   secondary = get_eq_char(ch, WEAR_DUAL_WIELD);

   if (iWear == WEAR_WIELD && secondary != NULL)
   {
      unequip_char(ch, secondary);
      equip_char(ch, secondary, WEAR_WIELD);
   }
   if (iWear == WEAR_DUAL_WIELD)
   {
      if (ch->race != grn_book)
      {
         act("$n stops dual wielding $p.", ch, obj, NULL, TO_ROOM);
         act("You stop dual wielding $p.", ch, obj, NULL, TO_CHAR);
      }
      else
      {
         act("$n stops grasping $p with $s tassles.", ch, obj, NULL, TO_ROOM);
         act("You stop grasping $p with your tassles.", ch, obj, NULL, TO_CHAR);
      }
      return TRUE;
   }

   act( "$n stops using $p.", ch, obj, NULL, TO_ROOM );
   act( "You stop using $p.", ch, obj, NULL, TO_CHAR );
   return TRUE;
}



/*
   Wear one object.
   Optional replacement of existing objects.
   Big repetitive code, ick.
*/
bool wear_obj( CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace )
{
   OBJ_DATA *secondary;
   OBJ_DATA *obj_check;
   OBJ_DATA *primary;
   OBJ_DATA *weapon;
   OBJ_DATA *oldobj;
   int sn, skill;
   bool status = FALSE;
   bool book = ch->race == grn_book;
   if ( obj == NULL )
   return FALSE;

   if ( obj->pIndexData->vnum == OBJ_VNUM_COIL_SNAKES )
   {
      if ( (get_eq_char( ch, WEAR_NECK_1 ) != NULL
      && get_eq_char( ch, WEAR_NECK_2 ) != NULL)
      && !remove_obj( ch, WEAR_NECK_2, fReplace, FALSE )
      && !remove_obj( ch, WEAR_NECK_2, fReplace, FALSE ) )
      {
         if ( !remove_obj( ch, WEAR_BODY, fReplace, FALSE ) )
         {
            if ( !remove_obj( ch, WEAR_WAIST, fReplace, FALSE ) )
            {
               oldobj = get_eq_char(ch, WEAR_HEAD);
               if ( oldobj != NULL
               && oldobj->pIndexData->vnum == 12161 )
               remove_obj_whitehelm(ch, oldobj);

               if ( !remove_obj( ch, WEAR_HEAD, fReplace, FALSE ) )
               {
                  send_to_char( "The coil of snakes writhes violently, slipping out of your grasp.\n\r", ch );
                  act( "$n tries to pick up a coil of snakes, but they slip from $s grasp.", ch, obj, NULL, TO_ROOM );
                  return FALSE;
               }

               act( "A coil of snakes wraps tightly around $n's head!",   ch, obj, NULL, TO_ROOM );
               send_to_char( "The coil of snakes wraps tightly around your head and squeezes!\n\r", ch );
               equip_char( ch, obj, WEAR_HEAD );
               return TRUE;
            } else
            {
               act( "A coil of snakes wraps tightly around $n's waist!",   ch, obj, NULL, TO_ROOM );
               send_to_char( "The coil of snakes wraps tightly around your waist and squeezes!\n\r", ch );
               equip_char( ch, obj, WEAR_WAIST );
               return TRUE;
            }
         } else
         {
            act( "A coil of snakes wraps tightly around $n's torso!",   ch, obj, NULL, TO_ROOM );
            send_to_char( "The coil of snakes wraps tightly around your torso and squeezes!\n\r", ch );
            equip_char( ch, obj, WEAR_BODY );
            return TRUE;
         }
      } else
      {
         if ( get_eq_char( ch, WEAR_NECK_1 ) == NULL )
         {
            act( "A coil of snakes wraps tightly around $n's neck!",   ch, obj, NULL, TO_ROOM );
            send_to_char( "The coil of snakes wraps tightly around your neck and squeezes!\n\r", ch );
            equip_char( ch, obj, WEAR_NECK_1 );
            return TRUE;
         } else
         if ( get_eq_char( ch, WEAR_NECK_2 ) == NULL )
         {
            act( "A coil of snakes wraps tightly around $n's neck!",   ch, obj, NULL, TO_ROOM );
            send_to_char( "The coil of snakes wraps tightly around your neck and squeezes!\n\r", ch );
            equip_char( ch, obj, WEAR_NECK_2 );
            return TRUE;
         }
      }
      bug( "Wear_obj: COIL_SNAKES found occupied slot.", 0 );
      return FALSE;
   }

   if (ch->class == GUILD_MONK-1){
      if (CAN_WEAR(obj, ITEM_WEAR_NECK) &&
      obj->weight > 10){
         send_to_char("It is too heavy for you to wear about your neck.\n\r", ch);
         return FALSE;
      }
   }

   if (ch->class == CLASS_MONK && CAN_WEAR(obj, ITEM_WEAR_FEET))
   {
      if (obj->pIndexData->vnum != OBJ_VNUM_SHACKLES)
      return FALSE;
   }

   if (ch->class == CLASS_MONK)
   {
      if
      (
         !CAN_WEAR( obj, ITEM_WEAR_FINGER) &&
         !CAN_WEAR( obj, ITEM_WEAR_NECK)   &&
         !CAN_WEAR( obj, ITEM_WEAR_WAIST) &&
         !CAN_WEAR( obj, ITEM_WEAR_ABOUT) &&
         !CAN_WEAR( obj, ITEM_WEAR_FEET) &&
         !CAN_WEAR( obj, ITEM_HOLD) &&
         !CAN_WEAR( obj, ITEM_WEAR_EYES) &&
         !CAN_WEAR( obj, ITEM_WEAR_BRAND) &&
         !CAN_WEAR( obj, ITEM_WEAR_WRIST) &&
         obj->pIndexData->vnum != OBJ_VNUM_GENOCIDE_AXE
      )
      {
         return FALSE;
      }
   }

   if ( CAN_WEAR( obj, ITEM_WEAR_FINGER ) )
   {
      if ( get_eq_char( ch, WEAR_FINGER_L ) != NULL
      &&   get_eq_char( ch, WEAR_FINGER_R ) != NULL
      &&   !remove_obj( ch, WEAR_FINGER_L, fReplace, TRUE )
      &&   !remove_obj( ch, WEAR_FINGER_R, fReplace, TRUE) )
      return FALSE;

      if ( get_eq_char( ch, WEAR_FINGER_L ) == NULL )
      {
         if (!book)
         {
            act( "$n wears $p on $s left finger.",    ch, obj, NULL, TO_ROOM );
            act( "You wear $p on your left finger.",  ch, obj, NULL, TO_CHAR );
         }
         else
         {
            act("$n slides $p onto a tassle string.",    ch, obj, NULL, TO_ROOM );
            act("You wear $p on a tassle string.",  ch, obj, NULL, TO_CHAR );
         }
         equip_char( ch, obj, WEAR_FINGER_L );
         return TRUE;
      }

      if ( get_eq_char( ch, WEAR_FINGER_R ) == NULL )
      {
         if (!book)
         {
            act( "$n wears $p on $s right finger.",   ch, obj, NULL, TO_ROOM );
            act( "You wear $p on your right finger.", ch, obj, NULL, TO_CHAR );
         }
         else
         {
            act("$n slides $p onto a tassle string.",    ch, obj, NULL, TO_ROOM );
            act("You wear $p on a tassle string.",  ch, obj, NULL, TO_CHAR );
         }
         equip_char( ch, obj, WEAR_FINGER_R );
         return TRUE;
      }

      bug( "Wear_obj: no free finger.", 0 );
      send_to_char( "You already wear two rings.\n\r", ch );
      return FALSE;
   }

   if ( CAN_WEAR( obj, ITEM_WEAR_NECK ) )
   {
      if ( get_eq_char( ch, WEAR_NECK_1 ) != NULL
      &&   get_eq_char( ch, WEAR_NECK_2 ) != NULL
      &&   !remove_obj( ch, WEAR_NECK_1, fReplace, TRUE )
      &&   !remove_obj( ch, WEAR_NECK_2, fReplace, TRUE ) )
      return FALSE;

      if (obj->pIndexData->vnum == OBJ_VNUM_BADGE)
      {
         obj_check = get_eq_char(ch, WEAR_NECK_1);
         if (obj_check != NULL && obj_check->pIndexData->vnum == OBJ_VNUM_BADGE)
         status = TRUE;
         obj_check = get_eq_char(ch, WEAR_NECK_2);
         if (obj_check != NULL && obj_check->pIndexData->vnum == OBJ_VNUM_BADGE)
         status = TRUE;

         if (status)
         {
            send_to_char("You are only able to wear one such item.\n\r", ch);
            return FALSE;
         }
         send_to_char("You feel at odds with the law!\n\r", ch);
      }

      if ( get_eq_char( ch, WEAR_NECK_1 ) == NULL )
      {
         if (!book)
         {
            act( "$n wears $p around $s neck.",   ch, obj, NULL, TO_ROOM );
            act( "You wear $p around your neck.", ch, obj, NULL, TO_CHAR );
         }
         else
         {
            act("$n wears $p around $s bookmark.",   ch, obj, NULL, TO_ROOM);
            act("You wear $p around your bookmark.", ch, obj, NULL, TO_CHAR);
         }
         equip_char( ch, obj, WEAR_NECK_1 );
         return TRUE;
      }

      if ( get_eq_char( ch, WEAR_NECK_2 ) == NULL )
      {
         if (!book)
         {
            act( "$n wears $p around $s neck.",   ch, obj, NULL, TO_ROOM );
            act( "You wear $p around your neck.", ch, obj, NULL, TO_CHAR );
         }
         else
         {
            act("$n wears $p around $s bookmark.",   ch, obj, NULL, TO_ROOM);
            act("You wear $p around your bookmark.", ch, obj, NULL, TO_CHAR);
         }
         equip_char( ch, obj, WEAR_NECK_2 );
         return TRUE;
      }

      bug( "Wear_obj: no free neck.", 0 );
      send_to_char( "You already wear two neck items.\n\r", ch );
      return FALSE;
   }

   if ( CAN_WEAR( obj, ITEM_WEAR_BODY ) )
   {
      if ( !remove_obj( ch, WEAR_BODY, fReplace, TRUE ) )
      return FALSE;
      if (!book)
      {
         act( "$n wears $p on $s torso.",   ch, obj, NULL, TO_ROOM );
         act( "You wear $p on your torso.", ch, obj, NULL, TO_CHAR );
      }
      else
      {
         act( "$n wears $p around $s cover.",   ch, obj, NULL, TO_ROOM );
         act( "You wear $p around your cover.", ch, obj, NULL, TO_CHAR );
      }
      equip_char( ch, obj, WEAR_BODY );
      return TRUE;
   }


   if ( CAN_WEAR( obj, ITEM_WEAR_HANDS ) )
   {
      if ( !remove_obj( ch, WEAR_HANDS, fReplace, TRUE ) )
      return FALSE;
      if (!book)
      {
         act( "$n wears $p on $s hands.",   ch, obj, NULL, TO_ROOM );
         act( "You wear $p on your hands.", ch, obj, NULL, TO_CHAR );
      }
      else
      {
         act("$n wears $p on $s tassles.",   ch, obj, NULL, TO_ROOM);
         act("You wear $p on your tassles.", ch, obj, NULL, TO_CHAR);
      }
      equip_char( ch, obj, WEAR_HANDS );
      return TRUE;
   }

   if ( CAN_WEAR( obj, ITEM_WEAR_ARMS ) )
   {
      if ( !remove_obj( ch, WEAR_ARMS, fReplace, TRUE ) )
      return FALSE;
      if (!book)
      {
         act( "$n wears $p on $s arms.",   ch, obj, NULL, TO_ROOM );
         act( "You wear $p on your arms.", ch, obj, NULL, TO_CHAR );
      }
      else
      {
         act("$n hangs $p over $s tassles.",   ch, obj, NULL, TO_ROOM);
         act("You hang $p over your tassles.", ch, obj, NULL, TO_CHAR);
      }
      equip_char( ch, obj, WEAR_ARMS );
      return TRUE;
   }

   if ( CAN_WEAR( obj, ITEM_WEAR_ABOUT ) )
   {
      if ( !remove_obj( ch, WEAR_ABOUT, fReplace, TRUE ) )
      return FALSE;
      if (!book)
      {
         act( "$n wears $p about $s body.",   ch, obj, NULL, TO_ROOM );
         act( "You wear $p about your body.", ch, obj, NULL, TO_CHAR );
      }
      else
      {
         act("$n hangs $p over $s cover.",   ch, obj, NULL, TO_ROOM);
         act("You hang $p over your cover.", ch, obj, NULL, TO_CHAR);
      }
      equip_char( ch, obj, WEAR_ABOUT );
      return TRUE;
   }

   if ( CAN_WEAR( obj, ITEM_WEAR_WAIST ) )
   {
      if ( !remove_obj( ch, WEAR_WAIST, fReplace, TRUE ) )
      return FALSE;
      if (!book)
      {
         act( "$n wears $p about $s waist.",   ch, obj, NULL, TO_ROOM );
         act( "You wear $p about your waist.", ch, obj, NULL, TO_CHAR );
      }
      else
      {
         act("$n ties $p around $mself.",   ch, obj, NULL, TO_ROOM);
         act("You tie $p around yourself.", ch, obj, NULL, TO_CHAR);
      }
      equip_char( ch, obj, WEAR_WAIST );
      return TRUE;
   }

   if ( CAN_WEAR( obj, ITEM_WEAR_WRIST ) )
   {
      if ( get_eq_char( ch, WEAR_WRIST_L ) != NULL
      &&   get_eq_char( ch, WEAR_WRIST_R ) != NULL
      &&   !remove_obj( ch, WEAR_WRIST_L, fReplace, TRUE )
      &&   !remove_obj( ch, WEAR_WRIST_R, fReplace, TRUE ) )
      return FALSE;

      if ( get_eq_char( ch, WEAR_WRIST_L ) == NULL )
      {
         if (!book)
         {
            act("$n wears $p around $s left wrist.", ch, obj, NULL, TO_ROOM);
            act("You wear $p around your left wrist.", ch, obj, NULL, TO_CHAR);
         }
         else
         {
            act("$n wears $p around $s left tassle.", ch, obj, NULL, TO_ROOM);
            act("You wear $p around your left tassle.", ch, obj, NULL, TO_CHAR);
         }
         equip_char( ch, obj, WEAR_WRIST_L );
         return TRUE;
      }

      if ( get_eq_char( ch, WEAR_WRIST_R ) == NULL )
      {
         if (!book)
         {
            act("$n wears $p around $s right wrist.", ch, obj, NULL, TO_ROOM);
            act("You wear $p around your right wrist.", ch, obj, NULL, TO_CHAR);
         }
         else
         {
            act("$n wears $p around $s right tassle.", ch, obj, NULL, TO_ROOM);
            act("You wear $p around your right tassle.", ch, obj, NULL, TO_CHAR);
         }
         equip_char( ch, obj, WEAR_WRIST_R );
         return TRUE;
      }

      bug( "Wear_obj: no free wrist.", 0 );
      send_to_char( "You already wear two wrist items.\n\r", ch );
      return FALSE;
   }
   /* Ear and Eyes Eq Slots -- Drinlinda */
   if ( CAN_WEAR( obj, ITEM_WEAR_EYES ) )
   {
      if ( !remove_obj( ch, WEAR_EYES, fReplace, TRUE ) )
      return FALSE;
      if (!book)
      {
         act( "$n wears $p over $s eyes.",   ch, obj, NULL, TO_ROOM );
         act( "You wear $p over your eyes.", ch, obj, NULL, TO_CHAR );
      }
      else
      {
         act("$n wedges $p into $s pages.",   ch, obj, NULL, TO_ROOM);
         act("You wedge $p into your pages.", ch, obj, NULL, TO_CHAR);
      }
      equip_char( ch, obj, WEAR_EYES );
      return TRUE;
   }

   if ( CAN_WEAR( obj, ITEM_WEAR_EAR ) )
   {
      if ( get_eq_char( ch, WEAR_EAR_L ) != NULL
      &&   get_eq_char( ch, WEAR_EAR_R ) != NULL
      &&   !remove_obj( ch, WEAR_EAR_L, fReplace, TRUE )
      &&   !remove_obj( ch, WEAR_EAR_R, fReplace, TRUE ) )
      return FALSE;

      if ( get_eq_char( ch, WEAR_EAR_L ) == NULL )
      {
         if (!book)
         {
            act("$n wears $p on $s left ear.", ch, obj, NULL, TO_ROOM);
            act("You wear $p on your left ear.", ch, obj, NULL, TO_CHAR);
         }
         else
         {
            act("$n wears $p in $s left tassle.", ch, obj, NULL, TO_ROOM);
            act("You wear $p in your left tassle.", ch, obj, NULL, TO_CHAR);
         }
         equip_char( ch, obj, WEAR_EAR_L );
         return TRUE;
      }

      if ( get_eq_char( ch, WEAR_EAR_R ) == NULL )
      {
         if (!book)
         {
            act("$n wears $p on $s right ear.", ch, obj, NULL, TO_ROOM);
            act("You wear $p on your right ear.", ch, obj, NULL, TO_CHAR);
         }
         else
         {
            act("$n wears $p in $s right tassle.", ch, obj, NULL, TO_ROOM);
            act("You wear $p in your right tassle.", ch, obj, NULL, TO_CHAR);
         }
         equip_char( ch, obj, WEAR_EAR_R );
         return TRUE;
      }

      bug( "Wear_obj: no free ear.", 0 );
      send_to_char( "You already wear two ear items.\n\r", ch );
      return FALSE;
   }

   /* Racial Eq Slots -- Drinlinda */
   /* Minotaur Horn Wear Slot */
   /* Choice of horns&head slots - FIZZFALDT */
   if (CAN_WEAR(obj, ITEM_WEAR_HEAD) || CAN_WEAR(obj, ITEM_WEAR_HORNS))
   {
      if
      (
         (
            CAN_WEAR(obj, ITEM_WEAR_HEAD) &&
            ch->race != grn_minotaur
         ) ||
         !CAN_WEAR(obj, ITEM_WEAR_HORNS)
      )
      {
         if (CAN_WEAR( obj, ITEM_WEAR_HEAD))
         {
            oldobj =(get_eq_char(ch, WEAR_HEAD));
            if ( oldobj != NULL )
            if ( oldobj->pIndexData->vnum == 12161 )
            remove_obj_whitehelm(ch, oldobj);

            if ( !remove_obj( ch, WEAR_HEAD, fReplace, TRUE ) )
            return FALSE;
            if
            (
               ch->race != grn_minotaur
            )
            {
               if (!book)
               {
                  act("$n wears $p on $s head.",   ch, obj, NULL, TO_ROOM);
                  act("You wear $p on your head.", ch, obj, NULL, TO_CHAR);
               }
               else
               {
                  act
                  (
                     "$n wears $p on top of $s cover.",
                     ch,
                     obj,
                     NULL,
                     TO_ROOM
                  );
                  act
                  (
                     "You wear $p on top of your cover.",
                     ch,
                     obj,
                     NULL,
                     TO_CHAR
                  );
               }
            }
            equip_char( ch, obj, WEAR_HEAD );
            return TRUE;
         }
      }
      else
      {
         if (CAN_WEAR(obj, ITEM_WEAR_HORNS))
         {
            if (!remove_obj(ch, WEAR_HORNS, fReplace, TRUE))
            {
               return FALSE;
            }
            if (ch->race == grn_minotaur)
            {
               if (!book)
               {
                  act("$n wears $p on $s horns.",   ch, obj, NULL, TO_ROOM);
                  act("You wear $p on your horns.", ch, obj, NULL, TO_CHAR);
               }
               else
               {
                  act
                  (
                     "$n wears $p on top of $s cover.",
                     ch,
                     obj,
                     NULL,
                     TO_ROOM
                  );
                  act
                  (
                     "You wear $p on top of your cover.",
                     ch,
                     obj,
                     NULL,
                     TO_CHAR
                  );
               }
               equip_char(ch, obj, WEAR_HORNS);
               return TRUE;
            }
            equip_char(ch, obj, WEAR_HORNS);
            return FALSE;
         }
      }
   }

   /* Choice of feet&hooves&fourhooves slots - FIZZFALDT */
   if
   (
      CAN_WEAR(obj, ITEM_WEAR_FEET) ||
      CAN_WEAR(obj, ITEM_WEAR_HOOVES) ||
      CAN_WEAR(obj, ITEM_WEAR_FOURHOOVES)
   )
   {
      if
      (
         (
            CAN_WEAR(obj, ITEM_WEAR_FEET) &&
            (
               (
                  ch->race != grn_minotaur &&
                  ch->race != grn_centaur
               ) ||
               (
                  ch->race == grn_minotaur &&
                  !CAN_WEAR(obj, ITEM_WEAR_HOOVES)
               ) ||
               (
                  ch->race == grn_centaur &&
                  !CAN_WEAR(obj, ITEM_WEAR_FOURHOOVES)
               )
            )

         ) ||
         (
            !CAN_WEAR(obj, ITEM_WEAR_HOOVES) &&
            !CAN_WEAR(obj, ITEM_WEAR_FOURHOOVES)
         )
      )
      {
         if ( CAN_WEAR( obj, ITEM_WEAR_FEET ) )
         {
            if ( !remove_obj( ch, WEAR_FEET, fReplace, TRUE ) )
            return FALSE;
            if
            (
               ch->race != grn_minotaur &&
               ch->race != grn_centaur
            )
            {
               if (!book)
               {
                  act("$n wears $p on $s feet.",   ch, obj, NULL, TO_ROOM);
                  act("You wear $p on your feet.", ch, obj, NULL, TO_CHAR);
               }
               else
               {
                  act
                  (
                     "$n hops onto $p and starts dragging it on the floor.",
                     ch,
                     obj,
                     NULL,
                     TO_ROOM
                  );
                  act
                  (
                     "You hop into $p and start dragging it on the floor.",
                     ch,
                     obj,
                     NULL,
                     TO_CHAR
                  );
               }
            }
            equip_char( ch, obj, WEAR_FEET );
            return TRUE;
         }
      }
      else if
      (
         (
            CAN_WEAR(obj, ITEM_WEAR_HOOVES) &&
            ch->race == grn_minotaur
         ) ||
         (
            !CAN_WEAR(obj, ITEM_WEAR_FEET) &&
            !CAN_WEAR(obj, ITEM_WEAR_FOURHOOVES)
         )
      )
      {
         /* Minotaur Hooves Wear Slot */
         if (CAN_WEAR(obj, ITEM_WEAR_HOOVES))
         {
            if (!remove_obj( ch, WEAR_HOOVES, fReplace, TRUE))
            {
               return FALSE;
            }
            if (ch->race == grn_minotaur)
            {
               act("$n wears $p on $s hooves.",   ch, obj, NULL, TO_ROOM );
               act("You wear $p on your hooves.", ch, obj, NULL, TO_CHAR );
               equip_char(ch, obj, WEAR_HOOVES);
               return TRUE;
            }
            equip_char(ch, obj, WEAR_HOOVES);
            return FALSE;
         }
      }
      else
      {
         /* Centaur Four Hooves Wear Slot */
         if (CAN_WEAR( obj, ITEM_WEAR_FOURHOOVES))
         {
            if (!remove_obj(ch, WEAR_FOURHOOVES, fReplace, TRUE))
            {
               return FALSE;
            }
            if (ch->race == grn_centaur)
            {
               act("$n wears $p on $s four hooves.",   ch, obj, NULL, TO_ROOM );
               act("You wear $p on your four hooves.", ch, obj, NULL, TO_CHAR );
               equip_char(ch, obj, WEAR_FOURHOOVES);
               return TRUE;
            }
            equip_char(ch, obj, WEAR_FOURHOOVES);
            return FALSE;
         }
      }
   }


   /* Minotaur Snout Wear Slot */
   if (CAN_WEAR(obj, ITEM_WEAR_SNOUT))
   {
      if (!remove_obj( ch, WEAR_SNOUT, fReplace, TRUE))
      {
         return FALSE;
      }
      if (ch->race == grn_minotaur)
      {
         act("$n wears $p on $s snout.",   ch, obj, NULL, TO_ROOM );
         act("You wear $p on your snout.", ch, obj, NULL, TO_CHAR );
         equip_char(ch, obj, WEAR_SNOUT);
         return TRUE;
      }
      equip_char(ch, obj, WEAR_SNOUT);
      return FALSE;
   }

   /* book inside cover slot */
   if (CAN_WEAR(obj, ITEM_WEAR_INSIDE_COVER))
   {
      if (ch->race == grn_book)
      {
         if (!IS_NPC(ch))
         {
            if (ch->pcdata->current_desc == BOOK_RACE_CLOSED)
            {
               send_to_char("You are closed right now.\n\r", ch);
               return FALSE;
            }
            if (ch->pcdata->current_desc == BOOK_RACE_LOCKED)
            {
               send_to_char("You are closed and locked right now.\n\r", ch);
               return FALSE;
            }
         }
         if (!remove_obj(ch, WEAR_INSIDE_COVER, fReplace, TRUE))
         {
            return FALSE;
         }
         act("$n wears $p inside $s cover.",   ch, obj, NULL, TO_ROOM );
         act("You wear $p inside your cover.", ch, obj, NULL, TO_CHAR );
         equip_char(ch, obj, WEAR_INSIDE_COVER);
         return TRUE;
      }
      if (!remove_obj(ch, WEAR_INSIDE_COVER, fReplace, TRUE))
      {
         return FALSE;
      }
      equip_char(ch, obj, WEAR_INSIDE_COVER);
      return FALSE;
   }

   /* Choice of legs&fourlegs slots - FIZZFALDT */
   if
   (
      CAN_WEAR(obj, ITEM_WEAR_LEGS) ||
      CAN_WEAR(obj, ITEM_WEAR_FOURLEGS)
   )
   {
      if
      (
         (
            CAN_WEAR(obj, ITEM_WEAR_LEGS) &&
            ch->race != grn_centaur
         ) ||
         (
            !CAN_WEAR(obj, ITEM_WEAR_FOURLEGS)
         )
      )
      {
         if ( CAN_WEAR( obj, ITEM_WEAR_LEGS ) )
         {
            if ( !remove_obj( ch, WEAR_LEGS, fReplace, TRUE ) )
            return FALSE;
            if
            (
               ch->race != grn_centaur
            )
            {
               if (!book)
               {
                  act("$n wears $p on $s legs.",   ch, obj, NULL, TO_ROOM);
                  act("You wear $p on your legs.", ch, obj, NULL, TO_CHAR);
               }
               else
               {
                  act
                  (
                     "$n ties $p around $s lower tassles.",
                     ch,
                     obj,
                     NULL,
                     TO_ROOM
                  );
                  act
                  (
                     "You tie $p around your lower tassles.",
                     ch,
                     obj,
                     NULL,
                     TO_CHAR
                  );
               }
            }
            equip_char( ch, obj, WEAR_LEGS );
            return TRUE;
         }
      }
      else
      {
         /* Centaur Four Legs Wear Slot */
         if (CAN_WEAR(obj, ITEM_WEAR_FOURLEGS))
         {
            if (!remove_obj(ch, WEAR_FOURLEGS, fReplace, TRUE))
            {
               return FALSE;
            }
            if (ch->race == grn_centaur)
            {
               act("$n wears $p on $s four legs.",   ch, obj, NULL, TO_ROOM );
               act("You wear $p on your four legs.", ch, obj, NULL, TO_CHAR );
               equip_char(ch, obj, WEAR_FOURLEGS);
               return TRUE;
            }
            equip_char(ch, obj, WEAR_FOURLEGS);
            return FALSE;
         }
      }
   }

   /* Drinlinda's bosom slot */
   if (CAN_WEAR(obj, ITEM_WEAR_BOSOM))
   {
      if (!remove_obj(ch, WEAR_BOSOM, fReplace, TRUE))
      {
         return FALSE;
      }
      if
      (
         ch->sex == SEX_FEMALE &&
         IS_IMMORTAL(ch) &&
         !IS_NPC(ch)
      )
      {
         act
         (
            "$n wears $p on $s bosom.",
            ch,
            obj,
            NULL,
            TO_ROOM
         );
         act
         (
            "You wear $p on your bosom.",
            ch,
            obj,
            NULL,
            TO_CHAR
         );
         equip_char(ch, obj, WEAR_BOSOM);
         return TRUE;
      }
      send_to_char("You cannot wear this for some inexplicable reason.\n\r", ch);
      return FALSE;
   }



   /* ====== DUAL WIELD STUFF STARTS HERE ======= */
   /* Dual wielding and limiting to 2 hands for objects */

   /*
      Light code moved before 'held' because many lights were incorrectly
      given the 'hold' flag
   */
   /* Lots of dual wield stuff now */
   if (obj->item_type == ITEM_LIGHT)
   {
      if ((get_eq_char(ch, WEAR_LIGHT) == NULL) && (hands_full(ch)) )
      {
         if (!fReplace)
         return FALSE;
         else
         {
            send_to_char("Your hands are full.\n\r", ch);
            return FALSE;
         }
      }
      if (!remove_obj(ch, WEAR_LIGHT, fReplace, TRUE))
      return FALSE;
      weapon = get_eq_char(ch, WEAR_WIELD);
      if (weapon != NULL && ((ch->size < SIZE_LARGE
      &&IS_WEAPON_STAT(weapon, WEAPON_TWO_HANDS))
      ||weapon->value[0]==WEAPON_BOW))
      {
         send_to_char("Your hands are tied up with your weapon.\n\r", ch);
         return FALSE;
      }
      if (!book)
      {
         act
         (
            "$n lights $p and holds it.",
            ch,
            obj,
            NULL,
            TO_ROOM
         );
         act
         (
            "You light $p and hold it.",
            ch,
            obj,
            NULL,
            TO_CHAR
         );
      }
      else
      {
         act
         (
            "$n lights $p and uses it as a bookmark.",
            ch,
            obj,
            NULL,
            TO_ROOM
         );
         act
         (
            "You light $p and use it as a bookmark.",
            ch,
            obj,
            NULL,
            TO_CHAR
         );
      }
      equip_char(ch, obj, WEAR_LIGHT);
      return TRUE;
   }

   if (CAN_WEAR(obj, ITEM_HOLD))
   {
      if ((get_eq_char(ch, WEAR_HOLD) == NULL) && (hands_full(ch)) )
      {
         if (!fReplace)
         return FALSE;
         else
         {
            send_to_char("Your hands are full.\n\r", ch);
            return FALSE;
         }
      }
      if (!remove_obj(ch, WEAR_HOLD, fReplace, TRUE))
      return FALSE;
      weapon = get_eq_char(ch, WEAR_WIELD);
      if (weapon != NULL && ((ch->size < SIZE_LARGE
      && IS_WEAPON_STAT(weapon, WEAPON_TWO_HANDS))
      ||weapon->value[0]==WEAPON_BOW))
      {
         send_to_char("Your hands are tied up with your weapon.\n\r", ch);
         return FALSE;
      }

      if (!book)
      {
         act("$n holds $p in $s hands.", ch, obj, NULL, TO_ROOM);
         act("You hold $p in your hands.", ch, obj, NULL, TO_CHAR);
      }
      else
      {
         act("$n holds $p in $s tassles.", ch, obj, NULL, TO_ROOM);
         act("You hold $p in your tassles.", ch, obj, NULL, TO_CHAR);
      }
      equip_char(ch, obj, WEAR_HOLD);
      if ( obj->pIndexData->vnum == OBJ_VNUM_SUMMONING_STONE )
      hold_summoning_stone( ch, obj );
      return TRUE;
   }

   if (CAN_WEAR(obj, ITEM_WEAR_SHIELD))
   {
      if ((get_eq_char(ch, WEAR_SHIELD) == NULL) && (hands_full(ch)) )
      {
         if (!fReplace)
         return FALSE;
         else
         {
            send_to_char("Your hands are full.\n\r", ch);
            return FALSE;
         }
      }
      if (!remove_obj(ch, WEAR_SHIELD, fReplace, TRUE))
      return FALSE;
      weapon = get_eq_char(ch, WEAR_WIELD);
      if (weapon != NULL && ((ch->size < SIZE_LARGE
      && IS_WEAPON_STAT(weapon, WEAPON_TWO_HANDS))
      || weapon->value[0]== WEAPON_BOW))
      {
         send_to_char("Your hands are tied up with your weapon.\n\r", ch);
         return FALSE;
      }
      if (!book)
      {
         act("$n wears $p as a shield.", ch, obj, NULL, TO_ROOM);
         act("You wear $p as a shield.", ch, obj, NULL, TO_CHAR);
      }
      else
      {
         act("$n slings $p over $s rear cover.", ch, obj, NULL, TO_ROOM);
         act("You sling $p over your rear cover.", ch, obj, NULL, TO_CHAR);
      }
      equip_char(ch, obj, WEAR_SHIELD);
      return TRUE;
   }

   if (CAN_WEAR(obj, ITEM_WIELD))
   {
      bool dual = FALSE;

      if (obj->item_type != ITEM_WEAPON)
      {
         send_to_char("That is not a weapon!\n\r", ch);
         return FALSE;
      }
      switch (obj->value[0])
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
         case(WEAPON_BOW):        sn = gsn_archery;        break;
         case(WEAPON_STAFF):        sn = gsn_staff;        break;
      }
      skill = get_weapon_skill(ch, sn);
      if (can_wield_tertiary(ch, obj, fReplace))
      {
         return FALSE;
      }
      primary = get_eq_char(ch, WEAR_WIELD);
      secondary = get_eq_char(ch, WEAR_DUAL_WIELD);
      if
      (
         primary == NULL &&
         hands_full(ch)
      )
      {
         if (!fReplace)
         {
            return FALSE;
         }
         else
         {
            send_to_char("Your hands are full.\n\r", ch);
            return FALSE;
         }
      }
      if
      (
         primary != NULL &&
         (
            IS_NPC(ch) ||
            (
               get_skill(ch, gsn_dual_wield) > 0 &&
               has_skill(ch, gsn_dual_wield)
            )
         )
      )
      {
         dual = TRUE;
      }
      if
      (
         dual &&
         (
            (
               ch->size < SIZE_LARGE &&
               (
                  (
                     primary != NULL &&
                     IS_WEAPON_STAT(primary, WEAPON_TWO_HANDS)
                  ) ||
                  IS_WEAPON_STAT(obj, WEAPON_TWO_HANDS)
               )
            ) ||
            obj->value[0] == WEAPON_BOW ||
            (
               get_eq_char(ch, WEAR_SHIELD) != NULL ||
               get_eq_char(ch, WEAR_HOLD) != NULL ||
               get_eq_char(ch, WEAR_LIGHT) != NULL
            )
         )
      )
      {
         dual = FALSE;
      }
      if
      (
         dual &&
         (
            obj->weight < primary->weight * 75 / 100 ||
            obj->weight < 30
         ) &&
         (
            secondary == NULL ||
            remove_obj(ch, WEAR_DUAL_WIELD, fReplace, TRUE)
         ) &&
         primary->value[0] != WEAPON_BOW
      )
      {
         act("$n dual wields $p.", ch, obj, NULL, TO_ROOM);
         act("You dual wield $p.", ch, obj, NULL, TO_CHAR);
         equip_char(ch, obj, WEAR_DUAL_WIELD);
      }
      else
      {
         /*
            Try primary wield.
            <0> chance for dual wield
         */
         if (!remove_obj(ch, WEAR_WIELD, fReplace, TRUE))
         {
            /* Trying to primary, but cannot remove */
            return FALSE;
         }
         if
         (
            (
               obj->value[0] == WEAPON_BOW ||
               (
                  ch->size < SIZE_LARGE &&
                  (
                     (
                        primary != NULL &&
                        IS_WEAPON_STAT(primary, WEAPON_TWO_HANDS)
                     ) ||
                     IS_WEAPON_STAT(obj, WEAPON_TWO_HANDS)
                  )
               )
            ) &&
            (
               get_eq_char(ch, WEAR_SHIELD) != NULL ||
               get_eq_char(ch, WEAR_HOLD) != NULL ||
               get_eq_char(ch, WEAR_LIGHT) != NULL ||
               (
                  get_eq_char(ch, WEAR_WIELD) != NULL &&
                  !remove_obj(ch, WEAR_WIELD, fReplace, TRUE)
               )
            )
         )
         {
            /* Need two hands, and do not have them */
            send_to_char
            (
               "You need both hands free for that weapon.\n\r",
               ch
            );
            reslot_weapon(ch);
            return FALSE;
         }
         if
         (
            secondary != NULL &&
            secondary->wear_loc == WEAR_NONE
         )
         {
            secondary = NULL;
         }
         if
         (
            secondary == NULL ||
            (
               secondary->weight < obj->weight * 75 / 100 ||
               secondary->weight < 30
            )
         )
         {
            /* Try primal wield */
            if
            (
               get_obj_weight(obj) >
               (
                  str_app[get_curr_stat(ch, STAT_STR)].wield * 10
               )
            )
            {
               send_to_char("It is too heavy for you to wield.\n\r", ch);
               reslot_weapon(ch);
               return FALSE;
            }
            if (obj->pIndexData->vnum == OBJ_VNUM_GENOCIDE_AXE)
            {
               act("A dark cloud envelopes $n. When it fades a Black Knight is standing in $s place.", ch, NULL, NULL, TO_ROOM);
               send_to_char("You feel a cloud of darkness come over you!\n\r", ch);
            }
            if (!book)
            {
               act("$n wields $p.", ch, obj, NULL, TO_ROOM);
               act("You wield $p.", ch, obj, NULL, TO_CHAR);
            }
            else
            {
               act("$n grasps $p with $s tassles.", ch, obj, NULL, TO_ROOM);
               act("You grasp $p with your tassles.", ch, obj, NULL, TO_CHAR);
            }
            if (secondary)
            {
               unequip_char(ch, secondary);
            }
            equip_char(ch, obj, WEAR_WIELD);
            if (secondary)
            {
               equip_char(ch, secondary, WEAR_DUAL_WIELD);
            }
         }
         else
         {
            act("$p is too heavy to dual wield.", ch, obj, NULL, TO_CHAR);
            reslot_weapon(ch);
            return FALSE;
         }
      }
      if
      (
         get_eq_char(ch, WEAR_WIELD) == obj ||
         get_eq_char(ch, WEAR_DUAL_WIELD) == obj
      )
      {
         char* msg;
         if (skill >= 100)
         {
            msg = "$p feels like a part of you!";
         }
         else if (skill > 85)
         {
            msg = "You feel quite confident with $p.";
         }
         else if (skill > 70)
         {
            msg = "You are skilled with $p.";
         }
         else if (skill > 50)
         {
            msg = "Your skill with $p is adequate.";
         }
         else if (skill > 25)
         {
            msg = "$p feels a little clumsy in your hands.";
         }
         else if (skill > 1)
         {
            msg = "You fumble and almost drop $p.";
         }
         else
         {
            msg = "You do not even know which end is up on $p.";
         }
         act(msg, ch, obj, NULL, TO_CHAR);
         return TRUE;
      }
      return FALSE;
   }
   /* ==== END DUAL_WIELD CODE ======= */
   if ( CAN_WEAR(obj, ITEM_WEAR_BRAND) )
   {
      if (!remove_obj(ch, WEAR_BRAND, fReplace, TRUE) )
      return FALSE;
      act("You wear it.", ch, obj, NULL, TO_CHAR);
      equip_char(ch, obj, WEAR_BRAND);
      return TRUE;
   }

   if ( fReplace )
   send_to_char( "You can't wear, wield, or hold that.\n\r", ch );

   return FALSE;
}

/*
 * Below is some plug-in code for adding equipment sets that
 * give special enhancements.  In this case, wearing a set
 * gives the bearer the armor spell.
 * - Xurinos
 */
/*
void wear_special_set_object_Rings_of_Focus
(
   CHAR_DATA* ch,
   int count
)
{
   if (count != 2) return;

   if (!is_affected(ch, gsn_armor))
   {
      AFFECT_DATA af;
      af.where     = TO_AFFECTS;
      af.type      = gsn_armor;
      af.level     = 51;
      af.duration  = 24;
      af.modifier  = -20;
      af.location  = APPLY_AC;
      af.bitvector = 0;
      affect_to_char(ch, &af);
      send_to_char("You feel someone protecting you.\n\r", ch);
   }
}

int is_special_set_obj_Rings_of_Focus(sh_int vnum)
{
   switch (vnum)
   {
      case OBJ_VNUM_PHYSICAL_FOCUS:
      case OBJ_VNUM_MENTAL_FOCUS:
         return 1;
         break;

      default:
         return 0;
   }
}

int is_special_set_obj(sh_int vnum)
{
   return
      is_special_set_obj_Rings_of_Focus(vnum) ||
      is_special_set_obj_Rings_of_Focus(vnum);
}

void check_special_set_object
(
   CHAR_DATA* ch,
   OBJ_DATA* obj
)
{
   if (is_special_set_obj(obj->pIndexData->vnum))
   {
      OBJ_DATA* ch_obj;
      int count_special_set_obj_Rings_of_Focus = 0;

      for (ch_obj = ch->carrying; ch_obj != NULL; ch_obj = ch_obj->next_content)
         if (ch_obj->wear_loc != WEAR_NONE)
         {
            if (is_special_set_obj_Rings_of_Focus(ch_obj->pIndexData->vnum))
            {
               count_special_set_obj_Rings_of_Focus++;
            }
         }

      wear_special_set_object_Rings_of_Focus(
         ch,
         count_special_set_obj_Rings_of_Focus);
   }
}
*/

void do_wear( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   OBJ_DATA *obj;

   one_argument( argument, arg );

   if ( arg[0] == '\0' )
   {
      send_to_char( "Wear, wield, or hold what?\n\r", ch );
      return;
   }

   if
   (
      IS_NPC(ch) &&
      IS_AFFECTED(ch, AFF_CHARM) &&
      str_cmp(argument, "enfshackles")
   )
   {
      send_to_char("A mob wearing eq? Ha!\n\r", ch);
      return;
   }

   if ( !str_cmp( arg, "all" ) )
   {
      OBJ_DATA *obj_next;
      if (check_shadowstrike(ch, TRUE, TRUE))
      {
         return;
      }
      for ( obj = ch->carrying; obj != NULL; obj = obj_next )
      {
         obj_next = obj->next_content;
         if ( obj->wear_loc == WEAR_NONE && can_see_obj( ch, obj ) )
         {
            /* Only allow Crusaders to wear poncho's of the same level, or ones created by Imm's */
            if ( !IS_NPC(ch)
            && !IS_IMMORTAL(ch)
            && ch->house == HOUSE_CRUSADER
            && obj->pIndexData->vnum == OBJ_VNUM_PONCHO
            && obj->pIndexData->level != ch->level )
            {
               if ( obj->level >= LEVEL_IMMORTAL )
               send_to_char( "You slowly exhale as the poncho stretches and begins to conform to the shape of your body!\n\r", ch );
               else
               if ( obj->level > ch->level )
               {
                  continue;
               }
            }

            /* Only allow full skill/spell masters to wear these rings */
            if (obj->pIndexData->vnum == OBJ_VNUM_PHYSICAL_FOCUS)
            {
               int x;

               if ( !IS_NPC(ch) )
               for (x = 1; x < MAX_SKILL; x++)
               {
                  if (skill_table[x].name == NULL )
                  break;

                  if ( ch->pcdata->learnlvl[x] <= ch->level
                  && skill_table[x].spell_fun == spell_null
                  && ch->pcdata->learned[x] > 0
                  && ch->pcdata->learned[x] < 100 )
                  {
                     continue;
                  }
               }
            }
            if (obj->pIndexData->vnum == OBJ_VNUM_MENTAL_FOCUS)
            {
               int x;

               if ( !IS_NPC(ch) )
               for (x = 1; x < MAX_SKILL; x++)
               {
                  if (skill_table[x].name == NULL )
                  break;

                  if ( ch->pcdata->learnlvl[x] <= ch->level
                  && skill_table[x].spell_fun != spell_null
                  && ch->pcdata->learned[x] > 0
                  && ch->pcdata->learned[x] < 100 )
                  {
                     continue;
                  }
               }
            }

            if
            (
               obj->pIndexData->vnum != OBJ_VNUM_SHACKLES &&
               obj->pIndexData->vnum != OBJ_VNUM_MONK_FLOWERS &&
               obj->pIndexData->vnum != OBJ_VNUM_GENOCIDE_AXE &&
               ch->class == GUILD_MONK - 1
            )
            {
               if
               (
                  !CAN_WEAR(obj, ITEM_WEAR_FINGER) &&
                  !CAN_WEAR(obj, ITEM_WEAR_FEET) &&
                  !CAN_WEAR(obj, ITEM_WEAR_WAIST) &&
                  !CAN_WEAR(obj, ITEM_WEAR_ABOUT) &&
                  !CAN_WEAR(obj, ITEM_WEAR_NECK) &&
                  !CAN_WEAR(obj, ITEM_WEAR_EYES) &&
                  !CAN_WEAR(obj, ITEM_HOLD) &&
                  !CAN_WEAR(obj, ITEM_WEAR_BRAND) &&
                  !CAN_WEAR(obj, ITEM_WEAR_WRIST)
               )
               {
                  continue;
               }
               if (CAN_WEAR(obj, ITEM_WEAR_NECK) && obj->weight > 10)
               {
                  continue;
               }
            }

            wear_obj( ch, obj, FALSE );
            if (obj->wear_loc != WEAR_NONE)
            {
               if (is_pulsing(obj))
               {
                  check_impure(ch, obj->short_descr, IMPURE_WEAR);
               }
               if (obj->pIndexData->vnum == 2927 && !str_cmp(obj->owner, ch->name))
               {
                  send_to_char("The cross on your Breastplate glows blue!\n\r", ch);
                  magic_spell
                  (
                     ch,
                     "blah self",
                     CAST_BITS_PLR_SKILL,
                     obj->level + 9,
                     gsn_bless
                  );
               }

               /*check_special_set_object(ch, obj);*/
               if (obj->pIndexData->vnum == OBJ_VNUM_GENOCIDE_AXE)
               wear_obj_genocide(ch, obj);
               if (obj->pIndexData->vnum == 24418 )
               wear_obj_girdle(ch, obj);
               if (obj->pIndexData->vnum == 16022)
               wear_obj_troll_skin(ch, obj);

               if (obj->pIndexData->vnum == OBJ_VNUM_SWORD_DEMONS)
               wear_obj_sword_demon(ch, obj);

               if (obj->pIndexData->vnum == OBJ_VNUM_SYMBOL_MAGIC)
               wear_obj_symbol_magic(ch, obj);

               if (obj->pIndexData->vnum == OBJ_VNUM_WIZARDRY_1
               || obj->pIndexData->vnum == OBJ_VNUM_WIZARDRY_2)
               wear_obj_wizardry(ch, obj);

               if (obj->pIndexData->vnum == OBJ_VNUM_PHYSICAL_FOCUS)
               wear_obj_physical_focus(ch, obj);

               if (obj->pIndexData->vnum == OBJ_VNUM_MENTAL_FOCUS)
               wear_obj_mental_focus(ch, obj);

               if (obj->pIndexData->vnum == OBJ_VNUM_TIARA)
               wear_obj_tiara(ch, obj);

               if (obj->pIndexData->vnum == 19002)
               wear_obj_19002(ch, obj);

               if (obj->pIndexData->vnum == 15123)
               wear_obj_channeling(ch, obj);

               if (obj->pIndexData->vnum == 19445) /* strange bracers */
               wear_obj_bracers(ch, obj);
               if (obj->pIndexData->vnum == 18792) /* elven_forged light mithril armguards */
               wear_obj_elven_armguards(ch, obj);
               if (obj->pIndexData->vnum == 23751) /* blue dragon hide */
               wear_obj_blue_hide(ch, obj);

               if (obj->pIndexData->vnum == 23752) /* Green dragon hide */
               wear_obj_green_hide(ch, obj);

               if (obj->pIndexData->vnum == 23706) /* Dragonmage feet */
               wear_obj_dragonmage_feet(ch, obj);

               if (obj->pIndexData->vnum == 14005) /* black zhentil robes */
               wear_obj_zhentil_robe(ch, obj);
               if (obj->pIndexData->vnum == 12161) /* white tin helm */
               wear_obj_whitehelm(ch, obj);
               if (obj->pIndexData->vnum == 13711)
               wear_obj_fallen_wings(ch, obj);
               if (obj->pIndexData->vnum == OBJ_VNUM_BLACK_HIDE) /* black dragon hide */
               wear_obj_black_hide(ch, obj);
               if (obj->pIndexData->vnum == OBJ_VNUM_MISTY_ESSENCE) /* misty cloak */
               wear_obj_misty_cloak(ch, obj);
               if (obj->pIndexData->vnum == OBJ_VNUM_CLOAK_UNDERWORLD) /* cloak of underworld */
               wear_obj_cloak_underworld(ch, obj);
               if (obj->pIndexData->vnum == OBJ_VNUM_RED_DRAGONSTAR) /* red dragonstar */
               wear_obj_red_dragonstar(ch, obj);
               if (obj->pIndexData->vnum == OBJ_VNUM_BLACK_DRAGONSTAR) /* black dragonstar */
               wear_obj_black_dragonstar(ch, obj);

               if (obj->pIndexData->vnum == OBJ_VNUM_SHADOW_HIDE) /* shadow dragon hide */
               wear_obj_shadow_hide(ch, obj);

               if (obj->pIndexData->vnum == OBJ_VNUM_RING_PROTECT)
               wear_obj_ring_protection(ch, obj);
               if (obj->pIndexData->vnum == OBJ_VNUM_ROBE_AVENGER)
               wear_obj_robe_avenger(ch, obj);
            }
         }
      }
      return;
   }
   else
   {
      if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
      {
         send_to_char( "You do not have that item.\n\r", ch );
         return;
      }
      if
      (
         str_cmp(argument, "enfshackles") &&
         check_shadowstrike(ch, TRUE, TRUE)
      )
      {
         return;
      }

      /* Only allow Crusaders to wear poncho's of the same level, or ones created by Imm's */
      if ( !IS_NPC(ch)
      && !IS_IMMORTAL(ch)
      && ch->house == HOUSE_CRUSADER
      && obj->pIndexData->vnum == OBJ_VNUM_PONCHO
      && obj->pIndexData->level != ch->level )
      {
         if ( obj->level >= LEVEL_IMMORTAL )
         send_to_char( "You slowly exhale as the poncho stretches and begins to conform to the shape of your body!\n\r", ch );
         else
         if ( obj->level > ch->level )
         {
            send_to_char( "The fit would be too tight due to the detailed craftsmanship of this poncho.\n\r", ch );
            return;
         }
      }

      /* Only allow full skill/spell masters to wear these rings */
      if (obj->pIndexData->vnum == OBJ_VNUM_PHYSICAL_FOCUS)
      {
         int x;

         if ( !IS_NPC(ch) )
         for (x = 1; x < MAX_SKILL; x++)
         {
            if (skill_table[x].name == NULL )
            break;

            if ( ch->pcdata->learnlvl[x] <= ch->level
            && skill_table[x].spell_fun == spell_null
            && ch->pcdata->learned[x] > 0
            && ch->pcdata->learned[x] < 100 )
            {
               act( "The ring resists your attempt to place it upon your finger.", ch, NULL, NULL, TO_CHAR );
               return;
            }
         }
      }
      if (obj->pIndexData->vnum == OBJ_VNUM_MENTAL_FOCUS)
      {
         int x;

         if ( !IS_NPC(ch) )
         for (x = 1; x < MAX_SKILL; x++)
         {
            if (skill_table[x].name == NULL )
            break;

            if ( ch->pcdata->learnlvl[x] <= ch->level
            && skill_table[x].spell_fun != spell_null
            && ch->pcdata->learned[x] > 0
            && ch->pcdata->learned[x] < 100 )
            {
               act( "The ring resists your attempt to place it upon your finger.", ch, NULL, NULL, TO_CHAR );
               return;
            }
         }
      }

      if
      (
         obj->pIndexData->vnum != OBJ_VNUM_SHACKLES &&
         obj->pIndexData->vnum != OBJ_VNUM_MONK_FLOWERS &&
         obj->pIndexData->vnum != OBJ_VNUM_GENOCIDE_AXE &&
         ch->class == GUILD_MONK - 1
      )
      {
         if (!CAN_WEAR( obj, ITEM_WEAR_FINGER) &&
         !CAN_WEAR( obj, ITEM_WEAR_FEET) &&
         !CAN_WEAR( obj, ITEM_WEAR_WAIST) &&
         !CAN_WEAR( obj, ITEM_WEAR_ABOUT) &&
         !CAN_WEAR( obj, ITEM_WEAR_NECK) &&
         !CAN_WEAR( obj, ITEM_WEAR_EYES) &&
         !CAN_WEAR( obj, ITEM_HOLD) &&
         !CAN_WEAR( obj, ITEM_WEAR_BRAND) &&
         !CAN_WEAR( obj, ITEM_WEAR_WRIST))
         {
            send_to_char("Your vows prevent you from wearing such items.\n\r", ch);
            return;
         }
         if (CAN_WEAR(obj, ITEM_WEAR_NECK) && obj->weight > 10)
         {
            send_to_char("Your vows prevent you from wearing such heavy neckwear.\n\r", ch);
            return;
         }
      }

      wear_obj( ch, obj, TRUE );
      if (obj->wear_loc != WEAR_NONE)
      {
         if (is_pulsing(obj))
         {
            check_impure(ch, obj->short_descr, IMPURE_WEAR);
         }

         if (obj->pIndexData->vnum == 2927 && !str_cmp(obj->owner, ch->name))
         {
            send_to_char("The cross on your Breastplate glows blue!\n\r", ch);
            magic_spell
            (
               ch,
               "blah self",
               CAST_BITS_PLR_SKILL,
               obj->level + 9,
               gsn_bless
            );
         }

         /*check_special_set_object(ch, obj);*/

         if (obj->pIndexData->vnum == OBJ_VNUM_SWORD_DEMONS)
         wear_obj_sword_demon(ch, obj);

         if (obj->pIndexData->vnum == OBJ_VNUM_GENOCIDE_AXE)
         wear_obj_genocide(ch, obj);

         if (obj->pIndexData->vnum == 24418)
         wear_obj_girdle(ch, obj);
         if (obj->pIndexData->vnum == 16022)
         wear_obj_troll_skin(ch, obj);

         if (obj->pIndexData->vnum == OBJ_VNUM_SYMBOL_MAGIC)
         wear_obj_symbol_magic(ch, obj);

         if (obj->pIndexData->vnum == OBJ_VNUM_WIZARDRY_1
         || obj->pIndexData->vnum == OBJ_VNUM_WIZARDRY_2)
         wear_obj_wizardry(ch, obj);

         if (obj->pIndexData->vnum == OBJ_VNUM_PHYSICAL_FOCUS)
         wear_obj_physical_focus(ch, obj);

         if (obj->pIndexData->vnum == OBJ_VNUM_MENTAL_FOCUS)
         wear_obj_mental_focus(ch, obj);

         if (obj->pIndexData->vnum == OBJ_VNUM_TIARA)
         wear_obj_tiara(ch, obj);

         if (obj->pIndexData->vnum == 19002)
         wear_obj_19002(ch, obj);

         if (obj->pIndexData->vnum == 15123)
         wear_obj_channeling(ch, obj);

         if (obj->pIndexData->vnum == 19445) /* strange bracers */
         wear_obj_bracers(ch, obj);
         if (obj->pIndexData->vnum == 18792) /* elven_forged light mithril armguards */
         wear_obj_elven_armguards(ch, obj);

         if (obj->pIndexData->vnum == 23751) /* blue dragon hide */
         wear_obj_blue_hide(ch, obj);
         if (obj->pIndexData->vnum == OBJ_VNUM_RED_DRAGONSTAR) /* red dragonstar */
         wear_obj_red_dragonstar(ch, obj);
         if (obj->pIndexData->vnum == OBJ_VNUM_BLACK_DRAGONSTAR) /* black dragonstar */
         wear_obj_black_dragonstar(ch, obj);

         if (obj->pIndexData->vnum == 23752) /* Green dragon hide */
         wear_obj_green_hide(ch, obj);

         if (obj->pIndexData->vnum == 23706) /* Dragonmage feet */
         wear_obj_dragonmage_feet(ch, obj);

         if (obj->pIndexData->vnum == 14005) /* black zhentil robes */
         wear_obj_zhentil_robe(ch, obj);
         if (obj->pIndexData->vnum == 12161) /* white tin helm */
         wear_obj_whitehelm(ch, obj);
         if (obj->pIndexData->vnum == 13711)
         wear_obj_fallen_wings(ch, obj);
         if (obj->pIndexData->vnum == OBJ_VNUM_BLACK_HIDE) /* black dragon hide */
         wear_obj_black_hide(ch, obj);
         if (obj->pIndexData->vnum == OBJ_VNUM_MISTY_ESSENCE) /* misty cloak */
         wear_obj_misty_cloak(ch, obj);
         if (obj->pIndexData->vnum == OBJ_VNUM_CLOAK_UNDERWORLD) /* cloak of underworld */
         wear_obj_cloak_underworld(ch, obj);
         if (obj->pIndexData->vnum == OBJ_VNUM_SHADOW_HIDE) /* shadow dragon hide */
         wear_obj_shadow_hide(ch, obj);
         if (obj->pIndexData->vnum == OBJ_VNUM_RING_PROTECT)
         wear_obj_ring_protection(ch, obj);
         if (obj->pIndexData->vnum == OBJ_VNUM_ROBE_AVENGER)
         wear_obj_robe_avenger(ch, obj);
      }
   }

   return;
}



void do_remove( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   OBJ_DATA *obj;

   one_argument( argument, arg );

   if ( arg[0] == '\0' )
   {
      send_to_char( "Remove what?\n\r", ch );
      return;
   }

   if ( !str_cmp( arg, "all" ) )
   {
      OBJ_DATA *obj_next;

      if (check_shadowstrike(ch, TRUE, TRUE))
      {
         return;
      }
      for ( obj = ch->carrying; obj != NULL; obj = obj_next )
      {
         obj_next = obj->next_content;
         if ( obj->wear_loc != WEAR_NONE
         && can_see_obj( ch, obj )
         && obj->wear_loc != WEAR_BRAND )
         remove_obj( ch, obj->wear_loc, TRUE, TRUE );
         if (obj->wear_loc == WEAR_NONE)
         {
         }
      }
      reslot_weapon(ch);
      return;
   }
   else if ( ( obj = get_obj_wear( ch, arg ) ) == NULL )
   {
      send_to_char( "You do not have that item.\n\r", ch );
      return;
   }
   if (check_shadowstrike(ch, TRUE, TRUE))
   {
      return;
   }

   remove_obj( ch, obj->wear_loc, TRUE, TRUE );
   reslot_weapon(ch);
   if (FALSE) /* (obj->wear_loc == WEAR_NONE) */
   {
      if (obj->pIndexData->vnum == OBJ_VNUM_GENOCIDE_AXE)
      remove_obj_genocide(ch, obj);
      if (obj->pIndexData->vnum == 24418)
      remove_obj_girdle(ch, obj);

      if (obj->pIndexData->vnum == 16022)
      remove_obj_troll_skin(ch, obj);


      if (obj->pIndexData->vnum == OBJ_VNUM_SYMBOL_MAGIC)
      remove_obj_symbol_magic(ch, obj);

      if (obj->pIndexData->vnum == 15123)
      remove_obj_channeling(ch, obj);

      if (obj->pIndexData->vnum == 19002)
      remove_obj_19002(ch, obj);
      if (obj->pIndexData->vnum == 19445)
      remove_obj_bracers(ch, obj);

      if (obj->pIndexData->vnum == 23706)
      remove_obj_dragonmage_feet(ch, obj);
      if (obj->pIndexData->vnum == 23751)
      remove_obj_blue_hide(ch, obj);
      if (obj->pIndexData->vnum == 23752)
      remove_obj_green_hide(ch, obj);
      if (obj->pIndexData->vnum == OBJ_VNUM_WIZARDRY_1
      || obj->pIndexData->vnum == OBJ_VNUM_WIZARDRY_2)
      remove_obj_wizardry(ch, obj);

      if (obj->pIndexData->vnum == OBJ_VNUM_TIARA)
      remove_obj_tiara(ch, obj);
      if (obj->pIndexData->vnum == OBJ_VNUM_HEAVENLY_SCEPTRE)
      remove_obj_sceptre(ch, obj);
      if (obj->pIndexData->vnum == 12161) /* white tin helm */
      remove_obj_whitehelm(ch, obj);

      if (obj->pIndexData->vnum == 13711)
      remove_obj_fallen_wings(ch, obj);
      if (obj->pIndexData->vnum == OBJ_VNUM_BLACK_HIDE)        /* black dragon hide */
      remove_obj_black_hide(ch, obj);
      if (obj->pIndexData->vnum == OBJ_VNUM_MISTY_ESSENCE) /* misty cloak */
      remove_obj_misty_cloak(ch, obj);
      if (obj->pIndexData->vnum == OBJ_VNUM_CLOAK_UNDERWORLD)        /* cloak of underworld */
      remove_obj_cloak_underworld(ch, obj);
      if (obj->pIndexData->vnum == OBJ_VNUM_RED_DRAGONSTAR)        /* red dragonstar */
      remove_obj_red_dragonstar(ch, obj);
      if (obj->pIndexData->vnum == OBJ_VNUM_BLACK_DRAGONSTAR)        /* black dragonstar */
      remove_obj_black_dragonstar(ch, obj);

      if (obj->pIndexData->vnum == OBJ_VNUM_SHADOW_HIDE)        /* shadow dragon hide */
      remove_obj_shadow_hide(ch, obj);
      if (obj->pIndexData->vnum == OBJ_VNUM_RING_PROTECT)
      remove_obj_ring_protection(ch, obj); else
      if (obj->pIndexData->vnum == OBJ_VNUM_ROBE_AVENGER)
      remove_obj_robe_avenger(ch, obj);

   }
   return;
}

void destroy_chaos_potions(CHAR_DATA* ch)
{
   OBJ_DATA* obj;
   OBJ_DATA* obj_next;

   for (obj = ch->carrying; obj; obj = obj_next)
   {
      obj_next = obj->next_content;

      if (
            (obj->pIndexData->vnum == OBJ_VNUM_GOOD_CHAOS_POTION) ||
            (obj->pIndexData->vnum == OBJ_VNUM_BAD_CHAOS_POTION) ||
            (obj->pIndexData->vnum == OBJ_VNUM_NEUTRAL_CHAOS_POTION)
         )
      {
         act(
            "$p has dissolved into nothingness.",
            ch,
            obj,
            NULL,
            TO_CHAR);
         act(
            "$p has dissolved into nothingness.",
            ch,
            obj,
            NULL,
            TO_ROOM);
         extract_obj(obj, FALSE);
      }
   }
}

void make_chaos_brand_maybe(CHAR_DATA* ch, OBJ_DATA* obj)
{
   if (IS_IMMORTAL(ch) || IS_NPC(ch))
   {
      send_to_char("Nothing happens.  You get the feeling this potion is not for you.\n\r", ch);
      destroy_chaos_potions(ch);
      return;
   }

   switch (obj->pIndexData->vnum)
   {
      case OBJ_VNUM_GOOD_CHAOS_POTION:
         act(
            "$n quivers for a moment as $p\n\r"
            "dissolves into nothingness.  $n is surrounded by a shower of glowing\n\r"
            "sparks that also fade into nothingness.  $n has chosen...wisely.",
            ch,
            obj,
            NULL,
            TO_ROOM);
         act(
            "You quiver as the potion begins to take hold of you.  Your vision grows hazy,\n\r"
            "and then snaps back into perfect clarity.  Time flows in all directions about\n\r"
            "you, and $p dissolves into nothingness.\n\r"
            "You are surrounded by a shower of glowing sparks that also fade into\n\r"
            "nothingness.  You have chosen...wisely.",
            ch,
            obj,
            NULL,
            TO_CHAR);

         destroy_chaos_potions(ch);
         break;

      case OBJ_VNUM_BAD_CHAOS_POTION:
         act(
            "$n quivers for a moment as $p\n\r"
            "dissolves into nothingness.  The sound of buzzing bees increases in volume,\n\r"
            "and a black cloud surrounds $n.  In an instant, $n is torn\n\r"
            "into bloody shreds, and the black cloud implodes in $s wake.  $n has\n\r"
            "chosen...unwisely.",
            ch,
            obj,
            NULL,
            TO_ROOM);
         act(
            "You quiver for a moment as $p dissolves\n\r"
            "into nothingness.  The sound of buzzing bees increases in volume, and a black\n\r"
            "cloud surrounds you!  In an instant, you are torn into bloody shreds,\n\r"
            "accompanied by the sound of maniacal laughter coming from dozens of echoing\n\r"
            "voices.  You have chosen...unwisely.",
            ch,
            obj,
            NULL,
            TO_CHAR);

         destroy_chaos_potions(ch);
         raw_kill(ch, ch);
         break;

      case OBJ_VNUM_NEUTRAL_CHAOS_POTION:
         act(
            "$n quivers for a moment as $p\n\r"
            "dissolves into nothingness.  Nothing happens.",
            ch,
            obj,
            NULL,
            TO_ROOM);
         act(
            "You quiver as the potion begins to take hold of you.  Your vision grows hazy,\n\r"
            "and then snaps back into perfect clarity.  Time flows in all directions about\n\r"
            "you, and $p dissolves into nothingness.\n\r"
            "You realize that nothing has happened.  Wrong potion: Try again.",
            ch,
            obj,
            NULL,
            TO_CHAR);
         extract_obj(obj, FALSE);
   }
}

void do_quaff( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   OBJ_DATA *obj;

   one_argument( argument, arg );

   if ( arg[0] == '\0' )
   {
      send_to_char( "Quaff what?\n\r", ch );
      return;
   }

   if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
   {
      send_to_char( "You do not have that potion.\n\r", ch );
      return;
   }

   if ( obj->item_type != ITEM_POTION )
   {
      send_to_char( "You can quaff only potions.\n\r", ch );
      return;
   }

   if ( ch->fighting != NULL )
   {
      send_to_char( "You're too busy fighting to quaff.\n\r", ch );
      return;
   }

   if (
         (ch->house == 4) &&
         (obj->pIndexData->vnum != OBJ_VNUM_GOOD_CHAOS_POTION) &&
         (obj->pIndexData->vnum != OBJ_VNUM_BAD_CHAOS_POTION) &&
         (obj->pIndexData->vnum != OBJ_VNUM_NEUTRAL_CHAOS_POTION)
      )
   {
      send_to_char("You are a Crusader, you don't use such things as potions!\n\r", ch);
      return;
   }

   if (IS_SET(ch->act2, PLR_LICH))
   {
      send_to_char( "Ingested substances are of no use to your undead metabolism.\n\r", ch);
      return;
   }

   if (is_affected(ch, gsn_nausea))
   {
      send_to_char("You are far too nauseated to drink anything.\n\r", ch);
      return;
   }

   act("$n quaffs $p.", ch, obj, NULL, TO_ROOM);
   act("You quaff $p.", ch, obj, NULL , TO_CHAR);

   switch (obj->pIndexData->vnum)
   {
      case OBJ_VNUM_LICHPOTION:
         make_lich(ch, obj->level);
         extract_obj(obj, FALSE);
         break;

      case OBJ_VNUM_GOOD_CHAOS_POTION:
      case OBJ_VNUM_BAD_CHAOS_POTION:
      case OBJ_VNUM_NEUTRAL_CHAOS_POTION:
         make_chaos_brand_maybe(ch, obj);
         break;

      default:
         check_impure(ch, obj->short_descr, IMPURE_QUAFF);
         obj_cast_spell(obj->value[1], obj->value[0], ch, ch, NULL);
         if (ch->in_room != NULL)
         {
            obj_cast_spell(obj->value[2], obj->value[0], ch, ch, NULL);
         }
         if (ch->in_room != NULL)
         {
            obj_cast_spell(obj->value[3], obj->value[0], ch, ch, NULL);
         }
         if (ch->in_room != NULL)
         {
            obj_cast_spell(obj->value[4], obj->value[0], ch, ch, NULL);
         }
         if (obj->pIndexData->vnum != 12505) /* endless flask of recall for riallus's quest */
         {
            extract_obj(obj, FALSE);
         }
   }

   return;
}



void do_recite(CHAR_DATA* ch, char* argument)
{
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   CHAR_DATA* victim;
   OBJ_DATA* scroll;
   OBJ_DATA* obj;

   if (check_peace(ch))
   {
      return;
   }

   argument = one_argument(argument, arg1);
   argument = one_argument(argument, arg2);

   if (ch->house == HOUSE_CRUSADER)
   {
      send_to_char
      (
         "You are a Crusader, you do not use such things as scrolls!\n\r",
         ch
      );
      return;
   }

   if ((scroll = get_obj_carry(ch, arg1, ch)) == NULL)
   {
      send_to_char("You do not have that scroll.\n\r", ch);
      return;
   }

   if (scroll->item_type != ITEM_SCROLL)
   {
      send_to_char("You can recite only scrolls.\n\r", ch);
      return;
   }

   if (ch->level < scroll->level)
   {
      send_to_char
      (
         "This scroll is too complex for you to comprehend.\n\r",
         ch
      );
      return;
   }

   if (ch->fighting != NULL)
   {
      send_to_char("You are too busy fighting to recite.\n\r", ch);
      return;
   }
   obj = NULL;
   if (arg2[0] == '\0')
   {
      victim = ch;
   }
   else
   {
      if
      (
         (
            victim = get_char_room(ch, arg2)
         ) == NULL &&
         (
            obj = get_obj_here(ch, arg2)
         ) == NULL
      )
      {
         send_to_char("You cannot find it.\n\r", ch);
         return;
      }
   }

   act("$n recites $p.", ch, scroll, NULL, TO_ROOM);
   act("You recite $p.", ch, scroll, NULL, TO_CHAR);

   WAIT_STATE(ch, PULSE_VIOLENCE);

   if
   (
      number_percent() >=
      (
         20 + get_skill(ch, gsn_corrupt("scrolls", &gsn_scrolls)) * 4 / 5
      ) ||
      (
         /* Only conclave/immortals can recite conclave scrolls */
         scroll->pIndexData->vnum == OBJ_VNUM_CONCLAVE_SCROLL &&
         ch->house != HOUSE_CONCLAVE &&
         !IS_IMMORTAL(ch)
      )
   )
   {
      send_to_char("You mispronounce a syllable.\n\r", ch);
      check_improve(ch, gsn_corrupt("scrolls", &gsn_scrolls), FALSE, 2);
      if (scroll->pIndexData->vnum == OBJ_VNUM_LIBRAM_INFINITE)
      {
         check_impure(ch, scroll->short_descr, IMPURE_RECITE);
         recite_libram_conjuration(ch, victim, scroll, obj, FALSE);
         return;
      }
      extract_obj(scroll, FALSE);
      return;
   }
   check_improve(ch, gsn_corrupt("scrolls", &gsn_scrolls), TRUE, 2);
   if (scroll->pIndexData->vnum == OBJ_VNUM_LIBRAM_INFINITE)
   {
      check_impure(ch, scroll->short_descr, IMPURE_RECITE);
      recite_libram_conjuration(ch, victim, scroll, obj, TRUE);
      return;
   }
   if (obj == NULL && oblivion_blink(ch, victim))
   {
      /* Blink completely out from the reciting */
      extract_obj(scroll, FALSE);
      return;
   }
   check_impure(ch, scroll->short_descr, IMPURE_RECITE);
   if
   (
      victim == NULL ||
      (
         !spellbaned_cast(ch, victim, scroll->value[1]) &&
         !check_absorb_cast(ch, victim, scroll->value[1])
      )
   )
   {
      obj_cast_spell(scroll->value[1], scroll->value[0], ch, victim, obj);
   }
   if
   (
      (
         obj == NULL &&
         victim != NULL &&
         victim != ch &&
         victim != get_char_room(ch, arg2)
      ) ||
      (
         victim == NULL &&
         obj != NULL &&
         obj != get_obj_here(ch, arg2)
      ) ||
      ch->in_room == NULL
   )
   {
      extract_obj(scroll, FALSE);
      return;
   }
   /* check blink for a single spell */
   if
   (
      (
         obj != NULL ||
         !oblivion_blink(ch, victim)
      ) &&
      (
         victim == NULL ||
         (
            !spellbaned_cast(ch, victim, scroll->value[2]) &&
            !check_absorb_cast(ch, victim, scroll->value[2])
         )
      )
   )
   {
      obj_cast_spell(scroll->value[2], scroll->value[0], ch, victim, obj);
   }
   if
   (
      (
         obj == NULL &&
         victim != NULL &&
         victim != ch &&
         victim != get_char_room(ch, arg2)
      ) ||
      (
         victim == NULL &&
         obj != NULL &&
         obj != get_obj_here(ch, arg2)
      ) ||
      ch->in_room == NULL
   )
   {
      extract_obj(scroll, FALSE);
      return;
   }
   /* check blink for a single spell */
   if
   (
      (
         obj != NULL ||
         !oblivion_blink(ch, victim)
      ) &&
      (
         victim == NULL ||
         (
            !spellbaned_cast(ch, victim, scroll->value[3]) &&
            !check_absorb_cast(ch, victim, scroll->value[3])
         )
      )
   )
   {
      obj_cast_spell(scroll->value[3], scroll->value[0], ch, victim, obj);
   }
   if
   (
      (
         obj == NULL &&
         victim != NULL &&
         victim != ch &&
         victim != get_char_room(ch, arg2)
      ) ||
      (
         victim == NULL &&
         obj != NULL &&
         obj != get_obj_here(ch, arg2)
      ) ||
      ch->in_room == NULL
   )
   {
      extract_obj(scroll, FALSE);
      return;
   }
   /* check blink for a single spell */
   if
   (
      (
         obj != NULL ||
         !oblivion_blink(ch, victim)
      ) &&
      (
         victim == NULL ||
         (
            !spellbaned_cast(ch, victim, scroll->value[4]) &&
            !check_absorb_cast(ch, victim, scroll->value[4])
         )
      )
   )
   {
      obj_cast_spell(scroll->value[4], scroll->value[0], ch, victim, obj);
   }

   extract_obj(scroll, FALSE);
   return;
}



void do_brandish( CHAR_DATA *ch, char *argument )
{
   CHAR_DATA *vch;
   CHAR_DATA *vch_next;
   OBJ_DATA *staff;
   int sn;
   int llevel;

   if (check_peace(ch)) return;


   if ( ( staff = get_eq_char( ch, WEAR_HOLD ) ) == NULL )
   {
      send_to_char( "You hold nothing in your hand.\n\r", ch );
      return;
   }

   if (staff->pIndexData->vnum == OBJ_VNUM_SCEPTRE_DEAD)
   {
      check_impure(ch, staff->short_descr, IMPURE_BRANDISH);
      brandish_sceptre_dead(ch, staff);
      return;
   }

   if (staff->pIndexData->vnum == OBJ_VNUM_SOUL_ORB)
   {
      check_impure(ch, staff->short_descr, IMPURE_BRANDISH);
      brandish_soul_orb(ch, staff);
      return;
   }
   if (staff->pIndexData->vnum == 29108)
   {
      check_impure(ch, staff->short_descr, IMPURE_BRANDISH);
      brandish_staff_fectus(ch, staff);
      return;
   }

   if (staff->pIndexData->vnum == OBJ_VNUM_HORN_VALERE)
   {
      check_impure(ch, staff->short_descr, IMPURE_BRANDISH);
      obj_brandish_horn_valere(ch, staff);
      return;
   }
   if (ch->house == 4)
   {
      send_to_char("You are a Crusader, you don't use such things as staves!\n\r", ch);
      return;
   }

   if ( staff->item_type != ITEM_STAFF )
   {
      send_to_char( "You can brandish only with a staff.\n\r", ch );
      return;
   }

   if ( ( sn = staff->value[3] ) < 1
   ||   sn >= MAX_SKILL
   ||   skill_table[sn].spell_fun == 0 )
   {
      bug( "Do_brandish: bad sn %d.", sn );
      return;
   }

   WAIT_STATE( ch, 2 * PULSE_VIOLENCE );

   if ( staff->value[2] > 0 )
   {
      act( "$n brandishes $p.", ch, staff, NULL, TO_ROOM );
      act( "You brandish $p.",  ch, staff, NULL, TO_CHAR );
      check_impure(ch, staff->short_descr, IMPURE_BRANDISH);

      llevel = ch->level;
      if ((!IS_NPC(ch)) && (IS_SET(ch->act2, PLR_LICH)))
      llevel = ch->level+( (ch->played)/(3600*150) );

      if ( llevel < staff->level
      ||   number_percent() >= 20 + get_skill(ch, gsn_staves) * 4/5)
      {
         act ("You fail to invoke $p.", ch, staff, NULL, TO_CHAR);
         act ("...and nothing happens.", ch, NULL, NULL, TO_ROOM);
         check_improve(ch, gsn_staves, FALSE, 2);
      }

      else for ( vch = ch->in_room->people; vch; vch = vch_next )
      {
         vch_next        = vch->next_in_room;

         if (IS_IMMORTAL(vch) && !can_see(ch, vch)) continue;

         switch ( skill_table[sn].target )
         {
            default:
            bug( "Do_brandish: bad target for sn %d.", sn );
            return;

            case TAR_IGNORE:
            if ( vch != ch )
            continue;
            break;

            case TAR_CHAR_OFFENSIVE:
            if ( IS_NPC(ch) ? IS_NPC(vch) : !IS_NPC(vch) )
            continue;
            if (oblivion_blink(ch, vch))
            {
               continue;
            }
            break;

            case TAR_CHAR_HEALING:
            case TAR_CHAR_DEFENSIVE:
            if ( IS_NPC(ch) ? !IS_NPC(vch) : IS_NPC(vch) )
            continue;
            if (oblivion_blink(ch, vch))
            {
               continue;
            }
            break;

            case TAR_CHAR_SELF:
            if ( vch != ch )
            continue;
            break;
         }
         check_improve(ch, gsn_staves, TRUE, 2);
         if
         (
            vch != ch &&
            (
               spellbaned_cast(ch, vch, staff->value[3]) ||
               check_absorb_cast(ch, vch, staff->value[3])
            )
         )
         {
            continue;
         }
         obj_cast_spell( staff->value[3], staff->value[0], ch, vch, NULL );
      }
   }

   if ( --staff->value[2] <= 0 )
   {
      act( "$n's $p blazes brightly and is gone.", ch, staff, NULL, TO_ROOM );
      act( "Your $p blazes brightly and is gone.", ch, staff, NULL, TO_CHAR );
      extract_obj( staff, FALSE );
   }

   return;
}



void do_zap( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   OBJ_DATA *wand;
   OBJ_DATA *obj = NULL;
   int llevel;

   if (check_peace(ch)) return;

   one_argument( argument, arg );
   if (ch->house == 4)
   {
      send_to_char("You are a Crusader, you don't use such things as wands!\n\r", ch);
      return;
   }

   if ( arg[0] == '\0' && ch->fighting == NULL )
   {
      send_to_char( "Zap whom or what?\n\r", ch );
      return;
   }

   if ( ( wand = get_eq_char( ch, WEAR_HOLD ) ) == NULL )
   {
      send_to_char( "You hold nothing in your hand.\n\r", ch );
      return;
   }

   if ( (wand->item_type != ITEM_WAND) && (wand->pIndexData->vnum != 1891))
   {
      send_to_char( "You can zap only with a wand.\n\r", ch );
      return;
   }

   obj = NULL;
   if ( arg[0] == '\0' )
   {
      if ( ch->fighting != NULL )
      {
         victim = ch->fighting;
      }
      else
      {
         send_to_char( "Zap whom or what?\n\r", ch );
         return;
      }
   }
   else
   {
      if ( ( victim = get_char_room ( ch, arg ) ) == NULL
      &&   ( obj    = get_obj_here  ( ch, arg ) ) == NULL )
      {
         send_to_char( "You can't find it.\n\r", ch );
         return;
      }
   }

   WAIT_STATE( ch, 2 * PULSE_VIOLENCE );

   if ( (wand->value[2] > 0) || (wand->pIndexData->vnum == 1891) )
   {
      if ( victim != NULL )
      {
         act( "$n zaps $N with $p.", ch, wand, victim, TO_NOTVICT );
         act( "You zap $N with $p.", ch, wand, victim, TO_CHAR );
         act( "$n zaps you with $p.", ch, wand, victim, TO_VICT );
      }
      else
      {
         act( "$n zaps $P with $p.", ch, wand, obj, TO_ROOM );
         act( "You zap $P with $p.", ch, wand, obj, TO_CHAR );
      }

      check_impure(ch, wand->short_descr, IMPURE_ZAP);
      llevel = ch->level;
      if ((!IS_NPC(ch)) && (IS_SET(ch->act2, PLR_LICH)))
      llevel = ch->level+( (ch->played)/(3600*150) );
      if (((llevel < wand->level)
      ||  (number_percent() >= 20 + get_skill(ch, gsn_wands) * 4/5))
      && (wand->pIndexData->vnum != 1891))
      {
         act( "Your efforts with $p produce only smoke and sparks.",
         ch, wand, NULL, TO_CHAR);
         act( "$n's efforts with $p produce only smoke and sparks.",
         ch, wand, NULL, TO_ROOM);
         check_improve(ch, gsn_wands, FALSE, 2);
      }
      else
      {
         if (wand->pIndexData->vnum != 1891)
         {
            check_improve(ch, gsn_wands, TRUE, 2);
            if
            (
               victim == NULL ||
               (
                  !oblivion_blink(ch, victim) &&
                  (
                     !spellbaned_cast(ch, victim, wand->value[3]) &&
                     !check_absorb_cast(ch, victim, wand->value[3])
                  )
               )
            )
            {
               obj_cast_spell(wand->value[3], wand->value[0], ch, victim, obj);
            }
         }
         else  /* This is the caduceus */
         if (victim != NULL)
         {
            char buf[MAX_STRING_LENGTH];
            AFFECT_DATA *paf, *paf_last = NULL;
            char time_string[MAX_INPUT_LENGTH];
            char affect_format[MAX_INPUT_LENGTH];
            char space_format[MAX_INPUT_LENGTH];
            unsigned int max_length = 0;


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

            if (victim->affected != NULL)
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
                  paf_last = paf;
               }
            }
            else
            {
               act("$E is not affected by anything.", ch, NULL, victim, TO_CHAR);
            }
            if (!IS_NPC(victim))
            {
               sprintf( buf,
               "They have %d/%d hit, %d/%d mana, %d/%d movement.\n\r",
               victim->hit,  victim->max_hit,
               victim->mana, victim->max_mana,
               victim->move, victim->max_move);
               send_to_char( buf, ch );
            }
            return;
         }
      }
   }

   if ( --wand->value[2] <= 0 )
   {
      act( "$n's $p explodes into fragments.", ch, wand, NULL, TO_ROOM );
      act( "Your $p explodes into fragments.", ch, wand, NULL, TO_CHAR );
      extract_obj( wand, FALSE );
   }

   return;
}

/*
void do_offer( CHAR_DATA *ch, char *argument )
{
char buf  [MAX_STRING_LENGTH];
char arg1 [MAX_INPUT_LENGTH];
char arg2 [MAX_INPUT_LENGTH];
CHAR_DATA *victim;
OBJ_DATA *obj;
AFFECT_DATA af;
int percent;
int chance;

if (get_skill(ch, gsn_offer) <= 0 ||
!has_skill(ch, gsn_offer)) {
send_to_char("Huh?\n\r", ch);
return;
}

if (house_down(ch, HOUSE_OUTLAW))
return;

if (check_peace(ch)) return;

if( !IS_IMMORTAL(ch) )
return;

argument = one_argument( argument, arg1 );
argument = one_argument( argument, arg2 );

if ( arg1[0] == '\0' || arg2[0] == '\0' )
{
send_to_char( "Offer freedom to whom for what?\n\r", ch );
return;
}

if ( ( victim = get_char_room( ch, arg2 ) ) == NULL )
{
send_to_char( "They aren't here.\n\r", ch );
return;
}

if (is_safe(ch, victim))
return;

if ( victim == ch )
{
send_to_char( "You can't offer yourself freedom!\n\r", ch );
return;
}

if (victim->fighting != NULL)
{
send_to_char("Perhaps they would listen to you if they were not fighting.\n\r", ch);
return;
}

if( is_affected(victim, gsn_offer) )
{
send_to_char("They have recently experienced your kind of freedom.\n\r", ch);
return;
}

if (!IS_AWAKE(victim))
{
send_to_char("It's pretty tough to persuade a sleeping victim...\n\r", ch);
return;
}

un_camouflage(ch, NULL);
un_hide(ch, NULL);
un_earthfade(ch, NULL);
un_forest_blend(ch);

if (!IS_NPC(ch) && !IS_IMMORTAL(ch) && !IS_IMMORTAL(victim))
{
if(!IS_NPC(victim) || IS_AFFECTED(victim, AFF_CHARM))
{
ch->quittime = 20;
}
}

WAIT_STATE( ch, skill_table[gsn_offer].beats );

if( (obj = get_obj_wear( victim, arg1 )) == NULL )
{
if( (obj = get_obj_carry( victim, arg1, ch )) == NULL )
{
send_to_char("Your oratory is wasted, they don't have what you want.\n\r", ch);
return;
}
}


if (obj->pIndexData->vnum == OBJ_VNUM_PHYLACTERY)
{
send_to_char("No force could possibly make them give thatup.\n\r", ch);
return;
}

if( obj->item_type != ITEM_WEAPON)
{
send_to_char("Freedom may only be shared for weaponry!\n\r", ch);
return;
}

chance = get_skill(ch, gsn_offer);

percent  = number_percent();
percent += 25;
percent += ((victim->level - ch->level)*2);
if (IS_AWAKE(victim))
{
percent -= get_curr_stat(ch, STAT_DEX);
percent += get_curr_stat(victim, STAT_INT)/2;
percent += get_curr_stat(victim, STAT_DEX)/3;
}

if (IS_NPC(victim) && IS_SET(victim->act, ACT_IS_CHANGER) )
chance = 0;

if (!IS_AWAKE(victim) && !IS_SET(victim->act, ACT_IS_CHANGER) )
{
chance+=100;
}

if ( percent > chance)
{
send_to_char( "Your brilliant speech falls on seemingly deaf ears!\n\r", ch );
affect_strip(ch, gsn_sneak);
REMOVE_BIT(ch->affected_by, AFF_SNEAK);

check_improve(ch, gsn_offer, FALSE, 2);
act( "$n offers you a taste of freedom, but the price is too high.", ch, NULL, victim, TO_VICT    );
act( "$n offers $N a taste of freedom, but the price is to high.",  ch, NULL, victim, TO_NOTVICT );
sprintf(buf, "Help! %s tried to talk me into donating to the Revolution!", PERS(ch, NULL));
if (IS_AWAKE(victim) && !IS_NPC(victim))
{
do_myell( victim, buf );
sprintf(log_buf, "[%s] tried offer on [%s] at %d", ch->name, victim->name,
ch->in_room->vnum);
log_string(log_buf);
}

if ( !IS_NPC(ch))
{
if ( IS_NPC(victim) && IS_AWAKE(victim))
{
multi_hit( victim, ch, TYPE_UNDEFINED );
}
else
{
sprintf(buf, "$N tried to offer %s outlaw freedom.", victim->name);
wiznet(buf, ch, NULL, WIZ_FLAGS, 0, 0);
}
}

return;
}

if (!IS_NPC(victim))
{
sprintf(log_buf, "[%s] tried offer on [%s] at %d", ch->name,  victim->name, ch->in_room->vnum);
log_string(log_buf);
}

if ( !can_drop_obj( victim, obj )
||   IS_SET(obj->extra_flags, ITEM_INVENTORY) )
{
send_to_char( "Regretfully, they cannot let go of that.\n\r", ch );
act( "$n persuades you to try their freedom, but you cannot release what they want.", ch, NULL, victim, TO_VICT );
return;
}

if ( ch->carry_number + get_obj_number( obj ) > can_carry_n( ch ) )
{
send_to_char( "Your hands are full.\n\r", ch);
return;
}

if ( ch->carry_weight + get_obj_weight( obj ) > can_carry_w( ch ) )
{
send_to_char( "Either it's too heavy or you're too weak.\n\r", ch );
return;
}

send_to_char( "You persuade them to try your freedom.\n\r", ch );
act( "$n convinces you to try their freedom.", ch, NULL, victim, TO_VICT    );

if( obj->wear_loc > 0
&& !remove_obj( victim, obj->wear_loc, TRUE, TRUE ) )
{
send_to_char( "As much as they're moved, they can't let that go.\n\r", ch );
act( "$n persuades you to try their freedom, but you can't release what they want.", ch, NULL, victim, TO_VICT );
return;
}

reslot_weapon(victim);

af.where = TO_AFFECTS;
af.type = gsn_offer;
af.level = ch->level;
af.duration = 250 / ch_level;
af.location  = APPLY_NONE;
af.modifier  = 0;
af.bitvector = 0;
affect_to_char(victim, &af);

obj_from_char( obj );
obj_to_char( obj, ch );

check_improve(ch, gsn_offer, TRUE, 2);
return;
}
*/

/* The original offer plus adjustments as Xurinos wanted originally */
void do_offer( CHAR_DATA *ch, char *argument )
{
   char buf  [MAX_STRING_LENGTH];
   char arg1 [MAX_INPUT_LENGTH];
   char arg2 [MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   OBJ_DATA *obj;
   AFFECT_DATA af;
   int percent;
   int chance;

   if (get_skill(ch, gsn_offer) <= 0 ||
   /*  ch->level<skill_table[gsn_offer].skill_level[ch->class]*/
   !has_skill(ch, gsn_offer)) {
      send_to_char("Huh?\n\r", ch);
      return;
   }

   if (house_down(ch, HOUSE_OUTLAW))
   return;

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );

   if ( arg1[0] == '\0' || arg2[0] == '\0' )
   {
      send_to_char( "Offer whom a deal for what?\n\r", ch );
      return;
   }

   if ( ( victim = get_char_room( ch, arg2 ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if
   (
      IS_NPC(victim) &&
      (
         (
            obj = get_obj_list(victim, arg1, victim->carrying)
         ) != NULL ||
         (
            obj = get_obj_list(ch, arg1, victim->carrying)
         ) != NULL
      )
      &&
      obj->wear_loc == WEAR_BRAND
   )
   {
      mob_punish(victim, ch);
      return;
   }

   if ( victim == ch )
   {
      send_to_char( "You make yourself an offer and refuse anyway.\n\r", ch );
      return;
   }

   if (check_peace(ch)) return;

   if (is_safe(ch, victim, 0))
   return;

   if (victim->fighting != NULL)
   {
      send_to_char("Perhaps they would listen to you if they were not fighting.\n\r", ch);
      return;
   }

   if ( is_affected(victim, gsn_offer) )
   {
      send_to_char("They look like they have already been asked to make a donation to the cause recently.\n\r", ch);
      return;
   }

   if ( !str_cmp( arg1, "coin"  )
   ||   !str_cmp( arg1, "coins" )
   ||   !str_cmp( arg1, "gold"  )
   ||   !str_cmp( arg1, "silver"))
   {
      send_to_char("Resorting to petty thievery? C'mon, you can fence the REAL goods later.\n\r", ch);
      return;
   }

   if (ch->gold < 10)
   {
      send_to_char("The services of your henchmen require a small fee of 10 gold.\n\r", ch);
      send_to_char("You know what gold is, right? That pretty, bright yellowish coin?\n\r", ch);
      return;
   }

   if (!IS_AWAKE(victim))
   {
      send_to_char("Trying to intimidate a sleeping victim with an offer?\n\r", ch);
      return;
   }

   un_camouflage(ch, NULL);
   un_hide(ch, NULL);
   un_earthfade(ch, NULL);
   un_forest_blend(ch); /* - Wicket */

   ch->gold -= 10;
   if (!IS_NPC(ch) && !IS_IMMORTAL(ch) && !IS_IMMORTAL(victim))
   {
      if (!IS_NPC(victim) || IS_AFFECTED(victim, AFF_CHARM))
      {
         QUIT_STATE(ch, 20);
      }
   }
   WAIT_STATE( ch, skill_table[gsn_offer].beats );
   chance = get_skill(ch, gsn_offer);

   percent  = number_percent();
   percent -= ((victim->level - ch->level)*2);
   if (IS_AWAKE(victim))
   {
      percent -= get_curr_stat(ch, STAT_DEX);
      percent += get_curr_stat(victim, STAT_INT)/2;
      percent += get_curr_stat(victim, STAT_DEX)/3;
   }

   if (IS_NPC(victim) && IS_SET(victim->act, ACT_IS_CHANGER) )
   chance = 0;

   if (!IS_AWAKE(victim) && !IS_SET(victim->act, ACT_IS_CHANGER) )
   {
      chance+=100;
   }

   if ( ( get_obj_carry( victim, arg1, ch ) == NULL ) &&
   ( obj = get_obj_wear( victim, arg1 ) ) != NULL )
   {
      if (IS_NPC(victim)) chance = 0;
      else
      {
         chance -= 15;
         if (obj->item_type == ITEM_WEAPON) chance -= 20;
         if (victim->pcdata->ethos > 0) chance -= 5;
         if (victim->house == HOUSE_ENFORCER) chance -= 5;
      }
   }

   chance /= 2;

   af.where = TO_AFFECTS;
   af.type = gsn_offer;
   af.level = ch->level;
   af.duration = 1224 / ch->level;
   af.location  = APPLY_NONE;
   af.modifier  = 0;
   af.bitvector = 0;
   affect_to_char(victim, &af);

   if ( percent > chance)
   {
      /*
         Failure.
      */
      send_to_char( "You make an offer they can't refuse, and they refuse anyway!\n\r", ch );
      affect_strip(ch, gsn_sneak);
      REMOVE_BIT(ch->affected_by, AFF_SNEAK);

      check_improve(ch, gsn_offer, FALSE, 2);
      act( "$n makes you an offer you can't refuse, but you refuse anyway.", ch, NULL, victim, TO_VICT    );
      act( "$n tried to make $N an offer $E could not refuse and got snubbed.",  ch, NULL, victim, TO_NOTVICT );
      sprintf(buf, "Help! %s tried to swindle me out of my life savings!", PERS(ch, NULL));
      if (IS_AWAKE(victim) && !IS_NPC(victim))
      {
         do_myell( victim, buf );
         sprintf(log_buf, "[%s] tried to offer [%s] at %d", ch->name, victim->name,
         ch->in_room->vnum);
         log_string(log_buf);
      }
      send_to_char("Your giants run off with the 10 gold payment.\n\r", ch);
      if ( !IS_NPC(ch))
      {
         if ( IS_NPC(victim) && IS_AWAKE(victim))
         {
            multi_hit( victim, ch, TYPE_UNDEFINED );
         }
         else
         {
            sprintf(buf, "$N tried to make an offer to %s.", victim->name);
            wiznet(buf, ch, NULL, WIZ_FLAGS, 0, 0);
         }
      }

      return;
   }

   if (!IS_NPC(victim))
   {
      sprintf(log_buf, "[%s] tried to offer [%s] at %d", ch->name,  victim->name, ch->in_room->vnum);
      log_string(log_buf);
   }

   if ( ( obj = get_obj_carry( victim, arg1, ch ) ) == NULL &&
   ( obj = get_obj_wear( victim, arg1 ) ) == NULL)
   {
      send_to_char( "You call upon your giants to rough them up.\n\r", ch);
      send_to_char( "Your giants frown and display empty hands.\n\r", ch);
      send_to_char( "They take your 10 gold anyway.\n\r", ch);
      act( "$n makes you an offer for something you do not have.", ch, NULL, victim, TO_VICT    );
      act( "$n orders a pair of giants to rough $N up, but they find nothing.", ch, NULL, victim, TO_NOTVICT );
      return;
   }

   if ( !can_drop_obj( victim, obj )
   ||   IS_SET(obj->extra_flags, ITEM_INVENTORY)
   ||   obj->wear_loc == WEAR_BRAND)
   {
      send_to_char( "Your giants pull and pull but cannot seem to get it.\n\r", ch );
      act( "A pair of $n's giants cannot seem to yank something off you.", ch, NULL, victim, TO_VICT );
      act( "$n makes a pair of giants pull and pull at $N, but they give up.", ch, NULL, victim, TO_NOTVICT );
      return;
   }

   if ( ch->carry_number + get_obj_number( obj ) > can_carry_n( ch ) )
   {
      send_to_char( "Your giants eye you suspiciously, and you realize your hands are full.\n\r", ch );
      send_to_char( "Snickering, they take your 10 gold anyway.\n\r", ch);
      return;
   }

   if ( ch->carry_weight + get_obj_weight( obj ) > can_carry_w( ch ) )
   {
      send_to_char( "Your giants laugh at your inability to carry as much as they can.\n\r", ch );
      send_to_char( "They thank you for the show and walk away, 10 gold richer.\n\r", ch);
      return;
   }

   send_to_char( "You make them an offer they cannot refuse.\n\r", ch );
   act( "$n makes you an offer you cannot refuse.", ch, NULL, victim, TO_VICT    );

   if (obj == get_obj_wear( victim, arg1 ))
   {
      if (!remove_obj( victim, obj->wear_loc, TRUE, TRUE ))
      {
         send_to_char( "Your giants pull and pull but cannot seem to get it.\n\r", ch );
         act( "A pair of $n's giants cannot seem to yank something off you.", ch, NULL, victim, TO_VICT );
         act( "$n makes a pair of giants pull and pull at $N, but they give up.", ch, NULL, victim, TO_NOTVICT );
         return;
      }
      reslot_weapon(victim);
   }

   send_to_char( "Two giants show up to rough them up, and they acquire stolen goods!\n\r", ch);
   send_to_char( "You pay the giants 10 gold for their services.\n\r", ch);
   act( "Two giants rough you up, and your inventory is lighter.", ch, NULL, victim, TO_VICT    );
   act( "Two giants rough $N up and walk away satisfied.", ch, NULL, victim, TO_NOTVICT    );

   obj_from_char( obj );
   obj_to_char( obj, ch );

   check_improve(ch, gsn_offer, TRUE, 2);
   return;
}


void do_steal(CHAR_DATA* ch, char* argument)
{
   char buf  [MAX_STRING_LENGTH];
   char arg1 [MAX_INPUT_LENGTH];
   char arg2 [MAX_INPUT_LENGTH];
   CHAR_DATA* victim;
   OBJ_DATA* obj;
   int percent;
   int chance;

   if
   (
      (chance = get_skill(ch, gsn_steal)) <= 0 ||
      !has_skill(ch, gsn_steal)
   )
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }

   argument = one_argument(argument, arg1);
   argument = one_argument(argument, arg2);

   if (arg1[0] == '\0' || arg2[0] == '\0')
   {
      send_to_char("Steal what from whom?\n\r", ch);
      return;
   }

   if ((victim = get_char_room(ch, arg2)) == NULL)
   {
      send_to_char("They are not here.\n\r", ch);
      return;
   }

   if
   (
      !IS_IMMORTAL(ch) &&
      ch->class != CLASS_THIEF &&
      str_cmp(arg1, "coin") &&
      str_cmp(arg1, "coins") &&
      str_cmp(arg1, "gold") &&
      str_cmp(arg1, "silver")
   )
   {
      send_to_char("Only thieves are skilled enough for that.\n\r", ch);
      return;
   }

   if (IS_SET(victim->act2, ACT_NO_STEAL))
   {
      send_to_char("You failed.\n\r", ch);
      return;
   }
   if
   (
      IS_IMMORTAL(victim) &&
      !IS_NPC(victim) &&
      !IS_IMMORTAL(ch)
   )
   {
      send_to_char("You failed.\n\r", ch);
      if (!IS_NPC(ch))
      {
         ch->pcdata->learned[gsn_steal] /= 3;
      }
      return;
   }

   if (victim == ch)
   {
      send_to_char("That is pointless.\n\r", ch);
      return;
   }

   if (check_peace(ch))
   {
      return;
   }

   if (is_safe(ch, victim, 0))
   {
      return;
   }

   if (victim->fighting != NULL)
   {
      send_to_char("You cannot steal while that person is fighting.\n\r", ch);
      return;
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
   }
   WAIT_STATE(ch, skill_table[gsn_steal].beats);

   if (oblivion_blink(ch, victim))
   {
      return;
   }

   if
   (
      is_affected(victim, gsn_halo_of_eyes) &&
      can_see(victim, ch) &&
      number_percent() < 30
   )
   {
      send_to_char
      (
         "You notice one of the eyes surrounding them looking straight at you!"
         "\n\r",
         ch
      );
      chance = 0;
   }

   percent  = number_percent();
   percent += 25;
   percent += ((victim->level - ch->level) * 2);
   if (IS_AWAKE(victim))
   {
      percent -= get_curr_stat(ch, STAT_DEX);
      percent += get_curr_stat(victim, STAT_INT) / 2;
      percent += get_curr_stat(victim, STAT_DEX) / 3;
   }
   else
   {
      /*
      if (ch->level >= victim->level)
      {
      chance += ((victim->level - ch->level) * 25);
      }
      else
      {
      chance += 20;
      }
      */
      chance+=100;
   }

   if (is_affected(victim, gsn_worm_hide))
   {
      percent += 50;
   }
   /* halflings steal better than others -werv */
   if
   (
      !IS_NPC(ch) &&
      ch->race == grn_halfling
   )
   {
      chance += 35;
   }

   if
   (
      IS_NPC(victim) &&
      IS_SET(victim->act, ACT_IS_CHANGER)
   )
   {
      chance = 0;
   }

   if (percent > chance)
   {
      /*
         Failure.
      */
      send_to_char("Oops.\n\r", ch);
      un_sneak(ch, NULL); /* Wicket */
      un_hide(ch, NULL); /* XUR */

      check_improve(ch, gsn_steal, FALSE, 2);
      act
      (
         "$n tried to steal from you.\n\r",
         ch,
         NULL,
         victim,
         TO_VICT
      );
      act
      (
         "$n tried to steal from $N.\n\r",
         ch,
         NULL,
         victim,
         TO_NOTVICT
      );
      if (IS_AWAKE(victim) && !IS_NPC(victim))
      {
         switch (number_range(0, 3))
         {
            default: /* 0 */
            {
               sprintf
               (
                  buf,
                  "%s is a lousy thief!",
                  PERS(ch, victim)
               );
               break;
            }
            case (1):
            {
               sprintf
               (
                  buf,
                  "%s couldn't rob %s way out of a paper bag!",
                  PERS(ch, victim),
                  (ch->sex == 2) ? "her" : "his"
               );
               break;
            }
            case (2):
            {
               sprintf
               (
                  buf,
                  "%s tried to rob me!",
                  PERS(ch, victim)
               );
               break;
            }
            case (3):
            {
               sprintf
               (
                  buf,
                  "Keep your hands out of there, %s!",
                  PERS(ch, victim)
               );
               break;
            }
         }
         do_myell(victim, buf);
         sprintf
         (
            log_buf,
            "[%s] tried to steal [%s] at %d",
            ch->name,
            victim->name,
            ch->in_room->vnum
         );
         log_string(log_buf);
      }
      if (!IS_NPC(ch))
      {
         if (IS_NPC(victim) && IS_AWAKE(victim))
         {
            multi_hit(victim, ch, TYPE_UNDEFINED);
         }
         else
         {
            sprintf
            (
               buf,
               "$N tried to steal from %s.",
               victim->name
            );
            wiznet
            (
               buf,
               ch,
               NULL,
               WIZ_FLAGS,
               0,
               0
            );
         }
      }
      return;
   }

   if (!IS_NPC(victim))
   {
      sprintf
      (
         log_buf,
         "[%s] tried to steal [%s] at %d",
         ch->name,
         victim->name,
         ch->in_room->vnum
      );
      log_string(log_buf);
   }
   check_perception(ch, victim, "stealing");

   if
   (
      !str_cmp(arg1, "coin") ||
      !str_cmp(arg1, "coins") ||
      !str_cmp(arg1, "gold") ||
      !str_cmp(arg1, "silver")
   )
   {
      int gold;
      int silver;

      gold = victim->gold * number_range(1, ch->level) / 60;
      silver = victim->silver * number_range(1, ch->level) / 60;
      if (gold <= 0 && silver <= 0)
      {
         send_to_char("You couldn't get any coins.\n\r", ch);
         return;
      }

      ch->gold       += gold;
      ch->silver     += silver;
      victim->silver -= silver;
      victim->gold   -= gold;
      if (silver <= 0)
      {
         sprintf(buf, "Bingo!  You got %d gold coins.\n\r", gold);
      }
      else if (gold <= 0)
      {
         sprintf( buf, "Bingo!  You got %d silver coins.\n\r", silver);
      }
      else
      {
         sprintf
         (
            buf,
            "Bingo!  You got %d silver and %d gold coins.\n\r",
            silver,
            gold
         );
      }
      send_to_char(buf, ch);
      check_improve(ch, gsn_steal, TRUE, 2);
      return;
   }

   if ((obj = get_obj_carry(victim, arg1, ch)) == NULL)
   {
      send_to_char("You cannot find it.\n\r", ch);
      return;
   }

   if (obj->pIndexData->vnum == OBJ_VNUM_PHYLACTERY)
   {
      send_to_char("A malevolent force prevents you from stealing that.\n\r", ch);
      return;
   }

   if
   (
      !can_drop_obj(ch, obj) ||
      (
         IS_SET(obj->extra_flags, ITEM_INVENTORY) &&
         IS_NPC(victim)
      )
   )
   {
      un_hide(ch, NULL); /* XUR */
      send_to_char("You cannot pry it away.\n\r", ch);
      return;
   }

   if
   (
      obj->item_type == ITEM_BOAT &&
      victim->in_room->sector_type == SECT_WATER_NOSWIM &&
      !IS_FLYING(victim)
   )
   {
      send_to_char("You cannot steal a boat while it is being used!\n\r", ch);
      return;
   }
   if (ch->carry_number + get_obj_number(obj) > can_carry_n(ch))
   {
      send_to_char( "You have your hands full.\n\r", ch );
      return;
   }

   if (ch->carry_weight + get_obj_weight(obj) > can_carry_w(ch))
   {
      send_to_char("You cannot carry that much weight.\n\r", ch);
      return;
   }

   if
   (
      get_obj_weight(obj) >
      (
         (
            get_skill(ch, gsn_steal) / 100.0
         ) *
         (
            str_app[get_curr_stat(ch, STAT_STR)].wield * 10
         )
      )
   )
   {
      un_hide(ch, NULL);  /* XUR */
   }

   obj_from_char(obj);
   obj_to_char(obj, ch);
   check_improve(ch, gsn_steal, TRUE, 2);
   send_to_char("Got it!\n\r", ch);
   return;
}

/* Thief skill 'plant': Coded based on do_give and do_steal -- Wicket (04/01/01) */
void do_plant( CHAR_DATA *ch, char *argument )
{
   char buf  [MAX_STRING_LENGTH];
   char arg1 [MAX_INPUT_LENGTH];
   char arg2 [MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   OBJ_DATA  *obj = NULL;
   bool silver = FALSE;
   int  amount = 0;
   int  percent;
   int  chance;

   if ( get_skill(ch, gsn_plant) <= 0 || !has_skill(ch, gsn_plant) )
   {
      send_to_char( "Huh?\n\r", ch );
      return;
   }

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );

   if ( arg1[0] == '\0' || arg2[0] == '\0' )
   {
      send_to_char( "Attempt to plant what on whom?\n\r", ch );
      return;
   }

   /*
      First we check to see if the char is using an item or money and if
      it's a valid object to use.
   */
   if ( is_number( arg1 ) )
   {
      amount   = atoi(arg1);
      if ( amount <= 0
      || ( str_cmp( arg2, "coins" ) && str_cmp( arg2, "coin" ) &&
      str_cmp( arg2, "gold"  ) && str_cmp( arg2, "silver")) )
      {
         send_to_char( "Sorry, you can't do that.\n\r", ch );
         return;
      }

      silver = str_cmp(arg2, "gold");

      if ( (!silver && ch->gold < amount) || (silver && ch->silver < amount) )
      {
         send_to_char( "You haven't got that much.\n\r", ch );
         return;
      }

      argument = one_argument( argument, arg2 );
      if ( arg2[0] == '\0' )
      {
         sprintf( buf, "Attempt to plant %d %s on whom?", amount, silver ? "silver" : "gold" );
         act( buf, ch, NULL, NULL, TO_CHAR );
         return;
      }
   }
   else
   {
      if ( ( obj = get_obj_carry( ch, arg1, ch ) ) == NULL )
      {
         send_to_char( "You do not have that item.\n\r", ch );
         return;
      }
      if ( obj->wear_loc != WEAR_NONE )
      {
         send_to_char( "You must remove it first.\n\r", ch );
         return;
      }
      if ( !can_drop_obj( ch, obj ) )
      {
         send_to_char( "You can't let go of it.\n\r", ch );
         return;
      }
   }

   /* Second, we check to see if the char is able to attempt a plant */
   if ( check_peace(ch) )
   return;

   /* Third, we check to see if the victim is valid */
   if ( ( victim = get_char_room( ch, arg2 ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }
   if ( victim == ch )
   {
      send_to_char( "That's pointless.\n\r", ch );
      return;
   }

   /* Fourth, we check to see if the victim can accept the item */
   if (IS_NPC(victim) && victim->pIndexData->pShop != NULL)
   {
      act("$N tells you 'Sorry, you'll have to sell that.'",
      ch, NULL, victim, TO_CHAR);
      ch->reply = victim;
      return;
   }

   if (victim->fighting != NULL)
   {
      send_to_char( "They are moving around too much.\n\r", ch );
      return;
   }

   if (oblivion_blink(ch, victim))
   {
      return;
   }

   if ( !is_number( arg1 ) )
   {
      if ( victim->carry_number + get_obj_number( obj ) > can_carry_n( victim ) )
      {
         act( "$N has $S hands full.", ch, NULL, victim, TO_CHAR );
         return;
      }
      if (get_carry_weight(victim) + get_obj_weight(obj) > can_carry_w( victim ) )
      {
         act( "$N can't carry that much weight.", ch, NULL, victim, TO_CHAR );
         return;
      }
      /*        if ( !can_see_obj( victim, obj ) )
      {
      act( "$N can't see it.", ch, NULL, victim, TO_CHAR );
      return;
      } */
   }
   if ( is_safe(ch, victim, 0) )
   return;

   /* Fifth, the char takes a chance and attempts to plant the object on the victim */
   WAIT_STATE( ch, skill_table[gsn_plant].beats );
   chance = get_skill( ch, gsn_plant );

   if (!IS_NPC(victim) && IS_IMMORTAL(victim) && !IS_IMMORTAL(ch))
   {
      send_to_char("You failed.\n\r", ch);
      act( "$n tried to plant something on you. Silly mortal, trix are for kids *wink*", ch, NULL, victim, TO_VICT );
      return;
   }

   if (is_affected(victim, gsn_halo_of_eyes)
   && can_see(victim, ch)
   && number_percent() < 30)
   {
      send_to_char("You notice one of the eyes surrounding them looking straight at you!\n\r", ch);
      chance = 0;
   }

   percent  = number_percent();
   percent += 25;
   percent += ( (victim->level - ch->level)*2 );

   if ( !IS_AWAKE(victim) )
   chance += 100;
   else
   {
      percent -= get_curr_stat(ch, STAT_DEX);
      percent += get_curr_stat(victim, STAT_INT)/2;
      percent += get_curr_stat(victim, STAT_DEX)/3;
   }
   if ( is_affected(victim, gsn_worm_hide) )
   percent += 50;
   if ( !IS_NPC(ch) && ch->race == grn_halfling )
   chance += 35;

   /* Sixth, we check if the char failed his chance or not */
   if ( percent > chance)
   {
      /*
         Failure.
      */
      send_to_char( "Oops.\n\r", ch );
      un_sneak(ch, NULL);
      un_hide(ch, NULL);

      check_improve(ch, gsn_plant, FALSE, 2);
      act( "You've caught $n trying to plant something on you!\n\r", ch, NULL, victim, TO_VICT );
      act( "$N caught $n trying to plant something on $M!\n\r",  ch, NULL, victim, TO_NOTVICT );

      switch (number_range(0, 4))
      {
         case 0 :
         sprintf( buf, "%s is a lousy thief!", PERS(ch, victim) );
         break;
         case 1 :
         sprintf( buf, "%s couldn't plant a needle in a haystack!", PERS(ch, victim) );
         break;
         case 2 :
         sprintf( buf, "Do I look like a knapsack to you, %s?!", PERS(ch, victim) );
         break;
         case 3 :
         sprintf( buf, "Keep your filthy hands off of me, %s!", PERS(ch, victim));
         break;
         case 4 :
         sprintf( buf, "Go find yourself a garden to plant your trash in, %s!", PERS(ch, victim));
         break;
      }

      if ( !IS_NPC(victim) && IS_AWAKE(victim) )
      {
         do_myell( victim, buf );
         if ( is_number( arg1 ) )
         {
            sprintf(log_buf, "[%s] tried to plant some coins on [%s] at %d", ch->name,
            victim->name, ch->in_room->vnum);
            log_string(log_buf);
         }
         else
         {
            sprintf(log_buf, "[%s] tried to plant %s on [%s] at %d", ch->name,
            obj->short_descr, victim->name, ch->in_room->vnum);
            log_string(log_buf);
         }
      }
      if ( !IS_NPC(ch) )
      {
         if ( IS_NPC(victim) && IS_AWAKE(victim))
         {
            multi_hit( victim, ch, TYPE_UNDEFINED );
         }
         else
         {
            if ( is_number( arg1 ) )
            sprintf(buf, "$N tried to plant some coins on %s.", victim->name);
            else
            sprintf(buf, "$N tried to plant %s on %s.", obj->short_descr, victim->name);
            wiznet(buf, ch, NULL, WIZ_FLAGS, 0, 0);
         }
      }
   }
   else
   {
      check_perception(ch, victim, "planting junk");
      /*
         Succession.
      */
      if ( is_number( arg1 ) )
      {
         if (silver)
         {
            ch->silver        -= amount;
            victim->silver    += amount;
         }
         else
         {
            ch->gold          -= amount;
            victim->gold      += amount;
         }
         check_improve( ch, gsn_plant, TRUE, 2);
         sprintf( buf, "Success!  You've planted %d %s on $N.", amount, silver ? "silver" : "gold" );
         act( buf, ch, NULL, victim, TO_CHAR );
         return;
      }

      obj_from_char( obj );
      obj_to_char( obj, victim );
      check_improve( ch, gsn_plant, TRUE, 2);
      act( "Success!  You've planted $p on $N.", ch, obj, victim, TO_CHAR );

      /* Catch same site and newbie plants */
      if (!IS_NPC(ch) && !IS_NPC(victim) && (victim->desc == NULL))
      {
         sprintf(buf, "[nolink] %s (%d) planted %s (%d) on %s (%d)",
         ch->name, get_trust(ch), obj->short_descr, obj->level,
         victim->name, get_trust(victim));
         wiznet(buf, ch, NULL, WIZ_TRANSFERS, 0, get_trust(ch));
         sprintf(log_buf, "%s", buf);
         log_string(log_buf);
      }
      else
      if (!IS_NPC(ch) && !IS_NPC(victim))
      {
         if (!strcmp(ch->host, victim->host))
         {
            sprintf(buf, "[site] %s (%d) planted %s (%d) on %s (%d)",
            ch->name, get_trust(ch), obj->short_descr, obj->level,
            victim->name, get_trust(victim));
            wiznet(buf, ch, NULL, WIZ_TRANSFERS, 0, get_trust(ch));
            sprintf(log_buf, "%s", buf);
            log_string(log_buf);
         }
         else
         if ((get_trust(victim) <= 10) && (get_trust(ch) > 10)
         && (get_trust(ch) > get_trust(victim) + 8)
         && (obj->level > get_trust(victim) + 8))
         {
            sprintf(buf, "[newbie] %s (%d) planted %s (%d) on %s (%d)",
            ch->name, get_trust(ch), obj->short_descr, obj->level,
            victim->name, get_trust(victim));
            wiznet(buf, ch, NULL, WIZ_TRANSFERS, 0, get_trust(ch));
            sprintf(log_buf, "%s", buf);
            log_string(log_buf);
         }
      }

      /* Make sure the houses get their shudder */
      if (obj->pIndexData->house)
      {
         house_shudder(obj->pIndexData->house);
      }

      /* Log all plants on mobs that do not belong to the char */
      if (!IS_NPC(ch)
      && IS_NPC(victim)
      && !((IS_AFFECTED(victim, AFF_CHARM))
      && (victim->leader == ch)))
      {
         sprintf(log_buf, "[MOB PLANT] %s planted %s on %s", ch->name,
         obj->short_descr, victim->short_descr);
         wiznet(log_buf, ch, NULL, WIZ_TRANSFERS, 0, get_trust(ch));
         log_string(log_buf);
      }

      /* Finally, check to make sure the victim drops anything he can't carry */
      if (cant_carry(victim, obj))
      {
         act("$n is burnt by $p and drops it.", victim, obj, NULL, TO_ROOM);
         act("You are burnt by $p and drop it.", victim, obj, NULL, TO_CHAR);
         obj_from_char(obj);
         obj_to_room(obj, victim->in_room);
      }
   }

   return;
}

/*
   Shopping commands.
*/
CHAR_DATA* find_keeper(CHAR_DATA* ch, char* argument)
{
   CHAR_DATA* keeper;
   SHOP_DATA* pShop;

   pShop = NULL;
   if (argument[0] != '\0')
   {
      if
      (
         (
            keeper = get_char_room(ch, argument)
         ) != NULL &&
         IS_NPC(keeper) &&
         (
            pShop = keeper->pIndexData->pShop
         ) != NULL
      )
      {
         /* Good */
      }
      else
      {
         return NULL;
      }
   }
   else
   {
      /* find first */
      for (keeper = ch->in_room->people; keeper; keeper = keeper->next_in_room)
      {
         if
         (
            IS_NPC(keeper) &&
            (
               pShop = keeper->pIndexData->pShop
            ) != NULL
         )
         {
            break;
         }
      }
   }

   if ( pShop == NULL )
   {
      return NULL;
   }

   if (keeper->fighting != NULL)
   {
      do_say( keeper, "Look, pal, can't you see I'm busy?" );
      return NULL;
   }

   /*
      Invisible or hidden people.
   */
   if ( !can_see( keeper, ch ) )
   {
      do_say( keeper, "I don't trade with folks I can't see." );
      return NULL;
   }

   /* Keepers will not trade with members of Legion within Castle Valor - Wicket */
   if ( !IS_NPC(ch) && (ch->house == HOUSE_EMPIRE)
   && ((ch->in_room->vnum > 10200) && (ch->in_room->vnum < 10251)) )
   {
      act("$N shakes $S head at you.", ch, NULL, keeper, TO_CHAR);
      act("$N shakes $S head at $n.", ch, NULL, keeper, TO_ROOM);
      return NULL;
   }

   /* Keepers will not trade with Rebels within Ethshar - Wicket
   if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_REBEL) && is_occupied(ch->in_room->vnum) )
   {
   do_say (keeper, "Be gone rebel! Only citizens loyal to the Empire are welcome here.");
   return NULL;
   }
   */
   /*
      Shop hours.
   */
   if ( current_mud_time.hour < pShop->open_hour )
   {
      do_say( keeper, "Sorry, I am closed. Come back later." );
      return NULL;
   }

   if ( current_mud_time.hour > pShop->close_hour )
   {
      do_say( keeper, "Sorry, I am closed. Come back tomorrow." );
      return NULL;
   }

   return keeper;
}

/* insert an object at the right spot for the keeper */
void obj_to_keeper( OBJ_DATA *obj, CHAR_DATA *ch )
{
   OBJ_DATA *t_obj, *t_obj_next;

   /* see if any duplicates are found */
   for (t_obj = ch->carrying; t_obj != NULL; t_obj = t_obj_next)
   {
      t_obj_next = t_obj->next_content;

      if (obj->pIndexData == t_obj->pIndexData
      &&  !str_cmp(obj->short_descr, t_obj->short_descr))
      {
         /* if this is an unlimited item, destroy the new one */
         if (IS_OBJ_STAT(t_obj, ITEM_INVENTORY))
         {
            extract_obj(obj, FALSE);
            return;
         }
         obj->cost = t_obj->cost; /* keep it standard */
         break;
      }
   }

   obj_to_char(obj, ch);
   if
   (
      t_obj != NULL &&
      ch->carrying == obj
   )
   {
      /* Put in correct order in list */
      ch->carrying = obj->next_content;
      obj->next_content = t_obj->next_content;
      t_obj->next_content = obj;
   }
   /*
      to fix a weird bug where selling a object to a shoppy other than
      its own, reduces the cost
   */
   obj->cost            = obj->pIndexData->cost;
}

/* get an object from a shopkeeper's list */
OBJ_DATA *get_obj_keeper( CHAR_DATA *ch, CHAR_DATA *keeper, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   OBJ_DATA *obj;
   int number;
   int count;

   number = number_argument( argument, arg );
   count  = 0;
   for ( obj = keeper->carrying; obj != NULL; obj = obj->next_content )
   {
      if (obj->wear_loc == WEAR_NONE
      &&  can_see_obj( keeper, obj )
      &&  can_see_obj(ch, obj)
      &&  (((!IS_NPC(keeper)) && (obj->newcost > 0)) || (IS_NPC(keeper)))
      &&  is_name( arg, obj->name ) )
      {
         if ( ++count == number )
         return obj;

         /* skip other objects of the same name */
         while (obj->next_content != NULL
         && obj->pIndexData == obj->next_content->pIndexData
         && !str_cmp(obj->short_descr, obj->next_content->short_descr))
         obj = obj->next_content;
      }
   }

   return NULL;
}

int get_cost( CHAR_DATA *keeper, OBJ_DATA *obj, bool fBuy )
{
   SHOP_DATA *pShop;
   int cost;

   if ( obj == NULL || ( pShop = keeper->pIndexData->pShop ) == NULL )
   return 0;

   if ( fBuy )
   {
      cost = obj->cost * pShop->profit_buy  / 100;
   }
   else
   {
      OBJ_DATA *obj2;
      int itype;

      cost = 0;
      for ( itype = 0; itype < MAX_TRADE; itype++ )
      {
         if ( obj->item_type == pShop->buy_type[itype] )
         {
            cost = obj->cost * pShop->profit_sell / 100;
            break;
         }
      }

      if (!IS_OBJ_STAT(obj, ITEM_SELL_EXTRACT))
      for ( obj2 = keeper->carrying; obj2; obj2 = obj2->next_content )
      {
         if ( obj->pIndexData == obj2->pIndexData
         &&   !str_cmp(obj->short_descr, obj2->short_descr) )
         {
            if (IS_OBJ_STAT(obj2, ITEM_INVENTORY))
            cost /= 2;
            else
            cost = cost * 3 / 4;
         }
      }
   }

   if ( obj->item_type == ITEM_STAFF || obj->item_type == ITEM_WAND )
   {
      if (obj->value[1] == 0)
      cost /= 4;
      else
      cost = cost * obj->value[2] / obj->value[1];
   }

   return cost;
}



void do_buy( CHAR_DATA *ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   char arg[MAX_INPUT_LENGTH];
   char from[MAX_INPUT_LENGTH];

   int cost, roll;
   int cnt;
   int skill;

   CHAR_DATA *keeperFirst;

   if ( argument[0] == '\0' )
   {
      send_to_char( "Buy what?\n\r", ch );
      return;
   }

   argument = one_argument(argument, arg);
   argument = one_argument(argument, from);
   if ( IS_SET(ch->in_room->room_flags, ROOM_PET_SHOP) )
   {
      CHAR_DATA *pet;
      ROOM_INDEX_DATA *pRoomIndexNext;
      ROOM_INDEX_DATA *in_room;

      if ( IS_NPC(ch) )
      return;


      /* malignus */



      keeperFirst = find_keeper(ch, from);
      if ( keeperFirst == NULL )
      {
         send_to_char( "Where is the owner of this petshop eh?\n\r", ch );
         return;
      }




      /* hack to make new thalos pets work */
      if (ch->in_room->vnum == 9621)
      pRoomIndexNext = get_room_index(9706);
      else
      pRoomIndexNext = get_room_index( ch->in_room->vnum + 1 );
      if ( pRoomIndexNext == NULL )
      {
         bug( "Do_buy: bad pet shop at vnum %d.", ch->in_room->vnum );
         send_to_char( "Sorry, you can't buy that here.\n\r", ch );
         return;
      }

      in_room     = ch->in_room;
      char_from_room(ch);
      char_to_room_1(ch, pRoomIndexNext, TO_ROOM_AT);
      pet         = get_char_room( ch, arg );
      char_from_room(ch);
      char_to_room_1(ch, in_room, TO_ROOM_AT);



      if ( pet == NULL || !IS_SET(pet->act, ACT_PET) )
      {
         send_to_char( "Sorry, you can't buy that here.\n\r", ch );
         return;
      }

      if ( ch->pet != NULL )
      {
         send_to_char("You already own a pet.\n\r", ch);
         return;
      }

      cost = 10 * pet->level * pet->level;

      if ( (ch->silver + 100 * ch->gold) < cost )
      {
         send_to_char( "You can't afford it.\n\r", ch );
         return;
      }

      if ( ch->level < pet->level )
      {
         send_to_char(
         "You're not powerful enough to master this pet.\n\r", ch );
         return;
      }

      /* haggle */
      roll = number_percent();
      if
      (
         (
            skill = get_skill(ch, gsn_haggle)
         ) &&
         roll <= skill
      )
      {
         cost -= cost / (number_range(2, 4));
         sprintf(buf, "You haggle the price down to %d coins.\n\r", cost);
         send_to_char(buf, ch);
         check_improve(ch, gsn_haggle, TRUE, 4);

      }

      deduct_cost(ch, cost);
      pet                        = create_mobile( pet->pIndexData );
      SET_BIT(pet->act, ACT_PET);
      SET_BIT(pet->affected_by, AFF_CHARM);
      SET_BIT(pet->act, ACT_SENTINEL);
      pet->comm = COMM_NOTELL|COMM_NOSHOUT|COMM_NOCHANNELS|COMM_NOPRAY;

      sprintf( buf, "%sA neck tag says 'I belong to %s'.\n\r",
      pet->description, PERS(ch, NULL) );
      free_string( pet->description );
      pet->description = str_dup( buf );

      char_to_room( pet, ch->in_room );
      add_follower( pet, ch );
      pet->leader = ch;
      ch->pet = pet;
      send_to_char( "Enjoy your pet.\n\r", ch );
      act( "$n bought $N as a pet.", ch, NULL, pet, TO_ROOM );
      return;
   }
   else
   {
      CHAR_DATA *keeper, *keeper2;
      OBJ_DATA *obj,*t_obj;
      int number, count = 1;
      bool found=FALSE, outlawdealer=FALSE;

      /*
      for ( keeper = ch->in_room->people; keeper; keeper = keeper->next_in_room)
      {
      for ( obj = keeper->carrying; obj != NULL; obj = obj->next_content )
      {
      if ( obj->wear_loc == WEAR_NONE
      &&   can_see_obj( ch, obj )
      &&   obj->newcost > 0)
      found = TRUE;
      }
      if (found) break;
      }
      */
      for (keeper = ch->in_room->people; keeper; keeper = keeper->next_in_room)
      {
         if (IS_NPC(keeper) && keeper->pIndexData->vnum == 9804)
         {
            found = TRUE; outlawdealer=TRUE;
            break;
         }
      }
      if (found)
      {
         found = FALSE;
         for ( keeper = char_list; keeper; keeper = keeper->next)
         {
            if (IS_NPC(keeper)) continue;
            for ( obj = keeper->carrying; obj != NULL; obj = obj->next_content )
            {
               if ( obj->wear_loc == WEAR_NONE
               &&   can_see_obj( ch, obj )
               &&   obj->newcost > 0)
               found = TRUE;
               if (found) break;
            }
            if (found) break;
         }
      } else found = FALSE;

      keeper2 = find_keeper(ch, from);
      if (!found && keeper2 == NULL)
      {
         if (outlawdealer)
         send_to_char("The Underground has nothing to offer at this time.\n\r", ch);
         else
         {
            send_to_char("You cannot do that here.\n\r", ch);
         }
         return;
      }

      if (keeper2 != NULL)
      {
         keeper = keeper2;
         found = FALSE;
      }
      number = mult_argument(arg, arg);
      if (found)
      {
         obj = NULL;
         for ( keeper = char_list; keeper; keeper = keeper->next)
         {
            if (IS_NPC(keeper)) continue;
            obj = get_obj_keeper(ch, keeper, arg);
            if (obj != NULL) break;
         }
      } else
      obj  = get_obj_keeper( ch, keeper, arg );
      if (obj == NULL)
      cost = 0;
      else
      cost = found ? obj->newcost : get_cost( keeper, obj, TRUE );

      if ((get_trust(ch) < 15) && found)
      {
         send_to_char("The Underground will have nothing to do with people of your inexperience.\n\r", ch);
         return;
      }

      if (number < 1)
      {
         if (found)
         send_to_char("You realize that would be silly.\n\r", ch);
         else
         act("$n tells you 'Get real!", keeper, NULL, ch, TO_VICT);
         return;
      }

      if ( cost <= 0 || !can_see_obj( ch, obj ) )
      {
         if (found)
         {
            send_to_char( "You do not think that is sold here. Try 'list'.\n\r", ch);
         }
         else
         {
            act( "$n tells you 'I don't sell that -- try 'list''.",
            keeper, NULL, ch, TO_VICT );
            ch->reply = keeper;
         }
         return;
      }

      if (found && ch == keeper)
      {
         send_to_char("You already have it.\n\r", ch);
         return;
      }

      if (found && number > 1)
      {
         send_to_char("The underground sells items one at a time.\n\r", ch);
         return;
      }

      if (!IS_OBJ_STAT(obj, ITEM_INVENTORY) && !found)
      {
         for (t_obj = obj->next_content;
         count < number && t_obj != NULL;
         t_obj = t_obj->next_content)
         {
            if (t_obj->pIndexData == obj->pIndexData
            &&  !str_cmp(t_obj->short_descr, obj->short_descr))
            count++;
            else
            break;
         }

         if (count < number)
         {
            act("$n tells you 'I don't have that many in stock.",
            keeper, NULL, ch, TO_VICT);
            ch->reply = keeper;
            return;
         }
      }

      if (( (ch->silver + ch->gold * 100) < cost * number && !found) ||
      ( ch->gold < cost && found))
      {
         if (number > 1)
         act("$n tells you 'You can't afford to buy that many.",
         keeper, obj, ch, TO_VICT);
         else
         {
            if (found)
            send_to_char("You do not have the gold.\n\r", ch);
            else
            act( "$n tells you 'You can't afford to buy $p'.",
            keeper, obj, ch, TO_VICT );
         }
         ch->reply = keeper;
         return;
      }

      if ( (obj->level > ch->level) && (!found) )
      {
         act( "$n tells you 'You can't use $p yet'.",
         keeper, obj, ch, TO_VICT );
         ch->reply = keeper;
         return;
      }

      if (ch->carry_number +  number * get_obj_number(obj) > can_carry_n(ch))
      {
         send_to_char( "You can't carry that many items.\n\r", ch );
         return;
      }

      if ( ch->carry_weight + number * get_obj_weight(obj) > can_carry_w(ch))
      {
         send_to_char( "You can't carry that much weight.\n\r", ch );
         return;
      }

      if (!found)
      {
         /* haggle */
         roll = number_percent();
         if (!IS_OBJ_STAT(obj, ITEM_SELL_EXTRACT) && has_skill(ch, gsn_haggle)
         && roll < UMAX(get_skill(ch, gsn_haggle), 2))
         {
            cost -= cost / (number_range(2, 4));
            act("You haggle with $N.", ch, NULL, keeper, TO_CHAR);
            check_improve(ch, gsn_haggle, TRUE, 4);
         }
      }

      if (number > 1)
      {
         sprintf(buf, "$n buys $p[%d].", number);
         act(buf, ch, obj, NULL, TO_ROOM);
         sprintf(buf, "You buy $p[%d] for %d silver.", number, cost * number);
         act(buf, ch, obj, NULL, TO_CHAR);
      }
      else
      {
         act( "$n buys $p.", ch, obj, NULL, TO_ROOM );
         if (!found)
         sprintf(buf, "You buy $p for %d silver.", cost);
         else
         sprintf(buf, "You buy $p for %d gold.", cost);
         act( buf, ch, obj, NULL, TO_CHAR );
         if (found)
         {
            sprintf(buf, "$p is purchased from you for %d gold.", cost);
            act( buf, keeper, obj, NULL, TO_CHAR );
            if (!IS_NPC(ch) && !IS_NPC(keeper))
            {
               for (cnt = 0; cnt < 2000; cnt++)
               if (black_market_data[cnt][0] == obj->pIndexData->vnum) break;
               if (black_market_data[cnt][0] != obj->pIndexData->vnum)
               cnt = black_market_data_free;
               black_market_data[cnt][0] = obj->pIndexData->vnum;
               black_market_data[cnt][1] = (black_market_data[cnt][1] *
               black_market_data[cnt][2] + cost)
               /(black_market_data[cnt][2]+1);
               black_market_data[cnt][2]++;
               black_market_data_free++;
               if (black_market_data_free > 1999) black_market_data_free = 0;
               save_globals();
               sprintf(log_buf, "%s purchased %s from %s.", ch->name,
               obj->short_descr, keeper->name);
               log_string(log_buf);
            }
         }
      }
      if (!found)
      {
         deduct_cost(ch, cost * number);
         keeper->gold += cost * number/100;
         keeper->silver += cost * number - (cost * number/100) * 100;
      } else
      {
         ch->gold -= cost;
         keeper->gold += cost;
      }

      for (count = 0; count < number; count++)
      {
         if ( IS_SET( obj->extra_flags, ITEM_INVENTORY ) && !found)
         t_obj = create_object( obj->pIndexData, obj->level );
         else
         {
            t_obj = obj;
            obj = obj->next_content;
            obj_from_char( t_obj );
         }

         obj_to_char( t_obj, ch );
         if ((cost < t_obj->cost) && !found)
         t_obj->cost = cost;
      }
   }
}

void do_fence( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   OBJ_DATA *obj;
   bool found;
   int chance, amount;
   int cnt;

   if ( (chance = get_skill(ch, gsn_fence)) <= 0
   || /*ch->level < skill_table[gsn_fence].skill_level[ch->class] */
   !has_skill(ch, gsn_fence))
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }

   if (house_down(ch, HOUSE_OUTLAW))
   return;

   argument = one_argument( argument, arg );

   if ( arg[0] == '\0' )
   {
      found = FALSE;
      for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
      {
         if ( obj->wear_loc == WEAR_NONE
         &&   can_see_obj( ch, obj )
         &&   obj->newcost > 0)
         {
            if (!found)
            {
               found = TRUE;
               send_to_char("Your personal sells:\n\r", ch);
               send_to_char( "[Price] Item\n\r", ch );
            }
            sprintf(buf, "[%5d] %s\n\r", obj->newcost, obj->short_descr);
            send_to_char(buf, ch);
         }
      }
      if (!found) send_to_char("You are not fencing anything.\n\r", ch);
      return;
   }

   if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
   {
      send_to_char( "You do not have that item.\n\r", ch );
      return;
   }

   if ( !can_drop_obj( ch, obj ) )
   {
      send_to_char( "You had better get it uncursed first.\n\r", ch );
      return;
   }

   argument = one_argument( argument, arg );

   if (!str_cmp(arg, "suggest"))
   {
      for (cnt = 0; cnt < 2000; cnt++)
      if (black_market_data[cnt][0] == obj->pIndexData->vnum) break;
      if (black_market_data[cnt][0] != obj->pIndexData->vnum)
      {
         send_to_char("There are no records of this item selling before.\n\r", ch);
         return;
      }
      sprintf(buf, "This item has sold %d times for an average of %d gold.\n\r",
      black_market_data[cnt][2], black_market_data[cnt][1]);
      send_to_char(buf, ch);
      return;
   }
   amount = atoi(arg);

   if (amount < 0)
   {
      send_to_char("You must specify an amount of gold greater than or equal to 0.\n\r", ch);
      return;
   }

   if (amount == 0)
   {
      obj->newcost = amount;
      send_to_char("You are no longer fencing this item.\n\r", ch);
      return;
   }

   if (number_percent() > chance)
   {
      send_to_char("You get confused about the new price and drop the item, losing it.\n\r", ch);
      extract_obj(obj, FALSE);
      check_improve(ch, gsn_fence, FALSE, 2);
      WAIT_STATE(ch, 12);
      return;
   }

   obj->newcost = amount;
   sprintf(buf, "You fence %s for %d gold.\n\r", obj->short_descr, obj->newcost);
   send_to_char(buf, ch);
   check_improve(ch, gsn_fence, TRUE, 2);
   WAIT_STATE(ch, 6);
   /* cause all dealers to auction it - werv */
   {
      CHAR_DATA * wch;
      char buf[MAX_STRING_LENGTH];
      char obj_name[MAX_STRING_LENGTH];

      for ( wch = char_list; wch != NULL ; wch = wch->next )
      {
         if (wch->in_room == NULL) continue;
         if (!IS_NPC(wch)) continue;
         if (wch->pIndexData->vnum != 9804) continue;
         if (is_affected_obj(obj, gsn_alchemy))
         sprintf(obj_name, "(alchemied) %s", obj->short_descr);
         else if (obj->enchanted)
         sprintf(obj_name, "(enchanted) %s", obj->short_descr);
         else
         sprintf(obj_name, "%s", obj->short_descr);
         switch (number_range(0, 3))
         {
            default:
            case 0:
            sprintf(buf, "%s, best your money can buy, only %d gold!", obj_name, obj->newcost);
            break;
            case 1:
            sprintf(buf, "Perfect condition, top quality, %s for only %d gold!", obj_name, obj->newcost);
            break;
            case 2:
            sprintf(buf, "%s for a mere %d gold coins!", obj_name, obj->newcost);
            break;
            case 3:
            sprintf(buf, "What a bargain!  %s for a mere %d gold!", obj_name, obj->newcost);
         }
         do_auction(wch, buf);
      }
   }

   return;
}

void do_list( CHAR_DATA *ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];

   if ( IS_SET(ch->in_room->room_flags, ROOM_PET_SHOP) )
   {
      ROOM_INDEX_DATA *pRoomIndexNext;
      CHAR_DATA *keeper;
      CHAR_DATA *pet;
      bool found;

      /* hack to make new thalos pets work */
      if (ch->in_room->vnum == 9621)
      pRoomIndexNext = get_room_index(9706);
      else
      pRoomIndexNext = get_room_index( ch->in_room->vnum + 1 );

      if ( pRoomIndexNext == NULL )
      {
         bug( "Do_list: bad pet shop at vnum %d.", ch->in_room->vnum );
         send_to_char( "You can't do that here.\n\r", ch );
         return;
      }

      keeper = find_keeper(ch, argument);
      if ( keeper == NULL )
      {
         send_to_char( "Where is the owner of this petshop eh?\n\r", ch );
         return;
      }

      found = FALSE;
      for ( pet = pRoomIndexNext->people; pet; pet = pet->next_in_room )
      {
         if ( IS_SET(pet->act, ACT_PET) )
         {
            if ( !found )
            {
               found = TRUE;
               send_to_char( "Pets for sale:\n\r", ch );
            }
            sprintf( buf, "[%2d] %8d - %s\n\r",
            pet->level,
            10 * pet->level * pet->level,
            pet->short_descr );
            send_to_char( buf, ch );
         }
      }
      if ( !found )
      send_to_char( "Sorry, we're out of pets right now.\n\r", ch );
      return;
   }
   else
   {
      CHAR_DATA *keeper, *keeper2;
      OBJ_DATA *obj;
      int cost, count;
      bool found=FALSE, outlawdealer=FALSE;

      for (keeper = ch->in_room->people; keeper; keeper = keeper->next_in_room)
      {
         if (IS_NPC(keeper) && keeper->pIndexData->vnum == 9804)
         {
            found = TRUE; outlawdealer=TRUE;
            break;
         }
      }
      if (found)
      {
         found = FALSE;
         for ( keeper = char_list; keeper; keeper = keeper->next)
         {
            if (IS_NPC(keeper)) continue;
            for ( obj = keeper->carrying; obj != NULL; obj = obj->next_content )
            {
               if ( obj->wear_loc == WEAR_NONE
               &&   can_see_obj( ch, obj )
               &&   obj->newcost > 0)
               found = TRUE;
               if (found) break;
            }
            if (found) break;
         }
      } else found = FALSE;

      keeper2 = find_keeper(ch, argument);
      if ( keeper2 == NULL && !found)
      {
         if (outlawdealer)
         send_to_char( "The Underground has nothing to offer at this time.\n\r", ch);
         else
         send_to_char( "You can't do that here.\n\r", ch );
         return;
      }

      if (found && keeper2 == NULL)
      {
         found = FALSE;
         for ( keeper = char_list; keeper != NULL; keeper = keeper->next )
         {
            /* Dealer add . and ^^^^ */
            if (IS_NPC(keeper)) continue; /* Dealer add */
            for ( obj = keeper->carrying; obj != NULL; obj = obj->next_content )
            {
               if ( obj->wear_loc == WEAR_NONE
               &&   can_see_obj( ch, obj )
               &&   can_see_obj( keeper, obj )
               &&   obj->newcost > 0)
               {
                  if (!found)
                  {
                     found = TRUE;
                     send_to_char("The following items are being sold in the underground:\n\r", ch);
                     send_to_char( "[Price] Item\n\r", ch );
                  }
                  sprintf(buf, "[%5d] %s\n\r", obj->newcost, obj->short_descr);
                  send_to_char(buf, ch);
               }
            }
         } /* Dealer add */
         return;
      } else keeper = keeper2;

      /* one_argument(argument, arg); */

      found = FALSE;
      for ( obj = keeper->carrying; obj; obj = obj->next_content )
      {
         if
         (
            obj->wear_loc == WEAR_NONE &&
            can_see_obj(ch, obj) &&
            (
               cost = get_cost(keeper, obj, TRUE)
            ) > 0
            /*
            &&
            (
               arg[0] == '\0' ||
               is_name(arg, obj->name)
            )
            */
         )
         {
            if ( !found )
            {
               found = TRUE;
               send_to_char( "[Lv  Price  Qty] Item\n\r", ch );
            }

            if (IS_OBJ_STAT(obj, ITEM_INVENTORY))
            sprintf(buf, "[%2d %6d  -- ] %s\n\r",
            obj->level, cost, obj->short_descr);
            else
            {
               count = 1;

               while (obj->next_content != NULL
               && obj->pIndexData == obj->next_content->pIndexData
               && !str_cmp(obj->short_descr,
               obj->next_content->short_descr))
               {
                  obj = obj->next_content;
                  count++;
               }
               sprintf(buf, "[%2d %6d  %2d ] %s\n\r",
               obj->level, cost, count, obj->short_descr);
            }
            send_to_char( buf, ch );
         }
      }

      if ( !found )
      {
         if (outlawdealer)
         send_to_char( "The Underground has nothing to offer at this time.\n\r", ch);
         else
         send_to_char( "You can't buy anything here.\n\r", ch );
      }
      return;
   }
}



void do_sell( CHAR_DATA *ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   char arg[MAX_INPUT_LENGTH];
   char from[MAX_INPUT_LENGTH];
   CHAR_DATA *keeper;
   OBJ_DATA *obj, *obj2;
   int cost, roll, count;

   argument = one_argument(argument, arg);
   argument = one_argument(argument, from);

   if ( arg[0] == '\0' )
   {
      send_to_char( "Sell what?\n\r", ch );
      return;
   }

   if
   (
      (
         obj = get_obj_carry(ch, arg, ch)
      ) != NULL &&
      ch->class == CLASS_RANGER &&
      ch->in_room->guild == GUILD_RANGER &&
      obj->pIndexData->vnum == OBJ_VNUM_SACK &&
      obj->item_type == ITEM_TREASURE
   )
   {
      for (keeper = ch->in_room->people; keeper != NULL; keeper = keeper->next_in_room)
      {
         if (!IS_NPC(keeper)) continue;
         if (IS_SET(keeper->act, ACT_PRACTICE) ) break;
      }
      if (keeper != NULL)
      {
         sprintf(buf, "$n purchases your pelt for %d gold coins.", obj->level/2+30);
         act( buf, keeper, NULL, ch, TO_VICT);
         act( "$N purchases a pelt from $n.", ch, NULL, keeper, TO_ROOM);
         ch->gold += obj->level/2 + 30;
         extract_obj(obj, FALSE);
         return;
      }
   }
   if
   (
      (
         obj = get_obj_carry(ch, arg, ch)
      ) != NULL &&
      (
         obj->pIndexData->vnum == OBJ_VNUM_COCKROACH_HEAD_1 ||
         obj->pIndexData->vnum == OBJ_VNUM_COCKROACH_HEAD_2 ||
         obj->pIndexData->vnum == OBJ_VNUM_COCKROACH_HEAD_3 ||
         obj->pIndexData->vnum == OBJ_VNUM_COCKROACH_HEAD_4 ||
         obj->pIndexData->vnum == OBJ_VNUM_COCKROACH_HEAD_5
      )
   )
   {
      for (keeper = ch->in_room->people; keeper != NULL; keeper = keeper->next_in_room)
      {
         if
         (
            IS_NPC(keeper) &&
            (
               keeper->pIndexData->vnum == MOB_VNUM_EXTERMINATOR_1 ||
               keeper->pIndexData->vnum == MOB_VNUM_EXTERMINATOR_2 ||
               keeper->pIndexData->vnum == MOB_VNUM_EXTERMINATOR_3
            )
         )
         {
            break;
         }
      }
      if (keeper != NULL)
      {
         int gold;

         if (!can_see(keeper, ch))
         {
            do_say(keeper, "Huh? Who is there?");
            return;
         }
         if (ch->level <= 10)
         {
            do_say
            (
               keeper,
               "Ooh, he used to be a big creeper, eh? Thanks for your help!"
            );
            gold = 5;
         }
         else if (ch->level <= 20)
         {
            do_say
            (
               keeper,
               "Hmm, not bad, though a bit on the small side."
            );
            gold = 2;
         }
         else
         {
            do_say
            (
               keeper,
               "Didn't take much work, looks like, but a dead bug is a dead bug."
            );
            gold = 1;
         }
         sprintf(buf, "%d gold \"%s\"", gold, get_name(ch, keeper));
         do_give(keeper, buf);
         extract_obj(obj, FALSE);
         return;
      }
   }

   if ( ( keeper = find_keeper( ch, from ) ) == NULL )
   {
      send_to_char( "You can't do that here.\n\r", ch );
      return;
   }

   if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
   {
      act( "$n tells you 'You don't have that item'.",
      keeper, NULL, ch, TO_VICT );
      ch->reply = keeper;
      return;
   }

   if ( !can_drop_obj( ch, obj ) )
   {
      send_to_char( "You can't let go of it.\n\r", ch );
      return;
   }

   if (!can_see_obj(keeper, obj))
   {
      act("$n doesn't see what you are offering.", keeper, NULL, ch, TO_VICT);
      return;
   }
   cost = get_cost(keeper, obj, FALSE);
   cost = UMIN(cost, 95 * get_cost(keeper, obj, TRUE) / 100);
   if (cost <= 0)
   {
      act( "$n looks uninterested in $p.", keeper, obj, ch, TO_VICT );
      return;
   }

   if ( cost > (keeper-> silver + 100 * keeper->gold) )
   {
      act("$n tells you 'I'm afraid I don't have enough wealth to buy $p.",
      keeper, obj, ch, TO_VICT);
      return;
   }

   count=0;
   for (obj2 = keeper->carrying; obj2 != NULL; obj2 = obj2->next_content)
   {
      if (obj2->pIndexData == obj->pIndexData && !str_cmp(obj2->short_descr, obj->short_descr))
      count++;
      if (count >= 5)
      {
         act("$n tells you 'I don't need another one of those.'", keeper, obj, ch, TO_VICT);
         return;
      }
   }

   act( "$n sells $p.", ch, obj, NULL, TO_ROOM );
   /* haggle */
   roll = number_percent();
   if (!IS_OBJ_STAT(obj, ITEM_SELL_EXTRACT) && roll < UMAX(get_skill(ch, gsn_haggle), 2))
   {
      int hag_cost;
      send_to_char("You haggle with the shopkeeper.\n\r", ch);
      hag_cost = cost + cost / (number_range(2, 4));
      /* Prevent overflow (if hag_cost overflows, it will be negative, and ignored) */
      cost = UMAX(cost, hag_cost);
      /*
         Overflow can be ignored, if get_cost * 95 overflows, it will be
         negative above and the shopkeep will be uninterested
      */
      cost = UMIN(cost, get_cost(keeper, obj, TRUE) * 95 / 100);
      if (cost > keeper->silver + 100*keeper->gold)
      {
         cost = keeper->silver + 100*keeper->gold;
      }
      check_improve(ch, gsn_haggle, TRUE, 4);
   }

   sprintf( buf, "You sell $p for %d silver and %d gold piece%s.",
   cost - (cost/100) * 100, cost/100, cost == 1 ? "" : "s" );
   act( buf, ch, obj, NULL, TO_CHAR );
   ch->gold     += cost/100;
   ch->silver          += cost - (cost/100) * 100;
   deduct_cost(keeper, cost);
   if ( keeper->gold < 0 )
   keeper->gold = 0;
   if ( keeper->silver< 0)
   keeper->silver = 0;
   REMOVE_BIT(obj->extra_flags, ITEM_INVENTORY);
   if ( obj->item_type == ITEM_TRASH || IS_OBJ_STAT(obj, ITEM_SELL_EXTRACT))
   {
      extract_obj( obj, FALSE );
   }
   else
   {
      obj_from_char( obj );
      obj_to_keeper( obj, keeper );
   }

   return;
}



void do_value( CHAR_DATA *ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   char arg[MAX_INPUT_LENGTH];
   char from[MAX_INPUT_LENGTH];
   CHAR_DATA *keeper;
   OBJ_DATA *obj;
   int cost;

   argument = one_argument(argument, arg);
   argument = one_argument(argument, from);

   if ( arg[0] == '\0' )
   {
      send_to_char( "Value what?\n\r", ch );
      return;
   }

   if ( ( keeper = find_keeper( ch, from ) ) == NULL )
   {
      send_to_char( "You can't do that here.\n\r", ch );
      return;
   }

   if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
   {
      act( "$n tells you 'You don't have that item'.",
      keeper, NULL, ch, TO_VICT );
      ch->reply = keeper;
      return;
   }

   if (!can_see_obj(keeper, obj))
   {
      act("$n doesn't see what you are offering.", keeper, NULL, ch, TO_VICT);
      return;
   }

   if ( !can_drop_obj( ch, obj ) )
   {
      send_to_char( "You can't let go of it.\n\r", ch );
      return;
   }

   if ( ( cost = get_cost( keeper, obj, FALSE ) ) <= 0 )
   {
      act( "$n looks uninterested in $p.", keeper, obj, ch, TO_VICT );
      return;
   }

   sprintf( buf,
   "$n tells you 'I'll give you %d silver and %d gold coins for $p'.",
   cost - (cost/100) * 100, cost/100 );
   act( buf, keeper, obj, ch, TO_VICT );
   ch->reply = keeper;

   return;
}


void do_request(CHAR_DATA *ch, char *argument)
{
   AFFECT_DATA af;
   CHAR_DATA  *victim;
   OBJ_DATA *obj;
   char i_name[MAX_INPUT_LENGTH];
   char m_name[MAX_INPUT_LENGTH];
   char buf1[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];
   char *target_name;
   bool enforcer_request = FALSE;

   target_name = one_argument(argument, i_name);  /* syntax: request <obj>
   <mob> */
   one_argument(target_name, m_name);

   if (IS_NPC(ch))
   return;     /* NPCs can't request */
   if (i_name[0] == '\0' || m_name[0] == '\0')
   {
      send_to_char("Request what of whom?\n\r", ch);
      return;
   }
   if ((victim = get_char_room(ch, m_name)) == NULL )
   {
      send_to_char("They aren't here.\n\r", ch);
      return;
   }

   if (check_peace(ch))
   {
      return;
   }

   if (!IS_NPC(victim))
   {
      send_to_char("Why not ask for it yourself?\n\r", ch);
      return;
   }

   if
   (
      (
         (
            obj = get_obj_list(victim, i_name, victim->carrying)
         ) != NULL ||
         (
            obj = get_obj_list(ch, i_name, victim->carrying)
         ) != NULL
      )
      &&
      obj->wear_loc == WEAR_BRAND
   )
   {
      mob_punish(victim, ch);
      return;
   }

   if (is_affected(ch, gsn_request))
   {
      send_to_char("You are still in contemplation of the last act of generosity.\n\r", ch);
      return;
   }

   if (victim->pIndexData->pShop != NULL)
   {
      sprintf(buf1, "I have to make a living. Why not just buy something?");
      do_say(victim, buf1);
      return;
   }

   if (victim->in_room->house !=0 ){
      do_say(victim, "I'd rather not.");
      send_to_char("You cannot request from creatures within houses.\n\r", ch);
      return;
   }

   if (victim->pIndexData->vnum == MOB_VNUM_LIGHT)
   {
      sprintf(buf1, "Do not dare to request my belongings!");
      sprintf(buf2, "Help! I'm being attacked by %s!", victim->short_descr);
      do_say(victim, buf1);
      do_yell(ch, buf2);
      multi_hit(victim, ch, TYPE_UNDEFINED);
      return;
   }

   if (ch->house == HOUSE_ENFORCER && victim->spec_fun == spec_lookup("spec_guard"))
   enforcer_request = TRUE;

   if (!IS_GOOD(victim) && !enforcer_request)
   {
      sprintf(buf1, "You dare to ask for my belongings!");
      sprintf(buf2, "Help! I'm being attacked by %s!", victim->short_descr);
      do_say(victim, buf1);
      do_yell(ch, buf2);
      multi_hit(victim, ch, TYPE_UNDEFINED);
      return;
   }

   if (!IS_GOOD(ch) && !enforcer_request)
   {
      sprintf(buf1, "You are unworthy to ask for anything!");
      sprintf(buf2, "Help! I'm being attacked by %s!", victim->short_descr);
      do_say(victim, buf1);
      do_yell(ch, buf2);
      multi_hit(victim, ch, TYPE_UNDEFINED);
      return;
   }

   if ( (obj = get_obj_list(ch, i_name, victim->carrying)) == NULL)
   {
      send_to_char("They do not have that object.\n\r", ch);
      return;
   }
   if (!can_see_obj(ch, obj) )
   {
      send_to_char("They do not have that object.\n\r", ch);
      return;
   }
   if (!can_see(victim, ch))
   {
      act("$N tells you, 'I don't give to those I can't see.'", ch, NULL, victim, TO_CHAR);
      return;
   }

   if (!can_see_obj(victim, obj))
   {
      act("$N tells you, 'I'm afraid I can't see such an object.'", ch, NULL, victim, TO_CHAR);
      return;
   }

   if ((victim->level >= (ch->level + 8)) && victim->level < 58)
   {
      act("$N tells you, 'All in good time my child'.", ch, NULL, victim, TO_CHAR);
      return;
   }
   else if (victim->level >= 59 && ch->level < 51)
   {
      act("$N tells you, 'All in good time my child'.", ch, NULL, victim, TO_CHAR);
      return;
   }

   if (ch->move < obj->level)
   {
      act("$N tells you, 'You look tired, perhaps you should rest first'.", ch, NULL, victim, TO_CHAR);
      return;
   }
   if (ch->hit < (ch->max_hit*3/7))
   {
      act("$N tells you, 'You look worn out, perhaps you should rest and get better first'.", ch, NULL, victim, TO_CHAR);
      return;
   }
   if ((obj->wear_loc != WEAR_NONE) && IS_SET(obj->extra_flags, ITEM_NOREMOVE) )
   {
      act("$N tells you, 'Alas, i am unable to release $p'.", ch, obj, victim, TO_CHAR);
      return;
   }
   if (IS_SET(obj->extra_flags, ITEM_NODROP) )
   {
      act("$N tells you, 'Alas, i am unable to release $p.'", ch, obj, victim, TO_CHAR);
      return;
   }

   if ( ch->carry_weight + get_obj_weight(obj)  > can_carry_w(ch) )
   {
      act("$N tells you, 'You can't carry the weight $n.'", ch, NULL, victim, TO_CHAR);
      return;
   }
   if ( ch->carry_number + 1 > can_carry_n(ch) )
   {
      act("$N tells you, 'You can't carry that many items $n.'", ch, NULL, victim, TO_CHAR);
      return;
   }
   act("$n requests off $N.", ch, NULL, victim, TO_ROOM);
   if (obj->wear_loc != WEAR_NONE)
   {
      act("$n stops using $p.", victim, obj, NULL, TO_ROOM);
      act("You stop using $p.", victim, obj, NULL, TO_CHAR);
   }
   act("$N gives $p to $n.", ch, obj, victim, TO_NOTVICT);
   act("$N gives you $p.", ch, obj, victim, TO_CHAR);

   if (IS_SET(obj->extra_flags, ITEM_ROT_DEATH) /*&& !floating*/ )
   {
      if (obj->pIndexData->condition < 0  &&
      obj->pIndexData->quitouts == FALSE ){
         obj->timer = (-1)*obj->pIndexData->condition;
         REMOVE_BIT(obj->extra_flags, ITEM_ROT_DEATH);
      }
   }
   REMOVE_BIT(obj->extra_flags, ITEM_VIS_DEATH);

   obj_from_char(obj);
   obj_to_char(obj, ch);
   if (enforcer_request)
   {
      act
      (
         "You thank $N heartily for $S kindness to you.",
         ch,
         NULL,
         victim,
         TO_CHAR
       );
   }
   else
   send_to_char("You stop and contemplate the goodness you have seen in the world.\n\r", ch);


   af.where = TO_AFFECTS;
   af.type = gsn_request;
   af.modifier = 0;
   af.duration = 5;
   af.location = 0;
   af.bitvector = 0;
   af.level = ch->level;
   affect_to_char(ch, &af);

   ch->move -= obj->level;
   ch->hit -= obj->level;
   return;
}




bool hands_full(CHAR_DATA *ch)
{
   int count;

   count = 0;

   if (get_eq_char(ch, WEAR_LIGHT) != NULL)
   count++;
   if (get_eq_char(ch, WEAR_WIELD) != NULL)
   count++;
   if (get_eq_char(ch, WEAR_HOLD) != NULL)
   count++;
   if (get_eq_char(ch, WEAR_SHIELD) != NULL)
   count++;
   if (get_eq_char(ch, WEAR_DUAL_WIELD) != NULL)
   count++;
   if (get_eq_char(ch, WEAR_TERTIARY_WIELD) != NULL)
   count++;

   if (ch->morph_form[0] == MORPH_ARM)
   count -= 1;

   if (count < 2)
   return FALSE;
   if (count > 2)
   bug("Hands full: Character holding %d items.", count);

   return TRUE;

}


void do_battleshield(CHAR_DATA *ch, char *argument)
{
   AFFECT_DATA af;
   AFFECT_DATA paf;

   OBJ_DATA *shield;
   int mod;

   if ( (get_skill(ch, gsn_battleshield) <= 0)
   || /*ch->level < skill_table[gsn_battleshield].skill_level[ch->class]*/
   !has_skill(ch, gsn_battleshield))
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }

   if (is_affected(ch, gsn_battleshield))
   {
      send_to_char("You aren't up to forging a new shield yet.\n\r", ch);
      return;
   }

   if (ch->mana < 60)
   {
      send_to_char("You don't have the mana.\n\r", ch);
      return;
   }

   if (number_percent() > get_skill(ch, gsn_battleshield))
   {
      send_to_char("You fail to forge a new battleshield.\n\r", ch);
      ch->mana -= 30;
      return;
   }
   ch->mana -= 60;

   shield = create_object(get_obj_index(OBJ_VNUM_BATTLESHIELD), ch->level);
   shield->level = ch->level;
   shield->value[0] = 8 + ch->level/10;
   shield->value[1] = 8 + ch->level/10;
   shield->value[2] = 8 + ch->level/10;
   shield->value[3] = 8 + ch->level/10;
   shield->value[4] = 8 + ch->level/10;

   af.where = TO_AFFECTS;
   af.type = gsn_battleshield;
   af.level = ch->level;
   af.duration = 24;
   af.bitvector = 0;
   af.modifier = 0;
   af.location = 0;
   affect_to_char(ch, &af);

   paf.where = TO_OBJECT;
   af.type = gsn_battleshield;
   af.level = ch->level;
   af.duration = -1;
   af.bitvector = 0;
   af.location = APPLY_DAMROLL;

   if (ch->level < 30)
   mod = 1;
   else if (ch->level < 40)
   mod = 2;
   else if (ch->level < 50)
   mod = 3;
   else
   mod = 4;

   af.modifier = 2 + mod;
   af.location = APPLY_DAMROLL;
   affect_to_obj(shield, &af);
   af.location = APPLY_HITROLL;
   affect_to_obj(shield, &af);

   af.location = APPLY_HIT;
   af.modifier = ch->level;
   affect_to_obj(shield, &af);

   af.location = APPLY_MOVE;
   affect_to_obj(shield, &af);

   af.location = APPLY_SAVES;
   af.modifier = -2*mod;
   affect_to_obj(shield, &af);

   obj_to_char(shield, ch);
   act("You forge $p!", ch, shield, NULL, TO_CHAR);
   act("$n forges $p!", ch, shield, NULL, TO_ROOM);
   return;
}

void do_embalm(CHAR_DATA *ch, char *argument)
{
   OBJ_DATA *corpse;
   char arg[100];

   if ( (get_skill(ch, gsn_embalm) <= 0)
   || /*ch->level < skill_table[gsn_embalm].skill_level[ch->class]*/
   !has_skill(ch, gsn_embalm))
   {
      send_to_char("You don't know how to embalm corpses.\n\r", ch);
      return;
   }
   one_argument(argument, arg);
   if (arg[0] == '\0')
   {
      send_to_char("Embalm what?\n\r", ch);
      return;
   }

   if ( (corpse = get_obj_here(ch, arg)) == NULL)
   {
      send_to_char("You don't have that item to embalm.\n\r", ch);
      return;
   }
   if (corpse->item_type != ITEM_CORPSE_NPC)
   {
      act("$p isn't a corpse you can embalm.\n\r", ch, corpse, NULL, TO_CHAR);
      return;
   }
   if (corpse->value[4] == 1)
   {
      send_to_char("That corpse has already been embalmed.\n\r", ch);
      return;
   }

   if (!IS_NPC(ch) && (number_percent() > ch->pcdata->learned[gsn_embalm]))
   {
      act("You spill your embalming fluids all over $p, ruining it.", ch, corpse, NULL, TO_CHAR);
      act("$n spills embalming fuilds over $p and ruins it.", ch, corpse, NULL, TO_ROOM);
      check_improve(ch, gsn_embalm, FALSE, 4);
      extract_obj(corpse, FALSE);
      return;
   }
   act("$n embalms $p!", ch, corpse, NULL, TO_ROOM);
   act("You succeed in embalming $p.", ch, corpse, NULL, TO_CHAR);
   corpse->timer += ch->level/2;
   corpse->value[4] = 1;
   check_improve(ch, gsn_embalm, TRUE, 4);
   return;
}

void house_shudder(int house)
{
   CHAR_DATA *ch;
   for (ch = char_list; ch != NULL; ch = ch->next)
   {
      if (ch->house == house)
      send_to_char("You feel a shudder in your House power!\n\r", ch);
   }
   return;
}

bool house_down(CHAR_DATA *ch, int house)
{
   return house_down_1(ch, house, FALSE);
}

bool house_down_1(CHAR_DATA* ch, int house, bool check)
{
   OBJ_DATA* obj;
   OBJ_INDEX_DATA* index;
   bool is_down;
   int cnt;
   register sh_int vnum;

   is_down = FALSE;

   if
   (
      house < 0 ||
      house >= MAX_HOUSE
   )
   {
      bug("house_down_1 house out of range: [%d]", house);
      return FALSE;
   }
   vnum = house_table[house].vnum_item;
   if
   (
      (
         index = get_obj_index(vnum)
      ) != NULL &&
      index->in_game_count > 0
   )
   {
      for (obj = object_list; obj != NULL; obj = obj->next)
      {
         if (obj->pIndexData->vnum == vnum)
         {
            break;
         }
      }
   }
   else
   {
      obj = NULL;
   }

   if
   (
      ch != NULL &&
      !IS_IMMORTAL(ch) &&
      !IS_NPC(ch) &&
      ch->house != house
   )
   {
      send_to_char("You cannot find the house power within you.\n\r", ch);
      return TRUE;
   }

   if (obj == NULL)
   {
      return FALSE;
   }

   if
   (
      obj->in_room == NULL ||
      obj->carried_by != NULL
   )
   {
      return FALSE;
   }

   is_down = FALSE;
   for (cnt = 0; cnt < MAX_HOUSE; cnt++)
   {
      if
      (
         obj->in_room->vnum == house_table[cnt].vnum_altar &&
         cnt != house
      )
      {
         is_down = TRUE;
      }
   }

   if (ch == NULL)
   {
      if (check)
      {
         return is_down;
      }
      if
      (
         is_down &&
         house_member_on(house)
      )
      {
         if (house == HOUSE_ENFORCER)
         {
            for (ch = char_list; ch; ch = ch->next)
            {
               if
               (
                  IS_NPC(ch) ||
                  !ch->on_line ||
                  ch->house != HOUSE_ENFORCER
               )
               {
                  continue;
               }
               ch->pcdata->targetkills -= 2;
               if (ch->pcdata->targetkills < 0)
               {
                  ch->pcdata->targetkills = 0;
               }
            }
         }
         house_downs[house]++;
         save_globals();
      }
      return TRUE;
   }

   if (house_table[house].powers_on == FALSE)
   {
      is_down = FALSE;
   }

   if (is_down)
   {
      send_to_char("You can not find the House power within you.\n\r", ch);
   }

   if (IS_IMMORTAL(ch))
   {
      is_down = FALSE;
   }

   return is_down;
}

bool cant_carry( CHAR_DATA *ch, OBJ_DATA *obj )
{
   bool status;
   status = FALSE;

   if ( IS_NPC(ch) )
   {
      if ( obj->pIndexData->house != 0 )
      status = TRUE;
   }

   return status;
}


bool is_restricted(CHAR_DATA *ch, OBJ_DATA *obj)
{
   int restrict;
   bool status;
   int race;

   if (IS_NPC(ch))
   return FALSE;

   if (str_cmp(obj->owner, "none") && str_cmp(obj->owner, ch->name))
   return TRUE;

   race = ch->race;
   restrict = obj->pIndexData->restrict_flags;

   status = FALSE;

   if (obj->pIndexData->vnum == 2732)
   {
      OBJ_DATA *bobj;
      for (bobj=ch->carrying; bobj != NULL; bobj = bobj->next_content)
      {
         if (((bobj->wear_loc == WEAR_WRIST_L) || (bobj->wear_loc == WEAR_WRIST_R))
         && (bobj->pIndexData->vnum == 2732))
         {
            send_to_char("You may only wear one of these bracers.\n\r", ch);
            return TRUE;
         }
      }
   }

   if ((obj->pIndexData->vnum == 27513) &&
   ((ch->class == CLASS_WARRIOR) ||
   (ch->class == CLASS_THIEF) ||
   (ch->class == CLASS_RANGER) ||
   (ch->class == CLASS_MONK)))
   {
      send_to_char("You do not have enough magic within you.\n\r", ch);
      return TRUE;
   } else
   if (obj->pIndexData->vnum == 27513)
   {
      SET_BIT(obj->extra_flags, ITEM_NOREMOVE);
      send_to_char("Oops. You feel your mind being slowly unraveled.\n\r", ch);
      switch (number_range(0, 2))
      {
         case 0: send_to_char("The last thing you remember is the word 'punish'.\n\r", ch); break;
         case 1: send_to_char("The last thing you remember is the word 'forget'.\n\r", ch); break;
         case 2: send_to_char("The last thing you remember is the word 'cantrips'.\n\r", ch);
      }
   }

   /*    if (!str_cmp(race, "changeling"))
   {
   if (IS_SET(obj->wear_flags, ITEM_WEAR_BODY))
   {
   send_to_char("Your race does not wear full body items.\n\r", ch);
   return TRUE;
   }
   } */

   /*  Racial restrictions -- Drinlinda */

   /* Minotaur, Centaur*/

   if (ch->race == grn_minotaur)  /* Is Minotaur */
   {
      if
      (
         IS_SET(obj->wear_flags, ITEM_WEAR_HEAD) &&
         !IS_SET(obj->wear_flags, ITEM_WEAR_HORNS)
      )
      {
         send_to_char("Your bovine head prohibits you from wearing this.\n\r", ch);
         return TRUE;
      }
      if
      (
         (
            IS_SET(obj->wear_flags, ITEM_WEAR_FEET) ||
            IS_SET(obj->wear_flags, ITEM_WEAR_FOURHOOVES)
         ) &&
         !IS_SET(obj->wear_flags, ITEM_WEAR_HOOVES)
      )
      {
         send_to_char("Your hooves don't fit in those.\n\r", ch);
         return TRUE;
      }
   }
   else  /* Not Minotaur */
   {
      if
      (
         IS_SET(obj->wear_flags, ITEM_WEAR_HORNS) &&
         !IS_SET(obj->wear_flags, ITEM_WEAR_HEAD)
      )
      {
         send_to_char("You do not have the horns of a minotaur and thus cannot wear this.\n\r", ch);
         return TRUE;
      }
      if
      (
         (
            IS_SET(obj->wear_flags, ITEM_WEAR_HOOVES)
         ) &&
         (
            (
               ch->race == grn_centaur &&
               !IS_SET(obj->wear_flags, ITEM_WEAR_FOURHOOVES)
            ) ||
            (
               ch->race != grn_centaur &&
               !IS_SET(obj->wear_flags, ITEM_WEAR_FEET)
            )
         )
      )
      {
         send_to_char("You require the hooves of a minotaur to wear this.\n\r", ch);
         return TRUE;
      }
      if (IS_SET(obj->wear_flags, ITEM_WEAR_SNOUT))
      {
         send_to_char("You don't have a snout to wear that on.\n\r", ch);
         return TRUE;
      }
   }
   if
   (
      ch->race != grn_book &&
      CAN_WEAR(obj, ITEM_WEAR_INSIDE_COVER)
   )
   {
      send_to_char("Only books have covers.\n\r", ch);
      return TRUE;
   }
   if (ch->race == grn_centaur)
   {
      if
      (
         (
            IS_SET(obj->wear_flags, ITEM_WEAR_FEET) ||
            IS_SET(obj->wear_flags, ITEM_WEAR_HOOVES)
         ) &&
         !IS_SET(obj->wear_flags, ITEM_WEAR_FOURHOOVES)
      )
      {
         send_to_char("Your hooves don't fit in those.\n\r", ch);
         return TRUE;
      }
      if
      (
         IS_SET(obj->wear_flags, ITEM_WEAR_LEGS) &&
         !IS_SET(obj->wear_flags, ITEM_WEAR_FOURLEGS)
      )
      {
         send_to_char("You can't manage to fit all four legs into that.\n\r", ch);
         return TRUE;
      }
   }
   else  /* Not Centaur */
   {
      if
      (
         (
            IS_SET(obj->wear_flags, ITEM_WEAR_FOURHOOVES)
         ) &&
         (
            (
               ch->race == grn_minotaur &&
               !IS_SET(obj->wear_flags, ITEM_WEAR_HOOVES)
            ) ||
            (
               ch->race != grn_minotaur &&
               !IS_SET(obj->wear_flags, ITEM_WEAR_FEET)
            )
         )
      )
      {
         send_to_char("You need to have four hooves to wear those.\n\r", ch);
         return TRUE;
      }
      if
      (
         IS_SET(obj->wear_flags, ITEM_WEAR_FOURLEGS) &&
         !IS_SET(obj->wear_flags, ITEM_WEAR_LEGS)
      )
      {
         send_to_char("You need to have four legs to wear those.\n\r", ch);
         return TRUE;
      }
   }


   if ( IS_SET(restrict, MAGE_ONLY) )
   {
      if (ch->class != CLASS_CLERIC
      && ch->class != CLASS_CHANNELER && ch->class != CLASS_NECROMANCER
      && ch->class != CLASS_ELEMENTALIST)
      status = TRUE;
   }

   /* RACE code */
   if
   (
      (
         (restrict & any_race_r) ||
         (obj->extra_flags2 & any_race_e2)
      ) &&
      !IS_SET(restrict, pc_race_table[ch->race].restrict_r_bit) &&
      !IS_SET(obj->extra_flags2, pc_race_table[ch->race].restrict_e2_bit)
   )
   {
      /* At least one race_only flag set, but not ours */
      /*
      send_to_char("You cannot fit into that properly.\n\r", ch);
      */
      return TRUE;
   }
   /* CLASS code */
   if
   (
      (
         (restrict & any_class_r) ||
         (obj->extra_flags2 & any_class_e2)
      ) &&
      !IS_SET(restrict, class_table[ch->class].restrict_r_bit) &&
      !IS_SET(obj->extra_flags2, class_table[ch->class].restrict_e2_bit)
   )
   {
      /* At least one class_only flag set, but not ours */
      /*
      send_to_char("You cannot fit into that properly.\n\r", ch);
      */
      return TRUE;
   }
   /* HOUSE code */
   if
   (
      (
         (restrict & any_house_r) ||
         (obj->extra_flags2 & any_house_e2)
      ) &&
      !IS_SET(restrict, house_table[ch->house].restrict_r_bit) &&
      !IS_SET(obj->extra_flags2, house_table[ch->house].restrict_e2_bit)
   )
   {
      /* At least one house_only flag set, but not ours */
      /*
      send_to_char("You cannot fit into that properly.\n\r", ch);
      */
      return TRUE;
   }
   /* SIZE code */
   if
   (
      (
         obj->extra_flags2 & any_size_e2
      ) &&
      !IS_SET
      (
         obj->extra_flags2,
         size_table[URANGE(SIZE_MINIMUM, ch->size, SIZE_MAXIMUM)].restrict_e2_bit
      )
   )
   {
      status = TRUE;
   }

   if
   (
      (
         IS_SET(obj->extra_flags2, FEMALE_ONLY) &&
         ch->pcdata->true_sex != SEX_FEMALE
      ) ||
      (
         IS_SET(obj->extra_flags2, MALE_ONLY) &&
         ch->pcdata->true_sex != SEX_MALE
      ) ||
      (
         IS_SET(obj->extra_flags2, GENDERLESS_ONLY) &&
         ch->pcdata->true_sex != SEX_NEUTRAL
      )
   )
   {
      status = TRUE;
   }


   return status;
}

void do_lore(CHAR_DATA *ch, char *argument)
{
   OBJ_DATA *obj=NULL;
   char arg[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   AFFECT_DATA *paf;
   int chance = 0;

   one_argument(argument, arg);

   if ( ((chance = get_skill(ch, gsn_lore)) <= 0)
   || /* ch->level < skill_table[gsn_lore].skill_level[ch->class] */
   !has_skill(ch, gsn_lore))
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }


   if (house_down(ch, HOUSE_CRUSADER))
   return;

   if (ch->mana < 30)
   {
      send_to_char("You don't have the mana.\n\r", ch);
      return;
   }

   obj = get_obj_carry(ch, arg, ch);

   if (obj == NULL)
   {
      send_to_char( "You do not have that item.\n\r", ch );
      return;
   }

   if (number_percent() > chance)
   {
      switch (number_range(1, 5))
      {
         case 1: send_to_char("You think it might be a chicken.\n\r", ch);
         break;
         case 2: send_to_char("You think it might be a duck.\n\r", ch);
         break;
         case 3: send_to_char("You are not sure WHAT to make of it.\n\r", ch);
         break;
         case 4: send_to_char("It's, uh, er, a whatchamacallit.\n\r", ch);
         break;
         case 5: send_to_char("Its nature is on the tip of your tongue.\n\r", ch);
      }
      check_improve(ch, gsn_lore, FALSE, 1);
      ch->mana -= 15;
      WAIT_STATE(ch, 12);
      return;
   }

   ch->mana -= 30;
   check_improve(ch, gsn_lore, TRUE, 1);

   if (obj->level - 3 > ch->level)
   {
      sprintf( buf,
      "Object '%s' is type %s.\n\rWeight is %d, value is %d.\n\r",
      obj->short_descr,
      item_name(obj->item_type),
      obj->weight / 10,
      obj->cost);
      send_to_char(buf, ch);
      return;
   }

   sprintf
   (
      buf,
      "Object '%s' is type %s.\n\rWeight is %d, value is %d, level is %d.\n\r",
      obj->short_descr,
      item_name(obj->item_type),
      /*        extra_bit_name( obj->extra_flags ), */
      obj->weight / 10,
      obj->cost,
      obj->level
   );
   send_to_char( buf, ch );

   switch ( obj->item_type )
   {
      case ITEM_SCROLL:
      case ITEM_POTION:
      case ITEM_HERB:
      case ITEM_PILL:
      if (obj->pIndexData->vnum == OBJ_VNUM_BERRY) break;
      sprintf( buf, "Yuck! It is a mage construct!\n\r");
      send_to_char( buf, ch );
      break;

      case ITEM_WAND:
      case ITEM_STAFF:
      /*        sprintf( buf, "Has %d charges of level %d",
      obj->value[2], obj->value[0] );*/
      sprintf( buf, "Yuck! It is a mage construct!\n\r");
      send_to_char( buf, ch );
      break;

      case ITEM_DRINK_CON:
      sprintf(buf, "It holds %s-colored %s.\n\r",
      liq_table[obj->value[2]].liq_color,
      liq_table[obj->value[2]].liq_name);
      send_to_char(buf, ch);
      break;

      case ITEM_CONTAINER:
      send_to_char("You can hold things in it.\n\r", ch);
      break;

      case ITEM_WEAPON:
      send_to_char("Weapon type is ", ch);
      switch (obj->value[0])
      {
         case(WEAPON_EXOTIC) : send_to_char("exotic.\n\r", ch);       break;
         case(WEAPON_BOW)        : send_to_char("bow.\n\r", ch);                break;
         case(WEAPON_ARROWS) : send_to_char("arrows.\n\r", ch);        break;
         case(WEAPON_SWORD)  : send_to_char("sword.\n\r", ch);        break;
         case(WEAPON_DAGGER) : send_to_char("dagger.\n\r", ch);       break;
         case(WEAPON_SPEAR)  : send_to_char("spear/staff.\n\r", ch);  break;
         case(WEAPON_MACE)   : send_to_char("mace/club.\n\r", ch);    break;
         case(WEAPON_AXE)    : send_to_char("axe.\n\r", ch);          break;
         case(WEAPON_FLAIL)  : send_to_char("flail.\n\r", ch);        break;
         case(WEAPON_WHIP)   : send_to_char("whip.\n\r", ch);         break;
         case(WEAPON_POLEARM): send_to_char("polearm.\n\r", ch);      break;
         case (WEAPON_STAFF) : send_to_char("staff.\n\r", ch);    break;
         default             : send_to_char("unknown.\n\r", ch);      break;
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
      /*        if (obj->value[4])
      {
      sprintf(buf, "Weapons flags: %s\n\r", weapon_bit_name(obj->value[4]));
      send_to_char(buf, ch);

      }*/
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
      /*        if ( paf->location != APPLY_NONE && paf->modifier != 0 )*/
      if ( paf->modifier != 0 && (paf->location == APPLY_HITROLL ||
      paf->location == APPLY_DAMROLL || paf->location == APPLY_AC))
      {
         sprintf( buf, "Affects %s by %d.\n\r",
         affect_loc_name( paf->location ), paf->modifier );
         send_to_char(buf, ch);
         if (paf->bitvector || paf->bitvector2)
         {
            switch (paf->where)
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
      /*        if ( paf->location != APPLY_NONE && paf->modifier != 0 )*/
      if ( paf->modifier != 0 && (paf->location == APPLY_HITROLL ||
      paf->location == APPLY_DAMROLL || paf->location == APPLY_AC))
      {
         sprintf( buf, "Affects %s by %d",
         affect_loc_name( paf->location ), paf->modifier );
         send_to_char( buf, ch );
         if ( paf->duration > -1)
         sprintf(buf,", %d hours.\n\r", paf->duration);
         else
         sprintf(buf, ".\n\r");
         send_to_char(buf, ch);
         if
         (
            paf->bitvector ||
            paf->bitvector2
         )
         {
            switch (paf->where)
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

   WAIT_STATE(ch, 24);

   return;
}

void do_trophy(CHAR_DATA *ch, char *argument)
{
   AFFECT_DATA /*af,*/ *paf;
   char arg[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   char *name;
   /*    char *last_name;    */
   OBJ_DATA *obj;
   OBJ_DATA *poncho;
   int chance;
   int vnum;

   one_argument(argument, arg);

   if ( ((chance = get_skill(ch, gsn_trophy)) <= 0) || !has_skill(ch, gsn_trophy))
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }

   if (house_down(ch, HOUSE_CRUSADER))
   return;

   if (is_affected(ch, gsn_trophy))
   {
      send_to_char("You don't feel up to making a new trophy yet.\n\r", ch);
      return;
   }
   if (ch->mana < 30)
   {
      send_to_char("You don't have the mana.\n\r", ch);
      return;
   }

   obj = get_obj_carry(ch, arg, ch);

   if (obj == NULL || arg[0] == '\0')
   {
      send_to_char("Make a trophy from what body part?\n\r", ch);
      return;
   }

   vnum = obj->pIndexData->vnum;
   if (vnum != OBJ_VNUM_SEVERED_HEAD
   && vnum != OBJ_VNUM_TORN_HEART
   && vnum != OBJ_VNUM_SLICED_ARM
   && vnum != OBJ_VNUM_SLICED_LEG
   && vnum != OBJ_VNUM_GUTS
   && vnum != OBJ_VNUM_BRAINS)
   {
      send_to_char("You can't make a trophy from that.\n\r", ch);
      return;
   }

   if (number_percent() > chance)
   {
      act("$n tries to make a trophy from $p but destroys it.", ch, obj, NULL, TO_ROOM);
      send_to_char("You fail and destroy it.\n\r", ch);
      check_improve(ch, gsn_trophy, FALSE, 1);
      ch->mana -= 15;
      extract_obj(obj, FALSE);
      return;
   }

   act("$n makes a poncho from $p.", ch, obj, NULL, TO_ROOM);
   act("You make a bloody trophy from $p.", ch, obj, NULL, TO_CHAR);
   ch->mana -= 30;
   poncho = create_object(get_obj_index(OBJ_VNUM_PONCHO), 0);

   name = strstr( obj->short_descr, "of ");
   name = name + 3;

   poncho->level = ch->level;
   sprintf(buf, "a poncho from the carcass of %s", name);
   free_string(poncho->short_descr);
   poncho->short_descr = str_dup(buf);
   sprintf(buf, "A blood stained poncho from the remains of %s lies here.", name);
   free_string(poncho->description);
   poncho->description = str_dup(buf);
   poncho->timer = 58;

   paf = new_affect();
   paf->where      = TO_OBJECT;
   paf->type       = gsn_trophy;
   paf->level      = ch->level;
   paf->duration   = -1;
   paf->location   = APPLY_AC;
   paf->modifier   = -7*(ch->level/10);
   paf->bitvector  = 0;
   paf->next       = poncho->affected;
   poncho->affected   = paf;
   paf = new_affect();
   paf->where      = TO_OBJECT;
   paf->type       = gsn_trophy;
   paf->level      = ch->level;
   paf->duration   = -1;
   paf->location   = APPLY_HIT;
   paf->modifier   = 25*(ch->level/10);
   paf->bitvector  = 0;
   paf->next       = poncho->affected;
   poncho->affected   = paf;
   paf = new_affect();
   paf->where      = TO_OBJECT;
   paf->type       = gsn_trophy;
   paf->level      = ch->level;
   paf->duration   = -1;
   paf->location   = APPLY_HITROLL;
   paf->modifier   = 2*(ch->level/10);
   paf->bitvector  = 0;
   paf->next       = poncho->affected;
   poncho->affected   = paf;
   paf = new_affect();
   paf->where      = TO_OBJECT;
   paf->type       = gsn_trophy;
   paf->level      = ch->level;
   paf->duration   = -1;
   paf->location   = APPLY_DAMROLL;
   paf->modifier   = 2*(ch->level/10);
   paf->bitvector  = 0;
   paf->next       = poncho->affected;
   poncho->affected   = paf;

   obj_to_char(poncho, ch);
   extract_obj(obj, FALSE);

   /*    af.where = TO_AFFECTS;
   af.type = gsn_trophy;
   af.duration = 60;
   af.location = 0;
   af.bitvector = 0;
   af.modifier = 0;
   af.level = ch->level;
   affect_to_char(ch, &af); */

   check_improve(ch, gsn_trophy, TRUE, 1);
   return;
}

void wear_obj_sword_demon(CHAR_DATA *ch, OBJ_DATA *obj)
{
   send_to_char("You hear the laughter of demons!\n\r", ch);
   SET_BIT(obj->extra_flags, ITEM_NOREMOVE);

   return;
}

void wear_obj_bracers(CHAR_DATA *ch, OBJ_DATA *obj) /* strange bracers */
{
   AFFECT_DATA af;
   int sn_haste;

   sn_haste = gsn_haste;
   affect_strip(ch, sn_haste);
   af.type = sn_haste;
   af.level = 51;
   af.modifier = 3;
   af.location = APPLY_DEX;
   af.duration = -1;
   af.bitvector = AFF_HASTE;
   af.where = TO_AFFECTS;
   affect_to_char(ch, &af);
   send_to_char("Your arms feel lighter and free of constraints.\n\r", ch);
   send_to_char("You feel yourself moving faster!\n\r", ch);
   act("$n appears to be moving faster.", ch, NULL, NULL, TO_ROOM);
   return;
}

void wear_obj_elven_armguards(CHAR_DATA *ch, OBJ_DATA *obj) /* elven-forged light mithril armguards */
{
   send_to_char("You feel more balanced and able to move more fluidly.\n\r", ch);
   return;
}

void wear_obj_genocide(CHAR_DATA *ch, OBJ_DATA *obj)
{
   if (IS_NPC(ch)) return;
   if (!IS_SET(obj->carried_by->comm, COMM_ANSI)){
      act( "$p tells you 'You are mine now, together we will destroy all.'",
      ch, obj, NULL, TO_CHAR );
   }
   else
   {
      act( "$p tells you '\x01b[1;32mYou are mine now, together we will destroy all.\x01b[0;37m'", ch, obj, NULL, TO_CHAR);
   }
   send_to_char("You feel compelled to do the bidding of the axe.\n\r", ch);
   return;
}

void remove_obj_genocide(CHAR_DATA *ch, OBJ_DATA *obj)
{
   if (IS_NPC(ch)) return;
   send_to_char("You feel the darkness leave your body.\n\r", ch);
   return;
}

void wear_obj_girdle(CHAR_DATA *ch, OBJ_DATA *obj)  /* strange girdle */
{
   AFFECT_DATA af;

   if (IS_NPC(ch))
   return;

   if (ch->morph_form[0] > 1)
   {
      send_to_char("Your your body rejects this item!\n\r", ch);
      return;
   }

   af.where = TO_AFFECTS;
   af.type = gsn_strange_form;
   af.level = 51;
   af.location = APPLY_MORPH_FORM;
   af.duration = -1;
   af.bitvector = 0;
   act("$n screams in agony as $s face contorts and twists!", ch, NULL, NULL, TO_ROOM);
   send_to_char("You feel an intense pain in your skull as your face twists and shifts!\n\r", ch);

   switch (number_range(0, 2))
   {
      case (0):
      af.modifier = 8;     /* dragon */
      affect_to_char(ch, &af);
      break;
      case (1):
      af.modifier = 6;    /* goat */
      affect_to_char(ch, &af);
      skill_gain(ch, "ram", -1);
      ch->pcdata->learned[gsn_chimera_goat] = 100;
      break;
      case (2):
      af.modifier = 7;    /* lion */
      affect_to_char(ch, &af);
      skill_gain(ch, "bite", -1);
      ch->pcdata->learned[gsn_chimera_lion] = 100;
      break;
   }
   send_to_char("You suddenly feel strangely different.\n\r", ch);
   return;
}

void remove_obj_bracers(CHAR_DATA *ch, OBJ_DATA *obj) /* strange bracers */
{
   int sn_haste;
   sn_haste = gsn_haste;

   if (!is_affected(ch, sn_haste))
   return;
   affect_strip(ch, sn_haste);

   act("$n appears to be moving slower.", ch, NULL, NULL, TO_ROOM);
   send_to_char("Your arms suddenly feel much heavier.\n\r", ch);
   send_to_char("You are moving slower.\n\r", ch);
   return;
}

void remove_obj_elven_armguards(CHAR_DATA *ch, OBJ_DATA *obj) /* elven-forged light mithril armguards */
{
   send_to_char("Your arms suddenly feel heavier, less in balance.\n\r", ch);
   send_to_char("You are moving less fluidly than before you removed those armguards.\n\r", ch);
   return;
}

void remove_obj_girdle(CHAR_DATA *ch, OBJ_DATA *obj)
{
   if (!is_affected(ch, gsn_strange_form))
   return;
   act("$n's face twists and contorts.", ch, NULL, NULL, TO_ROOM);
   send_to_char("You feel your face melt back into its normal shape.\n\r", ch);
   affect_strip(ch, gsn_strange_form);
   ch->pcdata->learned[gsn_chimera_lion] = -1;
   ch->pcdata->learned[gsn_chimera_goat] = -1;
   return;
}

void do_demand(CHAR_DATA *ch, char *argument)
{
   CHAR_DATA  *victim;
   OBJ_DATA *obj;
   char i_name[MAX_INPUT_LENGTH];
   char m_name[MAX_INPUT_LENGTH];
   char buf1[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];
   char *target_name;
   int chance, vlevel;

   target_name = one_argument(argument, i_name);
   one_argument(target_name, m_name);

   chance = IS_NPC(ch) ? 100 : get_skill(ch, gsn_demand);
   if (chance == 0
   || /* ch->level < skill_table[gsn_demand].skill_level[ch->class] */
   !has_skill(ch, gsn_demand))
   {
      send_to_char("You are hardly intimidating enough to demand off others.\n\r", ch);
      return;
   }

   if (IS_NPC(ch))
   return;
   if ((victim = get_char_room(ch, m_name)) == NULL )
   {
      send_to_char("They aren't here.\n\r", ch);
      return;
   }
   if (!IS_NPC(victim))
   {
      send_to_char("Why not just threaten them in person?\n\r", ch);
      return;
   }

   if
   (
      (
         (
            obj = get_obj_list(victim, i_name, victim->carrying)
         ) != NULL ||
         (
            obj = get_obj_list(ch, i_name, victim->carrying)
         ) != NULL
      )
      &&
      obj->wear_loc == WEAR_BRAND
   )
   {
      mob_punish(victim, ch);
      return;
   }

   vlevel = victim->level;
   chance /= 2;
   chance += (3*ch->level);
   /*    chance -= 2*vlevel; */

   if (IS_GOOD(victim))
   chance -= 4*vlevel;
   else if (IS_EVIL(victim))
   chance -= 2*vlevel;
   else
   chance -= 3*vlevel;

   if (vlevel > (ch->level-8)) chance -= chance/8;
   if (vlevel > (ch->level-4)) chance -= chance/3;
   if (vlevel > ch->level) chance = 0;

   if ((obj = get_obj_list(victim, i_name, victim->carrying)) == NULL)
   {
      send_to_char("They do not have that object.\n\r", ch);
      return;
   }
   if (!can_see_obj(ch, obj) )
   {
      send_to_char("They do not have that object.\n\r", ch);
      return;
   }

   if (number_percent() > chance)
   {
      check_improve(ch, gsn_demand, FALSE, 2);
      sprintf(buf1, "I don't think i'd give my belongings to one as weak as you!");
      sprintf(buf2, "Help! I'm being attacked by %s!", victim->short_descr);
      do_say(victim, buf1);
      do_yell(ch, buf2);
      multi_hit(victim, ch, TYPE_UNDEFINED);
      return;
   }

   if (!can_see(victim, ch))
   {
      act("$N tells you, 'I can't give to those i can't see.'", ch, NULL, victim, TO_CHAR);
      return;
   }

   if (!can_see_obj(victim, obj))
   {
      act("$N tells you, 'I can't see such an object.'", ch, NULL, victim, TO_CHAR);
      return;
   }

   if (obj->level > (ch->level + 8))
   {
      do_say(victim, "That item is far to precious to hand over to scum like you!");
      sprintf(buf1, "Help! I'm being attacked by %s!", victim->short_descr);
      do_yell(ch, buf1);
      multi_hit(victim, ch, TYPE_UNDEFINED);
      return;
   }

   if (ch->move < obj->level)
   {
      act("$N tells you, 'Hah! You couldn't even get away if i chased you!'.", ch, NULL, victim, TO_CHAR);
      sprintf(buf1, "Help! I'm being attacked by %s!", victim->short_descr);
      do_yell(ch, buf1);
      multi_hit(victim, ch, TYPE_UNDEFINED);
      return;
   }

   if (ch->hit < (ch->max_hit*3/7))
   {
      do_say(victim, "Hah! You look weak enough that even I could kill you!");
      sprintf(buf1, "Help! I'm being attacked by %s!", victim->short_descr);
      do_yell(ch, buf1);
      multi_hit(victim, ch, TYPE_UNDEFINED);
      return;
   }

   if (ch->mana < 30)
   {
      send_to_char("You don't have the mana.\n\r", ch);
      return;
   }

   if ((obj->wear_loc != WEAR_NONE) && IS_SET(obj->extra_flags, ITEM_NOREMOVE) )
   {
      act("$N tells you, 'I'm unable to release $p'.", ch, obj, victim, TO_CHAR);
      act("$N cowers back from you in fright.", ch, NULL, victim, TO_CHAR);
      act("$N cowers back from $n in fright.", ch, NULL, victim, TO_NOTVICT);
      return;
   }
   if (IS_SET(obj->extra_flags, ITEM_NODROP) )
   {
      act("$N tells you, 'I'm unable to release $p'.", ch, obj, victim, TO_CHAR);
      act("$N cowers back from you in fright.", ch, NULL, victim, TO_CHAR);
      act("$N cowers back from $n in fright.", ch, NULL, victim, TO_NOTVICT);
      return;
   }

   if ( ch->carry_weight + get_obj_weight(obj)  > can_carry_w(ch) )
   {
      act("$N tells you, 'You can't carry the weight $n.'", ch, NULL, victim, TO_CHAR);
      return;
   }
   if ( ch->carry_number + 1 > can_carry_n(ch) )
   {
      act("$N tells you, 'You can't carry that many items $n.'", ch, NULL, victim, TO_CHAR);
      return;
   }

   act("$N caves in to the bully tactics of $n.", ch, NULL, victim, TO_NOTVICT);
   act("$N shivers in fright and caves in to your bully tactics.", ch, NULL, victim, TO_CHAR);

   if (obj->wear_loc != WEAR_NONE)
   {
      act("$n stops using $p.", victim, obj, NULL, TO_ROOM);
      act("You stop using $p.", victim, obj, NULL, TO_CHAR);
   }
   act("$N gives $p to $n.", ch, obj, victim, TO_NOTVICT);
   act("$N gives you $p.", ch, obj, victim, TO_CHAR);
   if (IS_SET(obj->extra_flags, ITEM_ROT_DEATH) /*&& !floating*/ )
   {
      if (obj->pIndexData->condition < 0
      && obj->pIndexData->quitouts == FALSE )
      {
         obj->timer = (-1)*obj->pIndexData->condition;
         REMOVE_BIT(obj->extra_flags, ITEM_ROT_DEATH);
      }
   }
   REMOVE_BIT(obj->extra_flags, ITEM_VIS_DEATH);

   check_improve(ch, gsn_demand, TRUE, 2);
   WAIT_STATE(ch, 24);
   obj_from_char(obj);
   obj_to_char(obj, ch);
   ch->mana -= 30;
   ch->hit -= 30;
   return;
}

/*
   Code for the trinal wield slot, by Ceran (goes with my dual wield
   code. You need the morph_form[0] == MORPH_ARM to have the third
   hand free to tri-wield with. I don't think any other mud has type of
   code so i'm winging it on how the wield slot is used.
   Also add to hands_full function:
   if (ch->morph_form[0] == MORPH_ARM)
   count--;
*/

bool can_wield_tertiary(CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace)
{
   OBJ_DATA *primary;
   OBJ_DATA *secondary;
   OBJ_DATA *tertiary;

   primary = get_eq_char(ch, WEAR_WIELD);
   secondary = get_eq_char(ch, WEAR_DUAL_WIELD);
   tertiary = get_eq_char(ch, WEAR_TERTIARY_WIELD);
   /*
      This is pretty basic...it's identical to dual_wield slotting code but
      you check for the third hand valid wearing conditions and return FALSE
      if you can't tri-wield. This then automatically sends the weapon into
      normal dual wield slotting process.
   */
   reslot_weapon(ch);  /* verify weapon slots in case of problems */

   return FALSE;

   if (primary == NULL || secondary == NULL)
   return FALSE;

   if (ch->morph_form[0] != MORPH_ARM)
   return FALSE;
   if (get_skill(ch, gsn_tertiary_wield) < 5)
   return FALSE;
   /*    if (ch->level < skill_table[gsn_tertiary_wield].skill_level[ch->class]) */
   if (!has_skill(ch, gsn_tertiary_wield))
   return FALSE;

   if (hands_full(ch)
   && (get_eq_char(ch, WEAR_HOLD) != NULL
   || get_eq_char(ch, WEAR_SHIELD) != NULL
   || get_eq_char(ch, WEAR_LIGHT) != NULL) )
   return FALSE;

   if ( obj->weight > 60)
   return FALSE;
   if ( obj->weight > secondary->weight)
   return FALSE;

   if (!remove_obj(ch, WEAR_TERTIARY_WIELD, fReplace, TRUE))
   return FALSE;

   act("$n wields $p in $s changeling hand.", ch, obj, NULL, TO_ROOM);
   act("You wield $p in your changeling hand.", ch, obj, NULL, TO_CHAR);
   equip_char(ch, obj, WEAR_TERTIARY_WIELD);

   report_weapon_skill(ch, obj);
   return TRUE;
}

void reslot_weapon(CHAR_DATA *ch)
{
   OBJ_DATA *primary;
   OBJ_DATA *secondary;
   OBJ_DATA *tertiary;
   primary = get_eq_char(ch, WEAR_WIELD);
   secondary = get_eq_char(ch, WEAR_DUAL_WIELD);
   tertiary = get_eq_char(ch, WEAR_TERTIARY_WIELD);

   if (primary == NULL && secondary != NULL)
   {
      unequip_char(ch, secondary);
      equip_char(ch, secondary, WEAR_WIELD);
      primary = secondary;
      secondary = NULL;
   }
   if (secondary == NULL && tertiary != NULL)
   {
      unequip_char(ch, tertiary);
      if (primary == NULL)
      {
         equip_char(ch, tertiary, WEAR_WIELD);
         primary = tertiary;
         tertiary = NULL;
      }
      else
      {
         equip_char(ch, tertiary, WEAR_DUAL_WIELD);
      }
   }
   return;
}

void report_weapon_skill(CHAR_DATA *ch, OBJ_DATA *obj)
{
   int skill, sn;
   char buf[MAX_STRING_LENGTH];

   if (IS_NPC(ch))
   return;
   if (obj->item_type != ITEM_WEAPON)
   {
      sprintf(buf, "report_weapon_skill: Bad obj->type, %d, vnum %d, carried by %s.",
      obj->item_type, obj->pIndexData->vnum, ch->name);
      bug(buf, 0);
      return;
   }

   if (obj->value[0] == WEAPON_DAGGER)
   sn = gsn_dagger;
   else if (obj->value[0] == WEAPON_SWORD)
   sn = gsn_sword;
   else if (obj->value[0] == WEAPON_MACE)
   sn = gsn_mace;
   else if (obj->value[0] == WEAPON_SPEAR)
   sn = gsn_spear;
   else if (obj->value[0] == WEAPON_AXE)
   sn = gsn_axe;
   else if (obj->value[0] == WEAPON_FLAIL)
   sn = gsn_flail;
   else if (obj->value[0] == WEAPON_STAFF)
   sn = gsn_staff;
   else
   sn = -1;

   if (sn == -1)
   skill = ch->level * 3;
   else
   skill = ch->pcdata->learned[sn];
   if (skill >= 100)
   act("$p feels like a part of you!", ch, obj, NULL, TO_CHAR);
   else if (skill > 85)
   act("You feel quite confident with $p.", ch, obj, NULL, TO_CHAR);
   else if (skill > 70)
   act("You are skilled with $p.", ch, obj, NULL, TO_CHAR);
   else if (skill > 50)
   act("Your skill with $p is adequate.", ch, obj, NULL, TO_CHAR);
   else if (skill > 25)
   act("$p feels a little clumsy in your hands.", ch, obj, NULL, TO_CHAR);
   else if (skill > 1)
   act("You fumble and almost drop $p.", ch, obj, NULL, TO_CHAR);
   else
   act("You don't even know which end is up on $p.", ch, obj, NULL, TO_CHAR);

   return;
}


/* In sacrifice for elf blood bracelet */

bool sacrifice_obj_bracelet(CHAR_DATA *ch, OBJ_DATA *obj)
{
   OBJ_DATA *new_obj;
   if (IS_EVIL(ch))
   return FALSE;
   act("A white light slowly surrounds $p as the soul of the damned elf is freed.", ch, obj, NULL, TO_ROOM);
   act("A white light slowly surrounds $p as the soul of the damned elf is freed.", ch, obj, NULL, TO_CHAR);
   extract_obj(obj, FALSE);
   new_obj = create_object(get_obj_index(OBJ_VNUM_HAIR_BRACELET), 0);
   if (new_obj->pIndexData->limcount >= new_obj->pIndexData->limtotal)
   {
      extract_obj(new_obj, FALSE);
      return TRUE;
   }
   obj_to_char(new_obj, ch);
   act("When the glow fades $p has miraculously taken its place!", ch, new_obj, NULL, TO_ROOM);
   act("When the glow fades $p has miraculously taken its place!", ch, new_obj, NULL, TO_CHAR);

   return TRUE;
}


/*
   tiara & rings of wizardry....do several unseen things:
   Rings:
   1. Raises spellcraft percent by 5.
   2. Lowers mana cost of all spells by 10% (20% for high ring)
   3. Reduces magic damage past 200 points by 25%. (After sanc)
   Tiara:
   1. Regens mana at 1d3 mana per PULSE_VIOLENCE.
   2. Reduces magic damage past 150 points by 50%. (after sanc)
*/
void wear_obj_wizardry(CHAR_DATA *ch, OBJ_DATA *obj)
{
   send_to_char("You feel more attuned to the forces of magic.\n\r", ch);
   return;
}

void remove_obj_wizardry(CHAR_DATA *ch, OBJ_DATA  *obj)
{
   send_to_char("You feel less attuned to the forces of magic.\n\r", ch);
   return;
}

void wear_obj_tiara(CHAR_DATA *ch, OBJ_DATA *obj)
{
   act("$n is surrounded by a blue aura.", ch, NULL, NULL, TO_ROOM);
   send_to_char("You are surrounded by a blue aura.\n\r", ch);
   return;
}

void remove_obj_tiara(CHAR_DATA *ch, OBJ_DATA *obj)
{
   send_to_char("The blue aura around you fades.\n\r", ch);
   return;
}


void remove_obj_sceptre(CHAR_DATA *ch, OBJ_DATA *obj)
{
   int sn_frenzy;
   sn_frenzy = gsn_heavenly_wrath;
   if (is_affected(ch, sn_frenzy))
   {
      send_to_char("The heavenly wrath bestowed upon you fades away.\n\r", ch);
      affect_strip(ch, sn_frenzy);
   }

   return;
}


/* in do_brandish
if (obj->pIndex->vnum == OBJ_VNUM_HORN_VALERE)
{
obj_brandish_horn_valere(ch, obj);
return;
}
*/

void obj_brandish_horn_valere(CHAR_DATA *ch, OBJ_DATA *obj)
{
   CHAR_DATA *warrior1 = NULL;
   CHAR_DATA *warrior2 = NULL;
   CHAR_DATA *warrior3 = NULL;
   CHAR_DATA *bane     = NULL;
   int num;

   for (bane = ch->in_room->people; bane != NULL; bane = bane->next_in_room)
   {
      if (!IS_NPC(bane))
      continue;
      if (bane->pIndexData->vnum == 13718 && (bane->fighting == ch
      || ch->fighting == bane) )
      break;
   }
   if (!str_cmp(class_table[ch->class].name, "warrior"))
   num = 3;
   else if (!str_cmp(class_table[ch->class].name, "paladin"))
   num = 3;
   else if (!str_cmp(class_table[ch->class].name, "ranger"))
   num = 2;
   else if (!str_cmp(class_table[ch->class].name, "cleric"))
   num = 2;
   else if (!str_cmp(class_table[ch->class].name, "monk"))
   num = 3;
   else if (!str_cmp(class_table[ch->class].name, "anti-paladin"))
   num = 0;
   else if (!str_cmp(class_table[ch->class].name, "nightwalker"))
   num = 0;
   else if (!str_cmp(class_table[ch->class].name, "necromancer"))
   num = 0;
   else
   num = 1;

   if (bane == NULL || IS_NPC(ch) || num == 0)
   {
      act("$n raises $p and blows a resounding call but nothing happens.", ch, obj, NULL, TO_ROOM);
      act("You raise $p and blow a resounding call but nothing happens.", ch, obj, NULL, TO_CHAR);
      obj->value[1] -= 1;
      if (obj->value[1] > 0)
      return;
      act("$p crumbles to dust.", ch, obj, NULL, TO_ROOM);
      act("$p crumbles to dust.", ch, obj, NULL, TO_CHAR);
      extract_obj(obj, FALSE);
      return;
   }

   act("$n raises $p and blows a resounding battle call!", ch, obj, NULL, TO_ROOM);
   act("You raise $p and blow a resounding battle call!", ch, obj, NULL, TO_CHAR);
   warrior1 = create_mobile(get_mob_index(MOB_VNUM_DRANNOR_KNIGHT));

   if (warrior1 == NULL)
   {
      obj->value[1] -= 1;
      if (obj->value[1] > 0)
      return;
      act("$p crumbles to dust.", ch, obj, NULL, TO_ROOM);
      act("$p crumbles to dust.", ch, obj, NULL, TO_CHAR);
      extract_obj(obj, FALSE);
      return;
   }
   char_to_room(warrior1, ch->in_room);


   if (num >= 2 )
   {
      warrior2 = create_mobile(get_mob_index(MOB_VNUM_DRANNOR_KNIGHT));
      char_to_room(warrior2, ch->in_room);
   }
   if (num >= 3)
   {
      warrior3 = create_mobile(get_mob_index(MOB_VNUM_DRANNOR_KNIGHT));
      char_to_room(warrior3, ch->in_room);
   }

   act("$n appears in a burning halo of light!", warrior1, NULL, NULL, TO_ROOM);
   act("$n screams and attacks!", warrior1, NULL, NULL, TO_ROOM);
   multi_hit(warrior1, bane, TYPE_UNDEFINED);

   if (warrior2 != NULL)
   {
      act("$n appears in a burning halo of light!", warrior2, NULL, NULL, TO_ROOM);
      act("$n screams and attacks!", warrior2, NULL, NULL, TO_ROOM);
      multi_hit(warrior2, bane, TYPE_UNDEFINED);
   }

   if (warrior3 != NULL)
   {
      act("$n appears in a burning halo of light!", warrior3, NULL, NULL, TO_ROOM);
      act("$n screams and attacks!", warrior3, NULL, NULL, TO_ROOM);
      multi_hit(warrior3, bane, TYPE_UNDEFINED);
   }

   obj->value[1] -= 1;
   if (obj->value[1] > 0)
   return;
   act("$p crumbles to dust.", ch, obj, NULL, TO_ROOM);
   act("$p crumbles to dust.", ch, obj, NULL, TO_CHAR);
   extract_obj(obj, FALSE);
   return;
}



void obj_give_hydra_head(CHAR_DATA *ch, CHAR_DATA *lady, OBJ_DATA *obj)
{
   OBJ_DATA *horn;

   if (!IS_NPC(lady))  return;
   if (lady->pIndexData->vnum != MOB_VNUM_DRANNOR_LADY)
   return;
   if (obj->pIndexData->vnum != OBJ_VNUM_HYDRA_HEAD)
   return;
   horn = create_object(get_obj_index(OBJ_VNUM_HORN_VALERE), 0);
   if (horn == NULL)
   return;

   obj_to_char(horn, ch);
   extract_obj(obj, FALSE);
   do_say(lady, "Your worth is proven great adventurer.");
   act("$n gives you $p.", lady, horn, ch, TO_VICT);
   act("$n gives $N $p.", lady, horn, ch, TO_NOTVICT);
   do_say(lady, "Take this artifact in your quest to slay the evil, brandish it when you face darkest evil.");
   act("$n flickers for a moment then fades away.", lady, NULL, NULL, TO_ROOM);
   extract_char(lady, TRUE);
   return;
}

void wear_obj_ring_protection(CHAR_DATA *ch, OBJ_DATA *obj)
{
   AFFECT_DATA af;
   int sn;

   sn = gsn_protective_shield;
   send_to_char("You feel protected.\n\r", ch);
   affect_strip(ch, sn);
   af.where = TO_AFFECTS;
   af.type = sn;
   af.duration = -1;
   af.modifier = 0;
   af.location = 0;
   af.level = ch->level;
   af.bitvector = 0;
   affect_to_char(ch, &af);
   return;
}

void remove_obj_ring_protection(CHAR_DATA *ch, OBJ_DATA *obj)
{
   int sn;

   sn = gsn_protective_shield;
   if (!is_affected(ch, sn))
   return;
   send_to_char("You feel less protected.\n\r", ch);
   affect_strip(ch, sn);
   return;
}

void wear_obj_whitehelm(CHAR_DATA *ch, OBJ_DATA *obj)
{
   AFFECT_DATA af;
   int sn;

   if (IS_NPC(ch))
   return; /* no mobs wearing this puppy */
   if ( ch->pcdata->ethos < 1000 )
   return; /* fix to prevent zapping giving effect */
   sn = gsn_blackjack_timer;
   send_to_char("You become aware of the treachery all around you.\n\r", ch);
   affect_strip(ch, sn);
   af.where = TO_AFFECTS;
   af.type = sn;
   af.duration = -1;
   af.modifier = 0;
   af.location = 0;
   af.level = ch->level;
   af.bitvector = 0;
   affect_to_char(ch, &af);
   return;
}

void wear_obj_robe_avenger(CHAR_DATA *ch, OBJ_DATA *obj)
{
   send_to_char("You feel protected.\n\r", ch);
   return;
}

void remove_obj_robe_avenger(CHAR_DATA *ch, OBJ_DATA *obj)
{
   send_to_char("You feel less protected.\n\r", ch);
   return;
}

void remove_obj_whitehelm(CHAR_DATA *ch, OBJ_DATA *obj)
{
   int sn;

   sn = gsn_blackjack_timer;
   if (!is_affected(ch, sn))
   return;
   send_to_char("You once again turn a blind eye to treachery.\n\r", ch);
   affect_strip(ch, sn);
   return;
}

/* robes of zhentil mage */
void wear_obj_zhentil_robe(CHAR_DATA *ch, OBJ_DATA *obj)
{
   AFFECT_DATA af;

   if (IS_NPC(ch))
   return;
   send_to_char("You feel a darkness converge on your soul.\n\r", ch);
   if (!saves_spell(ch, 51, ch, DAM_OTHER, SAVE_SPELL))
   {
      affect_strip(ch, gsn_blindness);
      af.where = TO_AFFECTS;
      af.type = gsn_blindness;
      af.duration = -1;
      af.modifier = -3;
      af.location = APPLY_HITROLL;
      af.level = 51;
      af.bitvector = AFF_BLIND;
      affect_to_char(ch, &af);
      act("A veil of darkness seems to cover $n's eyes.", ch, NULL, NULL, TO_ROOM);
      send_to_char("You can't see a thing!\n\r", ch);
   }
   return;
}

void wear_obj_fallen_wings(CHAR_DATA *ch, OBJ_DATA *obj)
{
   AFFECT_DATA af;

   act("$n's shredded wings slowly beat and $s feet rise of the ground.", ch, NULL, NULL, TO_ROOM);
   send_to_char("Your shredded angel wings slowly beat and your feet rise off the ground.\n\r", ch);
   affect_strip(ch, gsn_fly);

   af.where = TO_AFFECTS;
   af.type = gsn_fly;
   af.modifier = 0;
   af.location = 0;
   af.bitvector = AFF_FLYING;
   af.level = 51;
   af.duration = -1;
   affect_to_char(ch, &af);

   return;
}

void remove_obj_fallen_wings(CHAR_DATA *ch, OBJ_DATA *obj)
{
   int sn_fly;
   sn_fly = gsn_fly;
   if (!is_affected(ch, sn_fly))
   return;
   act("$n slowly floats back to the ground.", ch, NULL, NULL, TO_ROOM);
   send_to_char("Your feet slowly float back to the ground.\n\r", ch);
   affect_strip(ch, sn_fly);
   return;
}

void wear_obj_black_hide(CHAR_DATA *ch, OBJ_DATA *obj)
{
   send_to_char("You feel more resistant to corrosive gasses and liquids.\n\r", ch);
   return;
}

void wear_obj_shadow_hide(CHAR_DATA *ch, OBJ_DATA *obj)
{
   AFFECT_DATA af;
   int sn_infra;
   sn_infra = gsn_infravision;
   if (IS_AFFECTED(ch, AFF_INFRARED))
   return;
   af.where = TO_AFFECTS;
   af.type = sn_infra;
   af.modifier = 0;
   af.duration = -1;
   af.location = 0;
   af.bitvector = AFF_INFRARED;
   affect_to_char(ch, &af);
   send_to_char("Your eyes glow red.\n\r", ch);
   return;
}


void remove_obj_black_hide(CHAR_DATA *ch, OBJ_DATA *obj)
{
   send_to_char("You feel less resistant to acid.\n\r", ch);
   return;
}

void remove_obj_shadow_hide(CHAR_DATA *ch, OBJ_DATA *obj)
{
   int sn_infra;
   sn_infra = gsn_infravision;
   if (is_affected(ch, sn_infra))
   {
      send_to_char("You no longer see in the dark.\n\r", ch);
      affect_strip(ch, sn_infra);
   }
   return;
}

void wear_obj_symbol_magic(CHAR_DATA *ch, OBJ_DATA *obj)
{
   act("Your $p glows brightly.", ch, obj, NULL, TO_CHAR);
   send_to_char("You feel arcane power flowing through you.\n\r", ch);
   act("$n's $p glows brightly.", ch, obj, NULL, TO_ROOM);

   return;
}

void remove_obj_symbol_magic(CHAR_DATA *ch, OBJ_DATA *obj)
{
   send_to_char("You feel less attuned to the powers of magic.\n\r", ch);
   return;
}

void wear_obj_red_dragonstar(CHAR_DATA *ch, OBJ_DATA *obj)
{
   send_to_char("You feel more resistant to fire.\n\r", ch);
   return;
}

void wear_obj_black_dragonstar(CHAR_DATA *ch, OBJ_DATA *obj)
{
   send_to_char("You feel more resistant to acid.\n\r", ch);
   return;
}

void remove_obj_red_dragonstar(CHAR_DATA *ch, OBJ_DATA *obj)
{
   send_to_char("You feel less resistant to fire.\n\r", ch);
   return;
}

void remove_obj_black_dragonstar(CHAR_DATA *ch, OBJ_DATA *obj)
{
   send_to_char("You feel less resistant to acid.\n\r", ch);
   return;
}

void wear_obj_cloak_underworld(CHAR_DATA *ch, OBJ_DATA *obj)
{
   AFFECT_DATA af;
   int sn1;
   sn1 = gsn_infravision;
   if (sn1 > 0)
   {
      af.where = TO_AFFECTS;
      af.type = sn1;
      af.modifier = 0;
      af.location = 0;
      af.level = 60;
      af.duration = -1;
      af.bitvector = AFF_INFRARED;
      affect_to_char(ch, &af);
      send_to_char("Your eyes glow red.\n\r", ch);
   }
   send_to_char("You feel immune to poisons and disease.\n\r", ch);
   return;
}

void remove_obj_cloak_underworld(CHAR_DATA *ch, OBJ_DATA *obj)
{
   int sn;
   sn = gsn_infravision;

   if (is_affected(ch, sn))
   {
      send_to_char("You no longer see in the dark.\n\r", ch);
      affect_strip(ch, sn);
   }
   send_to_char("You feel more susceptible to poisons and diseases.\n\r", ch);
   return;
}

/* Libram of infinite conjuration */
void recite_libram_conjuration(CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA *scroll, OBJ_DATA *obj, bool fRead)
{
   int sn1, sn2, sn3, sn;
   char *spell;
   sn1 = -1;
   sn2 = -1;
   sn3 = -1;

   /* Cannot spellbane */
   if (fRead)
   {
      obj_cast_spell(scroll->value[1], scroll->value[0], ch, victim, obj);
      if (ch->in_room != NULL)
      {
         obj_cast_spell(scroll->value[2], scroll->value[0], ch, victim, obj);
      }
      if (ch->in_room != NULL)
      {
         obj_cast_spell(scroll->value[3], scroll->value[0], ch, victim, obj);
      }
      check_improve(ch, gsn_corrupt("scrolls", &gsn_scrolls), TRUE, 5);
   }

   if (number_percent() < 5)
   {
      act("$n's libram of infinite conjuration glows brightly and vanishes!", ch, NULL, NULL, TO_ROOM);
      act("Your libram of infinite conjuration glows brightly then vanishes!", ch, NULL, NULL, TO_CHAR);
      extract_obj(scroll, FALSE);
      return;
   }
   else
   {
      scroll->value[0] = number_range(44, 56);
      scroll->value[1] = -1;
      scroll->value[2] = -1;
      scroll->value[3] = -1;
      for (; ;)
      {
         if (sn3 != -1)
         break;
         switch (number_range(0, 53))
         {
            case 1: spell = "armor";    break;
            case 2: spell = "shield"; break;
            case 3: spell = "bless"; break;
            case 4: spell = "protection good"; break;
            case 5: spell = "protection evil"; break;
            case 6: spell = "protective shield"; break;
            case 7: spell = "detect invis"; break;
            case 8: spell = "detect evil"; break;
            case 9: spell = "detect magic"; break;
            case 10: spell = "fireball"; break;
            case 11: spell = "iceball"; break;
            case 12: spell = "cone of cold"; break;
            case 13: spell = "power word kill"; break;
            case 14: spell = "curse"; break;
            case 15: spell = "blindness"; break;
            case 16: spell = "cure blind"; break;
            case 17: spell = "poison"; break;
            case 18: spell = "cure poison"; break;
            case 19: spell = "plague"; break;
            case 20: spell = "cure disease"; break;
            case 21: spell = "power word recall"; break;
            case 22: spell = "haste"; break;
            case 23: spell = "slow"; break;
            case 24: spell = "absorb"; break;
            case 25: spell = "chain lightning"; break;
            case 26: spell = "lightning bolt"; break;
            case 27: spell = "magic missile"; break;
            case 28: spell = "acid blast"; break;
            case 29: spell = "cremate"; break;
            case 30: spell = "frostbolt"; break;
            case 31: spell = "grounding"; break;
            case 32: spell = "fly"; break;
            case 33: spell = "passdoor"; break;
            case 34: spell = "chill touch"; break;
            case 35: spell = "create spring"; break;
            case 36: spell = "pox"; break;
            case 37: spell = "charm person"; break;
            case 38: spell = "energy drain"; break;
            case 39: spell = "teleport"; break;
            case 40: spell = "deathspell"; break;
            case 41: spell = "enchant weapon"; break;
            case 42: spell = "enchant armor"; break;
            case 43: spell = "invis"; break;
            case 44: spell = "mass invis"; break;
            case 45: spell = "timestop"; break;
            case 46: spell = "earthquake"; break;
            case 47: spell = "firestream"; break;
            case 48: spell = "chromatic shield"; break;
            case 49: spell = "forget"; break;
            case 50: spell = "concatenate"; break;
            case 51: spell = "harm"; break;
            case 52: spell = "heal"; break;
            default:    spell = "detect invis"; break;
         }

         sn = skill_lookup(spell);
         if (sn <= 0)
         continue;
         if (sn1 == -1)
         {
            sn1 = sn;
            scroll->value[1] = sn1;
            if (number_percent() < 75)
            break;
         }
         else if (sn2 == -1)
         {
            sn2 = sn;
            scroll->value[2] = sn2;
            if (number_percent() < 85)
            break;
         }
         else if (sn3 == -1)
         {
            sn3 = sn;
            scroll->value[3] = sn3;
            break;
         }
      }
   }

   send_to_char("Your libram of infinite conjuration glows softly and new words appear on the page.\n\r", ch);

   return;
}

void brandish_staff_fectus(CHAR_DATA *ch, OBJ_DATA *obj)
{
   int hahah;
   int sn;
   AFFECT_DATA af;

   hahah = number_percent();
   send_to_char("You make an error in the ceremony, and the staff turns on you!\n\r", ch);

   if (hahah <= 29)
   {
      send_to_char("The uncontrolled power of the staff consumes you!!!\n\r", ch);
      act( "$n makes an error in the ceremony, and is consumed by the staffs power!", ch, NULL, NULL, TO_ROOM );
      raw_kill(ch, ch);
      return;
   }

   if ((hahah > 30) && (hahah < 70))
   {
      send_to_char("You fall into a coma!\n\r", ch);
      act( "$n makes an error in the ceremony, and falls into a coma!", ch, NULL, NULL, TO_ROOM );

      sn = gsn_sleep;
      af.where = TO_AFFECTS;
      af.type         = sn;
      af.modifier         = 0;
      af.location         = 0;
      af.duration         = 48;
      af.level         = 100;
      af.bitvector         = AFF_SLEEP;
      affect_to_char(ch, &af);
      ch->position = POS_SLEEPING;
      return;
   }

   if (hahah >= 70)
   {
      send_to_char("The uncontrolled power of the staff blasts your mind!\n\r", ch);
      act( "$n makes an error in the ceremony, and is mentally crippled!", ch, NULL, NULL, TO_ROOM );

      sn = gsn_curse;
      af.where = TO_AFFECTS;
      af.type         = sn;
      af.modifier         = -15;
      af.location         = APPLY_INT;
      af.duration         = 48;
      af.level         = 100;
      af.bitvector         = AFF_CURSE;
      affect_to_char(ch, &af);
      af.location        = APPLY_WIS;
      affect_to_char(ch, &af);
      af.location        = APPLY_HITROLL;
      affect_to_char(ch, &af);
      af.modifier        = 20;
      af.location        = APPLY_SAVES;
      affect_to_char(ch, &af);
      return;
   }
   return;
}

void brandish_sceptre_dead(CHAR_DATA *ch, OBJ_DATA *obj)
{
   CHAR_DATA *victim;
   CHAR_DATA *v_next;
   int sn_stun, chance;
   AFFECT_DATA af;

   sn_stun = gsn_power_word_stun;
   if (sn_stun > 0)
   {
      af.where = TO_AFFECTS;
      af.type = sn_stun;
      af.location = APPLY_DEX;
      af.modifier = -5;
      af.duration = 6;
      af.bitvector = 0;
      af.level = ch->level;
   }

   for (victim = ch->in_room->people; victim != NULL; victim = v_next)
   {
      v_next = victim->next_in_room;
      if (!IS_NPC(victim))
      continue;
      if (!IS_SET(victim->act, ACT_UNDEAD))
      continue;
      chance = 65 + (ch->level - victim->level) * 3;
      if (number_percent() > chance)
      {
         act("$n's eyes flicker as the sceptre draws their attention, but then they return to normal.", victim, NULL, NULL, TO_ROOM);
         continue;
      }
      act("$n is transfixed by the power of the unholy sceptre of the dead and stops still.", victim, NULL, NULL, TO_ROOM);
      act("You are transfixed by the unholy power of the sceptre of dead.", victim, NULL, NULL, TO_CHAR);
      stop_fighting(victim, TRUE);
      if (ch->fighting == victim)
      stop_fighting(ch, TRUE);
      if (sn_stun <= 0)
      continue;
      if (!is_affected(victim, sn_stun) && !saves_spell(ch, ch->level, victim, DAM_OTHER, SAVE_OTHER))
      affect_to_char(victim, &af);

   }
   return;
}

void wear_obj_misty_cloak(CHAR_DATA *ch, OBJ_DATA *obj)
{
   int sn;
   AFFECT_DATA af;

   sn = gsn_pass_door;
   if (sn <= 0)
   return;

   if (is_affected(ch, sn))
   affect_strip(ch, sn);
   if (IS_AFFECTED(ch, AFF_PASS_DOOR)
   || ch->race == grn_ethereal)
   return;

   af.where = TO_AFFECTS;
   af.modifier = 0;
   af.location = 0;
   af.duration = -1;
   af.level = 60;
   af.bitvector = AFF_PASS_DOOR;
   af.type = sn;
   affect_to_char(ch, &af);

   send_to_char("You turn transulcent.\n\r", ch);
   return;
}

void remove_obj_misty_cloak(CHAR_DATA *ch, OBJ_DATA *obj)
{
   int sn;
   sn = gsn_pass_door;
   if (sn <= 0)
   return;
   if (!is_affected(ch, sn))
   return;
   send_to_char("You become more solid again.\n\r", ch);
   affect_strip(ch, sn);
   return;
}


void wear_obj_troll_skin(CHAR_DATA *ch, OBJ_DATA *obj)
{
   send_to_char("You feel bits of disgusting troll skin forming on your wounds.\n\r", ch);
   return;

}

void remove_obj_troll_skin(CHAR_DATA *ch, OBJ_DATA *obj)
{

   send_to_char("The bits of scabby troll skin flake off your body.\n\r", ch);
   return;
}

void hold_summoning_stone( CHAR_DATA *ch, OBJ_DATA *stone )
{
   CHAR_DATA *dragonmount;
   AFFECT_DATA af;

   if ( ch->level < 25 )
   {
      send_to_char("An alien voice echoes in your mind, \"In good time, young one.\"\n\r", ch);
      return;
   }

   if ( is_affected(ch, gsn_dragonmount) )
   {
      send_to_char("You are yet too weak to summon another Dragonmount.\n\r", ch);
      return;
   }

   for (dragonmount = char_list; dragonmount != NULL; dragonmount = dragonmount->next)
   {
      if ( IS_NPC(dragonmount) )
      if ( dragonmount->master == ch
      && dragonmount->pIndexData->vnum == MOB_VNUM_DRAGONMOUNT )
      {
         send_to_char("You already have summoned a dragonmount.\n\r", ch);
         return;
      }
   }

   af.where = TO_AFFECTS;
   af.level = 30;
   af.location = 0;
   af.modifier = 0;
   af.duration = 30;
   af.bitvector = 0;
   af.type = gsn_dragonmount;
   affect_to_char(ch, &af);

   dragonmount = create_mobile(get_mob_index(MOB_VNUM_DRAGONMOUNT));

   dragonmount->alignment = ch->alignment;
   act("$n summons a dragonmount through the summoning stone!", ch, NULL, NULL, TO_ROOM);
   send_to_char("You summon a dragonmount through the summoning stone!\n\r", ch);

   char_to_room(dragonmount, ch->in_room);

   SET_BIT(dragonmount->affected_by, AFF_CHARM);
   add_follower(dragonmount, ch);
   dragonmount->leader = ch;
   return;
}

void do_assess(CHAR_DATA *ch, char * argument)
{
   char buf[MAX_STRING_LENGTH];
   char arg[MAX_INPUT_LENGTH];
   OBJ_DATA *obj;
   int number, weight;

   one_argument(argument, arg);

   if (arg[0] == '\0')
   {
      send_to_char("Assess what?\n\r", ch);
      return;
   }

   if ((obj = get_obj_carry(ch, arg, ch)) == NULL)
   {
      send_to_char("You don't carry that.\n\r", ch);
      return;
   }

   if (ch->move < 5)
   {
      send_to_char("You don't have the energy to assess.\n\r", ch);
      return;
   }

   weight = get_obj_weight(obj)/10;
   number = weight;
   if (weight/10 > 0)
   number = number_fuzzy(number);
   if (weight/10 > 6)
   number = number_fuzzy(number);
   if (weight/10 > 13)
   number = number_fuzzy(number);
   if (weight/10 > 21)
   number = number_fuzzy(number);
   if (weight/10 > 30)
   number = number_fuzzy(number);
   if (weight/10 > 40)
   number = number_fuzzy(number);

   sprintf(buf, "You assess %s.\n\rIt weighs about %d pound%s and is made of %s.\n\r",
   obj->short_descr, number, number == 1 ? "" : "s",
   (is_affected_obj(obj, gsn_obscure)) ?
   "unknown mixture":obj->material);
   send_to_char(buf, ch);
   ch->move -= 5;
   return;
}

/* check for voodoo doll then if not do the social */
void do_voodoo(CHAR_DATA *ch, char * argument){
   OBJ_DATA *doll;
   CHAR_DATA *victim;
   char arg[MAX_INPUT_LENGTH];
   AFFECT_DATA af;

   doll = get_eq_char(ch, WEAR_HOLD);
   if (doll == NULL){
      check_social(ch, "voodoo", argument);
      return;
   }
   if (doll->pIndexData->vnum != OBJ_VNUM_VOODOO_DOLL){
      check_social(ch, "voodoo", argument);
      return;
   }
   one_argument(argument, arg);
   if (arg[0] == '\0'){
      check_social(ch, "voodoo", argument);
      return;
   }

   if ((victim = get_char_room(ch, arg)) == NULL){
      check_social(ch, "voodoo", argument);
      return;
   }

   if (is_safe(ch, victim, 0)){
      send_to_char("The gods prevent your black magic from affecting them.\n\r", ch);
      return;
   }
   if (is_affected(victim, gsn_voodoo)){
      send_to_char("They are already suffering from voodoo.\n\r", ch);
      return;
   }

   af.where = TO_AFFECTS;
   af.level = doll->level;
   af.duration = 4;
   af.type = gsn_voodoo;
   af.bitvector = 0;
   af.location = 0;
   af.modifier = 0;

   if (IS_IMMORTAL(victim) && !IS_IMMORTAL(ch)){
      send_to_char("They laugh at your feeble magic.\n\r", ch);
      victim = ch;
   }

   if ( saves_armor_of_god(ch, doll->level, victim) )
   {
      send_to_char( "They resist your voodoo!\n\r", ch );
      return;
   }

   switch (number_range(1, 5)){
      default:
      case 1:
      send_to_char("You feel a sudden pain in your eyes.\n\r", victim);
      send_to_char("You stab the doll vicously in the eyes.\n\r", ch);
      af.bitvector = AFF_BLIND;
      affect_to_char(victim, &af);
      break;
      case 2:
      send_to_char("You feel very sick.\n\r", victim);
      send_to_char("You stab the dolls stomach repeatedly.\n\r", ch);
      affect_to_char(victim, &af);
      af.type = gsn_plague;
      affect_to_char(victim, &af);
      break;
      case 3:
      send_to_char("You feel very weak.\n\r", victim);
      send_to_char("You stab the dolls arms.\n\r", ch);
      af.modifier = -10;
      af.location = APPLY_STR;
      affect_to_char(victim, &af);
      break;
      case 4:
      send_to_char("You suddenly forget who you are.\n\r", victim);
      send_to_char("You stab the dolls head evilly.\n\r", ch);
      affect_to_char(victim, &af);
      af.type = gsn_forget;
      affect_to_char(victim, &af);
      break;
      case 5:
      send_to_char("You feel unclean.\n\r", victim);
      send_to_char("You strike the doll all over.\n\r", ch);
      af.bitvector = AFF_CURSE;
      affect_to_char(victim, &af);
      break;
   }
   WAIT_STATE(ch, 24);
   return;
}

void brandish_soul_orb(CHAR_DATA *ch, OBJ_DATA *staff)
{
   int llevel = ch->level;

   if ((!IS_NPC(ch)) && (IS_SET(ch->act2, PLR_LICH)))
   llevel = ch->level+( (ch->played)/(3600*150) );

   if (staff->level > llevel){
      send_to_char("You are not strong enough to drain the life force stored in the orb.\n\r", ch);
      return;
   }
   if (number_percent() > get_skill(ch, gsn_staves)){
      send_to_char("You fail to drain the orb and it is destroyed.\n\r", ch);
      check_improve(ch, gsn_staves, FALSE, 2);
      extract_obj(staff, FALSE);
      return;
   }
   check_improve(ch, gsn_staves, TRUE, 2);
   ch->hit = UMIN(ch->hit + staff->value[0], ch->max_hit);
   ch->mana = UMIN(ch->mana + staff->value[1], ch->max_mana);
   ch->move = UMIN(ch->move + staff->value[2], ch->max_move);

   act( "$n shatters $p and is surrounded by a swirling cloud of dark energy.", ch, staff, NULL, TO_ROOM );
   act( "You shatter $p and are surrounded by a swirling cloud of dark energy.", ch, staff, NULL, TO_CHAR );
   send_to_char("You feel invigorated as you absorb the stored life energy.\n\r", ch);
   extract_obj(staff, FALSE);
   WAIT_STATE(ch, 12);
   return;
}


void do_use( CHAR_DATA *ch, char * argument )
{
   OBJ_DATA *obj;
   char objname[MAX_INPUT_LENGTH];

   argument = one_argument( argument, objname );
   if ( objname[0] == '\0' )
   {
      send_to_char("Use what?\n\r", ch);
      return;
   }

   obj = get_obj_list( ch, objname, ch->in_room->contents );
   if ( obj == NULL )
   {
      if ( ( obj = get_obj_carry( ch, objname, ch ) ) == NULL )
      {
         send_to_char( "You do not have that item.\n\r", ch );
         return;
      }
   }

   if ((obj->pIndexData->vnum == 15902) /* Xurinos' gameboard */
   || (obj->pIndexData->vnum == 28153))
   {
      EXTRA_DESCR_DATA * gamedesc;
      char gamecmd[MAX_INPUT_LENGTH];
      char newdesc[MAX_STRING_LENGTH];

      for (gamedesc = obj->extra_descr; (gamedesc != NULL) && (str_cmp(gamedesc->keyword, "board gameboard game")); gamedesc = gamedesc->next) ;
      if (!gamedesc)
      {
         sprintf(newdesc, "%s\n\r\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r",
         "You look upon the grey marble board with interest.",
         "    a   b   c   d   e   f   g   h",
         "  +---+---+---+---+---+---+---+---+",
         "1 |   |   |   |   |   |   |   |   | 1",
         "  +---+---+---+---+---+---+---+---+",
         "2 |   |   |   |   |   |   |   |   | 2",
         "  +---+---+---+---+---+---+---+---+",
         "3 |   |   |   |   |   |   |   |   | 3",
         "  +---+---+---+---+---+---+---+---+",
         "4 |   |   |   |   |   |   |   |   | 4",
         "  +---+---+---+---+---+---+---+---+",
         "5 |   |   |   |   |   |   |   |   | 5",
         "  +---+---+---+---+---+---+---+---+",
         "6 |   |   |   |   |   |   |   |   | 6",
         "  +---+---+---+---+---+---+---+---+",
         "7 |   |   |   |   |   |   |   |   | 7",
         "  +---+---+---+---+---+---+---+---+",
         "8 |   |   |   |   |   |   |   |   | 8",
         "  +---+---+---+---+---+---+---+---+",
         "    a   b   c   d   e   f   g   h");
         gamedesc = new_extra_descr();
         gamedesc->keyword = str_dup("board gameboard game");
         gamedesc->description = str_dup(newdesc);
         gamedesc->next = obj->extra_descr;
         obj->extra_descr = gamedesc;
      }

      argument = one_argument( argument, gamecmd );

      if (!strcmp(gamecmd, "play"))
      {
         char gamemove[MAX_INPUT_LENGTH];
         char * cur1, * cur2;
         unsigned int x=0, y=0;

         argument = one_argument( argument, gamemove );
         for (x=0;x<strlen(gamemove);x++) gamemove[x] = LOWER(gamemove[x]);
         if (strlen(gamemove) == 5) gamemove[2] = '-';
         if
         (
            (strlen(gamemove) < 4) ||
            (strlen(gamemove) > 5) ||
            (gamemove[0] < 'a') ||
            (gamemove[0] > 'h') ||
            (gamemove[1] < '1') ||
            (gamemove[1] > '8') ||
            ((strlen(gamemove) == 5) &&
            ((gamemove[3] < 'a') ||
            (gamemove[3] > 'h'))) ||
            ((strlen(gamemove) == 4) &&
            ((gamemove[2] < 'a') ||
            (gamemove[2] > 'h'))) ||
            ((strlen(gamemove) == 5) &&
            ((gamemove[4] < '1') ||
            (gamemove[4] > '8'))) ||
            ((strlen(gamemove) == 4) &&
            ((gamemove[3] < '1') ||
            (gamemove[3] > '8')))
         )
         {
            send_to_char("Invalid move.\n\r", ch);
            return;
         }

         x = LOWER(gamemove[0])-LOWER('a') + 1;
         y = gamemove[1] - '0';

         for (gamedesc = obj->extra_descr; (gamedesc != NULL) && (str_cmp(gamedesc->keyword, "board gameboard game")); gamedesc = gamedesc->next) ;
         sprintf(newdesc, "%s", gamedesc->description);

         cur1 = &newdesc[x*4+1 + (y-1)*76 + 125];
         if (*cur1 == ' ')
         {
            send_to_char("There is no piece at the starting coordinate.\n\r", ch);
            return;
         }

         x = LOWER(gamemove[((strlen(gamemove) == 5) ? 3 : 2)])-LOWER('a') + 1;
         y = gamemove[((strlen(gamemove) == 5) ? 4 : 3)] - '0';

         cur2 = &newdesc[x*4+1 + (y-1)*76 + 125];
         if (*cur2 != ' ')
         {
            send_to_char("There is already a piece at the ending coordinate.\n\r", ch);
            return;
         }

         *cur2 = *cur1;
         *cur1 = ' ';

         free_string(gamedesc->description);
         gamedesc->description = str_dup(newdesc);
         do_say(ch, gamemove);
         send_to_char("You then make your play on the gameboard.\n\r", ch);
         act( "$n then makes the play on the gameboard.", ch, NULL, NULL, TO_ROOM );
         return;
      }
      else
      if (!strcmp(gamecmd, "clear"))
      {
         for (gamedesc = obj->extra_descr; (gamedesc != NULL) && (str_cmp(gamedesc->keyword, "board gameboard game")); gamedesc = gamedesc->next) ;

         sprintf(newdesc, "%s\n\r\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r",
         "You look upon the grey marble board with interest.",
         "    a   b   c   d   e   f   g   h",
         "  +---+---+---+---+---+---+---+---+",
         "1 |   |   |   |   |   |   |   |   | 1",
         "  +---+---+---+---+---+---+---+---+",
         "2 |   |   |   |   |   |   |   |   | 2",
         "  +---+---+---+---+---+---+---+---+",
         "3 |   |   |   |   |   |   |   |   | 3",
         "  +---+---+---+---+---+---+---+---+",
         "4 |   |   |   |   |   |   |   |   | 4",
         "  +---+---+---+---+---+---+---+---+",
         "5 |   |   |   |   |   |   |   |   | 5",
         "  +---+---+---+---+---+---+---+---+",
         "6 |   |   |   |   |   |   |   |   | 6",
         "  +---+---+---+---+---+---+---+---+",
         "7 |   |   |   |   |   |   |   |   | 7",
         "  +---+---+---+---+---+---+---+---+",
         "8 |   |   |   |   |   |   |   |   | 8",
         "  +---+---+---+---+---+---+---+---+",
         "    a   b   c   d   e   f   g   h");

         free_string(gamedesc->description);
         gamedesc->description = str_dup(newdesc);
         send_to_char("You clear the gameboard.\n\r", ch);
         act( "$n clears the gameboard.", ch, NULL, NULL, TO_ROOM );
         return;
      } else
      if (!strcmp(gamecmd, "draughts"))
      {
         for (gamedesc = obj->extra_descr; (gamedesc != NULL) && (str_cmp(gamedesc->keyword, "board gameboard game")); gamedesc = gamedesc->next) ;

         sprintf(newdesc, "%s\n\r\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r",
         "You look upon the grey marble board with interest.",
         "    a   b   c   d   e   f   g   h",
         "  +---+---+---+---+---+---+---+---+",
         "1 |   | X |   | X |   | X |   | X | 1",
         "  +---+---+---+---+---+---+---+---+",
         "2 | X |   | X |   | X |   | X |   | 2",
         "  +---+---+---+---+---+---+---+---+",
         "3 |   | X |   | X |   | X |   | X | 3",
         "  +---+---+---+---+---+---+---+---+",
         "4 |   |   |   |   |   |   |   |   | 4",
         "  +---+---+---+---+---+---+---+---+",
         "5 |   |   |   |   |   |   |   |   | 5",
         "  +---+---+---+---+---+---+---+---+",
         "6 | O |   | O |   | O |   | O |   | 6",
         "  +---+---+---+---+---+---+---+---+",
         "7 |   | O |   | O |   | O |   | O | 7",
         "  +---+---+---+---+---+---+---+---+",
         "8 | O |   | O |   | O |   | O |   | 8",
         "  +---+---+---+---+---+---+---+---+",
         "    a   b   c   d   e   f   g   h");

         free_string(gamedesc->description);
         gamedesc->description = str_dup(newdesc);
         send_to_char("You set up the gameboard for a game of Draughts.\n\r", ch);
         act( "$n sets up the gameboard for a game of Draughts.", ch, NULL, NULL, TO_ROOM );
         return;
      } else
      if (!strcmp(gamecmd, "khas"))
      {
         for (gamedesc = obj->extra_descr; (gamedesc != NULL) && (str_cmp(gamedesc->keyword, "board gameboard game")); gamedesc = gamedesc->next) ;

         sprintf(newdesc, "%s\n\r\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r%s\n\r",
         "You look upon the grey marble board with interest.",
         "    a   b   c   d   e   f   g   h",
         "  +---+---+---+---+---+---+---+---+",
         "1 | R | N | B | Q | K | B | N | R | 1",
         "  +---+---+---+---+---+---+---+---+",
         "2 | P | P | P | P | P | P | P | P | 2",
         "  +---+---+---+---+---+---+---+---+",
         "3 |   |   |   |   |   |   |   |   | 3",
         "  +---+---+---+---+---+---+---+---+",
         "4 |   |   |   |   |   |   |   |   | 4",
         "  +---+---+---+---+---+---+---+---+",
         "5 |   |   |   |   |   |   |   |   | 5",
         "  +---+---+---+---+---+---+---+---+",
         "6 |   |   |   |   |   |   |   |   | 6",
         "  +---+---+---+---+---+---+---+---+",
         "7 | p | p | p | p | p | p | p | p | 7",
         "  +---+---+---+---+---+---+---+---+",
         "8 | r | n | b | q | k | b | n | r | 8",
         "  +---+---+---+---+---+---+---+---+",
         "    a   b   c   d   e   f   g   h");

         free_string(gamedesc->description);
         gamedesc->description = str_dup(newdesc);
         send_to_char("You set up the gameboard for a game of Khas.\n\r", ch);
         act( "$n sets up the gameboard for a game of Khas.", ch, NULL, NULL, TO_ROOM );
         return;
      }
      else
      if
      (
         (!str_prefix(gamecmd, "black")) ||
         (!str_prefix(gamecmd, "white")) ||
         (!str_prefix(gamecmd, "blackpawn")) ||
         (!str_prefix(gamecmd, "bp")) ||
         (!str_prefix(gamecmd, "blackrook")) ||
         (!str_prefix(gamecmd, "br")) ||
         (!str_prefix(gamecmd, "blackbishop")) ||
         (!str_prefix(gamecmd, "bb")) ||
         (!str_prefix(gamecmd, "blackqueen")) ||
         (!str_prefix(gamecmd, "bq")) ||
         (!str_prefix(gamecmd, "blackknight")) ||
         (!str_prefix(gamecmd, "bn")) ||
         (!str_prefix(gamecmd, "whitepawn")) ||
         (!str_prefix(gamecmd, "wp")) ||
         (!str_prefix(gamecmd, "whiterook")) ||
         (!str_prefix(gamecmd, "wr")) ||
         (!str_prefix(gamecmd, "whitebishop")) ||
         (!str_prefix(gamecmd, "wb")) ||
         (!str_prefix(gamecmd, "whitequeen")) ||
         (!str_prefix(gamecmd, "wq")) ||
         (!str_prefix(gamecmd, "whiteknight")) ||
         (!str_prefix(gamecmd, "wn")) ||
         (!str_prefix(gamecmd, "whiteking")) ||
         (!str_prefix(gamecmd, "wk")) ||
         (!str_prefix(gamecmd, "blackking")) ||
         (!str_prefix(gamecmd, "bk"))
      )
      {
         char piece=' ';
         char sx[MAX_INPUT_LENGTH];
         char sy[MAX_INPUT_LENGTH];
         char cur;
         int x=0, y=0;

         if (!str_prefix(gamecmd, "black")) piece = 'X'; else
         if (!str_prefix(gamecmd, "white")) piece = 'O'; else
         if (!str_prefix(gamecmd, "blackking")) piece = 'K'; else
         if (!str_prefix(gamecmd, "whiteking")) piece = 'k'; else
         if (!str_prefix(gamecmd, "whitequeen")) piece = 'q'; else
         if (!str_prefix(gamecmd, "blackqueen")) piece = 'Q'; else
         if (!str_prefix(gamecmd, "whitebishop")) piece = 'b'; else
         if (!str_prefix(gamecmd, "blackbishop")) piece = 'B'; else
         if (!str_prefix(gamecmd, "whiteknight")) piece = 'n'; else
         if (!str_prefix(gamecmd, "blackknight")) piece = 'N'; else
         if (!str_prefix(gamecmd, "whiterook")) piece = 'r'; else
         if (!str_prefix(gamecmd, "blackrook")) piece = 'R'; else
         if (!str_prefix(gamecmd, "whitepawn")) piece = 'p'; else
         if (!str_prefix(gamecmd, "blackpawn")) piece = 'P'; else

         if (!str_prefix(gamecmd, "bq")) piece = 'Q'; else
         if (!str_prefix(gamecmd, "bb")) piece = 'B'; else
         if (!str_prefix(gamecmd, "bn")) piece = 'N'; else
         if (!str_prefix(gamecmd, "br")) piece = 'R'; else
         if (!str_prefix(gamecmd, "bp")) piece = 'P'; else
         if (!str_prefix(gamecmd, "wq")) piece = 'q'; else
         if (!str_prefix(gamecmd, "wb")) piece = 'b'; else
         if (!str_prefix(gamecmd, "wn")) piece = 'n'; else
         if (!str_prefix(gamecmd, "wr")) piece = 'r'; else
         if (!str_prefix(gamecmd, "wp")) piece = 'p'; else
         if (!str_prefix(gamecmd, "bk")) piece = 'K'; else
         if (!str_prefix(gamecmd, "wk")) piece = 'k';

         argument = one_argument( argument, sx );
         argument = one_argument( argument, sy );

         if ( (sx[0] == '\0') || (sy[0] == '\0') )
         {
            send_to_char("You must provide two coordinates, column and row.\n\r", ch);
            return;
         }

         if (!is_number( sy ))
         {
            send_to_char("You must provide two coordinates, letter column and number row.\n\r", ch);
            return;
         }

         if ((LOWER(sx[0]) < 'a') || (LOWER(sx[0]) > 'h'))
         {
            send_to_char("Valid letter coordinates are within a range of a to h.\n\r", ch);
            return;
         }

         x = LOWER(sx[0])-LOWER('a') + 1;
         y = atoi(sy);

         if ((y < 1) || (y > 8))
         {
            send_to_char("Valid number coordinates are within a range of 1 to 8.\n\r", ch);
            return;
         }

         for (gamedesc = obj->extra_descr; (gamedesc != NULL) && (str_cmp(gamedesc->keyword, "board gameboard game")); gamedesc = gamedesc->next) ;
         sprintf(newdesc, "%s", gamedesc->description);

         cur = newdesc[x*4+1 + (y-1)*76 + 125];
         if (cur == ' ')
         {
            newdesc[x*4+1 + (y-1)*76 + 125] = piece;
            free_string(gamedesc->description);
            gamedesc->description = str_dup(newdesc);
            switch (piece)
            {
               case 'X':
               send_to_char("You place a black stone on the gameboard.\n\r", ch);
               act( "$n places a black stone on the gameboard.", ch, NULL, NULL, TO_ROOM );
               break;
               case 'O':
               send_to_char("You place a white stone on the gameboard.\n\r", ch);
               act( "$n places a white stone on the gameboard.", ch, NULL, NULL, TO_ROOM );
               break;
               case 'K':
               send_to_char("You place a black king on the gameboard.\n\r", ch);
               act( "$n places a black king on the gameboard.", ch, NULL, NULL, TO_ROOM );
               break;
               case 'k':
               send_to_char("You place a white king on the gameboard.\n\r", ch);
               act( "$n places a white king on the gameboard.", ch, NULL, NULL, TO_ROOM );
               break;
               case 'q':
               send_to_char("You place a white queen on the gameboard.\n\r", ch);
               act( "$n places a white queen on the gameboard.", ch, NULL, NULL, TO_ROOM );
               break;
               case 'b':
               send_to_char("You place a white bishop on the gameboard.\n\r", ch);
               act( "$n places a white bishop on the gameboard.", ch, NULL, NULL, TO_ROOM );
               break;
               case 'n':
               send_to_char("You place a white knight on the gameboard.\n\r", ch);
               act( "$n places a white knight on the gameboard.", ch, NULL, NULL, TO_ROOM );
               break;
               case 'r':
               send_to_char("You place a white rook on the gameboard.\n\r", ch);
               act( "$n places a white rook on the gameboard.", ch, NULL, NULL, TO_ROOM );
               break;
               case 'p':
               send_to_char("You place a white pawn on the gameboard.\n\r", ch);
               act( "$n places a white pawn on the gameboard.", ch, NULL, NULL, TO_ROOM );
               break;
               case 'Q':
               send_to_char("You place a black queen on the gameboard.\n\r", ch);
               act( "$n places a black queen on the gameboard.", ch, NULL, NULL, TO_ROOM );
               break;
               case 'B':
               send_to_char("You place a black bishop on the gameboard.\n\r", ch);
               act( "$n places a black bishop on the gameboard.", ch, NULL, NULL, TO_ROOM );
               break;
               case 'N':
               send_to_char("You place a black knight on the gameboard.\n\r", ch);
               act( "$n places a black knight on the gameboard.", ch, NULL, NULL, TO_ROOM );
               break;
               case 'R':
               send_to_char("You place a black rook on the gameboard.\n\r", ch);
               act( "$n places a black rook on the gameboard.", ch, NULL, NULL, TO_ROOM );
               break;
               case 'P':
               send_to_char("You place a black pawn on the gameboard.\n\r", ch);
               act( "$n places a black pawn on the gameboard.", ch, NULL, NULL, TO_ROOM );
               break;
            }
         } else
         if (cur != piece)
         {
            send_to_char("A different piece is already there.\n\r", ch);
            return;
         } else
         {
            newdesc[x*4+1 + (y-1)*76 + 125] = ' ';
            free_string(gamedesc->description);
            gamedesc->description = str_dup(newdesc);
            switch (piece)
            {
               case 'X':
               send_to_char("You take a black stone from the gameboard.\n\r", ch);
               act( "$n takes a black stone from the gameboard.", ch, NULL, NULL, TO_ROOM );
               break;
               case 'O':
               send_to_char("You take a white stone from the gameboard.\n\r", ch);
               act( "$n takes a white stone from the gameboard.", ch, NULL, NULL, TO_ROOM );
               break;
               case 'K':
               send_to_char("You take a black king from the gameboard.\n\r", ch);
               act( "$n takes a black king from the gameboard.", ch, NULL, NULL, TO_ROOM );
               break;
               case 'k':
               send_to_char("You take a white king from the gameboard.\n\r", ch);
               act( "$n takes a white king from the gameboard.", ch, NULL, NULL, TO_ROOM );
               break;
               case 'q':
               send_to_char("You take a white queen from the gameboard.\n\r", ch);
               act( "$n takes a white queen from the gameboard.", ch, NULL, NULL, TO_ROOM );
               break;
               case 'b':
               send_to_char("You take a white bishop from the gameboard.\n\r", ch);
               act( "$n takes a white bishop from the gameboard.", ch, NULL, NULL, TO_ROOM );
               break;
               case 'n':
               send_to_char("You take a white knight from the gameboard.\n\r", ch);
               act( "$n takes a white knight from the gameboard.", ch, NULL, NULL, TO_ROOM );
               break;
               case 'r':
               send_to_char("You take a white rook from the gameboard.\n\r", ch);
               act( "$n takes a white rook from the gameboard.", ch, NULL, NULL, TO_ROOM );
               break;
               case 'p':
               send_to_char("You take a white pawn from the gameboard.\n\r", ch);
               act( "$n takes a white pawn from the gameboard.", ch, NULL, NULL, TO_ROOM );
               break;
               case 'Q':
               send_to_char("You take a black queen from the gameboard.\n\r", ch);
               act( "$n takes a black queen from the gameboard.", ch, NULL, NULL, TO_ROOM );
               break;
               case 'B':
               send_to_char("You take a black bishop from the gameboard.\n\r", ch);
               act( "$n takes a black bishop from the gameboard.", ch, NULL, NULL, TO_ROOM );
               break;
               case 'N':
               send_to_char("You take a black knight from the gameboard.\n\r", ch);
               act( "$n takes a black knight from the gameboard.", ch, NULL, NULL, TO_ROOM );
               break;
               case 'R':
               send_to_char("You take a black rook from the gameboard.\n\r", ch);
               act( "$n takes a black rook from the gameboard.", ch, NULL, NULL, TO_ROOM );
               break;
               case 'P':
               send_to_char("You take a black pawn from the gameboard.\n\r", ch);
               act( "$n takes a black pawn from the gameboard.", ch, NULL, NULL, TO_ROOM );
               break;
            }
         }
         return;
      }
      else
      {
         send_to_char("You cannot use the gameboard in that fashion.\n\r", ch);
         return;
      }

      send_to_char("You might wish to read the instructions on the board.\n\r", ch);
      return;
   }
   else
   if (obj->pIndexData->vnum == 4420) /* Compendium of the Drow Language */
   {
      char * dictionary[][2] = {
         { "'anon", "flower" },
         { "'anonen", "flowers" },
         { "'bgualyiz", "correct" },
         { "'bgualyizles", "correction" },
         { "'che", "loved" },
         { "'chev", "beloved" },
         { "'ranndi", "wife" },
         { "'sohna", "again" },
         { "'udtila", "does" },
         { "'zil", "as" },
         { "a", "at" },
         { "a", "by" },
         { "a'leai", "rare" },
         { "a'leaiil", "rarity" },
         { "a'ni", "general" },
         { "a'nshal'a", "ambassador" },
         { "a'quarth", "commission" },
         { "a'quarthus", "commissioned" },
         { "a'temra", "demoness" },
         { "a'till", "earned" },
         { "a'zil", "consorts" },
         { "aadk", "shout" },
         { "aadkat", "shouting" },
         { "abban", "ally" },
         { "abbanel", "affiliation" },
         { "abbanelith", "politics" },
         { "abbanelith'ha", "politic" },
         { "abbanelithl", "political" },
         { "abbanelithlne", "politically" },
         { "abbanen", "allies" },
         { "abbanid", "alliance" },
         { "abbaniden", "alliances" },
         { "abbil", "friend" },
         { "abbil", "comrade" },
         { "abbil", "comrad" },
         { "abbil", "trusted friend" },
         { "abbil'olplynir", "borrow" },
         { "abbilen", "friends" },
         { "abyl", "testicle" },
         { "acraba", "mastery" },
         { "acratura", "manage" },
         { "acraturae", "manages" },
         { "acraturain", "managing" },
         { "acraturaus", "managed" },
         { "acti'von", "participate" },
         { "ael", "abandon" },
         { "aelus", "abandoned" },
         { "aeros", "air" },
         { "afya", "health" },
         { "aglust", "part" },
         { "ahaluthh", "glorious" },
         { "aiqy", "cake" },
         { "air'oth", "air (element)" },
         { "ajak", "remember" },
         { "ak'nen", "brain" },
         { "ak'uech", "forge" },
         { "akai'zhar", "forsaken" },
         { "akana", "ruling" },
         { "akezhar", "nemisis" },
         { "akh", "group" },
         { "akh", "band" },
         { "al", "well" },
         { "al'doer", "welcome" },
         { "al'doere", "welcomes" },
         { "al'doerin", "welcoming" },
         { "al'doerus", "welcomed" },
         { "al'sanbone", "alertness" },
         { "al'vur", "cow" },
         { "alantha", "midnight" },
         { "aliss", "invite" },
         { "alissem", "invitation" },
         { "alkar", "forfeit" },
         { "alright", "collain" },
         { "altho", "during" },
         { "alti'ui", "wing" },
         { "alu", "go" },
         { "aluin", "going" },
         { "alur", "superior" },
         { "alur", "better" },
         { "alurl", "best" },
         { "alus", "gone" },
         { "alus", "went" },
         { "alust", "infront" },
         { "alust", "facing" },
         { "alust", "front" },
         { "aluve", "farewell" },
         { "aluve'", "farewell" },
         { "aluve'", "i leave you" },
         { "amankil", "clergy" },
         { "an'ar", "dancer" },
         { "an'kin", "teacher" },
         { "an'yui", "answer" },
         { "an'yuil", "answers" },
         { "andon", "clock" },
         { "andrew", "shite" },
         { "angelic", "robin" },
         { "angzuess", "harrower" },
         { "angzuessai", "harrowers" },
         { "anika'", "doesn't" },
         { "ann", "beg" },
         { "ann'ish", "appear" },
         { "annhan", "drawbridge" },
         { "annhannen", "drawbridges" },
         { "anth", "ring" },
         { "anthes", "rings" },
         { "antilles", "pilot" },
         { "anulo", "sky" },
         { "anulon", "skies" },
         { "anzzar", "sigil" },
         { "anzzarrim", "sigils" },
         { "aolth", "abase" },
         { "ap'za", "fate" },
         { "ap'zen", "luck" },
         { "ar'thalos", "development" },
         { "areion", "through" },
         { "arisa", "stalker" },
         { "arlathil", "nature" },
         { "arlyurl", "breast" },
         { "arlyurlen", "breasts" },
         { "armtes", "military" },
         { "arretwnh", "arwen" },
         { "arshes", "thunder" },
         { "asanque", "as you wish" },
         { "asanque", "likewise" },
         { "asanquel", "as you wish" },
         { "asizil", "unable" },
         { "aske'th", "asked" },
         { "aslu", "got" },
         { "asrogg", "ruin" },
         { "asse", "eagle" },
         { "aster", "truth" },
         { "astu", "undone" },
         { "astunin", "undoing" },
         { "astux", "undo" },
         { "atat", "rib" },
         { "atatal", "ribs" },
         { "aterruce", "creature" },
         { "athiyk", "spirit" },
         { "atsar", "catch" },
         { "atsarin", "catching" },
         { "atse", "catches" },
         { "atsus", "caught" },
         { "aturr", "used" },
         { "au", "near" },
         { "auflaque", "dog" },
         { "auflaque", "jackle" },
         { "auflaqui", "dogs" },
         { "autna", "granddaughter" },
         { "avariel", "eternity" },
         { "avelxune", "spied" },
         { "avrol", "april" },
         { "avuna", "fury" },
         { "awni", "soup" },
         { "awylir", "unwary" },
         { "axsa", "damnable" },
         { "azbahar'uss", "supremacy" },
         { "b'ahlach", "beholder" },
         { "b'luth", "recast" },
         { "b'luth'ol", "arrow" },
         { "b'luthe", "recasts" },
         { "b'luthel", "bolt" },
         { "b'luthin", "recasting" },
         { "b'luthyrr", "bow" },
         { "b'morfeth", "remake" },
         { "b'morfethe", "remakes" },
         { "b'morfethin", "remaking" },
         { "b'morfethus", "remade" },
         { "b'varess", "recall" },
         { "b'vecko", "strange" },
         { "b'veknin", "strangely" },
         { "ba'dor", "board" },
         { "ba'uren", "ankles" },
         { "bahk", "dominant" },
         { "bahk'tess", "dominate" },
         { "bakha", "manuer" },
         { "balbau", "grant" },
         { "barra", "shadow" },
         { "bashuk", "honour" },
         { "baur", "ankle" },
         { "baut'wael", "student" },
         { "bauth", "about" },
         { "bauth", "around" },
         { "bautha", "to dodge" },
         { "bautha", "dodge" },
         { "bein", "busy" },
         { "bekanan", "wiser" },
         { "bekea", "wise" },
         { "bekeanan", "wiser" },
         { "bel", "smellsaftersrain" },
         { "bel'la", "thank" },
         { "bel'la", "to honor" },
         { "bel'la", "honor" },
         { "bel'la", "to thank" },
         { "bel'la'n", "honoring" },
         { "bel'laon", "praising" },
         { "bel'laton", "thanking" },
         { "belaern", "profit" },
         { "belaern", "wealth" },
         { "belasco", "satan" },
         { "belbau", "to give" },
         { "belbau", "give" },
         { "belbaue", "gives" },
         { "belbaun", "giving" },
         { "belbaunin", "given" },
         { "belbaus", "gave" },
         { "belbol", "gift" },
         { "beldraein", "building" },
         { "beldro", "to create" },
         { "beldro", "create" },
         { "beldroin", "creator" },
         { "belgareth", "maze" },
         { "belggir", "braggart" },
         { "bera'lut", "bracelets" },
         { "bera'lut", "slavesshackle" },
         { "berrkigg", "fields" },
         { "biga", "shower" },
         { "bihurr", "explain" },
         { "bista", "short" },
         { "biu", "an" },
         { "blada", "council" },
         { "blas", "ant" },
         { "blyn", "seven" },
         { "blynar", "seventh" },
         { "blynol", "several" },
         { "blynszith", "seventy" },
         { "blynszithar", "seventieth" },
         { "bneir'pak", "shoot" },
         { "bol", "item" },
         { "boron", "advantage" },
         { "br'ornen", "bask" },
         { "bran", "loud" },
         { "brane'", "annoying" },
         { "brane'sa", "annoyance" },
         { "branell", "annoy" },
         { "branelle", "annoys" },
         { "branellus", "annoyed" },
         { "breliav", "gathers" },
         { "bri", "covered" },
         { "briz'esii", "distinguish" },
         { "bronretla", "wonderful" },
         { "brorn", "surprise" },
         { "brorn", "wonder" },
         { "brorna", "suprises" },
         { "brou", "pity" },
         { "brou'ka", "pitiful" },
         { "brouen", "pities" },
         { "bry", "heel" },
         { "brygn", "heels" },
         { "bryklyth", "tangled" },
         { "buat'leb", "sphere" },
         { "buki", "bloody" },
         { "bunjiro", "asshole" },
         { "bura", "obtained" },
         { "bura'u", "obtain" },
         { "bura'ue", "obtains" },
         { "but'laeb", "ball" },
         { "bwael", "good" },
         { "byr", "other" },
         { "byrren", "others" },
         { "c'nros", "witch" },
         { "c'rintri", "noble drow" },
         { "c'rintrin", "nobles" },
         { "c'thlan", "cope" },
         { "caghresst", "boil" },
         { "cahallin", "food" },
         { "cahlev'dar", "ophidian" },
         { "cahlind", "snake" },
         { "cal", "eat" },
         { "cal'tuu", "eaten" },
         { "calesset", "balls" },
         { "calestio", "pleasing" },
         { "calice", "cower" },
         { "caluss", "eater" },
         { "calussai", "eaters" },
         { "canakkzak", "mentality" },
         { "canthen", "forms" },
         { "cas", "gets" },
         { "ceagrass", "ale" },
         { "cel'droth", "brew" },
         { "cenk'si", "cents" },
         { "cha'kohk", "curse" },
         { "cha'kohkev", "cursed" },
         { "cha'ol", "incense" },
         { "chaer'el", "armorer" },
         { "chan", "cara" },
         { "chanash'gi", "rope" },
         { "chaon", "business" },
         { "chaos", "jesse" },
         { "char'riss", "message" },
         { "charnag", "deep" },
         { "charnaggen", "depths" },
         { "chath", "fire" },
         { "chath", "flame" },
         { "chath-palar", "pyre" },
         { "chathbualeab", "fireball" },
         { "che'el", "city" },
         { "che'elzen", "citizen" },
         { "chechatuso", "hanging" },
         { "chi'", "betray" },
         { "chi'dilok", "fallen paladin" },
         { "chi'qazz", "effulgent" },
         { "chi'us", "betrayed" },
         { "chiana", "betrayal" },
         { "chillala", "underwear" },
         { "chitt", "stench" },
         { "chkazzk", "clashing" },
         { "chonirfan", "acquaintance" },
         { "chu", "comes" },
         { "chu'tgera", "smoke" },
         { "chu'tgerin", "smoking" },
         { "chu'tgerus", "smoked" },
         { "chuth", "ash" },
         { "chuthen", "ashes" },
         { "cielra", "mysterious" },
         { "ciesattle", "intimidate" },
         { "ciiholtryui", "godess" },
         { "clarg'arth", "species" },
         { "clau", "dick" },
         { "clete", "dancersof" },
         { "clor", "rite" },
         { "clorrh", "rites" },
         { "cluta", "knuckle" },
         { "clutan", "knuckles" },
         { "co'ce", "cook" },
         { "co'nbluth", "non-drow" },
         { "colbauth", "known way" },
         { "colnbluth", "outsider" },
         { "colno'", "coat" },
         { "colno'en", "coats" },
         { "colnoh", "cape" },
         { "colnohen", "capes" },
         { "comp'sashnan", "ate" },
         { "comson", "crimson" },
         { "conamori", "comfort" },
         { "contri", "access" },
         { "contrir", "accessed" },
         { "corl", "bicep" },
         { "corlae", "biceps" },
         { "cot'rhea", "commence" },
         { "crastia", "apple" },
         { "craz", "brat" },
         { "cress", "web" },
         { "cressen", "webs" },
         { "cretok", "orc" },
         { "cri'n", "crying" },
         { "crup", "blame" },
         { "crupen", "blames" },
         { "crupowl", "blamed" },
         { "cruprou", "blaming" },
         { "cryso", "rose" },
         { "crysos", "roses" },
         { "crztil", "creative" },
         { "cth'ral", "calico" },
         { "cu'thezth", "compared" },
         { "cuass'ili", "room" },
         { "cydiur", "bar" },
         { "d'", "of" },
         { "d'anthe", "dear" },
         { "d'aron", "knight" },
         { "d'elezz", "though" },
         { "d'feza", "ode" },
         { "d'issan", "dawn" },
         { "d'jal", "all of you" },
         { "d'lil", "of the" },
         { "d'nilok", "paladin" },
         { "d'ril", "although" },
         { "d'sansha", "feces" },
         { "d'silva", "ineffective" },
         { "d'sreft", "blow" },
         { "d'thala", "dripping" },
         { "da'ramy", "daydream" },
         { "da're", "arm" },
         { "da'rex", "arms" },
         { "da'ur", "leg" },
         { "da'uren", "legs" },
         { "da'urzotreth", "kick" },
         { "daewl", "wish" },
         { "daewle", "wishes" },
         { "daewlin", "wishing" },
         { "daggar", "keeshe" },
         { "dajakk", "fix" },
         { "dajakke", "fixes" },
         { "dal", "from" },
         { "dalda'", "from" },
         { "dalhar", "child" },
         { "dalharen", "children" },
         { "dalharil", "daughter" },
         { "dalharuk", "son" },
         { "dalninil", "sister" },
         { "dalninin", "sisters" },
         { "dalninuk", "brother" },
         { "dalzhind", "history" },
         { "darthien", "elves" },
         { "darthiir", "surface elf" },
         { "darthiir", "fairies" },
         { "darthiir", "traitors" },
         { "darthiir", "surface elves" },
         { "darthirii", "elf" },
         { "darthirphor", "moonrise" },
         { "daur", "leg" },
         { "daxunyrr", "savior" },
         { "dazzan", "race" },
         { "de'", "of" },
         { "death", "aphyon" },
         { "deg'rabah", "trespass" },
         { "degahr", "trouble" },
         { "degahr'hi", "troublesome" },
         { "deis", "dreams" },
         { "deisenin", "dreaming" },
         { "dek'za", "legion" },
         { "del", "of" },
         { "della", "told" },
         { "delmah", "home" },
         { "delomaeth", "obsession" },
         { "derfi", "chicken" },
         { "dero'lyle", "possession" },
         { "derothel", "implements" },
         { "deshmal", "bye" },
         { "desu", "often" },
         { "detholar", "choose" },
         { "detholare", "chooses" },
         { "detholin", "choosing" },
         { "detholir", "choice" },
         { "detholus", "chose" },
         { "detholusin", "chosen" },
         { "detill", "subject" },
         { "dhrakta", "rapier" },
         { "dhyn", "mortal" },
         { "dhynen", "mortals" },
         { "di'morad", "actions" },
         { "dinien", "dexterous" },
         { "dispur", "abjure" },
         { "ditronw", "right" },
         { "ditronw'urn", "rightful" },
         { "divil", "inferior" },
         { "dizta", "sheep" },
         { "dkinoss", "else" },
         { "dkinosse'vel'klar", "elsewhere" },
         { "do'bauth", "to escape" },
         { "do'bauthin", "escaping" },
         { "do'ch", "gay" },
         { "do'suul", "outside" },
         { "dobluth", "outcast" },
         { "dobluthan", "outcasts" },
         { "dobor", "door" },
         { "doboren", "doors" },
         { "doeb", "out" },
         { "doer", "come" },
         { "doera", "become" },
         { "doerain", "becoming" },
         { "doerin", "coming" },
         { "doerrus", "came" },
         { "dofith", "flee" },
         { "dofithin", "fleeing" },
         { "dogheut", "wanders" },
         { "dol'ruth", "passing" },
         { "dormagyn", "save" },
         { "dorn", "i'll" },
         { "dorthan", "aids" },
         { "dortho", "aid" },
         { "dos", "you" },
         { "dos`misun", "misjudge" },
         { "dosman", "gentle" },
         { "dosst", "your" },
         { "dossta", "yours" },
         { "dosstan", "yourself" },
         { "dosstanen", "yourselves" },
         { "dothka", "drider" },
         { "dothkarn", "driders" },
         { "dra'kai", "jayme" },
         { "dra'malar", "duel" },
         { "draa", "two" },
         { "draak", "reptile" },
         { "draakarliik", "ettin" },
         { "drada", "second" },
         { "draeval", "time" },
         { "draevalen", "times" },
         { "draevalsp'los", "past" },
         { "dragazhar", "underdark bat" },
         { "draix", "center" },
         { "drasv", "year" },
         { "drasven", "years" },
         { "draszith", "twenty" },
         { "draszithael", "twenty-eight" },
         { "draszithal", "twenty-nine" },
         { "draszithar", "twentieth" },
         { "draszithdra", "twenty-two" },
         { "draszithla", "twenty-three" },
         { "draszithlyn", "twenty-seven" },
         { "draszithraun", "twenty-six" },
         { "draszithuel", "twenty-five" },
         { "draszithuen", "twenty-four" },
         { "draszithus", "twenty-one" },
         { "drathir", "moon" },
         { "dregzt", "drag" },
         { "drejan", "frost" },
         { "drelbersulm", "umberhulk" },
         { "drescable", "december" },
         { "drewst", "remove" },
         { "drewstin", "removing" },
         { "drider", "half-spider" },
         { "drider", "half-drow" },
         { "drin", "reasons" },
         { "drislved", "enhance" },
         { "drital", "drink" },
         { "dritalk", "drinks" },
         { "drith", "scorn" },
         { "drith'al", "scorned" },
         { "drith'na", "scornful" },
         { "drizlah", "tall" },
         { "drizlahrit", "taller" },
         { "dro", "life" },
         { "dro", "alive" },
         { "dro", "live" },
         { "dro'us", "lived" },
         { "dro'xun", "survive" },
         { "dro'xundus", "survival" },
         { "dro'xunen", "survives" },
         { "dro'xunou", "surviving" },
         { "dro'xunus", "survived" },
         { "drokk", "fist" },
         { "drolnar", "assasin" },
         { "dron", "lives" },
         { "dros'he", "purpose" },
         { "drostan", "metal" },
         { "druegar", "darkdwarf" },
         { "dryss'ho", "gather" },
         { "du'ased", "blessed" },
         { "du'nherz", "casually" },
         { "duanda", "mistake" },
         { "dubo", "hard" },
         { "duergan", "dwarves" },
         { "duergar", "deep dwarf" },
         { "duib", "stiff" },
         { "duifie", "fei" },
         { "duk'tak", "executioner" },
         { "dumo", "bless" },
         { "dumoas", "blessing" },
         { "duranfeh", "conviction" },
         { "duranfehen", "convictions" },
         { "dus'kin", "devious" },
         { "dusqup", "doomed" },
         { "duucald", "combat" },
         { "duul'sso", "free" },
         { "duulakah", "pie" },
         { "dwalc", "lonely" },
         { "dwen'del", "dwarf" },
         { "dwen'deles", "dwarfs" },
         { "dyne", "perfection" },
         { "dyunhk", "likes" },
         { "e'spdon", "archmage" },
         { "e'trit", "filth" },
         { "e'ussus", "stoke" },
         { "eairthil", "grounds" },
         { "eairthin", "ground" },
         { "echk", "hi" },
         { "edonil", "twilight" },
         { "eemsorr", "awesome" },
         { "eeoctsl", "coolest" },
         { "efst'ut", "aura" },
         { "eghin", "boerd" },
         { "ehmtau", "owns" },
         { "ehmth", "territory" },
         { "ehmtrud", "owned" },
         { "ehmtu", "posses" },
         { "ehmtu", "own" },
         { "ehmtua", "owner" },
         { "eit'jae", "implore" },
         { "el", "die" },
         { "el'an", "bestowed" },
         { "el'garderin", "economic" },
         { "el'garin", "resource" },
         { "el'inssrigg", "inn" },
         { "el'inssring", "tavern" },
         { "el'lar", "non-noble house" },
         { "el'tacch", "dissapoint" },
         { "el'ujol", "bottle" },
         { "elakar", "thy" },
         { "elam'aph", "devoted" },
         { "elamshin", "destiny" },
         { "elamshin", "the will of Lloth" },
         { "elamshinae", "favor" },
         { "elamshinae", "grace" },
         { "elar", "dies" },
         { "eld'chalok", "poisonous" },
         { "eldalie", "elven" },
         { "ele", "why" },
         { "elemmiire", "star" },
         { "elend", "usual" },
         { "elend", "traditional" },
         { "elendar", "continue" },
         { "elendar", "endure" },
         { "elendarin", "continuing" },
         { "elg'belbau", "cruse" },
         { "elg'cahal", "to poison" },
         { "elg'cahl", "poison" },
         { "elg'car", "whine" },
         { "elg'car", "complain" },
         { "elg'caress", "hag" },
         { "elg'caress", "bitch" },
         { "elg'carin", "complaining" },
         { "elg'carin", "whining" },
         { "elg'hasek", "ranger" },
         { "elge", "destruction" },
         { "elgg", "kill" },
         { "elgg", "slay" },
         { "elgg", "destroy" },
         { "elgg'e", "slays" },
         { "elgg'namba", "warhammer" },
         { "elgg'namban", "warhammers" },
         { "elgg'rilkh", "exterminate" },
         { "elggar", "kills" },
         { "elggarin", "slaying" },
         { "elggat", "destroyed" },
         { "elgge", "destroys" },
         { "elggen", "killed" },
         { "elggin", "killing" },
         { "elggor", "dying" },
         { "elggs", "destroyer" },
         { "elggur", "killer" },
         { "elggus", "slain" },
         { "elghinnfaer", "necromancy" },
         { "elghinyrr", "dead" },
         { "elghinyrr'fearn", "lich" },
         { "elghinyrrok", "deadly" },
         { "elghkhel", "skeleton" },
         { "elghkhellen", "skeletons" },
         { "elghliik", "skull" },
         { "elghliiken", "skulls" },
         { "elgluth", "whip" },
         { "elgluth", "scourge" },
         { "elgull", "offense" },
         { "elkil", "maim" },
         { "elkilen", "maimed" },
         { "elnoar", "ruthless" },
         { "elrie", "crush" },
         { "els'qatyu", "arbiter" },
         { "els'qatyu", "arbitrator" },
         { "eluith'orth", "faith" },
         { "emp'poss", "job" },
         { "emplaniilid", "fulfilled" },
         { "emptrem", "jasmine" },
         { "endar", "different" },
         { "enrot", "enslave" },
         { "enrothe", "enslaved" },
         { "entarer", "ornament" },
         { "enzzeir", "harbinger" },
         { "epistology", "thesstudysofsthesepistles" },
         { "equi", "experienced" },
         { "equidai", "skill" },
         { "er'griff", "only" },
         { "er'l", "rips" },
         { "erg'les", "question" },
         { "erkd'fe", "study" },
         { "erl'elee", "embrace" },
         { "erl'eleeus", "embraced" },
         { "ernin", "remainder" },
         { "eroln", "orcs" },
         { "errdegahr", "demon" },
         { "errgan", "mostly" },
         { "erth'netora", "summer" },
         { "erthe", "winter" },
         { "ertwed", "phone" },
         { "eryd", "weom" },
         { "esaph", "alike" },
         { "eszak", "worry" },
         { "et'zarreth", "ass" },
         { "euol", "gem" },
         { "euolniar", "clarification" },
         { "evagna", "forgive" },
         { "evven'to", "rape" },
         { "expulsion", "expulsion" },
         { "ezsakil", "worried" },
         { "ezual", "saying" },
         { "f'sarn", "i'm" },
         { "fa'la zatoast", "bastard" },
         { "fa'narow", "funny" },
         { "faentar", "activate" },
         { "faente", "activates" },
         { "faentin", "activating" },
         { "faentus", "activated" },
         { "faer", "magic" },
         { "faer'atca", "magical attack" },
         { "faerbol", "magic item" },
         { "faerl", "magical" },
         { "faern", "wizard" },
         { "faernen", "wizards" },
         { "faerz'ol", "spellbook" },
         { "faerz'ress", "magical strength" },
         { "faerz'un'arr", "seeress" },
         { "faerz'undus", "magic spell" },
         { "fag'ded", "sitting" },
         { "fahliell", "warlock" },
         { "falann", "spring" },
         { "falanni", "stream" },
         { "falduna", "praise" },
         { "faldunan", "praises" },
         { "falma", "wave" },
         { "falmar", "waves" },
         { "farjali", "precious" },
         { "fashka", "force" },
         { "fay'laph", "savor" },
         { "feher", "puppy" },
         { "feheri", "puppies" },
         { "fei'ir", "fair" },
         { "feir", "far" },
         { "feithin", "waiting" },
         { "fel", "ace" },
         { "felah", "further" },
         { "feldin", "invisible" },
         { "feryxonis", "underground" },
         { "fetear", "blazingsfire" },
         { "fetiner", "petition spetitioner" },
         { "fielthal", "contribution" },
         { "fielthalen", "contributions" },
         { "filut", "sense" },
         { "fin'rai", "harvestor" },
         { "fjolla'danne", "incompetence" },
         { "flamgra", "burn" },
         { "flamgra'in", "burning" },
         { "flasmix", "energy" },
         { "flatu'lar", "fart" },
         { "fle'nruid", "limit" },
         { "flen'los", "artery" },
         { "floh'l", "follower" },
         { "flohlu", "follow" },
         { "fol", "some" },
         { "folbol", "something" },
         { "foldraeval", "sometime" },
         { "foldraevals", "sometimes" },
         { "folggash", "friendship" },
         { "folkhel", "somebody" },
         { "folt", "such" },
         { "foluss", "someone" },
         { "zho'aminth", "forgotten" },
         { "fotus", "forth" },
         { "fowai", "weird" },
         { "fre'sla", "stand" },
         { "fredon", "august" },
         { "fridish", "salute" },
         { "fridj", "just" },
         { "frin'kla", "overload" },
         { "fris", "sent" },
         { "frustration", "rebanouim" },
         { "fty", "july" },
         { "fuck", "kiluki" },
         { "fuer'yon", "beast" },
         { "fuer'yonii", "beasts" },
         { "fuma", "family" },
         { "g'jahall", "defeat" },
         { "g'jahalle", "defeats" },
         { "g'jahallin", "defeating" },
         { "g'jahallus", "defeated" },
         { "g'rftte", "thanks" },
         { "g'waeklyth", "entangled" },
         { "g'yutri", "outsmart" },
         { "gaahr", "gray" },
         { "gaalsh'li", "secular" },
         { "gaer", "there" },
         { "gajak", "later" },
         { "gajen", "late" },
         { "galach", "amuse" },
         { "galavern", "universe" },
         { "galla", "trying" },
         { "gar'ven", "pleasant" },
         { "gareth", "strong" },
         { "garethur", "stronger" },
         { "garethurl", "strongest" },
         { "garr", "gargoyle" },
         { "gary", "willy" },
         { "gauka", "thirst" },
         { "gay", "kevin" },
         { "gekka", "homing" },
         { "genialaa", "lifts" },
         { "gethloc", "stalagtite" },
         { "ggaina", "swim" },
         { "ghefna", "quickest" },
         { "gheut", "wander" },
         { "ghil", "here" },
         { "gi", "oh" },
         { "gi'shal", "squish" },
         { "gil'ssrigghinn", "anyway" },
         { "gilaghu", "forsee" },
         { "gladare", "glad" },
         { "gleath", "militia" },
         { "glenn", "soldier" },
         { "glin", "hook" },
         { "glin'seir", "hooking" },
         { "glinoth", "hooked" },
         { "glit'r", "rip" },
         { "glit'ret", "ripping" },
         { "gluesa", "cheese" },
         { "gnoth", "yawn" },
         { "go'h", "pig" },
         { "go'hi", "pigs" },
         { "gol", "goblin" },
         { "gol'ssinss", "goblinslover" },
         { "golhyrr", "trick" },
         { "golhyrr", "ruse" },
         { "golhyrr", "trap" },
         { "goln", "goblins" },
         { "gordo", "whole" },
         { "gorgath", "desperate" },
         { "gorgathne", "desperately" },
         { "goros", "till" },
         { "gort", "goed" },
         { "gos", "goes" },
         { "gotfrer", "forget" },
         { "gow", "smell" },
         { "gow", "oder" },
         { "gragg", "violate" },
         { "gragg'reth", "violation" },
         { "gre'as'anto", "peace" },
         { "grentza", "oral" },
         { "groova", "trip" },
         { "growg", "pincer" },
         { "growgeg", "pincers" },
         { "grt", "wit" },
         { "grtn", "wits" },
         { "gu'e", "hey" },
         { "gukluth", "seemed" },
         { "gul", "ghost" },
         { "gultah", "offers" },
         { "gumash", "could" },
         { "gur'del", "insure" },
         { "gurrouh", "yuck" },
         { "guta", "hug" },
         { "guten", "hugs" },
         { "gutus", "hugged" },
         { "guu'lac", "doubt" },
         { "guuan", "wake" },
         { "guy'ya", "bear" },
         { "gyolaen", "oppose" },
         { "gyot", "pose" },
         { "h'aryn", "possess" },
         { "h'aryne", "possesses" },
         { "h'arynin", "possessing" },
         { "h'arynoin", "possessor" },
         { "h'arynus", "possessed" },
         { "h'ralenk", "poke" },
         { "h'ros", "can't" },
         { "h'sievss", "prophecies" },
         { "h'sievssrig", "prophecy" },
         { "h'sievssruk", "prophecied" },
         { "h'uena", "once" },
         { "ha'ishe", "sucks" },
         { "ha'uren", "thighs" },
         { "hacral", "hobbit" },
         { "halisstrad", "mystery" },
         { "halrathn", "saber" },
         { "halt", "thick" },
         { "har", "ear" },
         { "har'dro", "earth" },
         { "har'luth", "conquer" },
         { "har'ol", "cave" },
         { "har'ol", "cavern" },
         { "har'olen", "caverns" },
         { "har'oloth", "the underdark" },
         { "har'oloth", "underdark" },
         { "har'zian", "horizon" },
         { "haranth", "earring" },
         { "hargluk", "dwarf" },
         { "harglukkin", "dwarves" },
         { "harl", "down" },
         { "harl", "under" },
         { "harl", "below" },
         { "harl'il'cik", "kneel" },
         { "harlil", "bottom" },
         { "harlor", "stony" },
         { "haru", "wound" },
         { "haruk", "doom" },
         { "harventh", "cut" },
         { "harventh", "sever" },
         { "harventhe", "severs" },
         { "harventhel", "cutting" },
         { "harventhus", "severed" },
         { "has'tras", "monkey" },
         { "haska", "talking" },
         { "hass'l", "mean" },
         { "hass'len", "meant" },
         { "hasstn", "angry" },
         { "haszak", "illithid" },
         { "haszakkin", "illithids" },
         { "haur", "thigh" },
         { "hawressae", "insolence" },
         { "hawrest", "insolent" },
         { "heen", "course" },
         { "heeth", "shape" },
         { "heethir'ku", "shapeshifter" },
         { "hekbal", "riptide" },
         { "helbau", "province" },
         { "helothannin", "travel" },
         { "henotep", "almost" },
         { "her%b4op", "lollipop" },
         { "her'tan", "guild" },
         { "heru'nga", "support" },
         { "hery", "wounds" },
         { "hes'thar", "laughs" },
         { "hiever", "prophet" },
         { "hiran", "giant" },
         { "hithern", "oath" },
         { "ho'tuc", "happening" },
         { "hojh", "fail" },
         { "hojj", "fails" },
         { "hojjau", "failed" },
         { "hojjl", "failing" },
         { "hong'anto", "tranquility" },
         { "honglath", "calm" },
         { "honglath", "bravery" },
         { "hosse", "army" },
         { "hossen", "armies" },
         { "houj", "failure" },
         { "huela", "five" },
         { "huelar", "fifth" },
         { "hueszith", "fifty" },
         { "hueszithar", "fiftieth" },
         { "huin", "january" },
         { "huk'treemal", "fearfully" },
         { "huk'treemma", "fearful" },
         { "hull'phir", "prefer" },
         { "huthin", "next" },
         { "hwesta", "breeze" },
         { "hwuen", "until" },
         { "hyrr", "item" },
         { "hyrr'ol", "honest" },
         { "hyrr'olar", "honesty" },
         { "i'dol", "way" },
         { "i'dollen", "ways" },
         { "i'lluth", "pointless" },
         { "i'nik", "sand" },
         { "i'sav", "foundation" },
         { "i'ssuri", "mom" },
         { "iaurrh", "bridge" },
         { "iaurrhen", "bridges" },
         { "ib'ahalii", "glory" },
         { "ib'leua", "humble" },
         { "ibhurral", "explained" },
         { "ibihurr", "explains" },
         { "iblith", "excrement" },
         { "ichl", "too" },
         { "ick'neos", "arse" },
         { "idrasith", "acabus" },
         { "iglata", "promise" },
         { "iglatan", "promises" },
         { "iishry'dur", "torcy" },
         { "iiyola", "treasure" },
         { "iiyolasmapat'drachim", "treasuresmap" },
         { "ik'tarn", "pacifist" },
         { "ikal'daka", "wolves" },
         { "ikkask", "telling" },
         { "il", "she" },
         { "il'kahtical", "imbecile" },
         { "il'neathe", "freaks" },
         { "il'tess", "awaits" },
         { "il'yandrosa", "treachery" },
         { "il'yivera", "attractive" },
         { "ilfu'da", "visit" },
         { "ilhar", "mother" },
         { "ilhar", "to mother" },
         { "ilharess", "matron" },
         { "ilharessn", "matrons" },
         { "ilharn", "patron" },
         { "ilharn", "father" },
         { "ilhars", "mothers" },
         { "ilhyrr", "explanation" },
         { "ilhyrr'g", "explanations" },
         { "ilindith", "aim" },
         { "ilindith", "goal" },
         { "ilinsar", "medalyn" },
         { "ilinsar", "amulet" },
         { "ilinsar", "pendent" },
         { "ilinsar", "medalion" },
         { "ilkalik", "halt" },
         { "illing", "bone" },
         { "illistic", "fragile" },
         { "illithid", "mindsflayer" },
         { "illithor", "eternal servant" },
         { "illyiff", "beam" },
         { "ilniatu", "club" },
         { "ilninuk", "uncle" },
         { "ilquanthafer", "fulfillment" },
         { "ilro'e", "exactly" },
         { "ilstar", "rule" },
         { "ilt", "hers" },
         { "ilta", "her" },
         { "iltan", "herself" },
         { "ilthy'eo", "equal" },
         { "ilva'nesh", "effective" },
         { "ilythiiri", "drow" },
         { "ilythiirra", "drowish" },
         { "in'loil", "piece" },
         { "in'loilfrey", "shard" },
         { "in'tuil", "amount" },
         { "inalarn", "impatient" },
         { "inbal", "have" },
         { "inbaln't", "haven%27t" },
         { "inbalus", "had" },
         { "inbau", "get" },
         { "inbauin", "getting" },
         { "indar", "similar" },
         { "indarae", "opposite" },
         { "indo", "mood" },
         { "indola", "moody" },
         { "inhirt", "afore" },
         { "inlul", "small" },
         { "inlul'cal", "olive" },
         { "inlul'quar", "imp" },
         { "insaha", "poking" },
         { "inth", "plan" },
         { "inth", "stratagem" },
         { "inth", "scheme" },
         { "inthel", "rebel" },
         { "inthel'og", "rebellious" },
         { "inthen", "schemes" },
         { "inthigg", "agreement" },
         { "inthuul", "cold" },
         { "intooth", "dragoon" },
         { "inzormit", "generation" },
         { "iqualment", "simultaneous" },
         { "irr'jahk", "reality" },
         { "ishrass'ur", "tolerant" },
         { "isilme", "moonlight" },
         { "isintol", "interest" },
         { "issluilling", "collarbones" },
         { "ist'a", "vain" },
         { "isto", "night" },
         { "istolla", "nightfall" },
         { "istra'armesh", "armedsconflict" },
         { "it'x", "items" },
         { "itami", "start" },
         { "ivress", "fingers" },
         { "iwaotc", "person" },
         { "iz'", "as" },
         { "izil", "as" },
         { "izil", "able" },
         { "izilt", "appreciate" },
         { "izilted", "appreciated" },
         { "izin", "allow" },
         { "izwin", "known" },
         { "izznarg", "large" },
         { "j'nesst", "woman" },
         { "ja'hai'n", "accepting" },
         { "ja'haie", "accepts" },
         { "ja`hai", "accept" },
         { "ja`hai-na", "accepted" },
         { "jaael", "scratch" },
         { "jabar", "treacherous" },
         { "jabbress", "mistress" },
         { "jabbuk", "master" },
         { "jabplynn", "error" },
         { "jaed", "ignorant" },
         { "jak'li", "elite" },
         { "jakluth", "seeming" },
         { "jal", "all" },
         { "jal'klar", "anywhere" },
         { "jal'yur", "already" },
         { "jala", "any" },
         { "jalamzild", "anymore" },
         { "jalbol", "anything" },
         { "jalbyr", "another" },
         { "jalil", "female" },
         { "jalil'vedir", "mounted fighter" },
         { "jalilen", "females" },
         { "jalkhel", "anybody" },
         { "jallasat", "tolerate" },
         { "jallil", "lady" },
         { "jaluk", "male" },
         { "jalukul", "males" },
         { "jaluss", "anyone" },
         { "jas", "top" },
         { "jatha'la", "big" },
         { "jatha'laurl", "biggest" },
         { "jatha'ur", "bigger" },
         { "jejik", "maw" },
         { "jess", "fang" },
         { "jesshc", "fangs" },
         { "jhal", "but" },
         { "jhinrae", "wine" },
         { "jhun", "rune" },
         { "jhunai", "runes" },
         { "ji", "so" },
         { "jiala", "laugh" },
         { "jiala'in", "laughing" },
         { "jier", "hating" },
         { "jihard", "stick" },
         { "jiku", "rend" },
         { "jil", "jade" },
         { "jilorbb", "jade spider" },
         { "jimkal", "loser" },
         { "jindurn", "face" },
         { "jinique", "property" },
         { "jiniquiln", "properties" },
         { "jiole", "cool" },
         { "jiv'elgan", "torturer" },
         { "jiv'elgg", "torture" },
         { "jiv'elgg", "tortue" },
         { "jiv'ress", "torturer" },
         { "jiv'undus", "pain" },
         { "jiv'xund", "stress" },
         { "jiv'xunden", "stresses" },
         { "jivvenn", "plays" },
         { "jivviim", "hurt" },
         { "jivviim", "harm" },
         { "jivviimil", "hurted" },
         { "jivviimir", "hurts" },
         { "jivvin", "fun" },
         { "jivvin", "play" },
         { "jivvin", "amusing" },
         { "jivvuln", "played" },
         { "jivvust", "playing" },
         { "jjuna", "standing" },
         { "jjunle", "shades" },
         { "jlha'zen", "gnome" },
         { "jor", "rat" },
         { "jora", "rats" },
         { "joril", "junk" },
         { "joros", "ask" },
         { "jorosin", "asking" },
         { "jous", "show" },
         { "jresh", "scatter" },
         { "jrrooi", "jodi" },
         { "juba", "total" },
         { "juba'ss", "integrity" },
         { "jugare", "ok" },
         { "juloienm", "june" },
         { "jurhi'x", "growth" },
         { "jusith", "flask" },
         { "jusron", "wrath" },
         { "jusst", "questionablesfact" },
         { "jyzumar", "space" },
         { "k'alf", "direct" },
         { "k'hil", "nonsbeliever" },
         { "k'jakr", "still" },
         { "k'lar", "place" },
         { "k'lar", "location" },
         { "k'lararl", "pass" },
         { "k'rena", "korina" },
         { "k'shish", "elvish" },
         { "ka", "if" },
         { "ka'lith", "mercy" },
         { "kaas", "name" },
         { "kaaseel", "names" },
         { "kahwaqu'", "hand-crossbow" },
         { "kahzet", "accord" },
         { "kal'daka", "wolf" },
         { "kaliath'", "merciful" },
         { "kalyar", "illuminate" },
         { "kalye", "illuminates" },
         { "kalyin", "illuminating" },
         { "kalyza", "illumination" },
         { "kampi'un", "understand" },
         { "kampi'unus", "understood" },
         { "kanarvan", "longsword" },
         { "kaoveh", "cousin" },
         { "karadi", "link" },
         { "karilth", "purity" },
         { "karliik", "head" },
         { "karliiken", "heads" },
         { "karoth", "slaver" },
         { "karral", "swirl" },
         { "kat", "didn't" },
         { "katrill", "slaughter" },
         { "katrillen", "slaughtered" },
         { "katrillzecce", "slaughtering" },
         { "kaztirth", "cries" },
         { "kcisl", "glunk" },
         { "ke", "indeed" },
         { "ke'brei", "breaker" },
         { "ke'hl'ash", "bounty" },
         { "keer", "taught" },
         { "keeshe", "dagger" },
         { "keffal", "seen" },
         { "kel'nar", "dad" },
         { "kel'tahk", "invincible" },
         { "kelia", "bored" },
         { "kelliat", "daggar" },
         { "kenoth", "fake" },
         { "ker'nar", "add" },
         { "ker'narad", "added" },
         { "keradsh", "aback" },
         { "kerath", "carnage" },
         { "kerol", "oger" },
         { "kertest", "wash" },
         { "kestal", "hope" },
         { "kestan", "fullsofshope" },
         { "kev", "drop" },
         { "kghret", "pleasent" },
         { "khal", "to trust" },
         { "khal'abbil", "my trusted friend" },
         { "khaless", "trust" },
         { "khalessev", "trusted" },
         { "khaliizi", "stone" },
         { "khalith", "courage" },
         { "khalithan", "courageous" },
         { "khalithane", "courageously" },
         { "khazid'hea", "cutter" },
         { "khel", "body" },
         { "khluryar", "judge" },
         { "khluryare", "judges" },
         { "khluryin", "judging" },
         { "khlurysten", "judgement" },
         { "khluryus", "judged" },
         { "khowz", "slob" },
         { "khr'uss", "certainly" },
         { "khruste", "certain" },
         { "khurzoan", "worthier" },
         { "khurzon", "worthy" },
         { "khurzuil", "worthiest" },
         { "khyrpua", "harmful" },
         { "kiel", "slow" },
         { "kikkamm", "sickle" },
         { "kil'dobar", "counterfeit" },
         { "killian", "sword" },
         { "killianen", "swords" },
         { "kissa", "swiftly" },
         { "kitrye", "half" },
         { "kivvil", "surfacer" },
         { "kivvin", "surfacers" },
         { "kke", "rude" },
         { "kku", "finish" },
         { "kkublyn'tan", "weekend" },
         { "kkuuth", "finished" },
         { "kl'ae", "use" },
         { "kl'aein", "using" },
         { "kl'eril", "useless" },
         { "kla'ath", "serve" },
         { "kla'athin", "serving" },
         { "kleel", "important" },
         { "klen'ne", "doctor" },
         { "klev", "tool" },
         { "klew'ar", "moment" },
         { "klew'kin", "hour" },
         { "klew'kinnen", "hours" },
         { "klew'nor", "minute" },
         { "klez", "thing" },
         { "klezn", "things" },
         { "klithophe", "tangling" },
         { "klu'chud", "spawn" },
         { "kluchud'ssussun", "color" },
         { "kluth", "seem" },
         { "kluthak", "seems" },
         { "klyth", "tangle" },
         { "klythmaril", "entangling" },
         { "kmindu", "indomitable" },
         { "knan", "grow" },
         { "knif", "glow" },
         { "knif'rt", "glowing" },
         { "ko", "uh" },
         { "kopel", "fugly" },
         { "kor", "anger" },
         { "kor'in", "furry" },
         { "kor'inth", "rage" },
         { "korishi", "knock" },
         { "korit'al", "snow" },
         { "korriless", "vaguely" },
         { "korrle", "vague" },
         { "kostith", "holding" },
         { "kr'athin", "ready" },
         { "kr'athina", "readiness" },
         { "kraltok", "taco" },
         { "kre'j", "begin" },
         { "kre'jil", "begining" },
         { "kre'tan", "morning" },
         { "kreth'el", "regret" },
         { "kreth'el", "repent" },
         { "kri'sha", "cover" },
         { "krik'vlicss", "tears" },
         { "kristass", "instant" },
         { "krithyj", "murderess" },
         { "krrja", "puzzle" },
         { "krrjat", "puzzled" },
         { "kruk", "state" },
         { "kry'stona", "place of caves" },
         { "ktonos", "slayer" },
         { "ku'lam", "rise" },
         { "ku'nal", "faithful" },
         { "kuk", "staff" },
         { "kul'gobsula", "prison" },
         { "kul'gobuss", "prisoner" },
         { "kulauk", "drool" },
         { "kulg", "snag" },
         { "kulg", "blockage" },
         { "kulggen", "shield" },
         { "kulggen", "barrier" },
         { "kulith", "feelings" },
         { "kult'ressin", "ruler" },
         { "kultar", "rules" },
         { "kuma", "kobold" },
         { "kus", "send" },
         { "kuttra", "mushroom" },
         { "kuuv", "dare" },
         { "ky'hul", "cloth" },
         { "ky'huras", "clothing" },
         { "ky'ostal", "armor" },
         { "ky'ov'aer", "magic wards" },
         { "kyikh", "age" },
         { "kyil", "dart" },
         { "kyilen", "darts" },
         { "kylara", "lethal" },
         { "kyon", "care" },
         { "kyona", "carefulness" },
         { "kyone", "alert" },
         { "kyone", "careful" },
         { "kyone", "alertly" },
         { "kyor", "saw" },
         { "kyor'ol", "expect" },
         { "kyorl", "watch" },
         { "kyorl", "wait" },
         { "kyorl", "guard" },
         { "kyorl", "see" },
         { "kyorl'n", "witnesses" },
         { "kyorl'urn", "watchful" },
         { "kyorlen", "watchers" },
         { "kyorli", "watcher" },
         { "kyorlin", "watching" },
         { "kyreshorl", "obvious" },
         { "kyreshorlh", "obviously" },
         { "kyuvr", "breed" },
         { "kyuvrem", "breeding" },
         { "kzzo", "blah" },
         { "l'", "the" },
         { "l'amith", "enjoy" },
         { "l'ashm", "armada" },
         { "l'elamshin", "the destiny" },
         { "l'elend", "the traditional" },
         { "l'essnil", "wench" },
         { "l'ikranzh", "thence" },
         { "l'inya", "gaze" },
         { "l'lonna", "wither" },
         { "l'lonneal", "withered" },
         { "l'puul", "foul" },
         { "l'quack", "countenance" },
         { "l'sith", "address" },
         { "l'thi", "animal" },
         { "la'uren", "knees" },
         { "lac", "seek" },
         { "lac'na", "seeks" },
         { "lac'nala", "seeker" },
         { "lac'nou", "seeking" },
         { "ladir", "ladder" },
         { "lael", "eight" },
         { "laelar", "eighth" },
         { "laelszith", "eighty" },
         { "laelszithar", "eightieth" },
         { "lamin", "shines" },
         { "lani", "keeper" },
         { "lanke", "lake" },
         { "lann", "wear" },
         { "lanne", "wears" },
         { "lannin", "wearing" },
         { "lannus", "wore" },
         { "laoles", "praised" },
         { "lar", "summon" },
         { "lar", "call" },
         { "lar'aen", "summoning" },
         { "lar'ild", "calling" },
         { "larnde", "lavender" },
         { "lasinian", "sheath" },
         { "lasse", "leaf" },
         { "lassen", "leaves" },
         { "lassrinn", "tame" },
         { "lassrinn", "break" },
         { "lassrinnb", "tamed" },
         { "lassrinneh", "taming" },
         { "lassrinnuss", "tamer" },
         { "laur", "knee" },
         { "lauske", "hair" },
         { "lawat", "table" },
         { "leith", "fish" },
         { "lentan", "smirk" },
         { "lerg", "baby" },
         { "lerut'l", "coal" },
         { "less", "mzuld" },
         { "lest", "lets" },
         { "leza", "bit" },
         { "lgor", "figure" },
         { "lgoruth", "figured" },
         { "ligah", "native" },
         { "ligrr", "girl" },
         { "ligrrabbil", "girlfriend" },
         { "liiko", "wax" },
         { "lil", "the" },
         { "lil'alur", "fightsnow" },
         { "lil'alure", "dance" },
         { "lil'alurear", "dances" },
         { "lilbh'iahin", "joy" },
         { "lilbh'iahinen", "joys" },
         { "linath", "song" },
         { "linathen", "songs" },
         { "lince'sa", "pets" },
         { "linddil", "lizardman" },
         { "lindua", "sings" },
         { "linoin", "west" },
         { "lintaguth", "platter" },
         { "linth'el", "cauldron" },
         { "linthar", "bard" },
         { "lintharen", "bards" },
         { "linthel", "melody" },
         { "linthelea", "melodious" },
         { "linthelen", "melodies" },
         { "linthre", "brazier" },
         { "lish", "super" },
         { "lishcalillisticandroz", "supercalifragilisticexpallidocious" },
         { "lisse'", "harmony" },
         { "lisse'lea", "harmonious" },
         { "literaw", "abash" },
         { "lith'lossta", "intimate" },
         { "llaar", "really" },
         { "llanath", "presence" },
         { "llar", "three" },
         { "llarh", "babe" },
         { "llarl", "adamantine" },
         { "llarnbuss", "third" },
         { "llarszith", "thirty" },
         { "llarszithael", "thirty-eight" },
         { "llarszithal", "thirty-nine" },
         { "llarszithar", "thirtieth" },
         { "llarszithdra", "thirty-two" },
         { "llarszithla", "thirty-three" },
         { "llarszithlyn", "thirty-seven" },
         { "llarszithraun", "thirty-six" },
         { "llarszithuel", "thirty-five" },
         { "llarszithuen", "thirty-four" },
         { "llarszithus", "thirty-one" },
         { "lle'isgar", "rising" },
         { "lle'warin", "miss" },
         { "lle'warinil", "missed" },
         { "llentol", "quietly" },
         { "llieh", "perfect" },
         { "llinshin", "lunch" },
         { "lloun", "art" },
         { "lloun'az", "artist" },
         { "lo'larox", "phoenix" },
         { "lo'peria", "abaft" },
         { "lodias", "people" },
         { "loff'ta", "happy" },
         { "loff'tarien", "happiness" },
         { "loktriam", "guy" },
         { "loniar", "wet" },
         { "lor", "look" },
         { "lor'", "oxygen" },
         { "loril", "looked" },
         { "lorith", "looking" },
         { "lorr", "air" },
         { "lorug", "tree" },
         { "lorugen", "trees" },
         { "lorulm", "wood" },
         { "losva", "thumb" },
         { "losval", "thumbs" },
         { "lotha", "little" },
         { "lsyt", "sly" },
         { "lu'", "and" },
         { "lu'oh", "how" },
         { "luent", "east" },
         { "lueth", "and" },
         { "luf'la", "tosplans" },
         { "lukkil", "ratman" },
         { "lul", "stray" },
         { "luth", "cast" },
         { "luth", "throw" },
         { "luth", "hurl" },
         { "luth'ol", "spear" },
         { "luth'ol", "javelin" },
         { "luthe", "casts" },
         { "luthk", "thrown" },
         { "luthtar", "gladly" },
         { "ly'ryus", "letter" },
         { "m'elzar", "mage" },
         { "m'elzaren", "mages" },
         { "m'garr", "midgard" },
         { "m'ranndii", "mate" },
         { "m'ranndii", "husband" },
         { "m'rstirans", "fromsnowson" },
         { "m'sig", "music" },
         { "m'thain", "priest" },
         { "maav'at", "ignorance" },
         { "madari", "signal" },
         { "maelthra", "dragon (young)" },
         { "maglust", "apart" },
         { "maglust", "alone" },
         { "magthere", "school" },
         { "magtheres", "schools" },
         { "mal", "fights" },
         { "mal'ai", "idiot" },
         { "mal'rak", "forever" },
         { "mala", "manners" },
         { "malar", "fight" },
         { "malarin", "fighting" },
         { "malaruth", "fought" },
         { "maligore", "crafty" },
         { "malla", "honored" },
         { "mamulen", "chest" },
         { "manner", "valor" },
         { "mapat'drachim", "map" },
         { "mar'ius", "extreme" },
         { "maral", "storm" },
         { "marala", "storms" },
         { "maristo", "tonight" },
         { "marofke", "bell" },
         { "martheo", "situation" },
         { "mauhni", "compel" },
         { "mauhnie", "compels" },
         { "mauhnin", "compelling" },
         { "mauhnius", "compelled" },
         { "maunech", "burden" },
         { "maunq'a", "whether" },
         { "maurna", "boat" },
         { "may'salkiriiss", "nightshadow" },
         { "mayar", "chance" },
         { "mayoe", "maybe" },
         { "mechrola", "swallow" },
         { "medose", "harder" },
         { "medosek", "early" },
         { "medri", "death bringer" },
         { "meifft", "balance" },
         { "melaer", "fight with magic" },
         { "meldrin", "adore" },
         { "meldrine", "adores" },
         { "meldrinin", "adoring" },
         { "meldrinus", "adored" },
         { "melee", "fight with weapons" },
         { "mem", "stomach" },
         { "menah", "wide" },
         { "menvis", "path" },
         { "menvissen", "paths" },
         { "micamer", "ma'am" },
         { "mii'n", "means" },
         { "mildrin", "single" },
         { "mimun", "weaken" },
         { "mina", "them" },
         { "minasien", "mystic" },
         { "minasienal", "mystical" },
         { "mir", "to hold" },
         { "mir", "hold" },
         { "mir'cahallin", "jar" },
         { "mir'ply", "fluffy" },
         { "mir't", "pulled" },
         { "mirar", "holds" },
         { "mirshann", "angel" },
         { "mith", "grey" },
         { "mithuth", "sin" },
         { "miulk'ars", "minions" },
         { "mizil'ra", "matters" },
         { "mizil'ros", "questions" },
         { "ml'aen", "sight" },
         { "molerngoth", "dragonclaw" },
         { "molo", "wraith" },
         { "mon'tu", "point" },
         { "mora", "act" },
         { "morad", "action" },
         { "mordivvin", "mock" },
         { "morfel", "made" },
         { "morfelith", "making" },
         { "morfeth", "make" },
         { "morfethar", "makes" },
         { "morn'lo", "heat" },
         { "morngha", "gloomy" },
         { "mortath", "maker" },
         { "mrannd'ssinss", "lover (male)" },
         { "mri'kul", "lock" },
         { "mri'kul", "to lock" },
         { "mrigg", "to inspire" },
         { "mrigg", "inspire" },
         { "mrigg", "guide" },
         { "mrigg", "to guide" },
         { "mriggan", "guidance" },
         { "mrigge", "guides" },
         { "mriggin", "guiding" },
         { "mriggus", "guided" },
         { "mrim'ol", "key" },
         { "mrimmd'ssinss", "lover (female)" },
         { "mru'nga", "encourage" },
         { "mru'ngae", "encourages" },
         { "mru'ngain", "encouraging" },
         { "mru'shka", "tempt" },
         { "mua", "foe" },
         { "muatrix", "matrix" },
         { "mufiga", "yesterday" },
         { "mufo", "tomorrow" },
         { "mumbaro", "move" },
         { "murrpau", "feline" },
         { "murrpau", "cat" },
         { "muth", "found" },
         { "myar", "bad" },
         { "mylthar", "devour" },
         { "myrloch", "astral" },
         { "myrlochar", "soul spider" },
         { "myrxle", "miracle" },
         { "mzil", "many" },
         { "mzild", "more" },
         { "mziln", "also" },
         { "mziln", "extra" },
         { "mziln", "additional" },
         { "mzilst", "most" },
         { "mzilt", "much" },
         { "mzulst", "least" },
         { "n'abyl", "testicles" },
         { "n'belaern", "debt" },
         { "n'corda", "contract" },
         { "n'kyorl", "await" },
         { "n'kyorlin", "awaiting" },
         { "n'shtyl", "slightest" },
         { "n'tol", "vicious" },
         { "na'ceroth", "barbarian" },
         { "na'eroth", "trembling" },
         { "na'rescho", "uneasy" },
         { "na'uren", "hips" },
         { "naak'e", "jealous" },
         { "nacta", "pierce" },
         { "nadorhuan", "coward" },
         { "nalt'chaxxan", "murderous" },
         { "namba", "hammer" },
         { "namban", "hammers" },
         { "naqualla", "displease" },
         { "nar", "wich" },
         { "nar'harcan", "vendetta" },
         { "nar'hethi", "vengence" },
         { "narikia", "nymph" },
         { "narkuth", "scroll" },
         { "narrakan", "shy" },
         { "nat'raeth", "connected" },
         { "nat'tuk", "gin" },
         { "natha", "a" },
         { "nau", "no" },
         { "nau'shind", "forbid" },
         { "nau'shindcal", "fruit" },
         { "nau'shindin", "forbidding" },
         { "nau'vin", "unusual" },
         { "naubol", "nothing" },
         { "naucrup", "blameless" },
         { "naudal", "across" },
         { "naukhel", "nobody" },
         { "nauphlyle", "claimless" },
         { "nauqu'allath", "displeased" },
         { "nauqualla", "displease" },
         { "naur", "hip" },
         { "nauritz", "valueless" },
         { "naurka", "harsh" },
         { "nausbyr", "neither" },
         { "nausfi", "nonsense" },
         { "nausrigg'tul", "displeasure" },
         { "naust", "none" },
         { "nausura", "naked" },
         { "naut", "not" },
         { "naut'kyn", "blind" },
         { "naut'shinder", "forbidden" },
         { "naut-elghinyrr", "undead" },
         { "nautya", "hostile" },
         { "nauvost", "ceaseless" },
         { "nauxahuu", "refuse" },
         { "nauxxizz", "helpless" },
         { "nauxxizzna", "helplessly" },
         { "ne'enuvar", "waterlily" },
         { "ne'has're", "new clothes" },
         { "ne'kales", "distrust" },
         { "ne'kalsa", "nottrusting" },
         { "neantak", "void" },
         { "nec'perya", "sanctuary" },
         { "neccasalmor'", "necromancer" },
         { "neccasalmor'en", "necromancers" },
         { "neera", "becomes" },
         { "neerrh", "salvage" },
         { "neeveembee", "november" },
         { "nei", "empress" },
         { "neitar", "never" },
         { "nek", "slut" },
         { "nelgeth", "to forgive" },
         { "nelgetha", "forgiveness" },
         { "nelgethaun", "forgiven" },
         { "nelid'", "tentacle" },
         { "nempori", "ugly" },
         { "nempori'phish", "magenta" },
         { "nereth'ul", "dictionnary" },
         { "nesst", "man" },
         { "nesstren", "men" },
         { "nez", "win" },
         { "nezcau'bol", "benefit" },
         { "nezdous", "prevail" },
         { "nezmuth", "won" },
         { "ni'sfuer", "tosbegs%3astosplead" },
         { "niar", "water" },
         { "niar'haanin", "river" },
         { "niar'uuthli", "teal" },
         { "niaren", "waters" },
         { "nibele", "game" },
         { "nibeless", "games" },
         { "nictus", "hopping" },
         { "nicushacahmes", "enlightened" },
         { "nienar", "mourn" },
         { "nienare", "mourns" },
         { "nigunti'kar", "genitals" },
         { "niir'ls", "tear" },
         { "nika", "stranger" },
         { "niki", "strangers" },
         { "nilah", "misery" },
         { "nin", "now" },
         { "nin", "present time" },
         { "nin'jahr", "thus" },
         { "nind", "they" },
         { "nindel", "that" },
         { "nindol", "this" },
         { "nindolen", "these" },
         { "nindyn", "those" },
         { "nint", "theirs" },
         { "ninta", "their" },
         { "nintan", "themselves" },
         { "nir'sel", "bath" },
         { "nirgon", "madman" },
         { "niss'nir", "lack" },
         { "nixm'io", "she%27ll" },
         { "niyar", "weep" },
         { "niye", "weeps" },
         { "niyin", "weeping" },
         { "nizzik", "sharpen" },
         { "nizzikef", "sharpened" },
         { "nizzikn", "sharpening" },
         { "nizziknir", "sharpener" },
         { "nizzre'", "lightning" },
         { "nl'gnos", "missile" },
         { "nna'biss", "tug" },
         { "nnabiss", "pull" },
         { "noa", "lose" },
         { "noalith", "neutral" },
         { "noamuth", "lost" },
         { "noamuth", "unknown" },
         { "noamuth", "wanderer" },
         { "noriam", "count" },
         { "norrs", "mouth" },
         { "nota'man", "devil" },
         { "nou'hai", "concern" },
         { "nrunn", "bore" },
         { "nrunnin", "boring" },
         { "nrunnkel", "boredom" },
         { "nug'ri", "hungry" },
         { "nuij", "fiend" },
         { "num're", "sail" },
         { "numl", "sad" },
         { "numl'esstu", "sadness" },
         { "nuor", "roll" },
         { "nuore", "rolls" },
         { "nuorin", "rolling" },
         { "nuorus", "rolled" },
         { "nuw", "jaw" },
         { "nuwei", "jaws" },
         { "nwn't", "untold" },
         { "ny", "covet" },
         { "nygurath", "powerfull" },
         { "nyln", "coveted" },
         { "nym'uer", "hear" },
         { "nym'uerus", "heard" },
         { "nyrr", "coveting" },
         { "nythe", "floor" },
         { "nzet", "neat" },
         { "o'dad", "druid" },
         { "o'goth", "heal" },
         { "o'gothe", "heals" },
         { "obok", "high" },
         { "obok'ssuss", "noon" },
         { "obokur", "higher" },
         { "obokurl", "highest" },
         { "obsu'arl", "trapdoor" },
         { "obsul", "opening" },
         { "obsul", "gap" },
         { "obsul'e", "portal" },
         { "obsul'ress", "level" },
         { "ocan", "mole" },
         { "octobre", "october" },
         { "ofil'nisha", "clothes" },
         { "og'elend", "heretic" },
         { "og'elend", "traitor" },
         { "og'elendar", "treason" },
         { "ogglin", "rival" },
         { "ogglinn", "enemy" },
         { "ogglinnar", "enemies" },
         { "ogglir", "to rival" },
         { "ogglirin", "rivaling" },
         { "ogglit", "opponent" },
         { "oirna", "eternal" },
         { "ol", "it" },
         { "ol'elg", "assassinate" },
         { "ol'elghinn", "assassination" },
         { "olath", "dark" },
         { "olath'yukin", "brown" },
         { "olathur", "darker" },
         { "olathurl", "darkest" },
         { "olis'inth", "plot" },
         { "olis'inth", "conspiracy" },
         { "olis'inthen", "plots" },
         { "olis'inthigg", "coup" },
         { "olist", "caution" },
         { "olist", "stealth" },
         { "olister", "acquire" },
         { "olog", "troll" },
         { "oloth", "darkness" },
         { "olplyen", "thieves" },
         { "olplyn", "thief" },
         { "olplynir", "steal" },
         { "olt", "its" },
         { "oltan", "itself" },
         { "olva", "plant" },
         { "omegarialt", "abandonment" },
         { "onhasta", "dedicated" },
         { "onhir", "worship" },
         { "onhire", "worships" },
         { "onhirin", "worshipping" },
         { "onnhar", "loan" },
         { "onnharin", "loaning" },
         { "onnhe", "loans" },
         { "onnhil", "loaner" },
         { "onnhillen", "loaners" },
         { "onnhus", "loaned" },
         { "ooble'", "tongue" },
         { "ooble'en", "tongues" },
         { "oolos", "fools" },
         { "op'elg", "pirate" },
         { "op'elgin", "piracy" },
         { "op'elgin", "thieving" },
         { "op'lin", "spilt" },
         { "opholudou", "fascinating" },
         { "or'a", "money" },
         { "or'shael", "sacrifical" },
         { "or'shanse", "sacrifice" },
         { "or'shansein", "sacrificing" },
         { "or'shansene", "sacrifices" },
         { "or'shausedus", "sacrificial" },
         { "orbb", "spider" },
         { "orbbcress", "spiderweb" },
         { "orbben", "spiders" },
         { "orbdrin", "mask" },
         { "orbdrinus", "masked" },
         { "oreb", "raven" },
         { "ori'gato", "let" },
         { "orior", "prepare" },
         { "orl'nak", "pillow" },
         { "orlenggin", "altar" },
         { "orlime", "dreary" },
         { "orlingg", "bed" },
         { "orn", "will" },
         { "orn'la", "would" },
         { "orofarne", "dwelling" },
         { "orofarnen", "dwellings" },
         { "orrl", "scorpion" },
         { "orshal", "challice" },
         { "ortelanth", "pray" },
         { "ortelanthe", "prays" },
         { "ortelanthin", "praying" },
         { "ortelassa", "prayer" },
         { "ortelassan", "prayers" },
         { "orthae", "holy" },
         { "orthae", "sacred" },
         { "orthelanthus", "prayed" },
         { "orvelve", "sacrificial dagger" },
         { "orvulae", "stiletto" },
         { "ou'tix", "actually" },
         { "oubran", "loudly" },
         { "oubwael", "goodness" },
         { "ouhyll", "match" },
         { "oukke", "rudely" },
         { "oura", "tiger" },
         { "ouvalyrin", "kindness" },
         { "ouwaela", "foolishness" },
         { "p'leik", "suffering" },
         { "p'los", "before" },
         { "p'luin", "after" },
         { "p'luvt", "prey" },
         { "p'obon", "mark" },
         { "p'wal", "because" },
         { "pa'das", "panther" },
         { "pahntar", "open" },
         { "pahnte", "opens" },
         { "pahntus", "opened" },
         { "paken", "took" },
         { "pand", "include" },
         { "pande", "includes" },
         { "pandin", "including" },
         { "pandus", "included" },
         { "parth", "lawn" },
         { "paxil", "decay" },
         { "pera'dene", "scapegoat" },
         { "peri'sol", "perish" },
         { "pes'cis", "spice" },
         { "ph'", "are" },
         { "ph'ashya", "adria" },
         { "phalar", "grave" },
         { "phalar", "marker" },
         { "philth", "despise" },
         { "phindar", "monster" },
         { "phish", "red" },
         { "phla'ta", "terrible" },
         { "phlith", "hate" },
         { "phlithe", "hates" },
         { "phlithus", "hated" },
         { "phlyle", "claim" },
         { "phlynn", "claims" },
         { "phlyou", "claiming" },
         { "phlyus", "claimed" },
         { "pholod", "onto" },
         { "pholor", "on" },
         { "pholor", "upon" },
         { "pholor'katri", "onslaught" },
         { "pholud", "fascinate" },
         { "pholudeq", "fascinated" },
         { "phor", "above" },
         { "phor", "up" },
         { "phor", "over" },
         { "phord", "read" },
         { "phraktos", "gods" },
         { "phreng", "realm" },
         { "phrengen", "realms" },
         { "phu'", "are" },
         { "phuul", "are" },
         { "pi'rescho", "easily" },
         { "pict", "pick" },
         { "pictin", "picking" },
         { "pidoa", "weirdest" },
         { "pielch", "stab" },
         { "pirkuloitte", "looting" },
         { "piwafwi", "cloak" },
         { "plak'la", "gold" },
         { "plak'la'la", "golden" },
         { "platinum", "platinum" }, /* someone had "penis" here as the word.  This some kind of joke? */
         { "plica", "neck" },
         { "plithou", "hateful" },
         { "plithut", "hatred" },
         { "ply'usaerth", "kidnap" },
         { "ply'uss", "arrest" },
         { "ply'uss", "capture" },
         { "ply'ussiln", "captivate" },
         { "ply'ussilnaa", "captivating" },
         { "plyken", "buy" },
         { "plynn", "take" },
         { "plynn", "seize" },
         { "plynnet", "taken" },
         { "plynnil", "write" },
         { "plynnith", "writen" },
         { "plynnithus", "writing" },
         { "plyussk", "captor" },
         { "plyussq", "captive" },
         { "polah", "swab" },
         { "poop", "stoop" },
         { "pragray'l", "paranoid" },
         { "priqual", "problem" },
         { "prollussk", "boys" },
         { "psught", "independence" },
         { "ptau'a", "attend" },
         { "ptau'al", "attention" },
         { "puis'tary", "unfortunately" },
         { "pussy", "puisin" },
         { "q'zaky", "mom" },
         { "qee", "fast" },
         { "qee'lak", "quick" },
         { "qeeh", "faster" },
         { "qeesuaco", "whirlwind" },
         { "qloav", "lips" },
         { "qlov", "lip" },
         { "qo", "clip" },
         { "qos", "dread" },
         { "qos'su", "dreading" },
         { "qosou", "dreaded" },
         { "qreec", "immense" },
         { "qu'abban", "house agent" },
         { "qu'el'faeruk", "house wizard" },
         { "qu'el'saruk", "house weapons master" },
         { "qu'el'velguk", "house assassin" },
         { "qu'elaeruk", "sorcerer" },
         { "qu'ellar", "house" },
         { "qu'ellaril", "renegade" },
         { "qu'ess", "prince" },
         { "qu'essan", "princess" },
         { "qu'ilinsar", "house insignia" },
         { "qu'madonest", "sailor" },
         { "qu'mados", "sea" },
         { "qu'madosfa", "aquatic" },
         { "qu'madosfan", "ocean" },
         { "qu'rea", "timid" },
         { "qu'reane", "timidly" },
         { "qu'reanurl", "timidness" },
         { "qu'rearn", "timidity" },
         { "qu'za", "abode" },
         { "qu'zenas", "domicile" },
         { "qua'l", "agree" },
         { "qua'lae", "disagree" },
         { "qua'laelay", "arguement" },
         { "qua'laelayen", "arguments" },
         { "qua'laen", "against" },
         { "quae", "demented" },
         { "qualla", "please" },
         { "quanth", "fill" },
         { "quanthe", "fills" },
         { "quanthin", "filling" },
         { "quar'valsharess", "goddess (not Lloth)" },
         { "quarnarin", "turning" },
         { "quarne", "turns" },
         { "quarnus", "turned" },
         { "quarth", "order (authority)" },
         { "quarth", "command" },
         { "quarth", "order" },
         { "quarthas", "duty" },
         { "quarthen", "commanded" },
         { "quarthen", "ordered" },
         { "quarthes", "appropriate" },
         { "quarval-sharess", "goddess" },
         { "queelas", "quickly" },
         { "quellarin", "castle" },
         { "quen", "four" },
         { "quenar", "fourth" },
         { "quengen", "linger" },
         { "quenszith", "forty" },
         { "quenszithael", "forty-eight" },
         { "quenszithal", "forty-nine" },
         { "quenszithar", "fortieth" },
         { "quenszithdra", "forty-two" },
         { "quenszithla", "forty-three" },
         { "quenszithlyn", "forty-seven" },
         { "quenszithraun", "forty-six" },
         { "quenszithuel", "forty-five" },
         { "quenszithuen", "forty-four" },
         { "quenszithus", "forty-one" },
         { "quill", "pen" },
         { "quin", "yet" },
         { "quivved", "guest" },
         { "qunnessaa", "fairy" },
         { "quor'vlosaba", "soulfriend" },
         { "quor'vlosara", "soulmate" },
         { "quorin", "drown" },
         { "quorine", "drowns" },
         { "quorinhin", "drowning" },
         { "quortek", "soul" },
         { "quugh", "mar" },
         { "quughuth", "marred" },
         { "quuk", "blemish" },
         { "qwe", "needle" },
         { "qwe'en", "needles" },
         { "qweoth", "needling" },
         { "r'glin", "hooks" },
         { "r'hul", "avenger" },
         { "r'yder", "wrestle" },
         { "ra'ri", "apocalypse" },
         { "ra'ton", "orphan" },
         { "ra'uren", "calves" },
         { "raeh'li", "fierce" },
         { "ragar", "find" },
         { "ragar", "discover" },
         { "ragar", "uncover" },
         { "ragarrl", "search" },
         { "raghvaril", "villain" },
         { "raghvarilna", "villains" },
         { "ragrubah", "dungeon" },
         { "ragrubahn", "dungeons" },
         { "rah", "hand" },
         { "rah'fol", "handsom" },
         { "rahi", "hands" },
         { "rahkri'sha", "glove" },
         { "rahkri'shuu", "gloves" },
         { "raim'xium", "million" },
         { "rak'nes", "especially" },
         { "raldar", "strip" },
         { "raldarin", "stripping" },
         { "ramak", "sins" },
         { "ramith", "nightmares" },
         { "ramoth", "nightmare" },
         { "ranndek", "mates" },
         { "ranndilin", "mating" },
         { "raq'tar", "arrive" },
         { "raq'tarin", "arriving" },
         { "raq'te", "arrives" },
         { "raq'tus", "arrived" },
         { "rarr", "hearts" },
         { "rasimf", "frustrate" },
         { "rasimfe", "frustrates" },
         { "rasimfein", "frustrating" },
         { "rasimfus", "frustrated" },
         { "rasimfusne", "frustratedly" },
         { "rasimorf", "frustration" },
         { "rath", "back" },
         { "rath'arg", "coward" },
         { "rath'argh", "dishonor" },
         { "rath'argurl", "cowardice" },
         { "rath'elg", "backstab" },
         { "ratha", "backs" },
         { "rathi'dol", "backwards" },
         { "rathrea", "behind" },
         { "raur", "calf" },
         { "ravhel", "hundred" },
         { "ravhelar", "hundredth" },
         { "ravhelen", "hundreds" },
         { "raxoll", "element" },
         { "raza", "slit" },
         { "rduj", "pott" },
         { "re'ew", "screw" },
         { "rei", "fall" },
         { "rei'shann", "archangel" },
         { "rei'yiss", "falling" },
         { "reibe", "wall" },
         { "reiyal", "quite" },
         { "reliq", "savage" },
         { "rena'wen", "basalt" },
         { "rendan", "abyss" },
         { "renor", "black" },
         { "repalninkiz", "pregnant" },
         { "rescho", "easy" },
         { "rescho'ur", "easier" },
         { "rescho'url", "easiest" },
         { "reshka", "returns" },
         { "resk'afar", "hole" },
         { "resk'sultha", "penetrate" },
         { "ressverd", "reserved" },
         { "retlah", "full" },
         { "retlahlgoruth", "full-figured" },
         { "revi'n", "street" },
         { "revis", "road" },
         { "rewt", "fat" },
         { "rezl", "abdomen" },
         { "rezlb", "abdomens" },
         { "rezlet", "denial" },
         { "reztorm", "along" },
         { "rhoque", "scavenger" },
         { "rhoquil", "scavengers" },
         { "rhseev", "receive" },
         { "rifhitah", "uncomfortable" },
         { "riknueth", "bitter" },
         { "ril", "every" },
         { "rilbol", "everything" },
         { "rilkhel", "everybody" },
         { "rilu'oh", "however" },
         { "riluss", "everyone" },
         { "rilvel'klar", "everywhere" },
         { "rin'ov", "ever" },
         { "rinovdro", "immortal" },
         { "rinteith", "throat" },
         { "rith'tar", "sentinel" },
         { "ritze", "value" },
         { "ritzeld", "valuable" },
         { "ritzelden", "valuables" },
         { "ritzemme", "valuing" },
         { "ritzen", "values" },
         { "ritzou", "valued" },
         { "rivol", "gorrilla" },
         { "rivven", "humans" },
         { "rivvil", "human" },
         { "rivvil'ha", "mankind" },
         { "rivvilse", "humans" },
         { "rivvin", "common" },
         { "rnahm", "polite" },
         { "rodjax'r", "dictionary" },
         { "roesor", "sorrow" },
         { "roo'lar", "rouge" },
         { "rosa", "rain" },
         { "rosin", "born" },
         { "rotarkmal", "grilled" },
         { "rotharln", "slaving" },
         { "rothe", "slave" },
         { "rothe", "cattle" },
         { "rothen", "slaves" },
         { "rothrl", "obey" },
         { "rovu", "warg" },
         { "rovul", "wargs" },
         { "roxuc", "practical" },
         { "roxucul", "practically" },
         { "rrakke", "rudeness" },
         { "rraun", "six" },
         { "rraunar", "sixth" },
         { "rraunszith", "sixty" },
         { "rraunszithar", "sixtieth" },
         { "rrhovor", "batter" },
         { "rrhovoruth", "battered" },
         { "rrok", "scavenge" },
         { "rrokai", "scavenging" },
         { "rrorrok", "hound" },
         { "rrorrokmzil", "hounds" },
         { "rrosa", "docks" },
         { "rryatanan", "solving" },
         { "ruebusan", "owing" },
         { "ruebusl", "owes" },
         { "ruebuss", "owe" },
         { "ruebusu", "owed" },
         { "ruinay", "runaway" },
         { "rul'selozan", "disgusting" },
         { "rumunasin", "sorceress" },
         { "run'coii", "mordor" },
         { "runest", "signs" },
         { "russtar", "exhaust" },
         { "russtarin", "exhausting" },
         { "russtarus", "exhausted" },
         { "russte", "exhausts" },
         { "russus", "box" },
         { "ruulk'na", "disturb" },
         { "ruulk'nae", "disturbs" },
         { "ruulk'nain", "disturbing" },
         { "ruulk'naine", "disturbingly" },
         { "ruulk'naus", "disturbed" },
         { "ruzzo'iso", "dumb" },
         { "rytho'le", "deserve" },
         { "s'argt", "guardian" },
         { "s'ck", "sick" },
         { "s'enar", "sort" },
         { "s'gos", "brave" },
         { "s'ilnesa", "snail" },
         { "s'lat'halin", "fighter" },
         { "s'lozan", "slime" },
         { "s'luge", "flay" },
         { "s'lurpp", "fawn" },
         { "s'lurpp", "flatter" },
         { "s'lurppin", "flattering" },
         { "s'lurppuk", "sycophant" },
         { "s'pdon", "spellcaster" },
         { "s'rinelli", "succubus" },
         { "s'ssiron", "sassy" },
         { "s'tharl", "sit" },
         { "s'xor", "suck" },
         { "sa'uren", "shins" },
         { "sadei", "fox" },
         { "sae'etha", "content" },
         { "sae'uth", "pleased" },
         { "sakphen", "halflings" },
         { "sakphul", "halfling" },
         { "salkin", "dancing" },
         { "sanguine", "vampire" },
         { "sanrr", "reason" },
         { "sansiss", "cruel" },
         { "sanwttdd", "smile" },
         { "saph", "like" },
         { "saphun", "liked" },
         { "sapphir'", "sapphire" },
         { "saqos", "dreadful" },
         { "saqqal", "forearm" },
         { "saqqaln", "forearms" },
         { "sar", "pin" },
         { "sara", "wheat" },
         { "sargh", "strength at arms" },
         { "sargh", "valor" },
         { "sargtlin", "drow warrior" },
         { "sargtlin", "warrior" },
         { "sargtlinen", "warriors" },
         { "sariya", "cloud" },
         { "sariyan", "clouds" },
         { "sarn", "beware" },
         { "sarn'", "warning" },
         { "sarn'elgg", "punish" },
         { "sarnor", "swift" },
         { "sarol", "weapon" },
         { "sarol'veldruk", "weaponmaster" },
         { "saroless", "weapons" },
         { "satiir", "feel" },
         { "satiiras", "feels" },
         { "saur", "shin" },
         { "sca'varn", "screaming" },
         { "schak'gg", "reckoned" },
         { "scra'qyo", "scorpions" },
         { "screa", "learn" },
         { "screa'in", "learning" },
         { "screan", "learns" },
         { "screus", "learned" },
         { "sdfghj", "yittk" },
         { "sea'an", "hero" },
         { "sefal", "anyways" },
         { "sei'lor", "set" },
         { "seib'n", "mix" },
         { "seil", "solace" },
         { "seke", "true" },
         { "seke'olath", "shade" },
         { "seke'ssussun", "hue" },
         { "seke'ssussuni", "tint" },
         { "sel", "new" },
         { "sel'tur", "soft" },
         { "sel'turi", "softly" },
         { "selg'tarn", "matter" },
         { "senger", "lord" },
         { "ser", "keep" },
         { "ser'lech", "stupidity" },
         { "serisen", "lovers" },
         { "seriso", "lover" },
         { "serto", "evergreen" },
         { "serto'uuthli", "cyan" },
         { "setabesop", "september" },
         { "sethess", "searched" },
         { "sevir", "leave" },
         { "sha'nalt", "happen" },
         { "sha-rasa", "tremble" },
         { "shabeull", "conquered" },
         { "shadowshunter", "karandras" },
         { "shadrak", "undead slayer" },
         { "shadyr", "genius" },
         { "shadyrae", "genie" },
         { "shaiith", "nameless" },
         { "shalh'hataa", "paris" },
         { "sham'aroth", "unholy" },
         { "shanaal", "chalice" },
         { "shanaal", "goblet" },
         { "shanxxizz", "helpful" },
         { "shanxxizzul", "helpfully" },
         { "shaok", "slug" },
         { "shar", "mind" },
         { "shar'tleg", "turn" },
         { "sharorr", "throne" },
         { "sharulg", "royal seal" },
         { "shay'la", "twinkle" },
         { "shcrten", "stubborn" },
         { "shebali", "rogue" },
         { "shebali", "non-noble drow" },
         { "shee'lot", "drunk" },
         { "shezqa", "stark" },
         { "shinder", "allowed" },
         { "shinduago", "surface" },
         { "shineria", "creep" },
         { "shineriasta", "creeping" },
         { "shinul", "occur" },
         { "shinule", "occurs" },
         { "shinulin", "occurrence" },
         { "shinulinnen", "occurrences" },
         { "shlu'ta", "can" },
         { "shlubnaut", "cannot" },
         { "shosan", "chivalry" },
         { "shosanzik", "chivalrious" },
         { "shu", "shit" },
         { "shu'iblith", "dung" },
         { "shuk", "market" },
         { "shuu", "crap" },
         { "si'iha", "armpit" },
         { "si'ihan", "armpits" },
         { "siasta", "scimitar" },
         { "sieva", "suppose" },
         { "sievazhau", "estimate" },
         { "siffat", "stinking" },
         { "sifsihlan", "nasty" },
         { "sii'a", "current" },
         { "sii'ahne", "currently" },
         { "sik", "whenever" },
         { "sikul", "defense" },
         { "sikulen", "defenses" },
         { "sil'ilos", "purple" },
         { "sil'iluuth", "violet" },
         { "sil'in", "noble" },
         { "sila", "bring" },
         { "sila've", "brought" },
         { "silara", "brings" },
         { "silasisann", "undreamed" },
         { "silinrai", "hunters" },
         { "silinrul", "hunter" },
         { "siltoz", "survivor" },
         { "siltozen", "survivors" },
         { "siltrin", "flesh" },
         { "sin", "inch" },
         { "sirn", "mirror" },
         { "sirnenj", "mirroring" },
         { "sirnet", "mirrored" },
         { "sirnnh", "mirrors" },
         { "sithe'", "lots" },
         { "sithyrr", "crossbow" },
         { "siyo", "yes" },
         { "sjaad'ur", "excuse" },
         { "skal'as", "lacks" },
         { "skikudis", "steel" },
         { "skragharigg", "unicorn" },
         { "slaggiss", "slayers" },
         { "slud'ou", "chained" },
         { "sluda", "chain" },
         { "sluden", "chains" },
         { "slyan", "star" },
         { "slyannen", "stars" },
         { "sn'dar'adii", "bloodwrath" },
         { "snizilizil", "snizilizil" },
         { "softe", "beer" },
         { "soh'rad", "respond" },
         { "sokoya", "pretty" },
         { "sol", "eye" },
         { "solen", "eyes" },
         { "spill", "poure" },
         { "spo'taa", "hill" },
         { "sreen", "danger" },
         { "sreen'aur", "safe" },
         { "sreen'aur", "safety" },
         { "sreenath", "dangerous" },
         { "srif", "eyelash" },
         { "srifey", "eyelashes" },
         { "sril've", "shimmering" },
         { "srow", "scum" },
         { "ssa", "serpent" },
         { "sseren", "hot" },
         { "ssiggrin", "thought" },
         { "ssiggrins", "thoughts" },
         { "ssil'ernuhn", "sarcastic" },
         { "ssin", "beauty" },
         { "ssin'urn", "beautiful" },
         { "ssin'urn", "sexy" },
         { "ssin'urne", "beautifully" },
         { "ssin'urninness", "seducer" },
         { "ssinaeth", "addiction" },
         { "ssindossa", "whore" },
         { "ssinjin", "sweet" },
         { "ssins d'aerth", "professional entertainer (prostitute)" },
         { "ssins d'aerth", "prostitute" },
         { "ssinss", "seduction" },
         { "ssinss", "charm" },
         { "ssinss", "allure" },
         { "ssinss'rhul", "nude" },
         { "ssinssd'aerth", "professional entertainers (prostitutes)" },
         { "ssinssd'aerth", "prostitute" },
         { "ssinssrickla", "giggles" },
         { "ssinssrigg", "passion" },
         { "ssinssrigg", "lust" },
         { "ssinssrigg", "love" },
         { "ssinssriggin", "lusting" },
         { "ssinssriggin", "loving" },
         { "ssinssrigin", "loves" },
         { "ssinssrin", "want" },
         { "ssinssrin", "desire" },
         { "ssinssrine", "desires" },
         { "ssinssrinil", "wanted" },
         { "ssinsuurul", "music" },
         { "ssintchal", "pastry" },
         { "ssish", "smelt" },
         { "ssissill", "violent" },
         { "ssissilluk", "violence" },
         { "ssivah", "voice" },
         { "sslazt", "sleek" },
         { "sslig'ne", "defend" },
         { "sslig'ne", "protect" },
         { "sslu", "collar" },
         { "ssluilling", "collarbone" },
         { "ssol", "echo" },
         { "ssol'riss", "echoed" },
         { "ssolsse", "echoes" },
         { "ssolssohl", "echoing" },
         { "ssouk", "predator" },
         { "ssouki", "predators" },
         { "ssran", "worth" },
         { "ssrethalas", "streets" },
         { "ssrig'luin", "need" },
         { "ssrigg'tul", "pleasure" },
         { "ssriggah", "passionate" },
         { "sssiks", "sun" },
         { "sssiksvig", "sunworm" },
         { "ssuil", "incite" },
         { "ssuile", "incites" },
         { "ssuilin", "inciting" },
         { "ssuilus", "incited" },
         { "ssuorr", "delicious" },
         { "ssussun", "light" },
         { "ssussun", "bright" },
         { "ssussun'eb", "lightless" },
         { "ssussun'yukin", "yellow" },
         { "ssussunel", "lightly" },
         { "ssussunur", "brighter" },
         { "ssussunurl", "brightest" },
         { "ssuth", "escape" },
         { "ssuu'hha", "breath" },
         { "ssuu'hhan", "breaths" },
         { "ssuu'hhane", "breathily" },
         { "ssuule", "breathes" },
         { "ssuulin", "breathing" },
         { "ssuulus", "breathed" },
         { "stallon", "sigh" },
         { "stath", "few" },
         { "statha", "story" },
         { "ste'kol", "toy" },
         { "ste'kolen", "toys" },
         { "stre", "afraid" },
         { "streea", "suicide" },
         { "streea", "death" },
         { "streeadus", "suicidal" },
         { "streeatul", "arena" },
         { "streeatulgh", "arenas" },
         { "streeka", "reckless" },
         { "streeka", "recklessness" },
         { "stu'eriod", "lamer" },
         { "stylad", "pulse" },
         { "su'aco", "wind" },
         { "sui'aerl", "arcane" },
         { "suingmc", "silly" },
         { "suldaim", "shame" },
         { "suliss", "grace" },
         { "suliss'urn", "graceful" },
         { "sultha", "enter" },
         { "sundu", "sing" },
         { "sunduiri", "singer" },
         { "sunduirien", "singers" },
         { "sune", "warm" },
         { "sune'le", "warming" },
         { "suri", "birds" },
         { "suru", "bird" },
         { "sussu'ri", "sunrise" },
         { "sut", "release" },
         { "sut'rinos", "commander" },
         { "sute", "releases" },
         { "sutin", "releasing" },
         { "sutus", "released" },
         { "suul", "side" },
         { "suul'et'jabar", "pompous" },
         { "suulen", "siding" },
         { "suur", "palm" },
         { "suuril", "palms" },
         { "suust", "quiet" },
         { "suut", "knot" },
         { "suut'oth", "knotted" },
         { "suutsus", "knotting" },
         { "suuz'chok", "tradition" },
         { "suvok", "listener" },
         { "suvokai", "listeners" },
         { "suxxizz", "helping" },
         { "svirfnebli", "deep gnome" },
         { "svirfneblin", "deep gnomes" },
         { "swariy", "swear" },
         { "sweer", "excellent" },
         { "swift", "heales" },
         { "szeous", "secret" },
         { "szeoussen", "secrets" },
         { "szi'tangi", "tendaysweek" },
         { "szith", "ten" },
         { "szithael", "eighteen" },
         { "szithaelar", "eighteenth" },
         { "szithal", "nineteen" },
         { "szithalar", "nineteenth" },
         { "szithar", "tenth" },
         { "szithdra", "twelve" },
         { "szithdrar", "twelfth" },
         { "szithdrasv", "decade" },
         { "szithla", "thirteen" },
         { "szithlar", "thirteenth" },
         { "szithlyn", "seventeen" },
         { "szithlynar", "seventeenth" },
         { "szithraun", "sixteen" },
         { "szithraunar", "sixteenth" },
         { "szithrel", "thousand" },
         { "szithuel", "fifteen" },
         { "szithuelar", "fifteenth" },
         { "szithuen", "fourteen" },
         { "szithuenar", "fourteenth" },
         { "szithus", "eleven" },
         { "szithusar", "eleventh" },
         { "szuk", "tread" },
         { "t'larryo", "broken" },
         { "t'puuli", "silver" },
         { "t'puuli", "white" },
         { "t'rilamsha", "ownship" },
         { "t'yin", "then" },
         { "t'zaraw", "deal" },
         { "t'zarreth", "ass" },
         { "ta'ecelle", "sunlight" },
         { "taeborss", "beating" },
         { "taga", "than" },
         { "tagnik'zun", "dragons" },
         { "tagnik'zur", "dragon" },
         { "tagnik'zur'zurdo", "draconian" },
         { "tah", "hunt" },
         { "tah'entil", "hunting" },
         { "tahcaluss", "hunger" },
         { "tahh", "chase" },
         { "tahta", "scared" },
         { "tai'luen", "spare" },
         { "tajo", "vision" },
         { "takar", "dry" },
         { "takata", "begone" },
         { "taknea", "thankyou" },
         { "takrin", "desert" },
         { "takrome", "shot" },
         { "tal'skala", "ignore" },
         { "tala", "pink" },
         { "talar", "bonded" },
         { "talhalrustir", "ambassedor" },
         { "talinth", "to think" },
         { "talinth", "think" },
         { "talinth", "consider" },
         { "talinth", "to consider" },
         { "talintha", "thinking" },
         { "talinthe", "thinks" },
         { "talinthin", "considering" },
         { "talinthus", "considered" },
         { "talthalra", "meeting" },
         { "talwien", "feet" },
         { "talya", "huntress" },
         { "tangi", "day" },
         { "tangin", "days" },
         { "tangis'", "even" },
         { "tangisto", "evening" },
         { "tanook", "quit" },
         { "tanth", "worm" },
         { "tap'as", "pasta" },
         { "tar'annen", "company" },
         { "tara'handuir", "smiling" },
         { "tarath", "swirling" },
         { "tarthe", "away" },
         { "tau", "thee" },
         { "taudl", "sorry" },
         { "taur", "forest" },
         { "taurea", "forested" },
         { "tauren", "forests" },
         { "taurina", "wooden" },
         { "tayaere", "remorse" },
         { "tchaka", "chocolate" },
         { "tchal", "bread" },
         { "tchallen", "breads" },
         { "te-smur", "prosper" },
         { "tebaun", "bladedancer" },
         { "tejmook'cinkjuu", "feeling" },
         { "tekartas", "society" },
         { "teknil", "greed" },
         { "tela", "tip" },
         { "teladinea", "drunken" },
         { "telan", "tips" },
         { "telanth", "say" },
         { "telanth", "speak" },
         { "telanth", "talk" },
         { "telanthaad", "speaking" },
         { "telanthus", "said" },
         { "telassin", "persuade" },
         { "telassine", "persuades" },
         { "telassinin", "persuading" },
         { "telassinus", "persuaded" },
         { "telloprt'kitherin", "albino" },
         { "tenu", "alright" },
         { "terini'nestg", "interesting" },
         { "tessai", "blossom" },
         { "tesso", "tell" },
         { "tessoe", "tells" },
         { "tet", "lovely" },
         { "th'auix", "bounce" },
         { "tha", "met" },
         { "tha'tsit", "possible" },
         { "thac'zil", "land" },
         { "thac'zilen", "lands" },
         { "thain", "untrustworthy" },
         { "thal", "nine" },
         { "thalack", "war" },
         { "thalack", "open fighting" },
         { "thalack'vel", "ambush" },
         { "thalackz'hind", "raid" },
         { "thalackz'ondus", "raided" },
         { "thalar", "ninth" },
         { "thalia", "sssssssss" },
         { "thalra", "meet" },
         { "thalra", "encounter" },
         { "thalszith", "ninety" },
         { "thalszithar", "ninetieth" },
         { "thaylie", "beutifulsisland" },
         { "theral", "copper" },
         { "thir'ku", "change" },
         { "thrityh", "attack" },
         { "thry", "cause" },
         { "thryar", "toscause" },
         { "thryarus", "caused" },
         { "thrye", "causes" },
         { "thuulstrea", "anticipation of impending death" },
         { "thyrais", "empath" },
         { "ti'bux", "servent" },
         { "tialej", "drinking" },
         { "ticondo", "tired" },
         { "tigar'zimut", "domination" },
         { "tighrabalt", "mourning" },
         { "tiimar'l", "simitars" },
         { "timpe", "drizzle" },
         { "tir", "off" },
         { "tirloc", "stalagmite" },
         { "tiu", "fly" },
         { "tiu'xa", "flight" },
         { "tiuin'iona", "dust" },
         { "tix", "actual" },
         { "tizzin", "lizard" },
         { "tk'parhn", "broadsword" },
         { "tlakh", "applause" },
         { "tlakheese", "applauding" },
         { "tlakhev", "applauded" },
         { "tlakhir", "applaud" },
         { "tlakhis", "applauds" },
         { "tlu", "be" },
         { "tlu", "to be" },
         { "tlu'og", "lend" },
         { "tluin", "being" },
         { "tlun", "am" },
         { "tlus", "been" },
         { "tlusher", "begun" },
         { "tnoah", "doesnt" },
         { "to'ryll", "emerald" },
         { "to'ryll", "green" },
         { "to'zoe", "toad" },
         { "toha", "same" },
         { "tois", "died" },
         { "tol'galen", "emeral isle" },
         { "tomanita'sorne", "hellfire" },
         { "tona'", "dirt" },
         { "tonaik", "dirty" },
         { "tonashss", "rock" },
         { "toofar", "reside" },
         { "toofare", "resides" },
         { "toofarin", "residing" },
         { "toofarus", "resided" },
         { "toofin", "resident" },
         { "toofinen", "residents" },
         { "tosdki", "other" },
         { "toun", "impress" },
         { "tounle", "impressive" },
         { "tr'inno", "sucker" },
         { "tra", "abbreviate" },
         { "transilit", "translate" },
         { "trantz", "real" },
         { "treem", "fearing" },
         { "treemma", "fear" },
         { "treemmafol", "fearsome" },
         { "treemmai", "fears" },
         { "treemmou", "feared" },
         { "treiw", "pale" },
         { "treiw'waessuth", "pale-skinned" },
         { "trek'si", "dollars" },
         { "trelao", "divine" },
         { "tresk'ri", "world" },
         { "trez'in", "northwest" },
         { "trez'nt", "northeast" },
         { "trezen", "north" },
         { "tril", "bite" },
         { "tril'vol", "images" },
         { "trilaren", "bitten" },
         { "trinn'tele", "alcohol" },
         { "trisil", "agent" },
         { "tropalkia", "steak" },
         { "tsak", "tainted" },
         { "tsoss", "kiss" },
         { "tsyddus", "thinner" },
         { "tsyde", "thin" },
         { "tu'", "both" },
         { "tu'fyr", "between" },
         { "tu'jol", "beyond" },
         { "tu'rilthiin", "half-elves" },
         { "tu'rilthiir", "half-elf" },
         { "tu'suul", "beside" },
         { "tuain't", "poisoned" },
         { "tualcss", "cutlass" },
         { "tud", "flew" },
         { "tuin", "spun" },
         { "tul'gen", "lumber" },
         { "tullus", "hall" },
         { "tullusas", "halls" },
         { "tulshar", "empire" },
         { "tundro", "deathstalker" },
         { "tupora", "living" },
         { "turi'ko", "threat" },
         { "turi'konda", "threatened" },
         { "turu'mi", "justify" },
         { "tuth", "both" },
         { "tuvok", "listening" },
         { "tyav", "taste" },
         { "tyln", "spinning" },
         { "tyn", "spin" },
         { "tyne", "spinner" },
         { "tynil", "spinners" },
         { "tyrnae", "quench" },
         { "tyuilf", "taunt" },
         { "u'fran", "moldy" },
         { "u'ilk", "virgin" },
         { "u'lar", "impossible" },
         { "u'nef", "fortune" },
         { "u'thet", "recognize" },
         { "ualstin", "approve" },
         { "uchado", "here" },
         { "ud'phuul", "we%27re" },
         { "udos", "we" },
         { "udossa", "us" },
         { "udosst", "ours" },
         { "udossta", "our" },
         { "udosstan", "ourselves" },
         { "ughasha", "intrigue" },
         { "ughashag", "intriguing" },
         { "ugul", "low" },
         { "ugulove", "lowers" },
         { "ugulovin", "lowering" },
         { "ugulovus", "lowered" },
         { "uhew", "pot" },
         { "uih", "muscle" },
         { "uil", "am" },
         { "uil'mriz", "trophy" },
         { "ujool", "glass" },
         { "uk", "he" },
         { "ukt", "his" },
         { "ukta", "him" },
         { "uktan", "himself" },
         { "ul'din", "raymus" },
         { "ul'hyal", "inspiration" },
         { "ul'hyrr", "idea" },
         { "ul'kas", "called" },
         { "ul'mubar", "moving" },
         { "ul'naus", "together" },
         { "ul'nusst", "scream" },
         { "ul'nusst", "cry" },
         { "ul'plynnda", "to mistake" },
         { "ul'plynndae", "mistakes" },
         { "ul'plynndain", "mistaking" },
         { "ul'plynndane", "mistakenly" },
         { "ul'plyr", "reach" },
         { "ul'saruk", "warlord" },
         { "ul'trin", "sound" },
         { "ul'tro", "sounds" },
         { "ul-ilindithu", "destiny" },
         { "ula", "fine" },
         { "ulan", "toward" },
         { "ulath'elzaren", "arch-mage" },
         { "ulathtallar", "arch-priestess" },
         { "ulin", "future (time)" },
         { "ulin", "future" },
         { "ulisha", "fearie" },
         { "uljabplynn", "err" },
         { "uljabplynn", "mistaken" },
         { "uljabplynne", "errs" },
         { "uljabplynnus", "erred" },
         { "ulkund", "flood" },
         { "ulkunde", "floods" },
         { "ulkundin", "flooding" },
         { "uln'hyrr", "liar" },
         { "uln'hyrr", "lair" },
         { "ulnar", "lie" },
         { "ulnar", "untruth" },
         { "ulnarii", "lying" },
         { "ulnen", "lies" },
         { "ulnin", "soon" },
         { "ulnuth", "lied" },
         { "ulorith", "exist" },
         { "ultrin", "supreme" },
         { "ultrin", "transcendent" },
         { "ultrine", "mighty" },
         { "ultrinnan", "conquering" },
         { "ultrinnan", "victory" },
         { "ulu", "to" },
         { "ulu'fashrit", "pressing" },
         { "unboi", "step" },
         { "ungue", "hollow" },
         { "unl'r", "ends" },
         { "unnslith", "lately" },
         { "uns'aa", "me" },
         { "unsilanon", "catastrophe" },
         { "untaramar", "ultimate" },
         { "uoi'nota", "hell" },
         { "upnsdeysassay", "upsthesass" },
         { "ur'ac", "foot" },
         { "ur'thal", "rider" },
         { "ur'thalrss", "riders" },
         { "uradinna'lun", "translators" },
         { "uri'shoelt", "happened" },
         { "uriu", "has" },
         { "urlk'ris", "govern" },
         { "us'jalil", "m'lady" },
         { "usar", "sliver" },
         { "usbyr", "either" },
         { "ush'akal", "sir" },
         { "ushdui", "crazy" },
         { "ushkaneisel", "weasel" },
         { "usi'entil", "guessing" },
         { "usieva", "guess" },
         { "usievaah", "pretend" },
         { "uspir", "spiral" },
         { "uss", "one" },
         { "uss'sol", "cyclops" },
         { "ussa", "me" },
         { "ussgolhyrr", "trickster" },
         { "ussgyot", "position" },
         { "usst", "mine" },
         { "usst'na", "brimestone" },
         { "ussta", "my" },
         { "usstan", "i" },
         { "usstan", "this one" },
         { "usstan", "myself" },
         { "usstan'bal", "i've" },
         { "usstan'luf", "oneselff" },
         { "usstan'sargh", "arrogant" },
         { "usstan'sargho", "arrogance" },
         { "usstil", "one in my place" },
         { "usstzigh", "imagination" },
         { "ussvelusscal", "acid" },
         { "ust", "first" },
         { "ust'dan", "infidel" },
         { "usto", "freak" },
         { "usulii", "member" },
         { "usuud", "offal" },
         { "ut'siltis", "toscollars" },
         { "ut'siltis", "stosenslave" },
         { "uuthli", "saphire" },
         { "uuthli", "blue" },
         { "uxxahuu", "dispose" },
         { "uyl'udith", "greatest" },
         { "v'dre", "rest" },
         { "v'dri", "sleep" },
         { "v'drin", "sleeping" },
         { "v'driy", "sleepy" },
         { "v'nal", "vale" },
         { "v'ren", "lick" },
         { "vaen", "last" },
         { "valbyl", "bind" },
         { "valbylis", "binding" },
         { "valbynae", "bindings" },
         { "valdruk", "shadowmaster" },
         { "valkhel", "liche" },
         { "vallabha", "dearest" },
         { "valm", "join" },
         { "valm", "bond" },
         { "valmuth", "bonds" },
         { "valshar", "emporer" },
         { "valsharen", "imperial" },
         { "valsharess", "queen" },
         { "valshath", "palace" },
         { "valuk", "king" },
         { "valukes", "kings" },
         { "valyr", "manner" },
         { "valyrin", "kind" },
         { "valyrine", "kindly" },
         { "vaq'", "consume" },
         { "vaq'e", "consumes" },
         { "vaq'in", "consuming" },
         { "vassnten", "innocents" },
         { "vassnti", "innocent" },
         { "vasta", "dump" },
         { "vdrei", "rests" },
         { "vec'end", "odd" },
         { "vedaust", "goodbye" },
         { "vedir", "mount" },
         { "vedui'", "greetings" },
         { "vee", "bid" },
         { "vee'ther", "weather" },
         { "veein", "bidding" },
         { "veen", "bids" },
         { "veeus", "bade" },
         { "veir", "close" },
         { "veire", "closes" },
         { "veirin", "closing" },
         { "veirs", "closer" },
         { "veirsa", "closest" },
         { "veirus", "closed" },
         { "vel'bol", "what" },
         { "vel'bolen", "which" },
         { "vel'dos", "whose" },
         { "vel'drav", "when" },
         { "vel'jamren", "rulers" },
         { "vel'karel", "nomination" },
         { "vel'klar", "where" },
         { "vel'shamress", "administrative" },
         { "vel'uss", "who" },
         { "vel'ussa", "whom" },
         { "vel'xundussa", "security" },
         { "vel'xunil", "spying" },
         { "vel'xunyrr", "spy" },
         { "vel'xunyrr", "to spy" },
         { "veldri", "hide" },
         { "veldrin", "shadows" },
         { "veldrin", "concealment" },
         { "veldrinn", "hides" },
         { "veldriss", "shadow mistress" },
         { "veldruk", "master" },
         { "veldrukev", "mastered" },
         { "velendev", "longer" },
         { "velg'larn", "assassin" },
         { "velg'larns", "assassins" },
         { "velkresa", "hiding" },
         { "velkyn", "unseen" },
         { "velkyn", "hidden" },
         { "vellupa", "marry" },
         { "vellupala", "married" },
         { "velnarin", "instrument" },
         { "velve", "blade" },
         { "velvel", "knife" },
         { "velveli", "knives" },
         { "velven", "blades" },
         { "velvorn", "bladed" },
         { "velxunai", "spies" },
         { "vendui", "hello" },
         { "vendui'", "greetings" },
         { "venoch", "murder" },
         { "venorik", "silent" },
         { "venorsh", "silence" },
         { "ventash'ma", "leader" },
         { "ver'n", "serious" },
         { "ver'nuin", "pantywaste" },
         { "veresi", "moonless" },
         { "verin", "evil" },
         { "verve", "long" },
         { "ves", "very" },
         { "ves'aph", "likely" },
         { "vesdrac", "souls" },
         { "vesss", "smooth" },
         { "vet", "veil" },
         { "vhaid", "proud" },
         { "vharc", "vengeance" },
         { "vharcan", "revenge" },
         { "vharr", "ward" },
         { "vharren", "wards" },
         { "vharrin", "warding" },
         { "vharrus", "warded" },
         { "vhid", "chaos" },
         { "vholk", "iron" },
         { "vi'", "vice" },
         { "vi'noriam", "viscount" },
         { "videnn", "gate" },
         { "vigaj", "madness" },
         { "viggtu", "insane" },
         { "viggtuii", "insanity" },
         { "vigh", "mad" },
         { "vikkhn", "pragmatic" },
         { "vikkhnoq", "pragmatically" },
         { "vil'klviss", "worms" },
         { "vil'zra'quath", "freeze" },
         { "vilrath", "malice" },
         { "vin'ult", "hail" },
         { "vir'ednith", "explosion" },
         { "vishin", "upsets" },
         { "vith", "sexual-intercourse" },
         { "vith", "sex" },
         { "vith", "sex-genders" },
         { "vith", "fuck" },
         { "vith'ir", "fucksyou" },
         { "vith'rell", "fucker" },
         { "vithanaik", "sexuality" },
         { "vithanna", "sexual" },
         { "vizh'tho", "brandon" },
         { "vla'rinnyn", "brotherhood" },
         { "vlo'jihnrae", "bloodwine" },
         { "vlos", "blood" },
         { "voen'llyl", "heed" },
         { "voiry", "book" },
         { "vok", "listen" },
         { "vost", "cease" },
         { "vostul", "ceasing" },
         { "vreg'nth", "wicked" },
         { "vress'lve", "claw" },
         { "vress'lvel", "talon" },
         { "vress'ol", "finger" },
         { "vrin'klatu", "essence" },
         { "vrine'winith", "stop" },
         { "vrueh", "studying" },
         { "vrune", "shoulder" },
         { "vrunen", "shoulders" },
         { "vyk'zlade", "snake" },
         { "vynnessia", "butterfly" },
         { "vzahaz", "troops" },
         { "w'kar", "godly" },
         { "wa'q", "tower" },
         { "wa'qin", "towering" },
         { "wa'tani", "argue" },
         { "waeklyth", "entangle" },
         { "wael", "fool" },
         { "waela", "foolish" },
         { "waelarin", "fooling" },
         { "waele", "stupid" },
         { "waelen", "idiotic" },
         { "waeles", "idiot" },
         { "waelin", "young" },
         { "waenre", "servants" },
         { "waer'honglath", "justice" },
         { "waerr'ess", "deceipt" },
         { "waess", "skin" },
         { "wahven", "waste" },
         { "wahven'urn", "wasteful" },
         { "wanre", "apprentice" },
         { "wanre", "servant" },
         { "wargerr", "seas" },
         { "wayc'e", "dwell" },
         { "wayc'el", "dwelled" },
         { "wayc'er", "dwells" },
         { "we'ha", "penis" },
         { "weafl", "useful" },
         { "wenress", "maiden" },
         { "wenressen", "maidens" },
         { "wenressne", "maidenly" },
         { "wern'in", "southwest" },
         { "wern'nt", "southeast" },
         { "werneth", "south" },
         { "weth", "each" },
         { "whol", "for" },
         { "wiles", "words" },
         { "wiu", "boy" },
         { "wlalth", "spell" },
         { "wlalths", "spells" },
         { "wobadikist", "steamy" },
         { "wolkil", "baseball" },
         { "wor'ge", "valley" },
         { "wu'suul", "inside" },
         { "wun", "in" },
         { "wund", "among" },
         { "wund", "within" },
         { "wund", "into" },
         { "wund'akh", "belong" },
         { "wussrun'wa", "dream" },
         { "wyirv", "speculate" },
         { "wyirvaerth", "speculation" },
         { "wylth", "thorn" },
         { "wylthen", "thorns" },
         { "wynthau", "succeeded" },
         { "wynthe", "success" },
         { "wynthl", "succeeding" },
         { "wynthye", "succeed" },
         { "wynthyil", "succeeds" },
         { "x'khat", "became" },
         { "xa", "yeah" },
         { "xa'huu", "garbage" },
         { "xa'huul", "litter" },
         { "xa'huuli", "worthless" },
         { "xa'huuli", "trash" },
         { "xa'los", "oracle" },
         { "xal", "might" },
         { "xal", "perhaps" },
         { "xal", "may" },
         { "xan'ss", "word" },
         { "xanalress", "language" },
         { "xar'zith", "ice" },
         { "xarrmak", "clique" },
         { "xas", "yes" },
         { "xellased", "dwellers" },
         { "xenca'", "frog guts" },
         { "xentra", "carribean" },
         { "xenu", "sicken" },
         { "xesst", "entice" },
         { "xet", "discord" },
         { "xey'los", "vein" },
         { "xhandal", "challenge" },
         { "xi'hum", "pets" },
         { "xi'hum", "splaythings" },
         { "xia", "radiant" },
         { "xinanath", "parents" },
         { "xindarl", "drug" },
         { "xindos", "nevertheless" },
         { "xl'fye", "harness" },
         { "xo'a", "attempt" },
         { "xo'al", "try" },
         { "xo'an", "attempts" },
         { "xoch'ark", "scintillate" },
         { "xonathull", "battle" },
         { "xonathulls", "battles" },
         { "xor", "or" },
         { "xortin", "raised" },
         { "xsa", "damn" },
         { "xsa'ol", "damnit" },
         { "xta'rl", "touch" },
         { "xuat", "don't" },
         { "xuil", "with" },
         { "xuileb", "without" },
         { "xukuth", "heart" },
         { "xukuthe'l", "heartless" },
         { "xun", "do" },
         { "xun", "to complete" },
         { "xun", "complete" },
         { "xun", "accomplish" },
         { "xund", "striving" },
         { "xund", "effort" },
         { "xund", "work" },
         { "xundus", "achievement" },
         { "xundus", "accomplishment" },
         { "xundussa", "achievements" },
         { "xundussa", "accomplishments" },
         { "xunin", "doing" },
         { "xunor", "done" },
         { "xunsin", "drive" },
         { "xunus", "did" },
         { "xurtop", "february" },
         { "xusst", "wrong" },
         { "xussten", "wrongs" },
         { "xuz", "end" },
         { "xxilfet", "armour" },
         { "xxizz", "help" },
         { "xxizz'a", "helped" },
         { "xxizzia", "helps" },
         { "xxizzuss", "helper" },
         { "y'sik", "rather" },
         { "y'teni", "raise" },
         { "y'uotl", "amateur" },
         { "ya'quin", "morph" },
         { "yacal", "eating" },
         { "yah", "god" },
         { "yaith", "pay" },
         { "yaithess", "payment" },
         { "yallt", "today" },
         { "yath", "of the temple" },
         { "yath'abban", "temple agent" },
         { "yathrin", "priestess of Lloth" },
         { "yathrin", "priestess" },
         { "yathtallar", "high priestess of Lloth" },
         { "ycal", "eats" },
         { "yentil", "lot" },
         { "yeunn", "pathetic" },
         { "yib", "toe" },
         { "yiben", "toes" },
         { "yibin", "weak" },
         { "yibinss", "weakness" },
         { "yibinssen", "weaknesses" },
         { "yigsawla", "fornication" },
         { "yikss", "decide" },
         { "yin'xir'ziji", "frozen" },
         { "yingil", "gnome" },
         { "yingilin", "gnomes" },
         { "yinn", "tooth" },
         { "yinnin", "teeth" },
         { "yinvezz", "cute" },
         { "yivera", "attract" },
         { "yiverae", "attracts" },
         { "yiverain", "attracting" },
         { "yiveraus", "attracted" },
         { "ykihk", "stinky" },
         { "yllssigul", "caress" },
         { "yllssigul", "stroke" },
         { "yllui'th", "agreed" },
         { "yochlol", "handmaiden of Lloth" },
         { "yol", "since" },
         { "yorlii", "fog" },
         { "yorlii'uak", "foggy" },
         { "yorn", "power" },
         { "yorn", "will" },
         { "yorn", "servant" },
         { "yu'na", "bend" },
         { "yugho", "nice" },
         { "yuih", "muscles" },
         { "yukin", "orange" },
         { "yukin'ik", "beige" },
         { "yul", "gender" },
         { "yulul", "prize" },
         { "yulun", "prizes" },
         { "yulurln", "prizing" },
         { "yuluwyl", "prized" },
         { "yur'i", "changes" },
         { "yutreess", "intelligent" },
         { "yutri", "smart" },
         { "yutrilanil", "intelligence" },
         { "yutrquns", "march" },
         { "yutsu", "return" },
         { "yutsu'", "returned" },
         { "yvalm", "bound" },
         { "ywrazz", "fact" },
         { "z'arlathil", "natural" },
         { "z'arlathilu", "naturally" },
         { "z'haan", "run" },
         { "z'haanin", "running" },
         { "z'har", "to ride" },
         { "z'har", "ride" },
         { "z'hin", "to walk" },
         { "z'hin", "walk" },
         { "z'hind", "journey" },
         { "z'hind", "expedition" },
         { "z'hins", "walks" },
         { "z'hinus", "walked" },
         { "z'hren", "flow" },
         { "z'klaen", "must" },
         { "z'lonzic", "enough" },
         { "z'orr", "climb" },
         { "z'orr", "to climb" },
         { "z'reninth", "believe" },
         { "z'reninthin", "believing" },
         { "z'ress", "power" },
         { "z'ress", "strength" },
         { "z'talin", "esp" },
         { "z'talin", "telepathy" },
         { "zah'har", "suffer" },
         { "zahanzai", "scouts" },
         { "zahanzon", "scout" },
         { "zahur", "courier" },
         { "zakath", "agony" },
         { "zanjur", "messenger" },
         { "zao", "teach" },
         { "zaphodiop", "horse" },
         { "zaphos", "sausage" },
         { "zaqh", "spike" },
         { "zarachi'i", "phrase" },
         { "zasimartek", "authority" },
         { "zathul", "jinx" },
         { "zatoasten", "bastards" },
         { "zav", "clever" },
         { "zayfa", "wielded" },
         { "ze'zhuanth", "ancient" },
         { "zeklet'tau", "heaven" },
         { "zenphe", "chamber" },
         { "zenpheir", "chambers" },
         { "zer'tath", "crystal" },
         { "zet", "left" },
         { "zexen'uma", "stay" },
         { "zezz", "razor" },
         { "zha'linth", "memory" },
         { "zha'trass", "boot" },
         { "zhadur", "belief" },
         { "zhaduren", "beliefs" },
         { "zhah", "is" },
         { "zhahen", "were" },
         { "zhahn", "then" },
         { "zhahn", "past (time)" },
         { "zhahus", "was" },
         { "zhal", "shall" },
         { "zhal'essin", "avatar" },
         { "zhal'essuk", "incarnation" },
         { "zhal'la", "should" },
         { "zhalongfranam", "heating" },
         { "zhas", "wild" },
         { "zhaun", "know" },
         { "zhaun", "knows" },
         { "zhaun'ol", "book" },
         { "zhaun'ol", "treatise" },
         { "zhaun'velkyn", "wisp" },
         { "zhaunau", "knew" },
         { "zhaunil", "wisdom" },
         { "zhaunil", "knowledge" },
         { "zhaunl", "knowing" },
         { "zhaunus", "sure" },
         { "zhaunyl", "surely" },
         { "zhennu", "great" },
         { "zhil'za", "probably" },
         { "zhlaass", "relax" },
         { "zho'aminth", "forgotten" },
         { "zhu'lect", "followed" },
         { "zhuanth", "old" },
         { "zhuel", "former" },
         { "zhuelne", "formerly" },
         { "zif", "absolute" },
         { "zifreinn", "absolutely" },
         { "zig", "base" },
         { "zigh", "image" },
         { "zigh'hlah", "imagery" },
         { "zighen", "imagine" },
         { "zik", "sharp" },
         { "zik'den'vever", "plane" },
         { "zikegh", "sharpest" },
         { "zikeir", "sharper" },
         { "zikowt", "tribe" },
         { "zil", "consort" },
         { "zilib", "consorting" },
         { "zin'olhyrr", "loyal" },
         { "zin'olhyrran", "loyalty" },
         { "zirh", "fed" },
         { "zirn", "feed" },
         { "zirnra", "feeding" },
         { "zirnran", "feedings" },
         { "zith", "page" },
         { "zithrelar", "thousandth" },
         { "zjut", "elbow" },
         { "zjutae", "elbows" },
         { "zjuttenur", "elbowed" },
         { "zniee", "sincere" },
         { "zolarix", "painful" },
         { "zotreth", "strike" },
         { "zotreth", "punch" },
         { "zotreth", "hit" },
         { "zotreth'mal", "striking" },
         { "zoxidon", "system" },
         { "zra'ha", "mentor" },
         { "zsatelis", "newbies" },
         { "zu'tour", "shut" },
         { "zuch", "always" },
         { "zud'dar", "test" },
         { "zuel", "track" },
         { "zuelolb", "tracked" },
         { "zuelsai", "trackers" },
         { "zuelsul", "tracker" },
         { "zuelyi", "tracking" },
         { "zuess", "harrow" },
         { "zuesswy", "harrowing" },
         { "zuneerrh", "salvation" },
         { "zuul'raght", "amazing" },
         { "zygul", "sharp edged" },
         { "zze'ill", "werewolf" },
         { "zzikhc", "smells" },
         { "zzil", "fur" },
         { "", "" }
      };

      char transword[MAX_INPUT_LENGTH],
      transtype[MAX_INPUT_LENGTH],
      buf[MAX_STRING_LENGTH];

      int dictpos = 0,
      itranstype,
      found,
      firstarg = 1;

      if (ch->race != grn_dark_elf && !IS_IMMORTAL(ch))
      {
         send_to_char("Only the drow may use this compendium.\n\r", ch);
         return;
      }

      if (argument[0] == '\0')
      {
         send_to_char("translate compendium [to|from] <phrase>\n\r", ch);
         return;
      }

      argument = one_argument( argument, transtype );
      if (!(!strcmp(transtype, "to") || !strcmp(transtype, "from")))
      {
         send_to_char("You may translate 'to' or 'from' Drow.\n\r", ch);
         return;
      }

      if (!strcmp(transtype, "from"))
      itranstype = 0;
      else
      itranstype = 1;

      if (argument[0] == '\0')
      {
         send_to_char("Translate what?\n\r", ch);
         return;
      }

      sprintf(buf, "Translation: ");
      while (argument[0] != '\0') {
         argument = one_argument( argument, transword );
         dictpos = 0;
         found = 0;
         if (firstarg)
         firstarg = 0;
         else
         strcat(buf, "...");
         while (strcmp(dictionary[dictpos][0], ""))
         {
            if (!strcmp(dictionary[dictpos][itranstype], transword))
            {
               if (found)
               strcat(buf, " / ");
               else
               found = 1;
               strcat(buf, dictionary[dictpos][-1*(itranstype-1)]);
            }
            dictpos++;
         }
         if (!found) strcat(buf, transword);
      }

      strcat(buf, "\n\r");
      send_to_char(buf, ch);

      return;
   }
   else
   if (obj->pIndexData->vnum == OBJ_VNUM_IMPROV_BOX)
   {
      char* object_list[] =
      {
         "a fork",
         "a mattress",
         "a sheet",
         "a pillow",
         "a tassel",
         "a pillowcase",
         "a feather",
         "a vase",
         "a glasses",
         "a plate",
         "a toilet",
         "a paper",
         "a towel",
         "a wash basin",
         "a rack",
         "a basket",
         "a box",
         "a curtain",
         "a quilt",
         "an hourglass",
         "a duster",
         "a broom",
         "a reed mat",
         "a bed",
         "a chair",
         "a desk",
         "an outhouse",
         "a table",
         "a rug",
         "a tapestry",
         "a key",
         "a scroll",
         "a bag",
         "an ice cube",
         "a stove",
         "a box",
         "a ladder",
         "a fence",
         "a dagger",
         "a shield",
         "a sword",
         "a staff",
         "a spear",
         "a mace",
         "a flail",
         "an axe",
         "a log",
         "a whip",
         "a rose",
         "a daisy",
         "a tulip",
         "a rake",
         "a pickle",
         "a slice of bread",
         "a stick",
         "a quill",
         "a pendant",
         "a severed finger",
         "a glass eye",
         "a fish head",
         "a bucket",
         "a big red ball",
         "a lady's fluffy wig"
      };
      const int kNumberOfObjects = sizeof(object_list) / sizeof(char*);
      int object_itr;

      /* WARNING: (int*) cast of (char*) may not use the same amount of space
       *          on other systems and implementations.
       */
      shuffle(kNumberOfObjects, (int*) object_list, (int*) object_list);

      act(
         "You open the mysterious box.  Black smoke pours out, and a blue hand with\n\r"
         "long, purple nails and golden glitter emerges.  The hand snaps its fingers,\n\r"
         "closes, and opens amidst more smoke to reveal three objects:\n\r",
         ch,
         NULL,
         0,
         TO_CHAR);
      act(
         "$n opens the mysterious box.  Black smoke pours out, and a blue\n\r"
         "hand with long, purple nails and golden glitter emerges.  The hand snaps\n\r"
         "its fingers, closes, and opens amidst more smoke to reveal three objects:\n\r",
         ch,
         NULL,
         0,
         TO_ROOM);

      for (object_itr = 0; object_itr < 3; object_itr++)
      {
         act("   $t", ch, object_list[object_itr], NULL, TO_CHAR);
         act("   $t", ch, object_list[object_itr], NULL, TO_ROOM);
      }

      return;
   }

   send_to_char("You are unable to figure out how to use the item.\n\r", ch);
   return;
}

void do_drain( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH], dcontainer[MAX_INPUT_LENGTH];
   OBJ_DATA *obj, *odrink;

   if (get_skill(ch, gsn_drain) <= 0 ||
   !has_skill(ch, gsn_drain)) {
      send_to_char("Huh?\n\r", ch);
      return;
   }

   if (house_down(ch, HOUSE_ANCIENT))
   return;

   argument = one_argument( argument, arg );

   if ( arg[0] == '\0' )
   {
      send_to_char( "Drain blood from which corpse?\n\r", ch );
      return;
   }

   one_argument( argument, dcontainer );
   if (dcontainer[0] == '\0')
   {
      send_to_char("What container will you use to hold the blood?\n\r", ch);
      return;
   }

   obj = get_obj_list( ch, arg, ch->in_room->contents );
   if ( obj == NULL )
   {
      send_to_char( "You can't find it.\n\r", ch );
      return;
   }

   if ( ( odrink = get_obj_carry( ch, dcontainer, ch ) ) == NULL )
   {
      send_to_char( "You do not have that container.\n\r", ch );
      return;
   }

   if (ch->fighting != NULL)
   {
      send_to_char( "You're too busy fighting to drain blood from a corpse.\n\r", ch);
      return;
   }

   if ((obj->item_type != ITEM_CORPSE_PC) && (obj->item_type != ITEM_CORPSE_NPC))
   {
      send_to_char("That is not a corpse.\n\r", ch);
      return;
   }

   if
   (
      IS_SET(obj->extra_flags2, ITEM_NO_BLOOD) ||
      IS_SET(obj->extra_flags, CORPSE_NO_ANIMATE) ||
      obj->timer < 4
   )
   {
      send_to_char("The corpse is too dried up.\n\r", ch);
      return;
   }

   if
   (
      (odrink->value[1] > 0)
      && !(
         (
            (obj->item_type == ITEM_CORPSE_PC)
            && (odrink->value[2] == liq_lookup("lifeblood"))
         )
         || (
            (obj->item_type == ITEM_CORPSE_NPC)
            && (odrink->value[2] == liq_lookup("blood") )
         )
      )
   )
   {
      send_to_char("Your container already holds a different substance.\n\r", ch);
      return;
   }

   if (odrink->value[1] >= odrink->value[0])
   {
      send_to_char("That container is already full.\n\r", ch);
      return;
   }

   if (number_percent() >= get_skill(ch, gsn_drain))
   {
      act( "$n tries to siphon blood from $p but spills it in the process.", ch, obj, NULL, TO_ROOM );
      act( "You try to siphon blood from $p but spill it accidently.", ch, obj, NULL, TO_CHAR );
      obj->timer--;
      check_improve(ch, gsn_drain, FALSE, 1);
      return;
   }

   act( "$n siphons blood from $p, draining it completely.", ch, obj, NULL, TO_ROOM );
   act( "You siphon blood from $p, draining it completely.", ch, obj, NULL, TO_CHAR );
   SET_BIT(obj->extra_flags2, ITEM_NO_BLOOD);
   check_improve(ch, gsn_drain, TRUE, 1);
   odrink->value[1] = UMIN(odrink->value[0], odrink->value[1] + 64);
   odrink->value[2] = (obj->item_type == ITEM_CORPSE_PC) ?
   liq_lookup("lifeblood") : liq_lookup("blood");

   return;
}

void wear_obj_physical_focus(CHAR_DATA *ch, OBJ_DATA *obj)
{
   act( "Your muscles and mind begin to work in complete harmony.", ch, NULL, NULL, TO_CHAR );
   return;
}

void wear_obj_mental_focus(CHAR_DATA *ch, OBJ_DATA *obj)
{
   act( "The facets of your mind combine and work together.", ch, NULL, NULL, TO_CHAR );
   return;
}

void do_trapmaking(CHAR_DATA *ch, char * argument)
{
   OBJ_DATA *trap;
   int type = 0;
   AFFECT_DATA af;
   AFFECT_DATA *paf;
   bool trap_type[4];

   if (get_skill(ch, gsn_trapmaking) <= 0 ||
   !has_skill(ch, gsn_trapmaking))
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }
   trap_type[0] = FALSE;
   trap_type[1] = FALSE;
   trap_type[2] = FALSE;
   trap_type[3] = FALSE;
   for ( paf = ch->affected; paf != NULL; paf = paf->next )
   {
      if ( paf->type == gsn_trapmaking && paf->modifier >= 0 &&
      paf->modifier <= 3)
      trap_type[paf->modifier] = TRUE;
   }


   if (ch->move < 20)
   {
      send_to_char("Your not up to making a trap at the moment.\n\r", ch);
      return;
   }
   if (argument[0] == '\0')
   {
      send_to_char("Make which type of trap: vine, snare, log, punjie\n\r", ch);
      return;
   }
   if (!str_cmp(argument, "vine")) type = 0;
   if (!str_cmp(argument, "snare")) type = 1;
   if (!str_cmp(argument, "log")) type = 2;
   if (!str_cmp(argument, "punjie")) type = 3;
   switch (type)
   {
      default:
      case 0:
      if (trap_type[0])
      {
         send_to_char("You can't make another vine trap yet.\n\r", ch);
         return;
      }
      break;
      case 1:
      if (ch->level < 20)
      {
         send_to_char("You haven't learned that trap yet.\n\r", ch);
         return;
      }
      if (trap_type[1])
      {
         send_to_char("You can't make another snare yet.\n\r", ch);
         return;
      }
      break;
      case 2:
      if (ch->level < 30)
      {
         send_to_char("You haven't learned that trap yet.\n\r", ch);
         return;
      }
      if (trap_type[2])
      {
         send_to_char("You can't make another log trap yet.\n\r", ch);
         return;
      }
      break;
      case 3:
      if (ch->level < 40)
      {
         send_to_char("You haven't learned that trap yet.\n\r", ch);
         return;
      }
      if (trap_type[3])
      {
         send_to_char("You can't make more punjie sticks yet.\n\r", ch);
         return;
      }
      break;
   }
   ch->move -= 20;
   WAIT_STATE(ch, 24);
   if (number_percent() > get_skill(ch, gsn_trapmaking))
   {
      send_to_char("Your trap doesn't look like it can catch anything.\n\r", ch);
      check_improve(ch, gsn_trapmaking, FALSE, 2);
      return;
   }
   check_improve(ch, gsn_trapmaking, TRUE, 2);
   trap = create_object( get_obj_index( OBJ_VNUM_TRAP ), 0 );
   trap->value[0] = type;
   trap->weight = 100;
   trap->timer = 24;
   switch (type)
   {
      default:
      case 0:
      free_string( trap->short_descr );
      free_string( trap->description );
      free_string (trap->name);
      trap->short_descr = str_dup( "a vine trap" );
      trap->description  = str_dup( "A vine trap lies here." );
      trap->name = str_dup( "vine trap" );
      trap->timer = 36;
      break;
      case 1:
      free_string( trap->short_descr );
      free_string( trap->description );
      free_string (trap->name);
      trap->short_descr = str_dup( "a snare" );
      trap->description  = str_dup( "A snare lies here." );
      trap->name = str_dup( "snare trap" );
      break;
      case 2:
      free_string( trap->short_descr );
      free_string( trap->description );
      free_string (trap->name);
      trap->short_descr = str_dup( "a log trap" );
      trap->description  = str_dup( "A log trap is here." );
      trap->name = str_dup( "log trap" );
      break;
      case 3:
      free_string( trap->short_descr );
      free_string( trap->description );
      free_string (trap->name);
      trap->short_descr = str_dup( "some punjie sticks" );
      trap->description  = str_dup( "Some punjie sticks are here." );
      trap->name = str_dup( "punjie stick trap" );
      break;
   }
   af.where = TO_AFFECTS;
   af.type = gsn_trapmaking;
   af.modifier = type;
   af.location = 0;
   af.duration = 12;
   af.level = ch->level;
   af.bitvector = 0;
   affect_to_char(ch, &af);

   act( "$n skillfully makes $p.", ch, trap, NULL, TO_ROOM );
   act( "You skillfully make $p.", ch, trap, NULL, TO_CHAR );
   obj_to_char( trap, ch );
   return;
}

void do_trapset(CHAR_DATA *ch, char *argument)
{
   OBJ_DATA *trap;
   int type;
   ROOM_AFFECT_DATA raf;

   if (!has_skill(ch, gsn_trapmaking) ||
   get_skill(ch, gsn_trapmaking) <= 0)
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }
   if ( ( trap = get_obj_carry( ch, argument, ch ) ) == NULL )
   {
      send_to_char("You don't have that trap to set.\n\r", ch);
      return;
   }
   if (trap->pIndexData->vnum != OBJ_VNUM_TRAP)
   {
      send_to_char("That isn't a trap.\n\r", ch);
      return;
   }
   type = trap->value[0];
   switch (type)
   {
      default:
      case 1:
      case 2:
      if ((ch->in_room->sector_type != SECT_FOREST)
      && (ch->in_room->sector_type != SECT_HILLS)
      && (ch->in_room->sector_type != SECT_MOUNTAIN) )
      {
         send_to_char("You can't set that here.\n\r", ch);
         return;
      }
      break;
      case 0:
      if (ch->in_room->sector_type == SECT_WATER_SWIM ||
      ch->in_room->sector_type == SECT_WATER_NOSWIM ||
      ch->in_room->sector_type == SECT_UNDERWATER ||
      ch->in_room->sector_type == SECT_AIR)
      {
         send_to_char("You can't set that here.\n\r", ch);
         return;
      }
      break;
      case 3:
      if ((ch->in_room->sector_type != SECT_FOREST)
      && (ch->in_room->sector_type != SECT_HILLS)
      && (ch->in_room->sector_type != SECT_UNDERGROUND)
      && (ch->in_room->sector_type != SECT_DESERT)
      && (ch->in_room->sector_type != SECT_FIELD)
      && (ch->in_room->sector_type != SECT_MOUNTAIN) )
      {
         send_to_char("You can't set that here.\n\r", ch);
         return;
      }
      break;

   }
   if (ch->in_room->house != 0)
   {
      send_to_char("You can't place a trap here.\n\r", ch);
      return;
   }
   type = trap->value[0];
   act( "$n skillfully sets $p.", ch, trap, NULL, TO_ROOM );
   act( "You skillfully set $p.", ch, trap, NULL, TO_CHAR );
   extract_obj(trap, FALSE);
   raf.where  = TO_ROOM;
   raf.duration = 24;
   raf.type = gsn_trapmaking;
   raf.level = ch->level;
   raf.modifier = type;
   raf.bitvector = 0;
   /* Replacements as per Fizzfaldt for fixing traps
   raf.caster = str_dup(ch->name);
   affect_to_room(ch->in_room, &raf);
   return; */

   raf.caster = ch;
   affect_to_room(ch->in_room, &raf);

   return;
}

void do_disarmtrap(CHAR_DATA *ch, char *argument)
{
   if (!IS_AFFECTED(ch, AFF_ACUTE_VISION))
   {
      send_to_char("You do not see a trap here.\n\r", ch);
      return;
   }
   if (!is_affected_room(ch->in_room, gsn_trapmaking))
   {
      send_to_char("You do not see a trap here.\n\r", ch);
      return;
   }
   if (number_percent() < 30)
   {
      act( "$n skillfully disarms a trap.", ch, NULL, NULL, TO_ROOM );
      act( "You skillfully disarm the trap.", ch, NULL, NULL, TO_CHAR );
      affect_strip_room_single(ch->in_room, gsn_trapmaking);
      return;
   }
   if (number_percent() < 30)
   {
      REMOVE_BIT(ch->affected_by, AFF_ACUTE_VISION);
      trap_trigger_check(ch);
      SET_BIT(ch->affected_by, AFF_ACUTE_VISION);
   }
   return;
}

void trap_trigger_check(CHAR_DATA *ch)
{
   ROOM_AFFECT_DATA *raf;
   char buf1[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];
   int type;
   AFFECT_DATA af;
   int dam;
   int this_one_went_off = FALSE;

   if (ch == NULL || ch->in_room == NULL)
   return;
   if (IS_IMMORTAL(ch))
   return;
   if (IS_AFFECTED(ch, AFF_ACUTE_VISION) && number_percent() < 90)
   return;
   if (!IS_NPC(ch) && ch->pcdata->spirit_room != NULL)
   return;
   if (!is_affected_room(ch->in_room, gsn_trapmaking))
   return;

   for ( raf = ch->in_room->affected; raf != NULL; raf = raf->next )
   {
      if (raf->type == gsn_trapmaking && raf->caster != ch)
      {
         type = raf->modifier;
         if (!is_safe_room(ch, raf, IS_SAFE_SILENT))
         {
            if (number_percent() < 10)
            {
               continue; /* chance they missed the trap altogether */
            }
            this_one_went_off = TRUE;
            switch (type)
            {
               case 0:
               sprintf(buf1, "$n trips over a vine.");
               sprintf(buf2, "You trip over a vine.");
               if (number_percent() < 5)
               this_one_went_off = FALSE;
               else
               {
                  stop_fighting(ch, FALSE);
                  ch->position = POS_RESTING;
                  WAIT_STATE(ch, 35);
               }
               break;
               case 1:
               sprintf(buf1, "$n is trapped by a snare.");
               sprintf(buf2, "You are trapped in a snare!");
               af.where = TO_AFFECTS;
               af.type = gsn_trapstun;
               af.location = 0;
               af.modifier = 0;
               af.duration = 3;
               af.bitvector = 0;
               af.level = ch->level;
               if (number_percent() < 20)
               this_one_went_off = FALSE;
               else
               {
                  if (!IS_IMMORTAL(ch))
                  {
                     QUIT_STATE(ch, af.duration + 1);
                  }
                  affect_to_char(ch, &af);
               }
               break;
               case 2:
               sprintf(buf1, "$n is slammed to the ground by a swinging log.");
               sprintf(buf2, "SNAP!  Ouch!  Everything goes dark!");
               af.where = TO_AFFECTS;
               af.type = gsn_sleep;
               af.location = 0;
               af.modifier = 0;
               af.duration = number_range(2, 4);
               af.bitvector = AFF_SLEEP;
               af.level = ch->level;
               if (number_percent() < 60 || ch->race == grn_arborian)
               this_one_went_off = FALSE;
               else
               {
                  if (!IS_IMMORTAL(ch))
                  {
                     QUIT_STATE(ch, af.duration + 1);
                  }
                  affect_to_char(ch, &af);
               }
               break;
               case 3:
               sprintf(buf1, "A series of sharp pointy sticks impales $n.");
               sprintf(buf2, "A series of sharp pointy sticks impales you.");
               dam = ch->level * number_range(3, 7);
               if (number_percent() < 50)
               this_one_went_off = FALSE;
               else
               damage(ch, ch, dam, gsn_trapmaking, DAM_PIERCE, TRAP_PUNJIE_DAM_TYPE);
               break;
            }
            if (this_one_went_off)
            {
               sprintf
               (
                  log_buf,
                  "%s caught in a trap placed by %s. Room [%d].",
                  ch->name,
                  raf->caster ? raf->caster->name : "",
                  ch->in_room->vnum
               );
               log_string(log_buf);
               do_myell(ch, "Help, I've been caught in a trap!");
               act( buf1, ch, NULL, NULL, TO_ROOM );
               act( buf2, ch, NULL, NULL, TO_CHAR );
               if (type == 2)
               {
                  stop_fighting(ch, FALSE);
                  ch->position = POS_SLEEPING;
               }
               affect_remove_room(ch->in_room, raf);
               return;
            }
            else
            {
               do_myell(ch, "Help, I was nearly caught in a trap!");
               switch (type)
               {
                  case 0:
                  send_to_char("You narrowly miss falling into a vine trap.\n\r", ch);
                  break;
                  case 1:
                  send_to_char("You almost fall into a snare.\n\r", ch);
                  break;
                  case 2:
                  send_to_char("You barely manage to dodge a swinging log.\n\r", ch);
                  break;
                  case 3:
                  send_to_char("A series of sharp sticks fly by you missing by inches.\n\r", ch);
                  break;
               }
               return;
            }
         }
      }
   }
   return;
}


void do_apply( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   char buf[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   OBJ_DATA *obj;
   AFFECT_DATA af;
   int counter    = 0;
   bool herb_used = FALSE;
   int cast_bits  = 0;

   argument = one_argument(argument, arg);
   if ( arg[0] == '\0' )
   {
      send_to_char( "Apply what to whom?\n\r", ch );
      return;
   }
   if ((obj = get_obj_carry(ch, arg, ch)) == NULL)
   {
      send_to_char("You do not have that herb to apply.\n\r", ch);
      return;
   }
   victim = NULL;
   if (argument[0] != '\0')  /* There is an argument, and a victim */
   {
      victim = get_char_room(ch, argument);
   }
   if (argument[0] == '\0')  /* There is an argument, you are the victim */
   {
      victim = ch;
   }
   if (victim == NULL)  /* Victim isn't in the room, or victim doesn't exist */
   {
      send_to_char("They are not here.\n\r", ch);
      return;
   }
   if ( obj->item_type != ITEM_HERB )/* Trying to apply something that isn't an herb */
   {
      send_to_char( "You can only apply herbs.\n\r", ch );
      return;
   }
   if (ch->fighting != NULL)
   {
      send_to_char("You're too busy fighting to apply anything.\n\r", ch);
      return;
   }
   if (is_affected(victim, gsn_forage))  /* Currently using a single timer for every herb */
   {
      send_to_char("They have already had their wounds treated with herbs.\n\r", ch);
      return;
   }
   if (oblivion_blink(ch, victim))
   {
      return;
   }
   /*
      Victim is not affected by the timer
      get the timer ready on the victim
      but do not apply it yet
   */
   af.where     = TO_AFFECTS;
   af.type      = gsn_forage;
   af.duration  = 3;
   af.modifier  = 0;
   af.location  = 0;
   af.bitvector = 0;
   af.level     = ch->level;

   if (victim == ch)
   {
      act("$n applies $p to $s wounds.", ch, obj, NULL, TO_ROOM);
      act("You apply $p to your wounds.", ch, obj, NULL , TO_CHAR);
   }
   else
   {
      /*
         Check for self-only herbs
      */
      if
      (
         obj->value[1] == gsn_cancellation ||
         obj->value[2] == gsn_cancellation ||
         obj->value[3] == gsn_cancellation ||
         obj->value[4] == gsn_cancellation
      )
      {
         send_to_char("You cannot apply those herbs to other people.\n\r", ch);
         return;
      }
      /*
         1st person ch, 1st person victim, 3rd person views
      */
      act("$n applies $p to $N's wounds.", ch, obj, victim, TO_NOTVICT);
      act("$n applies $p to your wounds.", ch, obj, victim, TO_VICT);
      act("You apply $p to $N's wounds.", ch, obj, victim, TO_CHAR);
   }
   for (counter = 1; counter < 5; counter++)
   {
      if
      (
         obj->value[counter] < 1 ||
         obj->value[counter] > MAX_SKILL
      )
      {
         continue;
         /*
            Not using nonexistant skills
         */
      }
      else
      {
         /*
            At least one skill that can actually be used
            the herb is actually being used
            Mostly for debugging
            as well as new herbs added later
         */
         herb_used = TRUE;
         if
         (
            obj->value[counter] == gsn_herb ||
            obj->value[counter] == gsn_heal
         )
         {
            if (is_affected(victim, gsn_black_mantle))
            {
               send_to_char("The black mantle absorbs your healing.\n\r", victim);
            }
            else
            {
               int heal;

               if (obj->value[counter] == gsn_heal)
               {
                  heal = 100;
               }
               else
               {
                  heal = obj->level * 4;
               }
               if
               (
                  is_affected(victim, gsn_mantle_oblivion) ||
                  (
                     !IS_NPC(victim) &&
                     IS_SET(victim->act2, PLR_LICH)
                  )
               )
               {
                  heal /= 2;
               }
               victim->hit = UMIN(victim->hit + heal, victim->max_hit);
               update_pos(victim);
               send_to_char("A warm feeling fills your body.\n\r", victim);
            }
         }
         else
         {
            cast_bits = 0;
            /*
               Its an herb casting the spell
            */
            SET_BIT(cast_bits, OBJ_CAST_HERB);
            /*
               Its a (magical) herb casting the spell
               Currently not used, can be commented out to use
            */
            /*
               SET_BIT(cast_bits, OBJ_CAST_MAGICAL_HERB);
            */
            /*
               This herb will ignore no_magic rooms
               (if its a magical herb, it probably
               will not ignore magical rooms, this can be done later)
            */
            SET_BIT(cast_bits, OBJ_CAST_IGNORE_NO_MAGIC);
            /*
               If its an offensive spell being used,
               ch will cast it on victim
               else, victim will cast on itself
            */
            SET_BIT(cast_bits, CAST_BIT_NO_BLOCK_BY_TAKE);
            /*
               Ignores 'takemagic' (this is really not magical)
            */
            switch (skill_table[obj->value[counter]].target)
            {
               default:
               {
                  /*
                     Non offensive spell, victim casting on itself
                  */
                  obj_cast_spell_2
                  (
                     obj->value[counter],
                     obj->value[0],
                     victim,
                     victim,
                     NULL,
                     cast_bits
                  );
                  break;
               }
               case TAR_CHAR_OFFENSIVE:
               case TAR_OBJ_CHAR_OFF:
               {
                  /*
                     offensive spell, ch casting on victim
                     currently no herb is offensive
                  */
                  obj_cast_spell_2
                  (
                     obj->value[counter],
                     obj->value[0],
                     ch,
                     victim,
                     NULL,
                     cast_bits
                  );
                  break;
               }
            }
         }
      }
   }
   if (herb_used)
   {
      WAIT_STATE(ch, 12);
      /*
         Affect is added here.  If we quit out because
         a self only herb was being used on someone else,
         or there was a bug with the herb,
         no effect is placed.
      */
      affect_to_char(victim, &af);
   }
   else
   {
      /*
         bug, the herb had no spells??
         Or, all spells were invalid
         if the herb is bugged, lets not
         lag the player using it, no wait_state
      */
      sprintf(buf, "do_apply: herb: %s had no spell/skill", obj->short_descr);
      bug(buf, 0);
   }
   /*
      Always extract the herb
   */
   extract_obj( obj, FALSE );
   return;
}

/*Jord*/
void crusader_druid_prog(CHAR_DATA* ch, CHAR_DATA* smith, OBJ_DATA* weapon)
{
   char buf[MAX_INPUT_LENGTH];
   int dt;
   bool drop = FALSE;

   sprintf(buf, "1. %s", ch->name);
   dt = weapon->value[3];
   if
   (
      dt >= 0 &&
      dt < MAX_DAMAGE_MESSAGE
   )
   {
      dt = attack_table[dt].damage;
   }
   else
   {
      dt = DAM_NONE;
   }

   if
   (
      !IS_NPC(smith) ||
      smith->pIndexData->vnum != 5714 ||
      ch->house != HOUSE_CRUSADER
   )
   {
      return;
   }
   if (weapon->item_type != ITEM_WEAPON)
   {
      do_say(smith, "I can only purify weapons.");
      if (IS_SET(weapon->extra_flags, ITEM_NODROP))
      {
         drop = TRUE;
         REMOVE_BIT(weapon->extra_flags, ITEM_NODROP);
      }
      do_give(smith, buf);
      if (smith->carrying == weapon)
      {
         do_emote(smith, "mutters about people carrying too many things.");
         do_drop(smith, "1.");
      }
      if (drop)
      {
         SET_BIT(weapon->extra_flags, ITEM_NODROP);
      }
      return;
   }
   if
   (
      dt == DAM_ENERGY ||
      dt == DAM_NEGATIVE ||
      dt == DAM_LIGHT ||
      IS_WEAPON_STAT(weapon, WEAPON_VAMPIRIC) ||
      weapon->pIndexData->vnum == OBJ_VNUM_TALON ||
      (
         weapon->pIndexData->vnum == OBJ_VNUM_RANGER_STAFF &&
         !str_cmp(weapon->material, "magic root wood")
      ) ||
      weapon->pIndexData->vnum == OBJ_VNUM_SWORD_DEMONS ||
      weapon->pIndexData->vnum == OBJ_VNUM_GENOCIDE_AXE ||
      weapon->pIndexData->vnum == 27035 ||
      weapon->pIndexData->vnum == 27036 ||
      weapon->pIndexData->vnum == 27037 ||
      weapon->pIndexData->vnum == 27038 ||
      weapon->pIndexData->vnum == 27039 ||
      weapon->pIndexData->vnum == 27040 ||
      weapon->pIndexData->vnum == 27041 ||
      weapon->pIndexData->vnum == 27042 ||
      weapon->pIndexData->vnum == 8003
   )
   {
      do_say(smith, "This cannot be purified, they are too tainted.");
      do_emote(smith, "destroys the weapon with one hard smash.");
      /* obj_from_char( weapon ); */
      /*
         Make it vis_death.. as far as mortals know its destroyed, but
         mages can't go and get it again.
      */
      SET_BIT(weapon->extra_flags, ITEM_VIS_DEATH);
      return;
   }
   else if
   (
      (
         IS_OBJ_STAT(weapon, ITEM_INVIS) ||
         dt == DAM_FIRE ||
         dt == DAM_COLD ||
         dt == DAM_LIGHTNING ||
         dt == DAM_DROWNING ||
         IS_WEAPON_STAT(weapon, WEAPON_FLAMING) ||
         IS_WEAPON_STAT(weapon, WEAPON_FROST) ||
         IS_WEAPON_STAT(weapon, WEAPON_SHOCKING) ||
         IS_WEAPON_STAT(weapon, WEAPON_DROWNING) ||
         IS_WEAPON_STAT(weapon, WEAPON_RESONATING) ||
         (
            weapon->pIndexData->vnum == OBJ_VNUM_RANGER_STAFF &&
            IS_SET(weapon->extra_flags, ITEM_MAGIC)
         )
      ) &&
      (
         !IS_SET(weapon->extra_flags2, ITEM_PURIFIED) ||
         weapon->enchanted
      )
   )
   {
      un_pulse(weapon);
      REMOVE_BIT(weapon->extra_flags, ITEM_INVIS);
      if (is_pulsing(weapon))
      {
         /* If still pulsing.. */
         do_say(smith, "This cannot be purified, they are too tainted.");
         do_emote(smith, "destroys the weapon with one hard smash.");
         SET_BIT(weapon->extra_flags, ITEM_VIS_DEATH);
         return;
      }
      SET_BIT(weapon->extra_flags2, ITEM_PURIFIED);
      if (IS_SET(weapon->extra_flags, ITEM_NODROP))
      {
         drop = TRUE;
         REMOVE_BIT(weapon->extra_flags, ITEM_NODROP);
      }
      do_give(smith, buf);
      do_say(smith, "This weapon is now imbued with the powers of the gods, it is pure");
      if (smith->carrying == weapon)
      {
         do_emote(smith, "mutters about people carrying too many things.");
         do_drop(smith, "1.");
      }
      if (drop)
      {
         SET_BIT(weapon->extra_flags, ITEM_NODROP);
      }
   }
   else if (is_pulsing(weapon))
   {
      do_say(smith, "The pulsing can not be purified by me.");
      do_give(smith, buf);
   }
   else
   {
      do_say(smith, "This needs no purification.");
      if (IS_SET(weapon->extra_flags, ITEM_NODROP))
      {
         drop = TRUE;
         REMOVE_BIT(weapon->extra_flags, ITEM_NODROP);
      }
      do_give(smith, buf);
      if (drop)
      {
         SET_BIT(weapon->extra_flags, ITEM_NODROP);
      }
   }
   if (smith->carrying == weapon)
   {
      do_emote(smith, "mutters about people carrying too many things.");
      if (IS_SET(weapon->extra_flags, ITEM_NODROP))
      {
         drop = TRUE;
         REMOVE_BIT(weapon->extra_flags, ITEM_NODROP);
      }
      do_drop(smith, "1.");
      if (drop)
      {
         SET_BIT(weapon->extra_flags, ITEM_NODROP);
      }
   }
}

/* Command only usable within Rantialen's shrine room, the Ahzra - Wicket */
void do_immerse( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];
   OBJ_DATA *obj = NULL;

   if ( ch->in_room->vnum != 7030 )
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }

   argument = one_argument( argument, arg );

   if ( arg[0] == '\0' )
   {
      send_to_char( "Immerse what into the depths of the pool?\n\r", ch );
      return;
   }

   /* Immerse the character */
   if ( !str_cmp( arg, "self" ) || is_name( arg, ch->name ) )
   {
      if ( ch->house == HOUSE_ANCIENT && is_affected( ch, gsn_cloak_form ) )
      {
         send_to_char( "You step to the edge of the bloody Ahzra, but an unseen force prevents you from staining your cloak.\n\r", ch );
         return;
      }

      act( "$n wades into the bloody Ahzra and is wholly submersed, embracing the reality of Eternity.", ch, obj, NULL, TO_ROOM );
      act( "You slowly lower yourself into the murky depths of the Ahzra, staining your body, and marking your soul.", ch, obj, NULL, TO_CHAR );
      if ( !IS_SET( ch->affected_by2, AFF_AHZRA_BLOODSTAIN ) )
      SET_BIT( ch->affected_by2, AFF_AHZRA_BLOODSTAIN );

      if ( ch->alignment > -1000 )
      {
         if ( IS_GOOD( ch ) )
         send_to_char( "Your soul is sapped of its former radiance!\n\r", ch );
         else
         if ( IS_NEUTRAL( ch ) )
         send_to_char( "Your vision blurs with crimson as the harmony is purged from your soul!\n\r", ch );

         sprintf( buf, "%s's alignment changed at the Ahzra.", ch->name );
         sprintf( buf2, "\n\rClass: %s\n\rRace : %s\n\rAlign: %d (Previous)\n\r", class_table[ch->class].name, race_table[ch->race].name, ch->alignment );
         make_note("Automated Immerse Note", buf, "Rantialen", buf2, NOTE_NOTE);

         update_alignment(ch);

         sprintf(log_buf, "[%s]'s alignment changed at the Ahzra (%d).", ch->name, ch->in_room->vnum);
         log_string(log_buf);
      }

      return;
   }

   /* Immerse an item */
   if ( !str_cmp( arg, "all" ) || !str_prefix( "all.", arg ) )
   {
      send_to_char( "You can only immerse one item at a time.\n\r", ch );
      return;
   }

   if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
   {
      send_to_char( "You do not have that item.\n\r", ch );
      return;
   }

   act( "The pool curdles eerily as $n dips $p within its depths.", ch, obj, NULL, TO_ROOM );
   act( "The pool curdles eerily as you dip $p within its depths.", ch, obj, NULL, TO_CHAR );
   if ( !IS_SET( obj->extra_flags2, ITEM_AHZRA_BLOODSTAIN ) )
   SET_BIT( obj->extra_flags2, ITEM_AHZRA_BLOODSTAIN );

   /* Good-only and House items are destroyed */
   if
   (
      (
         IS_OBJ_STAT( obj, ITEM_ANTI_NEUTRAL) &&
         IS_OBJ_STAT(obj, ITEM_ANTI_EVIL)
      ) || obj->pIndexData->house
   )
   {
      act( "$p dissolves, bubbling away into a thick froth.", ch, obj, NULL, TO_ALL );
      extract_obj( obj, FALSE );
      return;
   }

   /* Blessed items are dulled */
   if ( IS_OBJ_STAT( obj, ITEM_BLESS ) )
   {
      act( "$p fades, its once vibrant lustre now dull.", ch, obj, NULL, TO_ALL );
      REMOVE_BIT( obj->extra_flags, ITEM_BLESS );
   }

   return;
   /* Make the item evil-only */
   if ( !IS_OBJ_STAT( obj, ITEM_ANTI_GOOD ) )
   SET_BIT( obj->extra_flags, ITEM_ANTI_GOOD );
   if ( !IS_OBJ_STAT( obj, ITEM_ANTI_NEUTRAL ) )
   SET_BIT( obj->extra_flags, ITEM_ANTI_NEUTRAL );
   if ( IS_OBJ_STAT( obj, ITEM_ANTI_EVIL ) )
   REMOVE_BIT( obj->extra_flags, ITEM_ANTI_EVIL );

   return;
}

/*
   Dice rolling command used for the die sold within Glyndane Library.
   Contributed by Mornelithe - Wicket
*/
void do_dice_roll(CHAR_DATA * ch, char * argument)
{
   char num_string[MAX_INPUT_LENGTH];
   char msg[MAX_STRING_LENGTH];
   int dice_count = 0;
   int num_roll;
   const int kMaxDicePerLine = 5;
   int dice_roll[5];  /* [kMaxDicePerLine] */
   int dice_total;
   int dice_rolled;
   int count;
   int diedraw;
   OBJ_DATA * obj;

   argument = one_argument(argument, num_string);

   if (num_string[0] == '\0')
   {
      send_to_char("How many dice do you want to roll?\n\r", ch);
      return;
   }

   num_roll = atoi(num_string);

   if (num_roll < 1)
   {
      send_to_char("You must roll at least 1 die.\n\r", ch);
      return;
   }

   for (obj = ch->carrying;obj != NULL;obj = obj->next_content)
   {
      if (obj->pIndexData->vnum == OBJ_VNUM_DICE)
      dice_count++;
   }

   if (dice_count == 0)
   {
      send_to_char("You are not carrying any dice.\n\r", ch);
      return;
   }

   if (dice_count < num_roll)
   {
      send_to_char("You are not carrying that many dice.\n\r", ch);
      return;
   }

   if (num_roll == 1)
   {
      act("You roll a six-sided die...", ch, NULL, NULL, TO_CHAR);
      act("$n rolls a six-sided die...", ch, NULL, NULL, TO_ROOM);
   }
   else
   {
      sprintf(msg, "You roll %d six-sided dice...", num_roll);
      act(msg, ch, NULL, NULL, TO_CHAR);
      sprintf(msg, "$n rolls %d six-sided dice...", num_roll);
      act(msg, ch, NULL, NULL, TO_ROOM);
   }

   dice_total = 0;
   dice_rolled = 0;
   for (count = 0; count < num_roll; count++)
   {
      dice_roll[dice_rolled] = number_range(1, 6);
      dice_rolled++;
      dice_total += dice_roll[count % kMaxDicePerLine];

      /* ----------------------------------------------------------------
         We print at every kMaxDicePerLine steps, provided there are not
         too many dice
         ---------------------------------------------------------------- */
      if (
            (num_roll <= (kMaxDicePerLine * 2))
            &&
            (
               (dice_rolled == kMaxDicePerLine) || (count+1 == num_roll)
            )
         )
      {
         sprintf (msg, "\n\r");

         /* Line 1 of 5 */
         for (diedraw = 0; diedraw < dice_rolled; diedraw++)
            strcat (msg, "+-----+ ");
         strcat (msg, "\n\r");

         /* Line 2 of 5 */
         for (diedraw = 0; diedraw < dice_rolled; diedraw++)
            switch (dice_roll[diedraw])
            {
               case 1:  strcat (msg, "|     | ");  break;
               case 2:
               case 3:  strcat (msg, "|o    | ");  break;
               case 4:
               case 5:
               case 6:  strcat (msg, "|o   o| ");  break;
            }
         strcat (msg, "\n\r");

         /* Line 3 of 5 */
         for (diedraw = 0; diedraw < dice_rolled; diedraw++)
            switch (dice_roll[diedraw])
            {
               case 2:
               case 4:  strcat (msg, "|     | ");  break;
               case 1:
               case 3:
               case 5:  strcat (msg, "|  o  | ");  break;
               case 6:  strcat (msg, "|o   o| ");  break;
            }
         strcat (msg, "\n\r");

         /* Line 4 of 5 */
         for (diedraw = 0; diedraw < dice_rolled; diedraw++)
            switch (dice_roll[diedraw])
            {
               case 1:  strcat (msg, "|     | ");  break;
               case 2:
               case 3:  strcat (msg, "|    o| ");  break;
               case 4:
               case 5:
               case 6:  strcat (msg, "|o   o| ");  break;
            }
         strcat (msg, "\n\r");

         /* Line 5 of 5 */
         for (diedraw = 0; diedraw < dice_rolled; diedraw++)
            strcat (msg, "+-----+ ");

         act(msg, ch, NULL, NULL, TO_CHAR);
         act(msg, ch, NULL, NULL, TO_ROOM);

         dice_rolled = 0;
      }
   }

   sprintf (
      msg,
      "%sTotal = %d",
      num_roll <= (kMaxDicePerLine * 2) ? "\n\r" : "",
      dice_total);
   act(msg, ch, NULL, NULL, TO_CHAR);
   act(msg, ch, NULL, NULL, TO_ROOM);

   WAIT_STATE(ch, PULSE_VIOLENCE);
}

void do_pricelist(CHAR_DATA *ch, char *argument)
{
   OBJ_INDEX_DATA *obj;
   int cnt;
   char buf[MAX_STRING_LENGTH];
   BUFFER *buffer;
   int total_sales, total_profit;

   if (ch->house != HOUSE_OUTLAW && !IS_IMMORTAL(ch))
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }

   send_to_char("\n\r", ch);
   buffer = new_buf();
   total_sales = 0; total_profit = 0;
   for (cnt = 0; cnt < 2000; cnt++)
   {
      if (black_market_data[cnt][0] == 0) continue;
      obj = get_obj_index(black_market_data[cnt][0]);
      if (obj == NULL) continue;
      sprintf(buf, "%-20s has sold for %d gold average over %d deals\n\r",
      obj->short_descr,
      black_market_data[cnt][1], black_market_data[cnt][2]);
      buf[0] = UPPER(buf[0]);
      add_buf(buffer, buf);
      total_sales += black_market_data[cnt][2];
      total_profit += black_market_data[cnt][1] * black_market_data[cnt][2];
   }
   sprintf(buf, "\n\r%d total sales totaling at least %d gold.\n\r",
   total_sales, total_profit);
   add_buf(buffer, buf);
   page_to_char(buf_string(buffer), ch);
   free_buf(buffer);
   return;
}


void do_ransack( CHAR_DATA *ch, char *argument )
{
   char buf  [MAX_INPUT_LENGTH];
   char arg1 [MAX_INPUT_LENGTH];
   char arg2 [MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   OBJ_DATA *obj;
   AFFECT_DATA af;
   int percent;
   int chance;
   int blind_flag = FALSE;
   int number;
   int count;

   if (get_skill(ch, gsn_ransack) <= 0 ||
   !has_skill(ch, gsn_ransack)) {
      send_to_char("Huh?\n\r", ch);
      return;
   }

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );

   if ( arg1[0] == '\0' || arg2[0] == '\0' )
   {
      send_to_char( "Ransack whom for what?\n\r", ch );
      return;
   }

   if ( ( victim = get_char_room( ch, arg2 ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if ( victim == ch )
   {
      send_to_char( "You ransack yourself and get what you desire!\n\r", ch );
      return;
   }

   if (check_peace(ch)) return;

   if (is_safe(ch, victim, 0))
   return;

   if (victim->fighting != NULL)
   {
      send_to_char("They are moving around too much right now.\n\r", ch);
      return;
   }

   if (oblivion_blink(ch, victim))
   {
      return;
   }

   if ( is_affected(victim, gsn_ransack) )
   {
      send_to_char("They are guarding their possessions too well right now.\n\r", ch);
      return;
   }

   un_camouflage(ch, NULL);
   un_hide(ch, NULL);
   un_earthfade(ch, NULL);
   un_forest_blend(ch); /* - Wicket */

   if (!IS_NPC(ch) && !IS_IMMORTAL(ch) && !IS_IMMORTAL(victim))
   {
      if (!IS_NPC(victim) || IS_AFFECTED(victim, AFF_CHARM))
      {
         QUIT_STATE(ch, 20);
      }
   }
   WAIT_STATE( ch, skill_table[gsn_ransack].beats );
   chance = get_skill(ch, gsn_ransack)/4;
   if (!IS_AWAKE(victim))
   chance *= 2;
   percent  = number_percent();
   chance -= ((victim->level - ch->level)*2);

   if (IS_NPC(victim) && IS_SET(victim->act, ACT_IS_CHANGER) )
   chance = 0;

   number = number_argument(arg1, arg1);
   count  = 0;
   for (obj = victim->carrying; obj != NULL; obj = obj->next_content)
   {
      if
      (
         obj->wear_loc != WEAR_NONE &&
         can_see_obj(ch, obj) &&
         (
            arg1[0] == '\0' ||
            is_name(arg1, obj->name)
         ) &&
         ++count == number
      )
      {
         break;
      }
   }

   if (obj == NULL)
   {
      send_to_char("They don't seem to have that.\n\r", ch);
      return;
   }

   if (!IS_SET(obj->wear_flags, ITEM_WEAR_FINGER) &&
   !IS_SET(obj->wear_flags, ITEM_WEAR_NECK) &&
   !IS_SET(obj->wear_flags, ITEM_WEAR_WRIST) &&
   !IS_SET(obj->wear_flags, ITEM_HOLD))
   {
      send_to_char("That is too hard to remove from them to take in a ransack.\n\r", ch);
      return;
   }

   if
   (
      obj->item_type == ITEM_BOAT &&
      victim->in_room->sector_type == SECT_WATER_NOSWIM &&
      !IS_FLYING(victim)
   )
   {
      send_to_char("You cannot steal a boat while it is being used!\n\r", ch);
      return;
   }


   af.where = TO_AFFECTS;
   af.type = gsn_ransack;
   af.level = ch->level;
   af.duration = 8;
   af.location  = APPLY_NONE;
   af.modifier  = 0;
   af.bitvector = 0;
   affect_to_char(victim, &af);

   if (IS_SET(obj->extra_flags, ITEM_NODROP) ||
   IS_SET(obj->extra_flags, ITEM_NOREMOVE))
   {
      send_to_char("You can't pry it away!\n\r", ch);
      act( "$n ransacks you looking for something, but seems disappointed.", ch, NULL, victim, TO_VICT    );
      act( "$n ransacks $N, but doesnt seem to find what $e is looking for.",  ch, NULL, victim, TO_NOTVICT );
      sprintf(buf, "Help! %s is ransacking me!", PERS(ch, victim));
      if (IS_AWAKE(victim) && !IS_NPC(victim))
      {
         do_myell( victim, buf );
         sprintf(log_buf, "[%s] tried to ransack [%s] at %d", ch->name, victim->name,
         ch->in_room->vnum);
         log_string(log_buf);
      }
      damage(ch, victim, number_range(3, 5), gsn_ransack, DAM_PIERCE, TRUE);
      multi_hit( victim, ch, TYPE_UNDEFINED );
      return;
   }

   if ( percent > chance)
   {
      /*
         Failure.
      */
      send_to_char( "You ransack them and search for what you seek, but can't seem to get it!\n\r", ch );
      affect_strip(ch, gsn_sneak);
      REMOVE_BIT(ch->affected_by, AFF_SNEAK);

      check_improve(ch, gsn_ransack, FALSE, 2);
      act( "$n ransacks you looking for something, but seems disappointed.", ch, NULL, victim, TO_VICT    );
      act( "$n ransacks $N, but doesnt seem to find what $e is looking for.",  ch, NULL, victim, TO_NOTVICT );
      sprintf(buf, "Help! %s is ransacking me!", PERS(ch, victim));
      if (IS_AWAKE(victim) && !IS_NPC(victim))
      {
         do_myell( victim, buf );
         sprintf(log_buf, "[%s] tried to ransack [%s] at %d", ch->name, victim->name,
         ch->in_room->vnum);
         log_string(log_buf);
      }
      damage(ch, victim, number_range(3, 5), gsn_ransack, DAM_PIERCE, TRUE);
      multi_hit( victim, ch, TYPE_UNDEFINED );
      return;
   }

   if (!IS_NPC(victim))
   {
      sprintf(log_buf, "[%s] tried to ransack [%s] at %d", ch->name,  victim->name, ch->in_room->vnum);
      log_string(log_buf);
   }

   if ( ch->carry_number + get_obj_number( obj ) > can_carry_n( ch ) )
   {
      send_to_char( "Your hands are full!\n\r", ch );
      return;
   }

   if ( ch->carry_weight + get_obj_weight( obj ) > can_carry_w( ch ) )
   {
      send_to_char( "You can't carry that much weight!\n\r", ch );
      return;
   }
   /* remove blindness so they can remove the item then put it back
   */
   if (IS_AFFECTED(victim, AFF_BLIND))
   {
      REMOVE_BIT(victim->affected_by, AFF_BLIND);
      blind_flag = TRUE;
   }
   if (!remove_obj( victim, obj->wear_loc, TRUE, TRUE ))
   {
      if (blind_flag) SET_BIT(victim->affected_by, AFF_BLIND);
      send_to_char( "You ransack them, but can't pry it from their hands.\n\r", ch );
      act( "$n cannot seem to yank something off you.", ch, NULL, victim, TO_VICT );
      return;
   }
   if (blind_flag) SET_BIT(victim->affected_by, AFF_BLIND);
   reslot_weapon(victim);
   send_to_char( "You ransack them and find what your looking for.  Success!!!\n\r", ch );
   act( "$n attacks you and ransacks through your possessions.", ch, NULL, victim, TO_VICT    );
   act( "$n attacks $N and seems to be ransacking $M searching for things to steal.", ch, NULL, victim, TO_NOTVICT    );
   obj_from_char( obj );
   obj_to_char( obj, ch );
   check_improve(ch, gsn_ransack, TRUE, 2);
   sprintf(buf, "Help! %s is ransacking me!", PERS(ch, victim));
   if (IS_AWAKE(victim) && !IS_NPC(victim))
   {
      do_myell( victim, buf );
      sprintf(log_buf, "[%s] tried to ransack [%s] at %d", ch->name, victim->name,
      ch->in_room->vnum);
      log_string(log_buf);
   }
   damage(ch, victim, number_range(3, 5), gsn_ransack, DAM_PIERCE, TRUE);
   multi_hit( victim, ch, TYPE_UNDEFINED );
   return;
}

void do_marriage (CHAR_DATA* ch, char* arg)
{
   EXTRA_DESCR_DATA* ed;
   OBJ_DATA* brand;
   OBJ_DATA* ring;  /* rings of wizardry, and OBJ_ENERGY_AMULET */
   OBJ_DATA* ring1;
   OBJ_DATA* ring2;
   CHAR_DATA* ch1;
   CHAR_DATA* ch2;
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   char buf[MAX_INPUT_LENGTH];
   bool switched = 0;
   bool is_any_good = 0;
   bool is_any_evil = 0;
   int mana_needed = 500;
   sh_int pos_temp = 0;

   /* Mobs cannot do marriages unless they are switched Immortals */
   if
   (
      IS_NPC(ch) &&
      (ch->desc != NULL) &&
      (ch->desc->original != NULL)
   )
   {
      switched = TRUE;
   }
   else if (IS_NPC(ch))
   {
      return;
   }
   if
   (
      ch->class != CLASS_CLERIC &&
      !IS_IMMORTAL(ch) &&
      !switched
   )
   {
      act_new
      (
         "You cannot perform marriages.",
         ch,
         NULL,
         NULL,
         TO_CHAR,
         POS_STANDING
      );
      return;
   }
   if
   (
      (brand = get_eq_char(ch, WEAR_BRAND)) &&
      (brand->pIndexData->vnum == 433)
   )
   {
      send_to_char("The gods have forsaken you.\n\r", ch);
      return;
   }
   if
   (
      ch->level < DO_MARRIAGE_MINIMUM_LEVEL &&
      !switched
   )
   {
      act_new
      (
         "You are too young to perform a marriage.",
         ch,
         NULL,
         NULL,
         TO_CHAR,
         POS_STANDING
      );
      return;
   }
   arg = one_argument(arg, arg1);
   one_argument(arg, arg2);
   if (!(ch1 = get_char_room(ch, arg1)))
   {
      act_new("$t is not here.", ch, arg1, NULL, TO_CHAR, POS_STANDING);
      return;
   }
   if (!(ch2 = get_char_room(ch, arg2)))
   {
      act_new("$t is not here.", ch, arg2, NULL, TO_CHAR, POS_STANDING);
      return;
   }
   if ((ch == ch1) || (ch == ch2))
   {
      act_new
      (
         "You cannot marry yourself.",
         ch,
         NULL,
         NULL,
         TO_CHAR,
         POS_STANDING
      );
      return;
   }
   if (ch1 == ch2)
   {
      act_new
      (
         "You cannot marry $N to $Mself.",
         ch,
         NULL,
         ch1,
         TO_CHAR,
         POS_STANDING
      );
      return;
   }
   /*
      Immortals (whether switched or not)
      skip most checks. If they're performing
      the marriage we can trust they want to
      be married, and we can trust that the gods
      smile on the union (a god is doing it)
   */
   if
   (
      !switched &&
      !IS_IMMORTAL(ch)
   )
   {
      is_any_evil = IS_EVIL(ch) || IS_EVIL(ch1) || IS_EVIL(ch2);
      is_any_good = IS_GOOD(ch) || IS_GOOD(ch1) || IS_GOOD(ch2);
      /*
         Gods don't smile upon unions of NPC's
         if at least one of the three (cleric/ch1/ch2)
         is good, and at least one is evil,
         the gods will not smile upon it either
         uncomment the sex checks to dissalow same sex
         marriages
      */
      if
      (
         IS_NPC(ch1) ||
         IS_NPC(ch2) ||
         (is_any_good && is_any_evil)  /* ||

         (
         ch1->pcdata->true_sex == ch2->pcdata->true_sex &&
         ch1->pcdata->true_sex != SEX_NEUTRAL
         )
         */
      )
      {
         act_new
         (
            "The gods do not smile upon this union.",
            ch,
            NULL,
            NULL,
            TO_CHAR,
            POS_STANDING
         );
         return;
      }
      /*
         This prevents 'ordered' marriages
      */
      if
      (
         IS_AFFECTED(ch, AFF_CHARM) ||
         IS_AFFECTED(ch1, AFF_CHARM) ||
         IS_AFFECTED(ch2, AFF_CHARM)
      )
      {
         act_new
         (
            "The gods do not smile upon forced marriages.",
            ch,
            NULL,
            NULL,
            TO_CHAR,
            POS_STANDING
         );
         return;
      }
      /*
         next if block
         forces the two newlyweds to be grouped
         (how we check if they want to be)
         independant of the full group check
         (this is how newlyweds get the emote)
         emotes to the newlyweds that
         the cleric wants to do a wedding ceremony,
         and if they join a group its considered acceptance
      */
      if (!is_same_group(ch1, ch2))
      {
         act_new
         (
            "$t is ready to marry you to $N.\n\r"
            "Join a group with $N to signal acceptance.",
            ch1,
            get_descr_form(ch, NULL, FALSE),
            ch2,
            TO_CHAR,
            POS_STANDING
         );
         act_new
         (
            "$t is ready to marry you to $N.\n\r"
            "Join a group with $N to signal acceptance.",
            ch2,
            get_descr_form(ch, NULL, FALSE),
            ch1,
            TO_CHAR,
            POS_STANDING
         );
         act_new
         (
            "The newlyweds must join in a group to symbolize acceptance.",
            ch,
            NULL,
            NULL,
            TO_CHAR,
            POS_STANDING
         );
         return;
      }
      /*
         From ACT_COMM.C (bool is_same_group)
         It is very important that this be an equivalence relation:
         (1) A ~ A
         (2) if A ~ B then B ~ A
         (3) if A ~ B  and B ~ C, then A ~ C
         The two checks I do should make certain
         All three are in a group together
      */
      /*
         next if block
         forces cleric to be in a group
         with the newlyweds
         independant of the first group check
      */
      if
      (
         !is_same_group(ch, ch1) ||
         !is_same_group(ch, ch2)
      )
      {
         act_new
         (
            "You must lead a group with the newlyweds to be.",
            ch,
            NULL,
            NULL,
            TO_CHAR,
            POS_STANDING
         );
         return;
      }
   }
   /*
      No marriage while cloaking (messes up ->owner bit)
   */
   if
   (
      is_affected(ch1, gsn_cloak_form) ||
      is_affected(ch2, gsn_cloak_form)
   )
   {
      act_new
      (
         "The newlyweds to be must shed their cloaks.",
         ch,
         NULL,
         NULL,
         TO_CHAR,
         POS_STANDING
      );
      return;
   }
   /*
      No marriage while cloaking (might mess up ->owner bit)
   */
   if
   (
      (ch1->morph_form[0] != 0) ||
      (ch2->morph_form[0] != 0)
   )
   {
      act_new
      (
         "The newlyweds should return to their natural form.",
         ch,
         NULL,
         NULL,
         TO_CHAR,
         POS_STANDING
      );
      return;
   }
   /*
      bride and groom must
      be standing to be married
   */
   if (ch1->position != POS_STANDING)
   {
      act_new
      (
         "$N really should be standing for this.",
         ch,
         NULL,
         ch1,
         TO_CHAR,
         POS_STANDING
      );
      return;
   }
   if (ch2->position != POS_STANDING)
   {
      act_new
      (
         "$N really should be standing for this.",
         ch,
         NULL,
         ch2,
         TO_CHAR,
         POS_STANDING
      );
      return;
   }
   /*
      Imortals don't have to be seen by the people
      they're marrying.
      Mortal clerics have to be visible to those
      that they're marrying
   */
   if
   (
      (
         !switched &&
         !IS_IMMORTAL(ch)
      ) &&
      (
         !can_see(ch1, ch) ||
         !can_see(ch2, ch)
      )
   )
   {
      act_new
      (
         "The newlyweds cannot see you.",
         ch,
         NULL,
         NULL,
         TO_CHAR,
         POS_STANDING
      );
      return;
   }
   /*
      You must be able to see your fiance to marry
      him, her, it
   */
   if
   (
      !can_see(ch1, ch2) ||
      !can_see(ch2, ch1)
   )
   {
      act_new
      (
         "The newlyweds cannot see each other.",
         ch,
         NULL,
         NULL,
         TO_CHAR,
         POS_STANDING
      );
      return;
   }
   /*
      Only allow one marriage
      You can comment this out
      to allow multiple marriages
      ie sultan rp
      or allow someone with a dead
      spouse to marry again without
      losing their old ring
      the code to be commented out
      to allow multiple marriages,
      is inside long ***********'s
   */
   /**************************************************/
   if ((ring = get_obj_char_vnum(ch1, OBJ_VNUM_WEDDING_RING)))
   {
      act_new
      (
         "$N is already married.",
         ch,
         NULL,
         ch1,
         TO_CHAR,
         POS_STANDING
      );
      act_new
      (
         "You are already married.",
         ch1,
         NULL,
         NULL,
         TO_CHAR,
         POS_STANDING
      );
      return;
   }
   if ((ring = get_obj_char_vnum(ch2, OBJ_VNUM_WEDDING_RING)))
   {
      act_new
      (
         "$N is already married.",
         ch,
         NULL,
         ch2,
         TO_CHAR,
         POS_STANDING
      );
      act_new
      (
         "You are already married.",
         ch2,
         NULL,
         NULL,
         TO_CHAR,
         POS_STANDING
      );
      return;
   }
   /**************************************************/

   /*
      reduce mana for rings of wizardry
      and energy amulet
   */

   /*
      start mana reduction
   */

   ring = get_eq_char(ch, WEAR_FINGER_R);
   if
   (
      ring != NULL &&
      ring->pIndexData->vnum == OBJ_VNUM_WIZARDRY_1
   )
   {
      mana_needed -= 7 * mana_needed / 100;
   }
   else if
   (
      ring != NULL &&
      ring->pIndexData->vnum == OBJ_VNUM_WIZARDRY_2
   )
   {
      mana_needed -= 15 * mana_needed / 100;
   }

   ring = get_eq_char(ch, WEAR_FINGER_L);
   if
   (
      ring != NULL &&
      ring->pIndexData->vnum == OBJ_VNUM_WIZARDRY_1
   )
   {
      mana_needed -= 7 * mana_needed / 100;
   }
   else if
   (
      ring != NULL &&
      ring->pIndexData->vnum == OBJ_VNUM_WIZARDRY_2
   )
   {
      mana_needed -= 15 * mana_needed / 100;
   }

   ring = get_eq_char(ch, WEAR_NECK_1);
   if
   (
      ring != NULL &&
      ring->pIndexData->vnum == OBJ_VNUM_ENERGY_AMULET
   )
   {
      mana_needed -= mana_needed / 5;
   }

   ring = get_eq_char(ch, WEAR_NECK_2);
   if
   (
      ring != NULL &&
      ring->pIndexData->vnum == OBJ_VNUM_ENERGY_AMULET
   )
   {
      mana_needed -= mana_needed / 5;
   }
   /*
      end mana reduction
      **********************
   */

   /*
      The following check can't be done yet,
      it requires pc_data to have a new field
      engaged,
      as well as a way to set it (it will be
      a string)
   */
   /*
   if
   (
      !(
         ch1->pcdata->engaged &&
         ch2->pcdata->engaged &&
         !str_cmp(ch1->pcdata->engaged, ch2->name) &&
         !str_cmp(ch2->pcdata->engaged, ch1->name)
      )
   )
   {
      act_new
      (
         "They are not engaged to each other.",
         ch,
         NULL,
         NULL,
         TO_CHAR,
         POS_STANDING
      );
      return;
   }
   */


   if
   (
      ch->mana < mana_needed &&
      !switched &&
      !IS_IMMORTAL(ch)
   )
   {
      act_new
      (
         "Your mind is too tired to perform a marriage.",
         ch,
         NULL,
         NULL,
         TO_CHAR,
         POS_STANDING
      );
      return;
   }

   /*
      Finally allowed to perform marriage
   */
   sprintf(buf, "%s is performing a marriage. %s-%s", ch->name, ch1->name, ch2->name);
   log_string(buf);
   wiznet(buf, ch, NULL, WIZ_MARRY, 0, get_trust(ch));
   ch->mana -= mana_needed;
   /*
      create the rings
   */
   ring1 = create_object(get_obj_index(OBJ_VNUM_WEDDING_RING), 0);
   ring2 = create_object(get_obj_index(OBJ_VNUM_WEDDING_RING), 0);
   if ((ring1 == NULL) || (ring2 == NULL))  /* sanity check and bug check */
   {
      bug
      (
         "do_marriage: OBJ_VNUM_WEDDING_RING = %d cannot be loaded",
         OBJ_VNUM_WEDDING_RING
      );
      return;
   }
   /*
      Set ownership
   */
   free_string(ring1->owner);
   ring1->owner = str_dup(ch1->name);
   free_string(ring2->owner);
   ring2->owner = str_dup(ch2->name);

   /*
      give rings to chars
      Yes, this is right.  The husband has the wife's ring,
      and vice versa.
   */
   obj_to_char(ring1, ch2);
   obj_to_char(ring2, ch1);

   /*
      Set up keywords ring 1
      if you want the character's
      name to be part of the keywords,
      uncomment the parameter, and remove the
      ""
   */
   free_string(ring1->name);
   sprintf(buf, "gold golden band wedding ring %s", "" /* ch1->name */);
   ring1->name = str_dup(buf);
   /*
      Set up keywords ring 2
      if you want the character's
      name to be part of the keywords,
      uncomment the parameter, and remove the
      ""
   */
   free_string(ring2->name);
   sprintf(buf, "gold golden band wedding ring %s", "" /* ch2->name */);
   ring2->name = str_dup(buf);

   /*
      set up short description ie what you're wearing
      ring 1
   */
   free_string(ring1->short_descr);
   sprintf
   (
      buf,
      "a golden wedding band engraved with the name '%s'",
      IS_NPC(ch2) ? ch2->short_descr : ch2->name
   );
   ring1->short_descr = str_dup(buf);
   /*
      set up short description ie what you're wearing
      ring 2
   */
   free_string(ring2->short_descr);
   sprintf
   (
      buf,
      "a golden wedding band engraved with the name '%s'",
      IS_NPC(ch1) ? ch1->short_descr : ch1->name
   );
   ring2->short_descr = str_dup(buf);

   /*
      setup description (on the floor in a room)
      ring 1
   */
   free_string(ring1->description);
   sprintf
   (
      buf,
      "A golden wedding band with the name '%s' lies here.",
      IS_NPC(ch2) ? ch2->short_descr : ch2->name
   );
   ring1->description = str_dup(buf);
   /*
      setup description (on the floor in a room)
      ring 2
   */
   free_string(ring2->description);
   sprintf
   (
      buf,
      "A golden wedding band with the name '%s' lies here.",
      IS_NPC(ch1) ? ch1->short_descr : ch1->name
   );
   ring2->description = str_dup(buf);

   /*
      setup extra description if they do a look ring, look band
      etc
      ring 1
   */
   ed = new_extra_descr();
   /*
      keywords of extra desc
      ring 1
   */
   sprintf(buf, "gold golden band wedding ring");
   ed->keyword = str_dup(buf);
   /*
      actual 'extra' description (2 lines)
      still ring 1
   */
   sprintf
   (
      buf,
      "This ring belongs to %s, married to %s.\n\r"
      "Please return it to %s or %s.\n\r",
      IS_NPC(ch1) ? ch1->short_descr : ch1->name,
      IS_NPC(ch2) ? ch2->short_descr : ch2->name,
      IS_NPC(ch1) ? ch1->short_descr : ch1->name,
      IS_NPC(ch2) ? ch2->short_descr : ch2->name
   );
   ed->description = str_dup(buf);
   /*
      add the extra desc to the object (ring 1)
   */
   ed->next = ring1->extra_descr;
   ring1->extra_descr = ed;

   /*
      setup extra description if they do a look ring, look band
      etc
      ring 2
   */
   ed = new_extra_descr();
   /*
      keywords of extra desc
      ring 2
   */
   sprintf(buf, "gold golden band wedding ring");
   ed->keyword = str_dup(buf);
   /*
      actual 'extra' description (2 lines)
      still ring 2
   */
   sprintf
   (
      buf,
      "This ring belongs to %s, married to %s.\n\r"
      "Please return it to %s or %s.\n\r",
      IS_NPC(ch2) ? ch2->short_descr : ch2->name,
      IS_NPC(ch1) ? ch1->short_descr : ch1->name,
      IS_NPC(ch2) ? ch2->short_descr : ch2->name,
      IS_NPC(ch1) ? ch1->short_descr : ch1->name
   );
   ed->description = str_dup(buf);
   /*
      add the extra desc to the object (ring 2)
   */
   ed->next = ring2->extra_descr;
   ring2->extra_descr = ed;

   /*
      emote for everyone but the newlyweds
   */
   pos_temp = ch->position;
   ch->position = POS_STUNNED;
   /* Most efficient way to skip ch for this emote */
   act_new
   (
      "$t completes the marriage ceremony and has been blessed with two\n\r"
      "wedding rings for the newlyweds.",
      ch1,
      get_descr_form(ch, NULL, FALSE),
      ch2,
      TO_NOTVICT,
      POS_RESTING
   );
   /* restore position */
   ch->position = pos_temp;
   send_to_char
   (
      "You complete the marriage ceremony and have been blessed with two\n\r"
      "wedding rings for the newlyweds.\n\r",
      ch
   );
   /*
      emote for ch1
   */
   act_new
   (
      "Your union has been blessed by the gods who have granted two bands\n\r"
      "of gold to symbolize your love.",
      ch1,
      NULL,
      ch2,
      TO_CHAR,
      POS_RESTING
   );
   /*
      emote for ch2
   */
   act_new
   (
      "Your union has been blessed by the gods who have granted two bands\n\r"
      "of gold to symbolize your love.",
      ch1,
      NULL,
      ch2,
      TO_VICT,
      POS_RESTING
   );
   act_new
   (
      "You have been given $N's ring.",
      ch1,
      NULL,
      ch2,
      TO_CHAR,
      POS_RESTING
   );
   act_new
   (
      "You have been given $N's ring.",
      ch2,
      NULL,
      ch1,
      TO_CHAR,
      POS_RESTING
   );
   /*
      force a save on ch1 and ch2
      so worst case (crash/void before autosave)
      their partner has the ring
      Uses internal save so they dont see it/
   */
   save_char_obj(ch1);
   save_char_obj(ch2);
}

/*------------------------------------------------------------------------
D O _ S C R I B E  (BEGIN)
------------------------------------------------------------------------*/

/* Chance to destroy scroll, if you don't have 'scrolls' skill */

#define DO_SCRIBE_NO_SCROLL_DESTROY 15

/* Minimum skill needed in a spell to scribe it */

#define DO_SCRIBE_MINIMUM_SPELL_SKILL 75

/* Minimum level needed to scribe four spells onto a scroll */

#define DO_SCRIBE_MINIMUM_LEVEL_FOUR 50

/* Minimum level needed to scribe three spells onto a scroll */

#define DO_SCRIBE_MINIMUM_LEVEL_THREE 40

/* Minimum level needed to scribe two spells onto a scroll */

#define DO_SCRIBE_MINIMUM_LEVEL_TWO 30

/*
   Mana base of scribe. Scribe will use this, plus the multiplier
   times the mana cost of the spell being scribed
   to decide how much mana is used by this skill
*/
/*
   using the skill_table entry for this
   #define DO_SCRIBE_MANA_BASE 30
*/
/*
   Multiplier of spell mana. Scribe will use this, time the mana
   cost of the spell being scribed, plus mana base
   (plus mana to create the scroll if its a new one)
   to decide how much mana is used by this skill
*/

#define DO_SCRIBE_MANA_SPELL_MULTIPLIER 2

/*
   Failure rate is increased by this percent if ch doesn't have
   'scrolls' skill
*/

#define DO_SCRIBE_SCROLL_PENALTY 30

/* Percentage of failure if absolutely everything is 100% */

#define DO_SCRIBE_BASE_FAIL 1

/*
   Percentage of failure added for every skill thats not mastered
   (scrolls, the spell itself, and scribe)
   if all three are not mastered, the penalty applies 3 times
*/

#define DO_SCRIBE_MASTERY_PENALTY 2

/*
   Amount of extra mana it costs to make a
   brand new scroll (summoning it)
   Can set this to 0 of course.
*/

#define DO_SCRIBE_MANA_CREATE_SCROLL 5



void do_scribe(CHAR_DATA* ch, char* arg)
{
   OBJ_DATA* scroll = NULL;
   char newname [MAX_INPUT_LENGTH];
   char arg1    [MAX_INPUT_LENGTH];
   char arg2    [MAX_INPUT_LENGTH];
   sh_int scribe_chance = 0;
   sh_int spell_chance  = 0;
   sh_int scroll_chance = 0;
   sh_int counter       = 0;
   sh_int current_spell = 0;
   sh_int mana_used     = 0;
   sh_int fail          = 0;
   bool new_scroll      = FALSE;
   extern const sh_int* scribe_spell_exclusion_table [];

   if (IS_NPC(ch))  /* mobs can't scribe */
   {
      send_to_char("Mobs cannot scribe spells onto scrolls.\n\r", ch);
      return;
   }
   if
   (
      (
         ch->house != HOUSE_CONCLAVE &&
         !IS_IMMORTAL(ch)
      ) ||
      (scribe_chance = get_skill(ch, gsn_scribe)) <= 0 ||
      !has_skill(ch, gsn_scribe)
   )
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }
   if (house_down(ch, HOUSE_CONCLAVE))
   {
      return;
   }
   arg = one_argument(arg, arg1);
   if
   (
      arg[0] == '\'' ||
      arg[0] == '\"'
   )
   {
      one_argument(arg, arg2);
   }
   else
   {
      /* allow scribe new 'magic missile' as well as scribe new magic missile */
      strcpy(arg2, arg);
   }
   if (arg1[0] == '\0')
   {
      send_to_char("Scribe upon what scroll?\n\r", ch);
      return;
   }
   if (str_cmp(arg1, "new"))  /* not new, add another spell to the scroll */
   {
      if ((scroll = get_obj_carry(ch, arg1, ch)) == NULL)
      {
         send_to_char("You do not have that scroll.\n\r", ch);
         return;
      }
      if (scroll->pIndexData->vnum != OBJ_VNUM_CONCLAVE_SCROLL)
      {
         act
         (
            "You cannot scribe any spells onto $p.",
            ch,
            scroll,
            NULL,
            TO_CHAR
         );
         /* Not a Conclave scroll, can't add to it */
         return;
      }
      if (scroll->value[4] != -1)
      {
         act
         (
            "$p has no more room on it to scribe additional spells.",
            ch,
            scroll,
            NULL,
            TO_CHAR
         );
         /* already 4 spells on the scroll, thats the max */
         return;
      }
      if
      (
         (
            (
               ch->level < DO_SCRIBE_MINIMUM_LEVEL_FOUR ||
               (
                  !IS_IMMORTAL(ch) &&
                  ch->pcdata->induct != 5
               )
            ) &&
            scroll->value[3] != -1
         ) ||
         (
            (
               ch->level < DO_SCRIBE_MINIMUM_LEVEL_THREE ||
               (
                  !IS_IMMORTAL(ch) &&
                  ch->pcdata->induct != 5 &&
                  ch->pcdata->induct != 3
               )
            ) &&
            scroll->value[2] != -1
         ) ||
         (
            ch->level < DO_SCRIBE_MINIMUM_LEVEL_TWO &&
            scroll->value[1] != -1
         )
      )
      {
         act
         (
            "You are not skilled enough to scribe more on $p.",
            ch,
            scroll,
            NULL,
            TO_CHAR
         );
         /* already max spells on the scroll for your level */
         return;
      }
      if (scroll->level > ch->level)
      {
         act
         (
            "You are not powerful enough to scribe onto $p.",
            ch,
            scroll,
            NULL,
            TO_CHAR
         );
         /* Can't scribe onto a scroll higher than your level */
         return;
      }
   }
   else  /* Create a new scroll */
   {
      new_scroll = TRUE;
   }
   if (arg2[0]=='\0')
   {
      send_to_char("Scribe what spell?\n\r", ch);
      return;
   }
   if ((current_spell = find_spell(ch, arg2)) == -1)
   {
      send_to_char("What spell was that?\n\r", ch);
      return;
   }
   if
   (
      current_spell >= MAX_SKILL ||
      current_spell < 1
   )
   {
      bug("Do_scribe: bad sn = %d", current_spell);
      send_to_char
      (
         "OOC: A bug has occurred and has been logged.\n\rPray OOC to the"
         " Immortals, telling them to check the logs for this bug.\n\r",
         ch
      );
      return;
   }
   if (skill_table[current_spell].spell_fun == spell_null)
   {
      send_to_char("You cannot scribe skills onto a scroll.\n\r", ch);
      return;
   }
   if
   (
      (spell_chance = get_skill(ch, current_spell)) < 1 ||
      !has_skill(ch, current_spell)
   )
   {
      send_to_char("You do not know any spell by that name.\n\r", ch);
      return;
   }
   /*
      if your skill in a spell is less than DO_SCRIBE_MINIMUM_SPELL_SKILL,
      you can't scribe it, defined at top of function
   */
   if
   (
      !IS_NPC(ch) &&
      ch->pcdata->learned[current_spell] < DO_SCRIBE_MINIMUM_SPELL_SKILL
   )
   {
      send_to_char
      (
         "You do not know that spell well enough to scribe it.\n\r",
         ch
      );
      return;
   }
   /*
      Check if current_spell is a spell thats allowed to be scribed onto
      a scroll by checking the exclusion table
      Immortals can scribe any spell
   */
   if (!IS_IMMORTAL(ch))
   {
      for
      (
         counter = 0;
         scribe_spell_exclusion_table[counter] != NULL;
         counter++
      )
      {
         if (current_spell == scribe_spell_exclusion_table[counter][0])
         {
            /*
               The current_spell is in the exclusion list, thus not allowed
               to scribe it
            */
            send_to_char("That spell cannot be scribed onto a scroll.\n\r", ch);
            return;
         }
      }
   }
   mana_used = mana_cost
   (
      ch,
      skill_table[current_spell].min_mana,
      current_spell
   );
   mana_used *= DO_SCRIBE_MANA_SPELL_MULTIPLIER;
   mana_used += skill_table[gsn_scribe].min_mana;
   if (new_scroll)  /* created a new scroll */
   {
      mana_used += DO_SCRIBE_MANA_CREATE_SCROLL;
   }
   /*
      mana_used is now equal to the mana base, plus mana usage of that spell times
      the multipler
   */
   if (ch->mana < mana_used)
   {
      send_to_char("You do not have the mana.\n\r", ch);
      return;
   }
   /*
      At this point, the spell CAN be scribed by ch, and is allowed to be
      scribed, and we have enough mana
   */

   if (new_scroll)
   {
      if
      (
         (
            scroll = create_object
            (
               get_obj_index(OBJ_VNUM_CONCLAVE_SCROLL),
               0
            )
         ) == NULL
      )
      {
         bug( "Do_scribe: obj_vnum_conclave_scroll cannot be loaded", 0 );
         send_to_char
         (
            "OOC: A bug has occurred and has been logged.\n\rPray OOC to the"
            " Immortals, telling them to check the logs for this bug.\n\r",
            ch
         );
         /*sanity check*/
         return;
      }
      if
      (
         get_carry_weight(ch) + get_obj_weight(scroll) > can_carry_w(ch) ||
         ch->carry_number + get_obj_number(scroll) > can_carry_n(ch)
      )
      {
         send_to_char("You cannot hold the scroll.\n\r", ch);
         extract_obj(scroll, FALSE);
         return;
      }
      obj_to_char(scroll, ch);
      scroll->level    = ch->level;
      scroll->value[0] = ch->level;  /* scroll level becomes your level */
      scroll->value[1] = current_spell;  /* Set Spell */
      scroll->value[2] = -1;  /* clear spell */
      scroll->value[3] = -1;  /* clear spell */
      scroll->value[4] = -1;  /* clear spell */
      /*
         Scrolls gain crumble timer equivalent to old ranger staffs or
         magic root wood.
         Not sure which yet, root wood = 150
         old = 20-30 randomly
      */
      /*
      scroll->timer = number_range(20, 30) * race_adjust(ch) / 25.0;
      */
      scroll->timer = 150;
   }

   /* only if we're actually going to do something will we make them lagged */
   WAIT_STATE(ch, skill_table[gsn_scribe].beats);

   /*
      If they don't have the scrolls skill yet, or at 0,
      DO_SCRIBE_NO_SCROLL_DESTROY % chance to destroy it
      (defined at beginning of function)
   */

   if
   (
      (scroll_chance = get_skill(ch, gsn_scrolls)) < 1 ||
      !has_skill(ch, gsn_scrolls)
   )
   {
      scroll_chance = 0;
      if (number_percent() < DO_SCRIBE_NO_SCROLL_DESTROY)
      {
         act("You pierce $n with your pen, destroying it.", ch, scroll, NULL, TO_CHAR);
         act("$n tears $s $p in half.", ch, scroll, NULL, TO_ROOM);
         extract_obj(scroll, FALSE);
         check_improve(ch, gsn_scribe, FALSE, 2);
         /*
            They broke the scroll without even doing anything, no mana loss
            They don't have the scrolls skill, so no improvement check
            on scrolls
         */
         return;
      }
   }
   if (scroll_chance == 0)
   {
      fail += DO_SCRIBE_SCROLL_PENALTY;
   }
   else if (scroll_chance < 100)
   {
      fail += DO_SCRIBE_MASTERY_PENALTY;
   }
   if (spell_chance < 100)
   {
      fail += DO_SCRIBE_MASTERY_PENALTY;
   }
   if (scribe_chance < 100)
   {
      fail += DO_SCRIBE_MASTERY_PENALTY;
   }
   if (IS_IMMORTAL(ch))
   {
      fail -= DO_SCRIBE_BASE_FAIL;
   }
   if (number_percent() > spell_chance)
   {
      send_to_char("You lost your concentration.\n\r", ch);
      check_improve(ch, current_spell, FALSE, 1);
      if (new_scroll)
      {
         extract_obj(scroll, FALSE);
      }
      ch->mana -= mana_used / 2;
      return;
   }
   else
   {
      send_to_char("You recite the spell in your mind.\n\r", ch);
      check_improve(ch, current_spell, TRUE, 1);
   }
   /* No more penalties if character doesn't have scrolls skill */
   if (scroll_chance != 0)
   {
      if (number_percent() > scroll_chance)
      {
         check_improve(ch, gsn_scrolls, FALSE, 1);
         ch->mana -= mana_used / 2;
         if (new_scroll)
         {
            send_to_char("You spill ink on the parchment.\n\r", ch);
            extract_obj(scroll, FALSE);
            return;
         }
         else
         {
            send_to_char("You could not find where to continue writing.\n\r", ch);
            return;
         }
      }
      else
      {
         check_improve(ch, gsn_scrolls, TRUE, 1);
      }
   }
   if (new_scroll)
   {
      send_to_char("You start writing on the scroll.\n\r", ch);
   }
   else
   {
      send_to_char("You start writing where the old script stops.\n\r", ch);
   }
   if (number_percent() > scribe_chance - fail - DO_SCRIBE_BASE_FAIL)
   {
      ch->mana -= mana_used / 2;
      if (!new_scroll)  /* Not blank scroll */
      {
         if (number_percent() > (fail + DO_SCRIBE_BASE_FAIL) / 2)
         {
            send_to_char
            (
               "Your penmanship is unreadable, but there is still room.\n\r",
               ch
            );
            check_improve(ch, gsn_scribe, FALSE, 2);
            return;
         }
         else
         {
            send_to_char
            (
               "Your penmanship is unreadable, making the scroll unusable.\n\r",
               ch
            );
            check_improve(ch, gsn_scribe, FALSE, 1);
            extract_obj(scroll, FALSE);
            return;
         }
      }
      else
      {
         send_to_char("You cannot read what you wrote.\n\r", ch);
         check_improve(ch, gsn_scribe, FALSE, 3);
         extract_obj(scroll, FALSE);
         return;
      }
   }
   ch->mana -= mana_used;
   /*COMPLETE SUCCESS!*/
   if (!new_scroll)
   {
      if (scroll->value[1] == -1)
      {
         scroll->value[1] = current_spell;
      }
      else if (scroll->value[2] == -1)
      {
         scroll->value[2] = current_spell;
      }
      else if (scroll->value[3] == -1)
      {
         scroll->value[3] = current_spell;
      }
      else
      {
         scroll->value[4] = current_spell;
      }
   }
   send_to_char("You scribe the spell upon the parchment.\n\r", ch);
   act
   (
      "$p glows as new words appear upon the page.",
      ch,
      scroll,
      NULL,
      TO_ALL
   );
   check_improve(ch, gsn_scribe, TRUE, 1);
   /*
      append the name of the spell to the keywords
      that this scroll can be referred to
      (ie get 'sanctuary power word kill' will get out
      a scroll that has sanctuary, and power word kill)
   */
   sprintf(newname, "%s %s", scroll->name, skill_table[current_spell].name);
   free_string(scroll->name);
   scroll->name = str_dup(newname);
   /*
      If character is evil, add evil flag
      if character is good, add bless flag
      They don't overwrite each other, they
      cancel each other.
      (so a third one can put it back on)
   */
   if (IS_EVIL(ch))
   {
      if (IS_SET(scroll->extra_flags, ITEM_BLESS))
      {
         send_to_char("The scroll dulls a bit.\n\r", ch);
         REMOVE_BIT(scroll->extra_flags, ITEM_BLESS);
      }
      else
      {
         send_to_char("The scroll glows red.\n\r", ch);
         SET_BIT(scroll->extra_flags, ITEM_EVIL);
      }
   }
   else if (IS_GOOD(ch))
   {
      if (IS_SET(scroll->extra_flags, ITEM_EVIL))
      {
         send_to_char("The scroll dulls a bit.\n\r", ch);
         REMOVE_BIT(scroll->extra_flags, ITEM_EVIL);
      }
      else
      {
         send_to_char("The scroll glows blue.\n\r", ch);
         SET_BIT(scroll->extra_flags, ITEM_BLESS);
      }
   }
   scroll->level = ch->level;
   /* Level of scroll goes up, level of spell does not */
   if
   (
      scroll->value[0] < ch->level &&
      !IS_SET(scroll->extra_flags, ITEM_HUM) &&
      number_percent() < ch->level
   )
   {
      send_to_char("The scroll begins to hum with power.\n\r", ch);
      SET_BIT(scroll->extra_flags, ITEM_HUM);
      scroll->value[0] = ch->level;
      /* Sometimes (only once) bring up level */
   }
   return;
}

#undef DO_SCRIBE_NO_SCROLL_DESTROY
#undef DO_SCRIBE_MINIMUM_SPELL_SKILL
#undef DO_SCRIBE_MINIMUM_LEVEL_FOUR
#undef DO_SCRIBE_MINIMUM_LEVEL_THREE
#undef DO_SCRIBE_MINIMUM_LEVEL_TWO
#undef DO_SCRIBE_MANA_SPELL_MULTIPLIER
#undef DO_SCRIBE_SCROLL_PENALTY
#undef DO_SCRIBE_BASE_FAIL
#undef DO_SCRIBE_MASTERY_PENALTY
#undef DO_SCRIBE_MANA_CREATE_SCROLL
/*------------------------------------------------------------------------
D O _ S C R I B E  (end)
------------------------------------------------------------------------*/

void stray_eat(CHAR_DATA* stray, OBJ_DATA* food, CHAR_DATA* giver)
{
   CHAR_DATA* ch = stray->master;
   bool poison = FALSE;
   bool had_poison = FALSE;
   int gain;
   AFFECT_DATA* paf;
   char buf[MAX_INPUT_LENGTH];

   if
   (
      giver->level < stray->level ||
      !IS_AWAKE(stray)
   )
   {
      return;
   }
   if
   (
      !(
         giver == ch &&
         is_affected(stray, gsn_create_food)
      ) &&
      (
         giver == ch ||
         ch == NULL ||
         stray->hit < stray->max_hit
      )
   )
   {
      act("$n sniffs $p.", stray, food, NULL, TO_ROOM);
      if (IS_AFFECTED(stray, AFF_POISON))
      {
         had_poison = TRUE;
         if
         (
            food->value[3] != 0 &&
            check_immune(stray, DAM_POISON) != IS_IMMUNE
         )
         {
            poison = TRUE;
         }
      }
      do_eat(stray, "1.");
      if
      (
         !had_poison &&
         IS_AFFECTED(stray, AFF_POISON)
      )
      {
         poison = TRUE;
      }
      if (poison)
      {
         act("$n hisses loudly at $N.", stray, NULL, giver, TO_NOTVICT);
         act("$n hisses loudly at you.", stray, NULL, giver, TO_VICT);
         if (ch == NULL)
         {
            sprintf(buf, "\"%s\"", get_name(giver, stray));
            do_bite(stray, buf);
            return;
         }
         else if (ch == giver)
         {
            act("$n walks off in search of food.", stray, NULL, NULL, TO_ROOM);
            stop_follower(stray);
         }
         return;
      }
   }
   else
   {
      act("$n sniffs $p.", stray, food, NULL, TO_ROOM);
      do_drop(stray, "1.");
      return;
   }
   paf = new_affect();
   paf->type = gsn_create_food;
   paf->duration = 2;
   gain = UMAX((stray->max_hit / 10), 5);
   if (ch == NULL)
   {
      act("$n rubs up against $N.", stray, NULL, giver, TO_NOTVICT);
      act("$n rubs up against you.", stray, NULL, giver, TO_VICT);
      SET_BIT(stray->affected_by, AFF_CHARM);
      add_follower(stray, giver);
      stray->leader = giver;
      if (giver->pet == NULL)
      {
         giver->pet = stray;
      }
      if
      (
         !is_affected(stray, gsn_create_food) &&
         stray->hit < stray->max_hit
      )
      {
         act("$n looks a little better.", stray, NULL, NULL, TO_ROOM);
         stray->hit = UMIN(stray->hit + gain, stray->max_hit);
         affect_to_char(stray, paf);
      }
   }
   else if
   (
      ch != giver &&
      !is_affected(stray, gsn_create_food) &&
      (
         (
            !IS_SET(stray->act2, ACT_NICE_STRAY) &&
            stray->hit < stray->max_hit * 9 / 10 &&
            number_bits(2) == 0  /* 25% */
         ) ||
         (
            ch->in_room != stray->in_room &&
            number_bits(4)  /* 15/16 chance to betray when you're not there */
         )
      )
   )
   {
      stop_follower(stray);
      act("$n rubs up against $N.", stray, NULL, giver, TO_NOTVICT);
      act("$n rubs up against you.", stray, NULL, giver, TO_VICT);
      SET_BIT(stray->affected_by, AFF_CHARM);
      add_follower(stray, giver);
      stray->leader = giver;
      if (giver->pet == NULL)
      {
         giver->pet = stray;
      }
      if
      (
         !is_affected(stray, gsn_create_food) &&
         stray->hit < stray->max_hit
      )
      {
         act("$n looks a little better.", stray, NULL, NULL, TO_ROOM);
         stray->hit = UMIN(stray->hit + gain, stray->max_hit);
         affect_to_char(stray, paf);
      }
   }
   else if (giver == ch)
   {
      act("$n rubs up against $N.", stray, NULL, giver, TO_NOTVICT);
      act("$n rubs up against you.", stray, NULL, giver, TO_VICT);
      if (giver->pet == NULL)
      {
         giver->pet = stray;
      }
      if
      (
         !is_affected(stray, gsn_create_food) &&
         stray->hit < stray->max_hit
      )
      {
         act("$n looks a little better.", stray, NULL, NULL, TO_ROOM);
         stray->hit = UMIN(stray->hit + gain, stray->max_hit);
      }
      affect_to_char(stray, paf);  /* always give effect */
   }
   free_affect(paf);
}

void stray_drink(CHAR_DATA* stray, OBJ_DATA* food, CHAR_DATA* giver)
{
   CHAR_DATA* ch = stray->master;
   bool poison = FALSE;
   bool had_poison = FALSE;
   int gain;
   AFFECT_DATA* paf;
   char buf[MAX_INPUT_LENGTH];

   if
   (
      giver->level < stray->level ||
      !IS_AWAKE(stray)
   )
   {
      return;
   }
   if
   (
      !(
         giver == ch &&
         is_affected(stray, gsn_create_water)
      ) &&
      (
         giver == ch ||
         ch == NULL ||
         stray->hit < stray->max_hit
      )
   )
   {
      act("$n sniffs $p.", stray, food, NULL, TO_ROOM);
      if (IS_AFFECTED(stray, AFF_POISON))
      {
         had_poison = TRUE;
         if
         (
            food->value[3] != 0 &&
            check_immune(stray, DAM_POISON) != IS_IMMUNE
         )
         {
            poison = TRUE;
         }
      }
      if (liq_table[food->value[2]].liq_affect[COND_THIRST] <= 0)
      {
         poison = TRUE;
      }
      do_drink(stray, food->name);
      if
      (
         !had_poison &&
         IS_AFFECTED(stray, AFF_POISON)
      )
      {
         poison = TRUE;
      }
      if (poison)
      {
         act("$n hisses loudly at $N.", stray, NULL, giver, TO_NOTVICT);
         act("$n hisses loudly at you.", stray, NULL, giver, TO_VICT);
         if (ch == NULL)
         {
            sprintf(buf, "\"%s\"", get_name(giver, stray));
            do_bite(stray, buf);
            return;
         }
         else if (ch == giver)
         {
            act("$n walks off in search of water.", stray, NULL, NULL, TO_ROOM);
            stop_follower(stray);
         }
         return;
      }
   }
   else
   {
      act("$n sniffs $p.", stray, food, NULL, TO_ROOM);
      do_drop(stray, "1.");
      return;
   }
   paf = new_affect();
   paf->type = gsn_create_water;
   paf->duration = 2;
   gain = UMAX((stray->max_hit / 10), 5);
   if (ch == NULL)
   {
      act("$n rubs up against $N.", stray, NULL, giver, TO_NOTVICT);
      act("$n rubs up against you.", stray, NULL, giver, TO_VICT);
      SET_BIT(stray->affected_by, AFF_CHARM);
      add_follower(stray, giver);
      stray->leader = giver;
      if (giver->pet == NULL)
      {
         giver->pet = stray;
      }
      if
      (
         !is_affected(stray, gsn_create_water) &&
         stray->hit < stray->max_hit
      )
      {
         act("$n looks a little better.", stray, NULL, NULL, TO_ROOM);
         stray->hit = UMIN(stray->hit + gain, stray->max_hit);
         affect_to_char(stray, paf);
      }
   }
   else if
   (
      ch != giver &&
      !is_affected(stray, gsn_create_water) &&
      (
         (
            !IS_SET(stray->act2, ACT_NICE_STRAY) &&
            stray->hit < stray->max_hit * 9 / 10 &&
            number_bits(2) == 0  /* 25% */
         ) ||
         (
            ch->in_room != stray->in_room &&
            number_bits(4)  /* 15/16 chance to betray when you're not there */
         )
      )
   )
   {
      stop_follower(stray);
      act("$n rubs up against $N.", stray, NULL, giver, TO_NOTVICT);
      act("$n rubs up against you.", stray, NULL, giver, TO_VICT);
      SET_BIT(stray->affected_by, AFF_CHARM);
      add_follower(stray, giver);
      stray->leader = giver;
      if (giver->pet == NULL)
      {
         giver->pet = stray;
      }
      if
      (
         !is_affected(stray, gsn_create_water) &&
         stray->hit < stray->max_hit
      )
      {
         act("$n looks a little better.", stray, NULL, NULL, TO_ROOM);
         stray->hit = UMIN(stray->hit + gain, stray->max_hit);
         affect_to_char(stray, paf);
      }
   }
   else if (giver == ch)
   {
      act("$n rubs up against $N.", stray, NULL, giver, TO_NOTVICT);
      act("$n rubs up against you.", stray, NULL, giver, TO_VICT);
      if (giver->pet == NULL)
      {
         giver->pet = stray;
      }
      if
      (
         !is_affected(stray, gsn_create_water) &&
         stray->hit < stray->max_hit
      )
      {
         act("$n looks a little better.", stray, NULL, NULL, TO_ROOM);
         stray->hit = UMIN(stray->hit + gain, stray->max_hit);
      }
      affect_to_char(stray, paf);  /* always give effect */
   }
   free_affect(paf);
}

bool is_branded_by_lestregus(CHAR_DATA *ch)
{
   OBJ_DATA *brand;

   brand = get_eq_char(ch, WEAR_BRAND);

   if (
         brand != NULL &&
         brand->pIndexData->vnum == OBJ_VNUM_LESTREGUS_BRAND
      )
   {
      return TRUE;
   }

   return FALSE;
}

bool obj_can_change_align_restrictions(OBJ_DATA *obj)
{
   int obj_vnum;

   if (!obj)
   {
      return FALSE;
   }

   obj_vnum = obj->pIndexData->vnum;

   if (
         obj_vnum == 2904 ||
         obj_vnum == 2905 ||
         obj_vnum == 2906 ||
         obj_vnum == 2908 ||
         obj_vnum == 2912 ||
         obj_vnum == 2913 ||
         obj_vnum == 2920 ||
         obj_vnum == 2921 ||
         obj_vnum == 2922 ||
         obj_vnum == 2923 ||
         str_cmp(obj->owner, "none")
      )
   {
      return FALSE;
   }

   return TRUE;
}
