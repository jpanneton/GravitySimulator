#include "SaveSimulationState.h"
#include "Engine/Core/ResourceHolder.h"
#include "Engine/Physics/Serializer.h"
#include <SFML/Graphics/RenderWindow.hpp>

SaveSimulationState::SaveSimulationState(StateStack & stack, Context context)
    : State{ stack, context }
    , m_saveText{}
    , m_fileNameTitleText{}
    , m_fileNameText{}
    , m_fileName{}
{

    m_saveText.setFont(getContext().fonts->get(FontsID::Main));
    m_saveText.setString("SAVE");
    m_saveText.setCharacterSize(10);
    m_saveText.setPosition(getContext().window->getSize().x - m_saveText.getGlobalBounds().width - 5.f, 5.f);

    m_fileNameTitleText.setFont(getContext().fonts->get(FontsID::Main));
    m_fileNameTitleText.setString("File name: ");
    m_fileNameTitleText.setCharacterSize(30);
    m_fileNameTitleText.setOutlineColor(sf::Color());
    m_fileNameTitleText.setOutlineThickness(1.f);
    
    sf::FloatRect textRect = m_fileNameTitleText.getLocalBounds();
    m_fileNameTitleText.setOrigin(textRect.left + textRect.width / 2.0f,	textRect.top + textRect.height / 2.0f);
    m_fileNameTitleText.setPosition(sf::Vector2f((getContext().window->getSize().x / 2.0f) - 75.f, getContext().window->getSize().y / 2.0f));
    
    m_fileNameText.setFont(getContext().fonts->get(FontsID::Main));
    m_fileNameText.setString(m_fileName);
    m_fileNameText.setCharacterSize(25);
    m_fileNameText.setOutlineColor(sf::Color());
    m_fileNameText.setOutlineThickness(1.f);
    
    m_fileNameText.setPosition(sf::Vector2f(getContext().window->getSize().x / 2.0f, getContext().window->getSize().y / 2.0f - 15.f));
}

bool SaveSimulationState::update(sf::Time dt)
{
    return false;
}

bool SaveSimulationState::handleEvent(const sf::Event & event)
{
    
    if (event.key.code == sf::Keyboard::Escape)
    {
        requestStackPop();
    }
    else if	(event.key.code == sf::Keyboard::BackSpace && !m_fileName.empty())
    {	
        m_fileName.pop_back();			
        m_fileNameText.setString(m_fileName);
    }
    else if (event.type == sf::Event::TextEntered )
    {
        // A code = 97 and Z code = 122
        if (event.key.code >= 97 && event.key.code <= 122) {
            m_fileName.push_back(static_cast<char>(event.text.unicode));
            m_fileNameText.setString(m_fileName);
        }		
    }
    else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter && !m_fileName.empty())
    {
        // Save file
        save();
        requestStackPop();
    }


    return false;
}

void SaveSimulationState::draw()
{
    getContext().window->draw(m_saveText);
    getContext().window->draw(m_fileNameTitleText);
    getContext().window->draw(m_fileNameText);
}

void SaveSimulationState::save() {
    // Create the new system file
    auto newSystem = std::ofstream("Systems/" + m_fileName + ".txt");
    getContext().system->save(newSystem);
    newSystem.close();
}