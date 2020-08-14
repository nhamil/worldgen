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

        void Search(const Vector3& pos, unsigned k, Vector<DataType>& out) const
        {
            out.clear(); 

            FJ_EASSERT(Size() >= k); 

            if (k == 0) return; 

            SearchData searchData(k); 

            Vector<NodeDir> stack; 

            SearchKInternal(pos, GetNode(0), stack, searchData); 

            while (stack.size() > 0) 
            {
                NodeDir nd = stack.back(); 
                stack.pop_back(); 

                unsigned axis = nd.Node->Axis; 
                float d = nd.Node->Position[axis] - pos[axis]; 
                d *= d; // dist squared 

                if (searchData.Count != k || d < searchData.Elements[k-1].Dist2) 
                {
                    // check other side of tree 
                    // nd.Left was reversed in SearchInternal 
                    const Node* search = GetNode(nd.Left ? nd.Node->Left : nd.Node->Right); 
                    SearchKInternal(pos, search, stack, searchData); 
                }
            }
            for (unsigned i = 0; i < k; i++) 
            {
                out.push_back(searchData.Elements[i].Elem->Data); 
            }
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

        struct SearchData 
        {
            struct Element 
            {
                const Node* Elem; 
                float Dist2; 
            };

            unsigned N; 
            unsigned Count = 0; 
            Vector<Element> Elements; 

            SearchData(unsigned n) : N(n) {} 

            bool Insert(const Node* elem, float dist2) 
            {
                for (unsigned i = 0; i < N; i++) 
                {
                    if (i == Count || dist2 < Elements[i].Dist2) 
                    {
                        if (Count == N) 
                        {
                            Elements.pop_back(); 
                        }
                        else 
                        {
                            Count++; 
                        }
                        Elements.insert(Elements.begin() + i, Element{elem, dist2}); 
                        return true; 
                    }
                }

                return false; 
            }
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

        void SearchKInternal(const Vector3& pos, const Node* node, Vector<NodeDir>& stack, SearchData& out) const 
        {
            while (node) 
            {
                unsigned axis = node->Axis; 
                bool left = node->Position[axis] < pos[axis]; 

                float dist2 = Length2(node->Position - pos); 
                out.Insert(node, dist2); 

                stack.push_back(NodeDir(node, !left)); 
                node = GetNode(left ? node->Left : node->Right); 
            }
        }

        Vector<Node> Nodes_; 
    }; 

}