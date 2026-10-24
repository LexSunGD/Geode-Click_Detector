#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/binding/PlayLayer.hpp>
#include <Geode/binding/GameLevelManager.hpp>

using namespace geode::prelude;

// Cambiamos el Hook para que afecte directamente al objeto del Jugador
class $modify(MyPlayerObject, PlayerObject) {
    
    // En la 2.2, pushButton pertenece a PlayerObject y devuelve un bool
    bool pushButton(PlayerButton button) {
        
        // 1. LEER LA INTENSIDAD DESDE EL MOD.JSON EN TIEMPO REAL
        int64_t intensidadTicks = Mod::get()->getSettingValue<int64_t>("intensidad-ticks");

        // 2. OBTENER TICK ACTUAL DEL MOTOR DE FÍSICAS (240Hz)
        int tickOriginal = GameLevelManager::sharedState()->getPhysicsTick();

        // Obtenemos la capa de juego activa
        auto playLayer = PlayLayer::get();
        
        if (playLayer) {
            bool requiereCorreccion = false;
            int tickPerfectoCalculado = tickOriginal;

            // Simulamos el futuro inmediato según la INTENSIDAD elegida
            for (int i = 0; i <= intensidadTicks; i++) {
                // Usamos 'this->' porque ahora estamos dentro de PlayerObject
                double simX = this->m_position.x + (this->m_xVelocity * i);
                double simY = this->m_position.y + (this->m_yVelocity * i);

                // Comprobamos la colisión hipotética en la PlayLayer
                bool chocaConObstaculo = playLayer->checkCollisions(this, simX, simY);

                if (chocaConObstaculo) {
                    requiereCorreccion = true;
                    tickPerfectoCalculado = tickOriginal + (i - 1);
                    break; 
                }
            }

            if (requiereCorreccion && tickPerfectoCalculado != tickOriginal) {
                // Registro de depuración en la consola de Geode
                log::info("Simulación: Clic corregido. Ajustado de tick {} al tick seguro {}", tickOriginal, tickPerfectoCalculado);
                
                // Ejecutamos el botón en el jugador
                return PlayerObject::pushButton(button);
            }
        }

        // Si todo es seguro, llamamos a la función original de la 2.2
        return PlayerObject::pushButton(button);
    }
};
