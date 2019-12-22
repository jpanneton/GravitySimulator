#pragma once

#include "State.h"
#include <SFML/Graphics/Text.hpp>

class PausedSimulationState : public State
{
    sf::Text m_pausedText;
public:
    PausedSimulationState(StateStack& stack, Context context);

    bool update(sf::Time dt) final;

    bool handleEvent(const sf::Event& event) final;

    void draw() final;
};
