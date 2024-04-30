//====== Copyright � 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================
#include "cbase.h"
#include "tf_weapon_compound_bow.h"
#include "in_buttons.h"
#include "engine/IEngineSound.h"

// Client specific.
#ifdef CLIENT_DLL
#include "c_tf_player.h"
// Server specific.
#else
#include "te_effect_dispatch.h"
#include "tf_player.h"
#include "tf_gamestats.h"
#endif

IMPLEMENT_NETWORKCLASS_ALIASED( TFCompoundBow, DT_WeaponCompoundBow )

BEGIN_NETWORK_TABLE( CTFCompoundBow, DT_WeaponCompoundBow )
#ifdef CLIENT_DLL
	RecvPropTime( RECVINFO( m_flChargeBeginTime ) ),
	RecvPropBool( RECVINFO( m_bFlame ) ),
#else
	SendPropTime( SENDINFO( m_flChargeBeginTime ) ),
	SendPropBool( SENDINFO ( m_bFlame ) ),
#endif
END_NETWORK_TABLE()

#ifdef CLIENT_DLL
BEGIN_PREDICTION_DATA( CTFCompoundBow )
	DEFINE_PRED_FIELD( m_flChargeBeginTime, FIELD_FLOAT, FTYPEDESC_INSENDTABLE ),
END_PREDICTION_DATA()
#endif

LINK_ENTITY_TO_CLASS( tf_weapon_compound_bow, CTFCompoundBow );
PRECACHE_WEAPON_REGISTER( tf_weapon_compound_bow );

// Server specific.
#ifndef CLIENT_DLL
BEGIN_DATADESC( CTFCompoundBow )
END_DATADESC()
#endif

CTFCompoundBow::CTFCompoundBow()
{
	Extinguish();
#ifdef CLIENT_DLL
	m_bFlame = false;
	bEmitting = false;
	bFirstPerson = false;
#endif
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFCompoundBow::Precache( void )
{
	PrecacheScriptSound( "ArrowLight" );
	PrecacheParticleSystem( "v_flaming_arrow" );
	PrecacheParticleSystem( "flaming_arrow" );

	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose: Reset the charge when we holster
//-----------------------------------------------------------------------------
bool CTFCompoundBow::Holster( CBaseCombatWeapon *pSwitchingTo )
{
	Extinguish();
	LowerBow();

	return BaseClass::Holster( pSwitchingTo );
}

//-----------------------------------------------------------------------------
// Purpose: Reset the charge when we deploy
//-----------------------------------------------------------------------------
bool CTFCompoundBow::Deploy( void )
{
	m_flChargeBeginTime = 0;

	return BaseClass::Deploy();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFCompoundBow::WeaponReset( void )
{
	Extinguish();
	BaseClass::WeaponReset();

	LowerBow();
}

// ---------------------------------------------------------------------------- -
// Purpose: 
//-----------------------------------------------------------------------------
void CTFCompoundBow::PrimaryAttack( void )
{
	if ( !CanAttack() )
	{
		m_flChargeBeginTime = 0;
		return;
	}

	if ( m_flChargeBeginTime <= 0 )
	{
		// Set the weapon mode.
		m_iWeaponMode = TF_WEAPON_PRIMARY_MODE;

		// save that we had the attack button down
		m_flChargeBeginTime = gpGlobals->curtime;

		SendWeaponAnim( ACT_ITEM2_VM_CHARGE );
		
		CTFPlayer *pOwner = GetTFPlayerOwner();
		if ( pOwner )
		{
			WeaponSound( SPECIAL1 );

			pOwner->m_Shared.AddCond( TF_COND_AIMING );
			pOwner->TeamFortress_SetSpeed();
		}
	}
}

// ---------------------------------------------------------------------------- -
// Purpose: 
//-----------------------------------------------------------------------------
void CTFCompoundBow::SecondaryAttack( void )
{
	LowerBow();
}

// ---------------------------------------------------------------------------- -
// Purpose: Cancel shot charging.
//-----------------------------------------------------------------------------
void CTFCompoundBow::LowerBow( void )
{
	if ( m_flChargeBeginTime == 0.0f )
		return;

	m_flChargeBeginTime = 0.0f;

	CTFPlayer *pOwner = GetTFPlayerOwner();
	if ( pOwner )
	{
		pOwner->m_Shared.RemoveCond( TF_COND_AIMING );
		pOwner->TeamFortress_SetSpeed();
	}

	// Delay the next fire so they don't immediately start charging again.
	m_flNextPrimaryAttack = gpGlobals->curtime + 1.0f;

	SendWeaponAnim( ACT_ITEM2_VM_DRYFIRE );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFCompoundBow::WeaponIdle( void )
{
	CTFPlayer *pOwner = GetTFPlayerOwner();
	if ( pOwner->GetWaterLevel() >= WL_Waist )
	{
		Extinguish();
	}

	if ( m_flChargeBeginTime > 0 && pOwner->GetAmmoCount( GetPrimaryAmmoType() ) > 0 )
	{
		FireArrow();
	}
	else
	{
		BaseClass::WeaponIdle();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFCompoundBow::ItemPostFrame( void )
{
	BaseClass::ItemPostFrame();

	if ( m_flChargeBeginTime != 0.0f )
	{
		float flChargeTime = gpGlobals->curtime - m_flChargeBeginTime;
		if ( flChargeTime >= TF_BOW_CHARGE_TIRED_TIME )
		{
			// Play flinching animation if it was charged for too long.
			if ( GetActivity() == ACT_ITEM2_VM_IDLE_2 )
			{
				SendWeaponAnim( ACT_ITEM2_VM_CHARGE_IDLE_3 );
			}
			else if ( IsViewModelSequenceFinished() )
			{
				SendWeaponAnim( ACT_ITEM2_VM_IDLE_3 );
			}
		}
		else if ( IsViewModelSequenceFinished() )
		{
			SendWeaponAnim( ACT_ITEM2_VM_IDLE_2 );
		}
	}
}

// ---------------------------------------------------------------------------- -
// Purpose: 
//-----------------------------------------------------------------------------
void CTFCompoundBow::FireArrow( void )
{
	m_bReloadedThroughAnimEvent = false; // Huntsman reload fix

	// Get the player owning the weapon.
	CTFPlayer *pPlayer = GetTFPlayerOwner();
	if ( !pPlayer )
		return;

	// Make sure we're aiming already
	if( !pPlayer->m_Shared.InCond( TF_COND_AIMING ) )
		return;

	CalcIsAttackCritical();

	SendWeaponAnim( ACT_VM_PRIMARYATTACK );

	pPlayer->SetAnimation( PLAYER_ATTACK1 );
	pPlayer->DoAnimationEvent( PLAYERANIMEVENT_ATTACK_PRIMARY );

	pPlayer->m_Shared.RemoveCond( TF_COND_AIMING );
	pPlayer->TeamFortress_SetSpeed();

	FireProjectile( pPlayer );

#if !defined( CLIENT_DLL ) 
	pPlayer->SpeakWeaponFire();
	CTF_GameStats.Event_PlayerFiredWeapon( pPlayer, IsCurrentAttackACrit() );
#endif

	// Set next attack times.
	float flDelay = m_pWeaponInfo->GetWeaponData( m_iWeaponMode ).m_flTimeFireDelay;
	CALL_ATTRIB_HOOK_FLOAT( flDelay, mult_postfiredelay );
	m_flNextPrimaryAttack = gpGlobals->curtime + flDelay;

	SetWeaponIdleTime( gpGlobals->curtime + SequenceDuration() );

	m_flChargeBeginTime = 0.0f;
	Extinguish();
}

void CTFCompoundBow::Extinguish( void )
{
#ifndef CLIENT_DLL
	m_bFlame = false;
#else
	if ( m_hFlameEffectHost.Get() )
	{
		m_hFlameEffectHost->ParticleProp()->StopEmission();
		m_hFlameEffectHost = NULL;
	}
	//ParticleProp()->StopEmission();
	bEmitting = false;
#endif
}

//-----------------------------------------------------------------------------
// Purpose: Set arrow on fire
//-----------------------------------------------------------------------------
void CTFCompoundBow::LightArrow( void )
{
	// don't light if we're already lit.
#ifndef CLIENT_DLL
	if ( m_bFlame || m_flNextPrimaryAttack > gpGlobals->curtime )
	{
		return;
	}
	m_bFlame = true;
	EmitSound( "ArrowLight" );
#else
	if( bEmitting )
	{
		return;
	}

	C_BaseEntity *pModel = GetWeaponForEffect();
	if ( pModel )
	{
		CTFPlayer *pOwner = GetTFPlayerOwner();

		// if the local player is the carrier and is in first person, use the v flame effects
		if ( pOwner == C_BasePlayer::GetLocalPlayer() && pOwner->InFirstPersonView() )
		{
			pModel->ParticleProp()->Create( "v_flaming_arrow", PATTACH_POINT_FOLLOW, "muzzle" );
			bFirstPerson = true;
		}
		else
		{
			pModel->ParticleProp()->Create( "flaming_arrow", PATTACH_POINT_FOLLOW, "muzzle" );
			bFirstPerson = false;
		}
		m_hFlameEffectHost = pModel;
	}

	bEmitting = true;
	SetNextClientThink( gpGlobals->curtime + 1.0f );
#endif
}

#ifdef CLIENT_DLL
void C_TFCompoundBow::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );
	if ( !bEmitting && m_bFlame )
	{
		LightArrow();
	}
}

void C_TFCompoundBow::ClientThink( void )
{
	C_TFPlayer *pOwner = GetTFPlayerOwner();

	if (  m_iState != WEAPON_IS_ACTIVE || !m_bFlame )
	{
		Extinguish();
		SetNextClientThink( CLIENT_THINK_NEVER );
		return;
	}

	if ( pOwner->IsLocalPlayer() && ( ( pOwner->InFirstPersonView() && !bFirstPerson ) || ( !pOwner->InFirstPersonView() && bFirstPerson ) ) )
	{
		Extinguish();
		LightArrow();
		return;
	}

	SetNextClientThink( gpGlobals->curtime + 1.0f );
}
#endif

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
float CTFCompoundBow::GetProjectileDamage( void )
{
	float flMaxChargeDamage = BaseClass::GetProjectileDamage();

	return RemapValClamped( ( gpGlobals->curtime - m_flChargeBeginTime ),
		0.0f,
		TF_BOW_MAX_CHARGE_TIME,
		TF_BOW_MIN_CHARGE_DAMAGE,
		TF_BOW_MIN_CHARGE_DAMAGE + flMaxChargeDamage );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
float CTFCompoundBow::GetProjectileSpeed( void )
{
	return RemapValClamped( ( gpGlobals->curtime - m_flChargeBeginTime ),
		0.0f,
		TF_BOW_MAX_CHARGE_TIME,
		TF_BOW_MIN_CHARGE_VEL,
		TF_BOW_MAX_CHARGE_VEL );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
float CTFCompoundBow::GetProjectileGravity( void )
{
	return RemapValClamped( ( gpGlobals->curtime - m_flChargeBeginTime ),
		0.0f,
		TF_BOW_MAX_CHARGE_TIME,
		0.5f,
		0.1f );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFCompoundBow::GetProjectileFireSetup( CTFPlayer *pPlayer, Vector vecOffset, Vector *vecSrc, QAngle *angForward, bool bHitTeammates, bool bUseHitboxes )
{
	BaseClass::GetProjectileFireSetup( pPlayer, vecOffset, vecSrc, angForward, bHitTeammates, bUseHitboxes );

	// Make the shot wildly inaccurate if charged for too long.
	if ( gpGlobals->curtime - m_flChargeBeginTime >= TF_BOW_CHARGE_TIRED_TIME )
	{
		float flSpread = TF_BOW_TIRED_SPREAD;
		angForward->x += RandomFloat( -flSpread, flSpread );
		angForward->y += RandomFloat( -flSpread, flSpread );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFCompoundBow::CalcIsAttackCriticalHelper( void )
{
	// No random critical hits.
	return false;
}

float CTFCompoundBow::GetChargeMaxTime( void )
{
	return TF_BOW_MAX_CHARGE_TIME;
}

float CTFCompoundBow::GetCurrentCharge( void ) const
{
	if (m_flChargeBeginTime > 0.0f)
		return Min( 1.0f, gpGlobals->curtime - m_flChargeBeginTime );

	return 0.0f;
}

#ifdef CLIENT_DLL
//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFCompoundBow::CreateMove(float flInputSampleTime, CUserCmd *pCmd, const QAngle &vecOldViewAngles)
{
	// Prevent jumping while aiming
	if (GetTFPlayerOwner()->m_Shared.InCond(TF_COND_AIMING))
	{
		pCmd->buttons &= ~IN_JUMP;
	}

	BaseClass::CreateMove(flInputSampleTime, pCmd, vecOldViewAngles);
}
#endif

