/**************************************************************************\
 * The screen in which the core-gameplay take place                       *
 *                                               ___                      *
 *    /\/\   __ _  __ _ _ __  _   _ _ __ ___     /___\_ __  _   _ ___     *
 *   /    \ / _` |/ _` | '_ \| | | | '_ ` _ \   //  // '_ \| | | / __|    *
 *  / /\/\ \ (_| | (_| | | | | |_| | | | | | | / \_//| |_) | |_| \__ \    *
 *  \/    \/\__,_|\__, |_| |_|\__,_|_| |_| |_| \___/ | .__/ \__,_|___/    *
 *                |___/                              |_|                  *
 *                                                                        *
 * Copyright (c) 2014 Florian Oetke                                       *
 *                                                                        *
 *  This file is part of MagnumOpus and distributed under the MIT License *
 *  See LICENSE file for details.                                         *
\**************************************************************************/

#pragma once

#include "game_engine.hpp"

#include "effects.hpp"

#include "../core/units.hpp"
#include "../core/ecs/ecs.hpp"
#include "../core/ecs/serializer.hpp"
#include <core/renderer/particles.hpp>

#include "level/level.hpp"
#include "level/tilemap.hpp"

#include "sys/physics/transform_system.hpp"
#include "sys/physics/physics_system.hpp"
#include "sys/graphic/graphic_system.hpp"
#include "sys/sound/sound_system.hpp"
#include "sys/controller/controller_system.hpp"
#include "sys/cam/camera_system.hpp"
#include "sys/ai/ai_system.hpp"
#include "sys/combat/combat_system.hpp"
#include "sys/item/item_system.hpp"
#include "sys/item/element_system.hpp"
#include "sys/state/state_system.hpp"
#include "sys/ui/ui_system.hpp"

namespace mo {
	namespace renderer{ class Camera; }

	struct Profile_data {
		Profile_data(std::string n, uint64_t s, int di, int de){
			name = n;
			seed = s;
			difficulty = di;
			depth = de;
		}

		Profile_data(){}

		std::string name;
		uint64_t seed = 0;
		int difficulty = 0;
		int depth = 0;

	};

	struct Saveable_state;
	using Saveable_state_ptr = asset::Ptr<Saveable_state>;

	struct Game_state {
		Game_engine& engine;
		Profile_data profile;

		level::Level level;
		ecs::Entity_manager em;
		level::Tilemap tilemap;

		sys::physics::Transform_system transform;

		renderer::Particle_renderer particle_renderer;

		Effect_source effect_bus;
		FFeedback_source forcefeedback_bus;
		FFeedback_slot forcefeedback_handler;

		sys::cam::Camera_system camera;
		sys::physics::Physics_system physics;
		sys::state::State_system state;
		sys::controller::Controller_system controller;
		sys::ai::Ai_system ai;
		sys::graphic::Graphic_system graphics;
		sys::combat::Combat_system combat;
		sys::item::Item_system items;
		sys::item::Element_system elements;
		sys::sound::Sound_system soundsys;
		sys::ui::Ui_system ui;

		ecs::Entity_ptr main_player;
		std::vector<ecs::Entity_ptr> sec_players;


		void update(Time dt);
		auto draw(Time dt) -> util::cvector_range<sys::cam::VScreen>;
		void draw_ui();

		auto add_player(sys::controller::Controller& controller, Position pos,
		                ecs::Entity_ptr e=ecs::Entity_ptr()) -> ecs::Entity_ptr;
		void remove_player(sys::controller::Controller& controller);

		static void delete_save();
		void save();
		auto save_to() -> Saveable_state;

		void forcefeedback(Position, float);

		float saturation()const;

		static auto create(Game_engine& engine,
		                   std::string name) -> std::unique_ptr<Game_state>;
		static auto create(Game_engine& engine,
		                   Profile_data profile,
		                   std::vector<ecs::ETO> players,
		                   int depth) -> std::unique_ptr<Game_state>;
		static auto create_from_save(Game_engine& engine,
		                             const Saveable_state&) -> std::unique_ptr<Game_state>;
		static auto load(Game_engine& engine) -> std::unique_ptr<Game_state>;
		static bool save_exists(Game_engine& engine);

		private:
			Game_state(Game_engine& engine, Profile_data profile);

			float _screen_saturation=1.f;
	};

	struct Saveable_state {
		Saveable_state(ecs::Entity_manager& em, Profile_data profile)
		    : em(em), profile(profile), my_stream(util::nothing()) {}
		Saveable_state(asset::istream stream)
		    : em(util::nothing()), profile(util::nothing()), my_stream(std::move(stream)) {}
		Saveable_state(Saveable_state&&)noexcept = default;

		auto operator=(Saveable_state&&) -> Saveable_state& = default;

		util::maybe<ecs::Entity_manager&> em;
		util::maybe<Profile_data> profile;
		util::maybe<asset::istream> my_stream;
	};

	namespace asset {
		template<>
		struct Loader<Saveable_state> {
			using RT = std::shared_ptr<Saveable_state>;

			static RT load(istream in);

			static void store(ostream out, const Saveable_state& asset);
		};
	}
}
