#ifndef __CITRUS_HEADER
#define __CITRUS_HEADER

#define CITRUS_VER "0.1.5"

// I want to delete.
#define DISABLE_REUSE_SPRITE

#include "cocos2d.h"
#include <SimpleAudioEngine.h>

#ifndef CITRUS_VIEW_NAME
#define CITRUS_VIEW_NAME "Citrus Game"
#endif

#define CitrusObject class Citrus *citrus;
#define CitrusInit() { citrus = new Citrus(); }
#define CitrusTerm() { delete( citrus ); citrus = NULL; }

extern class Citrus *citrus;

class CitrusGameView : public cocos2d::Layer
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
	cocos2d::Scene *scene;
	int h;
public:
	CitrusInput()
	{
		scene = NULL;
		setScene( NULL );
	}
	virtual void setScene( cocos2d::Scene *scene )
	{
		this->scene = scene;
	}
	virtual void setScreenSize( int w,int h )
	{
		this->h = h;
	}
	virtual void update(){}
	virtual int getX( int n = 0 )
	{
		return 0;
	}
	virtual int getY( int n = 0 )
	{
		return 0;
	}
	virtual int getFrame( int n = 0 )
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

	virtual void setScene( cocos2d::Scene *scene )
	{
		if ( scene == NULL )
		{
			this->scene = scene;
			return;
		}
		auto listener = cocos2d::EventListenerTouchOneByOne::create();

		listener->onTouchBegan = CC_CALLBACK_2( CitrusInputTap::onTouchBegan, this );
		listener->onTouchMoved = CC_CALLBACK_2( CitrusInputTap::onTouchMoved, this );
		listener->onTouchEnded = CC_CALLBACK_2( CitrusInputTap::onTouchEnded, this );

		scene->getEventDispatcher()->addEventListenerWithSceneGraphPriority( listener, scene );
	}

	virtual bool onTouchBegan( cocos2d::Touch *touch, cocos2d::Event *event )
	{
		buf = true;
		x = touch->getLocationInView().x;
		y = h - touch->getLocationInView().y;
		return true;
	}

	virtual void onTouchMoved( cocos2d::Touch *touch, cocos2d::Event *event )
	{
		x = touch->getLocationInView().x;
		y = h - touch->getLocationInView().y;
	}

	virtual void onTouchEnded( cocos2d::Touch *touch, cocos2d::Event *event )
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
	virtual int getX( int n = 0 )
	{
		return x;
	}
	virtual int getY( int n = 0 )
	{
		return y;
	}
	virtual int getFrame( int n = 0 )
	{
		return frame;
	}
};

class SpriteList
{
private:
	int now, max;
	cocos2d::Sprite **sprite;
public:
	SpriteList()
	{
		max = 1;
		sprite = ( cocos2d::Sprite ** )calloc( max, sizeof( cocos2d::Sprite * ) );
		reset();
	}
	virtual ~SpriteList()
	{
		free( sprite );
	}
	virtual void reset()
	{
		cocos2d::Rect rect;
		rect.setRect( 0, 0, 0, 0 );
		for ( now = 0 ; now < max; ++now )
		{
			if ( sprite[ now ] )
			{
				sprite[ now ]->setPosition( 0, 0 );
				sprite[ now ]->setTextureRect( rect );
			}
		}
		now = 0;
	}
	virtual void cut( cocos2d::SpriteBatchNode *batch )
	{
		cocos2d::Rect rect;
		rect.setRect( 0, 0, 0, 0 );
		for ( ; now < max; ++now )
		{
			if ( sprite[ now ] )
			{
				sprite[ now ]->setPosition( 0, 0 );
				sprite[ now ]->setTextureRect( rect );
				//batch->removeChild( sprite[ now ], true );
				//sprite[ now ] = NULL;
			}
		}
	}
	virtual cocos2d::Sprite *get( cocos2d::SpriteBatchNode *batch )
	{
		if ( max <= now || sprite[ now ] == NULL)
		{
			if (max <= now )
			{
				++max;
				sprite = ( cocos2d::Sprite ** )realloc( sprite, max * sizeof( cocos2d::Sprite * ) );
			}
			sprite[ now ] = cocos2d::Sprite::createWithTexture( batch->getTexture() );
			batch->addChild( sprite[ now ] );
		}
		return sprite[ now++ ];
	}
};

class CitrusTexture
{
private:
	cocos2d::SpriteBatchNode *batch;
	SpriteList list;
	GLubyte a;
public:
	CitrusTexture()
	{
		batch = NULL;
		setAlpha();
	}
	virtual void createTexture( cocos2d::Scene *scene, unsigned int tex, const char *file )
	{
		batch = cocos2d::SpriteBatchNode::create( file );
		batch->getTexture()->setAliasTexParameters();
		scene->addChild( batch );
	}
private:
	virtual cocos2d::Sprite * prepareTexture( int rx, int ry, int w, int h )
	{
		cocos2d::Rect rect;
		rect.setRect( rx, ry, w, h );
		cocos2d::Sprite *sprite;
#ifdef DISABLE_REUSE_SPRITE
		sprite = cocos2d::Sprite::createWithTexture( batch->getTexture() );
		batch->addChild( sprite );
#else
		sprite = list.get( batch );
#endif
		sprite->setTextureRect( rect );
		if ( a < 255 )
		{
			sprite->setOpacity( a );
		}
		return sprite;
	}
public:
	virtual void releaseTexture( cocos2d::Scene *scene, unsigned int tex )
	{
		scene->removeChild( batch );
	}

	virtual void clear()
	{
#ifdef DISABLE_REUSE_SPRITE
		batch->removeAllChildren();
#else
		list.reset();
#endif
	}

	virtual void after()
	{
		list.cut( batch );
	}

	virtual void setAlpha( GLbyte a = 0xFF )
	{
		this->a = a;
	}

	virtual void drawTexture( int rx, int ry, int w, int h, float dx, float dy )
	{
		cocos2d::Sprite *sprite = prepareTexture( rx, ry, w, h );
		sprite->setPosition( dx, dy );
		sprite->setAnchorPoint( cocos2d::ccp( 0, 0 ) );
	}

	virtual void drawTextureC( int rx, int ry, int w, int h, float dx, float dy )
	{
		cocos2d::Sprite *sprite = prepareTexture( rx, ry, w, h );
		sprite->setPosition( dx, dy );
	}

	virtual void drawTextureScaling( int rx, int ry, int w, int h, float dx, float dy, float scale )
	{
		cocos2d::Sprite *sprite = prepareTexture( rx, ry, w, h );
		//sprite->setColor( &color );
		sprite->setScale( scale );
		sprite->setPosition( dx, dy );
		sprite->setAnchorPoint( cocos2d::ccp( 0, 0 ) );
	}

	virtual void drawTextureScaling( int rx, int ry, int w, int h, float dx, float dy, float dw, float dh )
	{
		cocos2d::Sprite *sprite = prepareTexture( rx, ry, w, h );
		sprite->setScale( dw / w, dh / h );
		sprite->setPosition( dx, dy );
		sprite->setAnchorPoint( cocos2d::ccp( 0, 0 ) );
	}

	virtual void drawTextureScalingC( int rx, int ry, int w, int h, float dx, float dy, float scale )
	{
		cocos2d::Sprite *sprite = prepareTexture( rx, ry, w, h );
		sprite->setScale( scale );
		sprite->setPosition( dx, dy );
	}

	virtual void drawTextureScaleRotateC( int rx, int ry, int w, int h, float dx, float dy, float scale, float rad )
	{
		cocos2d::Sprite *sprite = prepareTexture( rx, ry, w, h );
		sprite->setScale( scale );
		sprite->setPosition( dx, dy );
		sprite->setRotation( rad );
	}
};

class Citrus
{
private:
	cocos2d::Scene *scene;
	class CitrusGameView *now, *next;
	CitrusTexture **texs;
	unsigned int texmax;
	class CitrusInput *input;
	unsigned int soundmax;
	char **bgm;
	char **se;
	float sevol;
	int *seid;

public:
	Citrus()
	{
		now = NULL;
		next = NULL;
		scene = NULL;
		texmax = 4;
		texs = (CitrusTexture **)calloc( texmax, sizeof( CitrusTexture * ) );
		input = new CitrusInputTap();
		soundmax = 5;
		bgm = (char**)calloc( soundmax, sizeof( char* ) );
		se = (char**)calloc( soundmax, sizeof( char* ) );
		seid = (int*)calloc( soundmax, sizeof( int ) );
		sevol = 1;
		CocosDenshion::SimpleAudioEngine::getInstance()->setBackgroundMusicVolume( 1.0f );
	}

	virtual ~Citrus()
	{
		free( texs );
		for ( ; 0 < soundmax; --soundmax )
		{
			if ( bgm[ soundmax - 1 ] )
			{
				free( bgm[ soundmax - 1 ] );
			}
			if ( se[ soundmax - 1 ] )
			{
				free( se[ soundmax - 1 ] );
			}
		}
		free( bgm );
		free( se );
		free( seid );
	}

	virtual void initInput( cocos2d::Scene *scene )
	{
		input->setScene( scene );
	}

	// System

	virtual cocos2d::Scene * createScene( class CitrusGameView *gv )
	{
		setScene( cocos2d::Scene::create() );

		now = gv;
		scene->addChild( gv );
		gv->UserInit();

		return scene;
	}

	virtual void setScene( cocos2d::Scene *s )
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

		after();
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

	virtual void setScreenSize( int width, int height )
	{
		cocos2d::Director *director = cocos2d::Director::getInstance();
		cocos2d::GLView *glview = director->getOpenGLView();
		glview->setDesignResolutionSize( width, height, ResolutionPolicy::SHOW_ALL );//kResolutionShowAll );
		input->setScreenSize( width, height );
	}

	// Inout
	virtual int getX( int n = 0 )
	{
		return input->getX( n );
	}
	virtual int getY( int n = 0 )
	{
		return input->getY( n );
	}
	virtual int getTouchFrame( int n = 0 )
	{
		return input->getFrame( n );
	}

	// Sound
private:
	char *ensureSoundFileName( const char *file )
	{
		int len = strlen( file );
		char ext[ 5 ] = "";
		if ( len < 4 || file[ len - 4 ] != '.' )
		{
			len += 4;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
			strcpy( ext, ".caf" );
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
			strcpy( ext, ".ogg" );
#else
			strcpy( ext, ".wav" );
#endif
		}
		char *ret = (char *)calloc( len + 1, sizeof( char ) );
		sprintf( ret, "%s%s", file, ext );
		return ret;
	}
public:
	virtual void setVolume( float vol )
	{
		CocosDenshion::SimpleAudioEngine::getInstance()->setBackgroundMusicVolume( vol );
		CocosDenshion::SimpleAudioEngine::getInstance()->setEffectsVolume( vol );
	}
	virtual void setVolume( float bgm, float se )
	{
		CocosDenshion::SimpleAudioEngine::getInstance()->setBackgroundMusicVolume( bgm );
		CocosDenshion::SimpleAudioEngine::getInstance()->setEffectsVolume( se );
		sevol = se;
	}

	virtual void loadBgm( unsigned int snd, const char *file )
	{
		if ( soundmax <= snd )
		{
			return;
		}
		if ( bgm[ snd ] )
		{
			releaseBgm( snd );
		}
		bgm[ snd ] = ensureSoundFileName( file );
		CocosDenshion::SimpleAudioEngine::getInstance()->preloadBackgroundMusic( bgm[ snd ] );
	}

	virtual void releaseBgm( unsigned int snd )
	{
		if ( soundmax <= snd )
		{
			return;
		}
		free( bgm[ snd ] );
		bgm[ snd ] = NULL;
	}

	virtual void playBgm( unsigned int snd, bool loop = false )
	{
		if ( soundmax <= snd )
		{
			return;
		}
		CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic( bgm[ snd ], loop );
	}

	virtual void stopBgm( unsigned int snd )
	{
		if ( soundmax <= snd )
		{
			return;
		}
		CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
	}


	virtual void loadSe( unsigned int snd, const char *file )
	{
		if ( soundmax <= snd )
		{
			return;
		}
		if ( se[ snd ] )
		{
			releaseSe( snd );
		}
		se[ snd ] = ensureSoundFileName( file );
		CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect( se[ snd ] );
	}

	virtual void releaseSe( unsigned int snd )
	{
		if ( soundmax <= snd )
		{
			return;
		}
		free( se[ snd ] );
		seid[ snd ] = 0;
		se[ snd ] = NULL;
	}

	virtual void playSe( unsigned int snd, bool loop = false )
	{
		if ( soundmax <= snd )
		{
			return;
		}
		seid[ snd ] = CocosDenshion::SimpleAudioEngine::getInstance()->playEffect( se[ snd ], loop, 1, 0, sevol );
	}

	virtual void stopSe( unsigned int snd )
	{
		if ( soundmax <= snd )
		{
			return;
		}
		CocosDenshion::SimpleAudioEngine::getInstance()->stopEffect( seid[ snd ] );
	}

	virtual void pauseSound( void )
	{
		CocosDenshion::SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
	}

	virtual void resumeSound( void )
	{
		CocosDenshion::SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
	}

	// Draw

	virtual void resizeTexture( unsigned int max )
	{
		texs = (CitrusTexture **)realloc( texs, sizeof( CitrusTexture * ) * max );
		for ( ; texmax < max; ++texmax )
		{
			texs[ texmax ] = NULL;
		}
	}

	virtual void createTexture( unsigned int tex, const char *file )
	{
		if ( texmax <= tex )
		{
			return;
		}
		if ( texs[ tex ] )
		{
			releaseTexture( tex );
		}
		texs[ tex ] = new CitrusTexture();
		texs[ tex ]->createTexture( scene, tex, file );
	}

	virtual void releaseTexture( unsigned int tex )
	{
		if ( texmax <= tex )
		{
			return;
		}
		texs[ tex ]->releaseTexture( scene, tex );
		delete( texs[ tex ] );
		texs[ tex ] = NULL;
	}

	virtual void clear()
	{
		for ( unsigned int i = 0; i < texmax; ++i )
		{
			if ( texs[ i ] )
			{
				texs[ i ]->clear();
			}
		}
	}

private:
	virtual void after()
	{
		for ( unsigned int i = 0; i < texmax; ++i )
		{
			if ( texs[ i ] )
			{
				texs[ i ]->after();
			}
		}
	}
public:

	virtual void setAlphaF( unsigned int tex, float a )
	{
		if ( texmax <= tex || texs[ tex ] == NULL )
		{
			return;
		}
		texs[ tex ]->setAlpha( (unsigned char)( 255 * a ) );
	}

	virtual void setAlpha( unsigned int tex, unsigned char a = 255 )
	{
		if ( texmax <= tex || texs[ tex ] == NULL )
		{
			return;
		}
		texs[ tex ]->setAlpha( a );
	}

	virtual void drawTexture( unsigned int tex, int rx, int ry, int w, int h, float dx, float dy )
	{
		if ( texmax <= tex || texs[ tex ] == NULL )
		{
			return;
		}
		texs[ tex ]->drawTexture( rx, ry, w, h, dx, dy );
	}

	virtual void drawTextureC( unsigned int tex, int rx, int ry, int w, int h, float dx, float dy )
	{
		if ( texmax <= tex || texs[ tex ] == NULL )
		{
			return;
		}
		texs[ tex ]->drawTextureC( rx, ry, w, h, dx, dy );
	}

	virtual void drawTextureScaling( unsigned int tex, int rx, int ry, int w, int h, float dx, float dy, float scale )
	{
		if ( texmax <= tex || texs[ tex ] == NULL )
		{
			return;
		}
		texs[ tex ]->drawTextureScaling( rx, ry, w, h, dx, dy, scale );
	}

	virtual void drawTextureScaling( unsigned int tex, int rx, int ry, int w, int h, float dx, float dy, float dw, float dh )
	{
		if ( texmax <= tex || texs[ tex ] == NULL )
		{
			return;
		}
		texs[ tex ]->drawTextureScaling( rx, ry, w, h, dx, dy, dw, dh );
	}

	virtual void drawTextureScalingC( unsigned int tex, int rx, int ry, int w, int h, float dx, float dy, float scale )
	{
		if ( texmax <= tex || texs[ tex ] == NULL )
		{
			return;
		}
		texs[ tex ]->drawTextureScalingC( rx, ry, w, h, dx, dy, scale );
	}

	virtual void drawTextureScaleRotateC( unsigned int tex, int rx, int ry, int w, int h, float dx, float dy, float scale, float rad )
	{
		if ( texmax <= tex || texs[ tex ] == NULL )
		{
			return;
		}
		texs[ tex ]->drawTextureScaleRotateC( rx, ry, w, h, dx, dy, scale, rad );
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

		cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
		cocos2d::Vec2 origin = cocos2d::Director::getInstance()->getVisibleOrigin();

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

class  CitrusApp : private cocos2d::Application
{
public:
	CitrusApp()
	{
		CitrusInit();
	}

	virtual ~CitrusApp()
	{
		CitrusTerm();
	}

	virtual class GameView * init() = 0;

	virtual void initGLContextAttrs()
	{
		GLContextAttrs glContextAttrs = { 8, 8, 8, 8, 24, 8 };

		cocos2d::GLView::setGLContextAttrs( glContextAttrs );
	}

	/**
	@brief    Implement Director and Scene init code here.
	@return true    Initialize success, app continue.
	@return false   Initialize failed, app terminate.
	*/
	virtual bool applicationDidFinishLaunching()
	{
		// initialize director
		auto director = cocos2d::Director::getInstance();
		auto glview = director->getOpenGLView();
		if ( !glview )
		{
			glview = cocos2d::GLViewImpl::create( CITRUS_VIEW_NAME );
			director->setOpenGLView( glview );
		}

#ifdef _DEBUG
		// turn on display FPS
		director->setDisplayStats( true );
#endif

		// set FPS. the default value is 1.0/60 if you don't call this
		director->setAnimationInterval( 1.0 / 30 );
		cocos2d::Size ssize = glview->getFrameSize();
		citrus->setScreenSize( ssize.width, ssize.height );

		GameView *gv = init();

		// create a scene. it's an autorelease object
		auto scene = citrus->createScene( gv );

		// run
		director->runWithScene( scene );

		return true;
	}

	/**
	@brief  The function be called when the application enter background
	@param  the pointer of the application
	*/
	virtual void applicationDidEnterBackground()
	{
		cocos2d::Director::getInstance()->stopAnimation();
		citrus->pauseSound();
	}

	/**
	@brief  The function be called when the application enter foreground
	@param  the pointer of the application
	*/
	virtual void applicationWillEnterForeground()
	{
		cocos2d::Director::getInstance()->startAnimation();
		citrus->resumeSound();
	}
};


#endif
