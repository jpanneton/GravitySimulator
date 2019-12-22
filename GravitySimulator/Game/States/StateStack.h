#pragma once

#include "State.h"
#include "StateIdentifiers.h"
#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Time.hpp>
#include <functional>
#include <unordered_map>
#include <vector>

class StateStack : sf::NonCopyable
{
public:
    enum Action
    {
        Push,
        Pop,
        Clear,
    };

    explicit StateStack(State::Context context);

    template<typename T>
    void registerState(StatesID stateID);

    void update(sf::Time dt);

    void draw();

    void handleEvent(const sf::Event& event);

    void pushState(StatesID stateID);

    void popState();

    void clearStates();

    bool isEmpty() const;

private:
    State::Ptr createState(StatesID stateID);

    void applyPendingChanges();

    struct PendingChange
    {
        explicit PendingChange(Action action, StatesID stateID = StatesID::None);

        Action action;
        StatesID stateID;
    };

    std::vector<State::Ptr> m_stack;
    std::vector<PendingChange> m_pendingList;

    State::Context m_context;
    std::unordered_map<StatesID, std::function<State::Ptr()>> m_factories;
};

template<typename StateType>
void StateStack::registerState(StatesID stateID)
{
    m_factories[stateID] = [this]()
    {
        return State::Ptr{ std::make_unique<StateType>(*this, m_context) };
    };
}