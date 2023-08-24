#include "primer/trie.h"
#include <string_view>
#include "common/exception.h"

namespace bustub {

template <class T>
auto Trie::Get(std::string_view key) const -> const T * {
  //throw NotImplementedException("Trie::Get is not implemented.");

  // You should walk through the trie to find the node corresponding to the key. If the node doesn't exist, return
  // nullptr. After you find the node, you should use `dynamic_cast` to cast it to `const TrieNodeWithValue<T> *`. If
  // dynamic_cast returns `nullptr`, it means the type of the value is mismatched, and you should return nullptr.
  // Otherwise, return the value.
  //std::cout<<"get key "<<key<<";"<<std::endl;
  if(root_)
  {
    //std::cout<<"not nullptr"<<std::endl;
    auto curnode = root_;
    for(size_t i = 0; i < key.size(); i++)
    {
      bool findmatch = false;
      for(auto node:curnode->children_)
      {
        //std::cout<<"idx "<<i<<", current searched char "<<node.first<<";"<<std::endl;
        if(node.first == key[i])
        {
          if(i == key.size() - 1 && node.second->is_value_node_)
          {
            //std::cout<<"end node find"<<std::endl;
            //auto debug_pointer = std::static_pointer_cast<const TrieNodeWithValue<T>>(node.second);
            //std::cout<<sizeof(*node.second)<<std::endl;
            //std::cout<<sizeof(*debug_pointer)<<std::endl;
            //std::cout<<"successfully cast"<<std::endl;
            auto nodeptr = std::dynamic_pointer_cast<const TrieNodeWithValue<T>>(node.second);
            if(nodeptr) return nodeptr->value_.get();
            else return nullptr;
          }
          curnode = node.second;
          findmatch = true;
          break;
        }
      }
      if(!findmatch) return nullptr;
    }
  }else return nullptr;
  //std::cout<<root_->is_value_node_<<std::endl;
  if(key.empty() && root_->is_value_node_)
  {
    auto nodeptr = std::dynamic_pointer_cast<const TrieNodeWithValue<T>>(root_);
    //std::cout<<"cast successfully"<<std::endl;
    if(nodeptr) return nodeptr->value_.get();
    else return nullptr;
  }
  else return nullptr;
}

template <class T>
auto Trie::Put(std::string_view key, T value) const -> Trie {
  // Note that `T` might be a non-copyable type. Always use `std::move` when creating `shared_ptr` on that value.
  //throw NotImplementedException("Trie::Put is not implemented.");

  // You should walk through the trie and create new nodes if necessary. If the node corresponding to the key already
  // exists, you should create a new `TrieNodeWithValue`.
  //std::cout<<"put key "<<key<<";"<<std::endl;
  if(key.empty())
  {
    if(root_)
    {
      auto new_root = std::static_pointer_cast<const TrieNode>(
        std::make_shared<const TrieNodeWithValue<T>>(
          TrieNodeWithValue<T>(root_->children_, std::make_shared<T>(std::move(value)))));
      return Trie(new_root);
    }
    else
    {
      auto new_root = std::static_pointer_cast<const TrieNode>(
        std::make_shared<const TrieNodeWithValue<T>>(
          TrieNodeWithValue<T>(decltype(TrieNode::children_)(), std::make_shared<T>(std::move(value)))));
      return Trie(new_root);
    }
  }
  else if(root_)
  {
    //std::cout<<"Not nullptr with key "<<key<<std::endl;
    auto curnode = root_;
    auto nextnode = curnode;
    std::vector<std::shared_ptr<const TrieNode>> changedNode(key.size());
    bool allmatch = true;
    int len = 0;
    for(; len < (int)key.size(); len++)
    {
      bool findmatch = false;
      for(auto node:curnode->children_)
      {
        //std::cout<<len<<std::endl;
        if(node.first == key[len])
        {
          findmatch = true;
          changedNode[len] = node.second;
          nextnode = node.second;
        }else{};
      }
      allmatch &= findmatch;
      if(findmatch) curnode = nextnode;
      else break;
    }
    //std::cout<<"out"<<std::endl;
    if(allmatch)
    {
      //std::cout<<"all match"<<std::endl;
      curnode = std::static_pointer_cast<const TrieNode>(std::make_shared<const TrieNodeWithValue<T>>(
                TrieNodeWithValue<T>(curnode->children_, std::make_shared<T>(std::move(value)))));
      for(int i = len - 1; i > 0; i--)
      {
        //std::cout<<"process idx: "<<i<<std::endl;
        auto fathernode = changedNode[i-1]->Clone();
        fathernode->children_[key[i]] = curnode;
        curnode = //std::make_shared<const TrieNode>(std::move(fathernode->children_));
                  std::move(fathernode);
      }
      auto new_root = root_->Clone();
      new_root->children_[key[0]] = curnode;
      curnode = //std::make_shared<const TrieNode>(std::move(new_root->children_));
                std::move(new_root);
      return Trie(curnode);
    }
    else
    {
      //std::cout<<"not all match"<<std::endl;
      auto endNode = std::static_pointer_cast<const TrieNode>(std::make_shared<const TrieNodeWithValue<T>>(
                    TrieNodeWithValue<T>(decltype(TrieNode::children_)(), std::make_shared<T>(std::move(value)))
                    ));
      for(int i = (int)key.size() - 1; i > len; i--)
      {
        //::cout<<"added idx "<<i<<std::endl;
        std::map<char, std::shared_ptr<const TrieNode>> fatherChild = {};
        fatherChild[key[i]] = endNode;
        endNode = std::make_shared<const TrieNode>(TrieNode(fatherChild));
      }
      for(int i = len - 1; i >= 0; i--)
      {
        //std::cout<<"replaced idx "<<i+1<<std::endl;
        auto fathernode = changedNode[i]->Clone();
        fathernode->children_[key[i+1]] = endNode;
        endNode = //std::make_shared<const TrieNode>(std::move(fathernode->children_));
                  std::move(fathernode);
      }
      //std::cout<<"process root"<<std::endl;
      auto new_root = root_->Clone();
      new_root->children_[key[0]] = endNode;
      endNode = std::make_shared<const TrieNode>(std::move(new_root->children_));
      return Trie(endNode);
    }
  }
  else
  {
    //std::cout<<"nullptr with key"<<key<<std::endl;
    auto endNode = std::static_pointer_cast<const TrieNode>(std::make_shared<const TrieNodeWithValue<T>>(
                  TrieNodeWithValue<T>(decltype(TrieNode::children_)(),std::make_shared<T>(std::move(value))
                  )));
    //std::cout<<std::static_pointer_cast<const TrieNodeWithValue<T>>(endNode)<<std::endl;
    for(int i = (int)key.size() - 1; i >= 0;i--)
    {
      //std::cout<<"current idx "<<i<<std::endl;
      std::map<char, std::shared_ptr<const TrieNode>> fatherChild = {};
      fatherChild[key[i]] = endNode;
      endNode = std::make_shared<const TrieNode>(TrieNode(fatherChild));
    }
    return Trie(endNode);
  }
}

auto Trie::Remove(std::string_view key) const -> Trie {
  //throw NotImplementedException("Trie::Remove is not implemented.");

  // You should walk through the trie and remove nodes if necessary. If the node doesn't contain a value any more,
  // you should convert it to `TrieNode`. If a node doesn't have children any more, you should remove it.
  //std::cout<<"remove key "<<key<<";"<<std::endl;
  if(key.empty())
  {
    if(root_ && root_->is_value_node_)
    {
      auto new_root = std::make_shared<const TrieNode>(
        TrieNode(root_->children_)
      );
      if(new_root->children_.empty()) return Trie(nullptr);
      else return Trie(new_root);
    }
    else return Trie(nullptr);
  }
  //std::cout<<"key not empty"<<std::endl;
  if(root_)
  {
    auto curnode = root_;
    std::vector<std::shared_ptr<const TrieNode>> changedNode(key.size());
    int len = 0;
    for(; len < (int)key.size(); len++)
    {
      bool findkey = false;
      for(auto node:curnode->children_)
      {
        if(node.first == key[len])
        {
          findkey = true;
          changedNode[len] = node.second;
          curnode = node.second;
          break;
        }
      }
      if(!findkey) return Trie(root_);
      else continue;
    }
    if(curnode->is_value_node_)
    {
      //std::cout<<"find remove node"<<std::endl;
      curnode = std::make_shared<const TrieNode>(TrieNode(curnode->children_));
      for(int i = len - 1; i > 0; i--)
      {
        auto fathernode = changedNode[i-1]->Clone();
        if(curnode->children_.empty() && !curnode->is_value_node_)
        {
          fathernode->children_.erase(key[i]);
        }
        else
        {
          fathernode->children_[key[i]] = curnode;
        }
        curnode = //std::make_shared<const TrieNode>(std::move(fathernode));
                  std::move(fathernode);
      }
      auto new_root = root_->Clone();
      if(curnode->children_.empty() && !curnode->is_value_node_)
      {
        new_root->children_.erase(key[0]);
      }
      else
      {
        new_root->children_[key[0]] = curnode;
      }
      curnode = //std::make_shared<const TrieNode>(std::move(new_root->is_value_node_));
                std::move(new_root);
      if(curnode->children_.empty()) return Trie(nullptr);
      else return Trie(curnode);
    }
    else return Trie(root_);
  }
  else return Trie(nullptr);
}

// Below are explicit instantiation of template functions.
//
// Generally people would write the implementation of template classes and functions in the header file. However, we
// separate the implementation into a .cpp file to make things clearer. In order to make the compiler know the
// implementation of the template functions, we need to explicitly instantiate them here, so that they can be picked up
// by the linker.

template auto Trie::Put(std::string_view key, uint32_t value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const uint32_t *;

template auto Trie::Put(std::string_view key, uint64_t value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const uint64_t *;

template auto Trie::Put(std::string_view key, std::string value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const std::string *;

// If your solution cannot compile for non-copy tests, you can remove the below lines to get partial score.

using Integer = std::unique_ptr<uint32_t>;

template auto Trie::Put(std::string_view key, Integer value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const Integer *;

template auto Trie::Put(std::string_view key, MoveBlocked value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const MoveBlocked *;

}  // namespace bustub
