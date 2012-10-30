//
//  HelloWorldScene.cpp
//  AngeryBirds
//
//  Created by Law Lincoln on 12-10-30.
//  Copyright __MyCompanyName__ 2012年. All rights reserved.
//
#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

using namespace cocos2d;
using namespace CocosDenshion;

#define PTM_RATIO 32
#define FLOOR_HEIGTH    62.0f
#define Addtexture(__NAME__) CCTextureCache::sharedTextureCache()->addImage(__NAME__)
enum {
	kTagTileMap = 1,
	kTagBatchNode = 1,
	kTagAnimation1 = 1,
};
bool MySprite::init(){
    if (!CCSprite::init()) {
        return  false;
    }
    return true;
}
void MySprite::setBody(b2Body *body){
    m_pBody = body;
}
b2Body*MySprite::getBody(){
    return m_pBody;
}
HelloWorld::HelloWorld()
{
    bullets = new CCArray();
    targets = new CCArray();
    enemies = new CCArray();

    setTouchEnabled( true );
    setAccelerometerEnabled( true );
    winSize = CCDirector::sharedDirector()->getWinSize();
    
    b2Vec2 gravity;
    gravity.Set(0.0f, -10.0f);
    
    bool doSleep = true;
    
    world = new b2World(gravity);
    world->SetAllowSleeping(doSleep);
    world->SetContinuousPhysics(true);
    
    m_debugDraw=new GLESDebugDraw(PTM_RATIO);
    world->SetDebugDraw(m_debugDraw);
    uint32 flags = 0;
    flags += b2Draw::e_shapeBit;
    flags += b2Draw::e_jointBit;
    //flags += b2Draw::e_aabbBit;
    flags += b2Draw::e_pairBit;
    flags += b2Draw::e_centerOfMassBit;
    m_debugDraw->SetFlags(flags);
    
    CCSprite *sprite = CCSprite::create("bg.png");
    sprite->setAnchorPoint(CCPointZero);
    this->addChild(sprite,-1);
    
    CCSprite *sprite2 = CCSprite::create("catapult_base_2.png");
    sprite2->setAnchorPoint(CCPointZero);
    sprite2->setPosition(ccp(181.0f, FLOOR_HEIGTH));
    this->addChild(sprite2,0);
    
    CCSprite *sprite3 = CCSprite::create("squirrel_1.png");
    sprite3->setAnchorPoint(CCPointZero);
    sprite3->setPosition(ccp(11.0f, FLOOR_HEIGTH));
    this->addChild(sprite3,0);
    
    CCSprite *sprite4 = CCSprite::create("catapult_base_1.png");
    sprite4->setAnchorPoint(CCPointZero);
    sprite4->setPosition(ccp(181.0f, FLOOR_HEIGTH));
    this->addChild(sprite4,9);
    
    CCSprite *sprite5 = CCSprite::create("squirrel_2.png");
    sprite5->setAnchorPoint(CCPointZero);
    sprite5->setPosition(ccp(240.0f, FLOOR_HEIGTH));
    this->addChild(sprite5,9);
    
    CCSprite *sprite6 = CCSprite::create("fg.png");
    sprite6->setAnchorPoint(CCPointZero);
    sprite6->setPosition(ccp(0, 0));
    this->addChild(sprite6,9);
    
    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(0, 0);
    
    groundBody = world->CreateBody(&groundBodyDef);
    
    //替换掉 b2PolygonShape
    float widthInMeters = 1800/ PTM_RATIO;
    float heightInMeters = winSize.height / PTM_RATIO;
    b2Vec2 lowerLeftCorner = b2Vec2(0, FLOOR_HEIGTH/PTM_RATIO);
    b2Vec2 lowerRightCorner = b2Vec2(widthInMeters, FLOOR_HEIGTH/PTM_RATIO);
    b2Vec2 upperLeftCorner = b2Vec2(0, heightInMeters);
    b2Vec2 upperRightCorner = b2Vec2(widthInMeters, heightInMeters);
    b2EdgeShape screenBorderShape;
    
    screenBorderShape.Set(lowerLeftCorner, lowerRightCorner);
    groundBody->CreateFixture(&screenBorderShape, 0);
    
    screenBorderShape.Set(lowerRightCorner, upperRightCorner);
    groundBody->CreateFixture(&screenBorderShape, 0);
    
    screenBorderShape.Set(upperRightCorner, upperLeftCorner);
    groundBody->CreateFixture(&screenBorderShape, 0);
    
    screenBorderShape.Set(upperLeftCorner, lowerLeftCorner);
    groundBody->CreateFixture(&screenBorderShape, 0);
    
    MySprite *arm = (MySprite*)CCSprite::create("catapult_arm.png");

    this->addChild(arm,1);
    
    b2BodyDef armBodyDef;
    armBodyDef.type = b2_dynamicBody;
    armBodyDef.linearDamping = 1;
    armBodyDef.angularDamping = 1;
    armBodyDef.position.Set(230.0f/PTM_RATIO,(FLOOR_HEIGTH+91.0f)/PTM_RATIO);
    armBodyDef.userData = arm;
    armBody = world->CreateBody(&armBodyDef);
    arm->setBody(armBody);
    b2PolygonShape armBox;
    b2FixtureDef armBoxDef;
    armBoxDef.shape = &armBox;
    armBoxDef.density = 0.3F;
    armBox.SetAsBox(11.0f/PTM_RATIO, 91.0f/PTM_RATIO);
    armFixture = armBody->CreateFixture(&armBoxDef);
    
    b2RevoluteJointDef armJointDef;
    armJointDef.Initialize(groundBody, armBody, b2Vec2(233.0f/PTM_RATIO,FLOOR_HEIGTH/PTM_RATIO));
    armJointDef.enableMotor = true;
    armJointDef.enableLimit = true;
    armJointDef.motorSpeed = -10;
    armJointDef.lowerAngle = CC_DEGREES_TO_RADIANS(9);
    armJointDef.upperAngle = CC_DEGREES_TO_RADIANS(75);
    armJointDef.maxMotorTorque = 700;
    
    armJoint = (b2RevoluteJoint*)world->CreateJoint(&armJointDef);
    
    schedule(schedule_selector(HelloWorld::tick));
    CCDelayTime *delayAction = CCDelayTime::create(2.0f);
    CCCallFunc *callSelectorAction = CCCallFunc::create(this,callfunc_selector(HelloWorld::resetGame));
    this->runAction(CCSequence::create(delayAction,callSelectorAction,NULL));
    _contactListener = new MyContactListener();
    world->SetContactListener(_contactListener);
}
void HelloWorld::resetGame(){
    if (bullets->count()) {
        CCObject *sprite;
        CCARRAY_FOREACH(bullets, sprite){
            MySprite *bullet = (MySprite*)sprite;
            b2Body *bullet_Body = bullet->getBody();
            this->removeChild(bullet, true);
            world->DestroyBody(bullet_Body);
        }
        //bullets->release();
        bullets ->removeAllObjects();
        
    }
    if (targets->count()) {
        CCObject *sprite;
        CCARRAY_FOREACH(targets, sprite){
            MySprite *target = (MySprite*)sprite;
            b2Body *target_Body = target->getBody();
            this->removeChild(target, true);
            world->DestroyBody(target_Body);
        }
        //targets->release();
        targets ->removeAllObjects();
    }
    if (enemies->count()) {
        enemies->removeAllObjects();
    }
    this->creatBullets(4);
    this->creatTargets();
    this->runAction(CCSequence::create(
                                       CCMoveTo::create(1.5f, ccp(-480, 0.0f)),
                                       CCCallFuncN::create(this, callfuncN_selector(HelloWorld::attachBullet)),
                                       CCDelayTime::create(10.f),
                                       CCMoveTo::create(1.5f, CCPointZero),
                                       NULL));
}
void HelloWorld::creatBullets(int count){
    currentBullet = 0;
    float pos = 62.0f;
    if (count>0) {
        float delta = (count>1)?((165.0f-32.0f-30.0f)/(count-1)):0.0f;
        for (int i = 0; i<count ; i++,pos+=delta) {
            MySprite *sprite = (MySprite*)CCSprite::create("acorn.png");
            this->addChild(sprite,1);
            sprite->setTag(1);
            b2BodyDef bulletBodyDef;
            bulletBodyDef.type = b2_dynamicBody;
            bulletBodyDef.bullet = true;
            bulletBodyDef.position.Set(pos/PTM_RATIO,(FLOOR_HEIGTH+15.0f)/PTM_RATIO);
            bulletBodyDef.userData = sprite;
            b2Body *bullet = world->CreateBody(&bulletBodyDef);
            bullet->SetActive(false);
            sprite->setBody(bullet);
            
            b2CircleShape circle;
            circle.m_radius = 15.0/PTM_RATIO;
            
            b2FixtureDef ballShapeDef;
            ballShapeDef.shape = &circle;
            ballShapeDef.density = 0.8f;
            ballShapeDef.restitution = 0.2f;
            ballShapeDef.friction = 0.99f;
            ballShapeDef.userData = (void*)2;
            bullet->CreateFixture(&ballShapeDef);
            
            bullets->addObject(sprite);
        }
    }
}
bool HelloWorld::attachBullet(){
    if (currentBullet < bullets->count()) {
        MySprite *bulletSp = (MySprite*)bullets->objectAtIndex(currentBullet++);
        bulletBody = bulletSp->getBody();
        bulletBody->SetTransform(b2Vec2(230.0f/PTM_RATIO,(155.0f+FLOOR_HEIGTH)/PTM_RATIO), 0.0f);
        bulletBody->SetActive(true);
        
        b2WeldJointDef welJointDef;
        welJointDef.Initialize(bulletBody, armBody, b2Vec2(230.0f/PTM_RATIO,(155.0f+FLOOR_HEIGTH)/PTM_RATIO));
        welJointDef.collideConnected = false;
        
        bulletJoint = (b2WeldJoint*)world->CreateJoint(&welJointDef);
        return true;
    }
    return  false;
}
void HelloWorld::resetBullet(){
    if (enemies->count() == 0) {
        CCDelayTime *delayAction = CCDelayTime::create(2.0f);
        CCCallFunc *callSelectorAction = CCCallFunc::create(this,callfunc_selector(HelloWorld::resetGame));
        this->runAction(CCSequence::create(delayAction,callSelectorAction,NULL));
    }else if(this->attachBullet()){
        this->runAction(CCMoveTo::create(2.0f, CCPointZero));
    }else{
        CCDelayTime *delayAction = CCDelayTime::create(2.0f);
        CCCallFunc *callSelectorAction = CCCallFunc::create(this,callfunc_selector(HelloWorld::resetGame));
        this->runAction(CCSequence::create(delayAction,callSelectorAction,NULL));
    }
}
void HelloWorld::creatTarget(const char *image, cocos2d::CCPoint position, float rotation, bool isCircle, bool isStatic, bool isEnemy){
    MySprite *sprite = (MySprite*)CCSprite::create(image);
    this->addChild(sprite,1);
    b2BodyDef bodyDef;
    bodyDef.type  = isStatic?b2_staticBody:b2_dynamicBody;
    bodyDef.position.Set((position.x+sprite->getContentSize().width/2.0f)/PTM_RATIO,
                         (position.y+sprite->getContentSize().height/2.0f)/PTM_RATIO);
    bodyDef.angle = CC_DEGREES_TO_RADIANS(rotation);
    bodyDef.userData = sprite;
    b2Body *body = world->CreateBody(&bodyDef);
    sprite->setBody(body);
    
    b2FixtureDef boxDef;
    if (isCircle) {
        b2CircleShape circle;
        circle.m_radius = sprite->getContentSize().width/2.0f/PTM_RATIO;
        boxDef.shape = &circle;
    }else{
        b2PolygonShape box;
        box.SetAsBox(sprite->getContentSize().width/2.0f/PTM_RATIO, sprite->getContentSize().height/2.0f/PTM_RATIO);
        boxDef.shape = &box;
    }
    if (isEnemy) {
        sprite->setTag(2);
        enemies->addObject(sprite);
    }else{
        sprite->setTag(3);
    }
    boxDef.density = .5f;
    body->CreateFixture(&boxDef);
    targets->addObject(sprite);
}
void HelloWorld::creatTargets(){
    this->creatTarget("brick_2.png", ccp(675.0, FLOOR_HEIGTH), 0.0, false, false, false);
    this->creatTarget("brick_1.png", ccp(741.0, FLOOR_HEIGTH), 0.0, false, false, false);
    this->creatTarget("brick_1.png", ccp(741.0, FLOOR_HEIGTH+23.0f), 0.0, false, false, false);
    this->creatTarget("brick_3.png", ccp(672.0, FLOOR_HEIGTH+46.0f), 0.0, false, false, false);
    this->creatTarget("brick_1.png", ccp(707.0, FLOOR_HEIGTH+58.0f), 0.0, false, false, false);
    this->creatTarget("brick_1.png", ccp(707.0, FLOOR_HEIGTH+81.0f), 0.0, false, false, false);
    
    
    this->creatTarget("head_dog.png", ccp(702.0, FLOOR_HEIGTH), 0.0, true, false, true);
    this->creatTarget("head_cat.png", ccp(680.0, FLOOR_HEIGTH+58.0f), 0.0, true, false, true);
    this->creatTarget("head_dog.png", ccp(740.0, FLOOR_HEIGTH+58.0f), 0.0, true, false, true);
    
    this->creatTarget("brick_2.png", ccp(770.0, FLOOR_HEIGTH), 0.0, false, false, false);
    this->creatTarget("brick_2.png", ccp(770.0, FLOOR_HEIGTH+46.0f), 0.0, false, false, false);
    
    this->creatTarget("head_dog.png", ccp(830.0, FLOOR_HEIGTH), 0.0, true, false, true);
    
    this->creatTarget("brick_platform.png", ccp(839.0, FLOOR_HEIGTH), 0.0, false, true, false);
    this->creatTarget("brick_2.png", ccp(854.0, FLOOR_HEIGTH+28.0f), 0.0, false, false, false);
    this->creatTarget("brick_2.png", ccp(675.0, FLOOR_HEIGTH+28.0f+46.0f), 0.0, false, false, false);
    this->creatTarget("head_cat.png", ccp(881.0, FLOOR_HEIGTH+28.0f), 0.0, true, false, true);
    this->creatTarget("brick_2.png", ccp(909.0, FLOOR_HEIGTH+28.0f), 0.0, false, false, false);
    this->creatTarget("brick_1.png", ccp(909.0, FLOOR_HEIGTH+28.0f+46.0f), 0.0, false, false, false);
    this->creatTarget("brick_1.png", ccp(909.0, FLOOR_HEIGTH+23.0f+28.0f+46.0f), 0.0, false, false, false);
    this->creatTarget("brick_2.png", ccp(882.0, FLOOR_HEIGTH+108.0f), 90.0, false, false, false);
    
}
void HelloWorld::tick(float dt){
    int32 velocityIterations = 8;
	int32 positionIterations = 1;
	world->Step(dt, velocityIterations, positionIterations);
    
    for (b2Body* b = world->GetBodyList(); b; b = b->GetNext())
	{
		if (b->GetUserData() != NULL) {
			//Synchronize the AtlasSprites position and rotation with the corresponding body
			CCSprite *myActor = (CCSprite*)b->GetUserData();
			myActor->setPosition(CCPointMake( b->GetPosition().x * PTM_RATIO, b->GetPosition().y * PTM_RATIO));
            myActor->setRotation(-1 * CC_RADIANS_TO_DEGREES(b->GetAngle()));
		}
	}
    
    // Arm is being released.
    if (releasingArm && bulletJoint)
    {
        // Check if the arm reached the end so we can return the limits
        if (armJoint->GetJointAngle() <= CC_DEGREES_TO_RADIANS(10))
        {
            releasingArm = false;
            
            // Destroy joint so the bullet will be free
            world->DestroyJoint(bulletJoint);
            bulletJoint = NULL;
            CCDelayTime *delayAction = CCDelayTime::create(5.0f);
            CCCallFunc *callSelectorAction = CCCallFunc::create(this,callfunc_selector(HelloWorld::resetBullet));
            this->runAction(CCSequence::create(delayAction,callSelectorAction,NULL));
        }
    }
    
    // Bullet is moving.
    if (bulletBody && bulletJoint == NULL)
    {
        b2Vec2 position = bulletBody->GetPosition();
        CCPoint myPosition = this->getPosition();
        
        // Move the camera.
        if (position.x > winSize.width / 2.0f / PTM_RATIO)
        {
            myPosition.x = -MIN(winSize.width * 2.0f - winSize.width, position.x * PTM_RATIO - winSize.width / 2.0f);
            this->setPosition(myPosition);
        }
    }
    
    // Check for impacts
    std::vector<b2Body*> toDestroy;
    std::vector<b2Body*> bulletToDestroy;
    std::vector<MyContact>::iterator pos;
    for(pos = _contactListener->_contacts.begin();
        pos != _contactListener->_contacts.end(); ++pos)
    {
        CCLOG("FUC");
        MyContact contact = *pos;
        CCPoint position;
        // Get the box2d bodies for each object
        b2Body *bodyA = contact.fixtureA->GetBody();
        b2Body *bodyB = contact.fixtureB->GetBody();
        if (bodyA->GetUserData() != NULL && bodyB->GetUserData() != NULL) {
            CCSprite *spriteA = (CCSprite *) bodyA->GetUserData();
            CCSprite *spriteB = (CCSprite *) bodyB->GetUserData();
            
            // Is sprite A a cat and sprite B a car?  If so, push the cat on a list to be destroyed...
            if (spriteA->getTag() == 1 && spriteB->getTag() == 2) {
               // toDestroy.push_back(bodyA);
                position = spriteA->getPosition();
              //  this->removeChild(spriteA, true);
              //  world->DestroyBody(bodyA);
              //  targets->removeObject(spriteA);
                 toDestroy.push_back(bodyB);
                bulletToDestroy.push_back(bodyA);
                CCLOG("YOU");
            }
            // Is sprite A a car and sprite B a cat?  If so, push the cat on a list to be destroyed...
             if (spriteA->getTag()== 2 && spriteB->getTag() == 1) {
               // toDestroy.push_back(bodyB);
                 position = spriteB->getPosition();
             //   this->removeChild(spriteB, true);
               // world->DestroyBody(bodyB);
             //   targets->removeObject(spriteB);
                toDestroy.push_back(bodyA);
                 bulletToDestroy.push_back(bodyB);
                CCLOG("SISTER");
            }
        }
        
    }
    std::vector<b2Body *>::iterator pos2;
    for(pos2 = toDestroy.begin(); pos2 != toDestroy.end(); ++pos2) {
        b2Body *body = *pos2;
        if (body->GetUserData() != NULL) {
            CCSprite *sprite = (CCSprite *) body->GetUserData();
            CCParticleSun *explosion = CCParticleSun::node();
            explosion->retain();
            explosion->setTexture(CCTextureCache::sharedTextureCache()->addImage("fire.png"));
            explosion->setAutoRemoveOnFinish(true);
            explosion->setTotalParticles(200);
            explosion->setStartSizeVar(10.0f);
            explosion->setSpeed(70.0f);
            explosion->setAnchorPoint(ccp(0.5f, 0.5f));
            explosion->setPosition(sprite->getPosition());
            explosion->setDuration(1.0f); 
            addChild(explosion, 11); 
            explosion->release();
            enemies->removeObject(sprite);
            targets->removeObject(sprite);
            this->removeChild(sprite, true);
        }
        world->DestroyBody(body);
    }
    std::vector<b2Body *>::iterator pos3;
    for(pos3 = bulletToDestroy.begin(); pos3 != bulletToDestroy.end(); ++pos3) {
        b2Body *body = *pos3;
        if (body->GetUserData() != NULL) {
            CCSprite *sprite = (CCSprite *) body->GetUserData();
            bullets->removeObject(sprite);
            this->removeChild(sprite, true);
        }
        world->DestroyBody(body);
    }
    
    
    _contactListener->_contacts.clear();

}
void HelloWorld::ccTouchesBegan(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent){
    if (mouseJoint != NULL) return;
   
    CCSetIterator it = pTouches->begin();
    CCTouch *myTouch = (CCTouch*)(*it);
    CCPoint location = myTouch->getLocationInView();
    location = CCDirector::sharedDirector()->convertToGL(location);
    b2Vec2 locationWorld = b2Vec2(location.x/PTM_RATIO,location.y/PTM_RATIO);
    if (locationWorld.x < armBody->GetWorldCenter().x+50.0/PTM_RATIO) {
        b2MouseJointDef md;
        md.bodyA = groundBody;
        md.bodyB = armBody;
        md.target = locationWorld;
        md.maxForce = 2000;
        
        mouseJoint = (b2MouseJoint*)world->CreateJoint(&md);
    }
}
void HelloWorld::ccTouchesMoved(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent){

    
    if (mouseJoint == NULL) return;
    CCSetIterator it = pTouches->begin();
    CCTouch *myTouch = (CCTouch*)(*it);
    CCPoint location = myTouch->getLocationInView();
    location = CCDirector::sharedDirector()->convertToGL(location);
    b2Vec2 locationWorld = b2Vec2(location.x/PTM_RATIO,location.y/PTM_RATIO);
    mouseJoint->SetTarget(locationWorld);
}
void HelloWorld::ccTouchesEnded(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent){
    if (mouseJoint != NULL){
        if (armJoint->GetJointAngle()>= CC_DEGREES_TO_RADIANS(20)) {
            releasingArm = true;
        }
        world->DestroyJoint(mouseJoint);
        mouseJoint = NULL;
    }
}
HelloWorld::~HelloWorld()
{
    delete bullets;
    delete enemies;
    delete targets;
    delete world;
    world = NULL;
    delete m_debugDraw;
    delete _contactListener;
    _contactListener = NULL;
}
/*void HelloWorld::draw()
{
    CCLayer::draw();
    
	ccGLEnableVertexAttribs( kCCVertexAttribFlag_Position );
    
	kmGLPushMatrix();
    
	world->DrawDebugData();
	kmGLPopMatrix();
}*/
CCScene* HelloWorld::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // add layer as a child to scene
    CCLayer* layer = new HelloWorld();
    scene->addChild(layer);
    layer->release();
    
    return scene;
}