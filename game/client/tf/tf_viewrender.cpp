//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: Responsible for drawing the scene
//
// $NoKeywords: $
//=============================================================================

#include "cbase.h"
#include "iviewrender.h"
#include "view_shared.h"
#include "tf_viewrender.h"
#include "viewpostprocess.h"
#include <game/client/iviewport.h>

#include "tf_hud_screenoverlays.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

static CTFViewRender g_ViewRender;

CTFViewRender::CTFViewRender()
{
	view = ( IViewRender * )this;
}

void CTFViewRender::Render2DEffectsPreHUD( const CViewSetup &view )
{
	BaseClass::Render2DEffectsPreHUD( view );
	g_ScreenOverlayManager.DrawOverlays( view );
}

//-----------------------------------------------------------------------------
// Purpose: Renders extra 2D effects in derived classes while the 2D view is on the stack
//-----------------------------------------------------------------------------
void CTFViewRender::Render2DEffectsPostHUD( const CViewSetup &view )
{
	BaseClass::Render2DEffectsPostHUD( view );

	// if we're in the intro menus
	if ( gViewPortInterface->GetActivePanel() != NULL )
	{
		DoEnginePostProcessing( view.x, view.y, view.width, view.height, false, true );
	}
}
