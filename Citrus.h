#ifndef __CITRUS_HEADER
#define __CITRUS_HEADER

#define CITRUS_VER "0.0.9"

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

class SpriteList
{
private:
	int now, max;
	Sprite **sprite;
public:
	SpriteList()
	{
		max = 1;
		reset();
		sprite = (Sprite **)calloc( max, sizeof( Sprite * ) );
	}
	virtual void reset()
	{
		now = 0;
	}
	virtual void cut( SpriteBatchNode *batch )
	{
		for ( ; now < max; ++now)
		{
			if ( sprite[ now ] )
			{
				batch->removeChild( sprite[ now ], true );
				sprite[ now ] = NULL;
			}
		}
	}
	virtual Sprite *get( SpriteBatchNode *batch )
	{
		if ( max <= now || sprite[ now ] == NULL)
		{
			if (max <= now )
			{
				++max;
				sprite = (Sprite **)realloc( sprite, max * sizeof( Sprite * ) );
			}
			sprite[ now ] = Sprite::createWithTexture( batch->getTexture() );
			batch->addChild( sprite[ now ] );
		}
		return sprite[ now++ ];
	}
};

class CitrusTexture
{
private:
	SpriteBatchNode *batch;
	SpriteList list;
	int alpha;
public:
	CitrusTexture()
	{
		batch = NULL;
	}
	virtual void createTexture( Scene *scene, unsigned int tex, const char *file )
	{
		batch = SpriteBatchNode::create( file );
		batch->getTexture()->setAliasTexParameters();
		scene->addChild( batch );
	}
private:
	virtual Sprite * prepareTexture( int rx, int ry, int w, int h )
	{
		Rect rect;
		rect.setRect( rx, ry, w, h );
		Sprite *sprite = list.get( batch );//Sprite::createWithTexture( batch->getTexture() );
		sprite->setTextureRect( rect );
		if ( alpha < 255 )
		{
			sprite->setOpacity( alpha );
		}
		return sprite;
	}
public:
	virtual void releaseTexture( Scene *scene, unsigned int tex )
	{
		scene->removeChild( batch );
	}

	virtual void clear()
	{
		//batch->removeAllChildren();
		list.reset();
	}

	virtual void after()
	{
		list.cut( batch );
	}

	virtual void drawTexture( int rx, int ry, int w, int h, float dx, float dy )
	{
		Sprite *sprite = prepareTexture( rx, ry, w, h );
		sprite->setPosition( dx + w / 2, dy + h / 2 );
		//sprite->setAnchorPoint();
	}

	virtual void drawTextureC( int rx, int ry, int w, int h, float dx, float dy )
	{
		Sprite *sprite = prepareTexture( rx, ry, w, h );
		sprite->setPosition( dx, dy );
	}

	virtual void drawTextureScaling( int rx, int ry, int w, int h, float dx, float dy, float scale )
	{
		Sprite *sprite = prepareTexture( rx, ry, w, h );
		//sprite->setColor( &color );
		sprite->setScale( scale );
		sprite->setPosition( dx + w * scale / 2, dy + h * scale / 2 );
	}

	virtual void drawTextureScaling( int rx, int ry, int w, int h, float dx, float dy, float dw, float dh )
	{
		Sprite *sprite = prepareTexture( rx, ry, w, h );
		sprite->setScale( dw / w, dh / h );
		sprite->setPosition( dx + (int)( dw / 2 ), dy + (int)( dh / 2 ) );
	}

	virtual void drawTextureScalingC( int rx, int ry, int w, int h, float dx, float dy, float scale )
	{
		Sprite *sprite = prepareTexture( rx, ry, w, h );
		sprite->setScale( scale );
		sprite->setPosition( dx, dy );
	}
};

class Citrus
{
private:
	Scene *scene;
	class CitrusGameView *now, *next;
	CitrusTexture **texs;
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
		texs = (CitrusTexture **)calloc( texmax, sizeof( CitrusTexture * ) );
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

	virtual void setAlphaF( float a )
	{
		alpha = (unsigned char)(255 * a);
	}

	virtual void setAlpha( unsigned char a = 255 )
	{
		alpha = a;
	}


public:
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
