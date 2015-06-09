# Citrus

Citrus is Cocos->MikanUI wrapper.

for Cocos2D-x 3.x

## これなに？

Cocos2d-x 3.xで2Dゲーム作る時に一部MikanUIで使うためのラッパーライブラリ。
ヘッダファイル一つClassesに突っ込んでソースをゴニョゴニョすれば使えるようにする予定。

完全に私用なので破壊的変更諸々あり。

CocosとMikanのいいとこ取りしたい。

## 使い方

* ClassesにCitrus.hをコピー
* AppDelegate.h、cppをちょっと改造

```c:AppDelegate.h
#ifndef  _APP_DELEGATE_H_
#define  _APP_DELEGATE_H_

#include "Citrus.h"

class  AppDelegate : private CitrusApp
{
public:
	virtual class GameView * init();
};

#endif // _APP_DELEGATE_H_
```

```c:AppDelegate.cpp
#include "AppDelegate.h"
#include "HelloWorldScene.h"

CitrusObject;

class GameView * AppDelegate::init() {
	return new HelloCitrus();
}
````

後はGameViewクラスを継承して使えば使えます。

```c:HelloWorldScene.h
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
```

MainLoopは毎フレーム呼ばれ、CocosのSpriteなども普通に使えます。

## 音周り

次のような想定で使う場合、Citrusで簡易振り分けができます。

* Androidでogg、iOSでcaf、Winその他でwavと使い分ける。
* 拡張子以外のファイル名は同じ

virtual void UserInit()
{
	// ファイル名だけの場合、プラットフォームごとにbgm.oggとbgm.cafとbgm.wavを切り替える。
	citrus->loadBgm( 0,"bgm" );
	citrus->playBgm( 0, true );
}

なおAndroidの無駄なファイルコピーは、以下の様な設定で回避できます。

* proj.android/build-cfg.json
    * copy_resourcesにexcludeの設定を追加する。

もしAndroidではogg以外再生しない場合は次のようにします。

```json:proj.android/build-cfg.json
{
    "ndk_module_path" :[
        "../cocos2d",
        "../cocos2d/cocos",
        "../cocos2d/external"
    ],
    "copy_resources": [
        {
            "from": "../Resources",
            "to": "",
            "exclude": [
                "*.caf", "*.wav"
            ]
        }
    ]
}
```
