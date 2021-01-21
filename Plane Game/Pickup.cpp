#include "Pickup.h"
#include "DataTables.h"
#include "Utility.h"
#include "SFML/Graphics/RenderTarget.hpp"
#include "SFML/Graphics/RenderStates.hpp"

namespace {
	const std::map<Pickup::Type,PickupData> TABLE = initializePickupData();
}

Pickup::Pickup(Type type, const TextureHolder_t& textures)
	: Entity(1)
	, type(type)
	, sprite(textures.get(TABLE.at(type).texture),TABLE.at(type).textureRect)
{
	centerOrigin(sprite);
}

unsigned int Pickup::getCategory() const
{
	return Category::Pickup;
}

sf::FloatRect Pickup::getBoundingRect() const
{
	return getWorldTransform().transformRect(sprite.getGlobalBounds());
}

void Pickup::apply(Aircraft& player) const
{
	TABLE.at(type).action(player);
}

void Pickup::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(sprite, states);
}
