/**************************************************************************\
 * The component that goes BOOM! (timed) area effects on contact/damage   *
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

#include "../../../level/elements.hpp"
#include "../../../effects.hpp"

#include "damage_effect_comp.hpp"

namespace mo {
namespace sys {
namespace combat {

	class Explosive_comp : public ecs::Component<Explosive_comp> {
		public:
			static constexpr const char* name() {return "Explosive";}
			void load(sf2::JsonDeserializer& state,
			          asset::Asset_manager& asset_mgr)override;
			void save(sf2::JsonSerializer& state)const override;

			Explosive_comp(ecs::Entity& owner, float damage=10, Distance range=Distance(2),
						   Time delay=Time(0), bool on_contact=false, bool on_damage=false) noexcept
				: Component(owner), _damage(damage), _range(range), _delay(delay),
				  _activate_on_contact(on_contact), _activate_on_damage(on_damage) {}
			Explosive_comp(Explosive_comp&&)noexcept = default;
			~Explosive_comp()noexcept = default;
			Explosive_comp& operator=(Explosive_comp&&)noexcept = default;


		private:
			friend class Combat_system;
			friend class Reaper_subsystem;

			float _damage;
			Distance _range;
			Time _delay;
			bool _activate_on_contact;
			bool _activate_on_damage;

			Time _delay_left = Time(0);
			bool _exloded = false;

			Force _blast_force = Force(0);

			level::Element _damage_type   = level::Element::neutral;
			Effect_type _explosion_effect = Effect_type::none;
			Damage_effect _damage_effect  = Damage_effect::none;

			float _force_feedback = 0;
	};

}
}
}
