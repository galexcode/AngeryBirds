//
//  HelloWorldScene.h
//  AngeryBirds
//
//  Created by Law Lincoln on 12-10-30.
//  Copyright __MyCompanyName__ 2012年. All rights reserved.
//
#ifndef __HELLO_WORLD_H__
#define __HELLO_WORLD_H__

// When you import this file, you import all the cocos2d classes
#include "cocos2d.h"
#include "Box2D.h"
#include "MyContactListener.h"
#include "GLES-Render.h"
USING_NS_CC;

class MySprite:public CCSprite{
private:
    bool init();
    b2Body *m_pBody;
public:
    void setBody(b2Body *body);
    b2Body* getBody();
    CREATE_FUNC(MySprite);
};
class HelloWorld : public cocos2d::CCLayer {
public:
    ~HelloWorld();
    HelloWorld();
    
    // returns a Scene that contains the HelloWorld as the only child
    static cocos2d::CCScene* scene();
    
    b2World* world;
	GLESDebugDraw *m_debugDraw;
    
    b2Fixture *armFixture;
    b2Body *armBody;
    b2RevoluteJoint *armJoint;
    b2MouseJoint *mouseJoint;
    b2Body *groundBody;
    
    CCArray *bullets;
    int currentBullet;
    
    b2Body *bulletBody;
    b2WeldJoint *bulletJoint;
    
    bool releasingArm;
    
    CCArray *targets;
    CCArray *enemies;
    
    MyContactListener *_contactListener;
    
    CCSize winSize;
    
    void resetGame();
    void resetBullet();
    void creatBullets(int count);
    bool attachBullet();
    void creatTargets();
    void tick(float dt);
    void creatTarget(char const * image,CCPoint position,float rotation, bool isCircle,bool isStatic ,bool isEnemy);
    void createTargets();
    void ccTouchesBegan(CCSet *pTouches,CCEvent*pEvent);
    void ccTouchesMoved(CCSet *pTouches,CCEvent*pEvent);
    void ccTouchesEnded(CCSet *pTouches,CCEvent*pEvent);
    //void draw();
};

#endif // __HELLO_WORLD_H__






















