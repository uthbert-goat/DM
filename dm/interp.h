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

static const char interp_h_rcsid[] = "$Id: interp.h,v 1.93 2004/11/25 08:52:20 fizzfaldt Exp $";

/* this is a listing of all the commands and command related data */

/* for command types */
#define ML    MAX_LEVEL        /* implementor */
#define L1    MAX_LEVEL - 1    /* creator */
#define L2    MAX_LEVEL - 2    /* supreme being */
#define L3    MAX_LEVEL - 3    /* deity */
#define L4    MAX_LEVEL - 4    /* god */
#define L5    MAX_LEVEL - 5    /* immortal */
#define L6    MAX_LEVEL - 6    /* demigod */
#define L7    MAX_LEVEL - 7    /* angel */
#define L8    MAX_LEVEL - 8    /* avatar */
#define IM    LEVEL_IMMORTAL   /* avatar */
#define HE    LEVEL_HERO       /* hero */

#define COM_INGORE    1


/*
 * Structure for a command in the command lookup table.
 */
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

/* the command table itself */ /* XUR -- had const after extern */
extern        struct    cmd_type    cmd_table    [];

/*
 * Command functions.
 * Defined in act_*.c (mostly).
 */
DECLARE_DO_FUN(do_acute_vision);
DECLARE_DO_FUN(do_ambush);
DECLARE_DO_FUN(do_advance);
DECLARE_DO_FUN(do_affects);
DECLARE_DO_FUN(do_alia);
DECLARE_DO_FUN(do_alias);
DECLARE_DO_FUN(do_allow);
DECLARE_DO_FUN(do_answer);
DECLARE_DO_FUN(do_areas);
DECLARE_DO_FUN(do_assess);
DECLARE_DO_FUN(do_at);
DECLARE_DO_FUN(do_auction);
DECLARE_DO_FUN(do_autoassist);
DECLARE_DO_FUN(do_autoexit);
DECLARE_DO_FUN(do_autogold);
DECLARE_DO_FUN(do_autolist);
DECLARE_DO_FUN(do_autoloot);
DECLARE_DO_FUN(do_autoreboot);
DECLARE_DO_FUN(do_autosac);
DECLARE_DO_FUN(do_autosplit);
DECLARE_DO_FUN(do_backstab);
DECLARE_DO_FUN(do_bamfin);
DECLARE_DO_FUN(do_bamfout);
DECLARE_DO_FUN(do_ban);
DECLARE_DO_FUN(do_bash);
DECLARE_DO_FUN(do_battlecry);
DECLARE_DO_FUN(do_whirlwind);
DECLARE_DO_FUN(do_berserk);
DECLARE_DO_FUN(do_brands);
DECLARE_DO_FUN(do_breath_morph);
DECLARE_DO_FUN(do_brandish);
DECLARE_DO_FUN(do_brief);
DECLARE_DO_FUN(do_bug);
DECLARE_DO_FUN(do_buy);
DECLARE_DO_FUN(do_camouflage);
DECLARE_DO_FUN(do_camp);
DECLARE_DO_FUN(do_cast);
DECLARE_DO_FUN(do_supplicate);
DECLARE_DO_FUN(do_cast_non_clergy);
DECLARE_DO_FUN(do_cast_silent);
DECLARE_DO_FUN(do_cforsaken);
DECLARE_DO_FUN(do_changes);
DECLARE_DO_FUN(do_channels);
DECLARE_DO_FUN(do_checkip);
DECLARE_DO_FUN(do_circle_stab);
DECLARE_DO_FUN(do_clone);
DECLARE_DO_FUN(do_close);
DECLARE_DO_FUN(do_color);
DECLARE_DO_FUN(do_color_reduce);
DECLARE_DO_FUN(do_commands);
DECLARE_DO_FUN(do_combine);
DECLARE_DO_FUN(do_compact);
DECLARE_DO_FUN(do_compare);
DECLARE_DO_FUN(do_consider);
DECLARE_DO_FUN(do_count);
DECLARE_DO_FUN(do_credits);
DECLARE_DO_FUN(do_cripple);
DECLARE_DO_FUN(do_deaf);
DECLARE_DO_FUN(do_deathstyle);
DECLARE_DO_FUN(do_storag);
DECLARE_DO_FUN(do_moonphase);
DECLARE_DO_FUN(do_storage);
DECLARE_DO_FUN(do_delet);
DECLARE_DO_FUN(do_delete);
DECLARE_DO_FUN(do_deny);
DECLARE_DO_FUN(do_description);
DECLARE_DO_FUN(do_description_type);
DECLARE_DO_FUN(do_book_description);
DECLARE_DO_FUN(do_detect_hidden);
DECLARE_DO_FUN(do_blind_fighting);
DECLARE_DO_FUN(do_dirt);
DECLARE_DO_FUN(do_sm);
DECLARE_DO_FUN(do_smoke_screen);
DECLARE_DO_FUN(do_disarm);
DECLARE_DO_FUN(do_disconnect);
DECLARE_DO_FUN(do_down);
DECLARE_DO_FUN(do_drain);
DECLARE_DO_FUN(do_drink);
DECLARE_DO_FUN(do_drop);
DECLARE_DO_FUN(do_dump);
DECLARE_DO_FUN(do_east);
DECLARE_DO_FUN(do_eat);
DECLARE_DO_FUN(do_echo);
DECLARE_DO_FUN(do_emote);
DECLARE_DO_FUN(do_empower);
DECLARE_DO_FUN(do_envenom);
DECLARE_DO_FUN(do_equipment);
DECLARE_DO_FUN(do_examine);
DECLARE_DO_FUN(do_exits);
DECLARE_DO_FUN(do_extitle);
DECLARE_DO_FUN(do_feed);
DECLARE_DO_FUN(do_fence);
DECLARE_DO_FUN(do_dice_roll); /* Wicket */
DECLARE_DO_FUN(do_immerse); /* Wicket */
DECLARE_DO_FUN(do_fill);
DECLARE_DO_FUN(do_flag);
DECLARE_DO_FUN(do_flee);
DECLARE_DO_FUN(do_flip);
DECLARE_DO_FUN(do_toke);
DECLARE_DO_FUN(do_follow);
DECLARE_DO_FUN(do_force);
DECLARE_DO_FUN(do_educate);
DECLARE_DO_FUN(do_forsake);
DECLARE_DO_FUN(do_freetell);
DECLARE_DO_FUN(do_freeze);
DECLARE_DO_FUN(do_gain);
DECLARE_DO_FUN(do_get);
DECLARE_DO_FUN(do_getip);
DECLARE_DO_FUN(do_getpk);
DECLARE_DO_FUN(do_getprac);
DECLARE_DO_FUN(do_getquest);
DECLARE_DO_FUN(do_give);
DECLARE_DO_FUN(do_glance);
DECLARE_DO_FUN(do_goto);
DECLARE_DO_FUN(do_group);
DECLARE_DO_FUN(do_groups);
DECLARE_DO_FUN(do_gtell);
DECLARE_DO_FUN(do_hang);
DECLARE_DO_FUN(do_heal);
DECLARE_DO_FUN(do_help);
DECLARE_DO_FUN(do_hit);
DECLARE_DO_FUN(do_hide);
DECLARE_DO_FUN(do_holylight);
DECLARE_DO_FUN(do_hordelist);
DECLARE_DO_FUN(do_idea);
DECLARE_DO_FUN(do_ignore);
DECLARE_DO_FUN(do_immtalk);
DECLARE_DO_FUN(do_builder);
DECLARE_DO_FUN(do_incognito);
DECLARE_DO_FUN(do_induct);
DECLARE_DO_FUN(do_leader);
DECLARE_DO_FUN(do_guildmaster);
DECLARE_DO_FUN(do_noguild);
DECLARE_DO_FUN(do_learnlvl);
DECLARE_DO_FUN(do_housetalk);
DECLARE_DO_FUN(do_imotd);
DECLARE_DO_FUN(do_inventory);
DECLARE_DO_FUN(do_invis);
DECLARE_DO_FUN(do_kick);
DECLARE_DO_FUN(do_kill);
DECLARE_DO_FUN(do_laying_hands);
DECLARE_DO_FUN(do_limits);
DECLARE_DO_FUN(do_list);
DECLARE_DO_FUN(do_load);
DECLARE_DO_FUN(do_lock);
DECLARE_DO_FUN(do_log);
DECLARE_DO_FUN(do_look);
DECLARE_DO_FUN(do_lore);
DECLARE_DO_FUN(do_makelich);
DECLARE_DO_FUN(do_mark);
DECLARE_DO_FUN(do_memory);
DECLARE_DO_FUN(do_mfind);
DECLARE_DO_FUN(do_mload);
DECLARE_DO_FUN(do_mob);
DECLARE_DO_FUN(do_morph_dragon);
DECLARE_DO_FUN(do_mpstat);
DECLARE_DO_FUN(do_mpdump);
DECLARE_DO_FUN(do_mset);
DECLARE_DO_FUN(do_mstat);
DECLARE_DO_FUN(do_mwhere);
DECLARE_DO_FUN(do_moron);
DECLARE_DO_FUN(do_motd);
DECLARE_DO_FUN(do_multicheck);
DECLARE_DO_FUN(do_murde);
DECLARE_DO_FUN(do_murder);
DECLARE_DO_FUN(do_nameallow);
DECLARE_DO_FUN(do_nameban);
DECLARE_DO_FUN(do_namepermban);
DECLARE_DO_FUN(do_newlock);
DECLARE_DO_FUN(do_news);
/* DECLARE_DO_FUN(do_newfight); */
DECLARE_DO_FUN(do_nochannels);
DECLARE_DO_FUN(do_noemote);
DECLARE_DO_FUN(do_nofollow);
DECLARE_DO_FUN(do_nopray);
DECLARE_DO_FUN(do_north);
DECLARE_DO_FUN(do_noshout);
DECLARE_DO_FUN(do_nosummon);
DECLARE_DO_FUN(do_helpnote);
DECLARE_DO_FUN(do_note);
DECLARE_DO_FUN(do_notell);
DECLARE_DO_FUN(do_offer);
DECLARE_DO_FUN(do_ofind);
DECLARE_DO_FUN(do_oload);
DECLARE_DO_FUN(do_ooctell);
DECLARE_DO_FUN(do_open);
DECLARE_DO_FUN(do_order);
DECLARE_DO_FUN(do_oset);
DECLARE_DO_FUN(do_ostat);
DECLARE_DO_FUN(do_outfit);
DECLARE_DO_FUN(do_overmax);
DECLARE_DO_FUN(do_owhere);
DECLARE_DO_FUN(do_pardon);
DECLARE_DO_FUN(do_password);
DECLARE_DO_FUN(do_peace);
DECLARE_DO_FUN(do_pecho);
DECLARE_DO_FUN(do_hecho);
DECLARE_DO_FUN(do_becho);
DECLARE_DO_FUN(do_penalty);
DECLARE_DO_FUN(do_permban);
DECLARE_DO_FUN(do_permnameban);
DECLARE_DO_FUN(do_pick);
DECLARE_DO_FUN(do_pmote);
DECLARE_DO_FUN(do_pose);
DECLARE_DO_FUN(do_pour);
DECLARE_DO_FUN(do_practice);
DECLARE_DO_FUN(do_pray);
DECLARE_DO_FUN(do_prefi);
DECLARE_DO_FUN(do_prefix);
DECLARE_DO_FUN(do_probation);
DECLARE_DO_FUN(do_prompt);
DECLARE_DO_FUN(do_protect);
DECLARE_DO_FUN(do_purge);
DECLARE_DO_FUN(do_put);
DECLARE_DO_FUN(do_quaff);
DECLARE_DO_FUN(do_quest);
DECLARE_DO_FUN(do_question);
DECLARE_DO_FUN(do_qui);
DECLARE_DO_FUN(do_quiet);
DECLARE_DO_FUN(do_quit);
DECLARE_DO_FUN(do_quote);
DECLARE_DO_FUN(do_read);
DECLARE_DO_FUN(do_reboo);
DECLARE_DO_FUN(do_reboot);
DECLARE_DO_FUN(do_recall);
DECLARE_DO_FUN(do_recho);
DECLARE_DO_FUN(do_recite);
DECLARE_DO_FUN(do_remove);
DECLARE_DO_FUN(do_rent);
DECLARE_DO_FUN(do_replay);
DECLARE_DO_FUN(do_reply);
DECLARE_DO_FUN(do_report);
DECLARE_DO_FUN(do_rescue);
DECLARE_DO_FUN(do_rest);
DECLARE_DO_FUN(do_restore);
DECLARE_DO_FUN(do_return);
DECLARE_DO_FUN(do_reward);
DECLARE_DO_FUN(do_rset);
DECLARE_DO_FUN(do_rstat);
DECLARE_DO_FUN(do_rules);
DECLARE_DO_FUN(do_sacrifice);
DECLARE_DO_FUN(do_save);
DECLARE_DO_FUN(do_say);
DECLARE_DO_FUN(do_scan);
DECLARE_DO_FUN(do_score);
DECLARE_DO_FUN(do_scroll);
DECLARE_DO_FUN(do_see);
DECLARE_DO_FUN(do_nosee);
DECLARE_DO_FUN(do_seehouse);
DECLARE_DO_FUN(do_seebrand);
DECLARE_DO_FUN(do_seeworship);
DECLARE_DO_FUN(do_seeclan);
DECLARE_DO_FUN(do_sell);
DECLARE_DO_FUN(do_set);
DECLARE_DO_FUN(do_show);
DECLARE_DO_FUN(do_shutdow);
DECLARE_DO_FUN(do_shutdown);
DECLARE_DO_FUN(do_silence);
DECLARE_DO_FUN(do_sit);
DECLARE_DO_FUN(do_skills);
DECLARE_DO_FUN(do_skin);
DECLARE_DO_FUN(do_sla);
DECLARE_DO_FUN(do_slay);
DECLARE_DO_FUN(do_sleep);
DECLARE_DO_FUN(do_slookup);
DECLARE_DO_FUN(do_smite);
DECLARE_DO_FUN(do_smote);
DECLARE_DO_FUN(do_sneak);
DECLARE_DO_FUN(do_snoop);
DECLARE_DO_FUN(do_socials);
DECLARE_DO_FUN(do_soulscry);
DECLARE_DO_FUN(do_diagnose);
DECLARE_DO_FUN(do_south);
DECLARE_DO_FUN(do_sockets);
DECLARE_DO_FUN(do_spells);
DECLARE_DO_FUN(do_split);
DECLARE_DO_FUN(do_sset);
DECLARE_DO_FUN(do_stand);
DECLARE_DO_FUN(do_stat);
DECLARE_DO_FUN(do_steal);
DECLARE_DO_FUN(do_plant); /* New thief skill -- Wicket */
DECLARE_DO_FUN(do_steed);
DECLARE_DO_FUN(do_steel_nerves);
DECLARE_DO_FUN(do_story);
DECLARE_DO_FUN(do_string);
DECLARE_DO_FUN(do_surrender);
DECLARE_DO_FUN(do_switch);
DECLARE_DO_FUN(do_tell);
DECLARE_DO_FUN(do_time);
DECLARE_DO_FUN(do_title);
DECLARE_DO_FUN(do_train);
DECLARE_DO_FUN(do_transfer);
DECLARE_DO_FUN(do_trip);
DECLARE_DO_FUN(do_trust);
DECLARE_DO_FUN(do_typo);
DECLARE_DO_FUN(do_unalias);
DECLARE_DO_FUN(do_unbrands);
DECLARE_DO_FUN(do_unlock);
DECLARE_DO_FUN(do_unread);
DECLARE_DO_FUN(do_up);
DECLARE_DO_FUN(do_use);
DECLARE_DO_FUN(do_value);
DECLARE_DO_FUN(do_visible);
DECLARE_DO_FUN(do_violate);
DECLARE_DO_FUN(do_vnum);
DECLARE_DO_FUN(do_vnumlist);
DECLARE_DO_FUN(do_wake);
DECLARE_DO_FUN(do_wear);
DECLARE_DO_FUN(do_weather);
DECLARE_DO_FUN(do_west);
DECLARE_DO_FUN(do_where);
DECLARE_DO_FUN(do_who);
DECLARE_DO_FUN(do_whois);
DECLARE_DO_FUN(do_wimpy);
DECLARE_DO_FUN(do_wizhelp);
DECLARE_DO_FUN(do_rphelp);
DECLARE_DO_FUN(do_wizlock);
DECLARE_DO_FUN(do_wizlist);
DECLARE_DO_FUN(do_wiznet);
DECLARE_DO_FUN(do_worth);
DECLARE_DO_FUN(do_yell);
DECLARE_DO_FUN(do_zap);
DECLARE_DO_FUN(do_zecho);

/* Here down added for DM by ceran , a pile above to, but i forget which
*/
DECLARE_DO_FUN(do_revert);

DECLARE_DO_FUN(do_barkskin);
DECLARE_DO_FUN(do_shadowgate);
DECLARE_DO_FUN(do_shroud);
DECLARE_DO_FUN(do_eye_of_the_predator);
DECLARE_DO_FUN(do_animal_call);

DECLARE_DO_FUN(do_vigilance);
DECLARE_DO_FUN(do_lunge);
DECLARE_DO_FUN(do_feign_death);
DECLARE_DO_FUN(do_critical_strike);
DECLARE_DO_FUN(do_blackjack);
DECLARE_DO_FUN(do_force_reset);
DECLARE_DO_FUN(do_request);
DECLARE_DO_FUN(do_bandage);
DECLARE_DO_FUN(do_herb);
DECLARE_DO_FUN(do_cleave);
DECLARE_DO_FUN(do_deathstrike);
DECLARE_DO_FUN(do_wanted);
DECLARE_DO_FUN(do_aura_of_sustenance);
DECLARE_DO_FUN(do_halo_of_the_sun);

DECLARE_DO_FUN(do_astrip);    /* Imm command */
DECLARE_DO_FUN(do_butcher);
DECLARE_DO_FUN(do_balance);
DECLARE_DO_FUN(do_deposit);
DECLARE_DO_FUN(do_withdraw);
DECLARE_DO_FUN(do_battleshield);
DECLARE_DO_FUN(do_limcounter);
DECLARE_DO_FUN(do_crush);
DECLARE_DO_FUN(do_vanish);
DECLARE_DO_FUN(do_riot);
DECLARE_DO_FUN(do_embalm);
DECLARE_DO_FUN(do_alist);

DECLARE_DO_FUN(do_release);
DECLARE_DO_FUN(do_call_crusader);
DECLARE_DO_FUN(do_release_crusader);

DECLARE_DO_FUN(do_classes);
DECLARE_DO_FUN(do_bear_call);

DECLARE_DO_FUN(do_trophy);
DECLARE_DO_FUN(do_emblam);

DECLARE_DO_FUN(do_tail);
DECLARE_DO_FUN(do_access);
DECLARE_DO_FUN(do_spellbane);

DECLARE_DO_FUN(do_nerve);
DECLARE_DO_FUN(do_palm);
DECLARE_DO_FUN(do_endure);
DECLARE_DO_FUN(do_throw);

DECLARE_DO_FUN(do_warcry);
DECLARE_DO_FUN(do_poison_dust);
DECLARE_DO_FUN(do_blindness_dust);
DECLARE_DO_FUN(do_strangle);


DECLARE_DO_FUN(do_ram);
DECLARE_DO_FUN(do_ram2);
DECLARE_DO_FUN(do_bite);
DECLARE_DO_FUN(do_enlist);

DECLARE_DO_FUN(do_lagout);
DECLARE_DO_FUN(do_myell);
DECLARE_DO_FUN(do_llimit);
DECLARE_DO_FUN(do_global);
DECLARE_DO_FUN(do_tame);
DECLARE_DO_FUN(do_find_water);
DECLARE_DO_FUN(do_track);
DECLARE_DO_FUN(do_shield_cleave);
DECLARE_DO_FUN(do_cloak);
DECLARE_DO_FUN(do_demand);

DECLARE_DO_FUN(do_awareness);
DECLARE_DO_FUN(do_forest_blending);
DECLARE_DO_FUN(do_breath_fire);
DECLARE_DO_FUN(do_gaseous_form);
DECLARE_DO_FUN(do_rear_kick);

DECLARE_DO_FUN(do_shapeshift);
DECLARE_DO_FUN(do_door_bash);
DECLARE_DO_FUN(do_multistrike);

DECLARE_DO_FUN(do_honorbound);
DECLARE_DO_FUN(do_hometown);
DECLARE_DO_FUN(do_retired);

DECLARE_DO_FUN(do_forage);
DECLARE_DO_FUN(do_assassinate);
DECLARE_DO_FUN(do_defend);
DECLARE_DO_FUN(do_intimidate);
DECLARE_DO_FUN(do_lash);
DECLARE_DO_FUN(do_pugil);
DECLARE_DO_FUN(do_protection_heat_cold);
DECLARE_DO_FUN(do_rally);

DECLARE_DO_FUN(do_notransfer);
DECLARE_DO_FUN(do_iron_resolve);
DECLARE_DO_FUN(do_dragon_spirit);
DECLARE_DO_FUN(do_quiet_movement);

DECLARE_DO_FUN(do_evaluation);
DECLARE_DO_FUN(do_specialize);

DECLARE_DO_FUN(do_deathmessage);

DECLARE_DO_FUN(do_focus);
DECLARE_DO_FUN(do_max_limits);
DECLARE_DO_FUN(do_psionic_blast); /* illithids - runge */
DECLARE_DO_FUN(do_spike);
DECLARE_DO_FUN(do_renam); /* Safety check on do_rename */
DECLARE_DO_FUN(do_rename);

DECLARE_DO_FUN(do_elder);
DECLARE_DO_FUN(do_records);
DECLARE_DO_FUN(do_mount);
DECLARE_DO_FUN(do_dismount);
DECLARE_DO_FUN(do_charge);
DECLARE_DO_FUN(do_blitz);
DECLARE_DO_FUN(do_rebel);
DECLARE_DO_FUN(do_lurk);
DECLARE_DO_FUN(do_surround);
DECLARE_DO_FUN(do_target);
DECLARE_DO_FUN(do_chant);
DECLARE_DO_FUN(do_healtrance);
DECLARE_DO_FUN(do_fists);
DECLARE_DO_FUN(do_open_claw);
DECLARE_DO_FUN(do_harmony);
DECLARE_DO_FUN(do_iron_will);
DECLARE_DO_FUN(do_stunning_strike);
DECLARE_DO_FUN(do_iron_palm);
DECLARE_DO_FUN(do_chi_attack);
DECLARE_DO_FUN(do_dchant);
DECLARE_DO_FUN(do_spin_kick);
DECLARE_DO_FUN(do_give_belt);
DECLARE_DO_FUN(do_pounce);
DECLARE_DO_FUN(do_spiritwalk);
DECLARE_DO_FUN(do_spirit_move);
DECLARE_DO_FUN(do_knockdown);
DECLARE_DO_FUN(do_learn_adv);
DECLARE_DO_FUN(do_newbie_report);
DECLARE_DO_FUN(do_lastsite);
DECLARE_DO_FUN(do_log_spec);
DECLARE_DO_FUN(do_portal);
DECLARE_DO_FUN(do_conjure);
DECLARE_DO_FUN(do_telekinesis);
DECLARE_DO_FUN(do_wind_shear);
DECLARE_DO_FUN(do_sonic_belch);
DECLARE_DO_FUN(do_troll_fart);
DECLARE_DO_FUN(do_star_shower);
DECLARE_DO_FUN(do_davatar_assault);
DECLARE_DO_FUN(do_penetrating_thrust);
DECLARE_DO_FUN(do_wagon_strike);
DECLARE_DO_FUN(do_megaslash);

DECLARE_DO_FUN(do_allow_rang);
DECLARE_DO_FUN(do_allow_thief);
DECLARE_DO_FUN(do_allow_elem);
DECLARE_DO_FUN(do_bounty);

/* uncomment here and in interp.c and include dbase.c in the make for
the dbase function -werv
DECLARE_DO_FUN(do_create_obj_list);  */
DECLARE_DO_FUN(do_ally);
DECLARE_DO_FUN(do_voodoo); /* for the voodoo doll */
DECLARE_DO_FUN(do_new_mem);

DECLARE_DO_FUN(do_chi_healing);
DECLARE_DO_FUN(do_dim_mak);

DECLARE_DO_FUN(do_nomagic);
DECLARE_DO_FUN(do_instruct);
DECLARE_DO_FUN(do_shieldbash);
DECLARE_DO_FUN(do_morale_report);
DECLARE_DO_FUN(do_psychdesc);
DECLARE_DO_FUN(do_info);
DECLARE_DO_FUN(do_promote);
DECLARE_DO_FUN(do_shackles);
DECLARE_DO_FUN(do_mentallink);
DECLARE_DO_FUN(do_know_time);
DECLARE_DO_FUN(do_grace);
DECLARE_DO_FUN(do_notick);
DECLARE_DO_FUN(do_alchemy);
DECLARE_DO_FUN(do_allowrp);
DECLARE_DO_FUN(do_sockwrite);
DECLARE_DO_FUN(do_badname);
DECLARE_DO_FUN(do_taunt);
DECLARE_DO_FUN(do_brandtalk);
DECLARE_DO_FUN(do_stealth);
DECLARE_DO_FUN(do_immrp);
DECLARE_DO_FUN(do_demon_swarm);
DECLARE_DO_FUN(do_ctell);
DECLARE_DO_FUN(do_conference);
DECLARE_DO_FUN(do_cecho);
DECLARE_DO_FUN(do_skullbash);
DECLARE_DO_FUN(do_wildfury);
DECLARE_DO_FUN(do_warpaint);
DECLARE_DO_FUN(do_battlescream);
DECLARE_DO_FUN(do_carving);
DECLARE_DO_FUN(do_killer_instinct);
DECLARE_DO_FUN(do_canopy_walk);
DECLARE_DO_FUN(do_trapmaking);
DECLARE_DO_FUN(do_trapset);
DECLARE_DO_FUN(do_beastialstance);
DECLARE_DO_FUN(do_hunters_knife);
DECLARE_DO_FUN(do_beast_command);
DECLARE_DO_FUN(do_disarmtrap);
DECLARE_DO_FUN(do_call_wild);
DECLARE_DO_FUN(do_apply);
DECLARE_DO_FUN(do_code);
DECLARE_DO_FUN(do_scheck);
DECLARE_DO_FUN(do_autoattack);
DECLARE_DO_FUN(do_getaway);
DECLARE_DO_FUN(do_mute);
DECLARE_DO_FUN(do_houtlaw);
DECLARE_DO_FUN(do_vote);
DECLARE_DO_FUN(do_view);
DECLARE_DO_FUN(do_bribery);
DECLARE_DO_FUN(do_scalepick);
DECLARE_DO_FUN(do_irvcheck);
DECLARE_DO_FUN(do_limpurge);
DECLARE_DO_FUN(do_material_check);
DECLARE_DO_FUN(do_tinker);
DECLARE_DO_FUN(do_forge_weapon);
DECLARE_DO_FUN(do_extract);
DECLARE_DO_FUN(do_pricelist);
DECLARE_DO_FUN(do_sharpen);
DECLARE_DO_FUN(do_sense_evil);
DECLARE_DO_FUN(do_armor_of_god);
DECLARE_DO_FUN(do_summon);
DECLARE_DO_FUN(do_block_retreat);
DECLARE_DO_FUN(do_ransack);
DECLARE_DO_FUN(do_lookout);
DECLARE_DO_FUN(do_bind);
DECLARE_DO_FUN(do_blindfold);
DECLARE_DO_FUN(do_gag);
DECLARE_DO_FUN(do_offhand_disarm);
DECLARE_DO_FUN(do_rapier);
DECLARE_DO_FUN(do_thrust);
DECLARE_DO_FUN(do_hire_crew);
DECLARE_DO_FUN(do_seenewbie);
DECLARE_DO_FUN(do_tanning);
DECLARE_DO_FUN(do_hoarder);
DECLARE_DO_FUN(do_burrow);

DECLARE_DO_FUN(do_jump);
DECLARE_DO_FUN(do_impale);

DECLARE_DO_FUN(do_novice);

DECLARE_DO_FUN(do_darkfocus);
DECLARE_DO_FUN(do_helper_robe);
DECLARE_DO_FUN(do_swing);

DECLARE_DO_FUN(do_worship);
DECLARE_DO_FUN(do_bless);
DECLARE_DO_FUN(do_punish);

DECLARE_DO_FUN(do_book_read);
DECLARE_DO_FUN(do_book_sign);
DECLARE_DO_FUN(do_last_logon);
DECLARE_DO_FUN(do_nofight);
DECLARE_DO_FUN(do_revoke);

DECLARE_DO_FUN(do_clannote);
DECLARE_DO_FUN(do_deathnote);
DECLARE_DO_FUN(do_banknote);
DECLARE_DO_FUN(do_crimnote);

DECLARE_DO_FUN(do_clanfor);
DECLARE_DO_FUN(do_clanform);
DECLARE_DO_FUN(do_clandissolv);
DECLARE_DO_FUN(do_clandissolve);
DECLARE_DO_FUN(do_claninduc);
DECLARE_DO_FUN(do_claninduct);
DECLARE_DO_FUN(do_clantransfe);
DECLARE_DO_FUN(do_clantransfer);
DECLARE_DO_FUN(do_clanqui);
DECLARE_DO_FUN(do_clanquit);
DECLARE_DO_FUN(do_clan_promote);
DECLARE_DO_FUN(do_clanappoint);
DECLARE_DO_FUN(do_clist);
DECLARE_DO_FUN(do_clantitle);
DECLARE_DO_FUN(do_cctell);
DECLARE_DO_FUN(do_dossier);
DECLARE_DO_FUN(do_roster);
DECLARE_DO_FUN(do_recruit);
DECLARE_DO_FUN(do_wecho);
DECLARE_DO_FUN(do_clecho);
DECLARE_DO_FUN(do_purify);
DECLARE_DO_FUN(do_resurrect);
DECLARE_DO_FUN(do_dump_alchemy);
DECLARE_DO_FUN(do_dump_alchem);
DECLARE_DO_FUN(do_wizireport);
DECLARE_DO_FUN(do_clan_public);
DECLARE_DO_FUN(do_email);
DECLARE_DO_FUN(do_scribe);
DECLARE_DO_FUN(do_charm);
DECLARE_DO_FUN(do_uncharm);
DECLARE_DO_FUN(do_nosee_perm);
DECLARE_DO_FUN(do_music);
DECLARE_DO_FUN(do_music_dl);
DECLARE_DO_FUN(do_book_contents);
DECLARE_DO_FUN(do_pen_fill);
DECLARE_DO_FUN(do_psistorm);
DECLARE_DO_FUN(do_jdancestaves);
DECLARE_DO_FUN(do_active_brand);
DECLARE_DO_FUN(do_clanalias);
DECLARE_DO_FUN(do_acidfire);
DECLARE_DO_FUN(do_surname);
DECLARE_DO_FUN(do_relname);
DECLARE_DO_FUN(do_suraccept);
DECLARE_DO_FUN(do_resurrec);
DECLARE_DO_FUN(do_devour);
DECLARE_DO_FUN(do_carebearstare);
DECLARE_DO_FUN(do_krazyeyes);
DECLARE_DO_FUN(do_clan_class);
DECLARE_DO_FUN(do_clan_race);
DECLARE_DO_FUN(do_moniker);
DECLARE_DO_FUN(do_lawless);
DECLARE_DO_FUN(do_trace);
DECLARE_DO_FUN(do_muster);
DECLARE_DO_FUN(do_pre_title);
DECLARE_DO_FUN(do_hemote);
DECLARE_DO_FUN(do_stalking);
DECLARE_DO_FUN(do_field_surgery);
DECLARE_DO_FUN(do_lottery);
DECLARE_DO_FUN(do_deathfire_missiles);
DECLARE_DO_FUN(do_abyssal_claws);
DECLARE_DO_FUN(do_destruction_shockwave);
DECLARE_DO_FUN(do_brandpower);
DECLARE_DO_FUN(do_nap);
DECLARE_DO_FUN(do_hyper);
DECLARE_DO_FUN(do_cunning_strike);
DECLARE_DO_FUN(do_thugs_stance);
DECLARE_DO_FUN(do_misinformation);
DECLARE_DO_FUN(do_silver_tongue);
DECLARE_DO_FUN(do_hgtalk);
DECLARE_DO_FUN(do_bribe);
DECLARE_DO_FUN(do_song_of_shadows);
DECLARE_DO_FUN(do_soothing_voice);
DECLARE_DO_FUN(do_belt_use);
DECLARE_DO_FUN(do_dance_of_the_rogue);
DECLARE_DO_FUN(do_takemagic);

