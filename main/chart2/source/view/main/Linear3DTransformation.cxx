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
#include "precompiled_chart2.hxx"
#include "Linear3DTransformation.hxx"
#include <algorithm>

using namespace ::com::sun::star;

using ::com::sun::star::uno::Sequence;
using ::com::sun::star::uno::RuntimeException;

namespace chart
{

    Linear3DTransformation::Linear3DTransformation( const drawing::HomogenMatrix& rHomMatrix, bool bSwapXAndY )
    : m_Matrix(rHomMatrix)
    , m_bSwapXAndY(bSwapXAndY)
{}

Linear3DTransformation::~Linear3DTransformation()
{}

// ____ XTransformation ____
Sequence< double > SAL_CALL Linear3DTransformation::transform(
                        const Sequence< double >& rSourceValues )
    throw (RuntimeException,
           lang::IllegalArgumentException)
{
    double fX = rSourceValues[0];
    double fY = rSourceValues[1];
    double fZ = rSourceValues[2];
    if(m_bSwapXAndY)
        std::swap(fX,fY);
    Sequence< double > aNewVec(3);
	double fZwi;

    fZwi = m_Matrix.Line1.Column1 * fX
         + m_Matrix.Line1.Column2 * fY
	     + m_Matrix.Line1.Column3 * fZ
	     + m_Matrix.Line1.Column4;
    aNewVec[0] = fZwi;

    fZwi = m_Matrix.Line2.Column1 * fX
	     + m_Matrix.Line2.Column2 * fY
	     + m_Matrix.Line2.Column3 * fZ
	     + m_Matrix.Line2.Column4;
    aNewVec[1] = fZwi;

    fZwi = m_Matrix.Line3.Column1 * fX
	     + m_Matrix.Line3.Column2 * fY
	     + m_Matrix.Line3.Column3 * fZ
	     + m_Matrix.Line3.Column4;
    aNewVec[2] = fZwi;

	fZwi = m_Matrix.Line4.Column1 * fX
		 + m_Matrix.Line4.Column2 * fY
		 + m_Matrix.Line4.Column3 * fZ
		 + m_Matrix.Line4.Column4;
	if(fZwi != 1.0 && fZwi != 0.0)
	{
		aNewVec[0] /= fZwi;
		aNewVec[1] /= fZwi;
		aNewVec[2] /= fZwi;
	}
	return aNewVec;
}

sal_Int32 SAL_CALL Linear3DTransformation::getSourceDimension()
    throw (RuntimeException)
{
    return 3;
}

sal_Int32 SAL_CALL Linear3DTransformation::getTargetDimension()
    throw (RuntimeException)
{
    return 3;
}


}  // namespace chart
