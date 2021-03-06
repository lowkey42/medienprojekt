#include "primitives.hpp"

#include "texture.hpp"

namespace mo {
namespace renderer {

	Vertex_layout simple_vertex_layout {
		Vertex_layout::Mode::triangles,
		vertex("position", &Simple_vertex::xy),
		vertex("uv", &Simple_vertex::uv)
	};

	namespace {
		struct Ray_vertex {
			glm::vec2 pos;
			float radius_pos;
		};

		auto ray_layout = renderer::Vertex_layout{
			renderer::Vertex_layout::Mode::triangle_strip,
			renderer::vertex("position", &Ray_vertex::pos),
			renderer::vertex("radius_pos", &Ray_vertex::radius_pos)
		};
		auto ray_mesh = std::vector<Ray_vertex>{
			{{0,0},-1}, {{1,0},-1}, {{0,1},1}, {{1,1},1}
		};
	}

	Ray_renderer::Ray_renderer(asset::Asset_manager& assets)
		: _obj(ray_layout, create_buffer(ray_mesh)) {

		_prog.attach_shader(assets.load<Shader>("frag_shader:ray"_aid))
			 .attach_shader(assets.load<Shader>("vert_shader:ray"_aid))
			 .bind_all_attribute_locations(ray_layout)
			 .build()
			 .bind()
			 .set_uniform("color", glm::vec4(1,0,0,0.5));
	}

	void Ray_renderer::set_vp(const glm::mat4& vp) {
		_prog.bind().set_uniform("vp", vp);
	}

	void Ray_renderer::draw(glm::vec3 p, Angle a, float length, glm::vec4 color, float width) {
		auto scale = glm::scale(glm::translate(glm::mat4(), glm::vec3(0,-width/2.f,0)),
								glm::vec3(length, width, 1));
		auto rot = glm::rotate(a.value(), glm::vec3(0,0,1));
		auto trans = glm::translate(glm::mat4(), p);

		_prog.bind()
				.set_uniform("model", trans*rot*scale)
		        .set_uniform("color", color);
		_obj.draw();
	}


	namespace {
		auto create_bubble_mesh(float radius) -> std::vector<Simple_vertex> {
			using glm::vec2;

			constexpr auto steps = 12;

			std::vector<Simple_vertex> m;
			m.reserve(steps+1);

			m.emplace_back(vec2{0,0},vec2{.5f,.5f});

			for(int i=0; i<=steps; ++i) {
				float phi = i/float(steps);

				auto x = std::cos(phi * 2*PI);
				auto y = std::sin(phi * 2*PI);
				m.emplace_back(vec2{x,y} * radius, (vec2{x,-y}+1.f)/2.f);
			}

			return m;
		}

		Vertex_layout bubble_vertex_layout {
			Vertex_layout::Mode::triangle_fan,
			vertex("position", &Simple_vertex::xy),
			vertex("uv", &Simple_vertex::uv)
		};
	}

	Bubble_renderer::Bubble_renderer(asset::Asset_manager& assets, float radius)
	    : _radius(radius), _obj(bubble_vertex_layout, create_buffer(create_bubble_mesh(radius)))
	{
		_prog.attach_shader(assets.load<Shader>("frag_shader:bubble"_aid))
			 .attach_shader(assets.load<Shader>("vert_shader:bubble"_aid))
			 .bind_all_attribute_locations(bubble_vertex_layout)
			 .build();
	}

	void Bubble_renderer::set_vp(const glm::mat4& vp) {
		_prog.bind().set_uniform("vp", vp);
	}

	void Bubble_renderer::draw(glm::vec2 center, float fill_level, float activity, float time, const Texture& texture) {
		auto trans = glm::translate(glm::mat4(), {center.x, center.y, 0});

		texture.bind();
		_prog.bind()
		     .set_uniform("model", trans)
		     .set_uniform("fill_level", glm::clamp(fill_level, 0.f, 1.f))
		     .set_uniform("time", time)
		     .set_uniform("activity", glm::clamp(activity, 0.f, 1.f))
		     .set_uniform("color", glm::vec4(1,1,1,1))
		     .set_uniform("texture", 0);
		_obj.draw();
	}

	void Bubble_renderer::draw_glow(glm::vec2 center, float fill_level, float activity, float time, const Texture& texture) {
		if(activity>0) {
			auto trans = glm::translate(glm::mat4(), {center.x+_radius*0, center.y-2, 0});

			texture.bind();
			_prog.bind()
			     .set_uniform("model", trans)
			     .set_uniform("fill_level", glm::clamp(fill_level, 0.f, 1.f))
			     .set_uniform("time", time)
			     .set_uniform("activity", glm::clamp(activity, 0.f, 1.f))
			     .set_uniform("color", glm::vec4(0.2 * activity,0.2 * activity,0.2 * activity, 0.0))
			     .set_uniform("texture", 0);
			_obj.draw();
		}
	}


	namespace {

		auto create_box_mesh(float width, float height) -> std::vector<Simple_vertex> {


			std::vector<Simple_vertex> retVec {
				{{-width / 2, -height / 2}, {0, 1}},
				{{-width / 2, height / 2}, {0, 0}},
				{{width / 2, height / 2}, {1, 0}},

				{{width / 2, height / 2}, {1, 0}},
				{{width / 2, -height / 2}, {1, 1}},
				{{-width / 2, -height / 2}, {0, 1}},
			};

			return retVec;
		}

	}

	Sprite_renderer::Sprite_renderer(asset::Asset_manager& assets)
	    : _obj(simple_vertex_layout, create_buffer(create_box_mesh(1,1))) {

		_prog.attach_shader(assets.load<Shader>("vert_shader:simple"_aid))
		     .attach_shader(assets.load<Shader>("frag_shader:simple"_aid))
		     .bind_all_attribute_locations(simple_vertex_layout)
		     .build();
	}

	void Sprite_renderer::set_vp(const glm::mat4& vp) {
		_prog.bind().set_uniform("VP", vp);
	}

	void Sprite_renderer::draw(Texture_ptr tex, glm::vec2 center,
	                           glm::vec4 color, glm::vec4 clip, glm::vec2 size,
	                           glm::mat4 transform) {

		if(size.x==0.f || size.y==0.f) {
			size.x = tex->width() *clip.z;
			size.y = tex->height()*clip.w;
		}

		auto trans = glm::scale(glm::translate(glm::mat4(),

		                            glm::vec3(center.x, center.y, 0.f)), {size.x,size.y,1});

		_prog.bind()
		     .set_uniform("model", trans * transform)
		     .set_uniform("color", color)
		     .set_uniform("clip", clip)
		     .set_uniform("texture", 0)
		     .set_uniform("layer", 0.f);

		tex->bind();
		_obj.draw();
	}


	Textured_box::Textured_box(asset::Asset_manager& assets, Texture_ptr tex, int width, int height)
		: _obj(simple_vertex_layout, create_buffer(create_box_mesh(width>0?width:tex->width(), height>0?height:tex->height())))
	{
		_prog.attach_shader(assets.load<Shader>("vert_shader:simple"_aid))
		     .attach_shader(assets.load<Shader>("frag_shader:simple"_aid))
		     .bind_all_attribute_locations(simple_vertex_layout)
		     .build()
		     .bind().set_uniform("color", glm::vec4(1,1,1,1));

		_texture = tex;

	}

	void Textured_box::set_vp(const glm::mat4& vp) {
		_prog.bind().set_uniform("VP", vp);
	}
	void Textured_box::set_color(const glm::vec4& c) {
		_prog.bind().set_uniform("color", c);
	}

	void Textured_box::draw(glm::vec2 center) {
		auto trans = glm::translate(glm::mat4(), glm::vec3(center.x, center.y, 0.f));

		_prog.bind().set_uniform("model", trans)
		            .set_uniform("clip", glm::vec4(0,0,1,1))
					.set_uniform("layer", 0.f);
		_texture->bind();
		_obj.draw();

	}

}
}
