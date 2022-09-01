#pragma once
#include "RenderInstruction.hpp"

class RenderQueue
{
public:

	RenderQueue& operator+=( RenderInstruction a_Instruction )
	{
		if ( a_Instruction.Modification != RenderInstruction::Modification::NONE )
		{
			m_RenderInstructions.push_back( a_Instruction );
		}

		return *this;
	}

private:

	inline void Sort()
	{
		// Need to sort the queue here somehow.
	}

	inline RenderInstruction& Front()
	{
		return m_RenderInstructions.front();
	}

	inline void Pop()
	{
		m_RenderInstructions.pop_front();
	}

	inline bool Empty()
	{
		return m_RenderInstructions.empty();
	}

	friend class RenderingPipeline;

	std::list< RenderInstruction > m_RenderInstructions;
};