/**************************************************************************\
 * Draws the per-player ingame ui                                         *
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
#include <core/utils/events.hpp>

#include <core/renderer/graphics_ctx.hpp>
#include <core/renderer/shader.hpp>
#include <core/renderer/text.hpp>
#include <core/renderer/camera.hpp>
#include <core/renderer/primitives.hpp>

#include "ui_comp.hpp"

namespace mo {
namespace sys {
namespace ui {

	class Ui_system {
		public:
			Ui_system(Engine& e, ecs::Entity_manager& em);

			void update(Time dt);
			void draw();

		private:
			Ui_comp::Pool& _ui_comps;

			renderer::Camera _cam;
			renderer::Shader_program _hud_shader;
			renderer::Shader_program _health_shader;
			renderer::Object _hud;
			renderer::Texture_ptr _hud_bg_tex;
			renderer::Texture_ptr _hud_fg_tex;
			renderer::Texture_ptr _hud_health_tex;

			renderer::Shader_program _score_shader;
			renderer::Font_ptr    _score_font;
			renderer::Text_dynamic _score_text;

			renderer::Bubble_renderer _bubble_renderer;
			asset::Asset_manager& _assets;

			Time _time_acc = Time{0};
	};

}
}
}
