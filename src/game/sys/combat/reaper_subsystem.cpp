#include "reaper_subsystem.hpp"

#include <core/ecs/ecs.hpp>
#include <core/units.hpp>
#include <core/utils/random.hpp>
#include <core/asset/aid.hpp>

#include "../state/state_comp.hpp"
#include "../graphic/sprite_comp.hpp"
#include "../physics/transform_comp.hpp"
#include "../physics/physics_comp.hpp"
#include "comp/score_comp.hpp"
#include "comp/explosive_comp.hpp"
#include "comp/health_comp.hpp"
#include "comp/weapon_comp.hpp"

namespace mo {
namespace sys {
namespace combat {

	using namespace state;
	using namespace unit_literals;

	namespace {
		auto rng = util::create_random_generator();

		glm::vec2 random_dir() {
			return rotate(glm::vec2{1,0}, Angle{util::random_real(rng, remove_unit(0_deg), remove_unit(360_deg))});
		}
	}

	Reaper_subsystem::Reaper_subsystem(
	        ecs::Entity_manager& entity_manager,
	        state::State_system& state_system,
	        Effect_source& effect,
	        FFeedback_source& ffeedback)
		: _em(entity_manager), _reap_slot(&Reaper_subsystem::_reap, this),
	      _effects(effect), _ffeedback(ffeedback) {

		_reap_slot.connect(state_system.state_change_events);
	}

	void Reaper_subsystem::_reap(ecs::Entity& e, sys::state::State_data& s) {
		if(s.s==Entity_state::dying) {
			e.get<Health_comp>().process([&](auto& hc) {
				if(hc.death_effect()!=Effect_type::none)
					_effects.inform(e, hc.death_effect());

				if(hc._death_force_feedback>0.f) {
					e.get<physics::Transform_comp>().process([&](auto& transform){
						_ffeedback.inform(transform.position(), hc._death_force_feedback);
					});
				}
			});

			e.erase<physics::Physics_comp>();
			e.erase<Health_comp>();
			e.erase<Weapon_comp>();

			e.get<physics::Transform_comp>().process([&](auto& transform){
				transform.layer(0.1f);

				e.get<Score_comp>().process([&](Score_comp& s){
					auto count = std::min(s._value/2, 100);
					for(int i=0; i<count; ++i) {
						auto coin = _em.emplace("blueprint:coin"_aid);
						coin->get<physics::Transform_comp>().get_or_throw().position(transform.position());
						coin->get<physics::Physics_comp>().get_or_throw().impulse(random_dir()  *10_n);
					}
				});
			});
		}

		if(s.s==Entity_state::dead) { // he's dead jim

			if(e.get<State_comp>().get_or_throw().delete_dead())
				_em.erase(e.shared_from_this());

			else // remove components:
				e.erase_other<
						Explosive_comp,
						physics::Transform_comp,
						graphic::Sprite_comp,
						state::State_comp>();
		}
	}


}
}
}

