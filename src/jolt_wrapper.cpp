#include "jolt_wrapper.h"

JoltWrapper::JoltWrapper()
{
	joltRegister(); // must be first

	temp_allocator_ptr = new TempAllocatorImpl(10 * 1024 * 1024);
	job_system_ptr = new JobSystemThreadPool(cMaxPhysicsJobs, cMaxPhysicsBarriers, 
		thread::hardware_concurrency() - 1);

	ps.Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints, 
		broad_phase_layer_interface, 
		object_vs_broadphase_layer_filter, 
		object_vs_object_layer_filter);
	
	ps.SetBodyActivationListener(&body_activation_listener);

	ps.SetContactListener(&contact_listener);

	interface = &ps.GetBodyInterface();
}

JoltWrapper::~JoltWrapper()
{
	cout << "Deleteing jolt Wrapper.." << endl;
	for (BodyID bodyID : bodyIDs)
    {
		// Remove the shape from the physics system. Note that the sphere itself keeps all of its state and can be re-added at any time.
        interface->RemoveBody(bodyID);
		// Destroy the shape. After this the sphere ID is no longer valid.
		interface->DestroyBody(bodyID);
    }

	joltUnregister();
}

void JoltWrapper::joltRegister()
{
	// Register allocation hook. In this example we'll just let Jolt use malloc / free but you can override these if you want (see Memory.h).
	// This needs to be done before any other Jolt function is called.
	RegisterDefaultAllocator();
    
	// Install trace and assert callbacks
	Trace = TraceImpl;
	JPH_IF_ENABLE_ASSERTS(AssertFailed = AssertFailedImpl;)

	// Create a factory, this class is responsible for creating instances of classes based on their name or hash and is mainly used for deserialization of saved data.
	// It is not directly used in this example but still required.
	Factory::sInstance = new Factory();

	// Register all physics types with the factory and install their collision handlers with the CollisionDispatch class.
	// If you have your own custom shape types you probably need to register their handlers with the CollisionDispatch before calling this function.
	// If you implement your own default material (PhysicsMaterial::sDefault) make sure to initialize it before this function or else this function will create one for you.
	RegisterTypes();
}

void JoltWrapper::joltUnregister()
{
	// Unregisters all types with the factory and cleans up the default material
	UnregisterTypes();

	// Destroy the factory
	delete Factory::sInstance;
	Factory::sInstance = nullptr;
}


void JoltWrapper::update()
{
	// Step the world
	ps.Update(cDeltaTime, cCollisionSteps, temp_allocator_ptr, job_system_ptr);

	int i = 0;
	for (const auto& bodyID : bodyIDs)
    {
		if(interface->IsActive(bodyID))
		{
			matrices[i] = interface->GetWorldTransform(bodyID);
			
			// cout << "updating[" << i << "]" << endl;

			// Check if the object has fallen below the threshold
            if (interface->GetPosition(bodyID).GetY() < cKillThreshold) {
                // cout << "Body ID " << bodyID.GetIndex() << " has fallen below the threshold." << endl;

                // Option 1: Deactivate the body (stop its simulation)
                interface->DeactivateBody(bodyID);

                // Option 2: Remove the body from the simulation
                // interface->RemoveBody(bodyID);

                // Option 3: Reset the object's position to a safe location (e.g., a spawn point)
                // bodyInterface.SetPosition(bodyID, Vec3(0.0f, 10.0f, 0.0f));
                // bodyInterface.SetLinearVelocity(bodyID, Vec3::sZero());  // Reset velocity as well
            }

		}
		i++;
	}
	
}

BodyID JoltWrapper::create_object(RenderPool& render, objectType inType, modelI &inModel, RVec3Arg inPosition, QuatArg inRot)
{
	cout << " created object" << endl;
	BodyID result;

	render.insert(inModel.vertices, inModel.indices, 1);

	// Rotation (quaternion)
    // Quat rotation = Quat::sRotation(Vec3::sAxisY(), DegreesToRadians(45.0f));  // Rotate 45 degrees around the Y axis

    // Create the transformation matrix
	matrices.push_back(RMat44::sRotationTranslation(inRot, inPosition));

	if(inType == enviroment_static)
    {
        result = create_shape(
            new BoxShape(Vec3(1.0f, 1.0f, 1.0f)), false, 
            inPosition, Quat::sIdentity(), 
            EMotionType::Static, 
            Layers::NON_MOVING, 
            EActivation::DontActivate
        );
    }
    else if(inType == enviroment_dynamic)
    {
        result = create_shape(
            new BoxShape(Vec3(1.0f, 1.0f, 1.0f)), false, 
            inPosition, Quat::sIdentity(),  
            EMotionType::Dynamic, 
            Layers::MOVING, 
            EActivation::Activate
        );
        interface->SetRestitution(result, 0.2f);
		interface->SetFriction(result, 4.0f);
    }
    else if(inType == player)
    {
		throw std::runtime_error("trying to make player");
        // result = create_shape(
        //     new CapsuleShape(2.0f, 1.0f), false, 
        //     inPosition, Quat::sIdentity(),  
        //     EMotionType::Kinematic, 
        //     Layers::MOVING, 
        //     EActivation::DontActivate
        // );
    }
    else if(inType == npc)
    {
        
    }

	bodyIDs.push_back(result);

	return result;
}

/// @brief use to create shapes without rendering, collision boxes
/// @param inShape 
/// @param inPosition 
/// @param inRotation 
/// @param inMotionType 
/// @param inObjectLayer 
/// @param inActivation 
/// @return 
BodyID JoltWrapper::create_shape(const Shape *inShape, bool isSensor, RVec3Arg inPosition, QuatArg inRotation, EMotionType inMotionType, ObjectLayer inObjectLayer, EActivation inActivation)
{
	// inShape->SetEmbedded();
	BodyCreationSettings settings(inShape, inPosition, inRotation, inMotionType, inObjectLayer);
    
	settings.mIsSensor = isSensor;
	// settings.mFriction = 10.0f;
	// return get_interface().CreateAndAddBody(settings, inActivation);

	// Create the body in the physics system
	// Body* body = get_interface().CreateBody(settings);
	
	// Add the body to the physics system and activate it
	// get_interface().AddBody(body->GetID(), inActivation);

	BodyID b_id = interface->CreateAndAddBody(settings, inActivation);

	// bodyIDs.push_back(b_id); // no more! use ps.GetBodies() bodyIDs is for rendering simulations
	return b_id;
}

unsigned int JoltWrapper::getBufferSize()
{
    return matrices.size() * sizeof(RMat44);
}

const void *JoltWrapper::getBufferData()
{
    return matrices.data();
}

void JoltWrapper::optimize()
{
	// Optional step: Before starting the physics simulation you can optimize the broad phase. This improves collision detection performance (it's pointless here because we only have 2 bodies).
	// You should definitely not call this every frame or when e.g. streaming in a new level section as it is an expensive operation.
	// Instead insert all new objects in batches instead of 1 at a time to keep the broad phase efficient.
	ps.OptimizeBroadPhase();
}

Vec3 PlayerController::mouseScreenToWorld(vec2 mouse)
{
	// Convert mouse position to NDC
	float ndcX = (2.0f * mouse.x) / pControllerI->windowWidth - 1.0f;
	float ndcY = 1.0f - (2.0f * mouse.y) / pControllerI->windowHeight; // Invert Y-axis

	// Create NDC positions for near and far points (Z = -1 for near, 1 for far)
	glm::vec4 rayClipNear(ndcX, ndcY, -1.0f, 1.0f);
	glm::vec4 rayClipFar(ndcX, ndcY, 1.0f, 1.0f);

	// Convert from clip space to view space
	glm::mat4 inverseProjection = glm::inverse(pCameraI->projection);
	glm::vec4 rayViewNear = inverseProjection * rayClipNear;
	glm::vec4 rayViewFar = inverseProjection * rayClipFar;
	rayViewNear /= rayViewNear.w; // Perspective divide
	rayViewFar /= rayViewFar.w;
	
	// Convert from view space to world space
	glm::mat4 inverseView = glm::inverse(pCameraI->view);
	glm::vec4 rayWorldNear = inverseView * rayViewNear;
	glm::vec4 rayWorldFar = inverseView * rayViewFar;

	// Ray origin is the near point, direction is the vector from near to far
	vec3 rayOrigin = glm::vec3(rayWorldNear);
	vec3 rayDirection = glm::normalize(glm::vec3(rayWorldFar - rayWorldNear));

	/* plane intersection */

	// Plane equation: y = 0
	float t = -rayOrigin.y / rayDirection.y;

	// If t < 0, the intersection is behind the ray origin, so ignore it
	// if (t < 0.0f) {
		// cout << "false" << endl;
	// }

	// Calculate the intersection point
	vec3 intersectionPoint = rayOrigin + t * rayDirection;

	// return vec3(0);
    return v(glm::normalize(intersectionPoint - pCameraI->target));
}

vec2 PlayerController::mouseScreenDirection(vec2 mouse)
{
		// Convert mouse position to NDC
	float ndcX = (2.0f * mouse.x) / pControllerI->windowWidth - 1.0f;
	float ndcY = 1.0f - (2.0f * mouse.y) / pControllerI->windowHeight; // Invert Y-axis

	vec2 n = normalize(vec2(ndcX, ndcY));

    return vec2(roundto(n.x,100),roundto(n.y,100));
}

PlayerController::PlayerController(JoltWrapper *inJolt, Animator *inAnimator, controllerI *inControllerI, cameraI *inCameraI, Vec3 inPosition)
{
	pJolt = inJolt;
	pAnimator = inAnimator;
	pControllerI = inControllerI;
	pCameraI = inCameraI;

	position = inPosition;

	// breaking out create object for player so we can control capsule shape 
	// without changing creat object parameters
	// inRenderPool.insert(inModel.vertices, inModel.indices);
	// pJolt->matrices.push_back(RMat44::sRotationTranslation(Quat::sIdentity(), inPosition));
	bodyID = pJolt->create_shape(
		new SphereShape(cRadius), false, 
		inPosition, Quat::sIdentity(),  
		EMotionType::Kinematic, 
		Layers::MOVING, 
		EActivation::DontActivate
	);
	// removing player body id from jolt wrapper bodyIDs will stop it 
	// from being added to the ssbo update so we can add it to another 
	// ssbo for another shader allowing player to be shaded else where
	// pJolt->bodyIDs.push_back(bodyID);

	// Create an identity matrix (no rotation, no translation)
	// matrices.push_back(Mat44::sTranslation(inPosition));
}

bool PlayerController::cast_ray(Vec3 start, Vec3 end, float *outDepth)
{
	// 	SpecifiedBroadPhaseLayerFilter(BroadPhaseLayers::NON_MOVING):
	//     This filter is checking against a specific broad phase layer. In your case, it's BroadPhaseLayers::NON_MOVING, which likely refers to static objects like the ground or other immovable entities.

	// SpecifiedObjectLayerFilter(Layers::NON_MOVING):
	//     This filter applies to objects in the NON_MOVING layer, meaning it will only consider objects that are categorized under that layer for the raycast.

    // Perform the raycast
    RRayCast ray(start, end - start);
    RayCastResult result;
	
    bool hasHit = pJolt->ps.GetNarrowPhaseQuery().CastRay(ray, result, 
		SpecifiedBroadPhaseLayerFilter(BroadPhaseLayers::NON_MOVING), 
		SpecifiedObjectLayerFilter(Layers::NON_MOVING));

    // Debug output
    if (hasHit) 
	{
		// cast ray feels like it works backwards, have to subtract result from length
		// to get displacement require for corretion

        // cout << "Ray hit! Fraction: " << result.mFraction << endl;
		// *outDepth = abs(result.mFraction) - (end - start).Length();
		
		if(outDepth != nullptr)
		{
			*outDepth = abs(result.mFraction) - (end - start).Length();
		}
		
        return true;// < cGroundDetectionThreshold;
    } 
	else 
	{
        // cout << "Ray did not hit anything." << endl;
        return false;
		
    }
}

Vec3 PlayerController::cast_shape(Vec3 inDirection, Vec3 inPosition)
{
    Vec3 result = Vec3::sZero();

    if(inDirection != Vec3::sZero())
    {
        const Shape* bodyShape = pJolt->interface->GetShape(bodyID);
        RShapeCast shapeCaster(bodyShape, Vec3(1,1,1), Mat44::sTranslation(inPosition), inDirection.Normalized());
        
        // Set up shape cast settings with small penetration tolerance to prevent overlap
        ShapeCastSettings shapeCastSettings;
        MyCastShapeCollector collector;

        // Perform the shape cast
        pJolt->ps.GetNarrowPhaseQuery().CastShape(
            shapeCaster, shapeCastSettings, 
            Vec3::sZero(), collector,
            SpecifiedBroadPhaseLayerFilter(BroadPhaseLayers::NON_MOVING),
            SpecifiedObjectLayerFilter(Layers::NON_MOVING),
			*pJolt->pBodyFilter,
			// BodyFilter(),
			ShapeFilter()
        );

        // Check if any collisions occurred
        if (collector.HasHits())
        {
            for (size_t i = 0; i < collector.mHits.size(); i++) {
                const ShapeCastResult& hitResult = collector.mHits[i];
                
                // Ignore floor collisions by checking the Y component of the penetration axis
                if(hitResult.mPenetrationDepth > 0.01f)
                {
                    // cout << "Hit " << i << ": " << endl;
                    // cout << "  Fraction: " << hitResult.mFraction << endl;
                    // cout << "  Penetration Depth: " << hitResult.mPenetrationDepth << endl;
                    // cout << "  Penetration Axis: " << hitResult.mPenetrationAxis << endl;
                    // cout << "  Contact Point on Shape1: " << hitResult.mContactPointOn1 << endl;
                    // cout << "  Contact Point on Shape2: " << hitResult.mContactPointOn2 << endl;

					// Vec3 horizontal = Vec3(hitResult.mPenetrationAxis.GetX(), 0, hitResult.mPenetrationAxis.GetZ());
                    result += hitResult.mPenetrationDepth * hitResult.mPenetrationAxis.Normalized();
                }
            }
        }
    }

    return result;
}

/// @brief 
/// @param deltaTime
void PlayerController::update(float deltaTime)
{
	// Get player position and adjust for capsule height
	// Adjust this based on your player capsule's half-height

	// Start at player's feet
    Vec3 start = position - Vec3(0.0f, cHalfHeight, 0.0f);  
    Vec3 end = start + Vec3(0.0f, -0.01f, 0.0f);
	
	isGrounded = cast_ray(start, end, nullptr);

	// isGrounded = true;
	if (isGrounded) 
	{
		velocity.SetY(0.0f);// Reset vertical velocity when grounded

		// Handle jumping input (e.g., space bar)
		if (pControllerI->isJumping) {
			velocity.SetY(jumpForce);  // Apply jump force
			cout << "j";
		}
		// only on mouse position change / controller changed mouse direction, update velocity!
		if(pControllerI->mouseLeftDown && pControllerI->isMouseScreenDirty)
		{
			Vec3 direction = mouseScreenToWorld(pControllerI->mouseScreenPosition);
			velocity.SetX(direction.GetX() * playerSpeed);
			velocity.SetZ(direction.GetZ() * playerSpeed);
			
			cout << "w";
		}
		if(!pControllerI->mouseLeftDown && velocity != Vec3::sZero())
		{
			velocity.SetX(0);
			velocity.SetZ(0);
			cout << "!";
		}
	} 
	else 
	{
		if(velocity.GetY() > 0.3f) // up velocity the > is when to start the double gravity
		{
			// Apply gravity if not grounded
			velocity += Vec3(0.0f, cGravity * deltaTime , 0.0f);
		}
		else // down velocity
		{
			velocity += Vec3(0.0f, cGravityFall * deltaTime , 0.0f);
		}
		// Cap fall speed
		if (velocity.GetY() < cMaxFallSpeed) 
		{
			velocity.SetY(cMaxFallSpeed);
			cout << "falling" << endl;
		}
	}
	
	if(velocity != Vec3::sZero())
	{
		position -= cast_shape(velocity, position); // collision resolution
		position += velocity * deltaTime; // Update player position based on velocity
	}

	// !bug? having joly interface inside the velocity != zero causes strange physics behaviour
	// Move the player body
	pJolt->interface->MoveKinematic(bodyID, position, Quat::sIdentity(), deltaTime);

}

animState PlayerController::getPlayerState()
{
	// playerState_dirty = false;
    return playerState;
}

void PlayerController::setPlayerState(vec2 direction, float allowance)
{
	// animState temp;
	// // playerState_dirty = true;
	// if(direction.y >= allowance)
	// {
	// 	// cout << "up?";
	// 	temp = animState::up;
	// }
	
	// if(direction.y <= -allowance)
	// {
	// 	// cout << "down?";
	// 	temp = animState::down;
	// }

	// if(direction.x >= allowance)
	// {
	// 	// cout << "right?";
	// 	temp = animState::right;
	// }

	// if(direction.x <= -allowance)
	// {
	// 	// cout << "left?";
	// 	temp = animState::left;
	// }

	// if(direction.x > 0 && direction.y > 0 && direction.y < allowance && direction.x < allowance)
	// {
	// 	// cout << "up right";
	// 	temp = animState::upRight;
	// }

	// if(direction.x > 0 && direction.y < 0 && direction.y > -allowance && direction.x < allowance)
	// {
	// 	// cout << "down right";
	// 	temp = animState::downRight;
	// }

	// if(direction.x < 0 && direction.y > 0 && direction.y < allowance && direction.x > -allowance)
	// {
	// 	// cout << "up left";
	// 	temp = animState::upLeft;
	// }

	// if(direction.x < 0 && direction.y < 0 && direction.y > -allowance && direction.x > -allowance)
	// {
	// 	// cout << "down left";
	// 	temp = animState::downLeft;
	// }
	// // cout << temp << endl;

	// if(playerState == temp)
	// {
	// 	return;
	// }
	// else
	// {
	// 	playerState = temp;
	// 	switch(playerState)
	// 	{
	// 		case animState::up:
	// 			pAnimator->setAnimation("hello", 0);
	// 			break;
	// 		case animState::down:
	// 			pAnimator->setAnimation("hello", 0);
	// 			break;
	// 		case animState::left:
	// 			pAnimator->setAnimation("hello", 0);
	// 			break;
	// 		case animState::right:
	// 			pAnimator->setAnimation("hello", 0);
	// 			break;
	// 		case animState::upLeft:
	// 			pAnimator->setAnimation("null", 0);
	// 			break;
	// 		case animState::upRight:
	// 			pAnimator->setAnimation("null", 0);
	// 			break;
	// 		case animState::downLeft:
	// 			pAnimator->setAnimation("null", 0);
	// 			break;
	// 		case animState::downRight:
	// 			pAnimator->setAnimation("null", 0);
	// 			break;
	// 	}
	// 	// cout << "change state" << endl;
	// }
}

// Function to print a single RMat44 matrix
void console_RMat44(const RMat44& mat) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            cout << mat(i, j) << " ";
        }
        cout << endl;
    }
    cout << endl;
}

vec3 v(Vec3 input)
{
	return vec3(input.GetX(), input.GetY(), input.GetZ());
}

Vec3 v(vec3 input)
{
	return Vec3(input.x, input.y, input.z);
}