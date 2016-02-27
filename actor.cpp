#include "actor.h"


void Bomb::update()
{
    if (frame_ > lifetime_) { dead_ = true; }
    updatePos();

    double e = EaseInOut<Easing::Cubic>(frame_ / 60.0);

    hitArea_ = Circle(pos_, 64 + 120*e);
}

void Bomb::draw()
{
    int f = (frame_ / 2) % 2;
    getHitArea().draw(Color(25, 255 - f * 100, 32, 168 * (1.1 - frame_ / 60.0)));
    getHitArea().drawFrame(1.0-frame_/60.0, 1.0, Color(32 + f * 100, 255 - f * 240, 32 + f * 100, 200 * (1.1 - frame_ / 60.0)));
}

void Shield::update()
{
    double shield_w = 80;
    double shield_h = 20;
    if (mode_ >= 1) { swap(shield_w, shield_h); }

    pos_ = player_->getPos();
    Vec2 v = player_->getPos() - egg_->getPos();
    angle_ = atan2(v.x, -v.y);
    hitArea_ = Rect((pos_-Vec2(shield_w/2, shield_h/2)).asPoint(), shield_w, shield_h).rotated(angle_);
}

void Shield::draw()
{
    // green rect
    int f = (frame_ / 1) % 2;
    getHitArea().draw(Color(0, (200 + 55 * (1 + sin(frame_ / 6.0)) / 2.0) * f, 0, 192));
    getHitArea().drawFrame(1.0, Color(32, 80 + 170 * (1 + sin(frame_ / 6.0)) / 2.0 * f, 32, 100*f));
}

void Player::draw()
{
    // green cross
    const Color color{ 32, (uint32)(64 + 128 * (1.0 + sin(frame_ / 8.0)) / 2.0), 32 };
    const double len = 10;
    Line(pos_.x - len, pos_.y, pos_.x + len, pos_.y).draw(1.0, color);
    Line(pos_.x, pos_.y - len, pos_.x, pos_.y + len).draw(1.0, color);
}

void Egg::update()
{
    // follow player
    Vec2 dir = player_->getPos() - pos_;
    double length = dir.length();
    if (length > 10.0) pos_ += dir.normalized() * min(length/90.0, 0.2);

    // invincible
    if (inv_ > 0) {
        inv_--;
        collidable_ = false;
    }
    else {
        collidable_ = true;
    }
}

void Egg::draw()
{
    // egg image (future)
    const uint32 f = (frame_/2)%2;
    Color color{ 255, 255, 255 };

    if (!collidable_) { color.a = 100 + 155 * ((frame_ / 2) % 2); }

    //const double r = 24;
    //Circle(pos_, r).draw(color);
    TextureAsset(L"a")(0, 32, 50, 50).drawAt(pos_.x, pos_.y+2.0*((frame_/16)%2), color);
}

void Enemy::update()
{
    // move by speed,angle
    updatePos();

    updateFuncs_[type_]();
}

void Enemy::draw()
{
    drawFuncs_[type_]();
}
