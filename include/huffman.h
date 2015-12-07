//
//  huffman.h
//  Huffman Compressor
//
//  Created by Patricio Beckmann on 24-04-14.
//  Copyright (c) 2014 Patricio Beckmann. All rights reserved.
//

#ifndef HUFFMANCOMPRESSOR_HUFFMAN_H
#define HUFFMANCOMPRESSOR_HUFFMAN_H

#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <queue>
#include <vector>
#include <cstdint>
#include <functional>

namespace encode {
  struct Node {
    Node(unsigned freq, Node* p) : frequency{freq}, parent{p} {}
    unsigned frequency;
    Node* parent;
    
    unsigned depth() const noexcept {
      
      unsigned count = 0;
      Node* it = parent;
      
      while (it) {
        ++count;
        it = it->parent;
      }
      return count;
    }
  };
  
  struct NodeCmp {
    bool operator()(Node* lhs, Node* rhs) const noexcept {
      return lhs->frequency > rhs->frequency;
    }
  };
}

namespace decode {
  template <typename T>
  struct Node {
    Node* lhs;
    Node* rhs;
    T symbol;
    const bool IsLeaf() { return !(lhs || rhs); }
  };
}

template <typename T>
class Huffman {
 private:
  
  using codeword_t = std::vector<bool>;
  using code_t = std::uintmax_t;
  using filesize_t = std::uintmax_t;
  using encodetree_t = std::vector<encode::Node>;
  using decodetree_t = std::vector<decode::Node<T>>;
  using codewordlength_t = std::vector<std::pair<T, code_t> > ;
  using frequencytable_t = std::unordered_map<T, std::size_t>;
  using codebook_t = std::unordered_map<T, codeword_t>;
  
  
  codebook_t CodeBook_;
  decodetree_t DecodeTree_;
  filesize_t Amount_;
  std::uintmax_t init_pos;
  

  void IncrementCodeWord(codeword_t& code) {
    auto it = std::find(code.rbegin(), code.rend(), 0);
    if (it != code.rend()) {
      *it = 1;
      std::fill(code.rbegin(), it, 0);
    } else {
      std::fill(code.begin(), code.end(), 0);
    }
  }
  
  void SetAmount(std::istream &is){
    is.seekg(0, is.end);
    Amount_ = is.tellg();
    is.seekg(0, is.beg);
  }

  frequencytable_t GenerateFrequencyTable(std::istream& is) {
    frequencytable_t table;

    ReadSymbolStream(is, [this, &table](T c) {
      ++table[c]; });
    
    return table;
  }

  encodetree_t GenerateEncodeTree(frequencytable_t table) {
    
    encodetree_t tree;

    tree.reserve(table.size() * 2);
    std::priority_queue<void, std::vector<encode::Node*>, encode::NodeCmp> pq;

    for (const auto& pair : table) {
      tree.emplace_back(pair.second, nullptr);
      pq.push(&tree.back());
    }

    while (pq.size() > 1) {
      encode::Node* right = pq.top();
      pq.pop();
      encode::Node* left = pq.top();
      pq.pop();

      tree.emplace_back(left->frequency + right->frequency, nullptr);
      left->parent = &tree.back();
      right->parent = &tree.back();
      pq.push(&tree.back());
    }
    
    return tree;
  }
  
  void GenerateDecodeTree() {
    
    DecodeTree_.reserve(CodeBook_.size() * 2);
    DecodeTree_.emplace_back();
    
    for (auto&& codeword : CodeBook_) {
      decode::Node<T>* node = &DecodeTree_.front();
      for (bool bit : codeword.second) {
        if (bit && !node->rhs) {
          DecodeTree_.emplace_back();
          node->rhs = &DecodeTree_.back();
        } else if (!(bit || node->lhs)) {
          DecodeTree_.emplace_back();
          node->lhs = &DecodeTree_.back();
        }
        node = (bit ? node->rhs : node->lhs);
      }
      node->symbol = codeword.first;
    }
  }
  
  codewordlength_t GenerateCodeWordLength(encodetree_t &tree, const frequencytable_t table){
    codewordlength_t codelength;
    
    auto tree_it = tree.begin();
    
    for (const auto& pair : table) {
      codelength.emplace_back(pair.first, (*tree_it).depth());
      ++tree_it;
    }
    
    std::sort(codelength.begin(),
              codelength.end(),
              [](std::pair<T, std::size_t> lhs, std::pair<T, std::size_t> rhs) {
                return lhs.second < rhs.second ||
                (lhs.second == rhs.second && lhs.first < rhs.first);
              });
    
    return codelength;
  }

  void GenerateCodeBook(codewordlength_t &codelength) {
    codeword_t code;
    for (const auto& pair : codelength) {
      std::size_t curlength = pair.second;
      while (code.size() != curlength)
        code.push_back(0);
      CodeBook_[pair.first] = code;
      IncrementCodeWord(code);
    }
  }
  
  void ReadSymbolStream(std::istream& is, std::function<void(T)> func) {
    
    T symbol;
    for (std::size_t i = 0; i < Amount_; i += sizeof(T)) {
      is.read(reinterpret_cast<char*>(&symbol), sizeof(T));
      func(symbol);
    }
    
    is.seekg(0, is.beg);
  }

  void Encode(std::istream& is, std::ostream& os) {
    char buffer = 0;
    std::size_t bit_count = 0;
    
    ReadSymbolStream(is, [this, &os, &buffer, &bit_count](T c) {
      for (bool bit : CodeBook_.at(c)) {

        if (bit_count < CHAR_BIT) {
          buffer |= bit << bit_count;
          ++bit_count;

        } else {
          WriteBuffer(os, buffer, bit_count);
          buffer |= bit << bit_count;
          ++bit_count;
        }
      }
    });
    if (bit_count > 0) {
      WriteBuffer(os, buffer, bit_count);
    }
  }
  
  void WriteBuffer(std::ostream& os, char& buffer, std::size_t& bit_count) {
    os.write(reinterpret_cast<char*>(&buffer), sizeof(char));
    buffer = 0;
    bit_count = 0;
  }
  
  void WriteHeader(std::ostream& os, codewordlength_t &codelength){
    auto length = codelength.size();
    os.write(reinterpret_cast<char*>(&length), sizeof(length));
    for (auto & pair : codelength){
      os.write(reinterpret_cast<char*>(&pair.first), sizeof(T));
      os.write(reinterpret_cast<char*>(&pair.second), sizeof(code_t));
    }
    os.write(reinterpret_cast<char*>(&Amount_), sizeof(Amount_));
  }
  
  codewordlength_t ReadHeader(std::istream& is){
    std::size_t length;
    is.read(reinterpret_cast<char*>(&length), sizeof(std::size_t));
    codewordlength_t codewordlengths;
    T symbol;
    code_t codelength;
    for (std::size_t i = 0; i < length; ++i) {
      is.read(reinterpret_cast<char*>(&symbol), sizeof(T));
      is.read(reinterpret_cast<char*>(&codelength), sizeof(code_t));
      codewordlengths.emplace_back(symbol, codelength);
    }
    is.read(reinterpret_cast<char*>(&Amount_), sizeof(Amount_));
    
    return codewordlengths;
  }

  bool ReadBit(char buffer, std::size_t pos) { return buffer & (1 << pos); }

  void Decode(std::istream& is, std::ostream& os) {
    char buffer;
    decode::Node<T>* aux = &DecodeTree_.front();
    while (Amount_){
      is.read(reinterpret_cast<char*>(&buffer), sizeof(char));
      for (size_t pos = 0; pos < CHAR_BIT; ++pos) {
        if (aux->IsLeaf() && Amount_) {
          os.write(reinterpret_cast<char*>(&aux->symbol), sizeof(T));
          Amount_ -= sizeof(T);
          if(Amount_ <= 0) break;
          aux = &DecodeTree_.front();
        }
        aux = (ReadBit(buffer, pos) ? aux->rhs : aux->lhs);
      }
    }
  }

 public:
  
  void imprimir(frequencytable_t table){
    using std::cout;
    using std::endl;
    
    cout << "\n\tsymbols \t|\t frequency" << endl;
    for(const auto & pair : table){
      cout << sizeof(pair.first);
      cout << '\t' << pair.first << " \t|\t " << pair.second << endl;
    }
    cout << endl << endl;
    
  }
  
  void imprimir(codebook_t table){
    using std::cout;
    using std::endl;
    
    cout << "\n\tsymbols \t|\t frequency" << endl;
    for(const auto & pair : table){
      cout << '\t' << pair.first << " \t|\t ";
      for(const bool & bit : pair.second){
        cout << bit;
      }
      cout << endl;
    }
    cout << endl << endl;
    
  }

  void Compress(std::istream& is, std::ostream& os) {
    std::cout << "Seting Ammount of characteres... ";
    SetAmount(is);
    std::cout << "\t Done. file with " << Amount_ << " characteres\n";
    
    if(Amount_ % sizeof(T) != 0)
      throw std::length_error("the file length has to be multiple");
    
    std::cout << "creating frecuency table... ";
    frequencytable_t ftable = GenerateFrequencyTable(is);
    std::cout << "\t Done\n";
    
    std::cout << "Generating Tree... ";
    encodetree_t tree = GenerateEncodeTree(ftable);
    std::cout << "\t Done\n";
    
    std::cout << "Generating Codeword Lengths... ";
    codewordlength_t codelength = GenerateCodeWordLength(tree, ftable);
    std::cout << "\t Done\n";
    
    std::cout << "Writing Header on File... ";
    WriteHeader(os, codelength);
    std::cout << "\t Done\n";
    
    std::cout << "Generating CodeBook... ";
    GenerateCodeBook(codelength);
    std::cout << "\t Done\n";
    
    std::cout << "Encoding... ";
    Encode(is, os);
    std::cout << "\t Done\n";
  }

  void Decompress(std::istream& is, std::ostream& os) {
    
    std::cout << "Reading Header... ";
    codewordlength_t codewordlength = ReadHeader(is);
    std::cout << "\t Done. file with " << Amount_ << " characteres\n";
    
    std::cout << "Generating CodeBook... ";
    GenerateCodeBook(codewordlength);
    std::cout << "\t Done\n";
    
    std::cout << "Creating Tree... ";
    GenerateDecodeTree();
    std::cout << "\t Done\n";
    
    std::cout << "Decoding...";
    Decode(is, os);
    std::cout << "\t Done\n";
  }
};

#endif /* defined(HUFFMANCOMPRESSOR_HUFFMAN_H) */
