#pragma once

#include <cmath>
#include <algorithm>
#include <numeric>
#include <string>
#include <vector>
#include <windows.h>

typedef std::vector<std::wstring> TTableRowContent;
typedef std::vector<TTableRowContent> TTableContent;

class TextTable
{
public:
    TextTable(HDC hDc, TTableContent& vContent, INT x, INT y, INT wdthTable,
        HFONT hFont, BOOL bAlignRows = TRUE, INT hghtEmptyTable = 0);
    VOID Draw();
    INT GetHeight();
private:
    HDC m_hDc;
    TTableContent m_vContent;
    INT m_x;
    INT m_y;
    INT m_wdthTable;
    INT m_hghtEmptyTable;
    BOOL m_bAlignRows;

    HFONT m_hFont;
    TEXTMETRIC m_txtmtrc;
    INT m_wdthCell;
    std::vector<INT> m_ahghtCell;
    INT m_hghtCellMax;
    INT m_hghtTableMax;
    INT m_hghtTableActual;
    UINT m_cCols;
    UINT m_cRows;

    static CONST FLOAT CHAR_WIDTH_COEFF;
    static CONST COLORREF TABLE_BRUSH_COLOR;
    static CONST COLORREF TABLE_PEN_COLOR;
    static CONST INT TABLE_PEN_WIDTH;
    static CONST INT TABLE_PEN_TYPE;

    TextTable(CONST TextTable&) = delete;
    VOID CalculateMetrics();
    std::vector<INT> GetCellHeights(TTableContent& content);
    static INT GetNumberOfColumns(TTableContent& content);
    static INT GetMaxElementLength(TTableRowContent& row);
};