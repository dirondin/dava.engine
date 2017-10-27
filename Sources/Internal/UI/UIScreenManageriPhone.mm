#include "Base/BaseTypes.h"
#if defined(__DAVAENGINE_IPHONE__)

#include "UI/UIScreenManager.h"
#include "UI/UIScreenManageriPhoneImpl.h"
#include "Base/BaseObject.h"
#include "Engine/Engine.h"
#include "Logger/Logger.h"
#include "UI/UIControlSystem.h"
#include "UI/UIScreen.h"

@class RenderViewController;
@class RenderView;

namespace DAVA
{
UIScreenManager::UIScreenManager()
{
    glControllerId = -1;
    activeControllerId = -1;
    activeScreenId = -1;
}

UIScreenManager::~UIScreenManager()
{
    Vector<Screen> releaseBuf;
    for (Map<int, Screen>::const_iterator it = screens.begin(); it != screens.end(); it++)
    {
        if (it->second.type == Screen::TYPE_SCREEN)
        {
            ((UIScreen*)it->second.value)->UnloadGroup();
            //it->second.type == Screen::TYPE_NULL;
            releaseBuf.push_back(it->second);
        }
    }
    for (Vector<Screen>::const_iterator it = releaseBuf.begin(); it != releaseBuf.end(); it++)
    {
        ((UIScreen*)it->value)->Release();
    }
}
void UIScreenManager::SetGLControllerId(int _glControllerId)
{
    glControllerId = _glControllerId;
}

void UIScreenManager::ActivateGLController()
{
}

void UIScreenManager::SetFirst(int screenId)
{
    Screen& screen = screens[screenId];
    if (screen.type == Screen::TYPE_CONTROLLER)
    {
        [[ScreenManagerImpl instance] applicationLaunched:(UIViewController*)screen.value];
        activeControllerId = screenId;
    }
    else if (screen.type == Screen::TYPE_SCREEN)
    {
        Screen& glController = screens[glControllerId];
        UIViewController* controller = (UIViewController*)glController.value;
        if (controller)
        {
            [[ScreenManagerImpl instance] applicationLaunched:(UIViewController*)controller];
            activeControllerId = glControllerId;
        }
        else
        {
            Logger::Error("ScreenManager::SetFirst no gl controller registered (use SetGLControllerId)");
        }
        activeScreenId = screenId;
        GetEngineContext()->uiControlSystem->SetScreen((UIScreen*)screen.value);
    }
    else
    {
        Logger::Error("ScreenManager::SetFirst wrong type of screen");
    }
}

void UIScreenManager::SetScreen(int screenId)
{
    Screen& screen = screens[screenId];
    if (screen.type == Screen::TYPE_CONTROLLER)
    {
        UIViewController* controller = (UIViewController*)screen.value;
        if (controller)
        {
            [[ScreenManagerImpl instance] setViewController:controller];
        }
        activeControllerId = screenId;
        GetEngineContext()->uiControlSystem->SetScreen(nullptr);

        activeScreenId = -1;
    }
    else if (screen.type == Screen::TYPE_SCREEN)
    {
        // Set GL Controller first
        if (activeControllerId != glControllerId)
        {
            Screen& glController = screens[glControllerId];
            UIViewController* controller = (UIViewController*)glController.value;
            if (controller)
            {
                [[ScreenManagerImpl instance] setViewController:controller];
            }
            activeControllerId = glControllerId;
        }
        activeScreenId = screenId;

        GetEngineContext()->uiControlSystem->SetScreen((UIScreen*)screen.value);
    }
}

void UIScreenManager::ResetScreen()
{
    activeScreenId = -1;
    GetEngineContext()->uiControlSystem->Reset();
}

void UIScreenManager::RegisterController(int controllerId, void* controller)
{
    screens[controllerId] = Screen(Screen::TYPE_CONTROLLER, controller);
}

void UIScreenManager::RegisterScreen(int screenId, UIScreen* screen)
{
    screens[screenId] = Screen(Screen::TYPE_SCREEN, SafeRetain(screen));
}

UIScreen* UIScreenManager::GetScreen(int screenId)
{
    Screen& screen = screens[screenId];
    if (screen.type == Screen::TYPE_CONTROLLER)
    {
        return NULL;
    }
    else if (screen.type == Screen::TYPE_SCREEN)
    {
        return (UIScreen*)screen.value;
    }
    return NULL;
}
UIScreen* UIScreenManager::GetScreen()
{
    return GetScreen(activeScreenId);
}

int UIScreenManager::GetScreenId()
{
    return activeScreenId;
}

void* UIScreenManager::GetController(int controllerId)
{
    Screen& screen = screens[controllerId];
    if (screen.type == Screen::TYPE_SCREEN)
    {
        return NULL;
    }
    else if (screen.type == Screen::TYPE_CONTROLLER)
    {
        return (void*)screen.value;
    }
    return NULL;
}

void* UIScreenManager::GetController()
{
    return GetController(activeControllerId);
}

int UIScreenManager::GetControllerId()
{
    return activeControllerId;
}

void UIScreenManager::StopGLAnimation()
{
    Screen& glController = screens[glControllerId];
    UIViewController* controller = (UIViewController*)glController.value;
    RenderView* view = (RenderView*)controller.view;
    [view performSelector:@selector(stopAnimation)];
}

void UIScreenManager::StartGLAnimation()
{
    Screen& glController = screens[glControllerId];
    UIViewController* controller = (UIViewController*)glController.value;
    RenderView* view = (RenderView*)controller.view;
    [view performSelector:@selector(startAnimation)];
}
}

#endif // #if defined(__DAVAENGINE_IPHONE__)
