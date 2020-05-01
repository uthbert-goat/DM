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

static const char rcsid[] = "$Id: recycle.c,v 1.101 2004/10/23 04:58:01 maelstrom Exp $";

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

extern char* str_none;

/* stuff for recyling notes */
NOTE_DATA *note_free;

NOTE_DATA *new_note()
{
   static NOTE_DATA note_zero;
   NOTE_DATA *note;
   if (note_free == NULL){
      note = alloc_perm(sizeof(*note));
   }
   else
   {
      note = note_free;
      note_free = note_free->next;
   }
   mem_counts[NMEM_NOTES]++;
   *note = note_zero;
   VALIDATE(note);
   return note;
}

void free_note(NOTE_DATA *note)
{
   if (!note || !IS_VALID(note))
   return;

   free_string( note->text    );
   free_string( note->subject );
   free_string( note->to_list );
   free_string( note->date    );
   free_string( note->sender  );
   INVALIDATE(note);

   mem_counts[NMEM_NOTES]--;
   note->next = note_free;
   note_free   = note;
}


/* stuff for recycling ban structures */
BAN_DATA *ban_free;

BAN_DATA *new_ban(void)
{
   static BAN_DATA ban_zero;
   BAN_DATA *ban;

   if (ban_free == NULL){
      ban = alloc_perm(sizeof(*ban));
   }
   else
   {
      ban = ban_free;
      ban_free = ban_free->next;
   }

   *ban = ban_zero;
   mem_counts[NMEM_BANS]++;
   VALIDATE(ban);
   ban->name = &str_empty[0];
   return ban;
}

void free_ban(BAN_DATA *ban)
{
   if (!ban || !IS_VALID(ban))
   return;

   free_string(ban->name);
   INVALIDATE(ban);

   mem_counts[NMEM_BANS]--;
   ban->next = ban_free;
   ban_free = ban;
}

/* stuff for recycling nameban structures */
NAMEBAN_DATA *nameban_free;

NAMEBAN_DATA *new_nameban(void)
{
   static NAMEBAN_DATA nameban_zero;
   NAMEBAN_DATA *nameban;

   if (nameban_free == NULL){
      nameban = alloc_perm(sizeof(*nameban));
   }
   else
   {
      nameban = nameban_free;
      nameban_free = nameban_free->next;
   }

   *nameban = nameban_zero;
   mem_counts[NMEM_NAMEBANS]++;
   VALIDATE(nameban);
   nameban->name = &str_empty[0];
   return nameban;
}

void free_nameban(NAMEBAN_DATA *nameban)
{
   if (!nameban || !IS_VALID(nameban))
   return;

   free_string(nameban->name);
   INVALIDATE(nameban);

   mem_counts[NMEM_NAMEBANS]--;
   nameban->next = nameban_free;
   nameban_free = nameban;
}

/* stuff for recycling descriptors */
DESCRIPTOR_DATA *descriptor_free;

DESCRIPTOR_DATA *new_descriptor(void)
{
   static DESCRIPTOR_DATA d_zero;
   DESCRIPTOR_DATA *d;

   if (descriptor_free == NULL){
      d = alloc_perm(sizeof(*d));
   }
   else
   {
      d = descriptor_free;
      descriptor_free = descriptor_free->next;
   }
   /*
      taken care of by d_zero
      d->snoops = NULL;
   */
   *d = d_zero;
   mem_counts[NMEM_DESCRTOR]++;
   VALIDATE(d);
   return d;
}

void free_descriptor(DESCRIPTOR_DATA *d)
{
   if (!d || !IS_VALID(d))
   return;

   free_string( d->host );
   free_mem( d->outbuf, d->outsize );
   INVALIDATE(d);
   mem_counts[NMEM_DESCRTOR]--;
   d->next = descriptor_free;
   descriptor_free = d;
}

/* stuff for recycling gen_data */
GEN_DATA* gen_data_free;

GEN_DATA* new_gen_data(void)
{
   static GEN_DATA gen_zero;
   GEN_DATA* gen;
   int cnt;
   bool* skill_chosen_temp;

   if (gen_data_free == NULL)
   {
      gen = alloc_perm(sizeof(*gen));
      gen->skill_chosen = alloc_perm(MAX_SKILL * sizeof(bool));
   }
   else
   {
      gen = gen_data_free;
      gen_data_free = gen_data_free->next;
   }

   mem_counts[NMEM_GEN_DATA]++;

   skill_chosen_temp = gen->skill_chosen;
   *gen = gen_zero;
   gen->skill_chosen = skill_chosen_temp;
   /*
      Zero out dynamically generated array
      DYNAMICALLY GENERATED, not taken care of by
      gen_zero
   */
   for (cnt = 0; cnt < MAX_SKILL; cnt++)
   {
      gen->skill_chosen[cnt] = 0;
   }
   VALIDATE(gen);
   return gen;
}

void free_gen_data(GEN_DATA* gen)
{
   if
   (
      !gen ||
      !IS_VALID(gen)
   )
   {
      return;
   }

   INVALIDATE(gen);
   mem_counts[NMEM_GEN_DATA]--;
   gen->next = gen_data_free;
   gen_data_free = gen;
}

/* stuff for recycling extended descs */
EXTRA_DESCR_DATA *extra_descr_free;

EXTRA_DESCR_DATA *new_extra_descr(void)
{
   EXTRA_DESCR_DATA *ed;

   if (extra_descr_free == NULL){
      ed = alloc_perm(sizeof(*ed));
   }
   else
   {
      ed = extra_descr_free;
      extra_descr_free = extra_descr_free->next;
   }

   mem_counts[NMEM_EX_DESCR]++;
   ed->keyword = &str_empty[0];
   ed->description = &str_empty[0];
   VALIDATE(ed);
   return ed;
}

void free_extra_descr(EXTRA_DESCR_DATA *ed)
{
   if (!ed || !IS_VALID(ed))
   return;

   free_string(ed->keyword);
   free_string(ed->description);
   INVALIDATE(ed);
   mem_counts[NMEM_EX_DESCR]--;
   ed->next = extra_descr_free;
   extra_descr_free = ed;
}

/* stuff for recycling affects */
AFFECT_DATA *affect_free;

AFFECT_DATA *new_affect(void)
{
   static AFFECT_DATA af_zero;
   AFFECT_DATA *af;

   total_affects++;

   if (affect_free == NULL){
      af = alloc_perm(sizeof(*af));
   }
   else
   {
      af = affect_free;
      affect_free = affect_free->next;
   }

   mem_counts[NMEM_AFFECTS]++;

   /*
      The following line sets ALL static memory to 0's
      (even static arrays)
      The only memory that needs be initialized is
      things that are not set to NULL or 0, or was
      dynamically generated.
   */
   *af = af_zero;
   /*
      af_zero takes care of the following
      af->level      = 0;
      af->duration   = 0;
      af->modifier   = 0;
      af->location   = 0;
      af->bitvector  = 0;
      af->bitvector2 = 0;
      af->next       = NULL;
      af->caster     = NULL;
      / * Version 1 added bitvector2 * /
      af->bitvector2 = 0;
   */
   /* Initialize all values */
   af->where      = TO_AFFECTS;

   VALIDATE(af);

   return af;
}


void free_affect(AFFECT_DATA *af)
{
   if (!af || !IS_VALID(af))
   return;

   if (af->caster != NULL)
   free_string(af->caster);

   total_affects--;

   mem_counts[NMEM_AFFECTS]--;
   INVALIDATE(af);
   af->next = affect_free;
   affect_free = af;
}

/* stuff for declaring snoops */

SNOOP_DATA *new_snoop(DESCRIPTOR_DATA *desc)
{
   SNOOP_DATA *snoop;

   snoop = (SNOOP_DATA *) malloc(sizeof(SNOOP_DATA));
   if (snoop == NULL)
   {
      log_string("Error allocating mem for snoop data.");
      exit(-1);
   }
   snoop->next = desc->snoops;
   desc->snoops = snoop;
   return snoop;
}

void free_snoop(DESCRIPTOR_DATA *desc, SNOOP_DATA * snoop)
{
   SNOOP_DATA *ptr;

   if (desc->snoops == snoop)
   {
      desc->snoops = snoop->next;
      free(snoop);
      return;
   }
   ptr = desc->snoops;
   while(ptr != NULL)
   {
      if (ptr->next == snoop)
      break;
      ptr = ptr->next;
   }
   if (ptr == NULL)
   {
      log_string("BUG: free_snoop, snoop not found.");
      return;
   }
   ptr->next = snoop->next;
   free(snoop);
   return;
}

/* stuff for recycling affects for rooms*/
ROOM_AFFECT_DATA *room_affect_free;

ROOM_AFFECT_DATA *new_room_affect(void)
{
   ROOM_AFFECT_DATA *af;
   static ROOM_AFFECT_DATA room_affect_zero;
   ROOM_AFFECT_DATA *raf;

   if (room_affect_free == NULL){
      af = alloc_perm(sizeof(*af));
   }
   else
   {
      af = room_affect_free;
      room_affect_free = room_affect_free->next;
   }

   /*
      The following line sets ALL static memory to 0's
      (even static arrays)
      The only memory that needs be initialized is
      things that are not set to NULL or 0, or was
      dynamically generated.
   */
   *af = room_affect_zero;

   /*
      taken care of by room_affect_zero
      af->next_global = 0;
      af->caster = NULL;
   */
   if (first_room_affect == NULL)
   {
      first_room_affect = af;
   }
   else
   {

      raf = first_room_affect;
      while (raf->next_global != NULL)
      raf = raf->next_global;
      raf->next_global = af;
   }
   VALIDATE(af);
   return af;
}

void free_room_affect(ROOM_AFFECT_DATA *af)
{
   ROOM_AFFECT_DATA *prev;
   ROOM_AFFECT_DATA *raf;


   if (!af || !IS_VALID(af))
   return;

   af->caster = NULL;

   raf = first_room_affect;
   prev = NULL;
   while(raf != af){
      prev = raf;
      raf = raf->next_global;
   }
   /* XUR
   if (prev != NULL)
   prev->next_global = raf->next_global;
   if (raf == first_room_affect)
   first_room_affect = raf->next_global;
   raf->next_global = NULL;
   */

   if (prev)
   prev->next_global = raf->next_global;
   else
   first_room_affect = raf->next_global;
   raf->next_global = NULL;

   INVALIDATE(af);
   af->next = room_affect_free;
   room_affect_free = af;
}

/* stuff for recycling objects */
OBJ_DATA *obj_free;

OBJ_DATA *new_obj(void)
{
   static OBJ_DATA obj_zero;
   OBJ_DATA *obj;
   sh_int cnt;


   if (obj_free == NULL)
   {
      obj = alloc_perm(sizeof(*obj));
   }
   else
   {
      obj = obj_free;
      obj_free = obj_free->next;
   }
   mem_counts[NMEM_OBJECTS]++;

   /*
      The following line sets ALL static memory to 0's
      (even static arrays)
      The only memory that needs be initialized is
      things that are not set to NULL or 0, or was
      dynamically generated.
   */
   *obj = obj_zero;

   /*
      taken care of by obj_zero
      obj->contains = NULL;
      obj->in_obj = NULL;
      obj->on = NULL;
      obj->carried_by = NULL;
      obj->extra_descr = NULL;
      obj->affected = NULL;
      obj->pIndexData = NULL;
      obj->in_room = NULL;
      obj->talked = NULL;
      obj->name = NULL;
      obj->short_descr = NULL;
      obj->description = NULL;
      obj->material = NULL;
   */

   obj->owner = &str_none[0];
   for (cnt = 0; cnt < MAX_OWNERS; cnt++)
   {
      obj->prev_owners[cnt] = &str_empty[0];
      obj->prev_owners_site[cnt] = &str_empty[0];
      obj->prev_owners_id[cnt] = -1;
   }
   VALIDATE(obj);

   return obj;
}

void free_obj(OBJ_DATA *obj)
{
   AFFECT_DATA* paf;
   AFFECT_DATA* paf_next;
   EXTRA_DESCR_DATA* ed;
   EXTRA_DESCR_DATA* ed_next;
   sh_int cnt;

   if (!obj || !IS_VALID(obj))
   return;


   if (obj->book_info != NULL)
   {
      free_bookdata(obj->book_info);
   }
   for (paf = obj->affected; paf != NULL; paf = paf_next)
   {
      paf_next = paf->next;
      free_affect(paf);
   }
   obj->affected = NULL;

   for (ed = obj->extra_descr; ed != NULL; ed = ed_next )
   {
      ed_next = ed->next;
      free_extra_descr(ed);
   }
   obj->extra_descr = NULL;

   free_string(obj->name);
   free_string(obj->description);
   free_string(obj->short_descr);
   free_string(obj->owner);
   for (cnt = 0; cnt < MAX_OWNERS; cnt++)
   {
      free_string(obj->prev_owners[cnt]);
      free_string(obj->prev_owners_site[cnt]);
   }
   INVALIDATE(obj);

   mem_counts[NMEM_OBJECTS]--;
   obj->next   = obj_free;
   obj_free    = obj;
}


/* stuff for recyling characters */
CHAR_DATA *char_free;

CHAR_DATA *new_char (void)
{
   extern CHAR_DATA* last_char_extract;
   extern CHAR_DATA* last_char_extract_failed;
   static CHAR_DATA ch_zero;
   CHAR_DATA *ch;
   int i;

   if (char_free == NULL){
      ch = alloc_perm(sizeof(*ch));
   }
   else
   {
      ch = char_free;
      char_free = char_free->next;
   }
   mem_counts[NMEM_CHAR_DAT]++;

   /*
      The following line sets ALL static memory to 0's
      (even static arrays)
      The only memory that needs be initialized is
      things that are not set to NULL or 0, or was
      dynamically generated.
   */
   *ch                = ch_zero;
   /*
      pcdata_zero takes care of the following
      ch->mprog_target            = NULL;
      ch->nightfall               = NULL;
      ch->defending               = NULL;
      ch->life_lined              = NULL;
      ch->see                     = NULL;
      ch->nosee                   = NULL;
      ch->alarm                   = NULL;
      ch->memory                  = NULL;
      ch->spec_fun                = NULL;
      ch->pIndexData              = NULL;
      ch->desc                    = NULL;
      ch->affected                = NULL;
      ch->pnote                   = NULL;
      ch->carrying = NULL;
      ch->on = NULL;
      ch->in_room = NULL;
      ch->was_in_room = NULL;
      ch->zone = NULL;
      ch->pcdata = NULL;
      ch->gen_data = NULL;
      ch->material = NULL;
      ch->act                     = 0;
      ch->act2                    = 0;
      ch->spyfor                  = NULL;
      ch->master                  = NULL;
      ch->leader                  = NULL;
      ch->pet                     = NULL;
      ch->fighting                = NULL;
      ch->reply                   = NULL;
      ch->subrace = 0;
      ch->spell_power = 0;
      ch->holy_power = 0;
      ch->sight = 0;
      ch->ghost = 0;
      ch->on_line                = FALSE;
      ch->is_riding = NULL;
      ch->air_loss = 0;
   */

   ch->version                = 49;
   ch->name                   = &str_empty[0];
   ch->short_descr            = &str_empty[0];
   ch->long_descr             = &str_empty[0];
   ch->description            = &str_empty[0];
   ch->prompt                 = &str_empty[0];
   ch->host                   = &str_empty[0];
   ch->prefix                 = &str_empty[0];
   ch->logon                  = current_time;
   ch->lines                  = PAGELEN;
   ch->comm2                  = COMM_REDUCED_COLOR;
   for (i = 0; i < 4; i++)
   {
      ch->armor[i]            = 100;
   }
   ch->position               = POS_STANDING;
   ch->hit                    = 20;
   ch->max_hit                = 20;
   ch->mana                   = 100;
   ch->max_mana               = 100;
   ch->move                   = 100;
   ch->max_move               = 100;
   ch->true_alignment         = -5000;
   ch->align_mod              = 0;
   for (i = 0; i < MAX_STATS; i ++)
   {
      ch->perm_stat[i] = 13;
      /*
         taken care of by ch_zero
         ch->mod_stat[i] = 0;
      */
   }
   /* These are global variables, not taken care of by ch_zero */
   if (ch == last_char_extract)
   {
      last_char_extract = NULL;
   }
   if (ch == last_char_extract_failed)
   {
      last_char_extract_failed = NULL;
   }
   VALIDATE(ch);
   return ch;
}

ROSTER* new_roster(char* data)
{
   ROSTER* proster;

   proster = (ROSTER*)malloc(sizeof(*proster));
   proster->next = NULL;
   proster->data = str_dup(data);
   return proster;
}

void free_roster(ROSTER* proster)
{
   if (proster == NULL)
   {
      return;
   }
   free_roster(proster->next);
   free_string(proster->data);
   free(proster);
}

void free_char (CHAR_DATA *ch)
{
   OBJ_DATA *obj;
   OBJ_DATA *obj_next;
   CHAR_DATA *tch;
   AFFECT_DATA *paf;
   AFFECT_DATA *paf_next;
   ROOM_AFFECT_DATA* raf;
   MESSAGE * hung_message;
   ALARM_DATA* before;

   if (!ch || !IS_VALID(ch))
   return;

   if (IS_NPC(ch))
   {
      mobile_count--;
   }

   /* remove ch from the id hash table for future lookups */
   remove_id_hash(ch);

   if (ch->race == grn_book)
   {
      remove_node_for(ch, book_race_list);
   }
   raf = first_room_affect;
   while (raf != NULL)
   {
      if (raf->caster == ch)
      {
         raf->caster = NULL;
      }
      raf = raf->next_global;
   }
   for (tch = char_list; tch != NULL; tch = tch->next)
   {
      if (!IS_NPC(tch) && tch->pcdata->called_by == ch)
      {
         tch->pcdata->called_by = NULL;
      }
      if (tch->see == ch) tch->see = NULL;
      if (tch->nosee == ch) tch->nosee = NULL;
   }

   free_string(ch->host);
   ch->host = &str_empty[0];
   for (obj = ch->carrying; obj != NULL; obj = obj_next)
   {
      obj_next = obj->next_content;
      /*        obj->pIndexData->limcount++;*/
      /*        bug("Free_char: Extracting obj %d", obj->pIndexData->vnum);*/
      if
      (
         IS_NPC(ch) ||
         ch->pcdata->being_restored ||
         ch->level < 2
      )
      {
         extract_obj(obj, FALSE);
      }
      else
      {
         extract_obj(obj, TRUE);
      }
   }

   if ( (hung_message = get_message_char(ch, MM_HANG1)) != NULL)
   {
      message_remove(hung_message);
   }
   if ( (hung_message = get_message_char(ch, MM_HANG2)) != NULL)
   {
      message_remove(hung_message);
   }
   if ( (hung_message = get_message_char(ch, MM_HANG3)) != NULL)
   {
      message_remove(hung_message);
   }
   if ( (hung_message = get_message_char(ch, MM_HANG4)) != NULL)
   {
      message_remove(hung_message);
   }
   for (paf = ch->affected; paf != NULL; paf = paf_next)
   {
      paf_next = paf->next;
      affect_remove(ch, paf);
   }

   if (ch->alarm)
   {
      if (ch->alarm->room->alarm == ch->alarm)
      {
         ch->alarm->room->alarm = ch->alarm->room->alarm->next;
         free_alarm(ch->alarm);
      }
      else
      {
         before = ch->alarm->room->alarm;
         while
         (
            before != NULL &&
            before->next &&
            before->next != ch->alarm
         )
         {
            before = before->next;
         }
         if
         (
            before &&
            before->next == ch->alarm
         )
         {
            before->next = before->next->next;
            free_alarm(ch->alarm);
         }
      }
      ch->alarm = NULL;
   }
   free_string(ch->name);
   free_string(ch->short_descr);
   free_string(ch->long_descr);
   free_string(ch->description);
   free_string(ch->prompt);
   free_string(ch->prefix);
   if (ch->pcdata != NULL)
   free_pcdata(ch->pcdata);

   if ( ch->is_riding )
   free_mount_info(ch->is_riding);

   if (ch->skill_mods)
   {
      free_list(ch->skill_mods);
      ch->skill_mods = NULL;
   }
   mem_counts[NMEM_CHAR_DAT]--;
   ch->next = char_free;
   char_free  = ch;

   INVALIDATE(ch);
   return;
}

SKILL_MOD* skill_mod_free;

SKILL_MOD* new_skill_mod(int skill_percent, int sn)
{
   static SKILL_MOD mod_zero;
   SKILL_MOD* mod;

   if (skill_mod_free == NULL)
   {
      mod = alloc_perm(sizeof(SKILL_MOD));
   }
   else
   {
      mod = skill_mod_free;
      skill_mod_free = skill_mod_free->next;
   }
   *mod = mod_zero;
   mem_counts[NMEM_SKILL_MOD]++;
   mod->skill_percent = skill_percent;
   mod->sn = sn;
   VALIDATE(mod);
   return mod;
}

void free_skill_mod(SKILL_MOD* mod)
{
   if
   (
      !mod ||
      !IS_VALID(mod)
   )
   {
      return;
   }

   INVALIDATE(mod);
   mem_counts[NMEM_SKILL_MOD]--;
   mod->next = skill_mod_free;
   skill_mod_free = mod;
}

BOOK_DATA* new_bookdata(void)
{
   static BOOK_DATA bdata_zero;
   BOOK_DATA* bdata;

   bdata = (BOOK_DATA*)malloc(sizeof(BOOK_DATA));
   *bdata = bdata_zero;
   /* sets everything to 0, including ->book_save_info */
   return bdata;
}

void free_bookdata(BOOK_DATA* bdata)
{
   int cnt;

   for (cnt = 0; cnt < BOOK_MAX_PAGES; cnt++)
   {
      free_string(bdata->page[cnt]);
      free_string(bdata->title[cnt]);
   }
   if (bdata->book_save_info != NULL)
   {
      free_booksavedata(bdata->book_save_info);
   }
   free(bdata);
}

BOOK_SAVE_DATA* new_booksavedata(void)
{
   static BOOK_SAVE_DATA bdata_zero;
   BOOK_SAVE_DATA* bdata;

   bdata = (BOOK_SAVE_DATA*)malloc(sizeof(BOOK_SAVE_DATA));
   *bdata = bdata_zero;
   return bdata;
}

void free_booksavedata(BOOK_SAVE_DATA* bdata)
{
   free(bdata);
}

PC_DATA* pcdata_free;

PC_DATA* new_pcdata(void)
{
   static PC_DATA pcdata_zero;
   PC_DATA* pcdata;
   sh_int desc_count;
   sh_int* learned_temp;
   sh_int* learnlvl_temp;
   bool*   updated_temp;
   int cnt;

   if (pcdata_free == NULL)
   {
      pcdata = alloc_perm(sizeof(*pcdata));
      pcdata->learned = (sh_int*)alloc_perm (MAX_SKILL * sizeof(sh_int));
      pcdata->learnlvl = (sh_int*)alloc_perm (MAX_SKILL * sizeof(sh_int));
      pcdata->updated = (bool*)alloc_perm (MAX_SKILL * sizeof(sh_int));
   }
   else
   {
      pcdata = pcdata_free;
      pcdata_free = pcdata_free->next;
   }
   mem_counts[NMEM_PC_DATA]++;

   learned_temp = pcdata->learned;
   learnlvl_temp = pcdata->learnlvl;
   updated_temp = pcdata->updated;
   /*
      The following line sets ALL static memory to 0's
      (even static arrays)
      The only memory that needs be initialized is
      things that are not set to NULL or 0, or was
      dynamically generated.
   */
   *pcdata = pcdata_zero;
   pcdata->learned = learned_temp;
   pcdata->learnlvl = learnlvl_temp;
   pcdata->updated = updated_temp;
   /*
      Zero out dynamically generated array
      NEED to do index 0
      NOT TAKEN CARE OF by pcdata_zero
   */
   for (cnt = 0; cnt < MAX_SKILL; cnt++)
   {
      pcdata->learned[cnt] = 0;
      pcdata->learnlvl[cnt] = 61;
      pcdata->updated[cnt] = FALSE;
   }
   /*
      pcdata_zero takes care of the following
      for (alias = 0; alias < MAX_ALIAS; alias++)
      {
         pcdata->alias[alias] = NULL;
         pcdata->alias_sub[alias] = NULL;
      }
      pcdata->house_rank   = 0;
      pcdata->house_rank2  = 0;
      pcdata->brand_rank   = 0;
      pcdata->saved_flags  = 0;
      pcdata->allied_with  = 0;
      pcdata->mark_price   = 0;
      pcdata->mark_time    = 0;
      pcdata->mark_accepted= 0;
      pcdata->last_seen_in = NULL;
      pcdata->last_death = 0;  / * never killed * /
      pcdata->last_logon = 0;
      pcdata->molting_into      = 0;
      pcdata->molting_time      = 0;
      pcdata->autopurge    = 0;
      pcdata->pk_expand    = 0;
      pcdata->surname      = NULL;
      pcdata->target = NULL;
      pcdata->bamfin = NULL;
      pcdata->bamfout = NULL;
      pcdata->title = NULL;
      pcdata->extitle = NULL;
      pcdata->imm_death = NULL;
      pcdata->ignore = NULL;
      pcdata->quest_name = NULL;
      pcdata->target = NULL;
      pcdata->spirit_room = NULL;
      pcdata->xp_pen = 0;
      pcdata->hacked = 0;
      pcdata->being_restored = FALSE;
      pcdata->called_by = NULL;
      pcdata->worship = NULL;  / * Needs to stay NULL * /
      pcdata->clan = 0;
      pcdata->email = NULL;
      pcdata->nosee_perm = NULL;
      pcdata->music_area = NULL;
      pcdata->linked = NULL;
      pcdata->gained_imm_flags   = 0;
      pcdata->gained_res_flags   = 0;
      pcdata->gained_vuln_flags  = 0;
      pcdata->gained_affects     = 0;
      pcdata->gained_affects2    = 0;
      pcdata->lost_imm_flags     = 0;
      pcdata->lost_res_flags     = 0;
      pcdata->lost_vuln_flags    = 0;
      pcdata->lost_affects       = 0;
      pcdata->lost_affects2      = 0;
      pcdata->spam               = 0;
      for (bonus = 0; bonus < 5; bonus++)
      {
         pcdata->stat_bonus[bonus] = 0;
      }
   */


   pcdata->psych_desc   = &str_empty[0];
   pcdata->dossier      = &str_empty[0];
   pcdata->marker       = &str_empty[0];
   pcdata->orig_marker  = &str_empty[0];
   pcdata->last_site    = &str_empty[0];
   pcdata->first_site   = &str_empty[0];
   pcdata->book_description = &str_empty[0];
   pcdata->buffer = new_buf();
   pcdata->pwd = &str_empty[0];;
   pcdata->old_pwd = &str_empty[0];;
   pcdata->pre_title = &str_empty[0];
   pcdata->last_voted = -1;
   pcdata->current_desc = -1;
   pcdata->clan_name = &str_empty[0];
   pcdata->worship_slot = -100;
   pcdata->moniker      = &str_empty[0];
   pcdata->condition[COND_THIRST] = 48;
   pcdata->condition[COND_HUNGER] = 48;
   for (desc_count = 0; desc_count < MAX_DESCRIPTIONS; desc_count++)
   {
      pcdata->desc_list[desc_count] = &str_empty[0];
   }
   VALIDATE(pcdata);
   return pcdata;
}


void free_pcdata(PC_DATA* pcdata)
{
   int alias;
   sh_int desc_count;

   if
   (
      !pcdata ||
      !IS_VALID(pcdata)
   )
   {
      return;
   }

   free_string(pcdata->psych_desc);
   free_string(pcdata->surname);
   free_string(pcdata->dossier);
   free_string(pcdata->first_site);
   free_string(pcdata->last_site);
   free_string(pcdata->marker);
   free_string(pcdata->orig_marker);
   free_string(pcdata->pwd);
   free_string(pcdata->bamfin);
   free_string(pcdata->bamfout);
   free_string(pcdata->title);
   free_string(pcdata->quest_name);
   free_string(pcdata->worship);
   free_string(pcdata->email);
   free_string(pcdata->nosee_perm);
   free_string(pcdata->moniker);
   free_string(pcdata->pre_title);
   free_string(pcdata->book_description);
   free_buf(pcdata->buffer);
   for (alias = 0; alias < MAX_ALIAS; alias++)
   {
      free_string(pcdata->alias[alias]);
      free_string(pcdata->alias_sub[alias]);
   }
   for (desc_count = 0; desc_count < MAX_DESCRIPTIONS; desc_count++)
   {
      free_string(pcdata->desc_list[desc_count]);
   }
   INVALIDATE(pcdata);
   mem_counts[NMEM_PC_DATA]--;
   pcdata->next = pcdata_free;
   pcdata_free = pcdata;

   return;
}




/* stuff for setting ids */
long    last_pc_id;
long    last_mob_id = 1;  /* no 0 id's */

long get_pc_id(void)
{
   int val;

   val = (current_time <= last_pc_id) ? last_pc_id + 1 : current_time;
   last_pc_id = val;
   return val;
}

long get_mob_id(void)
{
   last_mob_id++;
   return last_mob_id;
}

MEM_DATA *mem_data_free;

/* procedures and constants needed for buffering */

BUFFER *buf_free;

MEM_DATA *new_mem_data(void)
{
   MEM_DATA *memory;

   if (mem_data_free == NULL)
   memory = alloc_mem(sizeof(*memory));
   else
   {
      memory = mem_data_free;
      mem_data_free = mem_data_free->next;
   }

   memory->next = NULL;
   memory->id = 0;
   memory->reaction = 0;
   memory->when = 0;
   VALIDATE(memory);

   return memory;
}

void free_mem_data(MEM_DATA *memory)
{
   if (!memory || !IS_VALID(memory))
   return;

   memory->next = mem_data_free;
   mem_data_free = memory;
   INVALIDATE(memory);
}



/* buffer sizes */
const int buf_size[MAX_BUF_LIST] =
{
   16, 32, 64, 128, 256, 1024, 2048, 4096, 8192, 16384, 32768, 65536
};

/* local procedure for finding the next acceptable size */
/* -1 indicates out-of-boundary error */
int get_size (int val)
{
   int i;

   for (i = 0; i < MAX_BUF_LIST; i++)
   if (buf_size[i] >= val)
   {
      return buf_size[i];
   }

   return -1;
}

BUFFER *new_buf()
{
   BUFFER *buffer;
   long mem_before = sAllocPerm;

   if (buf_free == NULL){
      buffer = alloc_perm(sizeof(*buffer));
   }
   else
   {
      buffer = buf_free;
      buf_free = buf_free->next;
   }
   mem_counts[NMEM_BUFFERS]++;

   buffer->next    = NULL;
   buffer->state    = BUFFER_SAFE;
   buffer->size    = get_size(BASE_BUF);

   buffer->string    = alloc_mem(buffer->size);
   buffer->string[0]    = '\0';
   VALIDATE(buffer);
   mem_used[30] += sAllocPerm - mem_before;

   return buffer;
}

BUFFER *new_buf_size(int size)
{
   BUFFER *buffer;

   if (buf_free == NULL){
      buffer = alloc_perm(sizeof(*buffer));
   }
   else
   {
      buffer = buf_free;
      buf_free = buf_free->next;
   }
   mem_counts[NMEM_BUFFERS]++;

   buffer->next        = NULL;
   buffer->state       = BUFFER_SAFE;
   buffer->size        = get_size(size);
   if (buffer->size == -1)
   {
      bug("new_buf: buffer size %d too large.", size);
      exit(1);
   }
   buffer->string      = alloc_mem(buffer->size);
   buffer->string[0]   = '\0';
   VALIDATE(buffer);

   return buffer;
}


void free_buf(BUFFER *buffer)
{
   if (!buffer || !IS_VALID(buffer))
   return;

   free_mem(buffer->string, buffer->size);
   buffer->string = NULL;
   buffer->size   = 0;
   buffer->state  = BUFFER_FREED;
   INVALIDATE(buffer);

   mem_counts[NMEM_BUFFERS]--;
   buffer->next  = buf_free;
   buf_free      = buffer;
}


bool add_buf(BUFFER *buffer, char *string)
{
   int len;
   char *oldstr;
   int oldsize;

   oldstr = buffer->string;
   oldsize = buffer->size;

   if (buffer->state == BUFFER_OVERFLOW) /* don't waste time on bad strings! */
   return FALSE;

   len = strlen(buffer->string) + strlen(string) + 1;

   while (len >= buffer->size) /* increase the buffer size */
   {
      buffer->size     = get_size(buffer->size + 1);
      {
         if (buffer->size == -1) /* overflow */
         {
            buffer->size = oldsize;
            buffer->state = BUFFER_OVERFLOW;
            bug("buffer overflow past size %d", buffer->size);
            return FALSE;
         }
      }
   }

   if (buffer->size != oldsize)
   {
      buffer->string    = alloc_mem(buffer->size);

      strcpy(buffer->string, oldstr);
      free_mem(oldstr, oldsize);
   }

   strcat(buffer->string, string);
   return TRUE;
}


void clear_buf(BUFFER *buffer)
{
   buffer->string[0] = '\0';
   buffer->state     = BUFFER_SAFE;
}


char *buf_string(BUFFER *buffer)
{
   return buffer->string;
}



MOUNT_INFO *mountinfo_free;

MOUNT_INFO* new_mount_info( void )
{
   static MOUNT_INFO mount_info_zero;
   MOUNT_INFO* mount_info;

   if ( mountinfo_free == NULL )
   {
      mount_info = alloc_perm(sizeof(*mount_info));
   }
   else
   {
      mount_info = mountinfo_free;
      mountinfo_free = mountinfo_free->next;
   }

   mem_counts[NMEM_MOUNTS]++;  /* Fizzfaldt */
   /*
      The following line sets ALL static memory to 0's
      (even static arrays)
      The only memory that needs be initialized is
      things that are not set to NULL or 0, or was
      dynamically generated.
   */
   *mount_info = mount_info_zero;

   /*
      Taken care of by mount_info_zero
      mount_info->mount_type = 0;
      mount_info->mount = NULL;
      mount_info->mount_function = NULL;
   */

   VALIDATE(mount_info);
   return mount_info;
}

void free_mount_info( MOUNT_INFO *mount_info )
{
   if ( !IS_VALID(mount_info) )
   return;

   INVALIDATE(mount_info);
   mem_counts[NMEM_MOUNTS]--;  /* Fizzfaldt */
   mount_info->next = mountinfo_free;
   mountinfo_free = mount_info;

   return;
}

/* For use in messaging system - Mael */

MESSAGE *message_free;

MESSAGE* new_message( void )
{
   static MESSAGE message_zero;
   MESSAGE* message;

   if ( message_free == NULL )
   {
      message = alloc_perm(sizeof(*message));
   }
   else
   {
      message = message_free;
      message_free = message_free->next;
   }

   /*
      The following line sets ALL static memory to 0's
      (even static arrays)
      The only memory that needs be initialized is
      things that are not set to NULL or 0, or was
      dynamically generated.
   */
   *message = message_zero;

   /*
      taken care of by message_zero
      message->target = NULL;
      message->target_type = 0;
      message->message_type = 0;
      message->message_data = 0;
      message->message_string = NULL;
      message->message_flags = 0;
      message->next = NULL;
      message->next_to_poll = NULL;
   */

   VALIDATE(message);
   return message;
}

void free_message( MESSAGE *message )
{
   if ( !message )
   return;

   free_string( message->message_string );

   INVALIDATE(message);
   message->next = message_free;
   message_free = message;
}

/* stuff for recycling alarms */
ALARM_DATA* alarm_free;

static ALARM_DATA alarm_zero;

ALARM_DATA* new_alarm(void)
{
   ALARM_DATA* alarm;

   if (alarm_free == NULL)
   {
      alarm = alloc_perm(sizeof(*alarm));
   }
   else
   {
      alarm = alarm_free;
      alarm_free = alarm_free->next;
   }
   mem_counts[NMEM_ALARM]++;
   *alarm = alarm_zero;
   VALIDATE(alarm);
   return alarm;
}

void free_alarm(ALARM_DATA *alarm)
{
   if
   (
      !alarm ||
      !IS_VALID(alarm)
   )
   {
      return;
   }
   INVALIDATE(alarm);
   mem_counts[NMEM_ALARM]--;
   *alarm = alarm_zero;
   alarm->next = alarm_free;
   alarm_free = alarm;
}

/* stuff for recycling nodes */
NODE_DATA* node_free;

static NODE_DATA node_zero;

NODE_DATA* new_node(LIST_DATA* list)
{
   NODE_DATA* node;

   if (node_free == NULL)
   {
      node = alloc_perm(sizeof(*node));
   }
   else
   {
      node = node_free;
      node_free = node_free->next;
   }
   mem_counts[NMEM_NODE]++;
   *node = node_zero;
   VALIDATE(node);
   node->list = list;
   return node;
}

void add_node(void* data, LIST_DATA* list)
{
   NODE_DATA* node;

   if
   (
      data == NULL ||
      list == NULL
   )
   {
      return;
   }
   node = new_node(list);
   if (list->string_list)
   {
      node->data = str_dup(data);
   }
   else
   {
      node->data = data;
   }
   if (list->last)
   {
      list->last->next = node;
   }
   node->prev = list->last;
   list->last = node;
   if (!list->first)
   {
      list->first = node;
   }
   list->size++;
}

bool node_exists(void* data, LIST_DATA* list)
{
   NODE_DATA* node;

   if
   (
      data == NULL ||
      list == NULL
   )
   {
      return FALSE;
   }
   for (node = list->first; node; node = node->next)
   {
      if
      (
         node->data == data ||
         (
            list->string_list &&
            !str_cmp(data, node->data)
         )
      )
      {
         return TRUE;
      }
   }
   return FALSE;
}

void free_node(NODE_DATA* node)
{
   if
   (
      !node ||
      !IS_VALID(node)
   )
   {
      return;
   }
   if (node->list)
   {
      node->list->size--;
      if (node->list->string_list)
      {
         free_string(node->data);
      }
      if (node == node->list->first)
      {
         node->list->first = node->next;
      }
      if (node == node->list->last)
      {
         node->list->last = node->prev;
      }
   }
   if (node->prev)
   {
      node->prev->next = node->next;
   }
   if (node->next)
   {
      node->next->prev = node->prev;
   }
   INVALIDATE(node);
   mem_counts[NMEM_NODE]--;
   *node = node_zero;
   node->next = node_free;
   node_free = node;
}

void remove_node_for(void* data, LIST_DATA* list)
{
   NODE_DATA* node;
   NODE_DATA* node_next;

   if
   (
      data == NULL ||
      list == NULL
   )
   {
      return;
   }
   for (node = list->first; node; node = node_next)
   {
      node_next = node->next;
      if
      (
         node->data == data ||
         (
            list->string_list &&
            !str_cmp(data, node->data)
         )
      )
      {
         free_node(node);
      }
   }
}

/* stuff for recycling lists */
LIST_DATA* list_data_free = NULL;

static LIST_DATA list_zero;

LIST_DATA* new_list(void)
{
   LIST_DATA* list;

   if (list_data_free == NULL)
   {
      list = alloc_perm(sizeof(*list));
   }
   else
   {
      list = list_data_free;
      list_data_free = list_data_free->next;
   }
   *list = list_zero;
   VALIDATE(list);
   return list;
}

LIST_DATA* new_string_list(void)
{
   LIST_DATA* list;

   list = new_list();
   list->string_list = TRUE;
   return list;
}

void free_list(LIST_DATA* list)
{
   if
   (
      !list ||
      !IS_VALID(list)
   )
   {
      return;
   }
   while (list->first)
   {
      free_node(list->first);
   }
   INVALIDATE(list);
   *list = list_zero;
   list->next = list_data_free;
   list_data_free = list;
}
