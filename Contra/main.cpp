#include <SFML/Graphics.hpp>
#include "Menu.h" // File menu của bạn
#include "Game.h" // File game của bạn
#include <memory> // Để dùng std::unique_ptr

// Tạo 2 trạng thái cho game
enum class GameState
{
    MainMenu,
    Playing
};

int main()
{
    // Tạo cửa sổ dùng chung
    sf::RenderWindow window(sf::VideoMode({ 1280, 720 }), "Contra");
    window.setFramerateLimit(60);

    // Dùng để tính delta time (dt)
    sf::Clock clock;

    // Bắt đầu game ở trạng thái MainMenu
    GameState currentState = GameState::MainMenu;

    // Khởi tạo Menu (nó sẽ tự chạy nhạc menu)
    Menu menu(&window);

    // Khởi tạo Game (dùng con trỏ để có thể tạo sau)
    std::unique_ptr<Game> game = nullptr;

    // --- VÒNG LẶP CHÍNH CỦA TOÀN BỘ CHƯƠNG TRÌNH ---
    while (window.isOpen())
    {
        // Tính thời gian cho 1 frame
        float dt = clock.restart().asSeconds();

        // --- XỬ LÝ LOGIC (UPDATE) ---
        // Logic sẽ chạy khác nhau tùy theo trạng thái
        switch (currentState)
        {
        case GameState::MainMenu:
        {
            menu.HandleInput(); // Xử lý input của Menu

            // Hỏi Menu xem người dùng đã chọn gì chưa
            Menu::MenuResult result = menu.GetMenuResult();

            if (result == Menu::MenuResult::Start)
            {
                // === CHUYỂN TRẠNG THÁI ===
                // 1. Tạo đối tượng Game mới
                //    (Hàm Game() sẽ tự bật nhạc game)
                game = std::make_unique<Game>(&window);

                // 2. Chuyển trạng thái
                currentState = GameState::Playing;
            }
            else if (result == Menu::MenuResult::Exit)
            {
                window.close();
            }
            break; // Hết case MainMenu
        }

        case GameState::Playing:
        {
            // Gọi các hàm public của Game
            game->HandleInput(dt);
            game->Update(dt);

            // (Nếu bạn muốn có cách quay lại menu, xử lý ở đây)
            // ví dụ: if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Escape))
            // {
            //    game = nullptr; // Hủy game (sẽ tự dừng nhạc game)
            //    menu.PlayMusic(); // (Bạn cần thêm hàm này)
            //    currentState = GameState::MainMenu;
            // }
            break; // Hết case Playing
        }
        }

        // --- VẼ LÊN MÀN HÌNH (DRAW) ---
        // (Luôn chạy, bất kể trạng thái nào)

        window.clear();

        // Vẽ cái gì tùy theo trạng thái
        if (currentState == GameState::MainMenu)
        {
            menu.Draw();
        }
        else if (currentState == GameState::Playing && game != nullptr)
        {
            game->Draw();
        }

        // Chỉ gọi display MỘT LẦN ở cuối vòng lặp
        window.display();
    }

    return 0;
}