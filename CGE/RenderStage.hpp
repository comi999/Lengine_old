#pragma once
#include <stdint.h>

enum class RenderStage : uint8_t
{
	// Query all renderers for render draw calls.
	// Reject any calls where a mesh is outside
	// frustum.
	COLLATE_DRAW_CALLS,

	// Sort all instructions by mesh, or some other
	// algorithm.
	POPULATE_INSTRUCTION_QUEUE,

	// Setup VAO's, VBO's, and other settings,
	// run shader pipeline.
	PROCESS_INSTRUCTION_QUEUE,

	// Waiting for to be reset.
	END
};