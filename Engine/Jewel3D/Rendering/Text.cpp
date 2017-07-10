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

	Text::Text(Entity& _owner, Font::Ptr font)
		: Component(_owner)
	{
		SetFont(font);
	}

	Text::Text(Entity& _owner, const std::string& _text)
		: Component(_owner)
		, text(_text)
	{
	}

	Text::Text(Entity& _owner, const std::string& _text, Font::Ptr font)
		: Component(_owner)
		, text(_text)
	{
		SetFont(font);
	}

	void Text::SetFont(Font::Ptr _data)
	{
		data = _data;
	}

	Font::Ptr Text::GetFont() const
	{
		return data;
	}
	
	u32 Text::GetNumLines() const
	{
		u32 count = 1;
		for (auto c : text)
		{
			if (c == '\n')
			{
				count++;
			}
		}

		return count;
	}
	
	f32 Text::GetLineWidth(u32 line) const
	{
		ASSERT(line != 0, "'line' must be greater than 0.");
		ASSERT(data != nullptr, "Must have a Font attached to query width.");

		if (line > GetNumLines())
		{
			return 0.0f;
		}
		else if (line == 1)
		{
			size_t loc = text.find('\n');
			if (loc == std::string::npos)
			{
				return static_cast<f32>(data->GetStringWidth(text));
			}
			else
			{
				return static_cast<f32>(data->GetStringWidth(text.substr(0, loc)));
			}
		}
		else
		{
			size_t start = std::string::npos;
			u32 count = 1;
			for (u32 i = 0; i < text.size(); i++)
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
			for (u32 i = start + 1; i < text.size(); i++)
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

			return static_cast<f32>(data->GetStringWidth(text.substr(start, end)));
		}
	}
}
