/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/

#include <sfx2/sidebar/propertypanel.hrc>
#include <sfx2/sidebar/Theme.hxx>
#include <sfx2/sidebar/ControlFactory.hxx>
#include <GraphicPropertyPanel.hxx>
#include <GraphicPropertyPanel.hrc>
#include <svx/dialogs.hrc>
#include <svx/dialmgr.hxx>
#include <vcl/field.hxx>
#include <vcl/lstbox.hxx>
#include <svl/intitem.hxx>
#include <sfx2/bindings.hxx>
#include <sfx2/dispatch.hxx>

using namespace css;
using namespace cssu;
using ::sfx2::sidebar::Theme;

#define A2S(pString) (::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM(pString)))

//////////////////////////////////////////////////////////////////////////////
// namespace open

namespace svx { namespace sidebar {

//////////////////////////////////////////////////////////////////////////////

GraphicPropertyPanel::GraphicPropertyPanel(
    Window* pParent,
    const cssu::Reference<css::frame::XFrame>& rxFrame,
    SfxBindings* pBindings)
:   Control(
        pParent, 
        SVX_RES(RID_SIDEBAR_GRAPHIC_PANEL)),
    mpFtBrightness(new FixedText(this, SVX_RES(FT_BRIGHTNESS))),
    mpMtrBrightness(new MetricField(this, SVX_RES(MTR_BRIGHTNESS))),
    mpFtContrast(new FixedText(this, SVX_RES(FT_CONTRAST))),
    mpMtrContrast(new MetricField(this, SVX_RES(MTR_CONTRAST))),
    mpFtColorMode(new FixedText(this, SVX_RES(FT_COLOR_MODE))),
    mpLBColorMode(new ListBox(this, SVX_RES(LB_COLOR_MODE))),
    mpFtTrans(new FixedText(this, SVX_RES(FT_TRANSPARENT))),
    mpMtrTrans(new MetricField(this, SVX_RES(MTR_TRANSPARENT))),
    mpMtrRed(new MetricField(this, SVX_RES(MF_RED))),
    mpMtrGreen(new MetricField(this, SVX_RES(MF_GREEN))),
    mpMtrBlue(new MetricField(this, SVX_RES(MF_BLUE))),
    mpMtrGamma(new MetricField(this, SVX_RES(MF_GAMMA))),
    maBrightControl(SID_ATTR_GRAF_LUMINANCE, *pBindings, *this),
    maContrastControl(SID_ATTR_GRAF_CONTRAST, *pBindings, *this),
    maTransparenceControl(SID_ATTR_GRAF_TRANSPARENCE, *pBindings, *this),
    maRedControl(SID_ATTR_GRAF_RED, *pBindings, *this),
    maGreenControl(SID_ATTR_GRAF_GREEN, *pBindings, *this),
    maBlueControl(SID_ATTR_GRAF_BLUE, *pBindings, *this),
    maGammaControl(SID_ATTR_GRAF_GAMMA, *pBindings, *this),
    maModeControl(SID_ATTR_GRAF_MODE, *pBindings, *this),
    maImgNormal(SVX_RES(IMG_NORMAL)),
    maImgBW(SVX_RES(IMG_BW)),
    maImgGray(SVX_RES(IMG_GRAY)),
    maImgWater(SVX_RES(IMG_WATER)),
    maImgRed(this, SVX_RES(IMG_RED)),
    maImgGreen(this, SVX_RES(IMG_GREEN)),
    maImgBlue(this, SVX_RES(IMG_BLUE)),
    maImgGamma(this, SVX_RES(IMG_GAMMA)),
    msNormal(SVX_RES(STR_NORMAL)),
    msBW(SVX_RES(STR_BW)),
    msGray(SVX_RES(STR_GRAY)),
    msWater(SVX_RES(STR_WATER)),
    mxFrame(rxFrame),
    mpBindings(pBindings)
{
    Initialize();
    FreeResource();
}

//////////////////////////////////////////////////////////////////////////////

GraphicPropertyPanel::~GraphicPropertyPanel()
{
}

//////////////////////////////////////////////////////////////////////////////

void GraphicPropertyPanel::Initialize()
{
    mpFtBrightness->SetBackground(Wallpaper());
    mpFtContrast->SetBackground(Wallpaper());
    mpFtColorMode->SetBackground(Wallpaper());
    mpFtTrans->SetBackground(Wallpaper());

    mpMtrBrightness->SetModifyHdl( LINK( this, GraphicPropertyPanel, ModifyBrightnessHdl ) );
	mpMtrBrightness->SetAccessibleName(::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Brightness")));	//wj acc
	mpMtrContrast->SetModifyHdl( LINK( this, GraphicPropertyPanel, ModifyContrastHdl ) );
	mpMtrContrast->SetAccessibleName(::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Contrast")));	//wj acc
	mpMtrTrans->SetModifyHdl( LINK( this, GraphicPropertyPanel, ModifyTransHdl ) );
	mpMtrTrans->SetAccessibleName(::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Transparency")));	//wj acc

	mpLBColorMode->InsertEntry( msNormal, maImgNormal );
	mpLBColorMode->InsertEntry( msGray, maImgGray );
	mpLBColorMode->InsertEntry( msBW, maImgBW );
	mpLBColorMode->InsertEntry( msWater, maImgWater );
	mpLBColorMode->SetSelectHdl( LINK( this, GraphicPropertyPanel, ClickColorModeHdl ));
	mpLBColorMode->SetAccessibleName(::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Color mode")));	//wj acc

	mpMtrRed->SetModifyHdl( LINK( this, GraphicPropertyPanel, RedHdl ) );
	mpMtrGreen->SetModifyHdl( LINK( this, GraphicPropertyPanel, GreenHdl ) );
	mpMtrBlue->SetModifyHdl( LINK( this, GraphicPropertyPanel, BlueHdl ) );
	mpMtrGamma->SetModifyHdl( LINK( this, GraphicPropertyPanel, GammaHdl ) );
	mpMtrRed->SetAccessibleName(mpMtrRed->GetQuickHelpText());	//wj acc
	mpMtrGreen->SetAccessibleName(mpMtrGreen->GetQuickHelpText());	//wj acc
	mpMtrBlue->SetAccessibleName(mpMtrBlue->GetQuickHelpText());		//wj acc
	mpMtrGamma->SetAccessibleName(::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Gamma value")));	//wj acc

    mpMtrRed->SetAccessibleRelationLabeledBy(mpMtrRed.get());
	mpMtrGreen->SetAccessibleRelationLabeledBy(mpMtrGreen.get());
	mpMtrBlue->SetAccessibleRelationLabeledBy(mpMtrBlue.get());
	mpMtrGamma->SetAccessibleRelationLabeledBy(mpMtrGamma.get());
	mpMtrBrightness->SetAccessibleRelationLabeledBy(mpFtBrightness.get());	//7874
	mpMtrContrast->SetAccessibleRelationLabeledBy(mpFtContrast.get());	//7874
	mpMtrTrans->SetAccessibleRelationLabeledBy(mpFtTrans.get());	//7874
	mpLBColorMode->SetAccessibleRelationLabeledBy(mpFtColorMode.get());	//7874
}

//////////////////////////////////////////////////////////////////////////////

IMPL_LINK( GraphicPropertyPanel, ModifyBrightnessHdl, void *, EMPTYARG )
{
	sal_Int16 nBright = mpMtrBrightness->GetValue();
	SfxInt16Item aBrightItem( SID_ATTR_GRAF_LUMINANCE, nBright );
	GetBindings()->GetDispatcher()->Execute(
		SID_ATTR_GRAF_LUMINANCE, SFX_CALLMODE_RECORD, &aBrightItem, 0L);
	return 0L;
}

//////////////////////////////////////////////////////////////////////////////

IMPL_LINK( GraphicPropertyPanel, ModifyContrastHdl, void *, EMPTYARG )
{
	sal_Int16 nContrast = mpMtrContrast->GetValue();
	SfxInt16Item aContrastItem( SID_ATTR_GRAF_CONTRAST, nContrast );
	GetBindings()->GetDispatcher()->Execute(
		SID_ATTR_GRAF_CONTRAST, SFX_CALLMODE_RECORD, &aContrastItem, 0L);
	return 0L;
}

//////////////////////////////////////////////////////////////////////////////

IMPL_LINK( GraphicPropertyPanel, ModifyTransHdl, void *, EMPTYARG )
{
	sal_Int16 nTrans = mpMtrTrans->GetValue();
	SfxInt16Item aTransItem( SID_ATTR_GRAF_TRANSPARENCE, nTrans );
	GetBindings()->GetDispatcher()->Execute(
		SID_ATTR_GRAF_TRANSPARENCE, SFX_CALLMODE_RECORD, &aTransItem, 0L);
	return 0L;
}

//////////////////////////////////////////////////////////////////////////////

IMPL_LINK( GraphicPropertyPanel, ClickColorModeHdl, ToolBox *, pBox )
{
	sal_Int16 nTrans = mpLBColorMode->GetSelectEntryPos();
	SfxInt16Item aTransItem( SID_ATTR_GRAF_MODE, nTrans );
	GetBindings()->GetDispatcher()->Execute(
		SID_ATTR_GRAF_MODE, SFX_CALLMODE_RECORD, &aTransItem, 0L);
	return 0L;
}

//////////////////////////////////////////////////////////////////////////////

IMPL_LINK( GraphicPropertyPanel, RedHdl, void*, EMPTYARG )
{
	sal_Int16 nRed = mpMtrRed->GetValue();
	SfxInt16Item aRedItem( SID_ATTR_GRAF_RED, nRed );
	GetBindings()->GetDispatcher()->Execute(
		SID_ATTR_GRAF_RED, SFX_CALLMODE_RECORD, &aRedItem, 0L);
	return 0L;
}

//////////////////////////////////////////////////////////////////////////////

IMPL_LINK( GraphicPropertyPanel, GreenHdl, void*, EMPTYARG )
{
	sal_Int16 nGreen = mpMtrGreen->GetValue();
	SfxInt16Item aGreenItem( SID_ATTR_GRAF_GREEN, nGreen );
	GetBindings()->GetDispatcher()->Execute(
		SID_ATTR_GRAF_GREEN, SFX_CALLMODE_RECORD, &aGreenItem, 0L);
	return 0L;
}

//////////////////////////////////////////////////////////////////////////////

IMPL_LINK(GraphicPropertyPanel, BlueHdl, void *, EMPTYARG)
{
	sal_Int16 nBlue = mpMtrBlue->GetValue();
	SfxInt16Item aBlueItem( SID_ATTR_GRAF_BLUE, nBlue );
	GetBindings()->GetDispatcher()->Execute(
		SID_ATTR_GRAF_BLUE, SFX_CALLMODE_RECORD, &aBlueItem, 0L);
	return 0L;
}

//////////////////////////////////////////////////////////////////////////////

IMPL_LINK(GraphicPropertyPanel, GammaHdl, void *, EMPTYARG)
{
	sal_Int32 nGamma = mpMtrGamma->GetValue();
	SfxInt32Item nGammaItem( SID_ATTR_GRAF_GAMMA, nGamma );
	GetBindings()->GetDispatcher()->Execute(
		SID_ATTR_GRAF_GAMMA, SFX_CALLMODE_RECORD, &nGammaItem, 0L);
	return 0L;
}

//////////////////////////////////////////////////////////////////////////////

void GraphicPropertyPanel::SetupIcons(void)
{
    if(Theme::GetBoolean(Theme::Bool_UseSymphonyIcons))
    {
        // todo
    }
    else
    {
        // todo
    }
}

//////////////////////////////////////////////////////////////////////////////

GraphicPropertyPanel* GraphicPropertyPanel::Create (
    Window* pParent,
    const cssu::Reference<css::frame::XFrame>& rxFrame,
    SfxBindings* pBindings)
{
    if (pParent == NULL)
        throw lang::IllegalArgumentException(A2S("no parent Window given to GraphicPropertyPanel::Create"), NULL, 0);
    if ( ! rxFrame.is())
        throw lang::IllegalArgumentException(A2S("no XFrame given to GraphicPropertyPanel::Create"), NULL, 1);
    if (pBindings == NULL)
        throw lang::IllegalArgumentException(A2S("no SfxBindings given to GraphicPropertyPanel::Create"), NULL, 2);
    
    return new GraphicPropertyPanel(
        pParent,
        rxFrame,
        pBindings);
}

//////////////////////////////////////////////////////////////////////////////

void GraphicPropertyPanel::DataChanged(
    const DataChangedEvent& rEvent)
{
    (void)rEvent;
    
    SetupIcons();
}

//////////////////////////////////////////////////////////////////////////////

void GraphicPropertyPanel::NotifyItemUpdate( 
    sal_uInt16 nSID, 
    SfxItemState eState, 
    const SfxPoolItem* pState)
{
	switch( nSID )
	{
	case SID_ATTR_GRAF_LUMINANCE:
		if( eState >= SFX_ITEM_AVAILABLE)
		{
			mpMtrBrightness->Enable();
            const SfxInt16Item* pItem = dynamic_cast< const SfxInt16Item* >(pState);

			if(pItem)
			{
				sal_Int64 nBright = pItem->GetValue();
				mpMtrBrightness->SetValue(nBright);
			}
		}
		else if( eState == SFX_ITEM_DISABLED )
			mpMtrBrightness->Disable();
		else
		{
			mpMtrBrightness->Enable();
			mpMtrBrightness->SetText( String());
		}
		break;
	case SID_ATTR_GRAF_CONTRAST:
		if( eState >= SFX_ITEM_AVAILABLE)
		{
			mpMtrContrast->Enable();
            const SfxInt16Item* pItem = dynamic_cast< const SfxInt16Item* >(pState);

            if(pItem)
			{
				sal_Int64 nContrast = pItem->GetValue();
				mpMtrContrast->SetValue(nContrast);
			}
		}
		else if( eState == SFX_ITEM_DISABLED )
			mpMtrContrast->Disable();
		else
		{
			mpMtrContrast->Enable();
			mpMtrContrast->SetText( String());
		}
		break;
	case SID_ATTR_GRAF_TRANSPARENCE:
		if( eState >= SFX_ITEM_AVAILABLE)
		{
			mpMtrTrans->Enable();
            const SfxUInt16Item* pItem = dynamic_cast< const SfxUInt16Item* >(pState);

			if(pItem)
			{
				sal_Int64 nTrans = pItem->GetValue();
				mpMtrTrans->SetValue(nTrans);
			}
		}
		else if( eState == SFX_ITEM_DISABLED )
			mpMtrTrans->Disable();
		else
		{
			mpMtrTrans->Enable();
			mpMtrTrans->SetText( String());
		}
		break;
	case SID_ATTR_GRAF_MODE:
		if( eState >= SFX_ITEM_AVAILABLE)
		{
			mpLBColorMode->Enable();
            const SfxUInt16Item* pItem = dynamic_cast< const SfxUInt16Item* >(pState);

			if(pItem)
			{
				sal_Int64 nTrans = pItem->GetValue();
				mpLBColorMode->SelectEntryPos(nTrans);
			}
		}
		else if( eState == SFX_ITEM_DISABLED )
			mpLBColorMode->Disable();
		else
		{
			mpLBColorMode->Enable();
			mpLBColorMode->SetNoSelection();
		}
		break;
	case SID_ATTR_GRAF_RED:
		if( eState >= SFX_ITEM_AVAILABLE)
		{
			mpMtrRed->Enable();
            const SfxInt16Item* pItem = dynamic_cast< const SfxInt16Item* >(pState);

			if(pItem)
			{
				sal_Int64 nRed = pItem->GetValue();
				mpMtrRed->SetValue( nRed );
			}
		}			
		else if( eState == SFX_ITEM_DISABLED )
			mpMtrRed->Disable();
		else
		{
			mpMtrRed->Enable();
			mpMtrRed->SetText( String());
		}
		break;
	case SID_ATTR_GRAF_GREEN:
		if( eState >= SFX_ITEM_AVAILABLE)
		{
			mpMtrGreen->Enable();
            const SfxInt16Item* pItem = dynamic_cast< const SfxInt16Item* >(pState);

			if(pItem)
			{
				sal_Int64 nGreen = pItem->GetValue();
				mpMtrGreen->SetValue( nGreen );
			}
		}
		else if( eState == SFX_ITEM_DISABLED )
			mpMtrGreen->Disable();
		else
		{
			mpMtrGreen->Enable();
			mpMtrGreen->SetText( String());
		}
		break;
	case SID_ATTR_GRAF_BLUE:
		if( eState >= SFX_ITEM_AVAILABLE)
		{
			mpMtrBlue->Enable();
            const SfxInt16Item* pItem = dynamic_cast< const SfxInt16Item* >(pState);

			if(pItem)
			{
				sal_Int64 nBlue = pItem->GetValue();
				mpMtrBlue->SetValue( nBlue );
			}
		}
		else if( eState == SFX_ITEM_DISABLED )
			mpMtrBlue->Disable();
		else
		{
			mpMtrBlue->Enable();
			mpMtrBlue->SetText( String());
		}
		break;
	case SID_ATTR_GRAF_GAMMA:
		if( eState >= SFX_ITEM_AVAILABLE)
		{
			mpMtrGamma->Enable();
            const SfxUInt32Item* pItem = dynamic_cast< const SfxUInt32Item* >(pState);

			if(pItem)
			{
				sal_Int64 nGamma = pItem->GetValue();
				mpMtrGamma->SetValue( nGamma );
			}
		}
		else if( eState == SFX_ITEM_DISABLED )
			mpMtrGamma->Disable();
		else
		{
			mpMtrGamma->Enable();
			mpMtrGamma->SetText( String());
		}
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////

SfxBindings* GraphicPropertyPanel::GetBindings() 
{ 
    return mpBindings; 
}

//////////////////////////////////////////////////////////////////////////////
// namespace close

}} // end of namespace ::svx::sidebar

//////////////////////////////////////////////////////////////////////////////
// eof
