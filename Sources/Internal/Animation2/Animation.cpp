#include "Animation.h"

Animation::Animation()
{
}

bool Animation::Bind(const Skeleton& target, const AnimationClip& anim)
{
    return false;
}

void Animation::Reset()
{
}

void Animation::Advance(float dt_s)
{
}

void Animation::GetPose(AnimationPose* pose) const
{
}

void Animation::GetPosePartial(AnimationPose* pose) const
{
}
