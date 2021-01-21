#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/Color.hpp>

struct Particle
{
	enum class Type
	{
		Propellant,
		Smoke,
		particleCount
	};

	sf::Vector2f	position;
	sf::Color		color;
	sf::Time		lifetime;
};