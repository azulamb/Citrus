#ifndef __CITRUS_HEADER
#define __CITRUS_HEADER

#define CITRUS_VER "0.0.8"

#include "cocos2d.h"
#include <SimpleAudioEngine.h>

#ifndef CITRUS_VIEW_NAME
#define CITRUS_VIEW_NAME "Citrus Game"
#endif

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
	int h;
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
		y = h - touch->getLocationInView().y;
		return true;
	}

	virtual void onTouchMoved( Touch *touch, Event *event )
	{
		x = touch->getLocationInView().x;
		y = h - touch->getLocationInView().y;
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

class Citrus
{
private:
	Scene *scene;
	class CitrusGameView *now, *next;
	SpriteBatchNode **sprites;
	unsigned int texmax;
	class CitrusInput *input;
	unsigned int soundmax;
	char **bgm;
	char **se;
	float sevol;
	int *seid;
	unsigned char alpha;

public:
	Citrus()
	{
		now = NULL;
		next = NULL;
		scene = NULL;
		texmax = 4;
		sprites = (SpriteBatchNode **)calloc( texmax, sizeof( SpriteBatchNode * ) );
		input = new CitrusInputTap();
		soundmax = 5;
		bgm = (char**)calloc( soundmax, sizeof( char* ) );
		se = (char**)calloc( soundmax, sizeof( char* ) );
		seid = (int*)calloc( soundmax, sizeof( int ) );
		alpha = 255;
		sevol = 1;
		CocosDenshion::SimpleAudioEngine::getInstance()->setBackgroundMusicVolume( 1.0f );
	}

	virtual ~Citrus()
	{
		free( sprites );
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

	virtual void setScreenSize( int width, int height )
	{
		Director *director = Director::getInstance();
		GLView *glview = director->getOpenGLView();
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
	virtual int getFrame( int n = 0 )
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
		sprites[ tex ]->getTexture()->setAliasTexParameters();
		scene->addChild( sprites[ tex ] );
	}

	virtual void releaseTexture( unsigned int tex )
	{
		if ( texmax <= tex )
		{
			return;
		}
		sprites[ tex ] = NULL;
		scene->removeChild( sprites[ tex ] );
		sprites[ tex ] = NULL;
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

	virtual void setAlphaF( float a )
	{
		alpha = (unsigned char)(255 * a);
	}

	virtual void setAlpha( unsigned char a = 255 )
	{
		alpha = a;
	}

private:
	virtual Sprite * prepareTexture( int tex, int rx, int ry, int w, int h )
	{
		Rect rect;
		rect.setRect( rx, ry, w, h );
		Sprite *sprite = Sprite::createWithTexture( sprites[ tex ]->getTexture() );
		sprite->setTextureRect( rect );
		if ( alpha < 255 )
		{
			sprite->setOpacity( alpha );
		}
		return sprite;
	}
public:
	virtual void drawTexture( unsigned int tex, int rx, int ry, int w, int h, float dx, float dy )
	{
		if ( texmax <= tex || sprites[ tex ] == NULL )
		{
			return;
		}
		Sprite *sprite = prepareTexture( tex, rx, ry, w, h );
		sprite->setPosition( dx + w / 2, dy + h / 2 );
		//sprite->setAnchorPoint();
		sprites[ tex ]->addChild( sprite );
	}

	virtual void drawTextureC( unsigned int tex, int rx, int ry, int w, int h, float dx, float dy )
	{
		if ( texmax <= tex || sprites[ tex ] == NULL )
		{
			return;
		}
		Sprite *sprite = prepareTexture( tex, rx, ry, w, h );
		sprite->setPosition( dx, dy );
		sprites[ tex ]->addChild( sprite );
	}

	virtual void drawTextureScaling( unsigned int tex, int rx, int ry, int w, int h, float dx, float dy, float scale )
	{
		if ( texmax <= tex || sprites[ tex ] == NULL )
		{
			return;
		}
		Sprite *sprite = prepareTexture( tex, rx, ry, w, h );
		//sprite->setColor( &color );
		sprite->setScale( scale );
		sprite->setPosition( dx + w * scale / 2, dy + h * scale / 2 );
		sprites[ tex ]->addChild( sprite );
	}

	virtual void drawTextureScalingC( unsigned int tex, int rx, int ry, int w, int h, float dx, float dy, float scale )
	{
		if ( texmax <= tex || sprites[ tex ] == NULL )
		{
			return;
		}
		Sprite *sprite = prepareTexture( tex, rx, ry, w, h );
		sprite->setScale( scale );
		sprite->setPosition( dx, dy );
		sprites[ tex ]->addChild( sprite );
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

class  CitrusApp : private Application
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

		GLView::setGLContextAttrs( glContextAttrs );
	}

	/**
	@brief    Implement Director and Scene init code here.
	@return true    Initialize success, app continue.
	@return false   Initialize failed, app terminate.
	*/
	virtual bool applicationDidFinishLaunching()
	{
		// initialize director
		auto director = Director::getInstance();
		auto glview = director->getOpenGLView();
		if ( !glview )
		{
			glview = GLViewImpl::create( CITRUS_VIEW_NAME );
			director->setOpenGLView( glview );
		}

#ifdef _DEBUG
		// turn on display FPS
		director->setDisplayStats( true );
#endif

		// set FPS. the default value is 1.0/60 if you don't call this
		director->setAnimationInterval( 1.0 / 30 );
		Size ssize = glview->getFrameSize();
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
		Director::getInstance()->stopAnimation();
		citrus->pauseSound();
	}

	/**
	@brief  The function be called when the application enter foreground
	@param  the pointer of the application
	*/
	virtual void applicationWillEnterForeground()
	{
		Director::getInstance()->startAnimation();
		citrus->resumeSound();
	}
};


#endif
