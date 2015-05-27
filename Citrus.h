#ifndef __CITRUS_HEADER
#define __CITRUS_HEADER

#define CITRUS_VER "0.0.1"

#include "cocos2d.h"

#define CitrusObject class Citrus *citrus;
#define CitrusInit() { citrus = new Citrus(); }
#define CitrusTerm() { delete( citrus ); citrus = NULL; }

extern class Citrus *citrus;

USING_NS_CC;

class CitrusGameView : public Layer
{
public:
	CitrusGameView(){
		this->retain();
	}
	virtual void UserInit(){}
	virtual bool MainLoop(){
		return true;
	}
	virtual void CleanUp(){}
};

class Citrus
{
private:
	Scene *scene;
	class CitrusGameView *now, *next;
	class SpriteBatchNode **sprites;
	unsigned int texmax;
public:
	Citrus()
	{
		now = NULL;
		next = NULL;
		scene = NULL;
		texmax = 10;
		sprites = (SpriteBatchNode **)calloc( texmax, sizeof( SpriteBatchNode * ) );
	}

	virtual ~Citrus()
	{
		free( sprites );
	}

	virtual Scene * createScene( class CitrusGameView *gv )
	{
		scene = Scene::create();

		now = gv;
		scene->addChild( gv );
		gv->UserInit();

		return scene;
	}

	virtual bool loop()
	{
		if ( next )
		{
			if ( now )
			{
				now->CleanUp();
				scene->removeChild( now );
			}
			now = next;
			next = NULL;
			scene->addChild( now );
			now->UserInit();
		}
		if ( now->MainLoop() )
		{
			return false;
		}

		return true;
	}

	virtual void setNextGameView( class CitrusGameView *gv )
	{
		if ( next )
		{
			next->release();
		}
		next = gv;
	}

	virtual void resizeTexture( unsigned int max )
	{
		sprites = (SpriteBatchNode **)realloc( sprites, sizeof( SpriteBatchNode * ) * max );
		for ( ; texmax < max; ++texmax )
		{
			sprites[ texmax ] = NULL;
		}
	}
	virtual void createTexture( unsigned int tex, const char *file )
	{
		if ( texmax <= tex )
		{
			return;
		}
		if ( sprites[ tex ] )
		{
			releaseTexture( tex );
		}
		sprites[ tex ] = SpriteBatchNode::create( file );
		scene->addChild( sprites[ tex ] );
	}
	virtual void releaseTexture( unsigned int tex )
	{
		sprites[ tex ] = NULL;
		scene->removeChild( sprites[ tex ] );
		sprites[ tex ] = NULL;
	}

	virtual void drawTexture( unsigned int tex, int rx, int ry, int w, int h, float dx, float dy )
	{
		if ( texmax <= tex || sprites[ tex ] == NULL )
		{
			return;
		}
		Rect rect;
		rect.setRect( rx, ry, w, h );
		Sprite *sprite = Sprite::createWithTexture( sprites[ tex ]->getTexture() );
		sprite->setTextureRect( rect );
		sprite->setPosition( dx, dy );
		sprites[ tex ]->addChild( sprite );
	}
	virtual void clear()
	{
		for ( int i = 0; i < texmax; ++i )
		{
			if ( sprites[ i ] )
			{
				sprites[ i ]->removeAllChildren();
			}
		}
	}
};

class GameView : public CitrusGameView{
public:
	GameView()
	{
		this->init();
		this->autorelease();
	}
	virtual bool init()
	{
		if ( !Layer::init() )
		{
			return false;
		}

		Size visibleSize = Director::getInstance()->getVisibleSize();
		Vec2 origin = Director::getInstance()->getVisibleOrigin();

		Layer::scheduleUpdate();
		return true;
	}

	virtual void menuCloseCallback( Ref* pSender )
	{
		cocos2d::Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
		exit( 0 );
#endif
	}

	virtual void update( float delta )
	{
		citrus->loop();
	}
};


#endif
