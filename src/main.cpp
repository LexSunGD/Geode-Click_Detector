#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/binding/PlayLayer.hpp>

using namespace geode::prelude;

class $modify(MyPlayerObject, PlayerObject) {
    // Registramos el update físico del jugador
    void update(float dt) {
        PlayerObject::update(dt);

        // 1. Verificar si estamos jugando y si el vehículo actual es el WAVE
        auto playLayer = PlayLayer::get();
        if (!playLayer || !this->m_isDart) return; // m_isDart es la variable interna para el Wave

        // 2. Obtener el ajuste de distancia de seguridad desde el mod.json
        // En lugar de ticks, usamos "unidades de juego" (ej. 30.0f es aproximadamente un bloque)
        float distanciaSeguridad = static_cast<float>(Mod::get()->getSettingValue<int64_t>("intensidad-ticks")) * 10.0f;

        // 3. Calcular el vector de dirección del Wave
        // El Wave en GD sube o baja en un ángulo constante dependiendo de m_isGoingUp
        float direccionY = this->m_isGoingUp ? 1.0f : -1.0f;
        
        // Obtenemos la velocidad real del Wave multiplicada por el delta time
        float velocidadX = this->m_playerSpeed * dt * 60.0f;
        float velocidadY = this->m_yVelocity * dt * 60.0f;

        // 4. PREDICCIÓN REAL: Desplazar una copia matemática temporal hacia adelante
        cocos2d::CCPoint posicionFutura = this->m_position;
        
        // Avanzamos en la diagonal actual para ver si hay colisión inminente
        posicionFutura.x += velocidadX * (distanciaSeguridad / 10.0f);
        posicionFutura.y += velocidadY * (distanciaSeguridad / 10.0f) * direccionY;

        // 5. Preguntar al motor del juego si en esa coordenada exacta hay muerte o bloque
        // checkCollisions en la 2.2 devuelve true si el jugador intersecta un objeto sólido/letal en esa posición
        bool choqueInminente = playLayer->checkCollisions(this, posicionFutura.x, posicionFutura.y);

        if (choqueInminente) {
            // ¡Peligro detectator! Si iba hacia arriba, debe bajar, y si iba abajo, debe subir.
            // Para cambiar la dirección del Wave, simulamos el input correspondiente:
            if (this->m_isGoingUp) {
                // Si va hacia arriba y va a chocar (con el techo o una espina), soltamos el click
                this->releaseButton(PlayerButton::Jump);
                log::info("[WaveBot] ¡Liberación automática ejecutada para evitar choque!");
            } else {
                // Si va hacia abajo y va a chocar (con el suelo o una espina), presionamos el click
                this->pushButton(PlayerButton::Jump);
                log::info("[WaveBot] ¡Click automático ejecutado para evitar choque!");
            }
        }
    }
};
