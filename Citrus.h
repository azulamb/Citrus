#ifndef __CITRUS_HEADER
#define __CITRUS_HEADER

#define CITRUS_VER "0.0.1"

#include "cocos2d.h"
#include <stdlib.h>

#define CitrusObject class Citrus *citrus;
#define CitrusInit(v) { citrus = new Citrus( v ); }
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
	class CitrusGameView *now, *next;
	Texture2D **texs;
	unsigned int texmax;
public:
	Citrus( class CitrusGameView *gv )
	{
		now = NULL;
		next = gv;
		texmax = 10;
		texs = (Texture2D **)calloc( texmax, sizeof( Texture2D * ) );
	}

	virtual ~Citrus()
	{
	}

	virtual Scene * createScene()
	{
		Scene *scene = Scene::create();

		scene->addChild( next );

		return scene;
	}

	virtual bool loop()
	{
		if ( next )
		{
			if ( now )
			{
				now->CleanUp();
			}
			//delete( now );
			now = next;
			next = NULL;
			now->UserInit();
		}

		if ( now->MainLoop() )
		{
			return false;
		}

		return true;
	}

	void resizeTexture( unsigned int max )
	{
		texs = (Texture2D **)realloc( texs, sizeof( Texture2D * ) * max );
		for ( ; texmax < max; ++texmax )
		{
			texs[ texmax ] = NULL;
		}
	}
	void createTexture( unsigned int tex, const char *file )
	{
		if ( texmax <= tex )
		{
			return;
		}
		if ( texs[ tex ] )
		{
			releaseTexture( tex );
		}

		texs[ tex ] = Director::getInstance()->getTextureCache()->addImage( file );
	}
	void releaseTexture( unsigned int tex )
	{
		texs[ tex ]->release();
		texs[ tex ] = NULL;
	}

	void drawTexture( unsigned int tex, int rx, int ry, int w, int h, float dx, float dy )
	{
		if ( texmax <= tex || texs[ tex ] == NULL )
		{
			return;
		}
		class Sprite *sprite = Sprite::createWithTexture( texs[ tex ] );
		sprite->setPosition( dx, dy );
		Rect rect;// = new Rect( rx, ry, w, h );
		rect.setRect( rx, ry, w, h );
		//Rect::Rect( rx, ry, w, h );
		sprite->setTextureRect( rect );//Rect::Rect( rx, ry, w, h )
		now->addChild( sprite );
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
