#include "ArrowsNode.h"

#include "EditorScene.h"
#include "Render/Highlevel/RenderFastNames.h"

#define RED Color(1.f, 0.f, 0.f, 1.f)
#define GREEN Color(0.f, 1.f, 0.f, 1.f)
#define BLUE Color(0.f, 0.f, 1.f, 1.f)
#define YELLOW Color(1.f, 1.f, 0.f, 1.f)
#define GRAY Color(0.5f, 0.5f, 0.5f, 1.f)

#define SELECTED_COLOR YELLOW
#define INACTIVE_COLOR GRAY

#define AXIS_HALF_LENGTH (2.5f * scaleFactor)
#define AXIS_HALF_WIDTH (0.2f * scaleFactor)
#define PLANE_HALF_LENGTH (1.f * scaleFactor)
#define PLANE_HALF_WIDTH (0.2f * scaleFactor)
#define ARROW_HEIGHT (AXIS_HALF_LENGTH * 0.3f)
#define ARROW_WIDTH (AXIS_HALF_LENGTH * 0.15f)

ArrowsNode::ArrowsNode():
	position(Vector3(0.f, 0.f, 0.f)),
	selected(AXIS_NONE),
	visible(false),
	active(false),
	scaleFactor(1.f)
{
	collisionConfiguration = new btDefaultCollisionConfiguration();
	collisionDispatcher = new btCollisionDispatcher(collisionConfiguration);
	axisSweep = new btAxisSweep3(btVector3(-1000, -1000, -1000),
								 btVector3( 1000,  1000,  1000));
	collisionWorld = new btCollisionWorld(collisionDispatcher, axisSweep, collisionConfiguration);

	RenderComponent* renderComponent = new RenderComponent();
	renderComponent->SetRenderObject(new ArrowsRenderObject(this));
	AddComponent(renderComponent);
}

ArrowsNode::~ArrowsNode()
{
	delete collisionConfiguration;
	delete collisionDispatcher;
	delete axisSweep;
	delete collisionWorld;
}

void ArrowsNode::PrepareColors(Color *colors)
{
	if (!IsActive())
	{
		for (uint32 i = COLORS_COUNT; i; --i)
			*colors++ = INACTIVE_COLOR;
	}
	else
	{
		Color defAxisColors[COLORS_COUNT] = {
			RED,		//X
			GREEN,		//Y
			BLUE,		//Z
			RED,		//XY_X
			GREEN,		//XY_Y
			GREEN,		//YZ_Y
			BLUE,		//YZ_Z
			RED,		//XZ_X
			BLUE		//XZ_Z
		};

		for (uint32 i = 0; i < COLORS_COUNT; ++i)
			colors[i] = defAxisColors[i];

		switch (selected)
		{
			case AXIS_X:
				colors[COLOR_X] = SELECTED_COLOR;
				break;

			case AXIS_Y:
				colors[COLOR_Y] = SELECTED_COLOR;
				break;

			case AXIS_Z:
				colors[COLOR_Z] = SELECTED_COLOR;
				break;

			case AXIS_XY:
				colors[COLOR_X] = SELECTED_COLOR;
				colors[COLOR_Y] = SELECTED_COLOR;
				colors[COLOR_XY_X] = SELECTED_COLOR;
				colors[COLOR_XY_Y] = SELECTED_COLOR;
				break;

			case AXIS_YZ:
				colors[COLOR_Y] = SELECTED_COLOR;
				colors[COLOR_Z] = SELECTED_COLOR;
				colors[COLOR_YZ_Y] = SELECTED_COLOR;
				colors[COLOR_YZ_Z] = SELECTED_COLOR;
				break;

			case AXIS_XZ:
				colors[COLOR_X] = SELECTED_COLOR;
				colors[COLOR_Z] = SELECTED_COLOR;
				colors[COLOR_XZ_X] = SELECTED_COLOR;
				colors[COLOR_XZ_Z] = SELECTED_COLOR;
				break;
		}
	}
}

void ArrowsNode::DrawPrism(const Vector3& p1, const Vector3& p2, const Vector3& p3, const Vector3& p4, const Vector3& p5)
{
	RenderHelper* helper = RenderHelper::Instance();

	Polygon3 poly;

	poly.AddPoint(p1);
	poly.AddPoint(p2);
	poly.AddPoint(p3);
	helper->FillPolygon(poly);

	poly.Clear();
	poly.AddPoint(p1);
	poly.AddPoint(p3);
	poly.AddPoint(p4);
	helper->FillPolygon(poly);

	poly.Clear();
	poly.AddPoint(p1);
	poly.AddPoint(p4);
	poly.AddPoint(p5);
	helper->FillPolygon(poly);

	poly.Clear();
	poly.AddPoint(p1);
	poly.AddPoint(p5);
	poly.AddPoint(p2);
	helper->FillPolygon(poly);

	poly.Clear();
	poly.AddPoint(p2);
	poly.AddPoint(p3);
	poly.AddPoint(p4);
	poly.AddPoint(p5);
	helper->FillPolygon(poly);
}

void ArrowsNode::Draw()
{
	if(!visible)
		return;

	RenderManager* manager = RenderManager::Instance();
	RenderHelper* helper = RenderHelper::Instance();

	uint32 oldState = manager->GetState();
	manager->SetState(RenderStateBlock::STATE_COLORMASK_ALL);

	Color colors[COLORS_COUNT];
	PrepareColors(colors);

	Vector3 p1, p2, p3, p4, p5, p6;

	p1 = p2 = position;
	p1 += Vector3(2 * AXIS_HALF_LENGTH + AXIS_HALF_WIDTH, 0, 0);
	p3 = p4 = p5 = p6 = p1;
	p3 += Vector3(-ARROW_HEIGHT, ARROW_WIDTH, 0);
	p4 += Vector3(-ARROW_HEIGHT, -ARROW_WIDTH, 0);
	p5 += Vector3(-ARROW_HEIGHT, 0, ARROW_WIDTH);
	p6 += Vector3(-ARROW_HEIGHT, 0, -ARROW_WIDTH);
	manager->SetColor(colors[COLOR_X]);
	helper->DrawLine(p1, p2);
	DrawPrism(p1, p3, p5, p4, p6);

	p1 = p2 = position;
	p1 += Vector3(0, 2 * AXIS_HALF_LENGTH + AXIS_HALF_WIDTH, 0);
	p3 = p4 = p5 = p6 = p1;
	p3 += Vector3(ARROW_WIDTH, -ARROW_HEIGHT, 0);
	p4 += Vector3(-ARROW_WIDTH, -ARROW_HEIGHT, 0);
	p5 += Vector3(0, -ARROW_HEIGHT, ARROW_WIDTH);
	p6 += Vector3(0, -ARROW_HEIGHT, -ARROW_WIDTH);
	manager->SetColor(colors[COLOR_Y]);
	helper->DrawLine(p1, p2);
	DrawPrism(p1, p3, p5, p4, p6);

	p1 = p2 = position;
	p1 += Vector3(0, 0, 2 * AXIS_HALF_LENGTH + AXIS_HALF_WIDTH);
	p3 = p4 = p5 = p6 = p1;
	p3 += Vector3(ARROW_WIDTH, 0, -ARROW_HEIGHT);
	p4 += Vector3(-ARROW_WIDTH, 0, -ARROW_HEIGHT);
	p5 += Vector3(0, ARROW_WIDTH, -ARROW_HEIGHT);
	p6 += Vector3(0, -ARROW_WIDTH, -ARROW_HEIGHT);
	manager->SetColor(colors[COLOR_Z]);
	helper->DrawLine(p1, p2);
	DrawPrism(p1, p3, p5, p4, p6);

	p1 = p2 = p3 = position;
	p1 += Vector3(2 * PLANE_HALF_LENGTH + AXIS_HALF_WIDTH, 2 * PLANE_HALF_LENGTH + AXIS_HALF_WIDTH, 0);
	p2 += Vector3(2 * PLANE_HALF_LENGTH + AXIS_HALF_WIDTH, 0, 0);
	p3 += Vector3(0, 2 * PLANE_HALF_LENGTH + AXIS_HALF_WIDTH, 0);
	manager->SetColor(colors[COLOR_XY_X]);
	helper->DrawLine(p1, p2);
	manager->SetColor(colors[COLOR_XY_Y]);
	helper->DrawLine(p1, p3);

	p1 = p2 = p3 = position;
	p1 += Vector3(2 * PLANE_HALF_LENGTH + AXIS_HALF_WIDTH, 0, 2 * PLANE_HALF_LENGTH + AXIS_HALF_WIDTH);
	p2 += Vector3(2 * PLANE_HALF_LENGTH + AXIS_HALF_WIDTH, 0, 0);
	p3 += Vector3(0, 0, 2 * PLANE_HALF_LENGTH + AXIS_HALF_WIDTH);
	manager->SetColor(colors[COLOR_XZ_X]);
	helper->DrawLine(p1, p2);
	manager->SetColor(colors[COLOR_XZ_Z]);
	helper->DrawLine(p1, p3);

	p1 = p2 = p3 = position;
	p1 += Vector3(0, 2 * PLANE_HALF_LENGTH + AXIS_HALF_WIDTH, 2 * PLANE_HALF_LENGTH + AXIS_HALF_WIDTH);
	p2 += Vector3(0, 2 * PLANE_HALF_LENGTH + AXIS_HALF_WIDTH, 0);
	p3 += Vector3(0, 0, 2 * PLANE_HALF_LENGTH + AXIS_HALF_WIDTH);
	manager->SetColor(colors[COLOR_YZ_Y]);
	helper->DrawLine(p1, p2);
	manager->SetColor(colors[COLOR_YZ_Z]);
	helper->DrawLine(p1, p3);

	manager->ResetColor();
	manager->SetState(oldState);
}

void ArrowsNode::ProcessMouse(UIEvent * event, const Vector3& cursorPos, const Vector3& cursorDir)
{
	if (!IsActive() || !IsVisible())
	{
		selected = AXIS_NONE;
		return;
	}

	Vector3 cursorEnd = cursorPos + cursorDir * 1000.0f;

	btBoxShape* xShape = new btBoxShape(btVector3(AXIS_HALF_LENGTH, AXIS_HALF_WIDTH, AXIS_HALF_WIDTH));
	btBoxShape* yShape = new btBoxShape(btVector3(AXIS_HALF_WIDTH, AXIS_HALF_LENGTH, AXIS_HALF_WIDTH));
	btBoxShape* zShape = new btBoxShape(btVector3(AXIS_HALF_WIDTH, AXIS_HALF_WIDTH, AXIS_HALF_LENGTH));
	btBoxShape* xyShape = new btBoxShape(btVector3(PLANE_HALF_LENGTH, PLANE_HALF_LENGTH, PLANE_HALF_WIDTH));
	btBoxShape* xzShape = new btBoxShape(btVector3(PLANE_HALF_LENGTH, PLANE_HALF_WIDTH, PLANE_HALF_LENGTH));
	btBoxShape* yzShape = new btBoxShape(btVector3(PLANE_HALF_WIDTH, PLANE_HALF_LENGTH, PLANE_HALF_LENGTH));

	btCollisionObject* xObject = new btCollisionObject();
	xObject->setCollisionShape(xShape);
	btTransform xTransform;
	xTransform.setIdentity();
	xTransform.setOrigin(btVector3(position.x + AXIS_HALF_LENGTH + AXIS_HALF_WIDTH, position.y, position.z));
	xObject->setWorldTransform(xTransform);

	btCollisionObject* yObject = new btCollisionObject();
	yObject->setCollisionShape(yShape);
	btTransform yTransform;
	yTransform.setIdentity();
	yTransform.setOrigin(btVector3(position.x, position.y + AXIS_HALF_LENGTH + AXIS_HALF_WIDTH, position.z));
	yObject->setWorldTransform(yTransform);

	btCollisionObject* zObject = new btCollisionObject();
	zObject->setCollisionShape(zShape);
	btTransform zTransform;
	zTransform.setIdentity();
	zTransform.setOrigin(btVector3(position.x, position.y, position.z + AXIS_HALF_LENGTH + AXIS_HALF_WIDTH));
	zObject->setWorldTransform(zTransform);

	btCollisionObject* xyObject = new btCollisionObject();
	xyObject->setCollisionShape(xyShape);
	btTransform xyTransform;
	xyTransform.setIdentity();
	xyTransform.setOrigin(btVector3(position.x + PLANE_HALF_LENGTH + AXIS_HALF_WIDTH,
									position.y + PLANE_HALF_LENGTH + AXIS_HALF_WIDTH,
									position.z));
	xyObject->setWorldTransform(xyTransform);

	btCollisionObject* xzObject = new btCollisionObject();
	xzObject->setCollisionShape(xzShape);
	btTransform xzTransform;
	xzTransform.setIdentity();
	xzTransform.setOrigin(btVector3(position.x + PLANE_HALF_LENGTH + AXIS_HALF_WIDTH,
									position.y,
									position.z + PLANE_HALF_LENGTH + AXIS_HALF_WIDTH));
	xzObject->setWorldTransform(xzTransform);

	btCollisionObject* yzObject = new btCollisionObject();
	yzObject->setCollisionShape(yzShape);
	btTransform yzTransform;
	yzTransform.setIdentity();
	yzTransform.setOrigin(btVector3(position.x,
									 position.y + PLANE_HALF_LENGTH + AXIS_HALF_WIDTH,
									 position.z + PLANE_HALF_LENGTH + AXIS_HALF_WIDTH));
	yzObject->setWorldTransform(yzTransform);

	btCollisionObject* objects[] = {
		xObject,
		yObject,
		zObject,
		xyObject,
		yzObject,
		xzObject
	};

	btCollisionShape* shapes[] = {
		xShape,
		yShape,
		zShape,
		xyShape,
		yzShape,
		xzShape
	};

	for(uint32 i = 0; i < AXIS_COUNT; ++i)
	{
		collisionWorld->addCollisionObject(objects[i]);
	}

	btVector3 v1(cursorPos.x, cursorPos.y, cursorPos.z);
	btVector3 v2(cursorEnd.x, cursorEnd.y, cursorEnd.z);
	btVector3 from, to;
	if (InputSystem::Instance()->GetKeyboard()->IsKeyPressed(DVKEY_ALT))
	{
		from = v2;
		to = v1;
	}
	else
	{
		from = v1;
		to = v2;
	}
	btCollisionWorld::ClosestRayResultCallback cb(from, to);
	collisionWorld->rayTest(from, to, cb);

	selected = AXIS_NONE;
	if (cb.hasHit())
	{
		for(selected = AXIS_X; selected < AXIS_COUNT; ++selected)
		{
			if (cb.m_collisionObject == objects[selected])
				break;
		}
	}

	for(uint32 i = 0; i < AXIS_COUNT; ++i)
	{
		collisionWorld->removeCollisionObject(objects[i]);
		delete objects[i];
		delete shapes[i];
	}
}

Vector3 ArrowsNode::GetPosition()
{
	return position;
}

void ArrowsNode::SetPosition(const DAVA::Vector3 &newPosition)
{
	position = newPosition;
}

bool ArrowsNode::IsVisible()
{
	return visible;
}

void ArrowsNode::SetVisible(bool visible)
{
	this->visible = visible;
}

ArrowsNode::eModAxis ArrowsNode::GetModAxis()
{
	return (eModAxis)selected;
}

bool ArrowsNode::IsActive()
{
	return active;
}

void ArrowsNode::SetActive(bool active)
{
	this->active = active;
}

void ArrowsNode::UpdateSize(const Vector3& camPos)
{
	float32 distance = (camPos - position).Length();

	const float32 baseDist = 50.f;

	scaleFactor = Max(1.0f, distance / baseDist);
}


ArrowsRenderObject::ArrowsRenderObject(ArrowsNode* node)
{
	AddRenderBatch(new ArrowsRenderBatch(node));
}


ArrowsRenderBatch::ArrowsRenderBatch(ArrowsNode* node)
:	node(node)
{
    
    SetOwnerLayerName(LAYER_OPAQUE);
}

void ArrowsRenderBatch::Draw(DAVA::Camera *camera)
{
	Matrix4 oldMatrix = RenderManager::Instance()->GetMatrix(RenderManager::MATRIX_MODELVIEW);
	RenderManager::Instance()->SetMatrix(RenderManager::MATRIX_MODELVIEW, camera->GetMatrix());

	if (node)
		node->Draw();

	RenderManager::Instance()->SetMatrix(RenderManager::MATRIX_MODELVIEW, oldMatrix);
}
