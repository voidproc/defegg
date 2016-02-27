#pragma once

enum class SceneType;
class Player;
class Egg;
class Shield;
class Enemy;
class Stage;


class Scene
{
public:
    Scene(list<unique_ptr<Scene>>* scenes);
    virtual void update();
    virtual void draw();
    virtual void postUpdate();


protected:
    SceneType next_;
    list<unique_ptr<Scene>>* scenes_;
    int frame_;

};

class SceneTitle : Scene
{
public:
    SceneTitle(list<unique_ptr<Scene>>* scenes);
    virtual void update() override;
    virtual void draw() override;
};

class SceneMain : Scene
{
public:
    SceneMain(list<unique_ptr<Scene>>* scenes);
    virtual void update() override;
    virtual void draw() override;
    virtual void postUpdate() override;

private:
    unique_ptr<Player> player_;
    unique_ptr<Egg> egg_;
    unique_ptr<Shield> shield_;
    unique_ptr<Stage> stage_;
    int score_;
};

class SceneGameOver : Scene
{
public:
    SceneGameOver(list<unique_ptr<Scene>>* scenes);
    virtual void update() override;
    virtual void draw() override;

};

class SceneClear : Scene
{
public:
    SceneClear(list<unique_ptr<Scene>>* scenes);
    virtual void update() override;
    virtual void draw() override;

};


