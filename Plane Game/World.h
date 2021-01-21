#pragma once

#include "ResourceHolder.h"
#include "ResourceIdentifier.h"
#include "SceneNode.h"
#include "SpriteNode.h"
#include "Aircraft.h"
#include "CommandQueue.h"
#include "Command.h"
#include "BloomEffect.h"
#include "SoundPlayer.h"

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include <array>

// Forward declaration
namespace sf
{
	class RenderWindow;
	class RenderTarget;
}
class World : private sf::NonCopyable
{
public:
	explicit							World(sf::RenderTarget& outputTarget,FontHolder_t& fonts, SoundPlayer& sounds);
	void								update(sf::Time dt);
	void								draw();

	CommandQueue&						getCommands();

	bool								hasAlivePlayer() const;
	bool								hasPlayerReachedEnd() const;

private:
	void								loadTextures();
	void								buildScene();

	void								addEnemies();
	void								addEnemy(Aircraft::Type type, float relX, float relY);
	void								spawnEnemies();

	void								adaptPlayerVelocity();
	void								adaptPlayerPosition();

	void								updateSounds();

	sf::FloatRect						getViewBounds() const;
	sf::FloatRect						getBattlefieldBounds() const;

	void								destroyEntitiesOutsideView();
	void								guideMissiles();
	void								handleCollisions();
	bool								matchesCategories(SceneNode::Pair& colliders, Category::Type type1, Category::Type type2);

private:
	enum Layer
	{
		Background,
		LowerAir,
		UpperAir,
		LayerCount
	};

	struct SpawnPoint {
		SpawnPoint(Aircraft::Type type, float x, float y)
			:type(type), x(x), y(y) {}
		Aircraft::Type type;
		float x, y;
	};

private:
	sf::RenderTarget&					target;
	sf::RenderTexture					sceneTexture;
	sf::View							worldView;
	TextureHolder_t						textures;
	const FontHolder_t&					fonts;
	SoundPlayer&						sounds;

	SceneNode							sceneGraph;
	std::array<SceneNode*, LayerCount>	sceneLayers;
	CommandQueue						commandQueue;

	sf::FloatRect						worldBounds;
	sf::Vector2f						spawnPosition;

	float								scrollSpeed;
	Aircraft*							playerAircraft;

	std::vector<SpawnPoint>				enemySpawnPoints;
	std::vector<Aircraft*>				activeEnemies;
	
	BloomEffect							bloomEffect;
};

