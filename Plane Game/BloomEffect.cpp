#include "BloomEffect.h"
BloomEffect::BloomEffect()
	: shaders()
	, brightnessTexture()
	, firstPassTextures()
	, secondPassTextures()
{
	shaders.load(ShaderID::BrightnessPass, "Media/Shaders/Fullpass.vert", "Media/Shaders/Brightness.frag");
	shaders.load(ShaderID::DownSamplePass, "Media/Shaders/Fullpass.vert", "Media/Shaders/DownSample.frag");
	shaders.load(ShaderID::GaussianBlurPass, "Media/Shaders/Fullpass.vert", "Media/Shaders/GuassianBlur.frag");
	shaders.load(ShaderID::AddPass, "Media/Shaders/Fullpass.vert", "Media/Shaders/Add.frag");
}
void BloomEffect::apply(const sf::RenderTexture& input, sf::RenderTarget& output)
{
	prepareTextures(input.getSize());
	filterBright(input, brightnessTexture);
	downsample(brightnessTexture, firstPassTextures[0]);
	blurMultipass(firstPassTextures);
	downsample(firstPassTextures[0], secondPassTextures[0]);
	blurMultipass(secondPassTextures);
	add(firstPassTextures[0], secondPassTextures[0], firstPassTextures[1]);
	firstPassTextures[1].display();
	add(input, firstPassTextures[1], output);
}
void BloomEffect::prepareTextures(sf::Vector2u size)
{
	if (brightnessTexture.getSize() != size)
	{
		brightnessTexture.create(size.x, size.y);
		brightnessTexture.setSmooth(true);
		firstPassTextures[0].create(size.x / 2, size.y / 2);
		firstPassTextures[0].setSmooth(true);
		firstPassTextures[1].create(size.x / 2, size.y / 2);
		firstPassTextures[1].setSmooth(true);
		secondPassTextures[0].create(size.x / 4, size.y / 4);
		secondPassTextures[0].setSmooth(true);
		secondPassTextures[1].create(size.x / 4, size.y / 4);
		secondPassTextures[1].setSmooth(true);
	}
}
void BloomEffect::filterBright(const sf::RenderTexture& input, sf::RenderTexture& output)
{
	sf::Shader& brightness = shaders.get(ShaderID::BrightnessPass);
	brightness.setUniform("source", input.getTexture());
	applyShader(brightness, output);
	output.display();
}
void BloomEffect::blurMultipass(RenderTextureArray& renderTextures)
{
	sf::Vector2u textureSize = renderTextures[0].getSize();
	for (std::size_t count = 0; count < 2; ++count)
	{
		blur(renderTextures[0], renderTextures[1], sf::Vector2f(0.f, 1.f / textureSize.y));
		blur(renderTextures[1], renderTextures[0], sf::Vector2f(1.f / textureSize.x, 0.f));
	}
}
void BloomEffect::blur(const sf::RenderTexture& input, sf::RenderTexture& output, sf::Vector2f offsetFactor)
{
	sf::Shader& gaussianBlur = shaders.get(ShaderID::GaussianBlurPass);
	gaussianBlur.setUniform("source", input.getTexture());
	gaussianBlur.setUniform("offsetFactor", offsetFactor);
	applyShader(gaussianBlur, output);
	output.display();
}
void BloomEffect::downsample(const sf::RenderTexture& input, sf::RenderTexture& output)
{
	sf::Shader& downSampler = shaders.get(ShaderID::DownSamplePass);
	downSampler.setUniform("source", input.getTexture());
	downSampler.setUniform("sourceSize", sf::Vector2f(input.getSize()));
	applyShader(downSampler, output);
	output.display();
}
void BloomEffect::add(const sf::RenderTexture& source, const sf::RenderTexture& bloom, sf::RenderTarget& output)
{
	sf::Shader& adder = shaders.get(ShaderID::AddPass);
	adder.setUniform("source", source.getTexture());
	adder.setUniform("bloom", bloom.getTexture());
	applyShader(adder, output);
}