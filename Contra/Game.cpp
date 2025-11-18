#include <SFML/Graphics.hpp>
#include "Game.h"
#include <iostream>   
#include <algorithm> 
#include "SoldierEnemy.h"
#include "SpiderEnemy.h"
#include "EnemyWizard.h"
#include "EnemyWizard2.h"
#include "WizardBullet.h"
#include "Bullet.h"
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <list>



Game::Game(sf::RenderWindow* window)
    : m_window(window), m_isRunning(true),
    m_spiderSpawner(sf::Vector2f(1200.0f, 50.0f), sf::Vector2f(32.0f, 32.0f), 2.5f),
    m_impactBuffer(),
    m_impactSound(m_impactBuffer),
    m_notificationTimer(0.0f), // Khởi tạo timer
    m_notificationText(m_font, "The wizard is empowered", 30),
    m_gameEnd(window)
{
    InitEnemies();

    // ======= KHỞI TẠO VĂN BẢN THÔNG BÁO =======
    if (!m_font.openFromFile("CinzelDecorative-Bold.ttf")) // Dùng font bất kỳ bạn có
    {
        std::cerr << "Khong the tai font\n";
    }

	// Khoi tạo văn bản thông báo
    m_notificationText.setFillColor(sf::Color::Yellow);


    // --- Căn giữa và đặt vị trí ---
    // (Bây giờ truy cập bằng ->)
    sf::FloatRect textRect = m_notificationText.getLocalBounds();
    m_notificationText.setOrigin({
        textRect.position.x + textRect.size.x * 0.5f,
        textRect.position.y + textRect.size.y * 0.5f
    });

    float screenWidth = static_cast<float>(m_window->getSize().x);
    float screenHeight = static_cast<float>(m_window->getSize().y);

    float centerX = screenWidth * 0.5f;
    float centerY = screenHeight * 0.5f;

    m_notificationText.setPosition({ centerX, centerY });





    // Danh sách file background
    std::vector<std::string> bgFiles = {
        "bg1GameDemo.png",
        "bg2GameDemo.png",
        "bg3GameDemo.png",
        "bg4GameDemo.png"
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

        //std::cerr << "Game dang chay\n";

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
    m_enemies.push_back(std::make_unique<SoldierEnemy>(
        sf::Vector2f(1000.0f, 610.0f), // Vị trí spawn
        500.0f                        // Khoảng cách tuần tra
    )); m_enemies.push_back(std::make_unique<SoldierEnemy>(
        sf::Vector2f(1800.0f, 610.0f), // Vị trí spawn
        500.0f                        // Khoảng cách tuần tra
    )); m_enemies.push_back(std::make_unique<SoldierEnemy>(
        sf::Vector2f(2500.0f, 610.0f), // Vị trí spawn
        500.0f                        // Khoảng cách tuần tra
    ));
    m_enemies.push_back(std::make_unique<EnemyWizard>(
        sf::Vector2f(3200.0f, 580.0f), // Vị trí spawn
        50.0f                        // Khoảng cách tuần tra
    ));
    m_enemies.push_back(std::make_unique<EnemyWizard>(
        sf::Vector2f(2100.0f, 580.0f), // Vị trí spawn
        50.0f                        // Khoảng cách tuần tra
    ));
    m_enemies.push_back(std::make_unique<EnemyWizard2>(
        sf::Vector2f(3100.0f, 610.0f), // Vị trí spawn
        50.0f                        // Khoảng cách tuần tra
    ));
    m_enemies.push_back(std::make_unique<EnemyWizard2>(
        sf::Vector2f(2000.0f, 610.0f), // Vị trí spawn
        50.0f                        // Khoảng cách tuần tra
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
    
    // code in ra tọa độ x của player
    std::cout << "Player X position: " << playerPos.x + scrollOffset.x << std::endl;

    //m_bossSpawned = false;

    //-------------------SINH NHỆN TỰ ĐỘNG-------------------
    // Máy sinh sẽ tự động thêm SpiderEnemy mới vào m_enemies nếu đến lúc
    m_spiderSpawner.Update(dt, m_enemies, scrollOffset.x, playerPos);

    for (auto& spawner : m_bossSpawners)
    {
        spawner.Update(dt, m_enemies, scrollOffset.x, playerPos);
    }
    //--------------------------------------------------------


    



	//------------------------- TẠM DỪNG KHUNG HÌNH -------------------------
    float screenWidth = static_cast<float>(m_window->getSize().x);
    float halfScreen = screenWidth * 0.5f;
    float quarterScreen = screenWidth * 0.25f;
    float playerSpeed = m_player.GetSpeed();


    // Tổng chiều rộng của map (5 ảnh)
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
    else if (playerPos.x < quarterScreen && m_totalScroll > 0.f && m_totalScroll < maxScroll) {
        m_player.SetPosition(sf::Vector2f(quarterScreen, playerPos.y));

        m_totalScroll -= playerSpeed * dt;

        if (m_totalScroll < 0.f)
            m_totalScroll = 0.f;

    }

    // --- Trường hợp 3: Đầu map – camera KHÓA và player không được đi trái ---
    else if (m_totalScroll <= 0.f) {
        float leftEdge = 50.f;

        // Chặn trái
        if (playerPos.x < leftEdge)
            m_player.SetPosition(sf::Vector2f(leftEdge, playerPos.y));

    }

    // --- Trường hợp 4: Player ở cuối map ---
    else if (m_totalScroll >= maxScroll) {
        // Camera dừng cuộn, player đi tự do
        // Giới hạn player không vượt khỏi mép phải màn hình
        float screenRight = screenWidth - 50.f;  // khoảng cách từ cạnh màn hình (tùy sprite)
        if (playerPos.x > screenRight) {
            m_player.SetPosition(sf::Vector2f(screenRight, playerPos.y));
        }

        // Chặn mép trái luôn cho an toàn
        float screenLeft = 50.f;
        if (playerPos.x < screenLeft) {
            m_player.SetPosition(sf::Vector2f(screenLeft, playerPos.y));
        }
    }

	//-----------------------------------------------------------------------


    //float screenWidth = static_cast<float>(m_window->getSize().x);


	// cường hóa wizard
    const float TARGET_X = 3700.0f;
    static bool eventTriggered = false; // Biến cờ để đảm bảo sự kiện chỉ xảy ra 1 lần

    if (playerPos.x + scrollOffset.x >= TARGET_X && !eventTriggered) {
        const float NEW_JUMP_FORCE = -1000.0f;
        const float NEW_SHOOT_DELAY = 0.25f;
        m_player.SetJumpForce(NEW_JUMP_FORCE);
        m_player.SetShootDelay(NEW_SHOOT_DELAY);

        std::cout << "CƯỜNG HÓA WIZARD!" << std::endl;

        // *** KÍCH HOẠT HIỂN THỊ VĂN BẢN ***
        m_showNotification = true;
        m_notificationTimer = m_notificationDuration; // Bắt đầu đếm ngược

        eventTriggered = true; // Đặt cờ
    }

    // ================= HIỆU ỨNG THÔNG BÁO MỜ DẦN =================
    if (m_showNotification)
    {
        m_notificationTimer -= dt;

        // Tỷ lệ còn lại 1 → 0
        float alphaRatio = m_notificationTimer / m_notificationDuration;

        if (alphaRatio < 0.f) alphaRatio = 0.f;
        if (alphaRatio > 1.f) alphaRatio = 1.f;

        // Đổi alpha của màu chữ (0–255)
        sf::Color c = m_notificationText.getFillColor();
        c.a = static_cast<std::uint8_t>(255 * alphaRatio);
        m_notificationText.setFillColor(c);

        // Hết thời gian → tắt thông báo
        if (m_notificationTimer <= 0.f)
        {
            m_showNotification = false;
        }
    }


    // Cập nhật boss nếu đã spawn
    if (m_bossSpawned && m_minotaurBoss) {
        m_minotaurBoss->Update(dt, playerPos + scrollOffset, m_totalScroll);
    }

    float m_bossSpawnX = 4000.f; // Vị trí X để spawn boss (ví dụ cuối map)
    //Spawn boss khi player tới gần m_bossSpawnX
    if (!m_bossSpawned && playerPos.x + m_totalScroll >= m_bossSpawnX) {
        m_minotaurBoss = std::make_unique<MinotaurBoss>(
            sf::Vector2f(5050.f, 535.f), // spawn y gần mặt đất
            4250.f, // left corner X
            5050.f  // right corner X
        );
        m_bossSpawned = true;
    }

    // Cập nhật enemy
    for (auto& enemy : m_enemies)
    {
        // 1. CẬP NHẬT QUÁI (CHỈ 1 LẦN)
        enemy->Update(dt, playerPos, m_totalScroll);

        // 2. KIỂM TRA ĐỂ TẠO ĐẠN (CHO TẤT CẢ CÁC LOẠI)

        // --- Tính deltaX (dùng chung cho tất cả) ---
        sf::Vector2f screenPos = enemy->GetPosition() - sf::Vector2f(m_totalScroll, 0.f);
        float deltaX = playerPos.x - screenPos.x;

        // a. Kiểm tra Wizard 2
        if (auto wizard2 = dynamic_cast<EnemyWizard2*>(enemy.get()))
        {
            if (wizard2 && !wizard2->IsDead())
            {
                auto bulletInfo = wizard2->TryToAttack(dt, deltaX);
                if (bulletInfo.has_value())
                {
                    m_wizardBullets.emplace_back(
                        bulletInfo->startPosition,
                        bulletInfo->direction,
                        bulletInfo->speed
                    );
                }
            }
        }
        // b. Kiểm tra Wizard 1
        else if (auto wizard1 = dynamic_cast<EnemyWizard*>(enemy.get()))
        {
            if (wizard1 && !wizard1->IsDead())
            {
                auto bulletInfo = wizard1->TryToAttack(dt, deltaX);
                if (bulletInfo.has_value())
                {
                    // (Bạn đang thiếu code tạo đạn cho Wizard 1 ở đây)
                    // m_enemyBullets.push_back(std::make_unique<EnemyBullet>(...));
                }
            }
        }
        // c. Kiểm tra Soldier
        else if (auto soldier = dynamic_cast<SoldierEnemy*>(enemy.get()))
        {
            if (auto bulletInfoOpt = soldier->TryToAttack(dt, deltaX))
            {
                const SoldierBulletInfo& info = bulletInfoOpt.value();
                m_enemyBullets.push_back(std::make_unique<EnemyBullet>(
                    info.startPosition,
                    info.direction,
                    info.speed
                ));
            }
        }
    } // --- KẾT THÚC VÒNG LẶP FOR ---

    // Cập nhật đạn của Soldier (m_enemyBullets)
	
    for (auto& bullet : m_enemyBullets) {
        bullet->Update(dt);
    }

    // Cập nhật đạn của Wizard 2 (m_wizardBullets)
    for (auto it = m_wizardBullets.begin(); it != m_wizardBullets.end(); )
    {
        it->Update(dt);
        if (it->IsDead())
        {
            it = m_wizardBullets.erase(it);
        }
        else
        {
            ++it;
        }
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

    // === KIỂM TRA ĐIỀU KIỆN KẾT THÚC GAME ===
    // Chỉ cập nhật game khi chưa kết thúc
    if (m_gameEnd.GetEndState() == GameEnd::EndState::NONE) {
        // Logic cập nhật (di chuyển, va chạm, v.v...)
        
        // Giả sử m_minotaurBoss là đối tượng MinotaurBoss của bạn
        if (m_bossSpawned==true) {
            if (m_minotaurBoss->IsDead()) {
                m_gameEnd.SetEndState(GameEnd::EndState::WIN);
                // Tạm dừng nhạc/âm thanh gameplay
            }
        }

        if (m_player.IsDead()) {
            m_gameEnd.SetEndState(GameEnd::EndState::LOSE);
            // Tạm dừng nhạc/âm thanh gameplay
        }
    }
    else {
        // Nếu game đã kết thúc, chỉ xử lý input của GameEnd
        m_gameEnd.HandleInput();

        // THÊM KIỂM TRA ĐIỀU KIỆN RESTART
        if (m_gameEnd.ShouldRestart()) {
            m_isRunning = false; // Đặt cờ này thành false để vòng lặp Game::Run() kết thúc
        }
    }
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

    for (auto& spawner : m_bossSpawners)
    {
        spawner.Draw(*m_window, scrollOffset.x);
    }

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

    // Vẽ MinotaurBoss
    if (m_bossSpawned && m_minotaurBoss) {
        m_minotaurBoss->Draw(*m_window);
    }


    // ======= VẼ VĂN BẢN THÔNG BÁO CUỐI CÙNG (ĐỂ NÓ Ở TRÊN) =======
    if (m_showNotification) {
        m_window->draw(m_notificationText);
    }

    // Vẽ đạn của kẻ địch
    for (auto& bullet : m_enemyBullets) {
        if (bullet->IsAlive()) {
            bullet->Draw(*m_window, m_totalScroll);
        }
    }
  
    for (auto& bullet : m_wizardBullets)
    {
        bullet.Draw(*m_window, m_totalScroll);
    }

    /*
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

    // <--- VẼ HITBOX CỦA BOSS (MÀU TÍM) ---
    if (m_bossSpawned && m_minotaurBoss && !m_minotaurBoss->IsDead()) {
        debugHitbox.setOutlineColor(sf::Color(255, 0, 255, 200)); // Màu Tím
        sf::FloatRect bossBounds = m_minotaurBoss->GetBounds(); // Lấy World Coords

        // Chuyển sang Screen Coords để VẼ
        bossBounds.position.x -= m_totalScroll;

        debugHitbox.setPosition(bossBounds.position);
        debugHitbox.setSize(bossBounds.size);
        m_window->draw(debugHitbox);
    }

    // --- Vẽ Hitbox của Đạn Wizard (Màu Xanh lơ) ---
    debugHitbox.setOutlineColor(sf::Color::Cyan); // Màu Xanh lơ
    for (auto& bullet : m_wizardBullets) {
        if (bullet.IsDead()) continue;

        sf::FloatRect bulletBounds = bullet.GetBounds(); // Lấy World Coords

        // Chuyển sang Screen Coords để VẼ
        bulletBounds.position.x -= m_totalScroll;

        debugHitbox.setPosition(bulletBounds.position);
        debugHitbox.setSize(bulletBounds.size);
        m_window->draw(debugHitbox);
    }

    // --- Vẽ Hitbox của Kén Nhện (Màu Hồng) ---
    debugHitbox.setOutlineColor(sf::Color::Magenta); // Màu hồng

    // Chỉ vẽ hitbox nếu kén còn sống (chưa nổ)
    if (!m_spiderSpawner.IsDead())
    {
        // 1. Lấy hitbox (Đây là Tọa độ Thế giới)
        sf::FloatRect spawnerBounds = m_spiderSpawner.GetBounds();

        // 2. Chuyển sang Tọa độ Màn hình để VẼ
        spawnerBounds.position.x -= m_totalScroll;

        // 3. Vẽ hitbox
        debugHitbox.setPosition(spawnerBounds.position);
        debugHitbox.setSize(spawnerBounds.size);
        m_window->draw(debugHitbox);
    }

    // --- Vẽ Hitbox của Đạn Player (Màu Trắng) ---
    debugHitbox.setOutlineColor(sf::Color::White); // Màu Trắng

    // Lặp qua danh sách đạn của Player
    for (auto& bullet : m_player.GetBullets())
    {
        if (bullet.IsDead()) continue;

        // 1. Lấy hitbox (Đây là Tọa độ Màn hình)
        sf::FloatRect bulletBounds = bullet.GetBounds();

        // 2. Không cần chuyển đổi, VẼ TRỰC TIẾP
        debugHitbox.setPosition(bulletBounds.position);
        debugHitbox.setSize(bulletBounds.size);
        m_window->draw(debugHitbox);
    }
    */
  
    // Vẽ màn hình GameEnd (nếu có)
    m_gameEnd.Draw();

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

        // --- KIỂM TRA ĐẠN VỚI KÉN ---
        // Nếu đạn chưa trúng quái, thì xét xem có trúng kén không
        if (!bulletHit && !m_spiderSpawner.IsDead()) {
            sf::FloatRect spawnerBounds = m_spiderSpawner.GetBounds();

            if (spawnerBounds.findIntersection(bulletBounds)) {
                m_spiderSpawner.TakeDamage(bullet_it->GetDamage());
                m_impactSound.play();
                bulletHit = true; // Đánh dấu đạn đã trúng
            }
        }
        for (auto& spawner : m_bossSpawners)
        {
            if (!bulletHit && !spawner.IsDead())
            {
                sf::FloatRect spawnerBounds = spawner.GetBounds();
                if (spawnerBounds.findIntersection(bulletBounds))
                {
                    spawner.TakeDamage(bullet_it->GetDamage());
                    m_impactSound.play();
                    bulletHit = true;
                    break; // Thoát khỏi vòng lặp 'for' của spawner
                }
            }
        }
        // Kiểm tra đạn player với boss
        if (!bulletHit && m_bossSpawned && m_minotaurBoss && !m_minotaurBoss->IsDead()) {
            sf::FloatRect bossBounds = m_minotaurBoss->GetBounds(); // Lấy bounds (World Coords)

            if (bossBounds.findIntersection(bulletBounds)) {
                m_minotaurBoss->TakeDamage(bullet_it->GetDamage());
                m_impactSound.play();
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

    // ĐẠN ENEMY vs PLAYER
    if (m_player.IsDead()) return; // Player chết rồi thì không cần check nữa

    // Chuyển Bounds của Player sang Tọa độ Thế giới (World Coords)
    sf::FloatRect playerWorldBounds = m_player.GetBounds();
    playerWorldBounds.position.x += m_totalScroll;

    // --- Kiểm tra đạn của Wizard ---
    for (auto it = m_wizardBullets.begin(); it != m_wizardBullets.end(); ++it)
    {
        if (it->IsDead()) continue;

        sf::FloatRect bulletBounds = it->GetBounds(); // World Coords

        if (playerWorldBounds.findIntersection(bulletBounds))
        {
            m_player.TakeDamage(1); 
            it->Hit();
        }
    }

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

    // Kiểm tra melee với BOSS
    if (!m_player.IsInvincible() && m_bossSpawned && m_minotaurBoss && !m_minotaurBoss->IsDead())
    {
        sf::FloatRect bossBounds = m_minotaurBoss->GetBounds(); // World Coords
        if (playerWorldBounds.findIntersection(bossBounds))
        {
            // VA CHẠM VẬT LÝ VỚI BOSS! (Dù là HÚC hay ĐÁNH)
            m_player.TakeDamage(1); // Boss chạm vào cũng mất 1 máu
        }
    }

    if (m_player.IsInvincible()) {
        return;
    }

    // Lặp qua tất cả kẻ địch (cả Lính và Nhện)
    for (auto& enemy : m_enemies) {
        if (enemy->IsDead()) continue;

        // Lấy Bounds của Enemy (World Coords)
        sf::FloatRect enemyBounds = enemy->GetBounds();

        // Kiểm tra va chạm Player (World) vs Enemy (World)
        if (playerWorldBounds.findIntersection(enemyBounds)) {

            // VA CHẠM VẬT LÝ!
            // Gây sát thương cho Player (Hàm TakeDamage đã có cơ chế bất tử)
            m_player.TakeDamage(1);
            break;
        }
    }
}

bool Game::WasRestartRequested() const {
    return m_gameEnd.ShouldRestart();
}
