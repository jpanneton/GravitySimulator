#pragma once

#include "ResourceIdentifiers.h"
#include "States/StateStack.h"
#include "Engine/Core/ResourceHolder.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <string>

class Application : sf::NonCopyable
{
public:
    Application();

    void run();
    void runTest();

private:
    void processInput();

    void update(sf::Time dt);

    void render();

    //----------------------------------------

    void updateStatistics(sf::Time dt);

    void registerStates();

    //========================================

    static const sf::Time TimePerFrame;

    sf::RenderWindow m_window;	
    FontHolder m_fonts;

    TextureHolder m_textures;
    std::vector<Texture> m_materialTextures;
    std::vector<Texture> m_skyBoxTextures;

    std::string m_selectedSystem;
    System m_system;

    StateStack m_stateStack;

    bool m_updateGame;

    sf::Text m_statisticsText;
    sf::Time m_statisticsUpdateTime;
    size_t m_statisticsNumFrames;
};