#include<SFML/Graphics.hpp>
#include<unordered_map>
#include<string>

class AssetManeger {
public:
	static AssetManeger& getInstance();
	const sf::Texture& getTexture(const std::string& filename);
	const sf::Font& getFont(const std::string& filename);
private:
	AssetManeger() = default;
	AssetManeger(const AssetManeger&) = delete;;
	AssetManeger& operator=(const AssetManeger&) = delete;

	std::unordered_map<std::string, sf::Texture> m_texture;
	std::unordered_map<std::string, sf::Font> m_fonts;;
};

