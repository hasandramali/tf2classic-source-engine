t//=============================================================================//
//
// Purpose: Jarate Projectile
//
//=============================================================================//

#include "cbase.h"
#include "tf_projectile_jar.h"
#include "tf_gamerules.h"
#include "effect_dispatch_data.h"
#include "tf_weapon_jar.h"

#ifdef GAME_DLL
#include "tf_fx.h"
#else
#include "particles_new.h"
#endif

#define TF_WEAPON_JAR_MODEL		"models/weapons/c_models/urinejar.mdl"
#define TF_WEAPON_JARMILK_MODEL "models/weapons/c_models/c_madmilk/c_madmilk.mdl"

#define TF_WEAPON_JAR_LIFETIME  2.0f


IMPLEMENT_NETWORKCLASS_ALIASED( TFProjectile_Jar, DT_TFProjectile_Jar )

BEGIN_NETWORK_TABLE( CTFProjectile_Jar, DT_TFProjectile_Jar )
#ifdef CLIENT_DLL
	RecvPropBool( RECVINFO( m_bCritical ) ),
	RecvPropInt( RECVINFO( m_nSkin ) ),
#else
	SendPropBool( SENDINFO( m_bCritical ) ),
	SendPropInt( SENDINFO( m_nSkin ), 0, SPROP_UNSIGNED ),
#endif
END_NETWORK_TABLE()

#ifdef GAME_DLL
BEGIN_DATADESC( CTFProjectile_Jar )
END_DATADESC()
#endif

ConVar tf_jar_show_radius( "tf_jar_show_radius", "0", FCVAR_REPLICATED | FCVAR_CHEAT, "Render jar radius." );

LINK_ENTITY_TO_CLASS( tf_projectile_jar, CTFProjectile_Jar );
PRECACHE_REGISTER( tf_projectile_jar );

CTFProjectile_Jar::CTFProjectile_Jar()
{
}

CTFProjectile_Jar::~CTFProjectile_Jar()
{
#ifdef CLIENT_DLL
	ParticleProp()->StopEmission();
#endif
}

#ifdef GAME_DLL
CTFProjectile_Jar *CTFProjectile_Jar::Create( CBaseEntity *pWeapon, const Vector &vecOrigin, const QAngle &vecAngles, const Vector &vecVelocity, CBaseCombatCharacter *pOwner, CBaseEntity *pScorer, const AngularImpulse &angVelocity, const CTFWeaponInfo &weaponInfo )
{
	CTFProjectile_Jar *pJar = static_cast<CTFProjectile_Jar *>( CBaseEntity::CreateNoSpawn( "tf_projectile_jar", vecOrigin, vecAngles, pOwner ) );

	if ( pJar )
	{
		// Set scorer.
		pJar->SetScorer( pScorer );

		// Set firing weapon.
		pJar->SetLauncher( pWeapon );

		DispatchSpawn( pJar );

		//pJar->InitGrenade( vecVelocity, angVelocity, pOwner, weaponInfo );
		pJar->InitGrenade( vecVelocity, angVelocity, pOwner, pWeapon );

		pJar->ApplyLocalAngularVelocityImpulse( angVelocity );
		
		switch (pOwner->GetTeamNumber())
		{
			case TF_TEAM_RED:
				pJar->m_nSkin = 0;
				break;
			case TF_TEAM_BLUE:
				pJar->m_nSkin = 1;
				break;
		}
	}

	return pJar;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFProjectile_Jar::Precache( void )
{
	PrecacheModel( TF_WEAPON_JAR_MODEL );

	PrecacheTeamParticles( "peejar_trail_%s", false, g_aTeamNamesShort );
	PrecacheParticleSystem( "peejar_impact" );

	PrecacheScriptSound( "Jar.Explode" );
	PrecacheScriptSound( "Weapon_GasCan.Explode" );

	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFProjectile_Jar::Spawn( void )
{
	switch (GetEffectCondition())
	{
		case TF_COND_URINE:
			SetModel( TF_WEAPON_JAR_MODEL );
			break;
			
		case TF_COND_MAD_MILK:
			SetModel( TF_WEAPON_JARMILK_MODEL );
			break;
	}

	BaseClass::Spawn();
	SetTouch( &CTFProjectile_Jar::JarTouch );

	// Pumpkin Bombs
	AddFlag( FL_GRENADE );
	
	// We don't need this, but it's useful for situations where it could get stuck on level geometry (dynamic ramps)
	SetDetonateTimerLength( 15.0f );

	// Don't collide with anything for a short time so that we never get stuck behind surfaces
	SetCollisionGroup( COLLISION_GROUP_NONE );

	AddSolidFlags( FSOLID_TRIGGER );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFProjectile_Jar::Explode( trace_t *pTrace, int bitsDamageType )
{
	// TODO: Fix this later!!!

	// Invisible.
	SetModelName( NULL_STRING );
	AddSolidFlags( FSOLID_NOT_SOLID );
	m_takedamage = DAMAGE_NO;

	/*// Pull out of the wall a bit
	if ( pTrace->fraction != 1.0 )
	{
		SetAbsOrigin( pTrace->endpos + ( pTrace->plane.normal * 1.0f ) );
	}

	CTFWeaponBase *pWeapon = dynamic_cast<CTFWeaponBase *>( m_hLauncher.Get() );

	// Pull out a bit.
	if ( pTrace->fraction != 1.0 )
	{
		SetAbsOrigin( pTrace->endpos + ( pTrace->plane.normal * 1.0f ) );
	}

	// Play explosion sound and effect.
	Vector vecOrigin = GetAbsOrigin();
	// Splash!
	CPVSFilter particleFilter( vContactPoint );
	TE_TFParticleEffect( particleFilter, 0.0, GetImpactEffect(), vContactPoint, vec3_angle );

	// Explosion effect.
	CBroadcastRecipientFilter soundFilter;
	Vector vecOrigin = vContactPoint;
	CBaseEntity::EmitSound( soundFilter, iEntIndex, TF_WEAPON_PEEJAR_EXPLODE_SOUND, &vecOrigin );

	// Damage.
	CBaseEntity *pAttacker = NULL;
	if( pWeapon )
		pAttacker = pWeapon->GetOwnerEntity();

	float flRadius = GetDamageRadius();

	CTakeDamageInfo newInfo( this, pAttacker, m_hLauncher, vec3_origin, vecOrigin, GetDamage(), GetDamageType() );
	CTFRadiusDamageInfo radiusInfo( &newInfo, vecOrigin, flRadius, NULL, flRadius ); // NEW TF METHOD

	// Debug!
	if ( tf_jar_show_radius.GetBool() )
	{
		DrawRadius( flRadius );
	}
	*/

	SetThink( &CBaseGrenade::SUB_Remove );
	SetTouch( NULL );
	
	UTIL_Remove( this );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFProjectile_Jar::JarTouch( CBaseEntity *pOther )
{
	if ( pOther == GetThrower() )
	{
		// Make us solid if we're not already
		if ( GetCollisionGroup() == COLLISION_GROUP_NONE )
		{
			SetCollisionGroup( COLLISION_GROUP_PROJECTILE );
		}
		return;
	}

	// Verify a correct "other."
	if ( !pOther->IsSolid() || pOther->IsSolidFlagSet( FSOLID_VOLUME_CONTENTS ) )
		return;

	// Handle hitting skybox (disappear).
	trace_t pTrace;
	Vector velDir = GetAbsVelocity();
	VectorNormalize( velDir );
	Vector vecSpot = GetAbsOrigin() - velDir * 32;
	UTIL_TraceLine( vecSpot, vecSpot + velDir * 64, MASK_SOLID, this, COLLISION_GROUP_NONE, &pTrace );

	if ( pTrace.fraction < 1.0 && pTrace.surface.flags & SURF_SKY )
	{
		UTIL_Remove( this );
		return;
	}

	// Blow up if we hit a player
	if ( pOther->IsPlayer() )
	{
		m_hEnemy = pOther;
		CreateStickyBoltEffect( pOther, &pTrace );
		Explode( &pTrace, GetDamageType() );
	}
	// We should bounce off of certain surfaces (resupply cabinets, spawn doors, etc.)
	else
	{
		return;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFProjectile_Jar::Detonate()
{
	trace_t		tr;
	Vector		vecSpot;// trace starts here!

	SetThink( NULL );

	vecSpot = GetAbsOrigin() + Vector ( 0 , 0 , 8 );
	UTIL_TraceLine ( vecSpot, vecSpot + Vector ( 0, 0, -32 ), MASK_SHOT_HULL, this, COLLISION_GROUP_NONE, & tr);

	Explode( &tr, GetDamageType() );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFProjectile_Jar::VPhysicsCollision( int index, gamevcollisionevent_t *pEvent )
{
	BaseClass::VPhysicsCollision( index, pEvent );

	int otherIndex = !index;
	CBaseEntity *pHitEntity = pEvent->pEntities[otherIndex];

	if ( !pHitEntity )
	{
		return;
	}

	// Blow up if we hit something static in the world.
	if ( ( pHitEntity->entindex() == 0 ) && ( pHitEntity->GetMoveType() == MOVETYPE_NONE  ) )
	{
		// Blow up next think.
		SetThink( &CTFProjectile_Jar::Detonate );
		SetNextThink( gpGlobals->curtime );
	}

}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFProjectile_Jar::CreateStickyBoltEffect( CBaseEntity *pOther, trace_t *pTrace )
{
#if defined( TERROR )
	// If this is a friendly, don't make the effect at all.
	CTFPlayer *pTarget = ToTFPlayer( pOther );
	if ( !pTarget || pTarget->GetTeamNumber() == GetTeamNumber() )
		return;
			
	// Only attach breadmonsters
	if ( GetProjectileType() == TF_PROJECTILE_BREADMONSTER_JARATE || GetProjectileType() == TF_PROJECTILE_BREADMONSTER_MADMILK )
	{
		// Find what hitbox our jar landed on.
		mstudiobbox_t *pBox = NULL;
		CBaseAnimating* pAnimating = NULL;
		if ( pTrace->m_pEnt && pTrace->m_pEnt->GetTeamNumber() != GetTeamNumber() )
		{
			// A blob of if statements, just to make sure we set this right. 
			CStudioHdr* pStudioHdr = NULL;
			mstudiohitboxset_t* pSet = NULL;
			pAnimating = dynamic_cast<CBaseAnimating*>(pOther);
			if (pAnimating)
				pStudioHdr = pAnimating->GetModelPtr();
			if (pStudioHdr)
				pSet = pStudioHdr->pHitboxSet(pAnimating->GetHitboxSet());
			if (pSet)
				pBox = pSet->pHitbox( pTrace->hitbox );
		}
		// We got ourselves the hitbox and the animation info, time to attach it.
		// This is a heavily edited version of CTFProjectile_Arrow::GetBoneAttachmentInfo
		if ( pBox && pAnimating )
		{
			Vector vecBoneOrigin;
			QAngle vecBoneAngles;
			int iBone, iPhysicsBone;
			iBone = pBox->bone;
			iPhysicsBone = pAnimating->GetPhysicsBone(iBone);
			//pAnim->GetBonePosition( bone, vecOrigin, vecAngles );

			matrix3x4_t arrowToWorld, boneToWorld, invBoneToWorld, boneToWorldTransform;
			MatrixCopy(EntityToWorldTransform(), arrowToWorld);
			pAnimating->GetBoneTransform(iBone, boneToWorld);

			MatrixInvert(boneToWorld, invBoneToWorld);
			ConcatTransforms(invBoneToWorld, arrowToWorld, boneToWorldTransform);
			MatrixAngles(boneToWorldTransform, vecBoneAngles);
			MatrixGetColumn(boneToWorldTransform, 3, vecBoneOrigin);
		
		
			IGameEvent *event = gameeventmanager->CreateEvent( "arrow_impact" );

			if ( event )
			{
				event->SetInt( "attachedEntity", pTarget->entindex() );
				event->SetInt( "shooter", ToTFPlayer( GetThrower() )->entindex() );
				event->SetInt( "projectileType", GetProjectileType() );
				event->SetInt( "boneIndexAttached", iBone );
				event->SetFloat( "bonePositionX", vecBoneOrigin.x );
				event->SetFloat( "bonePositionY", vecBoneOrigin.y );
				event->SetFloat( "bonePositionZ", vecBoneOrigin.z );
				event->SetFloat( "boneAnglesX", vecBoneAngles.x );
				event->SetFloat( "boneAnglesY", vecBoneAngles.y );
				event->SetFloat( "boneAnglesZ", vecBoneAngles.z );
				event->SetBool( "critical", m_bCritical );
				gameeventmanager->FireEvent( event );
			}
		}
	}
#endif // TERROR
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFProjectile_Jar::SetScorer( CBaseEntity *pScorer )
{
	m_Scorer = pScorer;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CBasePlayer *CTFProjectile_Jar::GetAssistant( void )
{
	return dynamic_cast<CBasePlayer *>( m_Scorer.Get() );
}

bool CTFProjectile_Jar::IsDeflectable(void)
{
	// Don't deflect projectiles with non-deflect attributes.
	if (m_hLauncher.Get())
	{
		// Check to see if this is a non-deflectable projectile, like an energy projectile.
		int nCannotDeflect = 0;
		CALL_ATTRIB_HOOK_INT_ON_OTHER(m_hLauncher.Get(), nCannotDeflect, energy_weapon_no_deflect);
		if (nCannotDeflect != 0)
			return false;
	}
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFProjectile_Jar::Deflected( CBaseEntity *pDeflectedBy, Vector &vecDir )
{
	/*// Get jar's speed.
	float flVel = GetAbsVelocity().Length();

	QAngle angForward;
	VectorAngles( vecDir, angForward );
	AngularImpulse angVelocity( ( 600, random->RandomInt( -1200, 1200 ), 0 ) );

	// Now change jar's direction.
	SetAbsAngles( angForward );
	SetAbsVelocity( vecDir * flVel );

	// Bounce it up a bit
	ApplyLocalAngularVelocityImpulse( angVelocity );

	IncremenentDeflected();
	SetOwnerEntity( pDeflectedBy );
	ChangeTeam( pDeflectedBy->GetTeamNumber() );
	SetScorer( pDeflectedBy );*/

	IPhysicsObject *pPhysicsObject = VPhysicsGetObject();
	if ( pPhysicsObject )
	{
		Vector vecOldVelocity, vecVelocity;

		pPhysicsObject->GetVelocity( &vecOldVelocity, NULL );

		float flVel = vecOldVelocity.Length();

		vecVelocity = vecDir;
		vecVelocity *= flVel;
		AngularImpulse angVelocity( ( 600, random->RandomInt( -1200, 1200 ), 0 ) );

		// Now change grenade's direction.
		pPhysicsObject->SetVelocityInstantaneous( &vecVelocity, &angVelocity );
	}

	CBaseCombatCharacter *pBCC = pDeflectedBy->MyCombatCharacterPointer();

	IncremenentDeflected();
	m_hDeflectOwner = pDeflectedBy;
	SetThrower( pBCC );
	switch (pDeflectedBy->GetTeamNumber())
	{
		case TF_TEAM_RED:
			m_nSkin = 0;
			break;
		case TF_TEAM_BLUE:
			m_nSkin = 1;
			break;
	}
	ChangeTeam( pDeflectedBy->GetTeamNumber() );
}
#else
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_TFProjectile_Jar::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );

	if ( updateType == DATA_UPDATE_CREATED )
	{
		m_flCreationTime = gpGlobals->curtime;
		CreateTrails();
	}

	if ( m_iOldTeamNum && m_iOldTeamNum != m_iTeamNum )
	{
		ParticleProp()->StopEmission();
		CreateTrails();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_TFProjectile_Jar::CreateTrails( void )
{
	if ( IsDormant() )
		return;

	const char *pszEffectName = ConstructTeamParticle( "peejar_trail_%s", GetTeamNumber(), false, g_aTeamNamesShort );

	ParticleProp()->Create( pszEffectName, PATTACH_ABSORIGIN_FOLLOW );
}

//-----------------------------------------------------------------------------
// Purpose: Don't draw if we haven't yet gone past our original spawn point
//-----------------------------------------------------------------------------
int CTFProjectile_Jar::DrawModel( int flags )
{
	if ( gpGlobals->curtime - m_flCreationTime < 0.1f )
		return 0;

	return BaseClass::DrawModel( flags );
}
#endif

//=============================================================================
//
// Weapon JarMilk
//
#if defined( TERROR )
IMPLEMENT_NETWORKCLASS_ALIASED( TFProjectile_JarMilk, DT_TFProjectile_JarMilk )

BEGIN_NETWORK_TABLE( CTFProjectile_JarMilk, DT_TFProjectile_JarMilk )
END_NETWORK_TABLE()

#ifdef GAME_DLL
BEGIN_DATADESC( CTFProjectile_JarMilk )
END_DATADESC()
#endif

LINK_ENTITY_TO_CLASS( tf_projectile_jar_milk, CTFProjectile_JarMilk );
PRECACHE_REGISTER( tf_projectile_jar_milk );

#ifdef GAME_DLL
CTFProjectile_JarMilk *CTFProjectile_JarMilk::Create( CBaseEntity *pWeapon, const Vector &vecOrigin, const QAngle &vecAngles, const Vector &vecVelocity, CBaseCombatCharacter *pOwner, CBaseEntity *pScorer, const AngularImpulse &angVelocity, const CTFWeaponInfo &weaponInfo )
{
	CTFProjectile_JarMilk *pJar = static_cast<CTFProjectile_JarMilk *>( CBaseEntity::CreateNoSpawn( "tf_projectile_jar_milk", vecOrigin, vecAngles, pOwner ) );

	if ( pJar )
	{
		// Set scorer.
		pJar->SetScorer( pScorer );

		// Set firing weapon.
		pJar->SetLauncher( pWeapon );

		DispatchSpawn( pJar );

		pJar->InitGrenade( vecVelocity, angVelocity, pOwner, weaponInfo );

		pJar->ApplyLocalAngularVelocityImpulse( angVelocity );
	}

	return pJar;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFProjectile_JarMilk::Precache( void )
{
	PrecacheModel( TF_WEAPON_JARMILK_MODEL );
	PrecacheParticleSystem( "peejar_impact_milk" );

	BaseClass::Precache();
}
#endif // TERROR
#endif
