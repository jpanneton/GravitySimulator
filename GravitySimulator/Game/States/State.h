#pragma once

#include "StateIdentifiers.h"
#include "Engine/Display/Texture.h"
#include "Engine/Physics/System.h"
#include "Game/ResourceIdentifiers.h"
#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include <memory>
#include <string>
#include <vector>

namespace sf
{
    class RenderWindow;
}

class StateStack;

class State
{
public:
    using Ptr = std::unique_ptr<State>;

    struct Context
    {
        Context(sf::RenderWindow& window, TextureHolder& textures, FontHolder& fonts, std::string& selectedSystem, std::vector<Texture>& materialTextures, std::vector<Texture>& skyBoxTextures, System& system);

        sf::RenderWindow* window;
        TextureHolder* textures;
        FontHolder* fonts;
        std::string* selectedSystem;
        std::vector<Texture>* materialTextures;
        std::vector<Texture>* skyBoxTextures;
        System* system;
    };

    State(StateStack& stack, Context context);

    virtual ~State();

    virtual bool update(sf::Time dt) = 0;

    virtual void draw() = 0;

    virtual bool handleEvent(const sf::Event& event) = 0;

protected:
    void requestStackPush(StatesID stateID);

    void requestStackPop();

    void requestStackClear();

    Context getContext() const;

private:
    StateStack* m_stack;
    Context m_context;
};
