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

static const char magic_h_rcsid[] = "$Id: magic.h,v 1.20 2003/11/28 19:57:01 fizzfaldt Exp $";

bool check_dispel( int dis_level, CHAR_DATA *victim, int sn);

int    spellcraft_dam    args( ( int num, int dice) );
bool    check_spellcraft args( (CHAR_DATA *ch, int sn) );

/* check to see if is a house ability - Wervdon */
bool    is_house_spell  args( ( CHAR_DATA *ch, int sn) );

/*
 * Spell functions.
 * Defined in magic.c.
 */
DECLARE_SPELL_FUN(spell_null);
DECLARE_SPELL_FUN(spell_absorb);
DECLARE_SPELL_FUN(spell_acid_blast);
DECLARE_SPELL_FUN(spell_armor);
DECLARE_SPELL_FUN(spell_bless);
DECLARE_SPELL_FUN(spell_bless_of_dark);
DECLARE_SPELL_FUN(spell_blindness);
DECLARE_SPELL_FUN(spell_burning_hands);
DECLARE_SPELL_FUN(spell_call_lightning);
DECLARE_SPELL_FUN(spell_calm);
DECLARE_SPELL_FUN(spell_cancellation);
DECLARE_SPELL_FUN(spell_cause_critical);
DECLARE_SPELL_FUN(spell_cause_light);
DECLARE_SPELL_FUN(spell_cause_serious);
DECLARE_SPELL_FUN(spell_change_sex);
DECLARE_SPELL_FUN(spell_chain_lightning);
DECLARE_SPELL_FUN(spell_charm_person);
DECLARE_SPELL_FUN(spell_chill_touch);
DECLARE_SPELL_FUN(spell_colour_spray);
DECLARE_SPELL_FUN(spell_concatenate);
DECLARE_SPELL_FUN(spell_wrath);
DECLARE_SPELL_FUN(spell_continual_light);
DECLARE_SPELL_FUN(spell_control_weather);
DECLARE_SPELL_FUN(spell_create_food);
DECLARE_SPELL_FUN(spell_create_rose);
DECLARE_SPELL_FUN(spell_create_red_flower);
DECLARE_SPELL_FUN(spell_create_blue_flower);
DECLARE_SPELL_FUN(spell_create_spring);
DECLARE_SPELL_FUN(spell_create_water);
DECLARE_SPELL_FUN(spell_cure_blindness);
DECLARE_SPELL_FUN(spell_cure_critical);
DECLARE_SPELL_FUN(spell_cure_disease);
DECLARE_SPELL_FUN(spell_cure_light);
DECLARE_SPELL_FUN(spell_cure_poison);
DECLARE_SPELL_FUN(spell_cure_serious);
DECLARE_SPELL_FUN(spell_curse);
DECLARE_SPELL_FUN(spell_dark_wrath);
DECLARE_SPELL_FUN(spell_defilement);
DECLARE_SPELL_FUN(spell_demonfire);
DECLARE_SPELL_FUN(spell_detect_evil);
DECLARE_SPELL_FUN(spell_detect_good);
DECLARE_SPELL_FUN(spell_detect_hidden);
DECLARE_SPELL_FUN(spell_detect_invis);
DECLARE_SPELL_FUN(spell_detect_magic);
DECLARE_SPELL_FUN(spell_detect_poison);
DECLARE_SPELL_FUN(spell_dispel_evil);
DECLARE_SPELL_FUN(spell_dispel_good);
DECLARE_SPELL_FUN(spell_dispel_magic);
DECLARE_SPELL_FUN(spell_earthquake);
DECLARE_SPELL_FUN(spell_enchant_armor);
DECLARE_SPELL_FUN(spell_enchant_weapon);
DECLARE_SPELL_FUN(spell_energy_drain);
DECLARE_SPELL_FUN(spell_evil_eye);
DECLARE_SPELL_FUN(spell_faerie_fire);
DECLARE_SPELL_FUN(spell_faerie_fog);
DECLARE_SPELL_FUN(spell_farsight);
DECLARE_SPELL_FUN(spell_fireball);
DECLARE_SPELL_FUN(spell_fireproof);
DECLARE_SPELL_FUN(spell_flamestrike);
DECLARE_SPELL_FUN(spell_flash);
DECLARE_SPELL_FUN(spell_fly);
DECLARE_SPELL_FUN(spell_frenzy);
DECLARE_SPELL_FUN(spell_gate);
DECLARE_SPELL_FUN(spell_portal);
DECLARE_SPELL_FUN(spell_getaway);
DECLARE_SPELL_FUN(spell_giant_strength);
DECLARE_SPELL_FUN(spell_harm);
DECLARE_SPELL_FUN(spell_haste);
DECLARE_SPELL_FUN(spell_distort_time);
DECLARE_SPELL_FUN(spell_heal);
DECLARE_SPELL_FUN(spell_heat_metal);
DECLARE_SPELL_FUN(spell_holy_word);
DECLARE_SPELL_FUN(spell_ice_pox); /* nasty NW spell */
DECLARE_SPELL_FUN(spell_identify);
DECLARE_SPELL_FUN(spell_infravision);
DECLARE_SPELL_FUN(spell_insight);
DECLARE_SPELL_FUN(spell_invis);
DECLARE_SPELL_FUN(spell_know_alignment);
DECLARE_SPELL_FUN(spell_channel);
DECLARE_SPELL_FUN(spell_lightning_bolt);
DECLARE_SPELL_FUN(spell_locate_object);
DECLARE_SPELL_FUN(spell_magic_missile);
DECLARE_SPELL_FUN(spell_mass_healing);
DECLARE_SPELL_FUN(spell_mass_invis);
DECLARE_SPELL_FUN(spell_need);
DECLARE_SPELL_FUN(spell_pass_door);
DECLARE_SPELL_FUN(spell_plague);
DECLARE_SPELL_FUN(spell_poison);
DECLARE_SPELL_FUN(spell_power_word_kill);
DECLARE_SPELL_FUN(spell_power_word_stun);
DECLARE_SPELL_FUN(spell_protection_evil);
DECLARE_SPELL_FUN(spell_protection_good);
DECLARE_SPELL_FUN(spell_ray_of_truth);
DECLARE_SPELL_FUN(spell_recharge);
DECLARE_SPELL_FUN(spell_refresh);
DECLARE_SPELL_FUN(spell_remove_curse);
DECLARE_SPELL_FUN(spell_sanctuary);
DECLARE_SPELL_FUN(spell_chromatic_shield);
DECLARE_SPELL_FUN(spell_shocking_grasp);
DECLARE_SPELL_FUN(spell_shield);
DECLARE_SPELL_FUN(spell_freedom_cry);
DECLARE_SPELL_FUN(spell_shroud_pro);
DECLARE_SPELL_FUN(spell_sleep);
DECLARE_SPELL_FUN(spell_slow);
DECLARE_SPELL_FUN(spell_stone_skin);
DECLARE_SPELL_FUN(spell_summon);
DECLARE_SPELL_FUN(spell_teleport);
/*DECLARE_SPELL_FUN(spell_tinker);*/
DECLARE_SPELL_FUN(spell_turn_undead);
DECLARE_SPELL_FUN(spell_ventriloquate);
DECLARE_SPELL_FUN(spell_weaken);
DECLARE_SPELL_FUN(spell_web);
DECLARE_SPELL_FUN(spell_word_of_recall);
DECLARE_SPELL_FUN(spell_power_word_recall);
DECLARE_SPELL_FUN(spell_acid_breath);
DECLARE_SPELL_FUN(spell_fire_breath);
DECLARE_SPELL_FUN(spell_frost_breath);
DECLARE_SPELL_FUN(spell_gas_breath);
DECLARE_SPELL_FUN(spell_lightning_breath);
DECLARE_SPELL_FUN(spell_general_purpose);
DECLARE_SPELL_FUN(spell_high_explosive);

DECLARE_SPELL_FUN(spell_animate_dead);
DECLARE_SPELL_FUN(spell_venueport);

/* To be added to magic.c */

DECLARE_SPELL_FUN(spell_summon_fire_elemental);
DECLARE_SPELL_FUN(spell_summon_air_elemental);
DECLARE_SPELL_FUN(spell_summon_void_elemental);
DECLARE_SPELL_FUN(spell_summon_earth_elemental);
DECLARE_SPELL_FUN(spell_summon_water_elemental);
DECLARE_SPELL_FUN(spell_windwall);
DECLARE_SPELL_FUN(spell_tsunami);
DECLARE_SPELL_FUN(spell_earthmaw);
DECLARE_SPELL_FUN(spell_flesh_to_stone);

DECLARE_SPELL_FUN(spell_disenchant_armor);
DECLARE_SPELL_FUN(spell_disenchant_weapon);
DECLARE_SPELL_FUN(spell_drain);
DECLARE_SPELL_FUN(spell_wither);

DECLARE_SPELL_FUN(spell_noxious_gas);
DECLARE_SPELL_FUN(spell_cone_of_cold);
DECLARE_SPELL_FUN(spell_timestop);
DECLARE_SPELL_FUN(spell_icestorm);
DECLARE_SPELL_FUN(spell_raise_hell);
DECLARE_SPELL_FUN(spell_consecrate);
DECLARE_SPELL_FUN(spell_wither);
DECLARE_SPELL_FUN(spell_moonshine);
DECLARE_SPELL_FUN(spell_shadowstrike);
DECLARE_SPELL_FUN(spell_nightmare);
DECLARE_SPELL_FUN(spell_protective_shield);
DECLARE_SPELL_FUN(spell_inferno);
DECLARE_SPELL_FUN(spell_slip);
DECLARE_SPELL_FUN(spell_thunderclap);

DECLARE_SPELL_FUN(spell_spiritblade);

DECLARE_SPELL_FUN(spell_guard_call);
DECLARE_SPELL_FUN(spell_iceball);
DECLARE_SPELL_FUN(spell_retribution);
DECLARE_SPELL_FUN(spell_true_sight);
DECLARE_SPELL_FUN(spell_lesser_golem);

DECLARE_SPELL_FUN(spell_brew);
DECLARE_SPELL_FUN(spell_familiar);
DECLARE_SPELL_FUN(spell_firestream);

DECLARE_SPELL_FUN(spell_fireshield);
DECLARE_SPELL_FUN(spell_iceshield);
DECLARE_SPELL_FUN(spell_shadowself);
DECLARE_SPELL_FUN(spell_skull_cleaver);
DECLARE_SPELL_FUN(spell_ranger_staff);
DECLARE_SPELL_FUN(spell_bless_weapon);

DECLARE_SPELL_FUN(spell_chaos_mind);
DECLARE_SPELL_FUN(spell_revolt);
DECLARE_SPELL_FUN(spell_unholy_fire);
DECLARE_SPELL_FUN(spell_acid_spit);
DECLARE_SPELL_FUN(spell_team_spirit);
DECLARE_SPELL_FUN(spell_misdirection);
DECLARE_SPELL_FUN(spell_greater_golem);
DECLARE_SPELL_FUN(spell_safety);
DECLARE_SPELL_FUN(spell_protectorate_plate);
DECLARE_SPELL_FUN(spell_robe_of_the_light);
DECLARE_SPELL_FUN(spell_life_line);
DECLARE_SPELL_FUN(spell_decoy);
DECLARE_SPELL_FUN(spell_nightwalk);
DECLARE_SPELL_FUN(spell_holy_fire);
DECLARE_SPELL_FUN(spell_blade_barrier);
DECLARE_SPELL_FUN(spell_alarm);
DECLARE_SPELL_FUN(spell_badge);

DECLARE_SPELL_FUN(spell_nightwalk);
DECLARE_SPELL_FUN(spell_talk_to_dead);
DECLARE_SPELL_FUN(spell_dark_shield);
DECLARE_SPELL_FUN(spell_deathspell);
DECLARE_SPELL_FUN(spell_animate_object);
DECLARE_SPELL_FUN(spell_lifebane);
DECLARE_SPELL_FUN(spell_curse_weapon);
DECLARE_SPELL_FUN(spell_globe_of_darkness);
DECLARE_SPELL_FUN(spell_heavenly_sceptre_frenzy);
DECLARE_SPELL_FUN(spell_heavenly_sceptre_fire);

DECLARE_SPELL_FUN(spell_flame_scorch);
DECLARE_SPELL_FUN(spell_soulbind);

DECLARE_SPELL_FUN(spell_lightshield);
DECLARE_SPELL_FUN(spell_repent);
DECLARE_SPELL_FUN(spell_faith_healing);
DECLARE_SPELL_FUN(spell_warding);

DECLARE_SPELL_FUN(spell_frostbolt);
DECLARE_SPELL_FUN(spell_icelance);
DECLARE_SPELL_FUN(spell_earthfade);
DECLARE_SPELL_FUN(spell_forget);
DECLARE_SPELL_FUN(spell_earthbind);
DECLARE_SPELL_FUN(spell_cremate);
DECLARE_SPELL_FUN(spell_divine_touch);
DECLARE_SPELL_FUN(spell_transfer_object);
DECLARE_SPELL_FUN(spell_disintegrate);
DECLARE_SPELL_FUN(spell_grounding);
DECLARE_SPELL_FUN(spell_shock_sphere);


DECLARE_SPELL_FUN(spell_animate_skeleton);
DECLARE_SPELL_FUN(spell_mummify);
DECLARE_SPELL_FUN(spell_power_word_fear);
DECLARE_SPELL_FUN(spell_nightfall);
DECLARE_SPELL_FUN(spell_prevent_healing);
DECLARE_SPELL_FUN(spell_decay_corpse);
DECLARE_SPELL_FUN(spell_preserve);
DECLARE_SPELL_FUN(spell_regeneration);
DECLARE_SPELL_FUN(spell_restoration);
DECLARE_SPELL_FUN(spell_undead_drain);

DECLARE_SPELL_FUN(spell_fire_and_ice);
DECLARE_SPELL_FUN(spell_atrophy);
DECLARE_SPELL_FUN(spell_utter_heal);

DECLARE_SPELL_FUN(spell_call_slayer);
DECLARE_SPELL_FUN(spell_judgement);
DECLARE_SPELL_FUN(spell_bloodmist);
DECLARE_SPELL_FUN(spell_soulcraft);
DECLARE_SPELL_FUN(spell_taint);

/* -Werv Knight of Famine Starvation and Dehydation */

DECLARE_SPELL_FUN(spell_cause_starvation);
DECLARE_SPELL_FUN(spell_cause_dehydration);
DECLARE_SPELL_FUN(spell_tainted_food);
DECLARE_SPELL_FUN(spell_tainted_spring);
DECLARE_SPELL_FUN(spell_emaciation);
DECLARE_SPELL_FUN(spell_vermin);
DECLARE_SPELL_FUN(spell_spores);
DECLARE_SPELL_FUN(spell_bane);
DECLARE_SPELL_FUN(spell_epidemic);
DECLARE_SPELL_FUN(spell_rot);
/*DECLARE_SPELL_FUN(spell_summon_steed);*/
DECLARE_SPELL_FUN(spell_health);
DECLARE_SPELL_FUN(spell_forge);
DECLARE_SPELL_FUN(spell_meteor);
DECLARE_SPELL_FUN(spell_ameteor);
DECLARE_SPELL_FUN(spell_support);
DECLARE_SPELL_FUN(spell_devote);
DECLARE_SPELL_FUN(spell_capture);
DECLARE_SPELL_FUN(spell_darkforge);
/* Abyss spells - Runge */
DECLARE_SPELL_FUN(spell_demon_swarm);
DECLARE_SPELL_FUN(spell_pact);
/* Necro specialization spells - Runge */
DECLARE_SPELL_FUN(spell_gout_maggots);
DECLARE_SPELL_FUN(spell_ancient_plague);
DECLARE_SPELL_FUN(spell_lich);
DECLARE_SPELL_FUN(spell_strengthen_bone);
DECLARE_SPELL_FUN(spell_boiling_blood);
DECLARE_SPELL_FUN(spell_strengthen_construct);
DECLARE_SPELL_FUN(spell_bone_golem);
DECLARE_SPELL_FUN(spell_nether_shroud);
DECLARE_SPELL_FUN(spell_black_mantle);
DECLARE_SPELL_FUN(spell_divine_protect);
DECLARE_SPELL_FUN(spell_curse_room);
/* Outlaw */
DECLARE_SPELL_FUN(spell_propaganda);
DECLARE_SPELL_FUN(spell_lookout);
DECLARE_SPELL_FUN(spell_backup);
DECLARE_SPELL_FUN(spell_fence);
DECLARE_SPELL_FUN(spell_aura_defiance);
DECLARE_SPELL_FUN(spell_outlaw_cloak);
DECLARE_SPELL_FUN(spell_kidnap);

DECLARE_SPELL_FUN(spell_hellforge);
DECLARE_SPELL_FUN(spell_sunburst);
DECLARE_SPELL_FUN(spell_graft_flesh);
DECLARE_SPELL_FUN(spell_syphon_soul);
DECLARE_SPELL_FUN(spell_storm);
DECLARE_SPELL_FUN(spell_flood_room);
DECLARE_SPELL_FUN(spell_barrier);
DECLARE_SPELL_FUN(spell_ultra_heal);
DECLARE_SPELL_FUN(spell_spiritual_wrath);
DECLARE_SPELL_FUN(spell_cleanse);
DECLARE_SPELL_FUN(spell_health_bless);
DECLARE_SPELL_FUN(spell_benediction);
DECLARE_SPELL_FUN(spell_cloak_brave);
DECLARE_SPELL_FUN(spell_holyshield);
DECLARE_SPELL_FUN(spell_phasing);
DECLARE_SPELL_FUN(spell_exorcism);
DECLARE_SPELL_FUN(spell_hall_mirrors);
DECLARE_SPELL_FUN(spell_visions);
DECLARE_SPELL_FUN(spell_radiance);
DECLARE_SPELL_FUN(spell_bang);
DECLARE_SPELL_FUN(spell_fist_of_god);
DECLARE_SPELL_FUN(spell_mirror_image);
DECLARE_SPELL_FUN(spell_fear);
DECLARE_SPELL_FUN(spell_fog_conceilment);
DECLARE_SPELL_FUN(spell_strabismus);
DECLARE_SPELL_FUN(spell_illusionary_wall);
DECLARE_SPELL_FUN(spell_invis_mail);
DECLARE_SPELL_FUN(spell_lower_resist);
DECLARE_SPELL_FUN(spell_raise_resist);
DECLARE_SPELL_FUN(spell_enlarge);
DECLARE_SPELL_FUN(spell_shrink);
DECLARE_SPELL_FUN(spell_halo_of_eyes);
DECLARE_SPELL_FUN(spell_mass_protection);
DECLARE_SPELL_FUN(spell_super_charge);
DECLARE_SPELL_FUN(spell_mass_haste);
DECLARE_SPELL_FUN(spell_mass_slow);
DECLARE_SPELL_FUN(spell_freeze_person);
DECLARE_SPELL_FUN(spell_temporal_shield);
DECLARE_SPELL_FUN(spell_accelerate_time);
DECLARE_SPELL_FUN(spell_temporal_shear);
DECLARE_SPELL_FUN(spell_time_travel);
DECLARE_SPELL_FUN(spell_make_armor);
DECLARE_SPELL_FUN(spell_fade);
DECLARE_SPELL_FUN(spell_origin);
DECLARE_SPELL_FUN(spell_dissolve);
DECLARE_SPELL_FUN(spell_demonic_enchant);
DECLARE_SPELL_FUN(spell_waterbreathing);
DECLARE_SPELL_FUN(spell_nausea);
DECLARE_SPELL_FUN(spell_toad);
DECLARE_SPELL_FUN(spell_whiplash);
DECLARE_SPELL_FUN(spell_obscure);
DECLARE_SPELL_FUN(spell_lightblast);
DECLARE_SPELL_FUN(spell_lightbind);
DECLARE_SPELL_FUN(spell_hand_of_vengeance);
DECLARE_SPELL_FUN(spell_seal_of_justice);

DECLARE_SPELL_FUN(spell_napalm);
DECLARE_SPELL_FUN(spell_incinerate);
DECLARE_SPELL_FUN(spell_nova);
DECLARE_SPELL_FUN(spell_inferno);
DECLARE_SPELL_FUN(spell_imbue_flame);
DECLARE_SPELL_FUN(spell_ashes_to_ashes);
DECLARE_SPELL_FUN(spell_raging_fire);

DECLARE_SPELL_FUN(spell_charge_weapon);
DECLARE_SPELL_FUN(spell_tornado);
DECLARE_SPELL_FUN(spell_airshield);
DECLARE_SPELL_FUN(spell_suffocate);
DECLARE_SPELL_FUN(spell_jet_stream);
DECLARE_SPELL_FUN(spell_cyclone);
DECLARE_SPELL_FUN(spell_implosion);
DECLARE_SPELL_FUN(spell_thunder);
DECLARE_SPELL_FUN(spell_mass_fly);
DECLARE_SPELL_FUN(spell_dust_devil);
DECLARE_SPELL_FUN(spell_vortex);

DECLARE_SPELL_FUN(spell_stone);
DECLARE_SPELL_FUN(spell_stalagmite);
DECLARE_SPELL_FUN(spell_tremor);
DECLARE_SPELL_FUN(spell_avalanche);
DECLARE_SPELL_FUN(spell_cave_in);
DECLARE_SPELL_FUN(spell_crushing_hands);
DECLARE_SPELL_FUN(spell_shield_of_earth);
DECLARE_SPELL_FUN(spell_stoney_grasp);
DECLARE_SPELL_FUN(spell_meteor_storm);

DECLARE_SPELL_FUN(spell_geyser);
DECLARE_SPELL_FUN(spell_water_spout);
DECLARE_SPELL_FUN(spell_deluge);
DECLARE_SPELL_FUN(spell_whirlpool);
DECLARE_SPELL_FUN(spell_blizzard);
DECLARE_SPELL_FUN(spell_spring_rains);
DECLARE_SPELL_FUN(spell_submerge);
DECLARE_SPELL_FUN(spell_freeze);
DECLARE_SPELL_FUN(spell_frost_charge);
DECLARE_SPELL_FUN(spell_purify);
DECLARE_SPELL_FUN(spell_waterwalk);
DECLARE_SPELL_FUN(spell_ice_armor);
DECLARE_SPELL_FUN(spell_water_of_life);
DECLARE_SPELL_FUN(spell_fountain_of_youth);
DECLARE_SPELL_FUN(spell_rip_tide);
DECLARE_SPELL_FUN(spell_air_dagger);
DECLARE_SPELL_FUN(spell_gale_winds);

DECLARE_SPELL_FUN(spell_dark_armor);
DECLARE_SPELL_FUN(spell_aura_of_presence);
DECLARE_SPELL_FUN(spell_worm_damage);
DECLARE_SPELL_FUN(spell_elemental_gateway);
DECLARE_SPELL_FUN(spell_holy_beacon);
DECLARE_SPELL_FUN(spell_seize);
DECLARE_SPELL_FUN(spell_virulent_cysts);
DECLARE_SPELL_FUN(spell_esurience);
DECLARE_SPELL_FUN(spell_siren_screech);
DECLARE_SPELL_FUN(spell_phantasmal_force);
DECLARE_SPELL_FUN(spell_intangibility);
DECLARE_SPELL_FUN(spell_prismatic_sphere);
DECLARE_SPELL_FUN(spell_simulacrum);
DECLARE_SPELL_FUN(spell_rite_of_darkness);
DECLARE_SPELL_FUN(spell_call_of_duty);
