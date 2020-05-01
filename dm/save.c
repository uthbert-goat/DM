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

static const char rcsid[] = "$Id: save.c,v 1.81 2004/11/25 08:52:20 fizzfaldt Exp $";

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <malloc.h>
#include "merc.h"
#include "recycle.h"
#include "lookup.h"
#include "tables.h"
#include "worship.h"

#if !defined(macintosh)
extern  int     _filbuf         args( (FILE *) );
#endif

void count_demons(void);

void count_demons(void)
{
   CHAR_DATA *victim;

   demon_eggs = 0;
   demon_adults = 0;
   demon_babies = 0;

   return;  /* they killed all the demons */

   for ( victim = char_list; victim != NULL; victim = victim->next )
   {
      if ( victim->in_room != NULL
      &&  IS_NPC(victim)
      &&  victim->race == 2  /* how we tell them from conjured ones */
      &&  victim->pIndexData->vnum == MOB_VNUM_DEMON1)
      {
         if (!str_cmp(victim->name, "egg"))
         demon_eggs++;
         else if (!str_cmp(victim->name, "dragkagh baby demon"))
         demon_babies++;
         else
         demon_adults++;
      }
   }
   return;
}

int rename(const char *oldfname, const char *newfname);

char *print_flags(int flag)
{
   int count, pos = 0;
   static char buf[52];


   for (count = 0; count < 32;  count++)
   {
      if (IS_SET(flag, 1<<count))
      {
         if (count < 26)
         buf[pos] = 'A' + count;
         else
         buf[pos] = 'a' + (count - 26);
         pos++;
      }
   }

   if (pos == 0)
   {
      buf[pos] = '0';
      pos++;
   }

   buf[pos] = '\0';

   return buf;
}

/*
* Array of containers read for proper re-nesting of objects.
*/
#define MAX_NEST    100
static    OBJ_DATA *    rgObjNest    [MAX_NEST];

/*
* Local functions.
*/
void     fwrite_char args( (CHAR_DATA* ch,  FILE* fp) );
void     fwrite_obj  args( (CHAR_DATA* ch,  OBJ_DATA* obj, FILE* fp, int iNest, bool mount) );
void     fwrite_pet  args( (CHAR_DATA* pet, FILE* fp, bool is_pet) );
void     fread_char  args( (CHAR_DATA* ch,  FILE* fp) );
void     fread_pet   args( (CHAR_DATA* ch,  FILE* fp, bool is_pet) );
void     fread_obj   args( (CHAR_DATA* ch,  FILE* fp) );


void save_char_obj(CHAR_DATA* ch)
{
   save_char_obj_1(ch, SAVE_ALL_PETS);
}

/*
* Save a character and inventory.
* Would be cool to save NPC's too for quest purposes,
*   some of the infrastructure is provided.
*/
void save_char_obj_1(CHAR_DATA* ch, int flags)
{
   char strsave[MAX_INPUT_LENGTH];
   FILE *fp;
   CHAR_DATA* vch;

   if ( IS_NPC(ch) )
   return;
   if (ch->level == 1)
   return;

   if ( ch->desc != NULL && ch->desc->original != NULL )
   ch = ch->desc->original;

#if defined(unix)
   /* create god log */
   /********* XUR - A fix for bad disks. ;)  We did not totally need this dir anyway.
   if (IS_IMMORTAL(ch) || ch->level >= LEVEL_IMMORTAL)
   {
   fclose(fpReserve);
   sprintf(strsave, "%s%s", GOD_DIR, capitalize(ch->name));
   if ((fp = fopen(strsave, "w")) == NULL)
   {
   bug("Save_char_obj: fopen", 0);
   perror(strsave);
   }

   fprintf(fp, "Lev %2d Trust %2d  %s%s\n",
   ch->level, get_trust(ch), ch->name, ch->pcdata->title);
   fclose(fp);
   fpReserve = fopen( NULL_FILE, "r" );
   }
   */
#endif

   fclose( fpReserve );
   sprintf(strsave, "%s%s.plr", PLAYER_DIR, capitalize(ch->name));

   if ( ( fp = fopen( TEMP_FILE, "w" ) ) == NULL )
   {
      bug( "Save_char_obj: fopen", 0 );
      perror( strsave );
   } else
   {
      fwrite_char( ch, fp );
      if ( ch->carrying != NULL )
      fwrite_obj( ch, ch->carrying, fp, 0, FALSE );
      if ( ch->is_riding
      && ch->is_riding->mount_type == MOUNT_OBJECT
      && IS_SET(((OBJ_DATA *)ch->is_riding->mount)->mount_specs->mount_info_flags, MOUNT_INFO_INVENTORY)
      && ((OBJ_DATA *)ch->is_riding->mount)->in_room == ch->in_room )
      fwrite_obj( ch, (OBJ_DATA *)ch->is_riding->mount, fp, 0, TRUE );
      /* save the pets */
      if
      (
         ch->pet != NULL &&
         (
            /* Untill online, pets aren't in same room */
            !ch->on_line ||
            !ch->pet->on_line ||
            ch->pet->in_room == ch->in_room
         )
      )
      {
         fwrite_pet(ch->pet, fp, TRUE);
      }
      if
      (
         IS_SET(flags, SAVE_ALL_PETS) &&
         ch->in_room != NULL
      )
      {
         for (vch = char_list; vch; vch = vch->next)
         {
            if
            (
               !IS_NPC(vch) ||
               (
                  /* Untill online, pets aren't in same room */
                  ch->on_line &&
                  vch->on_line &&
                  vch->in_room != ch->in_room
               ) ||
               vch->master != ch ||
               ch->pet == vch ||  /* Already did pet */
               !IS_AFFECTED(vch, AFF_CHARM)
            )
            {
               continue;
            }
            fwrite_pet(vch, fp, FALSE);
         }
      }
      fprintf( fp, "#END\n" );
   }
   fclose(fp);
   rename(TEMP_FILE, strsave);
   fpReserve = fopen( NULL_FILE, "r" );
   return;
}



/*
* Write the char.
*/
void fwrite_char(CHAR_DATA* ch, FILE* fp)
{
   AFFECT_DATA* paf;
   int sn;
   int gn;
   int pos;
   sh_int counter;

   fprintf(fp, IS_NPC(ch) ? "#MOB\n" : "#PLAYER\n");
   fprintf(fp, "Name %s~\n", ch->name);
   if (ch->pcdata->surname != NULL)
   {
      fprintf(fp, "SurName %s~\n", ch->pcdata->surname);
   }
   if (ch->pcdata->moniker[0] != '\0')
   {
      fprintf(fp, "Moniker %s~\n", ch->pcdata->moniker);
   }
   fprintf(fp, "Id   %ld\n", ch->id);
   fprintf(fp, "AutoPurge %d\n", ch->pcdata->autopurge);
   /*
   Leave House, Clan, LogO, probation /level/trust
   near top to make clan/house/lastlogon faster
   */
   if (ch->house)
   {
      fprintf(fp, "Hous %s~\n", house_table[ch->house].name);
   }
   if (ch->pcdata->clan)
   {
      fprintf(fp, "Clan %d %s~\n", ch->pcdata->clan, ch->pcdata->clan_name);
   }
   if (ch->act != 0)
   {
      fprintf(fp, "Act  %s\n", print_flags(ch->act));
   }
   if (ch->act2 != 0)
   {
      fprintf(fp, "Act2 %s\n", print_flags(ch->act2));
   }
   /* Trust stays next line after level always */
   fprintf(fp, "Levl %d\n", ch->level);
   if (ch->trust != 0)
   {
      fprintf(fp, "Tru  %d\n", ch->trust);
   }
   if (ch->pcdata->email)
   {
      fprintf(fp, "Email  %s~\n", ch->pcdata->email);
   }
   if
   (
      IS_IMMORTAL(ch) &&
      ch->pcdata->worship_slot != -100
   )
   {
      fprintf(fp, "WSlot %d\n", ch->pcdata->worship_slot);
   }
#if defined(MSDOS)
   fprintf
   (
      fp,
      "LogO %d\n",
      (
         ch->pcdata->last_logon ?
         ch->pcdata->last_logon :
         current_time
      )
   );
   fprintf(fp, "Prob %d\n", ch->pcdata->probation + current_time - ch->logon);
   fprintf(fp, "ProbTime %d\n", ch->pcdata->prob_time);
   fprintf
   (
      fp,
      "Not  %d %d %d %d %d %d %d %d %d\n",
      ch->pcdata->last_note,
      ch->pcdata->last_idea,
      ch->pcdata->last_penalty,
      ch->pcdata->last_news,
      ch->pcdata->last_changes,
      ch->pcdata->last_bank,
      ch->pcdata->last_death_note,
      ch->pcdata->last_clan,
      ch->pcdata->last_crim
   );
   fprintf(fp, "Spam %d\n", ch->pcdata->spam);
#else
   fprintf
   (
      fp,
      "LogO %ld\n",
      (
         ch->pcdata->last_logon ?
         ch->pcdata->last_logon :
         current_time
      )
   );
   fprintf(fp, "Prob %ld\n", ch->pcdata->probation + current_time - ch->logon);
   fprintf(fp, "ProbTime %ld\n", ch->pcdata->prob_time);
   fprintf
   (
      fp,
      "Notes  %ld %ld %ld %ld %ld %ld %ld %ld %ld\n",
      ch->pcdata->last_note,
      ch->pcdata->last_idea,
      ch->pcdata->last_penalty,
      ch->pcdata->last_news,
      ch->pcdata->last_changes,
      ch->pcdata->last_bank,
      ch->pcdata->last_death_note,
      ch->pcdata->last_clan,
      ch->pcdata->last_crim
   );
   fprintf(fp, "Spam %ld\n", ch->pcdata->spam);
#endif

   fprintf(fp, "Vers %d\n", ch->version);
   for (sn = 0; sn < MAX_HOUSE; sn++)
   {
      if
      (
         (
            ch->pcdata->deposits[sn] ||
            ch->pcdata->life_deposits[sn]
         ) &&
         house_table[sn].name &&
         house_table[sn].name[0] != '\0'
      )
      {
         fprintf
         (
            fp,
            "HDeposits %s %d %d\n",
            house_table[sn].name,
            ch->pcdata->deposits[sn],
            ch->pcdata->life_deposits[sn]
         );
      }
   }
   if
   (
      ch->pcdata->clan &&
      (
         ch->pcdata->clan_deposits ||
         ch->pcdata->clan_life_deposits
      )
   )
   {
      fprintf
      (
         fp,
         "CDeposits %d %d %d\n",
         ch->pcdata->clan,
         ch->pcdata->clan_deposits,
         ch->pcdata->clan_life_deposits
      );
   }
   fprintf(fp, "Elec %d\n", ch->pcdata->last_voted);
   for (counter = 0; counter < 5; counter++)
   {
      if (ch->pcdata->stat_bonus[counter])
      {
         fprintf
         (
            fp,
            "Bonus %d %d %d %d %d\n",
            ch->pcdata->stat_bonus[0],
            ch->pcdata->stat_bonus[1],
            ch->pcdata->stat_bonus[2],
            ch->pcdata->stat_bonus[3],
            ch->pcdata->stat_bonus[4]
         );
         break;
      }
   }
   if
   (
      ch->pcdata->nosee_perm != NULL &&
      ch->pcdata->nosee_perm[0] != '\0'
   )
   {
      fprintf(fp, "NsP  %s~\n", ch->pcdata->nosee_perm);
   }
   if (ch->short_descr[0] != '\0')
   {
      fprintf( fp, "ShD  %s~\n", ch->short_descr);
   }
   if (ch->long_descr[0] != '\0')
   {
      fprintf(fp, "LnD  %s~\n", ch->long_descr);
   }
   if (ch->description[0] != '\0')
   {
      fprintf(fp, "Desc %s~\n", ch->description);
   }
   if (ch->pcdata->current_desc != -1)
   {
      sh_int desc_count;

      fprintf
      (
         fp,
         "DescCurrent %d\n",
         ch->pcdata->current_desc
      );
      for (desc_count = 0; desc_count < MAX_DESCRIPTIONS; desc_count++)
      {
         if (ch->pcdata->desc_list[desc_count][0] != '\0')
         {
            fprintf
            (
               fp,
               "DescMult %d\n%s~\n",
               desc_count,
               ch->pcdata->desc_list[desc_count]
            );
         }
      }
   }
   if (ch->pcdata->psych_desc[0] != '\0')
   {
      fprintf(fp, "PDesc %s~\n", ch->pcdata->psych_desc);
   }
   if
   (
      ch->race == grn_book &&
      ch->pcdata->book_description[0] != '\0'
   )
   {
      fprintf(fp, "BDesc %s~\n", ch->pcdata->book_description);
   }
   if (ch->pcdata->dossier[0] != '\0')
   {
      fprintf(fp, "Dossier %s~\n", ch->pcdata->dossier);
   }
   if
   (
      ch->prompt != NULL ||
      !str_cmp(ch->prompt, "<%hhp %mmn %vmv> ")
   )
   {
      fprintf(fp, "Prom %s~\n", ch->prompt);
   }
   fprintf(fp, "Race %s~\n", pc_race_table[ch->race].name);
   if (ch->subrace != 0)
   {
      fprintf(fp, "Subrace %d\n", ch->subrace);
   }
   if (ch->pcdata->race_lottery)
   {
      fprintf
      (
         fp,
         "RaceLottery %s~\n",
         pc_race_table[ch->pcdata->race_lottery].name
      );
   }
   if (ch->pcdata->molting_into != 0)
   {
      fprintf(fp, "Molt %d\n", ch->pcdata->molting_into);
      fprintf(fp, "Molttime %d\n", ch->pcdata->molting_time);
   }
   fprintf(fp, "Sex  %d\n", ch->sex);
   fprintf(fp, "Cla  %d\n", ch->class);
   fprintf(fp, "Plyd %d\n", ch->played + (int)(current_time - ch->logon));
   if (!IS_NPC(ch))
   {
      fprintf(fp, "Agemod %d\n", ch->pcdata->age_mod);
      if (ch->pcdata->death_status == HAS_DIED)
      {
         fprintf(fp, "Died %d\n", ch->pcdata->death_status);
         fprintf(fp, "Dtime %d\n", ch->pcdata->death_timer);
      }
      fprintf(fp, "Ghost %d\n", ch->ghost);
   }
   fprintf(fp, "Scro %d\n", ch->lines);

   /* temples for hometowns */
   fprintf
   (
      fp,
      "Temple %d\n",
      (ch->temple == 0) ?
      TEMPLE_GLYNDANE_N :
      ch->temple
   );

   fprintf
   (
      fp, "Room %d\n",
      (
         ch->in_room == get_room_index(ROOM_VNUM_LIMBO) &&
         ch->was_in_room != NULL
      ) ?
      ch->was_in_room->vnum :
      ch->in_room == NULL ?
      3001 :
      ch->in_room->vnum
   );

   fprintf
   (
      fp,
      "HMV  %d %d %d %d %d %d\n",
      ch->hit,
      ch->max_hit,
      ch->mana,
      ch->max_mana,
      ch->move,
      ch->max_move
   );
   fprintf(fp, "Noregen %d\n", ch->noregen_dam);
   if (!IS_NPC(ch))
   {
      fprintf(fp, "Deaths %d\n", ch->pcdata->death_count);
      fprintf(fp, "TargetKills %ld\n", ch->pcdata->targetkills);
      if (ch->pcdata->last_site != NULL)
      {
         fprintf(fp, "LastSite %s~\n", ch->pcdata->last_site);
      }
      if (ch->pcdata->marker != NULL)
      {
         fprintf(fp, "Marker %s~\n", ch->pcdata->marker);
      }
      if (ch->pcdata->orig_marker != NULL)
      {
         fprintf(fp, "OMarker %s~\n", ch->pcdata->orig_marker);
      }
      if (ch->pcdata->first_site != NULL)
      {
         fprintf(fp, "FirstSite %s~\n", ch->pcdata->first_site);
      }
      if (ch->pcdata->pk_expand != 0)
      {
         fprintf(fp, "PkExpand %d\n", (int)ch->pcdata->pk_expand);
      }
      if (ch->pcdata->hacked != 0)
      {
         fprintf(fp, "HackAttempts %ld\n", (long int)ch->pcdata->hacked);
      }
      if (ch->pcdata->house_rank != 0)
      {
         fprintf(fp, "Houserank %ld\n", (long int)ch->pcdata->house_rank);
      }
      if (ch->pcdata->house_rank2 != 0)
      {
         fprintf(fp, "Houserank2 %ld\n", (long int)ch->pcdata->house_rank2);
      }
      if (ch->pcdata->brand_rank != 0)
      {
         fprintf(fp, "Brandrank %ld\n", (long int)ch->pcdata->brand_rank);
      }
      if (ch->pcdata->mark_price != 0)
      {
         fprintf(fp, "Markprice %ld\n", (long int)ch->pcdata->mark_price);
         fprintf(fp, "Markaccepted %d\n", (int)ch->pcdata->mark_accepted);
      }
      if (ch->pcdata->saved_flags != 0)
      {
         fprintf(fp, "Savedflags %ld\n", (long int)ch->pcdata->saved_flags);
      }
      if (ch->pcdata->last_death != 0)
      {
#if defined(MSDOS)
         fprintf(fp, "LastDeath %d\n", ch->pcdata->last_death);
#else
         fprintf(fp, "LastDeath %ld\n", ch->pcdata->last_death);
#endif
      }
      if (ch->pcdata->mark_time != 0)
      {
#if defined(MSDOS)
         fprintf(fp, "MarkTime %d\n", ch->pcdata->mark_time);
#else
         fprintf(fp, "MarkTime %ld\n", ch->pcdata->mark_time);
#endif
      }
      fprintf(fp, "ExpPen %ld\n", ch->pcdata->xp_pen);
      fprintf(fp, "Specialize %d\n", ch->pcdata->special);
      fprintf(fp, "Rebels %d\n", ch->pcdata->rebels);
      fprintf(fp, "Wanteds %d\n", ch->pcdata->wanteds);

   }
   fprintf(fp, "Gold %ld\n", ch->gold);
   fprintf(fp, "Silv %ld\n", ch->silver);
   fprintf(fp, "Bgold %ld\n", ch->gold_bank);
   fprintf(fp, "Bsilv %ld\n", ch->silver_bank);
   fprintf(fp, "Exp  %d\n", ch->exp);
   fprintf(fp, "ExpT %d\n", ch->exp_total);
   if (ch->affected_by != 0)
   {
      fprintf(fp, "AfBy %s\n", print_flags(ch->affected_by));
   }
   if (ch->affected_by2 != 0)
   {
      fprintf(fp, "AfBy2 %s\n", print_flags(ch->affected_by2));
   }
   if (ch->pcdata->gained_imm_flags != 0)
   {
      fprintf(fp, "PGI %s\n", print_flags(ch->pcdata->gained_imm_flags));
   }
   if (ch->pcdata->gained_res_flags != 0)
   {
      fprintf(fp, "PGR %s\n", print_flags(ch->pcdata->gained_res_flags));
   }
   if (ch->pcdata->gained_vuln_flags != 0)
   {
      fprintf(fp, "PGV %s\n", print_flags(ch->pcdata->gained_vuln_flags));
   }
   if (ch->pcdata->gained_affects != 0)
   {
      fprintf(fp, "PGA %s\n", print_flags(ch->pcdata->gained_affects));
   }
   if (ch->pcdata->gained_affects2 != 0)
   {
      fprintf(fp, "PGA2 %s\n", print_flags(ch->pcdata->gained_affects2));
   }
   if (ch->pcdata->lost_imm_flags != 0)
   {
      fprintf(fp, "PLI %s\n", print_flags(ch->pcdata->lost_imm_flags));
   }
   if (ch->pcdata->lost_res_flags != 0)
   {
      fprintf(fp, "PLR %s\n", print_flags(ch->pcdata->lost_res_flags));
   }
   if (ch->pcdata->lost_vuln_flags != 0)
   {
      fprintf(fp, "PLV %s\n", print_flags(ch->pcdata->lost_vuln_flags));
   }
   if (ch->pcdata->lost_affects != 0)
   {
      fprintf(fp, "PLA %s\n", print_flags(ch->pcdata->lost_affects));
   }
   if (ch->pcdata->lost_affects2 != 0)
   {
      fprintf(fp, "PLA2 %s\n", print_flags(ch->pcdata->lost_affects2));
   }
   if (ch->affected_by2 != 0)
   {
      fprintf(fp, "AfBy2 %s\n", print_flags(ch->affected_by2));
   }

   if (ch->air_loss != 0)
   {
      fprintf(fp, "Air %d\n", ch->air_loss);
   }
   fprintf(fp, "Comm %s\n", print_flags(ch->comm));
   fprintf(fp, "Comm2 %s\n", print_flags(ch->comm2));
   if (ch->wiznet)
   {
      fprintf(fp, "Wizn %s\n", print_flags(ch->wiznet));
   }

   if (ch->wiznet2)
   {
      fprintf(fp, "Wizn2 %s\n", print_flags(ch->wiznet2));
   }

   if
   (
      ch->pcdata->worship &&
      ch->pcdata->worship[0] != '\0'
   )
   {
      fprintf(fp, "Worship %s~\n", ch->pcdata->worship);
   }
   if (ch->invis_level)
   {
      fprintf(fp, "Invi %d\n", ch->invis_level);
   }
   if (ch->incog_level)
   {
      fprintf(fp, "Inco %d\n", ch->incog_level);
   }
   fprintf
   (
      fp,
      "Pos  %d\n",
      ch->position == POS_FIGHTING ?
      POS_STANDING :
      ch->position
   );
   if (ch->practice != 0)
   {
      fprintf(fp, "Prac %d\n", ch->practice);
   }
   if (ch->train != 0)
   {
      fprintf(fp, "Trai %d\n", ch->train);
   }
   if (ch->spell_power != 0)
   {
      fprintf(fp, "SpellPower  %d\n", ch->spell_power);
   }
   if (ch->holy_power != 0)
   {
      fprintf(fp, "HolyPower  %d\n", ch->holy_power);
   }
   if (ch->sight != 0)
   {
      fprintf(fp, "Sight  %d\n", ch->sight);
   }
   if (ch->saving_throw != 0)
   {
      fprintf(fp, "Save  %d\n", ch->saving_throw);
   }
   if (ch->saving_spell != 0)
   {
      fprintf(fp, "SaveSpell %d\n", ch->saving_spell);
   }
   if (ch->saving_breath != 0)
   {
      fprintf(fp, "SaveBreath %d\n", ch->saving_breath);
   }
   if (ch->saving_maledict != 0)
   {
      fprintf(fp, "SaveMaledict %d\n", ch->saving_maledict);
   }
   if (ch->saving_transport != 0)
   {
      fprintf(fp, "SaveTransport %d\n", ch->saving_transport);
   }
   fprintf(fp, "Alig  %d\n", ch->alignment);
   fprintf(fp, "TAlig %d\n", ch->true_alignment);
   fprintf(fp, "Etho  %d\n", ch->pcdata->ethos);
   fprintf(fp, "Indu  %d\n", ch->pcdata->induct);
   if (ch->hitroll != 0)
   {
      fprintf(fp, "Hit   %d\n", ch->hitroll);
   }
   if (ch->damroll != 0)
   {
      fprintf(fp, "Dam   %d\n", ch->damroll);
   }
   fprintf
   (
      fp,
      "ACs %d %d %d %d\n",
      ch->armor[0],
      ch->armor[1],
      ch->armor[2],
      ch->armor[3]
   );
   if (ch->wimpy != 0)
   {
      fprintf(fp, "Wimp  %d\n", ch->wimpy);
   }
   fprintf
   (
      fp,
      "Attr %d %d %d %d %d\n",
      ch->perm_stat[STAT_STR],
      ch->perm_stat[STAT_INT],
      ch->perm_stat[STAT_WIS],
      ch->perm_stat[STAT_DEX],
      ch->perm_stat[STAT_CON]
   );
   fprintf
   (
      fp,
      "AMod %d %d %d %d %d\n",
      ch->mod_stat[STAT_STR],
      ch->mod_stat[STAT_INT],
      ch->mod_stat[STAT_WIS],
      ch->mod_stat[STAT_DEX],
      ch->mod_stat[STAT_CON]
   );
   if (IS_NPC(ch))
   {
      fprintf(fp, "Vnum %d\n", ch->pIndexData->vnum);
   }
   else
   {
      if
      (
         ch->pcdata->old_pwd &&
         ch->pcdata->old_pwd[0] != '\0'
      )
      {
         /* crypt*/
         fprintf(fp, "Pass %s~\n", ch->pcdata->old_pwd);
      }
      if
      (
         ch->pcdata->pwd &&
         ch->pcdata->pwd[0] != '\0'
      )
      {
         /* md5 */
         fprintf(fp, "PassMD5 %s~\n", ch->pcdata->pwd);
      }
      if (ch->pcdata->bamfin[0] != '\0')
      {
         fprintf(fp, "Bin  %s~\n", ch->pcdata->bamfin);
      }
      if (ch->pcdata->bamfout[0] != '\0')
      {
         fprintf(fp, "Bout %s~\n", ch->pcdata->bamfout);
      }
      fprintf(fp, "Titl %s~\n", ch->pcdata->title);
      if (ch->pcdata->extitle)
      {
         fprintf(fp, "EXTitl %s~\n", ch->pcdata->extitle);
      }
      if (ch->pcdata->pre_title[0] != '\0')
      {
         fprintf(fp, "PreTitl %s~\n", ch->pcdata->pre_title);
      }

      fprintf(fp, "TSex %d\n", ch->pcdata->true_sex);
      fprintf(fp, "LLev %d\n", ch->pcdata->last_level);
      fprintf
      (
         fp,
         "HMVP %d %d %d\n",
         ch->pcdata->perm_hit,
         ch->pcdata->perm_mana,
         ch->pcdata->perm_move
      );
      fprintf
      (
         fp,
         "Cnd  %d %d %d %d %d %d\n",
         ch->pcdata->condition[0],
         ch->pcdata->condition[1],
         ch->pcdata->condition[2],
         ch->pcdata->condition[3],
         ch->pcdata->condition[4],
         ch->pcdata->condition[5]
      );

      /* write alias */
      for (pos = 0; pos < MAX_ALIAS; pos++)
      {
         if
         (
            ch->pcdata->alias[pos] == NULL ||
            ch->pcdata->alias_sub[pos] == NULL
         )
         {
            break;
         }
         fprintf
         (
            fp,
            "Alias %s %s~\n",
            ch->pcdata->alias[pos],
            ch->pcdata->alias_sub[pos]
         );
      }

      for (sn = 1; sn < MAX_SKILL; sn++)
      {
         if
         (
            skill_table[sn].name != NULL &&
            (
               ch->pcdata->learned[sn] > 0 ||
               ch->pcdata->learned[sn] == -1
            )
         )
         {
            fprintf
            (
               fp,
               "Sk2 %d %d %d '%s'\n",
               ch->pcdata->updated[sn],
               ch->pcdata->learned[sn],
               ch->pcdata->learnlvl[sn],
               skill_table[sn].name
            );
         }
      }

      for (gn = 0; gn < MAX_GROUP; gn++)
      {
         if
         (
            group_table[gn].name != NULL &&
            ch->pcdata->group_known[gn]
         )
         {
            fprintf(fp, "Gr '%s'\n", group_table[gn].name);
         }
      }
   }

   for (paf = ch->affected; paf != NULL; paf = paf->next)
   {
      if
      (
         paf->type < 1 ||
         paf->type >= MAX_SKILL
      )
      {
         continue;
      }

      fprintf
      (
         fp,
         "Affb '%s' %3d %3d %3d %3d %3d %10d %10d\n",
         skill_table[paf->type].name,
         paf->where,
         paf->level,
         paf->duration,
         paf->modifier,
         paf->location,
         paf->bitvector,
         paf->bitvector2
      );
   }

   fprintf(fp, "End\n\n");
   return;
}

/* write a pet */
void fwrite_pet(CHAR_DATA *pet, FILE *fp, bool is_pet)
{
   AFFECT_DATA *paf;

   if (is_pet)
   {
      fprintf(fp, "#PET\n");
   }
   else
   {
      fprintf(fp, "#CHARMIE\n");
   }

   fprintf(fp, "Vnum %d\n", pet->pIndexData->vnum);

   fprintf(fp, "Name %s~\n", pet->name);
#if defined(MSDOS)
   fprintf(fp, "LogO %d\n", current_time);
#else
   fprintf(fp, "LogO %ld\n", current_time);
#endif
   if (pet->short_descr != pet->pIndexData->short_descr)
   fprintf(fp, "ShD  %s~\n", pet->short_descr);
   if (pet->long_descr != pet->pIndexData->long_descr)
   fprintf(fp, "LnD  %s~\n", pet->long_descr);
   if (pet->description != pet->pIndexData->description)
   fprintf(fp, "Desc %s~\n", pet->description);
   if (pet->race != pet->pIndexData->race)
   fprintf(fp, "Race %s~\n", race_table[pet->race].name);
   if (pet->house)
   fprintf( fp, "Hous %s~\n", house_table[pet->house].name);
   fprintf(fp, "Sex  %d\n", pet->sex);
   if (pet->level != pet->pIndexData->level)
   fprintf(fp, "Levl %d\n", pet->level);
   fprintf(fp, "HMV  %d %d %d %d %d %d\n",
   pet->hit, pet->max_hit, pet->mana, pet->max_mana, pet->move, pet->max_move);
   if (pet->gold > 0)
   fprintf(fp, "Gold %ld\n", pet->gold);
   if (pet->silver > 0)
   fprintf(fp, "Silv %ld\n", pet->silver);
   if (pet->exp > 0)
   fprintf(fp, "Exp  %d\n", pet->exp);
   if (pet->act != pet->pIndexData->act)
   fprintf(fp, "Act  %s\n", print_flags(pet->act));
   if (pet->off_flags != pet->pIndexData->off_flags)
   {
      fprintf(fp, "Off  %s\n", print_flags(pet->off_flags));
   }
   if (pet->affected_by != pet->pIndexData->affected_by)
   fprintf(fp, "AfBy %s\n", print_flags(pet->affected_by));
   if (pet->affected_by2 != pet->pIndexData->affected_by2)
   fprintf(fp, "AfBy2 %s\n", print_flags(pet->affected_by2));
   if (pet->comm != 0)
   if (pet->air_loss != 0)
   {
      fprintf(fp, "Air %d\n", pet->air_loss);
   }
   fprintf(fp, "Comm %s\n", print_flags(pet->comm));
   if (pet->comm2 != 0)
   fprintf(fp, "Comm2 %s\n", print_flags(pet->comm2));
   fprintf(fp, "Pos  %d\n", pet->position = POS_FIGHTING ? POS_STANDING : pet->position);
   if (pet->spell_power != 0)
   fprintf(fp, "SpellPower %d\n", pet->spell_power);
   if (pet->holy_power != 0)
   fprintf(fp, "HolyPower %d\n", pet->holy_power);
   if (pet->sight != 0)
   {
      fprintf(fp, "Sight %d\n", pet->sight);
   }
   if (pet->saving_throw != 0)
   fprintf(fp, "Save %d\n", pet->saving_throw);
   if (pet->saving_spell != 0)
   fprintf(fp, "SaveSpell %d\n", pet->saving_spell);
   if (pet->saving_maledict != 0)
   fprintf(fp, "SaveMaledict %d\n", pet->saving_maledict);
   if (pet->saving_transport != 0)
   fprintf(fp, "SaveTransport %d\n", pet->saving_transport);
   if (pet->saving_breath != 0)
   fprintf(fp, "SaveBreath %d\n", pet->saving_breath);
   if (pet->alignment != pet->pIndexData->alignment)
   fprintf(fp, "Alig %d\n", pet->alignment);
   if (pet->true_alignment != -5000)
   fprintf(fp, "TAlig %d\n", pet->true_alignment);
   if (pet->hitroll != pet->pIndexData->hitroll)
   fprintf(fp, "Hit  %d\n", pet->hitroll);
   if (pet->damroll != pet->pIndexData->damage[DICE_BONUS])
   fprintf(fp, "Dam  %d\n", pet->damroll);
   fprintf(fp, "ACs  %d %d %d %d\n",
   pet->armor[0], pet->armor[1], pet->armor[2], pet->armor[3]);
   fprintf(fp, "Attr %d %d %d %d %d\n",
   pet->perm_stat[STAT_STR], pet->perm_stat[STAT_INT],
   pet->perm_stat[STAT_WIS], pet->perm_stat[STAT_DEX],
   pet->perm_stat[STAT_CON]);
   fprintf(fp, "AMod %d %d %d %d %d\n",
   pet->mod_stat[STAT_STR], pet->mod_stat[STAT_INT],
   pet->mod_stat[STAT_WIS], pet->mod_stat[STAT_DEX],
   pet->mod_stat[STAT_CON]);

   for ( paf = pet->affected; paf != NULL; paf = paf->next )
   {
      if (paf->type < 1 || paf->type >= MAX_SKILL)
      continue;

      fprintf(fp, "Affb '%s' %3d %3d %3d %3d %3d %10d %10d\n",
      skill_table[paf->type].name,
      paf->where, paf->level, paf->duration, paf->modifier, paf->location,
      paf->bitvector, paf->bitvector2);
   }

   fprintf(fp, "End\n\n");
   return;
}

/*
* Write an object and its contents.
*/
void fwrite_obj( CHAR_DATA *ch, OBJ_DATA *obj, FILE *fp, int iNest, bool mount )
{
   EXTRA_DESCR_DATA *ed;
   AFFECT_DATA *paf;
   sh_int cnt;

   /*
   * Slick recursion to write lists backwards,
   *   so loading them will load in forwards order.
   */
   if ( !mount
   && obj->next_content != NULL )
   fwrite_obj( ch, obj->next_content, fp, iNest, FALSE );

   /*
   * Castrate storage characters.
   */
   if ( obj->item_type == ITEM_KEY
   ||   (obj->item_type == ITEM_MAP && !obj->value[0]))
   return;

   /*
   Never ever save house items  -Wervdon
   or lich phylacteries -Fizz
   */
   if
   (
      obj->pIndexData->house ||
      obj->pIndexData->vnum == OBJ_VNUM_PHYLACTERY
   )
   {
      return;
   }

   fprintf( fp, "#O\n" );
   fprintf( fp, "Vnum %d\n",   obj->pIndexData->vnum        );
   if (!obj->pIndexData->new_format)
   fprintf( fp, "Oldstyle\n");
   if (obj->enchanted)
   fprintf( fp, "Enchanted\n");
   fprintf( fp, "Nest %d\n",    iNest               );

   /* these data are only used if they do not match the defaults */

   if ( obj->name != obj->pIndexData->name)
   fprintf( fp, "Name %s~\n",    obj->name             );
   if ( obj->short_descr != obj->pIndexData->short_descr)
   fprintf( fp, "ShD  %s~\n",    obj->short_descr         );
   if ( obj->material != obj->pIndexData->material)
   fprintf( fp, "Mat %s~\n",    obj->material            );
   if ( obj->description != obj->pIndexData->description)
   fprintf( fp, "Desc %s~\n",    obj->description         );
   for (cnt = 0; cnt < MAX_OWNERS; cnt++)
   {
      if (obj->prev_owners[cnt][0] != '\0')
      {
         fprintf(fp, "PrvO %d %s~\n", cnt, obj->prev_owners[cnt]);
         if (obj->prev_owners_site[cnt][0] != '\0')
         {
            fprintf(fp, "PrvS %d %s~\n", cnt, obj->prev_owners_site[cnt]);
         }
         fprintf(fp, "PrvI %d %ld\n", cnt, obj->prev_owners_id[cnt]);
      }
      else
      {
         break;
      }
   }
   if (str_cmp(obj->owner, ""))
   fprintf( fp, "Owner %s~\n",    obj->owner            );

   if ( obj->extra_flags != obj->pIndexData->extra_flags)
   fprintf( fp, "ExtF %ld\n",       obj->extra_flags             );
   if ( obj->extra_flags2 != obj->pIndexData->extra_flags2)
   fprintf( fp, "ExtF2 %ld\n",      obj->extra_flags2            );
   if ( obj->wear_flags != obj->pIndexData->wear_flags)
   fprintf( fp, "WeaF %d\n",    obj->wear_flags             );
   if ( obj->item_type != obj->pIndexData->item_type)
   fprintf( fp, "Ityp %d\n",    obj->item_type             );
   if ( obj->weight != obj->pIndexData->weight)
   fprintf( fp, "Wt   %d\n",    obj->weight             );
   if ( obj->condition != obj->pIndexData->condition)
   fprintf( fp, "Cond %d\n",    obj->condition             );

   /* variable data */

   fprintf( fp, "Wear %d\n",   obj->wear_loc                );
   if (obj->level != obj->pIndexData->level)
   fprintf( fp, "Lev  %d\n",    obj->level             );

   /*    if (obj->pIndexData->vnum == OBJ_VNUM_SACK &&
   obj->timer == 0 &&
   str_cmp(obj->name, "sack"))
   obj->timer = obj->level * 8;
   */
   if (obj->timer != 0)
   fprintf( fp, "Time %d\n",    obj->timer         );
   fprintf( fp, "Cost %d\n",    obj->cost             );
   if (obj->value[0] != obj->pIndexData->value[0]
   ||  obj->value[1] != obj->pIndexData->value[1]
   ||  obj->value[2] != obj->pIndexData->value[2]
   ||  obj->value[3] != obj->pIndexData->value[3]
   ||  obj->value[4] != obj->pIndexData->value[4])
   fprintf( fp, "Val  %d %d %d %d %d\n",
   obj->value[0], obj->value[1], obj->value[2], obj->value[3],
   obj->value[4]         );

   switch ( obj->item_type )
   {
      case (ITEM_CORPSE_NPC):
      case (ITEM_CORPSE_PC):
      {
         if (obj->value[0] > 0)
         {
            fprintf(fp, "CorpseRace %s~\n", race_table[obj->value[0]].name);
         }
         break;
      }
      case (ITEM_BOOK):
      {
         int cnt;

         for (cnt = 0; cnt < obj->value[0]; cnt++)
         {
            if
            (
               (
                  (
                     obj->book_info->title[cnt]
                  ) != obj->pIndexData->book_info->title[cnt] &&
                  str_cmp
                  (
                     obj->book_info->title[cnt],
                     obj->pIndexData->book_info->title[cnt]
                  )
               ) ||
               (
                  (
                     obj->book_info->page[cnt]
                  ) != obj->pIndexData->book_info->page[cnt] &&
                  str_cmp
                  (
                     obj->book_info->page[cnt],
                     obj->pIndexData->book_info->page[cnt]
                  )
               )
            )
            {
               /* In here only if the page and or title are different */
               fprintf
               (
                  fp,
                  "Page %d %s~%s~\n",
                  cnt + 1,
                  obj->book_info->title[cnt],
                  obj->book_info->page[cnt]
               );
            }
         }
         break;
      }
      case (ITEM_PEN):
      case (ITEM_INKWELL):
      {
         fprintf
         (
            fp,
            "Pen '%s' '%s'\n",
            pencolor_table[obj->value[0]],
            inkwell_table[obj->value[2]]
         );
         break;
      }
      case ITEM_POTION:
      case ITEM_SCROLL:
      case ITEM_PILL:
      case ITEM_HERB:
      if ( obj->value[1] > 0 )
      {
         fprintf( fp, "Spell 1 '%s'\n",
         skill_table[obj->value[1]].name );
      }

      if ( obj->value[2] > 0 )
      {
         fprintf( fp, "Spell 2 '%s'\n",
         skill_table[obj->value[2]].name );
      }

      if ( obj->value[3] > 0 )
      {
         fprintf( fp, "Spell 3 '%s'\n",
         skill_table[obj->value[3]].name );
      }

      break;

      case ITEM_STAFF:
      case ITEM_WAND:
      if ( obj->value[3] > 0 )
      {
         fprintf( fp, "Spell 3 '%s'\n",
         skill_table[obj->value[3]].name );
      }

      break;
   }

   for ( paf = obj->affected; paf != NULL; paf = paf->next )
   {
      /* Include index 0 (reserved) */
      if (paf->type < 0 || paf->type >= MAX_SKILL)
      {
         sprintf
         (
            log_buf,
            "fwrite_obj: bad affect: %d in [%s] [%d]",
            paf->type,
            obj->short_descr,
            obj->pIndexData->vnum
         );
         bug(log_buf, 0);
         continue;
      }
      fprintf
      (
         fp, "Affb '%s' %3d %3d %3d %3d %3d %10d %10d\n",
         skill_table[paf->type].name,
         paf->where,
         paf->level,
         paf->duration,
         paf->modifier,
         paf->location,
         paf->bitvector,
         paf->bitvector2
      );
   }

   for ( ed = obj->extra_descr; ed != NULL; ed = ed->next )
   {
      fprintf( fp, "ExDe %s~ %s~\n",
      ed->keyword, ed->description );
   }

   fprintf( fp, "End\n\n" );

   if ( obj->contains != NULL )
   fwrite_obj( ch, obj->contains, fp, iNest + 1, FALSE );

   return;
}

/*
* Load a char and inventory into a new ch structure, from storage
*/
bool load_char_obj2( DESCRIPTOR_DATA *d, char *name )
{
   char strsave[MAX_INPUT_LENGTH];

#if !defined(MSDOS)
   char buf[MAX_STRING_LENGTH];
#endif

   CHAR_DATA *ch;
   FILE *fp;
   bool found;
   int stat;

   ch = new_char();
   ch->pcdata = new_pcdata();

   d->character            = ch;
   ch->desc                = d;
   ch->name                = str_dup( name );
   ch->id                = get_pc_id();
   ch->race                = grn_human;
   ch->act                = PLR_NOSUMMON;
   ch->act2                            = 0;
   ch->mprog_target                    = NULL;
   ch->comm                = COMM_COMBINE
   | COMM_PROMPT;
   ch->comm2                           = COMM_SEE_SURNAME;
   ch->prompt                 = str_dup("<%hhp %mmn %vmv> ");
   ch->conference                      = 0;
   ch->pcdata->confirm_delete        = FALSE;
   ch->pcdata->bamfin            = str_dup( "" );
   ch->pcdata->bamfout            = str_dup( "" );
   ch->pcdata->title            = str_dup( "" );
   ch->pcdata->ignore                  = str_dup( "" );
   ch->pcdata->targetkills = 0;
   ch->pcdata->probation = -1;
   ch->pcdata->prob_time = -1;
   for (stat =0; stat < MAX_STATS; stat++)
   ch->perm_stat[stat]        = 13;
   ch->pcdata->condition[COND_THIRST]    = 48;
   ch->pcdata->condition[COND_FULL]    = 48;
   ch->pcdata->condition[COND_HUNGER]    = 48;
   ch->pcdata->condition[COND_STARVING] = 0;
   ch->pcdata->condition[COND_DEHYDRATED] = 0;
   ch->see = NULL;
   ch->nosee = NULL;
   ch->pcdata->being_restored = TRUE;  /* Coming from storage, load limits */

   found = FALSE;
   fclose( fpReserve );

   #if defined(unix)
   /* decompress if .gz file exists */
   sprintf( strsave, "%s%s%s", STORAGE_DIR, capitalize(name), ".gz");
   if ( ( fp = fopen( strsave, "r" ) ) != NULL )
   {
      fclose(fp);
      sprintf(buf, "gzip -dfq %s", strsave);
      system(buf);
   }
   #endif

   sprintf( strsave, "%s%s.plr", STORAGE_DIR, capitalize( name ) );
   if ( ( fp = fopen( strsave, "r" ) ) != NULL )
   {
      int iNest;

      for ( iNest = 0; iNest < MAX_NEST; iNest++ )
      rgObjNest[iNest] = NULL;

      found = TRUE;
      for ( ; ; )
      {
         char letter;
         char *word;

         letter = fread_letter(fp);
         if ( letter == '*' )
         {
            fread_to_eol(fp);
            continue;
         }

         if ( letter != '#' )
         {
            bug( "Load_char_obj: # not found.", 0 );
            break;
         }

         word = fread_word(fp);
         if      ( !str_cmp( word, "PLAYER" ) ) {fread_char ( ch, fp );}
         else if
         (
            !str_cmp(word, "OBJECT") ||
            !str_cmp(word, "O"     )
         )
         {
            ROOM_INDEX_DATA* room;

            /* Don't double add light */
            room = ch->in_room;
            ch->in_room = NULL;
            fread_obj  ( ch, fp );
            ch->in_room = room;
         }
         else if ( !str_cmp( word, "PET"    ) ) {fread_pet(ch, fp, TRUE);}
         else if ( !str_cmp( word, "CHARMIE") ) {fread_pet(ch, fp, FALSE);}
         else if ( !str_cmp( word, "END"    ) ) {free_string(word); break;}
         else
         {
            free_string(word);
            bug( "Load_char_obj: bad section.", 0 );
            break;
         }
         free_string(word);

      }
      fclose(fp);
   }

   fpReserve = fopen( NULL_FILE, "r" );

   /* initialize race */
   if (found)
   {
      int i;

      if
      (
         ch->race <= 0 ||
         ch->race >= MAX_PC_RACE
      )
      {
         ch->race = grn_human;
         ch->size = pc_race_table[ch->race].size;
      }

      ch->dam_type = pc_race_table[ch->race].dam_type;

      for (i = 0; i < 5; i++)
      {
         if (pc_race_table[ch->race].skills[i] == NULL)
         break;
         group_add(ch, pc_race_table[ch->race].skills[i], FALSE);
      }
      ch->form    = race_table[ch->race].form;
      ch->parts    = race_table[ch->race].parts;
   }

   if (found)
   {
      irv_update(ch, FALSE, TRUE);
   }

   return found;
}


/*
* Load a char and inventory into a new ch structure.
*/
bool load_char_obj( DESCRIPTOR_DATA *d, char *name )
{
   char strsave[MAX_INPUT_LENGTH];

#if !defined(MSDOS)
   char buf[MAX_STRING_LENGTH];
#endif

   CHAR_DATA *ch;
   FILE *fp;
   bool found;
   int stat;

   ch = new_char();
   ch->pcdata = new_pcdata();

   d->character            = ch;
   ch->desc                = d;
   ch->name                = str_dup( name );
   ch->id                = get_pc_id();
   ch->race                = grn_human;
   ch->act                = PLR_NOSUMMON;
   ch->act2                            = 0;
   ch->mprog_target                    = NULL;
   ch->comm                = COMM_COMBINE
   | COMM_PROMPT;
   ch->comm2                           = COMM_SEE_SURNAME;
   ch->prompt                 = str_dup("<%hhp %mmn %vmv> ");
   ch->pcdata->confirm_delete        = FALSE;
   ch->pcdata->bamfin            = str_dup( "" );
   ch->pcdata->bamfout            = str_dup( "" );
   ch->pcdata->title            = str_dup( "" );
   ch->pcdata->ignore                  = str_dup( "" );
   ch->pcdata->targetkills = 0;
   ch->pcdata->probation = -1;
   ch->pcdata->prob_time = -1;
   for (stat =0; stat < MAX_STATS; stat++)
   ch->perm_stat[stat]        = 13;
   ch->pcdata->condition[COND_THIRST]    = 48;
   ch->pcdata->condition[COND_FULL]    = 48;
   ch->pcdata->condition[COND_HUNGER]    = 48;
   ch->pcdata->condition[COND_STARVING] = 0;
   ch->pcdata->condition[COND_DEHYDRATED] = 0;
   ch->see = NULL;
   ch->nosee = NULL;

   found = FALSE;
   fclose( fpReserve );

   #if defined(unix)
   /* decompress if .gz file exists */
   sprintf( strsave, "%s%s%s", PLAYER_DIR, capitalize(name), ".gz");
   if ( ( fp = fopen( strsave, "r" ) ) != NULL )
   {
      fclose(fp);
      sprintf(buf, "gzip -dfq %s", strsave);
      system(buf);
   }
   #endif

   sprintf( strsave, "%s%s.plr", PLAYER_DIR, capitalize( name ) );
   if ( ( fp = fopen( strsave, "r" ) ) != NULL )
   {
      int iNest;

      for ( iNest = 0; iNest < MAX_NEST; iNest++ )
      rgObjNest[iNest] = NULL;

      found = TRUE;
      for ( ; ; )
      {
         char letter;
         char *word;

         letter = fread_letter(fp);
         if ( letter == '*' )
         {
            fread_to_eol(fp);
            continue;
         }

         if ( letter != '#' )
         {
            bug( "Load_char_obj: # not found.", 0 );
            break;
         }

         word = fread_word(fp);
         if      ( !str_cmp( word, "PLAYER" ) ) {fread_char ( ch, fp );}
         else if
         (
            !str_cmp(word, "OBJECT") ||
            !str_cmp(word, "O"     )
         )
         {
            ROOM_INDEX_DATA* room;

            /* Don't double add light */
            room = ch->in_room;
            ch->in_room = NULL;
            fread_obj  ( ch, fp );
            ch->in_room = room;
         }
         else if ( !str_cmp( word, "PET"    ) ) {fread_pet(ch, fp, TRUE);}
         else if ( !str_cmp( word, "CHARMIE") ) {fread_pet(ch, fp, FALSE);}
         else if ( !str_cmp( word, "END"    ) ) {free_string(word); break;}
         else
         {
            free_string(word);
            bug( "Load_char_obj: bad section.", 0 );
            break;
         }
         free_string(word);
      }
      fclose(fp);
   }

   fpReserve = fopen( NULL_FILE, "r" );

   /* initialize race */
   if (found)
   {
      int i;

      if
      (
         ch->race <= 0 ||
         ch->race >= MAX_PC_RACE
      )
      {
         ch->race = grn_human;
         ch->size = pc_race_table[ch->race].size;
      }

      ch->dam_type = pc_race_table[ch->race].dam_type;

      for (i = 0; i < 5; i++)
      {
         if (pc_race_table[ch->race].skills[i] == NULL)
         break;
         group_add(ch, pc_race_table[ch->race].skills[i], FALSE);
      }
      ch->form    = race_table[ch->race].form;
      ch->parts    = race_table[ch->race].parts;
   }

   if (found)
   {
      irv_update(ch, FALSE, TRUE);
   }

   return found;
}



/*
* Read in a char.
*/

#if defined(KEY)
#undef KEY
#endif

#define KEY(literal, field, value)     \
if (!str_cmp(word, literal))           \
{                                      \
   field  = value;                     \
   fMatch = TRUE;                      \
   break;                              \
}


/* provided to free strings */
#if defined(KEYS)
#undef KEYS
#endif

#define KEYS(literal, field, value)    \
if (!str_cmp(word, literal))           \
{                                      \
   free_string(field);                 \
   field  = value;                     \
   fMatch = TRUE;                      \
   break;                              \
}

/* provided to use functions of one param on strings */
#if defined(KEYSF1)
#undef KEYSF1
#endif

#define KEYSF1(literal, field, func, value)     \
if (!str_cmp(word, literal))                    \
{                                               \
   temp = value;                                \
   field  = (func)(temp);                       \
   free_string(temp);                           \
   fMatch = TRUE;                               \
   break;                                       \
}

void fread_char(CHAR_DATA* ch, FILE* fp)
{
   char buf[MAX_STRING_LENGTH];
   char word[MAX_INPUT_LENGTH];
   char* temp;
   bool fMatch;
   int count = 0;
   int lastlogoff = current_time;
   int percent;
   sprintf(buf, "Loading %s.", ch->name);
   log_string(buf);
   for (;;)
   {
      if (feof(fp))
      {
         strcpy(word, "End");
      }
      else
      {
         temp = fread_word(fp);
         strcpy(word, temp);
         free_string(temp);
      }
      fMatch = FALSE;
      switch (UPPER(word[0]))
      {
         case '*':
         {
            fMatch = TRUE;
            fread_to_eol(fp);
            break;
         }
         case 'A':
         {
            KEY("Act",        ch->act,               fread_flag(fp));
            KEY("Act2",       ch->act2,              fread_flag(fp));
            KEY("AffectedBy", ch->affected_by,       fread_flag(fp));
            KEY("Agemod",     ch->pcdata->age_mod,   fread_number(fp));
            KEY("AfBy",       ch->affected_by,       fread_flag(fp));
            KEY("AfBy2",      ch->affected_by2,      fread_flag(fp));
            KEY("Air",        ch->air_loss,          fread_number(fp));
            KEY("Alignment",  ch->alignment,         fread_number(fp));
            KEY("Alig",       ch->alignment,         fread_number(fp));
            KEY("AutoPurge",  ch->pcdata->autopurge, fread_number(fp));
            if (!str_cmp(word, "Alia"))
            {
               if (count >= MAX_ALIAS)
               {
                  fread_to_eol(fp);
                  fMatch = TRUE;
                  break;
               }
               ch->pcdata->alias[count] = str_dup(temp = fread_word(fp));
               free_string(temp);
               ch->pcdata->alias_sub[count] = str_dup(temp = fread_word(fp));
               free_string(temp);
               count++;
               fMatch = TRUE;
               break;
            }
            if (!str_cmp(word, "Alias"))
            {
               if (count >= MAX_ALIAS)
               {
                  fread_to_eol(fp);
                  fMatch = TRUE;
                  break;
               }
               ch->pcdata->alias[count] = str_dup(temp = fread_word(fp));
               free_string(temp);
               ch->pcdata->alias_sub[count] = fread_string(fp);
               count++;
               fMatch = TRUE;
               break;
            }
            if
            (
               !str_cmp(word, "AC") ||
               !str_cmp(word, "Armor")
            )
            {
               fread_to_eol(fp);
               fMatch = TRUE;
               break;
            }
            if (!str_cmp(word, "ACs"))
            {
               int counter;

               for (counter = 0; counter < 4; counter++)
               {
                  ch->armor[counter] = fread_number(fp);
               }
               fMatch = TRUE;
               break;
            }
            if (!str_cmp(word, "AffD"))
            {
               AFFECT_DATA* paf;
               int sn;

               paf = new_affect();
               sn = skill_lookup_bin(temp = fread_word(fp));
               free_string(temp);
               if (sn < 0)
               {
                  bug("Fread_char: unknown skill.", 0);
               }
               else
               {
                  paf->type = sn;
               }
               paf->level     = fread_number(fp);
               paf->duration  = fread_number(fp);
               paf->modifier  = fread_number(fp);
               paf->location  = fread_number(fp);
               paf->bitvector = fread_number(fp);
               paf->next      = ch->affected;
               ch->affected   = paf;
               fMatch = TRUE;
               break;
            }
            if (!str_cmp(word, "Affc"))
            {
               AFFECT_DATA* paf;
               int sn;

               paf = new_affect();
               sn = skill_lookup_bin(temp = fread_word(fp));
               free_string(temp);
               if (sn < 0)
               {
                  bug("Fread_char: unknown skill.", 0);
               }
               else
               {
                  paf->type = sn;
               }
               paf->where     = fread_number(fp);
               paf->level     = fread_number(fp);
               paf->duration  = fread_number(fp);
               paf->modifier  = fread_number(fp);
               paf->location  = fread_number(fp);
               paf->bitvector = fread_number(fp);
               paf->next      = ch->affected;
               ch->affected   = paf;
               fMatch = TRUE;
               break;
            }
            if (!str_cmp(word, "Affb"))
            {
               AFFECT_DATA* paf;
               int sn;
               paf = new_affect();
               sn = skill_lookup_bin(temp = fread_word(fp));
               free_string(temp);
               if (sn < 0)
               {
                  bug("Fread_char: unknown skill.", 0);
               }
               else
               {
                  paf->type = sn;
               }
               paf->where      = fread_number(fp);
               paf->level      = fread_number(fp);
               paf->duration   = fread_number(fp);
               paf->modifier   = fread_number(fp);
               paf->location   = fread_number(fp);
               paf->bitvector  = fread_number(fp);
               paf->bitvector2 = fread_number(fp);
               paf->next       = ch->affected;
               ch->affected    = paf;
               fMatch = TRUE;
               break;
            }
            if
            (
               !str_cmp(word, "AttrMod") ||
               !str_cmp(word, "AMod")
            )
            {
               int stat;

               for (stat = 0; stat < MAX_STATS; stat ++)
               {
                  ch->mod_stat[stat] = fread_number(fp);
               }
               fMatch = TRUE;
               break;
            }
            if
            (
               !str_cmp(word, "AttrPerm") ||
               !str_cmp(word, "Attr")
            )
            {
               int stat;

               for (stat = 0; stat < MAX_STATS; stat++)
               {
                  ch->perm_stat[stat] = fread_number(fp);
               }
               fMatch = TRUE;
               break;
            }
            break;
         }
         case 'B':
         {
            KEY("Brandrank", ch->pcdata->brand_rank, fread_number(fp));
            KEY("Bgold",     ch->gold_bank,          fread_number(fp));
            KEY("Bsilv",     ch->silver_bank,        fread_number(fp));
            KEYS("BDesc",    ch->pcdata->book_description, fread_string(fp));
            KEYS("Bamfin",   ch->pcdata->bamfin,     fread_string(fp));
            KEYS("Bamfout",  ch->pcdata->bamfout,    fread_string(fp));
            KEYS("Bin",      ch->pcdata->bamfin,     fread_string(fp));
            KEYS("Bout",     ch->pcdata->bamfout,    fread_string(fp));
            if (!str_cmp(word, "Bonus"))
            {
               int stat;

               for (stat = 0; stat < MAX_STATS; stat++)
               {
                  ch->pcdata->stat_bonus[stat] = fread_number(fp);
               }
               fMatch = TRUE;
               break;
            }
            break;
         }
         case 'C':
         {
            KEY("Comm",  ch->comm,  fread_flag(fp));
            KEY("Comm2", ch->comm2, fread_flag(fp));
            if
            (
               !str_cmp(word, "Class") ||
               !str_cmp(word, "Cla")
            )
            {
               int sn;

               fMatch = TRUE;
               ch->class = fread_number(fp);
               /* Include index 0 */
               for (sn = 0; sn < MAX_SKILL; sn++)
               {
                  ch->pcdata->learnlvl[sn] =
                  (
                     skill_table[sn].skill_level[ch->class]
                  );
               }
               break;
            }
            if
            (
               !str_cmp(word, "Condition") ||
               !str_cmp(word, "Cond") ||
               !str_cmp(word, "Cnd")
            )
            {
               int counter;

               for (counter = 0; counter < 6; counter++)
               {
                  ch->pcdata->condition[counter] = fread_number(fp);
               }
               fMatch = TRUE;
               break;
            }
            if (!str_cmp(word, "Clan"))
            {
               ch->pcdata->clan = fread_number(fp);
               free_string(ch->pcdata->clan_name);
               ch->pcdata->clan_name = fread_string(fp);
               fMatch = TRUE;
               break;
            }
            if (!str_cmp(word, "CDeposits"))
            {
               int clan = fread_number(fp);
               ch->pcdata->clan_deposits = fread_number(fp);
               ch->pcdata->clan_life_deposits = fread_number(fp);
               if (clan != ch->pcdata->clan)
               {
                  ch->pcdata->clan_deposits = 0;
                  ch->pcdata->clan_life_deposits = 0;
               }
               fMatch = TRUE;
               break;
            }
            break;
         }
         case 'D':
         {
            KEY("Deaths",       ch->pcdata->death_count,  fread_number(fp));
            KEY("Died",         ch->pcdata->death_status, fread_number(fp));
            KEY("Dtime",        ch->pcdata->death_timer,  fread_number(fp));
            KEY("Damroll",      ch->damroll,              fread_number(fp));
            KEY("Dam",          ch->damroll,              fread_number(fp));
            KEY("DescCurrent",  ch->pcdata->current_desc, fread_number(fp));
            KEYS("Description", ch->description,          fread_string(fp));
            KEYS("Desc",        ch->description,          fread_string(fp));
            KEYS("Dossier",     ch->pcdata->dossier,      fread_string(fp));
            if (!str_cmp(word, "DescMult"))
            {
               sh_int desc_num = fread_number(fp);
               if
               (
                  desc_num < 0 ||
                  desc_num >= MAX_DESCRIPTIONS
               )
               {
                  bug("Read desc number too high, %d", desc_num);
                  free_string(fread_string(fp));
                  fMatch = TRUE;
                  break;
               }
               free_string(ch->pcdata->desc_list[desc_num]);
               ch->pcdata->desc_list[desc_num] = fread_string(fp);
               fMatch = TRUE;
               break;
            }
            break;
         }
         case 'E':
         {
            if
            (
               !str_cmp(word, "EXTitle") ||
               !str_cmp(word, "EXTitl")
            )
            {
               free_string(ch->pcdata->extitle);
               ch->pcdata->extitle = fread_string(fp);
               if
               (
                  ch->pcdata->extitle[0] != '.' &&
                  ch->pcdata->extitle[0] != ',' &&
                  ch->pcdata->extitle[0] != '!' &&
                  ch->pcdata->extitle[0] != '?'
               )
               {
                  sprintf(buf, " %s", ch->pcdata->extitle);
                  free_string(ch->pcdata->extitle);
                  ch->pcdata->extitle = str_dup(buf);
               }
               if (ch->pcdata->extitle[0] == '\0')
               {
                  free_string(ch->pcdata->extitle);
                  ch->pcdata->extitle = NULL;
               }
               fMatch = TRUE;
               break;
            }
            KEY("Etho",   ch->pcdata->ethos,      fread_number(fp));
            KEY("Exp",    ch->exp,                fread_number(fp));
            KEY("ExpPen", ch->pcdata->xp_pen,     fread_number(fp));
            KEY("ExpT",   ch->exp_total,          fread_number(fp));
            KEY("Elec",   ch->pcdata->last_voted, fread_number(fp));
            KEYS("Email", ch->pcdata->email,      fread_string(fp));
            if (!str_cmp(word, "End"))
            {
               /* adjust hp mana move up  -- here for speed's sake */
               percent = (current_time - lastlogoff) * 25 / ( 2 * 60 * 60);
               percent = UMIN(percent, 100);
               if
               (
                  percent > 0 &&
                  !IS_AFFECTED(ch, AFF_POISON) &&
                  !IS_AFFECTED(ch, AFF_PLAGUE)
               )
               {
                  ch->hit  += (ch->max_hit - ch->hit)   * percent / 100;
                  ch->mana += (ch->max_mana - ch->mana) * percent / 100;
                  ch->move += (ch->max_move - ch->move) * percent / 100;
               }
               if (!ch->pcdata->race_lottery)
               {
                  ch->pcdata->race_lottery = ch->race;
               }
               return;
            }
            break;
         }
         case 'F':
         {
            KEYS("FirstSite", ch->pcdata->first_site, fread_string(fp));
            break;
         }
         case 'G':
         {
            KEY("Ghost", ch->ghost, fread_number(fp));
            KEY("Gold",  ch->gold,  fread_number(fp));
            if
            (
               !str_cmp(word, "Group") ||
               !str_cmp(word, "Gr")
            )
            {
               int gn;
               char* temp;
               gn = group_lookup(temp = fread_word(fp));
               if (gn < 0)
               {
                  fprintf(stderr, "%s", temp);
                  bug("Fread_char: unknown group. ", 0);
               }
               else
               {
                  gn_add(ch, gn);
               }
               fMatch = TRUE;
               free_string(temp);
            }
            break;
         }
         case 'H':
         {
            KEY("HackAttempts", ch->pcdata->hacked,       fread_number(fp));
            KEY("Hitroll",      ch->hitroll,              fread_number(fp));
            KEY("Hit",          ch->hitroll,              fread_number(fp));
            KEY("Houserank",    ch->pcdata->house_rank,   fread_number(fp));
            KEY("Houserank2",   ch->pcdata->house_rank2,  fread_number(fp));
            KEY("HolyPower",    ch->holy_power,           fread_number(fp));
            KEYSF1("Hous",      ch->house, &house_lookup, fread_string(fp));
            if
            (
               !str_cmp(word, "HpManaMove") ||
               !str_cmp(word, "HMV")
            )
            {
               ch->hit      = fread_number(fp);
               ch->max_hit  = fread_number(fp);
               ch->mana     = fread_number(fp);
               ch->max_mana = fread_number(fp);
               ch->move     = fread_number(fp);
               ch->max_move = fread_number(fp);
               fMatch = TRUE;
               break;
            }
            if
            (
               !str_cmp(word, "HpManaMovePerm") ||
               !str_cmp(word, "HMVP")
            )
            {
               ch->pcdata->perm_hit  = fread_number(fp);
               ch->pcdata->perm_mana = fread_number(fp);
               ch->pcdata->perm_move = fread_number(fp);
               fMatch = TRUE;
               break;
            }
            if (!str_cmp(word, "HDeposits"))
            {
               char* temp;
               int house = house_lookup(temp = fread_word(fp));
               free_string(temp);
               if
               (
                  house > 0 &&
                  house < MAX_HOUSE
               )
               {
                  ch->pcdata->deposits[house] = fread_number(fp);
                  ch->pcdata->life_deposits[house] = fread_number(fp);
               }
               else
               {
                  fread_number(fp);
                  fread_number(fp);
               }
               fMatch = TRUE;
               break;
            }
            break;
         }
         case 'I':
         {
            KEY("Id",         ch->id,             fread_number(fp));
            KEY("Indu",       ch->pcdata->induct, fread_number(fp));
            KEY("InvisLevel", ch->invis_level,    fread_number(fp));
            KEY("Inco",       ch->incog_level,    fread_number(fp));
            KEY("Invi",       ch->invis_level,    fread_number(fp));
            break;
         }
         case 'L':
         {
            KEY("LastDeath",  ch->pcdata->last_death, fread_number(fp));
            KEY("LastLevel",  ch->pcdata->last_level, fread_number(fp));
            KEY("LLev",       ch->pcdata->last_level, fread_number(fp));
            KEY("Level",      ch->level,              fread_number(fp));
            KEY("Lev",        ch->level,              fread_number(fp));
            KEY("Levl",       ch->level,              fread_number(fp));
            if (!strcmp(word, "LogO"))
            {
               lastlogoff = fread_number(fp);
               ch->pcdata->last_logon = lastlogoff;
               fMatch = TRUE;
               break;
            }
            KEYS("LastSite",  ch->pcdata->last_site,  fread_string(fp));
            KEYS("LongDescr", ch->long_descr,         fread_string(fp));
            KEYS("LnD",       ch->long_descr,         fread_string(fp));
            break;
         }
         case 'M':
         {
            KEY("Markaccepted", ch->pcdata->mark_accepted, fread_number(fp));
            KEY("Marktime",     ch->pcdata->mark_time,     fread_number(fp));
            KEY("Markprice",    ch->pcdata->mark_price,    fread_number(fp));
            KEY("Molt",         ch->pcdata->molting_into,  fread_number(fp));
            KEY("Molttime",     ch->pcdata->molting_time,  fread_number(fp));
            KEYS("Marker",      ch->pcdata->marker,        fread_string(fp));
            KEYS("Moniker",     ch->pcdata->moniker,       fread_string(fp));
            break;
         }
         case 'N':
         {
            KEY("Noregen", ch->noregen_dam,        fread_number(fp));
            KEY("Note",    ch->pcdata->last_note,  fread_number(fp));
            KEYS("Name",   ch->name,               fread_string(fp));
            KEYS("NsP",    ch->pcdata->nosee_perm, fread_string(fp));
            if (!str_cmp(word, "Not"))
            {
               ch->pcdata->last_note    = fread_number(fp);
               ch->pcdata->last_idea    = fread_number(fp);
               ch->pcdata->last_penalty = fread_number(fp);
               ch->pcdata->last_news    = fread_number(fp);
               ch->pcdata->last_changes = fread_number(fp);
               fMatch = TRUE;
               break;
            }
            if (!str_cmp(word, "Notes"))
            {
               ch->pcdata->last_note       = fread_number(fp);
               ch->pcdata->last_idea       = fread_number(fp);
               ch->pcdata->last_penalty    = fread_number(fp);
               ch->pcdata->last_news       = fread_number(fp);
               ch->pcdata->last_changes    = fread_number(fp);
               ch->pcdata->last_bank       = fread_number(fp);
               ch->pcdata->last_death_note = fread_number(fp);
               ch->pcdata->last_clan       = fread_number(fp);
               ch->pcdata->last_crim       = fread_number(fp);
               fMatch = TRUE;
               break;
            }
            break;
         }
         case 'O':
         {
            KEYS("OMarker", ch->pcdata->orig_marker, fread_string(fp));
            break;
         }
         case 'P':
         {
            KEY("PkExpand",   ch->pcdata->pk_expand,           fread_number(fp));
            KEY("Played",     ch->played,                      fread_number(fp));
            KEY("Plyd",       ch->played,                      fread_number(fp));
            KEY("Position",   ch->position,                    fread_number(fp));
            KEY("Pos",        ch->position,                    fread_number(fp));
            KEY("Practice",   ch->practice,                    fread_number(fp));
            KEY("Prac",       ch->practice,                    fread_number(fp));
            KEY("Prob",       ch->pcdata->probation,           fread_number(fp));
            KEY("ProbTime",   ch->pcdata->prob_time,           fread_number(fp));
            KEYS("Prompt",    ch->prompt,                      fread_string(fp));
            KEYS("Prom",      ch->prompt,                      fread_string(fp));
            KEYS("Password",  ch->pcdata->old_pwd,             fread_string(fp));
            KEYS("Pass",      ch->pcdata->old_pwd,             fread_string(fp));
            KEYS("PassMD5",   ch->pcdata->pwd,                 fread_string(fp));
            KEYS("PDesc",     ch->pcdata->psych_desc,          fread_string(fp));
            KEYS("PreTitl",   ch->pcdata->pre_title,           fread_string(fp));
            KEY("PGI",        ch->pcdata->gained_imm_flags,    fread_flag(fp));
            KEY("PGR",        ch->pcdata->gained_res_flags,    fread_flag(fp));
            KEY("PGV",        ch->pcdata->gained_vuln_flags,   fread_flag(fp));
            KEY("PGA",        ch->pcdata->gained_affects,      fread_flag(fp));
            KEY("PGA2",       ch->pcdata->gained_affects2,     fread_flag(fp));
            KEY("PLI",        ch->pcdata->lost_imm_flags,      fread_flag(fp));
            KEY("PLR",        ch->pcdata->lost_res_flags,      fread_flag(fp));
            KEY("PLV",        ch->pcdata->lost_vuln_flags,     fread_flag(fp));
            KEY("PLA",        ch->pcdata->lost_affects,        fread_flag(fp));
            KEY("PLA2",       ch->pcdata->lost_affects2,       fread_flag(fp));
            break;
         }
         case 'R':
         {
            KEY("Rebels",  ch->pcdata->rebels,     fread_number(fp));
            if (!str_cmp(word, "Race"))
            {
               temp = fread_string(fp);
               ch->race = race_lookup(temp);
               if (ch->race >= MAX_PC_RACE)
               {
                  sprintf
                  (
                     log_buf,
                     "PC's (%s)'s race on load [%d] is not a PC race!",
                     ch->name,
                     ch->race
                  );
                  bug(log_buf, 0);
                  ch->race = grn_human;
               }
               ch->size = pc_race_table[ch->race].size;
               free_string(temp);
               fMatch = TRUE;
               break;
            }
            if (!str_cmp(word, "RaceLottery"))
            {
               temp = fread_string(fp);
               ch->pcdata->race_lottery = race_lookup(temp);
               if (ch->pcdata->race_lottery >= MAX_PC_RACE)
               {
                  sprintf
                  (
                     log_buf,
                     "PC's (%s)'s racelottery on load [%d] is not a PC race!",
                     ch->name,
                     ch->pcdata->race_lottery
                  );
                  bug(log_buf, 0);
                  ch->pcdata->race_lottery = 0;
               }
               free_string(temp);
               fMatch = TRUE;
               break;
            }
            if (!str_cmp(word, "Room"))
            {
               ch->in_room = get_room_index(fread_number(fp));
               if (ch->in_room == NULL)
               {
                  ch->in_room = get_room_index(ROOM_VNUM_LIMBO);
               }
               fMatch = TRUE;
               break;
            }
            break;
         }
         case 'S':
         {
            KEY("Save",          ch->saving_throw,        fread_number(fp));
            KEY("SaveSpell",     ch->saving_spell,        fread_number(fp));
            KEY("SaveMaledict",  ch->saving_maledict,     fread_number(fp));
            KEY("SaveBreath",    ch->saving_breath,       fread_number(fp));
            KEY("SaveTransport", ch->saving_transport,    fread_number(fp));
            KEY("Savedflags",    ch->pcdata->saved_flags, fread_number(fp));
            KEY("Scro",          ch->lines,               fread_number(fp));
            KEY("Sex",           ch->sex,                 fread_number(fp));
            KEY("Silv",          ch->silver,              fread_number(fp));
            KEY("Subrace",       ch->subrace,             fread_number(fp));
            KEY("Specialize",    ch->pcdata->special,     fread_number(fp));
            KEY("SpellPower",    ch->spell_power,         fread_number(fp));
            KEY("Sight",         ch->sight,               fread_number(fp));
            KEY("Spam",          ch->pcdata->spam,        fread_number(fp));
            KEYS("ShortDescr",   ch->short_descr,         fread_string(fp));
            KEYS("SurName",      ch->pcdata->surname,     fread_string(fp));
            KEYS("ShD",          ch->short_descr,         fread_string(fp));
            if
            (
               !str_cmp(word, "Skill") ||
               !str_cmp(word, "Sk")
            )
            {
               int sn;
               int value;
               int value2 = 0;
               char* temp;

               value  = fread_number(fp);
               value2 = fread_number(fp);
               sn     = skill_lookup_bin(temp = fread_word(fp));
               if (sn < 0)
               {
                  sprintf(log_buf, "Fread_char: unknown skill: [%s].", temp);
                  bug(log_buf, 0);
               }
               else
               {
                  ch->pcdata->learned[sn] = value;
                  ch->pcdata->learnlvl[sn] = value2;
               }
               fMatch = TRUE;
               free_string(temp);
            }
            if (!str_cmp(word, "Sk2"))
            {
               int sn;
               int value;
               int value2 = 0;
               bool updated;
               char* temp;

               updated = fread_number(fp);
               value  = fread_number(fp);
               value2 = fread_number(fp);
               sn     = skill_lookup_bin(temp = fread_word(fp));
               if (sn < 0)
               {
                  sprintf(log_buf, "Fread_char: unknown skill: [%s].", temp);
                  bug(log_buf, 0);
               }
               else
               {
                  ch->pcdata->learned[sn] = value;
                  ch->pcdata->learnlvl[sn] = value2;
                  ch->pcdata->updated[sn] = updated;
               }
               fMatch = TRUE;
               free_string(temp);
            }
            break;
         }
         case 'T':
         {
            KEY("TAlig",       ch->true_alignment,      fread_number(fp));
            KEY("TargetKills", ch->pcdata->targetkills, fread_number(fp));
            KEY("Temple",      ch->temple,              fread_number(fp));
            KEY("TrueSex",     ch->pcdata->true_sex,    fread_number(fp));
            KEY("TSex",        ch->pcdata->true_sex,    fread_number(fp));
            KEY("Trai",        ch->train,               fread_number(fp));
            KEY("Trust",       ch->trust,               fread_number(fp));
            KEY("Tru",         ch->trust,               fread_number(fp));
            if
            (
               !str_cmp(word, "Title") ||
               !str_cmp(word, "Titl")
            )
            {
               free_string(ch->pcdata->title);
               ch->pcdata->title = fread_string(fp);
               if
               (
                  ch->pcdata->title[0] != '.' &&
                  ch->pcdata->title[0] != ',' &&
                  ch->pcdata->title[0] != '!' &&
                  ch->pcdata->title[0] != '?'
               )
               {
                  sprintf(buf, " %s", ch->pcdata->title);
                  free_string(ch->pcdata->title);
                  ch->pcdata->title = str_dup(buf);
               }
               fMatch = TRUE;
               break;
            }
            break;
         }
         case 'V':
         {
            KEY("Version", ch->version, fread_number (fp));
            KEY("Vers",    ch->version, fread_number (fp));
            if (!str_cmp(word, "Vnum"))
            {
               ch->pIndexData = get_mob_index(fread_number(fp));
               fMatch = TRUE;
               break;
            }
            break;
         }
         case 'W':
         {
            KEY("Wanteds",  ch->pcdata->wanteds,      fread_number(fp));
            KEY("Wimpy",    ch->wimpy,                fread_number(fp));
            KEY("Wimp",     ch->wimpy,                fread_number(fp));
            KEY("Wizn",     ch->wiznet,               fread_flag(fp));
            KEY("Wizn2",    ch->wiznet2,              fread_flag(fp));
            KEY("WSlot",    ch->pcdata->worship_slot, fread_number(fp));

            KEYS("Worship", ch->pcdata->worship,      fread_string(fp));
            break;
         }
      }
      if (!fMatch)
      {
         bug("Fread_char: no match.", 0);
         sprintf(buf, "Word is: %s", word);
         log_string(buf);
         fread_to_eol(fp);
      }
   }
}

/* load a pet from the forgotten reaches */
void fread_pet(CHAR_DATA* ch, FILE* fp, bool is_pet)
{
   char word[MAX_INPUT_LENGTH];
   char* temp;
   CHAR_DATA *pet;
   bool fMatch;
   int lastlogoff = current_time;
   int percent;

   /* first entry had BETTER be the vnum or we barf */
   if (feof(fp))
   {
      strcpy(word, "END");
   }
   else
   {
      strcpy(word, temp = fread_word(fp));
      free_string(temp);
   }
   if (!str_cmp(word, "Vnum"))
   {
      int vnum;

      vnum = fread_number(fp);
      if (get_mob_index(vnum) == NULL)
      {
         bug("Fread_pet: bad vnum %d.", vnum);
         pet = create_mobile(get_mob_index(MOB_VNUM_FIDO));
      }
      else
      pet = create_mobile(get_mob_index(vnum));
   }
   else
   {
      bug("Fread_pet: no vnum in file.", 0);
      pet = create_mobile(get_mob_index(MOB_VNUM_FIDO));
   }

   for ( ; ; )
   {
      if (feof(fp))
      {
         strcpy(word, "END");
      }
      else
      {
         strcpy(word, temp = fread_word(fp));
         free_string(temp);
      }
      fMatch = FALSE;

      switch (UPPER(word[0]))
      {
         case '*':
         fMatch = TRUE;
         fread_to_eol(fp);
         break;

         case 'A':
         KEY( "Act",        pet->act,        fread_flag(fp));
         KEY( "Act2",        pet->act2,              fread_flag(fp));
         KEY( "AfBy",    pet->affected_by,    fread_flag(fp));
         KEY( "AfBy2",       pet->affected_by2,      fread_flag(fp));
         KEY("Air",        pet->air_loss,          fread_number(fp));
         KEY( "Alig",    pet->alignment,        fread_number(fp));

         if (!str_cmp(word, "ACs"))
         {
            int i;

            for (i = 0; i < 4; i++)
            pet->armor[i] = fread_number(fp);
            fMatch = TRUE;
            break;
         }

         if (!str_cmp(word, "AffD"))
         {
            AFFECT_DATA *paf;
            int sn;

            paf = new_affect();

            sn = skill_lookup_bin(temp = fread_word(fp));
            free_string(temp);
            if (sn < 0)
            bug("Fread_char: unknown skill.", 0);
            else
            paf->type = sn;

            paf->level    = fread_number(fp);
            paf->duration    = fread_number(fp);
            paf->modifier    = fread_number(fp);
            paf->location    = fread_number(fp);
            paf->bitvector    = fread_number(fp);
            paf->next    = pet->affected;
            pet->affected    = paf;
            fMatch        = TRUE;
            break;
         }

         if (!str_cmp(word, "Affc"))
         {
            AFFECT_DATA *paf;
            int sn;

            paf = new_affect();

            sn = skill_lookup_bin(temp = fread_word(fp));
            free_string(temp);
            if (sn < 0)
            bug("Fread_char: unknown skill.", 0);
            else
            paf->type = sn;

            paf->where    = fread_number(fp);
            paf->level      = fread_number(fp);
            paf->duration   = fread_number(fp);
            paf->modifier   = fread_number(fp);
            paf->location   = fread_number(fp);
            paf->bitvector  = fread_number(fp);
            paf->next       = pet->affected;
            pet->affected   = paf;
            fMatch          = TRUE;
            break;
         }

         if (!str_cmp(word, "Affb"))
         {
            AFFECT_DATA *paf;
            int sn;

            paf = new_affect();

            sn = skill_lookup_bin(temp = fread_word(fp));
            free_string(temp);
            if (sn < 0)
            bug("Fread_char: unknown skill.", 0);
            else
            paf->type = sn;

            paf->where    = fread_number(fp);
            paf->level      = fread_number(fp);
            paf->duration   = fread_number(fp);
            paf->modifier   = fread_number(fp);
            paf->location   = fread_number(fp);
            paf->bitvector  = fread_number(fp);
            paf->bitvector2 = fread_number(fp);
            paf->next       = pet->affected;
            pet->affected   = paf;
            fMatch          = TRUE;
            break;
         }

         if (!str_cmp(word, "AMod"))
         {
            int stat;

            for (stat = 0; stat < MAX_STATS; stat++)
            pet->mod_stat[stat] = fread_number(fp);
            fMatch = TRUE;
            break;
         }

         if (!str_cmp(word, "Attr"))
         {
            int stat;

            for (stat = 0; stat < MAX_STATS; stat++)
            pet->perm_stat[stat] = fread_number(fp);
            fMatch = TRUE;
            break;
         }
         break;

         case 'C':
         KEY( "Comm",    pet->comm,        fread_flag(fp));
         KEY( "Comm2",      pet->comm2,             fread_flag(fp));
         break;

         case 'D':
         KEY( "Dam",    pet->damroll,        fread_number(fp));
         KEYS( "Desc",    pet->description,    fread_string(fp));
         break;

         case 'E':
         if (!str_cmp(word, "End"))
         {
            pet->leader = ch;
            pet->master = ch;
            if (is_pet)
            {
               ch->pet = pet;
            }
            /* adjust hp mana move up  -- here for speed's sake */
            percent = (current_time - lastlogoff) * 25 / ( 2 * 60 * 60);

            if (percent > 0 && !IS_AFFECTED(ch, AFF_POISON)
            &&  !IS_AFFECTED(ch, AFF_PLAGUE))
            {
               percent = UMIN(percent, 100);
               pet->hit    += (pet->max_hit - pet->hit) * percent / 100;
               pet->mana   += (pet->max_mana - pet->mana) * percent / 100;
               pet->move   += (pet->max_move - pet->move)* percent / 100;
            }
            return;
         }
         KEY( "Exp",    pet->exp,        fread_number(fp));
         break;

         case 'G':
         KEY( "Gold",    pet->gold,        fread_number(fp));
         break;

         case 'H':
         KEY( "Hit",    pet->hitroll,        fread_number(fp));
         KEY( "HolyPower",  pet->holy_power,       fread_number(fp));
         KEYSF1( "Hous",       pet->house,       &house_lookup, fread_string(fp));

         if (!str_cmp(word, "HMV"))
         {
            pet->hit    = fread_number(fp);
            pet->max_hit    = fread_number(fp);
            pet->mana    = fread_number(fp);
            pet->max_mana    = fread_number(fp);
            pet->move    = fread_number(fp);
            pet->max_move    = fread_number(fp);
            fMatch = TRUE;
            break;
         }
         break;

         case 'L':
         KEY( "Levl",    pet->level,        fread_number(fp));
         KEYS( "LnD",    pet->long_descr,    fread_string(fp));
         KEY( "LogO",    lastlogoff,        fread_number(fp));
         break;

         case 'N':
         KEYS( "Name",    pet->name,        fread_string(fp));
         break;
         case 'O':
         {
            KEY("Off",        pet->off_flags,        fread_flag(fp));
            break;
         }
         case 'P':
         KEY( "Pos",    pet->position,        fread_number(fp));
         break;

         case 'R':
         KEYSF1( "Race",    pet->race, &race_lookup, fread_string(fp));
         break;

         case 'S' :
         KEY( "Save",    pet->saving_throw,    fread_number(fp));
         KEY( "SaveSpell",   pet->saving_spell,      fread_number(fp));
         KEY( "SaveBreath",  pet->saving_breath,     fread_number(fp));
         KEY( "SaveMaledict", pet->saving_maledict,  fread_number(fp));
         KEY( "SaveTransport", pet->saving_transport, fread_number(fp));
         KEY( "Sex",        pet->sex,        fread_number(fp));
         KEYS( "ShD",        pet->short_descr,    fread_string(fp));
         KEY( "Silv",        pet->silver,            fread_number(fp) );
         KEY( "SpellPower",  pet->spell_power,       fread_number(fp));
         KEY( "Sight",       pet->sight,             fread_number(fp) );
         break;
         case 'T' :
         KEY( "TAlign",      pet->true_alignment,    fread_number(fp));
         break;

         if ( !fMatch )
         {
            bug("Fread_pet: no match.", 0);
            fread_to_eol(fp);
         }

      }
   }
}

void skip_obj(CHAR_DATA* ch, FILE* fp, int vnum)
{
   char word[MAX_STRING_LENGTH];
   char* temp;
   int iNest;

   for (; ;)
   {
      if (feof(fp))
      {
         strcpy(word, "End");
      }
      else
      {
         strcpy(word, temp = fread_word(fp));
         free_string(temp);
      }
      if ( !str_cmp( word, "Nest" ) )
      {
         iNest = fread_number(fp);
         if ( iNest < 0 || iNest >= MAX_NEST )
         {
            bug("skip_obj: bad nest %d.", iNest);
         }
         else
         {
            rgObjNest[iNest] = NULL;
         }
      }
      else if (!str_cmp(word, "End"))
      {
         break;
      }
      fread_to_eol(fp);
   }
   sprintf
   (
      word,
      "skip_obj: %s lost %d",
      (
         (
            ch == NULL ||
            ch->name == NULL
         ) ?
         "someone" :
         ch->name
      ),
      vnum
   );
   bug(word, 0);
}

void fread_obj( CHAR_DATA *ch, FILE *fp )
{

   OBJ_DATA *obj;
   char word[MAX_INPUT_LENGTH];
   char* temp;
   int iNest;
   bool fMatch;
   bool fNest;
   bool fVnum;
   bool first;
   bool new_format;  /* to prevent errors */
   bool make_new;    /* update object */
   unsigned int quantity = 0;
   fVnum = FALSE;
   obj = NULL;
   first = TRUE;  /* used to counter fp offset */
   new_format = FALSE;
   make_new = FALSE;

   if (feof(fp))
   {
      strcpy(word, "End");
   }
   else
   {
      strcpy(word, temp = fread_word(fp));
      free_string(temp);
   }
   if (!str_cmp(word, "Vnum" ))
   {
      int vnum;
      first = FALSE;  /* fp will be in right place */

      vnum = fread_number(fp);
      if (  get_obj_index( vnum )  == NULL )
      {
         bug( "Fread_obj: bad vnum %d.", vnum );
         skip_obj(ch, fp, vnum);
         return;
      }
      else
      {
         obj = create_object(get_obj_index(vnum),-1);
         if
         (
            !IS_NPC(ch) &&
            !ch->pcdata->being_restored
         )
         {
            obj->pIndexData->limcount -= 1;
         }
         new_format = TRUE;
      }
   }

   if (obj == NULL)  /* either not found or old style */
   {
      obj = new_obj();
      obj->name        = str_dup( "" );
      obj->short_descr    = str_dup( "" );
      obj->description    = str_dup( "" );
   }

   fNest        = FALSE;
   fVnum        = TRUE;
   iNest        = 0;

   for ( ; ; )
   {
      if (first)
      first = FALSE;
      else
      {
         if (feof(fp))
         {
            strcpy(word, "End");
         }
         else
         {
            strcpy(word, temp = fread_word(fp));
            free_string(temp);
         }
      }

      fMatch = FALSE;

      switch ( UPPER(word[0]) )
      {
         case '*':
         fMatch = TRUE;
         fread_to_eol(fp);
         break;

         case 'A':
         if (!str_cmp(word, "AffD"))
         {
            AFFECT_DATA *paf;
            int sn;

            paf = new_affect();

            sn = skill_lookup_bin(temp = fread_word(fp));
            free_string(temp);
            if (sn < 0)
            bug("Fread_obj: unknown skill.", 0);
            else
            paf->type = sn;

            paf->level    = fread_number(fp);
            paf->duration    = fread_number(fp);
            paf->modifier    = fread_number(fp);
            paf->location    = fread_number(fp);
            paf->bitvector    = fread_number(fp);
            paf->next    = obj->affected;
            obj->affected    = paf;
            fMatch        = TRUE;
            break;
         }
         if (!str_cmp(word, "Affc"))
         {
            AFFECT_DATA *paf;
            int sn;

            paf = new_affect();

            sn = skill_lookup_bin(temp = fread_word(fp));
            free_string(temp);
            if (sn < 0)
            bug("Fread_obj: unknown skill.", 0);
            else
            paf->type = sn;

            paf->where    = fread_number(fp);
            paf->level      = fread_number(fp);
            paf->duration   = fread_number(fp);
            paf->modifier   = fread_number(fp);
            paf->location   = fread_number(fp);
            paf->bitvector  = fread_number(fp);
            paf->next       = obj->affected;
            obj->affected   = paf;
            fMatch          = TRUE;
            break;
         }
         if (!str_cmp(word, "Affb"))
         {
            AFFECT_DATA *paf;
            int sn;

            paf = new_affect();

            sn = skill_lookup_bin(temp = fread_word(fp));
            free_string(temp);
            if (sn < 0)
            bug("Fread_obj: unknown skill.", 0);
            else
            paf->type = sn;

            paf->where    = fread_number(fp);
            paf->level      = fread_number(fp);
            paf->duration   = fread_number(fp);
            paf->modifier   = fread_number(fp);
            paf->location   = fread_number(fp);
            paf->bitvector  = fread_number(fp);
            paf->bitvector2 = fread_number(fp);
            paf->next       = obj->affected;
            obj->affected   = paf;
            fMatch          = TRUE;
            break;
         }

         break;

         case 'C':
         KEY( "Cond",         obj->condition,   fread_number(fp));
         KEY( "Cost",         obj->cost,        fread_number(fp));
         KEYSF1("CorpseRace", obj->value[0],    &race_lookup, fread_string(fp));
         break;

         case 'D':
         KEYS( "Description",    obj->description,    fread_string(fp) );
         KEYS( "Desc",    obj->description,    fread_string(fp) );
         break;

         case 'E':

         if ( !str_cmp( word, "Enchanted"))
         {
            obj->enchanted = TRUE;
            fMatch     = TRUE;
            break;
         }

         KEY( "ExtraFlags",    obj->extra_flags,    fread_number(fp) );
         KEY( "ExtF",    obj->extra_flags,    fread_number(fp) );
         KEY( "ExtF2",       obj->extra_flags2,      fread_number(fp) );

         if ( !str_cmp( word, "ExtraDescr" ) || !str_cmp(word, "ExDe"))
         {
            EXTRA_DESCR_DATA *ed;

            ed = new_extra_descr();

            ed->keyword        = fread_string(fp);
            ed->description        = fread_string(fp);
            ed->next        = obj->extra_descr;
            obj->extra_descr    = ed;
            fMatch = TRUE;
         }

         if ( !str_cmp( word, "End" ) )
         {
            if ( !fNest || !fVnum || obj->pIndexData == NULL)
            {
               bug( "Fread_obj: incomplete object.", 0 );
               free_obj(obj);
               return;
            }
            else
            {
               if (!new_format)
               {
                  obj->next    = object_list;
                  object_list    = obj;
                  obj->pIndexData->count++;
               }

               if (!obj->pIndexData->new_format
               && obj->item_type == ITEM_ARMOR
               &&  obj->value[1] == 0)
               {
                  obj->value[1] = obj->value[0];
                  obj->value[2] = obj->value[0];
               }
               if (make_new)
               {
                  int wear;

                  wear = obj->wear_loc;
                  extract_obj(obj, FALSE);
                  /*            obj->pIndexData->limcount += 1;        */
                  obj = create_object(obj->pIndexData, 0);
                  obj->wear_loc = wear;
               }

               if ( iNest == 0 || rgObjNest[iNest] == NULL )
               obj_to_char( obj, ch );
               else
               obj_to_obj( obj, rgObjNest[iNest-1] );

               /* In order to undo the quantity feature safely for pfiles,
               this section is added.  It will read Quantity out of the
               pfile and duplicate the item up to 20 times. */
               if (quantity > 1)
               {
                  OBJ_DATA *qObj = NULL;
                  unsigned int i;

                  /* Duplicate the object */
                  for (i = 2; i <= UMIN(quantity, 20); i++)
                  {
                     /* Create */
                     qObj = create_object (obj->pIndexData, 0);
                     clone_object (obj, qObj);

                     /* Put the object in its appropriate location */
                     if ( iNest == 0 || rgObjNest[iNest] == NULL )
                     obj_to_char( qObj, ch );
                     else
                     obj_to_obj( qObj, rgObjNest[iNest-1] );
                  }
               }

               return;
            }
         }
         break;

         case 'I':
         KEY( "ItemType",    obj->item_type,        fread_number(fp) );
         KEY( "Ityp",    obj->item_type,        fread_number(fp) );
         break;

         case 'L':
         KEY( "Level",    obj->level,        fread_number(fp) );
         KEY( "Lev",        obj->level,        fread_number(fp) );
         break;

         case 'M':
         KEYS( "Mat",    obj->material,    fread_string(fp) );
         break;
         case 'N':
         KEYS( "Name",    obj->name,        fread_string(fp) );

         if ( !str_cmp( word, "Nest" ) )
         {
            iNest = fread_number(fp);
            if ( iNest < 0 || iNest >= MAX_NEST )
            {
               bug( "Fread_obj: bad nest %d.", iNest );
            }
            else
            {
               rgObjNest[iNest] = obj;
               fNest = TRUE;
            }
            fMatch = TRUE;
         }
         break;

         case 'O':
         KEYS( "Owner",    obj->owner,        fread_string( fp));
         if ( !str_cmp( word, "Oldstyle" ) )
         {
            if (obj->pIndexData != NULL && obj->pIndexData->new_format)
            make_new = TRUE;
            fMatch = TRUE;
            break;
         }
         break;
         case 'P':
         {
            if (!str_cmp(word, "Page"))
            {
               int page;

               if (obj->item_type != ITEM_BOOK)
               {
                  bug
                  (
                     "fread_obj: Loading page on non-book! Type %d.",
                     obj->item_type
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
               if (page > obj->value[0])
               {
                  bug("Load_objects: Too many pages: %d.", page);
                  exit(1);
               }
               /* first is 0, last is BOOK_MAX_PAGES - 1 */
               page--;
               obj->book_info->title[page]   = fread_string(fp);
               obj->book_info->page[page]    = fread_string(fp);
               fMatch = TRUE;
               break;
            }
            if (!str_cmp(word, "Pen"))
            {
               char* temp;

               temp = fread_word(fp);
               obj->value[0] = pencolor_lookup(temp);
               free_string(temp);
               temp = fread_word(fp);
               obj->value[2] = inkwell_lookup(temp);
               free_string(temp);
               fMatch = TRUE;
               break;
            }
            if (!str_cmp(word, "PrvO"))
            {
               sh_int temp = fread_number(fp);

               free_string(obj->prev_owners[temp]);
               obj->prev_owners[temp] = fread_string(fp);
               fMatch = TRUE;
               break;
            }
            if (!str_cmp(word, "PrvI"))
            {
               sh_int temp = fread_number(fp);

               obj->prev_owners_id[temp] = fread_number(fp);
               fMatch = TRUE;
               break;
            }
            if (!str_cmp(word, "PrvS"))
            {
               sh_int temp = fread_number(fp);

               free_string(obj->prev_owners_site[temp]);
               obj->prev_owners_site[temp] = fread_string(fp);
               fMatch = TRUE;
               break;
            }
            KEYS("PrevOwner",   obj->prev_owners[0],        fread_string(fp));
            KEYS("PrevOwnerSite", obj->prev_owners_site[0], fread_string(fp));
            break;
         }

         case 'Q':
         KEY ("Quantity", quantity, fread_number(fp));
         break;
         case 'S':
         KEYS( "ShortDescr",    obj->short_descr,    fread_string(fp) );
         KEYS( "ShD",        obj->short_descr,    fread_string(fp) );

         if ( !str_cmp( word, "Spell" ) )
         {
            int iValue;
            int sn;

            iValue = fread_number(fp);
            sn = skill_lookup_bin(temp = fread_word(fp));
            free_string(temp);
            if ( iValue < 0 || iValue > 3 )
            {
               bug( "Fread_obj: bad iValue %d.", iValue );
            }
            else if ( sn < 0 )
            {
               bug( "Fread_obj: unknown skill.", 0 );
            }
            else
            {
               obj->value[iValue] = sn;
            }
            fMatch = TRUE;
            break;
         }

         break;

         case 'T':
         KEY( "Timer",    obj->timer,        fread_number(fp) );
         KEY( "Time",    obj->timer,        fread_number(fp) );
         break;

         case 'V':
         if ( !str_cmp( word, "Values" ) || !str_cmp(word, "Vals"))
         {
            obj->value[0]    = fread_number(fp);
            obj->value[1]    = fread_number(fp);
            obj->value[2]    = fread_number(fp);
            obj->value[3]    = fread_number(fp);
            if (obj->item_type == ITEM_WEAPON && obj->value[0] == 0)
            obj->value[0] = obj->pIndexData->value[0];
            fMatch        = TRUE;
            break;
         }

         if ( !str_cmp( word, "Val" ) )
         {
            obj->value[0]     = fread_number(fp);
            obj->value[1]    = fread_number(fp);
            obj->value[2]     = fread_number(fp);
            obj->value[3]    = fread_number(fp);
            obj->value[4]    = fread_number(fp);
            fMatch = TRUE;
            break;
         }

         if ( !str_cmp( word, "Vnum" ) )
         {
            int vnum;

            vnum = fread_number(fp);
            if ( ( obj->pIndexData = get_obj_index( vnum ) ) == NULL )
            bug( "Fread_obj: bad vnum %d.", vnum );
            else
            fVnum = TRUE;
            fMatch = TRUE;
            quantity = 0;
            break;
         }
         break;

         case 'W':
         KEY( "WearFlags",    obj->wear_flags,    fread_number(fp) );
         KEY( "WeaF",    obj->wear_flags,    fread_number(fp) );
         KEY( "WearLoc",    obj->wear_loc,        fread_number(fp) );
         KEY( "Wear",    obj->wear_loc,        fread_number(fp) );
         KEY( "Weight",    obj->weight,        fread_number(fp) );
         KEY( "Wt",        obj->weight,        fread_number(fp) );
         break;

      }
      if ( !fMatch )
      {
         bug( "Fread_obj: no match.", 0 );
         fread_to_eol(fp);
      }
   }
}

void log_convo(CHAR_DATA* ch, CHAR_DATA* ch_to, int type, char* argument)
{
   FILE* gfile;
   char strsave[MAX_INPUT_LENGTH];
   char type_name[MAX_STRING_LENGTH];
   char type_name2[MAX_STRING_LENGTH];
   char* tstring;

   if (ch == NULL || IS_NPC(ch))
   {
      return;
   }

   fclose(fpReserve);
   sprintf(strsave, "../log/clog/%s.clog", ch->name);
   if ((gfile = fopen(strsave, "a")) == NULL)
   {
      bug("Conversation Logging: fopen", 0);
      perror(strsave);
   }

   /* write string here */
   tstring = ctime(&current_time);
   tstring[strlen(tstring) - 1] = '\0';

   switch (type)
   {
      default:
      sprintf(type_name, " N/A ");
      break;
      case (TYPE_TELL):
      case (TYPE_TELL2):
      sprintf(type_name, "(tell )");
      break;
      case (TYPE_SAY):
      case (TYPE_SAY2):
      sprintf(type_name, "( say )");
      break;
      case (TYPE_OOC):
      case (TYPE_OOC2):
      sprintf(type_name, "( ooc )");
      break;
      case (TYPE_GTELL):
      case (TYPE_GTELL2):
      sprintf(type_name, "(group)");
      break;
      case (TYPE_YELL):
      case (TYPE_YELL2):
      sprintf(type_name, "(yell )");
      break;
      case (TYPE_AUCTION):
      case (TYPE_AUCTION2):
      sprintf(type_name, "(auctn)");
      break;
      case (TYPE_QUESTION):
      case (TYPE_QUESTION2):
      sprintf(type_name, "(quest)");
      break;
      case (TYPE_HOUSE):
      case (TYPE_HOUSE2):
      sprintf(type_name, "(house)");
      break;
      case (TYPE_CLAN):
      case (TYPE_CLAN2):
      sprintf(type_name, "(house)");
      break;
      case (TYPE_IMM):
      case (TYPE_IMM2):
      sprintf(type_name, "(imm)");
      break;
      case (TYPE_CONFERENCE):
      case (TYPE_CONFERENCE2):
      sprintf(type_name, "(conference)");
      break;
   }
   sprintf(type_name2, "%s Room [%d] :", type_name, ch->in_room->vnum);

   if (type < 10)
   {
      if (ch_to == NULL)
      {
         fprintf
         (
            gfile,
            "%s: %s: %s %s\n",
            tstring,
            ch->name,
            type_name2,
            argument
         );
      }
      else
      {
         fprintf
         (
            gfile,
            "%s: %s to %s: %s %s\n",
            tstring,
            ch->name,
            ch_to->name,
            type_name2,
            argument
         );
      }
   }
   else
   {
      if (ch_to == NULL)
      {
         fprintf
         (
            gfile,
            "%s: ? to %s: %s %s\n",
            tstring,
            ch->name,
            type_name2,
            argument
         );
      }
      else
      {
         fprintf
         (
            gfile,
            "%s: %s to %s: %s %s\n",
            tstring,
            ch_to->name,
            ch->name,
            type_name2,
            argument
         );
      }
   }

   fclose(gfile);
   fpReserve = fopen(NULL_FILE, "r");
   return;
}


void save_globals(void)
{
   FILE * gfile;
   char strsave[MAX_INPUT_LENGTH];
   int cnt;
   extern bool newlock;
   OBJ_DATA *obj;
   int house;

   fclose( fpReserve );
   sprintf(strsave, "%sglobals.dat", "");
   if ( ( gfile = fopen( TEMP_FILE, "w" ) ) == NULL )
   {
      bug( "Save_globals: fopen", 0 );
      perror( strsave );
   }
   if (log_mobs)
   {
      fprintf(gfile, "LogMobprogs\n");
   }
   if (all_magic_gone)
   {
      fprintf(gfile, "all_magic_gone\n");
   }
   fprintf(gfile, "LBU %ld\n", last_banknote_update);
   for (house = 1; house < MAX_HOUSE; house++)
   {
      for (obj = object_list; obj != NULL; obj = obj->next)
      {
         if (obj->pIndexData->house == house)
         break;
      }
      if (obj == NULL) continue;
      if (obj->in_room == NULL) continue;
      if (obj->in_room->house == house) continue;
      for (cnt = 0; cnt < MAX_HOUSE; cnt++)
      if (obj->in_room->vnum == house_table[cnt].vnum_altar)
      fprintf(gfile, "Hitem %d %d\n", obj->pIndexData->vnum, obj->in_room->vnum);
   }

   fprintf(gfile, "Autoreboot  %d  %d\n", autoreboot_warning_hour, autoreboot_hour);

   fprintf(gfile, "Lottery Race Reset  %ld\n", lottery_race_reset);
   for (cnt = 1; cnt < MAX_PC_RACE; cnt++)
   {
      fprintf
      (
         gfile,
         "Lottery Race Run %s %ld\n",
         race_table[cnt].name,
         lottery_race_last_run[cnt]
      );
   }
   for (house = 0; house < MAX_HOUSE; house++)
   {
      if (!str_cmp(house_table[house].name, "")) continue;
      if (!str_cmp(house_table[house].balance_name, "")) continue;
      if (!str_cmp(house_table[house].morale_name, "")) continue;
      fprintf(gfile, "%s %ld\n", house_table[house].balance_name, house_account[house]);
      fprintf(gfile, "%s %ld %ld %ld %ld %ld %ld %ld %ld\n", house_table[house].morale_name, house_downs[house], house_kills[house], house_pks[house],
      unhoused_pk[house], unique_kills[house], unhoused_kills[house], raids_defended[house], raids_nodefender[house]);
      fprintf(gfile, "cov_ally %d %ld\n", house, covenant_allies[house]);
   }
   fprintf(gfile, "newlock %d\n", newlock);
   fprintf(gfile, "morale_reset %ld\n", morale_reset_time);
   fprintf(gfile, "max_on_ever %d\n", max_on_ever);
   for (cnt = 0; worship_table[cnt].name != NULL; cnt++)
   {
      if (worship_table[cnt].brand_info.bt_disable)
      {
         fprintf
         (
            gfile,
            "bt_disabled %s~\n",
            worship_table[cnt].name
         );
      }
   }
   for (cnt = 0; cnt < MAX_CLASS; cnt++){
      fprintf(gfile, "class_count %d %ld\n", cnt, class_count[cnt]);
   }
   for (cnt = 0; cnt < 20; cnt++)
   {
      fprintf(gfile, "votes %d %d\n", cnt, votes[cnt]);
   }
   for (cnt = 0; cnt < 2000; cnt++)
   if (black_market_data[cnt][0] != 0)
   fprintf(gfile, "bmd %d %d %d\n", black_market_data[cnt][0], black_market_data[cnt][1], black_market_data[cnt][2]);
   fprintf(gfile, "DCave %d %d %d\n", demon_eggs, demon_babies, demon_adults);
   if (frog_house)
   {
      fprintf(gfile, "FrogHouse %d\n", frog_house);
   }
   if (last_note_id)
   {
      fprintf
      (
         gfile,
         "NId %ld\n",
         last_note_id
      );
   }
   fprintf(gfile, "end\n");
   fclose(gfile);
   rename(TEMP_FILE, strsave);
   fpReserve = fopen( NULL_FILE, "r" );
   return;
}

void dump_wizireport(WORSHIP_TYPE* worship, FILE* fp, int slot)
{
   if (worship->history.total_played != 0)
   {
      fprintf
      (
         fp,
         "%s %d %ld %ld %ld %ld %ld %ld\n",
         worship->name,
         slot,
         worship->history.start,
         worship->history.played,
         worship->history.vis,
         worship->history.total_start,
         worship->history.total_played,
         worship->history.total_vis
      );
   }
}

void save_wizireport()
{
   FILE* gfile;
   char strsave[MAX_INPUT_LENGTH];
   WORSHIP_TYPE* worship;
   sh_int god;

   fclose(fpReserve);
   sprintf(strsave, "%swizireport.dat", "");
   if
   (
      (
         gfile = fopen(TEMP_FILE, "w")
      ) == NULL
   )
   {
      bug("Save_wizireport: fopen", 0);
      perror(strsave);
   }

   for (god = 0; worship_table[god].name != NULL; god++)
   {
      worship = &worship_table[god];
      dump_wizireport(worship, gfile, god);
   }
   for
   (
      god = 0;
      (
         temp_worship_table[god].name != NULL &&
         god < MAX_TEMP_GODS
      );
      god++
   )
   {
      worship = &temp_worship_table[god];
      dump_wizireport(worship, gfile, -1 - god);
   }
   fprintf(gfile, "end\n");
   fclose(gfile);
   rename(TEMP_FILE, strsave);
   fpReserve = fopen(NULL_FILE, "r");
   return;
}

void save_modified_book(OBJ_DATA* book_obj)
{
   FILE* book;
   char  strsave[MAX_INPUT_LENGTH];
   int   page;
   OBJ_INDEX_DATA* index = book_obj->pIndexData;

   fclose(fpReserve);
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
         book = fopen(TEMP_FILE, "w")
      ) == NULL
   )
   {
      bug("Save_modified_book: fopen", 0);
      perror(strsave);
      exit(1);
   }
   for (page = 0; page < BOOK_MAX_PAGES; page++)
   {
      if (index->book_info->book_save_info->modified[page])
      {
         free_string(index->book_info->title[page]);
         index->book_info->title[page] = str_dup(book_obj->book_info->title[page]);
         free_string(index->book_info->page[page]);
         index->book_info->page[page] = str_dup(book_obj->book_info->page[page]);
         fprintf
         (
            book,
            "%d\n"
            "%s~\n"
            "%s~\n",
            page + 1,
            index->book_info->title[page],
            index->book_info->page[page]
         );
      }
   }
   fprintf(book, "-1\n");
   fclose(book);
   rename(TEMP_FILE, strsave);
   fpReserve = fopen(NULL_FILE, "r");
}
