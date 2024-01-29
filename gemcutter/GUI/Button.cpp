// Copyright (c) 2021 Emilian Cioca
#include "Button.h"
#include "gemcutter/Application/HierarchicalEvent.h"
#include "gemcutter/GUI/Image.h"
#include "gemcutter/GUI/Label.h"
#include "gemcutter/Input/Input.h"
#include "gemcutter/Sound/SoundSource.h"

namespace gem
{
	Button::Button(Entity& _owner)
		: Widget(_owner)
	{
		image = &CreateChild<Image>();
		label = &CreateChild<Label>();

		auto& onKeyPressed  = owner.Require<HierarchicalListener<KeyPressed>>();
		auto& onKeyReleased = owner.Require<HierarchicalListener<KeyReleased>>();
		owner.Require<SoundSource>();

		onKeyPressed.callback.BindOwned([this](const KeyPressed& e) {
			if (state == State::Hover && e.key == Key::MouseLeft)
			{
				SetState(State::Pressed);
				return true;
			}

			return false;
		});

		onKeyReleased.callback.BindOwned([this](const KeyReleased& e) {
			if (state == State::Pressed && e.key == Key::MouseLeft)
			{
				const Rectangle& bounds = GetAbsoluteBounds();
				const vec2 mousePos = Input.GetMousePos();
				if (!bounds.Contains(mousePos))
				{
					SetState(State::Idle);
					return true;
				}

				onClick.Dispatch(*this);

				if (pressedSound)
				{
					auto& soundSource = owner.Get<SoundSource>();
					soundSource.SetData(pressedSound);
					soundSource.Play();
				}

				SetState(State::Hover);

				return true;
			}

			return false;
		});
	}

	Button::Button(Entity& _owner, Font::Ptr _font, std::string_view _text)
		: Button(_owner)
	{
		label->SetFont(std::move(_font));
		label->SetString(_text);
	}

	Button::Button(Entity& _owner, Texture::Ptr idle, Texture::Ptr hover, Texture::Ptr pressed)
		: Button(_owner)
	{
		idleTexture = std::move(idle);
		hoverTexture = std::move(hover);
		pressedTexture = std::move(pressed);

		SetTexture(State::Idle);
	}

	Button::Button(Entity& _owner, Font::Ptr _font, std::string_view _text, Texture::Ptr idle, Texture::Ptr hover, Texture::Ptr pressed)
		: Button(_owner)
	{
		label->SetFont(std::move(_font));
		label->SetString(_text);

		idleTexture = std::move(idle);
		hoverTexture = std::move(hover);
		pressedTexture = std::move(pressed);

		SetTexture(State::Idle);
	}

	void Button::Update()
	{
		const Rectangle& bounds = GetAbsoluteBounds();
		const vec2 mousePos = Input.GetMousePos();
		const bool isInside = bounds.Contains(mousePos);

		if (state == State::Pressed)
		{
			// We don't want to change the state here because the button is still
			// in focus until the key is released. We just want to change the texture.
			SetTexture(isInside ? State::Pressed : State::Hover);
		}
		else
		{
			if (isInside && !Input.IsDown(Key::MouseLeft))
			{
				SetState(State::Hover);
			}
			else
			{
				SetState(State::Idle);
			}
		}
	}

	Image& Button::GetImage()
	{
		return *image;
	}

	Label& Button::GetLabel()
	{
		return *label;
	}

	const Image& Button::GetImage() const
	{
		return *image;
	}

	const Label& Button::GetLabel() const
	{
		return *label;
	}

	Button::State Button::GetState() const
	{
		return state;
	}

	void Button::SetState(State newState)
	{
		if (state == newState)
		{
			return;
		}

		state = newState;
		SetTexture(state);
	}

	void Button::SetTexture(State _state)
	{
		switch(_state)
		{
			case State::Idle:
			image->SetTexture(idleTexture);
			break;

			case State::Hover:
			image->SetTexture(hoverTexture);
			break;

			case State::Pressed:
			image->SetTexture(pressedTexture);
			break;
		}
	}

	void Button::OnDisable()
	{
		SetState(State::Idle);

		image->owner.Disable();
		label->owner.Disable();
	}

	void Button::OnEnable()
	{
		image->owner.Enable();
		label->owner.Enable();
	}
}

REFLECT(gem::Button::State)
	ENUM_VALUES {
		REF_VALUE(Idle)
		REF_VALUE(Hover)
		REF_VALUE(Pressed)
	}
REF_END;

REFLECT_COMPONENT(gem::Button, gem::Widget)
	MEMBERS {
		REF_MEMBER(pressedSound)
		REF_MEMBER(idleTexture)
		REF_MEMBER(hoverTexture)
		REF_MEMBER(pressedTexture)
		REF_PRIVATE_MEMBER(state, gem::ReadOnly)
	}
REF_END;
