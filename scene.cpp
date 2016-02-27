#include "scene.h"
#include "actor.h"
#include "main.h"
#include "stage.h"


Scene::Scene(list<unique_ptr<Scene>>* scenes) : next_(SceneType::None), scenes_(scenes), frame_(0)
{
    
}

void Scene::update()
{
}

void Scene::draw()
{
}

void Scene::postUpdate()
{
    frame_++;
}

SceneTitle::SceneTitle(list<unique_ptr<Scene>>* scenes) : Scene(scenes)
{
    getEnemyList().clear();
    getBombList().clear();
}

void SceneTitle::update()
{
    if (Input::MouseL.clicked) {
        scenes_->push_back(unique_ptr<Scene>(new SceneMain(scenes_)));
        scenes_->pop_front();
    }
    else if (Input::MouseR.clicked) {
        scenes_->pop_back();
    }
}

void SceneTitle::draw()
{
    int f = (System::FrameCount() / 2) % 2;
    Rect(SCREEN_W, SCREEN_H).draw(Palette::Blue);
    FontAsset(L"fontL").drawCenter(L"D E F E G G", Vec2(SCREEN_W / 2.0, SCREEN_H / 2.0), Palette::White);
    FontAsset(L"font").draw(L"@voidproc", Vec2(SCREEN_W / 2.0 + 50, SCREEN_H / 2.0+20), Palette::White);
    FontAsset(L"font").drawCenter(L"START TO L-CLICK, QUIT TO R-CLICK", Vec2(SCREEN_W / 2.0, 220), Palette::White);
}

SceneMain::SceneMain(list<unique_ptr<Scene>>* scenes) : Scene(scenes), player_(), egg_(), shield_(), stage_(), score_(0)
{
    player_.reset(new Player());
    egg_.reset(new Egg(Vec2(SCREEN_W / 2, SCREEN_H / 2), player_.get()));
    shield_.reset(new Shield(player_.get(), egg_.get()));
    stage_.reset(new Stage());
}

void SceneMain::update()
{
    // mouse_R_click => bomb
    if (Input::MouseR.clicked) {
        if (player_->getBombPower() > 500) {
            getBombList().push_back(unique_ptr<Bomb>(new Bomb(player_->getPos(), 0.3, 60)));
            player_->setBombPower(player_->getBombPower() - 500);
        }
    }

    // mouse_L_click => shield mode change
    if (Input::MouseL.clicked) {
        shield_->changeMode();
    }

    // gen enemy
    if (frame_ == stage_->getNextEventFrame()) {
        StageEvent ev = stage_->getNextEvent();
        getEnemyList().push_back(unique_ptr<Enemy>(new Enemy(ev.pos, 0, 0, ev.name, egg_.get())));
    }

    //if (System::FrameCount() % 10 == 0) {
    //    getEnemyList().push_back(unique_ptr<Enemy>(new Enemy(
    //        Vec2(
    //            Random(SCREEN_W / 2 - 120, SCREEN_W / 2 + 120),
    //            -SCREEN_MARGIN + 1),
    //        Random(1.5, 3.0),
    //        Random(180 - 60, 180 + 60),
    //        "enemyA")));
    //}

    // update
    player_->update();
    egg_->update();
    shield_->update();
    for (auto& e : getEnemyList()) { e->update(); }
    for (auto& b : getBombList()) { b->update(); }

    // collision check
    if (egg_->isCollidable()) {

        for (auto& e : getEnemyList()) {
            if (e->isCollidable()) {
                if (Circle(egg_->getPos(), 20).intersects(Circle(e->getPos(), 2.0))) {
                    egg_->damage(10);
                    // blood effect
                    for (int i : step(Random(5, 10))) {
                        getEnemyList().push_back(unique_ptr<Enemy>(new Enemy(e->getPos(), Random(1.5, 4.0), Random(360), "blood", egg_.get())));
                    }
                    // gameover
                    if (egg_->getLife() <= 0) {
                        next_ = SceneType::GameOver;
                    }
                }
            }
        }
    }

    // shield
    for (auto& e : getEnemyList()) {
        if (e->isCollidable()) {
            if (shield_->getHitArea().intersects(Circle(e->getPos(), 2.0))) {
                e->damage(1);
                if (e->isDead()) {
                    getEnemyList().push_back(unique_ptr<Enemy>(new Enemy(e->getPos(), e->getSpeed() / 2.0, e->getAngle(), "bubble", egg_.get())));
                    score_ += e->getScore();
                }
            }
            for (auto& b : getBombList()) {
                if (b->getHitArea().intersects(Circle(e->getPos(), 2.0))) {
                    e->damage(4);
                    if (e->isDead()) {
                        getEnemyList().push_back(unique_ptr<Enemy>(new Enemy(e->getPos(), e->getSpeed() / 2.0, e->getAngle(), "bubble", egg_.get())));
                        score_ += e->getScore();
                    }
                }
            }
        }
    }

    // scene end?
    if (stage_->isEnd()) {
        if (getEnemyList().empty()) {
            next_ = SceneType::Clear;
        }
    }

    // scene change

    if (next_ == SceneType::GameOver) {
        scenes_->push_back(unique_ptr<Scene>(new SceneGameOver(scenes_)));
        next_ = SceneType::None;
    }
    else if (next_ == SceneType::Clear) {
        scenes_->push_back(unique_ptr<Scene>(new SceneClear(scenes_)));
        next_ = SceneType::None;
    }
}

void SceneMain::draw()
{
    // debug
    //ClearPrint();
    //Println(frame_);
    //Println(getEnemyList().size());

    // actors
    Rect(SCREEN_W, SCREEN_H).draw(Palette::Black);
    player_->draw();
    egg_->draw();
    shield_->draw();
    for (auto& b : getBombList()) { b->draw(); }
    for (auto& e : getEnemyList()) { e->draw(); }

    // egg life
    TextureAsset(L"a")(0, 0, 104, 16).draw(2, 2);
    TextureAsset(L"a")(2, 18, 1, 12).scale(Clamp(egg_->getLife()*1.0, 0.0, 100.0), 1.0).draw(2 + 2, 2 + 2, Palette::Yellow);
    
    // bomb power
    TextureAsset(L"a")(0, 0, 104, 16).draw(2+104+2, 2);
    TextureAsset(L"a")(2, 18, 1, 12).scale(Clamp(player_->getBombPower()*1.0, 0.0, 1000.0)/10.0, 1.0).draw(2 + 104 + 2 + 2, 2 + 2, Color(0, 255, 0));

    // score
    FontAsset(L"font").draw(Pad(score_, { 8, L'0' }), 2+104+2+2+104+2+2, 5, Palette::White);
}

void SceneMain::postUpdate()
{
    Scene::postUpdate();

    player_->postUpdate();
    egg_->postUpdate();
    shield_->postUpdate();
    for (auto& e : getEnemyList()) { e->postUpdate(); }
    for (auto& b : getBombList()) { b->postUpdate(); }

    Erase_if(getEnemyList(), [](auto& e) { return e->isDead() || e->isOffScreen(); });
    Erase_if(getBombList(), [](auto& e) { return e->isDead(); });
}

SceneGameOver::SceneGameOver(list<unique_ptr<Scene>>* scenes) : Scene(scenes)
{
}

void SceneGameOver::update()
{
    if (frame_ > 60) {
        if (Input::MouseL.clicked) {
            next_ = SceneType::Title;
        }
    }

    if (next_ == SceneType::Title) {
        next_ = SceneType::None;
        scenes_->push_back(unique_ptr<Scene>(new SceneTitle(scenes_)));
        scenes_->pop_front();
        scenes_->pop_front();
    }
}

void SceneGameOver::draw()
{
    Rect(SCREEN_W, SCREEN_H).draw(Color(0, 0, 0, 180));

    FontAsset(L"fontL").drawCenter(L"G A M E  O V E R", Vec2(SCREEN_W/2.0, SCREEN_H/2.0), Palette::White);
}

SceneClear::SceneClear(list<unique_ptr<Scene>>* scenes) : Scene(scenes)
{
}

void SceneClear::update()
{
    if (frame_ > 60) {
        if (Input::MouseL.clicked) {
            next_ = SceneType::Title;
        }
    }

    if (next_ == SceneType::Title) {
        next_ = SceneType::None;
        scenes_->push_back(unique_ptr<Scene>(new SceneTitle(scenes_)));
        scenes_->pop_front();
        scenes_->pop_front();
    }
}

void SceneClear::draw()
{
    Rect(SCREEN_W, SCREEN_H).draw(Color(0, 0, 0, 180));

    FontAsset(L"fontM").drawCenter(L"THANK YOU FOR PLAYING !!", Vec2(SCREEN_W / 2.0, SCREEN_H / 2.0), RandomColor());
}

