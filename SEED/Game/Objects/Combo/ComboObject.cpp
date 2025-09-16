#include "ComboObject.h"

ComboObject::ComboObject(){
    comboCount = 0;
    comboText.SetFont("DefaultAssets/M_PLUS_Rounded_1c/MPLUSRounded1c-Bold.ttf");
    comboText.text = "%d\nCOMBO";
    comboText.BindDatas({ comboCount });
    comboText.align = TextAlign::CENTER;
}

void ComboObject::Draw() const{

    comboText.Draw();
}
