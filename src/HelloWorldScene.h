#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

#include "Box2D/Box2D.h"

using namespace cocos2d;

class HelloWorld : public cocos2d::CCLayer
{
public:
	// �������� ���� Box2D, ����������� ����������� �������� �� ����, ����������� ���� � TouchDispatcher'�
    virtual bool init(); 

    static cocos2d::CCScene* scene();
    
    // a selector callback
    void menuCloseCallback(CCObject* pSender);

	// ���������� ���� Box2D, ���������� ��������� ��������� � ��� �������� ������ �����
	void Tick(float dt);

	bool ccTouchBegan(CCTouch* pTouch, CCEvent* event);
	void ccTouchMoved(CCTouch* pTouch, CCEvent* event);
	void ccTouchEnded(CCTouch* pTouch, CCEvent* event);

    // implement the "static node()" method manually
    CREATE_FUNC(HelloWorld);

	// �������� ���� Box2D, ��������� ����������
	~HelloWorld();
private:
	int score;
		// ��� ��� ��������� ���� Box2D
	b2World* _world;
	b2Body* _edgesBody;			// ������� ������, �� ������� ��� �� ������ ��������
	b2Body* _basketBody;		// ������� - ���� ��� ����
	b2Body* _ballBody;			// ��� - ������������ ����������� ������������� ������
	b2MouseJoint* mouseJoint;	// �������� � ���� ��� ���������� ����� (����������� ����)
	//b2WeldJoint* weldJoint;
	b2Vec2 prevLocationWorld;	// ��������� ���� �� ������ �����������. ��� ������� ����.
};

#endif  // __HELLOWORLD_SCENE_H__