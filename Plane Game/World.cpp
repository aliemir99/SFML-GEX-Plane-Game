#include "World.h"
#include <limits>
#include <algorithm>
#include "DataTables.h"
#include "ParticleNode.h"
#include "PostEffect.h"
#include "SoundNode.h"

World::World(sf::RenderTarget& outputTarget, FontHolder_t& fonts, SoundPlayer& sounds)
:target(outputTarget)
,sceneTexture()
,worldView(target.getDefaultView())
,textures()
,fonts(fonts)
,sounds(sounds)
,sceneGraph()
,sceneLayers()
,commandQueue()
,worldBounds(0.f,0.f, worldView.getSize().x,6000.f) //length of background scroller
,spawnPosition(worldView.getSize().x / 2.f, worldBounds.height - worldView.getSize().y / 2.f)
,scrollSpeed(-100.f)
,playerAircraft(nullptr)
{
	sceneTexture.create(target.getSize().x, target.getSize().y);
	loadTextures();
	buildScene();
	worldView.setCenter(spawnPosition);
}

void World::update(sf::Time dt)
{
	// scroll view
	worldView.move(0.f, scrollSpeed*dt.asSeconds());

	//reset player velocity
	playerAircraft->setVelocity(0.f, 0.f);
	
	destroyEntitiesOutsideView();
	guideMissiles();

	while (!commandQueue.isEmpty()) {
		sceneGraph.onCommand(commandQueue.pop(), dt);
	}
	adaptPlayerVelocity();
	//Remove all destroyed entities, create new ones
	sceneGraph.removeWrecks();
	//Collision detection and response(may destroy entities)
	handleCollisions();
	
	
	spawnEnemies();

	sceneGraph.update(dt,getCommands());
	adaptPlayerPosition();
	updateSounds();
}

void World::draw()
{
	if (PostEffect::isSupported()) 
	{
		sceneTexture.clear();
		sceneTexture.setView(worldView);
		sceneTexture.draw(sceneGraph);
		sceneTexture.display();
		bloomEffect.apply(sceneTexture, target);
	}
	else 
	{
		target.setView(worldView);
		target.draw(sceneGraph);
	}
}

CommandQueue& World::getCommands()
{
	return commandQueue;
}

bool World::hasAlivePlayer() const
{
	return !playerAircraft->isMarkedForRemoval();
}

bool World::hasPlayerReachedEnd() const
{
	return !worldBounds.contains(playerAircraft->getPosition());
}

void World::loadTextures()
{

	textures.load(TextureID::Desert, "Media/Textures/Desert.png");
	textures.load(TextureID::Jungle, "Media/Textures/Jungle.png");

	textures.load(TextureID::Explosion, "Media/Textures/Explosion.png");
	textures.load(TextureID::Particle, "Media/Textures/Particle.png");

	textures.load(TextureID::FinishLine, "Media/Textures/FinishLine.png");

	textures.load(TextureID::Entities, "Media/Textures/Entities.png");


}

void World::buildScene()
{
	for (std::size_t i = 0; i < LayerCount; ++i) {

		Category::Type category =
		(i == LowerAir) ? Category::Type::SceneAirLayer : Category::Type::None;

		SceneNode::Ptr layer(new SceneNode(category));
		sceneLayers[i] = layer.get();

		sceneGraph.attachChild(std::move(layer));
	}

	//add sound effect node
	std::unique_ptr<SoundNode> soundNode(new SoundNode(sounds));
	sceneGraph.attachChild(std::move(soundNode));

	//prepare background texture
	sf::Texture& texture = textures.get(TextureID::Jungle);
	texture.setRepeated(true);

	float viewHeight = worldView.getSize().y;
	sf::IntRect textureRect(worldBounds);
	textureRect.height += static_cast<int>(viewHeight);

	//background spriteNode
	std::unique_ptr<SpriteNode> jungle(new SpriteNode(texture,textureRect));
	jungle->setPosition(worldBounds.left, worldBounds.top-viewHeight);
	sceneLayers[Background]->attachChild(std::move(jungle));

	//finish line
	sf::Texture& finishTexture = textures.get(TextureID::FinishLine);
	std::unique_ptr<SpriteNode> finishSprite(new SpriteNode(finishTexture));
	finishSprite->setPosition(0.f, -76.f);
	sceneLayers[Background]->attachChild(std::move(finishSprite));

	// add particle systems
	std::unique_ptr<ParticleNode> smokeNode(new ParticleNode(Particle::Type::Smoke, textures));
	sceneLayers[LowerAir]->attachChild(std::move(smokeNode));

	std::unique_ptr<ParticleNode> propellantNode(new ParticleNode(Particle::Type::Propellant, textures));
	sceneLayers[LowerAir]->attachChild(std::move(propellantNode));

	


	//add player aircraft
	std::unique_ptr<Aircraft> leader(new Aircraft(Aircraft::Type::Eagle, textures, fonts));
	playerAircraft = leader.get();
	playerAircraft->setPosition(spawnPosition);
	playerAircraft->setVelocity(80.f, scrollSpeed);
	sceneLayers[UpperAir]->attachChild(std::move(leader));

	addEnemies();

}

void World::addEnemies()
{
	// Add enemies to the spawn point container
	addEnemy(Aircraft::Type::Raptor, 0.f, 1500.f);
	addEnemy(Aircraft::Type::Raptor, -170.f, 1500.f);
	addEnemy(Aircraft::Type::Raptor, 170.f, 1500.f);

	addEnemy(Aircraft::Type::Raptor, 0.f, 1000.f);
	addEnemy(Aircraft::Type::Raptor, -170.f, 500.f);
	addEnemy(Aircraft::Type::Raptor, 170.f, 1000.f);
	addEnemy(Aircraft::Type::Raptor, +100.f, 1100.f);
	addEnemy(Aircraft::Type::Raptor, -100.f, 1100.f);

	addEnemy(Aircraft::Type::Avenger, -70.f, 1400.f);
	addEnemy(Aircraft::Type::Avenger, -70.f, 1600.f);
	addEnemy(Aircraft::Type::Avenger, 70.f, 1400.f);
	addEnemy(Aircraft::Type::Avenger, 70.f, 1600.f);

	addEnemy(Aircraft::Type::Raptor, 0.f, 2500.f);
	addEnemy(Aircraft::Type::Raptor, 0.f, 3000.f);

	addEnemy(Aircraft::Type::Raptor, +100.f, 4100.f);
	addEnemy(Aircraft::Type::Raptor, -100.f, 4100.f);

	addEnemy(Aircraft::Type::Avenger, -70.f, 4400.f);
	addEnemy(Aircraft::Type::Avenger, -70.f, 4600.f);

	addEnemy(Aircraft::Type::Avenger, -170.f, 5400.f);
	addEnemy(Aircraft::Type::Avenger, 0.f, 5400.f);
	addEnemy(Aircraft::Type::Avenger, 170.f, 5400.f);

	addEnemy(Aircraft::Type::Avenger, -170.f, 5600.f);
	addEnemy(Aircraft::Type::Avenger, 0.f, 5600.f);
	addEnemy(Aircraft::Type::Avenger, 170.f, 5600.f);

	std::sort(enemySpawnPoints.begin(), enemySpawnPoints.end(),
		[](SpawnPoint lhs,SpawnPoint rhs) {
			return lhs.y < rhs.y;
		});
}

void World::addEnemy(Aircraft::Type type, float relX, float relY)
{
	SpawnPoint spawn(type, spawnPosition.x+relX, spawnPosition.y-relY);
	enemySpawnPoints.push_back(spawn);
}

void World::spawnEnemies()
{
	while (!enemySpawnPoints.empty() &&
		enemySpawnPoints.back().y > getBattlefieldBounds().top)
	{
		auto& spawn = enemySpawnPoints.back();

		std::unique_ptr<Aircraft> enemy(new Aircraft(spawn.type, textures, fonts));
		enemy->setPosition(spawn.x, spawn.y);
		enemy->setRotation(180.f);
		sceneLayers[UpperAir]->attachChild(std::move(enemy));
		// Enemy is spawned, remove from the list to spawn
		enemySpawnPoints.pop_back();
	}
}

void World::adaptPlayerVelocity()
{
	sf::Vector2f velocity = playerAircraft->getVelocity();
	// If moving diagonally, normalize the velocity
	if (velocity.x != 0.f && velocity.y != 0.f)
		playerAircraft->setVelocity(velocity / std::sqrt(2.f));
	// Add scrolling velocity
	playerAircraft->accelerate(0.f, scrollSpeed);
}

void World::adaptPlayerPosition()
{
	// Keep player's position inside the screen bounds, at least borderDistance units from the border
	sf::FloatRect viewBounds = getViewBounds();
	const float borderDistance = 40.f;

	sf::Vector2f position = playerAircraft->getPosition();
	position.x = std::max(position.x, viewBounds.left + borderDistance);
	position.x = std::min(position.x, viewBounds.left + viewBounds.width - borderDistance);
	position.y = std::max(position.y, viewBounds.top + borderDistance);
	position.y = std::min(position.y, viewBounds.top + viewBounds.height - borderDistance);
	playerAircraft->setPosition(position);
}

void World::updateSounds()
{
	sounds.setListenerPosition(playerAircraft->getWorldPosition());
	sounds.removeStoppedSounds();
}

sf::FloatRect World::getViewBounds() const
{
	return sf::FloatRect(worldView.getCenter() - worldView.getSize() / 2.f,worldView.getSize());
}

sf::FloatRect World::getBattlefieldBounds() const
{
	auto bounds = getViewBounds();
	bounds.top -= 100.f;
	bounds.height += 100.f;
	return bounds;
}

void World::destroyEntitiesOutsideView()
{
	Command command;
	command.category = Category::Projectile | Category::EnemyAircraft;
	command.action = derivedAction<Entity>([this](Entity& e, sf::Time)
		{
			if (!getBattlefieldBounds().intersects(e.getBoundingRect()))
				e.destroy();
		});

	commandQueue.push(command);
}

void World::guideMissiles()
{
	Command enemyCollector;
	enemyCollector.category = Category::EnemyAircraft;
	enemyCollector.action = derivedAction<Aircraft>(
		[this](Aircraft& enemy, sf::Time)
		{
			if (!enemy.isDestroyed())
				activeEnemies.push_back(&enemy);
		});
	Command missileGuider;
	missileGuider.category = Category::AlliedProjectile;
	missileGuider.action = derivedAction<Projectile>(
		[this](Projectile& missile, sf::Time)
		{
			// Ignore unguided bullets
			if (!missile.isGuided())
				return;

			float minDistance = std::numeric_limits<float>::max();
			Aircraft* closestEnemy = nullptr;

			for(Aircraft * enemy : activeEnemies)
			{
				float enemyDistance = calculateDistance(missile, *enemy);

				if (enemyDistance < minDistance)
				{
					closestEnemy = enemy;
					minDistance = enemyDistance;
				}
			}
			if (closestEnemy)
				missile.guideTowards(closestEnemy->getWorldPosition());
			});
	commandQueue.push(enemyCollector);
	commandQueue.push(missileGuider);
	activeEnemies.clear();
}

bool World::matchesCategories(SceneNode::Pair& colliders, Category::Type type1, Category::Type type2)
{
	unsigned int category1 = colliders.first->getCategory();
	unsigned int category2 = colliders.second->getCategory();

	if (type1 & category1 && type2 & category2)
	{
		return true;
	}
	else if (type1 & category2 && type2 & category1)
	{
		std::swap(colliders.first, colliders.second);
		return true;
	}
	else
	{
		return false;
	}
}

void World::handleCollisions()
{
	std::set<SceneNode::Pair> collisionPairs;
	sceneGraph.checkSceneCollision(sceneGraph, collisionPairs);

	for(SceneNode::Pair pair : collisionPairs)
	{
		if (matchesCategories(pair, Category::PlayerAircraft, Category::EnemyAircraft))
		{
			auto& player = static_cast<Aircraft&>(*pair.first);
			auto& enemy = static_cast<Aircraft&>(*pair.second);

			// Collision: Player damage = enemy's remaining HP
			player.damage(enemy.getHitpoints());
			enemy.destroy();
		}
		else if (matchesCategories(pair, Category::PlayerAircraft, Category::Pickup))
		{
			auto& player = static_cast<Aircraft&>(*pair.first);
			auto& pickup = static_cast<Pickup&>(*pair.second);

			// Apply pickup effect to player, destroy projectile
			pickup.apply(player);
			pickup.destroy();
			player.playLocalSound(commandQueue, EffectID::CollectPickup);
		}

		else if (matchesCategories(pair, Category::EnemyAircraft, Category::AlliedProjectile)
			|| matchesCategories(pair, Category::PlayerAircraft, Category::EnemyProjectile))
		{
			auto& aircraft = static_cast<Aircraft&>(*pair.first);
			auto& projectile = static_cast<Projectile&>(*pair.second);

			// Apply projectile damage to aircraft, destroy projectile
			aircraft.damage(projectile.getDamage());
			projectile.destroy();
		}
	}
}




