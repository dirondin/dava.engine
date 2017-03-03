#include "TArc/Controls/PropertyPanel/Private/BoolComponentValue.h"
#include "TArc/Controls/CheckBox.h"
#include "TArc/Controls/PropertyPanel/PropertyModelExtensions.h"

#include <Reflection/ReflectionRegistrator.h>
#include <Reflection/ReflectedMeta.h>

namespace DAVA
{
namespace TArc
{
Qt::CheckState BoolComponentValue::GetCheckState() const
{
    return GetValue().Cast<Qt::CheckState>();
}

void BoolComponentValue::SetCheckState(Qt::CheckState checkState)
{
    SetValue(checkState);
}

Any BoolComponentValue::GetMultipleValue() const
{
    return Any(Qt::PartiallyChecked);
}

bool BoolComponentValue::IsValidValueToSet(const Any& newValue, const Any& currentValue) const
{
    Qt::CheckState newCheckedState = newValue.Cast<Qt::CheckState>(Qt::PartiallyChecked);
    Qt::CheckState currentCheckedState = currentValue.Cast<Qt::CheckState>(Qt::PartiallyChecked);
    return newCheckedState != Qt::PartiallyChecked && newCheckedState != currentCheckedState;
}

ControlProxy* BoolComponentValue::CreateEditorWidget(QWidget* parent, const Reflection& model, DataWrappersProcessor* wrappersProcessor) const
{
    ControlDescriptorBuilder<CheckBox::Fields> descr;
    descr[CheckBox::Fields::Checked] = "bool";
    descr[CheckBox::Fields::TextHint] = "textHint";
    descr[CheckBox::Fields::IsReadOnly] = readOnlyFieldName;
    return new CheckBox(descr, wrappersProcessor, model, parent);
}

String BoolComponentValue::GetTextHint() const
{
    String result;
    Any value = GetValue();
    const M::ValueDescription* description = nodes.front()->field.ref.GetMeta<M::ValueDescription>();
    if (description != nullptr)
    {
        Qt::CheckState state = GetValue().Cast<Qt::CheckState>();
        if (state == Qt::PartiallyChecked)
        {
            result = "< multiple values >";
        }
        else
        {
            result = description->GetDescription(value);
        }
    }

    return result;
}

DAVA_VIRTUAL_REFLECTION_IMPL(BoolComponentValue)
{
    ReflectionRegistrator<BoolComponentValue>::Begin()
    .Field("bool", &BoolComponentValue::GetCheckState, &BoolComponentValue::SetCheckState)
    .Field("textHint", &BoolComponentValue::GetTextHint, nullptr)
    .End();
}
}
}
