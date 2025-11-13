#include <SFML/Graphics.hpp>
#include "Player.h"
#include "Game.h"
#include "Menu.h"
#include <iostream>

int main() {
    // Tạo cửa sổ dùng chung cho cả Menu và Game
    sf::RenderWindow window(sf::VideoMode({ 1280, 720 }), "Wizard's Fate");
    window.setFramerateLimit(60);

    // --- GIAI ĐOẠN 1: Hiển thị MENU ---
    Menu menu(&window);

    bool inMenu = true;
    int selectedOption = -1;

    while (inMenu && window.isOpen()) {
        menu.HandleInput();
        menu.Draw();

        selectedOption = menu.GetSelectedOption();

        // Kiểm tra nếu nhấn Enter để chọn
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter)) {
            if (selectedOption == 0) { // Start Game
                menu.StopMusic();      // Dừng nhạc nền menu
                inMenu = false;         // Thoát menu để vào game
            }
            else if (selectedOption == 1) { // Exit
                window.close();
                return 0;
            }
        }
    }

    // --- GIAI ĐOẠN 2: Chạy GAME ---
    if (window.isOpen()) {
        // Bắt đầu nhạc gameplay
        sf::Music gameMusic;
        if (!gameMusic.openFromFile("msic_gamePlayer.mp3")) {
            std::cerr << "khong the tai nhac gameplay!\n";
        }
        else {
            gameMusic.setLooping(true);
            gameMusic.setVolume(100.f);
            gameMusic.play();
        }

        // Chạy game
        Game game(&window);
        game.Run();
    }

    return 0;
}
