#pragma once
#include "PostEffect.h"
#include "ResourceHolder.h"
#include "ResourceIdentifier.h"

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Shader.hpp>

#include <array>

class BloomEffect :public PostEffect
{
public:

	BloomEffect();
	virtual void		apply(const sf::RenderTexture& input, sf::RenderTarget& output) override;
private:
	using RenderTextureArray = std::array<sf::RenderTexture, 2>;

private:
	void				prepareTextures(sf::Vector2u size);
	void				filterBright(const sf::RenderTexture& input, sf::RenderTexture& output);
	void				blurMultipass(RenderTextureArray& renderTextures);
	void				blur(const sf::RenderTexture& input, sf::RenderTexture& output, sf::Vector2f offsetFactor);
	void				downsample(const sf::RenderTexture& input, sf::RenderTexture& output);
	void				add(const sf::RenderTexture& source, const sf::RenderTexture& bloom, sf::RenderTarget& target);
private:
	ShaderHolder_t		shaders;

	sf::RenderTexture	brightnessTexture;
	RenderTextureArray	firstPassTextures;
	RenderTextureArray	secondPassTextures;
};

