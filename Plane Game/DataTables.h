#pragma once

#include "ResourceIdentifier.h"
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/Color.hpp>
#include <map>
#include <vector>
#include <functional>
#include "Aircraft.h"
#include "Projectile.h"
#include "Pickup.h"
#include "Particle.h"

struct Direction {
	Direction(float angle, float distance)
	: angle(angle)
	, distance(distance)
	{}
	float angle;
	float distance;
};

struct AircraftData
{
	int							hitpoints;
	float						speed;
	TextureID					texture;
	sf::IntRect					textureRect;
	sf::Time					fireInterval;
	std::vector<Direction>		directions;
};
struct ProjectileData
{
	int								damage;
	float							speed;
	TextureID						texture;
	sf::IntRect						textureRect;
};
struct PickupData
{
	std::function<void(Aircraft&)>	action;
	TextureID						texture;
	sf::IntRect						textureRect;
};
struct ParticleData
{
	sf::Color						color;
	sf::Time						lifetime;
};

//functions to fill data tables
std::map <Aircraft::Type, AircraftData>		initializeAircraftData();
std::map<Projectile::Type, ProjectileData>	initializeProjectileData();
std::map<Pickup::Type, PickupData>			initializePickupData();
std::map<Particle::Type, ParticleData>		initializeParticleData();