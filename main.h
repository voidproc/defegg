#pragma once
class Enemy;
class Bomb;

extern list<unique_ptr<Enemy>>& getEnemyList();
extern list<unique_ptr<Bomb>>& getBombList();
