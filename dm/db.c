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

static const char rcsid[] = "$Id: db.c,v 1.237 2004/11/25 08:52:20 fizzfaldt Exp $";

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
#include <sys/resource.h>
#endif

#include "merc.h"
#include "db.h"
#include "recycle.h"
#include "music.h"
#include "lookup.h"
#include "clan.h"
#include "worship.h"
#include "code.h"
#include "tables.h"
#include "database.h"

#if !defined(macintosh)
extern    int    _filbuf        args( (FILE *) );
#endif

#if !defined(OLD_RAND)
/* long random(); */
#if !defined(MSDOS)
void srandom(unsigned int);
#endif
int getpid();
time_t time(time_t *tloc);
#endif

/* for boot_db */
void    moon_update args( (void) );
void    load_clans  args( (void) );
int      sql_house   args( (int house) );

/* externals for counting purposes */
extern   OBJ_DATA*         obj_free;
extern   CHAR_DATA*        char_free;
extern   DESCRIPTOR_DATA*  descriptor_free;
extern   PC_DATA*          pcdata_free;
extern   AFFECT_DATA*      affect_free;
extern   MESSAGE*          message_free;

/*
* Globals.
*/
LIST_DATA*        house_deposits[MAX_HOUSE];
LIST_DATA*        clan_deposits[MAX_CLAN];

HELP_DATA *        help_first;
HELP_DATA *        help_last;

SHOP_DATA *        shop_first;
SHOP_DATA *        shop_last;

MPROG_CODE *            mprog_list;
NOTE_DATA *        note_free;

ROOM_AFFECT_DATA *    first_room_affect;
ROOM_AFFECT_DATA *    current_room_affect;
char            bug_buf        [2*MAX_INPUT_LENGTH];
CHAR_DATA *        char_list;
bool            is_test;
int                     votes[20];
long                    unhoused_pk[MAX_HOUSE];
long                    unique_kills[MAX_HOUSE];
long                    raids_defended[MAX_HOUSE];
long                    raids_nodefender[MAX_HOUSE];
long                    unhoused_kills[MAX_HOUSE];
long                    covenant_allies[MAX_HOUSE];
long                    house_kills[MAX_HOUSE];
long                    house_pks[MAX_HOUSE];
long                    house_downs[MAX_HOUSE];
time_t                  morale_reset_time = 0;
int             black_market_data_free = 0;
int             black_market_data[2000][3];
int             hero_count = 0;
long            class_count[MAX_CLASS];
long            race_count[MAX_PC_RACE];
long            house_account[MAX_HOUSE];
long            taxes_collected;
char *            help_greeting;
char            log_buf        [2*MAX_INPUT_LENGTH];
bool                    log_mobs = FALSE;
KILL_DATA        kill_table    [MAX_LEVEL];
NOTE_DATA *        note_list;
OBJ_DATA *        object_list;
WEATHER_DATA        weather_info;

MESSAGE *        message_pulse_list = NULL;
MESSAGE *        message_tick_list = NULL;

long            total_affects;
int             autoreboot_hour;
int             autoreboot_warning_hour;
bool            immrp_revolt = TRUE;
bool            immrp_blood;
bool            immrp_darkness;
bool           all_magic_gone = FALSE;
bool            allow_rang;
bool            allow_elem;
bool            allow_thief;
bool        resolve_no_ip;
long            mem_counts[15];
long            mem_used[42];
int            pulse_point = PULSE_TICK + 1; /*tick mechanism moved to here*/
int            frog_house;
long           last_note_id = 0;
AFFECT_DATA**     death_affects;

/* Lottery Stuff */
sh_int         lottery_race_count[MAX_PC_RACE];
LIST_DATA*     lottery_race_players[MAX_PC_RACE];
time_t         lottery_race_last_run[MAX_PC_RACE];
time_t         lottery_race_reset;

LIST_DATA*     book_race_list;

/* Roster stuff */
void add_to_roster args( (char* name, int slot, int prob, int probtime, bool storage, bool denied, sh_int type, long login_time, bool sort) );
void add_roster    args( (char* name, int clan, int house, int brand, int worship, int prob, int probtime, long login_time, bool storage, bool denied) );
void sort_rosters  args( (ROSTER** list, int (*compare) (const void*, const void*)) );
void dump_time_wipe_roster   args ( (void ) );
void add_to_time_wipe_roster args( (char* name, bool storage, bool denied, long login_time, char* filename) );
int  roster_compare          args( (const void* input1, const void* input2) );
ROSTER* tw_roster = NULL;  /* Roster to be timewiped */

/*do_area stuff*/
bool do_areas_is_valid_level_format args ( (char* level) );
int do_areas_compare_title args ( (const void* input1, const void* input2) );
int do_areas_compare_builder args ( (const void* input1, const void* input2) );
int do_areas_compare_level args ( (const void* input1, const void* input2) );
int do_areas_compare_special args ( (const void* input1, const void* input2) );
bool do_areas_restrict_none args ( (const char* restrict_by, const AREA_DATA* check) );
bool do_areas_restrict_builder args ( (const char* restrict_by, const AREA_DATA* check) );
bool do_areas_restrict_level args ( (const char* restrict_by, const AREA_DATA* check) );
bool do_areas_restrict_special args ( (const char* restrict_by, const AREA_DATA* check) );
void do_areas_print args
(
   (
      CHAR_DATA* ch,
      AREA_DATA** arr,
      sh_int size,
      const char* restrictor,
      bool (*restrict) (const char*, const AREA_DATA*)
   )
);

sh_int grn_unique;
sh_int grn_spider;
sh_int grn_human;
sh_int grn_dwarf;
sh_int grn_elf;
sh_int grn_grey_elf;
sh_int grn_dark_elf;
sh_int grn_centaur;
sh_int grn_troll;
sh_int grn_giant;
sh_int grn_gnome;
sh_int grn_draconian;
sh_int grn_ethereal;
sh_int grn_changeling;
sh_int grn_illithid;
sh_int grn_halfling;
sh_int grn_minotaur;
sh_int grn_arborian;
sh_int grn_book;
sh_int grn_demon;
sh_int grn_bat;
sh_int grn_bear;
sh_int grn_cat;
sh_int grn_centipede;
sh_int grn_duergar;
sh_int grn_dog;
sh_int grn_doll;
sh_int grn_doppelganger;
sh_int grn_dragon;
sh_int grn_fido;
sh_int grn_fish;
sh_int grn_fox;
sh_int grn_goblin;
sh_int grn_hobgoblin;
sh_int grn_kobold;
sh_int grn_lizard;
sh_int grn_modron;
sh_int grn_pig;
sh_int grn_rabbit;
sh_int grn_school_monster;
sh_int grn_snake;
sh_int grn_song_bird;
sh_int grn_water_fowl;
sh_int grn_wolf;
sh_int grn_wyvern;

sh_int gsn_hyper;
sh_int gsn_nap;
sh_int gsn_nap_timer;
sh_int gsn_nap_slow;

sh_int gsn_spiritblade;
sh_int gsn_team_spirit;
sh_int gsn_power_word_fear;
sh_int gsn_disintegrate;
sh_int gsn_ventriloquate;
sh_int gsn_iron_scales_dragon;
sh_int gsn_muster;
sh_int gsn_perception;
sh_int gsn_hand_of_vengeance;
sh_int gsn_trace;
sh_int gsn_prismatic_sphere;
sh_int gsn_simulacrum;
sh_int gsn_rite_of_darkness;
sh_int gsn_rite_of_darkness_timer;
sh_int gsn_rite_of_darkness_taint;
sh_int gsn_dracolich_evil;
sh_int gsn_dracolich_bloodlust;
sh_int gsn_intangibility;
sh_int gsn_intangibility_timer;
sh_int gsn_phantasmal_force;
sh_int gsn_phantasmal_force_area;
sh_int gsn_phantasmal_force_illusion;
sh_int gsn_siren_screech;
sh_int gsn_wail_wind;
sh_int gsn_esurience;
sh_int gsn_venueport;
sh_int gsn_scribe;
sh_int gsn_virulent_cysts;
sh_int gsn_seize;

sh_int gsn_purity;
sh_int gsn_helper_robe;
sh_int gsn_darkfocus;
sh_int gsn_swing;
sh_int gsn_impale;
sh_int gsn_jump;
sh_int gsn_command;
sh_int gsn_recruit;
sh_int gsn_dark_armor;
sh_int gsn_aura_of_presence;

sh_int gsn_strengthen_bone;

sh_int gsn_resurrection;

sh_int gsn_channel;
sh_int gsn_eyes_of_the_bandit;
sh_int gsn_warding;
sh_int gsn_guard_call;

/* dragon breaths */
sh_int gsn_acid_breath;
sh_int gsn_fire_breath;
sh_int gsn_frost_breath;
sh_int gsn_gas_breath;
sh_int gsn_lightning_breath;

/* Worship-related */
sh_int gsn_worship;
sh_int gsn_blessing;
sh_int gsn_detriment;
sh_int gsn_punishment;
sh_int gsn_teleport;
sh_int gsn_plague_of_the_worm;
sh_int gsn_worm_damage;
sh_int gsn_divine_inferno;
sh_int gsn_lightblast;
sh_int gsn_word_recall;
sh_int gsn_power_word_recall;
sh_int gsn_need;
sh_int gsn_gate;
sh_int gsn_summon;

/* Vrrin's Brand */
sh_int gsn_will_power;
sh_int gsn_will_dread;
sh_int gsn_will_oblivion;
sh_int gsn_mantle_oblivion;

/* Malignus's Brand */
sh_int gsn_mob_timer;
sh_int gsn_mob_pain;

/* elem subs gsn's */
sh_int gsn_gale_winds;
sh_int gsn_air_dagger;
sh_int gsn_napalm;
sh_int gsn_incinerate;
sh_int gsn_sunburst;
sh_int gsn_nova;
sh_int gsn_inferno;
sh_int gsn_flame_form;
sh_int gsn_imbue_flame;
sh_int gsn_ashes_to_ashes;
sh_int gsn_raging_fire;

sh_int gsn_charge_weapon;
sh_int gsn_tornado;
sh_int gsn_airshield;
sh_int gsn_suffocate;
sh_int gsn_jet_stream;
sh_int gsn_cyclone;
sh_int gsn_implosion;
sh_int gsn_thunder;
sh_int gsn_storm;
sh_int gsn_mass_fly;
sh_int gsn_dust_devil;
sh_int gsn_vortex;
sh_int gsn_wraithform;

sh_int gsn_earthbind;
sh_int gsn_stone;
sh_int gsn_stalagmite;
sh_int gsn_tremor;
sh_int gsn_avalanche;
sh_int gsn_cave_in;
sh_int gsn_crushing_hands;
sh_int gsn_shield_of_earth;
sh_int gsn_stoney_grasp;
sh_int gsn_burrow;
sh_int gsn_meteor_storm;
sh_int gsn_earth_form;

sh_int gsn_geyser;
sh_int gsn_water_spout;
sh_int gsn_deluge;
sh_int gsn_whirlpool;
sh_int gsn_blizzard;
sh_int gsn_spring_rains;
sh_int gsn_submerge;
sh_int gsn_freeze;
sh_int gsn_frost_charge;
sh_int gsn_waterwalk;
sh_int gsn_ice_armor;
sh_int gsn_water_form;
sh_int gsn_water_of_life;
sh_int gsn_fountain_of_youth;
sh_int gsn_rip_tide;
sh_int gsn_waterbreathing;

sh_int gsn_cunning_strike;
sh_int gsn_thugs_stance;
sh_int gsn_thugs_stance_timer;
sh_int gsn_misinformation;
sh_int gsn_silver_tongue;
sh_int gsn_marauder_bribe;
sh_int gsn_song_of_shadows;
sh_int gsn_soothing_voice;
sh_int gsn_gate_call;
sh_int gsn_guild_cloth;
sh_int gsn_incendiary_device;
sh_int gsn_healing_salve;
sh_int gsn_sonic_whistle;
sh_int gsn_itching_powder;
sh_int gsn_dance_of_the_rogue;

sh_int gsn_tanning;
sh_int gsn_hire_crew;
sh_int gsn_thrust;
sh_int gsn_rapier;
sh_int gsn_tumble;
sh_int gsn_offhand_disarm;
sh_int gsn_emblem_law;
sh_int gsn_seal_of_justice;
sh_int gsn_ransack;
sh_int gsn_entrench;
sh_int gsn_charging_retreat;
sh_int gsn_block_retreat;
sh_int gsn_gag;
sh_int gsn_bind;
sh_int gsn_blindfold;
sh_int gsn_bushwhack;
sh_int gsn_lookout;
sh_int gsn_battle_tactics;
sh_int gsn_dual_parry;
sh_int gsn_sharpen;
sh_int gsn_ninjitsu;
sh_int gsn_caltrops;
sh_int gsn_critical_strike;
sh_int gsn_feign_death;
sh_int gsn_sham_brand1;
sh_int gsn_sham_brand2;
sh_int gsn_delusions;
sh_int gsn_obscure;
sh_int gsn_anvil_brand;
sh_int gsn_mute;
sh_int gsn_getaway;
sh_int gsn_trapstun;
sh_int gsn_beaststance;
sh_int gsn_trapmaking;
sh_int gsn_killer_instinct;
sh_int gsn_battlescream;
sh_int gsn_carving;
sh_int gsn_toughen;
sh_int gsn_crushingblow;
sh_int gsn_wildfury;
sh_int gsn_skull_bash;
sh_int gsn_globe_darkness;
sh_int gsn_demon_swarm;
sh_int gsn_warpaint;
sh_int gsn_two_hand_wield;
sh_int gsn_toad;
sh_int gsn_taunt;
sh_int gsn_nausea;
sh_int gsn_fist_god;
sh_int gsn_justice_brand2;
sh_int gsn_justice_brand;
sh_int gsn_justice_brand_wrath;
sh_int gsn_alchemy;
sh_int gsn_temporal_shield;
sh_int gsn_temporal_shear;
sh_int gsn_grace;
sh_int gsn_know_time;
sh_int gsn_identify;
sh_int gsn_know_alignment;
sh_int gsn_halo_of_eyes;
sh_int gsn_shrink;
sh_int gsn_enlarge;
sh_int gsn_illusionary_wall;
sh_int gsn_strabismus;
sh_int gsn_fog_conceilment;
sh_int gsn_hall_mirrors;
sh_int gsn_phasing;
sh_int gsn_cloak_brave;
sh_int gsn_benediction;
sh_int gsn_barrier;
sh_int gsn_mentallink;
sh_int gsn_side_step;
sh_int gsn_backstab;
sh_int gsn_detect_hidden;
sh_int gsn_blind_fighting;
sh_int gsn_dodge;
sh_int gsn_envenom;
sh_int gsn_hide;
sh_int gsn_adv_hide;
sh_int gsn_exp_hide;
sh_int gsn_bribe;
sh_int gsn_peek;
sh_int gsn_pick_lock;
sh_int gsn_sneak;
sh_int gsn_steal;
sh_int gsn_plant;
sh_int gsn_offer;
sh_int gsn_fence;
sh_int gsn_silence;
sh_int gsn_disarm;
sh_int gsn_firemastery;
sh_int gsn_watermastery;
sh_int gsn_earthmastery;
sh_int gsn_airmastery;
sh_int gsn_enhanced_damage;
sh_int gsn_kick;
sh_int gsn_parry;
sh_int gsn_rescue;
sh_int gsn_second_attack;
sh_int gsn_third_attack;
sh_int gsn_fourth_attack;
sh_int gsn_fifth_attack;
sh_int gsn_sixth_attack;

sh_int gsn_lemniscate_brand;
sh_int gsn_blindness;

sh_int gsn_repent;
sh_int gsn_faith_healing;

sh_int gsn_charm_person;
sh_int gsn_backup;
sh_int gsn_kidnap;
sh_int gsn_propaganda;
sh_int gsn_aura_defiance;
sh_int gsn_stealth;
sh_int gsn_smoke_screen;
sh_int gsn_distort_time;
sh_int gsn_distort_time_faster;
sh_int gsn_distort_time_slower;
sh_int gsn_smite;
sh_int gsn_ancient_plague;
sh_int gsn_boiling_blood;
sh_int gsn_bloodmist;
sh_int gsn_curse;
sh_int gsn_invis;
sh_int gsn_mass_invis;
sh_int gsn_poison;
sh_int gsn_plague;
sh_int gsn_sleep;
sh_int gsn_sleep_timer;
sh_int gsn_soulcraft;
sh_int gsn_fly;
sh_int gsn_sanctuary;
sh_int gsn_chromatic_shield;
sh_int gsn_divine_protect;
/* new gsns */

sh_int gsn_axe;
sh_int gsn_dagger;
sh_int gsn_flail;
sh_int gsn_mace;
sh_int gsn_polearm;
sh_int gsn_shield_block;
sh_int gsn_spear;
sh_int gsn_sword;
sh_int gsn_whip;

sh_int gsn_archery;
sh_int gsn_adv_archery;
sh_int gsn_exp_archery;
sh_int gsn_adv_axe;
sh_int gsn_exp_axe;
sh_int gsn_adv_dagger;
sh_int gsn_exp_dagger;
sh_int gsn_adv_flail;
sh_int gsn_exp_flail;
sh_int gsn_adv_mace;
sh_int gsn_exp_mace;
sh_int gsn_adv_polearm;
sh_int gsn_exp_polearm;
sh_int gsn_adv_spear;
sh_int gsn_exp_spear;
sh_int gsn_adv_sword;
sh_int gsn_exp_sword;
sh_int gsn_adv_whip;
sh_int gsn_exp_whip;
sh_int gsn_adv_staff;
sh_int gsn_exp_staff;

sh_int gsn_shackles;
sh_int gsn_shackle;
sh_int gsn_shieldbash;
sh_int gsn_bash;
sh_int gsn_berserk;
sh_int gsn_rage;
sh_int gsn_dirt;
sh_int gsn_hand_to_hand;
sh_int gsn_trip;

sh_int gsn_recovery;
sh_int gsn_fast_healing;
sh_int gsn_haggle;
sh_int gsn_lore;
sh_int gsn_soulscry;
sh_int gsn_meditation;
sh_int gsn_meteor;
sh_int gsn_scrolls;
sh_int gsn_staves;
sh_int gsn_wands;
sh_int gsn_recall;

sh_int gsn_crush;
sh_int gsn_ground_control;

sh_int gsn_absorb;
sh_int gsn_camouflage;
sh_int gsn_acute_vision;
sh_int gsn_ambush;
sh_int gsn_laying_hands;
sh_int gsn_battlecry;
sh_int gsn_whirlwind;
sh_int gsn_empower;
sh_int gsn_circle;
sh_int gsn_dual_backstab;
sh_int gsn_power_word_stun;
sh_int gsn_power_word_fear;
sh_int gsn_skin;
sh_int gsn_camp;
sh_int gsn_steel_nerves;

sh_int gsn_morph_red;
sh_int gsn_morph_black;
sh_int gsn_morph_blue;
sh_int gsn_morph_green;
sh_int gsn_morph_white;
sh_int gsn_morph_winged;
sh_int gsn_morph_archangel;

sh_int gsn_morph_dragon;
sh_int gsn_breath_morph;
sh_int gsn_animate_dead;
sh_int gsn_barkskin;
sh_int gsn_animal_call;
sh_int gsn_aura_of_sustenance;
sh_int gsn_halo_of_the_sun;
sh_int gsn_shroud;
sh_int gsn_shadowgate;
sh_int gsn_eye_of_the_predator;
sh_int gsn_lunge;
sh_int gsn_blackjack;
sh_int gsn_vigilance;
sh_int gsn_dual_wield;
sh_int gsn_trance;
sh_int gsn_wanted;
sh_int gsn_cleave;
sh_int gsn_herb;
sh_int gsn_bandage;
sh_int gsn_deathstrike;
sh_int gsn_protective_shield;
sh_int gsn_consecrate;
sh_int gsn_timestop;
sh_int gsn_timestop_done;
sh_int gsn_true_sight;
sh_int gsn_butcher;
sh_int gsn_shadowstrike;
sh_int gsn_battleshield;
sh_int gsn_counter_parry;
sh_int gsn_riposte;
sh_int gsn_fireshield;
sh_int gsn_iceshield;
sh_int gsn_cure_blindness;
sh_int gsn_cure_disease;
sh_int gsn_cure_poison;
sh_int gsn_imbue_regeneration;
sh_int gsn_giant_strength;
sh_int gsn_heal;
sh_int gsn_remove_curse;
sh_int gsn_cancellation;
sh_int gsn_vanish;
sh_int gsn_riot;
sh_int gsn_embalm;
sh_int gsn_dark_dream;
sh_int gsn_counter;
sh_int gsn_chaos_mind;
sh_int gsn_revolt;
sh_int gsn_acid_spit;
sh_int gsn_bear_call;
sh_int gsn_trophy;
sh_int gsn_tail;
sh_int gsn_spellbane;
sh_int gsn_request;
sh_int gsn_endure;
sh_int gsn_dragon_spirit;
sh_int gsn_follow_through;
sh_int gsn_nerve;
sh_int gsn_palm;
sh_int gsn_throw;
sh_int gsn_poison_dust;
sh_int gsn_blindness_dust;
sh_int gsn_roll;
sh_int gsn_block;
sh_int gsn_strangle;
sh_int gsn_warcry;
sh_int gsn_no_magic_toggle;
sh_int gsn_ramming;
sh_int gsn_chimera_lion;
sh_int gsn_chimera_goat;
sh_int gsn_strange_form;
sh_int gsn_downstrike;
sh_int gsn_enlist;
sh_int gsn_shadowplane;
sh_int gsn_blackjack_timer;
sh_int gsn_strangle_timer;
sh_int gsn_tame;
sh_int gsn_find_water;
sh_int gsn_track;
sh_int gsn_shield_cleave;
sh_int gsn_spellcraft;
sh_int gsn_cloak_form;
sh_int gsn_demand;
sh_int gsn_tertiary_wield;
sh_int gsn_breath_fire;
sh_int gsn_awareness;
sh_int gsn_forest_blending;
sh_int gsn_gaseous_form;
sh_int gsn_rear_kick;
sh_int gsn_shapeshift;
sh_int gsn_regeneration;
sh_int gsn_parrot;
sh_int gsn_door_bash;
sh_int gsn_multistrike;
sh_int gsn_wolverine_brand;
sh_int gsn_flame_scorch;
sh_int gsn_flame_scorch_blue;
sh_int gsn_flame_scorch_green;
sh_int gsn_mutilated_left_arm;
sh_int gsn_mutilated_left_hand;
sh_int gsn_mutilated_left_leg;
sh_int gsn_mutilated_right_arm;
sh_int gsn_mutilated_right_leg;
sh_int gsn_mutilated_right_hand;
sh_int gsn_blood_tide;
sh_int gsn_flail_arms;
sh_int gsn_shattered_bone;
sh_int gsn_earthfade;
sh_int gsn_forget;
sh_int gsn_divine_touch;
sh_int gsn_grounding;
sh_int gsn_shock_sphere;
sh_int gsn_forage;
sh_int gsn_assassinate;
sh_int gsn_defend;
sh_int gsn_intimidate;
sh_int gsn_escape;
sh_int gsn_moving_ambush;
sh_int gsn_pugil;
sh_int gsn_staff;
sh_int gsn_evaluation;
sh_int gsn_enhanced_damage_two;
sh_int gsn_protection_heat_cold;
sh_int gsn_rally;
sh_int gsn_tactics;
sh_int gsn_lash;
sh_int gsn_prevent_healing;
sh_int gsn_regeneration;
sh_int gsn_undead_drain;
sh_int gsn_iron_resolve;
sh_int gsn_quiet_movement;
sh_int gsn_atrophy;
sh_int gsn_focus;
sh_int gsn_psionic_blast;
sh_int gsn_spike;
sh_int gsn_starvation;
sh_int gsn_dehydrated;
sh_int gsn_phoenix;
sh_int gsn_void;
sh_int gsn_peace;
sh_int gsn_peace_brand;
sh_int gsn_owl_brand;
sh_int gsn_toothless;
sh_int gsn_worm_brand;
sh_int gsn_worm_hide;
sh_int gsn_battleaxe_haste;
sh_int gsn_battleaxe_defense;
sh_int gsn_hector_brand_spark;
sh_int gsn_hector_brand_essence;
sh_int gsn_lestregus_brand_eternal;
sh_int gsn_lestregus_brand_taint;
sh_int gsn_lestregus_brand_thirst;
sh_int gsn_utara_brand_blur;
sh_int gsn_utara_brand2;
sh_int gsn_drinlinda_attack_brand;
sh_int gsn_drinlinda_defense_brand;
sh_int gsn_drithentir_attack_brand;
sh_int gsn_drithentir_vampiric_brand;
sh_int gsn_charm_timer;
sh_int gsn_vermin_timer;
sh_int gsn_eye_tiger;
sh_int gsn_rot;
sh_int gsn_chi_healing;
sh_int gsn_flood_room;
sh_int gsn_forestwalk;
sh_int gsn_charge;
sh_int gsn_blitz;
sh_int gsn_summon_steed;
sh_int gsn_doublestrike;
sh_int gsn_support;
sh_int gsn_rebel;
sh_int gsn_devote;
sh_int gsn_lurk;
sh_int gsn_surround;
sh_int gsn_target;
sh_int gsn_darkforge;
sh_int gsn_dimmak;
sh_int gsn_cripple;
sh_int gsn_sdragon_armor;
sh_int gsn_sdragon_valor;
sh_int gsn_kung_fu;
sh_int gsn_chant;
sh_int gsn_healing_trance;
sh_int gsn_spiritwalk;
sh_int gsn_pouncing_tiger;
sh_int gsn_knockdown;
sh_int gsn_darkforge_fists;
sh_int gsn_fists_fire;
sh_int gsn_fists_ice;
sh_int gsn_fists_poison;
sh_int gsn_fists_divine;
sh_int gsn_fists_fury;
sh_int gsn_open_claw;
sh_int gsn_harmony;
sh_int gsn_iron_will;
sh_int gsn_stunning_strike_timer;
sh_int gsn_stunning_strike;
sh_int gsn_counter_defense;
sh_int gsn_chi_attack;
sh_int gsn_acrobatics;
sh_int gsn_evasive_dodge;
sh_int gsn_dchant;
sh_int gsn_judo;
sh_int gsn_dance_venom;
sh_int gsn_subrank;
sh_int gsn_jump_kick;
sh_int gsn_spin_kick;
sh_int gsn_iron_palm;
sh_int gsn_nether_shroud;
sh_int gsn_black_mantle;
sh_int gsn_coil_snakes;
sh_int gsn_dragonmount;
sh_int gsn_high_herb;
sh_int gsn_hang;
sh_int gsn_conjure;
sh_int gsn_drain;
sh_int gsn_diagnose;
/*sh_int gsn_war_rage;
sh_int gsn_xeonauz_sword;*/
sh_int gsn_tinker;
sh_int gsn_forge_weapon;
sh_int gsn_sense_evil;
sh_int gsn_armor_of_god;
sh_int gsn_guardian_angel;
sh_int gsn_lightbind;
sh_int gsn_telekinesis;
sh_int gsn_wind_shear;
sh_int gsn_dancestaves;
sh_int gsn_acidfire;
sh_int gsn_devour;
sh_int gsn_smelly;
sh_int gsn_carebearstare;
sh_int gsn_krazyeyes;
sh_int gsn_sonicbelch;
sh_int gsn_trollfart;
sh_int gsn_taichi;
sh_int gsn_star_shower;
sh_int gsn_davatar_assault;
sh_int gsn_stalking;
sh_int gsn_field_surgery;
sh_int gsn_deathfire_missiles;
sh_int gsn_abyssal_claws;
sh_int gsn_destruction_shockwave;
sh_int gsn_evasive_defense;
sh_int gsn_penetratingthrust;
sh_int gsn_wagon_strike;
sh_int gsn_megaslash;

/* added originally for efficiency: */
sh_int gsn_cure_critical;
sh_int gsn_firestream;
sh_int gsn_misdirection;
sh_int gsn_heavenly_wrath;
sh_int gsn_create_spring;
sh_int gsn_create_food;
sh_int gsn_create_water;
sh_int gsn_mass_healing;
sh_int gsn_windwall;
sh_int gsn_detect_good;
sh_int gsn_detect_evil;
sh_int gsn_haste;
sh_int gsn_bless;
sh_int gsn_faerie_fog;
sh_int gsn_faerie_fire;
sh_int gsn_infravision;
sh_int gsn_pass_door;
sh_int gsn_voodoo;
sh_int gsn_earthquake;
sh_int gsn_call_of_duty;
sh_int gsn_holy_fire;
sh_int gsn_blade_barrier;
sh_int gsn_demonfire;
sh_int gsn_reserved;
sh_int gsn_heavenly_fire;
sh_int gsn_protection_evil;
sh_int gsn_protection_good;
sh_int gsn_chill_touch;
sh_int gsn_concatenate;
sh_int gsn_dispel_evil;
sh_int gsn_dispel_good;
sh_int gsn_energy_drain;
sh_int gsn_fireball;
sh_int gsn_demonic_enchant;
sh_int gsn_frenzy;
sh_int gsn_canopy_walk;
sh_int gsn_weaken;
sh_int gsn_shocking_grasp;
sh_int gsn_mind_sear;
sh_int gsn_lightning_bolt;
sh_int gsn_acid_blast;
sh_int gsn_shroud_of_pro;
sh_int gsn_ameteor;
sh_int gsn_beastial_command;
sh_int gsn_call_of_the_wild;
sh_int gsn_cure_light;
sh_int gsn_cure_serious;
sh_int gsn_restoration;
sh_int gsn_refresh;
sh_int gsn_power_word_kill;
sh_int gsn_blessing_of_darkness;
sh_int gsn_insight;
sh_int gsn_defilement;
sh_int gsn_lifeline;
sh_int gsn_cloak_of_transgression;
sh_int gsn_decoy;
sh_int gsn_health;
sh_int gsn_forge;
sh_int gsn_armor;
sh_int gsn_ray_of_enfeeblement;
sh_int gsn_calm;
sh_int gsn_change_sex;
sh_int gsn_detect_invis;
sh_int gsn_detect_magic;
sh_int gsn_shield;
sh_int gsn_slow;
sh_int gsn_stone_skin;
sh_int gsn_holyshield;
sh_int gsn_mass_slow;
sh_int gsn_wrath;
sh_int gsn_dark_wrath;
sh_int gsn_mass_haste;
sh_int gsn_hunters_knife;
sh_int gsn_tsunami;
sh_int gsn_iceball;
sh_int gsn_cone_of_cold;
sh_int gsn_magic_missile;
sh_int gsn_colour_spray;
sh_int gsn_fire_and_ice;
sh_int gsn_earthmaw;
sh_int gsn_wither;
sh_int gsn_taint;
sh_int gsn_gout_of_maggots;
sh_int gsn_strengthen_construct;
sh_int gsn_darkshield;
sh_int gsn_lightshield;
sh_int gsn_lower_resistance;
sh_int gsn_raise_resistance;
sh_int gsn_invisible_mail;
sh_int gsn_freeze_person;
sh_int gsn_dispel_magic;
sh_int gsn_retribution;
sh_int gsn_chain_lightning;
sh_int gsn_unholy_fire;
sh_int gsn_familiar;
sh_int gsn_deathspell;
sh_int gsn_purify;
sh_int gsn_psistorm;
/*
sh_int gsn_;
*/

/*
* Locals.
*/
MOB_INDEX_DATA*      mob_index_hash       [MAX_KEY_HASH];
OBJ_INDEX_DATA*      obj_index_hash       [MAX_KEY_HASH];
ROOM_INDEX_DATA*     room_index_hash      [MAX_KEY_HASH];
char*                string_hash          [MAX_KEY_HASH];
LIST_DATA*           npc_id_hash_table    [MAX_KEY_HASH];
LIST_DATA*           pc_id_hash_table     [MAX_KEY_HASH];

AREA_DATA *        area_first;
AREA_DATA *        area_last;

char *            string_space;
char *            top_string;
char            str_empty    [1];
char                    str_none        [5];

int            top_affect;
int            top_area;
int            top_ed;
int            top_exit;
int            top_help;
int            top_mob_index;
int            top_obj_index;
int            top_reset;
int            top_room;
int            top_shop;
int             mobile_count = 0;
int            newmobs = 0;
int            newobjs = 0;

/* demon seeding */
int            demon_eggs = -1;
int            demon_babies;
int            demon_adults;

/*
* Memory management.
* Increase MAX_STRING if you have too.
* Tune the others only if you understand what you're doing.
*/
#define                 MAX_STRING      10000000
/* Original was 1483120:  Other original was 3753120 other
Original was 4800000  Recent original was 7000000 -- Kye*/
#define            MAX_PERM_BLOCK    262144
#define            MAX_MEM_LIST    14

void *            rgFreeList    [MAX_MEM_LIST];
const int        rgSizeList    [MAX_MEM_LIST]    =
{
   16, 32, 64, 128, 256, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072 - 64
};

int            nAllocString;
int            sAllocString;
int            nAllocPerm;
int            sAllocPerm;



/*
* Semi-locals.
*/
bool           fBootDb;
FILE*          fpArea;
FILE*          fpChar;
char           strArea[MAX_INPUT_LENGTH];
char           strPlr[MAX_INPUT_LENGTH];
char           area_dir[MAX_STRING_LENGTH];


/*
* Local booting procedures.
*/
void    init_mm         args( ( void ) );
void    load_area    args( ( FILE *fp ) );
void    load_area_music args( (FILE* fp) );
void    load_helps    args( ( FILE *fp ) );
void    load_old_mob    args( ( FILE *fp ) );
void    load_mobiles    args( ( FILE *fp ) );
void    load_old_obj    args( ( FILE *fp ) );
void    load_objects    args( ( FILE *fp ) );
void    load_resets    args( ( FILE *fp ) );
void    load_rooms    args( ( FILE *fp ) );
void    load_shops    args( ( FILE *fp ) );
void    load_socials    args( ( FILE *fp ) );
void    load_specials    args( ( FILE *fp ) );
void    load_mobprogs   args( ( FILE *fp ) );
void    load_quests     args( ( FILE *fp ) );
void    load_notes    args( ( void ) );
void    load_bans    args( ( void ) );
void    load_namebans   args( ( void ) );
void    load_globals    args( ( void ) );
void    load_wizireport args( ( void ) );
int     count_objects   args( ( void ) );

void    fix_exits    args( ( void ) );
void    fix_mobprogs    args( ( void ) );

void    reset_area    args( ( AREA_DATA * pArea ) );

void    seed_demons(void);

int   any_race_r = 0;
int   any_race_e2 = 0;
int   any_class_r = 0;
int   any_class_e2 = 0;
int   any_house_r = 0;
int   any_house_e2 = 0;
int   any_size_e2 = 0;

void boot_restricts()
{
   int cnt;

   /* Initialize the bitvectors */
   for (cnt = 0; cnt < MAX_PC_RACE; cnt++)
   {
      any_race_r |= pc_race_table[cnt].restrict_r_bit;
      any_race_e2 |= pc_race_table[cnt].restrict_e2_bit;
   }
   for (cnt = 0; cnt < MAX_CLASS; cnt++)
   {
      any_class_r |= class_table[cnt].restrict_r_bit;
      any_class_e2 |= class_table[cnt].restrict_e2_bit;
   }
   for (cnt = 0; cnt < MAX_HOUSE; cnt++)
   {
      any_house_r |= house_table[cnt].restrict_r_bit;
      any_house_e2 |= house_table[cnt].restrict_e2_bit;
   }
   for (cnt = 0; size_table[cnt].name; cnt++)
   {
      any_size_e2 |= size_table[cnt].restrict_e2_bit;
   }
}

void boot_areas()
{
   FILE* fpList;
   char* word;

   if
   (
      is_test &&
      (
         fpList = fopen(AREA_DIR_FILE, "r")
      ) != NULL
   )
   {
      word = fread_string(fpList);
      strcpy(area_dir, word);
      free_string(word);
      fclose(fpList);
      strcpy(log_buf, "AREAS LOADING FROM: ");
      log_string(strcat(log_buf, area_dir));
   }
   else
   {
      strcpy(area_dir, AREA_DIR);
   }
   fpList = NULL;
   if (is_test)
   {
      strcpy(strArea, area_dir);
      strcat(strArea, AREA_LIST_TEST);
      fpList = fopen(strArea, "r");
   }
   if (fpList == NULL)
   {
      strcpy(strArea, area_dir);
      strcat(strArea, AREA_LIST);
      if
      (
         (
            fpList = fopen(strArea, "r")
         ) == NULL
      )
      {
         perror(strArea);
         exit(1);
      }
   }

   /* Reset database tables */
   database_reset_quests();

   for (; ;)
   {
      word = fread_word(fpList);
      if (word[0] == '$')
      {
         free_string(word);
         break;
      }
      if (word[0] == '-')
      {
         fpArea = stdin;
      }
      else
      {
         strcpy(strArea, area_dir);
         strcat
         (
            strArea,
            word
         );
         if
         (
            (
               fpArea = fopen(strArea, "r")
            ) == NULL
         )
         {
            perror(strArea);
            exit(1);
         }
      }
      free_string(word);
      for (; ;)
      {
         if (fread_letter(fpArea) != '#')
         {
            bug("Boot_area: # not found.", 0);
            exit(1);
         }
         word = fread_word( fpArea );
         if (word[0] == '$')
         {
            free_string(word);
            break;
         }
         else if (!str_cmp(word, "AREA"))
         {
            load_area(fpArea);
         }
         else if (!str_cmp(word, "MUSIC"))
         {
            load_area_music(fpArea);
         }
         else if (!str_cmp(word, "HELPS"))
         {
            load_helps(fpArea);
         }
         else if (!str_cmp(word, "MOBOLD"))
         {
            load_old_mob(fpArea);
         }
         else if (!str_cmp(word, "MOBILES"))
         {
            load_mobiles(fpArea);
         }
         else if (!str_cmp(word, "MOBPROGS"))
         {
            load_mobprogs(fpArea);
         }
         else if (!str_cmp(word, "OBJOLD"))
         {
            load_old_obj(fpArea);
         }
         else if (!str_cmp(word, "OBJECTS"))
         {
            load_objects(fpArea);
         }
         else if (!str_cmp(word, "RESETS"))
         {
            load_resets(fpArea);
         }
         else if (!str_cmp(word, "ROOMS"))
         {
            load_rooms(fpArea);
         }
         else if (!str_cmp(word, "SHOPS"))
         {
            load_shops(fpArea);
         }
         else if (!str_cmp(word, "SOCIALS"))
         {
            load_socials(fpArea);
         }
         else if (!str_cmp(word, "SPECIALS"))
         {
            load_specials(fpArea);
         }
         else if (!str_cmp(word, "QUESTS"))
         {
            load_quests(fpArea);
         }
         else
         {
            bug("Boot_area: bad section name.", 0);
            exit(1);
         }
         free_string(word);
      }

      if (fpArea != stdin)
      {
         fclose(fpArea);
      }
      fpArea = NULL;
   }
   fclose(fpList);
}

void escape_quote(char* buffer, char* name)
{
   char buf[MAX_INPUT_LENGTH];
   char* ptr;
   char* nptr;

   /* copy string, but copy ' as \' */
   strcpy(buf, name);  /* antialias */
   ptr = buf;
   nptr = buffer;
   while (*ptr != '\0')
   {
      if (*ptr == '\'')
      {
         *nptr = '\\';
         nptr++;
      }
      *nptr = *ptr;
      nptr++;
      ptr++;
   }
   *nptr = '\0';
}

void boot_players()
{
   FILE* fpChar_list;
   FILE* masterconv = NULL;
   FILE* sql;
   char chkbuf[MAX_STRING_LENGTH];
   char* word;
   char* p;
   int vnum;
   char letter = 10;
   OBJ_INDEX_DATA* pObjIndex;
   char* email;
   int level;
   int trust;
   long login_time;
   int house;
   int clan;
   char* clan_name;
   char* worship_name;
   char* name = NULL;
   char* moniker = NULL;
   sh_int race;
   sh_int race_lottery;
   sh_int room;
   char  moniker_sql[MAX_INPUT_LENGTH];
   int brand;
   int worship;
   bool autopurge;
   int prob;
   int probtime;
   bool storage;
   bool denied;
   bool branding;
   bool ancient;
   int version;
   char* password = NULL;
   char* password_old = NULL;
   char* dir;
   char* list;
#if !defined(MSDOS)
   char catplr[MAX_INPUT_LENGTH];
   char pbuf[100];
#endif
   WORSHIP_TYPE* table;
   int worship_slot;
   ROOM_INDEX_DATA* room_index;
   DEPOSIT_TYPE* deposit;

   for (house = 0; house < MAX_HOUSE; house++)
   {
      house_deposits[house] = new_list();
   }
   house = 0;
   for (clan = 0; clan < MAX_CLAN; clan++)
   {
      clan_deposits[clan] = new_list();
   }
   clan = 0;
   dir = STORAGE_DIR;
   list = STORAGE_LIST;
   storage = TRUE;
   if
   (
      (
         sql = fopen(TEMP_FILE, "w")
      ) == NULL
   )
   {
      bug("sql_temp file: fopen", 0);
      perror(TEMP_FILE);
   }
   fprintf
   (
      sql,
      "delete from access;\n"
      "delete from old_forum_accounts;\n"
      "replace into old_forum_accounts (username, password, alias, email, url, "
      "notes, admin_ind, last_login, gmt_offset, lock_ind, priv_ind, "
      "delete_ind, location, last_access) select username, password, alias, "
      "email, url, notes, admin_ind, last_login, gmt_offset, lock_ind, "
      "priv_ind, delete_ind, location, last_access from forum_accounts;\n"
   );
   for (; ;)
   {
      if (storage)
      {
         log_string("Loading rosters from storage.");
      }
      else
      {
         log_string("Loading limits and rosters from players.");
      }
#if !defined(MSDOS)
      sprintf(catplr, "%s%s", dir, "*.plr");
      sprintf(pbuf, "ls %s > %s", catplr, list);
      system(pbuf);
#endif

      if
      (
         (
            fpChar_list = fopen(list, "r")
         ) == NULL
      )
      {
         perror(list);
         exit(1);
      }
      if (!storage)
      {
         if
         (
            (
               masterconv = fopen("../log/conv.lst", "w")
            ) == NULL
         )
         {
            perror("\"../log/conv.lst\", \"w\"");
            exit(1);
         }
         fprintf(masterconv, "Conversation Log List\n\r");
      }

      for (; ;)
      {
         strcpy
         (
            strPlr,
            (word = fread_word(fpChar_list))
         );
         free_string(word);
         /* Update time for boot log */
         update_time();
         log_string(strPlr);
         sprintf(chkbuf, "%s%s", dir, "Zzz.plr");

#if defined(MSDOS)
         if (!str_cmp(strPlr, "Zzz.plr"))
         {
            /* Exit if == Zzz.plr file */
            break;
         }
#else
         if (!str_cmp(strPlr, chkbuf))
         {
            /* Exit if == Zzz.plr file */
            break;
         }
#endif

         if
         (
            (
               fpChar = fopen(strPlr, "r")
            ) == NULL
         )
         {
            perror(strPlr);
            exit(1);
         }
         login_time = 0;
         house = 0;
         clan = 0;
         brand = -1;
         prob = 0;
         probtime = 0;
         ancient = FALSE;
         autopurge = FALSE;
         denied = FALSE;
         free_string(moniker);
         moniker = NULL;
         room = 0;
         race = -1;
         race_lottery = -1;
         moniker_sql[0] = '\0';
         free_string(name);
         name = NULL;
         level = 0;
         trust = 0;
         email = NULL;
         worship = -1;
         letter = 10;
         worship_slot = -1000;
         branding = FALSE;
         version = 0;
         free_string(password);
         password = NULL;
         free_string(password_old);
         password_old = NULL;

         for (; ;)
         {
            while (letter != 10)
            {
               letter = getc(fpChar);
            }
            letter = getc(fpChar);
            if (letter == 10)
            {
               continue;
            }
            switch (letter)
            {
               default:
               {
                  continue;
               }
               case ('#'):
               {
                  break;
               }
               case ('A'):
               {
                  word = fread_word(fpChar);
                  if
                  (
                     !storage &&
                     !strcmp(word, "ct2")
                  )
                  {
                     free_string(word);
                     word = fread_word(fpChar);
                     for (p = word;  *p; p++)
                     {
                        switch (*p)
                        {
                           case ('H'):
                           {
                              /* logspec */
                              fprintf(masterconv, "%s\n\r", strPlr);
                              break;
                           }
                           case ('R'):
                           {
                              /* Actively branding mortals */
                              branding = TRUE;
                              break;
                           }
                           case ('J'):
                           case ('L'):
                           {
                              /* j=ancient l=known ancient */
                              ancient = TRUE;
                              break;
                           }
                           default:
                           {
                              break;
                           }
                        }
                     }
                     /* Assume H is the conversation log bit */
                     if (*p == 'H')
                     {
                        fprintf(masterconv, "%s\n\r", strPlr);
                     }
                  }
                  else if (!strcmp(word, "ct"))
                  {
                     free_string(word);
                     word = fread_word(fpChar);
                     for
                     (
                        p = word;
                        (
                           (*p != 0) &&
                           (*p != 'X')
                        );
                        p++
                     )
                     {
                     }
                     /* Assume H is the conversation log bit */
                     if (*p == 'X')
                     {
                        denied = TRUE;
                     }
                  }
                  else if (!strcmp(word, "utoPurge"))
                  {
                     autopurge = fread_number(fpChar);
                  }
                  free_string(word);
                  continue;
               }
               case ('C'):
               {
                  word = fread_word(fpChar);
                  if (!strcmp(word, "lan"))
                  {
                     clan = fread_number(fpChar);
                     clan_name = fread_string(fpChar);
                     if
                     (
                        !clan_table[clan].valid ||
                        strcmp(clan_name, clan_table[clan].name)
                     )
                     {
                        clan = 0;
                     }
                     free_string(clan_name);
                  }
                  else if (!strcmp(word, "Deposits"))
                  {
                     int c_dep;

                     c_dep = fread_number(fpChar);
                     deposit = (DEPOSIT_TYPE*)malloc(sizeof(DEPOSIT_TYPE));
                     if
                     (
                        autopurge ||
                        (
                           !ancient &&
                           house != HOUSE_ANCIENT
                        )
                     )
                     {
                        deposit->name = str_dup(name);
                     }
                     else
                     {
                        if (moniker)
                        {
                           deposit->name = str_dup(moniker);
                        }
                        else
                        {
                           sprintf
                           (
                              log_buf,
                              "Ancient [%s] has no moniker, (deposits logged)",
                              name
                           );
                           bug(log_buf, 0);
                           deposit->name = str_dup("a cloaked figure");
                        }
                     }
                     deposit->amount = fread_number(fpChar);
                     deposit->life_amount = fread_number(fpChar);
                     deposit->login_time = login_time;
                     if
                     (
                        c_dep > 0 &&
                        c_dep < MAX_CLAN
                     )
                     {
                        add_node(deposit, clan_deposits[c_dep]);
                     }
                     else
                     {
                        free(deposit);
                     }
                  }
                  free_string(word);
                  continue;
               }
               case ('E'):
               {
                  word = fread_word(fpChar);
                  if (!strcmp(word, "mail"))
                  {
                     email = fread_string(fpChar);
                  }
                  free_string(word);
                  continue;
               }
               case ('H'):
               {
                  word = fread_word(fpChar);
                  if (!strcmp(word, "ous"))
                  {
                     free_string(word);
                     house = house_lookup(word = fread_string(fpChar));
                  }
                  else if (!strcmp(word, "Deposits"))
                  {
                     int h_dep;

                     free_string(word);
                     h_dep = house_lookup(word = fread_word(fpChar));
                     deposit = (DEPOSIT_TYPE*)malloc(sizeof(DEPOSIT_TYPE));
                     if
                     (
                        autopurge ||
                        (
                           !ancient &&
                           house != HOUSE_ANCIENT
                        )
                     )
                     {
                        deposit->name = str_dup(name);
                     }
                     else
                     {
                        if (moniker)
                        {
                           deposit->name = str_dup(moniker);
                        }
                        else
                        {
                           sprintf
                           (
                              log_buf,
                              "Ancient [%s] has no moniker, (deposits logged)",
                              name
                           );
                           bug(log_buf, 0);
                           deposit->name = str_dup("a cloaked figure");
                        }
                     }
                     deposit->amount = fread_number(fpChar);
                     deposit->life_amount = fread_number(fpChar);
                     deposit->login_time = login_time;
                     if
                     (
                        h_dep > 0 &&
                        h_dep < MAX_HOUSE
                     )
                     {
                        add_node(deposit, house_deposits[h_dep]);
                     }
                     else
                     {
                        free(deposit);
                     }
                  }
                  free_string(word);
                  continue;
               }
               case ('L'):
               {
                  word = fread_word(fpChar);
                  if (!strcmp(word, "evl"))
                  {
                     if (level)
                     {
                        fread_number(fpChar);
                        /* Don't count mobs as heroes */
                     }
                     else
                     {
                        level = fread_number(fpChar);
                        if
                        (
                           !storage &&
                           level == LEVEL_HERO
                        )
                        {
                           hero_count++;
                        }
                     }
                  }
                  if (!strcmp(word, "ogO"))
                  {
                     login_time = fread_number(fpChar);
                  }
                  free_string(word);
                  continue;
               }
               case ('N'):
               {
                  if (name != NULL)
                  {
                     continue;
                  }
                  word = fread_word(fpChar);
                  if (!strcmp(word, "ame"))
                  {
                     name = fread_string(fpChar);
                  }
                  free_string(word);
                  continue;
               }
               case ('M'):
               {
                  word = fread_word(fpChar);
                  if (!strcmp(word, "oniker"))
                  {
                     moniker = fread_string(fpChar);
                     escape_quote(moniker_sql, moniker);
                  }
                  free_string(word);
                  continue;
               }
               case ('T'):
               {
                  word = fread_word(fpChar);
                  if (!strcmp(word, "ru"))
                  {
                     trust = fread_number(fpChar);
                  }
                  free_string(word);
                  continue;
               }
               case ('P'):
               {
                  word = fread_word(fpChar);
                  if (!strcmp(word, "rob"))
                  {
                     prob = fread_number(fpChar);
                  }
                  else if (!strcmp(word, "robTime"))
                  {
                     probtime = fread_number(fpChar);
                  }
                  else if (!strcmp(word, "assMD5"))
                  {
                     password = fread_string(fpChar);
                  }
                  else if (!strcmp(word, "ass"))
                  {
                     password_old = fread_string(fpChar);
                  }
                  free_string(word);
                  continue;
               }
               case ('R'):
               {
                  word = fread_word(fpChar);
                  if (!strcmp(word, "ace"))
                  {
                     free_string(word);
                     word = fread_string(fpChar);
                     race = race_lookup(word);
                     if
                     (
                        race <= 0 ||
                        race >= MAX_PC_RACE
                     )
                     {
                        race = -1;
                     }
                  }
                  else if (!strcmp(word, "aceLottery"))
                  {
                     free_string(word);
                     word = fread_string(fpChar);
                     race_lottery = race_lookup(word);
                     if
                     (
                        race_lottery <= 0 ||
                        race_lottery >= MAX_PC_RACE
                     )
                     {
                        race_lottery = -1;
                     }
                  }
                  else if (!strcmp(word, "oom"))
                  {
                     room = fread_number(fpChar);
                     if
                     (
                        (
                           room_index = get_room_index(room)
                        ) != NULL &&
                        IS_SET(room_index->extra_room_flags, ROOM_1212)
                     )
                     {
                        /*
                           Used for racial lottery, those in 1212 do not get to win
                        */
                        room = 1212;
                     }
                  }
                  free_string(word);
                  continue;
               }
               case ('V'):
               {
                  word = fread_word(fpChar);
                  if (!strcmp(word, "ers"))
                  {
                     version = fread_number(fpChar);
                  }
                  free_string(word);
                  continue;
               }
               case ('W'):
               {
                  word = fread_word(fpChar);
                  if (!strcmp(word, "orship"))
                  {
                     sh_int god;

                     if (worship == -1)
                     {
                        worship_name = fread_string(fpChar);
                        /* Check for brand */
                        for (god = 0; worship_table[god].name != NULL; god++)
                        {
                           if (!str_cmp(worship_name, worship_table[god].name))
                           {
                              break;
                           }
                        }
                        if (worship_table[god].name != NULL)
                        {
                           worship = god;
                        }
                        free_string(worship_name);
                     }
                  }
                  if (!strcmp(word, "Slot"))
                  {
                     worship_slot = fread_number(fpChar);
                  }
                  free_string(word);
                  continue;
               }
            }
            word = fread_word(fpChar);
            if (!str_cmp(word, "End"))
            {
               free_string(word);
               if (autopurge)
               {
                  house = 0;
               }
               else if
               (
                  house == 0 &&
                  ancient
               )
               {
                  house = HOUSE_ANCIENT;
               }
               if
               (
                  level < LEVEL_IMMORTAL &&
                  trust < LEVEL_IMMORTAL &&
                  house &&
                  house_closed(house, version)
               )
               {
                  house = 0;
               }
               add_roster
               (
                  name,
                  clan,
                  house,
                  brand,
                  worship,
                  prob,
                  probtime,
                  login_time,
                  storage,
                  denied
               );
               if
               (
                  level < LEVEL_IMMORTAL &&
                  trust < LEVEL_IMMORTAL
               )
               {
                  add_to_time_wipe_roster
                  (
                     name,
                     storage,
                     denied,
                     login_time,
                     strPlr
                  );
                  if
                  (
                     !denied &&
                     (
                        (
                           house &&
                           !storage
                        ) ||
                        level >= 20
                     )
                  )
                  {
                     if
                     (
                        (
                           password &&
                           password[0] != '\0'
                        ) ||
                        (
                           password_old &&
                           password_old[0] != '\0'
                        )
                     )
                     {
                        fprintf
                        (
                           sql,
                           "replace into forum_accounts (username, password)"
                           " values('%s','%s');\n",
                           name,
                           (
                              (
                                 password &&
                                 password[0] != '\0'
                              ) ?
                              password :
                              password_old
                           )
                        );
                     }
                     if
                     (
                        house &&
                        !storage &&
                        sql_house(house) != -1
                     )
                     {
                        fprintf
                        (
                           sql,
                           "insert into access(username, forum_id) values"
                           "('%s', '%d');\n",
                           name,
                           sql_house(house)
                        );
                        if (house == HOUSE_ANCIENT)
                        {
                           fprintf
                           (
                              sql,
                              "update forum_accounts set alias='%s' where"
                              " username='%s';\n",
                              (
                                 moniker_sql[0] != '\0' ?
                                 moniker_sql :
                                 "Cloaked Figure"
                              ),
                              name
                           );
                        }
                     }
                  }
               }
               else
               {
                  table = find_wizi_slot(name, &worship_slot);
                  table->wiz_info.level = level;
                  table->wiz_info.trust = trust ? trust : level;
                  table->wiz_info.stored = storage;
                  if (email)
                  {
                     free_string(table->wiz_info.email);
                     table->wiz_info.email = email;
                  }
                  if
                  (
                     /* Minimum branding level */
                     table->wiz_info.level >= IMMORTAL &&
                     branding &&
                     /* Only if have a brand */
                     table->vnum_brand != BRAND_NONE &&
                     /* Only if the brand exists */
                     get_obj_index(table->vnum_brand) != NULL
                  )
                  {
                     table->branding = TRUE;
                  }
                  else
                  {
                     table->branding = FALSE;
                  }
                  table->wiz_info.login = login_time;
               }
               /* Race Lottery count players */
               if
               (
                  race > 0 &&
                  race < MAX_PC_RACE &&
                  level >= 10 &&
                  level < LEVEL_IMMORTAL &&
                  trust < LEVEL_IMMORTAL
               )
               {
                  lottery_race_count[race]++;
               }
               if (race_lottery == -1)
               {
                  race_lottery = race;
               }
               /* Race Lottery */
               if
               (
                  race > 0 &&
                  race < MAX_PC_RACE &&
                  race_lottery > 0 &&
                  race_lottery < MAX_PC_RACE &&
                  level >= 30 &&
                  level < LEVEL_IMMORTAL &&
                  trust < LEVEL_IMMORTAL &&
                  !storage &&
                  house == 0 &&
                  /* Logged in within past 30 days */
                  (current_time - login_time) < 30 * 24 * 60 * 60 &&
                  name != NULL
               )
               {
                  remove_node_for(name, lottery_race_players[race_lottery]);
                  add_node(name, lottery_race_players[race_lottery]);
               }
               break;
            }

            if
            (
               !str_cmp(word, "O") ||
               !str_cmp(word, "OBJECT")
            )
            {
               free_string(word);
               word = fread_word(fpChar);
               if (!str_cmp(word, "Vnum"))
               {
                  vnum = fread_number(fpChar);
                  if
                  (
                     (
                        pObjIndex = get_obj_index(vnum)
                     ) == NULL
                  )
                  {
                     bug("Bad obj vnum in limits: %d", vnum);
                  }
                  else
                  {
                     sh_int god;

                     if (brand == -1)
                     {
                        /* Check for brand */
                        for (god = 0; worship_table[god].name != NULL; god++)
                        {
                           if (worship_table[god].vnum_brand == vnum)
                           {
                              break;
                           }
                        }
                        if (worship_table[god].name != NULL)
                        {
                           brand = god;
                        }
                     }
                     if (!storage)
                     {
                        pObjIndex->limcount++;
                     }
                  }
               }
            }
            free_string(word);
         }
         fclose(fpChar);
         fpChar = NULL;
      }
      fclose(fpChar_list);
      if (!storage)
      {
         break;
      }
      dir = PLAYER_DIR;
      list = PLAYER_LIST;
      storage = FALSE;
   }
   free_string(moniker);
   moniker = NULL;
   /* Global for this file, make sure to set to NULL when not in use */
   fpChar = NULL;

   fclose(masterconv);
   fprintf
   (
      sql,
      "replace into forum_accounts (username, password, alias, email, url, "
      "notes, admin_ind, last_login, gmt_offset, lock_ind, priv_ind, "
      "delete_ind, location, last_access) select username, password, alias, "
      "email, url, notes, admin_ind, last_login, gmt_offset, lock_ind, "
      "priv_ind, delete_ind, location, last_access from old_forum_accounts;\n"
      "delete from old_forum_accounts;\n"
   );
   fclose(sql);
   rename(TEMP_FILE, SQL_FILE);
}

void boot_limits()
{
   OBJ_INDEX_DATA* obj;
   AREA_DATA* parea;
   int count = 0;
   bool changed;

   log_string("Fixing Limits on hero count based items.");
   for (parea = area_first; parea; parea = parea->next)
   {
      for (obj = parea->objects; obj; obj = obj->next_in_area)
      {
         if (obj->limtotal >= 0)
         {
            continue;
         }
         changed = TRUE;
         obj->limtotal =
         (
            (float)obj->limtotal / (-200.0)
         ) * hero_count;
         if (obj->limtotal <= obj->limcount)
         {
            obj->limtotal = obj->limcount;
            changed = FALSE;
         }
         if (obj->limtotal < 1)
         {
            obj->limtotal = 1;
         }
         else if (changed)
         {
            count++;
         }
      }
   }
   update_time();
   sprintf(log_buf, "Done fixing limits: %d modified", count);
   log_string(log_buf);
}

void boot_time_weather()
{
   if (current_mud_time.hour <  5)
   {
      weather_info.sunlight = SUN_DARK;
   }
   else if (current_mud_time.hour == 5)
   {
      weather_info.sunlight = SUN_RISE;
   }
   else if (current_mud_time.hour < 19)
   {
      weather_info.sunlight = SUN_LIGHT;
   }
   else if (current_mud_time.hour < 20)
   {
      weather_info.sunlight = SUN_SET;
   }
   else
   {
      weather_info.sunlight = SUN_DARK;
   }

   weather_info.change = 0;
   weather_info.mmhg = 960;
   if
   (
      current_mud_time.month >= 7 &&
      current_mud_time.month <= 12
   )
   {
      weather_info.mmhg += number_range(1, 50);
   }
   else
   {
      weather_info.mmhg += number_range(1, 80);
   }

   if (weather_info.mmhg <=  980)
   {
      weather_info.sky = SKY_LIGHTNING;
   }
   else if (weather_info.mmhg <= 1000)
   {
      weather_info.sky = SKY_RAINING;
   }
   else if (weather_info.mmhg <= 1020)
   {
      weather_info.sky = SKY_CLOUDY;
   }
   else
   {
      weather_info.sky = SKY_CLOUDLESS;
   }
}

void boot_dopplegangers()
{
   CHAR_DATA* ch;
   CHAR_DATA* gch;
   char buf[MAX_STRING_LENGTH];
   ROOM_INDEX_DATA* pRoomIndex;
   AREA_DATA* parea;
   AFFECT_DATA af;
   int val;
   int doppel;
   int total;
   int count;
   int choice;

   if (get_mob_index(MOB_VNUM_DOPPLEGANGER) != NULL)
   {
      total = 0;
      for (parea = area_first; parea; parea = parea->next)
      {
         for
         (
            pRoomIndex = parea->rooms;
            pRoomIndex;
            pRoomIndex = pRoomIndex->next_in_area
         )
         {
            if
            (
               !IS_SET(pRoomIndex->extra_room_flags, ROOM_1212) &&
               pRoomIndex->guild == 0 &&
               !IS_SET(pRoomIndex->room_flags, ROOM_PRIVATE) &&
               !IS_SET(pRoomIndex->room_flags, ROOM_SOLITARY) &&
               !IS_SET(pRoomIndex->room_flags, ROOM_SAFE) &&
               !IS_SET(pRoomIndex->room_flags, ROOM_IMP_ONLY) &&
               !IS_SET(pRoomIndex->room_flags, ROOM_GODS_ONLY) &&
               !IS_SET(pRoomIndex->room_flags, ROOM_NEWBIES_ONLY) &&
               !IS_SET(pRoomIndex->room_flags, ROOM_LOW_ONLY) &&
               pRoomIndex->people != NULL &&
               IS_NPC(pRoomIndex->people)
            )
            {
               total++;
            }
         }
      }
      for (doppel = 0; doppel < 20; doppel++)
      {
         choice = number_range(1, total);
         count = 0;
         pRoomIndex = NULL;
         for (parea = area_first; parea; parea = parea->next)
         {
            for
            (
               pRoomIndex = parea->rooms;
               pRoomIndex;
               pRoomIndex = pRoomIndex->next_in_area
            )
            {
               if
               (
                  !IS_SET(pRoomIndex->extra_room_flags, ROOM_1212) &&
                  pRoomIndex->guild == 0 &&
                  !IS_SET(pRoomIndex->room_flags, ROOM_PRIVATE) &&
                  !IS_SET(pRoomIndex->room_flags, ROOM_SOLITARY) &&
                  !IS_SET(pRoomIndex->room_flags, ROOM_SAFE) &&
                  !IS_SET(pRoomIndex->room_flags, ROOM_IMP_ONLY) &&
                  !IS_SET(pRoomIndex->room_flags, ROOM_GODS_ONLY) &&
                  !IS_SET(pRoomIndex->room_flags, ROOM_NEWBIES_ONLY) &&
                  !IS_SET(pRoomIndex->room_flags, ROOM_LOW_ONLY) &&
                  pRoomIndex->people != NULL &&
                  IS_NPC(pRoomIndex->people) &&
                  ++count == choice
               )
               {
                  break;
               }
            }
            if (pRoomIndex)
            {
               break;
            }
         }
         if (pRoomIndex == NULL)
         {
            bug("Failed to load doppelganger #%d", doppel + 1);
            break;
         }

         gch = pRoomIndex->people;
         if
         (
            IS_NPC(gch) &&
            !(IS_AFFECTED(gch, AFF_WIZI))
         )
         {
            ch = create_mobile(get_mob_index(MOB_VNUM_DOPPLEGANGER));
            char_to_room(ch, pRoomIndex);
            act
            (
               "$n steps through a black gate and takes on a new form.",
               ch,
               NULL,
               NULL,
               TO_ROOM
            );
            send_to_char
            (
               "You step through a black gate and shift your form!\n\r",
               ch
            );
            sprintf(buf, "doppelganger %s", gch->name);
            free_string(ch->name);
            ch->name = str_dup(buf);
            free_string(ch->short_descr);
            ch->short_descr = str_dup(gch->short_descr);
            free_string(ch->long_descr);
            ch->long_descr = str_dup(gch->long_descr);
            free_string(ch->description);
            ch->description  = str_dup(gch->description);
            ch->sex          = gch->sex;
            ch->level        = gch->level;
            ch->trust        = 0;
            ch->hit          = gch->hit;
            ch->max_hit      = gch->max_hit;
            ch->mana         = gch->mana;
            ch->max_mana     = gch->max_mana;
            ch->move         = gch->move;
            ch->max_move     = gch->max_move;
            ch->exp          = gch->exp;
            ch->affected_by  = gch->affected_by;
            ch->spell_power  = gch->spell_power;
            ch->holy_power  = gch->holy_power;
            ch->sight        = gch->sight;
            ch->saving_throw = gch->saving_throw;
            ch->saving_maledict  = gch->saving_maledict;
            ch->saving_breath    = gch->saving_breath;
            ch->saving_transport = gch->saving_transport;
            ch->saving_spell = gch->saving_spell;
            ch->hitroll      = gch->hitroll;
            ch->damroll      = gch->damroll;
            ch->form         = gch->form;
            ch->parts        = gch->parts;
            ch->size         = gch->size;
            for (val = 0; val < 4; val++)
            {
               ch->armor[val] = gch->armor[val];
            }
            for (val = 0; val < MAX_STATS; val++)
            {
               ch->perm_stat[val] = gch->perm_stat[val];
               ch->mod_stat[val]  = gch->mod_stat[val];
            }
            for (val = 0; val < 3; val++)
            {
               ch->damage[val] = gch->damage[val];
            }
            if
            (
               IS_AFFECTED(ch, AFF_SANCTUARY) &&
               !is_affected(ch, gsn_sanctuary) &&
               !is_affected(ch, gsn_chromatic_shield)
            )
            {
               af.where     = TO_AFFECTS;
               af.level     = ch->level;
               af.duration  = -1;
               af.location  = APPLY_NONE;
               af.modifier  = 0;
               af.bitvector = AFF_SANCTUARY;
               if (is_clergy(ch))
               {
                  af.type = gsn_sanctuary;
               }
               else
               {
                  af.type = gsn_chromatic_shield;
               }
               affect_to_char(ch, &af);
            }
            act
            (
               "$n looks around for prey.",
               ch,
               NULL,
               NULL,
               TO_ROOM
            );
            send_to_char("You look around for prey.\n\r", ch);
         }
      }
      update_time();
      log_string("Doppelgangers loaded");
   }
   else
   {
      bug("Doppelgangers not loaded.", 0);
   }
}

void boot_positions()
{
   MOB_INDEX_DATA* index;
   AREA_DATA* parea;
   bool found = FALSE;

   log_string("Checking for stunned or dead mobs.");
   for (parea = area_first; parea; parea = parea->next)
   {
      for (index = parea->mobs; index; index = index->next_in_area)
      {
         if
         (
            index->start_pos >= POS_SLEEPING &&
            index->default_pos >= POS_SLEEPING
         )
         {
            continue;
         }
         found = TRUE;
         sprintf
         (
            log_buf,
            "\n"
            "Mob stunned or dead!  Vnum [%d] [%s] [%s]\n"
            "   Start: [%d] [%s]  Default [%d] [%s]\n",
            index->vnum,
            index->player_name,
            index->short_descr,
            index->start_pos,
            (
               (
                  index->start_pos >= 0 &&
                  position_table[index->start_pos].name != NULL
               ) ?
               position_table[index->start_pos].name :
               "NULL"
            ),
            index->default_pos,
            (
               (
                  index->default_pos >= 0 &&
                  position_table[index->default_pos].name != NULL
               ) ?
               position_table[index->default_pos].name :
               "NULL"
            )
         );
         bug(log_buf, 0);
      }
   }
   if (found)
   {
      exit(1);
   }
}

/*
   Assign gsn's for skills which have them.
   Include 0 (reserved)
*/
void boot_gsns()
{
   sh_int sn;

   for (sn = 0; sn < MAX_SKILL; sn++)
   {
      if
      (
         (skill_table[sn].pgsn != NULL)
      )
      {
         *skill_table[sn].pgsn = sn;
      }
      if
      (
         skill_table[sn].spell_fun == NULL
      )
      {
         sprintf
         (
            log_buf,
            "Skill [%s] [%d] has NULL for spell_fun.  Should be a spell_function, or spell_null!",
            skill_table[sn].name,
            sn
         );
         bug(log_buf, 0);
         exit(1);
      }
   }
}

/*
   Assign grn's for races
   Include 0 (reserved)
*/
void boot_grns()
{
   sh_int rn;

   for (rn = 0; race_table[rn].name; rn++)
   {
      if (race_table[rn].pgrn != NULL)
      {
         *race_table[rn].pgrn = rn;
      }
      else
      {
         sprintf
         (
            log_buf,
            "Race [%s] [%d] has NULL for Global Race Number!",
            race_table[rn].name,
            rn
         );
         bug(log_buf, 0);
         exit(1);
      }
   }
}

/*
   Check stability of skill table
   If skill A's name is a prefix of skill B's name,
   skill A MUST be before skill B in the skill table
*/
void boot_skills()
{
   int cnt;
   int sn;

   for (cnt = 1; cnt < MAX_SKILL; cnt++)
   {
      if (skill_table[cnt].name == NULL)
      {
         bug("Skill table entry [%d] has a NULL name!", cnt);
         exit(1);
      }
      if
      (
         UPPER(skill_table[cnt].name[0]) == UPPER(skill_table[0].name[0]) &&
         !str_prefix(skill_table[cnt].name, skill_table[0].name)
      )
      {
         sprintf
         (
            log_buf,
            "Skill table entry [%d] [%s] starts with reserved keyword [%s]!",
            cnt,
            skill_table[cnt].name,
            skill_table[0].name
         );
         bug(log_buf, 0);
         exit(1);
      }
      for (sn = cnt + 1; sn < MAX_SKILL; sn++)
      {
         if
         (
            UPPER(skill_table[sn].name[0]) == UPPER(skill_table[cnt].name[0])  &&
            !str_prefix(skill_table[sn].name, skill_table[cnt].name)
         )
         {
            sprintf
            (
               log_buf,
               "Skill table entry [%d] [%s] must be moved to before [%d] [%s]!",
               sn,
               skill_table[sn].name,
               cnt,
               skill_table[cnt].name
            );
            bug(log_buf, 0);
            exit(1);
         }
      }
   }
}

SORT_TYPE** sorted_skill_table;

int compare_sort_type(const void* input1, const void* input2)
{
   /*
      Comparator function for sort type
      sorts alphabetically
   */
   SORT_TYPE* sort1;
   SORT_TYPE* sort2;

   sort1 = *(SORT_TYPE**)input1;
   sort2 = *(SORT_TYPE**)input2;
   return str_cmp(sort1->name, sort2->name);
}

void boot_tables()
{
   sh_int cnt;

   /* init death affects */
   if
   (
      (
         death_affects = (AFFECT_DATA**) malloc
         (
            MAX_SKILL * sizeof(AFFECT_DATA*)
         )
      ) == NULL
   )
   {
      bug("Boot_db: can't alloc %d death affects space.", MAX_SKILL);
      exit(1);
   }
   sorted_skill_table = (SORT_TYPE**)malloc(MAX_SKILL * sizeof(SORT_TYPE*));
   for (cnt = 0; cnt < MAX_SKILL; cnt++)
   {
      sorted_skill_table[cnt] = (SORT_TYPE*)malloc(sizeof(SORT_TYPE));
      sorted_skill_table[cnt]->name = skill_table[cnt].name;
      sorted_skill_table[cnt]->index = cnt;
   }
   qsort(sorted_skill_table, MAX_SKILL, sizeof (SORT_TYPE*), compare_sort_type);
   for (cnt = 0; cnt < MAX_SKILL; cnt++)
   {
      death_affects[cnt] = NULL;
   }
   /* Init rosters */
   for (cnt = 0; cnt < MAX_HOUSE; cnt++)
   {
      house_table[cnt].proster = NULL;
   }
   for (cnt = 0; cnt < MAX_CLAN; cnt++)
   {
      clan_table[cnt].proster = NULL;
   }
   for (cnt = 0; worship_table[cnt].name != NULL; cnt++)
   {
      worship_table[cnt].proster = NULL;
      worship_table[cnt].pwroster = NULL;
   }
   /* Set mem_counts to 0 -Fizzfaldt */
   for
   (
      cnt = 0;
      cnt < (sh_int)(sizeof(mem_counts) / sizeof(mem_counts[0]));
      cnt++
   )
   {
      mem_counts[cnt] = 0;
   }
   value_1_list = new_list();
   book_race_list = new_list();
   for (cnt = 0; cnt < MAX_PC_RACE; cnt++)
   {
      lottery_race_count[cnt] = 0;
      lottery_race_players[cnt] = new_string_list();
      lottery_race_last_run[cnt] = 0;
      lottery_race_reset = 0;
   }
}

int compare_deposit_type(const void* input1, const void* input2)
{
   /*
      Comparator function for sort type
      deposits alphabetically
   */
   DEPOSIT_TYPE* deposit1;
   DEPOSIT_TYPE* deposit2;

   deposit1 = *(DEPOSIT_TYPE**)input1;
   deposit2 = *(DEPOSIT_TYPE**)input2;
   if (deposit1->amount > deposit2->amount)
   {
      return -1;
   }
   if (deposit1->amount < deposit2->amount)
   {
      return 1;
   }
   return str_cmp(deposit1->name, deposit2->name);
}

char* global_bank_message(DEPOSIT_TYPE** array, int size, char* account_type, unsigned max_name, bool kicked)
{
   char buf[MAX_STRING_LENGTH];
   BUFFER* output = new_buf();
   char* retval;
   char format[MAX_INPUT_LENGTH];
   int cnt;
   DEPOSIT_TYPE* deposit;

   if (kicked)
   {
      sprintf
      (
         buf,
         "The following members no longer belong to your %s.\n\r\n\r",
         account_type
      );
   }
   else
   {
      sprintf
      (
         buf,
         "Your %s account activity for the last period:\n\r\n\r",
         account_type
      );
   }
   add_buf(output, buf);
   sprintf
   (
      format,
      "%%-%us  %%s %%10d gold coin%%s Lifetime %%s %%10d gold coin%%s\n\r",
      max_name
   );
   for (cnt = 0; cnt < size; cnt++)
   {
      deposit = array[cnt];
      sprintf
      (
         buf,
         format,
         /*
         "%-15s  %s %10d gold coin%s Lifetime %s %10d gold coin%s\n\r",
         */
         deposit->name,
         (
            deposit->amount < 0 ?
            "withdrew " :
            "deposited"
         ),
         (
            deposit->amount < 0 ?
            -deposit->amount :
            deposit->amount
         ),
         (
            deposit->amount == 1 ?
            ", " :
            "s,"
         ),
         (
            deposit->life_amount < 0 ?
            "withdrew " :
            "deposited"
         ),
         (
            deposit->life_amount < 0 ?
            -deposit->life_amount :
            deposit->life_amount
         ),
         (
            deposit->life_amount == 1 ?
            "" :
            "s"
         )
      );
      add_buf(output, buf);
   }
   if (kicked)
   {
      add_buf
      (
         output,
         "\n\r"
         "Happy banking in the future for the rest of you.\n\r"
      );
   }
   else
   {
      add_buf
      (
         output,
         "\n\r"
         "This ends your periodical statement.\n\r"
      );
   }
   retval = buf_string(output);
   free_buf(output);
   return retval;
}

void global_bank_notes()
{
   int house;
   int clan;
   int cnt;
   DEPOSIT_TYPE** array;
   NODE_DATA* node;
   DEPOSIT_TYPE* deposit;
   int max = 0;
   int size;
   int notes_sent = 0;
   unsigned max_name;

   update_time();
   log_string("Global Banknotes");
   for (cnt = 0; cnt < MAX_HOUSE; cnt++)
   {
      if (house_deposits[cnt]->size > max)
      {
         max = house_deposits[cnt]->size;
      }
   }
   for (cnt = 0; cnt < MAX_CLAN; cnt++)
   {
      if (clan_deposits[cnt]->size > max)
      {
         max = clan_deposits[cnt]->size;
      }
   }
   if (max > 0)
   {
      /* All the note crap */
      /* Make array */
      array = (DEPOSIT_TYPE**)malloc(max * sizeof(DEPOSIT_TYPE*));
      for (house = 0; house < MAX_HOUSE; house++)
      {
         /* Set up array */
         size = 0;
         max_name = 0;
         if
         (
            house > 0 &&
            house_table[house].name &&
            house_table[house].name[0] != '\0'
         )
         {
            for (node = house_deposits[house]->first; node; node = node->next)
            {
               deposit = (DEPOSIT_TYPE*)node->data;
               if
               (
                  deposit->login_time >= last_banknote_update &&
                  deposit->amount
               )
               {
                  /*
                     Only if deposited something since last
                     note update.  Only include people active (balance-wise)
                  */
                  array[size++] = deposit;
                  if (strlen(deposit->name) > max_name)
                  {
                     max_name = strlen(deposit->name);
                  }
               }
            }
         }
         if (size)
         {
            notes_sent++;
            qsort(array, size, sizeof (DEPOSIT_TYPE*), compare_deposit_type);
            make_note
            (
               "Financial Advisor",
               "Periodic House Statement",
               capitalize(house_table[house].name),
               global_bank_message(array, size, "House", max_name, FALSE),
               NOTE_BANK
            );
         }
         /* free deposits in this house */
         for (node = house_deposits[house]->first; node; node = node->next)
         {
            deposit = (DEPOSIT_TYPE*)node->data;
            free_string(deposit->name);
            free(deposit);
         }
      }
      for (clan = 0; clan < MAX_CLAN; clan++)
      {
         /* Set up array */
         size = 0;
         max_name = 0;
         if
         (
            clan > 0 &&
            clan_table[clan].name &&
            clan_table[clan].name[0] != '\0' &&
            clan_table[clan].who_name &&
            clan_table[clan].who_name[0] != '\0' &&
            clan_table[clan].valid
         )
         {
            for (node = clan_deposits[clan]->first; node; node = node->next)
            {
               deposit = (DEPOSIT_TYPE*)node->data;
               if
               (
                  deposit->login_time >= last_banknote_update &&
                  deposit->amount
               )
               {
                  /*
                     Only if deposited something since last
                     note update.  Only include people active (balance-wise)
                  */
                  array[size++] = deposit;
                  if (strlen(deposit->name) > max_name)
                  {
                     max_name = strlen(deposit->name);
                  }
               }
            }
         }
         if (size)
         {
            notes_sent++;
            qsort(array, size, sizeof (DEPOSIT_TYPE*), compare_deposit_type);
            make_note
            (
               "Financial Advisor",
               "Periodic Clan Statement",
               clan_table[clan].who_name,
               global_bank_message(array, size, "Clan", max_name, FALSE),
               NOTE_BANK
            );
         }
         /* free deposits in this clan */
         for (node = clan_deposits[clan]->first; node; node = node->next)
         {
            deposit = (DEPOSIT_TYPE*)node->data;
            free_string(deposit->name);
            free(deposit);
         }
      }
      /* Free array */
      free(array);
   }
   /* Free all the lists */
   for (cnt = 0; cnt < MAX_HOUSE; cnt++)
   {
      free_list(house_deposits[cnt]);
      house_deposits[cnt] = NULL;
   }
   for (cnt = 0; cnt < MAX_CLAN; cnt++)
   {
      free_list(clan_deposits[cnt]);
      clan_deposits[cnt] = NULL;
   }
   /* END */
   update_time();
   if (notes_sent > 0)
   {
      last_banknote_update = current_time;
   }
   save_globals();
   update_time();
   sprintf(log_buf, "Global Banknotes Sent: %d", notes_sent);
   log_string(log_buf);
}

/*
Big mama top level function.
*/
void boot_db(void)
{
   boot_tables();
   boot_restricts();
   immrp_blood = FALSE;
   immrp_darkness = FALSE;
   allow_rang = FALSE;
   allow_elem = TRUE;
   allow_thief = TRUE;
   frog_house = 0;
   /*
   turn off all resolving
   NOTE: This does absolutely nothing,
   as this variable is never checked. - Fizz
   */
   resolve_no_ip = TRUE;
   total_affects = 0;  /* affect counting for mem tracking */
   /*
   * Init some data space stuff.
   */
   if
   (
      (
         string_space = calloc(1, MAX_STRING)
      ) == NULL
   )
   {
      bug("Boot_db: can't alloc %d string space.", MAX_STRING);
      exit(1);
   }
   top_string    = string_space;
   fBootDb        = TRUE;
   strcpy(str_none, "none");

   /*
   Init random number generator.
   */
   init_mm();

   /* Initialize moons */
   moon_update();

   /*
   Set time and weather.
   */
   boot_time_weather();

   boot_gsns();

   boot_grns();

   boot_skills();

   /*
   Read in all the area files.
   */
   boot_areas();

   fBootDb  = FALSE;    /* Exit boot status before limit load */

   boot_positions();  /* Make sure all mobs load sleeping or better */

   /*
   Logged inside, must be done before players
   for clan roster
   */
   load_clans();
   /* Load object counts off players now */
   boot_players();

   /* assign hero count based limits */
   boot_limits();

   /*
   * Fix up exits.
   * Declare db booting over.
   * Reset all areas once.
   * Load up the songs, notes, ban, and nameban files.
   */
   update_time();
   log_string("Fixing Exits");
   fix_exits();
   update_time();
   log_string("Fixing Mobprogs");
   fix_mobprogs( );
   fBootDb = FALSE;
   update_time();
   log_string("Area update starting");
   area_update();
   update_time();
   log_string("Loading Notes");
   load_notes();
   update_time();
   log_string("Loading Bans");
   load_bans();
   update_time();
   log_string("Loading Namebans");
   load_namebans();
   update_time();
   log_string("Loading Code reminders");
   codeFileLoad();
   /* Randomize doppelgangers */
   update_time();
   boot_dopplegangers();

   /* load in globals */
   update_time();
   load_wizireport();
   load_globals();
   update_time();
   sort_all_rosters();
   dump_time_wipe_roster();
   global_bank_notes();

   return;
}



/*
* Snarf an 'area' header line.
*/
void load_area(FILE* fp)
{
   AREA_DATA* pArea;
   char level   [MAX_STRING_LENGTH_DO_AREA];
   char builder [MAX_STRING_LENGTH_DO_AREA];
   char title   [MAX_STRING_LENGTH_DO_AREA];
   char buf     [MAX_STRING_LENGTH];

   pArea = alloc_perm(sizeof(*pArea));
   pArea->reset_first = NULL;
   pArea->reset_last  = NULL;
   pArea->rooms       = NULL;
   pArea->objects     = NULL;
   pArea->mobs        = NULL;
   pArea->file_name   = str_dup(&strArea[strlen(area_dir)]);
   fread_string(fp);  /* Old filename stored here.. now unused */
   pArea->name        = fread_string(fp);
   pArea->credits     = fread_string(fp);
   /* Check Credits validity for do_areas */
   level[0] = '\0';
   builder[0] = '\0';
   title[0] = '\0';
   sscanf
   (
      pArea->credits,
      "{%[^}]} %s %[^ ]",
      level,
      builder,
      title
   );
   if
   (
      level[0] == '\0' ||
      builder[0] == '\0' ||
      title[0] == '\0'
   )
   {
      sprintf
      (
         buf,
         "Non Standard Area Header: [%s] [%s] level [%s] builder [%s] title [%s]",
         pArea->file_name,
         pArea->name,
         level,
         builder,
         title
      );
      bug(buf, 0);
   }
   pArea->min_vnum    = fread_number(fp);
   pArea->max_vnum    = fread_number(fp);
   if
   (
      pArea->min_vnum >= MAX_VNUM ||
      pArea->max_vnum >= MAX_VNUM ||
      pArea->min_vnum >  pArea->max_vnum ||
      pArea->min_vnum <= 0 ||
      pArea->max_vnum <= 0
   )
   {
      sprintf
      (
         log_buf,
         "load_area, vnums messed up: [%d] to [%d] in [%s]",
         pArea->min_vnum,
         pArea->max_vnum,
         pArea->name
      );
      bug(log_buf, 0);
      exit(1);
   }
   pArea->age         = 32;
   pArea->nplayer     = 0;
   pArea->empty       = TRUE;
   pArea->empty_for   = 0;
   pArea->music       = NULL;

   if (area_first == NULL)
   {
      area_first = pArea;
   }
   if (area_last  != NULL)
   {
      area_last->next = pArea;
   }
   area_last = pArea;
   pArea->next = NULL;

   top_area++;
   /* Make time output correct for bootup logs */
   update_time();
   log_string(pArea->file_name);
   return;
}

void load_area_music(FILE* fp)
{
   area_last->music = fread_string(fp);
   return;
}

/*
* Snarf a help section.
*/
void load_helps(FILE* fp)
{
   HELP_DATA* pHelp;

   for (; ;)
   {
      pHelp = alloc_perm(sizeof(*pHelp));
      pHelp->level = fread_number(fp);
      pHelp->keyword = fread_string(fp);
      if (pHelp->keyword[0] == '$')
      {
         break;
      }
      pHelp->text = fread_string(fp);

      if (!str_cmp(pHelp->keyword, "greeting"))
      {
         help_greeting = pHelp->text;
      }
      if (help_first == NULL)
      {
         help_first = pHelp;
      }
      if (help_last  != NULL)
      {
         help_last->next = pHelp;
      }
      help_last    = pHelp;
      pHelp->next    = NULL;
      top_help++;
   }
   return;
}



/*
* Snarf a mob section.  old style
*/
void load_old_mob(FILE* fp)
{
   MOB_INDEX_DATA* pMobIndex;
   /* for race updating */
   int race;
   char name[MAX_STRING_LENGTH];
   sh_int vnum;
   char letter;
   int iHash;

   for (; ;)
   {
      letter = fread_letter(fp);
      if (letter != '#')
      {
         bug("Load_old_mobiles: # not found.", 0);
         exit(1);
      }

      vnum = fread_number(fp);
      if (vnum == 0)
      {
         break;
      }

      fBootDb = FALSE;
      if (get_mob_index(vnum) != NULL)
      {
         bug("Load_old_mobiles: vnum %d duplicated.", vnum);
         exit(1);
      }
      fBootDb = TRUE;

      pMobIndex = alloc_perm(sizeof(*pMobIndex));
      pMobIndex->vnum        = vnum;
      pMobIndex->new_format  = FALSE;
      pMobIndex->player_name = fread_string(fp);
      pMobIndex->short_descr = fread_string(fp);
      pMobIndex->long_descr  = fread_string(fp);
      pMobIndex->description = fread_string(fp);
      pMobIndex->next_in_area = area_last->mobs;
      area_last->mobs = pMobIndex;
      pMobIndex->long_descr[0]  = UPPER(pMobIndex->long_descr[0]);
      pMobIndex->description[0] = UPPER(pMobIndex->description[0]);

      pMobIndex->act  = fread_flag(fp) | ACT_IS_NPC;
      pMobIndex->act2 = 0;
      pMobIndex->affected_by = fread_flag(fp);
      pMobIndex->pShop       = NULL;
      pMobIndex->alignment   = fread_number(fp);
      letter                 = fread_letter(fp);
      pMobIndex->level       = fread_number(fp);

      /*
      * The unused stuff is for imps who want to use the old-style
      * stats-in-files method.
      */
      /* START UNUSED */
      fread_number(fp);  /* Unused */
      fread_number(fp);  /* Unused */
      fread_number(fp);  /* Unused */
      /* 'd'        */
      fread_letter(fp);  /* Unused */
      fread_number(fp);  /* Unused */
      /* '+'        */
      fread_letter(fp);  /* Unused */
      fread_number(fp);  /* Unused */
      fread_number(fp);  /* Unused */
      /* 'd'        */
      fread_letter(fp);  /* Unused */
      fread_number(fp);  /* Unused */
      /* '+'        */
      fread_letter(fp);  /* Unused */
      fread_number(fp);  /* Unused */

      pMobIndex->wealth      = fread_number(fp) / 20;
      /* xp can't be used! */
      fread_number(fp);  /* Unused */
      pMobIndex->start_pos   = fread_number(fp);    /* Unused */
      pMobIndex->default_pos = fread_number(fp);    /* Unused */

      if (pMobIndex->start_pos < POS_SLEEPING)
      {
         pMobIndex->start_pos = POS_STANDING;
      }
      if (pMobIndex->default_pos < POS_SLEEPING)
      {
         pMobIndex->default_pos = POS_STANDING;
      }
      /* END UNUSED */

      /*
      * Back to meaningful values.
      */
      pMobIndex->sex = fread_number(fp);

      /* compute the race BS */
      one_argument(pMobIndex->player_name, name);

      if
      (
         name[0] == '\0' ||
         (
            race =  race_lookup(name)
         ) == 0
      )
      {
         /* fill in with blanks */
         pMobIndex->race = grn_human;
         pMobIndex->off_flags = OFF_DODGE | OFF_DISARM | OFF_TRIP | ASSIST_VNUM;
         pMobIndex->imm_flags = 0;
         pMobIndex->res_flags = 0;
         pMobIndex->vuln_flags = 0;
         pMobIndex->form =
         (
            FORM_EDIBLE |
            FORM_SENTIENT |
            FORM_BIPED |
            FORM_MAMMAL
         );
         pMobIndex->parts =
         (
            PART_HEAD |
            PART_ARMS |
            PART_LEGS |
            PART_HEART |
            PART_BRAINS |
            PART_GUTS
         );
      }
      else
      {
         pMobIndex->race = race;
         pMobIndex->off_flags =
         (
            OFF_DODGE |
            OFF_DISARM |
            OFF_TRIP |
            ASSIST_RACE |
            race_table[race].off
         );
         pMobIndex->imm_flags = race_table[race].imm;
         pMobIndex->res_flags = race_table[race].res;
         pMobIndex->vuln_flags = race_table[race].vuln;
         pMobIndex->form = race_table[race].form;
         pMobIndex->parts = race_table[race].parts;
      }

      if (letter != 'S')
      {
         bug("Load_mobiles: vnum %d non-S.", vnum);
         exit(1);
      }

      iHash = vnum % MAX_KEY_HASH;
      pMobIndex->next = mob_index_hash[iHash];
      mob_index_hash[iHash] = pMobIndex;
      top_mob_index++;
      kill_table[URANGE(0, pMobIndex->level, MAX_LEVEL-1)].number++;
   }
   return;
}

/*
* Snarf an obj section.  old style
*/
void load_old_obj(FILE* fp)
{
   OBJ_INDEX_DATA* pObjIndex;
   AFFECT_DATA* paf;
   EXTRA_DESCR_DATA* ed;
   MOUNT_DATA* specs;
   sh_int vnum;
   char letter;
   int iHash;

   for (; ;)
   {
      letter = fread_letter(fp);
      if (letter != '#')
      {
         bug("Load_old_objects: # not found.", 0);
         exit(1);
      }

      vnum = fread_number(fp);
      if (vnum == 0)
      {
         break;
      }

      fBootDb = FALSE;
      if (get_obj_index(vnum) != NULL)
      {
         bug("Load_objects: vnum %d duplicated.", vnum);
         exit(1);
      }
      fBootDb = TRUE;

      pObjIndex = alloc_perm(sizeof(*pObjIndex));
      pObjIndex->vnum = vnum;

      pObjIndex->next_in_area = area_last->objects;
      area_last->objects = pObjIndex;
      pObjIndex->new_format = FALSE;
      pObjIndex->reset_num  = 0;
      pObjIndex->name       = fread_string(fp);
      pObjIndex->short_descr = fread_string(fp);
      pObjIndex->description = fread_string(fp);
      /*
      Action description
      */
      /*
      fread_string(fp);
      commented to add materials on next line
      */
      pObjIndex->material       = fread_string(fp);
      pObjIndex->short_descr[0] = LOWER(pObjIndex->short_descr[0]);
      pObjIndex->description[0] = UPPER(pObjIndex->description[0]);
      /*
      pObjIndex->material = str_dup("");
      -material types put in -Wervdon
      */
      pObjIndex->item_type   = fread_number(fp);
      pObjIndex->extra_flags = fread_flag(fp);
      pObjIndex->wear_flags  = fread_flag(fp);
      pObjIndex->value[0]    = fread_number(fp);
      pObjIndex->value[1]    = fread_number(fp);
      pObjIndex->value[2]    = fread_number(fp);
      pObjIndex->value[3]    = fread_number(fp);
      pObjIndex->value[4]    = 0;
      pObjIndex->level       = 0;
      pObjIndex->condition   = 100;
      pObjIndex->weight      = fread_number(fp);
      pObjIndex->cost        = fread_number(fp);    /* Unused */
      /*
      Cost per day
      Unused
      */
      fread_number(fp);
      pObjIndex->mount_specs = NULL;
      if (pObjIndex->item_type == ITEM_WEAPON)
      {
         if
         (
            is_name("two", pObjIndex->name) ||
            is_name("two-handed", pObjIndex->name) ||
            is_name("claymore", pObjIndex->name)
         )
         {
            SET_BIT(pObjIndex->value[4], WEAPON_TWO_HANDS);
         }
      }

      for (; ;)
      {
         letter = fread_letter(fp);

         if (letter == 'A')
         {
            paf             = alloc_perm(sizeof(*paf));
            paf->where      = TO_OBJECT;
            paf->type       = gsn_reserved;
            paf->level      = 20; /* RT temp fix */
            paf->duration   = -1;
            paf->location   = fread_number(fp);
            paf->modifier   = fread_number(fp);
            paf->bitvector  = 0;
            paf->bitvector2 = 0;
            paf->next       = pObjIndex->affected;
            pObjIndex->affected = paf;
            top_affect++;
         }
         else if (letter == 'H')
         {
            pObjIndex->house = fread_number(fp);
         }
         else if (letter == 'L')
         {
            pObjIndex->limtotal = fread_number(fp);
         }
         else if (letter == 'R')
         {
            pObjIndex->restrict_flags = fread_flag(fp);
         }
         else if (letter == 'E')
         {
            ed = alloc_perm(sizeof(*ed));
            ed->keyword = fread_string(fp);
            ed->description = fread_string(fp);
            ed->next = pObjIndex->extra_descr;
            pObjIndex->extra_descr = ed;
            top_ed++;
         }
         else if (letter == 'M')
         {
            if (!pObjIndex->mount_specs)
            {
               specs = alloc_mem(sizeof(*specs));
            }
            else
            {
               specs        = pObjIndex->mount_specs;
            }
            specs->move = fread_number(fp);
            specs->mobility = fread_number(fp);
            specs->move_flags = fread_flag(fp);
            pObjIndex->mount_specs = specs;
         }
         else
         {
            ungetc(letter, fp);
            break;
         }
      }

      /* fix armors */
      if (pObjIndex->item_type == ITEM_ARMOR)
      {
         pObjIndex->value[1] = pObjIndex->value[0];
         pObjIndex->value[2] = pObjIndex->value[1];
      }
      iHash = vnum % MAX_KEY_HASH;
      pObjIndex->next = obj_index_hash[iHash];
      obj_index_hash[iHash] = pObjIndex;
      top_obj_index++;
   }
   return;
}





/*
* Snarf a reset section.
*/
void load_resets(FILE* fp)
{
   RESET_DATA* pReset;
   ROOM_INDEX_DATA* pRoomIndex;
   EXIT_DATA* pexit;
   char letter;
   OBJ_INDEX_DATA* temp_index;

   if (area_last == NULL)
   {
      bug("Load_resets: no #AREA seen yet.", 0);
      exit(1);
   }

   for (; ;)
   {

      if
      (
         (
            letter = fread_letter(fp)
         ) == 'S'
      )
      {
         break;
      }

      if (letter == '*')
      {
         fread_to_eol(fp);
         continue;
      }

      pReset = alloc_perm(sizeof(*pReset));
      pReset->command = letter;
      /* if_flag UNUSED */
      fread_number(fp);
      pReset->arg1 = fread_number(fp);
      pReset->arg2 = fread_number(fp);
      pReset->arg3 =
      (
         (
            letter == 'G' ||
            letter == 'R'
         ) ?
         0 :
         fread_number(fp)
      );
      pReset->arg4 =
      (
         (
            letter == 'P' ||
            letter == 'M'
         ) ?
         fread_number(fp) :
         0
      );
      fread_to_eol(fp);

      /*
      Validate parameters.
      We're calling the index functions for the side effect.
      */
      switch (letter)
      {
         default:
         {
            bug( "Load_resets: bad command '%c'.", letter );
            exit( 1 );
            break;
         }
         case ('M'):
         {
            get_mob_index(pReset->arg1);
            get_room_index(pReset->arg3);
            break;
         }
         case ('O'):
         {
            temp_index = get_obj_index(pReset->arg1);
            temp_index->reset_num++;
            get_room_index(pReset->arg3);
            break;
         }
         case ('P'):
         {
            temp_index = get_obj_index(pReset->arg1);
            temp_index->reset_num++;
            get_obj_index(pReset->arg3);
            break;
         }
         case ('G'):
         case ('E'):
         case ('X'):
         {
            bool wrong_slot = FALSE;
            int new_slot = pReset->arg3;
            bool no_fix = FALSE;
            char exception[13];

            if (letter == 'X')
            {
               sprintf(exception, "(Exception) ");
            }
            else
            {
               exception[0] = '\0';
            }

            temp_index = get_obj_index(pReset->arg1);
            temp_index->reset_num++;
            if (temp_index == NULL)
            {
               break;
            }
            if (letter == 'E')
            {
               if (temp_index->item_type == ITEM_LIGHT)
               {
                  new_slot = WEAR_LIGHT;
               }
               else if (IS_SET(temp_index->wear_flags, ITEM_WEAR_FINGER))
               {
                  if (pReset->arg3 == WEAR_FINGER_R)
                  {
                     no_fix = TRUE;
                  }
                  new_slot = WEAR_FINGER_L;
               }
               else if (IS_SET(temp_index->wear_flags, ITEM_WEAR_NECK))
               {
                  if (pReset->arg3 == WEAR_NECK_2)
                  {
                     no_fix = TRUE;
                  }
                  new_slot = WEAR_NECK_1;
               }
               else if (IS_SET(temp_index->wear_flags, ITEM_WEAR_BODY))
               {
                  new_slot = WEAR_BODY;
               }
               else if (IS_SET(temp_index->wear_flags, ITEM_WEAR_HEAD))
               {
                  if
                  (
                     pReset->arg3 == WEAR_HEAD ||
                     pReset->arg3 == WEAR_HORNS
                  )
                  {
                     no_fix = TRUE;
                  }
                  new_slot = WEAR_HEAD;
               }
               else if (IS_SET(temp_index->wear_flags, ITEM_WEAR_LEGS))
               {
                  if
                  (
                     pReset->arg3 == WEAR_LEGS ||
                     pReset->arg3 == WEAR_FOURLEGS
                  )
                  {
                     no_fix = TRUE;
                  }
                  new_slot = WEAR_LEGS;
               }
               else if (IS_SET(temp_index->wear_flags, ITEM_WEAR_FEET))
               {
                  if
                  (
                     pReset->arg3 == WEAR_FEET ||
                     pReset->arg3 == WEAR_FOURHOOVES ||
                     pReset->arg3 == WEAR_HOOVES
                  )
                  {
                     no_fix = TRUE;
                  }
                  new_slot = WEAR_FEET;
               }
               else if (IS_SET(temp_index->wear_flags, ITEM_WEAR_HANDS))
               {
                  new_slot = WEAR_HANDS;
               }
               else if (IS_SET(temp_index->wear_flags, ITEM_WEAR_ARMS))
               {
                  new_slot = WEAR_ARMS;
               }
               else if (IS_SET(temp_index->wear_flags, ITEM_WEAR_SHIELD))
               {
                  new_slot = WEAR_SHIELD;
               }
               else if (IS_SET(temp_index->wear_flags, ITEM_WEAR_ABOUT))
               {
                  new_slot = WEAR_ABOUT;
               }
               else if (IS_SET(temp_index->wear_flags, ITEM_WEAR_WAIST))
               {
                  new_slot = WEAR_WAIST;
               }
               else if (IS_SET(temp_index->wear_flags, ITEM_WEAR_WRIST))
               {
                  if (pReset->arg3 == WEAR_WRIST_R)
                  {
                     no_fix = TRUE;
                  }
                  new_slot = WEAR_WRIST_L;
               }
               else if (IS_SET(temp_index->wear_flags, ITEM_WIELD))
               {
                  if (pReset->arg3 == WEAR_DUAL_WIELD)
                  {
                     no_fix = TRUE;
                  }
                  new_slot = WEAR_WIELD;
               }
               else if (IS_SET(temp_index->wear_flags, ITEM_HOLD))
               {
                  new_slot = WEAR_HOLD;
               }
               else if (IS_SET(temp_index->wear_flags, ITEM_WEAR_BRAND))
               {
                  new_slot = WEAR_BRAND;
               }
               else if (IS_SET(temp_index->wear_flags, ITEM_WEAR_HORNS))
               {
                  if
                  (
                     pReset->arg3 == WEAR_HEAD ||
                     pReset->arg3 == WEAR_HORNS
                  )
                  {
                     no_fix = TRUE;
                  }
                  new_slot = WEAR_HEAD;
               }
               else if (IS_SET(temp_index->wear_flags, ITEM_WEAR_SNOUT))
               {
                  new_slot = WEAR_SNOUT;
               }
               else if (IS_SET(temp_index->wear_flags, ITEM_WEAR_HOOVES))
               {
                  if
                  (
                     pReset->arg3 == WEAR_FEET ||
                     pReset->arg3 == WEAR_FOURHOOVES ||
                     pReset->arg3 == WEAR_HOOVES
                  )
                  {
                     no_fix = TRUE;
                  }
                  new_slot = WEAR_FEET;
               }
               else if (IS_SET(temp_index->wear_flags, ITEM_WEAR_FOURLEGS))
               {
                  if
                  (
                     pReset->arg3 == WEAR_LEGS ||
                     pReset->arg3 == WEAR_FOURLEGS
                  )
                  {
                     no_fix = TRUE;
                  }
                  new_slot = WEAR_LEGS;
               }
               else if (IS_SET(temp_index->wear_flags, ITEM_WEAR_FOURHOOVES))
               {
                  if
                  (
                     pReset->arg3 == WEAR_FEET ||
                     pReset->arg3 == WEAR_FOURHOOVES ||
                     pReset->arg3 == WEAR_HOOVES
                  )
                  {
                     no_fix = TRUE;
                  }
                  new_slot = WEAR_FEET;
               }
               else if (IS_SET(temp_index->wear_flags, ITEM_WEAR_EYES))
               {
                  new_slot = WEAR_EYES;
               }
               else if (IS_SET(temp_index->wear_flags, ITEM_WEAR_EAR))
               {
                  if (pReset->arg3 == WEAR_EAR_R)
                  {
                     no_fix = TRUE;
                  }
                  new_slot = WEAR_EAR_L;
               }
               else if (IS_SET(temp_index->wear_flags, ITEM_WEAR_BOSOM))
               {
                  new_slot = WEAR_BOSOM;
               }
               else
               {
                  /* DEFAULT */
                  new_slot = WEAR_HOLD;
               }
            }
            if
            (
               letter == 'E' &&
               new_slot != pReset->arg3
            )
            {
               if
               (
                  !no_fix &&
                  pReset->arg3 != -1  /* -1 means auto set */
               )
               {
                  sprintf
                  (
                     log_buf,
                     "load_resets: %s changed reset on [%d] [%s] [%s] from [%d] [%s] to slot: [%d] [%s]",
                     exception,
                     temp_index->vnum,
                     temp_index->name,
                     temp_index->short_descr,
                     pReset->arg3,
                     wear_slot_name(pReset->arg3),
                     new_slot,
                     wear_slot_name(new_slot)
                  );
                  bug(log_buf, 0);
               }
               pReset->arg3 = new_slot;
            }
            /* Find mistakes in resets */
            switch (pReset->arg3)
            {
               default:
               {
                  bug("Bad wear slot in load_resets: %d", pReset->arg3);
                  exit(1);
                  break;
               }
               case (WEAR_NONE):
               {
                  break;
               }
               case (WEAR_LIGHT):
               {
                  if
                  (
                     temp_index->item_type != ITEM_LIGHT &&
                     letter != 'G'  /* G always uses 0 */
                  )
                  {
                     sprintf
                     (
                        log_buf,
                        "load_resets: %s loading non light [%d] [%s] [%s] to light slot: [%d]",
                        exception,
                        temp_index->vnum,
                        temp_index->name,
                        temp_index->short_descr,
                        pReset->arg3
                     );
                     bug(log_buf, 0);
                  }
                  break;
               }
               case (WEAR_FINGER_L):
               case (WEAR_FINGER_R):
               {
                  if (!CAN_WEAR(temp_index, ITEM_WEAR_FINGER))
                  {
                     wrong_slot = TRUE;
                  }
                  break;
               }
               case (WEAR_NECK_1):
               case (WEAR_NECK_2):
               {
                  if (!CAN_WEAR(temp_index, ITEM_WEAR_NECK))
                  {
                     wrong_slot = TRUE;
                  }
                  break;
               }
               case (WEAR_BODY):
               {
                  if (!CAN_WEAR(temp_index, ITEM_WEAR_BODY))
                  {
                     wrong_slot = TRUE;
                  }
                  break;
               }
               case (WEAR_HEAD):  /* Default for all head stuff */
               {
                  if
                  (
                     !CAN_WEAR(temp_index, ITEM_WEAR_HEAD) &&
                     !CAN_WEAR(temp_index, ITEM_WEAR_HORNS)
                  )
                  {
                     wrong_slot = TRUE;
                  }
                  break;
               }
               case (WEAR_HORNS):
               {
                  if (!CAN_WEAR(temp_index, ITEM_WEAR_HORNS))
                  {
                     wrong_slot = TRUE;
                  }
                  break;
               }
               case (WEAR_LEGS):  /* Default for all leg stuff */
               {
                  if
                  (
                     !CAN_WEAR(temp_index, ITEM_WEAR_LEGS) &&
                     !CAN_WEAR(temp_index, ITEM_WEAR_FOURLEGS)
                  )
                  {
                     wrong_slot = TRUE;
                  }
                  break;
               }
               case (WEAR_FOURLEGS):
               {
                  if (!CAN_WEAR(temp_index, ITEM_WEAR_FOURLEGS))
                  {
                     wrong_slot = TRUE;
                  }
                  break;
               }
               case (WEAR_FEET):  /* Default for all foot stuff */
               {
                  if
                  (
                     !CAN_WEAR(temp_index, ITEM_WEAR_FEET) &&
                     !CAN_WEAR(temp_index, ITEM_WEAR_HOOVES) &&
                     !CAN_WEAR(temp_index, ITEM_WEAR_FOURHOOVES)
                  )
                  {
                     wrong_slot = TRUE;
                  }
                  break;
               }
               case (WEAR_HOOVES):
               {
                  if (!CAN_WEAR(temp_index, ITEM_WEAR_HOOVES))
                  {
                     wrong_slot = TRUE;
                  }
                  break;
               }
               case (WEAR_FOURHOOVES):
               {
                  if (!CAN_WEAR(temp_index, ITEM_WEAR_FOURHOOVES))
                  {
                     wrong_slot = TRUE;
                  }
                  break;
               }
               case (WEAR_HANDS):
               {
                  if (!CAN_WEAR(temp_index, ITEM_WEAR_HANDS))
                  {
                     wrong_slot = TRUE;
                  }
                  break;
               }
               case (WEAR_ARMS):
               {
                  if (!CAN_WEAR(temp_index, ITEM_WEAR_ARMS))
                  {
                     wrong_slot = TRUE;
                  }
                  break;
               }
               case (WEAR_SHIELD):
               {
                  if (!CAN_WEAR(temp_index, ITEM_WEAR_SHIELD))
                  {
                     wrong_slot = TRUE;
                  }
                  if (temp_index->item_type != ITEM_ARMOR)
                  {
                     sprintf
                     (
                        log_buf,
                        "load_resets: %s loading non armor [%d] [%s] [%s] to shield slot: [%d]",
                        exception,
                        temp_index->vnum,
                        temp_index->name,
                        temp_index->short_descr,
                        pReset->arg3
                     );
                     bug(log_buf, 0);
                  }
                  break;
               }
               case (WEAR_ABOUT):
               {
                  if (!CAN_WEAR(temp_index, ITEM_WEAR_ABOUT))
                  {
                     wrong_slot = TRUE;
                  }
                  break;
               }
               case (WEAR_WAIST):
               {
                  if (!CAN_WEAR(temp_index, ITEM_WEAR_WAIST))
                  {
                     wrong_slot = TRUE;
                  }
                  break;
               }
               case (WEAR_WRIST_L):
               case (WEAR_WRIST_R):
               {
                  if (!CAN_WEAR(temp_index, ITEM_WEAR_WRIST))
                  {
                     wrong_slot = TRUE;
                  }
                  break;
               }
               case (WEAR_WIELD):
               case (WEAR_DUAL_WIELD):
               case (WEAR_TERTIARY_WIELD):
               {
                  if (!CAN_WEAR(temp_index, ITEM_WIELD))
                  {
                     wrong_slot = TRUE;
                  }
                  if (temp_index->item_type != ITEM_WEAPON)
                  {
                     sprintf
                     (
                        log_buf,
                        "load_resets: %sloading non weapon [%d] [%s] [%s] to a wield slot: [%d]",
                        exception,
                        temp_index->vnum,
                        temp_index->name,
                        temp_index->short_descr,
                        pReset->arg3
                     );
                     bug(log_buf, 0);
                  }
                  break;
               }
               case (WEAR_HOLD):
               {
                  if (!CAN_WEAR(temp_index, ITEM_HOLD))
                  {
                     wrong_slot = TRUE;
                  }
                  break;
               }
               case (WEAR_BRAND):
               {
                  /* Should be noremove, extra_brand, no_locate, no_uncurse, burn_proof */
                  if (!CAN_WEAR(temp_index, ITEM_WEAR_BRAND))
                  {
                     wrong_slot = TRUE;
                  }
                  if (!IS_OBJ_STAT(temp_index, ITEM_NOREMOVE))
                  {
                     sprintf
                     (
                        log_buf,
                        "load_resets: loading brand not ITEM_NOREMOVE [%d] [%s] [%s]",
                        temp_index->vnum,
                        temp_index->name,
                        temp_index->short_descr
                     );
                     bug(log_buf, 0);
                  }
                  if (!IS_OBJ_STAT(temp_index, ITEM_BRAND))
                  {
                     sprintf
                     (
                        log_buf,
                        "load_resets: loading brand not ITEM_BRAND [%d] [%s] [%s]",
                        temp_index->vnum,
                        temp_index->name,
                        temp_index->short_descr
                     );
                     bug(log_buf, 0);
                  }
                  if (!IS_OBJ_STAT(temp_index, ITEM_NOLOCATE))
                  {
                     sprintf
                     (
                        log_buf,
                        "load_resets: loading brand not ITEM_NOLOCATE [%d] [%s] [%s]",
                        temp_index->vnum,
                        temp_index->name,
                        temp_index->short_descr
                     );
                     bug(log_buf, 0);
                  }
                  if (!IS_OBJ_STAT(temp_index, ITEM_NOUNCURSE))
                  {
                     sprintf
                     (
                        log_buf,
                        "load_resets: loading brand not ITEM_NOUNCURSE [%d] [%s] [%s]",
                        temp_index->vnum,
                        temp_index->name,
                        temp_index->short_descr
                     );
                     bug(log_buf, 0);
                  }
                  if (!IS_OBJ_STAT(temp_index, ITEM_BURN_PROOF))
                  {
                     sprintf
                     (
                        log_buf,
                        "load_resets: loading brand not ITEM_BURN_PROOF [%d] [%s] [%s]",
                        temp_index->vnum,
                        temp_index->name,
                        temp_index->short_descr
                     );
                     bug(log_buf, 0);
                  }
                  break;
               }
               case (WEAR_SNOUT):
               {
                  if (!CAN_WEAR(temp_index, ITEM_WEAR_SNOUT))
                  {
                     wrong_slot = TRUE;
                  }
                  break;
               }
               case (WEAR_EYES):
               {
                  if (!CAN_WEAR(temp_index, ITEM_WEAR_EYES))
                  {
                     wrong_slot = TRUE;
                  }
                  break;
               }
               case (WEAR_EAR_R):
               case (WEAR_EAR_L):
               {
                  if (!CAN_WEAR(temp_index, ITEM_WEAR_EAR))
                  {
                     wrong_slot = TRUE;
                  }
                  break;
               }
               case (WEAR_BOSOM):
               {
                  if (!CAN_WEAR(temp_index, ITEM_WEAR_BOSOM))
                  {
                     wrong_slot = TRUE;
                  }
                  break;
               }
            }
            if (!CAN_WEAR(temp_index, ITEM_TAKE))
            {
               /* Should be able to take it then */
               sprintf
               (
                  log_buf,
                  "load_resets: loading non-wear_take object [%d] [%s] [%s] to slot: [%d] [%s]",
                  temp_index->vnum,
                  temp_index->name,
                  temp_index->short_descr,
                  pReset->arg3,
                  wear_slot_name(pReset->arg3)
               );
               bug(log_buf, 0);
            }
            /*
               If it cannot be worn anywhere, the mob can put it where he likes
               except for wield or shield.
               i.e. hold a key that can't be held by mortals
            */
            if
            (
               wrong_slot &&
               letter != 'X' &&
               (
                  pReset->arg3 == ITEM_WIELD ||
                  pReset->arg3 == ITEM_WEAR_SHIELD ||
                  (
                     temp_index->wear_flags &
                     ~ITEM_TAKE &
                     ~ITEM_NO_SAC &
                     ~ITEM_KEEP
                  ) != 0
               )
            )
            {
               sprintf
               (
                  log_buf,
                  "load_resets: loading [%d] [%s] [%s] to wrong slot: [%d] [%s]",
                  temp_index->vnum,
                  temp_index->name,
                  temp_index->short_descr,
                  pReset->arg3,
                  wear_slot_name(pReset->arg3)
               );
               bug(log_buf, 0);
            }
            break;
         }
         case ('D'):
         {
            pRoomIndex = get_room_index(pReset->arg1);
            if
            (
               pReset->arg2 < 0 ||
               pReset->arg2 > 5 ||
               (
                  pexit = pRoomIndex->exit[pReset->arg2]
               ) == NULL ||
               !IS_SET(pexit->exit_info, EX_ISDOOR)
            )
            {
               bug("Load_resets: 'D'): exit %d not door.", pReset->arg2);
               exit(1);
            }
            if
            (
               pReset->arg3 < 0 ||
               pReset->arg3 > 2
            )
            {
               bug("Load_resets: 'D'): bad 'locks'): %d.", pReset->arg3);
               exit(1);
            }
            break;
         }
         case ('R'):
         {
            pRoomIndex = get_room_index(pReset->arg1);
            if
            (
               pReset->arg2 < 0 ||
               pReset->arg2 > 6
            )
            {
               bug("Load_resets: 'R'): bad exit %d.", pReset->arg2);
               exit(1);
            }
            break;
         }
      }

      if (area_last->reset_first == NULL)
      {
         area_last->reset_first = pReset;
      }
      if (area_last->reset_last  != NULL)
      {
         area_last->reset_last->next = pReset;
      }
      area_last->reset_last = pReset;
      pReset->next = NULL;
      top_reset++;
   }
   return;
}

/*
* Snarf a room section.
*/
void load_rooms(FILE* fp)
{
   ROOM_INDEX_DATA *pRoomIndex;
   /*    int vnum2 = 0;
   ROOM_INDEX_DATA *pRoomIndex2 = NULL; */

   if ( area_last == NULL )
   {
      bug( "Load_resets: no #AREA seen yet.", 0 );
      exit( 1 );
   }

   for ( ; ; )
   {
      sh_int vnum;
      char letter;
      int door;
      int iHash;

      letter                = fread_letter( fp );
      if ( letter != '#' )
      {
         bug( "Load_rooms: # not found.", 0 );
         exit( 1 );
      }

      vnum                = fread_number( fp );
      if ( vnum == 0 )
      break;

      fBootDb = FALSE;
      if ( get_room_index( vnum ) != NULL )
      {
         bug( "Load_rooms: vnum %d duplicated.", vnum );
         exit( 1 );
      }
      fBootDb = TRUE;

      pRoomIndex            = alloc_perm( sizeof(*pRoomIndex) );
      pRoomIndex->owner        = str_dup("");
      pRoomIndex->people        = NULL;
      pRoomIndex->contents        = NULL;
      pRoomIndex->extra_descr        = NULL;
      pRoomIndex->area        = area_last;
      pRoomIndex->next_in_area = pRoomIndex->area->rooms;
      pRoomIndex->area->rooms = pRoomIndex;
      pRoomIndex->vnum        = vnum;
      pRoomIndex->name        = fread_string( fp );
      pRoomIndex->description        = fread_string( fp );
      pRoomIndex->alarm = NULL;

      /* Area number */          fread_number( fp );
      pRoomIndex->room_flags        = fread_flag( fp );
      {
         int i;
         pRoomIndex->trackhead = 0;
         for (i=0;i<10;i++)
         {
            pRoomIndex->track_dir[i][0] = 0;
            pRoomIndex->track_dir[i][1] = -1;
         }
      }
      /* horrible hack */
      if ( 3000 <= vnum && vnum < 3400)
      SET_BIT(pRoomIndex->room_flags, ROOM_LAW);
      pRoomIndex->sector_type        = fread_number( fp );
      if (pRoomIndex->sector_type == -1)
      {
         char word[MAX_STRING_LENGTH];
         int s = SECT_FIELD;
         fscanf(fp, "%s", word);
         if (!str_cmp(word, "inside")) s = SECT_INSIDE;
         if (!str_cmp(word, "outside")) s = SECT_FIELD;
         if (!str_cmp(word, "city"))   s = SECT_CITY;
         if (!str_cmp(word, "field"))  s = SECT_FIELD;
         if (!str_cmp(word, "plains"))  s = SECT_FIELD;
         if (!str_cmp(word, "forest")) s = SECT_FOREST;
         if (!str_cmp(word, "hills"))  s = SECT_HILLS;
         if (!str_cmp(word, "mountain")) s=SECT_MOUNTAIN;
         if (!str_cmp(word, "water"))s=SECT_WATER_NOSWIM;
         if (!str_cmp(word, "water_swim"))s=SECT_WATER_SWIM;
         if (!str_cmp(word, "water_noswim"))s= SECT_WATER_NOSWIM;
         if (!str_cmp(word, "air"))       s =  SECT_AIR;
         if (!str_cmp(word, "sky"))       s =  SECT_AIR;
         if (!str_cmp(word, "desert"))    s = SECT_DESERT;
         if (!str_cmp(word, "underwater")) s= SECT_UNDERWATER;
         if (!str_cmp(word, "underground")) s = SECT_UNDERGROUND;
         if (!str_cmp(word, "cave")) s = SECT_UNDERGROUND;
         pRoomIndex->sector_type = s;
      }
      pRoomIndex->light        = 0;
      for ( door = 0; door <= 5; door++ )
      pRoomIndex->exit[door] = NULL;

      /* defaults */
      pRoomIndex->heal_rate = 100;
      pRoomIndex->mana_rate = 100;
      pRoomIndex->sound = NULL;
      pRoomIndex->sound_rate = 0;

      for ( ; ; )
      {
         letter = fread_letter( fp );

         if ( letter == 'S' )
         break;

         if ( letter == 'H') /* healing room */
         pRoomIndex->heal_rate = fread_number(fp);

         else if ( letter == 'F') /* extra room flags */
         pRoomIndex->extra_room_flags        = fread_flag( fp );

         else if ( letter == 'M') /* mana room */
         pRoomIndex->mana_rate = fread_number(fp);

         else if (letter == 's'){
            pRoomIndex->sound_rate = fread_number(fp);
            pRoomIndex->sound = fread_string(fp);
         }
         else if ( letter == 'G') /* Guilds...added by Ceran */
         pRoomIndex->guild = fread_number(fp);

         else if ( letter == 'C') /* house */
         {
            if (pRoomIndex->house)
            {
               bug("Load_rooms: duplicate house fields.", 0);
               exit(1);
            }
            /*
            pRoomIndex->house = house_lookup(fread_string(fp));
            */
            pRoomIndex->house = fread_number(fp);
         }

         else if ( letter == 'X' )
         {
            EXIT_DATA *pexit;
            int locks;

            door = -1;
            letter = fread_letter( fp );
            if (letter == 'N') door = 0;
            if (letter == 'E') door = 1;
            if (letter == 'S') door = 2;
            if (letter == 'W') door = 3;
            if (letter == 'U') door = 4;
            if (letter == 'D') door = 5;

            if ( door < 0 || door > 5 )
            {
               bug( "Fread_rooms: vnum %d has bad door letter.", vnum);
               exit( 1 );
            }

            pexit            = alloc_perm( sizeof(*pexit) );
            pexit->description    = fread_string( fp );
            pexit->keyword        = fread_string( fp );
            pexit->exit_info    = 0;
            locks            = fread_number( fp );
            pexit->key        = fread_number( fp );
            pexit->u1.vnum        = fread_number( fp );

            switch ( locks )
            {
               case 1: pexit->exit_info = EX_ISDOOR;                break;
               case 2: pexit->exit_info = EX_ISDOOR | EX_PICKPROOF; break;
               case 3: pexit->exit_info = EX_ISDOOR | EX_NOPASS;    break;
               case 4: pexit->exit_info = EX_ISDOOR|EX_NOPASS|EX_PICKPROOF;
               case 5: pexit->exit_info = EX_ISDOOR|EX_NOPASS|EX_PICKPROOF|EX_NOBASH;
               break;
            }

            pRoomIndex->exit[door]    = pexit;
            pRoomIndex->old_exit[door] = pexit;
            top_exit++;
         }

         else if ( letter == 'D' )
         {
            EXIT_DATA *pexit;
            int locks;

            door = fread_number( fp );
            if ( door < 0 || door > 5 )
            {
               bug( "Fread_rooms: vnum %d has bad door number.", vnum );
               exit( 1 );
            }

            pexit            = alloc_perm( sizeof(*pexit) );
            pexit->description    = fread_string( fp );
            pexit->keyword        = fread_string( fp );
            pexit->exit_info    = 0;
            locks            = fread_number( fp );
            pexit->key        = fread_number( fp );
            pexit->u1.vnum        = fread_number( fp );

            switch ( locks )
            {
               case 1: pexit->exit_info = EX_ISDOOR;                break;
               case 2: pexit->exit_info = EX_ISDOOR | EX_PICKPROOF; break;
               case 3: pexit->exit_info = EX_ISDOOR | EX_NOPASS;    break;
               case 4: pexit->exit_info = EX_ISDOOR|EX_NOPASS|EX_PICKPROOF;
               case 5: pexit->exit_info = EX_ISDOOR|EX_NOPASS|EX_PICKPROOF|EX_NOBASH;
               break;
            }

            pRoomIndex->exit[door]    = pexit;
            pRoomIndex->old_exit[door] = pexit;
            top_exit++;
         }
         else if ( letter == 'E' )
         {
            EXTRA_DESCR_DATA *ed;
            mem_counts[NMEM_EX_DESCR]++;

            ed            = alloc_perm( sizeof(*ed) );
            ed->keyword        = fread_string( fp );
            ed->description        = fread_string( fp );
            ed->next        = pRoomIndex->extra_descr;
            pRoomIndex->extra_descr    = ed;
            top_ed++;
         }

         else if (letter == 'O')
         {
            if (pRoomIndex->owner[0] != '\0')
            {
               bug("Load_rooms: duplicate owner.", 0);
               exit(1);
            }

            pRoomIndex->owner = fread_string(fp);
         }

         else
         {
            bug( "Load_rooms: vnum %d has flag not 'DES'.", vnum );
            exit( 1 );
         }
      }

      iHash            = vnum % MAX_KEY_HASH;
      pRoomIndex->next    = room_index_hash[iHash];
      room_index_hash[iHash]    = pRoomIndex;
      top_room++;
   }

   return;
}



/*
* Snarf a shop section.
*/
void load_shops( FILE *fp )
{
   SHOP_DATA *pShop;

   for ( ; ; )
   {
      MOB_INDEX_DATA *pMobIndex;
      int iTrade;

      pShop            = alloc_perm( sizeof(*pShop) );
      pShop->keeper        = fread_number( fp );
      if ( pShop->keeper == 0 )
      break;
      for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
      pShop->buy_type[iTrade]    = fread_number( fp );
      pShop->profit_buy    = fread_number( fp );
      pShop->profit_sell    = fread_number( fp );
      pShop->open_hour    = fread_number( fp );
      pShop->close_hour    = fread_number( fp );
      fread_to_eol( fp );
      pMobIndex        = get_mob_index( pShop->keeper );
      pMobIndex->pShop    = pShop;

      if ( shop_first == NULL )
      shop_first = pShop;
      if ( shop_last  != NULL )
      shop_last->next = pShop;

      shop_last    = pShop;
      pShop->next    = NULL;
      top_shop++;
   }

   return;
}


/*
* Snarf spec proc declarations.
*/
void load_specials( FILE *fp )
{
   for ( ; ; )
   {
      MOB_INDEX_DATA *pMobIndex;
      char letter;

      switch ( letter = fread_letter( fp ) )
      {
         default:
         bug( "Load_specials: letter '%c' not *MS.", letter );
         exit( 1 );

         case 'S':
         return;

         case '*':
         break;

         case 'M':
         pMobIndex        = get_mob_index    ( fread_number ( fp ) );
         pMobIndex->spec_fun    = spec_lookup    ( fread_word   ( fp ) );
         if ( pMobIndex->spec_fun == 0 )
         {
            bug( "Load_specials: 'M': vnum %d.", pMobIndex->vnum );
            exit( 1 );
         }
         break;
      }

      fread_to_eol( fp );
   }
}


/*
* Translate all room exits from virtual to real.
* Has to be done after all rooms are read in.
* Check for bad reverse exits.
*/
void fix_exits( void )
{
   extern const sh_int rev_dir [];
   char buf[MAX_STRING_LENGTH];
   ROOM_INDEX_DATA *pRoomIndex;
   AREA_DATA* parea;
   ROOM_INDEX_DATA *to_room;
   EXIT_DATA *pexit;
   EXIT_DATA *pexit_rev;
   int door;
   bool fexit;

   for (parea = area_first; parea; parea = parea->next)
   {
      for
      (
         pRoomIndex = parea->rooms;
         pRoomIndex;
         pRoomIndex = pRoomIndex->next_in_area
      )
      {
         fexit = FALSE;
         for (door = 0; door < 6; door++)
         {
            if
            (
               (
                  pexit = pRoomIndex->exit[door]
               ) != NULL
            )
            {
               if
               (
                  pexit->u1.vnum <= 0 ||
                  get_room_index(pexit->u1.vnum) == NULL
               )
               {
                  pexit->u1.to_room = NULL;
               }
               else
               {
                  fexit = TRUE;
                  pexit->u1.to_room = get_room_index(pexit->u1.vnum);
               }
            }
         }
         if (!fexit)
         {
            SET_BIT(pRoomIndex->room_flags, ROOM_NO_MOB);
         }
      }
   }
   /* Pointers are not real untill here. */
   for (parea = area_first; parea; parea = parea->next)
   {
      for
      (
         pRoomIndex = parea->rooms;
         pRoomIndex;
         pRoomIndex = pRoomIndex->next_in_area
      )
      {
         for (door = 0; door < 6; door++)
         {
            if
            (
               (
                  pexit = pRoomIndex->exit[door]
               ) != NULL &&
               (
                  to_room = pexit->u1.to_room
               ) != NULL &&
               (
                  pexit_rev = to_room->exit[rev_dir[door]]
               ) != NULL &&
               pexit_rev->u1.to_room != pRoomIndex &&
               (
                  pRoomIndex->vnum < 1200 ||
                  pRoomIndex->vnum > 1299
               )
            )
            {
               sprintf
               (
                  buf,
                  "Fix_exits: %d:%d -> %d:%d -> %d.",
                  pRoomIndex->vnum,
                  door,
                  to_room->vnum,
                  rev_dir[door],
                  pexit_rev->u1.to_room == NULL ?
                  0 :
                  pexit_rev->u1.to_room->vnum
               );
               log_string(buf);
            }
         }
      }
   }
   return;
}

/*
* Load mobprogs section
*/
void load_mobprogs( FILE *fp )
{
   MPROG_CODE *pMprog;

   if ( area_last == NULL )
   {
      bug( "Load_mobprogs: no #AREA seen yet.", 0 );
      exit( 1 );
   }

   for ( ; ; )
   {
      sh_int vnum;
      char letter;

      letter            = fread_letter( fp );
      if ( letter != '#' )
      {
         bug( "Load_mobprogs: # not found.", 0 );
         exit( 1 );
      }

      vnum             = fread_number( fp );
      if ( vnum == 0 )
      break;

      fBootDb = FALSE;
      if ( get_mprog_index( vnum ) != NULL )
      {
         bug( "Load_mobprogs: vnum %d duplicated.", vnum );
         exit( 1 );
      }
      fBootDb = TRUE;

      pMprog          = alloc_perm( sizeof(*pMprog) );
      pMprog->vnum    = vnum;
      pMprog->code    = fread_string( fp );
      if ( mprog_list == NULL )
      mprog_list = pMprog;
      else
      {
         pMprog->next = mprog_list;
         mprog_list  = pMprog;
      }
   }
   return;
}

/*
   fread_string_to_var

   This function reads a string in similar fashion to fread_string except that
   it does not hold the string in permanent storage, and the string is safely
   depositted into the passed-in string variable.

   Note: Unsafe.  We make the assumption that a given string_ptr has enough
   space to hold the input.
*/
void fread_string_to_var(FILE* fp, char* string_ptr)
{
   char c;
   char* char_ptr = string_ptr;

   *char_ptr = '\0';

   /* Skip blanks. Read first char.  */
   do
   {
      c = getc(fp);
   }
   while (isspace(c));

   /* Are we already done on the first character? */
   if (c == '~')
   {
      return;
   }

   /* Apply the first character to the string_ptr. */
   *char_ptr = c;
   char_ptr++;

   for ( ;; )
   {
      /*
      * Back off the char type lookup,
      *   it was too dirty for portability.
      *   -- Furey
      */

      switch (c = getc(fp))
      {
         case EOF:
            bug("Fread_string_to_var: EOF", 0);
            return;
            break;

         case '\n':
            *char_ptr = '\n';
            char_ptr++;
            *char_ptr = '\r';
            char_ptr++;
            break;

         case '\r':
            break;

         case '~':
            *char_ptr = '\0';
            return;
            break;

         default:
            *char_ptr = c;
            char_ptr++;
      }
   }
}

/*
* Load quests section
*/
void load_quests(FILE *fp)
{
   QUEST_TABLE new_quest;

   if (area_last == NULL)
   {
      bug("Load_quests: no #AREA seen yet.", 0);
      exit(1);
   }

   for (;;)
   {
      sh_int vnum;
      sh_int subvnum;
      char   letter;
      int    flags;

      new_quest.id[0]          = '\0';
      new_quest.title[0]       = '\0';
      new_quest.summary[0]     = '\0';
      new_quest.description[0] = '\0';
      new_quest.completed      = 0;
      new_quest.hidden         = 0;

      letter           = fread_letter(fp);
      if (letter != '#')
      {
         bug("Load_quests: # not found.", 0);
         exit(1);
      }

      /* Read the Quest ID. */
      vnum             = fread_number(fp);
      if (vnum == 0) /* Are we done with the QUESTS section? */
      {
         break;
      }

      letter           = fread_letter(fp);

      if (letter != '.')
      {
         bug("Load_quests: Quest ID of the form <quest>.<phase> not found where expected", 0);
         exit(1);
      }

      subvnum          = fread_number(fp);


      /* Fill in the quest data */
      sprintf(new_quest.id, "%d.%d", vnum, subvnum);
      fread_string_to_var(fp, new_quest.title);
      fread_string_to_var(fp, new_quest.summary);
      fread_string_to_var(fp, new_quest.description);

      flags = fread_flag(fp);
      new_quest.completed = IS_SET(flags, flag_convert('A'));
      new_quest.hidden    = IS_SET(flags, flag_convert('B'));


      /* Add the new quest to the database */
      database_add_quest(new_quest);
   }

   return;
}

/*
*  Translate mobprog vnums pointers to real code
*/
void fix_mobprogs(void)
{
   MOB_INDEX_DATA *pMobIndex;
   MPROG_LIST        *list;
   MPROG_CODE        *prog;
   char buf[MAX_INPUT_LENGTH];
   AREA_DATA* parea;

   log_string("Checking for stunned or dead mobs.");
   for (parea = area_first; parea; parea = parea->next)
   {
      for
      (
         pMobIndex = parea->mobs;
         pMobIndex;
         pMobIndex = pMobIndex->next_in_area
      )
      {
         for (list = pMobIndex->mprogs; list != NULL; list = list->next)
         {
            if ((prog = get_mprog_index(list->vnum)))
            {
               list->code = prog->code;
            }
            else
            {
               sprintf
               (
                  buf,
                  "Fix_mobprogs: code vnum %d for mob %d not found.",
                  list->vnum,
                  pMobIndex->vnum
               );
               bug(buf, 0);
               exit(1);
            }
         }
      }
   }
}


/*
* Repopulate areas periodically.
*/
void area_update( void )
{
   AREA_DATA *pArea;
   CHAR_DATA* ch;
   char buf[MAX_STRING_LENGTH];

   /* Because of so many memory corruption bugs, recount people in area */
   /* BEGIN RECOUNT */
   for (pArea = area_first; pArea != NULL; pArea = pArea->next)
   {
      pArea->nplayer = 0;
   }
   for (ch = char_list; ch; ch = ch->next)
   {
      if
      (
         !IS_NPC(ch) &&
         ch->on_line &&
         (
            ch->desc == NULL ||
            ch->desc->connected == CON_PLAYING
         ) &&
         ch->in_room
      )
      {
         ++ch->in_room->area->nplayer;
      }
   }
   /* END RECOUNT */
   for ( pArea = area_first; pArea != NULL; pArea = pArea->next )
   {
      /* Midgard doesnt reset for this reboot */

      if (++pArea->age < 3)
      {
         continue;
      }

      /*
         Check age and reset.
         Note: Mud School resets every 3 updates (6 minutes)
         (not 15 updates like normal).
      */
      if
      (
         (
            !pArea->empty &&
            (
               (
                  /* Empty for 2 resets in a row, slows down */
                  pArea->nplayer == 0 &&
                  pArea->empty_for < 2
               ) ||
               pArea->age >= 15
            )
         ) ||
         pArea->age >= 31
      )
      {
         ROOM_INDEX_DATA* pRoomIndex;

         reset_area(pArea);
         sprintf(buf, "%s has just been reset.", pArea->name);
         wiznet(buf, NULL, NULL, WIZ_RESETS, 0, 0);
         pRoomIndex = get_room_index(ROOM_VNUM_SCHOOL);
         if
         (
            pRoomIndex != NULL &&
            pArea == pRoomIndex->area
         )
         {
            pArea->age = 13;  /* 15 - 2 */
         }
         else if
         (
            (
               pRoomIndex = get_room_index(9401)
            ) != NULL &&
            pRoomIndex->area == pArea
         )
         {
            pArea->age = 13;  /* 15 - 2 */
         }
         else if (pArea->nplayer == 0)
         {
            pArea->age = number_range(0, 3);
            /*
               Empty for 3 resets in a row, slows down
               even further, and slows down processing
               in the area as well.
               takes 12 - 21 updates, (24-42 minutes)
               to get to the slower processing mode.
               Average about 33 minutes
            */
            if
            (
               !pArea->empty &&
               ++pArea->empty_for > 2
            )
            {
               pArea->empty = TRUE;
            }
         }
         else
         {
            pArea->age = number_bits(1);  /* 0 or 1, slightly random */
            /* Sanity check here */
            pArea->empty = FALSE;
            pArea->empty_for = 0;
         }
      }
   }

   return;
}



/*
* Reset one area.
*/
void reset_area( AREA_DATA *pArea )
{
   RESET_DATA *pReset;
   CHAR_DATA *mob;
   bool last;
   int level;

   mob     = NULL;
   last    = TRUE;
   level    = 0;
   for ( pReset = pArea->reset_first; pReset != NULL; pReset = pReset->next )
   {
      ROOM_INDEX_DATA *pRoomIndex;
      MOB_INDEX_DATA *pMobIndex;
      OBJ_INDEX_DATA *pObjIndex;
      OBJ_INDEX_DATA *pObjToIndex;
      OBJ_DATA* secondary;
      OBJ_DATA* primary;
      EXIT_DATA *pexit;
      OBJ_DATA *obj;
      OBJ_DATA *obj_to;
      int count, limit;

      switch ( pReset->command )
      {
         default:
         bug( "Reset_area: bad command %c.", pReset->command );
         break;

         case 'M':
         if ( ( pMobIndex = get_mob_index( pReset->arg1 ) ) == NULL )
         {
            bug( "Reset_area: 'M': bad vnum %d.", pReset->arg1 );
            continue;
         }

         if ( ( pRoomIndex = get_room_index( pReset->arg3 ) ) == NULL )
         {
            bug( "Reset_area: 'R': bad vnum %d.", pReset->arg3 );
            continue;
         }

         if ( pMobIndex->count >= pReset->arg2 )
         {
            last = FALSE;
            break;
         }

         count = 0;
         for (mob = pRoomIndex->people; mob != NULL; mob = mob->next_in_room)
         if (mob->pIndexData == pMobIndex)
         {
            count++;
            if (count >= pReset->arg4)
            {
               last = FALSE;
               break;
            }
         }

         if (count >= pReset->arg4)
         break;

         mob = create_mobile( pMobIndex );

         /*
         * Check for pet shop.
         */
         {
            ROOM_INDEX_DATA *pRoomIndexPrev;
            pRoomIndexPrev = get_room_index( pRoomIndex->vnum - 1 );
            if ( pRoomIndexPrev != NULL
            &&   IS_SET(pRoomIndexPrev->room_flags, ROOM_PET_SHOP) )
            SET_BIT(mob->act, ACT_PET);
         }

         /* set area */
         mob->zone = pRoomIndex->area;

         char_to_room( mob, pRoomIndex );
         level = URANGE( 0, mob->level - 2, LEVEL_HERO - 1 );
         last  = TRUE;
         break;

         case 'O':
         if ( ( pObjIndex = get_obj_index( pReset->arg1 ) ) == NULL )
         {
            bug( "Reset_area: 'O': bad vnum %d.", pReset->arg1 );
            continue;
         }

         if ( ( pRoomIndex = get_room_index( pReset->arg3 ) ) == NULL )
         {
            bug( "Reset_area: 'R': bad vnum %d.", pReset->arg3 );
            continue;
         }

         if ((pObjIndex->limcount >= pObjIndex->limtotal
         && pObjIndex->limtotal > 0)
         || count_obj_list(pObjIndex, pRoomIndex->contents) > 0)
         {
            last = FALSE;
            break;
         }

         obj       = create_object( pObjIndex, UMIN(number_fuzzy(level),
         LEVEL_HERO - 1) );
         obj->cost = 0;
         obj->owner = &str_none[0];
         obj->talked = str_dup("someone a long long time ago");
         obj_to_room( obj, pRoomIndex );
         last = TRUE;
         break;

         case 'P':
         if ( ( pObjIndex = get_obj_index( pReset->arg1 ) ) == NULL )
         {
            bug( "Reset_area: 'P': bad vnum %d.", pReset->arg1 );
            continue;
         }

         if ( ( pObjToIndex = get_obj_index( pReset->arg3 ) ) == NULL )
         {
            bug( "Reset_area: 'P': bad vnum %d.", pReset->arg3 );
            continue;
         }

         if (pReset->arg2 > 50) /* old format */
         limit = 6;
         else if (pReset->arg2 == -1) /* no limit */
         limit = 999;
         else
         limit = pReset->arg2;

         if (pArea->nplayer > 0
         || (obj_to = get_obj_type( pObjToIndex ) ) == NULL
         || (obj_to->in_room == NULL && !last)
         || ( pObjIndex->count >= limit && number_range(0, 4) != 0)
         || ( pObjIndex->limcount >= pObjIndex->limtotal && pObjIndex->limtotal > 0 )
         || (count = count_obj_list(pObjIndex, obj_to->contains))
         > pReset->arg4 )
         {
            last = FALSE;
            break;
         }

         while (count < pReset->arg4)
         {
            obj = create_object( pObjIndex, number_fuzzy(obj_to->level) );
            obj->owner = &str_none[0];
            obj_to_obj( obj, obj_to );
            count++;
            if (pObjIndex->limcount >= pObjIndex->limtotal
            && pObjIndex->limtotal > 0)
            break;
         }
         /* fix object lock state! */
         obj_to->value[1] = obj_to->pIndexData->value[1];
         last = TRUE;
         break;

         case 'G':
         case 'E':
         case 'X':
         if ( ( pObjIndex = get_obj_index( pReset->arg1 ) ) == NULL )
         {
            bug( "Reset_area: 'E', 'X', or 'G': bad vnum %d.", pReset->arg1 );
            continue;
         }

         if ( !last )
         break;

         if ( mob == NULL )
         {
            bug("Reset_area: 'E', 'X', or 'G': null mob for vnum %d.",
            pReset->arg1 );
            last = FALSE;
            break;
         }

         if ( mob->pIndexData->pShop != NULL )
         {
            int olevel = 0, i, j;

            if (!pObjIndex->new_format)
            switch ( pObjIndex->item_type )
            {
               case ITEM_HERB:
               case ITEM_PILL:
               case ITEM_POTION:
               case ITEM_SCROLL:
               olevel = 53;
               for (i = 1; i < 5; i++)
               {
                  if (pObjIndex->value[i] > 0)
                  {
                     for (j = 0; j < MAX_CLASS; j++)
                     {
                        olevel = UMIN(olevel,
                        skill_table[pObjIndex->value[i]].
                        skill_level[j]);
                     }
                  }
               }

               olevel = UMAX(0, (olevel * 3 / 4) - 2);
               break;
               case ITEM_WAND:        olevel = number_range( 10, 20 ); break;
               case ITEM_STAFF:    olevel = number_range( 15, 25 ); break;
               case ITEM_ARMOR:    olevel = number_range(  5, 15 ); break;
               case ITEM_WEAPON:    olevel = number_range(  5, 15 ); break;
               case ITEM_TREASURE:    olevel = number_range( 10, 20 ); break;
            }

            if (pObjIndex->limcount >= pObjIndex->limtotal
            && pObjIndex->limtotal > 0)
            {
               break;
            }
            else
            obj = create_object( pObjIndex, olevel );
            obj->owner = &str_none[0];
            if (obj->pIndexData->limtotal == 0)
            SET_BIT( obj->extra_flags, ITEM_INVENTORY );
         }

         else
         {
            if (pReset->arg2 > 50) /* old format */
            limit = 6;
            else if (pReset->arg2 == -1) /* no limit */
            limit = 999;
            else
            limit = pReset->arg2;

            if (pObjIndex->limcount >= pObjIndex->limtotal
            && pObjIndex->limtotal > 0)
            {
               break;
            }
            else
            obj=create_object(pObjIndex, UMIN(number_fuzzy(level), LEVEL_HERO - 1));
            obj->owner = &str_none[0];
         }
         if (mob->pIndexData->pShop)
         {
            obj_to_keeper(obj, mob);
         }
         else
         {
            obj_to_char(obj, mob);
         }
         if
         (
            pReset->command == 'E' ||
            pReset->command == 'X'
         )
         {
            int slot;
            int slot2;
            switch (pReset->arg3)
            {
               default:
               {
                  slot = pReset->arg3;
                  slot2 = slot;
                  break;
               }
               case (WEAR_FINGER_L):
               case (WEAR_FINGER_R):
               {
                  slot = WEAR_FINGER_L;
                  slot2 = WEAR_FINGER_R;
                  break;
               }
               case (WEAR_NECK_1):
               case (WEAR_NECK_2):
               {
                  slot = WEAR_NECK_1;
                  slot2 = WEAR_NECK_2;
                  break;
               }
               case (WEAR_WRIST_L):
               case (WEAR_WRIST_R):
               {
                  slot = WEAR_WRIST_L;
                  slot2 = WEAR_WRIST_R;
                  break;
               }
               case (WEAR_WIELD):
               case (WEAR_DUAL_WIELD):
               {
                  slot = WEAR_WIELD;
                  slot2 = WEAR_DUAL_WIELD;
                  primary = get_eq_char(mob, slot);
                  secondary = get_eq_char(mob, slot2);
                  if (primary == NULL)
                  {
                     equip_char(mob, obj, slot);
                  }
                  else if (secondary == NULL)
                  {
                     if (pReset->command == 'X')
                     {
                        if (pReset->arg3 == WEAR_DUAL_WIELD)
                        {
                           equip_char(mob, obj, slot2);
                        }
                        else
                        {
                           unequip_char(mob, primary);
                           equip_char(mob, obj, slot);
                           equip_char(mob, primary, slot2);
                        }
                     }
                     else
                     {
                        if (obj->weight <= primary->weight)
                        {
                           equip_char(mob, obj, slot2);
                        }
                        else
                        {
                           unequip_char(mob, primary);
                           equip_char(mob, obj, slot);
                           equip_char(mob, primary, slot2);
                        }
                     }
                  }
                  else
                  {
                     /* BUG */
                     bug("Both slots filled for wield slot: %d", slot);
                     equip_char(mob, obj, slot);
                  }
                  break;
               }
               case (WEAR_EAR_L):
               case (WEAR_EAR_R):
               {
                  slot = WEAR_EAR_L;
                  slot2 = WEAR_EAR_R;
                  break;
               }
               case (WEAR_FEET):
               case (WEAR_FOURHOOVES):
               case (WEAR_HOOVES):
               {
                  if (pReset->command == 'X')
                  {
                     slot = pReset->arg3;
                     slot2 = pReset->arg3;
                  }
                  else if (mob->race == grn_minotaur)
                  {
                     slot = WEAR_HOOVES;
                     slot2 = WEAR_HOOVES;
                     if
                     (
                        !IS_SET(obj->wear_flags, ITEM_WEAR_HOOVES) &&
                        pReset->command == 'E'
                     )
                     {
                        sprintf
                        (
                           log_buf,
                           "reset: [%s] [%s] [%d] missing ITEM_WEAR_HOOVES flag, loading on [%d] [%s] [%s]!",
                           pArea->file_name,
                           obj->short_descr,
                           obj->pIndexData->vnum,
                           mob->pIndexData->vnum,
                           mob->short_descr,
                           race_table[mob->race].name
                        );
                        bug(log_buf, 0);
                     }
                  }
                  else if (mob->race == grn_centaur)
                  {
                     slot = WEAR_FOURHOOVES;
                     slot2 = WEAR_FOURHOOVES;
                     if
                     (
                        !IS_SET(obj->wear_flags, ITEM_WEAR_FOURHOOVES) &&
                        pReset->command == 'E'
                     )
                     {
                        sprintf
                        (
                           log_buf,
                           "reset: [%s] [%s] [%d] missing ITEM_WEAR_FOURHOOVES flag, loading on [%d] [%s] [%s]!",
                           pArea->file_name,
                           obj->short_descr,
                           obj->pIndexData->vnum,
                           mob->pIndexData->vnum,
                           mob->short_descr,
                           race_table[mob->race].name
                        );
                        bug(log_buf, 0);
                     }
                  }
                  else
                  {
                     slot = WEAR_FEET;
                     slot2 = WEAR_FEET;
                     if
                     (
                        !IS_SET(obj->wear_flags, ITEM_WEAR_FEET) &&
                        pReset->command == 'E'
                     )
                     {
                        sprintf
                        (
                           log_buf,
                           "reset: [%s] [%s] [%d] missing ITEM_WEAR_FEET flag, loading on [%d] [%s] [%s]!",
                           pArea->file_name,
                           obj->short_descr,
                           obj->pIndexData->vnum,
                           mob->pIndexData->vnum,
                           mob->short_descr,
                           race_table[mob->race].name
                        );
                        bug(log_buf, 0);
                     }
                  }
                  break;
               }
               case (WEAR_LEGS):
               case (WEAR_FOURLEGS):
               {
                  if (pReset->command == 'X')
                  {
                     slot = pReset->arg3;
                     slot2 = pReset->arg3;
                  }
                  else if (mob->race == grn_centaur)
                  {
                     slot = WEAR_FOURLEGS;
                     slot2 = WEAR_FOURLEGS;
                     if
                     (
                        !IS_SET(obj->wear_flags, ITEM_WEAR_FOURLEGS) &&
                        pReset->command == 'E'
                     )
                     {
                        sprintf
                        (
                           log_buf,
                           "reset: [%s] [%s] [%d] missing ITEM_WEAR_FOURLEGS flag, loading on [%d] [%s] [%s]!",
                           pArea->file_name,
                           obj->short_descr,
                           obj->pIndexData->vnum,
                           mob->pIndexData->vnum,
                           mob->short_descr,
                           race_table[mob->race].name
                        );
                        bug(log_buf, 0);
                     }
                  }
                  else
                  {
                     slot = WEAR_LEGS;
                     slot2 = WEAR_LEGS;
                     if
                     (
                        !IS_SET(obj->wear_flags, ITEM_WEAR_LEGS) &&
                        pReset->command == 'E'
                     )
                     {
                        sprintf
                        (
                           log_buf,
                           "reset: [%s] [%s] [%d] missing ITEM_WEAR_LEGS flag, loading on [%d] [%s] [%s]!",
                           pArea->file_name,
                           obj->short_descr,
                           obj->pIndexData->vnum,
                           mob->pIndexData->vnum,
                           mob->short_descr,
                           race_table[mob->race].name
                        );
                        bug(log_buf, 0);
                     }
                  }
                  break;
               }
               case (WEAR_HEAD):
               case (WEAR_HORNS):
               {
                  if (pReset->command == 'X')
                  {
                     slot = pReset->arg3;
                     slot2 = pReset->arg3;
                  }
                  else if (mob->race == grn_minotaur)
                  {
                     slot = WEAR_HORNS;
                     slot2 = WEAR_HORNS;
                     if
                     (
                        !IS_SET(obj->wear_flags, ITEM_WEAR_HORNS) &&
                        pReset->command == 'E'
                     )
                     {
                        sprintf
                        (
                           log_buf,
                           "reset: [%s] [%s] [%d] missing ITEM_WEAR_HORNS flag, loading on [%d] [%s] [%s]!",
                           pArea->file_name,
                           obj->short_descr,
                           obj->pIndexData->vnum,
                           mob->pIndexData->vnum,
                           mob->short_descr,
                           race_table[mob->race].name
                        );
                        bug(log_buf, 0);
                     }
                  }
                  else
                  {
                     slot = WEAR_HEAD;
                     slot2 = WEAR_HEAD;
                     if
                     (
                        !IS_SET(obj->wear_flags, ITEM_WEAR_HEAD) &&
                        pReset->command == 'E'
                     )
                     {
                        sprintf
                        (
                           log_buf,
                           "reset: [%s] [%s] [%d] missing ITEM_WEAR_HEAD flag, loading on [%d] [%s] [%s]!",
                           pArea->file_name,
                           obj->short_descr,
                           obj->pIndexData->vnum,
                           mob->pIndexData->vnum,
                           mob->short_descr,
                           race_table[mob->race].name
                        );
                        bug(log_buf, 0);
                     }
                  }
                  break;
               }
            }
            if (slot == slot2)
            {
               primary = get_eq_char(mob, slot);
               if (primary != NULL)
               {
                  /* BUG */
                  bug("reset_area: slot already full: %d", slot);
               }
               equip_char(mob, obj, slot);
            }
            else if (slot != WEAR_WIELD)
            {
               primary = get_eq_char(mob, slot);
               secondary = get_eq_char(mob, slot2);
               if (primary == NULL)
               {
                  equip_char(mob, obj, slot);
               }
               else if (secondary == NULL)
               {
                  equip_char(mob, obj, slot2);
               }
               else
               {
                  /* BUG */
                  bug("Both slots filled for wear slot: %d", slot);
                  equip_char(mob, obj, slot);
               }
            }
         }
         last = TRUE;
         break;
         case 'D':
         if ( ( pRoomIndex = get_room_index( pReset->arg1 ) ) == NULL )
         {
            bug( "Reset_area: 'D': bad vnum %d.", pReset->arg1 );
            continue;
         }

         if ( ( pexit = pRoomIndex->exit[pReset->arg2] ) == NULL )
         break;

         switch ( pReset->arg3 )
         {
            case 0:
            REMOVE_BIT( pexit->exit_info, EX_CLOSED );
            REMOVE_BIT( pexit->exit_info, EX_LOCKED );
            break;

            case 1:
            SET_BIT(    pexit->exit_info, EX_CLOSED );
            REMOVE_BIT( pexit->exit_info, EX_LOCKED );
            break;

            case 2:
            SET_BIT(    pexit->exit_info, EX_CLOSED );
            SET_BIT(    pexit->exit_info, EX_LOCKED );
            break;
         }

         last = TRUE;
         break;

         case 'R':
         if ( ( pRoomIndex = get_room_index( pReset->arg1 ) ) == NULL )
         {
            bug( "Reset_area: 'R': bad vnum %d.", pReset->arg1 );
            continue;
         }

         {
            int d0;
            int d1;

            for ( d0 = 0; d0 < pReset->arg2 - 1; d0++ )
            {
               d1                   = number_range( d0, pReset->arg2-1 );
               pexit                = pRoomIndex->exit[d0];
               pRoomIndex->exit[d0] = pRoomIndex->exit[d1];
               pRoomIndex->exit[d1] = pexit;
            }
         }
         break;
      }
   }

   return;
}



/*
* Create an instance of a mobile.
*/
CHAR_DATA* create_mobile(MOB_INDEX_DATA* pMobIndex)
{
   CHAR_DATA *mob;
   int i;
   AFFECT_DATA af;
   int house;
   LIST_DATA* list;
   NODE_DATA* node;
   SKILL_MOD* mod;
   SKILL_MOD* mod_copy;

   mobile_count++;

   if ( pMobIndex == NULL )
   {
      bug( "Create_mobile: NULL pMobIndex.", 0 );
      exit( 1 );
   }

   mob = new_char();

   mob->pIndexData    = pMobIndex;

   mob->name        = pMobIndex->player_name;
   mob->id        = get_mob_id();
   mob->short_descr    = pMobIndex->short_descr;
   mob->long_descr    = pMobIndex->long_descr;
   mob->description    = pMobIndex->description;
   mob->spec_fun    = pMobIndex->spec_fun;
   mob->mprog_target   = NULL;
   mob->prompt        = NULL;
   mob->last_fought    = 0;

   if (pMobIndex->wealth == 0)
   {
      mob->silver = 0;
      mob->gold   = 0;
   }
   else
   {
      long wealth;

      wealth = number_range(pMobIndex->wealth/2, 3 * pMobIndex->wealth/2);
      mob->gold = number_range(wealth/200, wealth/100);
      mob->silver = wealth - (mob->gold * 100);
   }

   if (pMobIndex->new_format)
   /* load in new style */
   {
      /* read from prototype */
      mob->group        = pMobIndex->group;
      mob->act         = pMobIndex->act;
      mob->act2        = pMobIndex->act2;
      mob->affected_by    = pMobIndex->affected_by;
      mob->affected_by2       = pMobIndex->affected_by2;
      mob->alignment        = pMobIndex->alignment;
      mob->level        = pMobIndex->level;
      mob->hitroll        = pMobIndex->hitroll;
      mob->damroll        = pMobIndex->damage[DICE_BONUS];
      mob->max_hit        = dice(pMobIndex->hit[DICE_NUMBER],
      pMobIndex->hit[DICE_TYPE])
      + pMobIndex->hit[DICE_BONUS];
      mob->hit        = mob->max_hit;
      mob->max_mana        = dice(pMobIndex->mana[DICE_NUMBER],
      pMobIndex->mana[DICE_TYPE])
      + pMobIndex->mana[DICE_BONUS];
      mob->mana        = mob->max_mana;
      mob->damage[DICE_NUMBER]= pMobIndex->damage[DICE_NUMBER];
      mob->damage[DICE_TYPE]    = pMobIndex->damage[DICE_TYPE];
      mob->dam_type        = pMobIndex->dam_type;
      if (mob->dam_type == 0)
      switch(number_range(1, 3))
      {
         case (1): mob->dam_type = 3;        break;  /* slash */
         case (2): mob->dam_type = 7;        break;  /* pound */
         case (3): mob->dam_type = 11;       break;  /* pierce */
      }
      for (i = 0; i < 4; i++)
      mob->armor[i]    = pMobIndex->ac[i];
      mob->off_flags        = pMobIndex->off_flags;
      mob->imm_flags        = pMobIndex->imm_flags;
      mob->res_flags        = pMobIndex->res_flags;
      mob->vuln_flags        = pMobIndex->vuln_flags;
      mob->start_pos        = pMobIndex->start_pos;
      mob->default_pos    = pMobIndex->default_pos;
      mob->sex        = pMobIndex->sex;
      if (mob->sex == 3) /* random sex */
      mob->sex = number_range(1, 2);
      mob->race        = pMobIndex->race;
      mob->form        = pMobIndex->form;
      mob->parts        = pMobIndex->parts;
      mob->size        = pMobIndex->size;
      mob->material        = /* str_dup(pMobIndex->material); */ pMobIndex->material;

      /* computed on the spot */

      for (i = 0; i < MAX_STATS; i ++)
      mob->perm_stat[i] = UMIN(25, 11 + mob->level/4);

      if (IS_SET(mob->act, ACT_WARRIOR))
      {
         mob->perm_stat[STAT_STR] += 3;
         mob->perm_stat[STAT_INT] -= 1;
         mob->perm_stat[STAT_CON] += 2;
      }

      if (IS_SET(mob->act, ACT_THIEF))
      {
         mob->perm_stat[STAT_DEX] += 3;
         mob->perm_stat[STAT_INT] += 1;
         mob->perm_stat[STAT_WIS] -= 1;
      }

      if (IS_SET(mob->act, ACT_CLERIC))
      {
         mob->perm_stat[STAT_WIS] += 3;
         mob->perm_stat[STAT_DEX] -= 1;
         mob->perm_stat[STAT_STR] += 1;
      }

      if (IS_SET(mob->act, ACT_MAGE))
      {
         mob->perm_stat[STAT_INT] += 3;
         mob->perm_stat[STAT_STR] -= 1;
         mob->perm_stat[STAT_DEX] += 1;
      }

      if (IS_SET(mob->off_flags, OFF_FAST))
      mob->perm_stat[STAT_DEX] += 2;

      mob->perm_stat[STAT_STR] += mob->size - SIZE_MEDIUM;
      mob->perm_stat[STAT_CON] += (mob->size - SIZE_MEDIUM) / 2;

      /* let's get some spell action */
      if (IS_AFFECTED(mob, AFF_SANCTUARY))
      {
         af.where     = TO_AFFECTS;
         af.level     = mob->level;
         af.duration  = -1;
         af.location  = APPLY_NONE;
         af.modifier  = 0;
         af.bitvector = AFF_SANCTUARY;
         if (is_clergy(mob))
         {
            af.type = gsn_sanctuary;
         }
         else
         {
            af.type = gsn_chromatic_shield;
         }
         affect_to_char(mob, &af);
      }

      if (IS_AFFECTED(mob, AFF_HASTE))
      {
         af.where     = TO_AFFECTS;
         af.type      = gsn_haste;
         af.level     = mob->level;
         af.duration  = -1;
         af.location  = APPLY_DEX;
         af.modifier  = 1 + (mob->level >= 18) + (mob->level >= 25) +
         (mob->level >= 32);
         af.bitvector = AFF_HASTE;
         affect_to_char( mob, &af );
      }

      if (IS_AFFECTED(mob, AFF_PROTECT_EVIL))
      {
         af.where     = TO_AFFECTS;
         af.type      = gsn_protection_evil;
         af.level     = mob->level;
         af.duration     = -1;
         af.location     = APPLY_SAVES;
         af.modifier     = -1;
         af.bitvector = AFF_PROTECT_EVIL;
         affect_to_char(mob, &af);
      }

      if (IS_AFFECTED(mob, AFF_PROTECT_GOOD))
      {
         af.where     = TO_AFFECTS;
         af.type      = gsn_protection_good;
         af.level     = mob->level;
         af.duration  = -1;
         af.location  = APPLY_SAVES;
         af.modifier  = -1;
         af.bitvector = AFF_PROTECT_GOOD;
         affect_to_char(mob, &af);
      }
   }
   else /* read in old format and convert */
   {
      mob->act        = pMobIndex->act;
      mob->act2        = pMobIndex->act2;
      mob->affected_by    = pMobIndex->affected_by;
      mob->affected_by2       = pMobIndex->affected_by2;
      mob->alignment          = pMobIndex->alignment;
      mob->level        = pMobIndex->level;
      mob->hitroll        = pMobIndex->hitroll;
      mob->damroll        = 0;
      mob->max_hit        = mob->level * 8 + number_range(
      mob->level * mob->level/4,
      mob->level * mob->level);
      mob->max_hit *= .9;
      mob->hit        = mob->max_hit;
      mob->max_mana        = 100 + dice(mob->level, 10);
      mob->mana        = mob->max_mana;
      switch(number_range(1, 3))
      {
         case (1): mob->dam_type = 3;     break;  /* slash */
         case (2): mob->dam_type = 7;    break;  /* pound */
         case (3): mob->dam_type = 11;    break;  /* pierce */
      }
      for (i = 0; i < 3; i++)
      mob->armor[i]    = interpolate(mob->level, 100,-100);
      mob->armor[3]        = interpolate(mob->level, 100, 0);
      mob->race        = pMobIndex->race;
      mob->off_flags        = pMobIndex->off_flags;
      mob->imm_flags        = pMobIndex->imm_flags;
      mob->res_flags        = pMobIndex->res_flags;
      mob->vuln_flags        = pMobIndex->vuln_flags;
      mob->start_pos        = pMobIndex->start_pos;
      mob->default_pos    = pMobIndex->default_pos;
      mob->sex        = pMobIndex->sex;
      mob->form        = pMobIndex->form;
      mob->parts        = pMobIndex->parts;
      mob->size        = SIZE_MEDIUM;
      mob->material        = "";

      for (i = 0; i < MAX_STATS; i ++)
      mob->perm_stat[i] = 11 + mob->level/4;
   }

   mob->position = mob->start_pos;

   for (house = 0; house < MAX_HOUSE; house++)
   {
      if (pMobIndex->vnum == house_table[house].vnum_guardian)
      mob->house = house;
   }

   if (pMobIndex->vnum == MOB_VNUM_CRUSADER)
   {
      AFFECT_DATA af;
      mob->house = HOUSE_CRUSADER;
      af.where = TO_AFFECTS;
      af.duration = -1;
      af.location = 0;
      af.modifier = 0;
      af.level = 60;
      af.type = gsn_steel_nerves;
      af.bitvector = 0;
      affect_to_char(mob, &af);
   }
   unstash_valuables(mob);
   /* link the mob to the world list */
   mob->next        = char_list;
   char_list        = mob;
   pMobIndex->count++;

   if (IS_SET(mob->affected_by2, AFF_WIZI))
   {
      mob->invis_level = 52;
   }
   if (pMobIndex->skill_mods)
   {
      if (mob->skill_mods == NULL)
      {
         mob->skill_mods = new_list();
      }
      list = mob->skill_mods;
      for
      (
         node = pMobIndex->skill_mods->first;
         node;
         node = node->next
      )
      {
         mod = (SKILL_MOD*)node->data;
         mod_copy = new_skill_mod(mod->skill_percent, mod->sn);
         add_node(mod_copy, list);
      }
   }
   return mob;
}

void clone_mobile(CHAR_DATA* parent, CHAR_DATA* clone)
{
   clone_character(parent, clone, FALSE);
}

/* duplicate a character exactly -- except inventory */
void clone_character(CHAR_DATA* parent, CHAR_DATA* clone, bool pc_clone)
{
   int i;
   AFFECT_DATA* paf;
   char buf[MAX_INPUT_LENGTH];

   if
   (
      parent == NULL ||
      clone == NULL ||
      (
         !IS_NPC(parent) &&
         !pc_clone
      )
   )
   {
      return;
   }

   /* Remove all affects */
   while (clone->affected)
   {
      affect_strip(clone, clone->affected->type);
   }
   /* now add the affects */
   for (paf = parent->affected; paf != NULL; paf = paf->next)
   {
      /* Do not copy brand affects and other restricted abilities */
      if
      (
         skill_table[paf->type].spells.copy_clone ||
         !pc_clone
      )
      {
         affect_to_char_version(clone, paf, AFFECT_CURRENT_VERSION);
      }
   }
   /* start fixing values */
   free_string(clone->name);
   clone->name             = str_dup(parent->name);
   clone->version          = parent->version;
   free_string(clone->short_descr);
   if (IS_NPC(parent))
   {
      clone->short_descr   = str_dup(parent->short_descr);
   }
   else
   {
      clone->short_descr = str_dup(parent->name);
   }
   free_string(clone->long_descr);
   if (IS_NPC(parent))
   {
      clone->long_descr    = str_dup(parent->long_descr);
   }
   else
   {
      sprintf(buf, "%s is here.\n\r", parent->name);
      clone->long_descr    = str_dup(buf);
   }
   free_string(clone->description);
   clone->description      = str_dup(parent->description);
   clone->group            = parent->group;
   clone->sex              = parent->sex;
   clone->class            = parent->class;
   clone->race             = parent->race;
   clone->level            = parent->level;
   clone->trust            = 0;
   clone->timer            = parent->timer;
   clone->daze             = parent->daze;
   clone->wait             = parent->wait;
   clone->hit              = parent->hit;
   clone->max_hit          = parent->max_hit;
   clone->mana             = parent->mana;
   clone->max_mana         = parent->max_mana;
   clone->move             = parent->move;
   clone->max_move         = parent->max_move;
   clone->gold             = parent->gold;
   clone->silver           = parent->silver;
   clone->exp              = parent->exp;
   if (IS_NPC(parent))
   {
      clone->act           = parent->act;
      clone->act2          = parent->act2;
   }
   else
   {
      clone->act           = 0;
      clone->act2          = 0;
   }
   clone->comm             = parent->comm;
   clone->comm2            = parent->comm2;
   clone->imm_flags        = parent->imm_flags;
   clone->res_flags        = parent->res_flags;
   clone->vuln_flags       = parent->vuln_flags;
   clone->invis_level      = parent->invis_level;
   clone->affected_by      = parent->affected_by;
   clone->position         = parent->position;
   clone->practice         = parent->practice;
   clone->train            = parent->train;
   clone->saving_throw     = parent->saving_throw;
   clone->sight            = parent->sight;
   clone->spell_power      = parent->spell_power;
   clone->holy_power       = parent->holy_power;
   clone->saving_maledict  = parent->saving_maledict;
   clone->saving_spell     = parent->saving_spell;
   clone->saving_breath    = parent->saving_breath;
   clone->saving_transport = parent->saving_transport;
   clone->alignment        = parent->alignment;
   clone->hitroll          = parent->hitroll;
   clone->damroll          = parent->damroll;
   clone->wimpy            = parent->wimpy;
   clone->form             = parent->form;
   clone->parts            = parent->parts;
   clone->size             = parent->size;
   free_string(clone->material);
   if (parent->material != NULL)
   {
      clone->material         = str_dup(parent->material);
   }
   else
   {
      clone->material         = str_dup("");
   }
   clone->off_flags        = parent->off_flags;
   clone->dam_type         = parent->dam_type;
   clone->start_pos        = parent->start_pos;
   clone->default_pos      = parent->default_pos;
   clone->spec_fun         = parent->spec_fun;

   for (i = 0; i < 4; i++)
   {
      clone->armor[i] = parent->armor[i];
   }

   for (i = 0; i < MAX_STATS; i++)
   {
      clone->perm_stat[i]  = parent->perm_stat[i];
      clone->mod_stat[i]   = parent->mod_stat[i];
   }

   for (i = 0; i < 3; i++)
   {
      clone->damage[i] = parent->damage[i];
   }

   if
   (
      IS_AFFECTED(clone, AFF_SANCTUARY) &&
      !is_affected(clone, gsn_sanctuary) &&
      !is_affected(clone, gsn_chromatic_shield)
   )
   {
      paf = new_affect();
      paf->where     = TO_AFFECTS;
      paf->level     = clone->level;
      paf->duration  = -1;
      paf->bitvector = AFF_SANCTUARY;
      if (is_clergy(clone))
      {
         paf->type = gsn_sanctuary;
      }
      else
      {
         paf->type = gsn_chromatic_shield;
      }
      affect_to_char_1(clone, paf);
      free_affect(paf);
   }
}




/*
* Create an instance of an object.
*/
OBJ_DATA *create_object( OBJ_INDEX_DATA *pObjIndex, int level )
{
   AFFECT_DATA *paf;
   OBJ_DATA *obj;
   int mooncount;
   int i;

   if ( pObjIndex == NULL )
   {
      bug( "Create_object: NULL pObjIndex.", 0 );
      return NULL;
      exit( 1 );
   }

   obj = new_obj();

   obj->pIndexData    = pObjIndex;
   obj->in_room    = NULL;
   obj->enchanted    = FALSE;

   if (pObjIndex->new_format)
   obj->level = pObjIndex->level;
   else
   obj->level        = UMAX(0, level);
   obj->wear_loc    = -1;

   obj->name        = pObjIndex->name;
   obj->short_descr    = pObjIndex->short_descr;
   obj->description    = pObjIndex->description;
   obj->material    = str_dup(pObjIndex->material);
   obj->item_type    = pObjIndex->item_type;
   obj->extra_flags    = pObjIndex->extra_flags;
   obj->extra_flags2   = pObjIndex->extra_flags2;
   obj->wear_flags    = pObjIndex->wear_flags;
   obj->value[0]    = pObjIndex->value[0];
   obj->value[1]    = pObjIndex->value[1];
   obj->value[2]    = pObjIndex->value[2];
   obj->value[3]    = pObjIndex->value[3];
   obj->value[4]    = pObjIndex->value[4];
   for (mooncount = 0; mooncount < MAX_MOONS; mooncount++)
   {
      obj->moonphases[mooncount] = pObjIndex->moonphases[mooncount];
   }
   obj->weight        = pObjIndex->weight;
   obj->newcost        = 0;
   obj->condition     = pObjIndex->condition;
   obj->mount_specs    = pObjIndex->mount_specs;
   obj->message_list    = NULL;

   if (level == -1 || pObjIndex->new_format)
   obj->cost    = pObjIndex->cost;
   else
   obj->cost    = number_fuzzy( 10 )
   * number_fuzzy( level ) * number_fuzzy( level );

   /*
   * Mess with object properties.
   */
   switch ( obj->item_type )
   {
      default:
      bug( "Read_object: vnum %d bad type.", pObjIndex->vnum );
      break;

      case ITEM_LIGHT:
      if (obj->value[2] == 999)
      obj->value[2] = -1;
      break;

      case (ITEM_BOOK):
      {
         int cnt;

         obj->book_info = new_bookdata();
         for (cnt = 0; cnt < BOOK_MAX_PAGES; cnt++)
         {
            if (pObjIndex->book_info->page[cnt] == NULL)
            {
               obj->book_info->page[cnt] = NULL;
            }
            else
            {
               obj->book_info->page[cnt] = str_dup
               (
                  pObjIndex->book_info->page[cnt]
               );
            }
            if (pObjIndex->book_info->title[cnt] == NULL)
            {
               obj->book_info->title[cnt] = NULL;
            }
            else
            {
               obj->book_info->title[cnt] = str_dup
               (
                  pObjIndex->book_info->title[cnt]
               );
            }
         }
         break;
      }
      case (ITEM_PEN):
      case (ITEM_INKWELL):
      case ITEM_SPELLBOOK:
      case ITEM_FURNITURE:
      case ITEM_TRASH:
      case ITEM_CONTAINER:
      case ITEM_DRINK_CON:
      case ITEM_KEY:
      case ITEM_FOOD:
      case ITEM_BOAT:
      case ITEM_CORPSE_NPC:
      case ITEM_CORPSE_PC:
      case ITEM_FOUNTAIN:
      case ITEM_MAP:
      case ITEM_CLOTHING:
      case ITEM_PORTAL_NEW:
      if (!pObjIndex->new_format)
      obj->cost /= 5;
      break;

      case ITEM_TREASURE:
      case ITEM_WARP_STONE:
      case ITEM_ROOM_KEY:
      case ITEM_GEM:
      case ITEM_JEWELRY:
      case ITEM_TOOL:
      case ITEM_SKELETON:
      break;

      case ITEM_JUKEBOX:
      for (i = 0; i < 5; i++)
      obj->value[i] = -1;
      break;

      case ITEM_SCROLL:
      if (level != -1 && !pObjIndex->new_format)
      obj->value[0]    = number_fuzzy( obj->value[0] );
      break;

      case ITEM_WAND:
      case ITEM_STAFF:
      if (level != -1 && !pObjIndex->new_format)
      {
         obj->value[0]    = number_fuzzy( obj->value[0] );
         obj->value[1]    = number_fuzzy( obj->value[1] );
         obj->value[2]    = obj->value[1];
      }
      if (!pObjIndex->new_format)
      obj->cost *= 2;
      break;

      case ITEM_WEAPON:
      if (level != -1 && !pObjIndex->new_format)
      {
         obj->value[1] = number_fuzzy( number_fuzzy( 1 * level / 4 + 2 ) );
         obj->value[2] = number_fuzzy( number_fuzzy( 3 * level / 4 + 6 ) );
      }
      break;

      case ITEM_ARMOR:
      if (level != -1 && !pObjIndex->new_format)
      {
         obj->value[0]    = number_fuzzy( level / 5 + 3 );
         obj->value[1]    = number_fuzzy( level / 5 + 3 );
         obj->value[2]    = number_fuzzy( level / 5 + 3 );
      }
      break;

      case ITEM_POTION:
      case ITEM_HERB:
      case ITEM_PILL:
      if (level != -1 && !pObjIndex->new_format)
      obj->value[0] = number_fuzzy( number_fuzzy( obj->value[0] ) );
      break;

      case ITEM_MONEY:
      if (!pObjIndex->new_format)
      obj->value[0]    = obj->cost;
      break;
   }

   for (paf = pObjIndex->affected; paf != NULL; paf = paf->next)
   if ( paf->location == APPLY_SPELL_AFFECT )
   affect_to_obj(obj, paf);

   obj->owner = &str_none[0];
   obj->next        = object_list;
   object_list        = obj;
   pObjIndex->limcount += 1;
   pObjIndex->in_game_count += 1;

   if (pObjIndex->quitouts == TRUE)
   obj->timer = pObjIndex->condition;

   return obj;
}

/* duplicate an object exactly -- except contents */
void clone_object(OBJ_DATA *parent, OBJ_DATA *clone)
{
   int i;
   AFFECT_DATA* paf;
   EXTRA_DESCR_DATA *ed,*ed_new;

   if (parent == NULL || clone == NULL)
   return;

   /* start fixing the object */
   clone->name     = str_dup(parent->name);
   clone->short_descr     = str_dup(parent->short_descr);
   clone->description    = str_dup(parent->description);
   clone->item_type    = parent->item_type;
   clone->extra_flags    = parent->extra_flags;
   clone->extra_flags2 = parent->extra_flags2;
   clone->wear_flags    = parent->wear_flags;
   clone->weight    = parent->weight;
   clone->cost        = parent->cost;
   clone->level    = parent->level;
   clone->condition    = parent->condition;
   clone->material    = str_dup(parent->material);
   clone->timer    = parent->timer;
   clone->mount_specs    = parent->mount_specs;
   clone->message_list = NULL;

   for (i = 0;  i < 5; i ++)
   clone->value[i]    = parent->value[i];

   /* affects */
   clone->enchanted    = parent->enchanted;
   for (paf = parent->affected; paf != NULL; paf = paf->next)
   {
      affect_to_obj(clone, paf);
   }

   /* extended desc */
   for (ed = parent->extra_descr; ed != NULL; ed = ed->next)
   {
      ed_new                  = new_extra_descr();
      ed_new->keyword        = str_dup( ed->keyword);
      ed_new->description     = str_dup( ed->description );
      ed_new->next               = clone->extra_descr;
      clone->extra_descr      = ed_new;
   }

}



/*
* Clear a new character.
*/
void clear_char( CHAR_DATA *ch )
{
   static CHAR_DATA ch_zero;
   int i;

   *ch                = ch_zero;
   ch->name            = &str_empty[0];
   ch->short_descr        = &str_empty[0];
   ch->long_descr        = &str_empty[0];
   ch->description        = &str_empty[0];
   ch->prompt                  = &str_empty[0];
   ch->logon            = current_time;
   ch->lines            = PAGELEN;
   for (i = 0; i < 4; i++)
   ch->armor[i]        = 100;
   ch->position        = POS_STANDING;
   ch->hit            = 20;
   ch->max_hit            = 20;
   ch->mana            = 100;
   ch->max_mana        = 100;
   ch->move            = 100;
   ch->max_move        = 100;
   ch->on            = NULL;
   return;
}

/*
* Get an extra description from a list.
*/
char *get_extra_descr( const char *name, EXTRA_DESCR_DATA *ed )
{
   for ( ; ed != NULL; ed = ed->next )
   {
      if ( is_name( (char *) name, ed->keyword ) )
      return ed->description;
   }
   return NULL;
}


#if MAX_KEY_HASH >= MAX_VNUM
/*
   Translates mob virtual number to its mob index struct.
   No hash, since the hashes are higher than the vnums
*/
MOB_INDEX_DATA* get_mob_index(int vnum)
{
   if
   (
      vnum < 0 ||
      vnum >= MAX_VNUM
   )
   {
      bug("Get_mob_index: bad vnum %d.", vnum);
      if (fBootDb)
      {
         exit(1);
      }
      return NULL;
   }
   if (!fBootDb)
   {
      return mob_index_hash[vnum];
   }
   if (fBootDb)
   {
      if (mob_index_hash[vnum])
      {
         return mob_index_hash[vnum];
      }
      bug("Get_mob_index: bad vnum %d.", vnum);
      exit(1);
   }
   return NULL;
}

/*
   Translates mob virtual number to its obj index struct.
   No hash, since the hashes are higher than the vnums
*/
OBJ_INDEX_DATA* get_obj_index(int vnum)
{
   if
   (
      vnum < 0 ||
      vnum >= MAX_VNUM
   )
   {
      bug("Get_obj_index: bad vnum %d.", vnum);
      if (fBootDb)
      {
         exit(1);
      }
      return NULL;
   }
   if (!fBootDb)
   {
      return obj_index_hash[vnum];
   }
   if (fBootDb)
   {
      if (obj_index_hash[vnum])
      {
         return obj_index_hash[vnum];
      }
      bug("Get_obj_index: bad vnum %d.", vnum);
      exit(1);
   }
   return NULL;
}

/*
   Translates room virtual number to its room index struct.
   No hash, since the hashes are higher than the vnums
*/
ROOM_INDEX_DATA* get_room_index(int vnum)
{
   if
   (
      vnum < 0 ||
      vnum >= MAX_VNUM
   )
   {
      bug("Get_room_index: bad vnum %d.", vnum);
      if (fBootDb)
      {
         exit(1);
      }
      return NULL;
   }
   if (!fBootDb)
   {
      return room_index_hash[vnum];
   }
   if (fBootDb)
   {
      if (room_index_hash[vnum])
      {
         return room_index_hash[vnum];
      }
      bug("Get_room_index: bad vnum %d.", vnum);
      exit(1);
   }
   return NULL;
}
#else  /* #if MAX_KEY_HASH > MAX_VNUM */
/*
   Translates mob virtual number to its mob index struct.
   Hash table lookup.
*/
MOB_INDEX_DATA* get_mob_index(int vnum)
{
   MOB_INDEX_DATA* pMobIndex;
   int end = vnum % MAX_KEY_HASH;

   if
   (
      vnum < 0 ||
      vnum >= MAX_VNUM
   )
   {
      bug("Get_mob_index: bad vnum %d.", vnum);
      if (fBootDb)
      {
         exit(1);
      }
      return NULL;
   }
   for
   (
      pMobIndex  = mob_index_hash[end];
      (
         pMobIndex != NULL &&
         pMobIndex->vnum % MAX_KEY_HASH == end
      );
      pMobIndex  = pMobIndex->next
   )
   {
      if (pMobIndex->vnum == vnum)
      {
         return pMobIndex;
      }
   }

   if (fBootDb)
   {
      bug("Get_mob_index: bad vnum %d.", vnum);
      exit(1);
   }

   return NULL;
}

/*
   Translates mob virtual number to its obj index struct.
   Hash table lookup.
*/
OBJ_INDEX_DATA* get_obj_index(int vnum)
{
   OBJ_INDEX_DATA *pObjIndex;
   int end = vnum % MAX_KEY_HASH;

   if
   (
      vnum < 0 ||
      vnum >= MAX_VNUM
   )
   {
      bug("Get_obj_index: bad vnum %d.", vnum);
      if (fBootDb)
      {
         exit(1);
      }
      return NULL;
   }
   for
   (
      pObjIndex  = obj_index_hash[end];
      (
         pObjIndex != NULL &&
         pObjIndex->vnum % MAX_KEY_HASH == end
      );
      pObjIndex  = pObjIndex->next
   )
   {
      if (pObjIndex->vnum == vnum)
      {
         return pObjIndex;
      }
   }
   if (fBootDb)
   {
      bug("Get_obj_index: bad vnum %d.", vnum);
      exit(1);
   }
   return NULL;
}

/*
   Translates mob virtual number to its room index struct.
   Hash table lookup.
*/
ROOM_INDEX_DATA* get_room_index(int vnum)
{
   ROOM_INDEX_DATA *pRoomIndex;
   int end = vnum % MAX_KEY_HASH;

   if
   (
      vnum < 0 ||
      vnum >= MAX_VNUM
   )
   {
      bug("Get_room_index: bad vnum %d.", vnum);
      if (fBootDb)
      {
         exit(1);
      }
      return NULL;
   }
   for
   (
      pRoomIndex = room_index_hash[end];
      (
         pRoomIndex != NULL &&
         pRoomIndex->vnum % MAX_KEY_HASH == end
      );
      pRoomIndex  = pRoomIndex->next
   )
   {
      if (pRoomIndex->vnum == vnum)
      {
         return pRoomIndex;
      }
   }

   if (fBootDb)
   {
      bug("Get_room_index: bad vnum %d.", vnum);
      exit(1);
   }

   return NULL;
}
#endif  /* #if MAX_KEY_HASH >= MAX_VNUM */

void add_id_hash(CHAR_DATA* ch)
{
   long slot;

   if
   (
      !ch ||
      !ch->valid ||
      !ch->on_line ||
      ch->id < 1  /* sanity check */
   )
   {
      return;
   }
   slot = ch->id % MAX_KEY_HASH;
   if (IS_NPC(ch))
   {
      /* NPC list */
      if (!npc_id_hash_table[slot])
      {
         /* Create new list */
         npc_id_hash_table[slot] = new_list();
      }
      /* add to list */
      add_node(ch, npc_id_hash_table[slot]);
   }
   else
   {
      /* PC list */
      if (!pc_id_hash_table[slot])
      {
         /* Create new list */
         pc_id_hash_table[slot] = new_list();
      }
      /* add to list */
      add_node(ch, pc_id_hash_table[slot]);
   }
}

void remove_id_hash(CHAR_DATA* ch)
{
   long slot;

   if
   (
      !ch ||
      !ch->valid ||
      !ch->on_line ||
      ch->id < 1  /* sanity check */
   )
   {
      return;
   }
   slot = ch->id % MAX_KEY_HASH;
   if (IS_NPC(ch))
   {
      /* NPC list */
      remove_node_for(ch, npc_id_hash_table[slot]);
   }
   else
   {
      /* PC list */
      remove_node_for(ch, pc_id_hash_table[slot]);
   }
}

CHAR_DATA* id2name(long id, bool allow_npc)
{
   NODE_DATA* node;
   CHAR_DATA* ch;
   LIST_DATA* list;

   if (id < 1)
   {
      return NULL;
   }
   /* Find a PC */
   if
   (
      (
         list = pc_id_hash_table[id % MAX_KEY_HASH]
      ) != NULL
   )
   {
      for
      (
         node = list->first;
         node;
         node = node->next
      )
      {
         ch = (CHAR_DATA*)node->data;
         if
         (
            ch &&
            id == ch->id &&
            ch->valid &&
            ch->on_line &&
            ch->in_room &&
            (
               !ch->desc ||
               ch->desc->connected == CON_PLAYING
            )
         )
         {
            return ch;
         }
      }
   }
   if
   (
      allow_npc &&
      (
         list = npc_id_hash_table[id % MAX_KEY_HASH]
      ) != NULL
   )
   {
      /* Find an NPC */
      for
      (
         node = list->first;
         node;
         node = node->next
      )
      {
         ch = (CHAR_DATA*)node->data;
         if
         (
            ch &&
            id == ch->id &&
            ch->valid &&
            ch->on_line &&
            ch->in_room
         )
         {
            return ch;
         }
      }
   }
   return NULL;
}

CHAR_DATA* id2name_room(long id, ROOM_INDEX_DATA* room, bool allow_npc)
{
   NODE_DATA* node;
   CHAR_DATA* ch;
   LIST_DATA* list;

   if
   (
      id < 1 ||
      !room
   )
   {
      return NULL;
   }
   /* Find a PC */
   if
   (
      (
         list = pc_id_hash_table[id % MAX_KEY_HASH]
      ) != NULL
   )
   {
      for
      (
         node = list->first;
         node;
         node = node->next
      )
      {
         ch = (CHAR_DATA*)node->data;
         if
         (
            ch &&
            id == ch->id &&
            ch->valid &&
            ch->on_line &&
            ch->in_room == room &&
            (
               !ch->desc ||
               ch->desc->connected == CON_PLAYING
            )
         )
         {
            return ch;
         }
      }
   }
   if
   (
      allow_npc &&
      (
         list = npc_id_hash_table[id % MAX_KEY_HASH]
      ) != NULL
   )
   {
      /* Find an NPC */
      for
      (
         node = list->first;
         node;
         node = node->next
      )
      {
         ch = (CHAR_DATA*)node->data;
         if
         (
            ch &&
            id == ch->id &&
            ch->valid &&
            ch->on_line &&
            ch->in_room == room
         )
         {
            return ch;
         }
      }
   }
   return NULL;
}

MPROG_CODE *get_mprog_index( int vnum )
{
   MPROG_CODE *prg;
   for ( prg = mprog_list; prg; prg = prg->next )
   {
      if ( prg->vnum == vnum )
      return( prg );
   }
   return NULL;
}


/*
* Read a letter from a file.
*/
char fread_letter( FILE *fp )
{
   char c;

   do
   {
      c = getc( fp );
   }
   while ( isspace(c) );

   return c;
}



/*
* Read a number from a file.
*/
int fread_number( FILE *fp )
{
   int number;
   bool sign;
   char c;

   do
   {
      c = getc( fp );
   }
   while ( isspace(c) );

   number = 0;

   sign   = FALSE;
   if ( c == '+' )
   {
      c = getc( fp );
   }
   else if ( c == '-' )
   {
      sign = TRUE;
      c = getc( fp );
   }

   if ( !isdigit(c) )
   {
      bug( "Fread_number: bad format.", 0 );
      exit( 1 );
   }

   while ( isdigit(c) )
   {
      number = number * 10 + c - '0';
      c      = getc( fp );
   }

   if ( sign )
   number = 0 - number;

   if ( c == '|' )
   number += fread_number( fp );
   else if ( c != ' ' )
   ungetc( c, fp );

   return number;
}

long fread_flag( FILE *fp)
{
   int number;
   char c;
   bool negative = FALSE;
   int temp;

   do
   {
      c = getc(fp);
   }
   while ( isspace(c));

   if (c == '-')
   {
      negative = TRUE;
      c = getc(fp);
   }

   number = 0;

   if (!isdigit(c))
   {
      while (('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z'))
      {
         if (!IS_SET(number, (temp=flag_convert(c))))
         {
            number += temp;
         }
         else
         {
            sprintf
            (
               log_buf,
               "fread_flag: Flag already set! %c",
               c
            );
            bug(log_buf, 0);
         }
         c = getc(fp);
      }
   }

   while (isdigit(c))
   {
      number = number * 10 + c - '0';
      c = getc(fp);
   }

   if (c == '|')
   number += fread_flag(fp);

   else if  ( c != ' ')
   ungetc(c, fp);

   if (negative)
   return -1 * number;

   return number;
}

long read_flag( char *flags )
{
   long number=0, temp=0;
   char c;
   int i;

   for ( i=0; flags[i] != '\0'; i++ )
   {
      c = flags[i];

      if ( isdigit(c) )
      {
         bug( "Numbers in flag passed to read_flag.", 0 );
         continue;
      } else
      if ( (c < 'A' && c > 'Z') && (c < 'a' && c > 'z') )
      {
         bug( "Non-letter in flag passed to read_flag.", 0 );
         continue;
      }

      if ( !IS_SET(number, (temp=flag_convert(c))) )
      number += temp;
   }

   return number;
}

long flag_convert(char letter )
{
   long bitsum = 0;

   if ('A' <= letter && letter <= 'Z')
   {
      bitsum = 1 << (letter - 'A');
   }
   else if ('a' <= letter && letter <= 'z')
   {
      /* 2^26 << letter - 'a' */
      bitsum = 67108864 << (letter - 'a');
   }

   return bitsum;
}




/*
* Read and allocate space for a string from a file.
* These strings are read-only and shared.
* Strings are hashed:
*   each string prepended with hash pointer to prev string,
*   hash code is simply the string length.
*   this function takes 40% to 50% of boot-up time.
*/
char *fread_string( FILE *fp )
{
   char *plast;
   char c;

   plast = top_string + sizeof(char *);
   if ( plast > &string_space[MAX_STRING - MAX_STRING_LENGTH] )
   {
      bug( "Fread_string: MAX_STRING %d exceeded.", MAX_STRING );
      exit( 1 );
   }

   /*
   * Skip blanks.
   * Read first char.
   */
   do
   {
      c = getc( fp );
   }
   while ( isspace(c) );

   if ( ( *plast++ = c ) == '~' )
   return &str_empty[0];

   for ( ;; )
   {
      /*
      * Back off the char type lookup,
      *   it was too dirty for portability.
      *   -- Furey
      */

      switch ( *plast = getc(fp) )
      {
         default:
         plast++;
         break;

         case EOF:
         /* temp fix */
         bug( "Fread_string: EOF", 0 );
         return NULL;
         /* exit( 1 ); */
         break;

         case '\n':
         plast++;
         *plast++ = '\r';
         break;

         case '\r':
         break;

         case '~':
         plast++;
         {
            union
            {
               char *    pc;
               char    rgc[sizeof(char *)];
            } u1;
            unsigned int ic;
            int iHash;
            char *pHash;
            char *pHashPrev;
            char *pString;

            plast[-1] = '\0';
            iHash     = UMIN( MAX_KEY_HASH - 1, plast - 1 - top_string );
            for ( pHash = string_hash[iHash]; pHash; pHash = pHashPrev )
            {
               for ( ic = 0; ic < sizeof(char *); ic++ )
               u1.rgc[ic] = pHash[ic];
               pHashPrev = u1.pc;
               pHash    += sizeof(char *);

               if ( top_string[sizeof(char *)] == pHash[0]
               &&   !strcmp( top_string+sizeof(char *)+1, pHash+1 ) )
               return pHash;
            }

            if ( fBootDb )
            {
               pString        = top_string;
               top_string        = plast;
               u1.pc        = string_hash[iHash];
               for ( ic = 0; ic < sizeof(char *); ic++ )
               pString[ic] = u1.rgc[ic];
               string_hash[iHash]    = pString;

               nAllocString += 1;
               sAllocString += top_string - pString;
               return pString + sizeof(char *);
            }
            else
            {
               return str_dup( top_string + sizeof(char *) );
            }
         }
      }
   }
}

char *fread_string_eol( FILE *fp )
{
   static bool char_special[256-EOF];
   char *plast;
   char c;

   if ( char_special[EOF-EOF] != TRUE )
   {
      char_special[EOF -  EOF] = TRUE;
      char_special['\n' - EOF] = TRUE;
      char_special['\r' - EOF] = TRUE;
   }

   plast = top_string + sizeof(char *);
   if ( plast > &string_space[MAX_STRING - MAX_STRING_LENGTH] )
   {
      bug( "Fread_string: MAX_STRING %d exceeded.", MAX_STRING );
      exit( 1 );
   }

   /*
   * Skip blanks.
   * Read first char.
   */
   do
   {
      c = getc( fp );
   }
   while ( isspace(c) );

   if ( ( *plast++ = c ) == '\n')
   return &str_empty[0];

   for ( ;; )
   {
      if ( !char_special[ ( *plast++ = getc( fp ) ) - EOF ] )
      continue;

      switch ( plast[-1] )
      {
         default:
         break;

         case EOF:
         bug( "Fread_string_eol  EOF", 0 );
         exit( 1 );
         break;

         case '\n':  case '\r':
         {
            union
            {
               char *      pc;
               char        rgc[sizeof(char *)];
            } u1;
            unsigned int ic;
            int iHash;
            char *pHash;
            char *pHashPrev;
            char *pString;

            plast[-1] = '\0';
            iHash     = UMIN( MAX_KEY_HASH - 1, plast - 1 - top_string );
            for ( pHash = string_hash[iHash]; pHash; pHash = pHashPrev )
            {
               for ( ic = 0; ic < sizeof(char *); ic++ )
               u1.rgc[ic] = pHash[ic];
               pHashPrev = u1.pc;
               pHash    += sizeof(char *);

               if ( top_string[sizeof(char *)] == pHash[0]
               &&   !strcmp( top_string+sizeof(char *)+1, pHash+1 ) )
               return pHash;
            }

            if ( fBootDb )
            {
               pString             = top_string;
               top_string          = plast;
               u1.pc               = string_hash[iHash];
               for ( ic = 0; ic < sizeof(char *); ic++ )
               pString[ic] = u1.rgc[ic];
               string_hash[iHash]  = pString;

               nAllocString += 1;
               sAllocString += top_string - pString;
               return pString + sizeof(char *);
            }
            else
            {
               return str_dup( top_string + sizeof(char *) );
            }
         }
      }
   }
}



/*
* Read to end of line (for comments).
*/
void fread_to_eol( FILE *fp )
{
   char c;

   do
   {
      c = getc( fp );
   }
   while ( c != '\n' && c != '\r' );

   do
   {
      c = getc( fp );
   }
   while ( c == '\n' || c == '\r' );

   ungetc( c, fp );
   return;
}



/*
* Read one word (into static buffer).
*/
char *fread_word( FILE *fp )
{
   static char word[MAX_INPUT_LENGTH];
   char *pword;
   char cEnd;

   do
   {
      cEnd = getc( fp );
   }
   while ( isspace( cEnd ) );

   if ( cEnd == '\'' || cEnd == '"' )
   {
      pword   = word;
   }
   else
   {
      word[0] = cEnd;
      pword   = word+1;
      cEnd    = ' ';
   }

   for ( ; pword < word + MAX_INPUT_LENGTH; pword++ )
   {
      *pword = getc( fp );
      if ( cEnd == ' ' ? isspace(*pword) : *pword == cEnd )
      {
         if ( cEnd == ' ' )
         ungetc( *pword, fp );
         *pword = '\0';
         return word;
      }
   }

   pword--;
   ungetc( *pword, fp );
   *pword = '\0';
   sprintf(log_buf, "Word is: %s", word);
   log_string(log_buf);
   bug( "Fread_word: word too long.", 0 );
   exit( 1 );
   return NULL;
}

/*
* Allocate some ordinary memory,
*   with the expectation of freeing it someday.
*/
void *alloc_mem( int sMem )
{
   void *pMem;
   int *magic;
   int iList;

   sMem += sizeof(*magic);

   for ( iList = 0; iList < MAX_MEM_LIST; iList++ )
   {
      if ( sMem <= rgSizeList[iList] )
      break;
   }

   if ( iList == MAX_MEM_LIST )
   {
      bug( "Alloc_mem: size %d too large.", sMem );
      exit( 1 );
   }

   if ( rgFreeList[iList] == NULL )
   {
      pMem              = alloc_perm( rgSizeList[iList] );
   }
   else
   {
      pMem              = rgFreeList[iList];
      rgFreeList[iList] = * ((void **) rgFreeList[iList]);
   }

   magic = (int *) pMem;
   *magic = MAGIC_NUM;
   /* pMem += sizeof(*magic); */
   pMem = (void*) ((char*) pMem + sizeof(*magic));
   return pMem;
}



/*
* Free some memory.
* Recycle it back onto the free list for blocks of that size.
*/
void free_mem( void *pMem, int sMem )
{
   int iList;
   int *magic;

   /* pMem -= sizeof(*magic); */
   pMem = (void*) ((char*) pMem - sizeof(*magic));
   magic = (int *) pMem;

   if (*magic != MAGIC_NUM)
   {
      /*
      bug("Attempt to recyle invalid memory of size %d.", sMem);
      bug((char*) pMem + sizeof(*magic), 0);
      */
      return;
   }

   *magic = 0;
   sMem += sizeof(*magic);

   for ( iList = 0; iList < MAX_MEM_LIST; iList++ )
   {
      if ( sMem <= rgSizeList[iList] )
      break;
   }

   if ( iList == MAX_MEM_LIST )
   {
      bug( "Free_mem: size %d too large.", sMem );
      exit( 1 );
   }

   * ((void **) pMem) = rgFreeList[iList];
   rgFreeList[iList]  = pMem;
   return;
}


/*
* Allocate some permanent memory.
* Permanent memory is never freed,
*   pointers into it may be copied safely.
*/
void *alloc_perm( int sMem )
{
   void *pMem;
   static char *pMemPerm;
   static int iMemPerm;

   while ( sMem % sizeof(long) != 0 )
   sMem++;
   if ( sMem > MAX_PERM_BLOCK )
   {
      bug( "Alloc_perm: %d too large.", sMem );
      exit( 1 );
   }

   if ( pMemPerm == NULL || iMemPerm + sMem > MAX_PERM_BLOCK )
   {
      iMemPerm = 0;
      if ( ( pMemPerm = calloc( 1, MAX_PERM_BLOCK ) ) == NULL )
      {
         perror( "Alloc_perm" );
         exit( 1 );
      }
   }

   pMem        = pMemPerm + iMemPerm;
   iMemPerm   += sMem;
   nAllocPerm += 1;
   sAllocPerm += sMem;
   return pMem;
}



/*
* Duplicate a string into dynamic memory.
* Fread_strings are read-only and shared.
*/
char *str_dup( const char *str )
{
   char *str_new;

   if ( str[0] == '\0' )
   return &str_empty[0];

   if ( str >= string_space && str < top_string )
   return (char *) str;

   str_new = alloc_mem( strlen(str) + 1 );
   strcpy( str_new, str );
   return str_new;
}



/*
* Free a string.
* Null is legal here to simplify callers.
* Read-only shared strings are not touched.
*/
void free_string( char *pstr )
{
   if ( pstr == NULL
   ||   pstr == &str_empty[0]
   ||   pstr == &str_none[0]
   || ( pstr >= string_space && pstr < top_string ) )
   return;

   free_mem( pstr, strlen(pstr) + 1 );
   return;
}


/*************************************************************
AREAS VERSION 2.6
*************************************************************/

bool do_areas_is_valid_level_format (char* level)
{
   /*
   returns 1 if level (a string) is in a valid
   level format
   valid format is
   'XX YY'
   (no quotes), separated by spaces, 5 characters total
   where XX is a 2 digit number (lower number in the range)
   YY is a 2 digit number (upper number in the range)
   returns 0 if not in correct format
   level range 1 to 10
   would be
   01 10

   if the lower and higher level numbers are in wrong order
   ie
   "60 01" it will return false (not valid)
   */
   int lower  = 0;
   int higher = 0;

   if (strlen(level) != 5)
   {
      return FALSE;
   }
   if
   (
      !isdigit(level[0]) ||
      !isdigit(level[1]) ||
      (level[2] != ' ')  ||
      !isdigit(level[3]) ||
      !isdigit(level[4])
   )
   {
      return FALSE;
   }
   sscanf(level, "%d %d", &lower, &higher);
   return lower <= higher;
}

int do_areas_compare_title (const void* input1, const void* input2)
{
   /*
   Comparator function for do areas az
   sorts by area name
   */
   char temp   [MAX_STRING_LENGTH_DO_AREA];  /* placeholder */
   char title1 [MAX_STRING_LENGTH_DO_AREA];
   char title2 [MAX_STRING_LENGTH_DO_AREA];
   AREA_DATA* area1;
   AREA_DATA* area2;
   area1 = *(AREA_DATA **)input1;
   area2 = *(AREA_DATA **)input2;

   sscanf(area1->credits, "{%[^}]} %s %[^ ]", temp, temp, title1);
   sscanf(area2->credits, "{%[^}]} %s %[^ ]", temp, temp, title2);
   return strcmp(title1, title2);
}

int do_areas_compare_builder (const void* input1, const void* input2)
{
   /*
   Comparator function for do areas builder
   and do areas special <builder>
   sorts by the builder of the area,
   if same builder, sorts by area name
   */
   char temp     [MAX_STRING_LENGTH_DO_AREA];  /* placeholder */
   char builder1 [MAX_STRING_LENGTH_DO_AREA];
   char builder2 [MAX_STRING_LENGTH_DO_AREA];
   char title1   [MAX_STRING_LENGTH_DO_AREA];
   char title2   [MAX_STRING_LENGTH_DO_AREA];
   int retval = 0;
   AREA_DATA* area1;
   AREA_DATA* area2;

   area1 = *(AREA_DATA **)input1;
   area2 = *(AREA_DATA **)input2;
   sscanf(area1->credits, "{%[^}]} %s %[^ ]", temp, builder1, title1);
   sscanf(area2->credits, "{%[^}]} %s %[^ ]", temp, builder2, title2);
   retval = strcmp(builder1, builder2);
   if (retval != 0)
   {
      return retval;
   }
   return strcmp(title1, title2);
}

int do_areas_compare_level (const void* input1, const void* input2)
{
   /*
   Comparator function for do areas level
   and do areas level <level>
   if only one is a level range format, puts that one earlier
   in the array
   if neither are in level range format,
   sorts by area name
   if both are in level range format:
   sorts first by min level, if equal then max level,
   if equal then area title
   */
   char levels1 [MAX_STRING_LENGTH_DO_AREA];
   char levels2 [MAX_STRING_LENGTH_DO_AREA];
   char temp    [MAX_STRING_LENGTH_DO_AREA];  /* placeholder */
   char title1  [MAX_STRING_LENGTH_DO_AREA];
   char title2  [MAX_STRING_LENGTH_DO_AREA];
   int lowera    = 0;
   int highera   = 0;
   int lowerb    = 0;
   int higherb   = 0;
   int retval    = 0;
   AREA_DATA* area1;
   AREA_DATA* area2;

   area1 = *(AREA_DATA **)input1;
   area2 = *(AREA_DATA **)input2;
   sscanf(area1->credits, "{%[^}]} %s %[^ ]", levels1, temp, title1);
   sscanf(area2->credits, "{%[^}]} %s %[^ ]", levels2, temp, title2);
   /*
   Credits in level format are on top
   if one of them is not in level format
   it goes afterwards
   */
   if
   (
      do_areas_is_valid_level_format(levels1) &&
      !do_areas_is_valid_level_format(levels2)
   )
   {
      return -1;
   }
   if
   (
      !do_areas_is_valid_level_format(levels1) &&
      do_areas_is_valid_level_format(levels2)
   )
   {
      return 1;
   }
   /*
   If neither are in level format, sort
   by levels1 and levels2 as strings,
   if equal, sort by area title
   */
   if
   (
      !do_areas_is_valid_level_format(levels1) &&
      !do_areas_is_valid_level_format(levels2)
   )
   {
      retval = strcmp(levels1, levels2);
      if (retval != 0)
      {
         return retval;
      }
      return strcmp(title1, title2);
   }
   sscanf(levels1, "%d %d", &lowera, &highera);
   sscanf(levels2, "%d %d", &lowerb, &higherb);
   if (lowera < lowerb)
   {
      return -1;
   }
   if (lowera > lowerb)
   {
      return 1;
   }
   if (highera < higherb)
   {
      return -1;
   }
   if (highera > higherb)
   {
      return 1;
   }
   return strcmp(title1, title2);
}

int do_areas_compare_special (const void* input1, const void* input2)
{
   /*
      Comparator function for do areas special
      and do areas special <special>
      Does a simple string comparison on the thing between {}'s
      of each areas's credits
      if identical, compares the area name
   */
   char special1 [MAX_STRING_LENGTH_DO_AREA];
   char special2 [MAX_STRING_LENGTH_DO_AREA];
   char temp     [MAX_STRING_LENGTH_DO_AREA];  /* placeholder */
   char title1   [MAX_STRING_LENGTH_DO_AREA];
   char title2   [MAX_STRING_LENGTH_DO_AREA];
   int retval = 0;
   AREA_DATA* area1;
   AREA_DATA* area2;

   area1 = *(AREA_DATA **)input1;
   area2 = *(AREA_DATA **)input2;
   sscanf(area1->credits, "{%[^}]} %s %[^ ]", special1, temp, title1);
   sscanf(area2->credits, "{%[^}]} %s %[^ ]", special2, temp, title2);
   retval = strcmp(special1, special2);
   if (retval != 0)
   {
      return retval;
   }
   return strcmp(title1, title2);
}

/*
   Restrictor functions:
   TRUE means print out that line
   FALSE means don't print out the line
*/

bool do_areas_restrict_none(const char* restrict_by, const AREA_DATA* check)
/*
   restrictor function that doesn't restrict
*/
{
   return TRUE;
}

bool do_areas_restrict_builder(const char* restrict_by, const AREA_DATA* check)
{
   /*
   restrictor function for areas builder
   and areas builder <builder>
   If <builder> is empty, returns TRUE (prints everything)
   if <builder> isn't empty, returns TRUE only if restrict_by is the builder
   of this area else FALSE
   */
   char temp    [MAX_STRING_LENGTH_DO_AREA];  /* placeholder */
   char builder [MAX_STRING_LENGTH_DO_AREA];

   if (restrict_by[0] == '\0')
   {
      return TRUE;
   }
   sscanf(check->credits, "{%[^}]} %s %[^ ]", temp, builder, temp);
   /*
   following is substring matching
   check if restrict_by is inside of p2
   */
   return stristr(builder, restrict_by) != NULL;
}

bool do_areas_restrict_level(const char* restrict_by, const AREA_DATA* check)
/*
   restrictor function for areas level
   and areas level <level>
   If <level> is empty, returns TRUE (prints everything)
   if <level> isn't empty, returns TRUE only if the restrict_by is in the level
   range of the area, OR if its not in a level range format
   returns FALSE if not in the level range
*/
{
   char level [MAX_STRING_LENGTH_DO_AREA];
   char temp  [MAX_STRING_LENGTH_DO_AREA];  /* placeholder */
   int lower     = 0;
   int higher    = 0;
   int compareto = 0;

   if (restrict_by[0] == '\0')
   {
      return TRUE;
   }
   sscanf(check->credits, "{%[^}]} %s %[^ ]", level, temp, temp);
   if (!do_areas_is_valid_level_format(level))
   {
      return TRUE;
   }
   sscanf(level, "%d %d", &lower, &higher);
   sscanf(restrict_by, "%d", &compareto);
   return
   (
      (compareto >= lower) && (compareto <= higher)
   );
}

bool do_areas_restrict_special(const char* restrict_by, const AREA_DATA* check)
{
   /*
   restrictor function for areas special
   and areas special <special>
   If <special> is empty, returns FALSE (don't print) for anything that is
   in level range format else TRUE
   if <special> isn't empty, returns TRUE only if the thing inside {}'s matches
   <special> else FALSE
   */
   char special [MAX_STRING_LENGTH_DO_AREA];
   char temp    [MAX_STRING_LENGTH_DO_AREA];  /* placeholder */

   sscanf(check->credits, "{%[^}]} %s %[^ ]", special, temp, temp);
   if (restrict_by[0] == '\0')
   {
      return !do_areas_is_valid_level_format(special);
   }
   return !str_cmp(restrict_by, special);
}

void do_areas_print
(
   CHAR_DATA* ch,
   AREA_DATA** arr,
   sh_int size,
   const char* restrictor,
   bool (*restrict) (const char*, const AREA_DATA*)
)
{
   /*
   print out all the areas, except those restricted by the restrict function
   */
   int icount      = 0;
   unsigned int printed     = 0;
   unsigned int max_level   = 0;
   unsigned int max_builder = 0;
   char level   [MAX_STRING_LENGTH_DO_AREA];
   char builder [MAX_STRING_LENGTH_DO_AREA];
   char title   [MAX_STRING_LENGTH_DO_AREA];
   char format  [MAX_STRING_LENGTH_DO_AREA];
   char buf     [MAX_STRING_LENGTH_DO_AREA];
   BUFFER* output;

   output = new_buf();
   /*
   find the maximum length of item inside {}'s, and builder name
   (only count those that will be printed)
   don't need the max of p3 (actual area name)
   becuase that will be printed at end,
   and if it needs to wrap.. its going to wrap
   on the client side.  (should never be that long)
   */
   for (icount=0; icount < size; icount++)
   {
      if (restrict(restrictor, arr[icount]))
      {
         printed++;
         sscanf
         (
            arr[icount]->credits,
            "{%[^}]} %s %[^ ]",
            level,
            builder,
            title
         );
         if (strlen(level) > max_level)
         {
            max_level = strlen(level);
         }
         if (strlen(builder) > max_builder)
         {
            max_builder = strlen(builder);
         }
      }
   }
   /*
   Set up format string with the sizes of the first two parts of
   credits
   */
   sprintf(format, "{%%-%ds} %%-%ds %%s\n\r", max_level, max_builder);
   for (icount = 0; icount < size; icount++)
   {
      if (restrict(restrictor, arr[icount]))
      {
         /*
         If it passes the restrictor, print it out using the format
         */
         sscanf
         (
            arr[icount]->credits,
            "{%[^}]} %s %[^ ]",
            level,
            builder,
            title
         );
         sprintf(buf, format, level, builder, title);
         add_buf(output, buf);
      }
   }
   /*
   Find out how many areas were printed
   and add that to the end of the stream
   */
   if (printed == 0)
   {
      sprintf(buf, "No areas match.\n\r");
   }
   else
   {
      if (printed == 1)
      {
         sprintf(buf, "%d area found.\n\r", printed);
      }
      else
      {
         sprintf(buf, "%d areas found.\n\r", printed);
      }
   }
   add_buf(output, buf);
   /*
   Send it to the character, using paging if they have it set
   */
   page_to_char(buf_string(output), ch);
   free_buf(output);
   return;
}

void do_areas( CHAR_DATA* ch, char* arg )
{
   char argc [MAX_INPUT_LENGTH];  /* Argument Command(Optional) */
   char arg1 [MAX_INPUT_LENGTH];  /* Argument 1 (Optional) */
   AREA_DATA* pArea1    = 0;
   sh_int icounter      = 0;
   sh_int ilen          = 0;
   AREA_DATA** arr      = 0;
   sh_int arr_size      = 0;
   char level   [MAX_STRING_LENGTH_DO_AREA];
   char builder [MAX_STRING_LENGTH_DO_AREA];
   char title   [MAX_STRING_LENGTH_DO_AREA];
   char buf     [MAX_STRING_LENGTH];

   arg = one_argument(arg, argc);
   arg = one_argument(arg, arg1);
   /*
   Maximum of two arguments for this command, if three, return
   */
   if (arg[0] != '\0')
   {
      send_to_char("Too many arguments.  Read 'help areas'.\n\r", ch);
      return;
   }
   /*
   find out how many areas we have
   */
   for (pArea1 = area_first; pArea1; pArea1 = pArea1->next)
   {
      level[0] = '\0';
      builder[0] = '\0';
      title[0] = '\0';
      sscanf
      (
         pArea1->credits,
         "{%[^}]} %s %[^ ]",
         level,
         builder,
         title
      );
      if
      (
         level[0] == '\0' ||
         builder[0] == '\0' ||
         title[0] == '\0'
      )
      {
         sprintf
         (
            buf,
            "do_areas: Non Standard Area Header: [%s] [%s]",
            pArea1->file_name,
            pArea1->name
         );
         bug(buf, 0);
         continue;
      }
      arr_size++;
   }
   /*
   allocate memory for the array
   */
   arr = (AREA_DATA**) malloc(arr_size * sizeof(AREA_DATA*));
   /*
   setting up an array of pointers to areas
   */
   for (pArea1 = area_first; pArea1; pArea1 = pArea1->next)
   {
      level[0] = '\0';
      builder[0] = '\0';
      title[0] = '\0';
      sscanf
      (
         pArea1->credits,
         "{%[^}]} %s %[^ ]",
         level,
         builder,
         title
      );
      if
      (
         level[0] == '\0' ||
         builder[0] == '\0' ||
         title[0] == '\0'
      )
      {
         continue;
      }
      arr[icounter] = pArea1;
      icounter++;
   }
   /*
   If no arguments, do original areas output
   */
   if (argc[0] == '\0')
   {
      /*
      print out areas, print out all, so using do_areas_restrict_none
      */
      do_areas_print(ch, arr, arr_size, NULL, do_areas_restrict_none);
   }
   else
   /*
   Sort areas alphabetically
   */
   if (!str_cmp(argc, "az"))
   {
      /*
      Sort the array using do_areas_compare_title
      */
      qsort(arr, arr_size, sizeof (AREA_DATA*), do_areas_compare_title);
      /*
      print out areas, print out all, so using do_areas_restrict_none
      */
      do_areas_print(ch, arr, arr_size, NULL, do_areas_restrict_none);
   }
   else
   /*
   Sort areas by builder and then alphabetically by area name
   if arg1 isn't empty, restrict   to that builder only
   */
   if (!str_cmp(argc, "builder"))
   {
      /*
      Sort the array using do_areas_compare_builder
      */
      qsort(arr, arr_size, sizeof(AREA_DATA*), do_areas_compare_builder);
      /*
      use do_areas_restrict_builder to see if each area gets printed
      if arg1 is empty, it won't restrict any
      if arg1 isn't empty, it will restrict to that builder
      */
      do_areas_print(ch, arr, arr_size, arg1, do_areas_restrict_builder);
   }
   else
   /*
   Restrict to areas that don't have a level range.
   Sort areas by word inside the {}'s and then alphabetically by area name
   if arg1 isn't empty, restrict only to areas that have that inside
   the {}'s
   */
   if (!str_cmp(argc, "special"))
   {
      /*
      Sort the array using do_areas_compare_special
      */
      qsort(arr, arr_size, sizeof(AREA_DATA*), do_areas_compare_special);
      /*
      use do_areas_restrict_builder to see if each area gets printed
      if arg1 is empty, it won't print anything that uses a level range format
      if arg1 isn't empty, it will only print non-level range format areas
      that fit arg1 inside {}'s
      */
      do_areas_print(ch, arr, arr_size, arg1, do_areas_restrict_special);
   }
   else
   /*
   Sort areas by min level, then max level, then alphabetically
   by area name.  All area credits that don't fit level range go
   at bottom, sorted by whats inside the {}'s
   */
   if (!str_cmp(argc, "level"))
   {
      /*
      If there is an argument, need to check that its numeric
      */
      if (arg1[0] != '\0')
      {
         for (icounter = 0; icounter < ilen; icounter++)
         {
            /*
            If any of the characters in the argument aren't digits, cancel out,
            free memory, and warn the character
            */
            if (!isdigit(arg1[icounter]))
            {
               send_to_char("Invalid argument.  The parameter for 'level' should be a number.  Read 'help areas'.\n\r", ch);
               free(arr);
               return;
            }
            /*
            It is a number, but 3 digits or more.  Max level is 60, mobs could possibly
            be higher, so limiting to 99 by limiting to 2 digits.  free memory, warn
            character and cancel out
            */
            if (ilen > 2)
            {
               send_to_char("Incorrect argument, levels can't have more than two digits, read 'help areas'", ch);
               free(arr);
               return;
            }
         }
      }
      /*
      Sort the array using do_areas_compare_level
      */
      qsort(arr, arr_size, sizeof(AREA_DATA*), do_areas_compare_level);
      /*
      Print out the areas, use do_areas_restrict_level to see which ones get outputted/skipped
      */
      do_areas_print(ch, arr, arr_size, arg1, do_areas_restrict_level);
   }
   else
   {
      send_to_char("Incorrect argument, read 'help areas'", ch);
   }
   free(arr);
}
/*
------------------------------------------------------------------------
*/

void do_memory( CHAR_DATA *ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];

   sprintf( buf, "Affects %5d\n\r", top_affect    ); send_to_char( buf, ch );
   sprintf( buf, "(in use) %ld\n\r", total_affects); send_to_char( buf, ch );
   sprintf( buf, "Areas   %5d\n\r", top_area      ); send_to_char( buf, ch );
   sprintf( buf, "ExDes   %5d\n\r", top_ed        ); send_to_char( buf, ch );
   sprintf( buf, "Exits   %5d\n\r", top_exit      ); send_to_char( buf, ch );
   sprintf( buf, "Helps   %5d\n\r", top_help      ); send_to_char( buf, ch );
   sprintf( buf, "Socials %5d\n\r", social_count  ); send_to_char( buf, ch );
   sprintf( buf, "Mobs    %5d(%d new format)\n\r", top_mob_index, newmobs );
   send_to_char( buf, ch );
   sprintf( buf, "(in use)%5d\n\r", mobile_count  ); send_to_char( buf, ch );
   sprintf( buf, "Objs    %5d(%d new format)\n\r", top_obj_index, newobjs );
   send_to_char( buf, ch );
   sprintf( buf, "(in use) %d\n\r", count_objects());
   send_to_char( buf, ch);
   sprintf( buf, "Resets  %5d\n\r", top_reset     ); send_to_char( buf, ch );
   sprintf( buf, "Rooms   %5d\n\r", top_room      ); send_to_char( buf, ch );
   sprintf( buf, "Shops   %5d\n\r", top_shop      ); send_to_char( buf, ch );

   sprintf( buf, "Strings %5d strings of %7d bytes (max %d).\n\r",
   nAllocString, sAllocString, MAX_STRING );
   send_to_char( buf, ch );

   sprintf( buf, "Perms   %5d blocks  of %7d bytes.\n\r",
   nAllocPerm, sAllocPerm );
   send_to_char( buf, ch );
   sprintf( buf, "Total heroes: %d\n\r", hero_count);
   send_to_char(buf, ch);
   return;
}

void do_dump( CHAR_DATA *ch, char *argument )
{
   int count, count2, num_pcs, aff_count;
   CHAR_DATA *fch;
   MOB_INDEX_DATA *pMobIndex;
   PC_DATA *pc;
   OBJ_DATA *obj;
   OBJ_INDEX_DATA *pObjIndex;
   ROOM_INDEX_DATA *room;
   EXIT_DATA *exit;
   DESCRIPTOR_DATA *d;
   AFFECT_DATA *af;
   FILE *fp;
   int vnum, nMatch = 0;

   /* open file */
   fclose(fpReserve);
   fp = fopen("mem.dmp", "w");

   /* report use of data structures */

   num_pcs = 0;
   aff_count = 0;

   /* mobile prototypes */
#if defined(MSDOS)
   fprintf(fp, "MobProt    %4d (%8ld bytes)\n",
   top_mob_index, top_mob_index * (sizeof(*pMobIndex)));
#else
   fprintf(fp, "MobProt    %4d (%8d bytes)\n",
   top_mob_index, top_mob_index * (sizeof(*pMobIndex)));
#endif

   /* mobs */
   count = 0;  count2 = 0;
   for (fch = char_list; fch != NULL; fch = fch->next)
   {
      count++;
      if (fch->pcdata != NULL)
      num_pcs++;
      for (af = fch->affected; af != NULL; af = af->next)
      aff_count++;
   }
   for (fch = char_free; fch != NULL; fch = fch->next)
   count2++;

#if defined(MSDOS)
   fprintf(fp, "Mobs    %4d (%8ld bytes), %2d free (%ld bytes)\n",
   count, count * (sizeof(*fch)), count2, count2 * (sizeof(*fch)));
#else
   fprintf(fp, "Mobs    %4d (%8d bytes), %2d free (%d bytes)\n",
   count, count * (sizeof(*fch)), count2, count2 * (sizeof(*fch)));
#endif

   /* pcdata */
   count = 0;
   for (pc = pcdata_free; pc != NULL; pc = pc->next)
   count++;

#if defined(MSDOS)
   fprintf(fp, "Pcdata    %4d (%8ld bytes), %2d free (%ld bytes)\n",
   num_pcs, num_pcs * (sizeof(*pc)), count, count * (sizeof(*pc)));
#else
   fprintf(fp, "Pcdata    %4d (%8d bytes), %2d free (%d bytes)\n",
   num_pcs, num_pcs * (sizeof(*pc)), count, count * (sizeof(*pc)));
#endif

   /* descriptors */
   count = 0; count2 = 0;
   for (d = descriptor_list; d != NULL; d = d->next)
   count++;
   for (d= descriptor_free; d != NULL; d = d->next)
   count2++;

#if defined(MSDOS)
   fprintf(fp, "Descs    %4d (%8ld bytes), %2d free (%ld bytes)\n",
   count, count * (sizeof(*d)), count2, count2 * (sizeof(*d)));
#else
   fprintf(fp, "Descs    %4d (%8d bytes), %2d free (%d bytes)\n",
   count, count * (sizeof(*d)), count2, count2 * (sizeof(*d)));
#endif

   /* object prototypes */
   for ( vnum = 0; nMatch < top_obj_index; vnum++ )
   if ( ( pObjIndex = get_obj_index( vnum ) ) != NULL )
   {
      for (af = pObjIndex->affected; af != NULL; af = af->next)
      aff_count++;
      nMatch++;
   }

#if defined(MSDOS)
   fprintf(fp, "ObjProt    %4d (%8ld bytes)\n",
   top_obj_index, top_obj_index * (sizeof(*pObjIndex)));
#else
   fprintf(fp, "ObjProt    %4d (%8d bytes)\n",
   top_obj_index, top_obj_index * (sizeof(*pObjIndex)));
#endif

   /* objects */
   count = 0;  count2 = 0;
   for (obj = object_list; obj != NULL; obj = obj->next)
   {
      count++;
      for (af = obj->affected; af != NULL; af = af->next)
      aff_count++;
   }
   for (obj = obj_free; obj != NULL; obj = obj->next)
   count2++;

#if defined(MSDOS)
   fprintf(fp, "Objs    %4d (%8ld bytes), %2d free (%ld bytes)\n",
   count, count * (sizeof(*obj)), count2, count2 * (sizeof(*obj)));
#else
   fprintf(fp, "Objs    %4d (%8d bytes), %2d free (%d bytes)\n",
   count, count * (sizeof(*obj)), count2, count2 * (sizeof(*obj)));
#endif

   /* affects */
   count = 0;
   for (af = affect_free; af != NULL; af = af->next)
   count++;

#if defined(MSDOS)
   fprintf(fp, "Affects    %4d (%8ld bytes), %2d free (%ld bytes)\n",
   aff_count, aff_count * (sizeof(*af)), count, count * (sizeof(*af)));
#else
   fprintf(fp, "Affects    %4d (%8d bytes), %2d free (%d bytes)\n",
   aff_count, aff_count * (sizeof(*af)), count, count * (sizeof(*af)));
#endif

   /* rooms */
#if defined(MSDOS)
   fprintf(fp, "Rooms    %4d (%8ld bytes)\n",
   top_room, top_room * (sizeof(*room)));
#else
   fprintf(fp, "Rooms    %4d (%8d bytes)\n",
   top_room, top_room * (sizeof(*room)));
#endif

   /* exits */
#if defined(MSDOS)
   fprintf(fp, "Exits    %4d (%8ld bytes)\n",
   top_exit, top_exit * (sizeof(*exit)));
#else
   fprintf(fp, "Exits    %4d (%8d bytes)\n",
   top_exit, top_exit * (sizeof(*exit)));
#endif

   fclose(fp);

   /* start printing out mobile data */
   fp = fopen("mob.dmp", "w");

   fprintf(fp, "\nMobile Analysis\n");
   fprintf(fp,  "---------------\n");
   nMatch = 0;
   for (vnum = 0; nMatch < top_mob_index; vnum++)
   if ((pMobIndex = get_mob_index(vnum)) != NULL)
   {
      nMatch++;
      fprintf(fp, "#%-4d %3d active %3d killed     %s\n",
      pMobIndex->vnum, pMobIndex->count,
      pMobIndex->killed, pMobIndex->short_descr);
   }
   fclose(fp);

   /* start printing out object data */
   fp = fopen("obj.dmp", "w");

   fprintf(fp, "\nObject Analysis\n");
   fprintf(fp,  "---------------\n");
   nMatch = 0;
   for (vnum = 0; nMatch < top_obj_index; vnum++)
   if ((pObjIndex = get_obj_index(vnum)) != NULL)
   {
      nMatch++;
      fprintf(fp, "#%-4d %3d active %3d reset      %s\n",
      pObjIndex->vnum, pObjIndex->count,
      pObjIndex->reset_num, pObjIndex->short_descr);
   }

   /* close file */
   fclose(fp);
   fpReserve = fopen( NULL_FILE, "r" );
}



/*
   Stick a little fuzz on a number.
*/
int number_fuzzy( int number )
{
   switch ( number_bits( 2 ) )
   {
      case 0:  number -= 1; break;
      case 3:  number += 1; break;
   }

   return UMAX( 1, number );
}



/*
   Generate a random number.
*/
int number_range( int from, int to )
{
   int power;
   int number;

   if (from == 0 && to == 0)
   return 0;

   if ( ( to = to - from + 1 ) <= 1 )
   return from;

   for ( power = 2; power < to; power <<= 1 )
   ;

   while ( ( number = number_mm() & (power -1 ) ) >= to )
   ;

   return from + number;
}



/*
* Generate a percentile roll.
*/
int number_percent( void )
{
   int percent;

   while ( (percent = number_mm() & (128-1) ) > 99 )
   ;

   return 1 + percent;
}



/*
* Generate a random door.
*/
int number_door( void )
{
   int door;

   while ( ( door = number_mm() & (8-1) ) > 5)
   ;

   return door;
}

int number_bits( int width )
{
   return number_mm( ) & ( ( 1 << width ) - 1 );
}




/*
* I've gotten too many bad reports on OS-supplied random number generators.
* This is the Mitchell-Moore algorithm from Knuth Volume II.
* Best to leave the constants alone unless you've read Knuth.
* -- Furey
*/

/* I noticed streaking with this random number generator, so I switched
back to the system srandom call.  If this doesn't work for you,
define OLD_RAND to use the old system -- Alander */

#if defined (OLD_RAND)
static  int     rgiState[2+55];
#endif

void init_mm( )
{
#if defined (OLD_RAND)
   int *piState;
   int iState;

   piState     = &rgiState[2];

   piState[-2] = 55 - 55;
   piState[-1] = 55 - 24;

   piState[0]  = ((int) current_time) & ((1 << 30) - 1);
   piState[1]  = 1;
   for ( iState = 2; iState < 55; iState++ )
   {
      piState[iState] = (piState[iState-1] + piState[iState-2])
      & ((1 << 30) - 1);
   }
#else
   srandom(time(NULL)^getpid());
#endif
   return;
}



long number_mm( void )
{
#if defined (OLD_RAND)
   int *piState;
   int iState1;
   int iState2;
   int iRand;

   piState             = &rgiState[2];
   iState1             = piState[-2];
   iState2             = piState[-1];
   iRand               = (piState[iState1] + piState[iState2])
   & ((1 << 30) - 1);
   piState[iState1]    = iRand;
   if ( ++iState1 == 55 )
   iState1 = 0;
   if ( ++iState2 == 55 )
   iState2 = 0;
   piState[-2]         = iState1;
   piState[-1]         = iState2;
   return iRand >> 6;
#else
   return random() >> 6;
#endif
}


/*
* Roll some dice.
*/
int dice( int number, int size )
{
   int idice;
   int sum;

   switch ( size )
   {
      case 0: return 0;
      case 1: return number;
   }

   for ( idice = 0, sum = 0; idice < number; idice++ )
   sum += number_range( 1, size );

   return sum;
}



/*
* Simple linear interpolation.
*/
int interpolate( int level, int value_00, int value_32 )
{
   return value_00 + level * (value_32 - value_00) / 32;
}



/*
* Removes the tildes from a string.
* Used for player-entered strings that go into disk files.
*/
void smash_tilde( char *str )
{
   for ( ; *str != '\0'; str++ )
   {
      if ( *str == '~' )
      *str = '-';
   }

   return;
}

char* upstr(char* thestr)
{
   char buf[MAX_INPUT_LENGTH];
   int counter = 0;

   if (thestr == NULL)
   {
      return NULL;
   }
   for (counter = 0; counter < MAX_INPUT_LENGTH; counter++)
   {
      buf[counter] = UPPER(thestr[counter]);
      if (buf[counter] == '\0')
      {
         break;
      }
   }
   if (counter == MAX_INPUT_LENGTH)
   {
      buf[MAX_INPUT_LENGTH - 1] = '\0';
   }
   return str_dup(buf);
}

char* upstr_long(char* thestr)
{
   char buf[MAX_STRING_LENGTH];
   int counter = 0;

   if (thestr == NULL)
   {
      return NULL;
   }
   for (counter = 0; counter < MAX_STRING_LENGTH; counter++)
   {
      buf[counter] = UPPER(thestr[counter]);
      if (buf[counter] == '\0')
      {
         break;
      }
   }
   if (counter == MAX_STRING_LENGTH)
   {
      buf[MAX_INPUT_LENGTH - 1] = '\0';
   }
   return str_dup(buf);
}

/*
* Compare strings, case insensitive.
* Return 0 if same, return 1 if b > a, return -1 if b < a
*   (compatibility with historical functions).
*/
int str_cmp(const char* astr, const char* bstr)
{
   register char achar;
   register char bchar;

   if (astr == NULL)
   {
      bug("Str_cmp: null astr.", 0);
      return 1;
   }

   if (bstr == NULL)
   {
      bug("Str_cmp: null bstr.", 0);
      return -1;
   }

   for (; *astr || *bstr; astr++, bstr++)
   {
      achar = LOWER(*astr);
      bchar = LOWER(*bstr);
      if (achar < bchar)
      {
         return -1;
      }
      else if (achar > bchar)
      {
         return 1;
      }
   }

   return 0;
}



/*
* Compare strings, case insensitive, for prefix matching.
* Return TRUE if astr not a prefix of bstr
*   (compatibility with historical functions).
*/
bool str_prefix( const char *astr, const char *bstr )
{
   if ( astr == NULL )
   {
      bug( "Strn_cmp: null astr.", 0 );
      return TRUE;
   }

   if ( bstr == NULL )
   {
      bug( "Strn_cmp: null bstr.", 0 );
      return TRUE;
   }

   for ( ; *astr; astr++, bstr++ )
   {
      if ( LOWER(*astr) != LOWER(*bstr) )
      return TRUE;
   }

   return FALSE;
}



/*
* Compare strings, case insensitive, for match anywhere.
* Returns TRUE is astr not part of bstr.
*   (compatibility with historical functions).
*/
bool str_infix( const char *astr, const char *bstr )
{
   int sstr1;
   int sstr2;
   int ichar;
   char c0;

   if ( ( c0 = LOWER(astr[0]) ) == '\0' )
   return FALSE;

   sstr1 = strlen(astr);
   sstr2 = strlen(bstr);

   for ( ichar = 0; ichar <= sstr2 - sstr1; ichar++ )
   {
      if ( c0 == LOWER(bstr[ichar]) && !str_prefix( astr, bstr + ichar ) )
      return FALSE;
   }

   return TRUE;
}



/*
* Compare strings, case insensitive, for suffix matching.
* Return TRUE if astr not a suffix of bstr
*   (compatibility with historical functions).
*/
bool str_suffix( const char *astr, const char *bstr )
{
   int sstr1;
   int sstr2;

   sstr1 = strlen(astr);
   sstr2 = strlen(bstr);
   if ( sstr1 <= sstr2 && !str_cmp( astr, bstr + sstr2 - sstr1 ) )
   return FALSE;
   else
   return TRUE;
}



/*
* Returns an initial-capped string.
*/
char *capitalize( const char *str )
{
   static char strcap[MAX_STRING_LENGTH];
   int i;

   for (i = 0; i < MAX_STRING_LENGTH; i++)
   {
      strcap[i] = LOWER(str[i]);
      if (str[i] == '\0')
      {
         break;
      }
   }
   strcap[MAX_STRING_LENGTH - 1] = '\0';
   strcap[0] = UPPER(strcap[0]);
   return strcap;
}

char* full_capitalize(const char* str)
{
   static char strcap[MAX_STRING_LENGTH];
   int i;

   for (i = 0; i < MAX_STRING_LENGTH; i++)
   {
      strcap[i] = UPPER(str[i]);
      if (str[i] == '\0')
      {
         break;
      }
   }
   strcap[MAX_STRING_LENGTH - 1] = '\0';
   return strcap;
}

/*
* Append a string to a file.
*/
void append_file( CHAR_DATA *ch, char *file, char *str )
{
   FILE *fp;

   if ( IS_NPC(ch) || str[0] == '\0' )
   return;

   fclose( fpReserve );
   if ( ( fp = fopen( file, "a" ) ) == NULL )
   {
      perror( file );
      send_to_char( "Could not open the file!\n\r", ch );
   }
   else
   {
      fprintf( fp, "[%5d] %s: %s\n",
      ch->in_room ? ch->in_room->vnum : 0, ch->name, str );
      fclose( fp );
   }

   fpReserve = fopen( NULL_FILE, "r" );
   return;
}



/*
* Reports a bug.
*/

void bug_trust(const char* str, int param, int trust)
{
   char buf[MAX_STRING_LENGTH];
   extern bool boot_done;
   extern bool merc_down;
   FILE* lookup;
   char* file_name;

   if (fpArea != NULL)
   {
      lookup = fpArea;
      file_name = strArea;
   }
   else if (fpChar != NULL)
   {
      lookup = fpChar;
      file_name = strPlr;
   }
   else
   {
      lookup = NULL;
      file_name = NULL;
   }
   if (lookup != NULL)
   {
      int iLine;
      int iChar;

      if (lookup == stdin)
      {
         iLine = 0;
      }
      else
      {
         iChar = ftell(lookup);
         fseek(lookup, 0, 0);
         for (iLine = 0; ftell( lookup ) < iChar; iLine++)
         {
            while (getc( lookup ) != '\n')
            {
            }
         }
         fseek(lookup, iChar, 0);
      }

      sprintf(buf, "[*****] FILE: %s LINE: %d", file_name, iLine);
      log_string(buf);
   }

   strcpy(buf, "[*****] BUG: ");
   sprintf(buf + strlen(buf), str, param);
   log_string(buf);
   if
   (
      boot_done &&
      !merc_down
   )
   {
      wiznet
      (
         "$t",
         NULL,
         (OBJ_DATA*)(buf + strlen("[*****] BUG: ")),
         WIZ_BUGS,
         0,
         trust
      );
   }
   return;
}

void bug(const char* str, int param)
{
   bug_trust(str, param, DEMI);
}

/*
* Writes a string to the log.
*/
void log_string( const char *str )
{
   char *strtime;

   strtime                    = ctime( &current_time );
   strtime[strlen(strtime)-1] = '\0';
   fprintf( stderr, "%s :: %s\n", strtime, str );
   return;
}



/*
* This function is here to aid in debugging.
* If the last expression in a function is another function call,
*   gcc likes to generate a JMP instead of a CALL.
* This is called "tail chaining."
* It hoses the debugger call stack for that call.
* So I make this the last call in certain critical functions,
*   where I really need the call stack to be right for debugging!
*
* If you don't understand this, then LEAVE IT ALONE.
* Don't remove any calls to tail_chain anywhere.
*
* -- Furey
*/
void tail_chain( void )
{
   return;
}


void do_force_reset(CHAR_DATA *ch, char *argument)
{
   AREA_DATA *pArea;
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA *ich;

   pArea = ch->in_room->area;
   reset_area(pArea);
   pArea->age = number_range(0, 3);
   sprintf(buf, "Area: %s reset.\n\r", pArea->name);
   send_to_char(buf, ch);
   sprintf(buf, "Area: %s reset by %s.\n\r", pArea->name, ch->name);

   for (ich = char_list; ich != NULL; ich = ich->next)
   {
      if (!IS_NPC(ich) && ich != ch)
      {
         if (ich->level == 60 || ich->trust == 60)
         send_to_char(buf, ich);
      }
   }

   return;
}


void do_alist(CHAR_DATA *ch, char *argument)
{
   AREA_DATA *pArea;
   char buf[MAX_STRING_LENGTH];
   int searchvnum;
   char arg[MAX_STRING_LENGTH];
   bool Found;

   if (ch->level < 52 && !IS_SET(ch->comm, COMM_BUILDER))
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }

   one_argument( argument, arg );

   if ( arg[0] == '\0' )
   for (pArea = area_first; pArea != NULL; pArea = pArea->next)
   {
      sprintf(buf, "%-20s (%-12s): %4d %4d\n\r", pArea->name, pArea->file_name, pArea->min_vnum, pArea->max_vnum);
      send_to_char(buf, ch);
   } else
   {
      searchvnum = atoi(arg);
      Found = FALSE;
      for (pArea = area_first; pArea != NULL; pArea = pArea->next)
      {
         if (searchvnum >= pArea->min_vnum && searchvnum <= pArea->max_vnum)
         {
            sprintf(buf, "%-20s (%-12s): %4d %4d\n\r", pArea->name, pArea->file_name, pArea->min_vnum, pArea->max_vnum);
            send_to_char(buf, ch);
            Found = TRUE;
         }
      }
      if (Found == FALSE) send_to_char("That vnum does not exist.\n\r", ch);
   }
   return;
}

void do_vnumlist(CHAR_DATA *ch, char *argument)
{
   AREA_DATA *pArea, *sArea, *tArea;
   char buf[MAX_STRING_LENGTH];
   int curvnum, minvnum, maxvnum;
   char arg[MAX_STRING_LENGTH];
   bool Found = FALSE;

   if (ch->level < 52 && !IS_SET(ch->comm, COMM_BUILDER))
   {
      send_to_char("Huh?\n\r", ch);
      return;
   }

   one_argument( argument, arg );

   if (arg[0] == '\0')
   {
      maxvnum = 0;
      for (pArea = area_first; pArea != NULL; pArea = pArea->next)
      {
         if (pArea->min_vnum > maxvnum) maxvnum = pArea->min_vnum;
      }
      minvnum = maxvnum+1;
      for (pArea = area_first; pArea != NULL; pArea = pArea->next)
      {
         if (pArea->min_vnum < minvnum) minvnum = pArea->min_vnum;
      }
      while (minvnum <= maxvnum)
      {
         tArea = NULL;
         curvnum = minvnum;
         for (pArea = area_first; pArea != NULL; pArea = pArea->next)
         {
            if (minvnum == pArea->min_vnum) tArea = pArea;
         }
         minvnum = maxvnum+1;
         for (pArea = area_first; pArea != NULL; pArea = pArea->next)
         {
            if ((pArea->min_vnum < minvnum) && (pArea->min_vnum > curvnum))
            {
               minvnum = pArea->min_vnum;
            }
         }
         if (tArea != NULL)
         {
            sprintf(buf, "%-20s: %4d %4d\n\r", tArea->name, tArea->min_vnum, tArea->max_vnum);
            send_to_char(buf, ch);
         }
      }
      return;
   }

   if (!strcmp(arg, "free"))
   {
      maxvnum = 0;
      for (pArea = area_first; pArea != NULL; pArea = pArea->next)
      {
         if (pArea->min_vnum > maxvnum) maxvnum = pArea->min_vnum;
      }
      minvnum = maxvnum+1;
      for (pArea = area_first; pArea != NULL; pArea = pArea->next)
      {
         if (pArea->min_vnum < minvnum) minvnum = pArea->min_vnum;
      }
      while (minvnum <= maxvnum)
      {
         sArea = NULL;
         tArea = NULL;
         curvnum = minvnum;
         for (pArea = area_first; pArea != NULL; pArea = pArea->next)
         {
            if (minvnum == pArea->min_vnum) tArea = pArea;
         }
         minvnum = maxvnum+1;
         for (pArea = area_first; pArea != NULL; pArea = pArea->next)
         {
            if ((pArea->min_vnum < minvnum) && (pArea->min_vnum > curvnum))
            {
               minvnum = pArea->min_vnum;
            }
         }
         for (pArea = area_first; pArea != NULL; pArea = pArea->next)
         {
            if ((minvnum == pArea->min_vnum) && (minvnum <= maxvnum))
            sArea = pArea;
         }
         if ((sArea != NULL) && (tArea != NULL) &&
         (sArea->min_vnum - tArea->max_vnum > 2))
         {
            if (Found == FALSE)
            {
               send_to_char("Free Vnums:\n\r", ch);
               Found = TRUE;
            }
            sprintf(buf, "%4d - %4d\n\r", tArea->max_vnum+1, sArea->min_vnum-1);
            send_to_char(buf, ch);
         }
      }
      return;
   }

   send_to_char("Invalid parameter for vnumlist.\n\r", ch);

   return;
}

void do_overmax(CHAR_DATA *ch, char *argument)
{
   OBJ_INDEX_DATA *pObjIndex;
   char buf[MAX_STRING_LENGTH];
   bool Found = FALSE;
   AREA_DATA* parea;

   for (parea = area_first; parea; parea = parea->next)
   {
      for
      (
         pObjIndex = parea->objects;
         pObjIndex;
         pObjIndex = pObjIndex->next_in_area
      )
      {
         if
         (
            (
               pObjIndex->limtotal > 0 &&
               pObjIndex->limcount > pObjIndex->limtotal
            ) ||
            pObjIndex->limcount < 0
         )
         {
            if (!Found)
            {
               send_to_char("Overmaxed objects:\n\r", ch);
               send_to_char("Vnum   Limit  Count  Name\n\r", ch);
               Found = TRUE;
            }
            sprintf(buf, "%5d  %5d  %5d  %s  [%s]\n\r",
            pObjIndex->vnum, pObjIndex->limtotal,
            pObjIndex->limcount, pObjIndex->short_descr, pObjIndex->name);
            send_to_char(buf, ch);
         }
      }
   }

   if (!Found) send_to_char("No overmaxed objects.\n\r", ch);

   return;
}


void do_llimit(CHAR_DATA *ch, char *argument)
{
   FILE *fpChar_list;
   char catplr[MAX_INPUT_LENGTH];
   char chkbuf[MAX_STRING_LENGTH];
   char pbuf[100];
   OBJ_INDEX_DATA *lObjIndex;
   OBJ_DATA *obj;
   CHAR_DATA *carrier;
   AREA_DATA* parea;

   /*
      reset limit counts and recount items in game
      do NOT count items on players loaded
   */
   send_to_char("Reseting all counts to zero now.\n\r", ch);
   for (parea = area_first; parea; parea = parea->next)
   {
      for
      (
         lObjIndex = parea->objects;
         lObjIndex;
         lObjIndex = lObjIndex->next_in_area
      )
      {
         lObjIndex->limcount = 0;
      }
   }

   send_to_char("Counts set to zero....\n\r", ch);
   send_to_char("Loading all in-game counts now (excluding PC objects).\n\r", ch);
   for (obj = object_list; obj != NULL; obj = obj->next)
   {
      carrier = obj->carried_by;
      if (carrier != NULL && !IS_NPC(carrier))
      continue;
      obj->pIndexData->limcount++;
   }
   send_to_char("Loading all pfile object counts now.\n\r", ch);

   sprintf(catplr, "%s%s", PLAYER_DIR, "*.plr");
   sprintf(pbuf, "ls %s > %s", catplr, PLAYER_LIST);
   system(pbuf);

   if ((fpChar_list = fopen( PLAYER_LIST, "r")) == NULL)
   {
      perror(PLAYER_LIST);
      exit(1);
   }

   for (; ;)
   {
      strcpy(strPlr, fread_word(fpChar_list) );
      log_string(strPlr);
      sprintf(chkbuf, "%s%s", PLAYER_DIR, "Zzz.plr");
      if (!str_cmp(strPlr, "../player/Zzz.plr"))
      break;    /* Exit if == Zzz.plr file */

      if ( (    fpChar = fopen(strPlr, "r") ) == NULL)
      {
         perror(strPlr);
         exit(1);
      }

      for (; ;)
      {
         int vnum;
         char letter;
         char *word;
         OBJ_INDEX_DATA *pObjIndex;

         letter = fread_letter(fpChar);
         if (letter != '#')
         continue;
         word = fread_word(fpChar);

         if (!str_cmp(word, "End"))
         break;

         if ( !str_cmp( word, "O") || !str_cmp( word, "OBJECT"))
         {
            /*
            word = feof(fpChar) ? "End" : fread_word(fpChar);
            */
            word = fread_word(fpChar);
            if (!str_cmp(word, "Vnum"))
            {
               vnum = fread_number(fpChar);
               if ( (get_obj_index(vnum)) == NULL)
               {
                  bug("Bad obj vnum in limits: %d", vnum);
               }
               else
               {
                  pObjIndex = get_obj_index(vnum);
                  pObjIndex->limcount++;
               }
            }
         }
      }
      fclose(fpChar);
      fpChar = NULL;
   }
   /* Global for this file, make sure to set to NULL when not in use */
   fpChar = NULL;
   fclose( fpChar_list);
   send_to_char("All object counts reset.\n\r", ch);
   return;
}

/* Just find, if negative, look there too */
WORSHIP_TYPE* find_wizi_slot(char* name, int *slot)
{
   sh_int god;
   WORSHIP_TYPE* retval;

   if
   (
      *slot >= 0 &&
      *slot < worship_table_size
   )
   {
      /* Check all slots within 2 distance */
      if
      (
         worship_table[*slot].name != NULL &&
         !str_cmp(worship_table[*slot].name, name)
      )
      {
         return &worship_table[*slot];
      }
      *slot -= 1;
      if
      (
         *slot >= 0 &&
         worship_table[*slot].name != NULL &&
         !str_cmp(worship_table[*slot].name, name)
      )
      {
         return &worship_table[*slot];
      }
      *slot -= 1;
      if
      (
         *slot >= 0 &&
         worship_table[*slot].name != NULL &&
         !str_cmp(worship_table[*slot].name, name)
      )
      {
         return &worship_table[*slot];
      }
      *slot += 3;
      if
      (
         *slot < worship_table_size &&  /* no going off the end */
         worship_table[*slot].name != NULL &&
         !str_cmp(worship_table[*slot].name, name)
      )
      {
         return &worship_table[*slot];
      }
      *slot += 1;
      if
      (
         *slot < worship_table_size &&  /* no going off the end */
         worship_table[*slot].name != NULL &&
         !str_cmp(worship_table[*slot].name, name)
      )
      {
         return &worship_table[*slot];
      }
   }
   else if
   (
      *slot < 0 &&
      *slot >= -MAX_TEMP_GODS
   )
   {
      *slot = -(*slot + 1);
      /* Check slots within 2 range */
      if
      (
         *slot >= 0 &&
         temp_worship_table[*slot].name != NULL &&
         !str_cmp(temp_worship_table[*slot].name, name)
      )
      {
         retval = &temp_worship_table[*slot];
         *slot = -1 - *slot;
         return retval;
      }
      *slot -= 1;
      if
      (
         *slot >= 0 &&
         temp_worship_table[*slot].name != NULL &&
         !str_cmp(temp_worship_table[*slot].name, name)
      )
      {
         retval = &temp_worship_table[*slot];
         *slot = -1 - *slot;
         return retval;
      }
      *slot -= 1;
      if
      (
         *slot >= 0 &&
         temp_worship_table[*slot].name != NULL &&
         !str_cmp(temp_worship_table[*slot].name, name)
      )
      {
         retval = &temp_worship_table[*slot];
         *slot = -1 - *slot;
         return retval;
      }
      *slot += 3;
      if
      (
         *slot < MAX_TEMP_GODS &&
         temp_worship_table[*slot].name != NULL &&
         !str_cmp(temp_worship_table[*slot].name, name)
      )
      {
         retval = &temp_worship_table[*slot];
         *slot = -1 - *slot;
         return retval;
      }
      *slot += 1;
      if
      (
         *slot < MAX_TEMP_GODS &&
         temp_worship_table[*slot].name != NULL &&
         !str_cmp(temp_worship_table[*slot].name, name)
      )
      {
         retval = &temp_worship_table[*slot];
         *slot = -1 - *slot;
         return retval;
      }
   }
   /* slot is wrong, or was negative and didn't have one */
   for (god = 0; worship_table[god].name != NULL; god++)
   {
      if (!str_cmp(worship_table[god].name, name))
      {
         *slot = god;
         return &worship_table[god];
      }
   }
   for (god = 0; god < MAX_TEMP_GODS; god++)
   {
      if
      (
         temp_worship_table[god].name == NULL &&
         *slot != -500
      )
      {
         temp_worship_table[god].name = str_dup(name);
         *slot = -1 - god;
         return &temp_worship_table[god];
      }
      else if
      (
         temp_worship_table[god].name != NULL &&
         !str_cmp(temp_worship_table[god].name, name)
      )
      {
         *slot = -1 - god;
         return &temp_worship_table[god];
      }
   }
   if (*slot == -500)
   {
      return NULL;
   }
   bug("At least 11 immortals exist that are not in the worship table!", 0);
   free_string(final_worship.name);
   final_worship.name = str_dup(name);
   *slot = -100;
   return &final_worship;
}

/* If negative, ignore.  (try to find in real worship table) */
WORSHIP_TYPE* find_wizi_slot_load(char* name, int *slot)
{
   sh_int god;

   if
   (
      *slot >= 0 &&
      *slot < worship_table_size
   )
   {
      /* Check all slots within 2 distance */
      if
      (
         worship_table[*slot].name != NULL &&
         !str_cmp(worship_table[*slot].name, name)
      )
      {
         return &worship_table[*slot];
      }
      *slot -= 1;
      if
      (
         *slot >= 0 &&
         worship_table[*slot].name != NULL &&
         !str_cmp(worship_table[*slot].name, name)
      )
      {
         return &worship_table[*slot];
      }
      *slot -= 1;
      if
      (
         *slot >= 0 &&
         worship_table[*slot].name != NULL &&
         !str_cmp(worship_table[*slot].name, name)
      )
      {
         return &worship_table[*slot];
      }
      *slot += 3;
      if
      (
         *slot < worship_table_size &&  /* no going off the end */
         worship_table[*slot].name != NULL &&
         !str_cmp(worship_table[*slot].name, name)
      )
      {
         return &worship_table[*slot];
      }
      *slot += 1;
      if
      (
         *slot < worship_table_size &&  /* no going off the end */
         worship_table[*slot].name != NULL &&
         !str_cmp(worship_table[*slot].name, name)
      )
      {
         return &worship_table[*slot];
      }
   }
   /* slot is wrong, or was negative and didn't have one */
   for (god = 0; worship_table[god].name != NULL; god++)
   {
      if (!str_cmp(worship_table[god].name, name))
      {
         *slot = god;
         return &worship_table[god];
      }
   }
   for (god = 0; god < MAX_TEMP_GODS; god++)
   {
      if
      (
         temp_worship_table[god].name == NULL &&
         *slot != -500
      )
      {
         temp_worship_table[god].name = str_dup(name);
         *slot = -1 - god;
         return &temp_worship_table[god];
      }
      else if
      (
         temp_worship_table[god].name != NULL &&
         !str_cmp(temp_worship_table[god].name, name)
      )
      {
         *slot = -1 - god;
         return &temp_worship_table[god];
      }
   }
   if (*slot == -500)
   {
      return NULL;
   }
   bug("At least 11 immortals exist that are not in the worship table!", 0);
   free_string(final_worship.name);
   final_worship.name = str_dup(name);
   *slot = -100;
   return &final_worship;
}

void load_wizireport()
{
   sh_int god;
   FILE* gfile;
   char* name = NULL;
   WORSHIP_TYPE* worship;
   int slot;
   char str_save[MAX_INPUT_LENGTH];

   for (god = 0; worship_table[god].name != NULL; god++)
   {
      worship_table[god].history.last_change = 0;
      worship_table[god].history.start = 0;
      worship_table[god].history.played = 0;
      worship_table[god].history.vis = 0;
      worship_table[god].history.total_start = 0;
      worship_table[god].history.total_played = 0;
      worship_table[god].history.total_vis = 0;
      if (worship_table[god].wiz_info.email != NULL)
      {
         /*
         We will be freeing emails when changing
         Make sure free_string will not crash in the future
         (if there is a hardcoded email)
         */
         worship_table[god].wiz_info.email =
         (
            str_dup
            (
               worship_table[god].wiz_info.email
            )
         );
      }
   }
   for (god = 0; god < MAX_TEMP_GODS; god++)
   {
      temp_worship_table[god].history.last_change = 0;
      temp_worship_table[god].history.start = 0;
      temp_worship_table[god].history.played = 0;
      temp_worship_table[god].history.vis = 0;
      temp_worship_table[god].history.total_start = 0;
      temp_worship_table[god].history.total_played = 0;
      temp_worship_table[god].history.total_vis = 0;
   }
   fclose(fpReserve);
   sprintf(str_save, "%swizireport.dat", "");

   if
   (
      (
         gfile = fopen(str_save, "r")
      ) == NULL
   )
   {
      fpReserve = fopen(NULL_FILE, "r");
      return;
   }

   log_string("Loading wizireports.");
   for (; ;)
   {
      free_string(name);
      name = fread_word(gfile);
      if (!str_cmp(name, "end"))
      {
         free_string(name);
         break;
      }
      slot = fread_number(gfile);
      worship = find_wizi_slot_load(name, &slot);
      worship->history.start        = fread_number(gfile);
      worship->history.played       = fread_number(gfile);
      worship->history.vis          = fread_number(gfile);
      worship->history.total_start  = fread_number(gfile);
      worship->history.total_played = fread_number(gfile);
      worship->history.total_vis    = fread_number(gfile);
      /* Cannot be online before mud boots up */
   }

   fclose(gfile);
   fpReserve = fopen( NULL_FILE, "r" );

   return;
}

void load_globals(void)
{
   FILE* gfile;
   char* word = NULL;
   extern bool newlock;
   int vnum1;
   int vnum2;
   char str_save[MAX_INPUT_LENGTH];
   OBJ_INDEX_DATA* pObjIndex;
   OBJ_DATA* obj;
   ROOM_INDEX_DATA* room;
   int house;

   fclose(fpReserve);
   sprintf(str_save, "%sglobals.dat", "");

   if
   (
      (
         gfile = fopen(str_save, "r")
      ) == NULL
   )
   {
      fpReserve = fopen(NULL_FILE, "r");
      return;
   }

   log_string("Loading globals");
   for (; ;)
   {
      free_string(word);
      word = fread_word(gfile);
      if (!str_cmp(word, "end"))
      {
         free_string(word);
         break;
      }
      if (!str_cmp(word, "all_magic_gone"))
      {
         all_magic_gone = TRUE;
      }
      if (!str_cmp(word, "LogMobprogs"))
      {
         log_mobs = TRUE;
         continue;
      }
      if (!str_cmp(word, "LBU"))
      {
         last_banknote_update = fread_number(gfile);
         continue;
      }
      if (!str_cmp(word, "NId"))
      {
         last_note_id = fread_number(gfile);
         continue;
      }
      if (!str_cmp(word, "FrogHouse"))
      {
         frog_house = fread_number(gfile);
         continue;
      }
      if (!str_cmp(word, "Hitem"))
      {
         vnum1 = fread_number(gfile);
         vnum2 = fread_number(gfile);
         if ( ( pObjIndex = get_obj_index( vnum1 ) ) != NULL )
         {
            obj = create_object( pObjIndex, 0 );
            room = get_room_index( vnum2 );
            if (room != NULL)
            {
               obj_to_room( obj, room );
            }
            else
            {
               extract_obj(obj, FALSE);
            }
         }
         continue;
      }
      if (!str_cmp(word, "cov_ally"))
      {
         int number;

         number = fread_number(gfile);
         covenant_allies[number] = fread_number(gfile);
         continue;
      }
      if (!str_cmp(word, "morale_reset"))
      {
         morale_reset_time = fread_number(gfile);
         continue;
      }
      if (!str_cmp(word, "max_on_ever"))
      {
         max_on_ever = fread_number(gfile);
         continue;
      }
      if (!str_cmp(word, "Autoreboot"))
      {
         autoreboot_warning_hour = fread_number(gfile);
         autoreboot_hour = fread_number(gfile);
         continue;
      }
      for (house = 1; house < MAX_HOUSE; house++)
      {
         if (!str_cmp(house_table[house].balance_name, ""))
         {
            continue;
         }
         if (!str_cmp(house_table[house].morale_name, word))
         {
            house_downs[house]      = fread_number(gfile);
            house_kills[house]      = fread_number(gfile);
            house_pks[house]        = fread_number(gfile);
            unhoused_pk[house]      = fread_number(gfile);
            unique_kills[house]     = fread_number(gfile);
            unhoused_kills[house]   = fread_number(gfile);
            raids_defended[house]   = fread_number(gfile);
            raids_nodefender[house] = fread_number(gfile);
         }
         if (!str_cmp(house_table[house].balance_name, ""))
         {
            continue;
         }
         if (!str_cmp(house_table[house].balance_name, word))
         {
            house_account[house] = fread_number(gfile);
         }
      }
      if (!str_cmp(word, "newlock"))
      {
         newlock = fread_number(gfile);
         continue;
      }
      if (!str_cmp(word, "brand_channel"))
      {
         /* Disabled, replaced with bt_disabled */
         fread_to_eol(gfile);
         continue;
      }
      if (!str_cmp(word, "bt_disabled"))
      {
         sh_int god;
         char* name;

         name = fread_string(gfile);
         for (god = 0; worship_table[god].name != NULL; god++)
         {
            if (!strcmp(worship_table[god].name, name))
            {
               worship_table[god].brand_info.bt_disable = TRUE;
               break;
            }
         }
         free_string(name);
         continue;
      }
      if (!str_cmp(word, "bmd"))
      {
         black_market_data[black_market_data_free][0] = fread_number(gfile);
         black_market_data[black_market_data_free][1] = fread_number(gfile);
         black_market_data[black_market_data_free][2] = fread_number(gfile);
         black_market_data_free++;
         if (black_market_data_free >=1999)
         {
            black_market_data_free = 0;
         }
         continue;
      }
      if (!str_cmp(word, "DCave"))
      {
         demon_eggs  = fread_number(gfile);
         demon_babies = fread_number(gfile);
         demon_adults = fread_number(gfile);
         seed_demons();
         continue;
      }
      if (!str_cmp(word, "votes"))
      {
         votes[fread_number(gfile)] = fread_number(gfile);
         continue;
      }
      if (!str_cmp(word, "class_count"))
      {
         class_count[fread_number(gfile)] = fread_number(gfile);
         continue;
      }
      if (!str_cmp(word, "lottery"))
      {
         sh_int race;

         free_string(word);
         word = fread_word(gfile);
         if (!str_cmp(word, "race"))
         {
            free_string(word);
            word = fread_word(gfile);
            if (!str_cmp(word, "reset"))
            {
               lottery_race_reset = fread_number(gfile);
               continue;
            }
            if (!str_cmp(word, "run"))
            {
               free_string(word);
               word = fread_word(gfile);
               race = race_lookup(word);
               lottery_race_last_run[race] = fread_number(gfile);
               continue;
            }
            continue;
         }
         continue;
      }
   }

   fclose(gfile);
   fpReserve = fopen( NULL_FILE, "r" );

   /* initial seeding */
   if (demon_eggs == -1)
   {
      demon_adults = 5;
      demon_eggs = 5;
      seed_demons();
   }
   return;
}

void seed_demons(void)
{
   int cnt;
   int dtype;
   CHAR_DATA *demon;
   ROOM_INDEX_DATA *room = get_room_index(12480);

   return;  /* they killed all the demons */

   dtype = demon_lookup(NULL, "dragkagh");
   if (dtype == -1)
   return;
   if (room == NULL)
   return;

   /* do our transforms from last reboot */
   demon_adults += demon_babies/2;
   demon_babies /=2;
   demon_babies += demon_eggs/2;
   demon_eggs /=2;
   if (demon_eggs <= 2)
   {
      demon_babies += demon_eggs;
      demon_eggs = 0;
   }
   demon_eggs   += demon_adults/2;

   if (demon_adults > 100)
   demon_adults = 100;  /* something of a sanity check */

   /* start with eggs */
   for (cnt = 0; cnt < demon_eggs; cnt++)
   {
      demon = create_mobile(get_mob_index(MOB_VNUM_DEMON1));
      free_string(demon->name);
      free_string(demon->short_descr);
      free_string(demon->long_descr);
      free_string(demon->description);
      demon->description = str_dup("A large green egg covered in black and red spots.\n\r");
      demon->name = str_dup("egg");
      demon->short_descr = str_dup("The corrupted dragon egg");
      demon->long_descr = str_dup("A large green egg covered in black and red spots is here.\n\r");
      demon->max_hit = 2500;
      demon->hit = demon->max_hit;
      demon->race = 2;  /* I dont know what race that is but it doesnt matter */        demon->level = 30;
      demon->damroll = -1000;
      demon->hitroll = -1000;
      demon->spec_fun = 0;
      demon->armor[0] = -500;
      demon->armor[1] = -500;
      demon->armor[2] = -500;
      demon->armor[3] = -500;
      demon->imm_flags = demon_table[dtype].imm_flags;
      demon->res_flags = demon_table[dtype].res_flags;
      demon->vuln_flags = demon_table[dtype].vuln_flags;
      SET_BIT(demon->act, ACT_SENTINEL);
      SET_BIT(demon->act, ACT_STAY_AREA);
      char_to_room(demon, room);
   }

   /* now babies */
   for (cnt = 0; cnt < demon_babies; cnt++)
   {
      demon = create_mobile(get_mob_index(MOB_VNUM_DEMON1));
      free_string(demon->name);
      free_string(demon->short_descr);
      free_string(demon->long_descr);
      free_string(demon->description);
      demon->description = str_dup("A baby dragkagh looking fierce and gnawing on every bone in sight.\n\r");
      demon->name = str_dup("dragkagh baby demon");
      demon->race = 2;  /* I dont know what race that is but it doesnt matter */        demon->short_descr = str_dup("The baby Dragkagh");
      demon->long_descr = str_dup("A baby Dragkagh is here fiercly gnawing on every bone in sight.\n\r");
      demon->max_hit = demon_table[dtype].hp/2;
      demon->hit = demon->max_hit;
      demon->level = demon_table[dtype].min_level/2;
      demon->damroll = demon_table[dtype].dam;
      demon->spec_fun = 0;
      demon->armor[0] = demon_table[dtype].ac;
      demon->armor[1] = demon_table[dtype].ac;
      demon->armor[2] = demon_table[dtype].ac;
      demon->armor[3] = demon_table[dtype].ac * (.7);
      demon->imm_flags = demon_table[dtype].imm_flags;
      demon->res_flags = demon_table[dtype].res_flags;
      demon->vuln_flags = demon_table[dtype].vuln_flags;
      REMOVE_BIT(demon->act, ACT_SENTINEL);
      SET_BIT(demon->act, ACT_STAY_AREA);
      char_to_room(demon, room);
   }

   /* now adults */
   for (cnt = 0; cnt < demon_adults; cnt++)
   {
      demon = create_mobile(get_mob_index(MOB_VNUM_DEMON1));
      free_string(demon->name);
      free_string(demon->short_descr);
      free_string(demon->long_descr);
      free_string(demon->description);
      demon->description = str_dup(demon_table[dtype].description);
      demon->race = 2;  /* I dont know what race that is but it doesnt matter */
      demon->name = str_dup(demon_table[dtype].name);
      demon->short_descr = str_dup(demon_table[dtype].short_descr);
      demon->long_descr = str_dup(demon_table[dtype].long_descr);
      demon->max_hit = demon_table[dtype].hp;
      demon->hit = demon->max_hit;
      demon->level = demon_table[dtype].min_level;
      demon->damroll = demon_table[dtype].dam;
      demon->spec_fun = spec_lookup("spec_breath_gas");
      demon->armor[0] = demon_table[dtype].ac;
      demon->armor[1] = demon_table[dtype].ac;
      demon->armor[2] = demon_table[dtype].ac;
      demon->armor[3] = demon_table[dtype].ac * (.7);
      demon->imm_flags = demon_table[dtype].imm_flags;
      demon->res_flags = demon_table[dtype].res_flags;
      demon->vuln_flags = demon_table[dtype].vuln_flags;
      REMOVE_BIT(demon->act, ACT_SENTINEL);
      SET_BIT(demon->act, ACT_AGGRESSIVE);
      SET_BIT(demon->act, ACT_STAY_AREA);
      char_to_room(demon, room);
   }

   /* now misc demons */
   for (cnt = 0; cnt < demon_adults; cnt++)
   {
      demon = create_mobile(get_mob_index(MOB_VNUM_DEMON1));
      free_string(demon->name);
      free_string(demon->short_descr);
      free_string(demon->long_descr);
      free_string(demon->description);
      dtype = number_range(1, 8);
      demon->description = str_dup(demon_table[dtype].description);
      demon->name = str_dup(demon_table[dtype].name);
      demon->short_descr = str_dup(demon_table[dtype].short_descr);
      demon->long_descr = str_dup(demon_table[dtype].long_descr);
      demon->max_hit = demon_table[dtype].hp;
      demon->hit = demon->max_hit;
      demon->level = demon_table[dtype].min_level;
      demon->damroll = demon_table[dtype].dam;
      demon->spec_fun = spec_lookup("spec_brethren_demon");
      demon->armor[0] = demon_table[dtype].ac;
      demon->armor[1] = demon_table[dtype].ac;
      demon->armor[2] = demon_table[dtype].ac;
      demon->armor[3] = demon_table[dtype].ac * (.7);
      demon->imm_flags = demon_table[dtype].imm_flags;
      demon->res_flags = demon_table[dtype].res_flags;
      demon->vuln_flags = demon_table[dtype].vuln_flags;
      REMOVE_BIT(demon->act, ACT_SENTINEL);
      SET_BIT(demon->act, ACT_STAY_AREA);
      SET_BIT(demon->act, ACT_AGGRESSIVE);
      char_to_room(demon, room);
   }

   return;
}

int count_objects(void)
{
   int ret_val = 0;
   OBJ_DATA *obj;
   for (obj = object_list; obj != NULL; obj = obj->next)
   {
      ret_val++;
   }
   return ret_val;
}

void update_time()
{
   static TIMEVAL now_time;  /* Updated time */
   gettimeofday(&now_time, NULL);
   current_time = (time_t)now_time.tv_sec;
}

void add_roster(char* name, int clan, int house, int brand, int worship, int prob, int probtime, long login_time, bool storage, bool denied)
{
   if
   (
      name == NULL ||
      login_time == 0 ||
      (
         clan == 0 &&
         house == 0 &&
         brand == -1 &&
         worship == -1
      )
   )
   {
      return;
   }
   if (clan != 0)
   {
      add_to_roster
      (
         name,
         clan,
         prob,
         probtime,
         storage,
         denied,
         ROSTER_CLAN,
         login_time,
         FALSE
      );
   }
   if (house != 0)
   {
      add_to_roster
      (
         name,
         house,
         prob,
         probtime,
         storage,
         denied,
         ROSTER_HOUSE,
         login_time,
         FALSE
      );
   }
   if (brand != -1)
   {
      add_to_roster
      (
         name,
         brand,
         prob,
         probtime,
         storage,
         denied,
         ROSTER_BRAND,
         login_time,
         FALSE
      );
   }
   if (worship != -1)
   {
      add_to_roster
      (
         name,
         worship,
         prob,
         probtime,
         storage,
         denied,
         ROSTER_WORSHIP,
         login_time,
         FALSE
      );
   }
}

void add_to_roster(char* name, int slot, int prob, int probtime, bool storage, bool denied, sh_int type, long login_time, bool sort)
{
   ROSTER** roster;
   ROSTER*  proster;
   char buf[MAX_STRING_LENGTH];

   switch (type)
   {
      case (ROSTER_CLAN):
      {
         roster = &(clan_table[slot].proster);
         break;
      }
      case (ROSTER_BRAND):
      {
         roster = &(worship_table[slot].proster);
         break;
      }
      case (ROSTER_WORSHIP):
      {
         roster = &(worship_table[slot].pwroster);
         break;
      }
      case (ROSTER_HOUSE):
      {
         roster = &(house_table[slot].proster);
         break;
      }
      default:
      {
         return;
      }
   }
   sprintf
   (
      buf,
      "%-12s %s %3d hrs in %4d days (%4d Days Ago): %s",
      name,
      (
         denied ?
         "(Denied)" :
         (
            storage ?
            "(Stored)" :
            "        "
         )
      ),
      prob / 3600,
      (int)(current_time - probtime) / (3600 * 24),
      (int)(current_time - login_time) / (3600 * 24),
      ctime(&login_time)
   );
   if (sort)
   {
      proster = new_roster(buf);
      while
      (
         *roster != NULL &&
         roster_compare(&proster, roster) > 0
      )
      {
         roster = &((*roster)->next);
      }
      proster->next = *roster;
      *roster = proster;
   }
   else
   {
      /* Put storage people at end. */
      if (storage)
      {
         while (*roster)
         {
            roster = &((*roster)->next);
         }
      }
      proster = new_roster(buf);
      proster->next = *roster;
      *roster = proster;
   }
}

void remove_from_roster(CHAR_DATA* ch, int type)
{
   ROSTER** list;
   ROSTER*  proster;
   char     first[MAX_INPUT_LENGTH];
   sh_int god;

   if
   (
      ch == NULL ||
      IS_NPC(ch)
   )
   {
      return;
   }
   switch (type)
   {
      default:
      {
         return;
      }
      case (ROSTER_CLAN):
      {
         if (ch->pcdata->clan < 1)
         {
            return;
         }
         list = &(clan_table[ch->pcdata->clan].proster);
         break;
      }
      case (ROSTER_HOUSE):
      {
         sh_int house;

         house = ch->house;
         if
         (
            house == 0 &&
            (
               IS_SET(ch->act2, PLR_IS_ANCIENT) ||
               IS_SET(ch->act2, PLR_IS_ANCIENT_KNOWN)
            )
         )
         {
            house = HOUSE_ANCIENT;
         }
         if (house == 0)
         {
            return;
         }
         list = &(house_table[house].proster);
         break;
      }
      case (ROSTER_BRAND):
      {
         OBJ_DATA* brand = get_eq_char(ch, WEAR_BRAND);

         if (brand == NULL)
         {
            return;
         }
         for (god = 0; worship_table[god].name != NULL; god++)
         {
            if (worship_table[god].vnum_brand == brand->pIndexData->vnum)
            {
               break;
            }
         }
         if (worship_table[god].name == NULL)
         {
            return;
         }
         list = &(worship_table[god].proster);
         break;
      }
      case (ROSTER_WORSHIP):
      {
         if (ch->pcdata->worship == NULL)
         {
            return;
         }
         for (god = 0; worship_table[god].name != NULL; god++)
         {
            if (!str_cmp(ch->pcdata->worship, worship_table[god].name))
            {
               break;
            }
         }
         if (worship_table[god].name == NULL)
         {
            return;
         }
         list = &(worship_table[god].pwroster);
         break;
      }
   }
   if (*list == NULL)
   {
      return;
   }
   one_argument((*list)->data, first);
   if (!str_cmp(ch->name, first))
   {
      proster = *list;
      *list = (*list)->next;
      proster->next = NULL;
      free_roster(proster);
      return;
   }
   while ((*list)->next)
   {
      one_argument((*list)->next->data, first);
      if (!str_cmp(ch->name, first))
      {
         proster = (*list)->next;
         (*list)->next = (*list)->next->next;
         proster->next = NULL;
         free_roster(proster);
         return;
      }
      list = &((*list)->next);
   }
}

void remove_from_rosters(CHAR_DATA* ch)
{
   remove_from_roster(ch, ROSTER_BRAND);
   remove_from_roster(ch, ROSTER_WORSHIP);
   remove_from_roster(ch, ROSTER_HOUSE);
   remove_from_roster(ch, ROSTER_CLAN);
}

void update_roster(CHAR_DATA* ch, bool storage)
{
   sh_int worship = -1;
   sh_int brand   = -1;
   sh_int clan    = 0;
   sh_int house   = 0;
   sh_int god     = 0;
   OBJ_DATA* obj  = NULL;

   if
   (
      ch == NULL ||
      IS_NPC(ch)
   )
   {
      return;
   }
   obj = get_eq_char(ch, WEAR_BRAND);
   if (obj)
   {
      for (god = 0; worship_table[god].name != NULL; god++)
      {
         if (worship_table[god].vnum_brand == obj->pIndexData->vnum)
         {
            break;
         }
      }
      if (worship_table[god].name == NULL)
      {
         bug_trust
         (
            "Brand: %d not in worship table!",
            obj->pIndexData->vnum,
            get_trust(ch)
         );
      }
      else
      {
         brand = god;
      }
   }
   if (ch->pcdata->worship)
   {
      for (god = 0; worship_table[god].name != NULL; god++)
      {
         if (!str_cmp(worship_table[god].name, ch->pcdata->worship))
         {
            break;;
         }
      }
      if (worship_table[god].name != NULL)
      {
         worship = god;
      }
   }
   house = ch->house;
   if
   (
      house == 0 &&
      (
         IS_SET(ch->act2, PLR_IS_ANCIENT) ||
         IS_SET(ch->act2, PLR_IS_ANCIENT_KNOWN)
      )
   )
   {
      house = HOUSE_ANCIENT;
   }

   clan = ch->pcdata->clan;
   if (brand != -1)
   {
      add_to_roster
      (
         ch->name,
         brand,
         current_time - ch->logon + ch->pcdata->probation,
         ch->pcdata->prob_time,
         storage,
         FALSE,
         ROSTER_BRAND,
         current_time,
         TRUE
      );
      /* sort_rosters(&(worship_table[brand].proster), roster_compare); */
   }
   if (worship != -1)
   {
      add_to_roster
      (
         ch->name,
         worship,
         current_time - ch->logon + ch->pcdata->probation,
         ch->pcdata->prob_time,
         storage,
         FALSE,
         ROSTER_WORSHIP,
         current_time,
         TRUE
      );
      /* sort_rosters(&(worship_table[worship].pwroster), roster_compare); */
   }
   if (house)
   {
      add_to_roster
      (
         ch->name,
         house,
         current_time - ch->logon + ch->pcdata->probation,
         ch->pcdata->prob_time,
         storage,
         FALSE,
         ROSTER_HOUSE,
         current_time,
         TRUE
      );
      /* sort_rosters(&(house_table[house].proster), roster_compare); */
   }
   if (clan > 0)
   {
      add_to_roster
      (
         ch->name,
         clan,
         current_time - ch->logon + ch->pcdata->probation,
         ch->pcdata->prob_time,
         storage,
         FALSE,
         ROSTER_CLAN,
         current_time,
         TRUE
      );
      /* sort_rosters(&(clan_table[clan].proster), roster_compare); */
   }
}

int roster_compare(const void* input1, const void* input2)
{
   /*
   Comparator function for rosters
   */
   ROSTER* roster1 = *(ROSTER**)input1;
   ROSTER* roster2 = *(ROSTER**)input2;
   bool denied_1 = strstr(roster1->data, "(Denied)") != NULL;
   bool denied_2 = strstr(roster2->data, "(Denied)") != NULL;
   bool store_1 = strstr(roster1->data, "(Stored)") != NULL;
   bool store_2 = strstr(roster2->data, "(Stored)") != NULL;

   if (denied_1)
   {
      if (!denied_2)
      {
         return 1;
      }
   }
   if (denied_2)
   {
      if (!denied_1)
      {
         return -1;
      }
   }
   if (store_1)
   {
      if (!store_2)
      {
         return 1;
      }
   }
   else if (store_2)
   {
      return -1;
   }
   return strcmp(roster1->data, roster2->data);
}

int time_wipe_roster_compare(const void* input1, const void* input2)
{
   /*
   Comparator function for rosters
   */
   ROSTER* roster1 = *(ROSTER**)input1;
   ROSTER* roster2 = *(ROSTER**)input2;
   bool denied_1 = strstr(roster1->data, "(Denied)") != NULL;
   bool denied_2 = strstr(roster2->data, "(Denied)") != NULL;
   bool store_1 = strstr(roster1->data, "(Stored)") != NULL;
   bool store_2 = strstr(roster2->data, "(Stored)") != NULL;
   int days_1;
   int days_2;
   char name_1 [MAX_INPUT_LENGTH];
   char name_2 [MAX_INPUT_LENGTH];

   if (denied_1)
   {
      if (!denied_2)
      {
         return 1;
      }
   }
   if (denied_2)
   {
      if (!denied_1)
      {
         return -1;
      }
   }
   if (store_1)
   {
      if (!store_2)
      {
         return 1;
      }
   }
   else if (store_2)
   {
      return -1;
   }
   sscanf
   (
      roster1->data,
      "%[^:]: %d Days Ago",
      name_1,
      &days_1
   );
   sscanf
   (
      roster2->data,
      "%[^:]: %d Days Ago",
      name_2,
      &days_2
   );
   if (days_1 > days_2)
   {
      return -1;
   }
   else if (days_2 > days_1)
   {
      return 1;
   }
   return strcmp(name_1, name_2);
}

void sort_rosters(ROSTER** list, int (*compare) (const void*, const void*))
{
   ROSTER* proster   = NULL;
   ROSTER* p_last    = NULL;
   bool sorted       = TRUE;
   sh_int counter    = 0;
   ROSTER** arr   = NULL;
   sh_int arr_size   = 0;

   if
   (
      list == NULL ||
      *list == NULL
   )
   {
      return;
   }
   /*
   find out how many areas we have
   */
   for (proster = *list; proster; proster = proster->next)
   {
      arr_size++;
      if
      (
         p_last &&
         sorted &&
         roster_compare(&proster, &p_last) != 1
      )
      {
         sorted = FALSE;
      }
      p_last = proster;
   }
   if (sorted)
   {
      return;
   }
   /*
   allocate memory for the array
   */
   arr = (ROSTER**) malloc(arr_size * sizeof(ROSTER*));
   /*
   setting up an array of pointers to areas
   */
   counter = 0;
   for (proster = *list; proster; proster = proster->next)
   {
      arr[counter] = proster;
      counter++;
   }
   qsort(arr, arr_size, sizeof(ROSTER*), compare);
   *list = arr[0];
   proster = *list;
   proster->next = arr[1];
   for (counter = 2; counter < arr_size; counter++)
   {
      proster = arr[counter - 1];
      proster->next = arr[counter];
   }
   proster = proster->next;
   proster->next = NULL;
   free(arr);
}

void sort_all_rosters()
{
   sh_int counter;

   for (counter = 0; counter < MAX_HOUSE; counter++)
   {
      sort_rosters(&(house_table[counter].proster), roster_compare);
   }
   for (counter = 0; counter < MAX_CLAN; counter++)
   {
      sort_rosters(&(clan_table[counter].proster), roster_compare);
   }
   for (counter = 0; worship_table[counter].name != NULL; counter++)
   {
      sort_rosters(&(worship_table[counter].proster), roster_compare);
      sort_rosters(&(worship_table[counter].pwroster), roster_compare);
   }
   sort_rosters(&tw_roster, time_wipe_roster_compare);
}

void add_to_time_wipe_roster(char* name, bool storage, bool denied, long login_time, char* filename)
{
   ROSTER** roster = &tw_roster;
   ROSTER*  proster;
   char buf[MAX_STRING_LENGTH];
   /*
   set log_stored to TRUE to also list
   people in storage that are old
   */
   bool log_stored = TRUE;

   if
   (
      !denied &&
      (
         (
            (
               current_time - login_time
            ) /
            (3600 * 24)
         ) < 30 ||
         (
            !log_stored &&
            storage
         ) ||
         (
            storage &&
            (
               (
                  current_time - login_time
               ) /
               (3600 * 24)
            ) < (30 * 6)  /* 6 months storage */
         )
      )
   )
   {
      return;
   }
   sprintf
   (
      buf,
      "%s%-12s : %4ld Days Ago : %s",
      (
         denied ? "(Denied) " :
         log_stored && storage ?
         "(Stored) " :
         "         "
      ),
      name,
      (current_time - login_time) / (3600 * 24),
      filename
   );
   proster = new_roster(buf);
   proster->next = *roster;
   *roster = proster;
}

/*
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
*/

/*
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
*/

/*
#define TO_AFFECTS    0
#define TO_OBJECT    1
#define TO_IMMUNE    2
#define TO_RESIST    3
#define TO_VULN        4
#define TO_WEAPON    5
*/

struct alchem_info
{
   int ingredient[4];
   sh_int apply_char[33];
   long immune;
   long resist;
   long vuln;
   long weapon;
   int crumble_time;
   int restricted;
   long   affected;
   bool special[15];
   bool valid;
};

typedef struct alchem_info ALCHEM_INFO;

void clean_alchem(ALCHEM_INFO* data)
{
   sh_int counter;

   for (counter = 0; counter < 4; counter++)
   {
      data->ingredient[counter] = 0;
   }
   for (counter = 0; counter < 33; counter++)
   {
      data->apply_char[counter] = 0;
   }
   for (counter = 0; counter < 15; counter++)
   {
      data->special[counter] = 0;
   }
   data->immune = 0;
   data->resist = 0;
   data->vuln = 0;
   data->weapon = 0;
   data->crumble_time = 0;
   data->restricted = 0;
   data->affected = 0;
   data->valid = FALSE;
}

char* mat_name(int* num, char* buf)
{
   buf[0] = '\0';
   if (num[0] == -1)
   {
      strcat(buf, "NONE\n");
      return buf;
   }
   strcat(buf, material_table[num[0] + 1].name);
   if (num[0] >= 65 && num[0] <= 68)
   {
      strcat(buf, "*");
   }
   if (num[1] == -1)
   {
      strcat(buf, "\n");
      return buf;
   }
   strcat(buf, ", ");
   strcat(buf, material_table[num[1] + 1].name);
   if (num[1] >= 65 && num[1] <= 68)
   {
      strcat(buf, "*");
   }
   if (num[2] == -1)
   {
      strcat(buf, "\n");
      return buf;
   }
   strcat(buf, ", ");
   strcat(buf, material_table[num[2] + 1].name);
   if (num[2] >= 65 && num[2] <= 68)
   {
      strcat(buf, "*");
   }
   if (num[3] == -1)
   {
      strcat(buf, "\n");
      return buf;
   }
   strcat(buf, ", ");
   strcat(buf, material_table[num[3] + 1].name);
   if (num[3] >= 65 && num[3] <= 68)
   {
      strcat(buf, "*");
   }
   strcat(buf, "\n");
   return buf;
}

char* get_mat_name(int part, char* buf)
{
   int material[4];

   material[0] = formula_table[part].ingredient1;
   material[1] = formula_table[part].ingredient2;
   material[2] = formula_table[part].ingredient3;
   material[3] = formula_table[part].ingredient4;
   return (mat_name(material, buf));
}

/*
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
*/

void add_to_alchem(ALCHEM_INFO* data, int counter)
{
   data->valid = TRUE;
   if (formula_table[counter].restrict != -1)
   {
      data->restricted |= formula_table[counter].restrict;
   }
   if (formula_table[counter].special != 0)
   {
      data->special[formula_table[counter].special] = TRUE;
   }
   if (formula_table[counter].crumble_time != 0)
   {
      data->crumble_time = formula_table[counter].crumble_time;
   }
   data->apply_char[formula_table[counter].apply_loc] += formula_table[counter].apply_mod;
   switch (formula_table[counter].apply_where)
   {
      case (TO_OBJECT):
      {
         break;
      }
      case (TO_AFFECTS):
      {
         data->affected |= formula_table[counter].apply_bits;
         break;
      }
      case (TO_RESIST):
      {
         data->resist |= formula_table[counter].apply_bits;
         break;
      }
      case (TO_IMMUNE):
      {
         data->immune |= formula_table[counter].apply_bits;
         break;
      }
      case (TO_VULN):
      {
         data->vuln |= formula_table[counter].apply_bits;
         break;
      }
   }
}

char *special_name(bool* special)
{
   static char buf[512];
   static char buf2[512];

   buf[0] = '\0';
   buf2[0] = '\0';

   if (special[1])
   {
      strcat(buf, ", Flaming Bite");
   }
   if (special[2])
   {
      strcat(buf, ", Freezing Bite");
   }
   if (special[3])
   {
      strcat(buf, ", Deluge");
   }
   if (special[4])
   {
      strcat(buf, ", Shocking Bite");
   }
   if (special[5])
   {
      strcat(buf, ", Half Weight");
   }
   if (special[6])
   {
      strcat(buf, ", Double Weight");
   }
   if (special[7])
   {
      strcat(buf, ", Value Add");
   }
   if (special[8])
   {
      strcat(buf, ", Rot Death");
   }
   if (special[9])
   {
      strcat(buf, ", No_locate");
   }
   if (special[10])
   {
      strcat(buf, ", Invis(object)");
   }
   if (special[11])
   {
      strcat(buf, ", Norem");
   }
   if (special[12])
   {
      strcat(buf, ", Nodrop");
   }
   if (special[13])
   {
      strcat(buf, ", Burnproof");
   }
   if (special[14])
   {
      strcat(buf, ", Acidic Bite");
   }
   if (buf[0] == '\0')
   {
      return "";
   }
   strcat(buf2, "   Specials: ");
   strcat(buf2, buf + 2);
   strcat(buf2, "\n");
   return buf2;
}

void dump_alchemy(ALCHEM_INFO* data, FILE* fp, char* name_material)
{
   sh_int cnt;

   if (!(data->valid))
   {
      return;
   }


   /*
   struct alchem_info
   {
   int ingredient[4];***
   sh_int apply_char[33];***
   long immune;***
   long resist;***
   long vuln;***
   long weapon;***
   int crumble_time;***
   int restricted;***
   int special;
   long   affected;***
   bool special[15];
   bool valid;***
   };
   */
   fprintf
   (
      fp,
      "%s",
      name_material
   );
   if (data->crumble_time)
   {
      fprintf
      (
         fp,
         "   Lasts for %d ticks.\n",
         data->crumble_time
      );
   }
   if (data->immune)
   {
      fprintf
      (
         fp,
         "   Immunity to: %s\n",
         imm_bit_name(data->immune)
      );
   }
   if (data->resist)
   {
      fprintf
      (
         fp,
         "   Resistance to: %s\n",
         imm_bit_name(data->resist)
      );
   }
   if (data->vuln)
   {
      fprintf
      (
         fp,
         "   Vulnerability to: %s\n",
         imm_bit_name(data->vuln)
      );
   }
   if (data->weapon)
   {
      fprintf
      (
         fp,
         "   Weapon Flags: %s\n",
         weapon_bit_name(data->weapon)
      );
   }
   fprintf
   (
      fp,
      "%s",
      special_name(data->special)
   );

   if (data->restricted)
   {
      fprintf
      (
         fp,
         "   Restricted to: %s.\n",
         data->restricted == ITEM_WEAR_FINGER ? "fingers" :
         data->restricted == ITEM_WEAR_NECK ? "neck" :
         data->restricted == ITEM_WEAR_BODY ? "torso" :
         data->restricted == ITEM_WEAR_HEAD ? "head" :
         data->restricted == ITEM_WEAR_LEGS ? "legs" :
         data->restricted == ITEM_WEAR_FEET ? "feet" :
         data->restricted == ITEM_WEAR_HANDS ? "hands" :
         data->restricted == ITEM_WEAR_ARMS ? "arms" :
         data->restricted == ITEM_WEAR_SHIELD ? "shield" :
         data->restricted == ITEM_WEAR_ABOUT ? "body" :
         data->restricted == ITEM_WEAR_WAIST ? "waist" :
         data->restricted == ITEM_WEAR_WRIST ? "wrist" :
         data->restricted == ITEM_WIELD ? "weapon" :
         data->restricted == ITEM_HOLD ? "held" : "ERROR"
      );
   }
   if (data->affected)
   {
      fprintf
      (
         fp,
         "   Affects: %s\n",
         affect_bit_name(data->affected, 0)
      );
   }
   for (cnt = 0; cnt < 33; cnt++)
   {
      if (data->apply_char[cnt])
      {
         fprintf
         (
            fp,
            "   Affects %s by %d.\n",
            affect_loc_name(cnt),
            data->apply_char[cnt]
         );
      }
   }


}

void dump_time_wipe_roster()
{
   char strsave[MAX_INPUT_LENGTH];
   FILE *fp;
   ROSTER* proster = tw_roster;
   sh_int count = 0;
   sh_int run = 0;

   log_string("Creating time_wipe list");
   sprintf(strsave, "%stime_wipe.lst", PLAYER_DIR);
   fclose(fpReserve);
   if
   (
      (
         fp = fopen(TEMP_FILE, "w")
      ) == NULL
   )
   {
      bug("dump_time_wipe_roster: fopen", 0);
      perror(strsave);
   }
   else
   {
      if (proster == NULL)
      {
         fprintf(fp, "None\n");
      }
      else
      {
         while (proster)
         {
            /*
            Reset counts at each type
            Player, not denied. (0)
            Stored, not denied. (1)
            Player, Denied.     (2)
            Stored, Denied.     (3)
            */
            switch (run)
            {
               case (0):
               {
                  if (strstr(proster->data, "(Stored)") != NULL)
                  {
                     count = 0;
                     if (strstr(proster->data, "(Denied)") != NULL)
                     {
                        run = 3;
                     }
                     else
                     {
                        run = 1;
                     }
                  }
                  else if (strstr(proster->data, "(Denied)") != NULL)
                  {
                     count = 0;
                     run = 2;
                  }
                  break;
               }
               case (1):
               {
                  if (strstr(proster->data, "(Denied)") != NULL)
                  {
                     count = 0;
                     if (strstr(proster->data, "(Stored)") != NULL)
                     {
                        run = 3;
                     }
                     else
                     {
                        run = 2;
                     }
                  }
                  break;
               }
               case (2):
               {
                  if (strstr(proster->data, "(Stored)") != NULL)
                  {
                     count = 0;
                     run = 3;
                  }
                  break;
               }
               default:  /* 3 */
               {
                  break;
               }
            }
            fprintf(fp, "%4d %s\n", ++count, proster->data);
            proster = proster->next;
         }
      }
      fclose(fp);
      rename(TEMP_FILE, strsave);
   }
   fpReserve = fopen(NULL_FILE, "r");
   free_roster(tw_roster);
   tw_roster = NULL;
   return;
}

void do_dump_alchem(CHAR_DATA* ch, char* argument)
{
   send_to_char("If you want to DUMPALCHEMY, spell it out!\n\r", ch);
}

void do_dump_alchemy(CHAR_DATA* ch, char* argument)
{
   char strsave[MAX_INPUT_LENGTH];
   FILE *fp;
   ALCHEM_INFO data;
   int counter;
   char last[MAX_INPUT_LENGTH];
   char curr[MAX_INPUT_LENGTH];

   if (ch->desc == NULL)
   {
      return;
   }
   if (is_test)
   {
      send_to_char("Dumping cannot be done on test port.\n\r", ch);
      return;
   }
   if (str_cmp(argument, "now"))
   {
      send_to_char
      (
         "Syntax:\n\r"
         "  dumpalchemy now\n\r"
         "Spell out fully to prevent mistakes.\n\r",
         ch
      );
      return;
   }
   last[0] = '\0';
   clean_alchem(&data);
   sprintf(strsave, "%salchemy.lst", PLAYER_DIR);
   fclose(fpReserve);
   if
   (
      (
         fp = fopen(TEMP_FILE, "w")
      ) == NULL
   )
   {
      send_to_char("Failed to dump alchemy formulae.\n\r", ch);
      bug("dump_time_wipe_roster: fopen", 0);
      perror(strsave);
   }
   else
   {
      sprintf
      (
         log_buf,
         "%s: Dumping Alchemy",
         (
            ch->desc->original ?
            ch->desc->original->name :
            ch->name
         )
      );
      log_string(log_buf);
      send_to_char("All alchemy formulae dumped to ", ch);
      send_to_char(strsave, ch);
      send_to_char("\n\r", ch);
      for (counter = 0; formula_table[counter].ingredient1 != -1; counter++)
      {
         if (strcmp(last, get_mat_name(counter, curr)))
         {
            dump_alchemy(&data, fp, last);
            strcpy(last, curr);
            clean_alchem(&data);
         }
         add_to_alchem(&data, counter);
      }
      fclose(fp);
      rename(TEMP_FILE, strsave);
   }
   fpReserve = fopen(NULL_FILE, "r");
   return;
}

void dynamically_load_area(CHAR_DATA* ch, char* area_name)
{
   bool  old_fBootDb = fBootDb;
   char* word;
   int   error = 0;

   fBootDb = FALSE;

   strcpy(strArea, AREA_DIR);
   strcat(strArea, area_name);
   if ((fpArea = fopen(strArea, "r")) == NULL)
   {
      send_to_char("Unable to open the specified area.\n\r", ch);
      fBootDb = old_fBootDb;
      return;
   }

   for (; ;)
   {
      if (fread_letter(fpArea) != '#')
      {
         bug("Boot_area in dynamically_load_area: # not found.", 0);
         error = -1;
         break;
      }
      word = fread_word( fpArea );
      if (word[0] == '$')
      {
         free_string(word);
         break;
      }
      else if (!str_cmp(word, "AREA"))
      {
         load_area(fpArea);
      }
      else if (!str_cmp(word, "MUSIC"))
      {
         load_area_music(fpArea);
      }
      else if (!str_cmp(word, "HELPS"))
      {
         load_helps(fpArea);
      }
      else if (!str_cmp(word, "MOBOLD"))
      {
         load_old_mob(fpArea);
      }
      else if (!str_cmp(word, "MOBILES"))
      {
         load_mobiles(fpArea);
      }
      else if (!str_cmp(word, "MOBPROGS"))
      {
         load_mobprogs(fpArea);
      }
      else if (!str_cmp(word, "OBJOLD"))
      {
         load_old_obj(fpArea); }
      else if (!str_cmp(word, "OBJECTS"))
      {
         load_objects(fpArea);
      }
      else if (!str_cmp(word, "RESETS"))
      {
         load_resets(fpArea);
      }
      else if (!str_cmp(word, "ROOMS"))
      {
         load_rooms(fpArea);
      }
      else if (!str_cmp(word, "SHOPS"))
      {
         load_shops(fpArea);
      }
      else if (!str_cmp(word, "SOCIALS"))
      {
         load_socials(fpArea);
      }
      else if (!str_cmp(word, "SPECIALS"))
      {
         load_specials(fpArea);
      }
      else if (!str_cmp(word, "QUESTS"))
      {
         load_quests(fpArea);
      }
      else
      {
         send_to_char("Boot_area in dynamically_load_area: bad section name.", ch);
         error = -2;
         break;
      }
      free_string(word);
   }

   fclose(fpArea);

   if (error == 0)
   {
      extern const sh_int rev_dir [];
      char buf[MAX_STRING_LENGTH];
      ROOM_INDEX_DATA *pRoomIndex;
      AREA_DATA* parea;
      ROOM_INDEX_DATA *to_room;
      EXIT_DATA *pexit;
      EXIT_DATA *pexit_rev;
      int door;
      bool fexit;

      send_to_char("Area loaded.\n\r", ch);

      fBootDb  = FALSE;

      boot_positions();
      update_time();
      /*fix_exits();*/


      for (parea = area_first; parea; parea = parea->next)
      if (!str_cmp(parea->file_name, area_name))
      {
         for
         (
            pRoomIndex = parea->rooms;
            pRoomIndex;
            pRoomIndex = pRoomIndex->next_in_area
         )
         {
            fexit = FALSE;
            for (door = 0; door < 6; door++)
            {
               if
               (
                  (
                     pexit = pRoomIndex->exit[door]
                  ) != NULL
               )
               {
                  if
                  (
                     pexit->u1.vnum <= 0 ||
                     get_room_index(pexit->u1.vnum) == NULL
                  )
                  {
                     pexit->u1.to_room = NULL;
                  }
                  else
                  {
                     fexit = TRUE;
                     pexit->u1.to_room = get_room_index(pexit->u1.vnum);
                  }
               }
            }
            if (!fexit)
            {
               SET_BIT(pRoomIndex->room_flags, ROOM_NO_MOB);
            }
         }
      }
      /* Pointers are not real untill here. */
      for (parea = area_first; parea; parea = parea->next)
      if (!str_cmp(parea->file_name, area_name))
      {
         for
         (
            pRoomIndex = parea->rooms;
            pRoomIndex;
            pRoomIndex = pRoomIndex->next_in_area
         )
         {
            for (door = 0; door < 6; door++)
            {
               if
               (
                  (
                     pexit = pRoomIndex->exit[door]
                  ) != NULL &&
                  (
                     to_room = pexit->u1.to_room
                  ) != NULL &&
                  (
                     pexit_rev = to_room->exit[rev_dir[door]]
                  ) != NULL &&
                  pexit_rev->u1.to_room != pRoomIndex &&
                  (
                     pRoomIndex->vnum < 1200 ||
                     pRoomIndex->vnum > 1299
                  )
               )
               {
                  sprintf
                  (
                     buf,
                     "Fix_exits: %d:%d -> %d:%d -> %d.",
                     pRoomIndex->vnum,
                     door,
                     to_room->vnum,
                     rev_dir[door],
                     pexit_rev->u1.to_room == NULL ?
                     0 :
                     pexit_rev->u1.to_room->vnum
                  );
                  log_string(buf);
               }
            }
         }
      }


      update_time();
      fix_mobprogs( );
      fBootDb = FALSE;
      update_time();
      area_update();
      update_time();
   }

   fBootDb = old_fBootDb;
}
