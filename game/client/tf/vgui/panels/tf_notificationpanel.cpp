#include "cbase.h"
#include "tf_notificationpanel.h"
#include "tf_mainmenupanel.h"
#include "tf_mainmenu.h"
#include <vgui_controls/TextImage.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFNotificationPanel::CTFNotificationPanel( vgui::Panel* parent, const char *panelName ) : CTFMenuPanelBase( parent, panelName )
{
	m_pPrevButton = NULL;
	m_pNextButton = NULL;
	m_pMessageLabel = NULL;

	m_iOrgPrevPos = 0;
	m_iOrgNextPos = 0;
	m_iCurrent = 0;
	m_iCount = 0;
	m_iMinHeight = 0;
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFNotificationPanel::~CTFNotificationPanel()
{

}

void CTFNotificationPanel::ApplySchemeSettings( vgui::IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	LoadControlSettings( "resource/UI/main_menu/NotificationPanel.res" );

	m_pPrevButton = dynamic_cast<CTFButton *>( FindChildByName( "PrevButton" ) );
	m_pNextButton = dynamic_cast<CTFButton *>( FindChildByName( "NextButton" ) );
	m_pMessageLabel = dynamic_cast<CExLabel *>( FindChildByName( "MessageLabel" ) );

	m_iMinHeight = GetTall();
	m_iOrgPrevPos = m_pPrevButton ? m_pPrevButton->GetYPos() : 0;
	m_iOrgNextPos = m_pNextButton ? m_pNextButton->GetYPos() : 0;

	UpdateLabels();
}

void CTFNotificationPanel::PerformLayout()
{
	BaseClass::PerformLayout();

	// Adjust message box size based on text height.
	int iWindowHeight = m_iMinHeight;

	if ( m_pMessageLabel )
	{
		int x, y, wide, tall;
		m_pMessageLabel->GetPos( x, y );
		m_pMessageLabel->GetTextImage()->GetContentSize( wide, tall );

		// A bit of space for arrows.
		iWindowHeight = MAX( iWindowHeight, ( y + tall + YRES( 30 ) ) );

		m_pMessageLabel->SetSize( m_pMessageLabel->GetWide(), tall );
	}

	SetSize( GetWide(), iWindowHeight );

	if ( m_pPrevButton )
	{
		m_pPrevButton->SetPos( m_pPrevButton->GetXPos(), m_iOrgPrevPos + ( iWindowHeight - m_iMinHeight ) );
	}
	if ( m_pNextButton )
	{
		m_pNextButton->SetPos( m_pNextButton->GetXPos(), m_iOrgNextPos + ( iWindowHeight - m_iMinHeight ) );
	}
};


void CTFNotificationPanel::OnNotificationUpdate()
{
	m_iCount = GetNotificationManager()->GetNotificationsCount();
	if ( !IsVisible() )
	{
		for ( int i = 0; i < m_iCount; i++ )
		{
			MessageNotification *pNotification = GetNotificationManager()->GetNotification( i );
			if ( pNotification->bUnread )
			{
				m_iCurrent = i;
				break;
			}
		}
	}

	UpdateLabels();
};

void CTFNotificationPanel::UpdateLabels()
{
	m_iCount = GetNotificationManager()->GetNotificationsCount();
	if ( m_iCount <= 0 )
	{
		Hide();
		return;
	}
	if ( m_iCurrent >= m_iCount )
		m_iCurrent = m_iCount - 1;

	m_pNextButton->SetVisible( ( m_iCurrent < m_iCount - 1 ) );
	m_pPrevButton->SetVisible( ( m_iCurrent > 0 ) );

	SetDialogVariable( "current", m_iCurrent + 1 );
	SetDialogVariable( "count", m_iCount );

	MessageNotification *pNotification = GetNotificationManager()->GetNotification( m_iCurrent );

	SetDialogVariable( "title", pNotification->wszTitle );
	SetDialogVariable( "message", pNotification->wszMessage );
	SetDialogVariable( "timestamp", pNotification->wszDate );
	
	if ( IsVisible() )
	{
		pNotification->bUnread = false;
	}

	InvalidateLayout( true );
}

void CTFNotificationPanel::RemoveCurrent()
{
	GetNotificationManager()->RemoveNotification( m_iCurrent );
	UpdateLabels();
}

void CTFNotificationPanel::Show()
{
	BaseClass::Show();

	UpdateLabels();
}

void CTFNotificationPanel::Hide()
{
	BaseClass::Hide();
}

void CTFNotificationPanel::OnCommand( const char* command )
{
	if ( !Q_strcmp( command, "vguicancel" ) )
	{
		Hide();
	}
	else if ( !stricmp( command, "Ok" ) )
	{
		Hide();
	}
	else if ( !stricmp( command, "Next" ) )
	{
		if ( m_iCurrent < m_iCount - 1 )
			m_iCurrent++;
		UpdateLabels();
	}
	else if ( !stricmp( command, "Prev" ) )
	{
		if ( m_iCurrent > 0 )
			m_iCurrent--;
		UpdateLabels();
	}
	else if ( !stricmp( command, "Remove" ) )
	{
		RemoveCurrent();
	}
	else
	{
		BaseClass::OnCommand( command );
	}
}

void CTFNotificationPanel::OnTick()
{
	BaseClass::OnTick();
}

void CTFNotificationPanel::OnThink()
{
	BaseClass::OnThink();
}
