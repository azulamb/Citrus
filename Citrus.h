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

class CitrusInput
{
protected:
	Scene *scene;
public:
	CitrusInput()
	{
		scene = NULL;
		setScene( NULL );
	}
	virtual void setScene( Scene *scene )
	{
		this->scene = scene;
	}
	virtual void update(){}
	virtual int getX()
	{
		return 0;
	}
	virtual int getY()
	{
		return 0;
	}
	virtual int getFrame()
	{
		return 0;
	}
};

class CitrusInputTap : public CitrusInput
{
private:
	bool buf;
	int frame, x, y;
public:
	CitrusInputTap() : CitrusInput()
	{
		buf = false;
		frame = 0;
		x = y = 0;
	}

	virtual void setScene( Scene *scene )
	{
		if ( scene == NULL )
		{
			this->scene = scene;
			return;
		}
		auto listener = EventListenerTouchOneByOne::create();

		listener->onTouchBegan = CC_CALLBACK_2( CitrusInputTap::onTouchBegan, this );
		listener->onTouchMoved = CC_CALLBACK_2( CitrusInputTap::onTouchMoved, this );
		listener->onTouchEnded = CC_CALLBACK_2( CitrusInputTap::onTouchEnded, this );

		scene->getEventDispatcher()->addEventListenerWithSceneGraphPriority( listener, scene );
	}

	virtual bool onTouchBegan( Touch *touch, Event *event )
	{
		buf = true;
		x = touch->getLocationInView().x;
		y = touch->getLocationInView().y;
		return true;
	}

	virtual void onTouchMoved( Touch *touch, Event *event )
	{
		x = touch->getLocationInView().x;
		y = touch->getLocationInView().y;
	}

	virtual void onTouchEnded( Touch *touch, Event *event )
	{
		buf = false;
	}

	virtual void update()
	{
		if ( buf )
		{
			++frame;
		} else if ( 0 < frame )
		{
			frame = -1;
		} else
		{
			frame = 0;
		}
	}
	virtual int getX()
	{
		return x;
	}
	virtual int getY()
	{
		return y;
	}
	virtual int getFrame()
	{
		return frame;
	}
};

class Citrus
{
private:
	Scene *scene;
	class CitrusGameView *now, *next;
	SpriteBatchNode **sprites;
	unsigned int texmax;
	class CitrusInput *input;
public:
	Citrus()
	{
		now = NULL;
		next = NULL;
		scene = NULL;
		texmax = 4;
		sprites = (SpriteBatchNode **)calloc( texmax, sizeof( SpriteBatchNode * ) );
		input = new CitrusInputTap();
	}

	virtual ~Citrus()
	{
		free( sprites );
	}

	virtual void initInput( Scene *scene )
	{
		input->setScene( scene );
	}

	// System

	virtual Scene * createScene( class CitrusGameView *gv )
	{
		setScene( Scene::create() );

		now = gv;
		scene->addChild( gv );
		gv->UserInit();

		return scene;
	}

	virtual void setScene( Scene *s )
	{
		scene = s;
		initInput( s );
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

		if ( now->MainLoop() == false )
		{
			return false;
		}

		input->update();

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

	// Inout
	virtual int getX()
	{
		return input->getX();
	}
	virtual int getY()
	{
		return input->getY();
	}
	virtual int getFrame()
	{
		return input->getFrame();
	}

	// Draw

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
		sprite->setPosition( dx + w / 2, dy + h / 2 );
		sprites[ tex ]->addChild( sprite );
	}
	virtual void clear()
	{
		for ( unsigned int i = 0; i < texmax; ++i )
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
