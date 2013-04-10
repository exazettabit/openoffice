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



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_svx.hxx"

#include "svdfmtf.hxx"
#include <editeng/editdata.hxx>
#include <math.h>
#include <svx/xpoly.hxx>
#include <vcl/svapp.hxx>
#include <editeng/eeitem.hxx>
#include <editeng/fhgtitem.hxx>
#include <editeng/wghtitem.hxx>
#include <editeng/postitem.hxx>
#include <editeng/udlnitem.hxx>
#include <editeng/crsditem.hxx>
#include <editeng/shdditem.hxx>
#include <svx/xlnclit.hxx>
#include <svx/xlncapit.hxx>
#include <svx/xlnwtit.hxx>
#include <svx/xflclit.hxx>
#include <svx/xgrad.hxx>
#include <svx/xflgrit.hxx>
#include <editeng/fontitem.hxx>
#include <editeng/akrnitem.hxx>
#include <editeng/wrlmitem.hxx>
#include <editeng/cntritem.hxx>
#include <editeng/colritem.hxx>
#include <vcl/metric.hxx>
#include <editeng/charscaleitem.hxx>
#include <svx/xflhtit.hxx>
#include <svx/svdattr.hxx>
#include <svx/svdmodel.hxx>
#include <svx/svdpage.hxx>
#include <svx/svdobj.hxx>
#include "svx/svditext.hxx"
#include <svx/svdotext.hxx>
#include <svx/svdorect.hxx>
#include <svx/svdocirc.hxx>
#include <svx/svdograf.hxx>
#include <svx/svdopath.hxx>
#include <svx/svdetc.hxx>
#include <svl/itemset.hxx>
#include <basegfx/polygon/b2dpolygon.hxx>
#include <vcl/salbtype.hxx>		// FRound
#include <basegfx/matrix/b2dhommatrix.hxx>
#include <basegfx/matrix/b2dhommatrixtools.hxx>
#include <svx/xlinjoit.hxx>
#include <svx/xlndsit.hxx>
#include <basegfx/polygon/b2dpolygonclipper.hxx>
#include <svx/xbtmpit.hxx>
#include <svx/xfltrit.hxx>
#include <vcl/bmpacc.hxx>
#include <svx/xflbmtit.hxx>
#include <svx/xflbstit.hxx>
#include <svx/svdpntv.hxx>

////////////////////////////////////////////////////////////////////////////////////////////////////

ImpSdrGDIMetaFileImport::ImpSdrGDIMetaFileImport(
    SdrModel& rModel, 
    SdrLayerID nLay, 
    const Rectangle& rRect)
:   maTmpList(),
    maVD(),
    maScaleRect(rRect),
    mnMapScalingOfs(0),
	mpLineAttr(0),
    mpFillAttr(0),
    mpTextAttr(0),
    mpModel(&rModel),
    mnLayer(nLay),
    maOldLineColor(),
	mnLineWidth(0),
	maLineJoin(basegfx::B2DLINEJOIN_NONE),
	maLineCap(com::sun::star::drawing::LineCap_BUTT),
	maDash(XDASH_RECT, 0, 0, 0, 0, 0),
    mbMov(false),
    mbSize(false),
    maOfs(0, 0),
    mfScaleX(1.0),
    mfScaleY(1.0),
    maScaleX(1.0),
    maScaleY(1.0),
	mbFntDirty(true),
	mbLastObjWasPolyWithoutLine(false),
    mbNoLine(false),
    mbNoFill(false),
    mbLastObjWasLine(false),
    maClip()
{
	maVD.EnableOutput(false);
    maVD.SetLineColor();
    maVD.SetFillColor();
	maOldLineColor.SetRed( maVD.GetLineColor().GetRed() + 1 );
	mpLineAttr = new SfxItemSet(rModel.GetItemPool(), XATTR_LINE_FIRST, XATTR_LINE_LAST, 0, 0);
	mpFillAttr = new SfxItemSet(rModel.GetItemPool(), XATTR_FILL_FIRST, XATTR_FILL_LAST, 0, 0);
	mpTextAttr = new SfxItemSet(rModel.GetItemPool(), EE_ITEMS_START, EE_ITEMS_END, 0, 0);
    checkClip();
}

ImpSdrGDIMetaFileImport::~ImpSdrGDIMetaFileImport()
{
	delete mpLineAttr;
	delete mpFillAttr;
	delete mpTextAttr;
}

void ImpSdrGDIMetaFileImport::DoLoopActions(GDIMetaFile& rMtf, SvdProgressInfo* pProgrInfo, sal_uInt32* pActionsToReport)
{
	for( MetaAction* pAct = rMtf.FirstAction(); pAct; pAct = rMtf.NextAction() )
	{
		switch (pAct->GetType())
		{
			case META_PIXEL_ACTION          : DoAction((MetaPixelAction          &)*pAct); break;
			case META_POINT_ACTION          : DoAction((MetaPointAction          &)*pAct); break;
			case META_LINE_ACTION           : DoAction((MetaLineAction           &)*pAct); break;
			case META_RECT_ACTION           : DoAction((MetaRectAction           &)*pAct); break;
			case META_ROUNDRECT_ACTION      : DoAction((MetaRoundRectAction      &)*pAct); break;
			case META_ELLIPSE_ACTION        : DoAction((MetaEllipseAction        &)*pAct); break;
			case META_ARC_ACTION            : DoAction((MetaArcAction            &)*pAct); break;
			case META_PIE_ACTION            : DoAction((MetaPieAction            &)*pAct); break;
			case META_CHORD_ACTION          : DoAction((MetaChordAction          &)*pAct); break;
			case META_POLYLINE_ACTION	    : DoAction((MetaPolyLineAction		 &)*pAct); break;
			case META_POLYGON_ACTION        : DoAction((MetaPolygonAction        &)*pAct); break;
			case META_POLYPOLYGON_ACTION    : DoAction((MetaPolyPolygonAction    &)*pAct); break;
			case META_TEXT_ACTION           : DoAction((MetaTextAction           &)*pAct); break;
			case META_TEXTARRAY_ACTION      : DoAction((MetaTextArrayAction      &)*pAct); break;
			case META_STRETCHTEXT_ACTION    : DoAction((MetaStretchTextAction    &)*pAct); break;
			case META_BMP_ACTION			: DoAction((MetaBmpAction			 &)*pAct); break;
			case META_BMPSCALE_ACTION		: DoAction((MetaBmpScaleAction		 &)*pAct); break;
			case META_BMPEX_ACTION			: DoAction((MetaBmpExAction			 &)*pAct); break;
			case META_BMPEXSCALE_ACTION		: DoAction((MetaBmpExScaleAction	 &)*pAct); break;
			case META_LINECOLOR_ACTION      : DoAction((MetaLineColorAction      &)*pAct); break;
			case META_FILLCOLOR_ACTION      : DoAction((MetaFillColorAction      &)*pAct); break;
			case META_TEXTCOLOR_ACTION      : DoAction((MetaTextColorAction      &)*pAct); break;
			case META_TEXTFILLCOLOR_ACTION  : DoAction((MetaTextFillColorAction  &)*pAct); break;
			case META_FONT_ACTION           : DoAction((MetaFontAction           &)*pAct); break;
			case META_TEXTALIGN_ACTION		: DoAction((MetaTextAlignAction		 &)*pAct); break;
			case META_MAPMODE_ACTION        : DoAction((MetaMapModeAction        &)*pAct); break;
			case META_CLIPREGION_ACTION     : DoAction((MetaClipRegionAction     &)*pAct); break;
			case META_MOVECLIPREGION_ACTION : DoAction((MetaMoveClipRegionAction &)*pAct); break;
			case META_ISECTRECTCLIPREGION_ACTION: DoAction((MetaISectRectClipRegionAction&)*pAct); break;
			case META_ISECTREGIONCLIPREGION_ACTION: DoAction((MetaISectRegionClipRegionAction&)*pAct); break;
			case META_RASTEROP_ACTION       : DoAction((MetaRasterOpAction       &)*pAct); break;
			case META_PUSH_ACTION           : DoAction((MetaPushAction           &)*pAct); break;
			case META_POP_ACTION            : DoAction((MetaPopAction            &)*pAct); break;
			case META_HATCH_ACTION			: DoAction((MetaHatchAction          &)*pAct); break;
			case META_COMMENT_ACTION		: DoAction((MetaCommentAction        &)*pAct, &rMtf); break;

            // missing actions added
            case META_TEXTRECT_ACTION       : DoAction((MetaTextRectAction&)*pAct); break;
            case META_BMPSCALEPART_ACTION   : DoAction((MetaBmpScalePartAction&)*pAct); break;
            case META_BMPEXSCALEPART_ACTION : DoAction((MetaBmpExScalePartAction&)*pAct); break;
            case META_MASK_ACTION           : DoAction((MetaMaskAction&)*pAct); break;
            case META_MASKSCALE_ACTION      : DoAction((MetaMaskScaleAction&)*pAct); break;
            case META_MASKSCALEPART_ACTION  : DoAction((MetaMaskScalePartAction&)*pAct); break;
            case META_GRADIENT_ACTION       : DoAction((MetaGradientAction&)*pAct); break;
            case META_WALLPAPER_ACTION      : DoAction((MetaWallpaperAction&)*pAct); break;
            case META_TRANSPARENT_ACTION    : DoAction((MetaTransparentAction&)*pAct); break;
            case META_EPS_ACTION            : DoAction((MetaEPSAction&)*pAct); break;
            case META_REFPOINT_ACTION       : DoAction((MetaRefPointAction&)*pAct); break;
            case META_TEXTLINECOLOR_ACTION  : DoAction((MetaTextLineColorAction&)*pAct); break;
            case META_TEXTLINE_ACTION       : DoAction((MetaTextLineAction&)*pAct); break;
            case META_FLOATTRANSPARENT_ACTION : DoAction((MetaFloatTransparentAction&)*pAct); break;
            case META_GRADIENTEX_ACTION     : DoAction((MetaGradientExAction&)*pAct); break;
            case META_LAYOUTMODE_ACTION     : DoAction((MetaLayoutModeAction&)*pAct); break;
            case META_TEXTLANGUAGE_ACTION   : DoAction((MetaTextLanguageAction&)*pAct); break;
            case META_OVERLINECOLOR_ACTION  : DoAction((MetaOverlineColorAction&)*pAct); break;
		}

		if(pProgrInfo && pActionsToReport)
		{
            (*pActionsToReport)++;
            
            if(*pActionsToReport >= 16) // Alle 16 Action updaten
            {
                if(!pProgrInfo->ReportActions(*pActionsToReport))
                    break;

                *pActionsToReport = 0;
            }
		}
	}
}

sal_uInt32 ImpSdrGDIMetaFileImport::DoImport(
    const GDIMetaFile& rMtf,
	SdrObjList& rOL,
	sal_uInt32 nInsPos,
	SvdProgressInfo* pProgrInfo)
{
	// setup some global scale parameter
	// mfScaleX, mfScaleY, maScaleX, maScaleY, mbMov, mbSize
    mfScaleX = mfScaleY = 1.0;
	const Size aMtfSize(rMtf.GetPrefSize());

    if(aMtfSize.Width() & aMtfSize.Height() && (!maScaleRect.IsEmpty()))
    {
		maOfs = maScaleRect.TopLeft();

        if(aMtfSize.Width() != (maScaleRect.GetWidth() - 1))
        {
            mfScaleX = (double)( maScaleRect.GetWidth() - 1 ) / (double)aMtfSize.Width();
        }

        if(aMtfSize.Height() != (maScaleRect.GetHeight() - 1))
        {
            mfScaleY = (double)( maScaleRect.GetHeight() - 1 ) / (double)aMtfSize.Height();
        }
    }

	mbMov = maOfs.X()!=0 || maOfs.Y()!=0;
    mbSize = false;
	maScaleX = Fraction( 1, 1 );
	maScaleY = Fraction( 1, 1 );

    if(aMtfSize.Width() != (maScaleRect.GetWidth() - 1))
    {
        maScaleX = Fraction(maScaleRect.GetWidth() - 1, aMtfSize.Width());
        mbSize = true;
    }

    if(aMtfSize.Height() != (maScaleRect.GetHeight() - 1))
    {
        maScaleY = Fraction(maScaleRect.GetHeight() - 1, aMtfSize.Height());
        mbSize = true;
    }

	if(pProgrInfo)
    {
		pProgrInfo->SetActionCount(rMtf.GetActionCount());
    }

	sal_uInt32 nActionsToReport(0);

    // execute
    DoLoopActions(const_cast< GDIMetaFile& >(rMtf), pProgrInfo, &nActionsToReport);
    
    if(pProgrInfo)
	{
		pProgrInfo->ReportActions(nActionsToReport);
		nActionsToReport = 0;
	}

	// MapMode-Scaling  vornehmen
	MapScaling();

	// Beim berechnen der Fortschrittsanzeige wird GetActionCount()*3 benutzt.
	// Da in maTmpList allerdings weniger eintraege als GetActionCount()
	// existieren koennen, muessen hier die zuviel vermuteten Actionen wieder
	// hinzugefuegt werden.
	nActionsToReport = (rMtf.GetActionCount() - maTmpList.size()) * 2;

	// Alle noch nicht gemeldeten Rescales melden
	if(pProgrInfo)
	{
		pProgrInfo->ReportRescales(nActionsToReport);
		pProgrInfo->SetInsertCount(maTmpList.size());
	}

    nActionsToReport = 0;

	// alle in maTmpList zwischengespeicherten Objekte nun in rOL ab der Position nInsPos einfuegen
	if(nInsPos > rOL.GetObjCount()) 
    {
        nInsPos = rOL.GetObjCount();
    }

	SdrInsertReason aReason(SDRREASON_VIEWCALL);

	for(sal_uInt32 i(0); i < maTmpList.size(); i++)
	{
		SdrObject* pObj = maTmpList[i];
		rOL.NbcInsertObject(pObj, nInsPos, &aReason);
		nInsPos++;

		if(pProgrInfo)
		{
			nActionsToReport++;

            if(nActionsToReport >= 32) // Alle 32 Action updaten
			{
				pProgrInfo->ReportInserts(nActionsToReport);
				nActionsToReport = 0;
			}
		}
	}

	// ein letztesmal alle verbliebennen Inserts reporten
	if(pProgrInfo)
	{
		pProgrInfo->ReportInserts(nActionsToReport);
	}

	return maTmpList.size();
}

void ImpSdrGDIMetaFileImport::SetAttributes(SdrObject* pObj, bool bForceTextAttr)
{
	mbNoLine = false; 
    mbNoFill = false;
	bool bLine(!bForceTextAttr);
	bool bFill(!pObj || (pObj->IsClosedObj() && !bForceTextAttr));
	bool bText(bForceTextAttr || (pObj && pObj->GetOutlinerParaObject()));

	if(bLine)
	{
		if(mnLineWidth)
        {
			mpLineAttr->Put(XLineWidthItem(mnLineWidth));
        }
        else
        {
			mpLineAttr->Put(XLineWidthItem(0));
        }

		maOldLineColor = maVD.GetLineColor();

        if(maVD.IsLineColor())
		{
			mpLineAttr->Put(XLineStyleItem(XLINE_SOLID));
			mpLineAttr->Put(XLineColorItem(String(), maVD.GetLineColor()));
		}
		else
        {
			mpLineAttr->Put(XLineStyleItem(XLINE_NONE));
        }

		switch(maLineJoin)
		{
			default : // basegfx::B2DLINEJOIN_NONE
				mpLineAttr->Put(XLineJointItem(com::sun::star::drawing::LineJoint_NONE));
				break;
			case basegfx::B2DLINEJOIN_MIDDLE:
				mpLineAttr->Put(XLineJointItem(com::sun::star::drawing::LineJoint_MIDDLE));
				break;
			case basegfx::B2DLINEJOIN_BEVEL:
				mpLineAttr->Put(XLineJointItem(com::sun::star::drawing::LineJoint_BEVEL));
				break;
			case basegfx::B2DLINEJOIN_MITER:
				mpLineAttr->Put(XLineJointItem(com::sun::star::drawing::LineJoint_MITER));
				break;
			case basegfx::B2DLINEJOIN_ROUND:
				mpLineAttr->Put(XLineJointItem(com::sun::star::drawing::LineJoint_ROUND));
				break;
		}

        // Add LineCap support
        mpLineAttr->Put(XLineCapItem(maLineCap));

		if(((maDash.GetDots() && maDash.GetDotLen()) || (maDash.GetDashes() && maDash.GetDashLen())) && maDash.GetDistance())
		{
			mpLineAttr->Put(XLineDashItem(String(), maDash));
		}
        else
        {
			mpLineAttr->Put(XLineDashItem(String(), XDash(XDASH_RECT)));
        }
	}
	else
    {
		mbNoLine = true;
    }

	if(bFill)
	{
		if(maVD.IsFillColor())
		{
			mpFillAttr->Put(XFillStyleItem(XFILL_SOLID));
			mpFillAttr->Put(XFillColorItem(String(), maVD.GetFillColor()));
		}
		else
        {
			mpFillAttr->Put(XFillStyleItem(XFILL_NONE));
        }
	}
	else
    {
		mbNoFill = true;
    }

	if(bText && mbFntDirty)
	{
		Font aFnt(maVD.GetFont());
		const sal_uInt32 nHeight(FRound(aFnt.GetSize().Height() * mfScaleY));

        mpTextAttr->Put( SvxFontItem( aFnt.GetFamily(), aFnt.GetName(), aFnt.GetStyleName(), aFnt.GetPitch(), aFnt.GetCharSet(), EE_CHAR_FONTINFO ) );
		mpTextAttr->Put( SvxFontItem( aFnt.GetFamily(), aFnt.GetName(), aFnt.GetStyleName(), aFnt.GetPitch(), aFnt.GetCharSet(), EE_CHAR_FONTINFO_CJK ) );
		mpTextAttr->Put( SvxFontItem( aFnt.GetFamily(), aFnt.GetName(), aFnt.GetStyleName(), aFnt.GetPitch(), aFnt.GetCharSet(), EE_CHAR_FONTINFO_CTL ) );
        mpTextAttr->Put(SvxPostureItem(aFnt.GetItalic(), EE_CHAR_ITALIC));
        mpTextAttr->Put(SvxWeightItem(aFnt.GetWeight(), EE_CHAR_WEIGHT));
		mpTextAttr->Put( SvxFontHeightItem( nHeight, 100, EE_CHAR_FONTHEIGHT ) );
		mpTextAttr->Put( SvxFontHeightItem( nHeight, 100, EE_CHAR_FONTHEIGHT_CJK ) );
		mpTextAttr->Put( SvxFontHeightItem( nHeight, 100, EE_CHAR_FONTHEIGHT_CTL ) );
        mpTextAttr->Put(SvxCharScaleWidthItem(100, EE_CHAR_FONTWIDTH));
        mpTextAttr->Put(SvxUnderlineItem(aFnt.GetUnderline(), EE_CHAR_UNDERLINE));
        mpTextAttr->Put(SvxOverlineItem(aFnt.GetOverline(), EE_CHAR_OVERLINE));
        mpTextAttr->Put(SvxCrossedOutItem(aFnt.GetStrikeout(), EE_CHAR_STRIKEOUT));
        mpTextAttr->Put(SvxShadowedItem(aFnt.IsShadow(), EE_CHAR_SHADOW));

        // #i118485# Setting this item leads to problems (written #i118498# for this)
        // mpTextAttr->Put(SvxAutoKernItem(aFnt.IsKerning(), EE_CHAR_KERNING));

        mpTextAttr->Put(SvxWordLineModeItem(aFnt.IsWordLineMode(), EE_CHAR_WLM));
        mpTextAttr->Put(SvxContourItem(aFnt.IsOutline(), EE_CHAR_OUTLINE));
        mpTextAttr->Put(SvxColorItem(maVD.GetTextColor(), EE_CHAR_COLOR));
		//... svxfont textitem svditext
		mbFntDirty = false;
	}

    if(pObj)
	{
		pObj->SetLayer(mnLayer);

        if(bLine) 
        {
            pObj->SetMergedItemSet(*mpLineAttr);
        }

		if(bFill) 
        {
            pObj->SetMergedItemSet(*mpFillAttr);
        }

		if(bText)
		{
			pObj->SetMergedItemSet(*mpTextAttr);
			pObj->SetMergedItem(SdrTextHorzAdjustItem(SDRTEXTHORZADJUST_LEFT));
		}
	}
}

void ImpSdrGDIMetaFileImport::InsertObj(SdrObject* pObj, bool bScale)
{
    if(bScale && !maScaleRect.IsEmpty())
    {
		if(mbSize)
        {
			pObj->NbcResize(Point(), maScaleX, maScaleY);
        }

        if(mbMov)
        {
			pObj->NbcMove(Size(maOfs.X(), maOfs.Y()));
        }
	}

    if(isClip())
    {
        const basegfx::B2DPolyPolygon aPoly(pObj->TakeXorPoly());
        const basegfx::B2DRange aOldRange(aPoly.getB2DRange());
    	const SdrLayerID aOldLayer(pObj->GetLayer());
    	const SfxItemSet aOldItemSet(pObj->GetMergedItemSet());
        const SdrGrafObj* pSdrGrafObj = dynamic_cast< SdrGrafObj* >(pObj);
        BitmapEx aBitmapEx;

        if(pSdrGrafObj)
        {
        	aBitmapEx = pSdrGrafObj->GetGraphic().GetBitmapEx();
        }

        SdrObject::Free(pObj);

        if(!aOldRange.isEmpty())
        {
            // clip against ClipRegion
            const basegfx::B2DPolyPolygon aNewPoly(
                basegfx::tools::clipPolyPolygonOnPolyPolygon(
                    aPoly, 
                    maClip, 
                    true, 
                    aPoly.isClosed() ? false : true));
            const basegfx::B2DRange aNewRange(aNewPoly.getB2DRange());
            
            if(!aNewRange.isEmpty())
            {
                pObj = new SdrPathObj(
                    aNewPoly.isClosed() ? OBJ_POLY : OBJ_PLIN, 
                    aNewPoly);
            
		        pObj->SetLayer(aOldLayer);
		        pObj->SetMergedItemSet(aOldItemSet);

                if(!!aBitmapEx)
                {
                    // aNewRange is inside of aOldRange and defines which part of aBitmapEx is used
                    const double fScaleX(aBitmapEx.GetSizePixel().Width() / (aOldRange.getWidth() ? aOldRange.getWidth() : 1.0));
                    const double fScaleY(aBitmapEx.GetSizePixel().Height() / (aOldRange.getHeight() ? aOldRange.getHeight() : 1.0));
                    basegfx::B2DRange aPixel(aNewRange);
                    basegfx::B2DHomMatrix aTrans;

                    aTrans.translate(-aOldRange.getMinX(), -aOldRange.getMinY());
                    aTrans.scale(fScaleX, fScaleY);
                    aPixel.transform(aTrans);

                    const BitmapEx aClippedBitmap(
                        aBitmapEx, 
                        Point(floor(std::max(0.0, aPixel.getMinX())), floor(std::max(0.0, aPixel.getMinY()))), 
                        Size(ceil(aPixel.getWidth()), ceil(aPixel.getHeight())));

                    pObj->SetMergedItem(XFillStyleItem(XFILL_BITMAP));
                    pObj->SetMergedItem(XFillBitmapItem(String(), Graphic(aClippedBitmap)));
                    pObj->SetMergedItem(XFillBmpTileItem(false));
                    pObj->SetMergedItem(XFillBmpStretchItem(true));
                }
            }
        }
    }

    if(pObj)
    {
        // #i111954# check object for visibility
        // used are SdrPathObj, SdrRectObj, SdrCircObj, SdrGrafObj
        bool bVisible(false);

        if(pObj->HasLineStyle())
        {
            bVisible = true;
        }

        if(!bVisible && pObj->HasFillStyle())
        {
            bVisible = true;
        }

        if(!bVisible)
        {
            SdrTextObj* pTextObj = dynamic_cast< SdrTextObj* >(pObj);

            if(pTextObj && pTextObj->HasText())
            {
                bVisible = true;
            }
        }

        if(!bVisible)
        {
            SdrGrafObj* pGrafObj = dynamic_cast< SdrGrafObj* >(pObj);

            if(pGrafObj)
            {
                // this may be refined to check if the graphic really is visible. It
                // is here to ensure that graphic objects without fill, line and text
                // get created
                bVisible = true;
            }
        }

        if(!bVisible)
        {
            SdrObject::Free(pObj);
        }
        else
        {
	        maTmpList.push_back(pObj);

            if(dynamic_cast< SdrPathObj* >(pObj))
	        {
		        const bool bClosed(pObj->IsClosedObj());

                mbLastObjWasPolyWithoutLine = mbNoLine && bClosed;
		        mbLastObjWasLine = !bClosed;
	        }
	        else
	        {
		        mbLastObjWasPolyWithoutLine = false;
		        mbLastObjWasLine = false;
	        }
        }
    }
}

/**************************************************************************************************/

void ImpSdrGDIMetaFileImport::DoAction(MetaPixelAction& /*rAct*/)
{
}

void ImpSdrGDIMetaFileImport::DoAction(MetaPointAction& /*rAct*/)
{
}

void ImpSdrGDIMetaFileImport::DoAction(MetaLineAction& rAct)
{
	// #i73407# reformulation to use new B2DPolygon classes
	const basegfx::B2DPoint aStart(rAct.GetStartPoint().X(), rAct.GetStartPoint().Y());
	const basegfx::B2DPoint aEnd(rAct.GetEndPoint().X(), rAct.GetEndPoint().Y());

	if(!aStart.equal(aEnd))
	{
		basegfx::B2DPolygon aLine;
		const basegfx::B2DHomMatrix aTransform(basegfx::tools::createScaleTranslateB2DHomMatrix(mfScaleX, mfScaleY, maOfs.X(), maOfs.Y()));

		aLine.append(aStart);
		aLine.append(aEnd);
		aLine.transform(aTransform);

		const LineInfo& rLineInfo = rAct.GetLineInfo();
		const sal_Int32 nNewLineWidth(rLineInfo.GetWidth());
		bool bCreateLineObject(true);

		if(mbLastObjWasLine && (nNewLineWidth == mnLineWidth) && CheckLastLineMerge(aLine))
		{
			bCreateLineObject = false;
		}

		if(bCreateLineObject)
		{
			SdrPathObj* pPath = new SdrPathObj(OBJ_LINE, basegfx::B2DPolyPolygon(aLine));
			mnLineWidth = nNewLineWidth;
			maLineJoin = rLineInfo.GetLineJoin();
            maLineCap = rLineInfo.GetLineCap();
			maDash = XDash(XDASH_RECT,
				rLineInfo.GetDotCount(), rLineInfo.GetDotLen(),
				rLineInfo.GetDashCount(), rLineInfo.GetDashLen(),
				rLineInfo.GetDistance());
			SetAttributes(pPath);
			mnLineWidth = 0;
			maLineJoin = basegfx::B2DLINEJOIN_NONE;
			maDash = XDash();
			InsertObj(pPath, false);
		}
	}
}

void ImpSdrGDIMetaFileImport::DoAction(MetaRectAction& rAct)
{
    SdrRectObj* pRect=new SdrRectObj(rAct.GetRect());
	SetAttributes(pRect);
	InsertObj(pRect);
}

void ImpSdrGDIMetaFileImport::DoAction(MetaRoundRectAction& rAct)
{
    SdrRectObj* pRect=new SdrRectObj(rAct.GetRect());
	SetAttributes(pRect);
	long nRad=(rAct.GetHorzRound()+rAct.GetVertRound())/2;
	if (nRad!=0) {
		SfxItemSet aSet(*mpLineAttr->GetPool(), SDRATTR_ECKENRADIUS, SDRATTR_ECKENRADIUS, 0, 0);
		aSet.Put(SdrEckenradiusItem(nRad));
		pRect->SetMergedItemSet(aSet);
	}
	InsertObj(pRect);
}

/**************************************************************************************************/

void ImpSdrGDIMetaFileImport::DoAction(MetaEllipseAction& rAct)
{
    SdrCircObj* pCirc=new SdrCircObj(OBJ_CIRC,rAct.GetRect());
	SetAttributes(pCirc);
	InsertObj(pCirc);
}

void ImpSdrGDIMetaFileImport::DoAction(MetaArcAction& rAct)
{
    Point aCenter(rAct.GetRect().Center());
	long nStart=GetAngle(rAct.GetStartPoint()-aCenter);
	long nEnd=GetAngle(rAct.GetEndPoint()-aCenter);
	SdrCircObj* pCirc=new SdrCircObj(OBJ_CARC,rAct.GetRect(),nStart,nEnd);
	SetAttributes(pCirc);
	InsertObj(pCirc);
}

void ImpSdrGDIMetaFileImport::DoAction(MetaPieAction& rAct)
{
    Point aCenter(rAct.GetRect().Center());
	long nStart=GetAngle(rAct.GetStartPoint()-aCenter);
	long nEnd=GetAngle(rAct.GetEndPoint()-aCenter);
	SdrCircObj* pCirc=new SdrCircObj(OBJ_SECT,rAct.GetRect(),nStart,nEnd);
	SetAttributes(pCirc);
	InsertObj(pCirc);
}

void ImpSdrGDIMetaFileImport::DoAction(MetaChordAction& rAct)
{
    Point aCenter(rAct.GetRect().Center());
	long nStart=GetAngle(rAct.GetStartPoint()-aCenter);
	long nEnd=GetAngle(rAct.GetEndPoint()-aCenter);
	SdrCircObj* pCirc=new SdrCircObj(OBJ_CCUT,rAct.GetRect(),nStart,nEnd);
	SetAttributes(pCirc);
	InsertObj(pCirc);
}

/**************************************************************************************************/

bool ImpSdrGDIMetaFileImport::CheckLastLineMerge(const basegfx::B2DPolygon& rSrcPoly)
{
	// #i102706# Do not merge closed polygons
	if(rSrcPoly.isClosed())
	{
		return false;
	}

	// #i73407# reformulation to use new B2DPolygon classes
	if(mbLastObjWasLine && (maOldLineColor == maVD.GetLineColor()) && rSrcPoly.count())
	{
		SdrObject* pTmpObj = maTmpList.size() ? maTmpList[maTmpList.size() - 1] : 0;
		SdrPathObj* pLastPoly = dynamic_cast< SdrPathObj* >(pTmpObj);

		if(pLastPoly)
		{
			if(1L == pLastPoly->GetPathPoly().count())
			{
				bool bOk(false);
				basegfx::B2DPolygon aDstPoly(pLastPoly->GetPathPoly().getB2DPolygon(0L));

				// #i102706# Do not merge closed polygons
				if(aDstPoly.isClosed())
				{
					return false;
				}

				if(aDstPoly.count())
				{
					const sal_uInt32 nMaxDstPnt(aDstPoly.count() - 1L);
					const sal_uInt32 nMaxSrcPnt(rSrcPoly.count() - 1L);

					if(aDstPoly.getB2DPoint(nMaxDstPnt) == rSrcPoly.getB2DPoint(0L))
					{
						aDstPoly.append(rSrcPoly, 1L, rSrcPoly.count() - 1L);
						bOk = true;
					}
					else if(aDstPoly.getB2DPoint(0L) == rSrcPoly.getB2DPoint(nMaxSrcPnt))
					{
						basegfx::B2DPolygon aNew(rSrcPoly);
						aNew.append(aDstPoly, 1L, aDstPoly.count() - 1L);
						aDstPoly = aNew;
						bOk = true;
					}
					else if(aDstPoly.getB2DPoint(0L) == rSrcPoly.getB2DPoint(0L))
					{
						aDstPoly.flip();
						aDstPoly.append(rSrcPoly, 1L, rSrcPoly.count() - 1L);
						bOk = true;
					}
					else if(aDstPoly.getB2DPoint(nMaxDstPnt) == rSrcPoly.getB2DPoint(nMaxSrcPnt))
					{
						basegfx::B2DPolygon aNew(rSrcPoly);
						aNew.flip();
						aDstPoly.append(aNew, 1L, aNew.count() - 1L);
						bOk = true;
					}
				}

				if(bOk)
				{
					pLastPoly->NbcSetPathPoly(basegfx::B2DPolyPolygon(aDstPoly));
				}

				return bOk;
			}
		}
	}

	return false;
}

bool ImpSdrGDIMetaFileImport::CheckLastPolyLineAndFillMerge(const basegfx::B2DPolyPolygon & rPolyPolygon)
{
	// #i73407# reformulation to use new B2DPolygon classes
	if(mbLastObjWasPolyWithoutLine)
	{
		SdrObject* pTmpObj = maTmpList.size() ? maTmpList[maTmpList.size() - 1] : 0;
		SdrPathObj* pLastPoly = dynamic_cast< SdrPathObj* >(pTmpObj);

		if(pLastPoly)
		{
			if(pLastPoly->GetPathPoly() == rPolyPolygon)
			{
				SetAttributes(NULL);

				if(!mbNoLine && mbNoFill)
				{
					pLastPoly->SetMergedItemSet(*mpLineAttr);

					return true;
				}
			}
		}
	}

	return false;
}

void ImpSdrGDIMetaFileImport::checkClip()
{
    if(maVD.IsClipRegion())
    {
        maClip = maVD.GetClipRegion().GetAsB2DPolyPolygon();

        if(isClip())
        {
    		const basegfx::B2DHomMatrix aTransform(
                basegfx::tools::createScaleTranslateB2DHomMatrix(
                    mfScaleX, 
                    mfScaleY, 
                    maOfs.X(), 
                    maOfs.Y()));
	    	
            maClip.transform(aTransform);
        }
    }
}

bool ImpSdrGDIMetaFileImport::isClip() const
{
    return !maClip.getB2DRange().isEmpty();
}

void ImpSdrGDIMetaFileImport::DoAction( MetaPolyLineAction& rAct )
{
	// #i73407# reformulation to use new B2DPolygon classes
	basegfx::B2DPolygon aSource(rAct.GetPolygon().getB2DPolygon());

	if(aSource.count())
	{
		const basegfx::B2DHomMatrix aTransform(basegfx::tools::createScaleTranslateB2DHomMatrix(mfScaleX, mfScaleY, maOfs.X(), maOfs.Y()));
		aSource.transform(aTransform);
	}

	const LineInfo& rLineInfo = rAct.GetLineInfo();
	const sal_Int32 nNewLineWidth(rLineInfo.GetWidth());
	bool bCreateLineObject(true);

	if(mbLastObjWasLine && (nNewLineWidth == mnLineWidth) && CheckLastLineMerge(aSource))
	{
		bCreateLineObject = false;
	}
	else if(mbLastObjWasPolyWithoutLine && CheckLastPolyLineAndFillMerge(basegfx::B2DPolyPolygon(aSource)))
	{
		bCreateLineObject = false;
	}

	if(bCreateLineObject)
	{
        SdrPathObj* pPath = new SdrPathObj(
			aSource.isClosed() ? OBJ_POLY : OBJ_PLIN,
			basegfx::B2DPolyPolygon(aSource));
		mnLineWidth = nNewLineWidth;
		maLineJoin = rLineInfo.GetLineJoin();
        maLineCap = rLineInfo.GetLineCap();
		maDash = XDash(XDASH_RECT,
			rLineInfo.GetDotCount(), rLineInfo.GetDotLen(),
			rLineInfo.GetDashCount(), rLineInfo.GetDashLen(),
			rLineInfo.GetDistance());
		SetAttributes(pPath);
		mnLineWidth = 0;
		maLineJoin = basegfx::B2DLINEJOIN_NONE;
		maDash = XDash();
		InsertObj(pPath, false);
	}
}

void ImpSdrGDIMetaFileImport::DoAction( MetaPolygonAction& rAct )
{
	// #i73407# reformulation to use new B2DPolygon classes
	basegfx::B2DPolygon aSource(rAct.GetPolygon().getB2DPolygon());

	if(aSource.count())
	{
		const basegfx::B2DHomMatrix aTransform(basegfx::tools::createScaleTranslateB2DHomMatrix(mfScaleX, mfScaleY, maOfs.X(), maOfs.Y()));
		aSource.transform(aTransform);

		if(!mbLastObjWasPolyWithoutLine || !CheckLastPolyLineAndFillMerge(basegfx::B2DPolyPolygon(aSource)))
		{
			// #i73407# make sure polygon is closed, it's a filled primitive
			aSource.setClosed(true);
            SdrPathObj* pPath = new SdrPathObj(OBJ_POLY, basegfx::B2DPolyPolygon(aSource));
			SetAttributes(pPath);
			InsertObj(pPath, false);
		}
	}
}

void ImpSdrGDIMetaFileImport::DoAction(MetaPolyPolygonAction& rAct)
{
	// #i73407# reformulation to use new B2DPolygon classes
	basegfx::B2DPolyPolygon aSource(rAct.GetPolyPolygon().getB2DPolyPolygon());

	if(aSource.count())
	{
		const basegfx::B2DHomMatrix aTransform(basegfx::tools::createScaleTranslateB2DHomMatrix(mfScaleX, mfScaleY, maOfs.X(), maOfs.Y()));
		aSource.transform(aTransform);

		if(!mbLastObjWasPolyWithoutLine || !CheckLastPolyLineAndFillMerge(aSource))
		{
			// #i73407# make sure polygon is closed, it's a filled primitive
			aSource.setClosed(true);
            SdrPathObj* pPath = new SdrPathObj(OBJ_POLY, aSource);
			SetAttributes(pPath);
			InsertObj(pPath, false);
		}
	}
}

/**************************************************************************************************/

void ImpSdrGDIMetaFileImport::ImportText( const Point& rPos, const XubString& rStr, const MetaAction& rAct )
{
	// calc text box size, add 5% to make it fit safely

	FontMetric aFontMetric( maVD.GetFontMetric() );
	Font aFnt( maVD.GetFont() );
	FontAlign eAlg( aFnt.GetAlign() );

    sal_Int32 nTextWidth = (sal_Int32)( maVD.GetTextWidth( rStr ) * mfScaleX );
    sal_Int32 nTextHeight = (sal_Int32)( maVD.GetTextHeight() * mfScaleY );
	//sal_Int32 nDxWidth = 0;
	//sal_Int32 nLen = rStr.Len();

	Point aPos( FRound(rPos.X() * mfScaleX + maOfs.X()), FRound(rPos.Y() * mfScaleY + maOfs.Y()) );
    Size aSize( nTextWidth, nTextHeight );

	if ( eAlg == ALIGN_BASELINE )
		aPos.Y() -= FRound(aFontMetric.GetAscent() * mfScaleY);
	else if ( eAlg == ALIGN_BOTTOM )
		aPos.Y() -= nTextHeight;

	Rectangle aTextRect( aPos, aSize );
	SdrRectObj* pText =new SdrRectObj( OBJ_TEXT, aTextRect );

	if ( aFnt.GetWidth() || ( rAct.GetType() == META_STRETCHTEXT_ACTION ) )
	{
		pText->ClearMergedItem( SDRATTR_TEXT_AUTOGROWWIDTH );
		pText->SetMergedItem( SdrTextAutoGrowHeightItem( false ) );
		// don't let the margins eat the space needed for the text
		pText->SetMergedItem ( SdrTextUpperDistItem (0));
		pText->SetMergedItem ( SdrTextLowerDistItem (0));
		pText->SetMergedItem ( SdrTextRightDistItem (0));
		pText->SetMergedItem ( SdrTextLeftDistItem (0));
		pText->SetMergedItem( SdrTextFitToSizeTypeItem( SDRTEXTFIT_ALLLINES ) );
	}
	else
		pText->SetMergedItem( SdrTextAutoGrowWidthItem( true ) );

	pText->SetModel(mpModel);
	pText->SetLayer(mnLayer);
	pText->NbcSetText( rStr );
	SetAttributes( pText, true );
	pText->SetSnapRect( aTextRect );

	if (!aFnt.IsTransparent())
	{
		SfxItemSet aAttr(*mpFillAttr->GetPool(), XATTR_FILL_FIRST, XATTR_FILL_LAST, 0, 0);
		aAttr.Put(XFillStyleItem(XFILL_SOLID));
		aAttr.Put(XFillColorItem(String(), aFnt.GetFillColor()));
		pText->SetMergedItemSet(aAttr);
	}
	sal_uInt32 nWink = aFnt.GetOrientation();
	if ( nWink )
	{
		nWink*=10;
		double a=nWink*nPi180;
		double nSin=sin(a);
		double nCos=cos(a);
		pText->NbcRotate(aPos,nWink,nSin,nCos);
	}
	InsertObj( pText, false );
}

void ImpSdrGDIMetaFileImport::DoAction(MetaTextAction& rAct)
{
	XubString aStr(rAct.GetText());
	aStr.Erase(0,rAct.GetIndex());
	aStr.Erase(rAct.GetLen());
	ImportText( rAct.GetPoint(), aStr, rAct );
}

void ImpSdrGDIMetaFileImport::DoAction(MetaTextArrayAction& rAct)
{
	XubString aStr(rAct.GetText());
	aStr.Erase(0,rAct.GetIndex());
	aStr.Erase(rAct.GetLen());
	ImportText( rAct.GetPoint(), aStr, rAct );
}

void ImpSdrGDIMetaFileImport::DoAction(MetaStretchTextAction& rAct)
{
	XubString aStr(rAct.GetText());
	aStr.Erase(0,rAct.GetIndex());
	aStr.Erase(rAct.GetLen());
	ImportText( rAct.GetPoint(), aStr, rAct );
}

void ImpSdrGDIMetaFileImport::DoAction(MetaBmpAction& rAct)
{
	Rectangle aRect(rAct.GetPoint(),rAct.GetBitmap().GetSizePixel());
	aRect.Right()++; aRect.Bottom()++;
    SdrGrafObj* pGraf=new SdrGrafObj(Graphic(rAct.GetBitmap()),aRect);
	InsertObj(pGraf);
}

void ImpSdrGDIMetaFileImport::DoAction(MetaBmpScaleAction& rAct)
{
	Rectangle aRect(rAct.GetPoint(),rAct.GetSize());
	aRect.Right()++; aRect.Bottom()++;
    SdrGrafObj* pGraf=new SdrGrafObj(Graphic(rAct.GetBitmap()),aRect);
	InsertObj(pGraf);
}

void ImpSdrGDIMetaFileImport::DoAction(MetaBmpExAction& rAct)
{
	Rectangle aRect(rAct.GetPoint(),rAct.GetBitmapEx().GetSizePixel());
	aRect.Right()++; aRect.Bottom()++;
    SdrGrafObj* pGraf=new SdrGrafObj( rAct.GetBitmapEx(), aRect );
	InsertObj(pGraf);
}

void ImpSdrGDIMetaFileImport::DoAction(MetaBmpExScaleAction& rAct)
{
	Rectangle aRect(rAct.GetPoint(),rAct.GetSize());
	aRect.Right()++; aRect.Bottom()++;
    SdrGrafObj* pGraf=new SdrGrafObj( rAct.GetBitmapEx(), aRect );
	InsertObj(pGraf);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ImpSdrGDIMetaFileImport::DoAction( MetaHatchAction& rAct )
{
	// #i73407# reformulation to use new B2DPolygon classes
	basegfx::B2DPolyPolygon aSource(rAct.GetPolyPolygon().getB2DPolyPolygon());

	if(aSource.count())
	{
		const basegfx::B2DHomMatrix aTransform(basegfx::tools::createScaleTranslateB2DHomMatrix(mfScaleX, mfScaleY, maOfs.X(), maOfs.Y()));
		aSource.transform(aTransform);

		if(!mbLastObjWasPolyWithoutLine || !CheckLastPolyLineAndFillMerge(aSource))
		{
			const Hatch& rHatch = rAct.GetHatch();
            SdrPathObj* pPath = new SdrPathObj(OBJ_POLY, aSource);
			SfxItemSet aHatchAttr(mpModel->GetItemPool(), XATTR_FILLSTYLE, XATTR_FILLSTYLE, XATTR_FILLHATCH, XATTR_FILLHATCH, 0, 0);
			XHatchStyle eStyle;

			switch(rHatch.GetStyle())
			{
				case(HATCH_TRIPLE) :
				{
					eStyle = XHATCH_TRIPLE;
					break;
				}

				case(HATCH_DOUBLE) :
				{
					eStyle = XHATCH_DOUBLE;
					break;
				}

				default:
				{
					eStyle = XHATCH_SINGLE;
					break;
				}
			}

			SetAttributes(pPath);
			aHatchAttr.Put(XFillStyleItem(XFILL_HATCH));
			aHatchAttr.Put(XFillHatchItem(&mpModel->GetItemPool(), XHatch(rHatch.GetColor(), eStyle, rHatch.GetDistance(), rHatch.GetAngle())));
			pPath->SetMergedItemSet(aHatchAttr);

			InsertObj(pPath, false);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ImpSdrGDIMetaFileImport::DoAction(MetaLineColorAction& rAct)
{
	rAct.Execute(&maVD);
}

void ImpSdrGDIMetaFileImport::DoAction(MetaMapModeAction& rAct)
{
	MapScaling();
	rAct.Execute(&maVD);
	mbLastObjWasPolyWithoutLine = false;
	mbLastObjWasLine = false;
}

void ImpSdrGDIMetaFileImport::MapScaling()
{
	const sal_uInt32 nAnz(maTmpList.size());
	sal_uInt32 i(0);
	const MapMode& rMap = maVD.GetMapMode();
	Point aMapOrg( rMap.GetOrigin() );
	bool bMov2(aMapOrg.X() != 0 || aMapOrg.Y() != 0);

    if(bMov2)
    {
		for(i = mnMapScalingOfs; i < nAnz; i++)
        {
			SdrObject* pObj = maTmpList[i];

            pObj->NbcMove(Size(aMapOrg.X(), aMapOrg.Y()));
		}
	}

    mnMapScalingOfs = nAnz;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ImpSdrGDIMetaFileImport::DoAction( MetaCommentAction& rAct, GDIMetaFile* pMtf )
{
	ByteString aSkipComment;

	if( rAct.GetComment().CompareIgnoreCaseToAscii( "XGRAD_SEQ_BEGIN" ) == COMPARE_EQUAL )
	{
		MetaGradientExAction* pAct = (MetaGradientExAction*) pMtf->NextAction();

		if( pAct && pAct->GetType() == META_GRADIENTEX_ACTION )
		{
			// #i73407# reformulation to use new B2DPolygon classes
			basegfx::B2DPolyPolygon aSource(pAct->GetPolyPolygon().getB2DPolyPolygon());

			if(aSource.count())
			{
				if(!mbLastObjWasPolyWithoutLine || !CheckLastPolyLineAndFillMerge(aSource))
				{
					const Gradient&	rGrad = pAct->GetGradient();
                    SdrPathObj* pPath = new SdrPathObj(OBJ_POLY, aSource);
					SfxItemSet aGradAttr(mpModel->GetItemPool(), XATTR_FILLSTYLE, XATTR_FILLSTYLE, XATTR_FILLGRADIENT, XATTR_FILLGRADIENT, 0, 0);
					XGradient aXGradient;

					aXGradient.SetGradientStyle((XGradientStyle)rGrad.GetStyle());
					aXGradient.SetStartColor(rGrad.GetStartColor());
					aXGradient.SetEndColor(rGrad.GetEndColor());
					aXGradient.SetAngle((sal_uInt16)rGrad.GetAngle());
					aXGradient.SetBorder(rGrad.GetBorder());
					aXGradient.SetXOffset(rGrad.GetOfsX());
					aXGradient.SetYOffset(rGrad.GetOfsY());
					aXGradient.SetStartIntens(rGrad.GetStartIntensity());
					aXGradient.SetEndIntens(rGrad.GetEndIntensity());
					aXGradient.SetSteps(rGrad.GetSteps());

            		if(maVD.IsLineColor())
                    {
                        // switch line off; when there was one there will be a
                        // META_POLYLINE_ACTION following creating another object
    		            const Color aLineColor(maVD.GetLineColor());
    		            maVD.SetLineColor();
                        SetAttributes(pPath);
    		            maVD.SetLineColor(aLineColor);
                    }
                    else
                    {
                        SetAttributes(pPath);
                    }

					aGradAttr.Put(XFillStyleItem(XFILL_GRADIENT));
					aGradAttr.Put(XFillGradientItem(&mpModel->GetItemPool(), aXGradient));
					pPath->SetMergedItemSet(aGradAttr);

					InsertObj(pPath);
				}
			}

			aSkipComment = "XGRAD_SEQ_END";
		}
	}

	if(aSkipComment.Len())
	{
		MetaAction* pSkipAct = pMtf->NextAction();

		while( pSkipAct
			&& ((pSkipAct->GetType() != META_COMMENT_ACTION )
				|| (((MetaCommentAction*)pSkipAct)->GetComment().CompareIgnoreCaseToAscii(aSkipComment.GetBuffer()) != COMPARE_EQUAL)))
		{
			pSkipAct = pMtf->NextAction();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ImpSdrGDIMetaFileImport::DoAction(MetaTextRectAction& rAct)
{
    GDIMetaFile aTemp;

    maVD.AddTextRectActions(rAct.GetRect(), rAct.GetText(), rAct.GetStyle(), aTemp);
    DoLoopActions(aTemp, 0, 0);
}

void ImpSdrGDIMetaFileImport::DoAction(MetaBmpScalePartAction& rAct)
{
	Rectangle aRect(rAct.GetDestPoint(), rAct.GetDestSize());
    Bitmap aBitmap(rAct.GetBitmap());
	
    aRect.Right()++; 
    aRect.Bottom()++;
    aBitmap.Crop(Rectangle(rAct.GetSrcPoint(), rAct.GetSrcSize()));

    SdrGrafObj* pGraf = new SdrGrafObj(aBitmap, aRect);

    InsertObj(pGraf);
}

void ImpSdrGDIMetaFileImport::DoAction(MetaBmpExScalePartAction& rAct)
{
	Rectangle aRect(rAct.GetDestPoint(),rAct.GetDestSize());
    BitmapEx aBitmapEx(rAct.GetBitmapEx());
	
    aRect.Right()++; 
    aRect.Bottom()++;
    aBitmapEx.Crop(Rectangle(rAct.GetSrcPoint(), rAct.GetSrcSize()));

    SdrGrafObj* pGraf = new SdrGrafObj(aBitmapEx, aRect);

    InsertObj(pGraf);
}

void ImpSdrGDIMetaFileImport::DoAction(MetaMaskAction& rAct)
{
	Rectangle aRect(rAct.GetPoint(), rAct.GetBitmap().GetSizePixel());
    BitmapEx aBitmapEx(rAct.GetBitmap(), rAct.GetColor());

    aRect.Right()++; 
    aRect.Bottom()++;
    
    SdrGrafObj* pGraf = new SdrGrafObj(aBitmapEx, aRect);

    InsertObj(pGraf);
}

void ImpSdrGDIMetaFileImport::DoAction(MetaMaskScaleAction& rAct)
{
	Rectangle aRect(rAct.GetPoint(), rAct.GetSize());
    BitmapEx aBitmapEx(rAct.GetBitmap(), rAct.GetColor());

    aRect.Right()++; 
    aRect.Bottom()++;
    
    SdrGrafObj* pGraf = new SdrGrafObj(aBitmapEx, aRect);

    InsertObj(pGraf);
}

void ImpSdrGDIMetaFileImport::DoAction(MetaMaskScalePartAction& rAct)
{
	Rectangle aRect(rAct.GetDestPoint(), rAct.GetDestSize());
    BitmapEx aBitmapEx(rAct.GetBitmap(), rAct.GetColor());
	
    aRect.Right()++; 
    aRect.Bottom()++;
    aBitmapEx.Crop(Rectangle(rAct.GetSrcPoint(), rAct.GetSrcSize()));

    SdrGrafObj* pGraf = new SdrGrafObj(aBitmapEx, aRect);

    InsertObj(pGraf);
}

XGradientStyle getXGradientStyleFromGradientStyle(const GradientStyle& rGradientStyle)
{
    XGradientStyle aXGradientStyle(XGRAD_LINEAR);

    switch(rGradientStyle)
    {
        case GRADIENT_LINEAR: aXGradientStyle = XGRAD_LINEAR; break;
        case GRADIENT_AXIAL: aXGradientStyle = XGRAD_AXIAL; break;
        case GRADIENT_RADIAL: aXGradientStyle = XGRAD_RADIAL; break;
        case GRADIENT_ELLIPTICAL: aXGradientStyle = XGRAD_ELLIPTICAL; break;
        case GRADIENT_SQUARE: aXGradientStyle = XGRAD_SQUARE; break;
        case GRADIENT_RECT: aXGradientStyle = XGRAD_RECT; break;
        
        // Needed due to GRADIENT_FORCE_EQUAL_SIZE; this again is needed
        // to force the enum defines in VCL to a defined size for the compilers,
        // so despite it is never used it cannot be removed (would break the
        // API implementation probably).
        default: break; 
    }

    return aXGradientStyle;
}

void ImpSdrGDIMetaFileImport::DoAction(MetaGradientAction& rAct)
{
    basegfx::B2DRange aRange(rAct.GetRect().Left(), rAct.GetRect().Top(), rAct.GetRect().Right(), rAct.GetRect().Bottom());

	if(!aRange.isEmpty())
	{
		const basegfx::B2DHomMatrix aTransform(basegfx::tools::createScaleTranslateB2DHomMatrix(mfScaleX, mfScaleY, maOfs.X(), maOfs.Y()));
		aRange.transform(aTransform);
        const Gradient& rGradient = rAct.GetGradient();
        SdrRectObj* pRect = new SdrRectObj(
            Rectangle(
                floor(aRange.getMinX()),
                floor(aRange.getMinY()),
                ceil(aRange.getMaxX()),
                ceil(aRange.getMaxY())));
		SfxItemSet aGradientAttr(mpModel->GetItemPool(), XATTR_FILLSTYLE, XATTR_FILLSTYLE, XATTR_FILLGRADIENT, XATTR_FILLGRADIENT, 0, 0);
        const XGradientStyle aXGradientStyle(getXGradientStyleFromGradientStyle(rGradient.GetStyle()));
        const XFillGradientItem aXFillGradientItem(
            &mpModel->GetItemPool(), 
            XGradient(
                rGradient.GetStartColor(),
                rGradient.GetEndColor(),
                aXGradientStyle,
                rGradient.GetAngle(),
                rGradient.GetOfsX(),
                rGradient.GetOfsY(),
                rGradient.GetBorder(),
                rGradient.GetStartIntensity(),
                rGradient.GetEndIntensity(),
                rGradient.GetSteps()));

		SetAttributes(pRect);
		aGradientAttr.Put(XFillStyleItem(XFILL_HATCH));
		aGradientAttr.Put(aXFillGradientItem);
		pRect->SetMergedItemSet(aGradientAttr);

		InsertObj(pRect, false);
	}
}

void ImpSdrGDIMetaFileImport::DoAction(MetaWallpaperAction& /*rAct*/)
{
    OSL_ENSURE(false, "Tried to construct SdrObject from MetaWallpaperAction: not supported (!)");
}

void ImpSdrGDIMetaFileImport::DoAction(MetaTransparentAction& rAct)
{
	basegfx::B2DPolyPolygon aSource(rAct.GetPolyPolygon().getB2DPolyPolygon());

	if(aSource.count())
	{
		const basegfx::B2DHomMatrix aTransform(basegfx::tools::createScaleTranslateB2DHomMatrix(mfScaleX, mfScaleY, maOfs.X(), maOfs.Y()));
		aSource.transform(aTransform);
    	aSource.setClosed(true);

        SdrPathObj* pPath = new SdrPathObj(OBJ_POLY, aSource);
		SetAttributes(pPath);
        pPath->SetMergedItem(XFillTransparenceItem(rAct.GetTransparence()));
		InsertObj(pPath, false);
	}
}

void ImpSdrGDIMetaFileImport::DoAction(MetaEPSAction& /*rAct*/)
{
    OSL_ENSURE(false, "Tried to construct SdrObject from MetaEPSAction: not supported (!)");
}

void ImpSdrGDIMetaFileImport::DoAction(MetaTextLineAction& /*rAct*/)
{
    OSL_ENSURE(false, "Tried to construct SdrObject from MetaTextLineAction: not supported (!)");
}

void ImpSdrGDIMetaFileImport::DoAction(MetaGradientExAction& rAct)
{
	basegfx::B2DPolyPolygon aSource(rAct.GetPolyPolygon().getB2DPolyPolygon());

	if(aSource.count())
	{
		const basegfx::B2DHomMatrix aTransform(basegfx::tools::createScaleTranslateB2DHomMatrix(mfScaleX, mfScaleY, maOfs.X(), maOfs.Y()));
		aSource.transform(aTransform);

		if(!mbLastObjWasPolyWithoutLine || !CheckLastPolyLineAndFillMerge(aSource))
		{
        	const Gradient& rGradient = rAct.GetGradient();
            SdrPathObj* pPath = new SdrPathObj(OBJ_POLY, aSource);
			SfxItemSet aGradientAttr(mpModel->GetItemPool(), XATTR_FILLSTYLE, XATTR_FILLSTYLE, XATTR_FILLGRADIENT, XATTR_FILLGRADIENT, 0, 0);
            const XGradientStyle aXGradientStyle(getXGradientStyleFromGradientStyle(rGradient.GetStyle()));
            const XFillGradientItem aXFillGradientItem(
                &mpModel->GetItemPool(), 
                XGradient(
                    rGradient.GetStartColor(),
                    rGradient.GetEndColor(),
                    aXGradientStyle,
                    rGradient.GetAngle(),
                    rGradient.GetOfsX(),
                    rGradient.GetOfsY(),
                    rGradient.GetBorder(),
                    rGradient.GetStartIntensity(),
                    rGradient.GetEndIntensity(),
                    rGradient.GetSteps()));

			SetAttributes(pPath);
			aGradientAttr.Put(XFillStyleItem(XFILL_HATCH));
			aGradientAttr.Put(aXFillGradientItem);
			pPath->SetMergedItemSet(aGradientAttr);

			InsertObj(pPath, false);
		}
	}
}

void ImpSdrGDIMetaFileImport::DoAction(MetaFloatTransparentAction& rAct)
{
	const GDIMetaFile& rMtf = rAct.GetGDIMetaFile();

    if(rMtf.GetActionCount())
    {
	    const Rectangle aRect(rAct.GetPoint(),rAct.GetSize());

        // convert metafile sub-content to BitmapEx
        BitmapEx aBitmapEx(
            convertMetafileToBitmapEx(
                rMtf,
                basegfx::B2DRange(
                    aRect.Left(), aRect.Top(), 
                    aRect.Right(), aRect.Bottom()),
                125000));

        // handle colors
        const Gradient& rGradient = rAct.GetGradient();
	    basegfx::BColor aStart(rGradient.GetStartColor().getBColor());
	    basegfx::BColor aEnd(rGradient.GetEndColor().getBColor());
	    
        if(100 != rGradient.GetStartIntensity())
	    {
            aStart *= (double)rGradient.GetStartIntensity() / 100.0;
	    }

        if(100 != rGradient.GetEndIntensity())
	    {
            aEnd *= (double)rGradient.GetEndIntensity() / 100.0;
	    }

        const bool bEqualColors(aStart == aEnd);
        const bool bNoSteps(1 == rGradient.GetSteps());
        bool bCreateObject(true);
        bool bHasNewMask(false);
        AlphaMask aNewMask;
        double fTransparence(0.0);
        bool bFixedTransparence(false);

        if(bEqualColors || bNoSteps)
        {
            // single transparence
            const basegfx::BColor aMedium(basegfx::average(aStart, aEnd));
            fTransparence = aMedium.luminance();

            if(basegfx::fTools::lessOrEqual(fTransparence, 0.0))
            {
                // no transparence needed, all done
            }
            else if(basegfx::fTools::moreOrEqual(fTransparence, 1.0))
            {
                // all transparent, no object
                bCreateObject = false;
            }
            else
            {
                // 0.0 < transparence < 1.0, apply fixed transparence
                bFixedTransparence = true;
            }
        }
        else
        {
            // gradient transparence
            VirtualDevice aVDev;

            aVDev.SetOutputSizePixel(aBitmapEx.GetBitmap().GetSizePixel());
            aVDev.DrawGradient(Rectangle(Point(0, 0), aVDev.GetOutputSizePixel()), rGradient);

            aNewMask = AlphaMask(aVDev.GetBitmap(Point(0, 0), aVDev.GetOutputSizePixel()));
            bHasNewMask = true;
        }

        if(bCreateObject)
        {
            if(bHasNewMask || bFixedTransparence)
            {
                if(!aBitmapEx.IsAlpha() && !aBitmapEx.IsTransparent())
                {
                    // no transparence yet, apply new one
                    if(bFixedTransparence)
                    {
                        sal_uInt8 aAlpha(basegfx::fround(fTransparence * 255.0));

                        aNewMask = AlphaMask(aBitmapEx.GetBitmap().GetSizePixel(), &aAlpha);
                    }

                    aBitmapEx = BitmapEx(aBitmapEx.GetBitmap(), aNewMask);
                }
                else
                {
                    // mix existing and new alpha mask
                    AlphaMask aOldMask;

                    if(aBitmapEx.IsAlpha())
                    {
                        aOldMask = aBitmapEx.GetAlpha();
                    }
                    else if(TRANSPARENT_BITMAP == aBitmapEx.GetTransparentType())
                    {
                        aOldMask = aBitmapEx.GetMask();
                    }
                    else if(TRANSPARENT_COLOR == aBitmapEx.GetTransparentType())
                    {
                        aOldMask = aBitmapEx.GetBitmap().CreateMask(aBitmapEx.GetTransparentColor());
                    }

                    BitmapWriteAccess* pOld = aOldMask.AcquireWriteAccess();

                    if(pOld)
                    {
                        const double fFactor(1.0 / 255.0);
                        
                        if(bFixedTransparence)
                        {
                            const double fOpNew(1.0 - fTransparence);
                            
                            for(sal_uInt32 y(0); y < static_cast< sal_uInt32 >(pOld->Height()); y++)
                            {
                                for(sal_uInt32 x(0); x < static_cast< sal_uInt32 >(pOld->Width()); x++)
                                {
                                    const double fOpOld(1.0 - (pOld->GetPixel(y, x).GetIndex() * fFactor));
                                    const sal_uInt8 aCol(basegfx::fround((1.0 - (fOpOld * fOpNew)) * 255.0));

                                    pOld->SetPixel(y, x, BitmapColor(aCol));
                                }
                            }
                        }
                        else
                        {
                            BitmapReadAccess* pNew = aNewMask.AcquireReadAccess();

                            if(pNew)
                            {
                                if(pOld->Width() == pNew->Width() && pOld->Height() == pNew->Height())
                                {
                                    for(sal_uInt32 y(0); y < static_cast< sal_uInt32 >(pOld->Height()); y++)
                                    {
                                        for(sal_uInt32 x(0); x < static_cast< sal_uInt32 >(pOld->Width()); x++)
                                        {
                                            const double fOpOld(1.0 - (pOld->GetPixel(y, x).GetIndex() * fFactor));
                                            const double fOpNew(1.0 - (pNew->GetPixel(y, x).GetIndex() * fFactor));
                                            const sal_uInt8 aCol(basegfx::fround((1.0 - (fOpOld * fOpNew)) * 255.0));

                                            pOld->SetPixel(y, x, BitmapColor(aCol));
                                        }
                                    }
                                }
                                else
                                {
                                    OSL_ENSURE(false, "Alpha masks have different sizes (!)");
                                }
        
                                aNewMask.ReleaseAccess(pNew);
                            }
                            else
                            {
                                OSL_ENSURE(false, "Got no access to new alpha mask (!)");
                            }
                        }

                        aOldMask.ReleaseAccess(pOld);
                    }
                    else
                    {
                        OSL_ENSURE(false, "Got no access to old alpha mask (!)");
                    }

                    // apply combined bitmap as mask
                    aBitmapEx = BitmapEx(aBitmapEx.GetBitmap(), aOldMask);
                }
            }

            // create and add object
            SdrGrafObj* pGraf = new SdrGrafObj(aBitmapEx, aRect);

            InsertObj(pGraf);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// eof
