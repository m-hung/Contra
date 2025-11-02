#pragma once
#include <SFML/Graphics.hpp>

class IEnemy {
public:
	virtual ~IEnemy() {}
	virtual void Update(float dt, sf::Vector2f playerPos) = 0; // Cap nhat di chuyen, tan cong
	virtual void Draw(sf::RenderWindow& window) = 0;           // Ve ke dich
	virtual sf::FloatRect GetBounds() const = 0;               // Lay vung va cham
	virtual void TakeDamage(int damage) = 0;                   // Nhan sat thuong
	virtual bool IsDead() const = 0;                           // Kiem tra chet
};
