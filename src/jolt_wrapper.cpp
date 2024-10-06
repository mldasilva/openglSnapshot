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

BodyID JoltWrapper::create_object(RenderPool& render, objectType inType, modelI inModel, RVec3Arg inPosition, QuatArg inRot)
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

bool PlayerController::groundCheck()
{
	// Get player position and adjust for capsule height
	// Adjust this based on your player capsule's half-height

	// Start at player's feet
    Vec3 start = position - Vec3(0.0f, cHalfHeight, 0.0f);  
    Vec3 end = start + Vec3(0.0f, -0.01f, 0.0f);
	
	return cast_ray(start, end, nullptr);
}

void PlayerController::setPlayerState()
{
	// state rules
	isGrounded = groundCheck();
	if(isGrounded) // grounded
	{
		if(velocity == Vec3::sZero())
		{
			ePlayerState = playerState::still;
		}
		else if(velocity.GetY() > 0) // grounded but velocity > 0 special case
		{
			// so close to the floor and the tick are the perfect moment we are jumping and grounded
			ePlayerState = playerState::jumping;
		}
		else if(velocity.GetX() != 0 || velocity.GetZ() != 0)
		{
			ePlayerState = playerState::running;
		}
		else if(velocity.GetX() != 0 || velocity.GetY() != 0 || velocity.GetZ() != 0)
		{
			// todo prev ground ? if not ground bvut now grounded we've landed
			ePlayerState = playerState::landed;
		}
		else
		{
			throw std::runtime_error("Error is player update logic");
		}
	}
	else // falling or jumping
	{
		// we are headed upwards
		if(position.GetY() > prevPosition.GetY())
		{
			ePlayerState = playerState::jumping;
		}
		// we are headed downloads
		else if(position.GetY() <= prevPosition.GetY())
		{
			ePlayerState = playerState::falling;
		}
		// we might ave just landed
		else
		{
			throw std::runtime_error("Error is player update logic");
		}
	}
}

void PlayerController::setVelocity(float deltaTime)
{
	// state reaction
	if(ePlayerState == playerState::still)
	{
		velocity.SetY(0);
	}
	if(ePlayerState == playerState::running)
	{
		velocity.SetY(0);
	}
	if(ePlayerState == playerState::jumping)
	{
		velocity += Vec3(0.0f, cGravity * deltaTime , 0.0f);
	}
	if(ePlayerState == playerState::falling)
	{
		// Apply gravity if not grounded
		velocity += Vec3(0.0f, cGravityFall * deltaTime , 0.0f);
	}
	if(ePlayerState == playerState::landed)
	{
		velocity.SetY(0);
	}
}

void PlayerController::update(float deltaTime)
{
	ePrevPlayerState = ePlayerState;

	setPlayerState();

	// cout << endl;
	// cout << "prevVelocity:" << prevVelocity << endl;
	// cout << "velocity:" << velocity << endl;
	// cout << endl;

	prevVelocity = velocity;
	prevPosition = position;

	setVelocity(deltaTime);

	// controls: should only fire once when needed
	//================================

	if (pControllerI->isJumping && ePlayerState != playerState::jumping && isGrounded) {
		velocity.SetY(jumpForce);  // Apply jump force
	}

	// only on mouse position change / controller changed mouse direction, update velocity!
	if( (pControllerI->mouseLeftDown && pControllerI->isMouseScreenDirty) 
		|| (pControllerI->mouseLeftDown && !pControllerI->isMouseScreenDirty && ePlayerState == playerState::still) )
	{
		Vec3 direction = mouseScreenToWorld(pControllerI->mouseScreenPosition);
		velocity.SetX(direction.GetX() * playerSpeed);
		velocity.SetZ(direction.GetZ() * playerSpeed);
	}

	if(!pControllerI->mouseLeftDown && ePlayerState == playerState::running)
	{
		velocity.SetX(0);
		velocity.SetZ(0);
	}

	// animation :
	//================================

	ePrevAnimState = eAnimState;

	if(ePlayerState == playerState::running)
	{	
		// changes eAnimState
		setRunningAnimation(mouseScreenDirection(pControllerI->mouseScreenPosition), 0.98);

		if(ePrevAnimState != eAnimState)
		{
			cout << "running: ";
			switch(eAnimState)
			{	
				case animState::a_up:
					cout << "up" << endl;
					break;
				case animState::a_left:
					cout << "left" << endl;
					break;
				case animState::a_right:
					cout << "right" << endl;
					break;
				case animState::a_down:
					cout << "down" << endl;
					break;
				case animState::a_upLeft:
					cout << "up left" << endl;
					break;
				case animState::a_upRight:
					cout << "up right" << endl;
					break;
				case animState::a_downLeft:
					cout << "down left" << endl;
					break;
				case animState::a_downRight:
					cout << "down right" << endl;
					break;
				default:
					break;
			}
		}
	}

	if(ePlayerState == playerState::still)
	{
		eAnimState = animState::a_still;

		if(ePrevAnimState != eAnimState)
		{
			cout << "still animation" << endl;
		}
	}

	if(ePlayerState == playerState::jumping)
	{
		eAnimState = animState::a_jumping;
		
		if(ePrevAnimState != eAnimState)
		{
			cout << "jumping animation" << endl;
		}
	}

	if(ePlayerState == playerState::falling)
	{
		eAnimState = animState::a_falling;
		
		if(ePrevAnimState != eAnimState)
		{
			cout << "falling animation" << endl;
		}
	}

	if(ePlayerState == playerState::landed)
	{
		eAnimState = animState::a_landed;
		
		if(ePrevAnimState != eAnimState)
		{
			cout << "landed animation" << endl;
		}
	}

	// update
	if(ePlayerState != playerState::error)
	{
		position -= cast_shape(velocity, position); // collision resolution
		position += velocity * deltaTime; // Update player position based on velocity
	
		// !bug? having joly interface inside the velocity != zero causes strange physics behaviour
		// Move the player body
		pJolt->interface->MoveKinematic(bodyID, position, Quat::sIdentity(), deltaTime);
	}
}

// animState PlayerController::getPlayerState()
// {
// 	// playerState_dirty = false;
//     return playerState;
// }

void PlayerController::setRunningAnimation(vec2 direction, float allowance)
{
	if(direction.y >= allowance)
	{
		// cout << "up?" << endl;
		eAnimState = animState::a_up;
	}
	
	if(direction.y <= -allowance)
	{
		// cout << "down?" << endl;
		eAnimState = animState::a_down;
	}

	if(direction.x >= allowance)
	{
		// cout << "right?" << endl;
		eAnimState = animState::a_right;
	}

	if(direction.x <= -allowance)
	{
		// cout << "left?" << endl;
		eAnimState = animState::a_left;
	}

	if(direction.x > 0 && direction.y > 0 && direction.y < allowance && direction.x < allowance)
	{
		// cout << "up right" << endl;
		eAnimState = animState::a_upRight;
	}

	if(direction.x > 0 && direction.y < 0 && direction.y > -allowance && direction.x < allowance)
	{
		// cout << "down right" << endl;
		eAnimState = animState::a_downRight;
	}

	if(direction.x < 0 && direction.y > 0 && direction.y < allowance && direction.x > -allowance)
	{
		// cout << "up left" << endl;
		eAnimState = animState::a_upLeft;
	}

	if(direction.x < 0 && direction.y < 0 && direction.y > -allowance && direction.x > -allowance)
	{
		// cout << "down left" << endl;
		eAnimState = animState::a_downLeft;
	}
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