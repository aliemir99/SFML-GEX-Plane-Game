#pragma once


#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>

#include "StateIdentifiers.h"
#include "ResourceIdentifier.h"
#include <memory>

namespace sf {
	class RenderWindow;

}
class StateStack;
class Player;
class MusicPlayer;
class SoundPlayer;

class State
{
public:
	using Ptr = std::unique_ptr<State>;

	struct Context 
	{
		Context(sf::RenderWindow& window,
			TextureHolder_t& textures,
			FontHolder_t& fonts,
			Player& player,
			MusicPlayer& music,
			SoundPlayer& sounds);

		sf::RenderWindow*	window;
		TextureHolder_t*	textures;
		FontHolder_t*		fonts;
		Player*				player;
		MusicPlayer*		music;
		SoundPlayer*		sounds;
	};

							State(StateStack& stack, Context context);
	virtual					~State();

	virtual void			draw() = 0;
	virtual bool			update(sf::Time dt) = 0;
	virtual bool			handleEvents(const sf::Event& event) = 0;

protected:
	void					requestStackPush(StateID stateID);
	void					requestStackPop();
	void					requestStackClear();

	Context					getContext();

private:
	StateStack*				stack;
	Context					context;
};