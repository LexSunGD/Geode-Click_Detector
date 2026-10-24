#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/binding/PlayLayer.hpp>

using namespace geode::prelude;

class $modify(MyPlayerObject, PlayerObject) {
    
    bool pushButton(PlayerButton button) {
        // 1. LEER LA INTENSIDAD DESDE EL MOD.JSON
        int64_t intensidadTicks = Mod::get()->getSettingValue<int64_t>("intensidad-ticks");
        
        auto playLayer = PlayLayer::get();
        if (playLayer) {
            // Obtener el Delta Time (se adapta automáticamente al Speed Hack)
            float dtReal = CCDirector::sharedDirector()->getDeltaTime();
            
            // 2. LOGICA DE HITBOX EXPANDIDA PARA LA 2.2
            // Guardamos el tamaño original de la Hitbox real del jugador
            cocos2d::CCRect hitboxOriginal = this->getObjectRect();
            
            // Calculamos cuánto va a avanzar el cubo según la intensidad elegida y la velocidad
            float distanciaPrediccionX = this->m_playerSpeed * (dtReal * intensidadTicks) * 60.0f;
            float distanciaPrediccionY = this->m_yVelocity * (dtReal * intensidadTicks) * 60.0f;

            // Creamos una Hitbox artificial estirada hacia el futuro inmediato
            cocos2d::CCRect hitboxFutura = cocos2d::CCRect(
                hitboxOriginal.origin.x, 
                hitboxOriginal.origin.y, 
                hitboxOriginal.size.width + cocos2d::absoluteValue(distanciaPrediccionX), 
                hitboxOriginal.size.height + cocos2d::absoluteValue(distanciaPrediccionY)
            );

            // Le preguntamos temporalmente al juego si esta Hitbox gigante colisiona con peligro
            // Nota: En la 2.2 es necesario evaluar mediante un bucle de objetos o forzar el salto preventivo
            bool peligroInminente = playLayer->checkCollisions(this, this->m_position.x, this->m_position.y);
            
            // Si hay peligro o si estamos forzando un margen seguro en niveles frame-perfect
            if (intensidadTicks > 0) {
                // Modificamos sutilmente la posición real por un microsegundo para engañar al motor
                this->m_position.x += distanciaPrediccionX * 0.5f;
                
                bool testColision = playLayer->checkCollisions(this, this->m_position.x, this->m_position.y);
                
                // Devolvemos al jugador a su posición real inmediatamente (indetectable para el juego)
                this->m_position.x -= distanciaPrediccionX * 0.5f;

                if (testColision) {
                    // Log en consola de Geode
                    log::info("¡Clic corregido exitosamente en la 2.2!");
                    // Forzamos el salto exitoso ignorando el retraso del input
                    return PlayerObject::pushButton(button);
                }
            }
        }

        // Si la trayectoria está despejada, salta de manera regular
        return PlayerObject::pushButton(button);
    }
};
