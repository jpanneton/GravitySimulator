#include "PausedSimulationState.h"
#include "Engine/Core/ResourceHolder.h"
#include <SFML/Graphics/RenderWindow.hpp>

PausedSimulationState::PausedSimulationState(StateStack& stack, Context context)
    : State{ stack, context }
    , m_pausedText{}
{
    m_pausedText.setFont(getContext().fonts->get(FontsID::Main));
    m_pausedText.setString("PAUSE");
    m_pausedText.setCharacterSize(10);

    m_pausedText.setPosition(getContext().window->getSize().x - m_pausedText.getGlobalBounds().width - 5.f, 5.f);
}

bool PausedSimulationState::update(sf::Time dt)
{
    return false;
}

bool PausedSimulationState::handleEvent(const sf::Event & event)
{
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::P || event.type == sf::Event::KeyPressed  && event.key.code == sf::Keyboard::Escape)
    {
        requestStackPop();
    }
    return false;
}

void PausedSimulationState::draw()
{
    getContext().window->draw(m_pausedText);
}