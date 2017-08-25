#pragma once

#include <Base/RefPtr.h>
#include <UI/UIControl.h>
#include <Math/Color.h>

namespace UIControlUtils
{
DAVA::RefPtr<DAVA::UIControl> CreateLineWithColor(const DAVA::Color& color, const DAVA::String& name);
void MapToScreen(DAVA::Rect localRect, const DAVA::UIGeometricData& gd, const DAVA::RefPtr<DAVA::UIControl>& control);
}
