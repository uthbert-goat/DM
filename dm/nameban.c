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

/**************************************************
* Copied from ban.c by Xurinos for illegal names. *
***************************************************/

static const char rcsid[] = "$Id: nameban.c,v 1.7 2004/09/09 03:42:35 xurinos Exp $";

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

NAMEBAN_DATA *nameban_list;

void save_namebans(void)
{
   NAMEBAN_DATA *pnameban;
   FILE *fp;
   bool found = FALSE;

   fclose( fpReserve );
   if ( ( fp = fopen( NAMEBAN_FILE, "w" ) ) == NULL )
   {
      perror( NAMEBAN_FILE );
   }

   for (pnameban = nameban_list; pnameban != NULL; pnameban = pnameban->next)
   {
      if (IS_SET(pnameban->nameban_flags, BAN_PERMANENT))
      {
         found = TRUE;
         fprintf(fp, "%-20s %-2d %s\n", pnameban->name, pnameban->level,
         print_flags(pnameban->nameban_flags));
      }
   }

   fclose(fp);
   fpReserve = fopen( NULL_FILE, "r" );
   if (!found)
   remove(NAMEBAN_FILE);
}

void load_namebans(void)
{
   FILE *fp;
   NAMEBAN_DATA *nameban_last;

   if ( ( fp = fopen( NAMEBAN_FILE, "r" ) ) == NULL )
   return;

   nameban_last = NULL;
   for ( ; ; )
   {
      NAMEBAN_DATA *pnameban;
      if ( feof(fp) )
      {
         fclose( fp );
         return;
      }

      pnameban = new_nameban();

      pnameban->name = str_dup(fread_word(fp));
      pnameban->level = fread_number(fp);
      pnameban->nameban_flags = fread_flag(fp);
      fread_to_eol(fp);

      if (nameban_list == NULL)
      nameban_list = pnameban;
      else
      nameban_last->next = pnameban;
      nameban_last = pnameban;
   }
}

bool check_nameban(char *site, int type)
{
   NAMEBAN_DATA *pnameban;
   char host[MAX_STRING_LENGTH];

   strcpy(host, capitalize(site));
   host[0] = LOWER(host[0]);

   for ( pnameban = nameban_list; pnameban != NULL; pnameban = pnameban->next )
   {
      if (!IS_SET(pnameban->nameban_flags, type))
      continue;

      if (IS_SET(pnameban->nameban_flags, BAN_PREFIX)
      &&  IS_SET(pnameban->nameban_flags, BAN_SUFFIX)
      /*      &&  strstr(pnameban->name, host) != NULL)*/
      &&  strstr(host, pnameban->name) != NULL)
      return TRUE;

      if (IS_SET(pnameban->nameban_flags, BAN_PREFIX)
      &&  !str_suffix(pnameban->name, host))
      return TRUE;

      if (IS_SET(pnameban->nameban_flags, BAN_SUFFIX)
      &&  !str_prefix(pnameban->name, host))
      return TRUE;

      if (!strcmp(host, pnameban->name))
      return TRUE;
   }

   return FALSE;
}


void nameban_site(CHAR_DATA *ch, char *argument, bool fPerm)
{
   char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];
   char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
   char *name;
   BUFFER *buffer;
   NAMEBAN_DATA *pnameban, *prev;
   bool prefix = FALSE, suffix = FALSE;
   int type;

   argument = one_argument(argument, arg1);
   argument = one_argument(argument, arg2);

   if ( arg1[0] == '\0' )
   {
      if (nameban_list == NULL)
      {
         send_to_char("No names banned at this time.\n\r", ch);
         return;
      }
      buffer = new_buf();

      add_buf(buffer, "Banned names  level  type     status\n\r");
      for (pnameban = nameban_list;pnameban != NULL;pnameban = pnameban->next)
      {
         sprintf(buf2, "%s%s%s",
         IS_SET(pnameban->nameban_flags, BAN_PREFIX) ? "*" : "",
         pnameban->name,
         IS_SET(pnameban->nameban_flags, BAN_SUFFIX) ? "*" : "");
         sprintf(buf, "%-12s    %-3d  %-7s  %s\n\r",
         buf2, pnameban->level,
         IS_SET(pnameban->nameban_flags, BAN_NEWBIES) ? "newbies" :
         IS_SET(pnameban->nameban_flags, BAN_PERMIT)  ? "permit"  :
         IS_SET(pnameban->nameban_flags, BAN_ALL)     ? "all"     : "",
         IS_SET(pnameban->nameban_flags, BAN_PERMANENT) ? "perm" : "temp");
         add_buf(buffer, buf);
      }

      page_to_char( buf_string(buffer), ch );
      free_buf(buffer);
      return;
   }

   /* find out what type of nameban */
   if (arg2[0] == '\0' || !str_prefix(arg2, "all"))
   type = BAN_ALL;
   else if (!str_prefix(arg2, "newbies"))
   type = BAN_NEWBIES;
   else if (!str_prefix(arg2, "permit"))
   type = BAN_PERMIT;
   else
   {
      send_to_char("Acceptable nameban types are all, newbies, and permit.\n\r",
      ch);
      return;
   }

   name = arg1;

   if (name[0] == '*')
   {
      prefix = TRUE;
      name++;
   }

   if (name[strlen(name) - 1] == '*')
   {
      suffix = TRUE;
      name[strlen(name) - 1] = '\0';
   }

   if (strlen(name) == 0)
   {
      send_to_char("You have to nameban SOMETHING.\n\r", ch);
      return;
   }

   prev = NULL;
   for ( pnameban = nameban_list; pnameban != NULL; prev = pnameban, pnameban = pnameban->next )
   {
      if (!str_cmp(name, pnameban->name))
      {
         if (pnameban->level > get_trust(ch))
         {
            send_to_char( "That nameban was set by a higher power.\n\r", ch );
            return;
         }
         else
         {
            if (prev == NULL)
            nameban_list = pnameban->next;
            else
            prev->next = pnameban->next;
            free_nameban(pnameban);
         }
      }
   }

   pnameban = new_nameban();
   pnameban->name = str_dup(name);
   pnameban->level = get_trust(ch);

   /* set nameban type */
   pnameban->nameban_flags = type;

   if (prefix)
   SET_BIT(pnameban->nameban_flags, BAN_PREFIX);
   if (suffix)
   SET_BIT(pnameban->nameban_flags, BAN_SUFFIX);
   if (fPerm)
   SET_BIT(pnameban->nameban_flags, BAN_PERMANENT);

   pnameban->next  = nameban_list;
   nameban_list    = pnameban;
   save_namebans();
   sprintf(buf, "%s has been namebanned.\n\r", pnameban->name);
   send_to_char( buf, ch );
   return;
}

void do_nameban(CHAR_DATA *ch, char *argument)
{
   nameban_site(ch, argument, FALSE);
}

void do_permnameban(CHAR_DATA *ch, char *argument)
{
   nameban_site(ch, argument, TRUE);
}

void do_nameallow( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   NAMEBAN_DATA *prev;
   NAMEBAN_DATA *curr;

   one_argument( argument, arg );

   if ( arg[0] == '\0' )
   {
      send_to_char( "Remove which name from the nameban list?\n\r", ch );
      return;
   }

   prev = NULL;
   for ( curr = nameban_list; curr != NULL; prev = curr, curr = curr->next )
   {
      if ( !str_cmp( arg, curr->name ) )
      {
         if (curr->level > get_trust(ch))
         {
            send_to_char(
            "You are not powerful enough to lift that nameban.\n\r", ch);
            return;
         }
         if ( prev == NULL )
         nameban_list   = nameban_list->next;
         else
         prev->next = curr->next;

         free_nameban(curr);
         sprintf(buf, "Nameban on %s lifted.\n\r", arg);
         send_to_char( buf, ch );
         save_namebans();
         return;
      }
   }

   send_to_char( "Name is not namebanned.\n\r", ch );
   return;
}


