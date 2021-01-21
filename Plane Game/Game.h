#pragma once
#include"SFML/Graphics.hpp"
#include "ResourceHolder.h"
#include "ResourceIdentifier.h"
#include"World.h"
#include "Player.h"
class Game
{
public:
						Game();
	void				run();

private:
	void				processInput();
	void				update(sf::Time dt);
	void				render();


	void				updateStatistics(sf::Time et);
private:
	sf::RenderWindow	window;
	World				world;
	Player				player;


	sf::Font			font;
	sf::Text			statsText;
	sf::Time			statsUpdateTime;
	std::size_t			statsNumFrames;

};

