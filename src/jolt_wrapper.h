// Jolt Physics Library (https://github.com/jrouwe/JoltPhysics)
// SPDX-FileCopyrightText: 2021 Jorrit Rouwe
// SPDX-License-Identifier: MIT
#pragma once

#include "model.h"
#include "render.h"
#include "controller_interface.h"

// The Jolt headers don't include Jolt.h. Always include Jolt.h before including any other Jolt header.
// You can use Jolt.h in your precompiled header to speed up compilation.
#include <Jolt/Jolt.h>

// Jolt includes
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>

#include <Jolt/Physics/Collision/ShapeCast.h>
#include <Jolt/Physics/Collision/RayCast.h> // Include this for raycasting
#include <Jolt/Physics/Collision/CastResult.h> // Include this for raycasting
#include <Jolt/Physics/Collision/NarrowPhaseQuery.h>

// STL includes
#include <iostream>
#include <cstdarg>
#include <thread>

// Disable common warnings triggered by Jolt, you can use JPH_SUPPRESS_WARNING_PUSH / JPH_SUPPRESS_WARNING_POP to store and restore the warning state
JPH_SUPPRESS_WARNINGS

// All Jolt symbols are in the JPH namespace
using namespace JPH;

// If you want your code to compile using single or double precision write 0.0_r to get a Real value that compiles to double or float depending if JPH_DOUBLE_PRECISION is set or not.
using namespace JPH::literals;

// We're also using STL classes in this example
using namespace std;

// Callback for traces, connect this to your own trace function if you have one
static void TraceImpl(const char *inFMT, ...)
{
	// Format the message
	va_list list;
	va_start(list, inFMT);
	char buffer[1024];
	vsnprintf(buffer, sizeof(buffer), inFMT, list);
	va_end(list);

	// Print to the TTY
	cout << buffer << endl;
}

#ifdef JPH_ENABLE_ASSERTS

// Callback for asserts, connect this to your own assert handler if you have one
static bool AssertFailedImpl(const char *inExpression, const char *inMessage, const char *inFile, uint inLine)
{
	// Print to the TTY
	cout << inFile << ":" << inLine << ": (" << inExpression << ") " << (inMessage != nullptr? inMessage : "") << endl;

	// Breakpoint
	return true;
};

#endif // JPH_ENABLE_ASSERTS

// Layer that objects can be in, determines which other objects it can collide with
// Typically you at least want to have 1 layer for moving bodies and 1 layer for static bodies, but you can have more
// layers if you want. E.g. you could have a layer for high detail collision (which is not used by the physics simulation
// but only if you do collision testing).
namespace Layers
{
	static constexpr ObjectLayer NON_MOVING = 0;
	static constexpr ObjectLayer MOVING = 1;
	static constexpr ObjectLayer NUM_LAYERS = 2;
};

/// Class that determines if two object layers can collide
class ObjectLayerPairFilterImpl : public ObjectLayerPairFilter
{
public:
	virtual bool ShouldCollide(ObjectLayer inObject1, ObjectLayer inObject2) const override
	{
		switch (inObject1)
		{
		case Layers::NON_MOVING:
			return inObject2 == Layers::MOVING; // Non moving only collides with moving
		case Layers::MOVING:
			return true; // Moving collides with everything
		default:
			JPH_ASSERT(false);
			return false;
		}
	}
};

// Each broadphase layer results in a separate bounding volume tree in the broad phase. You at least want to have
// a layer for non-moving and moving objects to avoid having to update a tree full of static objects every frame.
// You can have a 1-on-1 mapping between object layers and broadphase layers (like in this case) but if you have
// many object layers you'll be creating many broad phase trees, which is not efficient. If you want to fine tune
// your broadphase layers define JPH_TRACK_BROADPHASE_STATS and look at the stats reported on the TTY.
namespace BroadPhaseLayers
{
	static constexpr BroadPhaseLayer NON_MOVING(0);
	static constexpr BroadPhaseLayer MOVING(1);
	static constexpr uint NUM_LAYERS(2);
};

// BroadPhaseLayerInterface implementation
// This defines a mapping between object and broadphase layers.
class BPLayerInterfaceImpl final : public BroadPhaseLayerInterface
{
public:
	BPLayerInterfaceImpl()
	{
		// Create a mapping table from object to broad phase layer
		mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
		mObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
	}

	virtual uint GetNumBroadPhaseLayers() const override
	{
		return BroadPhaseLayers::NUM_LAYERS;
	}

	virtual BroadPhaseLayer GetBroadPhaseLayer(ObjectLayer inLayer) const override
	{
		JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
		return mObjectToBroadPhase[inLayer];
	}

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
	virtual const char *			GetBroadPhaseLayerName(BroadPhaseLayer inLayer) const override
	{
		switch ((BroadPhaseLayer::Type)inLayer)
		{
		case (BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:	return "NON_MOVING";
		case (BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:		return "MOVING";
		default:													JPH_ASSERT(false); return "INVALID";
		}
	}
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

private:
	BroadPhaseLayer mObjectToBroadPhase[Layers::NUM_LAYERS];
};

/// Class that determines if an object layer can collide with a broadphase layer
class ObjectVsBroadPhaseLayerFilterImpl : public ObjectVsBroadPhaseLayerFilter
{
public:
	virtual bool ShouldCollide(ObjectLayer inLayer1, BroadPhaseLayer inLayer2) const override
	{
		switch (inLayer1)
		{
		case Layers::NON_MOVING:
			return inLayer2 == BroadPhaseLayers::MOVING;
		case Layers::MOVING:
			return true;
		default:
			JPH_ASSERT(false);
			return false;
		}
	}
};

// An example contact listener
class MyContactListener : public ContactListener
{
public:
	// See: ContactListener
	virtual ValidateResult	OnContactValidate(const Body &inBody1, const Body &inBody2, RVec3Arg inBaseOffset, const CollideShapeResult &inCollisionResult) override
	{
		// cout << "Contact validate callback" << endl;

		// Allows you to ignore a contact before it is created (using layers to not make objects collide is cheaper!)
		return ValidateResult::AcceptAllContactsForThisBodyPair;
	}

	virtual void OnContactAdded(const Body &inBody1, const Body &inBody2, const ContactManifold &inManifold, ContactSettings &ioSettings) override
	{
		// cout << "A contact was added" << endl;
	}

	virtual void OnContactPersisted(const Body &inBody1, const Body &inBody2, const ContactManifold &inManifold, ContactSettings &ioSettings) override
	{
		// cout << "A contact was persisted" << endl;
	}

	virtual void OnContactRemoved(const SubShapeIDPair &inSubShapePair) override
	{
		// cout << "A contact was removed" << endl;
	}
};

// An example activation listener
class MyBodyActivationListener : public BodyActivationListener
{
public:
	virtual void OnBodyActivated(const BodyID &inBodyID, uint64 inBodyUserData) override
	{
		// cout << "A body got activated" << endl;
	}

	virtual void OnBodyDeactivated(const BodyID &inBodyID, uint64 inBodyUserData) override
	{
		// cout << "A body went to sleep" << endl;
	}
};

class MyCastShapeCollector : public CastShapeCollector {
public:
    // Constructor
    MyCastShapeCollector() : CastShapeCollector() {}

    // Store hit results when a hit is found
    virtual void AddHit(const ResultType &inResult) override {
        // Store the hit
        mHits.push_back(inResult);

        // Update the early out fraction
        UpdateEarlyOutFraction(inResult.mFraction);
		mHasHits = true;  // Mark that we have recorded a hit
    }

    // Function to retrieve all the hits
    const std::vector<ResultType>& GetHits() const {
        return mHits;
    }

    // Function to get the earliest hit fraction
    float GetHitFraction() const {
        return !mHits.empty() ? mHits.front().mFraction : FLT_MAX;
    }

    // Get the penetration depth of the first hit
    float GetPenetrationDepth() const {
        return !mHits.empty() ? mHits.front().mPenetrationDepth : 0.0f;
    }

	// Check if we have any hits
    bool HasHits() const {
        return mHasHits;
    }
	std::vector<ResultType> mHits;  // Store all hits
private:
    
	bool mHasHits = false;  // Track whether any hits were recorded
};

class MyBodyFilter : public BodyFilter
{
public:
    MyBodyFilter(BodyID ignoreBodyID) : mIgnoreBodyID(ignoreBodyID) {}

    // Override the ShouldCollide function
    virtual bool ShouldCollide(const BodyID &inBodyID) const override
    {
        // Ignore the body if it matches the one you want to filter out
        return inBodyID != mIgnoreBodyID;
    }

private:
    BodyID mIgnoreBodyID; // Store the ID of the body to ignore
};
// class MyCastShapeCollector : public CastShapeCollector {
// public:
//     // Constructor
//     MyCastShapeCollector() : CastShapeCollector() {}

//     // Reset the collector to prepare for a new query
//     virtual void Reset() override {
//         mHits.clear();  // Clear previous hits
//         // mEarlyOutFraction = FLT_MAX;  // Reset early out fraction
//     }

//     // Store hit results when a hit is found
//     virtual void AddHit(const ResultType& inResult) override {
//         // Add the hit to the list of hits
//         mHits.push_back(inResult);

//         // Update the early out fraction (to prevent unnecessary calculations)
//         UpdateEarlyOutFraction(inResult.mFraction);
//     }

//     // Function to retrieve all the hits
//     const std::vector<ResultType>& GetHits() const {
//         return mHits;
//     }

//     // Check if we have any hits
//     bool HasHits() const {
//         return !mHits.empty();
//     }

//     // Function to get the first hit (if available)
//     const ResultType& GetFirstHit() const {
//         JPH_ASSERT(!mHits.empty());
//         return mHits.front();
//     }

//     // Function to get the earliest hit fraction
//     float GetHitFraction() const {
//         return !mHits.empty() ? mHits.front().mFraction : FLT_MAX;
//     }

// private:
//     std::vector<ResultType> mHits;  // Store all hits
// };

enum objectType
{
    player = 0,
    enviroment_static = 1,
    enviroment_dynamic = 2,
    npc = 3
};

class JoltWrapper
{
private:

	// The threshold below which we consider an object to have fallen off the world
	const float cKillThreshold = -50.0f;  

	// This is the max amount of rigid bodies that you can add to the physics system. If you try to add more you'll get an error.
	// Note: This value is low because this is a simple test. For a real project use something in the order of 65536.
	const uint cMaxBodies = 1024;

	// This determines how many mutexes to allocate to protect rigid bodies from concurrent access. Set it to 0 for the default settings.
	const uint cNumBodyMutexes = 0;

	// This is the max amount of body pairs that can be queued at any time (the broad phase will detect overlapping
	// body pairs based on their bounding boxes and will insert them into a queue for the narrowphase). If you make this buffer
	// too small the queue will fill up and the broad phase jobs will start to do narrow phase work. This is slightly less efficient.
	// Note: This value is low because this is a simple test. For a real project use something in the order of 65536.
	const uint cMaxBodyPairs = 1024;

	// This is the maximum size of the contact constraint buffer. If more contacts (collisions between bodies) are detected than this
	// number then these contacts will be ignored and bodies will start interpenetrating / fall through the world.
	// Note: This value is low because this is a simple test. For a real project use something in the order of 10240.
	const uint cMaxContactConstraints = 1024;

	// We simulate the physics world in discrete time steps. 60 Hz is a good rate to update the physics system.
	const float cDeltaTime = 1.0f / 60.0f;

	// If you take larger steps than 1 / 60th of a second you need to do multiple collision steps in order to keep the simulation stable. Do 1 collision step per 1 / 60th of a second (round up).
	const int cCollisionSteps = 1;

	// Player's current velocity (should be stored in the player object, this is just an example)
    Vec3 velocity = Vec3(0.0f, 0.0f, 0.0f);  // This needs to persist between updates
public:

	MyBodyFilter *pBodyFilter; // filter out floor in collision

	// Create mapping table from object layer to broadphase layer
	// Note: As this is an interface, PhysicsSystem will take a reference to this so this instance needs to stay alive!
	BPLayerInterfaceImpl broad_phase_layer_interface;

	// Create class that filters object vs broadphase layers
	// Note: As this is an interface, PhysicsSystem will take a reference to this so this instance needs to stay alive!
	ObjectVsBroadPhaseLayerFilterImpl object_vs_broadphase_layer_filter;

	// Create class that filters object vs object layers
	// Note: As this is an interface, PhysicsSystem will take a reference to this so this instance needs to stay alive!
	ObjectLayerPairFilterImpl object_vs_object_layer_filter;

	// A body activation listener gets notified when bodies activate and go to sleep
	// Note that this is called from a job so whatever you do here needs to be thread safe.
	// Registering one is entirely optional.
	MyBodyActivationListener body_activation_listener;

	// A contact listener gets notified when bodies (are about to) collide, and when they separate again.
	// Note that this is called from a job so whatever you do here needs to be thread safe.
	// Registering one is entirely optional.
	MyContactListener contact_listener;

	// We need a temp allocator for temporary allocations during the physics update. We're
	// pre-allocating 10 MB to avoid having to do allocations during the physics update.
	// B.t.w. 10 MB is way too much for this example but it is a typical value you can use.
	// If you don't want to pre-allocate you can also use TempAllocatorMalloc to fall back to
	// malloc / free.
	TempAllocatorImpl *temp_allocator_ptr; // changed to ptr for wrapper purpose

	// We need a job system that will execute physics jobs on multiple threads. Typically
	// you would implement the JobSystem interface yourself and let Jolt Physics run on top
	// of your own job scheduler. JobSystemThreadPool is an example implementation.
	JobSystemThreadPool *job_system_ptr; // changed to ptr for wrapper purpose

	BodyInterface *interface;

	// Now we can create the actual physics system.
	PhysicsSystem ps;
	
	/**************** important! */
	// vector to keep track of the body ids that require render as well as simulating
	// use physiscSystem.GetBodies() to get all bodies if needed
	BodyIDVector bodyIDs; 

	vector<RMat44> matrices; // only for rendered bodies to pass to ssbo

	JoltWrapper();
	~JoltWrapper();

	void joltRegister();
	void joltUnregister();
	void update();
	// void updateKinematic(BodyID inBody, Vec3 inPosition, float deltaTime);
	// BodyInterface& get_interface();
	BodyID create_object(RenderPool& render, objectType inType, Model &inModel, RVec3Arg inPosition, QuatArg inRot);
	BodyID create_shape(const Shape *inShape, bool isSensor, RVec3Arg inPosition, QuatArg inRotation = Quat::sIdentity(), 
		EMotionType inMotionType = EMotionType::Static, 
		ObjectLayer inObjectLayer = Layers::NON_MOVING, 
		EActivation inActivation = EActivation::DontActivate);
	
	void optimize();
};

class PlayerController {
private:
	// Ground detection threshold
	const float cGroundDetectionThreshold 	= 0.0f;
	const float cHalfHeight 				= 1.33f;
	const float cRadius 					= 1.33f;
	const float cGravity 					= -20.0f;
	const float cGravityFall 				= cGravity * 2.0f;
	const float cMaxFallSpeed 				= -50.0f;

	JoltWrapper *pJolt;
	
public:
    BodyID bodyID;
    Vec3 position  		= Vec3::sZero();
    Vec3 velocity 		= Vec3::sZero();
    bool isGrounded 	= false;
	bool isJumping 		= false;
    float jumpForce 	= 12.0f;
	float playerSpeed 	= 8;

	vector<uint> textureIndices;

	// vector<RMat44> matrices; // only for rendered bodies to pass to ssbo

	PlayerController(RenderPool& inRenderPool, JoltWrapper *inJolt, Vec3 inPosition);
	bool cast_ray(Vec3 start, Vec3 end, float *outDepth);
	Vec3 cast_shape(Vec3 inDirection, Vec3 inPosition);
	void update(ControllerInterface controllerInterface, float deltaTime);
};

void console_RMat44(const JPH::RMat44& mat);
vec3 v(Vec3 input);
Vec3 v(vec3 input);