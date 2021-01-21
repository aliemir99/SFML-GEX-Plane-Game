#include "Aircraft.h"
#include <SFML/Graphics/RenderTarget.hpp>
#include "DataTables.h"
#include "Utility.h"
#include "SoundNode.h"
#include <string>

namespace {
	const std::map<Aircraft::Type, AircraftData> TABLE = initializeAircraftData();
}


Aircraft::Aircraft(Type t, const TextureHolder_t& textures, const FontHolder_t& fonts)
	: Entity(TABLE.at(t).hitpoints)
	, type(t)
	, sprite(textures.get(TABLE.at(t).texture), TABLE.at(t).textureRect)
	, explosion(textures.get(TextureID::Explosion))
	, showExplosion(true)
	, spawnedPickup(false)
	, playedExplosionEffect(false)
	, fireCommand()
	, missileCommand()
	, dropPickupCommand()
	, fireCountdown(sf::Time::Zero)
	, isFiring(false)
	, isLaunchingMissile(false)
	, markedForRemoval(false)
	, fireRateLevel(1)
	, spreadLevel(1)
	, missileAmmo(10)
	, travelledDistance(0)
	, directionIndex(0)
	, healthDisplay(nullptr)
	, missileDisplay(nullptr)
{

	explosion.setFrameSize(sf::Vector2i(256, 256));
	explosion.setNumFrames(16);
	explosion.setDuration(sf::seconds(1));
	centerOrigin(explosion);

	centerOrigin(sprite);


	//set up fire and launch commands
	fireCommand.category = Category::SceneAirLayer;
	fireCommand.action = [this, &textures](SceneNode& node, sf::Time)
	{
		this->createBullets(node, textures);
	};

	missileCommand.category = Category::SceneAirLayer;
	missileCommand.action = [this, &textures](SceneNode& node, sf::Time)
	{
		createProjectile(node, Projectile::Type::Missile,0.f,0.5f,textures);
	};

	dropPickupCommand.category = Category::SceneAirLayer;
	dropPickupCommand.action = [this, &textures](SceneNode& node, sf::Time)
	{
		createPickup(node, textures);
	};

	std::unique_ptr<TextNode> health(new TextNode(fonts, ""));
	healthDisplay = health.get();
	attachChild(std::move(health));

	if (getCategory() == Category::PlayerAircraft)
	{
		std::unique_ptr<TextNode> missiles(new TextNode(fonts, ""));
		missileDisplay = missiles.get();
		attachChild(std::move(missiles));
	}

	updateTexts();
}

unsigned int Aircraft::getCategory() const
{
	switch (type) 
	{
	case Aircraft::Type::Eagle:
		return Category::PlayerAircraft;
	case Aircraft::Type::Raptor:
		return Category::EnemyAircraft;
	default:
		return Category::EnemyAircraft;
	}
}

bool Aircraft::isMarkedForRemoval() const
{
	return isDestroyed() &&(explosion.isFinished()|| !showExplosion);
}

void Aircraft::increaseFireRate()
{
	const int MAX_FIRE_RATE = 10;
	if (fireRateLevel < MAX_FIRE_RATE)
		fireRateLevel += 1;
}

void Aircraft::increaseSpread()
{
	const int MAX_SPREAD = 3;
	if (spreadLevel < MAX_SPREAD)
		spreadLevel += 1;
}

void Aircraft::collectMissiles(size_t count)
{
	missileAmmo += count;
}

void Aircraft::fire()
{
	if (TABLE.at(type).fireInterval != sf::Time::Zero) {
		isFiring = true;
	}
}

void Aircraft::launchMissile()
{
	if (missileAmmo > 0) {
		isLaunchingMissile = true;
		missileAmmo -= 1;
	}
}

void Aircraft::playLocalSound(CommandQueue& commands, EffectID effect)
{
	auto worldPosition = getWorldPosition();

	Command command;
		command.category = Category::SoundEffect;
		command.action = derivedAction<SoundNode>(
			[effect, worldPosition](SoundNode& node, sf::Time dt)
			{
				node.playSound(effect, worldPosition);
			});
		commands.push(command);
}



void Aircraft::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (isDestroyed() && showExplosion)
		target.draw(explosion, states);
	else
		target.draw(sprite, states);
}

void Aircraft::updateCurrent(sf::Time dt, CommandQueue& commands)
{
	updateTexts();

	if (isDestroyed()) {
		checkPickupDrop(commands);
		explosion.update(dt);
		
		if (!playedExplosionEffect) 
		{
			EffectID effect = (randomInt(2) == 0 ? EffectID::Explosion1 : EffectID::Explosion2);
			playLocalSound(commands, effect);
			playedExplosionEffect = true;
		}
			return;
	}
	updateMovementPattern(dt);
	
	checkProjectileLaunch(dt, commands);

	Entity::updateCurrent(dt,commands);
	
}

void Aircraft::updateTexts()
{
	healthDisplay->setString(std::to_string(getHitpoints())+" HP");
	healthDisplay->setPosition(0.f, 50.f);
	healthDisplay->setRotation(-getRotation());

	if (missileDisplay)
	{
		if (missileAmmo == 0)
			missileDisplay->setString("M: XX");
		else
			missileDisplay->setString("M: " + std::to_string(missileAmmo));
		
		missileDisplay->setPosition(0.f, 70.f);
	}
}

void Aircraft::updateMovementPattern(sf::Time dt)
{
	//enemy plane movement
	const std::vector<Direction>& directions = TABLE.at(type).directions;
	if (!directions.empty()) {
		if (travelledDistance > directions[directionIndex].distance) {
			directionIndex = (directionIndex + 1) % directions.size();
			travelledDistance = 0.f;
		}

		float radians = toRadian(directions[directionIndex].angle + 90.f);
		float vx = getMaxSpeed() * std::cos(radians);
		float vy = getMaxSpeed() * std::sin(radians);
		setVelocity(vx, vy);
		travelledDistance += getMaxSpeed() * dt.asSeconds();
	}
}

sf::FloatRect Aircraft::getBoundingRect() const
{
	return getWorldTransform().transformRect(sprite.getGlobalBounds());
}

float Aircraft::getMaxSpeed()
{
	return TABLE.at(type).speed;
}

void Aircraft::checkPickupDrop(CommandQueue& commands)
{
	if ((!isAllied() && randomInt(2) == 0) && !spawnedPickup) 
		commands.push(dropPickupCommand);
	
	spawnedPickup = true;
}

void Aircraft::checkProjectileLaunch(sf::Time dt, CommandQueue& commands)
{
	if (!isAllied())
		fire();

	if (isFiring && fireCountdown <= sf::Time::Zero)
	{
		commands.push(fireCommand);
		fireCountdown += TABLE.at(type).fireInterval / (fireRateLevel + 1.f);
		playLocalSound(commands, 
			isAllied() ? EffectID::AlliedGunfire : EffectID::EnemyGunfire);
	}
	else if (fireCountdown > sf::Time::Zero)
	{
		fireCountdown -= dt;
	}
	isFiring = false;

	if (isLaunchingMissile)
	{
		commands.push(missileCommand);
		isLaunchingMissile = false;

		playLocalSound(commands, EffectID::LaunchMissile);	
	}
}

void Aircraft::createBullets(SceneNode& node, const TextureHolder_t& textures) const
{
	Projectile::Type type = isAllied() ? Projectile::Type::AlliedBullet : Projectile::Type::EnemyBullet;
	switch (spreadLevel)
	{
	case 1:
		createProjectile(node, type, 0.0f, 0.5f, textures);
		break;

	case 2:
		createProjectile(node, type, -0.33f, 0.33f, textures);
		createProjectile(node, type, +0.33f, 0.33f, textures);
		break;

	case 3:
		createProjectile(node, type, -0.5f, 0.33f, textures);
		createProjectile(node, type, 0.0f, 0.5f, textures);
		createProjectile(node, type, +0.5f, 0.33f, textures);
		break;
	}
}

void Aircraft::createProjectile(SceneNode& node, Projectile::Type type, float xOffset, float yOffset, const TextureHolder_t& textures) const
{
	std::unique_ptr<Projectile> projectile(new Projectile(type, textures));

	sf::Vector2f offset(xOffset * sprite.getGlobalBounds().width, yOffset * sprite.getGlobalBounds().height);
	sf::Vector2f velocity(0, projectile->getMaxSpeed());

	float sign = isAllied() ? -1.f : +1.f;
	projectile->setPosition(getWorldPosition() + offset * sign);
	projectile->setVelocity(velocity * sign);
	node.attachChild(std::move(projectile));
}

void Aircraft::createPickup(SceneNode& node, const TextureHolder_t& textures) const
{
	
	auto type = static_cast<Pickup::Type>(randomInt(static_cast<int>(Pickup::Type::TypeCount)));

	std::unique_ptr<Pickup> pickup(new Pickup(type, textures));
	pickup->setPosition(getWorldPosition());
	pickup->setVelocity(0.f, 1.f);
	node.attachChild(std::move(pickup));

}

bool Aircraft::isAllied() const
{
	return type == Type::Eagle;
}

