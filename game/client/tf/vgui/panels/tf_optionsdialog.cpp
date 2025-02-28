//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "tf_optionsdialog.h"
#include "tf_mainmenu.h"

#include "vgui_controls/Button.h"
#include "vgui_controls/CheckButton.h"
#include "vgui_controls/PropertySheet.h"
#include "vgui_controls/Label.h"
#include "vgui_controls/QueryBox.h"
#include "controls/tf_advbutton.h"

#include "vgui/ILocalize.h"
#include "vgui/ISurface.h"
#include "vgui/ISystem.h"
#include "vgui/IVGui.h"

#include "KeyValues.h"
#include "tf_mainmenupanel.h"
#include "tf_optionsmousepanel.h"
#include "tf_optionstouchpanel.h"
#include "tf_optionskeyboardpanel.h"
#include "tf_optionsaudiopanel.h"
#include "tf_optionsvideopanel.h"
#include "tf_optionsadvancedpanel.h"

using namespace vgui;

// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

//-----------------------------------------------------------------------------
// Purpose: Basic help dialog
//-----------------------------------------------------------------------------
CTFOptionsDialog::CTFOptionsDialog( vgui::Panel *parent, const char *panelName ) : CTFDialogPanelBase( parent, panelName )
{
	m_pPanels.SetSize( PANEL_COUNT );

	AddPanel( new CTFOptionsAdvancedPanel( this, "MultiplayerOptions" ), PANEL_ADV );
	AddPanel( new CTFOptionsMousePanel( this, "MouseOptions" ), PANEL_MOUSE );
	AddPanel( new CTFOptionsTouchPanel( this, "TouchOptions" ), PANEL_TOUCH );
	AddPanel( new CTFOptionsKeyboardPanel( this, "KeyboardOptions" ), PANEL_KEYBOARD );
	AddPanel( new CTFOptionsAudioPanel( this, "AudioOptions" ), PANEL_AUDIO );
	AddPanel( new CTFOptionsVideoPanel( this, "VideoOptions" ), PANEL_VIDEO );

	m_pApplyButton = new CTFButton( this, "Apply", "" );
	m_pDefaultsButton = new CTFButton( this, "Defaults", "" );

	m_pOptionsCurrent = PANEL_ADV;
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFOptionsDialog::~CTFOptionsDialog()
{
}

void CTFOptionsDialog::AddPanel( CTFDialogPanelBase *m_pPanel, int iPanel )
{
	m_pPanels[iPanel] = m_pPanel;
	m_pPanel->SetEmbedded( true );
	m_pPanel->SetVisible( false );
}

void CTFOptionsDialog::SetCurrentPanel( OptionPanel pCurrentPanel )
{
	if ( m_pOptionsCurrent == pCurrentPanel )
		return;
	GetPanel( m_pOptionsCurrent )->Hide();
	GetPanel( pCurrentPanel )->Show();
	m_pOptionsCurrent = pCurrentPanel;

	m_pDefaultsButton->SetVisible( ( pCurrentPanel == PANEL_KEYBOARD ) );
}

CTFDialogPanelBase*	CTFOptionsDialog::GetPanel( int iPanel )
{
	return m_pPanels[iPanel];
}

void CTFOptionsDialog::ApplySchemeSettings( vgui::IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	LoadControlSettings( "resource/UI/main_menu/OptionsDialog.res" );
}

void CTFOptionsDialog::Show()
{
	BaseClass::Show();
	GetPanel( m_pOptionsCurrent )->Show();

	for ( int i = 0; i < PANEL_COUNT; i++ )
		GetPanel( i )->OnCreateControls();

	// Disable apply button, it'll get enabled once any controls change.
	m_pApplyButton->SetEnabled( false );
}

void CTFOptionsDialog::Hide()
{
	BaseClass::Hide();
	GetPanel( m_pOptionsCurrent )->Hide();

	for ( int i = 0; i < PANEL_COUNT; i++ )
		GetPanel( i )->OnDestroyControls();
};

void CTFOptionsDialog::OnCommand( const char* command )
{
	if ( !V_stricmp( command, "vguicancel" ) )
	{
		OnCancelPressed();
	}
	else if ( !V_stricmp( command, "Ok" ) )
	{
		OnOkPressed();
	}
	else if ( !V_stricmp( command, "Apply" ) )
	{
		OnApplyPressed();
	}
	else if ( !V_stricmp( command, "DefaultsOK" ) )
	{
		OnDefaultPressed();
	}
	else if ( !V_stricmp( command, "newoptionsadv" ) )
	{
		SetCurrentPanel( PANEL_ADV );
	}
	else if ( !V_stricmp( command, "newoptionsmouse" ) )
	{
		SetCurrentPanel( PANEL_MOUSE );
	}
	else if ( !V_stricmp( command, "newoptionstouch" ) )
	{
		SetCurrentPanel( PANEL_TOUCH );
	}
	else if ( !V_stricmp( command, "newoptionskeyboard" ) )
	{
		SetCurrentPanel( PANEL_KEYBOARD );
	}
	else if ( !V_stricmp( command, "newoptionsaudio" ) )
	{
		SetCurrentPanel( PANEL_AUDIO );
	}
	else if ( !V_stricmp( command, "newoptionsvideo" ) )
	{
		SetCurrentPanel( PANEL_VIDEO );
	}
	else
	{
		BaseClass::OnCommand( command );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFOptionsDialog::OnOkPressed()
{
	OnApplyPressed();
	Hide();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFOptionsDialog::OnApplyPressed()
{
	m_pApplyButton->SetEnabled( false );

	for ( int i = 0; i < PANEL_COUNT; i++ )
		GetPanel( i )->OnApplyChanges();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFOptionsDialog::OnCancelPressed()
{
	for ( int i = 0; i < PANEL_COUNT; i++ )
		GetPanel( i )->OnResetData();

	Hide();
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFOptionsDialog::OnDefaultPressed()
{
	for ( int i = 0; i < PANEL_COUNT; i++ )
		GetPanel( i )->OnSetDefaults();
}

//-----------------------------------------------------------------------------
// Purpose: Called when the GameUI is hidden
//-----------------------------------------------------------------------------
void CTFOptionsDialog::OnGameUIHidden()
{
	// tell our children about it
	for ( int i = 0; i < GetChildCount(); i++ )
	{
		Panel *pChild = GetChild( i );
		if ( pChild )
		{
			PostMessage( pChild, new KeyValues( "GameUIHidden" ) );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Enable apply button when something is changed.
//-----------------------------------------------------------------------------
void CTFOptionsDialog::OnApplyButtonEnabled( void )
{
	if ( !m_pApplyButton->IsEnabled() )
	{
		m_pApplyButton->SetEnabled( true );
	}
}
