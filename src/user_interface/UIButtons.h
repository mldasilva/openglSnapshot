#pragma once
#include "../glmath.h"
#include <functional>


// button logic
class UIButtons
{   
    private:

        enum UIButtonState
        {
            normal, clicked, hovered, hidden
        };

        struct UIButton
        {
            int index;
            vec2 position;
            vec2 size;

            function<void()> onClick;
            function<void()> onEnter;
            function<void()> onLeave;

            UIButtonState state = UIButtonState::normal;
        };

        UIButton* focused = nullptr;
        vector<UIButton>buttons;
        
        bool mouseCollision(vec2 mouse, vec2 objPosition, vec2 objSize)
        {
            //AABB collision
            return (mouse.x > objPosition.x && mouse.x < (objSize.x + objPosition.x) 
                    && mouse.y > objPosition.y && mouse.y < (objSize.y + objPosition.y));
        }

    public:

        void add(vec2 position, vec2 size, function<void()> onClick, function<void()> onEnter, function<void()> onLeave)
        {
            int index = buttons.size();
            UIButton b{index, position, size, onClick, onEnter, onLeave};
            buttons.push_back(b);
        };

        void move(int index, vec2 position)
        {
            buttons[index].position = position;
        };

        void update(vec2 mouse, bool isMouseLeftDown)
        {
            if(focused != nullptr) // reduces looping, only 1 button can fire even if overlapping
            {
                if(focused->state != UIButtonState::hidden) // button is not disabled
                {
                    if(mouseCollision(mouse, focused->position, focused->size))
                    {
                        // fires once
                        if((focused->state != UIButtonState::clicked) && isMouseLeftDown)
                        {
                            focused->state = UIButtonState::clicked;
                            if(focused->onClick != nullptr) 
                            {
                                focused->onClick();
                            }
                            // cout << "click on focus" << endl;
                        }

                        // hovered
                        if((focused->state != UIButtonState::hovered) && (!isMouseLeftDown))
                        {
                            focused->state = UIButtonState::hovered;
                            if(focused->onEnter != nullptr) 
                            {
                                focused->onEnter();
                            }
                            // cout << "enter on focus" << endl;
                        }
                    }
                    else
                    {
                        // reset
                        if(focused->state != UIButtonState::normal)
                        {
                            focused->state = UIButtonState::normal;
                            if(focused->onLeave != nullptr) 
                            {
                                focused->onLeave();
                            }
                            // cout << "leaving on focus" << endl;
                            focused = nullptr;
                        }
                    }
                }
            }
            else
            {
                // update since we dont have a focused button
                for(UIButton& button : buttons)
                {
                    if(button.state != UIButtonState::hidden && mouseCollision(mouse, button.position, button.size)) // button is not disabled
                    {
                        // only allow 1 button to be interacted with at a time
                        focused = &button;
                    }
                }
            }
            
        };
};