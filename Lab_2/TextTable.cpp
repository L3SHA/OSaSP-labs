#include "TextTable.h"

CONST FLOAT TextTable::CHAR_WIDTH_COEFF = 1.5;
CONST COLORREF TextTable::TABLE_BRUSH_COLOR = RGB(255, 255, 255);
CONST COLORREF TextTable::TABLE_PEN_COLOR = RGB(0, 0, 0);
CONST INT TextTable::TABLE_PEN_WIDTH = 1;
CONST INT TextTable::TABLE_PEN_TYPE = PS_SOLID;

TextTable::TextTable(HDC hDc, TTableContent& vContent, INT x, INT y, INT wdthTable,
    HFONT hFont, BOOL bAlignRows, INT hghtEmptyTable) : m_hDc(hDc), m_vContent(vContent), m_x(x), m_y(y), m_wdthTable(wdthTable),
    m_hFont(hFont), m_bAlignRows(bAlignRows), m_hghtEmptyTable(hghtEmptyTable), m_hghtTableMax(0), m_hghtTableActual(0)
{
    CalculateMetrics();
}

VOID TextTable::Draw()
{
    HFONT hOldFont = (HFONT)SelectObject(m_hDc, m_hFont);
    CONST HBRUSH hBr = CreateSolidBrush(TABLE_BRUSH_COLOR);
    CONST HBRUSH hOldBr = (HBRUSH)SelectObject(m_hDc, hBr);
    CONST HPEN hPn = CreatePen(TABLE_PEN_TYPE, TABLE_PEN_WIDTH, TABLE_PEN_COLOR);
    CONST HPEN hOldPn = (HPEN)SelectObject(m_hDc, hPn);

    if (!m_vContent.size()) {
        Rectangle(m_hDc, m_x, m_y, m_x + m_wdthTable, m_y + m_hghtEmptyTable);
    }
    else {
        INT xCell = m_x;
        INT yCell = m_y;
        INT hghtCell;
        for (UINT uRow = 0; uRow < m_cRows; uRow++) {
            hghtCell = m_bAlignRows ? m_hghtCellMax : m_ahghtCell.at(uRow);
            for (UINT uCol = 0; uCol < m_cCols; uCol++) {
                Rectangle(m_hDc, xCell - TABLE_PEN_WIDTH, yCell,
                    xCell + m_wdthCell, yCell + hghtCell + TABLE_PEN_WIDTH);

                RECT rectCell;
                rectCell.left = xCell + TABLE_PEN_WIDTH;
                rectCell.top = yCell + TABLE_PEN_WIDTH;
                rectCell.right = xCell + m_wdthCell - TABLE_PEN_WIDTH;
                rectCell.bottom = yCell + hghtCell - TABLE_PEN_WIDTH;
                if (m_vContent.size() > uRow && m_vContent[uRow].size() > uCol) {
                    DrawText(m_hDc, (LPCTSTR)m_vContent[uRow][uCol].c_str(), -1, &rectCell,
                        DT_CENTER | DT_WORDBREAK | DT_EDITCONTROL);
                }

                xCell += m_wdthCell;
            }
            xCell = m_x;
            yCell += hghtCell;
        }
    }

    SelectObject(m_hDc, hOldPn);
    DeleteObject(hPn);
    SelectObject(m_hDc, hOldBr);
    DeleteObject(hBr);
    SelectObject(m_hDc, hOldFont);
}

INT TextTable::GetHeight()
{
    return m_bAlignRows ? m_hghtTableMax : m_hghtTableActual;
}

VOID TextTable::CalculateMetrics()
{
    HFONT hOldFont = (HFONT)SelectObject(m_hDc, m_hFont);
    GetTextMetrics(m_hDc, &m_txtmtrc);
    SelectObject(m_hDc, hOldFont);

    m_cCols = TextTable::GetNumberOfColumns(m_vContent);
    m_cRows = m_vContent.size();

    if (!m_cCols && !m_cRows) {
        m_cCols = m_cRows = 1;
        m_wdthCell = m_wdthTable;
        m_hghtTableMax = m_hghtTableActual = m_hghtEmptyTable;
    }
    else {
        m_wdthCell = m_wdthTable / m_cCols;
        m_ahghtCell = GetCellHeights(m_vContent);

        m_hghtCellMax = *std::max_element(m_ahghtCell.begin(), m_ahghtCell.end());
        m_hghtTableMax = m_hghtCellMax * m_ahghtCell.size();
        m_hghtTableActual = std::accumulate(m_ahghtCell.begin(), m_ahghtCell.end(), 0, std::plus<INT>());
    }
}

std::vector<INT> TextTable::GetCellHeights(TTableContent& content) {

    CONST INT wdthCellInChars = (INT)std::ceil((DOUBLE)m_wdthCell /
        (m_txtmtrc.tmAveCharWidth * CHAR_WIDTH_COEFF + m_txtmtrc.tmOverhang));
    std::vector<INT> vhghtCell;
    for (TTableRowContent& row : content) {
        vhghtCell.push_back(
            (m_txtmtrc.tmInternalLeading + m_txtmtrc.tmExternalLeading + m_txtmtrc.tmHeight) *
            (INT)std::ceil(((DOUBLE)GetMaxElementLength(row) / wdthCellInChars)));
    }
    return vhghtCell;
}

INT TextTable::GetNumberOfColumns(TTableContent& content)
{
    INT iCols = 0;
    for (TTableRowContent& row : content)
    {
        if ((INT)row.size() > iCols) {
            iCols = row.size();
        }
    }
    return iCols;
}

INT TextTable::GetMaxElementLength(TTableRowContent& row)
{
    INT iMaxLength = 0;
    for (std::wstring& sElement : row) {
        if (((INT)sElement.length() > iMaxLength)) {
            iMaxLength = sElement.length();
        }
    }
    return iMaxLength;
}