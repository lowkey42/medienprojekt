/**************************************************************************\
 * Animation -														      *
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

#include <unordered_map>

#include "../renderer/texture.hpp"
#include "../../core/asset/asset_manager.hpp"

#include "../units.hpp"

#include <glm/glm.hpp>

namespace mo {
namespace renderer {

	enum class Animation_type{
		idle,
		walking,
		attacking_melee,
		attacking_range,
		interacting,
		taking,
		change_weapon,
		damaged,
		healed,
		died,
		dying,
		resurrected
	};

}
}

namespace std {
	template <> struct hash<mo::renderer::Animation_type> {
		size_t operator()(mo::renderer::Animation_type ac)const noexcept {
			return static_cast<size_t>(ac);
		}
	};
}

namespace mo {
namespace renderer {

	struct Animation_data;

	struct Animation_frame_data;

	struct Animation{

		Animation(std::unique_ptr<Animation_data> data);
		~Animation();

		Animation& operator=(Animation&& rhs)noexcept;

		// Attributes
		std::unique_ptr<Animation_data> _data;

		// Methods
		void modulation(Animation_type type, float mod) const noexcept;

		auto animation_exists(const Animation_type type) const noexcept -> bool;
		auto remaining_time(const Animation_type type, const float cur_frame) const noexcept -> Time;
		auto frame_width() const noexcept -> int;
		auto frame_height() const noexcept -> int;
		auto texture() const noexcept -> Texture_ptr;
		auto uv(const int frame, const Animation_type type = Animation_type::idle) const noexcept -> glm::vec4;
		auto next_frame(const Animation_type type, const float cur_frame, const float deltaTime, const bool repeat) const noexcept -> float ;

	};

}

namespace asset {
	template<>
	struct Loader<renderer::Animation> {
		using RT = std::shared_ptr<renderer::Animation>;

		static RT load(istream in);

		static void store(ostream out, const renderer::Animation& asset);
	};
}
}

