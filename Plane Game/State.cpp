#include "State.h"
#include "StateStack.h"

State::State(StateStack& stack, Context context)
	:stack(&stack)
	,context(context)
{
}

State::~State()
{
}

void State::requestStackPush(StateID stateID)
{
	stack->pushState(stateID);
}

void State::requestStackPop()
{
	stack->popState();
}

void State::requestStackClear()
{
	stack->clearStates();
}

State::Context State::getContext()
{
	return context;
}

State::Context::Context(sf::RenderWindow& window, TextureHolder_t& textures, FontHolder_t& fonts, Player& player, MusicPlayer& music, SoundPlayer& sounds)
	:window(&window)
	,textures(&textures)
	,fonts(&fonts)
	,player(&player)
	,music(&music)
	,sounds(&sounds)
{
}
