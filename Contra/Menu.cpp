#include "Menu.h"
#include <iostream>
#include <stdexcept>

//--------------------------------------------------------------
// Constructor: Khởi tạo menu và các thành phần hiển thị
// Lưu ý: m_title phải được khởi tạo trong initializer list vì
// sf::Text không còn constructor mặc định trong SFML 3.x
//--------------------------------------------------------------
Menu::Menu(sf::RenderWindow* window)
    : m_window(window)
    , m_selectedIndex(0)
    // khởi tạo m_title bằng m_font, chuỗi rỗng và kích thước 0
    // (m_font đã được mặc định khởi tạo trước theo thứ tự khai báo trong Menu.h)
    , m_title(m_font, "", 0)
{
    // Mở font (SFML 3: openFromFile). Nếu thất bại, báo lỗi.
    if (!m_font.openFromFile("Quantico-Bold.ttf"))
    {
        std::cerr << "Không thể tải font: Quantico-Bold.ttf\n";
        // Nếu muốn dừng hẳn: throw std::runtime_error("Font load failed");
    }

    // Thiết lập tiêu đề: bây giờ m_title đã hợp lệ và ta có thể set giá trị
    m_title.setFont(m_font);               // an toàn (font đã tồn tại)
    m_title.setString("CONTRA");
    m_title.setCharacterSize(64);
    m_title.setFillColor(sf::Color::Yellow);
    m_title.setStyle(sf::Text::Bold);
    m_title.setPosition({ 200.f, 100.f });

    // Danh sách lựa chọn (khởi tạo sf::Text bằng font để tránh lỗi)
    std::vector<std::string> labels = { "Start Game", "Exit" };
    for (std::size_t i = 0; i < labels.size(); ++i)
    {
        // SFML 3: sf::Text không có default ctor -> dùng ctor nhận font, string và size
        sf::Text text(m_font, labels[i], 36); // (font, string, characterSize)
        text.setFillColor(i == 0 ? sf::Color::Red : sf::Color::White);
        text.setPosition({ 250.f, 300.f + static_cast<float>(i) * 60.f });
        m_options.push_back(std::move(text));
    }
}

//--------------------------------------------------------------
// HandleInput: Xử lý sự kiện theo SFML 3 (pollEvent trả về std::optional)
//--------------------------------------------------------------
void Menu::HandleInput()
{
    // SFML 3: pollEvent() trả về std::optional<sf::Event>.
    while (auto event = m_window->pollEvent())
    {
        // Đóng cửa sổ
        if (event->is<sf::Event::Closed>())
        {
            m_window->close();
            continue;
        }

        // Nếu là phím nhấn
        if (auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
        {
            using Sc = sf::Keyboard::Scancode;

            if (keyPressed->scancode == Sc::Down)
            {
                m_selectedIndex = (m_selectedIndex + 1) % static_cast<int>(m_options.size());
            }
            else if (keyPressed->scancode == Sc::Up)
            {
                m_selectedIndex = (m_selectedIndex - 1 + static_cast<int>(m_options.size())) % static_cast<int>(m_options.size());
            }
            else if (keyPressed->scancode == Sc::Enter || keyPressed->code == sf::Keyboard::Key::Enter)
            {
                // xử lý khi nhấn Enter (ví dụ: nếu chọn Exit thì đóng)
                if (m_selectedIndex == 1) // Exit
                    m_window->close();
                // nếu Start Game thì bạn có thể đặt cờ để chuyển scene...
            }
        }
    }

    // Cập nhật màu menu: mục đang chọn = đỏ, còn lại trắng
    for (std::size_t i = 0; i < m_options.size(); ++i)
    {
        m_options[i].setFillColor(i == static_cast<std::size_t>(m_selectedIndex)
            ? sf::Color::Red
            : sf::Color::White);
    }
}

//--------------------------------------------------------------
// Draw: Vẽ menu
//--------------------------------------------------------------
void Menu::Draw()
{
    m_window->clear(sf::Color::Black);
    m_window->draw(m_title);
    for (auto& text : m_options)
        m_window->draw(text);
    m_window->display();
}

int Menu::GetSelectedOption() const
{
    return m_selectedIndex;
}
