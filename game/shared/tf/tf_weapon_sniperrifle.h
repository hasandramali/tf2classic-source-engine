//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======//
//
// Purpose: TF Sniper Rifle
//
//=============================================================================//
#ifndef TF_WEAPON_SNIPERRIFLE_H
#define TF_WEAPON_SNIPERRIFLE_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_gun.h"
#include "Sprite.h"

#if defined( CLIENT_DLL )
#define CTFSniperRifle C_TFSniperRifle
#define CSniperDot C_SniperDot
#endif

extern float GetChargePerSecValue( void );
extern float GetUnchargePerSecValue( void );

//=============================================================================
//
// Sniper Rifle Laser Dot class.
//
class CSniperDot : public CBaseEntity
{
public:

	DECLARE_CLASS( CSniperDot, CBaseEntity );
	DECLARE_NETWORKCLASS();
	DECLARE_DATADESC();

	// Creation/Destruction.
	CSniperDot( void );
	~CSniperDot( void );

	static CSniperDot *Create( const Vector &origin, CBaseEntity *pOwner = NULL, bool bVisibleDot = true );
	void		ResetChargeTime( void ) { m_flChargeStartTime = gpGlobals->curtime; }

	// Attributes.
	int			ObjectCaps()							{ return (BaseClass::ObjectCaps() & ~FCAP_ACROSS_TRANSITION) | FCAP_DONT_SAVE; }

	// Targeting.
	void        Update( CBaseEntity *pTarget, const Vector &vecOrigin, const Vector &vecNormal );
	CBaseEntity	*GetTargetEntity( void )				{ return m_hTargetEnt; }
	
// Client specific.
#ifdef CLIENT_DLL

	// Rendering.
	virtual bool			IsTransparent( void )		{ return true; }
	virtual RenderGroup_t		GetRenderGroup( void )		{ return RENDER_GROUP_TRANSLUCENT_ENTITY; }
	virtual int			DrawModel( int flags );
	virtual bool			ShouldDraw( void );
	
	//
	virtual void			OnDataChanged( DataUpdateType_t updateType );

	CMaterialReference		m_hSpriteMaterial;

#endif

protected:

	Vector					m_vecSurfaceNormal;
	EHANDLE					m_hTargetEnt;

	CNetworkVar( float, m_flChargeStartTime );
};

#define TF_WEAPON_SNIPERRIFLE_CHARGE_PER_SEC		50.0
#define TF_WEAPON_SNIPERRIFLE_UNCHARGE_PER_SEC		75.0

//=============================================================================
//
// Sniper Rifle class.
//
class CTFSniperRifle : public CTFWeaponBaseGun
{
public:

	DECLARE_CLASS( CTFSniperRifle, CTFWeaponBaseGun );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	CTFSniperRifle();
	~CTFSniperRifle();

	virtual ETFWeaponID GetWeaponID( void ) const { return TF_WEAPON_SNIPERRIFLE; }

	virtual void Spawn();
	virtual void Precache();
	void		 ResetTimers( void );

	virtual bool Reload( void );
	virtual bool CanHolster( void ) const;
	virtual bool Holster( CBaseCombatWeapon *pSwitchingTo );

	void		 HandleZooms( void );
	virtual void ItemPostFrame( void );
	virtual bool Lower( void );
	virtual float GetProjectileDamage( void );

	virtual void WeaponReset( void );

	virtual bool CanFireCriticalShot( bool bIsHeadshot = false );

#ifdef CLIENT_DLL
	float GetHUDDamagePerc( void );
#endif

	bool IsZoomed( void );
	
	bool CanHeadshot();
	
	virtual float GetChargeSpeed() { return TF_WEAPON_SNIPERRIFLE_CHARGE_PER_SEC; }
	virtual float GetUnchargeSpeed() { return TF_WEAPON_SNIPERRIFLE_UNCHARGE_PER_SEC; }

	float GetChargedDamage() const { return m_flChargedDamage; }
private:

	void CreateSniperDot( void );
	void DestroySniperDot( void );
	void UpdateSniperDot( void );

private:
	// Auto-rezooming handling
	void SetRezoom( bool bRezoom, float flDelay );

	void Zoom( void );
	void ZoomOutIn( void );
	void ZoomIn( void );
	void ZoomOut( void );
	void Fire( CTFPlayer *pPlayer );

private:

	CNetworkVar( float,	m_flChargedDamage );

#ifdef GAME_DLL
	CHandle<CSniperDot>		m_hSniperDot;
#else
	bool m_bDinged;
#endif

	// Handles rezooming after the post-fire unzoom
	float m_flUnzoomTime;
	float m_flRezoomTime;
	bool m_bRezoomAfterShot;

	CTFSniperRifle( const CTFSniperRifle & );
};

#endif // TF_WEAPON_SNIPERRIFLE_H
