#pragma once
#include "Aircraft.h"
#include "Entity.h"
class Pickup : public Entity
{
public:
    enum Type
    {
        HealthRefill,
        MissileRefill,
        FireSpread,
        FireRate,
        TypeCount
    };

public:
                           Pickup(Type type, const TextureHolder_t& textures);

    virtual unsigned int   getCategory() const;
    virtual sf::FloatRect  getBoundingRect() const;

    void                   apply(Aircraft& player) const;

protected:
    virtual void           drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;

private:
    Type              type;
    sf::Sprite        sprite;
};

