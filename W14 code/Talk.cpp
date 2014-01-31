#include "StdAfx.h"
#include "Talk.h"


CTalk::CTalk(void)
{
	//発話関連の初期化
	talkData = AquesTalkDa_Create();
}


CTalk::~CTalk(void)
{
	// 発話関連の解放
	AquesTalkDa_Release(talkData);
}


void CTalk::TalkLanguage(int talk_number)
{
	if(AquesTalkDa_IsPlay(talkData) == 1) {
		AquesTalkDa_Stop(talkData);
	}

	switch(talk_number) {
		// 最初の挨拶
	case TALK_HELLO:
		AquesTalkDa_Play(talkData, "こん+にちわ。", 100, NULL, 0, 0);
		break;
		// 追跡開始
	case TALK_START:
		AquesTalkDa_Play(talkData, "あな'たに/つ'いて+いきま'す。", 100, NULL, 0, 0);
		break;
		// 人とロボットの距離が近いとき
	case TALK_NEAR:
		AquesTalkDa_Play(talkData, "ちか'いです。", 100, NULL, 0, 0);
		break;
		// 追跡する人が早く歩いたとき
	case TALK_QUICK:
		AquesTalkDa_Play(talkData, "はやい/で'す、ゆっ'くり'/あるいて/くださ'い。", 100, NULL, 0, 0);
		break;
		// 見失ったとき
	case TALK_LOST:
		AquesTalkDa_Play(talkData, "あれ、ど'こえ/いっ'たの？", 100, NULL, 0, 0);
		break;
		// 手を振ったとき（終了の合図）
	case TALK_BYE:
		AquesTalkDa_Play(talkData, "さよーなら、また,よんで/くだ+さい。", 100, NULL, 0, 0);
		break;
		// 見失った後に再発見したとき
	case TALK_REDIS:
		AquesTalkDa_Play(talkData, "さいはっけん。", 100, NULL, 0, 0);
		break;
		//緊急停止
	case TALK_EMERGENCY:
		AquesTalkDa_Play(talkData, "しょうがいぶつあり。", 100, NULL, 0, 0);
		break;
	default:
		break;
	}
}