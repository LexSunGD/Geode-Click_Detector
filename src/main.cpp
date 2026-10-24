#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/binding/PlayLayer.hpp>

using namespace geode::prelude;

class $modify(MyPlayerObject, PlayerObject) {
    
    bool pushButton(PlayerButton button) {
        // 1. Leer intensidad (Margen de tiempo en segundos, ej: 0.05s)
        int64_t intensidadTicks = Mod::get()->getSettingValue<int64_t>("intensidad-ticks");
        
        auto playLayer = PlayLayer::get();
        if (playLayer) {
            // OBTENEMOS EL DELTA TIME ACTUAL (Se adapta automáticamente al Speed Hack)
            // Normalmente en 60Hz es 0.016, a 240Hz es 0.004. Si usas Speedhack, este número cambia.
            float dtReal = CCDirector::sharedDirector()->getDeltaTime();
            
            bool requiereCorreccion = false;

            // 2. LÓGICA ADAPTATIVA
            for (int i = 1; i <= intensidadTicks; i++) {
                // Multiplicamos por (dtReal * i) para saber la distancia exacta adaptada al Speed Hack
                double simX = this->m_position.x + (this->m_playerSpeed * (dtReal * i) * 60.0); 
                double simY = this->m_position.y + (this->m_yVelocity * (dtReal * i) * 60.0);

                // Comprobar la colisión exacta en esa fracción de segundo modificada
                bool chocaConObstaculo = playLayer->checkCollisions(this, simX, simY);

                if (chocaConObstaculo) {
                    requiereCorreccion = true;
                    break; 
                }
            }

            if (requiereCorreccion) {
                log::info("Corrección adaptada al Speed Hack exitosa.");
                return PlayerObject::pushButton(button);
            }
        }

        return PlayerObject::pushButton(button);
    }
};
