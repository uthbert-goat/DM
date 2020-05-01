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

static const char rcsid[] = "$Id: lookup.c,v 1.6 2004/10/19 00:11:31 maelstrom Exp $";

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <time.h>
#include "merc.h"
#include "tables.h"

int flag_lookup (const char *name, const struct flag_type *flag_table)
{
   int flag;

   for (flag = 0; flag_table[flag].name != NULL; flag++)
   {
      if (LOWER(name[0]) == LOWER(flag_table[flag].name[0])
      &&  !str_prefix(name, flag_table[flag].name))
      return flag_table[flag].bit;
   }

   return 0;
}

int house_lookup(const char *name)
{
   int house;

   if (name[0] == '\0') return 0;
   for (house = 0; house < MAX_HOUSE; house++)
   {
      if (LOWER(name[0]) == LOWER(house_table[house].name[0])
      &&  !str_prefix(name, house_table[house].name))
      return house;
   }

   return 0;
}

int position_lookup (const char *name)
{
   int pos;

   for (pos = 0; position_table[pos].name != NULL; pos++)
   {
      if (LOWER(name[0]) == LOWER(position_table[pos].name[0])
      &&  !str_prefix(name, position_table[pos].name))
      return pos;
   }

   return -1;
}

int sex_lookup (const char *name)
{
   int sex;

   for (sex = 0; sex_table[sex].name != NULL; sex++)
   {
      if (LOWER(name[0]) == LOWER(sex_table[sex].name[0])
      &&  !str_prefix(name, sex_table[sex].name))
      return sex;
   }

   return -1;
}

int size_lookup (const char *name)
{
   int size;

   for ( size = 0; size_table[size].name != NULL; size++)
   {
      if (LOWER(name[0]) == LOWER(size_table[size].name[0])
      &&  !str_prefix( name, size_table[size].name))
      return size;
   }

   return -1;
}

int association_table_lookup(const char *name, const ASSOCIATION_TYPE *table)
{
   int count;

   if (name == NULL || table == NULL)
   {
      return -1;
   }

   for (count = 0; table[count].name != NULL; count++)
   {
      if (
            LOWER(name[0]) == LOWER(table[count].name[0]) &&
            !str_prefix(name, table[count].name)
         )
      {
         return table[count].type;
      }
   }

   return -1;
}

char* association_table_name(const int type, const ASSOCIATION_TYPE *table)
{
   int count;

   if (table == NULL)
   {
      return "none";
   }

   for (count = 0; table[count].name != NULL; count++)
   {
      if (type == table[count].type)
      {
         return item_table[type].name;
      }
   }

   return "none";
}
