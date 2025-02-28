//========= Copyright © 1996-2005, Valve LLC, All rights reserved. ============
//
//=============================================================================
#ifndef TF_PLAYER_H
#define TF_PLAYER_H
#pragma once

#include "basemultiplayerplayer.h"
#include "server_class.h"
#include "tf_playeranimstate.h"
#include "tf_shareddefs.h"
#include "tf_player_shared.h"
#include "tf_playerclass.h"
#include "entity_tfstart.h"
#include "tf_inventory.h"
#include "tf_weapon_medigun.h"
#include "ihasattributes.h"
#include "trigger_area_capture.h"
#include "tf_obj_sentrygun.h"

#define SENTRY_MAX_RANGE 1100.0f
#define TF_ROCKET_RADIUS (146)

class CTFPlayer;
class CTFTeam;
class CTFGoal;
class CTFGoalItem;
class CTFItem;
class CTFWeaponBuilder;
class CBaseObject;
class CTFWeaponBase;
class CIntroViewpoint;

extern ConVar tf2c_random_weapons;

//=============================================================================
//
// Player State Information
//
class CPlayerStateInfo
{
public:

	int				m_nPlayerState;
	const char		*m_pStateName;

	// Enter/Leave state.
	void ( CTFPlayer::*pfnEnterState )();	
	void ( CTFPlayer::*pfnLeaveState )();

	// Think (called every frame).
	void ( CTFPlayer::*pfnThink )();
};

struct DamagerHistory_t
{
	DamagerHistory_t()
	{
		Reset();
	}
	void Reset()
	{
		hDamager = NULL;
		flTimeDamage = 0;
	}
	EHANDLE hDamager;
	float	flTimeDamage;
};
#define MAX_DAMAGER_HISTORY 2

//=============================================================================
//
// TF Player
//
class CTFPlayer : public CBaseMultiplayerPlayer, public IHasAttributes
{
public:
	DECLARE_CLASS( CTFPlayer, CBaseMultiplayerPlayer );
	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();

	CTFPlayer();
	~CTFPlayer();

	// Creation/Destruction.
	static CTFPlayer	*CreatePlayer( const char *className, edict_t *ed );
	static CTFPlayer	*Instance( int iEnt );

	virtual void		Spawn();
	virtual int			ShouldTransmit( const CCheckTransmitInfo *pInfo );
	virtual void		ForceRespawn();
	virtual CBaseEntity	*EntSelectSpawnPoint( void );
	virtual void		InitialSpawn();
	virtual void		Precache();
	virtual bool		IsReadyToPlay( void );
	virtual bool		IsReadyToSpawn( void );
	virtual bool		ShouldGainInstantSpawn( void );
	virtual void		ResetScores( void );
	virtual void		PlayerUse( void );

	void				CreateViewModel( int iViewModel = 0 );
	CBaseViewModel		*GetOffHandViewModel();
	void				SendOffHandViewModelActivity( Activity activity );

	virtual void		CheatImpulseCommands( int iImpulse );

	virtual void		CommitSuicide( bool bExplode = false, bool bForce = false );

	virtual void		LeaveVehicle( const Vector &vecExitPoint, const QAngle &vecExitAngles );

	// Combats
	virtual void		TraceAttack(const CTakeDamageInfo &info, const Vector &vecDir, trace_t *ptr, CDmgAccumulator *pAccumulator);
	virtual int			TakeHealth( float flHealth, int bitsDamageType );
	virtual	void		Event_KilledOther( CBaseEntity *pVictim, const CTakeDamageInfo &info );
	virtual void		Event_Killed( const CTakeDamageInfo &info );
	virtual bool		Event_Gibbed( const CTakeDamageInfo &info );
	virtual bool		BecomeRagdoll( const CTakeDamageInfo &info, const Vector &forceVector );
	void			StopRagdollDeathAnim( void );
	virtual void		PlayerDeathThink( void );

	virtual int			OnTakeDamage( const CTakeDamageInfo &inputInfo );
	virtual int			OnTakeDamage_Alive( const CTakeDamageInfo &info );
	void				ApplyPushFromDamage( const CTakeDamageInfo &info, Vector &vecDir );
	void				SetBlastJumpState( int iJumpType, bool bPlaySound );
	void				ClearBlastJumpState( void );
	int					GetBlastJumpFlags( void ) { return m_nBlastJumpFlags; }
	void				AddDamagerToHistory( EHANDLE hDamager );
	void				ClearDamagerHistory();
	DamagerHistory_t	&GetDamagerHistory( int i ) { return m_DamagerHistory[i]; }
	virtual void		DamageEffect(float flDamage, int fDamageType);
	virtual	bool		ShouldCollide( int collisionGroup, int contentsMask ) const;

	void				SetHealthBuffTime( float flTime )		{ m_flHealthBuffTime = flTime; }

	bool			CanShowClassMenu( void );

	CTFWeaponBase		*GetActiveTFWeapon( void ) const;
	bool				IsActiveTFWeapon(int iWeaponID);

	CEconItemView		*GetLoadoutItem( int iClass, int iSlot );
	void				HandleCommand_WeaponPreset(int iSlotNum, int iPresetNum);
	void				HandleCommand_WeaponPreset(int iClass, int iSlotNum, int iPresetNum);

	CBaseEntity			*GiveNamedItem( const char *pszName, int iSubType = 0, CEconItemView* pItem = NULL, int iAmmo = TF_GIVEAMMO_NONE );
	CBaseEntity			*GiveEconItem( const char *pszName, int iSubType = 0, int iAmmo = 0 );

	void				SaveMe( void );

	void				FireBullet( const FireBulletsInfo_t &info, bool bDoEffects, int nDamageType, int nCustomDamageType = TF_DMG_CUSTOM_NONE );
	void				ImpactWaterTrace( trace_t &trace, const Vector &vecStart );
	void				NoteWeaponFired();

	virtual void			OnMyWeaponFired( CBaseCombatWeapon *weapon );

	bool				HasItem( void ) const;					// Currently can have only one item at a time.
	void				SetItem( CTFItem *pItem );
	CTFItem				*GetItem( void ) const;

	void				Regenerate( void );
	void				Restock( bool bRestoreHealth, bool bRestoreAmmo );
	float				GetNextRegenTime( void ){ return m_flNextRegenerateTime; }
	void				SetNextRegenTime( float flTime ){ m_flNextRegenerateTime = flTime; }

	float				GetNextChangeClassTime(void){ return m_flNextChangeClassTime; }
	void				SetNextChangeClassTime(float flTime){ m_flNextChangeClassTime = flTime; }

	float				GetNextChangeTeamTime(void){ return m_flNextChangeTeamTime; }
	void				SetNextChangeTeamTime(float flTime){ m_flNextChangeTeamTime = flTime; }

	virtual	void		RemoveAllItems( bool removeSuit );
	virtual void		RemoveAllWeapons( void );

	bool				DropCurrentWeapon( void );
	void				DropFlag( void );
	void				TFWeaponRemove( int iWeaponID );
	bool				TFWeaponDrop( CTFWeaponBase *pWeapon, bool bThrowForward );

	// Class.
	CTFPlayerClass		*GetPlayerClass( void ) 					{ return &m_PlayerClass; }
	int					GetDesiredPlayerClassIndex( void )			{ return m_Shared.m_iDesiredPlayerClass; }
	void				SetDesiredPlayerClassIndex( int iClass )	{ m_Shared.m_iDesiredPlayerClass = iClass; }

	// Team.
	void				ForceChangeTeam( int iTeamNum );
	virtual void		ChangeTeam( int iTeamNum );

	// mp_fadetoblack
	void				HandleFadeToBlack( void );

	// Flashlight controls for SFM - JasonM
	virtual int FlashlightIsOn( void );
	virtual void FlashlightTurnOn( void );
	virtual void FlashlightTurnOff( void );

	// Think.
	virtual void		PreThink();
	virtual void		PostThink();

	virtual void		ItemPostFrame();
	virtual void		Weapon_FrameUpdate( void );
	virtual void		Weapon_HandleAnimEvent( animevent_t *pEvent );
	virtual bool		Weapon_ShouldSetLast( CBaseCombatWeapon *pOldWeapon, CBaseCombatWeapon *pNewWeapon );

	virtual void		GetStepSoundVelocities( float *velwalk, float *velrun );
	virtual void		SetStepSoundTime( stepsoundtimes_t iStepSoundTime, bool bWalking );

	// Utility.
	void				UpdateModel( void );
	void				UpdateSkin( int iTeam );

	virtual int			GiveAmmo( int iCount, int iAmmoIndex, bool bSuppressSound = false );
	virtual int			GiveAmmo( int iCount, int iAmmoIndex, bool bSuppressSound, EAmmoSource ammosource );
	int				GetMaxAmmo( int iAmmoIndex, int iClassNumber = -1 );
	virtual void			RemoveAmmo( int iCount, int iAmmoIndex );
	virtual int			GetAmmoCount( int iAmmoIndex ) const;

	bool				HasInfiniteAmmo() const;

	bool				CanAttack( void );

	// This passes the event to the client's and server's CPlayerAnimState.
	void				DoAnimationEvent( PlayerAnimEvent_t event, int mData = 0 );

	virtual bool		ClientCommand( const CCommand &args );
	void				ClientHearVox( const char *pSentence );
	void				DisplayLocalItemStatus( CTFGoal *pGoal );

	bool				CanPickupBuilding( CBaseObject *pObject );
	bool				TryToPickupBuilding( void );

	int					BuildObservableEntityList( void );
	virtual int			GetNextObserverSearchStartPoint( bool bReverse ); // Where we should start looping the player list in a FindNextObserverTarget call
	virtual CBaseEntity *FindNextObserverTarget(bool bReverse);
	virtual bool		IsValidObserverTarget(CBaseEntity * target); // true, if player is allowed to see this target
	virtual bool		SetObserverTarget(CBaseEntity * target);
	virtual bool		ModeWantsSpectatorGUI( int iMode ) { return (iMode != OBS_MODE_FREEZECAM && iMode != OBS_MODE_DEATHCAM); }
	void				FindInitialObserverTarget( void );
	CBaseEntity		    *FindNearestObservableTarget( Vector vecOrigin, float flMaxDist );
	virtual void		ValidateCurrentObserverTarget( void );

	void CheckUncoveringSpies( CTFPlayer *pTouchedPlayer );
	void Touch( CBaseEntity *pOther );

	void TeamFortress_SetSpeed();
	EHANDLE TeamFortress_GetDisguiseTarget( int nTeam, int nClass );

	void TeamFortress_ClientDisconnected();
	void RemoveAllOwnedEntitiesFromWorld( bool bSilent = true );
	void RemoveOwnedProjectiles( void );

	Vector EstimateProjectileImpactPosition( CTFWeaponBaseGun *weapon );						// estimate where a projectile fired from the given weapon will initially hit (it may bounce on from there)
	Vector EstimateProjectileImpactPosition( float pitch, float yaw, float initVel );			// estimate where a projectile fired will initially hit (it may bounce on from there)
	Vector EstimateStickybombProjectileImpactPosition( float pitch, float yaw, float charge );	// Estimate where a stickybomb projectile will hit, using given pitch, yaw, and weapon charge (0-1)

	CTFTeamSpawn *GetSpawnPoint( void ){ return m_pSpawnPoint; }
		
	void SetAnimation( PLAYER_ANIM playerAnim );

	bool IsAnyEnemySentryAbleToAttackMe();

	bool IsPlayerClass( int iClass ) const;

	void PlayFlinch( const CTakeDamageInfo &info );

	float PlayCritReceivedSound( void );
	void PainSound( const CTakeDamageInfo &info );
	void DeathSound( const CTakeDamageInfo &info );

	// TF doesn't want the explosion ringing sound
	virtual void			OnDamagedByExplosion( const CTakeDamageInfo &info ) { return; }

	void	OnBurnOther( CTFPlayer *pTFPlayerVictim );

	// Buildables
	void SetWeaponBuilder( CTFWeaponBuilder *pBuilder );
	CTFWeaponBuilder *GetWeaponBuilder( void );

	int GetBuildResources( void );
	void RemoveBuildResources( int iAmount );
	void AddBuildResources( int iAmount );

	bool IsBuilding( void );
	int CanBuild( int iObjectType, int iObjectMode = 0 );

	CBaseObject	*GetObject( int index ) const;
	CBaseObject *GetObjectOfType( int iObjectType, int iObjectMode = 0 ) const;
	int	GetObjectCount( void ) const;
	int GetNumObjects( int iObjectType, int iObjectMode = 0 );
	void RemoveAllObjects( bool bSilent );
	void StopPlacement( void );
	int	StartedBuildingObject( int iObjectType );
	void StoppedBuilding( int iObjectType );
	void FinishedObject( CBaseObject *pObject );
	void AddObject( CBaseObject *pObject );
	void OwnedObjectDestroyed( CBaseObject *pObject );
	void RemoveObject( CBaseObject *pObject );
	bool PlayerOwnsObject( CBaseObject *pObject );
	void DetonateOwnedObjectsOfType( int iType, int iMode = 0 );
	void StartBuildingObjectOfType( int iType, int iMode = 0);

	CTriggerAreaCapture *GetControlPointStandingOn( void );
	bool IsCapturingPoint();

	CTFTeam *GetTFTeam( void );
	CTFTeam *GetOpposingTFTeam( void );

	void TeleportEffect( void );
	void RemoveTeleportEffect( void );
	bool IsAllowedToPickUpFlag( void );
	bool HasTheFlag( void ) const;
	bool IsMiniBoss() const { return false; }

	// Death & Ragdolls.
	virtual void CreateRagdollEntity( void );
	void CreateRagdollEntity( bool bGib, bool bBurning, bool bElectrocuted, bool bOnGround, float flInvisLevel, int iDamageCustom );
	void DestroyRagdoll( void );
	CNetworkHandle( CBaseEntity, m_hRagdoll );	// networked entity handle 
	virtual bool ShouldGib( const CTakeDamageInfo &info );

	virtual void OnNavAreaChanged( CNavArea *enteredArea, CNavArea *leftArea );

	bool ShouldDropAmmoPack();

	// Dropping Ammo
	void DropAmmoPack( void );
	void DropWeapon( CTFWeaponBase *pWeapon, bool bKilled = false );
	void DropHealthPack( void );
	void DropFakeWeapon( CTFWeaponBase *pWeapon );
	void DropPowerups( ETFPowerupDropStyle dropStyle );

	bool CanDisguise( void );
	bool CanGoInvisible( void );
	void RemoveInvisibility( void );

	void RemoveDisguise( void );

	bool DoClassSpecialSkill( void );

	float GetLastDamageTime( void ) { return m_flLastDamageTime; }

	void SetClassMenuOpen( bool bIsOpen );
	bool IsClassMenuOpen( void );

	float GetCritMult( void ) { return m_Shared.GetCritMult(); }
	void  RecordDamageEvent( const CTakeDamageInfo &info, bool bKill ) { m_Shared.RecordDamageEvent(info,bKill); }

	bool GetHudClassAutoKill( void ){ return m_bHudClassAutoKill; }
	void SetHudClassAutoKill( bool bAutoKill ){ m_bHudClassAutoKill = bAutoKill; }

	bool GetMedigunAutoHeal( void ){ return m_bMedigunAutoHeal; }
	void SetMedigunAutoHeal( bool bMedigunAutoHeal ){ m_bMedigunAutoHeal = bMedigunAutoHeal; }

	bool IsCallingForMedic( void ) const; // return true if this player has called for a Medic in the last few seconds
	float GetTimeSinceCalledForMedic( void ) const;
	void NoteMedicCall( void );

	bool ShouldAutoRezoom( void ) { return m_bAutoRezoom; }
	void SetAutoRezoom( bool bAutoRezoom ) { m_bAutoRezoom = bAutoRezoom; }

	bool ShouldHoldToZoom( void ) { return m_bHoldZoom; }
	void SetHoldToZoom( bool bEnable ) { m_bHoldZoom = bEnable; }

	bool ShouldAutoReload( void ) { return m_bAutoReload; }
	void SetAutoReload( bool bAutoReload ) { m_bAutoReload = bAutoReload; }

	bool ShouldFlipViewModel( void ) { return m_bFlipViewModel; }
	void SetFlipViewModel( bool bFlip ) { m_bFlipViewModel = bFlip; }

	virtual void	ModifyOrAppendCriteria( AI_CriteriaSet& criteriaSet );

	// given a vector of points, return the point we can actually travel to the quickest (requires a nav mesh)
	CTeamControlPoint *SelectClosestControlPointByTravelDistance( CUtlVector< CTeamControlPoint * > *pointVector ) const;

	virtual bool CanHearAndReadChatFrom( CBasePlayer *pPlayer );

	Vector 	GetClassEyeHeight( void );

	void	UpdateExpression( void );
	void	ClearExpression( void );

	virtual IResponseSystem *GetResponseSystem();
	virtual bool			SpeakConceptIfAllowed( int iConcept, const char *modifiers = NULL, char *pszOutResponseChosen = NULL, size_t bufsize = 0, IRecipientFilter *filter = NULL );

	virtual bool CanSpeak( void );
	virtual void NotePlayerTalked();

	virtual bool CanSpeakVoiceCommand( void );
	virtual bool ShouldShowVoiceSubtitleToEnemy( void );
	virtual void NoteSpokeVoiceCommand( const char *pszScenePlayed );
	void	SpeakWeaponFire( int iCustomConcept = MP_CONCEPT_NONE );
	void	ClearWeaponFireScene( void );

	virtual int DrawDebugTextOverlays( void );

	virtual int CalculateTeamBalanceScore( void );
	void CalculateTeamScrambleScore( void );
	float GetTeamScrambleScore( void ) { return m_flTeamScrambleScore; }

	float m_flNextVoiceCommandTime;
	float m_flNextChatMessageTime;
	int m_iVoiceSpamCounter;
	int m_iTextSpamCounter;

	float m_flNextSpeakWeaponFire;

	bool ShouldAnnouceAchievement( void );

	virtual void		PlayStepSound( Vector &vecOrigin, surfacedata_t *psurface, float fvol, bool force );
	virtual bool		IsDeflectable( void ) { return true; }

	bool IsVIP();

	void JumpSound();

	int GetNumberOfDominations( void ) { return m_iDominations; }
	void UpdateDominationsCount( void );
	
	bool CanInstantlyRespawn();
	
	void SetRespawnEffect( const char *pszEffect )
	{
		V_strncpy( m_szRespawnEffect, pszEffect, sizeof( m_szRespawnEffect ) );
	}
	
	virtual CAttributeManager *GetAttributeManager( void ) { return &m_AttributeManager; }
	virtual CAttributeContainer *GetAttributeContainer( void ) { return NULL; }
	virtual CBaseEntity *GetAttributeOwner( void ) { return NULL; }
	virtual void ReapplyProvision( void ) { /*Do nothing*/ };

	bool GetClientConVarBoolValue( const char *pszValue );
	int GetClientConVarIntValue( const char *pszValue );
	float GetClientConVarFloatValue( const char *pszValue );

	void UpdatePlayerColor( void );
	void RemoveSpawnProtection( void );
	
	float GetTimeSinceLastInjuryByAnyEnemyTeam();

	virtual void    PlayerRunCommand( CUserCmd *ucmd, IMoveHelper *moveHelper );
	
	// Entity inputs
	void	InputIgnitePlayer( inputdata_t &inputdata );
	void	InputExtinguishPlayer( inputdata_t &inputdata );
	void	InputSpeakResponseConcept( inputdata_t &inputdata );
	void	InputSetForcedTauntCam( inputdata_t &inputdata );

	bool IsMobilePlayer()
	{
		return m_bIsMobilePlayer;
	}
	void SetIsMobilePlayer( bool bMobile )
	{
		m_bIsMobilePlayer = bMobile;
	}
public:

	CNetworkVector( m_vecPlayerColor );

	CTFPlayerShared m_Shared;

	int	    item_list;			// Used to keep track of which goalitems are 
								// affecting the player at any time.
								// GoalItems use it to keep track of their own 
								// mask to apply to a player's item_list

	float invincible_finished;
	float invisible_finished;
	float super_damage_finished;
	float radsuit_finished;

	int m_flNextTimeCheck;		// Next time the player can execute a "timeleft" command

	// TEAMFORTRESS VARIABLES
	int		no_sentry_message;
	int		no_dispenser_message;
	
	CNetworkVar( bool, m_bSaveMeParity );

	// teleporter variables
	int		no_entry_teleporter_message;
	int		no_exit_teleporter_message;

	float	m_flNextNameChangeTime;

	bool	m_bBlastLaunched;

	CNetworkVar( bool, m_bIsPlayerADev );
	CNetworkVar( bool, m_bIsPlayerChatProtected );

	int					StateGet( void ) const;

	void				SetOffHandWeapon( CTFWeaponBase *pWeapon );
	void				HolsterOffHandWeapon( void );

	float				GetSpawnTime() { return m_flSpawnTime; }

	virtual bool Weapon_Switch( CBaseCombatWeapon *pWeapon, int viewmodelindex = 0 );
	virtual void Weapon_Drop( CBaseCombatWeapon *pWeapon, const Vector *pvecTarget , const Vector *pVelocity );

	bool				ItemIsAllowed( CEconItemView *pItem );
	
	bool				ItemsMatch( CEconItemView *pItem1, CEconItemView *pItem2, CTFWeaponBase *pWeapon );
	void				ValidateWeapons( bool bRegenerate );
	void				ValidateWearables( void );
	void				ManageRegularWeapons( TFPlayerClassData_t *pData );
	void				ManageRegularWeaponsLegacy( TFPlayerClassData_t *pData );
	void				ManageRandomWeapons( TFPlayerClassData_t *pData );
	void				ManageBuilderWeapons( TFPlayerClassData_t *pData );
	void				ManageGrenades(TFPlayerClassData_t *pData);

	void				PostInventoryApplication( void );

	// Taunts.
	void				Taunt( void );
	bool				IsTaunting( void ) { return m_Shared.InCond( TF_COND_TAUNTING ); }
	void				DoTauntAttack( void );
	void				ClearTauntAttack( void );
	QAngle				m_angTauntCamera;

	virtual float		PlayScene( const char *pszScene, float flDelay = 0.0f, AI_Response *response = NULL, IRecipientFilter *filter = NULL );
	void				ResetTauntHandle( void )				{ m_hTauntScene = NULL; }
	void				SetDeathFlags( int iDeathFlags ) { m_iDeathFlags = iDeathFlags; }
	int					GetDeathFlags() { return m_iDeathFlags; }
	void				SetMaxSentryKills( int iMaxSentryKills ) { m_iMaxSentryKills = iMaxSentryKills; }
	int					GetMaxSentryKills() { return m_iMaxSentryKills; }

	CNetworkVar( int, m_iSpawnCounter );
	
	void				CheckForIdle( void );
	void				PickWelcomeObserverPoint();

	void				StopRandomExpressions( void ) { m_flNextRandomExpressionTime = -1; }
	void				StartRandomExpressions( void ) { m_flNextRandomExpressionTime = gpGlobals->curtime; }

	virtual bool			WantsLagCompensationOnEntity( const CBasePlayer	*pPlayer, const CUserCmd *pCmd, const CBitVec<MAX_EDICTS> *pEntityTransmitBits ) const;

	CBaseEntity			*MedicGetHealTarget( void );
	float				MedicGetChargeLevel( void );

	CWeaponMedigun		*GetMedigun( void );
	CTFWeaponBase		*Weapon_OwnsThisID( ETFWeaponID iWeaponID );
	CTFWeaponBase		*Weapon_GetWeaponByType( int iType );
	CEconWearable		*GetEntityForLoadoutSlot( ETFLoadoutSlot iSlot );
	CTFWeaponBase		*GetWeaponForLoadoutSlot( ETFLoadoutSlot iSlot );
	CEconWearable		*GetWearableForLoadoutSlot( ETFLoadoutSlot iSlot );

	bool CalculateAmmoPackPositionAndAngles( CTFWeaponBase *pWeapon, Vector &vecOrigin, QAngle &vecAngles );

	bool				SelectFurthestSpawnSpot( const char *pEntClassName, CBaseEntity* &pSpot, bool bTelefrag = true );

	//TF_MOD_BOT changes
	IntervalTimer m_lastCalledMedic;
	void OnSapperPlaced(CBaseEntity* sappedObject);
	bool IsPlacingSapper(void) const;

	bool IsInCombat( void ) const;

	// Client commands.
	void				HandleCommand_JoinTeam( const char *pTeamName );
	void				HandleCommand_JoinClass( const char *pClassName );
	void				HandleCommand_JoinTeam_NoMenus( const char *pTeamName );
	void				HandleCommand_JoinTeam_NoKill( const char *pTeamName );
	int				GetAutoTeam( void );
	void				ShowTeamMenu( bool bShow = true );
	void				ShowClassMenu( bool bShow = true );

	void				InitClass( void );

	bool IsThreatAimingTowardMe( CBaseEntity *threat, float cosTolerance = 0.8f ) const;
	bool IsThreatFiringAtMe( CBaseEntity *threat ) const;

private:

	// Creation/Destruction.
	void				GiveDefaultItems();
	bool				SelectSpawnSpot( const char *pEntClassName, CBaseEntity* &pSpot );
	void				PrecachePlayerModels( void );
	void				RemoveNemesisRelationships();

	// Think.
	void				TFPlayerThink();
	void				RegenThink();
	void				UpdateTimers( void );

	// Taunt.
	EHANDLE				m_hTauntScene;
	bool				m_bInitTaunt;

	//TF_MOD_BOT changes
	// Sapper events
	bool				m_bSapping;
	CountdownTimer			m_sapperTimer;
	IntervalTimer		m_calledForMedicTimer;

	// Bots.
	friend void			Bot_Think( CTFPlayer *pBot );

	// Physics.
	void				PhysObjectSleep();
	void				PhysObjectWake();

	// Ammo pack.
	void AmmoPackCleanUp( void );
	void DroppedWeaponCleanUp( void );

	// State.
	CPlayerStateInfo	*StateLookupInfo( int nState );
	void				StateEnter( int nState );
	void				StateLeave( void );
	void				StateTransition( int nState );
	void				StateEnterWELCOME( void );
	void				StateThinkWELCOME( void );
	void				StateEnterPICKINGTEAM( void );
	void				StateEnterACTIVE( void );
	void				StateEnterOBSERVER( void );
	void				StateThinkOBSERVER( void );
	void				StateEnterDYING( void );
	void				StateThinkDYING( void );

	virtual bool		SetObserverMode(int mode);
	virtual void		AttemptToExitFreezeCam( void );

	bool				PlayGesture( const char *pGestureName );
	bool				PlaySpecificSequence( const char *pSequenceName );
	bool				PlayDeathAnimation( const CTakeDamageInfo &info, CTakeDamageInfo &info_modified );

	bool				GetResponseSceneFromConcept( int iConcept, char *chSceneBuffer, int numSceneBufferBytes );

private:
	// Map introductions
	int					m_iIntroStep;
	CHandle<CIntroViewpoint> m_hIntroView;
	float				m_flIntroShowHintAt;
	float				m_flIntroShowEventAt;
	bool				m_bHintShown;
	bool				m_bAbortFreezeCam;
	bool				m_bSeenRoundInfo;
	bool				m_bRegenerating;

	// Items.
	CNetworkHandle( CTFItem, m_hItem );

	// Combat.
	CNetworkHandle( CTFWeaponBase, m_hOffHandWeapon );

	float					m_flHealthBuffTime;

	float					m_flLastHealthRegenAt;
	float					m_flNextRegenerateTime;
	float					m_flAccumulatedHealthRegen;
	float					m_flNextChangeClassTime;
	float					m_flNextChangeTeamTime;

	// Ragdolls.
	Vector					m_vecTotalBulletForce;

	// State.
	CPlayerStateInfo		*m_pStateInfo;

	// Spawn Point
	CTFTeamSpawn			*m_pSpawnPoint;

	// Networked.
	CNetworkQAngle( m_angEyeAngles );					// Copied from EyeAngles() so we can send it to the client.

	CTFPlayerClass		m_PlayerClass;
	int					m_WeaponPreset[TF_CLASS_COUNT_ALL][TF_LOADOUT_SLOT_COUNT];

	CTFPlayerAnimState	*m_PlayerAnimState;
	int					m_iLastWeaponFireUsercmd;				// Firing a weapon.  Last usercmd we shot a bullet on.
	int					m_iLastSkin;

	CNetworkVar( float, m_flLastDamageTime );

	float				m_flNextPainSoundTime;
	int					m_LastDamageType;
	int					m_iDeathFlags;				// TF_DEATH_* flags with additional death info
	int					m_iMaxSentryKills;			// most kills by a single sentry

	bool				m_bPlayedFreezeCamSound;

	CHandle< CTFWeaponBuilder > m_hWeaponBuilder;

	CUtlVector<EHANDLE>	m_aObjects;			// List of player objects

	bool m_bIsClassMenuOpen;

	Vector m_vecLastDeathPosition;

	float				m_flSpawnTime;

	float				m_flLastAction;
	bool				m_bIsIdle;

	CUtlVector<EHANDLE>	m_hObservableEntities;
	DamagerHistory_t m_DamagerHistory[MAX_DAMAGER_HISTORY];	// history of who has damaged this player
	CUtlVector<float>	m_aBurnOtherTimes;					// vector of times this player has burned others

	// Background expressions
	string_t			m_iszExpressionScene;
	EHANDLE				m_hExpressionSceneEnt;
	float				m_flNextRandomExpressionTime;
	EHANDLE				m_hWeaponFireSceneEnt;

	bool				m_bSpeakingConceptAsDisguisedSpy;

	bool				m_bHudClassAutoKill;
	bool 				m_bMedigunAutoHeal;
	bool				m_bAutoRezoom;	// does the player want to re-zoom after each shot for sniper rifles
	bool				m_bAutoReload;
	bool				m_bFlipViewModel;

	bool				m_bHoldZoom;
	
	bool m_bIsMobilePlayer;

	float				m_flTauntAttackTime;
	int					m_iTauntAttack;

	float				m_flNextCarryTalkTime;

	int					m_nBlastJumpFlags;
	bool				m_bJumpEffect;

	int				m_iDominations;
	
	char m_szRespawnEffect[128];

	CNetworkVar( int, m_nForceTauntCam );
	CNetworkVar( bool, m_bTyping );

	CNetworkVarEmbedded( CAttributeManager, m_AttributeManager );

	float m_flTeamScrambleScore;

	CountdownTimer m_ctNavCombatUpdate;
	
	COutputEvent		m_OnDeath;

public:
	bool				SetPowerplayEnabled( bool bOn );
	bool				PlayerHasPowerplay( void );
	bool				PlayerHasChatProtection();
	void				PowerplayThink( void );
	float				m_flPowerPlayTime;
};

//TF_MOD_BOT changes
inline void CTFPlayer::OnSapperPlaced(CBaseEntity* sappedObject)
{
	m_sapperTimer.Start(3.0f);
}

inline bool CTFPlayer::IsPlacingSapper(void) const
{
	return !m_sapperTimer.IsElapsed();
}

//-----------------------------------------------------------------------------
// Purpose: Utility function to convert an entity into a tf player.
//   Input: pEntity - the entity to convert into a player
//-----------------------------------------------------------------------------
inline CTFPlayer *ToTFPlayer( CBaseEntity *pEntity )
{
	if ( !pEntity || !pEntity->IsPlayer() )
		return NULL;

	Assert( dynamic_cast<CTFPlayer*>( pEntity ) != 0 );
	return static_cast< CTFPlayer* >( pEntity );
}

inline int CTFPlayer::StateGet( void ) const
{
	return m_Shared.m_nPlayerState;
}

inline bool CTFPlayer::IsInCombat( void ) const
{
	// the simplest condition is whether we've been firing our weapon very recently
	return GetTimeSinceWeaponFired() < 2.0f;
}

inline bool CTFPlayer::IsCallingForMedic( void ) const
{
	return m_calledForMedicTimer.HasStarted() && m_calledForMedicTimer.IsLessThen( 5.0f );
}

inline float CTFPlayer::GetTimeSinceCalledForMedic() const
{
	return m_calledForMedicTimer.GetElapsedTime();
}

inline void CTFPlayer::NoteMedicCall( void )
{
	m_calledForMedicTimer.Start();
}



#endif	// TF_PLAYER_H
