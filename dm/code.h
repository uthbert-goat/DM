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

static const char code_h_rcsid[] = "$Id: code.h,v 1.4 2003/09/24 06:11:01 thaust Exp $";

typedef struct code_data CODE_DATA;

struct code_data
{
  CODE_DATA*   next;
  char*        subject;
  char*        sender;
  char*        date;
  time_t       date_stamp;
  char*        acceptor;
  bool         approved;
  char*        approved_by;
  sh_int       priority;
  char*        text;
};

#define CODE_FILE "code.not"
#define CODE_SIZE 4096

void codeHelp      ( CHAR_DATA* ch, char* argument );
void codeNew       ( CHAR_DATA* ch, char* argument );
void codeSave      ( CHAR_DATA* ch );
void codeLoad      ( CHAR_DATA* ch );
void codeList      ( CHAR_DATA* ch, char* argument );
void codeSearch    ( CHAR_DATA* ch, char* argument );
void codeRead      ( CHAR_DATA* ch, unsigned int noteNumber );
void codeNoteRead  ( CHAR_DATA* ch, char* argument );
void codeSummary   ( CHAR_DATA* ch, unsigned int noteNumber, char* argument );
void codeAdd       ( CHAR_DATA* ch, unsigned int noteNumber, char* argument );
void codeDel       ( CHAR_DATA* ch, unsigned int noteNumber, char* argument );
void codeApprove   ( CHAR_DATA* ch, unsigned int noteNumber );
void codeAccept    ( CHAR_DATA* ch, unsigned int noteNumber );
void codeReject    ( CHAR_DATA* ch, unsigned int noteNumber );
void codePriority  ( CHAR_DATA* ch, unsigned int noteNumber, char* argument );
void codeRemove    ( CHAR_DATA* ch, unsigned int noteNumber );
void codeDisapprove( CHAR_DATA* ch, unsigned int noteNumber );

void codeNotFound  ( CHAR_DATA* ch );
void codeFreeNode  ( CODE_DATA *cd );
bool codeNoteValid ( CHAR_DATA* ch, unsigned int noteNumber );
CODE_DATA *codeCheckEdit( CHAR_DATA* ch, unsigned int noteNumber );
CODE_DATA *codeSelectNode( unsigned int codeSelection );
CODE_DATA *codeNewNode( );
void codeBriefNode ( CHAR_DATA* ch, CODE_DATA *cd, unsigned int listNumber );
void codeShowNode  ( CHAR_DATA* ch, CODE_DATA *cd, unsigned int listNumber );
void codeFlush     ( );
void codeFileSave  ( );
void codeFileLoad  ( );

