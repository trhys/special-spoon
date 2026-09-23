#include "CollisionSystem.h"

// copilot has super janked my system so im doing a big refactor by hand here
// i will be commenting a lot to keep track of where exactly i am in this
// enormous bowl of spaghetti

namespace Spoon {
	void CollisionSystem::Update(sf::Time tick, EntityManager& manager)
	{
		// get delta time for the frame
		const float dt = tick.asSeconds();
		if (dt <= 0.0f)
			return;

		// get component arrays
		auto& colliderArray = manager.GetArray<ColliderComp>(ColliderComp::Name);
		auto& movementArray = manager.GetArray<MovementComp>(MovementComp::Name);
		auto& transformArray = manager.GetArray<TransformComp>(TransformComp::Name);
		auto& physicsArray = manager.GetArray<PhysicsComp>(PhysicsComp::Name);

		// reset debug flags
		for (auto& movementComp : movementArray.m_Components)
		{
			movementComp.m_WasCorrectedByPhysics = false;
		}

		for (auto& collider : colliderArray.m_Components)
		{
			collider.touchingLastFrame = collider.touchingThisFrame;
			collider.touchingThisFrame.clear();
		}

		// return early if no colliders exist
		if (colliderArray.m_Components.empty())
			return;

		// init bodies
		std::unordered_map<UUID, BodyRuntime> bodies;
		bodies.reserve(colliderArray.m_Components.size());

		for (size_t index = 0; index < colliderArray.m_Components.size(); index++)
		{
			// get id from collider component
			// if we are missing a transform for this entity, skip the rest of this iteration
			const UUID id = colliderArray.m_IndexToId[index];
			if (!transformArray.m_IdToIndex.count(id))
				continue;

			// build or body for the solver later
			BodyRuntime body;
			body.id = id;
			body.collider = &colliderArray.m_Components[index];
			body.transform = &manager.GetComponent<TransformComp>(id, TransformComp::Name);
			body.position = body.transform->GetPosition();
			body.startPosition = body.position;

			// give the body physics data if it exists
			if (physicsArray.m_IdToIndex.count(id))
			{
				body.physics = &manager.GetComponent<PhysicsComp>(id, PhysicsComp::Name);
				body.physicsDriven = true;
				body.bodyType = body.physics->bodyType;
				if (body.bodyType != BodyType::Static)
				{
					body.remainingTime = dt;
					body.remainingDelta = body.physics->velocity * dt;
					body.canTranslate = true;
				}

				// get movement comp if it exists
				if (movementArray.m_IdToIndex.count(id))
				{
					body.movement = &manager.GetComponent<MovementComp>(id, MovementComp::Name);
					body.movement->m_ProposedDelta = body.remainingDelta;
				}
			}

			bodies.emplace(id, body);
		}

		// skip the solver algos if there isnt enough material to solve
if (bodies.size() == 1)
		{
			CommitRemainingMotion(bodies.begin()->second);
			SyncMovementState(bodies.begin()->second);
			return;
		}

		// run solver
		SimulateContinuousAABB(bodies);
		ResolveDiscreteCleanup(bodies);

		for (auto& [id, body] : bodies)
		{
			// non physics bodies have movement applied straight from the movement system
			if (!body.physics)
				continue;

			// write solved body dynamics back to the movement comp
			if (body.movement)
			{
				body.movement->m_Velocity = body.physics->velocity;
				body.movement->m_ProposedDelta = body.position - body.startPosition;
			}
		}
	}

	void CollisionSystem::SimulateContinuousAABB(std::unordered_map<UUID, BodyRuntime>& bodies)
	{
		// build broadphase
		quadtree.BuildTree(m_Config.bounds);

		const auto buildSweptBounds = [&](std::unordered_map<UUID, sf::FloatRect>& sweptBounds)
		{
			// clear cache
			sweptBounds.clear();
			sweptBounds.reserve(bodies.size());
			for (const auto& [id, body] : bodies)
			{
				// get starting AABB
				sf::FloatRect start = ComputeBoundsAt(body, body.position);
				// get positional delta
				sf::Vector2f delta = body.canTranslate ? body.remainingDelta : sf::Vector2f{ 0.0f, 0.0f };
				// build swept aabb
				sweptBounds.emplace(id, ComputeSweptBounds(start, delta));
			}
		};

		std::unordered_map<UUID, sf::FloatRect> sweptBounds;
auto buildCandidatePairs = [&](std::set<std::pair<UUID, UUID>>& candidatePairs)
		{
			// populate swept aabb vector and push onto broadphase
			buildSweptBounds(sweptBounds);
			quadtree.Populate(sweptBounds);
			candidatePairs = quadtree.GeneratePairs();

			for (const auto& [id, body] : bodies)
			{
				for (const UUID touchId : body.collider->touchingLastFrame)
				{
					if (touchId == id || !bodies.count(touchId))
						continue;
					UUID first = id;
					UUID second = touchId;
					if (first > second)
						std::swap(first, second);
					candidatePairs.insert({ first, second });
				}
			}
		};

		// set iterations
		const int maxBroadphaseRefreshes = std::max(1, static_cast<int>(bodies.size())) * k_MaxToiEvents;
		int broadphaseRefreshes = 0;
		while (true)
		{
			std::set<std::pair<UUID, UUID>> candidatePairs;
			buildCandidatePairs(candidatePairs);

			// broadphase came up empty - finish resolution
			if (candidatePairs.empty())
			{
				for (auto& [id, body] : bodies)
				{
					if (!body.canTranslate)
						continue;
					CommitRemainingMotion(body);
				}
				return;
			}

			// begin island graph
			std::unordered_map<UUID, UUID> parent;
			parent.reserve(bodies.size());

			// each body begins as its own root
			for (const auto& [id, body] : bodies)
				parent[id] = id;

			// recursive graph traversal
			const auto findRoot = [&](UUID id) -> UUID
			{
				UUID root = id;
				while (!(parent[root] == root))
					root = parent[root];

				UUID current = id;
				while (!(parent[current] == current))
				{
					UUID next = parent[current];
					parent[current] = root;
					current = next;
				}
				return root;
			};

			// group island pairs
			auto unite = [&](UUID a, UUID b)
			{
				UUID rootA = findRoot(a);
				UUID rootB = findRoot(b);
				if (rootA == rootB)
					return;
				parent[rootB] = rootA;
			};

			for (const auto& pair : candidatePairs)
				unite(pair.first, pair.second);

			// put every pair that shares a root into one island
			std::unordered_map<UUID, std::set<std::pair<UUID, UUID>>> islandPairs;
			for (const auto& pair : candidatePairs)
			{
				const UUID root = findRoot(pair.first);
				islandPairs[root].insert(pair);
			}
			// union-find completed
			
			bool rebuildBroadphase = false;
			std::unordered_set<UUID> bodiesInIslands;
			for (auto& [root, pairs] : islandPairs)
			{
				// create a set of ids in this island
				std::unordered_set<UUID> islandBodies;
				for (const auto& pair : pairs)
				{
					islandBodies.insert(pair.first);
					islandBodies.insert(pair.second);
				}
				bodiesInIslands.insert(islandBodies.begin(), islandBodies.end());

				bool islandRequiresBroadphaseRefresh = false;
				int zeroProgressGuard = 0;
				int eventIndex = 0;
				
				// begin TOI calculations
				for (; eventIndex < k_MaxToiEvents; eventIndex++)
				{
					float earliestToi = 1.0f;
					std::vector<Contact> earliestContacts;
					std::vector<Contact> persistentContacts;

					for (const auto& pair : pairs)
					{
						BodyRuntime& bodyA = bodies[pair.first];
						BodyRuntime& bodyB = bodies[pair.second];

						// prototype --- only implemented AABB for now
						if (bodyA.collider->GetType() != ColliderType::AABB || bodyB.collider->GetType() != ColliderType::AABB)
							continue;

						const sf::FloatRect boundsA = ComputeBoundsAt(bodyA, bodyA.position);
						const sf::FloatRect boundsB = ComputeBoundsAt(bodyB, bodyB.position);
						const sf::Vector2f deltaA = bodyA.canTranslate ? bodyA.remainingDelta : sf::Vector2f{ 0.0f, 0.0f };
						const sf::Vector2f deltaB = bodyB.canTranslate ? bodyB.remainingDelta : sf::Vector2f{ 0.0f, 0.0f };

						// get sweep result and resolve TOI
						SweepResult sweep = SweepAABBPair(boundsA, deltaA, boundsB, deltaB);
						if (sweep.startsTouching)
						{
							persistentContacts.push_back(Contact{ pair, sweep.normal });
						}

						if (!sweep.hit)
							continue;

						if (sweep.toi + k_TOIEpsilon < earliestToi)
						{
							earliestToi = sweep.toi;
							earliestContacts.clear();
						}

						if (std::abs(sweep.toi - earliestToi) <= k_TOIEpsilon)
						{
							earliestContacts.push_back(Contact{ pair, sweep.normal });
						}
					}

					const bool hasEarliest = !earliestContacts.empty();
					const bool hasPersistent = !persistentContacts.empty();

					// sweep turned no results - resolve motion
					if (!hasEarliest && !hasPersistent)
					{
						bool moved = false;
						for (UUID bodyId : islandBodies)
						{
							BodyRuntime& body = bodies[bodyId];
							if (!body.canTranslate)
								continue;
							if (IsZeroVector(body.remainingDelta))
								continue;

							CommitRemainingMotion(body);
							moved = true;
						}

						if (!moved)
							break;

						continue;
					}

					if (!hasEarliest)
					{
						bool constrained = false;
						for (const Contact& contact : persistentContacts)
						{
							BodyRuntime& bodyA = bodies[contact.pair.first];
							BodyRuntime& bodyB = bodies[contact.pair.second];
							AddTouch(bodyA.collider->touchingThisFrame, bodyB.id);
							AddTouch(bodyB.collider->touchingThisFrame, bodyA.id);
							constrained |= ConstrainPersistentContactMotion(bodyA, bodyB, contact.normal);
						}

						if (constrained)
						{
							islandRequiresBroadphaseRefresh = true;
							break;
						}

						bool moved = false;
						for (UUID bodyId : islandBodies)
						{
							BodyRuntime& body = bodies[bodyId];
							if (!body.canTranslate)
								continue;
							if (IsZeroVector(body.remainingDelta))
								continue;

							CommitRemainingMotion(body);
							moved = true;
						}

						if (!moved)
							break;

						continue;
					}

					float advance = hasEarliest ? earliestToi : 0.0f;
					advance = std::clamp(advance, 0.0f, 1.0f);
					if (advance > 0.0f)
					{
						for (UUID bodyId : islandBodies)
						{
							BodyRuntime& body = bodies[bodyId];
							if (!body.canTranslate)
								continue;

							// advance timestep in the sweep TOI
							const sf::Vector2f deltaStep = body.remainingDelta * advance;
							body.position += deltaStep;
							body.transform->SetPosition(body.position);
							body.remainingTime *= (1.0f - advance);
							body.remainingDelta -= deltaStep;
						}
					}

					std::unordered_map<std::pair<UUID, UUID>, Contact> batch;
					for (const Contact& contact : persistentContacts)
					{
						batch[contact.pair] = contact;
					}
					for (const Contact& contact : earliestContacts)
					{
						batch[contact.pair] = contact;
					}

					for (const std::pair<UUID, UUID>& pair : pairs)
					{
						BodyRuntime& bodyA = bodies[pair.first];
						BodyRuntime& bodyB = bodies[pair.second];

						// again - only AABB supported for now
						if (bodyA.collider->GetType() != ColliderType::AABB || bodyB.collider->GetType() != ColliderType::AABB)
							continue;

						sf::Vector2f normal = { 0.0f, 0.0f };
						float penetration = 0.0f;
						if (ComputeAABBContact(ComputeBoundsAt(bodyA, bodyA.position), ComputeBoundsAt(bodyB, bodyB.position), normal, penetration))
						{
							batch[pair] = Contact{ pair, normal };
						}
					}

					bool eventProgress = false;
					for (int iteration = 0; iteration < k_MaxContactIterations; iteration++)
					{
						bool iterationProgress = false;
						for (auto& [pair, contact] : batch)
						{
							BodyRuntime& bodyA = bodies[pair.first];
							BodyRuntime& bodyB = bodies[pair.second];

							sf::Vector2f normal = contact.normal;
							float penetration = 0.0f;
							if (!ComputeAABBContact(ComputeBoundsAt(bodyA, bodyA.position), ComputeBoundsAt(bodyB, bodyB.position), normal, penetration))
								continue;

							AddTouch(bodyA.collider->touchingThisFrame, bodyB.id);
							AddTouch(bodyB.collider->touchingThisFrame, bodyA.id);

							if (ApplyContactResponse(bodyA, bodyB, normal, penetration))
							{
								iterationProgress = true;
							}
						}

						eventProgress |= iterationProgress;
						if (!iterationProgress)
							break;
					}

					if (eventProgress)
					{
						islandRequiresBroadphaseRefresh = true;
						break;
					}

					if (advance <= 0.0f)
					{
						zeroProgressGuard++;
						if (zeroProgressGuard >= 2)
						{
							break;
						}
					}
					else
					{
						zeroProgressGuard = 0;
					}

					bool islandDone = true;
					for (UUID bodyId : islandBodies)
					{
						if (!IsZeroVector(bodies[bodyId].remainingDelta))
						{
							islandDone = false;
							break;
						}
					}
					if (islandDone)
						break;
				}

				if (eventIndex == k_MaxToiEvents)
				{
					for (UUID bodyId : islandBodies)
					{
						BodyRuntime& body = bodies[bodyId];
						CommitRemainingMotion(body);
						SyncMovementState(body);
					}
				}

				if (islandRequiresBroadphaseRefresh)
				{
					rebuildBroadphase = true;
					break;
				}
			}

			if (rebuildBroadphase)
			{
				broadphaseRefreshes++;
				if (broadphaseRefreshes >= maxBroadphaseRefreshes)
				{
					for (auto& [id, body] : bodies)
					{
						CommitRemainingMotion(body);
						SyncMovementState(body);
					}
					return;
				}
				continue;
			}

			for (auto& [id, body] : bodies)
			{
				if (!body.canTranslate)
					continue;
				if (bodiesInIslands.count(id))
					continue;
				if (IsZeroVector(body.remainingDelta))
					continue;

				CommitRemainingMotion(body);
			}
			return;
		}
	}

	bool CollisionSystem::ConstrainPersistentContactMotion(BodyRuntime& bodyA, BodyRuntime& bodyB, const sf::Vector2f& normalForA)
	{
		const sf::Vector2f deltaA = bodyA.canTranslate ? bodyA.remainingDelta : sf::Vector2f{ 0.0f, 0.0f };
		const sf::Vector2f deltaB = bodyB.canTranslate ? bodyB.remainingDelta : sf::Vector2f{ 0.0f, 0.0f };
		const sf::Vector2f relativeDelta = deltaA - deltaB;
		const float closingDelta = relativeDelta.x * normalForA.x + relativeDelta.y * normalForA.y;
		if (closingDelta >= 0.0f)
			return false;

		auto motionWeight = [](const BodyRuntime& body, const BodyRuntime& other)
		{
			if (!body.canTranslate)
				return 0.0f;

			if (body.bodyType == BodyType::Kinematic)
			{
				if (!other.canTranslate)
					return 1.0f;
				if (other.bodyType == BodyType::Dynamic)
					return 0.0f;
				return 1.0f;
			}

			const float invMass = InverseMass(body);
			return invMass > 0.0f ? invMass : 0.0f;
		};

		const float weightA = motionWeight(bodyA, bodyB);
		const float weightB = motionWeight(bodyB, bodyA);
		const float totalWeight = weightA + weightB;
		if (totalWeight <= 0.0f)
			return false;

		const sf::Vector2f blockedDelta = normalForA * closingDelta;
		if (weightA > 0.0f)
			bodyA.remainingDelta -= blockedDelta * (weightA / totalWeight);
		if (weightB > 0.0f)
			bodyB.remainingDelta += blockedDelta * (weightB / totalWeight);

		SyncVelocityFromRemainingDelta(bodyA);
		SyncVelocityFromRemainingDelta(bodyB);

		return true;
	}

	bool CollisionSystem::ApplyContactResponse(BodyRuntime& bodyA, BodyRuntime& bodyB, const sf::Vector2f& normalForA, float penetration)
	{
		bool changed = false;
		bool velocityChanged = false;
		const float invMassA = InverseMass(bodyA);
		const float invMassB = InverseMass(bodyB);
		const float totalInvMass = invMassA + invMassB;

		if (penetration > k_Epsilon && totalInvMass > 0.0f)
		{
			const sf::Vector2f correction = normalForA * penetration;
			if (invMassA > 0.0f)
			{
				const sf::Vector2f moveA = correction * (invMassA / totalInvMass);
				bodyA.position += moveA;
				bodyA.transform->SetPosition(bodyA.position);
				if (bodyA.movement)
					bodyA.movement->m_WasCorrectedByPhysics = true;
				changed = true;
			}
			if (invMassB > 0.0f)
			{
				const sf::Vector2f moveB = correction * (-invMassB / totalInvMass);
				bodyB.position += moveB;
				bodyB.transform->SetPosition(bodyB.position);
				if (bodyB.movement)
					bodyB.movement->m_WasCorrectedByPhysics = true;
				changed = true;
			}
		}

if (totalInvMass <= 0.0f)
		{
			const sf::Vector2f relativeDelta = bodyA.remainingDelta - bodyB.remainingDelta;
			const float closingDelta = DotProd(relativeDelta, normalForA);
			if (closingDelta >= 0.0f)
				return changed;
			if (bodyA.bodyType == BodyType::Kinematic &&
		        bodyB.bodyType == BodyType::Static)
		    {
		        bodyA.remainingDelta -= normalForA * closingDelta;
		        SyncVelocityFromRemainingDelta(bodyA);
		        return true;
		    }
		
		    if (bodyB.bodyType == BodyType::Kinematic &&
		        bodyA.bodyType == BodyType::Static)
		    {
		        bodyB.remainingDelta += normalForA * closingDelta;
		        SyncVelocityFromRemainingDelta(bodyB);
		        return true;
		    }
		
		    return false;
		}

		const sf::Vector2f velocityA = GetVelocity(bodyA);
		const sf::Vector2f velocityB = GetVelocity(bodyB);
		const sf::Vector2f relativeVelocity = velocityA - velocityB;
		const float velAlongNormal = relativeVelocity.x * normalForA.x + relativeVelocity.y * normalForA.y;
		if (velAlongNormal >= 0.0f)
			return changed;

		const float restitutionA = bodyA.physics ? PhysicsSystem::ResolveRestitution(*bodyA.physics) : 0.0f;
		const float restitutionB = bodyB.physics ? PhysicsSystem::ResolveRestitution(*bodyB.physics) : 0.0f;
		const float restitution = std::max(restitutionA, restitutionB);

		const float impulseMagnitude = -(1.0f + restitution) * velAlongNormal / totalInvMass;
		const sf::Vector2f impulse = normalForA * impulseMagnitude;

		if (invMassA > 0.0f)
		{
			SetVelocity(bodyA, velocityA + impulse * invMassA);
			changed = true;
			velocityChanged = true;
		}
		if (invMassB > 0.0f)
		{
			SetVelocity(bodyB, velocityB - impulse * invMassB);
			changed = true;
			velocityChanged = true;
		}

		const auto syncRemainingMotion = [&]()
		{
			if (!velocityChanged)
				return;

			RecomputeRemainingDelta(bodyA);
			RecomputeRemainingDelta(bodyB);
		};

		const sf::Vector2f postA = GetVelocity(bodyA);
		const sf::Vector2f postB = GetVelocity(bodyB);
		sf::Vector2f tangent = (postA - postB) - normalForA * ((postA - postB).x * normalForA.x + (postA - postB).y * normalForA.y);
		const float tangentLength = std::sqrt(tangent.x * tangent.x + tangent.y * tangent.y);
		if (tangentLength <= k_Epsilon)
		{
			syncRemainingMotion();
			return changed;
		}

		tangent /= tangentLength;
		const float frictionA = bodyA.physics ? PhysicsSystem::ResolveFriction(*bodyA.physics) : 0.0f;
		const float frictionB = bodyB.physics ? PhysicsSystem::ResolveFriction(*bodyB.physics) : 0.0f;
		const float friction = std::max(frictionA, frictionB);
		if (friction <= 0.0f)
		{
			syncRemainingMotion();
			return changed;
		}

		float jt = -((postA - postB).x * tangent.x + (postA - postB).y * tangent.y) / totalInvMass;
		const float maxJt = impulseMagnitude * friction;
		jt = std::clamp(jt, -maxJt, maxJt);
		const sf::Vector2f frictionImpulse = tangent * jt;

		if (invMassA > 0.0f)
			SetVelocity(bodyA, GetVelocity(bodyA) + frictionImpulse * invMassA);
		if (invMassB > 0.0f)
			SetVelocity(bodyB, GetVelocity(bodyB) - frictionImpulse * invMassB);

		if ((invMassA > 0.0f || invMassB > 0.0f) && !IsZeroVector(frictionImpulse))
			velocityChanged = true;

		syncRemainingMotion();
		return true;
	}

	void CollisionSystem::ResolveDiscreteCleanup(std::unordered_map<UUID, BodyRuntime>& bodies)
	{
		for (int iteration = 0; iteration < k_MaxDiscreteIterations; iteration++)
		{
			bool appliedCorrection = false;

			// build broadphase
			quadtree.BuildTree(m_Config.bounds);

			std::unordered_map<UUID, sf::FloatRect> currentBounds;
			currentBounds.reserve(bodies.size());
			for (const auto& [id, body] : bodies)
			{
				currentBounds.emplace(id, ComputeBoundsAt(body, body.position));
			}
			quadtree.Populate(currentBounds);

			for (const auto& [entityA, entityB] : quadtree.GeneratePairs())
			{
				sf::Vector2f correctionForA = { 0.0f, 0.0f };
				if (!ComputePairCorrection(bodies, entityA, entityB, correctionForA))
					continue;

				BodyRuntime& bodyA = bodies[entityA];
				BodyRuntime& bodyB = bodies[entityB];
				sf::Vector2f normal = { 0.0f, 0.0f };
				float penetration = std::sqrt(correctionForA.x * correctionForA.x + correctionForA.y * correctionForA.y);
				if (penetration > k_Epsilon)
				{
					normal = correctionForA / penetration;
					appliedCorrection |= ApplyContactResponse(bodyA, bodyB, normal, penetration);
				}
			}

			if (!appliedCorrection)
				break;
		}
	}

	// solve AABB pair and return result
	SweepResult CollisionSystem::SweepAABBPair(const sf::FloatRect& boxA, const sf::Vector2f& deltaA, const sf::FloatRect& boxB, const sf::Vector2f& deltaB)
	{
		SweepResult result;

		sf::Vector2f initialNormal = { 0.0f, 0.0f };
		float initialPenetration = 0.0f;
		if (ComputeAABBContact(boxA, boxB, initialNormal, initialPenetration))
		{
			result.startsTouching = true;
			result.normal = initialNormal;
			if (initialPenetration > k_Epsilon)
			{
				result.hit = true;
				result.toi = 0.0f;
				return result;
			}
		}

		const sf::Vector2f relativeDelta = deltaA - deltaB;

		// determine if touching bodies are moving together, apart, or tangentially
		if (result.startsTouching)
		{
			const float closingDelta = DotProd(relativeDelta, initialNormal);

	        if (closingDelta < -k_Epsilon)
	        {
	            result.hit = true;
	            result.toi = 0.0f;
	        }
	        return result;
		}
		
		if (IsZeroVector(relativeDelta))
			return result;

		float xEntry = 0.0f;
		float xExit = 0.0f;
		float yEntry = 0.0f;
		float yExit = 0.0f;

		if (relativeDelta.x > k_Epsilon)
		{
			xEntry = boxB.position.x - (boxA.position.x + boxA.size.x);
			xExit = (boxB.position.x + boxB.size.x) - boxA.position.x;
		}
		else if (relativeDelta.x < -k_Epsilon)
		{
			xEntry = (boxB.position.x + boxB.size.x) - boxA.position.x;
			xExit = boxB.position.x - (boxA.position.x + boxA.size.x);
		}
		else
		{
			if (boxA.position.x + boxA.size.x < boxB.position.x - k_TOIEpsilon || boxB.position.x + boxB.size.x < boxA.position.x - k_TOIEpsilon)
				return result;
			xEntry = -std::numeric_limits<float>::infinity();
			xExit = std::numeric_limits<float>::infinity();
		}

		if (relativeDelta.y > k_Epsilon)
		{
			yEntry = boxB.position.y - (boxA.position.y + boxA.size.y);
			yExit = (boxB.position.y + boxB.size.y) - boxA.position.y;
		}
		else if (relativeDelta.y < -k_Epsilon)
		{
			yEntry = (boxB.position.y + boxB.size.y) - boxA.position.y;
			yExit = boxB.position.y - (boxA.position.y + boxA.size.y);
		}
		else
		{
			if (boxA.position.y + boxA.size.y < boxB.position.y - k_TOIEpsilon || boxB.position.y + boxB.size.y < boxA.position.y - k_TOIEpsilon)
				return result;
			yEntry = -std::numeric_limits<float>::infinity();
			yExit = std::numeric_limits<float>::infinity();
		}

		const float txEntry = (std::isinf(xEntry) ? xEntry : xEntry / relativeDelta.x);
		const float txExit = (std::isinf(xExit) ? xExit : xExit / relativeDelta.x);
		const float tyEntry = (std::isinf(yEntry) ? yEntry : yEntry / relativeDelta.y);
		const float tyExit = (std::isinf(yExit) ? yExit : yExit / relativeDelta.y);

		const float tEntry = std::max(txEntry, tyEntry);
		const float tExit = std::min(txExit, tyExit);

		if (!std::isfinite(tEntry) && !result.startsTouching)
			return result;
		if (tEntry > tExit)
			return result;
		if (tExit < 0.0f)
			return result;
		if (tEntry > 1.0f)
			return result;

		result.hit = true;
		result.toi = std::clamp(tEntry, 0.0f, 1.0f);
		if (txEntry > tyEntry)
		{
			result.normal = (relativeDelta.x > 0.0f) ? sf::Vector2f{ -1.0f, 0.0f } : sf::Vector2f{ 1.0f, 0.0f };
		}
		else
		{
			result.normal = (relativeDelta.y > 0.0f) ? sf::Vector2f{ 0.0f, -1.0f } : sf::Vector2f{ 0.0f, 1.0f };
		}

		return result;
	}

	void CollisionSystem::CommitRemainingMotion(BodyRuntime& body)
	{
		if (!body.canTranslate)
			return;

		if (!IsZeroVector(body.remainingDelta))
		{
			body.position += body.remainingDelta;
			body.transform->SetPosition(body.position);
		}

		body.remainingTime = 0.0f;
		body.remainingDelta = { 0.0f, 0.0f };
	}

	void CollisionSystem::SyncMovementState(BodyRuntime& body)
	{
		if (!body.physics || !body.movement)
			return;

		body.movement->m_Velocity = body.physics->velocity;
		body.movement->m_ProposedDelta = body.position - body.startPosition;
	}

	bool CollisionSystem::ComputeAABBContact(const sf::FloatRect& boxA, const sf::FloatRect& boxB, sf::Vector2f& normalForA, float& penetration)
	{
		if (!AABBOverlapOrTouch(boxA, boxB))
			return false;

		const float moveLeft = boxB.position.x - (boxA.position.x + boxA.size.x);
		const float moveRight = (boxB.position.x + boxB.size.x) - boxA.position.x;
		const float moveUp = boxB.position.y - (boxA.position.y + boxA.size.y);
		const float moveDown = (boxB.position.y + boxB.size.y) - boxA.position.y;

		const float absLeft = std::abs(moveLeft);
		const float absRight = std::abs(moveRight);
		const float absUp = std::abs(moveUp);
		const float absDown = std::abs(moveDown);

		const float minX = std::min(absLeft, absRight);
		const float minY = std::min(absUp, absDown);

		if (minX <= minY)
		{
			if (absLeft <= absRight)
			{
				normalForA = { -1.0f, 0.0f };
				penetration = std::max(0.0f, -moveLeft);
			}
			else
			{
				normalForA = { 1.0f, 0.0f };
				penetration = std::max(0.0f, moveRight);
			}
		}
		else
		{
			if (absUp <= absDown)
			{
				normalForA = { 0.0f, -1.0f };
				penetration = std::max(0.0f, -moveUp);
			}
			else
			{
				normalForA = { 0.0f, 1.0f };
				penetration = std::max(0.0f, moveDown);
			}
		}

		return true;
	}

	bool CollisionSystem::AABBOverlapOrTouch(const sf::FloatRect& a, const sf::FloatRect& b, float epsilon)
	{
		return a.position.x <= b.position.x + b.size.x + epsilon &&
			a.position.x + a.size.x >= b.position.x - epsilon &&
			a.position.y <= b.position.y + b.size.y + epsilon &&
			a.position.y + a.size.y >= b.position.y - epsilon;
	}

	bool CollisionSystem::IsZeroVector(const sf::Vector2f& value)
	{
		return std::abs(value.x) < k_Epsilon && std::abs(value.y) < k_Epsilon;
	}

	void CollisionSystem::AddTouch(std::vector<UUID>& touched, UUID entity)
	{
		if (std::find(touched.begin(), touched.end(), entity) == touched.end())
			touched.push_back(entity);
	}

	float CollisionSystem::InverseMass(const BodyRuntime& body)
	{
		if (!body.physics)
			return 0.0f;
		if (body.physics->bodyType != BodyType::Dynamic)
			return 0.0f;
		if (body.physics->mass <= k_Epsilon)
			return 0.0f;
		return 1.0f / body.physics->mass;
	}

	// get collider rect
	sf::FloatRect CollisionSystem::ComputeBoundsAt(const BodyRuntime& body, const sf::Vector2f& position)
	{
		return body.collider->GetWorldBounds(position);
	}

	// calculate swept aabb
	sf::FloatRect CollisionSystem::ComputeSweptBounds(const sf::FloatRect& start, const sf::Vector2f& delta)
	{
		// copy starting position and apply delta
		sf::FloatRect end = start;
		end.position += delta;

		// connect the start and end rect to form the swept rect
		const float left = std::min(start.position.x, end.position.x);
		const float top = std::min(start.position.y, end.position.y);
		const float right = std::max(start.position.x + start.size.x, end.position.x + end.size.x);
		const float bottom = std::max(start.position.y + start.size.y, end.position.y + end.size.y);
		return sf::FloatRect({ left, top }, { right - left, bottom - top });
	}

	bool CollisionSystem::ComputePairCorrection(std::unordered_map<UUID, BodyRuntime>& bodies, UUID entityA, UUID entityB, sf::Vector2f& correctionForA)
	{
		if (!bodies.count(entityA) || !bodies.count(entityB))
			return false;

		BodyRuntime& bodyA = bodies[entityA];
		BodyRuntime& bodyB = bodies[entityB];

		const sf::FloatRect boundsA = ComputeBoundsAt(bodyA, bodyA.position);
		const sf::FloatRect boundsB = ComputeBoundsAt(bodyB, bodyB.position);
		if (!AABBOverlapOrTouch(boundsA, boundsB, 0.0f))
			return false;

		const ColliderType typeA = bodyA.collider->GetType();
		const ColliderType typeB = bodyB.collider->GetType();

		bool collided = false;
		if (typeA == ColliderType::AABB && typeB == ColliderType::AABB)
		{
			sf::Vector2f normal = { 0.0f, 0.0f };
			float penetration = 0.0f;
			if (ComputeAABBContact(boundsA, boundsB, normal, penetration))
			{
				collided = true;
				if (penetration > k_Epsilon)
					correctionForA = normal * penetration;
			}
		}
		else if (typeA == ColliderType::Circle && typeB == ColliderType::Circle)
		{
			const sf::Vector2f posA = bodyA.collider->GetWorldOrigin(bodyA.position);
			const sf::Vector2f posB = bodyB.collider->GetWorldOrigin(bodyB.position);
			const auto* circleA = bodyA.collider->AsCircle();
			const auto* circleB = bodyB.collider->AsCircle();
			if (circleA && circleB)
				collided = IntersectCircleCircle(posA, circleA->radius, posB, circleB->radius, correctionForA);
		}
		else if (typeA == ColliderType::AABB && typeB == ColliderType::Circle)
		{
			const sf::Vector2f posB = bodyB.collider->GetWorldOrigin(bodyB.position);
			const auto* circleB = bodyB.collider->AsCircle();
			if (circleB)
				collided = IntersectAABBCircle(boundsA, posB, circleB->radius, correctionForA);
		}
		else if (typeA == ColliderType::Circle && typeB == ColliderType::AABB)
		{
			const sf::Vector2f posA = bodyA.collider->GetWorldOrigin(bodyA.position);
			const auto* circleA = bodyA.collider->AsCircle();
			sf::Vector2f correctionForB = { 0.0f, 0.0f };
			if (circleA && IntersectAABBCircle(boundsB, posA, circleA->radius, correctionForB))
			{
				correctionForA = { -correctionForB.x, -correctionForB.y };
				collided = true;
			}
		}

		if (!collided)
			return false;

		AddTouch(bodyA.collider->touchingThisFrame, entityB);
		AddTouch(bodyB.collider->touchingThisFrame, entityA);
		return true;
	}

	void CollisionSystem::SyncVelocityFromRemainingDelta(BodyRuntime& body)
	{
		if (!body.canTranslate || !body.physics)
			return;

		if (body.remainingTime <= 0.0f)
		{
			SetVelocity(body, { 0.0f, 0.0f });
			return;
		}

		SetVelocity(body, body.remainingDelta / body.remainingTime);
	}

	void CollisionSystem::SetVelocity(BodyRuntime& body, const sf::Vector2f& velocity)
	{
		if (!body.physics)
			return;

		body.physics->velocity = velocity;
		if (body.movement)
			body.movement->m_Velocity = velocity;
	}

	sf::Vector2f CollisionSystem::GetVelocity(const BodyRuntime& body)
	{
		return body.physics ? body.physics->velocity : sf::Vector2f{ 0.0f, 0.0f };
	}

	void CollisionSystem::RecomputeRemainingDelta(BodyRuntime& body)
	{
		if (!body.canTranslate || !body.physics)
		{
			body.remainingDelta = { 0.0f, 0.0f };
			return;
		}

		body.remainingDelta = body.physics->velocity * body.remainingTime;
	}

	bool CollisionSystem::IntersectCircleCircle(const sf::Vector2f& centerA, float radiusA, const sf::Vector2f& centerB, float radiusB, sf::Vector2f& correctionForA)
	{
		const sf::Vector2f delta = centerA - centerB;
		const float distanceSquared = delta.x * delta.x + delta.y * delta.y;
		const float combinedRadius = radiusA + radiusB;
		const float combinedRadiusSquared = combinedRadius * combinedRadius;
		if (distanceSquared >= combinedRadiusSquared)
			return false;

		const float distance = std::sqrt(distanceSquared);
		if (distance > k_Epsilon)
		{
			const sf::Vector2f normal = delta / distance;
			correctionForA = normal * (combinedRadius - distance);
		}
		else
		{
			correctionForA = { combinedRadius, 0.0f };
		}

		return !IsZeroVector(correctionForA);
	}

	bool CollisionSystem::IntersectAABBCircle(const sf::FloatRect& boxA, const sf::Vector2f& circleCenter, float circleRadius, sf::Vector2f& correctionForA)
	{
		const float closestX = std::clamp(circleCenter.x, boxA.position.x, boxA.position.x + boxA.size.x);
		const float closestY = std::clamp(circleCenter.y, boxA.position.y, boxA.position.y + boxA.size.y);
		sf::Vector2f delta = { closestX - circleCenter.x, closestY - circleCenter.y };

		const float distanceSquared = delta.x * delta.x + delta.y * delta.y;
		const float radiusSquared = circleRadius * circleRadius;
		if (distanceSquared >= radiusSquared)
			return false;

		if (distanceSquared > k_Epsilon)
		{
			const float distance = std::sqrt(distanceSquared);
			const sf::Vector2f normal = delta / distance;
			const sf::Vector2f correctionForCircle = normal * (circleRadius - distance);
			correctionForA = { -correctionForCircle.x, -correctionForCircle.y };
			return !IsZeroVector(correctionForA);
		}

		const sf::Vector2f center = {
			boxA.position.x + boxA.size.x * 0.5f,
			boxA.position.y + boxA.size.y * 0.5f
		};
		const sf::Vector2f fromCenter = circleCenter - center;
		const float overlapX = (boxA.size.x * 0.5f + circleRadius) - std::abs(fromCenter.x);
		const float overlapY = (boxA.size.y * 0.5f + circleRadius) - std::abs(fromCenter.y);
		if (overlapX <= overlapY)
			correctionForA = { fromCenter.x < 0.0f ? overlapX : -overlapX, 0.0f };
		else
			correctionForA = { 0.0f, fromCenter.y < 0.0f ? overlapY : -overlapY };

		return !IsZeroVector(correctionForA);
	}

	float CollisionSystem::DotProd(const sf::Vector2f& a, const sf::Vector2f& b)
	{
	    return a.x * b.x + a.y * b.y;
	}
