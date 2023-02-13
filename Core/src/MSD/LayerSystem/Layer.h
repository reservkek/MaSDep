#pragma once

#include "../Core.h"
#include "../Events/Event.h"
#include "Graphics/Internal/Timestep.h"

namespace MSD {

	class MSD_API Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(Timestep ts) {}
		virtual void OnEvent(Event& event) {}

		inline const std::string& GetDebugName() const { return m_DebugName; }
	protected:
		std::string m_DebugName;
	};

}