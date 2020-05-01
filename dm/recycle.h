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

static const char recycle_h_rcsid[] = "$Id: recycle.h,v 1.11 2004/04/16 16:53:54 fizzfaldt Exp $";

/* externs */
extern char str_empty[1];
extern int mobile_count;

/* stuff for providing a crash-proof buffer */

#define MAX_BUF        16384
#define MAX_BUF_LIST     12
#define BASE_BUF     1024

/* valid states */
#define BUFFER_SAFE    0
#define BUFFER_OVERFLOW    1
#define BUFFER_FREED     2

/* note recycling */
#define ND NOTE_DATA
ND    *new_note args( (void) );
void    free_note args( (NOTE_DATA *note) );
#undef ND

/* ban data recycling */
#define BD BAN_DATA
BD    *new_ban args( (void) );
void    free_ban args( (BAN_DATA *ban) );
#undef BD

/* nameban data recycling */
#define NB NAMEBAN_DATA
NB      *new_nameban args( (void) );
void    free_nameban args( (NAMEBAN_DATA *nameban) );
#undef NB

/* descriptor recycling */
#define DD DESCRIPTOR_DATA
DD    *new_descriptor args( (void) );
void    free_descriptor args( (DESCRIPTOR_DATA *d) );
#undef DD

/* char gen data recycling */
#define GD GEN_DATA
GD     *new_gen_data args( (void) );
void    free_gen_data args( (GEN_DATA * gen) );
#undef GD

/* extra descr recycling */
#define ED EXTRA_DESCR_DATA
ED    *new_extra_descr args( (void) );
void    free_extra_descr args( (EXTRA_DESCR_DATA *ed) );
#undef ED

/* affect recycling */
#define AD AFFECT_DATA
AD    *new_affect args( (void) );
void    free_affect args( (AFFECT_DATA *af) );
ROOM_AFFECT_DATA *new_room_affect args( (void) );
void    free_room_affect args( (ROOM_AFFECT_DATA * af) );
#undef AD

/* object recycling */
#define OD OBJ_DATA
OD    *new_obj args( (void) );
void    free_obj args( (OBJ_DATA *obj) );
#undef OD

/* character recyling */
#define CD CHAR_DATA
#define PD PC_DATA
CD    *new_char args( (void) );
void    free_char args( (CHAR_DATA *ch) );
PD    *new_pcdata args( (void) );
void    free_pcdata args( (PC_DATA *pcdata) );
#undef PD
#undef CD

/* mount recycling - Mael */
MOUNT_INFO * new_mount_info  args( (void) );
void         free_mount_info args( (MOUNT_INFO *mount_info) );

/* message recycling - Mael */
MESSAGE*    new_message args( (void) );
void        free_message args( (MESSAGE *message) );

/* mob id and memory procedures */
#define MD MEM_DATA
long     get_pc_id args( (void) );
long    get_mob_id args( (void) );
MD    *new_mem_data args( (void) );
void    free_mem_data args( ( MEM_DATA *memory) );
MD    *find_memory args( (MEM_DATA *memory, long id) );
#undef MD

/* buffer procedures */

BUFFER    *new_buf args( (void) );
BUFFER  *new_buf_size args( (int size) );
void    free_buf args( (BUFFER *buffer) );
bool    add_buf args( (BUFFER *buffer, char *string) );
void    clear_buf args( (BUFFER *buffer) );
char    *buf_string args( (BUFFER *buffer) );

/* snoops */
SNOOP_DATA *new_snoop(DESCRIPTOR_DATA *desc);
void free_snoop(DESCRIPTOR_DATA *desc, SNOOP_DATA *snoop);

ROSTER* new_roster args( (char* data) );
void free_roster args( (ROSTER* proster) );

BOOK_DATA* new_bookdata args( () );
void free_bookdata      args( (BOOK_DATA* bdata) );
BOOK_SAVE_DATA* new_booksavedata    args( () );
void free_booksavedata              args( (BOOK_SAVE_DATA* bdata) );
void free_alarm                     args( (ALARM_DATA* alarm) );
ALARM_DATA* new_alarm               args( (void) );
void        add_node                args( (void* data, LIST_DATA* list) );
void        remove_node_for         args( (void* data, LIST_DATA* list) );
void        free_node               args( (NODE_DATA* node) );
LIST_DATA*  new_list                args( (void) );
LIST_DATA*  new_string_list         args( (void) );
bool        node_exists             args( (void* data, LIST_DATA* list) );
void        free_list               args( (LIST_DATA* list) );
SKILL_MOD*  new_skill_mod           args( (int skill_percent, int sn) );
void        free_skill_mod          args( (SKILL_MOD* mod) );
