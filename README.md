# HexEditor
Program which helps you to edit and view binary files.

#Dedicated language
HexEditor decodes binary files based on its own dedicated language.

It is design only for decoding files which makes it very useful and easy.

Example:
```
  pos=0                               // start decoding from begining
  check=uint4(pos)                    // load unsigned integer(4 bytes)
  header_size=uint4(pos)              // pos is updated each time by the size of loaded bytes
  chunks_num=uint4(pos)
  
  // define your own custom structure with code decoding it
  custom header_chunk(pos){
      chars_num=uint4(pos)
      name     =string(pos,chars_num) // load string (begining, characters number) 
      data_pos =uint4(pos)
      repeat   =uint4(pos)
      size     =uint4(pos)[repeat]    // load uint4 repeat times
      
      out data_pos,name,size          // show for an end user only interesting information
  }
  header=header_chunk(pos)[chunks_num]// use your structure to load some data
```

![Alt text](/screenshots/screenshot1.png?raw=true "Decoded header from binary file")
#Safe and fast
You can easily view and edit corrupted or unkown files.
![Alt text](/screenshots/screenshot2.png?raw=true "Decoded header from binary file")
##Internals
* C++11
* Qt4
* Own parser and interpreter for internal language
* Brief documentation for Doxygen
