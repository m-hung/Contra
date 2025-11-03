<<<<<<< Updated upstream
#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow window(sf::VideoMode({ 200, 200 }), "SFML works!");
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear();
        window.draw(shape);
        window.display();
    }
=======
﻿#include <SFML/Graphics.hpp>
#include "Player.h"
#include"Game.h"
int main() {
    Game game;
    game.Run();

    //sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "ADWS Demo");
    //window.setFramerateLimit(60);
    //Player player;
    //sf::Clock clock;

    //while (window.isOpen()) {
    //    sf::Time elapsed = clock.restart();
    //    float dt = elapsed.asSeconds();

    //  
    //    while (auto event = window.pollEvent()) {
    //        if (event->is<sf::Event::Closed>())
    //            window.close();
    //    }

    //    player.HandleInput(dt);
    //    player.Update(dt);

    //    window.clear(sf::Color(20, 20, 50));
    //    player.Draw(window);
    //    window.display();
    //}1
    return 0;
>>>>>>> Stashed changes
}