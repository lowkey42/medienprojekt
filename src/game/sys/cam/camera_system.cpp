#include "camera_system.hpp"

#include <game/game_engine.hpp>

#include "../physics/transform_comp.hpp"
#include "../physics/physics_comp.hpp"

#include <glm/gtc/random.hpp>

namespace mo {
namespace sys {
namespace cam {

	using namespace unit_literals;

	constexpr auto world_scale = 64; // pixel/meter
	constexpr auto vscreen_height = 512;
	constexpr auto min_zoom = 0.1f;
	constexpr auto max_zoom = 0.5f;

	VScreen::VScreen(glm::vec2 real_size, glm::vec2 vsize, float world_scale)
		: camera(vsize, world_scale), vscreen(real_size.x, real_size.y, true) {
	}

	Camera_system::Camera_system(ecs::Entity_manager& entity_manager, Game_engine& engine)
		: _gctx(engine.graphics_ctx()),
	      _targets(entity_manager.list<Camera_target_comp>()),
		  _vscreen_size(renderer::calculate_vscreen(engine, vscreen_height)),
	      _real_screen_size(engine.graphics_ctx().win_width(), engine.graphics_ctx().win_height()),
	      _main_camera(_real_screen_size, world_scale) {

		entity_manager.register_component_type<Camera_target_comp>();

		_cameras.emplace_back(_real_screen_size, _vscreen_size, world_scale);
		_cameras.back().camera.zoom(max_zoom);
	}

	void Camera_system::update(Time dt) {
		auto ffeedback_diff = (_target_force_feedback-_force_feedback);
		_force_feedback += ffeedback_diff * dt.value() * (ffeedback_diff>0 ? 10 : 3);

		if(_target_force_feedback>0 &&
		        std::abs(_target_force_feedback-_force_feedback)< 0.01f)
			_target_force_feedback = 0;


		glm::vec2 pos_acc, pos_min{99999.f, 99999.f}, pos_max;
		float pos_count=0;

		_cameras.back().targets.clear();

		for(auto& target : _targets) {

			if(target._rotation_zoom_time_left>0_s)
				target._rotation_zoom_time_left-=dt;

			Position target_offset{0_m, 0_m};

			target.owner().get<physics::Transform_comp>().process([&](auto& transform) {
				auto target_pos = transform.position();
				auto target_rot = transform.rotation();
				auto moving = false;

				// TODO[foe]: refactor
				target.owner().get<physics::Physics_comp>().process([&](auto& p){
					target_pos += p.velocity()*100_ms;

					auto tpnt = remove_units(p.velocity()*100_ms);
					moving = tpnt.x*tpnt.x + tpnt.y*tpnt.y >= 0.1f;
				});

				target._last_rotation = target._last_rotation*(1-dt.value()*4.f) + target_rot*dt.value()*4.f;

				//target_offset += rotate(Position{1_m, 0_m}, target._last_rotation);

				target.chase(target_pos, dt);
			});

			_cameras.back().targets.push_back(target.owner_ptr());
			auto p = remove_units(target.cam_position() + target_offset);
			pos_acc+=p;
			pos_count++;

			if(pos_min.x>p.x) pos_min.x = p.x;
			if(pos_min.y>p.y) pos_min.y = p.y;
			if(pos_max.x<p.x) pos_max.x = p.x;
			if(pos_max.y<p.y) pos_max.y = p.y;
		}

		if(pos_count>0) {
			pos_min-=2.0f;
			pos_max+=2.0f;

			auto& cam = _cameras.back().camera;
			auto cam_pos = pos_acc / pos_count;
			float max_dist = glm::length(pos_max-pos_min);
			auto new_zoom = glm::clamp((vscreen_height/world_scale)/max_dist, min_zoom, max_zoom);

			if(!_uninitialized) {
				cam.position(cam_pos*0.25f + cam.position()*0.75f);
				cam.zoom(cam.zoom()*0.2f + new_zoom*0.8f);

			} else {
				_uninitialized = false;
				cam.zoom(new_zoom);
				cam.position(cam_pos);
			}

		}

		_main_camera.position(_cameras.front().camera.position());
		_main_camera.zoom(_cameras.front().camera.zoom() * (_main_camera.viewport().w/_cameras.front().camera.viewport().w));
	}

	void Camera_system::reset() {
		for(auto& target : _targets) {
			target.reset();
		}
		_uninitialized = false;
	}

	auto Camera_system::_feedback_offset()const -> glm::vec2 {
		return _force_feedback>0.1 ? glm::circularRand(_force_feedback * _gctx.max_screenshake()) : glm::vec2{0,0};
	}

}
}
}
