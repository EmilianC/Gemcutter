// Copyright (c) 2021 Emilian Cioca
#pragma once
#include "gemcutter/Application/Delegate.h"
#include "gemcutter/GUI/Widget.h"
#include "gemcutter/Resource/Sound.h"
#include "gemcutter/Resource/Texture.h"
#include "gemcutter/Resource/Font.h"

#include <string_view>

namespace gem
{
	class Image;
	class Label;

	// An interactable button with a label and automatically updating background image.
	class Button : public Widget
	{
	public:
		Button(Entity& owner);
		Button(Entity& owner, Font::Ptr font, std::string_view text);
		Button(Entity& owner, Texture::Ptr idle, Texture::Ptr hover, Texture::Ptr pressed);
		Button(Entity& owner, Font::Ptr font, std::string_view text, Texture::Ptr idle, Texture::Ptr hover, Texture::Ptr pressed);

		enum class State
		{
			Idle,
			Hover,
			Pressed
		};

		void Update();

		Image& GetImage();
		Label& GetLabel();
		const Image& GetImage() const;
		const Label& GetLabel() const;
		State GetState() const;

		Sound::Ptr pressedSound;
		Texture::Ptr idleTexture;
		Texture::Ptr hoverTexture;
		Texture::Ptr pressedTexture;

		Dispatcher<void(Button&)> onClick;

	private:
		void SetState(State);
		void SetTexture(State);

		void OnDisable() override;
		void OnEnable() override;

		State state = State::Idle;
		Image* image = nullptr;
		Label* label = nullptr;
	};
}
