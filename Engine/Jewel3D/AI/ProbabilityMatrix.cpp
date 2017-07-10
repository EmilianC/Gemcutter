// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "ProbabilityMatrix.h"
#include "Jewel3D/Utilities/Random.h"

#include <cstring>

namespace Jwl
{
	ProbabilityMatrix::ProbabilityMatrix(u32 _numStates, u32 _numActions)
	{
		numStates = _numStates;
		numActions = _numActions;
		data = static_cast<f32*>(malloc(sizeof(f32) * numStates * numActions));

		SetUniform();
	}

	ProbabilityMatrix::ProbabilityMatrix(const ProbabilityMatrix& other)
	{
		numStates = other.numStates;
		numActions = other.numActions;
		data = static_cast<f32*>(malloc(sizeof(f32) * numStates * numActions));

		std::memcpy(data, other.data, sizeof(f32) * numStates * numActions);
	}

	ProbabilityMatrix::ProbabilityMatrix(ProbabilityMatrix&& other)
		: numStates(other.numStates)
		, numActions(other.numActions)
		, data(other.data)
	{
		other.numStates = 0;
		other.numActions = 0;
		other.data = nullptr;
	}

	ProbabilityMatrix::~ProbabilityMatrix()
	{
		free(data);
	}

	void ProbabilityMatrix::Normalize()
	{
		// Accumulate sum for each row in order to normalize.
		for (u32 i = 0; i < numStates; i++)
		{
			f32 sum = 0.0f;

			for (u32 j = 0; j < numActions; j++)
			{
				sum += GetValue(i, j);
			}

			for (u32 j = 0; j < numActions; j++)
			{
				f32 value = GetValue(i, j);
				SetValue(i, j, value / sum);
			}
		}
	}

	void ProbabilityMatrix::SetUniform()
	{
		f32 value = 1.0f / numActions;

		for (u32 i = 0; i < numStates * numActions; i++)
		{
			data[i] = value;
		}
	}

	s32 ProbabilityMatrix::QueryAction(u32 state) const
	{
		// Because the probabilities are normalized we can use a range of [0 - 1].
		f32 randomVal = RandomRange(0.0f, 1.0f);

		// Cascade through all the actions until our random value is larger.
		// This effectively incorporates the weight of each option.
		f32 sum = 0.0f;
		for (u32 i = 0; i < numActions; i++)
		{
			sum += GetValue(state, i);
			if (randomVal <= sum)
			{
				return i;
			}
		}

		// If we made it out of the loop we must take the last action.
		return numActions - 1;
	}

	void ProbabilityMatrix::ReinforceScale(u32 state, u32 action, f32 percentage)
	{
		f32 currentValue = GetValue(state, action);
		currentValue *= percentage;

		// Clamp to safe range.
		if (currentValue >= 0.0f)
		{
			SetValue(state, action, currentValue);
		}
		else
		{
			SetValue(state, action, 0.0f);
		}

		// Ensure normalization is maintained.
		Normalize();
	}

	void ProbabilityMatrix::ReinforceLinear(u32 state, u32 action, f32 value)
	{
		f32 currentValue = GetValue(state, action);
		currentValue += value;

		if (currentValue >= 0.0f)
		{
			SetValue(state, action, currentValue);
		}
		else
		{
			SetValue(state, action, 0.0f);
		}

		// Ensure normalization is maintained.
		Normalize();
	}

	f32 ProbabilityMatrix::GetValue(u32 state, u32 action) const
	{
		return data[action + (state * numActions)];
	}

	void ProbabilityMatrix::SetValue(u32 state, u32 action, f32 value)
	{
		data[action + (state * numActions)] = value;
	}

	s32 ProbabilityMatrix::GetNumStates() const
	{
		return numStates;
	}

	s32 ProbabilityMatrix::GetNumActions() const
	{
		return numActions;
	}
}
