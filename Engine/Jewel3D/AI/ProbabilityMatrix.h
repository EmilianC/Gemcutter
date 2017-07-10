// Copyright (c) 2017 Emilian Cioca
#pragma once

namespace Jwl
{
	//- Stores and manages a probability matrix of States vs Actions.
	class ProbabilityMatrix
	{
	public:
		ProbabilityMatrix(u32 numStates, u32 numActions);
		ProbabilityMatrix(const ProbabilityMatrix&);
		ProbabilityMatrix(ProbabilityMatrix&&);
		~ProbabilityMatrix();

		void Normalize();

		//- Re-initialize the matrix with uniform probabilities.
		void SetUniform();

		//- Based on the given state, returns a random action with respect to the probabilities.
		s32 QueryAction(u32 state) const;

		//- Reinforces (positively or negatively) an action by the scalar, percentage.
		void ReinforceScale(u32 state, u32 action, f32 percentage);

		//- Reinforces (positively or negatively) an action by the additive, value.
		void ReinforceLinear(u32 state, u32 action, f32 value);

		f32 GetValue(u32 state, u32 action) const;
		s32 GetNumStates() const;
		s32 GetNumActions() const;

	private:
		u32 numStates	= 0;
		u32 numActions	= 0;
		f32* data			= nullptr;

		void SetValue(u32 state, u32 action, f32 value);
	};
}
