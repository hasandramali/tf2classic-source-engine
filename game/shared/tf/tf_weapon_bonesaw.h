//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#ifndef TF_WEAPON_BONESAW_H
#define TF_WEAPON_BONESAW_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_melee.h"

#ifdef CLIENT_DLL
#define CTFBonesaw C_TFBonesaw
#endif

//=============================================================================
//
// Bonesaw class.
//
class CTFBonesaw : public CTFWeaponBaseMelee
{
public:

	DECLARE_CLASS( CTFBonesaw, CTFWeaponBaseMelee );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	CTFBonesaw() {}
	virtual ETFWeaponID GetWeaponID( void ) const { return TF_WEAPON_BONESAW; }

#ifdef CLIENT_DLL
	virtual void		OnDataChanged( DataUpdateType_t updateType );
	virtual bool		Deploy( void );
	void				UpdateChargePoseParam( void );
#endif

private:

	CTFBonesaw( const CTFBonesaw & ) {}
};

#endif // TF_WEAPON_BONESAW_H
