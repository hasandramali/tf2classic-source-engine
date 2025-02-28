//========= Copyright © 1996-2004, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
//=============================================================================
#ifndef TF_PLAYERANIMSTATE_H
#define TF_PLAYERANIMSTATE_H
#ifdef _WIN32
#pragma once
#endif

#include "convar.h"
#include "multiplayer_animstate.h"

#if defined( CLIENT_DLL )
class C_TFPlayer;
#define CTFPlayer C_TFPlayer
#else
class CTFPlayer;
#endif

// ------------------------------------------------------------------------------------------------ //
// CPlayerAnimState declaration.
// ------------------------------------------------------------------------------------------------ //
class CTFPlayerAnimState : public CMultiPlayerAnimState
{
public:
	
	DECLARE_CLASS( CTFPlayerAnimState, CMultiPlayerAnimState );

	CTFPlayerAnimState();
	CTFPlayerAnimState( CBasePlayer *pPlayer, MultiPlayerMovementData_t &movementData );
	~CTFPlayerAnimState();

	void InitTF( CTFPlayer *pPlayer );
	CTFPlayer *GetTFPlayer( void )							{ return m_pTFPlayer; }

	virtual void ClearAnimationState();
	virtual Activity TranslateActivity( Activity actDesired );
	virtual void Update( float eyeYaw, float eyePitch );
	virtual Activity CalcMainActivity( void );

	virtual void	DoAnimationEvent( PlayerAnimEvent_t event, int nData = 0 );
	virtual void	RestartGesture( int iGestureSlot, Activity iGestureActivity, bool bAutoKill = true );

	virtual bool	HandleMoving(Activity &idealActivity);
	virtual bool	HandleJumping(Activity &idealActivity);
	virtual bool	HandleDucking(Activity &idealActivity);
	virtual bool	HandleSwimming(Activity &idealActivity);

	void			CheckStunAnimation( void );

private:
	
	CTFPlayer   *m_pTFPlayer;
	bool		m_bInAirWalk;

	float		m_flHoldDeployedPoseUntilTime;
	float		m_flTauntAnimTime;
};

CTFPlayerAnimState *CreateTFPlayerAnimState( CTFPlayer *pPlayer );

#endif // TF_PLAYERANIMSTATE_H
