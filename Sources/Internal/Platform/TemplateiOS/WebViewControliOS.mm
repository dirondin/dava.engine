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


#include "WebViewControliOS.h"
#include "DAVAEngine.h"

#import <UIKit/UIKit.h>
#import <HelperAppDelegate.h>
#import "Platform/TemplateiOS/BackgroundView.h"

@interface WebViewURLDelegate : NSObject<UIWebViewDelegate>
{
	DAVA::IUIWebViewDelegate* delegate;
	DAVA::UIWebView* webView;
}

- (id)init;

- (void)setDelegate:(DAVA::IUIWebViewDelegate*)d andWebView:(DAVA::UIWebView*)w;

- (BOOL)webView: (UIWebView*)webView shouldStartLoadWithRequest:(NSURLRequest *)request navigationType:(UIWebViewNavigationType)navigationType;

- (void)webViewDidFinishLoad:(UIWebView *)webView;
- (void)webView:(UIWebView *)webView didFailLoadWithError:(NSError *)error;
- (void)leftGesture;
- (void)rightGesture;
- (UIImage *)takeSnapshotOfView:(UIView *)view;
- (void)setUIWebViewControl:(DAVA::UIWebView*) uiWebControl;
- (void)onExecuteJScript:(NSArray *)result;

@end

@implementation WebViewURLDelegate

- (id)init
{
	self = [super init];
	if (self)
	{
		delegate = NULL;
		webView = NULL;
	}
	return self;
}

- (void)leftGesture
{
    if (delegate)
    {
        delegate->SwipeGesture(true);
    }
}

- (void)rightGesture
{
    if (delegate)
    {
        delegate->SwipeGesture(false);
    }
}

- (void)setDelegate:(DAVA::IUIWebViewDelegate *)d andWebView:(DAVA::UIWebView *)w
{
	if (d && w)
	{
		delegate = d;
		webView = w;
	}
}

- (BOOL)webView:(UIWebView *)webView shouldStartLoadWithRequest:(NSURLRequest *)request navigationType:(UIWebViewNavigationType)navigationType
{
	BOOL process = YES;
	
	if (delegate && self->webView)
	{
		NSString* url = [[request URL] absoluteString];
		
		if (url)
		{
		    bool isRedirectedByMouseClick = navigationType == UIWebViewNavigationTypeLinkClicked;
			DAVA::IUIWebViewDelegate::eAction action = delegate->URLChanged(self->webView, [url UTF8String], isRedirectedByMouseClick);
			
			switch (action) {
				case DAVA::IUIWebViewDelegate::PROCESS_IN_WEBVIEW:
					DAVA::Logger::FrameworkDebug("PROCESS_IN_WEBVIEW");
					process = YES;
					break;
					
				case DAVA::IUIWebViewDelegate::PROCESS_IN_SYSTEM_BROWSER:
					DAVA::Logger::FrameworkDebug("PROCESS_IN_SYSTEM_BROWSER");
					[[UIApplication sharedApplication] openURL:[request URL]];
					process = NO;
					break;
					
				case DAVA::IUIWebViewDelegate::NO_PROCESS:
					DAVA::Logger::FrameworkDebug("NO_PROCESS");
					
				default:
					process = NO;
					break;
			}
		}
	}
	
	return process;
}


- (void)webViewDidFinishLoad:(UIWebView *)webViewParam
{
    // TODO render to image test
    UIImage* image = [self takeSnapshotOfView:webViewParam];
    DVASSERT(image);
    
    // First get the image into your data buffer
    CGImageRef imageRef = [image CGImage];
    DAVA::int32 width = static_cast<DAVA::int32>(CGImageGetWidth(imageRef));
    DAVA::int32 height = static_cast<DAVA::int32>(CGImageGetHeight(imageRef));
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    
    DAVA::Image* imageRGB = DAVA::Image::Create(width, height, DAVA::FORMAT_RGBA8888);
    DVASSERT(imageRGB);
    DAVA::uint8 *rawData = const_cast<DAVA::uint8*>(imageRGB->GetData()); // (unsigned char*) calloc(height * width * 4, sizeof(unsigned char));
    NSUInteger bytesPerPixel = 4;
    NSUInteger bytesPerRow = bytesPerPixel * width;
    NSUInteger bitsPerComponent = 8;
    CGContextRef context = CGBitmapContextCreate(rawData, width, height,
                    bitsPerComponent, bytesPerRow, colorSpace,
                    kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big);
    CGColorSpaceRelease(colorSpace);

    CGContextDrawImage(context, CGRectMake(0, 0, width, height), imageRef);
    CGContextRelease(context);

    DAVA::Texture* tex = DAVA::Texture::CreateFromData(imageRGB, false);
    DAVA::Rect rect = webView->GetRect();
    DAVA::Vector2 spriteSize(rect.dx, rect.dy);
    DAVA::Vector2 textureSize(width, height);
    DAVA::Sprite* spr = DAVA::Sprite::CreateFromTexture(textureSize, tex, DAVA::Vector2(0, 0), spriteSize);
    DVASSERT(spr);
    
    // TODO set webView
    
    webView->GetBackground()->SetSprite(spr, 0);
    
    NSArray * paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString * basePath = ([paths count] > 0) ? [paths objectAtIndex:0] : nil;

    NSData * binaryImageData = UIImagePNGRepresentation(image);

    [binaryImageData writeToFile:[basePath stringByAppendingPathComponent:@"test_outimage.png"] atomically:YES];

    
    if (delegate && self->webView)
	{
        delegate->PageLoaded(self->webView);
    }
}

- (void)webView:(UIWebView *)webView didFailLoadWithError:(NSError *)error
{
    if (delegate && self->webView)
	{
        delegate->PageLoaded(self->webView);
    }
}

- (UIImage *)takeSnapshotOfView:(UIView *)view
{
    CFTimeInterval startTime = CACurrentMediaTime();
    CGFloat contentScaleFactor = 2.f;//[view contentScaleFactor];
    
    size_t w = view.frame.size.width * contentScaleFactor;
    size_t h = view.frame.size.height * contentScaleFactor;
    
//#define TEST_ON 1
#ifdef TEST_ON
    [view.layer setNeedsDisplay];
    UIGraphicsBeginImageContext(CGSizeMake(w, h));
    CGContextRef ctx = UIGraphicsGetCurrentContext();
    [view.layer renderInContext:ctx];
#else
    UIGraphicsBeginImageContext(CGSizeMake(w, h));
    [view drawViewHierarchyInRect:CGRectMake(0, 0, w, h) afterScreenUpdates:YES];
#endif
    UIImage *image = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    
    CFTimeInterval elapsedTime = CACurrentMediaTime() - startTime;
    
    NSLog(@"time to render webview to image = %.4lfs", elapsedTime);
    
    return image;
}

- (void)onExecuteJScript:(NSArray *)result
{
    if (delegate)
    {
        NSNumber* requestId = (NSNumber*)[result objectAtIndex:0];
        NSString* requestResult = (NSString*)[result objectAtIndex:1];
        delegate->OnExecuteJScript(webView, [requestId intValue], DAVA::String([requestResult UTF8String]));
    }
    [result release];
}

- (void)setUIWebViewControl:(DAVA::UIWebView*) uiWebControl
{
    webView = uiWebControl;
}

@end

DAVA::WebViewControl::WebViewControl(DAVA::UIWebView& uiWeb):
    webViewPtr(0),
    webViewDelegatePtr(0),
    webViewURLDelegatePtr(0),
    rightSwipeGesturePtr(0),
    leftSwipeGesturePtr(0),
    gesturesEnabled(false),
    isRenderToTexture(false),
    isVisible(true),
    uiWebView(uiWeb)
{
    HelperAppDelegate* appDelegate = [[UIApplication sharedApplication] delegate];
    BackgroundView* backgroundView = [appDelegate glController].backgroundView;
    
    ::UIWebView* localWebView = [backgroundView CreateWebView];
    webViewPtr = localWebView;
    
    CGRect emptyRect = CGRectMake(0.0f, 0.0f, 0.0f, 0.0f);
    [localWebView setFrame:emptyRect];
    
    SetBounces(false);
    
    webViewURLDelegatePtr = [[WebViewURLDelegate alloc] init];
    [localWebView setDelegate:(WebViewURLDelegate*)webViewURLDelegatePtr];
    [(WebViewURLDelegate*)webViewURLDelegatePtr setUIWebViewControl:&uiWebView];
    
    [localWebView becomeFirstResponder];
}

namespace DAVA
{
	typedef DAVA::UIWebView DAVAWebView;

	//Use unqualified UIWebView and UIScreen from global namespace, i.e. from UIKit
	using ::UIWebView;
	using ::UIScreen;

int WebViewControl::runScriptID = 0;

    static const struct
    {
        DAVAWebView::eDataDetectorType davaDetectorType;
        NSUInteger systemDetectorType;
    }
    detectorsMap[] =
    {
        {DAVAWebView::DATA_DETECTOR_ALL, UIDataDetectorTypeAll},
        {DAVAWebView::DATA_DETECTOR_NONE, UIDataDetectorTypeNone},
        {DAVAWebView::DATA_DETECTOR_PHONE_NUMBERS, UIDataDetectorTypePhoneNumber},
        {DAVAWebView::DATA_DETECTOR_LINKS, UIDataDetectorTypeLink},
        {DAVAWebView::DATA_DETECTOR_ADDRESSES, UIDataDetectorTypeAddress},
        {DAVAWebView::DATA_DETECTOR_CALENDAR_EVENTS, UIDataDetectorTypeCalendarEvent}
    };



WebViewControl::~WebViewControl()
{
    SetGestures(NO);
	UIWebView* innerWebView = (UIWebView*)webViewPtr;

    
    [innerWebView setDelegate:nil];
    [innerWebView stopLoading];
    [innerWebView loadHTMLString:@"" baseURL:nil];
    
    [innerWebView resignFirstResponder];

    
    HelperAppDelegate* appDelegate = [[UIApplication sharedApplication] delegate];
    BackgroundView* backgroundView = [appDelegate glController].backgroundView;
    [backgroundView ReleaseWebView:innerWebView];
    
	webViewPtr = nil;

	WebViewURLDelegate* w = (WebViewURLDelegate*)webViewURLDelegatePtr;
	[w release];
	webViewURLDelegatePtr = nil;

	RestoreSubviewImages();
}
	
void WebViewControl::SetDelegate(IUIWebViewDelegate *delegate, DAVAWebView* webView)
{
	WebViewURLDelegate* w = (WebViewURLDelegate*)webViewURLDelegatePtr;
	[w setDelegate:delegate andWebView:webView];
}

void WebViewControl::Initialize(const Rect& rect)
{
	SetRect(rect);
}

// Open the URL requested.
void WebViewControl::OpenURL(const String& urlToOpen)
{
	NSString* nsURLPathToOpen = [NSString stringWithUTF8String:urlToOpen.c_str()];
	NSURL* url = [NSURL URLWithString:[nsURLPathToOpen stringByAddingPercentEscapesUsingEncoding: NSUTF8StringEncoding]];
	
	NSURLRequest* requestObj = [NSURLRequest requestWithURL:url];
    UIWebView* innerWebView = (UIWebView*)webViewPtr;
    [innerWebView stopLoading];
    [innerWebView loadRequest:requestObj];
}
    
void WebViewControl::OpenFromBuffer(const String& string, const FilePath& basePath)
{
	NSString* dataToOpen = [NSString stringWithUTF8String:string.c_str()];
    NSString* baseUrl = [NSString stringWithUTF8String:basePath.AsURL().c_str()];
    
    UIWebView* innerWebView = (UIWebView*)webViewPtr;
    [innerWebView stopLoading];
    
    [innerWebView loadHTMLString:dataToOpen baseURL:[NSURL URLWithString:baseUrl]];
}

void WebViewControl::LoadHtmlString(const WideString& htlmString)
{
	NSString* htmlPageToLoad = [[[NSString alloc] initWithBytes: htlmString.data()
												   length: htlmString.size() * sizeof(wchar_t)
												 encoding:NSUTF32LittleEndianStringEncoding] autorelease];

    [(UIWebView*)webViewPtr loadHTMLString:htmlPageToLoad baseURL:nil];
}

void WebViewControl::DeleteCookies(const String& targetUrl)
{
	NSString *targetUrlString = [NSString stringWithUTF8String:targetUrl.c_str()];
	NSHTTPCookieStorage* cookies = [NSHTTPCookieStorage sharedHTTPCookieStorage];
	// Delete all cookies for specified URL
	for(NSHTTPCookie *cookie in [cookies cookies])
	{
		if([[cookie domain] rangeOfString:targetUrlString].location != NSNotFound)
	  	{
       		[cookies deleteCookie:cookie];
   	 	}
	}
	// Syncronized all changes with file system
	[[NSUserDefaults standardUserDefaults] synchronize];
}

String WebViewControl::GetCookie(const String& targetUrl, const String& name) const
{
	Map<String, String> cookiesMap = GetCookies(targetUrl);
	Map<String, String>::iterator cIter = cookiesMap.find(name);
	
	if (cIter != cookiesMap.end())
	{
		return cIter->second;
	}

	return String();
}

Map<String, String> WebViewControl::GetCookies(const String& targetUrl) const
{
	Map<String, String> resultMap;
	
	NSString *targetUrlString = [NSString stringWithUTF8String:targetUrl.c_str()];
    NSArray  *cookiesArray = [[NSHTTPCookieStorage sharedHTTPCookieStorage] cookiesForURL: [NSURL URLWithString:targetUrlString]];

	for(NSHTTPCookie * cookie in cookiesArray)
	{
		String cookieName = [[cookie name] UTF8String];
		resultMap[cookieName] = [[cookie value] UTF8String];
    }
	
	return resultMap;
}

int32 WebViewControl::ExecuteJScript(const String& scriptString)
{
    int requestID = runScriptID++;
	NSString *jScriptString = [NSString stringWithUTF8String:scriptString.c_str()];
	NSString *resultString = [(UIWebView*)webViewPtr stringByEvaluatingJavaScriptFromString:jScriptString];

    WebViewURLDelegate* w = (WebViewURLDelegate*)webViewURLDelegatePtr;
    if (w)
    {
        NSArray* array = [NSArray arrayWithObjects:[NSNumber numberWithInt:requestID], resultString, nil];
        [array retain];
        [w performSelector:@selector(onExecuteJScript:) withObject:array afterDelay:0.0];
    }
    return requestID;
}

void WebViewControl::SetRect(const Rect& rect)
{
	CGRect webViewRect = [(UIWebView*)webViewPtr frame];

    VirtualCoordinatesSystem& VCS = *VirtualCoordinatesSystem::Instance();
    
    Rect physicalRect = VCS.ConvertVirtualToPhysical(rect);
    
    if (isRenderToTexture)
    {
        webViewRect.origin.x = physicalRect.x + VCS.GetPhysicalDrawOffset().x
                                                                - 10000;
        webViewRect.origin.y = physicalRect.y + VCS.GetPhysicalDrawOffset().y
                                                                - 10000;
    }
    else
    {
        webViewRect.origin.x = physicalRect.x + VCS.GetPhysicalDrawOffset().x;
        webViewRect.origin.y = physicalRect.y + VCS.GetPhysicalDrawOffset().y;
    }
    webViewRect.size.width = physicalRect.dx;
    webViewRect.size.height = physicalRect.dy;
	
	// Apply the Retina scale divider, if any.
    DAVA::float32 scaleDivider = [HelperAppDelegate GetScale];
	webViewRect.origin.x /= scaleDivider;
	webViewRect.origin.y /= scaleDivider;
	webViewRect.size.height /= scaleDivider;
	webViewRect.size.width /= scaleDivider;

	[(UIWebView*)webViewPtr setFrame: webViewRect];
}

void WebViewControl::SetVisible(bool isVisible, bool hierarchic)
{
    this->isVisible = isVisible;
    
    if (!isRenderToTexture)
    {
        [(UIWebView*)webViewPtr setHidden:!isVisible];
    } else
    {
        DAVA::Rect r = uiWebView.GetRect();
        SetRect(r);
    }
}

void WebViewControl::SetScalesPageToFit(bool isScalesToFit)
{
	[(UIWebView*)webViewPtr setScalesPageToFit:isScalesToFit];
}

void WebViewControl::SetBackgroundTransparency(bool enabled)
{
	UIWebView* webView = (UIWebView*)webViewPtr;
	[webView setOpaque: (enabled ? NO : YES)];

	UIColor* color = [webView backgroundColor];
	CGFloat r, g, b, a;
	[color getRed:&r green:&g blue:&b alpha:&a];

	if (enabled)
	{
		[webView setBackgroundColor:[UIColor colorWithRed:r green:g blue:b
                                                    alpha:0.f]];
		HideSubviewImages(webView);
	}
	else
	{
		[webView setBackgroundColor:[UIColor colorWithRed:r green:g blue:b
                                                    alpha:1.0f]];
		RestoreSubviewImages();
	}
}

void WebViewControl::HideSubviewImages(void* view)
{
    UIWebView* webView = (UIWebView*)webViewPtr;
    ::UIScrollView *scrollView = webView.scrollView;
    
	UIView* uiview = (UIView*)view;
	for (UIView* subview in [uiview subviews])
	{
        if(uiview == scrollView)
            continue;
        
		if ([subview isKindOfClass:[UIImageView class]])
		{
			subviewVisibilityMap[subview] = [subview isHidden];
			[subview setHidden:YES];
			[subview retain];
		}
		HideSubviewImages(subview);
	}
}

void WebViewControl::RestoreSubviewImages()
{
	Map<void*, bool>::iterator it;
	for (it = subviewVisibilityMap.begin(); it != subviewVisibilityMap.end(); ++it)
	{
		UIView* view = (UIView*)it->first;
		[view setHidden:it->second];
		[view release];
	}
	subviewVisibilityMap.clear();
}

bool WebViewControl::GetBounces() const
{
	if (!webViewPtr)
	{
		return false;
	}

	UIWebView* localWebView = (UIWebView*)webViewPtr;
	return (localWebView.scrollView.bounces == YES);
}
	
void WebViewControl::SetBounces(bool value)
{
	UIWebView* localWebView = (UIWebView*)webViewPtr;
	localWebView.scrollView.bounces = (value == true);
}

//for iOS we need use techique like http://stackoverflow.com/questions/12578895/how-to-detect-a-swipe-gesture-on-webview
void WebViewControl::SetGestures(bool value)
{
    HelperAppDelegate* appDelegate = [[UIApplication sharedApplication] delegate];
    UIView * backView = appDelegate.glController.backgroundView;

    if (value && !gesturesEnabled)
    {
        WebViewURLDelegate * urlDelegate = (WebViewURLDelegate *)webViewURLDelegatePtr;
        
        UISwipeGestureRecognizer * rightSwipeGesture = [[UISwipeGestureRecognizer alloc] initWithTarget:urlDelegate action:@selector(rightGesture)];
        UISwipeGestureRecognizer * leftSwipeGesture = [[UISwipeGestureRecognizer alloc] initWithTarget:urlDelegate action:@selector(leftGesture)];
        rightSwipeGesture.direction = UISwipeGestureRecognizerDirectionRight;
        leftSwipeGesture.direction = UISwipeGestureRecognizerDirectionLeft;
        
        [backView addGestureRecognizer:rightSwipeGesture];
        [backView addGestureRecognizer:leftSwipeGesture];
        
        UIWebView* localWebView = (UIWebView*)webViewPtr;
        [localWebView.scrollView.panGestureRecognizer requireGestureRecognizerToFail:rightSwipeGesture];
        [localWebView.scrollView.panGestureRecognizer requireGestureRecognizerToFail:leftSwipeGesture];
        rightSwipeGesturePtr = rightSwipeGesture;
        leftSwipeGesturePtr = leftSwipeGesture;
    }
    else if (!value && gesturesEnabled)
    {
        UISwipeGestureRecognizer *rightSwipeGesture = (UISwipeGestureRecognizer *)rightSwipeGesturePtr;
        UISwipeGestureRecognizer *leftSwipeGesture = (UISwipeGestureRecognizer *)leftSwipeGesturePtr;
        
        [backView removeGestureRecognizer:rightSwipeGesture];
        [backView removeGestureRecognizer:leftSwipeGesture];
        [rightSwipeGesture release];
        [leftSwipeGesture release];
        rightSwipeGesturePtr = nil;
        leftSwipeGesturePtr = nil;
    }
    gesturesEnabled = value;
}

void WebViewControl::SetDataDetectorTypes(int32 value)
{
    NSUInteger systemDetectorTypes = 0;

    int detectorsCount = COUNT_OF(detectorsMap);
    for (int i = 0; i < detectorsCount; i ++)
    {
        if ((value & detectorsMap[i].davaDetectorType) == detectorsMap[i].davaDetectorType)
        {
            systemDetectorTypes |= detectorsMap[i].systemDetectorType;
        }
    }

    UIWebView* localWebView = (UIWebView*)webViewPtr;
    localWebView.dataDetectorTypes = systemDetectorTypes;
}

int32 WebViewControl::GetDataDetectorTypes() const
{
    UIWebView* localWebView = (UIWebView*)webViewPtr;
    NSUInteger systemDetectorTypes = localWebView.dataDetectorTypes;

    int32 davaDetectorTypes = 0;
    
    int detectorsCount = COUNT_OF(detectorsMap);
    for (int i = 0; i < detectorsCount; i ++)
    {
        if ((systemDetectorTypes & detectorsMap[i].systemDetectorType) == detectorsMap[i].systemDetectorType)
        {
            davaDetectorTypes |= detectorsMap[i].davaDetectorType;
        }
    }

    return davaDetectorTypes;
}
    
void WebViewControl::SetRenderToTexture(bool value)
{
    isRenderToTexture = value;
    
    // hide windows - move to x=-10000, y=-10000 position
    // so it still can render WebView into
    DAVA::Rect r = uiWebView.GetRect();
    SetRect(r);
}

bool WebViewControl::IsRenderToTexture() const
{
    return isRenderToTexture;
}
    
} // end namespace DAVA
