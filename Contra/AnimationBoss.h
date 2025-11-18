#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <map>
#include <iostream>

class AnimationBoss {
public:
    // Constructor nh?n tham chi?u đ?n Sprite c?a Boss
    AnimationBoss(sf::Sprite& sprite);

    // Thêm animation m?i, bao g?m tham s? startY đ? xác đ?nh hàng frame
    void AddAnimation(const std::string& name, const sf::Texture* texture,
        int frameCount, sf::Vector2i frameSize, float frameDuration, int startY);

    // B?t đ?u/Chuy?n animation
    void Play(const std::string& name);

    // C?p nh?t frame
    void Update(float dt);

    // V? Sprite (dù Draw thư?ng n?m trong Boss, nhưng gi? l?i cho ti?n)
    void Draw(sf::RenderWindow& window);

private:
    struct AnimationData {
        const sf::Texture* texture = nullptr;;
        int frameCount;
        sf::Vector2i frameSize;
        float frameDuration;
        int startY; // T?a đ? Y b?t đ?u c?a hàng frame trong spritesheet
    };

    sf::Sprite& m_sprite;
    std::map<std::string, AnimationData> m_animations;
    std::string m_currentName;
    int m_currentFrame;
    float m_timer;

    static constexpr float ATTACK_FRAME_0_DURATION = 0.15f; // Thời gian chuẩn bị vung đánh
};
