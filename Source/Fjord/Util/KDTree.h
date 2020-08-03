#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/Math/Vector3.h" 

namespace Fjord 
{

    template <class DataType> 
    class KDTree3 
    {
    public: 
        KDTree3() {}
        KDTree3(const KDTree3<DataType>& other) 
        {
            Size_ = other.Size_; 
            RootNode_ = other.RootNode_; 
            if (RootNode_) RootNode_ = RootNode_->Copy(); 
        } 
        KDTree3<DataType>& operator=(const KDTree3<DataType>& other) 
        {
            if (&other == this) return *this; 

            Clear(); 
            Size_ = other.Size_; 
            RootNode_ = other.RootNode_; 
            if (RootNode_) RootNode_ = RootNode_->Copy(); 
            return *this; 
        } 

        ~KDTree3() 
        {
            Clear(); 
        } 

        unsigned Size() const 
        {
            return Size_; 
        }

        void Clear() 
        {
            Size_ = 0; 
            if (RootNode_) 
            {
                delete RootNode_; 
                RootNode_ = nullptr; 
            }
        }

        void Insert(const Vector3& pos, const DataType& data) 
        {
            Size_++; 
            if (RootNode_) 
            {
                Node* cur = RootNode_; 

                // will always find a child node 
                // or place new data and return 
                while (true) 
                {
                    unsigned axis = cur->Axis; 
                    bool left = cur->Position[axis] < pos[axis]; 
                    Node* child = left ? cur->Left : cur->Right; 
                    if (child) 
                    {
                        cur = child; 
                    }
                    else 
                    {
                        child = new Node(pos, data, (axis + 1) % 3); 
                        if (left) 
                        {
                            cur->Left = child; 
                        }
                        else 
                        {
                            cur->Right = child; 
                        }
                        return; 
                    }
                }
            }
            else 
            {
                RootNode_ = new Node(pos, data, 0); 
            }
        }

        const DataType& Search(const Vector3& pos) const 
        {
            FJ_EASSERT(RootNode_); 

            Vector<NodeDir> stack; 

            Node* best = nullptr; 
            float bestDist2 = INFINITY; 

            SearchInternal(pos, RootNode_, stack, best, bestDist2); 
            while (stack.size() > 0) 
            {
                NodeDir nd = stack.back(); 
                stack.pop_back(); 

                unsigned axis = nd.Node->Axis; 
                float d = nd.Node->Position[axis] - pos[axis]; 
                d *= d; // dist squared 

                if (d < bestDist2) 
                {
                    // check other side of tree 
                    // nd.Left was reversed in SearchInternal 
                    Node* search = nd.Left ? nd.Node->Left : nd.Node->Right; 
                    SearchInternal(pos, search, stack, best, bestDist2); 
                }
            }

            return best->Data; 
        } 

    private: 
        struct Node 
        {
            Vector3 Position; 
            DataType Data; 
            Node* Left = nullptr; 
            Node* Right = nullptr; 
            unsigned Axis; 

            Node(const Vector3& pos, const DataType& data, unsigned axis) 
                : Position(pos) 
                , Data(data)
                , Left(nullptr) 
                , Right(nullptr) 
                , Axis(axis) {} 

            ~Node() 
            {
                if (Left) delete Left; 
                if (Right) delete Right; 
            }

            Node* Copy() 
            {
                Node* n = new Node(Position, Data, Axis); 
                n->Left = Left ? Left->Copy() : nullptr; 
                n->Right = Right ? Right->Copy() : nullptr; 
                return n; 
            }
        };

        struct NodeDir 
        {
            struct Node* Node; 
            bool Left; 
        };

        void SearchInternal(const Vector3& pos, Node* node, Vector<NodeDir>& stack, Node*& best, float& bestDist2) const 
        {
            while (node) 
            {
                unsigned axis = node->Axis; 
                bool left = node->Position[axis] < pos[axis]; 

                float dist2 = Length2(node->Position - pos); 
                if (bestDist2 > dist2) 
                {
                    bestDist2 = dist2; 
                    best = node; 
                }

                stack.push_back({node, !left}); 
                node = left ? node->Left : node->Right; 
            }
        }

        Node* RootNode_ = nullptr; 
        unsigned Size_ = 0; 
    }; 

}