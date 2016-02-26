/*
* Вообще непонятный адовый кусок говна
*/

#include <time.h>
#include "textBox.h"
#include "../../input.h"
#include "../../render/primitives.h"

BEGIN_GUI_NAMESPACE

textBox::textBox()
{
    position = 0;
    flag = 6;
    multiline = 0;
    maxSize = 0;
}

bool textBox::update(vec2d cursor, uint key)
{
    button::update(cursor, key);
    uint inputKey = input::getSymbol();
    if(state&2 && inputKey)
    {
        switch(inputKey)
        {
        case 1: //двигать каретку влево
            if(position)
                position--;
            break;
        case 2: //двигать каретку вправо
            if(position < label.length())
                position++;
            break;
        case 5: //перенос строки
            //label += '\n';
            break;
        case 6: //удаление символа
            if(position)
            {
                position--;
                label.erase(position, 1);
            }
            break;
        default:
            if(inputKey > 6)
                if(!maxSize || maxSize >= label.size()-1)
                {
                    label.insert(position++, inputKey);
                }
            break;
        }
        inputKey = 0;
    }
    return 0;
}

bool textBox::update(uint key)
{
    return update(input::getCursor(), key);
}

void textBox::draw()
{
    button::draw();
    if(state&2)
    {
        render::primitives2d::lineColor = label.color;
        if(clock()%400 > 200)
        {
            vec2f lpos = pos+size/2-label.rect/2;
            printf("%f %f\n", label.rect.x, label.rect.y);
            vec2f cPos = label.getCaretPos(position);
            render::primitives2d::line(lpos+cPos,lpos+cPos+vec2f(0, label.scale*16));
        }
    }
}

END_GUI_NAMESPACE
