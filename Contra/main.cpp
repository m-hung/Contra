#include <SFML/Graphics.hpp>
#include "Player.h"
#include "Game.h"
#include "Menu.h"

int main() {
    // Tạo cửa sổ dùng chung cho cả Menu và Game
    sf::RenderWindow window(sf::VideoMode({ 1280, 720 }), "Contra");
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
        Game game(&window);  // game tạo cửa sổ riêng trong constructor
        game.Run();
    }

    return 0;
}
