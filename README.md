# Citrus

Citrus is Cocos->MikanUI wrapper.

for Cocos2D-x 3.x

## これなに？

Cocos2d-x 3.xで2Dゲーム作る時に一部MikanUIで使うためのラッパーライブラリ。
ヘッダファイル一つClassesに突っ込んでソースをゴニョゴニョすれば使えるようにする予定。

完全に私用なので破壊的変更諸々あり。

## 使い方

* ClassesにCitrus.hをコピー
* AppDelegate.cppをちょっと改造

```c:AppDelegate.cpp
...

#include "Citrus.h"
CitrusObject;

AppDelegate::AppDelegate()　{
　　　　CitrusInit();
}

AppDelegate::~AppDelegate()　{
　　　　CitrusTerm();
}

...

bool AppDelegate::applicationDidFinishLaunching() {

    ...

    // create a scene and GameView for Citrus.
	auto scene = citrus->createScene( new GameView() );

    // run
    director->runWithScene(scene);

    ...
}

...
````

後はGameViewクラスを継承して使えば使えます。

````c:sample.h
class HelloCitrus:public GameView
{
public:
	virtual void UserInit()
	{
		citrus->createTexture( 0, "HelloWorld.png" );
	}
	virtual bool MainLoop()
	{
		citrus->clear();
		citrus->drawTexture( 0, 0, 0, 50, 50, 10, 10 );

		citrus->drawTexture( 0, 0, 0, 50, 50, 100, 100 );
		return true;
	}
};
````

MainLoopは毎フレーム呼ばれ、CocosのSpriteなども普通に使えます。
