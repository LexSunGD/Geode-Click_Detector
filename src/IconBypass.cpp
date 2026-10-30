#include <Geode/Geode.hpp>
#include <Geode/modify/GameManager.hpp>

using namespace geode::prelude;

class $modify(GameManager) {

    bool isIconUnlocked(int iconID, IconType iconType) {
        bool bypassIconActive = Mod::get()->getSettingValue<bool>("bypass-icons");
        
        if (bypassIconActive) {
            return true;
        }
        return GameManager::isIconUnlocked(iconID, iconType);
    }

    bool isColorUnlocked(int colorID, UnlockType unlockType) {
        bool bypassIconActive = Mod::get()->getSettingValue<bool>("bypass-icons");
        
        if (bypassIconActive) {
            return true;
        }
        return GameManager::isColorUnlocked(colorID, unlockType);
    }
};
