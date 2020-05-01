/* ex: set expandtab ts=3:                                                 */
/* -*- Mode: C; tab-width:3 -*-                                            */
 /***************************************************************************
 *   Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,
*
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


static const char merc_h_rcsid[] = "$Id: merc.h,v 1.373 2004/11/25 08:52:20 fizzfaldt Exp $";

/*
 * Accommodate old non-Ansi compilers.
 */
#if defined(TRADITIONAL)
#define const
#define args( list )            ( )
#define DECLARE_DO_FUN( fun )        void fun( )
#define DECLARE_SPEC_FUN( fun )        bool fun( )
#define DECLARE_SPELL_FUN( fun )    void fun( )
#define DECLARE_MOUNT_FUN( fun )    bool fun( )
#else
#define args( list )            list
#define DECLARE_DO_FUN( fun )        DO_FUN    fun
#define DECLARE_SPEC_FUN( fun )        SPEC_FUN  fun
#define DECLARE_SPELL_FUN( fun )    SPELL_FUN fun
#define DECLARE_MOUNT_FUN( fun )        MOUNT_FUN fun
#endif

/* system calls */
int remove();
int system();

/*
 * Short scalar types.
 * Diavolo reports AIX compiler has bugs with short types.
 */
#if    !defined(FALSE)
#define FALSE     0
#endif

#if    !defined(TRUE)
#define TRUE     1
#endif

#if    defined(_AIX)
#if    !defined(const)
#define const
#endif
typedef int                sh_int;
typedef int                bool;
#define unix
#else
/*
Changed to full int -Fizz
Overflow is too common, memory not that much
of an issue
typedef short int                      sh_int;
bool.. possible memory corruption?
typedef unsigned char                  bool;
*/
typedef           int                  sh_int;
typedef           int                  bool;
#endif



/*
 * Structure types.
 */
typedef struct    snoop_data           SNOOP_DATA;
typedef struct    affect_data          AFFECT_DATA;
typedef struct    area_data            AREA_DATA;
typedef struct    ban_data             BAN_DATA;
typedef struct    nameban_data         NAMEBAN_DATA;
typedef struct    sort_type            SORT_TYPE;
typedef struct    size_type            SIZE_TYPE;
typedef struct    deposit_type         DEPOSIT_TYPE;
typedef struct    buf_type             BUFFER;
typedef struct    char_data            CHAR_DATA;
typedef struct    roster_data          ROSTER;
typedef struct    descriptor_data      DESCRIPTOR_DATA;
typedef struct    exit_data            EXIT_DATA;
typedef struct    extra_descr_data     EXTRA_DESCR_DATA;
typedef struct    help_data            HELP_DATA;
typedef struct    kill_data            KILL_DATA;
typedef struct    mem_data             MEM_DATA;
typedef struct    mob_index_data       MOB_INDEX_DATA;
typedef struct    note_data            NOTE_DATA;
typedef struct    obj_data             OBJ_DATA;
typedef struct    obj_index_data       OBJ_INDEX_DATA;
typedef struct    pc_data              PC_DATA;
typedef struct    gen_data             GEN_DATA;
typedef struct    reset_data           RESET_DATA;
typedef struct    room_affect_data     ROOM_AFFECT_DATA;
typedef struct    room_index_data      ROOM_INDEX_DATA;
typedef struct    shop_data            SHOP_DATA;
typedef struct    weather_data         WEATHER_DATA;
typedef struct    mprog_list           MPROG_LIST;
typedef struct    mprog_code           MPROG_CODE;
typedef struct    mount_data           MOUNT_DATA;
typedef struct    mount_info           MOUNT_INFO;
typedef struct    message              MESSAGE;
typedef struct    mud_time             MUD_TIME;
typedef struct    moon_data            MOON_DATA;
typedef struct    food_data            FOOD_DATA;
typedef struct    spring_data          SPRING_DATA;
typedef struct    timeval              TIMEVAL;
typedef struct    book_data            BOOK_DATA;
typedef struct    book_save_data       BOOK_SAVE_DATA;
typedef struct    strip_type           STRIP_DATA;
typedef struct    magic_type           MAGIC_DATA;
typedef struct    alarm_type           ALARM_DATA;
typedef struct    list_data            LIST_DATA;
typedef struct    node_data            NODE_DATA;
typedef struct    skill_mod_type       SKILL_MOD;
typedef struct    vulgar_type          VULGAR_TYPE;
typedef struct    flag_type            FLAG_TYPE;
typedef struct    association_type     ASSOCIATION_TYPE;

/*
 * Function types.
 */
typedef    void DO_FUN    args( ( CHAR_DATA *ch, char *argument ) );
typedef bool SPEC_FUN    args( ( CHAR_DATA *ch ) );
typedef void SPELL_FUN    args( ( int sn, int level, CHAR_DATA *ch, void *vo,
                int target ) );
typedef bool MOUNT_FUN  args( ( void* mount, int trigger ) );

/* Time defines */
#define TIME_0               (1049036400)  /* Sun Mar 30 10:00:00 2003 */
#define TIME_0_ORIGINAL      (845870400)   /* Mon Oct 21 00:00:00 1996 */
#define TIME_SECOND          (1)
#define TIME_MINUTE          (TIME_SECOND * 60)
#define TIME_HOUR            (TIME_MINUTE * 60)
#define TIME_DAY             (TIME_HOUR   * 24)
#define TIME_WEEK            (TIME_DAY    * 7)
#define TIME_MONTH           (TIME_WEEK   * 4)
#define TIME_YEAR            (TIME_MONTH  * 15)
#define HOURS_PER_DAY        (24)
#define DAYS_PER_WEEK        (7)
#define DAYS_PER_MONTH       (4 * DAYS_PER_WEEK)
#define DAYS_PER_YEAR        (15 * DAYS_PER_MONTH)
#define TYPE_TIME_AFFECTS    (1)
#define TYPE_TIME_KNOW_TIME  (2)
#define TYPE_TIME_STAT       (3)
#define TYPE_TIME_IDENTIFY   (4)
/* Wizireport defines */
#define WIZI_LOGON           (0)
#define WIZI_UPDATE          (1)


#define MAX_VNUM                    2000000000
/*
 * String and memory management parameters.
 */
#define MAX_FULL_HASH               65536  /* Maximum size of hash table */
#if MAX_VNUM < MAX_FULL_HASH
#define MAX_KEY_HASH                (MAX_VNUM + 1)
#else
#define MAX_KEY_HASH                (MAX_FULL_HASH)
#endif
#define MAX_STRING_LENGTH           4608
#define MAX_INPUT_LENGTH            256
#define PAGELEN                     23
#define EXECUTABLE_SIZE             2253336 /* used with mem warning, update
                        if the code grows alot-
                    This is the size of./area/darkmists */


/*
 * Game parameters.
 * Increase the max'es if you add more of something.
 * Adjust the pulse numbers to suit yourself.
 */
#define MAX_OWNERS                  10
#define MAX_MOONS                   3
#define MAX_MOON_PHASES             8
#define MAX_SOCIALS                 256
/* X - The below define is now made to be dynamic.
#define MAX_SKILL                   719
*/
#define MAX_GROUP                   40
#define MAX_IN_GROUP                354
#define MAX_ALIAS                   12
#define MAX_CLASS                   11
#define MAX_SUBCLASS                38
#define MAX_PC_RACE                 19
#define MAX_HOUSE                   15
#define MAX_CLAN                    100
#define MAX_DAMAGE_MESSAGE          53
#define MAX_STRING_LENGTH_DO_AREA   250
#define MAX_LEVEL                   60
#define LEVEL_HERO                  (MAX_LEVEL - 9)
#define LEVEL_IMMORTAL              (MAX_LEVEL - 8)
#define DO_MARRIAGE_MINIMUM_LEVEL   25

/* Maximum number of arguments accepted by owhere/mwhere - Maelstrom */
#define MAX_TOKENS                  3
#define MAX_TOKEN_ELEMENTS          3

/* Defines for alignment management - Maelstrom */
#define MAX_ALIGN                   1000
#define MIN_ALIGN                   -1000
#define ALIGN_STEP                  ((MAX_ALIGN-MIN_ALIGN)/2)
#define MAX_ALIGN_MOD               (10 * ALIGN_STEP)

#define SPAM_PENALTY                (5 * 60)    /* 5 minutes of no learning */

#define PULSE_PER_SECOND            4
#define PULSE_VIOLENCE              (3 * PULSE_PER_SECOND)
#define PULSE_MOBILE                (4 * PULSE_PER_SECOND)
/*
Unused
#define PULSE_MUSIC                 (6 * PULSE_PER_SECOND)
*/
#define PULSE_TICK                  (40 * PULSE_PER_SECOND)
#define PULSE_AREA                  (120 * PULSE_PER_SECOND)
#define PULSE_TRACK                 (20 * PULSE_PER_SECOND)
#define PULSE_RIOT                  (2 * PULSE_PER_SECOND)
#define PULSE_MESSAGE               (3 * PULSE_PER_SECOND)

#define IMPLEMENTOR                 MAX_LEVEL
#define CREATOR                     (MAX_LEVEL - 1)
#define SUPREME                     (MAX_LEVEL - 2)
#define DEITY                       (MAX_LEVEL - 3)
#define GOD                         (MAX_LEVEL - 4)
#define IMMORTAL                    (MAX_LEVEL - 5)
#define DEMI                        (MAX_LEVEL - 6)
#define ANGEL                       (MAX_LEVEL - 7)
#define AVATAR                      (MAX_LEVEL - 8)
#define HERO                        LEVEL_HERO



/*
 * Site ban structure.
 */

#define BAN_SUFFIX                  A
#define BAN_PREFIX                  B
#define BAN_NEWBIES                 C
#define BAN_ALL                     D
#define BAN_PERMIT                  E
#define BAN_PERMANENT               F
#define BAN_IPCHECKING              G

struct mud_time
{
   int year;
   int month;
   int week;
   int day;
   int hour;
   int minute;
   int second;
};

struct    ban_data
{
    BAN_DATA *    next;
    bool    valid;
    sh_int    ban_flags;
    sh_int    level;
    char *    name;
};

struct  nameban_data
{
    NAMEBAN_DATA * next;
    bool           valid;
    sh_int         nameban_flags;
    sh_int         level;
    char *         name;
};

struct size_type
{
   char*    name;
   int      restrict_e2_bit;
};

struct sort_type
{
   char*    name;
   int      index;
};

struct deposit_type
{
   char*    name;
   int      amount;
   int      life_amount;
   time_t   login_time;
};

struct buf_type
{
    BUFFER*    next;
    bool       valid;
    sh_int     state;  /* error state of the buffer */
    sh_int     size;   /* size in bytes */
    char*      string; /* buffer's string */
};

/*
 *  new_memory stuff
*/
#define NMEM_NOTES      0
#define NMEM_BANS       1
#define NMEM_NAMEBANS   2
#define NMEM_DESCRTOR   3
#define NMEM_GEN_DATA   4
#define NMEM_EX_DESCR   5
#define NMEM_AFFECTS    6
#define NMEM_OBJECTS    7
#define NMEM_CHAR_DAT   8
#define NMEM_PC_DATA    9
#define NMEM_BUFFERS    10
#define NMEM_MOUNTS     11
#define NMEM_ALARM      12
#define NMEM_NODE       13
#define NMEM_SKILL_MOD  14

/*
 *  Note stuff
*/
#define MAX_NOTE_LENGTH 4096
bool do_note_types args( (CHAR_DATA* ch, char* argument, char** text, char* note_type) );

/*
 * Time and weather stuff.
 */
#define SUN_DARK            0
#define SUN_RISE            1
#define SUN_LIGHT            2
#define SUN_SET                3

#define SKY_CLOUDLESS            0
#define SKY_CLOUDY            1
#define SKY_RAINING            2
#define SKY_LIGHTNING            3


struct    weather_data
{
    int        mmhg;
    int        change;
    int        sky;
    int        sunlight;
};



/*
 * Connected state for a channel.
 */
#define CON_PLAYING                    0
#define CON_GET_NAME                   1
#define CON_GET_OLD_PASSWORD           2
#define CON_CONFIRM_NEW_NAME           3
#define CON_GET_NEW_PASSWORD           4
#define CON_CONFIRM_NEW_PASSWORD       5
#define CON_GET_NEW_RACE               6
#define CON_GET_NEW_SEX                7
#define CON_GET_NEW_CLASS              8
#define CON_LETS_ROLL_STATS            9
#define CON_GET_ALIGNMENT              10
#define CON_GET_ETHOS                  11
#define CON_DEFAULT_CHOICE             12
#define CON_GEN_GROUPS                 13
#define CON_PICK_WEAPON                14
#define CON_READ_IMOTD                 15
#define CON_READ_MOTD                  16
#define CON_BREAK_CONNECT              17
#define CON_GET_SUBCLASS               18
#define CON_GET_STORAGE_NAME           19
#define CON_PICK_SCALE_COLOR           20
#define CON_PICK_HUMAN_STATS           21
#define CON_GET_SURNAME                22
#define CON_CONFIRM_SURNAME            23

/*
 * Descriptor (channel) structure.
 */
struct    descriptor_data
{
    DESCRIPTOR_DATA *    next;
    SNOOP_DATA *        snoops;
    CHAR_DATA *        character;
    CHAR_DATA *        original;
    bool        valid;
    char *        host;
    unsigned long       hostip;
    sh_int        descriptor;
    sh_int        connected;
    bool        fcommand;
    char        inbuf        [4 * MAX_INPUT_LENGTH];
    char        incomm        [MAX_INPUT_LENGTH];
    char        inlast        [MAX_INPUT_LENGTH];
    int            repeat;
    char *        outbuf;
    int            outsize;
    int            outtop;
    char *        showstr_head;
    char *        showstr_point;
};



/*
 * Attribute bonus structures.
 */
struct    str_app_type
{
    sh_int    tohit;
    sh_int    todam;
    sh_int    carry;
    sh_int    wield;
};

struct    int_app_type
{
    sh_int    learn;
};

struct    wis_app_type
{
    sh_int    practice;
};

struct    dex_app_type
{
    sh_int    defensive;
    sh_int    carry;
};

struct    con_app_type
{
    sh_int    hitp;
    sh_int    shock;
};



/*
 * TO types for act.
 */
#define TO_ROOM_NA         -1  /* means to room, not arena */
#define TO_ROOM            0
#define TO_NOTVICT        1
#define TO_VICT            2
#define TO_CHAR            3
#define TO_ALL            4
#define TO_ROOM_EXTRA        5
#define    TO_ROOM_SECTOR        6
#define TO_WARPAINT         7  /* just used to show which warpaint */

/*
 * Help table types.
 */
struct    help_data
{
    HELP_DATA *    next;
    sh_int    level;
    char *    keyword;
    char *    text;
};



/*
 * Shop types.
 */
#define MAX_TRADE     5

struct    shop_data
{
    SHOP_DATA *    next;            /* Next shop in list        */
    sh_int    keeper;            /* Vnum of shop keeper mob    */
    sh_int    buy_type [MAX_TRADE];    /* Item types shop will buy    */
    sh_int    profit_buy;        /* Cost multiplier for buying    */
    sh_int    profit_sell;        /* Cost multiplier for selling    */
    sh_int    open_hour;        /* First opening hour        */
    sh_int    close_hour;        /* First closing hour        */
    OBJ_DATA*  stashed;
};



/*
 * Per-class stuff.
 */

#define MAX_GUILD     8
#define MAX_STATS     5
#define STAT_STR     0
#define STAT_INT    1
#define STAT_WIS    2
#define STAT_DEX    3
#define STAT_CON    4

struct    class_type
{
    char *    name;            /* the full name of the class */
    char     who_name    [4];    /* Three-letter name for 'who'    */
   /* sh_int      attr_prime;              Prime attribute              */
    sh_int      align;                  /* see pc_race_type */
    sh_int      xpadd;                  /* see pc_race_type */
    sh_int    weapon;            /* First weapon            */
    sh_int    guild[MAX_GUILD];    /* Vnum of guild rooms        */
    sh_int    skill_adept;        /* Maximum skill level        */
    sh_int    thac0_00;        /* Thac0 for level  0        */
    sh_int    thac0_32;        /* Thac0 for level 32        */
    sh_int    hp_min;            /* Min hp gained on leveling    */
    sh_int    hp_max;            /* Max hp gained on leveling    */
    bool    fMana;            /* Class gains mana on level    */
    char *    base_group;        /* base skills gained        */
    char *    default_group;        /* default skills gained    */
    int        restrict_r_bit;
    int        restrict_e2_bit;
};

struct    bless_weapon_type
{
   int weapon_type;
   char *format;
   char *one_hand;
   char *two_hands;
   char *name_one;
   char *name_two;
   char *name_three;
   char *material;
};

/* MOVE_CHAR flags */
#define MOVE_CHAR_FOLLOW         (A)
#define MOVE_CHAR_SILENT         (B)
#define MOVE_CHAR_FLEE           (C)
#define MOVE_CHAR_SIMULACRUM_ON  (D)
#define MOVE_CHAR_SIMULACRUM_OFF (E)

/* Save flags */
#define SAVE_ALL_PETS         (A)

/* Induct settings */
#define INDUCT_NORMAL            (0)
#define INDUCT_EMERITUS          (-1)
#define INDUCT_APPRENTICE        (-2)

/* Induct Settings for Marauder */
#define INDUCT_ROGUE             (-3)
#define INDUCT_THUG              (-4)
#define INDUCT_TRICKSTER         (-5)

/* TO_ROOM flags  */
#define TO_ROOM_INSIGHT_TRACK    (A)
#define TO_ROOM_TRAPS            (B)
#define TO_ROOM_TRACKING         (C)
#define TO_ROOM_PLAGUE           (D)
#define TO_ROOM_HOUSE_ENTRY      (E)
#define TO_ROOM_BLOODLUST        (F)
#define TO_ROOM_MUSIC            (G)
#define TO_ROOM_MOVE             (H)
#define TO_ROOM_LOOK             (I)
#define TO_ROOM_SMELLY           (J)
#define TO_ROOM_RIPTIDE          (K)
#define TO_ROOM_SIMULACRUM_OFF   (L)

/* Default TO_ROOM flags */
#define TO_ROOM_LOGIN         \
(                             \
   TO_ROOM_INSIGHT_TRACK |    \
   TO_ROOM_TRAPS |            \
   TO_ROOM_TRACKING |         \
   TO_ROOM_PLAGUE |           \
   TO_ROOM_HOUSE_ENTRY |      \
   TO_ROOM_BLOODLUST |        \
   TO_ROOM_MUSIC |            \
   TO_ROOM_SMELLY |           \
   TO_ROOM_RIPTIDE |          \
   TO_ROOM_SIMULACRUM_OFF     \
)

#define TO_ROOM_MOVE_AFTER    (TO_ROOM_LOGIN)

#define TO_ROOM_AT            (TO_ROOM_MOVE)

#define TO_ROOM_NORMAL        \
(                             \
   TO_ROOM_LOGIN |            \
   TO_ROOM_MOVE               \
)

#define TO_ROOM_MOVE_CHAR     \
(                             \
   TO_ROOM_NORMAL &           \
   (~TO_ROOM_SIMULACRUM_OFF)  \
)

/* Look flags */
#define LOOK_AUTO          0
#define LOOK_NORM          1
#define LOOK_EXAMINE       2

/* Fizzfaldt (BUK) related */
#define ID_BUK             961463022

/* Illunus related */
#define ID_ILLUNUS         961372140
/* Drinlinda related */
#define ID_DRINLINDA       1015885502

/* Yanwei (won contest of some kind) */
#define ID_YANWEI          1042621855

/* Book related */
#define BOOK_MAX_PAGES     30
#define BOOK_CLOSED        -1
#define BOOK_LOCKED        -2

/* Book related */
#define BOOK_RACE_CLOSED   0
#define BOOK_RACE_LOCKED   1

/* Book related flags */
#define BOOK_PAGES         (A)
#define BOOK_WRITABLE      (B)
#define BOOK_IMM_ONLY      (C)
#define BOOK_SAVE          (D)

/* Pen related */
#define MAX_PEN_COLOR      50
#define MAX_INKWELLS       20

/* Pen related flags */
#define PEN_FILLABLE       (A)
#define PEN_FILL_DIP       (B)  /* dip to fill instead of fill */

extern char*   pencolor_table[MAX_PEN_COLOR];
extern char*   inkwell_table[MAX_INKWELLS];

struct book_data
{
   char*             page[BOOK_MAX_PAGES];
   char*             title[BOOK_MAX_PAGES];
   BOOK_SAVE_DATA*   book_save_info;
};

struct book_save_data
{
   bool     modified[BOOK_MAX_PAGES];
};

struct alarm_type
{
   bool              valid;
   CHAR_DATA*        caster;
   ALARM_DATA*       next;
   ROOM_INDEX_DATA*  room;
};

extern    const    struct    bless_weapon_type bless_weapon_type_names           [];

#define ALLOW_GOOD      (A)
#define ALLOW_EVIL      (B)
#define ALLOW_NEUTRAL   (C)
#define ALLOW_ROGUE     (D)
#define ALLOW_THUG      (E)
#define ALLOW_TRICKSTER (F)

#define ALLOW_ALL       \
(                       \
   ALLOW_GOOD |         \
   ALLOW_EVIL |         \
   ALLOW_NEUTRAL |      \
   ALLOW_ROGUE   |      \
   ALLOW_THUG    |      \
   ALLOW_TRICKSTER      \
)

struct moon_data
{
   const char* name;
   const sh_int cycle_length;
   sh_int phase;
};

struct food_data
{
   const sh_int level;
   const sh_int mana;
   const char* god_limit;
   const char* clan_limit;
   const char* name;
   const char* short_desc;
   const char* long_desc;
   const char* material;
   const bool allow_magic;
   const sh_int align;
   const bool brand_only;
};

extern const FOOD_DATA food_table[];


struct spring_data
{
   const sh_int level;
   const sh_int mana;
   const char* god_limit;
   const char* clan_limit;
   const char* name;
   const char* liq_type;
   const char* short_desc;
   const char* long_desc;
   const char* short_desc_magic;
   const char* long_desc_magic;
   const bool allow_magic;
   const sh_int align;
   const bool brand_only;
};

extern const SPRING_DATA spring_table[];

extern MOON_DATA moon_table[MAX_MOONS];

extern const char* moon_phase[MAX_MOON_PHASES];

struct    herb_spell_type
{
   sh_int *sn;
   char *new_name;
   char *new_fade;
};

extern    const    struct    herb_spell_type herb_spells           [];
/*
* Castbits
* Used in
* obj_cast_spell_2
* do_cast_spell
* say_spell2
*/
#define OBJ_CAST_HERB               (A)
#define OBJ_CAST_MAGICAL_HERB       (B)
#define OBJ_CAST_IGNORE_NO_MAGIC    (C)

#define CAST_BIT_HERB               (A)
#define CAST_BIT_MAGICAL_HERB       (B)
#define CAST_BIT_IGNORE_NO_MAGIC    (C)
#define CAST_BIT_IGNORE_SPELLBANE   (D)
#define CAST_BIT_SUPPLICATE         (E)
#define CAST_BIT_USE_MANA           (F)
#define CAST_BIT_CHECK_SKILL        (G)
#define CAST_BIT_CHECK_POSITION     (H)
#define CAST_BIT_MOB_CAST           (I)
#define CAST_BIT_OBJ_CAST           (J)
#define CAST_BIT_CHECK_INTERRUPT    (K)
#define CAST_BIT_HUNG               (L)
#define CAST_BIT_CHECK_ALIGN        (M)
#define CAST_BIT_HOLY_LIGHT_TARGET  (N)  /* Get victim no matter what.. using ID */
#define CAST_BIT_CHECK_SHADOWSTRIKE (O)
#define CAST_BIT_SAY_WORDS          (P)
#define CAST_BIT_LAG                (Q)
#define CAST_BIT_PART_OF_SPELL      (R)
#define CAST_BIT_KEEP_LEVEL         (S)
#define CAST_BIT_KEEP_SN            (T)
#define CAST_BIT_REPEAT             (U)
#define CAST_BIT_IGNORE_ABSORB      (V)
#define CAST_BIT_IGNORE_CAN_HELP    (W)
#define CAST_BIT_IGNORE_IMPURE      (X)
#define CAST_BIT_JUST_SET_REUSE     (Y)
#define CAST_BIT_MYELL              (Z)
#define CAST_BIT_ATTACK             (aa)
#define CAST_BIT_BLOCKED_BY_TAKE    (bb)
#define CAST_BIT_NO_BLOCK_BY_TAKE   (cc)  /* Overrides CAST_BIT_BLOCKED_BY_TAKE */

#define CAST_BITS_SPELL             \
(                                   \
   CAST_BIT_USE_MANA |              \
   CAST_BIT_CHECK_SKILL |           \
   CAST_BIT_CHECK_POSITION |        \
   CAST_BIT_CHECK_INTERRUPT |       \
   CAST_BIT_CHECK_ALIGN |           \
   CAST_BIT_CHECK_SHADOWSTRIKE |    \
   CAST_BIT_SAY_WORDS |             \
   CAST_BIT_MYELL |                 \
   CAST_BIT_ATTACK |                \
   CAST_BIT_LAG |                   \
   CAST_BIT_BLOCKED_BY_TAKE         \
)

#define CAST_BITS_OBJ_CAST_SPELL    \
(                                   \
   CAST_BIT_OBJ_CAST |              \
   CAST_BIT_KEEP_LEVEL |            \
   CAST_BIT_KEEP_SN |               \
   CAST_BIT_IGNORE_IMPURE |         \
   CAST_BIT_JUST_SET_REUSE |        \
   CAST_BIT_HOLY_LIGHT_TARGET |     \
   CAST_BIT_BLOCKED_BY_TAKE         \
)

#define CAST_BITS_REUSE             \
(                                   \
   CAST_BIT_PART_OF_SPELL |         \
   CAST_BIT_KEEP_LEVEL |            \
   CAST_BIT_KEEP_SN |               \
   CAST_BIT_REPEAT |                \
   CAST_BIT_IGNORE_IMPURE |         \
   CAST_BIT_IGNORE_SPELLBANE |      \
   CAST_BIT_IGNORE_ABSORB |         \
   CAST_BIT_IGNORE_CAN_HELP |       \
   CAST_BIT_HOLY_LIGHT_TARGET       \
)

#define CAST_BITS_PRAYER            \
(                                   \
   CAST_BITS_SPELL |                \
   CAST_BIT_IGNORE_IMPURE |         \
   CAST_BIT_SUPPLICATE              \
)

#define CAST_BITS_OBJ_SPELL         \
(                                   \
   CAST_BIT_OBJ_CAST |              \
   CAST_BIT_KEEP_LEVEL |            \
   CAST_BIT_KEEP_SN |               \
   CAST_BIT_IGNORE_IMPURE |         \
   CAST_BIT_HOLY_LIGHT_TARGET |     \
   CAST_BIT_BLOCKED_BY_TAKE         \
)

#define CAST_BITS_PLR_SPELL         \
(                                   \
   CAST_BIT_KEEP_LEVEL |            \
   CAST_BIT_KEEP_SN |               \
   CAST_BIT_IGNORE_IMPURE |         \
   CAST_BIT_HOLY_LIGHT_TARGET |     \
   CAST_BIT_BLOCKED_BY_TAKE         \
)

#define CAST_BITS_PLR_SKILL         \
(                                   \
   CAST_BIT_KEEP_LEVEL |            \
   CAST_BIT_KEEP_SN |               \
   CAST_BIT_HOLY_LIGHT_TARGET |     \
   CAST_BIT_IGNORE_SPELLBANE |      \
   CAST_BIT_IGNORE_ABSORB |         \
   CAST_BIT_IGNORE_IMPURE |         \
   CAST_BIT_IGNORE_NO_MAGIC |       \
   CAST_BIT_IGNORE_CAN_HELP         \
)

#define CAST_BITS_MOB_SPELL         \
(                                   \
   CAST_BIT_MOB_CAST |              \
   CAST_BIT_KEEP_LEVEL |            \
   CAST_BIT_KEEP_SN |               \
   CAST_BIT_IGNORE_IMPURE |         \
   CAST_BIT_HOLY_LIGHT_TARGET |     \
   CAST_BIT_LAG |                   \
   CAST_BIT_CHECK_INTERRUPT |       \
   CAST_BIT_MYELL |                 \
   CAST_BIT_ATTACK |                \
   CAST_BIT_SAY_WORDS |             \
   CAST_BIT_BLOCKED_BY_TAKE         \
)

#define CAST_BITS_MOB_SKILL         \
(                                   \
   CAST_BIT_MOB_CAST |              \
   CAST_BIT_KEEP_LEVEL |            \
   CAST_BIT_KEEP_SN |               \
   CAST_BIT_IGNORE_IMPURE |         \
   CAST_BIT_HOLY_LIGHT_TARGET |     \
   CAST_BIT_IGNORE_SPELLBANE |      \
   CAST_BIT_IGNORE_ABSORB |         \
   CAST_BIT_IGNORE_IMPURE |         \
   CAST_BIT_IGNORE_CAN_HELP |       \
   CAST_BIT_LAG |                   \
   CAST_BIT_CHECK_INTERRUPT         \
)

#define CAST_BITS_MOB_PRAYER        \
(                                   \
   CAST_BITS_MOB_SPELL |            \
   CAST_BIT_SUPPLICATE |            \
   CAST_BIT_BLOCKED_BY_TAKE         \
)

/*
  {  "forest name", vnum1, vnum2, { herb1,h2,h3,h4,h5,6 },
     { food1, f2, f3, f4, f5, f6, f7, f8 },
     { wood1, w2, w3, w4, w5, w6, w7, w8 },
     { animal1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11,
       a12, a13, a14, a15, a16, a17, a18, a19, a20, a21}
*/
/* defines to simplify things */
#define HERB_ORANGE_MOSS 0
#define HERB_SCAB_WEED   1
#define HERB_BLACK_VEIN  2
#define HERB_TACKLE_BERRY 3
#define HERB_NOLBRUSH    4
#define HERB_KRULL_LEAF  5
#define HERB_MONERA_LEAF 6
#define FOOD_BERRIES     0
#define FOOD_NUTS        1
#define FOOD_ROOTS       2
#define FOOD_FRUIT       3
#define FOOD_INSECTS     4
#define FOOD_BARK        5
#define FOOD_GREENS      6
#define FOOD_MUSHROOMS   7
#define WOOD_BAMBOO      0
#define WOOD_PINE        1
#define WOOD_REDWOOD     2
#define WOOD_OAK         3
#define WOOD_MAPLE       4
#define WOOD_CHERRY      5
#define WOOD_CYPRUS      6
#define WOOD_MAGIC       7

struct forest_type
{
  char * name;
  int vnum_min;
  int vnum_max;
  int herbs[7];
  int foods[8];
  int woods[8];
  int animals[22];
};

struct warpaint_type
{
  char * name;
  int  apply_where;
  int  apply_location;
  int  apply_modifier;
  int  apply_bitvector;
  int  level;
};

struct animal_type
{
   char* name;
   int   vnum;
   int   apply_where;
   int   apply_location;
   int   apply_modifier;
   int   apply_bitvector;
   int   level;
   int   restrict_r_bit;
   int   restrict_e2_bit;
};

struct demon_type
{
  char * name;
  char * real_name;
  char * short_descr;
  char * long_descr;
  char * description;
  int  min_level;
  int  dam;
  int  hp;
  int  ac;
  long imm_flags;
  long res_flags;
  long vuln_flags;
  char * spell;
  int  id_code;
  int  apply_where;
  int  apply_location;
  int  apply_modifier;
  int  apply_bitvector;
};

/* defined demon id_codes
NOTE: THESE refer to ai's that run, they do NOT have to be unique
      to any one demon, a demon with no special ai should be set to
      id_code 0 */

#define DEMON_DRETCH     1
#define DEMON_THIEF      2
#define DEMON_AGGRESSIVE 3
#define DEMON_TENGWAR    4
#define DEMON_WIMP       5
#define DEMON_CTHUL      6
#define DEMON_DRAGKAGH   7

struct tinker_type
{
   char  *name;
   int need_ore;
   int use_weapon;
   int use_armor;
   int weapon_location;
   int weapon_modifier;
   int armor_location;
   int armor_modifier;
   int rbonus_resistance;
   int forgable;
   int forge_location;
   int forge_modifier;

};


struct hometown_type
{
   char * name;
   char * abr_name;
   bool allow_good;
   bool allow_neutral;
   bool allow_evil;
   char * race;
   int  vnum;
   int  cost;
   bool allow_creation;
};

struct material_type
{
   char * name;
   char * lump_name;
   bool is_metal;
   bool is_liquid;
   bool is_wood;
   bool is_flammable;
   bool is_stone;
   bool is_gem;
};

struct old_formula_type
{
   char * mat1;
   char * mat2;
   char * mat3;
   char * mat4;
   char * mat5;
   int  apply_where;
   int  apply_loc;
   int  apply_mod;
   int  apply_bits;
   int  apply_dur;
   int  crumble_time;
};

struct formula_type
{
   int ingredient1;
   int ingredient2;
   int ingredient3;
   int ingredient4;
   int apply_where;
   int apply_loc;
   int apply_mod;
   int apply_bits;
   int crumble_time;
   int restrict;
   int special;
};

struct item_type
{
    int        type;
    char *    name;
};

struct subrestrict_type
{
    int class;
    int subclass;
    int* racep;
};

struct weapon_type
{
    char *    name;
    sh_int    vnum;
    sh_int    type;
    sh_int    *gsn;
};

struct wiznet_type
{
    char *    name;
    long     flag;
    int        level;
};

struct attack_type
{
    char *    name;            /* name */
    char *    noun;            /* message */
    int       damage;            /* damage class */
};

struct race_type
{
   char*    name;             /* call name of the race */
   bool     pc_race;          /* can be chosen by pcs */
   long     act;              /* act bits for the race */
   long     aff;              /* aff bits for the race */
   long     aff2;             /* aff2 bits for the race */
   long     off;              /* off bits for the race */
   long     imm;              /* imm bits for the race */
   long     res;              /* res bits for the race */
   long     vuln;             /* vuln bits for the race */
   long     form;             /* default form flag for the race */
   long     parts;            /* default parts for the race */
   bool     mental;           /* Race speaks mentally */
   bool     breathes;         /* Race breathes oxygen (through water or air, etc..) */
   sh_int*  pgrn;             /* Pointer to associated grn    */
};


struct pc_race_type  /* additional data for pc races */
{
   char*    name;            /* MUST be in race_type */
   char     who_name[6];
   sh_int   align;                  /* alignments race can have: 0=gne 1=gn 2=ne 3=g 4=n 5=e 6=ge*/
   sh_int    xpadd;                  /* extra xp per lvl needed */
   /*sh_int      points;                  cost in points of the race */
   /*sh_int      class_mult[MAX_CLASS];   exp multiplier for class, * 100 */
   char*    skills[5];        /* bonus skills for the race */
   sh_int   classes[11];         /* class restricts for races */
   sh_int   stats[MAX_STATS];    /* starting stats */
   sh_int   max_stats[MAX_STATS];    /* maximum stats */
   sh_int   size;            /* aff bits for the race */
   int      dam_type;
   int      restrict_r_bit;
   int      restrict_e2_bit;
};


struct spec_type
{
    char *     name;            /* special function name */
    SPEC_FUN *    function;        /* the function */
};

struct mount_info
{
    MOUNT_INFO* next;
    sh_int    mount_type;
    void*    mount;
    MOUNT_FUN*  mount_function;
    bool    valid;
};

struct mount_data
{
    int        move;
    int        mount_info_flags;    /* Strictly internal */
    int        mobility;
    long    move_flags;
};

/* Used for messaging system - Mael */
struct message
{
    void*    target;
    sh_int    target_type;
    long int    message_type;
    long int    message_data;
    char*    message_string;
    long int    message_flags;

/*  These are incorporated into the message_flags now - Mael
    bool    save;
*/

    int        time_to_live;
    bool    valid;
    MESSAGE*    next;
    MESSAGE*    next_to_poll;
};


/*
 * Data structure for notes.
 */

#define NOTE_NOTE       0
#define NOTE_IDEA       1
#define NOTE_PENALTY    2
#define NOTE_NEWS       3
#define NOTE_CHANGES    4
#define NOTE_HELP       5
#define NOTE_HOARDER    6
#define NOTE_CLAN       7
#define NOTE_BANK       8
#define NOTE_DEATH      9
#define NOTE_CRIM       10
#define MAX_NOTE_TYPE   11

struct    note_data
{
    NOTE_DATA* next;
    bool       valid;
    sh_int     type;
    char *     sender;
    char *     date;
    char *     to_list;
    char *     subject;
    char *     text;
    time_t     date_stamp;
    time_t     id;
    long       id_sender;
};

NOTE_DATA *help_list; /* its here so it can be used in act_info.c and note.c - Werv */

/*
 *  Dynamic snooping - Wervdon
 */
struct  snoop_data
{
 DESCRIPTOR_DATA * snoop_by;
 SNOOP_DATA * next;
};

#define AFFECT_CURRENT_VERSION 2  /* The newest version of affects */
/*
 * An affect.
 */
struct    affect_data
{
    AFFECT_DATA *    next;
    bool        valid;
    sh_int        where;
    sh_int        type;
    sh_int        level;
    sh_int        duration;
    sh_int        location;
    sh_int        modifier;
    int                    bitvector;
    int           bitvector2;
    char *        caster;
};

/*
 * An affect for rooms - different in that they are globally linked since
 * rooms arent.
 */
struct room_affect_data
{
    ROOM_AFFECT_DATA *    next;
    ROOM_AFFECT_DATA *  next_global;
    ROOM_INDEX_DATA  *  room;
    CHAR_DATA*        caster;
    bool        valid;
    sh_int        where;
    sh_int        type;
    sh_int        level;
    sh_int              modifier;
    sh_int        duration;
    sh_int        sector_old;
    sh_int        sector_new;
    int                    bitvector;
};

/* where definitions */
#define TO_AFFECTS   0
#define TO_OBJECT    1
#define TO_IMMUNE    2
#define TO_RESIST    3
#define TO_VULN      4
#define TO_WEAPON    5

#define HOUSE_ANCIENT   1
#define HOUSE_LIGHT     2
#define HOUSE_ARCANA    3
#define HOUSE_CRUSADER  4
#define HOUSE_ENFORCER  5
#define HOUSE_OUTLAW    6
#define HOUSE_VALOR     7
#define HOUSE_EMPIRE    8
#define HOUSE_BRETHREN  9
#define HOUSE_SCHOLAR  10
#define HOUSE_LIFE     11
#define HOUSE_COVENANT 12
#define HOUSE_CONCLAVE 13
#define HOUSE_MARAUDER 14

/* Covenant ranks */
#define RANK_SQUIRE                0
#define RANK_FOOTMAN               1
#define RANK_MASTERFOOTMAN         2
#define RANK_CORPORAL              3
#define RANK_MASTERCORPORAL        4
#define RANK_SPECIALIST            5
#define RANK_MASTERSPECIALIST      6
#define RANK_SERGEANT              7
#define RANK_MASTERSERGEANT        8
#define RANK_SERGEANTMAJOR         9
#define RANK_WARRANTOFFICER       10
#define RANK_MASTERWARRANTOFFICER 11
#define RANK_CHIEFWARRANTOFFICER  12
#define RANK_SECONDLIEUTENANT     13
#define RANK_LIEUTENANT           14
#define RANK_CAPTAIN              15
#define RANK_MAJOR                16
#define RANK_LIEUTENANTCOLONEL    17
#define RANK_COLONEL              18
#define RANK_BRIGADIERGENERAL     19
#define RANK_MAJORGENERAL         20
#define RANK_LIEUTENANTGENERAL    21
#define RANK_GENERAL              22
/*
 * A kill structure (indexed by level).
 */
struct    kill_data
{
    sh_int        number;
    sh_int        killed;
};

/* Guard Types */
#define GUARD_NORMAL             0
#define GUARD_DEFENDER           1
#define GUARD_MAGUS              2
#define GUARD_CURATE             3
#define GUARD_SENTINEL           4
#define GUARD_SENTRY             5


/***************************************************************************
 *                                                                         *
 *                   VALUES OF INTEREST TO AREA BUILDERS                   *
 *                   (Start of section ... start here)                     *
 *                                                                         *
 ***************************************************************************/

/*
 * Well known mob virtual numbers.
 * Defined in #MOBILES.
 */
#define MOB_VNUM_DOPPLEGANGER    1
#define MOB_VNUM_FIDO           3090
#define MOB_VNUM_CITYGUARD       3060
#define MOB_VNUM_VAMPIRE       3404

/* skills.are mobs */
#define MOB_VNUM_ZOMBIE           2901
#define MOB_VNUM_FIRE_ELEMENTAL       2902
#define MOB_VNUM_WATER_ELEMENTAL   2903
#define MOB_VNUM_VOID_ELEMENTAL       2931
#define MOB_VNUM_AIR_ELEMENTAL       2904
#define MOB_VNUM_EARTH_ELEMENTAL   2905
#define MOB_VNUM_PIRATE_CREW     2957
#define MOB_VNUM_FALCON           2906
#define MOB_VNUM_WOLF           2907
#define MOB_VNUM_BEAR           2908
#define MOB_VNUM_LION           2909
#define MOB_VNUM_ENFORCER_GUARD  2910
#define MOB_VNUM_COVENANT_AUXILIARY    2959
#define MOB_VNUM_COVENANT_NIGHTMARE 2960
#define MOB_VNUM_COVENANT_DRAGON    2958
#define MOB_VNUM_L_GOLEM       2911
#define MOB_VNUM_UNICORN       2912
#define MOB_VNUM_DRAGON              2913
#define MOB_VNUM_SHADOW             2914
#define MOB_VNUM_MIRROR             2962
#define MOB_VNUM_DECOY              2916
#define MOB_VNUM_G_GOLEM       2915
#define MOB_VNUM_PHANTOM         2961
#define MOB_VNUM_ARMOR           2917
#define MOB_VNUM_WEAPON           2918
#define MOB_VNUM_KEY           2919
#define MOB_VNUM_SLAYER           2920
#define MOB_VNUM_SKELETON        2921
#define MOB_VNUM_MUMMY            2922
#define MOB_VNUM_NIGHTFALL         2923
#define MOB_VNUM_OUTLAW_GIANT      2924
#define MOB_VNUM_GNOME_SPY         2925
#define MOB_VNUM_VERMIN            2926
#define MOB_VNUM_SPORE             2927
#define MOB_VNUM_STEED           2928
#define MOB_VNUM_STEED_PEGASUS       2932
#define MOB_VNUM_B_GOLEM       2930
#define MOB_VNUM_ABYSS_DEMON       2933
#define MOB_VNUM_PROTECTOR       23600
#define MOB_VNUM_BRETHREN_STATUE        1001
#define MOB_VNUM_ARCANA_STATUE        4403
#define MOB_VNUM_ANCIENT_STATUE        3806
#define MOB_VNUM_LIGHT_STATUE        4502
#define MOB_VNUM_CRUSADER_STATUE    5703
#define MOB_VNUM_OUTLAW_STATUE        0
#define MOB_VNUM_MARAUDER_STATUE        9803
#define MOB_VNUM_ENFORCER_STATUE   23602
#define MOB_VNUM_EMPIRE_STATUE     0
#define MOB_VNUM_VALOR_STATUE       1903
#define MOB_VNUM_COVENANT_STATUE      1507
#define MOB_VNUM_CONCLAVE_STATUE    21503
#define OBJ_VNUM_MARAUDER_TRAPDOOR  30200

#define MOB_VNUM_BRETHREN          1000
#define MOB_VNUM_ARCANA              4401
#define MOB_VNUM_ANCIENT       3801
#define MOB_VNUM_LIGHT           4500
#define MOB_VNUM_CRUSADER        5701
#define MOB_VNUM_OUTLAW           0
#define MOB_VNUM_MARAUDER           9800
#define MOB_VNUM_ENFORCER         23600
#define MOB_VNUM_EMPIRE             0
#define MOB_VNUM_COVENANT           1500
#define MOB_VNUM_VALOR            1901
#define MOB_VNUM_CONCLAVE           21500

#define MOB_VNUM_EMPIRE_G          2929
#define MOB_VNUM_PATROLMAN       2106
#define GROUP_VNUM_TROLLS       2100
#define GROUP_VNUM_OGRES       2101
#define MOB_VNUM_HYDRA         13724
#define MOB_VNUM_BONES        13725
#define MOB_VNUM_DRANNOR_LADY    13726
#define MOB_VNUM_DRANNOR_KNIGHT 13730
#define MOB_VNUM_DRAGONMOUNT    22667
#define MOB_VNUM_GUARDIAN_ANGEL 2956
#define MOB_VNUM_CRUSADER_DRUID 5714


/* demon house mobs - not used*/
#define MOB_VNUM_DEMON1        2933
#define MOB_VNUM_DEMON2        2934
#define MOB_VNUM_DEMON3        2935
#define MOB_VNUM_DEMON4        2936
#define MOB_VNUM_DEMON5        2937
#define MOB_VNUM_DEMON6        2938

/* Arkham Legion vnums */
#define MOB_VNUM_DEMOGORGON    29536
#define MOB_VNUM_SHADOWLORD    29535

/* Exterminators */
#define MOB_VNUM_EXTERMINATOR_1  3037
#define MOB_VNUM_EXTERMINATOR_2  9604
#define MOB_VNUM_EXTERMINATOR_3  10657


/* RT ASCII conversions -- used so we can have letters in this file */

#define A              1
#define B            2
#define C            4
#define D            8
#define E            16
#define F            32
#define G            64
#define H            128

#define I            256
#define J            512
#define K                1024
#define L             2048
#define M            4096
#define N             8192
#define O            16384
#define P            32768

#define Q            65536
#define R            131072
#define S            262144
#define T            524288
#define U            1048576
#define V            2097152
#define W            4194304
#define X            8388608

#define Y            16777216
#define Z            33554432
#define aa            67108864     /* doubled due to conflicts */
#define bb            134217728
#define cc            268435456
#define dd            536870912
#define ee            1073741824
#define ff             2147483648


/*
 * ACT bits for mobs.
 * Used in #MOBILES.
 */
#define ACT_IS_NPC            (A)             /* Auto set for mobs     */
#define ACT_SENTINEL          (B)             /* Stays in one room     */
#define ACT_SCAVENGER         (C)             /* Picks up objects      */
#define ACT_HOUSE_MOB         (D)             /* Attacks non-house PC's */
#define ACT_AGGRESSIVE        (F)             /* Attacks PC's          */
#define ACT_STAY_AREA         (G)             /* Won't leave area      */
#define ACT_WIMPY             (H)
#define ACT_PET               (I)             /* Auto set for pets     */
#define ACT_TRAIN             (J)             /* Can train PC's        */
#define ACT_PRACTICE          (K)             /* Can practice PC's     */
#define ACT_HOLYLIGHT         (N)
#define ACT_UNDEAD            (O)
#define ACT_CLERIC            (Q)
#define ACT_MAGE              (R)
#define ACT_THIEF             (S)
#define ACT_WARRIOR           (T)
#define ACT_NOALIGN           (U)
#define ACT_NOPURGE           (V)
#define ACT_OUTDOORS          (W)
#define ACT_INDOORS           (Y)
#define ACT_QUESTOR           (Z)
#define ACT_IS_HEALER        (aa)
#define ACT_GAIN             (bb)
#define ACT_UPDATE_ALWAYS    (cc)
#define ACT_IS_CHANGER       (dd)
#define ACT_BANKER           (ee)

/*
 * ACT2 bits for mobs.
 */
#define ACT_RIDEABLE          (A)
#define ACT_NO_STEAL          (B)
#define ACT_NO_DEATHSTRIKE    (C)
#define ACT_RANGER            (D)
#define ACT_INTELLIGENT       (E)
#define ACT_STRAY             (F)  /* Follows anyone who gives it food */
#define ACT_NICE_STRAY        (G)  /* Along with stray, does not attack ever */
#define ACT_PURE_GUILDMASTER  (H)  /* Does not train spells */
#define ACT_NO_TAME           (I)  /* Makes taming impossible */
#define ACT_NO_SEIZE          (J)  /* Makes seizing impossible */

/* damage classes */
#define DAM_NONE                0
#define DAM_BASH                1
#define DAM_PIERCE              2
#define DAM_SLASH               3
#define DAM_FIRE                4
#define DAM_COLD                5
#define DAM_LIGHTNING           6
#define DAM_ACID                7
#define DAM_POISON              8
#define DAM_NEGATIVE            9
#define DAM_HOLY                10
#define DAM_ENERGY              11
#define DAM_MENTAL              12
#define DAM_DISEASE             13
#define DAM_DROWNING            14
#define DAM_LIGHT        15
#define DAM_OTHER               16
#define DAM_HARM        17
#define DAM_CHARM        18
#define DAM_SOUND        19
#define DAM_IRON                20
#define DAM_WOOD                21
#define DAM_SILVER              22
#define DAM_METAL               23
#define DAM_SPELLS              24
#define DAM_NATURE              25

/* OFF bits for mobiles */
#define OFF_AREA_ATTACK         (A)
#define OFF_BACKSTAB            (B)
#define OFF_BASH                (C)
#define OFF_BERSERK             (D)
#define OFF_DISARM              (E)
#define OFF_DODGE               (F)
#define OFF_FADE                (G)
#define OFF_FAST                (H)
#define OFF_KICK                (I)
#define OFF_KICK_DIRT           (J)
#define OFF_PARRY               (K)
#define OFF_RESCUE              (L)
#define OFF_TAIL                (M)
#define OFF_TRIP                (N)
#define OFF_CRUSH        (O)
#define ASSIST_ALL           (P)
#define ASSIST_ALIGN            (Q)
#define ASSIST_RACE                 (R)
#define ASSIST_PLAYERS          (S)
#define ASSIST_GUARD            (T)
#define ASSIST_VNUM        (U)
#define NO_TRACK        (V)
#define STATIC_TRACKING        (W)
#define SPAM_MURDER        (X)
/* #define OFF_INTIMIDATED         (Y) - no longer used Werv */
#define OFF_UNDEAD_DRAIN    (Z)    /* True undead drain, very powerful */
#define OFF_LUNGE           (aa)
#define OFF_PUGIL           (bb)
#define OFF_KILLER          (cc)
#define OFF_FAST_TRACK      (dd)
#define OFF_NINJITSU        (ee)

/* return values for check_imm */
#define IS_NORMAL        0
#define IS_IMMUNE        1
#define IS_RESISTANT        2
#define IS_VULNERABLE        3

/* IMM bits for mobs */
#define IMM_SUMMON              (A)
#define IMM_CHARM               (B)
#define IMM_MAGIC               (C)
#define IMM_WEAPON              (D)
#define IMM_BASH                (E)
#define IMM_PIERCE              (F)
#define IMM_SLASH               (G)
#define IMM_FIRE                (H)
#define IMM_COLD                (I)
#define IMM_LIGHTNING           (J)
#define IMM_ACID                (K)
#define IMM_POISON              (L)
#define IMM_NEGATIVE            (M)
#define IMM_HOLY                (N)
#define IMM_ENERGY              (O)
#define IMM_MENTAL              (P)
#define IMM_DISEASE             (Q)
#define IMM_DROWNING            (R)
#define IMM_LIGHT               (S)
#define IMM_SOUND               (T)
#define IMM_HARM                (V)
#define IMM_WOOD                (X)
#define IMM_SILVER              (Y)
#define IMM_IRON                (Z)
#define IMM_METAL               (aa)
#define IMM_SPELLS              (bb)
#define IMM_NATURE              (cc)

/* RES bits for mobs */
#define RES_SUMMON        (A)
#define RES_CHARM        (B)
#define RES_MAGIC               (C)
#define RES_WEAPON              (D)
#define RES_BASH                (E)
#define RES_PIERCE              (F)
#define RES_SLASH               (G)
#define RES_FIRE                (H)
#define RES_COLD                (I)
#define RES_LIGHTNING           (J)
#define RES_ACID                (K)
#define RES_POISON              (L)
#define RES_NEGATIVE            (M)
#define RES_HOLY                (N)
#define RES_ENERGY              (O)
#define RES_MENTAL              (P)
#define RES_DISEASE             (Q)
#define RES_DROWNING            (R)
#define RES_LIGHT               (S)
#define RES_SOUND               (T)
#define RES_HARM                (V)
#define RES_WOOD                (X)
#define RES_SILVER              (Y)
#define RES_IRON                (Z)
#define RES_METAL               (aa)
#define RES_SPELLS              (bb)
#define RES_NATURE              (cc)

/* VULN bits for mobs */
#define VULN_SUMMON        (A)
#define VULN_CHARM        (B)
#define VULN_MAGIC              (C)
#define VULN_WEAPON             (D)
#define VULN_BASH               (E)
#define VULN_PIERCE             (F)
#define VULN_SLASH              (G)
#define VULN_FIRE               (H)
#define VULN_COLD               (I)
#define VULN_LIGHTNING          (J)
#define VULN_ACID               (K)
#define VULN_POISON             (L)
#define VULN_NEGATIVE           (M)
#define VULN_HOLY               (N)
#define VULN_ENERGY             (O)
#define VULN_MENTAL             (P)
#define VULN_DISEASE            (Q)
#define VULN_DROWNING           (R)
#define VULN_LIGHT              (S)
#define VULN_SOUND              (T)
#define VULN_HARM               (V)
#define VULN_WOOD               (X)
#define VULN_SILVER             (Y)
#define VULN_IRON               (Z)
#define VULN_METAL              (aa)
#define VULN_SPELLS             (bb)
#define VULN_NATURE             (cc)

/* body form */
#define FORM_EDIBLE             (A)
#define FORM_POISON             (B)
#define FORM_MAGICAL            (C)
#define FORM_INSTANT_DECAY      (D)
#define FORM_OTHER              (E)  /* defined by material bit */

/* actual form */
#define FORM_ANIMAL             (G)
#define FORM_SENTIENT           (H)
#define FORM_UNDEAD             (I)
#define FORM_CONSTRUCT          (J)
#define FORM_MIST               (K)
#define FORM_INTANGIBLE         (L)

#define FORM_BIPED              (M)
#define FORM_CENTAUR            (N)
#define FORM_INSECT             (O)
#define FORM_SPIDER             (P)
#define FORM_CRUSTACEAN         (Q)
#define FORM_WORM               (R)
#define FORM_BLOB        (S)

#define FORM_MAMMAL             (V)
#define FORM_BIRD               (W)
#define FORM_REPTILE            (X)
#define FORM_SNAKE              (Y)
#define FORM_DRAGON             (Z)
#define FORM_AMPHIBIAN          (aa)
#define FORM_FISH               (bb)
#define FORM_COLD_BLOOD        (cc)
#define FORM_PLANT              (dd)

/* body parts */
#define PART_HEAD               (A)
#define PART_ARMS               (B)
#define PART_LEGS               (C)
#define PART_HEART              (D)
#define PART_BRAINS             (E)
#define PART_GUTS               (F)
#define PART_HANDS              (G)
#define PART_FEET               (H)
#define PART_FINGERS            (I)
#define PART_EAR                (J)
#define PART_EYE                (K)
#define PART_LONG_TONGUE        (L)
#define PART_EYESTALKS          (M)
#define PART_TENTACLES          (N)
#define PART_FINS               (O)
#define PART_WINGS              (P)
#define PART_TAIL               (Q)
/* for combat */
#define PART_CLAWS              (U)
#define PART_FANGS              (V)
#define PART_HORNS              (W)
#define PART_SCALES             (X)
#define PART_TUSKS              (Y)
/* Added by Wicket */
#define PART_STALKS             (aa)
#define PART_ROOTS              (bb)

#define MORPH_RED         1
#define MORPH_BLACK       2
#define MORPH_BLUE        3
#define MORPH_WHITE       4
#define MORPH_GREEN       5
#define MORPH_GOAT_HEAD   6
#define MORPH_LION_HEAD   7
#define MORPH_DRAGON_HEAD 8
#define MORPH_CLOAK       9
#define MORPH_ARM        10
#define MORPH_BAT        11
#define MORPH_WOLF       12
#define MORPH_BEAR       13
#define MORPH_GASEOUS    14
#define MORPH_MIMIC      15
#define MORPH_BABOON     16
#define MORPH_WINGED     17
#define MORPH_ARCHANGEL  18
#define MORPH_DRACOLICH  19
#define MORPH_MAX        19
/*
 *  Description Stuff
*/
#define MAX_DOSSIER_LENGTH 16384
#define MAX_DESC_LENGTH 2048
#define MAX_DESCRIPTIONS MORPH_MAX + 1
/* Allow description for every form */
void do_desc_types args( (CHAR_DATA* ch, char* argument, char** desc, char* prefix) );


/*
 * Bits for 'affected_by'.
 * Used in #MOBILES.
 */
#define AFF_BLIND            (A)
#define AFF_INVISIBLE        (B)
#define AFF_DETECT_EVIL      (C)
#define AFF_DETECT_INVIS     (D)
#define AFF_DETECT_MAGIC     (E)
#define AFF_DETECT_HIDDEN    (F)
#define AFF_DETECT_GOOD      (G)
#define AFF_SANCTUARY        (H)
#define AFF_FAERIE_FIRE      (I)
#define AFF_INFRARED         (J)
#define AFF_CURSE            (K)
#define AFF_ACUTE_VISION     (L)
#define AFF_POISON           (M)
#define AFF_PROTECT_EVIL     (N)
#define AFF_PROTECT_GOOD     (O)
#define AFF_SNEAK            (P)
#define AFF_HIDE             (Q)
#define AFF_SLEEP            (R)
#define AFF_CHARM            (S)
#define AFF_FLYING           (T)
#define AFF_PASS_DOOR        (U)
#define AFF_HASTE            (V)
#define AFF_CALM             (W)
#define AFF_PLAGUE           (X)
#define AFF_WEAKEN           (Y)
#define AFF_DARK_VISION      (Z)
#define AFF_BERSERK          (aa)
#define AFF_SWIM             (bb)
#define AFF_REGENERATION     (cc)
#define AFF_SLOW             (dd)
#define AFF_CAMOUFLAGE       (ee)

/* bits for affected_by2 -Werv */
/* also used for bitvector2 in affects */
#define AFF_WIZI                 (A)
#define AFF_AHZRA_BLOODSTAIN     (B)   /* Used for Rantialen's Ahzra, do not use for building */
#define AFF_HERB_SPELL           (C)   /* Used for herbs as source of affects.  do not use */
#define AFF_NO_SLEEP             (D)   /* Cannot sleep by natural means. */
#define AFF_NO_REST              (E)   /* Cannot rest by natural means. */
#define AFF_SUPPLICATE           (F)   /* Cast by clergy or immortal */
#define AFF_FEIGN_DEATH          (G)   /* Only used for feign death skill */
#define AFF_INTANGIBILITY        (H)   /* Only used for intangibility spell */
#define AFF_RIPTIDE              (I)   /* Only used for riptide spell */
#define AFF_BLINDFOLD            (J)   /* Only used for blindfold skill */
#define AFF_GAG                  (K)   /* Only used for gag skill */
#define AFF_BIND                 (L)   /* Only used for bind skill */
#define AFF_HANG                 (M)   /* Only used for hang skill */
#define AFF_AWARENESS            (N)   /* Only used for awareness skill and racial awareness */
#define AFF_HYPER                (O)   /* Only used for hyper skill */
#define AFF_NAP                  (P)   /* Only used for nap skill */

/*
 * Sex.
 * Used in #MOBILES.
 */
#define SEX_NEUTRAL              0
#define SEX_MALE              1
#define SEX_FEMALE              2

/* AC types */
#define AC_PIERCE            0
#define AC_BASH                1
#define AC_SLASH            2
#define AC_EXOTIC            3

/* dice */
#define DICE_NUMBER            0
#define DICE_TYPE            1
#define DICE_BONUS            2

/* size */
/* Must go from 0 to SIZE_MAXIMUM, go in order of size.. (higher # = bigger) */
#define SIZE_TINY          0
#define SIZE_SMALL         1
#define SIZE_MEDIUM        2
#define SIZE_LARGE         3
#define SIZE_HUGE          4
#define SIZE_GIANT         5
#define SIZE_MAXIMUM      SIZE_GIANT
#define SIZE_MINIMUM      SIZE_TINY

extern      const    struct   size_type         size_table[];

/*
 * Well known object virtual numbers.
 * Defined in #OBJECTS.
 */
#define OBJ_VNUM_ALCHEMY_BOOK_CONTAINER   2955
#define OBJ_VNUM_ROPE            2947
#define OBJ_VNUM_STRAP           2946
#define OBJ_VNUM_OUTLAW            0
#define OBJ_VNUM_MARAUDER            9800
#define OBJ_VNUM_ENFORCER        23600
#define OBJ_VNUM_ENFORCER_KEY        23601
#define OBJ_VNUM_CRUSADER        5703
#define OBJ_VNUM_BRETHREN           1000
#define OBJ_VNUM_ARCANA              4403
#define OBJ_VNUM_ANCIENT        3803
#define OBJ_VNUM_LIGHT            4501
#define OBJ_VNUM_EMPIRE             0
#define OBJ_VNUM_VALOR            1903
#define OBJ_VNUM_COVENANT          1502
#define OBJ_VNUM_CONCLAVE           21502

#define KEY_BRETHREN            1001
#define KEY_ANCIENT        3802
#define KEY_ARCANA        4402
#define KEY_CRUSADER        5702
#define KEY_LIGHT        4500
#define KEY_ENFORCER        23601
#define KEY_OUTLAW        0
#define KEY_MARAUDER        9801
#define KEY_EMPIRE         0
#define KEY_VALOR        1902
#define KEY_COVENANT       1501
#define KEY_CONCLAVE                21500

#define OBJ_VNUM_SILVER_ONE          1
#define OBJ_VNUM_GOLD_ONE          2
#define OBJ_VNUM_GOLD_SOME          3
#define OBJ_VNUM_SILVER_SOME          4
#define OBJ_VNUM_COINS              5
#define OBJ_VNUM_SACK              2901
#define OBJ_VNUM_STEAK              2902
#define OBJ_VNUM_POTION              2903
#define OBJ_VNUM_BATTLESHIELD          2904
#define OBJ_VNUM_RANGER_STAFF          2905
#define OBJ_VNUM_RAPIER             2949
#define OBJ_VNUM_HELPER_ROBE        2950
#define OBJ_VNUM_SKULL_CLEAVER        2926
#define OBJ_VNUM_AVENGER          2906
#define OBJ_VNUM_PONCHO              2907
#define OBJ_VNUM_ANCIENT_GAUNTLET     2908
#define OBJ_VNUM_PROTECTORATE          2912
#define OBJ_VNUM_BADGE              2913
#define OBJ_VNUM_ROBE_LIGHT           2915
#define OBJ_VNUM_SKELETON          2925
#define OBJ_VNUM_ANCIENT_FORGE        2929
#define OBJ_VNUM_SCALP                2930
#define OBJ_VNUM_MONK_BELT          2931
#define OBJ_VNUM_PHYLACTERY          2933
#define OBJ_VNUM_LICHPOTION          2934
#define OBJ_VNUM_OUTLAW_CLOAK          2935
#define OBJ_VNUM_SOUL_ORB          2936
#define OBJ_VNUM_MATERIAL_LUMP        2939
#define OBJ_VNUM_CORPSE_NPC         10
#define OBJ_VNUM_CORPSE_PC         11
#define OBJ_VNUM_SEVERED_HEAD         12
#define OBJ_VNUM_TORN_HEART         13
#define OBJ_VNUM_SLICED_ARM         14
#define OBJ_VNUM_SLICED_LEG         15
#define OBJ_VNUM_GUTS             16
#define OBJ_VNUM_BRAINS             17
#define OBJ_VNUM_SHAM_BRAND       3881
#define OBJ_VNUM_TWIG_BRAND         29050
#define OBJ_VNUM_OBLIVION_BRAND   11701
#define OBJ_VNUM_MOB_BRAND       19800
#define OBJ_VNUM_MUSHROOM         20
#define OBJ_VNUM_LIGHT_BALL         21
#define OBJ_VNUM_SPRING             22
#define OBJ_VNUM_CURING_SPRING      2948
#define OBJ_VNUM_DISC             23
#define OBJ_VNUM_PORTAL             25
#define OBJ_VNUM_PORTAL_NEW       2932
#define OBJ_VNUM_PORTAL_ABYSS       2937
#define OBJ_VNUM_ELEMENTAL_GATEWAY  2951
#define OBJ_VNUM_CONCLAVE_SCROLL 2952
#define OBJ_VNUM_MARAUDER_GCLOTH 9802
#define OBJ_VNUM_MARAUDER_BOMB      9803
#define OBJ_VNUM_MARAUDER_SALVE     9804
#define OBJ_VNUM_MARAUDER_SBOMB     9805
#define OBJ_VNUM_MARAUDER_ITCH      9806
#define OBJ_VNUM_MARAUDER_WHISTLE   9807

#define OBJ_VNUM_SHACKLES          2938
#define OBJ_VNUM_MONK_FLOWERS       304

#define OBJ_VNUM_WEDDING_RING 1200
#define OBJ_VNUM_ROSE              7
#define OBJ_VNUM_SCARLET_PIMPERNEL   13422
#define OBJ_VNUM_BLUE_PIMPERNEL      13423

#define OBJ_VNUM_PIT           3010
#define PIT_ELVENHAME_G           8804
#define PIT_ELVENHAME_N           8805
#define PIT_ELVENHAME_E           8806
#define PIT_NEWTHALOS           9603
#define PIT_OFCOL           671
#define PIT_ARKHAM           765
#define PIT_GLYNDANE_G           10679
#define PIT_GLYNDANE_N           10678
#define PIT_GLYNDANE_E           10677
#define PIT_DDA               27756
#define PIT_ETHSHAR           3000
#define PIT_DEFAULT           3010


#define OBJ_VNUM_SCHOOL_MACE       13312
#define OBJ_VNUM_SCHOOL_DAGGER       13313
#define OBJ_VNUM_SCHOOL_SWORD       13314
#define OBJ_VNUM_SCHOOL_SPEAR       13327
#define OBJ_VNUM_SCHOOL_STAFF       13328
#define OBJ_VNUM_SCHOOL_AXE       13329
#define OBJ_VNUM_SCHOOL_FLAIL       13330
#define OBJ_VNUM_SCHOOL_WHIP       13331
#define OBJ_VNUM_SCHOOL_POLEARM    13327

#define OBJ_VNUM_SCHOOL_VEST       13315
#define OBJ_VNUM_SCHOOL_SHIELD       13316
#define OBJ_VNUM_SCHOOL_BANNER     13302
#define OBJ_VNUM_MAP           10689
#define OBJ_VNUM_MAP_THERA       10690
#define OBJ_VNUM_SCHOOL_ROBE       13333
#define OBJ_VNUM_SCHOOL_BELT       13334
#define OBJ_VNUM_WHISTLE       2116

#define OBJ_VNUM_GENOCIDE_AXE      17807
#define OBJ_VNUM_RING_EMPOWER        3810
#define OBJ_VNUM_ARCPOTION        2920
#define OBJ_VNUM_ARCSCROLL        2921
#define OBJ_VNUM_ARCWAND        2922
#define OBJ_VNUM_ARCSTAFF        2923
#define OBJ_VNUM_BERRY           2924
#define OBJ_VNUM_TRAP              2940

#define OBJ_VNUM_HAIR_BRACELET    13725
#define OBJ_VNUM_BLOOD_BRACELET    13720
#define OBJ_VNUM_MEDUSA_WIG       14111
#define OBJ_VNUM_MOB_MEDUSA       14105
#define OBJ_VNUM_TIARA        23231
#define OBJ_VNUM_WIZARDRY_1    23236
#define OBJ_VNUM_WIZARDRY_2    23238
#define OBJ_VNUM_SWORD_PLANES    13714
#define OBJ_VNUM_HEAVENLY_SCEPTRE    23263
#define OBJ_VNUM_MACE_DISRUPTION    23261
#define OBJ_VNUM_TORQUE    13715
#define OBJ_VNUM_ROD_LORDLY    13717
#define OBJ_VNUM_CROWN_BLOOD    13716
#define OBJ_VNUM_RED_IOUN        1
#define OBJ_VNUM_BLUE_IOUN        1
#define OBJ_VNUM_GREEN_IOUN        1
#define OBJ_VNUM_WINGS        13711
#define OBJ_VNUM_HYDRA_SLAYER    13738
#define OBJ_VNUM_HYDRA_HEAD    13737
#define OBJ_VNUM_BLACK_KEY    13736
#define OBJ_VNUM_HORN_VALERE    13739
#define OBJ_VNUM_RING_PROTECT   16029
#define OBJ_VNUM_ROBE_AVENGER   16030
#define OBJ_VNUM_TALON          11426
#define OBJ_VNUM_BLACK_HIDE     11428
#define OBJ_VNUM_SHADOW_HIDE    11431
#define OBJ_VNUM_SYMBOL_MAGIC   11439
#define OBJ_VNUM_FLAME_TOWER    11418
#define OBJ_VNUM_SWORD_DEMONS   2928
#define OBJ_VNUM_SENTIENT_SWORD 2223
#define OBJ_VNUM_TRAITOR_SHIELD 12513
#define OBJ_VNUM_COIL_SNAKES    22658
#define OBJ_VNUM_SUMMONING_STONE    22652
#define OBJ_VNUM_CONCAT_RING    14712
#define OBJ_VNUM_ENDLESS_QUIVER 12415
#define OBJ_VNUM_FIERY_ARROWS   12416
#define OBJ_VNUM_BATTLEAXE_BRAND    17806
#define OBJ_VNUM_CHAOS_BRAND    15904
#define OBJ_VNUM_DOGGIE_TOY    28301
#define OBJ_VNUM_HECTOR_BRAND     7359
#define OBJ_VNUM_UTARA_BRAND   7585
#define OBJ_VNUM_EMPIRE_CRIM_PIKE  26 /* Used when a rebel dies to a member of Legion */
#define OBJ_VNUM_PHYSICAL_FOCUS 6916
#define OBJ_VNUM_MENTAL_FOCUS 6917
#define OBJ_VNUM_DICE 6976
#define OBJ_VNUM_DRINLINDA_BRAND 21670
#define OBJ_VNUM_DRITHENTIR_BRAND 20025
#define OBJ_VNUM_COW_SUIT 80000 /* Makes spellcasting and talking have a low chance of messing up */
#define OBJ_VNUM_LESTREGUS_BRAND     7608
#define OBJ_VNUM_LESTREGUS_FOUNTAIN 30101
#define OBJ_VNUM_APPLE_OF_LIFE   30201

/* the following are all objects from mtowere.are -werv */
/* Set to -1 so they wont do nothing - werv */
#define OBJ_VNUM_VOODOO_DOLL    -1
#define OBJ_VNUM_STAR_DAGGER    -1
#define OBJ_VNUM_DEMON_TALISMON -1
/* cathedral dungeon specials */
#define OBJ_VNUM_SKULL_NECROMANCY 16039
#define OBJ_VNUM_ARMOR_UNDEAD     16041
#define OBJ_VNUM_SOUL_SAINT       16042
#define OBJ_VNUM_ENERGY_AMULET    16043
#define OBJ_VNUM_DEATH_REVENGE    16044

/* Arkham Legion vnums */
#define OBJ_VNUM_PILE_BODIES_RITUAL 29557
#define OBJ_VNUM_MISTY_ESSENCE      29605
#define OBJ_VNUM_CLOAK_UNDERWORLD   29549
#define OBJ_VNUM_RED_DRAGONSTAR     29588
#define OBJ_VNUM_BLACK_DRAGONSTAR   29589
#define OBJ_VNUM_LIBRAM_INFINITE    29606
#define OBJ_VNUM_SCEPTRE_DEAD       29550
#define OBJ_VNUM_GAUNTLETS_STRIKING 29511
#define OBJ_VNUM_STRIFE_SPEAR       29524

/* Cockroach heads */
#define OBJ_VNUM_COCKROACH_HEAD_1   3084
#define OBJ_VNUM_COCKROACH_HEAD_2   9612
#define OBJ_VNUM_COCKROACH_HEAD_3   10714
#define OBJ_VNUM_COCKROACH_HEAD_4   10808
#define OBJ_VNUM_COCKROACH_HEAD_5   8717

#define OBJ_VNUM_PHANTOM            2954

#define OBJ_VNUM_IMPROV_BOX         15908

/*
 * Definitions used in fight.c for rings of focus
 */
#define FOCUS_RING_FULL        2
#define FOCUS_RING_LIMITED    1
#define FOCUS_RING_NONE        0

/*
 * Item types.
 * Used in #OBJECTS.
 */
#define ITEM_LIGHT         1
#define ITEM_SCROLL        2
#define ITEM_WAND          3
#define ITEM_STAFF         4
#define ITEM_WEAPON        5
#define ITEM_NULL6         6
#define ITEM_NULL7         7
#define ITEM_TREASURE      8
#define ITEM_ARMOR         9
#define ITEM_POTION        10
#define ITEM_CLOTHING      11
#define ITEM_FURNITURE     12
#define ITEM_TRASH         13
#define ITEM_CONTAINER     15
#define ITEM_DRINK_CON     17
#define ITEM_KEY           18
#define ITEM_FOOD          19
#define ITEM_MONEY         20
#define ITEM_BOAT          22
#define ITEM_CORPSE_NPC    23
#define ITEM_CORPSE_PC     24
#define ITEM_FOUNTAIN      25
#define ITEM_PILL          26
#define ITEM_PROTECT       27
#define ITEM_MAP           28
/*
Unused.  Kept as REQUIRED placeholder.
It can be replaced, but the other item types cannot
be moved up.  (warpstone must remain 30, room_key stays 31.. etc
for pfiles.
#define ITEM_PORTAL        29
*/
#define ITEM_WARP_STONE    30
#define ITEM_ROOM_KEY      31
#define ITEM_GEM           32
#define ITEM_JEWELRY       33
#define ITEM_JUKEBOX       34
#define ITEM_HOUSE_ITEM    35
#define ITEM_SKELETON      36
#define ITEM_PORTAL_NEW    37
#define ITEM_SPELLBOOK     38
#define ITEM_HERB          39
#define ITEM_BOOK          40
#define ITEM_PEN           41
#define ITEM_INKWELL       42
#define ITEM_TOOL          43

/*
 * Extra flags.
 * Used in #OBJECTS.
 */
#define ITEM_GLOW             (A)
#define ITEM_HUM              (B)
#define ITEM_DARK             (C)
#define ITEM_LOCK             (D)
#define ITEM_EVIL             (E)
#define ITEM_INVIS            (F)
#define ITEM_MAGIC            (G)
#define ITEM_NODROP           (H)
#define ITEM_BLESS            (I)
#define ITEM_ANTI_GOOD        (J)
#define ITEM_ANTI_EVIL        (K)
#define ITEM_ANTI_NEUTRAL     (L)
#define ITEM_NOREMOVE         (M)
#define ITEM_INVENTORY        (N)
#define ITEM_NOPURGE          (O)
#define ITEM_ROT_DEATH        (P)
#define ITEM_VIS_DEATH        (Q)
#define ITEM_NONMETAL         (S)
#define ITEM_NOLOCATE         (T)
#define ITEM_MELT_DROP        (U)
#define ITEM_NO_BRAINS        (V)
#define ITEM_SELL_EXTRACT     (W)
#define ITEM_BURN_PROOF       (Y)
#define ITEM_NOUNCURSE        (Z)
#define ITEM_BRAND            (aa)
#define CORPSE_NO_ANIMATE     (bb)
#define ITEM_ANTI_METHODICAL  (cc)
#define ITEM_ANTI_NEUT        (dd)
#define ITEM_ANTI_IMPULSIVE   (ee)

/* object extra2 flags */
#define ITEM_NOBUY            (A)
#define ITEM_RIDEABLE         (B)
#define HALFLING_ONLY         (C)
#define BOOK_ONLY             (D)
#define ARBOR_ONLY            (E)
#define ITEM_NOORIGIN         (F)
#define ITEM_PURIFIED         (G)
#define ITEM_AHZRA_BLOODSTAIN (H) /* Used for Rantialen's Ahzra, do not use for building */
#define ITEM_NO_BLOOD         (I)
#define ITEM_NO_SHIELD_CLEAVE (J)
#define FEMALE_ONLY           (K)
#define MALE_ONLY             (L)
#define GENDERLESS_ONLY       (M)
#define DEMON_ONLY            (N)
#define BRETHREN_ONLY         (O)
#define COVENANT_ONLY         (P)
#define CONCLAVE_ONLY         (Q)
#define ETHEREAL_ONLY         (R)
#define WEAR_SIZE_TINY        (S)
#define WEAR_SIZE_SMALL       (T)
#define WEAR_SIZE_MEDIUM      (U)
#define WEAR_SIZE_LARGE       (V)
#define WEAR_SIZE_HUGE        (W)
#define WEAR_SIZE_GIANT       (X)

#define VALOR_ONLY            0
#define EMPIRE_ONLY           0
#define SCHOLAR_ONLY          0
#define LIFE_ONLY             0
/*
 * Mount flags.
 * Used in #OBJECTS.
 */
#define MOUNT_EARTH        (A)
#define MOUNT_AIR        (B)
#define MOUNT_FIRE        (C)
#define MOUNT_ON_WATER        (D)
#define MOUNT_UNDER_WATER    (E)
#define MOUNT_FOREST        (F)
#define MOUNT_MOUNTAIN        (G)

/*
 * Wear flags.
 * Used in #OBJECTS.
 */
#define ITEM_TAKE        (A)
#define ITEM_WEAR_FINGER    (B)
#define ITEM_WEAR_NECK        (C)
#define ITEM_WEAR_BODY        (D)
#define ITEM_WEAR_HEAD        (E)
#define ITEM_WEAR_LEGS        (F)
#define ITEM_WEAR_FEET        (G)
#define ITEM_WEAR_HANDS        (H)
#define ITEM_WEAR_ARMS        (I)
#define ITEM_WEAR_SHIELD    (J)
#define ITEM_WEAR_ABOUT        (K)
#define ITEM_WEAR_WAIST        (L)
#define ITEM_WEAR_WRIST        (M)
#define ITEM_WIELD        (N)
#define ITEM_HOLD        (O)
#define ITEM_NO_SAC        (P)
#define ITEM_WEAR_FLOAT        (Q)
#define ITEM_WEAR_BRAND         (R)
#define ITEM_KEEP               (S)
#define ITEM_WEAR_HORNS         (T)
#define ITEM_WEAR_SNOUT         (U)
#define ITEM_WEAR_HOOVES        (V)
#define ITEM_WEAR_FOURLEGS      (W)
#define ITEM_WEAR_FOURHOOVES    (X)
#define ITEM_WEAR_EYES          (Y)
#define ITEM_WEAR_EAR           (Z)
#define ITEM_WEAR_BOSOM         (aa)
#define ITEM_WEAR_INSIDE_COVER  (bb)

/* Used to check restrictions on objects (house/class/race) */
extern int  any_race_r;
extern int  any_race_e2;
extern int  any_class_r;
extern int  any_class_e2;
extern int  any_house_r;
extern int  any_house_e2;

extern int  any_size_e2;

/* object restrict_flags */
#define HUMAN_ONLY        (A)
#define DWARF_ONLY        (B)
#define ELF_ONLY        (C)
#define GREY_ONLY        (D)
#define DARK_ONLY        (E)
#define GIANT_ONLY        (F)
#define CENTAUR_ONLY        (G)
#define TROLL_ONLY        (H)
#define DRACONIAN_ONLY        (I)
#define GNOME_ONLY        (J)
#define WARRIOR_ONLY        (K)
#define THIEF_ONLY        (L)
#define CLERIC_ONLY        (M)
#define PALADIN_ONLY        (N)
#define ANTI_PALADIN_ONLY    (O)
#define RANGER_ONLY        (P)
#define MONK_ONLY        (Q)
#define CHANNELER_ONLY        (R)
#define NIGHTWALKER_ONLY    (S)
#define NECROMANCER_ONLY    (T)
#define ELEMENTALIST_ONLY    (U)

#define ANCIENT_ONLY        (V)
#define ARCANA_ONLY        (W)
#define CRUSADER_ONLY        (X)
#define LIGHT_ONLY        (Y)
#define ENFORCER_ONLY        (Z)
#define OUTLAW_ONLY        (aa)
#define MARAUDER_ONLY        (aa)
#define MAGE_ONLY        (bb)
#define MINOTAUR_ONLY           (cc)
#define CHANGELING_ONLY         (dd)
#define ILLITHID_ONLY           (ee)

/* weapon class */
#define WEAPON_EXOTIC        0
#define WEAPON_SWORD        1
#define WEAPON_DAGGER        2
#define WEAPON_SPEAR        3
#define WEAPON_MACE        4
#define WEAPON_AXE        5
#define WEAPON_FLAIL        6
#define WEAPON_WHIP        7
#define WEAPON_POLEARM        8
#define WEAPON_STAFF        9
#define WEAPON_BOW        10
#define WEAPON_ARROWS        11

/* weapon types */
#define WEAPON_FLAMING        (A)
#define WEAPON_FROST        (B)
#define WEAPON_VAMPIRIC        (C)
#define WEAPON_SHARP        (D)
#define WEAPON_VORPAL        (E)
#define WEAPON_TWO_HANDS    (F)
#define WEAPON_SHOCKING        (G)
#define WEAPON_POISON        (H)
#define WEAPON_AVENGER        (I) /* Do not use this in building. */
#define WEAPON_SHADOWBANE    (J) /* Do not use this in building. */
#define WEAPON_LIGHTBRINGER    (K) /* Do not use this in building. */
#define WEAPON_DROWNING         (L)
#define WEAPON_PIERCING         (M)
#define WEAPON_RESONATING       (N)
#define WEAPON_DRAINING         (O) /* Drains mana */
#define WEAPON_FATIGUING        (P) /* Drains movement */

/* new portal flags used with value 4 on portal objects */
#define PORTAL_CLOSED           (A)
#define PORTAL_TIMEGATE         (B)
#define PORTAL_MOONGATE         (C)

/* gate flags */
#define GATE_NORMAL_EXIT    (A)
#define GATE_NOCURSE        (B)
#define GATE_GOWITH        (C)
#define GATE_BUGGY        (D)
#define GATE_RANDOM        (E)

/* furniture flags */
#define STAND_AT        (A)
#define STAND_ON        (B)
#define STAND_IN        (C)
#define SIT_AT            (D)
#define SIT_ON            (E)
#define SIT_IN            (F)
#define REST_AT            (G)
#define REST_ON            (H)
#define REST_IN            (I)
#define SLEEP_AT        (J)
#define SLEEP_ON        (K)
#define SLEEP_IN        (L)
#define PUT_AT            (M)
#define PUT_ON            (N)
#define PUT_IN            (O)
#define PUT_INSIDE        (P)




/*
 * Apply types (for affects).
 * Used in #OBJECTS.
 */
#define APPLY_NONE              0
#define APPLY_STR               1
#define APPLY_DEX               2
#define APPLY_INT               3
#define APPLY_WIS               4
#define APPLY_CON               5
#define APPLY_SEX               6
#define APPLY_CLASS             7
#define APPLY_LEVEL             8
#define APPLY_AGE               9
#define APPLY_HEIGHT           10
#define APPLY_WEIGHT           11
#define APPLY_MANA             12
#define APPLY_HIT              13
#define APPLY_MOVE             14
#define APPLY_GOLD             15
#define APPLY_EXP              16
#define APPLY_AC               17
#define APPLY_HITROLL          18
#define APPLY_DAMROLL          19
#define APPLY_SAVES            20
#define APPLY_SAVING_MALEDICT  21
#define APPLY_SAVING_TRANSPORT 22
#define APPLY_SAVING_BREATH    23
#define APPLY_SAVING_SPELL     24
#define APPLY_SPELL_AFFECT     25
#define APPLY_MORPH_FORM       26
#define APPLY_REGENERATION     27
#define APPLY_ACCURACY         28
#define APPLY_ATTACKS          29
#define APPLY_SIZE             30
#define APPLY_SPELL_POWER      31
#define APPLY_SIGHT            32
#define APPLY_HOLY_POWER       33
#define APPLY_MN_REGENERATION  34
#define APPLY_MAX_STR          35
#define APPLY_ALIGN            36

/*
 * Peace modifiers
 */
#define MODIFY_PEACE_BY_CIRDAN_BRAND   0
#define MODIFY_PEACE_BY_SOOTHING_VOICE 1

/*
 * Values for containers (value[1]).
 * Used in #OBJECTS.
 */
#define CONT_CLOSEABLE              1 /* A */
#define CONT_PICKPROOF              2 /* B */
#define CONT_CLOSED              4 /* C */
#define CONT_LOCKED              8 /* D */
#define CONT_PUT_ON             16 /* E */
#define CONT_IS_QUIVER             32 /* F */
#define CONT_SMALLTRAP               64 /* G Malignus */
#define CONT_BIGTRAP                128 /* H Malignus */

#define HAS_DIED    8

/*
 * Well known room virtual numbers.
 * Defined in #ROOMS.
 */
#define ROOM_VNUM_DEATH_GOOD        1212
#define ROOM_VNUM_DEATH_NEUTRAL        1212
#define ROOM_VNUM_DEATH_EVIL        1212

#define ROOM_VNUM_OUTLAW            0
#define ROOM_VNUM_MARAUDER          9800
#define ROOM_VNUM_ENFORCER          23610
#define ROOM_VNUM_CRUSADER          5708
#define ROOM_VNUM_BRETHREN          1005
#define ROOM_VNUM_ARCANA            4406
#define ROOM_VNUM_ANCIENT           3808
#define ROOM_VNUM_LIGHT             4505
#define ROOM_VNUM_NIGHTWALK         2900
#define ROOM_VNUM_LIMBO             2
#define ROOM_VNUM_CHAT              1200
#define ROOM_VNUM_TEMPLE            3001
#define ROOM_VNUM_ALTAR             3054
#define ROOM_VNUM_SCHOOL            13300
#define ROOM_VNUM_BALANCE           4500
#define ROOM_VNUM_CIRCLE            4400
#define ROOM_VNUM_DEMISE            4201
#define ROOM_VNUM_HONOR             4300
#define ROOM_VNUM_VENUEPORT         9302
#define ROOM_VNUM_EMPIRE            0
#define ROOM_VNUM_VALOR             0  /* 1903 Room disabled */
#define ROOM_VNUM_COVENANT_PRISON   1524
#define ROOM_VNUM_ABYSS             1035
#define ROOM_VNUM_SCHOLAR           0
#define ROOM_VNUM_COVENANT          1507
#define ROOM_VNUM_CONCLAVE          21510

/* Arkham Legion vnums */
#define ROOM_VNUM_SHADOWLORD_SUMMON 29742
#define ROOM_VNUM_DEMOGORGON_SUMMON 29724
#define ROOM_VNUM_SHAD_DEMOGORGON   29599
#define ROOM_VNUM_THROAT            23657
#define ROOM_VNUM_BATHROOM          23661

/* temple vnums for hometowns */
#define TEMPLE_ELVENHAME_G    8839
#define TEMPLE_ELVENHAME_N    8844
#define TEMPLE_ELVENHAME_E    8848
#define TEMPLE_GLYNDANE_G    10670
#define TEMPLE_GLYNDANE_N    10681
#define TEMPLE_GLYNDANE_E    10699
#define TEMPLE_MIDGAARD        3054
#define TEMPLE_NEW_THALOS    9609
#define TEMPLE_OFCOL        699
#define TEMPLE_ARKHAM        768
#define TEMPLE_SHEQESTON    6837
#define TEMPLE_DDA        27815
#define TEMPLE_ETHSHAR        3054

#define ROOM_VNUM_HOMETOWNS    8881

/*
 * Room flags.
 * Used in #ROOMS.
 */
#define ROOM_DARK        (A)
#define ROOM_NO_BLITZ           (B)
#define ROOM_NO_MOB        (C)
#define ROOM_INDOORS        (D)
#define ROOM_HOUSE              (E)
#define ROOM_SHRINE             (F)
#define ROOM_PRIVATE        (J)
#define ROOM_SAFE        (K)
#define ROOM_SOLITARY        (L)
#define ROOM_PET_SHOP        (M)
#define ROOM_NO_RECALL        (N)
#define ROOM_IMP_ONLY        (O)
#define ROOM_GODS_ONLY        (P)
#define ROOM_HEROES_ONLY    (Q)
#define ROOM_NEWBIES_ONLY    (R)
#define ROOM_LAW        (S)
#define ROOM_NOWHERE        (T)
#define ROOM_NO_GATE        (U)
#define ROOM_CONSECRATED    (V)
#define ROOM_NO_SUMMON            (W)
#define ROOM_NO_CONSECRATE    (X)
#define ROOM_NO_TELEPORT     (Y)
#define ROOM_NO_ALARM        (Z)
#define ROOM_LOW_ONLY        (bb)
#define ROOM_NO_MAGIC        (cc)
#define ROOM_BLOODY_TIMER       (dd)
#define ROOM_NO_TRACK        (ee)

/* Room extra_room_flags */
#define ROOM_GLOBE_DARKNESS   (A)
#define ROOM_AP_CURSED        (B)
#define ROOM_1212             (C)
#define ROOM_DEAD_ZONE        (D)
#define ROOM_METEOR_STORM     (E)
#define ROOM_SPRING_RAINS     (F)

/* Random Room Generation */
#define RANDOM_ALL          0       /* covers all rooms               */
#define RANDOM_NORMAL       1    /* excludes shrines and imm areas */
#define RANDOM_AREA         2    /* gets room in current area only */
#define RANDOM_AREA_FOREST  3    /* gets room in current area (and forest) only */
#define RANDOM_AREA_IMAGE   4    /* Gets room in current area, even if can't get there */
#define RANDOM_MOB_TELEPORT 5    /* For mob teleporter-quests */

/*
 * Directions.
 * Used in #ROOMS.
 */
#define DIR_NORTH              0
#define DIR_EAST              1
#define DIR_SOUTH              2
#define DIR_WEST              3
#define DIR_UP                  4
#define DIR_DOWN              5


/*
 * Exit flags.
 * Used in #ROOMS.
 */
#define EX_ISDOOR              (A)
#define EX_CLOSED              (B)
#define EX_LOCKED              (C)
#define EX_PICKPROOF              (F)
#define EX_NOPASS              (G)
#define EX_EASY                  (H)
#define EX_HARD                  (I)
#define EX_INFURIATING              (J)
#define EX_NOCLOSE              (K)
#define EX_NOLOCK              (L)
#define EX_NOBASH              (M)

/*
 * Sector types.
 * Used in #ROOMS.
 */
#define SECT_INSIDE              0
#define SECT_CITY              1
#define SECT_FIELD              2
#define SECT_FOREST              3
#define SECT_HILLS              4
#define SECT_MOUNTAIN              5
#define SECT_WATER_SWIM              6
#define SECT_WATER_NOSWIM          7
#define SECT_UNUSED              8
#define SECT_AIR              9
#define SECT_DESERT             10
#define SECT_UNDERWATER              11
#define SECT_UNDERGROUND             12
#define SECT_MAX                     13

/* Class guild used in the room 'G'  flags */
#define GUILD_WARRIOR        1
#define GUILD_THIEF        2
#define GUILD_CLERIC        3
#define GUILD_PALADIN        4
#define GUILD_ANTI_PALADIN    5
#define GUILD_RANGER        6
#define GUILD_MONK        7
#define GUILD_CHANNELER        8
#define GUILD_NIGHTWALKER    9
#define GUILD_NECROMANCER    10
#define GUILD_ELEMENTALIST    11

#define CLASS_WARRIOR 0
#define CLASS_THIEF 1
#define CLASS_CLERIC 2
#define CLASS_PALADIN 3
#define CLASS_ANTI_PALADIN 4
#define CLASS_RANGER 5
#define CLASS_MONK 6
#define CLASS_CHANNELER 7
#define CLASS_NIGHTWALKER 8
#define CLASS_NECROMANCER 9
#define CLASS_ELEMENTALIST 10

/* define subrace values -Wervdon */
#define SCALE_NOT_PICKED  0
#define SCALE_GREEN  1
#define SCALE_RED     2
#define SCALE_BLACK   3
#define SCALE_WHITE   4
#define SCALE_BLUE    5
#define SCALE_GOLD    6
#define SCALE_SILVER  7
#define SCALE_BRONZE  8
#define SCALE_COPPER  9
#define SCALE_BRASS   10

/* define subclass specialization values - Wervdon */
#define SUBCLASS_WAITING         -2
#define SUBCLASS_PICKED             -1
#define SUBCLASS_NONE                 0
#define SUBCLASS_KNIGHT_OF_WAR        1
#define SUBCLASS_KNIGHT_OF_FAMINE     2
#define SUBCLASS_KNIGHT_OF_PESTILENCE 3
#define SUBCLASS_KNIGHT_OF_DEATH      4
#define SUBCLASS_ENCHANTER            5
#define SUBCLASS_ABJURER              6
#define SUBCLASS_ILLUSIONIST          7
#define SUBCLASS_PRIEST_HEALING       8
#define SUBCLASS_TEMPORAL_MASTER      9
#define SUBCLASS_PRIEST_PROTECTION    10
#define SUBCLASS_ZEALOT_FAITH         11
#define SUBCLASS_GEOMANCER            12
#define SUBCLASS_PYROMANCER           13
#define SUBCLASS_AQUAMANCER           14
#define SUBCLASS_AREOMANCER           15
#define SUBCLASS_ANATOMIST            16
#define SUBCLASS_CORRUPTOR            17
#define SUBCLASS_NECROPHILE           18
#define SUBCLASS_TEMPLAR_NIGHT        19
#define SUBCLASS_NIGHTSPAWN           20
#define SUBCLASS_BEASTMASTER          21
#define SUBCLASS_BARBARIAN            22
#define SUBCLASS_HUNTER               23
#define SUBCLASS_KNIGHT_VIRTUE        24
#define SUBCLASS_KNIGHT_ORDER         25
#define SUBCLASS_AVENGER_FAITH        26
#define SUBCLASS_NINJA                27
#define SUBCLASS_HIGHWAYMAN           28
#define SUBCLASS_SWASHBUCKLER         29
#define SUBCLASS_WEAPON_MASTER        30
#define SUBCLASS_TACTITIAN            31
#define SUBCLASS_JUGGERNAUT           32

/* Monk subclass schools -Wervdon */
/*
   Must stay in this order with nothing breaking them up
   Yanwei (monk winner of a contest) gets all emotes, and needs this
*/
#define SUBCLASS_SCHOOL_TIGER         33
#define SUBCLASS_SCHOOL_SNAKE         34
#define SUBCLASS_SCHOOL_DRAGON        35
#define SUBCLASS_SCHOOL_MANTIS        36
#define SUBCLASS_SCHOOL_CRANE         37


/*
 * Equpiment wear locations.
 * Used in #RESETS.
 */
#define WEAR_NONE               -1
#define WEAR_LIGHT               0
#define WEAR_FINGER_L            1
#define WEAR_FINGER_R            2
#define WEAR_NECK_1              3
#define WEAR_NECK_2              4
#define WEAR_BODY                5
#define WEAR_HEAD                6
#define WEAR_LEGS                7
#define WEAR_FEET                8
#define WEAR_HANDS               9
#define WEAR_ARMS                10
#define WEAR_SHIELD              11
#define WEAR_ABOUT               12
#define WEAR_WAIST               13
#define WEAR_WRIST_L             14
#define WEAR_WRIST_R             15
#define WEAR_WIELD               16
#define WEAR_HOLD                17
#define WEAR_DUAL_WIELD          18
#define WEAR_FLOAT               18
#define WEAR_TERTIARY_WIELD      19
#define WEAR_BRAND               20
#define WEAR_HORNS               21
#define WEAR_SNOUT               22
#define WEAR_HOOVES              23
#define WEAR_FOURLEGS            24
#define WEAR_FOURHOOVES          25
#define WEAR_EYES                26
#define WEAR_EAR_L               27
#define WEAR_EAR_R               28
#define WEAR_BOSOM               29
#define WEAR_INSIDE_COVER        30
#define MAX_WEAR                 31

/*
   Immortal only wear slots.  MAX_WEAR counts towards
   inventory slots, and we don't want these counting.
*/
#define WEAR_IMM_ONLY               2



/***************************************************************************
 *                                                                         *
 *                   VALUES OF INTEREST TO AREA BUILDERS                   *
 *                   (End of this section ... stop here)                   *
 *                                                                         *
 ***************************************************************************/

/*
 * Conditions.
 */
#define COND_DRUNK              0
#define COND_FULL              1
#define COND_THIRST              2
#define COND_HUNGER              3
#define COND_STARVING              4
#define COND_DEHYDRATED              5

/*
 * Positions.
 */
#define POS_DEAD              0
#define POS_MORTAL              1
#define POS_INCAP              2
#define POS_STUNNED              3
#define POS_SLEEPING              4
#define POS_RESTING              5
#define POS_SITTING              6
#define POS_FIGHTING              7
#define POS_STANDING              8

/*
 * Mount types
 */
#define MOUNT_NONE        0
#define MOUNT_MOBILE        1
#define MOUNT_OBJECT        2
#define MOUNT_PC        3

/*
 * Mount info flags
 * For internal tracking of mounts.
 */
#define MOUNT_INFO_INVENTORY        (A)

/*
 * Definitions for messaging system
 */
#define MESSAGE_TARGET_OBJECT    0
#define MESSAGE_TARGET_CHAR    1
#define MESSAGE_TARGET_ROOM    2    /* Room is not yet functional */

/*
 * For message_flag
 */
#define MESSAGE_POLL_PULSE    (A)
#define MESSAGE_POLL_TICK    (B)
#define MESSAGE_SAVE        (C)    /* Saving messages is not yet implemented */
#define MESSAGE_UNIQUE        (D)
#define MESSAGE_REPLACE        (E)


/*
 * Object messages
 */
#define OM_DOGGIE_TOY_RUN    0

/*
 * Mobile messages
 */
#define MM_HOUSE_ITEM_TAKEN    0
#define MM_HANG1                1
#define MM_HANG2                2
#define MM_HANG3                3
#define MM_HANG4                4

/*
 * ACT bits for players.
 */
#define PLR_IS_NPC        (A)        /* Don't EVER set.    */

/* RT auto flags */
#define PLR_AUTOASSIST        (C)
#define PLR_AUTOEXIT        (D)
#define PLR_AUTOLOOT        (E)
#define PLR_AUTOSAC             (F)
#define PLR_AUTOGOLD        (G)
#define PLR_AUTOSPLIT        (H)
#define PLR_COLOR               (I)
#define PLR_EVALUATION        (J)

/* RT personal flags */
#define PLR_NOSUMMON        (Q)
#define PLR_NOFOLLOW        (R)
/* 2 bits reserved, S-T */

#define PLR_HONORBOUND        (S)
#define PLR_NO_TRANSFER        (T)

/* penalty flags */
#define PLR_PERMIT        (U)
#define PLR_LOG            (W)
#define PLR_DENY        (X)
#define PLR_FREEZE        (Y)
#define PLR_THIEF        (Z)
#define PLR_KILLER        (aa)
#define PLR_CRIMINAL            (bb)
#define PLR_MARKED              (cc)
#define PLR_EVIL                (dd)
#define PLR_COVENANT_ALLY        (ee)

/* ACT2 bits */
#define PLR_ANCIENT_MARK        (A)  /* Hunted by Ancient        */
#define PLR_ANCIENT_MARK_PERM   (B)  /* Always hunted by Ancient */
#define PLR_GUILDMASTER         (C)  /* Player is a guildmaster */
#define PLR_GUILDLESS           (D)  /* Player has no guild */
#define WIZ_SEEBRAND            (E)  /* Allows branded to see throughwizi*/
#define WIZ_SEEHOUSE            (F)  /* Allows your house to see through wizi */
#define PLR_NOVICE              (G)  /* Shows novice on who */
#define PLR_LOG_SPEC            (H)  /* logs conversations to special file */
#define PLR_LICH                (I)
#define PLR_IS_ANCIENT          (J)
#define ACT2_AUTOATTACK         (K)
#define PLR_IS_ANCIENT_KNOWN    (L)
#define WIZ_SEENEWBIE           (M)
#define WIZ_SEEWORSHIP          (N)
#define PLR_NO_FIGHT            (O)
#define PLR_IMPURE              (P)
#define WIZ_SEECLAN             (Q)
#define PLR_ACTIVELY_BRANDING   (R)
#define PLR_LAWLESS             (S)
#define PLR_NO_HOUSE_EMOTE      (T)
#define PLR_MAGIC_TAKEN         (U)


/* Impurity types */
#define IMPURE_CAST              0
#define IMPURE_RECITE            1
#define IMPURE_EAT_PILL          2
#define IMPURE_QUAFF             3
#define IMPURE_BRANDISH          4
#define IMPURE_ZAP               5
#define IMPURE_GROUP_MAGE        6
#define IMPURE_WEAR              7
#define IMPURE_EAT               8
#define IMPURE_DRINK             9
#define IMPURE_PRACTICE          10
#define IMPURE_PRACTICE_INTENT   11


/* Logspec defines */
#define TYPE_TELL         1
#define TYPE_SAY          2
#define TYPE_GTELL        3
#define TYPE_OOC          4
#define TYPE_YELL         5
#define TYPE_AUCTION      6
#define TYPE_QUESTION     7
#define TYPE_HOUSE        8
#define TYPE_IMM          9
#define TYPE_TELL2       10
#define TYPE_SAY2        11
#define TYPE_GTELL2      12
#define TYPE_OOC2        13
#define TYPE_YELL2       14
#define TYPE_AUCTION2    15
#define TYPE_QUESTION2   16
#define TYPE_HOUSE2      17
#define TYPE_IMM2        18
#define TYPE_CONFERENCE  19
#define TYPE_CONFERENCE2 20
#define TYPE_CLAN        21
#define TYPE_CLAN2       22

/* RT comm flags -- may be used on both mobs and chars */
#define COMM_QUIET              (A)
#define COMM_DEAF                (B)
#define COMM_NOWIZ              (C)
#define COMM_NOAUCTION          (D)
#define COMM_NOGOSSIP           (E)
#define COMM_NOQUESTION         (F)
#define COMM_NOMUSIC            (G)
#define COMM_NOHOUSE             (H)
#define COMM_NOQUOTE        (I)
#define COMM_SHOUTSOFF        (J)
#define COMM_ANSI               (K)
#define COMM_OOCOFF             (ee)

/* display flags */
#define COMM_COMPACT        (L)
#define COMM_BRIEF        (M)
#define COMM_PROMPT        (N)
#define COMM_COMBINE        (O)
#define COMM_TELNET_GA        (P)
#define COMM_SHOW_AFFECTS    (Q)
#define COMM_MORON        (R)
#define COMM_IMMORTAL        (S)
#define COMM_LIMITS             (dd)

/* penalties */

#define COMM_NOEMOTE        (T)
#define COMM_NOSHOUT        (U)
#define COMM_NOTELL        (V)
#define COMM_NOCHANNELS        (W)
#define COMM_BUILDER        (X)
#define COMM_SNOOP_PROOF    (Y)
#define COMM_AFK        (Z)
#define COMM_ALL_HOUSES        (aa)
#define COMM_NOPRAY             (bb)
#define COMM_SILENCE            (cc)

/* comm2 flags - Wervdon */
#define COMM_BTGLOBAL            (A)
#define COMM_WIZ_TALK            (B)
#define COMM_STAT_TIME           (C)
#define COMM_WORSHIP_STOPPED     (D)
#define COMM_CAST_SILENT         (E)
#define COMM_WIZ_REVOKE          (F)   /* Wiz-powers revoked */
#define COMM_CTGLOBAL            (G)   /* Global clan talk */
#define COMM_NOCLAN              (H)   /* Global clan talk */
/* Music/sound tags */
#define COMM_MUSIC               (I)   /* Music on/off */
#define COMM_MUSIC_DOWNLOAD      (J)   /* Music download on/off */
#define COMM_SEE_SURNAME         (K)   /* See surnames on who list */
#define COMM_SURNAME_ACCEPTED    (L)   /* See surnames on who list */
#define COMM_REDUCED_COLOR       (M)   /* Reduced color */

/* WIZnet flags */
#define WIZ_ON          (A)
#define WIZ_TICKS       (B)
#define WIZ_LOGINS      (C)
#define WIZ_SITES       (D)
#define WIZ_LINKS       (E)
#define WIZ_DEATHS      (F)
#define WIZ_RESETS      (G)
#define WIZ_MOBDEATHS   (H)
#define WIZ_FLAGS       (I)
#define WIZ_PENALTIES   (J)
#define WIZ_SACCING     (K)
#define WIZ_LEVELS      (L)
#define WIZ_SECURE      (M)
#define WIZ_SWITCHES    (N)
#define WIZ_SNOOPS      (O)
#define WIZ_RESTORE     (P)
#define WIZ_LOAD        (Q)
#define WIZ_NEWBIE      (R)
#define WIZ_PREFIX      (S)
#define WIZ_SPAM        (T)
#define WIZ_TRANSFERS   (U)
#define PLR_FORSAKEN    (V)
#define WIZ_ALLOWRP     (W)
#define WIZ_ITEMS       (X)
#define WIZ_SWEAR       (Y)
#define WIZ_DELETES     (Z)
#define WIZ_MARRY       (aa)
#define WIZ_WORSHIP     (bb)
#define WIZ_BUGS        (cc)
#define WIZ_CLANS       (dd)
#define WIZ_CYBER       (ee)

/* WIZnet2 flags */
#define WIZ_NAME        (A)

/* see_state definitions */
#define NO_SEE_TARGET        0
#define SEE_TARGET_BF        1
#define SEE_TARGET        2

/* Search stuff for new owhere and mwhere - Maelstrom */
#define SCOPE_NONE      (A)
#define SCOPE_OBJECT    (B)
#define SCOPE_MOB       (C)
#define SCOPE_ROOM      (D)

#define SEARCH_NONE     0
#define SEARCH_OLD      1
#define SEARCH_NAME     2
#define SEARCH_MATERIAL 3
#define SEARCH_DAMCLASS 4
#define SEARCH_WEAPON   5
#define SEARCH_ITEM     6
#define SEARCH_SLOT     7
#define SEARCH_AFFECT   8
#define SEARCH_FLAG     9
#define SEARCH_ORIGIN  10
#define SEARCH_OWNER   11
#define SEARCH_SPELL   12
#define SEARCH_LEVEL   13
#define SEARCH_LIMIT   14
#define SEARCH_COST    15
#define SEARCH_VNUM    16
#define SEARCH_STRING  17

/*
 * Prototype for a mob.
 * This is the in-memory version of #MOBILES.
 */
struct    mob_index_data
{
   MOB_INDEX_DATA*    next;
   MOB_INDEX_DATA*   next_in_area;
   SPEC_FUN *        spec_fun;
   SHOP_DATA *        pShop;
   MPROG_LIST *        mprogs;
   sh_int        vnum;
   sh_int        group;
   bool        new_format;
   sh_int        count;
   sh_int        killed;
   char *        player_name;
   char *        short_descr;
   char *        long_descr;
   char *        description;
   long           act;
   long           act2;
   long           affected_by;
   long           affected_by2;
   sh_int        alignment;
   sh_int        level;
   sh_int        hitroll;
   sh_int        hit[3];
   sh_int        mana[3];
   sh_int        damage[3];
   sh_int        ac[4];
   sh_int         dam_type;
   long        off_flags;
   long        imm_flags;
   long        res_flags;
   long        vuln_flags;
   long                mprog_flags;
   sh_int        start_pos;
   sh_int        default_pos;
   sh_int        sex;
   sh_int        race;
   sh_int        subrace;
   long        wealth;
   long        form;
   long        parts;
   sh_int        size;
   char *        material;
   LIST_DATA*  skill_mods;
};

/*
 * Color codes
 */
#define ANSI_NORMAL            "\x01B[0;37m"
#define ANSI_BOLD            "\x01B[1m"
#define ANSI_BLINK            "\x01B[5m"
#define ANSI_REVERSE            "\x01B[7m"

/* memory settings */
#define MEM_CUSTOMER    A
#define MEM_SELLER    B
#define MEM_HOSTILE    C
#define MEM_AFRAID    D

/* memory for mobs */
struct mem_data
{
    MEM_DATA     *next;
    bool    valid;
    int        id;
    int     reaction;
    time_t     when;
};

/* Roster info for houses/clans/brands */
struct    roster_data
{
   ROSTER* next;
   char* data;
};
#define ROSTER_CLAN (1)
#define ROSTER_BRAND (2)
#define ROSTER_HOUSE (3)
#define ROSTER_WORSHIP (4)

/* -1 is skipped, because skill_lookup can return -1 */
#define SKILL_MOD_ALL   -2

struct skill_mod_type
{
   bool        valid;
   int         skill_percent;
   int         sn;
   SKILL_MOD*  next;
};

/*
   One character (PC or NPC).
*/
struct char_data
{
   CHAR_DATA*        next;
   CHAR_DATA*        next_in_room;
   CHAR_DATA*        master;
   CHAR_DATA*        leader;
   CHAR_DATA*        fighting;
   CHAR_DATA*        reply;
   CHAR_DATA*        pet;
   CHAR_DATA*        mprog_target;
   long              last_fought;
   long              last_fought2;
   CHAR_DATA*        nightfall;
   CHAR_DATA*        defending;
   CHAR_DATA*        life_lined;
   CHAR_DATA*        see;
   CHAR_DATA*        nosee;
   ALARM_DATA*       alarm;
   MEM_DATA*         memory;
   SPEC_FUN*         spec_fun;
   MOB_INDEX_DATA*   pIndexData;
   DESCRIPTOR_DATA*  desc;
   AFFECT_DATA*      affected;
   NOTE_DATA*        pnote;
   OBJ_DATA*         carrying;
   OBJ_DATA*         on;
   ROOM_INDEX_DATA*  in_room;
   ROOM_INDEX_DATA*  was_in_room;
   AREA_DATA*        zone;
   PC_DATA*          pcdata;
   GEN_DATA*         gen_data;
   bool              valid;
   char*             name;
   long              id;
   sh_int            version;
   char*             short_descr;
   char*             long_descr;
   char*             description;
   char*             prompt;
   char*             prefix;
   int               temple;
   sh_int            morph_form[3]; /* form,sn,charges */
   sh_int            group;
   sh_int            house;
   sh_int            sex;
   sh_int            class;
   sh_int            race;
   sh_int            subrace;
   sh_int            level;
   sh_int            trust;
   int               played;
   int               lines; /* for the pager */
   time_t            logon;
   sh_int            timer;
   sh_int            wait;
   sh_int            daze;
   sh_int            noregen_dam; /* For troll regen...Ceran */
   sh_int            regen_rate; /* For imbue regeneration spell */
   sh_int            mn_regen_rate;
   sh_int            drain_level;
   sh_int            hit;
   sh_int            max_hit;
   sh_int            mana;
   sh_int            max_mana;
   sh_int            move;
   sh_int            max_move;
   long              gold;
   long              silver;
   long              gold_bank;
   long              silver_bank;
   int               exp;
   int               exp_total;
   long              act;
   long              act2;
   long              comm; /* RT added to pad the vector */
   long              comm2;
   long              wiznet; /* wiz stuff */
   long              wiznet2;
   long              imm_flags;
   long              res_flags;
   long              vuln_flags;
   sh_int            invis_level;
   sh_int            incog_level;
   long              affected_by;
   long              affected_by2;
   sh_int            position;
   sh_int            practice;
   sh_int            train;
   sh_int            carry_weight;
   sh_int            carry_number;
   sh_int            saving_throw;
   sh_int            saving_maledict;
   sh_int            saving_transport;
   sh_int            saving_spell;
   sh_int            saving_breath;
   sh_int            spell_power;
   sh_int            holy_power;
   sh_int            alignment;
   sh_int            hitroll;
   sh_int            damroll;
   sh_int            armor[4];
   sh_int            wimpy;
   sh_int            true_alignment;
   long              align_mod;
   /* stats */
   sh_int            perm_stat[MAX_STATS];
   sh_int            mod_stat[MAX_STATS];
   /* parts stuff */
   long              form;
   long              parts;
   sh_int            size;
   char*             material;
   /* mobile stuff */
   long              off_flags;
   sh_int            damage[3];
   sh_int            dam_type;
   sh_int            start_pos;
   sh_int            default_pos;
   sh_int            mprog_delay;
   sh_int            pause;
   sh_int            quittime;
   sh_int            bloody_shrine;
   sh_int            ghost;
   CHAR_DATA*        spyfor;
   MOUNT_INFO*       is_riding;
   MESSAGE*          message_list;
   char              conference;
   sh_int            sight;
   sh_int            air_loss;
   char*             host;
   bool              on_line;
   LIST_DATA*        skill_mods;
};

/*
   Data which only PC's have.
*/
struct pc_data
{
   PC_DATA*          next;
   BUFFER*           buffer;
   char*             surname;
   char*             psych_desc;
   char*             dossier;
   char*             last_site;
   char*             first_site;
   bool              valid;
   char*             pwd;      /* md5 */
   char*             old_pwd;  /* old crypt */
   char*             bamfin;
   char*             bamfout;
   char*             title;
   char*             extitle;
   char*             imm_death;
   char*             ignore;
   time_t            last_note;
   time_t            last_idea;
   time_t            last_penalty;
   time_t            last_news;
   time_t            last_changes;
   time_t            last_bank;
   time_t            last_death_note;
   time_t            last_crim;
   time_t            last_clan;
   time_t            probation;
   time_t            prob_time;
   time_t            last_logon;
   sh_int            perm_hit;
   sh_int            perm_mana;
   sh_int            perm_move;
   sh_int            true_sex;
   sh_int            age_mod;
   sh_int            death_timer;
   sh_int            death_status;
   sh_int            ethos;
   sh_int            special;
   sh_int            rebels;
   sh_int            wanteds;
   sh_int            quest;
   char*             quest_name;
   sh_int            death_count;
   sh_int            induct;
   int               last_level;
   sh_int            condition[6];
   sh_int*           learned;
   sh_int*           learnlvl;  /* Level gain skill */
   bool*             updated;   /* changed since last practice/skill list */
   bool              group_known[MAX_GROUP];
   bool              confirm_delete;
   bool              confirm_storage;
   char*             alias[MAX_ALIAS];
   char*             alias_sub[MAX_ALIAS];
   CHAR_DATA*        target;
   ROOM_INDEX_DATA*  spirit_room;
   int               house_rank;
   int               house_rank2;
   int               brand_rank;
   long              targetkills;
   long              xp_pen;
   long              allied_with;
   sh_int            pk_expand;
   sh_int            hacked;
   char*             marker;
   char*             orig_marker;
   time_t            mark_time;
   long              mark_price;
   bool              mark_accepted;
   long              saved_flags;
   bool              being_restored;
   int               last_voted;
   bool              autopurge;
   sh_int            molting_into;
   int               molting_time;
   time_t            last_death;
   ROOM_INDEX_DATA*  last_seen_in;
   CHAR_DATA*        called_by;
   char*             worship;
   sh_int            current_desc;
   char*             desc_list[MAX_DESCRIPTIONS];
   sh_int            stat_bonus[5];
   sh_int            clan;
   char*             clan_name;
   int               worship_slot;
   char*             email;
   char*             nosee_perm;
   AREA_DATA*        music_area;
   char*             moniker;
   ROOM_INDEX_DATA*  linked;
   char*             pre_title;
   int               gained_imm_flags;
   int               gained_res_flags;
   int               gained_vuln_flags;
   int               gained_affects;
   int               gained_affects2;
   int               lost_imm_flags;
   int               lost_res_flags;
   int               lost_vuln_flags;
   int               lost_affects;
   int               lost_affects2;
   time_t            spam;
   sh_int            race_lottery;
   char*             book_description;
   int               deposits[MAX_HOUSE];
   int               life_deposits[MAX_HOUSE];
   int               clan_deposits;
   int               clan_life_deposits;
};

/* Data for generating characters -- only used during generation */
struct gen_data
{
    GEN_DATA    *next;
    bool    valid;
    bool*   skill_chosen;
    bool    group_chosen[MAX_GROUP];
    int        points_chosen;
};



/*
 * Liquids.
 */
#define LIQ_WATER        0

struct    liq_type
{
    char *     liq_name;
    char *     liq_color;
    sh_int     liq_affect[5];
    bool       is_milk;
};



/*
 * Extra description data for a room or object.
 */
struct    extra_descr_data
{
    EXTRA_DESCR_DATA *next;    /* Next in list                     */
    bool valid;
    char *keyword;              /* Keyword in look/examine          */
    char *description;          /* What to see                      */
};



/*
 * Prototype for an object.
 */
struct obj_index_data
{
   OBJ_INDEX_DATA*      next;
   OBJ_INDEX_DATA*      next_in_area;
   EXTRA_DESCR_DATA*    extra_descr;
   AFFECT_DATA *        affected;
   bool                 new_format;
   char *               name;
   char *               short_descr;
   char *               description;
   sh_int               vnum;
   sh_int               reset_num;
   sh_int               house;
   char *               material;
   sh_int               item_type;
   long                 extra_flags;
   long                 extra_flags2;
   int                  wear_flags;
   long                 restrict_flags;
   sh_int               level;
   sh_int               condition;
   sh_int               limtotal;
   sh_int               limcount;
   sh_int               in_game_count;
   sh_int               count;
   sh_int               weight;
   int                  cost;
   int                  value[5];
   int                  moonphases[MAX_MOONS];
   bool                 quitouts;
   MOUNT_DATA *         mount_specs;
   BOOK_DATA*           book_info;
};



/*
 * One object.
 */
struct    obj_data
{
   OBJ_DATA*            next;
   OBJ_DATA*            next_content;
   OBJ_DATA*            contains;
   OBJ_DATA*            in_obj;
   OBJ_DATA*            on;
   CHAR_DATA*           carried_by;
   EXTRA_DESCR_DATA*    extra_descr;
   AFFECT_DATA*         affected;
   OBJ_INDEX_DATA*      pIndexData;
   ROOM_INDEX_DATA*     in_room;
   bool                 valid;
   bool                 enchanted;
   char*                talked;
   char*                owner;
   char*                prev_owners[MAX_OWNERS];
   long                 prev_owners_id[MAX_OWNERS];
   char*                prev_owners_site[MAX_OWNERS];
   char*                name;
   char*                short_descr;
   char*                description;
   sh_int               item_type;
   long                 extra_flags;
   long                 extra_flags2;
   int                  wear_flags;
   sh_int               wear_loc;
   sh_int               weight;
   int                  cost;
   int                  newcost;
   sh_int               level;
   sh_int               condition;
   char*                material;
   sh_int               timer;
   int                  value[5];
   int                  moonphases[MAX_MOONS];
   MOUNT_DATA*          mount_specs;
   MESSAGE*             message_list;
   BOOK_DATA*           book_info;
};



/*
 * Exit data.
 */
struct    exit_data
{
    union
    {
    ROOM_INDEX_DATA *    to_room;
    sh_int            vnum;
    } u1;
    sh_int        exit_info;
    sh_int        key;
    char *        keyword;
    char *        description;
};



/*
 * Reset commands:
 *   '*': comment
 *   'M': read a mobile
 *   'O': read an object
 *   'P': put object in object
 *   'G': give object to mobile
 *   'E': equip object to mobile
 *   'D': set state of door
 *   'R': randomize room exits
 *   'S': stop (end of list)
 */

/*
 * Area-reset definition.
 */
struct reset_data
{
    RESET_DATA *    next;
    char        command;
    sh_int        arg1;
    sh_int        arg2;
    sh_int        arg3;
    sh_int        arg4;
};



/*
 * Area definition.
 */
struct area_data
{
   AREA_DATA*        next;
   RESET_DATA*       reset_first;
   RESET_DATA*       reset_last;
   char*             file_name;
   char*             name;
   char*             credits;
   sh_int            age;
   sh_int            empty_for;
   sh_int            nplayer;
   sh_int            low_range;
   sh_int            high_range;
   sh_int            min_vnum;
   sh_int            max_vnum;
   char*             music;
   bool              empty;
   ROOM_INDEX_DATA*  rooms;
   MOB_INDEX_DATA*   mobs;
   OBJ_INDEX_DATA*   objects;
};

struct list_data
{
   bool        valid;
   bool        string_list;  /* Data is strings, check string match */
   int         size;
   NODE_DATA*  first;
   NODE_DATA*  last;
   LIST_DATA*  next;  /* Only for recycling */
};

struct node_data
{
   bool        valid;
   void*       data;
   NODE_DATA*  next;
   NODE_DATA*  prev;
   LIST_DATA*  list;
};

/*
 * Room type.
 */
struct room_index_data
{
   ROOM_INDEX_DATA*    next;
   ROOM_INDEX_DATA*    next_in_area;
   CHAR_DATA*          people;
   OBJ_DATA*           contents;
   EXTRA_DESCR_DATA*   extra_descr;
   AREA_DATA*          area;
   EXIT_DATA*          exit    [6];
   EXIT_DATA*          old_exit[6];
   ROOM_AFFECT_DATA*   affected;
   long                track_dir[10][2];
   int                 trackhead;
   char*               name;
   char*               description;
   char*               owner;
   char*               sound;
   char                sound_rate;
   sh_int              vnum;
   int                 room_flags;
   int                 extra_room_flags;
   sh_int              light;
   sh_int              sector_type;
   sh_int              heal_rate;
   sh_int              mana_rate;
   sh_int              house;
   sh_int              guild;
   ALARM_DATA*         alarm;
};


/*
 * MOBprog definitions
 */
#define TRIG_ACT      (A)
#define TRIG_BRIBE    (B)
#define TRIG_DEATH    (C)
#define TRIG_ENTRY    (D)
#define TRIG_FIGHT    (E)
#define TRIG_GIVE     (F)
#define TRIG_GREET    (G)
#define TRIG_GRALL    (H)
#define TRIG_KILL     (I)
#define TRIG_HPCNT    (J)
#define TRIG_RANDOM   (K)
#define TRIG_SPEECH   (L)
#define TRIG_EXIT     (M)
#define TRIG_EXALL    (N)
#define TRIG_DELAY    (O)
#define TRIG_SURR     (P)

struct mprog_list
{
    int               trig_type;
    char *            trig_phrase;
    sh_int            vnum;
    char *            code;
    MPROG_LIST *      next;
};

struct mprog_code
{
    sh_int            vnum;
    char *            code;
    MPROG_CODE *      next;
};


/*
 * Types of attacks.
 * Must be non-overlapping with spell/skill types,
 * but may be arbitrary beyond that.
 */
#define TYPE_UNDEFINED               -1
#define TYPE_HIT                     1000



/*
 *  Target types.
 */
#define TAR_IGNORE            0
#define TAR_CHAR_OFFENSIVE        1
#define TAR_CHAR_DEFENSIVE        2
#define TAR_CHAR_SELF            3
#define TAR_OBJ_INV            4
#define TAR_OBJ_CHAR_DEF        5
#define TAR_OBJ_CHAR_OFF        6
#define TAR_OBJ                     7
#define TAR_CHAR_HEALING            8

#define TARGET_CHAR            0
#define TARGET_OBJ            1
#define TARGET_ROOM            2
#define TARGET_NONE            3


/*
   Things in strip_type and reflect_type
   Name              Table updated     Implemented fully in code
   no_dispel         TRUE              TRUE
   death_keep        TRUE              TRUE
   astrip_keep       TRUE              TRUE
   spellbane_pc      TRUE              TRUE
   spellbane_npc     TRUE              TRUE
   absorb_pc         TRUE              TRUE
   absorb_npc        TRUE              TRUE
   spellbane_dam_pc  TRUE              TRUE
   spellbane_dam_npc TRUE              TRUE
   absorb_dam_pc     TRUE              TRUE
   absorb_dam_npc    TRUE              TRUE
   silent            TRUE              TRUE
   level_reduce      TRUE              TRUE
   ALL CASTING    \
   routed to new  \
   function, so it\
   can use these  \
   things            TRUE              TRUE

*/

struct   strip_type
{
   bool           no_dispel;     /* Cannot be removed by check_dispel (harmony)  */
   bool           death_keep;    /* Kept after death           */
   bool           astrip_keep;   /* Ignores non-specific astrip */
};

struct   magic_type
{
   bool           spellbane_pc;      /* can be spellbaned when cast by players */
   bool           spellbane_npc;     /* can be spellbaned when cast by NPCs    */
   bool           absorb_pc;         /* can be absorbed when cast by players   */
   bool           absorb_npc;        /* can be absorbed when cast by npcs      */
   bool           silent;            /* No words are used when cast            */
   bool           level_reduce;      /* double lag until 15 levels later       */
   bool           copy_clone;        /* Copy affects when cloned */
};

/*
 * Skills include spells as a particular case.
 */
struct    skill_type
{
   char*          name;                      /* Name of skill        */
   sh_int         skill_level[MAX_CLASS];    /* Level needed by class    */
   sh_int         rating[MAX_CLASS];         /* How hard it is to learn    */
   SPELL_FUN*     spell_fun;                 /* Spell pointer (for spells)    */
   sh_int         target;                    /* Legal targets        */
   sh_int         minimum_position;          /* Position for caster / user    */
   sh_int*        pgsn;                      /* Pointer to associated gsn    */
   /* sh_int      slot;                      Slot for #OBJECT loading     */
   sh_int         min_mana;                  /* Minimum mana used        */
   sh_int         beats;                     /* Waiting time after use    */
   char*          noun_damage;               /* Damage message        */
   char*          msg_off;                   /* Wear off message        */
   char*          msg_obj;                   /* Wear off message for obects    */
   STRIP_DATA     strip;                     /* How this affect gets stripped */
   MAGIC_DATA     spells;                    /* Magic information, spellbane/absorb/double lag 15 levels */
};

struct  group_type
{
    char *    name;
    sh_int    rating[MAX_CLASS];
    char *    spells[MAX_IN_GROUP];
};


/*
   These are race numbers
*/
extern sh_int  grn_unique;
extern sh_int  grn_spider;
extern sh_int  grn_human;
extern sh_int  grn_dwarf;
extern sh_int  grn_elf;
extern sh_int  grn_grey_elf;
extern sh_int  grn_dark_elf;
extern sh_int  grn_centaur;
extern sh_int  grn_troll;
extern sh_int  grn_giant;
extern sh_int  grn_gnome;
extern sh_int  grn_draconian;
extern sh_int  grn_ethereal;
extern sh_int  grn_changeling;
extern sh_int  grn_illithid;
extern sh_int  grn_halfling;
extern sh_int  grn_minotaur;
extern sh_int  grn_arborian;
extern sh_int  grn_book;
extern sh_int  grn_demon;
extern sh_int  grn_bat;
extern sh_int  grn_bear;
extern sh_int  grn_cat;
extern sh_int  grn_centipede;
extern sh_int  grn_duergar;
extern sh_int  grn_dog;
extern sh_int  grn_doll;
extern sh_int  grn_doppelganger;
extern sh_int  grn_dragon;
extern sh_int  grn_fido;
extern sh_int  grn_fish;
extern sh_int  grn_fox;
extern sh_int  grn_goblin;
extern sh_int  grn_hobgoblin;
extern sh_int  grn_kobold;
extern sh_int  grn_lizard;
extern sh_int  grn_modron;
extern sh_int  grn_pig;
extern sh_int  grn_rabbit;
extern sh_int  grn_school_monster;
extern sh_int  grn_snake;
extern sh_int  grn_song_bird;
extern sh_int  grn_water_fowl;
extern sh_int  grn_wolf;
extern sh_int  grn_wyvern;

/*
 * These are skill_lookup return values for common skills and spells.
 */
extern int   demon_eggs;
extern int   demon_babies;
extern int   demon_adults;
extern int   sAllocPerm;
extern int   sAllocString;
extern long  total_affects;
extern bool  resolve_no_ip;
extern bool log_mobs;
extern int   autoreboot_hour;
extern int   autoreboot_warning_hour;
extern bool  immrp_revolt;
extern bool  immrp_darkness;
extern bool  immrp_blood;
extern bool    all_magic_gone;
extern bool  allow_rang;
extern bool  allow_elem;
extern bool  allow_thief;     /* global to allow Subclasses */
extern long  mem_counts[15];  /* counts all 14 types of perm mem */
extern long  mem_used[42];
extern int   hero_count;
extern int  frog_house;

extern sh_int  gsn_hyper;
extern sh_int  gsn_nap;
extern sh_int  gsn_nap_timer;
extern sh_int  gsn_nap_slow;

extern sh_int  gsn_spiritblade;
extern sh_int  gsn_team_spirit;
extern sh_int  gsn_power_word_fear;
extern sh_int  gsn_disintegrate;
extern sh_int  gsn_ventriloquate;
extern sh_int  gsn_iron_scales_dragon;
extern sh_int  gsn_muster;
extern sh_int  gsn_perception;
extern sh_int  gsn_hand_of_vengeance;
extern sh_int  gsn_trace;
extern sh_int  gsn_prismatic_sphere;
extern sh_int  gsn_simulacrum;
extern sh_int  gsn_rite_of_darkness;
extern sh_int  gsn_rite_of_darkness_timer;
extern sh_int  gsn_rite_of_darkness_taint;
extern sh_int  gsn_dracolich_evil;
extern sh_int  gsn_dracolich_bloodlust;
extern sh_int  gsn_intangibility;
extern sh_int  gsn_intangibility_timer;
extern sh_int  gsn_phantasmal_force;
extern sh_int  gsn_phantasmal_force_area;
extern sh_int  gsn_phantasmal_force_illusion;
extern sh_int  gsn_siren_screech;
extern sh_int  gsn_wail_wind;
extern sh_int  gsn_esurience;
extern sh_int  gsn_venueport;
extern sh_int  gsn_scribe;
extern sh_int  gsn_virulent_cysts;
extern sh_int  gsn_seize;

extern sh_int  gsn_purity;
extern sh_int  gsn_helper_robe;
extern sh_int  gsn_darkfocus;
extern sh_int  gsn_dark_armor;
extern sh_int  gsn_impale;
extern sh_int  gsn_swing;
extern sh_int  gsn_jump;
extern sh_int  gsn_command;
extern sh_int  gsn_aura_of_presence;
extern sh_int  gsn_recruit;

extern sh_int  gsn_strengthen_bone;

extern sh_int  gsn_resurrection;

extern sh_int  gsn_channel;
extern sh_int  gsn_warding;
extern sh_int  gsn_guard_call;

/* dragon breaths */
extern sh_int  gsn_acid_breath;
extern sh_int  gsn_fire_breath;
extern sh_int  gsn_frost_breath;
extern sh_int  gsn_gas_breath;
extern sh_int  gsn_lightning_breath;

/* Worship-related */
extern sh_int  gsn_worship;
extern sh_int  gsn_blessing;
extern sh_int  gsn_detriment;
extern sh_int  gsn_punishment;
extern sh_int  gsn_teleport;
extern sh_int  gsn_plague_of_the_worm;
extern sh_int  gsn_worm_damage;
extern sh_int  gsn_divine_inferno;
extern sh_int  gsn_lightblast;
extern sh_int  gsn_word_recall;
extern sh_int  gsn_power_word_recall;
extern sh_int  gsn_need;
extern sh_int  gsn_gate;
extern sh_int  gsn_summon;


/* Vrrin's Brand */
extern sh_int  gsn_will_power;
extern sh_int  gsn_will_dread;
extern sh_int  gsn_will_oblivion;
extern sh_int  gsn_mantle_oblivion;

/* Malignus's Brand */
extern sh_int  gsn_mob_timer;
extern sh_int  gsn_mob_pain;

/* Herb-spells */
extern sh_int  gsn_heal;
extern sh_int  gsn_cure_blindness;
extern sh_int  gsn_cure_disease;
extern sh_int  gsn_cure_poison;
extern sh_int  gsn_imbue_regeneration;
extern sh_int  gsn_giant_strength;
extern sh_int  gsn_remove_curse;
extern sh_int  gsn_cancellation;

/* elem sub spells */
extern sh_int  gsn_gale_winds;
extern sh_int  gsn_air_dagger;
extern sh_int  gsn_napalm;
extern sh_int  gsn_incinerate;
extern sh_int  gsn_sunburst;
extern sh_int  gsn_nova;
extern sh_int  gsn_inferno;
extern sh_int  gsn_flame_form;
extern sh_int  gsn_imbue_flame;
extern sh_int  gsn_ashes_to_ashes;
extern sh_int  gsn_raging_fire;

extern sh_int  gsn_charge_weapon;
extern sh_int  gsn_tornado;
extern sh_int  gsn_airshield;
extern sh_int  gsn_suffocate;
extern sh_int  gsn_jet_stream;
extern sh_int  gsn_cyclone;
extern sh_int  gsn_implosion;
extern sh_int  gsn_thunder;
extern sh_int  gsn_storm;
extern sh_int  gsn_mass_fly;
extern sh_int  gsn_dust_devil;
extern sh_int  gsn_vortex;
extern sh_int  gsn_wraithform;

extern sh_int  gsn_earthbind;
extern sh_int  gsn_stone;
extern sh_int  gsn_stalagmite;
extern sh_int  gsn_tremor;
extern sh_int  gsn_avalanche;
extern sh_int  gsn_cave_in;
extern sh_int  gsn_crushing_hands;
extern sh_int  gsn_shield_of_earth;
extern sh_int  gsn_stoney_grasp;
extern sh_int  gsn_burrow;
extern sh_int  gsn_meteor_storm;
extern sh_int  gsn_earth_form;

extern sh_int  gsn_geyser;
extern sh_int  gsn_water_spout;
extern sh_int  gsn_deluge;
extern sh_int  gsn_whirlpool;
extern sh_int  gsn_blizzard;
extern sh_int  gsn_spring_rains;
extern sh_int  gsn_submerge;
extern sh_int  gsn_freeze;
extern sh_int  gsn_frost_charge;
extern sh_int  gsn_waterwalk;
extern sh_int  gsn_waterbreathing;
extern sh_int  gsn_ice_armor;
extern sh_int  gsn_water_form;
extern sh_int  gsn_water_of_life;
extern sh_int  gsn_fountain_of_youth;
extern sh_int  gsn_rip_tide;

extern sh_int  gsn_tanning;
extern sh_int  gsn_hire_crew;
extern sh_int  gsn_thrust;
extern sh_int  gsn_rapier;
extern sh_int  gsn_tumble;
extern sh_int  gsn_offhand_disarm;
extern sh_int  gsn_emblem_law;
extern sh_int  gsn_seal_of_justice;
extern sh_int  gsn_ransack;
extern sh_int  gsn_entrench;
extern sh_int  gsn_charging_retreat;
extern sh_int  gsn_block_retreat;
extern sh_int  gsn_gag;
extern sh_int  gsn_bind;
extern sh_int  gsn_blindfold;
extern sh_int  gsn_bushwhack;
extern sh_int  gsn_lookout;
extern sh_int  gsn_ninjitsu;
extern sh_int  gsn_sharpen;
extern sh_int  gsn_dual_parry;
extern sh_int  gsn_battle_tactics;
extern sh_int  gsn_critical_strike;
extern sh_int  gsn_feign_death;
extern sh_int  gsn_caltrops;
extern sh_int  gsn_sham_brand1;
extern sh_int  gsn_sham_brand2;
extern sh_int  gsn_delusions;
extern sh_int  gsn_obscure;
extern sh_int  gsn_anvil_brand;
extern sh_int  gsn_mute;
extern sh_int  gsn_getaway;
extern sh_int  gsn_trapstun;
extern sh_int  gsn_trapmaking;
extern sh_int  gsn_beaststance;
extern sh_int  gsn_killer_instinct;
extern sh_int  gsn_battlescream;
extern sh_int  gsn_carving;
extern sh_int  gsn_toughen;
extern sh_int  gsn_crushingblow;
extern sh_int  gsn_wildfury;
extern sh_int  gsn_skull_bash;
extern sh_int  gsn_globe_darkness;
extern sh_int  gsn_demon_swarm;
extern sh_int  gsn_warpaint;
extern sh_int  gsn_two_hand_wield;
extern sh_int  gsn_toad;
extern sh_int  gsn_taunt;
extern sh_int  gsn_nausea;
extern sh_int  gsn_fist_god;
extern sh_int  gsn_justice_brand2;
extern sh_int  gsn_justice_brand;
extern sh_int  gsn_justice_brand_wrath;
extern sh_int  gsn_alchemy;
extern sh_int  gsn_temporal_shield;
extern sh_int  gsn_temporal_shear;
extern sh_int  gsn_grace;
extern sh_int  gsn_identify;
extern sh_int  gsn_know_alignment;
extern sh_int  gsn_know_time;
extern sh_int  gsn_halo_of_eyes;
extern sh_int  gsn_shrink;
extern sh_int  gsn_enlarge;
extern sh_int  gsn_illusionary_wall;
extern sh_int  gsn_strabismus;
extern sh_int  gsn_fog_conceilment;
extern sh_int  gsn_hall_mirrors;
extern sh_int  gsn_phasing;
extern sh_int  gsn_cloak_brave;
extern sh_int  gsn_benediction;
extern sh_int  gsn_barrier;
extern sh_int  gsn_mentallink;
extern sh_int  gsn_backstab;
extern sh_int  gsn_detect_hidden;
extern sh_int  gsn_blind_fighting;
extern sh_int  gsn_dodge;
extern sh_int  gsn_envenom;
extern sh_int  gsn_hide;
extern sh_int  gsn_adv_hide;
extern sh_int  gsn_exp_hide;
extern sh_int  gsn_bribe;
extern sh_int  gsn_peek;
extern sh_int  gsn_pick_lock;
extern sh_int  gsn_sneak;
extern sh_int  gsn_steal;
extern sh_int  gsn_plant;
extern sh_int  gsn_fence;
extern sh_int  gsn_silence;
extern sh_int  gsn_disarm;
extern sh_int  gsn_firemastery;
extern sh_int  gsn_watermastery;
extern sh_int  gsn_airmastery;
extern sh_int  gsn_earthmastery;
extern sh_int  gsn_enhanced_damage;
extern sh_int  gsn_kick;
extern sh_int  gsn_parry;
extern sh_int  gsn_rescue;
extern sh_int  gsn_second_attack;
extern sh_int  gsn_third_attack;
extern sh_int  gsn_fourth_attack;
extern sh_int  gsn_fifth_attack;
extern sh_int  gsn_sixth_attack;

extern sh_int  gsn_side_step;
extern sh_int  gsn_blindness;

extern sh_int  gsn_repent;
extern sh_int  gsn_faith_healing;

extern sh_int  gsn_syphon_soul;
extern sh_int  gsn_charm_person;
/* Outlaw */
extern sh_int  gsn_propaganda;
extern sh_int  gsn_aura_defiance;
extern sh_int  gsn_stealth;
extern sh_int  gsn_smoke_screen;
extern sh_int  gsn_kidnap;
extern sh_int  gsn_backup;
extern sh_int  gsn_smoke_screen;
extern sh_int  gsn_fence;
extern sh_int  gsn_offer;


extern sh_int  gsn_distort_time;
extern sh_int  gsn_distort_time_faster;
extern sh_int  gsn_distort_time_slower;
extern sh_int  gsn_smite;
extern sh_int  gsn_ancient_plague;
extern sh_int  gsn_boiling_blood;
extern sh_int  gsn_bloodmist;
extern sh_int  gsn_curse;
extern sh_int  gsn_invis;
extern sh_int  gsn_mass_invis;
extern sh_int  gsn_plague;
extern sh_int  gsn_poison;
extern sh_int  gsn_sleep;
extern sh_int  gsn_sleep_timer;
extern sh_int  gsn_soulcraft;
extern sh_int  gsn_fly;
extern sh_int  gsn_sanctuary;
extern sh_int  gsn_chromatic_shield;
extern sh_int  gsn_divine_protect;

/* new gsns */
extern sh_int  gsn_axe;
extern sh_int  gsn_dagger;
extern sh_int  gsn_flail;
extern sh_int  gsn_mace;
extern sh_int  gsn_polearm;
extern sh_int  gsn_shield_block;
extern sh_int  gsn_spear;
extern sh_int  gsn_sword;
extern sh_int  gsn_whip;

extern sh_int  gsn_adv_sword;
extern sh_int  gsn_exp_sword;
extern sh_int  gsn_adv_spear;
extern sh_int  gsn_exp_spear;
extern sh_int  gsn_adv_dagger;
extern sh_int  gsn_exp_dagger;
extern sh_int  gsn_adv_mace;
extern sh_int  gsn_exp_mace;
extern sh_int  gsn_adv_polearm;
extern sh_int  gsn_exp_polearm;
extern sh_int  gsn_adv_axe;
extern sh_int  gsn_exp_axe;
extern sh_int  gsn_adv_staff;
extern sh_int  gsn_exp_staff;
extern sh_int  gsn_adv_whip;
extern sh_int  gsn_exp_whip;
extern sh_int  gsn_adv_flail;
extern sh_int  gsn_exp_flail;
extern sh_int  gsn_archery;
extern sh_int  gsn_adv_archery;
extern sh_int  gsn_exp_archery;

extern sh_int  gsn_shackles;
extern sh_int  gsn_shackle;
extern sh_int  gsn_shieldbash;
extern sh_int  gsn_bash;
extern sh_int  gsn_berserk;
extern sh_int  gsn_rage;
extern sh_int  gsn_dirt;
extern sh_int  gsn_hand_to_hand;
extern sh_int  gsn_trip;

extern sh_int  gsn_recovery;
extern sh_int  gsn_fast_healing;
extern sh_int  gsn_haggle;
extern sh_int  gsn_lore;
extern sh_int  gsn_soulscry;
extern sh_int  gsn_diagnose;
extern sh_int  gsn_meditation;
extern sh_int  gsn_meteor;

extern sh_int  gsn_scrolls;
extern sh_int  gsn_staves;
extern sh_int  gsn_wands;
extern sh_int  gsn_recall;

extern sh_int  gsn_morph_red;
extern sh_int  gsn_morph_green;
extern sh_int  gsn_morph_black;
extern sh_int  gsn_morph_white;
extern sh_int  gsn_morph_blue;
extern sh_int  gsn_morph_winged;
extern sh_int  gsn_morph_archangel;

extern sh_int  gsn_morph_dragon;
extern sh_int  gsn_breath_morph;
extern sh_int  gsn_mimic;
extern sh_int  gsn_liquify;
extern sh_int  gsn_morph_weapon;

extern sh_int  gsn_crush;
extern sh_int  gsn_ground_control;

extern sh_int  gsn_absorb;
extern sh_int  gsn_camouflage;
extern sh_int  gsn_acute_vision;
extern sh_int  gsn_ambush;
extern sh_int  gsn_laying_hands;
extern sh_int  gsn_battlecry;
extern sh_int  gsn_whirlwind;
extern sh_int  gsn_circle;
extern sh_int  gsn_empower;
extern sh_int  gsn_dual_backstab;
extern sh_int  gsn_power_word_stun;
extern sh_int  gsn_power_word_fear;
extern sh_int  gsn_skin;
extern sh_int  gsn_camp;
extern sh_int  gsn_steel_nerves;
extern sh_int  gsn_animate_dead;
extern sh_int  gsn_barkskin;
extern sh_int  gsn_animal_call;
extern sh_int  gsn_aura_of_sustenance;
extern sh_int  gsn_halo_of_the_sun;
extern sh_int  gsn_shroud;
extern sh_int  gsn_shadowgate;
extern sh_int  gsn_eye_of_the_predator;
extern sh_int  gsn_blackjack;
extern sh_int  gsn_lunge;
extern sh_int  gsn_vigilance;
extern sh_int  gsn_dual_wield;
extern sh_int  gsn_trance;
extern sh_int  gsn_wanted;
extern sh_int  gsn_cleave;
extern sh_int  gsn_herb;
extern sh_int  gsn_bandage;
extern sh_int  gsn_deathstrike;
extern sh_int  gsn_protective_shield;
extern sh_int  gsn_consecrate;
extern sh_int  gsn_timestop;
extern sh_int  gsn_timestop_done;
extern sh_int  gsn_true_sight;
extern sh_int  gsn_butcher;
extern sh_int  gsn_shadowstrike;
extern sh_int  gsn_battleshield;
extern sh_int  gsn_counter_parry;
extern sh_int  gsn_riposte;
extern sh_int  gsn_fireshield;
extern sh_int  gsn_iceshield;
extern sh_int  gsn_high_herb;
extern sh_int  gsn_vanish;
extern sh_int  gsn_riot;
extern sh_int  gsn_embalm;
extern sh_int  gsn_dark_dream;
extern sh_int  gsn_counter;
extern sh_int  gsn_chaos_mind;
extern sh_int  gsn_revolt;
extern sh_int  gsn_acid_spit;
extern sh_int  gsn_bear_call;
extern sh_int  gsn_trophy;
extern sh_int  gsn_tail;
extern sh_int  gsn_spellbane;
extern sh_int  gsn_request;
extern sh_int  gsn_endure;
extern sh_int  gsn_dragon_spirit;
extern sh_int  gsn_palm;
extern sh_int  gsn_throw;
extern sh_int  gsn_nerve;
extern sh_int  gsn_follow_through;
extern sh_int  gsn_poison_dust;
extern sh_int  gsn_blindness_dust;
extern sh_int  gsn_roll;
extern sh_int  gsn_block;
extern sh_int  gsn_strangle;
extern sh_int  gsn_warcry;
extern sh_int  gsn_no_magic_toggle;
extern sh_int  gsn_ramming;
extern sh_int  gsn_chimera_lion;
extern sh_int  gsn_chimera_goat;
extern sh_int  gsn_strange_form;
extern sh_int  gsn_downstrike;
extern sh_int  gsn_enlist;
extern sh_int  gsn_shadowplane;
extern sh_int  gsn_blackjack_timer;
extern sh_int  gsn_strangle_timer;
extern sh_int  gsn_tame;
extern sh_int  gsn_track;
extern sh_int  gsn_find_water;
extern sh_int  gsn_shield_cleave;
extern sh_int  gsn_spellcraft;
extern sh_int  gsn_cloak_form;
extern sh_int  gsn_demand;
extern sh_int  gsn_tertiary_wield;
extern sh_int  gsn_breath_fire;
extern sh_int  gsn_awareness;
extern sh_int  gsn_forest_blending;
extern sh_int  gsn_gaseous_form;
extern sh_int  gsn_rear_kick;
extern sh_int  gsn_shapeshift;
extern sh_int  gsn_regeneration;
extern sh_int  gsn_parrot;
extern sh_int  gsn_door_bash;
extern sh_int  gsn_multistrike;
extern sh_int  gsn_wolverine_brand;
extern sh_int  gsn_flame_scorch;
extern sh_int  gsn_flame_scorch_blue;
extern sh_int  gsn_flame_scorch_green;
extern sh_int  gsn_mutilated_left_arm;
extern sh_int  gsn_mutilated_left_hand;
extern sh_int  gsn_mutilated_left_leg;
extern sh_int  gsn_mutilated_right_arm;
extern sh_int  gsn_mutilated_right_hand;
extern sh_int  gsn_mutilated_right_leg;
extern sh_int  gsn_shattered_bone;
extern sh_int  gsn_blood_tide;
extern sh_int  gsn_flail_arms;
extern sh_int  gsn_soulbind;
extern sh_int  gsn_earthfade;
extern sh_int  gsn_forget;
extern sh_int  gsn_divine_touch;
extern sh_int  gsn_grounding;
extern sh_int  gsn_shock_sphere;
extern sh_int  gsn_forage;
extern sh_int  gsn_assassinate;
extern sh_int  gsn_defend;
extern sh_int  gsn_intimidate;
extern sh_int  gsn_escape;
extern sh_int  gsn_moving_ambush;
extern sh_int  gsn_pugil;
extern sh_int  gsn_staff;
extern sh_int  gsn_evaluation;
extern sh_int  gsn_enhanced_damage_two;
extern sh_int  gsn_protection_heat_cold;
extern sh_int  gsn_rally;
extern sh_int  gsn_tactics;
extern sh_int  gsn_lash;
extern sh_int  gsn_prevent_healing;
extern sh_int  gsn_regeneration;
extern sh_int  gsn_undead_drain;
extern sh_int  gsn_quiet_movement;
extern sh_int  gsn_iron_resolve;
extern sh_int  gsn_atrophy;
extern sh_int  gsn_focus;
extern sh_int  gsn_psionic_blast;  /* illithids - runge */
extern sh_int  gsn_spike;
extern sh_int  gsn_starvation;
extern sh_int  gsn_black_death;
extern sh_int  gsn_black_scythe;
extern sh_int  gsn_spore;
extern sh_int  gsn_barbarian_strike;
extern sh_int  gsn_barbarian_bone_tear;
extern sh_int  gsn_dehydrated;  /* For starve/thirst */
extern sh_int  gsn_phoenix;  /* Riallus's brand */
extern sh_int  gsn_void;  /* Xurinos's brand */
extern sh_int  gsn_peace;  /* Cirdan's brand's effect */
extern sh_int  gsn_peace_brand;  /* Cirdan's brand */
extern sh_int  gsn_owl_brand;  /* Wervdon's brand */
extern sh_int  gsn_toothless;  /* Thrym's brand */
extern sh_int  gsn_lemniscate_brand;
extern sh_int  gsn_worm_brand;  /* Runge's combat affect - Runge */
extern sh_int  gsn_worm_hide;  /* Runge's anti-where affect - Runge */
extern sh_int  gsn_battleaxe_haste;  /* Darkwood's haste power */
extern sh_int  gsn_battleaxe_defense;  /* Darkwood's save vs. spells */
extern sh_int  gsn_hector_brand_spark;  /* Hector's brand - "Spark of Innovation" power */
extern sh_int  gsn_hector_brand_essence;  /* Hector's brand - "Essence Identity" power */
extern sh_int  gsn_lestregus_brand_eternal; /* Lestregus's Detect Good */
extern sh_int  gsn_lestregus_brand_taint; /* Lestregus's Taint power */
extern sh_int  gsn_lestregus_brand_thirst; /* Lestregus's Thirst affect */
extern sh_int  gsn_utara_brand_blur;  /* Utara's brand - "Blur of the Gremlin" power */
extern sh_int  gsn_utara_brand2;  /* Utara's brand - invoke timer*/
extern sh_int  gsn_charm_timer;  /* charm timer - Wervdon */
extern sh_int  gsn_drinlinda_attack_brand; /* Drin's brand attack */
extern sh_int  gsn_drinlinda_defense_brand; /* Drin's brand defense */
extern sh_int  gsn_drithentir_attack_brand; /* Drithentir's attack for brand */
extern sh_int  gsn_drithentir_vampiric_brand; /* Drithentir's vampiric aura for brand */
extern sh_int  gsn_vermin_timer;
extern sh_int  gsn_chi_healing;
extern sh_int  gsn_eye_tiger;
extern sh_int  gsn_rot;
extern sh_int  gsn_flood_room;
extern sh_int  gsn_forestwalk;
extern sh_int  gsn_charge;
extern sh_int  gsn_blitz;
extern sh_int  gsn_summon_steed;
extern sh_int  gsn_doublestrike;
extern sh_int  gsn_support;
extern sh_int  gsn_rebel;
extern sh_int  gsn_devote;
extern sh_int  gsn_lurk;
extern sh_int  gsn_surround;
extern sh_int  gsn_target;
extern sh_int  gsn_darkforge;
extern sh_int  gsn_dimmak;
extern sh_int  gsn_cripple;
extern sh_int  gsn_sdragon_armor;
extern sh_int  gsn_sdragon_valor;
extern sh_int  gsn_kung_fu;
extern sh_int  gsn_chant;
extern sh_int  gsn_healing_trance;
extern sh_int  gsn_spiritwalk;
extern sh_int  gsn_pouncing_tiger;
extern sh_int  gsn_knockdown;
extern sh_int  gsn_darkforge_fists;
extern sh_int  gsn_fists_fire;
extern sh_int  gsn_fists_ice;
extern sh_int  gsn_fists_poison;
extern sh_int  gsn_fists_divine;
extern sh_int  gsn_fists_fury;
extern sh_int  gsn_open_claw;
extern sh_int  gsn_harmony;
extern sh_int  gsn_iron_will;
extern sh_int  gsn_stunning_strike_timer;
extern sh_int  gsn_stunning_strike;
extern sh_int  gsn_counter_defense;
extern sh_int  gsn_chi_attack;
extern sh_int  gsn_acrobatics;
extern sh_int  gsn_evasive_dodge;
extern sh_int  gsn_dchant;
extern sh_int  gsn_judo;
extern sh_int  gsn_dance_venom;
extern sh_int  gsn_subrank;
extern sh_int  gsn_jump_kick;
extern sh_int  gsn_spin_kick;
extern sh_int  gsn_iron_palm;
extern sh_int  gsn_nether_shroud;
extern sh_int  gsn_black_mantle;
extern sh_int  gsn_coil_snakes;
extern sh_int  gsn_dragonmount;
extern sh_int  gsn_hang;
extern sh_int  gsn_conjure;
extern sh_int  gsn_drain;
extern sh_int  gsn_tinker;
extern sh_int  gsn_forge_weapon;
extern sh_int  gsn_sense_evil;
extern sh_int  gsn_armor_of_god;
extern sh_int  gsn_guardian_angel;
extern sh_int  gsn_lightbind;
extern sh_int  gsn_telekinesis;
extern sh_int  gsn_wind_shear;
extern sh_int  gsn_sonicbelch;
extern sh_int  gsn_trollfart;
extern sh_int  gsn_taichi;
extern sh_int  gsn_star_shower;
extern sh_int  gsn_davatar_assault;
extern sh_int  gsn_penetratingthrust;
extern sh_int  gsn_wagon_strike;
extern sh_int  gsn_megaslash;

/* added originally for efficiency: */
extern sh_int  gsn_cure_critical;
extern sh_int  gsn_firestream;
extern sh_int  gsn_misdirection;
extern sh_int  gsn_heavenly_wrath;
extern sh_int  gsn_create_spring;
extern sh_int  gsn_create_food;
extern sh_int  gsn_create_water;
extern sh_int  gsn_mass_healing;
extern sh_int  gsn_windwall;
extern sh_int  gsn_detect_good;
extern sh_int  gsn_detect_evil;
extern sh_int  gsn_haste;
extern sh_int  gsn_bless;
extern sh_int  gsn_faerie_fog;
extern sh_int  gsn_faerie_fire;
extern sh_int  gsn_infravision;
extern sh_int  gsn_pass_door;
extern sh_int  gsn_voodoo;
extern sh_int  gsn_earthquake;
extern sh_int  gsn_call_of_duty;
extern sh_int  gsn_holy_fire;
extern sh_int  gsn_blade_barrier;
extern sh_int  gsn_demonfire;
extern sh_int  gsn_reserved;
extern sh_int  gsn_heavenly_fire;
extern sh_int  gsn_protection_evil;
extern sh_int  gsn_protection_good;
extern sh_int  gsn_chill_touch;
extern sh_int  gsn_concatenate;
extern sh_int  gsn_dispel_evil;
extern sh_int  gsn_dispel_good;
extern sh_int  gsn_energy_drain;
extern sh_int  gsn_fireball;
extern sh_int  gsn_demonic_enchant;
extern sh_int  gsn_frenzy;
extern sh_int  gsn_canopy_walk;
extern sh_int  gsn_weaken;
extern sh_int  gsn_shocking_grasp;
extern sh_int  gsn_mind_sear;
extern sh_int  gsn_lightning_bolt;
extern sh_int  gsn_acid_blast;
extern sh_int  gsn_shroud_of_pro;
extern sh_int  gsn_ameteor;
extern sh_int  gsn_beastial_command;
extern sh_int  gsn_call_of_the_wild;
extern sh_int  gsn_cure_light;
extern sh_int  gsn_cure_serious;
extern sh_int  gsn_restoration;
extern sh_int  gsn_refresh;
extern sh_int  gsn_power_word_kill;
extern sh_int  gsn_blessing_of_darkness;
extern sh_int  gsn_insight;
extern sh_int  gsn_defilement;
extern sh_int  gsn_lifeline;
extern sh_int  gsn_cloak_of_transgression;
extern sh_int  gsn_decoy;
extern sh_int  gsn_health;
extern sh_int  gsn_forge;
extern sh_int  gsn_armor;
extern sh_int  gsn_ray_of_enfeeblement;
extern sh_int  gsn_calm;
extern sh_int  gsn_change_sex;
extern sh_int  gsn_detect_invis;
extern sh_int  gsn_detect_magic;
extern sh_int  gsn_shield;
extern sh_int  gsn_slow;
extern sh_int  gsn_stone_skin;
extern sh_int  gsn_holyshield;
extern sh_int  gsn_mass_slow;
extern sh_int  gsn_wrath;
extern sh_int  gsn_dark_wrath;
extern sh_int  gsn_mass_haste;
extern sh_int  gsn_hunters_knife;
extern sh_int  gsn_tsunami;
extern sh_int  gsn_iceball;
extern sh_int  gsn_cone_of_cold;
extern sh_int  gsn_magic_missile;
extern sh_int  gsn_colour_spray;
extern sh_int  gsn_fire_and_ice;
extern sh_int  gsn_earthmaw;
extern sh_int  gsn_wither;
extern sh_int  gsn_taint;
extern sh_int  gsn_gout_of_maggots;
extern sh_int  gsn_strengthen_construct;
extern sh_int  gsn_darkshield;
extern sh_int  gsn_lightshield;
extern sh_int  gsn_lower_resistance;
extern sh_int  gsn_raise_resistance;
extern sh_int  gsn_invisible_mail;
extern sh_int  gsn_freeze_person;
extern sh_int  gsn_dispel_magic;
extern sh_int  gsn_retribution;
extern sh_int  gsn_chain_lightning;
extern sh_int  gsn_unholy_fire;
extern sh_int  gsn_familiar;
extern sh_int  gsn_deathspell;
extern sh_int  gsn_purify;
extern sh_int  gsn_psistorm;
extern sh_int  gsn_dancestaves;
extern sh_int  gsn_acidfire;
extern sh_int  gsn_devour;
extern sh_int  gsn_smelly;
extern sh_int  gsn_carebearstare;
extern sh_int  gsn_krazyeyes;
extern sh_int  gsn_stalking;
extern sh_int  gsn_field_surgery;
extern sh_int  gsn_deathfire_missiles;
extern sh_int  gsn_abyssal_claws;
extern sh_int  gsn_destruction_shockwave;
extern sh_int  gsn_evasive_defense;


/* Random mob vnum ranges are used with get_random_mob() functions seek intervals */

#define RANDOM_MOB_GRAB_CHANCE               95  /* Chance for a mob in the vnum range to be picked by get_random_mob */
#define RANDOM_MOB_VNUM_RANGE              1000  /* To be used in combination with vnum_range_size constants          */
#define RANDOM_MOB_VNUM_RANGE_SMALL           1  /* Finer graining may result in higher cpu load - slower functioning */
#define RANDOM_MOB_VNUM_RANGE_MEDIUM          5
#define RANDOM_MOB_VNUM_RANGE_LARGE          10
#define RANDOM_MOB_VNUM_RANGE_HUGE           25

/* Marauder skillset and defines */

#define MARAUDER_MISINFORM_CHANCE            75
#define MARAUDER_EYES_DODGE_CHANCE           20
#define MARAUDER_EYES_DODGE_INCREMENT         5
#define MARAUDER_ITCH_CHANCE                 15
#define MARAUDER_DANCE_MOVE_COST             10
#define MARAUDER_DANCE_LAG_CHANCE            33
#define MARAUDER_BRIBE_COST                 200
#define MARAUDER_BRIBE_HINDRANCE              4
#define MARAUDER_BRIBE_THUG_MANA_COST        50
#define MARAUDER_BRIBE_ROGUE_MANA_COST       30
#define GATE_CALL_FIRST_ROOM              30200
#define GATE_CALL_LAST_ROOM               30236

extern sh_int  gsn_eyes_of_the_bandit;
extern sh_int  gsn_cunning_strike;
extern sh_int  gsn_thugs_stance;
extern sh_int  gsn_thugs_stance_timer;
extern sh_int  gsn_misinformation;
extern sh_int  gsn_silver_tongue;
extern sh_int  gsn_marauder_bribe;
extern sh_int  gsn_song_of_shadows;
extern sh_int  gsn_soothing_voice;
extern sh_int  gsn_gate_call;
extern sh_int  gsn_guild_cloth;
extern sh_int  gsn_incendiary_device;
extern sh_int  gsn_healing_salve;
extern sh_int  gsn_sonic_whistle;
extern sh_int  gsn_itching_powder;
extern sh_int  gsn_dance_of_the_rogue;

#define RANK_APPRENTICE            0
#define RANK_MARAUDER              1
#define RANK_JOURNEYMAN            2
#define RANK_GUILDMASTER           3
#define RANK_HEADMASTER            5

#define SECT_NONE                  0
#define SECT_ROGUE                 1
#define SECT_THUG                  2
#define SECT_TRICKSTER             3


/*
 * Utility macros.
 */
/*
#define IS_VALID(data)        (((data) != NULL && (data)->valid) || TRUE)
*/
#define IS_VALID(data)        ((data) != NULL && (data)->valid)

#define VALIDATE(data)        ((data)->valid = TRUE)
#define INVALIDATE(data)    ((data)->valid = FALSE)
#define UMIN(a, b)        ((a) < (b) ? (a) : (b))
#define UMAX(a, b)        ((a) > (b) ? (a) : (b))
#define URANGE(a, b, c)        ((b) < (a) ? (a) : ((b) > (c) ? (c) : (b)))
#define SET_ABSOLUTE(a)   if ((a) < 0) {(a) = -(a);}
#define LOWER(c)        ((c) >= 'A' && (c) <= 'Z' ? (c)+'a'-'A' : (c))
#define UPPER(c)        ((c) >= 'a' && (c) <= 'z' ? (c)+'A'-'a' : (c))
#define IS_SET(flag, bit)    ((flag) & (bit))
#define SET_BIT(var, bit)    ((var) |= (bit))
#define REMOVE_BIT(var, bit)    ((var) &= ~(bit))



/*
 * Character macros.
 */
#define IS_NPC(ch)        ((ch)->pcdata == NULL)
#define IS_IMMORTAL(ch)        (get_trust(ch) >= LEVEL_IMMORTAL)
#define IS_HERO(ch)        (get_trust(ch) >= LEVEL_HERO)
#define IS_TRUSTED(ch,level)    (get_trust((ch)) >= (level))
#define IS_AFFECTED(ch, sn)    (IS_SET((ch)->affected_by, (sn)))
#define IS_AFFECTED2(ch, sn)    (IS_SET((ch)->affected_by2, (sn)))
#define IS_ANSI(ch)        (!IS_NPC( ch ) && IS_SET((ch)->comm, COMM_ANSI))

#define GET_AGE(ch)        ((int) (17 + ((ch)->played \
                    + current_time - (ch)->logon )/72000))

#define HAS_TRIGGER(ch,trig)    (IS_SET((ch)->pIndexData->mprog_flags,(trig)))
#define IS_GOOD(ch)             (ch->alignment >= 1)
#define IS_EVIL(ch)             (ch->alignment <= -1)
#define IS_NEUTRAL(ch)                (!IS_GOOD(ch) && !IS_EVIL(ch))

#define IS_AWAKE(ch)        (ch->position > POS_SLEEPING)
#define GET_AC(ch,type)        ((ch)->armor[type]                \
                + ( IS_AWAKE(ch)                \
            ? dex_app[get_curr_stat(ch,STAT_DEX)].defensive : 0 ))
#define GET_HITROLL(ch)    \
        ((ch)->hitroll+str_app[get_curr_stat(ch,STAT_STR)].tohit)
#define GET_DAMROLL(ch) \
        ((ch)->damroll+str_app[get_curr_stat(ch,STAT_STR)].todam)

#define GET_LEVEL(ch)         ((ch)->level + (ch)->drain_level)

#define IS_OUTSIDE(ch) \
(\
   ch->in_room &&\
   !IS_SET((ch)->in_room->room_flags, ROOM_INDOORS) &&\
   (ch)->in_room->sector_type != SECT_INSIDE &&\
   (ch)->in_room->sector_type != SECT_UNDERGROUND\
)

#define WAIT_STATE(ch, npulse)    ((ch)->wait = UMAX((ch)->wait, (npulse)))
#define DAZE_STATE(ch, npulse)                        \
{                                                     \
   if (IS_NPC(ch))                                    \
   {                                                  \
      (ch)->daze = UMAX((ch)->daze, (npulse));        \
   }                                                  \
   else                                               \
   {                                                  \
      (ch)->daze = UMAX((ch)->daze, ((npulse) + 1));  \
   }                                                  \
}
#define QUIT_STATE(ch, npulse)  ((ch)->quittime = UMAX((ch)->quittime, (npulse)))
#define get_carry_weight(ch)    ((ch)->carry_weight + (ch)->silver/10 +   \
                                                      (ch)->gold * 2 / 5)

#define IS_GUARDIAN_ANGEL(mob) (mob && mob->pIndexData && (mob->pIndexData->vnum == MOB_VNUM_GUARDIAN_ANGEL))
#define EVIL_TO(ch, victim)                  \
(                                            \
   IS_EVIL((victim)) ||                      \
   (                                         \
      (                                      \
         (ch)->house == HOUSE_LIGHT ||       \
         IS_GUARDIAN_ANGEL((ch))             \
      ) &&                                   \
      (                                      \
         !IS_NPC((victim)) &&                \
         IS_SET((victim)->act, PLR_EVIL)     \
      )                                      \
   )                                         \
)
#define IS_FLYING(ch)                     \
(                                         \
   IS_AFFECTED((ch), AFF_FLYING) &&       \
   !is_affected((ch), gsn_earthbind) &&   \
   !is_affected((ch), gsn_earthfade)      \
)

/*
 * Object macros.
 */
#define CAN_WEAR(obj, part)    (IS_SET((obj)->wear_flags,  (part)))
#define IS_OBJ_STAT(obj, stat)    (IS_SET((obj)->extra_flags, (stat)))
#define IS_WEAPON_STAT(obj,stat)(IS_SET((obj)->value[4],(stat)))
#define WEIGHT_MULT(obj)    ((obj)->item_type == ITEM_CONTAINER ? \
    (obj)->value[4] : 100)

/*
 * Description macros.
 */
#define PERS(ch, looker) get_pers(ch, looker)
#define NPERS(ch, looker) get_npers(ch, looker)

/*
 * Structure for a social in the socials table.
 */
struct    social_type
{
    char      name[20];
    char *    char_no_arg;
    char *    others_no_arg;
    char *    char_found;
    char *    others_found;
    char *    vict_found;
    char *    char_not_found;
    char *      char_auto;
    char *      others_auto;
};



/*
 * Global constants.
 */
extern   const    struct   str_app_type    str_app        [26];
extern   const    struct   int_app_type    int_app        [26];
extern   const    struct   wis_app_type    wis_app        [26];
extern   const    struct   dex_app_type    dex_app        [26];
extern   const    struct   con_app_type    con_app        [26];

extern   const    struct   subrestrict_type  subrestrict_table    [];
extern   const    struct   class_type        class_table          [MAX_CLASS];
extern   const    struct   weapon_type       weapon_table         [];
extern   const    struct   forest_type       forest_table         [];
extern   const    struct   animal_type       animal_table         [];
extern   const    struct   demon_type        demon_table          [];
extern   const    struct   tinker_type       tinker_table         [];
extern   const    struct   warpaint_type     warpaint_table       [];
extern   const    struct   old_formula_type  old_formula_table    [];
extern   const    struct   formula_type      formula_table        [];
extern   const    struct   material_type     material_table       [];
extern   const             sh_int            material_length;
extern   const    struct   hometown_type     hometown_table       [];
extern   const    struct   item_type         item_table           [];
extern   const    struct   wiznet_type       wiznet_table         [];
extern   const    struct   attack_type       attack_table         [];
extern   const    struct   race_type         race_table           [];
extern   const    struct   pc_race_type      pc_race_table        [MAX_PC_RACE];
extern                     LIST_DATA*        value_1_list;
extern                     LIST_DATA*        book_race_list;
extern                     LIST_DATA*        house_deposits       [MAX_HOUSE];
extern                     LIST_DATA*        clan_deposits        [MAX_CLAN];
extern                     LIST_DATA*        lottery_race_players [MAX_PC_RACE];
extern                     sh_int            lottery_race_count   [MAX_PC_RACE];
extern                     time_t            lottery_race_last_run[MAX_PC_RACE];
extern                     time_t            lottery_race_reset;

extern   const    struct   spec_type         spec_table        [];
extern   const    struct   liq_type          liq_table         [];
extern   const    struct   skill_type        skill_table       [];
extern   SORT_TYPE** sorted_skill_table;
/* Used to keep temporary affects after death */
extern                     AFFECT_DATA**     death_affects;
extern   const             sh_int            MAX_SKILL;
extern   const    struct   group_type        group_table       [MAX_GROUP];
extern            struct   social_type       social_table      [MAX_SOCIALS];
extern   char*    const                      title_table       [MAX_CLASS]    [MAX_LEVEL+1]  [2];
extern   char*    const                      special_table     [MAX_CLASS]    [16];

struct vulgar_type
{
   char* word;
   bool  strong;  /* if false, need whole word.  Else anywhere */
};

extern struct vulgar_type vulgar_table [];

extern struct vulgar_type cyber_table [];

/*
 * Global variables.
 */
extern        HELP_DATA      *    help_first;
extern        SHOP_DATA      *    shop_first;

extern        ROOM_AFFECT_DATA  *    first_room_affect;
extern        ROOM_AFFECT_DATA  *    current_room_affect;
extern        AREA_DATA*            area_first;
extern        CHAR_DATA      *    char_list;
extern        DESCRIPTOR_DATA   *    descriptor_list;
extern        OBJ_DATA      *    object_list;

extern          MPROG_CODE        *     mprog_list;

extern        MESSAGE    *        message_pulse_list;
extern        MESSAGE *        message_tick_list;

extern         bool            is_test;
extern         int                     votes[20];
extern         int                     black_market_data_free;
extern         int                     black_market_data[2000][3];
extern         long                    unhoused_kills[];
extern         long                    covenant_allies[];
extern         long                    house_kills[];
extern         long                    unhoused_pk[];
extern         long                    unique_kills[];
extern         long                    raids_defended[];
extern         long                    raids_nodefender[];
extern         long                    house_pks[];
extern         long                    house_downs[];
extern         time_t                  morale_reset_time;
extern         long                    class_count[];
extern         long                    race_count[];
extern         long                    house_account[];
extern         long                    taxes_collected;
extern         char                    bug_buf        [];
extern         time_t                  current_time;
extern         time_t                  last_banknote_update;
extern         MUD_TIME        current_mud_time;
extern         bool            fLogAll;
extern         bool                    MOBtrigger;
extern         bool                    reboot_ok;
/*extern          bool                    check_ip;*/
extern         BAN_DATA *              ban_list;
extern         FILE *            fpReserve;
extern         KILL_DATA        kill_table    [];
extern         char            log_buf        [2 * MAX_INPUT_LENGTH];
extern         WEATHER_DATA        weather_info;
extern         long              last_note_id;
extern         int               top_room;
/*
 * OS-dependent declarations.
 * These are all very standard library functions,
 *   but some systems have incomplete or non-ansi header files.
 */
#if    defined(_AIX)
char *    crypt        args( ( const char *key, const char *salt ) );
#endif

#if    defined(apollo)
int    atoi        args( ( const char *string ) );
void *    calloc        args( ( unsigned nelem, size_t size ) );
char *    crypt        args( ( const char *key, const char *salt ) );
#endif

#if    defined(hpux)
char *    crypt        args( ( const char *key, const char *salt ) );
#endif

#if    defined(linux)
char *    crypt        args( ( const char *key, const char *salt ) );
void *  calloc          args( ( unsigned nelem, size_t size ) );
#endif

#if    defined(macintosh)
#define NOCRYPT
#if    defined(unix)
void *  calloc          args( ( unsigned nelem, size_t size ) );
#undef    unix
#endif
#endif

#if    defined(MIPS_OS)
char *    crypt        args( ( const char *key, const char *salt ) );
#endif

#if    defined(MSDOS)
#define NOCRYPT
#if    defined(unix)
#undef    unix
#endif
#endif

#if    defined(NeXT)
char *    crypt        args( ( const char *key, const char *salt ) );
#endif

#if    defined(sequent)
char *    crypt        args( ( const char *key, const char *salt ) );
int    fclose        args( ( FILE *stream ) );
int    fprintf        args( ( FILE *stream, const char *format, ... ) );
int    fread        args( ( void *ptr, int size, int n, FILE *stream ) );
int    fseek        args( ( FILE *stream, long offset, int ptrname ) );
void    perror        args( ( const char *s ) );
int    ungetc        args( ( int c, FILE *stream ) );
#endif

#if    defined(sun)
char *    crypt        args( ( const char *key, const char *salt ) );
int    fclose        args( ( FILE *stream ) );
int    fprintf        args( ( FILE *stream, const char *format, ... ) );
#if    defined(SYSV)
siz_t    fread        args( ( void *ptr, size_t size, size_t n,
                FILE *stream) );
#else
int    fread        args( ( void *ptr, int size, int n, FILE *stream ) );
#endif
int    fseek        args( ( FILE *stream, long offset, int ptrname ) );
void    perror        args( ( const char *s ) );
int    ungetc        args( ( int c, FILE *stream ) );
#endif

#if    defined(ultrix)
char *    crypt        args( ( const char *key, const char *salt ) );
#endif



/*
 * The crypt(3) function is not available on some operating systems.
 * In particular, the U.S. Government prohibits its export from the
 *   United States to foreign countries.
 * Turn on NOCRYPT to keep passwords in plain text.
 */
#if    defined(NOCRYPT)
#define crypt(s1, s2)    (s1)
#endif



/*
 * Data files used by the server.
 *
 * AREA_LIST contains a list of areas to boot.
 * All files are read in completely at bootup.
 * Most output files (bug, idea, typo, shutdown) are append-only.
 *
 * The NULL_FILE is held open so that we have a stream handle in reserve,
 *   so players can go ahead and telnet to all the other descriptors.
 * Then we close it whenever we need to open a file (e.g. a save file).
 */
#if defined(macintosh)
#define PLAYER_DIR    ""            /* Player files    */
#define STORAGE_DIR     ""
#define TEMP_FILE    "romtmp"
#define NULL_FILE    "proto.are"        /* To reserve one stream */
#endif

#if defined(MSDOS)
#define PLAYER_DIR    ""            /* Player files */
#define STORAGE_DIR     ""
#define STORAGE_LIST    "player.lst"  /* Stored Player list for rosters */
#define PLAYER_LIST     "player.lst"            /* Player list for limits */
#define TEMP_FILE    "romtmp"
#define NULL_FILE    "nul"            /* To reserve one stream */
#define AREA_DIR        ""
#endif

#if defined(unix)
#define PLAYER_DIR      "../player/"            /* Player files */
#define BOOK_DIR        "./books/"               /* Dynamic Book files */
#define STORAGE_DIR     "../storage/"           /* stored pfiles */
#define STORAGE_LIST    "../storage/Player.lst"  /* Stored Player list for rosters */
#define PLAYER_LIST     "../player/Player.lst"  /* Player list for limits */
#define GOD_DIR         "../gods/"          /* list of gods */
#define TEMP_FILE       "../player/romtmp"
#define NULL_FILE       "/dev/null"        /* To reserve one stream */
#define AREA_DIR        "../area/"  /* Default directory of areas */
#endif

#define AREA_DIR_FILE   "area.dir"  /* FOR TEST, directory of areas to use */
#define AREA_LIST       "area.lst"  /* List of areas */
#define AREA_LIST_TEST  "area.tst"  /* List of areas for test */
#define BUG_FILE        "bugs.txt" /* For 'bug' and bug()*/
#define TYPO_FILE       "typos.txt" /* For 'typo'*/
#define NOTE_FILE       "notes.not"/* For 'notes'*/
#define IDEA_FILE       "ideas.not"
#define PENALTY_FILE    "penal.not"
#define NEWS_FILE       "news.not"
#define CHANGES_FILE    "chang.not"
#define HELP_FILE       "help.not"
#define HOARDER_FILE    "hoarder.not"
#define SHUTDOWN_FILE   "shutdown.txt"/* For 'shutdown'*/
#define BAN_FILE        "ban.txt"
#define NAMEBAN_FILE    "nameban.txt"
#define MUSIC_FILE      "music.txt"
#define CRIM_FILE       "criminal.not"
#define BANK_FILE       "bank.not"
#define DEATH_FILE      "deaths.not"
#define CLAN_FILE       "clans.not"
#define BOOK_FILE_PRE   "book_"
#define BOOK_FILE_SUF   ".dat"
#define SQL_FILE        "account_creator.sql"

/*
 * Our function prototypes.
 * One big lump ... this is every function in Merc.
 */
#define CD    CHAR_DATA
#define MID    MOB_INDEX_DATA
#define OD    OBJ_DATA
#define OID    OBJ_INDEX_DATA
#define RID    ROOM_INDEX_DATA
#define SF    SPEC_FUN
#define AD    AFFECT_DATA
#define MPC     MPROG_CODE

/* act_comm.c */
void do_talk args( (CHAR_DATA* ch, char* argument) );
bool is_mental args( (CHAR_DATA* ch) );
void      check_sex    args( ( CHAR_DATA *ch) );
void    add_follower    args( ( CHAR_DATA *ch, CHAR_DATA *master ) );
void    stop_follower    args( ( CHAR_DATA *ch ) );
void     nuke_pets    args( ( CHAR_DATA *ch ) );
void    die_follower    args( ( CHAR_DATA *ch ) );
bool    is_same_group    args( ( CHAR_DATA *ach, CHAR_DATA *bch ) );
void    announce_logout args((CHAR_DATA *ch));
void    get_age_mod    args((CHAR_DATA *ch));    /* meant to be comm.c but accident */
void  un_pulse args((OBJ_DATA *obj));


/* act_enter.c */
RID  *get_random_room   args( (CHAR_DATA *ch, sh_int scope ) );
bool is_shrine          args( (ROOM_INDEX_DATA *room));
CHAR_DATA* get_random_mob   args( (CHAR_DATA *ch, sh_int scope ) );

/* act_info.c */
extern int max_on;
extern int max_on_ever;
void    set_title    args( ( CHAR_DATA *ch, char *title ) );
bool    check_peace     args( ( CHAR_DATA *ch ) );
char *  get_brand_title args( ( CHAR_DATA *ch ) );
char*  get_house_title  args( ( CHAR_DATA *ch ) );
char*  get_house_title2 args( ( CHAR_DATA *ch ) );
bool is_pulsing args((OBJ_DATA *obj));
bool house_member_on(int house);
void do_release_crusader args( (CHAR_DATA* ch, char* argument) );
void do_call_crusader args( (CHAR_DATA* ch, char* argument) );
void get_mud_time_string args( (MUD_TIME* mud_time_value, char* mud_time_string) );
char* get_time_string      args( (CHAR_DATA* ch, CHAR_DATA* vch, AFFECT_DATA* paf, ROOM_AFFECT_DATA* praf, char* buf, int type, bool ticks) );
void timeval_to_mudtime args( (TIMEVAL* input, MUD_TIME* output) );
extern char *    const    day_name    [];
extern char *    const    month_name  [];
void do_observe   args( (CHAR_DATA* ch, char* argument, int type) );

/* act_move.c */
void    move_char    args( ( CHAR_DATA *ch, int door, int flags ) );
void    un_hide         args( ( CHAR_DATA *ch, char *argument ) );
void    un_invis        args( ( CHAR_DATA *ch, char *argument ) );
void    un_earthfade       args( ( CHAR_DATA *ch, char *argument ) );
void    un_sneak        args( ( CHAR_DATA *ch, char *argument ) );
void     un_camouflage    args( ( CHAR_DATA *ch, char *argument) );
void    un_blackjack    args( ( CHAR_DATA *ch, char *argument) );
void    un_strangle    args( ( CHAR_DATA *ch, char *argument) );
void    un_shroud    args( (CHAR_DATA *ch, char *argument) );
void    un_forest_blend args( ( CHAR_DATA *ch));
void     un_gaseous    args( ( CHAR_DATA *ch) );
void    track_char      args( ( CHAR_DATA *ch) );
void    new_track       args( ( CHAR_DATA *ch, int direction) );
int     get_trackdir    args( ( CHAR_DATA *ch, int ID ) );
void    delete_track    args( ( CHAR_DATA *ch) );
CHAR_DATA*  id2name        args( (long id, bool allow_npc) );
CHAR_DATA*  id2name_room   args( (long id, ROOM_INDEX_DATA* room, bool allow_npc) );
void    trap_trigger_check   args((CHAR_DATA *ch));
void  passive_visible   args( (CHAR_DATA* ch) );

/* act_obj.c */
bool cant_carry        args( (CHAR_DATA *ch, OBJ_DATA *obj) );
bool is_restricted    args( (CHAR_DATA *ch, OBJ_DATA *obj) );
bool can_loot        args( (CHAR_DATA *ch, OBJ_DATA *obj) );
bool get_obj         args( ( CHAR_DATA *ch, OBJ_DATA *obj,
                            OBJ_DATA *container,
                            long objCount ) );
bool  wear_obj          args( (CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace) );
void  house_shudder    args( (int house) );
bool  house_down    args( (CHAR_DATA *ch,int house) );
bool  house_down_1  args( (CHAR_DATA* ch, int house, bool check) );
bool  hands_full    args( (CHAR_DATA *ch ) );
void wear_obj_girdle    args( (CHAR_DATA *ch, OBJ_DATA *obj) );
void wear_obj_bracers    args( (CHAR_DATA *ch, OBJ_DATA *obj) );
void wear_obj_elven_armguards    args( (CHAR_DATA *ch, OBJ_DATA *obj) );
void remove_obj_girdle    args( (CHAR_DATA *ch, OBJ_DATA *obj) );
void remove_obj_bracers    args( (CHAR_DATA *ch, OBJ_DATA *obj) );
void remove_obj_elven_armguards    args( (CHAR_DATA *ch, OBJ_DATA *obj) );
void reslot_weapon    args( (CHAR_DATA *ch) );
void report_weapon_skill    args( (CHAR_DATA *ch,OBJ_DATA *obj) );
bool can_wield_tertiary    args((CHAR_DATA *ch,OBJ_DATA *obj, bool fReplace));
void    wear_obj_channeling args(( CHAR_DATA *ch, OBJ_DATA *obj));
void    remove_obj_channeling args(( CHAR_DATA *ch, OBJ_DATA *obj));
void    wear_obj_dragonmage_feet args((CHAR_DATA *ch,OBJ_DATA *obj));
void    wear_obj_blue_hide      args((CHAR_DATA *ch,OBJ_DATA *obj));
void    wear_obj_green_hide     args((CHAR_DATA *ch,OBJ_DATA *obj));
void remove_obj_dragonmage_feet args((CHAR_DATA *ch,OBJ_DATA *obj));
void remove_obj_blue_hide       args((CHAR_DATA *ch,OBJ_DATA *obj));
void remove_obj_green_hide      args((CHAR_DATA *ch,OBJ_DATA *obj));
void wear_obj_19002 args( (CHAR_DATA *ch, OBJ_DATA *obj) );
void remove_obj_19002 args( ( CHAR_DATA *ch, OBJ_DATA *obj) );
void remove_obj_tiara   args( ( CHAR_DATA *ch, OBJ_DATA *obj) );
void remove_obj_wizardry args( ( CHAR_DATA *ch, OBJ_DATA *obj) );
void remove_obj_sceptre args((CHAR_DATA *ch,OBJ_DATA *obj));
void remove_obj_whitehelm args((CHAR_DATA *ch, OBJ_DATA *obj));
void remove_obj_fallen_wings args((CHAR_DATA *ch,OBJ_DATA *obj));
void remove_obj_black_hide args((CHAR_DATA *ch,OBJ_DATA *obj));
void remove_obj_shadow_hide args((CHAR_DATA *ch,OBJ_DATA *obj));
void remove_obj_symbol_magic args((CHAR_DATA *ch,OBJ_DATA *obj));
void remove_obj_red_dragonstar args((CHAR_DATA *ch,OBJ_DATA *obj));
void remove_obj_black_dragonstar args((CHAR_DATA *ch,OBJ_DATA *obj));
void remove_obj_cloak_underworld args((CHAR_DATA *ch,OBJ_DATA *obj));
void remove_obj_misty_cloak args((CHAR_DATA *ch,OBJ_DATA *obj));
void remove_obj_troll_skin args((CHAR_DATA *ch,OBJ_DATA *obj));
void remove_obj_ring_protection args((CHAR_DATA *ch,OBJ_DATA *obj));
void remove_obj_robe_avenger args((CHAR_DATA *ch,OBJ_DATA *obj));
void do_marriage args((CHAR_DATA* ch, char* arg));
void     obj_to_keeper        args( (OBJ_DATA* obj, CHAR_DATA* ch) );
bool is_branded_by_lestregus        args( (CHAR_DATA *ch) );
bool obj_can_change_align_restrictions     args( (OBJ_DATA *obj) );

/* act_wiz.c */
void wiznet        args( (char *string, CHAR_DATA *ch, OBJ_DATA *obj,
                   long flag, long flag_skip, int min_level ) );
/* alias.c */
void     substitute_alias args( (DESCRIPTOR_DATA *d, char *input) );

/* ban.c */
bool    check_ban    args( ( char *site, int type) );
void    ban_site        args( (CHAR_DATA *ch, char *argument, bool fPerm) );

/* nameban.c */
bool    check_nameban   args( ( char *site, int type) );

/* comm.c */
void    show_string    args( ( struct descriptor_data *d, char *input) );
void    close_socket    args( ( DESCRIPTOR_DATA *dclose ) );
void    write_to_buffer    args( ( DESCRIPTOR_DATA *d, const char *txt,
                int length ) );
void    make_lich    args( ( CHAR_DATA *ch, int lmod) );
void    send_to_char    args( ( const char *txt, CHAR_DATA *ch ) );
void    page_to_char    args( ( const char *txt, CHAR_DATA *ch ) );

/*
void    act        args( ( const char *format, CHAR_DATA *ch,
                const void *arg1, const void *arg2, int type ) );

void    act_color        args( ( const char *format, CHAR_DATA *ch,
                const void *arg1, const void *arg2, int type ) );
void    act_new        args( ( const char *format, CHAR_DATA *ch,
                const void *arg1, const void *arg2, int type,
                int min_pos) );
void    act_nnew        args( ( const char *format, CHAR_DATA *ch,
                            const void *arg1, const void *arg2, int type,
                            int min_pos) );
*/
/* Turned into defines to save on function calls */
#define act_new(format, ch, arg1, arg2, type, min_pos)   \
act_new_color(format, ch, arg1, arg2, type, min_pos, FALSE)

#define act(format, ch, arg1, arg2, type)                \
act_new(format, ch, arg1, arg2, type, POS_RESTING)

#define act_color(format, ch, arg1, arg2, type)          \
act_new_color(format, ch, arg1, arg2, type, POS_RESTING, TRUE)

#define act_nnew(format, ch, arg1, arg2, type, min_pos)  \
act_new_color(format, ch, arg1, arg2, type, min_pos, FALSE)

void    act_new_color        args( ( const char *format, CHAR_DATA *ch,
                const void *arg1, const void *arg2, int type,
                int min_pos, bool color) );
char *  get_descr_form    args( (CHAR_DATA *ch,CHAR_DATA *looker,bool
                get_long) );



/* db.c */
char *    print_flags    args( ( int flag ));
void    boot_db        args( ( void ) );
void    area_update    args( ( void ) );
MPC *   get_mprog_index args( ( int vnum ) );
CD *    create_mobile    args( ( MOB_INDEX_DATA *pMobIndex ) );
void    clone_mobile    args( ( CHAR_DATA *parent, CHAR_DATA *clone) );
void    clone_character args( (CHAR_DATA* parent, CHAR_DATA* clone, bool pc_clone) );
OD *    create_object    args( ( OBJ_INDEX_DATA *pObjIndex, int level ) );
void    clone_object    args( ( OBJ_DATA *parent, OBJ_DATA *clone ) );
void    clear_char    args( ( CHAR_DATA *ch ) );
char *    get_extra_descr    args( ( const char *name, EXTRA_DESCR_DATA *ed ) );
MID *    get_mob_index    args( ( int vnum ) );
OID *    get_obj_index    args( ( int vnum ) );
RID *    get_room_index    args( ( int vnum ) );
void     add_id_hash       args( (CHAR_DATA* ch) );
void     remove_id_hash    args( (CHAR_DATA* ch) );
char    fread_letter    args( ( FILE *fp ) );
int    fread_number    args( ( FILE *fp ) );
long     fread_flag    args( ( FILE *fp ) );
long    read_flag       args( ( char *flags ) );
char *    fread_string    args( ( FILE *fp ) );
char *  fread_string_eol args(( FILE *fp ) );
void    fread_to_eol    args( ( FILE *fp ) );
char *    fread_word    args( ( FILE *fp ) );
long    flag_convert    args( ( char letter) );
void *    alloc_mem    args( ( int sMem ) );
void *    alloc_perm    args( ( int sMem ) );
void    free_mem    args( ( void *pMem, int sMem ) );
char *    str_dup        args( ( const char *str ) );
void    free_string    args( ( char *pstr ) );
int    number_fuzzy    args( ( int number ) );
int    number_range    args( ( int from, int to ) );
int    number_percent    args( ( void ) );
int    number_door    args( ( void ) );
int    number_bits    args( ( int width ) );
long     number_mm       args( ( void ) );
int    dice        args( ( int number, int size ) );
int    interpolate    args( ( int level, int value_00, int value_32 ) );
void    smash_tilde    args( ( char *str ) );
char *  upstr           args( ( char *thestr ) );
char *  upstr_long      args( ( char *thestr ) );
int      str_cmp        args( ( const char *astr, const char *bstr ) );
bool    str_prefix    args( ( const char *astr, const char *bstr ) );
bool    str_infix    args( ( const char *astr, const char *bstr ) );
bool    str_suffix    args( ( const char *astr, const char *bstr ) );
char *    capitalize    args( ( const char *str ) );
char*    full_capitalize   args( (const char* str) );
void    append_file    args( ( CHAR_DATA *ch, char *file, char *str ) );
void    bug        args( ( const char *str, int param ) );
void    bug_trust  args( (const char *str, int param, int trust) );
void    log_string    args( ( const char *str ) );
void    tail_chain    args( ( void ) );
void    update_time args( (void ) );
void    update_roster      args( (CHAR_DATA* ch, bool storage) );
void    remove_from_rosters args( (CHAR_DATA* ch) );
void    sort_all_rosters   args( (void) );
char*    global_bank_message  args( (DEPOSIT_TYPE** array, int size, char* account_type, unsigned max_name, bool kicked) );
void    dynamically_load_area args( (CHAR_DATA* ch, char* area_name) );

/* effect.c */
void    acid_effect    args( (void *vo, int level, int dam, int target) );
void    cold_effect    args( (void *vo, int level, int dam, int target) );
void    fire_effect    args( (void *vo, int level, int dam, int target) );
void    poison_effect    args( (void *vo, int level, int dam, int target) );
void    shock_effect    args( (void *vo, int level, int dam, int target) );
void    drowning_effect args( (void *vo, int level, int dam, int target) );
void    resonating_effect args( (void *vo, int level, int am, int target) );

/* fight.c */
int  award_standing     args( (CHAR_DATA* victim) );
bool can_instakill      args( (CHAR_DATA* ch, CHAR_DATA* victim) );
void raw_kill           args( (CHAR_DATA* ch, CHAR_DATA* victim) );
bool spellbaned         args( (CHAR_DATA* chaster, CHAR_DATA* victim, int sn) );
bool fire_volley        args( (CHAR_DATA* ch, CHAR_DATA* victim, bool check_only) );

/* is_safe flags */
#define IS_SAFE_AREA_ATTACK      (A)
#define IS_SAFE_SILENT           (B)
#define IS_SAFE_IGNORE_ROOM      (C)
#define IS_SAFE_IGNORE_AFFECTS   (D)
#define IS_SAFE_RECURSIVE        (E)

bool     is_in_pk    args( (CHAR_DATA* ch, CHAR_DATA* victim) );
bool     is_safe        args( (CHAR_DATA* ch, CHAR_DATA* victim, int flags) );
bool     is_safe_room   args( (CHAR_DATA* ch, ROOM_AFFECT_DATA* raf, int flags) );

CHAR_DATA * get_damager_room(CHAR_DATA *victim, ROOM_AFFECT_DATA * af);
CHAR_DATA* get_damager  args((CHAR_DATA *victim,AFFECT_DATA *af));
void    violence_update    args( ( void ) );
void    multi_hit    args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
bool    damage        args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam,
                                int dt, int class, int show ) );
bool check_two_hand_wield args ((CHAR_DATA *ch));
bool check_evaluate args((CHAR_DATA *ch,CHAR_DATA *victim));
void    update_pos    args( ( CHAR_DATA *victim ) );
void    stop_fighting    args( ( CHAR_DATA *ch, bool fBoth ) );
void    check_killer    args( ( CHAR_DATA *ch, CHAR_DATA *victim) );
void    age_death    args((CHAR_DATA *ch));
char * check_evaluation_fight args((CHAR_DATA *ch,CHAR_DATA *victim));

/* md5.c */
char*    md5_hash    args( (char* str) );

/* handler.c */
char* get_npers args( (CHAR_DATA* ch, CHAR_DATA* looker) );
char* get_pers args( (CHAR_DATA* ch, CHAR_DATA* looker) );
char* stristr args( (const char *string1, const char *string2) );
bool is_vulgar args( (char* input, VULGAR_TYPE* table) );
ROOM_AFFECT_DATA * affect_find_room(ROOM_INDEX_DATA *room, int sn);
void affect_update args ( (AFFECT_DATA *paf, int version) );
AD      *affect_find args( (AFFECT_DATA *paf, int sn));
void    affect_check    args( (CHAR_DATA *ch, int where, int vector) );
int    count_users    args( (OBJ_DATA *obj) );
void     deduct_cost    args( (CHAR_DATA *ch, int cost) );
void    affect_enchant    args( (OBJ_DATA *obj) );
char *    color_value_string args( ( int color, bool bold, bool flash ) );
int     strlen_color    args( ( char *argument ) );
int     check_immune    args( (CHAR_DATA *ch, int dam_type) );
int    liq_lookup    args( ( const char *name) );
int    search_type_lookup   args( (const char *name, long scope) );
int    weapon_lookup    args( ( const char *name) );
char  * get_scale_color args( ( CHAR_DATA *ch));
char    *get_name    args( ( CHAR_DATA *ch, CHAR_DATA *victim));
void    bounty_note    args(( CHAR_DATA *ch));
char    *get_longname   args( ( CHAR_DATA *ch, CHAR_DATA *victim));
char    *get_special_name args( ( int class, int spec ) );
int    find_subclass    args( ( CHAR_DATA *ch, char *argument ) );
int     get_special_num args( ( int class, char * spec_name) );
int    weapon_type    args( ( const char *name) );
char     *weapon_name    args( ( int weapon_Type) );
int    item_lookup    args( ( const char *name) );
char    *item_name      args( ( int item_type) );
int    attack_lookup    args( ( const char *name) );
int    race_lookup    args( ( const char *name) );
long    wiznet_lookup    args( ( const char *name) );
int    class_lookup    args( ( const char *name) );
bool    is_house         args( (CHAR_DATA *ch) );
bool    is_same_house    args( (CHAR_DATA *ch, CHAR_DATA *victim));
bool    is_old_mob    args ( (CHAR_DATA *ch) );
int    get_skill    args( ( CHAR_DATA *ch, int sn ) );
void    save_globals    args( ( void ) );
void    save_wizireport args( ( void ) );
void    log_convo    args( (CHAR_DATA *ch, CHAR_DATA *ch_to,int type, char * argument) );
int    get_weapon_sn    args( ( CHAR_DATA *ch ) );
int    get_weapon_skill args(( CHAR_DATA *ch, int sn ) );
int      get_age        args( (CHAR_DATA* ch) );
int      get_real_age   args( (CHAR_DATA* ch) );
void    reset_char    args( ( CHAR_DATA *ch )  );
bool    collect_tax    args( ( CHAR_DATA *ch, int cost) );
int    get_trust    args( ( CHAR_DATA *ch ) );
int    get_curr_stat    args( ( CHAR_DATA *ch, int stat ) );
int     get_max_train    args( ( CHAR_DATA *ch, int stat ) );
int    can_carry_n    args( ( CHAR_DATA *ch ) );
int    can_carry_w    args( ( CHAR_DATA *ch ) );
bool    is_name        args( ( char *str, char *namelist ) );
bool    is_exact_name    args( ( char *str, char *namelist ) );
void    affect_to_char    args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void    affect_to_obj    args( ( OBJ_DATA *obj, AFFECT_DATA *paf ) );
void    affect_to_char_1  args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void    affect_to_obj_1  args( ( OBJ_DATA *obj, AFFECT_DATA *paf ) );
void     affect_to_char_version args( (CHAR_DATA *ch, AFFECT_DATA *paf, int version) );
void     affect_to_obj_version  args( (OBJ_DATA *obj, AFFECT_DATA *paf, int version) );
void    affect_modify   args( ( CHAR_DATA *ch, AFFECT_DATA *paf, bool fAdd) );
void    affect_modify_1   args( ( CHAR_DATA *ch, AFFECT_DATA *paf, bool fAdd) );
bool    affect_set_bit2  args( (AFFECT_DATA *paf, sh_int sn, int bit2) );
void    affect_remove    args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void    affect_remove_obj args( (OBJ_DATA *obj, AFFECT_DATA *paf ) );
void    affect_to_room  args( ( ROOM_INDEX_DATA *room,ROOM_AFFECT_DATA *paf) );
void    affect_remove_room args( (ROOM_INDEX_DATA *room,ROOM_AFFECT_DATA *paf));
void    affect_strip_room args((ROOM_INDEX_DATA *room,int sn));
void    affect_strip_room_single args((ROOM_INDEX_DATA *room,int sn));
void    affect_strip    args( ( CHAR_DATA *ch, int sn ) );
bool    is_affected_obj  args( (OBJ_DATA *obj, int sn ) );
bool    is_affected_room    args((ROOM_INDEX_DATA *room,int sn));
bool    room_sector_modified    args((ROOM_INDEX_DATA *room,CHAR_DATA *ch));
bool    is_affected    args( ( CHAR_DATA *ch, int sn ) );
void    affect_join    args( ( CHAR_DATA *ch, AFFECT_DATA *paf, sh_int limit ) );
void    char_from_room    args( ( CHAR_DATA *ch ) );
bool     house_closed      args( (int house, int version) );
void     stash_valuables      args( (CHAR_DATA* ch) );
void     unstash_valuables    args( (CHAR_DATA* ch) );
#define char_to_room(ch, pRoomIndex)         \
char_to_room_1(ch, pRoomIndex, TO_ROOM_NORMAL)

#define char_to_room_look(ch, pRoomIndex)    \
char_to_room_1(ch, pRoomIndex, TO_ROOM_NORMAL | TO_ROOM_LOOK)

/* Changed to macros */
/*
void    char_to_room    args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex ) );
void    char_to_room_look args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex ) );
*/
void    char_to_room_1  args( (CHAR_DATA* ch, ROOM_INDEX_DATA* pRoomIndex, int flags) );
void    obj_to_char    args( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
void    obj_from_char    args( ( OBJ_DATA *obj ) );
int    apply_ac    args( ( OBJ_DATA *obj, int iWear, int type ) );
OD *    get_eq_char    args( ( CHAR_DATA *ch, int iWear ) );
void    equip_char    args( ( CHAR_DATA *ch, OBJ_DATA *obj, int iWear ) );
void    unequip_char    args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
int    count_obj_list    args( ( OBJ_INDEX_DATA *obj, OBJ_DATA *list ) );
void    obj_from_room    args( ( OBJ_DATA *obj ) );
void    obj_to_room    args( ( OBJ_DATA *obj, ROOM_INDEX_DATA *pRoomIndex ) );
void    obj_to_obj    args( ( OBJ_DATA *obj, OBJ_DATA *obj_to ) );
void    obj_from_obj    args( ( OBJ_DATA *obj ) );
void    extract_obj    args( ( OBJ_DATA *obj, bool DoLimits ) );
void    extract_char    args( ( CHAR_DATA *ch, bool fPull ) );
CD *    get_char_room    args( ( CHAR_DATA *ch, char *argument ) );
CD *    get_char_world    args( ( CHAR_DATA *ch, char *argument ) );
CHAR_DATA* get_pc_world    args( (CHAR_DATA* ch, char* argument, bool ignore_blind) );
CD *    get_char_area   args( ( CHAR_DATA *ch, char *argument ) );
OD *    get_obj_type    args( ( OBJ_INDEX_DATA *pObjIndexData ) );
OD *    get_obj_list    args( ( CHAR_DATA *ch, char *argument,
                OBJ_DATA *list ) );
OD *    get_obj_carry    args( ( CHAR_DATA *ch, char *argument,
                CHAR_DATA *viewer ) );
OD*     get_obj_char_vnum args( (CHAR_DATA* ch, sh_int argument) );
OD *    get_obj_wear    args( ( CHAR_DATA *ch, char *argument ) );
OBJ_DATA* get_obj_inv_worn_room  args( (CHAR_DATA* ch, char* argument) );
OBJ_DATA* get_obj_inv_worn_room_world  args( ( CHAR_DATA *ch, char *argument ) );
OD *    get_obj_here    args( ( CHAR_DATA *ch, char *argument ) );
OD *    get_obj_world    args( ( CHAR_DATA *ch, char *argument ) );
OD *    create_money    args( ( int gold, int silver ) );
int    get_obj_number    args( ( OBJ_DATA *obj ) );
int    get_obj_weight    args( ( OBJ_DATA *obj ) );
int    get_true_weight    args( ( OBJ_DATA *obj ) );
bool    room_is_dark    args( ( ROOM_INDEX_DATA *pRoomIndex ) );
bool    is_room_owner    args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *room) );
bool    room_is_private    args( ( ROOM_INDEX_DATA *pRoomIndex ) );
bool    can_see        args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    can_see_rogue  args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    can_area_hit_rogue    args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    can_see_obj    args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
bool    can_see_room    args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex) );
bool    can_drop_obj    args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
bool    can_remove_obj  args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
int     hoard_score     args( ( CHAR_DATA *ch) );
void    getLimb         args( ( CHAR_DATA * ch, char * limb ) );
bool    check_bloodlust args( ( CHAR_DATA * ch ) );
char * get_herb_spell_name args( (AFFECT_DATA* paf, bool name, sh_int sn_in) );
void    switch_desc     args( (CHAR_DATA* ch, sh_int new_desc) );
int    find_spell    args( ( CHAR_DATA* ch, const char* name) );
int    find_skill_spell    args( ( CHAR_DATA* ch, const char* name) );
bool oblivion_blink     args( (CHAR_DATA* ch, CHAR_DATA* victim) );
int gsn_corrupt         args( (const char* skill, sh_int* gsn) );
bool contains_pulsing   args( (OBJ_DATA* obj) );
bool check_room         args( (CHAR_DATA* ch, bool purge, char* reason) );
void check_impure       args( (CHAR_DATA* ch, char* name, sh_int type) );
void strip_limits        args( (OBJ_DATA* obj) );
bool check_shadowstrike args( (CHAR_DATA* ch, bool shadow, bool skull) );
void fix_fighting args( (CHAR_DATA* ch) );
void update_visible args( (CHAR_DATA* ch) );
void remove_name args( (char* string, char* name) );
int pencolor_lookup     args( (const char* name) );
int inkwell_lookup      args( (const char* name) );
int spell_match         args( (const char* name) );
int spell_match_num     args( (int skill) );
bool wizi_to            args( (CHAR_DATA* ch, CHAR_DATA* looker) );
void check_perception   args( (CHAR_DATA* ch, CHAR_DATA* victim, char* reason) );
int   guard_type        args( (CHAR_DATA* ch) );
int race_adjust         args( (CHAR_DATA *ch) );
int    see_state    args( (CHAR_DATA *ch, CHAR_DATA *victim) );
void     light_check    args( (ROOM_INDEX_DATA* room, CHAR_DATA* ch, OBJ_DATA* obj, const char* func) );
char *    affect_loc_name    args( ( int location ) );
char*    affect_bit_name   args( (int vector, int vector2) );
char * restrict_bit_name args( (int restrict_flags) );
char *    extra_bit_name    args( (int vector, int vector2) );
char *     wear_bit_name    args( ( int wear_flags ) );
char *  act_bit_name    args( ( int act_flags, int act2_flags ) );
char *    off_bit_name    args( ( int off_flags ) );
char *  imm_bit_name    args( ( int imm_flags ) );
char *     form_bit_name    args( ( int form_flags ) );
char *    part_bit_name    args( ( int part_flags ) );
char *    weapon_bit_name    args( ( int weapon_flags ) );
char *  comm_bit_name    args( ( int vector, int vector2 ) );
char *    cont_bit_name    args( ( int cont_flags) );
char*    room_bit_name     args( (int vector, int vector2) );
char *  movement_bit_name args( ( int move_flags ) );
bool     frog_dodge     args( (CHAR_DATA* victim, CHAR_DATA* ch, int dt) );
int      check_simulacrum        args( (CHAR_DATA* ch) );
void     simulacrum_on           args( (CHAR_DATA* ch) );
void     simulacrum_off          args( (CHAR_DATA* ch) );
void     strip_simulacrum        args( (CHAR_DATA* ch, CHAR_DATA* victim, bool force) );
size_t   strlcat                 args( (char* dst, const char* src, size_t siz) );
void     irv_update              args( (CHAR_DATA* ch, bool tick, bool recalculate) );
bool     can_exit                args( (CHAR_DATA* ch, EXIT_DATA* pexit) );
void     store_death_affect      args( (AFFECT_DATA* paf_old) );
void     restore_death_affects   args( (CHAR_DATA* ch) );
CHAR_DATA*  get_char_room_holy_light   args( (CHAR_DATA* ch, char* argument) );
CHAR_DATA*  get_char_world_holy_light  args( (CHAR_DATA* ch, char* argument) );
const char*    wear_slot_name    args( (int slot) );
void     stop_feign              args( (CHAR_DATA* ch, bool chance) );
void shuffle args( (int size, const int* listToShuffle, int* shuffledList) );
void     update_alignment        args( (CHAR_DATA* ch) );

/* material.c stuff */
bool formula_match(int i1, int i2, int i3, int i4, int i5, int f1, int f2, int f3, int f4, int f5);
int     material_lookup args( ( OBJ_DATA *obj) );
int     material_lookup2 args( ( char *material) );
bool    is_metal        args( ( OBJ_DATA *obj) );
bool    is_intangible   args( ( OBJ_DATA *obj) );
bool    is_liquid       args( ( OBJ_DATA *obj) );
bool    is_wood         args( ( OBJ_DATA *obj) );
bool    is_flammable     args( ( OBJ_DATA *obj) );
bool    is_stone        args( ( OBJ_DATA *obj) );
bool    is_gem          args( ( OBJ_DATA *obj) );

/*  Functions for messaging system - Mael */

bool    message_to_obj        args( ( OBJ_DATA *obj, MESSAGE *message ) );
bool    message_to_char        args( ( CHAR_DATA *ch, MESSAGE *message ) );
void    message_remove        args( ( MESSAGE *message ) );
void    message_strip_obj    args( ( OBJ_DATA *obj, long int message_type ) );
bool    has_message_obj        args( ( OBJ_DATA *obj, long int message_type ) );
void    message_join_obj    args( ( OBJ_DATA *obj, MESSAGE *message, sh_int limit ) );
MESSAGE* get_message_obj    args( ( OBJ_DATA *obj, long int message_type ) );
MESSAGE* get_message_char    args( ( CHAR_DATA *ch, long int message_type ) );

/* interp.c */
void    do_gsndump   args( (CHAR_DATA* ch, char* arg) );
void    interpret    args( ( CHAR_DATA *ch, char *argument ) );
bool    is_number    args( ( char *arg ) );
int    number_argument    args( ( char *argument, char *arg ) );
int    mult_argument    args( ( char *argument, char *arg) );
char*  one_argument          args( (char *argument, char *arg_first) );
char*  one_argument_space    args( (char* argument, char* arg_first) );
char*  one_argument_cs       args( (char* argument, char* arg_first) );

/* lookup.c */
int flag_lookup args( ( const char *name, const struct flag_type *flag_table) );

/* magic.c */
bool    check_absorb    args( (CHAR_DATA *ch,CHAR_DATA *victim,int dt) );
int     mana_cost       (CHAR_DATA *ch, int min_mana, int sn);
int     demon_lookup    args( ( char * name, char * real_name) );
bool    demonai_match   args( ( CHAR_DATA *ch, int id_check) );
void    evaluate_wounds args( (CHAR_DATA *ch, CHAR_DATA *victim) );
int     hometown_lookup args( ( int temple_vnum ) );
int      skill_lookup_bin  args( ( const char *name ) );
int      skill_lookup      args( ( const char *name ) );
int    slot_lookup    args( ( int slot ) );
bool    saves_spell     args( ( CHAR_DATA *ch, int level, CHAR_DATA *victim, int dam_type, int save_type ) );
bool saves_armor_of_god   args( ( CHAR_DATA *ch, int level, CHAR_DATA *victim ) );
void    obj_cast_spell    args( ( int sn, int level, CHAR_DATA *ch,
                    CHAR_DATA *victim, OBJ_DATA *obj ) );
void obj_cast_spell_2 args( ( int sn, int level, CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA *obj,int cast_bits ) );
void magic_spell_vict   args( (CHAR_DATA* ch, CHAR_DATA* victim, int cast_bits, int spell_level, int spell_num) );
void magic_spell  args( (CHAR_DATA* ch, char* argument, int cast_bits, int spell_level, int spell_num) );
int magic_power      args( (CHAR_DATA* ch) );

/* worship.c */
char* get_god_name     args( (CHAR_DATA* ch, CHAR_DATA* looker) );
bool  is_god_room      args( (CHAR_DATA* ch, ROOM_INDEX_DATA* location) );
bool is_clergy         args( (CHAR_DATA* ch) );
bool clergy_fails_help args( (CHAR_DATA* ch, CHAR_DATA* victim, sh_int sn) );
sh_int god_lookup   args( (CHAR_DATA* ch) );
void mob_punish     args( (CHAR_DATA* mob, CHAR_DATA* victim) );
extern bool is_supplicating;

/* save types for save spell */
#define SAVE_SPELL     0  /* provided as a default */
#define SAVE_MALEDICT  1  /* curse, blind, similar mal's */
#define SAVE_TRAVEL    2  /* summon, gate, etc. */
#define SAVE_DEATH     3  /* what it says, versus dying */
#define SAVE_BREATH    4  /* defined in case we want to do breaths different */
#define SAVE_OTHER     5  /* for non-magic related stuff (no int/wis adjust) */

/* defines for dam_message2 types */
#define GLOBAL_QUAKE_DAM_TYPE  -1
#define TRAP_PUNJIE_DAM_TYPE   -2
#define TAUNT_DAM_TYPE         -3
#define POISON_DAM_TYPE        -4
#define PLAGUE_DAM_TYPE        -5
#define TEMP_SHEER_DAM_TYPE    -6
#define GLOBAL_RIFT_DAM_TYPE   -7
#define GLOBAL_METEOR_DAM_TYPE -8
#define ROOM_METEOR_DAM_TYPE   -9
#define DEMON_WIMP_DAM_TYPE    -10
#define SUFFOCATE_DAM_TYPE     -11
#define RAGING_FIRE_DAM_TYPE   -12
#define TORNADO_LIT_DAM_TYPE   -13
#define TORNADO_WIND_DAM_TYPE  -14
#define METEOR_STORM_DAM_TYPE  -15
#define TORNADO_DROWN_DAM_TYPE -16
#define BLIZZARD_DAM_TYPE      -17
#define MOB_DAM_TYPE           -18
#define BOILING_BLOOD_DAM_TYPE -19
#define BLOODMIST_DAM_TYPE     -20
#define STARVATION_DAM_TYPE    -21
#define DEHYDRATION_DAM_TYPE   -22
#define BLEEDING_DAM_TYPE      -23

/* note.c */
void make_note( char * sender, char * subject, char * note_to, char * buf,
        int note_type);

/* save.c */
void     save_char_obj        args( ( CHAR_DATA *ch ) );
void     save_char_obj_1      args( (CHAR_DATA* ch, int flags) );
bool     load_char_obj        args( ( DESCRIPTOR_DATA *d, char *name ) );
void     save_modified_book   args( (OBJ_DATA* index) );

/* loads from storage directory instead of player */
bool    load_char_obj2  args( ( DESCRIPTOR_DATA *d, char *name ) );

/* skills.c */
bool     parse_gen_groups args( ( CHAR_DATA *ch,char *argument ) );
void     list_group_costs args( ( CHAR_DATA *ch ) );
void    list_group_known args( ( CHAR_DATA *ch ) );
int     exp_per_level   args( ( CHAR_DATA *ch ) );
void     check_improve    args( ( CHAR_DATA *ch, int sn, bool success,
                    int multiplier ) );
bool    has_skill       args( ( CHAR_DATA *ch, int sn) );
int     group_lookup    args( (const char *name) );
void    gn_add        args( ( CHAR_DATA *ch, int gn) );
void     gn_remove    args( ( CHAR_DATA *ch, int gn) );
void     group_add    args( ( CHAR_DATA *ch, const char *name, bool deduct) );
void    group_remove    args( ( CHAR_DATA *ch, const char *name) );

/* special.c */
SF *    spec_lookup    args( ( const char *name ) );
char *    spec_name    args( ( SPEC_FUN *function ) );

/* subclass.c */
void    strip_char_subclass    args( ( CHAR_DATA *ch ) );
void    make_char_monksub    args( ( CHAR_DATA *ch, int subclass ) );
void    make_char_subclass    args( ( CHAR_DATA *ch, CHAR_DATA *rch, int subclass ) );

/* teleport.c */
RID *    room_by_name    args( ( char *target, int level, bool error) );

/* update.c */
void    advance_level    args( ( CHAR_DATA *ch, bool hide ) );
void    gain_exp    args( ( CHAR_DATA *ch, int gain ) );
void    gain_condition    args( ( CHAR_DATA *ch, int iCond, int value ) );
void    update_handler    args( ( void ) );
void    spec_update     args( ( CHAR_DATA *ch ) );
int     get_death_age    args((CHAR_DATA *ch));
char *    get_age_name    args((CHAR_DATA *ch));
void    weather_update  args( ( bool controlled ) );
void    obj_message_doggie_toy_run args( ( MESSAGE* run ) );
void    do_wizireport_update args( (CHAR_DATA* ch, int type) );
void    age_update_char  args( (CHAR_DATA* ch) );

/* note.c */
void auto_mark_note     args(( CHAR_DATA *ch, CHAR_DATA *victim, char *argument ));

/* mob_prog.c */
void  program_flow    args( ( sh_int vnum, char *source, CHAR_DATA *mob, CHAR_DATA *ch,
                              const void *arg1, const void *arg2 ) );
void  mp_act_trigger  args( ( char *argument, CHAR_DATA *mob, CHAR_DATA *ch,
                              const void *arg1, const void *arg2, int type ) );
bool  mp_percent_trigger args( ( CHAR_DATA *mob, CHAR_DATA *ch,
                              const void *arg1, const void *arg2, int type ) );
void  mp_bribe_trigger  args( ( CHAR_DATA *mob, CHAR_DATA *ch, int amount ) );
bool  mp_exit_trigger   args( ( CHAR_DATA *ch, int dir ) );
void  mp_give_trigger   args( ( CHAR_DATA *mob, CHAR_DATA *ch, OBJ_DATA *obj ) );
void  mp_greet_trigger  args( ( CHAR_DATA *ch ) );
void  mp_hprct_trigger  args( ( CHAR_DATA *mob, CHAR_DATA *ch ) );

/* mob_cmds.c */
void  mob_interpret  args( ( CHAR_DATA *ch, char *argument ) );
void  do_mpcast      args( (CHAR_DATA* ch, char* argument) );

#undef    CD
#undef    MID
#undef    OD
#undef    OID
#undef    RID
#undef    SF
#undef AD

#define OBJ_VNUM_MOB_CLAN_REGISTER 19827
#define CLAN_LEADER    (-1)
#define CLAN_POSITION1 ( 1)
#define CLAN_POSITION2 ( 2)
#define CLAN_POSITION3 ( 3)

struct clan_type
{
   char*    name;
   char*    who_name;
   char*    leader_title;
   char*    position1_title;
   char*    position2_title;
   char*    position3_title;
   sh_int   rank;
   sh_int   clan_race;
   sh_int   clan_class;
   long     gold;
   long     leader_id;
   long     position1_id;
   long     position2_id;
   long     position3_id;
   char*    leader_name;
   char*    position1_name;
   char*    position2_name;
   char*    position3_name;
   bool     valid;
   bool     is_public;
   ROSTER*  proster;
};

extern struct clan_type clan_table[MAX_CLAN];
