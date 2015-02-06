/*==================================================================================
    Copyright (c) 2008, binaryzebra
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    * Neither the name of the binaryzebra nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE binaryzebra AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL binaryzebra BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=====================================================================================*/



#include "DAVAEngine.h"
#include "UI/UIEvent.h"
#include "UI/UIControlSystem.h"

#include "Platform/Qt5/QtLayer.h"

#if defined (__DAVAENGINE_MACOS__)
    #include "Platform/Qt5/MacOS/CoreMacOSPlatformQt.h"
#elif defined (__DAVAENGINE_WIN32__)
    #include "Platform/Qt5/Win32/CorePlatformWin32Qt.h"
#endif //#if defined (__DAVAENGINE_MACOS__)

#include "davaglwidget.h"

#include <QMessageBox>
#include <QTimer>
#include <QElapsedTimer>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QDebug>


#include <QOpenGLContext>
#include <QOffscreenSurface>

#if defined(__DAVAENGINE_WIN32__) || defined(__DAVAENGINE_MACOS__)
#include "Network/NetCore.h"
#endif

#include "Main/mainwindow.h"
#include "ui_mainwindow.h"


QOpenGLContext * DavaGLWidget::defaultContext = nullptr;
QOffscreenSurface * DavaGLWidget::defaultSurface = nullptr;
DAVA::uint64 DavaGLWidget::defaultContextID = 0;


DavaGLWidget::DavaGLWidget(QWidget *parent)
	: QOpenGLWidget(parent)
    , renderTimer(nullptr)
    , fps(60)
    , isInitialized(false)
    , currentDPR(1)
    , currentWidth(0)
    , currentHeight(0)
    , currentContextID(0)
{
    setAcceptDrops(true);
    setMouseTracking(true);

    setFocusPolicy( Qt::StrongFocus );
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    setMinimumSize(100, 100);
    
    DAVA::QtLayer::Instance()->SetDelegate(this);
    
    SetFPS(60);
    renderTimer = new QTimer(this);
}

DavaGLWidget::~DavaGLWidget()
{
}

void DavaGLWidget::SetFPS(int _fps)
{
    DVASSERT(0 != fps);
    
    DAVA::RenderManager::Instance()->SetFPS(fps);
    fps = _fps;
}


void DavaGLWidget::initializeGL()
{
    context()->setShareContext(defaultContext);
    
    currentContextID = (DAVA::uint64)CGLGetCurrentContext();
    DAVA::Logger::Info("[current]\n\tcurrentContextID = %lld\n\tdefaultContextID = %lld", currentContextID, defaultContextID);

    
    currentDPR = devicePixelRatio();
    
    DAVA::QtLayer::Instance()->InitializeGlWindow();
    
    isInitialized = true;
}

void DavaGLWidget::resizeGL(int w, int h)
{
    DAVA::RenderManager::Instance()->SetRenderContextId(currentContextID);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    currentWidth = w;
    currentHeight = h;

    PerformSizeChange();

    DAVA::RenderManager::Instance()->SetRenderContextId(defaultContextID);
}

void DavaGLWidget::paintEvent(QPaintEvent * e)
{
    QOpenGLWidget::paintEvent(e);
}


void DavaGLWidget::paintGL()
{
    DAVA::RenderManager::Instance()->SetRenderContextId(currentContextID);
    
    QElapsedTimer frameTimer;
    frameTimer.start();
    
    const int dpr = devicePixelRatio();
    if(dpr != currentDPR)
    {
        currentDPR = dpr;
        PerformSizeChange();
    }

    
    DAVA::QtLayer::Instance()->ProcessFrame();
    
    const DAVA::uint64 requestedFrameDelta = 1000 / fps;
    const DAVA::uint64 nextFrameDelta = Max((DAVA::uint64)1, requestedFrameDelta - frameTimer.elapsed());
    
    DAVA::RenderManager::Instance()->SetRenderContextId(defaultContextID);

    renderTimer->singleShot((int) nextFrameDelta, this, SLOT(OnRenderTimer()));
}


void DavaGLWidget::OnRenderTimer()
{
    update();
}


void DavaGLWidget::keyPressEvent(QKeyEvent *e)
{
    char16 keyChar = e->nativeVirtualKey();
    const Qt::KeyboardModifiers modifiers = e->modifiers();
    
    if(modifiers & Qt::ShiftModifier)
    {
        DAVA::InputSystem::Instance()->GetKeyboard()->OnKeyPressed(DAVA::DVKEY_SHIFT);
    }
    if(modifiers & Qt::ControlModifier)
    {
        DAVA::InputSystem::Instance()->GetKeyboard()->OnKeyPressed(DAVA::DVKEY_CTRL);
    }
    if(modifiers & Qt::AltModifier)
    {
        DAVA::InputSystem::Instance()->GetKeyboard()->OnKeyPressed(DAVA::DVKEY_ALT);
    }
    
    if(keyChar)
    {
        char16 davaKey = DAVA::InputSystem::Instance()->GetKeyboard()->GetDavaKeyForSystemKey(keyChar);
        DAVA::QtLayer::Instance()->KeyPressed(keyChar, davaKey, e->count(), e->timestamp());
    }
}

void DavaGLWidget::keyReleaseEvent(QKeyEvent *e)
{
    int key = e->key();
    char16 keyChar = e->nativeVirtualKey();

    if(Qt::Key_Shift == key)
    {
        DAVA::InputSystem::Instance()->GetKeyboard()->OnKeyUnpressed(DAVA::DVKEY_SHIFT);
    }
    else if(Qt::Key_Control == key)
    {
        DAVA::InputSystem::Instance()->GetKeyboard()->OnKeyUnpressed(DAVA::DVKEY_CTRL);
    }
    else if(Qt::Key_Alt == key)
    {
        DAVA::InputSystem::Instance()->GetKeyboard()->OnKeyUnpressed(DAVA::DVKEY_ALT);
    }
    
    if(keyChar)
    {
        char16 davaKey = DAVA::InputSystem::Instance()->GetKeyboard()->GetDavaKeyForSystemKey(keyChar);
        DAVA::QtLayer::Instance()->KeyReleased(keyChar, davaKey);
    }
}


void DavaGLWidget::mouseMoveEvent(QMouseEvent * event)
{
    const Qt::MouseButtons buttons = event->buttons();
    DAVA::UIEvent davaEvent = MapMouseEventToDAVA(event, currentDPR);
    davaEvent.phase = DAVA::UIEvent::PHASE_DRAG;

    if(buttons & Qt::LeftButton)
    {
        davaEvent.tid = MapQtButtonToDAVA(Qt::LeftButton);
        DAVA::QtLayer::Instance()->MouseEvent(davaEvent);
    }
    if(buttons & Qt::RightButton)
    {
        davaEvent.tid = MapQtButtonToDAVA(Qt::RightButton);
        DAVA::QtLayer::Instance()->MouseEvent(davaEvent);
    }
    if(buttons & Qt::MiddleButton)
    {
        davaEvent.tid = MapQtButtonToDAVA(Qt::MiddleButton);
        DAVA::QtLayer::Instance()->MouseEvent(davaEvent);
    }
}

void DavaGLWidget::mousePressEvent(QMouseEvent * event)
{
    DAVA::UIEvent davaEvent = MapMouseEventToDAVA(event, currentDPR);
    davaEvent.phase = DAVA::UIEvent::PHASE_BEGAN;

    DAVA::QtLayer::Instance()->MouseEvent(davaEvent);
}

void DavaGLWidget::mouseReleaseEvent(QMouseEvent * event)
{
    DAVA::UIEvent davaEvent = MapMouseEventToDAVA(event, currentDPR);
    davaEvent.phase = DAVA::UIEvent::PHASE_ENDED;
    
    DAVA::QtLayer::Instance()->MouseEvent(davaEvent);
}

void DavaGLWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    DAVA::UIEvent davaEvent = MapMouseEventToDAVA(event, currentDPR);
    davaEvent.phase = DAVA::UIEvent::PHASE_ENDED;
    davaEvent.tapCount = 2;
    
    DAVA::QtLayer::Instance()->MouseEvent(davaEvent);
}

DAVA::UIEvent DavaGLWidget::MapMouseEventToDAVA(const QMouseEvent *event, int dpr)
{
    DAVA::UIEvent davaEvent;
    QPoint pos = event->pos();
    davaEvent.point = davaEvent.physPoint = Vector2(pos.x() * dpr, pos.y() * dpr);
    davaEvent.tid = MapQtButtonToDAVA(event->button());
    davaEvent.timestamp = event->timestamp();
    davaEvent.tapCount = 1;

    return davaEvent;
}


DAVA::UIEvent::eButtonID DavaGLWidget::MapQtButtonToDAVA(const Qt::MouseButton button)
{
    switch (button)
    {
        case Qt::LeftButton:
            return DAVA::UIEvent::BUTTON_1;

        case Qt::RightButton:
            return DAVA::UIEvent::BUTTON_2;

        case Qt::MiddleButton:
            return DAVA::UIEvent::BUTTON_3;

        default:
            break;
    }
    
    return DAVA::UIEvent::BUTTON_NONE;
}

void DavaGLWidget::dragEnterEvent(QDragEnterEvent *event)
{
    event->setDropAction(Qt::LinkAction);
	event->accept();
}

void DavaGLWidget::dragMoveEvent(QDragMoveEvent *event)
{
	DAVA::Vector<DAVA::UIEvent> touches;
	DAVA::Vector<DAVA::UIEvent> emptyTouches;

	DAVA::UIEvent newTouch;
	newTouch.tid = DAVA::UIEvent::BUTTON_1;
	newTouch.physPoint.x = event->pos().x() * currentDPR;
	newTouch.physPoint.y = event->pos().y() * currentDPR;
	newTouch.phase = DAVA::UIEvent::PHASE_MOVE;
	touches.push_back(newTouch);

    DAVA::UIControlSystem::Instance()->OnInput(DAVA::UIEvent::PHASE_MOVE, emptyTouches, touches);

    event->setDropAction(Qt::LinkAction);
	event->accept();
}

void DavaGLWidget::dropEvent(QDropEvent *event)
{
	const QMimeData *mimeData = event->mimeData();
	emit OnDrop(mimeData);

    event->setDropAction(Qt::LinkAction);
	event->accept();
}


void DavaGLWidget::Quit()
{
    exit(0);
}

void DavaGLWidget::ShowAssertMessage(const char * message)
{
    QMessageBox::critical(this, "", message);
}


void DavaGLWidget::PerformSizeChange()
{
    DAVA::QtLayer::Instance()->Resize(currentWidth * currentDPR, currentHeight * currentDPR);
    
    emit Resized(currentWidth, currentHeight, currentDPR);
}

bool DavaGLWidget::InitializeDefaultOpenGLContext()
{
    DVASSERT(nullptr == defaultContext);

    defaultContext = QOpenGLContext::currentContext();
    if(defaultContext)
    {
        context()->setShareContext(defaultContext);
    }
        
    defaultContextID = (DAVA::uint64)CGLGetCurrentContext();
    DAVA::RenderManager::Instance()->SetRenderContextId(defaultContextID);
    
    emit Initialized();

    return true;
}

void DavaGLWidget::LogContext()
{
    DAVA::uint64 contextID = (DAVA::uint64)CGLGetCurrentContext();
    DAVA::Logger::Info("\n[log]\n\tcontextID = %lld\n\tdefaultContextID = %lld", contextID, defaultContextID);
}
