#pragma once

#include "EditorSystems/Private/DistanceLines.h"

#include <Base/BaseTypes.h>
#include <Math/Rect.h>
#include <UI/UIGeometricData.h>

namespace DAVA
{
namespace TArc
{
class ContextAccessor;
}
}

class LinesFactory
{
public:
    virtual DAVA::Vector<std::unique_ptr<DistanceLine>> CreateLines() const = 0;
};

//factory to create distance lines between two controls
class ControlsLinesFactory : public LinesFactory
{
public:
    struct ControlLinesFactoryParams
    {
        DAVA::TArc::ContextAccessor* accessor = nullptr;
        DAVA::RefPtr<DAVA::Font> font;
        DAVA::UIControl* selectedControl = nullptr;
        DAVA::UIControl* highlightedControl = nullptr;
    };

    ControlsLinesFactory(const ControlLinesFactoryParams& params);

private:
    DAVA::Vector<std::unique_ptr<DistanceLine>> CreateLines() const override;

    SolidLine::SolidLineParams CreateParams(const DAVA::Vector2& startPoint, const DAVA::Vector2& endPos, DAVA::eAlign direction) const;
    void AddSolidLine(DAVA::Vector2::eAxis axis, const DAVA::Vector2& startPos, const DAVA::Vector2& endPos, DAVA::Vector<std::unique_ptr<DistanceLine>>& lines) const;

    DAVA::TArc::ContextAccessor* accessor = nullptr;
    DAVA::RefPtr<DAVA::Font> font;
    DAVA::Rect selectedRect;
    DAVA::Rect highlightedRect;
    DAVA::UIGeometricData parentGd;
};
