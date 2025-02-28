//=============================================================================
//
// Purpose: MLG
//
//=============================================================================
#ifndef TF_WEAPON_DISPLACER_H
#define TF_WEAPON_DISPLACER_H

#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_gun.h"

#ifdef CLIENT_DLL
#define CTFWeaponDisplacer C_TFWeaponDisplacer
#endif

class CTFWeaponDisplacer : public CTFWeaponBaseGun
{
public:
	DECLARE_CLASS( CTFWeaponDisplacer, CTFWeaponBaseGun );
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	CTFWeaponDisplacer();

	virtual ETFWeaponID GetWeaponID( void ) const { return TF_WEAPON_DISPLACER; }

	virtual void	Precache( void );
	virtual void	WeaponReset( void );
	virtual bool	CanHolster( void ) const;
	virtual bool	Holster( CBaseCombatWeapon *pSwitchingTo );
	virtual void	ItemPostFrame( void );
	virtual void	PrimaryAttack( void );
	virtual void	SecondaryAttack( void );

	bool			IsChargingAnyShot( void ) const;
	void			FinishTeleport( void );

#ifdef CLIENT_DLL
	virtual void	PreDataUpdate( DataUpdateType_t updateType );
	virtual void	OnDataChanged( DataUpdateType_t updateType );
	void			CreateWarmupEffect( bool bSecondary );
#endif

private:
	CNetworkVar( float, m_flBlastTime );
	CNetworkVar( float, m_flTeleportTime );
	CNetworkVar( bool, m_bLockedOn );

#ifdef GAME_DLL
	EHANDLE m_hTeleportSpot;
#else
	bool m_bWasChargingBlast;
	bool m_bWasChargingTeleport;
#endif

	bool m_bPlayedTeleportEffect;
};

#endif // TF_WEAPON_DISPLACER_H
