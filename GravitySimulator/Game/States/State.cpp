#include "State.h"
#include "StateStack.h"

State::Context::Context(sf::RenderWindow& window, TextureHolder& textures, FontHolder& fonts, std::string& selectedSystem, std::vector<Texture>& materialTextures , std::vector<Texture>& skyboxTextures, System& system)
    : window{ &window }
    , textures{ &textures }
    , fonts{ &fonts }
    , selectedSystem{ &selectedSystem }
    , materialTextures{ &materialTextures}
    , skyboxTextures{ &skyboxTextures }
    , system{ &system }
{
}

//========================================

State::State(StateStack& stack, Context context)
    : m_stack{ &stack }
    , m_context{ context }
{
}

State::~State()
{
}

void State::requestStackPush(StatesID stateID)
{
    m_stack->pushState(stateID);
}

void State::requestStackPop()
{
    m_stack->popState();
}

void State::requestStackClear()
{
    m_stack->clearStates();
}

State::Context State::getContext() const
{
    return m_context;
}