#pragma once

#include "content/id.hpp"
#include "libold/content/flags/armed_mode.hpp"

namespace gorc {
namespace game {
namespace world {
namespace events {

class armed_mode_changed {
public:
    #include "armed_mode_changed.uid"

    thing_id thing;
    flags::armed_mode mode;

    armed_mode_changed(thing_id thing, flags::armed_mode mode);
};

}
}
}
}
