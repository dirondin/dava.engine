#pragma once

#include "EditorSystems/EditorSystemsConstants.h"
#include "EditorSystems/EditorSystemsManager.h"

namespace DAVA
{
class UIEvent;
class ContextAccessor;
}

namespace Painting
{
class Painter;
}

using CanvasControls = DAVA::Vector<DAVA::RefPtr<DAVA::UIControl>>;

class BaseEditorSystem
{
public:
    BaseEditorSystem(DAVA::ContextAccessor* accessor);

    virtual ~BaseEditorSystem() = default;

protected:
    //some systems can process OnUpdate from UpdateViewsSystem
    //order of update is matter, because without it canvas views will be in invalid state during the frame

    const EditorSystemsManager* GetSystemsManager() const;
    EditorSystemsManager* GetSystemsManager();
    Painting::Painter* GetPainter() const;

    DAVA::ContextAccessor* accessor = nullptr;

private:
    //this class is designed to be used only by EditorSystemsManager
    friend class EditorSystemsManager;

    //Ask system for a new state
    //this state must be unique
    //if two different systems require different states at the same time - this is logical error
    //this method is not const because it can update cached states
    virtual eDragState RequireNewState(DAVA::UIEvent* currentInput, eInputSource inputSource);
    //if system can not process input - OnInput method will not be called. Generated indicates that input is generated by canvas
    virtual bool CanProcessInput(DAVA::UIEvent* currentInput, eInputSource inputSource) const;
    //process input to realize state logic.  Generated indicates that input is generated by canvas
    virtual void ProcessInput(DAVA::UIEvent* currentInput, eInputSource inputSource);
    //invalidate caches or prepare to work depending on states
    virtual void OnDragStateChanged(eDragState currentState, eDragState previousState);
    virtual void OnDisplayStateChanged(eDisplayState currentState, eDisplayState previousState);
    virtual CanvasControls CreateCanvasControls();
    virtual void DeleteCanvasControls(const CanvasControls& canvasControls);

    virtual eSystems GetOrder() const = 0;
    virtual void OnUpdate();

    virtual void Invalidate();
};
