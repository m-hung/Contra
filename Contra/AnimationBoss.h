#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <map>
#include <iostream>

class AnimationBoss {
public:
    // Constructor nh?n tham chi?u ð?n Sprite c?a Boss
    AnimationBoss(sf::Sprite& sprite);

    // Thêm animation m?i, bao g?m tham s? startY ð? xác ð?nh hàng frame
    void AddAnimation(const std::string& name, const sf::Texture* texture,
        int frameCount, sf::Vector2i frameSize, float frameDuration, int startY);

    // B?t ð?u/Chuy?n animation
    void Play(const std::string& name);

    // C?p nh?t frame
    void Update(float dt);

    // V? Sprite (dù Draw thý?ng n?m trong Boss, nhýng gi? l?i cho ti?n)
    void Draw(sf::RenderWindow& window);

private:
    struct AnimationData {
        const sf::Texture* texture;
        int frameCount;
        sf::Vector2i frameSize;
        float frameDuration;
        int startY; // T?a ð? Y b?t ð?u c?a hàng frame trong spritesheet
    };

    sf::Sprite& m_sprite;
    std::map<std::string, AnimationData> m_animations;
    std::string m_currentName;
    int m_currentFrame;
    float m_timer;
};
