// Copyright (c) 2017 Emilian Cioca
#include "Text.h"
#include "gemcutter/Application/Logging.h"
#include "gemcutter/Resource/Font.h"

namespace
{
	const gem::vec3 GLYPH_NORMAL = gem::vec3(0.0f, 0.0f, 1.0f);
}

namespace gem
{
	Text::Text(Entity& _owner)
		: Renderable(_owner)
	{
	}

	Text::Text(Entity& _owner, Font::Ptr _font)
		: Renderable(_owner)
	{
		SetFont(std::move(_font));
	}

	Text::Text(Entity& _owner, std::string _string)
		: Renderable(_owner)
		, string(std::move(_string))
	{
		owner.Tag<detail::StaleStringMesh>();
	}

	Text::Text(Entity& _owner, Material::Ptr material)
		: Renderable(_owner, std::move(material))
	{
	}

	Text::Text(Entity& _owner, Font::Ptr _font, std::string _string, Material::Ptr material)
		: Renderable(_owner, std::move(material))
		, string(std::move(_string))
	{
		SetFont(std::move(_font));
	}

	void Text::SetString(std::string str)
	{
		string = std::move(str);
		owner.Tag<detail::StaleStringMesh>();
	}

	const std::string& Text::GetString() const
	{
		return string;
	}

	void Text::SetFont(Font::Ptr newFont)
	{
		font = std::move(newFont);
		if (font)
		{
			textures.Add(font->GetTexture()->GetPtr(), 0);
		}

		owner.Tag<detail::StaleStringMesh>();
	}

	Font::Ptr Text::GetFont() const
	{
		return font;
	}

	void Text::SetCenteredX(bool newState)
	{
		if (centeredX != newState)
		{
			centeredY = newState;
			owner.Tag<detail::StaleStringMesh>();
		}
	}

	bool Text::GetCenteredX() const
	{
		return centeredX;
	}

	void Text::SetCenteredY(bool newState)
	{
		if (centeredY != newState)
		{
			centeredY = newState;
			owner.Tag<detail::StaleStringMesh>();
		}
	}

	bool Text::GetCenteredY() const
	{
		return centeredY;
	}

	void Text::SetKerning(float newValue)
	{
		if (kerning != newValue)
		{
			kerning = newValue;
			owner.Tag<detail::StaleStringMesh>();
		}
	}

	float Text::GetKerning() const
	{
		return kerning;
	}

	unsigned Text::GetNumLines() const
	{
		return 1 + std::count(string.begin(), string.end(), '\n');
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
			for (size_t i = start + 1; i < string.size(); ++i)
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

	void Text::UpdateAll()
	{
		for (auto& entity : With<detail::StaleStringMesh>())
		{
			entity.Get<Text>().UpdateStringBuffer();
		}

		Entity::GlobalRemoveTag<detail::StaleStringMesh>();
	}

	void Text::UpdateStringBuffer()
	{
		const unsigned requiredSize = static_cast<unsigned>(string.size());
		if (!stringBuffer) [[unlikely]]
		{
			CreateStringBuffer(requiredSize);
		}
		else
		{
			ResizeStringBuffer(requiredSize);
		}

		array->SetVertexCount(string.size() * 6);

		std::span<const Font::Char> characters = font->GetCharacters();

		const vec3 advanceDirection = vec3::Right;
		const vec3 returnDirection  = vec3::Down;
		vec3 linePosition;
		vec3 penPosition;
		unsigned currentLine = 1;

		if (centeredX)
		{
			linePosition -= advanceDirection * ((GetLineWidth(currentLine) + kerning * string.size()) / 2.0f);
		}

		if (centeredY)
		{
			linePosition += returnDirection * ((font->GetStringHeight() * static_cast<float>(GetNumLines())) / 2.0f);
		}

		BufferMapping mapping = stringBuffer->MapBuffer(VertexAccess::WriteOnly);
		std::byte* bufferPtr = mapping.GetPtr();

		bufferPtr += sizeof(vec3); // Skip past the static normal.
		float* vertexWriter = reinterpret_cast<float*>(bufferPtr);
		bufferPtr += sizeof(vec3) * 3;
		float* uvWriter = reinterpret_cast<float*>(bufferPtr);

		const size_t writerStride = 3 + 2; // is this value right?

		for (char character : string)
		{
			// Handle whitespace.
			if (character == ' ')
			{
				penPosition += advanceDirection * (font->GetSpaceWidth() + kerning);
				continue;
			}
			else if (character == '\n')
			{
				linePosition += returnDirection * static_cast<float>(font->GetStringHeight()) * 1.33f;
				currentLine++;

				if (centeredX)
				{
					linePosition -= advanceDirection * ((GetLineWidth(currentLine) + kerning * string.size()) / 2.0f);
				}

				continue;
			}
			else if (character == '\t')
			{
				penPosition += advanceDirection * (font->GetSpaceWidth() + kerning) * 4;
				continue;
			}

			const unsigned charIndex = static_cast<unsigned>(character) - '!';
			if (charIndex >= characters.size())
			{
				Warning("Text: Skipped unsupported char: '%c'", character);
				penPosition += advanceDirection * (font->GetSpaceWidth() + kerning);
				continue;
			}

			const Font::Char& charData = characters[charIndex];
			if (!charData.isValid)
			{
				Warning("Text: Skipped char without valid font data: '%c'", character);
				penPosition += advanceDirection * (font->GetSpaceWidth() + kerning);
				continue;
			}

			/* Adjusts the node's position based on the character. */
			vec3 characterPosition = linePosition + penPosition;
			characterPosition.x += static_cast<float>(charData.offsetX);
			characterPosition.y += static_cast<float>(charData.offsetY);

			/* Construct a polygon based on the current character's dimensions. */
			vertexWriter[0]  = characterPosition.x;
			vertexWriter[1]  = characterPosition.y;
			vertexWriter[2]  = characterPosition.z;
			vertexWriter[3]  = characterPosition.x + static_cast<float>(charData.width);
			vertexWriter[4]  = characterPosition.y;
			vertexWriter[5]  = characterPosition.z;
			vertexWriter[6]  = characterPosition.x;
			vertexWriter[7]  = characterPosition.y + static_cast<float>(charData.height);
			vertexWriter[8]  = characterPosition.z;
			vertexWriter[9]  = characterPosition.x + static_cast<float>(charData.width);
			vertexWriter[10] = characterPosition.y;
			vertexWriter[11] = characterPosition.z;
			vertexWriter[12] = characterPosition.x + static_cast<float>(charData.width);
			vertexWriter[13] = characterPosition.y + static_cast<float>(charData.height);
			vertexWriter[14] = characterPosition.z;
			vertexWriter[15] = characterPosition.x;
			vertexWriter[16] = characterPosition.y + static_cast<float>(charData.height);
			vertexWriter[17] = characterPosition.z;

			uvWriter[0]  = charData.UvLeft;
			uvWriter[1]  = charData.UvBottom;
			uvWriter[2]  = charData.UvRight;
			uvWriter[3]  = charData.UvBottom;
			uvWriter[4]  = charData.UvLeft;
			uvWriter[5]  = charData.UvTop;
			uvWriter[6]  = charData.UvRight;
			uvWriter[7]  = charData.UvBottom;
			uvWriter[8]  = charData.UvRight;
			uvWriter[9]  = charData.UvTop;
			uvWriter[10] = charData.UvLeft;
			uvWriter[11] = charData.UvTop;

			vertexWriter += writerStride;
			uvWriter     += writerStride;

			// Advance to next character.
			penPosition += advanceDirection * (charData.advanceX + kerning);
		}
	}

	void Text::CreateStringBuffer(unsigned requiredSize)
	{
		requiredSize += sizeof(GLYPH_NORMAL);

		stringBuffer = VertexBuffer::MakeNew(requiredSize, BufferUsage::Dynamic, VertexBufferType::Data);
		stringBuffer->SetData(0, sizeof(GLYPH_NORMAL), &GLYPH_NORMAL);

		const unsigned int stride = sizeof(vec3) + sizeof(vec2);

		// Vertex positions.
		array->AddStream({
			.buffer       = stringBuffer,
			.bindingUnit  = 0,
			.format       = VertexFormat::Vec3,
			.startOffset  = sizeof(vec3),
			.stride       = stride
		});

		// UV coordinates.
		array->AddStream({
			.buffer       = stringBuffer,
			.bindingUnit  = 1,
			.format       = VertexFormat::Vec2,
			.startOffset  = sizeof(vec3) + sizeof(vec2),
			.stride       = stride
		});

		// Normals.
		array->AddStream({
			.buffer       = stringBuffer,
			.bindingUnit  = 2,
			.format       = VertexFormat::Vec3,
			.startOffset  = 0,
			.stride       = 0
		});
	}

	void Text::ResizeStringBuffer(unsigned requiredSize)
	{
		requiredSize += sizeof(GLYPH_NORMAL);

		if (stringBuffer->GetSize() < requiredSize)
		{
			stringBuffer->Resize(requiredSize, true);
		}
	}
}

REFLECT_COMPONENT(gem::Text, gem::Renderable)
	MEMBERS {
		REF_PRIVATE_MEMBER_GET_SET(font,      nullptr, &gem::Text::SetFont)
		REF_PRIVATE_MEMBER_GET_SET(centeredX, nullptr, &gem::Text::SetCenteredX)
		REF_PRIVATE_MEMBER_GET_SET(centeredY, nullptr, &gem::Text::SetCenteredY)
		REF_PRIVATE_MEMBER_GET_SET(string,    nullptr, &gem::Text::SetString)
		REF_PRIVATE_MEMBER_GET_SET(kerning,   nullptr, &gem::Text::SetKerning, description("Extra spacing between letters."))
	}
REF_END;
