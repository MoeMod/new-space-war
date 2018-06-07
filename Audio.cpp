#include "SimpleAudioEngine.h"

using CocosDenshion::SimpleAudioEngine;
SimpleAudioEngine *audioengine = nullptr;

void Audio_Init()
{
	//³õÊ¼»¯±³¾°ÒôÀÖ
	audioengine = SimpleAudioEngine::getInstance();
	audioengine->preloadBackgroundMusic("bgm.mp3");
}

void Audio_PlayBgm()
{
	audioengine->playBackgroundMusic("bgm.mp3", true);
}

void Audio_StopBgm()
{
	audioengine->stopBackgroundMusic();
}