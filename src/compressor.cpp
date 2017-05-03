#include <iostream>
#include <string>
#include <fstream>

#include <huffman.hpp>

inline void Help(const std::string &namefile){
  std::cout << "Usage: " << namefile << " [Compress Type] <input_file> <output_file> \n";
  std::cout << "COMPRESS TYPES:\n";
  std::cout << "\t-c: char\n";
  std::cout << "\t-s: short\n";
  std::cout << "\t-i: int\n";
  std::cout << "\t-l: long\n";
  std::cout << "EXAMPLE:\n";
  std::cout << '\t' << namefile <<" -c test.txt test.comp\n\n";
}

int main(int argc, const char* argv[]) {
  if (argc != 4) {
    Help(argv[0]);
    return 0;
  }
  
  std::string input_file = argv[2];
  std::string output_file = argv[3];
  
  std::ifstream input(input_file, std::ios::binary);
  std::ofstream output(output_file, std::ios::binary);
  
  if (!input.is_open()) {
    std::cout << "Error: The file \"" << input_file << "\" cannot be opened. \n please try again \n";
    return -1;
  }
  
  char type = argv[1][1];
  
  switch (type) {
    case 'c':
    {
      output.put(type);
      Huffman<char> huffchar;
      
      std::cout << "Compressing whit char... \n";
      
      huffchar.Compress(input, output);
      
      input.close();
      output.close();
      break;
    }
    case 's':
    {
      output.put(type);
      Huffman<short> huffshort;
      std::cout << "Compressing whit short... \n";
      
      huffshort.Compress(input, output);
      
      input.close();
      output.close();
      break;
    }
    case 'i':
    {
      output.put(type);
      Huffman<int> huffint;
      
      std::cout << "Compressing whit int... \n";
      
      huffint.Compress(input, output);
      
      input.close();
      output.close();
      break;
    }
    case 'l':
    {
      output.put(type);
      Huffman<std::int64_t> hufflong;
      
      std::cout << "Compressing whit long... \n";
      
      hufflong.Compress(input, output);
      
      input.close();
      output.close();
      break;
    }
      
    default:
      std::cout << "\nError: " <<'-' << type << " is not a compress type.\n\n";
      Help(argv[0]);
      break;
  }
  
  return 0;
  
  
}
