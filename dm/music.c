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

static const char rcsid[] = "$Id: music.c,v 1.8 2004/09/08 02:46:14 xurinos Exp $";

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "merc.h"
#include "music.h"
#include "recycle.h"

void music_on(CHAR_DATA* ch)
{
   char* music_name;
   char  buf[MAX_STRING_LENGTH];
   char  download[MAX_INPUT_LENGTH];

   if
   (
      IS_NPC(ch) ||
      !IS_SET(ch->comm2, COMM_MUSIC) ||
      ch->in_room == NULL ||
      ch->pcdata->music_area == ch->in_room->area  /* Already playing */
   )
   {
      return;
   }
   music_name = ch->in_room->area->music;
   if (music_name == NULL)
   {
      music_off(ch);
      return;
   }
   ch->pcdata->music_area = ch->in_room->area;
   if (IS_SET(ch->comm2, COMM_MUSIC_DOWNLOAD))
   {
      sprintf
      (
         download,
         " U=%s%s.mid",
         MUSIC_DIR,
         music_name
      );
   }
   else
   {
      download[0] = '\0';
   }
   sprintf
   (
      buf,
      "!!MUSIC(%s.* V=100 "/*L=-1 */"C=1 T=music%s)\n\r",
      music_name,

      download
   );
   write_to_buffer(ch->desc, buf, 0);
}

void music_off(CHAR_DATA* ch)
{
   if
   (
      IS_NPC(ch) ||
      ch->pcdata->music_area == NULL  /* if null, music is off */
   )
   {
      return;
   }
   write_to_buffer(ch->desc, "!!MUSIC(off)\n\r", 0);
   ch->pcdata->music_area = NULL;
}
