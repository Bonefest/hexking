#ifndef SYSTEM_H_INCLUDED
#define SYSTEM_H_INCLUDED

#include <tuple>

#include "../Dependencies/entt.hpp"
#include "ui/UIText.h"
#include "cocos2d.h"



#include "../Components/Components.h"
#include "../Commands/Command.h"
#include "../HexagonDrawNode.h"
#include "../Events/Events.h"
#include "../HexagonNode.h"
#include "../GameMap.h"
#include "../helper.h"
#include "../common.h"

namespace hk {

    class ISystem {
    public:
        virtual void onEnter(entt::registry& registry, entt::dispatcher& dispatcher) { }
        virtual void update(entt::registry& registry, entt::dispatcher& dispatcher, float delta) { }
        virtual void onExit(entt::registry& registry, entt::dispatcher& dispatcher) { }
    };

    class HexagonRenderingSystem: public ISystem {
    public:
        HexagonRenderingSystem(): m_elapsedTime(0.0f) { }

        virtual void onEnter(entt::registry& registry, entt::dispatcher& dispatcher) {
            auto runningScene = cocos2d::Director::getInstance()->getRunningScene();
            m_renderer = HexagonDrawNode::createNode();
            runningScene->addChild(m_renderer);

            registry.on_construct<HexagonRole>().connect<&HexagonRenderingSystem::onHexagonRoleAssigned>(*this);
        }

        virtual void update(entt::registry& registry, entt::dispatcher& dispatcher, float delta) {
            m_elapsedTime += 5.0f * delta;
            m_renderer->clear();

            registry.view<Hexagon>().each([&](entt::entity entity, Hexagon& hexagon){
                hexagon.stateOwner.draw(m_renderer, registry, entity);
            });

            GameData& gameData = registry.ctx<GameData>();
            float hexagonSize = gameData.hexagonSize;
            registry.view<Hexagon, PressedHexagon>().each([&](entt::entity hexagon, Hexagon& hexagonComponent, PressedHexagon& pressedHexagonComponent){

                double t = std::min((getCurrentTimeInMs() - pressedHexagonComponent.pressingTime) / 1000.0, 1.0);

                m_renderer->drawHexagon(hexToRectCoords(hexagonComponent.position, hexagonSize),
                                        hexagonSize,
                                        cocos2d::Color4F(0.75f, 0.75f, 0.75f, (t + 1.0f) * 0.25f + 0.25f),
                                        cocos2d::Color4F::BLACK,
                                        0.0f);
            });

            registry.view<Hexagon, FocusedHexagon>().each([&](entt::entity hexagon, Hexagon& hexagonComponent, FocusedHexagon& focusedHexagonComponent){
                double t = (std::sin(m_elapsedTime) + 1.0f);
                m_renderer->drawHexagon(hexToRectCoords(hexagonComponent.position, hexagonSize),
                                        hexagonSize,
                                        cocos2d::Color4F(0, 0, 0, 0),
                                        cocos2d::Color4F(0.0f, 1.0f, 1.0f, t * 0.25f + 0.5f),
                                        3.0f);
            });

        }

        void onHexagonRoleAssigned(entt::registry& registry, entt::entity hexagon) {
            auto runningScene = cocos2d::Director::getInstance()->getRunningScene();
            Hexagon& hexagonComponent = registry.get<Hexagon>(hexagon);

            if(hexagonComponent.icon != nullptr) {
                hexagonComponent.icon->removeFromParentAndCleanup(true);
                hexagonComponent.icon = nullptr;
            }

            HexagonRole& hexagonRoleComponent = registry.get<HexagonRole>(hexagon);
            GameData& gameData = registry.ctx<GameData>();

            cocos2d::Vec2 worldPosition = hexToRectCoords(hexagonComponent.position, gameData.hexagonSize);
            cocos2d::Sprite* roleSprite = cocos2d::Sprite::create(roleToSpriteName(hexagonRoleComponent.role));
            roleSprite->getTexture()->setAliasTexParameters();
            roleSprite->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
            roleSprite->setPosition(worldPosition);

            runningScene->addChild(roleSprite, 10);

            hexagonComponent.icon = roleSprite;
        }

    private:
        HexagonDrawNode* m_renderer;
        float m_elapsedTime;

        /*
            ? Loading hexagon vertices once and every frame controlling hexagon size. On resizing - recalculating model. ?
        */

    };

    class HexagonUpdatingSystem: public ISystem {
    public:
        virtual void update(entt::registry& registry, entt::dispatcher& dispatcher, float delta) {
            registry.view<Hexagon>().each([&](entt::entity hexagon, Hexagon& hexagonComponent){
                hexagonComponent.stateOwner.update(registry, dispatcher, hexagon, delta);
            });
        }
    };

    typedef cocos2d::EventTouch::EventCode event_code;
    typedef std::pair<event_code, cocos2d::Touch> event_data;


    class InputHandlingSystem: public ISystem {
    public:
        virtual void onEnter(entt::registry& registry, entt::dispatcher& dispatcher) {
            auto runningScene = cocos2d::Director::getInstance()->getRunningScene();

            auto cocosEDispatcher = cocos2d::Director::getInstance()->getEventDispatcher();

            auto touchListener = cocos2d::EventListenerTouchOneByOne::create();
            touchListener->setSwallowTouches(true);

            touchListener->onTouchBegan = CC_CALLBACK_2(InputHandlingSystem::onTouchBegan, this);
            touchListener->onTouchMoved = CC_CALLBACK_2(InputHandlingSystem::onTouchMoved, this);
            touchListener->onTouchEnded = CC_CALLBACK_2(InputHandlingSystem::onTouchEnded, this);
            touchListener->onTouchCancelled = CC_CALLBACK_2(InputHandlingSystem::onTouchCancelled, this);

            cocosEDispatcher->addEventListenerWithFixedPriority(touchListener, 2);
        }

        virtual void update(entt::registry& registry, entt::dispatcher& dispatcher, float delta) {
            for(event_data ed : m_unprocessedEvents) {
                switch(ed.first) {
                    case event_code::BEGAN: {
                        dispatcher.trigger<TouchBeganEvent>(ed.second);
                        break;
                    }

                    case event_code::MOVED: {
                        dispatcher.trigger<TouchMovedEvent>(ed.second);

                        auto camera = cocos2d::Director::getInstance()->getRunningScene()->getDefaultCamera();
                        camera->setPosition(camera->getPosition() - ed.second.getDelta());

                        break;
                    }

                    case event_code::ENDED: {
                        dispatcher.trigger<TouchEndedEvent>(ed.second);

                        break;
                    }

                    case event_code::CANCELLED: {
                        dispatcher.trigger<TouchCancelledEvent>(ed.second);

                        break;
                    }
                }
            }

            m_unprocessedEvents.clear();
        }

        bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event) {
            m_unprocessedEvents.push_back(event_data{event_code::BEGAN, cocos2d::Touch(*touch)});
            return true;
        }

        void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event) {
            m_unprocessedEvents.push_back(event_data{event_code::MOVED, cocos2d::Touch(*touch)});
        }

        void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event) {
            m_unprocessedEvents.push_back(event_data{event_code::ENDED, cocos2d::Touch(*touch)});
        }

        void onTouchCancelled(cocos2d::Touch* touch, cocos2d::Event* event) {
            m_unprocessedEvents.push_back(event_data{event_code::CANCELLED, cocos2d::Touch(*touch)});
        }

    private:
        std::vector<event_data> m_unprocessedEvents;
    };

    class HexagonTouchHandleSystem: public ISystem {
    public:
        HexagonTouchHandleSystem() {
            m_pressedHexagon = std::pair{entt::null, 0.0};
        }

        virtual void onEnter(entt::registry& registry, entt::dispatcher& dispatcher) {
            dispatcher.sink<TouchBeganEvent>().connect<&HexagonTouchHandleSystem::onTouchBegan>(*this);
            dispatcher.sink<TouchEndedEvent>().connect<&HexagonTouchHandleSystem::onTouchEnded>(*this);
            dispatcher.sink<TouchMovedEvent>().connect<&HexagonTouchHandleSystem::onTouchMoved>(*this);
            dispatcher.sink<TouchCancelledEvent>().connect<&HexagonTouchHandleSystem::onTouchCancelled>(*this);
        }

        virtual void update(entt::registry& registry, entt::dispatcher& dispatcher, float delta) {
            GameData& gameData = registry.ctx<GameData>();
            GameMap& gameMap = registry.ctx<GameMap>();

            for(auto event: m_unprocessedTouchBeganEvents) {
                auto hexagon = gameMap.getHexagonAtPixel(getTouchWorldLocation(event.touch), gameData.hexagonSize);
                if(registry.valid(hexagon)) {
                    m_pressedHexagons[event.touch.getID()] = hexagon;
                    dispatcher.trigger<HexagonPressedEvent>(event.touch, hexagon);
                }
            }

            for(auto event: m_unprocessedTouchMovedEvents) {
                auto hexagonIter = m_pressedHexagons.find(event.touch.getID());
                if(hexagonIter == m_pressedHexagons.end()) break;

                auto hexagon = gameMap.getHexagonAtPixel(getTouchWorldLocation(event.touch), gameData.hexagonSize);
                if(hexagonIter->second != hexagon) {
                    m_pressedHexagons.erase(event.touch.getID());
                    dispatcher.trigger<HexagonCancelledEvent>(event.touch, hexagon);
                }
            }

            for(auto event: m_unprocessedTouchEndedEvents) {
                auto hexagonIter = m_pressedHexagons.find(event.touch.getID());
                if(hexagonIter == m_pressedHexagons.end()) break;

                auto hexagon = gameMap.getHexagonAtPixel(getTouchWorldLocation(event.touch), gameData.hexagonSize);
                if(hexagonIter->second == hexagon) {
                    dispatcher.trigger<HexagonReleasedEvent>(event.touch, hexagon);
                }

                m_pressedHexagons.erase(event.touch.getID());
            }

            m_unprocessedTouchBeganEvents.clear();
            m_unprocessedTouchEndedEvents.clear();
            m_unprocessedTouchMovedEvents.clear();
            m_unprocessedTouchCancelledEvents.clear();
        }

        void onTouchBegan(const TouchBeganEvent& event) {
            m_unprocessedTouchBeganEvents.push_back(event);
        }

        void onTouchEnded(const TouchEndedEvent& event) {
            m_unprocessedTouchEndedEvents.push_back(event);
        }

        void onTouchMoved(const TouchMovedEvent& event) {
            m_unprocessedTouchMovedEvents.push_back(event);
        }

        void onTouchCancelled(const TouchCancelledEvent& event) {
            m_unprocessedTouchCancelledEvents.push_back(event);
        }
    private:
        cocos2d::Vec2 getTouchWorldLocation(cocos2d::Touch touch) {
            cocos2d::Camera* camera = cocos2d::Director::getInstance()->getRunningScene()->getDefaultCamera();
            cocos2d::Vec2 worldLocation = camera->getPosition();
            worldLocation -= cocos2d::Director::getInstance()->getVisibleSize() * 0.5f;
            worldLocation += touch.getLocation();

            return worldLocation;
        }

        std::vector<TouchBeganEvent> m_unprocessedTouchBeganEvents;
        std::vector<TouchEndedEvent> m_unprocessedTouchEndedEvents;
        std::vector<TouchMovedEvent> m_unprocessedTouchMovedEvents;
        std::vector<TouchCancelledEvent> m_unprocessedTouchCancelledEvents;

        std::pair<entt::entity, double> m_pressedHexagon;
        std::map<int, entt::entity> m_pressedHexagons;
    };

    class HexagonTouchActionSystem: public ISystem {
    public:
        virtual void onEnter(entt::registry& registry, entt::dispatcher& dispatcher) {
            dispatcher.sink<HexagonPressedEvent>().connect<&HexagonTouchActionSystem::onHexagonPressedEvent>(*this);
            dispatcher.sink<HexagonReleasedEvent>().connect<&HexagonTouchActionSystem::onHexagonReleasedEvent>(*this);
            dispatcher.sink<HexagonCancelledEvent>().connect<&HexagonTouchActionSystem::onHexagonCancelledEvent>(*this);
        }

        virtual void update(entt::registry& registry, entt::dispatcher& dispatcher, float delta) {
            for(auto event: m_unprocessedPressedEvents) {
                m_pressedHexagons[event.hexagon] = getCurrentTimeInMs();
                if(isFriendlyHexagon(registry, event.hexagon))
                    registry.assign_or_replace<PressedHexagon>(event.hexagon, getCurrentTimeInMs());
            }

            for(auto event: m_unprocessedReleasedEvents) {
                auto hexagonIter = m_pressedHexagons.find(event.hexagon);
                if(hexagonIter != m_pressedHexagons.end()) {
                    m_pressedHexagons.erase(hexagonIter);
                    registry.remove_if_exists<PressedHexagon>(event.hexagon);

                    dispatcher.trigger<ShowHexagonMenuEvent>(event.hexagon);
                }

            }

            for(auto event: m_unprocessedCanceledEvents) {
                auto hexagonIter = m_pressedHexagons.find(event.hexagon);
                if(hexagonIter != m_pressedHexagons.end()) {
                    registry.remove_if_exists<PressedHexagon>(event.hexagon);
                    registry.remove_if_exists<FocusedHexagon>(event.hexagon);

                    m_pressedHexagons.erase(hexagonIter);
                }
            }

            for(auto pressedHexagonIter = m_pressedHexagons.begin(); pressedHexagonIter != m_pressedHexagons.end();) {

                if(getCurrentTimeInMs() - pressedHexagonIter->second > 1000.0) {

                    if(registry.valid(pressedHexagonIter->first) && registry.has<PressedHexagon>(pressedHexagonIter->first)) {
                        cocos2d::log("trigger upgrade hexagon event!");
                        registry.remove<PressedHexagon>(pressedHexagonIter->first);

                        pressedHexagonIter = m_pressedHexagons.erase(pressedHexagonIter);
                        continue;
                    }

                }

                pressedHexagonIter++;
            }

            m_unprocessedPressedEvents.clear();
            m_unprocessedReleasedEvents.clear();
            m_unprocessedCanceledEvents.clear();
        }

        void onHexagonPressedEvent(const HexagonPressedEvent& event) {
            m_unprocessedPressedEvents.push_back(event);
        }

        void onHexagonReleasedEvent(const HexagonReleasedEvent& event) {
            m_unprocessedReleasedEvents.push_back(event);
        }

        void onHexagonCancelledEvent(const HexagonCancelledEvent& event) {
            m_unprocessedCanceledEvents.push_back(event);
        }

    private:
        std::vector<HexagonPressedEvent> m_unprocessedPressedEvents;
        std::vector<HexagonReleasedEvent> m_unprocessedReleasedEvents;
        std::vector<HexagonCancelledEvent> m_unprocessedCanceledEvents;

        std::map<entt::entity, double> m_pressedHexagons;
    };

    typedef std::pair<HexagonNode*, std::shared_ptr<ICommand>> button;

    class HexagonMenuSystem: public ISystem {
    public:
        HexagonMenuSystem(): m_hexagonTarget(entt::null) { }

        virtual void onEnter(entt::registry& registry, entt::dispatcher& dispatcher) {
            auto runningScene = cocos2d::Director::getInstance()->getRunningScene();

            m_menuRenderer = cocos2d::DrawNode::create();
            runningScene->addChild(m_menuRenderer);

            dispatcher.sink<ShowHexagonMenuEvent>().connect<&HexagonMenuSystem::onShowHexagonMenuEvent>(*this);

            m_touchListener = cocos2d::EventListenerTouchOneByOne::create();
            m_touchListener->onTouchBegan = CC_CALLBACK_2(HexagonMenuSystem::onTouchMenuButtonBegan, this);
            m_touchListener->setSwallowTouches(true);

            cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(m_touchListener, 1);
        }

        virtual void update(entt::registry& registry, entt::dispatcher& dispatcher, float delta) {
            m_menuRenderer->clear();

            GameData& gameData = registry.ctx<GameData>();
            GameMap& gameMap = registry.ctx<GameMap>();

            processEvents(registry);
            processCommands(registry, dispatcher);

            if(registry.view<Hexagon, FocusedHexagon>().empty()) {
                clearButtons();
                m_hexagonTarget = entt::null;
            }
        }

        void onShowHexagonMenuEvent(const ShowHexagonMenuEvent& event) {
            m_unprocessedEvents.push_back(event);
        }

        bool onTouchMenuButtonBegan(cocos2d::Touch* touch, cocos2d::Event* event) {
            auto globalTouchLocation = cocos2d::Director::getInstance()->getRunningScene()->getDefaultCamera()->getPosition();
            globalTouchLocation += touch->getLocation();
            globalTouchLocation -= cocos2d::Director::getInstance()->getVisibleSize() * 0.5f;

            for(auto button : m_buttons) {
                cocos2d::Rect buttonRect = button.first->getBoundingBox();
                buttonRect.origin -= button.first->getContentSize() * 0.5f;
                if(buttonRect.containsPoint(globalTouchLocation)) {
                    m_unprocessedCommands.push_back(button.second);
                    return true;
                }
            }

            return false;
        }

    private:
        void processCommands(entt::registry& registry, entt::dispatcher& dispatcher) {
            GameData& gameData = registry.ctx<GameData>();

            if(!m_unprocessedCommands.empty()) {
                registry.clear<FocusedHexagon>();

                auto command = m_unprocessedCommands.back();
                dispatcher.trigger<CommandEvent>(command, m_hexagonTarget, gameData.controllablePlayer);

                m_unprocessedCommands.clear();
            }
        }

        void processEvents(entt::registry& registry) {
            GameData& gameData = registry.ctx<GameData>();
            GameMap& gameMap = registry.ctx<GameMap>();

            if(!m_unprocessedEvents.empty()) {
                registry.clear<FocusedHexagon>();

                auto hexagon = m_unprocessedEvents.back().hexagon;
                m_hexagonTarget = hexagon;

                Hexagon& hexagonComponent = registry.get<Hexagon>(hexagon);
                if(isFriendlyHexagon(registry, hexagon) ||
                   gameMap.hasFriendNeighbour(registry, hexagonComponent.position, gameData.controllableTeam)) {
                    clearButtons();

                    registry.assign_or_replace<FocusedHexagon>(hexagon);
                    generateButtons(registry, hexagon);
                }

                m_unprocessedEvents.clear();

            }
        }

        void generateButtons(entt::registry& registry, entt::entity hexagon) {
            auto runningScene = cocos2d::Director::getInstance()->getRunningScene();
            GameData& gameData = registry.ctx<GameData>();

            Hexagon& hexagonComponent = registry.get<Hexagon>(hexagon);

            cocos2d::Vec2 hexagonOrigin = hexToRectCoords(hexagonComponent.position, gameData.hexagonSize);

            if(registry.has<HexagonRole>(hexagon)) {

                HexagonNode* upgradeButtonNode = createButtonNode("upgrade.png",
                                                                  hexagonOrigin,
                                                                  gameData.hexagonSize);

                upgradeButtonNode->runAction(cocos2d::Spawn::create(cocos2d::MoveBy::create(0.5f, cocos2d::Vec2(0, gameData.hexagonSize * 3.0f)),
                                                                    cocos2d::FadeIn::create(0.5f),
                                                                    nullptr));

                runningScene->addChild(upgradeButtonNode, Constants::UI_LEVEL);

                m_buttons.push_back(button{upgradeButtonNode, std::make_shared<UpgradeCommand>()});
            } else {
                //sprite name, animation moving direction, role
                std::pair<cocos2d::Vec2, Role> buttonsData[] = {
                    {cocos2d::Vec2(-0.707f, 0.707f), Role::WORKER},
                    {cocos2d::Vec2(0.0f, 1.0f), Role::ATTACKER},
                    {cocos2d::Vec2(0.707f, 0.707f), Role::DEFENDER}
                };

                for(auto data: buttonsData) {

                    HexagonNode* purchaseWorkerButtonNode = createButtonNode(roleToSpriteName(std::get<1>(data)),
                                                                             hexagonOrigin,
                                                                             gameData.hexagonSize);

                    purchaseWorkerButtonNode->runAction(cocos2d::Spawn::create(cocos2d::MoveBy::create(0.5f, std::get<0>(data) * gameData.hexagonSize * 3.0f),
                                                                        cocos2d::FadeIn::create(0.5f),
                                                                        nullptr));


                    runningScene->addChild(purchaseWorkerButtonNode, Constants::UI_LEVEL);
                    m_buttons.push_back(button{purchaseWorkerButtonNode, std::make_shared<PurchaseCommand>(std::get<1>(data))});
                }
            }
        }

        HexagonNode* createButtonNode(const std::string& spriteName, cocos2d::Vec2 position, float hexagonSize) {
            HexagonNode* button = HexagonNode::createHexagon(hexagonSize * 1.25f);

            button->setPosition(position);
            button->setColor(cocos2d::Color3B(0, 192, 192));

            cocos2d::Sprite* sprite = cocos2d::Sprite::create(spriteName);
            sprite->getTexture()->setAliasTexParameters();
            sprite->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
            sprite->setContentSize(cocos2d::Size(hexagonSize, hexagonSize) * 1.25f);

            button->addChild(sprite);
            return button;
        }

        void clearButtons() {
            for(auto button : m_buttons) {
                button.first->removeFromParentAndCleanup(true);
            }

            m_buttons.clear();
        }

        cocos2d::DrawNode* m_menuRenderer;
        cocos2d::EventListenerTouchOneByOne* m_touchListener;

        entt::entity m_hexagonTarget;

        std::vector<ShowHexagonMenuEvent> m_unprocessedEvents;
        std::vector<button> m_buttons;

        std::vector<std::shared_ptr<ICommand>> m_unprocessedCommands;
    };

    class WarSystem: public ISystem {
    public:
        WarSystem():m_elapsedTime(0.0f) { }

        virtual void onEnter(entt::registry& registry, entt::dispatcher& dispatcher) {
            dispatcher.sink<HexagonDamageCausedEvent>().connect<&WarSystem::onDamageCaused>(*this);
        }

        virtual void update(entt::registry& registry, entt::dispatcher& dispatcher, float delta) {

            processEvents(registry);

            m_elapsedTime+= delta;
            if(m_elapsedTime < 0.5f) return;

            GameMap& gameMap = registry.ctx<GameMap>();

            auto [width, height] = gameMap.getSize();

            for(int y = 0; y < height; y++) {
                for(int x = 0; x < width; x++) {
                    cocos2d::Vec2 position(x, y);
                    auto hexagon = gameMap.getHexagon(position);

                    if(registry.valid(hexagon)) {
                        Hexagon& hexagonComponent = registry.get<Hexagon>(hexagon);
                        if(hexagonComponent.team == Team::NO_TEAM) continue;

                        if(registry.has<FightingHexagon>(hexagon)) {
                            if(!gameMap.hasEnemyNeighbour(registry, position, hexagonComponent.team)) {
                                registry.remove<FightingHexagon>(hexagon);
                                //?trigger WarFinishedEvent?
                            }
                        } else if(gameMap.hasEnemyNeighbour(registry, position, hexagonComponent.team)) {
                            registry.assign<FightingHexagon>(hexagon);
                            hexagonComponent.stateOwner.setState(registry, hexagon, std::make_shared<HexagonAttack>());
                            //?trigger WarStartedEvent?
                        }
                    }
                }
            }

            m_elapsedTime = 0.0f;
        }

        void onDamageCaused(const HexagonDamageCausedEvent& event) {
            m_unprocessedEvents.push_back(event);
        }

    private:
        void processEvents(entt::registry& registry) {
            for(auto event: m_unprocessedEvents) {
                if(auto targetRoleComponent = registry.try_get<HexagonRole>(event.target); targetRoleComponent) {
                    if(targetRoleComponent->currentHp < 0.0f) {
                        auto strikerHexagonComponent = registry.get<Hexagon>(event.striker);
                        auto targetHexagonComponent = registry.get<Hexagon>(event.target);

                        targetRoleComponent->currentHp = targetRoleComponent->hp * 0.1f;
                    }
                }
            }

            m_unprocessedEvents.clear();
        }

        float m_elapsedTime;
        std::vector<HexagonDamageCausedEvent> m_unprocessedEvents;

    };

    using random = cocos2d::RandomHelper;

    class FloatingTextSystem: public ISystem {
    public:
        virtual void onEnter(entt::registry& registry, entt::dispatcher& dispatcher) {
            dispatcher.sink<CreateFloatTextEvent>().connect<&FloatingTextSystem::onCreateFloatEvent>(*this);
        }

        void onCreateFloatEvent(const CreateFloatTextEvent& event) {
            auto runningScene = cocos2d::Director::getInstance()->getRunningScene();
            cocos2d::ui::Text* floatingText = cocos2d::ui::Text::create(event.text, Constants::DEFAULT_FONT, event.size);
            floatingText->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE);
            floatingText->setPosition(event.position);
            floatingText->setTextColor(event.color);

            float spawningTime = random::random_real(1.0, 1.5);
            cocos2d::Vec2 spawningEndPosition = cocos2d::Vec2(random::random_real(-0.2, 0.2), 1) * random::random_real(120.0, 150.0);

            auto spawnAction = cocos2d::Spawn::create(cocos2d::MoveBy::create(spawningTime,spawningEndPosition),
                                                      cocos2d::FadeOut::create(spawningTime),
                                                      nullptr);

            floatingText->runAction(cocos2d::Sequence::create(spawnAction, cocos2d::RemoveSelf::create(true), nullptr));
            runningScene->addChild(floatingText, 5);

        }
    private:
        std::vector<CreateFloatTextEvent> m_unprocessedEvents;
    };

    class CommandHandlingSystem: public ISystem {
    public:
        void onEnter(entt::registry& registry, entt::dispatcher& dispatcher) {
            dispatcher.sink<CommandEvent>().connect<&CommandHandlingSystem::onCommandEvent>(*this);
        }

        void update(entt::registry& registry, entt::dispatcher& dispatcher, float delta) {
            for(auto commandEvent: m_unprocessedCommands) {
                commandEvent.command->execute(registry, dispatcher, commandEvent.target, commandEvent.sender);
            }

            m_unprocessedCommands.clear();
        }

        void onCommandEvent(const CommandEvent& event) {
            m_unprocessedCommands.push_back(event);
        }

    private:
        std::vector<CommandEvent> m_unprocessedCommands;

    };
}


#endif // SYSTEM_H_INCLUDED
