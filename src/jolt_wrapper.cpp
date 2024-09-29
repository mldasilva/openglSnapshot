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

// void joltWrapper::updateKinematic(BodyID inBody, Vec3 inPosition, float deltaTime)
// {
//     if(check_ground(inBody))
//     {
//         // Reset vertical velocity when grounded
//         // velocity.SetY(0.0f);

//         // Move kinematic body to the desired position
//         interface->MoveKinematic(inBody, inPosition, Quat::sIdentity(), deltaTime);
//     }
//     else
//     {
//         // Apply gravity to velocity if not grounded
//         velocity += cGravity * deltaTime * 10;

//         // Update the position by adding the velocity to it
//         Vec3 newPosition = inPosition + velocity * deltaTime;

//         // Output falling for debug purposes
//         cout << "falling " << endl;

//         // Move the player to the new position (affected by gravity)
//         interface->MoveKinematic(inBody, newPosition, Quat::sIdentity(), deltaTime);
//     }
// }

// BodyInterface &joltWrapper::get_interface()
// {
// 	// The main way to interact with the bodies in the physics system is through the body interface. There is a locking and a non-locking
// 	// variant of this. We're going to use the locking version (even though we're not planning to access bodies from multiple threads)
//     return ps.GetBodyInterface();
// }

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

PlayerController::PlayerController(JoltWrapper *inJolt, Vec3 inPosition)
{
	pJolt = inJolt;
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


void PlayerController::update(controllerI controllerInterface, float deltaTime)
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
		// Reset vertical velocity when grounded
		velocity.SetY(0.0f);

		// Handle jumping input (e.g., space bar)
		if (controllerInterface.isJumping) {
			velocity.SetY(jumpForce);  // Apply jump force
		}

		velocity.SetX(controllerInterface.mouseDirection.x * playerSpeed);
		velocity.SetZ(controllerInterface.mouseDirection.z * playerSpeed);
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
		}
	}
	
	position -= cast_shape(velocity, position); // collision resolution
	position += velocity * deltaTime; // Update player position based on velocity

	// Move the player body
	pJolt->interface->MoveKinematic(bodyID, position, Quat::sIdentity(), deltaTime);
	
	// temp need to update matrices
	// matrices[0] = Mat44::sTranslation(position);

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