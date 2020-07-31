#include "Fjord/GUI/Layout.h" 

#include "Fjord/GUI/GUIElement.h" 

namespace Fjord 
{

    void DefaultLayout::Apply(GUIElement* element) 
    {
        FJ_EASSERT(element); 

        int childWidth = element->GetWidth() - 2 * PaddingX_; 

        int totalChildHeight = 0; 

        for (unsigned i = 0; i < element->GetChildCount(); i++) 
        {
            Ref<GUIElement> child = element->GetChild(i); 
            child->SetSize(
                childWidth, 
                child->GetHeight()
            );

            child->SetPosition(
                PaddingX_, 
                totalChildHeight + PaddingY_
            );

            totalChildHeight += child->GetHeight() + PaddingY_; 
        }

        element->SetSize(
            element->GetWidth(), 
            totalChildHeight + PaddingY_
        );
    }

}