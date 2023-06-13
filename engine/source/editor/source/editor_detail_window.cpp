#include "editor/include/editor_detail_window.h"
#include "editor/include/editor_ui_utils.h"

#include "function/ui/Plugins/DDTarget.h"
#include "function/ui/Widgets/Buttons/Button.h"
#include "function/ui/Widgets/Layout/GroupCollapsable.h"
#include "function/ui/Widgets/Visual/Separator.h"

#include "runtime/function/ui/Widgets/Layout/Columns.h"

#include "runtime/function/framework/component/animation/animation_component.h"
#include "runtime/function/framework/component/camera/camera_component.h"
#include "runtime/function/framework/component/component.h"
#include "runtime/function/framework/component/lua/lua_component.h"
#include "runtime/function/framework/component/mesh/mesh_component.h"
#include "runtime/function/framework/component/motor/motor_component.h"
#include "runtime/function/framework/component/particle/particle_component.h"
#include "runtime/function/framework/component/rigidbody/rigidbody_component.h"
#include "runtime/function/framework/component/transform/transform_component.h"

namespace LunarYue
{
    using namespace UI::Widgets;

    DetailWindow::DetailWindow(const std::string& p_title, bool p_opened, const UI::Settings::PanelWindowSettings& p_windowSettings) :
        PanelWindow(p_title, p_opened, p_windowSettings)
    {
        m_inspectorHeader          = &CreateWidget<UI::Widgets::Layout::Group>();
        m_inspectorHeader->enabled = false;
        m_actorInfo                = &CreateWidget<UI::Widgets::Layout::Group>();

        auto& headerColumns = m_inspectorHeader->CreateWidget<UI::Widgets::Layout::Columns<2>>();

        /* Name field */
        auto nameGatherer = [this] { return !m_targetObject.expired() ? m_targetObject.lock()->getName() : "%undef%"; };
        auto nameProvider = [this](const std::string& p_newName) {
            if (!m_targetObject.expired())
                m_targetObject.lock()->setName(p_newName);
        };
        GUIDrawer::DrawString(headerColumns, "Name", nameGatherer, nameProvider);

        /* Tag field */
        auto tagGatherer = [this] { return !m_targetObject.expired() ? m_targetObject.lock()->getTag() : "%undef%"; };
        auto tagProvider = [this](const std::string& p_newName) {
            if (!m_targetObject.expired())
                m_targetObject.lock()->setTag(p_newName);
        };
        GUIDrawer::DrawString(headerColumns, "Tag", tagGatherer, tagProvider);

        /* Active field */
        auto activeGatherer = [this] { return !m_targetObject.expired() ? m_targetObject.lock()->isSelfActive() : false; };
        auto activeProvider = [this](bool p_active) {
            if (!m_targetObject.expired())
                m_targetObject.lock()->setActive(p_active);
        };
        GUIDrawer::DrawBoolean(headerColumns, "Active", activeGatherer, activeProvider);

        /* Component select + button */
        {
            auto& componentSelectorWidget     = m_inspectorHeader->CreateWidget<UI::Widgets::Selection::ComboBox>(0);
            componentSelectorWidget.lineBreak = false;
            componentSelectorWidget.choices.emplace(0, "RigidBodyComponent");
            componentSelectorWidget.choices.emplace(1, "ParticleComponent");
            componentSelectorWidget.choices.emplace(2, "MotorComponent");
            componentSelectorWidget.choices.emplace(3, "MeshComponent");
            componentSelectorWidget.choices.emplace(4, "LuaComponent");
            componentSelectorWidget.choices.emplace(5, "CameraComponent");
            componentSelectorWidget.choices.emplace(6, "AnimationComponent");

            auto& addComponentButton = m_inspectorHeader->CreateWidget<UI::Widgets::Buttons::Button>("Add Component", Vector2 {100.f, 0});
            addComponentButton.idleBackgroundColor = UI::Types::Color {0.7f, 0.5f, 0.f};
            addComponentButton.textColor           = UI::Types::Color::White;
            addComponentButton.ClickedEvent += [&componentSelectorWidget, this] {
                const auto object = m_targetObject.lock();
                if (object)
                    return;

                switch (componentSelectorWidget.currentChoice)
                {
                    case 0:
                        object->addComponent<RigidBodyComponent>();
                        break;
                    case 1:
                        object->addComponent<ParticleComponent>();
                        break;
                    case 2:
                        object->addComponent<MotorComponent>();
                        break;
                    case 3:
                        object->addComponent<MeshComponent>();
                        break;
                    case 4:
                        object->addComponent<LuaComponent>();
                        break;
                    case 5:
                        object->addComponent<CameraComponent>();
                        break;
                    case 6:
                        object->addComponent<AnimationComponent>();
                        break;
                }

                componentSelectorWidget.ValueChangedEvent.Invoke(componentSelectorWidget.currentChoice);
            };

            componentSelectorWidget.ValueChangedEvent += [this, &addComponentButton](int p_value) {
                auto defineButtonsStates = [&addComponentButton](bool p_componentExists) {
                    addComponentButton.disabled = p_componentExists;
                    addComponentButton.idleBackgroundColor =
                        !p_componentExists ? UI::Types::Color {0.7f, 0.5f, 0.f} : UI::Types::Color {0.1f, 0.1f, 0.1f};
                };

                const auto object = m_targetObject.lock();
                if (object)
                    return;

                switch (p_value)
                {
                    case 0:
                        defineButtonsStates(object->getComponent<RigidBodyComponent>());
                        return;
                    case 1:
                        defineButtonsStates(object->getComponent<ParticleComponent>());
                        return;
                    case 2:
                        defineButtonsStates(object->getComponent<MotorComponent>());
                        return;
                    case 3:
                        defineButtonsStates(object->getComponent<MeshComponent>());
                        return;
                    case 4:
                        defineButtonsStates(object->getComponent<LuaComponent>());
                        return;
                    case 5:
                        defineButtonsStates(object->getComponent<CameraComponent>());
                        return;
                    case 6:
                        defineButtonsStates(object->getComponent<AnimationComponent>());
                        return;
                }
            };

            m_componentSelectorWidget = &componentSelectorWidget;
        }

        ///* Script selector + button */
        //{
        //    m_scriptSelectorWidget            = &m_inspectorHeader->CreateWidget<UI::Widgets::InputFields::InputText>("");
        //    m_scriptSelectorWidget->lineBreak = false;
        //    auto& ddTarget = m_scriptSelectorWidget->AddPlugin<UI::Plugins::DDTarget<std::pair<std::string, Layout::Group*>>>("File");

        //    auto& addScriptButton               = m_inspectorHeader->CreateWidget<UI::Widgets::Buttons::Button>("Add Script", Vector2 {100.f, 0});
        //    addScriptButton.idleBackgroundColor = UI::Types::Color {0.7f, 0.5f, 0.f};
        //    addScriptButton.textColor           = UI::Types::Color::White;

        //    // Add script button state updater
        //    const auto updateAddScriptButton = [&addScriptButton, this](const std::string& p_script) {
        //        const std::string realScriptPath = EDITOR_CONTEXT(projectScriptsPath) + p_script + ".lua";

        //        const auto targetActor   = GetTargetActor();
        //        const bool isScriptValid = std::filesystem::exists(realScriptPath) && targetActor && !targetActor->GetBehaviour(p_script);

        //        addScriptButton.disabled            = !isScriptValid;
        //        addScriptButton.idleBackgroundColor = isScriptValid ? UI::Types::Color {0.7f, 0.5f, 0.f} : UI::Types::Color {0.1f, 0.1f, 0.1f};
        //    };

        //    m_scriptSelectorWidget->ContentChangedEvent += updateAddScriptButton;

        //    addScriptButton.ClickedEvent += [updateAddScriptButton, this] {
        //        const std::string realScriptPath = EDITOR_CONTEXT(projectScriptsPath) + m_scriptSelectorWidget->content + ".lua";

        //        // Ensure that the script is a valid one
        //        if (std::filesystem::exists(realScriptPath))
        //        {
        //            GetTargetActor()->AddBehaviour(m_scriptSelectorWidget->content);
        //            updateAddScriptButton(m_scriptSelectorWidget->content);
        //        }
        //    };

        //    ddTarget.DataReceivedEvent += [updateAddScriptButton, this](std::pair<std::string, Layout::Group*> p_data) {
        //        m_scriptSelectorWidget->content = EDITOR_EXEC(GetScriptPath(p_data.first));
        //        updateAddScriptButton(m_scriptSelectorWidget->content);
        //    };
        //}

        m_inspectorHeader->CreateWidget<UI::Widgets::Visual::Separator>();

        // m_destroyedListener = Core::ECS::Actor::DestroyedEvent += [this](Core::ECS::Actor& p_destroyed) {
        //     if (&p_destroyed == m_targetObject)
        //         UnFocus();
        // };
    }

    DetailWindow::~DetailWindow()
    {
        // Core::ECS::Actor::DestroyedEvent -= m_destroyedListener;

        // UnFocus();
    }

    void DetailWindow::FocusActor(const std::weak_ptr<Object>& target)
    {
        // if (m_targetObject)
        //     UnFocus();

        // m_actorInfo->RemoveAllWidgets();

        // m_targetObject = &p_target;

        // m_componentAddedListener = m_targetObject->ComponentAddedEvent += [this](auto& useless) { EDITOR_EXEC(DelayAction([this] { Refresh(); }));
        // }; m_behaviourAddedListener = m_targetObject->BehaviourAddedEvent += [this](auto& useless) { EDITOR_EXEC(DelayAction([this] { Refresh();
        // })); }; m_componentRemovedListener = m_targetObject->ComponentRemovedEvent +=
        //     [this](auto& useless) { EDITOR_EXEC(DelayAction([this] { Refresh(); })); };
        // m_behaviourRemovedListener = m_targetObject->BehaviourRemovedEvent +=
        //     [this](auto& useless) { EDITOR_EXEC(DelayAction([this] { Refresh(); })); };

        // m_inspectorHeader->enabled = true;

        // CreateActorInspector(p_target);

        //// Force component and script selectors to trigger their ChangedEvent to update button states
        // m_componentSelectorWidget->ValueChangedEvent.Invoke(m_componentSelectorWidget->currentChoice);
        // m_scriptSelectorWidget->ContentChangedEvent.Invoke(m_scriptSelectorWidget->content);

        // EDITOR_EVENT(ActorSelectedEvent).Invoke(*m_targetObject);
    }

    void DetailWindow::UnFocus()
    {
        // if (m_targetObject)
        //{
        //     m_targetObject->ComponentAddedEvent -= m_componentAddedListener;
        //     m_targetObject->ComponentRemovedEvent -= m_componentRemovedListener;
        //     m_targetObject->BehaviourAddedEvent -= m_behaviourAddedListener;
        //     m_targetObject->BehaviourRemovedEvent -= m_behaviourRemovedListener;
        // }

        // SoftUnFocus();
    }

    void DetailWindow::SoftUnFocus()
    {
        // if (m_targetObject)
        //{
        //     EDITOR_EVENT(ActorUnselectedEvent).Invoke(*m_targetObject);
        //     m_inspectorHeader->enabled = false;
        //     m_targetObject             = nullptr;
        //     m_actorInfo->RemoveAllWidgets();
        // }
    }

    const std::weak_ptr<Object>& DetailWindow::GetTargetActor() const { return m_targetObject; }

    void DetailWindow::CreateActorInspector(const std::weak_ptr<Object>& target)
    {
        // std::map<std::string, Core::ECS::Components::AComponent*> components;

        // for (auto component : p_target.GetComponents())
        //     if (component->GetName() != "Transform")
        //         components[component->GetName()] = component.get();

        // auto transform = p_target.GetComponent<Core::ECS::Components::CTransform>();
        // if (transform)
        //     DrawComponent(*transform);

        // for (auto& [name, instance] : components)
        //     DrawComponent(*instance);

        // auto& behaviours = p_target.GetBehaviours();

        // for (auto& [name, behaviour] : behaviours)
        //     DrawBehaviour(behaviour);
    }

    void DetailWindow::DrawComponent(Reflection::ReflectionInstance& instance)
    {
        // if (auto inspectorItem = dynamic_cast<Core::API::IInspectorItem*>(&p_component); inspectorItem)
        //{
        //     auto& header    = m_actorInfo->CreateWidget<UI::Widgets::Layout::GroupCollapsable>(p_component.GetName());
        //     header.closable = !dynamic_cast<Core::ECS::Components::CTransform*>(&p_component);
        //     header.CloseEvent += [this, &header, &p_component] {
        //         if (p_component.owner.RemoveComponent(p_component))
        //             m_componentSelectorWidget->ValueChangedEvent.Invoke(m_componentSelectorWidget->currentChoice);
        //     };
        //     auto& columns     = header.CreateWidget<UI::Widgets::Layout::Columns<2>>();
        //     columns.widths[0] = 200;
        //     inspectorItem->OnInspector(columns);
        // }
    }

    void DetailWindow::DrawBehaviour(Reflection::ReflectionInstance& instance)
    {
        // if (auto inspectorItem = dynamic_cast<Core::API::IInspectorItem*>(&p_behaviour); inspectorItem)
        //{
        //     auto& header    = m_actorInfo->CreateWidget<UI::Widgets::Layout::GroupCollapsable>(p_behaviour.name);
        //     header.closable = true;
        //     header.CloseEvent += [this, &header, &p_behaviour] { p_behaviour.owner.RemoveBehaviour(p_behaviour); };

        //    auto& columns     = header.CreateWidget<UI::Widgets::Layout::Columns<2>>();
        //    columns.widths[0] = 200;
        //    inspectorItem->OnInspector(columns);
        //}
    }

    void DetailWindow::Refresh()
    {
        // if (m_targetObject)
        //{
        //     m_actorInfo->RemoveAllWidgets();
        //     CreateActorInspector(*m_targetObject);
        // }
    }
} // namespace LunarYue
