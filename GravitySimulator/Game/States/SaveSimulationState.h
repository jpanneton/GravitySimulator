#pragma once

#include "State.h"
#include <SFML/Graphics/Text.hpp>

class SaveSimulationState : public State
{
    sf::Text m_saveText;
    sf::Text m_fileNameText;
    sf::Text m_fileNameTitleText;
    std::string m_fileName;

    void save();
public:
    SaveSimulationState(StateStack& stack, Context context);

    bool update(sf::Time dt) final;

    bool handleEvent(const sf::Event& event) final;

    void draw() final;
};
