#pragma once
#include "Entity.h"
#include "ResourceIdentifier.h"
#include "ResourceHolder.h"
#include "TextNode.h"
#include "CommandQueue.h"
#include "Command.h"
#include "Projectile.h"
#include "Animation.h"
#include <SFML/Graphics/Sprite.hpp>

class Aircraft : public Entity
{
public:
	enum class Type {Eagle, Raptor, Avenger};

public:
						    Aircraft(Type t, const TextureHolder_t& textures, const FontHolder_t& fonts);

	virtual unsigned int	getCategory() const override;
	virtual bool			isMarkedForRemoval() const override;

	void                    increaseFireRate();
	void                    increaseSpread();
	void                    collectMissiles(size_t count);

	void					fire();
	void					launchMissile();

	void					playLocalSound(CommandQueue& commands, EffectID effect);


private:
	virtual void			drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const override;
	
	virtual void			updateCurrent(sf::Time dt, CommandQueue& commands) override;
	void					updateTexts();
	void					updateMovementPattern(sf::Time dt);

	virtual sf::FloatRect	getBoundingRect() const;
	float					getMaxSpeed();

	void					checkPickupDrop(CommandQueue& commands);
	void					checkProjectileLaunch(sf::Time dt, CommandQueue& commands);
	void					createBullets(SceneNode& node, const TextureHolder_t& textures) const;
	void					createProjectile(SceneNode& node, Projectile::Type type,
												float xOffset,float yOffset,
												const TextureHolder_t& textures) const;

	void                    createPickup(SceneNode& node, const TextureHolder_t& textures) const;
	
	bool					isAllied() const;
private:
	Type					type;
	sf::Sprite				sprite;
	Animation				explosion;
	bool					showExplosion;
	bool					spawnedPickup;
	bool					playedExplosionEffect;

	Command					fireCommand;
	Command					missileCommand;
	Command					dropPickupCommand;

	sf::Time				fireCountdown;
	bool					isFiring;
	bool					isLaunchingMissile;
	bool                    markedForRemoval;

	int						missileAmmo;
	int						fireRateLevel;
	int						spreadLevel;

	float					travelledDistance;
	size_t					directionIndex;

	TextNode*				healthDisplay;
	TextNode*				missileDisplay;

	
};

     