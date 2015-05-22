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

#include <core/ecs/serializer.hpp>
#include "sys/state/state_system.hpp"

#include "../core/units.hpp"

#include "game_ui.hpp"

namespace mo {
	namespace renderer{ class Camera; }

	struct Game_state;

	class Game_screen : public Screen {
		public:
			Game_screen(Game_engine& engine, std::string profile="default",
			            std::vector<ecs::ETO> players=
			                 std::vector<ecs::ETO>(),
			            util::maybe<int> depth=util::nothing());
			~Game_screen()noexcept;

		protected:
			void _update(float delta_time)override;
			void _draw(float delta_time)override;

			auto _prev_screen_policy()const noexcept -> Prev_screen_policy override {
				return Prev_screen_policy::discard;
			}

			void _on_state_change(ecs::Entity&, sys::state::State_data&);

			void _on_enter(util::maybe<Screen&> prev) override;
			void _on_leave(util::maybe<Screen&> next) override;

			void _join(sys::controller::Controller_added_event e);
			void _unjoin(sys::controller::Controller_removed_event e);

			void _save()const;

		private:
			Game_engine& _engine;

			std::unique_ptr<Game_state> _state;
			Game_ui _ui;

			util::slot<ecs::Entity&, sys::state::State_data&> _player_sc_slot;
	};

}
