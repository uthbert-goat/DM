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

static const char rcsid[] = "$Id: note.c,v 1.38 2004/10/25 06:35:22 fizzfaldt Exp $";

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "merc.h"
#include "recycle.h"
#include "tables.h"
#include "clan.h"
#include "worship.h"

/* globals from db.c for load_notes */
#if !defined(macintosh)
extern  int     _filbuf         args( (FILE *) );
#endif
extern FILE *                  fpArea;
extern char                    strArea[MAX_INPUT_LENGTH];

/* Time of last house/clan bank update */
time_t last_banknote_update;

/* local procedures */
bool search_string( char *str, char *namelist );
void cloak_note(CHAR_DATA *ch);
void load_thread(char *name, NOTE_DATA **list, int type, time_t free_time);
void parse_note(CHAR_DATA *ch, char *argument, int type);
bool hide_note(CHAR_DATA *ch, NOTE_DATA *pnote);
void hoard_report(CHAR_DATA *ch);

NOTE_DATA* note_list;
NOTE_DATA* idea_list;
NOTE_DATA* penalty_list;
NOTE_DATA* news_list;
NOTE_DATA* changes_list;
NOTE_DATA* hoarder_list;
NOTE_DATA* clan_list;
NOTE_DATA* bank_list;
NOTE_DATA* crim_list;
NOTE_DATA* death_list;

DECLARE_DO_FUN(    do_help    );

long get_note_id(void)
{
   if (current_time <= last_note_id)
   {
      last_note_id++;
   }
   else
   {
      last_note_id = current_time;
   }
   return last_note_id;
}

int count_spool(CHAR_DATA *ch, NOTE_DATA *spool)
{
   int count = 0;
   NOTE_DATA *pnote;

   for (pnote = spool; pnote != NULL; pnote = pnote->next)
   if (!hide_note(ch, pnote))
   count++;

   return count;
}

void do_unread(CHAR_DATA *ch)
{
   char buf[MAX_STRING_LENGTH];
   int count;
   bool found = FALSE;

   if (IS_NPC(ch))
   {
      return;
   }

   if ((count = count_spool(ch, news_list)) > 0)
   {
      found = TRUE;
      sprintf
      (
         buf,
         "There %s %d new news article%s waiting.\n\r",
         count > 1 ? "are" : "is",
         count,
         count > 1 ? "s" : ""
      );
      send_to_char(buf, ch);
   }
   if ((count = count_spool(ch, changes_list)) > 0)
   {
      found = TRUE;
      sprintf
      (
         buf,
         "There %s %d change%s waiting to be read.\n\r",
         count > 1 ? "are" : "is",
         count,
         count > 1 ? "s" : ""
      );
      send_to_char(buf, ch);
   }
   if ((count = count_spool(ch, note_list)) > 0)
   {
      found = TRUE;
      sprintf
      (
         buf,
         "You have %d new note%s waiting.\n\r",
         count,
         count > 1 ? "s" : ""
      );
      send_to_char(buf, ch);
   }
   if ((count = count_spool(ch, idea_list)) > 0)
   {
      found = TRUE;
      sprintf
      (
         buf,
         "You have %d unread idea%s to peruse.\n\r",
         count,
         count > 1 ? "s" : ""
      );
      send_to_char(buf, ch);
   }
   if
   (
      IS_IMMORTAL(ch) &&
      (count = count_spool(ch, penalty_list)) > 0
   )
   {
      found = TRUE;
      sprintf
      (
         buf,
         "%d penalt%s been added.\n\r",
         count,
         count > 1 ? "ies have" : "y has"
      );
      send_to_char(buf, ch);
   }
   if
   (
      IS_TRUSTED(ch, ANGEL) &&
      (count = count_spool(ch, clan_list)) > 0
   )
   {
      found = TRUE;
      sprintf
      (
         buf,
         "%d clan notice%s waiting for you.\n\r",
         count,
         count > 1 ? "s are" : " is"
      );
      send_to_char(buf, ch);
   }
   if ((count = count_spool(ch, bank_list)) > 0)
   {
      found = TRUE;
      sprintf
      (
         buf,
         "%d bank notice%s waiting for you.\n\r",
         count,
         count > 1 ? "s are" : " is"
      );
      send_to_char(buf, ch);
   }
   if ((count = count_spool(ch, death_list)) > 0)
   {
      found = TRUE;
      sprintf
      (
         buf,
         "%d ancient scroll%s waiting for you.\n\r",
         count,
         count > 1 ? "s are" : " is"
      );
      send_to_char(buf, ch);
   }
   if ((count = count_spool(ch, crim_list)) > 0)
   {
      found = TRUE;
      sprintf
      (
         buf,
         "%d criminal notice%s waiting for you.\n\r",
         count,
         count > 1 ? "s are" : " is"
      );
      send_to_char(buf, ch);
   }
   if (!found)
   {
      send_to_char("You have no unread notes.\n\r", ch);
   }
}

void do_note(CHAR_DATA* ch, char* argument)
{
   parse_note(ch, argument, NOTE_NOTE);
}

void do_idea(CHAR_DATA* ch, char* argument)
{
   parse_note(ch, argument, NOTE_IDEA);
}

void do_penalty(CHAR_DATA* ch, char* argument)
{
   parse_note(ch, argument, NOTE_PENALTY);
}

void do_hoarder(CHAR_DATA *ch, char *argument)
{
   parse_note(ch, argument, NOTE_HOARDER);
}

void do_helpnote(CHAR_DATA *ch, char *argument)
{
   if (!IS_IMMORTAL(ch))
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }
   parse_note(ch, argument, NOTE_HELP);
}

void do_news(CHAR_DATA* ch, char* argument)
{
   parse_note(ch, argument, NOTE_NEWS);
}

void do_changes(CHAR_DATA* ch, char* argument)
{
   parse_note(ch, argument, NOTE_CHANGES);
}

void do_clannote(CHAR_DATA *ch, char *argument)
{
   if (!IS_IMMORTAL(ch))
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }
   parse_note(ch, argument, NOTE_CLAN);
}

void do_deathnote(CHAR_DATA* ch, char* argument)
{
   parse_note(ch, argument, NOTE_DEATH);
}

void do_banknote(CHAR_DATA* ch, char* argument)
{
   parse_note(ch, argument, NOTE_BANK);
}

void do_crimnote(CHAR_DATA* ch, char* argument)
{
   parse_note(ch, argument, NOTE_CRIM);
}

void save_notes(int type)
{
   FILE *fp;
   char *name;
   NOTE_DATA *pnote;

   switch (type)
   {
      default:
      {
         return;
      }
      case (NOTE_NOTE):
      {
         name = NOTE_FILE;
         pnote = note_list;
         break;
      }
      case (NOTE_IDEA):
      {
         name = IDEA_FILE;
         pnote = idea_list;
         break;
      }
      case (NOTE_HELP):
      {
         name = HELP_FILE;
         pnote = help_list;
         break;
      }
      case (NOTE_HOARDER):
      {
         name = HOARDER_FILE;
         pnote = hoarder_list;
         break;
      }
      case (NOTE_PENALTY):
      {
         name = PENALTY_FILE;
         pnote = penalty_list;
         break;
      }
      case (NOTE_NEWS):
      {
         name = NEWS_FILE;
         pnote = news_list;
         break;
      }
      case (NOTE_CHANGES):
      {
         name = CHANGES_FILE;
         pnote = changes_list;
         break;
      }
      case (NOTE_BANK):
      {
         name = BANK_FILE;
         pnote = bank_list;
         break;
      }
      case (NOTE_CLAN):
      {
         name = CLAN_FILE;
         pnote = clan_list;
         break;
      }
      case (NOTE_DEATH):
      {
         name = DEATH_FILE;
         pnote = death_list;
         break;
      }
      case (NOTE_CRIM):
      {
         name = CRIM_FILE;
         pnote = crim_list;
         break;
      }
   }

   fclose( fpReserve );
   if ( ( fp = fopen( name, "w" ) ) == NULL )
   {
      perror( name );
   }
   else
   {
      for ( ; pnote != NULL; pnote = pnote->next )
      {
         fprintf( fp, "Sender  %s~\n", pnote->sender);
         fprintf( fp, "Date    %s~\n", pnote->date);
#if defined(MSDOS)
         fprintf(fp, "Stamp   %d\n", pnote->date_stamp);
         fprintf(fp, "Id      %d\n", pnote->id);
         fprintf(fp, "IdS     %d\n", pnote->id_sender);
#else
         fprintf(fp, "Stamp   %ld\n", pnote->date_stamp);
         fprintf(fp, "Id      %ld\n", pnote->id);
         fprintf(fp, "IdS     %ld\n", pnote->id_sender);
#endif
         fprintf( fp, "To      %s~\n", pnote->to_list);
         fprintf( fp, "Subject %s~\n", pnote->subject);
         fprintf( fp, "Text\n%s~\n",   pnote->text);
      }
      fclose( fp );
      fpReserve = fopen( NULL_FILE, "r" );
      return;
   }
}

void load_notes(void)
{
   log_string("Loading note.not");
   load_thread
   (
      NOTE_FILE,
      &note_list,
      NOTE_NOTE,
      5*24*60*60
   );
   update_time();
   log_string("Loading idea.not");
   load_thread
   (
      IDEA_FILE,
      &idea_list, NOTE_IDEA,
      3 * 28 * 24 * 60 * 60
   );
   update_time();
   log_string("Loading penal.not");
   load_thread
   (
      PENALTY_FILE,
      &penalty_list,
      NOTE_PENALTY,
      0
   );
   update_time();
   log_string("Loading hoarder.not");
   load_thread
   (
      HOARDER_FILE,
      &hoarder_list,
      NOTE_HOARDER,
      60 * 28 * 24 * 60 * 60
   );
   update_time();
   log_string("Loading help.not");
   load_thread
   (
      HELP_FILE,
      &help_list,
      NOTE_HELP,
      0
   );
   update_time();
   log_string("Loading news.not");
   load_thread
   (
      NEWS_FILE,
      &news_list,
      NOTE_NEWS,
      3 * 28 * 24 * 60 * 60
   );
   update_time();
   log_string("Loading changes.not");
   load_thread
   (
      CHANGES_FILE,
      &changes_list,
      NOTE_CHANGES,
      0
   );
   update_time();
   log_string("Loading criminal.not");
   load_thread
   (
      CRIM_FILE,
      &crim_list,
      NOTE_CRIM,
      3 * 28 * 24 * 60 * 60
   );
   update_time();
   log_string("Loading bank.not");
   load_thread
   (
      BANK_FILE,
      &bank_list,
      NOTE_BANK,
      3 * 28 * 24 * 60 * 60
   );
   update_time();
   log_string("Loading deaths");
   load_thread
   (
      DEATH_FILE,
      &death_list,
      NOTE_DEATH,
      6 * 28 * 24 * 60 * 60
   );
   update_time();
   log_string("Loading clans.not");
   load_thread
   (
      CLAN_FILE,
      &clan_list,
      NOTE_CLAN,
      3 * 28 * 24 * 60 * 60
   );
}

void load_thread(char* name, NOTE_DATA** list, int type, time_t free_time)
{
   FILE* fp;
   NOTE_DATA* pnotelast = NULL;
   bool imm_keep = (type == NOTE_IDEA || type == NOTE_NEWS);
   char* word;
   bool update = FALSE;

   if
   (
      (
         fp = fopen(name, "r")
      ) == NULL
   )
   {
      return;
   }

   for (;;)
   {
      NOTE_DATA* pnote;
      char letter;

      do
      {
         letter = getc(fp);
         if (feof(fp))
         {
            fclose(fp);
            if (update)
            {
               save_notes(type);
            }
            return;
         }
      }
         while (isspace(letter));

      ungetc(letter, fp);

      pnote = alloc_perm(sizeof(*pnote));

      if (str_cmp(fread_word(fp), "sender"))
      {
         break;
      }
      pnote->sender = fread_string(fp);

      if (str_cmp(fread_word(fp), "date"))
      {
         break;
      }
      pnote->date = fread_string(fp);

      if (str_cmp(fread_word(fp), "stamp"))
      {
         break;
      }
      pnote->date_stamp = fread_number(fp);

      word = fread_word(fp);

      if (str_cmp(word, "id"))
      {
         update = TRUE;
         pnote->id = pnote->date_stamp;
         if (str_cmp(word, "to"))
         {
            break;
         }
      }
      else
      {
         pnote->id = fread_number(fp);
         word = fread_word(fp);
         if (str_cmp(word, "IdS"))
         {
            pnote->id_sender = 0;
            if (str_cmp(word, "to"))
            {
               break;
            }
         }
         else
         {
            pnote->id_sender = fread_number(fp);
            if (str_cmp(fread_word(fp), "to"))
            {
               break;
            }
         }
      }
      pnote->to_list = fread_string( fp );

      if (str_cmp(fread_word(fp), "subject"))
      {
         break;
      }
      pnote->subject = fread_string(fp);

      if (str_cmp(fread_word(fp), "text"))
      {
         break;
      }
      pnote->text = fread_string(fp);
      mem_counts[NMEM_NOTES]++;

      if
      (
         free_time &&
         !(
            imm_keep &&
            (
               is_name("immortal", pnote->to_list) ||
               is_name("imm", pnote->to_list)
           )
        ) &&
         pnote->date_stamp < current_time - free_time
     )
      {
         update = TRUE;
         free_note(pnote);
         continue;
      }

      pnote->type = type;

      if (*list == NULL)
      {
         *list = pnote;
      }
      else
      {
         pnotelast->next = pnote;
      }

      pnotelast       = pnote;
   }

   strcpy(strArea, NOTE_FILE);
   fpArea = fp;
   bug("Load_notes: bad key word.", 0);
   exit(1);
   return;
}

void append_note(NOTE_DATA *pnote)
{
   FILE *fp;
   char *name;
   NOTE_DATA **list;
   NOTE_DATA *last;

   switch (pnote->type)
   {
      default:
      {
         return;
      }
      case (NOTE_NOTE):
      {
         name = NOTE_FILE;
         list = &note_list;
         break;
      }
      case (NOTE_IDEA):
      {
         name = IDEA_FILE;
         list = &idea_list;
         break;
      }
      case (NOTE_HELP):
      {
         name = HELP_FILE;
         list = &help_list;
         break;
      }
      case (NOTE_HOARDER):
      {
         name = HOARDER_FILE;
         list = &hoarder_list;
         break;
      }
      case (NOTE_PENALTY):
      {
         name = PENALTY_FILE;
         list = &penalty_list;
         break;
      }
      case (NOTE_NEWS):
      {
         name = NEWS_FILE;
         list = &news_list;
         break;
      }
      case (NOTE_CHANGES):
      {
         name = CHANGES_FILE;
         list = &changes_list;
         break;
      }
      case (NOTE_BANK):
      {
         name = BANK_FILE;
         list = &bank_list;
         break;
      }
      case (NOTE_CLAN):
      {
         name = CLAN_FILE;
         list = &clan_list;
         break;
      }
      case (NOTE_DEATH):
      {
         name = DEATH_FILE;
         list = &death_list;
         break;
      }
      case (NOTE_CRIM):
      {
         name = CRIM_FILE;
         list = &crim_list;
         break;
      }
   }

   if (*list == NULL)
   {
      *list = pnote;
   }
   else
   {
      for (last = *list; last->next != NULL; last = last->next);
      last->next = pnote;
   }
   pnote->id = get_note_id();
   fclose(fpReserve);
   if ((fp = fopen(name, "a")) == NULL)
   {
      perror(name);
   }
   else
   {
      fprintf(fp, "Sender  %s~\n", pnote->sender);
      fprintf(fp, "Date    %s~\n", pnote->date);
#if defined(MSDOS)
      fprintf(fp, "Stamp   %d\n", pnote->date_stamp);
      fprintf(fp, "Id      %d\n", pnote->id);
      fprintf(fp, "IdS     %d\n", pnote->id_sender);
#else
      fprintf(fp, "Stamp   %ld\n", pnote->date_stamp);
      fprintf(fp, "Id      %ld\n", pnote->id);
      fprintf(fp, "IdS     %ld\n", pnote->id_sender);
#endif
      fprintf(fp, "To      %s~\n", pnote->to_list);
      fprintf(fp, "Subject %s~\n", pnote->subject);
      fprintf(fp, "Text\n%s~\n", pnote->text);
      fclose(fp);
   }
   fpReserve = fopen( NULL_FILE, "r" );
}

bool is_note_to(CHAR_DATA* ch, NOTE_DATA* pnote)
{
   OBJ_DATA * brand = get_eq_char(ch, WEAR_BRAND);
   sh_int god;
   sh_int iclass;

   if (IS_NPC(ch))
   {
      return FALSE;
   }

   if
   (
      (
         pnote->id_sender &&
         ch->id == pnote->id_sender
      ) ||
      (
         !pnote->id_sender &&
         !str_cmp(ch->name, pnote->sender)
      )
   )
   {
      return TRUE;
   }

   if (is_name("all", pnote->to_list))
   {
      return TRUE;
   }

   /* law message board in my shrine -werv */
   if
   (
      ch->in_room != NULL &&
      ch->in_room->vnum == 12512 &&
      is_name("law", pnote->to_list)
   )
   {
      return TRUE;
   }

   if
   (
      brand != NULL &&
      (
         god = god_lookup(ch)
      ) != -1 &&
      brand->pIndexData->vnum == worship_table[god].vnum_brand &&
      worship_table[god].brand_info.note_bin != NULL &&
      is_name
      (
         worship_table[god].brand_info.note_bin,
         pnote->to_list
      )
   )
   {
      return TRUE;
   }

   if (IS_IMMORTAL(ch))
   {
      for (iclass = 0; iclass < MAX_CLASS; iclass++)
      {
         if (is_name(class_table[iclass].name, pnote->to_list))
         {
            return TRUE;
         }
      }
      for (iclass = 0; iclass < MAX_PC_RACE; iclass++)
      {
         if
         (
            is_name(race_table[iclass].name, pnote->to_list) &&
            (
               ch->id == ID_BUK ||
               iclass != grn_book
               /* Only fizzy gets these */
            )
         )
         {
            return TRUE;
         }
      }
   }
   else if
   (
      is_name(class_table[ch->class].name, pnote->to_list) ||
      is_name(race_table[ch->race].name, pnote->to_list)
   )
   {
      return TRUE;
   }
   if
   (
      IS_IMMORTAL(ch) &&
      (
         is_name("immortal", pnote->to_list) ||
         is_name("immortals", pnote->to_list) ||
         is_name("imms", pnote->to_list) ||
         is_name("imm", pnote->to_list)
      )
   )
   {
      return TRUE;
   }

   if
   (
      IS_IMMORTAL(ch) &&
      ch->house == HOUSE_ANCIENT &&
      is_name("immancient", pnote->to_list)
   )
   {
      return TRUE;
   }

   if
   (
      ch->house &&
      is_name(house_table[ch->house].name, pnote->to_list)
   )
   {
      return TRUE;
   }

   if
   (
      !IS_NPC(ch) &&
      ch->pcdata->clan != 0 &&
      clan_table[ch->pcdata->clan].valid &&
      is_name
      (
         clan_table[ch->pcdata->clan].who_name,
         pnote->to_list
      )
   )
   {
      return TRUE;
   }

   if
   (
      ch->house == HOUSE_ANCIENT &&
      (
         is_name("assassin", pnote->to_list) ||
         is_name("assassins", pnote->to_list) ||
         is_name("assassination", pnote->to_list)
      )
   )
   {
      return TRUE;
   }

   if (is_name(ch->name, pnote->to_list))
   {
      return TRUE;
   }

   if
   (
      IS_SET(ch->comm, COMM_BUILDER) &&
      is_name("builder", pnote->to_list)
   )
   {
      return TRUE;
   }
   if
   (
      ch->house == HOUSE_ANCIENT &&
      ch->pcdata->induct == 5 &&
      is_name("lordancient", pnote->to_list)
   )
   {
      return TRUE;
   }
   /*
   No senate
   if
   (
   (
   ch->house == HOUSE_EMPIRE &&
   (
   IS_IMMORTAL(ch) ||
   ch->pcdata->house_rank == 6 ||
   ch->pcdata->house_rank == 3 ||
   ch->pcdata->house_rank == 4 ||
   ch->pcdata->house_rank == 5
   )
   ) &&
   is_name("senate", pnote->to_list)
   )
   {
   return TRUE;
   }
   */
   if
   (
      !str_cmp("malignus", ch->name) &&
      is_name("godfather", pnote->to_list)
   )
   {
      return TRUE;
   }

   if
   (
      ch->pcdata->induct == 5 &&
      (
         is_name("leader", pnote->to_list) ||
         is_name("leaders", pnote->to_list)
      )
   )
   {
      return TRUE;
   }

   /* Coder Note addition by Wicket */
   if
   (
      IS_IMMORTAL(ch) &&
      (
         is_name("coder", pnote->to_list) ||
         is_name("coders", pnote->to_list)
      ) &&
      (
         !str_cmp("xurinos", ch->name) ||
         !str_cmp("styx", ch->name) ||
         !str_cmp("wervdon", ch->name) ||
         !str_cmp("jord", ch->name) ||
         !str_cmp("maelstrom", ch->name) ||
         !str_cmp("xyza", ch->name) ||
         !str_cmp("thrym", ch->name) ||
         !str_cmp("rungekutta", ch->name) ||
         !str_cmp("shamutanti", ch->name) ||
         !str_cmp("vrrin", ch->name) ||
         !str_cmp("restin", ch->name) ||
         !str_cmp("nixalis", ch->name) ||
         !str_cmp("drinlinda", ch->name) ||
         !str_cmp("drithentir", ch->name) ||
         !str_cmp("riallus", ch->name) ||
         !str_cmp("joja", ch->name) ||
         !str_cmp("cirdan", ch->name) ||
         !str_cmp("thaust", ch->name) ||
         !str_cmp("fizzfaldt", ch->name) ||
         !str_cmp("wicket", ch->name)
      )
   )
   {
      return TRUE;
   }
   /*RP note for the Order of Supremacy*/
   if
   (
      !str_cmp("jord", ch->name) &&
      is_name("supremacy", pnote->to_list)
   )
   {
      return TRUE;
   }
   if
   (
      ch->id == ID_BUK &&
      is_name("buk", pnote->to_list)
   )
   {
      return TRUE;
   }

   return FALSE;
}



void note_attach( CHAR_DATA *ch, int type )
{
   NOTE_DATA *pnote;
   char name[MAX_INPUT_LENGTH];

   if ( ch->pnote != NULL )
   return;

   pnote = new_note();

   pnote->next        = NULL;
   if ((is_affected(ch, gsn_cloak_form)) && (ch->pcdata->induct == 5) && (ch->house != HOUSE_OUTLAW))
   {
      if
      (
         !IS_NPC(ch) &&
         ch->pcdata->moniker[0] != '\0' &&
         !IS_IMMORTAL(ch)
      )
      {
         sprintf
         (
            name,
            "LordAncient (%s)",
            ch->pcdata->moniker
         );
         pnote->sender = str_dup(name);
      }
      else
      {
         pnote->sender = str_dup("LordAncient");
      }
   }
   else if (is_affected(ch, gsn_cloak_form))
   {
      if
      (
         !IS_NPC(ch) &&
         ch->pcdata->moniker[0] != '\0' &&
         !IS_IMMORTAL(ch)
      )
      {
         sprintf
         (
            name,
            "Cloaked Figure (%s)",
            ch->pcdata->moniker
         );
         pnote->sender = str_dup(name);
      }
      else
      {
         pnote->sender = str_dup("Cloaked Figure");
      }
   }
   else
   {
      pnote->sender    = str_dup(ch->name);
   }
   pnote->id_sender  = ch->id;
   pnote->date        = str_dup( "" );
   pnote->to_list    = str_dup( "" );
   pnote->subject    = str_dup( "" );
   pnote->text        = str_dup( "" );
   pnote->type        = type;
   ch->pnote        = pnote;
   return;
}



void note_remove( CHAR_DATA *ch, NOTE_DATA *pnote, bool delete)
{
   char to_new[MAX_INPUT_LENGTH];
   char to_one[MAX_INPUT_LENGTH];
   NOTE_DATA *prev;
   NOTE_DATA **list;
   char *to_list;

   if (pnote->type == NOTE_HELP && get_trust(ch) >= 54) delete = TRUE;
   if (!delete)
   {
      /* make a new list */
      to_new[0]    = '\0';
      to_list    = pnote->to_list;
      while ( *to_list != '\0' )
      {
         to_list    = one_argument( to_list, to_one );
         if ( to_one[0] != '\0' && str_cmp( ch->name, to_one ) )
         {
            strcat( to_new, " " );
            strcat( to_new, to_one );
         }
      }
      /* Just a simple recipient removal? */
      if
      (
         (
            (
               pnote->id_sender &&
               ch->id != pnote->id_sender
            ) ||
            (
               !pnote->id_sender &&
               str_cmp(ch->name, pnote->sender)
            )
         ) &&
         to_new[0] != '\0'
      )
      {
         free_string( pnote->to_list );
         pnote->to_list = str_dup( to_new + 1 );
         return;
      }
   }
   /* nuke the whole note */

   switch(pnote->type)
   {
      default:
      {
         return;
      }
      case (NOTE_NOTE):
      {
         list = &note_list;
         break;
      }
      case (NOTE_IDEA):
      {
         list = &idea_list;
         break;
      }
      case (NOTE_HELP):
      {
         list = &help_list;
         break;
      }
      case (NOTE_HOARDER):
      {
         list = &hoarder_list;
         break;
      }
      case (NOTE_PENALTY):
      {
         list = &penalty_list;
         break;
      }
      case (NOTE_NEWS):
      {
         list = &news_list;
         break;
      }
      case (NOTE_CHANGES):
      {
         list = &changes_list;
         break;
      }
      case (NOTE_BANK):
      {
         list = &bank_list;
         break;
      }
      case (NOTE_CLAN):
      {
         list = &clan_list;
         break;
      }
      case (NOTE_DEATH):
      {
         list = &death_list;
         break;
      }
      case (NOTE_CRIM):
      {
         list = &crim_list;
         break;
      }
   }

   /*
   * Remove note from linked list.
   */
   if ( pnote == *list )
   {
      *list = pnote->next;
   }
   else
   {
      for ( prev = *list; prev != NULL; prev = prev->next )
      {
         if ( prev->next == pnote )
         break;
      }

      if ( prev == NULL )
      {
         bug( "Note_remove: pnote not found.", 0 );
         return;
      }

      prev->next = pnote->next;
   }

   save_notes(pnote->type);
   free_note(pnote);
   return;
}

bool hide_note (CHAR_DATA *ch, NOTE_DATA *pnote)
{
   time_t last_read;

   if (IS_NPC(ch))
   return TRUE;

   switch (pnote->type)
   {
      default:
      {
         return TRUE;
      }
      case (NOTE_NOTE):
      {
         last_read = ch->pcdata->last_note;
         break;
      }
      case (NOTE_IDEA):
      {
         last_read = ch->pcdata->last_idea;
         break;
      }
      case (NOTE_PENALTY):
      {
         last_read = ch->pcdata->last_penalty;
         break;
      }
      case (NOTE_NEWS):
      {
         last_read = ch->pcdata->last_news;
         break;
      }
      case (NOTE_CHANGES):
      {
         last_read = ch->pcdata->last_changes;
         break;
      }
      case (NOTE_BANK):
      {
         last_read = ch->pcdata->last_bank;
         break;
      }
      case (NOTE_DEATH):
      {
         last_read = ch->pcdata->last_death_note;
         break;
      }
      case (NOTE_CLAN):
      {
         last_read = ch->pcdata->last_clan;
         break;
      }
      case (NOTE_CRIM):
      {
         last_read = ch->pcdata->last_crim;
         break;
      }
   }

   if (pnote->id <= last_read)
   return TRUE;

   if
   (
      (
         pnote->id_sender &&
         ch->id == pnote->id_sender
      ) ||
      (
         !pnote->id_sender &&
         !str_cmp(ch->name, pnote->sender)
      )
   )
   {
      return TRUE;
   }

   if (!is_note_to(ch, pnote))
   return TRUE;

   return FALSE;
}

void update_read(CHAR_DATA *ch, NOTE_DATA *pnote)
{
   time_t stamp;

   if (IS_NPC(ch))
   return;

   stamp = pnote->id;

   switch (pnote->type)
   {
      default:
      {
         return;
      }
      case (NOTE_NOTE):
      {
         ch->pcdata->last_note = UMAX(ch->pcdata->last_note, stamp);
         break;
      }
      case (NOTE_IDEA):
      {
         ch->pcdata->last_idea = UMAX(ch->pcdata->last_idea, stamp);
         break;
      }
      case (NOTE_PENALTY):
      {
         ch->pcdata->last_penalty = UMAX(ch->pcdata->last_penalty, stamp);
         break;
      }
      case (NOTE_NEWS):
      {
         ch->pcdata->last_news = UMAX(ch->pcdata->last_news, stamp);
         break;
      }
      case (NOTE_CHANGES):
      {
         ch->pcdata->last_changes = UMAX(ch->pcdata->last_changes, stamp);
         break;
      }
      case (NOTE_BANK):
      {
         ch->pcdata->last_bank = UMAX(ch->pcdata->last_bank, stamp);
         break;
      }
      case (NOTE_CRIM):
      {
         ch->pcdata->last_crim = UMAX(ch->pcdata->last_crim, stamp);
         break;
      }
      case (NOTE_DEATH):
      {
         ch->pcdata->last_death_note = UMAX(ch->pcdata->last_death_note, stamp);
         break;
      }
      case (NOTE_CLAN):
      {
         ch->pcdata->last_clan = UMAX(ch->pcdata->last_clan, stamp);
         break;
      }
   }
}


/* do_note_types version 1.5 */
bool do_note_types(CHAR_DATA* ch, char* argument, char** text, char* note_type)
{
   char buf       [MAX_NOTE_LENGTH + 512];
   char arg1      [MAX_INPUT_LENGTH];
   char arg2      [MAX_INPUT_LENGTH];
   char new_line  [MAX_INPUT_LENGTH];
   char note_name [20];
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
      bug("do_note_types called with NULL ch (1st parameter)", 0);
      return FALSE;
   }
   if (text == NULL)
   {
      /* Called incorrectly */
      bug("do_note_types called with NULL text (3rd parameter)", 0);
      return FALSE;
   }
   if (argument == NULL)
   {
      /* Called incorrectly, but can recover */
      bug("do_note_types called with NULL argument (2nd parameter)", 0);
      argument = "";
   }
   if (note_type == NULL)
   {
      /* Called incorrectly, but can recover */
      bug("do_note_types called with NULL note_type (4th parameter)", 0);
      note_type = "";
   }

   smash_tilde(argument);

   strcpy(note_name, note_type);
   if (strlen(note_type) > 0)
   {
      /* cut off last letter to make singular form of note name */
      note_name[strlen(note_type) - 1] = '\0';
   }
   if (text[0] == NULL)
   {
      /* Take care of NULL notes */
      text[0] = str_dup("");
      empty = TRUE;
   }

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
         "Formatting your %s to wrap at 80 characters.\n\r",
         note_name
      );
      send_to_char(buf, ch);
      if (text[0] == NULL || text[0][0] == '\0')
      {
         send_to_char("Nothing to format.\n\r", ch);
         return TRUE;
      }
      buf[0] = '\0';
      pnew = buf;
      pold = text[0];
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
      if (strlen(buf) > MAX_NOTE_LENGTH)
      {
         sprintf(buf, "%s will be too long after formatting.\n\r", note_name);
         buf[0] = UPPER(buf[0]);
         send_to_char(buf, ch);
      }
      else
      {
         free_string(text[0]);
         text[0] = str_dup(buf);
      }
      return TRUE;
   }
   else if (!str_cmp(argument, "showlines"))
   {
      if (text[0] == NULL)
      {
         bug("do_note_types has NULL string at showlines", 0);
         return FALSE;
      }
      for (pold = text[0]; pold[0] != '\0'; pold++)
      {
         if (pold[0] == '\r')
         {
            num_lines ++;
         }
      }
      if  /* Take care of case of 1000 blank lines, just in case */
      (
         (strlen(text[0]) + (num_lines * 5) + 2) >= MAX_NOTE_LENGTH + 512
      )
      {
         sprintf(buf, "%s will be too long after adding lines.\n\r", note_name);
         buf[0] = UPPER(buf[0]);
         send_to_char(buf, ch);
         return TRUE;
      }
      else
      {
         sprintf(buf, "Showing your %s with line numbers.\n\r", note_name);
         send_to_char(buf, ch);
      }
      buf[0] = '\0';
      pnew = buf;
      pold = text[0];
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
      return TRUE;
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
      if (text[0] == NULL)
      {
         empty = TRUE;
         num_lines = 0;
      }
      else
      {
         /* Count lines */
         for (pold = text[0]; pold[0] != '\0'; pold++)
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
               "You cannot use the line option when your %s is empty.\n\r",
               note_name
            );
            send_to_char(buf, ch);
            return TRUE;
         }
         if (target_line < 1)
         {
            send_to_char("Line number too low.\n\r", ch);
            return TRUE;
         }
         if (target_line > num_lines)
         {
            sprintf
            (
               buf,
               "You do not have that many lines in your %s.\n\r",
               note_name
            );
            send_to_char(buf, ch);
            return TRUE;
         }
         if (arg2[0] == '\0')
         {
            sprintf
            (
               buf,
               "Too few parameters.  Read the help for %s.\n\r",
               note_name
            );
            send_to_char(buf, ch);
            return TRUE;
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
      else if
      (
         !str_cmp(plusminus, "syntax") ||
         !str_cmp(plusminus, "help")
      )
      {
         /* Show syntax */
         do_help(ch, "note");
      }
      else
      {
         return FALSE;
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
         /* Need to remember if there was nothing left to remove */
         if (num_lines == 1)
         {
            remove = FALSE;
         }
         show = FALSE;  /* No need to show empty note */
      }
      if
      (
         !clear &&
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
         pold = text[0];
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
            if (strlen(new_line) + strlen(text[0]) >= MAX_NOTE_LENGTH)
            {
               sprintf(buf, "%s too long.\n\r", note_name);
               buf[0] = UPPER(buf[0]);
               send_to_char(buf, ch);
               return TRUE;
            }
            strcat(buf, new_line);
            strcat(buf, pold);
            free_string(text[0]);
            text[0] = str_dup(buf);
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
            free_string(text[0]);
            text[0] = str_dup(buf);
         }
         if (!show)
         {
            send_to_char("Ok.\n\r", ch);
         }
      }
      else if (empty && add)
      {
         /* No free_string, empty means its NULL */
         text[0] = str_dup(new_line);
         if (!show)
         {
            send_to_char("Ok.\n\r", ch);
         }
      }
   }
   if (clear)
   {
      if (remove)
      {
         sprintf(buf, "No lines left to remove.\n\r");
         send_to_char(buf, ch);
      }
      else if (!show)
      {
         send_to_char("Ok.\n\r", ch);
      }
      free_string(text[0]);
      text[0] = str_dup("");
   }
   if (show)
   {
      send_to_char((text[0] != NULL && text[0][0] != '\0') ? text[0] : "\n\r", ch);
   }
   return TRUE;
}

void parse_note( CHAR_DATA *ch, char *argument, int type )
{
   char buf[MAX_STRING_LENGTH];
   char arg[MAX_INPUT_LENGTH];
   NOTE_DATA *pnote;
   NOTE_DATA **list;
   char *list_name;
   int vnum;
   int anum;
   int min_list;
   int max_list;
   int stype;
   int found;
   int cnt;
   char buf2[MAX_STRING_LENGTH];
   bool use_external    = TRUE;
   bool was_empty       = TRUE;
   bool extern_success  = FALSE;

   if ( IS_NPC(ch) )
   return;

   switch(type)
   {
      default:
      {
         return;
      }
      case (NOTE_NOTE):
      {
         list = &note_list;
         list_name = "notes";
         break;
      }
      case (NOTE_IDEA):
      {
         list = &idea_list;
         list_name = "ideas";
         break;
      }
      case (NOTE_HELP):
      {
         list = &help_list;
         list_name = "helps";
         break;
      }
      case (NOTE_HOARDER):
      {
         list = &hoarder_list;
         list_name = "hoarders";
         break;
      }
      case (NOTE_PENALTY):
      {
         list = &penalty_list;
         list_name = "penalties";
         break;
      }
      case (NOTE_NEWS):
      {
         list = &news_list;
         list_name = "news";
         break;
      }
      case (NOTE_CHANGES):
      {
         list = &changes_list;
         list_name = "changes";
         break;
      }
      case (NOTE_CLAN):
      {
         list = &clan_list;
         list_name = "clan notes";
         break;
      }
      case (NOTE_BANK):
      {
         list = &bank_list;
         list_name = "bank notes";
         break;
      }
      case (NOTE_CRIM):
      {
         list = &crim_list;
         list_name = "criminal notices";
         break;
      }
      case (NOTE_DEATH):
      {
         list = &death_list;
         list_name = "ancient scrolls";
         break;
      }
   }

   sprintf(buf2, "%s", argument);
   argument = one_argument( argument, arg );
   smash_tilde( argument );
   smash_tilde(buf2);
   if ( arg[0] == '\0' || !str_prefix( arg, "read" ) )
   {
      bool fAll;

      if ( !str_cmp( argument, "all" ) )
      {
         fAll = TRUE;
         anum = 0;
      }

      else if ( argument[0] == '\0' || !str_prefix(argument, "next"))
      /* read next unread note */
      {
         vnum = 0;
         for ( pnote = *list; pnote != NULL; pnote = pnote->next)
         {
            if (!hide_note(ch, pnote))
            {
               sprintf( buf, "[%3d] %s: %s\n\r%s\n\rTo: %s\n\r",
               vnum,
               pnote->sender,
               pnote->subject,
               pnote->date,
               pnote->to_list);
               send_to_char( buf, ch );
               page_to_char( pnote->text, ch );
               update_read(ch, pnote);
               return;
            }
            else if (is_note_to(ch, pnote))
            vnum++;
         }
         sprintf(buf, "You have no unread %s.\n\r", list_name);
         send_to_char(buf, ch);
         return;
      }

      else if ( is_number( argument ) )
      {
         fAll = FALSE;
         anum = atoi( argument );
      }
      else
      {
         send_to_char( "Read which number?\n\r", ch );
         return;
      }

      vnum = 0;
      for ( pnote = *list; pnote != NULL; pnote = pnote->next )
      {
         if ( is_note_to( ch, pnote ) && ( vnum++ == anum || fAll ) )
         {
            sprintf
            (
               buf,
               "[%3d] %s: %s\n\r%s\n\rTo: %s\n\r",
               vnum - 1,
               pnote->sender,
               pnote->subject,
               pnote->date,
               pnote->to_list
            );
            send_to_char( buf, ch );
            page_to_char( pnote->text, ch );
            update_read(ch, pnote);
            return;
         }
      }

      sprintf(buf, "There aren't that many %s.\n\r", list_name);
      send_to_char(buf, ch);
      return;
   }

   if
   (
      !str_prefix(arg, "copy") ||
      !str_prefix(arg, "forward")
   )
   {
      note_attach(ch, type);
      if (ch->pnote->type != type)
      {
         send_to_char
         (
            "You already have a different note in progress.\n\r",
            ch
         );
         return;
      }

      if (is_number(argument))
      {
         anum = atoi(argument);
      }
      else
      {
         send_to_char("Forward which scroll?\n\r", ch);
         return;
      }
      vnum = -1;
      for (pnote = *list; pnote != NULL; pnote = pnote->next)
      {
         if
         (
            is_note_to(ch, pnote) &&
            ++vnum == anum
         )
         {
            break;
         }
      }
      if (pnote == NULL)
      {
         sprintf(buf, "There aren't that many %s.\n\r", list_name);
         send_to_char(buf, ch);
         return;
      }
      free_string(ch->pnote->text);
      ch->pnote->text = str_dup(pnote->text);
      free_string(ch->pnote->subject);
      ch->pnote->subject = str_dup(pnote->subject);
      send_to_char( "Ok.\n\r", ch );
      return;
   }

   if ( !str_prefix( arg, "search") )
   {
      argument = one_argument(argument, arg);
      if (argument[0] == '\0')
      {
         send_to_char("Syntax: search <type> <keyword>\n\r", ch);
         return;
      }
      stype = -1;
      if (!str_cmp(arg, "body"))
      stype = 0;
      if (!str_cmp(arg, "subject"))
      stype = 1;
      if (!str_cmp(arg, "sender"))
      stype = 2;
      if (!str_cmp(arg, "to"))
      stype = 3;
      if (!str_cmp(arg, "all"))
      stype = 4;
      if (stype == -1)
      {
         send_to_char("Search types: body, subject, sender, to, all\n\r", ch);
         return;
      }
      argument = one_argument(argument, arg);
      if (arg[0] == '\0')
      {
         send_to_char("You must provide a keyword.\n\r", ch);
         return;
      }
      vnum = 0; found = FALSE;
      for ( pnote = *list; pnote != NULL; pnote = pnote->next )
      {
         if ( is_note_to( ch, pnote ))
         {
            sprintf( buf, "[%3d%s] %s: %s\n\r",
            vnum, hide_note(ch, pnote) ? " " : "N",
            pnote->sender, pnote->subject );
            vnum++;
            switch(stype)
            {
               default:
               case 0:
               if (!search_string(arg, pnote->text)) continue;
               break;
               case 1:
               if (!search_string(arg, pnote->subject)) continue;
               break;
               case 2:
               if (!search_string(arg, pnote->sender)) continue;
               break;
               case 3:
               if (!search_string(arg, pnote->to_list)) continue;
               break;
               case 4:
               if (!search_string(arg, pnote->text) &&
               !search_string(arg, pnote->subject) &&
               !search_string(arg, pnote->sender) &&
               !search_string(arg, pnote->to_list)) continue;
               break;
            }
            found = TRUE;
            send_to_char( buf, ch );
         }
      }
      if (!found)
      {
         switch(type)
         {
            default:
            case NOTE_NOTE:
            send_to_char("There are no notes for you that match.\n\r", ch);
            break;
            case NOTE_IDEA:
            send_to_char("There are no ideas for you that match.\n\r", ch);
            break;
            case NOTE_PENALTY:
            send_to_char("There are no penalties for you that match.\n\r", ch);
            break;
            case NOTE_NEWS:
            send_to_char("There is no news for you that match.\n\r", ch);
            break;
            case NOTE_CHANGES:
            send_to_char("There are no changes for you that match.\n\r", ch);
            break;
         }
      }
      return;
   }

   if ( !str_prefix( arg, "list" ) )
   {
      BUFFER* output;
      int recent_days = 7;

      argument = one_argument(argument, arg);
      min_list = -1;
      max_list = 9999;
      if (!str_cmp(arg, "all"))
      min_list = 0;
      if (!str_cmp(arg, "recent"))
      {
         min_list = -1;
         max_list = 9999;
         argument = one_argument(argument, arg);
         recent_days = 1;
         if (is_number(arg))
         recent_days = atoi(arg);
      }
      else
      {
         if (is_number(arg))
         {
            min_list = atoi(arg);
            if (argument[0] != '\0')
            {
               argument = one_argument(argument, arg);
               if (is_number(arg))
               max_list = atoi(arg);
            }
         }
      }
      vnum = 0;  found = FALSE;
      output = new_buf();
      for ( pnote = *list; pnote != NULL; pnote = pnote->next )
      {
         if ( is_note_to( ch, pnote ))
         {
            if
            (
               min_list == -1 &&
               pnote->date_stamp < (current_time - recent_days*24*3600)
            )
            {
               vnum++;
               continue;
            }
            if (vnum >= min_list && vnum <= max_list)
            {
               sprintf
               (
                  buf,
                  "[%3d%s] %s: %s\n\r",
                  vnum,
                  hide_note(ch, pnote) ? " " : "N",
                  pnote->sender,
                  pnote->subject
               );
               add_buf(output, buf);
               found = TRUE;
            }
            vnum++;
         }
      }
      if (found == FALSE && vnum)
      {
         send_to_char("There are no recent notes of this type for you. (try using list all)\n\r", ch);
      }
      if (!vnum)
      {
         switch(type)
         {
            default:
            case NOTE_NOTE:
            send_to_char("There are no notes for you.\n\r", ch);
            break;
            case NOTE_IDEA:
            send_to_char("There are no ideas for you.\n\r", ch);
            break;
            case NOTE_PENALTY:
            send_to_char("There are no penalties for you.\n\r", ch);
            break;
            case NOTE_NEWS:
            send_to_char("There is no news for you.\n\r", ch);
            break;
            case NOTE_CHANGES:
            send_to_char("There are no changes for you.\n\r", ch);
            break;
         }
      }
      if (found)
      {
         page_to_char(buf_string(output), ch);
      }
      free_buf(output);
      return;
   }

   if ( !str_prefix( arg, "remove" ) )
   {
      if ( !is_number( argument ) )
      {
         send_to_char( "Note remove which number?\n\r", ch );
         return;
      }

      anum = atoi( argument );
      vnum = 0;
      for ( pnote = *list; pnote != NULL; pnote = pnote->next )
      {
         if ( is_note_to( ch, pnote ) && vnum++ == anum )
         {
            note_remove( ch, pnote, FALSE );
            send_to_char( "Ok.\n\r", ch );
            return;
         }
      }

      sprintf(buf, "There aren't that many %s.\n\r", list_name);
      send_to_char(buf, ch);
      return;
   }

   if ( !str_prefix( arg, "delete" ))
   {
      if (get_trust(ch) >= MAX_LEVEL - 1)
      {
         if ( !is_number( argument ) )
         {
            send_to_char( "Note delete which number?\n\r", ch );
            return;
         }

         anum = atoi( argument );
         vnum = 0;
         for ( pnote = *list; pnote != NULL; pnote = pnote->next )
         {
            if ( is_note_to( ch, pnote ) && vnum++ == anum )
            {
               note_remove( ch, pnote, TRUE );
               send_to_char( "Ok.\n\r", ch );
               return;
            }
         }

         sprintf(buf, "There aren't that many %s.\n\r", list_name);
         send_to_char(buf, ch);
         return;
      }
      use_external = FALSE;
   }

   if (!str_prefix(arg, "catchup"))
   {
      switch(type)
      {
         default:
         {
            return;
         }
         case (NOTE_NOTE):
         {
            ch->pcdata->last_note = last_note_id;
            send_to_char("Note catch-up successful.\n\r", ch);
            break;
         }
         case (NOTE_IDEA):
         {
            ch->pcdata->last_idea = last_note_id;
            send_to_char("Idea catch-up successful.\n\r", ch);
            break;
         }
         case (NOTE_PENALTY):
         {
            ch->pcdata->last_penalty = last_note_id;
            send_to_char("Penalty catch-up successful.\n\r", ch);
            break;
         }
         case (NOTE_NEWS):
         {
            ch->pcdata->last_news = last_note_id;
            send_to_char("News catch-up successful.\n\r", ch);
            break;
         }
         case (NOTE_CHANGES):
         {
            ch->pcdata->last_changes = last_note_id;
            send_to_char("Changes catch-up successful.\n\r", ch);
            break;
         }
         case (NOTE_BANK):
         {
            ch->pcdata->last_bank = last_note_id;
            send_to_char("Banknote catch-up successful.\n\r", ch);
            break;
         }
         case (NOTE_CRIM):
         {
            ch->pcdata->last_crim = last_note_id;
            send_to_char("Criminal catch-up successful.\n\r", ch);
            break;
         }
         case (NOTE_DEATH):
         {
            ch->pcdata->last_death_note = last_note_id;
            send_to_char("Ancient scrolls catch-up successful.\n\r", ch);
            break;
         }
         case (NOTE_CLAN):
         {
            ch->pcdata->last_clan = last_note_id;
            send_to_char("Clans catch-up successful.\n\r", ch);
            break;
         }
      }
      return;
   }

   /* below this point only certain people can edit notes */
   if
   (
      !IS_TRUSTED(ch, LEVEL_IMMORTAL) &&
      (
         type == NOTE_NEWS ||
         type == NOTE_CHANGES ||
         type == NOTE_CLAN ||
         type == NOTE_DEATH ||
         type == NOTE_BANK
      )
   )
   {
      sprintf(buf, "You aren't high enough level to write %s.\n\r", list_name);
      send_to_char(buf, ch);
      return;
   }
   if
   (
      !IS_TRUSTED(ch, LEVEL_IMMORTAL) &&
      ch->house != HOUSE_ENFORCER &&
      type == NOTE_CRIM
   )
   {
      send_to_char("You may not write criminal notices.\n\r", ch);
      return;
   }

   if ( !str_prefix( arg, "subject" ) )
   {
      note_attach( ch, type );
      if (ch->pnote->type != type)
      {
         send_to_char(
         "You already have a different note in progress.\n\r", ch);
         return;
      }

      free_string( ch->pnote->subject );
      ch->pnote->subject = str_dup( argument );
      send_to_char( "Ok.\n\r", ch );
      return;
   }

   if ( !str_prefix( arg, "to" ) )
   {
      note_attach( ch, type );
      if (ch->pnote->type != type)
      {
         send_to_char(
         "You already have a different note in progress.\n\r", ch);
         return;
      }
      free_string( ch->pnote->to_list );
      ch->pnote->to_list = str_dup( argument );
      send_to_char( "Ok.\n\r", ch );
      if (strstr(ch->pnote->to_list, ","))
      {
         send_to_char
         (
            "Separate recipients with spaces, not commas.\n\r"
            "Please re-declare your recipients.\n\r",
            ch
         );
      }
      return;
   }

   if ( !str_prefix( arg, "from" ))
   {
      if
      (
         get_trust(ch) >= 55 ||
         (
            IS_SET(ch->wiznet, WIZ_ALLOWRP) &&
            IS_IMMORTAL(ch)
         )
      )
      {
         note_attach( ch, type );
         if (ch->pnote->type != type)
         {
            send_to_char(
            "You already have a different note in progress.\n\r", ch);
            return;
         }
         free_string( ch->pnote->sender );
         ch->pnote->sender = str_dup( argument );
         send_to_char( "Ok.\n\r", ch );
         return;
      }
      use_external = FALSE;
   }


   if ( !str_prefix( arg, "clear" ) )
   {
      if ( ch->pnote != NULL )
      {
         free_note(ch->pnote);
         ch->pnote = NULL;
      }

      send_to_char( "Ok.\n\r", ch );
      return;
   }

   if ( !str_prefix( arg, "post" ) || !str_prefix(arg, "send"))
   {
      char *strtime;

      if ( ch->pnote == NULL )
      {
         send_to_char( "You have no note in progress.\n\r", ch );
         return;
      }

      if (ch->pnote->type != type)
      {
         send_to_char("You aren't working on that kind of note.\n\r", ch);
         return;
      }

      if (!str_cmp(ch->pnote->to_list, ""))
      {
         send_to_char(
         "You need to provide a recipient (name, house, or immortal).\n\r",
         ch);
         return;
      }
      if (!IS_IMMORTAL(ch))
      {
         for (cnt = 0; cnt < MAX_CLASS; cnt++)
         {
            if
            (
               (
                  IS_SET(ch->act2, PLR_GUILDMASTER) &&
                  ch->class == cnt
               ) ||
               (
                  ch->pcdata->clan &&
                  clan_table[ch->pcdata->clan].rank >= 3 &&
                  clan_table[ch->pcdata->clan].clan_class == cnt &&
                  get_clan_position(ch, ch->pcdata->clan) == CLAN_LEADER
               )
            )
            {
               continue;
            }
            if (is_name(class_table[cnt].name, ch->pnote->to_list))
            {
               if (IS_SET(ch->act2, PLR_GUILDMASTER))
               {
                  send_to_char
                  (
                     "As a guildmaster, you may send notes to only your guild."
                     "\n\r",
                     ch
                  );
               }
               else
               {
                  send_to_char
                  (
                     "Only immortals can send notes to guilds.\n\r",
                     ch
                  );
               }
               return;
            }
         }
      }
      if (!IS_IMMORTAL(ch))
      {
         for (cnt = 0; cnt < MAX_PC_RACE; cnt++)
         {
            if
            (
               ch->pcdata->clan &&
               clan_table[ch->pcdata->clan].rank >= 3 &&
               clan_table[ch->pcdata->clan].clan_race == cnt &&
               get_clan_position(ch, ch->pcdata->clan) == CLAN_LEADER
            )
            {
              continue;
            }
            if (is_name(race_table[cnt].name, ch->pnote->to_list))
            {
               send_to_char
               (
                  "Only immortals can send notes to races.\n\r",
                  ch
               );
               return;
            }
         }
      }

      /* Coder Note addition by Wicket */
      if ( (is_name( "coder", ch->pnote->to_list )
      ||  is_name( "coders", ch->pnote->to_list))
      &&  !IS_IMMORTAL(ch) )
      {
         send_to_char("Only Immortals can send notes to coders.\n\r", ch);
         return;
      }

      if
      (
         is_name("all", ch->pnote->to_list) &&
         !IS_IMMORTAL(ch) &&
         ch->pcdata->induct == 0 &&
         (
            IS_NPC(ch) ||
            ch->pcdata->clan == 0 ||
            clan_table[ch->pcdata->clan].rank < 2 ||
            get_clan_position(ch, ch->pcdata->clan) != CLAN_LEADER
         )
      )
      {
         send_to_char("Only Immortals and House leaders can send notes to all.\n\r", ch);
         return;
      }

      if (!str_cmp(ch->pnote->subject, ""))
      {
         send_to_char("You need to provide a subject.\n\r", ch);
         return;
      }

      ch->pnote->next            = NULL;
      strtime                = ctime( &current_time );
      strtime[strlen(strtime)-1]    = '\0';
      ch->pnote->date            = str_dup( strtime );
      ch->pnote->date_stamp        = current_time;

      append_note(ch->pnote);
      ch->pnote = NULL;
      send_to_char( "Ok.\n\r", ch );
      if (is_affected(ch, gsn_cloak_form)){
         cloak_note(ch);
      }
      return;
   }

   if (use_external)
   {
      if ( ch->pnote != NULL )
      {
         was_empty = FALSE;
      }
      note_attach( ch, type );
      if (ch->pnote->type != type)
      {
         send_to_char
         (
            "You already have a different note in progress.\n\r",
            ch
         );
         return;
      }
      if ( ch->pnote == NULL )
      {
         send_to_char( "You have no note in progress.\n\r", ch );
         return;
      }
      if (!str_prefix( arg, "show" ) || !str_prefix( arg, "showlines" ))
      {
         sprintf
         (
            buf,
            "%s: %s\n\rTo: %s\n\r",
            ch->pnote->sender,
            ch->pnote->subject,
            ch->pnote->to_list
         );
         send_to_char( buf, ch );
         /* do_note_types doesn't use prefix */
         if (!str_cmp( arg, "showlines" ))
         {
            sprintf(buf2, "showlines");
         }
         else
         {
            sprintf(buf2, "show");
         }
      }
      extern_success = do_note_types(ch, buf2, &(ch->pnote->text), list_name);
      if
      (
         !extern_success &&
         ch->pnote != NULL &&
         was_empty
      )
      {
         free_note(ch->pnote);
         ch->pnote = NULL;
      }
   }
   if (!extern_success)
   {
      send_to_char( "You can't do that.\n\r", ch );
   }
   return;
}

void bounty_note(CHAR_DATA *ch)
{
   char buf[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH]; /*for subjects with names */
   NOTE_DATA *pnote;
   char *strtime;
   char temp_string[MAX_STRING_LENGTH];

   sprintf(temp_string, "ancient %s", ch->pcdata->marker);

   pnote = new_note();

   pnote->next        = NULL;
   pnote->sender       = str_dup( "The Ancients" );
   pnote->to_list      = str_dup( temp_string );
   sprintf(buf2, "Job Complete: %s", ch->name);
   pnote->subject = str_dup(buf2);

   sprintf(buf, "%s was terminated.\n\r", ch->name);
   pnote->text         = str_dup( buf );
   pnote->type         = NOTE_DEATH;
   strtime             = ctime( &current_time );
   strtime[strlen(strtime)-1]      = '\0';
   pnote->date         = str_dup( strtime );
   pnote->date_stamp   = current_time;

   append_note(pnote);
   return;
}

void hoard_report( CHAR_DATA *ch)
{
   char buf[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH]; /*for subjects with names */
   NOTE_DATA *pnote;
   char *strtime;

   pnote = new_note();

   pnote->next        = NULL;
   pnote->sender       = str_dup( "Darkmists Automated Note" );
   pnote->to_list      = str_dup( "immortal" );
   /*
   pnote->subject      = str_dup( "*****  Possible Hoarder  *****" );
   new subjects with names*/
   sprintf(buf2, "Possible Hoarder: %s", ch->name);
   pnote->subject = str_dup(buf2);

   sprintf(buf, "%s's probation time for the last %d days was:\n\r%d hours, %d minutes, %d seconds.\n\rHis hoard score was %d.\n\r%s%s\n\r",
   ch->name,
   (int) ((current_time-ch->pcdata->prob_time)/(3600*24)),
   (int) (ch->pcdata->probation/3600),
   (int) (ch->pcdata->probation - ((int)ch->pcdata->probation/3600)*3600)/60,
   (int) ch->pcdata->probation % 60,
   hoard_score(ch),
   (ch->house != 0) ? "House ":"",
   house_table[ch->house].name);
   pnote->text         = str_dup( buf );
   pnote->type         = NOTE_HOARDER;
   strtime             = ctime( &current_time );
   strtime[strlen(strtime)-1]      = '\0';
   pnote->date         = str_dup( strtime );
   pnote->date_stamp   = current_time;

   pnote->type        = NOTE_HOARDER;
   append_note(pnote);
   return;
}

void make_note( char * sender, char * subject, char * note_to, char * buf,
int note_type)
{
   NOTE_DATA *pnote;
   char *strtime;

   pnote = new_note();
   pnote->next = NULL;
   pnote->sender = str_dup( sender );
   pnote->to_list = str_dup( note_to);
   pnote->subject = str_dup( subject);
   pnote->text    = str_dup( buf );
   pnote->type    = note_type;
   strtime             = ctime( &current_time );
   strtime[strlen(strtime)-1]      = '\0';
   pnote->date         = str_dup( strtime );
   pnote->date_stamp   = current_time;

   append_note(pnote);
   return;
}

void auto_mark_note( CHAR_DATA *ch, CHAR_DATA *victim, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   NOTE_DATA *pnote;
   char *strtime;

   pnote = new_note();

   pnote->next        = NULL;
   pnote->sender       = str_dup( "The Eye of the Ancient" );
   pnote->to_list      = str_dup( "immancient" );
   pnote->subject      = str_dup( "Mark Information" );
   sprintf(buf, "%s %s %s.\n\r", victim->name, argument, ch->name);
   pnote->text         = str_dup( buf );
   pnote->type         = NOTE_NOTE;
   strtime             = ctime( &current_time );
   strtime[strlen(strtime)-1]      = '\0';
   pnote->date         = str_dup( strtime );
   pnote->date_stamp   = current_time+2;

   append_note(pnote);
   return;
}

void cloak_note(CHAR_DATA *ch)
{
   char buf[MAX_STRING_LENGTH];
   NOTE_DATA *pnote;
   char *strtime;

   pnote = new_note();

   pnote->next        = NULL;
   pnote->sender       = str_dup( "The Eye of the Ancient" );
   pnote->to_list      = str_dup( "immancient" );
   pnote->subject      = str_dup( "Cloaked note info" );
   sprintf(buf, "%s sent a note as a cloaked figure.\n\r", ch->name);
   pnote->text         = str_dup( buf );
   pnote->type         = NOTE_NOTE;
   strtime             = ctime( &current_time );
   strtime[strlen(strtime)-1]      = '\0';
   pnote->date         = str_dup( strtime );
   pnote->date_stamp   = current_time+1;

   append_note(pnote);
   return;
}


/* Modified and taken from my mud client -Wervdon */
bool search_string( char *str, char *namelist )
{
   int position;
   int letter_match;
   /* fix crash on NULL namelist */
   if (namelist == NULL || namelist[0] == '\0')
   return FALSE;

   /* fixed to prevent is_name on "" returning TRUE */
   if (str[0] == '\0')
   return FALSE;
   position = 0; letter_match = 0;
   while (namelist[position] != '\0')
   {
      if (LOWER(namelist[position]) == LOWER(str[letter_match]))
      {
         letter_match++;
         if (str[letter_match] == '\0')
         return TRUE;
      }
      else
      {
         letter_match = 0;
      }
      position++;
   }
   return FALSE;
}
