#pragma once

#include "libold/base/utility/entity_id.hpp"

namespace gorc {
namespace game {
namespace world {
namespace sounds {
namespace components {

class voice {
public:
    #include "voice.uid"

    thing_id sound;

    voice(thing_id sound);
};

}
}
}
}
}
