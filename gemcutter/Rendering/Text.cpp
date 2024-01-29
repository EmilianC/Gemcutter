// Copyright (c) 2017 Emilian Cioca
#include "Text.h"
#include "gemcutter/Application/Logging.h"
#include "gemcutter/Resource/Font.h"

namespace gem
{
	Text::Text(Entity& _owner)
		: Renderable(_owner)
	{
	}

	Text::Text(Entity& _owner, Font::Ptr _font)
		: Renderable(_owner)
		, font(std::move(_font))
	{
	}

	Text::Text(Entity& _owner, std::string _string)
		: Renderable(_owner)
		, string(std::move(_string))
	{
	}

	Text::Text(Entity& _owner, Material::Ptr material)
		: Renderable(_owner, std::move(material))
	{
	}

	Text::Text(Entity& _owner, Font::Ptr _font, std::string _string, Material::Ptr material)
		: Renderable(_owner, std::move(material))
		, font(std::move(_font))
		, string(std::move(_string))
	{
	}

	unsigned Text::GetNumLines() const
	{
		if (string.empty())
		{
			return 0;
		}

		unsigned count = 1;
		const char* ptr = string.data();
		while (true)
		{
			switch (*ptr)
			{
			case '\n':
				++count;
				break;

			case '\0':
				return count;
			}

			++ptr;
		}
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
			size_t loc = string.find('\n');
			if (loc == std::string::npos)
			{
				return static_cast<float>(font->GetStringWidth(string));
			}
			else
			{
				return static_cast<float>(font->GetStringWidth({ string.begin(), string.begin() + loc }));
			}
		}
		else
		{
			size_t start = std::string::npos;
			unsigned count = 1;
			for (unsigned i = 0; i < string.size(); ++i)
			{
				if (string[i] == '\n')
				{
					count++;
					if (count == line)
					{
						start = i;
						break;
					}
				}
			}

			size_t end = string.size();
			for (unsigned i = start + 1; i < string.size(); ++i)
			{
				if (string[i] == '\n')
				{
					end = i;
					break;
				}
			}

			if (start == std::string::npos)
			{
				return 0.0f;
			}

			return static_cast<float>(font->GetStringWidth({ string.begin() + start, string.begin() + end }));
		}
	}
}

REFLECT_COMPONENT(gem::Text, gem::Renderable)
	MEMBERS {
		REF_MEMBER(font)
		REF_MEMBER(string)
		REF_MEMBER(centeredX)
		REF_MEMBER(centeredY)
		REF_MEMBER(kerning)
	}
REF_END;
