#include "Animation.h"
#include <iostream>
#include<vector>
// Sửa 1: Thay đổi constructor
Animation::Animation(sf::Sprite& sprite)
    : m_sprite(sprite), // Khởi tạo m_sprite
    m_currentFrame(0),
    m_timer(0.f)
{
}

void Animation::AddAnimation(const std::string& name, const sf::Texture* texture,
    int frameCount, sf::Vector2i frameSize, float frameDuration)
{
    m_animations[name] = { texture, frameCount, frameSize, frameDuration };
}

void Animation::Play(const std::string& name)
{
    if (m_currentName == name) return;
    auto it = m_animations.find(name);
    if (it == m_animations.end()) {
        std::cerr << "[Animation] Khong tim thay animation: " << name << std::endl;
        return;
    }

    m_currentName = name;
    m_currentFrame = 0;
    m_timer = 0.f;

    m_sprite.setTexture(*it->second.texture);

    // Sửa 2: Dùng dấu ngoặc nhọn {}
    m_sprite.setTextureRect({ { 0, 0 }, {it->second.frameSize.x, it->second.frameSize.y } });
}

void Animation::Update(float dt)
{
    if (m_currentName.empty()) return;

    auto& anim = m_animations[m_currentName];
    m_timer += dt;
    if (m_timer >= anim.frameDuration) {
        m_timer = 0.f;
        m_currentFrame = (m_currentFrame + 1) % anim.frameCount;
    }

    // Cập nhật frame hiện tại
    m_sprite.setTextureRect({ { m_currentFrame * anim.frameSize.x, 0},
                             { anim.frameSize.x, anim.frameSize.y } });

    // ✅ Thêm đoạn này để đặt origin giữa frame (điểm đối xứng)
     auto bounds = m_sprite.getLocalBounds();
     m_sprite.setOrigin({ bounds.position.x + bounds.size.x / 2.f,
         bounds.position.y + bounds.size.y / 2.f });
}


void Animation::Draw(sf::RenderWindow& window)
{
    window.draw(m_sprite);
}