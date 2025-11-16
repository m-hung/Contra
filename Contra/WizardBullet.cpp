#include "WizardBullet.h"
#include "AssetManeger.h" // Phải include AssetManeger
#include <cmath>

// Cần 1 texture tạm
static sf::Texture dummyWizardBulletTexture;

// Hàm helper để chuẩn hóa (lấy từ file EnemyBullet cũ của bạn)
static sf::Vector2f normalize(sf::Vector2f v) {
    float length = std::sqrt(v.x * v.x + v.y * v.y);
    if (length != 0) {
        return sf::Vector2f(v.x / length, v.y / length);
    }
    return sf::Vector2f(0.f, 0.f);
}

WizardBullet::WizardBullet(sf::Vector2f startPos, sf::Vector2f direction, float speed)
    : m_sprite(dummyWizardBulletTexture),
    m_animation(m_sprite),
    m_position(startPos),
    m_velocity(normalize(direction)* speed),
    m_lifetime(2.0f), // Đạn sẽ tự biến mất sau 2 giây (bạn có thể đổi số này)
    m_secondAnimName("Trail"), // Tên cho animation thứ hai
    m_animTransitionTimer(0.4f) // Chuyển sau 0.4 giây (thời gian chạy của 4 frame đầu tiên)
    // --- KẾT THÚC KHỞI TẠO ---
{
    // --- THIẾT LẬP ANIMATION 4-FRAME ---
    auto& asset = AssetManeger::getInstance();

    // ===========================================
    // === THAY CÁC THÔNG SỐ NÀY CHO ĐÚNG ===

    m_frameSize = { 50, 50 };        // Kích thước 1 frame đạn (ví dụ 32x32)
    int frameCount = 4;                 // Số frame (bạn nói là 4)
    float frameDuration = 0.12f;         // Tốc độ animation (ví dụ 0.1 giây/frame)
    m_scale = 1.5f;                     // Phóng to đạn (ví dụ 1.5x)

    // Tên file texture 4-frame của bạn
    const char* textureName = "Wizard2_skill.png";

    // ===========================================

    const sf::Texture* bulletTex = &asset.getTexture(textureName);

    m_animation.AddAnimation("Fly", bulletTex, frameCount, m_frameSize, frameDuration);
    m_animation.Play("Fly");
    // === THÔNG SỐ ANIMATION THỨ HAI (7 frame) ===
    int frameCount2 = 7;
    float frameDuration2 = 0.08f; // Tốc độ nhanh hơn (ví dụ)
    const char* textureName2 = "Wizard2_skill2.png"; // <-- Tên file 7-frame THỨ HAI
    const sf::Texture* bulletTex2 = &asset.getTexture(textureName2);

    // Thêm animation "Trail" (lặp lại)
    m_animation.AddAnimation(m_secondAnimName, bulletTex2, frameCount2, m_frameSize, frameDuration2);

    // Bắt đầu với animation đầu tiên
    m_animation.Play("Start");
    // Đặt Origin vào giữa frame
    m_sprite.setOrigin({ m_frameSize.x / 2.f, m_frameSize.y / 2.f });

    // Lật sprite nếu đạn bay sang trái
    float scaleX = (direction.x < 0) ? -m_scale : m_scale;
    m_sprite.setScale({ scaleX, m_scale });

    m_sprite.setPosition(m_position);
}

void WizardBullet::Update(float dt)
{
    if (IsDead()) return; // Không cập nhật nếu đã chết

    m_lifetime -= dt;
    m_position += m_velocity * dt; // Cập nhật vị trí World
    m_animation.Update(dt);
    if (!m_secondAnimName.empty() && m_animTransitionTimer > 0.0f)
    {
        m_animTransitionTimer -= dt;
        if (m_animTransitionTimer <= 0.0f)
        {
            // Khi timer hết, chuyển sang animation thứ hai
            m_animation.Play(m_secondAnimName);
            m_secondAnimName.clear(); // Xóa tên để không chạy lại logic này
        }
    }
    // --- KẾT THÚC LOGIC CHUYỂN ANIMATION ---
}

void WizardBullet::Draw(sf::RenderWindow& window, float scrollOffset)
{
    if (IsDead()) return;

    // Tính vị trí Screen (để vẽ)
    sf::Vector2f screenPos = m_position - sf::Vector2f(scrollOffset, 0.f);
    m_sprite.setPosition(screenPos);

    // Vẽ sprite tại vị trí Screen
    m_animation.Draw(window);

    // Trả sprite về vị trí World (cho logic Update lần sau)
    m_sprite.setPosition(m_position);
}

sf::FloatRect WizardBullet::GetBounds() const
{
    // Tính hitbox thủ công dựa trên vị trí World
    float w = m_frameSize.x * m_scale;
    float h = m_frameSize.y * m_scale;

    return sf::FloatRect(
        sf::Vector2f(m_position.x - w / 2.f, m_position.y - h / 2.f), // position
        sf::Vector2f(w, h)                                            // size
    );
}