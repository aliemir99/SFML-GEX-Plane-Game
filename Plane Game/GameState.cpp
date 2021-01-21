#include "GameState.h"
#include "MusicPlayer.h"
#include <SFML/Graphics/RenderWindow.hpp>
GameState::GameState(StateStack& stack, Context context)
	:State(stack,context)
	,world(*context.window,*context.fonts, *context.sounds)
	,player(*context.player)
{

	context.music->play(MusicID::MissionTheme);
}

void GameState::draw()
{
	world.draw();

}

bool GameState::update(sf::Time dt)
{
	world.update(dt);
	if (!world.hasAlivePlayer()) {
		player.setMissionStatus(Player::MissionStatus::Failure);
		requestStackPush(StateID::GameOver);
	}
	else if (world.hasPlayerReachedEnd()) {
		player.setMissionStatus(Player::MissionStatus::Success);
		requestStackPush(StateID::GameOver);
	}
	CommandQueue& commands = world.getCommands();
	player.handleRealTimeInput(commands);

	return true;
}

bool GameState::handleEvents(const sf::Event& event)
{
	CommandQueue& commands = world.getCommands();
	player.handleEvent(event, commands);

	// Escape pressed, trigger the pause screen
	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
		requestStackPush(StateID::Pause);
	//Q pressed, trigger the menu state
	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Q)
		requestStackPush(StateID::Menu);
	//G pressed, trigger the gex state
	//if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::G)
		//requestStackPush(StateID::Gex);

	return false;
}
