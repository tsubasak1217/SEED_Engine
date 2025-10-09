#pragma once
// 以下に作成したコンポーネントをincludeしてください
#include <SEED/Source/Basic/Components/IComponent.h>

// 2D専用のコンポーネント
#include <SEED/Source/Basic/Components/Only2D/Collision2DComponent.h>
#include <SEED/Source/Basic/Components/Only2D/UIComponent.h>
#include <SEED/Source/Basic/Components/Only2D/AnimCurve2DComponent.h>

// 3D専用のコンポーネント
#include <SEED/Source/Basic/Components/Only3D/ModelRenderComponent.h>
#include <SEED/Source/Basic/Components/Only3D/Collision3DComponent.h>
#include <SEED/Source/Basic/Components/Only3D/SpotLightComponent.h>
#include <SEED/Source/Basic/Components/Only3D/Gravity3DComponent.h>
#include <SEED/Source/Basic/Components/Only3D/Move3DComponent.h>
#include <SEED/Source/Basic/Components/Only3D/Routine3DComponent.h>
#include <SEED/Source/Basic/Components/Only3D/AnimCurveComponent.h>

// 3D・2D共通のコンポーネント
#include <SEED/Source/Basic/Components/3D&2D/TextComponent.h>
#include <SEED/Source/Basic/Components/3D&2D/JumpComponent.h>