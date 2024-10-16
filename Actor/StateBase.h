#pragma once

#include <memory>
class StateBase
{
public:
    //開始処理
    virtual void Enter() abstract;
    //更新処理
    virtual void Update() abstract;
    //終了処理
    virtual void Exit() abstract;

};

