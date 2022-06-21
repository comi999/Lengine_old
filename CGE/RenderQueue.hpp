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

	std::list< RenderInstruction > m_RenderInstructions;
};