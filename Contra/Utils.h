#pragma once
#include <SFML/Graphics.hpp>

// Hàm kiểm tra va chạm giữa 2 hình chữ nhật (Rect) - tương thích SFML 3.0
inline bool RectIntersects(const sf::Rect<float>& a, const sf::Rect<float>& b)
{
    return (a.position.x < b.position.x + b.size.x &&
        a.position.x + a.size.x > b.position.x &&
        a.position.y < b.position.y + b.size.y &&
        a.position.y + a.size.y > b.position.y);
}
