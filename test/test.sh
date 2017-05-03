#!/bin/bash

print_in_color() {
    printf "%b" \
        "$(tput setaf "$2" 2> /dev/null)" \
        "$1" \
        "$(tput sgr0 2> /dev/null)"
}

pgreen() {
    print_in_color "$1" 2
}

ppurple() {
    print_in_color "$1" 5
}

pblue() {
  print_in_color "$1" 12
}

pred() {
    print_in_color "$1" 1
}

pyellow() {
    print_in_color "$1" 3
}

pbold() {
  printf  "%b" \
          "$(tput bold "$2" 2> /dev/null)" \
          "$1" \
          "$(tput sgr0 2> /dev/null)"
}

pinfo() {
  pblue "\n==> "
  pbold "$1\n\n"
}

pquestion() {
  pyellow "   [?] $1"
}

perror() {
  pred "   [✖] [$1] $2\n"
}

psuccess() {
    pgreen "   [✔] $1\n"
}

pwarning() {
    pyellow "   [!] $1\n"
}

abort() {
  pbold "...Aborting\n" && exit 1
}

declare -r FILE="bible"
declare -r FILESIZE="$(ls -lah "$FILE".txt | awk '{ print $5 }')"

compressor() {
  local TYPE="$1"
  local FILENAME="$2"
  local FILENAMECOMPRESS="$2.$3"
  
  pinfo "Compressing with $3"
  if [ ! -f "$FILENAME" ]; then
    perror "File" "$FILENAME does not exist"
    return 1
  fi
  local filesizetest=""
  ./../build/bin/compressor "$TYPE" "$FILENAME" "$FILENAMECOMPRESS"
  filesizetest="$(ls -lah "$FILENAMECOMPRESS" | awk '{ print $5 }')"

  if [ ! -f "$FILENAMECOMPRESS" ]; then
    perror "Compression" "The compression was filed"
    return 1
  fi
  
  psuccess "Compression ok: Original filesize-> $FILESIZE ; Compressed filesize-> $filesizetest"
  return 0

}

main() {
  cd "$(dirname "${BASH_SOURCE[0]}")" || exit 1
  local filename="$FILE.txt"
  compressor "-c" "$filename" "char" || abort
  compressor "-s" "$filename" "short" || abort
  compressor "-i" "$filename" "int" || abort
  compressor "-l" "$filename" "long" || abort
}


main
