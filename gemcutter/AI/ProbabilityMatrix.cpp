// Copyright (c) 2017 Emilian Cioca
#include "ProbabilityMatrix.h"
#include "gemcutter/Application/Logging.h"
#include "gemcutter/Utilities/Random.h"

#include <cstring>

namespace Jwl
{
	ProbabilityMatrix::ProbabilityMatrix(unsigned _numStates, unsigned _numActions)
	{
		ASSERT(_numStates != 0, "Cannot have zero states.");
		ASSERT(_numActions != 0, "Cannot have zero actions.");

		numStates = _numStates;
		numActions = _numActions;
		data = static_cast<float*>(malloc(sizeof(float) * numStates * numActions));

		Reset();
	}

	ProbabilityMatrix::ProbabilityMatrix(const ProbabilityMatrix& other)
	{
		*this = other;
	}

	ProbabilityMatrix::ProbabilityMatrix(ProbabilityMatrix&& other) noexcept
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

	ProbabilityMatrix& ProbabilityMatrix::operator=(const ProbabilityMatrix& other)
	{
		const unsigned oldSize = numStates * numActions;
		numStates = other.numStates;
		numActions = other.numActions;

		if (oldSize != numStates * numActions)
		{
			data = static_cast<float*>(realloc(data, sizeof(float) * numStates * numActions));
		}

		std::memcpy(data, other.data, sizeof(float) * numStates * numActions);

		return *this;
	}

	ProbabilityMatrix& ProbabilityMatrix::operator=(ProbabilityMatrix&& other) noexcept
	{
		numStates = other.numStates;
		numActions = other.numActions;
		data = other.data;

		other.numStates = 0;
		other.numActions = 0;
		other.data = nullptr;

		return *this;
	}

	void ProbabilityMatrix::Normalize()
	{
		// Accumulate sum for each row in order to normalize.
		for (unsigned i = 0; i < numStates; ++i)
		{
			float sum = 0.0f;

			for (unsigned j = 0; j < numActions; ++j)
			{
				sum += GetValue(i, j);
			}

			for (unsigned j = 0; j < numActions; ++j)
			{
				float value = GetValue(i, j);
				SetValue(i, j, value / sum);
			}
		}
	}

	void ProbabilityMatrix::Reset()
	{
		float value = 1.0f / numActions;

		for (unsigned i = 0; i < numStates * numActions; ++i)
		{
			data[i] = value;
		}
	}

	int ProbabilityMatrix::QueryAction(unsigned state) const
	{
		// Because the probabilities are normalized we can use a range of [0 - 1].
		float randomVal = RandomRange(0.0f, 1.0f);

		// Cascade through all the actions until our random value is larger.
		// This effectively incorporates the weight of each option.
		float sum = 0.0f;
		for (unsigned i = 0; i < numActions; ++i)
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

	void ProbabilityMatrix::ReinforceScale(unsigned state, unsigned action, float percentage)
	{
		float currentValue = GetValue(state, action);
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

	void ProbabilityMatrix::ReinforceLinear(unsigned state, unsigned action, float value)
	{
		float currentValue = GetValue(state, action);
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

	float ProbabilityMatrix::GetValue(unsigned state, unsigned action) const
	{
		return data[action + (state * numActions)];
	}

	void ProbabilityMatrix::SetValue(unsigned state, unsigned action, float value)
	{
		data[action + (state * numActions)] = value;
	}

	int ProbabilityMatrix::GetNumStates() const
	{
		return numStates;
	}

	int ProbabilityMatrix::GetNumActions() const
	{
		return numActions;
	}
}
