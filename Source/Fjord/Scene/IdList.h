#pragma once 

#include "Fjord/Common.h" 

namespace Fjord 
{

    template <class Id> 
    class IdList
    {
    public: 
        IdList() : CurrentId_(1) {} 
        ~IdList() = default;  

        Id GetId() 
        {
            if (Available_.size() > 0) 
            {
                Id id = Available_.back(); 
                Available_.pop_back(); 
                return id; 
            }
            else 
            {
                return CurrentId_++; 
            }
        } 

        void ReturnId(Id id) 
        {
            Available_.push_back(id); 
        }

    private: 
        Id CurrentId_; 
        Vector<Id> Available_; 
    };

}