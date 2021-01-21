#include "Projectile.h"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include "DataTables.h"
#include "Utility.h"
#include "EmitterNode.h"

namespace
{
    const std::map<Projectile::Type,ProjectileData> TABLE = initializeProjectileData();
}
Projectile::Projectile(Type type, const TextureHolder_t& textures)
    : Entity(1)
    , type(type)
    , sprite(textures.get(TABLE.at(type).texture), TABLE.at(type).textureRect)
    , targetDirection()
{
    centerOrigin(sprite);

    if (isGuided()) 
    {
        std::unique_ptr<EmitterNode> smoke(new EmitterNode(Particle::Type::Smoke));
        smoke->setPosition(0.f, getBoundingRect().height / 2.f);
        attachChild(std::move(smoke));

        std::unique_ptr<EmitterNode> fire(new EmitterNode(Particle::Type::Propellant));
        fire->setPosition(0.f, getBoundingRect().height / 2.f);
        attachChild(std::move(fire));
    }
}

void Projectile::guideTowards(sf::Vector2f position)
{
    assert(isGuided());
    targetDirection = unitVector(position - getWorldPosition());
}

bool Projectile::isGuided() const
{
    return type == Type::Missile;
}

unsigned int Projectile::getCategory() const
{
    if (type == Projectile::Type::EnemyBullet)
        return Category::EnemyProjectile;
    else
        return Category::AlliedProjectile;
}

float Projectile::getMaxSpeed() const
{
    return TABLE.at(type).speed;
}

int Projectile::getDamage() const
{
    return TABLE.at(type).damage;
}

sf::FloatRect Projectile::getBoundingRect() const
{
    return getWorldTransform().transformRect(sprite.getGlobalBounds());
}

void Projectile::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(sprite, states);
}

void Projectile::updateCurrent(sf::Time dt, CommandQueue& commands)
{
    if (isGuided())
    {
        const float approachRate = 200.f;
    
        sf::Vector2f newVelocity = unitVector(approachRate * dt.asSeconds() * targetDirection + getVelocity());
    
        newVelocity *= getMaxSpeed();
        float angle = std::atan2(newVelocity.y, newVelocity.x);
    
        setRotation(toDegree(angle) + 90.f);
        setVelocity(newVelocity);
    }

    Entity::updateCurrent(dt, commands);
}

sf::Vector2f Projectile::unitVector(sf::Vector2f pos)
{
    return normalize(pos);
}
