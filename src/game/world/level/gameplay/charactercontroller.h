#pragma once

#include "thingcontroller.h"

namespace Gorc {
namespace Game {
namespace World {
namespace Level {

class Thing;

namespace Gameplay {

enum class StandingMaterial {
	None,
	Hard,
	Dirt,
	Metal,
	ShallowWater,
	DeepWater,
	VeryDeepWater
};

class FilteredClosestRayResultCallback;

class CharacterController : public ThingController {
private:
	StandingMaterial GetStandingMaterial(Thing& thing);

	void PlayRunningAnimation(int thing_id, Thing& thing, double speed);
	void PlayStandingAnimation(int thing_id, Thing& thing);

	void RunFallingSweep(int thing_id, Thing& thing, double dt, FilteredClosestRayResultCallback& rrcb);
	void RunWalkingSweep(int thing_id, Thing& thing, double dt, FilteredClosestRayResultCallback& rrcb);
	void UpdateFalling(int thing_id, Thing& thing, double dt);
	void UpdateStandingOnSurface(int thing_id, Thing& thing, double dt);
	void UpdateStandingOnThing(int thing_id, Thing& thing, double dt);
	void StepOnSurface(int thing_id, Thing& thing, unsigned int surf_id, const FilteredClosestRayResultCallback& rrcb);
	void StepOnThing(int thing_id, Thing& thing, int step_thing_id, const FilteredClosestRayResultCallback& rrcb);
	void LandOnSurface(int thing_id, Thing& thing, unsigned int surf_id, const FilteredClosestRayResultCallback& rrcb);
	void LandOnThing(int thing_id, Thing& thing, int land_thing_id, const FilteredClosestRayResultCallback& rrcb);
	void Jump(int thing_id, Thing& thing);
	void JumpFromSurface(int thing_id, Thing& thing, unsigned int surf_id);
	void JumpFromThing(int thing_id, Thing& thing, int jump_thing_id);

public:
	using ThingController::ThingController;

	virtual void Update(int thing_id, double dt) override;
	virtual void CreateControllerData(int thing_id) override;
	virtual void HandleAnimationMarker(int thing_id, Flags::KeyMarkerType marker) override;

	void PlayLeftRunFootstep(int thing_id);
	void PlayRightRunFootstep(int thing_id);
};

}
}
}
}
}
