#include "GameObject.h"
#include "AssetManeger.h"
#include <iostream>

GameObject::GameObject(const std::string& texturePath,
    sf::Vector2f position,
    sf::Vector2f scale)
    : m_sprite(AssetManeger::getInstance().getTexture(texturePath)) // ✅ dùng đúng texturePath
{
    m_sprite.setPosition(position);
    m_sprite.setScale(scale);
}

void GameObject::Draw(sf::RenderWindow& window, float scrollOffset)
{
    sf::Vector2f worldPos = m_sprite.getPosition();
    m_sprite.setPosition({ worldPos.x - scrollOffset, worldPos.y });
    window.draw(m_sprite);
    m_sprite.setPosition(worldPos);
}

sf::FloatRect GameObject::GetBounds() const
{
    return m_sprite.getGlobalBounds();
}

void GameObject::SetScale(sf::Vector2f newScale)
{
    m_sprite.setScale(newScale);
}

void GameObject::SetPosition(sf::Vector2f newPos)
{
    m_sprite.setPosition(newPos);
}
