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

static const char rcsid[] = "$Id: db2.c,v 1.26 2004/09/29 06:54:59 drinlinda Exp $";

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif

#include "merc.h"
#include "db.h"
#include "lookup.h"
#include "tables.h"
#include "recycle.h"


/* values for db2.c */
struct        social_type    social_table        [MAX_SOCIALS];
extern int flag_lookup args((const char *name, const struct flag_type *flag_table));
int        social_count;
extern   AREA_DATA*  area_last;

/* snarf a socials file */
void load_socials( FILE *fp)
{
   for ( ; ; )
   {
      struct social_type social;
      char *temp;
      /* clear social */
      social.char_no_arg = NULL;
      social.others_no_arg = NULL;
      social.char_found = NULL;
      social.others_found = NULL;
      social.vict_found = NULL;
      social.char_not_found = NULL;
      social.char_auto = NULL;
      social.others_auto = NULL;

      temp = fread_word(fp);
      if (!strcmp(temp, "#"))
      continue;

      if (!strcmp(temp, "#0"))
      return;  /* done */
#if defined(social_debug)
      else
      fprintf(stderr, "%s\n\r", temp);
#endif

      strcpy(social.name, temp);
      fread_to_eol(fp);

      temp = fread_string_eol(fp);
      if (!strcmp(temp, "$"))
      social.char_no_arg = NULL;
      else if (!strcmp(temp, "#"))
      {
         social_table[social_count] = social;
         social_count++;
         continue;
      }
      else
      social.char_no_arg = temp;

      temp = fread_string_eol(fp);
      if (!strcmp(temp, "$"))
      social.others_no_arg = NULL;
      else if (!strcmp(temp, "#"))
      {
         social_table[social_count] = social;
         social_count++;
         continue;
      }
      else
      social.others_no_arg = temp;

      temp = fread_string_eol(fp);
      if (!strcmp(temp, "$"))
      social.char_found = NULL;
      else if (!strcmp(temp, "#"))
      {
         social_table[social_count] = social;
         social_count++;
         continue;
      }
      else
      social.char_found = temp;

      temp = fread_string_eol(fp);
      if (!strcmp(temp, "$"))
      social.others_found = NULL;
      else if (!strcmp(temp, "#"))
      {
         social_table[social_count] = social;
         social_count++;
         continue;
      }
      else
      social.others_found = temp;

      temp = fread_string_eol(fp);
      if (!strcmp(temp, "$"))
      social.vict_found = NULL;
      else if (!strcmp(temp, "#"))
      {
         social_table[social_count] = social;
         social_count++;
         continue;
      }
      else
      social.vict_found = temp;

      temp = fread_string_eol(fp);
      if (!strcmp(temp, "$"))
      social.char_not_found = NULL;
      else if (!strcmp(temp, "#"))
      {
         social_table[social_count] = social;
         social_count++;
         continue;
      }
      else
      social.char_not_found = temp;

      temp = fread_string_eol(fp);
      if (!strcmp(temp, "$"))
      social.char_auto = NULL;
      else if (!strcmp(temp, "#"))
      {
         social_table[social_count] = social;
         social_count++;
         continue;
      }
      else
      social.char_auto = temp;

      temp = fread_string_eol(fp);
      if (!strcmp(temp, "$"))
      social.others_auto = NULL;
      else if (!strcmp(temp, "#"))
      {
         social_table[social_count] = social;
         social_count++;
         continue;
      }
      else
      social.others_auto = temp;

      social_table[social_count] = social;
      social_count++;
   }
   return;
}






/*
* Snarf a mob section.  new style
*/
void load_mobiles( FILE *fp )
{
   MOB_INDEX_DATA *pMobIndex;
   int sn;
   int percent;
   LIST_DATA* list;
   NODE_DATA* node;
   SKILL_MOD* mod;

   for ( ; ; )
   {
      sh_int vnum;
      char letter;
      int iHash;

      letter                          = fread_letter( fp );
      if ( letter != '#' )
      {
         bug( "Load_mobiles: # not found.", 0 );
         exit( 1 );
      }

      vnum                            = fread_number( fp );
      if ( vnum == 0 )
      break;

      fBootDb = FALSE;
      if ( get_mob_index( vnum ) != NULL )
      {
         bug( "Load_mobiles: vnum %d duplicated.", vnum );
         exit( 1 );
      }
      fBootDb = TRUE;

      pMobIndex                       = alloc_perm( sizeof(*pMobIndex) );
      pMobIndex->vnum                 = vnum;
      pMobIndex->next_in_area = area_last->mobs;
      area_last->mobs = pMobIndex;
      pMobIndex->new_format        = TRUE;
      newmobs++;
      pMobIndex->player_name          = fread_string( fp );
      pMobIndex->short_descr          = fread_string( fp );
      pMobIndex->long_descr           = fread_string( fp );
      pMobIndex->description          = fread_string( fp );
      pMobIndex->race             = race_lookup(fread_string( fp ));

      pMobIndex->long_descr[0]        = UPPER(pMobIndex->long_descr[0]);
      pMobIndex->description[0]       = UPPER(pMobIndex->description[0]);

      pMobIndex->act                  = fread_flag( fp ) | ACT_IS_NPC
      | race_table[pMobIndex->race].act;
      pMobIndex->act2            = 0;
      pMobIndex->affected_by          = fread_flag( fp )
      | race_table[pMobIndex->race].aff;
      pMobIndex->pShop                = NULL;
      pMobIndex->alignment            = fread_number( fp );
      pMobIndex->group                = fread_number( fp );

      pMobIndex->level                = fread_number( fp );
      pMobIndex->hitroll              = fread_number( fp );

      /* read hit dice */
      pMobIndex->hit[DICE_NUMBER]     = fread_number( fp );
      /* 'd'          */                fread_letter( fp );
      pMobIndex->hit[DICE_TYPE]       = fread_number( fp );
      /* '+'          */                fread_letter( fp );
      pMobIndex->hit[DICE_BONUS]      = fread_number( fp );

      /* read mana dice */
      pMobIndex->mana[DICE_NUMBER]    = fread_number( fp );
      fread_letter( fp );
      pMobIndex->mana[DICE_TYPE]    = fread_number( fp );
      fread_letter( fp );
      pMobIndex->mana[DICE_BONUS]    = fread_number( fp );

      /* read damage dice */
      pMobIndex->damage[DICE_NUMBER]    = fread_number( fp );
      fread_letter( fp );
      pMobIndex->damage[DICE_TYPE]    = fread_number( fp );
      fread_letter( fp );
      pMobIndex->damage[DICE_BONUS]    = fread_number( fp );
      pMobIndex->dam_type        = attack_lookup(fread_word(fp));

      /* read armor class */
      pMobIndex->ac[AC_PIERCE]    = fread_number( fp ) * 10;
      pMobIndex->ac[AC_BASH]        = fread_number( fp ) * 10;
      pMobIndex->ac[AC_SLASH]        = fread_number( fp ) * 10;
      pMobIndex->ac[AC_EXOTIC]    = fread_number( fp ) * 10;

      /* read flags and add in data from the race table */
      pMobIndex->off_flags        = fread_flag( fp )
      | race_table[pMobIndex->race].off;
      pMobIndex->imm_flags        = fread_flag( fp )
      | race_table[pMobIndex->race].imm;
      pMobIndex->res_flags        = fread_flag( fp )
      | race_table[pMobIndex->race].res;
      pMobIndex->vuln_flags        = fread_flag( fp )
      | race_table[pMobIndex->race].vuln;

      /* vital statistics */
      pMobIndex->start_pos        = position_lookup(fread_word(fp));
      pMobIndex->default_pos        = position_lookup(fread_word(fp));
      pMobIndex->sex            = sex_lookup(fread_word(fp));

      pMobIndex->wealth        = fread_number( fp );

      pMobIndex->form            = fread_flag( fp )
      | race_table[pMobIndex->race].form;
      pMobIndex->parts        = fread_flag( fp )
      | race_table[pMobIndex->race].parts;
      /* size */
      pMobIndex->size            = size_lookup(fread_word(fp));
      pMobIndex->material        = str_dup(fread_word( fp ));

      for ( ; ; )
      {
         letter = fread_letter( fp );

         if (letter == 'F')
         {
            char *word;
            long vector;

            word                    = fread_word(fp);
            vector            = fread_flag(fp);

            if (!str_prefix(word, "act"))
            REMOVE_BIT(pMobIndex->act, vector);
            else if (!str_prefix(word, "aff"))
            REMOVE_BIT(pMobIndex->affected_by, vector);
            else if (!str_prefix(word, "off"))
            REMOVE_BIT(pMobIndex->off_flags, vector);
            else if (!str_prefix(word, "imm"))
            REMOVE_BIT(pMobIndex->imm_flags, vector);
            else if (!str_prefix(word, "res"))
            REMOVE_BIT(pMobIndex->res_flags, vector);
            else if (!str_prefix(word, "vul"))
            REMOVE_BIT(pMobIndex->vuln_flags, vector);
            else if (!str_prefix(word, "for"))
            REMOVE_BIT(pMobIndex->form, vector);
            else if (!str_prefix(word, "par"))
            REMOVE_BIT(pMobIndex->parts, vector);
            else
            {
               bug("Flag remove: flag not found.", 0);
               exit(1);
            }
         }
         else if ( letter == 'M' )
         {
            MPROG_LIST *pMprog;
            char *word;
            int trigger = 0;

            pMprog              = alloc_perm(sizeof(*pMprog));
            word                = fread_word( fp );
            if ( !(trigger = flag_lookup( word, mprog_flags )) )
            {
               bug("MOBprogs: invalid trigger.", 0);
               exit(1);
            }
            SET_BIT( pMobIndex->mprog_flags, trigger );
            pMprog->trig_type   = trigger;
            pMprog->vnum        = fread_number( fp );
            pMprog->trig_phrase = fread_string( fp );
            pMprog->next        = pMobIndex->mprogs;
            pMobIndex->mprogs   = pMprog;
         }
         else if ( letter == 'A' )
         {
            pMobIndex->act2        = fread_flag( fp );
         }
         else if ( letter == 'B' )
         {
            pMobIndex->affected_by2 = fread_flag( fp );
         }
         else if ( letter == 'S' )
         {
            char* name = fread_word(fp);
            char* temp;

            if (!str_cmp(name, "all"))
            {
               sn = SKILL_MOD_ALL;
            }
            else
            {
               sn = skill_lookup(name);
               if
               (
                  sn < 0 ||
                  sn >= MAX_SKILL
               )
               {
                  sprintf
                  (
                     log_buf,
                     "Mob [%d] has invalid modskill\n"
                     "Skill [%s] does not exist!",
                     vnum,
                     name
                  );
                  bug(log_buf, 0);
                  exit(1);
               }
            }
            /* sn is valid */
            temp = fread_word(fp);
            if (!is_number(temp))
            {
               sprintf
               (
                  log_buf,
                  "Mob [%d] has invalid modskill percent\n"
                  "Skill [%s], [%s] is not a number!",
                  vnum,
                  name,
                  temp
               );
               bug(log_buf, 0);
               exit(1);
            }
            percent = atoi(temp);
            if
            (
               percent < 0 ||
               percent > 100
            )
            {
               sprintf
               (
                  log_buf,
                  "Mob [%d] has invalid modskill percent\n"
                  "Skill [%s], [%d] is out of range!",
                  vnum,
                  name,
                  percent
               );
               bug(log_buf, 0);
               exit(1);
            }
            if (!pMobIndex->skill_mods)
            {
               pMobIndex->skill_mods = new_list();
            }
            else
            {
               /* Check for duplicate mod skills */
               for
               (
                  node = pMobIndex->skill_mods->first;
                  node;
                  node = node->next
               )
               {
                  mod = (SKILL_MOD*)node->data;
                  if (mod->sn == sn)
                  {
                     sprintf
                     (
                        log_buf,
                        "Mob [%d] has duplicate modskill for skill [%s]\n",
                        vnum,
                        name
                     );
                     bug(log_buf, 0);
                     exit(1);
                  }
               }
            }
            list = pMobIndex->skill_mods;
            mod = new_skill_mod(percent, sn);
            add_node(mod, list);
         }
         else
         {
            ungetc(letter, fp);
            break;
         }
      }

      iHash                   = vnum % MAX_KEY_HASH;
      pMobIndex->next         = mob_index_hash[iHash];
      mob_index_hash[iHash]   = pMobIndex;
      top_mob_index++;
      kill_table[URANGE(0, pMobIndex->level, MAX_LEVEL-1)].number++;
   }

   return;
}

void load_modified_book(OBJ_INDEX_DATA* index)
{
   FILE* book;
   char  strsave[MAX_INPUT_LENGTH];
   int   page;
   char* title;
   char* chapter;

   sprintf
   (
      strsave,
      "%s%s%d%s",
      BOOK_DIR,
      BOOK_FILE_PRE,
      index->vnum,
      BOOK_FILE_SUF
   );
   if
   (
      (
         book = fopen(strsave, "r")
      ) == NULL
   )
   {
      /* Never modified, this is fine */
      return;
   }
   for ( ; ; )
   {
      page = fread_number(book);
      /* End */
      if (page == -1)
      {
         fclose(book);
         return;
      }
      if
      (
         page < 1 ||
         page > BOOK_MAX_PAGES
      )
      {
         sprintf
         (
            log_buf,
            "Load_modified_book: Bad page number %d Book: [%d].",
            page,
            index->vnum
         );
         bug(log_buf, 0);
         exit(1);
      }
      if (page > index->value[0])
      {
         sprintf
         (
            log_buf,
            "Load_modified_book: Too many pages: %d. Book: [%d].",
            page,
            index->vnum
         );
         bug(log_buf, 0);
         exit(1);
      }
      /* first is 0, last is BOOK_MAX_PAGES - 1 */
      page--;
      title                               = fread_string(book);
      chapter                             = fread_string(book);
      if
      (
         str_cmp(title, index->book_info->title[page]) ||
         str_cmp(chapter, index->book_info->page[page])
      )
      {
         /* fread strings don't really get duplicated, no memory leak */
         index->book_info->title[page]   = str_dup(title);
         index->book_info->page[page]    = str_dup(chapter);
         index->book_info->book_save_info->modified[page] = TRUE;
      }
   }
}

/*
* Snarf an obj section. new style
*/
void load_objects( FILE *fp )
{
   OBJ_INDEX_DATA *pObjIndex;

   for ( ; ; )
   {
      sh_int vnum;
      char letter;
      int iHash;

      letter                          = fread_letter( fp );
      if ( letter != '#' )
      {
         bug( "Load_objects: # not found.", 0 );
         exit( 1 );
      }

      vnum                            = fread_number( fp );
      if ( vnum == 0 )
      break;

      fBootDb = FALSE;
      if ( get_obj_index( vnum ) != NULL )
      {
         bug( "Load_objects: vnum %d duplicated.", vnum );
         exit( 1 );
      }
      fBootDb = TRUE;

      pObjIndex                       = alloc_perm( sizeof(*pObjIndex) );
      pObjIndex->vnum                 = vnum;
      pObjIndex->next_in_area = area_last->objects;
      area_last->objects = pObjIndex;
      pObjIndex->new_format           = TRUE;
      pObjIndex->reset_num        = 0;
      newobjs++;
      pObjIndex->limcount        = 0;
      pObjIndex->limtotal        = 0;
      pObjIndex->in_game_count   = 0;
      pObjIndex->name                 = fread_string( fp );
      pObjIndex->short_descr          = fread_string( fp );
      pObjIndex->description          = fread_string( fp );
      pObjIndex->material        = fread_string( fp );

      pObjIndex->item_type            = item_lookup(fread_word( fp ));
      pObjIndex->extra_flags          = fread_flag( fp );
      pObjIndex->wear_flags           = fread_flag( fp );
      pObjIndex->extra_flags2         = 0;
      if (IS_SET(pObjIndex->wear_flags, ITEM_WEAR_FLOAT))
      {
         REMOVE_BIT(pObjIndex->wear_flags, ITEM_WEAR_FLOAT);
         if
         (
            !IS_SET(pObjIndex->wear_flags, ITEM_WIELD) &&
            pObjIndex->item_type != ITEM_WEAPON
         )
         {
            SET_BIT(pObjIndex->wear_flags, ITEM_HOLD);
         }
      }

      switch(pObjIndex->item_type)
      {
         case ITEM_WEAPON:
         pObjIndex->value[0]        = weapon_type(fread_word(fp));
         pObjIndex->value[1]        = fread_number(fp);
         pObjIndex->value[2]        = fread_number(fp);
         pObjIndex->value[3]        = attack_lookup(fread_word(fp));
         pObjIndex->value[4]        = fread_flag(fp);
         if (pObjIndex->value[0] == WEAPON_BOW){
            SET_BIT(pObjIndex->value[4], WEAPON_TWO_HANDS);
         }
         break;
         case ITEM_CONTAINER:
         pObjIndex->value[0]        = fread_number(fp);
         pObjIndex->value[1]        = fread_flag(fp);
         pObjIndex->value[2]        = fread_number(fp);
         pObjIndex->value[3]        = fread_number(fp);
         pObjIndex->value[4]        = fread_number(fp);
         break;
         case ITEM_DRINK_CON:
         case ITEM_FOUNTAIN:
         pObjIndex->value[0]         = fread_number(fp);
         pObjIndex->value[1]         = fread_number(fp);
         pObjIndex->value[2]         = liq_lookup(fread_word(fp));
         pObjIndex->value[3]         = fread_number(fp);
         pObjIndex->value[4]         = fread_number(fp);
         break;
         case ITEM_WAND:
         case ITEM_STAFF:
         pObjIndex->value[0]        = fread_number(fp);
         pObjIndex->value[1]        = fread_number(fp);
         pObjIndex->value[2]        = fread_number(fp);
         pObjIndex->value[3]        = spell_match(fread_word(fp));
         pObjIndex->value[4]        = fread_number(fp);
         break;
         case (ITEM_POTION):
         case (ITEM_HERB):
         case (ITEM_PILL):
         case (ITEM_SCROLL):
         {
            pObjIndex->value[0]        = fread_number(fp);
            pObjIndex->value[1]        = spell_match(fread_word(fp));
            pObjIndex->value[2]        = spell_match(fread_word(fp));
            pObjIndex->value[3]        = spell_match(fread_word(fp));
            pObjIndex->value[4]        = spell_match(fread_word(fp));
            break;
         }
         case ITEM_PORTAL_NEW:
         pObjIndex->value[0]         = fread_number(fp);
         pObjIndex->value[1]         = fread_number(fp);
         pObjIndex->value[2]         = fread_number(fp);
         pObjIndex->value[3]         = fread_number(fp);
         pObjIndex->value[4]         = fread_flag(fp);
         break;
         case ITEM_SPELLBOOK:
         pObjIndex->value[0]        = fread_number(fp);
         pObjIndex->value[1]        = fread_number(fp);
         pObjIndex->value[2]        = spell_match(fread_word(fp));
         pObjIndex->value[3]        = spell_match(fread_word(fp));
         pObjIndex->value[4]        = spell_match(fread_word(fp));
         break;
         case (ITEM_INKWELL):
         case (ITEM_PEN):
         {
            pObjIndex->value[0]        = pencolor_lookup(fread_word(fp));
            pObjIndex->value[1]        = fread_number(fp);
            pObjIndex->value[2]        = inkwell_lookup(fread_word(fp));
            pObjIndex->value[3]        = fread_number(fp);
            pObjIndex->value[4]        = fread_flag(fp);
            break;
         }
         case (ITEM_CORPSE_NPC):
         case (ITEM_CORPSE_PC):
         {
            char* word = fread_word(fp);

            if (is_number(word))
            {
               pObjIndex->value[0] = 0;
            }
            else
            {
               pObjIndex->value[0] = race_lookup(word);
            }
            pObjIndex->value[1]     = fread_flag(fp);
            pObjIndex->value[2]     = fread_flag(fp);
            pObjIndex->value[3]     = fread_flag(fp);
            pObjIndex->value[4]     = fread_flag(fp);
            break;
         }
         default:
         pObjIndex->value[0]             = fread_flag( fp );
         pObjIndex->value[1]             = fread_flag( fp );
         pObjIndex->value[2]             = fread_flag( fp );
         pObjIndex->value[3]             = fread_flag( fp );
         pObjIndex->value[4]            = fread_flag( fp );
         break;
      }
      pObjIndex->level        = fread_number( fp );
      pObjIndex->weight               = fread_number( fp );
      pObjIndex->cost                 = fread_number( fp );
      pObjIndex->mount_specs        = NULL;

      /* condition */
      letter                 = fread_letter( fp );
      switch (letter)
      {
         case ('P') :        pObjIndex->condition = 100; break;
         case ('G') :        pObjIndex->condition =  90; break;
         case ('A') :        pObjIndex->condition =  75; break;
         case ('W') :        pObjIndex->condition =  50; break;
         case ('D') :        pObjIndex->condition =  25; break;
         case ('B') :        pObjIndex->condition =  10; break;
         case ('R') :        pObjIndex->condition =   0; break;
         case ('X') :                pObjIndex->condition = fread_number( fp );
         pObjIndex->quitouts = FALSE;
         if (pObjIndex->condition < 0) pObjIndex->quitouts = TRUE;
         if (pObjIndex->condition > 0) pObjIndex->condition = (-1)*pObjIndex->condition;
         break;
         default:            pObjIndex->condition = 100; break;
      }

      if (pObjIndex->item_type == ITEM_BOOK)
      {
         pObjIndex->book_info = new_bookdata();
      }
      else
      {
         pObjIndex->book_info = NULL;
      }
      for ( ; ; )
      {
         char letter;

         letter = fread_letter(fp);

         if (letter == 'B')
         {
            pObjIndex->extra_flags2 |= fread_flag(fp);
         }
         else if (letter == 'A')
         {
            AFFECT_DATA *paf;

            paf                     = alloc_perm( sizeof(*paf) );
            paf->where              = TO_OBJECT;
            paf->type               = gsn_reserved;
            paf->level              = pObjIndex->level;
            paf->duration           = -1;
            paf->location           = fread_number(fp);
            paf->modifier           = fread_number(fp);
            paf->bitvector          = 0;
            paf->next               = pObjIndex->affected;
            pObjIndex->affected     = paf;
            top_affect++;
         }
         else if (letter == 'H')
         {
            pObjIndex->house = fread_number(fp);
         }
         else if (letter == 'G')
         {
            /* Wervdon 6-20-2004
             * For moongates
             * If we add moons, we'll need to update this to load more
             * moon phases and possibly be backwards compatible with
             * older moongates that only have a setup for 3 phases
             */
            pObjIndex->moonphases[0]        = fread_number(fp);
            pObjIndex->moonphases[1]        = fread_number(fp);
            pObjIndex->moonphases[2]        = fread_number(fp);
         }
         else if (letter == 'F')
         {
            AFFECT_DATA *paf;

            paf                  = alloc_perm(sizeof(*paf));
            letter               = fread_letter(fp);
            switch (letter)
            {
               case 'A':
               paf->where       = TO_AFFECTS;
               break;
               case 'I':
               paf->where        = TO_IMMUNE;
               break;
               case 'R':
               paf->where        = TO_RESIST;
               break;
               case 'V':
               paf->where        = TO_VULN;
               break;
               default:
               bug("Load_objects: Bad where on flag set.", 0);
               exit(1);
            }
            paf->type               = gsn_reserved;
            paf->level              = pObjIndex->level;
            paf->duration           = -1;
            paf->location           = fread_number(fp);
            paf->modifier           = fread_number(fp);
            paf->bitvector          = fread_flag(fp);
            paf->next               = pObjIndex->affected;
            pObjIndex->affected     = paf;
            top_affect++;
         }
         else if (letter == 'E')
         {
            EXTRA_DESCR_DATA *ed;

            ed                      = alloc_perm( sizeof(*ed) );
            ed->keyword             = fread_string(fp);
            ed->description         = fread_string(fp);
            ed->next                = pObjIndex->extra_descr;
            pObjIndex->extra_descr  = ed;
            top_ed++;
         }
         else if (letter == 'R')
         {
            pObjIndex->restrict_flags = fread_flag(fp);
         }
         else if (letter == 'L')
         {
            pObjIndex->limtotal = fread_number(fp);
            if (pObjIndex->limtotal > 15)
            pObjIndex->limtotal *= -1.15;
         }
         else if (letter == 'M')
         {
            MOUNT_DATA *specs;

            if ( !pObjIndex->mount_specs )
            {
               specs        = alloc_perm( sizeof(*specs) );
            } else
            {
               specs        = pObjIndex->mount_specs;
            }

            specs->move        = fread_number(fp);
            specs->mobility        = fread_number(fp);
            specs->move_flags    = fread_flag(fp);
            specs->mount_info_flags = 0;
            pObjIndex->mount_specs    = specs;
         }
         else if (letter == 'P')
         {
            int page;

            if (pObjIndex->item_type != ITEM_BOOK)
            {
               bug
               (
                  "Load_objects: Loading page on non-book! Type %d.",
                  pObjIndex->item_type
               );
               exit(1);
            }
            page = fread_number(fp);
            if
            (
               page < 1 ||
               page > BOOK_MAX_PAGES
            )
            {
               bug("Load_objects: Bad page number %d on load page.", page);
               exit(1);
            }
            if (page > pObjIndex->value[0])
            {
               bug("Load_objects: Too many pages: %d.", page);
               exit(1);
            }
            /* first is 0, last is BOOK_MAX_PAGES - 1 */
            page--;
            pObjIndex->book_info->title[page]   = fread_string(fp);
            pObjIndex->book_info->page[page]    = fread_string(fp);
         }
         else
         {
            ungetc( letter, fp );
            break;
         }
      }
      if (pObjIndex->item_type == ITEM_BOOK)
      {
         int cnt;

         for (cnt = 0; cnt < BOOK_MAX_PAGES; cnt++)
         {
            if (pObjIndex->book_info->title[cnt] == NULL)
            {
               pObjIndex->book_info->title[cnt] = &str_empty[0];
            }
            if (pObjIndex->book_info->page[cnt] == NULL)
            {
               pObjIndex->book_info->page[cnt] = &str_empty[0];
            }
         }
         /* number of pages */
         if
         (
            pObjIndex->value[0] < 1 ||
            pObjIndex->value[0] > BOOK_MAX_PAGES
         )
         {
            bug("Load_objects: invalid # of pages: %d", pObjIndex->value[0]);
            exit(1);
         }
         if
         (
            pObjIndex->value[1] < 0 ||
            pObjIndex->value[1] > 4
         )
         {
            bug("Load_objects: invalid length of page: %d", pObjIndex->value[1]);
            exit(1);
         }
         if
         (
            pObjIndex->value[2] < -1 ||
            (
               pObjIndex->value[2] > 0 &&
               get_obj_index(pObjIndex->value[2]) == NULL
            )
         )
         {
            bug("Load_objects: invalid key: %d", pObjIndex->value[2]);
            exit(1);
         }
         if
         (
            pObjIndex->value[3] < BOOK_LOCKED ||
            pObjIndex->value[3] >= BOOK_MAX_PAGES
         )
         {
            bug("Load_objects: invalid starting page: %d", pObjIndex->value[3]);
            exit(1);
         }
         if
         (
            pObjIndex->value[3] == BOOK_LOCKED &&
            pObjIndex->value[2] == -1
         )
         {
            bug("Load_objects: book locked with no key: %d", pObjIndex->vnum);
            exit(1);
         }
         if (IS_SET(pObjIndex->value[4], BOOK_SAVE))
         {
            /* Limit is 1 hardcoded */
            pObjIndex->limtotal = 1;
            /* Must have write set. */
            if (!IS_SET(pObjIndex->value[4], BOOK_WRITABLE))
            {
               bug
               (
                  "Load_objects: book has save flag but not writable: %d",
                  pObjIndex->vnum
               );
               exit(1);
            }
            pObjIndex->book_info->book_save_info = new_booksavedata();
            /* Load modified pages */
            load_modified_book(pObjIndex);
         }
      }

      iHash                   = vnum % MAX_KEY_HASH;
      pObjIndex->next         = obj_index_hash[iHash];
      obj_index_hash[iHash]   = pObjIndex;
      top_obj_index++;
   }

   return;
}

