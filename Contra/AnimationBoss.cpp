#include "AnimationBoss.h"
#include <iostream>

AnimationBoss::AnimationBoss(sf::Sprite& sprite)
    : m_sprite(sprite),
    m_currentFrame(0),
    m_timer(0.f)
{
}

void AnimationBoss::AddAnimation(const std::string& name, const sf::Texture* texture,
    int frameCount, sf::Vector2i frameSize, float frameDuration, int startY)
{
    m_animations[name] = { texture, frameCount, frameSize, frameDuration, startY };
}

void AnimationBoss::Play(const std::string& name)
{
    if (m_currentName == name) return;
    auto it = m_animations.find(name);
    if (it == m_animations.end()) {
        std::cerr << "[AnimationBoss] Khong tim thay animation: " << name << std::endl;
        return;
    }

    m_currentName = name;
    m_currentFrame = 0;
    m_timer = 0.f;

    m_sprite.setTexture(*it->second.texture);

    // C?p nh?t frame ban ð?u
    const auto& anim = it->second;
    m_sprite.setTextureRect({ { 0, anim.startY },
                             { anim.frameSize.x, anim.frameSize.y } });

    // Ð?t Origin v? gi?a frame (ð? l?t sprite ðúng)
    auto bounds = m_sprite.getLocalBounds();
    m_sprite.setOrigin({ bounds.position.x + bounds.size.x / 2.f,
                         bounds.position.y + bounds.size.y / 2.f });
}

void AnimationBoss::Update(float dt)
{
    if (m_currentName.empty()) return;

    auto& anim = m_animations[m_currentName];
    m_timer += dt;
    if (m_timer >= anim.frameDuration) {
        m_timer = 0.f;
        m_currentFrame = (m_currentFrame + 1) % anim.frameCount;
    }

    // C?p nh?t frame hi?n t?i, s? d?ng startY ð? ch?n hàng frame
    m_sprite.setTextureRect({ { m_currentFrame * anim.frameSize.x, anim.startY},
                             { anim.frameSize.x, anim.frameSize.y } });
}

void AnimationBoss::Draw(sf::RenderWindow& window)
{
    window.draw(m_sprite);
}