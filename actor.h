#pragma once

#include "const.h"
#include "main.h"
class Scene;
class Player;
class Egg;


class Actor
{
public:
    Actor() : Actor(Vec2())
    {
    }

    Actor(const Vec2 pos) : Actor(pos, 0, 0)
    {
    }

    Actor(const Vec2 pos, const double speed, const double angle)
        : frame_(0), pos_(pos), speed_(speed), angle_(angle), life_(1), dead_(false), collidable_(true)
    {
    }

    virtual void update() = 0;

    virtual void draw() = 0;

    virtual void postUpdate()
    {
        frame_++;
    }

    const Vec2& getPos() { return pos_; }
    const double getSpeed() { return speed_; }
    const double getAngle() { return angle_; }

    const bool isOffScreen() {
        bool b = !pos_.intersects(Rect(-SCREEN_MARGIN, -SCREEN_MARGIN, SCREEN_W + SCREEN_MARGIN, SCREEN_H + SCREEN_MARGIN));
        return b;
    }

    const bool isDead() { return dead_; }

    const int getLife() { return life_; }
    virtual void damage(const int damageVal) { life_ -= damageVal; }

    const bool isCollidable() { return collidable_; }

protected:
    int frame_;
    Vec2 pos_;
    double speed_;
    double angle_;
    int life_;
    bool dead_;
    bool collidable_;

    void updatePos()
    {
        pos_.moveBy(speed_*sin(Radians(angle_)), -speed_*cos(Radians(angle_)));
    }

};

class Bomb : public Actor
{
public:
    Bomb(const Vec2 pos, const double speed, const int lifetime) : Actor(pos, speed, 0), lifetime_(lifetime)
    {
        hitArea_ = Circle(pos, 1);
    }

    virtual void update() override;

    virtual void draw() override;

    Circle& getHitArea() { return hitArea_; }

private:
    int lifetime_;
    Circle hitArea_;

};

class Shield : public Actor
{
public:
    Shield(Player* player, Egg* egg) : Actor(), player_(player), egg_(egg), mode_(0)
    {
        hitArea_ = Rect(pos_.asPoint(), 1, 1).rotated(0);
    }

    virtual void update() override;

    virtual void draw() override;

    void changeMode() { mode_ = 1 - mode_; }

    Quad& getHitArea() { return hitArea_; }

private:
    Player* player_;
    Egg* egg_;
    int mode_; // 0 or 1
    Quad hitArea_;

};

class Player : public Actor
{
public:
    Player() : Actor(), bombPower_(0)
    {
    }

    virtual void update() override
    {
        pos_ = Mouse::Pos() / SCALE;
        pos_.x = Clamp(pos_.x, 1.0, SCREEN_W - 1.0);
        pos_.y = Clamp(pos_.y, 1.0, SCREEN_H - 1.0);
        bombPower_ = Clamp(bombPower_ + 2, 0, 1000);
    }

    virtual void draw() override;

    const int getBombPower() { return bombPower_; }
    void setBombPower(const int bombPower) { bombPower_ = bombPower; }

private:
    int bombPower_;

};

class Egg : public Actor
{
public:
    Egg(const Vec2 pos, Player* player) : Actor(pos), player_(player), inv_(0)
    {
        life_ = 100;
    }

    virtual void update() override;

    virtual void draw() override;

    virtual void damage(const int damageVal) override
    {
        Actor::damage(damageVal);
        inv_ = 80;
    }

private:
    Player* player_;
    int inv_;

};

class Enemy : public Actor
{
public:
    Enemy(const Vec2 pos, const double speed, const double angle, const string& type, Egg* egg)
        : Actor(pos, speed, angle), type_(type), egg_(egg)
    {
        if (type == "blood") {
            lifetime_ = Random(10, 40);
        }
        else if (type == "bubble") {
            lifetime_ = 25;
        }
        life_ = lifes_[type_];
        collidable_ = collidable_list_[type_];
    }

    virtual void update() override;

    virtual void draw() override;

    virtual void damage(const int damageVal) override
    {
        Actor::damage(damageVal);
        if (life_ <= 0) { dead_ = true; }
    }

    const int getScore() { return scores_[type_]; }

private:
    string type_;
    int lifetime_;
    Egg* egg_;

    const double getAimEggAngle()
    {
        Vec2 v = egg_->getPos() - pos_;
        return Degrees(atan2(v.x, -v.y));
    }

    map<string, bool> collidable_list_ = {
        { "bulletA", true },
        { "enemyA" , true },
        { "enemyB" , true },
        { "enemyC" , true },
        { "genA" ,   false },
        { "blood" ,  false },
        { "bubble" , false },
    };
    map<string, int> lifes_ = {
        { "bulletA", 1 },
        { "enemyA" , 8 },
        { "enemyB" , 8 },
        { "enemyC" , 1000 },
        { "genA" ,   1 },
        { "blood" ,  1 },
        { "bubble" , 1 },
    };
    map<string, int> scores_ = {
        { "bulletA", 100 },
        { "enemyA" , 500 },
        { "enemyB" , 500 },
        { "enemyC" , 500 },
        { "genA" ,   0 },
        { "blood" ,  0 },
        { "bubble" , 0 },
    };
    map<string, function<void()>> updateFuncs_ = {
        {
            "bulletA",
            [&]() {
            }
        },
        {
            "enemyA",
            [&]() {
                if (frame_ == 0) {
                    angle_ = 180;
                    speed_ = 2.0;
                }
                if (1 <= frame_ && frame_ < 60) {
                    speed_ -= 2.0 / 60.0;
                }
                if (60 <= frame_ && frame_ < 100) {
                    angle_ = 0;
                    speed_ = 0;
                    if (frame_ % 20 == 0) {
                        getEnemyList().push_back(unique_ptr<Enemy>(new Enemy(pos_, 2.5, getAimEggAngle(), "bulletA", egg_)));
                    }
                }
                if (100 <= frame_) {
                    speed_ = Clamp(speed_ + 3.0 / 180.0, 0.0, 3.0);
                }
            }
        },
        {
            "enemyB",
            [&]() {
                if (frame_ == 0) {
                    if (pos_.x < 160)
                        angle_ = 90 - 20;
                    else
                        angle_ = -90 + 20;
                    speed_ = 2.8;
                }
                if (1 <= frame_ && frame_ < 60) {
                }
                if (60 <= frame_ && frame_ < 240) {
                    if (frame_ % 16 == 0) {
                        getEnemyList().push_back(unique_ptr<Enemy>(new Enemy(pos_, 2.5, getAimEggAngle(), "bulletA", egg_)));
                    }
                }
                if (240 <= frame_) {
                }
            }
        },
        {
            "enemyC",
            [&]() {
                if (frame_ == 0) {
                    angle_ = 180;
                    speed_ = 2.0;
                }
                if (1 <= frame_ && frame_ < 120) {
                    speed_ -= 2.0 / 120.0;
                }
                if (60 <= frame_ && frame_ < 240) {
                    if (frame_ % 20 == 0) {
                        for (int i = 0; i < 16; i++)
                            getEnemyList().push_back(unique_ptr<Enemy>(new Enemy(pos_, 3.4, getAimEggAngle()+360.0/16.0*i, "bulletA", egg_)));
                    }
                }
                if (240 <= frame_ && frame_ < 300) {
                    angle_ = 270;
                    speed_ += 2.0 / 60.0;
                }
                if (300 <= frame_ && frame_ < 360) {
                    speed_ -= 2.0 / 60.0;
                    if (frame_ % 10 == 0) {
                        for (int i = 0; i < 8; i++)
                            getEnemyList().push_back(unique_ptr<Enemy>(new Enemy(pos_, 3.4, getAimEggAngle() + 360.0 / 8.0*i, "bulletA", egg_)));
                    }
                }
                if (360 <= frame_ && frame_ < 480) {
                    angle_ = 90;
                    speed_ += 2.0 / 120.0;
                    if (frame_ % 12 == 0) {
                        for (int i = 0; i < 8; i++)
                            getEnemyList().push_back(unique_ptr<Enemy>(new Enemy(pos_, 3.4, getAimEggAngle() + 360.0 / 8.0*i, "bulletA", egg_)));
                    }
                }
                if (480 <= frame_) {
                    angle_ = 0;
                    speed_ += 3.0 / 100.0;
                }
            }
        },
        {
            "genA",
            [&]() {
                if (20 <= frame_ && frame_ < 20+60*8) {
                    if (frame_ % 6 == 0) {
                        getEnemyList().push_back(unique_ptr<Enemy>(new Enemy(
                            Vec2(Random(40.0, 320.0-40.0), -31),
                            Random(2.0, 3.5),
                            Random(180.0-40.0, 180.0+40.0),
                            "bulletA", egg_)));
                    }
                }
                if (frame_ > 20 + 60 * 8) {
                    dead_ = true;
                }
            }
        },
        {
            "blood",
            [&]() {
               if (frame_ > lifetime_) { dead_ = true; }
            }
        },
        {
            "bubble",
            [&]() {
                if (frame_ > lifetime_) { dead_ = true; }
            }
        },
    };

    map<string, function<void()>> drawFuncs_ = {
        {
            "bulletA",
            [&]() {
                const uint32 f = (frame_ / 2) % 2;
                const Color color1{ 255, 250 - 250 * f, 250 - 230 * f };
                const Color color2{ 255, 80 - 20 * f, 80 - 40 * f };
                const double r = 6;
                Circle(pos_, r).draw(color1);
                Circle(pos_, r).drawFrame(0, 1, color2);
            }
        },
        {
            "enemyA",
            [&]() {
                const uint32 f = (frame_ / 2) % 2;
                const Color color1{ 220 - 220 * f, 230 - 230 * f, 255 };
                //const double r = 15;
                //Circle(pos_, r).draw(color1);
                Rect((pos_ - Vec2(12.0, 12.0)).asPoint(), 24.0, 24.0).rotated(frame_ / 12.0).draw(color1);
            }
        },
        {
            "enemyB",
            [&]() {
                const uint32 f = (frame_ / 2) % 2;
                const Color color1{ 230 - 220 * f, 230 - 200 * f, 255 };
                //const double r = 15;
                //Circle(pos_, r).draw(color1);
                Rect((pos_ - Vec2(12.0, 12.0)).asPoint(), 24.0, 24.0).rotated(frame_ / 12.0).draw(color1);
            }
        },
        {
            "enemyC",
            [&]() {
                const uint32 f = (frame_ / 2) % 2;
                const Color color1{ 255 - 100 * f, 255 - 100 * f, 0 };
                //const double r = 15;
                //Circle(pos_, r).draw(color1);
                Rect((pos_ - Vec2(20.0, 20.0)).asPoint(), 40.0, 40.0).rotated(frame_ / 16.0).draw(color1);
            }
        },
        {
            "genA",
            [&]() {
            }
        },
        {
            "blood",
            [&]() {
                Circle(pos_, (lifetime_ - frame_ + 1) / 4.0).draw(Color(225 * ((frame_ / 2) % 2), 64, 32));
            }
        },
        {
            "bubble",
            [&]() {
                const uint32 f = (frame_ / 2) % 2;
                Circle(pos_, 8 + frame_ / 4.0).drawFrame(3.0*(1.0 - frame_ / 25.0), 1.0, Color(128+127 * f, 128-96*f, 128-96*f));
            }
        },
    };
};

