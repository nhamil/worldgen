#pragma once 

#include "Fjord/Common.h" 
#include "Fjord/Math/Vector3.h" 

namespace Fjord 
{

    template <class DataType> 
    class KDTree3 
    {
    public: 
        KDTree3() = default; 
        ~KDTree3() = default; 

        unsigned Size() const 
        {
            return Nodes_.size(); 
        }

        void Clear() 
        {
            Nodes_.clear(); 
        }

        void Insert(const Vector3& pos, const DataType& data) 
        {
            if (Size() > 0) 
            {
                Index curIndex = 0; 
                Node* cur = GetNode(0); 

                // will always find a child node 
                // or place new data and return 
                while (true) 
                {
                    unsigned axis = cur->Axis; 
                    bool left = cur->Position[axis] < pos[axis]; 
                    Index childIndex = left ? cur->Left : cur->Right; 
                    if (childIndex != -1) 
                    {
                        curIndex = childIndex; 
                        cur = GetNode(childIndex); 
                    }
                    else 
                    {
                        childIndex = NewNode(pos, data, (axis + 1) % 3); 
                        cur = GetNode(curIndex); 
                        if (left) 
                        {
                            cur->Left = childIndex; 
                        }
                        else 
                        {
                            cur->Right = childIndex; 
                        }
                        return; 
                    }
                }
            }
            else 
            {
                // create root node 
                NewNode(pos, data, 0);
            }
        }

        const DataType& Search(const Vector3& pos) const
        {
            FJ_EASSERT(Size() > 0); 

            Vector<NodeDir> stack; 

            const Node* best = nullptr; 
            float bestDist2 = INFINITY; 

            SearchInternal(pos, GetNode(0), stack, best, bestDist2); 
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
                    const Node* search = GetNode(nd.Left ? nd.Node->Left : nd.Node->Right); 
                    SearchInternal(pos, search, stack, best, bestDist2); 
                }
            }

            return best->Data; 
        } 

    private: 
        using Index = int32; 

        struct Node 
        {
            Index Left = -1; 
            Index Right = -1; 
            Vector3 Position; 
            DataType Data; 
            unsigned Axis; 

            Node(const Vector3& pos, const DataType& data, unsigned axis) 
                : Position(pos) 
                , Data(data)
                , Axis(axis) {}

            Node() = default; 

            ~Node() {}
        };

        Index NewNode(const Vector3& pos, const DataType& data, unsigned axis) 
        {
            Nodes_.push_back(Node(pos, data, axis)); 
            return (Index) Nodes_.size() - 1;  
        }

        struct NodeDir 
        {
            const struct Node* Node = nullptr; 
            bool Left = false; 

            NodeDir(const struct Node* node, bool left) 
                : Node(node) 
                , Left(left) {} 
        };

        /** 
         * Must re-call every time new node is created 
         */ 
        Node* GetNode(Index in) 
        {
            return in == -1 ? nullptr : &Nodes_[0] + in; 
        }

        const Node* GetNode(Index in) const 
        {
            return in == -1 ? nullptr : &Nodes_[0] + in; 
        }

        void SearchInternal(const Vector3& pos, const Node* node, Vector<NodeDir>& stack, const Node*& best, float& bestDist2) const 
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

                stack.push_back(NodeDir(node, !left)); 
                node = GetNode(left ? node->Left : node->Right); 
            }
        }

        Vector<Node> Nodes_; 
    }; 

}