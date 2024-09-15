#include "jolt_wrapper.h"

joltWrapper::joltWrapper()
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
}

joltWrapper::~joltWrapper()
{
	cout << "Deleteing jolt Wrapper.." << endl;
	for (BodyID bodyID : bodyIDs)
    {
		// Remove the shape from the physics system. Note that the sphere itself keeps all of its state and can be re-added at any time.
        get_interface().RemoveBody(bodyID);
		// Destroy the shape. After this the sphere ID is no longer valid.
		get_interface().DestroyBody(bodyID);
    }

	joltUnregister();
}

void joltWrapper::joltRegister()
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

void joltWrapper::joltUnregister()
{
	// Unregisters all types with the factory and cleans up the default material
	UnregisterTypes();

	// Destroy the factory
	delete Factory::sInstance;
	Factory::sInstance = nullptr;
}

void joltWrapper::update()
{
	// Step the world
	ps.Update(cDeltaTime, cCollisionSteps, temp_allocator_ptr, job_system_ptr);
}

BodyInterface &joltWrapper::get_interface()
{
	// The main way to interact with the bodies in the physics system is through the body interface. There is a locking and a non-locking
	// variant of this. We're going to use the locking version (even though we're not planning to access bodies from multiple threads)
    return ps.GetBodyInterface();
}

Body* joltWrapper::create_shape(const Shape *inShape, RVec3Arg inPosition, QuatArg inRotation, EMotionType inMotionType, ObjectLayer inObjectLayer, EActivation inActivation)
{
	// inShape->SetEmbedded();
	BodyCreationSettings settings(inShape, inPosition, inRotation, inMotionType, inObjectLayer);
    
	// return get_interface().CreateAndAddBody(settings, inActivation);

	// Create the body in the physics system
	Body* body = get_interface().CreateBody(settings);

	// Add the body to the physics system and activate it
	get_interface().AddBody(body->GetID(), inActivation);

	bodyIDs.push_back(body->GetID());
	return body;
}

void joltWrapper::optimize()
{
	// Optional step: Before starting the physics simulation you can optimize the broad phase. This improves collision detection performance (it's pointless here because we only have 2 bodies).
	// You should definitely not call this every frame or when e.g. streaming in a new level section as it is an expensive operation.
	// Instead insert all new objects in batches instead of 1 at a time to keep the broad phase efficient.
	ps.OptimizeBroadPhase();
}
