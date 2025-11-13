#pragma once
#include <SFML/Graphics.hpp>
#include "IEnemy.h"
#include <memory>
#include "EnemyBullet.h"
#include <optional>

enum class SoldierState {
	PATROL,		//Tuan tra: di chuyen qua lai trong khu vuc nhat dinh
	CHASE,		//Duoi theo: di chuyen ve phia Player
	ATTACK,		//Tan cong: ban dan ve phia Player
};

class SoldierEnemy : public IEnemy {
	private:
		const sf::Texture* m_texture;//Texture cua ke dich	
		sf::Vector2f m_position;	//Vi tri hien tai cua ke dich
		float m_speed;				//Toc do di chuyen cua ke dich	
		int m_health;				//Mau cua ke dich
		bool m_facingRight;			//Huong quay mat cua ke dich
		float m_attackCooldown;		//Thoi gian cho giua cac dot tan cong
		float m_attackTimer;		//Bo dem thoi gian tan cong
		sf::Vector2f m_patrolStart;	//Diem bat dau cua khu vuc tuan tra
		sf::Vector2f m_patrolEnd;	//Diem ket thuc cua khu vuc tuan tra
		float m_patrolDirection = 1.0f;	//Huong di chuyen trong khu vuc tuan tra
		float m_chaseRange = 500.0f;	//Khoang cach de bat dau duoi theo Player
		float m_attackRange = 400.0f;	//Tam tan cong cua ke dich
		SoldierState m_currentState = SoldierState::PATROL; //Trang thai hien tai cua ke dich
		std::unique_ptr<sf::Sprite> m_sprite;		//Hinh anh ke dich
		sf::Vector2f m_drawPos; // vị trí tạm để vẽ sau khi trừ scroll
		void TransitionState(SoldierState newState);
		bool CheckAttackRange(float deltaX) const;
	public:
		SoldierEnemy(sf::Vector2f spawnPos, float patrolDistance);
		void Update(float dt, sf::Vector2f playerPos, float scrollOffset) override;
		void Draw(sf::RenderWindow& window) override;
		sf::FloatRect GetBounds() const override;
		void TakeDamage(int damage) override;
		bool IsDead() const override;
		sf::Vector2f GetPosition() const override { return m_position; }
		void SetDrawPosition(const sf::Vector2f& pos) override;
		std::optional<SoldierBulletInfo> TryToAttack(float dt, float deltaX);
};