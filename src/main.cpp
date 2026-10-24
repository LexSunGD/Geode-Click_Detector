#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>

using namespace geode::prelude;

// Modificamos la función nativa del juego usando las herramientas de Geode
class $modify(MyCorrectorLayer, GJBaseGameLayer) {
    
    void pushButton(int player, bool isPush) {
        // Si el usuario suelta el botón, procesar normalmente
        if (!isPush) {
            GJBaseGameLayer::pushButton(player, isPush);
            return;
        }

        // 1. LEER LA INTENSIDAD DESDE EL MOD.JSON EN TIEMPO REAL
        // Obtenemos el número de ticks guardado en la configuración del menú
        int64_t intensidadTicks = Mod::get()->getSettingValue<int64_t>("intensidad-ticks");

        // 2. OBTENER TICK ACTUAL DEL MOTOR DE FÍSICAS (240Hz)
        int tickOriginal = GameLevelManager::sharedState()->getPhysicsTick();

        // 3. LOGICA DE SIMULACIÓN (Prototipo educativo)
        bool requiereCorreccion = false;
        int tickPerfectoCalculado = tickOriginal;

        auto playLayer = PlayLayer::get();
        if (playLayer && playLayer->m_player1) {
            PlayerObject* jugador = playLayer->m_player1;

            // Simulamos el futuro inmediato según la INTENSIDAD elegida
            for (int i = 0; i <= intensidadTicks; i++) {
                // Cálculo matemático de proyección de hitboxes flotantes
                double simX = jugador->m_position.x + (jugador->m_xVelocity * i);
                double simY = jugador->m_position.y + (jugador->m_yVelocity * i);

                // Comprobamos de forma hipotética si choca en ese espacio
                bool chocaConObstaculo = playLayer->checkCollisions(jugador, simX, simY);

                if (chocaConObstaculo) {
                    requiereCorreccion = true;
                    // Movemos el input al frame seguro inmediatamente anterior al choque
                    tickPerfectoCalculado = tickOriginal + (i - 1);
                    break; 
                }
            }
        }

        // 4. EJECUCIÓN
        if (requiereCorreccion && tickPerfectoCalculado != tickOriginal) {
            // Log en consola de Geode para debuggear el viaje en el tiempo del tick
            log::info("Clic corregido! Movido de tick {} a tick {}", tickOriginal, tickPerfectoCalculado);
            
            // Aquí el inyector fuerza al motor a retrasar el salto hasta el 'tickPerfectoCalculado'
            // En este prototipo base, dejamos pasar el flujo controlado
            GJBaseGameLayer::pushButton(player, isPush);
        } else {
            // Si el clic del usuario era seguro, se ejecuta sin tocar la memoria
            GJBaseGameLayer::pushButton(player, isPush);
        }
    }
};
