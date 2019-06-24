#ifndef SBHSPRITES
#define SBHSPRITES

#include <vector>
#include <GL\glut.h>
#include "OpenGLImage.h"
#include "vec2f.h"

#define R 0
#define G 1
#define B 2
#define A 3

typedef struct {
	double l, r, b, t;
} Extent;


void test();
void init();

class SBHSprite
{
public:
	Vec2f pos;
	Vec2f v;
	Vec2f acc;
	float friction;

	Vec2f mousePos;
	float angle;
	float size;
	float color[4];
	GLuint textureID_;
	
	int health;
	bool isDead;
	bool toBeDisposed;
	int scorePoint;

	int hurterATK;

	int numOfBombs;
	int lvl;

	virtual void collision(SBHSprite * sufferer);
	void dropSupply(Vec2f pos_,int type,int probability);

	void displayTexture(GLuint ID, float transparency);
	virtual void display();
	virtual void update();
};

class Enemy :public SBHSprite
{
public:
	Vec2f movePos;
	bool reachedPos;
	float vlimit;
	float alimit;

	int cdOuterTotal;
	int cdOuterRest;
	int innerCount;
	int innerAmount;
	int cdInnerTotal;
	int cdInnerRest;
	SBHSprite* target;

	Enemy();
	Enemy(Vec2f pos_, Vec2f movePos_,GLuint ID);
	void display();
	void update();
};

class SmartEnemy :public Enemy
{
public:
	float bulletSpeed;

	float safeDistance;

	SmartEnemy(Vec2f pos_, Vec2f movePos_, GLuint ID);
	void update();
	void dodgeBullet();
};

class Hero :public SBHSprite
{
public:
	float vlimit;

	Hero();
	Hero(Vec2f pos_, Vec2f v_, float size_, float color_[4]);
	void display();
	void update();
};

class Ally :public SBHSprite
{
public:

};

class Supply :public SBHSprite
{
public:
	Vec2f center;
	float orbitAngle;
	float radius;
	int lifeCycle;

	void display();
	void update();
	//void collision(SBHSprite * sufferer);
};

class AidBox :public Supply
{
public:
	AidBox(Vec2f center_);
	void collision(SBHSprite* sufferer);
};

class WeaponBox :public Supply
{
public:
	WeaponBox(Vec2f center_);
	void collision(SBHSprite* sufferer);
};

class BombBox :public Supply
{
public:
	BombBox(Vec2f center_);
	void collision(SBHSprite* sufferer);
};

class Bullet :public SBHSprite
{
public:
	Bullet();
	Bullet(Vec2f pos_,float angle_,float speed);
	void display();
	void update();
};

class Ene1Bullet :public Bullet
{
public:
	Ene1Bullet(Vec2f pos_, float angle_, float speed);

	void display();
//	void collision(SBHSprite* sufferer);
};

class Ene2Bullet :public Bullet
{
public:
	Ene2Bullet(Vec2f pos_, float angle_, float speed);
	void display();
};

class Bomb :public Bullet
{
public:
	Bomb(Vec2f pos_);
	void display();
	void update();
	void collision(SBHSprite* sufferer);
};

class Rock :public SBHSprite
{
public:
	float spinning;

	Rock(Vec2f pos_, Vec2f dir, float speed, float size_, float spinning_);
	void display();
	void update();
};





//link structure

class SBHNode
{
public:
	SBHSprite* sprite;
	SBHNode* nextNode;

	SBHNode(SBHSprite* sprite_);
};

class SpritesLink
{
public:
	SBHNode* head;
	SBHNode* tail;
	int linkLength;

	SpritesLink();
	SpritesLink(SBHSprite* sprite);
	//~SpritesLink();
	void appendNode(SBHSprite* sprite);

	virtual void display();
	virtual void update();

	void suffererCollision(SpritesLink* hurter);//call me
	void hurterCollision(SBHSprite* sprite);//dont call me
};

class FriendLink :public SpritesLink
{
public:
	void update();
};

class FriendFireLink :public SpritesLink
{
public:
	bool mousePressed;
	bool mouseRightPressed;
	bool mouseRightReleased;
	int cdTotal;
	int cdRest;

	FriendFireLink();

	void update(Vec2f pos_,float angle);
};

class EneFireLink :public SpritesLink
{
public:
	Bomb* bomb;

	EneFireLink();
	void update();
	void suffererCollision(SBHSprite* bomb);
};

class RockLink :public SpritesLink
{
public:
	int cdTotalRock;
	int cdRestRock;
	int rockAmount;

	RockLink();

	void update();
};

class EneLink :public SpritesLink
{
public:
	int cdTotal;
	int cdRest;
	int amount;

	int cdSEneTotal;
	int cdSEneRest;
	
	EneLink();
	void update();
};




//other graphics

class BackGround
{
public:
	double y;
	double speed;
	GLuint textureID_;

	BackGround(GLuint ID);
	void display();
};

class Panel :public SBHSprite
{
public:
	char title[16];
	int* pvalue;
	int max;
	float scaleFactor;

	Panel();
	Panel(char* title_, int max_, Vec2f pos_);
	Panel(char* title_, int* pvalue_, int max_, Vec2f pos_);
	void display();
};

class ScorePanel :public Panel
{
public:
	ScorePanel(char* title_);
	void display();
};

class BombNumberPanel :public Panel
{
public:
	BombNumberPanel(int* pvalue_);
	void display();
	void displayCircle(bool oo);
};


class GameProgram
{
public:
	int timer;
	int state;

	GameProgram();
	void update();
};

#endif
