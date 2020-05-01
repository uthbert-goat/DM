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


static const char rcsid[] = "$Id: comm.c,v 1.186 2004/11/04 04:34:13 maelstrom Exp $";

/*
   This file contains all of the OS-dependent stuff:
   startup, signals, BSD sockets for tcp/ip, i/o, timing.

   The data flow for input is:
      Game_loop ---> Read_from_descriptor ---> Read
      Game_loop ---> Read_from_buffer

   The data flow for output is:
      Game_loop ---> Process_Output ---> Write_to_descriptor -> Write

   The OS-dependent functions are Read_from_descriptor and Write_to_descriptor.
   -- Furey  26 Jan 1993
*/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "merc.h"
#include "recycle.h"
#include "subclass.h"
#include "tables.h"
#include "clan.h"
#include "magic.h"
#include "worship.h"
#include "music.h"
#include "database.h"

/* command procedures needed */
ROOM_INDEX_DATA* hoarder_check(CHAR_DATA*ch, ROOM_INDEX_DATA* room);
void save_bans(void);
DECLARE_DO_FUN(do_induct        );
DECLARE_DO_FUN(do_help        );
DECLARE_DO_FUN(do_look        );
DECLARE_DO_FUN(do_skills    );
DECLARE_DO_FUN(do_outfit    );
DECLARE_DO_FUN(do_unread    );
DECLARE_DO_FUN(do_remove    );
DECLARE_DO_FUN(do_return    );
DECLARE_DO_FUN(do_nameallow     );
DECLARE_DO_FUN(do_save);
DECLARE_DO_FUN(do_elder);
DECLARE_DO_FUN(do_leader);

int     powtwo          args( ( int,  int ) );
void announce_login args( ( CHAR_DATA *ch));
void announce_logout args( ( CHAR_DATA *ch));
void initialize_stats( CHAR_DATA *ch, DESCRIPTOR_DATA *d );
void skill_adjust(CHAR_DATA *ch, bool version_only);
void apply_irv_ch(CHAR_DATA *ch);
void timeval_to_mudtime args( (TIMEVAL* input, MUD_TIME* output) );
void impure_note args ( (CHAR_DATA* ch, char* message) );
void fix_logon_time     args ( (long sec_delta) );
void racial_skills   args( (CHAR_DATA* ch) );
/* For act_color and act_new_color */
char * convert_color args( ( char * strbuf, bool ISANSI ) );
/*
* Malloc debugging stuff.
*/
#if defined(sun)
#undef MALLOC_DEBUG
#endif

#if defined(MALLOC_DEBUG)
#include <malloc.h>
extern    int    malloc_debug    args( ( int  ) );
extern    int    malloc_verify    args( ( void ) );
#endif



/*
* Signal handling.
* Apollo has a problem with __attribute(atomic) in signal.h,
*   I dance around it.
*/
#if defined(apollo)
#define __attribute(x)
#endif

#if defined(unix)
#include <signal.h>
#endif

#if defined(apollo)
#undef __attribute
#endif



/*
* Socket and TCP/IP stuff.
*/
#if    defined(macintosh) || defined(MSDOS)
const    char    echo_off_str    [] = { '\0' };
const    char    echo_on_str    [] = { '\0' };
const    char     go_ahead_str    [] = { '\0' };
#endif

#if    defined(unix)
#include <fcntl.h>
#define __STRICT_ANSI__ 1
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "telnet.h"
const    char    echo_off_str    [] = { (char) IAC, (char) WILL, TELOPT_ECHO, '\0' };
const    char    echo_on_str    [] = { (char) IAC, (char) WONT, TELOPT_ECHO, '\0' };
const    char     go_ahead_str    [] = { (char) IAC, (char) GA, '\0' };
#endif


/*
* OS-dependent declarations.
*/
#if    defined(_AIX)
#include <sys/select.h>
int    accept        args( ( int s, struct sockaddr *addr, int *addrlen ) );
int    bind        args( ( int s, struct sockaddr *name, int namelen ) );
void    bzero        args( ( char *b, int length ) );
int    getpeername    args( ( int s, struct sockaddr *name, int *namelen ) );
int    getsockname    args( ( int s, struct sockaddr *name, int *namelen ) );
int     gofday    args( ( TIMEVAL *tp, struct timezone *tzp ) );
int    listen        args( ( int s, int backlog ) );
int    setsockopt    args( ( int s, int level, int optname, void *optval, int optlen ) );
int    socket        args( ( int domain, int type, int protocol ) );
#endif

#if    defined(apollo)
#include <unistd.h>
void    bzero        args( ( char *b, int length ) );
#endif

#if    defined(__hpux)
int    accept        args( ( int s, void *addr, int *addrlen ) );
int    bind        args( ( int s, const void *addr, int addrlen ) );
void    bzero        args( ( char *b, int length ) );
int    getpeername    args( ( int s, void *addr, int *addrlen ) );
int    getsockname    args( ( int s, void *name, int *addrlen ) );
int     gofday    args( ( TIMEVAL *tp, struct timezone *tzp ) );
int    listen        args( ( int s, int backlog ) );
int    setsockopt    args( ( int s, int level, int optname,
const void *optval, int optlen ) );
int    socket        args( ( int domain, int type, int protocol ) );
#endif

#if    defined(interactive)
#include <net/errno.h>
#include <sys/fnctl.h>
#endif

#if    defined(linux)
/*
   Linux shouldn't need these. If you have a problem compiling, try
   uncommenting accept and bind.
int    accept        args( ( int s, struct sockaddr *addr, int *addrlen ) );
int    bind        args( ( int s, struct sockaddr *name, int namelen ) );
*/

int    close        args( ( int fd ) );
/*
int    getpeername    args( ( int s, struct sockaddr *name, int *namelen ) );
int    getsockname    args( ( int s, struct sockaddr *name, int *namelen ) );
*/
int     gofday    args( ( TIMEVAL *tp, struct timezone *tzp ) );
/*
int    listen        args( ( int s, int backlog ) );
*/
int    read        args( ( int fd, char *buf, int nbyte ) );
int    select        args( ( int width, fd_set *readfds, fd_set *writefds,
fd_set *exceptfds, TIMEVAL *timeout ) );
int    socket        args( ( int domain, int type, int protocol ) );
int    write        args( ( int fd, char *buf, int nbyte ) );
#endif

#if    defined(macintosh)
#include <console.h>
#include <fcntl.h>
#include <unix.h>
struct    timeval
{
   time_t    tv_sec;
   time_t    tv_usec;
};
#if    !defined(isascii)
#define    isascii(c)        ( (c) < 0200 )
#endif
static    long            theKeys    [4];

int     gofday            args( ( TIMEVAL *tp, void *tzp ) );
#endif

#if    defined(MIPS_OS)
extern    int        errno;
#endif

#if defined(MSDOS) || defined(MSWINDOWS)
int     gofday    args( ( TIMEVAL *tp, void *tzp ) );
int    kbhit        args( ( void ) );
int    read        args( ( int fd, char *buf, int nbyte ) );
int    write        args( ( int fd, char *buf, int nbyte ) );
int    close        args( ( int fd ) );
#endif

#if    defined(NeXT)
int    close        args( ( int fd ) );
int    fcntl        args( ( int fd, int cmd, int arg ) );
#if    !defined(htons)
u_short    htons        args( ( u_short hostshort ) );
#endif
#if    !defined(ntohl)
u_long    ntohl        args( ( u_long hostlong ) );
#endif
int    read        args( ( int fd, char *buf, int nbyte ) );
int    select        args( ( int width, fd_set *readfds, fd_set *writefds,
fd_set *exceptfds, TIMEVAL *timeout ) );
int    write        args( ( int fd, char *buf, int nbyte ) );
#endif

#if    defined(sequent)
int    accept        args( ( int s, struct sockaddr *addr, int *addrlen ) );
int    bind        args( ( int s, struct sockaddr *name, int namelen ) );
int    close        args( ( int fd ) );
int    fcntl        args( ( int fd, int cmd, int arg ) );
int    getpeername    args( ( int s, struct sockaddr *name, int *namelen ) );
int    getsockname    args( ( int s, struct sockaddr *name, int *namelen ) );
int     gofday    args( ( TIMEVAL *tp, struct timezone *tzp ) );
#if    !defined(htons)
u_short    htons        args( ( u_short hostshort ) );
#endif
int    listen        args( ( int s, int backlog ) );
#if    !defined(ntohl)
u_long    ntohl        args( ( u_long hostlong ) );
#endif
int    read        args( ( int fd, char *buf, int nbyte ) );
int    select        args( ( int width, fd_set *readfds, fd_set *writefds,
fd_set *exceptfds, TIMEVAL *timeout ) );
int    setsockopt    args( ( int s, int level, int optname, caddr_t optval,
int optlen ) );
int    socket        args( ( int domain, int type, int protocol ) );
int    write        args( ( int fd, char *buf, int nbyte ) );
#endif

/* This includes Solaris Sys V as well */
#if defined(sun)
int    accept        args( ( int s, struct sockaddr *addr, int *addrlen ) );
int    bind        args( ( int s, struct sockaddr *name, int namelen ) );
void    bzero        args( ( char *b, int length ) );
int    close        args( ( int fd ) );
int    getpeername    args( ( int s, struct sockaddr *name, int *namelen ) );
int    getsockname    args( ( int s, struct sockaddr *name, int *namelen ) );
int     gofday    args( ( TIMEVAL *tp, struct timezone *tzp ) );
int    listen        args( ( int s, int backlog ) );
int    read        args( ( int fd, char *buf, int nbyte ) );
int    select        args( ( int width, fd_set *readfds, fd_set *writefds,
fd_set *exceptfds, TIMEVAL *timeout ) );
#if defined(SYSV)
int setsockopt        args( ( int s, int level, int optname,
const char *optval, int optlen ) );
#else
int    setsockopt    args( ( int s, int level, int optname, void *optval,
int optlen ) );
#endif
int    socket        args( ( int domain, int type, int protocol ) );
int    write        args( ( int fd, char *buf, int nbyte ) );
#endif

#if defined(ultrix)
int    accept        args( ( int s, struct sockaddr *addr, int *addrlen ) );
int    bind        args( ( int s, struct sockaddr *name, int namelen ) );
void    bzero        args( ( char *b, int length ) );
int    close        args( ( int fd ) );
int    getpeername    args( ( int s, struct sockaddr *name, int *namelen ) );
int    getsockname    args( ( int s, struct sockaddr *name, int *namelen ) );
int     gofday    args( ( TIMEVAL *tp, struct timezone *tzp ) );
int    listen        args( ( int s, int backlog ) );
int    read        args( ( int fd, char *buf, int nbyte ) );
int    select        args( ( int width, fd_set *readfds, fd_set *writefds,
fd_set *exceptfds, TIMEVAL *timeout ) );
int    setsockopt    args( ( int s, int level, int optname, void *optval,
int optlen ) );
int    socket        args( ( int domain, int type, int protocol ) );
int    write        args( ( int fd, char *buf, int nbyte ) );
#endif



/*
* Global variables.
*/
DESCRIPTOR_DATA *   descriptor_list;    /* All open descriptors        */
DESCRIPTOR_DATA *   d_next;        /* Next descriptor in loop    */
FILE *            fpReserve;        /* Reserved file handle        */
bool            god;        /* All new chars are gods!    */
bool            merc_down;        /* Shutdown            */
bool            boot_done = FALSE;  /* Booting up done */
bool            wizlock;        /* Game is wizlocked        */
bool            newlock;        /* Game is newlocked        */
bool                MOBtrigger = TRUE;  /* act() switch                 */
char            str_boot_time[MAX_INPUT_LENGTH];
time_t            current_time;    /* time of this pulse */
bool                reboot_ok = TRUE;   /* auto reboot toggle */
/*bool                check_ip = TRUE; */   /* allows resolving of IPs to names */


/*
* OS-dependent local functions.
*/
#if defined(macintosh) || defined(MSDOS)
void    game_loop_mac_msdos    args( ( void ) );
bool    read_from_descriptor    args( ( DESCRIPTOR_DATA *d ) );
bool    write_to_descriptor    args( ( int desc, char *txt, int length ) );
#endif

#if defined(unix)
void    game_loop_unix        args( ( int control ) );
int    init_socket        args( ( int port ) );
void    init_descriptor        args( ( int control ) );
bool    read_from_descriptor    args( ( DESCRIPTOR_DATA *d ) );
bool    write_to_descriptor    args( ( int desc, char *txt, int length ) );
#endif




/*
* Other local functions (OS-independent).
*/
int    get_old_xp        args( (CHAR_DATA *ch ));
bool    check_parse_name    args( ( char *name ) );
bool    check_reconnect        args( ( DESCRIPTOR_DATA *d, char *name,
bool fConn, CHAR_DATA** old ) );
bool    check_playing        args( ( DESCRIPTOR_DATA *d, char *name, CHAR_DATA** old, bool test ) );
bool    check_playing_newbie args( ( DESCRIPTOR_DATA *d) );
int    main            args( ( int argc, char **argv ) );
void    nanny            args( ( DESCRIPTOR_DATA *d, char *argument ) );
bool    process_output        args( ( DESCRIPTOR_DATA *d, bool fPrompt ) );
void    read_from_buffer    args( ( DESCRIPTOR_DATA *d ) );
void    stop_idling        args( ( CHAR_DATA *ch ) );
void    bust_a_prompt           args( ( CHAR_DATA *ch ) );
bool     output_buffer        args( ( DESCRIPTOR_DATA *d ) );
void    hoard_report        args( ( CHAR_DATA *ch ) );
extern   void print_version();


int main( int argc, char **argv )
{

   TIMEVAL now_time;
   int port;

#if defined(unix)
   int control;
#endif

   print_version();

   /*
   * Memory debugging if needed.
   */
#if defined(MALLOC_DEBUG)
   malloc_debug( 2 );
#endif

   /*
   * Init time.
   */
   gettimeofday( &now_time, NULL );
   current_time     = (time_t) now_time.tv_sec;
   timeval_to_mudtime(&now_time, &current_mud_time);
   /* time adjust -werv  */      current_time += 0 * 3600;
   strcpy( str_boot_time, ctime( &current_time ) );
   log_string("Darkmists booting up!");

   /*
   * Macintosh console initialization.
   */
#if defined(macintosh)
   console_options.nrows = 31;
   cshow( stdout );
   csetmode( C_RAW, stdin );
   cecho2file( "log file", 1, stderr );
#endif

   /*
   * Reserve one channel for our use.
   */
   if ( ( fpReserve = fopen( NULL_FILE, "r" ) ) == NULL )
   {
      perror( NULL_FILE );
      exit( 1 );
   }

   /*
   * Get the port number.
   */
   port = 4000;
   if ( argc > 1 )
   {
      if ( !is_number( argv[1] ) )
      {
         fprintf( stderr, "Usage: %s [port #]\n", argv[0] );
         exit( 1 );
      }
      else if ( ( port = atoi( argv[1] ) ) <= 1024 )
      {
         fprintf( stderr, "Port number must be above 1024.\n" );
         exit( 1 );
      }
   }

   /* just to know if dm is real or not */
   if (port ==2222)
   is_test = FALSE;
   else
   is_test = TRUE;

   /*
   * Run the game.
   */

   /* Initialize mysql database connection */
   database_connect();

#if defined(macintosh) || defined(MSDOS)
   boot_db( );
   log_string( "Merc is ready to rock." );
   game_loop_mac_msdos( );
#endif

#if defined(unix)
   control = init_socket( port );
   boot_db( );
   sprintf(log_buf,  "ROM is ready to rock on port %d.", port );
   log_string( log_buf );
   boot_done = TRUE;
   game_loop_unix( control );
   close (control);
#endif

   /*
   * That's all, folks.
   */

   /* Close mysql database connection */
   database_close();

   log_string( "Normal termination of game." );

   exit( 0 );
   return 0;
}



#if defined(unix)
int init_socket( int port )
{
   static struct sockaddr_in sa_zero;
   struct sockaddr_in sa;
   int x = 1;
   int fd;

   if ( ( fd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
   {
      perror( "Init_socket: socket" );
      exit( 1 );
   }

   if ( setsockopt( fd, SOL_SOCKET, SO_REUSEADDR,
   (char *) &x, sizeof(x) ) < 0 )
   {
      perror( "Init_socket: SO_REUSEADDR" );
      close(fd);
      exit( 1 );
   }

#if defined(SO_DONTLINGER) && !defined(SYSV)
   {
      struct    linger    ld;

      ld.l_onoff  = 1;
      ld.l_linger = 1000;

      if ( setsockopt( fd, SOL_SOCKET, SO_DONTLINGER,
      (char *) &ld, sizeof(ld) ) < 0 )
      {
         perror( "Init_socket: SO_DONTLINGER" );
         close(fd);
         exit( 1 );
      }
   }
#endif

   sa            = sa_zero;
   sa.sin_family   = AF_INET;
   sa.sin_port        = htons( port );

   if ( bind( fd, (struct sockaddr *) &sa, sizeof(sa) ) < 0 )
   {
      perror("Init socket: bind" );
      close(fd);
      exit(1);
   }


   if ( listen( fd, 3 ) < 0 )
   {
      perror("Init socket: listen");
      close(fd);
      exit(1);
   }

   return fd;
}
#endif



#if defined(macintosh) || defined(MSDOS)
void game_loop_mac_msdos( void )
{
   TIMEVAL last_time;
   TIMEVAL now_time;
   static DESCRIPTOR_DATA dcon;

   gettimeofday( &last_time, NULL );
   current_time = (time_t) last_time.tv_sec;
   /* time adjust -werv    */    current_time += 0*3600;

   /*
   * New_descriptor analogue.
   */
   dcon.descriptor    = 0;
   dcon.connected    = CON_GET_NAME;
   dcon.host        = str_dup( "localhost" );
   dcon.outsize    = 2000;
   dcon.outbuf        = alloc_mem( dcon.outsize );
   dcon.next        = descriptor_list;
   dcon.showstr_head    = NULL;
   dcon.showstr_point    = NULL;
   descriptor_list    = &dcon;

   /*
   * Send the greeting.
   */
   {
      extern char * help_greeting;
      if ( help_greeting[0] == '.' )
      write_to_buffer( &dcon, help_greeting+1, 0 );
      else
      write_to_buffer( &dcon, help_greeting  , 0 );
   }

   /* Main loop */
   while ( !merc_down )
   {
      DESCRIPTOR_DATA *d;

      /*
      * Process input.
      */
      for ( d = descriptor_list; d != NULL; d = d_next )
      {
         d_next    = d->next;
         d->fcommand    = FALSE;

#if defined(MSDOS)
         if ( kbhit( ) )
#endif
         {
            if ( d->character != NULL )
            d->character->timer = 0;
            if ( !read_from_descriptor( d ) )
            {
               if ( d->character != NULL)
               save_char_obj( d->character );
               d->outtop    = 0;
               close_socket( d );
               continue;
            }
         }

         if (d->character != NULL && d->character->daze > 0)
         --d->character->daze;

         if ( d->character != NULL && d->character->wait > 0 )
         {
            --d->character->wait;
            continue;
         }

         if (d->character != NULL && d->character->daze > 1)
         continue;

         read_from_buffer( d );
         if ( d->incomm[0] != '\0' )
         {
            d->fcommand    = TRUE;
            stop_idling( d->character );

            if ( d->connected == CON_PLAYING )
            substitute_alias( d, d->incomm );
            else
            nanny( d, d->incomm );

            d->incomm[0]    = '\0';
         }
      }



      /*
      * Autonomous game motion.
      */
      update_handler( );



      /*
      * Output.
      */
      for ( d = descriptor_list; d != NULL; d = d_next )
      {
         d_next = d->next;

         if ( ( d->fcommand || d->outtop > 0 ) )
         {
            if ( !process_output( d, TRUE ) )
            {
               if ( d->character != NULL && d->character->level > 1)
               save_char_obj( d->character );
               d->outtop    = 0;
               close_socket( d );
            }
         }
      }



      /*
      * Synchronize to a clock.
      * Busy wait (blargh).
      */
      now_time = last_time;
      for ( ; ; )
      {
         int delta;

#if defined(MSDOS)
         if ( kbhit( ) )
#endif
         {
            if ( dcon.character != NULL )
            dcon.character->timer = 0;
            if ( !read_from_descriptor( &dcon ) )
            {
               if ( dcon.character != NULL && d->character->level > 1)
               save_char_obj( d->character );
               dcon.outtop    = 0;
               close_socket( &dcon );
            }
#if defined(MSDOS)
            break;
#endif
         }

         gettimeofday( &now_time, NULL );
         delta = ( now_time.tv_sec  - last_time.tv_sec  ) * 1000 * 1000
         + ( now_time.tv_usec - last_time.tv_usec );
         if ( delta >= 1000000 / PULSE_PER_SECOND )
         break;
      }
      last_time    = now_time;
      current_time = (time_t) last_time.tv_sec;
      /* time adjust -werv   */     current_time += 0*3600;

   }

   return;
}
#endif



#if defined(unix)
void game_loop_unix( int control )
{
   static TIMEVAL null_time;
   TIMEVAL last_time;
   bool repeat;

   signal( SIGPIPE, SIG_IGN );
   gettimeofday( &last_time, NULL );
   timeval_to_mudtime(&last_time, &current_mud_time);
   current_time = (time_t) last_time.tv_sec;
   /* time adjust -werv    */    current_time += 0*3600;

   /* Main loop */
   while ( !merc_down )
   {
      fd_set in_set;
      fd_set out_set;
      fd_set exc_set;
      DESCRIPTOR_DATA *d;
      int maxdesc;

#if defined(MALLOC_DEBUG)
      if ( malloc_verify( ) != 1 )
      abort( );
#endif

      /*
      * Poll all active descriptors.
      */
      FD_ZERO( &in_set  );
      FD_ZERO( &out_set );
      FD_ZERO( &exc_set );
      FD_SET( control, &in_set );
      maxdesc    = control;
      for ( d = descriptor_list; d; d = d->next )
      {
         maxdesc = UMAX( maxdesc, d->descriptor );
         FD_SET( d->descriptor, &in_set  );
         FD_SET( d->descriptor, &out_set );
         FD_SET( d->descriptor, &exc_set );
      }
      do
      {
         repeat = FALSE;
         if (select(maxdesc+1, &in_set, &out_set, &exc_set, &null_time) < 0)
         {
            if (errno != EINTR)
            {
               perror( "Game_loop: select: poll" );
               exit( 1 );
            }
            else
            {
               repeat = TRUE;
            }
         }
      }
      while (repeat);
      /*
      * New connection?
      */
      if ( FD_ISSET( control, &in_set ) )
      init_descriptor( control );
      /*
      * Kick out the freaky folks.
      */
      for ( d = descriptor_list; d != NULL; d = d_next )
      {
         d_next = d->next;
         if ( FD_ISSET( d->descriptor, &exc_set ) )
         {
            FD_CLR( d->descriptor, &in_set  );
            FD_CLR( d->descriptor, &out_set );
            if ( d->character && d->character->level > 1)
            save_char_obj( d->character );
            d->outtop    = 0;
            close_socket( d );
         }
      }
      /*
      * Process input.
      */
      for ( d = descriptor_list; d != NULL; d = d_next )
      {
         d_next    = d->next;
         d->fcommand    = FALSE;

         if ( FD_ISSET( d->descriptor, &in_set ) )
         {
            if ( d->character != NULL )
            d->character->timer = 0;
            if ( !read_from_descriptor( d ) )
            {
               FD_CLR( d->descriptor, &out_set );
               if ( d->character != NULL && d->character->level > 1)
               save_char_obj( d->character );
               d->outtop    = 0;
               close_socket( d );
               continue;
            }
         }

         if (d->character != NULL && d->character->daze > 0)
         --d->character->daze;

         if ( d->character != NULL && d->character->wait > 0 )
         {
            --d->character->wait;
            continue;
         }

         if (d->character != NULL && d->character->daze > 1)
         continue;

         read_from_buffer( d );
         if ( d->incomm[0] != '\0' )
         {
            d->fcommand    = TRUE;
            stop_idling( d->character );

            if (d->showstr_point)
            show_string(d, d->incomm);
            else if ( d->connected == CON_PLAYING )
            substitute_alias( d, d->incomm );
            else
            nanny( d, d->incomm );

            d->incomm[0]    = '\0';
         }
      }


      /*
      * Autonomous game motion.
      */
      update_handler( );


      /*
      * Output.
      */
      for ( d = descriptor_list; d != NULL; d = d_next )
      {
         d_next = d->next;

         if ( ( d->fcommand || d->outtop > 0 )
         &&   FD_ISSET(d->descriptor, &out_set) )
         {
            if ( !process_output( d, TRUE ) )
            {
               if ( d->character != NULL && d->character->level > 1)
               save_char_obj( d->character );
               d->outtop    = 0;
               close_socket( d );
            }
         }
      }


      /*
      * Synchronize to a clock.
      * Sleep( last_time + 1/PULSE_PER_SECOND - now ).
      * Careful here of signed versus unsigned arithmetic.
      */
      {
         TIMEVAL now_time;
         long secDelta;
         long usecDelta;

         gettimeofday( &now_time, NULL );
         timeval_to_mudtime(&now_time, &current_mud_time);
         usecDelta    = ((int) last_time.tv_usec) - ((int) now_time.tv_usec)
         + 1000000 / PULSE_PER_SECOND;
         secDelta    = ((int) last_time.tv_sec ) - ((int) now_time.tv_sec );
         while ( usecDelta < 0 )
         {
            usecDelta += 1000000;
            secDelta  -= 1;
         }

         while ( usecDelta >= 1000000 )
         {
            usecDelta -= 1000000;
            secDelta  += 1;
         }
         /*
            Modify logon times so that no one ghosts
            because of time mistakes or lag
         */
         fix_logon_time(secDelta);
         if (secDelta < 0)
         {
            /* Seconds data is incorrect if negative */
            secDelta++;
            usecDelta = 1000000 - usecDelta;
            if (secDelta < 0)
            {
               sprintf
               (
                  log_buf,
                  "CPU lag, or Clock set forwards! Difference: %ld.%06ld seconds.",
                  -secDelta,
                  usecDelta
               );
               bug(log_buf, 0);
            }
            else
            {
               sprintf
               (
                  log_buf,
                  "[*****] BUG: CPU lag, or Clock set forwards! Difference: %ld.%06ld seconds.",
                  -secDelta,
                  usecDelta
               );
               log_string(log_buf);
            }
            /* Set standard delay for the pulse */
            usecDelta = 0;
         }
         else if
         (
            secDelta > 0 ||
            usecDelta > 1000000 / PULSE_PER_SECOND
         )
         {
            if (secDelta > 0)
            {
               sprintf
               (
                  log_buf,
                  "Clock set backwards! Difference: %ld.%ld seconds.",
                  secDelta,
                  usecDelta
               );
               bug(log_buf, 0);
            }
            /*
               Time went backwards?
            */
            usecDelta = 0;
         }
         /* Ignores secDelta, max pulse is usecdelta anyway */
         if (usecDelta > 0)
         {
            static TIMEVAL stall_time;

            stall_time.tv_usec = usecDelta;
            if
            (
               select( 0, NULL, NULL, NULL, &stall_time ) < 0 &&
               errno != EINTR
            )
            {
               perror("Game_loop: select: stall");
               exit(1);
            }
         }
      }
      gettimeofday( &last_time, NULL );
      current_time = (time_t) last_time.tv_sec;
      /* time adjust -werv    */    current_time += 0*3600;
   }
   save_wizireport();
   save_clans();

   return;
}
#endif



#if defined(unix)
void init_descriptor(int control)
{
   char buf[MAX_STRING_LENGTH];
   DESCRIPTOR_DATA* dnew;
   struct sockaddr_in sock;
   int desc;
   socklen_t size;
   int addr;

   size = sizeof(sock);
   getsockname(control, (struct sockaddr *) &sock, &size);
   if
   (
      (
         desc = accept(control, (struct sockaddr*)&sock, &size)
      ) < 0
   )
   {
      perror("New_descriptor: accept");
      return;
   }

#if !defined(FNDELAY)
#define FNDELAY O_NDELAY
#endif

   if (fcntl(desc, F_SETFL, FNDELAY) == -1)
   {
      perror("New_descriptor: fcntl: FNDELAY");
      return;
   }

   /*
   * Cons a new descriptor.
   */
   dnew = new_descriptor();

   dnew->descriptor = desc;
   dnew->connected = CON_GET_NAME;
   dnew->showstr_head = NULL;
   dnew->showstr_point = NULL;
   dnew->outsize = 2000;
   dnew->outbuf = alloc_mem(dnew->outsize);
   size = sizeof(sock);
   if (getpeername(desc, (struct sockaddr*)&sock, &size) < 0)
   {
      perror( "New_descriptor: getpeername");
      dnew->host = str_dup("(unknown)");
   }
   else
   {
      /*
      * Would be nice to use inet_ntoa here but it takes a struct arg,
      * which ain't very compatible between gcc and system libraries.
      */

      addr = ntohl(sock.sin_addr.s_addr);
      dnew->hostip = addr;
      sprintf
      (
         buf,
         "%d.%d.%d.%d",
         (addr >> 24) & 0xFF,
         (addr >> 16) & 0xFF,
         (addr >>  8) & 0xFF,
         (addr) & 0xFF
      );
      sprintf(log_buf, "Sock.sinaddr:  %s", buf);
      log_string(log_buf);
      /*
      struct hostent* from;
      TIMEVAL iptime0;
      time_t iptime1;
      time_t iptime2;

      Fizzfaldt-
      This is a major cause of lag.
      To get ip's, just use 'getip' in game, or use whois
      if (check_ban(buf, BAN_IPCHECKING))
      {
      from = NULL;
      }
      else
      {
      / *
      from =
      (check_ip == TRUE) ?
      gethostbyaddr
      (
      (char*)&sock.sin_addr,
      sizeof(sock.sin_addr),
      AF_INET
      ) :
      NULL;
      * /
      / *
      timer = current_time;   -- wrong, current_time won't change in this function
      * /
      gettimeofday(&iptime0, NULL);
      iptime1 = (time_t)iptime0.tv_sec;
      from = gethostbyaddr
      (
      (char*)&sock.sin_addr,
      sizeof(sock.sin_addr),
      AF_INET
      );
      gettimeofday(&iptime0, NULL);
      iptime2 = (time_t) iptime0.tv_sec;
      if ((iptime2 - iptime1) > 5)
      {
      BAN_DATA* pban;
      pban = new_ban();
      pban->name = str_dup(buf);
      pban->level = 52;
      pban->ban_flags = BAN_IPCHECKING;
      SET_BIT(pban->ban_flags, BAN_PERMANENT);
      pban->next = ban_list;
      ban_list = pban;
      save_bans();
      sprintf(log_buf, "%s auto ip check banned by time limit.", buf);
      log_string(log_buf);
      }
      }
      if (from)
      {
      sprintf(log_buf,  "%s -> %s", buf, from->h_name);
      log_string(log_buf);
      }
      dnew->host = str_dup(from ? from->h_name : buf);
      */
      dnew->host = str_dup(buf);
   }

   /*
   * Swiftest: I added the following to ban sites.  I don't
   * endorse banning of sites, but Copper has few descriptors now
   * and some people from certain sites keep abusing access by
   * using automated 'autodialers' and leaving connections hanging.
   *
   * Furey: added suffix check by request of Nickel of HiddenWorlds.

   if ( check_ban(dnew->host, BAN_ALL))
   {
   write_to_descriptor( desc,
   "Your site has been banned from this mud.\n\r", 0 );
   close( desc );
   free_descriptor(dnew);
   return;
   }
   */
   /*
   * Init descriptor data.
   */
   dnew->next = descriptor_list;
   descriptor_list = dnew;

   /*
   * Send the greeting.
   */
   {
      extern char* help_greeting;
      if (help_greeting[0] == '.')
      {
         write_to_buffer(dnew, help_greeting + 1, 0);
      }
      else
      {
         write_to_buffer(dnew, help_greeting, 0);
      }
   }

   return;
}
#endif



void close_socket(DESCRIPTOR_DATA* dclose)
{
   CHAR_DATA* ch;
   CHAR_DATA* ich;
   SNOOP_DATA* snoops;
   SNOOP_DATA* snoop_next;
   DESCRIPTOR_DATA* d;

   if (dclose->original)
   {
      if (IS_SET(dclose->original->comm2, COMM_MUSIC))
      {
         music_off(dclose->original);
      }
   }
   else if
   (
      dclose->character &&
      IS_SET(dclose->character->comm2, COMM_MUSIC)
   )
   {
      music_off(dclose->character);
   }
   if
   (
      (dclose->outtop > 0) &&    /* We have output data in the buffer */
      (dclose->outsize < 32000)  /* We can safely add to the buffer */
   )
   {
      process_output(dclose, FALSE);
   }

   if (dclose->snoops != NULL)
   {
      snoops = dclose->snoops;
      while (snoops != NULL)
      {
         snoop_next = snoops->next;
         write_to_buffer
         (
            snoops->snoop_by,
            "Your victim has left the game.\n\r",
            0
         );
         free_snoop(dclose, snoops);
         snoops = snoop_next;
      }
   }


   for ( d = descriptor_list; d != NULL; d = d->next )
   {
      snoops = d->snoops;
      while(snoops != NULL)
      {
         snoop_next = snoops->next;
         if (snoops->snoop_by == dclose)
         free_snoop(d, snoops);
         snoops = snoop_next;
      }
   }
   if (dclose->original)
   {
      do_return(dclose->character, "");
   }
   if
   (
      dclose->character != NULL &&
      !IS_NPC(dclose->character) &&
      dclose->character->pcdata->spirit_room != NULL
   )
   {
      do_return(dclose->character, "");
   }

   if ((ch = dclose->character) != NULL)
   {
      sprintf
      (
         log_buf,
         "Closing link to %s@%s.",
         ch->name,
         dclose->host
      );
      log_string(log_buf);
      /* cut down on wiznet spam when rebooting */
      if (dclose->connected == CON_PLAYING && !merc_down)
      {
         do_wizireport_update
         (
            (
               dclose->original ?
               dclose->original :
               dclose->character
            ),
            WIZI_UPDATE
         );
         for (ich = ch->in_room->people; ich != NULL; ich = ich->next_in_room)
         {
            if
            (
               ich != ch &&
               ich->desc != NULL &&
               ich->desc->outsize < 32000 &&  /* No infinite loop crashes */
               !(
                  IS_IMMORTAL(ch) &&
                  !can_see(ich, ch)
               )
            )
            {
               act_new
               (
                  "$n has slipped into unconsciousness.",
                  ch,
                  ch,
                  ich,
                  TO_VICT,
                  POS_RESTING
               );
            }
         }

         wiznet
         (
            "Net death has claimed $N.",
            ch,
            NULL,
            WIZ_LINKS,
            0,
            get_trust(ch)
         );
         /* Report char's with quittimers on wiznet - Werv */
         if (ch->quittime > 15)
         {
            wiznet
            (
               "$N's quit timer was above 15.",
               ch,
               NULL,
               WIZ_LINKS,
               0,
               get_trust(ch)
            );
         }
         ch->desc = NULL;
      }
      else
      {
         if
         (
            dclose->connected == CON_PLAYING &&
            merc_down
         )
         {
            do_wizireport_update
            (
               (
                  dclose->original ?
                  dclose->original :
                  dclose->character
               ),
               WIZI_UPDATE
            );
         }
         /* Calculate light correctly */
         (
            dclose->original ?
            dclose->original :
            dclose->character
         )->in_room = NULL;
         die_follower(dclose->original ? dclose->original : dclose->character);
         free_char(dclose->original ? dclose->original : dclose->character);
      }
   }

   if (d_next == dclose)
   {
      d_next = d_next->next;
   }

   if (dclose == descriptor_list)
   {
      descriptor_list = descriptor_list->next;
   }
   else
   {

      for (d = descriptor_list; (d && d->next != dclose); d = d->next)
      {
      }
      if (d != NULL)
      {
         d->next = dclose->next;
      }
      else
      {
         bug("Close_socket: dclose not found.", 0);
      }
   }

   close(dclose->descriptor);
   free_descriptor(dclose);
#if defined(MSDOS) || defined(macintosh)
   exit(1);
#endif
   return;
}



bool read_from_descriptor( DESCRIPTOR_DATA *d )
{
   unsigned int iStart;

   /* Hold horses if pending command already. */
   if ( d->incomm[0] != '\0' )
   return TRUE;

   /* Check for overflow. */
   iStart = strlen(d->inbuf);
   if ( iStart >= sizeof(d->inbuf) - 10 )
   {
      sprintf(log_buf,  "%s input overflow!", d->host );
      log_string( log_buf );
      write_to_descriptor( d->descriptor,
      "\n\r*** PUT A LID ON IT!!! ***\n\r", 0 );
      return FALSE;
   }

   /* Snarf input. */
#if defined(macintosh)
   for ( ; ; )
   {
      int c;
      c = getc( stdin );
      if ( c == '\0' || c == EOF )
      break;
      putc( c, stdout );
      if ( c == '\r' )
      putc( '\n', stdout );
      d->inbuf[iStart++] = c;
      if ( iStart > sizeof(d->inbuf) - 10 )
      break;
   }
#endif

#if defined(MSDOS) || defined(unix)
   for ( ; ; )
   {
      int nRead;

      nRead = read( d->descriptor, d->inbuf + iStart,
      sizeof(d->inbuf) - 10 - iStart );
      if ( nRead > 0 )
      {
         iStart += nRead;
         if ( d->inbuf[iStart-1] == '\n' || d->inbuf[iStart-1] == '\r' )
         break;
      }
      else if ( nRead == 0 )
      {
         log_string( "EOF encountered on read." );
         return FALSE;
      }
      else if ( errno == EAGAIN )
      break;
      else
      {
         perror( "Read_from_descriptor" );
         return FALSE;
      }
   }
#endif

   d->inbuf[iStart] = '\0';
   return TRUE;
}



/*
* Transfer one line from input buffer to input line.
*/
void read_from_buffer( DESCRIPTOR_DATA *d )
{
   int i, j, k;

   /*
   * Hold horses if pending command already.
   */
   if ( d->incomm[0] != '\0' )
   return;

   /*
   * Look for at least one new line.
   */
   for ( i = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++ )
   {
      if ( d->inbuf[i] == '\0' )
      return;
   }

   /*
   * Canonical input processing.
   */
   for ( i = 0, k = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++ )
   {
      if ( k >= MAX_INPUT_LENGTH - 2 )
      {
         write_to_descriptor( d->descriptor, "Line too long.\n\r", 0 );

         /* skip the rest of the line */
         for ( ; d->inbuf[i] != '\0'; i++ )
         {
            if ( d->inbuf[i] == '\n' || d->inbuf[i] == '\r' )
            break;
         }
         d->inbuf[i]   = '\n';
         d->inbuf[i+1] = '\0';
         break;
      }

      if ( d->inbuf[i] == '\b' && k > 0 )
      --k;
      else if ( isascii(d->inbuf[i]) && isprint(d->inbuf[i]) )
      d->incomm[k++] = d->inbuf[i];
   }

   /*
   * Finish off the line.
   */
   if ( k == 0 )
   d->incomm[k++] = ' ';
   d->incomm[k] = '\0';

   /*
   * Deal with bozos with #repeat 1000 ...
   */

   if ( k > 1 || d->incomm[0] == '!' )
   {
      if ( d->incomm[0] != '!' && strcmp( d->incomm, d->inlast ) )
      {
         d->repeat = 0;
      }
      else
      {
         if (++d->repeat >= 25 && d->character
         &&  d->connected == CON_PLAYING)
         {
            if (IS_IMMORTAL(d->character))
            d->repeat = 0;
            else
            {
               sprintf(log_buf,  "%s input spamming!", d->host );
               log_string( log_buf );
               wiznet("Spam spam spam $N spam spam spam spam spam!",
               d->character, NULL, WIZ_SPAM, 0, get_trust(d->character));
               if (d->incomm[0] == '!')
               wiznet(d->inlast, d->character, NULL, WIZ_SPAM, 0,
               get_trust(d->character));
               else
               wiznet(d->incomm, d->character, NULL, WIZ_SPAM, 0,
               get_trust(d->character));

               d->repeat = 0;
               d->character->move/=2;
               d->character->mana-=20;
               d->character->exp=d->character->exp-100;
               write_to_descriptor(d->descriptor,
               "\n\rYour mind feels drained from trying to do the same thing over and over again...\n\r", 0);
               WAIT_STATE(d->character, 24);
               if (!IS_NPC(d->character))
               {
                  if (d->character->pcdata->spam < current_time)
                  {
                     d->character->pcdata->spam = current_time;
                  }
                  d->character->pcdata->spam += SPAM_PENALTY;
               }
            }
         }
      }
   }


   /*
   * Do '!' substitution.
   */
   if ( d->incomm[0] == '!' )
   strcpy( d->incomm, d->inlast );
   else
   strcpy( d->inlast, d->incomm );

   /*
   * Shift the input buffer.
   */
   while ( d->inbuf[i] == '\n' || d->inbuf[i] == '\r' )
   i++;
   for ( j = 0; ( d->inbuf[j] = d->inbuf[i+j] ) != '\0'; j++ )
   ;
   return;
}



/*
* Low level output function.
*/
bool process_output( DESCRIPTOR_DATA *d, bool fPrompt )
{
   extern bool merc_down;
   SNOOP_DATA *snoops;

   /*
   * Bust a prompt.
   */
   if (!merc_down && d->showstr_point)
   {
      write_to_buffer(d, "[Hit Return to continue]\n\r", 0);
   }
   else if (fPrompt && !merc_down && d->connected == CON_PLAYING)
   {
      CHAR_DATA *ch;
      CHAR_DATA *victim;

      ch = d->character;

      fix_fighting(ch);
      /* battle prompt */
      if ((victim = ch->fighting) != NULL && can_see(ch, victim))
      {
         int percent;
         char wound[100];
         char buf[MAX_STRING_LENGTH];

         if (IS_SET(ch->act, PLR_EVALUATION)
         && (get_skill(ch, gsn_evaluation) >= 1))
         {
            if ((number_percent() > get_skill(ch, gsn_evaluation))
            /*    && (get_skill(ch, gsn_evaluation) < 100)*/
            && !IS_NPC(ch) && (ch->pcdata->learned[gsn_evaluation] < 100))
            {
               send_to_char("You lose concentration and stop evaluating your opponents.\n\r", ch);
               REMOVE_BIT(ch->act, PLR_EVALUATION);
               if
               (
                  ch->pcdata->spam < current_time &&
                  !is_affected(ch, gsn_smite) &&
                  !is_affected(ch, gsn_punishment)
               )
               {
                  int old_val = ch->pcdata->learned[gsn_evaluation];
                  ch->pcdata->learned[gsn_evaluation] += 1;
                  ch->pcdata->learned[gsn_evaluation] = UMIN(ch->pcdata->learned[gsn_evaluation], 100);
                  if (old_val != ch->pcdata->learned[gsn_evaluation])
                  {
                     ch->pcdata->updated[gsn_evaluation] = TRUE;
                  }
                  send_to_char("You learn from your mistakes, and your evaluation skill improves.\n\r", ch);
                  save_char_obj( ch );
               }
            }
         }

         if (IS_SET(ch->act, PLR_EVALUATION))
         {
            if (victim->max_hit > 0)
            percent = victim->hit * 100 / victim->max_hit;
            else
            percent = -1;

            if (percent >= 100)
            sprintf(wound, "is in perfect condition.");
            else if (percent >= 95)
            sprintf(wound, "has a few scratches.");
            else if (percent >= 90)
            sprintf(wound, "has a few bruises.");
            else if (percent >= 80)
            sprintf(wound, "has some small wounds.");
            else if (percent >= 70)
            sprintf(wound, "has quite a few wounds.");
            else if (percent >= 60)
            sprintf(wound, "is covered in bleeding wounds.");
            else if (percent >= 50)
            sprintf(wound, "is bleeding profusely.");
            else if (percent >= 40)
            sprintf(wound, "is gushing blood.");
            else if (percent >= 30)
            sprintf(wound, "is screaming in pain.");
            else if (percent >= 20)
            sprintf(wound, "is spasming in shock.");
            else if (percent >= 10)
            sprintf(wound, "is writhing in agony.");
            else if (percent >= 1)
            sprintf(wound, "is convulsing on the ground.");
            else
            sprintf(wound, "is nearly dead.");
         }
         else
         {
            if (victim->max_hit > 0)
            percent = victim->hit * 100 / victim->max_hit;
            else
            percent = -1;

            if (percent >= 100)
            sprintf(wound, "is in perfect condition.");
            else if (percent >= 90)
            sprintf(wound, "has a few scratches.");
            else if (percent >= 75)
            sprintf(wound, "has some small wounds.");
            else if (percent >= 50)
            sprintf(wound, "has some nasty cuts.");
            else if (percent >= 30)
            sprintf(wound, "is bleeding profusely.");
            else if (percent >= 15)
            sprintf(wound, "is screaming in pain.");
            else if (percent >= 0)
            sprintf(wound, "is in pretty bad shape.");
            else
            sprintf(wound, "is nearly dead.");
         }

         if (!is_affected(victim, gsn_cloak_form))
         {
            sprintf(buf, "%s %s \n\r",
            IS_NPC(victim) ? victim->short_descr : victim->name, wound);
            buf[0] = UPPER(buf[0]);
            write_to_buffer( d, buf, 0);
         }
         else
         {
            sprintf(buf, "%s %s \n\r",
            IS_NPC(victim) ? victim->short_descr : "The cloaked figure", wound);
            write_to_buffer( d, buf, 0);
         }
      }


      ch = d->original ? d->original : d->character;
      if (!IS_SET(ch->comm, COMM_COMPACT) )
      write_to_buffer( d, "\n\r", 2 );


      if ( IS_SET(ch->comm, COMM_PROMPT) )
      bust_a_prompt( d->character );

      if (IS_SET(ch->comm, COMM_TELNET_GA))
      write_to_buffer(d, go_ahead_str, 0);
   }

   /*
   * Short-circuit if nothing to write.
   */
   if ( d->outtop == 0 )
   return TRUE;

   /*
   * Snoop-o-rama.
   */
   if ( d->snoops != NULL )
   {
      snoops = d->snoops;
      while(snoops != NULL)
      {
         if (d->character != NULL)
         write_to_buffer( snoops->snoop_by, d->character->name, 0);
         write_to_buffer( snoops->snoop_by, "> ", 2 );
         write_to_buffer( snoops->snoop_by, d->outbuf, d->outtop );
         snoops = snoops->next;
      }
   }

   /*
   * OS-dependent output.
   *
   * now done at output_buffer( ) to deal with color codes.
   * - Wreck
   */
   /*    return output_buffer( d );*/
   if ( !write_to_descriptor( d->descriptor, d->outbuf, d->outtop ) )
   {
      d->outtop = 0;
      return FALSE;
   }
   else
   {
      d->outtop = 0;
      return TRUE;
   }
}

/*
* Bust a prompt (player settable prompt)
* coded by Morgenes for Aldara Mud
*/
void bust_a_prompt( CHAR_DATA *ch )
{
   char buf[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];
   const char *str;
   const char *i;
   char *point;
   char doors[MAX_INPUT_LENGTH];
   EXIT_DATA *pexit;
   bool found;
   const char *dir_name[] = {"N", "E", "S", "W", "U", "D"};
   int door;

   point = buf;
   str = ch->prompt;
   if (str == NULL || str[0] == '\0')
   {
      sprintf( buf, "<%dhp %dmn %dmv> %s",
      ch->hit, ch->mana, ch->move, ch->prefix);
      send_to_char(buf, ch);
      return;
   }

   if (IS_SET(ch->comm, COMM_AFK))
   {
      send_to_char("<AFK> ", ch);
      return;
   }

   while( *str != '\0' )
   {
      if ( *str != '%' )
      {
         *point++ = *str++;
         continue;
      }
      ++str;
      switch( *str )
      {
         default :
         i = " "; break;
         case 'e':
         found = FALSE;
         doors[0] = '\0';
         for (door = 0; door < 6; door++)
         {
            if ((pexit = ch->in_room->exit[door]) != NULL
            &&  pexit ->u1.to_room != NULL
            &&  (can_see_room(ch, pexit->u1.to_room)
            ||   (IS_AFFECTED(ch, AFF_INFRARED)
            &&    !IS_AFFECTED(ch, AFF_BLIND)))
            &&  !IS_SET(pexit->exit_info, EX_CLOSED))
            {
               found = TRUE;
               strcat(doors, dir_name[door]);
            }
         }
         if (!found)
         strcat(buf, "none");
         sprintf(buf2, "%s", doors);
         i = buf2; break;
         case 'c' :
         sprintf(buf2, "%s", "\n\r");
         i = buf2; break;
         case 'h' :
         if (ch->level >= 30)
         sprintf( buf2, "%d", ch->hit );
         else
         sprintf( buf2, "%d%%",
         (ch->max_hit == 0) ? 0 :
         (ch->hit*100)/ch->max_hit );
         i = buf2; break;
         case 'H' :
         sprintf( buf2, "%d", ch->max_hit );
         i = buf2; break;
         case 'm' :
         if (ch->level >= 30)
         sprintf( buf2, "%d", ch->mana );
         else
         sprintf( buf2, "%d%%",
         (ch->max_mana == 0) ? 0 :  (ch->mana*100)/ch->max_mana );
         i = buf2; break;
         case 'M' :
         sprintf( buf2, "%d", ch->max_mana );
         i = buf2; break;
         case 'v':
         if (ch->level >= 30)
         sprintf( buf2, "%d", ch->move );
         else
         sprintf( buf2, "%d%%",
         (ch->max_move == 0) ? 0 : (ch->move*100)/ch->max_move );
         i = buf2; break;
         case 'V' :
         sprintf( buf2, "%d", ch->max_move );
         i = buf2; break;
         case 'x' :
         sprintf( buf2, "%d", ch->exp );
         i = buf2; break;
         case 'X' :
         sprintf(buf2, "%d", IS_NPC(ch) ? 0 :
         ch->level * exp_per_level(ch) - ch->exp);
         i = buf2; break;
         case 'g' :
         sprintf( buf2, "%ld", ch->gold);
         i = buf2; break;
         case 's' :
         sprintf( buf2, "%ld", ch->silver);
         i = buf2; break;
         case 'a' :
         if ( ch->level > 9 )
         sprintf( buf2, "%d", ch->alignment );
         else
         sprintf( buf2, "%s", IS_GOOD(ch) ? "good" : IS_EVIL(ch) ?
         "evil" : "neutral" );
         i = buf2; break;
         case 'r' :
         if ( ch->in_room != NULL )
         sprintf( buf2, "%s",
         ((!IS_NPC(ch) && IS_SET(ch->act, ACT_HOLYLIGHT)) ||
         (!IS_AFFECTED(ch, AFF_BLIND) && !room_is_dark( ch->in_room )))
         ? ch->in_room->name : "darkness");
         else
         sprintf( buf2, " " );
         i = buf2; break;
         case 'R' :
         if ( IS_IMMORTAL( ch ) && ch->in_room != NULL )
         sprintf( buf2, "%d", ch->in_room->vnum );
         else
         sprintf( buf2, " " );
         i = buf2; break;
         case 'z' :
         if ( IS_IMMORTAL( ch ) && ch->in_room != NULL )
         sprintf( buf2, "%s", ch->in_room->area->name );
         else
         sprintf( buf2, " " );
         i = buf2; break;
         case '%' :
         sprintf( buf2, "%%" );
         i = buf2; break;
      }
      ++str;
      while( (*point = *i) != '\0' )
      ++point, ++i;
   }
   write_to_buffer( ch->desc, buf, point - buf );

   if (ch->prefix[0] != '\0')
   write_to_buffer(ch->desc, ch->prefix, 0);
   return;
}

/*
* output_buffer( descriptor )
* this function sends output down a socket. Color codes are stripped off
* is the player is not using color, or converted to ANSI color sequences
* to provide colored output.
* When using ANSI, the buffer can become a lot larger due to the (sometimes)
* lengthy ANSI sequences, thus potentially overflowing the buffer. Therefor
* *new* buffer is send in chunks.
* The 'bzero's may seem unnecessary, but i didn't want to take risks.
*
* - Wreck
*/

bool output_buffer( DESCRIPTOR_DATA *d )
{
   char    buf[MAX_STRING_LENGTH];
   char    buf2[128];
   const char     *str;
   char     *i;
   char     *point;
   bool    flash=FALSE, o_flash,
   bold=FALSE, o_bold;
   bool    act=FALSE, ok=TRUE, color_code=FALSE;
   int        color=7, o_color;

   /* discard NULL descriptor */
   if ( d==NULL )
   return FALSE;

   bzero( buf, MAX_STRING_LENGTH );
   point=buf;
   str=d->outbuf;
   o_color=color;
   o_bold=bold;
   o_flash=flash;

   while ( *str != '\0' && (str-d->outbuf)<d->outtop )
   {
      if ( (int)(point-buf)>=MAX_STRING_LENGTH-32 )
      {
         /* buffer is full, so send it through the socket */
         *point++='\0';
         if ( !(ok=write_to_descriptor( d->descriptor,
         buf,
         strlen( buf ) )) )
         break;
         bzero( buf, MAX_STRING_LENGTH );
         point=buf;
      }

      if ( *str != '{' )
      {
         color_code=FALSE;
         *point++ = *str++;
         continue;
      }

      if ( !color_code && *(str+1)!='<' )
      {
         o_color=color;
         o_bold=bold;
         o_flash=flash;
      }
      color_code=TRUE;

      act=FALSE;
      str++;
      switch ( *str )
      {
         default:    sprintf( buf2, "{%c", *str );                break;
         case 'x':     sprintf( buf2, "{" );                        break;
         case '-':     sprintf( buf2, "~" );                        break;
         case '<':     color=o_color; bold=o_bold; flash=o_flash;
         act=TRUE; break;
         case '0':    color=0;                  act=TRUE; break;
         case '1':    color=1;                  act=TRUE; break;
         case '2':    color=2;                  act=TRUE; break;
         case '3':    color=3;                  act=TRUE; break;
         case '4':    color=4;                  act=TRUE; break;
         case '5':    color=5;                  act=TRUE; break;
         case '6':    color=6;                  act=TRUE; break;
         case '7':    color=7;                 act=TRUE; break;
         case 'B':    bold=TRUE;                    act=TRUE; break;
         case 'b':    bold=FALSE;                   act=TRUE; break;
         case 'F':    flash=TRUE;                   act=TRUE; break;
         case 'f':    flash=FALSE;                  act=TRUE; break;
         case 'n':    if ( d->character && IS_ANSI( d->character ) )
         sprintf( buf2, "%s", ANSI_NORMAL );
         else
         buf2[0]='\0';
         bold=FALSE; color=7; flash=FALSE;    break;
      }
      if ( act )
      {
         if ( d->character && IS_ANSI( d->character ) )
         {
            sprintf( buf2, "%s", color_value_string( color, bold, flash ) );
            color_code=TRUE;
         }
         else
         buf2[0]='\0';
      }

      i=buf2;
      str++;
      while ( ( *point = *i ) != '\0' )
      ++point, ++i;
   }

   *point++='\0';
   ok=ok && (write_to_descriptor( d->descriptor, buf, strlen( buf ) ));
   d->outtop=0;

   return ok;
}


/*
* Append onto an output buffer.
*/
void write_to_buffer( DESCRIPTOR_DATA *d, const char *txt, int length )
{
   /*
   * Find length in case caller didn't.
   */
   if ( length <= 0 )
   length = strlen(txt);

   /*
   * Initial \n\r if needed.
   */
   if ( d->outtop == 0 && !d->fcommand )
   {
      d->outbuf[0]    = '\n';
      d->outbuf[1]    = '\r';
      d->outtop    = 2;
   }

   /*
   * Expand the buffer as needed.
   */
   while ( d->outtop + length >= d->outsize )
   {
      char *outbuf;

      if (d->outsize >= 32000)
      {
         bug_trust
         (
            "Buffer overflow. Closing.\n\r",
            0,
            d->character ?
            get_trust(d->character) + 1 :  /* Infinite loop here otherwise */
            54
         );
         close_socket(d);
         return;
      }
      outbuf      = alloc_mem( 2 * d->outsize );
      strncpy( outbuf, d->outbuf, d->outtop );
      free_mem( d->outbuf, d->outsize );
      d->outbuf   = outbuf;
      d->outsize *= 2;
   }

   /*
   * Copy.
   */
   strcpy( d->outbuf + d->outtop, txt );
   d->outtop += length;
   return;
}



/*
* Lowest level output function.
* Write a block of text to the file descriptor.
* If this gives errors on very long blocks (like 'ofind all'),
*   try lowering the max block size.
*/
bool write_to_descriptor( int desc, char *txt, int length )
{
   int iStart;
   int nWrite;
   int nBlock;

#if defined(macintosh) || defined(MSDOS)
   if ( desc == 0 )
   desc = 1;
#endif

   if ( length <= 0 )
   length = strlen(txt);

   for ( iStart = 0; iStart < length; iStart += nWrite )
   {
      nBlock = UMIN( length - iStart, 4096 );
      if ( ( nWrite = write( desc, txt + iStart, nBlock ) ) < 0 )
      { perror( "Write_to_descriptor" ); return FALSE; }
   }

   return TRUE;
}



/*
* Deal with sockets that haven't logged in yet.
*/
void nanny( DESCRIPTOR_DATA *d, char *argument )
{
   DESCRIPTOR_DATA *d_old, *d_next;
   ROOM_INDEX_DATA* room;
   char buf[MAX_STRING_LENGTH];
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA* ch;
   CHAR_DATA* ich;
   CHAR_DATA* ich_next;
   CHAR_DATA* old = NULL;
   /*    OBJ_DATA *fobj;*/ /* For pfile limit bug */
   /*    OBJ_DATA *fobj_next; */
   char *pwdnew;
   char *p;
   int iClass, race, i, sn;
   bool fOld;
   int dh;
   int mainclass;
   int subclass;
   int hometown;
   int ht_choice = -1;
   int align;

   while (isspace(*argument))
   {
      argument++;
   }

   ch = d->character;

   switch ( d->connected )
   {

      default:
      {
         bug("Nanny: bad d->connected %d.", d->connected);
         close_socket(d);
         return;
      }

      case (CON_GET_STORAGE_NAME):
      {
         if (argument[0] == '\0')
         {
            close_socket(d);
            return;
         }

         argument[0] = UPPER(argument[0]);

         if (!load_char_obj2(d, argument))
         {
            write_to_buffer(d, "Character does not exist in storage.\n\r", 0);
            close_socket(d);
            return;
         }
         ch = d->character;
         if
         (
            ch->desc &&
            ch->desc->host
         )
         {
            free_string(ch->host);
            ch->host = str_dup(ch->desc->host);
         }
         sprintf(buf, "Loaded %s from %s.", ch->name, d->host);
         log_string(buf);

         if (check_ban(d->host, BAN_ALL) && !IS_SET(ch->act, PLR_PERMIT))
         {
            write_to_buffer
            (
               d,
               "Your site has been banned from this mud.\n\r",
               0
            );
            sprintf
            (
               log_buf,
               "%s@%s denied access due to site ban.\n\r",
               ch->name,
               d->host
            );
            log_string(log_buf);
            close_socket(d);
            return;
         }

         if (wizlock && !IS_IMMORTAL(ch))
         {
            write_to_buffer(d, "The game is wizlocked.\n\r", 0);
            close_socket(d);
            return;
         }

         write_to_buffer(d, "Password: ", 0);
         write_to_buffer(d, echo_off_str, 0);
         d->connected = CON_GET_OLD_PASSWORD;
         ch->pcdata->being_restored = TRUE;
         return;
         break;
      }

      case (CON_GET_NAME):
      {
         if (argument[0] == '\0')
         {
            close_socket(d);
            return;
         }

         if (!str_cmp(argument, "restore") || !str_cmp(argument, "storage"))
         {
            d->connected = CON_GET_STORAGE_NAME;
            write_to_buffer(d, "\n\rStored Characters Name: ", 0);
            return;
         }

         argument[0] = UPPER(argument[0]);

         if
         (
            check_nameban(argument, BAN_PERMIT) ||
            check_nameban(argument, BAN_ALL) ||
            check_nameban(argument, BAN_PERMANENT)
         )
         {
            write_to_buffer(d, "Your name has been banned from this mud.\n\r", 0);
            close_socket(d);
            return;
         }

         fOld = load_char_obj(d, argument);
         if (!fOld && !check_parse_name(argument))
         {
            /*
               Do not prevent characters from logging in
               if mobs with their names are created after they are.
            */
            die_follower(d->character);
            free_char(d->character);
            d->character = NULL;
            write_to_buffer(d, "Illegal name, try another.\n\rName: ", 0);
            return;
         }

         ch = d->character;
         if
         (
            ch->desc &&
            ch->desc->host
         )
         {
            free_string(ch->host);
            ch->host = str_dup(ch->desc->host);
         }
         sprintf(buf, "Loaded %s from %s.", ch->name, d->host);
         log_string(buf);

         if (IS_SET(ch->act, PLR_DENY))
         {
            sprintf(log_buf, "Denying access to %s@%s.", argument, d->host);
            log_string(log_buf);
            write_to_buffer(d, "You are denied access.\n\r", 0);
            close_socket(d);
            return;
         }

         if (check_ban(d->host, BAN_ALL) && !IS_SET(ch->act, PLR_PERMIT))
         {
            write_to_buffer(d, "Your site has been banned from this mud.\n\r", 0);
            sprintf
            (
               log_buf,
               "%s@%s denied access due to site ban.\n\r",
               ch->name,
               d->host
            );
            log_string(log_buf);
            close_socket(d);
            return;
         }
         old = NULL;
         if (check_reconnect(d, argument, FALSE, &old))
         {
            fOld = TRUE;
         }
         else
         {
            if (wizlock && !IS_IMMORTAL(ch))
            {
               write_to_buffer(d, "The game is wizlocked.\n\r", 0);
               close_socket(d);
               return;
            }
         }

         if (fOld)
         {
            /* Old player */
            write_to_buffer(d, "Password: ", 0);
            write_to_buffer(d, echo_off_str, 0);
            d->connected = CON_GET_OLD_PASSWORD;
            return;
         }
         else
         {
            /* New player */
            if (newlock)
            {
               write_to_buffer(d, "The game is newlocked.\n\r", 0);
               close_socket(d);
               return;
            }

            if (check_ban(d->host, BAN_NEWBIES))
            {
               write_to_buffer
               (
                  d,
                  "New players are not allowed from your site.\n\r",
                  0
               );
               close_socket(d);
               return;
            }

            if (check_nameban(argument, BAN_NEWBIES))
            {
               write_to_buffer
               (
                  d,
                  "New players are not allowed with that name.\n\r",
                  0
               );
               close_socket(d);
               return;
            }

            /* To make sure people see our name policy */
            write_to_buffer
            (
               d,
               "\n\r\n\r"
               "Darkmists naming policy take heed:\n\r"
               ">  This is a rp mud, choose a name that is appropriate to a medieval setting.\n\r"
               ">  Do not string two names together.(i.e. SunStarer)\n\r"
               ">  Do not use names with multiple capitalization.(i.e. McNeil)\n\r"
               ">  Do not use bad names such as Dragonslayer, Goddess, or Megaman.\n\r"
               ">> If you roll with a bad name, we will make you reroll your character.\n\r"
               "   We will deny on sight, no questions.\n\r\n\r",
               0
            );
            sprintf(buf, "Did I get that right, %s (Y/N)? ", argument);
            write_to_buffer(d, buf, 0);
            d->connected = CON_CONFIRM_NEW_NAME;
            return;
         }
         break;
      }
      case (CON_GET_SURNAME):
      {
         char* temp;
         char* first;
         char  last;
         int   cap;
         int   apostraphe;
         int   hyphen;

         free_string(ch->pcdata->surname);
         ch->pcdata->surname = NULL;
         if (argument[0] == '\0')
         {
            d->connected = CON_CONFIRM_SURNAME;
            ch->pcdata->surname = &str_empty[0];
            write_to_buffer
            (
               d,
               "\n\r\n\r"
               "Really choose no surname?  You will not get another chance.\n\r"
               "(Yes/No)? ",
               0
            );
            return;
         }
         strcpy(buf, argument);
         buf[0] = UPPER(buf[0]);
         cap = 0;
         apostraphe = 0;
         hyphen = 0;
         if (strlen(buf) > 15)
         {
            write_to_buffer
            (
               d,
               "\n\r"
               "Surname too long.\n\r\n\rPlease pick a surname:\n\r"
               "(Just return for no surname)",
               0
            );
            return;
         }
         if (strlen(buf) < 2)
         {
            write_to_buffer
            (
               d,
               "\n\r"
               "Surname too short.\n\r\n\rPlease pick a surname:\n\r"
               "(Just return for no surname)",
               0
            );
            return;
         }
         first = &buf[0];
         last = *first;
         for (temp = &buf[0]; *temp != '\0'; temp++)
         {
            last = *temp;
            if
            (
               *temp >= 'a' &&
               *temp <= 'z'
            )
            {
               continue;
            }
            if
            (
               *temp >= 'A' &&
               *temp <= 'Z'
            )
            {
               if (++cap > 2)
               {
                  write_to_buffer
                  (
                     d,
                     "\n\r\n\r"
                     "You may have no more than two capital letters in your\n\r"
                     "surname. Including the first letter which must be\n\r"
                     "capitalized.\n\r\n\rPlease pick a surname:\n\r"
                     "(Just return for no surname)",
                     0
                  );
                  return;
               }
               continue;
            }
            if (*temp == '\'')
            {
               if
               (
                  temp == first ||
                  ++apostraphe > 1
               )
               {
                  write_to_buffer
                  (
                     d,
                     "\n\r\n\r"
                     "You may have no more than one apostraphe in your\n\r"
                     "surname.\n\r\n\rPlease pick a surname:\n\r"
                     "(Just return for no surname)",
                     0
                  );
                  return;
               }
               continue;
            }
            if (*temp == '-')
            {
               if
               (
                  temp == first ||
                  ++hyphen > 1
               )
               {
                  write_to_buffer
                  (
                     d,
                     "\n\r\n\r"
                     "You may have no more than one hyphen in your\n\r"
                     "surname.\n\r\n\rPlease pick a surname:\n\r"
                     "(Just return for no surname)",
                     0
                  );
                  return;
               }
               continue;
            }
            write_to_buffer
            (
               d,
               "\n\r\n\r"
               "Invalid character(s) in your surname.\n\r"
               "\n\rPlease pick a surname:\n\r"
               "(Just return for no surname)",
               0
            );
            return;
         }
         if
         (
            last == '\'' ||
            last == '-' ||
            (
               last >= 'A' &&
               last <= 'Z'
            )
         )
         {
            write_to_buffer
            (
               d,
               "\n\r\n\r"
               "Invalid final character in your surname.\n\r"
               "\n\rPlease pick a surname:\n\r"
               "(Just return for no surname)",
               0
            );
            return;
         }
         ch->pcdata->surname = str_dup(buf);
         sprintf
         (
            buf,
            "Did I get that right, %s for a surname. (Yes/No)? ",
            ch->pcdata->surname
         );
         write_to_buffer(d, buf, 0);
         d->connected = CON_CONFIRM_SURNAME;
         break;
      }
      case (CON_CONFIRM_SURNAME):
      {
         if (!str_prefix(argument, "No"))
         {
            write_to_buffer
            (
               d,
               "\n\r\n\r"
               "\n\rAlright, please pick a new surname:\n\r"
               "(Just return for no surname)",
               0
            );
            d->connected = CON_GET_SURNAME;
            return;
         }
         if (!str_prefix(argument, "Yes"))
         {
            if (ch->level > 0)
            {
               d->connected = CON_READ_MOTD;
               nanny(d, "");
               return;
            }
            sprintf
            (
               buf,
               "Give me a password for %s%s%s: %s",
               ch->name,
               (
                  ch->pcdata->surname[0] == '\0' ?
                  "" :
                  " "
               ),
               ch->pcdata->surname,
               echo_off_str
            );
            write_to_buffer(d, buf, 0);
            d->connected = CON_GET_NEW_PASSWORD;
            break;
         }
         sprintf
         (
            buf,
            "Please type Yes or No.\n\r"
            "Did I get that right, %s for a surname. (Yes/No)? ",
            (
               ch->pcdata->surname[0] == '\0' ?
               "nothing" :
               ch->pcdata->surname
            )
         );
         if (ch->pcdata->surname[0] == '\0')
         {
            strcat
            (
               buf,
               "\n\rRemember, if you choose no surname, you will not get"
               " another chance to pick one.\n\r"
            );
         }
         write_to_buffer(d, buf, 0);
         d->connected = CON_CONFIRM_SURNAME;
         break;
      }

      case (CON_GET_OLD_PASSWORD):
      {
#if defined(unix)
         write_to_buffer(d, "\n\r", 2);
#endif

         if
         (
            (
               (
                  ch->pcdata->pwd &&
                  ch->pcdata->pwd[0] != '\0'
               ) ||
               (
                  ch->pcdata->old_pwd &&
                  ch->pcdata->old_pwd[0] != '\0'
              )
            ) &&
            (
               (
                  ch->pcdata->pwd == NULL ||
                  ch->pcdata->pwd[0] == '\0' ||
                  strcmp(md5_hash(argument), ch->pcdata->pwd)
               ) &&
               (
                  ch->pcdata->old_pwd == NULL ||
                  ch->pcdata->old_pwd[0] == '\0' ||
                  strcmp(crypt(argument, ch->pcdata->old_pwd), ch->pcdata->old_pwd)
               )
            )
         )
         {
            sprintf
            (
               log_buf,
               "Closing %s for a bad password from %s.",
               ch->name,
               d->host
            );
            log_string (log_buf);
            write_to_buffer(d, "Wrong password.\n\r", 0);
            if
            (
               check_reconnect(d, argument, FALSE, &old) ||
               check_playing(d, ch->name, &old, TRUE)
            )
            {
               old->pcdata->hacked++;
               if (ch->pcdata->being_restored == FALSE)
               {
                  save_char_obj(old);
               }
               ch->pcdata->hacked = old->pcdata->hacked;
            }
            else
            {
               ch->pcdata->hacked++;
               if (ch->pcdata->being_restored == FALSE)
               {
                  save_char_obj(ch);
               }
            }
            if (ch->pcdata->hacked > 2)
            {
               sprintf
               (
                  log_buf,
                  "%s(%s) has failed to login %d times from %s.",
                  ch->name,
                  ch->pcdata->last_site,
                  ch->pcdata->hacked,
                  d->host
               );
               /*
               make_note
               (
               "Automated Hacker Report",
               "Hack Info",
               "Immortal",
               buf,
               NOTE_NOTE
               );
               */
               log_string(log_buf);
            }
            /*
            * This  should hopefully handle the pfile obj->limcount bug with bad pwds
            */
            /*    for (fobj = ch->carrying; fobj != NULL; fobj = fobj_next)
            {
            fobj_next = fobj->next_content;
            fobj->pIndexData->limcount++;*/ /* Increment count due to drop
            from */
            /* bad password bug */
            /*    }*/

            close_socket(d);
            return;
         }
         if
         (
            ch->pcdata->old_pwd &&
            ch->pcdata->old_pwd[0] != '\0' &&
            (
               !ch->pcdata->pwd ||
               ch->pcdata->pwd[0] == '\0'
            )
         )
         {
            /* Convert hash in pfile to md5 */
            ch->pcdata->pwd = str_dup(md5_hash(argument));
         };
         write_to_buffer(d, echo_on_str, 0);

         if (check_playing(d, ch->name, &old, FALSE))
         {
            return;
         }

         if (check_reconnect(d, ch->name, TRUE, &old))
         {
            return;
         }

         free_string(ch->pcdata->last_site);
         ch->pcdata->last_site = str_dup(d->host);
         sprintf
         (
            log_buf,
            "%s@%s has connected.",
            ch->name,
            d->host
         );
         log_string(log_buf);
         wiznet(log_buf, NULL, NULL, WIZ_SITES, 0, get_trust(ch));

         /* old monk pick schools here!!! */

         /* new subclass structure - Mael */
         if (ch->pcdata->special == SUBCLASS_NONE)
         {
            switch(ch->class)
            {
               case (CLASS_RANGER):
               {
                  dump_rang_subs(d);
                  write_to_buffer
                  (
                     d,
                     "For a short while, you may also \"wait\" to choose a"
                     " subclass.\n\r",
                     0
                  );
                  d->connected = CON_GET_SUBCLASS;
                  return;
                  break;
               }
               case (CLASS_CHANNELER):
               {
                  dump_chan_subs(d);
                  write_to_buffer
                  (
                     d,
                     "For a short while, you may also \"wait\" to choose a"
                     " subclass.\n\r",
                     0
                  );
                  d->connected = CON_GET_SUBCLASS;
                  return;
                  break;
               }
               case (CLASS_ELEMENTALIST):
               {
                  if (allow_elem)
                  {
                     dump_elem_subs(d);
                     write_to_buffer
                     (
                        d,
                        "For a short while, you may also \"wait\" to choose a"
                        " subclass.\n\r",
                        0
                     );
                     d->connected = CON_GET_SUBCLASS;
                     return;
                  }
                  break;
               }
               case (CLASS_THIEF):
               {
                  if (allow_thief)
                  {
                     dump_thief_subs(d);
                     write_to_buffer
                     (
                        d,
                        "For a short while, you may also \"wait\" to choose a"
                        " subclass.\n\r",
                        0
                     );
                     d->connected = CON_GET_SUBCLASS;
                     return;
                  }
                  break;
               }
               case (CLASS_MONK):
               {
                  dump_monk_subs(d);
                  d->connected = CON_GET_SUBCLASS;
                  return;
               }
               case (CLASS_ANTI_PALADIN):
               {
                  dump_ap_subs(d);
                  write_to_buffer
                  (
                     d,
                     "For a short while, you may also \"wait\" to choose a"
                     " subclass.\n\r",
                     0
                  );
                  d->connected = CON_GET_SUBCLASS;
                  return;
               }
               case (CLASS_NECROMANCER):
               {
                  dump_necro_subs(d);
                  write_to_buffer
                  (
                     d,
                     "For a short while, you may also \"wait\" to choose a"
                     " subclass.\n\r",
                     0
                  );
                  d->connected = CON_GET_SUBCLASS;
                  return;
                  break;
               }
               case (CLASS_CLERIC):
               {
                  dump_cleric_subs(d);
                  write_to_buffer
                  (
                     d,
                     "For a short while, you may also \"wait\" to choose a"
                     " subclass.\n\r",
                     0
                  );
                  d->connected = CON_GET_SUBCLASS;
                  return;
                  break;
               }
               default:
               {
                  break;
               }
            }
         }
         /* end new subclass structure - Mael */

         if (IS_IMMORTAL(ch))
         {
            do_help(ch, "imotd");
            d->connected = CON_READ_IMOTD;
         }
         else
         {
            do_help(ch, "motd");
            d->connected = CON_READ_MOTD;
         }
         break;
      }

      /* RT code for breaking link */
      case (CON_BREAK_CONNECT):
      {
         switch(UPPER(*argument))
         {
            case ('Y'):
            {
               for (d_old = descriptor_list; d_old != NULL; d_old = d_next)
               {
                  d_next = d_old->next;
                  if (d_old == d || d_old->character == NULL)
                  {
                     continue;
                  }

                  if
                  (
                     str_cmp
                     (
                        ch->name,
                        d_old->original ?
                        d_old->original->name :
                        d_old->character->name
                     )
                  )
                  {
                     continue;
                  }
                  close_socket(d_old);
               }
               if (check_reconnect(d, ch->name, TRUE, &old))
               {
                  return;
               }
               write_to_buffer(d, "Reconnect attempt failed.\n\rName: ", 0);
               if (d->character != NULL)
               {
                  die_follower(d->character);
                  free_char(d->character);
                  d->character = NULL;
               }
               d->connected = CON_GET_NAME;
               break;
            }

            case ('N'):
            {
               write_to_buffer(d, "Name: ", 0);
               if (d->character != NULL)
               {
                  die_follower(d->character);
                  free_char(d->character);
                  d->character = NULL;
               }
               d->connected = CON_GET_NAME;
               break;
            }

            default:
            {
               write_to_buffer(d, "Please type Y or N? ", 0);
               break;
            }
         }
         break;
      }

      case (CON_CONFIRM_NEW_NAME):
      {
         switch (UPPER(*argument))
         {
            case 'Y':
            {
               sprintf
               (
                  buf,
                  "New character.\n\r\n\r\n\rPlease pick a surname:\n\r"
                  "(Just return for no surname)"
               );
               write_to_buffer(d, buf, 0);
               d->connected = CON_GET_SURNAME;
               break;
            }
            case 'N':
            {
               write_to_buffer(d, "Ok, what IS it, then? ", 0);
               die_follower(d->character);
               free_char(d->character);
               d->character = NULL;
               d->connected = CON_GET_NAME;
               break;
            }
            default:
            {
               write_to_buffer(d, "Please type Yes or No? ", 0);
               break;
            }
         }
         break;
      }

      case CON_GET_NEW_PASSWORD:
      {
#if defined(unix)
         write_to_buffer(d, "\n\r", 2);
#endif

         if (strlen(argument) < 5)
         {
            write_to_buffer
            (
               d,
               "Password must be at least five characters long.\n\rPassword: ",
               0
            );
            return;
         }

         /*
         switched to md5
         pwdnew = crypt(argument, ch->name);
         */
         pwdnew = md5_hash(argument);
         for (p = pwdnew; *p != '\0'; p++)
         {
            if (*p == '~')
            {
               write_to_buffer
               (
                  d,
                  "New password not acceptable, try again.\n\rPassword: ",
                  0
               );
               return;
            }
         }

         free_string(ch->pcdata->pwd);
         ch->pcdata->pwd = str_dup(pwdnew);
         write_to_buffer(d, "Please retype password: ", 0);
         d->connected = CON_CONFIRM_NEW_PASSWORD;
         break;
      }
      case CON_CONFIRM_NEW_PASSWORD:
      {
#if defined(unix)
         write_to_buffer(d, "\n\r", 2);
#endif

         /*
         switched to md5
         if (strcmp(crypt(argument, ch->pcdata->pwd), ch->pcdata->pwd))
         */
         if (strcmp(md5_hash(argument), ch->pcdata->pwd))
         {
            write_to_buffer
            (
               d,
               "Passwords do not match.\n\rRetype password: ",
               0
            );
            d->connected = CON_GET_NEW_PASSWORD;
            return;
         }

         write_to_buffer(d, echo_on_str, 0);
         write_to_buffer(d, "\n\rTime to define your character!\n\r", 0);
         write_to_buffer
         (
            d,
            "\n\rAre your morals generally GOOD, EVIL, or NEUTRAL (G/E/N)? ",
            0
         );
         d->connected = CON_GET_ALIGNMENT;
         break;
      }
      case CON_GET_ALIGNMENT:
      {
         switch(UPPER(argument[0]))
         {
            case ('G'):
            {
               ch->alignment = 1000;
               break;
            }
            case ('N'):
            {
               ch->alignment = 0;
               break;
            }
            case ('E'):
            {
               ch->alignment = -1000;
               break;
            }
            default:
            {
               write_to_buffer
               (
                  d,
                  "That's not a valid alignment.\n\r",
                  0
               );
               write_to_buffer
               (
                  d,
                  "Are your morals generally GOOD, EVIL, or NEUTRAL (G/E/N)? ",
                  0
               );
               return;
            }
         }
         ch->true_alignment = ch->alignment;
         write_to_buffer
         (
            d,
            "\n\rDo you METHODICALLY organize your actions, IMPULSIVELY do"
            " whatever you\n\rfeel, or NEUTRALLY make decisions (M/I/N)? ",
            0
         );
         d->connected = CON_GET_ETHOS;
         break;
      }

      case (CON_GET_ETHOS):
      {
         switch(UPPER(argument[0]))
         {
            case ('M'):
            {
               ch->pcdata->ethos = 1000;
               break;
            }
            case ('N'):
            {
               ch->pcdata->ethos = 0;
               break;
            }
            case ('I'):
            {
               ch->pcdata->ethos = -1000;
               break;
            }
            default:
            {
               write_to_buffer(d, "That's not a valid ethos.\n\r", 0);
               write_to_buffer
               (
                  d,
                  "Do you METHODICALLY organize your actions,"
                  " IMPULSIVELY do whatever you\n\rfeel, or"
                  " NEUTRALLY make decisions (M/I/N)? ",
                  0
               );
               return;
            }
         }
         write_to_buffer
         (
            d,
            "\n\rWhat race are you?\n\rExtra experience needed per level:"
            "\n\r------------------------------------------\n\r",
            0
         );
         for (race = 1; race < MAX_PC_RACE; race++)
         {
            if
            (
               race_table[race].pc_race &&
               (
                  (
                     ch->alignment > 0 &&
                     pc_race_table[race].align != 4 &&
                     pc_race_table[race].align != 5 &&
                     pc_race_table[race].align != 2
                  ) ||
                  (
                     ch->alignment == 0 &&
                     pc_race_table[race].align != 3 &&
                     pc_race_table[race].align != 5 &&
                     pc_race_table[race].align != 6
                  ) ||
                  (
                     ch->alignment < 0 &&
                     pc_race_table[race].align != 3 &&
                     pc_race_table[race].align != 4 &&
                     pc_race_table[race].align != 1
                  )
               )
            )
            {
               sprintf
               (
                  buf,
                  "%-13s costs %3d experience points.\n\r",
                  pc_race_table[race].name,
                  pc_race_table[race].xpadd
               );
               write_to_buffer(d, buf, 0);
            }
         }
         write_to_buffer
         (
            d,
            "------------------------------------------\n\r\n\r",
            0
         );
         write_to_buffer(d, "Races: [ ", 0);
         for
         (
            race = 1;
            (
               race_table[race].name != NULL &&
               race < MAX_PC_RACE
            );
            race++
         )
         {
            if
            (
               race_table[race].pc_race &&
               (
                  (
                     ch->alignment > 0 &&
                     pc_race_table[race].align != 4 &&
                     pc_race_table[race].align != 5 &&
                     pc_race_table[race].align != 2
                  ) ||
                  (
                     ch->alignment == 0 &&
                     pc_race_table[race].align != 3 &&
                     pc_race_table[race].align != 5 &&
                     pc_race_table[race].align != 6
                  ) ||
                  (
                     ch->alignment < 0 &&
                     pc_race_table[race].align != 3 &&
                     pc_race_table[race].align != 4 &&
                     pc_race_table[race].align != 1
                  )
               )
            )
            {
               write_to_buffer(d, race_table[race].name, 0);
               write_to_buffer(d, " ", 1);
            }
         }
         write_to_buffer(d, "]\n\r\n\r", 0);
         write_to_buffer
         (
            d,
            "What is your race (HELP for more information, LIST for races)? ",
            0
         );
         d->connected = CON_GET_NEW_RACE;
         break;
      }

      case CON_GET_NEW_RACE:
      {
         one_argument(argument, arg);

         if (!strcmp(arg, "help"))
         {
            argument = one_argument(argument, arg);
            if (argument[0] == '\0')
            {
               do_help(ch, "race");
            }
            else
            {
               do_help(ch, argument);
            }
            write_to_buffer
            (
               d,
               "\n\rWhat is your race (HELP for more information, LIST for"
               " races)? ",
               0
            );
            break;
         }
         if (!strcmp(arg, "list"))
         {
            write_to_buffer
            (
               d,
               "\n\rWhat race are you?\n\rExtra experience needed per level"
               ":\n\r------------------------------------------\n\r",
               0
            );
            for
            (
               race = 1;
               (
                  race_table[race].name != NULL &&
                  race < MAX_PC_RACE
               );
               race++
            )
            {
               if
               (
                  race_table[race].pc_race &&
                  (
                     (
                        ch->alignment > 0 &&
                        pc_race_table[race].align != 4 &&
                        pc_race_table[race].align != 5 &&
                        pc_race_table[race].align != 2
                     ) ||
                     (
                        ch->alignment == 0 &&
                        pc_race_table[race].align != 3 &&
                        pc_race_table[race].align != 5 &&
                        pc_race_table[race].align != 6
                     ) ||
                     (
                        ch->alignment < 0 &&
                        pc_race_table[race].align != 3 &&
                        pc_race_table[race].align != 4 &&
                        pc_race_table[race].align != 1
                     )
                  )
               )
               {
                  sprintf
                  (
                     buf,
                     "%-13s costs %3d experience points.\n\r",
                     pc_race_table[race].name,
                     pc_race_table[race].xpadd
                  );
                  write_to_buffer(d, buf, 0);
               }
            }
            write_to_buffer
            (
               d,
               "------------------------------------------\n\r\n\r",
               0
            );
            write_to_buffer(d, "Races: [ ", 0);
            for
            (
               race = 1;
               (
                  race_table[race].name != NULL &&
                  race < MAX_PC_RACE
               );
               race++
            )
            {
               if
               (
                  race_table[race].pc_race &&
                  (
                     (
                        ch->alignment > 0 &&
                        pc_race_table[race].align != 4 &&
                        pc_race_table[race].align != 5 &&
                        pc_race_table[race].align != 2
                     ) ||
                     (
                        ch->alignment == 0 &&
                        pc_race_table[race].align != 3 &&
                        pc_race_table[race].align != 5 &&
                        pc_race_table[race].align != 6
                     ) ||
                     (
                        ch->alignment < 0 &&
                        pc_race_table[race].align != 3 &&
                        pc_race_table[race].align != 4 &&
                        pc_race_table[race].align != 1
                     )
                  )
               )
               {
                  write_to_buffer(d, race_table[race].name, 0);
                  write_to_buffer(d, " ", 1);
               }
            }
            write_to_buffer(d, "]\n\r\n\r", 0);
            write_to_buffer
            (
               d,
               "What is your race (HELP for more information, LIST for"
               " races)? ",
               0
            );
            break;
         }

         race = race_lookup(argument);

         if
         (
            !race_table[race].pc_race ||
            !(
               (
                  ch->alignment > 0 &&
                  pc_race_table[race].align != 4 &&
                  pc_race_table[race].align != 5 &&
                  pc_race_table[race].align != 2
               ) ||
               (
                  ch->alignment == 0 &&
                  pc_race_table[race].align != 3 &&
                  pc_race_table[race].align != 5 &&
                  pc_race_table[race].align != 6
               ) ||
               (
                  ch->alignment < 0 &&
                  pc_race_table[race].align != 3 &&
                  pc_race_table[race].align != 4 &&
                  pc_race_table[race].align != 1
               )
            )
         )
         {
            write_to_buffer(d, "That is not a race.\n\r", 0);
            write_to_buffer(d, "Races: [ ", 0);
            for
            (
               race = 1;
               (
                  race_table[race].name != NULL &&
                  race < MAX_PC_RACE
               );
               race++
            )
            {
               if
               (
                  race_table[race].pc_race &&
                  (
                     (
                        ch->alignment > 0 &&
                        pc_race_table[race].align != 4 &&
                        pc_race_table[race].align != 5 &&
                        pc_race_table[race].align != 2
                     ) ||
                     (
                        ch->alignment == 0 &&
                        pc_race_table[race].align != 3 &&
                        pc_race_table[race].align != 5 &&
                        pc_race_table[race].align != 6
                     ) ||
                     (
                        ch->alignment < 0 &&
                        pc_race_table[race].align != 3 &&
                        pc_race_table[race].align != 4 &&
                        pc_race_table[race].align != 1
                     )
                  )
               )
               {
                  write_to_buffer(d, race_table[race].name, 0);
                  write_to_buffer(d, " ", 1);
               }
            }
            write_to_buffer(d, "]\n\r\n\r", 0);
            write_to_buffer
            (
               d,
               "What is your race (HELP for more information, LIST for"
               " races)? ",
               0
            );
            break;
         }

         ch->race = race;
         ch->pcdata->race_lottery = race;
         /* initialize stats */

         for (i = 0; i < MAX_STATS; i++)
         {
            ch->perm_stat[i] = pc_race_table[race].stats[i];
         }
         ch->race = race;
         ch->affected_by = ch->affected_by | race_table[race].aff;
         ch->imm_flags    = ch->imm_flags | race_table[race].imm;
         ch->res_flags    = ch->res_flags | race_table[race].res;
         ch->vuln_flags    = ch->vuln_flags | race_table[race].vuln;
         ch->form = race_table[race].form;
         ch->parts = race_table[race].parts;

         ch->size = pc_race_table[race].size;

         if (ch->race == grn_draconian)
         {
            d->connected = CON_PICK_SCALE_COLOR;
            write_to_buffer(d, "\n\rWhat color are your scales? ", 0);
            if (ch->alignment == 1000)
            {
               write_to_buffer(d, "[Gold, Silver, Bronze, Brass, Copper]\n\r", 0);
            }
            else
            {
               write_to_buffer(d, "[Green, Blue, Red, Black, White]\n\r", 0);
            }
            break;
         }
         if (ch->race == grn_human)
         {
            d->connected = CON_PICK_HUMAN_STATS;
            write_to_buffer
            (
               d,
               "\n\rDistribute 3 points between the five stats."
               "\n\rBonus distributed:\n\rStr: 0 Int: 0 Wis: 0 Dex: 0 Con: 0\n\r"
               "[str, int, wis, dex, con]?",
               0
            );
            break;
         }
         /* illithids are sexless - runge */
         d->connected = CON_GET_NEW_SEX;
         if (ch->race != grn_illithid)
         {
            write_to_buffer( d, "\n\rAre you MALE or FEMALE (M/F)? ", 0 );
         }
         else
         {
            nanny(d, "");
         }
         break;
      }

      case (CON_PICK_HUMAN_STATS):
      {
         sh_int total = 0;

         for (i = 0; i < 5; i++)
         {
            total += ch->pcdata->stat_bonus[i];
         }
         one_argument(argument, arg);
         total++;
         buf[0] = '\0';
         if (!str_prefix("str", arg))
         {
            ch->pcdata->stat_bonus[STAT_STR]++;
         }
         else if (!str_prefix("int", arg))
         {
            ch->pcdata->stat_bonus[STAT_INT]++;
         }
         else if (!str_prefix("wis", arg))
         {
            ch->pcdata->stat_bonus[STAT_WIS]++;
         }
         else if (!str_prefix("dex", arg))
         {
            ch->pcdata->stat_bonus[STAT_DEX]++;
         }
         else if (!str_prefix("con", arg))
         {
            ch->pcdata->stat_bonus[STAT_CON]++;
         }
         else
         {
            total--;
            if (arg[0] != '\0')
            {
               strcat(buf, "\n\rThat is not a valid stat.");
            }
         }
         sprintf
         (
            buf,
            "%s\n\rBonus distributed: Str: %d Int: %d Wis: %d Dex: %d Con: %d",
            buf,
            ch->pcdata->stat_bonus[STAT_STR],
            ch->pcdata->stat_bonus[STAT_INT],
            ch->pcdata->stat_bonus[STAT_WIS],
            ch->pcdata->stat_bonus[STAT_DEX],
            ch->pcdata->stat_bonus[STAT_CON]
         );
         if (total < 3)
         {
            sprintf
            (
               buf,
               "%s\n\rYou have %d points left to distribute.\n\r"
               "[str, int, wis, dex, con]? ",
               buf,
               3 - total
            );
         }
         else
         {
            if (ch->level > 0)  /* Old chars being upgraded */
            {
               skill_adjust(ch, TRUE);
               d->connected = CON_READ_MOTD;
               write_to_buffer(d, buf, 0);
               nanny(d, "");
               return;
            }
            else
            {
               strcat(buf, "\n\r\n\rAre you MALE or FEMALE (M/F)? ");
               d->connected = CON_GET_NEW_SEX;
            }
         }
         write_to_buffer(d, buf, 0);
         break;
      }
      case CON_PICK_SCALE_COLOR:
      {
         if (ch->alignment == 1000)
         {
            if (!str_cmp(argument, "gold"))
            ch->subrace = SCALE_GOLD;
            else if (!str_cmp(argument, "silver"))
            ch->subrace = SCALE_SILVER;
            else if (!str_cmp(argument, "bronze"))
            ch->subrace = SCALE_BRONZE;
            else if (!str_cmp(argument, "copper"))
            ch->subrace = SCALE_COPPER;
            else if (!str_cmp(argument, "brass"))
            ch->subrace = SCALE_BRASS;
            else
            {
               write_to_buffer(d, "That's not a valid scale color.\n\r", 0);
               write_to_buffer(d, "What color are your scales? ", 0);
               if (ch->alignment == 1000)
               {
                  write_to_buffer(d, "[Gold, Silver, Bronze, Brass, Copper]\n\r", 0);
               }
               else
               {
                  write_to_buffer(d, "[Green, Blue, Red, Black, White]\n\r", 0);
               }
               return;
            }
         }
         else
         {
            if (!str_cmp(argument, "green"))
            ch->subrace = SCALE_GREEN;
            else if (!str_cmp(argument, "red"))
            ch->subrace = SCALE_RED;
            else if (!str_cmp(argument, "black"))
            ch->subrace = SCALE_BLACK;
            else if (!str_cmp(argument, "white"))
            ch->subrace = SCALE_WHITE;
            else if (!str_cmp(argument, "blue"))
            ch->subrace = SCALE_BLUE;
            else
            {
               write_to_buffer(d, "That's not a valid scale color.\n\r", 0);
               write_to_buffer(d, "What color are your scales? ", 0);
               if (ch->alignment == 1000)
               {
                  write_to_buffer(d, "[Gold, Silver, Bronze, Brass, Copper]\n\r", 0);
               }
               else
               {
                  write_to_buffer(d, "[Green, Blue, Red, Black, White]\n\r", 0);
               }
               return;
            }

         }
         write_to_buffer( d, "\n\rAre you MALE or FEMALE (M/F)? ", 0 );
         d->connected = CON_GET_NEW_SEX;
         break;
      }

      case CON_GET_NEW_SEX:
      if (ch->race == grn_illithid)
      {
         ch->sex = SEX_NEUTRAL;
         ch->pcdata->true_sex = SEX_NEUTRAL;
      }
      else
      {
         switch ( argument[0] )
         {
            case 'm': case 'M': ch->sex = SEX_MALE;
            ch->pcdata->true_sex = SEX_MALE;
            break;
            case 'f': case 'F': ch->sex = SEX_FEMALE;
            ch->pcdata->true_sex = SEX_FEMALE;
            break;
            default:
            write_to_buffer( d, "That's not a sex.\n\rWhat IS your sex? ", 0 );
            return;
         }
      }

      write_to_buffer
      (
         d,
         "\n\rWhat class are you?\n\rExtra experience needed per level:"
         "\n\r------------------------------------------\n\r",
         0
      );
      for (race = 0; race < MAX_CLASS; race++)
      {
         if
         (
            pc_race_table[ch->race].classes[race] == 1 &&
            (
               (
                  ch->alignment > 0 &&
                  class_table[race].align != 2 &&
                  class_table[race].align != 4 &&
                  class_table[race].align != 5
               ) ||
               (
                  ch->alignment == 0 &&
                  class_table[race].align != 3 &&
                  class_table[race].align != 5
               ) ||
               (
                  ch->alignment < 0 &&
                  class_table[race].align != 1 &&
                  class_table[race].align != 3 &&
                  class_table[race].align != 4
               )
            ) &&
            !(
               (
                  /* Paladin, Monk */
                  ch->pcdata->ethos <= 0 &&
                  (
                     race == 3 ||
                     race == 6
                  )
               ) ||
               (
                  /* anti-Paladin */
                  ch->pcdata->ethos >= 0 &&
                  race == 4
               )
            )
         )
         {
            sprintf(buf, "%-13s costs %3d experience points.\n\r", class_table[race].name, class_table[race].xpadd);
            write_to_buffer(d, buf, 0);
         }
      }
      write_to_buffer(d, "------------------------------------------\n\r\n\r", 0);
      strcpy( buf, "Classes: [ " );
      for (iClass = 0; iClass < MAX_CLASS; iClass++)
      {
         if
         (
            pc_race_table[ch->race].classes[iClass] == 1 &&
            (
               (
                  ch->alignment > 0 &&
                  class_table[iClass].align != 2 &&
                  class_table[iClass].align != 4 &&
                  class_table[iClass].align != 5
               ) ||
               (
                  ch->alignment == 0 &&
                  class_table[iClass].align != 3 &&
                  class_table[iClass].align != 5
               ) ||
               (
                  ch->alignment < 0 &&
                  class_table[iClass].align != 1 &&
                  class_table[iClass].align != 3 &&
                  class_table[iClass].align != 4
               )
            ) &&
            !(
               (
                  ch->pcdata->ethos <= 0 &&
                  (
                     /* Paladin, Monk */
                     iClass == 3 ||
                     iClass == 6
                  )
               ) ||
               (
                  /* anti-Paladin */
                  ch->pcdata->ethos >= 0 &&
                  iClass == 4
               )
            )
         )
         {
            strcat( buf, class_table[iClass].name );
            strcat( buf, " " );
         }
      }
      strcat( buf, "]\n\r\n\r" );
      write_to_buffer( d, buf, 0 );
      write_to_buffer(d, "What is your class (HELP for more information, LIST for classes)? ", 0);
      d->connected = CON_GET_NEW_CLASS;
      break;


      case CON_GET_NEW_CLASS:
      one_argument(argument, arg);

      if (!strcmp(arg, "help"))
      {
         argument = one_argument(argument, arg);
         if (argument[0] == '\0')
         do_help(ch, "class");
         else
         do_help(ch, argument);
         write_to_buffer(d,
         "\n\rWhat is your class (HELP for more information, LIST for classes)? ", 0);
         break;
      }
      if (!strcmp(arg, "list"))
      {
         write_to_buffer( d, "\n\rWhat class are you?\n\rExtra experience needed per level:\n\r------------------------------------------\n\r", 0 );
         for ( race = 0; race < MAX_CLASS; race++ )
         {
            if
            (
               pc_race_table[ch->race].classes[race] == 1 &&
               (
                  (
                     ch->alignment > 0 &&
                     class_table[race].align != 2 &&
                     class_table[race].align != 4 &&
                     class_table[race].align != 5
                  ) ||
                  (
                     ch->alignment == 0 &&
                     class_table[race].align != 3 &&
                     class_table[race].align != 5
                  ) ||
                  (
                     ch->alignment < 0 &&
                     class_table[race].align != 1 &&
                     class_table[race].align != 3 &&
                     class_table[race].align != 4
                  )
               ) &&
               !(
                  (
                     /* Paladin, Monk */
                     ch->pcdata->ethos <= 0 &&
                     (
                        race == 3 ||
                        race == 6
                     )
                  ) ||
                  (
                     /* anti-Paladin */
                     ch->pcdata->ethos >= 0 &&
                     race == 4
                  )
               )
            )
            {
               sprintf(buf, "%-13s costs %3d experience points.\n\r", class_table[race].name, class_table[race].xpadd);
               write_to_buffer(d, buf, 0);
            }
         }
         write_to_buffer(d, "------------------------------------------\n\r\n\r", 0);
         strcpy( buf, "Classes: [ " );
         for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
         {
            if
            (
               pc_race_table[ch->race].classes[iClass] == 1 &&
               (
                  (
                     ch->alignment > 0 &&
                     class_table[iClass].align != 2 &&
                     class_table[iClass].align != 4 &&
                     class_table[iClass].align != 5
                  ) ||
                  (
                     ch->alignment == 0 &&
                     class_table[iClass].align != 3 &&
                     class_table[iClass].align != 5
                  ) ||
                  (
                     ch->alignment < 0 &&
                     class_table[iClass].align != 1 &&
                     class_table[iClass].align != 3 &&
                     class_table[iClass].align != 4
                  )
               ) &&
               !(
                  (
                     ch->pcdata->ethos <= 0 &&
                     (
                        /* Paladin, Monk */
                        iClass == 3 ||
                        iClass == 6
                     )
                  ) ||
                  (
                     /* anti-Paladin */
                     ch->pcdata->ethos >= 0 &&
                     iClass == 4
                  )
               )
            )
            {
               strcat( buf, class_table[iClass].name );
               strcat( buf, " " );
            }
         }
         strcat( buf, "]\n\r\n\r" );
         write_to_buffer( d, buf, 0 );
         write_to_buffer(d, "What is your class (HELP for more information, LIST for classes)? ", 0);
         break;
      }

      iClass = class_lookup(argument);

      if ( iClass == -1 )
      {
         write_to_buffer( d,
         "That's not a class.\n\r", 0 );
         strcpy( buf, "Classes: [ " );
         for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
         {
            if
            (
               pc_race_table[ch->race].classes[iClass] == 1 &&
               (
                  (
                     ch->alignment > 0 &&
                     class_table[iClass].align != 2 &&
                     class_table[iClass].align != 4 &&
                     class_table[iClass].align != 5
                  ) ||
                  (
                     ch->alignment == 0 &&
                     class_table[iClass].align != 3 &&
                     class_table[iClass].align != 5
                  ) ||
                  (
                     ch->alignment < 0 &&
                     class_table[iClass].align != 1 &&
                     class_table[iClass].align != 3 &&
                     class_table[iClass].align != 4
                  )
               ) &&
               !(
                  (
                     ch->pcdata->ethos <= 0 &&
                     (
                        /* Paladin, Monk */
                        iClass == 3 ||
                        iClass == 6
                     )
                  ) ||
                  (
                     /* anti-Paladin */
                     ch->pcdata->ethos >= 0 &&
                     iClass == 4
                  )
               )
            )
            {
               strcat( buf, class_table[iClass].name );
               strcat( buf, " " );
            }
         }
         strcat( buf, "]\n\r\n\r" );
         write_to_buffer( d, buf, 0 );
         write_to_buffer(d, "What is your class (HELP for more information, LIST for classes)? ", 0);
         return;
      }

      if
      (
         !(
            pc_race_table[ch->race].classes[iClass] == 1 &&
            (
               (
                  ch->alignment > 0 &&
                  class_table[iClass].align != 2 &&
                  class_table[iClass].align != 4 &&
                  class_table[iClass].align != 5
               ) ||
               (
                  ch->alignment == 0 &&
                  class_table[iClass].align != 3 &&
                  class_table[iClass].align != 5
               ) ||
               (
                  ch->alignment < 0 &&
                  class_table[iClass].align != 1 &&
                  class_table[iClass].align != 3 &&
                  class_table[iClass].align != 4
               )
            ) &&
            !(
               (
                  ch->pcdata->ethos <= 0 &&
                  (
                     /* Paladin, Monk */
                     iClass == 3 ||
                     iClass == 6
                  )
               ) ||
               (
                  /* anti-Paladin */
                  ch->pcdata->ethos >= 0 &&
                  iClass == 4
               )
            )
         )
      )
      {
         write_to_buffer( d,
         "That's not a class.\n\r", 0 );
         strcpy( buf, "Classes: [ " );
         for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
         {
            if
            (
               pc_race_table[ch->race].classes[iClass] == 1 &&
               (
                  (
                     ch->alignment > 0 &&
                     class_table[iClass].align != 2 &&
                     class_table[iClass].align != 4 &&
                     class_table[iClass].align != 5
                  ) ||
                  (
                     ch->alignment == 0 &&
                     class_table[iClass].align != 3 &&
                     class_table[iClass].align != 5
                  ) ||
                  (
                     ch->alignment < 0 &&
                     class_table[iClass].align != 1 &&
                     class_table[iClass].align != 3 &&
                     class_table[iClass].align != 4
                  )
               ) &&
               !(
                  (
                     ch->pcdata->ethos <= 0 &&
                     (
                        /* Paladin, Monk */
                        iClass == 3 ||
                        iClass == 6
                     )
                  ) ||
                  (
                     /* anti-Paladin */
                     ch->pcdata->ethos >= 0 &&
                     iClass == 4
                  )
               )
            )
            {
               strcat( buf, class_table[iClass].name );
               strcat( buf, " " );
            }
         }
         strcat( buf, "]\n\r\n\r" );
         write_to_buffer( d, buf, 0 );
         write_to_buffer(d, "What is your class (HELP for more information, LIST for classes)? ", 0);
         return;
      }

      ch->class = iClass;
      /* Does 0 on purpose (set up reserved learnlvl) */
      for (sn = 0; sn < MAX_SKILL; sn++)
      {
         ch->pcdata->learnlvl[sn] = skill_table[sn].skill_level[ch->class];
      }

      sprintf(log_buf,  "%s@%s new player.", ch->name, d->host );
      log_string( log_buf );
      wiznet("Newbie alert!  $N sighted.", ch, NULL, WIZ_NEWBIE, 0, 0);
      wiznet(log_buf, NULL, NULL, WIZ_SITES, 0, get_trust(ch));

      /* new subclass structure - Mael */

      switch( ch->class )
      {
         case CLASS_MONK:
         dump_monk_subs( d );
         d->connected = CON_GET_SUBCLASS;
         return;
         case CLASS_ANTI_PALADIN:
         dump_ap_subs( d );
         d->connected = CON_GET_SUBCLASS;
         return;
         case CLASS_NECROMANCER:
         dump_necro_subs( d );
         d->connected = CON_GET_SUBCLASS;
         return;
         break;
         case CLASS_RANGER:
         dump_rang_subs(d);
         d->connected = CON_GET_SUBCLASS;
         return;
         break;
         case CLASS_ELEMENTALIST:
         if (allow_elem)
         {
            dump_elem_subs(d);
            d->connected = CON_GET_SUBCLASS;
            return;
         }
         break;
         case CLASS_CHANNELER:
         dump_chan_subs(d);
         d->connected = CON_GET_SUBCLASS;
         return;
         break;

         case CLASS_CLERIC:
         dump_cleric_subs(d);
         d->connected = CON_GET_SUBCLASS;
         return;
         break;
         case CLASS_THIEF:
         if (allow_thief)
         {
            dump_thief_subs(d);
            d->connected = CON_GET_SUBCLASS;
            return;
         }
         break;
         default:
         ;
      }
      racial_skills(ch);
      initialize_stats(ch, d);
      d->connected = CON_LETS_ROLL_STATS;
      break;

      case CON_GET_SUBCLASS:
      mainclass = ch->class;

      one_argument(argument, arg);

      if (!strcmp(arg, "help"))
      {
         argument = one_argument(argument, arg);
         if (argument[0] == '\0')
         switch( mainclass )
         {
            case CLASS_MONK:
            do_help(ch, "school");
            break;
            case CLASS_ANTI_PALADIN:
            ap_subs_help( ch );
            break;
            case CLASS_NECROMANCER:
            necro_subs_help( ch );
            break;
            case CLASS_CLERIC:
            cleric_subs_help(ch);
            break;
            case CLASS_THIEF:
            thief_subs_help(ch);
            break;
            case CLASS_CHANNELER:
            chan_subs_help(ch);
            break;
            case CLASS_RANGER:
            rang_subs_help(ch);
            break;
            case CLASS_ELEMENTALIST:
            elem_subs_help(ch);
            break;
            default:
            ;
         }
         else
         do_help(ch, argument);

         switch( mainclass )
         {
            case CLASS_MONK:
            write_to_buffer(d, "\n\rWhich school do you aspire to join (HELP for information, LIST for schools)? ", 0);
            break;
            default:
            write_to_buffer(d, "\n\rWhich subclass do you wish to join (HELP for information, LIST for subclasses)? ", 0);
            break;
         }
         break;
      }

      if (!strcmp(arg, "list"))
      {
         switch( mainclass )
         {
            case CLASS_MONK:
            dump_monk_subs( d );
            break;
            case CLASS_ANTI_PALADIN:
            dump_ap_subs( d );
            break;
            case CLASS_NECROMANCER:
            dump_necro_subs( d );
            break;
            case CLASS_RANGER:
            dump_rang_subs( d );
            write_to_buffer(d, "For a short while, you may also \"wait\" to choose a subclass.\n\r", 0);
            break;
            case CLASS_ELEMENTALIST:
            dump_elem_subs( d );
            write_to_buffer(d, "For a short while, you may also \"wait\" to choose a subclass.\n\r", 0);
            break;
            case CLASS_CHANNELER:
            dump_chan_subs( d );
            write_to_buffer(d, "For a short while, you may also \"wait\" to choose a subclass.\n\r", 0);
            break;
            case CLASS_CLERIC:
            dump_cleric_subs( d );
            write_to_buffer(d, "For a short while, you may also \"wait\" to choose a subclass.\n\r", 0);
            break;
            case CLASS_THIEF:
            dump_thief_subs( d );
            write_to_buffer(d, "For a short while, you may also \"wait\" to choose a subclass.\n\r", 0);
            break;
            default:
            ;
         }
         break;
      }

      subclass = find_subclass( ch, argument );
      if (!can_subclass(ch, subclass, TRUE))
      {
         write_to_buffer(d, "\n\r\n\rWhich school do you aspire to join (HELP for information, LIST for schools)? ", 0);
         return;
      }
      if ( mainclass == CLASS_MONK )
      {
         switch( subclass )
         {
            case SUBCLASS_WAITING:
            case SUBCLASS_PICKED:
            write_to_buffer(d, "You must choose a school.\n\rSchools: [ Tiger, Snake, Dragon, Praying Mantis, or Crane ]", 0);
            write_to_buffer(d, "\n\r\n\rWhich school do you aspire to join (HELP for information, LIST for schools)? ", 0);
            return;
            case -5:
            write_to_buffer(d, "That is not an acceptable school.\n\rSchools: [ Tiger, Snake, Dragon, Praying Mantis, or Crane ]", 0);
            write_to_buffer(d, "\n\r\n\rWhich school do you aspire to join (HELP for information, LIST for schools)? ", 0);
            return;
            default:
            make_char_monksub(ch, subclass);
         }
      }
      else
      {
         switch( subclass )
         {
            case SUBCLASS_WAITING:
            if ( ch->level > 0 )
            {
               write_to_buffer(d, "You will be asked to choose again the next time you log in.\n\r", 0);
               ch->pcdata->special = SUBCLASS_NONE;
               break;
            }
            else
            {
               write_to_buffer(d, "New characters may not choose this option.\n\r", 0);
               write_to_buffer(d, "\n\r\n\rWhich subclass do you wish to join (HELP for information, LIST for subclasses)? ", 0);
               return;
            }
            case SUBCLASS_PICKED:
            write_to_buffer(d, "You will have no subclass, then.\n\r", 0);
            ch->pcdata->special = subclass;
            break;
            case -5:
            write_to_buffer(d, "That is not an acceptable subclass.\n\r", 0);
            write_to_buffer(d, "\n\r\n\rWhich subclass do you wish to join (HELP for information, LIST for subclasses)? ", 0);
            return;
            default:
            make_char_subclass(ch, NULL, subclass);
         }
      }
      racial_skills(ch);
      if ( ch->level > 0 )
      {
         if ( IS_IMMORTAL(ch) )
         {
            do_help( ch, "imotd" );
            d->connected = CON_READ_IMOTD;
         }
         else
         {
            do_help( ch, "motd" );
            d->connected = CON_READ_MOTD;
         }
         break;
      }

      initialize_stats( ch, d );
      d->connected = CON_LETS_ROLL_STATS;
      break;

      /* end new subclass structure - Mael */

      case CON_LETS_ROLL_STATS:
      switch ( argument[0] )
      {
         case 'y': case 'Y':
         {
            ch->mod_stat[0]=0;
            ch->mod_stat[1]=0;
            ch->mod_stat[2]=0;
            ch->mod_stat[3]=0;
            ch->mod_stat[4]=0;
            break;
         }
         case 'n': case 'N':
         {
            ch->perm_stat[0] = number_range(12, pc_race_table[ch->race].stats[0]);
            ch->perm_stat[1] = number_range(12, pc_race_table[ch->race].stats[1]);
            ch->perm_stat[2] = number_range(12, pc_race_table[ch->race].stats[2]);
            ch->perm_stat[3] = number_range(12, pc_race_table[ch->race].stats[3]);
            ch->perm_stat[4] = number_range(12, pc_race_table[ch->race].stats[4]);
            for ( i = 0; i < 5; i++ )
            {
               if ( ch->perm_stat[i] > get_max_train(ch, i) )
               {
                  ch->perm_stat[i] = get_max_train(ch, i);
               }
            }
            sprintf(buf, "\n\rStrength: %d Intelligence: %d Wisdom: %d Dexterity: %d Constitution: %d\n\rDo these reflect your training (Y/N)? ",
            ch->perm_stat[0],
            ch->perm_stat[1],
            ch->perm_stat[2],
            ch->perm_stat[3],
            ch->perm_stat[4]);
            write_to_buffer(d, buf, 0);
            return;
         }
         default:
         {
            sprintf(buf, "Please type Y or N...\n\r\n\rStrength: %d Intelligence: %d Wisdom: %d Dexterity: %d Constitution: %d\n\rDo these reflect your training (Y/N)? ",
            ch->perm_stat[0],
            ch->perm_stat[1],
            ch->perm_stat[2],
            ch->perm_stat[3],
            ch->perm_stat[4]);
            write_to_buffer(d, buf, 0);
            return;
         }
      }

      write_to_buffer(d, "\n\r", 0);
      group_add(ch, "class basics", FALSE);
      SET_BIT(ch->comm, COMM_SHOW_AFFECTS);
      SET_BIT(ch->act, PLR_AUTOEXIT);

      write_to_buffer(d, "Where are you from? In which town lies the temple that\n\r", 0);
      write_to_buffer(d, "you recall to, or go when you die? Glyndane and Elvenhame\n\r", 0);
      write_to_buffer(d, "are for all alignments. Ofcol is for good only, Tyr-Zinet\n\r", 0);
      write_to_buffer(d, "is for neutral only, and Arkham is for evil only. Most new\n\r", 0);
      write_to_buffer(d, "adventurers are from Glyndane since it is most central.\n\r", 0);
      write_to_buffer(d, "All others are better suited for more experienced people.\n\r\n\r", 0);

      {
         int hometown = 0;
         int align = ch->alignment;
         while(hometown_table[hometown].vnum != 0)
         {
            if (hometown_table[hometown].vnum != ch->temple)
            if ((align == 1000 && hometown_table[hometown].allow_good) ||
            (align == 0    && hometown_table[hometown].allow_neutral) ||
            (align == -1000 && hometown_table[hometown].allow_evil))
            {
               sprintf(buf, "%-20s (%-4s)\n\r", hometown_table[hometown].name,
               hometown_table[hometown].abr_name);
               send_to_char(buf, ch);
            }
            hometown++;
         }
      }
      send_to_char("\n\rWhat would you like your hometown to be? ", ch);
      d->connected = CON_DEFAULT_CHOICE;
      break;

      case CON_DEFAULT_CHOICE:

      align = ch->alignment;

      hometown = 0;
      ht_choice = -1;
      while(hometown_table[hometown].vnum != 0)
      {
         if ( hometown_table[hometown].allow_creation)
         {
            if ((align == 1000 && hometown_table[hometown].allow_good) ||
            (align == 0    && hometown_table[hometown].allow_neutral) ||
            (align == -1000 && hometown_table[hometown].allow_evil))
            {
               if (!str_cmp(hometown_table[hometown].name, argument) ||
               !str_cmp(hometown_table[hometown].abr_name, argument))
               ht_choice = hometown;
            }
         }
         hometown++;
      }
      if (ht_choice == -1)
      {
         send_to_char("\n\rThat's not a hometown choice for you.\n\r\n\r", ch);
         send_to_char("Hometowns available to you:\n\r\n\r", ch);
         hometown = 0;
         while(hometown_table[hometown].vnum != 0)
         {
            if (hometown_table[hometown].vnum != ch->temple)
            if ((align == 1000 && hometown_table[hometown].allow_good) ||
            (align == 0    && hometown_table[hometown].allow_neutral) ||
            (align == -1000 && hometown_table[hometown].allow_evil))
            {
               sprintf(buf, "%-20s (%-4s)\n\r", hometown_table[hometown].name,
               hometown_table[hometown].abr_name);
               send_to_char(buf, ch);
            }
            hometown++;
         }
         send_to_char("\n\rWhat would you like your hometown to be? ", ch);
         return;
      }
      ch->temple = hometown_table[ht_choice].vnum;

      write_to_buffer(d, "\n\r\n\r", 2);
      do_help( ch, "motd" );
      d->connected = CON_READ_MOTD;
      break;


      case CON_READ_IMOTD:
      write_to_buffer(d, "\n\r\n\r", 2);
      do_help( ch, "motd" );
      d->connected = CON_READ_MOTD;
      break;


      case CON_READ_MOTD:
      if ( ch->pcdata == NULL || ch->pcdata->pwd[0] == '\0')
      {
         write_to_buffer( d, "Warning! Null password!\n\r", 0 );
         write_to_buffer( d, "Please report old password with bug.\n\r", 0);
         write_to_buffer( d,
         "Type 'password null <new-password> <new-password verification>' to fix.\n\r", 0);
      }
      if (ch->pcdata->surname == NULL)
      {
         d->connected = CON_GET_SURNAME;
         write_to_buffer(d, "Please pick a surname:", 0);
         return;
      }
      /* Prevent double names for new characters */
      if (check_playing_newbie(d))
      {
         write_to_buffer
         (
            d,
            "That character already exists.\n\r",
            0
         );
         close_socket(d);
         return;
      }

      if
      (
         ch->race == grn_human &&
         ch->version <= 18 &&
         ch->level > 0
      )
      {
         d->connected = CON_PICK_HUMAN_STATS;
         write_to_buffer
         (
            d,
            "\n\rDistribute 3 points between the five stats."
            "\n\rBonus distributed:\n\rStr: 0 Int: 0 Wis: 0 Dex: 0 Con: 0\n\r"
            "[str, int, wis, dex, con]?",
            0
         );
         break;
      }


      write_to_buffer( d, "\n\r\n\rWelcome to the MUD.  Please do not feed the mobiles.\n\r\n\r",
      0 );
      ch->next    = char_list;
      char_list    = ch;
      d->connected    = CON_PLAYING;
      reset_char(ch);

      /* store current room */
      room = ch->in_room;
      /* Prevent list errors, not actually in the room yet */
      ch->in_room = NULL;

      if ( ch->level == 0 )
      {
         ch->level   = 1;
         ch->exp     = 0;
         ch->hit    = ch->max_hit;
         ch->mana    = ch->max_mana;
         ch->move    = ch->max_move;
         ch->train     = 3;
         ch->practice = 5;
         sprintf( buf, "the %s",
         title_table [ch->class] [ch->level]
         [ch->sex == SEX_FEMALE ? 1 : 0] );
         set_title( ch, buf );

         do_outfit(ch, "");

         /*
         obj_to_char(create_object(get_obj_index(OBJ_VNUM_MAP), 0), ch);
         */
         room = get_room_index(ROOM_VNUM_SCHOOL);
         send_to_char("\n\r", ch);
         do_help(ch, "NEWBIE INFO");
         send_to_char("\n\r", ch);
      }
      if (room == NULL)
      {
         if ( IS_IMMORTAL(ch) )
         {
            room = get_room_index(ROOM_VNUM_CHAT);
         }
         else
         {
            room = get_room_index(ROOM_VNUM_TEMPLE);
         }
      }

      group_add(ch, "class basics", FALSE);

      if (ch->pcdata->xp_pen == 0)
      {
         if (ch->level == 1)
         {
            free_string(ch->pcdata->first_site);
            ch->pcdata->first_site = str_dup(d->host);
            ch->pcdata->xp_pen =  pc_race_table[ch->race].xpadd +
            class_table[ch->class].xpadd+1500;
            if (ch->class == CLASS_CLERIC
            && ch->pcdata->special > 0)
            ch->pcdata->xp_pen += 250;
            if (ch->class == CLASS_RANGER
            && ch->pcdata->special > 0)
            ch->pcdata->xp_pen += 50;
            if (ch->class == CLASS_THIEF
            && ch->pcdata->special == SUBCLASS_SWASHBUCKLER)
            ch->pcdata->xp_pen += 250;
            if (ch->class == CLASS_THIEF
            && ch->pcdata->special == SUBCLASS_NINJA)
            ch->pcdata->xp_pen += 500;
            if (ch->class == CLASS_THIEF
            && ch->pcdata->special == SUBCLASS_HIGHWAYMAN)
            ch->pcdata->xp_pen += 200;
         }
         else
         ch->pcdata->xp_pen = get_old_xp(ch);
      }

      if (ch->pcdata->age_mod == 0)
      get_age_mod(ch);
      dh = ch->pcdata->condition[COND_DEHYDRATED];

      if (ch->pcdata->condition[COND_DEHYDRATED] > 80)
      ch->pcdata->condition[COND_DEHYDRATED] = 0;


      if (ch->pcdata->quest == 0)
      {
         free_string(ch->pcdata->quest_name);
         ch->pcdata->quest_name = str_dup("for nothing");
      }

      ch->pcdata->condition[COND_DEHYDRATED] = dh;

      {
         OBJ_DATA *obj;
         OBJ_DATA *obj_next;

         for (obj = object_list; obj != NULL; obj = obj_next)
         {
            obj_next = obj->next;
            if (obj->carried_by == ch)
            {
               if (obj->pIndexData->house)
               {
                  extract_obj(obj, FALSE);
               }
               else if (obj->wear_loc != WEAR_NONE && obj->pIndexData->vnum == OBJ_VNUM_PHYSICAL_FOCUS)
               {
                  int x;

                  if ( !IS_NPC(ch) )
                  for (x = 1; x < MAX_SKILL; x++)
                  {
                     if (skill_table[x].name == NULL )
                     break;

                     if ( ch->pcdata->learnlvl[x] <= ch->level
                     && skill_table[x].spell_fun == spell_null
                     && ch->pcdata->learned[x] > 0
                     && ch->pcdata->learned[x] < 100 )
                     {
                        obj->timer = 10;
                        break;
                     }
                  }
               }
               else if (obj->wear_loc != WEAR_NONE && obj->pIndexData->vnum == OBJ_VNUM_MENTAL_FOCUS)
               {
                  int x;

                  if ( !IS_NPC(ch) )
                  for (x = 1; x < MAX_SKILL; x++)
                  {
                     if (skill_table[x].name == NULL )
                     break;

                     if ( ch->pcdata->learnlvl[x] <= ch->level
                     && skill_table[x].spell_fun != spell_null
                     && ch->pcdata->learned[x] > 0
                     && ch->pcdata->learned[x] < 100 )
                     {
                        obj->timer = 10;
                        break;
                     }
                  }
               }
               else if (obj->pIndexData->limtotal != 0 && ch->level < 10)
               extract_obj(obj, FALSE);
            }
         }

      }

      /*        if (ch->pcdata->special == 0)   For now, do it anyway */
      {
         OBJ_DATA *thirdw=NULL;

         if ( (ch->level > 10)
         && (ch->pcdata->learned[gsn_recall] >= 0)
         && (ch->house != HOUSE_CRUSADER) )
         {
            ch->pcdata->learnlvl[gsn_recall] = 53;
            ch->pcdata->learned[gsn_recall] = -1;
         }

         if ( (ch->house == HOUSE_CRUSADER)
         && (ch->pcdata->learned[gsn_steel_nerves] > 74) )
         {
            if ( ch->pcdata->learned[gsn_recall] < 75 )
            ch->pcdata->learned[gsn_recall] = 75;

            if ( ch->pcdata->learnlvl[gsn_recall] > 51 )
            ch->pcdata->learnlvl[gsn_recall] = 1;
         }


         /* Remove third arm stuff */
         if ((thirdw = get_eq_char(ch, WEAR_TERTIARY_WIELD)) != NULL)
         unequip_char( ch, thirdw );
         if (ch->morph_form[0] == MORPH_ARM)
         {
            ch->morph_form[1] = 0;
            ch->morph_form[2] = 0;
            affect_strip(ch, gsn_shapeshift);
         }
         if (ch->pcdata->learned[gsn_tertiary_wield] > -1)
         {
            ch->pcdata->learned[gsn_tertiary_wield] = -1;
            ch->pcdata->learnlvl[gsn_tertiary_wield] = 53;
         }
      }
      if (ch->morph_form[0] == MORPH_MIMIC)
      {
         affect_strip(ch, gsn_shapeshift);
         ch->morph_form[0] = 0;
         ch->morph_form[1] = 0;
         ch->morph_form[2] = 0;
         ch->mprog_target = NULL;
      }
      if (is_affected(ch, gsn_target))
      {
         affect_strip(ch, gsn_target);
         ch->pcdata->target = NULL;
      }

      if (IS_SET(ch->act2, PLR_LICH))
      {
         OBJ_DATA *phyl;
         char buf[MAX_INPUT_LENGTH];

         phyl = create_object(get_obj_index(OBJ_VNUM_PHYLACTERY), 0);
         sprintf(buf, "%s", ch->name);
         free_string(phyl->owner);
         phyl->owner = str_dup(buf);
         obj_to_char(phyl, ch);
      }

      if (ch->pcdata->last_logon < last_banknote_update)
      {
         /* ALready sent note, reset bank settings */
         for (i = 0; i < MAX_HOUSE; i++)
         {
            ch->pcdata->deposits[i] = 0;
         }
         ch->pcdata->clan_deposits = 0;
      }
      /*
         Successfully logged in, no need to store
         last logon anymore.  (stored to prevent
         people from using bad passwords for
         hoard characters)
      */
      ch->pcdata->last_logon = 0;

      /* report hack attemps and clear it - werv */
      if (ch->pcdata->hacked != 0)
      {
         sprintf(log_buf, "\n\r%d failed attempts at your password since your last login.\n\r", ch->pcdata->hacked);
         send_to_char(log_buf, ch);
         send_to_char("\n\r", ch);
         ch->pcdata->hacked = 0;
      }
      /* remove seehouse and seebrand at login -Wervdon */
      if (IS_IMMORTAL(ch) && IS_SET(ch->act2, WIZ_SEEHOUSE))
      REMOVE_BIT(ch->act2, WIZ_SEEHOUSE);
      if (IS_IMMORTAL(ch) && IS_SET(ch->act2, WIZ_SEEBRAND))
      REMOVE_BIT(ch->act2, WIZ_SEEBRAND);
      if (IS_IMMORTAL(ch) && IS_SET(ch->act2, WIZ_SEENEWBIE))
      REMOVE_BIT(ch->act2, WIZ_SEENEWBIE);
      if (IS_IMMORTAL(ch) && IS_SET(ch->act2, WIZ_SEEWORSHIP))
      {
         REMOVE_BIT(ch->act2, WIZ_SEEWORSHIP);
      }
      if (IS_IMMORTAL(ch) && IS_SET(ch->act2, WIZ_SEECLAN))
      {
         REMOVE_BIT(ch->act2, WIZ_SEECLAN);
      }
      if (is_affected(ch, gsn_spiritwalk))
      {
         do_return(ch, "");
      }
      ch->in_room = NULL;  /* sanity check */
      if (!ch->pcdata->being_restored)
      {
         room = hoarder_check(ch, room);
      }
      skill_adjust(ch, FALSE);
      if
      (
         !IS_IMMORTAL(ch) &&
         room != NULL &&
         room->house &&
         ch->house != room->house
      )
      {
         room = get_room_index(10601);  /* GLYNDANE market square */
      }
      char_to_room_1(ch, room, TO_ROOM_AT);
      char_to_room_1(ch, room, TO_ROOM_LOGIN);
      do_observe(ch, "", LOOK_AUTO);
      /* PERMANENT: room sector changes */
      update_visible(ch);
      save_char_obj(ch);  /* To be sure unknown skills are set to -1 */


      ch->pcdata->learnlvl[gsn_corrupt("subrank", &gsn_subrank)] = 53;


      apply_irv_ch(ch); /* Re-apply lost IVR affects - Wicket */

      wiznet("$N has left real life behind.", ch, NULL,
      WIZ_LOGINS, 0, get_trust(ch));

      if (ch->house != 0)
      announce_login(ch);

      do_unread(ch, "");
      for ( ich = ch->in_room->people; ich != NULL; ich = ich->next_in_room)
      {
         if (!((!can_see(ich, ch)) && (IS_IMMORTAL(ch))))
         act_new("$n has entered the realm.", ch, ch, ich, TO_VICT, POS_RESTING);
      }
      for (ich = char_list; ich; ich = ich_next)
      {
         ich_next = ich->next;
         if
         (
            IS_NPC(ich) &&
            (
               ich->master == ch ||
               ich == ch->pet
            )
         )
         {
            if (ich->on_line)
            {
               sprintf
               (
                  log_buf,
                  "Pets login, already on_line: (%s)'s [%s]",
                  ch->name,
                  ich->short_descr
               );
               bug_trust(log_buf, 0, get_trust(ch));
            }
            char_to_room_1(ich, ch->in_room, TO_ROOM_AT);
            act("$n has entered the realm.", ich, NULL, NULL, TO_ROOM);
            char_to_room_1(ich, ch->in_room, TO_ROOM_LOGIN);
         }
      }
      if
      (
         ch->desc &&
         ch->desc->host
      )
      {
         free_string(ch->host);
         ch->host = str_dup(ch->desc->host);
      }
      do_wizireport_update(ch, WIZI_LOGON);
      /*
      act( "$n has entered the realm.", ch, NULL, NULL, TO_ROOM );
      */
      break;
   }

   return;
}



/*
* Parse a name for acceptability.
*/
bool check_parse_name(char *name)
{
   sh_int god_itr;
   sh_int house_itr;
   sh_int clan_itr;

   /*
   * Reserved words.
   */
   if
   (
      is_name
      (
         name,
         "all auto immortal self zzz someone something the you demise balance "
         "circle loner honor builder imms imm assassin assassins "
         "assassination leader leaders"
      )
   )
   {
      return FALSE;
   }

   /*
   * Length restrictions.
   */

   if (strlen(name) <  2)
   {
      return FALSE;
   }

#if defined(MSDOS)
   if (strlen(name) >  8)
   {
      return FALSE;
   }
#endif

#if defined(macintosh) || defined(unix)
   if (strlen(name) > 12)
   {
      return FALSE;
   }
#endif

   /*
   * Alphanumerics only.
   * Lock out IllIll twits.
   */
   {
      char* pc;
      bool  fIll;
      bool  first = TRUE;

      fIll = TRUE;
      for (pc = name; *pc != '\0'; pc++)
      {
         if (!isalpha(*pc))
         {
            return FALSE;
         }

         if (!first && isupper(*pc)) /* ugly anti-caps hack */
         {
            return FALSE;
         }

         if
         (
            LOWER(*pc) != 'i' &&
            LOWER(*pc) != 'l'
         )
         {
            fIll = FALSE;
         }
         first = FALSE;
      }

      if (fIll)
      {
         return FALSE;
      }
   }

   /*
   * Prevent players from naming themselves after mobs.
   */
   {
      MOB_INDEX_DATA* pMobIndex;
      AREA_DATA* parea;

      for (parea = area_first; parea; parea = parea->next)
      {
         for (pMobIndex = parea->mobs; pMobIndex; pMobIndex = pMobIndex->next_in_area)
         {
            if (is_name(name, pMobIndex->player_name))
            {
               return FALSE;
            }
         }
      }
   }
   
   /* Brand notebins */
   for (god_itr = 0; worship_table[god_itr].name != NULL; god_itr++)
   {
      if
      (
         worship_table[god_itr].brand_info.note_bin != NULL &&
         !str_cmp(name, worship_table[god_itr].brand_info.note_bin)
      )
      {
         return FALSE;
      }
   }
   
   /* House notebins */
   for (house_itr = 1; house_itr < MAX_HOUSE; house_itr++)
   {
      if
      (
         house_table[house_itr].name != NULL &&
         !str_cmp(name, house_table[house_itr].name)
      )
      {
         return FALSE;
      }
   }
   
   /* Clan notebins */
   for (clan_itr = 1; clan_itr < MAX_CLAN - 1; clan_itr++)
   {
      if
      (
         clan_table[clan_itr].valid &&
         !str_cmp(name, clan_table[clan_itr].who_name)
      )
      {
         return FALSE;
      }
   }

   return TRUE;
}



/*
* Look for link-dead player to reconnect.
*/
bool check_reconnect(DESCRIPTOR_DATA *d, char *name, bool fConn, CHAR_DATA** old)
{
   CHAR_DATA *ch, *ich;
   /*    OBJ_DATA *obj;*/

   *old = NULL;
   for ( ch = char_list; ch != NULL; ch = ch->next )
   {
      if
      (
         !IS_NPC(ch) &&
         (
            !fConn ||
            ch->desc == NULL
         ) &&
         !str_cmp
         (
            d->character->name,
            ch->name
         )
      )
      {
         *old = ch;
         if ( fConn == FALSE )
         {
            free_string( d->character->pcdata->pwd );
            d->character->pcdata->pwd = str_dup( ch->pcdata->pwd );
         }
         else
         {
            /* Calculate light correctly */
            d->character->in_room = NULL;
            die_follower(d->character);
            free_char( d->character );
            d->character = ch;
            ch->desc     = d;
            if
            (
               ch->desc &&
               ch->desc->host
            )
            {
               free_string(ch->host);
               ch->host = str_dup(ch->desc->host);
            }
            ch->timer     = 0;
            if (buf_string(ch->pcdata->buffer)[0] == '\0')
            {
               send_to_char("Reconnecting.\n\r", ch);
            }
            else
            {
               send_to_char("Reconnecting.\n\rType replay to see missed tells.\n\r", ch );
            }
            /*        if (!ch->invis_level)
            act( "$n has regained consciousness.", ch, NULL, NULL, TO_ROOM );
            */
            do_wizireport_update
            (
               ch,
               WIZI_LOGON
            );

            for ( ich = ch->in_room->people; ich != NULL; ich =ich->next_in_room)
            {
               if (!((!can_see(ich, ch)) && (IS_IMMORTAL(ch))))
               act_new("$n has regained consciousness.", ch, ch, ich, TO_VICT, POS_RESTING);
            }

            /* Limit crap to balance reconnect objects from extracted link object */
            /*    for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
            {
            obj->pIndexData->limcount++;
            }*/

            sprintf(log_buf,  "%s@%s reconnected.", ch->name, d->host );
            log_string( log_buf );
            wiznet("$N groks the fullness of $S link.",
            ch, NULL, WIZ_LINKS, 0, get_trust(ch));
            d->connected = CON_PLAYING;
            if (ch->pcdata->hacked != 0)
            {
               sprintf
               (
                  log_buf,
                  "\n\r%d failed attempts at your password since your last login.\n\r",
                  ch->pcdata->hacked
               );
               send_to_char(log_buf, ch);
               send_to_char("\n\r", ch);
               ch->pcdata->hacked = 0;
            }
         }
         return TRUE;
      }
   }

   return FALSE;
}



/*
* Check if already playing.
*/
bool check_playing( DESCRIPTOR_DATA *d, char *name, CHAR_DATA** old, bool test)
{
   DESCRIPTOR_DATA *dold;

   *old = NULL;
   for ( dold = descriptor_list; dold; dold = dold->next )
   {
      if
      (
         dold != d &&
         dold->character != NULL &&
         dold->connected != CON_GET_NAME &&
         dold->connected != CON_GET_OLD_PASSWORD &&
         !str_cmp
         (
            name,
            dold->original ?
            dold->original->name :
            dold->character->name
         )
      )
      {
         *old = dold->original ? dold->original : dold->character;
         if (test)
         {
            return TRUE;
         }
         write_to_buffer( d, "That character is already playing.\n\r", 0);
         write_to_buffer( d, "Do you wish to connect anyway (Y/N)?", 0);
         d->connected = CON_BREAK_CONNECT;
         return TRUE;
      }
   }

   return FALSE;
}

/*
* Check if already playing.  (Duplicate named newbies)
*/
bool check_playing_newbie(DESCRIPTOR_DATA *d)
{
   DESCRIPTOR_DATA *d2;
   char* name;
   char* name2;

   name = d->character->name;
   for (d2 = descriptor_list; d2; d2 = d2->next)
   {
      if
      (
         d2 != d &&
         d2->character != NULL &&
         d2->connected == CON_PLAYING &&
         (
            name2 = d2->character->name
         ) &&
         !str_cmp(name, name2)
      )
      {
         return TRUE;
      }
   }
   return FALSE;
}


void stop_idling( CHAR_DATA *ch )
{
   if ( ch == NULL
   ||   ch->desc == NULL
   ||   ch->desc->connected != CON_PLAYING
   ||   ch->was_in_room == NULL
   ||   ch->in_room != get_room_index(ROOM_VNUM_LIMBO))
   return;

   ch->timer = 0;
   char_from_room( ch );
   char_to_room( ch, ch->was_in_room );
   ch->was_in_room    = NULL;
   act( "$n has returned from the void.", ch, NULL, NULL, TO_ROOM );
   return;
}


void make_lich(CHAR_DATA *ch, int lmod)
{
   int chance;
   int essmod;
   OBJ_DATA *phyl;
   char buf[MAX_INPUT_LENGTH];

   send_to_char("You feel the potion begin to act on your living body...\n\r", ch);
   chance = 30;
   essmod = 60 - (lmod*6);
   if (ch->perm_stat[STAT_INT] >= 23)
   chance += 25;
   if (ch->perm_stat[STAT_WIS] >= 20)
   chance += 25;
   if (ch->pcdata->special == SUBCLASS_NECROPHILE)
   chance += 10;
   if (ch->level < 51)
   chance -= 70;
   if (ch->class != (GUILD_NECROMANCER - 1))
   chance -= 70;
   chance = chance - essmod;

   if (number_percent() > chance)
   {
      send_to_char("You are too weak to survive the transformation!\n\r", ch);
      send_to_char("You are consumed by the power of the ritual!\n\r", ch);
      if (ch->level < 52 )
      ch->perm_stat[STAT_CON] = 3;
      raw_kill(ch, ch);
      return;
   }

   phyl = create_object(get_obj_index(OBJ_VNUM_PHYLACTERY), 0);
   sprintf(buf, "%s", ch->name);
   free_string(phyl->owner);
   phyl->owner = str_dup(buf);
   obj_to_char(phyl, ch);
   send_to_char("Your mortal body dies, and you are reborn...a lich!\n\r", ch);
   SET_BIT(ch->act2, PLR_LICH);
   ch->perm_stat[STAT_CON] = 20;
   return;
}
/*
* Write to one char.
*/
void send_to_char( const char *txt, CHAR_DATA *ch )
{
   if ( txt != NULL && ch->desc != NULL )
   write_to_buffer( ch->desc, txt, strlen(txt) );
   return;
}

/*
* Send a page to one char.
*/
void page_to_char( const char *txt, CHAR_DATA *ch )
{
   if ( txt == NULL || ch->desc == NULL)
   return;

   if (ch->lines == 0 )
   {
      send_to_char(txt, ch);
      return;
   }

#if defined(macintosh)
   send_to_char(txt, ch);
#else
   ch->desc->showstr_head = alloc_mem(strlen(txt) + 1);
   strcpy(ch->desc->showstr_head, txt);
   ch->desc->showstr_point = ch->desc->showstr_head;
   show_string(ch->desc, "");
#endif
}


/* string pager */
void show_string(struct descriptor_data *d, char *input)
{
   char buffer[4*MAX_STRING_LENGTH];
   char buf[MAX_INPUT_LENGTH];
   register char *scan, *chk;
   int lines = 0, toggle = 1;
   int show_lines;

   one_argument(input, buf);
   if (buf[0] != '\0')
   {
      if (d->showstr_head)
      {
         free_mem(d->showstr_head, strlen(d->showstr_head));
         d->showstr_head = 0;
      }
      d->showstr_point  = 0;
      return;
   }

   if (d->character)
   show_lines = d->character->lines;
   else
   show_lines = 0;

   for (scan = buffer; ; scan++, d->showstr_point++)
   {
      if (((*scan = *d->showstr_point) == '\n' || *scan == '\r')
      && (toggle = -toggle) < 0)
      lines++;

      else if (!*scan || (show_lines > 0 && lines >= show_lines))
      {
         *scan = '\0';
         write_to_buffer(d, buffer, strlen(buffer));
         for (chk = d->showstr_point; isspace(*chk); chk++);
         {
            if (!*chk)
            {
               if (d->showstr_head)
               {
                  free_mem(d->showstr_head, strlen(d->showstr_head));
                  d->showstr_head = 0;
               }
               d->showstr_point  = 0;
            }
         }
         return;
      }
   }
   return;
}


/* quick sex fixer */
void fix_sex(CHAR_DATA *ch)
{
   if (ch->sex < 0 || ch->sex > 2)
   ch->sex = IS_NPC(ch) ? 0 : ch->pcdata->true_sex;
}

/*
void act (const char *format, CHAR_DATA *ch, const void *arg1, const void *arg2, int type)
{
   act_new(format, ch, arg1, arg2, type, POS_RESTING);
   tail_chain();
}

void act_new( const char *format, CHAR_DATA *ch, const void *arg1, const void *arg2, int type, int min_pos)
{
   act_new_color(format, ch, arg1, arg2, type, min_pos, FALSE);
   tail_chain();
}

void act_color(const char *format, CHAR_DATA *ch, const void *arg1, const void *arg2, int type)
{
   act_new_color(format, ch, arg1, arg2, type, POS_RESTING, TRUE);
   tail_chain();
}

void act_nnew( const char *format, CHAR_DATA *ch, const void *arg1,
const void *arg2, int type, int min_pos)
{
   act_new_color(format, ch, arg1, arg2, type, min_pos, FALSE);
   tail_chain();
}
*/

/*
 * Strips the initial space in a string.
 */
void strip_initial_space(char* string_to_strip)
{
   char stripped_string[MAX_STRING_LENGTH];
   char* input_char_ptr = string_to_strip;

   /* Strip initial spaces. */
   while (*input_char_ptr != '\0' && isspace(*input_char_ptr))
   {
      input_char_ptr++;
   }

   if (input_char_ptr != string_to_strip)
   {
      /* Put the remainder string into our buffer. */
      strcpy(stripped_string, input_char_ptr);

      /* Copy the buffer back into our original string buffer. */
      strcpy(string_to_strip, stripped_string);
   }
}

/*
 * Is it an article?
 */
bool is_article(char* article)
{
   return
      !strcmp(article, "the") ||
      !strcmp(article, "a") ||
      !strcmp(article, "an");
}

/*
 * Is it a plural possessive?
 */
bool is_plural_possessive(char* article)
{
   /*
    * Plural possessive form is a plural noun that ends in "s'" or "es'".
    *
    * Examples:  "kittens' paws" "Immortals' commandments" "buses' wheels"
    *
    * Exceptions:  dialect; we should have none in the MUD code by standard
    *              (example -- Ye grab th' a pitchfork)
    *
    * Mitigation:  No dialect should precede our objects in standard MUD code.
    */

   int last_char = strlen(article) - 1;

   return
      article[last_char]     == '\'' &&
      article[last_char - 1] == 's';
}

/*
 * Is it a singular possessive?
 */
bool is_singular_possessive(char* article)
{
   /*
    * Singular possessive form is a singular noun that ends in "'s".
    *
    * Examples:  cat's X's
    *
    * Exceptions:  contractions (we should have none by standard)
    *
    *    WARNING:  "it's" is a contraction.
    *              Strangely, it's commonly mistaken for a possessive,
    *              probably because of its "'s" ending.
    *
    * Mitigation:  We expect to run this function on a word that
    *              precedes an object.  Therefore, phrases such as
    *              "X's a dummy" would translate to
    *              "X's dummy", especially given our contraction
    *              standard.
    */

   int last_char = strlen(article) - 1;

   return
      article[last_char]     == 's' &&
      article[last_char - 1] == '\'';
}

/*
 * Is it a possessive?
 */
bool is_possessive(char* article)
{
   return
      !strcmp(article, "my") ||
      !strcmp(article, "your") ||
      !strcmp(article, "his") ||
      !strcmp(article, "her") ||
      !strcmp(article, "its") ||
      !strcmp(article, "our") ||
      !strcmp(article, "their") ||
      is_singular_possessive(article) ||
      is_plural_possessive(article);
}

/*
 * Strips the initial article from the string
 */
void strip_article(char* string_to_strip)
{
   char stripped_string[MAX_STRING_LENGTH];
   char article[MAX_STRING_LENGTH];
   char* input_char_ptr = string_to_strip;
   char* article_char_ptr = article;
   const int max_length_of_article = 3;
   int current_article_length = 0;

   /* Strip initial spaces. */
   while
   (
      *input_char_ptr != '\0' &&
      !isspace(*input_char_ptr) &&
      current_article_length <= max_length_of_article
   )
   {
      *article_char_ptr = LOWER(*input_char_ptr);
      article_char_ptr++;
      input_char_ptr++;
      current_article_length++;
   }

   if (current_article_length > max_length_of_article)
   {
      /* Too long to be an article */
      return;
   }

   *article_char_ptr = '\0';

   if (
         *input_char_ptr == '\0' ||  /* Was the whole thing just one word? */
         !is_article(article)
      )
   {
      return;
   }

   /* Strip remaining spaces. */
   while (*input_char_ptr != '\0' && isspace(*input_char_ptr))
   {
      input_char_ptr++;
   }

   if (input_char_ptr != string_to_strip)
   {
      /* Put the remainder string into our buffer. */
      strcpy(stripped_string, input_char_ptr);

      /* Copy the buffer back into our original string buffer. */
      strcpy(string_to_strip, stripped_string);
   }
}

/*
 * Strips an aura from the beginning of a string.  An aura is defined
 * as some phrase surrounded by parentheses.
 */
void strip_aura(char* string_to_strip)
{
   char stripped_string[MAX_STRING_LENGTH];
   char* input_char_ptr = string_to_strip;
   int parentheses_nest_level = 0;

   strip_initial_space(string_to_strip);

   /*
    * Skip the first aura.  This algorithm also handles the case of nested
    * parentheses...just in case.
    */
   if (*input_char_ptr == '(')
   {
      parentheses_nest_level++;
      input_char_ptr++;

      while (parentheses_nest_level > 0)
      {
         if (*input_char_ptr == '\0')
         {
            break;
         }

         if (*input_char_ptr == '(')
         {
            parentheses_nest_level++;
         }

         if (*input_char_ptr == ')')
         {
            parentheses_nest_level--;
         }

         input_char_ptr++;
      }
   }

   if (input_char_ptr != string_to_strip)
   {
      /* Put the remainder string into our buffer. */
      strcpy(stripped_string, input_char_ptr);

      strip_initial_space(stripped_string);

      /* Copy the buffer back into our original string buffer. */
      strcpy(string_to_strip, stripped_string);
   }
}

void preceding_word_append
(
   char*  preceding_word,
   char** preceding_word_ptr,
   char   letter
)
{
   if (isalpha(letter) || (letter == '\''))
   {
      if (**preceding_word_ptr == '\0')
      {
         *preceding_word_ptr = preceding_word;
      }
      **preceding_word_ptr = LOWER(letter);
      (*preceding_word_ptr)++;
      **preceding_word_ptr = ' ';
   }
   else if (isspace(letter))
   {
      **preceding_word_ptr = '\0';
   }
}

void act_new_color( const char *format, CHAR_DATA *ch, const void *arg1,
const void *arg2, int type, int min_pos, bool color)
{
   static char * const he_she  [] = { "it",  "he",  "she" };
   static char * const him_her [] = { "it",  "him", "her" };
   static char * const his_her [] = { "its", "his", "her" };

   char output_string[MAX_STRING_LENGTH];
   char immbuf[MAX_STRING_LENGTH];
   char object_name[MAX_STRING_LENGTH];
   char fname[MAX_INPUT_LENGTH];
   char preceding_word[MAX_INPUT_LENGTH];
   char* ansi_filtered_format_string;
   CHAR_DATA *to;
   CHAR_DATA *vch = (CHAR_DATA *) arg2;
   OBJ_DATA *obj1 = (OBJ_DATA  *) arg1;
   OBJ_DATA *obj2 = (OBJ_DATA  *) arg2;
   const char *format_char_ptr;
   const char *replacement_char_ptr;
   char* preceding_word_ptr = preceding_word;
   char* output_char_ptr;
   bool no_arena = FALSE;
   bool skip_null = FALSE;
   bool wizi_skip;

   if (type == TO_ROOM_NA)
   {
      type = TO_ROOM;
      no_arena = TRUE;
   }
   if
   (
      type == TO_VICT ||
      type == TO_CHAR
   )
   {
      no_arena = TRUE;
   }
   /*
   * Discard null and zero-length messages.
   */
   if ( format == NULL || format[0] == '\0' )
   return;

   /* discard null rooms and chars */
   if (ch == NULL || ch->in_room == NULL)
   return;

   to = ch->in_room->people;
   if ( type == TO_VICT )
   {
      if ( vch == NULL )
      {
         bug( "Act: null vch with TO_VICT.", 0 );
         return;
      }

      if (vch->in_room == NULL)
      return;

      to = vch->in_room->people;
   }

   for ( ; to != NULL; to = to->next_in_room )
   {
      if ( (!IS_NPC(to) && to->desc == NULL )
      ||    to->position < min_pos )
      continue;

      if ( (type == TO_CHAR) && to != ch )
      continue;
      if ( type == TO_VICT && ( to != vch || to == ch ) )
      continue;
      if ( type == TO_ROOM && to == ch )
      continue;
      if ( type == TO_NOTVICT && (to == ch || to == vch) )
      continue;

      output_char_ptr   = output_string;
      if (color)
      {
         ansi_filtered_format_string  = convert_color((char*)format, IS_ANSI(to));
      }
      else
      {
         ansi_filtered_format_string  = (char*)format;
      }
      format_char_ptr     = ansi_filtered_format_string;
      wizi_skip = FALSE;
      preceding_word[0] = '\0';

      while
      (
         *format_char_ptr != '\0' &&
         !wizi_skip
      )
      {
         if (*format_char_ptr != '$')
         {
            preceding_word_append(
               preceding_word,
               &preceding_word_ptr,
               *format_char_ptr);
            *output_char_ptr++ = *format_char_ptr++;
            continue;
         }
         ++format_char_ptr;

         skip_null = FALSE;
         switch (*format_char_ptr)
         {
            default:
            break;
            case 't':
            case 'p':
            if (arg1 == NULL)
            {
               skip_null = TRUE;
            }
            break;
            case 'n':
            case 'f':
            case 'e':
            case 'm':
            case 's':
            case 'w':
            case 'y':
            case 'z':
            if (ch == NULL)
            {
               skip_null = TRUE;
            }
            break;
            case 'P':
            case 'T':
            case 'E':
            case 'M':
            case 'S':
            case 'F':
            case 'N':
            case 'W':
            case 'Y':
            case 'Z':
            if (arg2 == NULL)
            {
               skip_null = TRUE;
            }
            break;
         }
         if (skip_null)
         {
            sprintf
            (
               log_buf,
               "Act: code $%c with null argument: %s",
               *format_char_ptr,
               format
            );
            replacement_char_ptr = " <@@@> ";
            break;
         }
         else
         {
            switch ( *format_char_ptr )
            {
               default:
               sprintf
               (
                  log_buf,
                  "Act: bad code $%c: %s",
                  *format_char_ptr,
                  format
               );
               bug(log_buf, 0);
               replacement_char_ptr = " <@@@> ";                                break;
               /* Thx alex for 't' idea */
               case 't': replacement_char_ptr = (char *) arg1;                            break;
               case 'T': replacement_char_ptr = (char *) arg2;                            break;
               case 'w':
               {
                  if
                  (
                     !can_see(to, ch) &&
                     wizi_to(ch, to)
                  )
                  {
                     wizi_skip = TRUE;
                     continue;
                  }
                  /*
                     KEEP 'w' RIGHT BEFORE 'n'
                     It reuses the code!
                     That is why there is no break here.
                  */
               }
               case 'n':
               if
               (
                  IS_TRUSTED(to, ANGEL) &&
                  (
                     is_affected(ch, gsn_cloak_form) ||
                     (
                        (
                           is_affected(ch, gsn_strange_form) ||
                           ch->morph_form[0] == MORPH_MIMIC
                        ) &&
                        IS_TRUSTED(to, ANGEL)
                     )
                  )
               )
               sprintf(immbuf, "%s (%s)", get_descr_form(ch, to, FALSE),
               ch->name); else
               sprintf(immbuf, "%s", get_descr_form(ch, to, FALSE));
               replacement_char_ptr = immbuf;
               break;
               case 'W':
               {
                  if
                  (
                     !can_see(to, vch) &&
                     wizi_to(vch, to)
                  )
                  {
                     wizi_skip = TRUE;
                     continue;
                  }
                  /*
                     KEEP 'w' RIGHT BEFORE 'n'
                     It reuses the code!
                     That is why there is no break here.
                  */
               }
               case 'N': if ((IS_IMMORTAL(to) /*|| (vch->house ==
               to->house)*/) &&
               (is_affected(vch, gsn_cloak_form) || ((is_affected(vch, gsn_strange_form) ||
               (vch->morph_form[0] == MORPH_MIMIC)) && IS_TRUSTED(to, ANGEL))))
               sprintf(immbuf, "%s (%s)", get_descr_form(vch, to, FALSE),
               vch->name); else
               sprintf(immbuf, "%s", get_descr_form(vch, to, FALSE));
               replacement_char_ptr = immbuf;
               break;
               case 'f' :
               replacement_char_ptr = IS_NPC(ch) ? ch->short_descr : ch->name;
               break;
               case 'F' :
               replacement_char_ptr= IS_NPC(vch) ? vch->short_descr : vch->name;
               break;
               case 'e': replacement_char_ptr = he_she  [URANGE(0, ch  ->sex, 2)];        break;
               case 'E': replacement_char_ptr = he_she  [URANGE(0, vch ->sex, 2)];        break;
               case 'm': replacement_char_ptr = him_her [URANGE(0, ch  ->sex, 2)];        break;
               case 'M': replacement_char_ptr = him_her [URANGE(0, vch ->sex, 2)];        break;
               case 's': replacement_char_ptr = his_her [URANGE(0, ch  ->sex, 2)];        break;
               case 'S': replacement_char_ptr = his_her [URANGE(0, vch ->sex, 2)];        break;

               case 'z': replacement_char_ptr = get_god_name(ch, to);                     break;
               case 'Z': replacement_char_ptr = get_god_name(vch, to);                    break;

               case 'y': replacement_char_ptr = house_table[ch->house].name;                     break;
               case 'Y': replacement_char_ptr = house_table[vch->house].name;                    break;

               case 'p':
                  if (can_see_obj(to, obj1))
                  {
                     strcpy(object_name, obj1->short_descr);
                     strip_aura(object_name);
                     if (is_possessive(preceding_word) || is_article(preceding_word))
                     {
                        strip_article(object_name);
                     }
                     replacement_char_ptr = object_name;
                  }
                  else
                  {
                     replacement_char_ptr = "something";
                  }
                  break;

               case 'P':
                  if (can_see_obj(to, obj2))
                  {
                     strcpy(object_name, obj2->short_descr);
                     strip_aura(object_name);
                     if (is_possessive(preceding_word) || is_article(preceding_word))
                     {
                        strip_article(object_name);
                     }
                     replacement_char_ptr = object_name;
                  }
                  else
                  {
                     replacement_char_ptr = "something";
                  }
                  break;

               case 'd':
               if ( arg2 == NULL || ((char *) arg2)[0] == '\0' )
               {
                  replacement_char_ptr = "door";
               }
               else
               {
                  one_argument( (char *) arg2, fname );
                  replacement_char_ptr = fname;
               }
               break;
            }
         }

         ++format_char_ptr;
         while ((*output_char_ptr = *replacement_char_ptr) != '\0')
         {
            preceding_word_append(
               preceding_word,
               &preceding_word_ptr,
               *replacement_char_ptr);
            ++output_char_ptr, ++replacement_char_ptr;
         }
      }
      if (wizi_skip)
      {
         if (color)
         {
            free_string(ansi_filtered_format_string);
         }
         continue;
      }
      *output_char_ptr++ = '\n';
      *output_char_ptr++ = '\r';
      *output_char_ptr   = '\0';
      output_string[0]   = UPPER(output_string[0]);
      if ( to->desc != NULL )
      write_to_buffer( to->desc, output_string, output_char_ptr - output_string );
      else
      if ( MOBtrigger )
      mp_act_trigger( output_string, to, ch, arg1, arg2, TRIG_ACT );
      if (color)
      {
         free_string(ansi_filtered_format_string);
      }
   }
   /*
   * Arena act to_room extensions -Wervdon
   */
   if (!no_arena &&
   (ch->in_room->vnum >= 10437 && ch->in_room->vnum <= 10461))
   {
      int cnt;
      ROOM_INDEX_DATA *room;
      for (cnt = 10462; cnt <= 10496; cnt++)
      {
         room = get_room_index(cnt);
         if (room == NULL) continue;
         to = room->people;
         if (to == NULL) continue;
         for ( ; to != NULL; to = to->next_in_room )
         {
            if ( (!IS_NPC(to) && to->desc == NULL )
            ||    to->position < min_pos )
            continue;

            if ( (type == TO_CHAR) && to != ch )
            continue;
            if ( type == TO_ROOM && to == ch )
            continue;
            if ( type == TO_NOTVICT && (to == ch || to == vch) )
            continue;

            output_char_ptr   = output_string;
            if (color)
            {
               ansi_filtered_format_string = convert_color((char*)format, IS_ANSI(to));
            }
            else
            {
               ansi_filtered_format_string = (char*)format;
            }
            format_char_ptr     = ansi_filtered_format_string;
            wizi_skip = FALSE;
            preceding_word[0] = '\0';

            while
            (
               *format_char_ptr != '\0' &&
               !wizi_skip
            )
            {
               if (*format_char_ptr != '$')
               {
                  preceding_word_append(
                     preceding_word,
                     &preceding_word_ptr,
                     *format_char_ptr);
                  *output_char_ptr++ = *format_char_ptr++;
                  continue;
               }
               ++format_char_ptr;

               switch ( *format_char_ptr )
               {
                  default:
                  sprintf
                  (
                     log_buf,
                     "Act: bad code $%c: %s",
                     *format_char_ptr,
                     format
                  );
                  bug(log_buf, 0);
                  replacement_char_ptr = " <@@@> ";                                break;
                  /* Thx alex for 't' idea */
                  case 't': replacement_char_ptr = (char *) arg1;                            break;
                  case 'T': replacement_char_ptr = (char *) arg2;                            break;
                  case 'w':
                  {
                     if
                     (
                        !can_see(to, ch) &&
                        wizi_to(ch, to)
                     )
                     {
                        wizi_skip = TRUE;
                        continue;
                     }
                     /*
                        KEEP 'w' RIGHT BEFORE 'n'
                        It reuses the code!
                        That is why there is no break here.
                     */
                  }
                  case 'n': if ((IS_IMMORTAL(to) /*||
                  (ch->house==to->house)*/) &&
                  (is_affected(ch, gsn_cloak_form) || ((is_affected(ch, gsn_strange_form) ||
                  (ch->morph_form[0] == MORPH_MIMIC)) && IS_TRUSTED(to, ANGEL))))
                  sprintf(immbuf, "%s (%s)", get_descr_form(ch, to, FALSE),
                  ch->name); else
                  sprintf(immbuf, "%s", get_descr_form(ch, to, FALSE));
                  replacement_char_ptr = immbuf;
                  break;
                  case 'W':
                  {
                     if
                     (
                        !can_see(to, vch) &&
                        wizi_to(vch, to)
                     )
                     {
                        wizi_skip = TRUE;
                        continue;
                     }
                     /*
                        KEEP 'w' RIGHT BEFORE 'n'
                        It reuses the code!
                        That is why there is no break here.
                     */
                  }
                  case 'N': if ((IS_IMMORTAL(to)) &&
                  (is_affected(vch, gsn_cloak_form) || ((is_affected(vch, gsn_strange_form) ||
                  (vch->morph_form[0] == MORPH_MIMIC)) && IS_TRUSTED(to, ANGEL))))
                  sprintf(immbuf, "%s (%s)", get_descr_form(vch, to, FALSE),
                  vch->name); else
                  sprintf(immbuf, "%s", get_descr_form(vch, to, FALSE));
                  replacement_char_ptr = immbuf;
                  break;
                  case 'f' :
                  replacement_char_ptr = IS_NPC(ch) ? ch->short_descr : ch->name;
                  break;
                  case 'F' :
                  replacement_char_ptr = IS_NPC(vch) ? vch->short_descr : vch->name;
                  break;
                  case 'e': replacement_char_ptr = he_she  [URANGE(0, ch  ->sex, 2)];        break;
                  case 'E': replacement_char_ptr = he_she  [URANGE(0, vch ->sex, 2)];        break;
                  case 'm': replacement_char_ptr = him_her [URANGE(0, ch  ->sex, 2)];        break;
                  case 'M': replacement_char_ptr = him_her [URANGE(0, vch ->sex, 2)];        break;
                  case 's': replacement_char_ptr = his_her [URANGE(0, ch  ->sex, 2)];        break;
                  case 'S': replacement_char_ptr = his_her [URANGE(0, vch ->sex, 2)];        break;

                  case 'z': replacement_char_ptr = get_god_name(ch, to);                     break;
                  case 'Z': replacement_char_ptr = get_god_name(vch, to);                    break;

                  case 'y': replacement_char_ptr = house_table[ch->house].name;                     break;
                  case 'Y': replacement_char_ptr = house_table[vch->house].name;                    break;

                  case 'p':
                     if (can_see_obj(to, obj1))
                     {
                        strcpy(object_name, obj1->short_descr);
                        strip_aura(object_name);
                        if (is_possessive(preceding_word) || is_article(preceding_word))
                        {
                           strip_article(object_name);
                        }
                        replacement_char_ptr = object_name;
                     }
                     else
                     {
                        replacement_char_ptr = "something";
                     }
                     break;

                  case 'P':
                     if (can_see_obj(to, obj2))
                     {
                        strcpy(object_name, obj2->short_descr);
                        strip_aura(object_name);
                        if (is_possessive(preceding_word) || is_article(preceding_word))
                        {
                           strip_article(object_name);
                        }
                        replacement_char_ptr = object_name;
                     }
                     else
                     {
                        replacement_char_ptr = "something";
                     }
                     break;

                  case 'd':
                  if ( arg2 == NULL || ((char *) arg2)[0] == '\0' )
                  {
                     replacement_char_ptr = "door";
                  }
                  else
                  {
                     one_argument( (char *) arg2, fname );
                     replacement_char_ptr = fname;
                  }
                  break;
               }

               ++format_char_ptr;
               while ((*output_char_ptr = *replacement_char_ptr) != '\0')
               {
                  preceding_word_append(
                     preceding_word,
                     &preceding_word_ptr,
                     *replacement_char_ptr);
                  ++output_char_ptr, ++replacement_char_ptr;
               }
            }

            *output_char_ptr++ = '\n';
            *output_char_ptr++ = '\r';
            *output_char_ptr   = '\0';
            output_string[0]   = UPPER(output_string[0]);
            if ( to->desc != NULL )
            write_to_buffer( to->desc, output_string, output_char_ptr - output_string );
            else
            if ( MOBtrigger )
            mp_act_trigger( output_string, to, ch, arg1, arg2, TRIG_ACT );
            if (color)
            {
               free_string(ansi_filtered_format_string);
            }
         }
      }
   }
   return;
}

/*
* Macintosh support functions.
*/
#if defined(macintosh)
int gofday( TIMEVAL *tp, void *tzp )
{
   tp->tv_sec  = time( NULL );
   tp->tv_usec = 0;
}
#endif



void announce_login(CHAR_DATA* ch)
{
   CHAR_DATA* wch;
   char buf1[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];

   if
   (
      ch->house == 0 ||
      ch->house > MAX_HOUSE ||
      IS_SET(ch->act2, PLR_NO_HOUSE_EMOTE)
   )
   {
      return;
   }

   if (house_table[ch->house].greeting_self[0] != '\0')
   {
      sprintf(buf2, house_table[ch->house].greeting_self, ch->name);
   }
   else
   {
      buf2[0] = '\0';
   }
   if (house_table[ch->house].greeting_other[0] != '\0')
   {
      sprintf(buf1, house_table[ch->house].greeting_other, ch->name);
   }
   else
   {
      buf1[0] = '\0';
   }
   if
   (
      buf1[0] == '\0' &&
      buf2[0] == '\0'
   )
   {
      return;
   }
   for (wch = char_list; wch != NULL; wch = wch->next)
   {
      if
      (
         wch->house != ch->house ||
         (
            IS_IMMORTAL(ch) &&
            wizi_to(ch, wch)
         ) ||
         (
            !IS_NPC(wch) &&
            wch->pcdata->induct <= INDUCT_APPRENTICE &&
            !IS_IMMORTAL(wch)
         )
      )
      {
         continue;
      }
      if (wch != ch)
      {
         if (buf1[0] == '\0')
         {
            continue;
         }
         if
         (
            wch->house != HOUSE_ANCIENT ||
            IS_IMMORTAL(wch)
         )
         {
            send_to_char(buf1, wch);
         }
      }
      else
      {
         if (buf2[0] == '\0')
         {
            continue;
         }
         send_to_char(buf2, wch);
      }
   }

   return;
}

void announce_logout(CHAR_DATA *ch)
{
   CHAR_DATA *wch;
   char buf1[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];

   if
   (
      ch->house == 0 ||
      ch->house > MAX_HOUSE ||
      IS_SET(ch->act2, PLR_NO_HOUSE_EMOTE)
   )
   {
      return;
   }

   if (house_table[ch->house].farewell_self[0] != '\0')
   {
      sprintf(buf2, house_table[ch->house].farewell_self, ch->name);
   }
   else
   {
      buf2[0] = '\0';
   }
   if (house_table[ch->house].farewell_other[0] != '\0')
   {
      sprintf(buf1, house_table[ch->house].farewell_other, ch->name);
   }
   else
   {
      buf1[0] = '\0';
   }

   for (wch = char_list; wch != NULL; wch = wch->next)
   {
      if
      (
         wch->house != ch->house ||
         (
            IS_IMMORTAL(ch) &&
            wizi_to(ch, wch)
         ) ||
         (
            !IS_NPC(wch) &&
            wch->pcdata->induct <= INDUCT_APPRENTICE &&
            !IS_IMMORTAL(wch)
         )
      )
      {
         continue;
      }
      if (wch != ch)
      {
         if (buf1[0] == '\0')
         {
            continue;
         }
         if (wch->house!=HOUSE_ANCIENT || IS_IMMORTAL(wch))
         {
            if (wch->house==HOUSE_OUTLAW)
            act(buf1, ch, NULL, wch, TO_VICT);
            else
            send_to_char(buf1, wch);
         }
      }
      else
      {
         if (buf2[0] == '\0')
         {
            continue;
         }

         send_to_char(buf2, wch);
      }
   }

   return;
}

void do_rename (CHAR_DATA* ch, char* argument)
{
   char old_name[MAX_INPUT_LENGTH],
   new_name[MAX_INPUT_LENGTH],
   strsave [MAX_INPUT_LENGTH];

   CHAR_DATA* victim;
   FILE* file;

   argument = one_argument(argument, old_name); /* find new/old name
   */
   one_argument (argument, new_name);

   /* Trivial checks */
   if (!old_name[0])
   {
      send_to_char ("Rename who?\n\r", ch);
      return;
   }

   victim = get_char_world (ch, old_name);

   if (!victim)
   {
      send_to_char ("There is no such a person online.\n\r", ch);
      return;
   }

   if (IS_NPC(victim))
   {
      send_to_char ("You cannot use Rename on NPCs.\n\r", ch);
      return;
   }

   /* allow rename self new_name, but otherwise only lower level */
   if ( (victim != ch) && (get_trust (victim) >= get_trust (ch)) )
   {
      send_to_char ("You failed.\n\r", ch);
      return;
   }

   if (!victim->desc || (victim->desc->connected != CON_PLAYING) )
   {
      send_to_char("They are link-dead.\n\r", ch);
      return;
   }

   if (!new_name[0])
   {
      send_to_char ("Rename to what new name?\n\r", ch);
      return;
   }

   if (!check_parse_name(new_name))
   {
      send_to_char("That new name is illegal..\n\r", ch);
      return;
   }

   if (check_nameban(new_name, BAN_NEWBIES) ||
   check_nameban(new_name, BAN_PERMIT) ||
   check_nameban(new_name, BAN_ALL))
   {
      send_to_char ("The new name is namebanned.\n\r", ch);
      return;
   }

   /* First, check if there is a player named that off-line */
   sprintf( strsave, "%s%s.plr", PLAYER_DIR, capitalize( new_name ) );
   fclose (fpReserve); /* close the reserve file */
   file = fopen (strsave, "r"); /* attempt to to open pfile */
   if (file)
   {
      send_to_char ("A player with that name already exists.\n\r", ch)
      ;
      fclose (file);
      fpReserve = fopen( NULL_FILE, "r" );
      return;
   }
   fpReserve = fopen( NULL_FILE, "r" );  /* reopen the extra file */

   /* Check .gz file ! */
#if (!defined(machintosh) && !defined(MSDOS))
   sprintf( strsave, "%s%s%s%s.gz", PLAYER_DIR, capitalize( new_name ),
   "/", capitalize( new_name ) );
#else
   sprintf( strsave, "%s%s.plr.gz", PLAYER_DIR, capitalize( new_name ) );
#endif

   fclose (fpReserve); /* close the reserve file */
   file = fopen (strsave, "r"); /* attempt to to open pfile */
   if (file)
   {
      send_to_char ("A player with that name already exists in a compressed file.\n\r", ch);
      fclose (file);
      fpReserve = fopen( NULL_FILE, "r" );
      return;
   }
   fpReserve = fopen( NULL_FILE, "r" );  /* reopen the extra file */

   if (get_pc_world(ch, new_name, TRUE)) /* check for playing level-1 non-saved */
   {
      send_to_char ("A player with the name you specified already exists.\n\r", ch);
      return;
   }


   /* Save char and then rename and move pfile */
   save_char_obj (victim);

   free_string (victim->name);
   victim->name = str_dup (capitalize(new_name));
   /* Re-save char under new name */
   save_char_obj (victim);
   /*
   */
   /* unlink the old file */
   sprintf(strsave, "%s%s.plr", PLAYER_DIR, capitalize(old_name));
   remove(strsave);

   send_to_char ("Character renamed.\n\r", ch);

   victim->position = POS_STANDING;
   victim->on = NULL;
   act ("$n has renamed you to $N!", ch, NULL, victim, TO_VICT);

}

void do_renam(CHAR_DATA *ch, char *argument)
{
   send_to_char("If you want to RENAME an existing player your must type rename in full.\n\r", ch);
   send_to_char("rename <current name> <new name>\n\r", ch);
   return;
}

int get_old_xp(CHAR_DATA *ch)
{
   int class_mod;
   int race_mod;

   if
   (
      ch->race == grn_elf ||
      ch->race == grn_grey_elf ||
      ch->race == grn_dark_elf ||
      ch->race == grn_giant ||
      ch->race == grn_troll ||
      ch->race == grn_centaur
   )
   {
      race_mod = 500;
   }
   else if (ch->race == grn_gnome)
   {
      race_mod = 300;
   }
   else
   {
      race_mod = pc_race_table[ch->race].xpadd;
   }
   switch (ch->class)
   {
      case (CLASS_MONK):
      case (CLASS_ELEMENTALIST):
      case (CLASS_NECROMANCER):
      {
         class_mod = 300;
         break;
      }
      case (CLASS_CHANNELER):
      {
         class_mod = 200;
         break;
      }
      case (CLASS_RANGER):
      {
         class_mod = 250;
         break;
      }
      case (CLASS_NIGHTWALKER):
      case (CLASS_ANTI_PALADIN):
      {
         class_mod = 400;
         break;
      }
      case (CLASS_PALADIN):
      {
         class_mod = 500;
         break;
      }
      default:
      {
         class_mod = class_table[ch->class].xpadd;
         break;
      }
   }

   return (1500 + race_mod + class_mod);
}


void initialize_stats( CHAR_DATA *ch, DESCRIPTOR_DATA *d )
{
   char buf[MAX_STRING_LENGTH];
   int i;

   ch->perm_stat[0] = number_range(12, pc_race_table[ch->race].stats[0]);
   ch->perm_stat[1] = number_range(12, pc_race_table[ch->race].stats[1]);
   ch->perm_stat[2] = number_range(12, pc_race_table[ch->race].stats[2]);
   ch->perm_stat[3] = number_range(12, pc_race_table[ch->race].stats[3]);
   ch->perm_stat[4] = number_range(12, pc_race_table[ch->race].stats[4]);
   for ( i = 0; i < 5; i++ )
   {
      if ( ch->perm_stat[i] > get_max_train(ch, i) )
      {
         ch->perm_stat[i] = get_max_train(ch, i);
      }
   }
   write_to_buffer(d, "\n\rHow well have you trained your abilities?\n\r", 0);
   write_to_buffer(d, "\n\r-Strength affects basic muscle power.\n\r", 0);
   write_to_buffer(d, "-Intelligence influences mana usage and rate of training.\n\r", 0);
   write_to_buffer(d, "-Wisdom determines overall knowledge and ability to learn.\n\r", 0);
   write_to_buffer(d, "-Dexterity governs agility and speed.\n\r", 0);
   write_to_buffer(d, "-Constitution alters health and hardiness.\n\r", 0);
   write_to_buffer(d, "\n\rRolling......  [12-18+]:\n\r", 0);
   sprintf(buf, "\n\rStrength: %d Intelligence: %d Wisdom: %d Dexterity: %d Constitution: %d\n\rDo these reflect your training (Y/N)? ",
   ch->perm_stat[0],
   ch->perm_stat[1],
   ch->perm_stat[2],
   ch->perm_stat[3],
   ch->perm_stat[4]);
   write_to_buffer(d, buf, 0);
   return;
}

void fix_sanc_word(OBJ_DATA* fix)
{
   OBJ_DATA* obj;

   switch (fix->item_type)
   {
      case (ITEM_WAND):
      case (ITEM_STAFF):
      {
         fix->value[3]     = spell_match_num(fix->value[3]);
         break;
      }
      case (ITEM_POTION):
      case (ITEM_HERB):
      case (ITEM_PILL):
      case (ITEM_SCROLL):
      {
         fix->value[1]     = spell_match_num(fix->value[1]);
         fix->value[2]     = spell_match_num(fix->value[2]);
         fix->value[3]     = spell_match_num(fix->value[3]);
         fix->value[4]     = spell_match_num(fix->value[4]);
         break;
      }
      case (ITEM_SPELLBOOK):
      {
         fix->value[2]     = spell_match_num(fix->value[2]);
         fix->value[3]     = spell_match_num(fix->value[3]);
         fix->value[4]     = spell_match_num(fix->value[4]);
         break;
      }
      default:
      {
         break;
      }
   }
   for (obj = fix->contains; obj; obj = obj->next_content)
   {
      fix_sanc_word(obj);
   }
}

void fix_float_wield(OBJ_DATA* obj)
{
   OBJ_DATA* iobj;
   OBJ_DATA* next_obj;

   for (iobj = obj->contains; iobj; iobj = next_obj)
   {
      next_obj = iobj->next_content;
      fix_float_wield(iobj);
   }
   if
   (
      IS_SET(obj->wear_flags, ITEM_HOLD) &&
      (
         IS_SET(obj->wear_flags, ITEM_WIELD) ||
         obj->item_type == ITEM_WEAPON
      )
   )
   {
      REMOVE_BIT(obj->wear_flags, ITEM_HOLD);
      if
      (
         obj->wear_loc == WEAR_HOLD &&
         obj->carried_by
      )
      {
         unequip_char(obj->carried_by, obj);
      }
   }
}

void skill_adjust(CHAR_DATA* ch, bool version_only)
{
   DESCRIPTOR_DATA* d;
   int count = 0;
   int cnt;
   OBJ_DATA* obj;
   OBJ_DATA* next_obj;
   AFFECT_DATA* paf;
   char buf[MAX_INPUT_LENGTH];

   if (IS_NPC(ch))
   {
      return;  /* sanity check */
   }

   /* this function is to adjust skill changes on existing chars-werv */


   /*
   version change over's
   MUST SET THE ch->version to what it checks for.
   Should be in order from lowest version to highest so all get done.
   Can be removed once a higher version is added if the changes in the
   versions arent essential anymore.
   -Werv
   */
   if (ch->version < 13)
   {
      if (ch->carrying)
      for (obj = ch->carrying; obj != NULL; obj = next_obj)
      {
         next_obj = obj->next_content;
         free_string(obj->material);
         obj->material = str_dup(obj->pIndexData->material);
      }
      ch->version = 13;
   }
   if (ch->version < 14)
   {
      if (ch->house == HOUSE_OUTLAW && !IS_IMMORTAL(ch))
      {
         skill_gain(ch, "fence", 20);
      }
      ch->version = 14;
   }
   if (ch->version < 15)
   {
      if (!IS_NPC(ch) && IS_IMMORTAL(ch))
      {
         SET_BIT(ch->wiznet, WIZ_DELETES);
      }
      ch->version = 15;
   }
   if (ch->version < 16)
   {
      if (!IS_NPC(ch))
      {
         sh_int skill_num;
         for (skill_num = 0; skill_num < MAX_SKILL; skill_num++)
         {
            if
            (
               ch->pcdata->learnlvl[skill_num] == 0 &&
               skill_table[skill_num].name != NULL
            )
            {
               ch->pcdata->learnlvl[skill_num] =
               skill_table[skill_num].skill_level[ch->class];
            }
         }
      }
      ch->version = 16;
   }
   if (ch->version < 17)
   {
      /* Set up multiple descriptions for changelings */
      for (paf = ch->affected; paf != NULL; paf = paf->next)
      {
         if
         (
            paf->type == gsn_shapeshift &&
            paf->location == APPLY_MORPH_FORM &&
            paf->modifier > 0 &&
            paf->modifier < MORPH_MIMIC
         )
         {
            ch->pcdata->current_desc = paf->modifier;
            break;
         }
      }
      ch->version = 17;
   }
   if (ch->version < 18)
   {
      if
      (
         ch->class == CLASS_CHANNELER &&
         ch->pcdata->special == SUBCLASS_ENCHANTER
      )
      {
         skill_gain(ch, "armor", 1);
         skill_gain(ch, "protective shield", 17);
      }
      ch->version = 18;
   }
   if (ch->version < 19)
   {
      /* Actual changes are done in nanny */
      ch->version = 19;
   }
   if (ch->version < 20)
   {
      /*
         Changes dealt with obsolete skill command.
         Skill no longer exists.
         Code removed.
      */
      ch->version = 20;
   }
   if (ch->version < 21)
   {
      if
      (
         (IS_SET(ch->act2, PLR_IS_ANCIENT_KNOWN))
      )
      {
         skill_gain(ch, "dark focus", 10);
      }
      ch->version = 21;
   }
   if (ch->version < 22)
   {
      /* Make monks methodical */
      if (ch->class == CLASS_MONK)
      {
         ch->pcdata->ethos = 1000;
      }
      ch->version = 22;
   }
   if (ch->version < 23)
   {
      /* Give holy beacon spell */
      if (ch->class == CLASS_PALADIN)
      {
         skill_gain(ch, "holy beacon", 5);
      }
      ch->version = 23;
   }
   if (ch->version < 24)
   {
      /* Check impure crusaders */
      if (ch->house == HOUSE_CRUSADER)
      {
         impure_note(ch, "Previously Impure");
      }
      ch->version = 24;
   }
   if (ch->version < 25)
   {
      /* Clearing up non permanents */
      if
      (
         ch->pcdata->special == SUBCLASS_PYROMANCER ||
         ch->pcdata->special == SUBCLASS_AQUAMANCER
      )
      {
         skill_lose(ch, "fire and ice");
      }
      if (ch->pcdata->special == SUBCLASS_HUNTER)
      {
         skill_gain(ch, "tanning", 30);
      }
      ch->pcdata->learnlvl[gsn_call_of_duty] = 53;
      if (ch->house == HOUSE_EMPIRE)
      {
         ch->pcdata->induct = 5;
         do_induct(ch, "self none");
         ch->pcdata->induct = 0;
         ch->pcdata->autopurge = 0;
      }
      /* draco conversion */
      if
      (
         !has_skill(ch, gsn_breath_fire) &&
         ch->race == grn_draconian
      )
      {
         ch->pcdata->learnlvl[gsn_breath_fire] =  20;
         ch->pcdata->learned[gsn_breath_fire] = 100;
      }
      /* Nov 2001 */
      if (ch->pcdata->special == SUBCLASS_HIGHWAYMAN)
      {
         ch->pcdata->learnlvl[gsn_circle] = 15;
         if (ch->pcdata->learned[gsn_circle] < 1)
         ch->pcdata->learned[gsn_circle] = 75;
      }
      if
      (
         ch->class == CLASS_WARRIOR &&
         !has_skill(ch, gsn_dual_parry)
      )
      {
         skill_gain(ch, "dual parry", 22);
         skill_gain(ch, "battle tactics", 32);
         skill_gain(ch, "block retreat", 38);
      }
      if (ch->true_alignment == -5000)
      {
         ch->true_alignment = ch->alignment;
      }
      skill_lose(ch, "getaway");
      /* Outlaw powers removed since Outlaw is phasing out of the Mafia */
      if
      (
         ch->house == HOUSE_OUTLAW &&
         !IS_IMMORTAL(ch)
      )
      {
         ch->pcdata->learned[gsn_backup] = -1;
         ch->pcdata->learnlvl[gsn_backup] = 53;
         ch->pcdata->learned[gsn_mute] = -1;
         ch->pcdata->learnlvl[gsn_mute] = 53;
      }
      ch->version = 25;
   }
   if (ch->version < 26)
   {
      /* Fix with Ahzra and alignments */
      if
      (
         !(
            (
               (
                  obj = get_eq_char(ch, WEAR_WIELD)
               ) != NULL &&
               obj->pIndexData->vnum == OBJ_VNUM_GENOCIDE_AXE
            ) ||
            (
               (
                  obj = get_eq_char(ch, WEAR_DUAL_WIELD)
               ) != NULL &&
               obj->pIndexData->vnum == OBJ_VNUM_GENOCIDE_AXE
            ) ||
            IS_AFFECTED2(ch, AFF_AHZRA_BLOODSTAIN)
         )
      )
      {
         if
         (
            ch->true_alignment != -5000 &&
            ch->true_alignment != ch->alignment
         )
         {
            sprintf
            (
               log_buf,
               "%s CHANGED ALIGNMENT from %d to %d",
               ch->name,
               ch->alignment,
               ch->true_alignment
            );
            log_string(log_buf);
            ch->alignment = ch->true_alignment;
         }
         ch->true_alignment = ch->alignment;
      }
      if (ch->true_alignment != -5000)
      {
         ch->true_alignment = ch->alignment;
      }
      ch->version = 26;
   }
   if (ch->version < 27)
   {
      OBJ_DATA* armguards = get_eq_char(ch, WEAR_ARMS);

      if
      (
         armguards != NULL &&
         armguards->pIndexData->vnum == 18792
      )
      {
         affect_strip(ch, gsn_haste);
         SET_BIT(ch->affected_by, AFF_HASTE);
      }
      ch->version = 27;
   }
   if (ch->version < 28)
   {
      int cnt;
      int grp;
      int sn;
      AFFECT_DATA* paf;
      bool* learnlvl;
      int advexp_count = 0;
      bool too_many_adv_exp = FALSE;
      int house;

#define CHECK_SKILL(skill_name)  \
sn = skill_lookup(skill_name);   \
if (sn != -1)                    \
{                                \
   learnlvl[sn] = TRUE;          \
}

#define CHECK_ADV_SKILL(skill_name)          \
sn = skill_lookup(skill_name);               \
if                                           \
(                                            \
   sn != -1 &&                               \
   ch->pcdata->learnlvl[sn] <= ch->level &&  \
   ch->pcdata->learned[sn] > 0               \
)                                            \
{                                            \
   advexp_count++;                           \
   learnlvl[sn] = TRUE;                      \
}


      learnlvl = (bool*)malloc(MAX_SKILL * sizeof(bool));

      for (cnt = 0; cnt < MAX_SKILL; cnt++)
      {
         learnlvl[cnt] = FALSE;
      }
      for (grp = 0; grp < MAX_GROUP; grp++)
      {
         if (ch->pcdata->group_known[grp])
         {
            for (cnt = 0; cnt < MAX_IN_GROUP; cnt++)
            {
               if (group_table[grp].spells[cnt] == NULL)
               {
                  break;
               }
               CHECK_SKILL(group_table[grp].spells[cnt]);
            }
         }
      }
      house = ch->house;
      if
      (
         !house &&
         IS_SET(ch->act2, PLR_IS_ANCIENT)
      )
      {
         house = HOUSE_ANCIENT;
      }
      if (house)
      {
         for (cnt = 0; cnt < 99; cnt++)
         {
            if (house_table[house].skills[cnt].name == NULL)
            {
               break;
            }
            if
            (
               !IS_SET(house_table[house].skills[cnt].alignment, ALLOW_EVIL) &&
               ch->alignment < 0
            )
            {
               continue;
            }
            if
            (
               !IS_SET(house_table[house].skills[cnt].alignment, ALLOW_NEUTRAL) &&
               ch->alignment == 0
            )
            {
               continue;
            }
            if
            (
               !IS_SET(house_table[house].skills[cnt].alignment, ALLOW_GOOD) &&
               ch->alignment > 0
            )
            {
               continue;
            }
            CHECK_SKILL(house_table[house].skills[cnt].name);
         }
      }
      if
      (
         ch->pcdata->induct == 3 ||
         ch->pcdata->induct == 5
      )
      {
         if (ch->house == HOUSE_CRUSADER)
         {
            CHECK_SKILL("lore");
         }
         if (ch->house == HOUSE_ARCANA)
         {
            CHECK_SKILL("soulscry");
         }
      }
      if (IS_SET(ch->act2, PLR_IS_ANCIENT_KNOWN))
      {
         CHECK_SKILL("dark focus");
      }
      switch (ch->pcdata->special)
      {
         default:
         {
            break;
         }
         case (SUBCLASS_KNIGHT_OF_WAR):
         {
            CHECK_SKILL("frenzy");
            CHECK_SKILL("skull cleaver");
            CHECK_SKILL("unholy fire");
            CHECK_SKILL("fourth attack");
            break;
         }
         case (SUBCLASS_KNIGHT_OF_FAMINE):
         {
            CHECK_SKILL("trance");
            CHECK_SKILL("envenom");
            CHECK_SKILL("cause starvation");
            CHECK_SKILL("cause dehydration");
            CHECK_SKILL("tainted food");
            CHECK_SKILL("tainted spring");
            CHECK_SKILL("emaciation");
            CHECK_SKILL("taunt");
            CHECK_SKILL("nausea");
            break;
         }
         case (SUBCLASS_KNIGHT_OF_PESTILENCE):
         {
            CHECK_SKILL("trance");
            CHECK_SKILL("ice pox");
            CHECK_SKILL("atrophy");
            CHECK_SKILL("bane");
            CHECK_SKILL("epidemic");
            CHECK_SKILL("vermin");
            break;
         }
         case (SUBCLASS_KNIGHT_OF_DEATH):
         {
            CHECK_SKILL("trance");
            CHECK_SKILL("decay corpse");
            CHECK_SKILL("talk to dead");
            CHECK_SKILL("animate skeleton");
            CHECK_SKILL("black mantle");
            CHECK_SKILL("rot");
            break;
         }
         case (SUBCLASS_ENCHANTER):
         {
            CHECK_SKILL("make armor");
            CHECK_SKILL("fade");
            CHECK_SKILL("origin");
            CHECK_SKILL("dissolve");
            CHECK_SKILL("alchemy");
            CHECK_SKILL("obscuration");
            break;
         }
         case (SUBCLASS_ABJURER):
         {
            CHECK_SKILL("invisible mail");
            CHECK_SKILL("ray of enfeeblement");
            CHECK_SKILL("super charge");
            CHECK_SKILL("mass protection");
            CHECK_SKILL("enlarge");
            CHECK_SKILL("shrink");
            CHECK_SKILL("halo of eyes");
            CHECK_SKILL("lower resistance");
            CHECK_SKILL("raise resistance");
            break;
         }
         case (SUBCLASS_ILLUSIONIST):
         {
            CHECK_SKILL("hall of mirrors");
            CHECK_SKILL("fog of concealment");
            CHECK_SKILL("illusionary wall");
            CHECK_SKILL("toad");
            CHECK_SKILL("fear");
            CHECK_SKILL("strabismus");
            CHECK_SKILL("mirror image");
            CHECK_SKILL("fist of god");
            CHECK_SKILL("radiance");
            CHECK_SKILL("bang");
            CHECK_SKILL("visions");
            CHECK_SKILL("mass invis");
            break;
         }
         case (SUBCLASS_PRIEST_HEALING):
         {
            CHECK_SKILL("ultra heal");
            CHECK_SKILL("cleanse");
            CHECK_SKILL("health blessing");
            CHECK_SKILL("benediction");
            CHECK_SKILL("imbue regeneration");
            break;
         }
         case (SUBCLASS_TEMPORAL_MASTER):
         {
            CHECK_SKILL("mass haste");
            CHECK_SKILL("accelerate time");
            CHECK_SKILL("mass slow");
            CHECK_SKILL("know time");
            CHECK_SKILL("temporal shield");
            CHECK_SKILL("temporal shear");
            CHECK_SKILL("freeze person");
            CHECK_SKILL("distort time");
            CHECK_SKILL("time travel");
            CHECK_SKILL("grace");
            break;
         }
         case (SUBCLASS_PRIEST_PROTECTION):
         {
            CHECK_SKILL("barrier");
            CHECK_SKILL("divine protection");
            CHECK_SKILL("holyshield");
            CHECK_SKILL("phasing");
            break;
         }
         case (SUBCLASS_ZEALOT_FAITH):
         {
            CHECK_SKILL("spiritual wrath");
            CHECK_SKILL("enhanced damage");
            CHECK_SKILL("cloak of bravery");
            CHECK_SKILL("exorcism");
            break;
         }
         case (SUBCLASS_GEOMANCER):
         {
            CHECK_SKILL("earthbind");
            CHECK_SKILL("stone");
            CHECK_SKILL("stalagmite");
            CHECK_SKILL("tremor");
            CHECK_SKILL("avalanche");
            CHECK_SKILL("cave-in");
            CHECK_SKILL("crushing hands");
            CHECK_SKILL("shield of earth");
            CHECK_SKILL("stoney grasp");
            CHECK_SKILL("burrow");
            CHECK_SKILL("meteor storm");
            CHECK_SKILL("earth form");
            break;
         }
         case (SUBCLASS_PYROMANCER):
         {
            CHECK_SKILL("napalm");
            CHECK_SKILL("incinerate");
            CHECK_SKILL("sunburst");
            CHECK_SKILL("nova");
            CHECK_SKILL("inferno");
            CHECK_SKILL("flame form");
            CHECK_SKILL("imbue flame");
            CHECK_SKILL("flamestrike");
            CHECK_SKILL("fireproof");
            CHECK_SKILL("burning hands");
            CHECK_SKILL("ashes to ashes");
            CHECK_SKILL("raging fire");

            CHECK_SKILL("fireball");
            CHECK_SKILL("firestream");
            CHECK_SKILL("summon fire elemental");
            break;
         }
         case (SUBCLASS_AQUAMANCER):
         {
            CHECK_SKILL("geyser");
            CHECK_SKILL("water spout");
            CHECK_SKILL("deluge");
            CHECK_SKILL("whirlpool");
            CHECK_SKILL("blizzard");
            CHECK_SKILL("spring rains");
            CHECK_SKILL("submerge");
            CHECK_SKILL("freeze");
            CHECK_SKILL("frost charge");
            CHECK_SKILL("purify");
            CHECK_SKILL("waterwalk");
            CHECK_SKILL("water breathing");
            CHECK_SKILL("ice armor");
            CHECK_SKILL("water form");
            CHECK_SKILL("water of life");
            CHECK_SKILL("fountain of youth");
            CHECK_SKILL("rip tide");
            CHECK_SKILL("cone of cold");
            CHECK_SKILL("iceball");
            CHECK_SKILL("icelance");
            CHECK_SKILL("chill touch");
            CHECK_SKILL("summon water elemental");
            break;
         }
         case (SUBCLASS_AREOMANCER):
         {
            CHECK_SKILL("charge weapon");
            CHECK_SKILL("tornado");
            CHECK_SKILL("airshield");
            CHECK_SKILL("suffocate");
            CHECK_SKILL("jet stream");
            CHECK_SKILL("cyclone");
            CHECK_SKILL("implosion");
            CHECK_SKILL("thunder");
            CHECK_SKILL("storm");
            CHECK_SKILL("mass fly");
            CHECK_SKILL("dust devil");
            CHECK_SKILL("vortex");
            CHECK_SKILL("wraithform");
            CHECK_SKILL("lightning bolt");
            CHECK_SKILL("shocking grasp");
            CHECK_SKILL("air dagger");
            CHECK_SKILL("gale winds");
            CHECK_SKILL("summon air elemental");
            break;
         }
         case (SUBCLASS_ANATOMIST):
         {
            CHECK_SKILL("boiling blood");
            CHECK_SKILL("strengthen bone");
            CHECK_SKILL("strengthen construct");
            CHECK_SKILL("graft flesh");
            CHECK_SKILL("bone golem");
            break;
         }
         case (SUBCLASS_CORRUPTOR):
         {
            CHECK_SKILL("gout of maggots");
            CHECK_SKILL("ancient plague");
            CHECK_SKILL("atrophy");
            CHECK_SKILL("curse room");
            break;
         }
         case (SUBCLASS_NECROPHILE):
         {
            CHECK_SKILL("nether shroud");
            CHECK_SKILL("blessing of darkness");
            CHECK_SKILL("syphon soul");
            break;
         }
         case (SUBCLASS_BEASTMASTER):
         {
            CHECK_SKILL("call of the wild");
            CHECK_SKILL("beastial command");
            CHECK_SKILL("beastial stance");
            CHECK_SKILL("canopy walk");
            break;
         }
         case (SUBCLASS_BARBARIAN):
         {
            CHECK_SKILL("wild fury");
            CHECK_SKILL("two-handed wielding");
            CHECK_SKILL("toughen");
            CHECK_SKILL("crushing blow");
            CHECK_SKILL("enhanced damage");
            CHECK_SKILL("skull bash");
            CHECK_SKILL("bash");
            CHECK_SKILL("crush");
            CHECK_SKILL("battle scream");
            CHECK_SKILL("war paint");
            CHECK_SKILL("mace");
            break;
         }
         case (SUBCLASS_HUNTER):
         {
            CHECK_SKILL("forest walk");
            CHECK_SKILL("hunters knife");
            CHECK_SKILL("tracking");
            CHECK_SKILL("trap making");
            CHECK_SKILL("killer instinct");
            CHECK_SKILL("tanning");
            break;
         }
         case (SUBCLASS_NINJA):
         {
            CHECK_SKILL("caltrops");
            CHECK_SKILL("assassinate");
            CHECK_SKILL("strangle");
            CHECK_SKILL("nerve");
            CHECK_SKILL("vanish");
            CHECK_SKILL("throw");
            CHECK_SKILL("ninjitsu");
            CHECK_SKILL("endure");
            CHECK_SKILL("poison dust");
            CHECK_SKILL("critical strike");
            CHECK_SKILL("feign death");
            CHECK_SKILL("armor");
            CHECK_SKILL("invisibility");
            CHECK_SKILL("detect invis");
            CHECK_SKILL("faerie fire");
            CHECK_SKILL("word of recall");
            break;
         }
         case (SUBCLASS_HIGHWAYMAN):
         {
            CHECK_SKILL("ransack");
            CHECK_SKILL("entrench");
            CHECK_SKILL("charging retreat");
            CHECK_SKILL("lookout");
            CHECK_SKILL("gag");
            CHECK_SKILL("bind");
            CHECK_SKILL("blindfold");
            CHECK_SKILL("circle stab");
            break;
         }
         case (SUBCLASS_SWASHBUCKLER):
         {
            CHECK_SKILL("offhand disarm");
            CHECK_SKILL("tumble");
            CHECK_SKILL("rapier");
            CHECK_SKILL("thrust");
            CHECK_SKILL("lunge");
            CHECK_SKILL("hire crew");
            CHECK_SKILL("dual parry");
            CHECK_SKILL("advanced swords");
            CHECK_SKILL("expert swords");
            break;
         }
         case (SUBCLASS_SCHOOL_TIGER):
         {
            CHECK_SKILL("open claw fighting");
            CHECK_SKILL("fists of fury");
            CHECK_SKILL("pouncing tiger");
            CHECK_SKILL("spinning kick");
            CHECK_SKILL("deathstrike");
            CHECK_SKILL("eye of the tiger");
            break;
         }
         case (SUBCLASS_SCHOOL_SNAKE):
         {
            CHECK_SKILL("poison dust");
            CHECK_SKILL("fists of poison");
            CHECK_SKILL("stunning strike");
            CHECK_SKILL("dance of venom");
            CHECK_SKILL("sixth attack");
            CHECK_SKILL("dim-mak");
            break;
         }
         case (SUBCLASS_SCHOOL_DRAGON):
         {
            CHECK_SKILL("fists of fire");
            CHECK_SKILL("protection heat cold");
            CHECK_SKILL("dragon chant");
            CHECK_SKILL("iron palm");
            CHECK_SKILL("counter");
            CHECK_SKILL("dragon spirit");
            break;
         }
         case (SUBCLASS_SCHOOL_MANTIS):
         {
            CHECK_SKILL("escape");
            CHECK_SKILL("fists of divinity");
            CHECK_SKILL("counter defense");
            CHECK_SKILL("judo");
            CHECK_SKILL("knockdown");
            CHECK_SKILL("blind fighting");
            break;
         }
         case (SUBCLASS_SCHOOL_CRANE):
         {
            CHECK_SKILL("acrobatics");
            CHECK_SKILL("fists of ice");
            CHECK_SKILL("evasive dodge");
            CHECK_SKILL("jump kick");
            CHECK_SKILL("chi attack");
            CHECK_SKILL("chi healing");
            break;
         }
      }
      if (ch->class == CLASS_WARRIOR)
      {
         CHECK_ADV_SKILL("advanced swords");
         CHECK_ADV_SKILL("advanced axes");
         CHECK_ADV_SKILL("advanced daggers");
         CHECK_ADV_SKILL("advanced staffs");
         CHECK_ADV_SKILL("advanced maces");
         CHECK_ADV_SKILL("advanced whips");
         CHECK_ADV_SKILL("advanced flails");
         CHECK_ADV_SKILL("advanced pole-arms");
         CHECK_ADV_SKILL("advanced spears");
         CHECK_ADV_SKILL("expert swords");
         CHECK_ADV_SKILL("expert axes");
         CHECK_ADV_SKILL("expert daggers");
         CHECK_ADV_SKILL("expert staffs");
         CHECK_ADV_SKILL("expert maces");
         CHECK_ADV_SKILL("expert whips");
         CHECK_ADV_SKILL("expert flails");
         CHECK_ADV_SKILL("expert pole-arms");
         CHECK_ADV_SKILL("expert spears");
         if
         (
            ch->level < 20 &&
            advexp_count > 0
         )
         {
            sprintf
            (
               log_buf,
               "%s has %d advanced/expert skills before level 20!",
               ch->name,
               advexp_count
            );
            wiznet
            (
               log_buf,
               ch,
               NULL,
               WIZ_BUGS,
               0,
               0
            );
            strcat(log_buf, "\n");
            log_string(log_buf);
            too_many_adv_exp = TRUE;
         }
         else if
         (
            ch->level < 35 &&
            advexp_count > 1
         )
         {
            sprintf
            (
               log_buf,
               "%s has %d advanced/expert skills before level 35!",
               ch->name,
               advexp_count
            );
            wiznet
            (
               log_buf,
               ch,
               NULL,
               WIZ_BUGS,
               0,
               0
            );
            strcat(log_buf, "\n");
            log_string(log_buf);
            too_many_adv_exp = TRUE;
         }
         else if
         (
            ch->level < 45 &&
            advexp_count > 2
         )
         {
            sprintf
            (
               log_buf,
               "%s has %d advanced/expert skills before level 45!",
               ch->name,
               advexp_count
            );
            wiznet
            (
               log_buf,
               ch,
               NULL,
               WIZ_BUGS,
               0,
               0
            );
            strcat(log_buf, "\n");
            log_string(log_buf);
            too_many_adv_exp = TRUE;
         }
         else if
         (
            ch->level < LEVEL_IMMORTAL &&
            advexp_count > 3
         )
         {
            sprintf
            (
               log_buf,
               "%s has %d advanced/expert skills and is mortal!",
               ch->name,
               advexp_count
            );
            wiznet
            (
               log_buf,
               ch,
               NULL,
               WIZ_BUGS,
               0,
               0
            );
            strcat(log_buf, "\n");
            log_string(log_buf);
            too_many_adv_exp = TRUE;
         }
      }
      if
      (
         ch->class == CLASS_RANGER &&
         ch->race == grn_arborian
      )
      {
         CHECK_SKILL("camouflage");
      }
      for (cnt = 0; cnt < 5; cnt++)
      {
         if (pc_race_table[ch->race].skills[cnt] == NULL)
         {
            break;
         }
         sn = skill_lookup(pc_race_table[ch->race].skills[cnt]);
         learnlvl[sn] = TRUE;
      }
      if (ch->class == CLASS_MONK)
      {
         CHECK_SKILL("subrank");
      }
      paf = affect_find(ch->affected, gsn_strange_form);
      if (paf != NULL)
      {
         switch (paf->modifier)
         {
            default:
            {
               break;
            }
            case (MORPH_GOAT_HEAD):
            {
               CHECK_SKILL("ram");
               break;
            }
            case (MORPH_LION_HEAD):
            {
               CHECK_SKILL("bite");
               break;
            }
         }
      }
      for (cnt = 1; cnt < MAX_SKILL; cnt++)
      {
         if (!learnlvl[cnt])
         {
            ch->pcdata->learned[cnt] = 0;
            ch->pcdata->learnlvl[cnt] = MAX_LEVEL + 1;
         }
      }
      ch->pcdata->learnlvl[0] = 99;
      if (IS_SET(ch->comm, COMM_ANSI))
      {
         write_to_buffer
         (
            ch->desc,
            "\x01B[1;31m",
            0
         );
      }
      write_to_buffer
      (
         ch->desc,
         "\n\r"
         "\n\r"
         "\n\r"
         "IF YOU HAVE SKILLS OR SPELLS YOU KNOW YOU SHOULD NOT HAVE,\n\r"
         "OR HAVE GAINED <<ANYTHING>> YOU KNOW YOU SHOULD NOT HAVE,\n\r"
         "NOTIFY AN IMMORTAL IMMEDIATELY FOR ASSISTANCE.\n\r"
         "IF NO IMMORTAL RESPONDS, SEND A NOTE TO IMMORTAL.\n\r"
         "EXPLOITATION OF BUGS WILL RESULT IN A DENIAL.\n\r"
         "\n\r"
         "\n\r"
         "\n\r",
         0
      );
      if (too_many_adv_exp)
      {
         write_to_buffer
         (
            ch->desc,
            "\n\r"
            "\n\r"
            "\n\r"
            "YOU, YES <<<YOU>>> ARE BUGGED.  YOU HAVE TOO MANY ADVANCED OR\n\r"
            "EXPERT WEAPON SKILLS FOR YOUR LEVEL.\n\r"
            "NOTIFY AN IMMORTAL IMMEDIATELY FOR ASSISTANCE.\n\r"
            "IF NO IMMORTAL RESPONDS, SEND A NOTE TO IMMORTAL.\n\r"
            "EXPLOITATION OF BUGS WILL RESULT IN A DENIAL.\n\r"
            "THIS <HAS> BEEN LOGGED\n\r"
            "\n\r"
            "\n\r"
            "\n\r",
            0
         );
      }

      if (IS_SET(ch->comm, COMM_ANSI))
      {
         write_to_buffer
         (
            ch->desc,
            "\x01B[0;37m",
            0
         );
      }
      free(learnlvl);
      #undef CHECK_SKILL
      #undef CHECK_ADV_SKILL
      ch->version = 28;
   }
   if (ch->version < 29)
   {
      if (ch->pcdata->special == SUBCLASS_KNIGHT_OF_FAMINE)
      {
         skill_gain(ch, "esurience", 37);
      }
      ch->version = 29;
   }
   if (ch->version < 30)
   {
      OBJ_DATA* obj;

      /* fix sanc/word of recall items */
      for (obj = ch->carrying; obj; obj = obj->next_content)
      {
         fix_sanc_word(obj);
      }
      if (!is_clergy(ch))
      {
         ch->pcdata->learnlvl[gsn_chromatic_shield] = ch->pcdata->learnlvl[gsn_sanctuary];
         ch->pcdata->learned[gsn_chromatic_shield]  = ch->pcdata->learned[gsn_sanctuary];
         ch->pcdata->learnlvl[gsn_power_word_recall] = ch->pcdata->learnlvl[gsn_word_recall];
         ch->pcdata->learned[gsn_power_word_recall] = ch->pcdata->learned[gsn_word_recall];
         skill_lose(ch, "word of recall");
         skill_lose(ch, "sanctuary");
      }
      ch->version = 30;
   }
   if (ch->version < 31)
   {
      /* Place holder */
      ch->version = 31;
   }
   if (ch->version < 32)
   {
      /* Place holder */
      if
      (
         !IS_IMMORTAL(ch) &&
         ch->house == HOUSE_ENFORCER
      )
      {
         ch->pcdata->autopurge = 0;
         ch->pcdata->induct = 5;
         do_induct(ch, "self none");
         ch->pcdata->induct = 0;
         ch->pcdata->house_rank = 0;
      }
      ch->version = 32;
   }
   if (ch->version < 33)
   {
      if
      (
         ch->pcdata->special == SUBCLASS_KNIGHT_OF_FAMINE ||
         IS_SET(ch->act2, PLR_LICH) ||
         ch->race == grn_arborian
      )
      {
         ch->pcdata->condition[COND_STARVING] = 0;
         ch->pcdata->condition[COND_DEHYDRATED] = 0;
         ch->pcdata->condition[COND_THIRST] = 48;
         ch->pcdata->condition[COND_HUNGER] = 48;
      }
      /* Place holder */
      ch->version = 33;
   }
   if (ch->version < 34)
   {
      if
      (
         IS_SET(ch->act, PLR_CRIMINAL) ||
         ch->pcdata->wanteds > 0 ||
         IS_SET(ch->pcdata->saved_flags, PLR_CRIMINAL)
      )
      {
         /* Pardon everyone, no enforcer */
         REMOVE_BIT(ch->act, PLR_CRIMINAL);
         REMOVE_BIT(ch->pcdata->saved_flags, PLR_CRIMINAL);
         ch->pcdata->wanteds = 0;
         for (cnt = 0; hometown_table[cnt].name != NULL; cnt++)
         {
            if (ch->temple == hometown_table[cnt].vnum)
            {
               break;
            }
         }
         if (IS_SET(ch->comm, COMM_ANSI))
         {
            write_to_buffer
            (
               ch->desc,
               "\x01B[1;36m",
               0
            );
         }
         if (hometown_table[cnt].name == NULL)
         {
            write_to_buffer
            (
               ch->desc,
               "You have received a formal pardon from the Mayor\n\r"
               "for all of your past criminal transgressions.\n\r",
               0
            );
         }
         else
         {
            sprintf
            (
               buf,
               "You have received a formal pardon from the Mayor of %s\n\r"
               "for all of your past criminal transgressions.\n\r",
               hometown_table[cnt].name
            );
            write_to_buffer
            (
               ch->desc,
               buf,
               0
            );
         }
         if (IS_SET(ch->comm, COMM_ANSI))
         {
            write_to_buffer
            (
               ch->desc,
               "\x01B[0;37m",
               0
            );
         }
      }
      ch->version = 34;
   }
   if (ch->version < 35)
   {
      if (ch->house == HOUSE_OUTLAW)
      {
         send_to_char("The casino has been destroyed!\n\r", ch);
         ch->pcdata->autopurge = 0;
         ch->pcdata->induct = 5;
         do_induct(ch, "self none");
         ch->pcdata->induct = 0;
         ch->pcdata->house_rank = 0;
      }
      ch->version = 35;
   }
   if (ch->version < 36)
   {
      if (IS_SET(ch->act, PLR_MARKED))
      {
         REMOVE_BIT(ch->act, PLR_MARKED);
         for (cnt = 0; hometown_table[cnt].name != NULL; cnt++)
         {
            if (ch->temple == hometown_table[cnt].vnum)
            {
               break;
            }
         }
         if (IS_SET(ch->comm, COMM_ANSI))
         {
            write_to_buffer
            (
               ch->desc,
               "\x01B[1;36m",
               0
            );
         }
         sprintf
         (
            buf,
            "With all the casino records destroyed, the Mayor %s%s\n\r"
            "cannot recall if you are an enemy of freedom or not.\n\r"
            "He has decided to err on the side of less paperwork,\n\r"
            "You have been forgiven.\n\r",
            (
               hometown_table[cnt].name == NULL ?
               "" :
               "of "
            ),
            (
               hometown_table[cnt].name == NULL ?
               "" :
               hometown_table[cnt].name
            )
         );
         write_to_buffer
         (
            ch->desc,
            buf,
            0
         );
         if (IS_SET(ch->comm, COMM_ANSI))
         {
            write_to_buffer
            (
               ch->desc,
               "\x01B[0;37m",
               0
            );
         }
      }
      ch->version = 36;
   }
   if (ch->version < 37)
   {
      if (ch->house == HOUSE_VALOR)
      {
         ch->pcdata->autopurge = 0;
         /* Strip leader abilities */
         ch->pcdata->induct = 5;
         do_induct(ch, "self none");
         ch->pcdata->induct = 0;
         ch->pcdata->house_rank = 0;
      }
      ch->version = 37;
   }
   if (ch->version < 38)
   {
      if (ch->house == HOUSE_COVENANT)
      {
         if (IS_SET(ch->comm, COMM_ANSI))
         {
            write_to_buffer
            (
               ch->desc,
               "\x01B[1;36m",
               0
            );
         }
         write_to_buffer
         (
            ch->desc,
            "Commander's orders: flee to regroup, find your brethren.\n\r"
            "The Fortress is lost.\n\r",
            0
         );
         if (IS_SET(ch->comm, COMM_ANSI))
         {
            write_to_buffer
            (
               ch->desc,
               "\x01B[0;37m",
               0
            );
         }
         ch->pcdata->autopurge = 0;
         ch->pcdata->induct = 5;
         do_induct(ch, "self none");
         ch->pcdata->induct = 0;
         ch->pcdata->house_rank = 0;
      }
      REMOVE_BIT(ch->act, PLR_COVENANT_ALLY);
      ch->version = 38;
   }
   if (ch->version < 39)
   {
      if (!IS_SET(ch->comm2, COMM_SEE_SURNAME))
      {
         SET_BIT(ch->comm2, COMM_SEE_SURNAME);
         if (IS_SET(ch->comm, COMM_ANSI))
         {
            write_to_buffer
            (
               ch->desc,
               "\x01B[1;36m",
               0
            );
         }
         write_to_buffer
         (
            ch->desc,
            "You will now see surnames.\n\r"
            "See help 'surname' for more information.\n\r",
            0
         );
         if (IS_SET(ch->comm, COMM_ANSI))
         {
            write_to_buffer
            (
               ch->desc,
               "\x01B[0;37m",
               0
            );
         }
      }
      ch->version = 39;
   }
   /* Drinlinda addition as per Riallus:  Channelers get staff as a skill at
    * level 1 for all subs and nonsub
    */
   if (ch->version < 40)
   {
      if
      (
         ch->class == CLASS_CHANNELER
      )
      {
         skill_gain(ch, "staff", 1);
      }
      if
      (
         ch->class == CLASS_WARRIOR
      )
      {
         skill_gain(ch, "archery", 1);
      }
   ch->version = 40;
   }
   if (ch->version < 41)
   {
      for (cnt = 1; cnt < MAX_SKILL; cnt++)
      {
         if
         (
            cnt != gsn_subrank &&
            ch->pcdata->learnlvl[cnt] == 61 &&
            skill_table[cnt].skill_level[ch->class] != 61
         )
         {
            ch->pcdata->learnlvl[cnt] = skill_table[cnt].skill_level[ch->class];
            ch->pcdata->learned[cnt] = -1;
         }
      }
      ch->version = 41;
   }
   if (ch->version < 42)
   {
      if
      (
         ch->class == CLASS_MONK &&
         ch->pcdata->special == SUBCLASS_SCHOOL_DRAGON
      )
      {
         ch->pcdata->learnlvl[gsn_iron_scales_dragon] = ch->pcdata->learnlvl[gsn_protection_heat_cold];
         ch->pcdata->learned[gsn_iron_scales_dragon]  = ch->pcdata->learned[gsn_protection_heat_cold];
         skill_lose(ch, "protection heat cold");
      }
      ch->version = 42;
   }
   if (ch->version < 43)
   {
      if
      (
         ch->class == CLASS_MONK &&
         ch->pcdata->special == SUBCLASS_SCHOOL_MANTIS
      )
      {
         ch->pcdata->learnlvl[gsn_taichi] = ch->pcdata->learnlvl[gsn_judo];
         ch->pcdata->learned[gsn_taichi]  = ch->pcdata->learned[gsn_judo];
         skill_lose(ch, "judo");
      }
      ch->version = 43;
   }
   if (ch->version < 44)
   {
      if
      (
         ch->class == CLASS_MONK &&
         ch->pcdata->special == SUBCLASS_SCHOOL_MANTIS
      )
      {
         skill_gain(ch, "tai chi", 35);
      }
      ch->version = 44;
   }
   if (ch->version < 45)
   {
      if (ch->house == HOUSE_ENFORCER)
      {
         ch->pcdata->learnlvl[gsn_seal_of_justice] = ch->pcdata->learnlvl[gsn_emblem_law];
         ch->pcdata->learned[gsn_seal_of_justice]  = ch->pcdata->learned[gsn_emblem_law];
         skill_lose(ch, "emblem of law");
      }
      ch->version = 45;
   }
   if (ch->version < 46)
   {
      OBJ_DATA* obj;
      OBJ_DATA* next_obj;

      for (obj = ch->carrying; obj; obj = next_obj)
      {
         next_obj = obj->next_content;
         fix_float_wield(obj);
      }
      ch->version = 46;
   }
   if (ch->version < 47)
   {
      /* Perma sneak races get sneak on practice list */
      if
      (
         ch->race == grn_halfling ||
         ch->race == grn_dark_elf ||
         ch->race == grn_elf ||
         ch->race == grn_grey_elf
      )
      {
         if (ch->pcdata->learnlvl[gsn_sneak] > 20)
         {
            ch->pcdata->learnlvl[gsn_sneak] = 20;
         }
         ch->pcdata->learned[gsn_sneak] = 100;
      }
      /* archery races get it on practice list */
      if
      (
         ch->race == grn_elf ||
         ch->race == grn_grey_elf
      )
      {
         if (ch->pcdata->learnlvl[gsn_archery] > 20)
         {
            ch->pcdata->learnlvl[gsn_archery] = 20;
         }
         ch->pcdata->learned[gsn_archery] = 100;
      }
      /* Fix for dark elf archery at 100%, warriors not getting it */
      if (ch->race == grn_dark_elf)
      {
         if (ch->class == CLASS_WARRIOR)
         {
            skill_gain(ch, "archery", 1);
            if (ch->pcdata->learned[gsn_archery] > 75)
            {
               /* Remove bugged racial 100% */
               ch->pcdata->learned[gsn_archery] = 75;
            }
         }
         else
         {
            skill_lose(ch, "archery");
         }
      }
      /* Old bug with crush/crushing blow.  Fix for everyone all at once */
      if (ch->class == CLASS_WARRIOR)
      {
         if (ch->pcdata->learned[gsn_crushingblow] > 0)
         {
            ch->pcdata->learned[gsn_crush] = ch->pcdata->learned[gsn_crushingblow];
            ch->pcdata->learned[gsn_crushingblow] = 0;
            ch->pcdata->learnlvl[gsn_crushingblow] = 53;
         }
      }
      racial_skills(ch);
      ch->version = 47;
   }
   if (ch->version < 48)
   {
      if
      (
         (
            obj = get_eq_char(ch, WEAR_BRAND)
         ) != NULL &&
         obj->pIndexData->vnum == OBJ_VNUM_TWIG_BRAND
      )
      {
         if (ch->pcdata->learned[gsn_hyper] < 1)
         {
            ch->pcdata->learned[gsn_hyper] = 1;
         }
         ch->pcdata->learnlvl[gsn_hyper] = skill_table[gsn_hyper].skill_level[ch->class];
         if (ch->pcdata->learned[gsn_nap] < 1)
         {
            ch->pcdata->learned[gsn_nap] = 1;
         }
         ch->pcdata->learnlvl[gsn_nap] = skill_table[gsn_nap].skill_level[ch->class];
      }
      ch->version = 48;
   }
   if (ch->version < 49)
   {
      if
      (
         ch->pcdata->special == SUBCLASS_KNIGHT_OF_FAMINE ||
         IS_SET(ch->act2, PLR_LICH) ||
         ch->pcdata->death_status == HAS_DIED ||
         ch->race == grn_demon ||
         ch->race == grn_book
      )
      {
         ch->pcdata->condition[COND_HUNGER] = 48;
         ch->pcdata->condition[COND_THIRST] = 48;
         ch->pcdata->condition[COND_STARVING] = 0;
         ch->pcdata->condition[COND_DEHYDRATED] = 0;
      }
      else if (ch->race == grn_arborian)
      {
         ch->pcdata->condition[COND_HUNGER] = 48;
         ch->pcdata->condition[COND_STARVING] = 0;
      }
      ch->version = 49;
   }
   /*
      ***************************************
      all skill changes should use versions.
      Anything permanent goes below
      ***************************************
   */
   /* PERMANENT, skip for vesion updates only */
   if (version_only)
   {
      return;
   }
   /* PERMANENT: covenant ally flag */
   if
   (
      IS_SET(ch->act, PLR_COVENANT_ALLY) ||
      (
         ch->house != 0 &&
         covenant_allies[ch->house] == TRUE
      )
   )
   {
      SET_BIT(ch->pcdata->allied_with, powtwo(2, HOUSE_COVENANT));
   }
   /* PERMANENT: Count players */
   for (d = descriptor_list; d != NULL; d = d->next)
   {
      if (d->connected == CON_PLAYING)
      {
         count++;
      }
   }
   if (count > max_on)
   {
      max_on = count;
   }
   if (count > max_on_ever)
   {
      max_on_ever = count;
      save_globals();
   }

   /*
   PERMANENT:
   This is the autopurge function from pfiles.
   to autopurge someone from a house, change
   Autopurge 0
   to
   Autopurge 1
   in their pfile
   */
   if (ch->pcdata->autopurge)
   {
      char buf[MAX_STRING_LENGTH];
      char buf2[MAX_STRING_LENGTH];

      ch->pcdata->autopurge = 0;
      /* Strip leader abilities */
      sprintf
      (
         buf,
         "%s autopurged from %s.\n\r",
         ch->name,
         house_table[ch->house].name
      );
      ch->pcdata->induct = 5;
      do_induct(ch, "self none");
      ch->pcdata->induct = 0;
      ch->pcdata->house_rank = 0;
      sprintf(log_buf, "%s autopurged.", ch->name);
      log_string(log_buf);
      sprintf(buf2, "Autopurge Penalty: %s", ch->name);
      make_note("Darkmists", buf2, "Immortal", buf, NOTE_PENALTY);
   }

   /* PERMANENT hours fix */
   if
   (
      !IS_IMMORTAL(ch) &&
      !IS_SET(ch->act2, PLR_LICH) &&
      (
         ch->played < 0 ||
         ch->played > (3600 * 2500)
      )
   )
   {
      ch->played = 0;
      ch->pcdata->prob_time = current_time;
      ch->pcdata->probation = 0;
      ch->pcdata->death_status = 0;
      ch->pcdata->death_timer = 0;
   }
   /* PERMANENT this is a perm one cause of the skill loss for aligns - werv */
   if (ch->class == CLASS_CLERIC)
   {
      if (IS_GOOD(ch))
      {
         ch->pcdata->learnlvl[gsn_holy_fire] = 30;
         ch->pcdata->learnlvl[gsn_blade_barrier] = 53;
         ch->pcdata->learnlvl[gsn_demonfire] = 53;
      }
      if (IS_EVIL(ch))
      {
         ch->pcdata->learnlvl[gsn_demonfire] = 30;
         ch->pcdata->learnlvl[gsn_blade_barrier] = 53;
         ch->pcdata->learnlvl[gsn_holy_fire] = 53;
      }
      if (ch->alignment == 0)
      {
         ch->pcdata->learnlvl[gsn_blade_barrier] = 30;
         ch->pcdata->learnlvl[gsn_demonfire] = 53;
         ch->pcdata->learnlvl[gsn_holy_fire] = 53;
      }
   }
   /* this one is perm - werv (or until outlaw loses fence)
   If removed, make sure to set fence to 75 in act_wiz.c (do_induct) - Wicket */
   /* if (ch->house == HOUSE_OUTLAW)
   ch->pcdata->learned[skill_lookup("fence")] = 100;*/
   /* Removed untill its aproved for them to get at 100 instead of 75 */

   /* PERMANENT, do not keep allowrp when logging on */
   REMOVE_BIT(ch->wiznet, WIZ_ALLOWRP);

   /* PERMANENT: storage system - Werv */
   if (ch->pcdata->being_restored)
   {
      char strsave[MAX_INPUT_LENGTH];
      do_nameallow(ch, ch->name);
      sprintf(strsave, "%s%s.plr", STORAGE_DIR, capitalize(ch->name));
      remove(strsave);
      ch->pcdata->being_restored = FALSE;
      wiznet
      (
         "$N has returned to Thera from Storage.",
         ch,
         NULL,
         WIZ_LOGINS,
         0,
         get_trust(ch)
      );
      sprintf(log_buf, "%s restored from storage.", ch->name);
      log_string(log_buf);
      if
      (
         ch->level >= 30 &&
         !IS_IMMORTAL(ch) &&
         (
            ch->in_room == NULL ||
            !IS_SET(ch->in_room->extra_room_flags, ROOM_1212)
         ) &&
         ch->pcdata->race_lottery > 0 &&
         ch->pcdata->race_lottery < MAX_PC_RACE &&
         !ch->house &&
         !IS_SET(ch->act2, PLR_IS_ANCIENT)
      )
      {
         remove_node_for(ch->name, lottery_race_players[ch->pcdata->race_lottery]);
         add_node(ch->name, lottery_race_players[ch->pcdata->race_lottery]);
      }
      if (IS_IMMORTAL(ch))
      {
         (
            find_wizi_slot(ch->name, &ch->pcdata->worship_slot)
         )->wiz_info.stored = FALSE;
      }
      ch->pcdata->prob_time = current_time;
      ch->pcdata->probation = 0;
      /* Make sure they have nothing limited */
      strip_limits(ch->carrying);
      do_save(ch, "");
   }
   /* PERMANENT: worship system - Fizzfaldt */
   if (ch->pcdata->worship && IS_IMMORTAL(ch))
   {
      free_string(ch->pcdata->worship);
      ch->pcdata->worship = NULL;
      REMOVE_BIT(ch->comm2, COMM_WORSHIP_STOPPED);
   }
   /* PERMANENT: multi-desc system - Fizzfaldt */
   if
   (
      ch->race != grn_book &&  /* Fizzfaldt, books */
      (
         IS_IMMORTAL(ch) ||
         ch->race != grn_changeling
      )
   )
   {
      switch_desc(ch, -1);
   }
   else
   {
      if
      (
         ch->race == grn_changeling &&
         ch->pcdata->current_desc == -1
      )
      {
         switch_desc(ch, 0);
      }
   }
   /* PERMANENT: max stat changes, give trains for above max stats. */
   for (count = 0; count < 5; count++)
   {
      sh_int temp;

      if
      (
         (
            temp = ch->perm_stat[count] - get_max_train(ch, count)
         ) > 0
      )
      {
         ch->train += temp;
         ch->perm_stat[count] -= temp;
      }
   }
   /* PERMANENT: clan system */
   if
   (
      ch->pcdata->clan != 0 &&
      (
         !clan_table[ch->pcdata->clan].valid ||
         strcmp(ch->pcdata->clan_name, clan_table[ch->pcdata->clan].name)
      )
   )
   {
      /* Clan was destroyed, and a new one took that slot */
      set_clan_title(ch, ch->pcdata->clan, 0);
      clan_strip(ch, ch->pcdata->clan, TRUE, TRUE, FALSE);
   }
   /* PERMANENT: clan title system */
   if (ch->pcdata->clan != 0)
   {
      sh_int clan     = ch->pcdata->clan;
      sh_int position = get_clan_position(ch, clan);
      sh_int title    = is_clan_title(ch->pcdata->extitle, clan);

      if
      (
         (
            clan_table[clan].rank == 1 &&
            title
         ) ||
         (
            title &&
            title == CLAN_POSITION3 &&
            clan_table[clan].rank == 2
         )
      )
      {
         set_clan_title
         (
            ch,
            clan,
            0
         );
      }
      else if
      (
         title != position &&
         clan_table[clan].rank != 1
      )
      {
         set_clan_title
         (
            ch,
            clan,
            position
         );
      }
   }
   /* Changeover code from New Thalos as a hometown to Tyr-Zinet, courtesy of Wervdon */
   /* --Drinny */
   if (ch->temple == 9609)
   {
      ch->temple = 70090;
   }
   /* PERMANENT: roster system */
   remove_from_rosters(ch);
   update_roster(ch, FALSE);
   return;
}

/* This will check to see if a player is wearing any items that grant
* imm, res, or vuln flags. If so, the flags are applied to the character.
* This fixes the problem with people losing their IRV eq. affects after
* they quit and logged in again.
*/

void apply_irv_ch(CHAR_DATA *ch)
{
   AFFECT_DATA *paf;
   OBJ_DATA * obj;

   for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
   {
      if (obj->wear_loc != -1)
      {
         if (!obj->enchanted)
         for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
         {
            if (paf->location != APPLY_SPELL_AFFECT)
            switch(paf->where)
            {
               case TO_AFFECTS: break;  /* already covered somewhere */
               case TO_IMMUNE: SET_BIT(ch->imm_flags, paf->bitvector); break;
               case TO_RESIST: SET_BIT(ch->res_flags, paf->bitvector); break;
               case TO_VULN:   SET_BIT(ch->vuln_flags, paf->bitvector); break;
            }
         }
         else
         for (paf = obj->affected; paf != NULL; paf = paf->next)
         {
            if (paf->location != APPLY_SPELL_AFFECT)
            switch(paf->where)
            {
               case TO_AFFECTS: break;  /* already covered somewhere */
               case TO_IMMUNE: SET_BIT(ch->imm_flags, paf->bitvector); break;
               case TO_RESIST: SET_BIT(ch->res_flags, paf->bitvector); break;
               case TO_VULN:   SET_BIT(ch->vuln_flags, paf->bitvector); break;
            }
         }
      }
   }

   return;
}

ROOM_INDEX_DATA* hoarder_check(CHAR_DATA* ch, ROOM_INDEX_DATA* room)
{
   int days, hours, score, req;

   if (ch->pcdata->probation < 0 || ch->pcdata->prob_time < 0){
      ch->pcdata->probation = 0;
      ch->pcdata->prob_time = current_time;
      return room;
   }
   score = hoard_score(ch);
   days = (int) (current_time -ch->pcdata->prob_time)/(3600*24);
   hours = (int) (ch->pcdata->probation/3600);
   if (days < 30) return room;
   if (ch->in_room != NULL && IS_SET(ch->in_room->extra_room_flags, ROOM_1212))
   return room;  /* can't go from 1212 to hoarder land */

   if (score == 0)
   {
      ch->pcdata->prob_time = current_time;
      ch->pcdata->probation = 0;
      return room;
   }
   req = 5;
   if (score < 100) req = 5;
   if (score > 100 && score <= 150) req = 6;
   if (score > 150 && score <= 200) req = 7;
   if (score > 200 && score <= 250) req = 10;
   if (score > 250 && score <= 300) req = 12;
   if (score > 300) req = 15;
   if (hours > req)
   {
      ch->pcdata->prob_time = current_time;
      ch->pcdata->probation = 0;
      return room;
   }
   sprintf(log_buf, "%s is a hoarder.", ch->name);
   wiznet(log_buf, ch, NULL, WIZ_LOGINS, 0, get_trust(ch));
   log_string(log_buf);
   hoard_report(ch);
   room = get_room_index(1224);  /* 1224 = new hoarder land */
   ch->pcdata->prob_time = current_time;
   ch->pcdata->probation = 0;
   return room;
}

void fix_logon_time(long sec_delta)
{
   DESCRIPTOR_DATA* d;
   CHAR_DATA* ch;

   if (sec_delta == 0)
   {
      return;
   }

   for (d = descriptor_list; d; d = d->next)
   {
      if
      (
         d->original &&
         !IS_NPC(d->original)
      )
      {
         d->original->logon -= sec_delta;
      }
      if
      (
         d->character &&
         !IS_NPC(d->character)
      )
      {
         d->character->logon -= sec_delta;
      }
   }
   for (ch = char_list; ch; ch = ch->next)
   {
      if
      (
         !IS_NPC(ch) &&
         (
            ch->desc == NULL ||
            (
               ch != ch->desc->original &&
               ch != ch->desc->character
            )
         )
      )
      {
         ch->logon -= sec_delta;
      }
   }
}

void racial_skills(CHAR_DATA* ch)
{
   int type;
   int i;
   int sn;

   if (IS_NPC(ch))
   {
      return;
   }
   /* add race skills moved to here for learnlevels - wervdon */
   for (i = 0; i < 5; i++)
   {
      if (pc_race_table[ch->race].skills[i]==NULL)
      {
         break;
      }
      sn = skill_lookup(pc_race_table[ch->race].skills[i]);
      if
      (
         sn <= 0 ||
         sn >= MAX_SKILL
      )
      {
         continue;
      }
      if (ch->race == grn_book)
      {
         /* Books get racial skills at 1% */
         if (ch->pcdata->learned[sn] < 1)
         {
            ch->pcdata->learned[sn] = 1;
         }
      }
      else
      {
         ch->pcdata->learned[sn] = 100;
      }
      ch->pcdata->learnlvl[sn] = skill_table[sn].skill_level[ch->class];
      if
      (
         sn == gsn_archery &&
         ch->class == CLASS_MONK
      )
      {
         /* Monks do not get archery */
         continue;
      }
      if (ch->pcdata->learnlvl[sn] > 51)
      {
         ch->pcdata->learnlvl[sn] = 20;
      }
   }
   if (ch->pcdata->learned[gsn_recall] < 75)
   {
      ch->pcdata->learned[gsn_recall] = 75;
   }
   if (ch->pcdata->special == SUBCLASS_BARBARIAN)
   {
      if (ch->pcdata->learned[gsn_mace] < 40)
      {
         ch->pcdata->learned[gsn_mace] = 40;
      }
   }
   else if (ch->class == CLASS_MONK)
   {
      if (ch->pcdata->learned[gsn_hand_to_hand] < 40)
      {
         ch->pcdata->learned[gsn_hand_to_hand] = 40;
      }
   }
   else
   {
      for (type = 0; weapon_table[type].name != NULL; type++)
      {
         if (class_table[ch->class].weapon == weapon_table[type].vnum)
         {
            if (ch->pcdata->learned[*weapon_table[type].gsn] < 40)
            {
               ch->pcdata->learned[*weapon_table[type].gsn] = 40;
            }
            break;
         }
      }
   }
   if
   (
      ch->class == CLASS_RANGER &&
      ch->race == grn_arborian
   )
   {
      ch->pcdata->learned[gsn_camouflage] = 100;
      ch->pcdata->learnlvl[gsn_camouflage] = 1;
   }
}
