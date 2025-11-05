#pragma once
#include <SFML/Graphics.hpp>
#include "IEnemy.h"

enum class SodierState {
	PATROL,		//Tuan tra: di chuyen qua lai trong khu vuc nhat dinh
	CHASE,		//Duoi theo: di chuyen ve phia Player
	ATTACK,		//Tan cong: ban dan ve phia Player
	DEAD,		//Dung yen: khong lam gi ca
};

class SodierEnemy : public IEnemy {
	private:
		sf::Sprite m_sprite;		//Hinh anh ke dich
		sf::Texture m_texture;		//Texture cua ke dich	
		sf::Vector2f m_position;	//Vi tri hien tai cua ke dich
		float m_speed;				//Toc do di chuyen cua ke dich	
		int m_health;				//Mau cua ke dich
		bool m_facingRight;			//Huong quay mat cua ke dich
		float m_attackCooldown;		//Thoi gian cho giua cac dot tan cong
		float m_attackTimer;		//Bo dem thoi gian tan cong
		sf::Vector2f m_patrolStart;	//Diem bat dau cua khu vuc tuan tra
		sf::Vector2f m_patrolEnd;	//Diem ket thuc cua khu vuc tuan tra
		float m_patrolDirection = 1.0f	//Huong di chuyen trong khu vuc tuan tra
		const float m_chaseRange = 300.0f;	//Khoang cach de bat dau duoi theo Player
		SodierState m_currentState = SodierState::PATROL; //Trang thai hien tai cua ke dich
	puclic:
		SodierEnemy(sf::Vector2f spawnPos, float patrolDistance);
		void Update(float dt, sf::Vector2f playerPos) override;
		void Draw(sf::RenderWindow& window) override;
		sf::FloatRect GetBounds() const override;
		void TakeDamage(int damage) override;
		bool IsDead() const override;
};