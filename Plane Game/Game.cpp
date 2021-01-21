#include "Game.h"
#include <cfloat>
#include"SFML/Graphics.hpp"


static const sf::Time TimePerFrame = sf::seconds(1.f / 60.f);


sf::Vector2f normalize(sf::Vector2f v) {
    float size = sqrt(v.x * v.x + v.y* v.y);
    if (size > FLT_MIN)
        v = v / size;
    return v;
}


Game::Game()
    : window(sf::VideoMode(1280, 720), "SFML works!")
    ,world(window)
    ,player()
    ,font()
    ,statsText()
    ,statsUpdateTime()
    ,statsNumFrames(0)

{
    
    //HUD Stats
    font.loadFromFile("Media/Sansation.ttf");
    statsText.setFont(font);
    statsText.setPosition(5.f, 5.f);
    statsText.setCharacterSize(20);

}


void Game::run()
{
    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;
    while (window.isOpen()) {
        
        sf::Time elapsedTime = clock.restart();
        timeSinceLastUpdate += elapsedTime;

        while(timeSinceLastUpdate > TimePerFrame){
            timeSinceLastUpdate -= TimePerFrame;
            processInput();
            update(TimePerFrame);
        
        }
        updateStatistics(elapsedTime);
        render();
    }
}

void Game::processInput()
{
    CommandQueue& commands = world.getCommands();
    sf::Event event;
    while (window.pollEvent(event))
    {
        player.handleEvent(event, commands);
        if (event.type == sf::Event::Closed) {
            window.close();
        }
    }
    player.handleRealTimeInput(commands);
}

void Game::update(sf::Time dt)
{
    world.update(dt);
}

void Game::render()
{
    window.clear();
    world.draw();

    window.setView(window.getDefaultView());

    window.draw(statsText);
    window.display();
}


void Game::updateStatistics(sf::Time et)
{
    statsUpdateTime += et;
    statsNumFrames += 1;

    if (statsUpdateTime >= sf::seconds(1.0f)) {
    
        statsText.setString(
            "Frames/ Second = " + std::to_string(statsNumFrames) + "\n" +
            "Time/ Update = " + std::to_string(statsUpdateTime.asMicroseconds() / statsNumFrames) + "us"
        );
        statsUpdateTime -= sf::seconds(1.0f);
        statsNumFrames = 0;
    }
}
