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

                // >>> THÊM ĐOẠN NÀY ĐỂ XÓA BỎ BỘ ĐỆM PHÍM ENTER <<<
                while (window.pollEvent()) {
                    // Chỉ cần lấy các sự kiện ra khỏi hàng đợi
                    // HÀNH ĐỘNG NÀY RẤT QUAN TRỌNG
                }
            }
            else if (selectedOption == 1) { // Exit
                window.close();
                return 0;
            }
        }
    }

    // --- GIAI ĐOẠN 2: Chạy GAME ---
    //if (window.isOpen()) {
    //    // Bắt đầu nhạc gameplay
    //    sf::Music gameMusic;
    //    if (!gameMusic.openFromFile("music_gamePlayer.mp3")) {
    //        std::cerr << "khong the tai nhac gameplay!\n";
    //    }
    //    else {
    //        gameMusic.setLooping(true);
    //        gameMusic.setVolume(50.f);
    //        gameMusic.play();
    //    }

    //    // Chạy game
    //    Game game(&window);
    //    game.Run();
    //}

    // Trong main.cpp (GIAI ĐOẠN 2: Chạy GAME)
    // main.cpp - GIAI ĐOẠN 2: Chạy GAME
    if (window.isOpen()) {
        // 1. Tải nhạc (giữ nguyên logic của bạn)
        /*sf::Music gameMusic;
        if (!gameMusic.openFromFile("music_gamePlayer.mp3")) {
            std::cerr << "khong the tai nhac gameplay!\n";
        }
        else {
            gameMusic.setLooping(true);
            gameMusic.setVolume(50.f);
            gameMusic.play();
        }*/

        /*Game game(&window);
        game.Run();*/



        // 2. Vòng lặp chính để chạy/restart game
        bool running = true;
        while (running && window.isOpen()) {

            // 🟢 TẠO ĐỐI TƯỢNG GAME MỚI (để reset game)
            Game game(&window);

            // 🟢 CHẠY VÒNG LẶP GAME BÊN TRONG HÀM Run()
            // Hàm Run() sẽ lặp cho đến khi m_isRunning = false (WIN/LOSE + Enter)
            game.Run();

            //std::cout << "Game restarted.\n";
            
            // 3. KIỂM TRA ĐIỀU KIỆN KẾT THÚC CỦA VÒNG LẶP Run()
            if (game.WasRestartRequested()) {
                // Nếu yêu cầu restart, vòng lặp `while (running...)` sẽ lặp lại 
                // và tạo đối tượng `Game` mới.
                std::cout << "Game restarted.\n";
                continue;
            }
            else {
                // Nếu m_isRunning = false KHÔNG phải do restart 
                // (ví dụ: người chơi nhấn đóng cửa sổ trong khi game đang chạy),
                // thì thoát khỏi vòng lặp và kết thúc chương trình.
                running = false;
            }
        }
    }
    return 0;
}
