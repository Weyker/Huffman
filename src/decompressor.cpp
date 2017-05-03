#include <iostream>
#include <string>
#include <fstream>

#include <huffman.hpp>

inline void Help(const std::string &namefile){
  std::cout << "Usage: " << namefile << " <compress_file> <output_file> \n";
  std::cout << "EXAMPLE:\n";
  std::cout << '\t' << namefile <<" -c test.txt test.comp\n\n";
}

int main(int argc, const char* argv[]) {
  if (argc != 3) {
    Help(argv[0]);
    return 0;
  }
  
  std::string input_file = argv[1];
  std::string output_file = argv[2];
  
  std::ifstream input(input_file, std::ios::binary);
  std::ofstream output(output_file, std::ios::binary);
  
  if (!input.is_open()) {
    std::cout << "Error: The file \"" << input_file << "\" cannot be opened. \n please try again \n";
    return -1;
  }
  
  auto value = input.get();
  
  switch (value) {
    case 'c':
    {
      Huffman<char> huffchar;
      
      std::cout << "Decompressing whit char... \n";
      
      huffchar.Decompress(input, output);
      
      input.close();
      output.close();
      break;
    }
    case 's':
    {
      Huffman<short> huffshort;
      std::cout << "Decompressing whit short... \n";
      
      huffshort.Decompress(input, output);
      
      input.close();
      output.close();
      break;
    }
    case 'i':
    {
      Huffman<int> huffint;
      
      std::cout << "Decompressing whit int... \n";
      
      huffint.Decompress(input, output);
      
      input.close();
      output.close();
      break;
    }
    case 'l':
    {
      Huffman<std::int64_t> hufflong;
      
      std::cout << "Decompressing whit long... \n";
      
      hufflong.Decompress(input, output);
      
      input.close();
      output.close();
      break;
    }
  }
  
  return 0;
}
