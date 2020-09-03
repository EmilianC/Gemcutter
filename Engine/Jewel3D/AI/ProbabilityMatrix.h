// Copyright (c) 2017 Emilian Cioca
#pragma once

namespace Jwl
{
	// Stores and manages a probability matrix of States vs Actions.
	class ProbabilityMatrix
	{
	public:
		ProbabilityMatrix(unsigned numStates, unsigned numActions);
		ProbabilityMatrix(const ProbabilityMatrix&);
		ProbabilityMatrix(ProbabilityMatrix&&) noexcept;
		~ProbabilityMatrix();

		ProbabilityMatrix& operator=(const ProbabilityMatrix&);
		ProbabilityMatrix& operator=(ProbabilityMatrix&&) noexcept;

		// Re-initialize the matrix with uniform probabilities.
		void Reset();

		// Based on the given state, returns a random action with respect to the probabilities.
		int QueryAction(unsigned state) const;

		// Reinforces (positively or negatively) an action by the scalar, percentage.
		void ReinforceScale(unsigned state, unsigned action, float percentage);

		// Reinforces (positively or negatively) an action by the additive, value.
		void ReinforceLinear(unsigned state, unsigned action, float value);

		// Returns the probability of taking an action from a given state.
		float GetValue(unsigned state, unsigned action) const;
		int GetNumStates() const;
		int GetNumActions() const;

	private:
		unsigned numStates  = 0;
		unsigned numActions = 0;
		float* data = nullptr;

		void Normalize();
		void SetValue(unsigned state, unsigned action, float value);
	};
}
