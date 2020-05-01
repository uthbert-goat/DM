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

static const char rcsid[] = "$Id: interp.c,v 1.179 2004/11/25 08:52:20 fizzfaldt Exp $";

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
#include "merc.h"
#include "interp.h"
#include "subskill.h"
#include "marauder.h"
#include "worship.h"

bool    is_note_to( CHAR_DATA *ch, NOTE_DATA *pnote );

bool    check_social    args( ( CHAR_DATA *ch, char *command, char *argument ) );
void    spirit_command    args( ( CHAR_DATA *ch, int cmd, char * argument));

/* Buffer functions */
bool add_buf      args( (BUFFER *buffer, char *string) );
BUFFER *new_buf   args( () );
char *buf_string  args( (BUFFER *buffer) );
void free_buf     args( (BUFFER *buffer) );
void page_to_char args( ( const char *txt, CHAR_DATA *ch ) );

/*
* Command logging types.
*/
#define LOG_NORMAL    0
#define LOG_ALWAYS    1
#define LOG_NEVER    2



/*
* Log-all switch.
*/
bool                fLogAll        = FALSE;


/*
   struct    cmd_type
   {
       char * const  name;
       DO_FUN *      do_fun;
       sh_int        position;
       sh_int        level;
       sh_int        log;
       bool          show;
       bool          hide;
       bool          allow_rp;
       bool          ghost;
   };
   Explanation of table fields for the interp table: (in order above)
   name: In quotes, a single word (no spaces, no tabs, no special characters)
      how to invoke this command.
      It can be done with an abbreviation,
      if you want to enforce spelling out fully, i.e.
      delete
      you make a prefix command missing one letter.
      i.e.
      an entry for "delet"
      and "delete"
      "delet" will bring up a message telling you to write it out fully.
      You can use underscores, but I reccomend against it.
      Entries higher up in the table
      have priority for prefixes.
   do_fun:  What function to call.
      The function must have a prototype like this:
      void FUNCTIONNAME(CHAR_DATA* ch, char* argument)
      Function name can of course be replaced,
      ch and argument can be renamed, only the data types are important.
      (as well as a void, or no return value, function)
   position:   Minimum position to be able to use this command.
               If you are at this position, or higher, you can use
               this command (if you meet the other prereqs)
               From low to high, these are the positions:
               (Note, for information only Imm commands, you should
               probably use POS_DEAD, so the immortal can use at any time)
               POS_DEAD
               POS_MORTAL
               POS_INCAP
               POS_STUNNED
               POS_SLEEPING
               POS_RESTING
               POS_SITTING
               POS_FIGHTING
               POS_STANDING
   level:      Minimum level/trust level to use this command.
               For normal mortals, this compares against their level.
               i.e. marry requires level 25, or they get a 'Huh?'
               For normal immortals, this compares against their level.
               For trusted mortals/immortals, this compares only against
               their trust level.
               ML = max level (60) at the moment.
               L# means Max level - #
               i.e. L3 = 60 - 3 = 57
               L8 means 52 (just immortal)
               IM means 52, but don't use it, use L8 instead
   log:        3 choices:
      LOG_NORMAL:    Standard.  Can be snooped, can be logged if a high immortal
                     turns on log_all, or logs the character
      LOG_ALWAYS:    Log each and every single time anyone types
                     this command.
                     useful for 'delete', 'pray', certain imm commands like 'slay'
                     Can be snooped.  Logged whether or not log_all is on, or
                     log for the character is on
      LOG_NEVER:     NEVER LOG.  This means you cannot snoop this command either.
                     EVEN if you turn on full complete logging of absolutely everything,
                     this will still not get logged.
                     This will also not be checked for profanity.
                     This should only be used for things such as 'north' 'south' 'east'..etc
                     that are used so much that would be a problem if they were ever logged,
                     or things such as 'password' which would be bad to be able to be
                     snooped, or logged by mistake.
   show:       TRUE or FALSE
               Show in the commands/wizhelp list?
               TRUE means show.
                  If level is HERO level or less, it shows up in 'commands'
                  If level is greater than HERO level, it shows up in 'wizhelp'
               FALSE means do NOT show
                  If level is HERO level or less, it doesn't show up anywhere.
                  If level is greater than HERO level, it shows up in 'wizhelp', ONLY if you use the
                     wizhelp hidden option.
   hide:       TRUE or FALSE
               TRUE means if you use this command, you step out of the shadows.
               FALSE means you can use this command, and stay in the shadows
               (shadows being the hide skill)
               NOTE:
                  If only a failed, or only a successful use of a skill will
                  make you step out of the shadows,
                  use 0 (to allow staying in shadows)
                  and make the person step out of the shadows
                  inside the actual function
   allow_rp:   TRUE or FALSE
               TRUE means that if you do not have the trust to use the command,
                    but you have allowrp, you CAN use it.
               FALSE means that if you do not have the trust to use the command,
                    you cannot use it.
   ghost:      TRUE or FALSE
               TRUE means you can use this command as a ghost.
               FALSE means you cannot use this command as a ghost.
   Tutorial by Fizzfaldt
*/
/*
* Command table.        XUR -- had const before struct
*/
struct    cmd_type    cmd_table    [] =
{
   /*
   * Common movement commands.
   *
   * Explanation of numbers in order from right to left:
   * 0 = minimum level needed for command
   * 1 = show command in the "commands" list (0 for hide command)
   * 0 = command does not take you out of 'hide' (1 if you want players to un-hide)
   *
   */

   { "north",          do_north,       POS_STANDING,    0,  LOG_NEVER,  TRUE,  FALSE, FALSE, TRUE,  },
   { "east",           do_east,        POS_STANDING,    0,  LOG_NEVER,  TRUE,  FALSE, FALSE, TRUE,  },
   { "south",          do_south,       POS_STANDING,    0,  LOG_NEVER,  TRUE,  FALSE, FALSE, TRUE,  },
   { "west",           do_west,        POS_STANDING,    0,  LOG_NEVER,  TRUE,  FALSE, FALSE, TRUE,  },
   { "up",             do_up,          POS_STANDING,    0,  LOG_NEVER,  TRUE,  FALSE, FALSE, TRUE,  },
   { "down",           do_down,        POS_STANDING,    0,  LOG_NEVER,  TRUE,  FALSE, FALSE, TRUE,  },

   /*
   * Common other commands.
   * Placed here so one and two letter abbreviations work.
   */
   { "affects",        do_affects,     POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, TRUE,  },
   { "at",             do_at,          POS_DEAD,       L6,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "c",              do_cast,        POS_RESTING,     0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "cast",           do_cast_non_clergy, POS_RESTING, 0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "castsilent",     do_cast_silent, POS_DEAD,       L6,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "supplicate",     do_supplicate,  POS_RESTING,     0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "hang",           do_hang,        POS_STANDING,   20,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "auction",        do_auction,     POS_RESTING,     0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "buy",            do_buy,         POS_RESTING,     0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "burrow",         do_burrow,      POS_STANDING,    0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "channels",       do_channels,    POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "exits",          do_exits,       POS_RESTING,     0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "get",            do_get,         POS_RESTING,     0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "nomagic",        do_nomagic,     POS_DEAD,       L5,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "goto",           do_goto,        POS_DEAD,       L8,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "group",          do_group,       POS_SLEEPING,    0,  LOG_NORMAL, TRUE,  FALSE, FALSE, TRUE,  },
   { "gsndump",        do_gsndump,     POS_DEAD,       L6,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "balance",        do_balance,     POS_RESTING,     0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "deposit",        do_deposit,     POS_RESTING,     0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "withdraw",       do_withdraw,    POS_RESTING,     0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "hit",            do_hit,         POS_FIGHTING,    0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "bounty",         do_bounty,      POS_SLEEPING,    0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "bookdescription", do_book_description, POS_DEAD,   0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "look",           do_look,        POS_RESTING,     0,  LOG_NORMAL, TRUE,  FALSE, FALSE, TRUE,  },
   { "see",            do_see,         POS_DEAD,       L8,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "nosee",          do_nosee,       POS_DEAD,       L8,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, }, /* new imm skill - Drinlinda */
   { "seebrand",       do_seebrand,    POS_DEAD,       L5,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "seehouse",       do_seehouse,    POS_DEAD,       L7,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "seenewbie",      do_seenewbie,   POS_DEAD,       L8,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "seeworship",     do_seeworship,  POS_DEAD,       L7,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "seeclan",        do_seeclan,     POS_DEAD,       L7,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "elder",          do_elder,       POS_DEAD,        0,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "leader",         do_leader,      POS_DEAD,       L7,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "makelich",       do_makelich,    POS_DEAD,       L4,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "inventory",      do_inventory,   POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "instruct",       do_instruct,    POS_DEAD,        0,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "induct",         do_induct,      POS_DEAD,        0,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "novice",         do_novice,      POS_DEAD,        0,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "ally",           do_ally,        POS_DEAD,        0,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "ignore",         do_ignore,      POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "honorbound",     do_honorbound,  POS_DEAD,       L5,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "kill",           do_kill,        POS_FIGHTING,    0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "killerinstinct", do_killer_instinct, POS_FIGHTING, 0, LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "houtlaw",        do_houtlaw,     POS_SLEEPING,   L5,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "bt",             do_brandtalk,   POS_SLEEPING,    0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "ht",             do_housetalk,   POS_SLEEPING,    0,  LOG_NORMAL, TRUE,  FALSE, FALSE, TRUE,  },
   { "hgtalk",         do_hgtalk,      POS_SLEEPING,   L7,  LOG_NORMAL, TRUE,  FALSE, FALSE, TRUE,  },
   { "/",              do_recall,      POS_FIGHTING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, TRUE,  },
   { "order",          do_order,       POS_RESTING,     0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "practice",       do_practice,    POS_SLEEPING,    0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "pray",           do_pray,        POS_SLEEPING,    0,  LOG_ALWAYS, TRUE,  FALSE, FALSE, TRUE,  },
   { "rest",           do_rest,        POS_SLEEPING,    0,  LOG_NORMAL, TRUE,  FALSE, FALSE, TRUE,  },
   { "sit",            do_sit,         POS_SLEEPING,    0,  LOG_NORMAL, TRUE,  FALSE, FALSE, TRUE,  },
   { "sockets",        do_sockets,     POS_DEAD,       L7,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "extitle",        do_extitle,     POS_DEAD,       L6,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "multicheck",     do_multicheck,  POS_DEAD,       L5,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "awareness",      do_awareness,   POS_RESTING,     0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "blending",       do_forest_blending, POS_STANDING, 0, LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "gaseous",        do_gaseous_form, POS_STANDING,   0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "stand",          do_stand,       POS_SLEEPING,    0,  LOG_NORMAL, TRUE,  FALSE, FALSE, TRUE,  },
   { "marriage",       do_marriage,    POS_STANDING, DO_MARRIAGE_MINIMUM_LEVEL, LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "marry",          do_marriage,    POS_STANDING, DO_MARRIAGE_MINIMUM_LEVEL, LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "stance",         do_beastialstance, POS_FIGHTING, 0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "tell",           do_tell,        POS_RESTING,     0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, TRUE,  },
   { "talk",           do_talk,        POS_DEAD,       L7,  LOG_NORMAL, TRUE,  FALSE, TRUE,  FALSE, },
   { "use",            do_use,         POS_RESTING,     0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "apply",          do_apply,       POS_STANDING,    0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "ooctell",        do_ooctell,     POS_RESTING,     0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, TRUE,  },
   { "unlock",         do_unlock,      POS_RESTING,     0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "wield",          do_wear,        POS_RESTING,     0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "wizhelp",        do_wizhelp,     POS_DEAD,       L8,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "rphelp",         do_rphelp,      POS_DEAD,       L8,  LOG_NORMAL, TRUE,  FALSE, TRUE,  FALSE, },
   { "request",        do_request,     POS_STANDING,    0,  LOG_ALWAYS, TRUE,  TRUE,  FALSE, FALSE, },
   { "demand",         do_demand,      POS_STANDING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "demonswarm",     do_demon_swarm, POS_FIGHTING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   /*
   * Informational commands.
   */
   { "areas",          do_areas,       POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, TRUE,  },
   { "bug",            do_bug,         POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "changes",        do_changes,     POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "commands",       do_commands,    POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, TRUE,  },
   { "compare",        do_compare,     POS_RESTING,     0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "consider",       do_consider,    POS_RESTING,     0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "count",          do_count,       POS_SLEEPING,    0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "cre",            do_release_crusader, POS_RESTING, 0, LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "credits",        do_credits,     POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, TRUE,  },
   { "equipment",      do_equipment,   POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "examine",        do_examine,     POS_RESTING,     0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "glance",         do_glance,      POS_RESTING,     0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "help",           do_help,        POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, TRUE,  },
   { "helpadd",        do_helpnote,    POS_DEAD,       L8,  LOG_NORMAL, TRUE,  TRUE,  TRUE,  FALSE, },
   { "helperrobe",     do_helper_robe, POS_STANDING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "idea",           do_idea,        POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "motd",           do_motd,        POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "news",           do_news,        POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "report",         do_report,      POS_RESTING,     0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "rules",          do_rules,       POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "score",          do_score,       POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, TRUE,  },
   { "skills",         do_skills,      POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "socials",        do_socials,     POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "show",           do_show,        POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "spells",         do_spells,      POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "story",          do_story,       POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "time",           do_time,        POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, TRUE,  },
   { "typo",           do_typo,        POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "weather",        do_weather,     POS_RESTING,     0,  LOG_NORMAL, TRUE,  FALSE, FALSE, TRUE,  },
   { "who",            do_who,         POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, TRUE,  },
   { "whois",          do_whois,       POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, TRUE,  },
   { "wizlist",        do_wizlist,     POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "worth",          do_worth,       POS_SLEEPING,    0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "worship",        do_worship,     POS_RESTING,     0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "bless",          do_bless,       POS_RESTING,    L5,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "punish",         do_punish,      POS_RESTING,    L5,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "retired",        do_retired,     POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },

   /*
   * Configuration commands.
   */
   { "alia",           do_alia,        POS_DEAD,        0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "alias",          do_alias,       POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "autolist",       do_autolist,    POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "evaluate",       do_evaluation,  POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "notransfer",     do_notransfer,  POS_SLEEPING,    0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "autoassist",     do_autoassist,  POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "autoexit",       do_autoexit,    POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "autogold",       do_autogold,    POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "autoloot",       do_autoloot,    POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "autosac",        do_autosac,     POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "autosplit",      do_autosplit,   POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "autoattack",     do_autoattack,  POS_FIGHTING,    0,  LOG_ALWAYS, FALSE, FALSE, FALSE, FALSE, },
   /*
   MUSIC_DISABLED
   { "music",          do_music,       POS_DEAD,        0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "musicdownload",  do_music_dl,    POS_DEAD,        0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "musicdl",        do_music_dl,    POS_DEAD,        0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   */
   { "brief",          do_brief,       POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "color",          do_color,       POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "colorreduce",    do_color_reduce, POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "notick",         do_notick,      POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "combine",        do_combine,     POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "compact",        do_compact,     POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "description",    do_description_type, POS_DEAD,   0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "psych",          do_psychdesc,   POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "info",           do_info,        POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "storag",         do_storag,      POS_DEAD,        0,  LOG_ALWAYS, FALSE, FALSE, FALSE, FALSE, },
   { "storage",        do_storage,     POS_STANDING,    0,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "delet",          do_delet,       POS_DEAD,        0,  LOG_ALWAYS, FALSE, FALSE, FALSE, TRUE,  },
   { "delete",         do_delete,      POS_STANDING,    0,  LOG_ALWAYS, TRUE,  FALSE, FALSE, TRUE,  },
   { "nofollow",       do_nofollow,    POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "nosummon",       do_nosummon,    POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "outfit",         do_outfit,      POS_RESTING,     0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "password",       do_password,    POS_DEAD,        0,  LOG_NEVER,  TRUE,  FALSE, FALSE, FALSE, },
   { "prompt",         do_prompt,      POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "scroll",         do_scroll,      POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "title",          do_title,       POS_DEAD,       L7,  LOG_ALWAYS, TRUE,  FALSE, TRUE,  FALSE, },
   { "unalias",        do_unalias,     POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "wimpy",          do_wimpy,       POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },

   /*
   * Communication commands.
   */
   { "answer",         do_answer,      POS_RESTING,     0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "deaf",           do_deaf,        POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "emote",          do_emote,       POS_RESTING,     0,  LOG_NORMAL, TRUE,  FALSE, FALSE, TRUE,  },
   { "pmote",          do_pmote,       POS_RESTING,     0,  LOG_NORMAL, TRUE,  FALSE, FALSE, TRUE,  },
   { ",",              do_emote,       POS_RESTING,     0,  LOG_NORMAL, FALSE, FALSE, FALSE, TRUE,  },
   { "gtell",          do_gtell,       POS_DEAD,        0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, TRUE,  },
   { ";",              do_gtell,       POS_DEAD,        0,  LOG_NORMAL, FALSE, TRUE,  FALSE, TRUE,  },
   { "note",           do_note,        POS_SLEEPING,    0,  LOG_NORMAL, TRUE,  FALSE, FALSE, TRUE,  },
   { "code",           do_code,        POS_DEAD,       L7,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "pose",           do_pose,        POS_RESTING,     0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "quest",          do_quest,       POS_RESTING,     0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "question",       do_question,    POS_RESTING,     0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "quiet",          do_quiet,       POS_SLEEPING,    0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "reply",          do_reply,       POS_RESTING,     0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, TRUE,  },
   { "replay",         do_replay,      POS_SLEEPING,    0,  LOG_NORMAL, TRUE,  FALSE, FALSE, TRUE,  },
   { "say",            do_say,         POS_RESTING,     0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, TRUE,  },
   { "flip",           do_flip,        POS_RESTING,     0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "toke",           do_toke,        POS_RESTING,    L2,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "'",              do_say,         POS_RESTING,     0,  LOG_NORMAL, FALSE, TRUE,  FALSE, TRUE,  },
   { "unread",         do_unread,      POS_SLEEPING,    0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "yell",           do_yell,        POS_RESTING,     0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, TRUE,  },

   /*
   * Object manipulation commands.
   */
   { "brandish",       do_brandish,    POS_RESTING,     0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "close",          do_close,       POS_RESTING,     0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, TRUE,  },
   { "drink",          do_drink,       POS_RESTING,     0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "drop",           do_drop,        POS_RESTING,     0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "embalm",         do_embalm,      POS_STANDING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "eat",            do_eat,         POS_RESTING,     0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "empower",        do_empower,     POS_STANDING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "envenom",        do_envenom,     POS_RESTING,     0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "sharpen",        do_sharpen,     POS_RESTING,     0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "fill",           do_fill,        POS_RESTING,     0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "forest",         do_forest_blending, POS_STANDING, 0, LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "give",           do_give,        POS_RESTING,     0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "heal",           do_heal,        POS_RESTING,     0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "hold",           do_wear,        POS_RESTING,     0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "list",           do_list,        POS_RESTING,     0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "lock",           do_lock,        POS_RESTING,     0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "open",           do_open,        POS_RESTING,     0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, TRUE,  },
   { "read",           do_book_read,   POS_RESTING,     0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "sign",           do_book_sign,   POS_RESTING,     0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "write",          do_book_sign,   POS_RESTING,     0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "contents",       do_book_contents, POS_RESTING,   0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "pick",           do_pick,        POS_STANDING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "pour",           do_pour,        POS_RESTING,     0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "put",            do_put,         POS_RESTING,     0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "pricelist",      do_pricelist,   POS_RESTING,     0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "quaff",          do_quaff,       POS_RESTING,     0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "recite",         do_recite,      POS_RESTING,     0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "remove",         do_remove,      POS_RESTING,     0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "door",           do_door_bash,   POS_FIGHTING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "sell",           do_sell,        POS_RESTING,     0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "take",           do_get,         POS_RESTING,     0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "takemagic",      do_takemagic,   POS_DEAD,       L3,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "sacrifice",      do_sacrifice,   POS_RESTING,     0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "junk",           do_sacrifice,   POS_RESTING,     0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "butcher",        do_butcher,     POS_STANDING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "skin",           do_skin,        POS_STANDING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "tan",            do_tanning,     POS_STANDING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "huntersknife",   do_hunters_knife, POS_FIGHTING,  0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "darkfocus",      do_darkfocus,   POS_FIGHTING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "rapier",         do_rapier,      POS_FIGHTING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "carve",          do_carving,     POS_STANDING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "call",           do_call_wild,   POS_FIGHTING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "ccall",          do_call_crusader, POS_RESTING,   0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "tap",            do_sacrifice,   POS_RESTING,     0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "value",          do_value,       POS_RESTING,     0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "wear",           do_wear,        POS_RESTING,     0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "zap",            do_zap,         POS_RESTING,     0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "riot",           do_riot,        POS_FIGHTING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "vanish",         do_vanish,      POS_FIGHTING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   /*  { "aura",           do_aura_of_sustenance, POS_RESTING, 0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, }, */
   { "halo",           do_halo_of_the_sun, POS_RESTING, 0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "feed",           do_feed,        POS_RESTING,     0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "fence",          do_fence,       POS_RESTING,     0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "dice",           do_dice_roll,   POS_RESTING,     0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   /*
   * Combat commands.
   */
   { "cleave",         do_cleave,      POS_STANDING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "herb",           do_herb,        POS_STANDING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "wanted",         do_wanted,      POS_SLEEPING,    0,  LOG_ALWAYS, FALSE, FALSE, FALSE, FALSE, },
   { "lawless",        do_lawless,     POS_SLEEPING,    0,  LOG_ALWAYS, FALSE, FALSE, FALSE, FALSE, },
   { "rebel",          do_rebel,       POS_SLEEPING,    0,  LOG_ALWAYS, FALSE, FALSE, FALSE, FALSE, },
   { "records",        do_records,     POS_SLEEPING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "deathstrike",    do_deathstrike, POS_STANDING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "spike",          do_spike,       POS_STANDING,   25,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "ambush",         do_ambush,      POS_FIGHTING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "getaway",        do_getaway,     POS_STANDING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "pugil",          do_pugil,       POS_FIGHTING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "mute",           do_mute,        POS_FIGHTING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "canopy",         do_canopy_walk, POS_FIGHTING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "grace",          do_grace,       POS_FIGHTING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "know",           do_know_time,   POS_FIGHTING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "lash",           do_lash,        POS_FIGHTING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "skullbash",      do_skullbash,   POS_FIGHTING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "shield",         do_shield_cleave, POS_FIGHTING,  0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "backstab",       do_backstab,    POS_FIGHTING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "sbash",          do_shieldbash,  POS_FIGHTING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "bash",           do_bash,        POS_FIGHTING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "throw",          do_throw,       POS_FIGHTING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "strangle",       do_strangle,    POS_STANDING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "steed",          do_steed,       POS_STANDING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "blindness",      do_blindness_dust, POS_FIGHTING, 0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "blindfold",      do_blindfold,   POS_STANDING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "bind",           do_bind,        POS_STANDING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "gag",            do_gag,         POS_STANDING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "poison",         do_poison_dust, POS_FIGHTING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "warcry",         do_warcry,      POS_FIGHTING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "warpaint",       do_warpaint,    POS_STANDING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "focus",          do_focus,       POS_FIGHTING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "endure",         do_endure,      POS_STANDING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "palm",           do_palm,        POS_FIGHTING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "nerve",          do_nerve,       POS_FIGHTING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "ram",            do_ram2,        POS_FIGHTING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "bite",           do_bite,        POS_FIGHTING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "target",         do_target,      POS_FIGHTING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "tail",           do_tail,        POS_FIGHTING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "battlecry",      do_battlecry,   POS_FIGHTING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "whirlwind",      do_whirlwind,   POS_FIGHTING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "spellbane",      do_spellbane,   POS_FIGHTING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "battleshield",   do_battleshield, POS_STANDING,   0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "battlescream",   do_battlescream, POS_FIGHTING,   0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "trophy",         do_trophy,      POS_RESTING,     0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "blackjack",      do_blackjack,   POS_STANDING,    0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "blindfight",     do_blind_fighting, POS_STANDING, 0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "shackle",        do_shackles,    POS_FIGHTING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "bj",             do_blackjack,   POS_STANDING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "breath",         do_breath_fire, POS_FIGHTING,    0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "dragonbreath",   do_breath_morph, POS_FIGHTING,   0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "mlink",          do_mentallink,  POS_FIGHTING,    0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "psionic",        do_psionic_blast, POS_FIGHTING,  0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "bs",             do_backstab,    POS_FIGHTING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "healingtrance",  do_healtrance,  POS_STANDING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "spiritwalk",     do_spiritwalk,  POS_STANDING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "knockdown",      do_knockdown,   POS_FIGHTING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "fists",          do_fists,       POS_FIGHTING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "claw",           do_open_claw,   POS_FIGHTING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "harmony",        do_harmony,     POS_STANDING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "wildfury",       do_wildfury,    POS_FIGHTING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "iron will",      do_iron_will,   POS_STANDING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "dspirit",        do_dragon_spirit, POS_STANDING,  0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "stunning",       do_stunning_strike, POS_FIGHTING, 0, LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "ipalm",          do_iron_palm,   POS_FIGHTING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "chi",            do_chi_attack,  POS_FIGHTING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "dchant",         do_dchant,      POS_STANDING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "pounce",         do_pounce,      POS_FIGHTING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "spinning kick",  do_spin_kick,   POS_FIGHTING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "promote",        do_promote,     POS_STANDING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "berserk",        do_berserk,     POS_FIGHTING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "circle",         do_circle_stab, POS_FIGHTING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "detect",         do_detect_hidden, POS_STANDING,  0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "vigilance",      do_vigilance,   POS_STANDING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "crush",          do_crush,       POS_FIGHTING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "dirt",           do_dirt,        POS_FIGHTING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "dip",            do_pen_fill,    POS_RESTING,     0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "sm",             do_sm,          POS_DEAD,        0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "smoke",          do_smoke_screen, POS_FIGHTING,   0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "disarm",         do_disarm,      POS_FIGHTING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "offdisarm",      do_offhand_disarm, POS_FIGHTING, 0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "offhand",        do_offhand_disarm, POS_FIGHTING, 0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "disarmtrap",     do_disarmtrap,  POS_STANDING,    0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "flee",           do_flee,        POS_FIGHTING,    0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "blockretreat",   do_block_retreat, POS_FIGHTING,  0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "kick",           do_kick,        POS_FIGHTING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "rear",           do_rear_kick,   POS_FIGHTING,    0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },

   { "lunge",          do_lunge,       POS_FIGHTING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "jump",           do_jump,        POS_STANDING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "thrust",         do_thrust,      POS_FIGHTING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "feign",          do_feign_death, POS_FIGHTING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "critical",       do_critical_strike, POS_FIGHTING, 0, LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "taunt",          do_taunt,       POS_FIGHTING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "multistrike",    do_multistrike, POS_FIGHTING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "murde",          do_murde,       POS_FIGHTING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "murder",         do_murder,      POS_FIGHTING,    0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "rescue",         do_rescue,      POS_FIGHTING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   /*  { "surrender",      do_surrender,   POS_FIGHTING,    0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },*/
   { "trip",           do_trip,        POS_FIGHTING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "trapmaking",     do_trapmaking,  POS_STANDING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "cripple",        do_cripple,     POS_FIGHTING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "dimmak",         do_dim_mak,     POS_FIGHTING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "defend",         do_defend,      POS_FIGHTING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "assassinate",    do_assassinate, POS_STANDING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "forage",         do_forage,      POS_STANDING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "lore",           do_lore,        POS_RESTING,     0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "soulscry",       do_soulscry,    POS_RESTING,     0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "diagnose",       do_diagnose,    POS_RESTING,     0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "intimidate",     do_intimidate,  POS_FIGHTING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "cunning",        do_cunning_strike, POS_FIGHTING, 0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "thugstance",     do_thugs_stance, POS_FIGHTING,   0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "misinform",      do_misinformation, POS_FIGHTING, 0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "silvertongue",   do_silver_tongue,  POS_STANDING, 0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "song",           do_song_of_shadows, POS_STANDING, 0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "soothe",         do_soothing_voice, POS_FIGHTING, 0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "beltuse",        do_belt_use,       POS_FIGHTING, 0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "roguedance",     do_dance_of_the_rogue, POS_FIGHTING, 0, LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "eyesofbandit",   do_eyes_of_the_bandit, POS_STANDING, 0, LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "gatecall",       do_gate_call,      POS_FIGHTING, 0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "guildcloth",     do_guild_cloth,    POS_STANDING, 0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },

   /*
   * Mob command interpreter (placed here for faster scan...)
   */
   { "mob",            do_mob,         POS_DEAD,        0,  LOG_NEVER,  FALSE, FALSE, FALSE, FALSE, },

   /*
   * Miscellaneous commands.
   */
   { "assess",         do_assess,      POS_RESTING,     0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "follow",         do_follow,      POS_RESTING,     0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "scan",           do_scan,        POS_RESTING,     0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "scale",          do_scalepick,   POS_DEAD,        0,  LOG_ALWAYS, FALSE, FALSE, FALSE, FALSE, },
   { "gain",           do_gain,        POS_STANDING,    0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "groups",         do_groups,      POS_SLEEPING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "acute",          do_acute_vision, POS_STANDING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "barkskin",       do_barkskin,    POS_STANDING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "shroud",         do_shroud,      POS_STANDING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "eye",            do_eye_of_the_predator, POS_STANDING, 0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "shadowgate",     do_shadowgate,  POS_STANDING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "animal",         do_animal_call, POS_STANDING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "enlist",         do_enlist,      POS_STANDING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "bear",           do_bear_call,   POS_STANDING,    0,  LOG_ALWAYS, FALSE, FALSE, FALSE, FALSE, },
   { "beastcommand",   do_beast_command, POS_FIGHTING,  0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "camouflage",     do_camouflage,  POS_STANDING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "camp",           do_camp,        POS_RESTING,     0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "hide",           do_hide,        POS_RESTING,     0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "laying",         do_laying_hands, POS_STANDING,   0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "chiheal",        do_chi_healing, POS_STANDING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "morphdragon",    do_morph_dragon, POS_STANDING,   0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "mount",          do_mount,       POS_STANDING,    0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "dismount",       do_dismount,    POS_STANDING,    0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "shapeshift",     do_shapeshift,  POS_STANDING,    0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "revert",         do_revert,      POS_FIGHTING,    0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "qui",            do_qui,         POS_DEAD,        0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "quit",           do_quit,        POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, TRUE,  },
   { "release",        do_release,     POS_FIGHTING,    0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "crelease",       do_release_crusader, POS_RESTING, 0, LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "recall",         do_recall,      POS_FIGHTING,    0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, TRUE,  },
   { "bribe",          do_bribe,       POS_STANDING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "dobribery",      do_bribery,     POS_FIGHTING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "tame",           do_tame,        POS_FIGHTING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "find water",     do_find_water,  POS_STANDING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "fw",             do_find_water,  POS_STANDING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "track",          do_track,       POS_RESTING,     0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "trace",          do_trace,       POS_RESTING,     0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "rent",           do_rent,        POS_DEAD,        0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "save",           do_save,        POS_DEAD,        0,  LOG_NORMAL, TRUE,  FALSE, FALSE, TRUE,  },
   { "sleep",          do_sleep,       POS_SLEEPING,    0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "view",           do_view,        POS_STANDING,    0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "vote",           do_vote,        POS_STANDING,    0,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "voodoo",         do_voodoo,      POS_FIGHTING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "sneak",          do_sneak,       POS_STANDING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "split",          do_split,       POS_RESTING,     0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "steal",          do_steal,       POS_STANDING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "lookout",        do_lookout,     POS_STANDING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "ransack",        do_ransack,     POS_STANDING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "plant",          do_plant,       POS_STANDING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, }, /* New thief skill -- Wicket */
   { "stealth",        do_stealth,     POS_STANDING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "offer",          do_offer,       POS_STANDING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "steel",          do_steel_nerves, POS_FIGHTING,   0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "learn",          do_learn_adv,   POS_FIGHTING,    0,  LOG_NORMAL, TRUE,  TRUE,  FALSE, FALSE, },
   { "train",          do_train,       POS_RESTING,     0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "visible",        do_visible,     POS_SLEEPING,    0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "wake",           do_wake,        POS_SLEEPING,    0,  LOG_NORMAL, TRUE,  FALSE, FALSE, TRUE,  },
   { "where",          do_where,       POS_RESTING,     0,  LOG_NORMAL, TRUE,  FALSE, FALSE, TRUE,  },
   { "cloak",          do_cloak,       POS_STANDING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "hometown",       do_hometown,    POS_RESTING,     0,  LOG_ALWAYS, TRUE,  TRUE,  FALSE, FALSE, },
   { "chant",          do_chant,       POS_STANDING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "portal",         do_portal,      POS_STANDING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "enter",          do_portal,      POS_STANDING,    0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "conjure",        do_conjure,     POS_STANDING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "drain",          do_drain,       POS_STANDING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },

   /*
   * Immortal commands.
   */
   { "advance",        do_advance,      POS_DEAD,      ML,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "alist",          do_alist,        POS_DEAD,      L7,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "dump",           do_dump,         POS_DEAD,      ML,  LOG_ALWAYS, FALSE, FALSE, FALSE, FALSE, },
   { "trust",          do_trust,        POS_DEAD,      ML,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "violate",        do_violate,      POS_DEAD,      ML,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "allow",          do_allow,        POS_DEAD,      L2,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "allowrp",        do_allowrp,      POS_DEAD,      L3,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "ban",            do_ban,          POS_DEAD,      L2,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "access",         do_access,       POS_DEAD,      ML,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "deny",           do_deny,         POS_DEAD,      L5,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "forsake",        do_forsake,      POS_DEAD,      L5,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "cforsaken",      do_cforsaken,    POS_DEAD,      L5,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "disconnect",     do_disconnect,   POS_DEAD,      L5,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "flag",           do_flag,         POS_DEAD,      L4,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "freeze",         do_freeze,       POS_DEAD,      L4,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "global",         do_global,       POS_DEAD,      L6,  LOG_NORMAL, TRUE,  FALSE, TRUE,  FALSE, },
   { "smite",          do_smite,        POS_DEAD,      L6,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "nameallow",      do_nameallow,    POS_DEAD,      L2,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "nameban",        do_nameban,      POS_DEAD,      L2,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "permban",        do_permban,      POS_DEAD,      L1,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "permnameban",    do_permnameban,  POS_DEAD,      L1,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "probation",      do_probation,    POS_DEAD,      L7,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "protect",        do_protect,      POS_DEAD,      L1,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "reboo",          do_reboo,        POS_DEAD,      L3,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "reboot",         do_reboot,       POS_DEAD,      L3,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "autoreboot",     do_autoreboot,   POS_DEAD,      L3,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "set",            do_set,          POS_DEAD,      L4,  LOG_ALWAYS, TRUE,  FALSE, TRUE,  TRUE,  },
   { "settrap",        do_trapset,      POS_STANDING,   0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "shutdow",        do_shutdow,      POS_DEAD,      L1,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "shutdown",       do_shutdown,     POS_DEAD,      L1,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "wizlock",        do_wizlock,      POS_DEAD,      L2,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "checkip",        do_checkip,      POS_DEAD,      L5,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "classes",        do_classes,      POS_DEAD,      L8,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "force",          do_force,        POS_DEAD,      L7,  LOG_ALWAYS, TRUE,  FALSE, TRUE,  FALSE, },
   { "educate",        do_educate,      POS_DEAD,      L8,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "load",           do_load,         POS_DEAD,      L6,  LOG_ALWAYS, TRUE,  FALSE, TRUE,  FALSE, },
   { "moron",          do_moron,        POS_DEAD,      L5,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "newlock",        do_newlock,      POS_DEAD,      L4,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "nochannels",     do_nochannels,   POS_DEAD,      L5,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "noemote",        do_noemote,      POS_DEAD,      L5,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "noshout",        do_noshout,      POS_DEAD,      L5,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "notell",         do_notell,       POS_DEAD,      L5,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "nopray",         do_nopray,       POS_DEAD,      L5,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "noguild",        do_noguild,      POS_DEAD,      L5,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "guildmaster",    do_guildmaster,  POS_DEAD,      L5,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "silence",        do_silence,      POS_DEAD,      L5,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "pecho",          do_pecho,        POS_DEAD,      L6,  LOG_ALWAYS, TRUE,  FALSE, TRUE,  FALSE, },
   { "pardon",         do_pardon,       POS_DEAD,      L3,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "purge",          do_purge,        POS_DEAD,      L4,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "restore",        do_restore,      POS_DEAD,      L4,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "sla",            do_sla,          POS_DEAD,      L3,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "slay",           do_slay,         POS_DEAD,      L3,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "teleport",       do_transfer,     POS_DEAD,      L8,  LOG_ALWAYS, TRUE,  FALSE, TRUE,  FALSE, },
   { "transfer",       do_transfer,     POS_DEAD,      L8,  LOG_ALWAYS, TRUE,  FALSE, TRUE,  FALSE, },

   { "poofin",         do_bamfin,       POS_DEAD,      L8,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "limits",         do_limits,       POS_DEAD,      L8,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "limpurge",       do_limpurge,     POS_DEAD,      L7,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "maxlimits",      do_max_limits,   POS_DEAD,      L3,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "overmax",        do_overmax,      POS_DEAD,      L3,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "poofout",        do_bamfout,      POS_DEAD,      L8,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "freset",         do_force_reset,  POS_DEAD,      L5,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "gecho",          do_echo,         POS_DEAD,      L4,  LOG_ALWAYS, TRUE,  FALSE, TRUE,  FALSE, },
   { "sockwrite",      do_sockwrite,    POS_DEAD,      L4,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "badname",        do_badname,      POS_DEAD,      L7,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "astrip",         do_astrip,       POS_DEAD,      L5,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "freetell",       do_freetell,     POS_DEAD,      L8,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "holylight",      do_holylight,    POS_DEAD,      L8,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "hoardlist",      do_hordelist,    POS_DEAD,      L7,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "incognito",      do_incognito,    POS_DEAD,      L8,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "invis",          do_invis,        POS_DEAD,      L8,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "log",            do_log,          POS_DEAD,      L1,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "logspec",        do_log_spec,     POS_DEAD,      L5,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "nmemory",        do_new_mem,      POS_DEAD,      L7,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "memory",         do_memory,       POS_DEAD,      L7,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "mwhere",         do_mwhere,       POS_DEAD,      L7,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "newthief",       do_allow_thief,  POS_DEAD,      L5,  LOG_ALWAYS, FALSE, FALSE, FALSE, FALSE, },
   { "newrang",        do_allow_rang,   POS_DEAD,      L5,  LOG_ALWAYS, FALSE, FALSE, FALSE, FALSE, },
   { "newelem",        do_allow_elem,   POS_DEAD,      L5,  LOG_ALWAYS, FALSE, FALSE, FALSE, FALSE, },
   { "owhere",         do_owhere,       POS_DEAD,      L7,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "peace",          do_peace,        POS_DEAD,      L6,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "penalty",        do_penalty,      POS_DEAD,      L8,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "hoarder",        do_hoarder,      POS_DEAD,      L7,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "becho",          do_becho,        POS_DEAD,      L5,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "hecho",          do_hecho,        POS_DEAD,      L7,  LOG_ALWAYS, TRUE,  FALSE, TRUE,  FALSE, },
   { "echo",           do_recho,        POS_DEAD,      L6,  LOG_ALWAYS, TRUE,  FALSE, TRUE,  FALSE, },
   { "return",         do_return,       POS_DEAD,       0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "reward",         do_reward,       POS_DEAD,      L5,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "snoop",          do_snoop,        POS_DEAD,      L5,  LOG_ALWAYS, TRUE,  FALSE, TRUE,  FALSE, },
   /* { "dbase",          do_create_obj_list, POS_DEAD,   L5,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, }, */
   { "moralerep",      do_morale_report, POS_DEAD,     L7,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "newbierep",      do_newbie_report, POS_DEAD,     L6,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "lastsite",       do_lastsite,     POS_DEAD,      L5,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "stat",           do_stat,         POS_DEAD,      L7,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "moonphase",      do_moonphase,    POS_DEAD,      L5,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "irvcheck",       do_irvcheck,     POS_DEAD,      L7,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "materialcheck",  do_material_check, POS_DEAD,    L6,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "scheck",         do_scheck,       POS_DEAD,      L7,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "getpk",          do_getpk,        POS_DEAD,      L7,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "getprac",        do_getprac,      POS_DEAD,      L7,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "getquest",       do_getquest,     POS_DEAD,      L7,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "getip",          do_getip,        POS_DEAD,      L5,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "string",         do_string,       POS_DEAD,      L5,  LOG_ALWAYS, TRUE,  FALSE, TRUE,  FALSE, },
   { "switch",         do_switch,       POS_DEAD,      L6,  LOG_ALWAYS, TRUE,  FALSE, TRUE,  FALSE, },
   { "wizinvis",       do_invis,        POS_DEAD,      L8,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "vnum",           do_vnum,         POS_DEAD,      L4,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "vnumlist",       do_vnumlist,     POS_DEAD,      L2,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "zecho",          do_zecho,        POS_DEAD,      L4,  LOG_ALWAYS, TRUE,  FALSE, TRUE,  FALSE, },
   { "brands",         do_brands,       POS_DEAD,      L5,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "unbrands",       do_unbrands,     POS_DEAD,      L5,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "mpdump",         do_mpdump,       POS_DEAD,      L7,  LOG_NEVER,  TRUE,  FALSE, FALSE, FALSE, },
   { "mpstat",         do_mpstat,       POS_DEAD,      L7,  LOG_NEVER,  TRUE,  FALSE, FALSE, FALSE, },

   { "clone",          do_clone,        POS_DEAD,      L5,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },

   { "wiznet",         do_wiznet,       POS_DEAD,      L8,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "immtalk",        do_immtalk,      POS_DEAD,       0,  LOG_NORMAL, FALSE, FALSE, FALSE, TRUE,  },
   { "immrp",          do_immrp,        POS_DEAD,      L4,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "imotd",          do_imotd,        POS_DEAD,      L8,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   /*
   Put here to prevent interference with short forms of immtalk,
   immrp, and imotd - Wicket
   */
   { "immerse",        do_immerse,      POS_RESTING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { ":",              do_immtalk,      POS_DEAD,       0,  LOG_NORMAL, FALSE, FALSE, TRUE,  TRUE,  },
   { "builder",        do_builder,      POS_SLEEPING,   0,  LOG_NORMAL, FALSE, FALSE, FALSE, TRUE,  },
   { "specialize",     do_specialize,   POS_RESTING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "renam",          do_renam,        POS_DEAD,      L7,  LOG_ALWAYS, FALSE, FALSE, FALSE, FALSE, },
   { "rename",         do_rename,       POS_DEAD,      L7,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "bd",             do_builder,      POS_SLEEPING,   0,  LOG_NORMAL, FALSE, FALSE, FALSE, TRUE,  },
   { "smote",          do_smote,        POS_DEAD,      L8,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "prefi",          do_prefi,        POS_DEAD,      L8,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "prefix",         do_prefix,       POS_DEAD,      L8,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "alchemy",        do_alchemy,      POS_STANDING,   0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "protection",     do_protection_heat_cold, POS_STANDING, 0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "bandage",        do_bandage,      POS_RESTING,    0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "mark",           do_mark,         POS_RESTING,    0,  LOG_ALWAYS, FALSE, FALSE, FALSE, FALSE, },
   { "charge",         do_charge,       POS_FIGHTING,   0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "blitz",          do_blitz,        POS_FIGHTING,   0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "lurk",           do_lurk,         POS_STANDING,   0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "surround",       do_surround,     POS_FIGHTING,   0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "ctell",          do_ctell,        POS_DEAD,       0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "cecho",          do_cecho,        POS_DEAD,      L6,  LOG_ALWAYS, TRUE,  FALSE, TRUE,  FALSE, },
   { "conference",     do_conference,   POS_DEAD,      L7,  LOG_ALWAYS, TRUE,  FALSE, TRUE,  FALSE, },
   { "tinker",         do_tinker,       POS_STANDING,   0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "forge",          do_forge_weapon, POS_STANDING,   0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "senseevil",      do_sense_evil,   POS_STANDING,   0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "armor",          do_armor_of_god, POS_STANDING,   0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "extract",        do_extract,      POS_STANDING,   0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "summon",         do_summon,       POS_FIGHTING,   0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "hirecrew",       do_hire_crew,    POS_STANDING,   0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "impale",         do_impale,       POS_FIGHTING,   0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "swing",          do_swing,        POS_FIGHTING,   0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "lastlogin",      do_last_logon,   POS_DEAD,      L7,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "lastlogon",      do_last_logon,   POS_DEAD,      L7,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "nofight",        do_nofight,      POS_DEAD,      L2,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "revoke",         do_revoke,       POS_DEAD,      ML,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "telekinesis",    do_telekinesis,  POS_FIGHTING,   0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "wind shear",     do_wind_shear,   POS_FIGHTING,   0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "sonic belch",    do_sonic_belch,  POS_FIGHTING,   0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "troll fart",     do_troll_fart,   POS_FIGHTING,   0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "star shower",    do_star_shower,  POS_FIGHTING,   0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "davatar assault", do_davatar_assault, POS_FIGHTING, 0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "criminal",       do_crimnote,     POS_DEAD,       0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "ancient",        do_deathnote,    POS_DEAD,       0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "banknote",       do_banknote,     POS_DEAD,       0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "clannote",       do_clannote,     POS_DEAD,      L7,  LOG_NORMAL, TRUE,  FALSE, TRUE,  FALSE, },
   { "penetrating thrust", do_penetrating_thrust, POS_FIGHTING, 0, LOG_NORMAL, FALSE, TRUE, FALSE, FALSE},
   { "wagon strike",   do_wagon_strike, POS_FIGHTING,   0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "megaslash",      do_megaslash,    POS_FIGHTING,   0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },

   { "clanfor",        do_clanfor,      POS_STANDING,   0,  LOG_NORMAL, FALSE, FALSE, TRUE,  FALSE, },
   { "clanform",       do_clanform,     POS_STANDING,   0,  LOG_NORMAL, TRUE,  FALSE, TRUE,  FALSE, },
   { "clandissolv",    do_clandissolv,  POS_STANDING,   0,  LOG_NORMAL, FALSE, FALSE, TRUE,  FALSE, },
   { "clandissolve",   do_clandissolve, POS_STANDING,   0,  LOG_NORMAL, TRUE,  FALSE, TRUE,  FALSE, },
   { "claninduc",      do_claninduc,    POS_STANDING,   0,  LOG_NORMAL, FALSE, FALSE, TRUE,  FALSE, },
   { "claninduct",     do_claninduct,   POS_STANDING,   0,  LOG_NORMAL, TRUE,  FALSE, TRUE,  FALSE, },
   { "clantransfe",    do_clantransfe,  POS_STANDING,   0,  LOG_NORMAL, FALSE, FALSE, TRUE,  TRUE,  },
   { "clantransfer",   do_clantransfer, POS_STANDING,   0,  LOG_NORMAL, TRUE,  FALSE, TRUE,  TRUE,  },
   { "clanqui",        do_clanqui,      POS_STANDING,   0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "clanquit",       do_clanquit,     POS_STANDING,   0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "clanpromote",    do_clan_promote, POS_STANDING,  L2,  LOG_NORMAL, TRUE,  FALSE, TRUE,  FALSE, },
   { "clanappoint",    do_clanappoint,  POS_STANDING,   0,  LOG_NORMAL, TRUE,  FALSE, TRUE,  FALSE, },
   { "clist",          do_clist,        POS_DEAD,      L7,  LOG_NORMAL, TRUE,  FALSE, TRUE,  FALSE, },
   { "clantitle",      do_clantitle,    POS_DEAD,      L6,  LOG_NORMAL, TRUE,  FALSE, TRUE,  FALSE, },
   { "cspeak",         do_cctell,       POS_RESTING,    0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "cctell",         do_cctell,       POS_RESTING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "clantell",       do_cctell,       POS_RESTING,    0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "dossier",        do_dossier,      POS_DEAD,      L8,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "roster",         do_roster,       POS_DEAD,       0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "recruit",        do_recruit,      POS_STANDING,   0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "wecho",          do_wecho,        POS_DEAD,      L6,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "clecho",         do_clecho,       POS_DEAD,      L6,  LOG_NORMAL, TRUE,  FALSE, TRUE,  FALSE, },
   { "purify",         do_purify,       POS_FIGHTING,   0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "dumpalchem",     do_dump_alchem,  POS_DEAD,      ML,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "dumpalchemy",    do_dump_alchemy, POS_DEAD,      ML,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "wizireport",     do_wizireport,   POS_DEAD,      ML,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "clanpublic",     do_clan_public,  POS_STANDING,  L6,  LOG_NORMAL, TRUE,  FALSE, TRUE,  FALSE, },
   { "clanrace",       do_clan_race,    POS_STANDING,  L6,  LOG_NORMAL, TRUE,  FALSE, TRUE,  FALSE, },
   { "clanclass",      do_clan_class,   POS_STANDING,  L6,  LOG_NORMAL, TRUE,  FALSE, TRUE,  FALSE, },
   { "email",          do_email,        POS_STANDING,  L8,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "scribe",         do_scribe,       POS_STANDING,  30,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "charm",          do_charm,        POS_DEAD,      L4,  LOG_ALWAYS, TRUE,  FALSE, TRUE,  FALSE, },
   { "uncharm",        do_uncharm,      POS_DEAD,      L4,  LOG_NORMAL, TRUE,  FALSE, TRUE,  FALSE, },
   { "noseeperm",      do_nosee_perm,   POS_DEAD,      L6,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "psistorm",       do_psistorm,     POS_FIGHTING,   0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "jdancestaves",   do_jdancestaves, POS_FIGHTING,   0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "myell",          do_myell,        POS_RESTING,   L7,  LOG_NORMAL, TRUE,  TRUE,  TRUE,  FALSE, },
   { "activebranding", do_active_brand, POS_DEAD,      L5,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "clanalias",      do_clanalias,    POS_DEAD,      L6,  LOG_NORMAL, TRUE,  FALSE, TRUE,  FALSE, },
   { "acidfire",       do_acidfire,     POS_FIGHTING,   0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "suraccept",      do_suraccept,    POS_DEAD,      L8,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "surname",        do_surname,      POS_SLEEPING,   0,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "relname",        do_relname,      POS_DEAD,      L8,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "resurrec",       do_resurrec,     POS_DEAD,      L2,  LOG_ALWAYS, FALSE, FALSE, FALSE, FALSE, },
   { "resurrect",      do_resurrect,    POS_DEAD,      L2,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "devour",         do_devour,       POS_FIGHTING,   0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "carebearstare",  do_carebearstare, POS_FIGHTING,   0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "krazyeyes",      do_krazyeyes,    POS_FIGHTING,   0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "moniker",        do_moniker,      POS_STANDING,   0,  LOG_ALWAYS, FALSE, FALSE, FALSE, FALSE, },
   { "muster",         do_muster,       POS_STANDING,   0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "pretitle",       do_pre_title,    POS_DEAD,      L5,  LOG_ALWAYS, TRUE,  FALSE, FALSE, FALSE, },
   { "hemote",         do_hemote,       POS_DEAD,      L8,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "stalking",       do_stalking,     POS_STANDING,   0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "field",          do_field_surgery, POS_STANDING,  0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
   { "lottery",        do_lottery,      POS_RESTING,    0,  LOG_NORMAL, TRUE,  FALSE, FALSE, TRUE,  },
   { "deathfire",      do_deathfire_missiles, POS_FIGHTING,   0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "abyssal",        do_abyssal_claws, POS_FIGHTING,   0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "destruction",    do_destruction_shockwave, POS_FIGHTING,   0,  LOG_NORMAL, FALSE, TRUE,  FALSE, FALSE, },
   { "brandpower",     do_brandpower,   POS_DEAD,      L5,  LOG_NORMAL, TRUE,  FALSE, FALSE, FALSE, },
   { "nap",            do_nap,          POS_RESTING,    1,  LOG_NORMAL, TRUE,  TRUE,  FALSE, TRUE, },
   { "hyper",          do_hyper,        POS_RESTING,    1,  LOG_NORMAL, TRUE,  TRUE,  FALSE, TRUE, },

   /*
   * End of list.
   */
   { "",               NULL,            POS_DEAD,       0,  LOG_NORMAL, FALSE, FALSE, FALSE, FALSE, },
};

bool string_contains_godname(char* string)
{
   int counter;

   for (counter = 0; worship_table[counter].name != NULL; counter++)
   {
      if (stristr(string, worship_table[counter].name))
      {
         return TRUE;
      }
   }
   for
   (
      counter = 0;
      (
         counter < MAX_TEMP_GODS &&
         temp_worship_table[counter].name != NULL
      );
      counter++
   )
   {
      if (stristr(string, temp_worship_table[counter].name))
      {
         return TRUE;
      }
   }
   return FALSE;
}

/*
* The main entry point for executing commands.
* Can be recursively called from 'at', 'order', 'force'.
*/
void interpret(CHAR_DATA* ch, char* argument)
{
   char command[MAX_INPUT_LENGTH];
   char logline[MAX_INPUT_LENGTH];
   char logline_arg[MAX_INPUT_LENGTH];
   SNOOP_DATA* snoops;
   int cmd;
   int trust;
   bool found;
   extern char* target_name;  /* clear every time */
   bool allowrp;

   target_name = "";  /* Target name cleared every interpret -Fizz */
   /* TEMP FIX -- XURINOS */
   if (ch == NULL)
   {
      bug("BUG (interpret): ch == NULL", 0);
      return;
   }
   if (!check_room(ch, TRUE, "BUG (interpret): ch->in_room == NULL"))
   {
      return;
   }

   /*
   * Strip leading spaces.
   */
   while ( isspace(*argument) )
   {
      argument++;
   }
   if (argument[0] == '\0')
   {
      return;
   }

   /*
   * Implement freeze command.
   */
   if
   (
      !IS_NPC(ch) &&
      IS_SET(ch->act, PLR_FREEZE)
   )
   {
      send_to_char("You are completely frozen!\n\r", ch);
      return;
   }

   /*
   Grab the command word.
   Special parsing so ' can be a command,
   also no spaces needed after punctuation.
   */
   strcpy(logline, argument);
   if
   (
      !isalpha(argument[0]) &&
      !isdigit(argument[0])
   )
   {
      command[0] = argument[0];
      command[1] = '\0';
      argument++;
      while (isspace(*argument))
      {
         argument++;
      }
   }
   else
   {
      argument = one_argument(argument, command);
   }
   strcpy(logline_arg, argument);

   /*
   Look for command in command table.
   */
   found = FALSE;
   trust = get_trust(ch);

   if
   (
      IS_SET(ch->wiznet, WIZ_ALLOWRP) &&
      IS_IMMORTAL(ch)
   )
   {
      allowrp = TRUE;
   }
   else
   {
      allowrp = FALSE;
   }

   for (cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++)
   {
      if
      (
         (
            cmd_table[cmd].level <= trust ||
            (
               allowrp &&
               cmd_table[cmd].allow_rp
            )
         ) &&
         command[0] == cmd_table[cmd].name[0] &&
         !str_prefix(command, cmd_table[cmd].name)
      )
      {
         found = TRUE;
         break;
      }
   }

   /*
   Log and snoop.
   */
   if (cmd_table[cmd].log == LOG_NEVER)
   {
      logline[0] = '\0';
      logline_arg[0] = '\0';
   }

   if
   (
      logline[0] != '\0' &&
      (
         (
            !IS_NPC(ch) &&
            IS_SET(ch->act, PLR_LOG)
         ) ||
         fLogAll ||
         cmd_table[cmd].log == LOG_ALWAYS
      )
   )
   {
      sprintf
      (
         log_buf,
         "Log %s: %s %s",
         cmd_table[cmd].name,
         ch->name,
         logline_arg
      );
      log_string(log_buf);
      wiznet("$t", ch, (OBJ_DATA*)log_buf, WIZ_SECURE, 0, get_trust(ch));
   }

   if (string_contains_godname(logline_arg))
   {
      sprintf(log_buf, "%s is using your name: %s", ch->name, logline);
      wiznet(
         "$t",
         ch,
         (OBJ_DATA*)log_buf,
         0,
         WIZ_NAME,
         get_trust(ch));
   }

   if
   (
      !IS_NPC(ch) ||
      IS_AFFECTED(ch, AFF_CHARM) ||
      ch->desc != NULL
   )
   {
      if (is_vulgar(logline_arg, vulgar_table))
      {
         if
         (
            IS_AFFECTED(ch, AFF_CHARM) &&
            ch->master
         )
         {
            sprintf
            (
               log_buf,
               "(%s's) %s is Swearing: %s",
               ch->master->name,
               ch->name,
               logline
            );
         }
         else
         {
            sprintf
            (
               log_buf,
               "%s is Swearing: %s",
               ch->name,
               logline
            );
         }
         log_string(log_buf);
         wiznet("$t", ch, (OBJ_DATA*)log_buf, WIZ_SWEAR, 0, get_trust(ch));
      }
      else if (is_vulgar(logline_arg, cyber_table))
      {
         if
         (
            IS_AFFECTED(ch, AFF_CHARM) &&
            ch->master
         )
         {
            sprintf
            (
               log_buf,
               "(%s's) %s might be cybering: %s",
               ch->master->name,
               ch->name,
               logline
            );
         }
         else
         {
            sprintf
            (
               log_buf,
               "%s might be cybering: %s",
               ch->name,
               logline
            );
         }
         log_string(log_buf);
         wiznet("$t", ch, (OBJ_DATA*)log_buf, WIZ_CYBER, 0, get_trust(ch));
      }
   }
   /*
   MUSIC_DISABLED
   if
   (
      strstr(logline, ")") != NULL &&
      (
         stristr(logline, "!!MUSIC(") != NULL ||
         stristr(logline, "!!SOUND(") != NULL
      )
   )
   {
      send_to_char("Sorry, no sending sound or music codes.\n\r", ch);
      return;
   }
   */
   if ( ch->desc != NULL && ch->desc->snoops != NULL )
   {
      snoops = ch->desc->snoops;
      while (snoops != NULL)
      {
         write_to_buffer(snoops->snoop_by, ch->name, 0);
         write_to_buffer(snoops->snoop_by, "% ",     2);
         write_to_buffer(snoops->snoop_by, logline,  0);
         write_to_buffer(snoops->snoop_by, "\n\r",   2);
         snoops = snoops->next;
      }
   }

   if
   (
      found &&
      is_affected(ch, gsn_charm_person) &&
      (
         cmd_table[cmd].name == "wanted" ||
         cmd_table[cmd].name == "practice" ||
         cmd_table[cmd].name == "train" ||
         cmd_table[cmd].name == "delete" ||
         cmd_table[cmd].name == "pray" ||
         cmd_table[cmd].name == "induct" ||
         cmd_table[cmd].name == "note" ||
         cmd_table[cmd].name == "builder" ||
         cmd_table[cmd].name == "bd" ||
         cmd_table[cmd].name == "bug" ||
         cmd_table[cmd].name == "typo" ||
         cmd_table[cmd].name == "idea" ||
         cmd_table[cmd].name == "penalty" ||
         cmd_table[cmd].name == "news" ||
         cmd_table[cmd].name == "ignore"
      )
   )
   {
      send_to_char("Somehow, you resist the urge to do that.\n\r", ch);
      return;
   }

   if
   (
      (
         found &&
         cmd_table[cmd].name != "immtalk" &&
         cmd_table[cmd].name != "affects" &&
         cmd_table[cmd].name != ":" &&
         cmd_table[cmd].name != "return" &&
         cmd_table[cmd].log != LOG_NEVER &&
         cmd_table[cmd].level < L8  /* allow all imm abilities */
      ) ||
      !found
   )
   {
      /* freeze spell */
      if (is_affected(ch, gsn_freeze))
      {
         send_to_char("You have been trapped in ice and can't do that.\n\r", ch);
         return;
      }

      /* stone */
      if (is_affected(ch, gsn_stone))
      {
         send_to_char("You have been turned to stone and are incapable of doing that.\n\r", ch);
         return;
      }
      /* Timestop implementation */
      if (is_affected(ch, gsn_timestop))
      {
         send_to_char("Time has frozen for you and you are trapped between ticks of the clock...\n\r", ch);
         return;
      }
      /* Power word Stun implementation */
      if (is_affected(ch, gsn_power_word_stun) )
      {
         if
         (
            (
               found &&
               cmd_table[cmd].name != "score" &&
               cmd_table[cmd].name != "look" &&
               cmd_table[cmd].name != "who" &&
               cmd_table[cmd].name != "group" &&
               cmd_table[cmd].name != "gt" &&
               cmd_table[cmd].name != "gtell"
            ) ||
            !found
         )
         {
            send_to_char
            (
               "You are far too stunned by the power word to move.\n\r",
               ch
            );
            return;
         }
      }
   }

   if
   (
      !IS_NPC(ch) &&
      ch->pcdata->death_status == HAS_DIED &&
      (
         (
            found &&
            cmd_table[cmd].level < L8 && /* allow all imm abilities */
            !cmd_table[cmd].ghost
         ) ||
         !found
      )
   )
   {
      send_to_char("You are a hovering spirit, you can't do that anymore.\n\r", ch);
      return;
   }

   if
   (
      !IS_NPC(ch) &&
      ch->pcdata->spirit_room != NULL &&
      found
   )
   {
      spirit_command(ch, cmd, argument);
      return;
   }

   if
   (
      is_affected(ch, gsn_justice_brand_wrath) &&
      number_percent() > 70 &&
      (
         cmd_table[cmd].name == "cast" ||
         cmd_table[cmd].name == "c" ||
         cmd_table[cmd].name == "supplicate" ||
         cmd_table[cmd].name == "flee"
      )
   )
   {
      act
      (
         "$n panics as $e hears the cries of justice!",
         ch,
         NULL,
         NULL,
         TO_ROOM
      );
      if (number_percent() < 50)
      {
         send_to_char
         (
            "Cries of injustice below out loudly distracting you!\n\r",
            ch
         );
      }
      else
      {
         send_to_char
         (
            "Those dealt injustices moan aloud, disturbing your"
            " concentration.\n\r",
            ch
         );
      }
      WAIT_STATE(ch, 24);
      return;
   }

   if (!found)
   {

      /*
      * Look for command in socials table.
      */
      if (!check_social(ch, command, argument))
      {
         send_to_char("Huh?\n\r", ch);
      }
      return;
   }
   /* Get correct position */
   fix_fighting(ch);
   /*
   Character not in position for command?
   */
   update_pos(ch);
   if (ch->position < cmd_table[cmd].position)
   {
      switch (ch->position)
      {
         case POS_DEAD:
         send_to_char( "Lie still; you are DEAD.\n\r", ch );
         break;

         case POS_MORTAL:
         case POS_INCAP:
         send_to_char( "You are hurt far too bad for that.\n\r", ch );
         break;

         case POS_STUNNED:
         send_to_char( "You are too stunned to do that.\n\r", ch );
         break;

         case POS_SLEEPING:
         send_to_char( "In your dreams, or what?\n\r", ch );
         break;

         case POS_RESTING:
         send_to_char( "Nah... You feel too relaxed...\n\r", ch);
         break;

         case POS_SITTING:
         send_to_char( "Better stand up first.\n\r", ch);
         break;

         case POS_FIGHTING:
         send_to_char( "No way!  You are still fighting!\n\r", ch);
         break;

      }
      return;
   }

   if
   (
      cmd_table[cmd].level >= L8 &&
      IS_SET(ch->comm2, COMM_WIZ_REVOKE)
   )
   {
      send_to_char("Your powers have been revoked!\n\r", ch);
      return;
   }
   /*
   * Hide parsing.
   */
   if (cmd_table[cmd].hide)
   {
      un_hide(ch, NULL);
   }

   /*
   * Dispatch the command.
   */
   (*cmd_table[cmd].do_fun) ( ch, argument );

   tail_chain( );
   return;
}



bool check_social( CHAR_DATA *ch, char *command, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   int cmd;
   bool found;

   found  = FALSE;
   for ( cmd = 0; social_table[cmd].name[0] != '\0'; cmd++ )
   {
      if ( command[0] == social_table[cmd].name[0]
      &&   !str_prefix( command, social_table[cmd].name ) )
      {
         found = TRUE;
         break;
      }
   }

   if ( !found )
   return FALSE;

   if ( !IS_NPC(ch) && IS_SET(ch->comm, COMM_NOEMOTE) )
   {
      send_to_char( "You are anti-social!\n\r", ch );
      return TRUE;
   }
   update_pos(ch);
   switch ( ch->position )
   {
      case POS_DEAD:
      send_to_char( "Lie still; you are DEAD.\n\r", ch );
      return TRUE;

      case POS_INCAP:
      case POS_MORTAL:
      send_to_char( "You are hurt far too bad for that.\n\r", ch );
      return TRUE;

      case POS_STUNNED:
      send_to_char( "You are too stunned to do that.\n\r", ch );
      return TRUE;

      case POS_SLEEPING:
      /*
      * I just know this is the path to a 12 inch 'if' statement.  :(
      * But two players asked for it already!  -- Furey
      */
      if ( !str_cmp( social_table[cmd].name, "snore" ) )
      break;
      send_to_char( "In your dreams, or what?\n\r", ch );
      return TRUE;

   }

   one_argument( argument, arg );
   victim = NULL;
   if ( arg[0] == '\0' )
   {
      act( social_table[cmd].others_no_arg, ch, NULL, victim, TO_ROOM    );
      act( social_table[cmd].char_no_arg,   ch, NULL, victim, TO_CHAR    );
   }
   else if ( ( victim = get_char_room( ch, arg ) ) == NULL )
   {
      send_to_char( "They aren't here.\n\r", ch );
   }
   else if ( victim == ch )
   {
      act( social_table[cmd].others_auto,   ch, NULL, victim, TO_ROOM    );
      act( social_table[cmd].char_auto,     ch, NULL, victim, TO_CHAR    );
   }
   else
   {
      act( social_table[cmd].others_found,  ch, NULL, victim, TO_NOTVICT );
      act( social_table[cmd].char_found,    ch, NULL, victim, TO_CHAR    );
      act( social_table[cmd].vict_found,    ch, NULL, victim, TO_VICT    );

      if ( !IS_NPC(ch) && IS_NPC(victim)
      &&   !IS_AFFECTED(victim, AFF_CHARM)
      &&   IS_AWAKE(victim)
      &&   victim->desc == NULL)
      {
         switch ( number_bits( 4 ) )
         {
            case 0: case 9: case 10:

            case 1: case 2: case 3: case 4:
            case 5: case 6: case 7: case 8:
            act( social_table[cmd].others_found,
            victim, NULL, ch, TO_NOTVICT );
            act( social_table[cmd].char_found,
            victim, NULL, ch, TO_CHAR    );
            act( social_table[cmd].vict_found,
            victim, NULL, ch, TO_VICT    );
            /*        break;

            case 9: case 10: case 11: case 12:
            act( "$n slaps $N.",  victim, NULL, ch, TO_NOTVICT );
            act( "You slap $N.",  victim, NULL, ch, TO_CHAR    );
            act( "$n slaps you.", victim, NULL, ch, TO_VICT    );
            break;*/
         }
      }
   }

   return TRUE;
}



/*
   Return true if an argument is completely numeric.
*/
bool is_number ( char *arg )
{

   if ( *arg == '+' || *arg == '-' )
   arg++;

   if ( *arg == '\0' )
   return FALSE;

   for ( ; *arg != '\0'; arg++ )
   {
      if ( !isdigit( *arg ) )
      return FALSE;
   }

   return TRUE;
}



/*
   Given a string like 14.foo, return 14 and 'foo'
*/
int number_argument( char *argument, char *arg )
{
   char *pdot;
   int number;

   for ( pdot = argument; *pdot != '\0'; pdot++ )
   {
      if ( *pdot == '.' )
      {
         *pdot = '\0';
         number = atoi( argument );
         *pdot = '.';
         strcpy( arg, pdot+1 );
         return number;
      }
   }

   strcpy( arg, argument );
   return 1;
}

/*
   Given a string like 14*foo, return 14 and 'foo'
*/
int mult_argument(char *argument, char *arg)
{
   char *pdot;
   int number;

   for ( pdot = argument; *pdot != '\0'; pdot++ )
   {
      if ( *pdot == '*' )
      {
         *pdot = '\0';
         number = atoi( argument );
         *pdot = '*';
         strcpy( arg, pdot+1 );
         return number;
      }
   }

   strcpy( arg, argument );
   return 1;
}



/*
* Pick off one argument from a string and return the rest.
* Understands quotes.
*/
char *one_argument( char *argument, char *arg_first )
{
   char cEnd;

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
      *arg_first = LOWER(*argument);
      arg_first++;
      argument++;
   }
   *arg_first = '\0';

   while ( isspace(*argument) )
   argument++;

   return argument;
}

/*
* Pick off one argument from a string and return the rest.
* Does not strip trailing spaces
* Understands quotes.
*/
char *one_argument_space( char *argument, char *arg_first )
{
   char cEnd;

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
      *arg_first = LOWER(*argument);
      arg_first++;
      argument++;
   }
   *arg_first = '\0';

   return argument;
}

/*
* Pick off one argument from a string and return the rest.
* Understands quotes. case sensitive
*/
char* one_argument_cs(char* argument, char* arg_first)
{
   char cEnd = ' ';

   while (isspace(*argument))
   {
      argument++;
   }
   if
   (
      *argument == '\'' ||
      *argument == '"'
   )
   {
      cEnd = *argument++;
   }
   while (*argument != '\0')
   {
      if (*argument == cEnd)
      {
         argument++;
         break;
      }
      *arg_first = *argument;
      arg_first++;
      argument++;
   }
   *arg_first = '\0';

   while (isspace(*argument))
   {
      argument++;
   }
   return argument;
}

/*
* Contributed by Alander.
*/
void do_commands( CHAR_DATA *ch, char *argument )
{
   HELP_DATA *pHelp;
   NOTE_DATA *pnote;
   char buf[MAX_STRING_LENGTH];
   bool help_found;
   int cmd;
   int col;

   send_to_char("\n\r", ch);
   col = 0;
   for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
   {
      if ( cmd_table[cmd].level <=  LEVEL_HERO
      &&   cmd_table[cmd].level <= get_trust( ch )
      &&   cmd_table[cmd].show)
      {
         help_found = FALSE;
         for ( pHelp = help_first; pHelp != NULL; pHelp = pHelp->next )
         if ( is_name( cmd_table[cmd].name, pHelp->keyword ) )
         help_found = TRUE;
         for (pnote = help_list; pnote != NULL; pnote = pnote->next)
         if ( is_name( cmd_table[cmd].name, pnote->subject ) && is_note_to(ch, pnote))
         help_found = TRUE;
         sprintf(buf, "%-12s", cmd_table[cmd].name);
         if (IS_SET(ch->comm, COMM_ANSI) && help_found)
         sprintf(buf, "\x01b[1;37m%-12s\x01b[0;37m",
         cmd_table[cmd].name);
         send_to_char(buf, ch);
         if (++col % 6 == 0)
         send_to_char("\n\r", ch);
      }
   }
   if ( col % 6 != 0 )
   send_to_char( "\n\r", ch );
   return;
}

void do_wizhelp( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   int cmd;
   int low = 0;
   int high = 0;
   BUFFER *buffer;
   char ability_list[MAX_LEVEL - LEVEL_HERO][MAX_STRING_LENGTH];
   char ability_columns[MAX_LEVEL - LEVEL_HERO];
   int level;
   bool found = FALSE;
   char buf[MAX_STRING_LENGTH];
   bool show_hidden = FALSE;

   argument = one_argument(argument, arg);
   while (arg[0] != '\0')
   {
      if (!str_cmp(arg, "hidden") && !show_hidden)
      {
         show_hidden = TRUE;
      }
      else if (is_number(arg) && low == 0)
      {
         low = atoi(arg);
      }
      else if (is_number(arg) && high == 0)
      {
         high = atoi(arg);
      }
      else
      {
         send_to_char("Wrong syntax; read help 'wizhelp'.\n\r", ch);
         return;
      }
      argument = one_argument(argument, arg);
   }
   if (high < low)
   {
      int temp = high;
      high = low;
      low = temp;
   }
   if (low == 0)
   {
      low = IM;
   }
   if (high == 0)
   {
      high = get_trust(ch);
   }
   /* If you put the numbers in reverse order, fix them */
   if (high < low)
   {
      int temp = high;
      high = low;
      low = temp;
   }
   /*
   This is for wizcommands,
   the ranges allowable are anything between immortal level,
   and your trust
   */
   if
   (
      low < IM ||
      high > get_trust(ch)
   )
   {
      sprintf(buf, "Levels must be between %d and your level.\n\r", IM);
      send_to_char(buf, ch);
      return;
   }
   for (level = LEVEL_IMMORTAL; level < MAX_LEVEL + 1; level++)
   {
      ability_columns[level - LEVEL_IMMORTAL] = 0;
      ability_list[level - LEVEL_IMMORTAL][0] = '\0';
   }
   for (cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++)
   {
      if
      (
         cmd_table[cmd].level <= high &&
         cmd_table[cmd].level >= low &&
         (
            show_hidden ||
            (
               !show_hidden &&
               cmd_table[cmd].show
            )
         )
      )
      {
         found = TRUE;
         level = cmd_table[cmd].level;
         sprintf(buf, "%-14s", cmd_table[cmd].name);
         if (ability_list[level - LEVEL_IMMORTAL][0] == '\0')
         {
            sprintf
            (
               ability_list[level - LEVEL_IMMORTAL],
               "\n\rLevel %2d: %s",
               level,
               buf
            );
         }
         else /* append */
         {
            if (++ability_columns[level - LEVEL_IMMORTAL] % 5 == 0)
            {
               strcat(ability_list[level - LEVEL_IMMORTAL], "\n\r          ");
            }
            strcat(ability_list[level - LEVEL_IMMORTAL], buf);
         }
      }
   }
   /* return results */

   if (!found)
   {
      send_to_char("No abilities found.\n\r", ch);
      return;
   }

   buffer = new_buf();
   for (level = LEVEL_IMMORTAL; level < MAX_LEVEL + 1; level++)
   {
      if (ability_list[level - LEVEL_IMMORTAL][0] != '\0')
      {
         add_buf(buffer, ability_list[level - LEVEL_IMMORTAL]);
      }
   }
   add_buf(buffer, "\n\r");
   page_to_char(buf_string(buffer), ch);
   free_buf(buffer);
}

void do_rphelp(CHAR_DATA* ch, char* argument)
{
   char arg[MAX_INPUT_LENGTH];
   int cmd;
   int low = 0;
   int high = 0;
   BUFFER *buffer;
   char ability_list[MAX_LEVEL - LEVEL_HERO][MAX_STRING_LENGTH];
   char ability_columns[MAX_LEVEL - LEVEL_HERO];
   int level;
   bool found = FALSE;
   char buf[MAX_STRING_LENGTH];
   bool show_hidden = FALSE;

   if (!IS_IMMORTAL(ch))
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }
   if
   (
      get_trust(ch) < L3 &&
      !IS_SET(ch->wiznet, WIZ_ALLOWRP)
   )
   {
      send_to_char("You do not have allow rp right now.\n\r", ch);
      return;
   }

   argument = one_argument(argument, arg);
   while (arg[0] != '\0')
   {
      if (!str_cmp(arg, "hidden") && !show_hidden)
      {
         show_hidden = TRUE;
      }
      else if (is_number(arg) && low == 0)
      {
         low = atoi(arg);
      }
      else if (is_number(arg) && high == 0)
      {
         high = atoi(arg);
      }
      else
      {
         send_to_char("Wrong syntax; read help 'rphelp'.\n\r", ch);
         return;
      }
      argument = one_argument(argument, arg);
   }
   if (high < low)
   {
      int temp = high;
      high = low;
      low = temp;
   }
   if (low == 0)
   {
      low = IM;
   }
   if (high == 0)
   {
      high = ML;
   }
   /* If you put the numbers in reverse order, fix them */
   if (high < low)
   {
      int temp = high;
      high = low;
      low = temp;
   }
   /*
   This is for wizcommands,
   the ranges allowable are anything between immortal level,
   and your trust
   */
   if
   (
      low < IM ||
      high > ML
   )
   {
      sprintf(buf, "Levels must be between %d and %d.\n\r", IM, ML);
      send_to_char(buf, ch);
      return;
   }
   for (level = LEVEL_IMMORTAL; level <= MAX_LEVEL; level++)
   {
      ability_columns[level - LEVEL_IMMORTAL] = 0;
      ability_list[level - LEVEL_IMMORTAL][0] = '\0';
   }
   for (cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++)
   {
      if
      (
         cmd_table[cmd].level <= high &&
         cmd_table[cmd].level >= low &&
         (
            show_hidden ||
            (
               !show_hidden &&
               cmd_table[cmd].show
            )
         ) &&
         cmd_table[cmd].allow_rp
      )
      {
         found = TRUE;
         level = cmd_table[cmd].level;
         sprintf(buf, "%-14s", cmd_table[cmd].name);
         if (ability_list[level - LEVEL_IMMORTAL][0] == '\0')
         {
            sprintf
            (
               ability_list[level - LEVEL_IMMORTAL],
               "\n\rLevel %2d: %s",
               level,
               buf
            );
         }
         else /* append */
         {
            if (++ability_columns[level - LEVEL_IMMORTAL] % 5 == 0)
            {
               strcat(ability_list[level - LEVEL_IMMORTAL], "\n\r          ");
            }
            strcat(ability_list[level - LEVEL_IMMORTAL], buf);
         }
      }
   }
   /* return results */

   if (!found)
   {
      send_to_char("No abilities found.\n\r", ch);
      return;
   }

   buffer = new_buf();
   add_buf(buffer, "Abilities granted by Allow RP:\n\r\n\r");
   for (level = LEVEL_IMMORTAL; level <= MAX_LEVEL; level++)
   {
      if (ability_list[level - LEVEL_IMMORTAL][0] != '\0')
      {
         add_buf(buffer, ability_list[level - LEVEL_IMMORTAL]);
      }
   }
   add_buf(buffer, "\n\r");
   page_to_char(buf_string(buffer), ch);
   free_buf(buffer);
}

void spirit_command(CHAR_DATA *ch, int cmd, char * argument){
   ROOM_INDEX_DATA * home = ch->in_room;

   if ((cmd_table[cmd].name == "return")){
      do_return(ch, "");
      return;
   }
   if ((cmd_table[cmd].name == "north") ||
   (cmd_table[cmd].name == "south") ||
   (cmd_table[cmd].name == "east") ||
   (cmd_table[cmd].name == "west") ||
   (cmd_table[cmd].name == "up") ||
   (cmd_table[cmd].name == "down") ||
   (cmd_table[cmd].name == "d") ||
   (cmd_table[cmd].name == "u") ||
   (cmd_table[cmd].name == "n") ||
   (cmd_table[cmd].name == "s") ||
   (cmd_table[cmd].name == "e") ||
   (cmd_table[cmd].name == "w")){

      do_spirit_move(ch, cmd_table[cmd].name);
      return;
   }
   if ((cmd_table[cmd].name != "score")
   && (cmd_table[cmd].name != "credits")
   && (cmd_table[cmd].name != "commands")
   && (cmd_table[cmd].name != "areas")
   && (cmd_table[cmd].name != "score")
   && (cmd_table[cmd].name != "time")
   && (cmd_table[cmd].name != "weather")
   && (cmd_table[cmd].name != "l")
   && (cmd_table[cmd].name != "lo")
   && (cmd_table[cmd].name != "look")
   && (cmd_table[cmd].name != "who")
   && (cmd_table[cmd].name != "whois")
   && (cmd_table[cmd].name != "exits")
   && (cmd_table[cmd].name != "affects")
   && (cmd_table[cmd].name != "gt")
   && (cmd_table[cmd].name != "gtell")
   && (cmd_table[cmd].name != ";")
   && (cmd_table[cmd].name != "immtalk")
   && (cmd_table[cmd].name != "builder")
   && (cmd_table[cmd].name != "bd")
   && (cmd_table[cmd].name != "ht")
   && (cmd_table[cmd].name != "where")
   && (cmd_table[cmd].name != "pray")
   && (cmd_table[cmd].name != "help")
   && (cmd_table[cmd].name != "save")
   && (cmd_table[cmd].name != "return")){
      send_to_char("You can't do that in spiritual form.\n\r", ch);
      return;
   }
   char_from_room(ch);
   char_to_room(ch, ch->pcdata->spirit_room);
   ch->position = POS_STANDING;
   (*cmd_table[cmd].do_fun) ( ch, argument );
   char_from_room(ch);
   char_to_room(ch, home);
   ch->position = POS_SLEEPING;
   return;
}

