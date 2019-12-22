#pragma once

#include "State.h"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <filesystem>

namespace fs = std::filesystem;

class TitleState : public State
{
public:
    TitleState(StateStack& stack, Context context);

    void draw() final;

    bool update(sf::Time dt) final;

    bool handleEvent(const sf::Event& event) final;

private:
    sf::Sprite m_backgroundSprite;

    sf::Text m_selectSystemText;
    sf::Text m_selectedSystemNameText;

    std::vector<fs::path> m_allSystems;
    std::size_t m_selectedSystemIndex;

    void loadSystemsNames();
    void selectNextSystem();
    void selectPreviousSystem();

    void createRandomSystem();
};