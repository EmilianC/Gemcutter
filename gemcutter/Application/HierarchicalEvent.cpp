// Copyright (c) 2023 Emilian Cioca
#include "HierarchicalEvent.h"
#include "gemcutter/Application/Application.h"
#include "gemcutter/Input/Input.h"

#include <loupe/loupe.h>

REFLECT_SIMPLE(gem::HierarchicalListener<gem::KeyPressed>);
REFLECT_SIMPLE(gem::HierarchicalListener<gem::KeyReleased>);
REFLECT_SIMPLE(gem::HierarchicalListener<gem::MouseMoved>);
REFLECT_SIMPLE(gem::HierarchicalListener<gem::MouseScrolled>);
REFLECT_SIMPLE(gem::HierarchicalListener<gem::Resize>);

REFLECT_SIMPLE(gem::HierarchicalDispatcher<gem::KeyPressed>);
REFLECT_SIMPLE(gem::HierarchicalDispatcher<gem::KeyReleased>);
REFLECT_SIMPLE(gem::HierarchicalDispatcher<gem::MouseMoved>);
REFLECT_SIMPLE(gem::HierarchicalDispatcher<gem::MouseScrolled>);
REFLECT_SIMPLE(gem::HierarchicalDispatcher<gem::Resize>);
