/**************************************************************************\
 * lines, circles, ...                                                    *
 *                                               ___                      *
 *    /\/\   __ _  __ _ _ __  _   _ _ __ ___     /___\_ __  _   _ ___     *
 *   /    \ / _` |/ _` | '_ \| | | | '_ ` _ \   //  // '_ \| | | / __|    *
 *  / /\/\ \ (_| | (_| | | | | |_| | | | | | | / \_//| |_) | |_| \__ \    *
 *  \/    \/\__,_|\__, |_| |_|\__,_|_| |_| |_| \___/ | .__/ \__,_|___/    *
 *                |___/                              |_|                  *
 *                                                                        *
 * Copyright (c) 2014 Florian Oetke                                       *
 *  Based on code of GDW-SS2014 project by Stefan Bodenschatz             *
 *  which was distributed under the MIT license.                          *
 *                                                                        *
 *  This file is part of MagnumOpus and distributed under the MIT License *
 *  See LICENSE file for details.                                         *
\**************************************************************************/

#pragma once

#include "../units.hpp"
#include "shader.hpp"
#include "vertex_object.hpp"
#include "texture.hpp"

namespace mo {
namespace renderer {

	class Texture;

	struct Simple_vertex {
		glm::vec2 xy;
		glm::vec2 uv;
		Simple_vertex(glm::vec2 xy, glm::vec2 uv) : xy(xy), uv(uv) {}
	};
	extern Vertex_layout simple_vertex_layout;


	class Ray_renderer {
		public:
			Ray_renderer(asset::Asset_manager& assets);

			void set_vp(const glm::mat4& vp);

			void draw(glm::vec3 p, Angle a, float length, glm::vec4 color, float width=1);

		private:
			Shader_program _prog;
			Object _obj;
	};

	class Bubble_renderer {
		public:
			Bubble_renderer(asset::Asset_manager& assets, float radius);

			void set_vp(const glm::mat4& vp);

			void draw(glm::vec2 center, float fill_level, float activity, float time, const Texture& texture);

			void draw_glow(glm::vec2 center, float fill_level, float activity, float time, const Texture& texture);

		private:
			const float _radius;
			Shader_program _prog;
			Object _obj;
	};

	class Sprite_renderer {
		public:
			Sprite_renderer(asset::Asset_manager& assets);

			void set_vp(const glm::mat4& vp);

			void draw(Texture_ptr tex, glm::vec2 center,
			          glm::vec4 color=glm::vec4{1,1,1,1},
			          glm::vec4 clip=glm::vec4{0,0,1,1},
			          glm::vec2 size=glm::vec2(0,0),
			          glm::mat4 transform=glm::mat4());

		private:
			Shader_program _prog;
			Object _obj;
	};

	class Textured_box {
		public:
			Textured_box(asset::Asset_manager& assets, Texture_ptr tex, int width = 0, int height = 0);

			void set_vp(const glm::mat4& vp);
			void set_color(const glm::vec4& c);

			void draw(glm::vec2 center);

			auto texture()const noexcept {return _texture;}

		private:
			Shader_program _prog;
			Object _obj;
			Texture_ptr _texture;

	};

}
}
