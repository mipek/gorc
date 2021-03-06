#include "puppet_animation_aspect.hpp"
#include "game/world/level_model.hpp"
#include "game/world/keys/components/key_state.hpp"
#include "game/world/keys/key_presenter.hpp"
#include "game/world/events/armed_mode_changed.hpp"
#include "game/world/events/jumped.hpp"
#include "game/world/events/landed.hpp"
#include "game/world/events/killed.hpp"
#include "game/world/events/thing_created.hpp"

using gorc::game::world::aspects::puppet_animation_aspect;

puppet_animation_aspect::puppet_animation_aspect(entity_component_system<thing_id> &cs,
                                                 level_presenter &presenter)
    : inner_join_aspect(cs)
    , presenter(presenter) {

    created_delegate =
        cs.bus.add_handler<events::thing_created>([&](events::thing_created const &e) {
        maybe_if(e.tpl.pup, [&](auto pup) {
            // New thing has a puppet. Create a puppet animations component.
            cs.emplace_component<components::puppet_animations>(e.thing, pup);
        });
    });

    armed_mode_delegate =
        cs.bus.add_handler<events::armed_mode_changed>([&](events::armed_mode_changed const &e) {
        for(auto &pup : cs.find_component<components::puppet_animations>(e.thing)) {
            bool is_underwater = false;
            for(auto &thing : cs.find_component<components::thing>(e.thing)) {
                auto const &cur_sector = at_id(presenter.model->sectors, thing.second->sector);
                is_underwater = cur_sector.flags & flags::sector_flag::Underwater;
            }

            flags::puppet_mode_type maj_mode = flags::puppet_mode_type::unarmed;
            switch(e.mode) {
            case flags::armed_mode::unarmed:
                maj_mode = is_underwater ? flags::puppet_mode_type::swimming_unarmed
                                         : flags::puppet_mode_type::unarmed;
                break;

            case flags::armed_mode::armed:
                maj_mode = is_underwater ? flags::puppet_mode_type::swimming_armed
                                         : flags::puppet_mode_type::armed;
                break;

            case flags::armed_mode::saber:
                maj_mode = is_underwater ? flags::puppet_mode_type::swimming_saber
                                         : flags::puppet_mode_type::saber;
                break;
            }

            pup.second->puppet_mode_type = maj_mode;
        }
    });

    jumped_delegate =
        cs.bus.add_handler<events::jumped>([&](events::jumped const &e) {
        for(auto &pup : cs.find_component<components::puppet_animations>(e.thing)) {
            for(auto &thing : cs.find_component<components::thing>(e.thing)) {
                auto jump_submode = flags::puppet_submode_type::Rising;
                if(thing.second->physics_flags & flags::physics_flag::is_crouching) {
                    jump_submode = flags::puppet_submode_type::Leap;
                }

                set_walk_animation(*thing.second, *pup.second, jump_submode, 1.0f);
            }
        }
    });

    landed_delegate =
        cs.bus.add_handler<events::landed>([&](events::landed const &e) {
        auto rng = cs.find_component<components::puppet_animations>(e.thing);
        for(auto it = rng.begin(); it != rng.end(); ++it) {
            presenter.key_presenter->play_mode(thing_id(e.thing), flags::puppet_submode_type::Land);
        }
    });

    killed_delegate =
        cs.bus.add_handler<events::killed>([&](events::killed const &e) {
        for(auto &pup : cs.find_component<components::puppet_animations>(e.thing)) {
            // HACK: Stop idle animation
            maybe_if(pup.second->actor_walk_animation, [&](thing_id anim) {
                presenter.key_presenter->stop_key(e.thing, anim, 0.0f);
                pup.second->actor_walk_animation = nothing;
            });

            presenter.key_presenter->play_mode(e.thing, flags::puppet_submode_type::Death);
        }
    });
    return;
}

void puppet_animation_aspect::set_walk_animation(components::thing &thing,
                                                 components::puppet_animations &pup,
                                                 flags::puppet_submode_type type,
                                                 float speed) {
    if(thing.type == flags::thing_type::Corpse || !pup.actor_walk_animation.has_value()) {
        // Thing is dead and/or cannot play a walk animation.
        return;
    }

    keys::key_state *keyState = nullptr;
    maybe_if(pup.actor_walk_animation, [&](thing_id anim) {
            for(auto &key_state : ecs.find_component<keys::key_state>(anim)) {
                keyState = key_state.second;
            }
        });

    if(!keyState) {
        return;
    }

    auto const &maybe_submode = pup.puppet->get_mode(pup.puppet_mode_type).get_submode(type);
    maybe_if(maybe_submode, [&](content::assets::puppet_submode const *submode) {
        if(keyState->animation != submode->anim) {
            keyState->animation_time = 0.0;
        }

        keyState->animation = submode->anim;
        keyState->high_priority = submode->hi_priority;
        keyState->low_priority = submode->lo_priority;
        keyState->flags = flag_set<flags::key_flag>();
        keyState->speed = speed;
    });
}

void puppet_animation_aspect::set_walk_animation_speed(components::puppet_animations &pup,
                                                       float speed) {
    maybe_if(pup.actor_walk_animation, [&](thing_id anim) {
        for(auto &key_state : ecs.find_component<keys::key_state>(anim)) {
            key_state.second->speed = speed;
        }
    });
}

bool puppet_animation_aspect::is_walk_animation_mode(components::puppet_animations &pup,
                                                     flags::puppet_submode_type type) {
    maybe_if(pup.actor_walk_animation, [&](thing_id anim) {
        for(auto &key_state : ecs.find_component<keys::key_state>(anim)) {
            auto const &maybe_submode = pup.puppet->get_mode(pup.puppet_mode_type).get_submode(type);
            maybe_if(maybe_submode, [&](content::assets::puppet_submode const *submode) {
                return key_state.second->animation == submode->anim;
            });
        }
    });

    return false;
}

void puppet_animation_aspect::update_standing_animation(components::thing &thing,
                                                        components::puppet_animations &pup) {
    auto oriented_vel = invert(thing.orient).transform(thing.vel);
    auto run_length = length(thing.vel);

    auto vel_fb = get<1>(oriented_vel);
    auto vel_lr = get<0>(oriented_vel);
    auto turn_rate = get<1>(thing.ang_vel);

    float run_anim_speed = run_length * 20.0f;
    float turn_anim_speed = static_cast<float>(fabs(turn_rate) / 360.0);

    if(thing.physics_flags & flags::physics_flag::is_crouching) {
        if(fabs(turn_rate) > 0.0001) {
            set_walk_animation(thing, pup, flags::puppet_submode_type::CrouchForward, turn_anim_speed * 20.0f);
        }
        else if(vel_fb >= 0.0f || fabs(vel_lr) > fabs(vel_fb)) {
            set_walk_animation(thing, pup, flags::puppet_submode_type::CrouchForward, run_anim_speed);
        }
        else {
            set_walk_animation(thing, pup, flags::puppet_submode_type::CrouchBack, run_anim_speed);
        }
    }
    else {
        if(run_length < 0.001f) {
            // Idle or turning.
            if(turn_rate > 60.0f) {
                // Turning right
                set_walk_animation(thing, pup, flags::puppet_submode_type::TurnRight, turn_anim_speed);
            }
            else if(turn_rate < -60.0f) {
                // Turning left
                set_walk_animation(thing, pup, flags::puppet_submode_type::TurnLeft, turn_anim_speed);
            }
            else if(fabs(turn_rate) < 0.001) {
                set_walk_animation(thing, pup, flags::puppet_submode_type::Stand, 1.0f);
            }
            else if(!is_walk_animation_mode(pup, flags::puppet_submode_type::Stand)) {
                set_walk_animation_speed(pup, turn_anim_speed);
            }
        }
        else if(fabs(vel_lr) > fabs(vel_fb)) {
            // Strafing left or right
            if(vel_lr >= 0.0f) {
                set_walk_animation(thing, pup, flags::puppet_submode_type::StrafeRight, run_anim_speed);
            }
            else {
                set_walk_animation(thing, pup, flags::puppet_submode_type::StrafeLeft, run_anim_speed);
            }
        }
        else if(vel_fb > 0.5f) {
            // Running forward
            set_walk_animation(thing, pup, flags::puppet_submode_type::Run, run_anim_speed);
        }
        else if(vel_fb > 0.0f) {
            set_walk_animation(thing, pup, flags::puppet_submode_type::Walk, run_anim_speed);
        }
        else {
            // Walking backwards
            set_walk_animation(thing, pup, flags::puppet_submode_type::WalkBack, run_anim_speed);
        }
    }
}

void puppet_animation_aspect::update(time_delta,
                                     thing_id tid,
                                     components::puppet_animations &pup,
                                     components::thing &thing) {
    // Updates the idle animation loop

    // There are many possible states. This aspect tries to determine how the
    // entity is moving based on its thing properties.

    // Create the walk animation, if one does not already exist
    if(!pup.actor_walk_animation.has_value()) {
        pup.actor_walk_animation =
            presenter.key_presenter->play_mode(tid, flags::puppet_submode_type::Stand);
        maybe_if(pup.actor_walk_animation, [&](auto walk_anim) {
            for(auto &key_state : ecs.find_component<keys::key_state>(walk_anim)) {
                key_state.second->flags = flag_set<flags::key_flag>();
            }
        });
    }

    if(static_cast<int>(thing.attach_flags)) {
        // Thing is standing on a hard surface
        update_standing_animation(thing, pup);
    }
    else {
        // Thing is in freefall.
        // If the player has just jumped, the jump animation is already playing.
        // Check to see if the player has reached apogee:
        if(dot(thing.vel, make_vector(0.0f, 0.0f, -1.0f)) > 0.0f) {
            // Thing's trajectory has reached apogee.
            set_walk_animation(thing, pup, flags::puppet_submode_type::Drop, 1.0f);
        }
    }

    return;
}
