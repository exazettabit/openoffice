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



#ifndef SVTOOLS_TABLESORT_HXX
#define SVTOOLS_TABLESORT_HXX

#include "svtools/table/tabletypes.hxx"

//......................................................................................................................
namespace svt { namespace table
{
//......................................................................................................................

	//==================================================================================================================
	//= ColumnSortDirection
	//==================================================================================================================
    enum ColumnSortDirection
    {
        ColumnSortAscending,
        ColumnSortDescending
    };

	//==================================================================================================================
	//= ColumnSort
	//==================================================================================================================
    struct ColumnSort
    {
        ColPos              nColumnPos;
        ColumnSortDirection eSortDirection;

        ColumnSort()
            :nColumnPos( COL_INVALID )
            ,eSortDirection( ColumnSortAscending )
        {
        }

        ColumnSort( ColPos const i_columnPos, ColumnSortDirection const i_sortDirection )
            :nColumnPos( i_columnPos )
            ,eSortDirection( i_sortDirection )
        {
        }
    };

	//==================================================================================================================
	//= ITableDataSort
	//==================================================================================================================
    /** provides sorting functionality for the datta underlying an ITableModel
    */
	class SAL_NO_VTABLE ITableDataSort
	{
    public:
        /** sorts the rows in the model by the given column's data, in the given direction.
        */
        virtual void        sortByColumn( ColPos const i_column, ColumnSortDirection const i_sortDirection ) = 0;

        /** retrieves the current sort order of the data

            If the <code>nColumnIndex</code> member of the returned srtructure is <code>COL_INVALID</code>, then
            the data is currently not sorted.
        */
        virtual ColumnSort  getCurrentSortOrder() const = 0;
	};

//......................................................................................................................
} } // namespace svt::table
//......................................................................................................................

#endif // SVTOOLS_TABLESORT_HXX
