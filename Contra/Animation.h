#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>

class Animation {
private:
    struct AnimationData {
        const sf::Texture* texture = nullptr; // Khởi tạo con trỏ là nullptr
        int frameCount = 0;                   // Khởi tạo số là 0
        sf::Vector2i frameSize = { 0, 0 };      // Khởi tạo vector là {0, 0}
        float frameDuration = 0.0f;           // Khởi tạo float là 0.0f
        bool loop = false;
    };

    std::unordered_map<std::string, AnimationData> m_animations;
    std::string m_currentName;
    int m_currentFrame;
    float m_timer;
    sf::Sprite& m_sprite;
    

public:
    Animation(sf::Sprite& sprite);

    void AddAnimation(const std::string& name, const sf::Texture* texture,
        int frameCount, sf::Vector2i frameSize, float frameDuration, bool loop = true);

    void Play(const std::string& name);
    void Update(float dt);
    void Draw(sf::RenderWindow& window);

    sf::Sprite& GetSprite() { return m_sprite; }
    const sf::Sprite& GetSprite() const { return m_sprite; }

    bool IsFinished() const;

};

