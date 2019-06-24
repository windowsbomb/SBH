#include "stdafx.h"
#include "sprites.h"

extern Extent world;
extern BackGround* backGround;
extern FriendLink* friendLink;
extern RockLink* rockLink;
extern EneLink* eneLink;
extern FriendFireLink* friendFireLink;
extern EneFireLink* eneFireLink;
extern SpritesLink* panelLink;
extern int* scOrE;
extern SpritesLink* supplyLink;
extern GameProgram* prog;
extern const float dt;

#define SMALL_DISTANCE 0.08f
#define MAXHEROLIFE 500
#define MAXENELIFE 200

#define PATH_HERO "data//hero.png"
#define ID_HERO 0
#define PATH_ROCK "data//rock.png"
#define ID_ROCK 1
#define PATH_ENE1 "data//ene1.png"
#define ID_ENE1 2
#define PATH_ENE2 "data//ene2.png"
#define ID_ENE2 3
#define PATH_FRIEND "data//fri.png"
#define ID_FRI 4
#define PATH_BG "data//bg.png"
#define ID_BG 5
#define PATH_SUPPLY_HEALTH "data//health.png"
#define ID_HEALTH 6
#define PATH_WEAPON "data//weapon.png"
#define ID_WEAPON 7
#define PATH_BOMB "data//bomb.png"
#define ID_BOMB 8
#define PATH_MISSILE "data//missi.png"
#define ID_BOMB 9



Enemy::Enemy()
{
	pos[X] = .55;
	pos[Y] = .15;
	v[X] = 0.;
	v[Y] = 0.;
	acc[X] = 0.;
	acc[Y] = 0.;
	friction = -0.0001;

	size = .2;
	color[R] = 0.;
	color[G] = .5;
	color[B] = .5;
	color[A] = 0.2;
	angle = 30.f;
	textureID_ = 1;

	toBeDisposed = false;
}

Enemy::Enemy(Vec2f pos_,  Vec2f movePos_,GLuint ID)
{
	Vec2fCopy(pos, pos_);
	Vec2fCopy(movePos, movePos_);
	reachedPos = false;
	v[X] = 0.f;
	v[Y] = 0.f;
	vlimit = 0.01f;
	acc[X] = 0.f;
	acc[Y] = 0.f;
	alimit = 0.004f;
	friction = -0.001f;

	size = 0.1f;
	angle = 0.f;
	textureID_ = ID;

	cdOuterTotal = 50;
	cdOuterRest = cdOuterTotal;
	innerAmount = 2;
	innerCount = 0;
	cdInnerTotal = 10;
	cdInnerRest = cdInnerTotal;
	target = friendLink->head->sprite;

	health = MAXENELIFE;
	toBeDisposed = false; 
	scorePoint = 50;

	hurterATK = MAXHEROLIFE + 1;
}

void Enemy::display()
{
	SBHSprite::display();
	SBHSprite::displayTexture(textureID_, 0.1f);
}

void Enemy::update()
{
	if (health < 0)
	{
		*scOrE += scorePoint;
		dropSupply(pos, rand() % 2, 4);
		toBeDisposed = true;
		return;
	}

	//acc
	if (!reachedPos)
	{
		if (pos[X] + SMALL_DISTANCE < movePos[X] || pos[X] - SMALL_DISTANCE > movePos[X] ||
			pos[Y] + SMALL_DISTANCE < movePos[Y] || pos[Y] - SMALL_DISTANCE > movePos[Y])
		{
			Vec2fSub(acc, movePos, pos);
			Vec2fUnit(acc);
			acc[X] *= alimit;
			acc[Y] *= alimit;
		}
		else
		{
			acc[X] = 0.f;
			acc[Y] = 0.f;
			//v[X] /= 2.f;
			//v[Y] /= 2.f;
			reachedPos = true;
		}
	}

	SBHSprite::update();
	//velocity and pos limit
	Vec2fClamp(v, -vlimit, vlimit);
	pos[X] += v[X];
	pos[Y] += v[Y];
	//Vec2fClamp(pos, -0.9f, 0.9f);

	Vec2f dir;
	Vec2fSub(dir, target->pos, pos);

	if (Vec2fMag(dir) > SMALL_NUM)
	{
		Vec2fUnit(dir);
		if (dir[Y] > 0.f)
			angle = acos(dir[X]) / PI*180.f + 90.f;
		else
			angle = -acos(dir[X]) / PI*180.f + 90.f;
	}


	if (reachedPos)
	{
		//clear
		if (pos[X] + size + size < world.l || pos[X] - size - size > world.r ||
			pos[Y] + size + size < world.b || pos[Y] - size - size > world.t)
		{
			toBeDisposed = true;
		}
		else
		{
			//open fire
			if (cdOuterRest > 0)
				cdOuterRest--;
			if (cdOuterRest == 0)
			{
				if (cdInnerRest > 0)
					cdInnerRest--;
				if (cdInnerRest == 0)
				{
					Ene1Bullet* bullet = new Ene1Bullet(pos, angle - 90.f, 0.01f);
					eneFireLink->appendNode(bullet);
					cdInnerRest = cdInnerTotal;
					innerCount++;
				}
				if (innerCount > innerAmount)
				{
					cdOuterRest = cdOuterTotal;
					innerCount = 0;
				}
			}
		}
	}
}

Hero::Hero()
{
	pos[X] = 0.;
	pos[Y] = 0.;
	v[X] = 0.;
	v[Y] = 0.;
	vlimit = 0.02f;
	acc[X] = 0.;
	acc[Y] = 0.;
	friction = -0.001;

	size = 0.08;
	color[R] = .5;
	color[G] = .5;
	color[B] = 1.;
	color[A] = .8;
	angle = 0.2;
	textureID_ = 0;

	health = MAXHEROLIFE;
	isDead = false;
	toBeDisposed = false;

	numOfBombs = 2;
	lvl = 0;
}

Hero::Hero(Vec2f pos_, Vec2f v_, float size_, float color_[4])
{
	Vec2fCopy(pos, pos_);
	Vec2fCopy(v, v_);
	size = size_;
	color[R] = color_[R];
	color[G] = color_[G];
	color[B] = color_[B];
	color[A] = color_[A];
	toBeDisposed = false;
}

void Hero::display()
{
	SBHSprite::display();
	SBHSprite::displayTexture(ID_HERO, 0.5f);
}

void Hero::update()
{
	SBHSprite::update();
	//velocity and pos limit
	Vec2fClamp(v, -vlimit, vlimit);
	//pos[X] += v[X] * dt;
	pos[X] += v[X];
	pos[Y] += v[Y];
	Vec2fClamp(pos, -0.9f, 0.9f);


	//direction
	Vec2f dir;
	Vec2fSub(dir, mousePos, pos);
	if (Vec2fMag(dir) > SMALL_NUM)
	{
		Vec2fUnit(dir);
		if (dir[Y] > 0.f)
			angle = acos(dir[X]) / PI*180.f + 90.f;
		else
			angle = -acos(dir[X]) / PI*180.f + 90.f;
	}


	//health
	if (health <= 0)
		isDead = true;
}


void SBHSprite::collision(SBHSprite * sufferer)
{
	if (pos[X] + size<sufferer->pos[X] - sufferer->size ||
		pos[X] - size>sufferer->pos[X] + sufferer->size ||
		pos[Y] + size<sufferer->pos[Y] - sufferer->size ||
		pos[Y] - size>sufferer->pos[Y] + sufferer->size
		)
	{
		return;
	}
	else
	{
		Vec2f distance;
		Vec2fSub(distance, pos, sufferer->pos);
		if (Vec2fMag(distance) < sufferer->size + size)
		{
			sufferer->health -= hurterATK;
			//if (sufferer->health < 0)
			//	sufferer->toBeDisposed = true;
			toBeDisposed = true;
		}
	}

}

void SBHSprite::dropSupply(Vec2f pos_, int type, int probability)
{
	if ((rand() % probability) == 0)
	{
		Supply* supply = new Supply();
		switch (type)
		{
		case 0:
			supply = new AidBox(pos_);
			break;
		case 1:
			supply = new WeaponBox(pos_);
			break;
		case 2:
			supply = new BombBox(pos_);
			break;
		}
		supplyLink->appendNode(supply);
	}
}

void SBHSprite::displayTexture(GLuint ID, float transparency)
{
	//glColor3d(0, 0, 0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, ID);
	//glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, transparency);
	glBegin(GL_QUADS);
	{
		glTexCoord2d(0, 0);
		glVertex2d(-size, -size);
		glTexCoord2d(1, 0);
		glVertex2d(+size, -size);
		glTexCoord2d(1, 1);
		glVertex2d(+size, +size);
		glTexCoord2d(0, 1);
		glVertex2d(-size, +size);
	}
	glEnd();
	glPopMatrix();
	//angle += 1.;

	glDisable(GL_ALPHA_TEST);
	//glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);

}

void SBHSprite::display()
{
	glClearColor(0, 0, 0, 1);
	glLoadIdentity();
	glPushMatrix();
	glTranslatef(pos[X], pos[Y], 0.f);
	glRotatef(angle, 0.f, 0.f, 1.f);
	//glTranslatef(-pos[X], -pos[Y], 0.f);
}

void SBHSprite::update()
{
	//acc
	Vec2f acc_;
	Vec2fCopy(acc_, acc);
	if (friction != 0.f)
	{	
		Vec2f frict;
		Vec2fCopy(frict, v);
		//Vec2fPrint(frict);
		if (Vec2fMag(frict) > SMALL_NUM)
		{
			Vec2fUnit(frict);
			frict[X] = friction*frict[X];
			frict[Y] = friction*frict[Y];
			acc_[X] += frict[X];
			acc_[Y] += frict[Y];
		}
	}
	v[X] += acc_[X];
	v[Y] += acc_[Y];
}

Bullet::Bullet()
{
}

Bullet::Bullet(Vec2f pos_, float angle_, float speed)
{
	Vec2fCopy(pos, pos_);
	if (angle_ > 0.f)
	{
		v[X] = speed*cos(angle_*PI / 180.f);
		v[Y] = speed*sin(angle_*PI / 180.f);
	}
	else
	{
		v[X] = speed*cos(angle_*PI / 180.f);
		v[Y] = speed*sin(angle_*PI / 180.f);
	}
	//printf("angle %f\n", angle_);
	//Vec2fPrint(v);

	acc[X] = 0.f;
	acc[Y] = 0.f;
	friction = 0.f;

	angle = angle_ + 90.f;
	size = 0.008;

	toBeDisposed = false;

	hurterATK = 20;
}

void Bullet::display()
{
	SBHSprite::display();

	glColor3d(0.8f, 0.8f, 0.0f);
	glBegin(GL_POLYGON);
	glVertex2d(- 0.004, 0.f);
	glVertex2d(+ 0.004, 0.f);
	glVertex2d(+ 0.004, 0.02);
	glVertex2d(- 0.004, 0.02);
	glEnd();
	glPopMatrix();
}

void Bullet::update()
{
	pos[X] += v[X];
	pos[Y] += v[Y];
	//Vec2fClamp(pos, -0.9f, 0.9f);
	if (pos[X]<world.l || pos[X]>world.r || pos[Y]<world.b || pos[Y]>world.t)
	{
		toBeDisposed = true;
	}
}


SBHNode::SBHNode(SBHSprite * sprite_)
{
	sprite = sprite_;
	nextNode = NULL;
}

SpritesLink::SpritesLink()
{
	head = NULL;
	tail = NULL;
	linkLength = 0;
}

SpritesLink::SpritesLink(SBHSprite * sprite)
{
	head = new SBHNode(sprite);
	tail = head;
	linkLength = 1;
}

void SpritesLink::appendNode(SBHSprite * sprite)
{
	if (head == NULL)
	{
		head = new SBHNode(sprite);
		tail = head;
		linkLength = 1;
	}
	else
	{
		SBHNode* node = head;
		SBHNode* next = new SBHNode(sprite);
		tail->nextNode = next;
		tail = next;
		linkLength++;
	}
}

void SpritesLink::display()
{
	SBHNode* node = head;
	while (node != NULL)
	{
		node->sprite->display();
		node = node->nextNode;
	}
}

void SpritesLink::update()
{
	SBHNode* node = head;
	SBHNode* prevNode = NULL;
	while (node != NULL)
	{
		//update
		node->sprite->update();
		if (node->sprite->toBeDisposed)
		{
			if (node->nextNode == NULL)
			{
				head = NULL;
				tail = NULL;
				delete node->sprite;
				delete node;
				node = NULL;
				linkLength--;
				break;
			}
			else
			{
				head = node->nextNode;
				delete node->sprite;
				delete node;
				node = head;
				linkLength--;
			}
		}
		else
		{
			prevNode = node;
			node = node->nextNode;
			break;
		}
	}

	while (node != NULL)
	{
		//update
		node->sprite->update();
		if (node->sprite->toBeDisposed)
		{
			prevNode->nextNode = node->nextNode;
			delete node->sprite;
			delete node;
			node = prevNode->nextNode;
			if (node == NULL)
				tail = prevNode;
			linkLength--;
		}
		else
		{
			prevNode = node;
			node = node->nextNode;
		}
	}
	printf("length %d\n", linkLength);
}

void SpritesLink::suffererCollision(SpritesLink * hurter)
{
	SBHNode* node = head;
	while (node != NULL)
	{
		hurter->hurterCollision(node->sprite);
		node = node->nextNode;
	}
}

void SpritesLink::hurterCollision(SBHSprite * sprite)
{
	SBHNode* hurterNode = head;
	while (hurterNode != NULL)
	{
		hurterNode->sprite->collision(sprite);
		hurterNode = hurterNode->nextNode;
	}
}


FriendFireLink::FriendFireLink()
{
	SpritesLink::SpritesLink();
	mousePressed = false;
	mouseRightPressed = false;
	mouseRightReleased = true;
	cdTotal = 4;
	cdRest = cdTotal;
}

void FriendFireLink::update(Vec2f pos_, float angle_)
{
	SpritesLink::update();

	if (cdRest>0)
		cdRest--;
	if (mousePressed && (cdRest == 0))
	{
		if (friendLink->head->sprite->lvl == 0)
		{
			Bullet* bullet = new Bullet(pos_, angle_, 0.05f);
			appendNode(bullet);

		}
		else if (friendLink->head->sprite->lvl > 0)
		{
			Vec2f p1, p2;
			p1[X] = cos((angle_ + 45.f)*PI / 180.f);
			p1[Y] = sin((angle_ + 45.f)*PI / 180.f);
			p2[X] = cos((angle_ - 45.f)*PI / 180.f);
			p2[Y] = sin((angle_ - 45.f)*PI / 180.f);
			Vec2fAddS(p1, 0.04f, p1, pos_);
			Vec2fAddS(p2, 0.04f, p2, pos_);

			Bullet* bullet1 = new Bullet(p1, angle_, 0.05f);
			appendNode(bullet1);
			Bullet* bullet2 = new Bullet(p2, angle_, 0.05f);
			appendNode(bullet2);
		}
		if (friendLink->head->sprite->lvl == 2)
		{
			Bullet* bullet = new Bullet(pos_, angle_, 0.05f);
			appendNode(bullet);
		}
		cdRest = cdTotal;
	}
	if (mouseRightPressed && mouseRightReleased)
	{
		//bomb
		if (friendLink->head->sprite->numOfBombs > 0)
		{
			Bomb* bomb = new Bomb(friendLink->head->sprite->pos);
			appendNode(bomb);
			friendLink->head->sprite->numOfBombs--;
			eneFireLink->bomb = bomb;
		}
		mouseRightReleased = false;
	}
}

Rock::Rock(Vec2f pos_, Vec2f dir, float speed,float size_,float spinning_)
{
	Vec2fCopy(pos, pos_);
	
	if (Vec2fMag(dir) > SMALL_NUM)
	{
		Vec2fUnit(dir);
		v[X] = dir[X] * speed;
		v[Y] = dir[Y] * speed;
		if (dir[Y] > 0.f)
			angle = acos(dir[X]) / PI*180.f;
		else
			angle = -acos(dir[X]) / PI*180.f;
	}
	else
	{
		v[X] = speed;
		v[Y] = 0.f;
		angle = 0.f;
	}

	acc[X] = 0.f;
	acc[Y] = 0.f;
	friction = 0.f;

	size = size_;
	spinning = spinning_;

	health = (int)((size_ - 0.05f) / 0.025f) * 100 - 50;
	toBeDisposed = false;
	scorePoint = (int)((size_ - 0.05f) / 0.025f) * 20 - 10;

	hurterATK = MAXHEROLIFE + 1;
}

void Rock::display()
{
	SBHSprite::display();
	SBHSprite::displayTexture(ID_ROCK, 0.1f);
}

void Rock::update()
{
	if (health < 0)
	{
		*scOrE += scorePoint;
		if (size > 0.1f)
			dropSupply(pos, rand() % 2, 2);
		toBeDisposed = true;
		return;
	}
	pos[X] += v[X];
	pos[Y] += v[Y];
	angle += spinning;

	if (pos[X]<(world.l * 1.5f) || pos[X]>(world.r * 1.5f) ||
		pos[Y]<(world.b * 1.5f) || pos[Y]>(world.t * 1.5f))
	{
		toBeDisposed = true;
	}
}

RockLink::RockLink()
{
	SpritesLink::SpritesLink();
	cdTotalRock = 200;
	cdRestRock = cdTotalRock;
	rockAmount = 3;
}

void RockLink::update()
{
	SpritesLink::update();

	if (cdRestRock > 0)
	{
		cdRestRock--;
	}
	if (cdRestRock == 0 && linkLength < rockAmount && eneLink->linkLength < 4)
	{
		//single rock
		Vec2f pos_, target, dir;
		if (rand() % 2)
		{
			pos_[X] = (rand() % 2 - 0.5f)*2.2f;
			pos_[Y] = ((rand() % 600) - 300) / 300.f;

		}
		else
		{
			pos_[X] = ((rand() % 600) - 300) / 300.f;
			pos_[Y] = (rand() % 2 - 0.5f)*2.2f;
		}
		//Vec2fClamp(pos_, -1.f, 1.f);
		target[X] = ((rand() % 600) - 300) / 350.f;
		target[Y] = ((rand() % 600) - 300) / 350.f;
		Vec2fSub(dir, target, pos_);
		float size_ = (rand() % 4 + 1) / 40.f + 0.05f;//
		//float speed = 0.16 - size_;
		float speed = (rand() % 10 + 1) / 800.f;//
		float spinning= (rand() % 10 -5) / 1.f;//
		Rock* rock = new Rock(pos_, dir, speed, size_, spinning);
		appendNode(rock);
		cdRestRock = cdTotalRock;
		//printf(" cd+ %d\n\n",cdRestRock);
	}
	suffererCollision(friendFireLink);
}

void loadTexture()
{
	OpenGLImage *imageHero = new OpenGLImage(PATH_HERO, ID_HERO, GL_CLAMP);
	OpenGLImage *imageEne1 = new OpenGLImage(PATH_ENE1, ID_ENE1, GL_CLAMP);
	OpenGLImage *imageEne2 = new OpenGLImage(PATH_ENE2, ID_ENE2, GL_CLAMP);
	OpenGLImage* imageRock = new OpenGLImage(PATH_ROCK, ID_ROCK, GL_CLAMP);
	OpenGLImage* imageBg = new OpenGLImage(PATH_BG, ID_BG, GL_REPEAT);
	OpenGLImage* imageHealth = new OpenGLImage(PATH_SUPPLY_HEALTH, ID_HEALTH, GL_CLAMP);
	OpenGLImage* imageWeapon = new OpenGLImage(PATH_WEAPON, ID_WEAPON, GL_CLAMP);
	OpenGLImage* imageBomb = new OpenGLImage(PATH_BOMB, ID_BOMB, GL_CLAMP);

	delete imageHero;
	delete imageEne1;
	delete imageRock;
	delete imageBg;
	delete imageHealth;
	delete imageWeapon;
	delete imageBomb;
}

void init()
{
	Hero* hero = new Hero();

	delete backGround;
	delete friendLink;
	delete rockLink;
	delete eneLink;
	delete friendFireLink;
	delete eneFireLink;
	delete panelLink;
	delete supplyLink;

	backGround = new BackGround(ID_BG);
	friendLink = new FriendLink();
	friendLink->appendNode(hero);
	rockLink = new RockLink();
	eneLink = new EneLink();
	friendFireLink = new FriendFireLink();
	eneFireLink = new EneFireLink();

	char lifeBarTitle[16] = "HP";
	Vec2f barPos;
	barPos[X] = -0.96f;
	barPos[Y] = 0.9f;
	Panel* lifeBar = new Panel(lifeBarTitle, &(friendLink->head->sprite->health), MAXHEROLIFE, barPos);
	lifeBar->color[R] = .8f;
	lifeBar->color[G] = .2f;
	lifeBar->color[B] = .0f;

	scOrE = new int;
	*scOrE = 0;
	char scoreBarTitle[16] = "SCORE";
	ScorePanel* scoreBar = new ScorePanel(scoreBarTitle);
	//scoreBar->pvalue = &(friendLink->head->sprite->health);
	scoreBar->pvalue = scOrE;
	scoreBar->color[R] = 1.f;
	scoreBar->color[G] = 0.f;
	scoreBar->color[B] = 0.f;


	BombNumberPanel* bombNum = new BombNumberPanel(&hero->numOfBombs);

	panelLink = new SpritesLink(scoreBar);
	panelLink->appendNode(lifeBar);
	panelLink->appendNode(bombNum);

	supplyLink = new SpritesLink();
}

void test()
{
	//srand(10);
	loadTexture();
	init();
	prog = new GameProgram();
	printf("done/n");
}



Panel::Panel()
{
}

Panel::Panel(char * title_, int max_, Vec2f pos_)
{
	sprintf(title, title_);
	max = max_;
	scaleFactor = 0.04f / 119.05f;
	Vec2fCopy(pos, pos_);
}

Panel::Panel(char* title_, int *pvalue_, int max_, Vec2f pos_)
{
	sprintf(title, title_);
	pvalue = pvalue_;
	max = max_;
	scaleFactor = 0.04f/119.05f;
	Vec2fCopy(pos, pos_);
}

void Panel::display()
{
	glClearColor(0, 0, 0, 1);
	glColor3d(.95f, .95f, .95f);
	glLoadIdentity();
	glPushMatrix();
	{
		glTranslatef(pos[X], pos[Y], 0.f);
		glBegin(GL_POLYGON);
		glVertex2d(0.f, 0.f);
		glVertex2d(0.6f, 0.f);
		glVertex2d(0.6f, 0.06f);
		glVertex2d(0.f, 0.06f);
		glEnd();

		glColor3d(color[R], color[G], color[B]);
		glPushMatrix();
		{
			glTranslatef(0.01f, 0.01f, 0.f);
			glScalef(scaleFactor, scaleFactor, 0.);
			for (int i = 0; i < 16; i++)
				glutStrokeCharacter(GLUT_STROKE_ROMAN, title[i]);
			//glutStrokeCharacter(GLUT_STROKE_ROMAN, 'H');
			//glutStrokeCharacter(GLUT_STROKE_ROMAN, 'P');
		}
		glPopMatrix();

		glPushMatrix();
		{
			if (*pvalue > 0)
			{
				glTranslatef(0.08f, 0.f, 0.f);
				glBegin(GL_POLYGON);
				glVertex2d(0.01, 0.01f);
				glVertex2d(0.01f + 0.5f*(float)*pvalue / MAXHEROLIFE, 0.01f);
				glVertex2d(0.01f + 0.5f*(float)*pvalue / MAXHEROLIFE, 0.05f);
				//glVertex2d(0.51f, 0.01f);
				//glVertex2d(0.51f, 0.05f);
				glVertex2d(0.01f, 0.05f);
				glEnd();
			}
		}
		glPopMatrix();

		glPushMatrix();
		{
			glColor3d(.95f, .95f, .95f);
			glTranslatef(0.08f, 0.f, 0.f);
			glBegin(GL_TRIANGLES);
			glVertex2d(0.01f, 0.01f);
			glVertex2d(0.09f, 0.05f);
			glVertex2d(0.01f, 0.05f);
			glEnd();
		}
		glPopMatrix();


	}
	glPopMatrix();
}

EneLink::EneLink()
{
	linkLength = 0;
	cdTotal = 300;
	cdRest = cdTotal;
	amount = 3;

	cdSEneTotal = 500;
	cdSEneRest = cdSEneTotal;

}

void EneLink::update()
{
	SpritesLink::update();

	//Ene1
	if (cdRest > 0)
	{
		cdRest--;
	}
	if (cdRest == 0 && linkLength < amount)
	{
		Vec2f pos_, movePos, dir;
		if (rand() % 2)
		{
			pos_[X] = (rand() % 2 - 0.5f)*2.6f;
			pos_[Y] = ((rand() % 600) - 300) / 300.f;

		}
		else
		{
			pos_[X] = ((rand() % 600) - 300) / 300.f;
			pos_[Y] = (rand() % 2 - 0.5f)*2.6f;
		}
		Vec2fSub(dir, pos_, friendLink->head->sprite->pos);
		//Vec2fClamp(pos_, -1.f, 1.f);
		Vec2fUnit(dir);
		Vec2fAddS(movePos, 0.9f, dir, friendLink->head->sprite->pos);
		Vec2fClamp(movePos, -0.9f, 0.65f);

		int amount = rand() % 3 + 1;
		Enemy* ene1;
		for (int i = 0; i < amount; i++)
		{
			pos_[X] += 0.2f;
			movePos[X] += 0.18f;
			ene1 = new Enemy(pos_, movePos, ID_ENE1);
			appendNode(ene1);
		}
		cdRest = cdTotal;
	}

	//Ene2
	if (cdSEneRest > 0)
		cdSEneRest--;
	if (cdSEneRest == 0)
	{
		Vec2f pos_, movePos, dir;

		pos_[X] = ((rand() % 600) - 300) / 300.f;
		pos_[Y] = 1.3f;

		Vec2fSub(dir, pos_, friendLink->head->sprite->pos);
		//Vec2fClamp(pos_, -1.f, 1.f);
		Vec2fUnit(dir);
		Vec2fAddS(movePos, 1.3f, dir, friendLink->head->sprite->pos);
		Vec2fClamp(movePos, -0.9f, 0.9f);

		SmartEnemy* ene2 = new SmartEnemy(pos_, movePos, ID_ENE2);
		appendNode(ene2);
		cdSEneRest = cdSEneTotal;
	}

	suffererCollision(friendFireLink);
}

BackGround::BackGround(GLuint ID)
{
	y = 0.;
	speed = 0.0005;
	textureID_ = ID;
}

void BackGround::display()
{	
	//glColor3d(0, 0, 0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureID_);
	glBegin(GL_QUADS);
	{
		glTexCoord2d(0., y);
		glVertex2d(-1., -1.);
		glTexCoord2d(1., y);
		glVertex2d(1. ,-1.);
		glTexCoord2d(1., y+0.25);
		glVertex2d(1., 1.);
		glTexCoord2d(0., y+0.25);
		glVertex2d(-1., 1.);
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);
	y += speed;
	if (y > 1.)
		y -= 1.;
	//printf("y = %f", y);
}

Ene1Bullet::Ene1Bullet(Vec2f pos_, float angle_, float speed)
{
	Vec2fCopy(pos, pos_);
	v[X] = speed*cos(angle_*PI / 180.f);
	v[Y] = speed*sin(angle_*PI / 180.f);
	//printf("angle %f\n", angle_);
	//Vec2fPrint(v);

	acc[X] = 0.f;
	acc[Y] = 0.f;
	friction = 0.f;

	angle = angle_ + 90.f;
	size = 0.008;

	toBeDisposed = false;
	scorePoint = 0;

	hurterATK = 20;
}

void Ene1Bullet::display()
{
	SBHSprite::display();

	glColor3d(0.7f, 0.7f, 0.9f);
	glBegin(GL_POLYGON);
	glVertex2d(-0.004, 0.f);
	glVertex2d(+0.004, 0.f);
	glVertex2d(+0.004, 0.02);
	glVertex2d(-0.004, 0.02);
	glEnd();
	glPopMatrix();
}

void FriendLink::update()
{
	SpritesLink::update();
	suffererCollision(supplyLink);
	suffererCollision(rockLink);
	suffererCollision(eneLink);
	suffererCollision(eneFireLink);
}

ScorePanel::ScorePanel(char* title_)
{
	sprintf(title, title_);
	scaleFactor = 0.05f / 119.05f;
	pos[X] = 0.5f;
	pos[Y] = 0.88f;
}

void ScorePanel::display()
{
	glClearColor(0, 0, 0, 1);
	glColor3d(.6f, .6f, .6f);
	glLoadIdentity();
	glPushMatrix();
	{
		glTranslatef(pos[X], pos[Y], 0.f);
		glBegin(GL_POLYGON);
		glVertex2d(0.f, 0.f);
		glVertex2d(0.44f, 0.f);
		glVertex2d(0.44f, 0.08f);
		glVertex2d(0.f, 0.08f);
		glEnd();

		glColor3d(1.f,0.8f,0.2f);
		glPushMatrix();
		{
			glTranslatef(0.02f, 0.02f, 0.f);
			glScalef(scaleFactor, scaleFactor, 0.);
			char chars[16];
			sprintf(chars,"SCORE: %d", *pvalue);
			for (int i = 0; i < 16;i++)
				glutStrokeCharacter(GLUT_STROKE_ROMAN, chars[i]);
		}
		glPopMatrix();

	}
	glPopMatrix();

}

SmartEnemy::SmartEnemy(Vec2f pos_, Vec2f movePos_, GLuint ID)
{
	Vec2fCopy(pos, pos_);
	Vec2fCopy(movePos, movePos_);
	reachedPos = false;
	v[X] = 0.f;
	v[Y] = 0.f;
	vlimit = 0.02f;
	acc[X] = 0.f;
	acc[Y] = 0.f;
	alimit = 0.004f;
	friction = -0.002f;

	size = 0.08f;
	angle = 0.f;
	textureID_ = ID;

	cdOuterTotal = 100;
	cdOuterRest = cdOuterTotal;
	innerAmount = 2;
	innerCount = 0;
	cdInnerTotal = 4;
	cdInnerRest = cdInnerTotal;
	target = friendLink->head->sprite;
	bulletSpeed = 0.05f;

	safeDistance = 0.5f;

	health = MAXENELIFE;
	toBeDisposed = false;
	scorePoint = 200;

	hurterATK = MAXHEROLIFE + 1;
}

void SmartEnemy::update()
{
	if (health < 0)
	{
		*scOrE += scorePoint;
		dropSupply(pos, rand() % 2, 4);
		dropSupply(pos, rand() % 2, 4);
		dropSupply(pos, 2, 10);
		toBeDisposed = true;
		return;
	}

	//acc
	if (!reachedPos)
	{
		if (pos[X] + SMALL_DISTANCE < movePos[X] || pos[X] - SMALL_DISTANCE > movePos[X] ||
			pos[Y] + SMALL_DISTANCE < movePos[Y] || pos[Y] - SMALL_DISTANCE > movePos[Y])
		{
			Vec2fSub(acc, movePos, pos);
			Vec2fUnit(acc);
			acc[X] *= alimit;
			acc[Y] *= alimit;
		}
		else
		{
			acc[X] = 0.f;
			acc[Y] = 0.f;
			//v[X] /= 2.f;
			//v[Y] /= 2.f;
			reachedPos = true;
		}
	}


	//dodge bullets
	if(reachedPos)
		dodgeBullet();

	SBHSprite::update();
	//velocity and pos limit
	Vec2fClamp(v, -vlimit, vlimit);
	pos[X] += v[X];
	pos[Y] += v[Y];
	if(reachedPos)
		Vec2fClamp(pos, -0.95f, 0.95f);


	//face to hero
	Vec2f dir;
	Vec2fSub(dir, target->pos, pos);

	if (Vec2fMag(dir) > SMALL_NUM)
	{
		Vec2f Predict;
		double DD, t, v1v2, v1p;
		Vec2fSub(dir, target->pos, pos);
		v1v2 = Vec2fDotProd(target->v, target->v) - bulletSpeed*bulletSpeed;
		v1p = Vec2fDotProd(dir, target->v);
		DD = v1p*v1p - v1v2*Vec2fDotProd(dir, dir);
		t = (sqrt(DD) + v1p) / -v1v2;
		Vec2fAddS(Predict, t, target->v, target->pos);

		Vec2fSub(dir, Predict, pos);

		Vec2fUnit(dir);
		if (dir[Y] > 0.f)
			angle = acos(dir[X]) / PI*180.f + 90.f;
		else
			angle = -acos(dir[X]) / PI*180.f + 90.f;
	}


	if (reachedPos)
	{
		//clear
		if (pos[X] + size + size < world.l || pos[X] - size - size > world.r ||
			pos[Y] + size + size < world.b || pos[Y] - size - size > world.t)
		{
			toBeDisposed = true;
		}
		else
		{
			//open fire
			if (cdOuterRest > 0)
				cdOuterRest--;
			if (cdOuterRest == 0)
			{
				if (cdInnerRest > 0)
					cdInnerRest--;
				if (cdInnerRest == 0)
				{
					Ene2Bullet* bullet = new Ene2Bullet(pos, angle - 90.f, bulletSpeed);
					eneFireLink->appendNode(bullet);
					cdInnerRest = cdInnerTotal;
					innerCount++;
				}
				if (innerCount > innerAmount)
				{
					cdOuterRest = cdOuterTotal;
					innerCount = 0;
				}
			}
		}
	}

}

void SmartEnemy::dodgeBullet()
{
	SBHNode* node = friendFireLink->head;
	Vec2f p2p1;
	double dis, dotProd;
	while (node != NULL)
	{
		Vec2fSub(p2p1, pos, node->sprite->pos);
		dis = Vec2fMag(p2p1);
		dotProd = Vec2fDotProd(p2p1, node->sprite->v);
		if (dis < safeDistance && dotProd>0)
		{
			double v, cos_, sin_;
			v= Vec2fMag(node->sprite->v);
			cos_ = dotProd / dis / v;
			sin_ = size / dis;
			if (cos_*cos_ + sin_*sin_ > 1.)
			{
				Vec2f vertical, acc_;
				Vec2fCopy(vertical, node->sprite->v);
				Vec2fUnit(vertical);
				vertical[X] *= dis*cos_;
				vertical[Y] *= dis*cos_;
				Vec2fSub(acc_, p2p1, vertical);
				if (Vec2fMag(acc_) > SMALL_NUM)
				{
					Vec2fUnit(acc_);
				}
				else
				{
					acc_[X] = -vertical[Y];
					acc_[Y] = vertical[X];
					Vec2fUnit(acc_);
				}
				acc[X] = alimit*acc_[X];
				acc[Y] = alimit*acc_[Y];
				return;
			}
		}
		node = node->nextNode;
	}
	acc[X] = 0.f;
	acc[Y] = 0.f;
}

void Supply::display()
{
	SBHSprite::display();
	SBHSprite::displayTexture(textureID_, 0.2f);
}

void Supply::update()
{
	if (lifeCycle < 0)
	{
		toBeDisposed = true;
	}
	Vec2f r;
	if (orbitAngle > 0.f)
	{
		r[X] = cos(orbitAngle*PI / 180.f);
		r[Y] = sin(orbitAngle*PI / 180.f);
	}
	else
	{
		r[X] = cos(orbitAngle*PI / 180.f);
		r[Y] = sin(orbitAngle*PI / 180.f);
	}
	Vec2fAddS(pos, radius, r, center);
	orbitAngle += 5.f;
	lifeCycle--;
}

AidBox::AidBox(Vec2f center_)
{
	Vec2fCopy(center, center_);
	angle = 0.f;
	orbitAngle = 0.f;
	radius = 0.1f;
	size=0.04;
	textureID_ = ID_HEALTH;

	lifeCycle = 500;
	toBeDisposed = false;
	scorePoint = 0;

	hurterATK = 200;
}

void AidBox::collision(SBHSprite * sufferer)
{
	if (pos[X] + size<sufferer->pos[X] - sufferer->size ||
		pos[X] - size>sufferer->pos[X] + sufferer->size ||
		pos[Y] + size<sufferer->pos[Y] - sufferer->size ||
		pos[Y] - size>sufferer->pos[Y] + sufferer->size
		)
	{
		return;
	}
	else
	{
		Vec2f distance;
		Vec2fSub(distance, pos, sufferer->pos);
		if (Vec2fMag(distance) < sufferer->size + size)
		{
			sufferer->health += hurterATK;
			if (sufferer->health > MAXHEROLIFE)
				sufferer->health = MAXHEROLIFE;
			toBeDisposed = true;
		}
	}

}

WeaponBox::WeaponBox(Vec2f center_)
{
	Vec2fCopy(center, center_);
	angle = 0.f;
	orbitAngle = 0.f;
	radius = 0.05f;
	size = 0.04;
	textureID_ = ID_WEAPON;

	lifeCycle = 500;
	toBeDisposed = false;
	scorePoint = 0;

	hurterATK = 10;
}

void WeaponBox::collision(SBHSprite * sufferer)
{
	if (pos[X] + size<sufferer->pos[X] - sufferer->size ||
		pos[X] - size>sufferer->pos[X] + sufferer->size ||
		pos[Y] + size<sufferer->pos[Y] - sufferer->size ||
		pos[Y] - size>sufferer->pos[Y] + sufferer->size
		)
	{
		return;
	}
	else
	{
		Vec2f distance;
		Vec2fSub(distance, pos, sufferer->pos);
		if (Vec2fMag(distance) < sufferer->size + size)
		{
			//lvl up
			if (sufferer->lvl < 2)
			{
				sufferer->lvl++;
				friendFireLink->cdTotal--;
			}
			toBeDisposed = true;
		}
	}

}

BombBox::BombBox(Vec2f center_)
{
	Vec2fCopy(center, center_);
	angle = 0.f;
	orbitAngle = 0.f;
	radius = 0.05f;
	size = 0.04;
	textureID_ = ID_BOMB;

	lifeCycle = 500;
	toBeDisposed = false;
	scorePoint = 0;

	hurterATK = 10;
}

void BombBox::collision(SBHSprite * sufferer)
{
	if (pos[X] + size<sufferer->pos[X] - sufferer->size ||
		pos[X] - size>sufferer->pos[X] + sufferer->size ||
		pos[Y] + size<sufferer->pos[Y] - sufferer->size ||
		pos[Y] - size>sufferer->pos[Y] + sufferer->size
		)
	{
		return;
	}
	else
	{
		Vec2f distance;
		Vec2fSub(distance, pos, sufferer->pos);
		if (Vec2fMag(distance) < sufferer->size + size)
		{
			//one more bomb
			if (sufferer->numOfBombs < 5)
				sufferer->numOfBombs++;
			toBeDisposed = true;
		}
	}

}

Bomb::Bomb(Vec2f pos_)
{
	Vec2fCopy(pos, pos_);

	angle = 0.f;
	size = 0.008;

	toBeDisposed = false;

	hurterATK = MAXENELIFE + 1;

}

void Bomb::display()
{
	SBHSprite::display();

	glColor3d(0.8f, 0.8f, 0.8f);
	glBegin(GL_LINE_LOOP);
	float angle_ = 0.f;
	for (int i = 0; i < 90; i++) 
	{
		glVertex2d(size*cos(angle_*PI / 180.0f), size*sin(angle_*PI / 180.0f));
		angle_ += 4.f;
	}
	glEnd();
	glPopMatrix();
}

void Bomb::update()
{
	size += 0.03;
	if (size > 3.f)
	{
		toBeDisposed = true;
		eneFireLink->bomb = NULL;
	}
}

void Bomb::collision(SBHSprite * sufferer)
{
	if (pos[X] + size<sufferer->pos[X] - sufferer->size ||
		pos[X] - size>sufferer->pos[X] + sufferer->size ||
		pos[Y] + size<sufferer->pos[Y] - sufferer->size ||
		pos[Y] - size>sufferer->pos[Y] + sufferer->size
		)
	{
		return;
	}
	else
	{
		Vec2f distance;
		Vec2fSub(distance, pos, sufferer->pos);
		if (Vec2fMag(distance) < sufferer->size + size)
		{
			//attack
			sufferer->health -= hurterATK;
			sufferer->toBeDisposed = true;
		}
	}
}

EneFireLink::EneFireLink()
{
	SpritesLink::SpritesLink();
	bomb = NULL;
}

void EneFireLink::update()
{
	SpritesLink::update();
	if (bomb != NULL)
	{
		suffererCollision(bomb);
	}
}

void EneFireLink::suffererCollision(SBHSprite * bomb_)
{
	SBHNode* node = head;
	while (node != NULL)
	{
		bomb_->collision(node->sprite);
		node = node->nextNode;
	}
}

BombNumberPanel::BombNumberPanel(int * pvalue_)
{
	pvalue = pvalue_;
	scaleFactor = 0.04f / 119.05f;
	pos[X] = -0.98f;
	pos[Y] = 0.85f;
	angle = 0.f;
}

void BombNumberPanel::display()
{
	SBHSprite::display();
	for (int bomb = 0; bomb < 5; bomb++)
	{
		glTranslatef(0.1f, 0.f, 0.f);
		displayCircle(friendLink->head->sprite->numOfBombs > bomb);
		//displayCircle(false);
	}
	glPopMatrix();
}

void BombNumberPanel::displayCircle(bool oo)
{
	glPushMatrix();
	glColor3d(0.8f, 0.8f, 0.8f);
	glBegin(GL_LINE_LOOP);
	float angle_ = 0.f;
	float size = 0.03;
	for (int i = 0; i < 90; i++)
	{
		glVertex2d(size*cos(angle_*PI / 180.0f), size*sin(angle_*PI / 180.0f));
		angle_ += 4.f;
	}
	glEnd();
	if(oo)
		glColor3d(0.2f, 0.2f, 0.2f);
	else
		glColor3d(0.6f, 0.6f, 0.6f);
	glBegin(GL_POLYGON);
	angle_ = 0.f;
	for (int i = 0; i < 36; i++)
	{
		glVertex2d(size*cos(angle_*PI / 180.0f), size*sin(angle_*PI / 180.0f));
		angle_ += 10.f;
	}
	glEnd();
	glPopMatrix();

}

Ene2Bullet::Ene2Bullet(Vec2f pos_, float angle_, float speed)
{
	Vec2fCopy(pos, pos_);
	v[X] = speed*cos(angle_*PI / 180.f);
	v[Y] = speed*sin(angle_*PI / 180.f);
	//printf("angle %f\n", angle_);
	//Vec2fPrint(v);

	acc[X] = 0.f;
	acc[Y] = 0.f;
	friction = 0.f;

	angle = angle_ + 90.f;
	size = 0.012;

	toBeDisposed = false;
	scorePoint = 0;

	hurterATK = 75;
}

void Ene2Bullet::display()
{
	SBHSprite::display();

	glColor3d(1.f, 0.5f, 0.2f);
	glBegin(GL_POLYGON);
	glVertex2d(-0.006, 0.f);
	glVertex2d(+0.006, 0.f);
	glVertex2d(+0.006, 0.03);
	glVertex2d(-0.006, 0.03);
	glEnd();
	glPopMatrix();
}
