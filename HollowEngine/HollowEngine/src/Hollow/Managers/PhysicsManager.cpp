#include <hollowpch.h>
#include "PhysicsManager.h"
#include "InputManager.h"

#include "DebugDrawManager.h"

#include "Hollow/Components/Transform.h"
#include "Hollow/Physics/NarrowPhase/SAT.h"
#include "Hollow/Physics/Broadphase/DynamicAABBTree.h"
#include "Hollow/Components/Camera.h"

#include "Utils/RayCast.h"
#include <stack>
#include "RenderManager.h"

namespace Hollow {

	void PhysicsManager::ApplyImpulse(GameObject* object, glm::vec3 impulse)
	{
		Body* pBody = object->GetComponent<Body>();

		pBody->mVelocity += impulse / pBody->mMass;
		// TODO should also have some angular component to it
		// depending on the axis the impulse was applied on
	}
	
	Collider* PhysicsManager::castRay()
	{
		glm::ivec2 mouseXY = InputManager::Instance().GetMousePosition();
		float x = 2.0f * (mouseXY.x + 0.5f) / 1280.0f - 1.0f,
			y = 1.0f - 2.0f * (mouseXY.y + 0.5f) / 720.0f;

		auto& camera = RenderManager::Instance().mMainCamera;
		
		Ray r;
		r.origin = camera.mEyePosition; // -mRayCastCamera->mFront;

		// NDC to camera space
		glm::vec4 ray_clip = glm::vec4(x, y, -1.0f, 1.0f);
		glm::vec4 ray_eye = glm::inverse(camera.mProjectionMatrix) * ray_clip;

		// camera to world space
		ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);
		glm::vec3 direction = glm::vec3(glm::inverse(camera.mViewMatrix) * ray_eye);

		direction = glm::normalize(direction);

		r.direction = direction;

		DebugDrawManager::Instance().DebugRay(r.origin, r.direction);

		IntersectionData id, closest;
		closest.object = nullptr;
		closest.depth = std::numeric_limits<float>::max();

		// simple but inefficient solution
		/*for (int i = 0; i < mGameObjects.size(); ++i) {
			Shape* shape = mGameObjects[i]->GetComponent<Collider>()->mpShape;
			glm::mat3& rot = mGameObjects[i]->GetComponent<Body>()->mRotationMatrix;
			glm::vec3 extents = mGameObjects[i]->GetComponent<Transform>()->mScale;

			if (shape->TestRay(r, id, rot, extents)) {
				if (id.depth < closest.depth) {
					closest = id;
				}
			}
		}*/

		Node* root = mTree.GetRoot();

		std::stack<Node*> s;
		Node* curr = root;

		// inorder traversal for printing
		while ((curr != NULL && curr->aabb->TestRay(r, id)) || s.empty() == false)
		{

			while (curr != NULL && curr->aabb->TestRay(r, id))
			{
				s.push(curr);
				curr = curr->left;
			}

			curr = s.top();
			s.pop();

			if (curr->IsLeaf())
			{
				// cannot use curr->aabb because the mpOwnerCollider in the shape would always be null
				Shape* shape = static_cast<Collider*>(curr->mClientData)->mpShape;

				glm::mat3& rot = static_cast<Collider*>(curr->mClientData)->mpBody->mRotationMatrix;
				glm::vec3 extents = static_cast<Collider*>(curr->mClientData)->mpTr->mScale;
				if (shape->TestRay(r, id, rot, extents)) {
					if (id.depth < closest.depth) {
						closest = id;
					}
				}
			}

			curr = curr->right;
		}

		if (closest.object == nullptr) { return nullptr; }

		return static_cast<Collider*>(closest.object);
	
	}
}