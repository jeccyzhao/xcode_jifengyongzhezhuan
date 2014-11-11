#include "CAnimation.h"
#include "CEventFrame.h"
#include "support/data_support/uthash.h"
using namespace std;

CCArmatureAnimation *CCArmatureAnimation::create() {
	CCArmatureAnimation *pARmatureAnimation = new CCArmatureAnimation();
	if (pARmatureAnimation && pARmatureAnimation->init()) {
		pARmatureAnimation->autorelease();
		return pARmatureAnimation;
	}
	CC_SAFE_DELETE(pARmatureAnimation);
	return NULL;
}

CCArmatureAnimation::CCArmatureAnimation()
{
	mEventListener		= NULL;
	mArmatureAniData	= NULL;
	mBoneAniData		= NULL;
    m_pAnimArmature     = NULL;
	mIsPauseOnOver = false;
	mIsSingleAnimationOver = false;
}

CCArmatureAnimation::~CCArmatureAnimation(void)
{
	CC_SAFE_DELETE( mTweens );
}

bool CCArmatureAnimation::init()
{
	bool bRet = false;
	do {

		mAniIDNow = "";
		mAnimFrame = "";

		mTweens		= new CCDictionary();
		CCAssert(mTweens, "create CCArmatureAnimation::mTweens fail!");

		bRet = true;
	} while (0);

	return bRet;
}

void CCArmatureAnimation::remove() 
{
	stop();
	CCProcessBase::remove();

	CCDictElement *_element = NULL;
	CCDICT_FOREACH(mTweens, _element) {
		CCTween* _tween = (CCTween*)_element->getObject();
		_tween->remove();
	}

	mArmatureAniData = NULL;
	mBoneAniData = NULL;
	mAniIDNow = "";
	mAnimFrame = "";

	mTweens->removeAllObjects();
}

void CCArmatureAnimation:: pause()
{
	CCProcessBase::pause();

	CCDictElement *_element = NULL;
	CCDICT_FOREACH(mTweens, _element) {
		CCTween* _tween = (CCTween*)_element->getObject();
		_tween->pause();
	}

}

void CCArmatureAnimation::resume() 
{
	CCProcessBase::resume();

	CCDictElement *_element = NULL;
	CCDICT_FOREACH(mTweens, _element) {
		CCTween* _tween = (CCTween*)_element->getObject();
		_tween->resume();
	}

}

void CCArmatureAnimation::stop()
{
	CCProcessBase::stop();

	CCDictElement *_element = NULL;
	CCDICT_FOREACH(mTweens, _element) {
		CCTween* _tween = (CCTween*)_element->getObject();
		_tween->stop();
	}

}

CCArmatureAniData* CCArmatureAnimation::getData()
{
	return mArmatureAniData;
}

void CCArmatureAnimation::setData(CCArmatureAniData* _aniData)
{
	remove();
	mArmatureAniData = _aniData;
}

void CCArmatureAnimation::setAnimationScale(float _scale, const char* _boneName )
{
	CCTween* _tween;
	if (_boneName != "") {
		_tween = (CCTween*)mTweens->objectForKey(_boneName);

		if (_tween) {
			_tween->setScale(_scale);
		}
	} else {
		mScale = _scale;

		CCDictElement *_element = NULL;
		CCDICT_FOREACH(mTweens, _element) {
			CCTween* _tween = (CCTween*)_element->getObject();
			_tween->setScale(_scale);
		}

	}
}

void CCArmatureAnimation::addTween(CCBone* _bone) 
{
	const char* _boneID = _bone->getName();
	CCTween		*_tween = (CCTween*)mTweens->objectForKey(_boneID);	
	if (!_tween) {
		_tween = CCTween::create();
		mTweens->setObject(_tween, _boneID);	
	}
	else
	{
		CCLOG("Bone %s Already Exist",_boneID);
		_tween->resetValue();
	}
	// Note: 无法修改mTween中的值
	_bone->setTween(_tween);
	_tween->setNode(_bone->getTweenNode());
}

void CCArmatureAnimation::removeTween(CCBone* _bone)
{
	const char* _boneID = _bone->getName();
	CCTween		*_tween = (CCTween*)mTweens->objectForKey(_boneID);
	if (_tween) {
		_tween->remove();
	}

}

CCTween* CCArmatureAnimation::getTween(const char* _boneID) 
{
	return (CCTween*)mTweens->objectForKey(_boneID);
}

void CCArmatureAnimation::updateTween(CCTween *_tween, float dt)
{
	if (_tween) {
		_tween->update( dt );
	}
}

void CCArmatureAnimation::updateTween(const char *_boneID, float dt)
{
	CCTween* _tween = NULL;
	CCDictElement *pElement = NULL;
	HASH_FIND_STR(mTweens->m_pElements, _boneID, pElement);
	if (pElement != NULL)
	{
		_tween = (CCTween*)pElement->getObject();
	}

	if (_tween) {
		_tween->update( dt );
	}

}

void CCArmatureAnimation::playTo(void * _to, int _durationTo /* = -1 */, int _durationTween /* = -1 */, 
	bool _loop /* = true */, int _ease /* = -2 */) 
{
	mIsPauseOnOver = false;
	mIsSingleAnimationOver = false;

	//CCXLOG("Enter CCArmatureAnimation::playTo");
	if (!mArmatureAniData) {
		return;
	}
	mBoneAniData = mArmatureAniData->getAnimation( (const char *)_to );
	CCAssert(mBoneAniData, "mBoneAniData can not be null");

	mAniIDNow		= (const char *)_to;

	_durationTo		= _durationTo<0 ? mBoneAniData->getDurationTo() : _durationTo;
	//_durationTween	= _durationTween<0 ? mBoneAniData->getDurationTween() : _durationTween;
	//_durationTween	= _durationTween<0 ? mBoneAniData->getDuration() : _durationTween;
	_durationTween	= mBoneAniData->getDuration();
	//_ease			= (_ease == -2) ? mBoneAniData->getTweenEasing() : _ease;
	_ease			= -2;
	// 使用程序设定的_loop值，而不是从文件中读取的配置信息
	//_loop			= mBoneAniData->getLoop();
	

	CCProcessBase::playTo(_to, _durationTo, _durationTween, _loop, _ease);


	mDuration = mBoneAniData->getDuration();

	if (mDuration == 1) {
		mLoop = SINGLE;
	}else {
		if (_loop) {
			mLoop = LIST_LOOP_START;
		}else {
			mLoop = LIST_START;
			mDuration --;
		}
		mDurationTween = _durationTween;
	}

	CCFrameNodeList *_eachA;
	CCDictElement	*_element = NULL;
	CCTween			*_tween;

	CCDICT_FOREACH(mTweens, _element) {
		_tween = (CCTween*)_element->getObject();
		_eachA = mBoneAniData->getAnimation(_element->getStrKey());
		if (_eachA) {
			_tween->playTo(_eachA, _durationTo, _durationTween, _loop, _ease);
		}else{
			_tween->setActive(false);
		}
	}

	//CCXLOG("Finish CCArmatureAnimation::playTo");
}

void CCArmatureAnimation::SendAnimationHandleMessage()
{
	if (mEventListener!=NULL) {
		if (mActionStateFlag != "")
		{
			mEventListener->animationHandler(mActionStateFlag.c_str(), mAniIDNow, mAnimFrame.c_str());
		}		
	}
}

void CCArmatureAnimation::updateHandler() {
	mActionStateFlag = "";
	mAnimFrame = "";
	if (mCurrentPrecent >= 1) {
		switch(mLoop) {
		case LIST_START:
			//列表过渡
			mLoop = -1;
			mCurrentFrame = mCurrentPrecent * mTotalFrames;
			mCurrentPrecent = mCurrentFrame / mDurationTween;
			//CCXLOG("currentPercent : %f", mCurrentPrecent);
			if (mCurrentPrecent >= 1.0f) {
				//播放速度太快或durationTween时间太短，进入下面的case
			} else {
				// mCurrentPrecent = (int)(mCurrentPrecent) % 1;
				mTotalFrames = mDurationTween;
				mTotalDuration = 0;
				if (mEventListener!=NULL) {
					//mEventListener->animationHandler(START, mAniIDNow, "");
					mActionStateFlag = START;
				}
				break;
			}
		case LIST:
		case SINGLE:
			mCurrentPrecent = 1;
			mIsComplete = true;
			mIsSingleAnimationOver = true;
			if (mEventListener!=NULL) {
				//mEventListener->animationHandler(COMPLETE, mAniIDNow, "");
				mActionStateFlag = COMPLETE;
			}
			break;
		case LIST_LOOP_START:
			//循环开始
			mLoop = 0;
			mCurrentPrecent = 0;
			mTotalFrames = mDurationTween;
			mTotalDuration = 0;
			if (mEventListener!=NULL) {
				//mEventListener->animationHandler(START, mAniIDNow, "");
				mActionStateFlag = START;
			}
			break;
		default:
			//继续循环			
			mLoop += int(mCurrentPrecent);
			mCurrentPrecent = (int)(mCurrentPrecent) % 1;
			if (mEventListener != NULL) {
				if (mIsPauseOnOver)
				{
					pause();
				}
				//mEventListener->animationHandler(LOOP_COMPLETE, mAniIDNow, "");
				mActionStateFlag = LOOP_COMPLETE;
			}
			break;
		}
	}
	else
	{
		if (mEventListener != NULL) {
			//mEventListener->animationHandler(IN_FRAME, mAniIDNow, "");
			mActionStateFlag = IN_FRAME;
			char buffer[100];
			sprintf(buffer,"%d",(int)mCurrentFrame);
			mAnimFrame = buffer;
		}
	}

	//if (mLoop >= LIST && mBoneAniData->getEventList()->count() > 0) {
	//	//多关键帧动画过程
	//	//updateCurrentPrecent();
	//}
}

void CCArmatureAnimation::updateCurrentPrecent() {
	float _playedFrames = mDuration * mCurrentPrecent;
	if (_playedFrames <= mTotalDuration-mBetweenDuration || _playedFrames > mTotalDuration) {
		mToFrameID = 0;
		mTotalDuration = 0;

		CCEventFrame *_eventFrame = (CCEventFrame*)(mBoneAniData->getEventList()->objectAtIndex(mToFrameID));
		int _prevFrameID;
		do {
			mBetweenDuration = _eventFrame->getFrame();
			mTotalDuration += mBetweenDuration;
			_prevFrameID = mToFrameID;
			if (++mToFrameID >= mBoneAniData->getEventList()->count()) {
				mToFrameID = 0;
			}
		} while (_playedFrames >= mTotalDuration);

		if (mEventListener != NULL) {
			char buffer[100];
			sprintf(buffer,"%d",_eventFrame->getFrame());
			mAnimFrame = buffer;
			mEventListener->animationHandler(IN_FRAME, mAniIDNow, mAnimFrame.c_str());
		}
	}

	mCurrentPrecent = 1 - (mTotalDuration - _playedFrames) / mBetweenDuration;
}

bool CCArmatureAnimation::GetSingleAnimationOverOrNot()
{
	return mIsSingleAnimationOver;
}