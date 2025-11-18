#pragma once

#include <SFML/Graphics.hpp>
#include <string>

class GameEnd {
public:
    // Enum để xác định trạng thái kết thúc game
    enum class EndState {
        NONE, // Chưa kết thúc
        WIN,  // Thắng (Boss chết)
        LOSE  // Thua (Player chết)
    };

    // Constructor: Cần tham chiếu cửa sổ để lấy kích thước
    GameEnd(sf::RenderWindow* window);

    // Thiết lập trạng thái kết thúc game (sẽ cập nhật dòng chữ và màu sắc)
    void SetEndState(EndState state);

    // Xử lý Input (kiểm tra nhấn Enter)
    void HandleInput();

    // Vẽ overlay và các dòng chữ lên màn hình
    void Draw();

    // Kiểm tra xem người dùng có muốn khởi động lại game không
    bool ShouldRestart() const;

    // Lấy trạng thái hiện tại
    EndState GetEndState() const { return m_currentState; }

private:
    sf::RenderWindow* m_window;
    EndState m_currentState;

    sf::Font m_font;
    sf::Text m_mainText;       // Dòng chữ "WIN" hoặc "LOSE"
    sf::Text m_restartText;    // Dòng chữ "Push Enter to restart game"

    sf::RectangleShape m_overlay; // Background trong suốt hơi tối

    bool m_restartRequested;

    // Hàm tiện ích để căn giữa văn bản
    void CenterText(sf::Text& text);
};