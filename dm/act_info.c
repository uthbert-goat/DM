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


static const char rcsid[] = "$Id: act_info.c,v 1.265 2004/11/25 09:26:08 fizzfaldt Exp $";

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "merc.h"
#include "magic.h"
#include "recycle.h"
#include "tables.h"
#include "clan.h"
#include "lookup.h"
#include "subclass.h"
#include "worship.h"
#include "music.h"

/* command procedures needed */
DECLARE_DO_FUN(    do_exits    );
DECLARE_DO_FUN( do_look        );
DECLARE_DO_FUN( do_freetell     );
DECLARE_DO_FUN( do_help        );
DECLARE_DO_FUN( do_affects    );
DECLARE_DO_FUN( do_play        );
DECLARE_DO_FUN( do_morph_dragon );
DECLARE_DO_FUN( do_morph_archangel );
DECLARE_DO_FUN( do_morph_winged );
DECLARE_DO_FUN( do_revert    );
DECLARE_DO_FUN( do_tell        );
DECLARE_DO_FUN( do_rebel    );
DECLARE_DO_FUN( do_name_list    );
DECLARE_DO_FUN( do_hecho );

int tax_today = 0;
void add_who_tags(CHAR_DATA *ch, CHAR_DATA *victim, char *buf);
bool is_note_to( CHAR_DATA *ch, NOTE_DATA *pnote );
char * get_eqslot_name_ordered( CHAR_DATA *ch, int slotnum, bool book);
char * get_eqslot_name( CHAR_DATA *ch, int slotnum);
bool appended_eqlist(CHAR_DATA *ch, CHAR_DATA *victim);
void append_position(char *buf, CHAR_DATA *ch, CHAR_DATA *victim);
void timeval_to_mudtime args( (TIMEVAL* input, MUD_TIME* output) );
void mudtime_to_timeval args( (MUD_TIME* input, TIMEVAL* output) );
void buk_desc           args( (CHAR_DATA* ch, CHAR_DATA* book) );
void check_changeling_desc args( (CHAR_DATA* ch) );

char* mud_tick_string      args( (int duration, char* buf) );
char* mud_time_string      args( (MUD_TIME* input, char* buf, bool exact) );
void  set_time_to_accuracy args( (MUD_TIME* time, int accuracy) );
int   get_time_accuracy    args( (CHAR_DATA* ch, int type) );
char* get_dossier_time     args( (CHAR_DATA* ch, char* buf) );

typedef struct  wear_name_data              WEAR_NAME_DATA;

struct    wear_name_data
{
   int slot;
   char* const name;
};



/*
   put in by Fizzfaldt
   reorder the lines
   to reorder do_equipment,
   look, and glance output of
   equipment
*/
const WEAR_NAME_DATA   wear_name_ordered      [] =
{
   {WEAR_LIGHT,          "<used as light>        "},
   {WEAR_FINGER_L,       "<worn on finger>       "},
   {WEAR_FINGER_R,       "<worn on finger>       "},
   {WEAR_NECK_1,         "<worn around neck>     "},
   {WEAR_NECK_2,         "<worn around neck>     "},
   {WEAR_BODY,           "<worn on torso>        "},
   {WEAR_BOSOM,          "<worn on bosom>        "},
   {WEAR_HORNS,          "<worn on horns>        "},
   {WEAR_HEAD,           "<worn on head>         "},
   {WEAR_INSIDE_COVER,   "<worn inside cover>    "},
   {WEAR_EYES,           "<worn over eyes>       "},
   {WEAR_EAR_L,          "<worn on ear>          "},
   {WEAR_EAR_R,          "<worn on ear>          "},
   {WEAR_SNOUT,          "<worn on snout>        "},
   {WEAR_LEGS,           "<worn on legs>         "},
   {WEAR_FOURLEGS,       "<worn on four legs>    "},
   {WEAR_FEET,           "<worn on feet>         "},
   {WEAR_HOOVES,         "<worn on hooves>       "},
   {WEAR_FOURHOOVES,     "<worn on four hooves>  "},
   {WEAR_HANDS,          "<worn on hands>        "},
   {WEAR_ARMS,           "<worn on arms>         "},
   {WEAR_SHIELD,         "<worn as shield>       "},
   {WEAR_ABOUT,          "<worn on body>         "},
   {WEAR_WAIST,          "<worn on waist>        "},
   {WEAR_WRIST_L,        "<worn around wrist>    "},
   {WEAR_WRIST_R,        "<worn around wrist>    "},
   {WEAR_WIELD,          "<wielded>              "},
   {WEAR_HOLD,           "<held>                 "},
   {WEAR_DUAL_WIELD,     "<dual wielded>         "},
   {WEAR_TERTIARY_WIELD, "<trinal wielded>       "},
   {WEAR_BRAND,          "<branded>              "}
};

const WEAR_NAME_DATA   wear_name_ordered_buk    [] =
{
   {WEAR_LIGHT,          "<used as bookmark>     "},
   {WEAR_FINGER_L,       "<stuck on string>      "},
   {WEAR_FINGER_R,       "<stuck on string>      "},
   {WEAR_NECK_1,         "<worn on bookmark>     "},
   {WEAR_NECK_2,         "<worn on bookmark>     "},
   {WEAR_BODY,           "<worn around cover>    "},
   {WEAR_BOSOM,          "<stuck on front cover> "},
   {WEAR_HORNS,          "<worn on top of cover> "},
   {WEAR_HEAD,           "<worn on top of cover> "},
   {WEAR_INSIDE_COVER,   "<worn inside cover>    "},
   {WEAR_EYES,           "<wedged into pages>    "},
   {WEAR_EAR_L,          "<worn in left tassle>  "},
   {WEAR_EAR_R,          "<worn in right tassle> "},
   {WEAR_SNOUT,          "<glued on cover>       "},
   {WEAR_LEGS,           "<tied on tassles>      "},
   {WEAR_FOURLEGS,       "<tied on tassles>      "},
   {WEAR_FEET,           "<dragging on floor>    "},
   {WEAR_HOOVES,         "<dragging on floor>    "},
   {WEAR_FOURHOOVES,     "<dragging on floor>    "},
   {WEAR_HANDS,          "<worn on tassles>      "},
   {WEAR_ARMS,           "<hanging off tassles>  "},
   {WEAR_SHIELD,         "<covering rear cover>  "},
   {WEAR_ABOUT,          "<hanging off cover>    "},
   {WEAR_WAIST,          "<tied on>              "},
   {WEAR_WRIST_L,        "<worn around tassle>   "},
   {WEAR_WRIST_R,        "<worn around tassle>   "},
   {WEAR_WIELD,          "<grasped in tassle>    "},
   {WEAR_HOLD,           "<held in tassles>      "},
   {WEAR_DUAL_WIELD,     "<grasped in tassle>    "},
   {WEAR_TERTIARY_WIELD, "<grasped in tassle>    "},
   {WEAR_BRAND,          "<branded>              "}
};

char *  const   wear_name      [] =
{
   "<used as light>       ",
   "<worn on finger>      ",
   "<worn on finger>      ",
   "<worn around neck>    ",
   "<worn around neck>    ",
   "<worn on torso>       ",
   "<worn on head>        ",
   "<worn on legs>        ",
   "<worn on feet>        ",
   "<worn on hands>       ",
   "<worn on arms>        ",
   "<worn as shield>      ",
   "<worn on body>        ",
   "<worn on waist>       ",
   "<worn around wrist>   ",
   "<worn around wrist>   ",
   "<wielded>             ",
   "<held>                ",
   "<dual wielded>        ",
   "<trinal wielded>      ",
   "<branded>             ",
   "<worn on horns>       ",
   "<worn on snout>       ",
   "<worn on hooves>      ",
   "<worn on four legs>   ",
   "<worn on four hooves> ",
   "<worn over eyes>      ",
   "<worn on ear>         ",
   "<worn on ear>         ",
   "<worn on bosom>       ",
};

/* for do_count */
int max_on = 0;
int max_on_ever = 0;


/*
   Local functions.
*/
void    list_ancients        args((CHAR_DATA *ch));
char *  get_ancient_title    args((CHAR_DATA *ch));
void    collect_mark_refund    args( (CHAR_DATA *ch, CHAR_DATA *criminal));
void    mark_list        args( (CHAR_DATA *ch ));
bool    is_occupied        args( ( int ) );
bool    check_room_protected    args( (ROOM_INDEX_DATA * room ) );
char *    format_obj_to_char    args( ( OBJ_DATA *obj, CHAR_DATA *ch,
bool fShort ) );
void    show_list_to_char    args( ( OBJ_DATA *list, CHAR_DATA *ch,
bool fShort, bool fShowNothing ) );
void    show_char_to_char_0    args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void    show_char_to_char_1    args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void    show_char_to_char_2    args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void    show_char_to_char_3    args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void    show_char_to_char    args( ( CHAR_DATA *list, CHAR_DATA *ch ) );
bool    check_blind        args( ( CHAR_DATA *ch ) );

bool    can_shapeshift          args( ( CHAR_DATA *ch, int form_num, CHAR_DATA *morphtarget) );
bool    shapeshift_revert    args( ( CHAR_DATA *ch) );

char *format_obj_to_char( OBJ_DATA *obj, CHAR_DATA *ch, bool fShort )
{
   static char buf[MAX_STRING_LENGTH];
   OBJ_DATA* brand;

   buf[0] = '\0';

   if ((fShort && (obj->short_descr == NULL || obj->short_descr[0] == '\0'))
   ||  (obj->description == NULL || obj->description[0] == '\0'))
   return buf;

   if
   (
      ch != NULL &&
      IS_SET(ch->comm, COMM_LIMITS) &&
      obj != NULL &&
      obj->pIndexData != NULL &&
      obj->pIndexData->limtotal > 0
   )
   {
      if (obj->pIndexData->limcount > obj->pIndexData->limtotal)
      strcat( buf, "(O) " );
      else if (obj->pIndexData->limtotal == obj->pIndexData->limcount)
      strcat( buf, "(M) " );
      else
      strcat( buf, "(L) " );
   }
   if
   (
      obj->item_type == ITEM_PORTAL_NEW &&
      IS_SET(obj->value[4], PORTAL_TIMEGATE)
   )
   {
      strcat(buf, "(Temporal Flux) ");
   }
   if
   (
      obj->item_type == ITEM_PORTAL_NEW &&
      IS_SET(obj->value[4],PORTAL_TIMEGATE) &&
      IS_SET(obj->value[4],PORTAL_CLOSED)
   )
   {
      strcat(buf, "(Sealed) ");
   }
   if
   (
      obj->item_type == ITEM_PORTAL_NEW &&
      IS_SET(obj->value[4],PORTAL_MOONGATE) &&
      IS_SET(obj->value[4],PORTAL_CLOSED)
   )
   {
      strcat(buf, "(Sealed) ");
      strcat(buf, " ");
   }
   if (IS_OBJ_STAT(obj, ITEM_INVIS))
   {
      strcat( buf, "(Invis) "     );
   }
   if
   (
      IS_OBJ_STAT(obj, ITEM_EVIL) &&
      (
         IS_AFFECTED(ch, AFF_DETECT_EVIL) ||
         ch->house == HOUSE_LIGHT
      )
   )
   {
      strcat( buf, "(Red Aura) "  );
   }
   if
   (
      IS_AFFECTED(ch, AFF_DETECT_GOOD) &&
      IS_OBJ_STAT(obj, ITEM_BLESS)
   )
   {
      strcat( buf, "(Blue Aura) "    );
   }
   if
   (
      IS_AFFECTED(ch, AFF_DETECT_MAGIC) &&
      IS_OBJ_STAT(obj, ITEM_MAGIC)
   )
   {
      strcat( buf, "(Magical) "   );
   }
   if (IS_OBJ_STAT(obj, ITEM_GLOW))
   {
      strcat( buf, "(Glowing) "   );
   }
   if (IS_OBJ_STAT(obj, ITEM_HUM))
   {
      strcat( buf, "(Humming) "   );
   }
   if
   (
      is_affected(ch, gsn_vigilance) &&
      is_pulsing(obj)
   )
   {
      strcat( buf, "(Pulsing) "   );
   }
   if
   (
      IS_SET(obj->extra_flags2, ITEM_PURIFIED) &&
      !obj->enchanted
   )
   {
      strcat (buf, "(Purified) " ); /*Jord*/
   }
   if
   (
      obj->wear_loc == WEAR_INSIDE_COVER &&
      obj->carried_by &&
      !IS_NPC(obj->carried_by) &&
      (
         obj->carried_by->pcdata->current_desc == BOOK_RACE_CLOSED ||
         obj->carried_by->pcdata->current_desc == BOOK_RACE_LOCKED
      )
   )
   {
      strcat(buf, "(Behind Cover) ");
   }

   /* Rantialen's Ahzra 'immerse' bloodstain - Wicket */
   if ( IS_SET( obj->extra_flags2, ITEM_AHZRA_BLOODSTAIN ) )
   strcat (buf, "(Bloodstained) " );

   if ( is_affected(ch, gsn_vigilance)
   && ( (obj->extra_flags & ITEM_NODROP)
   ||   (obj->extra_flags & ITEM_NOREMOVE))) strcat( buf, "(Sticky) "      );

   if (obj->pIndexData->vnum == OBJ_VNUM_CHAOS_BRAND)
   {
      if (fShort)
      {
         strcat(buf, "the brand of the ");
         switch (number_range(1, 21))
         {
            case  1: strcat(buf, "rubber "); break;
            case  2: strcat(buf, "floppy "); break;
            case  3: strcat(buf, "blueberry "); break;
            case  4: strcat(buf, "slippery "); break;
            case  5: strcat(buf, "eternal "); break;
            case  6: strcat(buf, "dying "); break;
            case  7: strcat(buf, "smiling "); break;
            case  8: strcat(buf, "hairy "); break;
            case  9: strcat(buf, "spitting "); break;
            case 10: strcat(buf, "bloody "); break;
            case 11: strcat(buf, "drunken "); break;
            case 12: strcat(buf, "cynical "); break;
            case 13: strcat(buf, "chortling "); break;
            case 14: strcat(buf, "pulsating "); break;
            case 15: strcat(buf, "crackling "); break;
            case 16: strcat(buf, "broken "); break;
            case 17: strcat(buf, "checkered "); break;
            case 18: strcat(buf, "festering "); break;
            case 19: strcat(buf, "mooing "); break;
            case 20: strcat(buf, "bubbling "); break;
            default: strcat(buf, "puzzled ");
         }
         switch (number_range(1, 21))
         {
            case  1: strcat(buf, "duck"); break;
            case  2: strcat(buf, "cow"); break;
            case  3: strcat(buf, "broomstick"); break;
            case  4: strcat(buf, "chicken"); break;
            case  5: strcat(buf, "dodo"); break;
            case  6: strcat(buf, "wart"); break;
            case  7: strcat(buf, "eyeball"); break;
            case  8: strcat(buf, "toenail"); break;
            case  9: strcat(buf, "llama"); break;
            case 10: strcat(buf, "kielbasa"); break;
            case 11: strcat(buf, "moose"); break;
            case 12: strcat(buf, "cheese"); break;
            case 13: strcat(buf, "diggerydoo"); break;
            case 14: strcat(buf, "harmonica"); break;
            case 15: strcat(buf, "jellybean"); break;
            case 16: strcat(buf, "rope"); break;
            case 17: strcat(buf, "keyhole"); break;
            case 18: strcat(buf, "weasel"); break;
            case 19: strcat(buf, "donut"); break;
            case 20: strcat(buf, "gerbil"); break;
            default: strcat(buf, "flower");
         }
      }
      else
      {
         strcat(buf, "The brand of Chaos fades in and out of view, constantly shifting its nature.");
      }
   }
   else if
   (
      !IS_NPC(ch) &&
      obj->pIndexData->vnum == OBJ_VNUM_OBLIVION_BRAND &&
      obj->carried_by != NULL &&
      !IS_NPC(obj->carried_by) &&
      (
         obj->carried_by == ch ||
         (
            (brand = get_eq_char(ch, WEAR_BRAND)) != NULL &&
            (
               brand->pIndexData->vnum == OBJ_VNUM_OBLIVION_BRAND  /* ||
               brand->pIndexData->vnum == OBJ_VNUM_RESTIN_BRAND
               Restin's followers can see it as well, if he ever has
               a brand
               */
            )
         )
      ) &&
      obj->carried_by->pcdata->brand_rank <= 3 &&
      obj->carried_by->pcdata->brand_rank >= 0
   )
   {
      if (fShort)
      {
         switch (obj->carried_by->pcdata->brand_rank)
         {
            default:  /* 0 */
            {
               strcat(buf, "(Faded Aura) a Torn Page");
               break;
            }
            case (1):
            {
               strcat(buf, "(Faded Aura) a Broken Circle");
               break;
            }
            case (2):
            {
               strcat(buf, "(Faded Aura) a Faded Moon");
               break;
            }
            case (3):
            {
               strcat(buf, "(Faded Aura) a Raven embossed upon a Cracked Shield");
               break;
            }
         }
      }
      else
      {
         switch (obj->carried_by->pcdata->brand_rank)
         {
            default:  /* 0 */
            {
               strcat
               (
                  buf,
                  "A runic marking is carved into your skin styled in the form"
                  " of a torn parchment."
               );
               break;
            }
            case (1):
            {
               strcat
               (
                  buf,
                  "A runic marking is carved into your skin styled in the form"
                  " of a broken circle."
               );
               break;
            }
            case (2):
            {
               strcat
               (
                  buf,
                  "A runic marking is carved into your skin, styled in the form"
                  " of a Moon fading from existence."
               );
               break;
            }
            case (3):
            {
               strcat
               (
                  buf,
                  "A runic marking is carved into your skin, styled in the form"
                  " of Cracked Shield with a Raven embossed upon it."
               );
               break;
            }
         }
      }
   }
   else
   {
      if ( fShort )
      {
         if ( obj->short_descr != NULL )
         strcat( buf, obj->short_descr );
      }
      else
      {
         if ( obj->description != NULL)
         strcat( buf, obj->description );
      }
   }

   return buf;
}



/*
   Show a list to a character.
   Can coalesce duplicated items.
*/
void show_list_to_char( OBJ_DATA *list, CHAR_DATA *ch, bool fShort, bool fShowNothing )
{
   char buf[MAX_STRING_LENGTH];
   BUFFER *output;
   char **prgpstrShow;
   int *prgnShow;
   char *pstrShow;
   OBJ_DATA *obj;
   int nShow;
   int iShow;
   int count;
   int line;
   bool fCombine;

   if ( ch->desc == NULL )
   return;

   /*
   * Alloc space for output lines.
   */
   output = new_buf();
   line = ch->lines;
   ch->lines = 0;
   count = 0;
   for ( obj = list; obj != NULL; obj = obj->next_content )
   count++;
   prgpstrShow    = alloc_mem( count * sizeof(char *) );
   prgnShow    = alloc_mem( count * sizeof(int)    );
   nShow    = 0;

   /*
   * Format the list of objects.
   */
   for ( obj = list; obj != NULL; obj = obj->next_content )
   {
      if ( obj->wear_loc == WEAR_NONE && can_see_obj( ch, obj ))
      {
         pstrShow = format_obj_to_char( obj, ch, fShort );

         fCombine = FALSE;

         if ( IS_NPC(ch) || IS_SET(ch->comm, COMM_COMBINE) )
         {
            /*
            * Look for duplicates, case sensitive.
            * Matches tend to be near end so run loop backwords.
            */
            for ( iShow = nShow - 1; iShow >= 0; iShow-- )
            {
               if ( !strcmp( prgpstrShow[iShow], pstrShow ) )
               {
                  prgnShow[iShow]++;
                  fCombine = TRUE;
                  break;
               }
            }
         }

         /*
         * Couldn't combine, or didn't want to.
         */
         if ( !fCombine )
         {
            prgpstrShow [nShow] = str_dup( pstrShow );
            prgnShow    [nShow] = 1;
            nShow++;
         }
      }
   }

   /*
   * Output the formatted list.
   */
   for ( iShow = 0; iShow < nShow; iShow++ )
   {
      if (prgpstrShow[iShow][0] == '\0')
      {
         free_string(prgpstrShow[iShow]);
         continue;
      }

      if ( IS_NPC(ch) || IS_SET(ch->comm, COMM_COMBINE))
      {
         if ( prgnShow[iShow] != 1 )
         {
            sprintf( buf,  "(%2d) ", prgnShow[iShow] );
            add_buf(output, buf);
         }
         else
         {
            add_buf(output, "     ");
         }
      }
      add_buf(output, prgpstrShow[iShow]);
      add_buf(output, "\n\r");
      free_string( prgpstrShow[iShow] );
   }

   if ( fShowNothing && nShow == 0 )
   {
      if ( IS_NPC(ch) || IS_SET(ch->comm, COMM_COMBINE) )
      send_to_char( "     ", ch );
      send_to_char( "Nothing.\n\r", ch );
   }
   page_to_char(buf_string(output), ch);

   /*
   * Clean up.
   */
   free_buf(output);
   free_mem( prgpstrShow, count * sizeof(char *) );
   free_mem( prgnShow,    count * sizeof(int)    );
   ch->lines = line;
   return;
}



void show_char_to_char_0( CHAR_DATA *victim, CHAR_DATA *ch )
{
   char buf[MAX_STRING_LENGTH];
   int sn_faerie_fog;
   OBJ_DATA *obj;
   buf[0] = '\0';

   if (is_affected(victim, gsn_feign_death))
   {
      strcat( buf, get_descr_form(victim, ch, TRUE) );
      buf[0] = UPPER(buf[0]);
      send_to_char(buf, ch);
      return;
   }

   obj = get_eq_char(victim, WEAR_HEAD);

   sn_faerie_fog = gsn_faerie_fog;
   if ( IS_SET(victim->comm, COMM_AFK      )   ) strcat( buf, "[AFK] "         );
   if ( !IS_NPC(victim))
   {
      if (victim->pcdata->death_status == HAS_DIED)
      strcat(buf, "(GHOST) ");
   }
   if (is_affected(victim, gsn_smelly))
   {
      strcat(buf, "(Smelly) ");
   }
   if (check_simulacrum(victim) != -1        ) strcat(buf, "(Distorted) ");
   if ( IS_AFFECTED(victim, AFF_INVISIBLE)   ) strcat( buf, "(Invis) "      );
   if ( victim->invis_level >= LEVEL_HERO    ) strcat( buf, "(Wizi) "         );
   if ( is_affected(victim, gsn_shroud)      ) strcat( buf, "(Shroud) "     );
   if ( is_affected(victim, gsn_earthfade)    ) strcat(buf, "(Earthfade) " );
   if ( IS_AFFECTED(victim, AFF_HIDE)        ) strcat( buf, "(Hide) "       );
   if ( IS_AFFECTED(victim, AFF_CAMOUFLAGE)  ) strcat( buf, "(Camouflage) " );
   if (is_affected(victim, gsn_forest_blending)) strcat(buf, "(Blending) " );
   if ( IS_AFFECTED(victim, AFF_CHARM)       ) strcat( buf, "(Charmed) "    );
   if ( IS_AFFECTED(victim, AFF_PASS_DOOR)   ) strcat( buf, "(Translucent) ");
   if ( IS_AFFECTED(victim, AFF_FAERIE_FIRE) ) strcat( buf, "(Pink Aura) "  );
   if (is_affected(victim, sn_faerie_fog) )    strcat( buf, "(Purple Aura) ");
   if ( is_affected(victim, gsn_nether_shroud) )    strcat( buf, "(Black Aura) ");
   if ( is_affected(victim, gsn_battleaxe_haste) ) strcat( buf, "(Green Aura) ");
   /* Aura for Hector's brand - Wicket */
   if ( is_affected(victim, gsn_hector_brand_spark) ) strcat( buf, "(Brilliant Aura) ");
   /* Aura for Utara's brand - Wicket */
   if ( is_affected(victim, gsn_utara_brand_blur) ) strcat( buf, "(Emerald Aura) ");
   /* Aura for Drinlinda's brand - Drinlinda */
   if (is_affected(victim, gsn_drinlinda_defense_brand)) strcat(buf, "(Aura of Innocence) ");
   /* Aura for Drithentir's brand - Drinlinda */
   if (is_affected(victim, gsn_drithentir_vampiric_brand)) strcat(buf, "(Vampiric Aura) ");
   /* Aura used for Rantialen's Ahzra - Wicket */
   if ( ( victim->house != HOUSE_ANCIENT && !is_affected( victim, gsn_cloak_form ) )
   && IS_SET( victim->affected_by2, AFF_AHZRA_BLOODSTAIN ) ) strcat( buf, "(Bloodstained) ");
   if ( IS_EVIL(victim)
   && (IS_AFFECTED(ch, AFF_DETECT_EVIL)
   || ch->house == HOUSE_LIGHT)     ) strcat( buf, "(Red Aura) "   );
   if ( IS_GOOD(victim)
   &&   IS_AFFECTED(ch, AFF_DETECT_GOOD)     ) strcat( buf, "(Golden Aura) ");
   if (is_affected(ch, gsn_eyes_of_the_bandit) && !IS_NPC(victim) && !IS_IMMORTAL(victim))
   {
      int victim_score = hoard_score(victim);

      if ((victim_score >= 1) && (victim_score <= 75))
         strcat(buf, "(Emerald Aura) ");
      else
      if ((victim_score >= 76) && (victim_score <= 150))
         strcat(buf, "(Sapphire Aura) ");
      else
      if (victim_score >= 151)
         strcat(buf, "(Diamond Aura) ");
   }
   if
   (
      is_affected(victim, gsn_will_power) ||
      is_affected(victim, gsn_will_dread) ||
      is_affected(victim, gsn_will_oblivion)
   )
   {
      strcat(buf, "(Faded Aura) ");
   }
   if (IS_AFFECTED(victim, AFF_SANCTUARY))
   {
      AFFECT_DATA* sanc;

      sanc = affect_find(victim->affected, gsn_sanctuary);
      if (sanc != NULL)
      {
         strcat(buf, "(Holy Aura) ");
      }
      else if
      (
         (
            sanc = affect_find(victim->affected, gsn_chromatic_shield)
         ) != NULL
      )
      {
         strcat(buf, "(Chromatic Aura) ");
      }
      else
      {
         sprintf(log_buf, "White aura on %s.", victim->name);
         bug_trust(log_buf, 0, get_trust(victim));
         strcat(buf, "(White Aura) ");
      }
   }
   if (is_affected(victim, gsn_sham_brand1)  ) strcat(buf, "(Violet Aura) ");
   if (is_affected(victim, gsn_stone) ) strcat(buf, "(Stone) ");
   if (is_affected(victim, gsn_freeze)) strcat(buf, "(Frozen) ");
   if (is_affected(victim, gsn_temporal_shear) ||
   is_affected(victim, gsn_timestop) ||
   is_affected(victim, gsn_temporal_shield)) strcat(buf, "(Temporal Flux) ");
   if (is_affected(victim, gsn_enlarge)) strcat(buf, "(Enlarged) ");
   if (is_affected(victim, gsn_shrink)) strcat(buf, "(Shrunk) ");
   if (obj != NULL && obj->pIndexData->vnum == OBJ_VNUM_TIARA)
   strcat( buf, "(Blue aura) ");
   if ( !IS_NPC(victim) && IS_SET(victim->act, PLR_KILLER ) )
   strcat( buf, "(KILLER) "     );
   if ( !IS_NPC(victim) && IS_SET(victim->act, PLR_THIEF  ) )
   strcat( buf, "(THIEF) "      );
   if (!IS_NPC(victim))
   {
      if (IS_SET(victim->act2, PLR_LAWLESS))
      {
         strcat(buf, "(LAWLESS) ");
      }
      else if (IS_SET(victim->act, PLR_CRIMINAL))
      {
         strcat(buf, "(CRIMINAL) ");
      }
   }

   if ( !IS_NPC(victim) && IS_SET(victim->act, PLR_EVIL) &&
   ((ch->house == HOUSE_LIGHT) || (IS_IMMORTAL(ch))))
   strcat( buf, "(EVIL) "       );
   if (IS_IMMORTAL(ch)){
      if ( !IS_NPC(victim) && IS_SET(victim->act, PLR_MARKED))
      strcat( buf, "(MARAUDER_MARKED) ");
      if ( !IS_NPC(victim) && (IS_SET(victim->act2, PLR_ANCIENT_MARK) ||
      IS_SET(victim->act2, PLR_ANCIENT_MARK_PERM)))
      strcat( buf, "(ANC_MARKED) ");
   }
   else
   {
      if
      (
         !IS_NPC(victim) &&
         IS_SET(victim->act, PLR_MARKED) &&
         ( (ch->house == HOUSE_OUTLAW) ||(ch->house == HOUSE_MARAUDER) )
      )
      strcat( buf, "(MARKED) "     );
      if ( !IS_NPC(victim) && (IS_SET(victim->act2, PLR_ANCIENT_MARK) ||
      IS_SET(victim->act2, PLR_ANCIENT_MARK_PERM)) &&
      ((ch->house == HOUSE_ANCIENT) || IS_IMMORTAL(ch)))
      strcat( buf, "(MARKED) "     );
   }
   if ( !IS_NPC(victim) && IS_SET(victim->wiznet, PLR_FORSAKEN) /* &&
   (ch->house == HOUSE_CRUSADER || IS_IMMORTAL(ch)) */)
   strcat( buf, "(FORSAKEN) ");
   if ( !IS_NPC(victim) && IS_SET(victim->act2, PLR_GUILDLESS))
   strcat( buf, "(GUILDLESS) ");

   if
   (
      is_affected(victim, gsn_cloak_form) ||
      victim->long_descr[0] != '\0' ||
      (
         victim->morph_form[0] != 0 &&
         (
            victim->morph_form[0] != MORPH_MIMIC ||
            (
               victim->mprog_target != NULL &&
               victim->mprog_target->long_descr[0] != '\0'
            )
         )
      ) ||
      is_affected(victim, gsn_toad)
   )
   {
      strcat( buf, get_descr_form(victim, ch, TRUE) );
      buf[0] = UPPER(buf[0]);
      if (is_affected(ch, gsn_strabismus))
      {
         send_to_char(buf, ch);
         if (number_percent() < 50)
         send_to_char(buf, ch);
         if (number_percent() < 50)
         send_to_char(buf, ch);
      }
      send_to_char( buf, ch );
      return;
   }
   if
   (
      !IS_NPC(victim) &&
      !IS_SET(ch->comm, COMM_BRIEF)
   )
   {
      if
      (
         victim->morph_form[0] == 0 &&
         victim->pcdata->pre_title[0] != '\0'
      )
      {
         strcat(buf, victim->pcdata->pre_title);
         strcat(buf, " ");
      }
      else if
      (
         victim->morph_form[0] == MORPH_MIMIC &&
         victim->mprog_target != NULL &&
         !IS_NPC(victim->mprog_target) &&
         victim->mprog_target->pcdata->pre_title[0] != '\0'
      )
      {
         strcat(buf, victim->mprog_target->pcdata->pre_title);
         strcat(buf, " ");
      }
   }
   strcat( buf, get_descr_form(victim, ch, FALSE) );
   if
   (
      !IS_NPC(victim) &&
      !IS_SET(ch->comm, COMM_BRIEF) &&
      victim->morph_form[0] == 0
   )
   {
      strcat(buf, victim->pcdata->title);
   }
   if ( !IS_NPC(victim) && !IS_SET(ch->comm, COMM_BRIEF) &&
   (victim->morph_form[0] == MORPH_MIMIC) && (victim->mprog_target != NULL)
   && !IS_NPC(victim->mprog_target))
   strcat( buf, victim->mprog_target->pcdata->title );

   append_position(buf, victim, ch);
   buf[0] = UPPER(buf[0]);
   if (is_affected(ch, gsn_strabismus))
   {
      send_to_char(buf, ch);
      if (number_percent() < 50)
      send_to_char(buf, ch);
      if (number_percent() < 50)
      send_to_char(buf, ch);
   }
   send_to_char( buf, ch );
   return;
}



void show_char_to_char_1( CHAR_DATA *victim, CHAR_DATA *ch )
{
   char buf[MAX_STRING_LENGTH];
   OBJ_DATA *obj;
   int iWear;
   int percent;
   bool found;
   CHAR_DATA *tch;
   int skill;
   bool book = victim->race == grn_book;

   /*    if ( can_see( victim, ch ) )
   {*/
   if (ch == victim)
   {
      for (tch = ch->in_room->people; tch != NULL; tch = tch->next_in_room)
      {
         if (can_see(tch, ch) && (ch != victim) && (tch != ch))
         act("$n looks at $mself.", ch, NULL, tch, TO_VICT);
      }
      /*        act( "$n looks at $mself.", ch, NULL, NULL, TO_ROOM);*/
   }
   else
   {
      if (can_see(victim, ch))
      act( "$n looks at you.", ch, NULL, victim, TO_VICT    );
      for (tch = ch->in_room->people; tch != NULL; tch = tch->next_in_room)
      {
         if (can_see(tch, ch) && (tch != victim))
         {
            sprintf(buf,  "$n looks at %s.", get_descr_form(victim, tch, FALSE));
            act(buf, ch, NULL, tch, TO_VICT);
         }
      }
      /*        act( "$n looks at $N.",  ch, NULL, victim, TO_NOTVICT );*/
   }
   /*    }*/
   if (is_affected(victim, gsn_cloak_form)) {
      send_to_char("The figure is buried deep within a dark colored cloak.\n\r", ch);
   }
   else if ((victim->morph_form[0] == MORPH_MIMIC) && (victim->mprog_target != NULL))
   {
      if (victim->mprog_target->description[0] != '\0')
      send_to_char(victim->mprog_target->description, ch);
      else
      act( "You see nothing special about $M.", ch, NULL, victim->mprog_target, TO_CHAR);
   } else if ( victim->description[0] != '\0' )
   {
      if (victim->race == grn_book)
      {
         buk_desc(ch, victim);
      }
      else
      {
         send_to_char(victim->description, ch);
      }
   }
   else
   {
      act( "You see nothing special about $M.", ch, NULL, victim, TO_CHAR );
   }

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

   if (!is_affected(victim, gsn_cloak_form)||IS_TRUSTED(ch, ANGEL)) {
      found = FALSE;
      for ( iWear = 0; iWear < MAX_WEAR; iWear++ )
      {
         if ( ( obj = get_eq_char( victim, wear_name_ordered[iWear].slot ) ) != NULL
         &&   can_see_obj( ch, obj ) )
         {
            if ( !found )
            {
               send_to_char( "\n\r", ch );
               act( "$N is using:", ch, NULL, victim, TO_CHAR );
               found = TRUE;
            }

            /* Malignus' styx brand fix */
            /*
            Styx's brand/worshippers are no longer secret
            at Styx's request - Fizzfaldt
            brand = get_eq_char(ch, WEAR_BRAND);  the looker's brand

            if (obj->pIndexData->vnum != 425)
            {
            */
            if (obj->pIndexData->vnum != OBJ_VNUM_TWIG_BRAND)
            {
               send_to_char( get_eqslot_name_ordered(victim, iWear, book), ch );
            }
            else
            {
               send_to_char("<glued on>             ", ch);
            }

            send_to_char( format_obj_to_char( obj, ch, TRUE ), ch );
            send_to_char( "\n\r", ch );

            /*
            Styx's brand/worshippers are no longer secret
            at Styx's request - Fizzfaldt
            } else if  (brand != NULL && brand->pIndexData->vnum == 425) {
               send_to_char( get_eqslot_name_ordered(victim, iWear, book), ch );
               send_to_char( format_obj_to_char( obj, ch, TRUE ), ch );
               send_to_char( "\n\r", ch );
            }
            */
            /* end Malignus' styx brand fix */


         }
      }
      appended_eqlist(ch, victim);

      if (IS_NPC(victim) &&
      IS_AFFECTED(victim, AFF_CHARM) &&
      victim->master == ch)
      {
         char buf[MAX_STRING_LENGTH];
         send_to_char("\n\r", ch);
         act("$N shows you $S inventory:", ch, NULL, victim, TO_CHAR);
         /*    send_to_char( "\n\rYou peek at the inventory:\n\r", ch ); */
         show_list_to_char( victim->carrying, ch, TRUE, TRUE );
         sprintf(buf,  "%ld gold coins, and %ld silver coins.\n\r", victim->gold,
         victim->silver);
         send_to_char(buf, ch);
         return;
      }

      /* enforcer courthouse peek */
      if (victim != ch
      &&  !IS_NPC(ch)
      &&  (IS_IMMORTAL(ch) || !IS_IMMORTAL(victim))
      &&  ch->house == HOUSE_ENFORCER
      &&  ch->in_room->vnum >= 18300 && ch->in_room->vnum <= 18349)
      {
         char buf[MAX_STRING_LENGTH];
         send_to_char( "\n\rYou peek at the inventory:\n\r", ch );
         show_list_to_char( victim->carrying, ch, TRUE, TRUE );
         sprintf(buf,  "%ld gold coins, and %ld silver coins.\n\r", victim->gold,
         victim->silver);
         send_to_char(buf, ch);
         return;
      }

      if
      (
         victim != ch &&
         !IS_NPC(ch) &&
         (
            IS_IMMORTAL(ch) ||
            !IS_IMMORTAL(victim)
         ) &&
         (
            skill = get_skill(ch, gsn_peek)
         ) &&
         number_percent( ) < UMAX(skill, 2) &&  /* work at 1% */
         (
            IS_IMMORTAL(ch) ||
            !is_affected(victim, gsn_worm_hide)
         )
      )
      {
         send_to_char( "\n\rYou peek at the inventory:\n\r", ch );
         check_improve(ch, gsn_peek, TRUE, 4);
         show_list_to_char( victim->carrying, ch, TRUE, TRUE );
         if (number_range(1, 2) == 1){
            char buf[MAX_STRING_LENGTH];
            int gold = victim->gold;
            int silver = victim->silver;
            gold = (float) gold *
            number_range((float) get_skill(ch, gsn_peek), 100.0)/100.0;
            silver = (float) silver *
            number_range((float) get_skill(ch, gsn_peek), 100.0)/100.0;
            sprintf(buf,  "%d gold coins, and %d silver coins.\n\r", gold, silver);
            send_to_char(buf, ch);
         }
      }
   }
   return;
}


void show_char_to_char_2( CHAR_DATA *victim, CHAR_DATA *ch )
{
   char buf[MAX_STRING_LENGTH];
   OBJ_DATA *obj;
   int iWear;
   int percent;
   bool found;
   OBJ_DATA *brand;
   int skill;
   bool book = victim->race == grn_book;

   if ( can_see( victim, ch ) )
   {
      if (ch == victim)
      act( "$n glances at $mself.", ch, NULL, NULL, TO_ROOM);
      else
      {
         act( "$n glances at you.", ch, NULL, victim, TO_VICT    );
         act( "$n glances at $N.",  ch, NULL, victim, TO_NOTVICT );
      }
   }

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

   found = FALSE;
   for ( iWear = 0; iWear < MAX_WEAR; iWear++ )
   {
      if ( ( obj = get_eq_char( victim, wear_name_ordered[iWear].slot ) ) != NULL
      &&   can_see_obj( ch, obj ) )
      {
         if ( !found )
         {
            send_to_char( "\n\r", ch );
            act( "$N is using:", ch, NULL, victim, TO_CHAR );
            found = TRUE;
         }



         /* Malignus' styx brand fix */
         brand = get_eq_char(ch, WEAR_BRAND); /*the looker's brand */

         /*
         Styx's brand/worshippers are no longer secret
         at Styx's request - Fizzfaldt
         if (obj->pIndexData->vnum != 425) {
         */
         if (obj->pIndexData->vnum != OBJ_VNUM_TWIG_BRAND)
         {
            send_to_char( get_eqslot_name_ordered(victim, iWear, book), ch );
         }
         else
         {
            send_to_char("<glued on>             ", ch);
         }
         send_to_char( format_obj_to_char( obj, ch, TRUE ), ch );
         send_to_char( "\n\r", ch );

         /*
         Styx's brand/worshippers are no longer secret
         at Styx's request - Fizzfaldt
         } else if  (brand != NULL && brand->pIndexData->vnum == 425) {
            send_to_char( get_eqslot_name_ordered(victim, iWear, book), ch );
            send_to_char( format_obj_to_char( obj, ch, TRUE ), ch );
            send_to_char( "\n\r", ch );
         }
         */

         /*  end Malignus' styx brand fix */

      }
   }
   appended_eqlist(ch, victim);

   if (victim != ch
   &&  !IS_NPC(ch)
   &&  (IS_IMMORTAL(ch) || !IS_IMMORTAL(victim))
   &&  ch->house == HOUSE_ENFORCER
   &&  ch->in_room->vnum >= 18300 && ch->in_room->vnum <= 18349)
   {
      char buf[MAX_STRING_LENGTH];
      send_to_char( "\n\rYou peek at the inventory:\n\r", ch );
      show_list_to_char( victim->carrying, ch, TRUE, TRUE );
      sprintf(buf,  "%ld gold coins, and %ld silver coins.\n\r", victim->gold,
      victim->silver);
      send_to_char(buf, ch);
      return;
   }

   if
   (
      victim != ch &&
      !IS_NPC(ch) &&
      (
         IS_IMMORTAL(ch) ||
         !IS_IMMORTAL(victim)
      ) &&
      (
         skill = get_skill(ch, gsn_peek)
      ) &&
      number_percent( ) < UMAX(skill, 2) &&  /* work at 1% */
      (
         IS_IMMORTAL(ch) ||
         !is_affected(victim, gsn_worm_hide)
      )
   )
   {
      send_to_char( "\n\rYou peek at the inventory:\n\r", ch );
      check_improve(ch, gsn_peek, TRUE, 4);
      show_list_to_char( victim->carrying, ch, TRUE, TRUE );
   }

   return;
}

void show_char_to_char_3( CHAR_DATA *victim, CHAR_DATA *ch )
{
   char buf[MAX_STRING_LENGTH];
   int percent;

   if ( can_see( victim, ch ) )
   {
      if (ch == victim)
      act( "$n looks closely at $mself.", ch, NULL, NULL, TO_ROOM);
      else
      {
         CHAR_DATA *tch;
         act( "$n looks closely at you.", ch, NULL, victim, TO_VICT    );
         /*        act( "$n looks closely at $N.",  ch, NULL, victim, TO_NOTVICT );*/
         for (tch = ch->in_room->people; tch != NULL; tch = tch->next_in_room)
         {
            if (can_see(tch, ch) && (tch != victim))
            {
               sprintf(buf,  "$n looks closely at %s.", get_descr_form(victim, tch, FALSE));
               act(buf, ch, NULL, tch, TO_VICT);
            }
         }
      }
   }

   if (is_affected(victim, gsn_cloak_form)) {
      send_to_char("The figure is buried deep within a dark colored cloak.\n\r", ch);
   }
   else if ((victim->morph_form[0] == MORPH_MIMIC) && (victim->mprog_target != NULL))
   {
      if (victim->mprog_target->description[0] != '\0')
      send_to_char(victim->mprog_target->description, ch);
      else
      act( "You see nothing special about $M.", ch, NULL, victim->mprog_target, TO_CHAR );
   } else if ( victim->description[0] != '\0' ) {
      if (victim->race == grn_book)
      {
         buk_desc(ch, victim);
      }
      else
      {
         send_to_char(victim->description, ch);
      }
   }
   else
   {
      act( "You see nothing special about $M.", ch, NULL, victim, TO_CHAR );
   }

   if ( victim->max_hit > 0 )
   percent = ( 100 * victim->hit ) / victim->max_hit;
   else
   percent = -1;

   strcpy( buf, get_descr_form(victim, ch, FALSE));

   if (IS_SET(ch->act, PLR_EVALUATION))
   {
      if (percent >= 100)
      strcat(buf, " is in perfect condition\n\r.");
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


void show_char_to_char( CHAR_DATA *list, CHAR_DATA *ch )
{
   CHAR_DATA *rch;
   bool cant_see = FALSE;
   bool blind_fighting = FALSE;
   int skill;

   if ( IS_AFFECTED( ch, AFF_BLIND )
   && is_affected( ch, gsn_blind_fighting ) )
   blind_fighting = TRUE;

   if
   (
      !IS_NPC(ch) &&
      is_affected_room(ch->in_room, gsn_globe_darkness) &&
      !IS_AFFECTED(ch, AFF_INFRARED) &&
      !IS_SET(ch->act, ACT_HOLYLIGHT)
   )
   {
      cant_see = TRUE;
   }

   for ( rch = list; rch != NULL; rch = rch->next_in_room )
   {
      /* Do not list ourselves in the room */
      if (rch == ch)
      {
         continue;
      }

      /* We may be able to see them with skills.  Whom can we never see? */
      if (
            !can_see(ch, rch) &&
            (
               IS_IMMORTAL(rch) ||                 /* unseen Imms */
               IS_SET(rch->affected_by2, AFF_WIZI) /* wizi mobs */
            )
         )
      {
         continue;
      }

      if (blind_fighting && !is_affected(rch, gsn_earthfade))
      {
         switch( rch->size )
         {
            case SIZE_TINY:
            send_to_char( "You sense a tiny figure.\n\r", ch );
            continue;
            case SIZE_SMALL:
            send_to_char( "You sense a small figure.\n\r", ch );
            continue;
            case SIZE_MEDIUM:
            send_to_char( "You sense a medium figure.\n\r", ch );
            continue;
            case SIZE_LARGE:
            send_to_char( "You sense a large figure.\n\r", ch );
            continue;
            case SIZE_HUGE:
            send_to_char( "You sense a huge figure.\n\r", ch );
            continue;
            case SIZE_GIANT:
            send_to_char( "You sense a giant figure.\n\r", ch );
            continue;
            default:
            continue;
         }
      }

      if (!can_see(ch, rch))
      {
         if
         (
            ch->pcdata->learned[gsn_corrupt("subrank", &gsn_subrank)] >= 9 &&
            has_skill(ch, gsn_eye_tiger) &&
            (
               skill = get_skill(ch, gsn_eye_tiger)
            ) &&
            number_percent() < UMAX(skill, 2)
         )
         {
            check_improve(ch, gsn_eye_tiger, TRUE, 4);
            send_to_char("Someone is here.\n\r", ch);
         }
         continue;
      }

      if ((cant_see))
      {
         send_to_char( "You see glowing red eyes watching YOU!\n\r", ch );
      }
      else if ( (cant_see))
      continue;
      else if ( can_see( ch, rch ))
      {
         show_char_to_char_0( rch, ch );
      }
      else if ( room_is_dark( ch->in_room )
      &&        IS_AFFECTED(rch, AFF_INFRARED ) )
      {
         send_to_char( "You see glowing red eyes watching YOU!\n\r", ch );
      }
   }

   return;
}



bool check_blind( CHAR_DATA *ch )
{

   /* Holy light for mobs with no mobprogs does not override blind */
   if
   (
      (
         !IS_NPC(ch) ||
         ch->pIndexData->mprog_flags != 0
      ) &&
      IS_SET(ch->act, ACT_HOLYLIGHT)
   )
   {
      return TRUE;
   }

   if ( is_affected(ch, gsn_bloodmist) )
   {
      if ( !is_affected( ch, gsn_blind_fighting ) )
      send_to_char("Your vision is clouded with blood!\n\r", ch);
      return FALSE;
   }

   if (IS_AFFECTED(ch, AFF_BLIND))
   {
      if (!is_affected(ch, gsn_blind_fighting))
      {
         send_to_char("You can't see a thing!\n\r", ch);
      }
      return FALSE;
   }
   if (is_affected_room(ch->in_room, gsn_corrupt("smoke screen", &gsn_smoke_screen)))
   {
      if (!is_affected(ch, gsn_blind_fighting))
      {
         send_to_char("You cannot see a thing for the smoke!\n\r", ch);
      }
      return FALSE;
   }

   return TRUE;
}

/* changes your scroll */
void do_scroll(CHAR_DATA *ch, char *argument)
{
   char arg[MAX_INPUT_LENGTH];
   char buf[100];
   int lines;

   one_argument(argument, arg);

   if (arg[0] == '\0')
   {
      if (ch->lines == 0)
      send_to_char("You do not page long messages.\n\r", ch);
      else
      {
         sprintf(buf,  "You currently display %d lines per page.\n\r",
         ch->lines + 2);
         send_to_char(buf, ch);
      }
      return;
   }

   if (!is_number(arg))
   {
      send_to_char("You must provide a number.\n\r", ch);
      return;
   }

   lines = atoi(arg);

   if (lines == 0)
   {
      send_to_char("Paging disabled.\n\r", ch);
      ch->lines = 0;
      return;
   }

   if (lines < 10 || lines > 100)
   {
      send_to_char("You must provide a reasonable number.\n\r", ch);
      return;
   }

   sprintf(buf,  "Scroll set to %d lines.\n\r", lines);
   send_to_char(buf, ch);
   ch->lines = lines - 2;
}

/* RT does socials */
void do_socials(CHAR_DATA *ch, char *argument)
{
   char buf[MAX_STRING_LENGTH];
   int iSocial;
   int col;

   col = 0;

   for (iSocial = 0; social_table[iSocial].name[0] != '\0'; iSocial++)
   {
      sprintf(buf,  "%-12s", social_table[iSocial].name);
      send_to_char(buf, ch);
      if (++col % 6 == 0)
      send_to_char("\n\r", ch);
   }

   if ( col % 6 != 0)
   send_to_char("\n\r", ch);
   return;
}



/* RT Commands to replace news, motd, imotd, etc from ROM */

void do_motd(CHAR_DATA *ch, char *argument)
{
   do_help(ch, "motd");
}

void do_imotd(CHAR_DATA *ch, char *argument)
{
   do_help(ch, "imotd");
}

void do_rules(CHAR_DATA *ch, char *argument)
{
   do_help(ch, "rules");
}

void do_story(CHAR_DATA *ch, char *argument)
{
   do_help(ch, "story");
}

char* center(char* input)
{
   static char buf[MAX_STRING_LENGTH];
   char format[MAX_INPUT_LENGTH];
   int space;

   if (strlen(input) >= 80)
   {
      space = 0;
   }
   else
   {
      space = 40 + (strlen(input) / 2);
   }
   sprintf
   (
      format,
      "%%%ds\n\r",
      space
   );
   sprintf(buf, format, input);
   return buf;
}

char* level_string(int level)
{
   static char buf[MAX_STRING_LENGTH];
   char format[MAX_INPUT_LENGTH];
   char buf2[MAX_INPUT_LENGTH];
   int counter;
   int length;
   char* name;

   switch(level)
   {
      case (IMPLEMENTOR):
      {
         name = "Implementors";
         break;
      }
      case (CREATOR):
      {
         name = "Creators";
         break;
      }
      case (SUPREME):
      {
         name = "Supremacies";
         break;
      }
      case (DEITY):
      {
         name = "Deities";
         break;
      }
      case (GOD):
      {
         name = " Gods ";
         break;
      }
      case (IMMORTAL):
      {
         name = "Immortals";
         break;
      }
      case (DEMI):
      {
         name = "Demi-Gods";
         break;
      }
      case (ANGEL):
      {
         name = "Angels";
         break;
      }
      case (AVATAR):
      {
         name = "Avatars";
         break;
      }
      default:
      {
         return "";
      }
   }
   length = strlen(name);
   buf[0] = '\0';
   length /= 2;
   length += 2;
   buf2[0] = '\0';
   for (counter = 0; counter < length; counter++)
   {
      strcat(buf2, "-");
      strcat(buf2, "=");
   }
   strcat(buf2, "-");
   strcat(buf, center(buf2));
   buf2[0] = '\0';
   strcat(buf2, "-=- ");
   strcat(buf2, name);
   if (strlen(name) % 2)
   {
      strcat(buf2, " -=-");
   }
   else
   {
      strcat(buf2, "  -=-");
   }
   strcat(buf, center(buf2));
   buf2[0] = '\0';
   sprintf
   (
      format,
      "-=-%%%ds[%d]%%%ds-=-",
      (strlen(name) - 4) / 2,
      level,
      (strlen(name) - 4) / 2 + 1
   );
   sprintf(buf2, format, "", "");
   strcat(buf, center(buf2));
   buf2[0] = '\0';
   for (counter = 0; counter < length; counter++)
   {
      strcat(buf2, "-");
      strcat(buf2, "=");
   }
   strcat(buf2, "-");
   strcat(buf, center(buf2));
   strcat(buf, "\n\r");
   return buf;
}

char* add_list_imm(int level, int min_level, int max_level, bool active, WORSHIP_TYPE* worship, bool branding)
{
   static char buf[MAX_STRING_LENGTH];
   char buf2[MAX_INPUT_LENGTH];
   OBJ_INDEX_DATA* brand;
   char* brand_desc;
   char* find;

   buf[0] = '\0';
   if (!str_cmp(worship->name, "Malcor"))
   {
      /*
      Prevent Malcor (site admin) from showing up on wizlist
      */
      return buf;
   }
   if (worship->wiz_info.level == 0)
   {
      /*
      worship->wiz_info.level = LEVEL_IMMORTAL;
      */
      sprintf
      (
         buf2,
         "%s (IMMORTAL)'s level is <0> in worship table",
         worship->name
      );
      bug(buf2, 0);
      /* Imms without pfiles. */
   }
   if
   (
      worship->wiz_info.level == level &&
      level >= min_level &&
      level <= max_level &&
      (
         (
            active &&
            (
               (
                  level == MAX_LEVEL &&
                  worship->wiz_info.trust == MAX_LEVEL
               ) ||
               (
                  worship->wiz_info.stored == FALSE &&
                  (
                     current_time - worship->wiz_info.login
                  ) < 3600 * 24 * 30
               )
            )
         ) ||
         (
            !active &&
            (
               level != MAX_LEVEL ||
               worship->wiz_info.trust != MAX_LEVEL
            ) &&
            (
               worship->wiz_info.stored ||
               (
                  current_time - worship->wiz_info.login
               ) >= 3600 * 24 * 30
            )
         )
      )
   )
   {
      sprintf
      (
         buf2,
         "%s%s",
         worship->name,
         (
            worship->long_name_append ?
            worship->long_name_append :
            ""
         )
      );
      strcat(buf, center(buf2));
      sprintf
      (
         buf2,
         "(%s)",
         (
            worship->wiz_info.email ?
            worship->wiz_info.email :
            "email currently unavailable"
         )
      );
      strcat(buf, center(buf2));
      if (branding)
      {
         brand = get_obj_index(worship->vnum_brand);
         if (brand == NULL)
         {
            buf[0] = '\0';
            return buf;
         }
         /* Has a brand! */
         brand_desc = brand->short_descr;
         /* Remove auras of all kind, just keep desc */
         for (find = brand_desc; *find; find++)
         {
            switch (*find)
            {
               default:
               {
                  break;
               }
               case (')'):
               case (']'):
               case ('>'):
               case ('}'):
               {
                  do
                  {
                     find++;
                  } while (*find == ' ');
                  brand_desc = find;
               }
            }
         }
         strcat(buf, center(worship->beliefs));
         sprintf(buf2, "Brand of %s", brand_desc);
         strcat(buf, center(buf2));
      }
      strcat(buf, "\n\r");
   }
   return buf;
}

void do_list_imms(CHAR_DATA* ch, char* argument, bool active)
{
   WORSHIP_TYPE* worship;
   BUFFER* buffer;
   char arg[MAX_INPUT_LENGTH];
   int level;
   int god;
   int min_level = LEVEL_IMMORTAL;
   int max_level = MAX_LEVEL;
   bool found;
   char* add;
   bool branding;

   if (!str_cmp(argument, "help"))
   {
      if (active)
      {
         send_to_char
         (
            "Syntax:\n\r"
            " wizlist\n\r"
            " wizlist [level]\n\r"
            " wizlist [min] [max]\n\r"
            " wizlist branding\n\r"
            " wizlist branding [level]\n\r"
            " wizlist branding [min] [max]\n\r"
            " If level is given, show only immortals of that level.\n\r"
            " If min and max are given, show immortals within the range.\n\r"
            " 'Branding' will show only branding imms.\n\r",
            ch
         );
      }
      else
      {
         send_to_char
         (
            "Syntax:\n\r"
            " retired\n\r"
            " retired [level]\n\r"
            " retired [min] [max]\n\r"
            " If level is given, show only immortals of that level.\n\r"
            " If min and max are given, show immortals within the range.\n\r",
            ch
         );
      }
      return;
   }
   argument = one_argument(argument, arg);
   if
   (
      active &&
      UPPER(arg[0]) == 'B' &&
      !str_prefix(arg, "branding")
   )
   {
      branding = TRUE;
      argument = one_argument(argument, arg);
      min_level = IMMORTAL;
   }
   else
   {
      branding = FALSE;
   }
   if (arg[0] != '\0')
   {
      if (is_number(arg))
      {
         min_level = atoi(arg);
         argument = one_argument(argument, arg);
         if (arg[0] != '\0')
         {
            if (is_number(arg))
            {
               max_level = atoi(arg);
            }
            else
            {
               do_list_imms(ch, "help", active);
               return;
            }
         }
         else
         {
            max_level = min_level;
         }
      }
      else
      {
         do_list_imms(ch, "help", active);
         return;
      }
   }
   if
   (
      branding &&
      min_level < IMMORTAL
   )
   {
      sprintf(arg, "Immortals cannot brand before level %d.\n\r", IMMORTAL);
      send_to_char(arg, ch);
      return;
   }
   buffer = new_buf();
   add_buf(buffer, center("- =============================== -"));
   add_buf(buffer, center("-= The Immortals of the Dark Mists =-"));
   add_buf(buffer, center("- =============================== -"));
   add_buf(buffer, "\n\r");
   if (!active)
   {
      add_buf
      (
         buffer,
         "The following Immortals are either gone or semi-retired from"
         " Darkmists.\n\r\n\r"
      );
   }
   else if (branding)
   {
      add_buf
      (
         buffer,
         "The following Immortals are actively branding.\n\r\n\r"
      );
   }
   for (level = MAX_LEVEL; level > LEVEL_HERO; level--)
   {
      found = FALSE;
      for (god = 0; worship_table[god].name != NULL; god++)
      {
         worship = &worship_table[god];
         if
         (
            branding &&
            !worship->branding
         )
         {
            continue;
         }
         add = add_list_imm
         (
            level,
            min_level,
            max_level,
            active,
            worship,
            branding
         );
         if (add[0] != '\0')
         {
            if (!found)
            {
               found = TRUE;
               add_buf(buffer, level_string(level));
            }
            add_buf(buffer, add);
         }
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
         worship = &temp_worship_table[god];
         if
         (
            branding &&
            !worship->branding
         )
         {
            continue;
         }
         add = add_list_imm
         (
            level,
            min_level,
            max_level,
            active,
            worship,
            branding
         );
         if (add[0] != '\0')
         {
            if (!found)
            {
               found = TRUE;
               add_buf(buffer, level_string(level));
            }
            add_buf(buffer, add);
         }
      }
   }
   page_to_char(buf_string(buffer), ch);
   free_buf(buffer);
}

void do_wizlist(CHAR_DATA *ch, char *argument)
{
   do_list_imms(ch, argument, TRUE);
   return;
}

/* Added by Ceran for our retired imms..like me :P */
void do_retired(CHAR_DATA *ch, char *argument)
{
   do_list_imms(ch, argument, FALSE);
   return;
}

/*
   RT this following section holds all the auto commands from ROM, as well as
   replacements for config
*/

void do_autolist(CHAR_DATA *ch, char *argument)
{
   /* lists most player flags */
   if (IS_NPC(ch))
   return;

   send_to_char("   action     status\n\r", ch);
   send_to_char("---------------------\n\r", ch);

   send_to_char("autoassist     ", ch);
   if (IS_SET(ch->act, PLR_AUTOASSIST))
   send_to_char("ON\n\r", ch);
   else
   send_to_char("OFF\n\r", ch);

   send_to_char("autoexit       ", ch);
   if (IS_SET(ch->act, PLR_AUTOEXIT))
   send_to_char("ON\n\r", ch);
   else
   send_to_char("OFF\n\r", ch);

   send_to_char("autogold       ", ch);
   if (IS_SET(ch->act, PLR_AUTOGOLD))
   send_to_char("ON\n\r", ch);
   else
   send_to_char("OFF\n\r", ch);

   send_to_char("autoloot       ", ch);
   if (IS_SET(ch->act, PLR_AUTOLOOT))
   send_to_char("ON\n\r", ch);
   else
   send_to_char("OFF\n\r", ch);

   send_to_char("autosac        ", ch);
   if (IS_SET(ch->act, PLR_AUTOSAC))
   send_to_char("ON\n\r", ch);
   else
   send_to_char("OFF\n\r", ch);

   send_to_char("autosplit      ", ch);
   if (IS_SET(ch->act, PLR_AUTOSPLIT))
   send_to_char("ON\n\r", ch);
   else
   send_to_char("OFF\n\r", ch);

   send_to_char("compact mode   ", ch);
   if (IS_SET(ch->comm, COMM_COMPACT))
   send_to_char("ON\n\r", ch);
   else
   send_to_char("OFF\n\r", ch);

   send_to_char("see surnames   ", ch);
   if (IS_SET(ch->comm2, COMM_SEE_SURNAME))
   {
      send_to_char("ON\n\r", ch);
   }
   else
   {
      send_to_char("OFF\n\r", ch);
   }

   send_to_char("prompt         ", ch);
   if (IS_SET(ch->comm, COMM_PROMPT))
   send_to_char("ON\n\r", ch);
   else
   send_to_char("OFF\n\r", ch);

   send_to_char("color          ", ch);
   if (IS_SET(ch->comm, COMM_ANSI))
   send_to_char("ON\n\r", ch);
   else
   send_to_char("OFF\n\r", ch);

   send_to_char("reduced color  ", ch);
   if (IS_SET(ch->comm2, COMM_REDUCED_COLOR))
   send_to_char("ON\n\r", ch);
   else
   send_to_char("OFF\n\r", ch);

   send_to_char("combine items  ", ch);
   if (IS_SET(ch->comm, COMM_COMBINE))
   send_to_char("ON\n\r", ch);
   else
   send_to_char("OFF\n\r", ch);
/*
MUSIC_DISABLED
   send_to_char("music          ", ch);
   if (IS_SET(ch->comm2, COMM_MUSIC))
   {
      send_to_char("ON\n\r", ch);
      if (IS_SET(ch->comm2, COMM_MUSIC_DOWNLOAD))
      {
         send_to_char("Downloading music automatically.\n\r", ch);
      }
      else
      {
         send_to_char("Not downloading music.\n\r", ch);
      }
   }
   else
   {
      send_to_char("OFF\n\r", ch);
   }
*/
   if (ch->pcdata->special == SUBCLASS_TEMPORAL_MASTER)
   {
      send_to_char("tick sensing   ", ch);
      if (IS_SET(ch->wiznet, WIZ_TICKS))
      send_to_char("ON  (notick to toggle)\n\r", ch);
      else
      send_to_char("OFF (notick to toggle)\n\r", ch);
   }
   if (IS_IMMORTAL(ch))
   {
      send_to_char("item limits    ", ch);
      if (IS_SET(ch->comm, COMM_LIMITS))
      send_to_char("ON\n\r", ch);
      else
      send_to_char("OFF\n\r", ch);
   }

   if (IS_SET(ch->act, PLR_NOSUMMON))
   send_to_char("You cannot be summoned by anyone out of PK.\n\r", ch);
   else
   send_to_char("You can be summoned by anyone.\n\r", ch);

   if (IS_SET(ch->act, PLR_NOFOLLOW))
   send_to_char("You do not welcome followers.\n\r", ch);
   else
   send_to_char("You accept followers.\n\r", ch);

   if (IS_SET(ch->act, PLR_NO_TRANSFER))
   send_to_char("You do not accept transfered objects from the transfer object spell.\n\r", ch);
   else
   send_to_char("You accept transfered objects from the transfer object spell.\n\r", ch);

}
void do_color(CHAR_DATA *ch, char *argument)
{
   if (IS_NPC(ch))
   return;

   if (IS_SET(ch->comm, COMM_ANSI))
   {
      send_to_char("Color has been removed.\n\r", ch);
      REMOVE_BIT(ch->comm, COMM_ANSI);
   }
   else
   {
      send_to_char("You now see color.\n\r", ch);
      SET_BIT(ch->comm, COMM_ANSI);
   }
}

void do_color_reduce(CHAR_DATA *ch, char *argument)
{
   if (IS_SET(ch->comm2, COMM_REDUCED_COLOR))
   {
      send_to_char("Color has been increased.\n\r", ch);
      REMOVE_BIT(ch->comm2, COMM_REDUCED_COLOR);
   }
   else
   {
      send_to_char("You now see reduced color.\n\r", ch);
      SET_BIT(ch->comm2, COMM_REDUCED_COLOR);
   }
   if (!IS_NPC(ch))
   {
      if (IS_SET(ch->comm, COMM_ANSI))
      {
         send_to_char("Note: color is currently ON.\n\r", ch);
      }
      else
      {
         send_to_char("Note: color is currently OFF.\n\r", ch);
      }
   }
}

void do_autoattack(CHAR_DATA* ch, char* argument)
{
   CHAR_DATA* master;

   if
   (
      !IS_NPC(ch) ||
      !IS_AFFECTED(ch, AFF_CHARM)
   )
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }
   switch (guard_type(ch))
   {
      default:
      {
         return;
      }
      case (GUARD_NORMAL):
      case (GUARD_DEFENDER):
      case (GUARD_MAGUS):
      case (GUARD_CURATE):
      {
         master = ch->master;
         break;
      }
      case (GUARD_SENTINEL):
      {
         master = ch->mprog_target;
         break;
      }
      case (GUARD_SENTRY):
      {
         if (ch->mprog_target != NULL)
         {
            send_to_char
            (
               "Sentries detain criminals, they do not attack them.\n\r",
               ch->mprog_target
            );
         }
         return;
      }
   }
   if (master == NULL)
   {
      send_to_char("Not without a master.\n\r", ch);
      return;
   }
   if (IS_SET(ch->act2, ACT2_AUTOATTACK))
   {
      act("$n will no longer attack criminals on sight.", ch, NULL, master, TO_VICT);
      REMOVE_BIT(ch->act2, ACT2_AUTOATTACK);
   }
   else
   {
      act("$n will now attack criminals on sight.", ch, NULL, master, TO_VICT);
      SET_BIT(ch->act2, ACT2_AUTOATTACK);
   }
}

void do_active_brand(CHAR_DATA* ch, char* argument)
{
   CHAR_DATA* real_ch;
   WORSHIP_TYPE* worship;
   OBJ_INDEX_DATA* brand;

   if (IS_NPC(ch))
   {
      real_ch = ch->desc->original;
   }
   else
   {
      real_ch = ch;
   }
   worship = find_wizi_slot(real_ch->name, &real_ch->pcdata->worship_slot);

   if (IS_SET(real_ch->act2, PLR_ACTIVELY_BRANDING))
   {
      /* Allow removal, for case of demotions or bugs.  */
      send_to_char("You are no longer actively branding.\n\r", ch);
      REMOVE_BIT(ch->act2, PLR_ACTIVELY_BRANDING);
      worship->branding = FALSE;
   }
   else
   {
      if (real_ch->level < IMMORTAL)
      {
         send_to_char("You are not powerful enough to brand anyone.\n\r", ch);
         return;
      }
      if
      (
         worship->vnum_brand == BRAND_NONE ||
         (
            brand = get_obj_index(worship->vnum_brand)
         ) == NULL
      )
      {
         send_to_char("You must have a brand to actively brand.", ch);
         return;
      }
      send_to_char("You are now actively branding.\n\r", ch);
      SET_BIT(ch->act2, PLR_ACTIVELY_BRANDING);
      worship->branding = TRUE;
   }
   save_char_obj(real_ch);
}

void do_autoassist(CHAR_DATA *ch, char *argument)
{
   if (IS_NPC(ch))
   return;

   if (IS_SET(ch->act, PLR_AUTOASSIST))
   {
      send_to_char("Autoassist removed.\n\r", ch);
      REMOVE_BIT(ch->act, PLR_AUTOASSIST);
   }
   else
   {
      send_to_char("You will now assist when needed.\n\r", ch);
      SET_BIT(ch->act, PLR_AUTOASSIST);
   }
}

void do_autoexit(CHAR_DATA *ch, char *argument)
{
   if (IS_NPC(ch))
   return;

   if (IS_SET(ch->act, PLR_AUTOEXIT))
   {
      send_to_char("Exits will no longer be displayed.\n\r", ch);
      REMOVE_BIT(ch->act, PLR_AUTOEXIT);
   }
   else
   {
      send_to_char("Exits will now be displayed.\n\r", ch);
      SET_BIT(ch->act, PLR_AUTOEXIT);
   }
}

void do_music(CHAR_DATA* ch, char* argument)
{
   if (IS_NPC(ch))
   {
      return;
   }

   if (IS_SET(ch->comm2, COMM_MUSIC))
   {
      send_to_char("You will no longer hear music.\n\r", ch);
      REMOVE_BIT(ch->comm2, COMM_MUSIC);
      music_off(ch);
   }
   else
   {
      send_to_char("You will now hear music.\n\r", ch);
      SET_BIT(ch->comm2, COMM_MUSIC);
      if (str_cmp(argument, "auto"))
      {
         music_on(ch);
      }
   }
}

void do_music_dl(CHAR_DATA* ch, char* argument)
{
   if (IS_NPC(ch))
   {
      return;
   }

   if (!IS_SET(ch->comm2, COMM_MUSIC))
   {
      /* Turn on music */
      do_music(ch, "");
   }
   if (IS_SET(ch->comm2, COMM_MUSIC_DOWNLOAD))
   {
      send_to_char("You will no longer download music.\n\r", ch);
      REMOVE_BIT(ch->comm2, COMM_MUSIC_DOWNLOAD);
   }
   else
   {
      send_to_char("You will automatically download music now.\n\r", ch);
      SET_BIT(ch->comm2, COMM_MUSIC_DOWNLOAD);
      music_on(ch);
   }
}

void do_autogold(CHAR_DATA *ch, char *argument)
{
   if (IS_NPC(ch))
   return;

   if (IS_SET(ch->act, PLR_AUTOGOLD))
   {
      send_to_char("Autogold removed.\n\r", ch);
      REMOVE_BIT(ch->act, PLR_AUTOGOLD);
   }
   else
   {
      send_to_char("Automatic gold looting set.\n\r", ch);
      SET_BIT(ch->act, PLR_AUTOGOLD);
   }
}

void do_autoloot(CHAR_DATA *ch, char *argument)
{
   if (IS_NPC(ch))
   return;

   if (IS_SET(ch->act, PLR_AUTOLOOT))
   {
      send_to_char("Autolooting removed.\n\r", ch);
      REMOVE_BIT(ch->act, PLR_AUTOLOOT);
   }
   else
   {
      send_to_char("Automatic corpse looting set.\n\r", ch);
      SET_BIT(ch->act, PLR_AUTOLOOT);
   }
}

void do_autosac(CHAR_DATA *ch, char *argument)
{
   if (IS_NPC(ch))
   return;

   if (IS_SET(ch->act, PLR_AUTOSAC))
   {
      send_to_char("Autosacrificing removed.\n\r", ch);
      REMOVE_BIT(ch->act, PLR_AUTOSAC);
   }
   else
   {
      send_to_char("Automatic corpse sacrificing set.\n\r", ch);
      SET_BIT(ch->act, PLR_AUTOSAC);
   }
}

void do_gsndump(CHAR_DATA* ch, char* arg)
{
   bool found = FALSE;
   sh_int sn = 0;
   BUFFER *output;
   char buf [MAX_INPUT_LENGTH];

   /*
   do_gsndump(NULL, "fix") will be called
   periodically by system to keep aware of gsn problems
   */
   if
   (
      ch == NULL &&
      strcmp(arg, "fix")
   )
   {
      bug("gsndump NULL ch", 0);
      return;
   }
   if (arg[0] == '\0')
   {
      send_to_char
      (
         "Syntax:\n\r"
         "gsndump corrupt\n\r"
         "  Shows only corrupt gsn's\n\r"
         "gsndump gsn\n\r"
         "  Shows all entries that have gsn's\n\r"
         "gsndump all\n\r"
         "  Shows all entries in the skill table\n\r"
         "gsndump fix\n\r"
         "  Shows and fixes all corrupt entries in the skill table\n\r"
         "default is corrupt\n\r"
         "\n\r"
         "Running Corrupt:\n\r",
         ch
      );
   }
   if (ch != NULL)
   {
      output = new_buf();
   }
   else
   {
      output = NULL;
   }

   if (!str_cmp("gsn", arg))
   {
      found = FALSE;
      send_to_char("All gsn's:\n\r", ch);
      /* Starts at 0, gsn_reserved COULD be corrupt */
      for (sn = 0; sn < MAX_SKILL; sn++)
      {
         if (skill_table[sn].pgsn != NULL)
         {
            found = TRUE;
            sprintf
            (
               buf,
               "skill '%-20s' gsn = '%-6d' sn = '%-6d'\n\r",
               skill_table[sn].name,
               skill_table[sn].pgsn[0],
               sn
            );
            add_buf(output, buf);
         }
      }
   }
   else if (!str_cmp("fix", arg))
   {
      /* Loops starting from 0 in case 0 is corrupt */
      for (sn = 0; sn < MAX_SKILL; sn++)
      {
         if
         (
            skill_table[sn].pgsn != NULL &&
            skill_table[sn].pgsn[0] != sn
         )
         {
            if (!found)
            {
               log_string("Gsn Corruption Found");
            }
            found = TRUE;
            sprintf
            (
               buf,
               "Fixed skill '%-20s' old gsn = '%-6d' sn = '%-6d'\n\r",
               skill_table[sn].name,
               skill_table[sn].pgsn[0],
               sn
            );
            log_string(buf);
            wiznet(buf, NULL, NULL, WIZ_BUGS, 0, 0);
            if (ch != NULL)
            {
               add_buf(output, buf);
            }
            *skill_table[sn].pgsn = sn;
         }
      }
   }
   else if (!str_cmp("all", arg))
   {
      found = TRUE;
      send_to_char("All skills:\n\r", ch);
      /* Starts at 0 intentionally, show reserved */
      for (sn = 0; sn < MAX_SKILL; sn++)
      {
         if (skill_table[sn].pgsn != NULL)
         {
            sprintf
            (
               buf,
               "skill '%-20s' gsn = '%-6d' sn = '%-6d'\n\r",
               skill_table[sn].name,
               skill_table[sn].pgsn[0],
               sn
            );
            add_buf(output, buf);
         }
         else
         {
            sprintf
            (
               buf,
               "skill '%-20s' gsn = '%-6s' sn = '%-6d'\n\r",
               skill_table[sn].name,
               "(none)",
               sn
            );
            add_buf(output, buf);
         }
      }
   }
   else
   {
      found = FALSE;
      send_to_char("Corrupt gsn's:\n\r", ch);
      /* Starts at 0 intentionally, show reserved */
      for (sn = 0; sn < MAX_SKILL; sn++)
      {
         if (skill_table[sn].pgsn != NULL)
         {
            if (skill_table[sn].pgsn[0]!=sn)
            {
               found = TRUE;
               sprintf
               (
                  buf,
                  "skill '%-20s' gsn = '%-6d' sn = '%-6d'\n\r",
                  skill_table[sn].name,
                  skill_table[sn].pgsn[0],
                  sn
               );
               log_string(buf);
               add_buf(output, buf);
            }
         }
      }
   }
   if (ch != NULL)
   {
      if (!found)
      {
         send_to_char("Nothing found.\n\r", ch);
      }
      else
      {
         page_to_char(buf_string(output), ch);
      }
      free_buf(output);
   }
   return;
}

void do_autosplit(CHAR_DATA *ch, char *argument)
{
   if (IS_NPC(ch))
   return;

   if (IS_SET(ch->act, PLR_AUTOSPLIT))
   {
      send_to_char("Autosplitting removed.\n\r", ch);
      REMOVE_BIT(ch->act, PLR_AUTOSPLIT);
   }
   else
   {
      send_to_char("Automatic gold splitting set.\n\r", ch);
      SET_BIT(ch->act, PLR_AUTOSPLIT);
   }
}

void do_brief(CHAR_DATA *ch, char *argument)
{
   if (IS_SET(ch->comm, COMM_BRIEF))
   {
      send_to_char("Full descriptions activated.\n\r", ch);
      REMOVE_BIT(ch->comm, COMM_BRIEF);
   }
   else
   {
      send_to_char("Short descriptions activated.\n\r", ch);
      SET_BIT(ch->comm, COMM_BRIEF);
   }
}

void do_compact(CHAR_DATA *ch, char *argument)
{
   if (IS_SET(ch->comm, COMM_COMPACT))
   {
      send_to_char("Compact mode removed.\n\r", ch);
      REMOVE_BIT(ch->comm, COMM_COMPACT);
   }
   else
   {
      send_to_char("Compact mode set.\n\r", ch);
      SET_BIT(ch->comm, COMM_COMPACT);
   }
}

void do_show(CHAR_DATA *ch, char *argument)
{
   if (IS_SET(ch->comm, COMM_SHOW_AFFECTS))
   {
      send_to_char("Affects will no longer be shown in score.\n\r", ch);
      REMOVE_BIT(ch->comm, COMM_SHOW_AFFECTS);
   }
   else
   {
      send_to_char("Affects will now be shown in score.\n\r", ch);
      SET_BIT(ch->comm, COMM_SHOW_AFFECTS);
   }
}

void do_prompt(CHAR_DATA *ch, char *argument)
{
   char buf[MAX_STRING_LENGTH];

   if ( argument[0] == '\0' )
   {
      if (IS_SET(ch->comm, COMM_PROMPT))
      {
         send_to_char("You will no longer see prompts.\n\r", ch);
         REMOVE_BIT(ch->comm, COMM_PROMPT);
      }
      else
      {
         send_to_char("You will now see prompts.\n\r", ch);
         SET_BIT(ch->comm, COMM_PROMPT);
      }
      return;
   }
   return;
   if ( !strcmp( argument, "all" ) )
   strcpy( buf, "<%hhp %mmn %vmv> ");
   else
   {
      if ( strlen(argument) > 50 )
      argument[50] = '\0';
      strcpy( buf, argument );
      smash_tilde( buf );
      if (str_suffix("%c", buf))
      strcat(buf, " ");

   }

   free_string( ch->prompt );
   ch->prompt = str_dup( buf );
   sprintf(buf,  "Prompt set to %s\n\r", ch->prompt );
   send_to_char(buf, ch);
   return;
}

void do_combine(CHAR_DATA *ch, char *argument)
{
   if (IS_SET(ch->comm, COMM_COMBINE))
   {
      send_to_char("Long inventory selected.\n\r", ch);
      REMOVE_BIT(ch->comm, COMM_COMBINE);
   }
   else
   {
      send_to_char("Combined inventory selected.\n\r", ch);
      SET_BIT(ch->comm, COMM_COMBINE);
   }
}

void do_nofollow(CHAR_DATA *ch, char *argument)
{
   CHAR_DATA* wch;
   CHAR_DATA* wch_next;
   if (IS_NPC(ch))
   return;

   if (IS_AFFECTED(ch, AFF_CHARM))
   {
      send_to_char("Now why would you want to leave your master?\n\r",
      ch);
      return;
   }

   if (IS_SET(ch->act, PLR_NOFOLLOW))
   {
      send_to_char("You now accept followers.\n\r", ch);
      REMOVE_BIT(ch->act, PLR_NOFOLLOW);
   }
   else
   {
      send_to_char("You no longer accept followers.\n\r", ch);
      SET_BIT(ch->act, PLR_NOFOLLOW);
      if (ch->master != NULL)
      {
         if (ch->master->pet == ch)
         {
            ch->master->pet = NULL;
         }
         stop_follower(ch);
      }

      ch->leader = NULL;

      for ( wch = char_list; wch != NULL; wch = wch_next )
      {
         /* Need this new line to traverse list properly */
         wch_next = wch->next;

         if
         (
            (
               wch->master != ch &&
               wch->leader != ch
            ) ||
            IS_AFFECTED(wch, AFF_CHARM)
         )
         {
            continue;
         }
         if (wch->life_lined == ch)
         {
            act("Your lifeline to $N breaks.", ch, NULL, wch, TO_CHAR);
            wch->life_lined = NULL;
         }
         if (wch->leader == ch)
         {
            wch->leader = wch;
         }
         if (wch->master == ch)
         {
            stop_follower(wch);
         }
      }
   }
}

void do_nosummon(CHAR_DATA *ch, char *argument)
{
   if (IS_NPC(ch))
   {
      if (IS_SET(ch->imm_flags, IMM_SUMMON))
      {
         send_to_char("You may now be summoned by anyone.\n\r", ch);
         REMOVE_BIT(ch->imm_flags, IMM_SUMMON);
      }
      else
      {
         send_to_char("You may no longer be summoned.\n\r", ch);
         SET_BIT(ch->imm_flags, IMM_SUMMON);
      }
   }
   else
   {
      if (IS_SET(ch->act, PLR_NOSUMMON))
      {
         send_to_char("You may now be summoned by anyone.\n\r", ch);
         REMOVE_BIT(ch->act, PLR_NOSUMMON);
      }
      else
      {
         send_to_char("You may only be summoned by those in PK.\n\r", ch);
         SET_BIT(ch->act, PLR_NOSUMMON);
      }
   }
}

void do_glance( CHAR_DATA *ch, char *argument )
{
   CHAR_DATA *victim;

   if ( ch->desc == NULL )
   return;

   if ( argument[0] == '\0' )
   {
      send_to_char( "Glance at whom?\n\r", ch );
      return;
   }

   if ( ch->position < POS_SLEEPING )
   {
      send_to_char( "You can't see anything but stars!\n\r", ch );
      return;
   }

   if ( ch->position == POS_SLEEPING )
   {
      send_to_char( "You can't see anything, you're sleeping!\n\r", ch );
      return;
   }

   if ( !check_blind( ch ) )
   return;

   if ( (victim = get_char_room(ch, argument)) == NULL )
   {
      send_to_char( "That person isn't here.\n\r", ch );
      return;
   }
   if (is_affected(victim, gsn_cloak_form))
   {
      send_to_char("The figure is buried deep within a dark colored cloak.\n\r", ch);
      return;
   }

   show_char_to_char_2(victim, ch);
   return;
}

void do_look(CHAR_DATA* ch, char* argument)
{
   do_observe(ch, argument, LOOK_NORM);
}

void do_examine(CHAR_DATA* ch, char* argument)
{
   do_observe(ch, argument, LOOK_EXAMINE);
}

void do_observe(CHAR_DATA* ch, char* argument, int type)
{
   char buf  [MAX_STRING_LENGTH];
   char arg1 [MAX_INPUT_LENGTH];
   char arg2 [MAX_INPUT_LENGTH];
   char arg3 [MAX_INPUT_LENGTH];
   ROOM_INDEX_DATA *room1;
   ROOM_INDEX_DATA *room2;
   ROOM_AFFECT_DATA *raf;
   EXIT_DATA *pexit;
   CHAR_DATA *victim;
   OBJ_DATA *obj = NULL;
   char *pdesc;
   char* find;
   int door;
   bool found = FALSE;
   int number, count;
   extern char* const dir_name[];

   if (ch->desc == NULL)
   {
      return;
   }

   if
   (
      argument[0] == '\0' &&
      type == LOOK_EXAMINE
   )
   {
      send_to_char("Examine what?\n\r", ch);
      return;
   }
   if (ch->position < POS_SLEEPING)
   {
      send_to_char("You cannot see anything but stars!\n\r", ch);
      return;
   }

   if (ch->position == POS_SLEEPING)
   {
      send_to_char("You cannot see anything, you are sleeping!\n\r", ch);
      return;
   }
   if (!check_blind(ch))
   {
      if
      (
         (
            type == LOOK_AUTO ||
            (
               argument[0] == '\0' &&
               type == LOOK_NORM
            )
         ) &&
         is_affected(ch, gsn_blind_fighting)
      )
      {
         if
         (
            IS_NPC(ch) ||
            IS_SET(ch->act, PLR_AUTOEXIT)
         )
         {
            send_to_char("\n\rYou sense the exits:\n\r", ch);
            do_exits(ch, "auto");
         }
         show_char_to_char(ch->in_room->people, ch);
      }
      return;
   }
   argument = one_argument(argument, arg1);
   argument = one_argument(argument, arg2);
   number = number_argument(arg1, arg3);
   if
   (
      type != LOOK_EXAMINE &&
      is_affected(ch, gsn_delusions)
   )
   {
      send_to_char
      (
         "The Far Recesses of Your Mind\n\r"
         "The air around you grows cloudly, a violet cloud which\n\r"
         "changes to various different hues.  At first, it begins as\n\r"
         "a fine mist, like the spray which rises off the lakes in the\n\r"
         "early morning.  It then grows denser until you are\n\r"
         "surrounded by a heavy violet fog and unable to see more\n\r"
         "then a few inches in any direction.\n\r"
         "\n\r[Exits: north east south west up down]\n\r",
         ch
      );
      return;
   }
   if
   (
      !IS_NPC(ch) &&
      !IS_SET(ch->act, ACT_HOLYLIGHT) &&
      (
         is_affected_room(ch->in_room, gsn_globe_darkness) ||
         room_is_dark(ch->in_room)
      ) &&
      !IS_AFFECTED(ch, AFF_INFRARED)
   )
   {
      send_to_char("It is pitch black ... \n\r", ch);
      show_char_to_char(ch->in_room->people, ch);
      return;
   }
   if
   (
      is_affected_room(ch->in_room, gsn_hall_mirrors) &&
      (
         (
            arg1[0] == '\0' &&
            type == LOOK_NORM
         ) ||
         type == LOOK_AUTO
      ) &&
      !IS_NPC(ch) &&
      !IS_SET(ch->act, ACT_HOLYLIGHT) &&
      ch->pcdata->special != SUBCLASS_ILLUSIONIST
   )
   {
      send_to_char
      (
         "\n\r"
         " You are standing on a reflective surface, and every direction you"
         " look,\n\r"
         "you see a reflection of yourself and your surroundings.\n\r\n\r",
         ch
      );
      show_char_to_char(ch->in_room->people, ch);
      return;
   }

   count = 0;

   if
   (
      (
         arg1[0] == '\0' &&
         type == LOOK_NORM
      ) ||
      type == LOOK_AUTO
   )
   {
      /* 'look' or 'look auto' */
      send_to_char( ch->in_room->name, ch );

      if (IS_IMMORTAL(ch) && (IS_NPC(ch) || IS_SET(ch->act, ACT_HOLYLIGHT)))
      {
         sprintf(buf,  " [Room %d]", ch->in_room->vnum);
         send_to_char(buf, ch);
      }

      send_to_char( "\n\r", ch );

      if
      (
         (
            arg1[0] == '\0' &&
            type != LOOK_AUTO
         ) ||
         (
            IS_NPC(ch) ||
            !IS_SET(ch->comm, COMM_BRIEF)
         )
      )
      {
         int max;

         if (type == LOOK_AUTO)
         {
            max = 400;
         }
         else
         {
            max = 200;
         }
         send_to_char( "  ", ch);
         send_to_char( ch->in_room->description, ch );
         /* show simulacrum */
         for (victim = ch->in_room->people; victim; victim = victim->next_in_room)
         {
            if
            (
               !is_same_group(victim, ch) &&
               is_affected(victim, gsn_simulacrum) &&
               number_range(1, max) <= ch->level
            )
            {
               send_to_char
               (
                  "You detect a small distortion in the fabric of reality.\n\r",
                  ch
               );
               break;
            }
         }
      }
      if (is_affected_room(ch->in_room, gsn_water_spout))
      {
         send_to_char("\n\rA large water spout twists and turns throwing waves everywhere!\n\r", ch);

      }
      if (is_affected_room(ch->in_room, gsn_water_of_life))
      {
         send_to_char("\n\rA strange thick forest grows rapidly nurtured by a light mist.\n\r", ch);
      }
      if (is_affected_room(ch->in_room, gsn_spring_rains))
      {
         send_to_char("\n\rA gentle soothing rain falls here.\n\r", ch);
      }
      if (is_affected_room(ch->in_room, gsn_meteor_storm))
      {
         send_to_char("\n\rThe sky is ominously dark here.\n\r", ch);
      }
      if (is_affected_room(ch->in_room, gsn_storm))
      {
         send_to_char("\n\rAn intense and ominous storm seems to hover right over you!\n\r", ch);
      }

      if (is_affected_room(ch->in_room, gsn_raging_fire))
      {
         send_to_char( "\n\rThe forest crackles and burns around you!\n\r", ch);
      }
      if (is_affected_room(ch->in_room, gsn_tornado))
      {
         send_to_char( "\n\rThe black tornadic storm is raging above!\n\r", ch);
      }

      if (is_affected_room(ch->in_room, gsn_trapmaking)
      && ( IS_IMMORTAL(ch) || IS_AFFECTED(ch, AFF_ACUTE_VISION) ))
      {
         for ( raf = ch->in_room->affected; raf != NULL; raf = raf->next )
         {
            if (raf->type == gsn_trapmaking)
            {
               switch (raf->modifier)
               {
                  case 0: send_to_char("You notice a hunter's vine trap here.\n\r", ch); break;
                  case 1: send_to_char("You notice a hunter's snare here.\n\r", ch); break;
                  case 2: send_to_char("You notice a log trap here.\n\r", ch); break;
                  case 3: send_to_char("You notice a bunch of sharp punjie sticks here.\n\r", ch); break;
                  default:
                  {
                     break;
                  }
               }
            }
         }
      }
      if (is_affected_room(ch->in_room, gsn_meteor))
      {
         send_to_char("\n\rThe ground is scorched black and cracked.\n\r", ch);
      }
      if (is_affected_room(ch->in_room, gsn_flood_room))
      {
         if
         (
            IS_AFFECTED(ch, AFF_BLIND) &&
            !IS_FLYING(ch)
         )
         {
            send_to_char("Your feet feel strangely wet.\n\r", ch);
         }
         else
         {
            send_to_char("\n\rThere is water everywhere!\n\r", ch);
         }
      }
      if (is_affected_room(ch->in_room, gsn_fog_conceilment))
      {
         send_to_char("\n\rA thick fog is here shrouding all the surroundings in mystery.\n\r", ch);
      }

      if (is_affected_room(ch->in_room, gsn_illusionary_wall))
      {
         if (IS_IMMORTAL(ch) || (!IS_NPC(ch) && ch->pcdata->special == SUBCLASS_ILLUSIONIST))
         {
            send_to_char("\n\rAn illusionary wall is here hiding an exit.\n\r", ch);
         }
      }

      if (is_affected_room(ch->in_room, gsn_corrupt("hall of mirrors", &gsn_hall_mirrors)))
      {
         send_to_char("\n\rAn illusion of a Hall of Mirrors is here.\n\r", ch);
      }

      if ( IS_NPC(ch) || IS_SET(ch->act, PLR_AUTOEXIT) )
      {
         send_to_char("\n\r", ch);
         do_exits( ch, "auto" );
      }

      show_list_to_char( ch->in_room->contents, ch, FALSE, FALSE );
      show_char_to_char( ch->in_room->people,   ch );

      /* Ancient Target Tracking */
      if (!IS_NPC(ch) && is_affected(ch, gsn_target) && (number_range(1, 2) == 1)
      && ch->pcdata->target != NULL)
      {
         sprintf(buf,  "The prey's tracks lead ");
         switch (get_trackdir(ch, ch->pcdata->target->id)-1)
         {
            case DIR_NORTH: strcat(buf, "north."); break;
            case DIR_EAST: strcat(buf, "east."); break;
            case DIR_SOUTH: strcat(buf, "south."); break;
            case DIR_WEST: strcat(buf, "west."); break;
            case DIR_UP: strcat(buf, "up."); break;
            case DIR_DOWN: strcat(buf, "down."); break;
            default: sprintf(buf,  "You detect no sign of the prey's tracks.");
         }
         strcat(buf, "\n\r");
         send_to_char(buf, ch);
      }

      /* LIGHT Sense Evil Tracking */
      if
      (
         !IS_NPC(ch) &&
         is_affected(ch, gsn_sense_evil)
      )
      {
         DESCRIPTOR_DATA *d;
         CHAR_DATA *trackch;
         char* pronounName;
         int dir;

         for (d = descriptor_list; d != NULL; d = d->next)
         {
            if
            (
               d->connected != CON_PLAYING ||
               !can_see(ch, d->character)
            )
            {
               continue;
            }
            trackch = (d->original) ? d->original : d->character;
            if
            (
               (
                  !IS_EVIL(trackch) &&
                  !IS_SET(trackch->act, PLR_EVIL)
               ) ||
               (
                  IS_IMMORTAL(trackch) &&
                  !can_see(ch, trackch) &&
                  wizi_to(trackch, ch)
               ) ||
               !is_in_pk(ch, trackch) ||
               (
                  number_percent() > get_skill(ch, gsn_sense_evil)
               )
            )
            {
               continue;
            }
            dir = get_trackdir(ch, trackch->id) - 1;
            if
            (
               dir < 0 ||
               dir > 5
            )
            {
               continue;
            }
            switch (UPPER(race_table[trackch->race].name[0]))
            {
               case ('A'):
               case ('E'):
               case ('I'):
               case ('O'):
               case ('U'):
               {
                  pronounName = "an";
                  break;
               }
               default:
               {
                  pronounName = "a";
                  break;
               }
            }
            sprintf
            (
               buf,
               "You smell the evil residue of %s %s's passing going %s.\n\r",
               pronounName,
               race_table[trackch->race].name,
               dir_name[dir]
            );
            send_to_char(buf, ch);
         }
      }
      return;
   }

   if
   (
      !(
         !str_cmp(arg1, "i") ||
         !str_cmp(arg1, "in")  ||
         !str_cmp(arg1, "on") ||
         !str_cmp(arg1, "through")
      ) &&
      (
         victim = get_char_room(ch, arg1)
      ) != NULL
   )
   {
      if (type == LOOK_EXAMINE)
      {
         show_char_to_char_3(victim, ch);
      }
      else
      {
         show_char_to_char_1(victim, ch);
      }
      return;
   }
   obj = NULL;
   pdesc = NULL;
   if
   (
      !(
         !str_cmp(arg1, "i") ||
         !str_cmp(arg1, "in")  ||
         !str_cmp(arg1, "on") ||
         !str_cmp(arg1, "through") ||
         (
            type != LOOK_EXAMINE &&
            arg1[0] != '\0' &&
            (
               !str_prefix(arg1, "north") ||
               !str_prefix(arg1, "east") ||
               !str_prefix(arg1, "south") ||
               !str_prefix(arg1, "weast") ||
               !str_prefix(arg1, "up") ||
               !str_prefix(arg1, "down")
            )
         )
      )
   )
   {
      find = arg3;
      pdesc = NULL;
      for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
      {
         if
         (
            !can_see_obj(ch, obj) ||
            obj->wear_loc == WEAR_NONE
         )
         {
            continue;
         }
         if
         (
            (
               (
                  (
                     pdesc = get_extra_descr(find, obj->extra_descr)
                  ) != NULL
               ) ||
               (
                  (
                     pdesc = get_extra_descr(find, obj->pIndexData->extra_descr)
                  ) != NULL
               ) ||
               (
                  is_name(find, obj->name) &&
                  (
                     pdesc = obj->description
                  ) != NULL
               )
            ) &&
            ++count == number
         )
         {
            break;
         }
         pdesc = NULL;
      }
      if (obj == NULL)
      {
         for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
         {
            if
            (
               !can_see_obj(ch, obj) ||
               obj->wear_loc != WEAR_NONE
            )
            {
               continue;
            }
            if
            (
               (
                  (
                     (
                        pdesc = get_extra_descr(find, obj->extra_descr)
                     ) != NULL
                  ) ||
                  (
                     (
                        pdesc = get_extra_descr(find, obj->pIndexData->extra_descr)
                     ) != NULL
                  ) ||
                  (
                     is_name(find, obj->name) &&
                     (
                        pdesc = obj->description
                     ) != NULL
                  )
               ) &&
               ++count == number
            )
            {
               break;
            }
            pdesc = NULL;
         }
      }
      if (obj == NULL)
      {
         for (obj = ch->in_room->contents; obj != NULL; obj = obj->next_content)
         {
            if (!can_see_obj(ch, obj))
            {
               continue;
            }
            if
            (
               (
                  (
                     (
                        pdesc = get_extra_descr(find, obj->extra_descr)
                     ) != NULL
                  ) ||
                  (
                     (
                        pdesc = get_extra_descr(find, obj->pIndexData->extra_descr)
                     ) != NULL
                  ) ||
                  (
                     is_name(find, obj->name) &&
                     (
                        pdesc = obj->description
                     ) != NULL
                  )
               ) &&
               ++count == number
            )
            {
               break;
            }
            pdesc = NULL;
         }
      }
      if
      (
         obj == NULL &&
         (
            pdesc = get_extra_descr(find, ch->in_room->extra_descr)
         ) &&
         ++count == number
      )
      {
         /* Room extra descs */
         send_to_char(pdesc, ch);
         return;
      }
      if (obj != NULL)
      {
         if
         (
            obj->pIndexData->vnum == 4420 &&  /* Drow compendium */
            ch->race != grn_dark_elf &&
            !IS_IMMORTAL(ch)
         )
         {
            send_to_char("Only the drow may use this compendium.\n\r", ch);
            return;
         }
         if
         (
            obj->pIndexData->vnum == 4432 &&  /* Thief's Manifesto */
            str_cmp(class_table[ch->class].name, "thief") &&
            !IS_IMMORTAL(ch)
         )
         {
            send_to_char
            (
               "This book appears to be written in the code language of the"
               " Thieves Guild.\n\r",
               ch
            );
            return;
         }
         send_to_char(pdesc, ch);
         if (pdesc == obj->description)
         {
            send_to_char("\n\r", ch);
         }
         if
         (
            obj->carried_by == ch &&
            obj->timer > 0 &&
            obj->timer < 15
         )
         {
            act("$p appears to be in a state of disrepair.", ch, obj, ch, TO_CHAR);
         }
         if (type != LOOK_EXAMINE)
         {
            return;
         }
         found = TRUE;
      }
      if (count > 0 && count != number)
      {
         if (count == 1)
         {
            sprintf(buf, "You only see one %s here.\n\r", find);
         }
         else
         {
            sprintf(buf, "You only see %d of those here.\n\r", count);
         }
         send_to_char(buf, ch);
         return;
      }
      /* Continue on */
   }
   if
   (
      type == LOOK_EXAMINE ||
      !str_cmp(arg1, "i") ||
      !str_cmp(arg1, "in")  ||
      !str_cmp(arg1, "on") ||
      !str_cmp(arg1, "through")
   )
   {
      if (!found)
      {
         /* 'look in' */
         if
         (
            type != LOOK_EXAMINE &&
            arg2[0] == '\0'
         )
         {
            send_to_char("Look in what?\n\r", ch);
            return;
         }
         if
         (
            type == LOOK_EXAMINE ||
            (
               obj = get_obj_here(ch, arg2)
            ) == NULL
         )
         {
            send_to_char("You do not see that here.\n\r", ch);
            return;
         }
      }

      switch (obj->item_type)
      {
         default:
         {
            if (type == LOOK_EXAMINE)
            {
               if (obj->pIndexData->vnum == OBJ_VNUM_ENFORCER)
               {
                  do_help(ch, "laws");
               }
               else if
               (
                  obj->item_type == ITEM_WEAPON &&
                  obj->value[0]==WEAPON_ARROWS
               )
               {
                  if (obj->weight > 1)
                  {
                     sprintf
                     (
                        buf,
                        "There are %d arrows remaining.\n\r",
                        obj->weight
                     );
                  }
                  else
                  {
                     strcat
                     (
                        buf,
                        "There is only a single arrow remaining.\n\r"
                     );
                  }
                  send_to_char(buf, ch);
               }
               return;
            }
            send_to_char("That is not a container.\n\r", ch);
            return;
         }
         case (ITEM_PORTAL_NEW):
         {
            room1 = ch->in_room;
            room2 = get_room_index(obj->value[0]);
            act("$n peers through $p.", ch, obj, NULL, TO_ROOM);
            if (room2 == NULL)
            {
               send_to_char("The portal leads nowhere.\n\r", ch);
               break;
            }
            char_from_room(ch);
            char_to_room_1(ch, room2, TO_ROOM_AT);
            send_to_char("\n\rLooking through the portal you see:\n\r", ch);
            do_observe(ch, "", LOOK_AUTO);
            char_from_room(ch);
            char_to_room_1(ch, room1, TO_ROOM_AT);
            break;
         }
         case (ITEM_DRINK_CON):
         {
            if ( obj->value[1] <= 0 )
            {
               send_to_char( "It is empty.\n\r", ch );
               break;
            }
            sprintf
            (
               buf,
               "It's %sfilled with %s liquid.\n\r",
               (
                  obj->value[1] < obj->value[0] / 4 ?
                  "less than half-" :
                  (
                     obj->value[1] < 3 * obj->value[0] / 4 ?
                     "about half-" :
                     (
                        obj->value[1] == obj->value[0] ?
                        "completely " :
                        "more than half-"
                     )
                  )
               ),
               liq_table[obj->value[2]].liq_color
            );
            send_to_char(buf, ch);
            break;
         }
         case (ITEM_CONTAINER):
         case (ITEM_CORPSE_NPC):
         case (ITEM_CORPSE_PC):
         {
            if (IS_SET(obj->value[1], CONT_CLOSED))
            {
               send_to_char("It is closed.\n\r", ch);
               break;
            }
            act("$p holds:", ch, obj, NULL, TO_CHAR);
            show_list_to_char(obj->contains, ch, TRUE, TRUE);
            break;
         }
      }
      return;
   }
   if (type == LOOK_EXAMINE)
   {
      send_to_char("You do not see that here.\n\r", ch);
      return;
   }
   if ( !str_cmp( arg1, "n" ) || !str_cmp( arg1, "north" ) ) door = 0;
   else if ( !str_cmp( arg1, "e" ) || !str_cmp( arg1, "east"  ) ) door = 1;
   else if ( !str_cmp( arg1, "s" ) || !str_cmp( arg1, "south" ) ) door = 2;
   else if ( !str_cmp( arg1, "w" ) || !str_cmp( arg1, "west"  ) ) door = 3;
   else if ( !str_cmp( arg1, "u" ) || !str_cmp( arg1, "up"    ) ) door = 4;
   else if ( !str_cmp( arg1, "d" ) || !str_cmp( arg1, "down"  ) ) door = 5;
   else
   {
      send_to_char( "You do not see that here.\n\r", ch );
      return;
   }

   /* 'look direction' */
   if ( ( pexit = ch->in_room->exit[door] ) == NULL )
   {
      send_to_char( "Nothing special there.\n\r", ch );
      return;
   }

   if ( pexit->description != NULL && pexit->description[0] != '\0' )
   send_to_char( pexit->description, ch );
   else
   send_to_char( "Nothing special there.\n\r", ch );

   if ( pexit->keyword    != NULL
   &&   pexit->keyword[0] != '\0'
   &&   pexit->keyword[0] != ' ' )
   {
      if ( IS_SET(pexit->exit_info, EX_CLOSED) )
      {
         act( "The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR );
      }
      else if ( IS_SET(pexit->exit_info, EX_ISDOOR) )
      {
         act( "The $d is open.",   ch, NULL, pexit->keyword, TO_CHAR );
      }
   }
   return;
}

/* RT added back for the hell of it */
void do_read (CHAR_DATA *ch, char *argument )
{
   do_look(ch, argument);
}


/*
   Thanks to Zrin for auto-exit part.
*/
void do_exits( CHAR_DATA *ch, char *argument )
{
   extern char * const dir_name[];
   char buf[MAX_STRING_LENGTH];
   EXIT_DATA *pexit;
   bool found;
   bool fAuto;
   int door;
   ROOM_AFFECT_DATA *raf;
   int dskip[6] = {0, 0, 0, 0, 0, 0};


   if (!IS_IMMORTAL(ch) && (!IS_NPC(ch) &&
   ch->pcdata->special != SUBCLASS_ILLUSIONIST))
   {
      if (is_affected_room(ch->in_room, gsn_illusionary_wall))
      {
         raf = ch->in_room->affected;
         while(raf != NULL)
         {
            if (raf->type == gsn_illusionary_wall)
            dskip[raf->modifier] = 1;
            raf = raf->next;
         }
      }
   }

   fAuto  = !str_cmp( argument, "auto" );

   if ( !check_blind( ch )
   && !is_affected( ch, gsn_blind_fighting ) )
   return;
   if (is_affected_room(ch->in_room, gsn_corrupt("hall of mirrors", &gsn_hall_mirrors)))
   {
      if (!IS_IMMORTAL(ch) && !IS_NPC(ch) && ch->pcdata->special != SUBCLASS_ILLUSIONIST)
      {
         if (fAuto)
         send_to_char("[Exits: north east south west up down]\n\r", ch);
         else
         {
            send_to_char("Obvious exits:\n\r", ch);
            send_to_char("North - Hall of Mirrors\n\rEast  - Hall of Mirrors\n\r", ch);
            send_to_char("South - Hall of Mirrors\n\rWest  - Hall of Mirrors\n\r", ch);
            send_to_char("Up    - Hall of Mirrors\n\rDown  - Hall of Mirrors\n\r", ch);
            send_to_char("\n\r", ch);
         }
         return;
      }
   }
   if (fAuto)
   sprintf(buf,  "[Exits:");
   else if (IS_IMMORTAL(ch))
   sprintf(buf,  "Obvious exits from room %d:\n\r", ch->in_room->vnum);
   else
   sprintf(buf,  "Obvious exits:\n\r");

   found = FALSE;
   for ( door = 0; door <= 5; door++ )
   {
      if ( ( pexit = ch->in_room->exit[door] ) != NULL
      &&   pexit->u1.to_room != NULL
      &&   can_see_room(ch, pexit->u1.to_room)
      &&   dskip[door] != TRUE
      &&   !IS_SET(pexit->exit_info, EX_CLOSED) )
      {
         found = TRUE;
         if ( fAuto )
         {
            strcat( buf, " " );
            strcat( buf, dir_name[door] );
         }
         else
         {
            /* sprintf( buf + strlen(buf), "%-5s - %s", */
            sprintf
            (
               buf,  "%s%-5s - %s",
               buf,
               capitalize( dir_name[door] ),
               room_is_dark( pexit->u1.to_room )
               ?  "Too dark to tell"
               : pexit->u1.to_room->name
            );
            if (IS_IMMORTAL(ch))
            sprintf(buf,
            "%s (room %d)\n\r", buf, pexit->u1.to_room->vnum);
            else
            sprintf(buf,  "%s\n\r", buf);
         }
      }
   }

   if ( !found )
   strcat( buf, fAuto ? " none" : "None.\n\r" );

   if ( fAuto )
   strcat( buf, "]\n\r" );

   send_to_char( buf, ch );
   return;
}

void do_worth( CHAR_DATA *ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];

   if (IS_NPC(ch))
   {
      sprintf(buf,  "You have %ld gold and %ld silver.\n\r",
      ch->gold, ch->silver);
      send_to_char(buf, ch);
      return;
   }

   sprintf(buf,
   "You have %ld gold, %ld silver, and %d experience (%d exp to level).\n\r",
   ch->gold, ch->silver, ch->exp,
   ch->level * exp_per_level(ch) - ch->exp);

   send_to_char(buf, ch);

   return;
}


void do_score(CHAR_DATA* ch, char* argument)
{
   char buf[MAX_STRING_LENGTH];
   int ac_cnt;

   if (IS_NPC(ch))
   {
      send_to_char("Nope, not for NPC's. Try stat <mob>\n\r", ch);
      return;
   }

   sprintf
   (
      buf,
      "You are %s%s%s%s%s%s.\n\rLevel %d, %d years old (%d hours). ",
      (
         ch->pcdata->pre_title[0] != '\0' ?
         ch->pcdata->pre_title :
         ""
      ),
      (
         ch->pcdata->pre_title[0] != '\0' ?
         " " :
         ""
      ),
      ch->name,
      (
         (
            ch->pcdata->surname[0] != '\0' &&
            IS_SET(ch->comm2, COMM_SEE_SURNAME) &&
            IS_SET(ch->comm2, COMM_SURNAME_ACCEPTED)
         ) ?
         " " :
         ""
      ),
      (
         (
            ch->pcdata->surname[0] != '\0' &&
            IS_SET(ch->comm2, COMM_SEE_SURNAME) &&
            IS_SET(ch->comm2, COMM_SURNAME_ACCEPTED)
         ) ?
         ch->pcdata->surname :
         ""
      ),
      IS_NPC(ch) ? "" : ch->pcdata->title,
      ch->level, get_age(ch),
      (ch->played + (int)(current_time - ch->logon)) / 3600
   );
   send_to_char(buf, ch);
   sprintf
   (
      buf,  "You are %s.\n\r", get_age_name(ch)
   );
   send_to_char(buf, ch);
   if (get_trust(ch) != ch->level)
   {
      sprintf
      (
         buf,
         "You are trusted at level %d.\n\r",
         get_trust(ch)
      );
      send_to_char(buf, ch);
   }
   sprintf
   (
      buf,
      "Race: %s%s  Sex: %s  Class: %s\n\r",
      race_table[ch->race].name,
      get_scale_color(ch),
      ch->sex == 0 ? "sexless" : ch->sex == 1 ? "male" : "female",
      IS_NPC(ch) ? "mobile" : class_table[ch->class].name
   );
   send_to_char(buf, ch);

   if (ch->level < 30)
   {
      sprintf
      (
         buf,
         "You have %d%% hit, %d%% mana, %d%% movement.\n\r",
         ch->max_hit ? (ch->hit * 100) / ch->max_hit : 0,
         ch->max_mana ? (ch->mana * 100) / ch->max_mana : 0,
         ch->max_move ? (ch->move * 100) / ch->max_move : 0
      );
   }
   else
   {
      sprintf
      (
         buf,
         "You have %d/%d hit, %d/%d mana, %d/%d movement.\n\r",
         ch->hit,
         ch->max_hit,
         ch->mana,
         ch->max_mana,
         ch->move,
         ch->max_move
      );
   }
   send_to_char(buf, ch);
   sprintf
   (
      buf,
      "You have %d practices and %d training sessions.\n\r",
      ch->practice, ch->train
   );
   send_to_char(buf, ch);
   sprintf
   (
      buf,
      "You are carrying %d/%d items with weight %ld/%d pounds.\n\r",
      ch->carry_number,
      can_carry_n(ch),
      get_carry_weight(ch) / 10,
      can_carry_w(ch) / 10
   );
   send_to_char(buf, ch);
   if (ch->level < 20)
   {
      sprintf
      (
         buf,
         "Str: %d(?""?)  Int: %d(?""?)  Wis: %d(?""?)  Dex: %d(?""?)  Con: %d(?"
         "?)\n\r",
         ch->perm_stat[STAT_STR],
         ch->perm_stat[STAT_INT],
         ch->perm_stat[STAT_WIS],
         ch->perm_stat[STAT_DEX],
         ch->perm_stat[STAT_CON]
      );
   }
   else
   {
      sprintf
      (
         buf,
         "Str: %d(%d)  Int: %d(%d)  Wis: %d(%d)  Dex: %d(%d)  Con: %d(%d)\n\r",
         ch->perm_stat[STAT_STR],
         get_curr_stat(ch, STAT_STR),
         ch->perm_stat[STAT_INT],
         get_curr_stat(ch, STAT_INT),
         ch->perm_stat[STAT_WIS],
         get_curr_stat(ch, STAT_WIS),
         ch->perm_stat[STAT_DEX],
         get_curr_stat(ch, STAT_DEX),
         ch->perm_stat[STAT_CON],
         get_curr_stat(ch, STAT_CON)
      );
   }
   send_to_char(buf, ch);
   sprintf
   (
      buf,
      "You have scored %d exp, and have %ld gold and %ld silver coins.\n\r",
      ch->exp,
      ch->gold,
      ch->silver
   );
   send_to_char( buf, ch );

   /* RT shows exp to level */
   if
   (
      !IS_NPC(ch) &&
      ch->level < LEVEL_HERO
   )
   {
      sprintf
      (
         buf,
         "You need %d exp to level.\n\r",
         ch->level * exp_per_level(ch) - ch->exp
      );
      send_to_char(buf, ch);
   }
   if (ch->level < 30)
   {
      sprintf
      (
         buf,
         "Wimpy set to %d%% hit points.  Hometown is %s.\n\r",
         ch->wimpy,
         hometown_table[hometown_lookup(ch->temple)].name
      );
   }
   else
   {
      sprintf
      (
         buf,
         "Wimpy set to %d hit points.  Hometown is %s.\n\r",
         ch->wimpy,
         hometown_table[hometown_lookup(ch->temple)].name
      );
   }
   send_to_char(buf, ch);

   if
   (
      !IS_NPC(ch) &&
      ch->pcdata->condition[COND_DRUNK] > 10
   )
   {
      send_to_char("You are drunk.\n\r", ch);
   }
   if
   (
      !IS_NPC(ch) &&
      ch->pcdata->condition[COND_THIRST] == 0
   )
   {
      send_to_char("You are thirsty.\n\r", ch);
   }
   if
   (
      !IS_NPC(ch) &&
      ch->pcdata->condition[COND_HUNGER] ==  0
   )
   {
      send_to_char("You are hungry.\n\r", ch);
   }
   if (!ch->is_riding)
   {
      switch(ch->position)
      {
         case (POS_DEAD):
         {
            send_to_char("You are DEAD!!\n\r", ch);
            break;
         }
         case (POS_MORTAL):
         {
            send_to_char("You are mortally wounded.\n\r", ch);
            break;
         }
         case (POS_INCAP):
         {
            send_to_char("You are incapacitated.\n\r", ch);
            break;
         }
         case (POS_STUNNED):
         {
            send_to_char("You are stunned.\n\r", ch);
            break;
         }
         case (POS_SLEEPING):
         {
            send_to_char("You are sleeping.\n\r", ch);
            break;
         }
         case (POS_RESTING):
         {
            send_to_char("You are resting.\n\r", ch);
            break;
         }
         case (POS_SITTING):
         {
            send_to_char("You are sitting.\n\r", ch);
            break;
         }
         case (POS_FIGHTING):
         {
            send_to_char("You are fighting.\n\r", ch);
            break;
         }
         default:  /* Standing/corrupt */
         {
            send_to_char("You are standing.\n\r", ch);
            break;
         }
      }
   }
   else
   {
      switch( ch->is_riding->mount_type )
      {
         case (MOUNT_MOBILE):
         {
            sprintf
            (
               buf,
               "You are riding %s.\n\r",
               ((CHAR_DATA*)ch->is_riding->mount)->name
            );
            break;
         }
         case (MOUNT_OBJECT):
         {
            sprintf
            (
               buf,
               "You are riding %s.\n\r",
               ((OBJ_DATA*)ch->is_riding->mount)->short_descr
            );
            break;
         }
         default:
         {
            sprintf(buf, "You are mounted.\n\r");
            break;
         }
      }
      send_to_char(buf, ch);
   }


   /* print AC values */
   if (ch->level >= 25)
   {
      sprintf
      (
         buf,
         "Armor: pierce: %d  bash: %d  slash: %d  magic: %d\n\r",
         GET_AC(ch, AC_PIERCE),
         GET_AC(ch, AC_BASH),
         GET_AC(ch, AC_SLASH),
         GET_AC(ch, AC_EXOTIC)
      );
      send_to_char(buf, ch);
   }

   for (ac_cnt = 0; ac_cnt < 4; ac_cnt++)
   {
      char* ac_name;
      int ac_val;

      switch(ac_cnt)
      {
         case(AC_PIERCE):
         {
            ac_name = "piercing";
            break;
         }
         case(AC_BASH):
         {
            ac_name = "bashing";
            break;
         }
         case(AC_SLASH):
         {
            ac_name = "slashing";
            break;
         }
         case(AC_EXOTIC):
         {
            ac_name = "magic";
            break;
         }
         default:
         {
            ac_name = "error";
            break;
         }
      }

      send_to_char("You are ", ch);
      ac_val = GET_AC(ch, ac_cnt);
      if (ac_val >=  101 )
      {
         sprintf
         (
            buf,
            "hopelessly vulnerable to %s.\n\r",
            ac_name
         );
      }
      else if (ac_val >= 80)
      {
         sprintf
         (
            buf,
            "defenseless against %s.\n\r",
            ac_name
         );
      }
      else if (ac_val >= 60)
      {
         sprintf
         (
            buf,
            "barely protected from %s.\n\r",
            ac_name
         );
      }
      else if (ac_val >= 40)
      {
         sprintf
         (
            buf,
            "slightly armored against %s.\n\r",
            ac_name
         );
      }
      else if (ac_val >= 20)
      {
         sprintf
         (
            buf,
            "somewhat armored against %s.\n\r",
            ac_name
         );
      }
      else if (ac_val >= 0)
      {
         sprintf
         (
            buf,
            "armored against %s.\n\r",
            ac_name
         );
      }
      else if (ac_val >= -20)
      {
         sprintf
         (
            buf,
            "well-armored against %s.\n\r",
            ac_name
         );
      }
      else if (ac_val >= -40)
      {
         sprintf
         (
            buf,
            "very well-armored against %s.\n\r",
            ac_name
         );
      }
      else if (ac_val >= -60)
      {
         sprintf
         (
            buf,
            "heavily armored against %s.\n\r",
            ac_name
         );
      }
      else if (ac_val >= -80)
      {
         sprintf
         (
            buf,
            "superbly armored against %s.\n\r",
            ac_name
         );
      }
      else if (ac_val >= -100)
      {
         sprintf
         (
            buf,
            "almost invulnerable to %s.\n\r",
            ac_name
         );
      }
      else if (ac_val >= -150)
      {
         sprintf
         (
            buf,
            "divinely armored against %s.\n\r",
            ac_name
         );
      }
      else if (ac_val >= -200)
      {
         sprintf
         (
            buf,
            "supremely armored against %s.\n\r",
            ac_name
         );
      }
      else
      {
         sprintf
         (
            buf,
            "god-like against %s.\n\r",
            ac_name
         );
      }
      send_to_char(buf, ch);
   }


   /* RT wizinvis and holy light */
   if (IS_IMMORTAL(ch))
   {
      send_to_char("Holy Light: ", ch);
      if (IS_SET(ch->act, ACT_HOLYLIGHT))
      {
         send_to_char("on", ch);
      }
      else
      {
         send_to_char("off", ch);
      }
      if (ch->invis_level)
      {
         sprintf(buf, "  Invisible: %d", ch->invis_level);
         send_to_char(buf, ch);
      }

      if (ch->incog_level)
      {
         sprintf(buf, "  Incognito: %d", ch->incog_level);
         send_to_char(buf, ch);
      }
      send_to_char("\n\r", ch);
   }
   if (ch->level >= 15)
   {
      sprintf
      (
         buf,
         "Hitroll: %d  Damroll: %d  ",
         GET_HITROLL(ch),
         GET_DAMROLL(ch)
      );
      send_to_char(buf, ch);
      if (ch->level >= 51)
      {
         sprintf
         (
            buf,
            "Spell Power: %2d  Holy Power: %2d",
            ch->spell_power,
            ch->holy_power
         );
         send_to_char(buf, ch);
      }
      if (ch->level < 39)
      {
         send_to_char("\n\r", ch);
      }
   }
   if (ch->level >= 39 && ch->level < 45)
   {
      sprintf(buf, "Saves: %d\n\r", ch->saving_throw);
      send_to_char(buf, ch);
   }
   if (ch->level >= 45)
   {
      sprintf
      (
         buf,
         "\n\rSaves: normal (%d) breath (%d) malediction (%d) transport (%d)"
         " spell (%d)\n\r",
         ch->saving_throw,
         ch->saving_breath,
         ch->saving_maledict,
         ch->saving_transport,
         ch->saving_spell
      );
      send_to_char(buf, ch);
   }
   if (ch->level > 51 && ch->sight)
   {
      sprintf(buf, "Sight: %d\n\r", ch->sight);
      send_to_char(buf, ch);
   }
   send_to_char("You are ", ch);
   if (IS_GOOD(ch))
   {
      send_to_char("good,", ch);
   }
   else if (IS_EVIL(ch))
   {
      send_to_char("evil,", ch);
   }
   else
   {
      send_to_char("neutral,", ch);
   }
   send_to_char(" and ", ch);
   if (ch->pcdata->ethos == 1000)
   {
      send_to_char("methodically inclined.\n\r", ch);
   }
   else if (ch->pcdata->ethos == 0)
   {
      send_to_char("neutrally inclined.\n\r", ch);
   }
   else
   {
      send_to_char("impulsively inclined.\n\r", ch);
   }

   if (IS_SET(ch->affected_by2, AFF_AHZRA_BLOODSTAIN))
   {
      send_to_char("You are bloodstained.\n\r", ch);
   }
   /*
   sprintf
   (
   buf,
   "Questing %s.\n\r",
   ch->pcdata->quest_name
   );
   send_to_char(buf, ch);
   */
   if (IS_SET(ch->comm, COMM_SHOW_AFFECTS))
   {
      do_affects(ch, "");
   }
}

void do_affects(CHAR_DATA* ch, char* argument)
{
   AFFECT_DATA* paf;
   AFFECT_DATA* paf_last = NULL;
   char buf[MAX_STRING_LENGTH];
   int  ticks = FALSE;
   char time_string[MAX_INPUT_LENGTH];
   char affect_format[MAX_INPUT_LENGTH];
   char space_format[MAX_INPUT_LENGTH];
   unsigned int max_length = 0;

   /* calculate longest name of affect */
   for (paf = ch->affected; paf != NULL; paf = paf->next)
   {
      if
      (
         paf->type < 1 ||
         paf->type >= MAX_SKILL
      )
      {
         bug_trust
         (
            "do_affects, invalid paf->type: %d",
            paf->type,
            get_trust(ch)
         );
         affect_remove(ch, paf);
         continue;
      }
      max_length =
      (
         UMAX
         (
            max_length,
            strlen(get_herb_spell_name(paf, TRUE, 0))
         )
      );
   }
   max_length++;
   /* set up max_length spaces */
   sprintf(affect_format, "%%-%ds", max_length);
   sprintf(space_format, affect_format, "");
   if (!str_cmp(argument, "ticks"))
   {
      ticks = TRUE;
   }
   if (ch->affected != NULL)
   {
      send_to_char("You are affected by the following:\n\r", ch);
      for (paf = ch->affected; paf != NULL; paf = paf->next)
      {
         if (paf_last != NULL && paf->type == paf_last->type)
         {
            if (ch->level < 20)
            {
               continue;
            }
            else
            {
               send_to_char(space_format, ch);
            }
         }
         else
         {
            sprintf(buf, affect_format, get_herb_spell_name(paf, TRUE, 0));
            buf[0] = UPPER(buf[0]);
            send_to_char(buf, ch);
         }
         paf_last = paf;
         if (ch->level < 20)
         {
            send_to_char("\n\r", ch);
            continue;
         }
         sprintf
         (
            buf,
            ": modifies %s by %d%s\n\r",
            affect_loc_name(paf->location),
            paf->modifier,
            get_time_string
            (
               ch,
               ch,
               paf,
               NULL,
               time_string,
               TYPE_TIME_AFFECTS,
               ticks
            )
         );
         send_to_char(buf, ch);
      }
   }
   else
   {
      send_to_char("You are not affected by anything.\n\r", ch);
   }
   return;
}



char *    const    day_name    [] =
{
   "the Moon", "the Bull", "Deception", "Thunder", "Freedom",
   "the Great Gods", "the Sun"
};

char *    const    month_name    [] =
{
   "Winter", "the Winter Wolf", "the Frost Giant", "the Old Forces",
   "the Grand Struggle", "the Spring", "Nature", "Futility", "the Dragon",
   "the Sun", "the Heat", "the Battle", "the Dark Shades", "the Shadows",
   "the Long Shadows", "the Ancient Darkness", "the Great Evil"
};

void moon_update();

void get_mud_time_string(MUD_TIME* mud_time_value, char* mud_time_string)
{
   static char* am_pm[2] = {" am", " pm"};
   int curr_day = (mud_time_value->day + mud_time_value->week * 7 + 1);
   int curr_hour;
   char* suf;

   curr_hour = (mud_time_value->hour % 12);
   if (!curr_hour)
   {
      curr_hour = 12;
   }
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
      mud_time_string,
      "%s%d:%s%d:%s%d%s, Day of %s, %d%s the Month of %s, in the year %d",
      curr_hour < 10 ? "0" : "",
      curr_hour,
      mud_time_value->minute < 10 ? "0" : "",
      mud_time_value->minute,
      mud_time_value->second < 10 ? "0" : "",
      mud_time_value->second,
      am_pm[mud_time_value->hour / 12],
      day_name[mud_time_value->day],
      curr_day,
      suf,
      month_name[mud_time_value->month],
      mud_time_value->year
   );
}

void do_time( CHAR_DATA *ch, char *argument )
{
   extern char str_boot_time[];
   char buf[MAX_STRING_LENGTH];
   char* suf;
   static char* am_pm[2] = {" am", " pm"};
   int curr_day = (current_mud_time.day + current_mud_time.week * 7 + 1);
   int curr_hour;
   int moon;

   curr_hour = (current_mud_time.hour % 12);
   if (!curr_hour)
   {
      curr_hour = 12;
   }
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
      "\n\rIt is %s%d:%s%d:%s%d%s, Day of %s, %d%s the Month of %s, in the year"
      " %d.\n\r\n\r",
      curr_hour < 10 ? "0" : "",
      curr_hour,
      current_mud_time.minute < 10 ? "0" : "",
      current_mud_time.minute,
      current_mud_time.second < 10 ? "0" : "",
      current_mud_time.second,
      am_pm[current_mud_time.hour / 12],
      day_name[current_mud_time.day],
      curr_day,
      suf,
      month_name[current_mud_time.month],
      current_mud_time.year
   );
   send_to_char(buf, ch);

   /* Removed indoors-outdoors restriction and time restriction to see moons */

   for (moon = 0; moon < MAX_MOONS; moon++)
   {
      sprintf
      (
         buf,
         "%s%s\n\r",
         moon_table[moon].name,
         moon_phase[moon_table[moon].phase]
      );
      send_to_char(buf, ch);
   }

   sprintf
   (
      buf,
      /* Extra \r's because ctime ends in only \r */
      "\n\rDM started up at %s\r\n\rThe system time is %s\r",
      str_boot_time,
      (char*) ctime(&current_time)
   );
   send_to_char(buf, ch);
   return;
}

/* Immortal command to see moon phases even in the day */
void do_moonphase(CHAR_DATA* ch, char* argument)
{
   int moon;
   char buf[MAX_STRING_LENGTH];

   for (moon = 0; moon < MAX_MOONS; moon++)
   {
     sprintf
     (
         buf,
         "%s%s (%d)\n\r",
         moon_table[moon].name,
         moon_phase[moon_table[moon].phase],
         moon_table[moon].phase
     );
     send_to_char(buf, ch);
   }
   return;
}

void do_weather( CHAR_DATA *ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA *vch;
   OBJ_DATA *brand;

   static char * const sky_look[4] =
   {
      "cloudless",
      "cloudy",
      "raining",
      "lit by flashes of lightning"
   };

   if (is_affected_room(ch->in_room, gsn_storm)){
      send_to_char("An intense and ominous storm seems to hover right over you!\n\r", ch);
      return;
   }
   if ( !IS_OUTSIDE(ch) )
   {
      send_to_char( "You can't see the weather indoors.\n\r", ch );
      return;
   }


   for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room )
   {
      if ( (brand = get_eq_char(vch, WEAR_BRAND))
      && brand->pIndexData->vnum == 876 )
      {
         send_to_char( "The sky is dark with thunderclouds and rain pelts your face.\n\r", ch );
         return;
      }
   }

   sprintf
   (
      buf,  "The sky is %s and %s.\n\r",
      sky_look[weather_info.sky],
      weather_info.change >= 0
      ? "a warm southerly breeze blows"
      : "a cold northern gust blows"
   );
   send_to_char( buf, ch );
   return;
}

void do_help( CHAR_DATA *ch, char *argument )
{
   HELP_DATA *pHelp;
   NOTE_DATA *pnote;
   BUFFER *output;
   bool found = FALSE;
   char argall[MAX_INPUT_LENGTH], argone[MAX_INPUT_LENGTH];
   int level;

   if (IS_NPC(ch)) return;
   output = new_buf();

   if ( argument[0] == '\0' )
   argument = "summary";
   if (!str_cmp(argument, "missing") && IS_IMMORTAL(ch))
   {
      /* list out all skills that have no helps */
      int sn;
      int help_found;
      add_buf(output, "Skills missing help files:\n\r");

      for (sn = 1; sn < MAX_SKILL; sn++)
      {
         if (skill_table[sn].name == NULL)
         {
            continue;
         }
         help_found = FALSE;
         for (pHelp = help_first; pHelp != NULL; pHelp = pHelp->next)
         {
            if (is_name(skill_table[sn].name, pHelp->keyword))
            {
               break;
            }
         }
         if (pHelp != NULL)
         {
            continue;
         }
         for (pnote = help_list; pnote != NULL; pnote = pnote->next)
         {
            if ( is_name( skill_table[sn].name, pnote->subject ))
            {
               break;
            }
         }
         if (pnote != NULL)
         {
            continue;
         }
         found = TRUE;
         add_buf(output, skill_table[sn].name);
         add_buf(output, "\n\r");
      }
   }
   else if (!str_cmp(argument, "topics"))
   {
      /* list out all help topics the user can see */
      add_buf(output, "Help topics available to you:\n\r");
      for ( pHelp = help_first; pHelp != NULL; pHelp = pHelp->next )
      {
         level = (pHelp->level < 0) ? -1 * pHelp->level - 1 : pHelp->level;

         if (level > get_trust( ch ) )
         {
            if (pHelp->level < 61)
            continue;
            if ((pHelp->level == 61 && ch->house != HOUSE_ANCIENT)
            || ( pHelp->level == 62 && ch->house != HOUSE_CONCLAVE)
            || ( pHelp->level == 63 && ch->house != HOUSE_LIGHT)
            || ( pHelp->level == 64 && ch->house != HOUSE_OUTLAW)
            || ( pHelp->level == 65 && ch->house != HOUSE_ENFORCER)
            || ( pHelp->level == 66 && ch->house != HOUSE_CRUSADER)
            || ( pHelp->level == 67 && ch->house != HOUSE_VALOR)
            || ( pHelp->level == 68 && ch->house != HOUSE_COVENANT)
            || ( pHelp->level == 69 && ch->house != HOUSE_BRETHREN) )
            continue;
         }
         found = TRUE;
         add_buf(output, pHelp->keyword);
         add_buf(output, "\n\r");
      }
      for (pnote = help_list; pnote != NULL; pnote = pnote->next)
      {
         if ( is_note_to(ch, pnote))
         {
            found = TRUE;
            add_buf(output, pnote->subject);
            add_buf(output, "\n\r");
         }
      }
   }
   else
   {
      /* this parts handles help a b so that it returns help 'a b' */
      argall[0] = '\0';
      while (argument[0] != '\0' )
      {
         argument = one_argument(argument, argone);
         if (argall[0] != '\0')
         strcat(argall, " ");
         strcat(argall, argone);
      }

      for ( pHelp = help_first; pHelp != NULL; pHelp = pHelp->next )
      {
         level = (pHelp->level < 0) ? -1 * pHelp->level - 1 : pHelp->level;

         if (level > get_trust( ch ) )
         {
            if (pHelp->level < 61)
            continue;
            if ((pHelp->level == 61 && ch->house != HOUSE_ANCIENT)
            || ( pHelp->level == 62 && ch->house != HOUSE_CONCLAVE)
            || ( pHelp->level == 63 && ch->house != HOUSE_LIGHT)
            || ( pHelp->level == 64 && ch->house != HOUSE_OUTLAW)
            || ( pHelp->level == 65 && ch->house != HOUSE_ENFORCER)
            || ( pHelp->level == 66 && ch->house != HOUSE_CRUSADER)
            || ( pHelp->level == 67 && ch->house != HOUSE_VALOR)
            || ( pHelp->level == 68 && ch->house != HOUSE_COVENANT)
            || ( pHelp->level == 69 && ch->house != HOUSE_BRETHREN) )
            continue;
         }

         if ( is_name( argall, pHelp->keyword ) )
         {
            /* add seperator if found */
            if (found)
            add_buf(output,
            "\n\r============================================================\n\r\n\r");
            if ( pHelp->level >= 0 && str_cmp( argall, "imotd" ) )
            {
               add_buf(output, pHelp->keyword);
               add_buf(output, "\n\r");
            }

            /*
            * Strip leading '.' to allow initial blanks.
            */
            if ( pHelp->text[0] == '.' )
            add_buf(output, pHelp->text+1);
            else
            add_buf(output, pHelp->text);
            found = TRUE;
            /* small hack :) */
            if (ch->desc != NULL && ch->desc->connected != CON_PLAYING
            &&              ch->desc->connected != CON_GEN_GROUPS)
            break;
         }
      }

      for (pnote = help_list; pnote != NULL; pnote = pnote->next)
      {
         if ( is_name( argall, pnote->subject ) && is_note_to(ch, pnote))
         {
            /* add seperator if found */
            /*            if (found)
            add_buf(output,
            "\n\r============================================================\n\r\n\r"); */
            /*
            * Strip leading '.' to allow initial blanks.
            */
            if ( pnote->text[0] == '.' )
            add_buf(output, pnote->text+1);
            else
            add_buf(output, pnote->text);
            found = TRUE;
            /* small hack :) */
            if (ch->desc != NULL && ch->desc->connected != CON_PLAYING
            &&              ch->desc->connected != CON_GEN_GROUPS)
            break;
         }
      }
   }
   if (!found)
   {
      send_to_char("Your research into that topic reveals nothing.\n\r", ch);
   }
   else
   {
      send_to_char
      (
         "Your research into that topic reveals the following:\n\r\n\r",
         ch
      );
      page_to_char(buf_string(output), ch);
   }

   free_buf(output);
   return;
}

bool house_member_on(int house)
{
   DESCRIPTOR_DATA *d;
   CHAR_DATA * list;
   for (d = descriptor_list; d != NULL; d = d->next)
   {
      list = d->character;
      if (list == NULL || d->connected != CON_PLAYING)
      continue;
      if (IS_IMMORTAL(list)) continue;
      if (IS_NPC(list))
      continue;
      if (list->house == house) return TRUE;
   }
   return FALSE;
}

void do_name_list(CHAR_DATA *ch, char *argument)
{
   DESCRIPTOR_DATA *d;
   CHAR_DATA * list;
   char buf[MAX_STRING_LENGTH];
   char status[5];
   int count = 0;
   int true_count = 0;

   send_to_char("\n\r", ch);
   buf[0] = '\0';

   for (d = descriptor_list; d != NULL; d = d->next)
   {
      list = d->character;
      if (list == NULL || d->connected != CON_PLAYING || !can_see(ch, list))
      continue;

      if (IS_NPC(list))
      continue;
      if (is_affected(list, gsn_submerge)
      && list != ch && !IS_IMMORTAL(ch)) continue;
      if
      (
         is_affected(list, gsn_stalking) &&
         !IS_IMMORTAL(ch) &&
         ch->house != HOUSE_COVENANT
      )
      {
         if (number_percent()<(get_skill(list, gsn_stalking)-10))
         {
            continue;
         }
      }
      if (!IS_TRUSTED(ch, ANGEL)&&is_affected(list, gsn_cloak_form)&&ch!=list)
      {
         /*if (list->house != ch->house) */ continue;
      }

      if (IS_IMMORTAL(list))
      strcpy(status,  "IMM ");
      else if (list->level < 10)
      strcpy(status,  "NEW ");
      else if (is_in_pk(ch, list))
      strcpy(status,  "(PK)");
      else
      strcpy(status,  "    ");

      if (IS_IMMORTAL(ch)){
         if (list->house != 0)
         sprintf(status,  "%s", house_table[list->house].who_list_name);
      }

      if (IS_IMMORTAL(ch) && list->level > 5)
      {
         char* description;

         if (list->race == grn_book)
         {
            description = list->pcdata->book_description;
         }
         else
         {
            description = list->description;
         }
         if
         (
            description == NULL ||
            strlen(description) < 2
         )
         {
            strcpy(status,  "(ND)");
         }
         else if (strlen(description) < 60*5)
         {
            strcpy(status,  "(SD)");
         }
         else if
         (
            (
               list->pcdata->psych_desc == NULL ||
               strlen(list->pcdata->psych_desc) < 60*5
            ) && (list->level> 29)
            && !IS_IMMORTAL(list)
         )
         {
            strcpy(status,  "(NP)"); /* - Wicket */
         }
      }

      sprintf(buf,  "%s%s %-12s ", buf, status, list->name);
      count++;
      true_count++;
      if (count > 3){
         sprintf(buf,  "%s\n\r", buf);
         send_to_char(buf, ch);
         count = 0;
         buf[0] = '\0';
      }
   }
   if (count != 0){
      sprintf(buf,  "%s\n\r", buf);
      send_to_char(buf, ch);
      count = 0;
      buf[0] = '\0';
   }

   if (true_count > 1){
      sprintf(buf,  "\n\rThere are %d people in the lands that you can see.\n\r"
      , true_count);
   }
   else
   {
      sprintf(buf,  "\n\rYou are the only person you see in the lands.\n\r");
   }
   send_to_char(buf, ch);
   return;
}



/* whois command */
void do_whois (CHAR_DATA *ch, char *argument)
{
   char arg[MAX_INPUT_LENGTH];
   BUFFER *output;
   char buf[MAX_STRING_LENGTH];
   DESCRIPTOR_DATA *d;
   bool found = FALSE;

   one_argument(argument, arg);

   if (arg[0] == '\0')
   {
      send_to_char("Whois who?\n\r", ch);
      return;
   }

   output = new_buf();

   for (d = descriptor_list; d != NULL; d = d->next)
   {
      CHAR_DATA *wch;

      if (d->connected != CON_PLAYING || !can_see(ch, d->character))
      continue;

      wch = ( d->original != NULL ) ? d->original : d->character;

      if (!can_see(ch, wch) || (!IS_TRUSTED(ch, ANGEL) && is_affected(wch, gsn_cloak_form)
      && !IS_IMMORTAL(wch)/* && (ch->house != wch->house)*/))
      continue;
      if (is_affected(wch, gsn_submerge)
      && wch != ch && !IS_IMMORTAL(ch)) continue;
      if (is_affected(wch, gsn_stalking) && !IS_IMMORTAL(ch) && ch->house != wch->house){
         if (number_percent()<(get_skill(wch, gsn_stalking)-10))
         continue;
      }

      if (!str_prefix(arg, wch->name))
      {
         found = TRUE;

         add_who_tags(ch, wch, buf);
         add_buf(output, buf);
      }
   }

   if (!found)
   {
      free_buf(output);
      send_to_char("No one of that name is playing.\n\r", ch);
      return;
   }

   page_to_char(buf_string(output), ch);
   free_buf(output);
}


/*
* New 'who' command originally by Alander of Rivers of Mud.
*/
void do_who(CHAR_DATA* ch, char* argument)
{
   char buf[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];
   char god_name[MAX_INPUT_LENGTH];
   BUFFER* output;
   DESCRIPTOR_DATA* d;
   OBJ_DATA* brand1;
   OBJ_DATA* brand2;
   int iClass;
   int iRace;
   int iHouse;
   int iclan;
   int iSubclass;
   int iLevelLower;
   int iLevelUpper;
   int nNumber;
   int nMatch;
   bool anc = FALSE;
   bool rgfClass[MAX_CLASS];
   bool rgfRace[MAX_PC_RACE];
   bool rgfSubclass[MAX_SUBCLASS + 2];
   bool rgfHouse[MAX_HOUSE];
   bool rgfclan[MAX_CLAN];
   bool fCriminal = FALSE;
   bool fLawless = FALSE;
   bool fClassRestrict = FALSE;
   bool fSubclassRestrict = FALSE;
   bool fHouseRestrict = FALSE;
   bool fclanRestrict = FALSE;
   bool fHouse = FALSE;
   bool fRaceRestrict = FALSE;
   bool fImmortalOnly = FALSE;
   bool fPkOnly = FALSE;
   bool fBuilder = FALSE;
   bool fMoron = FALSE;
   bool fHelper = FALSE;
   OBJ_DATA* robe;
   bool fBrand = FALSE;
   bool fSleeping = FALSE;
   bool fEvil  = FALSE;
   bool fMarked = FALSE;
   bool fcforsaken = FALSE;
   bool fcguildless = FALSE;
   bool fNewbie = FALSE;
   bool fRebel = FALSE;
   bool fInduct = FALSE;
   bool f_worship_only = FALSE;
   bool fpure = FALSE;
   bool fimpure = FALSE;
   bool fnosee = FALSE;
   bool fcalled = FALSE;
   bool funcalled = FALSE;
   bool fsurname = FALSE;

   /*
   * Set default arguments.
   */
   iLevelLower    = 0;
   iLevelUpper    = MAX_LEVEL;
   for (iClass = 0; iClass < MAX_CLASS; iClass++)
   {
      rgfClass[iClass] = FALSE;
   }
   for (iSubclass = 0; iSubclass < MAX_SUBCLASS + 2; iSubclass++)
   {
      rgfSubclass[iSubclass] = FALSE;
   }
   for (iRace = 0; iRace < MAX_PC_RACE; iRace++)
   {
      rgfRace[iRace] = FALSE;
   }
   for (iHouse = 0; iHouse < MAX_HOUSE; iHouse++)
   {
      rgfHouse[iHouse] = FALSE;
   }
   for (iclan = 0; iclan < MAX_CLAN; iclan++)
   {
      rgfclan[iclan] = FALSE;
   }

   /*
   * Parse arguments.
   */
   nNumber = 0;
   for (;;)
   {
      char arg[MAX_STRING_LENGTH];

      argument = one_argument( argument, arg );
      if ( arg[0] == '\0' )
      break;

      if (is_number(arg))
      {
         /* comment started here -werv */
         if (IS_IMMORTAL(ch))
         {
            switch (++nNumber)
            {
               case (1):
               {
                  iLevelLower = atoi(arg);
                  break;
               }
               case (2):
               {
                  iLevelUpper = atoi(arg); break;
               }
               default:
               {
                  send_to_char("Only two level numbers allowed.\n\r", ch);
                  return;
               }
            }

         }
         else
         {
            /* comment was here */

            send_to_char
            (
               "That's not a valid race or house.\n\r",
               ch
            );
            return;
         }
      }
      else
      {
         /*
         * Look for classes to turn on.
         */
         if (!str_prefix(arg, "immortals"))
         {
            fImmortalOnly = TRUE;
         }
         else if (!str_prefix(arg, "pk"))
         {
            fPkOnly = TRUE;
         }
         else if (!str_prefix(arg, "pure") && IS_IMMORTAL(ch))
         {
            fpure = TRUE;
         }
         else if (!str_prefix(arg, "impure") && IS_IMMORTAL(ch))
         {
            fimpure = TRUE;
         }
         else if
         (
            !str_prefix(arg, "worship") &&
            IS_IMMORTAL(ch) &&
            ch->desc
         )
         {
            if (ch->desc->original)
            {
               strcpy(god_name, ch->desc->original->name);
            }
            else
            {
               strcpy(god_name, ch->desc->character->name);
            }
            f_worship_only = TRUE;
         }
         else if
         (
            !str_prefix(arg, "leader") &&
            IS_IMMORTAL(ch)
         )
         {
            fInduct = TRUE;
         }
         else if
         (
            !str_prefix(arg, "wanted") ||
            !str_prefix(arg, "criminal")
         )
         {
            fCriminal = TRUE;
         }
         else if
         (
            !str_prefix(arg, "lawless")
         )
         {
            fLawless = TRUE;
         }
         else if
         (
            IS_IMMORTAL(ch) &&
            !str_prefix(arg, "surname")
         )
         {
            fsurname = TRUE;
         }
         else if
         (
            ch->house == HOUSE_CRUSADER &&
            !str_prefix(arg, "called")
         )
         {
            fcalled = TRUE;
         }
         else if
         (
            ch->house == HOUSE_CRUSADER &&
            !str_prefix(arg, "uncalled")
         )
         {
            funcalled = TRUE;
         }
         else if
         (
            !str_prefix(arg, "evil") &&
            ch->house == HOUSE_LIGHT
         )
         {
            fEvil = TRUE;
         }
         else if
         (
            !str_prefix(arg, "marked") &&
            (
               ch->house == HOUSE_MARAUDER ||
               ch->house == HOUSE_OUTLAW ||
               IS_IMMORTAL(ch) ||
               ch->house == HOUSE_ANCIENT
            )
         )
         {
            fMarked = TRUE;
         }
         else if
         (
            !str_prefix(arg, "forsaken")  /* &&
            (
            ch->house == HOUSE_CRUSADER ||
            IS_IMMORTAL(ch)
            )
            */
         )
         {
            fcforsaken = TRUE;
         }
         else if (!str_prefix(arg, "guildless"))
         {
            fcguildless = TRUE;
         }
         else if
         (
            !str_prefix(arg, "moron") &&
            IS_IMMORTAL(ch)
         )
         {
            fMoron = TRUE;
         }
         else if
         (
            !str_prefix(arg, "builder") &&
            IS_IMMORTAL(ch)
         )
         {
            fBuilder = TRUE;
         }
         else if (!str_prefix(arg, "helper"))
         {
            fHelper = TRUE;
         }
         else if (!str_prefix(arg, "newbie"))
         {
            fNewbie = TRUE;
         }
         else if (!str_prefix(arg, "ally"))
         {
            fRebel = TRUE;
         }
         else if (!str_cmp(arg, "list"))
         {
            do_name_list(ch, "");
            return;
         }
         else if
         (
            !str_prefix(arg, "ancient") &&
            !IS_IMMORTAL(ch) &&
            (
               ch->house == HOUSE_ANCIENT ||
               IS_SET(ch->act2, PLR_IS_ANCIENT)
            )
         )
         {
            list_ancients(ch);
            return;
         }
         else if (!str_prefix(arg, "brand"))
         {
            fBrand = TRUE;
         }
         else if
         (
            !str_prefix(arg, "sleeping") &&
            IS_IMMORTAL(ch)
         )
         {
            fSleeping = TRUE;
         }
         else if
         (
            !str_prefix(arg, "nosee") &&
            IS_IMMORTAL(ch)
         )
         {
            fnosee = TRUE;
         }
         else
         {
            iSubclass = find_subclass(NULL, arg);
            if
            (
               iSubclass != -5 &&
               IS_IMMORTAL(ch)
            )
            {
               fSubclassRestrict = TRUE;
               rgfSubclass[iSubclass + 2] = TRUE;
               if (iSubclass == SUBCLASS_PICKED)
               {
                  rgfSubclass[iSubclass + 3]= TRUE;
               }
               continue;
            }
            iClass = class_lookup(arg);
            if (iClass == -1)
            {
               iRace = race_lookup(arg);

               if
               (
                  iRace == 0 ||
                  iRace >= MAX_PC_RACE
               )
               {
                  iHouse = house_lookup(arg);
                  if (iHouse)
                  {
                     if
                     (
                        iHouse == ch->house ||
                        IS_IMMORTAL(ch)
                     )
                     {
                        fHouseRestrict = TRUE;
                        rgfHouse[iHouse] = TRUE;
                     }
                     else
                     {
                        send_to_char
                        (
                           "You are not a member of that house.\n\r",
                           ch
                        );
                        return;
                     }
                  }
                  else
                  {
                     iclan = clan_lookup(arg);
                     if
                     (
                        iclan &&
                        !IS_NPC(ch)
                     )
                     {
                        if
                        (
                           (
                              iclan == ch->pcdata->clan &&
                              clan_table[iclan].rank == 3
                           ) ||
                           IS_IMMORTAL(ch)
                        )
                        {
                           fclanRestrict = TRUE;
                           rgfclan[iclan] = TRUE;
                        }
                        else
                        {
                           if (iclan == ch->pcdata->clan)
                           {
                              send_to_char
                              (
                                 "Your clan needs a promotion for that.\n\r",
                                 ch
                              );
                              return;
                           }
                           else
                           {
                              send_to_char
                              (
                                 "You are not a member of that clan.\n\r",
                                 ch
                              );
                              return;
                           }
                        }
                     }
                     else
                     {
                        send_to_char
                        (
                           "That's not a valid race or house.\n\r",
                           ch
                        );
                        return;
                     }
                  }
               }
               else
               {
                  fRaceRestrict = TRUE;
                  rgfRace[iRace] = TRUE;
               }
            }
            else
            {
               if (IS_IMMORTAL(ch))
               {
                  fClassRestrict = TRUE;
                  rgfClass[iClass] = TRUE;
               }
               else
               {
                  send_to_char
                  (
                     "That's not a valid race or house.\n\r",
                     ch
                  );
                  return;
               }
            }
         }
      }
   }

   /*
   * Now show matching chars.
   */
   nMatch = 0;
   buf[0] = '\0';
   output = new_buf();
   for (d = descriptor_list; d != NULL; d = d->next)
   {
      CHAR_DATA *wch;

      /*
      * Check for match against restrictions.
      * Don't use trust as that exposes trusted mortals.
      */
      if
      (
         d->connected != CON_PLAYING ||
         !can_see(ch, d->character)
      )
      {
         continue;
      }

      wch = (d->original != NULL) ? d->original : d->character;
      robe = get_eq_char(wch, WEAR_ABOUT);
      if (!can_see(ch, wch))
      {
         continue;
      }
      if
      (
         is_affected(wch, gsn_submerge) &&
         wch != ch &&
         !IS_IMMORTAL(ch)
      )
      {
         continue;
      }
      if
      (
         is_affected(wch, gsn_stalking) &&
         !IS_IMMORTAL(ch) &&
         ch->house != wch->house &&
         number_percent() < (get_skill(wch, gsn_stalking) - 10)
      )
      {
         continue;
      }
      if
      (
         !IS_TRUSTED(ch, ANGEL) &&
         is_affected(wch, gsn_cloak_form) &&
         ch != wch
      )
      {
         if
         (
            is_in_pk(ch, wch) &&
            (wch->house != HOUSE_OUTLAW) &&
            (wch->house != HOUSE_MARAUDER)
         )
         {
            anc = TRUE;
         }
         /*
         if (wch->house != ch->house)
         */
         continue;
      }
      if
      (
         wch->level < iLevelLower ||
         wch->level > iLevelUpper ||
         (
            fImmortalOnly &&
            wch->level < LEVEL_IMMORTAL
         ) ||
         (
            fPkOnly &&
            !is_in_pk(ch, wch)
         ) ||
         (
            fpure &&
            IS_SET(wch->act2, PLR_IMPURE)
         ) ||
         (
            fimpure &&
            !IS_SET(wch->act2, PLR_IMPURE)
         ) ||
         (
            fsurname &&
            (
               wch->pcdata->surname[0] == '\0' ||
               IS_SET(wch->comm2, COMM_SURNAME_ACCEPTED)
            )
         ) ||
         (
            fnosee &&
            !(
               ch != NULL &&
               wch != NULL &&
               (
                  wch == ch->nosee ||
                  (
                     !IS_NPC(ch) &&
                     !IS_NPC(wch) &&
                     IS_IMMORTAL(ch) &&
                     wch->pcdata->nosee_perm != NULL &&
                     wch->pcdata->nosee_perm[0] != '\0' &&
                     is_name(ch->name, wch->pcdata->nosee_perm)
                  )
               )
            )
         ) ||
         (
            fSubclassRestrict &&
            !rgfSubclass[wch->pcdata->special + 2]
         ) ||
         (
            fClassRestrict &&
            !rgfClass[wch->class]
         ) ||
         (
            fRaceRestrict &&
            !rgfRace[wch->race]
         ) ||
         (
            fCriminal &&
            !IS_SET(wch->act, PLR_CRIMINAL) &&
            !IS_SET(wch->act2, PLR_LAWLESS)
         ) ||
         (
            fLawless &&
            !IS_SET(wch->act2, PLR_LAWLESS)
         ) ||
         (
            fcalled &&
            wch->pcdata->called_by == NULL
         ) ||
         (
            funcalled &&
            wch->pcdata->called_by != NULL
         ) ||
         (
            fInduct &&
            wch->pcdata->induct <= 0
         ) ||
         (
            fHelper &&
            (
               robe == NULL ||
               robe->pIndexData->vnum != OBJ_VNUM_HELPER_ROBE
            )
         ) ||
         (
            fBuilder &&
            !IS_SET(wch->comm, COMM_BUILDER)
         ) ||
         (
            fMoron &&
            !IS_SET(wch->comm, COMM_MORON)
         ) ||
         (
            fEvil &&
            !IS_SET(wch->act, PLR_EVIL)
         ) ||
         (
            fMarked &&
            !(
               (
                  IS_SET(wch->act, PLR_MARKED) &&
                  ( ch->house == HOUSE_OUTLAW || ch->house == HOUSE_MARAUDER )
               ) ||
               (
                  (
                     IS_SET(wch->act2, PLR_ANCIENT_MARK) ||
                     IS_SET(wch->act2, PLR_ANCIENT_MARK_PERM)
                  ) &&
                  (
                     ch->house == HOUSE_ANCIENT
                  )
               )
            )
         ) ||
         (
            fcforsaken &&
            !IS_SET(wch->wiznet, PLR_FORSAKEN)
         ) ||
         (
            fcguildless &&
            !IS_SET(wch->act2, PLR_GUILDLESS)
         ) ||
         (
            fRebel &&
            !IS_SET(wch->act, PLR_COVENANT_ALLY)
         ) ||
         (
            fNewbie &&
            wch->level > 10
         ) ||
         (
            fHouse &&
            !is_house(wch) &&
            !IS_IMMORTAL(ch)
         ) ||
         (
            fBrand &&
            (
               (
                  (
                     brand1 = get_eq_char(ch, WEAR_BRAND)
                  ) == NULL
               ) ||
               (
                  (
                     brand2 = get_eq_char(wch, WEAR_BRAND)
                  ) == NULL
               ) ||
               brand1->pIndexData != brand2->pIndexData
            )
         ) ||
         (
            fSleeping &&
            (
               wch->position != POS_SLEEPING
            )
         ) ||
         (
            fHouseRestrict &&
            !rgfHouse[IS_SET(wch->act2, PLR_IS_ANCIENT) ? HOUSE_ANCIENT : wch->house]
         ) ||
         (
            fclanRestrict &&
            !rgfclan[wch->pcdata->clan]
         ) ||
         (
            f_worship_only &&
            (
               wch->pcdata->worship == NULL ||
               str_cmp(god_name, wch->pcdata->worship)
            )
         )
      )
      {
         continue;
      }

      nMatch++;
      add_who_tags(ch, wch, buf);
      add_buf(output, buf);
   }
   sprintf( buf2,  "\n\rPlayers found: %d\n\r", nMatch );
   add_buf(output, buf2);
   if ( anc == 1 )
   {
      sprintf(buf,  "Ancient is on the prowl.\n\r");
      add_buf(output, buf);
   }

   page_to_char( buf_string(output), ch );
   free_buf(output);
   return;
}

void do_count ( CHAR_DATA *ch, char *argument )
{
   int count;
   DESCRIPTOR_DATA *d;
   int not_seen;
   char buf[MAX_STRING_LENGTH];

   not_seen = 0;
   count = 0;

   for ( d = descriptor_list; d != NULL; d = d->next )
   if ( d->connected == CON_PLAYING)
   {
      if ( can_see( ch, d->character ) )
      count++;
      else
      if (d->character->invis_level < get_trust(ch))
      not_seen++;
   }

   if (count >= max_on)
   sprintf(buf, "There are %d characters on, the most so far today.\n\r",
   count);
   else
   sprintf(buf, "There are %d characters on, the most on today was %d.\n\r",
   count, max_on);
   send_to_char(buf, ch);

   if (not_seen != 0)
   {
      sprintf(buf, "There are %d characters on that you can not see.\n\r", not_seen);
      send_to_char(buf, ch);
   }
   sprintf(buf, "The most ever was %d.\n\r", max_on_ever);
   send_to_char(buf, ch);
   return;

}

void do_inventory( CHAR_DATA *ch, char *argument )
{
   send_to_char( "You are carrying:\n\r", ch );
   show_list_to_char( ch->carrying, ch, TRUE, TRUE );
   return;
}



void do_equipment( CHAR_DATA *ch, char *argument )
{
   OBJ_DATA *obj;
   int iWear;
   bool found;
   bool book = ch->race == grn_book;

   send_to_char( "You are using:\n\r", ch );
   found = FALSE;
   for ( iWear = 0; iWear < MAX_WEAR; iWear++ )
   {
      if ( ( obj = get_eq_char( ch, wear_name_ordered[iWear].slot ) ) == NULL )
      continue;

      if (obj->pIndexData->vnum != OBJ_VNUM_TWIG_BRAND)
      {
         send_to_char( get_eqslot_name_ordered(ch, iWear, book), ch );
      }
      else
      {
         send_to_char("<glued on>             ", ch);
      }
      if ( can_see_obj( ch, obj ) )
      {
         send_to_char( format_obj_to_char( obj, ch, TRUE ), ch );
         send_to_char( "\n\r", ch );
      }
      else
      {
         send_to_char( "something.\n\r", ch );
      }
      found = TRUE;
   }
   if (appended_eqlist(ch, ch))
   found = TRUE;

   if ( !found )
   send_to_char( "Nothing.\n\r", ch );

   return;
}



void do_compare( CHAR_DATA *ch, char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   OBJ_DATA *obj1;
   OBJ_DATA *obj2;
   int value1;
   int value2;
   char *msg;

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );
   if ( arg1[0] == '\0' )
   {
      send_to_char( "Compare what to what?\n\r", ch );
      return;
   }

   if ( ( obj1 = get_obj_carry( ch, arg1, ch ) ) == NULL )
   {
      send_to_char( "You do not have that item.\n\r", ch );
      return;
   }

   if (arg2[0] == '\0')
   {
      for (obj2 = ch->carrying; obj2 != NULL; obj2 = obj2->next_content)
      {
         if (obj2->wear_loc != WEAR_NONE
         &&  can_see_obj(ch, obj2)
         &&  obj1->item_type == obj2->item_type
         &&  (obj1->wear_flags & obj2->wear_flags & ~ITEM_TAKE) != 0 )
         break;
      }

      if (obj2 == NULL)
      {
         send_to_char("You aren't wearing anything comparable.\n\r", ch);
         return;
      }
   }

   else if ( (obj2 = get_obj_carry(ch, arg2, ch) ) == NULL )
   {
      send_to_char("You do not have that item.\n\r", ch);
      return;
   }

   msg        = NULL;
   value1    = 0;
   value2    = 0;

   if ( obj1 == obj2 )
   {
      msg = "You compare $p to itself.  It looks about the same.";
   }
   else if ( obj1->item_type != obj2->item_type )
   {
      msg = "You can't compare $p and $P.";
   }
   else
   {
      switch ( obj1->item_type )
      {
         default:
         msg = "You can't compare $p and $P.";
         break;

         case ITEM_ARMOR:
         value1 = obj1->value[0] + obj1->value[1] + obj1->value[2];
         value2 = obj2->value[0] + obj2->value[1] + obj2->value[2];
         break;

         case ITEM_WEAPON:
         if (obj1->pIndexData->new_format)
         value1 = (1 + obj1->value[2]) * obj1->value[1];
         else
         value1 = obj1->value[1] + obj1->value[2];

         if (obj2->pIndexData->new_format)
         value2 = (1 + obj2->value[2]) * obj2->value[1];
         else
         value2 = obj2->value[1] + obj2->value[2];
         break;
      }
   }

   if ( msg == NULL )
   {
      if ( value1 == value2 ) msg = "$p and $P look about the same.";
      else if ( value1  > value2 ) msg = "$p looks better than $P.";
      else                         msg = "$p looks worse than $P.";
   }

   act( msg, ch, obj1, obj2, TO_CHAR );
   return;
}



void do_credits( CHAR_DATA *ch, char *argument )
{
   do_help( ch, "diku" );
   return;
}

bool where_output(CHAR_DATA* ch, CHAR_DATA* victim, ROOM_INDEX_DATA* room)
{
   char buf[MAX_STRING_LENGTH];
   char* name;
   bool pk;

   if
   (
      !IS_IMMORTAL(ch) &&
      victim != ch &&
      (
         is_affected(victim, gsn_submerge) ||
         is_affected(victim, gsn_worm_hide) ||
         (
            is_affected(victim, gsn_stalking) &&
            ch->house != victim->house &&
            number_percent() < get_skill(victim, gsn_stalking)
         )
      )
   )
   {
      return FALSE;
   }
   if
   (
      is_affected(victim, gsn_cloak_form) &&
      !IS_TRUSTED(ch, ANGEL)
   )
   {
      name = "cloaked figure";
   }
   else
   {
      name = PERS(victim, ch);
   }
   pk =
   (
      (
         !IS_NPC(victim) &&
         is_in_pk(ch, victim)
      )
      /*
      Show decoys/etc as pk as if they were the real thing
      Commented out for now.
      ||
      (
         IS_NPC(victim) &&
         (
            victim->pIndexData->vnum == MOB_VNUM_DECOY ||
            victim->pIndexData->vnum == MOB_VNUM_SHADOW ||
            victim->pIndexData->vnum == MOB_VNUM_MIRROR
         ) &&
         victim->mprog_target &&
         !IS_NPC(victim->mprog_target) &&
         is_in_pk(ch, victim->mprog_target)
      )
      */
   );
   sprintf
   (
      buf,
      "%s%-28s %s\n\r",
      pk ?
      "(PK) " :
      "",
      name,
      (
         is_affected_room(room, gsn_hall_mirrors) ?
         "Hall of Mirrors":
         room->name
      )
   );
   send_to_char(buf, ch);
   return TRUE;
}

void do_where( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   DESCRIPTOR_DATA *d;
   bool found;
   bool fuzzy;  /* People that show up multiple places */
   CHAR_DATA*  fuzzy_source;
   int         fuzzy_count;
   bool pk = FALSE;
   int cnt;
   int before;
   ROOM_INDEX_DATA* room;

   one_argument( argument, arg );

   if (is_affected(ch, gsn_delusions))
   {
      send_to_char("A purple haze blocks your view of the area.\n\r", ch);
      return;
   }
   if (!check_blind(ch))
   {
      return;
   }

   if
   (
      arg[0] == '\0' ||
      (
         pk = !str_prefix(arg, "pk")
      )
   )
   {
      send_to_char( "Players near you:\n\r", ch );
      found = FALSE;
      for (d = descriptor_list; d; d = d->next)
      {
         if
         (
            d->connected == CON_PLAYING &&
            (
               victim = d->character
            ) != NULL &&
            !IS_NPC(victim) &&
            victim->in_room != NULL &&
            !IS_SET(victim->in_room->room_flags, ROOM_NOWHERE) &&
            (
               is_room_owner(ch, victim->in_room) ||
               !room_is_private(victim->in_room)
            ) &&
            victim->in_room->area == ch->in_room->area &&
            can_see(ch, victim) &&
            (
               !pk ||
               is_in_pk(ch, victim)
            ) &&
            (
               ch == victim ||
               !is_affected(victim, gsn_feign_death) ||
               (
                  (
                     get_skill(victim, gsn_feign_death) *
                     9 /
                     10 +
                     victim->level -
                     ch->level
                  ) <
                  number_percent()
               )
            )
         )
         {
            fuzzy = FALSE;
            fuzzy_source = NULL;
            fuzzy_count = 0;
            before = 0;
            if
            (
               is_affected(victim, gsn_phantasmal_force_area) &&
               (
                  ch == victim ||
                  (
                     !IS_IMMORTAL(ch) &&
                     ch->house != HOUSE_CONCLAVE
                  )
               )
            )
            {
               fuzzy_source = victim;
               fuzzy_count = number_range
               (
                  victim->level / 15,
                  victim->level / 10
               );
               fuzzy = TRUE;
               before = number_range(0, fuzzy_count);
               for (cnt = 0; cnt < before; cnt++)
               {
                  room = get_random_room(victim, RANDOM_AREA_IMAGE);
                  where_output(ch, fuzzy_source, room);
               }
            }
            if
            (
               where_output(ch, victim, victim->in_room) &&
               !found
            )
            {
               found = TRUE;
            }
            if (fuzzy)
            {
               fuzzy_count -= before;
               for (cnt = 0; cnt < fuzzy_count; cnt++)
               {
                  room = get_random_room(victim, RANDOM_AREA_IMAGE);
                  where_output(ch, fuzzy_source, room);
               }
            }
         }
      }
      if (!found)
      {
         send_to_char("None\n\r", ch);
      }
   }
   else
   {
      found = FALSE;
      fuzzy = FALSE;
      fuzzy_source = NULL;
      fuzzy_count = 0;
      before = 0;
      for ( victim = char_list; victim != NULL; victim = victim->next )
      {
         if
         (
            victim->in_room != NULL &&
            victim->in_room->area == ch->in_room->area &&
            !IS_SET(victim->in_room->room_flags, ROOM_NOWHERE) &&
            (
               !IS_AFFECTED(victim, AFF_SNEAK) ||
               IS_SET(ch->act, ACT_HOLYLIGHT)
            ) &&
            can_see(ch, victim) &&
            is_name(arg, get_name(victim, ch)) &&
            (
               ch == victim ||
               !is_affected(victim, gsn_feign_death) ||
               (
                  (
                     get_skill(victim, gsn_feign_death) *
                     9 /
                     10 +
                     victim->level -
                     ch->level
                  ) <
                  number_percent()
               )
            )
         )
         {
            if
            (
               is_affected(victim, gsn_phantasmal_force_area) &&
               (
                  ch == victim ||
                  (
                     !IS_IMMORTAL(ch) &&
                     ch->house != HOUSE_CONCLAVE
                  )
               )
            )
            {
               fuzzy_source = victim;
               fuzzy_count = number_range
               (
                  victim->level / 15,
                  victim->level / 10
               );
               fuzzy = TRUE;
               before = number_range(0, fuzzy_count);
               for (cnt = 0; cnt < before; cnt++)
               {
                  room = get_random_room(victim, RANDOM_AREA_IMAGE);
                  where_output(ch, fuzzy_source, room);
               }
            }
            if
            (
               where_output(ch, victim, victim->in_room) &&
               !found
            )
            {
               found = TRUE;
            }
            if (fuzzy)
            {
               fuzzy_count -= before;
               for (cnt = 0; cnt < fuzzy_count; cnt++)
               {
                  room = get_random_room(victim, RANDOM_AREA_IMAGE);
                  where_output(ch, fuzzy_source, room);
               }
            }
            break;
            /*

            Where will show all matches, instead of just the first
            break;
            Will just show one again, for now.
            Commented out, waiting for immortal decision.
            */
         }
      }
      if (!found)
      {
         act("You did not find any $T.", ch, NULL, arg, TO_CHAR);
      }
   }

   return;
}




void do_consider( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   char *msg, *buf;
   int diff;

   one_argument( argument, arg );

   if ( arg[0] == '\0' )
   {
      send_to_char( "Consider killing whom?\n\r", ch );
      return;
   }

   if ( ( victim = get_char_room( ch, arg ) ) == NULL )
   {
      send_to_char( "They're not here.\n\r", ch );
      return;
   }

   if (is_safe(ch, victim, IS_SAFE_SILENT | IS_SAFE_IGNORE_AFFECTS))
   {
      send_to_char("Don't even think about it.\n\r", ch);
      return;
   }

   diff = victim->level - ch->level;

   if ( diff <= -10 )
   buf = "Your sneeze would kill $N.";
   else if ( diff <=  -5 )
   buf = "$N wouldn't last long against you.";
   else if ( diff <=  -2 )
   buf = "$N looks like an easy kill.";
   else if ( diff <=   1 )
   buf = "The perfect match!";
   else if ( diff <=   4 )
   buf = "$N looks just a little tough.";
   else if ( diff <=   9 )
   buf = "$N wouldn't need much help to kill you.";
   else
   buf = "You must have a fascination with death.";

   if (!IS_IMMORTAL(ch) && is_affected(victim, gsn_worm_hide))
   buf = "You can't determine their power, relative to yours.";

   act(buf, ch, NULL, victim, TO_CHAR);

   if (!IS_NPC(victim))
   {
      /* Do not show align for PC considers */
      return;
   }

   if ( victim->alignment >=  1 ) msg = "$N smiles happily at you.";
   else if ( victim->alignment <= -1 ) msg = "$N grins evilly at you.";
   else                    msg = "$N seems indifferent towards you.";

   if (!IS_IMMORTAL(ch) && is_affected(victim, gsn_worm_hide))
   msg = "$N's moral stance is unclear to you.";
   act( msg, ch, NULL, victim, TO_CHAR );
   return;
}



void set_title( CHAR_DATA *ch, char *title )
{
   char buf[MAX_STRING_LENGTH];

   if ( IS_NPC(ch) )
   {
      bug( "Set_title: NPC.", 0 );
      return;
   }

   if ( title[0] != '.' && title[0] != ',' && title[0] != '!' && title[0] != '?' )
   {
      buf[0] = ' ';
      strcpy( buf+1, title );
   }
   else
   {
      strcpy( buf, title );
   }

   free_string( ch->pcdata->title );
   ch->pcdata->title = str_dup( buf );
   return;
}



void do_title( CHAR_DATA *ch, char *argument )
{
   CHAR_DATA *victim;
   char arg1[MAX_INPUT_LENGTH];
   char *pArg;
   char cEnd;

   /*
   * Can't use one_argument here because it smashes case.
   * So we just steal all its code.  Bleagh.
   */
   pArg = arg1;
   while ( isspace(*argument) )
   argument++;

   cEnd = ' ';
   if ( *argument == '\'' || *argument == '"' )
   cEnd = *argument++;

   while ( *argument != '\0' )
   {
      if ( *argument == cEnd )
      {
         argument++;
         break;
      }
      *pArg++ = *argument++;
   }
   *pArg = '\0';

   if ( arg1[0] == '\0' || argument[0] == '\0' )
   {
      send_to_char( "Change who's title to what?\n\r", ch );
      return;
   }

   if ( (victim = get_char_world(ch, arg1)) == NULL)
   {
      send_to_char( "Player isn't on.\n\r", ch );
      return;
   }

   if ( IS_NPC(victim) )
   {
      send_to_char( "Trying to change a mob's title?\n\r", ch );
      return;
   }

   if ( ch != victim && get_trust(victim) >= get_trust(ch) )
   {
      send_to_char( "You can't do that to that person.\n\r", ch );
      return;
   }

   if ( strlen(argument) > 45 )
   argument[45] = '\0';

   smash_tilde( argument );
   set_title( victim, argument );
   send_to_char( "Ok.\n\r", ch );
}

void set_extitle(CHAR_DATA *ch, char *title) {
   char buf[MAX_STRING_LENGTH];

   if (IS_NPC(ch)) {
      bug("Set_extitle: NPC.", 0);
      return;
   }

   if (title[0]!='.'&&title[0]!=','&&title[0]!='!'&&title[0]!='?') {
      buf[0] = ' ';
      strcpy(buf+1, title);
   } else {
      strcpy(buf, title);
   }

   free_string(ch->pcdata->extitle);
   ch->pcdata->extitle = str_dup(buf);
   return;
}

void do_extitle( CHAR_DATA *ch, char *argument ) {
   CHAR_DATA *victim;
   char arg1[MAX_INPUT_LENGTH];
   char *pArg;
   char cEnd;

   if (IS_NPC(ch))
   return;

   /*
   * Can't use one_argument here because it smashes case.
   * So we just steal all its code.  Bleagh.
   */
   pArg = arg1;
   while ( isspace(*argument) )
   argument++;

   cEnd = ' ';
   if ( *argument == '\'' || *argument == '"' )
   cEnd = *argument++;

   while ( *argument != '\0' )
   {
      if ( *argument == cEnd )
      {
         argument++;
         break;
      }
      *pArg++ = *argument++;
   }
   *pArg = '\0';

   if ( arg1[0] == '\0')
   {
      send_to_char( "Change who's extra title to what?\n\r", ch );
      return;
   }

   if ( (victim = get_char_world(ch, arg1)) == NULL)
   {
      send_to_char( "That player isn't on.\n\r", ch );
      return;
   }

   /* Styx's request for Illunus' title to be "Styx's Little Woman" hardcoded */
   if (victim->id == ID_ILLUNUS)
   {
      send_to_char("Illunus' extitle is to be Styx's Little Woman.\n\r", ch);
      send_to_char("Styx's orders.\n\r", ch);
      send_to_char("Extitle failed.\n\r", ch);
      return;
   }

   if ( IS_NPC(victim) )
   {
      send_to_char( "Trying to change a mob's extra title?\n\r", ch );
      return;
   }

   if ( ch != victim && get_trust(victim) >= get_trust(ch) )
   {
      send_to_char( "You can't do that to that person.\n\r", ch );
      return;
   }

   if (argument[0]=='\0') {
      free_string(victim->pcdata->extitle);
      victim->pcdata->extitle=NULL;
      send_to_char("Extitle cleared.\n\r", ch);
      return;
   }

   if ( strlen(argument) > 45 )
   argument[45] = '\0';

   smash_tilde( argument );
   set_extitle( victim, argument );
   send_to_char( "Ok.\n\r", ch );

}

void do_info(CHAR_DATA *ch, char *argument)
{
   char buf[MAX_STRING_LENGTH];
   char buf2[MAX_INPUT_LENGTH];
   char arg1[MAX_STRING_LENGTH];
   char arg2[MAX_STRING_LENGTH];
   char*  dossier;
   sh_int dossier_lines;
   CHAR_DATA *victim;
   bool brief = FALSE;

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );

   if (!str_cmp(arg2, "brief"))
   brief = TRUE;

   if (IS_NPC(ch))
   {
      send_to_char("You cannot see their information.\n\r", ch);
      return;
   }

   if (!str_cmp(arg1, "cloaked") || !str_cmp(arg1, "figure"))
   {
      send_to_char( "The cloaked figure is shrouded in mystery.\n\r", ch);
      return;
   }

   if (arg1[0] == '\0')
   {
      victim = ch;
   }
   else
   {
      strcat(buf, arg1);
      victim = get_pc_world(ch, arg1, FALSE);
      if (!can_see(ch, victim))
      {
         victim = NULL;
      }
   }
   if (victim == NULL)
   {
      send_to_char("Character not found.\n\r", ch);
      return;
   }
   else
   if ( IS_NPC(victim) )
   {
      if
      (
         victim->master != NULL &&
         can_see(ch, victim->master) &&
         !IS_NPC(victim->master)
      )
      {
         victim = victim->master;
      }
      else
      {
         send_to_char("Character not found.\n\r", ch);
         return;
      }
   }


   /*    if (victim == NULL || IS_NPC(victim))
   {
   send_to_char("Character not found.\n\r", ch);
   return;
   } */


   if (!IS_TRUSTED(ch, ANGEL) && is_affected(victim, gsn_cloak_form)
   && victim != ch)
   {
      send_to_char( "Character not found.\n\r", ch);
      return;
   }
   sprintf
   (
      buf,
      "Information on %s%s%s:\n\r",
      (
         victim->pcdata->pre_title[0] != '\0' ?
         victim->pcdata->pre_title :
         ""
      ),
      (
         victim->pcdata->pre_title[0] != '\0' ?
         " " :
         ""
      ),
      get_longname(victim, ch)
   );
   send_to_char(buf, ch);
   buf2[0] = '\0';
   if (victim->pcdata->pre_title[0] != '\0')
   {
      strcat(buf2, victim->pcdata->pre_title);
      strcat(buf2, " ");
   }
   strcat(buf2, victim->name);
   if
   (
      (
         IS_IMMORTAL(ch) ||
         ch == victim ||
         victim->level >= LEVEL_HERO
      ) &&
      IS_SET(ch->comm2, COMM_SEE_SURNAME) &&
      victim->pcdata->surname[0] != '\0'
   )
   {
      if (IS_SET(victim->comm2, COMM_SURNAME_ACCEPTED))
      {
         strcat(buf2, " ");
         strcat(buf2, victim->pcdata->surname);
      }
      else if (IS_IMMORTAL(ch))
      {
         strcat(buf2, " [-");
         strcat(buf2, victim->pcdata->surname);
         strcat(buf2, "-]");
      }
   }
   if ( IS_IMMORTAL(ch) )
   {
      if (victim->pcdata->extitle != NULL)
      sprintf(buf,  "%s%s%s (Rank %d)\n\r",
      buf2, victim->pcdata->title, victim->pcdata->extitle, victim->level);
      else
      sprintf(buf,  "%s%s (Rank %d)\n\r", buf2, victim->pcdata->title, victim->level);
   }
   else
   {
      if (victim->pcdata->extitle != NULL)
      sprintf(buf,  "%s%s%s\n\r", buf2, victim->pcdata->title,
      victim->pcdata->extitle);
      else
      sprintf(buf,  "%s%s\n\r", buf2, victim->pcdata->title);
   }

   send_to_char(buf, ch);

   if
   (
      IS_IMMORTAL(ch) &&
      !IS_NPC(victim) &&
      victim->pcdata->dossier != NULL &&
      victim->pcdata->dossier[0] != '\0'
   )
   {
      dossier_lines = 0;
      dossier = victim->pcdata->dossier;
      while (*dossier != '\0')
      {
         if (*dossier == '\n')
         {
            dossier_lines++;
         }
         dossier++;
      }
      if (dossier_lines)
      {
         sprintf
         (
            buf,
            "Dossier contains %d %s.\n\r",
            dossier_lines,
            dossier_lines == 1 ? "entry" : "entries"
         );
         send_to_char(buf, ch);
      }
   }
   if
   (
      victim->pcdata->worship &&
      (
         IS_IMMORTAL(ch) ||
         ch == victim
      )
   )
   {
      sprintf(buf, "Worships: %s.\n\r", victim->pcdata->worship);
      send_to_char(buf, ch);
   }
   if (victim->pcdata->special != 0 && !IS_IMMORTAL(victim))
   {
      sprintf(buf,  "%s\n\r", get_char_subtitle(victim));
      send_to_char(buf, ch);
   }

   if (get_brand_title(victim) != NULL)
   {
      sprintf(buf,  "%s\n\r", get_brand_title(victim));
      send_to_char(buf, ch);
   }
   if (get_house_title(victim) != NULL)
   {
      sprintf(buf, "%s\n\r", get_house_title(victim));
      send_to_char(buf, ch);
   }
   if (get_house_title2(victim) != NULL)
   {
      sprintf(buf, "%s\n\r", get_house_title2(victim));
      send_to_char(buf, ch);
   }

   if (IS_SET(victim->act2, PLR_GUILDMASTER))
   {
      sprintf(buf, "Guildmaster of %ss.\n\r",
      class_table[victim->class].name);
      send_to_char(buf, ch);
   }
   if (victim->race == grn_draconian)
   {
      sprintf(buf, "Scale color:%s\n\r", get_scale_color(victim));
      send_to_char(buf, ch);
   }

   if
   (
      ((ch->house == HOUSE_MARAUDER) || IS_IMMORTAL(ch)) &&
      victim->house == HOUSE_MARAUDER
   )
   {
      switch (victim->pcdata->induct)
      {
         case (RANK_APPRENTICE):
         {
             send_to_char("Apprentice of the Guild ", ch);
             break;
         }
         case (RANK_MARAUDER):
         {
             send_to_char("Marauder of the Guild ", ch);
             break;
         }
         case (RANK_JOURNEYMAN):
         {
             send_to_char("Journeyman of the Guild ", ch);
             break;
         }
         case (RANK_GUILDMASTER):
         {
             send_to_char("Guildmaster of the Guild ", ch);
             break;
         }
         case (RANK_HEADMASTER):
         {
             send_to_char("Headmaster of the Guild ", ch);
             break;
         }
         default:
         {
             bug("do_info: Marauder does not have a valid induct level.", 0);
             send_to_char("Marauder of the Guild ", ch);
             break;
         }
      }
      switch (victim->pcdata->house_rank2)
      {
         case (SECT_ROGUE):
         {
             send_to_char("(Rogue)\n\r", ch);
             break;
         }
         case (SECT_THUG):
         {
             send_to_char("(Thug)\n\r", ch);
             break;
         }
         case (SECT_TRICKSTER):
         {
             send_to_char("(Trickster)\n\r", ch);
             break;
         }
         case (SECT_NONE):
         {
            if (victim->pcdata->induct != RANK_APPRENTICE)
            {
               bug("do_info: marauder does not belong to a sect or have incorrect sect.", 0);
            }
            send_to_char("\n\r", ch);
            break;
         }
         default:
         {
             bug("do_info: marauder does not belong to a sect or have incorrect sect.", 0);
             send_to_char("\n\r", ch);
             break;
         }
      }
   }

   if (brief) return;

   if (IS_IMMORTAL(ch) || ch->in_room == victim->in_room)
   {
      send_to_char("\n\rDescription:\n\r", ch);
      if (victim->race == grn_book)
      {
         buk_desc(ch, victim);
      }
      else
      {
         send_to_char
         (
            victim->description ?
            victim->description :
            "(None).\n\r",
            ch
         );
      }

   }

   send_to_char("\n\r", ch);

   if (ch == victim || IS_IMMORTAL(ch))
   {
      if (victim == ch)
      send_to_char( "Your psychological description is:\n\r", ch );
      else
      send_to_char( "Their psychological description is:\n\r", ch );
      send_to_char( victim->pcdata->psych_desc ? victim->pcdata->psych_desc
      : "(None).\n\r", ch );
   }
   return;
}

/* do_desc_types version 1.5 */
void do_desc_types(CHAR_DATA* ch, char* argument, char** desc, char* prefix)
{
   char buf      [MAX_STRING_LENGTH];
   char arg1     [MAX_INPUT_LENGTH];
   char arg2     [MAX_INPUT_LENGTH];
   char new_line [MAX_INPUT_LENGTH];
   sh_int target_line = 0;
   sh_int num_lines   = 0;
   sh_int last_line   = 0;
   bool insert        = FALSE;
   bool empty         = FALSE;
   bool show          = FALSE;
   bool add           = FALSE;
   bool remove        = FALSE;
   bool clear         = FALSE;
   char* plusminus    = NULL;
   char* pnew         = NULL;
   char* pold         = NULL;
   sh_int place       = 0;
   sh_int last_space  = 0;

   /* Sanity checks, this isn't called directly by the interpreter. */
   if (ch == NULL)
   {
      /* Called incorrectly */
      bug("do_desc_types called with NULL ch (1st parameter)", 0);
      return;
   }
   if (desc == NULL)
   {
      /* Called incorrectly */
      bug("do_desc_types called with NULL desc (3rd parameter)", 0);
      return;
   }
   if (argument == NULL)
   {
      /* Called incorrectly, but can recover */
      bug("do_desc_types called with NULL argument (2nd parameter)", 0);
      argument = "";
   }
   if (prefix == NULL)
   {
      /* Called incorrectly, but can recover */
      bug("do_desc_types called with NULL prefix (4th parameter)", 0);
      prefix = "";
   }

   smash_tilde(argument);
   if ((argument[0] == '\0') || (!str_cmp(argument, "show")))
   {
      show = TRUE;
   }
   else if (!str_cmp(argument, "clear"))
   {
      clear = TRUE;
   }
   else if (!str_cmp(argument, "format"))
   {
      sprintf
      (
         buf,
         "Formatting your %sdescription to wrap at 80 characters.\n\r",
         prefix
      );
      send_to_char(buf, ch);
      if (desc[0] == NULL)
      {
         send_to_char("Nothing to format.\n\r", ch);
         return;
      }
      buf[0] = '\0';
      pnew = buf;
      pold = desc[0];
      place = 0;
      last_space = 0;
      while (pold[0] != '\0')
      {
         pnew[0] = pold[0];
         if (pnew[0] == ' ')
         {
            last_space = 0;
         }
         else if (pnew[0] == '\r')
         {
            place = -1;
            last_space = -1;
         }
         last_space++;
         place++;
         pnew++;
         pold++;
         if (place == 82)  /* 80 characters, \n\r */
         {
            if (last_space == 82)  /* 81 characters, not a single space */
            {
               pnew -= 2;
               pold -= 2;
               pnew[0] = '\n';
               pnew[1] = '\r';
               pnew[2] = ' ';  /* Indent when you move it over */
               pnew += 3;
               last_space = 0;
               place = 0;
            }
            else
            {
               pnew -= last_space;
               pold -= last_space;
               pold++;
               pnew[0] = '\n';
               pnew[1] = '\r';
               pnew += 2;
               last_space = 0;
               place = 0;
            }
         }
      }
      pnew[0] = '\0';
      if (strlen(buf) > MAX_DESC_LENGTH)
      {
         sprintf(buf, "%sdescription will be too long after formatting.\n\r", prefix);
         buf[0] = UPPER(buf[0]);
         send_to_char(buf, ch);
      }
      else
      {
         free_string(desc[0]);
         desc[0] = str_dup(buf);
      }
      return;
   }
   else if (!str_cmp(argument, "showlines"))
   {
      if (desc[0] == NULL)
      {
         send_to_char("You have no lines.\n\r", ch);
         return;
      }
      for (pold = desc[0]; pold[0] != '\0'; pold++)
      {
         if (pold[0] == '\r')
         {
            num_lines ++;
         }
      }
      if  /* Take care of case of 1000 blank lines, just in case */
      (
         (strlen(desc[0]) + (num_lines * 5) + 2) >= MAX_STRING_LENGTH
      )
      {
         sprintf(buf, "%sdescription will be too long after adding lines.\n\r", prefix);
         buf[0] = UPPER(buf[0]);
         send_to_char(buf, ch);
         return;
      }
      else
      {
         sprintf(buf, "Showing your %sdescription with line numbers.\n\r", prefix);
         buf[0] = UPPER(buf[0]);
         send_to_char(buf, ch);
      }
      buf[0] = '\0';
      pnew = buf;
      pold = desc[0];
      place++;
      sprintf(arg1, "%-3d: ", place);
      strcat(buf, arg1);
      pnew += 5;  /* Number is 3 digits, ':' and ' ' */
      pnew[0] = '\0';
      while (pold[0] != '\0')
      {
         pnew[0] = pold[0];
         if (pnew[0] == '\r')
         {
            pnew++;
            pold++;
            if (pold[0] != '\0')  /* Don't need line number at the end */
            {
               place++;
               sprintf(arg1, "%-3d: ", place);
               pnew[0] = '\0';
               strcat(buf, arg1);
               pnew += 5;  /* Number is 3 digits, ':' and ' ' */
            }
         }
         else
         {
            pnew++;
            pold++;
         }
      }
      pnew[0] = '\0';
      send_to_char(buf, ch);
      return;
   }
   else
   {
      /* All special commands are checked before splitting up into arguments. */
      argument = one_argument_space(argument, arg1);

      if (is_number(arg1))
      {
         insert = TRUE;
         target_line = atoi(arg1);
      }
      if (desc[0] == NULL)
      {
         empty = TRUE;
         num_lines = 0;
      }
      else
      {
         /* Count lines */
         for (pold = desc[0]; pold[0] != '\0'; pold++)
         {
            if (pold[0] == '\r')
            {
               num_lines ++;
            }
         }
      }
      if (insert && !show)
      {
         /* Need 2 parameters for line option, 3rd is line, can be blank */
         argument = one_argument_space(argument, arg2);
         if (empty)
         {
            sprintf
            (
               buf,
               "You cannot use the line option when your %sdescription is"
               " empty.\n\r",
               prefix
            );
            send_to_char(buf, ch);
            return;
         }
         if (target_line < 1)
         {
            send_to_char("Line number too low.\n\r", ch);
            return;
         }
         if (target_line > num_lines)
         {
            sprintf
            (
               buf,
               "You do not have that many lines in your %sdescription.\n\r",
               prefix
            );
            send_to_char(buf, ch);
            return;
         }
         if (arg2[0] == '\0')
         {
            sprintf
            (
               buf,
               "Too few parameters.  Read the help for %sdescription.\n\r",
               prefix
            );
            send_to_char(buf, ch);
            return;
         }
         plusminus = arg2;
         target_line--;  /* Need to be before the target line always. */
      }
      else
      {
         plusminus = arg1;
      }
      sprintf(new_line, "%s\n\r", argument);
      if (!str_cmp(plusminus, "+"))
      {
         add = TRUE;
         show = FALSE;
      }
      else if (!str_cmp(plusminus, "++"))
      {
         add = TRUE;
         show = TRUE;
      }
      else if (!str_cmp(plusminus, "-"))
      {
         remove = TRUE;
         show = FALSE;
      }
      else if (!str_cmp(plusminus, "--"))
      {
         remove = TRUE;
         show = TRUE;
      }
      else
      {
         /* Show syntax */
         send_to_char
         (
            "Syntax:\n\r"
            "psych (parameters)\n\r"
            "desc (parameters)\n\r"
            "desc clear\n\r"
            "  Clears your description.\n\r"
            "desc show\n\r"
            "  Shows your description.\n\r"
            "desc showlines\n\r"
            "  Shows your description with line numbers.\n\r"
            "desc <number> + (string)\n\r"
            "desc <number> ++ (string)\n\r"
            "  example: desc 5 ++ He has soft blue eyes.\n\r"
            "  Would insert 'He has soft blue eyes.' before the fifth line.\n\r"
            "  Number can be between 1 and the number of lines in your"
            " description.\n\r"
            "  If <number> is ommitted, it adds to the end.\n\r",
            ch
         );
         send_to_char
         (
            "  + is identical to ++, but ++ will show you your description"
            "\n\r"
            "  after every line you add.\n\r"
            "desc <number> -\n\r"
            "desc <number> --\n\r"
            "  example: desc 5 --\n\r"
            "  This would remove the fifth line of your description.\n\r"
            "  example: desc 3 -\n\r"
            "  This would remove the third line of your description.\n\r"
            "  If <number> is omitted, it will remove the last line of your"
            " description.\n\r"
            "desc format\n\r"
            "  This will wordwrap your description at 80 characters per line"
            ".\n\r",
            ch
         );
         send_to_char
         (
            "Anything else will show you this syntax string. \n\r",
            ch
         );
         return;
      }
      if (!insert && add)
      {
         target_line = num_lines;  /* Append */
      }
      else if (!insert && remove)
      {
         target_line = num_lines - 1;  /* Delete last line */
      }
      if
      (
         remove &&
         (
            empty ||
            num_lines <= 1
         )
      )
      {
         /* Gonna be empty */
         insert  = FALSE;
         clear  = TRUE;
         remove = FALSE;
         show   = FALSE;
      }
      if
      (
         !empty &&
         (
            add ||
            remove
         )
      )
      {
         last_line = 0;
         /* Get to line */
         pnew = buf;
         pold = desc[0];
         while (pold[0] != '\0')
         {
            if (last_line == target_line)
            {
               break;
            }
            pnew[0] = pold[0];
            if (pold[0] == '\r')
            {
               last_line++;
            }
            pnew++;
            pold++;
         }
         pnew[0] = '\0';  /* At the place to delete, or add */
         if (add)
         {
            if (strlen(new_line) + strlen(desc[0]) >= MAX_DESC_LENGTH)
            {
               sprintf(buf, "%sdescription too long.\n\r", prefix);
               buf[0] = UPPER(buf[0]);
               send_to_char(buf, ch);
               return;
            }
            strcat(buf, new_line);
            strcat(buf, pold);
            free_string(desc[0]);
            desc[0] = str_dup(buf);
         }
         else  /* remove */
         {
            while (pold[0] != '\0')
            {
               if (pold[0] == '\r')
               {
                  pold++;
                  break;
               }
               pold++;
            }
            strcat(buf, pold);
            free_string(desc[0]);
            desc[0] = str_dup(buf);
         }
         if (!show)
         {
            send_to_char("Ok.\n\r", ch);
         }
      }
      else if (empty && add)
      {
         free_string(desc[0]);
         desc[0] = str_dup(new_line);
         if (!show)
         {
            send_to_char("Ok.\n\r", ch);
         }
      }
   }
   if (clear)
   {
      sprintf
      (
         buf,
         "Your %sdescription has been cleared.\n\r",
         prefix
      );
      send_to_char(buf, ch);
      free_string(desc[0]);
      desc[0] = str_dup("");
      return;
   }
   if (show)
   {
      sprintf
      (
         buf,
         "Your %sdescription is:\n\r",
         prefix
      );
      send_to_char(buf, ch);
      send_to_char(desc[0] ? desc[0] : "(None)\n\r", ch);
   }
   return;
}

void do_psychdesc(CHAR_DATA* ch, char* argument)
{
   if (IS_NPC(ch))
   {
      send_to_char("NPCs do not have psych descriptions.", ch);
      return;
   }
   do_desc_types(ch, argument, &(ch->pcdata->psych_desc), "psychological ");
}

void do_description(CHAR_DATA* ch, char *argument)
{
   do_desc_types(ch, argument, &(ch->description), "");
}

void do_description_type(CHAR_DATA* ch, char *argument)
{
   if
   (
      IS_NPC(ch) ||
      ch->race != grn_book
   )
   {
      do_description(ch, argument);
   }
   else
   {
      do_desc_types(ch, argument, &(ch->pcdata->book_description), "");
   }
}

void do_book_description(CHAR_DATA* ch, char *argument)
{
   if
   (
      IS_NPC(ch) ||
      ch->race != grn_book
   )
   {
      send_to_char("Huh?\n\r", ch);
   }
   else
   {
      do_desc_types(ch, argument, &(ch->description), "book page ");
   }
}

void do_report( CHAR_DATA *ch, char *argument )
{
   char buf[MAX_INPUT_LENGTH];
   int percenta;
   int percentb;
   int percentc;
   char* verb;

   if (ch->max_hit == 0)
   percenta = 0;
   else
   percenta = (ch->hit * 100) / ch->max_hit;
   if (ch->max_mana == 0)
   percentb = 0;
   else
   percentb = (ch->mana * 100) / ch->max_mana;
   if (ch->max_move == 0)
   percentc = 0;
   else
   percentc = (ch->move * 100) / ch->max_move;

   /* Based on if you mentally project or 'say' */
   if (is_mental(ch))
   {
      verb = "mentally project";
   }
   else
   {
      verb = "say";
   }
   sprintf
   (
      buf,
      "You %s 'I have %d%% hp %d%% mana %d%% mv %d xp.'\n\r",
      verb,
      percenta,
      percentb,
      percentc,
      ch->exp
   );
   send_to_char( buf, ch );

   sprintf
   (
      buf,
      "$n %ss 'I have %d%% hp %d%% mana %d%% mv %d xp.'",
      verb,
      percenta,
      percentb,
      percentc,
      ch->exp
   );

   /*    (ch->hit*100)/ch->max_hit,
   (ch->mana*100)/ch->max_mana,
   (ch->move*100)/ch->max_move,
   ch->exp   );
   */

   act( buf, ch, NULL, NULL, TO_ROOM );

   return;
}

void do_getpk( CHAR_DATA *ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];
   BUFFER *output;
   DESCRIPTOR_DATA *d;
   OBJ_DATA *brand1, *brand2;
   int iClass;
   int iRace;
   int iHouse;
   int iLevelLower;
   int iLevelUpper;
   int nMatch;
   int anc=0;
   bool rgfClass[MAX_CLASS];
   bool rgfRace[MAX_PC_RACE];
   bool rgfHouse[MAX_HOUSE];
   bool fCriminal = FALSE;
   bool fClassRestrict = FALSE;
   bool fHouseRestrict = FALSE;
   bool fHouse = FALSE;
   bool fRaceRestrict = FALSE;
   bool fImmortalOnly = FALSE;
   bool fPkOnly = TRUE;
   bool fBuilder = FALSE;
   bool fMoron = FALSE;
   bool fBrand = FALSE;
   bool fEvil  = FALSE;
   bool fMarked = FALSE;
   bool fcforsaken = FALSE;
   bool fcguildless = FALSE;
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;

   one_argument( argument, arg );

   if ( arg[0] == '\0' )
   {
      send_to_char( "Get whose PK?\n\r", ch );
      return;
   }

   if ( ( victim = get_char_world( ch, argument ) ) == NULL
   || !can_see(ch, victim))
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if ( IS_NPC(victim) )
   {
      send_to_char( "Mobs don't get a PK.\n\r", ch );
      return;
   }

   /*
   * Set default arguments.
   */
   iLevelLower    = 0;
   iLevelUpper    = MAX_LEVEL;
   for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
   rgfClass[iClass] = FALSE;
   for ( iRace = 0; iRace < MAX_PC_RACE; iRace++ )
   rgfRace[iRace] = FALSE;
   for (iHouse = 0; iHouse < MAX_HOUSE; iHouse++)
   rgfHouse[iHouse] = FALSE;

   /*
   * Now show matching chars.
   */
   nMatch = 0;
   buf[0] = '\0';
   output = new_buf();
   for ( d = descriptor_list; d != NULL; d = d->next )
   {
      CHAR_DATA *wch;
      char const *class;

      /*
      * Check for match against restrictions.
      * Don't use trust as that exposes trusted mortals.
      */
      if ( d->connected != CON_PLAYING || !can_see( ch, d->character ) )
      continue;

      wch   = ( d->original != NULL ) ? d->original : d->character;

      if (!can_see(ch, wch)) continue;

      if
      (
         !IS_TRUSTED(victim, ANGEL) &&
         is_affected(wch, gsn_cloak_form) &&
         victim != wch &&
         (
            !(
               victim->house == HOUSE_ANCIENT &&
               victim->pcdata->induct == 5
            )
         )
      )
      {
         if
         (
            is_in_pk(victim, wch) &&
            (wch->house != HOUSE_OUTLAW) &&
            (wch->house != HOUSE_MARAUDER)
         )
         {
            anc=1;
         }
      }

      if ( wch->level < iLevelLower
      ||   wch->level > iLevelUpper
      || ( fImmortalOnly  && wch->level < LEVEL_IMMORTAL )
      || ( fPkOnly && !is_in_pk(victim, wch) )
      || ( fClassRestrict && !rgfClass[wch->class] )
      || ( fRaceRestrict && !rgfRace[wch->race])
      || ( fCriminal && !IS_SET(wch->act, PLR_CRIMINAL))
      || ( fEvil && !IS_SET(wch->act, PLR_EVIL))
      || ( fMarked && !((IS_SET(wch->act, PLR_MARKED) && ((ch->house == HOUSE_OUTLAW)||(ch->house == HOUSE_MARAUDER)))
      || ((IS_SET(wch->act2, PLR_ANCIENT_MARK) || IS_SET(wch->act2, PLR_ANCIENT_MARK_PERM)) && (ch->house == HOUSE_ANCIENT)) ))
      /*        || ( fMarked && !(IS_SET(wch->act, PLR_MARKED) && (ch->house ==
      HOUSE_OUTLAW)))
      || ( fMarked && !((IS_SET(wch->act2, PLR_ANCIENT_MARK) ||
      IS_SET(wch->act2, PLR_ANCIENT_MARK_PERM)) && (ch->house == HOUSE_ANCIENT))) */
      || ( fcforsaken && !IS_SET(wch->wiznet, PLR_FORSAKEN))
      || ( fcguildless && !IS_SET(wch->act2, PLR_GUILDLESS))
      || ( fBuilder && !IS_SET(wch->comm, COMM_BUILDER))
      || ( fMoron && !IS_SET(wch->comm, COMM_MORON))
      || ( fHouse && !is_house(wch) && !IS_IMMORTAL(ch))
      || ( fBrand &&
      (((brand1 = get_eq_char(ch, WEAR_BRAND)) == NULL) ||
      ((brand2 = get_eq_char(wch, WEAR_BRAND)) == NULL) ||
      (brand1->pIndexData->vnum != brand2->pIndexData->vnum)))
      || ( fHouseRestrict && !rgfHouse[wch->house]))
      continue;

      nMatch++;

      /*
      * Figure out what to print for class.
      */
      class = class_table[wch->class].who_name;
      switch ( wch->level )
      {
         default: break;
         {
            case MAX_LEVEL - 0 : class = "IMP";     break;
            case MAX_LEVEL - 1 : class = "CRE";     break;
            case MAX_LEVEL - 2 : class = "SUP";     break;
            case MAX_LEVEL - 3 : class = "DEI";     break;
            case MAX_LEVEL - 4 : class = "GOD";     break;
            case MAX_LEVEL - 5 : class = "IMM";     break;
            case MAX_LEVEL - 6 : class = "DEM";     break;
            case MAX_LEVEL - 7 : class = "ANG";     break;
            case MAX_LEVEL - 8 : class = "AVA";     break;
         }
      }

      /*
      * Format it up.
      */
      if (wch->level >= 52 || IS_IMMORTAL(ch) || wch == ch)
      {
         sprintf(buf,  "[%2d %5s %s] %s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s\n\r",
         wch->level,
         wch->race < MAX_PC_RACE ? pc_race_table[wch->race].who_name
         : "     ",
         class,
         is_in_pk(victim, wch) ? "(PK) " : "",
         wch->incog_level >= LEVEL_HERO ? "(Incog) " : "",
         wch->invis_level >= LEVEL_HERO ? "(Wizi) " : "",
         (IS_SET(wch->act2, PLR_IS_ANCIENT) && (wch->house == 0))
         ? "[ancient] " : house_table[wch->house].who_name,
         IS_SET(wch->comm, COMM_AFK) ? "[AFK] " : "",
         ((ch->level>51) && (IS_SET(wch->comm, COMM_MORON))) ? "[MORON] " : "",
         (ch->house == HOUSE_COVENANT && IS_SET(wch->act, PLR_COVENANT_ALLY)) ? "(ALLIED) " : "",
         (
            IS_SET(wch->act2, PLR_LAWLESS) ?
            "(LAWLESS) " :
            (
               IS_SET(wch->act, PLR_CRIMINAL)
            ) ?
            "(CRIMINAL) " :
            ""
         ),
         IS_SET(wch->act, PLR_THIEF)  ? "(THIEF) "  : "",
         IS_SET(wch->act, PLR_EVIL) &&
         ((ch->house == HOUSE_LIGHT) || (IS_IMMORTAL(ch))) ? "(EVIL) ":"",
         IS_SET(wch->wiznet, PLR_FORSAKEN) /* && (ch->house ==
         HOUSE_CRUSADER || IS_IMMORTAL(ch)) */ ? "(FORSAKEN) ":"",
         IS_SET(wch->act2, PLR_GUILDLESS) ? "(GUILDLESS) " : "",
         IS_SET(wch->act, PLR_MARKED) &&
         ((ch->house == HOUSE_OUTLAW)||(ch->house == HOUSE_MARAUDER)) ? "(MARKED) ":"",
         (IS_SET(wch->act2, PLR_ANCIENT_MARK) || IS_SET(wch->act2, PLR_ANCIENT_MARK_PERM)) &&
         ((ch->house == HOUSE_ANCIENT) || IS_IMMORTAL(ch)) ? "(MARKED) ":"",
         (
            wch->pcdata->pre_title[0] != '\0' ?
            wch->pcdata->pre_title :
            ""
         ),
         (
            wch->pcdata->pre_title[0] != '\0' ?
            " " :
            ""
         ),
         wch->name,
         IS_NPC(wch) ? "" : wch->pcdata->title ,
         IS_SET(wch->act, PLR_HONORBOUND) ? ""/*", Honorbound"*/ : "",
         IS_NPC(wch) ? "" :
         (wch->pcdata->extitle) ? wch->pcdata->extitle : "",
         (is_affected(wch, gsn_cloak_form))?" (Cloaked)":"");
         add_buf(output, buf);
      } else {
         sprintf(buf,  "[   %5s    ] %s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s\n\r",
         wch->race < MAX_PC_RACE ? pc_race_table[wch->race].who_name
         : "     ",
         is_in_pk(victim, wch) ? "(PK) " : "",
         wch->incog_level >= LEVEL_HERO ? "(Incog) " : "",
         wch->invis_level >= LEVEL_HERO ? "(Wizi) " : "",
         (wch->house == HOUSE_EMPIRE || wch->house == HOUSE_ENFORCER) ?
         house_table[wch->house].who_name :
         (wch->house == victim->house) ?
         house_table[wch->house].who_name : "",
         IS_SET(wch->comm, COMM_AFK) ? "[AFK] " : "",
         (ch->house == HOUSE_COVENANT && IS_SET(wch->act, PLR_COVENANT_ALLY) )? "(ALLY) " : "",
         (IS_SET(wch->act, PLR_CRIMINAL) )? "(CRIMINAL) " : "",
         IS_SET(wch->act, PLR_THIEF)  ? "(THIEF) "  : "",
         IS_SET(wch->act, PLR_EVIL) &&
         ((ch->house == HOUSE_LIGHT) || (IS_IMMORTAL(ch))) ? "(EVIL) ":"",
         IS_SET(wch->wiznet, PLR_FORSAKEN) /* && (ch->house ==
         HOUSE_CRUSADER || IS_IMMORTAL(ch)) */ ? "(FORSAKEN) ":"",
         IS_SET(wch->act2, PLR_GUILDLESS) ? "(GUILDLESS) " : "",
         IS_SET(wch->act, PLR_MARKED) &&
         ((ch->house == HOUSE_OUTLAW)||(ch->house == HOUSE_MARAUDER)) ? "(MARKED) ":"",
         (IS_SET(wch->act2, PLR_ANCIENT_MARK) || IS_SET(wch->act2, PLR_ANCIENT_MARK_PERM)) &&
         ((ch->house == HOUSE_ANCIENT) || IS_IMMORTAL(ch)) ? "(MARKED) ":"",
         IS_NPC(wch) ? "" : wch->pcdata->title,
         IS_SET(wch->act, PLR_HONORBOUND) ? ""/*", Honorbound"*/ : "",
         IS_NPC(wch) ? "" :
         (wch->pcdata->extitle) ? wch->pcdata->extitle : "",
         (is_affected(wch, gsn_cloak_form)/* && (ch->house == wch->house)
         */)?
         " (Cloaked)": "");
         add_buf(output, buf);
      }
   }

   sprintf(buf2,  "\n\rPlayers found: %d\n\r", nMatch );
   add_buf(output, buf2);

   page_to_char( buf_string(output), ch );
   free_buf(output);
   return;
}

void do_getprac( CHAR_DATA *ch, char *argument ) {
   char buf[MAX_STRING_LENGTH];
   BUFFER *buffer;
   char skill_list[LEVEL_HERO + 1][MAX_STRING_LENGTH];
   char skill_columns[LEVEL_HERO + 1];
   int sn, level, min_lev = 1, max_lev = LEVEL_HERO;
   bool found = FALSE;
   int count1, count2, sum1, sum2, count3, count4;
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   int col;

   one_argument( argument, arg );

   if ( arg[0] == '\0' )
   {
      send_to_char( "Get whose practices?\n\r", ch );
      return;
   }

   if (!str_cmp(arg, "list"))
   {
      argument = one_argument(argument, arg);
      if ( ( victim = get_char_world( ch, argument ) ) == NULL
      || !can_see(ch, victim))
      {
         send_to_char( "Character not found.\n\r", ch );
         return;
      }

      if ( IS_NPC(victim) )
      {
         if ( victim->master != NULL && can_see(ch, victim->master))
         victim = victim->master;
         else
         {
            send_to_char("Character not found.\n\r", ch);
            return;
         }
      }

      /* initialize data */
      for (level = 0; level < LEVEL_HERO + 1; level++)
      {
         skill_columns[level] = 0;
         skill_list[level][0] = '\0';
      }
      count1 = 0; count2 = 0; sum1 = 0; sum2 = 0; count3 = 0; count4 = 0;
      max_lev = victim->level;
      for (sn = 1; sn < MAX_SKILL; sn++)
      {
         if (skill_table[sn].name == NULL )
         break;

         if ((level = victim->pcdata->learnlvl[sn]) < LEVEL_HERO + 1
         &&  level >= min_lev && level <= max_lev
         &&  victim->pcdata->learned[sn] > 0)
         {
            found = TRUE;
            sum1 += victim->pcdata->learned[sn];
            count1++;
            if (victim->pcdata->learned[sn] > 10)
            {
               sum2 += victim->pcdata->learned[sn];
               count2++;
            }
            if (victim->pcdata->learned[sn] == 100)
            {
               if (skill_table[sn].spell_fun == spell_null)
               count3++;
               else
               count4++;
            }
            level = victim->pcdata->learnlvl[sn];
            if (victim->level < level)
            sprintf(buf, "%-18s n/a      ", skill_table[sn].name);
            else
            sprintf(buf, "%-18s %3d%%      ", skill_table[sn].name, victim->pcdata->learned[sn]);
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
      add_buf(buffer, "\n\r");
      sprintf(buf,  "%d Total skills, %3.2f%% learned.\n\r", count1, (float) sum1/(float) count1);
      add_buf(buffer, buf);
      sprintf(buf,  "%d practiced skills, %3.2f%% learned.\n\r", count2, (float) sum2/(float) count2);
      add_buf(buffer, buf);
      sprintf(buf,  "%d total skills mastered,  %3.2f%% total mastery.\n\r", count3, 100.0*(float) count3/ (float) count1);
      add_buf(buffer, buf);
      sprintf
      (
         buf,
         "%d total %s mastered,  %3.2f%% total mastery.\n\r",
         count4,
         is_clergy(ch) ? "prayers" : "spells",
         100.0 * (float)count4 / (float)count1
      );
      add_buf(buffer, buf);
      count3 += count4;
      sprintf(buf,  "%d total mastered,  %3.2f%% total mastery.\n\r", count3, 100.0*(float) count3/ (float) count1);
      add_buf(buffer, buf);
      page_to_char(buf_string(buffer), ch);
      free_buf(buffer);
      return;
   }

   if ( ( victim = get_char_world( ch, argument ) ) == NULL
   || !can_see(ch, victim))
   {
      send_to_char( "Character not found.\n\r", ch );
      return;
   }

   if ( IS_NPC(victim) )
   {
      if ( victim->master != NULL && can_see(ch, victim->master))
      victim = victim->master;
      else
      {
         send_to_char("Character not found.\n\r", ch);
         return;
      }
   }

   col    = 0;
   for ( sn = 1; sn < MAX_SKILL; sn++ )
   {
      if ( skill_table[sn].name == NULL )
      break;
      if ( /* victim->level < skill_table[sn].skill_level[victim->class] */
      !has_skill(victim, sn)
      || victim->pcdata->learned[sn] < 1 /* skill is not known */)
      continue;

      sprintf(buf,  "%-18s %3d%%  ",
      skill_table[sn].name, victim->pcdata->learned[sn] );
      send_to_char( buf, ch );
      if ( ++col % 3 == 0 )
      send_to_char( "\n\r", ch );
   }

   if ( col % 3 != 0 )
   send_to_char( "\n\r", ch );

   sprintf(buf,  "%s has %d practice sessions left.\n\r",
   victim->name, victim->practice );
   send_to_char( buf, ch );
   return;
}

void do_practice( CHAR_DATA *ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   BUFFER *buffer;
   char skill_list[LEVEL_HERO + 1][MAX_STRING_LENGTH * 4];
   char skill_columns[LEVEL_HERO + 1];
   int sn, level, min_lev = 1, max_lev = LEVEL_HERO;
   bool found = FALSE;
   int count1, count2, sum1, sum2, count3, count4;
   bool ansi = IS_ANSI(ch);

   if ( IS_NPC(ch) )
   return;

   if (!str_cmp(argument, "list"))
   {
      /* initialize data */
      for (level = 0; level < LEVEL_HERO + 1; level++)
      {
         skill_columns[level] = 0;
         skill_list[level][0] = '\0';
      }
      count1 = 0; count2 = 0; sum1 = 0; sum2 = 0; count3 = 0; count4 = 0;
      max_lev = ch->level;
      for (sn = 1; sn < MAX_SKILL; sn++)
      {
         if (skill_table[sn].name == NULL )
         break;

         if ((level = ch->pcdata->learnlvl[sn]) < LEVEL_HERO + 1
         &&  level >= min_lev && level <= max_lev
         &&  ch->pcdata->learned[sn] > 0)
         {
            found = TRUE;
            sum1 += ch->pcdata->learned[sn];
            count1++;
            if (ch->pcdata->learned[sn] > 10)
            {
               sum2 += ch->pcdata->learned[sn];
               count2++;
            }
            if (ch->pcdata->learned[sn] == 100)
            {
               if (skill_table[sn].spell_fun == spell_null)
               count3++;
               else
               count4++;
            }
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
      add_buf(buffer, "\n\r");
      sprintf(buf,  "%d Total skills, %3.2f%% learned.\n\r", count1, (float) sum1/(float) count1);
      add_buf(buffer, buf);
      sprintf(buf,  "%d practiced skills, %3.2f%% learned.\n\r", count2, (float) sum2/(float) count2);
      add_buf(buffer, buf);
      sprintf(buf,  "%d total skills mastered,  %3.2f%% total mastery.\n\r", count3, 100.0*(float) count3/ (float) count1);
      add_buf(buffer, buf);
      sprintf
      (
         buf,
         "%d total %s mastered,  %3.2f%% total mastery.\n\r",
         count4,
         is_clergy(ch) ? "prayers" : "spells",
         100.0 * (float)count4 / (float)count1
      );
      add_buf(buffer, buf);
      count3 += count4;
      sprintf(buf,  "%d total mastered,  %3.2f%% total mastery.\n\r", count3, 100.0*(float) count3/ (float) count1);
      add_buf(buffer, buf);
      page_to_char(buf_string(buffer), ch);
      free_buf(buffer);
      return;
   }

   if ( argument[0] == '\0' )
   {
      int col;

      col    = 0;
      buffer = new_buf();
      for ( sn = 1; sn < MAX_SKILL; sn++ )
      {
         if ( skill_table[sn].name == NULL )
         break;
         if (!has_skill(ch, sn))
         continue;

         sprintf
         (
            buf,
            "%s%-18s%s %3d%%  ",
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
         ch->pcdata->updated[sn] = FALSE;
         add_buf(buffer, buf);
         if ( ++col % 3 == 0 )
         {
            add_buf(buffer, "\n\r");
         }
      }

      if ( col % 3 != 0 )
      {
         add_buf(buffer, "\n\r");
      }

      sprintf(buf, "You have %d practice sessions left.\n\r", ch->practice);
      add_buf(buffer, buf);
      page_to_char(buf_string(buffer), ch);
      free_buf(buffer);
   }
   else
   {
      CHAR_DATA *mob;
      int adept;

      if ( !IS_AWAKE(ch) )
      {
         send_to_char( "In your dreams, or what?\n\r", ch );
         return;
      }

      for ( mob = ch->in_room->people; mob != NULL; mob = mob->next_in_room )
      {
         if (( IS_NPC(mob) && IS_SET(mob->act, ACT_PRACTICE) ) ||
         (!IS_NPC(mob) && IS_SET(mob->act2, PLR_GUILDMASTER)
         && (mob->class == ch->class)))
         break;
      }

      if ( ( sn = find_skill_spell( ch, argument ) ) < 0
      || ( !IS_NPC(ch)
      &&   ( /* ch->level < skill_table[sn].skill_level[ch->class] */
      !has_skill(ch, sn)
      ||    ch->pcdata->learned[sn] < 1 /* skill is not known */
      ||    skill_table[sn].rating[ch->class] == 0)))
      {
         send_to_char( "You can't practice that.\n\r", ch );
         return;
      }

      if ( mob == NULL )
      {
         send_to_char( "You can't do that here.\n\r", ch );
         if
         (
            sn == gsn_staves ||
            sn == gsn_scrolls ||
            sn == gsn_wands ||
            sn == gsn_alchemy ||
            (
               !is_clergy(ch) &&
               skill_table[sn].spell_fun != spell_null
            )
         )
         {
            check_impure(ch, skill_table[sn].name, IMPURE_PRACTICE_INTENT);
         }
         return;
      }

      if (/* !IS_NPC(mob) && */ IS_SET(ch->act2, PLR_GUILDLESS))
      {
         send_to_char( "No guildmaster will train an outcast.\n\r", ch);
         if
         (
            sn == gsn_staves ||
            sn == gsn_scrolls ||
            sn == gsn_wands ||
            sn == gsn_alchemy ||
            (
               !is_clergy(ch) &&
               skill_table[sn].spell_fun != spell_null
            )
         )
         {
            check_impure(ch, skill_table[sn].name, IMPURE_PRACTICE_INTENT);
         }
         return;
      }

      if ( ch->practice <= 0 )
      {
         send_to_char( "You have no practice sessions left.\n\r", ch );
         if
         (
            sn == gsn_staves ||
            sn == gsn_scrolls ||
            sn == gsn_wands ||
            sn == gsn_alchemy ||
            (
               !is_clergy(ch) &&
               skill_table[sn].spell_fun != spell_null
            )
         )
         {
            check_impure(ch, skill_table[sn].name, IMPURE_PRACTICE_INTENT);
         }
         return;
      }

      if
      (
         sn == gsn_staves ||
         sn == gsn_scrolls ||
         sn == gsn_wands ||
         sn == gsn_alchemy ||
         (
            !is_clergy(ch) &&
            skill_table[sn].spell_fun != spell_null
         )
      )
      {
         if
         (
            IS_NPC(mob) &&
            IS_SET(mob->act2, ACT_PURE_GUILDMASTER)
         )
         {
            check_impure(ch, skill_table[sn].name, IMPURE_PRACTICE_INTENT);
            do_say(mob, "We do not teach the taint here. You do not need it.");
            return;
         }
         if
         (
            !IS_NPC(mob) &&
            mob->house == HOUSE_CRUSADER
         )
         {
            check_impure(ch, skill_table[sn].name, IMPURE_PRACTICE_INTENT);
            do_say(mob, "I do not teach the taint. You do not need it.");
            return;
         }
      }


      adept = IS_NPC(ch) ? 100 : class_table[ch->class].skill_adept;
      if ( sn == gsn_adv_hide && ch->pcdata->learned[gsn_hide] < 100)
      {
         send_to_char( "The guildmaster tells you 'But you haven't mastered your normal technique yet'\n\r", ch);
         return;
      }

      if ( sn == gsn_exp_hide && ch->pcdata->learned[gsn_adv_hide] < 100)
      {
         send_to_char( "The guildmaster tells you 'But you haven't mastered your advanced technique yet'\n\r", ch);
         return;
      }
      if ( ch->pcdata->learned[sn] >= adept )
      {
         sprintf(buf,  "You are already learned at %s.\n\r",
         skill_table[sn].name );
         if
         (
            sn == gsn_staves ||
            sn == gsn_scrolls ||
            sn == gsn_wands ||
            (
               !is_clergy(ch) &&
               skill_table[sn].spell_fun != spell_null
            )
         )
         {
            check_impure(ch, skill_table[sn].name, IMPURE_PRACTICE_INTENT);
         }
         send_to_char( buf, ch );
      }
      else
      {
         if (sn == gsn_hyper)
         {
            /*
               Hyper and nap cannot be practiced,
               they must be learned up from 1%
            */
            do_say(mob, "Eat some candy.");
            do_say(mob, "Next!");
            return;
         }
         else if (sn == gsn_nap)
         {
            /*
               Hyper and nap cannot be practiced,
               they must be learned up from 1%
            */
            do_say(mob, "Drink some milk.");
            do_say(mob, "Next!");
            return;
         }
         un_hide(ch, NULL); /* XUR */
         ch->practice--;
         ch->pcdata->learned[sn] +=
         int_app[get_curr_stat(ch, STAT_INT)].learn /
         skill_table[sn].rating[ch->class];
         if
         (
            sn == gsn_staves ||
            sn == gsn_scrolls ||
            sn == gsn_wands ||
            (
               !is_clergy(ch) &&
               skill_table[sn].spell_fun != spell_null
            )
         )
         {
            check_impure(ch, skill_table[sn].name, IMPURE_PRACTICE);
         }
         if ( ch->pcdata->learned[sn] < adept )
         {
            act( "You practice $T.",
            ch, NULL, skill_table[sn].name, TO_CHAR );
            act( "$n practices $T.",
            ch, NULL, skill_table[sn].name, TO_ROOM );
         }
         else
         {
            ch->pcdata->learned[sn] = adept;
            act( "You are now learned at $T.",
            ch, NULL, skill_table[sn].name, TO_CHAR );
            act( "$n is now learned at $T.",
            ch, NULL, skill_table[sn].name, TO_ROOM );
         }
      }
   }
   return;
}



/*
* 'Wimpy' originally by Dionysos.
*/
void do_wimpy( CHAR_DATA *ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   char arg[MAX_INPUT_LENGTH];
   int wimpy;

   one_argument( argument, arg );

   if ( arg[0] == '\0' )
   {
      if (ch->level >= 30)
      wimpy = ch->max_hit / 5;
      else
      wimpy = 20;
   }
   else
   wimpy = atoi( arg );

   if ( wimpy < 0 )
   {
      send_to_char( "Your courage exceeds your wisdom.\n\r", ch );
      return;
   }
   if (ch->level >= 30)
   {
      if ( wimpy > ch->max_hit/2 )
      {
         send_to_char( "Such cowardice ill becomes you.\n\r", ch );
         return;
      }

      ch->wimpy    = wimpy;
      sprintf(buf,  "Wimpy set to %d hit points.\n\r", wimpy );
      send_to_char( buf, ch );
   }
   else
   {
      if ( wimpy > 50)
      {
         send_to_char( "Such cowardice ill becomes you.\n\r", ch );
         return;
      }
      ch->wimpy = wimpy;
      sprintf(buf,  "Wimpy set to %d%% hit points.\n\r", wimpy );
      send_to_char( buf, ch );
   }
   return;
}



void do_password( CHAR_DATA *ch, char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   char arg3[MAX_INPUT_LENGTH];
   char buf[MAX_INPUT_LENGTH];
   char *pArg;
   char *pwdnew;
   char *p;
   char cEnd;

   if ( IS_NPC(ch) )
   return;

   /*
   * Can't use one_argument here because it smashes case.
   * So we just steal all its code.  Bleagh.
   */
   pArg = arg1;
   while ( isspace(*argument) )
   argument++;

   cEnd = ' ';
   if ( *argument == '\'' || *argument == '"' )
   cEnd = *argument++;

   while ( *argument != '\0' )
   {
      if ( *argument == cEnd )
      {
         argument++;
         break;
      }
      *pArg++ = *argument++;
   }
   *pArg = '\0';

   pArg = arg2;
   while ( isspace(*argument) )
   argument++;

   cEnd = ' ';
   if ( *argument == '\'' || *argument == '"' )
   cEnd = *argument++;

   while ( *argument != '\0' )
   {
      if ( *argument == cEnd )
      {
         argument++;
         break;
      }
      *pArg++ = *argument++;
   }
   *pArg = '\0';

   pArg = arg3;
   while ( isspace(*argument) )
   argument++;

   cEnd = ' ';
   if ( *argument == '\'' || *argument == '"' )
   cEnd = *argument++;

   while ( *argument != '\0' )
   {
      if ( *argument == cEnd )
      {
         argument++;
         break;
      }
      *pArg++ = *argument++;
   }
   *pArg = '\0';

   if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
   {
      send_to_char( "Syntax: password <old> <new> <new verification>.\n\r", ch );
      return;
   }

   if
   (
      (
         (
            ch->pcdata->pwd &&
            ch->pcdata->pwd[0] != '\0'
         ) ||
         (
            ch->pcdata->old_pwd &&
            ch->pcdata->old_pwd[0] != '\0'
        )
      ) &&
      (
         (
            ch->pcdata->pwd == NULL ||
            ch->pcdata->pwd[0] == '\0' ||
            strcmp(md5_hash(arg1), ch->pcdata->pwd)
         ) &&
         (
            ch->pcdata->old_pwd == NULL ||
            ch->pcdata->old_pwd[0] == '\0' ||
            strcmp(crypt(arg1, ch->pcdata->old_pwd), ch->pcdata->old_pwd)
         )
      )
   )
   {
      WAIT_STATE(ch, 40);
      send_to_char("Wrong password.  Wait 10 seconds.\n\r", ch);
      return;
   }

   if (strlen(arg2) < 5)
   {
      send_to_char("New password must be at least five characters long.\n\r", ch);
      return;
   }

   if (strcmp(arg2, arg3))
   {
      send_to_char("New password verification failed, please try again.\n\r", ch);
      return;
   }

   /*
   * No tilde allowed because of player file format.
   */
   /*
      Switched to md5 passwords - Fizzfaldt
   pwdnew = crypt( arg2, ch->name );
   */
   pwdnew = md5_hash(arg2);
   /*
      md5 does not ever have tildes,
      but keep here in case we eventually use something
      that does, or we switch the delimiter in pfiles
   */
   for ( p = pwdnew; *p != '\0'; p++ )
   {
      if ( *p == '~' )
      {
         send_to_char( "New password not acceptable, please try again.\n\r", ch );
         return;
      }
   }
   free_string(ch->pcdata->pwd);
   ch->pcdata->pwd = str_dup(pwdnew);
   free_string(ch->pcdata->old_pwd);
   ch->pcdata->old_pwd = str_dup("");  /* Fully converted to md5 */
   save_char_obj(ch);
   send_to_char("Ok.\n\r", ch);
   sprintf(buf, "Password change: %s", ch->name);
   log_string(buf);
   return;
}



void do_revert(CHAR_DATA *ch, char *argument)
{

   if (shapeshift_revert(ch))
   {
      if (str_cmp(argument, "auto"))
      {
         check_changeling_desc(ch);
      }
      return;
   }
   /*return;*/
   if (!is_affected(ch, gsn_morph_red) && !is_affected(ch, gsn_morph_blue)
   && !is_affected(ch, gsn_morph_black)
   && !is_affected(ch, gsn_morph_green)
   && !is_affected(ch, gsn_morph_white)
   && !is_affected(ch, gsn_morph_winged)
   && !is_affected(ch, gsn_morph_archangel)   )
   {
      send_to_char("But you are already in your normal form.\n\r", ch);
      return;
   }
   send_to_char("Your mind clouds over as you force your body to change.\n\r", ch);
   send_to_char("With fluid motion you revert back to your natural form!\n\r", ch);
   act("$n seems to shift and lose solidity as $s reforms.", ch, NULL, NULL, TO_ROOM);

   affect_strip(ch, gsn_morph_red);
   affect_strip(ch, gsn_morph_black);
   affect_strip(ch, gsn_morph_blue);
   affect_strip(ch, gsn_morph_green);
   affect_strip(ch, gsn_morph_white);
   affect_strip(ch, gsn_morph_winged);
   affect_strip(ch, gsn_morph_archangel);

   ch->morph_form[0] = 0;
   ch->morph_form[1] = 0;
   ch->morph_form[2] = 0;
   act("With a slight shiver $n reverts to $s natural form!", ch, NULL, NULL, TO_ROOM);
   return;
}

void do_morph_dragon(CHAR_DATA *ch, char *argument)
{
   AFFECT_DATA af;
   CHAR_DATA *morphtarget=NULL;
   char form_type[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   int morph_sn;
   int form_num, breath_num;
   bool changeling = FALSE;
   struct lvl_check
   {
      char *color;
      int min_level;
   };
   static const struct lvl_check form_check[] =
   {
      {" ", 0},
      {"red", 42},
      {"black", 50},
      {"blue", 46},
      {"white", 48},
      {"green", 44},
      {"goat", 61},
      {"lion", 61},
      {"dragon", 61},
      {"cloaked", 61},
      {"arm", 53}, /* was 15 */
      {"bat", 10},
      {"wolf", 20},
      {"bear", 30},
      {"gaseous", 53},
      {"mimic", 61},
      {"baboon", 61},
      {"winged", 61},
      {"archangel", 61},
      {"", 0}
   };

   one_argument(argument, form_type);
   if (!str_cmp(form_type, "red"))
   {
      breath_num = gsn_fire_breath;
      form_num = 1;
      morph_sn = gsn_morph_red;
   }
   else if (!str_cmp(form_type, "black"))
   {
      breath_num = gsn_acid_breath;
      form_num = 2;
      morph_sn = gsn_morph_black;
   }
   else if (!str_cmp(form_type, "blue"))
   {
      breath_num = gsn_lightning_breath;
      form_num = 3;
      morph_sn = gsn_morph_blue;

   }
   else if (!str_cmp(form_type, "white"))
   {
      breath_num = gsn_frost_breath;
      form_num = 4;
      morph_sn = gsn_morph_white;
   }
   else if (!str_cmp(form_type, "green"))
   {
      breath_num = gsn_gas_breath;
      form_num = 5;
      morph_sn = gsn_morph_green;
   }

   else if (!str_cmp(form_type, "winged"))
   {
      breath_num = gsn_gas_breath;
      form_num = 17;
      morph_sn = gsn_morph_winged;
   }
   else if (!str_cmp(form_type, "archangel"))
   {
      breath_num = gsn_gas_breath;
      form_num = 18;
      morph_sn = gsn_morph_archangel;
   }
   /*    else if (!str_cmp(form_type, "arm"))
   {
   breath_num = 0;
   form_num = MORPH_ARM;
   morph_sn = gsn_shapeshift;
   } */
   else if (!str_cmp(form_type, "bat"))
   {
      breath_num = 0;
      form_num = MORPH_BAT;
      morph_sn = gsn_shapeshift;
   }
   else if (!str_cmp(form_type, "wolf"))
   {
      breath_num = 0;
      form_num = MORPH_WOLF;
      morph_sn = gsn_shapeshift;
   }
   else if (!str_cmp(form_type, "bear"))
   {
      breath_num = 0;
      form_num = MORPH_BEAR;
      morph_sn = gsn_shapeshift;
   }
   else if (!str_cmp(form_type, "revert"))
   {
      if (is_affected(ch, gsn_shapeshift))
      {
         changeling = TRUE;
      }
      do_revert(ch, "auto");
      if
      (
         changeling &&
         !is_affected(ch, gsn_shapeshift)
      )
      {
         check_changeling_desc(ch);
      }
      return;
   }
   else if ((form_type[0] != '\0') && (morphtarget = get_char_room(ch, form_type)) != NULL)
   {
      if (oblivion_blink(ch, morphtarget))
      {
         return;
      }
      breath_num = 0;
      form_num = MORPH_MIMIC;
      morph_sn = gsn_shapeshift;
   }
   else
   {
      form_num = -1;
      breath_num = 0;
      morph_sn = 0;
      /*
      send_to_char("Which form was that?\n\r", ch);
      return;
      */
   }

   if (!can_shapeshift(ch, form_num, morphtarget))
   return;

   /*shapeshift_revert(ch);*/
   if (is_affected(ch, gsn_shapeshift))
   {
      changeling = TRUE;
   }
   if (ch->morph_form[0] != 0) do_revert(ch, "auto");

   if ((ch->level < form_check[form_num].min_level) && (morphtarget == NULL))
   {
      send_to_char("You are not skilled enough to take that form yet.\n\r", ch);
      if
      (
         changeling &&
         !is_affected(ch, gsn_shapeshift)
      )
      {
         check_changeling_desc(ch);
      }
      return;
   }

   if (morphtarget != NULL && is_affected(morphtarget, gsn_cloak_form))
   {
      send_to_char("You can't take the form of a cloaked figure.\n\r", ch);
      if
      (
         changeling &&
         !is_affected(ch, gsn_shapeshift)
      )
      {
         check_changeling_desc(ch);
      }
      return;
   }
   if (ch == morphtarget)
   {
      send_to_char("Having identity problems?\n\r", ch);
      if
      (
         changeling &&
         !is_affected(ch, gsn_shapeshift)
      )
      {
         check_changeling_desc(ch);
      }
      return;
   }

   send_to_char("You go into a trance as your form shifts.\n\r", ch);

   if (number_percent() > get_skill(ch, morph_sn) )
   {
      send_to_char("With a shudder you lose concentration and fail the shapeshifting.\n\r", ch);
      ch->mana -= 50;
      if
      (
         changeling &&
         !is_affected(ch, gsn_shapeshift)
      )
      {
         check_changeling_desc(ch);
      }
      return;
   }

   if ((morphtarget != NULL) && (morphtarget->morph_form[0] != 0))
   {
      send_to_char("You find the form too complicated and fail the shapeshifting.\n\r", ch);
      ch->mana -= 50;
      if
      (
         changeling &&
         !is_affected(ch, gsn_shapeshift)
      )
      {
         check_changeling_desc(ch);
      }
      return;
   }

   if (IS_IMMORTAL(ch))
   {
      send_to_char("Better to remain who you are: you are important.\n\r", ch);
      if
      (
         changeling &&
         !is_affected(ch, gsn_shapeshift)
      )
      {
         check_changeling_desc(ch);
      }
      return;
   }

   if ((morphtarget != NULL) &&
   ((IS_IMMORTAL(morphtarget) && !IS_IMMORTAL(ch)) || (ch->level + 6 < morphtarget->level)
   || (number_percent() > 90)))
   {
      send_to_char("You find the form too challenging and fail the shapeshifting.\n\r", ch);
      ch->mana -= 50;
      if
      (
         changeling &&
         !is_affected(ch, gsn_shapeshift)
      )
      {
         check_changeling_desc(ch);
      }
      return;
   }

   if (ch->morph_form[0] != 0)
   {
      if
      (
         changeling &&
         !is_affected(ch, gsn_shapeshift)
      )
      {
         check_changeling_desc(ch);
      }
      return;
      act("$n seems to shift and fluidly change.", ch, NULL, NULL, TO_ROOM);
      send_to_char("You concentrate a moment and fluidly change your form!\n\r", ch);
   }
   else if (form_num == MORPH_MIMIC)
   {
      act("$n's body suddenly seems to flow into $N's shape!", ch, NULL, morphtarget, TO_NOTVICT);
      act("$n's body suddenly seems to flow into your shape!", ch, NULL, morphtarget, TO_VICT);
      sprintf(buf,  "You concentrate a moment and fluidly change your form!\n\r");
      send_to_char( buf, ch);
   }
   else
   {
      act("$n's body suddenly seems to flow into a new shape!", ch, NULL, NULL, TO_ROOM);
      sprintf(buf,  "You concentrate a moment and fluidly change your form!\n\r");
      send_to_char( buf, ch);
   }

   affect_strip(ch, morph_sn);
   if (form_num > MORPH_MIMIC)
   form_num = 0;
   affect_strip(ch, gsn_shapeshift);
   affect_strip(ch, gsn_gaseous_form);

   WAIT_STATE(ch, skill_table[morph_sn].beats);
   ch->mana -= 50;
   ch->morph_form[1] = breath_num;
   ch->morph_form[2] = ch->level/5;
   if (form_num == 0)
   return;

   af.where = TO_AFFECTS;
   af.type = morph_sn;
   af.level = ch->level;
   af.bitvector = 0;
   if (morphtarget == NULL)
   af.duration = -1;
   else
   {
      if (IS_NPC(morphtarget))
      af.duration = 10+number_range(0, ch->level-morphtarget->level);
      else
      af.duration = 5+number_range(0, (ch->level-morphtarget->level) / 2);
      ch->mprog_target = morphtarget;
   }
   af.location = APPLY_MORPH_FORM;
   af.modifier = form_num;
   affect_to_char(ch, &af);
   if
   (
      form_num != MORPH_MIMIC &&
      form_num > 0 &&
      form_num < MORPH_MIMIC &&
      morph_sn == gsn_shapeshift
   )
   {
      switch_desc(ch, form_num);
   }
   if (form_num == 11)    /* bat */
   {
      af.modifier = 0;
      af.location = 0;
      af.bitvector = AFF_FLYING;
      affect_to_char(ch, &af);
      af.modifier = -1;
      af.location = APPLY_HEIGHT;
      af.bitvector = 0;
      affect_to_char(ch, &af);
      if (IS_FLYING(ch))
      send_to_char("Your feet rise off the ground as you change into bat form.\n\r", ch);
      else
      send_to_char("You change into a bat...but remain on the ground!\n\r", ch);
   }
   if (form_num == 12)    /* wolf */
   {
      af.modifier = 0;
      af.location = 0;
      af.bitvector = AFF_HASTE;
      affect_to_char(ch, &af);
      send_to_char("You feel yourself moving faster in wolf form.\n\r", ch);
   }
   if (form_num == 13)    /* bear */
   {
      af.modifier = ch->level;
      af.location = APPLY_HIT;
      affect_to_char(ch, &af);
      send_to_char("You feel much tougher.\n\r", ch);
   }
   if (form_num == MORPH_MIMIC)
   {
      send_to_char("You are not sure how long you can maintain this form.\n\r", ch);
   }

   check_changeling_desc(ch);

   if (form_num > 5)
   return;

   af.location = APPLY_STR;
   af.modifier = ch->level/10;
   affect_to_char(ch, &af);
   af.location = APPLY_DEX;
   af.modifier = -3;
   affect_to_char(ch, &af);
   af.location = APPLY_HITROLL;
   af.modifier = ch->level/10;
   affect_to_char(ch, &af);
   af.location = APPLY_DAMROLL;
   af.modifier = ch->level/10;
   affect_to_char(ch, &af);
   af.location = APPLY_HEIGHT;
   af.modifier = 1;
   affect_to_char(ch, &af);
   return;
}

char * get_descr_form(CHAR_DATA *ch, CHAR_DATA *looker, bool get_long)
{
   struct l_form
   {
      char *short_descr;
      char *long_descr;
   };
   static char buf[MAX_STRING_LENGTH];
   char *buf2;
   OBJ_DATA * obj;
   int genocide = FALSE;

   static const struct l_form look_form[] =
   {
      {" ", " "},
      {"a red dragon", "A red dragon breathing plumes of smoke"},
      {"a black dragon", "A black dragon spitting burning acids"},
      {"a blue dragon", "A blue dragon spitting lighting bolts"},
      {"a white dragon", "A white dragon sends out blasts of freezing air"},
      {"a green dragon", "A green dragon breathing noxious poison"},
      {"a goat headed", "A goat headed person"},
      {"a lion headed", "A lion headed person"},
      {"a dragon headed", "A dragon headed person"},
      {"a cloaked figure", "A cloaked figure"},
      {"normal", "the three armed.\n\r"},
      {"normal", "A huge bat"},
      {"normal", "A large grey wolf"},
      {"normal", "A large, mean looking bear"},
      {"a gaseous cloud", "A cloud of boiling gasses"},
      {"normal", "An odd-looking person"},
      {"a baboon-headed", "An odd baboon-headed person"},
      {"a winged centaur", "A winged centaur"},
      {"a shining archangel", "An archangel bathed in radiance"},
      {"normal", "A twisted, half-dracolich aberration of evil"},
      {"", ""}
   };

   buf[0] = '\0';
   buf2 = "";

   if (!ch)
   {
      return "someone";
   }
   if (!can_see(looker, ch) && !get_long)
   {
      if
      (
         !IS_NPC(ch) &&
         IS_IMMORTAL(ch) &&
         (
            (
               looker != NULL &&
               (
                  ch->invis_level > get_trust(looker) ||
                  (
                     ch->incog_level > get_trust(looker) &&
                     ch->in_room != looker->in_room
                  ) ||
                  ch->nosee == looker
               )
            ) ||
            (
               looker == NULL &&
               ch->invis_level >= LEVEL_IMMORTAL
            ) ||
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
      return "a divine presence";
      else
      return "someone";
   }
   /*
   if (!can_see(looker, ch) && !get_long)
   {
   if (!IS_IMMORTAL(ch) || ch->invis_level < 52)
   return "someone";
   else
   return "a divine presence";
   }
   */
   /* cloaked figure case */
   if (is_affected(ch, gsn_cloak_form))
   {
      if (get_long)
      {
         switch(ch->morph_form[0])
         {
            default: case 0: case 9:
            sprintf(buf,  "A cloaked figure"); break;
            case 1: case 2: case 3: case 4: case 5:
            sprintf(buf,  "A cloaked dragon"); break;
            case 6:
            sprintf(buf,  "A cloaked goat headed man"); break;
            case 7:
            sprintf(buf,  "A cloaked lion headed man"); break;
            case 8:
            sprintf(buf,  "A cloaked dragon headed man"); break;
            case 11: sprintf(buf,  "A cloaked bat"); break;
            case 12: sprintf(buf,  "A cloaked wolf"); break;
            case 13: sprintf(buf,  "A cloaked bear"); break;
            case 16: sprintf(buf,  "A cloaked baboon-headed man"); break;
         }
         if
         (
            looker != NULL &&
            !IS_NPC(looker) &&
            IS_TRUSTED(looker, ANGEL) &&
            IS_SET(looker->act, ACT_HOLYLIGHT)
         )
         {
            strcat(buf, " (");
            strcat(buf, ch->name);
            strcat(buf, ")");
         }
         append_position(buf, ch, looker);
         buf2 = buf;
         return buf2;
      }
      else
      {
         strcpy(buf, "a cloaked figure");
         if
         (
            looker != NULL &&
            !IS_NPC(looker) &&
            !IS_NPC(ch) &&
            can_see(looker, ch) &&
            (
               (
                  IS_TRUSTED(looker, ANGEL) &&
                  IS_SET(looker->act, ACT_HOLYLIGHT)
               ) ||
               looker->house == HOUSE_ANCIENT ||
               (
                  looker->house == HOUSE_ENFORCER &&
                  has_skill(looker, gsn_perception) &&
                  get_skill(looker, gsn_perception) > 0
               )
            ) &&
            ch->pcdata->moniker[0] != '\0'
         )
         {
            strcat(buf, " (");
            strcat(buf, ch->pcdata->moniker);
            strcat(buf, ")");
         }
         return (buf2 = buf);
      }
   }

   /* toad case */
   if (is_affected(ch, gsn_feign_death) && get_long)
   {
      if
      (
         ch->in_room->sector_type == SECT_WATER_SWIM ||
         ch->in_room->sector_type == SECT_WATER_NOSWIM
      )
      {
         sprintf(buf,  "     The corpse of %s is floating here.\n\r", get_longname(ch, NULL));
      }
      else if
      (
         ch->in_room->sector_type == SECT_UNDERWATER ||
         ch->in_room->sector_type == SECT_AIR
      )
      {
         sprintf(buf,  "     The corpse of %s is here.\n\r", get_longname(ch, NULL));
      }
      else
      {
         sprintf(buf,  "     The corpse of %s is lying here.\n\r", get_longname(ch, NULL));
      }
      buf2 = buf;
      return buf2;
   }
   if (is_affected(ch, gsn_toad))
   {
      if (get_long)
      {
         sprintf(buf,  "A small toad");
         append_position(buf, ch, looker);
         buf2 = buf;
         return buf2;
      }
      else
      {
         return "a small toad";
      }
   }

   /* genocide case */
   obj = get_eq_char(ch, WEAR_WIELD);
   if (obj != NULL && obj->pIndexData->vnum == OBJ_VNUM_GENOCIDE_AXE)
   genocide = TRUE;
   obj = get_eq_char(ch, WEAR_DUAL_WIELD);
   if (obj != NULL && obj->pIndexData->vnum == OBJ_VNUM_GENOCIDE_AXE)
   genocide = TRUE;
   if (genocide)
   {
      if (get_long)
      {
         sprintf(buf,  "A black knight radiating hatred");
         append_position(buf, ch, looker);
         buf2 = buf;
         return buf2;
      }
      else
      {
         return "A Black Knight";
      }
   }

   /* morph mimic case */
   if ((ch->morph_form[0] == MORPH_MIMIC) && (ch->mprog_target != NULL))
   {
      if (get_long)
      {
         if (is_affected(ch->mprog_target, gsn_cloak_form))
         {
            return "";
         }
         else if (!IS_NPC(ch->mprog_target))
         {
            sprintf(buf, ch->mprog_target->long_descr);
            append_position(buf, ch, looker);
            buf2 = buf;
            return buf2;
         }
         else
         {
            return ch->mprog_target->long_descr;
         }
      }
      else
      {
         return NPERS(ch->mprog_target, NULL);
      }
   }
   /* other cases */
   if (get_long)
   {
      if (IS_NPC(ch))
      return ch->long_descr;
      if (ch->morph_form[0] == 0)
      sprintf(buf, "%s", ch->long_descr);
      else if (ch->morph_form[0] != MORPH_ARM)
      {
         sprintf(buf, "%s", look_form[ch->morph_form[0]].long_descr);
      }
      else
      {
         sprintf(buf, "%s %s", ch->name, look_form[MORPH_ARM].long_descr);
         buf2 = buf;
         return buf2;
      }
      append_position(buf, ch, looker);
      buf2 = buf;
      return buf2;
   }

   /* not get_long */
   if (ch->morph_form[0] == 0)
   return PERS(ch, looker);

   if (
         ch->morph_form[0] == MORPH_DRACOLICH &&
         is_affected(ch, gsn_dracolich_bloodlust)
      )
   {
      return "Shal'ne-Sir";
   }

   if (!str_cmp(look_form[ch->morph_form[0]].short_descr, "normal") )
   return PERS(ch, looker);
   if ((ch->morph_form[0] < 6) || (ch->morph_form[0] >= 9))
   return look_form[ch->morph_form[0]].short_descr;
   else if (ch->sex == SEX_MALE)
   sprintf(buf, "%s %s", look_form[ch->morph_form[0]].short_descr, "man");
   else if (ch->sex == SEX_FEMALE)
   sprintf(buf, "%s %s", look_form[ch->morph_form[0]].short_descr, "woman");
   else
   sprintf(buf, "%s %s", look_form[ch->morph_form[0]].short_descr, "creature");
   buf2 = buf;
   return buf2;

}

/* Replacement for the handler.c get_char_room to handle morph dragon */
CHAR_DATA* get_char_room(CHAR_DATA* ch, char* argument)
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA* rch;
   int number;
   int count;
   bool pconly = FALSE;
   bool mobonly = FALSE;

   if (ch == NULL)
   {
      return NULL;
   }
   if
   (
      (argument[0] == '+') &&
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
   count = 0;
   if (!str_cmp(arg, "self"))
   {
      return ch;
   }
   for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room)
   {
      if
      (
         !can_see(ch, rch) ||
         (
            pconly &&
            IS_NPC(rch)
         ) ||
         (
            mobonly &&
            !IS_NPC(rch)
         ) ||
         (
            !mobonly &&
            IS_NPC(rch) &&
            IS_IMMORTAL(rch) &&
            (
               rch->pIndexData->vnum == MOB_VNUM_DECOY ||
               rch->pIndexData->vnum == MOB_VNUM_SHADOW ||
               rch->pIndexData->vnum == MOB_VNUM_MIRROR ||
               rch->pIndexData->vnum == MOB_VNUM_PHANTOM
            )
         )
      )
      {
         continue;
      }
      if
      (
         (
            arg[0] == '\0' ||
            is_name(arg, get_name(rch, ch)) ||
            (
               rch->morph_form[0] != 0 &&
               (
                  (
                     rch->morph_form[0] < 6 &&
                     !str_cmp(arg, "dragon")
                  ) ||
                  (
                     rch->morph_form[0] == MORPH_GOAT_HEAD &&
                     !str_cmp(arg, "goat")
                  ) ||
                  (
                     rch->morph_form[0] == MORPH_LION_HEAD &&
                     !str_cmp(arg, "lion")
                  ) ||
                  (
                     rch->morph_form[0] == MORPH_DRAGON_HEAD &&
                     !str_cmp(arg, "dragon")
                  ) ||
                  (
                     rch->morph_form[0] == MORPH_BAT &&
                     !str_cmp(arg, "bat")
                  ) ||
                  (
                     rch->morph_form[0] == MORPH_WOLF &&
                     !str_cmp(arg, "wolf")
                  ) ||
                  (
                     rch->morph_form[0] == MORPH_BEAR &&
                     !str_cmp(arg, "bear")
                  ) ||
                  (
                     rch->morph_form[0] == MORPH_MIMIC &&
                     rch->mprog_target != NULL &&
                     is_name(arg, rch->mprog_target->name)
                  ) ||
                  (
                     rch->morph_form[0] == MORPH_BABOON &&
                     !str_cmp(arg, "baboon")
                  ) ||
                  (
                     rch->morph_form[0] == MORPH_WINGED &&
                     !str_cmp(arg, "winged")
                  ) ||
                  (
                     rch->morph_form[0] == MORPH_ARCHANGEL &&
                     !str_cmp(arg, "archangel")
                  ) ||
                  (
                     rch->morph_form[0] == MORPH_DRACOLICH &&
                     !str_cmp(arg, "dracolich")
                  )
               )
            )
         ) &&
         ++count == number
      )
      {
         return rch;
      }
   }
   return NULL;
}


void do_eye_of_the_predator(CHAR_DATA *ch, char *argument)
{
   AFFECT_DATA af;

   if ( (get_skill(ch, gsn_eye_of_the_predator) <= 0)
   || /*(ch->level < skill_table[gsn_eye_of_the_predator].skill_level[ch->class])*/
   !has_skill(ch, gsn_eye_of_the_predator) )
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }

   if (house_down(ch, HOUSE_ANCIENT))
   return;

   if (!is_affected(ch, gsn_cloak_form)) {
      send_to_char("You may only invoke that power while concealed.\n\r", ch);
      return;
   }

   if (is_affected(ch, gsn_eye_of_the_predator) )
   {
      send_to_char("You already watch things through preadatory eyes.\n\r", ch);
      return;
   }
   if (ch->mana < 40)
   {
      send_to_char("You do not have the mana.\n\r", ch);
      return;
   }

   if ((!IS_NPC(ch)) && (number_percent() > ch->pcdata->learned[gsn_eye_of_the_predator] ))
   {
      send_to_char("Your vision sharpens but then fades.\n\r", ch);
      check_improve(ch, gsn_eye_of_the_predator, FALSE, 1);
      ch->mana -= 20;
      return;
   }

   af.where = TO_AFFECTS;
   af.type = gsn_eye_of_the_predator;
   af.level = ch->level;
   af.modifier = 0;
   af.location = 0;
   af.duration = ch->level/2;
   af.bitvector = AFF_DETECT_INVIS;
   send_to_char("You begin watching things through the eyes of a hunter.\n\r", ch);
   send_to_char("The shadows are revealed to you.\n\r", ch);
   affect_to_char(ch, &af);
   af.bitvector = AFF_DETECT_HIDDEN;
   affect_to_char( ch, &af );
   /*
   if (ch->level>50&&!is_affected(ch, gsn_acute_vision)) {
   af.type = gsn_acute_vision;
   af.bitvector = 0;
   affect_to_char(ch, &af);
   send_to_char("You begin to see through the twigs and brush.\n\r", ch);
   }
   */
   ch->mana -= 40;
   check_improve(ch, gsn_eye_of_the_predator, TRUE, 1);

   return;
}

void do_cforsaken(CHAR_DATA *ch, char *argument)
{
   CHAR_DATA *criminal;
   char arg[MAX_INPUT_LENGTH];
   char status[MAX_INPUT_LENGTH];
   char *next_arg;
   bool wanted;

   wanted = FALSE;
   next_arg = one_argument(argument, arg);
   one_argument(next_arg, status);
   if (IS_NPC(ch))
   return;

   if (!(ch->pcdata->induct >= 3 &&
   ch->house == HOUSE_CRUSADER))
   {
      send_to_char("Only Crusader Immortal Leaders may declare Forsaken.\n\r", ch);
      return;
   }

   if (!str_cmp(status, "yes"))
   wanted = TRUE;
   else if (!str_cmp(status, "no"))
   ;
   else
   {
      send_to_char("You must use CFORSAKEN PLAYER (YES/NO).\n\r", ch);
      return;
   }

   if (( criminal = get_char_world(ch, arg) ) == NULL )
   {
      send_to_char("That person isn't here.\n\r", ch);
      return;
   }
   if (IS_NPC(criminal))
   {
      send_to_char("You can only declare other players as Forsaken.\n\r", ch);
      return;
   }

   if (wanted)
   {
      if (IS_SET(criminal->wiznet, PLR_FORSAKEN))
      {
         send_to_char("That person is already Forsaken.\n\r", ch);
         return;
      }
      else
      {
         SET_BIT(criminal->wiznet, PLR_FORSAKEN);
         act("$N is now Forsaken.", ch, NULL, criminal, TO_CHAR);
         return;
      }
   }

   if (!IS_SET(criminal->wiznet, PLR_FORSAKEN))
   {
      send_to_char("That person is not Forsaken.\n\r", ch);
      return;
   }
   else
   {
      REMOVE_BIT(criminal->wiznet, PLR_FORSAKEN);
      act("$N is no longer Forsaken.", ch, NULL, criminal, TO_CHAR);
   }
   return;
}

void do_bounty(CHAR_DATA *ch, char *argument)
{
   CHAR_DATA *criminal = NULL;
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   char arg3[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   bool wanted, add_ok;
   int  amount = 0;

   wanted = TRUE;
   add_ok = FALSE;

   if (str_cmp(argument, "list") && argument[0] != '\0')
   {
      sprintf(log_buf,  "%s used bounty: %s", ch->name, argument);
      log_string(log_buf);
   }
   argument = one_argument(argument, arg1);
   argument = one_argument(argument, arg2);
   argument = one_argument(argument, arg3);

   if (!str_cmp(arg1, "cloaked") || !str_cmp(arg1, "figure"))
   {
      send_to_char("Ancient will not take jobs on itself.\n\r", ch);
      return;
   }
   if (!IS_IMMORTAL(ch) && IS_SET(ch->in_room->extra_room_flags, ROOM_1212)){
      send_to_char("The gods prevent that from here.\n\r", ch);
      return;
   }

   if (!str_cmp(arg3, "add"))
   add_ok = TRUE;
   if (IS_NPC(ch) && (ch->pIndexData->vnum != MOB_VNUM_ANCIENT))
   return;


   if (!str_cmp(arg1, "list"))
   {
      mark_list(ch);
      return;
   }

   if (arg1[0] == '\0')
   {
      mark_list(ch);
      return;
   }


   if (( criminal = get_char_world(ch, arg1) ) == NULL )
   {
      send_to_char("That person isn't here.\n\r", ch);
      return;
   }
   if (IS_NPC(criminal))
   {
      send_to_char("That person isn't here.\n\r", ch);
      return;
   }

   if (criminal == ch && ch->house != HOUSE_ANCIENT)
   {
      send_to_char("Have you gone mad?\n\r", ch);
      return;
   }
   if (IS_NPC(ch) && (!IS_SET(criminal->act2, PLR_ANCIENT_MARK) ||
   criminal->pcdata->mark_price < 1000))
   {
      SET_BIT(criminal->act2, PLR_ANCIENT_MARK);
      if (criminal->pcdata->marker != str_empty)
      free_string(criminal->pcdata->marker);
      criminal->pcdata->marker = str_dup("The Assassin");
      criminal->pcdata->mark_time = current_time;
      criminal->pcdata->mark_accepted = TRUE;
      return;
   }

   if   (!IS_SET(criminal->act2, PLR_ANCIENT_MARK) &&
   !IS_SET(criminal->act2, PLR_ANCIENT_MARK_PERM))
   {
      add_ok = FALSE;
   }

   if (criminal->house == HOUSE_ANCIENT || IS_IMMORTAL(criminal))
   {
      send_to_char("That person isn't here.\n\r", ch);
      return;
   }

   if (!str_cmp(arg2, "refund") &&
   !str_cmp(criminal->pcdata->marker, ch->name))
   {
      sprintf(buf,  "%s's bounty refunded", criminal->name);
      sprintf(log_buf,  "%s's bounty refunded by %s.\n\r",
      criminal->name, ch->name);
      make_note("Mark Info", buf, "ancient", log_buf, NOTE_DEATH);
      collect_mark_refund(ch, criminal);
      return;
   }

   if (!str_cmp(arg2, "refund"))
   {
      send_to_char("You can't do that.\n\r", ch);
      return;
   }

   if (!str_cmp(arg2, "accept"))
   {
      DESCRIPTOR_DATA *d;
      bool marker_found = FALSE;
      CHAR_DATA *wch = NULL;

      if (ch->house != HOUSE_ANCIENT)
      {
         send_to_char("Only the Ancients may accept bounties.\n\r", ch);
         return;
      }

      if (criminal->pcdata->mark_price <= 0)
      {
         send_to_char("No client has bountied this mark.\n\r", ch);
         return;
      }

      for (d = descriptor_list; d != NULL; d = d->next)
      {

         if (d->connected != CON_PLAYING || !can_see(ch, d->character))
         continue;

         wch = ( d->original != NULL ) ? d->original : d->character;

         if (str_cmp(criminal->pcdata->orig_marker, wch->name)) continue;

         marker_found = TRUE;
         break;
      }

      if
      (
         marker_found &&
         (
            IS_NPC(ch) ||
            IS_NPC(wch) ||
            IS_IMMORTAL(ch) ||
            IS_IMMORTAL(wch) ||
            ch->in_room == wch->in_room
         )
      )
      {
         act("Your bounty is accepted: $n is now being hunted by Ancient.", criminal, NULL, wch, TO_VICT);
         SET_BIT(criminal->act2, PLR_ANCIENT_MARK);
         criminal->pcdata->mark_accepted = TRUE;
         send_to_char("They are now being hunted by Ancient.\n\r", ch);
      }
      else
      {
         send_to_char("The client is not here.\n\r", ch);
      }
      return;
   }

   if (ch->house == HOUSE_ANCIENT)
   {
      if (!str_cmp(arg2, "yes"))
      wanted = TRUE;
      else if (!str_cmp(arg2, "no"))
      wanted = FALSE;
   }
   else
   {
      if (!is_number(arg2))
      {
         send_to_char("How much are you willing to pay again?\n\r", ch);
         return;
      }
      amount = atoi(arg2);
      if (amount < 1000  && !add_ok)
      {
         send_to_char("Not at that price.\n\r", ch);
         return;
      }
   }

   if (wanted)
   {
      if   (IS_SET(criminal->act2, PLR_ANCIENT_MARK) ||
      IS_SET(criminal->act2, PLR_ANCIENT_MARK_PERM))
      {
         if (!add_ok)
         {
            send_to_char("That person is already being hunted\n\rUse: bounty <name> <amount> add, to add to the price may add to the price.\n\r", ch);
            return;
         }
      }
      else add_ok = FALSE;
   }

   if (ch->gold < amount){
      send_to_char("You have to be able to pay in gold.\n\r", ch);
      return;
   }

   ch->gold -= amount;

   if (wanted){
      if (!add_ok)
      {
         if (IS_SET(ch->act2, PLR_IS_ANCIENT) || (ch->house == HOUSE_ANCIENT))
         {
            send_to_char("They are now being hunted by Ancient.\n\r", ch);
            SET_BIT(criminal->act2, PLR_ANCIENT_MARK);
            criminal->pcdata->mark_accepted = TRUE;
         }
         else
         {
            bool ancient_found = FALSE;
            DESCRIPTOR_DATA *d;
            char buf[MAX_STRING_LENGTH];

            for (d = descriptor_list; d != NULL; d = d->next)
            {
               CHAR_DATA *wch;

               if (d->connected != CON_PLAYING || !can_see(ch, d->character))
               continue;

               wch = ( d->original != NULL ) ? d->original : d->character;

               if (wch->house != HOUSE_ANCIENT) continue;

               if (!can_see(ch, wch))
               continue;

               if ( is_affected(wch, gsn_submerge)
               && wch != ch && !IS_IMMORTAL(ch)) continue;
               if ((is_affected(wch, gsn_stalking) && !IS_IMMORTAL(ch) && ch->house != wch->house)
               && (number_percent()<(get_skill(wch, gsn_stalking)-10))) continue;

               ancient_found = TRUE;

               sprintf
               (
                  buf,
                  ">>> A bounty for the %s of %s is offered by $n. <<<",
                  (
                     criminal->race != grn_arborian ?
                     (
                        criminal->race != grn_book ?
                        "scalp" :
                        "tassle"
                     ) :
                     "root"
                  ),
                  get_longname(criminal, NULL)
               );
               act(buf, ch, NULL, wch, TO_VICT);
            }

            if (ancient_found)
            send_to_char("Ancient is aware of your intentions.", ch);
            else
            {
               ch->gold += amount;
               send_to_char("You must make this offer to the nearest Ancient for it to be considered.\n\r", ch);
               return;
            }
         }
      }
      else
      send_to_char("The price on their head is even higher.\n\r", ch);

      /* SET_BIT(criminal->act2, PLR_ANCIENT_MARK); */
      if (!add_ok){
         if (criminal->pcdata->marker != str_empty)
         free_string(criminal->pcdata->marker);
         if (criminal->pcdata->orig_marker != str_empty)
         free_string(criminal->pcdata->orig_marker);
         criminal->pcdata->orig_marker = str_dup(ch->name);
         criminal->pcdata->marker = str_dup(get_longname(ch, NULL));
         criminal->pcdata->mark_time = current_time;
      }
      criminal->pcdata->mark_price += amount;
   }
   else
   {
      REMOVE_BIT(criminal->act2, PLR_ANCIENT_MARK);
      if (criminal->pcdata->marker != str_empty)
      {
         free_string(criminal->pcdata->marker);
         criminal->pcdata->marker = &str_empty[0];
      }
      if (criminal->pcdata->orig_marker != str_empty)
      {
         free_string(criminal->pcdata->orig_marker);
         criminal->pcdata->orig_marker = &str_empty[0];
      }

      criminal->pcdata->mark_price = 0;
      criminal->pcdata->mark_time = 0;
      criminal->pcdata->mark_accepted = FALSE;
      send_to_char("They are no longer being hunted.\n\r", ch);
   }
   return;
}

void do_mark(CHAR_DATA *ch, char *argument)
{
   CHAR_DATA *criminal;
   char arg[MAX_INPUT_LENGTH];
   char status[MAX_INPUT_LENGTH];
   char perm[MAX_INPUT_LENGTH];
   char *next_arg;
   bool wanted;

   wanted = FALSE;
   next_arg = one_argument(argument, arg);
   next_arg = one_argument(next_arg, status);
   one_argument(next_arg, perm);
   if (IS_NPC(ch) && (ch->pIndexData->vnum != MOB_VNUM_ANCIENT))
   return;
   if (IS_SET(ch->act2, PLR_NOVICE))
   {
      send_to_char("Novices shouldn't use mark.\n\r", ch);
      return;
   }

   if (!IS_IMMORTAL(ch)){
      if
      (
         !IS_NPC(ch) &&
         !(ch->pcdata->induct >= 3 &&
         (ch->house == HOUSE_LIGHT)) &&
         (ch->house != HOUSE_OUTLAW ) &&
         (ch->house != HOUSE_MARAUDER )
      )
      {
         send_to_char("Huh?\n\r", ch);
         return;
      }
   }
   if ((ch->house == HOUSE_LIGHT && house_down(ch, HOUSE_LIGHT)) ||
   (ch->house == HOUSE_ANCIENT && house_down(ch, HOUSE_ANCIENT)) ||
   (ch->house == HOUSE_OUTLAW && house_down(ch, HOUSE_OUTLAW)) ||
   (ch->house == HOUSE_MARAUDER && house_down(ch, HOUSE_MARAUDER)))
   return;

   if (!str_cmp(status, "yes"))
   wanted = TRUE;
   else if (!str_cmp(status, "no"))
   ;
   else
   {
      send_to_char("You must use MARK PLAYER (YES/NO).\n\r", ch);
      return;
   }

   if (( criminal = get_char_world(ch, arg) ) == NULL )
   {
      send_to_char("That person isn't here.\n\r", ch);
      return;
   }
   if (IS_NPC(criminal))
   {
      send_to_char("You can only mark other players.\n\r", ch);
      return;
   }

   if (wanted)
   {
      if
      (
         (ch->house == HOUSE_LIGHT && IS_SET(criminal->act, PLR_EVIL)) ||
         (ch->house == HOUSE_ANCIENT &&
         (IS_SET(criminal->act2, PLR_ANCIENT_MARK) ||
         IS_SET(criminal->act2, PLR_ANCIENT_MARK_PERM))) ||
         (
            (
               (ch->house == HOUSE_OUTLAW) || (ch->house == HOUSE_MARAUDER)
            ) && IS_SET(criminal->act, PLR_MARKED)
         )
      )
      {
         send_to_char("That person is already marked.\n\r", ch);
         return;
      }
      else
      {
         if (ch->house == HOUSE_LIGHT)
         SET_BIT(criminal->act, PLR_EVIL);
         else if
         (
            (ch->house == HOUSE_OUTLAW) || (ch->house == HOUSE_MARAUDER)
         )
         {
            SET_BIT(criminal->act, PLR_MARKED);
            switch (number_range(1, 3))
            {
               case 1:
                  send_to_char(
                     "You feel a light touch upon your shoulder and see a dark purple handprint.\n\r",
                     criminal);
                  break;
               case 2:
                  send_to_char(
                     "The gaze of eyes from the shadows can be felt upon you.\n\r",
                     criminal);
                  break;
               case 3:
                  send_to_char(
                     "A young thief bumps into you and whispers, \"'Ware the shadows.\"\n\r",
                     criminal);
                  break;
            }
         }
         else
         {
            if (IS_IMMORTAL(ch) && !str_cmp(perm, "perm"))
            SET_BIT(criminal->act2, PLR_ANCIENT_MARK_PERM);
            else
            {
               SET_BIT(criminal->act2, PLR_ANCIENT_MARK);
               if (!IS_IMMORTAL(ch))
               auto_mark_note(ch, criminal, "was marked by");
            }
         }
         act("$N is now marked.", ch, NULL, criminal, TO_CHAR);
         return;
      }
   }

   if ((!IS_SET(criminal->act, PLR_EVIL) && ch->house == HOUSE_LIGHT) ||
   (!IS_SET(criminal->act2, PLR_ANCIENT_MARK) &&
   !IS_SET(criminal->act2, PLR_ANCIENT_MARK_PERM) &&
   ch->house == HOUSE_ANCIENT) ||
      (
         !IS_SET(criminal->act, PLR_MARKED) &&
         ( ch->house == HOUSE_OUTLAW || ch->house == HOUSE_MARAUDER)
      )
   )
   {
      send_to_char("That person is unmarked.\n\r", ch);
      criminal->pcdata->mark_price = 0;
      criminal->pcdata->mark_time = 0;
      criminal->pcdata->mark_accepted = FALSE;
      return;
   }
   else
   {
      if (ch->house == HOUSE_LIGHT)
      {
         REMOVE_BIT(criminal->act, PLR_EVIL);
      }
      else if ((ch->house == HOUSE_OUTLAW) || (ch->house == HOUSE_MARAUDER))
      {
         REMOVE_BIT(criminal->act, PLR_MARKED);
         send_to_char(
            "The Guild has been appeased; you feel safe once more.\n\r",
            criminal);
      }
      else if (!IS_IMMORTAL(ch) && IS_SET(criminal->act2, PLR_ANCIENT_MARK_PERM))
      {
         act("$N is permanently marked. You cannot remove it.", ch, NULL, criminal, TO_CHAR);
         return;
      }
      else
      {
         CHAR_DATA *tch;

         for (tch = char_list; tch != NULL; tch = tch->next)
         if (!IS_NPC(tch))
         {
            if (!IS_NPC(tch) && tch->pcdata->target == ch)
            {
               tch->pcdata->target = NULL;
               if (is_affected(tch, gsn_target)) affect_strip(tch, gsn_target);
               send_to_char("The mark has been granted reprieve. You stop targetting.\n\r", tch);
            }
         }
         REMOVE_BIT(criminal->act2, PLR_ANCIENT_MARK);
         REMOVE_BIT(criminal->act2, PLR_ANCIENT_MARK_PERM);
         if (!IS_IMMORTAL(ch))
         {
            auto_mark_note(ch, criminal, "was unmarked by");
         }
      }
      act("$N is no longer marked.", ch, NULL, criminal, TO_CHAR);
   }
   return;
}

void do_wanted(CHAR_DATA *ch, char *argument) {
   CHAR_DATA *criminal;
   char arg[MAX_INPUT_LENGTH];
   char status[MAX_INPUT_LENGTH];
   char *next_arg;
   bool wanted;
   CHAR_DATA* vch;
   AFFECT_DATA* paf;

   wanted = FALSE;
   next_arg = one_argument(argument, arg);
   one_argument(next_arg, status);

   if
   (
      IS_NPC(ch) ||
      !(
         ch->house == HOUSE_ENFORCER ||
         IS_TRUSTED(ch, ANGEL)
      )
   )
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }
   if (house_down(ch, HOUSE_ENFORCER))
   {
      return;
   }

   if (!IS_IMMORTAL(ch) && IS_SET(ch->in_room->extra_room_flags, ROOM_1212))
   {
      send_to_char("You can't do that from here.\n\r", ch);
      sprintf(log_buf, "%s tried to use wanted from 1212.", ch->name);
      log_string(log_buf);
      return;
   }
   criminal = NULL;
   for (vch = char_list; vch; vch = vch->next)
   {
      if
      (
         IS_NPC(vch) ||
         (
            IS_IMMORTAL(vch) &&
            !can_see(ch, vch)
         ) ||
         !is_name(arg, get_name(vch, ch))
      )
      {
         continue;
      }
      criminal = vch;
      break;
   }
   if (criminal == NULL)
   {
      send_to_char("That person isn't here.\n\r", ch);
      return;
   }

   if (IS_NPC(criminal))
   {
      send_to_char("You can only flag other players.\n\r", ch);
      return;
   }

   if (oblivion_blink(ch, criminal))
   {
      return;
   }
   if (IS_SET(criminal->act2, PLR_LAWLESS))
   {
      act("$N's fate has already been sealed.", ch, NULL, criminal, TO_CHAR);
      return;
   }
   if (!str_cmp(status, "yes"))
   wanted = TRUE;
   else if (!str_cmp(status, "no"))
   ;
   else if (!str_cmp(status, "punished"))
   {
      if (!IS_SET(criminal->act, PLR_CRIMINAL))
      {
         send_to_char("They can't be punished when they aren't even wanted.\n\r", ch);
         return;
      }
      if (criminal->pcdata->wanteds > 100)
      {
         send_to_char("Their record is too great for that option.\n\r", ch);
         return;
      }
      REMOVE_BIT(criminal->act, PLR_CRIMINAL);
      send_to_char("They have been marked as punished.\n\r", ch);
      send_to_char("Your criminal flag has been removed, but your record remains.\n\r", criminal);
      return;
   }
   else
   {
      send_to_char("You must use WANTED PLAYER (YES/NO).\n\r", ch);
      return;
   }

   if (wanted)
   {
      if (is_affected(criminal, gsn_wanted))
      {
         act
         (
            "You receive a notice that $N's last crime has already been reported.",
            ch,
            NULL,
            criminal,
            TO_CHAR
         );
         return;
      }
      paf = new_affect();
      paf->type = gsn_wanted;
      paf->where = TO_AFFECTS;
      paf->duration = 1;
      affect_to_char(criminal, paf);
      free_affect(paf);
      criminal->pcdata->wanteds += 1;
      if (IS_SET(criminal->act, PLR_CRIMINAL))
      {
         send_to_char
         (
            "Your record with the Justicars has been further tarnished.\n\r",
            criminal
         );
         act
         (
            "$N's record has been adjusted accordingly.",
            ch,
            NULL,
            criminal,
            TO_CHAR
         );
         return;
      }
      else
      {
         SET_BIT(criminal->act, PLR_CRIMINAL);
         send_to_char("You are now WANTED!!\n\r", criminal);
         act("$N is now a criminal.", ch, NULL, criminal, TO_CHAR);
         return;
      }
   }

   if (!IS_SET(criminal->act, PLR_CRIMINAL))
   {
      send_to_char("That person is not a criminal.\n\r", ch);
      return;
   }
   else
   {
      REMOVE_BIT(criminal->act, PLR_CRIMINAL);
      send_to_char("You are no longer wanted.\n\r", criminal);
      act("$N is no longer wanted.", ch, NULL, criminal, TO_CHAR);
      criminal->pcdata->wanteds -= 1;
   }
   return;
}

void do_rebel(CHAR_DATA *ch, char *argument) {
   CHAR_DATA *criminal;
   char arg[MAX_INPUT_LENGTH];
   char status[MAX_INPUT_LENGTH];
   char *next_arg;
   bool wanted;

   wanted = FALSE;
   next_arg = one_argument(argument, arg);
   one_argument(next_arg, status);
   if (IS_NPC(ch))
   return;
   if ( (get_skill(ch, gsn_rebel) <= 0)
   || !has_skill(ch, gsn_rebel))
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }
   if (house_down(ch, HOUSE_EMPIRE))
   return;


   if (!str_cmp(status, "yes"))
   wanted = TRUE;
   else if (!str_cmp(status, "no"))
   ;
   else
   {
      send_to_char("You must use REBEL PLAYER (YES/NO).\n\r", ch);
      return;
   }

   if (( criminal = get_char_world(ch, arg) ) == NULL )
   {
      send_to_char("That person isn't here.\n\r", ch);
      return;
   }
   if (IS_NPC(criminal))
   {
      send_to_char("You can only make players rebels.\n\r", ch);
      return;
   }

   if (oblivion_blink(ch, criminal))
   {
      return;
   }
   if (!IS_IMMORTAL(ch) && IS_SET(ch->in_room->extra_room_flags, ROOM_1212)){
      send_to_char("You can't do that from here.\n\r", ch);
      return;
      sprintf(log_buf, "%s tried to use rebel from 1212.", ch->name);
      log_string(log_buf);
   }

   if (wanted)
   {
      if (IS_SET(criminal->act, PLR_COVENANT_ALLY))
      {
         send_to_char("That person is already a rebel.\n\r", ch);
         return;
      }
      else
      {
         SET_BIT(criminal->act, PLR_COVENANT_ALLY);
         criminal->pcdata->rebels++;
         if (!IS_IMMORTAL(ch))
         {
            QUIT_STATE(ch, 10);
         }
         send_to_char("You are now considered a rebel to the empire!\n\r", criminal);
         act("$N is now considered a rebel.", ch, NULL, criminal, TO_CHAR);
         return;
      }
   }

   if (!IS_SET(criminal->act, PLR_COVENANT_ALLY) )
   {
      send_to_char("That person is not a rebel.\n\r", ch);
      return;
   }
   else
   {
      REMOVE_BIT(criminal->act, PLR_COVENANT_ALLY);
      criminal->pcdata->rebels--;
      send_to_char("You are no longer considered a rebel.\n\r", criminal);
      act("$N is no longer considered a rebel.", ch, NULL, criminal, TO_CHAR);
   }
   return;
}

void do_balance(CHAR_DATA* ch, char* argument)
{
   CHAR_DATA* banker;
   char buf[100];

   for
   (
      banker = ch->in_room->people;
      banker != NULL;
      banker = banker->next_in_room
   )
   {
      if (IS_NPC(banker) && IS_SET(banker->act, ACT_BANKER))
      {
         break;
      }
   }
   if (banker == NULL && !IS_IMMORTAL(ch))
   {
      send_to_char("You cannot do that here.\n\r", ch);
      return;
   }

   sprintf
   (
      buf,
      "You have %ld gold coins and %ld silver in your account.\n\r",
      ch->gold_bank,
      ch->silver_bank
   );
   if
   (
      ch->gold_bank == 0 &&
      ch->silver_bank == 0 &&
      !IS_IMMORTAL(ch)
   )
   {
      sprintf(buf, "You have no account here!\n\r");
   }

   if (ch->house != 0 && !IS_IMMORTAL(ch))
   {
      send_to_char(buf, ch);
      sprintf
      (
         buf,
         "Your house's account has %ld gold in it.\n\r",
         house_account[ch->house]
      );
      if (ch->house == HOUSE_EMPIRE)
      {
         send_to_char(buf, ch);
         sprintf(buf, "Taxes collected today: %d\n\r\n\r", tax_today);
      }
   }
   if (IS_IMMORTAL(ch))
   {
      int house;
      send_to_char(buf, ch);
      send_to_char("\n\r", ch);
      for (house = 0; house < MAX_HOUSE; house++)
      {
         if (!str_cmp(house_table[house].name, ""))
         {
            continue;
         }
         sprintf
         (
            buf,
            "%-12s: %12ld  gold\n\r",
            house_table[house].name,
            house_account[house]
         );
         send_to_char(buf, ch);
      }
   }
   else
   {
      send_to_char(buf, ch);
   }
   if
   (
      !IS_NPC(ch) &&
      ch->pcdata->clan != 0
   )
   {
      sprintf
      (
         buf,
         "Your clan's account has %ld gold in it.\n\r",
         clan_table[ch->pcdata->clan].gold
      );
      send_to_char(buf, ch);
   }
   return;
}

void do_withdraw(CHAR_DATA* ch, char* argument)
{
   CHAR_DATA *banker;
   char arg[MAX_STRING_LENGTH];
   int amount;
   char buf[MAX_STRING_LENGTH];
   int charges;
   int h_account;
   int c_account = 0;

   for
   (
      banker = ch->in_room->people;
      banker != NULL;
      banker = banker->next_in_room
   )
   {
      if (IS_NPC(banker) && IS_SET(banker->act, ACT_BANKER))
      {
         break;
      }
   }
   if (banker == NULL)
   {
      send_to_char("You cannot do that here.\n\r", ch);
      return;
   }

   argument = one_argument(argument, arg);
   if (arg[0] == '\0')
   {
      send_to_char("Withdraw how much of which coin type?\n\r", ch);
      return;
   }
   if (!is_number(arg))
   {
      send_to_char("Withdraw how much of which type of coin?\n\r", ch);
      return;
   }

   amount = atoi(arg);
   argument = one_argument(argument, arg);
   h_account = 0;
   h_account = house_lookup(arg);
   if (!IS_NPC(ch))
   {
      c_account = clan_lookup(arg);
   }

   if (amount > 0 && h_account != 0)
   {
      if
      (
         (
            ch->house != h_account ||
            (
               !IS_NPC(ch) &&
               ch->pcdata->induct <= INDUCT_APPRENTICE
            )
         ) &&
         !IS_IMMORTAL(ch)
      )
      {
         act("$N tells you, 'I think they'd be upset if I gave you their money.'", ch, NULL, banker, TO_CHAR);
         return;
      }

      if (house_account[h_account] < amount)
      {
         act("$N tells you, 'Sorry you do not have an account here, and we don't give loans.'", ch, NULL, banker, TO_CHAR);
         return;
      }
      house_account[h_account] -= amount;
      ch->gold += amount;
      sprintf(buf, "You withdraw %d gold coins from your house's account.\n\r", amount);
      send_to_char(buf, ch);
      if
      (
         !IS_NPC(ch) &&
         !IS_IMMORTAL(ch)
      )
      {
         ch->pcdata->deposits[h_account] -= amount;
         ch->pcdata->life_deposits[h_account] -= amount;
         save_char_obj(ch);
      }
      sprintf
      (
         log_buf,
         "%s withdrew %d gold from %s's account.",
         ch->name,
         amount,
         house_table[h_account].name
      );
      log_string(log_buf);
      save_globals();
      return;
   }

   if (amount > 0 && c_account != 0)
   {
      if (ch->pcdata->clan != c_account && !IS_IMMORTAL(ch))
      {
         act
         (
            "$N tells you, 'I think they'd be upset if I gave you their money.'",
            ch,
            NULL,
            banker,
            TO_CHAR
         );
         return;
      }
      if (clan_table[c_account].gold < amount)
      {
         act
         (
            "$N tells you, 'Sorry you do not have an account here, and we do"
            " not give loans.'",
            ch,
            NULL,
            banker,
            TO_CHAR
         );
         return;
      }
      clan_table[c_account].gold -= amount;
      ch->gold += amount;
      sprintf
      (
         buf,
         "You withdraw %d gold coins from your clan's account.\n\r",
         amount
      );
      send_to_char(buf, ch);
      if
      (
         !IS_NPC(ch) &&
         !IS_IMMORTAL(ch)
      )
      {
         ch->pcdata->clan_deposits -= amount;
         ch->pcdata->clan_life_deposits -= amount;
         save_char_obj(ch);
      }
      sprintf
      (
         log_buf,
         "%s withdrew %d gold from %s's account.",
         ch->name,
         amount,
         clan_table[c_account].who_name
      );
      log_string(log_buf);
      save_clans();
      return;
   }

   if (amount <= 0 || (str_cmp(arg, "gold") && str_cmp(arg, "silver")))
   {
      sprintf(buf, "%s Sorry, withdraw how much of which coin type?", ch->name);
      do_tell(banker, buf);
      return;
   }
   charges = 5 * amount;
   charges /= 100;

   if (!str_cmp(arg, "gold"))
   {
      if (ch->gold_bank < amount)
      {
         act("$N tells you, 'Sorry you do not have enough; we don't give loans.'", ch, NULL, banker, TO_CHAR);
         return;
      }
      ch->gold_bank -= amount;
      ch->gold += amount;
      ch->gold -= charges;
   }
   else if (!str_cmp(arg, "silver"))
   {
      if (ch->silver_bank < amount)
      {
         act("$N tells you, 'You don't have that much silver in the bank.'", ch, NULL, banker, TO_CHAR);
         return;
      }
      ch->silver_bank -= amount;
      ch->silver += amount;
      ch->silver -= charges;
   }

   sprintf(buf, "You withdraw %d %s.\n\r", amount, arg);
   send_to_char(buf, ch);
   sprintf(buf, "You are charged a small fee of %d %s.\n\r", charges, !str_cmp(arg, "gold") ? "gold" : "silver");

   send_to_char(buf, ch);
   return;
}

void do_deposit(CHAR_DATA *ch, char *argument)
{
   CHAR_DATA* banker;
   char arg[MAX_STRING_LENGTH];
   int amount;
   char buf[MAX_STRING_LENGTH];
   int h_account;
   int c_account = 0;

   for
   (
      banker = ch->in_room->people;
      banker != NULL;
      banker = banker->next_in_room
   )
   {
      if (IS_NPC(banker) && IS_SET(banker->act, ACT_BANKER))
      {
         break;
      }
   }

   if (banker == NULL)
   {
      send_to_char("You cannot do that here.\n\r", ch);
      return;
   }

   argument = one_argument(argument, arg);
   if (arg[0] == '\0')
   {
      send_to_char("Deposit how much of which coin type?\n\r", ch);
      return;
   }
   if (!is_number(arg))
   {
      send_to_char("Deposit how much of which type of coin?\n\r", ch);
      return;
   }

   amount = atoi(arg);
   argument = one_argument(argument, arg);
   h_account = house_lookup(arg);
   c_account = clan_lookup(arg);
   if (amount > 0 && h_account != 0)
   {
      if (ch->gold < amount)
      {
         act("$N tells you, 'You don't have that much gold on you!'", ch, NULL, banker, TO_CHAR);
         return;
      }
      house_account[h_account] += amount;
      ch->gold -= amount;
      save_globals();
      sprintf(buf, "You deposit %d into %s's account.\n\r", amount, house_table[h_account].name);
      send_to_char(buf, ch);
      if (ch->house == h_account){
         sprintf(buf, "Your house's balance is %ld gold.\n\r", house_account[ch->house]);
         send_to_char(buf, ch);
      }
      if
      (
         !IS_NPC(ch) &&
         !IS_IMMORTAL(ch)
      )
      {
         ch->pcdata->deposits[h_account] += amount;
         ch->pcdata->life_deposits[h_account] += amount;
         save_char_obj(ch);
      }
      if
      (
         !IS_NPC(ch) ||
         IS_AFFECTED(ch, AFF_CHARM)
      )
      {
         sprintf (log_buf, "%s deposits %d gold into %s's account.", ch->name, amount, house_table[h_account].name);
         log_string (log_buf);
      }
      return;
   }

   if (amount > 0 && c_account != 0)
   {
      if (ch->gold < amount)
      {
         act
         (
            "$N tells you, 'You don't have that much gold on you!'",
            ch,
            NULL,
            banker,
            TO_CHAR
         );
         return;
      }
      clan_table[c_account].gold += amount;
      ch->gold -= amount;
      save_clans();
      sprintf
      (
         buf,
         "You deposit %d into %s's account.\n\r",
         amount,
         clan_table[c_account].name
      );
      send_to_char(buf, ch);
      if
      (
         !IS_NPC(ch) &&
         ch->pcdata->clan == c_account
      )
      {
         sprintf
         (
            buf,
            "Your clan's balance is %ld gold.\n\r",
            clan_table[ch->pcdata->clan].gold
         );
         send_to_char(buf, ch);
      }
      if
      (
         !IS_NPC(ch) &&
         amount >= 1000 &&
         !IS_IMMORTAL(ch) &&
         ch->pcdata->clan != c_account
      )
      {
         sprintf
         (
            log_buf,
            "%s deposited %d gold coins into your clan's account.\n\r",
            (
               (
                  (
                     ch->house == HOUSE_ANCIENT ||
                     IS_SET(ch->act2, PLR_IS_ANCIENT) ||
                     IS_SET(ch->act2, PLR_IS_ANCIENT_KNOWN)
                  ) &&
                  ch->pcdata->moniker &&
                  ch->pcdata->moniker[0] != '\0'
               ) ?
               ch->pcdata->moniker :
               ch->name
            ),
            amount
         );
         make_note
         (
            banker->short_descr,
            "Clan account update",
            clan_table[c_account].who_name,
            log_buf,
            NOTE_BANK
         );
      }
      else if
      (
         !IS_NPC(ch) &&
         !IS_IMMORTAL(ch) &&
         ch->pcdata->clan == c_account
      )
      {
         ch->pcdata->clan_deposits += amount;
         ch->pcdata->clan_life_deposits += amount;
         save_char_obj(ch);
      }
      if
      (
         !IS_NPC(ch) ||
         IS_AFFECTED(ch, AFF_CHARM)
      )
      {
         sprintf
         (
            log_buf,
            "%s deposits %d gold into %s's account.",
            ch->name,
            amount,
            clan_table[c_account].who_name
         );
         log_string(log_buf);
      }
      return;
   }

   if (amount <= 0 || (str_cmp(arg, "gold") && str_cmp(arg, "silver")))
   {
      act("$N tells you, 'Sorry, deposit how much of which coin type?'", ch, NULL, banker, TO_CHAR);
      return;
   }

   if (!str_cmp(arg, "gold"))
   {
      if (ch->gold < amount)
      {
         act("$N tells you, 'You don't have that much gold on you!'", ch, NULL, banker, TO_CHAR);
         return;
      }
      ch->gold_bank += amount;
      ch->gold -= amount;
   }
   else if (!str_cmp(arg, "silver"))
   {
      if (ch->silver < amount)
      {
         act("$N tells you, 'You don't have that much silver on you!'", ch, NULL, banker, TO_CHAR);
         return;
      }
      ch->silver_bank += amount;
      ch->silver -= amount;
   }

   sprintf(buf, "You deposit %d %s.\n\r", amount, arg);
   send_to_char(buf, ch);
   sprintf(buf, "Your new balance is %ld gold, %ld silver.\n\r", ch->gold_bank, ch->silver_bank);
   send_to_char(buf, ch);
   return;
}

void do_shapeshift(CHAR_DATA* ch, char* argument)
{
   do_morph_dragon(ch, argument);
   return;
}

/*
   Some shapeshifter code
   do a if (can_shapeshift(ch, form_num)) check.
   If not then return and don't revert. Otherwise shapeshift_revert()
   and then apply new morph num to the player in the do_morph_dragon fun.
*/

bool can_shapeshift(CHAR_DATA* ch, int form_num, CHAR_DATA* morphtarget)
{
   /* form_num == shape to take on */
   bool able = TRUE;

   if (IS_NPC(ch))
   {
      send_to_char("You are a mob and cannot shapeshift.\n\r", ch);
      return FALSE;
   }

   if (ch->morph_form[0] == MORPH_ARM
   && hands_full(ch))
   {
      send_to_char("You can't change form while using your shapeshifted hand to carry an item.\n\r", ch);
      return FALSE;
   }

   if
   (
      ch->race == grn_draconian &&
      (
         form_num < 1 ||
         form_num > 5
      )
   )
   {
      send_to_char("You are not of the changeling race and cannot shapeshift.\n\r", ch);
      return FALSE;
   }
   if
   (
      ch->race != grn_changeling &&
      ch->race != grn_draconian &&
      ch->race != grn_centaur
   )
   {
      send_to_char("You are not of the changeling race and cannot shapeshift.\n\r", ch);
      return FALSE;
   }
   if
   (
      form_num == -1 ||
      (
         ch->race == grn_changeling &&
         form_num >= 1 &&
         form_num <= 5
      )
   )
   {
      send_to_char("Which form was that?\n\r", ch);
      return FALSE;
   }

   if (ch->morph_form[0] == MORPH_CLOAK)
   {
      send_to_char("You can't shapeshift while cloaked.\n\r", ch);
      return FALSE;
   }
   if (is_affected(ch, gsn_cloak_form))
   if (form_num == 11 || form_num == MORPH_MIMIC)
   {
      send_to_char("You cannot assume that forms while cloaked.\n\r", ch);
      return FALSE;
   }

   if ((form_num == MORPH_BAT && ch->level < 10)
   ||  (form_num == MORPH_WOLF && ch->level < 15)
   ||  (form_num == MORPH_BEAR && ch->level < 30)
   ||  ((morphtarget != NULL) && ((IS_NPC(morphtarget) && (ch->level < 20))
   || (!IS_NPC(morphtarget) && (ch->level < 35)))))
   {
      send_to_char("You are not yet high enough to take that form.\n\r", ch);
      return FALSE;
   }

   if (ch->mana < 50)
   {
      send_to_char("You don't have the mana.\n\r", ch);
      return FALSE;
   }

   if (form_num == MORPH_BAT)
   {
      if (get_eq_char(ch, WEAR_ARMS) != NULL
      || get_eq_char(ch, WEAR_LEGS) != NULL
      || get_eq_char(ch, WEAR_HEAD) != NULL
      || get_eq_char(ch, WEAR_BODY) != NULL
      || get_eq_char(ch, WEAR_WAIST) != NULL)
      able = FALSE;
   }
   else if (form_num == MORPH_WOLF)
   {
      if (get_eq_char(ch, WEAR_ARMS) != NULL
      || get_eq_char(ch, WEAR_BODY) != NULL)
      able = FALSE;
   }
   else if (form_num == MORPH_BEAR)
   {
      if (get_eq_char(ch, WEAR_BODY) != NULL)
      able = FALSE;
   }
   else if (form_num == MORPH_ARM)
   {
      if (get_eq_char(ch, WEAR_BODY) != NULL)
      able = FALSE;
   }

   if (able)
   return TRUE;

   send_to_char("You are wearing items which disrupt the flow of shapeshifting to that shape.\n\r", ch);
   return FALSE;
}


bool shapeshift_revert(CHAR_DATA *ch)
{
   if (ch->morph_form[0] == 0)
   return FALSE;

   if (!is_affected(ch, gsn_shapeshift))
   return FALSE;

   if (ch->morph_form[0] == MORPH_ARM
   && hands_full(ch))
   {
      send_to_char("You can't change form while your third arm is in use.\n\r", ch);
      return TRUE;
   }

   if (ch->morph_form[0] != MORPH_MIMIC)
   {
      /* Switch to multiple descs */
      switch_desc(ch, 0);
   }

   if (ch->morph_form[0] == MORPH_BAT)
   {
      send_to_char("Your wings fold away and you slowly reform yourself.\n\r", ch);
      act("$n's wings fold away and a new form takes its place.", ch, NULL, NULL, TO_ROOM);
   }
   else if (ch->morph_form[0] == MORPH_WOLF)
   {
      send_to_char("Your wolven coat shrinks away and you slowly reform yourself.\n\r", ch);
      act("$n's fur shrinks away to nothing as it slowly stands up and takes on a new form.", ch, NULL, NULL, TO_ROOM);
   }
   else if (ch->morph_form[0] == MORPH_BEAR)
   {
      send_to_char("The thick fur about your body recedes as you assume a new form.\n\r", ch);
      act("$n's thick brown hair recedes as it fluidly changes into a new form.", ch, NULL, NULL, TO_ROOM);
   }
   else if (ch->morph_form[0] == MORPH_ARM)
   {
      send_to_char("You fluidly retract your shapeshifted arm.\n\r", ch);
      act("$n's shapeshifted arm fluidly flows back into $s body.", ch, NULL, NULL, TO_ROOM);
   }
   else if (ch->morph_form[0] == MORPH_CLOAK)
   {
      send_to_char("You stop cloaking yourself.\n\r", ch);
      affect_strip(ch, gsn_cloak_form);
      ch->morph_form[0] = 0;
   }
   else if (ch->morph_form[0] == MORPH_MIMIC)
   {
      char buf[MAX_STRING_LENGTH];
      send_to_char("You fluidly alter your structure to its more familiar form.\n\r", ch);
      sprintf(buf, "$n fluidly alters $s body and no longer looks like %s.", ch->mprog_target->name);
      act(buf, ch, NULL, NULL, TO_NOTVICT);
      act("$n fluidly alters $s body and no longer looks like you.", ch, NULL, ch->mprog_target, TO_VICT);
      ch->mprog_target = NULL;
      ch->morph_form[1] = 0;
      ch->morph_form[2] = 0;
   }

   affect_strip(ch, gsn_shapeshift);
   return TRUE;
}

void do_records(CHAR_DATA *ch, char *argument)
{
   char buf[MAX_STRING_LENGTH];
   char buf1[MAX_STRING_LENGTH];
   CHAR_DATA *victim;
   int count = 0;
   bool self_only;
   bool imm = IS_IMMORTAL(ch);

   if
   (
      ch &&
      (ch->house != HOUSE_ENFORCER) &&
      !IS_IMMORTAL(ch)
   )
   {
      self_only = TRUE;
   }
   else
   {
      self_only = FALSE;
   }

   buf1[0] = '\0';
   if (self_only)
   {
      sprintf(buf, "\n\rYour Criminal Record:\n\r");
   }
   else
   {
      sprintf(buf, "\n\rThe Criminal records of Thera's citizens:\n\r");
   }

   for (victim = char_list; victim != NULL; victim = victim->next)
   {
      if
      (
         IS_NPC(victim) ||
         (
            ch != victim &&
            (
               self_only ||
               (
                  !imm &&
                  is_affected(victim, gsn_submerge)
               )
            )
         ) ||
         (
            !imm &&
            is_affected(victim, gsn_stalking) &&
            number_percent() < get_skill(victim, gsn_stalking) - 10
         ) ||
         IS_IMMORTAL(victim) ||
         (
            !imm &&
            is_affected(victim, gsn_cloak_form)
         ) ||
         !can_see(ch, victim)
      )
      {
         continue;
      }
      if (IS_SET(victim->act2, PLR_LAWLESS))
      {
         count++;
         sprintf
         (
            buf1,
            "%s has lost the protection of the law.\n\r",
            victim->name
         );
         strcat(buf, buf1);
      }
      else if (victim->pcdata->wanteds > 0)
      {
         count++;
         sprintf
         (
            buf1,
            "%s has %d flags.\n\r",
            victim->name,
            victim->pcdata->wanteds
         );
         strcat(buf, buf1);
      }
   }
   if (count == 0)
   {
      if (self_only)
      {
         strcat(buf, "You do not have a criminal record.\n\r");
      }
      else
      {
         strcat(buf, "None in the realms right now have had a flag before.\n\r");
      }
   }

   send_to_char(buf, ch);
   return;
}


bool is_pulsing(OBJ_DATA* obj)
{
   AFFECT_DATA* paf;
   sh_int natural = 0;
   sh_int enchanted = 0;
   sh_int nach2 = 0;
   sh_int ench2 = 0;

   if
   (
      IS_SET(obj->extra_flags2, ITEM_PURIFIED) &&
      !obj->enchanted
   )
   {
      return FALSE; /*Jord*/
   }
   if
   (
      (
         obj->item_type == ITEM_PILL ||
         obj->item_type == ITEM_SCROLL ||
         obj->item_type == ITEM_STAFF ||
         obj->item_type == ITEM_POTION ||
         obj->item_type == ITEM_WAND
      ) ||
      (
         IS_SET(obj->extra_flags, ITEM_MAGIC) &&
         (
            obj->pIndexData->vnum == OBJ_VNUM_BERRY ||
            obj->pIndexData->vnum == OBJ_VNUM_RANGER_STAFF ||
            obj->pIndexData->vnum == OBJ_VNUM_SPRING
         )
      ) ||
      (
         obj->pIndexData->vnum == OBJ_VNUM_MUSHROOM &&
         is_name("magic", obj->name)
      ) ||
      is_affected_obj(obj, gsn_alchemy) ||
      (
         obj->item_type == ITEM_WEAPON &&
         (
            IS_WEAPON_STAT(obj, WEAPON_VAMPIRIC) ||
            obj->pIndexData->vnum == OBJ_VNUM_TALON ||
            obj->pIndexData->vnum == OBJ_VNUM_SWORD_DEMONS ||
            obj->pIndexData->vnum == OBJ_VNUM_GENOCIDE_AXE ||
            obj->pIndexData->vnum == 27035 ||
            obj->pIndexData->vnum == 27036 ||
            obj->pIndexData->vnum == 27037 ||
            obj->pIndexData->vnum == 27038 ||
            obj->pIndexData->vnum == 27039 ||
            obj->pIndexData->vnum == 27040 ||
            obj->pIndexData->vnum == 27041 ||
            obj->pIndexData->vnum == 27042 ||
            obj->pIndexData->vnum == 8003 ||
            obj->pIndexData->vnum == OBJ_VNUM_FLAME_TOWER ||
            obj->pIndexData->vnum == 27505 ||
            obj->pIndexData->vnum == OBJ_VNUM_MACE_DISRUPTION ||
            obj->pIndexData->vnum == OBJ_VNUM_SWORD_PLANES ||
            obj->pIndexData->vnum == 27503 ||
            obj->pIndexData->vnum == 27504 ||
            obj->pIndexData->vnum == 27307 ||
            obj->pIndexData->vnum == 27751 ||
            (
               (
                  obj->value[3] >= 0 &&
                  obj->value[3] < MAX_DAMAGE_MESSAGE
               ) &&
               (
                  attack_table[obj->value[3]].damage == DAM_ENERGY ||
                  attack_table[obj->value[3]].damage == DAM_NEGATIVE ||
                  attack_table[obj->value[3]].damage == DAM_LIGHT
               )
            )
         )
      ) ||
      (
         obj->item_type == ITEM_ARMOR &&
         (
            obj->pIndexData->vnum == 24418 ||
            obj->pIndexData->vnum == OBJ_VNUM_CONCAT_RING ||
            obj->pIndexData->vnum == 19445 ||
            obj->pIndexData->vnum == OBJ_VNUM_PHYSICAL_FOCUS ||
            obj->pIndexData->vnum == OBJ_VNUM_MENTAL_FOCUS ||
            obj->pIndexData->vnum == OBJ_VNUM_CROWN_BLOOD ||
            obj->pIndexData->vnum == OBJ_VNUM_HAIR_BRACELET ||
            obj->pIndexData->vnum == OBJ_VNUM_BLOOD_BRACELET ||
            obj->pIndexData->vnum == OBJ_VNUM_CLOAK_UNDERWORLD ||
            obj->pIndexData->vnum == 23706 ||
            obj->pIndexData->vnum == 14005 ||
            obj->pIndexData->vnum == OBJ_VNUM_RING_PROTECT ||
            obj->pIndexData->vnum == 14005 ||
            obj->pIndexData->vnum == OBJ_VNUM_SCEPTRE_DEAD ||
            obj->pIndexData->vnum == OBJ_VNUM_SYMBOL_MAGIC ||
            obj->pIndexData->vnum == OBJ_VNUM_WIZARDRY_1 ||
            obj->pIndexData->vnum == OBJ_VNUM_WIZARDRY_2 ||
            obj->pIndexData->vnum == OBJ_VNUM_TIARA ||
            obj->pIndexData->vnum == 27513 ||
            obj->pIndexData->vnum == OBJ_VNUM_SHADOW_HIDE ||
            obj->pIndexData->vnum == 6983 ||
            obj->pIndexData->vnum == 6984 ||
            obj->pIndexData->vnum == 6985 ||
            obj->pIndexData->vnum == 6986 ||
            obj->pIndexData->vnum == OBJ_VNUM_MEDUSA_WIG ||
            obj->pIndexData->vnum == OBJ_VNUM_HEAVENLY_SCEPTRE ||
            obj->pIndexData->vnum == OBJ_VNUM_SUMMONING_STONE ||
            obj->pIndexData->vnum == 27757 ||
            obj->pIndexData->vnum == 23728
         )
      ) ||
      obj->pIndexData->vnum == OBJ_VNUM_ROD_LORDLY ||
      obj->pIndexData->vnum == OBJ_VNUM_WINGS ||
      obj->pIndexData->vnum == OBJ_VNUM_MISTY_ESSENCE ||
      obj->pIndexData->vnum == OBJ_VNUM_ENDLESS_QUIVER ||
      obj->pIndexData->vnum == OBJ_VNUM_DOGGIE_TOY ||
      obj->pIndexData->vnum == OBJ_VNUM_TORQUE ||
      obj->pIndexData->vnum == OBJ_VNUM_PHANTOM
   )
   {
      return TRUE;
   }

   for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
   {
      if
      (
         (
            (
               paf->bitvector &&
               paf->where == TO_AFFECTS
            ) ||
            paf->location == APPLY_SEX ||
            paf->location == APPLY_SPELL_POWER ||
            paf->location == APPLY_MN_REGENERATION
         ) &&
         (
            paf->bitvector != AFF_HASTE ||
            obj->pIndexData->vnum != 18792
         )
      )
      {
         return TRUE;
      }
   }
   for (paf = obj->affected; paf != NULL; paf = paf->next)
   {
      if
      (
         (
            (
               paf->bitvector &&
               paf->where == TO_AFFECTS
            ) ||
            paf->location == APPLY_SEX ||
            paf->location == APPLY_SPELL_POWER
         ) &&
         (
            paf->bitvector != AFF_HASTE ||
            obj->pIndexData->vnum != 18792
         )
      )
      {
         return TRUE;
      }
   }

   if (!obj->enchanted)
   {
      return FALSE;
   }

   if (obj->enchanted)
   {
      if (obj->item_type == ITEM_ARMOR)
      {
         for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
         {
            if (paf->location != APPLY_AC)
            {
               continue;
            }
            natural += paf->modifier;
         }
         for (paf = obj->affected; paf != NULL; paf = paf->next)
         {
            if (paf->location != APPLY_AC)
            {
               continue;
            }
            enchanted += paf->modifier;
         }

         if (enchanted < natural)
         {
            return TRUE;
         }
      }
      else if (obj->item_type == ITEM_WEAPON)
      {

         for (paf = obj->pIndexData->affected; paf != NULL; paf=paf->next)
         {
            if (paf->location == APPLY_HITROLL)
            {
               natural += paf->modifier;
            }
            else if (paf->location == APPLY_DAMROLL)
            {
               nach2 += paf->modifier;
            }
         }
         for (paf = obj->affected; paf != NULL; paf = paf->next)
         {
            if (paf->location != APPLY_HITROLL)
            {
               enchanted += paf->modifier;
            }
            else if (paf->location == APPLY_DAMROLL)
            {
               ench2 += paf->modifier;
            }
         }
         if
         (
            enchanted > natural ||
            ench2 > nach2
         )
         {
            return TRUE;
         }
      }
   }

   return FALSE;
}

void do_halo_of_the_sun(CHAR_DATA *ch, char *argument)
{
   AFFECT_DATA af;

   if ((get_skill(ch, gsn_halo_of_the_sun)) <= 0
   ||  !has_skill(ch, gsn_halo_of_the_sun))
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }

   if (house_down(ch, HOUSE_LIFE))
   return;

   if (ch->mana < 50)
   {
      send_to_char("You do not have the mana.\n\r", ch);
      return;
   }

   if ( is_affected(ch, gsn_halo_of_the_sun) )
   {
      send_to_char("You already have a halo.\n\r", ch);
      return;
   }

   if (number_percent() > get_skill(ch, gsn_halo_of_the_sun))
   {
      send_to_char("You fail to call upon the power of light.\n\r", ch);
      ch->mana -= 25;
      check_improve(ch, gsn_halo_of_the_sun, FALSE, 1);
      return;
   }

   af.where = TO_AFFECTS;
   af.location = 0;
   af.level = ch->level;
   af.duration = 24;
   af.type = gsn_halo_of_the_sun;
   af.modifier = 0;
   af.bitvector = 0;
   affect_to_char(ch, &af);


   send_to_char("You smile as a ray of light forms into a halo atop your head.\n\r", ch);
   act("$n smiles as a ray of light forms into a halo atop $s head.", ch, NULL, NULL, TO_ROOM);

   ch->mana -= 50;
   check_improve(ch, gsn_halo_of_the_sun, TRUE, 1);

   return;
}

void do_target( CHAR_DATA *ch, char *argument )
{
   AFFECT_DATA af;
   CHAR_DATA *victim;
   char arg[MAX_INPUT_LENGTH];

   if (house_down(ch, HOUSE_ANCIENT))
   return;

   one_argument( argument, arg );
   if ( arg[0] == '\0' )
   {
      send_to_char( "Target whom?\n\r", ch );
      return;
   }

   if ( ( victim = get_char_world( ch, arg ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
      return;
   }

   if ( ch == victim )
   {
      send_to_char( "Why don't you just employ the use of a high tree limb and some rope?\n\r", ch );
      return;
   }

   if ( victim->house == HOUSE_ANCIENT )
   {
      send_to_char( "Better if you do not.\n\r", ch );
      return;
   }

   if (!IS_SET(victim->act2, PLR_ANCIENT_MARK) &&
   !IS_SET(victim->act2, PLR_ANCIENT_MARK_PERM))
   {
      send_to_char( "You cannot target an unmarked opponent.\n\r", ch );
      return;
   }

   if (is_affected(ch, gsn_target))
   {
      affect_strip(ch, gsn_target);
   }

   if (ch->mana < 75)
   {
      send_to_char("You don't have enough mana.\n\r", ch);
      return;
   }

   if (!is_affected(ch, gsn_cloak_form) && !IS_SET(ch->act2, PLR_IS_ANCIENT_KNOWN)){
      send_to_char("You can't accept the hunt if your uncloaked.\n\r", ch);
      return;
   }

   if (number_percent() < get_skill(ch, gsn_target))
   {
      if (oblivion_blink(ch, victim))
      {
         return;
      }
      ch->mana -= 75;
      af.where = TO_AFFECTS;
      af.level = ch->level;
      af.duration = -1;
      af.bitvector = 0;
      af.type = gsn_target;
      af.modifier = UMAX(ch->level/10, 4);
      af.location  = APPLY_HITROLL;
      affect_to_char(ch, &af);
      ch->pcdata->target = victim;
      send_to_char("You accept the mark and begin the hunt!\n\r", ch);
      check_improve(ch, gsn_target, TRUE, 1);
      return;
   }
   else
   {
      ch->mana -= 38;
      send_to_char("Your concentration fails you, and the mark remains untargetted.\n\r", ch);
      check_improve(ch, gsn_target, FALSE, 1);
      return;
   }

   return;
}

bool check_peace(CHAR_DATA *ch)
{
   bool result = is_affected(ch, gsn_peace);

   if (result == TRUE)
   {
      send_to_char("You feel too at peace with the world to do that.\n\r", ch);
   }

   return result;
}

bool collect_tax(CHAR_DATA *ch, int cost){
   int tax;
   char buf[MAX_STRING_LENGTH];

   return FALSE;

   if (!is_occupied(ch->in_room->vnum))
   return TRUE;
   if (ch->house == HOUSE_EMPIRE || ch->house == HOUSE_ENFORCER)
   return TRUE;
   tax = cost * 15/100;
   if ((ch->silver + ch->gold * 100) <= tax){
      send_to_char("You can't afford the tax.\n\r", ch);
      send_to_char("The Empire generously pardons this trespess.\n\r", ch);
      return FALSE;
   }
   sprintf(buf, "You pay taxes to Empire of %d silver!\n\r", tax);
   send_to_char(buf, ch);
   taxes_collected += tax;
   while (tax > 0){
      if (ch->silver > tax){
         ch->silver -= tax;
         tax = 0;
         continue;
      }
      ch->gold -= 1;
      ch->silver += 100;
   }
   tax = 0; /* just in case */
   while (taxes_collected > 100){
      taxes_collected -= 100;
      tax += 1;
   }
   if (tax > 0){
      tax_today += tax;
      house_account[HOUSE_EMPIRE] += tax;
      save_globals();
   }

   return TRUE;
}

void mark_list(CHAR_DATA *ch)
{
   DESCRIPTOR_DATA *d;
   CHAR_DATA * list;
   char buf[MAX_STRING_LENGTH];
   int days;
   int count = 0;

   send_to_char("\n\r", ch);
   buf[0] = '\0';

   send_to_char("Name              Price    Bounty by         Days Marked Worth  Accepted\n\r", ch);

   send_to_char("------------------------------------------------------------------------\n\r", ch);
   for (d = descriptor_list; d != NULL; d = d->next)
   {
      list = d->character;
      if (list == NULL || d->connected != CON_PLAYING || !can_see(ch, list))
      continue;

      if (is_affected(list, gsn_submerge)
      && list != ch && !IS_IMMORTAL(ch)) continue;
      if (is_affected(list, gsn_stalking) && !IS_IMMORTAL(ch) && ch->house != list->house){
         if (number_percent()<(get_skill(list, gsn_stalking)-10))
         continue;
      }

      if (is_affected(list, gsn_cloak_form) && !IS_TRUSTED(ch, ANGEL))
      continue;

      if (IS_IMMORTAL(list) || IS_NPC(list))
      continue;
      if   (!IS_SET(list->act2, PLR_ANCIENT_MARK) &&
      !IS_SET(list->act2, PLR_ANCIENT_MARK_PERM) &&
      (list->pcdata->mark_price <= 0))
      continue;

      if (!IS_IMMORTAL(ch) && ch->house != HOUSE_ANCIENT &&!IS_SET(ch->act2, PLR_IS_ANCIENT))
      if (str_cmp(list->pcdata->marker, ch->name))
      continue;

      days = (int) ((current_time-list->pcdata->mark_time)/(3600*24));

      if (IS_IMMORTAL(ch) && !str_cmp(list->pcdata->marker, "cloaked figure")
      && list->pcdata->orig_marker != str_empty ){
         sprintf(buf, "%-17s %-8ld CF:%-15s %-11d %-2d %-20s\n\r", get_longname(list, ch),
         list->pcdata->mark_price, list->pcdata->orig_marker, days,
         award_standing(list), list->pcdata->mark_accepted ? "Accepted":"No");
      }
      else
      {
         sprintf(buf, "%s%-12s%s %-8ld %-18s %-11d %-2d %-20s\n\r",
         is_in_pk(ch, list)?"(PK) ":"", get_longname(list, ch),
         is_in_pk(ch, list)?"":"     ", /* to correct spacing */
         list->pcdata->mark_price, list->pcdata->marker, days,
         award_standing(list), list->pcdata->mark_accepted ? "Accepted":"No");
      }
      count++;
      send_to_char(buf, ch);

   }

   if (count == 0)
   send_to_char("No marks to list.\n\r", ch);

   return;
}

void collect_mark_refund(CHAR_DATA *ch, CHAR_DATA *criminal){
   int days;
   int refund = criminal->pcdata->mark_price;

   days = (int) ((current_time-criminal->pcdata->mark_time)/(3600*24));

   REMOVE_BIT(criminal->act2, PLR_ANCIENT_MARK);
   if (criminal->pcdata->marker != str_empty){
      free_string(criminal->pcdata->marker);
      criminal->pcdata->marker = &str_empty[0];
   }
   criminal->pcdata->mark_price = 0;
   criminal->pcdata->mark_time = 0;
   criminal->pcdata->mark_accepted = FALSE;
   send_to_char("They are no longer being hunted.\n\r", ch);

   if (days < 11)
   refund = (float) refund * (float) (days-1)/10;
   if (days < 1) refund = 0;

   if (refund < 0)
   refund = 0;
   if (refund != 0){
      sprintf(log_buf, "You collect a refund of %d gold.\n\r", refund);
      send_to_char(log_buf, ch);
   }else
   send_to_char("However you cannot receive a refund on the first day.\n\r", ch);
   ch->gold += refund;

   return;
}

char * get_ancient_title( CHAR_DATA *ch )
{
   int kills = 0;

   if ( IS_NPC(ch) ) return "";
   if ( ch->house != HOUSE_ANCIENT ) return "";

   kills = ch->pcdata->targetkills;
   if (ch->pcdata->induct == INDUCT_APPRENTICE)
   {
      kills = 0;
   }
   if (ch->id == ID_BUK)
   {
      return "Buk";
   }
   if ( IS_SET(ch->act2, PLR_IS_ANCIENT_KNOWN) && is_affected(ch, gsn_cloak_form) )
   {
      return ch->name;
   }

   if ( ch->pcdata->induct == 5 )
   return "Lord Ancient";
   if (kills <= 3)
   return "Novice";
   if (kills <= 6)
   return "Apprentice";
   if (kills <= 10)
   return "Beginner";
   if (kills <= 14)
   return "Amateur";
   if (kills <= 18)
   return "Soldier";
   if (kills <= 21)
   return "Mercenary";
   if (kills <= 24)
   return "Intermediate";
   if (kills <= 29)
   return "Veteran";
   if (kills <= 34 )
   return "Adept";
   if (kills <= 39 )
   return "Murderer";
   if (kills <= 44)
   return "Killer";
   if (kills <= 49)
   return "Slayer";
   if (kills <= 59)
   return "Executioner";
   if (kills <= 69)
   return "Professional";
   if (kills <= 79)
   return "Expert";
   if (kills <= 89)
   return "Elite";
   if (kills <= 99)
   return "Master";
   if (kills <= 114)
   return "High Master";
   if (kills <= 129)
   return "Assassin";
   if (kills <= 144)
   return "Expert Assassin";
   if (kills <= 159)
   return "Elite Assassin";
   if (kills <= 174)
   return "Master Assassin";
   if (kills <= 199)
   return "High Master Assassin";

   return "Supreme Master Assassin";
}

void list_ancients(CHAR_DATA *ch){
   DESCRIPTOR_DATA *d;
   CHAR_DATA * list;
   char buf[MAX_STRING_LENGTH];
   int count = 0;

   send_to_char("\n\r", ch);
   buf[0] = '\0';

   for (d = descriptor_list; d != NULL; d = d->next)
   {
      list = d->character;
      if (list == NULL || d->connected != CON_PLAYING || !can_see(ch, list))
      continue;


      if (IS_NPC(list)) continue;

      if (list->house != HOUSE_ANCIENT)
      continue;

      if (is_affected(list, gsn_cloak_form))
      {
         sprintf
         (
            buf,
            "%s%s, %s (Standing %ld)\n\r",
            (
               is_in_pk(list, ch)?
               "(PK) " :
               ""
            ),
            (
               list->pcdata->moniker[0] == '\0' ?
               "Cloaked Figure" :
               list->pcdata->moniker
            ),
            get_ancient_title(list),
            list->pcdata->targetkills
         );
      }
      else if
      (
         IS_IMMORTAL(list) ||
         IS_SET(list->act2, PLR_IS_ANCIENT_KNOWN)
      )
      {
         sprintf
         (
            buf,
            "%s%s, %s (Standing %ld)\n\r",
            (
               is_in_pk(list, ch) ?
               "(PK) " :
               ""
            ),
            list->name,
            get_ancient_title(list),
            list->pcdata->targetkills
         );
      }
      else
      {
         continue;
      }

      count++;
      send_to_char(buf, ch);

   }

   if (count == 0)
   send_to_char("There are no other ancients.\n\r", ch);

   return;
}

void do_blind_fighting(CHAR_DATA *ch, char *argument)
{
   AFFECT_DATA af;
   int number;

   if ( is_affected(ch, gsn_blind_fighting) )
   {
      send_to_char("You are already as alert as you can be. \n\r", ch);
      return;
   }

   if ( (number = get_skill(ch, gsn_blind_fighting)) <= 0 ||
   !has_skill(ch, gsn_blind_fighting))
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }
   if (ch->pcdata->learned[gsn_corrupt("subrank", &gsn_subrank)] < 9)
   {
      send_to_char("You are too inexperienced to use this technique.\n\r", ch);
      return;
   }
   if ( (number = number_percent()) > (get_skill(ch, gsn_blind_fighting)-10) )
   {
      send_to_char("You close your eyes in meditation, but fail to attain awareness.\n\r", ch);
      check_improve(ch, gsn_blind_fighting, FALSE, 2);
      return;
   }

   af.where     = TO_AFFECTS;
   af.type      = gsn_blind_fighting;
   af.level     = ch->level;
   af.duration  = ch->level;
   af.location  = APPLY_NONE;
   af.modifier  = 0;
   af.bitvector = 0;
   affect_to_char( ch, &af );
   send_to_char( "You close your eyes and awareness washes over you.\n\r", ch );
   check_improve( ch, gsn_blind_fighting, TRUE, 2 );
   return;
}

void do_morale_report(CHAR_DATA* ch, char* argument)
{
   char buf[MAX_STRING_LENGTH];
   int cnt;
   int house;

   if (!str_cmp(argument, "reset"))
   {
      for (cnt = 0; cnt < MAX_HOUSE; cnt++)
      {
         morale_reset_time = current_time;
         house_downs[cnt] = 0;
         house_kills[cnt] = 0;
         house_pks[cnt] = 0;
         unhoused_kills[cnt] = 0;
         unhoused_pk[cnt] = 0;
         unique_kills[cnt] = 0;
         raids_defended[cnt] = 0;
         raids_nodefender[cnt] = 0;
      }
      send_to_char("All morale reports reset.\n\r", ch);
      return;
   }
   for (house = 0; house < MAX_HOUSE; house++)
   {
      if
      (
         house_table[house].name == NULL ||
         !str_cmp(house_table[house].name, "")
      )
      {
         continue;
      }
      if
      (
         house_downs[house] == 0 &&
         house_kills[house] == 0 &&
         house_pks[house] == 0 &&
         unique_kills[house] == 0 &&
         raids_defended[house] == 0 &&
         raids_nodefender[house] == 0
      )
      {
         continue;
      }
      sprintf
      (
         buf,
         "\n\r%s\n\r----------\n\r"
         "Kills:  %ld total  %ld on unhoused  %ld non-recently killed\n\r"
         "Times killed: %ld (%ld by unhoused)\n\r"
         "Item taken %ld times defended, %ld times no defender. (%ld ticks down)\n\r",
         house_table[house].name,
         house_pks[house],
         unhoused_pk[house],
         unique_kills[house],
         house_kills[house],
         unhoused_kills[house],
         raids_defended[house],
         raids_nodefender[house],
         house_downs[house]
      );
      send_to_char(buf, ch);
   }
   return;
}

char * get_brand_title(CHAR_DATA *ch)
{
   OBJ_DATA *brand;
   int rank;

   if (IS_NPC(ch)) return NULL;

   rank = ch->pcdata->brand_rank;
   brand = get_eq_char(ch, WEAR_BRAND);
   if (brand == NULL) return NULL;

   if (brand->pIndexData->vnum == 12500)
   {
      if (rank == 0) return "Follower of the Owl";
      if (rank == 1) return "Chosen of the Owl";
      if (rank == 2) return "Priest of Order";
      if (rank == 3) return "Priest of Wisdom";
      if (rank == 4) return "Priest of Society";
      if (rank == 5) return "High Priest of the Owl";
      if (IS_IMMORTAL(ch)) return "Wervdon, Lord of the Owl";
      return "Follower of the Owl";
   }
   return NULL;
}

char* get_house_title2(CHAR_DATA *ch)
{
   /* Covenant Branch branching -- Drinlinda */
   if (ch->house == HOUSE_COVENANT)
   {
      switch(ch->class)
      {
         default:
         {
            return "Covenant Soldier";
         }
         case (CLASS_MONK):
         case (CLASS_THIEF):
         case (CLASS_NIGHTWALKER):
         {
            return "Covenant Covert Operations";
         }
         case (CLASS_RANGER):
         {
            return "Covenant Pathfinder Corps";
         }
         case (CLASS_CLERIC):
         {
            return "Covenant Chaplain Corps";
         }
         case (CLASS_WARRIOR):
         case (CLASS_ANTI_PALADIN):
         {
            return "Covenant Infantry";
         }
      }
   }
   return NULL;
}

char* get_house_title(CHAR_DATA *ch)
{
   int rank = 0;

   if ( ch->house == 0 ) return NULL;

   if ( IS_NPC(ch) ) return NULL;

   rank = ch->pcdata->house_rank;

   if ( ch->house == HOUSE_COVENANT)
   {
      if ( IS_IMMORTAL(ch) ) return "Dark Covenant Supreme Commander";
      switch(rank)
      {
         default:
         return "Dark Covenant Knight";
         break;
         case RANK_SQUIRE:
         return "Dark Covenant Squire";
         break;
         case RANK_FOOTMAN:
         return "Dark Covenant Footman";
         break;
         case RANK_MASTERFOOTMAN:
         return "Dark Covenant Master Footman";
         break;
         case RANK_CORPORAL:
         return "Dark Covenant Corporal";
         break;
         case RANK_SPECIALIST:
         return "Dark Covenant Specialist";
         break;
         case RANK_MASTERSPECIALIST:
         return "Dark Covenant Master Specialist";
         break;
         case RANK_MASTERCORPORAL:
         return "Dark Covenant Master Corporal";
         break;
         case RANK_SERGEANT:
         return "Dark Covenant Sergeant";
         break;
         case RANK_MASTERSERGEANT:
         return "Dark Covenant Master Sergeant";
         break;
         case RANK_SERGEANTMAJOR:
         return "Dark Covenant Sergeant Major";
         break;
         case RANK_WARRANTOFFICER:
         return "Dark Covenant Warrant Officer";
         break;
         case RANK_MASTERWARRANTOFFICER:
         return "Dark Covenant Master Warrant Officer";
         break;
         case RANK_CHIEFWARRANTOFFICER:
         return "Dark Covenant Chief Warrant Officer";
         break;
         case RANK_SECONDLIEUTENANT:
         return "Dark Covenant Second Lieutenant";
         break;
         case RANK_LIEUTENANT:
         return "Dark Covenant Lieutenant";
         break;
         case RANK_CAPTAIN:
         return "Dark Covenant Captain";
         break;
         case RANK_MAJOR:
         return "Dark Covenant Major";
         break;
         case RANK_LIEUTENANTCOLONEL:
         return "Dark Covenant Lieutenant Colonel";
         break;
         case RANK_COLONEL:
         return "Dark Covenant Colonel";
         break;
         case RANK_BRIGADIERGENERAL:
         return "Dark Covenant Brigadier General";
         break;
         case RANK_MAJORGENERAL:
         return "Dark Covenant Major General";
         break;
         case RANK_LIEUTENANTGENERAL:
         return "Dark Covenant Lieutenant General";
         break;
         case RANK_GENERAL:
         return "Dark Covenant General";
         break;
      }
   }
   if ( ch->house == HOUSE_EMPIRE )
   {
      if ( IS_IMMORTAL(ch) ) return "Divine Lord of the Empire";
      switch(rank)
      {
         case 0: switch (ch->pcdata->house_rank2)
         {
            default:
            return "Imperial Centurion"; break;
            case 1:
            return "Centurion in the First Division"; break;
            case 2:
            return "Centurion in the Second Division"; break;
            case 3:
            return "Centurion in the Third Division"; break;
            case 4:
            return "Centurion in the Fourth Division"; break;
         }
         case 1: switch(ch->pcdata->house_rank2)
         {
            default:
            return "Tribune of the Imperial Armies"; break;
            case 1:
            return "Tribune of the First Division"; break;
            case 2:
            return "Tribune of the Second Division"; break;
            case 3:
            return "Tribune of the Third Division"; break;
            case 4:
            return "Tribune of the Fourth Division"; break;
         }
         case 2:  switch(ch->pcdata->house_rank2)
         {
            default:
            return "Legate of the Imperial Armies"; break;
            case 1:
            case 2:
            return "Legate of the First and Second Divisions"; break;
            case 3:
            case 4:
            return "Legate of the Third and Fourth Divisions"; break;
         }
         case 3: return "General of the Imperial Armies"; break;
         case 4: return "Member of the Honor Guard"; break;
         case 5: return "Captain of the Honor Guard"; break;
         case 6:
         switch (ch->temple)
         {
            case 10681:
            case 10699:
            case 10670: return "Senator of Glyndane"; break;
            case 8839:
            case 8844:
            case 8848:  return "Senator of Elvenhame"; break;
            case 9609:  return "Senator of New Thalos"; break;
            case 699:   return "Senator of Ofcol"; break;
            case 3054:  return "Senator of Ethshar"; break;
         }
         break;
      }
      return "Member of the Imperial Army";
   }

   return NULL;
}

char * get_eqslot_name( CHAR_DATA *ch, int slotnum)
{
   if (slotnum == WEAR_WIELD && check_two_hand_wield(ch))
   return "<two-handed wielded>   ";
   return wear_name[slotnum];
}

/*
   Added by Fizzfaldt
   Used to use the new reordered
   array so we can add new eq slots easily.
*/
char* get_eqslot_name_ordered(CHAR_DATA* ch, int slotnum, bool book)
{
   if
   (
      wear_name_ordered[slotnum].slot == WEAR_WIELD &&
      check_two_hand_wield(ch)
   )
   {
      if (!book)
      {
         return "<two-handed wielded>   ";
      }
      else
      {
         return "<grasped tightly>      ";
      }
   }
   if (!book)
   {
      return wear_name_ordered[slotnum].name;
   }
   else
   {
      return wear_name_ordered_buk[slotnum].name;
   }
}

/*
   used to "fake" eq from affects -Wervdon
   barbarian warpaint & possibly tattoo's later
*/
bool appended_eqlist(CHAR_DATA *ch, CHAR_DATA *victim)
{
   bool found = FALSE;
   AFFECT_DATA *af;
   int paint_numb = 0;
   char buf[MAX_STRING_LENGTH];

   if (is_affected(victim, gsn_seal_of_justice))
   {
      found = TRUE;
      send_to_char
      (
         "<worn proudly>         a Seal of Justice\n\r",
         ch
      );
   }
   if (is_affected(victim, gsn_warpaint))
   {
      found = TRUE;
      send_to_char("<painted>              ", ch);
      for (af = victim->affected; af != NULL; af = af->next)
      if (af->type == gsn_warpaint && af->where == TO_WARPAINT)
      paint_numb = af->modifier;
      switch(paint_numb)
      {
         default:
         case 1:  sprintf(buf,  "An ornate design depicting the earth"); break;
         case 2:  sprintf(buf,  "An ornate design depicting the wind"); break;
         case 3:  sprintf(buf,  "An ornate design depicting water"); break;
         case 4:  sprintf(buf,  "An ornate design depicting the sun"); break;
         case 5:  sprintf(buf,  "A fierce looking brown bear"); break;
         case 6:  sprintf(buf,  "An eagle suspended in flight"); break;
         case 7:  sprintf(buf,  "A scaly green crocodile"); break;
         case 8:  sprintf(buf,  "A fierce looking lion"); break;
         case 9:  sprintf(buf,  "A fierce falcon on the hunt"); break;
         case 10: sprintf(buf,  "A long limbed monkey clinging to a branch"); break;
         case 11: sprintf(buf,  "A large rhinoceros ready to charge"); break;
         case 12: sprintf(buf,  "A large gorilla screaming as it attacks"); break;
         case 13: sprintf(buf,  "A beautiful unicorn"); break;
         case 14: sprintf(buf,  "A swift jet black panther crouching to pounce"); break;
         case 15: sprintf(buf,  "A large disgusting rat"); break;
         case 16: sprintf(buf,  "A cobra preparing to strike"); break;
         case 17: sprintf(buf,  "A huge elephant ready to trample its foes"); break;
         case 18: sprintf(buf,  "A striped muscular tiger"); break;
         case 19: sprintf(buf,  "A large wild boar"); break;
         case 20: sprintf(buf,  "A small green parrot"); break;
         case 21: sprintf(buf,  "A long green viper with sharp fangs"); break;
         case 22: sprintf(buf,  "An elegant golden unicorn with a white horn"); break;
         case 23: sprintf(buf,  "A grey wolf"); break;
         case 24: sprintf(buf,  "A large python"); break;
      }
      send_to_char(buf, ch);
      send_to_char("\n\r", ch);
   }
   return found;
}

void do_warpaint(CHAR_DATA *ch, char * argument)
{
   int pn = 0;
   char save_name[MAX_STRING_LENGTH];
   AFFECT_DATA af;

   if (get_skill(ch, gsn_warpaint) <= 0 || !has_skill(ch, gsn_warpaint))
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }
   if (argument[0] == '\0')
   {
      send_to_char("You know the following designs:\n\r", ch);
      pn = 0;
      save_name[0] = '\0';
      while( warpaint_table[pn].name != NULL)
      {
         if (str_cmp(save_name, warpaint_table[pn].name))
         {
            sprintf(save_name,  "%s", warpaint_table[pn].name);
            if (warpaint_table[pn].level <= ch->level)
            {
               send_to_char(warpaint_table[pn].name, ch);
               send_to_char("\n\r", ch);
            }
         }
         pn++;
      }
      return;
   }
   if (is_affected(ch, gsn_warpaint))
   {
      send_to_char("You are already painted.\n\r", ch);
      return;
   }
   WAIT_STATE(ch, 24);
   if (number_percent() > get_skill(ch, gsn_warpaint))
   {
      send_to_char("You draw a flawed design and have to start over.\n\r", ch);
      check_improve(ch, gsn_warpaint, FALSE, 2);
      return;
   }
   check_improve(ch, gsn_warpaint, TRUE, 2);
   pn = 0;
   while (warpaint_table[pn].name != NULL)
   {
      if (!str_cmp(warpaint_table[pn].name, argument))
      {
         af.where = warpaint_table[pn].apply_where;
         af.type = gsn_warpaint;
         af.level = ch->level;
         af.modifier = warpaint_table[pn].apply_modifier;
         af.location = warpaint_table[pn].apply_location;
         af.duration = 12;
         af.bitvector = warpaint_table[pn].apply_bitvector;
         if (warpaint_table[pn].level <= ch->level)
         affect_to_char(ch, &af);
      }
      pn++;
   }
   if (!is_affected(ch, gsn_warpaint))
   {
      send_to_char("You do not know that design.\n\r", ch);
   }
   else
   {
      send_to_char("You paint yourself with an ornate design.\n\r", ch);
      act( "$n paints $mself with an ornate design.", ch, NULL, NULL, TO_ROOM);
   }
   return;
}

void do_beastialstance(CHAR_DATA *ch, char * argument)
{
   int pn = 0;
   char save_name[MAX_STRING_LENGTH];
   AFFECT_DATA af;
   int sn;

   sn = gsn_beaststance;
   if (get_skill(ch, sn) <= 0 || !has_skill(ch, sn))
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }
   if (argument[0] == '\0')
   {
      send_to_char("You know the following stances:\n\r", ch);
      pn = 0;
      save_name[0] = '\0';
      while( animal_table[pn].name != NULL)
      {
         if
         (
            !(
               (
                  (animal_table[pn].restrict_r_bit & any_race_r) ||
                  (animal_table[pn].restrict_e2_bit & any_race_e2)
               ) &&
               !IS_SET
               (
                  animal_table[pn].restrict_r_bit,
                  pc_race_table[ch->race].restrict_r_bit
               ) &&
               !IS_SET
               (
                  animal_table[pn].restrict_e2_bit,
                  pc_race_table[ch->race].restrict_e2_bit
               )
            ) &&
            str_cmp(save_name, animal_table[pn].name)
         )
         {
            sprintf(save_name,  "%s", animal_table[pn].name);
            if (animal_table[pn].level <= ch->level)
            {
               send_to_char(animal_table[pn].name, ch);
               send_to_char("\n\r", ch);
            }
         }
         pn++;
      }
      return;
   }
   if (!str_cmp(argument, "none"))
   {
      if (is_affected(ch, sn))
      {
         affect_strip(ch, sn);
         send_to_char("You stop mimicing animals.\n\r", ch);
         return;
      }
      send_to_char("You are not mimicing any animal.\n\r", ch);
      return;
   }
   pn = 0;
   while (animal_table[pn].name != NULL)
   {
      if
      (
         !str_cmp(animal_table[pn].name, argument) &&
         animal_table[pn].level <= ch->level &&
         !(
            (
               (animal_table[pn].restrict_r_bit & any_race_r) ||
               (animal_table[pn].restrict_e2_bit & any_race_e2)
            ) &&
            !IS_SET
            (
               animal_table[pn].restrict_r_bit,
               pc_race_table[ch->race].restrict_r_bit
            ) &&
            !IS_SET
            (
               animal_table[pn].restrict_e2_bit,
               pc_race_table[ch->race].restrict_e2_bit
            )
         )
      )
      {
         if (is_affected(ch, sn))
         {
            affect_strip(ch, sn);
         }
         WAIT_STATE(ch, 12);
         if (number_percent() > get_skill(ch, sn))
         {
            send_to_char("You fail to mimic that animal.\n\r", ch);
            check_improve(ch, sn, FALSE, 2);
            return;
         }
         af.where = animal_table[pn].apply_where;
         af.type = sn;
         af.level = ch->level;
         af.modifier = animal_table[pn].apply_modifier;
         af.location = animal_table[pn].apply_location;
         af.duration = 24;
         af.bitvector = 0;
         affect_to_char(ch, &af);
         if (animal_table[pn].apply_bitvector != 0)
         {
            af.where = TO_AFFECTS;
            af.modifier = 0;
            af.location = 0;
            af.bitvector = animal_table[pn].apply_bitvector;
            affect_to_char(ch, &af);
         }
         check_improve(ch, sn, TRUE, 2);
         send_to_char("You begin to mimic the animal.\n\r", ch);
         act( "$n begins acting like a wild beast.", ch, NULL, NULL, TO_ROOM);
         return;
      }
      pn++;
   }
   send_to_char("You do not know how to mimic that animal.\n\r", ch);
}

/*
*   Function constructs and sends 1 line of a who list
*   to the ch for the victim's name complete with all
*   of his special who tags.
*
*/
void add_who_tags(CHAR_DATA* ch, CHAR_DATA* victim, char* buf)
{
   char const* class;

   class = class_table[victim->class].who_name;
   if (IS_NPC(victim))
   {
      return;
   }

   switch (victim->level)
   {
      default:
      {
         break;
      }
      case (MAX_LEVEL - 0):
      {
         class = "IMP";
         break;
      }
      case (MAX_LEVEL - 1):
      {
         class = "CRE";
         break;
      }
      case (MAX_LEVEL - 2):
      {
         class = "SUP";
         break;
      }
      case (MAX_LEVEL - 3):
      {
         class = "DEI";
         break;
      }
      case (MAX_LEVEL - 4):
      {
         class = "GOD";
         break;
      }
      case (MAX_LEVEL - 5):
      {
         class = "IMM";
         break;
      }
      case (MAX_LEVEL - 6):
      {
         class = "DEM";
         break;
      }
      case (MAX_LEVEL - 7):
      {
         class = "ANG";
         break;
      }
      case (MAX_LEVEL - 8):
      {
         class = "AVA";
         break;
      }
   }

   buf[0] = '\0';
   if
   (
      victim->level >= 52 ||
      IS_IMMORTAL(ch) ||
      victim == ch
   )
   {
      sprintf
      (
         buf,
         "[%2d %5s %s] ",
         victim->level,
         (
            victim->race < MAX_PC_RACE ?
            pc_race_table[victim->race].who_name :
            "     "
         ),
         class
      );
      if (victim->pcdata->called_by != NULL && ch->house == HOUSE_CRUSADER)
      {
         strcat(buf, "(");
         strcat(buf, victim->pcdata->called_by->name);
         strcat(buf, ") ");
      }
      if (is_in_pk(ch, victim))
      {
         strcat(buf, "(PK) ");
      }
      if (victim->incog_level >= LEVEL_HERO)
      {
         strcat(buf, "(Incog) ");
      }
      if (victim->invis_level >= LEVEL_HERO)
      {
         strcat(buf, "(Wizi) ");
      }
      if
      (
         IS_SET(victim->act2, PLR_IS_ANCIENT) &&
         victim->house == 0
      )
      {
         strcat(buf, "[ancient] ");
      }
      if (victim->house != 0)
      {
         switch (victim->house)
         {
            default:
            {
               strcat(buf, house_table[victim->house].who_name);
               break;
            }
            case (HOUSE_EMPIRE):
            {
               if (victim->pcdata->house_rank == 6)
               {
                  strcat(buf, "[SENATOR] ");
               }
               else
               {
                  strcat(buf, "[EMPIRE] ");
               }
               break;
            }
         }
         switch (victim->pcdata->induct)
         {
            case (INDUCT_APPRENTICE):
            {
               strcat(buf, "(Apprentice) ");
               break;
            }
            case (INDUCT_EMERITUS):
            {
               strcat(buf, "(Emeritus) ");
               break;
            }
            default:
            {
               break;
            }
         }
      }
      if
      (
         victim->pcdata->clan != 0 &&
         clan_table[victim->pcdata->clan].rank >= 2 &&
         (
            IS_NPC(ch) ||
            IS_IMMORTAL(ch) ||
            ch->pcdata->clan == victim->pcdata->clan ||
            (
               clan_table[victim->pcdata->clan].is_public &&
               clan_table[victim->pcdata->clan].rank == 3
            )
         )
      )
      {
         strcat(buf, "[");
         strcat(buf, clan_table[victim->pcdata->clan].who_name);
         strcat(buf, "] ");
      }
      if
      (
         (
            IS_IMMORTAL(ch) ||
            victim == ch
         ) &&
         victim->level > 5
      )
      {
         char* description;

         if (victim->race == grn_book)
         {
            description = victim->pcdata->book_description;
         }
         else
         {
            description = victim->description;
         }
         if
         (
            description == NULL ||
            strlen(description) < 2
         )
         {
            strcat(buf, "(No Description) ");
         }
         else if (strlen(description) < 60 * 5)
         {
            strcat(buf, "(Short Description) ");
         }
         if
         (
            victim->level > 29 &&
            !IS_IMMORTAL(victim)
         )
         {
            if
            (
               victim->pcdata->psych_desc == NULL ||
               strlen(victim->pcdata->psych_desc) < 3
            )
            {
               strcat(buf, "(No Psych) ");
            }
            else if (strlen(victim->pcdata->psych_desc) < 60 * 5)
            {
               strcat(buf, "(Short Psych) ");
            }
         }
         /* - Wicket */
      }

      if (IS_SET(victim->act2, PLR_NOVICE))
      {
         switch(victim->house)
         {
            default:
            {
               strcat(buf, "(Novice) ");
               break;
            }
            case (HOUSE_BRETHREN):
            {
               strcat(buf, "(Minion) ");
               break;
            }
         }
      }
      if
      (
         ch->level > 51 &&
         IS_SET(victim->comm, COMM_MORON)
      )
      {
         strcat(buf, "[MORON] ");
      }
      if (IS_SET(victim->act2, PLR_LAWLESS))
      {
         strcat(buf, "(LAWLESS) ");
      }
      else if (IS_SET(victim->act, PLR_CRIMINAL))
      {
         strcat(buf, "(CRIMINAL) ");
      }
      if
      (
         (
            (
               victim->house != 0 &&
               covenant_allies[victim->house]
            ) ||
            IS_SET(victim->act, PLR_COVENANT_ALLY)
         ) &&
         (
            IS_IMMORTAL(ch) ||
            ch->house == HOUSE_COVENANT ||
            ch == victim
         )
      )
      {
         strcat(buf, "(COVENANT ALLY) ");
      }
      if
      (
         (
            IS_IMMORTAL(ch) ||
            ch->house == HOUSE_LIGHT
         ) &&
         IS_SET(victim->act, PLR_EVIL)
      )
      {
         strcat(buf, "(EVIL) ");
      }
      if (IS_SET(victim->wiznet, PLR_FORSAKEN))
      {
         strcat(buf, "(FORSAKEN) ");
      }
      if (IS_SET(victim->act2, PLR_GUILDLESS))
      {
         strcat(buf, "(GUILDLESS) ");
      }
      if (IS_SET(victim->act, PLR_MARKED))
      {
         if (IS_IMMORTAL(ch))
         {
            strcat(buf, "(MARAUDER MARKED) ");
         }
         else
         {
            if (ch->house == HOUSE_OUTLAW)
            {
               strcat(buf, "(MARKED) ");
            }
         }
      }
      if
      (
         (
            IS_SET(victim->act2, PLR_ANCIENT_MARK) ||
            IS_SET(victim->act2, PLR_ANCIENT_MARK_PERM)
         ) &&
         (
            IS_IMMORTAL(ch) ||
            ch->house == HOUSE_ANCIENT
         )
      )
      {
         strcat(buf, "(MARKED) ");
      }
      if (victim->pcdata->pre_title[0] != '\0')
      {
         strcat(buf, victim->pcdata->pre_title);
         strcat(buf, " ");
      }
      strcat
      (
         buf,
         victim->name
      );
      if
      (
         victim->level >= LEVEL_HERO &&
         IS_SET(ch->comm2, COMM_SEE_SURNAME) &&
         IS_SET(victim->comm2, COMM_SURNAME_ACCEPTED) &&
         victim->pcdata->surname[0] != '\0'
      )
      {
         strcat(buf, " ");
         strcat(buf, victim->pcdata->surname);
      }
      else if
      (
         IS_SET(ch->comm2, COMM_SEE_SURNAME) &&
         victim->pcdata->surname[0] != '\0'
      )
      {
         strcat(buf, " [");
         if (!IS_SET(victim->comm2, COMM_SURNAME_ACCEPTED))
         {
            strcat(buf, "-");
         }
         strcat(buf, victim->pcdata->surname);
         if (!IS_SET(victim->comm2, COMM_SURNAME_ACCEPTED))
         {
            strcat(buf, "-");
         }
         strcat(buf, "]");
      }
      strcat(buf, victim->pcdata->title);
      if (victim->pcdata->extitle)
      {
         strcat(buf, victim->pcdata->extitle);
      }
      if (is_affected(victim, gsn_cloak_form))
      {
         strcat(buf, " (Cloaked)");
      }
   }
   else
   {
      sprintf
      (
         buf,
         "[   %5s    ] ",
         (
            victim->race < MAX_PC_RACE ?
            pc_race_table[victim->race].who_name :
            "     "
         )
      );
      if
      (
         victim->pcdata->called_by &&
         ch->house == HOUSE_CRUSADER
      )
      {
         strcat(buf, "(");
         strcat(buf, victim->pcdata->called_by->name);
         strcat(buf, ") ");
      }
      if (is_in_pk(ch, victim))
      {
         strcat(buf, "(PK) ");
      }
      if (victim->incog_level >= LEVEL_HERO)
      {
         strcat(buf, "(Incog) ");
      }
      if (victim->invis_level >= LEVEL_HERO)
      {
         strcat(buf, "(Wizi) ");
      }
      if (victim->house != 0)
      {
         if
         (
            ch->house == victim->house ||
            IS_IMMORTAL(ch) ||
            (
               victim->pcdata->induct > INDUCT_APPRENTICE &&
               house_table[victim->house].player_tag_who
            ) ||
            (
               victim->pcdata->induct > 0 &&
               house_table[victim->house].leader_tag_who
            ) ||
            (
               IS_IMMORTAL(victim) &&
               house_table[victim->house].imm_tag_who
            )
         )
         {
            strcat(buf, house_table[victim->house].who_name);
            switch (victim->pcdata->induct)
            {
               case (INDUCT_APPRENTICE):
               {
                  strcat(buf, "(Apprentice) ");
                  break;
               }
               case (INDUCT_EMERITUS):
               {
                  strcat(buf, "(Emeritus) ");
                  break;
               }
               default:
               {
                  break;
               }
            }
         }
      }
      if
      (
         victim->pcdata->clan != 0 &&
         clan_table[victim->pcdata->clan].rank >= 2 &&
         (
            IS_NPC(ch) ||
            IS_IMMORTAL(ch) ||
            ch->pcdata->clan == victim->pcdata->clan ||
            (
               clan_table[victim->pcdata->clan].is_public &&
               clan_table[victim->pcdata->clan].rank == 3
            )
         )
      )
      {
         strcat(buf, "[");
         strcat(buf, clan_table[victim->pcdata->clan].who_name);
         strcat(buf, "] ");
      }
      if
      (
         IS_SET(victim->act2, PLR_NOVICE) &&
         victim->house == ch->house
      )
      {
         switch(victim->house)
         {
            default:
            {
               strcat(buf, "(Novice) ");
               break;
            }
            case (HOUSE_BRETHREN):
            {
               strcat(buf, "(Minion) ");
               break;
            }
         }
      }
      if (IS_SET(victim->act2, PLR_LAWLESS))
      {
         strcat(buf, "(LAWLESS) ");
      }
      else if (IS_SET(victim->act, PLR_CRIMINAL))
      {
         strcat(buf, "(CRIMINAL) ");
      }
      if
      (
         (
            (
               victim->house != 0 &&
               covenant_allies[victim->house]
            ) ||
            IS_SET(victim->act, PLR_COVENANT_ALLY)
         ) &&
         (
            IS_IMMORTAL(ch) ||
            ch->house == HOUSE_COVENANT ||
            ch == victim
         )
      )
      {
         strcat(buf, "(COVENANT ALLY) ");
      }
      if
      (
         ch->house == HOUSE_LIGHT &&
         IS_SET(victim->act, PLR_EVIL)
      )
      {
         strcat(buf, "(EVIL) ");
      }
      if (IS_SET(victim->wiznet, PLR_FORSAKEN))
      {
         strcat(buf, "(FORSAKEN) ");
      }
      if (IS_SET(victim->act2, PLR_GUILDLESS))
      {
         strcat(buf, "(GUILDLESS) ");
      }
      if
      (
         IS_SET(victim->act, PLR_MARKED) &&
         ( ch->house == HOUSE_OUTLAW || ch->house == HOUSE_MARAUDER )
      )
      {
         strcat(buf, "(MARKED) ");
      }
      if
      (
         (
            IS_SET(victim->act2, PLR_ANCIENT_MARK) ||
            IS_SET(victim->act2, PLR_ANCIENT_MARK_PERM)
         ) &&
         ch->house == HOUSE_ANCIENT
      )
      {
         strcat(buf, "(MARKED) ");
      }
      if (victim->pcdata->pre_title[0] != '\0')
      {
         strcat(buf, victim->pcdata->pre_title);
         strcat(buf, " ");
      }
      strcat
      (
         buf,
         victim->name
      );
      if
      (
         victim->level >= LEVEL_HERO &&
         IS_SET(ch->comm2, COMM_SEE_SURNAME) &&
         IS_SET(victim->comm2, COMM_SURNAME_ACCEPTED) &&
         victim->pcdata->surname[0] != '\0'
      )
      {
         strcat(buf, " ");
         strcat(buf, victim->pcdata->surname);
      }
      strcat(buf, victim->pcdata->title);
      if (victim->pcdata->extitle)
      {
         strcat(buf, victim->pcdata->extitle);
      }
      if (is_affected(victim, gsn_cloak_form))
      {
         strcat(buf, " (Cloaked)");
      }
   }
   strcat(buf, "\n\r");
   return;
}

void do_view(CHAR_DATA *ch, char *argument)
{
   DESCRIPTOR_DATA *d;
   char buf[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];
   char rletter;
   int percent = 0;

   if (ch->in_room == NULL) return;
   if (ch->in_room->vnum < 10462 || ch->in_room->vnum > 10496)
   {
      send_to_char("You enjoy the view around you.\n\r", ch);
      act("$n looks around $mself enjoying the view.", ch, NULL, NULL, TO_ROOM);
      return;
   }
   send_to_char("\n\rYou view the arena below and its combatants:\n\r", ch);
   send_to_char("            +-----+\n\r", ch);
   send_to_char("            +  A  +\n\r", ch);
   send_to_char("      +-----+-----+-----+\n\r", ch);
   send_to_char("      +  B  +  C  +  D  +\n\r", ch);
   send_to_char("+-----+-----+-----+-----+-----+\n\r", ch);
   send_to_char("+  E  +  F  +  G  +  H  +  I  +\n\r", ch);
   send_to_char("+-----+-----+-----+-----+-----+\n\r", ch);
   send_to_char("+  J  +  K  +  L  +  M  +  N  +\n\r", ch);
   send_to_char("+-----+-----+-----+-----+-----+\n\r", ch);
   send_to_char("+  O  +  P  +  Q  +  R  +  S  +\n\r", ch);
   send_to_char("+-----+-----+-----+-----+-----+\n\r", ch);
   send_to_char("      +  T  +  U  +  V  +\n\r", ch);
   send_to_char("      +-----+-----+-----+\n\r", ch);
   send_to_char("            +  W  +\n\r", ch);
   send_to_char("            +-----+\n\r\n\r", ch);

   for (d = descriptor_list; d != NULL; d = d->next)
   {
      CHAR_DATA *wch;

      if (d->connected != CON_PLAYING || !can_see(ch, d->character))
      continue;

      wch = ( d->original != NULL ) ? d->original : d->character;
      if (IS_IMMORTAL(wch)) continue;
      if (wch->in_room == NULL) continue;
      if (wch->in_room->vnum < 10437 || wch->in_room->vnum > 10461) continue;
      if ( wch->max_hit > 0 )
      percent = ( 100 * wch->hit ) / wch->max_hit;
      else
      percent = -1;
      if (percent >= 100)
      sprintf(buf, " is in perfect condition");
      else if (percent >= 95)
      sprintf(buf,  " has a few scratches");
      else if (percent >= 90)
      sprintf(buf, " has a few bruises");
      else if (percent >= 80)
      sprintf(buf, " has some small wounds");
      else if (percent >= 70)
      sprintf(buf, " has quite a few wounds");
      else if (percent >= 60)
      sprintf(buf, " is covered in bleeding wounds");
      else if (percent >= 50)
      sprintf(buf, " is bleeding profusely");
      else if (percent >= 40)
      sprintf(buf, " is gushing blood");
      else if (percent >= 30)
      sprintf(buf, " is screaming in pain");
      else if (percent >= 20)
      sprintf(buf, " is spasming in shock");
      else if (percent >= 10)
      sprintf(buf, " is writhing in agony");
      else if (percent >= 1)
      sprintf(buf, " is convulsing on the ground");
      else
      sprintf(buf, " is nearly dead");
      switch(wch->in_room->vnum)
      {
         default:    rletter = '-'; break;
         case 10461: rletter = 'A'; break;
         case 10458: rletter = 'B'; break;
         case 10459: rletter = 'C'; break;
         case 10460: rletter = 'D'; break;
         case 10453: rletter = 'E'; break;
         case 10454: rletter = 'F'; break;
         case 10455: rletter = 'G'; break;
         case 10456: rletter = 'H'; break;
         case 10457: rletter = 'I'; break;
         case 10447: rletter = 'J'; break;
         case 10448: rletter = 'K'; break;
         case 10449: rletter = 'L'; break;
         case 10450: rletter = 'M'; break;
         case 10451: rletter = 'N'; break;
         case 10441: rletter = 'O'; break;
         case 10442: rletter = 'P'; break;
         case 10443: rletter = 'Q'; break;
         case 10444: rletter = 'R'; break;
         case 10445: rletter = 'S'; break;
         case 10439: rletter = 'T'; break;
         case 10438: rletter = 'U'; break;
         case 10440: rletter = 'V'; break;
         case 10437: rletter = 'W'; break;
      }
      sprintf(buf2, "%s%s at %s [%c]\n\r", get_name(wch, ch), buf, wch->in_room->name, rletter);
      send_to_char(buf2, ch);
   }
   return;
}

void append_position(char *buf, CHAR_DATA *ch, CHAR_DATA *looker)
{
   char message[MAX_STRING_LENGTH];

   if (is_affected(ch, gsn_submerge))
   {
      strcat(buf, " is beneath the waves!\n\r" );
      return;
   }
   switch ( ch->position )
   {
      case POS_DEAD:     strcat( buf, " is DEAD!!" );              break;
      case POS_MORTAL:   strcat( buf, " is mortally wounded." );   break;
      case POS_INCAP:    strcat( buf, " is incapacitated." );      break;
      case POS_STUNNED:  strcat( buf, " is lying here stunned." ); break;
      case POS_SLEEPING:
      if (ch->on != NULL)
      {
         if (IS_SET(ch->on->value[2], SLEEP_AT))
         {
            sprintf(message,  " is sleeping at %s.",
            ch->on->short_descr);
            strcat(buf, message);
         }
         else if (IS_SET(ch->on->value[2], SLEEP_ON))
         {
            sprintf(message,  " is sleeping on %s.",
            ch->on->short_descr);
            strcat(buf, message);
         }
         else
         {
            sprintf(message,  " is sleeping in %s.",
            ch->on->short_descr);
            strcat(buf, message);
         }
      }
      else
      strcat(buf, " is sleeping here.");
      break;
      case POS_RESTING:
      if (ch->on != NULL)
      {
         if (IS_SET(ch->on->value[2], REST_AT))
         {
            sprintf(message,  " is resting at %s.",
            ch->on->short_descr);
            strcat(buf, message);
         }
         else if (IS_SET(ch->on->value[2], REST_ON))
         {
            sprintf(message,  " is resting on %s.",
            ch->on->short_descr);
            strcat(buf, message);
         }
         else
         {
            sprintf(message,  " is resting in %s.",
            ch->on->short_descr);
            strcat(buf, message);
         }
      }
      else
      strcat( buf, " is resting here." );
      break;
      case POS_SITTING:
      if (ch->on != NULL)
      {
         if (IS_SET(ch->on->value[2], SIT_AT))
         {
            sprintf(message,  " is sitting at %s.",
            ch->on->short_descr);
            strcat(buf, message);
         }
         else if (IS_SET(ch->on->value[2], SIT_ON))
         {
            sprintf(message,  " is sitting on %s.",
            ch->on->short_descr);
            strcat(buf, message);
         }
         else
         {
            sprintf(message,  " is sitting in %s.",
            ch->on->short_descr);
            strcat(buf, message);
         }
      }
      else
      strcat(buf, " is sitting here.");
      break;
      case POS_STANDING:
      if (ch->on != NULL)
      {
         if (IS_SET(ch->on->value[2], STAND_AT))
         {
            sprintf(message,  " is standing at %s.",
            ch->on->short_descr);
            strcat(buf, message);
         }
         else if (IS_SET(ch->on->value[2], STAND_ON))
         {
            sprintf(message,  " is standing on %s.",
            ch->on->short_descr);
            strcat(buf, message);
         }
         else
         {
            sprintf(message, " is standing in %s.",
            ch->on->short_descr);
            strcat(buf, message);
         }
      }
      else
      strcat( buf, " is here." );
      break;
      case POS_FIGHTING:
      {
         if (!IS_NPC(ch) && IS_SET(ch->act2, PLR_NO_FIGHT))
         {
            strcat(buf, " is here, under attack by ");
         }
         else
         {
            strcat(buf, " is here, fighting ");
         }
         if (ch->fighting == NULL)
         {
            strcat(buf, "thin air??");
         }
         else if
         (
            ch->fighting == looker &&
            looker != NULL
         )
         {
            strcat(buf, "YOU!");
         }
         else if (ch->in_room == ch->fighting->in_room)
         {
            strcat(buf, PERS(ch->fighting, looker));
            strcat(buf, ".");
         }
         else
         {
            strcat(buf, "someone who left??");
         }
         break;
      }
   }
   strcat( buf, "\n\r");
   return;
}

void do_call_crusader(CHAR_DATA* ch, char* argument)
{
   DESCRIPTOR_DATA* d;
   char arg[MAX_INPUT_LENGTH];
   char buf[MAX_INPUT_LENGTH];

   if (ch->house != HOUSE_CRUSADER)
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }
   if
   (
      (
         IS_NPC(ch) ||
         ch->pcdata->induct <= INDUCT_APPRENTICE
      ) &&
      !IS_IMMORTAL(ch)
   )
   {
      send_to_char("You cannot call targets.\n\r", ch);
      return;
   }
   if (argument[0] == '\0')
   {
      send_to_char("Call whom?\n\r", ch);
      return;
   }
   strcpy(arg, argument);
   arg[0] = UPPER(arg[0]);
   if (!str_cmp(arg, ch->name) || !str_cmp(arg, "self"))
   {
      send_to_char("You cannot call yourself.\n\r", ch);
      return;
   }
   for (d = descriptor_list; d != NULL; d = d->next)
   {
      CHAR_DATA *wch;
      if (d->connected != CON_PLAYING)
      {
         continue;
      }
      wch = (d->original != NULL) ? d->original : d->character;
      if (!can_see(ch, wch) || is_affected(wch, gsn_cloak_form))
      {
         continue;
      }
      if (!str_cmp(arg, wch->name))
      {
         if (!is_in_pk(ch, wch) && !IS_IMMORTAL(ch))
         {
            send_to_char("You cannot hunt ", ch);
            send_to_char(arg, ch);
            send_to_char(".\n\r", ch);
            return;
         }
         if (wch->pcdata->called_by == ch)
         {
            send_to_char("You have already called ", ch);
            send_to_char(arg, ch);
            send_to_char(".\n\r", ch);
            return;
         }
         else if (wch->pcdata->called_by == NULL)
         {
            send_to_char("You call ", ch);
            send_to_char(arg, ch);
            send_to_char(".\n\r", ch);
            wch->pcdata->called_by = ch;
            sprintf
            (
               buf,
               "{B{3%s calls %s.{n",
               ch->name,
               wch->name
            );
            do_hecho(ch, buf);
            return;
         }
         else
         {
            send_to_char(arg, ch);
            send_to_char(" is called by ", ch);
            send_to_char(wch->pcdata->called_by->name, ch);
            send_to_char(".\n\r", ch);
            return;
         }
      }
   }
   send_to_char("They are not here.\n\r", ch);
}

void do_release_crusader(CHAR_DATA* ch, char* argument)
{
   bool found = FALSE;
   bool all   = FALSE;
   DESCRIPTOR_DATA* d;
   char buf[MAX_INPUT_LENGTH];
   char arg[MAX_INPUT_LENGTH];

   if
   (
      ch->house != HOUSE_CRUSADER ||
      (
         !IS_NPC(ch) &&
         ch->pcdata->induct <= INDUCT_APPRENTICE &&
         !IS_IMMORTAL(ch)
      )
   )
   {
      do_credits(ch, argument);
      /*
      credits used to be first on the interp list
      this lets crusaders use shorthand for crelease,
      and default to credits for non-crusaders
      */
      return;
   }
   if (IS_NPC(ch))
   {
      send_to_char("NPC's cannot release targets.\n\r", ch);
      return;
   }
   strcpy(arg, argument);
   arg[0] = UPPER(arg[0]);
   if (!str_cmp(arg, ch->name) || !str_cmp(arg, "self"))
   {
      send_to_char("You cannot release yourself.\n\r", ch);
      return;
   }
   if (arg[0] == '\0')
   {
      send_to_char("Release whom?\n\r", ch);
      return;
   }
   if (!str_cmp(arg, "all"))
   {
      all = TRUE;
   }

   for (d = descriptor_list; d != NULL; d = d->next)
   {
      CHAR_DATA *wch;
      if (d->connected != CON_PLAYING)
      {
         continue;
      }
      wch = (d->original != NULL) ? d->original : d->character;
      if (is_affected(wch, gsn_cloak_form))
      {
         continue;
      }
      if
      (
         !can_see(ch, wch) &&
         IS_IMMORTAL(wch)
      )
      {
         continue;
      }
      if (all && wch->pcdata->called_by == ch)
      {
         found = TRUE;
         wch->pcdata->called_by = NULL;
         sprintf
         (
            buf,
            "{B{3%s releases %s.{n",
            ch->name,
            wch->name
         );
         do_hecho(ch, buf);
      }
      else if (!all && !str_cmp(arg, wch->name))
      {
         if (wch->pcdata->called_by == ch)
         {
            send_to_char("You release ", ch);
            send_to_char(arg, ch);
            send_to_char(".\n\r", ch);
            wch->pcdata->called_by = NULL;
            sprintf
            (
               buf,
               "{B{3%s releases %s.{n",
               ch->name,
               wch->name
            );
            do_hecho(ch, buf);
            return;
         }
         else if (wch->pcdata->called_by == NULL)
         {
            send_to_char(arg, ch);
            send_to_char(" is not called.\n\r", ch);
            return;
         }
         else
         {
            send_to_char(arg, ch);
            send_to_char(" is called by ", ch);
            send_to_char(wch->pcdata->called_by->name, ch);
            send_to_char(".\n\r", ch);
            return;
         }
      }
   }
   if (!found)
   {
      if (all)
      {
         send_to_char("You have no targets called.\n\r", ch);
      }
      else
      {
         send_to_char("They are not here.\n\r", ch);
      }
   }
   else
   {
      send_to_char("You have released all your targets.\n\r", ch);
   }
   return;
}

MUD_TIME current_mud_time;

void timeval_to_mudtime(TIMEVAL* input, MUD_TIME* output)
{
   int second = input->tv_sec;
   int micro  = input->tv_usec;
   int sub_hour;
   int above_hour;

   /* Set to TIME_0, OCT 21, 1996, 00:00:00 */
   second        -= TIME_0;
   /*
   Set sub_hour to be number of seconds (less than 3600 = hour)
   set above_hour to be number of hours
   */
   sub_hour       = micro * 30 / 1000000 + second % 120 * 30;
   above_hour     = second / 120 + sub_hour / 3600;
   sub_hour      %= 3600;
   /* Set seconds and minutes */
   output->second = sub_hour % 60;
   output->minute = sub_hour / 60;
   /* Set hours, days, weeks, months and years */
   output->hour   = above_hour % 24;
   above_hour    /= 24;
   output->day    = above_hour % 7;
   above_hour    /= 7;
   output->week   = above_hour % 4;
   above_hour    /= 4;
   output->month  = above_hour % 15;
   output->year   = above_hour / 15;

   /* In case of time corruption, no negative values */
   SET_ABSOLUTE(output->year)
   SET_ABSOLUTE(output->month)
   SET_ABSOLUTE(output->week)
   SET_ABSOLUTE(output->day)
   SET_ABSOLUTE(output->hour)
   SET_ABSOLUTE(output->minute)
   SET_ABSOLUTE(output->second)
}

void mudtime_to_timeval(MUD_TIME* input, TIMEVAL* output)
{
   int sub_hour   = 0;
   int above_hour = 0;

   output->tv_sec = 0;
   output->tv_usec = 0;
   above_hour += input->year;
   above_hour *= 15;
   above_hour += input->month;
   above_hour *= 4;
   above_hour += input->week;
   above_hour *= 7;
   above_hour += input->day;
   above_hour *= 24;
   above_hour += input->hour;
   sub_hour = input->second + input->minute * 60;
   output->tv_usec = sub_hour % 30 * 1000000 / 30;
   sub_hour /= 30;
   sub_hour += above_hour % 30 * 120;
   above_hour /= 30;
   output->tv_sec = sub_hour + above_hour * 3600 + TIME_0;
}

char* get_time_string(CHAR_DATA* ch, CHAR_DATA* vch, AFFECT_DATA* paf, ROOM_AFFECT_DATA* praf, char* buf, int type, bool ticks)
{
   TIMEVAL convert;
   extern int     pulse_point;
   int pulses;
   MUD_TIME new_time;
   int duration;
   int sn;
   int accuracy;

   if (paf != NULL)
   {
      duration = paf->duration;
      sn       = paf->type;
   }
   else if (praf != NULL)
   {
      duration = praf->duration;
      sn       = praf->type;
   }
   else
   {
      strcpy(buf, "");
      return buf;
   }
   if (vch == NULL)
   {
      vch = ch;
   }
   if
   (
      !IS_IMMORTAL(ch) &&
      (
         sn == gsn_distort_time ||
         sn == gsn_distort_time_faster ||
         sn == gsn_distort_time_slower
      )
   )  /* distort time.. replace the right gsn's */
   {
      strcpy(buf, " for an unknown length of time");
      return buf;
   }
   if (duration == -1)
   {
      strcpy(buf, " permanently");
      return buf;
   }
   if (ticks)
   {
      return mud_tick_string(duration, buf);
   }
   /* Normal time now! */
   pulses = duration * PULSE_TICK + pulse_point - 1;
   if
   (
      sn != gsn_distort_time_faster &&
      sn != gsn_distort_time_slower &&
      sn != gsn_distort_time &&
      praf == NULL &&
      paf->where == TO_AFFECTS
   )
   {
      if (is_affected(vch, gsn_distort_time_faster))
      {
         if (duration % 2)  /* if duration is odd */
         {
            pulses = (duration / 2 + 1) * PULSE_TICK + pulse_point - 1;
         }
         else
         {
            /* duration is even, each tick counts half */
            pulses = (duration / 2 * PULSE_TICK) + pulse_point - 1;
         }
      }
      else if (is_affected(vch, gsn_distort_time_slower))
      {
         AFFECT_DATA* distort;
         bool distort_first = TRUE;
         bool odd = FALSE;

         for (distort = vch->affected; distort; distort = distort->next)
         {
            if (distort == paf)
            {
               distort_first = FALSE;
            }
            else if (distort->type == gsn_distort_time)
            {
               if (distort->duration % 2)  /* odd duration */
               {
                  odd = TRUE;
               }
               else
               {
                  odd = FALSE;
               }
               break;
            }
         }
         if (distort_first)
         {
            /*
            the distort will change to even duration before
            the paf in question is checked
            */
            odd = !odd;
         }
         if (odd)
         {
            pulses = (duration * 2 * PULSE_TICK) + pulse_point - 1;
         }
         else
         {
            pulses = ((duration * 2 + 1) * PULSE_TICK) + pulse_point - 1;
         }
      }
   }
   if (pulses <= 0)
   {
      strcpy(buf, " for no time at all");
      return buf;
   }
   /* Calculate duration as a time_val */
   convert.tv_sec   = 0;
   convert.tv_usec  = 0;
   convert.tv_sec  += pulses / PULSE_PER_SECOND;
   convert.tv_usec += ((pulses % PULSE_PER_SECOND) * 1000000 / PULSE_PER_SECOND);
   /* Normalize to real time */
   convert.tv_sec  += TIME_0;
   /* convert to MUD_TIME */
   timeval_to_mudtime(&convert, &new_time);
   /* Calculate accuracy */
   accuracy = get_time_accuracy(ch, type);
   set_time_to_accuracy(&new_time, accuracy);
   return mud_time_string(&new_time, buf, (accuracy == 1));
}

void set_time_to_accuracy(MUD_TIME* time, int accuracy)
{
   if (accuracy < 1)
   {
      accuracy = 1;
   }
   if (accuracy >= TIME_YEAR)
   {
      time->month  = 0;
      time->week   = 0;
      time->day    = 0;
      time->hour   = 0;
      time->minute = 0;
      time->second = 0;
      time->year -= time->year % (accuracy / TIME_YEAR);
      return;
   }
   if (accuracy >= TIME_MONTH)
   {
      time->week   = 0;
      time->day    = 0;
      time->hour   = 0;
      time->minute = 0;
      time->second = 0;
      time->month -= time->month % (accuracy / TIME_MONTH);
      return;
   }
   if (accuracy >= TIME_WEEK)
   {
      time->day    = 0;
      time->hour   = 0;
      time->minute = 0;
      time->second = 0;
      time->week -= time->week % (accuracy / TIME_WEEK);
      return;
   }
   if (accuracy >= TIME_DAY)
   {
      time->hour   = 0;
      time->minute = 0;
      time->second = 0;
      time->day -= time->day % (accuracy / TIME_DAY);
      return;
   }
   if (accuracy >= TIME_HOUR)
   {
      time->minute = 0;
      time->second = 0;
      time->hour -= time->hour % (accuracy / TIME_HOUR);
      return;
   }
   if (accuracy >= TIME_MINUTE)
   {
      time->second = 0;
      time->minute -= time->minute % (accuracy / TIME_MINUTE);
      return;
   }
   time->second -= time->second % (accuracy / TIME_SECOND);
}

char* mud_time_string(MUD_TIME* input, char* buf, bool exact)
{
   int count = 0;
   char comma[4];

   if
   (
      !input->year &&
      !input->month &&
      !input->week &&
      !input->day &&
      !input->hour &&
      !input->minute &&
      !input->second
   )
   {
      strcpy(buf, " for no time at all");
      return buf;
   }
   if (input->year)
   {
      count++;
   }
   if (input->month)
   {
      count++;
   }
   if (input->week)
   {
      count++;
   }
   if (input->day)
   {
      count++;
   }
   if (input->hour)
   {
      count++;
   }
   if (input->minute)
   {
      count++;
   }
   if (input->second)
   {
      count++;
   }
   if (count < 3)
   {
      comma[0] = '\0';
   }
   else
   {
      comma[0] = ',';
      comma[1] = '\0';
   }
   count = 0;

   strcpy(buf, " for");
   if (!exact)
   {
      strcat(buf, " about");
   }
   if (input->year)
   {
      if (input->year > 1)
      {
         sprintf(buf, "%s %d yrs", buf, input->year);
      }
      else
      {
         strcat(buf, " 1 yr");
      }
      count++;
   }
   if (input->month)
   {
      if (count)
      {
         strcat(buf, comma);
         if
         (
            input->week   == 0 &&
            input->day    == 0 &&
            input->hour   == 0 &&
            input->minute == 0 &&
            input->second == 0
         )
         {
            strcat(buf, " and");
         }
      }
      count++;
      if (input->month > 1)
      {
         sprintf(buf, "%s %d mnths", buf, input->month);
      }
      else
      {
         strcat(buf, " 1 mnth");
      }
   }
   if (input->week)
   {
      if (count)
      {
         strcat(buf, comma);
         if
         (
            input->day    == 0 &&
            input->hour   == 0 &&
            input->minute == 0 &&
            input->second == 0
         )
         {
            strcat(buf, " and");
         }
      }
      count++;
      if (input->week > 1)
      {
         sprintf(buf, "%s %d wks", buf, input->week);
      }
      else
      {
         strcat(buf, " 1 wk");
      }
   }
   if (input->day)
   {
      if (count)
      {
         strcat(buf, comma);
         if
         (
            input->hour   == 0 &&
            input->minute == 0 &&
            input->second == 0
         )
         {
            strcat(buf, " and");
         }
      }
      count++;
      if (input->day > 1)
      {
         sprintf(buf, "%s %d days", buf, input->day);
      }
      else
      {
         strcat(buf, " 1 day");
      }
   }
   if (input->hour)
   {
      if (count)
      {
         strcat(buf, comma);
         if
         (
            input->minute == 0 &&
            input->second == 0
         )
         {
            strcat(buf, " and");
         }
      }
      count++;
      if (input->hour > 1)
      {
         sprintf(buf, "%s %d hrs", buf, input->hour);
      }
      else
      {
         strcat(buf, " 1 hr");
      }
   }
   if (input->minute)
   {
      if (count)
      {
         strcat(buf, comma);
         if
         (
            input->second == 0
         )
         {
            strcat(buf, " and");
         }
      }
      count++;
      if (input->minute > 1)
      {
         sprintf(buf, "%s %d mins", buf, input->minute);
      }
      else
      {
         strcat(buf, " 1 min");
      }
   }
   if (input->second)
   {
      if (count)
      {
         strcat(buf, comma);
         strcat(buf, " and");
      }
      count++;
      if (input->second > 1)
      {
         sprintf(buf, "%s %d secs", buf, input->second);
      }
      else
      {
         strcat(buf, " 1 sec");
      }
   }
   return buf;
}

char* mud_tick_string(int duration, char* buf)
{
   if (duration == 0)
   {
      strcpy(buf, " for 1 more tick");
   }
   else
   {
      sprintf(buf, " for %d more ticks", duration + 1);
   }
   return buf;
}

int get_time_accuracy(CHAR_DATA* ch, int type)
{
   int skill_know;
   int skill_grace;
   int skill_id;
   int accuracy = (PULSE_TICK / PULSE_PER_SECOND) * 30;

   if (IS_NPC(ch))
   {
      return 1;
   }
   skill_grace = get_skill(ch, gsn_grace);
   if (!has_skill(ch, gsn_grace) || skill_grace < 75)
   {
      skill_grace = 0;
   }
   else
   {
      skill_grace -= 75;
   }
   skill_know = get_skill(ch, gsn_know_time);
   if (!has_skill(ch, gsn_know_time) || skill_know <= 75)
   {
      skill_know = 0;
   }
   else
   {
      skill_know -= 75;
   }
   skill_id = get_skill(ch, gsn_identify);
   if (!has_skill(ch, gsn_identify) || skill_id <= 75)
   {
      skill_id = 0;
   }
   else
   {
      skill_id -= 75;
   }
   switch (type)
   {
      default:
      {
         accuracy = (PULSE_TICK / PULSE_PER_SECOND) * 30;
         break;
      }
      case (TYPE_TIME_AFFECTS):
      {
         accuracy =
         (
            (PULSE_TICK / PULSE_PER_SECOND) * 30 -
            ((GET_LEVEL(ch) - 20) * 29) -
            (skill_grace * 15)
         );
         break;
      }
      case (TYPE_TIME_KNOW_TIME):
      {
         accuracy =
         (
            (PULSE_TICK / PULSE_PER_SECOND) * 30 -
            ((GET_LEVEL(ch) - 20) * 10) -
            (skill_know * 36)
         );
         break;
      }
      case (TYPE_TIME_IDENTIFY):
      {
         accuracy =
         (
            (PULSE_TICK / PULSE_PER_SECOND) * 30 -
            ((GET_LEVEL(ch) - 20) * 12) -
            (skill_id * 21) -
            (skill_know * 7) -
            (skill_grace * 7)
         );
         break;
      }
      case (TYPE_TIME_STAT):
      {
         accuracy = 5;
         break;
      }
   }
   if (accuracy < 1)
   {
      return 1;
   }
   else
   {
      return accuracy;
   }
}

void buk_desc(CHAR_DATA* ch, CHAR_DATA* book)
{
   char first_line[MAX_INPUT_LENGTH];

   if
   (
      IS_NPC(book) ||
      book->race != grn_book
   )
   {
      send_to_char(book->description, ch);
      return;
   }
   send_to_char(book->pcdata->book_description, ch);
   send_to_char("\n\r", ch);
   /* TODO TODO put in some kind of description thingy here */
   /*
   send_to_char
   (
      "A strange covered book seems to be flapping in midair.  Its cover looks like a\n\r"
      "group of children were allowed to play over it, splashing it with many colours\n\r"
      "of paint. Sticking out at odd angles are a pair of small white, feathery wings\n\r"
      "flapping furiously to keep it afloat.  Sticking out from in between pages that\n\r"
      "consist of a strange green are purple tassles.   The book is so small, that it\n\r"
      "appears that even a gnomish child could easily beat it in size and weight.  In\n\r",
      ch
   );
   send_to_char
   (
      "great contrast to the green edges of the pages,  is the stark white color that\n\r"
      "the body of each page consists of.   Upon the white can be seen many different\n\r"
      "styles of writing in a wide varieties of inks.   In the middle of the array of\n\r"
      "colours sits a small green frog. Judging from the look of the book, it somehow\n\r"
      "does not seem out of place.  The frog sits upon what appears to have once been\n\r"
      "a title but with time and much handling, is little more than tiny fragments of\n\r"
      "rubber.\n\r"

      Desc until June 6, 2003
      "A small book, with random colors splashed all over the cover.\n\r"
      "Small white, feathery wings flap about, keeping it afloat.\n\r"
      "Around the edges bright green trim with flapping purple tassles.\n\r"
      "At one and a half feet and two pounds, a gnome seems a giant.\n\r"
      "The pages are bright white with black writing.\n\r"
      "A little green frog sits on top of the book.\n\r"
      "The cover of the book looks like it once had a title,\n\r"
      "but is now covered in tiny fragments of rubber.\n\r"

      "\n\r",
      ch
   );
   */
   /* get first line of actual description */
   if
   (
      stristr
      (
         book->description,
         "\n\r"
      ) != NULL
   )
   {
      sh_int counter;
      for (counter = 0; counter < MAX_INPUT_LENGTH - 1; counter++)
      {
         first_line[counter] = book->description[counter];
         if (first_line[counter] == '\r')
         {
            counter++;
            break;
         }
      }
      first_line[counter] = '\0';
      send_to_char(first_line, ch);
   }
}

void do_dossier_modify(CHAR_DATA* ch, char* argument, CHAR_DATA* vch)
{
   char buf      [MAX_DOSSIER_LENGTH];
   char arg1     [MAX_INPUT_LENGTH];
   char arg2     [MAX_INPUT_LENGTH];
   char new_line [MAX_INPUT_LENGTH * 2];
   char time_stamp [MAX_INPUT_LENGTH];
   char** desc;
   sh_int target_line = 0;
   sh_int num_lines   = 0;
   sh_int last_line   = 0;
   bool insert        = FALSE;
   bool empty         = FALSE;
   bool show          = FALSE;
   bool add           = FALSE;
   bool remove        = FALSE;
   bool clear         = FALSE;
   char* plusminus    = NULL;
   char* pnew         = NULL;
   char* pold         = NULL;
   sh_int place       = 0;

   /* Sanity checks, this isn't called directly by the interpreter. */
   if (ch == NULL)
   {
      /* Called incorrectly */
      bug("do_dossier_modify called with NULL ch (1st parameter)", 0);
      return;
   }
   if (vch == NULL)
   {
      /* Called incorrectly */
      bug("do_dossier_modify called with NULL vch (3rdt parameter)", 0);
      return;
   }
   if (IS_NPC(vch))
   {
      /* Called incorrectly */
      bug("do_dossier_modify called with MOB vch", 0);
      return;
   }
   desc = &(vch->pcdata->dossier);
   if (argument == NULL)
   {
      /* Called incorrectly, but can recover */
      bug("do_dossier_modify called with NULL argument (2nd parameter)", 0);
      argument = "show";
   }
   smash_tilde(argument);
   if ((argument[0] == '\0') || (!str_cmp(argument, "show")))
   {
      show = TRUE;
   }
   else if (!str_cmp(argument, "clear"))
   {
      clear = TRUE;
   }
   else if (!str_cmp(argument, "showlines"))
   {
      if (desc[0] == NULL)
      {
         send_to_char("You have no lines.\n\r", ch);
         return;
      }
      for (pold = desc[0]; pold[0] != '\0'; pold++)
      {
         if (pold[0] == '\r')
         {
            num_lines ++;
         }
      }
      if  /* Take care of case of 1000 blank lines, just in case */
      (
         (strlen(desc[0]) + (num_lines * 5) + 2) >= MAX_DOSSIER_LENGTH
      )
      {
         send_to_char("Dossier will be too long after adding lines.\n\r", ch);
         return;
      }
      else
      {
         act
         (
            "Showing $N's dossier with line numbers.",
            ch,
            NULL,
            vch,
            TO_CHAR
         );
      }
      buf[0] = '\0';
      pnew = buf;
      pold = desc[0];
      place++;
      sprintf(arg1, "%-3d: ", place);
      strcat(buf, arg1);
      pnew += 5;  /* Number is 3 digits, ':' and ' ' */
      pnew[0] = '\0';
      while (pold[0] != '\0')
      {
         pnew[0] = pold[0];
         if (pnew[0] == '\r')
         {
            pnew++;
            pold++;
            if (pold[0] != '\0')  /* Don't need line number at the end */
            {
               place++;
               sprintf(arg1, "%-3d: ", place);
               pnew[0] = '\0';
               strcat(buf, arg1);
               pnew += 5;  /* Number is 3 digits, ':' and ' ' */
            }
         }
         else
         {
            pnew++;
            pold++;
         }
      }
      pnew[0] = '\0';
      send_to_char(buf, ch);
      return;
   }
   else
   {
      /* All special commands are checked before splitting up into arguments. */
      argument = one_argument_space(argument, arg1);

      if (is_number(arg1))
      {
         insert = TRUE;
         target_line = atoi(arg1);
      }
      if (desc[0] == NULL)
      {
         empty = TRUE;
         num_lines = 0;
      }
      else
      {
         /* Count lines */
         for (pold = desc[0]; pold[0] != '\0'; pold++)
         {
            if (pold[0] == '\r')
            {
               num_lines ++;
            }
         }
      }
      if (insert && !show)
      {
         /* Need 2 parameters for line option, 3rd is line, can be blank */
         argument = one_argument_space(argument, arg2);
         if (empty)
         {
            act
            (
               "You cannot use the line option when $N's dossier is empty.",
               ch,
               NULL,
               vch,
               TO_CHAR
            );
            return;
         }
         if (target_line < 1)
         {
            send_to_char("Line number too low.\n\r", ch);
            return;
         }
         if (target_line > num_lines)
         {
            act
            (
               "There are not that many lines in $N's dossier.",
               ch,
               NULL,
               vch,
               TO_CHAR
            );
            return;
         }
         if (arg2[0] == '\0')
         {
            send_to_char
            (
               "Too few parameters.  Read the help for dossier.\n\r",
               ch
            );
            return;
         }
         plusminus = arg2;
         target_line--;  /* Need to be before the target line always. */
      }
      else
      {
         plusminus = arg1;
      }
      if (argument[0] != '\0')
      {
         sprintf
         (
            new_line,
            "%s%s\n\r",
            get_dossier_time(ch, time_stamp),
            argument
         );
      }
      else
      {
         sprintf(new_line, "%s\n\r", argument);
      }
      if (!str_cmp(plusminus, "+"))
      {
         add = TRUE;
         show = FALSE;
      }
      else if (!str_cmp(plusminus, "++"))
      {
         add = TRUE;
         show = TRUE;
      }
      else if (!str_cmp(plusminus, "-"))
      {
         remove = TRUE;
         show = FALSE;
      }
      else if (!str_cmp(plusminus, "--"))
      {
         remove = TRUE;
         show = TRUE;
      }
      else
      {
         /* Show syntax */
         send_to_char
         (
            "Syntax:\n\r"
            "dossier <victim> (parameters)\n\r"
            "dossier <victim> clear\n\r"
            "  Clears victim's dossier.\n\r"
            "dossier <victim> show\n\r"
            "  Shows victim's dossier.\n\r"
            "dossier <victim> showlines\n\r"
            "  Shows victim's dossier with line numbers.\n\r"
            "dossier <victim> <number> + (string)\n\r"
            "dossier <victim> <number> ++ (string)\n\r"
            "  example: dossier <victim> 5 ++ jerk.\n\r"
            "  Would insert '[<yourname> <date>] jerk.' before the fifth line.\n\r",
            ch
         );
         send_to_char
         (
            "  Number can be between 1 and the number of lines in victim's dossier.\n\r"
            "  If <number> is ommitted, it adds to the end.\n\r"
            "  + is identical to ++, but ++ will show victim's dossier\n\r"
            "  after every line you add.\n\r"
            "dossier <victim> <number> -\n\r"
            "dossier <victim> <number> --\n\r"
            "  example: dossier <victim> 5 --\n\r"
            "  This would remove the fifth line of victim's dossier.\n\r"
            "  example: dossier <victim> 3 -\n\r"
            "  This would remove the third line of victim's dossier.\n\r",
            ch
         );
         send_to_char
         (
            "  If <number> is omitted, it will remove the last line of victim's dossier.\n\r"
            "dossier <victim> format\n\r"
            "  Though similar to descriptions, 'format' is disabled due\n\r"
            "  to messing up timestamps.\n\r"
            "Anything else will show you this syntax string. \n\r",
            ch
         );
         return;
      }
      if (!insert && add)
      {
         target_line = num_lines;  /* Append */
      }
      else if (!insert && remove)
      {
         target_line = num_lines - 1;  /* Delete last line */
      }
      if
      (
         remove &&
         (
            empty ||
            num_lines <= 1
         )
      )
      {
         /* Gonna be empty */
         insert  = FALSE;
         clear  = TRUE;
         remove = FALSE;
         show   = FALSE;
      }
      if
      (
         !empty &&
         (
            add ||
            remove
         )
      )
      {
         last_line = 0;
         /* Get to line */
         pnew = buf;
         pold = desc[0];
         while (pold[0] != '\0')
         {
            if (last_line == target_line)
            {
               break;
            }
            pnew[0] = pold[0];
            if (pold[0] == '\r')
            {
               last_line++;
            }
            pnew++;
            pold++;
         }
         pnew[0] = '\0';  /* At the place to delete, or add */
         if (add)
         {
            if (strlen(new_line) + strlen(desc[0]) >= MAX_DESC_LENGTH)
            {
               act
               (
                  "$N's dossier is too long.",
                  ch,
                  NULL,
                  vch,
                  TO_CHAR
               );
               return;
            }
            strcat(buf, new_line);
            strcat(buf, pold);
            free_string(desc[0]);
            desc[0] = str_dup(buf);
         }
         else  /* remove */
         {
            while (pold[0] != '\0')
            {
               if (pold[0] == '\r')
               {
                  pold++;
                  break;
               }
               pold++;
            }
            strcat(buf, pold);
            free_string(desc[0]);
            desc[0] = str_dup(buf);
         }
         if (!show)
         {
            send_to_char("Ok.\n\r", ch);
         }
      }
      else if (empty && add)
      {
         free_string(desc[0]);
         desc[0] = str_dup(new_line);
         if (!show)
         {
            send_to_char("Ok.\n\r", ch);
         }
      }
   }
   if (clear)
   {
      act
      (
         "$N's dossier cleared.",
         ch,
         NULL,
         vch,
         TO_CHAR
      );
      free_string(desc[0]);
      desc[0] = str_dup("");
      return;
   }
   if (show)
   {
      act
      (
         "$N's dossier is:",
         ch,
         NULL,
         vch,
         TO_CHAR
      );
      if
      (
         desc[0] ||
         desc[0][0] == '\0'
      )
      {
         page_to_char(desc[0], ch);
      }
      else
      {
         send_to_char("(None)\n\r", ch);
      }
   }
   return;
}

void check_changeling_desc(CHAR_DATA* ch)
{
   if
   (
      IS_NPC(ch) ||
      ch->race != grn_changeling ||
      strlen(ch->description) >= 60 * 5
   )
   {
      return;
   }
   send_to_char
   (
      "Each of your changeling forms has a separate description.\n\r",
      ch
   );
   if (strlen(ch->description) < 3)
   {
      send_to_char
      (
         "\n\rThis form does not have a description.\n\r",
         ch
      );
   }
   else
   {
      send_to_char
      (
         "This form does not have a long enough description.\n\r",
         ch
      );
   }
}

void do_surname(CHAR_DATA* ch, char* argument)
{
   if (IS_NPC(ch))
   return;

   if (IS_SET(ch->comm2, COMM_SEE_SURNAME))
   {
      send_to_char("You will no longer see surnames.\n\r", ch);
      REMOVE_BIT(ch->comm2, COMM_SEE_SURNAME);
   }
   else
   {
      send_to_char("You will now see surnames.\n\r", ch);
      SET_BIT(ch->comm2, COMM_SEE_SURNAME);
   }
}

void do_relname(CHAR_DATA* ch, char* argument)
{
   CHAR_DATA* victim;
   char buf[MAX_INPUT_LENGTH];
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];

   argument = one_argument_cs(argument, arg1);
   strcpy(arg2, argument);
   smash_tilde(arg2);
   if
   (
      (
         victim = get_char_world(ch, arg1)
      ) == NULL
   )
   {
      send_to_char("They are not here.\n\r", ch);
      return;
   }
   if (IS_NPC(victim))  /* sanity check */
   {
      send_to_char("You may only resurname players.\n\r", ch);
      return;
   }
   if
   (
      ch != victim &&
      get_trust(victim) >= get_trust(ch)
   )
   {
      act("You cannot mess with $N.", ch, NULL, victim, TO_CHAR);
      return;
   }
   free_string(victim->pcdata->surname);
   victim->pcdata->surname = str_dup(arg2);
   REMOVE_BIT(victim->comm2, COMM_SURNAME_ACCEPTED);
   if (arg2[0] != '\0')
   {
      sprintf
      (
         buf,
         "$N's surname is now %s, but not accepted.",
         arg2
      );
      act(buf, ch, NULL, victim, TO_CHAR);
   }
   else
   {
      act("$N's surname cleared.", ch, NULL, victim, TO_CHAR);
   }
}

void do_suraccept(CHAR_DATA* ch, char* argument)
{
   CHAR_DATA* victim;
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];

   argument = one_argument_cs(argument, arg1);
   argument = one_argument_cs(argument, arg2);
   smash_tilde(arg2);
   if
   (
      (
         victim = get_char_world(ch, arg1)
      ) == NULL
   )
   {
      send_to_char("They are not here.\n\r", ch);
      return;
   }
   if (IS_NPC(victim))  /* sanity check */
   {
      send_to_char("You may only resurname players.\n\r", ch);
      return;
   }
   if
   (
      ch != victim &&
      get_trust(victim) >= get_trust(ch)
   )
   {
      act("You cannot mess with $N.", ch, NULL, victim, TO_CHAR);
      return;
   }
   if (IS_SET(victim->comm2, COMM_SURNAME_ACCEPTED))
   {
      act
      (
         "$N's surname no longer accepted.",
         ch,
         NULL,
         victim,
         TO_CHAR
      );
      REMOVE_BIT(victim->comm2, COMM_SURNAME_ACCEPTED);
   }
   else if (victim->pcdata->surname[0] == '\0')
   {
      act
      (
         "$N's surname is blank, you cannot accept it.",
         ch,
         NULL,
         victim,
         TO_CHAR
      );
      return;
   }
   else
   {
      act
      (
         "$N's surname is now accepted.",
         ch,
         NULL,
         victim,
         TO_CHAR
      );
      SET_BIT(victim->comm2, COMM_SURNAME_ACCEPTED);
   }
}

void do_moniker(CHAR_DATA* ch, char* argument)
{
   CHAR_DATA* victim;
   char buf[MAX_INPUT_LENGTH];
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];

   if (IS_NPC(ch))
   {
      send_to_char("Not with an NPC\n\r", ch);
      return;
   }
   if
   (
      ch->house != HOUSE_ANCIENT &&
      !IS_SET(ch->act2, PLR_IS_ANCIENT) &&
      !IS_TRUSTED(ch, ANGEL)
   )
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }
   argument = one_argument_cs(argument, arg1);
   strcpy(arg2, argument);
   smash_tilde(arg2);
   if
   (
      (
         victim = get_char_world(ch, arg1)
      ) == NULL
   )
   {
      send_to_char("They are not here.\n\r", ch);
      return;
   }
   if (IS_NPC(victim))  /* sanity check */
   {
      send_to_char("Only players may have monikers.\n\r", ch);
      return;
   }
   if
   (
      !IS_IMMORTAL(ch) &&
      ch->pcdata->induct < 5
   )
   {
      if (ch != victim)
      {
         send_to_char
         (
            "Only Lord Ancient may pick a moniker for another.\n\r",
            ch
         );
         return;
      }
      if (victim->pcdata->moniker[0] != '\0')
      {
         send_to_char
         (
            "Only Lord Ancient may change your moniker for you.\n\r",
            ch
         );
         return;
      }
   }
   if (!IS_SET(victim->act2, PLR_IS_ANCIENT))
   {
      send_to_char("Only ancients may have monikers.\n\r", ch);
      return;
   }
   if
   (
      ch != victim &&
      IS_IMMORTAL(ch) &&
      get_trust(victim) >= get_trust(ch)
   )
   {
      act("You cannot mess with $N.", ch, NULL, victim, TO_CHAR);
      return;
   }
   free_string(victim->pcdata->moniker);
   victim->pcdata->moniker = str_dup(arg2);
   if (arg2[0] != '\0')
   {
      if (ch != victim)
      {
         sprintf
         (
            buf,
            "$N's moniker is now %s.",
            arg2
         );
         act(buf, ch, NULL, victim, TO_CHAR);
      }
      sprintf
      (
         buf,
         "Your moniker is now %s.\n\r",
         arg2
      );
      send_to_char(buf, victim);
   }
   else
   {
      if (ch != victim)
      {
         act("$N's moniker cleared.", ch, NULL, victim, TO_CHAR);
      }
      send_to_char("Your moniker has been cleared.\n\r", victim);
   }
}

void do_lawless(CHAR_DATA* ch, char* argument)
{
   CHAR_DATA* vch;
   bool imm;

   if
   (
      IS_NPC(ch) ||
      !(
         (
            ch->house == HOUSE_ENFORCER &&
            ch->pcdata->induct == 5
         ) ||
         IS_TRUSTED(ch, ANGEL)
      )
   )
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }
   if (house_down(ch, HOUSE_ENFORCER))
   {
      return;
   }

   if
   (
      !IS_IMMORTAL(ch) &&
      IS_SET(ch->in_room->extra_room_flags, ROOM_1212)
   )
   {
      send_to_char("You can't do that from here.\n\r", ch);
      sprintf(log_buf, "%s tried to use lawless from 1212.", ch->name);
      log_string(log_buf);
      return;
   }
   imm = IS_IMMORTAL(ch);
   for (vch = char_list; vch; vch = vch->next)
   {
      if
      (
         IS_NPC(vch) ||
         (
            IS_IMMORTAL(vch) &&
            !can_see(ch, vch)
         ) ||
         !is_name(argument, get_name(vch, ch))
      )
      {
         continue;
      }
      break;
   }

   if (vch == NULL)
   {
      send_to_char("That person is not here.\n\r", ch);
      return;
   }

   if (IS_NPC(vch))  /* Sanity check */
   {
      send_to_char("You can only flag other players.\n\r", ch);
      return;
   }

   if (oblivion_blink(ch, vch))
   {
      return;
   }
   if (IS_SET(vch->act2, PLR_LAWLESS))
   {
      REMOVE_BIT(vch->act2, PLR_LAWLESS);
      send_to_char
      (
         "You have regained the protection of the laws of Justicar.\n\r",
         vch
      );
      act
      (
         "You have restored protection of the law to $N.",
         ch,
         NULL,
         vch,
         TO_CHAR
      );
      return;
   }
   SET_BIT(vch->act2, PLR_LAWLESS);
   act
   (
      "You stripped $N's right to be protected by the law.",
      ch,
      NULL,
      vch,
      TO_CHAR
   );
   send_to_char
   (
      "You are no longer protected by the laws of Justicar.\n\r",
      vch
   );
   return;
}

void do_trace(CHAR_DATA* ch, char* argument)
{
   CHAR_DATA* vch;
   int chance;
   int lag;
   AFFECT_DATA* paf;
   ROOM_INDEX_DATA* room;
   ROOM_INDEX_DATA *pRoomIndex;

   if
   (
      !has_skill(ch, gsn_trace) ||
      (
         chance = get_skill(ch, gsn_trace)
      ) < 1
   )
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }
   if (house_down(ch, HOUSE_ENFORCER))
   {
      return;
   }
   if (is_affected(ch, gsn_trace))
   {
      send_to_char
      (
         "Your trainees are still tired from your previous search party.\n\r",
         ch
      );
      return;
   }

   for (vch = char_list; vch; vch = vch->next)
   {
      if
      (
         IS_NPC(vch) ||
         IS_IMMORTAL(vch) ||
         !is_name(argument, get_name(vch, ch))
      )
      {
         continue;
      }
      break;
   }
   if (vch == NULL)
   {
      send_to_char("Your trainees do not know who to search for.\n\r", ch);
      return;
   }
   if (IS_NPC(vch))  /* sanity check */
   {
      send_to_char("You can only conduct search parties for players.\n\r", ch);
      return;
   }
   if
   (
      !IS_SET(vch->act, PLR_CRIMINAL) &&
      !IS_SET(vch->act2, PLR_LAWLESS)
   )
   {
      send_to_char
      (
         "Conducting search parties for non-criminals would be a breach of"
         " protocol.\n\r",
         ch
      );
      return;
   }
   act
   (
      "You send out a search party for $N.",
      ch,
      NULL,
      vch,
      TO_CHAR
   );
   chance += (ch->level - vch->level) * 4;
   lag = skill_table[gsn_trace].beats;
   if (IS_SET(vch->act2, PLR_LAWLESS))
   {
      lag = lag * 3 / 4;
   }
   if (number_percent() > chance)
   {
      check_improve(ch, gsn_trace, FALSE, 1);
      if (!number_bits(2))
      {
         check_improve(ch, gsn_trace, FALSE, 1);
         lag *= 2;
         send_to_char("Your trainees have gotten lost.\n\r", ch);
      }
      WAIT_STATE(ch, lag);
      act
      (
         "Your trainees were unable to find $N.",
         ch,
         NULL,
         vch,
         TO_CHAR
      );
      return;
   }
   WAIT_STATE(ch, lag);
   check_improve(ch, gsn_trace, TRUE, 1);
   act
   (
      "Your trainees managed to track down $N!",
      ch,
      NULL,
      vch,
      TO_CHAR
   );
   if (!number_bits(4))
   {
      send_to_char
      (
         "You notice a young man bearing a Justicar uniform peering about.\n\r",
         vch
      );
   }

   if
   (
      number_percent() < MARAUDER_MISINFORM_CHANCE &&
      is_affected(vch, gsn_misinformation)
   )
   {
      if
      (
         (
            pRoomIndex = get_random_room(vch, RANDOM_NORMAL)
         ) == NULL
      )
      {
         bug("NULL room from get_random_room.", 0);
         return;
      }
   }
   else
   {
      pRoomIndex = vch->in_room;
   }

   room = ch->in_room;
   char_from_room(ch);
   char_to_room_1(ch, pRoomIndex, TO_ROOM_AT);
   do_observe(ch, "", LOOK_AUTO);
   char_from_room(ch);
   char_to_room_1(ch, room, TO_ROOM_AT);
   paf = new_affect();
   paf->duration = 2;
   paf->type = gsn_trace;
   paf->where = TO_AFFECTS;
   affect_to_char_1(ch, paf);
   free_affect(paf);
   return;
}

void do_pre_title(CHAR_DATA* ch, char* argument)
{
   CHAR_DATA *victim;
   char arg1[MAX_INPUT_LENGTH];

   if
   (
      IS_NPC(ch) ||
      ch->level < 52
   )
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }
   argument = one_argument_cs(argument, arg1);
   if (arg1[0] == '\0')
   {
      send_to_char( "Change who's pretitle to what?\n\r", ch );
      return;
   }

   if ((victim = get_char_world(ch, arg1)) == NULL)
   {
      send_to_char( "Player isn't on.\n\r", ch );
      return;
   }

   if (IS_NPC(victim))
   {
      send_to_char("Trying to change a mob's pretitle?\n\r", ch);
      return;
   }

   if
   (
      ch != victim &&
      get_trust(victim) >= get_trust(ch)
   )
   {
      send_to_char("You cannot do that to that person.\n\r", ch);
      return;
   }

   if (strlen(argument) > 20)
   {
      send_to_char("Pretitle too long.\n\r", ch);
      return;
   }
   smash_tilde(argument);
   if (argument[0] == '\0')
   {
      send_to_char("Pretitle cleared.\n\r", ch);
      free_string(victim->pcdata->pre_title);
      victim->pcdata->pre_title = &str_empty[0];
      return;
   }
   free_string(victim->pcdata->pre_title);
   victim->pcdata->pre_title = str_dup(argument);
   send_to_char("Pretitle set.\n\r", ch);
}

void do_hemote(CHAR_DATA* ch, char* argument)
{
   if (IS_NPC(ch))
   {
      return;
   }
   if (IS_SET(ch->act2, PLR_NO_HOUSE_EMOTE))
   {
      send_to_char
      (
         "You will be announced when logging on/off again.\n\r",
         ch
      );
      REMOVE_BIT(ch->act2, PLR_NO_HOUSE_EMOTE);
   }
   else
   {
      send_to_char
      (
         "You will no longer be announced when logging on/off.\n\r",
         ch
      );
      SET_BIT(ch->act2, PLR_NO_HOUSE_EMOTE);
   }
}
