#include "Base/Platform.h"

#if defined(__DAVAENGINE_COREV2__)
#if defined(__DAVAENGINE_IPHONE__)

#include "Base/BaseTypes.h"
#include "Concurrency/Semaphore.h"
#include "Concurrency/AutoResetEvent.h"
#include "Engine/Private/EngineBackend.h"
#include "Debug/DVAssert.h"
#include "Logger/Logger.h"

#import <Foundation/NSThread.h>
#import <UIKit/UIAlertView.h>
#import <UIKit/UIApplication.h>

@interface AlertDialog : NSObject<UIAlertViewDelegate>
{
    BOOL dismissedOnResignActive;
    UIAlertView* alert;
}
- (int)showModal;
- (void)addButtonWithTitle:(NSString*)buttonTitle;
- (void)alertView:(UIAlertView*)alertView didDismissWithButtonIndex:(NSInteger)buttonIndex;

@property(nonatomic, assign) NSString* title;
@property(nonatomic, assign) NSString* message;
@property(nonatomic, assign) NSMutableArray<NSString*>* buttonNames;
@property(nonatomic, readonly) int clickedIndex;
@property(nonatomic, assign) BOOL dismissOnResignActive;

@end

@implementation AlertDialog

- (int)showModal
{
    DAVA::Private::EngineBackend::showingModalMessageBox = true;

    @autoreleasepool
    {
        alert = [[[UIAlertView alloc] initWithTitle:_title
                                            message:_message
                                           delegate:self
                                  cancelButtonTitle:nil
                                  otherButtonTitles:nil, nil] autorelease];
        for (NSString* s : _buttonNames)
        {
            [alert addButtonWithTitle:s];
        }

        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(applicationWillResignActive:)
                                                     name:UIApplicationWillResignActiveNotification
                                                   object:nil];

        _clickedIndex = -1;
        dismissedOnResignActive = NO;

        [alert show];
        @autoreleasepool
        {
            while (_clickedIndex < 0)
            {
                [[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate:[NSDate distantFuture]];
            }
        }

        [[NSNotificationCenter defaultCenter] removeObserver:self];
        [alert setDelegate:nil];
        alert = nil;
        if (dismissedOnResignActive)
        {
            _clickedIndex = -1;
        }
    }

    DAVA::Private::EngineBackend::showingModalMessageBox = false;
    return _clickedIndex;
}

- (void)applicationWillResignActive:(NSNotification*)notification
{
    if (_dismissOnResignActive)
    {
        dismissedOnResignActive = YES;
        [alert dismissWithClickedButtonIndex:0 animated:NO];
    }
}

- (void)addButtonWithTitle:(NSString*)buttonTitle
{
    if (_buttonNames == nil)
    {
        _buttonNames = [[NSMutableArray alloc] init];
    }
    [_buttonNames addObject:buttonTitle];
}

- (void)alertView:(UIAlertView*)alertView didDismissWithButtonIndex:(NSInteger)buttonIndex
{
    _clickedIndex = static_cast<int>(buttonIndex);
}

@end

namespace DAVA
{
namespace Debug
{
Semaphore semaphore(1);
AutoResetEvent autoEvent;

int MessageBox(const String& title, const String& message, const Vector<String>& buttons, int defaultButton)
{
    DVASSERT(0 < buttons.size() && buttons.size() <= 3);
    DVASSERT(0 <= defaultButton && defaultButton < static_cast<int>(buttons.size()));

    int result = -1;
    auto showMessageBox = [title, message, buttons, defaultButton, &result]() {
        @autoreleasepool
        {
            AlertDialog* alertDialog = [[[AlertDialog alloc] init] autorelease];
            [alertDialog setTitle:@(title.c_str())];
            [alertDialog setMessage:@(message.c_str())];
            [alertDialog setDismissOnResignActive:[NSThread isMainThread]];

            for (const String& s : buttons)
            {
                [alertDialog addButtonWithTitle:@(s.c_str())];
            }

            [alertDialog performSelectorOnMainThread:@selector(showModal)
                                          withObject:nil
                                       waitUntilDone:YES];
            result = [alertDialog clickedIndex];
            autoEvent.Signal();
        }
    };

    const bool directCall = [NSThread isMainThread];
    if (directCall)
    {
        showMessageBox();
    }
    else
    {
        // Do not use Window::RunOnUIThread as message box becomes unresponsive to user input.
        // I do not know why so.
        semaphore.Wait();
        showMessageBox();
        autoEvent.Wait();
        semaphore.Post(1);
    }
    return result;
}

} // namespace Debug
} // namespace DAVA

#endif // defined(__DAVAENGINE_IPHONE__)
#endif // defined(__DAVAENGINE_COREV2__)
