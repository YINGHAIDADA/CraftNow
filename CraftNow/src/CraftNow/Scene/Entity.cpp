﻿#include "cnpch.h"
#include "Entity.h"

namespace CraftNow {
	Entity::Entity(entt::entity handle, Scene* scene)
		: m_EntityHandle(handle), m_Scene(scene)
	{
	}
}

