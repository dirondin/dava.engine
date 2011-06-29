/*
* Copyright (c) 2006-2007 Erin Catto http://www.gphysics.com
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#ifndef B2_FRICTION_JOINT_H
#define B2_FRICTION_JOINT_H

#include <Box2D/Dynamics/Joints/b2Joint.h>

/// Friction joint definition.
struct b2FrictionJointDef : public b2JointDef
{
	b2FrictionJointDef()
	{
		type = e_frictionJoint;
		localAnchorA.SetZero();
		localAnchorB.SetZero();
		maxForce = 0.0f;
		maxTorque = 0.0f;
	}

	/// Initialize the bodies, anchors, axis, and reference angle using the world
	/// anchor and world axis.
	void Initialize(b2Body* bodyA, b2Body* bodyB, const b2Vec2& anchor);

	/// The local anchor point relative to bodyA's origin.
	b2Vec2 localAnchorA;

	/// The local anchor point relative to bodyB's origin.
	b2Vec2 localAnchorB;

	/// The maximum friction force in N.
	b2_float32 maxForce;

	/// The maximum friction torque in N-m.
	b2_float32 maxTorque;
};

/// Friction joint. This is used for top-down friction.
/// It provides 2D translational friction and angular friction.
class b2FrictionJoint : public b2Joint
{
public:
	b2Vec2 GetAnchorA() const;
	b2Vec2 GetAnchorB() const;

	b2Vec2 GetReactionForce(b2_float32 inv_dt) const;
	b2_float32 GetReactionTorque(b2_float32 inv_dt) const;

	/// Set the maximum friction force in N.
	void SetMaxForce(b2_float32 force);

	/// Get the maximum friction force in N.
	b2_float32 GetMaxForce() const;

	/// Set the maximum friction torque in N*m.
	void SetMaxTorque(b2_float32 torque);

	/// Get the maximum friction torque in N*m.
	b2_float32 GetMaxTorque() const;

protected:

	friend class b2Joint;

	b2FrictionJoint(const b2FrictionJointDef* def);

	void InitVelocityConstraints(const b2TimeStep& step);
	void SolveVelocityConstraints(const b2TimeStep& step);
	bool SolvePositionConstraints(b2_float32 baumgarte);

	b2Vec2 m_localAnchorA;
	b2Vec2 m_localAnchorB;

	b2Mat22 m_linearMass;
	b2_float32 m_angularMass;

	b2Vec2 m_linearImpulse;
	b2_float32 m_angularImpulse;

	b2_float32 m_maxForce;
	b2_float32 m_maxTorque;
};

#endif
