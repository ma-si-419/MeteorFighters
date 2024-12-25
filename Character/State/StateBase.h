#pragma once

#include <memory>
class StateBase
{
public:
    //�J�n����
    virtual void Enter() abstract;
    //�X�V����
    virtual void Update() abstract;
    //�I������
    virtual void Exit() abstract;

};

