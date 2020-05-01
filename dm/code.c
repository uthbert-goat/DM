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

static const char rcsid[] = "$Id: code.c,v 1.8 2004/09/08 02:46:07 xurinos Exp $";

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fnmatch.h>
#include "merc.h"
#include "recycle.h"

#include "code.h"

CODE_DATA * code_list = NULL;
CODE_DATA * code_list_last = NULL;
unsigned int code_count = 0;

void do_code( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   unsigned int noteNumber;

   smash_tilde( argument );
   argument = one_argument( argument, arg );

   if ( arg[0] == '\0' )
   codeHelp( ch, "" );

   else if ( is_number( arg ) )
   {
      noteNumber = atoi( arg );

      argument = one_argument( argument, arg );

      if
      (
         arg[0] == '\0' ||
         !str_prefix( arg, "read" )
      )
      codeRead( ch, noteNumber );

      else if
      (
         !str_prefix(arg, "summary") ||
         !str_prefix( arg, "subject")
      )
      codeSummary( ch, noteNumber, argument );

      else if ( !str_prefix( arg, "+" ) )
      codeAdd( ch, noteNumber, argument );

      else if ( !str_prefix( arg, "-" ) )
      codeDel( ch, noteNumber, argument );

      else if ( !str_prefix( arg, "approve" ) )
      codeApprove( ch, noteNumber );

      else if ( !str_prefix( arg, "accept" ) )
      codeAccept( ch, noteNumber );

      else if ( !str_prefix( arg, "reject" ) )
      codeReject( ch, noteNumber );

      else if ( !str_prefix( arg, "priority" ) )
      codePriority( ch, noteNumber, argument );

      else if ( !str_prefix( arg, "remove" ) )
      codeRemove( ch, noteNumber );

      else if ( !str_prefix( arg, "disapprove" ) )
      codeDisapprove( ch, noteNumber );

      else
      codeNotFound( ch );
   }

   else if
   (
      !str_prefix(arg, "summary") ||
      !str_prefix(arg, "subject") ||
      !str_prefix(arg, "new")
   )
   codeNew( ch, argument );

   else if ( !str_prefix( arg, "save" ) )
   codeSave( ch );

   else if ( !str_prefix( arg, "load" ) )
   codeLoad( ch );

   else if ( !str_prefix( arg, "read" ) )
   codeNoteRead( ch, argument );

   else if ( !str_prefix( arg, "list" ) )
   codeList( ch, argument );

   else if ( !str_prefix( arg, "search" ) )
   codeSearch( ch, argument );

   else if ( !str_prefix( arg, "help" ) )
   codeHelp( ch, argument );

   else
   codeNotFound( ch );
}


void codeNotFound  ( CHAR_DATA *ch )
{
   send_to_char( "Invalid code command.\n\r", ch );
   codeHelp( ch, "" );
}


void codeBasicHelp ( CHAR_DATA *ch )
{
   send_to_char( "CODE HELP\n\r\n\r", ch );
   send_to_char( "The code command is a special database for managing coding projects, both\n\rmajor and minor.\n\r\n\r", ch );
   send_to_char( "The following commands may be used with the code command:\n\r", ch );
   send_to_char( "  code help\n\r", ch );
   send_to_char( "  code list [<first number> [<last number>]]\n\r", ch );
   send_to_char( "  code (new | summary | subject) <summary>\n\r", ch );
   send_to_char( "  code save\n\r", ch );
   send_to_char( "  code load\n\r", ch );
   send_to_char( "  code search <search type> <pattern>\n\r", ch );
   send_to_char( "  code <number> + <new line>\n\r", ch );
   send_to_char( "  code <number> - [<modified line>]\n\r", ch );
   send_to_char( "  code <number> accept\n\r", ch );
   send_to_char( "  code <number> approve\n\r", ch );
   send_to_char( "  code <number> disapprove\n\r", ch );
   send_to_char( "  code <number> priority\n\r", ch );
   send_to_char( "  code <number> [read]\n\r", ch );
   send_to_char( "  code <number> reject\n\r", ch );
   send_to_char( "  code <number> remove\n\r", ch );
   send_to_char( "  code <number> (summary | subject)\n\r", ch );
}


void codeHelp      ( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];

   one_argument( argument, arg );

   if ( arg[0] == '\0' )
   {
      codeBasicHelp( ch );
   }

   else if ( !str_cmp( arg, "help" ) )
   {
      send_to_char( "CODE HELP HELP\n\r\n\r", ch );
      send_to_char( "Usage: code help <code command>\n\r", ch );
      send_to_char( "Gives help for a specified code command.  No command specified yields the\n\rgeneral help.\n\r\n\r", ch );
      send_to_char( "You may get help on the following code commands:\n\r", ch );
      send_to_char( "help list new summary subject save load search + - accept\n\r", ch );
      send_to_char( "approve disapprove priority read reject remove\n\r", ch );
   }

   else if ( !str_cmp( arg, "list" ) )
   {
      send_to_char( "CODE HELP LIST\n\r\n\r", ch );
      send_to_char( "Usage: code list [<first number> [<last number>]]\n\r", ch );
      send_to_char( "Displays a list of code reminders between the first and last numbers\n\r", ch );
      send_to_char( "specified.\n\r", ch );
   }

   else if ( !str_cmp( arg, "new" ) )
   {
      send_to_char( "CODE HELP NEW\n\r\n\r", ch );
      send_to_char( "Usage: code new <subject>\n\r", ch );
      send_to_char( "Creates a new code reminder.\n\r", ch );
   }

   else if
   (
      !str_cmp(arg, "summary") ||
      !str_cmp(arg, "subject")
   )
   {
      send_to_char( "CODE HELP SUMMARY/SUBJECT\n\r\n\r", ch );
      send_to_char( "Usage: code <number> (summary | subject)\n\r", ch );
      send_to_char( "Changes the subject of a specific code reminder.  If a number is not\n\r", ch );
      send_to_char( "specified, a new code reminder with the given subject is created.\n\r", ch );
   }

   else if ( !str_cmp( arg, "save" ) )
   {
      send_to_char( "CODE HELP SAVE\n\r\n\r", ch );
      send_to_char( "Usage: code save\n\r", ch );
      send_to_char( "Saves the current code setup to the code reminders file.\n\r", ch );
      send_to_char( "Eventually, the code list will be autosaved.  CODE THIS.\n\r", ch );
   }

   else if ( !str_cmp( arg, "load" ) )
   {
      send_to_char( "CODE HELP LOAD\n\r\n\r", ch );
      send_to_char( "Usage: code load\n\r", ch );
      send_to_char( "Loads the code reminders file dynamically into the MUD.\n\r", ch );
      send_to_char( "Only Implementors may use this command.\n\r", ch );
   }

   else if ( !str_cmp( arg, "search" ) )
   {
      send_to_char( "CODE HELP SEARCH\n\r\n\r", ch );
      send_to_char( "Usage: code search <search type> <pattern>\n\r", ch );
      send_to_char( "Searches the code reminders for specific criteria.\n\r", ch );
      send_to_char( "Search types: subject, sender, acceptor, text\n\r", ch );
      send_to_char( "One may specify many search types and patterns.\n\r", ch );
   }

   else if ( !str_cmp( arg, "+" ) )
   {
      send_to_char( "CODE HELP +\n\r\n\r", ch );
      send_to_char( "Usage: code <number> +\n\r", ch );
      send_to_char( "Adds a line to a code reminder's description.\n\r", ch );
   }

   else if ( !str_cmp( arg, "-" ) )
   {
      send_to_char( "CODE HELP -\n\r\n\r", ch );
      send_to_char( "Usage: code <number> -\n\r", ch );
      send_to_char( "Removes a line from a code reminder's description.\n\r", ch );
   }

   else if ( !str_cmp( arg, "accept" ) )
   {
      send_to_char( "CODE HELP ACCEPT\n\r\n\r", ch );
      send_to_char( "Usage: code <number> accept\n\r", ch );
      send_to_char( "Accepts the responsibility of working on the code reminder.\n\r", ch );
   }

   else if ( !str_cmp( arg, "approve" ) )
   {
      send_to_char( "CODE HELP APPROVE\n\r\n\r", ch );
      send_to_char( "Usage: code <number> approve\n\r", ch );
      send_to_char( "Approves a code reminder for coding.  Only an IMP may make an approval.\n\r", ch );
   }

   else if ( !str_cmp( arg, "disapprove" ) )
   {
      send_to_char( "CODE HELP DISAPPROVE\n\r\n\r", ch );
      send_to_char( "Usage: code <number> disapprove\n\r", ch );
      send_to_char( "Disapproves a code reminder for coding.  Only an IMP may make an approval.\n\r", ch );
   }

   else if ( !str_cmp( arg, "priority" ) )
   {
      send_to_char( "CODE HELP PRIORITY\n\r\n\r", ch );
      send_to_char( "Usage: code <number> priority\n\r", ch );
      send_to_char( "Sets a code reminder's priority between 1 and 10.\n\r", ch );
   }

   else if ( !str_cmp( arg, "read" ) )
   {
      send_to_char( "CODE HELP READ\n\r\n\r", ch );
      send_to_char( "Usage: code <number> [read]\n\r", ch );
      send_to_char( "       code [read] <number>\n\r", ch );
      send_to_char( "Displays the code reminder specified by <number>.\n\r", ch );
   }

   else if ( !str_cmp( arg, "reject" ) )
   {
      send_to_char( "CODE HELP REJECT\n\r\n\r", ch );
      send_to_char( "Usage: code <number> reject\n\r", ch );
      send_to_char( "Rejects the responsibility of working on the code reminder.\n\r", ch );
   }

   else if ( !str_cmp( arg, "remove" ) )
   {
      send_to_char( "CODE HELP REMOVE\n\r\n\r", ch );
      send_to_char( "Usage: code <number> remove\n\r", ch );
      send_to_char( "Deletes a code reminder.\n\r", ch );
   }

   else
   {
      send_to_char( "Invalid code command.\n\r", ch );
      codeBasicHelp( ch );
   }
}


void codeNew       ( CHAR_DATA *ch, char *argument )
{
   CODE_DATA * cd;
   char arg[MAX_INPUT_LENGTH];

   one_argument( argument, arg );

   if ( arg[0] == '\0' )
   {
      send_to_char( "What is the subject of your new code reminder?\n\r", ch );
      return;
   }

   cd = codeNewNode( );
   code_count++;

   if ( code_list_last )
   {
      code_list_last->next = cd;
      code_list_last = code_list_last->next;
   }
   else
   {
      code_list = cd;
      code_list_last = code_list;
   }

   cd->sender = str_dup( ch->name );
   cd->subject = str_dup( argument );

   codeShowNode( ch, cd, code_count );
   codeFileSave( );
}


void codeSave      ( CHAR_DATA *ch )
{
   char buf[MAX_STRING_LENGTH];

   codeFileSave( );

   sprintf( buf, "%d code reminders saved.\n\r", code_count );
   send_to_char( buf, ch );
}


void codeLoad      ( CHAR_DATA *ch )
{
   char buf[MAX_STRING_LENGTH];

   if ( get_trust( ch ) < 60 )
   {
      send_to_char("Only IMPs may load the code list.\n\r", ch );
      return;
   }

   codeFileLoad( );

   sprintf( buf, "%d code reminders loaded.\n\r", code_count );
   send_to_char( buf, ch );
}


void codeList      ( CHAR_DATA *ch, char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   unsigned int firstCode,
   lastCode,
   i;
   CODE_DATA *cd = code_list;

   argument = one_argument( argument, arg1 );
   firstCode = atoi( arg1 );

   one_argument( argument, arg2 );
   lastCode = atoi( arg2 );

   if
   (
      firstCode > code_count ||
      lastCode  > code_count
   )
   {
      send_to_char( "There are not that many code reminders.\n\r", ch );
      return;
   }

   if ( lastCode == 0 ) lastCode = code_count;

   for ( i = 1; cd && (i <= lastCode); i++, cd = cd->next )
   if ( i >= firstCode ) codeBriefNode( ch, cd, i );

   if ( code_count == 0 )
   send_to_char( "There are no code reminders.\n\r", ch );
}


void codeSearch    ( CHAR_DATA *ch, char *argument )
{
   char searchType[MAX_INPUT_LENGTH];
   char patternArg[MAX_INPUT_LENGTH];
   char pattern[MAX_STRING_LENGTH];
   char* temp1 = NULL;  /* Prevent Memory leaks */
   char* temp2 = NULL;  /* Prevent Memory leaks */
   /* bool found[code_count]; */
   bool* found;  /* Dynamic memory -Fizzfaldt */
   bool foundOne = FALSE;
   unsigned int i;
   CODE_DATA * cd;

   if (code_count == 0)  /* code_count is unsigned, do not need <= 0 */
   {
      send_to_char( "There are no code reminders to search.\n\r", ch );
      return;
   }
   found = (bool*) malloc(code_count * sizeof(bool));

   for ( i = 0; i < code_count; i++ )
   found[i] = TRUE;

   while ( argument[0] != '\0' )
   {
      argument = one_argument( argument, searchType );
      argument = one_argument( argument, patternArg );

      if
      (
         (searchType[0] == '\0') ||
         (pattern[0] == '\0')
      )
      break;

      sprintf( pattern, "*%s*", patternArg );

      foundOne = TRUE;

      if ( !str_cmp( searchType, "subject" ) )
      {
         for ( cd = code_list, i = 0; cd ; cd = cd->next, i++ )
         if ( fnmatch((temp1 = upstr(pattern)), (temp2 = upstr(cd->subject)), 0) )
         found[i] = FALSE;
         free_string(temp1);
         free_string(temp2);
      }
      else if ( !str_cmp( searchType, "sender" ) )
      {
         for ( cd = code_list, i = 0; cd ; cd = cd->next, i++ )
         if ( fnmatch((temp1 = upstr(pattern)), (temp2 = upstr(cd->sender)), 0) )
         found[i] = FALSE;
         free_string(temp1);
         free_string(temp2);
      }
      else if ( !str_cmp( searchType, "acceptor" ) )
      {
         for ( cd = code_list, i = 0; cd ; cd = cd->next, i++ )
         if ( fnmatch((temp1 = upstr(pattern)), (temp2 = upstr(cd->acceptor)), 0) )
         found[i] = FALSE;
         free_string(temp1);
         free_string(temp2);
      }
      else if ( !str_cmp( searchType, "text" ) )
      {
         for ( cd = code_list, i = 0; cd ; cd = cd->next, i++ )
         if ( fnmatch((temp1 = upstr(pattern)), (temp2 = upstr(cd->text)), 0) )
         found[i] = FALSE;
         free_string(temp1);
         free_string(temp2);
      }
      else
      {
         char buf[MAX_STRING_LENGTH];

         sprintf( buf, "Invalid search type: %s\n\r", searchType );
         send_to_char( buf, ch );
         free(found);
         return;
      }
   }

   if ( !foundOne )
   {
      send_to_char( "Search for what?\n\r", ch );
      free(found);
      return;
   }
   else
   foundOne = FALSE;

   for ( i = 0; i < code_count; i++ )
   if ( found[i] ) foundOne = TRUE;

   if ( foundOne )
   {
      for ( cd = code_list, i = 0; i < code_count; cd = cd->next, i++ )
      if ( found[i] ) codeBriefNode( ch, cd, i + 1 );
   }
   else
   send_to_char( "No code reminders match your search pattern.\n\r", ch );

   free(found);
   return;
}


void codeRead      ( CHAR_DATA *ch, unsigned int noteNumber )
{
   if ( !codeNoteValid(ch, noteNumber) ) return;

   codeShowNode( ch, codeSelectNode(noteNumber), noteNumber );
}


void codeNoteRead  ( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   int codeSelection;

   one_argument( argument, arg );
   codeSelection = atoi( arg );
   codeRead( ch, codeSelection );
}


void codeSummary   ( CHAR_DATA *ch, unsigned int noteNumber, char *argument )
{
   CODE_DATA * cd = codeCheckEdit( ch, noteNumber );

   if ( !cd ) return;

   free_string( cd->subject );
   cd->subject = str_dup( argument );

   codeShowNode( ch, cd, noteNumber );
   codeFileSave( );
}


void codeAdd       ( CHAR_DATA *ch, unsigned int noteNumber, char *argument )
{
   CODE_DATA * cd = codeCheckEdit( ch, noteNumber );
   BUFFER * buffer;

   if ( !cd ) return;

   if ( strlen(cd->text) + strlen(argument) + 2 >= CODE_SIZE )
   {
      send_to_char( "Code reminder too long.\n\r", ch );
      return;
   }

   buffer = new_buf( );

   add_buf( buffer, cd->text );
   add_buf( buffer, argument );
   add_buf( buffer, "\n\r" );
   free_string( cd->text );
   cd->text = str_dup( buf_string(buffer) );
   free_buf(buffer);

   codeShowNode( ch, cd, noteNumber );
   codeFileSave( );
}


void codeDel       ( CHAR_DATA *ch, unsigned int noteNumber, char *argument )
{
   CODE_DATA * cd = codeCheckEdit( ch, noteNumber );
   char buf[MAX_STRING_LENGTH];
   int i;

   if ( !cd ) return;

   sprintf( buf, "%s", cd->text );
   if ( buf[0] == '\0' )
   {
      send_to_char( "Code reminder is already clear.\n\r", ch );
      return;
   }

   for ( i = strlen(buf) - 2; (i > 0) && (buf[i] != '\r'); i-- );
   buf[i+1] = '\0';

   free_string( cd->text );
   cd->text = str_dup( buf );

   codeShowNode( ch, cd, noteNumber );
   codeFileSave( );
}


void codeApprove   ( CHAR_DATA *ch, unsigned int noteNumber )
{
   CODE_DATA * cd;
   char buf[MAX_STRING_LENGTH];

   if ( !codeNoteValid(ch, noteNumber) ) return;

   if ( get_trust( ch ) < 60 )
   {
      send_to_char( "Only IMPs may ultimately approve or disapprove any coding project.\n\r", ch );
      return;
   }

   cd = codeSelectNode( noteNumber );

   if ( !cd->approved )
   {
      cd->approved = TRUE;
      free_string( cd->approved_by );
      cd->approved_by = str_dup( ch->name );

      send_to_char( "Code reminder approved.\n\r", ch );
      codeFileSave( );
   }
   else
   {
      sprintf( buf, "Code reminder %d has already been approved.\n\r", noteNumber );
      send_to_char( buf, ch );
      return;
   }
}


void codeDisapprove( CHAR_DATA *ch, unsigned int noteNumber )
{
   CODE_DATA * cd;
   char buf[MAX_STRING_LENGTH];

   if ( !codeNoteValid(ch, noteNumber) ) return;

   if ( get_trust( ch ) < 60 )
   {
      send_to_char("Only IMPs may ultimately approve or disapprove any coding project.\n\r", ch );
      return;
   }

   cd = codeSelectNode( noteNumber );

   if ( cd->approved )
   {
      cd->approved = FALSE;
      free_string( cd->approved_by );
      cd->approved_by = &str_empty[0];

      send_to_char( "Code reminder disapproved.\n\r", ch );
      codeFileSave( );
   }
   else
   {
      sprintf( buf, "Code reminder %d has already been disapproved.\n\r", noteNumber );
      send_to_char( buf, ch );
      return;
   }
}

void codeAccept    ( CHAR_DATA *ch, unsigned int noteNumber )
{
   CODE_DATA * cd;

   if ( !codeNoteValid(ch, noteNumber) ) return;

   cd = codeSelectNode( noteNumber );

   if ( (cd->acceptor[0] != '\0') && str_cmp(cd->acceptor, ch->name) )
   {
      send_to_char( "One cannot accept another coder's project.  Ask them to reject it first.\n\r", ch );
      return;
   }

   if ( !str_cmp( cd->acceptor, ch->name ) )
   {
      send_to_char( "You have already accepted this code reminder.\n\r", ch );
      return;
   }

   free_string( cd->acceptor );  /* Just in case of major weirness. */
   cd->acceptor = str_dup( ch->name );

   send_to_char( "Code reminder accepted!\n\r", ch );
   codeFileSave( );
}


void codeReject    ( CHAR_DATA *ch, unsigned int noteNumber )
{
   CODE_DATA * cd;

   if ( !codeNoteValid(ch, noteNumber) ) return;

   cd = codeSelectNode( noteNumber );

   if ( (cd->acceptor[0] != '\0') && str_cmp(cd->sender, ch->name) )
   {
      send_to_char( "Only the Acceptor of this code reminder may reject it.\n\r", ch );
      return;
   }

   if ( cd->acceptor[0] == '\0' )
   {
      send_to_char( "Nobody has accepted this code reminder.\n\r", ch );
      return;
   }

   free_string( cd->acceptor );
   cd->acceptor = &str_empty[0];

   send_to_char( "Code reminder rejected!\n\r", ch );
   codeFileSave( );
}


void codePriority  ( CHAR_DATA *ch, unsigned int noteNumber, char *argument )
{
   CODE_DATA * cd = codeCheckEdit( ch, noteNumber );
   int priority = atoi( argument );

   if ( !cd ) return;

   if
   (
      priority < 1 ||
      priority > 10
   )
   {
      send_to_char( "Only priorities of 1 through 10 are accepted.\n\r", ch );
      return;
   }

   cd->priority = priority;

   send_to_char( "The code reminder's priority is set.\n\r", ch );
   codeFileSave( );
}


void codeRemove    ( CHAR_DATA *ch, unsigned int noteNumber )
{
   CODE_DATA * cd = codeCheckEdit( ch, noteNumber );
   CODE_DATA * previous = NULL;

   if ( !cd ) return;

   if ( noteNumber > 1 )
   previous = codeSelectNode( noteNumber - 1 );

   if ( cd == code_list_last ) code_list_last = previous;

   if ( previous )
   {
      previous->next = cd->next;
      cd->next = NULL;
   }

   if ( cd == code_list )
   {
      code_list = cd->next;
      cd->next = NULL;
   }

   codeFreeNode( cd );

   code_count--;

   send_to_char( "Code reminder removed.\n\r", ch );
   codeFileSave( );
}


void codeFreeNode( CODE_DATA *cd )
{
   if ( cd )
   {
      free_string( cd->subject );
      free_string( cd->sender );
      free_string( cd->date );
      free_string( cd->acceptor );
      free_string( cd->approved_by );
      free_string( cd->text );
      free_mem( cd, sizeof(*cd) );
   }
}


bool codeNoteValid( CHAR_DATA *ch, unsigned int noteNumber )
{
   if
   (
      noteNumber <= 0 ||
      noteNumber > code_count
   )
   {
      send_to_char( "That code reminder does not exist.\n\r", ch );
      return FALSE;
   }

   return TRUE;
}


CODE_DATA *codeCheckEdit( CHAR_DATA *ch, unsigned int noteNumber )
{
   CODE_DATA * cd;

   if ( !codeNoteValid(ch, noteNumber) ) return NULL;

   cd = codeSelectNode( noteNumber );

   if (   !( get_trust(ch) == 60 )
   && !( !str_cmp( ch->name, cd->sender ) || (cd->sender[0] == '\0') )
   && !( !str_cmp( ch->name, cd->acceptor ) || (cd->acceptor[0] == '\0') )
)
{
   send_to_char( "Only an IMP, the original poster, or the accepting coder may do this.\n\r", ch );
   return NULL;
}

return cd;
}


CODE_DATA * codeSelectNode( unsigned int codeSelection )
{
   CODE_DATA * cd = NULL;

   if
   (
      codeSelection <= code_count &&
      codeSelection > 0
   )
   {
      unsigned int i;
      cd = code_list;

      for ( i = 1; (i < codeSelection) && cd; i++ )
      cd = cd->next;
   }

   return cd;
}


CODE_DATA * codeNewNode( )
{
   CODE_DATA * cd;
   char * strtime;

   cd = alloc_mem ( sizeof(*cd) );
   cd->next = NULL;

   cd->subject = &str_empty[0];
   cd->sender = &str_empty[0];
   cd->acceptor = &str_empty[0];
   cd->approved = FALSE;
   cd->approved_by = &str_empty[0];
   cd->priority = 10;
   cd->text = &str_empty[0];

   strtime = ctime( &current_time );
   strtime[strlen( strtime ) - 1] = '\0';
   cd->date = str_dup( strtime );
   cd->date_stamp = current_time;

   return cd;
}


void codeBriefNode ( CHAR_DATA *ch, CODE_DATA *cd, unsigned int listNumber )
{
   char buf[MAX_STRING_LENGTH];

   sprintf
   (
      buf,
      "[ %2d%s] %s (P%d%s%s%s%s)\n\r",
      listNumber,
      (
         (
            !cd->approved ||
            cd->acceptor[0] == '\0'
         ) ?
         "*" :
         " "
      ),
      cd->subject,
      cd->priority,
      cd->approved ? " - APPROVED by " : "",
      cd->approved_by[0] != '\0' ? cd->approved_by : "",
      cd->acceptor[0] != '\0' ? " - ACCEPTED by " : "",
      cd->acceptor[0] != '\0' ? cd->acceptor : ""
   );
   send_to_char( buf, ch );
}


void codeShowNode  ( CHAR_DATA *ch, CODE_DATA *cd, unsigned int listNumber )
{
   char buf[MAX_STRING_LENGTH];

   codeBriefNode( ch, cd, listNumber );

   sprintf( buf, "From: %s\n\r", cd->sender );
   send_to_char( buf, ch );

   sprintf( buf, "Date: %s\n\r", cd->date );
   send_to_char( buf, ch );

   page_to_char( cd->text, ch );
}


void codeFlush     ( )
{
   CODE_DATA * cd = code_list;
   CODE_DATA * oldcd;

   while ( cd )
   {
      oldcd = cd;
      cd = cd->next;
      codeFreeNode( oldcd );
   }

   code_list = NULL;
   code_list_last = code_list;
}


void codeFileSave  ( )
{
   CODE_DATA * cd = code_list;
   FILE * f;

   fclose( fpReserve );
   if ( (f = fopen( CODE_FILE, "w" )) == NULL )
   {
      bug( "codeFileSave: Unable to open CODE_FILE for writing!", 0 );
      return;
   }

   if ( cd )
   {
      code_count = 0;

      while ( cd )
      {
         fprintf( f, "Subject  %s~\n", cd->subject );
         fprintf( f, "Sender  %s~\n", cd->sender );
         fprintf( f, "Date  %s~\n", cd->date );
#if defined(MSDOS)
         fprintf( f, "Stamp  %d\n", cd->date_stamp );
#else
         fprintf( f, "Stamp  %ld\n", cd->date_stamp );
#endif
         fprintf( f, "Acceptor  %s~\n", cd->acceptor );
         fprintf( f, "Approved  %d %s~\n", cd->approved, cd->approved ? cd->approved_by : "");
         fprintf( f, "Priority  %d\n", cd->priority );
         fprintf( f, "Text\n%s~\n", cd->text );

         code_count++;

         cd = cd->next;
      }
   }
   fclose( f );
   fpReserve = fopen( NULL_FILE, "r" );
}


void codeFileLoad  ( )
{
   CODE_DATA * cdlast = NULL;
   CODE_DATA * cd;
   char letter;
   FILE * f;

   codeFlush( );
   code_count = 0;

   if ( (f = fopen( CODE_FILE, "r" )) == NULL ) return;

   for (;;)
   {
      do
      {
         letter = getc( f );
         if ( feof(f) )
         {
            fclose( f );
            return;
         }
      } while ( isspace(letter) );

      ungetc( letter, f );

      cd = alloc_mem ( sizeof(*cd) );
      cd->next = NULL;

      if ( str_cmp( fread_word(f), "subject" ) ) break;
      cd->subject = fread_string( f );

      if ( str_cmp( fread_word(f), "sender" ) ) break;
      cd->sender = fread_string( f );

      if ( str_cmp( fread_word(f), "date" ) ) break;
      cd->date = fread_string( f );

      if ( str_cmp( fread_word(f), "stamp" ) ) break;
      cd->date_stamp = fread_number( f );

      if ( str_cmp( fread_word(f), "acceptor" ) ) break;
      cd->acceptor = fread_string( f );

      if ( str_cmp( fread_word(f), "approved" ) ) break;
      cd->approved = fread_number( f );
      cd->approved_by = fread_string( f );

      if ( str_cmp( fread_word(f), "priority" ) ) break;
      cd->priority = fread_number( f );

      if ( str_cmp( fread_word(f), "text" ) ) break;
      cd->text = fread_string( f );

      if ( cdlast )
      {
         cdlast->next = cd;
         cdlast = cdlast->next;
      }
      else
      {
         code_list = cd;
         cdlast = code_list;
      }

      code_list_last = cd;

      code_count++;
   }

   bug( "codeFileLoad: bad key word, aborting code lode", 0 );

   if ( cd ) cdlast->next = cd;
   codeFlush( );

   return;
}
