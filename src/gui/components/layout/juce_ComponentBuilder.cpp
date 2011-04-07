/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-11 by Raw Material Software Ltd.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the GNU General
   Public License (Version 2), as published by the Free Software Foundation.
   A copy of the license is included in the JUCE distribution, or can be found
   online at www.gnu.org/licenses.

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.rawmaterialsoftware.com/juce for more information.

  ==============================================================================
*/

#include "../../../core/juce_StandardHeader.h"

BEGIN_JUCE_NAMESPACE

#include "juce_ComponentBuilder.h"


//=============================================================================
namespace ComponentBuilderHelpers
{
    const String getStateId (const ValueTree& state)
    {
        return state [ComponentBuilder::idProperty].toString();
    }

    Component* findComponentWithID (OwnedArray<Component>& components, const String& compId)
    {
        jassert (compId.isNotEmpty());

        for (int i = components.size(); --i >= 0;)
        {
            Component* const c = components.getUnchecked (i);

            if (c->getComponentID() == compId)
                return components.removeAndReturn (i);
        }

        return 0;
    }

    Component* findComponentWithID (Component* const c, const String& compId)
    {
        jassert (compId.isNotEmpty());
        if (c->getComponentID() == compId)
            return c;

        for (int i = c->getNumChildComponents(); --i >= 0;)
        {
            Component* const child = findComponentWithID (c->getChildComponent (i), compId);

            if (child != nullptr)
                return child;
        }

        return 0;
    }

    Component* createNewComponent (ComponentBuilder::TypeHandler& type,
                                   const ValueTree& state, Component* parent)
    {
        Component* const c = type.addNewComponentFromState (state, parent);
        jassert (c != nullptr && c->getParentComponent() == parent);
        c->setComponentID (getStateId (state));
        return c;
    }

    void updateComponent (ComponentBuilder& builder, const ValueTree& state)
    {
        Component* topLevelComp = builder.getManagedComponent();

        if (topLevelComp != nullptr)
        {
            ComponentBuilder::TypeHandler* const type = builder.getHandlerForState (state);
            const String uid (getStateId (state));

            if (type == nullptr || uid.isEmpty())
            {
                // ..handle the case where a child of the actual state node has changed.
                if (state.getParent().isValid())
                    updateComponent (builder, state.getParent());
            }
            else
            {
                Component* const changedComp = findComponentWithID (topLevelComp, uid);

                if (changedComp != nullptr)
                    type->updateComponentFromState (changedComp, state);
            }
        }
    }
}

//=============================================================================
const Identifier ComponentBuilder::idProperty ("id");

ComponentBuilder::ComponentBuilder (const ValueTree& state_)
    : state (state_), imageProvider (nullptr)
{
    state.addListener (this);
}

ComponentBuilder::~ComponentBuilder()
{
    state.removeListener (this);

   #if JUCE_DEBUG
    // Don't delete the managed component!! The builder owns that component, and will delete
    // it automatically when it gets deleted.
    jassert (componentRef.get() == static_cast <Component*> (component));
   #endif
}

Component* ComponentBuilder::getManagedComponent()
{
    if (component == nullptr)
    {
        component = createComponent();

       #if JUCE_DEBUG
        componentRef = component;
       #endif
    }

    return component;
}

Component* ComponentBuilder::createComponent()
{
    jassert (types.size() > 0);  // You need to register all the necessary types before you can load a component!

    TypeHandler* const type = getHandlerForState (state);
    jassert (type != nullptr); // trying to create a component from an unknown type of ValueTree

    return type != nullptr ? ComponentBuilderHelpers::createNewComponent (*type, state, 0) : 0;
}

void ComponentBuilder::registerTypeHandler (ComponentBuilder::TypeHandler* const type)
{
    jassert (type != nullptr);

    // Don't try to move your types around! Once a type has been added to a builder, the
    // builder owns it, and you should leave it alone!
    jassert (type->builder == nullptr);

    types.add (type);
    type->builder = this;
}

ComponentBuilder::TypeHandler* ComponentBuilder::getHandlerForState (const ValueTree& s) const
{
    const Identifier targetType (s.getType());

    for (int i = 0; i < types.size(); ++i)
    {
        TypeHandler* const t = types.getUnchecked(i);

        if (t->getType() == targetType)
            return t;
    }

    return 0;
}

int ComponentBuilder::getNumHandlers() const noexcept
{
    return types.size();
}

ComponentBuilder::TypeHandler* ComponentBuilder::getHandler (const int index) const noexcept
{
    return types [index];
}

void ComponentBuilder::setImageProvider (ImageProvider* newImageProvider) noexcept
{
    imageProvider = newImageProvider;
}

ComponentBuilder::ImageProvider* ComponentBuilder::getImageProvider() const noexcept
{
    return imageProvider;
}

void ComponentBuilder::valueTreePropertyChanged (ValueTree& tree, const Identifier&)
{
    ComponentBuilderHelpers::updateComponent (*this, tree);
}

void ComponentBuilder::valueTreeChildAdded (ValueTree& tree, ValueTree&)
{
    ComponentBuilderHelpers::updateComponent (*this, tree);
}

void ComponentBuilder::valueTreeChildRemoved (ValueTree& tree, ValueTree&)
{
    ComponentBuilderHelpers::updateComponent (*this, tree);
}

void ComponentBuilder::valueTreeChildOrderChanged (ValueTree& tree)
{
    ComponentBuilderHelpers::updateComponent (*this, tree);
}

void ComponentBuilder::valueTreeParentChanged (ValueTree& tree)
{
    ComponentBuilderHelpers::updateComponent (*this, tree);
}

//==============================================================================
ComponentBuilder::TypeHandler::TypeHandler (const Identifier& valueTreeType_)
   : builder (nullptr),
     valueTreeType (valueTreeType_)
{
}

ComponentBuilder::TypeHandler::~TypeHandler()
{
}

ComponentBuilder* ComponentBuilder::TypeHandler::getBuilder() const noexcept
{
    // A type handler needs to be registered with a ComponentBuilder before using it!
    jassert (builder != nullptr);
    return builder;
}

void ComponentBuilder::updateChildComponents (Component& parent, const ValueTree& children)
{
    using namespace ComponentBuilderHelpers;

    const int numExistingChildComps = parent.getNumChildComponents();

    Array <Component*> componentsInOrder;
    componentsInOrder.ensureStorageAllocated (numExistingChildComps);

    {
        OwnedArray<Component> existingComponents;
        existingComponents.ensureStorageAllocated (numExistingChildComps);

        int i;
        for (i = 0; i < numExistingChildComps; ++i)
            existingComponents.add (parent.getChildComponent (i));

        const int newNumChildren = children.getNumChildren();
        for (i = 0; i < newNumChildren; ++i)
        {
            const ValueTree childState (children.getChild (i));

            ComponentBuilder::TypeHandler* const type = getHandlerForState (childState);
            jassert (type != nullptr);

            if (type != nullptr)
            {
                Component* c = findComponentWithID (existingComponents, getStateId (childState));

                if (c == nullptr)
                    c = createNewComponent (*type, childState, &parent);

                componentsInOrder.add (c);
            }
        }

        // (remaining unused items in existingComponents get deleted here as it goes out of scope)
    }

    // Make sure the z-order is correct..
    if (componentsInOrder.size() > 0)
    {
        componentsInOrder.getLast()->toFront (false);

        for (int i = componentsInOrder.size() - 1; --i >= 0;)
            componentsInOrder.getUnchecked(i)->toBehind (componentsInOrder.getUnchecked (i + 1));
    }
}


END_JUCE_NAMESPACE
