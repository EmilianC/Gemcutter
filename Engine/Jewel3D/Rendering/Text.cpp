// Copyright (c) 2017 Emilian Cioca
#include "Jewel3D/Precompiled.h"
#include "Text.h"
#include "Jewel3D/Application/Logging.h"
#include "Jewel3D/Resource/Font.h"

namespace Jwl
{
	Text::Text(Entity& _owner)
		: Component(_owner)
	{
	}

	Text::Text(Entity& _owner, Font::Ptr _font)
		: Component(_owner)
		, font(_font)
	{
	}

	Text::Text(Entity& _owner, const std::string& _text)
		: Component(_owner)
		, text(_text)
	{
	}

	Text::Text(Entity& _owner, const std::string& _text, Font::Ptr _font)
		: Component(_owner)
		, text(_text)
		, font(_font)
	{
	}

	unsigned Text::GetNumLines() const
	{
		unsigned count = 1;
		for (auto c : text)
		{
			if (c == '\n')
			{
				count++;
			}
		}

		return count;
	}
	
	float Text::GetLineWidth(unsigned line) const
	{
		ASSERT(line != 0, "'line' must be greater than 0.");
		ASSERT(font != nullptr, "Must have a Font attached to query width.");

		if (line > GetNumLines())
		{
			return 0.0f;
		}
		else if (line == 1)
		{
			size_t loc = text.find('\n');
			if (loc == std::string::npos)
			{
				return static_cast<float>(font->GetStringWidth(text));
			}
			else
			{
				return static_cast<float>(font->GetStringWidth(text.substr(0, loc)));
			}
		}
		else
		{
			size_t start = std::string::npos;
			unsigned count = 1;
			for (unsigned i = 0; i < text.size(); i++)
			{
				if (text[i] == '\n')
				{
					count++;
					if (count == line)
					{
						start = i;
						break;
					}
				}
			}

			size_t end = std::string::npos;
			for (unsigned i = start + 1; i < text.size(); i++)
			{
				if (text[i] == '\n')
				{
					end = i;
					break;
				}
			}

			if (start == std::string::npos)
			{
				return 0.0f;
			}

			return static_cast<float>(font->GetStringWidth(text.substr(start, end)));
		}
	}
}
