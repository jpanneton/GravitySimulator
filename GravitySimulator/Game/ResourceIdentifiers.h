#pragma once

// ResourceHolder
template <typename Resource, typename Identifier>
class ResourceHolder;

/************** Forward declarations **************/

// SFML classes
namespace sf
{
    class Font;
    class Texture;
}

/************** Resource identifiers **************/

// Fonts
enum class FontsID
{
    Main
};

// Textures
enum class TexturesID
{
    TitleScreen,
    Crosshair
};

using FontHolder = ResourceHolder<sf::Font, FontsID>;
using TextureHolder = ResourceHolder<sf::Texture, TexturesID>;