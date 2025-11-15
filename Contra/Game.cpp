#include <SFML/Graphics.hpp>
#include "Game.h"
#include <iostream>   
#include <algorithm> 
#include "SoldierEnemy.h"
#include "SpiderEnemy.h"
#include "Bullet.h"
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

Game::Game(sf::RenderWindow* window)
    : m_window(window), m_isRunning(true),
    m_spiderSpawner(sf::Vector2f(1200.0f, 50.0f), sf::Vector2f(32.0f, 32.0f), 3.0f),
    m_impactBuffer(),
    m_impactSound(m_impactBuffer)
{
    InitEnemies();

    // Danh sách file background
    std::vector<std::string> bgFiles = {
        "bg1GameDemo.png",
        "bg2GameDemo.png",
        "bg3GameDemo.png",
        "bg4GameDemo.png",
        "bg5GameDemo.png"
    };
    // ======= KHỞI TẠO VẬT THỂ =======
   /* m_objects.emplace_back("box_vatthe.png", sf::Vector2f(700.f, 560.f), sf::Vector2f(0.3f, 0.3f)); // thùng gỗ
    m_objects.emplace_back("obstacle_1.png", sf::Vector2f(600.f, 626.f), sf::Vector2f(0.2f, 0.1f)); // chuong ngai vat 2
    m_objects.emplace_back("cloud_vatthe.png", sf::Vector2f(300.f, 530.f), sf::Vector2f(0.3f, 0.3f)); // đám mây
    m_objects.emplace_back("obstacle_1.png", sf::Vector2f(230.f, 626.f), sf::Vector2f(0.2f, 0.1f)); // chuong ngai vat 1*/
    // Dự trữ dung lượng để tránh realloc (an toàn khi dùng reference)
    m_bgTextures.reserve(bgFiles.size());
    m_bgSprites.reserve(bgFiles.size());

    for (size_t i = 0; i < bgFiles.size(); ++i)
    {
        // Tạo texture tạm
       sf::Texture texture;
        if (!texture.loadFromFile(bgFiles[i]))
        {
            std::cerr << "Không thể tải " << bgFiles[i] << "\n";
            continue;
        }

        // Đưa texture vào vector (move để tránh copy nặng)
        m_bgTextures.push_back(std::move(texture));

        // Tạo sprite từ texture vừa thêm vào
        sf::Sprite sprite(m_bgTextures.back());

        // Lấy kích thước cửa sổ và ảnh
        sf::Vector2u texSize = m_bgTextures.back().getSize();
        sf::Vector2u winSize = m_window->getSize();

        // Scale vừa khung hình
        float scaleX = static_cast<float>(winSize.x) / static_cast<float>(texSize.x);
        float scaleY = static_cast<float>(winSize.y) / static_cast<float>(texSize.y);
        sprite.setScale(sf::Vector2f(scaleX, scaleY));

        // Đặt ảnh nối liền nhau
        sprite.setPosition(sf::Vector2f(
            static_cast<float>(i) * static_cast<float>(winSize.x),
            0.f
        ));

        // Lưu sprite vào danh sách
        m_bgSprites.push_back(std::move(sprite));
    }
    if (!m_impactBuffer.loadFromFile("Damage_enemy.mp3")) { // Tên file của bạn
        std::cerr << "Khong the tai am thanh Damage_enemy.mp3\n";
    }
}

void Game::Run() {
    while (m_isRunning && m_window->isOpen()) {
        float dt = m_clock.restart().asSeconds();  // thời gian giữa 2 frame

        ProcessInput(dt);  // truyền dt vào
        Update(dt);
        Render();
    }
}

void Game::ProcessInput(float dt) {
    while (auto event = m_window->pollEvent()) {
        if (event->is<sf::Event::Closed>())
            m_isRunning = false;
    }
    m_player.HandleInput(dt);
}

void Game::InitEnemies() {
    // Kẻ địch 1: Tuần tra dài
    m_enemies.push_back(std::make_unique<SoldierEnemy>(
        sf::Vector2f(800.0f, 610.0f), // Vị trí spawn
        500.0f                        // Khoảng cách tuần tra
    ));

    // Kẻ địch 2: Tuần tra ngắn
    /*
    m_enemies.push_back(std::make_unique<SoldierEnemy>(
        sf::Vector2f(800.0f, 500.0f),
        100.0f
    ));
    */
}

void Game::Update(float dt) {
    m_player.Update(dt);
    sf::Vector2f playerPos = m_player.GetPosition();

    // Tính scroll offset hiện tại (dựa vào camera/map cuộn)
    sf::Vector2f scrollOffset(m_totalScroll, 0.f);




    //-------------------SINH NHỆN TỰ ĐỘNG-------------------
    // Máy sinh sẽ tự động thêm SpiderEnemy mới vào m_enemies nếu đến lúc
    m_spiderSpawner.Update(dt, m_enemies, scrollOffset.x, playerPos);

    //--------------------------------------------------------




    float screenWidth = static_cast<float>(m_window->getSize().x);
    float halfScreen = screenWidth * 0.5f;
    float quarterScreen = screenWidth * 0.25f;
    float playerSpeed = m_player.GetSpeed();

    // Tổng chiều rộng của map (3 ảnh)
    float maxScroll = (static_cast<float>(m_bgSprites.size()) - 1.f) * screenWidth;

    // --- Trường hợp 1: Player ở giữa map, còn có thể cuộn ---
    if (m_totalScroll < maxScroll && playerPos.x > halfScreen) {
        // Giữ player ở giữa màn hình
        m_player.SetPosition(sf::Vector2f(halfScreen, playerPos.y));

        // Cuộn map sang phải (ngược hướng player)
        m_totalScroll += playerSpeed * dt;

        // Giới hạn không vượt background cuối
        if (m_totalScroll > maxScroll)
            m_totalScroll = maxScroll;
    }
    // --- Trường hợp 2: Player ở gần đầu map ---
    else if (playerPos.x < quarterScreen && m_totalScroll > 0.f) {
        m_player.SetPosition(sf::Vector2f(quarterScreen, playerPos.y));

        m_totalScroll -= playerSpeed * dt;

        if (m_totalScroll < 0.f)
            m_totalScroll = 0.f;
    }

    // --- Trường hợp 3: Player ở cuối map ---
    else if (m_totalScroll >= maxScroll) {
        // Camera dừng cuộn, player đi tự do
    }

    // Cập nhật enemy
    for (auto& enemy : m_enemies) {
        sf::Vector2f playerScreenPos = playerPos;

        // Enemy Update cần biết vị trí Player TƯƠNG ĐỐI VỚI MÀN HÌNH để định hướng/chase
        enemy->Update(dt, playerScreenPos, m_totalScroll);

        if (auto soldier = dynamic_cast<SoldierEnemy*>(enemy.get())) {
            // Khoảng cách ngang giữa Player và Enemy trên màn hình
            float deltaX = playerScreenPos.x - (soldier->GetPosition().x - m_totalScroll);

            if (auto bulletInfoOpt = soldier->TryToAttack(dt, deltaX)) {
                // Đã có yêu cầu bắn đạn, tạo đạn mới
                const SoldierBulletInfo& info = bulletInfoOpt.value();

                m_enemyBullets.push_back(std::make_unique<EnemyBullet>(
                    info.startPosition,
                    info.direction,
                    info.speed
                ));
            }
        }
    }

    for (auto& bullet : m_enemyBullets) {
        bullet->Update(dt);
    }

    CleanupDeadEnemies();
    CheckCollisions();

    // XÓA ĐẠN KẺ ĐỊCH ĐÃ CHẾT/HIT
    m_enemyBullets.erase(
        std::remove_if(m_enemyBullets.begin(), m_enemyBullets.end(),
            [](const std::unique_ptr<EnemyBullet>& b) {
                return !b->IsAlive();
            }),
        m_enemyBullets.end()
    );
}



void Game::CleanupDeadEnemies() {
    m_enemies.erase(
        std::remove_if(m_enemies.begin(), m_enemies.end(),
            [](const std::unique_ptr<IEnemy>& e) {
                return e->IsDead();
            }),
        m_enemies.end()
    );
}

void Game::Render() {
    m_window->clear();

    float screenWidth = static_cast<float>(m_window->getSize().x);
    int totalBG = static_cast<int>(m_bgSprites.size());
    if (totalBG == 0) return;

    // Tính chỉ số nền hiện tại
    int index = static_cast<int>(m_totalScroll / screenWidth);
    float offset = std::fmod(m_totalScroll, screenWidth);

    // Vẽ nền hiện tại
    m_bgSprites[index].setPosition(sf::Vector2f(-offset, 0.f));
    m_window->draw(m_bgSprites[index]);

    // Vẽ nền kế tiếp (nếu còn)
    if (index + 1 < totalBG) {
        m_bgSprites[index + 1].setPosition(sf::Vector2f(screenWidth - offset, 0.f));
        m_window->draw(m_bgSprites[index + 1]);
    }

    // --- dịch enemy theo scroll ---
    sf::Vector2f scrollOffset(m_totalScroll, 0.f);

    // Vẽ player
    m_player.Draw(*m_window);

	// Vẽ spider spawner
    m_spiderSpawner.Draw(*m_window, scrollOffset.x);


    // --- Dịch vật thể theo scroll ---
    for (auto& obj : m_objects) {
        obj.Draw(*m_window, m_totalScroll);
    }


    // Vẽ enemy theo vị trí thực trừ scroll offset
    for (auto& enemy : m_enemies) {
        sf::Vector2f worldPos = enemy->GetPosition();
        sf::Vector2f screenPos = worldPos - scrollOffset; // dịch theo map
        enemy->SetDrawPosition(screenPos);                // tạm lưu vị trí vẽ
        enemy->Draw(*m_window);
    }

    // Vẽ đạn của kẻ địch
    for (auto& bullet : m_enemyBullets) {
        if (bullet->IsAlive()) {
            bullet->Draw(*m_window, m_totalScroll);
        }
    }

    // --- Tạo một hình chữ nhật để vẽ hitbox (dùng 1 lần) ---
    static sf::RectangleShape debugHitbox;
    debugHitbox.setFillColor(sf::Color::Transparent);
    debugHitbox.setOutlineThickness(1.f);

    // --- Vẽ Hitbox của Player (Màu Đỏ) ---
    debugHitbox.setOutlineColor(sf::Color(255, 0, 0, 200)); // Màu đỏ
    sf::FloatRect playerBounds = m_player.GetBounds(); // Lấy Screen Coords

    // Vị trí vẽ chính là vị trí Screen Coords
    debugHitbox.setPosition(playerBounds.position);
    debugHitbox.setSize(playerBounds.size);
    m_window->draw(debugHitbox);


    // --- Vẽ Hitbox của Đạn Enemy (Màu Vàng) ---
    debugHitbox.setOutlineColor(sf::Color(255, 255, 0, 200)); // Màu vàng
    for (auto& bullet : m_enemyBullets) {
        if (!bullet->IsAlive()) continue;

        sf::FloatRect bulletBounds = bullet->GetBounds(); // Lấy World Coords

        // Chuyển sang Screen Coords để VẼ
        bulletBounds.position.x -= m_totalScroll;

        debugHitbox.setPosition(bulletBounds.position);
        debugHitbox.setSize(bulletBounds.size);
        m_window->draw(debugHitbox);
    }

    // --- Vẽ Hitbox của Enemies (Màu Xanh lá) ---
    debugHitbox.setOutlineColor(sf::Color(0, 255, 0, 200)); // Màu xanh lá
    for (auto& enemy : m_enemies) {
        if (enemy->IsDead()) continue;

        sf::FloatRect enemyBounds = enemy->GetBounds(); // Lấy World Coords

        // Chuyển sang Screen Coords để VẼ
        enemyBounds.position.x -= m_totalScroll;

        debugHitbox.setPosition(enemyBounds.position);
        debugHitbox.setSize(enemyBounds.size);
        m_window->draw(debugHitbox);
    }

    m_window->display();
}

void Game::CheckCollisions() {
    // Lấy tham chiếu đến danh sách đạn của Player
    auto& bullets = m_player.GetBullets();

    // Duyệt qua tất cả đạn
    for (auto bullet_it = bullets.begin(); bullet_it != bullets.end(); ) {
        bool bulletHit = false;

        // Lấy ranh giới (bounds) của viên đạn trong TỌA ĐỘ THẾ GIỚI (World Space)
        sf::FloatRect bulletBounds = bullet_it->GetBounds();

        bulletBounds.position.x += m_totalScroll;

        // Duyệt qua tất cả Quái vật
        for (auto& enemy : m_enemies) {
            if (enemy->IsDead()) continue; // Bỏ qua quái vật đã chết

            // Lấy ranh giới của Quái vật (cũng trong World Space)
            sf::FloatRect enemyBounds = enemy->GetBounds();

            // KIỂM TRA VA CHẠM
            if (enemyBounds.findIntersection(bulletBounds)) {
                // Gây sát thương
                enemy->TakeDamage(bullet_it->GetDamage());
                bulletHit = true;
                m_impactSound.play();
                break;
            }
        }

        // --- (B) KIỂM TRA ĐẠN VỚI KÉN ---
        // Nếu đạn chưa trúng quái, thì xét xem có trúng kén không
        if (!bulletHit && !m_spiderSpawner.IsDead()) {
            sf::FloatRect spawnerBounds = m_spiderSpawner.GetBounds();

            if (spawnerBounds.findIntersection(bulletBounds)) {
                m_spiderSpawner.TakeDamage(bullet_it->GetDamage());
                bulletHit = true; // Đánh dấu đạn đã trúng
            }
        }

        // Xử lý đạn: Nếu đạn trúng đích, xóa nó
        if (bulletHit) {
            // Xóa viên đạn và chuyển iterator sang phần tử tiếp theo
            bullet_it = bullets.erase(bullet_it);
        
        }
        else {
            // Đạn không trúng, chuyển sang viên đạn tiếp theo
            ++bullet_it;
        }
    }
    if (m_player.IsDead()) return; // Player chết rồi thì không cần check nữa

    // Lấy Bounds của Player (đây là Tọa độ Màn hình - Screen Coords)
    sf::FloatRect playerScreenBounds = m_player.GetBounds();

    // Chuyển Bounds của Player sang Tọa độ Thế giới (World Coords)
    sf::FloatRect playerWorldBounds = playerScreenBounds;

    playerWorldBounds.position.x += m_totalScroll;

    // Vòng lặp kiểm tra tất cả đạn của kẻ địch
    for (auto& bullet : m_enemyBullets) {
        if (!bullet->IsAlive()) continue;

        // Lấy Bounds của đạn (đây đã là Tọa độ Thế giới - World Coords)
        sf::FloatRect bulletWorldBounds = bullet->GetBounds();

        // So sánh World (Player) vs World (Bullet)
        if (playerWorldBounds.findIntersection(bulletWorldBounds)) {

            // VA CHẠM THÀNH CÔNG!
            m_player.TakeDamage(1); // Giả sử đạn địch gây 1 damage
            bullet->Hit();          // Đánh dấu đạn đã trúng để xóa ở frame sau
        }
    }

    if (m_player.IsInvincible()) {
        return;
    }

    // Lấy Player Bounds (Screen Coords)
    sf::FloatRect playerScreenBounds_Melee = m_player.GetBounds();

    // Chuyển sang World Coords
    sf::FloatRect playerWorldBounds_Melee = playerScreenBounds_Melee;
    playerWorldBounds_Melee.position.x += m_totalScroll;

    // Lặp qua tất cả kẻ địch (cả Lính và Nhện)
    for (auto& enemy : m_enemies) {
        if (enemy->IsDead()) continue;

        // Lấy Bounds của Enemy (World Coords)
        sf::FloatRect enemyBounds = enemy->GetBounds();

        // Kiểm tra va chạm Player (World) vs Enemy (World)
        if (playerWorldBounds_Melee.findIntersection(enemyBounds)) {

            // VA CHẠM VẬT LÝ!
            // Gây sát thương cho Player (Hàm TakeDamage đã có cơ chế bất tử)
            m_player.TakeDamage(1);
            break;
        }
    }
}
