#include "cbase.h"
#include "tf_dialogpanelbase.h"
#include "tf_mainmenu.h"
#include "controls/tf_advbutton.h"
#include "controls/tf_advpanellistpanel.h"
#include "controls/tf_scriptobject.h"
#include "controls/tf_cvartogglecheckbutton.h"
#include "controls/tf_cvarslider.h"
#include "vgui_controls/ComboBox.h"
#include "vgui_controls/Tooltip.h"
#include "inputsystem/iinputsystem.h"

#include <KeyValues.h>
#include <vgui/IScheme.h>
#include "tier1/convar.h"
#include <stdio.h>
#include <vgui_controls/TextEntry.h>
// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

using namespace vgui;

CTFDialogPanelBase::CTFDialogPanelBase(vgui::Panel *parent, const char *panelName) : CTFMenuPanelBase(parent, panelName)
{
	Init();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CTFDialogPanelBase::~CTFDialogPanelBase()
{
	DestroyControls();
}

bool CTFDialogPanelBase::Init()
{
	BaseClass::Init();
	SetKeyBoardInputEnabled(true);
	m_pListPanel = NULL;
	bEmbedded = false;
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: sets background color & border
//-----------------------------------------------------------------------------
void CTFDialogPanelBase::ApplySchemeSettings(IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);
	if (bEmbedded)
	{
		OnCreateControls();
	}
	else
	{
		//Show();
	}

}

void CTFDialogPanelBase::PerformLayout()
{
	BaseClass::PerformLayout();
	m_cShotcutKeys.RemoveAll();
};

void CTFDialogPanelBase::OnCommand(const char* command)
{
	if (!Q_strcmp(command, "vguicancel"))
	{
		PostActionSignal(new KeyValues("CancelPressed"));
		OnResetData();
		Hide();
	}
	else if (!stricmp(command, "Ok"))
	{
		PostActionSignal(new KeyValues("OkPressed"));
		OnApplyChanges();
		Hide();
	}
	else
	{
		BaseClass::OnCommand(command);
	}
}

void CTFDialogPanelBase::Show()
{
	BaseClass::Show();
	if (!bEmbedded)
	{
		RequestFocus();
		MakePopup();
	}
	vgui::GetAnimationController()->RunAnimationCommand(this, "Alpha", 255, 0.05f, 0.3f, vgui::AnimationController::INTERPOLATOR_SIMPLESPLINE);
	int _x, _y;
	GetPos(_x, _y);
	SetPos(_x - YRES(20), _y);
	AnimationController::PublicValue_t p_AnimHover(_x, _y);
	vgui::GetAnimationController()->RunAnimationCommand(this, "Position", p_AnimHover, 0.0f, 0.3f, vgui::AnimationController::INTERPOLATOR_SIMPLESPLINE, NULL);
	MAINMENU_ROOT->ShowPanel(SHADEBACKGROUND_MENU);
};

void CTFDialogPanelBase::Hide()
{
	BaseClass::Hide();
	vgui::GetAnimationController()->RunAnimationCommand(this, "Alpha", 0, 0.0f, 0.1f, vgui::AnimationController::INTERPOLATOR_LINEAR);
	MAINMENU_ROOT->HidePanel(SHADEBACKGROUND_MENU);
	MAINMENU_ROOT->ShowPanel(CURRENT_MENU);
	if (bShowSingle)
	{
		engine->ClientCmd("gameui_hide");
	}
};

void CTFDialogPanelBase::OnKeyCodePressed(vgui::KeyCode code)
{
	BaseClass::OnKeyCodePressed(code);

	if (code == KEY_ESCAPE)
	{
		Hide();
	}
	else if (!bEmbedded)
	{
		const char *keyName = g_pInputSystem->ButtonCodeToString(code);
		if (Q_strlen(keyName) == 1)
		{
			unsigned int id = m_cShotcutKeys.Find(g_pInputSystem->ButtonCodeToString(code));
			if (id < m_cShotcutKeys.Count())
			{
				const char* cCommand = m_cShotcutKeys[id];
				if (Q_strcmp(cCommand, ""))
					OnCommand(cCommand);
			}
		}
	}
	
}

void CTFDialogPanelBase::AddControl( vgui::Panel* panel, int iType, const char* text )
{
	if ( !m_pListPanel )
		return;

	mpcontrol_t	*pCtrl = new mpcontrol_t( m_pListPanel, "mpcontrol_t" );
	HFont hFont = GETSCHEME()->GetFont( m_pListPanel->GetFontString(), true );

	switch ( iType )
	{
	case O_CATEGORY:
	{
		Label *pTitle = assert_cast<Label*>( panel );
		pTitle->MakeReadyForUse();

		pTitle->SetFont( GETSCHEME()->GetFont( ADVBUTTON_DEFAULT_FONT, true ) );
		pTitle->SetBorder( GETSCHEME()->GetBorder( "AdvSettingsTitleBorder" ) );
		pTitle->SetFgColor( GETSCHEME()->GetColor( ADVBUTTON_DEFAULT_COLOR, COLOR_WHITE ) );
		break;
	}
	case O_BOOL:
	{
		CTFCheckButton *pBox = assert_cast<CTFCheckButton*>( panel );
		pBox->MakeReadyForUse();

		pBox->SetFont( hFont );
		//pBox->SetToolTip(dynamic_cast<CTFAdvCheckButton*>(panel)->GetName());
		break;
	}
	case O_SLIDER:
	{
		CTFSlider *pScroll = assert_cast<CTFSlider*>( panel );
		pScroll->MakeReadyForUse();

		pScroll->SetFont( hFont );
		//pScroll->SetToolTip(dynamic_cast<CTFAdvSlider*>(panel)->GetName());
		break;
	}
	case O_LIST:
	{
		ComboBox *pCombo = assert_cast<ComboBox*>( panel );
		pCombo->MakeReadyForUse();
		pCombo->SetFont( hFont );

		pCtrl->pPrompt = new vgui::Label( pCtrl, "DescLabel", "" );
		pCtrl->pPrompt->MakeReadyForUse();

		pCtrl->pPrompt->SetFont( hFont );
		pCtrl->pPrompt->SetContentAlignment( vgui::Label::a_west );
		pCtrl->pPrompt->SetTextInset( 5, 0 );
		pCtrl->pPrompt->SetText( text );
		break;
	}
	default:
		break;
	}

	panel->SetParent( pCtrl );
	pCtrl->pControl = panel;
	int h = m_pListPanel->GetTall() / 13.0; //(float)GetParent()->GetTall() / 15.0;
	pCtrl->SetSize( 800, h );
	m_pListPanel->AddItem( pCtrl );
}

void CTFDialogPanelBase::CreateControls()
{
	DestroyControls();
}

void CTFDialogPanelBase::DestroyControls()
{
	if (!m_pListPanel)
		return;

	m_pListPanel->DeleteAllItems();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFDialogPanelBase::OnResetData()
{

}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFDialogPanelBase::OnApplyChanges()
{

}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFDialogPanelBase::OnSetDefaults()
{

}

void CTFDialogPanelBase::OnThink()
{

}