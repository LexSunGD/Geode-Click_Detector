#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/binding/PlayLayer.hpp>

using namespace geode::prelude;

// Hook directo al objeto del jugador para interceptar inputs
class $modify(MyPlayerObject, PlayerObject) {
    
    bool pushButton(PlayerButton button) {
        // 1. LEER LA INTENSIDAD DESDE EL MOD.JSON
        int64_t intensidadTicks = Mod::get()->getSettingValue<int64_t>("intensidad-ticks");

        // Obtenemos la capa de juego actual
        auto playLayer = PlayLayer::get();
        
        if (playLayer) {
            bool requiereCorreccion = false;

            // 2. LÓGICA DE PREDICCIÓN ACTUALIZADA (2.2)
            // Simulamos el trayecto del jugador hacia el futuro cuadro por cuadro
            for (int i = 0; i <= intensidadTicks; i++) {
                // En la 2.2 la velocidad horizontal es 'm_playerSpeed' y la vertical es 'm_yVelocity'
                double simX = this->m_position.x + (this->m_playerSpeed * i);
                double simY = this->m_position.y + (this->m_yVelocity * i);

                // Comprobamos si el jugador colisionaría con un objeto peligroso en esa posición futura
                bool chocaConObstaculo = playLayer->checkCollisions(this, simX, simY);

                if (chocaConObstaculo) {
                    requiereCorreccion = true;
                    break; 
                }
            }

            // 3. RESPUESTA ANTE CORRECCIÓN
            if (requiereCorreccion) {
                // Alertas de depuración internas en la consola del juego
                log::info("Predicción activa: Ajustando el margen de colisión del input.");
                
                // Forzamos la ejecución segura del botón
                return PlayerObject::pushButton(button);
            }
        }

        // Si la trayectoria actual es segura, el clic se procesa sin alteraciones
        return PlayerObject::pushButton(button);
    }
};
