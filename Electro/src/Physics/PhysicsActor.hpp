//                    ELECTRO ENGINE
// Copyright(c) 2021 - Electro Team - All rights reserved
#pragma once
#include "Core/Ref.hpp"
#include "Scene/Entity.hpp"
#include "Physics/PhysicsEngine.hpp"
#include "PhysXUtils.hpp"

namespace physx
{
    class PxRigidActor;
    class PxShape;
    class PxMaterial;
}

namespace Electro
{
    class PhysicsActor : public IElectroRef
    {
    public:
        PhysicsActor(Entity entity);
        ~PhysicsActor();

        Entity& GetEntity() { return mEntity; }
        void Rotate(const glm::vec3& rotation);
        float GetMass() const;
        void SetMass(float mass);

        glm::vec3 GetPosition() { return PhysXUtils::FromPhysXVector(mInternalActor->getGlobalPose().p); }
        glm::quat GetRotation() { return PhysXUtils::FromPhysXQuat(mInternalActor->getGlobalPose().q); }

        void AddForce(const glm::vec3& force, ForceMode forceMode);
        void AddTorque(const glm::vec3& torque, ForceMode forceMode);

        glm::vec3 GetLinearVelocity() const;
        void SetLinearVelocity(const glm::vec3& velocity);

        glm::vec3 GetAngularVelocity() const;
        void SetAngularVelocity(const glm::vec3& velocity);

        void SetLinearDrag(float drag) const;
        void SetAngularDrag(float drag) const;

        bool IsDynamic() const { return mRigidBody.BodyType == RigidBodyComponent::Type::Dynamic; }
        void Update(float fixedTimestep);
    private:
        void Initialize();
        void SubmitToPhysX();
        void UpdateTransform();
    private:
        Entity mEntity;
        RigidBodyComponent& mRigidBody;
        Ref<PhysicsMaterial> mPhysicsMaterial;

        physx::PxRigidActor* mInternalActor;
        std::unordered_map<int, std::vector<physx::PxShape*>> mShapes;
        physx::PxMaterial* mInternalMaterial;

        friend class PhysicsEngine;
        friend class PhysXInternal;
    };
}
