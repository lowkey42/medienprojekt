/**************************************************************************\
 *												                          *
 *                                               ___                      *
 *    /\/\   __ _  __ _ _ __  _   _ _ __ ___     /___\_ __  _   _ ___     *
 *   /    \ / _` |/ _` | '_ \| | | | '_ ` _ \   //  // '_ \| | | / __|    *
 *  / /\/\ \ (_| | (_| | | | | |_| | | | | | | / \_//| |_) | |_| \__ \    *
 *  \/    \/\__,_|\__, |_| |_|\__,_|_| |_| |_| \___/ | .__/ \__,_|___/    *
 *                |___/                              |_|                  *
 *                                                                        *
 * Copyright (c) 2015 Florian Oetke & Sebastian Schalow                   *
 *                                                                        *
 *  This file is part of MagnumOpus and distributed under the MIT License *
 *  See LICENSE file for details.                                         *
\**************************************************************************/

#pragma once

#include <core/ecs/ecs.hpp>
#include <core/renderer/texture.hpp>
#include <core/renderer/sprite_batch.hpp>
#include <core/renderer/animation.hpp>

namespace mo {
namespace sys {
namespace sprite {

	class Sprite_comp : public ecs::Component<Sprite_comp> {

	public:

		static constexpr const char* name() {return "Sprite";}
		void load(ecs::Entity_state&)override;
		void store(ecs::Entity_state&)override;

		// TODO: nullptr check
		Sprite_comp(ecs::Entity& owner, asset::Ptr<renderer::Animation> animation = asset::Ptr<renderer::Animation>()) :
			Component(owner), _animation(animation), _animType(renderer::Animation_type::idle){}

        auto sprite() const noexcept {
			return renderer::Sprite_batch::Sprite{{}, 0, _animation->uv(), _animation};
		}

		struct Persisted_state;
		friend struct Persisted_state;

	private:

		asset::Ptr<renderer::Animation> _animation;
		renderer::Animation_type _animType;

	};

}
}
}
