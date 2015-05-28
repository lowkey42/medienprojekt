/**************************************************************************\
 * marks entities with ingame ui                                          *
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

#include <core/ecs/ecs.hpp>
#include <core/units.hpp>

namespace mo {
namespace sys {
namespace ui {

	class Ui_comp : public ecs::Component<Ui_comp> {
		public:
			static constexpr const char* name() {return "ui";}
			//void load(ecs::Entity_state&)override;
			//void store(ecs::Entity_state&)override;

			Ui_comp(ecs::Entity& owner) noexcept
				: Component(owner) {
			}

			// struct Persisted_state;
			// friend struct Persisted_state;
		private:
	};

}
}
}
