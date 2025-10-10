#include "CurveEditor.h"


void CurveEditor::EditGUI(){
    ImFunc::CustomBegin("Curve Editor", MoveOnly_TitleBar);
    {
        ImGui::Indent();

        // カーブエディタの表示
        //ImCurveEdit::CurveBezier

        ImGui::Unindent();
    }
}