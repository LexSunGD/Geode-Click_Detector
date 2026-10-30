#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

class $modify(MyPlayLayer, PlayLayer) {

    void destroyPlayer(PlayerObject* player, GameObject* obstacle) {
        bool isNoclipActive = Mod::get()->getSettingValue<bool>("noclip-enabled");

        if (!isNoclipActive) {
            PlayLayer::destroyPlayer(player, obstacle);
        }
    }
};
