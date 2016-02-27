#include "actor.h"
#include "scene.h"

list<unique_ptr<Enemy>> g_enemies;
list<unique_ptr<Bomb>> g_bombs;


list<unique_ptr<Enemy>>& getEnemyList()
{
    return g_enemies;
}

list<unique_ptr<Bomb>>& getBombList()
{
    return g_bombs;
}

void beginDraw(RenderTexture& renderTexture)
{
    Graphics2D::SetRenderTarget(renderTexture);
    Graphics2D::SetBlendState(BlendState::Default);
    Graphics2D::SetSamplerState(SamplerState::ClampPoint);
}

void endDraw(RenderTexture& renderTexture)
{
    Graphics2D::SetRenderTarget(Graphics::GetSwapChainTexture());
    Graphics2D::SetBlendState(BlendState::Default);
    Graphics2D::SetSamplerState(SamplerState::ClampPoint);
    renderTexture.scale(SCALE).draw();
}

void Main()
{
    RenderTexture renderTexture{ SCREEN_W, SCREEN_H, Palette::Black };

    TextureAsset::Register(L"a", L"Assets/a.png");
    FontManager::Register(L"Assets/FFFIXING09.TTF");
    FontAsset::Register(L"font", 7, L"FF Fixing09");
    FontAsset::Register(L"fontM", 10, L"FF Fixing09");
    FontAsset::Register(L"fontL", 14, L"FF Fixing09");
    

    list<unique_ptr<Scene>> scenes;
    scenes.push_back(unique_ptr<Scene>(new SceneTitle(&scenes)));


	while (System::Update())
	{
        scenes.back()->update();
        if (scenes.empty()) { break; }

        beginDraw(renderTexture);
        for (auto& s : scenes) {
            s->draw();
        }
        endDraw(renderTexture);

        scenes.back()->postUpdate();

    }
}
