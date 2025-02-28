//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
//  
//
//=============================================================================

#ifndef TF_FX_H
#define TF_FX_H
#ifdef _WIN32
#pragma once
#endif

#include "particle_parse.h"
#include "tf_player.h"

void TE_FireBullets( int iPlayerIndex, const Vector &vOrigin, const QAngle &vAngles, int iWeaponID, int	iMode, int iSeed, float flSpread, bool bCritical );
void TE_TFExplosion( IRecipientFilter &filter, float flDelay, const Vector &vecOrigin, const Vector &vecNormal, ETFWeaponID iWeaponID, int nEntIndex, CBasePlayer *pPlayer = NULL, int iTeam = TEAM_UNASSIGNED, bool bCrit = false, int iItemID = -1 );

void TE_TFParticleEffect( IRecipientFilter &filter, float flDelay, const char *pszParticleName, ParticleAttachment_t iAttachType, CBaseEntity *pEntity, const char *pszAttachmentName, bool bResetAllParticlesOnEntity = false );
void TE_TFParticleEffect( IRecipientFilter &filter, float flDelay, const char *pszParticleName, ParticleAttachment_t iAttachType, CBaseEntity *pEntity = NULL, int iAttachmentPoint = -1, bool bResetAllParticlesOnEntity = false );
void TE_TFParticleEffect( IRecipientFilter &filter, float flDelay, const char *pszParticleName, Vector vecOrigin, QAngle vecAngles, CBaseEntity *pEntity = NULL, int iAttachType = PATTACH_CUSTOMORIGIN );
void TE_TFParticleEffect( IRecipientFilter &filter, float flDelay, const char *pszParticleName, Vector vecOrigin, Vector vecStart, QAngle vecAngles, CBaseEntity *pEntity = NULL );
void TE_TFParticleEffect( IRecipientFilter &filter, float flDelay, int iEffectIndex, Vector vecOrigin, Vector vecStart, QAngle vecAngles, CBaseEntity *pEntity = NULL );

void TE_TFSpawnEffect( CTFPlayer *pPlayer, const char *pszEffect );

#endif	// TF_FX_H
