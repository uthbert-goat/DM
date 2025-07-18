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
 ***************************************************************************
 *  Based on MERC 2.2 MOBprograms by N'Atas-ha.                            *
 *  Written and adapted to ROM 2.4 by                                      *
 *          Markku Nylander (markku.nylander@uta.fi)                       *
 ***************************************************************************/

/***************************************************************************



 ***************************************************************************/

static const char rcsid[] = "$Id: mob_cmds.c,v 1.30 2004/10/04 19:33:18 fizzfaldt Exp $";



#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"
#include "mob_cmds.h"
#include "recycle.h"
#include "database.h"

DECLARE_DO_FUN(do_look);
DECLARE_DO_FUN(do_string);
extern ROOM_INDEX_DATA *find_location( CHAR_DATA *, char * );
void bust_a_prompt args((CHAR_DATA *ch));
char * convert_color args( ( char * strbuf, bool ISANSI ) );

/*
* Command table.
*/
const    struct    mob_cmd_type    mob_cmd_table    [] =
{
   {"asound",        do_mpasound},
   {"gecho",         do_mpgecho},
   {"zecho",         do_mpzecho},
   {"kill",          do_mpkill},
   {"assist",        do_mpassist},
   {"junk",          do_mpjunk},
   {"echo",          do_mpecho},
   {"echoaround",    do_mpechoaround},
   {"echoat",        do_mpechoat},
   {"mload",         do_mpmload},
   {"oload",         do_mpoload},
   {"purge",         do_mppurge},
   {"goto",          do_mpgoto},
   {"at",            do_mpat},
   {"transfer",      do_mptransfer},
   {"gtransfer",     do_mpgtransfer},
   {"otransfer",     do_mpotransfer},
   {"force",         do_mpforce},
   {"gforce",        do_mpgforce},
   {"vforce",        do_mpvforce},
   {"cast",          do_mpcast},
   {"skillcast",     do_mpskillcast},
   {"damage",        do_mpdamage},
   {"remember",      do_mpremember},
   {"forget",        do_mpforget},
   {"delay",         do_mpdelay},
   {"cancel",        do_mpcancel},
   {"call",          do_mpcall},
   {"flee",          do_mpflee},
   {"remove",        do_mpremove},
   {"wait",          do_mpwait},
   {"peace",         do_mppeace},
   {"modskill",      do_mpmod_skill},
   {"setquest",      do_mpsetquest},
   {"removequest",   do_mpremovequest},
   {"timestamp",     do_mptimestamp},
   {"rewardgold",    do_mpreward_gold},
   {"rewardxp",      do_mpreward_xp},
   {"literal",       interpret},
   {"string",        do_mpstring},
   {"",              NULL}
};

void do_mob( CHAR_DATA *ch, char *argument )
{
   /*
   * Security check!
   */
   if ( ch->desc != NULL && get_trust(ch) < MAX_LEVEL )
   return;
   mob_interpret( ch, argument );
}

/*
* Grants/REMOVES a skill to a mob
*/
void do_mpmod_skill(CHAR_DATA* ch, char* argument)
{
   char buf[MAX_STRING_LENGTH];
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   char arg3[MAX_INPUT_LENGTH];
   int sn;
   char* orig_argument = argument;
   LIST_DATA* list;
   NODE_DATA* node;
   SKILL_MOD* mod = NULL;
   int percent;

   if (!IS_NPC(ch))
   {
      /*
         For Implementors
         (Implementors can use MOB commands, but
         this one could cause corruption)
      */
      send_to_char("MOB SKILL cannot be used by PCs!\n\r", ch);
      return;
   }
   if
   (
      ch->hit <= 0 ||
      ch->position <= POS_STUNNED ||
      !ch->valid ||
      ch->ghost ||
      !ch->in_room
   )
   {
      /* Don't let dead mobs try it */
      return;
   }
   argument = one_argument(argument, arg1);
   argument = one_argument(argument, arg2);
   argument = one_argument(argument, arg3);

   if (!str_cmp(arg1, "all"))
   {
      sn = SKILL_MOD_ALL;
   }
   else if
   (
      (
         sn = skill_lookup(arg1)
      ) == -1 ||
      sn < 1 ||
      sn >= MAX_SKILL
   )
   {
      sprintf
      (
         buf,
         "Mob_skill: invalid skill [%s] from mob %d",
         arg1,
         IS_NPC(ch) ? ch->pIndexData->vnum : 0
      );
      bug(buf, 0);
      sprintf
      (
         buf,
         "Mob_skill: full line was: [%s]",
         orig_argument
      );
      bug(buf, 0);
      return;
   }
   /* sn by this point is valid (either SKILL_MOD_ALL, or a normal skill) */
   if (!str_cmp(arg2, "lose"))
   {
      if
      (
         (
            list = ch->skill_mods
         ) == NULL
      )
      {
         sprintf
         (
            buf,
            "Mob_skill: losing skill [%s] from mob %d who has NO mods!",
            arg1,
            IS_NPC(ch) ? ch->pIndexData->vnum : 0
         );
         bug(buf, 0);
         return;
      }
      if (arg3[0] != '\0')
      {
         sprintf
         (
            buf,
            "Mob_skill: losing skill [%s] from mob %d INVALID syntax",
            arg1,
            IS_NPC(ch) ? ch->pIndexData->vnum : 0
         );
         bug(buf, 0);
         sprintf
         (
            buf,
            "Mob_skill: Should just be MOB SKILL '%s' lose",
            arg1
         );
         bug(buf, 0);
         return;
      }
      for (node = list->first; node; node = node->next)
      {
         mod = (SKILL_MOD*)node->data;
         if (mod->sn == sn)
         {
            free_node(node);
            if (list->size < 1)
            {
               free_list(list);
               ch->skill_mods = NULL;
            }
            return;
         }
      }
      sprintf
      (
         buf,
         "Mob_skill: losing skill [%s] from mob %d but not affected by that skill_mod!",
         arg1,
         IS_NPC(ch) ? ch->pIndexData->vnum : 0
      );
      bug(buf, 0);
      return;
   }
   if (arg2[0] == '\0')
   {
      percent = 100;
   }
   else if (is_number(arg2))
   {
      percent = atoi(arg2);
      if
      (
         percent > 100 ||
         percent < 0
      )
      {
         sprintf
         (
            buf,
            "Mob_skill: gaining skill [%s] from mob %d, invalid percent: [%d]",
            arg1,
            IS_NPC(ch) ? ch->pIndexData->vnum : 0,
            percent
         );
         bug(buf, 0);
         return;
      }
   }
   else
   {
      sprintf
      (
         buf,
         "Mob_skill: gaining skill [%s] from mob %d INVALID syntax",
         arg1,
         IS_NPC(ch) ? ch->pIndexData->vnum : 0
      );
      bug(buf, 0);
      bug("Mob_skill: second parameter must be a percentage or blank", 0);
      return;
   }
   if (arg3[0] != '\0')
   {
      sprintf
      (
         buf,
         "Mob_skill: gaining skill [%s] from mob %d INVALID syntax",
         arg1,
         IS_NPC(ch) ? ch->pIndexData->vnum : 0
      );
      bug(buf, 0);
      sprintf
      (
         buf,
         "Mob_skill: Should just be MOB SKILL '%s' [skill percent or blank]",
         arg1
      );
      bug(buf, 0);
      return;
   }
   if (!ch->skill_mods)
   {
      ch->skill_mods = new_list();
   }
   list = ch->skill_mods;
   /* Remove old skill mod for this sn if it exists */
   for (node = list->first; node; node = node->next)
   {
      mod = (SKILL_MOD*)node->data;
      if (mod->sn == sn)
      {
         free_node(node);
         free_skill_mod(mod);
         break;
      }
   }
   mod = new_skill_mod(percent, sn);
   add_node(mod, list);
}

/*
* Mob command interpreter. Implemented separately for security and speed
* reasons. A trivial hack of interpret()
*/
void mob_interpret( CHAR_DATA *ch, char *argument )
{
   char buf[MAX_STRING_LENGTH], command[MAX_INPUT_LENGTH];
   int cmd;

   if (log_mobs)
   {
      sprintf
      (
         log_buf,
         "MOB_INTERPRET: [%d] [%s] : [%s]",
         (
            (
               ch &&
               IS_NPC(ch)
            ) ?
            ch->pIndexData->vnum :
            0
         ),
         (
            ch ?
            (
               IS_NPC(ch) ?
               ch->short_descr :
               ch->name
            ) :
            "<null>"
         ),
         argument
      );
      log_string(log_buf);
   }

   argument = one_argument( argument, command );

   if (!str_prefix("someone", argument))
   {
      /* Failed to see the person, will not have an affect anyway */
      return;
   }
   /*
   * Look for command in command table.
   */
   for ( cmd = 0; mob_cmd_table[cmd].name[0] != '\0'; cmd++ )
   {
      if ( command[0] == mob_cmd_table[cmd].name[0]
      &&   !str_prefix( command, mob_cmd_table[cmd].name ) )
      {
         (*mob_cmd_table[cmd].do_fun) ( ch, argument );
         tail_chain( );
         return;
      }
   }
   sprintf( buf, "Mob_interpret: invalid cmd from mob %d: '%s'",
   IS_NPC(ch) ? ch->pIndexData->vnum : 0, command );
   bug( buf, 0 );
}

char *mprog_type_to_name( int type )
{
   switch ( type )
   {
      case TRIG_ACT:                 return "ACT";
      case TRIG_SPEECH:              return "SPEECH";
      case TRIG_RANDOM:              return "RANDOM";
      case TRIG_FIGHT:               return "FIGHT";
      case TRIG_HPCNT:               return "HPCNT";
      case TRIG_DEATH:               return "DEATH";
      case TRIG_ENTRY:               return "ENTRY";
      case TRIG_GREET:               return "GREET";
      case TRIG_GRALL:            return "GRALL";
      case TRIG_GIVE:                return "GIVE";
      case TRIG_BRIBE:               return "BRIBE";
      case TRIG_KILL:              return "KILL";
      case TRIG_DELAY:               return "DELAY";
      case TRIG_SURR:              return "SURRENDER";
      case TRIG_EXIT:              return "EXIT";
      case TRIG_EXALL:              return "EXALL";
      default:                      return "ERROR";
   }
}

/*
* Displays MOBprogram triggers of a mobile
*
* Syntax: mpstat [name]
*/
void do_mpstat( CHAR_DATA *ch, char *argument )
{
   char         arg [MAX_STRING_LENGTH];
   MPROG_LIST  *mprg;
   CHAR_DATA   *victim;
   int i;
   char find[MAX_INPUT_LENGTH];

   /* NPC Only */
   find[0] = '\0';
   if (argument[0] != '-')
   {
      strcat(find, "-");
   }
   strcat(find, argument);
   if ( ( victim = get_char_world( ch, find ) ) == NULL )
   {
      send_to_char( "No such creature.\n\r", ch );
      return;
   }

   if ( !IS_NPC( victim ) )
   {
      send_to_char( "That is not a mobile.\n\r", ch);
      return;
   }

   sprintf( arg, "Mobile #%-6d [%s]\n\r",
   victim->pIndexData->vnum, victim->short_descr );
   send_to_char( arg, ch );

   sprintf( arg, "Delay   %-6d [%s]\n\r",
   victim->mprog_delay,
   victim->mprog_target == NULL
   ? "No target" : victim->mprog_target->name );
   send_to_char( arg, ch );

   if ( !victim->pIndexData->mprog_flags )
   {
      send_to_char( "[No programs set]\n\r", ch);
      return;
   }

   for ( i = 0, mprg = victim->pIndexData->mprogs; mprg != NULL;
   mprg = mprg->next )

   {
      sprintf( arg, "[%2d] Trigger [%-8s] Program [%4d] Phrase [%s]\n\r",
      ++i,
      mprog_type_to_name( mprg->trig_type ),
      mprg->vnum,
      mprg->trig_phrase );
      send_to_char( arg, ch );
   }

   return;

}

/*
* Displays the source code of a given MOBprogram
*
* Syntax: mpdump [vnum]
*/
void do_mpdump( CHAR_DATA *ch, char *argument )
{
   char buf[ MAX_INPUT_LENGTH ];
   MPROG_CODE *mprg;

   one_argument( argument, buf );
   if ( ( mprg = get_mprog_index( atoi(buf) ) ) == NULL )
   {
      send_to_char( "No such MOBprogram.\n\r", ch );
      return;
   }
   page_to_char( mprg->code, ch );
}

/*
* Prints the argument to all active players in the game
*
* Syntax: mob gecho [string]
*/
void do_mpgecho(CHAR_DATA* ch, char* argument)
{
   DESCRIPTOR_DATA *d;
   char* temp;

   if (argument[0] == '\0')
   {
      bug
      (
         "MpGEcho: missing argument from vnum %d",
         IS_NPC(ch) ?
         ch->pIndexData->vnum :
         0
      );
      return;
   }

   for (d = descriptor_list; d; d = d->next)
   {
      if (d->connected == CON_PLAYING)
      {
         if (IS_IMMORTAL(d->character))
         {
            send_to_char("Mob echo> ", d->character);
         }
         send_to_char
         (
            (temp = convert_color(argument, IS_ANSI(d->character))),
            d->character
         );
         free_string(temp);
         send_to_char( "\n\r", d->character);
      }
   }
}

/*
* Prints the argument to all players in the same area as the mob
*
* Syntax: mob zecho [string]
*/
void do_mpzecho(CHAR_DATA* ch, char* argument)
{
   DESCRIPTOR_DATA* d;
   char* temp;

   if (argument[0] == '\0')
   {
      bug
      (
         "MpZEcho: missing argument from vnum %d",
         IS_NPC(ch) ?
         ch->pIndexData->vnum :
         0
      );
      return;
   }

   if (ch->in_room == NULL)
   {
      return;
   }

   for (d = descriptor_list; d; d = d->next)
   {
      if
      (
         d->connected == CON_PLAYING &&
         d->character->in_room != NULL &&
         d->character->in_room->area == ch->in_room->area &&
         IS_AWAKE(d->character)
      )
      {
         if (IS_IMMORTAL(d->character))
         {
            send_to_char("Mob echo> ", d->character);
         }
         send_to_char
         (
            (temp = convert_color(argument, IS_ANSI(d->character))),
            d->character
         );
         free_string(temp);
         send_to_char("\n\r", d->character);
      }
   }
}

/*
* Prints the argument to all the rooms aroud the mobile
*
* Syntax: mob asound [string]
*/
void do_mpasound(CHAR_DATA* ch, char* argument)
{
   ROOM_INDEX_DATA* was_in_room;
   int              door;

   if (argument[0] == '\0')
   {
      return;
   }

   was_in_room = ch->in_room;
   for (door = 0; door < 6; door++)
   {
      EXIT_DATA* pexit;

      if
      (
         (pexit = was_in_room->exit[door]) != NULL &&
         pexit->u1.to_room != NULL &&
         pexit->u1.to_room != was_in_room
      )
      {
         char_from_room(ch);
         char_to_room_1(ch, pexit->u1.to_room, TO_ROOM_AT);
         MOBtrigger  = FALSE;
         act_color(argument, ch, NULL, NULL, TO_ROOM);
         MOBtrigger  = TRUE;
      }
   }
   char_from_room(ch);
   char_to_room_1(ch, was_in_room, TO_ROOM_AT);
   return;
}

/*
* Lets the mobile kill any player or mobile without murder
*
* Syntax: mob kill [victim]
*/
void do_mpkill( CHAR_DATA *ch, char *argument )
{
   char      arg[ MAX_INPUT_LENGTH ];
   CHAR_DATA *victim;

   one_argument( argument, arg );

   if ( arg[0] == '\0' )
   return;

   if ( ( victim = get_char_room( ch, arg ) ) == NULL )
   return;

   if ( victim == ch || IS_NPC(victim) || ch->position == POS_FIGHTING )
   return;

   if ( IS_AFFECTED( ch, AFF_CHARM ) && ch->master == victim )
   {
      bug( "MpKill - Charmed mob attacking master from vnum %d.",
      IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
      return;
   }

   multi_hit( ch, victim, TYPE_UNDEFINED );
   return;
}

/*
* Lets the mobile assist another mob or player
*
* Syntax: mob assist [character]
*/
void do_mpassist( CHAR_DATA *ch, char *argument )
{
   char      arg[ MAX_INPUT_LENGTH ];
   CHAR_DATA *victim;

   one_argument( argument, arg );

   if ( arg[0] == '\0' )
   return;

   if ( ( victim = get_char_room( ch, arg ) ) == NULL )
   return;

   if ( victim == ch || ch->fighting != NULL || victim->fighting == NULL )
   return;

   multi_hit( ch, victim->fighting, TYPE_UNDEFINED );
   return;
}


/*
* Lets the mobile destroy an object in its inventory
* it can also destroy a worn object and it can destroy
* items using all.xxxxx or just plain all of them
*
* Syntax: mob junk [item]
*/

void do_mpjunk( CHAR_DATA *ch, char *argument )
{
   char      arg[ MAX_INPUT_LENGTH ];
   OBJ_DATA *obj;
   OBJ_DATA *obj_next;

   one_argument( argument, arg );

   if ( arg[0] == '\0')
   return;

   if ( str_cmp( arg, "all" ) && str_prefix( "all.", arg ) )
   {
      if ( ( obj = get_obj_wear( ch, arg ) ) != NULL )
      {
         unequip_char( ch, obj );
         extract_obj( obj, FALSE );
         return;
      }
      if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
      return;
      extract_obj( obj, FALSE );
   }
   else
   for ( obj = ch->carrying; obj != NULL; obj = obj_next )
   {
      obj_next = obj->next_content;
      if ( arg[3] == '\0' || is_name( &arg[4], obj->name ) )
      {
         if ( obj->wear_loc != WEAR_NONE)
         unequip_char( ch, obj );
         extract_obj( obj, FALSE );
      }
   }

   return;

}

/*
* Prints the message to everyone in the room other than the mob and victim
*
* Syntax: mob echoaround [victim] [string]
*/

void do_mpechoaround(CHAR_DATA* ch, char* argument)
{
   char       arg[ MAX_INPUT_LENGTH ];
   CHAR_DATA* victim;

   argument = one_argument(argument, arg);

   if ( arg[0] == '\0' )
   {
      return;
   }

   if ((victim = get_char_room(ch, arg)) == NULL)
   {
      return;
   }

   act_color(argument, ch, NULL, victim, TO_NOTVICT);
}

/*
* Prints the message to only the victim
*
* Syntax: mob echoat [victim] [string]
*/
void do_mpechoat(CHAR_DATA* ch, char* argument)
{
   char       arg[ MAX_INPUT_LENGTH ];
   CHAR_DATA* victim;

   argument = one_argument(argument, arg);

   if ( arg[0] == '\0' || argument[0] == '\0' )
   {
      return;
   }

   if ((victim = get_char_room(ch, arg)) == NULL)
   {
      return;
   }

   act_color(argument, ch, NULL, victim, TO_VICT);
}

/*
* Prints the message to the room at large
*
* Syntax: mpecho [string]
*/
void do_mpecho(CHAR_DATA* ch, char* argument)
{
   if (argument[0] == '\0')
   {
      return;
   }
   act_color(argument, ch, NULL, NULL, TO_ROOM);
}

/*
* Lets the mobile load another mobile.
*
* Syntax: mob mload [vnum]
*/
void do_mpmload( CHAR_DATA *ch, char *argument )
{
   char            arg[ MAX_INPUT_LENGTH ];
   MOB_INDEX_DATA *pMobIndex;
   CHAR_DATA      *victim;
   int vnum;

   one_argument( argument, arg );

   if ( ch->in_room == NULL || arg[0] == '\0' || !is_number(arg) )
   return;

   vnum = atoi(arg);
   if ( ( pMobIndex = get_mob_index( vnum ) ) == NULL )
   {
      sprintf( arg, "Mpmload: bad mob index (%d) from mob %d",
      vnum, IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
      bug( arg, 0 );
      return;
   }
   victim = create_mobile( pMobIndex );
   char_to_room( victim, ch->in_room );
   return;
}

/*
* Lets the mobile load an object
*
* Syntax: mob oload [vnum] [level] {R}
*/
void do_mpoload( CHAR_DATA *ch, char *argument )
{
   char arg1[ MAX_INPUT_LENGTH ];
   char arg2[ MAX_INPUT_LENGTH ];
   char arg3[ MAX_INPUT_LENGTH ];
   OBJ_INDEX_DATA *pObjIndex;
   OBJ_DATA       *obj;
   int             level;
   bool            fToroom = FALSE, fWear = FALSE;

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );
   one_argument( argument, arg3 );

   if ( arg1[0] == '\0' || !is_number( arg1 ) )
   {
      bug( "Mpoload - Bad syntax from vnum %d.",
      IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
      return;
   }

   if ( arg2[0] == '\0' )
   {
      level = get_trust( ch );
   }
   else
   {
      /*
      * New feature from Alander.
      */
      if ( !is_number( arg2 ) )
      {
         bug( "Mpoload - Bad syntax from vnum %d.",
         IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
         return;
      }
      level = atoi( arg2 );
      if ( level < 0 || level > get_trust( ch ) )
      {
         bug( "Mpoload - Bad level from vnum %d.",
         IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
         return;
      }
   }

   /*
   * Added 3rd argument
   * omitted - load to mobile's inventory
   * 'R'     - load to room
   * 'W'     - load to mobile and force wear
   */
   if ( arg3[0] == 'R' || arg3[0] == 'r' )
   fToroom = TRUE;
   else if ( arg3[0] == 'W' || arg3[0] == 'w' )
   fWear = TRUE;

   if ( ( pObjIndex = get_obj_index( atoi( arg1 ) ) ) == NULL )
   {
      bug( "Mpoload - Bad vnum arg from vnum %d.",
      IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
      return;
   }

   /* Mobs should not load maxed, or overmaxed, items */
   if ( pObjIndex->limcount >= pObjIndex->limtotal && pObjIndex->limtotal > 0 )
   return;

   obj = create_object( pObjIndex, level );
   if ( (fWear || !fToroom) && CAN_WEAR(obj, ITEM_TAKE) )
   {
      obj_to_char( obj, ch );
      if ( fWear )
      wear_obj( ch, obj, TRUE );
   }
   else
   {
      obj_to_room( obj, ch->in_room );
   }

   return;
}

/*
* Lets the mobile purge all objects and other npcs in the room,
* or purge a specified object or mob in the room. The mobile cannot
* purge itself for safety reasons.
*
* syntax mob purge {target}
*/
void do_mppurge( CHAR_DATA *ch, char *argument )
{
   char       arg[ MAX_INPUT_LENGTH ];
   CHAR_DATA *victim;
   OBJ_DATA  *obj;

   one_argument( argument, arg );

   if ( arg[0] == '\0' )
   {
      /* 'purge' */
      CHAR_DATA *vnext;
      OBJ_DATA  *obj_next;

      for ( victim = ch->in_room->people; victim != NULL; victim = vnext )
      {
         vnext = victim->next_in_room;
         if ( IS_NPC( victim ) && victim != ch
         &&   !IS_SET(victim->act, ACT_NOPURGE) )
         extract_char( victim, TRUE );
      }

      for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
      {
         obj_next = obj->next_content;
         if ( !IS_SET(obj->extra_flags, ITEM_NOPURGE) )
         extract_obj( obj, FALSE );
      }

      return;
   }

   if ( ( victim = get_char_room( ch, arg ) ) == NULL )
   {
      if ( ( obj = get_obj_here( ch, arg ) ) )
      {
         extract_obj( obj, FALSE );
      }
      else
      {
         bug( "Mppurge - Bad argument from vnum %d.",
         IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
      }
      return;
   }

   if ( !IS_NPC( victim ) )
   {
      bug( "Mppurge - Purging a PC from vnum %d.",
      IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
      return;
   }
   extract_char( victim, TRUE );
   return;
}


/*
* Lets the mobile goto any location it wishes that is not private.
*
* Syntax: mob goto [location]
*/
void do_mpgoto( CHAR_DATA *ch, char *argument )
{
   char             arg[ MAX_INPUT_LENGTH ];
   ROOM_INDEX_DATA *location;

   one_argument( argument, arg );
   if ( arg[0] == '\0' )
   {
      bug( "Mpgoto - No argument from vnum %d.",
      IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
      return;
   }

   if ( ( location = find_location( ch, arg ) ) == NULL )
   {
      bug( "Mpgoto - No such location from vnum %d.",
      IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
      return;
   }

   if ( ch->fighting != NULL )
   stop_fighting( ch, TRUE );

   char_from_room( ch );
   char_to_room( ch, location );

   return;
}

/*
* Lets the mobile do a command at another location.
*
* Syntax: mob at [location] [commands]
*/
void do_mpat( CHAR_DATA *ch, char *argument )
{
   char             arg[ MAX_INPUT_LENGTH ];
   ROOM_INDEX_DATA *location;
   ROOM_INDEX_DATA *original;
   CHAR_DATA       *wch;
   OBJ_DATA         *on;

   argument = one_argument( argument, arg );

   if ( arg[0] == '\0' || argument[0] == '\0' )
   {
      bug( "Mpat - Bad argument from vnum %d.",
      IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
      return;
   }

   if ( ( location = find_location( ch, arg ) ) == NULL )
   {
      bug( "Mpat - No such location from vnum %d.",
      IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
      return;
   }

   original = ch->in_room;
   on = ch->on;
   char_from_room(ch);
   char_to_room_1(ch, location, TO_ROOM_AT);
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

/*
* Lets the mobile transfer people.  The 'all' argument transfers
*  everyone in the current room to the specified location
*
* Syntax: mob transfer [target|'all'] [location]
*/
void do_mptransfer( CHAR_DATA *ch, char *argument )
{
   char             arg1[ MAX_INPUT_LENGTH ];
   char             arg2[ MAX_INPUT_LENGTH ];
   char             arg3[ MAX_INPUT_LENGTH ];
   char         buf[MAX_STRING_LENGTH];
   ROOM_INDEX_DATA *location;
   CHAR_DATA       *victim;

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );
   argument = one_argument( argument, arg3 );

   if ( arg1[0] == '\0' )
   {
      bug( "Mptransfer - Bad syntax from vnum %d.",
      IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
      return;
   }

   if ( !str_cmp( arg1, "all" ) )
   {
      CHAR_DATA *victim_next;

      for ( victim = ch->in_room->people; victim != NULL; victim = victim_next )
      {
         victim_next = victim->next_in_room;
         if ( !IS_NPC(victim) )
         {
            sprintf( buf, "'%s' %s", victim->name, arg2 );
            do_mptransfer( ch, buf );
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
         bug( "Mptransfer - No such location from vnum %d.",
         IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
         return;
      }

      if ( room_is_private( location ) )
      return;
   }

   if (arg3[0] == '\0')
   {
      if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
      return;

      if ( victim->in_room == NULL )
      return;
   }
   else
   {
      while ((victim = get_char_room(ch, arg1)) != NULL)
      {
         if (!IS_NPC(victim))
         {
            if ( victim->fighting != NULL )
            stop_fighting( victim, TRUE );
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
            do_observe(victim, "", LOOK_AUTO);
         }
      }
      return;
   }
   if ( victim->fighting != NULL )
   stop_fighting( victim, TRUE );
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
   do_observe(victim, "", LOOK_AUTO);

   return;
}

/*
* Lets the mobile transfer all chars in same group as the victim.
*
* Syntax: mob gtransfer [victim] [location]
*/
void do_mpgtransfer( CHAR_DATA *ch, char *argument )
{
   char             arg1[ MAX_INPUT_LENGTH ];
   char             arg2[ MAX_INPUT_LENGTH ];
   char         buf[MAX_STRING_LENGTH];
   CHAR_DATA       *who, *victim, *victim_next;

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );

   if ( arg1[0] == '\0' )
   {
      bug( "Mpgtransfer - Bad syntax from vnum %d.",
      IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
      return;
   }

   if ( (who = get_char_room( ch, arg1 )) == NULL )
   return;
   for ( victim = ch->in_room->people; victim; victim = victim_next )
   {
      victim_next = victim->next_in_room;
      if ( is_same_group( who, victim ) )
      {
         sprintf
         (
            buf,
            "\"%c%s\" %s",
            IS_NPC(victim) ? '-' : '+',
            get_name(victim, ch),
            arg2
         );
         do_mptransfer( ch, buf );
      }
   }
   return;
}

/*
* Lets the mobile force someone to do something. Must be mortal level
* and the all argument only affects those in the room with the mobile.
*
* Syntax: mob force [victim] [commands]
*/
void do_mpforce( CHAR_DATA *ch, char *argument )
{
   char arg[ MAX_INPUT_LENGTH ];

   argument = one_argument( argument, arg );

   if ( arg[0] == '\0' || argument[0] == '\0' )
   {
      bug( "Mpforce - Bad syntax from vnum %d.",
      IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
      return;
   }

   if ( !str_cmp( arg, "all" ) )
   {
      CHAR_DATA *vch;
      CHAR_DATA *vch_next;

      for ( vch = char_list; vch != NULL; vch = vch_next )
      {
         vch_next = vch->next;

         if ( vch->in_room == ch->in_room
         && get_trust( vch ) < get_trust( ch )
         && can_see( ch, vch ) )
         {
            interpret( vch, argument );
         }
      }
   }
   else
   {
      CHAR_DATA *victim;

      if ( ( victim = get_char_room( ch, arg ) ) == NULL )
      return;

      if ( victim == ch )
      return;

      interpret( victim, argument );
   }

   return;
}

/*
* Lets the mobile force a group something. Must be mortal level.
*
* Syntax: mob gforce [victim] [commands]
*/
void do_mpgforce( CHAR_DATA *ch, char *argument )
{
   char arg[ MAX_INPUT_LENGTH ];
   CHAR_DATA *victim, *vch, *vch_next;

   argument = one_argument( argument, arg );

   if ( arg[0] == '\0' || argument[0] == '\0' )
   {
      bug( "MpGforce - Bad syntax from vnum %d.",
      IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
      return;
   }

   if ( ( victim = get_char_room( ch, arg ) ) == NULL )
   return;

   if ( victim == ch )
   return;

   for ( vch = victim->in_room->people; vch != NULL; vch = vch_next )
   {
      vch_next = vch->next_in_room;

      if ( is_same_group(victim, vch) )
      {
         interpret( vch, argument );
      }
   }
   return;
}

/*
* Forces all mobiles of certain vnum to do something (except ch)
*
* Syntax: mob vforce [vnum] [commands]
*/
void do_mpvforce( CHAR_DATA *ch, char *argument )
{
   CHAR_DATA *victim, *victim_next;
   char arg[ MAX_INPUT_LENGTH ];
   int vnum;

   argument = one_argument( argument, arg );

   if ( arg[0] == '\0' || argument[0] == '\0' )
   {
      bug( "MpVforce - Bad syntax from vnum %d.",
      IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
      return;
   }

   if ( !is_number( arg ) )
   {
      bug( "MpVforce - Non-number argument vnum %d.",
      IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
      return;
   }

   vnum = atoi( arg );

   for ( victim = char_list; victim; victim = victim_next )
   {
      victim_next = victim->next;
      if ( IS_NPC(victim) && victim->pIndexData->vnum == vnum
      &&   ch != victim && victim->fighting == NULL )
      interpret( victim, argument );
   }
   return;
}


/*
* Lets the mobile cast spells --
* Beware: this does only crude checking on the target validity
* and does not account for mana etc., so you should do all the
* necessary checking in your mob program before issuing this cmd!
*
* Syntax: mob cast [spell] {target}
*/

void do_mpcast(CHAR_DATA* ch, char* argument)
{
   char* arg;
   char spell[ MAX_INPUT_LENGTH ],
   target[ MAX_INPUT_LENGTH ];
   int sn;

   arg = one_argument(argument, spell);
   one_argument(arg, target);

   if (spell[0] == '\0')
   {
      bug
      (
         "MpCast - Bad syntax from vnum %d.",
         IS_NPC(ch) ? ch->pIndexData->vnum : 0
      );
      return;
   }

   if ((sn = skill_lookup(spell)) < 0)
   {
      bug
      (
         "MpCast - No such spell from vnum %d.",
         IS_NPC(ch) ? ch->pIndexData->vnum : 0
      );
      return;
   }
   if (is_clergy(ch))
   {
      is_supplicating = TRUE;
      magic_spell(ch, argument, CAST_BITS_MOB_PRAYER, ch->level, sn);
   }
   else
   {
      magic_spell(ch, argument, CAST_BITS_MOB_SPELL, ch->level, sn);
   }
   is_supplicating = FALSE;
}

void do_mpskillcast(CHAR_DATA* ch, char* argument)
{
   char* arg;
   char spell[ MAX_INPUT_LENGTH ],
   target[ MAX_INPUT_LENGTH ];
   int sn;

   arg = one_argument(argument, spell);
   one_argument(arg, target);

   if (spell[0] == '\0')
   {
      bug
      (
         "MpSkillCast - Bad syntax from vnum %d.",
         IS_NPC(ch) ? ch->pIndexData->vnum : 0
      );
      return;
   }

   if ((sn = skill_lookup(spell)) < 0)
   {
      bug
      (
         "MpSkillCast - No such spell from vnum %d.",
         IS_NPC(ch) ? ch->pIndexData->vnum : 0
      );
      return;
   }
   if (is_clergy(ch))
   {
      is_supplicating = TRUE;
      magic_spell(ch, argument, CAST_BITS_MOB_SKILL | CAST_BIT_SUPPLICATE, ch->level, sn);
   }
   else
   {
      magic_spell(ch, argument, CAST_BITS_MOB_SKILL, ch->level, sn);
   }
   is_supplicating = FALSE;
}

/*
* Lets mob cause unconditional damage to someone. Nasty, use with caution.
* Also, this is silent, you must show your own damage message...
*
* Syntax: mob damage [victim] [min] [max] {kill}
*/
void do_mpdamage( CHAR_DATA *ch, char *argument )
{
   CHAR_DATA *victim = NULL, *victim_next;
   char target[ MAX_INPUT_LENGTH ],
   min[ MAX_INPUT_LENGTH ],
   max[ MAX_INPUT_LENGTH ];
   int low, high;
   bool fAll = FALSE, fKill = FALSE;

   argument = one_argument( argument, target );
   argument = one_argument( argument, min );
   argument = one_argument( argument, max );

   if ( target[0] == '\0' )
   {
      bug( "MpDamage - Bad syntax from vnum %d.",
      IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
      return;
   }
   if ( !str_cmp( target, "all" ) )
   fAll = TRUE;
   else if ( ( victim = get_char_room( ch, target ) ) == NULL )
   return;

   if ( is_number( min ) )
   low = atoi( min );
   else
   {
      bug( "MpDamage - Bad damage min vnum %d.",
      IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
      return;
   }
   if ( is_number( max ) )
   high = atoi( max );
   else
   {
      bug( "MpDamage - Bad damage max vnum %d.",
      IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
      return;
   }
   one_argument( argument, target );

   /*
   * If kill parameter is omitted, this command is "safe" and will not
   * kill the victim.
   */

   if ( target[0] != '\0' )
   fKill = TRUE;
   if ( fAll )
   {
      for ( victim = ch->in_room->people; victim; victim = victim_next )
      {
         victim_next = victim->next_in_room;
         if ( victim != ch && victim->ghost <= 0)
         damage( victim, victim,
         fKill ?
         number_range(low, high) : UMIN(victim->hit, number_range(low, high)),
         TYPE_UNDEFINED, DAM_OTHER, FALSE );
      }
   }
   else if (victim->ghost <= 0)
   damage( victim, victim,
   fKill ?
   number_range(low, high) : UMIN(victim->hit, number_range(low, high)),
   TYPE_UNDEFINED, DAM_OTHER, FALSE );
   return;
}

/*
* Lets the mobile to remember a target. The target can be referred to
* with $q and $Q codes in MOBprograms. See also "mob forget".
*
* Syntax: mob remember [victim]
*/
void do_mpremember( CHAR_DATA *ch, char *argument )
{
   char arg[ MAX_INPUT_LENGTH ];
   one_argument( argument, arg );
   if ( arg[0] != '\0' )
   ch->mprog_target = get_char_world( ch, arg );
   else
   bug( "MpRemember: missing argument from vnum %d.",
   IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
}

/*
* Reverse of "mob remember".
*
* Syntax: mob forget
*/
void do_mpforget( CHAR_DATA *ch, char *argument )
{
   ch->mprog_target = NULL;
}

/*
* Sets a delay for MOBprogram execution. When the delay time expires,
* the mobile is checked for a MObprogram with DELAY trigger, and if
* one is found, it is executed. Delay is counted in PULSE_MOBILE
*
* Syntax: mob delay [pulses]
*/
void do_mpdelay( CHAR_DATA *ch, char *argument )
{
   char arg[ MAX_INPUT_LENGTH ];

   one_argument( argument, arg );
   if ( !is_number( arg ) )
   {
      bug( "MpDelay: invalid arg from vnum %d.",
      IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
      return;
   }
   ch->mprog_delay = atoi( arg );
}

/* Makes mob bust a prompt to all in room */
void do_mpwait( CHAR_DATA *ch, char *argument )
{
   DESCRIPTOR_DATA *d;

   for ( d = descriptor_list ; d != NULL ; d = d->next )
   {
      if ( d->connected == CON_PLAYING
      && d->character->in_room == ch->in_room
      && d->character != ch )
      {
         send_to_char("\n\r", d->character);
         bust_a_prompt( d->character );
         send_to_char("\n\r", d->character);
      }
   }
}

/*
* Reverse of "mob delay", deactivates the timer.
*
* Syntax: mob cancel
*/
void do_mpcancel( CHAR_DATA *ch, char *argument )
{
   ch->mprog_delay = -1;
}
/*
* Lets the mobile to call another MOBprogram withing a MOBprogram.
* This is a crude way to implement subroutines/functions. Beware of
* nested loops and unwanted triggerings... Stack usage might be a problem.
* Characters and objects referred to must be in the same room with the
* mobile.
*
* Syntax: mob call [vnum] [victim|'null'] [object1|'null'] [object2|'null']
*
*/
void do_mpcall( CHAR_DATA *ch, char *argument )
{
   char arg[ MAX_INPUT_LENGTH ];
   CHAR_DATA *vch;
   OBJ_DATA *obj1, *obj2;
   MPROG_CODE *prg;
   extern void program_flow( sh_int, char *, CHAR_DATA *, CHAR_DATA *, const void *, const void * );

   argument = one_argument( argument, arg );
   if ( arg[0] == '\0' )
   {
      bug( "MpCall: missing arguments from vnum %d.",
      IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
      return;
   }
   if ( ( prg = get_mprog_index( atoi(arg) ) ) == NULL )
   {
      bug( "MpCall: invalid prog from vnum %d.",
      IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
      return;
   }
   vch = NULL;
   obj1 = obj2 = NULL;
   argument = one_argument( argument, arg );
   if ( arg[0] != '\0' )
   vch = get_char_room( ch, arg );
   argument = one_argument( argument, arg );
   if ( arg[0] != '\0' )
   obj1 = get_obj_here( ch, arg );
   argument = one_argument( argument, arg );
   if ( arg[0] != '\0' )
   obj2 = get_obj_here( ch, arg );
   program_flow( prg->vnum, prg->code, ch, vch, (void *)obj1, (void *)obj2 );
}

/*
* Forces the mobile to flee.
*
* Syntax: mob flee
*
*/
void do_mpflee( CHAR_DATA *ch, char *argument )
{
   ROOM_INDEX_DATA *was_in;
   EXIT_DATA *pexit;
   int door, attempt;

   if ( ch->fighting != NULL )
   return;

   if ( (was_in = ch->in_room) == NULL )
   return;

   for ( attempt = 0; attempt < 6; attempt++ )
   {
      door = number_door( );
      if ( ( pexit = was_in->exit[door] ) == 0
      ||   pexit->u1.to_room == NULL
      ||   IS_SET(pexit->exit_info, EX_CLOSED)
      || ( IS_NPC(ch)
      &&   IS_SET(pexit->u1.to_room->room_flags, ROOM_NO_MOB) ) )
      continue;

      move_char( ch, door, MOVE_CHAR_FLEE);
      if ( ch->in_room != was_in )
      return;
   }
}

/*
* Lets the mobile to transfer an object. The object must be in the same
* room with the mobile.
*
* Syntax: mob otransfer [item name] [location]
*/
void do_mpotransfer( CHAR_DATA *ch, char *argument )
{
   OBJ_DATA *obj;
   ROOM_INDEX_DATA *location;
   char arg[ MAX_INPUT_LENGTH ];
   char buf[ MAX_INPUT_LENGTH ];

   argument = one_argument( argument, arg );
   if ( arg[0] == '\0' )
   {
      bug( "MpOTransfer - Missing argument from vnum %d.",
      IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
      return;
   }
   one_argument( argument, buf );
   if ( ( location = find_location( ch, buf ) ) == NULL )
   {
      bug( "MpOTransfer - No such location from vnum %d.",
      IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
      return;
   }
   if ( (obj = get_obj_here( ch, arg )) == NULL )
   return;
   if ( obj->carried_by == NULL )
   obj_from_room( obj );
   else
   {
      if ( obj->wear_loc != WEAR_NONE )
      unequip_char( ch, obj );
      obj_from_char( obj );
   }
   obj_to_room( obj, location );
}

/*
* Lets the mobile to strip an object or all objects from the victim.
* Useful for removing e.g. quest objects from a character.
*
* Syntax: mob remove [victim] [object vnum|'all']
*/
void do_mpremove( CHAR_DATA *ch, char *argument )
{
   CHAR_DATA *victim;
   OBJ_DATA *obj, *obj_next;
   sh_int vnum = 0;
   bool fAll = FALSE;
   char arg[ MAX_INPUT_LENGTH ];

   argument = one_argument( argument, arg );
   if ( ( victim = get_char_room( ch, arg ) ) == NULL )
   return;

   one_argument( argument, arg );
   if ( !str_cmp( arg, "all" ) )
   fAll = TRUE;
   else if ( !is_number( arg ) )
   {
      bug ( "MpRemove: Invalid object from vnum %d.",
      IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
      return;
   }
   else
   vnum = atoi( arg );

   for ( obj = victim->carrying; obj; obj = obj_next )
   {
      obj_next = obj->next_content;
      if ( fAll || obj->pIndexData->vnum == vnum )
      {
         unequip_char( ch, obj );
         obj_from_char( obj );
         extract_obj( obj, FALSE );
      }
   }
}

void do_mppeace(CHAR_DATA* ch, char* argument)
{
   do_peace(ch, argument);
}

void do_mpsetquest(CHAR_DATA *ch, char *argument)
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *vch;

   argument = one_argument(argument, arg);
   if (arg[0] != '\0')
   {
      vch = get_char_world(ch, arg);
   }
   else
   {
      bug(
         "do_mpsetquest: missing <target> argument from vnum %d.",
         IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
      return;
   }

   if (IS_NPC(vch))
   {
      return;
   }

   argument = one_argument(argument, arg);
   if (arg[0] == '\0')
   {
      bug(
         "do_mpsetquest: missing Quest ID from vnum %d.",
         IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
      return;
   }

   database_set_quest(vch, arg);
}

void do_mpremovequest(CHAR_DATA *ch, char *argument)
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *vch;

   argument = one_argument(argument, arg);
   if (arg[0] != '\0')
   {
      vch = get_char_world(ch, arg);
   }
   else
   {
      bug(
         "do_mpremovequest: missing <target> argument from vnum %d.",
         IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
      return;
   }

   if (IS_NPC(vch))
   {
      return;
   }

   argument = one_argument(argument, arg);
   if (arg[0] == '\0')
   {
      bug(
         "do_mpremovequest: missing Quest ID from vnum %d.",
         IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
      return;
   }

   database_remove_quest(vch, arg);
}

void do_mptimestamp(CHAR_DATA *ch, char *argument)
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *vch;

   argument = one_argument(argument, arg);
   if (arg[0] != '\0')
   {
      vch = get_char_world(ch, arg);
   }
   else
   {
      bug(
         "do_mptimestamp: missing <target> argument from vnum %d.",
         IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
      return;
   }

   if (IS_NPC(vch))
   {
      return;
   }

   argument = one_argument(argument, arg);
   if (arg[0] == '\0')
   {
      bug(
         "do_mptimestamp: missing Quest ID from vnum %d.",
         IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
      return;
   }

   database_set_timestamp(vch, arg);
}

void do_mpreward_gold(CHAR_DATA *ch, char *argument)
{
   char arg[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA *vch;
   int gold = 0;

   argument = one_argument(argument, arg);
   if (arg[0] != '\0')
   {
      vch = get_char_world(ch, arg);
   }
   else
   {
      bug(
         "do_mpreward_gold: missing <target> argument from vnum %d.",
         IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
      return;
   }

   if (IS_NPC(vch))
   {
      return;
   }

   one_argument(argument, arg);
   if (arg[0] == '\0')
   {
      bug(
         "do_mpreward_gold: missing gold value from vnum %d.",
         IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
      return;
   }

   gold = atoi(arg);

   if (gold <= 0)
   {
      bug(
         "do_mpreward_gold: gold value must be positive in vnum %d.",
         IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
      return;
   }

   /* We need to check weight.  Can the player carry the gold? */
   if ((gold / 5) > UMAX(can_carry_w(vch) - get_carry_weight(vch), 0))
   {
      /* If the player cannot carry the gold, we will drop it. */
      if (ch->in_room)
      {
         obj_to_room(create_money(gold, 0), ch->in_room);
         sprintf(
            buf,
            "You have earned %d gold!\n\r"
            "Since you cannot carry it, it is on the ground.\n\r",
            gold);
      }
   }
   else
   {
      vch->gold += gold;
      sprintf(buf, "You have earned %d gold!\n\r", gold);
   }
   send_to_char(buf, vch);
}

void do_mpreward_xp(CHAR_DATA *ch, char *argument)
{
   char arg[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA *vch;
   int experience = 0;

   argument = one_argument(argument, arg);
   if (arg[0] != '\0')
   {
      vch = get_char_world(ch, arg);
   }
   else
   {
      bug(
         "do_mpreward_xp: missing <target> argument from vnum %d.",
         IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
      return;
   }

   if (IS_NPC(vch))
   {
      return;
   }

   one_argument(argument, arg);
   if (arg[0] == '\0')
   {
      bug(
         "do_mpreward_xp: missing experience value from vnum %d.",
         IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
      return;
   }

   experience = atoi(arg);

   sprintf(
      buf,
      "You have %s %d experience points!\n\r",
      (experience < 0) ? "lost" : "earned",
      experience);
   send_to_char(buf, vch);

   gain_exp(vch, experience);
}

void do_mpstring( CHAR_DATA *ch, char *argument)
{
   do_string(ch, argument);
}

